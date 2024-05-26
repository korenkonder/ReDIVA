/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "enbaya.hpp"
#include <stdio.h>

struct quat_trans_int {
    int32_t sample;
    vec4i quat;
    vec3i trans;
};

struct enb_plain_animation {
    int32_t data_count;
    int32_t track_count;
    int32_t num_components;
    float_t duration;
    float_t quantization_error;
    int32_t sample_rate;
    quat_trans_int* track_data[300];
    int32_t num_track_data_samples[300];
};

struct enb_anim_value {
    int32_t value;
    bool has_value;
};

struct enb_anim_track_sample {
    enb_anim_value comp[7];
};

struct enb_anim_tracks {
    enb_anim_track_sample* samples;
};

struct enb_stream {
    uint8_t* data;
    size_t size;
    bool init;
};

struct enb_byte_stream {
    enb_stream base;
};

struct enb_octet_stream {
    enb_byte_stream** stream_list;
    int32_t stream_count;
    uint32_t total_size;
    uint8_t* data;
    uint32_t size;
    enb_byte_stream* byte_stream;
};

struct enb_bit_octet_stream {
    enb_octet_stream base;
    uint16_t u2_counter;
    uint16_t u4_counter;
    uint8_t temp;
};

struct enb_anim_track_init_stream {
    enb_bit_octet_stream i2_stream;
    enb_octet_stream i8_stream;
    enb_octet_stream i16_stream;
    enb_octet_stream i32_stream;
};

struct enb_anim_track_stream {
    enb_bit_octet_stream i2_stream;
    enb_bit_octet_stream i4_stream;
    enb_octet_stream i8_stream;
    enb_octet_stream i16_stream;
    enb_octet_stream i32_stream;
};

struct enb_anim_state_stream {
    enb_bit_octet_stream u2_stream;
    enb_octet_stream u8_stream;
    enb_octet_stream u16_stream;
    enb_octet_stream u32_stream;
};

static void enb_get_track_data(enb_anim_context* anim_ctx, int32_t track_id,
    quat_trans** prev, quat_trans** next, float_t time);
static quat_trans* enb_get_track_data_next(enb_anim_context* anim_ctx, int32_t track_id);
static quat_trans* enb_get_track_data_prev(enb_anim_context* anim_ctx, int32_t track_id);
static void enb_init(enb_anim_context* anim_ctx, enb_anim_stream* anim_stream);
static void enb_init_decoder(enb_anim_context* anim_ctx);
static void enb_set_time(enb_anim_context* anim_ctx, float_t time);
static void enb_track_init(enb_anim_context* anim_ctx,
    const int32_t track_count, enb_anim_track_data_init_decoder* track_data_init);
static void enb_track_step_forward(enb_anim_context* anim_ctx,
    const int32_t track_count, enb_anim_track_data_decoder* track_data);
static void enb_track_step_backward(enb_anim_context* anim_ctx,
    const int32_t track_count, enb_anim_track_data_decoder* track_data);
static void enb_state_step_init(enb_anim_state* state,
    enb_anim_state_data_decoder* state_data);
static void enb_state_step_forward(enb_anim_state* state,
    enb_track* track, const int32_t track_count, enb_anim_state_data_decoder* state_data);
static void enb_state_step_backward(enb_anim_state* state,
    enb_track* track, const int32_t track_count, enb_anim_state_data_decoder* state_data);
static void enb_track_init_apply(enb_anim_context* anim_ctx,
    const int32_t track_count, const uint8_t* flags, const float_t quantization_error);
static void enb_track_apply(enb_anim_context* anim_ctx, const int32_t track_count,
    const bool forward, const float_t quantization_error, const float_t time);

inline static int32_t enb_anim_track_data_init_decode(enb_anim_track_data_init_decoder* track_data_init);
inline static int32_t enb_anim_track_data_forward_decode(enb_anim_track_data_decoder* track_data);
inline static int32_t enb_anim_track_data_backward_decode(enb_anim_track_data_decoder* track_data);
inline static uint32_t enb_anim_state_data_forward_decode(enb_anim_state_data_decoder* state_data);
inline static uint32_t enb_anim_state_data_backward_decode(enb_anim_state_data_decoder* state_data);

inline static uint8_t* enb_anim_stream_get_track_data_init_i2(enb_anim_stream* anim_stream);
inline static uint8_t* enb_anim_stream_get_track_data_init_i8(enb_anim_stream* anim_stream);
inline static uint8_t* enb_anim_stream_get_track_data_init_i16(enb_anim_stream* anim_stream);
inline static uint8_t* enb_anim_stream_get_track_data_init_i32(enb_anim_stream* anim_stream);
inline static uint8_t* enb_anim_stream_get_track_data_i2(enb_anim_stream* anim_stream);
inline static uint8_t* enb_anim_stream_get_track_data_i4(enb_anim_stream* anim_stream);
inline static uint8_t* enb_anim_stream_get_track_data_i8(enb_anim_stream* anim_stream);
inline static uint8_t* enb_anim_stream_get_track_data_i16(enb_anim_stream* anim_stream);
inline static uint8_t* enb_anim_stream_get_track_data_i32(enb_anim_stream* anim_stream);
inline static uint8_t* enb_anim_stream_get_state_data_u2(enb_anim_stream* anim_stream);
inline static uint8_t* enb_anim_stream_get_state_data_u8(enb_anim_stream* anim_stream);
inline static uint8_t* enb_anim_stream_get_state_data_u16(enb_anim_stream* anim_stream);
inline static uint8_t* enb_anim_stream_get_state_data_u32(enb_anim_stream* anim_stream);
inline static uint8_t* enb_anim_stream_get_track_flags(enb_anim_stream* anim_stream);
inline static uint32_t enb_anim_stream_get_length(enb_anim_stream* anim_stream);

static void enb_anim_stream_encoder_find_value_ranges(
    enb_anim_track_sample* samples, int32_t size, int32_t min_range_size);

static void enb_plain_anim_init(enb_plain_animation* plain_anim);
static void enb_plain_anim_prepare_data(enb_plain_animation* plain_anim, quat_trans* track_data,
    int32_t* track_data_count, int32_t num_tracks, int32_t num_components, float_t duration, int32_t sample_rate,
    float_t quantization_error, quat_trans_interp_method quat_method, quat_trans_interp_method trans_method);
static void enb_plain_anim_flip_rotation(enb_plain_animation* plain_anim, quat_trans** track_data);
static void enb_anim_track_data_flip_rotation(quat_trans* track_data, int32_t count);
static void enb_plain_anim_get_animation_data(enb_plain_animation* plain_anim,
    quat_trans** track_data, quat_trans_interp_method quat_method, quat_trans_interp_method trans_method);
static void enb_plain_anim_get_track_data(enb_plain_animation* plain_anim,
    quat_trans* track_data, int32_t* num_track_data_samples, int32_t track_id,
    quat_trans_interp_method quat_method, quat_trans_interp_method trans_method);
static void enb_plain_anim_get_data(enb_plain_animation* plain_anim, quat_trans_int* dst, quat_trans* src);
static int32_t enb_plain_anim_get_largest_track_id(enb_plain_animation* plain_anim);
static int32_t enb_plain_anim_get_num_track_data_samples(enb_plain_animation* plain_anim, int32_t track_id);
static void enb_plain_anim_get_samples(
    enb_plain_animation* plain_anim, int32_t track_id, enb_anim_track_sample* samples);
static quat_trans_int* enb_plain_anim_get_track_data_sample(
    enb_plain_animation* plain_anim, int32_t track_id, int32_t sample);
static void enb_plain_anim_write_data(enb_plain_animation* plain_anim, int32_t num_tracks,
    int32_t num_components, float_t duration, int32_t sample_rate, float_t quantization_error,
    uint8_t** data_out, size_t* data_out_len);
static void enb_plain_anim_free(enb_plain_animation* plain_anim);

static void enb_stream_init(enb_stream* stream);
static void enb_stream_reset(enb_stream* stream);
static void enb_stream_free(enb_stream* stream);

static void enb_byte_stream_alloc(enb_byte_stream* byte_stream, size_t size);
static void enb_byte_stream_init(enb_byte_stream* byte_stream);
static uint8_t* enb_byte_stream_get_data(enb_byte_stream* byte_stream);
static size_t enb_byte_stream_get_size(enb_byte_stream* byte_stream);
static void enb_byte_stream_free(enb_byte_stream* byte_stream);
static void enb_byte_stream_set_data(enb_byte_stream* byte_stream, uint8_t* value);
static void enb_byte_stream_set_size(enb_byte_stream* byte_stream, size_t value);

static void enb_octet_stream_alloc(enb_octet_stream* octet_stream);
static void enb_octet_stream_copy_to_byte_stream(enb_octet_stream* octet_stream, enb_byte_stream* byte_stream);
static void enb_octet_stream_init(enb_octet_stream* octet_stream);
static void enb_octet_stream_put_i8(enb_octet_stream* octet_stream, int8_t value);
static void enb_octet_stream_put_u8(enb_octet_stream* octet_stream, uint8_t value);
static void enb_octet_stream_put_i16(enb_octet_stream* octet_stream, int16_t value);
static void enb_octet_stream_put_u16(enb_octet_stream* octet_stream, uint16_t value);
static void enb_octet_stream_put_i32(enb_octet_stream* octet_stream, int32_t value);
static void enb_octet_stream_put_u32(enb_octet_stream* octet_stream, uint32_t value);
static void enb_octet_stream_reset(enb_octet_stream* octet_stream);
static void enb_octet_stream_free(enb_octet_stream* octet_stream);

static void enb_bit_octet_stream_init(enb_bit_octet_stream* bit_octet_stream);
static void enb_bit_octet_stream_copy_to_byte_stream(
    enb_bit_octet_stream* bit_octet_stream, enb_byte_stream* byte_stream);
static void enb_bit_octet_stream_put_u2(enb_bit_octet_stream* bit_octet_stream, uint8_t value);
static void enb_bit_octet_stream_put_u4(enb_bit_octet_stream* bit_octet_stream, uint8_t value);
static void enb_bit_octet_stream_free(enb_bit_octet_stream* bit_octet_stream);

static void enb_anim_track_init_stream_init(enb_anim_track_init_stream* track_init_stream);
static void enb_anim_track_init_stream_copy_to_byte_stream(enb_anim_track_init_stream* track_init_stream,
    enb_byte_stream* i2_stream, enb_byte_stream* i8_stream,
    enb_byte_stream* i16_stream, enb_byte_stream* i32_stream);
