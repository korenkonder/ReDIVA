/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "effect.h"
#include "animation.h"
#include "emitter.h"

static bool glitter_effect_pack_file(GLT, f2_struct* st, glitter_effect* a3);
static bool glitter_effect_unpack_file(GLT, void* data,
    glitter_effect* a3, bool use_big_endian, object_database* obj_db);

glitter_effect::glitter_effect(GLT) : name(), translation(), rotation(), data() {
    version = GLT_VAL == GLITTER_X ? 0x0C : 0x07;
    scale = vec3_identity;
    data.start_time = 0;
    data.ext_anim = 0;
    data.flags = (glitter_effect_flag)0;
    data.name_hash = GLT_VAL != GLITTER_FT ? hash_murmurhash_empty : hash_fnv1a64m_empty;
    data.seed = 0;
}

glitter_effect::~glitter_effect() {
    free(data.ext_anim);
    for (glitter_emitter*& i : emitters)
        delete i;
}

bool glitter_effect_parse_file(GlitterEffectGroup* a1,
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

bool glitter_effect_unparse_file(GLT,
    GlitterEffectGroup* a1, f2_struct* st, glitter_effect* a3) {
    if (!glitter_effect_pack_file(GLT_VAL, st, a3))
        return false;

    f2_struct s;
    if (glitter_animation_unparse_file(GLT_VAL, &s, &a3->animation, glitter_effect_curve_flags))
        st->sub_structs.push_back(s);

    for (glitter_emitter*& i : a3->emitters) {
        if (!i)
            continue;

        f2_struct s;
        if (glitter_emitter_unparse_file(GLT_VAL, a1, &s, i, a3))
            st->sub_structs.push_back(s);
    }
    return true;
}

object_info glitter_effect_ext_anim_get_object_info(uint64_t hash, object_database* obj_db) {
    for (object_set_info& i : obj_db->object_set) {
        for (object_info_data& j : i.object)
            if (hash == j.name_hash_murmurhash || hash == j.name_hash_fnv1a64m_upper)
                return { j.id, i.id };
    }
    return object_info();
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

    if (~a2->data.flags & GLITTER_EFFECT_LOCAL && ext_anim)
        if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_CHARA_ANIM) {
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
    if (a2->data.flags & GLITTER_EFFECT_ALPHA)
        enum_or(flags, GLITTER_EFFECT_FILE_ALPHA);

    if (a2->data.flags & GLITTER_EFFECT_FOG)
        enum_or(flags, GLITTER_EFFECT_FILE_FOG);
    else if (a2->data.flags & GLITTER_EFFECT_FOG_HEIGHT)
        enum_or(flags, GLITTER_EFFECT_FILE_FOG_HEIGHT);

    if (a2->data.flags & GLITTER_EFFECT_EMISSION)
        enum_or(flags, GLITTER_EFFECT_FILE_EMISSION);

    *(uint64_t*)d = GLT_VAL != GLITTER_FT
        ? hash_utf8_murmurhash(a2->name)
        : hash_utf8_fnv1a64m(a2->name);;
    *(int32_t*)(d + 8) = a2->data.appear_time;
    *(int32_t*)(d + 12) = a2->data.life_time;
    *(int32_t*)(d + 16) = a2->data.start_time;
    *(int32_t*)(d + 20) = 0xFFFFFFFF;
    *(int32_t*)(d + 24) = a2->data.flags & GLITTER_EFFECT_LOOP ? 1 : 0;
    *(vec3*)(d + 28) = a2->translation;
    *(vec3*)(d + 40) = a2->rotation;
    *(int32_t*)(d + 52) = flags;
    d += 56;

    if (a2->version == 7) {
        *(float_t*)d = a2->data.emission;
        d += 4;
    }

    if (a2->data.flags & GLITTER_EFFECT_LOCAL) {
        *(int32_t*)d = 1;
        d += 4;
    }
    else if (ext_anim)
        if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_CHARA_ANIM) {
            *(int32_t*)d = 2;
            d += 4;
            glitter_effect_ext_anim_flag ext_anim_flag
                = (glitter_effect_ext_anim_flag)(ext_anim->flags & ~GLITTER_EFFECT_EXT_ANIM_CHARA_ANIM);
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

static bool glitter_effect_unpack_file(GLT, void* data,
    glitter_effect* a2, bool use_big_endian, object_database* obj_db) {
    size_t d;
    glitter_effect_ext_anim* ext_anim;
    glitter_effect_ext_anim_x* ext_anim_x;
    glitter_effect_file_flag flags;

    if (GLT_VAL == GLITTER_X) {
        a2->scale = vec3_identity;
        a2->data.start_time = 0;
        a2->data.ext_anim = 0;
        a2->data.flags = (glitter_effect_flag)0;
        a2->data.name_hash = hash_murmurhash_empty;
        a2->data.seed = 0;

        if (a2->version < 8 || a2->version > 12)
            return false;

        d = (size_t)data;
        if (use_big_endian) {
            a2->data.name_hash = load_reverse_endianness_uint64_t((void*)d);
            a2->data.appear_time = load_reverse_endianness_int32_t((void*)(d + 8));
            a2->data.life_time = load_reverse_endianness_int32_t((void*)(d + 12));
            a2->data.start_time = load_reverse_endianness_int32_t((void*)(d + 16));
            if (load_reverse_endianness_int32_t((void*)(d + 24)))
                enum_or(a2->data.flags, GLITTER_EFFECT_LOOP);
            a2->translation.x = load_reverse_endianness_float_t((void*)(d + 28));
            a2->translation.y = load_reverse_endianness_float_t((void*)(d + 32));
            a2->translation.z = load_reverse_endianness_float_t((void*)(d + 36));
            a2->rotation.x = load_reverse_endianness_float_t((void*)(d + 40));
            a2->rotation.y = load_reverse_endianness_float_t((void*)(d + 44));
            a2->rotation.z = load_reverse_endianness_float_t((void*)(d + 48));
            flags = (glitter_effect_file_flag)load_reverse_endianness_int32_t((void*)(d + 52));
        }
        else {
            a2->data.name_hash = *(uint64_t*)data;
            a2->data.appear_time = *(int32_t*)(d + 8);
            a2->data.life_time = *(int32_t*)(d + 12);
            a2->data.start_time = *(int32_t*)(d + 16);
            if (*(int32_t*)(d + 24))
                enum_or(a2->data.flags, GLITTER_EFFECT_LOOP);
            a2->translation = *(vec3*)(d + 28);
            a2->rotation = *(vec3*)(d + 40);
            flags = (glitter_effect_file_flag) * (int32_t*)(d + 52);
        }

        if (flags & GLITTER_EFFECT_FILE_ALPHA)
            enum_or(a2->data.flags, GLITTER_EFFECT_ALPHA);

        if (flags & GLITTER_EFFECT_FILE_FOG)
            enum_or(a2->data.flags, GLITTER_EFFECT_FOG);
        else if (flags & GLITTER_EFFECT_FILE_FOG_HEIGHT)
            enum_or(a2->data.flags, GLITTER_EFFECT_FOG_HEIGHT);

        if (flags & GLITTER_EFFECT_FILE_EMISSION)
            enum_or(a2->data.flags, GLITTER_EFFECT_EMISSION);

        if (flags & GLITTER_EFFECT_FILE_USE_SEED)
            enum_or(a2->data.flags, GLITTER_EFFECT_USE_SEED);

        if (a2->version != 8 && flags & 0x20)
            enum_or(a2->data.flags, 0x80);

        if (use_big_endian) {
            a2->data.emission = load_reverse_endianness_float_t((void*)(d + 56));
            a2->data.seed = load_reverse_endianness_int32_t((void*)(d + 60));
        }
        else {
            a2->data.emission = *(float_t*)(d + 56);
            a2->data.seed = *(int32_t*)(d + 60);
        }

        int32_t type;
        if (use_big_endian)
            type = load_reverse_endianness_int32_t((void*)(d + (a2->version != 8 ? 76 : 80)));
        else
            type = *(int32_t*)(d + (a2->version != 8 ? 76 : 80));
        d += a2->version != 8 ? 80 : 88;

        if (type == 1)
            enum_or(a2->data.flags, GLITTER_EFFECT_LOCAL);
        else if (type == 2) {
            ext_anim_x = force_malloc_s(glitter_effect_ext_anim_x, 1);
            a2->data.ext_anim_x = ext_anim_x;
            if (ext_anim_x) {
                if (use_big_endian) {
                    ext_anim_x->chara_index = load_reverse_endianness_int32_t((void*)d);
                    ext_anim_x->flags = (glitter_effect_ext_anim_flag)
                        load_reverse_endianness_int32_t((void*)(d + 4));
                    ext_anim_x->node_index = (glitter_effect_ext_anim_chara_node)
                        load_reverse_endianness_int32_t((void*)(d + 8));
                }
                else {
                    ext_anim_x->chara_index = *(int32_t*)d;
                    ext_anim_x->flags = (glitter_effect_ext_anim_flag) * (int32_t*)(d + 4);
                    ext_anim_x->node_index = (glitter_effect_ext_anim_chara_node) * (int32_t*)(d + 8);
                }

                enum_or(ext_anim_x->flags, GLITTER_EFFECT_EXT_ANIM_CHARA_ANIM);
            }
        }
        else if (type == 3) {
            if (a2->version == 8) {
                ext_anim_x = force_malloc_s(glitter_effect_ext_anim_x, 1);
                a2->data.ext_anim_x = ext_anim_x;
                if (ext_anim_x) {
                    if (use_big_endian) {
                        ext_anim_x->object_hash = (uint32_t)load_reverse_endianness_uint64_t((void*)d);
                        ext_anim_x->flags = (glitter_effect_ext_anim_flag)
                            load_reverse_endianness_int32_t((void*)(d + 8));
                    }
                    else {
                        ext_anim_x->object_hash = (uint32_t) * (uint64_t*)d;
                        ext_anim_x->flags = (glitter_effect_ext_anim_flag) * (int32_t*)(d + 8);
                    }

                    ext_anim_x->instance_id = 0;
                    ext_anim_x->file_name_hash = hash_murmurhash_empty;
                    if (*(char*)(d + 12)) {
                        strncpy_s(ext_anim_x->mesh_name, 0x80, (char*)(d + 20), 0x80);
                        ext_anim_x->mesh_name[0x7F] = '\0';
                    }
                    else
                        ext_anim_x->mesh_name[0] = 0;
                }
            }
            else if (a2->version == 10) {
                ext_anim_x = force_malloc_s(glitter_effect_ext_anim_x, 1);
                a2->data.ext_anim_x = ext_anim_x;
                if (ext_anim_x) {
                    if (use_big_endian) {
                        ext_anim_x->object_hash = (uint32_t)load_reverse_endianness_uint64_t((void*)d);
                        ext_anim_x->flags = (glitter_effect_ext_anim_flag)
                            load_reverse_endianness_int32_t((void*)(d + 8));
                    }
                    else {
                        ext_anim_x->object_hash = (uint32_t) * (uint64_t*)d;
                        ext_anim_x->flags = (glitter_effect_ext_anim_flag) * (int32_t*)(d + 8);
                    }

                    ext_anim_x->instance_id = 0;
                    ext_anim_x->file_name_hash = hash_murmurhash_empty;
                    if (*(char*)(d + 16)) {
                        strncpy_s(ext_anim_x->mesh_name, 0x80, (char*)(d + 16), 0x80);
                        ext_anim_x->mesh_name[0x7F] = '\0';
                    }
                    else
                        ext_anim_x->mesh_name[0] = 0;
                }
            }
            else {
                ext_anim_x = force_malloc_s(glitter_effect_ext_anim_x, 1);
                a2->data.ext_anim_x = ext_anim_x;
                if (ext_anim_x) {
                    if (use_big_endian) {
                        ext_anim_x->object_hash = (uint32_t)load_reverse_endianness_uint64_t((void*)d);
                        ext_anim_x->flags = (glitter_effect_ext_anim_flag)
                            load_reverse_endianness_int32_t((void*)(d + 8));
                        ext_anim_x->instance_id = load_reverse_endianness_int32_t((void*)(d + 12));
                        ext_anim_x->file_name_hash = (uint32_t)load_reverse_endianness_uint64_t((void*)(d + 16));
                    }
                    else {
                        ext_anim_x->object_hash = (uint32_t) * (uint64_t*)d;
                        ext_anim_x->flags = (glitter_effect_ext_anim_flag) * (int32_t*)(d + 8);
                        ext_anim_x->instance_id = *(int32_t*)(d + 12);
                        ext_anim_x->file_name_hash = (uint32_t) * (uint64_t*)(d + 16);
                    }

                    if (*(char*)(d + 32)) {
                        strncpy_s(ext_anim_x->mesh_name, 0x80, (char*)(d + 32), 0x80);
                        ext_anim_x->mesh_name[0x7F] = '\0';
                    }
                    else
                        ext_anim_x->mesh_name[0] = 0;
                }
            }
        }
    }
    else {
        a2->scale = vec3_identity;
        a2->data.start_time = 0;
        a2->data.ext_anim = 0;
        a2->data.flags = (glitter_effect_flag)0;
        a2->data.name_hash = GLT_VAL != GLITTER_FT ? hash_murmurhash_empty : hash_fnv1a64m_empty;
        a2->data.seed = 0;

        if (a2->version != 6 && a2->version != 7)
            return false;

        d = (size_t)data;
        if (use_big_endian) {
            a2->data.name_hash = load_reverse_endianness_uint64_t((void*)d);
            a2->data.appear_time = load_reverse_endianness_int32_t((void*)(d + 8));
            a2->data.life_time = load_reverse_endianness_int32_t((void*)(d + 12));
            a2->data.start_time = load_reverse_endianness_int32_t((void*)(d + 16));
            if (load_reverse_endianness_int32_t((void*)(d + 24)))
                enum_or(a2->data.flags, GLITTER_EFFECT_LOOP);
            a2->translation.x = load_reverse_endianness_float_t((void*)(d + 28));
            a2->translation.y = load_reverse_endianness_float_t((void*)(d + 32));
            a2->translation.z = load_reverse_endianness_float_t((void*)(d + 36));
            a2->rotation.x = load_reverse_endianness_float_t((void*)(d + 40));
            a2->rotation.y = load_reverse_endianness_float_t((void*)(d + 44));
            a2->rotation.z = load_reverse_endianness_float_t((void*)(d + 48));
            flags = (glitter_effect_file_flag)load_reverse_endianness_int32_t((void*)(d + 52));
        }
        else {
            a2->data.name_hash = *(uint64_t*)data;
            a2->data.appear_time = *(int32_t*)(d + 8);
            a2->data.life_time = *(int32_t*)(d + 12);
            a2->data.start_time = *(int32_t*)(d + 16);
            if (*(int32_t*)(d + 24))
                enum_or(a2->data.flags, GLITTER_EFFECT_LOOP);
            a2->translation = *(vec3*)(d + 28);
            a2->rotation = *(vec3*)(d + 40);
            flags = (glitter_effect_file_flag) * (int32_t*)(d + 52);
        }
        d += 56;

        if (flags & GLITTER_EFFECT_FILE_ALPHA)
            enum_or(a2->data.flags, GLITTER_EFFECT_ALPHA);

        if (flags & GLITTER_EFFECT_FILE_FOG)
            enum_or(a2->data.flags, GLITTER_EFFECT_FOG);
        else if (flags & GLITTER_EFFECT_FILE_FOG_HEIGHT)
            enum_or(a2->data.flags, GLITTER_EFFECT_FOG_HEIGHT);

        if (flags & GLITTER_EFFECT_FILE_EMISSION)
            enum_or(a2->data.flags, GLITTER_EFFECT_EMISSION);

        if (a2->version == 7) {
            if (use_big_endian)
                a2->data.emission = load_reverse_endianness_float_t((void*)d);
            else
                a2->data.emission = *(float_t*)d;
            d += 4;
        }

        int32_t type;
        if (use_big_endian)
            type = load_reverse_endianness_int32_t((void*)d);
        else
            type = *(int32_t*)d;
        d += 4;

        if (type == 1)
            enum_or(a2->data.flags, GLITTER_EFFECT_LOCAL);
        else if (type == 2) {
            ext_anim = force_malloc_s(glitter_effect_ext_anim, 1);
            a2->data.ext_anim = ext_anim;
            if (ext_anim) {
                if (use_big_endian) {
                    ext_anim->chara_index = load_reverse_endianness_int32_t((void*)d);
                    ext_anim->flags = (glitter_effect_ext_anim_flag)
                        load_reverse_endianness_int32_t((void*)(d + 4));
                    ext_anim->node_index = (glitter_effect_ext_anim_chara_node)
                        load_reverse_endianness_int32_t((void*)(d + 8));
                }
                else {
                    ext_anim->chara_index = *(int32_t*)d;
                    ext_anim->flags = (glitter_effect_ext_anim_flag) * (int32_t*)(d + 4);
                    ext_anim->node_index = (glitter_effect_ext_anim_chara_node) * (int32_t*)(d + 8);
                }

                enum_or(ext_anim->flags, GLITTER_EFFECT_EXT_ANIM_CHARA_ANIM);
            }
        }
        else if (type == 3) {
            ext_anim = force_malloc_s(glitter_effect_ext_anim, 1);
            a2->data.ext_anim = ext_anim;
            if (ext_anim) {
                if (use_big_endian) {
                    ext_anim->object_hash = load_reverse_endianness_uint64_t((void*)d);
                    ext_anim->flags = (glitter_effect_ext_anim_flag)
                        load_reverse_endianness_int32_t((void*)(d + 8));
                }
                else {
                    ext_anim->object_hash = *(uint64_t*)d;
                    ext_anim->flags = (glitter_effect_ext_anim_flag) * (int32_t*)(d + 8);
                }

                ext_anim->object = glitter_effect_ext_anim_get_object_info(ext_anim->object_hash, obj_db);
                ext_anim->node_index = GLITTER_EFFECT_EXT_ANIM_CHARA_MAX;
                if (*(char*)(d + 12)) {
                    strncpy_s(ext_anim->mesh_name, 0x80, (char*)(d + 12), 0x80);
                    ext_anim->mesh_name[0x7F] = 0;
                }
                else
                    ext_anim->mesh_name[0x00] = 0;
            }
        }
    }
    return true;
}
