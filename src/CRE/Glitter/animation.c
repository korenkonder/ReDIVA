/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "animation.h"
#include "curve.h"

#if CRE_DEV
void glitter_animation_add_value(GLT, glitter_animation* anim,
    float_t val, glitter_curve_type_flags flags) {
    for (int32_t i = GLITTER_CURVE_TRANSLATION_X;
        i <= GLITTER_CURVE_V_SCROLL_ALPHA_2ND; i++) {
        if (!(flags & 1 << (size_t)i))
            continue;

        for (glitter_curve** j = anim->begin; j != anim->end; j++) {
            glitter_curve* c = *j;
            if (c && c->type == i)
                glitter_curve_add_value(GLT_VAL, c, val);
        }
    }
}
#endif

void glitter_animation_copy(glitter_animation* src, glitter_animation* dst) {
    if (!dst)
        return;

    vector_old_ptr_glitter_curve_free(dst, glitter_curve_dispose);

    if (!src)
        return;

    vector_old_ptr_glitter_curve_reserve(dst, vector_old_length(*src));
    for (glitter_curve** i = src->begin; i != src->end; i++)
        if (*i) {
            glitter_curve* c = glitter_curve_copy(*i);
            if (c)
                vector_old_ptr_glitter_curve_push_back(dst, &c);
        }
}

void glitter_animation_free(glitter_animation* anim) {
    vector_old_ptr_glitter_curve_free(anim, glitter_curve_dispose);
}

bool glitter_animation_parse_file(GLT, f2_struct* st,
    glitter_animation* anim, glitter_curve_type_flags flags) {
    glitter_curve* c;

    if (!st || !st->header.data_size)
        return false;

    for (f2_struct* i = st->sub_structs.begin; i != st->sub_structs.end; i++) {
        if (!i->header.data_size)
            continue;

        if (i->header.signature == reverse_endianness_uint32_t('CURV')
            && glitter_curve_parse_file(GLT_VAL, i, st->header.version, &c))
            if (flags & 1 << (size_t)(int32_t)c->type)
                vector_old_ptr_glitter_curve_push_back(anim, &c);
            else
                glitter_curve_dispose(c);
    }
    return true;
}

bool glitter_animation_unparse_file(GLT, f2_struct* st,
    glitter_animation* anim, glitter_curve_type_flags flags) {
    memset(st, 0, sizeof(f2_struct));

    if (vector_old_length(*anim) < 1)
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

        for (glitter_curve** j = anim->begin; j != anim->end; j++) {
            glitter_curve* c = *j;
            if (!c || c->type != order[i])
                continue;

            f2_struct s;
            if (glitter_curve_unparse_file(GLT_VAL, &s, c)) {
                vector_old_f2_struct_push_back(&st->sub_structs, &s);
                break;
            }
        }
    }

    if (vector_old_length(st->sub_structs) < 1)
        return false;

    st->header.signature = reverse_endianness_uint32_t('ANIM');
    st->header.length = 0x20;
    st->header.use_big_endian = false;
    st->header.use_section_size = true;
    return true;
}