/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "qti.h"

static void qti_interpolate(qti* interp);

qti* quat_trans_array_to_qti(quat_trans* array, size_t length,
    float_t interpolation_framerate, float_t requested_framerate) {
    if (!array || !length)
        return 0;

    qti* interp = force_malloc(sizeof(qti));
    interp->time = 0.0f;
    interp->array = array;
    interp->length = length;
    interp->frame = 0.0f;
    interp->delta_frame = interpolation_framerate / requested_framerate;
    interp->interpolation_framerate = interpolation_framerate;
    interp->requested_framerate = requested_framerate;
    interp->value = quat_trans_identity;
    interp->first_key = interp->last_key = 0;
    qti_reset(interp);

    if (array && length) {
        interp->value = array[0];
        interp->first_key = array;
        interp->last_key = array + length - 1;
    }
    return interp;
}

inline void qti_set_interpolation_framerate(qti* interp, float_t interpolation_framerate) {
    interp->interpolation_framerate = interpolation_framerate;
    interp->delta_frame = interp->interpolation_framerate / interp->requested_framerate;
}

inline void qti_set_requested_framerate(qti* interp, float_t requested_framerate) {
    interp->requested_framerate = requested_framerate;
    interp->delta_frame = interp->interpolation_framerate / interp->requested_framerate;
}

inline void qti_update(qti* interp) {
    interp->delta_frame = interp->interpolation_framerate / interp->requested_framerate;
}

inline void qti_reset(qti* interp) {
    interp->delta_frame = interp->interpolation_framerate / interp->requested_framerate;
    interp->frame = 0.0f;
    interp->time = interp->frame / interp->requested_framerate;
}

void qti_set_time(qti* interp, quat_trans* result, float_t time) {
    if (!interp || !interp->array || !interp->length) {
        *result = quat_trans_identity;
        return;
    }

    interp->frame = time * interp->interpolation_framerate;
    interp->time = time;

    qti_interpolate(interp);
    *result = interp->value;
}

void qti_set_frame(qti* interp, quat_trans* result, float_t frame) {
    if (!interp || !interp->array || !interp->length) {
        *result = quat_trans_identity;
        return;
    }

    interp->frame = frame * interp->delta_frame;
    interp->time = frame / interp->requested_framerate;

    qti_interpolate(interp);
    *result = interp->value;
}

void qti_add_time(qti* interp, quat_trans* result, float_t time) {
    if (!interp || !interp->array || !interp->length) {
        *result = quat_trans_identity;
        return;
    }

    interp->frame = time * interp->interpolation_framerate;
    interp->time += time;

    qti_interpolate(interp);
    *result = interp->value;
}

void qti_next_frame(qti* interp, quat_trans* result) {
    if (!interp || !interp->array || !interp->length) {
        *result = quat_trans_identity;
        return;
    }

    interp->frame += interp->delta_frame;
    interp->time = interp->frame / interp->interpolation_framerate;

    qti_interpolate(interp);
    *result = interp->value;
}

static void qti_interpolate(qti* interp) {
    float_t time = interp->frame / interp->interpolation_framerate;

    quat_trans* keys = interp->array;
    size_t key = 0;
    size_t length = interp->length;
    size_t temp;
    while (length > 0)
        if (time > keys[key + (temp = length >> 1)].time) {
            key += temp + 1;
            length -= temp + 1;
        }
        else
            length = temp;

    if (key == 0) {
        interp->value = *interp->first_key;
        return;
    }
    else if (key == interp->length) {
        interp->value = *interp->last_key;
        return;
    }

    quat_trans* c, * n;
    c = &keys[key - 1];
    n = &keys[key];

    quat_trans value;
    if (c->time < n->time)
        lerp_quat_trans(c, n, &value, (time - c->time) / (n->time - c->time));
    else
        value = *c;
    interp->value = value;
}
