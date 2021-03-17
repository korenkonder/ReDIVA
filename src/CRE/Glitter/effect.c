/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "effect.h"
#include "animation.h"
#include "curve.h"
#include "emitter.h"

extern glitter_particle_manager* gpm;

static bool FASTCALL glitter_effect_pack_file(f2_struct* st, glitter_effect* a3, bool use_big_endian);
static bool FASTCALL glitter_effect_unpack_file(int32_t* data, glitter_effect* a3, bool use_big_endian);

glitter_effect* FASTCALL glitter_effect_init() {
    glitter_effect* e = force_malloc(sizeof(glitter_effect));
    e->data.name_hash = gpm->f2 ? 0x0CAD3078 : 0xCBF29CE44FD0BFC1;
    return e;
}

bool FASTCALL glitter_effect_parse_file(glitter_effect_group* a1,
    f2_struct* st, vector_ptr_glitter_effect* vec) {
    f2_struct* i;
    glitter_effect* effect;

    if (!st || !st->header.data_size)
        return false;

    effect = glitter_effect_init();
    effect->version = st->header.version;
    if (!glitter_effect_unpack_file(st->data, effect, st->header.use_big_endian)) {
        glitter_effect_dispose(effect);
        return false;
    }

    for (i = st->sub_structs.begin; i != st->sub_structs.end; i++) {
        if (!i->header.data_size)
            continue;

        if (i->header.signature == 0x4D494E41)
            glitter_animation_parse_file(i, &effect->curve);
        else if (i->header.signature == 0x54494D45)
            glitter_emitter_parse_file(a1, i, &effect->emitters, effect);
    }
    vector_ptr_glitter_effect_push_back(vec, &effect);
    return true;
}

bool FASTCALL glitter_effect_unparse_file(glitter_effect_group* a1,
    f2_struct* st, glitter_effect* a3, bool use_big_endian) {
    if (!glitter_effect_pack_file(st, a3, use_big_endian))
        return false;

    f2_struct s;
    if (glitter_animation_unparse_file(&s, &a3->curve, use_big_endian))
        vector_f2_struct_push_back(&st->sub_structs, &s);

    for (glitter_emitter** i = a3->emitters.begin; i != a3->emitters.end; i++)
        if (*i && glitter_emitter_unparse_file(a1, &s, *i, a3, use_big_endian))
            vector_f2_struct_push_back(&st->sub_structs, &s);
    return true;
}

void FASTCALL glitter_effect_dispose(glitter_effect* e) {
    free(e->data.a3da);
    vector_ptr_glitter_curve_free(&e->curve, (void*)glitter_curve_dispose);
    vector_ptr_glitter_emitter_free(&e->emitters, (void*)glitter_emitter_dispose);
    free(e);
}

