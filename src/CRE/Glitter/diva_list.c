/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "diva_list.h"
#include "effect_group.h"
#include "list.h"

bool glitter_diva_list_parse_file(GlitterEffectGroup* a1, f2_struct* st) {
    if (!st || !st->header.data_size)
        return false;

    for (f2_struct& i : st->sub_structs) {
        if (!i.header.data_size)
            continue;

        if (i.header.signature == reverse_endianness_uint32_t('GEFF')) {
            glitter_list_unpack_file(a1, &i);
            break;
        }
    }
    return true;
}

bool glitter_diva_list_unparse_file(GlitterEffectGroup* a1, f2_struct* st) {
    f2_struct s;
    if (!glitter_list_pack_file(a1, &s))
        return false;

    st->sub_structs.push_back(s);

    st->header.signature = reverse_endianness_uint32_t('LIST');
    st->header.length = 0x20;
    st->header.use_big_endian = false;
    st->header.use_section_size = true;
    return true;
}
