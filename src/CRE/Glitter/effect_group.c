/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "effect_group.h"
#include "effect.h"
#include "scene.h"
#include "texture.h"

glitter_effect_group* glitter_effect_group_init(GLT) {
    glitter_effect_group* eg = force_malloc(sizeof(glitter_effect_group));
    eg->emission = 1.0f;
    eg->version = GLT_VAL == GLITTER_X ? 0x0C : 0x09;
    return eg;
}

bool glitter_effect_group_parse_file(glitter_effect_group* a1, f2_struct* st) {
    for (f2_struct* i = st->sub_structs.begin; i != st->sub_structs.end; i++) {
        if (!i->header.data_size)
            continue;

        if (i->header.signature == reverse_endianness_uint32_t('EFCT')
            && !glitter_effect_parse_file(a1, i, &a1->effects))
            return false;
        else if (i->header.signature == reverse_endianness_uint32_t('DVRS')
            && !glitter_texture_hashes_unpack_file(a1, i))
            return false;
    }
    return true;
}

bool glitter_effect_group_unparse_file(GLT, glitter_effect_group* a1, f2_struct* st) {
    memset(st, 0, sizeof(f2_struct));

    for (glitter_effect** i = a1->effects.begin; i != a1->effects.end; i++) {
        if (!*i)
            continue;

        f2_struct s;
        memset(&s, 0, sizeof(f2_struct));
        if (glitter_effect_unparse_file(GLT_VAL, a1, &s, *i))
            vector_f2_struct_push_back(&st->sub_structs, &s);
    }

    f2_struct s;
    memset(&s, 0, sizeof(f2_struct));
    if (glitter_texture_hashes_pack_file(a1, &s))
        vector_f2_struct_push_back(&st->sub_structs, &s);

    st->header.signature = reverse_endianness_uint32_t('DVEF');
    st->header.length = 0x20;
    st->header.use_big_endian = false;
    st->header.use_section_size = true;
    st->header.version = a1->version;
    return true;
}

void glitter_effect_group_dispose(glitter_effect_group* eg) {
    vector_ptr_glitter_effect_free(&eg->effects, glitter_effect_dispose);
    vector_uint64_t_free(&eg->resource_hashes);
    glitter_texture_unload(eg);
    tex_set_free(&eg->resources_tex);
    free(eg);
}
