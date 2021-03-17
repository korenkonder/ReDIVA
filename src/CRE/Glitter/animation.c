/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "animation.h"
#include "curve.h"

void FASTCALL glitter_animation_parse_file(f2_struct* st, vector_ptr_glitter_curve* vec) {
    if (!st || !st->header.data_size)
        return;

    for (f2_struct* i = st->sub_structs.begin; i != st->sub_structs.end; i++) {
        if (!i->header.data_size)
            continue;

        if (i->header.signature == 0x56525543)
            glitter_curve_parse_file(i, vec);
    }
}

bool FASTCALL glitter_animation_unparse_file(f2_struct* st, vector_ptr_glitter_curve* vec, bool use_big_endian) {
    memset(st, 0, sizeof(f2_struct));

    if (vec->end - vec->begin <= 0)
        return false;

    f2_struct* s = f2_struct_init();
    for (glitter_curve** i = vec->begin; i != vec->end; i++)
        if (*i) {
            glitter_curve_unparse_file(s, *i, use_big_endian);
            vector_f2_struct_push_back(&st->sub_structs, s);
        }
    free(s);

    st->header.signature = 0x4D494E41;
    st->header.length = 0x20;
    st->header.use_big_endian = false;
    st->header.use_section_size = true;
    return true;
}