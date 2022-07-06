/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "quat_trans.hpp"

void lerp_quat_trans(quat_trans* x, quat_trans* y, quat_trans* z, float_t blend) {
    if (blend > 1.0f)
        blend = 1.0f;
    else if (blend < 0.0f)
        blend = 0.0f;

    quat_slerp(&x->quat, &y->quat, &z->quat, blend);
    vec3_lerp_scalar(x->trans, y->trans, z->trans, blend);
    z->time = lerp(x->time, y->time, blend);
}
