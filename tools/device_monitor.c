#define _POSIX_C_SOURCE 200809L
#include "frame_protocol.h"
#include "linux_device_port.h"

#include <errno.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef enum { INITIALIZED, OPENING, RUNNING, TIMEOUT, DISCONNECTED, PROTOCOL_ERROR, FAILED, STOPPED } state_t;
static const char *state_name(state_t state)
{
    static const char *names[] = {"INITIALIZED", "OPENING", "RUNNING", "TIMEOUT", "DISCONNECTED", "PROTOCOL_ERROR", "FAILED", "STOPPED"};
    return (unsigned)state < sizeof(names)/sizeof(names[0]) ? names[state] : "INVALID";
}
static void set_state(state_t *state, state_t next)
{
    *state = next;
    printf("STATE %s\n", state_name(next));
    fflush(stdout);
}

static bool parse_count(const char *s, unsigned *value)
{
    if(s == NULL || *s == '\0' || *s == '-') return false;
    char *end;
    unsigned long parsed = strtoul(s, &end, 10);
    if(*end || parsed > 1000) return false;
    *value = (unsigned)parsed;
    return true;
}

int main(int argc, char **argv)
{
    const char *path = NULL;
    unsigned expect_data = 2, expect_invalid = 0;
    bool expect_disconnect = false, expect_timeout = false;
    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i], "--device") == 0 && ++i < argc) path = argv[i];
        else if(strcmp(argv[i], "--expect-data") == 0 && ++i < argc && parse_count(argv[i], &expect_data)) {}
        else if(strcmp(argv[i], "--expect-invalid") == 0 && ++i < argc && parse_count(argv[i], &expect_invalid)) {}
        else if(strcmp(argv[i], "--expect-disconnect") == 0) expect_disconnect = true;
        else if(strcmp(argv[i], "--expect-timeout") == 0) expect_timeout = true;
        else { fprintf(stderr, "invalid arguments\n"); return 2; }
    }
    if(path == NULL || (expect_disconnect && expect_timeout)) { fprintf(stderr, "device and exclusive mode required\n"); return 2; }
    state_t state = INITIALIZED;
    printf("STATE %s\n", state_name(state));
    set_state(&state, OPENING);
    int fd = linux_port_open(path);
    if(fd < 0) { perror("open device"); set_state(&state, FAILED); return 1; }
    printf("EVENT OPEN path=%s fd=%d nonblock=1\n", path, fd);
    set_state(&state, RUNNING);
    frame_parser_t parser;
    device_model_t model;
    frame_parser_reset(&parser);
    device_model_reset(&model);
    bool command_sent = false, ack_seen = false, success = false;
    int64_t deadline = linux_port_now_ms() + (expect_timeout ? 700 : 5000);
    while(linux_port_now_ms() < deadline) {
        struct pollfd pfd = {.fd=fd, .events=POLLIN};
        int64_t remaining = deadline - linux_port_now_ms();
        int ready = poll(&pfd, 1, remaining > 200 ? 200 : (int)remaining);
        if(ready < 0) { if(errno == EINTR) continue; perror("poll"); set_state(&state, FAILED); break; }
        if(ready == 0) continue;
        if(pfd.revents & POLLNVAL) { set_state(&state, FAILED); break; }
        if(pfd.revents & POLLIN) {
            uint8_t buffer[64];
            ssize_t n = read(fd, buffer, sizeof(buffer));
            if(n < 0 && errno == EINTR) continue;
            if(n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) continue;
            if(n < 0 && errno != EIO) { perror("read"); set_state(&state, FAILED); break; }
            if(n <= 0) { set_state(&state, DISCONNECTED); success = expect_disconnect && model.accepted >= expect_data; break; }
            printf("EVENT READ bytes=%ld\n", (long)n);
            for(ssize_t i = 0; i < n; i++) {
                frame_t frame;
                uint32_t before = parser.rejected;
                if(frame_parser_push(&parser, buffer[i], &frame)) {
                    if(frame.type == FRAME_DATA) {
                        if(device_model_accept(&model, &frame)) {
                            printf("EVENT DATA seq=%u value=%u\n", frame.sequence, model.last_value);
                            if(!command_sent && !expect_disconnect && !expect_timeout) {
                                frame_t command = {.type=FRAME_COMMAND, .sequence=1, .length=4, .payload={'P','I','N','G'}};
                                uint8_t encoded[FRAME_SIZE_MAX]; size_t length;
                                if(!frame_encode(&command, encoded, sizeof(encoded), &length) ||
                                   linux_port_write_all(fd, encoded, length, 500) != 0) {
                                    perror("command write"); set_state(&state, FAILED); goto done;
                                }
                                command_sent = true;
                                puts("EVENT COMMAND_SENT seq=1 payload=PING");
                            }
                        } else { puts("EVENT REJECT sequence"); set_state(&state, PROTOCOL_ERROR); set_state(&state, RUNNING); }
                    } else if(frame.type == FRAME_ACK && frame.sequence == 1 && frame.length == 4 &&
                              memcmp(frame.payload, "PONG", 4) == 0) { ack_seen = true; puts("EVENT ACK seq=1 payload=PONG"); }
                    else { puts("EVENT REJECT unexpected_frame"); set_state(&state, PROTOCOL_ERROR); set_state(&state, RUNNING); }
                }
                if(parser.rejected != before) { printf("EVENT REJECT parser_total=%u\n", parser.rejected); set_state(&state, PROTOCOL_ERROR); set_state(&state, RUNNING); }
            }
            fflush(stdout);
            if(!expect_disconnect && !expect_timeout && model.accepted >= expect_data && ack_seen &&
               parser.rejected + model.rejected_sequence >= expect_invalid) { success = true; break; }
        }
        if(pfd.revents & (POLLERR | POLLHUP)) { set_state(&state, DISCONNECTED); success = expect_disconnect && model.accepted >= expect_data; break; }
    }
    if(!success && expect_timeout && state == RUNNING && model.accepted == 0) { set_state(&state, TIMEOUT); success = true; }
    else if(!success && state == RUNNING) set_state(&state, FAILED);
done:
    close(fd);
    puts("EVENT CLOSE fd_released=1");
    set_state(&state, STOPPED);
    printf("SUMMARY accepted=%u parser_rejected=%u sequence_rejected=%u ack=%d result=%s\n",
           model.accepted, parser.rejected, model.rejected_sequence, ack_seen, success ? "PASS" : "FAIL");
    return success ? 0 : 1;
}
