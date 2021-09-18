/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "moti.h"
#include "interpolation.h"

static void moti_interpolate(moti* interp);

moti* kft2_array_to_moti(kft2* array, size_t length, float_t interpolation_framerate, float_t requested_framerate) {
    if (!array || !length)
        return 0;

    moti* interp = force_malloc(sizeof(moti));
    interp->time = 0.0f;
    interp->array = array;
    interp->length = length;
    interp->frame = 0.0f;
    interp->delta_frame = interpolation_framerate / requested_framerate;
    interp->interpolation_framerate = interpolation_framerate;
    interp->requested_framerate = requested_framerate;
    interp->value = 0.0f;
    interp->first_key = interp->last_key = 0;
    moti_reset(interp);

    if (array && length) {
        interp->first_key = array;
        interp->last_key = array + length - 1;
    }
    return interp;
}

inline void moti_set_interpolation_framerate(moti* interp, float_t interpolation_framerate) {
    interp->interpolation_framerate = interpolation_framerate;
    interp->delta_frame = interp->interpolation_framerate / interp->requested_framerate;
}

inline void moti_set_requested_framerate(moti* interp, float_t requested_framerate) {
    interp->requested_framerate = requested_framerate;
    interp->delta_frame = interp->interpolation_framerate / interp->requested_framerate;
}

inline void moti_update(moti* interp) {
    interp->delta_frame = interp->interpolation_framerate / interp->requested_framerate;
}

inline void moti_reset(moti* interp) {
    interp->delta_frame = interp->interpolation_framerate / interp->requested_framerate;
    interp->frame = 0.0f;
    interp->time = interp->frame / interp->requested_framerate;
}

float_t moti_set_time(moti* interp, float_t time) {
    if (!interp || !interp->array || !interp->length)
        return 0.0f;

    interp->frame = time * interp->interpolation_framerate;
    interp->time = time;

    moti_interpolate(interp);
    return interp->value;
}

float_t moti_set_frame(moti* interp, float_t frame) {
    if (!interp || !interp->array || !interp->length)
        return 0.0f;

    interp->frame = frame * interp->delta_frame;
    interp->time = frame / interp->requested_framerate;

    moti_interpolate(interp);
    return interp->value;
}

float_t moti_add_time(moti* interp, float_t time) {
    if (!interp || !interp->array || !interp->length)
        return 0.0f;

    interp->frame = time * interp->interpolation_framerate;
    interp->time += time;

    moti_interpolate(interp);
    return interp->value;
}

float_t moti_next_frame(moti* interp) {
    if (!interp || !interp->array || !interp->length)
        return 0.0f;

    interp->frame += interp->delta_frame;
    interp->time = interp->frame / interp->interpolation_framerate;

    moti_interpolate(interp);
    return interp->value;
}

static void moti_interpolate(moti* interp) {
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

    if (key == 0) {
        interp->value = interp->first_key->value;
        return;
    }
    else if (key == interp->length) {
        interp->value = interp->last_key->value;
        return;
    }

    kft2* c, * n;
    c = &keys[key - 1];
    n = &keys[key];

    float_t value;
    if (c->frame < n->frame)
        value = interpolate_chs_value(c->value, n->value,
            c->tangent, n->tangent, c->frame, n->frame, frame);
    else
        value = c->value;
    interp->value = value;
}