static void enb_anim_track_init_stream_put_value(enb_anim_track_init_stream* track_init_stream, int32_t value);
static void enb_anim_track_init_stream_free(enb_anim_track_init_stream* track_init_stream);

static void enb_anim_track_stream_init(enb_anim_track_stream* track_stream);
static void enb_anim_track_stream_copy_to_byte_stream(enb_anim_track_stream* track_stream,
    enb_byte_stream* i2_stream, enb_byte_stream* i4_stream, enb_byte_stream* i8_stream,
    enb_byte_stream* i16_stream, enb_byte_stream* i32_stream);
static void enb_anim_track_stream_put_value(enb_anim_track_stream* track_stream, int32_t value);
static void enb_anim_track_stream_free(enb_anim_track_stream* track_stream);

static void enb_anim_state_stream_init(enb_anim_state_stream* state_stream);
static void enb_anim_state_stream_copy_to_byte_stream(enb_anim_state_stream* state_stream,
    enb_byte_stream* u2_stream, enb_byte_stream* u8_stream,
    enb_byte_stream* u16_stream, enb_byte_stream* u32_stream);
static void enb_anim_state_stream_put_value(enb_anim_state_stream* state_stream, uint32_t value);
static void enb_anim_state_stream_free(enb_anim_state_stream* state_stream);

static const int32_t shift_table_track_data_i2[] = { 6, 4, 2, 0 };      // 0x08BF1CE8
static const int32_t shift_table_track_data_i4[] = { 4, 0 };            // 0x08BF1CF8
static const int32_t shift_table_track_data_init_i2[] = { 6, 4, 2, 0 }; // 0x08BF2160
static const int32_t shift_table_state_data_u2[] = { 6, 4, 2, 0 };      // 0x08BF2170
static const int32_t value_table_track_data_i2[] = { 0, 1, 0, -1 };     // 0x08BB3FC0
static const int32_t value_table_track_data_i4[] = { 0, 8, 2, 3, 4, 5, 6, 7, -8, -7, -6, -5, -4, -3, -2, -9 }; // 0x08BB3FD0

int32_t enb_process(uint8_t* data_in, uint8_t** data_out, size_t* data_out_len, float_t* duration,
    float_t* fps, int32_t* frames, quat_trans_interp_method quat_method, quat_trans_interp_method trans_method) {
    if (!data_in)
        return -1;
    else if (!data_out)
        return -2;
    else if (!data_out_len)
        return -3;
    else if (!duration)
        return -4;
    else if (!fps)
        return -5;
    else if (!frames)
        return -6;

    enb_anim_context* anim_ctx;
    int32_t code = enb_initialize(data_in, &anim_ctx);
    if (code) {
        free_def(*data_out);
        return code - 0x10;
    }

    enb_anim_stream* anim_stream = (enb_anim_stream*)data_in;
    int32_t track_count = anim_stream->track_count;
    *duration = anim_stream->duration;

    if (*fps > 600.0f)
        *fps = 600.0f;
    else if (*fps < (float_t)anim_stream->sample_rate)
        *fps = (float_t)anim_stream->sample_rate;

    float_t frames_float = *duration * *fps;
    *frames = (int32_t)(int64_t)frames_float + (fmodf(frames_float, 1.0f) >= 0.5f) + 1;
    if (*frames > 0x7FFFFFFFU)
        return -7;

    *data_out_len = sizeof(quat_trans) * track_count * *frames + 0x10;
    *data_out = force_malloc<uint8_t>(*data_out_len);

    if (!*data_out)
        return -8;

    memset(*data_out, 0, *data_out_len);

    ((int32_t*)*data_out)[0] = track_count;
    ((int32_t*)*data_out)[1] = *frames;
    ((float_t*)*data_out)[2] = *fps;
    ((float_t*)*data_out)[3] = *duration;

    quat_trans* track_data = (quat_trans*)(*data_out + 0x10);
    for (int32_t i = 0; i < *frames; i++) {
        float_t time = (float_t)i / *fps;
        for (int32_t j = 0; j < track_count; j++, track_data++)
            enb_get_component_values(anim_ctx, time, j, track_data, quat_method, trans_method);
    }
    enb_free(&anim_ctx);
    return 0;
}

int32_t enb_initialize(uint8_t* data, enb_anim_context** anim_ctx) {
    if (!data)
        return -1;
    else if (!anim_ctx)
        return -2;
    *anim_ctx = 0;

    enb_anim_stream* anim_stream = (enb_anim_stream*)data;
    enb_anim_context* ac = force_malloc<enb_anim_context>();
    if (!ac)
        return -3;

    memset(ac, 0, sizeof(enb_anim_context));

    ac->data.stream = anim_stream;
    enb_init(ac, anim_stream);

    ac->data.track = force_malloc<enb_track>(anim_stream->track_count);

    if (!ac->data.track) {
        free_def(ac);
        return -4;
    }

    memset(ac->data.track, 0, sizeof(enb_track) * anim_stream->track_count);

    *anim_ctx = ac;
    return 0;
}

void enb_free(enb_anim_context** anim_ctx) {
    if (!anim_ctx || !*anim_ctx)
        return;

    free_def((*anim_ctx)->data.track);
    free_def(*anim_ctx);
    *anim_ctx = 0;
}

void enb_get_component_values(enb_anim_context* anim_ctx, float_t time, int32_t track_id,
    quat_trans* data, quat_trans_interp_method quat_method, quat_trans_interp_method trans_method) {
    quat_trans* prev = 0;
    quat_trans* next = 0;
    enb_get_track_data(anim_ctx, track_id, &prev, &next, time);

    float_t blend = (time - prev->time) / anim_ctx->seconds_per_sample;
    interp_quat_trans(prev, next, data, blend, quat_method, trans_method);
}

int32_t enb_encode_data(quat_trans* track_data, int32_t* track_data_count, int32_t num_tracks,
    int32_t num_components, float_t duration, int32_t sample_rate, float_t quantization_error,
    quat_trans_interp_method quat_method, quat_trans_interp_method trans_method,
    uint8_t** data_out, size_t* data_out_len) {
    if (num_components != 7)
        return -1;

    enb_plain_animation plain_anim;
    enb_plain_anim_init(&plain_anim);
    enb_plain_anim_prepare_data(&plain_anim, track_data, track_data_count, num_tracks,
        7, duration, sample_rate, quantization_error, quat_method, trans_method);
    enb_plain_anim_write_data(&plain_anim, num_tracks, 7, duration,
        sample_rate, quantization_error, data_out, data_out_len);
    enb_plain_anim_free(&plain_anim);
    return 0;
}

static void enb_get_track_data(enb_anim_context* anim_ctx, int32_t track_id,
    quat_trans** prev, quat_trans** next, float_t time) { // 0x08A8C34
    if (time < anim_ctx->data.previous_sample_time
        || time > anim_ctx->data.current_sample_time)
        enb_set_time(anim_ctx, time);

    *next = enb_get_track_data_next(anim_ctx, track_id);
    *prev = enb_get_track_data_prev(anim_ctx, track_id);
}

static quat_trans* enb_get_track_data_next(enb_anim_context* anim_ctx, int32_t track_id) {
    return &anim_ctx->data.track[track_id].qt[anim_ctx->track_selector & 0x01];
}

static quat_trans* enb_get_track_data_prev(enb_anim_context* anim_ctx, int32_t track_id) {
    return &anim_ctx->data.track[track_id].qt[(anim_ctx->track_selector & 0x01) ^ 0x01];
}

static void enb_init(enb_anim_context* anim_ctx, enb_anim_stream* anim_stream) { // 0x08A08050 in ULJM05681
    uint8_t* data = (uint8_t*)anim_stream;
    anim_ctx->data.current_sample = -1;
    anim_ctx->data.current_sample_time = -1.0f;
    anim_ctx->data.previous_sample_time = -1.0f;
    anim_ctx->requested_time = -1.0f;
    anim_ctx->seconds_per_sample = 1.0f / (float_t)anim_stream->sample_rate;
    anim_ctx->track_direction = 0;

    anim_ctx->data.data_length = enb_anim_stream_get_length(anim_stream);
    anim_ctx->track_flags = enb_anim_stream_get_track_flags(anim_stream);
    anim_ctx->track_data_init.i2 = (const uint8_t*)enb_anim_stream_get_track_data_init_i2(anim_stream);
    anim_ctx->track_data_init.i8 = (const int8_t*)enb_anim_stream_get_track_data_init_i8(anim_stream);
    anim_ctx->track_data_init.i16 = (const int16_t*)enb_anim_stream_get_track_data_init_i16(anim_stream);
    anim_ctx->track_data_init.i32 = (const int32_t*)enb_anim_stream_get_track_data_init_i32(anim_stream);
    anim_ctx->track_data.i2 = (const uint8_t*)enb_anim_stream_get_track_data_i2(anim_stream);
    anim_ctx->track_data.i4 = (const uint8_t*)enb_anim_stream_get_track_data_i4(anim_stream);
    anim_ctx->track_data.i8 = (const int8_t*)enb_anim_stream_get_track_data_i8(anim_stream);
    anim_ctx->track_data.i16 = (const int16_t*)enb_anim_stream_get_track_data_i16(anim_stream);
    anim_ctx->track_data.i32 = (const int32_t*)enb_anim_stream_get_track_data_i32(anim_stream);
    anim_ctx->state_data.u2 = enb_anim_stream_get_state_data_u2(anim_stream);
    anim_ctx->state_data.u8 = enb_anim_stream_get_state_data_u8(anim_stream);
    anim_ctx->state_data.u16 = (const uint16_t*)enb_anim_stream_get_state_data_u16(anim_stream);
    anim_ctx->state_data.u32 = (const uint32_t*)enb_anim_stream_get_state_data_u32(anim_stream);
    anim_ctx->state_data.u8 = enb_anim_stream_get_state_data_u8(anim_stream);
    anim_ctx->state_data.u8 = enb_anim_stream_get_state_data_u8(anim_stream);

    enb_init_decoder(anim_ctx);
}

