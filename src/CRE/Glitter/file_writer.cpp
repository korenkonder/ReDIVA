/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"
#include "../../KKdLib/io/path.hpp"
#include "../../KKdLib/interpolation.hpp"
#include "../../KKdLib/farc.hpp"
#include "../../KKdLib/str_utils.hpp"
#include "../data.hpp"

namespace Glitter {
    FileWriter::FileWriter() : type() {

    }

    void FileWriter::PackCurve(f2_struct* st, Curve* c) {
        if (!c->keys.size()) {
            st->header.signature = reverse_endianness_uint32_t('KEYS');
            st->header.length = 0x20;
            st->header.use_big_endian = false;
            st->header.use_section_size = true;
            st->header.version = c->keys_version;
            return;
        }

        size_t l = 0;

        uint32_t o;
        enrs e;
        enrs_entry ee;

        size_t count;
        if (c->flags & CURVE_KEY_RANDOM_RANGE) {
            KeyType key_type = c->keys.front().type;
            if (key_type == KEY_HERMITE) {
                ee = { 0, 2, 20, 1 };
                ee.append(0, 2, ENRS_WORD);
                ee.append(0, 4, ENRS_DWORD);
                l = 20;
            }
            else {
                ee = { 0, 2, 12, 1 };
                ee.append(0, 2, ENRS_WORD);
                ee.append(0, 2, ENRS_DWORD);
                l = 12;
            }

            count = 1;
            Curve::Key* i_begin = c->keys.data() + 1;
            Curve::Key* i_end = c->keys.data() + c->keys.size();
            for (Curve::Key* i = i_begin; i != i_end;
                i++, count++, l += key_type == KEY_HERMITE ? 20 : 12) {
                if (i->type == key_type)
                    continue;

                if (count > 0) {
                    ee.repeat_count = (uint32_t)count;
                    e.vec.push_back(ee);
                }

                count = 1;
                o = (uint32_t)((key_type == KEY_HERMITE ? 20 : 12) * count);
                if (i->type == KEY_HERMITE) {
                    ee = { o, 2, 20, 1 };
                    ee.append(0, 2, ENRS_WORD);
                    ee.append(0, 4, ENRS_DWORD);
                }
                else {
                    ee = { o, 2, 12, 1 };
                    ee.append(0, 2, ENRS_WORD);
                    ee.append(0, 2, ENRS_DWORD);
                }
                key_type = i->type;
            }
        }
        else {
            KeyType key_type = c->keys.front().type;
            if (key_type == KEY_HERMITE) {
                ee = { 0, 2, 16, 1 };
                ee.append(0, 2, ENRS_WORD);
                ee.append(0, 3, ENRS_DWORD);
                l = 16;
            }
            else {
                ee = { 0, 2, 8, 1 };
                ee.append(0, 2, ENRS_WORD);
                ee.append(0, 1, ENRS_DWORD);
                l = 8;
            }

            count = 1;
            Curve::Key* i_begin = c->keys.data() + 1;
            Curve::Key* i_end = c->keys.data() + c->keys.size();
            for (Curve::Key* i = i_begin; i != i_end;
                i++, count++, l += key_type == KEY_HERMITE ? 16 : 8) {
                if (i->type == key_type)
                    continue;

                if (count > 0) {
                    ee.repeat_count = (uint32_t)count;
                    e.vec.push_back(ee);
                }

                count = 1;
                o = (uint32_t)((key_type == KEY_HERMITE ? 16 : 8) * count);
                if (i->type == KEY_HERMITE) {
                    ee = { o, 2, 16, 1 };
                    ee.append(0, 2, ENRS_WORD);
                    ee.append(0, 3, ENRS_DWORD);
                }
                else {
                    ee = { o, 2, 8, 1 };
                    ee.append(0, 2, ENRS_WORD);
                    ee.append(0, 1, ENRS_DWORD);
                }
                key_type = i->type;
            }
        }

        if (count > 0) {
            ee.repeat_count = (uint32_t)count;
            e.vec.push_back(ee);
        }

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

        l = align_val(l, 0x10);
        st->data.resize(l);
        size_t d = (size_t)st->data.data();

        if (scale == 1.0f)
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

        st->enrs = e;

        st->header.signature = reverse_endianness_uint32_t('KEYS');
        st->header.length = 0x20;
        st->header.use_big_endian = false;
        st->header.use_section_size = true;
        st->header.version = c->keys_version;
    }