static bool FASTCALL glitter_effect_pack_file(f2_struct* st, glitter_effect* a2, bool use_big_endian) {
    size_t l;
    int32_t* d;
    glitter_effect_a3da* a3da;
    glitter_effect_file_flag flags;

    if (a2->version != 6 && a2->version != 7)
        return false;

    a3da = a2->data.a3da;
    memset(st, 0, sizeof(f2_struct));
    l = 52;

    if (a2->version == 7)
        l += 4;

    if (a2->data.flags & GLITTER_EFFECT_FLAG_LOCAL)
        l += 4;
    else if (a3da)
        if (a3da->flags & GLITTER_EFFECT_A3DA_FLAG_SET_A3DA_BY_OBJECT_INDEX)
            l += 16;
        else
            l += 144;

    l = align_val(l, 0x10);
    d = force_malloc(l);
    st->length = l;
    st->data = d;

    flags = 0;
    if (a2->data.flags & GLITTER_EFFECT_FLAG_ALPHA)
        flags |= GLITTER_EFFECT_FILE_FLAG_ALPHA;

    if (a2->data.flags & GLITTER_EFFECT_FLAG_FOG)
        flags |= GLITTER_EFFECT_FILE_FLAG_FOG;
    else if (a2->data.flags & GLITTER_EFFECT_FLAG_FOG_HEIGHT)
        flags |= GLITTER_EFFECT_FILE_FLAG_FOG_HEIGHT;

    if (a2->data.flags & GLITTER_EFFECT_FLAG_EMISSION)
        flags |= GLITTER_EFFECT_FILE_FLAG_EMISSION;

    if (use_big_endian) {
        *(uint64_t*)d = reverse_endianess_uint64_t(a2->data.name_hash);
        d[2] = reverse_endianess_int32_t((int32_t)roundf(a2->data.appear_time));
        d[3] = reverse_endianess_int32_t((int32_t)roundf(a2->data.life_time));
        d[4] = reverse_endianess_int32_t((int32_t)roundf(a2->data.start_time));
        d[5] = 0xFFFFFFFF;
        d[6] = reverse_endianess_int32_t(a2->data.flags & GLITTER_EFFECT_FLAG_LOOP ? 1 : 0);
        *(float_t*)&d[7] = reverse_endianess_float_t(a2->translation.x);
        *(float_t*)&d[8] = reverse_endianess_float_t(a2->translation.y);
        *(float_t*)&d[9] = reverse_endianess_float_t(a2->translation.z);
        *(float_t*)&d[10] = reverse_endianess_float_t(a2->rotation.x);
        *(float_t*)&d[11] = reverse_endianess_float_t(a2->rotation.y);
        *(float_t*)&d[12] = reverse_endianess_float_t(a2->rotation.z);
        d[13] = reverse_endianess_int32_t(flags);
    }
    else {
        *(uint64_t*)d = a2->data.name_hash;
        d[2] = (int32_t)roundf(a2->data.appear_time);
        d[3] = (int32_t)roundf(a2->data.life_time);
        d[4] = (int32_t)roundf(a2->data.start_time);
        d[5] = 0xFFFFFFFF;
        d[6] = a2->data.flags & GLITTER_EFFECT_FLAG_LOOP ? 1 : 0;
        *(vec3*)&d[7] = a2->translation;
        *(vec3*)&d[10] = a2->rotation;
        d[13] = flags;
    }

    if (a2->version == 7) {
        if (use_big_endian)
            *(float_t*)&d[14] = reverse_endianess_float_t(a2->data.emission);
        else
            *(float_t*)&d[14] = a2->data.emission;
        d++;
    }

    if (a2->data.flags & GLITTER_EFFECT_FLAG_LOCAL)
        d[14] = 1;
    else if (a3da)
        if (a3da->flags & GLITTER_EFFECT_A3DA_FLAG_SET_A3DA_BY_OBJECT_INDEX) {
            d[14] = 2;
            d += 15;
            glitter_effect_a3da_flag a3da_flag = a3da->flags & ~GLITTER_EFFECT_A3DA_FLAG_SET_A3DA_BY_OBJECT_INDEX;
            if (use_big_endian) {
                d[0] = reverse_endianess_int32_t(a3da->object_index);
                d[1] = reverse_endianess_int32_t(a3da_flag);
                d[2] = reverse_endianess_int32_t(a3da->index);
            }
            else {
                d[0] = a3da->object_index;
                d[1] = a3da_flag;
                d[2] = a3da->index;
            }
        }
        else {
            d[14] = 3;
            d += 15;
            if (use_big_endian) {
                *(uint64_t*)d = reverse_endianess_uint64_t(a3da->object_hash);
                d[2] = reverse_endianess_int32_t(a3da->flags);
            }
            else {
                *(uint64_t*)d = a3da->object_hash;
                d[2] = a3da->flags;
                a3da->flags = d[2];
            }
            memcpy((char*)(d + 12), a3da->mesh_name, 128);
        }

    st->header.signature = 0x54434645;
    st->header.length = 0x20;
    st->header.use_big_endian = use_big_endian ? true : false;
    st->header.use_section_size = true;
    st->header.version = a2->version;
    return true;
}

