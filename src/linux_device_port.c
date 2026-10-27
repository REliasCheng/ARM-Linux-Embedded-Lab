#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#include "linux_device_port.h"

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

int64_t linux_port_now_ms(void)
{
    struct timespec ts;
    if(clock_gettime(CLOCK_MONOTONIC, &ts) != 0) return -1;
    return (int64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

int linux_port_open(const char *path)
{
    if(path == NULL) { errno = EINVAL; return -1; }
    int fd = open(path, O_RDWR | O_NOCTTY | O_NONBLOCK | O_CLOEXEC);
    if(fd < 0) return -1;
    struct termios config;
    if(tcgetattr(fd, &config) != 0) { int saved = errno; close(fd); errno = saved; return -1; }
    cfmakeraw(&config);
    config.c_cflag |= CLOCAL | CREAD;
    config.c_cc[VMIN] = 0;
    config.c_cc[VTIME] = 0;
    if(tcsetattr(fd, TCSANOW, &config) != 0) { int saved = errno; close(fd); errno = saved; return -1; }
    return fd;
}

int linux_port_write_all(int fd, const uint8_t *data, size_t length, int timeout_ms)
{
    if(fd < 0 || data == NULL || timeout_ms < 0) { errno = EINVAL; return -1; }
    int64_t deadline = linux_port_now_ms() + timeout_ms;
    size_t offset = 0;
    while(offset < length) {
        ssize_t n = write(fd, data + offset, length - offset);
        if(n > 0) { offset += (size_t)n; continue; }
        if(n < 0 && errno == EINTR) continue;
        if(n == 0 || (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))) {
            int64_t remaining = deadline - linux_port_now_ms();
            if(remaining <= 0) { errno = ETIMEDOUT; return -1; }
            struct pollfd pfd = {.fd=fd, .events=POLLOUT};
            int ready = poll(&pfd, 1, (int)remaining);
            if(ready < 0 && errno == EINTR) continue;
            if(ready <= 0) { if(ready == 0) errno = ETIMEDOUT; return -1; }
            if(pfd.revents & (POLLERR | POLLHUP | POLLNVAL)) { errno = EIO; return -1; }
            continue;
        }
        return -1;
    }
    return 0;
}
