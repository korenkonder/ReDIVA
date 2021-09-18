/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#if defined(VIDEO)
#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/io_stream.h"
#include "hash.h"
#include "lock.h"
#include <x264.h>

typedef enum video_x264_bit_depth {
    VIDEO_X264_BIT_DEPTH_8_BIT  = 0,
    VIDEO_X264_BIT_DEPTH_10_BIT = 1,
} video_x264_bit_depth;

typedef enum video_x264_color_space {
    VIDEO_X264_COLOR_SPACE_I400 = 0x01,
    VIDEO_X264_COLOR_SPACE_I420 = 0x02,
    VIDEO_X264_COLOR_SPACE_YV12 = 0x03,
    VIDEO_X264_COLOR_SPACE_NV12 = 0x04,
    VIDEO_X264_COLOR_SPACE_NV21 = 0x05,
    VIDEO_X264_COLOR_SPACE_I422 = 0x06,
    VIDEO_X264_COLOR_SPACE_YV16 = 0x07,
    VIDEO_X264_COLOR_SPACE_NV16 = 0x08,
    VIDEO_X264_COLOR_SPACE_I444 = 0x0C,
    VIDEO_X264_COLOR_SPACE_YV24 = 0x0D,
    VIDEO_X264_COLOR_SPACE_BGR  = 0x0E,
    VIDEO_X264_COLOR_SPACE_BGRA = 0x0F,
    VIDEO_X264_COLOR_SPACE_RGB  = 0x10,
}video_x264_color_space;

typedef enum video_x264_rate_control_method {
    VIDEO_X264_RATE_CONTROL_CQP = 0,
    VIDEO_X264_RATE_CONTROL_CRF = 1,
    VIDEO_X264_RATE_CONTROL_ABR = 2,
} video_x264_rate_control_method;

typedef enum video_x264_pic_format {
    VIDEO_X264_PIC_FORMAT_NONE = 0,
    VIDEO_X264_PIC_FORMAT_RGB8,
    VIDEO_X264_PIC_FORMAT_RGB10,
    VIDEO_X264_PIC_FORMAT_RGB16,
    VIDEO_X264_PIC_FORMAT_RGBA8,
    VIDEO_X264_PIC_FORMAT_RGB10_A2,
    VIDEO_X264_PIC_FORMAT_RGBA16,
} video_x264_pic_format;

typedef enum video_x264_profile {
    VIDEO_X264_PROFILE_NONE     = -1,
    VIDEO_X264_PROFILE_BASELINE =  0,
    VIDEO_X264_PROFILE_MAIN     =  1,
    VIDEO_X264_PROFILE_HIGH     =  2,
} video_x264_profile;

typedef enum video_x264_preset {
    VIDEO_X264_PRESET_NONE      = -1,
    VIDEO_X264_PRESET_ULTRAFAST =  0,
    VIDEO_X264_PRESET_SUPERFAST =  1,
    VIDEO_X264_PRESET_VERYFAST  =  2,
    VIDEO_X264_PRESET_FASTER    =  3,
    VIDEO_X264_PRESET_FAST      =  4,
    VIDEO_X264_PRESET_MEDIUM    =  5,
    VIDEO_X264_PRESET_SLOW      =  6,
    VIDEO_X264_PRESET_SLOWER    =  7,
    VIDEO_X264_PRESET_VERYSLOW  =  8,
    VIDEO_X264_PRESET_PLACEBO   =  9,
} video_x264_preset;

typedef enum video_x264_tune {
    VIDEO_X264_TUNE_NONE        = -1,
    VIDEO_X264_TUNE_FILM        =  0,
    VIDEO_X264_TUNE_ANIMATION   =  1,
    VIDEO_X264_TUNE_GRAIN       =  2,
    VIDEO_X264_TUNE_STILLIMAGE  =  3,
    VIDEO_X264_TUNE_PSNR        =  4,
    VIDEO_X264_TUNE_SSIM        =  5,
    VIDEO_X264_TUNE_FASTDECODE  =  6,
    VIDEO_X264_TUNE_ZEROLATENCY =  7,
    VIDEO_X264_TUNE_MAX         =  8,
} video_x264_tune;

typedef struct video_x264_rate_control {
    video_x264_rate_control_method method;
    union {
        int32_t cqp;
        float_t crf;
        int32_t abr;
    };
} video_x264_rate_control;

typedef struct video_x264_init_data {
    int32_t width;
    int32_t height;
    double_t fps;
    bool flip;
    video_x264_preset preset;
    video_x264_tune tune;
    video_x264_profile profile;
    video_x264_color_space color_space;
    video_x264_bit_depth bit_depth;
    video_x264_rate_control rate_control;
} video_x264_init_data;

typedef struct video_x264_state {
    x264_param_t param;
    x264_picture_t pic;
    x264_picture_t pic_out;
    x264_t* enc;
    x264_nal_t* nal;
    int32_t i_nal;
    void* buffer;
} video_x264_state;

typedef struct video_x264 {
    int32_t error_state;
    int32_t frame;
    video_x264_state state;
    stream io;
    void* data;
    video_x264_pic_format fmt;
    lock lock;
} video_x264;

hash_dat(video_x264)
vector(hash_video_x264)

extern bool video_x264_init(video_x264* x, video_x264_init_data* init);
extern bool video_x264_encode(video_x264* x, int32_t frame);
extern void video_x264_load_image_data(video_x264* x);
extern bool video_x264_flush(video_x264* x);
extern void video_x264_close(video_x264* x);
#endif
