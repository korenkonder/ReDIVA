/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "a3dai.h"
#include "interpolation.h"

static void a3dai_interpolate(a3dai* interp);

a3dai* a3da_data_to_a3dai(auth_3d_data* data, float_t interpolation_framerate, float_t requested_framerate) {
    if (!data)
        return 0;

    auth_3d_data_struct* s = auth_3d_data_to_auth_3d_data_struct(data);

    if (!s)
        return 0;

    a3dai* interp = force_malloc(sizeof(a3dai));

    if (!interp)
        return 0;

    memset(interp, 0, sizeof(a3dai));

    interp->time = 0.0f;
    interp->auth_3d = s;
    interp->frame = 0.0f;
    interp->delta_frame = interpolation_framerate / requested_framerate;
    interp->interpolation_framerate = interpolation_framerate;
    interp->requested_framerate = requested_framerate;
    interp->value = 0.0f;
    interp->first_key = interp->last_key = 0;
    a3dai_reset(interp);

    if (s->keys && s->length) {
        interp->first_key = s->keys;
        interp->last_key = s->keys + s->length - 1;
    }
    return interp;
}

a3dai* a3da_head_to_a3dai(auth_3d_data_struct* s, float_t interpolation_framerate, float_t requested_framerate) {
    if (!s)
        return 0;

    a3dai* interp = force_malloc(sizeof(a3dai));

    if (!interp)
        return 0;

    memset(interp, 0, sizeof(a3dai));

    interp->time = 0.0f;
    interp->auth_3d = s;
    interp->frame = 0.0f;
    interp->delta_frame = interpolation_framerate / requested_framerate;
    interp->interpolation_framerate = interpolation_framerate;
    interp->requested_framerate = requested_framerate;
    interp->value = 0.0f;
    interp->first_key = interp->last_key = 0;
    a3dai_reset(interp);

    if (s->keys && s->length) {
        interp->first_key = s->keys;
        interp->last_key = s->keys + s->length - 1;
    }
    return interp;
}

inline void a3dai_set_interpolation_framerate(a3dai* interp, float_t interpolation_framerate) {
    interp->interpolation_framerate = interpolation_framerate;
    interp->delta_frame = interp->interpolation_framerate / interp->requested_framerate;
}

inline void a3dai_set_requested_framerate(a3dai* interp, float_t requested_framerate) {
    interp->requested_framerate = requested_framerate;
    interp->delta_frame = interp->interpolation_framerate / interp->requested_framerate;
}

inline void a3dai_update(a3dai* interp) {
    interp->delta_frame = interp->interpolation_framerate / interp->requested_framerate;
}

inline void a3dai_reset(a3dai* interp) {
    interp->delta_frame = interp->interpolation_framerate / interp->requested_framerate;
    interp->frame = 0.0f;
    interp->time = interp->frame / interp->requested_framerate;
}

float_t a3dai_set_time(a3dai* interp, float_t time) {
    if (!interp || !interp->auth_3d || !interp->auth_3d->keys || !interp->auth_3d->length)
        return 0.0f;

    interp->frame = time * interp->interpolation_framerate;
    interp->time = time;

    switch (interp->auth_3d->type) {
    case AUTH_3D_DATA_STATIC:
        interp->value = interp->auth_3d->value;
        break;
    case AUTH_3D_DATA_LINEAR:
    case AUTH_3D_DATA_HERMITE:
    case AUTH_3D_DATA_HOLD:
        a3dai_interpolate(interp);
        break;
    default:
        interp->value = 0.0f;
        break;
    }
    return interp->value;
}

float_t a3dai_set_frame(a3dai* interp, float_t frame) {
    if (!interp || !interp->auth_3d || !interp->auth_3d->keys || !interp->auth_3d->length)
        return 0.0f;

    interp->frame = frame * interp->delta_frame;
    interp->time = frame / interp->requested_framerate;

    switch (interp->auth_3d->type) {
    case AUTH_3D_DATA_STATIC:
        interp->value = interp->auth_3d->value;
        break;
    case AUTH_3D_DATA_LINEAR:
    case AUTH_3D_DATA_HERMITE:
    case AUTH_3D_DATA_HOLD:
        a3dai_interpolate(interp);
        break;
    default:
        interp->value = 0.0f;
        break;
    }
    return interp->value;
}

