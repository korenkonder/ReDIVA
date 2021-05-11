/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "animation.h"
#include "curve.h"

void FASTCALL glitter_animation_copy(GPM,
    vector_ptr_glitter_curve* src, vector_ptr_glitter_curve* dst) {
    if (!dst)
        return;

    vector_ptr_glitter_curve_free(dst, glitter_curve_dispose);

    if (!src)
        return;

    vector_ptr_glitter_curve_append(dst, src->end - src->begin);
    for (glitter_curve** i = src->begin; i != src->end; i++)
        if (*i) {
            glitter_curve* c = glitter_curve_copy(GPM_VAL, *i);
            if (c)
                vector_ptr_glitter_curve_push_back(dst, &c);
        }
}

bool FASTCALL glitter_animation_parse_file(GPM, f2_struct* st,
    vector_ptr_glitter_curve* vec, glitter_curve_type_flags flags) {
    glitter_curve* c;

    if (!st || !st->header.data_size)
        return false;

    for (f2_struct* i = st->sub_structs.begin; i != st->sub_structs.end; i++) {
        if (!i->header.data_size)
            continue;

        if (i->header.signature == reverse_endianess_uint32_t('CURV')
            && glitter_curve_parse_file(GPM_VAL, i, st->header.version, &c))
            if (flags & 1 << (size_t)(int32_t)c->type)
                vector_ptr_glitter_curve_push_back(vec, &c);
            else
                glitter_curve_dispose(c);
    }
    return true;
}

bool FASTCALL glitter_animation_unparse_file(GPM, f2_struct* st,
    vector_ptr_glitter_curve* vec, glitter_curve_type_flags flags) {
    memset(st, 0, sizeof(f2_struct));

    if (vec->end - vec->begin <= 0)
        return false;

    static const glitter_curve_type order[] = {
        GLITTER_CURVE_COLOR_A,
        GLITTER_CURVE_COLOR_R,
        GLITTER_CURVE_COLOR_G,
        GLITTER_CURVE_COLOR_B,
        GLITTER_CURVE_COLOR_RGB_SCALE,
        GLITTER_CURVE_COLOR_A_2ND,
        GLITTER_CURVE_COLOR_R_2ND,
        GLITTER_CURVE_COLOR_G_2ND,
        GLITTER_CURVE_COLOR_B_2ND,
        GLITTER_CURVE_COLOR_RGB_SCALE_2ND,
        GLITTER_CURVE_TRANSLATION_X,
        GLITTER_CURVE_TRANSLATION_Y,
        GLITTER_CURVE_TRANSLATION_Z,
        GLITTER_CURVE_ROTATION_X,
        GLITTER_CURVE_ROTATION_Y,
        GLITTER_CURVE_ROTATION_Z,
        GLITTER_CURVE_SCALE_X,
        GLITTER_CURVE_SCALE_Y,
        GLITTER_CURVE_SCALE_Z,
        GLITTER_CURVE_SCALE_ALL,
        GLITTER_CURVE_EMISSION_INTERVAL,
        GLITTER_CURVE_PARTICLES_PER_EMISSION,
        GLITTER_CURVE_U_SCROLL,
        GLITTER_CURVE_V_SCROLL,
        GLITTER_CURVE_U_SCROLL_ALPHA,
        GLITTER_CURVE_V_SCROLL_ALPHA,
        GLITTER_CURVE_U_SCROLL_2ND,
        GLITTER_CURVE_V_SCROLL_2ND,
        GLITTER_CURVE_U_SCROLL_ALPHA_2ND,
        GLITTER_CURVE_V_SCROLL_ALPHA_2ND,
    };

    for (int32_t i = GLITTER_CURVE_TRANSLATION_X; i <= GLITTER_CURVE_V_SCROLL_ALPHA_2ND; i++) {
        if (!(flags & 1 << (size_t)order[i]))
            continue;

        for (glitter_curve** j = vec->begin; j != vec->end; j++) {
            glitter_curve* c = *j;
            if (!c || c->type != order[i])
                continue;

            f2_struct s;
            if (glitter_curve_unparse_file(GPM_VAL, &s, c)) {
                vector_f2_struct_push_back(&st->sub_structs, &s);
                break;
            }
        }
    }

    if (st->sub_structs.end - st->sub_structs.begin < 1)
        return false;

    st->header.signature = reverse_endianess_uint32_t('ANIM');
    st->header.length = 0x20;
    st->header.use_big_endian = false;
    st->header.use_section_size = true;
    return true;
}