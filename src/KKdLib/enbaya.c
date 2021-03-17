/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "enbaya.h"

static void enb_init(enb_play_head* play_head, enb_head* head);
static void enb_copy_pointers(enb_play_head* play_head);
static void enb_get_track_unscaled_init(enb_play_head* play_head);
static void enb_get_track_unscaled_forward(enb_play_head* play_head);
static void enb_get_track_unscaled_backward(enb_play_head* play_head);
static void enb_calc_params_init(enb_play_head* play_head);
static void enb_calc_params_forward(enb_play_head* play_head);
static void enb_calc_params_backward(enb_play_head* play_head);
static void enb_calc_track_init(enb_play_head* play_head);
static void enb_calc_track(enb_play_head* play_head, float_t time, bool forward);

int32_t enb_process(uint8_t* data_in, uint8_t** data_out,
    size_t* data_out_len, float_t* duration, float_t* fps, size_t* frames) {
    enb_play_head* play_head;
    enb_head* head;
    quat_trans* qt_data;
    size_t i, j;
    int32_t code;

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

    code = enb_initialize(data_in, &play_head);
    if (code) {
        free(*data_out);
        return code - 0x10;
    }

    head = (enb_head*)data_in;
    *duration = head->duration;

    if (*fps > 600.0f)
        *fps = 600.0f;
    else if (*fps < (float_t)head->samples)
        *fps = (float_t)head->samples;

    float_t frames_float = *duration * *fps;
    *frames = (size_t)frames_float + (fmodf(frames_float, 1.0f) >= 0.5f) + 1;
    if (*frames > 0x7FFFFFFFU)
        return -7;

    *data_out_len = sizeof(quat_trans) * head->track_count * *frames + 0x10;
    *data_out = force_malloc(*data_out_len);

    if (!*data_out)
        return -8;

    memset(*data_out, 0, *data_out_len);

    ((uint32_t*)*data_out)[0] = head->track_count;
    ((uint32_t*)*data_out)[1] = *(uint32_t*)frames;
    ((float_t*)*data_out)[2] = *fps;
    ((float_t*)*data_out)[3] = *duration;

    qt_data = (quat_trans*)(*data_out + 0x10);
    for (i = 0; i < *frames; i++) {
        enb_set_time(play_head, (float_t)i / *fps);

        for (j = 0; j < head->track_count; j++, qt_data++)
            enb_get_track_data(play_head, j, qt_data);
    }
    enb_free(&play_head);
    return 0;
}

int32_t enb_initialize(uint8_t* data, enb_play_head** play_head) {
    if (!data)
        return -1;
    else if (!play_head)
        return -2;
    *play_head = 0;

    enb_head* head = (enb_head*)data;
    if (head->signature != 0x100A9DA4 && head->signature != 0x100AAD74)
        return -3;

    enb_play_head* ph = force_malloc(sizeof(enb_play_head));
    if (!ph)
        return -4;

    memset(ph, 0, sizeof(enb_play_head));

    ph->data_header = head;
    enb_init(ph, head);

    ph->track_data = force_malloc_s(sizeof(enb_track), head->track_count);

    if (!ph->track_data) {
        free(ph);
        return -5;
    }

    memset(ph->track_data, 0, sizeof(enb_track) * head->track_count);

    *play_head = ph;
    return 0;
}

void enb_free(enb_play_head** play_head) {
    if (!play_head || !*play_head)
        return;

    free((*play_head)->track_data);
    free(*play_head);
    *play_head = 0;
}

void enb_get_track_data(enb_play_head* play_head, size_t track, quat_trans* data) {
    if (!data)
        return;
    else if (!play_head || play_head->data_header->track_count < track) {
        *data = quat_trans_identity;
        return;
    }

    if (play_head->track_mode_selector) {
        quat_trans* qt1 = play_head->track_data[track].qt + (play_head->track_data_selector & 0x1);
        quat_trans* qt2 = play_head->track_data[track].qt + ((play_head->track_data_selector & 0x1) ^ 1);
        float_t blend = (play_head->requested_time - play_head->previous_sample_time)
            / (play_head->current_sample_time - play_head->previous_sample_time);
        lerp_quat_trans(qt1, qt2, data, blend);
    }
    else
        *data = *play_head->track_data[track].qt;
}

