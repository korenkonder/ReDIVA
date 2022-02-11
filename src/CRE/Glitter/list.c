/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "list.h"

bool glitter_list_pack_file(glitter_effect_group* a1, f2_struct* st) {
    size_t data;
    glitter_effect** i;
    size_t length;

    if (!a1->effects.begin)
        return false;

    memset(st, 0, sizeof(f2_struct));

    vector_enrs_entry e = vector_empty(enrs_entry);
    enrs_entry ee;

    ee = { 0, 1, 4, 1, vector_empty(enrs_sub_entry) };
    vector_enrs_sub_entry_append(&ee.sub, 0, 1, ENRS_DWORD);
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

    st->header.signature = reverse_endianness_uint32_t('GEFF');
    st->header.length = 0x20;
    st->header.use_big_endian = false;
    st->header.use_section_size = true;
    return true;
}

bool glitter_list_unpack_file(glitter_effect_group* a1, f2_struct* st) {
    size_t d;
    glitter_effect** i;
    uint32_t length;

    if (!st || !st->header.data_size)
        return false;

    d = (size_t)st->data;
    if (st->header.use_big_endian)
        length = load_reverse_endianness_uint32_t((void*)d);
    else
        length = *(uint32_t*)d;

    if (length != vector_length(a1->effects)) {
        for (i = a1->effects.begin; i != a1->effects.end; i++)
            if (*i)
                memset((*i)->name, 0, 0x80);
        return false;
    }

    if (length) {
        d += 0x04;
        for (i = a1->effects.begin; i != a1->effects.end; i++, d += 0x80)
            if (*i)
                memcpy((*i)->name, (void*)d, 0x80);
    }
    return true;
}