static void enb_init_decoder(enb_anim_context* anim_ctx) { // 0x08A07FD0 in ULJM05681
    anim_ctx->track_data_init_dec.i2 = anim_ctx->track_data_init.i2;
    anim_ctx->track_data_init_dec.i8 = anim_ctx->track_data_init.i8;
    anim_ctx->track_data_init_dec.i16 = anim_ctx->track_data_init.i16;
    anim_ctx->track_data_init_dec.i32 = anim_ctx->track_data_init.i32;
    anim_ctx->track_data_init_dec.i2_counter = 0;

    anim_ctx->track_data_dec.i2 = anim_ctx->track_data.i2;
    anim_ctx->track_data_dec.i4 = anim_ctx->track_data.i4;
    anim_ctx->track_data_dec.i8 = anim_ctx->track_data.i8;
    anim_ctx->track_data_dec.i16 = anim_ctx->track_data.i16;
    anim_ctx->track_data_dec.i32 = anim_ctx->track_data.i32;
    anim_ctx->track_data_dec.i2_counter = 0;
    anim_ctx->track_data_dec.i4_counter = 0;

    anim_ctx->state_data_dec.u2 = anim_ctx->state_data.u2;
    anim_ctx->state_data_dec.u8 = anim_ctx->state_data.u8;
    anim_ctx->state_data_dec.u16 = anim_ctx->state_data.u16;
    anim_ctx->state_data_dec.u32 = anim_ctx->state_data.u32;
    anim_ctx->state_data_dec.u2_counter = 0;
}

static void enb_set_time(enb_anim_context* anim_ctx, float_t time) { // 0x08A0876C in ULJM05681
    if (time == anim_ctx->requested_time)
        return;

    uint32_t track_count = anim_ctx->data.stream->track_count;
    float_t quantization_error = anim_ctx->data.stream->quantization_error;
    float_t requested_time = anim_ctx->requested_time;
    float_t sps = anim_ctx->seconds_per_sample;

    if ((requested_time == -1.0f) || (time < 0.000001f) || (requested_time - time > time)
        || ((sps <= requested_time) && (sps > time))) {
        anim_ctx->data.current_sample = 0;
        anim_ctx->data.current_sample_time = 0.0f;
        anim_ctx->data.previous_sample_time = 0.0f;
        anim_ctx->track_direction = 0;

        enb_init_decoder(anim_ctx);
        enb_state_step_init(&anim_ctx->state, &anim_ctx->state_data_dec);
        enb_track_init(anim_ctx, track_count, &anim_ctx->track_data_init_dec);
        enb_track_init_apply(anim_ctx, track_count, anim_ctx->track_flags, quantization_error);
    }

    anim_ctx->requested_time = time;
    if (time < 0.000001f)
        return;

    while ((time > anim_ctx->data.current_sample_time)
        && (anim_ctx->data.stream->duration - anim_ctx->data.current_sample_time > 0.00001f)) {
        if (anim_ctx->track_direction == 2) {
            enb_anim_state_data_forward_decode(&anim_ctx->state_data_dec);
            anim_ctx->track_direction = 1;
        }
        else if (anim_ctx->data.current_sample > 0) {
            enb_state_step_forward(&anim_ctx->state,
                anim_ctx->data.track, track_count, &anim_ctx->state_data_dec);
            anim_ctx->track_direction = 1;
        }

        enb_track_step_forward(anim_ctx, track_count, &anim_ctx->track_data_dec);
        float_t time = ++anim_ctx->data.current_sample * sps;

        if (time >= anim_ctx->data.stream->duration)
            time = anim_ctx->data.stream->duration;

        enb_track_apply(anim_ctx, track_count, true, quantization_error, time);
        anim_ctx->data.current_sample_time = anim_ctx->data.current_sample * sps;
        anim_ctx->data.previous_sample_time = (anim_ctx->data.current_sample - 1) * sps;
    }

    while (time < anim_ctx->data.previous_sample_time) {
        if (anim_ctx->track_direction == 1) {
            enb_anim_state_data_backward_decode(&anim_ctx->state_data_dec);
            anim_ctx->track_direction = 2;
        }
        else
            enb_state_step_backward(&anim_ctx->state,
                anim_ctx->data.track, track_count, &anim_ctx->state_data_dec);

        anim_ctx->data.current_sample--;
        enb_track_step_backward(anim_ctx, track_count, &anim_ctx->track_data_dec);

        anim_ctx->data.current_sample_time = anim_ctx->data.current_sample * sps;
        anim_ctx->data.previous_sample_time = (anim_ctx->data.current_sample - 1) * sps;
        enb_track_apply(anim_ctx, track_count, false, -quantization_error, anim_ctx->data.previous_sample_time);
    }
}

static void enb_track_init(enb_anim_context* anim_ctx,
    const int32_t track_count, enb_anim_track_data_init_decoder* track_data_init) { // 0x08A08D3C in ULJM05681
    enb_track* track = anim_ctx->data.track;
    for (int32_t i = 0; i < track_count; i++, track++) {
        for (int32_t j = 0; j < 7; j++) {
            int32_t val = enb_anim_track_data_init_decode(track_data_init);

            switch (j) {
            case 0:
                track->quat.x = (float_t)val;
                break;
            case 1:
                track->quat.y = (float_t)val;
                break;
            case 2:
                track->quat.z = (float_t)val;
                break;
            case 3:
                track->quat.w = (float_t)val;
                break;
            case 4:
                track->trans.x = (float_t)val;
                break;
            case 5:
                track->trans.y = (float_t)val;
                break;
            case 6:
                track->trans.z = (float_t)val;
                break;
            }
        }
    }
    anim_ctx->data.current_sample = 0;
}


static void enb_track_step_forward(enb_anim_context* anim_ctx,
    const int32_t track_count, enb_anim_track_data_decoder* track_data) { // 0x08A08E7C in ULJM05681
    enb_track* track = anim_ctx->data.track;
    for (int32_t i = 0; i < track_count; i++, track++) {
        if (track->flags == 0)
            continue;

        for (int32_t j = 0; j < 7; j++) {
            if ((track->flags & (1 << j)) == 0)
                continue;

            int32_t val = enb_anim_track_data_forward_decode(track_data);

            switch (j) {
            case 0:
                track->quat.x += (float_t)val;
                break;
            case 1:
                track->quat.y += (float_t)val;
                break;
            case 2:
                track->quat.z += (float_t)val;
                break;
            case 3:
                track->quat.w += (float_t)val;
                break;
            case 4:
                track->trans.x += (float_t)val;
                break;
            case 5:
                track->trans.y += (float_t)val;
                break;
            case 6:
                track->trans.z += (float_t)val;
                break;
            }
        }
    }
}


static void enb_track_step_backward(enb_anim_context* anim_ctx,
    const int32_t track_count, enb_anim_track_data_decoder* track_data) { // 0x08A090A0 in ULJM05681
    enb_track* track = anim_ctx->data.track;
    track += track_count - 1LL;
    for (int32_t i = track_count - 1; i != -1; i--, track--) {
        if (track->flags == 0)
            continue;

        for (int32_t j = 0; j < 7; j++) {
            if ((track->flags & (1 << (6 - j))) == 0)
                continue;

            int32_t val = enb_anim_track_data_backward_decode(track_data);

            switch (6 - j) {
            case 0:
                track->quat.x -= val;
                break;
            case 1:
                track->quat.y -= val;
                break;
            case 2:
                track->quat.z -= val;
                break;
            case 3:
                track->quat.w -= val;
                break;
            case 4:
                track->trans.x -= val;
                break;
            case 5:
                track->trans.y -= val;
                break;
            case 6:
                track->trans.z -= val;
                break;
            }
        }
    }
}

static void enb_state_step_init(enb_anim_state* state,
    enb_anim_state_data_decoder* state_data) { // 0x08A0931C in ULJM05681
    state->next_step = enb_anim_state_data_forward_decode(state_data);
    state->prev_step = 0;
}

static void enb_state_step_forward(enb_anim_state* state,
    enb_track* track, const int32_t track_count, enb_anim_state_data_decoder* state_data) { // 0x08A09404 in ULJM05681
    int32_t track_comps_count = track_count * 7;
    int32_t i = 0;
    while (i < track_comps_count) {
        int32_t next_step = state->next_step;
        if (next_step == 0) {
            track[i / 7].flags ^= 0x01 << (i % 7);
            state->next_step = enb_anim_state_data_forward_decode(state_data);
            state->prev_step = 0;
            i++;
        }
        else {
            int32_t temp = next_step < (track_comps_count - i) ? next_step : (track_comps_count - i);
            i += temp;
            state->next_step -= temp;
            state->prev_step += temp;
        }
    }
}

static void enb_state_step_backward(enb_anim_state* state,
    enb_track* track, const int32_t track_count, enb_anim_state_data_decoder* state_data) { // 0x08A0968C in ULJM05681
    int32_t track_comps_count = track_count * 7;
    int32_t i = track_comps_count - 1;
    while (i != -1) {
        int32_t prev_step = state->prev_step;
        if (prev_step == 0) {
            track[i / 7].flags ^= 0x01 << (i % 7);
            state->next_step = 0;
            state->prev_step = enb_anim_state_data_backward_decode(state_data);
            i--;
        }
        else {
            int32_t temp = prev_step < (i + 1) ? prev_step : (i + 1);
            i -= temp;
            state->next_step += temp;
            state->prev_step -= temp;
        }
    }
}

static void enb_track_init_apply(enb_anim_context* anim_ctx,
    const int32_t track_count, const uint8_t* flags, float_t quantization_error) { // 0x08A086CC in ULJM05681
    enb_track* track = anim_ctx->data.track;
    for (int32_t i = 0; i < track_count; i++, track++) {
        quat quat_delta = track->quat;
        vec3 trans_delta = track->trans;

        quat quat_result = quat_delta * quantization_error;
        vec3 trans_result = trans_delta * quantization_error;

        quat_result = quat::normalize_rcp(quat_result);

        track->qt[0].quat = quat_result;
        track->qt[0].trans = trans_result;
        track->qt[0].time = 0.0f;
        track->qt[1].quat = quat_result;
        track->qt[1].trans = trans_result;
        track->qt[1].time = 0.0f;

        track->quat = 0.0f;
        track->trans = 0.0f;
        track->flags = *flags++;
    }
    anim_ctx->track_selector = 0;
}