static void enb_init(enb_play_head* play_head, enb_head* head) { // 0x08A08050 in ULJM05681
    uint8_t* data;
    uint32_t temp;

    data = (uint8_t*)head;
    play_head->current_sample = -1;
    play_head->current_sample_time = -1.0f;
    play_head->previous_sample_time = -1.0f;
    play_head->requested_time = -1.0f;
    play_head->seconds_per_sample = 1.0f / (float_t)head->samples;
    play_head->track_mode_selector = 0;

    temp = 0x50;
    play_head->orig_track_data_init_i32 = (int32_t*)(data + temp);

    temp += head->track_data_init_i32_length;
    play_head->orig_track_data_i32 = (int32_t*)(data + temp);

    temp += head->track_data_i32_length;
    play_head->orig_params_u32 = (uint32_t*)(data + temp);

    temp += head->params_u32_length;
    play_head->orig_track_data_init_i16 = (int16_t*)(data + temp);

    temp += head->track_data_init_i16_length;
    play_head->orig_track_data_i16 = (int16_t*)(data + temp);

    temp += head->track_data_i16_length;
    play_head->orig_params_u16 = (uint16_t*)(data + temp);

    temp += head->params_u16_length;
    play_head->orig_track_data_init_mode = data + temp;

    temp += head->track_data_init_mode_length;
    play_head->orig_track_data_init_i8 = (int8_t*)(data + temp);

    temp += head->track_data_init_i8_length;
    play_head->orig_track_data_mode = data + temp;

    temp += head->track_data_mode_length;
    play_head->orig_track_data_mode2 = data + temp;

    temp += head->track_data_mode2_length;
    play_head->orig_track_data_i8 = (int8_t*)(data + temp);

    temp += head->track_data_i8_length;
    play_head->orig_params_mode = data + temp;

    temp += head->params_mode_length;
    play_head->orig_params_u8 = data + temp;

    temp += head->params_u8_length;
    play_head->track_flags = data + temp;

    temp += head->track_flags_length;
    play_head->data_length = temp;

    enb_copy_pointers(play_head);
}

static void enb_copy_pointers(enb_play_head* play_head) { // 0x08A07FD0 in ULJM05681
    play_head->track_data_init_mode = play_head->orig_track_data_init_mode;
    play_head->track_data_init_i8 = play_head->orig_track_data_init_i8;
    play_head->track_data_init_i16 = play_head->orig_track_data_init_i16;
    play_head->track_data_init_i32 = play_head->orig_track_data_init_i32;
    play_head->track_data_init_counter = 0;

    play_head->track_data_mode = play_head->orig_track_data_mode;
    play_head->track_data_mode2 = play_head->orig_track_data_mode2;
    play_head->track_data_i8 = play_head->orig_track_data_i8;
    play_head->track_data_i16 = play_head->orig_track_data_i16;
    play_head->track_data_i32 = play_head->orig_track_data_i32;
    play_head->track_data_mode_counter = 0;
    play_head->track_data_mode2_counter = 0;

    play_head->params_mode = play_head->orig_params_mode;
    play_head->params_u8 = play_head->orig_params_u8;
    play_head->params_u16 = play_head->orig_params_u16;
    play_head->params_u32 = play_head->orig_params_u32;
    play_head->params_counter = 0;
}

