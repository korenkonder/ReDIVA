/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "a3dai.h"

a3dai* a3da_key_to_a3dai(auth_3d_key* key, float_t interpolation_framerate, float_t requested_framerate) {
    if (!key)
        return 0;

    auth_3d_struct* s = auth_3d_key_to_auth_3d_struct(key);

    if (!s)
        return 0;

    a3dai* interp = force_malloc(sizeof(a3dai));

    if (!interp)
        return 0;

    memset(interp, 0, sizeof(a3dai));

    interp->time = 0.0f;
    interp->auth_3d = s;
    interp->frame = -1.0f;
    interp->delta_frame = 0.0f;
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

a3dai* a3da_head_to_a3dai(auth_3d_struct* s, float_t interpolation_framerate, float_t requested_framerate) {
    if (!s)
        return 0;

    a3dai* interp = force_malloc(sizeof(a3dai));

    if (!interp)
        return 0;

    memset(interp, 0, sizeof(a3dai));

    interp->time = 0.0f;
    interp->auth_3d = s;
    interp->frame = -1.0f;
    interp->delta_frame = 0.0f;
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

FORCE_INLINE void a3dai_set_interpolation_framerate(a3dai* interp, float_t interpolation_framerate) {
    interp->interpolation_framerate = interpolation_framerate;
    interp->delta_frame = interp->interpolation_framerate / interp->requested_framerate;
}

FORCE_INLINE void a3dai_set_requested_framerate(a3dai* interp, float_t requested_framerate) {
    interp->requested_framerate = requested_framerate;
    interp->delta_frame = interp->interpolation_framerate / interp->requested_framerate;
}

FORCE_INLINE void a3dai_update(a3dai* interp) {
    interp->delta_frame = interp->interpolation_framerate / interp->requested_framerate;
}

FORCE_INLINE void a3dai_reset(a3dai* interp) {
    interp->delta_frame = interp->interpolation_framerate / interp->requested_framerate;
    interp->frame = -interp->delta_frame; interp->time = interp->frame / interp->requested_framerate;
}

static void a3dai_interpolate(a3dai* interp) {
    float_t delta_frame, offset = 0.0f;
    float_t frame = interp->frame;

    kft3 first_key = *interp->first_key;
    kft3 last_key = *interp->last_key;
    if (frame <= first_key.frame) {
        delta_frame = first_key.frame - frame;
        switch (interp->auth_3d->ep_type_pre) {
        case AUTH_3D_EP_TYPE_LINEAR:
            interp->value = first_key.value - delta_frame * first_key.tangent2;
            return;
        case AUTH_3D_EP_TYPE_CYCLE:
            frame = last_key.frame - fmodf(delta_frame, interp->auth_3d->frame_delta);
            break;
        case AUTH_3D_EP_TYPE_CYCLE_OFFSET:
            frame = last_key.frame - fmodf(delta_frame, interp->auth_3d->frame_delta);
            offset = -(float_t)((int32_t)(delta_frame / interp->auth_3d->frame_delta) + 1) * interp->auth_3d->value_delta;
            break;
        default:
            interp->value = first_key.value;
            return;
        }
    }
    else if (frame >= last_key.frame) {
        delta_frame = frame - last_key.frame;
        switch (interp->auth_3d->ep_type_post) {
        case AUTH_3D_EP_TYPE_LINEAR:
            interp->value = last_key.value + delta_frame * last_key.tangent1;
            return;
        case AUTH_3D_EP_TYPE_CYCLE:
            frame = first_key.frame + fmodf(delta_frame, interp->auth_3d->frame_delta);
            break;
        case AUTH_3D_EP_TYPE_CYCLE_OFFSET:
            frame = first_key.frame + fmodf(delta_frame, interp->auth_3d->frame_delta);
            offset = (float_t)((int32_t)(delta_frame / interp->auth_3d->frame_delta) + 1) * interp->auth_3d->value_delta;
            break;
        default:
            interp->value = last_key.value;
            return;
        }
    }

    auth_3d_key_type type = interp->auth_3d->type;
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

    kft3 c, n;
    c = keys[key - 1];
    n = keys[key];

    if (frame <= c.frame || frame > n.frame)
        interp->value = frame > c.frame ? n.value : c.value;
    else if (type == AUTH_3D_KEY_TYPE_LINEAR) {
        float_t t = (frame - c.frame) / (n.frame - c.frame);
        interp->value = (1.0f - t) * c.value + t * n.value;
    }
    else if (type == AUTH_3D_KEY_TYPE_HERMITE) {
        float_t t = (frame - c.frame) / (n.frame - c.frame);
        float_t t_2 = (1.0f - t) * (1.0f - t);
        interp->value = t_2 * c.value * (1.0f + 2.0f * t) + (t * n.value * (3.0f - 2.0f * t) +
            (t_2 * c.tangent2 + t * (t - 1.0f) * n.tangent1) * (n.frame - c.frame)) * t;
    }
    else
        interp->value = c.value;
    interp->value += offset;
}

float_t a3dai_set_time(a3dai* interp, float_t time) {
    if (!interp || !interp->auth_3d || !interp->auth_3d->keys || !interp->auth_3d->length)
        return 0.0f;

    interp->frame = time * interp->interpolation_framerate;
    interp->time = time;

    switch (interp->auth_3d->type) {
    case AUTH_3D_KEY_TYPE_STATIC:
        interp->value = interp->auth_3d->value;
        break;
    case AUTH_3D_KEY_TYPE_LINEAR:
    case AUTH_3D_KEY_TYPE_HERMITE:
    case AUTH_3D_KEY_TYPE_HOLD:
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
    case AUTH_3D_KEY_TYPE_STATIC:
        interp->value = interp->auth_3d->value;
        break;
    case AUTH_3D_KEY_TYPE_LINEAR:
    case AUTH_3D_KEY_TYPE_HERMITE:
    case AUTH_3D_KEY_TYPE_HOLD:
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
    case AUTH_3D_KEY_TYPE_STATIC:
        interp->value = interp->auth_3d->value;
        break;
    case AUTH_3D_KEY_TYPE_LINEAR:
    case AUTH_3D_KEY_TYPE_HERMITE:
    case AUTH_3D_KEY_TYPE_HOLD:
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
    case AUTH_3D_KEY_TYPE_STATIC:
        interp->value = interp->auth_3d->value;
        break;
    case AUTH_3D_KEY_TYPE_LINEAR:
    case AUTH_3D_KEY_TYPE_HERMITE:
    case AUTH_3D_KEY_TYPE_HOLD:
        a3dai_interpolate(interp);
        break;
    default:
        interp->value = 0.0f;
        break;
    }
    return interp->value;
}
