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

    z->quat = quat::slerp(x->quat, y->quat, blend);
    z->trans = vec3::lerp(x->trans, y->trans, blend);
    z->time = lerp_def(x->time, y->time, blend);
}
