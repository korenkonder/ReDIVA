/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern glitter_curve* glitter_curve_init(GLT);
#if defined(CRE_DEV) || defined(CLOUD_DEV)
extern void glitter_curve_add_value(GLT, glitter_curve* curve, float_t val);
#endif
extern glitter_curve* glitter_curve_copy(glitter_curve* c);
extern bool glitter_curve_get_value(GLT, glitter_curve* c,
    float_t frame, float_t* value, int32_t random_value, glitter_random* random);
extern bool glitter_curve_parse_file(GLT,
    f2_struct* st, uint32_t version, glitter_curve** c);
#if defined(CRE_DEV) || defined(CLOUD_DEV)
extern void glitter_curve_recalculate(GLT, glitter_curve* curve);
#endif
extern bool glitter_curve_unparse_file(GLT,
    f2_struct* st, glitter_curve* c);
extern void glitter_curve_dispose(glitter_curve* c);
