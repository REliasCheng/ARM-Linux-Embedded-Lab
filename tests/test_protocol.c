#include "frame_protocol.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

static bool feed(frame_parser_t *p, const uint8_t *data, size_t length, frame_t *out)
{
    bool seen = false;
    for(size_t i = 0; i < length; i++) if(frame_parser_push(p, data[i], out)) seen = true;
    return seen;
}

int main(void)
{
    uint8_t bytes[FRAME_SIZE_MAX];
    size_t used = 0;
    frame_t input = {.type=FRAME_DATA, .sequence=65535, .length=1, .payload={42}}, output;
    frame_parser_t parser;
    device_model_t model;
    assert(!frame_encode(NULL, bytes, sizeof(bytes), &used));
    assert(!frame_encode(&input, NULL, sizeof(bytes), &used));
    assert(!frame_encode(&input, bytes, 2, &used));
    assert(frame_encode(&input, bytes, sizeof(bytes), &used) && used == 8);
    frame_parser_reset(&parser);
    assert(!feed(&parser, bytes, used - 1, &output));
    assert(feed(&parser, bytes + used - 1, 1, &output));
    assert(output.sequence == 65535 && output.payload[0] == 42);
    device_model_reset(&model);
    assert(device_model_accept(&model, &output));
    assert(!device_model_accept(&model, &output) && model.rejected_sequence == 1);
    input.sequence = 0;
    assert(frame_encode(&input, bytes, sizeof(bytes), &used));
    assert(feed(&parser, bytes, used, &output));
    assert(device_model_accept(&model, &output) && model.last_sequence == 0);
    bytes[used - 1] ^= 1;
    assert(!feed(&parser, bytes, used, &output) && parser.rejected > 0);
    frame_parser_reset(&parser);
    input.length = FRAME_PAYLOAD_MAX;
    memset(input.payload, 0x55, sizeof(input.payload));
    assert(frame_encode(&input, bytes, sizeof(bytes), &used) && used == FRAME_SIZE_MAX);
    assert(feed(&parser, bytes, used, &output) && output.length == FRAME_PAYLOAD_MAX);
    input.length = FRAME_PAYLOAD_MAX + 1;
    assert(!frame_encode(&input, bytes, sizeof(bytes), &used));
    input.length = 1;
    assert(frame_encode(&input, bytes, sizeof(bytes), &used));
    bytes[5] = 255;
    assert(!feed(&parser, bytes, 6, &output));
    assert(parser.rejected > 0);
    frame_parser_reset(&parser);
    assert(!frame_parser_push(NULL, 0, &output));
    assert(!frame_parser_push(&parser, 0, NULL));
    assert(!device_model_accept(NULL, &output));
    assert(!device_model_accept(&model, NULL));
    assert(!device_model_accept(&model, &output));
    model.accepted = UINT32_MAX;
    model.rejected_sequence = UINT32_MAX;
    assert(!device_model_accept(&model, &output) && model.rejected_sequence == UINT32_MAX);
    device_model_reset(&model);
    assert(model.accepted == 0 && !model.has_sequence);
    puts("protocol: PASS");
    return 0;
}