float_t a3dai_add_time(a3dai* interp, float_t time) {
    if (!interp || !interp->auth_3d || !interp->auth_3d->keys || !interp->auth_3d->length)
        return 0.0f;

    interp->frame = time * interp->interpolation_framerate;
    interp->time += time;

    switch (interp->auth_3d->type) {
    case AUTH_3D_DATA_STATIC:
        interp->value = interp->auth_3d->value;
        break;
    case AUTH_3D_DATA_LINEAR:
    case AUTH_3D_DATA_HERMITE:
    case AUTH_3D_DATA_HOLD:
        a3dai_interpolate(interp);
        break;
    default:
        interp->value = 0.0f;
        break;
    }
    return interp->value;
}

float_t a3dai_next_frame(a3dai* interp) {
    if (!interp || !interp->auth_3d || !interp->auth_3d->keys || !interp->auth_3d->length)
        return 0.0f;

    interp->frame += interp->delta_frame;
    interp->time = interp->frame / interp->interpolation_framerate;

    switch (interp->auth_3d->type) {
    case AUTH_3D_DATA_STATIC:
        interp->value = interp->auth_3d->value;
        break;
    case AUTH_3D_DATA_LINEAR:
    case AUTH_3D_DATA_HERMITE:
    case AUTH_3D_DATA_HOLD:
        a3dai_interpolate(interp);
        break;
    default:
        interp->value = 0.0f;
        break;
    }
    return interp->value;
}

static void a3dai_interpolate(a3dai* interp) {
    float_t offset;
    float_t frame = interp->frame;

    kft3* first_key = interp->first_key;
    kft3* last_key = interp->last_key;
    if (frame <= first_key->frame) {
        float_t delta_frame = first_key->frame - frame;
        switch (interp->auth_3d->ep_type_pre) {
        case AUTH_3D_EP_NONE:
        default:
            interp->value = first_key->value;
            return;
        case AUTH_3D_EP_LINEAR:
            interp->value = first_key->value - delta_frame * first_key->tangent1;
            return;
        case AUTH_3D_EP_CYCLE:
            frame = last_key->frame - fmodf(delta_frame, interp->auth_3d->frame_delta);
            offset = 0.0f;
            break;
        case AUTH_3D_EP_CYCLE_OFFSET:
            frame = last_key->frame - fmodf(delta_frame, interp->auth_3d->frame_delta);
            offset = -(float_t)((int32_t)(delta_frame / interp->auth_3d->frame_delta) + 1)
                * interp->auth_3d->value_delta;
            break;
        }
    }
    else if (frame >= last_key->frame) {
        float_t delta_frame = frame - last_key->frame;
        switch (interp->auth_3d->ep_type_post) {
        case AUTH_3D_EP_NONE:
        default:
            interp->value = last_key->value;
            return;
        case AUTH_3D_EP_LINEAR:
            interp->value = last_key->value + delta_frame * last_key->tangent2;
            return;
        case AUTH_3D_EP_CYCLE:
            frame = first_key->frame + fmodf(delta_frame, interp->auth_3d->frame_delta);
            offset = 0.0f;
            break;
        case AUTH_3D_EP_CYCLE_OFFSET:
            frame = first_key->frame + fmodf(delta_frame, interp->auth_3d->frame_delta);
            offset = (float_t)((int32_t)(delta_frame / interp->auth_3d->frame_delta) + 1)
                * interp->auth_3d->value_delta;
            break;
        }
    }
    else
        offset = 0.0f;

    auth_3d_data_type type = interp->auth_3d->type;
    kft3* keys = interp->auth_3d->keys;
    size_t key = 0;
    size_t length = interp->auth_3d->length;
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
    else if (key == interp->auth_3d->length) {
        interp->value = interp->last_key->value;
        return;
    }

    kft3* c, * n;
    c = &keys[key - 1];
    n = &keys[key];

    float_t value;
    switch (interp->auth_3d->type) {
    case AUTH_3D_DATA_LINEAR:
        if (c->frame < n->frame)
            value = interpolate_linear_value(c->value, n->value, c->frame, n->frame, frame);
        else
            value = c->value;
        break;
    case AUTH_3D_DATA_HERMITE:
        if (c->frame < n->frame)
            value = interpolate_chs_value(c->value, n->value,
                c->tangent2, n->tangent1, c->frame, n->frame, frame);
        else
            value = c->value;
        break;
    case AUTH_3D_DATA_HOLD:
        if (frame < n->frame)
            value = c->value;
        else
            value = n->value;
        break;
    default:
        value = 0.0f;
        break;
    }
    interp->value = value + offset;
}
