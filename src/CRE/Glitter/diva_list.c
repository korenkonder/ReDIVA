/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "diva_list.h"
#include "effect_group.h"
#include "list.h"

bool FASTCALL glitter_diva_list_parse_file(glitter_effect_group* a1, f2_struct* st) {
    f2_struct* i;

    if (!st || !st->header.data_size)
        return false;

    for (i = st->sub_structs.begin; i != st->sub_structs.end; i++) {
        if (!i->header.data_size)
            continue;

        if (i->header.signature == 0x46464547) {
            glitter_list_unpack_file(a1, i);
            break;
        }
    }
    return true;
}

bool FASTCALL glitter_diva_list_unparse_file(glitter_effect_group* a1, f2_struct* st, bool use_big_endian) {
    memset(st, 0, sizeof(f2_struct));

    f2_struct* s = f2_struct_init();
    if (!glitter_list_pack_file(a1, s, use_big_endian)) {
        f2_struct_dispose(s);
        return false;
    }

    vector_f2_struct_push_back(&st->sub_structs, s);
    free(s);

    st->header.signature = 0x5453494C;
    st->header.length = 0x20;
    st->header.use_big_endian = false;
    st->header.use_section_size = true;
    return true;
}
