/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "enbaya.hpp"

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

static const int32_t shift_table_track_data_i2[] = { 6, 4, 2, 0 };      // 0x08BF1CE8
static const int32_t shift_table_track_data_i4[] = { 4, 0 };            // 0x08BF1CF8
static const int32_t shift_table_track_data_init_i2[] = { 6, 4, 2, 0 }; // 0x08BF2160
static const int32_t shift_table_state_data_u2[] = { 6, 4, 2, 0 };      // 0x08BF2170
static const int32_t value_table_track_data_i2[] = { 0, 1, 0, -1 };     // 0x08BB3FC0
static const int32_t value_table_track_data_i4[] = { 0, 8, 2, 3, 4, 5, 6, 7, -8, -7, -6, -5, -4, -3, -2, -9 }; // 0x08BB3FD0

int32_t enb_process(uint8_t* data_in, uint8_t** data_out, size_t* data_out_len,
    float_t* duration, float_t* fps, int32_t* frames, quat_trans_interp_method method) {
    enb_anim_context* anim_ctx;
    enb_anim_stream* anim_stream;
    quat_trans* qt_data;
    int32_t code, i, j;

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

    code = enb_initialize(data_in, &anim_ctx);
    if (code) {
        free_def(*data_out);
        return code - 0x10;
    }

    anim_stream = (enb_anim_stream*)data_in;
    *duration = anim_stream->duration;

    if (*fps > 600.0f)
        *fps = 600.0f;
    else if (*fps < (float_t)anim_stream->sample_rate)
        *fps = (float_t)anim_stream->sample_rate;

    float_t frames_float = *duration * *fps;
    *frames = (int32_t)(int64_t)frames_float + (fmodf(frames_float, 1.0f) >= 0.5f) + 1;
    if (*frames > 0x7FFFFFFFU)
        return -7;

    *data_out_len = sizeof(quat_trans) * anim_stream->track_count * *frames + 0x10;
    *data_out = force_malloc<uint8_t>(*data_out_len);

    if (!*data_out)
        return -8;

    memset(*data_out, 0, *data_out_len);

    ((int32_t*)*data_out)[0] = anim_stream->track_count;
    ((int32_t*)*data_out)[1] = *frames;
    ((float_t*)*data_out)[2] = *fps;
    ((float_t*)*data_out)[3] = *duration;

    qt_data = (quat_trans*)(*data_out + 0x10);
    for (i = 0; i < *frames; i++) {
        float_t time = (float_t)i / *fps;
        for (j = 0; j < anim_stream->track_count; j++, qt_data++)
            enb_get_component_values(anim_ctx, time, j, qt_data, method);
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

void enb_get_component_values(enb_anim_context* anim_ctx, float_t time,
    int32_t track_id, quat_trans* data, quat_trans_interp_method method) {
    quat_trans* prev = 0;
    quat_trans* next = 0;
    enb_get_track_data(anim_ctx, track_id, &prev, &next, time);

    float_t blend = (time - prev->time) / anim_ctx->seconds_per_sample;
    interp_quat_trans(prev, next, data, blend, method);
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
    uint8_t* data;
    uint32_t temp;

    data = (uint8_t*)anim_stream;
    anim_ctx->data.current_sample = -1;
    anim_ctx->data.current_sample_time = -1.0f;
    anim_ctx->data.previous_sample_time = -1.0f;
    anim_ctx->requested_time = -1.0f;
    anim_ctx->seconds_per_sample = 1.0f / (float_t)anim_stream->sample_rate;
    anim_ctx->track_direction = 0;

    temp = 0x50;
    anim_ctx->track_data_init.i32 = (int32_t*)(data + temp);

    temp += anim_stream->track_data_init_i32_length;
    anim_ctx->track_data.i32 = (int32_t*)(data + temp);

    temp += anim_stream->track_data_i32_length;
    anim_ctx->state_data.u32 = (uint32_t*)(data + temp);

    temp += anim_stream->state_data_u32_length;
    anim_ctx->track_data_init.i16 = (int16_t*)(data + temp);

    temp += anim_stream->track_data_init_i16_length;
    anim_ctx->track_data.i16 = (int16_t*)(data + temp);

    temp += anim_stream->track_data_i16_length;
    anim_ctx->state_data.u16 = (uint16_t*)(data + temp);

    temp += anim_stream->state_data_u16_length;
    anim_ctx->track_data_init.i2 = data + temp;

    temp += anim_stream->track_data_init_i2_length;
    anim_ctx->track_data_init.i8 = (int8_t*)(data + temp);

    temp += anim_stream->track_data_init_i8_length;
    anim_ctx->track_data.i2 = data + temp;

    temp += anim_stream->track_data_i2_length;
    anim_ctx->track_data.i4 = data + temp;

    temp += anim_stream->track_data_i4_length;
    anim_ctx->track_data.i8 = (int8_t*)(data + temp);

    temp += anim_stream->track_data_i8_length;
    anim_ctx->state_data.u2 = data + temp;

    temp += anim_stream->state_data_u2_length;
    anim_ctx->state_data.u8 = data + temp;

    temp += anim_stream->state_data_u8_length;
    anim_ctx->track_flags = data + temp;

    temp += anim_stream->track_flags_length;
    anim_ctx->data.data_length = temp;

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
    uint32_t track_count;
    float_t quantization_error;
    float_t requested_time;
    float_t sps; // seconds per sample

    if (time == anim_ctx->requested_time)
        return;

    track_count = anim_ctx->data.stream->track_count;
    quantization_error = anim_ctx->data.stream->quantization_error;
    requested_time = anim_ctx->requested_time;
    sps = anim_ctx->seconds_per_sample;

    if ((requested_time == -1.0f) || (0.000001f > time) || (requested_time - time > time)
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
        requested_time = ++anim_ctx->data.current_sample * sps;

        if (anim_ctx->data.stream->duration <= requested_time)
            requested_time = anim_ctx->data.stream->duration;

        enb_track_apply(anim_ctx, track_count, true, quantization_error, requested_time);
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
    int32_t i, j, val;

    enb_track* track = anim_ctx->data.track;

    for (i = 0; i < track_count; i++, track++) {
        for (j = 0; j < 7; j++) {
            val = enb_anim_track_data_init_decode(track_data_init);

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
    int32_t i, j, val;

    enb_track* track = anim_ctx->data.track;

    for (i = 0; i < track_count; i++, track++) {
        if (track->flags == 0)
            continue;

        for (j = 0; j < 7; j++) {
            if ((track->flags & (1 << j)) == 0)
                continue;

            val = enb_anim_track_data_forward_decode(track_data);

            switch (j) {
            case 0:
                track->quat.x += val;
                break;
            case 1:
                track->quat.y += val;
                break;
            case 2:
                track->quat.z += val;
                break;
            case 3:
                track->quat.w += val;
                break;
            case 4:
                track->trans.x += val;
                break;
            case 5:
                track->trans.y += val;
                break;
            case 6:
                track->trans.z += val;
                break;
            }
        }
    }
}


static void enb_track_step_backward(enb_anim_context* anim_ctx,
    const int32_t track_count, enb_anim_track_data_decoder* track_data) { // 0x08A090A0 in ULJM05681
    int32_t i, j, val;

    enb_track* track = anim_ctx->data.track;

    track += track_count - 1LL;
    for (i = track_count - 1; i != -1; i--, track--) {
        if (track->flags == 0)
            continue;

        for (j = 0; j < 7; j++) {
            if ((track->flags & (1 << (6 - j))) == 0)
                continue;

            val = enb_anim_track_data_backward_decode(track_data);

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
    int32_t i, j, temp, track_comps_count;

    track_comps_count = track_count * 7;
    i = 0;
    while (i < track_comps_count) {
        j = state->next_step;
        if (j == 0) {
            track[i / 7].flags ^= 0x01 << (i % 7);
            state->next_step = enb_anim_state_data_forward_decode(state_data);
            state->prev_step = 0;
            i++;
        }
        else {
            temp = j < (track_comps_count - i) ? j : (track_comps_count - i);
            i += temp;
            state->next_step -= temp;
            state->prev_step += temp;
        }
    }
}

static void enb_state_step_backward(enb_anim_state* state,
    enb_track* track, const int32_t track_count, enb_anim_state_data_decoder* state_data) { // 0x08A0968C in ULJM05681
    int32_t i, j, temp, track_comps_count;

    track_comps_count = track_count * 7;
    i = track_comps_count - 1;
    while (i != -1) {
        j = state->prev_step;
        if (j == 0) {
            track[i / 7].flags ^= 0x01 << (i % 7);
            state->next_step = 0;
            state->prev_step = enb_anim_state_data_backward_decode(state_data);
            i--;
        }
        else {
            temp = j < (i + 1) ? j : (i + 1);
            i -= temp;
            state->next_step += temp;
            state->prev_step -= temp;
        }
    }
}

static void enb_track_init_apply(enb_anim_context* anim_ctx,
    const int32_t track_count, const uint8_t* flags, float_t quantization_error) { // 0x08A086CC in ULJM05681
    int32_t i;
    quat quat_delta, quat_result;
    vec3 trans_delta, trans_result;

    enb_track* track = anim_ctx->data.track;

    for (i = 0; i < track_count; i++, track++) {
        quat_delta = track->quat;
        trans_delta = track->trans;

        quat_result = quat_delta * quantization_error;
        trans_result = trans_delta * quantization_error;

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
    uint8_t s0, s1;
    int32_t i;
    quat quat_delta, quat_result, quat_data;
    vec3 trans_delta, trans_result, trans_data;

    enb_track* track = anim_ctx->data.track;

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

    for (i = 0; i < track_count; i++, track++) {
        quat_delta = track->quat;
        trans_delta = track->trans;

        quat_data = track->qt[s0].quat;
        trans_data = track->qt[s0].trans;

        quat_result = quat_delta * quantization_error + quat_data;
        trans_result = trans_delta * quantization_error + trans_data;

        quat_result = quat::normalize_rcp(quat_result);

        track->qt[s1].quat = quat_result;
        track->qt[s1].trans = trans_result;
        track->qt[s1].time = time;
    }
}

inline static int32_t enb_anim_track_data_init_decode(enb_anim_track_data_init_decoder* track_data_init) {
    int32_t val;

    if (track_data_init->i2_counter == 4) {
        track_data_init->i2_counter = 0;
        track_data_init->i2++;
    }

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
    int32_t val;

    if (track_data->i2_counter == 4) {
        track_data->i2_counter = 0;
        track_data->i2++;
    }

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
    int32_t val;

    if (--track_data->i2_counter == -1) {
        track_data->i2_counter = 3;
        track_data->i2--;
    }

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
    uint32_t val;

    if (state_data->u2_counter == 4) {
        state_data->u2_counter = 0;
        state_data->u2++;
    }

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
    uint32_t val;

    if (--state_data->u2_counter == -1) {
        state_data->u2_counter = 3;
        state_data->u2--;
    }

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
