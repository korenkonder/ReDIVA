/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "quat.h"
#include "vec.h"

typedef struct quat_trans {
    quat quat;
    vec3 trans;
    float_t time;
} quat_trans;

static const quat_trans quat_trans_identity = { { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, 0.0f };

extern void lerp_quat_trans(quat_trans* x, quat_trans* y, quat_trans* z, float_t blend);
