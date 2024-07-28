/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"
#include "../../KKdLib/io/file_stream.hpp"
#include "../../KKdLib/io/path.hpp"
#include "../../KKdLib/interpolation.hpp"
#include "../../KKdLib/farc.hpp"
#include "../../KKdLib/str_utils.hpp"
#include "../data.hpp"

namespace Glitter {
    FileWriter::FileWriter() : type() {

    }

    void add_f2_key_random_range(KeyType key_type, enrs_entry& ee, uint32_t o) {
        if (key_type == KEY_HERMITE) {
            ee = { o, 2, 20, 1 };
            ee.append(0, 2, ENRS_WORD);
            ee.append(0, 4, ENRS_DWORD);
        }
        else {
            ee = { o, 2, 12, 1 };
            ee.append(0, 2, ENRS_WORD);
            ee.append(0, 2, ENRS_DWORD);
        }
    };

    void add_f2_key_no_random_range(KeyType key_type, enrs_entry& ee, uint32_t o) {
        if (key_type == KEY_HERMITE) {
            ee = { o, 2, 16, 1 };
            ee.append(0, 2, ENRS_WORD);
            ee.append(0, 3, ENRS_DWORD);
        }
        else {
            ee = { o, 2, 8, 1 };
            ee.append(0, 2, ENRS_WORD);
            ee.append(0, 1, ENRS_DWORD);
        }
    };

    int32_t get_f2_size_random_range(KeyType key_type) {
        return key_type == KEY_HERMITE ? 20 : 12;
    };

    int32_t get_f2_size_no_random_range(KeyType key_type) {
        return key_type == KEY_HERMITE ? 16 : 8;
    };

    void add_x_key_random_range(KeyType key_type, enrs_entry& ee, uint32_t o) {
        if (key_type == KEY_HERMITE) {
            ee = { o, 2, 32, 1 };
            ee.append(0, 2, ENRS_WORD);
            ee.append(0, 7, ENRS_DWORD);
        }
        else {
            ee = { o, 2, 16, 1 };
            ee.append(0, 2, ENRS_WORD);
            ee.append(0, 3, ENRS_DWORD);
        }
    };

    void add_x_key_no_random_range(KeyType key_type, enrs_entry& ee, uint32_t o) {
        if (key_type == KEY_HERMITE) {
            ee = { o, 2, 16, 1 };
            ee.append(0, 2, ENRS_WORD);
            ee.append(0, 3, ENRS_DWORD);
        }
        else {
            ee = { o, 2, 16, 1 };
            ee.append(0, 2, ENRS_WORD);
            ee.append(0, 3, ENRS_DWORD);
        }
    };

    int32_t get_x_size_random_range(KeyType key_type) {
        return key_type == KEY_HERMITE ? 32 : 16;
    };

    int32_t get_x_size_no_random_range(KeyType key_type) {
        return key_type == KEY_HERMITE ? 16 : 16;
    };

    void FileWriter::PackCurve(f2_struct* st, Curve* c, bool big_endian) {
        if (!c->keys.size()) {
            st->header.signature = reverse_endianness_uint32_t('KEYS');
            st->header.length = 0x20;
            st->header.use_big_endian = false;
            st->header.use_section_size = true;
            st->header.version = c->keys_version;
            return;
        }

        size_t l = 0;

        enrs e;
        enrs_entry ee;

        size_t count;

        void (*add_key)(KeyType key_type, enrs_entry & ee, uint32_t o);
        int32_t(*get_size)(KeyType key_type);

        const bool key_random_range = !!(c->flags & CURVE_KEY_RANDOM_RANGE);

        if (type == Glitter::X && c->keys_version != 2)
            if (key_random_range) {
                add_key = add_x_key_random_range;
                get_size = get_x_size_random_range;
            }
            else {
                add_key = add_x_key_no_random_range;
                get_size = get_x_size_no_random_range;
            }
        else
            if (key_random_range) {
                add_key = add_f2_key_random_range;
                get_size = get_f2_size_random_range;
            }
            else {
                add_key = add_f2_key_no_random_range;
                get_size = get_f2_size_no_random_range;
            }

        KeyType key_type = c->keys.front().type;

        add_key(key_type, ee, 0);

        count = 1;
        auto i_begin = c->keys.begin() + 1;
        auto i_end = c->keys.end();
        for (auto i = i_begin; i != i_end; i++, count++) {
            if (i->type == key_type)
                continue;

            ee.repeat_count = (uint32_t)count;
            e.vec.push_back(ee);

            l += get_size(key_type) * count;
            add_key(key_type, ee, (uint32_t)(get_size(key_type) * count));
            count = 0;
            key_type = i->type;
        }

        ee.repeat_count = (uint32_t)count;
        e.vec.push_back(ee);

        l += get_size(key_type) * count;

        l = align_val(l, 0x10);
        st->data.resize(l);
        size_t d = (size_t)st->data.data();

        if (type == Glitter::X) {
            if (c->keys_version == 2) {
                if (big_endian)
                    if (c->flags & CURVE_KEY_RANDOM_RANGE)
                        for (const Curve::Key& i : c->keys) {
                            store_reverse_endianness_int16_t((void*)d, (int16_t)i.type);
                            store_reverse_endianness_int16_t((void*)(d + 2), (int16_t)i.frame);
                            if (i.type == KEY_HERMITE) {
                                store_reverse_endianness_float_t((void*)(d + 4), i.tangent1);
                                store_reverse_endianness_float_t((void*)(d + 8), i.tangent2);
                                store_reverse_endianness_float_t((void*)(d + 12), i.random_range);
                                store_reverse_endianness_float_t((void*)(d + 16), i.value);
                                d += 20;
                            }
                            else {
                                store_reverse_endianness_float_t((void*)(d + 4), i.random_range);
                                store_reverse_endianness_float_t((void*)(d + 8), i.value);
                                d += 12;
                            }
                        }
                    else
                        for (const Curve::Key& i : c->keys) {
                            store_reverse_endianness_int16_t((void*)d, (int16_t)i.type);
                            store_reverse_endianness_int16_t((void*)(d + 2), (int16_t)i.frame);
                            if (i.type == KEY_HERMITE) {
                                store_reverse_endianness_float_t((void*)(d + 4), i.tangent1);
                                store_reverse_endianness_float_t((void*)(d + 8), i.tangent2);
                                store_reverse_endianness_float_t((void*)(d + 12), i.value);
                                d += 16;
                            }
                            else {
                                store_reverse_endianness_float_t((void*)(d + 4), i.value);
                                d += 8;
                            }
                        }
                else
                    if (c->flags & CURVE_KEY_RANDOM_RANGE)
                        for (const Curve::Key& i : c->keys) {
                            *(int16_t*)d = (int16_t)i.type;
                            *(int16_t*)(d + 2) = (int16_t)i.frame;
                            if (i.type == KEY_HERMITE) {
                                *(float_t*)(d + 4) = i.tangent1;
                                *(float_t*)(d + 8) = i.tangent2;
                                *(float_t*)(d + 12) = i.random_range;
                                *(float_t*)(d + 16) = i.value;
                                d += 20;
                            }
                            else {
                                *(float_t*)(d + 4) = i.random_range;
                                *(float_t*)(d + 8) = i.value;
                                d += 12;
                            }
                        }
                    else
                        for (const Curve::Key& i : c->keys) {
                            *(int16_t*)d = (int16_t)i.type;
                            *(int16_t*)(d + 2) = (int16_t)i.frame;
                            if (i.type == KEY_HERMITE) {
                                *(float_t*)(d + 4) = i.tangent1;
                                *(float_t*)(d + 8) = i.tangent2;
                                *(float_t*)(d + 12) = i.value;
                                d += 16;
                            }
                            else {
                                *(float_t*)(d + 4) = i.value;
                                d += 8;
                            }
                        }
            }
            else {
                if (big_endian)
                    if (c->flags & CURVE_KEY_RANDOM_RANGE)
                        for (const Curve::Key& i : c->keys) {
                            store_reverse_endianness_int16_t((void*)d, (int16_t)i.type);
                            store_reverse_endianness_int16_t((void*)(d + 2), (int16_t)i.frame);
                            if (i.type == KEY_HERMITE) {
                                store_reverse_endianness_float_t((void*)(d + 16), i.tangent1);
                                store_reverse_endianness_float_t((void*)(d + 20), i.tangent2);
                                store_reverse_endianness_float_t((void*)(d + 24), i.random_range);
                                store_reverse_endianness_float_t((void*)(d + 28), i.value);
                                d += 32;
                            }
                            else {
                                store_reverse_endianness_float_t((void*)(d + 8), i.random_range);
                                store_reverse_endianness_float_t((void*)(d + 12), i.value);
                                d += 16;
                            }
                        }
                    else
                        for (const Curve::Key& i : c->keys) {
                            store_reverse_endianness_int16_t((void*)d, (int16_t)i.type);
                            store_reverse_endianness_int16_t((void*)(d + 2), (int16_t)i.frame);
                            if (i.type == KEY_HERMITE) {
                                store_reverse_endianness_float_t((void*)(d + 4), i.tangent1);
                                store_reverse_endianness_float_t((void*)(d + 8), i.tangent2);
                                store_reverse_endianness_float_t((void*)(d + 12), i.value);
                                d += 16;
                            }
                            else {
                                store_reverse_endianness_float_t((void*)(d + 12), i.value);
                                d += 16;
                            }
                        }
                else
                    if (c->flags & CURVE_KEY_RANDOM_RANGE)
                        for (const Curve::Key& i : c->keys) {
                            *(int16_t*)d = (int16_t)i.type;
                            *(int16_t*)(d + 2) = (int16_t)i.frame;
                            if (i.type == KEY_HERMITE) {
                                *(float_t*)(d + 16) = i.tangent1;
                                *(float_t*)(d + 20) = i.tangent2;
                                *(float_t*)(d + 24) = i.random_range;
                                *(float_t*)(d + 28) = i.value;
                                d += 32;
                            }
                            else {
                                *(float_t*)(d + 8) = i.random_range;
                                *(float_t*)(d + 12) = i.value;
                                d += 16;
                            }
                        }
                    else
                        for (const Curve::Key& i : c->keys) {
                            *(int16_t*)d = (int16_t)i.type;
                            *(int16_t*)(d + 2) = (int16_t)i.frame;
                            if (i.type == KEY_HERMITE) {
                                *(float_t*)(d + 4) = i.tangent1;
                                *(float_t*)(d + 8) = i.tangent2;
                                *(float_t*)(d + 12) = i.value;
                                d += 16;
                            }
                            else {
                                *(float_t*)(d + 12) = i.value;
                                d += 16;
                            }
                        }
            }
        }
        else {
            float_t scale = 1.0f;
            if (c->keys_version == 0)
                switch (c->type) {
                case CURVE_ROTATION_X:
                case CURVE_ROTATION_Y:
                case CURVE_ROTATION_Z:
                    scale = RAD_TO_DEG_FLOAT;
                    break;
                case CURVE_COLOR_R:
                case CURVE_COLOR_G:
                case CURVE_COLOR_B:
                case CURVE_COLOR_A:
                    scale = 255.0f;
                    break;
                }

            if (scale == 1.0f) {
                if (big_endian)
                    if (c->flags & CURVE_KEY_RANDOM_RANGE)
                        for (Curve::Key& i : c->keys) {
                            store_reverse_endianness_int16_t((void*)d, (int16_t)i.type);
                            store_reverse_endianness_int16_t((void*)(d + 2), (int16_t)i.frame);
                            if (i.type == KEY_HERMITE) {
                                store_reverse_endianness_float_t((void*)(d + 4), i.tangent1);
                                store_reverse_endianness_float_t((void*)(d + 8), i.tangent2);
                                store_reverse_endianness_float_t((void*)(d + 12), i.random_range);
                                store_reverse_endianness_float_t((void*)(d + 16), i.value);
                                d += 20;
                            }
                            else {
                                store_reverse_endianness_float_t((void*)(d + 4), i.random_range);
                                store_reverse_endianness_float_t((void*)(d + 8), i.value);
                                d += 12;
                            }
                        }
                    else
                        for (Curve::Key& i : c->keys) {
                            store_reverse_endianness_int16_t((void*)d, (int16_t)i.type);
                            store_reverse_endianness_int16_t((void*)(d + 2), (int16_t)i.frame);
                            if (i.type == KEY_HERMITE) {
                                store_reverse_endianness_float_t((void*)(d + 4), i.tangent1);
                                store_reverse_endianness_float_t((void*)(d + 8), i.tangent2);
                                store_reverse_endianness_float_t((void*)(d + 12), i.value);
                                d += 16;
                            }
                            else {
                                store_reverse_endianness_float_t((void*)(d + 4), i.value);
                                d += 8;
                            }
                        }
                else
                    if (c->flags & CURVE_KEY_RANDOM_RANGE)
                        for (Curve::Key& i : c->keys) {
                            *(int16_t*)d = (int16_t)i.type;
                            *(int16_t*)(d + 2) = (int16_t)i.frame;
                            if (i.type == KEY_HERMITE) {
                                *(float_t*)(d + 4) = i.tangent1;
                                *(float_t*)(d + 8) = i.tangent2;
                                *(float_t*)(d + 12) = i.random_range;
                                *(float_t*)(d + 16) = i.value;
                                d += 20;
                            }
                            else {
                                *(float_t*)(d + 4) = i.random_range;
                                *(float_t*)(d + 8) = i.value;
                                d += 12;
                            }
                        }
                    else
                        for (Curve::Key& i : c->keys) {
                            *(int16_t*)d = (int16_t)i.type;
                            *(int16_t*)(d + 2) = (int16_t)i.frame;
                            if (i.type == KEY_HERMITE) {
                                *(float_t*)(d + 4) = i.tangent1;
                                *(float_t*)(d + 8) = i.tangent2;
                                *(float_t*)(d + 12) = i.value;
                                d += 16;
                            }
                            else {
                                *(float_t*)(d + 4) = i.value;
                                d += 8;
                            }
                        }
            }
            else {
                if (big_endian)
                    if (c->flags & CURVE_KEY_RANDOM_RANGE)
                        for (Curve::Key& i : c->keys) {
                            store_reverse_endianness_int16_t((void*)d, (int16_t)i.type);
                            store_reverse_endianness_int16_t((void*)(d + 2), (int16_t)i.frame);
                            if (i.type == KEY_HERMITE) {
                                store_reverse_endianness_float_t((void*)(d + 4), i.tangent1 * scale);
                                store_reverse_endianness_float_t((void*)(d + 8), i.tangent2 * scale);
                                store_reverse_endianness_float_t((void*)(d + 12), i.random_range * scale);
                                store_reverse_endianness_float_t((void*)(d + 16), i.value * scale);
                                d += 20;
                            }
                            else {
                                store_reverse_endianness_float_t((void*)(d + 4), i.random_range * scale);
                                store_reverse_endianness_float_t((void*)(d + 8), i.value * scale);
                                d += 12;
                            }
                        }
                    else
                        for (Curve::Key& i : c->keys) {
                            store_reverse_endianness_int16_t((void*)d, (int16_t)i.type);
                            store_reverse_endianness_int16_t((void*)(d + 2), (int16_t)i.frame);
                            if (i.type == KEY_HERMITE) {
                                store_reverse_endianness_float_t((void*)(d + 4), i.tangent1 * scale);
                                store_reverse_endianness_float_t((void*)(d + 8), i.tangent2 * scale);
                                store_reverse_endianness_float_t((void*)(d + 12), i.value * scale);
                                d += 16;
                            }
                            else {
                                store_reverse_endianness_float_t((void*)(d + 4), i.value * scale);
                                d += 8;
                            }
                        }
                else
                    if (c->flags & CURVE_KEY_RANDOM_RANGE)
                        for (Curve::Key& i : c->keys) {
                            *(int16_t*)d = (int16_t)i.type;
                            *(int16_t*)(d + 2) = (int16_t)i.frame;
                            if (i.type == KEY_HERMITE) {
                                *(float_t*)(d + 4) = i.tangent1 * scale;
                                *(float_t*)(d + 8) = i.tangent2 * scale;
                                *(float_t*)(d + 12) = i.random_range * scale;
                                *(float_t*)(d + 16) = i.value * scale;
                                d += 20;
                            }
                            else {
                                *(float_t*)(d + 4) = i.random_range * scale;
                                *(float_t*)(d + 8) = i.value * scale;
                                d += 12;
                            }
                        }
                    else
                        for (Curve::Key& i : c->keys) {
                            *(int16_t*)d = (int16_t)i.type;
                            *(int16_t*)(d + 2) = (int16_t)i.frame;
                            if (i.type == KEY_HERMITE) {
                                *(float_t*)(d + 4) = i.tangent1 * scale;
                                *(float_t*)(d + 8) = i.tangent2 * scale;
                                *(float_t*)(d + 12) = i.value * scale;
                                d += 16;
                            }
                            else {
                                *(float_t*)(d + 4) = i.value * scale;
                                d += 8;
                            }
                        }
            }
        }

        st->enrs = e;

        st->header.signature = reverse_endianness_uint32_t('KEYS');
        st->header.length = 0x20;
        st->header.use_big_endian = big_endian;
        st->header.use_section_size = true;
        st->header.version = c->keys_version;
    }

