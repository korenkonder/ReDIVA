/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

#if defined(CRE_DEV) || defined(CLOUD_DEV)
extern void glitter_curve_add_value(GLT, GlitterCurve* curve, float_t val);
#endif
extern GlitterCurve* glitter_curve_copy(GlitterCurve* c);
extern bool glitter_curve_get_value(GLT, GlitterCurve* c,
    float_t frame, float_t* value, int32_t random_value, glitter_random* random);
extern bool glitter_curve_parse_file(GLT,
    f2_struct* st, uint32_t version, GlitterCurve** c);
#if defined(CRE_DEV) || defined(CLOUD_DEV)
extern void glitter_curve_recalculate(GLT, GlitterCurve* curve);
#endif
extern bool glitter_curve_unparse_file(GLT,
    f2_struct* st, GlitterCurve* c);