static void enb_track_apply(enb_anim_context* anim_ctx, const int32_t track_count,
    const bool forward, const float_t quantization_error, const float_t time) { // 0x08A085D8 in ULJM05681
    uint8_t s0;
    uint8_t s1;
    if (forward) {
        s0 = anim_ctx->track_selector & 0x01;
        s1 = s0 ^ 0x01;
        anim_ctx->track_selector = s1;
    }
    else {
        s1 = anim_ctx->track_selector & 0x01;
        s0 = s1 ^ 0x01;
        anim_ctx->track_selector = s0;
    }

    enb_track* track = anim_ctx->data.track;
    for (int32_t i = 0; i < track_count; i++, track++) {
        quat quat_delta = track->quat;
        vec3 trans_delta = track->trans;

        quat quat_data = track->qt[s0].quat;
        vec3 trans_data = track->qt[s0].trans;

        quat quat_result = quat_delta * quantization_error + quat_data;
        vec3 trans_result = trans_delta * quantization_error + trans_data;

        quat_result = quat::normalize_rcp(quat_result);

        track->qt[s1].quat = quat_result;
        track->qt[s1].trans = trans_result;
        track->qt[s1].time = time;
    }
}

inline static int32_t enb_anim_track_data_init_decode(enb_anim_track_data_init_decoder* track_data_init) {
    if (track_data_init->i2_counter == 4) {
        track_data_init->i2_counter = 0;
        track_data_init->i2++;
    }

    int32_t val;
    val = *track_data_init->i2 >> shift_table_track_data_init_i2[track_data_init->i2_counter++];
    val &= 0x03;

    switch (val) {
    case 1:
        val = *track_data_init->i8++;
        break;
    case 2:
        val = *track_data_init->i16++;
        break;
    case 3:
        val = *track_data_init->i32++;
        break;
    }

    return val;
}

inline static int32_t enb_anim_track_data_forward_decode(enb_anim_track_data_decoder* track_data) {
    if (track_data->i2_counter == 4) {
        track_data->i2_counter = 0;
        track_data->i2++;
    }

    int32_t val;
    val = *track_data->i2 >> shift_table_track_data_i2[track_data->i2_counter++];
    val &= 0x03;

    if (val == 2) {
        if (track_data->i4_counter == 2) {
            track_data->i4_counter = 0;
            track_data->i4++;
        }

        val = *track_data->i4 >> shift_table_track_data_i4[track_data->i4_counter++];
        val &= 0x0F;

        if (val == 0) {
            val = *track_data->i8++;
            if (val == 0) {
                val = *track_data->i16++;
                if (val == 0)
                    val = *track_data->i32++;
            }
            else if ((val > 0) && (val < 9))
                val += 0x7F;
            else if ((val > -9) && (val < 0))
                val -= 0x80;
        }
        else
            val = value_table_track_data_i4[val];
    }
    else
        val = value_table_track_data_i2[val];

    return val;
}

inline static int32_t enb_anim_track_data_backward_decode(enb_anim_track_data_decoder* track_data) {
    if (--track_data->i2_counter == -1) {
        track_data->i2_counter = 3;
        track_data->i2--;
    }

    int32_t val;
    val = *track_data->i2 >> shift_table_track_data_i2[track_data->i2_counter];
    val &= 0x03;

    if (val == 2) {
        if (--track_data->i4_counter == -1) {
            track_data->i4_counter = 1;
            track_data->i4--;
        }

        val = *track_data->i4 >> shift_table_track_data_i4[track_data->i4_counter];
        val &= 0x0F;

        if (val == 0) {
            val = *--track_data->i8;
            if (val == 0) {
                val = *--track_data->i16;
                if (val == 0)
                    val = *--track_data->i32;
            }
            else if ((val > 0) && (val < 9))
                val += 0x7F;
            else if ((val > -9) && (val < 0))
                val -= 0x80;
        }
        else
            val = value_table_track_data_i4[val];
    }
    else
        val = value_table_track_data_i2[val];

    return val;
}

inline static uint32_t enb_anim_state_data_forward_decode(enb_anim_state_data_decoder* state_data) {
    if (state_data->u2_counter == 4) {
        state_data->u2_counter = 0;
        state_data->u2++;
    }

    uint32_t val;
    val = *state_data->u2 >> shift_table_state_data_u2[state_data->u2_counter++];
    val &= 0x03;

    switch (val) {
    case 1:
        val = *state_data->u8++;
        break;
    case 2:
        val = *state_data->u16++;
        break;
    case 3:
        val = *state_data->u32++;
        break;
    }

    return val;
}

inline static uint32_t enb_anim_state_data_backward_decode(enb_anim_state_data_decoder* state_data) {
    if (--state_data->u2_counter == -1) {
        state_data->u2_counter = 3;
        state_data->u2--;
    }

    uint32_t val;
    val = *state_data->u2 >> shift_table_state_data_u2[state_data->u2_counter];
    val &= 0x03;

    switch (val) {
    case 1:
        val = *--state_data->u8;
        break;
    case 2:
        val = *--state_data->u16;
        break;
    case 3:
        val = *--state_data->u32;
        break;
    }

    return val;
}

inline static uint8_t* enb_anim_stream_get_track_data_init_i2(enb_anim_stream* anim_stream) {
    return &enb_anim_stream_get_state_data_u16(anim_stream)[anim_stream->state_data_u16_length];
}

inline static uint8_t* enb_anim_stream_get_track_data_init_i8(enb_anim_stream* anim_stream) {
    return &enb_anim_stream_get_track_data_init_i2(anim_stream)[anim_stream->track_data_init_i2_length];
}

inline static uint8_t* enb_anim_stream_get_track_data_init_i16(enb_anim_stream* anim_stream) {
    return &enb_anim_stream_get_state_data_u32(anim_stream)[anim_stream->state_data_u32_length];
}

inline static uint8_t* enb_anim_stream_get_track_data_init_i32(enb_anim_stream* anim_stream) {
    return (uint8_t*)anim_stream + sizeof(enb_anim_stream);
}

inline static uint8_t* enb_anim_stream_get_track_data_i2(enb_anim_stream* anim_stream) {
    return &enb_anim_stream_get_track_data_init_i8(anim_stream)[anim_stream->track_data_init_i8_length];
}

inline static uint8_t* enb_anim_stream_get_track_data_i4(enb_anim_stream* anim_stream) {
    return &enb_anim_stream_get_track_data_i2(anim_stream)[anim_stream->track_data_i2_length];
}

inline static uint8_t* enb_anim_stream_get_track_data_i8(enb_anim_stream* anim_stream) {
    return &enb_anim_stream_get_track_data_i4(anim_stream)[anim_stream->track_data_i4_length];
}

inline static uint8_t* enb_anim_stream_get_track_data_i16(enb_anim_stream* anim_stream) {
    return &enb_anim_stream_get_track_data_init_i16(anim_stream)[anim_stream->track_data_init_i16_length];
}

inline static uint8_t* enb_anim_stream_get_track_data_i32(enb_anim_stream* anim_stream) {
    return &enb_anim_stream_get_track_data_init_i32(anim_stream)[anim_stream->track_data_init_i32_length];
}

inline static uint8_t* enb_anim_stream_get_state_data_u2(enb_anim_stream* anim_stream) {
    return &enb_anim_stream_get_track_data_i8(anim_stream)[anim_stream->track_data_i8_length];
}

inline static uint8_t* enb_anim_stream_get_state_data_u8(enb_anim_stream* anim_stream) {
    return &enb_anim_stream_get_state_data_u2(anim_stream)[anim_stream->state_data_u2_length];
}

inline static uint8_t* enb_anim_stream_get_state_data_u16(enb_anim_stream* anim_stream) {
    return &enb_anim_stream_get_track_data_i16(anim_stream)[anim_stream->track_data_i16_length];
}

inline static uint8_t* enb_anim_stream_get_state_data_u32(enb_anim_stream* anim_stream) {
    return &enb_anim_stream_get_track_data_i32(anim_stream)[anim_stream->track_data_i32_length];
}

inline static uint8_t* enb_anim_stream_get_track_flags(enb_anim_stream* anim_stream) {
    return &enb_anim_stream_get_state_data_u8(anim_stream)[anim_stream->state_data_u8_length];
}

inline static uint32_t enb_anim_stream_get_length(enb_anim_stream* anim_stream) {
    return sizeof(enb_anim_stream)
        + anim_stream->track_data_init_i2_length
        + anim_stream->track_data_init_i8_length
        + anim_stream->track_data_init_i16_length
        + anim_stream->track_data_init_i32_length
        + anim_stream->track_data_i2_length
        + anim_stream->track_data_i4_length
        + anim_stream->track_data_i8_length
        + anim_stream->track_data_i16_length
        + anim_stream->track_data_i32_length
        + anim_stream->state_data_u2_length
        + anim_stream->state_data_u8_length
        + anim_stream->state_data_u16_length
        + anim_stream->state_data_u32_length
        + anim_stream->track_flags_length;
}

static void enb_anim_stream_encoder_find_value_ranges(
    enb_anim_track_sample* samples, int32_t size, int32_t min_range_size) {
    int32_t range_size = 0;
    for (int32_t i = 0; i < 7; i++) {
        bool no_value = true;
        for (int32_t j = 1; j < size; j++)
            if (samples[j].comp[i].value) {
                no_value = false;
                break;
            }

        if (no_value) {
            for (int32_t j = 1; j < size; j++)
                samples[j].comp[i].has_value = false;
            continue;
        }

        bool set_no_value = true;
        int32_t l;
        for (l = 1; l < size; l++)
            if (samples[l].comp[i].value) {
                if (set_no_value && range_size > min_range_size / 2 || range_size > min_range_size)
                    for (int32_t k = l - 1; k > l - 1 - range_size; k--)
                        samples[k].comp[i].has_value = false;
                range_size = 0;
                set_no_value = false;
            }
            else
                range_size++;

        if (range_size > min_range_size / 2)
            for (int32_t k = l - 1; k > l - 1 - range_size; k--)
                samples[k].comp[i].has_value = false;
        range_size = 0;
    }
}

