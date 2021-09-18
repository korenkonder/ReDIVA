/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "a3da_data.h"

auth_3d_data_struct* auth_3d_data_to_auth_3d_struct(auth_3d_data* d) {
    if (!d)
        return 0;

    auth_3d_data_struct* a = force_malloc(sizeof(auth_3d_data_struct));

    if (!a)
        return 0;

    memset(a, 0, sizeof(auth_3d_data_struct));

    a->max_frames = d->max;
    if (d->type > AUTH_3D_DATA_STATIC && d->length > 1) {
        a->type = d->type;
        a->length = d->length;
        a->keys = d->keys;
        a->ep_type_post = d->ep_type_post;
        a->ep_type_pre = d->ep_type_pre;
        a->frame_delta = d->keys[d->length - 1].frame - d->keys[0].frame;
        a->value_delta = d->keys[d->length - 1].value - d->keys[0].value;
    }
    else {
        a->type = d->type;
        a->value = d->type > AUTH_3D_DATA_NONE ? d->value : 0.0f;
        a->ep_type_post = d->ep_type_post;
        a->ep_type_pre = d->ep_type_pre;
        a->frame_delta = a->max_frames;
        a->value_delta = a->value;
    }
    return a;
}
