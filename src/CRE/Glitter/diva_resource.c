/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "diva_resource.h"
#include "effect_group.h"
#include "texture.h"

bool glitter_diva_resource_parse_file(GPM, GlitterEffectGroup* a1, f2_struct* st) {
    if (!st || !st->header.data_size)
        return false;

    for (f2_struct& i : st->sub_structs) {
        if (!i.header.data_size)
            continue;

        if (i.header.signature == reverse_endianness_uint32_t('TXPC')) {
            glitter_texture_resource_unpack_file(GPM_VAL, a1, &i);
            break;
        }
    }
    return true;
}

bool glitter_diva_resource_unparse_file(GlitterEffectGroup* a1, f2_struct* st) {
    f2_struct s;
    if (!glitter_texture_resource_pack_file(a1, &s))
        return false;

    st->sub_structs.push_back(s);

    st->header.signature = reverse_endianness_uint32_t('DVRS');
    st->header.length = 0x20;
    st->header.use_big_endian = false;
    st->header.use_section_size = true;
    st->header.version = 1;
    return true;
}