static void enb_anim_stream_encoder_write(enb_anim_tracks* track_data, int32_t num_tracks,
    int32_t num_components, int32_t num_track_data_samples, enb_anim_track_init_stream* track_data_init_stream,
    enb_anim_track_stream* track_data_stream, enb_anim_state_stream* state_data_stream,
    enb_byte_stream* track_flags_byte_stream, FILE* f) {
    if (num_track_data_samples > 0)
        for (int32_t j = 0; j < num_tracks; j++)
            for (int32_t k = 0; k < 7; k++)
                    enb_anim_track_init_stream_put_value(track_data_init_stream,
                        track_data[j].samples->comp[k].value);

    for (int32_t i = 1; i < num_track_data_samples; i++)
        for (int32_t j = 0; j < num_tracks; j++)
            for (int32_t k = 0; k < 7; k++)
                if (track_data[j].samples[i].comp[k].has_value)
                    enb_anim_track_stream_put_value(track_data_stream,
                        track_data[j].samples[i].comp[k].value);

    enb_octet_stream track_flags_stream;
    enb_octet_stream_init(&track_flags_stream);

    for (int32_t i = 0; i < num_tracks; i++) {
        uint8_t flags = 0x00;
        for (int32_t j = 0; j < 7; j++)
            if (track_data[i].samples[1].comp[j].has_value)
                flags |= (uint8_t)(0x01 << j);
        enb_octet_stream_put_u8(&track_flags_stream, flags);
    }

    enb_octet_stream_copy_to_byte_stream(&track_flags_stream, track_flags_byte_stream);
    enb_octet_stream_free(&track_flags_stream);

    uint32_t step = 0;
    for (int32_t i = 2; i < num_track_data_samples; i++)
        for (int32_t j = 0; j < num_tracks; j++)
            for (int32_t k = 0; k < 7; k++) {
                step++;
                if (track_data[j].samples[i].comp[k].has_value != track_data[j].samples[i - 1].comp[k].has_value) {
                    enb_anim_state_stream_put_value(state_data_stream, step - 1);
                    if (f)
                        fprintf(f, "boneId: %d,  compNum: %d,  step: %d\n", j, k, step - 1);
                    step = 0;
                }
            }

    enb_anim_state_stream_put_value(state_data_stream, step + 100);

    if (f)
        fprintf(f, "step: %d\n", step + 100);
}

static void enb_plain_anim_init(enb_plain_animation* plain_anim) {
    plain_anim->data_count = 0;
    plain_anim->track_count = 0;
    plain_anim->num_components = 0;
    plain_anim->duration = 0.0f;
    plain_anim->quantization_error = 0.0f;
    plain_anim->sample_rate = 0;

    for (int32_t i = 0; i < 300; i++)
        plain_anim->track_data[i] = 0;

    for (int32_t i = 0; i < 300; i++)
        plain_anim->num_track_data_samples[i] = 0;
}

static void enb_plain_anim_prepare_data(enb_plain_animation* plain_anim, quat_trans* track_data,
    int32_t* track_data_count, int32_t num_tracks, int32_t num_components, float_t duration, int32_t sample_rate,
    float_t quantization_error, quat_trans_interp_method quat_method, quat_trans_interp_method trans_method) {
    plain_anim->quantization_error = quantization_error + quantization_error;
    plain_anim->sample_rate = sample_rate;
    plain_anim->num_components = 7;

    quat_trans** block = (quat_trans**)calloc(300, sizeof(quat_trans*));
    if (!block)
        return;

    for (int32_t i = 0; i < 300; i++) {
        if (plain_anim->track_data[i]) {
            free(plain_anim->track_data[i]);
            plain_anim->track_data[i] = 0;
        }

        plain_anim->num_track_data_samples[i] = 0;
    }

    plain_anim->data_count = 0;
    plain_anim->track_count = num_tracks;
    plain_anim->duration = duration;
    for (int32_t i = 0, j = 0; i < num_tracks; i++) {
        block[i] = (quat_trans*)calloc(track_data_count[i], sizeof(quat_trans));
        if (!block[i])
            continue;

        plain_anim->num_track_data_samples[i] = track_data_count[i];
        plain_anim->data_count += track_data_count[i];
        for (int32_t k = 0; k < plain_anim->num_track_data_samples[i]; j++, k++)
            block[i][k] = track_data[j];
    }

    enb_plain_anim_flip_rotation(plain_anim, block);
    enb_plain_anim_get_animation_data(plain_anim, block, quat_method, trans_method);

    for (int32_t l = 0; l < num_tracks; l++)
        if (block[l]) {
            free(block[l]);
            block[l] = 0;
        }

    if (block)
        free(block);
}

static void enb_plain_anim_flip_rotation(enb_plain_animation* plain_anim, quat_trans** track_data) {
    for (int32_t i = 0; i < plain_anim->track_count; i++)
        enb_anim_track_data_flip_rotation(track_data[i], plain_anim->num_track_data_samples[i]);
}

static void enb_anim_track_data_flip_rotation(quat_trans* track_data, int32_t count) {
    for (int32_t i = 1; i < count; i++) {
        if (track_data[i].quat.w * track_data[i - 1].quat.w >= 0.0f
            || fabsf(track_data[i].quat.w - track_data[i - 1].quat.w) <= 0.1f)
            continue;

        quat_trans* data = &track_data[i];
        double_t theta = acos(data->quat.w);
        double_t s = sin(theta);
        if (fabs(s) > 0.000001f) {
            double_t axis_x = data->quat.x / s;
            double_t axis_y = data->quat.y / s;
            double_t axis_z = data->quat.z / s;
            theta *= 2.0;
            if (theta <= 0.0)
                theta += (M_PI * 2.0);
            else
                theta -= (M_PI * 2.0);
            theta /= 2.0;
            double_t cos_val = cos(theta);
            double_t sin_val = sin(theta);
            data->quat.w = (float_t)cos_val;
            data->quat.x = (float_t)(sin_val * axis_x);
            data->quat.y = (float_t)(sin_val * axis_y);
            data->quat.z = (float_t)(sin_val * axis_z);
        }
    }
}

static void enb_plain_anim_get_animation_data(enb_plain_animation* plain_anim,
    quat_trans** track_data, quat_trans_interp_method quat_method, quat_trans_interp_method trans_method) {
    for (int32_t i = 0; i < plain_anim->track_count; i++)
        enb_plain_anim_get_track_data(plain_anim, track_data[i],
            &plain_anim->num_track_data_samples[i], i, quat_method, trans_method);
}

static void enb_plain_anim_get_track_data(enb_plain_animation* plain_anim,
    quat_trans* track_data, int32_t* num_track_data_samples, int32_t track_id,
    quat_trans_interp_method quat_method, quat_trans_interp_method trans_method) {
    int32_t _num_track_data_samples = *num_track_data_samples;

    float_t seconds_per_sample = 1.0f / (float_t)plain_anim->sample_rate;

    int32_t max_samples = (int32_t)(plain_anim->duration / seconds_per_sample) + 2;
    plain_anim->track_data[track_id] = (quat_trans_int*)calloc(max_samples, sizeof(quat_trans_int));

    quat_trans_int* data_int = plain_anim->track_data[track_id];
    enb_plain_anim_get_data(plain_anim, &data_int[0], &track_data[0]);
    *num_track_data_samples = 1;

    for (int32_t i = 1, j = 1; i < max_samples - 1; i++) {
        float_t time = (float_t)i * seconds_per_sample;
        while (time > track_data[j].time)
            if (++j >= _num_track_data_samples) {
                j = _num_track_data_samples - 1;
                break;
            }

        quat_trans* prev = &track_data[j - 1];
        quat_trans* next = &track_data[j];

        float_t blend = (time - prev->time) / (next->time - prev->time);

        quat_trans curr;
        switch (quat_method) {
        case QUAT_TRANS_INTERP_NONE:
            curr.quat = prev->quat;
            break;
        case QUAT_TRANS_INTERP_LERP:
            curr.quat = quat::lerp(prev->quat, next->quat, blend);
            break;
        case QUAT_TRANS_INTERP_SLERP:
            curr.quat = quat::slerp(prev->quat, next->quat, blend);
            break;
        }

        switch (trans_method) {
        case QUAT_TRANS_INTERP_NONE:
            curr.trans = prev->trans;
            break;
        case QUAT_TRANS_INTERP_LERP:
        case QUAT_TRANS_INTERP_SLERP:
            curr.trans = vec3::lerp(prev->trans, next->trans, blend);
            break;
        }

        enb_plain_anim_get_data(plain_anim, &data_int[*num_track_data_samples], &curr);
        (*num_track_data_samples)++;
    }

    enb_plain_anim_get_data(plain_anim, &data_int[*num_track_data_samples],
        &track_data[_num_track_data_samples - 1]);
    data_int[(*num_track_data_samples)++].sample = (int32_t)(plain_anim->duration / seconds_per_sample) + 1;
    plain_anim->data_count += *num_track_data_samples - _num_track_data_samples;
}

static void enb_plain_anim_get_data(enb_plain_animation* plain_anim, quat_trans_int* dst, quat_trans* src) {
    const float_t quantization_error = plain_anim->quantization_error;

    dst->sample = (int32_t)((double_t)plain_anim->sample_rate * src->time + 0.5);

    double_t quat_x = src->quat.x;
    double_t quat_y = src->quat.y;
    double_t quat_z = src->quat.z;
    double_t quat_w = src->quat.w;
    double_t length = 1.0 / sqrt(quat_x * quat_x
        + quat_y * quat_y + quat_z * quat_z + quat_w * quat_w);
    dst->quat.x = (int32_t)(src->quat.x * length / quantization_error);
    dst->quat.y = (int32_t)(src->quat.y * length / quantization_error);
    dst->quat.z = (int32_t)(src->quat.z * length / quantization_error);
    dst->quat.w = (int32_t)(src->quat.w * length / quantization_error);

    for (int32_t i = 4; i < plain_anim->num_components; i++)
        (&dst->quat.x)[i] = (int32_t)((&src->quat.x)[i] / quantization_error);
}

static int32_t enb_plain_anim_get_largest_track_id(enb_plain_animation* plain_anim) {
    int32_t track_id = 0;
    for (int32_t i = 1; i < plain_anim->track_count; i++)
        if (plain_anim->num_track_data_samples[i] > plain_anim->num_track_data_samples[track_id])
            track_id = i;
    return track_id;
}

static int32_t enb_plain_anim_get_num_track_data_samples(enb_plain_animation* plain_anim, int32_t track_id) {
    return plain_anim->num_track_data_samples[track_id];
}

