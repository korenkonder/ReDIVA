/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.hpp"
#include "quat.hpp"
#include "quat_trans.hpp"
#include "vec.hpp"

struct enb_track {
    quat_trans qt[2];
    quat quat;
    vec3 trans;
    uint8_t flags;
    uint8_t padding[3];
};

struct enb_head {
    uint32_t signature;                   // 0x00
    uint32_t track_count;                 // 0x04
    float_t scale;                        // 0x08
    float_t duration;                     // 0x0C
    uint32_t samples;                     // 0x10
    uint32_t track_data_init_mode_length; // 0x14
    uint32_t track_data_init_i8_length;   // 0x18
    uint32_t track_data_init_i16_length;  // 0x1C
    uint32_t track_data_init_i32_length;  // 0x20
    uint32_t track_data_mode_length;      // 0x24
    uint32_t track_data_mode2_length;     // 0x28
    uint32_t track_data_i8_length;        // 0x2C
    uint32_t track_data_i16_length;       // 0x30
    uint32_t track_data_i32_length;       // 0x44
    uint32_t params_mode_length;          // 0x38
    uint32_t params_u8_length;            // 0x3C
    uint32_t params_u16_length;           // 0x40
    uint32_t params_u32_length;           // 0x44
    uint32_t track_flags_length;          // 0x48
    uint32_t unknown;                     // In runtime becomes pointer to data after this uint32_t
};

struct enb_play_head {
    uint32_t current_sample;            // 0x00
    float_t current_sample_time;        // 0x04
    float_t previous_sample_time;       // 0x08
    enb_head* data_header;              // 0x0C
    enb_track* track_data;              // 0x10
    uint32_t data_length;               // 0x14
    uint32_t unknown[2];                // 0x18
    float_t requested_time;             // 0x20
    float_t seconds_per_sample;         // 0x24
    uint32_t next_params_change;        // 0x28
    uint32_t prev_params_change;        // 0x2C
    uint8_t* track_flags;               // 0x30
    uint8_t* track_data_init_mode;      // 0x34
    int8_t* track_data_init_i8;         // 0x38
    int16_t* track_data_init_i16;       // 0x3C
    int32_t* track_data_init_i32;       // 0x40
    uint8_t track_data_init_counter;    // 0x44
    uint8_t padding[3];                 // 0x45
    uint8_t* track_data_mode;           // 0x48
    uint8_t* track_data_mode2;          // 0x4C
    int8_t* track_data_i8;              // 0x50
    int16_t* track_data_i16;            // 0x54
    int32_t* track_data_i32;            // 0x58
    uint8_t track_data_mode_counter;    // 0x5C
    uint8_t track_data_mode2_counter;   // 0x5D
    uint8_t padding2[2];                // 0x5E
    uint8_t* params_mode;               // 0x60
    uint8_t* params_u8;                 // 0x64
    uint16_t* params_u16;               // 0x68
    uint32_t* params_u32;               // 0x6C
    uint8_t params_counter;             // 0x70
    uint8_t padding3[3];                // 0x71
    uint8_t* orig_track_data_init_mode; // 0x74
    int8_t* orig_track_data_init_i8;    // 0x78
    int16_t* orig_track_data_init_i16;  // 0x7C
    int32_t* orig_track_data_init_i32;  // 0x80
    uint8_t* orig_track_data_mode;      // 0x84
    uint8_t* orig_track_data_mode2;     // 0x88
    int8_t* orig_track_data_i8;         // 0x8C
    int16_t* orig_track_data_i16;       // 0x90
    int32_t* orig_track_data_i32;       // 0x94
    uint8_t* orig_params_mode;          // 0x98
    uint8_t* orig_params_u8;            // 0x9C
    uint16_t* orig_params_u16;          // 0xA0
    uint32_t* orig_params_u32;          // 0xA4
    uint8_t track_mode_selector;        // 0xA8
    uint8_t track_data_selector;        // 0xA9
    uint8_t padding4[6];                // 0xAA
};

extern int32_t enb_process(uint8_t* data_in, uint8_t** data_out,
    size_t* data_out_len, float_t* duration, float_t* fps, size_t* frames);
extern int32_t enb_initialize(uint8_t* data, enb_play_head** play_head);
extern void enb_free(enb_play_head** play_head);
extern void enb_get_track_data(enb_play_head* play_head, size_t track, quat_trans* data);
extern void enb_set_time(enb_play_head* play_head, float_t time);
