/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "list.h"

bool glitter_list_pack_file(GlitterEffectGroup* a1, f2_struct* st) {
    size_t data;
    size_t length;

    if (!a1->effects.size())
        return false;

    enrs e;
    enrs_entry ee;

    ee = { 0, 1, 4, 1 };
    ee.sub.push_back({ 0, 1, ENRS_DWORD });
    e.vec.push_back(ee);

    length = 0;
    for (glitter_effect*& i : a1->effects)
        if (i)
            length++;

    st->data.resize(0x10 + 0x80 * length);
    data = (size_t)st->data.data();
    st->enrs = e;

    *(uint32_t*)data = (uint32_t)length;
    data += 4;

    for (glitter_effect*& i : a1->effects)
        if (i) {
            memcpy((void*)data, i->name, 0x80);
            data += 0x80;
        }

    st->header.signature = reverse_endianness_uint32_t('GEFF');
    st->header.length = 0x20;
    st->header.use_big_endian = false;
    st->header.use_section_size = true;
    return true;
}

bool glitter_list_unpack_file(GlitterEffectGroup* a1, f2_struct* st) {
    size_t d;
    uint32_t length;

    if (!st || !st->header.data_size)
        return false;

    d = (size_t)st->data.data();
    if (st->header.use_big_endian)
        length = load_reverse_endianness_uint32_t((void*)d);
    else
        length = *(uint32_t*)d;

    if (length != a1->effects.size()) {
        for (glitter_effect*& i : a1->effects)
            if (i)
                memset(i->name, 0, 0x80);
        return false;
    }

    if (length) {
        d += 0x04;
        for (glitter_effect*& i : a1->effects) {
            if (i)
                memcpy(i->name, (void*)d, 0x80);
            d += 0x80;
        }
    }
    return true;
}