static void enb_plain_anim_get_samples(
    enb_plain_animation* plain_anim, int32_t track_id, enb_anim_track_sample* samples) {
    for (int32_t i = 0; i < 7; i++) {
        int32_t prev_delta = 0;
        int32_t* prev_data = &enb_plain_anim_get_track_data_sample(plain_anim, track_id, 0)->quat.x;
        samples[0].comp[i].value = prev_data[i];
        samples[0].comp[i].has_value = true;
        int32_t num_track_data_samples = enb_plain_anim_get_num_track_data_samples(plain_anim, track_id);
        for (int32_t j = 0; j < num_track_data_samples - 1; j++) {
            int32_t* data = &enb_plain_anim_get_track_data_sample(plain_anim, track_id, j + 1)->quat.x;
            int32_t delta = data[i] - prev_data[i];
            samples[j + 1].comp[i].value = delta - prev_delta;
            samples[j + 1].comp[i].has_value = true;
            prev_data = data;
            prev_delta = delta;
        }
    }
}

static quat_trans_int* enb_plain_anim_get_track_data_sample(
    enb_plain_animation* plain_anim, int32_t track_id, int32_t sample) {
    return &plain_anim->track_data[track_id][sample];
}

static void enb_plain_anim_write_data(enb_plain_animation* plain_anim, int32_t num_tracks,
    int32_t num_components, float_t duration, int32_t sample_rate, float_t quantization_error,
    uint8_t** data_out, size_t* data_out_len) {
    int32_t num_track_data_samples = enb_plain_anim_get_num_track_data_samples(plain_anim,
        enb_plain_anim_get_largest_track_id(plain_anim));
    enb_anim_tracks* tracks = (enb_anim_tracks*)calloc(num_tracks, sizeof(enb_anim_tracks));
    if (!tracks)
        return;

    for (int32_t i = 0; i < num_tracks; i++)
        tracks[i].samples = (enb_anim_track_sample*)calloc(num_track_data_samples, sizeof(enb_anim_track_sample));

    for (int32_t i = 0; i < num_tracks; i++) {
        enb_plain_anim_get_samples(plain_anim, i, tracks[i].samples);
        enb_anim_stream_encoder_find_value_ranges(tracks[i].samples,
            enb_plain_anim_get_num_track_data_samples(plain_anim, i), 9);
    }

    enb_anim_track_stream track_data_stream;
    enb_anim_track_init_stream track_data_init_stream;
    enb_anim_state_stream state_data_stream;
    enb_byte_stream track_flags_byte_stream;
    enb_anim_track_stream_init(&track_data_stream);
    enb_anim_track_init_stream_init(&track_data_init_stream);
    enb_anim_state_stream_init(&state_data_stream);
    enb_byte_stream_init(&track_flags_byte_stream);

    FILE* f = 0;
    enb_anim_stream_encoder_write(tracks, num_tracks, 7, num_track_data_samples,
        &track_data_init_stream, &track_data_stream, &state_data_stream, &track_flags_byte_stream, f);

    enb_byte_stream track_data_init_i2_byte_stream;
    enb_byte_stream track_data_init_i8_byte_stream;
    enb_byte_stream track_data_init_i16_byte_stream;
    enb_byte_stream track_data_init_i32_byte_stream;
    enb_byte_stream track_data_i2_byte_stream;
    enb_byte_stream track_data_i4_byte_stream;
    enb_byte_stream track_data_i8_byte_stream;
    enb_byte_stream track_data_i16_byte_stream;
    enb_byte_stream track_data_i32_byte_stream;
    enb_byte_stream state_data_u2_byte_stream;
    enb_byte_stream state_data_u8_byte_stream;
    enb_byte_stream state_data_u16_byte_stream;
    enb_byte_stream state_data_u32_byte_stream;
    enb_byte_stream_init(&track_data_init_i2_byte_stream);
    enb_byte_stream_init(&track_data_init_i8_byte_stream);
    enb_byte_stream_init(&track_data_init_i16_byte_stream);
    enb_byte_stream_init(&track_data_init_i32_byte_stream);
    enb_byte_stream_init(&track_data_i2_byte_stream);
    enb_byte_stream_init(&track_data_i4_byte_stream);
    enb_byte_stream_init(&track_data_i8_byte_stream);
    enb_byte_stream_init(&track_data_i16_byte_stream);
    enb_byte_stream_init(&track_data_i32_byte_stream);
    enb_byte_stream_init(&state_data_u2_byte_stream);
    enb_byte_stream_init(&state_data_u8_byte_stream);
    enb_byte_stream_init(&state_data_u16_byte_stream);
    enb_byte_stream_init(&state_data_u32_byte_stream);
    enb_anim_track_init_stream_copy_to_byte_stream(
        &track_data_init_stream,
        &track_data_init_i2_byte_stream,
        &track_data_init_i8_byte_stream,
        &track_data_init_i16_byte_stream,
        &track_data_init_i32_byte_stream);
    enb_anim_track_stream_copy_to_byte_stream(
        &track_data_stream,
        &track_data_i2_byte_stream,
        &track_data_i4_byte_stream,
        &track_data_i8_byte_stream,
        &track_data_i16_byte_stream,
        &track_data_i32_byte_stream);
    enb_anim_state_stream_copy_to_byte_stream(
        &state_data_stream,
        &state_data_u2_byte_stream,
        &state_data_u8_byte_stream,
        &state_data_u16_byte_stream,
        &state_data_u32_byte_stream);

    size_t data_size = sizeof(enb_anim_stream);
    data_size += enb_byte_stream_get_size(&track_data_init_i2_byte_stream);
    data_size += enb_byte_stream_get_size(&track_data_init_i8_byte_stream);
    data_size += enb_byte_stream_get_size(&track_data_init_i16_byte_stream);
    data_size += enb_byte_stream_get_size(&track_data_init_i32_byte_stream);
    data_size += enb_byte_stream_get_size(&track_data_i2_byte_stream);
    data_size += enb_byte_stream_get_size(&track_data_i4_byte_stream);
    data_size += enb_byte_stream_get_size(&track_data_i8_byte_stream);
    data_size += enb_byte_stream_get_size(&track_data_i16_byte_stream);
    data_size += enb_byte_stream_get_size(&track_data_i32_byte_stream);
    data_size += enb_byte_stream_get_size(&state_data_u2_byte_stream);
    data_size += enb_byte_stream_get_size(&state_data_u8_byte_stream);
    data_size += enb_byte_stream_get_size(&state_data_u16_byte_stream);
    data_size += enb_byte_stream_get_size(&state_data_u32_byte_stream);
    data_size += enb_byte_stream_get_size(&track_flags_byte_stream);

    *data_out = (uint8_t*)malloc(data_size);
    if (!*data_out)
        return;

    *data_out_len = data_size;

    enb_anim_stream* anim_stream = (enb_anim_stream*)*data_out;
    anim_stream->signature = 0;
    anim_stream->duration = duration;
    anim_stream->sample_rate = sample_rate;
    anim_stream->track_count = num_tracks;
    anim_stream->quantization_error = quantization_error + quantization_error;
    anim_stream->track_data_init_i2_length = (uint32_t)enb_byte_stream_get_size(&track_data_init_i2_byte_stream);
    anim_stream->track_data_init_i8_length = (uint32_t)enb_byte_stream_get_size(&track_data_init_i8_byte_stream);
    anim_stream->track_data_init_i16_length = (uint32_t)enb_byte_stream_get_size(&track_data_init_i16_byte_stream);
    anim_stream->track_data_init_i32_length = (uint32_t)enb_byte_stream_get_size(&track_data_init_i32_byte_stream);
    anim_stream->track_data_i2_length = (uint32_t)enb_byte_stream_get_size(&track_data_i2_byte_stream);
    anim_stream->track_data_i4_length = (uint32_t)enb_byte_stream_get_size(&track_data_i4_byte_stream);
    anim_stream->track_data_i8_length = (uint32_t)enb_byte_stream_get_size(&track_data_i8_byte_stream);
    anim_stream->track_data_i16_length = (uint32_t)enb_byte_stream_get_size(&track_data_i16_byte_stream);
    anim_stream->track_data_i32_length = (uint32_t)enb_byte_stream_get_size(&track_data_i32_byte_stream);
    anim_stream->state_data_u2_length = (uint32_t)enb_byte_stream_get_size(&state_data_u2_byte_stream);
    anim_stream->state_data_u8_length = (uint32_t)enb_byte_stream_get_size(&state_data_u8_byte_stream);
    anim_stream->state_data_u16_length = (uint32_t)enb_byte_stream_get_size(&state_data_u16_byte_stream);
    anim_stream->state_data_u32_length = (uint32_t)enb_byte_stream_get_size(&state_data_u32_byte_stream);
    anim_stream->track_flags_length = (uint32_t)enb_byte_stream_get_size(&track_flags_byte_stream);
    anim_stream->data = (uint32_t)((size_t)*data_out + sizeof(enb_anim_stream));

    if (enb_byte_stream_get_size(&track_data_init_i2_byte_stream))
        memcpy(enb_anim_stream_get_track_data_init_i2(anim_stream),
            enb_byte_stream_get_data(&track_data_init_i2_byte_stream),
            enb_byte_stream_get_size(&track_data_init_i2_byte_stream));

    if (enb_byte_stream_get_size(&track_data_init_i8_byte_stream))
        memcpy(enb_anim_stream_get_track_data_init_i8(anim_stream),
            enb_byte_stream_get_data(&track_data_init_i8_byte_stream),
            enb_byte_stream_get_size(&track_data_init_i8_byte_stream));

    if (enb_byte_stream_get_size(&track_data_init_i16_byte_stream))
        memcpy(enb_anim_stream_get_track_data_init_i16(anim_stream),
            enb_byte_stream_get_data(&track_data_init_i16_byte_stream),
            enb_byte_stream_get_size(&track_data_init_i16_byte_stream));

    if (enb_byte_stream_get_size(&track_data_init_i32_byte_stream))
        memcpy(enb_anim_stream_get_track_data_init_i32(anim_stream),
            enb_byte_stream_get_data(&track_data_init_i32_byte_stream),
            enb_byte_stream_get_size(&track_data_init_i32_byte_stream));

    if (enb_byte_stream_get_size(&track_data_i2_byte_stream))
        memcpy(enb_anim_stream_get_track_data_i2(anim_stream),
            enb_byte_stream_get_data(&track_data_i2_byte_stream),
            enb_byte_stream_get_size(&track_data_i2_byte_stream));

    if (enb_byte_stream_get_size(&track_data_i4_byte_stream))
        memcpy(enb_anim_stream_get_track_data_i4(anim_stream),
            enb_byte_stream_get_data(&track_data_i4_byte_stream),
            enb_byte_stream_get_size(&track_data_i4_byte_stream));

    if (enb_byte_stream_get_size(&track_data_i8_byte_stream))
        memcpy(enb_anim_stream_get_track_data_i8(anim_stream),
            enb_byte_stream_get_data(&track_data_i8_byte_stream),
            enb_byte_stream_get_size(&track_data_i8_byte_stream));

    if (enb_byte_stream_get_size(&track_data_i16_byte_stream))
        memcpy(enb_anim_stream_get_track_data_i16(anim_stream),
            enb_byte_stream_get_data(&track_data_i16_byte_stream),
            enb_byte_stream_get_size(&track_data_i16_byte_stream));

    if (enb_byte_stream_get_size(&track_data_i32_byte_stream))
        memcpy(enb_anim_stream_get_track_data_i32(anim_stream),
            enb_byte_stream_get_data(&track_data_i32_byte_stream),
            enb_byte_stream_get_size(&track_data_i32_byte_stream));

    if (enb_byte_stream_get_size(&state_data_u2_byte_stream))
        memcpy(enb_anim_stream_get_state_data_u2(anim_stream),
            enb_byte_stream_get_data(&state_data_u2_byte_stream),
            enb_byte_stream_get_size(&state_data_u2_byte_stream));

    if (enb_byte_stream_get_size(&state_data_u8_byte_stream))
        memcpy(enb_anim_stream_get_state_data_u8(anim_stream),
            enb_byte_stream_get_data(&state_data_u8_byte_stream),
            enb_byte_stream_get_size(&state_data_u8_byte_stream));

    if (enb_byte_stream_get_size(&state_data_u16_byte_stream))
        memcpy(enb_anim_stream_get_state_data_u16(anim_stream),
            enb_byte_stream_get_data(&state_data_u16_byte_stream),
            enb_byte_stream_get_size(&state_data_u16_byte_stream));

    if (enb_byte_stream_get_size(&state_data_u32_byte_stream))
        memcpy(enb_anim_stream_get_state_data_u32(anim_stream),
            enb_byte_stream_get_data(&state_data_u32_byte_stream),
            enb_byte_stream_get_size(&state_data_u32_byte_stream));

    if (enb_byte_stream_get_size(&track_flags_byte_stream))
        memcpy(enb_anim_stream_get_track_flags(anim_stream),
            enb_byte_stream_get_data(&track_flags_byte_stream),
            enb_byte_stream_get_size(&track_flags_byte_stream));

    for (int32_t i = 0; i < num_tracks; i++)
        if (tracks[i].samples) {
            free(tracks[i].samples);
            tracks[i].samples = 0;
        }

    if (tracks) {
        free(tracks);
        tracks = 0;
    }

    enb_byte_stream_free(&state_data_u32_byte_stream);
    enb_byte_stream_free(&state_data_u16_byte_stream);
    enb_byte_stream_free(&state_data_u8_byte_stream);
    enb_byte_stream_free(&state_data_u2_byte_stream);
    enb_byte_stream_free(&track_data_i32_byte_stream);
    enb_byte_stream_free(&track_data_i16_byte_stream);
    enb_byte_stream_free(&track_data_i8_byte_stream);
    enb_byte_stream_free(&track_data_i4_byte_stream);
    enb_byte_stream_free(&track_data_i2_byte_stream);
    enb_byte_stream_free(&track_data_init_i32_byte_stream);
    enb_byte_stream_free(&track_data_init_i16_byte_stream);
    enb_byte_stream_free(&track_data_init_i8_byte_stream);
    enb_byte_stream_free(&track_data_init_i2_byte_stream);
    enb_byte_stream_free(&track_flags_byte_stream);
    enb_anim_state_stream_free(&state_data_stream);
    enb_anim_track_init_stream_free(&track_data_init_stream);
    enb_anim_track_stream_free(&track_data_stream);
}

