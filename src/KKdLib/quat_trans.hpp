/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.hpp"
#include "quat.hpp"
#include "vec.hpp"

struct quat_trans {
    quat quat;
    vec3 trans;
    float_t time;
};

enum quat_trans_interp_method {
    QUAT_TRANS_INTERP_NONE = 0,
    QUAT_TRANS_INTERP_LERP,
    QUAT_TRANS_INTERP_SLERP,
};

static const quat_trans quat_trans_identity = { { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, 0.0f };

extern void interp_quat_trans(quat_trans* x, quat_trans* y,
    quat_trans* z, float_t blend, quat_trans_interp_method method);
