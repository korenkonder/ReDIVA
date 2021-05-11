/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "texture.h"

bool FASTCALL glitter_list_pack_file(glitter_effect_group* a1, f2_struct* st) {
    size_t data;
    glitter_effect** i;
    size_t length;

    if (!a1->effects.begin)
        return false;

    memset(st, 0, sizeof(f2_struct));

    vector_enrs_entry e = { 0, 0, 0 };
    enrs_entry ee;

    ee = (enrs_entry){ 0, 1, 4, 1, { 0, 0, 0 } };
    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_TYPE_DWORD });
    vector_enrs_entry_push_back(&e, &ee);

    length = 0;
    for (i = a1->effects.begin; i != a1->effects.end; i++)
        if (*i)
            length++;

    data = (size_t)force_malloc(0x10 + 0x80 * length);
    st->data = (void*)data;
    st->length = 0x10 + 0x80 * length;
    st->enrs = e;

    *(uint32_t*)data = (uint32_t)length;
    data += 4;

    for (i = a1->effects.begin; i != a1->effects.end; i++)
        if (*i) {
            memcpy((void*)data, (*i)->name, 0x80);
            data += 0x80;
        }

    st->header.signature = 0x46464547;
    st->header.length = 0x20;
    st->header.use_big_endian = false;
    st->header.use_section_size = true;
    return true;
}

bool FASTCALL glitter_list_unpack_file(glitter_effect_group* a1, f2_struct* st) {
    char* data;
    glitter_effect** i;
    uint32_t length;

    if (!st || !st->header.data_size)
        return false;

    data = st->data;
    if (st->header.use_big_endian)
        length = reverse_endianess_uint32_t(*(uint32_t*)data);
    else
        length = *(uint32_t*)data;

    if (length != a1->effects.end - a1->effects.begin) {
        for (i = a1->effects.begin; i != a1->effects.end; i++)
            if (*i)
                memset((*i)->name, 0, 0x80);
        return false;
    }

    if (length) {
        data += 0x04;
        for (i = a1->effects.begin; i != a1->effects.end; i++, data += 0x80)
            if (*i)
                memcpy((*i)->name, data, 0x80);
    }
    return true;
}
