/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/
#include "glitter.hpp"

namespace Glitter {
    Animation::Animation() {
        curves.reserve(0x80);
    }

    Animation::~Animation() {
        for (Curve*& i : curves)
            delete i;
    }

#if defined(CRE_DEV)
    void Animation::AddValue(GLT, float_t val, CurveTypeFlags flags) {
        for (int32_t i = CURVE_TRANSLATION_X;
            i <= CURVE_V_SCROLL_ALPHA_2ND; i++) {
            if (~flags & (1 << (size_t)i))
                continue;

            for (Curve*& j : curves) {
                Curve* c = j;
                if (c && c->type == i)
                    c->AddValue(GLT_VAL, val);
            }
        }
    }
#endif
}
