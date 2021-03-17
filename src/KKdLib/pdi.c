/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "pdi.h"

pdi* kft2_array_to_pdi(kft2* array, size_t length, float_t interpolation_framerate, float_t requested_framerate) {
    if (!array || !length)
        return 0;

    pdi* interp = force_malloc(sizeof(pdi));
    interp->time = 0.0f;
    interp->array = array;
    interp->length = length;
    interp->frame = -1.0f;
    interp->delta_frame = 0.0f;
    interp->interpolation_framerate = interpolation_framerate;
    interp->requested_framerate = requested_framerate;
    interp->value = 0.0f;
    interp->first_key = interp->last_key = 0;
    pdi_reset(interp);

    if (array && length) {
        interp->first_key = array;
        interp->last_key = array + length - 1;
    }
    return interp;
}

FORCE_INLINE void pdi_set_interpolation_framerate(pdi* interp, float_t interpolation_framerate) {
    interp->interpolation_framerate = interpolation_framerate;
    interp->delta_frame = interp->interpolation_framerate / interp->requested_framerate;
}

FORCE_INLINE void pdi_set_requested_framerate(pdi* interp, float_t requested_framerate) {
    interp->requested_framerate = requested_framerate;
    interp->delta_frame = interp->interpolation_framerate / interp->requested_framerate;
}

FORCE_INLINE void pdi_update(pdi* interp) {
    interp->delta_frame = interp->interpolation_framerate / interp->requested_framerate;
}

FORCE_INLINE void pdi_reset(pdi* interp) {
    interp->delta_frame = interp->interpolation_framerate / interp->requested_framerate;
    interp->frame = -interp->delta_frame; interp->time = interp->frame / interp->requested_framerate;
}

static void pdi_interpolate(pdi* interp) {
    float_t frame = interp->frame;

    kft2* keys = interp->array;
    size_t key = 0;
    size_t length = interp->length;
    size_t temp;
    while (length > 0)
        if (frame > keys[key + (temp = length >> 1)].frame) {
            key += temp + 1;
            length -= temp + 1;
        }
        else
            length = temp;

    kft2 c, n;
    c = keys[key - 1];
    n = keys[key];

    if (frame <= c.frame || frame >= n.frame)
        interp->value = frame > c.frame ? n.value : c.value;
    else if (n.tangent == c.tangent && n.tangent == 0.0f) {
        float_t t = (frame - c.frame) / (n.frame - c.frame);
        interp->value = (1.0f - t) * c.value + t * n.value;
    }
    else {
        float_t t = (frame - c.frame) / (n.frame - c.frame);
        float_t t_1 = t - 1.0f;
        interp->value = (t_1 * 2.0f - 1.0f) * (c.value - n.value) * t * t +
            (t_1 * c.tangent + t * n.tangent) * t_1 * (frame - c.frame) + c.value;
    }
}

float_t pdi_set_time(pdi* interp, float_t time) {
    if (!interp || !interp->array || !interp->length)
        return 0.0f;

    interp->frame = time * interp->interpolation_framerate;
    interp->time = time;

    pdi_interpolate(interp);
    return interp->value;
}

float_t pdi_set_frame(pdi* interp, float_t frame) {
    if (!interp || !interp->array || !interp->length)
        return 0.0f;

    interp->frame = frame * interp->delta_frame;
    interp->time = frame / interp->requested_framerate;

    pdi_interpolate(interp);
    return interp->value;
}

float_t pdi_add_time(pdi* interp, float_t time) {
    if (!interp || !interp->array || !interp->length)
        return 0.0f;

    interp->frame = time * interp->interpolation_framerate;
    interp->time += time;

    pdi_interpolate(interp);
    return interp->value;
}

float_t pdi_next_frame(pdi* interp) {
    if (!interp || !interp->array || !interp->length)
        return 0.0f;

    interp->frame += interp->delta_frame;
    interp->time = interp->frame / interp->interpolation_framerate;

    pdi_interpolate(interp);
    return interp->value;
}