void enb_set_time(enb_play_head* play_head, float_t time) { // 0x08A0876C in ULJM05681
    float_t requested_time;
    float_t sps; // seconds per sample
    uint32_t mode;

    if (time == play_head->requested_time)
        return;

    requested_time = play_head->requested_time;
    sps = play_head->seconds_per_sample;

    if ((requested_time == -1.0f) || (0.000001f > time) || (requested_time - time > time)
        || ((sps <= requested_time) && (sps > time))) {
        play_head->current_sample = 0;
        play_head->current_sample_time = 0.0f;
        play_head->previous_sample_time = 0.0f;
        play_head->track_mode_selector = 0;

        enb_copy_pointers(play_head);
        enb_calc_params_init(play_head);
        enb_get_track_unscaled_init(play_head);
        enb_calc_track_init(play_head);
    }

    play_head->requested_time = time;
    if (time < 0.000001f)
        return;

    while ((time > play_head->current_sample_time)
        && (play_head->data_header->duration - play_head->current_sample_time > 0.00001f)) {
        if (play_head->track_mode_selector == 2) {
            if (play_head->params_counter == 4) {
                play_head->params_counter = 0;
                play_head->params_mode++;
            }

            mode = *play_head->params_mode >> shift_table_1[play_head->params_counter++];
            mode &= 0x3;

            switch (mode) {
            case 1:
                play_head->params_u8++;
                break;
            case 2:
                play_head->params_u16++;
                break;
            case 3:
                play_head->params_u32++;
                break;
            }
            play_head->track_mode_selector = 1;
        }
        else if (play_head->current_sample > 0) {
            enb_calc_params_forward(play_head);
            play_head->track_mode_selector = 1;
        }

        enb_get_track_unscaled_forward(play_head);
        requested_time = ++play_head->current_sample * sps;

        if (play_head->data_header->duration <= requested_time)
            requested_time = play_head->data_header->duration;

        enb_calc_track(play_head, requested_time, true);
        play_head->current_sample_time = play_head->current_sample * sps;
        play_head->previous_sample_time = (play_head->current_sample - 1) * sps;
    }

    while (time < play_head->previous_sample_time) {
        if (play_head->track_mode_selector == 1) {
            if (--play_head->params_counter == 0xFF) {
                play_head->params_counter = 3;
                play_head->params_mode--;
            }

            mode = *play_head->params_mode >> shift_table_1[play_head->params_counter];
            mode &= 0x3;

            switch (mode) {
            case 1:
                play_head->params_u8--;
                break;
            case 2:
                play_head->params_u16--;
                break;
            case 3:
                play_head->params_u32--;
                break;
            }
            play_head->track_mode_selector = 2;
        }
        else
            enb_calc_params_backward(play_head);

        play_head->current_sample--;
        enb_get_track_unscaled_backward(play_head);
        play_head->current_sample_time = play_head->current_sample * sps;
        play_head->previous_sample_time = (play_head->current_sample - 1) * sps;
        enb_calc_track(play_head, play_head->previous_sample_time, false);
    }
}

static void enb_get_track_unscaled_init(enb_play_head* play_head) { // 0x08A08D3C in ULJM05681
    uint32_t i, j, mode;
    int32_t val;

    enb_track* track_data = play_head->track_data;

    for (i = 0; i < play_head->data_header->track_count; i++, track_data++) {
        for (j = 0; j < 7; j++) {
            if (play_head->track_data_init_counter == 4) {
                play_head->track_data_init_counter = 0;
                play_head->track_data_init_mode++;
            }

            mode = *play_head->track_data_init_mode >> shift_table_1[play_head->track_data_init_counter++];
            mode &= 0x3;

            val = 0;
            switch (mode) {
            case 1:
                val = *play_head->track_data_init_i8++;
                break;
            case 2:
                val = *play_head->track_data_init_i16++;
                break;
            case 3:
                val = *play_head->track_data_init_i32++;
                break;
            }

            switch (j) {
            case 0:
                track_data->quat.x = (float_t)val;
                break;
            case 1:
                track_data->quat.y = (float_t)val;
                break;
            case 2:
                track_data->quat.z = (float_t)val;
                break;
            case 3:
                track_data->quat.w = (float_t)val;
                break;
            case 4:
                track_data->trans.x = (float_t)val;
                break;
            case 5:
                track_data->trans.y = (float_t)val;
                break;
            case 6:
                track_data->trans.z = (float_t)val;
                break;
            }
        }
    }
    play_head->current_sample = 0;
}

static void enb_get_track_unscaled_forward(enb_play_head* play_head) { // 0x08A08E7C in ULJM05681
    uint32_t i, j;
    int32_t val;

    enb_track* track_data = play_head->track_data;

    for (i = 0; i < play_head->data_header->track_count; i++, track_data++) {
        if (track_data->flags == 0)
            continue;

        for (j = 0; j < 7; j++) {
            if ((track_data->flags & (1 << j)) == 0)
                continue;

            if (play_head->track_data_mode_counter == 4) {
                play_head->track_data_mode_counter = 0;
                play_head->track_data_mode++;
            }

            val = *play_head->track_data_mode >> shift_table_1[play_head->track_data_mode_counter++];
            val &= 0x3;

            if (val == 2) {
                if (play_head->track_data_mode2_counter == 2) {
                    play_head->track_data_mode2_counter = 0;
                    play_head->track_data_mode2++;
                }

                val = *play_head->track_data_mode2 >> shift_table_2[play_head->track_data_mode2_counter++];
                val &= 0xF;

                if (val == 0) {
                    val = *play_head->track_data_i8;
                    play_head->track_data_i8++;
                    if (val == 0) {
                        val = *play_head->track_data_i16++;
                        if (val == 0)
                            val = *play_head->track_data_i32++;
                    }
                    else if ((val > 0) && (val < 9))
                        val += 0x7f;
                    else if ((val > -9) && (val < 0))
                        val -= 0x80;
                }
                else
                    val = value_table_2[val];
            }
            else
                val = value_table_1[val];

            switch (j) {
            case 0:
                track_data->quat.x += val;
                break;
            case 1:
                track_data->quat.y += val;
                break;
            case 2:
                track_data->quat.z += val;
                break;
            case 3:
                track_data->quat.w += val;
                break;
            case 4:
                track_data->trans.x += val;
                break;
            case 5:
                track_data->trans.y += val;
                break;
            case 6:
                track_data->trans.z += val;
                break;
            }
        }
    }
}