static bool FASTCALL glitter_effect_unpack_file(int32_t* data, glitter_effect* a2, bool use_big_endian) {
    glitter_effect_a3da* a3da;
    glitter_effect_file_flag flags;

    a2->scale = vec3_identity;
    a2->data.start_time = 0.0f;
    a2->data.a3da = 0;
    a2->data.flags = 0;
    a2->data.name_hash = gpm->f2 ? 0x0CAD3078 : 0xCBF29CE44FD0BFC1;
    if (a2->version != 6 && a2->version != 7)
        return false;

    if (use_big_endian) {
        a2->data.name_hash = reverse_endianess_uint64_t(*(uint64_t*)data);
        a2->data.appear_time = (float_t)reverse_endianess_int32_t(data[2]);
        a2->data.life_time = (float_t)reverse_endianess_int32_t(data[3]);
        a2->data.start_time = (float_t)reverse_endianess_int32_t(data[4]);
        if (reverse_endianess_int32_t(data[6]))
            a2->data.flags |= GLITTER_EFFECT_FLAG_LOOP;
        a2->translation.x = reverse_endianess_float_t(*(float_t*)&data[7]);
        a2->translation.y = reverse_endianess_float_t(*(float_t*)&data[8]);
        a2->translation.z = reverse_endianess_float_t(*(float_t*)&data[9]);
        a2->rotation.x = reverse_endianess_float_t(*(float_t*)&data[10]);
        a2->rotation.y = reverse_endianess_float_t(*(float_t*)&data[11]);
        a2->rotation.z = reverse_endianess_float_t(*(float_t*)&data[12]);
        flags = reverse_endianess_int32_t(data[13]);
    }
    else {
        a2->data.name_hash = *(uint64_t*)data;
        a2->data.appear_time = (float_t)data[2];
        a2->data.life_time = (float_t)data[3];
        a2->data.start_time = (float_t)data[4];
        if (data[6])
            a2->data.flags |= GLITTER_EFFECT_FLAG_LOOP;
        a2->translation = *(vec3*)&data[7];
        a2->rotation = *(vec3*)&data[10];
        flags = data[13];
    }

    if (flags & GLITTER_EFFECT_FILE_FLAG_ALPHA)
        a2->data.flags |= GLITTER_EFFECT_FLAG_ALPHA;

    if (flags & GLITTER_EFFECT_FILE_FLAG_FOG)
        a2->data.flags |= GLITTER_EFFECT_FLAG_FOG;
    else if (flags & GLITTER_EFFECT_FILE_FLAG_FOG_HEIGHT)
        a2->data.flags |= GLITTER_EFFECT_FLAG_FOG_HEIGHT;

    if (flags & GLITTER_EFFECT_FILE_FLAG_EMISSION)
        a2->data.flags |= GLITTER_EFFECT_FLAG_EMISSION;

    if (a2->version == 7) {
        if (use_big_endian)
            a2->data.emission = reverse_endianess_float_t(*(float_t*)&data[14]);
        else
            a2->data.emission = *(float_t*)&data[14];
        data++;
    }

    if (data[14] == 1)
        a2->data.flags |= GLITTER_EFFECT_FLAG_LOCAL;
    else if (data[14] == 2) {
        data += 15;
        a3da = force_malloc(sizeof(glitter_effect_a3da));
        a2->data.a3da = a3da;
        if (a3da) {
            if (use_big_endian) {
                a3da->object_index = reverse_endianess_int32_t(data[0]);
                a3da->flags = reverse_endianess_int32_t(data[1]);
                a3da->index = reverse_endianess_int32_t(data[2]);
            }
            else {
                a3da->object_index = data[0];
                a3da->flags = data[1];
                a3da->index = data[2];
            }
            a3da->flags |= GLITTER_EFFECT_A3DA_FLAG_SET_A3DA_BY_OBJECT_INDEX;
            a3da->mesh_name[0] = 0;
        }
    }
    else if (data[14] == 3) {
        data += 15;
        //v10 = sub_14045A570(*(uint64_t*)data);
        //if (v10 != -1) {
            a3da = force_malloc(sizeof(glitter_effect_a3da));
            a2->data.a3da = a3da;
            if (a3da) {
                //a3da->object_index = v10;
                if (use_big_endian) {
                    a3da->object_hash = reverse_endianess_uint64_t(*(uint64_t*)data);
                    a3da->flags = reverse_endianess_int32_t(data[2]);
                }
                else {
                    a3da->object_hash = *(uint64_t*)data;
                    a3da->flags = data[2];
                }
                a3da->index = 201;
                memcpy(a3da->mesh_name, (char*)(data + 12), 128);
            }
        //}
    }
    return true;
}
