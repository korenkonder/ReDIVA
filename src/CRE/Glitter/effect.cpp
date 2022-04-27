/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"

namespace Glitter {
    object_info Effect::ExtAnim::GetObjectInfo(uint64_t hash, object_database* obj_db) {
        for (object_set_info& i : obj_db->object_set) {
            for (object_info_data& j : i.object)
                if (hash == j.name_hash_murmurhash || hash == j.name_hash_fnv1a64m_upper)
                    return { j.id, i.id };
        }
        return object_info();
    }

    Effect::Data::Data(GLT) : appear_time(), life_time(),
        start_time(), ext_anim(), flags(), emission(), seed() {
        color = { 0xFF, 0xFF, 0xFF, 0xFF };
        name_hash = GLT_VAL != Glitter::FT
            ? hash_murmurhash_empty : hash_fnv1a64m_empty;
    }

    Effect::Effect(GLT) : data(GLT_VAL) {
        version = GLT_VAL == Glitter::X ? 0x0C : 0x07;
    }

    Effect::~Effect() {
        for (Emitter*& i : emitters)
            delete i;
    }
}

/*
#include "effect.hpp"
#include "animation.hpp"
#include "emitter.hpp"

static bool glitter_effect_pack_file(GLT, f2_struct* st, glitter_effect* a3);

glitter_effect::glitter_effect(GLT) : name(), translation(), rotation(), data() {
}

glitter_effect::~glitter_effect() {
    free(data.ext_anim);
    for (glitter_emitter*& i : emitters)
        delete i;
}

bool glitter_effect_parse_file(EffectGroup* a1,
    f2_struct* st, std::vector<glitter_effect*>* vec, object_database* obj_db) {
    glitter_effect* effect;

    if (!st || !st->header.data_size)
        return false;

    effect = new glitter_effect(a1->type);
    effect->version = st->header.version;
    if (!glitter_effect_unpack_file(a1->type, st->data.data(),
        effect, st->header.use_big_endian, obj_db)) {
        delete effect;
        return false;
    }

    for (f2_struct& i : st->sub_structs) {
        if (!i.header.data_size)
            continue;

        if (i.header.signature == reverse_endianness_uint32_t('ANIM'))
            glitter_animation_parse_file(a1->type, &i, &effect->animation, glitter_effect_curve_flags);
        else if (i.header.signature == reverse_endianness_uint32_t('EMIT'))
            glitter_emitter_parse_file(a1, &i, &effect->emitters, effect);
    }
    vec->push_back(effect);
    return true;
}

static bool glitter_effect_pack_file(GLT, f2_struct* st, glitter_effect* a2) {
    size_t l;
    size_t d;
    glitter_effect_ext_anim* ext_anim;
    glitter_effect_file_flag flags;

    if (a2->version != 6 && a2->version != 7)
        return false;

    ext_anim = a2->data.ext_anim;
    l = 0;

    uint32_t o;
    enrs e;
    enrs_entry ee;

    ee = { 0, 2, 56, 1 };
    ee.sub.push_back({ 0, 1, ENRS_QWORD });
    ee.sub.push_back({ 0, 12, ENRS_DWORD });
    e.vec.push_back(ee);
    l += o = 56;

    if (a2->version == 7) {
        ee = { 0, 1, 4, 1 };
        ee.sub.push_back({ 0, 1, ENRS_DWORD });
        e.vec.push_back(ee);
        l += o = 4;
    }

    ee = { 0, 1, 4, 1 };
    ee.sub.push_back({ 0, 1, ENRS_DWORD });
    e.vec.push_back(ee);
    l += o = 4;

    if (~a2->data.flags & EFFECT_LOCAL && ext_anim)
        if (ext_anim->flags & EFFECT_EXT_ANIM_CHARA_ANIM) {
            ee = { 0, 1, 12, 1 };
            ee.sub.push_back({ 0, 3, ENRS_DWORD });
            e.vec.push_back(ee);
            l += o = 12;
        }
        else {
            ee = { 0, 2, 140, 1 };
            ee.sub.push_back({ 0, 1, ENRS_QWORD });
            ee.sub.push_back({ 0, 1, ENRS_DWORD });
            e.vec.push_back(ee);
            l += o = 140;
        }

    l = align_val(l, 0x10);
    st->data.resize(l);
    d = (size_t)st->data.data();
    st->enrs = e;

    flags = (glitter_effect_file_flag)0;
    if (a2->data.flags & EFFECT_ALPHA)
        enum_or(flags, EFFECT_FILE_ALPHA);

    if (a2->data.flags & EFFECT_FOG)
        enum_or(flags, EFFECT_FILE_FOG);
    else if (a2->data.flags & EFFECT_FOG_HEIGHT)
        enum_or(flags, EFFECT_FILE_FOG_HEIGHT);

    if (a2->data.flags & EFFECT_EMISSION)
        enum_or(flags, EFFECT_FILE_EMISSION);

    *(uint64_t*)d = GLT_VAL != Glitter::FT
        ? hash_utf8_murmurhash(a2->name)
        : hash_utf8_fnv1a64m(a2->name);;
    *(int32_t*)(d + 8) = a2->data.appear_time;
    *(int32_t*)(d + 12) = a2->data.life_time;
    *(int32_t*)(d + 16) = a2->data.start_time;
    *(int32_t*)(d + 20) = 0xFFFFFFFF;
    *(int32_t*)(d + 24) = a2->data.flags & EFFECT_LOOP ? 1 : 0;
    *(vec3*)(d + 28) = a2->translation;
    *(vec3*)(d + 40) = a2->rotation;
    *(int32_t*)(d + 52) = flags;
    d += 56;

    if (a2->version == 7) {
        *(float_t*)d = a2->data.emission;
        d += 4;
    }

    if (a2->data.flags & EFFECT_LOCAL) {
        *(int32_t*)d = 1;
        d += 4;
    }
    else if (ext_anim)
        if (ext_anim->flags & EFFECT_EXT_ANIM_CHARA_ANIM) {
            *(int32_t*)d = 2;
            d += 4;
            glitter_effect_ext_anim_flag ext_anim_flag
                = (glitter_effect_ext_anim_flag)(ext_anim->flags & ~EFFECT_EXT_ANIM_CHARA_ANIM);
            *(int32_t*)d = ext_anim->chara_index;
            *(int32_t*)(d + 4) = ext_anim_flag;
            *(int32_t*)(d + 8) = ext_anim->node_index;
        }
        else {
            *(int32_t*)d = 3;
            d += 4;
            *(uint64_t*)d = ext_anim->object_hash;
            *(int32_t*)(d + 8) = ext_anim->flags;
            memcpy((uint8_t*)(d + 12), ext_anim->mesh_name, 128);
        }
    else {
        *(int32_t*)d = 0;
        d += 4;
    }

    st->header.signature = reverse_endianness_uint32_t('EFCT');
    st->header.length = 0x20;
    st->header.use_big_endian = false;
    st->header.use_section_size = true;
    st->header.version = a2->version;
    return true;
}
*/