    bool FileWriter::PackDivaList(EffectGroup* eff_group, f2_struct* st, bool big_endian) {
        if (!eff_group->effects.size())
            return false;

        enrs e;
        enrs_entry ee;

        ee = { 0, 1, 4, 1 };
        ee.append(0, 1, ENRS_DWORD);
        e.vec.push_back(ee);

        size_t length = 0;
        for (Effect*& i : eff_group->effects)
            if (i)
                length++;

        st->data.resize(0x10 + 0x80 * length);
        size_t d = (size_t)st->data.data();

        if (big_endian)
            store_reverse_endianness_uint32_t((void*)d, (uint32_t)length);
        else
            *(uint32_t*)d = (uint32_t)length;
        d += 4;

        for (Effect*& i : eff_group->effects)
            if (i) {
                size_t size = min_def(i->name.size(), 0x7F);
                memcpy((void*)d, i->name.c_str(), size);
                memset((void*)(d + size), 0, 0x80 - size);
                d += 0x80;
            }

        st->enrs = e;

        st->header.signature = reverse_endianness_uint32_t('GEFF');
        st->header.length = 0x20;
        st->header.use_big_endian = big_endian;
        st->header.use_section_size = true;
        return true;
    }

    bool FileWriter::PackDivaResource(EffectGroup* eff_group, f2_struct* st) {
        if (eff_group->resources_tex.textures.size() < 1)
            return false;

        if (!eff_group->resources_tex.produce_enrs(&st->enrs))
            return false;

        eff_group->resources_tex.pack_file(st->data, false);

        st->header.signature = reverse_endianness_uint32_t('TXPC');
        st->header.length = 0x20;
        st->header.use_big_endian = false;
        st->header.use_section_size = true;
        return true;
    }

    bool FileWriter::PackDivaResourceHashes(EffectGroup* eff_group, f2_struct* st, bool big_endian) {
        if (eff_group->effects.size() < 1 || !eff_group->resources_count
            || eff_group->resource_hashes.size() < 1)
            return false;

        size_t count = eff_group->resources_count;

        size_t l = 0;

        enrs e;
        enrs_entry ee;

        ee = { 0, 2, (uint32_t)(8 + count * 8), 1 };
        ee.append(0, 1, ENRS_DWORD);
        ee.append(4, (uint32_t)count, ENRS_QWORD);
        e.vec.push_back(ee);
        l += 8 + count * 8;

        l = align_val(l, 0x10);
        st->data.resize(l);
        size_t d = (size_t)st->data.data();

        if (big_endian)
            store_reverse_endianness_int32_t((void*)d, (int32_t)count);
        else
            *(int32_t*)d = (int32_t)count;
        *(int32_t*)(d + 4) = 0;
        d += 8;

        if (big_endian) {
            uint64_t* hashes_src = eff_group->resource_hashes.data();
            uint64_t* hashes_dst = (uint64_t*)(void*)d;
            for (size_t i = 0; i < count; i++)
                store_reverse_endianness_uint64_t(hashes_dst++, *hashes_src++);
        }
        else
            memcpy((void*)d, eff_group->resource_hashes.data(), sizeof(uint64_t) * count);

        st->enrs = e;

        st->header.signature = reverse_endianness_uint32_t('DVRS');
        st->header.length = 0x20;
        st->header.use_big_endian = big_endian;
        st->header.use_section_size = true;
        return true;
    }

