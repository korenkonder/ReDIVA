/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "a3da_head.h"

auth_3d_struct* auth_3d_key_to_auth_3d_struct(auth_3d_key* k) {
    if (!k)
        return 0;

    auth_3d_struct* a = force_malloc(sizeof(auth_3d_struct));

    if (!a)
        return 0;

    memset(a, 0, sizeof(auth_3d_struct));

    a->max_frames = k->max;
    if (k->type > AUTH_3D_KEY_TYPE_STATIC && k->length > 1) {
        a->type = k->type;
        a->length = k->length;
        a->keys = k->keys;
        a->ep_type_post = k->ep_type_post;
        a->ep_type_pre = k->ep_type_pre;
        a->frame_delta = k->keys[k->length - 1].frame - k->keys[0].frame;
        a->value_delta = k->keys[k->length - 1].value - k->keys[0].value;
    }
    else {
        a->type = k->type;
        a->value = k->type > AUTH_3D_KEY_TYPE_NONE ? k->value : 0.0f;
        a->ep_type_post = k->ep_type_post;
        a->ep_type_pre = k->ep_type_pre;
        a->frame_delta = a->max_frames;
        a->value_delta = a->value;
    }
    return a;
}
