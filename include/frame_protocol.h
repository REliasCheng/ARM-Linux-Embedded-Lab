#ifndef FRAME_PROTOCOL_H
#define FRAME_PROTOCOL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define FRAME_PAYLOAD_MAX 16u
#define FRAME_SIZE_MAX (7u + FRAME_PAYLOAD_MAX)

enum { FRAME_DATA = 1, FRAME_COMMAND = 2, FRAME_ACK = 3 };

typedef struct {
    uint8_t type;
    uint16_t sequence;
    uint8_t length;
    uint8_t payload[FRAME_PAYLOAD_MAX];
} frame_t;

typedef struct {
    uint8_t bytes[FRAME_SIZE_MAX];
    size_t used;
    uint32_t rejected;
} frame_parser_t;

typedef struct {
    bool has_sequence;
    uint16_t last_sequence;
    uint32_t accepted;
    uint32_t rejected_sequence;
    uint8_t last_value;
} device_model_t;

bool frame_encode(const frame_t *frame, uint8_t *out, size_t capacity, size_t *written);
void frame_parser_reset(frame_parser_t *parser);
bool frame_parser_push(frame_parser_t *parser, uint8_t byte, frame_t *out);
void device_model_reset(device_model_t *model);
bool device_model_accept(device_model_t *model, const frame_t *frame);

#endif
