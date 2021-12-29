/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "diva_resource.h"
#include "effect_group.h"
#include "texture.h"

bool glitter_diva_resource_parse_file(GPM, glitter_effect_group* a1, f2_struct* st) {
    f2_struct* i;

    if (!st || !st->header.data_size)
        return false;

    for (i = st->sub_structs.begin; i != st->sub_structs.end; i++) {
        if (!i->header.data_size)
            continue;

        if (i->header.signature == reverse_endianness_uint32_t('TXPC')) {
            glitter_texture_resource_unpack_file(GPM_VAL, a1, i);
            break;
        }
    }
    return true;
}

bool glitter_diva_resource_unparse_file(glitter_effect_group* a1, f2_struct* st) {
    memset(st, 0, sizeof(f2_struct));

    f2_struct s;
    memset(&s, 0, sizeof(f2_struct));
    if (!glitter_texture_resource_pack_file(a1, &s)) {
        f2_struct_free(&s);
        return false;
    }

    vector_f2_struct_push_back(&st->sub_structs, &s);

    st->header.signature = reverse_endianness_uint32_t('DVRS');
    st->header.length = 0x20;
    st->header.use_big_endian = false;
    st->header.use_section_size = true;
    st->header.version = 1;
    return true;
}
