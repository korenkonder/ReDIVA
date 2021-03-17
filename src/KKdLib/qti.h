/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "quat_trans.h"

typedef struct qti {
    quat_trans* array;
    size_t length;

    float_t frame;
    float_t time;
    quat_trans value;
    float_t delta_frame;
    float_t interpolation_framerate;
    float_t requested_framerate;

    quat_trans* first_key;
    quat_trans* last_key;
} qti;

extern qti* quat_trans_array_to_qti(quat_trans* array, size_t length,
    float_t interpolation_framerate, float_t requested_framerate);
extern void qti_set_interpolation_framerate(qti* interp, float_t interpolation_framerate);
extern void qti_set_requested_framerate(qti* interp, float_t requested_framerate);
extern void qti_update(qti* interp);
extern void qti_reset(qti* interp);
extern void qti_set_time(qti* interp, quat_trans* result, float_t time);
extern void qti_set_frame(qti* interp, quat_trans* result, float_t frame);
extern void qti_add_time(qti* interp, quat_trans* result, float_t time);
extern void qti_next_frame(qti* interp, quat_trans* result);