    bool FileWriter::PackEffect(f2_struct* st, Effect* eff, bool big_endian) {
        if (type != Glitter::X && (eff->version < 6 || eff->version > 7)
            || type == Glitter::X && (eff->version < 8 || eff->version > 12))
            return false;

        Effect::ExtAnim* ext_anim = eff->data.ext_anim;

        int32_t type;
        if (eff->data.flags & EFFECT_LOCAL)
            type = 1;
        else if (!ext_anim)
            type = 0;
        else if (ext_anim->flags & EFFECT_EXT_ANIM_CHARA_ANIM)
            type = 2;
        else
            type = 3;

        size_t l = 0;

        uint32_t o;
        enrs e;
        enrs_entry ee;

        if (this->type == Glitter::X) {
            if (eff->version != 8) {
                ee = { 0, 16, 80, 1 };
                ee.append(0, 1, ENRS_QWORD);
                ee.append(0, 3, ENRS_DWORD);
                ee.append(4, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                e.vec.push_back(ee);
                l += o = 80;
            }
            else {
                ee = { 0, 18, 88, 1 };
                ee.append(0, 1, ENRS_QWORD);
                ee.append(0, 3, ENRS_DWORD);
                ee.append(4, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                e.vec.push_back(ee);
                l += o = 88;
            }

            if (type == 2) {
                ee = { o, 1, 12, 1 };
                ee.append(0, 3, ENRS_DWORD);
                e.vec.push_back(ee);
                l += o = 12;
            }
            else if (type == 3) {
                if (eff->version == 8) {
                    ee = { o, 2, 140, 1 };
                    ee.append(0, 1, ENRS_QWORD);
                    ee.append(0, 1, ENRS_DWORD);
                    e.vec.push_back(ee);
                    l += o = 140;
                }
                else if (eff->version == 10) {
                    ee = { o, 2, 144, 1 };
                    ee.append(0, 1, ENRS_QWORD);
                    ee.append(0, 2, ENRS_DWORD);
                    e.vec.push_back(ee);
                    l += o = 144;
                }
                else {
                    ee = { o, 4, 160, 1 };
                    ee.append(0, 1, ENRS_QWORD);
                    ee.append(0, 2, ENRS_DWORD);
                    ee.append(0, 1, ENRS_QWORD);
                    ee.append(0, 2, ENRS_DWORD);
                    e.vec.push_back(ee);
                    l += o = 160;
                }
            }
        }
        else {
            if (eff->version == 7) {
                ee = { 0, 12, 64, 1 };
                ee.append(0, 1, ENRS_QWORD);
                ee.append(0, 3, ENRS_DWORD);
                ee.append(4, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                e.vec.push_back(ee);
                l += o = 64;
            }
            else {
                ee = { 0, 11, 64, 1 };
                ee.append(0, 1, ENRS_QWORD);
                ee.append(0, 3, ENRS_DWORD);
                ee.append(4, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                e.vec.push_back(ee);
                l += o = 60;
            }

            if (type == 2) {
                ee = { 0, 1, 12, 1 };
                ee.append(0, 3, ENRS_DWORD);
                e.vec.push_back(ee);
                l += o = 12;
            }
            else if (type == 3) {
                ee = { 0, 2, 140, 1 };
                ee.append(0, 1, ENRS_QWORD);
                ee.append(0, 1, ENRS_DWORD);
                e.vec.push_back(ee);
                l += o = 140;
            }
        }

        l = align_val(l, 0x10);
        st->data.resize(l);
        size_t d = (size_t)st->data.data();

        EffectFileFlag flags = (EffectFileFlag)0;
        if (eff->data.flags & EFFECT_ALPHA)
            enum_or(flags, EFFECT_FILE_ALPHA);

        if (eff->data.flags & EFFECT_FOG)
            enum_or(flags, EFFECT_FILE_FOG);
        else if (eff->data.flags & EFFECT_FOG_HEIGHT)
            enum_or(flags, EFFECT_FILE_FOG_HEIGHT);

        if (eff->data.flags & EFFECT_EMISSION)
            enum_or(flags, EFFECT_FILE_EMISSION);

        if (this->type == Glitter::X) {
            if (eff->data.flags & EFFECT_USE_SEED)
                enum_or(flags, EFFECT_FILE_USE_SEED);

            if (eff->version != 8 && eff->data.flags & 0x80)
                enum_or(flags, 0x20);

            if (big_endian) {
                store_reverse_endianness_uint64_t((void*)d, (uint64_t)hash_string_murmurhash(eff->name));
                store_reverse_endianness_int32_t((void*)(d + 8), eff->data.appear_time);
                store_reverse_endianness_int32_t((void*)(d + 12), eff->data.life_time);
                store_reverse_endianness_int32_t((void*)(d + 16), eff->data.start_time);
                *(uint8_t*)(d + 20) = eff->data.color.b;
                *(uint8_t*)(d + 21) = eff->data.color.g;
                *(uint8_t*)(d + 22) = eff->data.color.r;
                *(uint8_t*)(d + 23) = eff->data.color.a;
                store_reverse_endianness_int32_t((void*)(d + 24), eff->data.flags & EFFECT_LOOP ? 1 : 0);
                store_reverse_endianness_float_t((void*)(d + 28), eff->translation.x);
                store_reverse_endianness_float_t((void*)(d + 32), eff->translation.y);
                store_reverse_endianness_float_t((void*)(d + 36), eff->translation.z);
                store_reverse_endianness_float_t((void*)(d + 40), eff->rotation.x);
                store_reverse_endianness_float_t((void*)(d + 40), eff->rotation.y);
                store_reverse_endianness_float_t((void*)(d + 40), eff->rotation.z);
                store_reverse_endianness_int32_t((void*)(d + 52), flags);
            }
            else {
                *(uint64_t*)d = hash_string_murmurhash(eff->name);
                *(int32_t*)(d + 8) = eff->data.appear_time;
                *(int32_t*)(d + 12) = eff->data.life_time;
                *(uint8_t*)(d + 20) = eff->data.color.b;
                *(uint8_t*)(d + 21) = eff->data.color.g;
                *(uint8_t*)(d + 22) = eff->data.color.r;
                *(uint8_t*)(d + 23) = eff->data.color.a;
                *(int32_t*)(d + 24) = eff->data.flags & EFFECT_LOOP ? 1 : 0;
                *(vec3*)(d + 28) = eff->translation;
                *(vec3*)(d + 40) = eff->rotation;
                *(int32_t*)(d + 52) = flags;
            }
            d += 56;

            if (big_endian) {
                store_reverse_endianness_float_t((void*)d, eff->data.emission);
                store_reverse_endianness_int32_t((void*)(d + 4), eff->data.seed);
                store_reverse_endianness_float_t((void*)(d + 8), eff->data.ext_anim_scale_start_time);
            }
            else {
                *(float_t*)d = eff->data.emission;
                *(int32_t*)(d + 4) = eff->data.seed;
                *(float_t*)(d + 8) = eff->data.ext_anim_scale_start_time;
            }
            d += 12;

            if (eff->version != 8)
                d += 8;
            else
                d += 12;

            if (big_endian)
                store_reverse_endianness_int32_t((void*)d, type);
            else
                *(int32_t*)d = type;

            if (eff->version != 8)
                d += 4;
            else
                d += 8;

            Effect::ExtAnimX* ext_anim_x = eff->data.ext_anim_x;
            if (type == 2) {
                EffectExtAnimFlag ext_anim_flag = ext_anim_x->flags;
                enum_and(ext_anim_flag, ~EFFECT_EXT_ANIM_CHARA_ANIM);

                if (big_endian) {
                    store_reverse_endianness_int32_t((void*)d, ext_anim_x->chara_index);
                    store_reverse_endianness_int32_t((void*)(d + 4), ext_anim_flag);
                    store_reverse_endianness_int32_t((void*)(d + 8), ext_anim_x->node_index);
                }
                else {
                    *(int32_t*)d = ext_anim_x->chara_index;
                    *(int32_t*)(d + 4) = ext_anim_flag;
                    *(int32_t*)(d + 8) = ext_anim_x->node_index;
                }
            }
            else if (type == 3) {
                if (eff->version == 8) {
                    if (big_endian) {
                        store_reverse_endianness_uint64_t((void*)d, (uint64_t)ext_anim_x->object_hash);
                        store_reverse_endianness_int32_t((void*)(d + 8), ext_anim_x->flags);
                    }
                    else {
                        *(uint64_t*)d = ext_anim_x->object_hash;
                        *(int32_t*)(d + 8) = ext_anim_x->flags;
                    }

                    if (ext_anim_x->mesh_name[0]) {
                        strncpy_s((char*)(d + 12), 0x80, ext_anim_x->mesh_name, 0x7F);
                        ((char*)(d + 12))[0x7F] = 0;
                    }
                    else
                        *(char*)(d + 12) = 0;
                }
                else if (eff->version == 10) {
                    if (big_endian) {
                        store_reverse_endianness_uint64_t((void*)d, (uint64_t)ext_anim_x->object_hash);
                        store_reverse_endianness_int32_t((void*)(d + 8), ext_anim_x->flags);
                    }
                    else {
                        *(uint64_t*)d = ext_anim_x->object_hash;
                        *(int32_t*)(d + 8) = ext_anim_x->flags;
                    }

                    if (ext_anim_x->mesh_name[0]) {
                        strncpy_s((char*)(d + 16), 0x80, ext_anim_x->mesh_name, 0x7F);
                        ((char*)(d + 16))[0x7F] = 0;
                    }
                    else
                        *(char*)(d + 16) = 0;
                }
                else {
                    if (big_endian) {
                        store_reverse_endianness_uint64_t((void*)d, (uint64_t)ext_anim_x->object_hash);
                        store_reverse_endianness_int32_t((void*)(d + 8), ext_anim_x->flags);
                        store_reverse_endianness_int32_t((void*)(d + 12), ext_anim_x->instance_id);
                        store_reverse_endianness_uint64_t((void*)(d + 16), (uint64_t)ext_anim_x->file_name_hash);
                    }
                    else {
                        *(uint64_t*)d = ext_anim_x->object_hash;
                        *(int32_t*)(d + 8) = ext_anim_x->flags;
                        *(int32_t*)(d + 12) = ext_anim_x->instance_id;
                        *(uint64_t*)(d + 16) = ext_anim_x->file_name_hash;
                    }

                    if (ext_anim_x->mesh_name[0]) {
                        strncpy_s((char*)(d + 32), 0x80, ext_anim_x->mesh_name, 0x7F);
                        ((char*)(d + 32))[0x7F] = 0;
                    }
                    else
                        *(char*)(d + 32) = 0;
                }
            }
        }
        else {
            if (big_endian) {
                store_reverse_endianness_uint64_t((void*)d, type != Glitter::FT
                    ? hash_string_murmurhash(eff->name) : hash_string_fnv1a64m(eff->name));
                store_reverse_endianness_int32_t((void*)(d + 8), eff->data.appear_time);
                store_reverse_endianness_int32_t((void*)(d + 12), eff->data.life_time);
                store_reverse_endianness_int32_t((void*)(d + 16), eff->data.start_time);
                *(uint8_t*)(d + 20) = eff->data.color.b;
                *(uint8_t*)(d + 21) = eff->data.color.g;
                *(uint8_t*)(d + 22) = eff->data.color.r;
                *(uint8_t*)(d + 23) = eff->data.color.a;
                store_reverse_endianness_int32_t((void*)(d + 24), eff->data.flags & EFFECT_LOOP ? 1 : 0);
                store_reverse_endianness_float_t((void*)(d + 28), eff->translation.x);
                store_reverse_endianness_float_t((void*)(d + 32), eff->translation.y);
                store_reverse_endianness_float_t((void*)(d + 36), eff->translation.z);
                store_reverse_endianness_float_t((void*)(d + 40), eff->rotation.x);
                store_reverse_endianness_float_t((void*)(d + 40), eff->rotation.y);
                store_reverse_endianness_float_t((void*)(d + 40), eff->rotation.z);
                store_reverse_endianness_int32_t((void*)(d + 52), flags);
            }
            else {
                *(uint64_t*)d = type != Glitter::FT
                    ? hash_string_murmurhash(eff->name) : hash_string_fnv1a64m(eff->name);
                *(int32_t*)(d + 8) = eff->data.appear_time;
                *(int32_t*)(d + 12) = eff->data.life_time;
                *(int32_t*)(d + 16) = eff->data.start_time;
                *(uint8_t*)(d + 20) = eff->data.color.b;
                *(uint8_t*)(d + 21) = eff->data.color.g;
                *(uint8_t*)(d + 22) = eff->data.color.r;
                *(uint8_t*)(d + 23) = eff->data.color.a;
                *(int32_t*)(d + 24) = eff->data.flags & EFFECT_LOOP ? 1 : 0;
                *(vec3*)(d + 28) = eff->translation;
                *(vec3*)(d + 40) = eff->rotation;
                *(int32_t*)(d + 52) = flags;
            }
            d += 56;

            if (eff->version == 7) {
                if (big_endian)
                    store_reverse_endianness_float_t((void*)d, eff->data.emission);
                else
                    *(float_t*)d = eff->data.emission;
                d += 4;
            }

            if (big_endian)
                store_reverse_endianness_int32_t((void*)d, type);
            else
                *(int32_t*)d = type;
            d += 4;

            if (type == 2) {
                EffectExtAnimFlag ext_anim_flag = ext_anim->flags;
                enum_and(ext_anim_flag, ~EFFECT_EXT_ANIM_CHARA_ANIM);

                if (big_endian) {
                    store_reverse_endianness_int32_t((void*)d, ext_anim->chara_index);
                    store_reverse_endianness_int32_t((void*)(d + 4), ext_anim_flag);
                    store_reverse_endianness_int32_t((void*)(d + 8), ext_anim->node_index);
                }
                else {
                    *(int32_t*)d = ext_anim->chara_index;
                    *(int32_t*)(d + 4) = ext_anim_flag;
                    *(int32_t*)(d + 8) = ext_anim->node_index;
                }
            }
            else if (type == 3) {
                if (big_endian) {
                    store_reverse_endianness_uint64_t((void*)d, ext_anim->object_hash);
                    store_reverse_endianness_int32_t((void*)(d + 8), ext_anim->flags);
                }
                else {
                    *(uint64_t*)d = ext_anim->object_hash;
                    *(int32_t*)(d + 8) = ext_anim->flags;
                }

                if (ext_anim->mesh_name[0]) {
                    strncpy_s((char*)(d + 12), 0x80, ext_anim->mesh_name, 0x7F);
                    ((char*)(d + 12))[0x7F] = 0;
                }
                else
                    *(char*)(d + 12) = 0;
            }
        }

        st->enrs = e;

        st->header.signature = reverse_endianness_uint32_t('EFCT');
        st->header.length = 0x20;
        st->header.use_big_endian = big_endian;
        st->header.use_section_size = true;
        st->header.version = eff->version;
        return true;
    }

    bool FileWriter::PackEmitter(f2_struct* st, Emitter* emit, bool big_endian) {
        if (type != Glitter::X && (emit->version < 1 || emit->version > 2)
            || type == Glitter::X && (emit->version < 3 || emit->version > 4))
            return false;

        switch (emit->data.type) {
        case EMITTER_BOX:
        case EMITTER_CYLINDER:
        case EMITTER_SPHERE:
        case EMITTER_POLYGON:
            break;
        default:
            return false;
        }

        size_t l = 0;

        uint32_t o;
        enrs e;
        enrs_entry ee;

        if (type == Glitter::X) {
            if (emit->version == 3) {
                ee = { 0, 5, 116, 1 };
                ee.append(0, 5, ENRS_DWORD);
                ee.append(0, 2, ENRS_WORD);
                ee.append(0, 2, ENRS_DWORD);
                ee.append(0, 2, ENRS_WORD);
                ee.append(0, 20, ENRS_DWORD);
                e.vec.push_back(ee);
                l += o = 116;
            }
            else {
                ee = { 0, 5, 112, 1 };
                ee.append(0, 5, ENRS_DWORD);
                ee.append(0, 2, ENRS_WORD);
                ee.append(0, 2, ENRS_DWORD);
                ee.append(0, 2, ENRS_WORD);
                ee.append(0, 19, ENRS_DWORD);
                e.vec.push_back(ee);
                l += o = 112;
            }

            switch (emit->data.type) {
            case EMITTER_BOX:
                ee = { o, 1, 12, 1 };
                ee.append(0, 3, ENRS_DWORD);
                e.vec.push_back(ee);
                l += o = 12;
                break;
            case EMITTER_CYLINDER:
                ee = { o, 1, 20, 1 };
                ee.append(0, 5, ENRS_DWORD);
                e.vec.push_back(ee);
                l += o = 20;
                break;
            case EMITTER_SPHERE:
                ee = { o, 1, 16, 1 };
                ee.append(0, 4, ENRS_DWORD);
                e.vec.push_back(ee);
                l += o = 16;
                break;
            case EMITTER_POLYGON:
                ee = { o, 1, 12, 1 };
                ee.append(0, 3, ENRS_DWORD);
                e.vec.push_back(ee);
                l += o = 12;
                break;
            }

            l = align_val(l, 0x10);
            st->data.resize(l);
            size_t d = (size_t)st->data.data();

            if (big_endian) {
                store_reverse_endianness_int32_t((void*)d, emit->data.start_time);
                store_reverse_endianness_int32_t((void*)(d + 4), emit->data.life_time);
                store_reverse_endianness_int32_t((void*)(d + 8), emit->data.loop_start_time);
                store_reverse_endianness_int32_t((void*)(d + 12), emit->data.loop_end_time);
                store_reverse_endianness_int32_t((void*)(d + 16), emit->data.flags);
                store_reverse_endianness_int32_t((void*)(d + 20), (int16_t)emit->data.type);
                store_reverse_endianness_int32_t((void*)(d + 22), (int16_t)emit->data.direction);
                store_reverse_endianness_float_t((void*)(d + 24), emit->data.emission_interval);
                store_reverse_endianness_float_t((void*)(d + 28), emit->data.particles_per_emission);
                store_reverse_endianness_int16_t((void*)(d + 32), (int16_t)emit->data.timer);
                store_reverse_endianness_int16_t((void*)(d + 34), (int16_t)0);
                store_reverse_endianness_int32_t((void*)(d + 36), emit->data.seed);
            }
            else {
                *(int32_t*)d = emit->data.start_time;
                *(int32_t*)(d + 4) = emit->data.life_time;
                *(int32_t*)(d + 8) = emit->data.loop_start_time;
                *(int32_t*)(d + 12) = emit->data.loop_end_time;
                *(int32_t*)(d + 16) = emit->data.flags;
                *(int16_t*)(d + 20) = (int16_t)emit->data.type;
                *(int16_t*)(d + 22) = (int16_t)emit->data.direction;
                *(float_t*)(d + 24) = emit->data.emission_interval;
                *(float_t*)(d + 28) = emit->data.particles_per_emission;
                *(int16_t*)(d + 32) = (int16_t)emit->data.timer;
                *(int16_t*)(d + 34) = 0;
                *(int32_t*)(d + 36) = emit->data.seed;
            }
            d += emit->version == 3 ? 56 : 52;

            if (big_endian) {
                store_reverse_endianness_float_t((void*)d, emit->translation.x);
                store_reverse_endianness_float_t((void*)(d + 4), emit->translation.y);
                store_reverse_endianness_float_t((void*)(d + 8), emit->translation.z);
                store_reverse_endianness_float_t((void*)(d + 12), emit->rotation.x);
                store_reverse_endianness_float_t((void*)(d + 16), emit->rotation.y);
                store_reverse_endianness_float_t((void*)(d + 20), emit->rotation.z);
                store_reverse_endianness_float_t((void*)(d + 24), emit->scale.x);
                store_reverse_endianness_float_t((void*)(d + 28), emit->scale.y);
                store_reverse_endianness_float_t((void*)(d + 32), emit->scale.z);
                store_reverse_endianness_float_t((void*)(d + 36), emit->data.rotation_add.x);
                store_reverse_endianness_float_t((void*)(d + 40), emit->data.rotation_add.y);
                store_reverse_endianness_float_t((void*)(d + 44), emit->data.rotation_add.z);
                store_reverse_endianness_float_t((void*)(d + 48), emit->data.rotation_add_random.x);
                store_reverse_endianness_float_t((void*)(d + 52), emit->data.rotation_add_random.y);
                store_reverse_endianness_float_t((void*)(d + 56), emit->data.rotation_add_random.z);
            }
            else {
                *(vec3*)d = emit->translation;
                *(vec3*)(d + 12) = emit->rotation;
                *(vec3*)(d + 24) = emit->scale;
                *(vec3*)(d + 36) = emit->data.rotation_add;
                *(vec3*)(d + 48) = emit->data.rotation_add_random;
            }
            d += 60;

            switch (emit->data.type) {
            case EMITTER_BOX:
                if (big_endian) {
                    store_reverse_endianness_float_t((void*)d, emit->data.box.size.x);
                    store_reverse_endianness_float_t((void*)(d + 4), emit->data.box.size.y);
                    store_reverse_endianness_float_t((void*)(d + 8), emit->data.box.size.z);
                }
                else
                    *(vec3*)d = emit->data.box.size;
                break;
            case EMITTER_CYLINDER:
                if (big_endian) {
                    store_reverse_endianness_float_t((void*)d, emit->data.cylinder.radius);
                    store_reverse_endianness_float_t((void*)(d + 4), emit->data.cylinder.height);
                    store_reverse_endianness_float_t((void*)(d + 8), emit->data.cylinder.start_angle);
                    store_reverse_endianness_float_t((void*)(d + 12), emit->data.cylinder.end_angle);
                    store_reverse_endianness_int32_t((void*)(d + 16), ((int32_t)emit->data.cylinder.direction << 1)
                        | (emit->data.cylinder.on_edge ? 1 : 0));
                }
                else {
                    *(float_t*)d = emit->data.cylinder.radius;
                    *(float_t*)(d + 4) = emit->data.cylinder.height;
                    *(float_t*)(d + 8) = emit->data.cylinder.start_angle;
                    *(float_t*)(d + 12) = emit->data.cylinder.end_angle;
                    *(int32_t*)(d + 16) = ((int32_t)emit->data.cylinder.direction << 1)
                        | (emit->data.cylinder.on_edge ? 1 : 0);
                }
                break;
            case EMITTER_SPHERE:
                if (big_endian) {
                    store_reverse_endianness_float_t((void*)d, emit->data.sphere.radius);
                    store_reverse_endianness_float_t((void*)(d + 4), emit->data.sphere.latitude);
                    store_reverse_endianness_float_t((void*)(d + 8), emit->data.sphere.longitude);
                    store_reverse_endianness_int32_t((void*)(d + 12), ((int32_t)emit->data.sphere.direction << 1)
                        | (emit->data.sphere.on_edge ? 1 : 0));
                }
                else {
                    *(float_t*)d = emit->data.sphere.radius;
                    *(float_t*)(d + 4) = emit->data.sphere.latitude;
                    *(float_t*)(d + 8) = emit->data.sphere.longitude;
                    *(int32_t*)(d + 12) = ((int32_t)emit->data.sphere.direction << 1)
                        | (emit->data.sphere.on_edge ? 1 : 0);
                }
                break;
            case EMITTER_POLYGON:
                if (big_endian) {
                    store_reverse_endianness_float_t((void*)d, emit->data.polygon.size);
                    store_reverse_endianness_int32_t((void*)(d + 4), emit->data.polygon.count);
                    store_reverse_endianness_int32_t((void*)(d + 8), (int32_t)emit->data.polygon.direction << 1);
                }
                else {
                    *(float_t*)d = emit->data.polygon.size;
                    *(int32_t*)(d + 4) = emit->data.polygon.count;
                    *(int32_t*)(d + 8) = (int32_t)emit->data.polygon.direction << 1;
                }
                break;
            }
        }
        else {
            ee = { 0, 5, 96, 1 };
            ee.append(0, 5, ENRS_DWORD);
            ee.append(0, 2, ENRS_WORD);
            ee.append(0, 2, ENRS_DWORD);
            ee.append(0, 2, ENRS_WORD);
            ee.append(0, 15, ENRS_DWORD);
            e.vec.push_back(ee);
            l += o = 96;

            switch (emit->data.type) {
            case EMITTER_BOX:
                ee = { o, 1, 12, 1 };
                ee.append(0, 3, ENRS_DWORD);
                e.vec.push_back(ee);
                l += o = 12;
                break;
            case EMITTER_CYLINDER:
                ee = { o, 1, 20, 1 };
                ee.append(0, 5, ENRS_DWORD);
                e.vec.push_back(ee);
                l += o = 20;
                break;
            case EMITTER_SPHERE:
                ee = { o, 1, 16, 1 };
                ee.append(0, 4, ENRS_DWORD);
                e.vec.push_back(ee);
                l += o = 16;
                break;
            case EMITTER_POLYGON:
                ee = { o, 1, 8, 1 };
                ee.append(0, 2, ENRS_DWORD);
                e.vec.push_back(ee);
                l += o = 8;
                break;
            }

            l = align_val(l, 0x10);
            st->data.resize(l);
            size_t d = (size_t)st->data.data();

            if (big_endian) {
                store_reverse_endianness_int32_t((void*)d, emit->data.start_time);
                store_reverse_endianness_int32_t((void*)(d + 4), emit->data.life_time);
                store_reverse_endianness_int32_t((void*)(d + 8), emit->data.loop_start_time);
                store_reverse_endianness_int32_t((void*)(d + 12), emit->data.loop_end_time);
                store_reverse_endianness_int32_t((void*)(d + 16), emit->data.flags);
                store_reverse_endianness_int32_t((void*)(d + 20), (int16_t)emit->data.type);
                store_reverse_endianness_int32_t((void*)(d + 22), (int16_t)emit->data.direction);
                store_reverse_endianness_float_t((void*)(d + 24), emit->data.emission_interval);
                store_reverse_endianness_float_t((void*)(d + 28), emit->data.particles_per_emission);
                store_reverse_endianness_int16_t((void*)(d + 32), (int16_t)0);
                store_reverse_endianness_int16_t((void*)(d + 34), (int16_t)0);

                store_reverse_endianness_float_t((void*)(d + 36), emit->translation.x);
                store_reverse_endianness_float_t((void*)(d + 40), emit->translation.y);
                store_reverse_endianness_float_t((void*)(d + 44), emit->translation.z);
                store_reverse_endianness_float_t((void*)(d + 48), emit->rotation.x);
                store_reverse_endianness_float_t((void*)(d + 52), emit->rotation.y);
                store_reverse_endianness_float_t((void*)(d + 56), emit->rotation.z);
                store_reverse_endianness_float_t((void*)(d + 60), emit->scale.x);
                store_reverse_endianness_float_t((void*)(d + 64), emit->scale.y);
                store_reverse_endianness_float_t((void*)(d + 68), emit->scale.z);
                store_reverse_endianness_float_t((void*)(d + 72), emit->data.rotation_add.x);
                store_reverse_endianness_float_t((void*)(d + 76), emit->data.rotation_add.y);
                store_reverse_endianness_float_t((void*)(d + 80), emit->data.rotation_add.z);
                store_reverse_endianness_float_t((void*)(d + 84), emit->data.rotation_add_random.x);
                store_reverse_endianness_float_t((void*)(d + 88), emit->data.rotation_add_random.y);
                store_reverse_endianness_float_t((void*)(d + 92), emit->data.rotation_add_random.z);
            }
            else {
                *(int32_t*)d = emit->data.start_time;
                *(int32_t*)(d + 4) = emit->data.life_time;
                *(int32_t*)(d + 8) = emit->data.loop_start_time;
                *(int32_t*)(d + 12) = emit->data.loop_end_time;
                *(int32_t*)(d + 16) = emit->data.flags;
                *(int16_t*)(d + 20) = (int16_t)emit->data.type;
                *(int16_t*)(d + 22) = (int16_t)emit->data.direction;
                *(float_t*)(d + 24) = emit->data.emission_interval;
                *(float_t*)(d + 28) = emit->data.particles_per_emission;
                *(int16_t*)(d + 32) = 0;
                *(int16_t*)(d + 34) = 0;

                *(vec3*)(d + 36) = emit->translation;
                *(vec3*)(d + 48) = emit->rotation;
                *(vec3*)(d + 60) = emit->scale;
                *(vec3*)(d + 72) = emit->data.rotation_add;
                *(vec3*)(d + 84) = emit->data.rotation_add_random;
            }
            d += 96;

            switch (emit->data.type) {
            case EMITTER_BOX:
                if (big_endian) {
                    store_reverse_endianness_float_t((void*)d, emit->data.box.size.x);
                    store_reverse_endianness_float_t((void*)(d + 4), emit->data.box.size.y);
                    store_reverse_endianness_float_t((void*)(d + 8), emit->data.box.size.z);
                }
                else
                    *(vec3*)d = emit->data.box.size;
                break;
            case EMITTER_CYLINDER:
                if (big_endian) {
                    store_reverse_endianness_float_t((void*)d, emit->data.cylinder.radius);
                    store_reverse_endianness_float_t((void*)(d + 4), emit->data.cylinder.height);
                    store_reverse_endianness_float_t((void*)(d + 8), emit->data.cylinder.start_angle);
                    store_reverse_endianness_float_t((void*)(d + 12), emit->data.cylinder.end_angle);
                    store_reverse_endianness_int32_t((void*)(d + 16), ((int32_t)emit->data.cylinder.direction << 1)
                        | (emit->data.cylinder.on_edge ? 1 : 0));
                }
                else {
                    *(float_t*)d = emit->data.cylinder.radius;
                    *(float_t*)(d + 4) = emit->data.cylinder.height;
                    *(float_t*)(d + 8) = emit->data.cylinder.start_angle;
                    *(float_t*)(d + 12) = emit->data.cylinder.end_angle;
                    *(int32_t*)(d + 16) = ((int32_t)emit->data.cylinder.direction << 1)
                        | (emit->data.cylinder.on_edge ? 1 : 0);
                }
                break;
            case EMITTER_SPHERE:
                if (big_endian) {
                    store_reverse_endianness_float_t((void*)d, emit->data.sphere.radius);
                    store_reverse_endianness_float_t((void*)(d + 4), emit->data.sphere.latitude);
                    store_reverse_endianness_float_t((void*)(d + 8), emit->data.sphere.longitude);
                    store_reverse_endianness_int32_t((void*)(d + 12), ((int32_t)emit->data.sphere.direction << 1)
                        | (emit->data.sphere.on_edge ? 1 : 0));
                }
                else {
                    *(float_t*)d = emit->data.sphere.radius;
                    *(float_t*)(d + 4) = emit->data.sphere.latitude;
                    *(float_t*)(d + 8) = emit->data.sphere.longitude;
                    *(int32_t*)(d + 12) = ((int32_t)emit->data.sphere.direction << 1)
                        | (emit->data.sphere.on_edge ? 1 : 0);
                }
                break;
            case EMITTER_POLYGON:
                if (big_endian) {
                    store_reverse_endianness_float_t((void*)d, emit->data.polygon.size);
                    store_reverse_endianness_int32_t((void*)(d + 4), emit->data.polygon.count);
                }
                else {
                    *(float_t*)d = emit->data.polygon.size;
                    *(int32_t*)(d + 4) = emit->data.polygon.count;
                }
                break;
            }
        }

        st->enrs = e;

        st->header.signature = reverse_endianness_uint32_t('EMIT');
        st->header.length = 0x20;
        st->header.use_big_endian = big_endian;
        st->header.use_section_size = true;
        st->header.version = emit->version;
        return true;
    }

    bool FileWriter::PackParticle(EffectGroup* eff_group,
        f2_struct* st, Particle* ptcl, Effect* eff, bool big_endian) {
        if (type != Glitter::X && ptcl->version < 2
            || type == Glitter::X && (ptcl->version < 4 || ptcl->version > 5))
            return false;

        size_t l = 0;

        uint32_t o;
        enrs e;
        enrs_entry ee;

        if (type == Glitter::X) {
            ee = { 0, 1, 256, 1 };
            ee.append(0, 64, ENRS_DWORD);
            e.vec.push_back(ee);
            l += o = 256;

            if (ptcl->data.type == PARTICLE_LOCUS) {
                ee = { o, 1, 4, 1 };
                ee.append(0, 2, ENRS_WORD);
                e.vec.push_back(ee);
                l += o = 4;
            }
            else if (ptcl->data.type == PARTICLE_MESH) {
                ee = { o, 2, 88, 1 };
                ee.append(0, 2, ENRS_QWORD);
                ee.append(64, 1, ENRS_QWORD);
                e.vec.push_back(ee);
                l += o = 88;
            }

            ee = { o, 11, 48, 1 };
            ee.append(0, 1, ENRS_QWORD);
            ee.append(4, 1, ENRS_DWORD);
            ee.append(0, 1, ENRS_DWORD);
            ee.append(0, 1, ENRS_DWORD);
            ee.append(0, 1, ENRS_DWORD);
            ee.append(0, 1, ENRS_DWORD);
            ee.append(0, 1, ENRS_WORD);
            ee.append(0, 1, ENRS_WORD);
            ee.append(0, 1, ENRS_DWORD);
            ee.append(0, 1, ENRS_DWORD);
            ee.append(0, 1, ENRS_DWORD);
            e.vec.push_back(ee);
            l += o = 48;

            if (ptcl->data.flags & PARTICLE_TEXTURE_MASK) {
                ee = { o, 2, 16, 1 };
                ee.append(0, 1, ENRS_QWORD);
                ee.append(0, 2, ENRS_DWORD);
                e.vec.push_back(ee);
                l += o = 16;
            }
        }
        else {
            if (ptcl->version == 3) {
                ee = { 0, 1, 212, 1 };
                ee.append(0, 53, ENRS_DWORD);
                e.vec.push_back(ee);
                l += o = 212;
            }
            else {
                ee = { 0, 1, 204, 1 };
                ee.append(0, 51, ENRS_DWORD);
                e.vec.push_back(ee);
                l += o = 204;
            }

            if (ptcl->data.type == PARTICLE_LOCUS || ptcl->data.type == PARTICLE_MESH) {
                ee = { o, 1, 4, 1 };
                ee.append(0, 2, ENRS_WORD);
                e.vec.push_back(ee);
                l += o = 4;
            }

            if (eff_group->version >= 7) {
                ee = { o, 11, 48, 1 };
                ee.append(0, 1, ENRS_QWORD);
                ee.append(4, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_WORD);
                ee.append(0, 1, ENRS_WORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                e.vec.push_back(ee);
                l += o = 48;
            }
            else {
                ee = { o, 10, 44, 1 };
                ee.append(0, 1, ENRS_QWORD);
                ee.append(4, 5, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_WORD);
                ee.append(0, 1, ENRS_WORD);
                ee.append(0, 1, ENRS_DWORD);
                ee.append(0, 1, ENRS_DWORD);
                e.vec.push_back(ee);
                l += o = 44;
            }

            if (ptcl->data.flags & PARTICLE_TEXTURE_MASK) {
                ee = { o, 2, 16, 1 };
                ee.append(0, 1, ENRS_QWORD);
                ee.append(0, 2, ENRS_DWORD);
                e.vec.push_back(ee);
                l += o = 16;
            }
        }

        l = align_val(l, 0x10);
        st->data.resize(l);
        size_t d = (size_t)st->data.data();

        ParticleFlag flags = ptcl->data.flags;
        if (eff->data.flags & EFFECT_LOCAL)
            enum_and(flags, ~PARTICLE_LOCAL);
        if (eff->data.flags & EFFECT_EMISSION)
            enum_and(flags, ~PARTICLE_EMISSION);

        if (type == Glitter::X) {
            int32_t type = ptcl->data.type;
            switch (type) {
            case PARTICLE_LINE:
                type = 3;
                break;
            case PARTICLE_MESH:
                type = 1;
                break;
            }

            vec3 direction = ptcl->data.direction;
            vec3 direction_random = ptcl->data.direction_random;

            if (ptcl->version == 4) {
                direction /= 10.0f;
                direction_random /= 10.0f;
            }

            if (big_endian) {
                store_reverse_endianness_int32_t((void*)d, ptcl->data.life_time);
                store_reverse_endianness_int32_t((void*)(d + 4), ptcl->data.life_time_random);
                store_reverse_endianness_int32_t((void*)(d + 8), ptcl->data.fade_in);
                store_reverse_endianness_int32_t((void*)(d + 12), ptcl->data.fade_in_random);
                store_reverse_endianness_int32_t((void*)(d + 16), ptcl->data.fade_out);
                store_reverse_endianness_int32_t((void*)(d + 20), ptcl->data.fade_out_random);
                store_reverse_endianness_int32_t((void*)(d + 24), type);
                store_reverse_endianness_int32_t((void*)(d + 28), ptcl->data.draw_type);
                store_reverse_endianness_float_t((void*)(d + 32), ptcl->data.rotation.x);
                store_reverse_endianness_float_t((void*)(d + 36), ptcl->data.rotation.y);
                store_reverse_endianness_float_t((void*)(d + 40), ptcl->data.rotation.z);
                store_reverse_endianness_float_t((void*)(d + 44), ptcl->data.rotation_random.x);
                store_reverse_endianness_float_t((void*)(d + 48), ptcl->data.rotation_random.y);
                store_reverse_endianness_float_t((void*)(d + 52), ptcl->data.rotation_random.z);
                store_reverse_endianness_float_t((void*)(d + 56), ptcl->data.rotation_add.x);
                store_reverse_endianness_float_t((void*)(d + 60), ptcl->data.rotation_add.y);
                store_reverse_endianness_float_t((void*)(d + 64), ptcl->data.rotation_add.z);
                store_reverse_endianness_float_t((void*)(d + 68), ptcl->data.rotation_add_random.x);
                store_reverse_endianness_float_t((void*)(d + 72), ptcl->data.rotation_add_random.y);
                store_reverse_endianness_float_t((void*)(d + 76), ptcl->data.rotation_add_random.z);
                store_reverse_endianness_float_t((void*)(d + 80), ptcl->data.scale.x);
                store_reverse_endianness_float_t((void*)(d + 84), ptcl->data.scale.y);
                store_reverse_endianness_float_t((void*)(d + 88), ptcl->data.scale.z);
                store_reverse_endianness_float_t((void*)(d + 92), ptcl->data.scale_random.x);
                store_reverse_endianness_float_t((void*)(d + 96), ptcl->data.scale_random.y);
                store_reverse_endianness_float_t((void*)(d + 100), ptcl->data.scale_random.z);
                store_reverse_endianness_float_t((void*)(d + 104), ptcl->data.z_offset);
                store_reverse_endianness_int32_t((void*)(d + 108), ptcl->data.pivot);
                store_reverse_endianness_int32_t((void*)(d + 112), flags);
                store_reverse_endianness_int32_t((void*)(d + 116), ptcl->data.unk0);
                store_reverse_endianness_float_t((void*)(d + 120), ptcl->data.uv_scroll_2nd_add.x);
                store_reverse_endianness_float_t((void*)(d + 124), ptcl->data.uv_scroll_2nd_add.y);
                store_reverse_endianness_float_t((void*)(d + 128), ptcl->data.uv_scroll_2nd_add_scale);
                store_reverse_endianness_float_t((void*)(d + 136), ptcl->data.speed);
                store_reverse_endianness_float_t((void*)(d + 140), ptcl->data.speed_random);
                store_reverse_endianness_float_t((void*)(d + 144), ptcl->data.deceleration);
                store_reverse_endianness_float_t((void*)(d + 148), ptcl->data.deceleration_random);
                store_reverse_endianness_float_t((void*)(d + 152), direction.x);
                store_reverse_endianness_float_t((void*)(d + 156), direction.y);
                store_reverse_endianness_float_t((void*)(d + 160), direction.z);
                store_reverse_endianness_float_t((void*)(d + 164), direction_random.x);
                store_reverse_endianness_float_t((void*)(d + 168), direction_random.y);
                store_reverse_endianness_float_t((void*)(d + 172), direction_random.z);
                store_reverse_endianness_float_t((void*)(d + 176), ptcl->data.gravity.x);
                store_reverse_endianness_float_t((void*)(d + 180), ptcl->data.gravity.y);
                store_reverse_endianness_float_t((void*)(d + 184), ptcl->data.gravity.z);
                store_reverse_endianness_float_t((void*)(d + 188), ptcl->data.acceleration.x);
                store_reverse_endianness_float_t((void*)(d + 192), ptcl->data.acceleration.y);
                store_reverse_endianness_float_t((void*)(d + 196), ptcl->data.acceleration.z);
                store_reverse_endianness_float_t((void*)(d + 200), ptcl->data.acceleration_random.x);
                store_reverse_endianness_float_t((void*)(d + 204), ptcl->data.acceleration_random.y);
                store_reverse_endianness_float_t((void*)(d + 208), ptcl->data.acceleration_random.z);
                store_reverse_endianness_float_t((void*)(d + 212), ptcl->data.reflection_coeff);
                store_reverse_endianness_float_t((void*)(d + 216), ptcl->data.reflection_coeff_random);
                store_reverse_endianness_float_t((void*)(d + 220), ptcl->data.rebound_plane_y);
                store_reverse_endianness_float_t((void*)(d + 224), ptcl->data.uv_scroll_add.x);
                store_reverse_endianness_float_t((void*)(d + 228), ptcl->data.uv_scroll_add.y);
                store_reverse_endianness_float_t((void*)(d + 232), ptcl->data.uv_scroll_add_scale);
                store_reverse_endianness_int32_t((void*)(d + 236), ptcl->data.sub_flags);
                store_reverse_endianness_int32_t((void*)(d + 240), ptcl->data.count);
                store_reverse_endianness_int32_t((void*)(d + 244), ptcl->data.draw_flags);
                store_reverse_endianness_float_t((void*)(d + 248), ptcl->data.unk1);
                store_reverse_endianness_float_t((void*)(d + 252), ptcl->data.emission);
            }
            else {
                *(int32_t*)d = ptcl->data.life_time;
                *(int32_t*)(d + 4) = ptcl->data.life_time_random;
                *(int32_t*)(d + 8) = ptcl->data.fade_in;
                *(int32_t*)(d + 12) = ptcl->data.fade_in_random;
                *(int32_t*)(d + 16) = ptcl->data.fade_out;
                *(int32_t*)(d + 20) = ptcl->data.fade_out_random;
                *(int32_t*)(d + 24) = type;
                *(int32_t*)(d + 28) = ptcl->data.draw_type;
                *(vec3*)(d + 32) = ptcl->data.rotation;
                *(vec3*)(d + 44) = ptcl->data.rotation_random;
                *(vec3*)(d + 56) = ptcl->data.rotation_add;
                *(vec3*)(d + 68) = ptcl->data.rotation_add_random;
                *(vec3*)(d + 80) = ptcl->data.scale;
                *(vec3*)(d + 92) = ptcl->data.scale_random;
                *(float_t*)(d + 104) = ptcl->data.z_offset;
                *(int32_t*)(d + 108) = ptcl->data.pivot;
                *(int32_t*)(d + 112) = flags;
                *(int32_t*)(d + 116) = ptcl->data.unk0;
                *(vec2*)(d + 120) = ptcl->data.uv_scroll_2nd_add;
                *(float_t*)(d + 128) = ptcl->data.uv_scroll_2nd_add_scale;
                *(float_t*)(d + 136) = ptcl->data.speed;
                *(float_t*)(d + 140) = ptcl->data.speed_random;
                *(float_t*)(d + 144) = ptcl->data.deceleration;
                *(float_t*)(d + 148) = ptcl->data.deceleration_random;
                *(vec3*)(d + 152) = direction;
                *(vec3*)(d + 164) = direction_random;
                *(vec3*)(d + 176) = ptcl->data.gravity;
                *(vec3*)(d + 188) = ptcl->data.acceleration;
                *(vec3*)(d + 200) = ptcl->data.acceleration_random;
                *(float_t*)(d + 212) = ptcl->data.reflection_coeff;
                *(float_t*)(d + 216) = ptcl->data.reflection_coeff_random;
                *(float_t*)(d + 220) = ptcl->data.rebound_plane_y;
                *(vec2*)(d + 224) = ptcl->data.uv_scroll_add;
                *(float_t*)(d + 232) = ptcl->data.uv_scroll_add_scale;
                *(int32_t*)(d + 236) = ptcl->data.sub_flags;
                *(int32_t*)(d + 240) = ptcl->data.count;
                *(int32_t*)(d + 244) = ptcl->data.draw_flags;
                *(float_t*)(d + 248) = ptcl->data.unk1;
                *(float_t*)(d + 252) = ptcl->data.emission;
            }
            d += 256;

            if (ptcl->data.type == PARTICLE_LOCUS) {
                if (big_endian) {
                    store_reverse_endianness_uint16_t((void*)d, (uint16_t)ptcl->data.locus_history_size);
                    store_reverse_endianness_uint16_t((void*)(d + 2), (uint16_t)ptcl->data.locus_history_size_random);
                }
                else {
                    *(uint16_t*)d = (uint16_t)ptcl->data.locus_history_size;
                    *(uint16_t*)(d + 2) = (uint16_t)ptcl->data.locus_history_size_random;
                }
                d += 4;
            }
            else if (ptcl->data.type == PARTICLE_MESH) {
                if (big_endian) {
                    store_reverse_endianness_uint64_t((void*)d, ptcl->data.mesh.object_name_hash);
                    store_reverse_endianness_uint64_t((void*)(d + 8), ptcl->data.mesh.object_set_name_hash);
                }
                else {
                    *(uint64_t*)d = ptcl->data.mesh.object_name_hash;
                    *(uint64_t*)(d + 8) = ptcl->data.mesh.object_set_name_hash;
                }
                //strncpy_s((char*)(d + 16), 0x40, ptcl->data.mesh.mesh_name, 0x3F);
                //((char*)(d + 16))[0x3F] = 0;
                //if (big_endian)
                //    store_reverse_endianness_uint64_t((void*)(d + 80), ptcl->data.mesh.sub_mesh_hash);
                //else
                //    *(uint64_t*)(d + 80) = ptcl->data.mesh.sub_mesh_hash;
                if (big_endian)
                    store_reverse_endianness_uint64_t((void*)(d + 80), (uint64_t)hash_murmurhash_empty);
                else
                    *(uint64_t*)(d + 80) = hash_murmurhash_empty;
                d += 88;
            }

            if (big_endian) {
                store_reverse_endianness_uint64_t((void*)d, ptcl->data.tex_hash);
                *(uint8_t*)(d + 8) = (uint8_t)prj::roundf(clamp_def(ptcl->data.color.x, 0.0f, 1.0f) * 255.0f);
                *(uint8_t*)(d + 9) = (uint8_t)prj::roundf(clamp_def(ptcl->data.color.y, 0.0f, 1.0f) * 255.0f);
                *(uint8_t*)(d + 10) = (uint8_t)prj::roundf(clamp_def(ptcl->data.color.z, 0.0f, 1.0f) * 255.0f);
                *(uint8_t*)(d + 11) = (uint8_t)prj::roundf(clamp_def(ptcl->data.color.w, 0.0f, 1.0f) * 255.0f);
                store_reverse_endianness_int32_t((void*)(d + 12), ptcl->data.blend_mode);
                store_reverse_endianness_int32_t((void*)(d + 16), ptcl->data.unk2);
                store_reverse_endianness_int32_t((void*)(d + 20), ptcl->data.split_u);
                store_reverse_endianness_int32_t((void*)(d + 24), ptcl->data.split_v);
                store_reverse_endianness_int32_t((void*)(d + 28), ptcl->data.uv_index_type);
                store_reverse_endianness_int16_t((void*)(d + 32), (int16_t)ptcl->data.uv_index);
                store_reverse_endianness_int16_t((void*)(d + 34), (int16_t)ptcl->data.frame_step_uv);
                store_reverse_endianness_int32_t((void*)(d + 36), ptcl->data.uv_index_start);
                store_reverse_endianness_int32_t((void*)(d + 40), ptcl->data.uv_index_end);
                store_reverse_endianness_int32_t((void*)(d + 44), ptcl->data.unk3);
            }
            else {
                *(uint64_t*)d = ptcl->data.tex_hash;
                *(uint8_t*)(d + 8) = (uint8_t)prj::roundf(clamp_def(ptcl->data.color.x, 0.0f, 1.0f) * 255.0f);
                *(uint8_t*)(d + 9) = (uint8_t)prj::roundf(clamp_def(ptcl->data.color.y, 0.0f, 1.0f) * 255.0f);
                *(uint8_t*)(d + 10) = (uint8_t)prj::roundf(clamp_def(ptcl->data.color.z, 0.0f, 1.0f) * 255.0f);
                *(uint8_t*)(d + 11) = (uint8_t)prj::roundf(clamp_def(ptcl->data.color.w, 0.0f, 1.0f) * 255.0f);
                *(int32_t*)(d + 12) = ptcl->data.blend_mode;
                *(int32_t*)(d + 16) = ptcl->data.unk2;
                *(int32_t*)(d + 20) = ptcl->data.split_u;
                *(int32_t*)(d + 24) = ptcl->data.split_v;
                *(int32_t*)(d + 28) = ptcl->data.uv_index_type;
                *(int16_t*)(d + 32) = (int16_t)ptcl->data.uv_index;
                *(int16_t*)(d + 34) = (int16_t)ptcl->data.frame_step_uv;
                *(int32_t*)(d + 36) = ptcl->data.uv_index_start;
                *(int32_t*)(d + 40) = ptcl->data.uv_index_end;
                *(int32_t*)(d + 44) = ptcl->data.unk3;
            }
            d += 48;

            if (ptcl->data.flags & PARTICLE_TEXTURE_MASK) {
                if (big_endian) {
                    store_reverse_endianness_uint64_t((void*)d, ptcl->data.mask_tex_hash);
                    store_reverse_endianness_int32_t((void*)(d + 8), ptcl->data.mask_blend_mode);
                    store_reverse_endianness_int32_t((void*)(d + 12), ptcl->data.unk4);
                }
                else {
                    *(uint64_t*)d = ptcl->data.mask_tex_hash;
                    *(int32_t*)(d + 8) = ptcl->data.mask_blend_mode;
                    *(int32_t*)(d + 12) = ptcl->data.unk4;
                }
                d += 16;
            }
        }
        else {
            if (big_endian) {
                store_reverse_endianness_int32_t((void*)d, ptcl->data.life_time);
                store_reverse_endianness_int32_t((void*)(d + 4), ptcl->data.type);
                store_reverse_endianness_int32_t((void*)(d + 8), ptcl->data.draw_type);
                store_reverse_endianness_float_t((void*)(d + 12), ptcl->data.rotation.x);
                store_reverse_endianness_float_t((void*)(d + 16), ptcl->data.rotation.y);
                store_reverse_endianness_float_t((void*)(d + 20), ptcl->data.rotation.z);
                store_reverse_endianness_float_t((void*)(d + 24), ptcl->data.rotation_random.x);
                store_reverse_endianness_float_t((void*)(d + 28), ptcl->data.rotation_random.y);
                store_reverse_endianness_float_t((void*)(d + 32), ptcl->data.rotation_random.z);
                store_reverse_endianness_float_t((void*)(d + 36), ptcl->data.rotation_add.x);
                store_reverse_endianness_float_t((void*)(d + 40), ptcl->data.rotation_add.y);
                store_reverse_endianness_float_t((void*)(d + 44), ptcl->data.rotation_add.z);
                store_reverse_endianness_float_t((void*)(d + 48), ptcl->data.rotation_add_random.x);
                store_reverse_endianness_float_t((void*)(d + 52), ptcl->data.rotation_add_random.y);
                store_reverse_endianness_float_t((void*)(d + 56), ptcl->data.rotation_add_random.z);
                store_reverse_endianness_float_t((void*)(d + 60), ptcl->data.scale.x);
                store_reverse_endianness_float_t((void*)(d + 64), ptcl->data.scale.y);
                store_reverse_endianness_float_t((void*)(d + 68), ptcl->data.scale.z);
                store_reverse_endianness_float_t((void*)(d + 72), ptcl->data.scale_random.x);
                store_reverse_endianness_float_t((void*)(d + 76), ptcl->data.scale_random.y);
                store_reverse_endianness_float_t((void*)(d + 80), ptcl->data.scale_random.z);
                store_reverse_endianness_float_t((void*)(d + 84), ptcl->data.z_offset);
                store_reverse_endianness_int32_t((void*)(d + 88), ptcl->data.pivot);
                store_reverse_endianness_int32_t((void*)(d + 92), flags);
                store_reverse_endianness_float_t((void*)(d + 96), ptcl->data.speed);
                store_reverse_endianness_float_t((void*)(d + 100), ptcl->data.speed_random);
                store_reverse_endianness_float_t((void*)(d + 104), ptcl->data.deceleration);
                store_reverse_endianness_float_t((void*)(d + 108), ptcl->data.deceleration_random);
                store_reverse_endianness_float_t((void*)(d + 112), ptcl->data.direction.x);
                store_reverse_endianness_float_t((void*)(d + 116), ptcl->data.direction.y);
                store_reverse_endianness_float_t((void*)(d + 120), ptcl->data.direction.z);
                store_reverse_endianness_float_t((void*)(d + 124), ptcl->data.direction_random.x);
                store_reverse_endianness_float_t((void*)(d + 128), ptcl->data.direction_random.y);
                store_reverse_endianness_float_t((void*)(d + 132), ptcl->data.direction_random.z);
                store_reverse_endianness_float_t((void*)(d + 136), ptcl->data.gravity.x);
                store_reverse_endianness_float_t((void*)(d + 140), ptcl->data.gravity.y);
                store_reverse_endianness_float_t((void*)(d + 144), ptcl->data.gravity.z);
                store_reverse_endianness_float_t((void*)(d + 148), ptcl->data.acceleration.x);
                store_reverse_endianness_float_t((void*)(d + 152), ptcl->data.acceleration.y);
                store_reverse_endianness_float_t((void*)(d + 156), ptcl->data.acceleration.z);
                store_reverse_endianness_float_t((void*)(d + 160), ptcl->data.acceleration_random.x);
                store_reverse_endianness_float_t((void*)(d + 164), ptcl->data.acceleration_random.y);
                store_reverse_endianness_float_t((void*)(d + 168), ptcl->data.acceleration_random.z);
                store_reverse_endianness_float_t((void*)(d + 172), ptcl->data.reflection_coeff);
                store_reverse_endianness_float_t((void*)(d + 176), ptcl->data.reflection_coeff_random);
                store_reverse_endianness_float_t((void*)(d + 180), ptcl->data.rebound_plane_y);
                store_reverse_endianness_float_t((void*)(d + 184), ptcl->data.uv_scroll_add.x);
                store_reverse_endianness_float_t((void*)(d + 188), ptcl->data.uv_scroll_add.y);
                store_reverse_endianness_float_t((void*)(d + 192), ptcl->data.uv_scroll_add_scale);
                store_reverse_endianness_int32_t((void*)(d + 196), ptcl->data.sub_flags);
                store_reverse_endianness_int32_t((void*)(d + 200), ptcl->data.count);
            }
            else {
                *(int32_t*)d = ptcl->data.life_time;
                *(int32_t*)(d + 4) = ptcl->data.type;
                *(int32_t*)(d + 8) = ptcl->data.draw_type;
                *(vec3*)(d + 12) = ptcl->data.rotation;
                *(vec3*)(d + 24) = ptcl->data.rotation_random;
                *(vec3*)(d + 36) = ptcl->data.rotation_add;
                *(vec3*)(d + 48) = ptcl->data.rotation_add_random;
                *(vec3*)(d + 60) = ptcl->data.scale;
                *(vec3*)(d + 72) = ptcl->data.scale_random;
                *(float_t*)(d + 84) = ptcl->data.z_offset;
                *(int32_t*)(d + 88) = ptcl->data.pivot;
                *(int32_t*)(d + 92) = flags;
                *(float_t*)(d + 96) = ptcl->data.speed;
                *(float_t*)(d + 100) = ptcl->data.speed_random;
                *(float_t*)(d + 104) = ptcl->data.deceleration;
                *(float_t*)(d + 108) = ptcl->data.deceleration_random;
                *(vec3*)(d + 112) = ptcl->data.direction;
                *(vec3*)(d + 124) = ptcl->data.direction_random;
                *(vec3*)(d + 136) = ptcl->data.gravity;
                *(vec3*)(d + 148) = ptcl->data.acceleration;
                *(vec3*)(d + 160) = ptcl->data.acceleration_random;
                *(float_t*)(d + 172) = ptcl->data.reflection_coeff;
                *(float_t*)(d + 176) = ptcl->data.reflection_coeff_random;
                *(float_t*)(d + 180) = ptcl->data.rebound_plane_y;
                *(vec2*)(d + 184) = ptcl->data.uv_scroll_add;
                *(float_t*)(d + 192) = ptcl->data.uv_scroll_add_scale;
                *(int32_t*)(d + 196) = ptcl->data.sub_flags;
                *(int32_t*)(d + 200) = ptcl->data.count;
            }
            d += 204;

            if (ptcl->version == 3) {
                *(int32_t*)d = 0;
                if (big_endian) {
                    store_reverse_endianness_float_t((void*)d, ptcl->data.unk1);
                    store_reverse_endianness_float_t((void*)(d + 4), ptcl->data.emission);
                }
                else {
                    *(float_t*)d = ptcl->data.unk1;
                    *(float_t*)(d + 4) = ptcl->data.emission;
                }
                d += 8;
            }

            if (ptcl->data.type == PARTICLE_LOCUS || ptcl->data.type == PARTICLE_MESH) {
                if (big_endian) {
                    store_reverse_endianness_uint16_t((void*)d, (uint16_t)ptcl->data.locus_history_size);
                    store_reverse_endianness_uint16_t((void*)(d + 2), (uint16_t)ptcl->data.locus_history_size_random);
                }
                else {
                    *(uint16_t*)d = (uint16_t)ptcl->data.locus_history_size;
                    *(uint16_t*)(d + 2) = (uint16_t)ptcl->data.locus_history_size_random;
                }
                d += 4;
            }

            if (big_endian) {
                store_reverse_endianness_uint64_t((void*)d, ptcl->data.tex_hash);
                *(uint8_t*)(d + 8) = (uint8_t)prj::roundf(clamp_def(ptcl->data.color.x, 0.0f, 1.0f) * 255.0f);
                *(uint8_t*)(d + 9) = (uint8_t)prj::roundf(clamp_def(ptcl->data.color.y, 0.0f, 1.0f) * 255.0f);
                *(uint8_t*)(d + 10) = (uint8_t)prj::roundf(clamp_def(ptcl->data.color.z, 0.0f, 1.0f) * 255.0f);
                *(uint8_t*)(d + 11) = (uint8_t)prj::roundf(clamp_def(ptcl->data.color.w, 0.0f, 1.0f) * 255.0f);
                store_reverse_endianness_int32_t((void*)(d + 12), ptcl->data.blend_mode);
                store_reverse_endianness_int32_t((void*)(d + 16), ptcl->data.unk2);
                store_reverse_endianness_int32_t((void*)(d + 20), ptcl->data.split_u);
                store_reverse_endianness_int32_t((void*)(d + 24), ptcl->data.split_v);
                store_reverse_endianness_int32_t((void*)(d + 28), ptcl->data.uv_index_type);
                store_reverse_endianness_int16_t((void*)(d + 32), (int16_t)ptcl->data.uv_index);
                store_reverse_endianness_int16_t((void*)(d + 34), (int16_t)ptcl->data.frame_step_uv);
                store_reverse_endianness_int32_t((void*)(d + 36), ptcl->data.uv_index_start);
                store_reverse_endianness_int32_t((void*)(d + 40), ptcl->data.uv_index_end);
            }
            else {
                *(uint64_t*)d = ptcl->data.tex_hash;
                *(uint8_t*)(d + 8) = (uint8_t)prj::roundf(clamp_def(ptcl->data.color.x, 0.0f, 1.0f) * 255.0f);
                *(uint8_t*)(d + 9) = (uint8_t)prj::roundf(clamp_def(ptcl->data.color.y, 0.0f, 1.0f) * 255.0f);
                *(uint8_t*)(d + 10) = (uint8_t)prj::roundf(clamp_def(ptcl->data.color.z, 0.0f, 1.0f) * 255.0f);
                *(uint8_t*)(d + 11) = (uint8_t)prj::roundf(clamp_def(ptcl->data.color.w, 0.0f, 1.0f) * 255.0f);
                *(int32_t*)(d + 12) = ptcl->data.blend_mode;
                *(int32_t*)(d + 16) = ptcl->data.unk2;
                *(int32_t*)(d + 20) = ptcl->data.split_u;
                *(int32_t*)(d + 24) = ptcl->data.split_v;
                *(int32_t*)(d + 28) = ptcl->data.uv_index_type;
                *(int16_t*)(d + 32) = (int16_t)ptcl->data.uv_index;
                *(int16_t*)(d + 34) = (int16_t)ptcl->data.frame_step_uv;
                *(int32_t*)(d + 36) = ptcl->data.uv_index_start;
                *(int32_t*)(d + 40) = ptcl->data.uv_index_end;
            }
            d += 44;

            if (eff_group->version >= 7) {
                if (big_endian)
                    store_reverse_endianness_int32_t((void*)d, ptcl->data.unk3);
                else
                    *(int32_t*)d = ptcl->data.unk3;
                d += 4;
            }

            if (ptcl->data.flags & PARTICLE_TEXTURE_MASK) {
                if (big_endian) {
                    store_reverse_endianness_uint64_t((void*)d, ptcl->data.mask_tex_hash);
                    store_reverse_endianness_int32_t((void*)(d + 8), ptcl->data.mask_blend_mode);
                    store_reverse_endianness_int32_t((void*)(d + 12), ptcl->data.unk4);
                }
                else {
                    *(uint64_t*)d = ptcl->data.mask_tex_hash;
                    *(int32_t*)(d + 8) = ptcl->data.mask_blend_mode;
                    *(int32_t*)(d + 12) = ptcl->data.unk4;
                }
                d += 16;
            }
        }

        st->enrs = e;

        st->header.signature = reverse_endianness_uint32_t('PTCL');
        st->header.length = 0x20;
        st->header.use_big_endian = big_endian;
        st->header.use_section_size = true;
        st->header.version = ptcl->version;
        return true;
    }

    bool FileWriter::UnparseAnimation(f2_struct* st, Animation* anim, CurveTypeFlags flags, bool big_endian) {
        if (anim->curves.size() < 1)
            return false;

        static const CurveType order[] = {
            CURVE_COLOR_A,
            CURVE_COLOR_R,
            CURVE_COLOR_G,
            CURVE_COLOR_B,
            CURVE_COLOR_RGB_SCALE,
            CURVE_COLOR_A_2ND,
            CURVE_COLOR_R_2ND,
            CURVE_COLOR_G_2ND,
            CURVE_COLOR_B_2ND,
            CURVE_COLOR_RGB_SCALE_2ND,
            CURVE_TRANSLATION_X,
            CURVE_TRANSLATION_Y,
            CURVE_TRANSLATION_Z,
            CURVE_ROTATION_X,
            CURVE_ROTATION_Y,
            CURVE_ROTATION_Z,
            CURVE_SCALE_X,
            CURVE_SCALE_Y,
            CURVE_SCALE_Z,
            CURVE_SCALE_ALL,
            CURVE_EMISSION_INTERVAL,
            CURVE_PARTICLES_PER_EMISSION,
            CURVE_U_SCROLL,
            CURVE_V_SCROLL,
            CURVE_U_SCROLL_ALPHA,
            CURVE_V_SCROLL_ALPHA,
            CURVE_U_SCROLL_2ND,
            CURVE_V_SCROLL_2ND,
            CURVE_U_SCROLL_ALPHA_2ND,
            CURVE_V_SCROLL_ALPHA_2ND,
        };

        for (int32_t i = CURVE_TRANSLATION_X; i <= CURVE_V_SCROLL_ALPHA_2ND; i++) {
            if (!(flags & (1 << (size_t)order[i])))
                continue;

            for (Curve*& j : anim->curves) {
                Curve* c = j;
                if (!c || c->type != order[i])
                    continue;

                f2_struct s;
                if (UnparseCurve(&s, c, big_endian)) {
                    st->sub_structs.push_back(s);
                    break;
                }
            }
        }

        if (st->sub_structs.size() < 1)
            return false;

        st->header.signature = reverse_endianness_uint32_t('ANIM');
        st->header.length = 0x20;
        st->header.use_big_endian = big_endian;
        st->header.use_section_size = true;
        return true;
    }

    bool FileWriter::UnparseCurve(f2_struct* st, Curve* c, bool big_endian) {
        std::vector<Curve::Key> keys;

        extern bool glitter_editor_enable;
        if (glitter_editor_enable) {
            if (c->keys_rev.size() < 1)
                return false;

            keys.assign(c->keys.begin(), c->keys.end());
            c->Recalculate(type);
        }

        if (c->keys.size() < 1)
            return false;

        size_t l;
        size_t d;
        l = 0;

        uint32_t o;
        enrs e;
        enrs_entry ee;

        if (type == Glitter::X) {
            if (st->header.version == 1) {
                ee = { 0, 2, 24, 1 };
                ee.append(0, 4, ENRS_DWORD);
                ee.append(0, 4, ENRS_WORD);
                e.vec.push_back(ee);
                l += o = 24;

                l = align_val(l, 0x10);
                st->data.resize(l);
                d = (size_t)st->data.data();

                if (st->header.use_big_endian) {
                    store_reverse_endianness_uint32_t((void*)d, c->type);
                    store_reverse_endianness_uint32_t((void*)(d + 4), c->repeat ? 1 : 0);
                    store_reverse_endianness_uint32_t((void*)(d + 8), c->flags);
                    store_reverse_endianness_float_t((void*)(d + 12), c->random_range);
                    store_reverse_endianness_uint16_t((void*)(d + 16), (uint16_t)c->keys.size());
                    store_reverse_endianness_uint16_t((void*)(d + 18), (uint16_t)c->start_time);
                    store_reverse_endianness_uint16_t((void*)(d + 20), (uint16_t)c->end_time);
                }
                else {
                    *(uint32_t*)d = c->type;
                    *(uint32_t*)(d + 4) = c->repeat ? 1 : 0;
                    *(uint32_t*)(d + 8) = c->flags;
                    *(float_t*)(d + 12) = c->random_range;
                    *(uint16_t*)(d + 16) = (uint16_t)c->keys.size();;;
                    *(uint16_t*)(d + 18) = c->start_time;
                    *(uint16_t*)(d + 20) = c->end_time;
                }
            }
            else {
                ee = { 0, 2, 36, 1 };
                ee.append(0, 7, ENRS_DWORD);
                ee.append(0, 4, ENRS_WORD);
                e.vec.push_back(ee);
                l += o = 36;

                l = align_val(l, 0x10);
                st->data.resize(l);
                d = (size_t)st->data.data();

                if (st->header.use_big_endian) {
                    store_reverse_endianness_uint32_t((void*)d, c->type);
                    store_reverse_endianness_uint32_t((void*)(d + 4), c->repeat ? 1 : 0);
                    store_reverse_endianness_uint32_t((void*)(d + 8), c->flags);
                    store_reverse_endianness_float_t((void*)(d + 12), c->random_range);
                    store_reverse_endianness_uint16_t((void*)(d + 28), (uint16_t)c->keys.size());
                    store_reverse_endianness_uint16_t((void*)(d + 30), (uint16_t)c->start_time);
                    store_reverse_endianness_uint16_t((void*)(d + 32), (uint16_t)c->end_time);
                }
                else {
                    *(uint32_t*)d = c->type;
                    *(uint32_t*)(d + 4) = c->repeat ? 1 : 0;
                    *(uint32_t*)(d + 8) = c->flags;
                    *(float_t*)(d + 12) = c->random_range;
                    *(uint16_t*)(d + 28) = (uint16_t)c->keys.size();;
                    *(uint16_t*)(d + 30) = c->start_time;
                    *(uint16_t*)(d + 32) = c->end_time;
                }
            }
        }
        else {
            ee = { 0, 2, 24, 1 };
            ee.append(0, 4, ENRS_DWORD);
            ee.append(0, 4, ENRS_WORD);
            e.vec.push_back(ee);
            l += o = 24;

            l = align_val(l, 0x10);
            st->data.resize(l);
            d = (size_t)st->data.data();

            float_t random_range = c->random_range;
            if (c->version == 0)
                switch (c->type) {
                case CURVE_ROTATION_X:
                case CURVE_ROTATION_Y:
                case CURVE_ROTATION_Z:
                    random_range = 0.0f;
                    break;
                }

            if (big_endian) {
                store_reverse_endianness_uint32_t((void*)d, c->type);
                store_reverse_endianness_uint32_t((void*)(d + 4), c->repeat ? 1 : 0);
                store_reverse_endianness_uint32_t((void*)(d + 8), c->flags);
                store_reverse_endianness_float_t((void*)(d + 12), random_range);
                store_reverse_endianness_uint16_t((void*)(d + 16), (uint16_t)c->keys.size());
                store_reverse_endianness_uint16_t((void*)(d + 18), (uint16_t)c->start_time);
                store_reverse_endianness_uint16_t((void*)(d + 20), (uint16_t)c->end_time);
            }
            else {
                *(uint32_t*)d = c->type;
                *(uint32_t*)(d + 4) = c->repeat ? 1 : 0;
                *(uint32_t*)(d + 8) = c->flags;
                *(float_t*)(d + 12) = random_range;
                *(uint16_t*)(d + 16) = (uint16_t)c->keys.size();
                *(uint16_t*)(d + 18) = (uint16_t)c->start_time;
                *(uint16_t*)(d + 20) = (uint16_t)c->end_time;
            }

            if (c->version == 0)
                switch (c->type) {
                case CURVE_ROTATION_X:
                case CURVE_ROTATION_Y:
                case CURVE_ROTATION_Z:
                    c->random_range = 0.0f;
                    break;
                }
        }

        st->enrs = e;

        st->header.signature = reverse_endianness_uint32_t('CURV');
        st->header.length = 0x20;
        st->header.use_big_endian = big_endian;
        st->header.use_section_size = true;
        st->header.version = c->version;

        f2_struct s;
        PackCurve(&s, c, big_endian);
        st->sub_structs.push_back(s);

        if (glitter_editor_enable)
            c->keys.assign(keys.begin(), keys.end());
        return true;
    }

    bool FileWriter::UnparseDivaEffect(EffectGroup* eff_group, f2_struct* st, bool big_endian) {
        UnparseEffectGroup(eff_group, st, big_endian);

        st->header.signature = reverse_endianness_uint32_t('DVEF');
        st->header.length = 0x20;
        st->header.use_big_endian = false;
        st->header.use_section_size = true;
        st->header.version = eff_group->version;
        return true;
    }

    bool FileWriter::UnparseDivaList(EffectGroup* eff_group, f2_struct* st, bool big_endian) {
        f2_struct s;
        if (!PackDivaList(eff_group, &s, big_endian))
            return false;

        st->sub_structs.push_back(s);

        st->header.signature = reverse_endianness_uint32_t('LIST');
        st->header.length = 0x20;
        st->header.use_big_endian = big_endian;
        st->header.use_section_size = true;
        return true;
    }

    bool FileWriter::UnparseDivaResource(EffectGroup* eff_group, f2_struct* st) {
        f2_struct s;
        if (!PackDivaResource(eff_group, &s))
            return false;

        st->sub_structs.push_back(s);

        st->header.signature = reverse_endianness_uint32_t('DVRS');
        st->header.length = 0x20;
        st->header.use_big_endian = false;
        st->header.use_section_size = true;
        st->header.version = 1;
        return true;
    }

    bool FileWriter::UnparseEffect(EffectGroup* eff_group, f2_struct* st, Effect* eff, bool big_endian) {
        if (!PackEffect(st, eff, big_endian))
            return false;

        f2_struct s;
        if (UnparseAnimation(&s, &eff->animation, effect_curve_flags, big_endian))
            st->sub_structs.push_back(s);

        for (Emitter*& i : eff->emitters) {
            if (!i)
                continue;

            f2_struct s;
            if (UnparseEmitter(eff_group, &s, i, eff, big_endian))
                st->sub_structs.push_back(s);
        }
        return true;
    }

    void FileWriter::UnparseEffectGroup(EffectGroup* eff_group, f2_struct* st, bool big_endian) {
        for (Effect*& i : eff_group->effects) {
            if (!i)
                continue;

            f2_struct s;
            if (UnparseEffect(eff_group, &s, i, big_endian))
                st->sub_structs.push_back(s);
        }

        f2_struct s;
        if (PackDivaResourceHashes(eff_group, &s, big_endian))
            st->sub_structs.push_back(s);
    }

    bool FileWriter::UnparseEmitter(EffectGroup* eff_group,
        f2_struct* st, Emitter* emit, Effect* eff, bool big_endian) {
        if (!PackEmitter(st, emit, big_endian))
            return false;

        f2_struct s;
        if (UnparseAnimation(&s, &emit->animation, emitter_curve_flags, big_endian))
            st->sub_structs.push_back(s);

        for (Particle*& i : emit->particles) {
            if (!i)
                continue;

            f2_struct s;
            if (UnparseParticle(eff_group, &s, i, eff, big_endian))
                st->sub_structs.push_back(s);
        }
        return true;
    }

    bool FileWriter::UnparseParticle(EffectGroup* eff_group,
        f2_struct* st, Particle* ptcl, Effect* eff, bool big_endian) {
        if (!PackParticle(eff_group, st, ptcl, eff, big_endian))
            return false;

        CurveTypeFlags flags;
        if (type == Glitter::X)
            flags = particle_x_curve_flags;
        else
            flags = particle_curve_flags;

        if (ptcl->data.type != PARTICLE_MESH) {
            enum_and(flags, ~CURVE_TYPE_UV_SCROLL_2ND);
            if (ptcl->data.draw_type != DIRECTION_PARTICLE_ROTATION)
                enum_and(flags, ~(CURVE_TYPE_ROTATION_X | CURVE_TYPE_ROTATION_Y));
        }

        f2_struct s;
        if (UnparseAnimation(&s, &ptcl->animation, flags, big_endian))
            st->sub_structs.push_back(s);
        return true;
    }

    void FileWriter::Write(GLT, EffectGroup* eff_group,
        const char* path, const char* file, FileWriterFlags writer_flags) {
        FileWriter fr;
        fr.type = GLT_VAL;

        bool compress = !!(writer_flags & FILE_WRITER_COMPRESS);
        bool encrypt = !!(writer_flags & FILE_WRITER_ENCRYPT);
        bool save_lst = !(writer_flags & FILE_WRITER_NO_LIST);
        bool big_endian = !!(writer_flags & FILE_WRITER_BIG_ENDIAN);
        farc f;
        {
            size_t file_len = utf8_length(file);
            char* temp = force_malloc<char>(file_len + 5);
            if (!temp)
                return;

            memcpy(temp, file, file_len);
            temp[file_len] = 0;


            {
                f2_struct dve_st;
                if (fr.UnparseDivaEffect(eff_group, &dve_st, big_endian)) {
                    memcpy(&temp[file_len], ".dve", 4);
                    temp[file_len + 4] = 0;
                    farc_file* ff_dve = f.add_file(temp);
                    dve_st.write(&ff_dve->data, &ff_dve->size);
                    ff_dve->compressed = compress;
                    ff_dve->encrypted = encrypt;
                }
                else {
                    free(temp);
                    return;
                }
            }

            {
                f2_struct drs_st;
                if (fr.UnparseDivaResource(eff_group, &drs_st)) {
                    memcpy(&temp[file_len], ".drs", 4);
                    temp[file_len + 4] = 0;
                    farc_file* ff_drs = f.add_file(temp);
                    drs_st.write(&ff_drs->data, &ff_drs->size);
                    ff_drs->compressed = compress;
                    ff_drs->encrypted = encrypt;
                }
            }

            if (fr.type == Glitter::FT || save_lst) {
                f2_struct lst_st;
                if (fr.UnparseDivaList(eff_group, &lst_st, big_endian)) {
                    memcpy(&temp[file_len], ".lst", 4);
                    temp[file_len + 4] = 0;
                    farc_file* ff_lst = f.add_file(temp);
                    lst_st.write(&ff_lst->data, &ff_lst->size);
                    ff_lst->compressed = compress;
                    ff_lst->encrypted = encrypt;
                }
                else {
                    free(temp);
                    return;
                }
            }
            free(temp);
        }

        farc_signature signature;
        farc_flags flags;
        if (encrypt) {
            signature = FARC_FARC;
            flags = FARC_AES;

            if (compress)
                enum_or(flags, FARC_GZIP);
        }
        else if (compress) {
            signature = FARC_FArC;
            flags = FARC_NONE;
        }
        else {
            signature = FARC_FArc;
            flags = FARC_NONE;
        }

        char* temp = str_utils_add(path, file);
        if (glt_type != Glitter::FT && save_lst) {
            char* list_temp = str_utils_add(temp, ".glitter.txt");
            file_stream s;
            s.open(list_temp, "wb");
            if (s.check_not_null()) {
                for (Glitter::Effect*& i : eff_group->effects)
                    if (i) {
                        s.write_string(i->name);
                        s.write_char('\n');
                    }
            }
            free_def(list_temp);
        }
        f.write(temp, signature, flags, true, false);
        free_def(temp);
    }
}