static void enb_plain_anim_free(enb_plain_animation* plain_anim) {
    for (int32_t i = 0; i < 300; i++)
        if (plain_anim->track_data[i]) {
            free(plain_anim->track_data[i]);
            plain_anim->track_data[i] = 0;
        }
}

static void enb_stream_init(enb_stream* stream) {
    stream->data = 0;
    stream->size = 0;
    stream->init = true;
}

static void enb_stream_reset(enb_stream* stream) {
    if (stream->init && stream->data) {
        free(stream->data);
        stream->data = 0;
    }

    stream->size = 0;
}

static void enb_stream_free(enb_stream* stream) {
    enb_stream_reset(stream);
}

static void enb_byte_stream_alloc(enb_byte_stream* byte_stream, size_t size) {
    byte_stream->base.data = (uint8_t*)calloc(size, sizeof(uint8_t));
    byte_stream->base.size = size;
    byte_stream->base.init = true;
}

static void enb_byte_stream_init(enb_byte_stream* byte_stream) {
    enb_stream_init(&byte_stream->base);
}

static uint8_t* enb_byte_stream_get_data(enb_byte_stream* byte_stream) {
    return byte_stream->base.data;
}

static size_t enb_byte_stream_get_size(enb_byte_stream* byte_stream) {
    return byte_stream->base.size;
}

static void enb_byte_stream_free(enb_byte_stream* byte_stream) {
    enb_stream_free(&byte_stream->base);
}

static void enb_byte_stream_set_data(enb_byte_stream* byte_stream, uint8_t* value) {
    byte_stream->base.data = value;
}

static void enb_byte_stream_set_size(enb_byte_stream* byte_stream, size_t value) {
    byte_stream->base.size = value;
}

static void enb_octet_stream_alloc(enb_octet_stream* octet_stream) {
    if (octet_stream->size) {
        enb_byte_stream_set_size(octet_stream->byte_stream, octet_stream->size);
        octet_stream->stream_count++;

        enb_byte_stream** stream_list = (enb_byte_stream**)realloc(octet_stream->stream_list,
            sizeof(enb_byte_stream*) * octet_stream->stream_count);
        if (!stream_list)
            return;

        octet_stream->stream_list = stream_list;
        octet_stream->stream_list[octet_stream->stream_count - 1] = octet_stream->byte_stream;
        octet_stream->total_size += octet_stream->size;
    }
    else if (octet_stream->byte_stream)
        return;

    octet_stream->byte_stream = (enb_byte_stream*)malloc(sizeof(enb_byte_stream));
    enb_byte_stream_alloc(octet_stream->byte_stream, 0x1000);
    octet_stream->data = enb_byte_stream_get_data(octet_stream->byte_stream);
    octet_stream->size = 0;
}

static void enb_octet_stream_copy_to_byte_stream(enb_octet_stream* octet_stream, enb_byte_stream* byte_stream) {
    enb_octet_stream_alloc(octet_stream);
    enb_byte_stream_alloc(byte_stream, octet_stream->total_size);
    enb_byte_stream_set_size(byte_stream, octet_stream->total_size);

    uint8_t* data = enb_byte_stream_get_data(byte_stream);
    for (int32_t i = 0; i < octet_stream->stream_count; i++) {
        size_t size = enb_byte_stream_get_size(octet_stream->stream_list[i]);
        memcpy(data, enb_byte_stream_get_data(octet_stream->stream_list[i]), size);
        data += size;
    }
}

static void enb_octet_stream_init(enb_octet_stream* octet_stream) {
    octet_stream->stream_list = 0;
    octet_stream->stream_count = 0;
    octet_stream->total_size = 0;
    octet_stream->data = 0;
    octet_stream->size = 0;
    octet_stream->byte_stream = 0;

    enb_octet_stream_alloc(octet_stream);
}

static void enb_octet_stream_put_i8(enb_octet_stream* octet_stream, int8_t value) {
    if (octet_stream->size + sizeof(int8_t) >= 0x1000)
        enb_octet_stream_alloc(octet_stream);

    *(int8_t*)octet_stream->data = value;
    octet_stream->data += sizeof(int8_t);
    octet_stream->size += sizeof(int8_t);
}

static void enb_octet_stream_put_u8(enb_octet_stream* octet_stream, uint8_t value) {
    if (octet_stream->size + sizeof(uint8_t) >= 0x1000)
        enb_octet_stream_alloc(octet_stream);

    *(uint8_t*)octet_stream->data = value;
    octet_stream->data += sizeof(uint8_t);
    octet_stream->size += sizeof(uint8_t);
}

static void enb_octet_stream_put_i16(enb_octet_stream* octet_stream, int16_t value) {
    if (octet_stream->size + sizeof(int16_t) >= 0x1000)
        enb_octet_stream_alloc(octet_stream);

    *(int16_t*)octet_stream->data = value;
    octet_stream->data += sizeof(int16_t);
    octet_stream->size += sizeof(int16_t);
}

static void enb_octet_stream_put_u16(enb_octet_stream* octet_stream, uint16_t value) {
    if (octet_stream->size + sizeof(uint16_t) >= 0x1000)
        enb_octet_stream_alloc(octet_stream);

    *(uint16_t*)octet_stream->data = value;
    octet_stream->data += sizeof(uint16_t);
    octet_stream->size += sizeof(uint16_t);
}

static void enb_octet_stream_put_i32(enb_octet_stream* octet_stream, int32_t value) {
    if (octet_stream->size + sizeof(int32_t) >= 0x1000)
        enb_octet_stream_alloc(octet_stream);

    *(int32_t*)octet_stream->data = value;
    octet_stream->data += sizeof(int32_t);
    octet_stream->size += sizeof(int32_t);
}

static void enb_octet_stream_put_u32(enb_octet_stream* octet_stream, uint32_t value) {
    if (octet_stream->size + sizeof(uint32_t) >= 0x1000)
        enb_octet_stream_alloc(octet_stream);

    *(uint32_t*)octet_stream->data = value;
    octet_stream->data += sizeof(uint32_t);
    octet_stream->size += sizeof(uint32_t);
}

static void enb_octet_stream_reset(enb_octet_stream* octet_stream) {
    for (int32_t i = 0; i < octet_stream->stream_count; i++) {
        if (octet_stream->stream_list[i]) {
            enb_byte_stream_free(octet_stream->stream_list[i]);
            free(octet_stream->stream_list[i]);
            octet_stream->byte_stream = 0;
        }
        octet_stream->stream_list[i] = 0;
    }
    octet_stream->stream_count = 0;

    if (octet_stream->byte_stream) {
        enb_byte_stream_free(octet_stream->byte_stream);
        free(octet_stream->byte_stream);
        octet_stream->byte_stream = 0;
    }

    octet_stream->total_size = 0;
    octet_stream->data = 0;
    octet_stream->size = 0;

    enb_octet_stream_alloc(octet_stream);
}

