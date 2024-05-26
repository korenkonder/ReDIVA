/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "quat_trans.hpp"

quat_trans::quat_trans() : time() {

}

quat_trans::quat_trans(::quat quat, vec3 trans, float_t time) {
    this->quat = quat;
    this->trans = trans;
    this->time = time;
}

void interp_quat_trans(const quat_trans* x, const quat_trans* y, quat_trans* z, float_t blend,
    quat_trans_interp_method quat_method, quat_trans_interp_method trans_method) {
    if (blend > 1.0f)
        blend = 1.0f;
    else if (blend < 0.0f)
        blend = 0.0f;

    switch (quat_method) {
    case QUAT_TRANS_INTERP_NONE:
        z->quat = y->quat;
        break;
    case QUAT_TRANS_INTERP_LERP:
        z->quat = quat::lerp(x->quat, y->quat, blend);
        break;
    case QUAT_TRANS_INTERP_SLERP:
        z->quat = quat::slerp(x->quat, y->quat, blend);
        break;
    }

    switch (trans_method) {
    case QUAT_TRANS_INTERP_NONE:
        z->trans = y->trans;
        break;
    case QUAT_TRANS_INTERP_LERP:
    case QUAT_TRANS_INTERP_SLERP:
        z->trans = vec3::lerp(x->trans, y->trans, blend);
        break;
    }

    z->time = lerp_def(x->time, y->time, blend);
}
