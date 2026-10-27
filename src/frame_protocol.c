#include "frame_protocol.h"

#include <limits.h>
#include <string.h>

static void sat_inc(uint32_t *value) { if(*value != UINT32_MAX) ++*value; }

bool frame_encode(const frame_t *frame, uint8_t *out, size_t capacity, size_t *written)
{
    if(frame == NULL || out == NULL || written == NULL || frame->length > FRAME_PAYLOAD_MAX ||
       frame->type < FRAME_DATA || frame->type > FRAME_ACK || capacity < 7u + frame->length) return false;
    out[0] = 0xa5;
    out[1] = 0x5a;
    out[2] = frame->type;
    out[3] = (uint8_t)(frame->sequence >> 8);
    out[4] = (uint8_t)frame->sequence;
    out[5] = frame->length;
    memcpy(out + 6, frame->payload, frame->length);
    uint8_t checksum = 0;
    for(size_t i = 0; i < 6u + frame->length; i++) checksum ^= out[i];
    out[6u + frame->length] = checksum;
    *written = 7u + frame->length;
    return true;
}

void frame_parser_reset(frame_parser_t *parser)
{
    if(parser != NULL) memset(parser, 0, sizeof(*parser));
}

static void discard_first(frame_parser_t *parser)
{
    if(parser->used == 0) return;
    --parser->used;
    memmove(parser->bytes, parser->bytes + 1, parser->used);
}

bool frame_parser_push(frame_parser_t *parser, uint8_t byte, frame_t *out)
{
    if(parser == NULL || out == NULL) return false;
    if(parser->used == sizeof(parser->bytes)) {
        discard_first(parser);
        sat_inc(&parser->rejected);
    }
    parser->bytes[parser->used++] = byte;
    for(;;) {
        if(parser->used == 0) return false;
        if(parser->bytes[0] != 0xa5) { discard_first(parser); sat_inc(&parser->rejected); continue; }
        if(parser->used < 2) return false;
        if(parser->bytes[1] != 0x5a) { discard_first(parser); sat_inc(&parser->rejected); continue; }
        if(parser->used < 6) return false;
        uint8_t type = parser->bytes[2], length = parser->bytes[5];
        if(type < FRAME_DATA || type > FRAME_ACK || length > FRAME_PAYLOAD_MAX) {
            discard_first(parser);
            sat_inc(&parser->rejected);
            continue;
        }
        size_t total = 7u + length;
        if(parser->used < total) return false;
        uint8_t checksum = 0;
        for(size_t i = 0; i < total; i++) checksum ^= parser->bytes[i];
        if(checksum != 0) { discard_first(parser); sat_inc(&parser->rejected); continue; }
        out->type = type;
        out->sequence = (uint16_t)((uint16_t)parser->bytes[3] << 8 | parser->bytes[4]);
        out->length = length;
        memcpy(out->payload, parser->bytes + 6, length);
        parser->used -= total;
        memmove(parser->bytes, parser->bytes + total, parser->used);
        return true;
    }
}

void device_model_reset(device_model_t *model)
{
    if(model != NULL) memset(model, 0, sizeof(*model));
}

bool device_model_accept(device_model_t *model, const frame_t *frame)
{
    if(model == NULL || frame == NULL) return false;
    if(frame->type != FRAME_DATA || frame->length != 1 ||
       (model->has_sequence && frame->sequence != (uint16_t)(model->last_sequence + 1u))) {
        sat_inc(&model->rejected_sequence);
        return false;
    }
    model->has_sequence = true;
    model->last_sequence = frame->sequence;
    model->last_value = frame->payload[0];
    sat_inc(&model->accepted);
    return true;
}
