/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "effect.h"
#include "animation.h"
#include "emitter.h"

static bool FASTCALL glitter_effect_pack_file(GLT, f2_struct* st, glitter_effect* a3);
static bool FASTCALL glitter_effect_unpack_file(GLT,
    void* data, glitter_effect* a3, bool use_big_endian);

glitter_effect* FASTCALL glitter_effect_init(GLT) {
    glitter_effect* e = force_malloc(sizeof(glitter_effect));
    e->version = GLT_VAL == GLITTER_X ? 0x0C : 0x07;
    e->scale = vec3_identity;
    e->data.start_time = 0;
    e->data.ext_anim = 0;
    e->data.flags = 0;
    e->data.name_hash = GLT_VAL != GLITTER_AFT ? hash_murmurhash_empty : hash_fnv1a64_empty;
    e->data.seed = 0;
    return e;
}

glitter_effect* FASTCALL glitter_effect_copy(glitter_effect* e) {
    if (!e)
        return 0;

    glitter_effect* ec = force_malloc(sizeof(glitter_effect));
    *ec = *e;

    if (e->data.ext_anim) {
        ec->data.ext_anim = force_malloc(sizeof(glitter_effect_ext_anim));
        *ec->data.ext_anim = *e->data.ext_anim;
    }

    ec->emitters = (vector_ptr_glitter_emitter){ 0, 0, 0 };
    vector_ptr_glitter_emitter_append(&ec->emitters, e->emitters.end - e->emitters.begin);
    for (glitter_emitter** i = e->emitters.begin; i != e->emitters.end; i++)
        if (*i) {
            glitter_emitter* e = glitter_emitter_copy(*i);
            if (e)
                vector_ptr_glitter_emitter_push_back(&ec->emitters, &e);
        }

    ec->animation = (glitter_animation){ 0, 0, 0 };
    glitter_animation_copy(&e->animation, &ec->animation);
    return ec;
}

bool FASTCALL glitter_effect_parse_file(glitter_effect_group* a1,
    f2_struct* st, vector_ptr_glitter_effect* vec) {
    f2_struct* i;
    glitter_effect* effect;

    if (!st || !st->header.data_size)
        return false;

    effect = glitter_effect_init(a1->type);
    effect->version = st->header.version;
    if (!glitter_effect_unpack_file(a1->type, st->data, effect, st->header.use_big_endian)) {
        glitter_effect_dispose(effect);
        return false;
    }

    for (i = st->sub_structs.begin; i != st->sub_structs.end; i++) {
        if (!i->header.data_size)
            continue;

        if (i->header.signature == reverse_endianness_uint32_t('ANIM'))
            glitter_animation_parse_file(a1->type, i, &effect->animation, glitter_effect_curve_flags);
        else if (i->header.signature == reverse_endianness_uint32_t('EMIT'))
            glitter_emitter_parse_file(a1, i, &effect->emitters, effect);
    }
    vector_ptr_glitter_effect_push_back(vec, &effect);
    return true;
}

bool FASTCALL glitter_effect_unparse_file(GLT,
    glitter_effect_group* a1, f2_struct* st, glitter_effect* a3) {
    if (!glitter_effect_pack_file(GLT_VAL, st, a3))
        return false;

    f2_struct s;
    if (glitter_animation_unparse_file(GLT_VAL, &s, &a3->animation, glitter_effect_curve_flags))
        vector_f2_struct_push_back(&st->sub_structs, &s);

    for (glitter_emitter** i = a3->emitters.begin; i != a3->emitters.end; i++) {
        if (!*i)
            continue;

        f2_struct s;
        if (glitter_emitter_unparse_file(GLT_VAL, a1, &s, *i, a3))
            vector_f2_struct_push_back(&st->sub_structs, &s);
    }
    return true;
}

void FASTCALL glitter_effect_dispose(glitter_effect* e) {
    free(e->data.ext_anim);
    glitter_animation_free(&e->animation);
    vector_ptr_glitter_emitter_free(&e->emitters, glitter_emitter_dispose);
    free(e);
}

