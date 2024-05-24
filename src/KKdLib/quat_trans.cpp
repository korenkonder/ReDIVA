/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "quat_trans.hpp"

void interp_quat_trans(quat_trans* x, quat_trans* y,
    quat_trans* z, float_t blend, quat_trans_interp_method method) {
    if (blend > 1.0f)
        blend = 1.0f;
    else if (blend < 0.0f)
        blend = 0.0f;

    switch (method) {
    case QUAT_TRANS_INTERP_NONE:
        z->quat = y->quat;
        z->trans = y->trans;
        z->time = lerp_def(x->time, y->time, blend);
        break;
    case QUAT_TRANS_INTERP_LERP:
        z->quat = quat::lerp(x->quat, y->quat, blend);
        z->trans = vec3::lerp(x->trans, y->trans, blend);
        z->time = lerp_def(x->time, y->time, blend);
        break;
    case QUAT_TRANS_INTERP_SLERP:
        z->quat = quat::slerp(x->quat, y->quat, blend);
        z->trans = vec3::lerp(x->trans, y->trans, blend);
        z->time = lerp_def(x->time, y->time, blend);
        break;
    }
}
