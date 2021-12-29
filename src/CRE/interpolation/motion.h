/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.h"
#include "../../KKdLib/kf.h"

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
} motion_interpolation;

extern void kft2_array_to_motion_interpolation(motion_interpolation* interp,
    kft2* array, size_t length, float_t interpolation_framerate, float_t requested_framerate);
extern void motion_interpolation_set_interpolation_framerate(motion_interpolation* interp,
    float_t interpolation_framerate);
extern void motion_interpolation_set_requested_framerate(motion_interpolation* interp,
    float_t requested_framerate);
extern void motion_interpolation_update(motion_interpolation* interp);
extern void motion_interpolation_reset(motion_interpolation* interp);
extern float_t motion_interpolation_set_time(motion_interpolation* interp, float_t time);
extern float_t motion_interpolation_set_frame(motion_interpolation* interp, float_t frame);
extern float_t motion_interpolation_add_time(motion_interpolation* interp, float_t time);
extern float_t motion_interpolation_next_frame(motion_interpolation* interp);