static bool FASTCALL glitter_effect_pack_file(GLT, f2_struct* st, glitter_effect* a2) {
    size_t l;
    size_t d;
    glitter_effect_ext_anim* ext_anim;
    glitter_effect_file_flag flags;

    if (a2->version != 6 && a2->version != 7)
        return false;

    ext_anim = a2->data.ext_anim;
    memset(st, 0, sizeof(f2_struct));
    l = 0;

    uint32_t o;
    vector_enrs_entry e = { 0, 0, 0 };
    enrs_entry ee;

    ee = (enrs_entry){ 0, 2, 56, 1, { 0, 0, 0 } };
    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_TYPE_QWORD });
    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 12, ENRS_TYPE_DWORD });
    vector_enrs_entry_push_back(&e, &ee);
    l += o = 56;

    if (a2->version == 7) {
        ee = (enrs_entry){ 0, 1, 4, 1, { 0, 0, 0 } };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_TYPE_DWORD });
        vector_enrs_entry_push_back(&e, &ee);
        l += o = 4;
    }

    ee = (enrs_entry){ 0, 1, 4, 1, { 0, 0, 0 } };
    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_TYPE_DWORD });
    vector_enrs_entry_push_back(&e, &ee);
    l += o = 4;

    if (~a2->data.flags & GLITTER_EFFECT_LOCAL && ext_anim)
        if (ext_anim->flags & GLITTER_EFFECT_EXT_ANIM_CHARA_ANIM) {
            ee = (enrs_entry){ 0, 1, 12, 1, { 0, 0, 0 } };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 3, ENRS_TYPE_DWORD });
            vector_enrs_entry_push_back(&e, &ee);
            l += o = 12;
        }
        else {
            ee = (enrs_entry){ 0, 2, 140, 1, { 0, 0, 0 } };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_TYPE_QWORD });
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_TYPE_DWORD });
            vector_enrs_entry_push_back(&e, &ee);
            l += o = 140;
        }

    l = align_val(l, 0x10);
    d = (size_t)force_malloc(l);
    st->length = l;
    st->data = (void*)d;
    st->enrs = e;

    flags = 0;
    if (a2->data.flags & GLITTER_EFFECT_ALPHA)
        flags |= GLITTER_EFFECT_FILE_ALPHA;

    if (a2->data.flags & GLITTER_EFFECT_FOG)
        flags |= GLITTER_EFFECT_FILE_FOG;
    else if (a2->data.flags & GLITTER_EFFECT_FOG_HEIGHT)
        flags |= GLITTER_EFFECT_FILE_FOG_HEIGHT;

    if (a2->data.flags & GLITTER_EFFECT_EMISSION)
        flags |= GLITTER_EFFECT_FILE_EMISSION;

    *(uint64_t*)d = GLT_VAL != GLITTER_AFT
        ? hash_char_murmurhash(a2->name, 0, false) : hash_char_fnv1a64(a2->name);;
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
            glitter_effect_ext_anim_flag ext_anim_flag = ext_anim->flags & ~GLITTER_EFFECT_EXT_ANIM_CHARA_ANIM;
            *(int32_t*)d = ext_anim->index;
            *(int32_t*)(d + 4) = ext_anim_flag;
            *(int32_t*)(d + 8) = ext_anim->node_index;
        }
        else {
            *(int32_t*)d = 3;
            d += 4;
            *(uint64_t*)d = ext_anim->object_hash;
            *(int32_t*)(d + 8) = ext_anim->flags;
            memcpy((char*)(d + 12), ext_anim->mesh_name, 128);
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

static bool FASTCALL glitter_effect_unpack_file(GLT,
    void* data, glitter_effect* a2, bool use_big_endian) {
    size_t d;
    glitter_effect_ext_anim* ext_anim;
    glitter_effect_file_flag flags;

    if (GLT_VAL == GLITTER_X) {
        a2->scale = vec3_identity;
        a2->data.start_time = 0;
        a2->data.ext_anim = 0;
        a2->data.flags = 0;
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
                a2->data.flags |= GLITTER_EFFECT_LOOP;
            a2->translation.x = load_reverse_endianness_float_t((void*)(d + 28));
            a2->translation.y = load_reverse_endianness_float_t((void*)(d + 32));
            a2->translation.z = load_reverse_endianness_float_t((void*)(d + 36));
            a2->rotation.x = load_reverse_endianness_float_t((void*)(d + 40));
            a2->rotation.y = load_reverse_endianness_float_t((void*)(d + 44));
            a2->rotation.z = load_reverse_endianness_float_t((void*)(d + 48));
            flags = load_reverse_endianness_int32_t((void*)(d + 52));
        }
        else {
            a2->data.name_hash = *(uint64_t*)data;
            a2->data.appear_time = *(int32_t*)(d + 8);
            a2->data.life_time = *(int32_t*)(d + 12);
            a2->data.start_time = *(int32_t*)(d + 16);
            if (*(int32_t*)(d + 24))
                a2->data.flags |= GLITTER_EFFECT_LOOP;
            a2->translation = *(vec3*)(d + 28);
            a2->rotation = *(vec3*)(d + 40);
            flags = *(int32_t*)(d + 52);
        }

        if (flags & GLITTER_EFFECT_FILE_ALPHA)
            a2->data.flags |= GLITTER_EFFECT_ALPHA;

        if (flags & GLITTER_EFFECT_FILE_FOG)
            a2->data.flags |= GLITTER_EFFECT_FOG;
        else if (flags & GLITTER_EFFECT_FILE_FOG_HEIGHT)
            a2->data.flags |= GLITTER_EFFECT_FOG_HEIGHT;

        if (flags & GLITTER_EFFECT_FILE_EMISSION)
            a2->data.flags |= GLITTER_EFFECT_EMISSION;

        if (flags & GLITTER_EFFECT_FILE_USE_SEED)
            a2->data.flags |= GLITTER_EFFECT_USE_SEED;

        if (a2->version != 8 && flags & 0x20)
            a2->data.flags |= 0x80;

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
            a2->data.flags |= GLITTER_EFFECT_LOCAL;
        else if (type == 2) {
            ext_anim = force_malloc(sizeof(glitter_effect_ext_anim));
            a2->data.ext_anim = ext_anim;
            if (ext_anim) {
                if (use_big_endian) {
                    ext_anim->index = load_reverse_endianness_int32_t((void*)d);
                    ext_anim->flags = load_reverse_endianness_int32_t((void*)(d + 4));
                    ext_anim->node_index = load_reverse_endianness_int32_t((void*)(d + 8));
                }
                else {
                    ext_anim->index = *(int32_t*)d;
                    ext_anim->flags = *(int32_t*)(d + 4);
                    ext_anim->node_index = *(int32_t*)(d + 8);
                }
                ext_anim->flags |= GLITTER_EFFECT_EXT_ANIM_CHARA_ANIM;
                ext_anim->mesh_name[0] = 0;
            }
        }
        else if (type == 3) {
            if (a2->version == 8) {
                ext_anim = force_malloc(sizeof(glitter_effect_ext_anim));
                a2->data.ext_anim = ext_anim;
                if (ext_anim) {
                    if (use_big_endian) {
                        ext_anim->object_hash = load_reverse_endianness_uint64_t((void*)d);
                        ext_anim->flags = load_reverse_endianness_int32_t((void*)(d + 8));
                    }
                    else {
                        ext_anim->object_hash = *(uint64_t*)d;
                        ext_anim->flags = *(int32_t*)(d + 8);
                    }
                    ext_anim->instance_id = 0;
                    ext_anim->file_name_hash = hash_murmurhash_empty;
                    ext_anim->index = -1;
                    ext_anim->node_index = 18;
                    if (*(char*)(d + 12)) {
                        strncpy_s(ext_anim->mesh_name, 0x80, (char*)(d + 20), 0x80);
                        ext_anim->mesh_name[0x7F] = '\0';
                    }
                    else
                        ext_anim->mesh_name[0] = 0;
                }
            }
            else if (a2->version == 10) {
                ext_anim = force_malloc(sizeof(glitter_effect_ext_anim));
                a2->data.ext_anim = ext_anim;
                if (ext_anim) {
                    if (use_big_endian) {
                        ext_anim->object_hash = load_reverse_endianness_uint64_t((void*)d);
                        ext_anim->flags = load_reverse_endianness_int32_t((void*)(d + 8));
                    }
                    else {
                        ext_anim->object_hash = *(uint64_t*)d;
                        ext_anim->flags = *(int32_t*)(d + 8);
                    }
                    ext_anim->instance_id = 0;
                    ext_anim->file_name_hash = hash_murmurhash_empty;
                    ext_anim->index = -1;
                    ext_anim->node_index = 18;
                    if (*(char*)(d + 16)) {
                        strncpy_s(ext_anim->mesh_name, 0x80, (char*)(d + 16), 0x80);
                        ext_anim->mesh_name[0x7F] = '\0';
                    }
                    else
                        ext_anim->mesh_name[0] = 0;
                }
            }
            else {
                ext_anim = force_malloc(sizeof(glitter_effect_ext_anim));
                a2->data.ext_anim = ext_anim;
                if (ext_anim) {
                    if (use_big_endian) {
                        ext_anim->object_hash = load_reverse_endianness_uint64_t((void*)d);
                        ext_anim->flags = load_reverse_endianness_int32_t((void*)(d + 8));
                        ext_anim->instance_id = load_reverse_endianness_int32_t((void*)(d + 12));
                        ext_anim->file_name_hash = load_reverse_endianness_uint64_t((void*)(d + 16));
                    }
                    else {
                        ext_anim->object_hash = *(uint64_t*)d;
                        ext_anim->flags = *(int32_t*)(d + 8);
                        ext_anim->instance_id = *(int32_t*)(d + 12);
                        ext_anim->file_name_hash = *(uint64_t*)(d + 16);
                    }
                    ext_anim->index = -1;
                    ext_anim->node_index = 18;
                    if (*(char*)(d + 32)) {
                        strncpy_s(ext_anim->mesh_name, 0x80, (char*)(d + 32), 0x80);
                        ext_anim->mesh_name[0x7F] = '\0';
                    }
                    else
                        ext_anim->mesh_name[0] = 0;
                }
            }
        }
    }
    else {
        a2->scale = vec3_identity;
        a2->data.start_time = 0;
        a2->data.ext_anim = 0;
        a2->data.flags = 0;
        a2->data.name_hash = GLT_VAL != GLITTER_AFT ? hash_murmurhash_empty : hash_fnv1a64_empty;
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
                a2->data.flags |= GLITTER_EFFECT_LOOP;
            a2->translation.x = load_reverse_endianness_float_t((void*)(d + 28));
            a2->translation.y = load_reverse_endianness_float_t((void*)(d + 32));
            a2->translation.z = load_reverse_endianness_float_t((void*)(d + 36));
            a2->rotation.x = load_reverse_endianness_float_t((void*)(d + 40));
            a2->rotation.y = load_reverse_endianness_float_t((void*)(d + 44));
            a2->rotation.z = load_reverse_endianness_float_t((void*)(d + 48));
            flags = load_reverse_endianness_int32_t((void*)(d + 52));
        }
        else {
            a2->data.name_hash = *(uint64_t*)data;
            a2->data.appear_time = *(int32_t*)(d + 8);
            a2->data.life_time = *(int32_t*)(d + 12);
            a2->data.start_time = *(int32_t*)(d + 16);
            if (*(int32_t*)(d + 24))
                a2->data.flags |= GLITTER_EFFECT_LOOP;
            a2->translation = *(vec3*)(d + 28);
            a2->rotation = *(vec3*)(d + 40);
            flags = *(int32_t*)(d + 52);
        }
        d += 56;

        if (flags & GLITTER_EFFECT_FILE_ALPHA)
            a2->data.flags |= GLITTER_EFFECT_ALPHA;

        if (flags & GLITTER_EFFECT_FILE_FOG)
            a2->data.flags |= GLITTER_EFFECT_FOG;
        else if (flags & GLITTER_EFFECT_FILE_FOG_HEIGHT)
            a2->data.flags |= GLITTER_EFFECT_FOG_HEIGHT;

        if (flags & GLITTER_EFFECT_FILE_EMISSION)
            a2->data.flags |= GLITTER_EFFECT_EMISSION;

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
            a2->data.flags |= GLITTER_EFFECT_LOCAL;
        else if (type == 2) {
            ext_anim = force_malloc(sizeof(glitter_effect_ext_anim));
            a2->data.ext_anim = ext_anim;
            if (ext_anim) {
                if (use_big_endian) {
                    ext_anim->index = load_reverse_endianness_int32_t((void*)d);
                    ext_anim->flags = load_reverse_endianness_int32_t((void*)(d + 4));
                    ext_anim->node_index = load_reverse_endianness_int32_t((void*)(d + 8));
                }
                else {
                    ext_anim->index = *(int32_t*)d;
                    ext_anim->flags = *(int32_t*)(d + 4);
                    ext_anim->node_index = *(int32_t*)(d + 8);
                }
                ext_anim->flags |= GLITTER_EFFECT_EXT_ANIM_CHARA_ANIM;
                ext_anim->mesh_name[0] = 0;
            }
        }
        else if (type == 3) {
            ext_anim = force_malloc(sizeof(glitter_effect_ext_anim));
            a2->data.ext_anim = ext_anim;
            if (ext_anim) {
                ext_anim->index = -1;
                if (use_big_endian) {
                    ext_anim->object_hash = load_reverse_endianness_uint64_t((void*)d);
                    ext_anim->flags = load_reverse_endianness_int32_t((void*)(d + 8));
                }
                else {
                    ext_anim->object_hash = *(uint64_t*)d;
                    ext_anim->flags = *(int32_t*)(d + 8);
                }
                ext_anim->node_index = 18;
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