    bool FileWriter::PackDivaList(EffectGroup* eff_group, f2_struct* st) {
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

        *(uint32_t*)d = (uint32_t)length;
        d += 4;

        for (Effect*& i : eff_group->effects)
            if (i) {
                size_t size = min(i->name.size(), 0x7F);
                memcpy((void*)d, i->name.c_str(), size);
                memset((void*)(d + size), 0, 0x80 - size);
                d += 0x80;
            }

        st->enrs = e;

        st->header.signature = reverse_endianness_uint32_t('GEFF');
        st->header.length = 0x20;
        st->header.use_big_endian = false;
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

    bool FileWriter::PackDivaResourceHashes(EffectGroup* eff_group, f2_struct* st) {
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

        *(int32_t*)d = (int32_t)count;
        *(int32_t*)(d + 4) = 0;
        d += 8;

        memcpy((void*)d, eff_group->resource_hashes.data(), sizeof(uint64_t) * count);

        st->enrs = e;

        st->header.signature = reverse_endianness_uint32_t('DVRS');
        st->header.length = 0x20;
        st->header.use_big_endian = false;
        st->header.use_section_size = true;
        return true;
    }

    bool FileWriter::PackEffect(f2_struct* st, Effect* eff) {
        if (eff->version != 6 && eff->version != 7)
            return false;

        Effect::ExtAnim* ext_anim = eff->data.ext_anim;

        size_t l = 0;

        uint32_t o;
        enrs e;
        enrs_entry ee;

        ee = { 0, 2, 56, 1 };
        ee.append(0, 1, ENRS_QWORD);
        ee.append(0, 12, ENRS_DWORD);
        e.vec.push_back(ee);
        l += o = 56;

        if (eff->version == 7) {
            ee = { 0, 1, 4, 1 };
            ee.append(0, 1, ENRS_DWORD);
            e.vec.push_back(ee);
            l += o = 4;
        }

        ee = { 0, 1, 4, 1 };
        ee.append(0, 1, ENRS_DWORD);
        e.vec.push_back(ee);
        l += o = 4;

        if (~eff->data.flags & EFFECT_LOCAL && ext_anim)
            if (ext_anim->flags & EFFECT_EXT_ANIM_CHARA_ANIM) {
                ee = { 0, 1, 12, 1 };
                ee.append(0, 3, ENRS_DWORD);
                e.vec.push_back(ee);
                l += o = 12;
            }
            else {
                ee = { 0, 2, 140, 1 };
                ee.append(0, 1, ENRS_QWORD);
                ee.append(0, 1, ENRS_DWORD);
                e.vec.push_back(ee);
                l += o = 140;
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

        *(uint64_t*)d = type != Glitter::FT
            ? hash_string_murmurhash(eff->name) : hash_string_fnv1a64m(eff->name);
        *(int32_t*)(d + 8) = eff->data.appear_time;
        *(int32_t*)(d + 12) = eff->data.life_time;
        *(int32_t*)(d + 16) = eff->data.start_time;
        *(int32_t*)(d + 20) = 0xFFFFFFFF;
        *(int32_t*)(d + 24) = eff->data.flags & EFFECT_LOOP ? 1 : 0;
        *(vec3*)(d + 28) = eff->translation;
        *(vec3*)(d + 40) = eff->rotation;
        *(int32_t*)(d + 52) = flags;
        d += 56;

        if (eff->version == 7) {
            *(float_t*)d = eff->data.emission;
            d += 4;
        }

        if (eff->data.flags & EFFECT_LOCAL) {
            *(int32_t*)d = 1;
            d += 4;
        }
        else if (ext_anim) {
            if (ext_anim->flags & EFFECT_EXT_ANIM_CHARA_ANIM) {
                *(int32_t*)d = 2;
                d += 4;

                EffectExtAnimFlag ext_anim_flag = ext_anim->flags;
                enum_and(ext_anim_flag, ~EFFECT_EXT_ANIM_CHARA_ANIM);

                *(int32_t*)d = ext_anim->chara_index;
                *(int32_t*)(d + 4) = ext_anim_flag;
                *(int32_t*)(d + 8) = ext_anim->node_index;
            }
            else {
                *(int32_t*)d = 3;
                d += 4;

                *(uint64_t*)d = ext_anim->object_hash;
                *(int32_t*)(d + 8) = ext_anim->flags;
                strncpy_s((char*)(d + 12), 0x80, ext_anim->mesh_name, 0x80);
                ((char*)(d + 12))[0x7F] = '\0';
            }
        }
        else {
            *(int32_t*)d = 0;
            d += 4;
        }

        st->enrs = e;

        st->header.signature = reverse_endianness_uint32_t('EFCT');
        st->header.length = 0x20;
        st->header.use_big_endian = false;
        st->header.use_section_size = true;
        st->header.version = eff->version;
        return true;
    }

    bool FileWriter::PackEmitter(f2_struct* st, Emitter* emit) {
        if (emit->version != 1 && emit->version != 2)
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
        d += 96;

        switch (emit->data.type) {
        case EMITTER_BOX:
            *(vec3*)d = emit->data.box.size;
            break;
        case EMITTER_CYLINDER:
            *(float_t*)d = emit->data.cylinder.radius;
            *(float_t*)(d + 4) = emit->data.cylinder.height;
            *(float_t*)(d + 8) = emit->data.cylinder.start_angle;
            *(float_t*)(d + 12) = emit->data.cylinder.end_angle;
            *(int32_t*)(d + 16) = ((int32_t)emit->data.cylinder.direction << 1)
                | (emit->data.cylinder.on_edge ? 1 : 0);
            break;
        case EMITTER_SPHERE:
            *(float_t*)d = emit->data.sphere.radius;
            *(float_t*)(d + 4) = emit->data.sphere.latitude;
            *(float_t*)(d + 8) = emit->data.sphere.longitude;
            *(int32_t*)(d + 12) = ((int32_t)emit->data.sphere.direction << 1)
                | (emit->data.sphere.on_edge ? 1 : 0);
            break;
        case EMITTER_POLYGON:
            *(float_t*)d = emit->data.polygon.size;
            *(int32_t*)(d + 4) = emit->data.polygon.count;
            break;
        }

        st->enrs = e;

        st->header.signature = reverse_endianness_uint32_t('EMIT');
        st->header.length = 0x20;
        st->header.use_big_endian = false;
        st->header.use_section_size = true;
        st->header.version = emit->version;
        return true;
    }

    bool FileWriter::PackParticle(EffectGroup* eff_group, f2_struct* st, Particle* ptcl, Effect* eff) {
        if (ptcl->version < 2)
            return false;

        size_t l = 0;

        uint32_t o;
        enrs e;
        enrs_entry ee;

        ee = { 0, 1, 204, 1 };
        ee.append(0, 51, ENRS_DWORD);
        e.vec.push_back(ee);
        l += o = 204;

        if (ptcl->version == 3) {
            ee = { o, 1, 8, 1 };
            ee.append(0, 2, ENRS_DWORD);
            e.vec.push_back(ee);
            l += o = 8;
        }

        if (ptcl->data.type == PARTICLE_LOCUS || ptcl->data.type == PARTICLE_MESH) {
            ee = { o, 1, 4, 1 };
            ee.append(0, 2, ENRS_WORD);
            e.vec.push_back(ee);
            l += o = 4;
        }

        ee = { o, 4, 44, 1 };
        ee.append(0, 1, ENRS_QWORD);
        ee.append(4, 5, ENRS_DWORD);
        ee.append(0, 2, ENRS_WORD);
        ee.append(0, 2, ENRS_DWORD);
        if (eff_group->version >= 7) {
            ee.count++;
            ee.size += 4;
            ee.append(0, 1, ENRS_DWORD);
            e.vec.push_back(ee);
            l += o = 48;
        }
        else {
            e.vec.push_back(ee);
            l += o = 44;
        }

        ee = { o, 2, 12, 1 };
        ee.append(0, 1, ENRS_QWORD);
        ee.append(0, 1, ENRS_DWORD);
        e.vec.push_back(ee);
        l += o = 12;

        l = align_val(l, 0x10);
        st->data.resize(l);
        size_t d = (size_t)st->data.data();

        ParticleFlag flags = ptcl->data.flags;
        if (eff->data.flags & EFFECT_LOCAL)
            enum_and(flags, ~PARTICLE_LOCAL);
        if (eff->data.flags & EFFECT_EMISSION)
            enum_and(flags, ~PARTICLE_EMISSION);

        ParticleType type;
        if (ptcl->data.type == PARTICLE_LINE)
            type = PARTICLE_MESH;
        else if (ptcl->data.type == PARTICLE_MESH)
            type = PARTICLE_LINE;
        else
            type = ptcl->data.type;

        *(int32_t*)d = ptcl->data.life_time;
        *(int32_t*)(d + 4) = type;
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
        d += 204;

        if (ptcl->version == 3) {
            *(int32_t*)d = 0;
            *(float_t*)(d + 4) = ptcl->data.emission;
            d += 8;
        }

        if (ptcl->data.type == PARTICLE_LOCUS || ptcl->data.type == PARTICLE_MESH) {
            *(uint16_t*)d = ptcl->data.locus_history_size;
            *(uint16_t*)(d + 2) = ptcl->data.locus_history_size_random;
            d += 4;
        }

        *(uint64_t*)d = ptcl->data.tex_hash;
        *(uint8_t*)(d + 8) = (uint8_t)roundf(clamp(ptcl->data.color.x, 0.0f, 1.0f) * 255.0f);
        *(uint8_t*)(d + 9) = (uint8_t)roundf(clamp(ptcl->data.color.y, 0.0f, 1.0f) * 255.0f);
        *(uint8_t*)(d + 10) = (uint8_t)roundf(clamp(ptcl->data.color.z, 0.0f, 1.0f) * 255.0f);
        *(uint8_t*)(d + 11) = (uint8_t)roundf(clamp(ptcl->data.color.w, 0.0f, 1.0f) * 255.0f);
        *(int32_t*)(d + 12) = ptcl->data.blend_mode;
        *(int32_t*)(d + 16) = ptcl->data.unk0;
        *(int32_t*)(d + 20) = ptcl->data.split_u;
        *(int32_t*)(d + 24) = ptcl->data.split_v;
        *(int32_t*)(d + 28) = ptcl->data.uv_index_type;
        *(int16_t*)(d + 32) = ptcl->data.uv_index;
        *(int16_t*)(d + 34) = (int16_t)ptcl->data.frame_step_uv;
        *(int32_t*)(d + 36) = ptcl->data.uv_index_start;
        *(int32_t*)(d + 40) = ptcl->data.uv_index_end;
        if (eff_group->version >= 7) {
            *(int32_t*)(d + 44) = ptcl->data.unk1;
            d += 48;
        }
        else
            d += 44;

        if (ptcl->data.flags & PARTICLE_TEXTURE_MASK) {
            *(uint64_t*)d = ptcl->data.mask_tex_hash;
            *(int32_t*)(d + 8) = ptcl->data.mask_blend_mode;
            d += 12;
        }
        else {
            *(uint64_t*)d = 0;
            *(int32_t*)(d + 8) = 0;
            d += 12;
        }

        st->enrs = e;

        st->header.signature = reverse_endianness_uint32_t('PTCL');
        st->header.length = 0x20;
        st->header.use_big_endian = false;
        st->header.use_section_size = true;
        st->header.version = ptcl->version;
        return true;
    }

    bool FileWriter::UnparseAnimation(f2_struct* st, Animation* anim, CurveTypeFlags flags) {
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
            if (~flags & (1 << (size_t)order[i]))
                continue;

            for (Curve*& j : anim->curves) {
                Curve* c = j;
                if (!c || c->type != order[i])
                    continue;

                f2_struct s;
                if (UnparseCurve(&s, c)) {
                    st->sub_structs.push_back(s);
                    break;
                }
            }
        }

        if (st->sub_structs.size() < 1)
            return false;

        st->header.signature = reverse_endianness_uint32_t('ANIM');
        st->header.length = 0x20;
        st->header.use_big_endian = false;
        st->header.use_section_size = true;
        return true;
    }

    bool FileWriter::UnparseCurve(f2_struct* st, Curve* c) {
#if !defined(CRE_DEV)
        if (c->keys.size() < 1)
            return false;
#else
        std::vector<Curve::Key> keys = c->keys;
        if (c->keys_rev.size() < 1)
            return false;

        c->Recalculate(type);
        if (!c->keys.size()) {
            c->keys = keys;
            return false;
        }
#endif

        size_t l;
        size_t d;
        l = 0;

        uint32_t o;
        enrs e;
        enrs_entry ee;

        ee = { 0, 2, 32, 1 };
        ee.append(0, 4, ENRS_DWORD);
        ee.append(0, 3, ENRS_WORD);
        e.vec.push_back(ee);
        l += o = 32;

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

        *(uint32_t*)d = c->type;
        *(uint32_t*)(d + 4) = c->repeat ? 1 : 0;
        *(uint32_t*)(d + 8) = c->flags;
        *(float_t*)(d + 12) = random_range;
        *(int16_t*)(d + 16) = (int16_t)c->keys.size();
        *(int16_t*)(d + 18) = (int16_t)c->start_time;
        *(int16_t*)(d + 20) = (int16_t)c->end_time;

        st->enrs = e;

        st->header.signature = reverse_endianness_uint32_t('CURV');
        st->header.length = 0x20;
        st->header.use_big_endian = false;
        st->header.use_section_size = true;
        st->header.version = c->version;

        f2_struct s;
        PackCurve(&s, c);
        st->sub_structs.push_back(s);

#if defined(CRE_DEV)
        c->keys = keys;
#endif
        return true;
    }

    bool FileWriter::UnparseDivaEffect(EffectGroup* eff_group, f2_struct* st) {
        UnparseEffectGroup(eff_group, st);

        st->header.signature = reverse_endianness_uint32_t('DVEF');
        st->header.length = 0x20;
        st->header.use_big_endian = false;
        st->header.use_section_size = true;
        st->header.version = eff_group->version;
        return true;
    }

    bool FileWriter::UnparseDivaList(EffectGroup* eff_group, f2_struct* st) {
        f2_struct s;
        if (!PackDivaList(eff_group, &s))
            return false;

        st->sub_structs.push_back(s);

        st->header.signature = reverse_endianness_uint32_t('LIST');
        st->header.length = 0x20;
        st->header.use_big_endian = false;
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

    bool FileWriter::UnparseEffect(EffectGroup* eff_group, f2_struct* st, Effect* eff) {
        if (!PackEffect(st, eff))
            return false;

        f2_struct s;
        if (UnparseAnimation(&s, &eff->animation, effect_curve_flags))
            st->sub_structs.push_back(s);

        for (Emitter*& i : eff->emitters) {
            if (!i)
                continue;

            f2_struct s;
            if (UnparseEmitter(eff_group, &s, i, eff))
                st->sub_structs.push_back(s);
        }
        return true;
    }

    void FileWriter::UnparseEffectGroup(EffectGroup* eff_group, f2_struct* st) {
        for (Effect*& i : eff_group->effects) {
            if (!i)
                continue;

            f2_struct s;
            if (UnparseEffect(eff_group, &s, i))
                st->sub_structs.push_back(s);
        }

        f2_struct s;
        if (PackDivaResourceHashes(eff_group, &s))
            st->sub_structs.push_back(s);
    }

    bool FileWriter::UnparseEmitter(EffectGroup* eff_group, f2_struct* st, Emitter* emit, Effect* eff) {
        if (!PackEmitter(st, emit))
            return false;

        f2_struct s;
        if (UnparseAnimation(&s, &emit->animation, emitter_curve_flags))
            st->sub_structs.push_back(s);

        for (Particle*& i : emit->particles) {
            if (!i)
                continue;

            f2_struct s;
            if (UnparseParticle(eff_group, &s, i, eff))
                st->sub_structs.push_back(s);
        }
        return true;
    }

    bool FileWriter::UnparseParticle(EffectGroup* eff_group, f2_struct* st, Particle* ptcl, Effect* eff) {
        if (!PackParticle(eff_group, st, ptcl, eff))
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
        if (UnparseAnimation(&s, &ptcl->animation, flags))
            st->sub_structs.push_back(s);
        return true;
    }

    void FileWriter::Write(GLT, EffectGroup* eff_group, const char* path, const char* file, bool compress) {
        FileWriter fr;
        fr.type = GLT_VAL;

        farc f;
        {
            f2_struct st;
            if (fr.UnparseDivaEffect(eff_group, &st)) {
                f.add_file();
                farc_file& ff_drs = f.files.back();
                st.write(&ff_drs.data, &ff_drs.size);
                ff_drs.name = std::string(file) + ".dve";
            }
        }

        {
            f2_struct st;
            if (fr.UnparseDivaResource(eff_group, &st)) {
                f.add_file();
                farc_file& ff_dve = f.files.back();
                st.write(&ff_dve.data, &ff_dve.size);
                ff_dve.name = std::string(file) + ".drs";
            }
            else
                return;
        }

        if (fr.type == Glitter::FT) {
            f2_struct st;
            if (fr.UnparseDivaList(eff_group, &st)) {
                f.add_file();
                farc_file& ff_lst = f.files.back();
                st.write(&ff_lst.data, &ff_lst.size);
                ff_lst.name = std::string(file) + ".lst";
            }
            else
                return;
        }

        farc_compress_mode mode;
        if (compress)
            mode = glt_type != Glitter::FT ? FARC_COMPRESS_FARC_GZIP_AES : FARC_COMPRESS_FArC;
        else
            mode = FARC_COMPRESS_FArc;

        char* temp = str_utils_add(path, file);
        if (glt_type != Glitter::FT) {
            char* list_temp = str_utils_add(temp, ".glitter.txt");
            stream s;
            s.open(list_temp, "wb");
            if (s.io.stream) {
                for (Glitter::Effect*& i : eff_group->effects)
                    if (i) {
                        s.write_string(i->name);
                        s.write_char('\n');
                    }
            }
            free(list_temp);
        }
        f.write(temp, mode, false);
        free(temp);
    }
}
