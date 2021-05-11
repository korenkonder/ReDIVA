/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "kf.h"

typedef struct moti {
    kft2* array;
    size_t length;

    float_t frame;
    float_t time;
    float_t value;
    float_t delta_frame;
    float_t interpolation_framerate;
    float_t requested_framerate;

    kft2* first_key;
    kft2* last_key;
} moti;

extern moti* kft2_array_to_moti(kft2* array, size_t length,
    float_t interpolation_framerate, float_t requested_framerate);
extern void moti_set_interpolation_framerate(moti* interp, float_t interpolation_framerate);
extern void moti_set_requested_framerate(moti* interp, float_t requested_framerate);
extern void moti_update(moti* interp);
extern void moti_reset(moti* interp);
extern float_t moti_set_time(moti* interp, float_t time);
extern float_t moti_set_frame(moti* interp, float_t frame);
extern float_t moti_add_time(moti* interp, float_t time);
extern float_t moti_next_frame(moti* interp);