static void enb_get_track_unscaled_backward(enb_play_head* play_head) { // 0x08A090A0 in ULJM05681
    uint32_t i, j;
    int32_t val;

    enb_track* track_data = play_head->track_data;

    track_data += (size_t)play_head->data_header->track_count - 1;
    for (i = play_head->data_header->track_count - 1; i != 0xFFFFFFFFU; i--, track_data--) {
        if (track_data->flags == 0)
            continue;

        for (j = 0; j < 7; j++) {
            if ((track_data->flags & (1 << (6 - j))) == 0)
                continue;

            if (--play_head->track_data_mode_counter == 0xFF) {
                play_head->track_data_mode_counter = 3;
                play_head->track_data_mode--;
            }

            val = *play_head->track_data_mode >> shift_table_1[play_head->track_data_mode_counter];
            val &= 0x3;

            if (val == 2) {
                if (--play_head->track_data_mode2_counter == 0xFF) {
                    play_head->track_data_mode2_counter = 1;
                    play_head->track_data_mode2--;
                }

                val = *play_head->track_data_mode2 >> shift_table_2[play_head->track_data_mode2_counter];
                val &= 0xF;

                if (val == 0) {
                    val = *--play_head->track_data_i8;
                    if (val == 0) {
                        val = *--play_head->track_data_i16;
                        if (val == 0)
                            val = *--play_head->track_data_i32;
                    }
                    else if ((val > 0) && (val < 9))
                        val += 0x7f;
                    else if ((val > -9) && (val < 0))
                        val -= 0x80;
                }
                else
                    val = value_table_2[val];
            }
            else
                val = value_table_1[val];

            switch (6 - j) {
            case 0:
                track_data->quat.x -= val;
                break;
            case 1:
                track_data->quat.y -= val;
                break;
            case 2:
                track_data->quat.z -= val;
                break;
            case 3:
                track_data->quat.w -= val;
                break;
            case 4:
                track_data->trans.x -= val;
                break;
            case 5:
                track_data->trans.y -= val;
                break;
            case 6:
                track_data->trans.z -= val;
                break;
            }
        }
    }
}

static void enb_calc_params_init(enb_play_head* play_head) { // 0x08A0931C in ULJM05681
    uint32_t mode, val;

    if (play_head->params_counter == 4) {
        play_head->params_counter = 0;
        play_head->params_mode++;
    }

    mode = *play_head->params_mode >> shift_table_1[play_head->params_counter++];
    mode &= 0x3;

    val = 0;
    switch (mode) {
    case 1:
        val = *play_head->params_u8++;
        break;
    case 2:
        val = *play_head->params_u16++;
        break;
    case 3:
        val = *play_head->params_u32++;
        break;
    }
    play_head->next_params_change = val;
    play_head->prev_params_change = 0;
}

static void enb_calc_params_forward(enb_play_head* play_head) { // 0x08A09404 in ULJM05681
    uint32_t i, j, mode, temp, track_params_count, val;

    enb_track* track_data = play_head->track_data;

    track_params_count = play_head->data_header->track_count * 7;
    i = 0;
    while (i < track_params_count) {
        j = play_head->next_params_change;
        if (j == 0) {
            track_data[i / 7].flags ^= 1 << (i % 7);
            if (play_head->params_counter == 4) {
                play_head->params_counter = 0;
                play_head->params_mode++;
            }

            mode = *play_head->params_mode >> shift_table_1[play_head->params_counter++];
            mode &= 0x3;

            val = 0;
            switch (mode) {
            case 1:
                val = *play_head->params_u8++;
                break;
            case 2:
                val = *play_head->params_u16++;
                break;
            case 3:
                val = *play_head->params_u32++;
                break;
            }
            play_head->next_params_change = val;
            play_head->prev_params_change = 0;
            i++;
        }
        else {
            temp = j < (uint32_t)(track_params_count - i) ? j : (uint32_t)(track_params_count - i);
            i += (int32_t)temp;
            play_head->next_params_change -= temp;
            play_head->prev_params_change += temp;
        }
    }
}

