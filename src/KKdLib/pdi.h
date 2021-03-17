/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "kf.h"

typedef struct pdi {
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
} pdi;

extern pdi* kft2_array_to_pdi(kft2* array, size_t length,
    float_t interpolation_framerate, float_t requested_framerate);
extern void pdi_set_interpolation_framerate(pdi* interp, float_t interpolation_framerate);
extern void pdi_set_requested_framerate(pdi* interp, float_t requested_framerate);
extern void pdi_update(pdi* interp);
extern void pdi_reset(pdi* interp);
extern float_t pdi_set_time(pdi* interp, float_t time);
extern float_t pdi_set_frame(pdi* interp, float_t frame);
extern float_t pdi_add_time(pdi* interp, float_t time);
extern float_t pdi_next_frame(pdi* interp);