static void enb_octet_stream_free(enb_octet_stream* octet_stream) {
    enb_octet_stream_reset(octet_stream);

    if (octet_stream->byte_stream) {
        enb_byte_stream_free(octet_stream->byte_stream);
        free(octet_stream->byte_stream);
        octet_stream->byte_stream = 0;
    }
}

static void enb_bit_octet_stream_init(enb_bit_octet_stream* bit_octet_stream) {
    enb_octet_stream_init(&bit_octet_stream->base);
    bit_octet_stream->u2_counter = 0;
    bit_octet_stream->u4_counter = 0;
    bit_octet_stream->temp = 0;
}

static void enb_bit_octet_stream_copy_to_byte_stream(
    enb_bit_octet_stream* bit_octet_stream, enb_byte_stream* byte_stream) {
    if (bit_octet_stream->u2_counter || bit_octet_stream->u4_counter)
        enb_octet_stream_put_u8(&bit_octet_stream->base, bit_octet_stream->temp);
    enb_octet_stream_copy_to_byte_stream(&bit_octet_stream->base, byte_stream);
}

static void enb_bit_octet_stream_put_u2(enb_bit_octet_stream* bit_octet_stream, uint8_t value) {
    bit_octet_stream->temp |= (value & 0x03) << (2 * (4 - bit_octet_stream->u2_counter++) - 2);
    if (bit_octet_stream->u2_counter == 4) {
        enb_octet_stream_put_u8(&bit_octet_stream->base, bit_octet_stream->temp);
        bit_octet_stream->temp = 0;
        bit_octet_stream->u2_counter = 0;
    }
}

static void enb_bit_octet_stream_put_u4(enb_bit_octet_stream* bit_octet_stream, uint8_t value) {
    bit_octet_stream->temp |= (value & 0x0F) << (4 * (2 - bit_octet_stream->u2_counter++) - 4);
    if (bit_octet_stream->u2_counter == 2) {
        enb_octet_stream_put_u8(&bit_octet_stream->base, bit_octet_stream->temp);
        bit_octet_stream->temp = 0;
        bit_octet_stream->u2_counter = 0;
    }
}

static void enb_bit_octet_stream_free(enb_bit_octet_stream* bit_octet_stream) {
    enb_octet_stream_free(&bit_octet_stream->base);
}

static void enb_anim_track_init_stream_init(enb_anim_track_init_stream* track_init_stream) {
    enb_bit_octet_stream_init(&track_init_stream->i2_stream);
    enb_octet_stream_init(&track_init_stream->i8_stream);
    enb_octet_stream_init(&track_init_stream->i16_stream);
    enb_octet_stream_init(&track_init_stream->i32_stream);
}

static void enb_anim_track_init_stream_copy_to_byte_stream(enb_anim_track_init_stream* track_init_stream,
    enb_byte_stream* i2_stream, enb_byte_stream* i8_stream,
    enb_byte_stream* i16_stream, enb_byte_stream* i32_stream) {
    enb_bit_octet_stream_copy_to_byte_stream(&track_init_stream->i2_stream, i2_stream);
    enb_octet_stream_copy_to_byte_stream(&track_init_stream->i8_stream, i8_stream);
    enb_octet_stream_copy_to_byte_stream(&track_init_stream->i16_stream, i16_stream);
    enb_octet_stream_copy_to_byte_stream(&track_init_stream->i32_stream, i32_stream);
}

static void enb_anim_track_init_stream_put_value(enb_anim_track_init_stream* track_init_stream, int32_t value) {
    if (!value)
        enb_bit_octet_stream_put_u2(&track_init_stream->i2_stream, 0x00);
    else if (value >= -0x80 && value <= 0x7F) {
        enb_bit_octet_stream_put_u2(&track_init_stream->i2_stream, 0x01);
        enb_octet_stream_put_i8(&track_init_stream->i8_stream, (int8_t)value);
    }
    else if (value >= -0x8000 && value <= 0x7FFF) {
        enb_bit_octet_stream_put_u2(&track_init_stream->i2_stream, 0x02);
        enb_octet_stream_put_i16(&track_init_stream->i16_stream, (int16_t)value);
    }
    else {
        enb_bit_octet_stream_put_u2(&track_init_stream->i2_stream, 0x03);
        enb_octet_stream_put_i32(&track_init_stream->i32_stream, value);
    }
}

static void enb_anim_track_init_stream_free(enb_anim_track_init_stream* track_init_stream) {
    enb_octet_stream_free(&track_init_stream->i32_stream);
    enb_octet_stream_free(&track_init_stream->i16_stream);
    enb_octet_stream_free(&track_init_stream->i8_stream);
    enb_bit_octet_stream_free(&track_init_stream->i2_stream);
}

static void enb_anim_track_stream_init(enb_anim_track_stream* track_stream) {
    enb_bit_octet_stream_init(&track_stream->i2_stream);
    enb_bit_octet_stream_init(&track_stream->i4_stream);
    enb_octet_stream_init(&track_stream->i8_stream);
    enb_octet_stream_init(&track_stream->i16_stream);
    enb_octet_stream_init(&track_stream->i32_stream);
}

static void enb_anim_track_stream_copy_to_byte_stream(enb_anim_track_stream* track_stream,
    enb_byte_stream* i2_stream, enb_byte_stream* i4_stream, enb_byte_stream* i8_stream,
    enb_byte_stream* i16_stream, enb_byte_stream* i32_stream) {
    enb_bit_octet_stream_copy_to_byte_stream(&track_stream->i2_stream, i2_stream);
    enb_bit_octet_stream_copy_to_byte_stream(&track_stream->i4_stream, i4_stream);
    enb_octet_stream_copy_to_byte_stream(&track_stream->i8_stream, i8_stream);
    enb_octet_stream_copy_to_byte_stream(&track_stream->i16_stream, i16_stream);
    enb_octet_stream_copy_to_byte_stream(&track_stream->i32_stream, i32_stream);
}

static void enb_anim_track_stream_put_value(enb_anim_track_stream* track_stream, int32_t value) {
    if (value >= -0x01 && value <= 0x01) {
        enb_bit_octet_stream_put_u2(&track_stream->i2_stream, (uint8_t)value);
        return;
    }

    enb_bit_octet_stream_put_u2(&track_stream->i2_stream, 0x02);
    if (value >= -0x09 && value <= 0x08) {
        if (value == -0x09)
            enb_bit_octet_stream_put_u4(&track_stream->i4_stream, 0xFF);
        else if (value == 0x08)
            enb_bit_octet_stream_put_u4(&track_stream->i4_stream, 0x01);
        else
            enb_bit_octet_stream_put_u4(&track_stream->i4_stream, (uint8_t)value);
        return;
    }

    enb_bit_octet_stream_put_u4(&track_stream->i4_stream, 0x00);
    if (value >= -0x80 - 0x08 && value <= 0x7F + 0x08) {
        if (value > 0x7F)
            enb_octet_stream_put_i8(&track_stream->i8_stream, (int8_t)(value - 0x7F));
        else if (value < -0x80)
            enb_octet_stream_put_i8(&track_stream->i8_stream, (int8_t)(value + 0x80));
        else
            enb_octet_stream_put_i8(&track_stream->i8_stream, (int8_t)value);
        return;
    }

    enb_octet_stream_put_i8(&track_stream->i8_stream, 0x00);
    if (value >= -0x8000 && value <= 0x7FFF) {
        enb_octet_stream_put_i16(&track_stream->i16_stream, value);
        return;
    }

    enb_octet_stream_put_i16(&track_stream->i16_stream, 0x00);
    enb_octet_stream_put_i32(&track_stream->i32_stream, value);
}

static void enb_anim_track_stream_free(enb_anim_track_stream* track_stream) {
    enb_octet_stream_free(&track_stream->i32_stream);
    enb_octet_stream_free(&track_stream->i16_stream);
    enb_octet_stream_free(&track_stream->i8_stream);
    enb_bit_octet_stream_free(&track_stream->i4_stream);
    enb_bit_octet_stream_free(&track_stream->i2_stream);
}

static void enb_anim_state_stream_init(enb_anim_state_stream* state_stream) {
    enb_bit_octet_stream_init(&state_stream->u2_stream);
    enb_octet_stream_init(&state_stream->u8_stream);
    enb_octet_stream_init(&state_stream->u16_stream);
    enb_octet_stream_init(&state_stream->u32_stream);
}

static void enb_anim_state_stream_copy_to_byte_stream(enb_anim_state_stream* state_stream,
    enb_byte_stream* u2_stream, enb_byte_stream* u8_stream,
    enb_byte_stream* u16_stream, enb_byte_stream* u32_stream) {
    enb_bit_octet_stream_copy_to_byte_stream(&state_stream->u2_stream, u2_stream);
    enb_octet_stream_copy_to_byte_stream(&state_stream->u8_stream, u8_stream);
    enb_octet_stream_copy_to_byte_stream(&state_stream->u16_stream, u16_stream);
    enb_octet_stream_copy_to_byte_stream(&state_stream->u32_stream, u32_stream);
}

static void enb_anim_state_stream_put_value(enb_anim_state_stream* state_stream, uint32_t value) {
    if (!value)
        enb_bit_octet_stream_put_u2(&state_stream->u2_stream, 0x00);
    else if (value <= 0xFF) {
        enb_bit_octet_stream_put_u2(&state_stream->u2_stream, 0x01);
        enb_octet_stream_put_u8(&state_stream->u8_stream, (uint8_t)value);
    }
    else if (value <= 0xFFFF) {
        enb_bit_octet_stream_put_u2(&state_stream->u2_stream, 0x02);
        enb_octet_stream_put_u16(&state_stream->u16_stream, (uint16_t)value);
    }
    else {
        enb_bit_octet_stream_put_u2(&state_stream->u2_stream, 0x03);
        enb_octet_stream_put_u32(&state_stream->u32_stream, value);
    }
}

static void enb_anim_state_stream_free(enb_anim_state_stream* state_stream) {
    enb_octet_stream_free(&state_stream->u32_stream);
    enb_octet_stream_free(&state_stream->u16_stream);
    enb_octet_stream_free(&state_stream->u8_stream);
    enb_bit_octet_stream_free(&state_stream->u2_stream);
}