static void enb_calc_params_backward(enb_play_head* play_head) { // 0x08A0968C in ULJM05681
    uint32_t i, j, mode, temp, track_params_count, val;

    enb_track* track_data = play_head->track_data;

    track_params_count = play_head->data_header->track_count * 7;
    i = track_params_count - 1;
    while (i != 0xFFFFFFFFU) {
        j = play_head->prev_params_change;
        if (j == 0) {
            track_data[i / 7].flags ^= 1 << (i % 7);
            if (--play_head->params_counter == 0xFF) {
                play_head->params_counter = 3;
                play_head->params_mode--;
            }

            mode = *play_head->params_mode >> shift_table_1[play_head->params_counter];
            mode &= 0x3;

            val = 0;
            switch (mode) {
            case 1:
                val = *--play_head->params_u8;
                break;
            case 2:
                val = *--play_head->params_u16;
                break;
            case 3:
                val = *--play_head->params_u32;
                break;
            }
            play_head->next_params_change = 0;
            play_head->prev_params_change = val;
            i--;
        }
        else {
            temp = j < (uint32_t)(i + 1) ? j : (uint32_t)(i + 1);
            i -= (int32_t)temp;
            play_head->next_params_change += temp;
            play_head->prev_params_change -= temp;
        }
    }
}

static void enb_calc_track_init(enb_play_head* play_head) { // 0x08A086CC in ULJM05681
    uint32_t i;
    uint8_t* track_flags;
    quat C010, C100, C200; // PSP VFPU registers
    vec3 C020, C110; // PSP VFPU registers
    float_t S030; // PSP VFPU register

    enb_track* track_data = play_head->track_data;

    track_flags = play_head->track_flags;

    C200.x = C200.y = C200.z = C200.w = 0.0f;
    S030 = play_head->data_header->scale;
    for (i = 0; i < play_head->data_header->track_count; i++) {
        C010 = track_data->quat;
        C020 = track_data->trans;

        C100.x = C010.x * S030;
        C100.y = C010.y * S030;
        C100.z = C010.z * S030;
        C100.w = C010.w * S030;

        C110.x = C020.x * S030;
        C110.y = C020.y * S030;
        C110.z = C020.z * S030;

        quat_normalize(&C100, &C100);

        track_data->qt[0].quat = C100;
        track_data->qt[0].trans = C110;
        track_data->qt[0].time = 0.0f;
        track_data->qt[1].quat = C100;
        track_data->qt[1].trans = C110;
        track_data->qt[1].time = 0.0f;

        track_data->quat = C200;
        (&track_data->quat)[1] = C200;
        track_data->flags = *track_flags++;

        track_data++;
    }
    play_head->track_data_selector = 0;
}

static void enb_calc_track(enb_play_head* play_head, float_t time, bool forward) { // 0x08A085D8 in ULJM05681
    uint8_t s0, s1;
    uint32_t i;
    quat C010, C100, C120; // PSP VFPU registers
    vec3 C020, C110, C130; // PSP VFPU registers
    float_t S030; // PSP VFPU register

    enb_track* track_data = play_head->track_data;

    if (forward) {
        s1 = play_head->track_data_selector & 0x1;
        play_head->track_data_selector = s0 = s1 ^ 1;
    }
    else {
        s0 = play_head->track_data_selector & 0x1;
        play_head->track_data_selector = s1 = s0 ^ 1;
    }

    S030 = forward ? play_head->data_header->scale : -play_head->data_header->scale;
    for (i = 0; i < play_head->data_header->track_count; i++, track_data++) {
        C010 = track_data->quat;
        C020 = track_data->trans;

        C120 = track_data->qt[s0].quat;
        C130 = track_data->qt[s0].trans;

        C100.x = C010.x * S030 + C120.x;
        C100.y = C010.y * S030 + C120.y;
        C100.z = C010.z * S030 + C120.z;
        C100.w = C010.w * S030 + C120.w;

        C110.x = C020.x * S030 + C130.x;
        C110.y = C020.y * S030 + C130.y;
        C110.z = C020.z * S030 + C130.z;

        quat_normalize(&C100, &C100);

        track_data->qt[s1].quat = C100;
        track_data->qt[s1].trans = C110;
        track_data->qt[s1].time = time;
    }
}
