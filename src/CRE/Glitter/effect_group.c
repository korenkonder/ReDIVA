/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "effect_group.h"
#include "effect.h"
#include "scene.h"
#include "texture.h"

glitter_effect_group* FASTCALL glitter_effect_group_init() {
    glitter_effect_group* eg = force_malloc(sizeof(glitter_effect_group));
    eg->emission = 1.0f;
    return eg;
}

bool FASTCALL glitter_effect_group_parse_file(glitter_effect_group* a1, f2_struct* st) {
    for (f2_struct* i = st->sub_structs.begin; i != st->sub_structs.end; i++) {
        if (!i->header.data_size)
            continue;

        if (i->header.signature == 0x54434645 && !glitter_effect_parse_file(a1, i, &a1->effects))
            return false;
        else if (i->header.signature == 0x53525644 && !glitter_texture_hashes_unpack_file(a1, i))
            return false;
    }
    return true;
}

bool FASTCALL glitter_effect_group_unparse_file(glitter_effect_group* a1, f2_struct* st, bool use_big_endian) {
    memset(st, 0, sizeof(f2_struct));

    f2_struct* s = f2_struct_init();
    for (glitter_effect** i = a1->effects.begin; i != a1->effects.end; i++)
        if (*i && glitter_effect_unparse_file(a1, s, *i, use_big_endian))
            vector_f2_struct_push_back(&st->sub_structs, s);

    if (!glitter_texture_hashes_pack_file(a1, s, use_big_endian)) {
        f2_struct_dispose(s);
        return false;
    }

    vector_f2_struct_push_back(&st->sub_structs, s);
    free(s);

    st->header.signature = 0x46455644;
    st->header.length = 0x20;
    st->header.use_big_endian = false;
    st->header.use_section_size = true;
    st->header.version = a1->version;
    return true;
}

void FASTCALL glitter_effect_group_dispose(glitter_effect_group* eg) {
    vector_ptr_glitter_effect_free(&eg->effects, (void*)glitter_effect_dispose);
    free(eg->resource_hashes);
    if (eg->resources)
        glDeleteTextures(eg->resources_count, eg->resources);
    free(eg->resources);
    if (eg->resources_tex)
        txp_dispose(eg->resources_tex);
    free(eg);
}
