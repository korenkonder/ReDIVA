/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "particle.h"
#include "animation.h"
#include "curve.h"

static bool FASTCALL glitter_particle_pack_file(GLT, glitter_effect_group* a1,
    f2_struct* st, glitter_particle* a3, glitter_effect* effect);
static bool FASTCALL glitter_particle_unpack_file(GLT, glitter_effect_group* a1,
    void* data, glitter_particle* a3, glitter_effect* effect, bool use_big_endian);

glitter_particle* FASTCALL glitter_particle_init(GLT) {
    glitter_particle* p = force_malloc(sizeof(glitter_particle));
    p->version = GLT_VAL == GLITTER_X ? 0x05 : 0x03;
    return p;
}

glitter_particle* FASTCALL glitter_particle_copy(GLT, glitter_particle* p) {
    if (!p)
        return 0;

    glitter_particle* pc = force_malloc(sizeof(glitter_particle));
    *pc = *p;

    pc->animation = (glitter_animation){ 0, 0, 0 };
    glitter_animation_copy(GLT_VAL, &p->animation, &pc->animation);
    return pc;
}

bool FASTCALL glitter_particle_parse_file(glitter_effect_group* a1,
    f2_struct* st, vector_ptr_glitter_particle* vec, glitter_effect* effect) {
    f2_struct* i;
    glitter_particle* particle;

    if (!st || !st->header.data_size)
        return false;

    particle = glitter_particle_init(a1->type);
    particle->version = st->header.version;
    if (!glitter_particle_unpack_file(a1->type, a1, st->data, particle, effect, st->header.use_big_endian)) {
        glitter_particle_dispose(particle);
        return false;
    }

    for (i = st->sub_structs.begin; i != st->sub_structs.end; i++) {
        if (!i->header.data_size)
            continue;

        if (i->header.signature == reverse_endianess_uint32_t('ANIM')) {
            glitter_curve_type_flags flags = 0;
            if (a1->type == GLITTER_X)
                flags = glitter_particle_x_curve_flags;
            else
                flags = glitter_particle_curve_flags;

            if (particle->data.type != GLITTER_PARTICLE_MESH) {
                flags &= ~GLITTER_CURVE_TYPE_UV_SCROLL_2ND;
                if (particle->data.draw_type != GLITTER_DIRECTION_PARTICLE_ROTATION)
                    flags &= ~(GLITTER_CURVE_TYPE_ROTATION_X | GLITTER_CURVE_TYPE_ROTATION_Y);
            }
            glitter_animation_parse_file(a1->type, i, &particle->animation, flags);
        }
    }

    vector_ptr_glitter_particle_push_back(vec, &particle);
    return true;
}

bool FASTCALL glitter_particle_unparse_file(GLT, glitter_effect_group* a1,
    f2_struct* st, glitter_particle* a3, glitter_effect* effect) {
    if (!glitter_particle_pack_file(GLT_VAL, a1, st, a3, effect))
        return false;

    glitter_curve_type_flags flags = 0;
    if (a1->type == GLITTER_X)
        flags = glitter_particle_x_curve_flags;
    else
        flags = glitter_particle_curve_flags;

    if (a3->data.type != GLITTER_PARTICLE_MESH) {
        flags &= ~GLITTER_CURVE_TYPE_UV_SCROLL_2ND;
        if (a3->data.draw_type != GLITTER_DIRECTION_PARTICLE_ROTATION)
            flags &= ~(GLITTER_CURVE_TYPE_ROTATION_X | GLITTER_CURVE_TYPE_ROTATION_Y);
    }

    f2_struct s;
    if (glitter_animation_unparse_file(GLT_VAL, &s, &a3->animation, flags))
        vector_f2_struct_push_back(&st->sub_structs, &s);
    return true;
}

void FASTCALL glitter_particle_dispose(glitter_particle* p) {
    glitter_animation_free(&p->animation);
    free(p);
}

static bool FASTCALL glitter_particle_pack_file(GLT,
    glitter_effect_group* a1, f2_struct* st, glitter_particle* a3, glitter_effect* effect) {
    size_t l;
    size_t d;
    glitter_particle_flag flags;

    if (a3->version < 2)
        return false;

    memset(st, 0, sizeof(f2_struct));
    l = 0;

    uint32_t o;
    vector_enrs_entry e = { 0, 0, 0 };
    enrs_entry ee;

    ee = (enrs_entry){ 0, 1, 204, 1, { 0, 0, 0 } };
    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 51, ENRS_TYPE_DWORD });
    vector_enrs_entry_push_back(&e, &ee);
    l += o = 204;

    if (a3->version == 3) {
        ee = (enrs_entry){ o, 1, 8, 1, { 0, 0, 0 } };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_TYPE_DWORD });
        vector_enrs_entry_push_back(&e, &ee);
        l += o = 8;
    }

    if (a3->data.type == GLITTER_PARTICLE_LOCUS || a3->data.type == GLITTER_PARTICLE_MESH) {
        ee = (enrs_entry){ o, 1, 4, 1, { 0, 0, 0 } };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_TYPE_WORD });
        vector_enrs_entry_push_back(&e, &ee);
        l += o = 4;
    }

    ee = (enrs_entry){ o, 4, 44, 1, { 0, 0, 0 } };
    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_TYPE_QWORD });
    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 4, 5, ENRS_TYPE_DWORD });
    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_TYPE_WORD });
    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_TYPE_DWORD });
    if (a1->version >= 7) {
        ee.count++;
        ee.size += 4;
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_TYPE_DWORD });
        vector_enrs_entry_push_back(&e, &ee);
        l += o = 48;
    }
    else {
        vector_enrs_entry_push_back(&e, &ee);
        l += o = 44;
    }

    ee = (enrs_entry){ o, 2, 12, 1, { 0, 0, 0 } };
    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_TYPE_QWORD });
    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_TYPE_DWORD });
    vector_enrs_entry_push_back(&e, &ee);
    l += o = 12;

    l = align_val(l, 0x10);
    d = (size_t)force_malloc(l);
    st->length = l;
    st->data = (void*)d;
    st->enrs = e;

    flags = a3->data.flags;
    if (effect->data.flags & GLITTER_EFFECT_LOCAL)
        flags &= ~GLITTER_PARTICLE_LOCAL;
    if (effect->data.flags & GLITTER_EFFECT_EMISSION)
        flags &= ~GLITTER_PARTICLE_EMISSION;

    *(int32_t*)d = a3->data.life_time;
    *(int32_t*)(d + 4) = a3->data.type;
    *(int32_t*)(d + 8) = a3->data.draw_type;
    *(vec3*)(d + 12) = a3->data.rotation;
    *(vec3*)(d + 24) = a3->data.rotation_random;
    *(vec3*)(d + 36) = a3->data.rotation_add;
    *(vec3*)(d + 48) = a3->data.rotation_add_random;
    *(vec3*)(d + 60) = a3->data.scale;
    *(vec3*)(d + 72) = a3->data.scale_random;
    *(float_t*)(d + 84) = a3->data.z_offset;
    *(int32_t*)(d + 88) = a3->data.pivot;
    *(int32_t*)(d + 92) = flags;
    *(float_t*)(d + 96) = a3->data.speed;
    *(float_t*)(d + 100) = a3->data.speed_random;
    *(float_t*)(d + 104) = a3->data.deceleration;
    *(float_t*)(d + 108) = a3->data.deceleration_random;
    *(vec3*)(d + 112) = a3->data.direction;
    *(vec3*)(d + 124) = a3->data.direction_random;
    *(vec3*)(d + 136) = a3->data.gravity;
    *(vec3*)(d + 148) = a3->data.acceleration;
    *(vec3*)(d + 160) = a3->data.acceleration_random;
    *(float_t*)(d + 172) = a3->data.reflection_coeff;
    *(float_t*)(d + 176) = a3->data.reflection_coeff_random;
    *(float_t*)(d + 180) = a3->data.rebound_plane_y;
    *(vec2*)(d + 184) = a3->data.uv_scroll_add;
    *(float_t*)(d + 192) = a3->data.uv_scroll_add_scale;
    *(int32_t*)(d + 196) = a3->data.sub_flags;
    *(int32_t*)(d + 200) = a3->data.count;
    d += 204;

    if (a3->version == 3) {
        *(int32_t*)d = 0;
        *(float_t*)(d + 4) = a3->data.emission;
        d += 8;
    }

    if (a3->data.type == GLITTER_PARTICLE_LOCUS || a3->data.type == GLITTER_PARTICLE_MESH) {
        *(uint16_t*)d = a3->data.locus_history_size;
        *(uint16_t*)(d + 2) = a3->data.locus_history_size_random;
        d += 4;
    }

    *(uint64_t*)d = a3->data.tex_hash0;
    *(uint8_t*)(d + 8) = (uint8_t)roundf(clamp(a3->data.color.x, 0.0f, 1.0f) * 255.0f);
    *(uint8_t*)(d + 9) = (uint8_t)roundf(clamp(a3->data.color.y, 0.0f, 1.0f) * 255.0f);
    *(uint8_t*)(d + 10) = (uint8_t)roundf(clamp(a3->data.color.z, 0.0f, 1.0f) * 255.0f);
    *(uint8_t*)(d + 11) = (uint8_t)roundf(clamp(a3->data.color.w, 0.0f, 1.0f) * 255.0f);
    *(int32_t*)(d + 12) = a3->data.blend_mode0;
    *(int32_t*)(d + 16) = a3->data.unk0;
    *(int32_t*)(d + 20) = a3->data.split_u;
    *(int32_t*)(d + 24) = a3->data.split_v;
    *(int32_t*)(d + 28) = a3->data.uv_index_type;
    *(int16_t*)(d + 32) = a3->data.uv_index;
    *(int16_t*)(d + 34) = (int16_t)a3->data.frame_step_uv;
    *(int32_t*)(d + 36) = a3->data.uv_index_start;
    *(int32_t*)(d + 40) = a3->data.uv_index_end;
    if (a1->version >= 7) {
        *(int32_t*)(d + 44) = a3->data.unk1;
        d += 48;
    }
    else
        d += 44;

    if (a3->data.flags & GLITTER_PARTICLE_SECOND_TEXTURE) {
        *(uint64_t*)d = a3->data.tex_hash1;
        *(int32_t*)(d + 8) = a3->data.blend_mode1;
        d += 12;
    }
    else {
        *(uint64_t*)d = 0;
        *(int32_t*)(d + 8) = 0;
        d += 12;
    }

    st->header.signature = reverse_endianess_uint32_t('PTCL');
    st->header.length = 0x20;
    st->header.use_big_endian = false;
    st->header.use_section_size = true;
    st->header.version = a3->version;
    return true;
}

static bool FASTCALL glitter_particle_unpack_file(GLT, glitter_effect_group* a1,
    void* data, glitter_particle* a3, glitter_effect* effect, bool use_big_endian) {
    uint8_t r;
    uint8_t b;
    uint8_t g;
    uint8_t a;
    glitter_particle_blend blend_mode0;
    glitter_particle_blend blend_mode1;
    uint64_t tex_hash0;
    uint64_t tex_hash1;
    int32_t frame_step_uv;
    int32_t uv_index_start;
    int32_t uv_index;
    int32_t uv_index_end;
    int32_t uv_index_type;
    int32_t uv_max_count;
    uint8_t split_u;
    uint8_t split_v;
    int32_t unk0;
    int32_t unk1;
    size_t d;
    bool has_tex;

    if (GLT_VAL == GLITTER_X) {
        a3->data.mesh.object_name_hash = hash_murmurhash_empty;
        a3->data.mesh.object_file_hash = hash_murmurhash_empty;
        a3->data.mesh.object_mesh_name[0] = 0;
        a3->data.mesh.some_hash = hash_murmurhash_empty;

        d = (size_t)data;
        if (use_big_endian) {
            a3->data.life_time = reverse_endianess_int32_t(*(int32_t*)d);
            a3->data.life_time_random = reverse_endianess_int32_t(*(int32_t*)(d + 4));
            a3->data.fade_in = reverse_endianess_int32_t(*(int32_t*)(d + 8));
            a3->data.fade_in_random = reverse_endianess_int32_t(*(int32_t*)(d + 12));
            a3->data.fade_out = reverse_endianess_int32_t(*(int32_t*)(d + 16));
            a3->data.fade_out_random = reverse_endianess_int32_t(*(int32_t*)(d + 20));
            a3->data.type = reverse_endianess_int32_t(*(int32_t*)(d + 24));
            a3->data.draw_type = reverse_endianess_int32_t(*(int32_t*)(d + 28));
        }
        else {
            a3->data.life_time = *(int32_t*)d;
            a3->data.life_time_random = *(int32_t*)(d + 4);
            a3->data.fade_in = *(int32_t*)(d + 8);
            a3->data.fade_in_random = *(int32_t*)(d + 12);
            a3->data.fade_out = *(int32_t*)(d + 16);
            a3->data.fade_out_random = *(int32_t*)(d + 20);
            a3->data.type = *(int32_t*)(d + 24);
            a3->data.draw_type = *(int32_t*)(d + 28);
        }

        if (a3->version != 4 && a3->version != 5)
            return false;

        if (use_big_endian) {
            a3->data.rotation.x = reverse_endianess_float_t(*(float_t*)(d + 32));
            a3->data.rotation.y = reverse_endianess_float_t(*(float_t*)(d + 36));
            a3->data.rotation.z = reverse_endianess_float_t(*(float_t*)(d + 40));
            a3->data.rotation_random.x = reverse_endianess_float_t(*(float_t*)(d + 44));
            a3->data.rotation_random.y = reverse_endianess_float_t(*(float_t*)(d + 48));
            a3->data.rotation_random.z = reverse_endianess_float_t(*(float_t*)(d + 52));
            a3->data.rotation_add.x = reverse_endianess_float_t(*(float_t*)(d + 56));
            a3->data.rotation_add.y = reverse_endianess_float_t(*(float_t*)(d + 60));
            a3->data.rotation_add.z = reverse_endianess_float_t(*(float_t*)(d + 64));
            a3->data.rotation_add_random.x = reverse_endianess_float_t(*(float_t*)(d + 68));
            a3->data.rotation_add_random.y = reverse_endianess_float_t(*(float_t*)(d + 72));
            a3->data.rotation_add_random.z = reverse_endianess_float_t(*(float_t*)(d + 76));
            a3->data.scale.x = reverse_endianess_float_t(*(float_t*)(d + 80));
            a3->data.scale.y = reverse_endianess_float_t(*(float_t*)(d + 84));
            a3->data.scale.z = reverse_endianess_float_t(*(float_t*)(d + 88));
            a3->data.scale_random.x = reverse_endianess_float_t(*(float_t*)(d + 92));
            a3->data.scale_random.y = reverse_endianess_float_t(*(float_t*)(d + 96));
            a3->data.scale_random.z = reverse_endianess_float_t(*(float_t*)(d + 100));
            a3->data.z_offset = reverse_endianess_float_t(*(float_t*)(d + 104));
            a3->data.pivot = reverse_endianess_int32_t(*(int32_t*)(d + 108));
            a3->data.flags = reverse_endianess_int32_t(*(int32_t*)(d + 112));
        }
        else {
            a3->data.rotation = *(vec3*)(d + 32);
            a3->data.rotation_random = *(vec3*)(d + 44);
            a3->data.rotation_add = *(vec3*)(d + 56);
            a3->data.rotation_add_random = *(vec3*)(d + 68);
            a3->data.scale = *(vec3*)(d + 80);
            a3->data.scale_random = *(vec3*)(d + 92);
            a3->data.z_offset = *(float_t*)(d + 104);
            a3->data.pivot = *(int32_t*)(d + 108);
            a3->data.flags = *(int32_t*)(d + 112);
        }

        if (effect->data.flags & GLITTER_EFFECT_LOCAL)
            a3->data.flags |= GLITTER_PARTICLE_LOCAL;
        if (effect->data.flags & GLITTER_EFFECT_EMISSION)
            a3->data.flags |= GLITTER_PARTICLE_EMISSION;

        if (use_big_endian) {
            a3->data.uv_scroll_2nd_add.x = reverse_endianess_float_t(*(float_t*)(d + 120));
            a3->data.uv_scroll_2nd_add.y = reverse_endianess_float_t(*(float_t*)(d + 124));
            a3->data.uv_scroll_2nd_add_scale = reverse_endianess_float_t(*(float_t*)(d + 128));
            a3->data.speed = reverse_endianess_float_t(*(float_t*)(d + 136));
            a3->data.speed_random = reverse_endianess_float_t(*(float_t*)(d + 140));
            a3->data.deceleration = reverse_endianess_float_t(*(float_t*)(d + 144));
            a3->data.deceleration_random = reverse_endianess_float_t(*(float_t*)(d + 148));
            a3->data.direction.x = reverse_endianess_float_t(*(float_t*)(d + 152));
            a3->data.direction.y = reverse_endianess_float_t(*(float_t*)(d + 156));
            a3->data.direction.z = reverse_endianess_float_t(*(float_t*)(d + 160));
            a3->data.direction_random.x = reverse_endianess_float_t(*(float_t*)(d + 164));
            a3->data.direction_random.y = reverse_endianess_float_t(*(float_t*)(d + 168));
            a3->data.direction_random.z = reverse_endianess_float_t(*(float_t*)(d + 172));
            a3->data.gravity.x = reverse_endianess_float_t(*(float_t*)(d + 176));
            a3->data.gravity.y = reverse_endianess_float_t(*(float_t*)(d + 180));
            a3->data.gravity.z = reverse_endianess_float_t(*(float_t*)(d + 184));
            a3->data.acceleration.x = reverse_endianess_float_t(*(float_t*)(d + 188));
            a3->data.acceleration.y = reverse_endianess_float_t(*(float_t*)(d + 192));
            a3->data.acceleration.z = reverse_endianess_float_t(*(float_t*)(d + 196));
            a3->data.acceleration_random.x = reverse_endianess_float_t(*(float_t*)(d + 200));
            a3->data.acceleration_random.y = reverse_endianess_float_t(*(float_t*)(d + 204));
            a3->data.acceleration_random.z = reverse_endianess_float_t(*(float_t*)(d + 208));
            a3->data.reflection_coeff = reverse_endianess_float_t(*(float_t*)(d + 212));
            a3->data.reflection_coeff_random = reverse_endianess_float_t(*(float_t*)(d + 216));
            a3->data.rebound_plane_y = reverse_endianess_float_t(*(float_t*)(d + 220));
            a3->data.uv_scroll_add.x = reverse_endianess_float_t(*(float_t*)(d + 224));
            a3->data.uv_scroll_add.y = reverse_endianess_float_t(*(float_t*)(d + 228));
            a3->data.uv_scroll_add_scale = reverse_endianess_float_t(*(float_t*)(d + 232));
            a3->data.sub_flags = reverse_endianess_int32_t(*(int32_t*)(d + 236));
            a3->data.count = reverse_endianess_int32_t(*(int32_t*)(d + 240));
            a3->data.draw_flags = reverse_endianess_int32_t(*(int32_t*)(d + 244));
            a3->data.emission = reverse_endianess_float_t(*(float_t*)(d + 252));
        }
        else {
            a3->data.uv_scroll_2nd_add.x = *(float_t*)(d + 120);
            a3->data.uv_scroll_2nd_add.y = *(float_t*)(d + 124);
            a3->data.uv_scroll_2nd_add_scale = *(float_t*)(d + 128);
            a3->data.speed = *(float_t*)(d + 136);
            a3->data.speed_random = *(float_t*)(d + 140);
            a3->data.deceleration = *(float_t*)(d + 144);
            a3->data.deceleration_random = *(float_t*)(d + 148);
            a3->data.direction = *(vec3*)(d + 152);
            a3->data.direction_random = *(vec3*)(d + 164);
            a3->data.gravity = *(vec3*)(d + 176);
            a3->data.acceleration = *(vec3*)(d + 188);
            a3->data.acceleration_random = *(vec3*)(d + 200);
            a3->data.reflection_coeff = *(float_t*)(d + 212);
            a3->data.reflection_coeff_random = *(float_t*)(d + 216);
            a3->data.rebound_plane_y = *(float_t*)(d + 220);
            a3->data.uv_scroll_add = *(vec2*)(d + 224);
            a3->data.uv_scroll_add_scale = *(float_t*)(d + 232);
            a3->data.sub_flags = *(int32_t*)(d + 236);
            a3->data.count = *(int32_t*)(d + 240);
            a3->data.draw_flags = *(int32_t*)(d + 244);
            a3->data.emission = *(float_t*)(d + 252);
        }
        d += 256;

        if (a3->version == 4) {
            vec3_mult_scalar(a3->data.direction, 10.0f, a3->data.direction);
            vec3_mult_scalar(a3->data.direction_random, 10.0f, a3->data.direction_random);
        }

        if (a3->data.emission >= glitter_min_emission)
            a3->data.flags |= GLITTER_PARTICLE_EMISSION;

        a3->data.locus_history_size = 0;
        a3->data.locus_history_size_random = 0;

        if (a3->data.type == GLITTER_PARTICLE_MESH)
            a3->data.type = GLITTER_PARTICLE_LINE;
        else if (a3->data.type == GLITTER_PARTICLE_LINE)
            a3->data.type = GLITTER_PARTICLE_MESH;

        if (a3->data.type == GLITTER_PARTICLE_QUAD)
            has_tex = true;
        else if (a3->data.type == GLITTER_PARTICLE_LINE)
            has_tex = false;
        else if (a3->data.type == GLITTER_PARTICLE_LOCUS) {
            if (use_big_endian) {
                a3->data.locus_history_size = reverse_endianess_uint16_t(*(uint16_t*)(d + 0));
                a3->data.locus_history_size_random = reverse_endianess_uint16_t(*(uint16_t*)(d + 2));
            }
            else {
                a3->data.locus_history_size = *(uint16_t*)(d + 0);
                a3->data.locus_history_size_random = *(uint16_t*)(d + 2);
            }
            d += 4;
            has_tex = true;
        }
        else if (a3->data.type == GLITTER_PARTICLE_MESH) {
            a3->data.mesh.object_name_hash = *(uint64_t*)d;
            a3->data.mesh.object_file_hash = *(uint64_t*)(d + 8);
            memcpy(a3->data.mesh.object_mesh_name, (void*)(d + 16), 0x40);
            a3->data.mesh.some_hash = *(uint64_t*)(d + 80);
            d += 88;
            has_tex = false;
        }
        else
            return false;

        a3->data.texture0 = 0;
        a3->data.texture1 = 0;

        tex_hash0 = hash_murmurhash_empty;
        tex_hash1 = hash_murmurhash_empty;
        unk0 = 0;
        unk1 = 0;

        if (use_big_endian) {
            if (has_tex)
                tex_hash0 = reverse_endianess_uint64_t(*(uint64_t*)d);
            r = *(uint8_t*)(d + 8);
            g = *(uint8_t*)(d + 9);
            b = *(uint8_t*)(d + 10);
            a = *(uint8_t*)(d + 11);
            blend_mode0 = reverse_endianess_int32_t(*(int32_t*)(d + 12));
            split_u = (uint8_t)reverse_endianess_int32_t(*(int32_t*)(d + 20));
            split_v = (uint8_t)reverse_endianess_int32_t(*(int32_t*)(d + 24));
            uv_index_type = reverse_endianess_int32_t(*(int32_t*)(d + 28));
            uv_index = reverse_endianess_int16_t(*(int16_t*)(d + 32));
            frame_step_uv = reverse_endianess_int16_t(*(int16_t*)(d + 34));
            uv_index_start = reverse_endianess_int32_t(*(int32_t*)(d + 36));
            uv_index_end = reverse_endianess_int32_t(*(int32_t*)(d + 40));
            unk1 = reverse_endianess_int32_t(*(int32_t*)(d + 44));
        }
        else {
            if (has_tex)
                tex_hash0 = *(uint64_t*)d;
            r = *(uint8_t*)(d + 8);
            g = *(uint8_t*)(d + 9);
            b = *(uint8_t*)(d + 10);
            a = *(uint8_t*)(d + 11);
            blend_mode0 = *(int32_t*)(d + 12);
            split_u = (uint8_t) * (int32_t*)(d + 20);
            split_v = (uint8_t) * (int32_t*)(d + 24);
            uv_index_type = *(int32_t*)(d + 28);
            uv_index = *(int16_t*)(d + 32);
            frame_step_uv = *(int16_t*)(d + 34);
            uv_index_start = *(int32_t*)(d + 36);
            uv_index_end = *(int32_t*)(d + 40);
            unk1 = *(int32_t*)(d + 44);
        }
        d += 48;

        if (a3->data.flags & GLITTER_PARTICLE_SECOND_TEXTURE)
            if (use_big_endian) {
                if (has_tex)
                    tex_hash1 = reverse_endianess_uint64_t(*(uint64_t*)d);
                blend_mode1 = reverse_endianess_int32_t(*(int32_t*)(d + 8));
            }
            else {
                if (has_tex)
                    tex_hash1 = *(uint64_t*)d;
                blend_mode1 = *(int32_t*)(d + 8);
            }
        else
            blend_mode1 = GLITTER_PARTICLE_BLEND_TYPICAL;
    }
    else {
        d = (size_t)data;
        if (use_big_endian) {
            a3->data.life_time = reverse_endianess_int32_t(*(int32_t*)d);
            a3->data.type = reverse_endianess_int32_t(*(int32_t*)(d + 4));
            a3->data.draw_type = reverse_endianess_int32_t(*(int32_t*)(d + 8));
        }
        else {
            a3->data.life_time =  *(int32_t*)d;
            a3->data.type = *(int32_t*)(d + 4);
            a3->data.draw_type = *(int32_t*)(d + 8);
        }

        if (a3->version != 2 && a3->version != 3)
            return false;

        if (use_big_endian) {
            a3->data.rotation.x = reverse_endianess_float_t(*(float_t*)(d + 12));
            a3->data.rotation.y = reverse_endianess_float_t(*(float_t*)(d + 16));
            a3->data.rotation.z = reverse_endianess_float_t(*(float_t*)(d + 20));
            a3->data.rotation_random.x = reverse_endianess_float_t(*(float_t*)(d + 24));
            a3->data.rotation_random.y = reverse_endianess_float_t(*(float_t*)(d + 28));
            a3->data.rotation_random.z = reverse_endianess_float_t(*(float_t*)(d + 32));
            a3->data.rotation_add.x = reverse_endianess_float_t(*(float_t*)(d + 36));
            a3->data.rotation_add.y = reverse_endianess_float_t(*(float_t*)(d + 40));
            a3->data.rotation_add.z = reverse_endianess_float_t(*(float_t*)(d + 44));
            a3->data.rotation_add_random.x = reverse_endianess_float_t(*(float_t*)(d + 48));
            a3->data.rotation_add_random.y = reverse_endianess_float_t(*(float_t*)(d + 52));
            a3->data.rotation_add_random.z = reverse_endianess_float_t(*(float_t*)(d + 56));
            a3->data.scale.x = reverse_endianess_float_t(*(float_t*)(d + 60));
            a3->data.scale.y = reverse_endianess_float_t(*(float_t*)(d + 64));
            a3->data.scale.z = reverse_endianess_float_t(*(float_t*)(d + 68));
            a3->data.scale_random.x = reverse_endianess_float_t(*(float_t*)(d + 72));
            a3->data.scale_random.y = reverse_endianess_float_t(*(float_t*)(d + 76));
            a3->data.scale_random.z = reverse_endianess_float_t(*(float_t*)(d + 80));
            a3->data.z_offset = reverse_endianess_float_t(*(float_t*)(d + 84));
            a3->data.pivot = reverse_endianess_int32_t(*(int32_t*)(d + 88));
            a3->data.flags = reverse_endianess_int32_t(*(int32_t*)(d + 92));
        }
        else {
            a3->data.rotation = *(vec3*)(d + 12);
            a3->data.rotation_random = *(vec3*)(d + 24);
            a3->data.rotation_add = *(vec3*)(d + 36);
            a3->data.rotation_add_random = *(vec3*)(d + 48);
            a3->data.scale = *(vec3*)(d + 60);
            a3->data.scale_random = *(vec3*)(d + 72);
            a3->data.z_offset = *(float_t*)(d + 84);
            a3->data.pivot = *(int32_t*)(d + 88);
            a3->data.flags = *(int32_t*)(d + 92);
        }

        if (effect->data.flags & GLITTER_EFFECT_LOCAL)
            a3->data.flags |= GLITTER_PARTICLE_LOCAL;
        if (effect->data.flags & GLITTER_EFFECT_EMISSION)
            a3->data.flags |= GLITTER_PARTICLE_EMISSION;

        if (use_big_endian) {
            a3->data.speed = reverse_endianess_float_t(*(float_t*)(d + 96));
            a3->data.speed_random = reverse_endianess_float_t(*(float_t*)(d + 100));
            a3->data.deceleration = reverse_endianess_float_t(*(float_t*)(d + 104));
            a3->data.deceleration_random = reverse_endianess_float_t(*(float_t*)(d + 108));
            a3->data.direction.x = reverse_endianess_float_t(*(float_t*)(d + 112));
            a3->data.direction.y = reverse_endianess_float_t(*(float_t*)(d + 116));
            a3->data.direction.z = reverse_endianess_float_t(*(float_t*)(d + 120));
            a3->data.direction_random.x = reverse_endianess_float_t(*(float_t*)(d + 124));
            a3->data.direction_random.y = reverse_endianess_float_t(*(float_t*)(d + 128));
            a3->data.direction_random.z = reverse_endianess_float_t(*(float_t*)(d + 132));
            a3->data.gravity.x = reverse_endianess_float_t(*(float_t*)(d + 136));
            a3->data.gravity.y = reverse_endianess_float_t(*(float_t*)(d + 140));
            a3->data.gravity.z = reverse_endianess_float_t(*(float_t*)(d + 144));
            a3->data.acceleration.x = reverse_endianess_float_t(*(float_t*)(d + 148));
            a3->data.acceleration.y = reverse_endianess_float_t(*(float_t*)(d + 152));
            a3->data.acceleration.z = reverse_endianess_float_t(*(float_t*)(d + 156));
            a3->data.acceleration_random.x = reverse_endianess_float_t(*(float_t*)(d + 160));
            a3->data.acceleration_random.y = reverse_endianess_float_t(*(float_t*)(d + 164));
            a3->data.acceleration_random.z = reverse_endianess_float_t(*(float_t*)(d + 168));
            a3->data.reflection_coeff = reverse_endianess_float_t(*(float_t*)(d + 172));
            a3->data.reflection_coeff_random = reverse_endianess_float_t(*(float_t*)(d + 176));
            a3->data.rebound_plane_y = reverse_endianess_float_t(*(float_t*)(d + 180));
            a3->data.uv_scroll_add.x = reverse_endianess_float_t(*(float_t*)(d + 184));
            a3->data.uv_scroll_add.y = reverse_endianess_float_t(*(float_t*)(d + 188));
            a3->data.uv_scroll_add_scale = reverse_endianess_float_t(*(float_t*)(d + 192));
            a3->data.sub_flags = reverse_endianess_int32_t(*(int32_t*)(d + 196));
            a3->data.count = reverse_endianess_int32_t(*(int32_t*)(d + 200));
        }
        else {
            a3->data.speed = *(float_t*)(d + 96);
            a3->data.speed_random = *(float_t*)(d + 100);
            a3->data.deceleration = *(float_t*)(d + 104);
            a3->data.deceleration_random = *(float_t*)(d + 108);
            a3->data.direction = *(vec3*)(d + 112);
            a3->data.direction_random = *(vec3*)(d + 124);
            a3->data.gravity = *(vec3*)(d + 136);
            a3->data.acceleration = *(vec3*)(d + 148);
            a3->data.acceleration_random = *(vec3*)(d + 160);
            a3->data.reflection_coeff = *(float_t*)(d + 172);
            a3->data.reflection_coeff_random = *(float_t*)(d + 176);
            a3->data.rebound_plane_y = *(float_t*)(d + 180);
            a3->data.uv_scroll_add = *(vec2*)(d + 184);
            a3->data.uv_scroll_add_scale = *(float_t*)(d + 192);
            a3->data.sub_flags = *(int32_t*)(d + 196);
            a3->data.count = *(int32_t*)(d + 200);
        }
        d += 204;

        a3->data.locus_history_size = 0;
        a3->data.locus_history_size_random = 0;
        a3->data.draw_flags = 0;
        a3->data.emission = 0;

        if (a3->version == 3) {
            if (use_big_endian)
                a3->data.emission = reverse_endianess_float_t(*(float_t*)(d + 4));
            else
                a3->data.emission = *(float_t*)(d + 4);

            if (a3->data.emission >= glitter_min_emission)
                a3->data.flags |= GLITTER_PARTICLE_EMISSION;
            d += 8;
        }

        if (a3->data.type == GLITTER_PARTICLE_LOCUS || a3->data.type == GLITTER_PARTICLE_MESH) {
            if (use_big_endian) {
                a3->data.locus_history_size = reverse_endianess_uint16_t(*(uint16_t*)d);
                a3->data.locus_history_size_random = reverse_endianess_uint16_t(*(uint16_t*)(d + 2));
            }
            else {
                a3->data.locus_history_size = *(uint16_t*)d;
                a3->data.locus_history_size_random = *(uint16_t*)(d + 2);
            }
            d += 4;
        }

        a3->data.texture0 = 0;
        a3->data.texture1 = 0;

        unk0 = 0;
        unk1 = 0;

        if (use_big_endian) {
            tex_hash0 = reverse_endianess_uint64_t(*(uint64_t*)d);
            r = *(uint8_t*)(d + 8);
            g = *(uint8_t*)(d + 9);
            b = *(uint8_t*)(d + 10);
            a = *(uint8_t*)(d + 11);
            blend_mode0 = reverse_endianess_int32_t(*(int32_t*)(d + 12));
            unk0 = reverse_endianess_int32_t(*(int32_t*)(d + 16));
            split_u = (uint8_t)reverse_endianess_int32_t(*(int32_t*)(d + 20));
            split_v = (uint8_t)reverse_endianess_int32_t(*(int32_t*)(d + 24));
            uv_index_type = reverse_endianess_int32_t(*(int32_t*)(d + 28));
            uv_index = reverse_endianess_int16_t(*(int16_t*)(d + 32));
            frame_step_uv = reverse_endianess_int16_t(*(int16_t*)(d + 34));
            uv_index_start = reverse_endianess_int32_t(*(int32_t*)(d + 36));
            uv_index_end = reverse_endianess_int32_t(*(int32_t*)(d + 40));
        }
        else {
            tex_hash0 = *(uint64_t*)d;
            r = *(uint8_t*)(d + 8);
            g = *(uint8_t*)(d + 9);
            b = *(uint8_t*)(d + 10);
            a = *(uint8_t*)(d + 11);
            blend_mode0 = *(int32_t*)(d + 12);
            unk0 = *(int32_t*)(d + 16);
            split_u = (uint8_t) * (int32_t*)(d + 20);
            split_v = (uint8_t) * (int32_t*)(d + 24);
            uv_index_type = *(int32_t*)(d + 28);
            uv_index = *(int16_t*)(d + 32);
            frame_step_uv = *(int16_t*)(d + 34);
            uv_index_start = *(int32_t*)(d + 36);
            uv_index_end = *(int32_t*)(d + 40);
        }

        if (a1->version >= 7) {
            unk1 = *(int32_t*)(d + 44);
            if (use_big_endian)
                unk1 = reverse_endianess_int32_t(unk1);
            d += 48;
        }
        else
            d += 44;

        if (unk0) {
            char buf[0x40];
            sprintf_s(buf, 0x40, "A %X %016llX\n", unk0, effect->data.name_hash);
            OutputDebugStringA(buf);
        }

        if (unk1) {
            char buf[0x40];
            sprintf_s(buf, 0x40, "B %X %016llX\n", unk1, effect->data.name_hash);
            OutputDebugStringA(buf);
        }

        if (a3->data.flags & GLITTER_PARTICLE_SECOND_TEXTURE)
            if (use_big_endian) {
                tex_hash1 = reverse_endianess_uint64_t(*(uint64_t*)d);
                blend_mode1 = reverse_endianess_int32_t(*(int32_t*)(d + 8));
            }
            else {
                tex_hash1 = *(uint64_t*)d;
                blend_mode1 = *(int32_t*)(d + 8);
            }
        else {
            tex_hash1 = glt_type != GLITTER_AFT
                ? hash_murmurhash_empty : hash_fnv1a64_empty;
            blend_mode1 = GLITTER_PARTICLE_BLEND_TYPICAL;
        }
    }

    a3->data.tex_hash0 = tex_hash0;
    a3->data.tex_hash1 = tex_hash1;
    a3->data.color.x = (float_t)r;
    a3->data.color.y = (float_t)g;
    a3->data.color.z = (float_t)b;
    a3->data.color.w = (float_t)a;
    a3->data.blend_mode0 = blend_mode0;
    a3->data.blend_mode1 = blend_mode1;
    a3->data.split_u = split_u;
    a3->data.split_v = split_v;
    a3->data.uv_index_type = uv_index_type;
    a3->data.uv_index = max(uv_index, 0);
    a3->data.frame_step_uv = frame_step_uv;
    a3->data.uv_index_start = max(uv_index_start, 0);
    a3->data.uv_index_end = uv_index_end;
    a3->data.split_uv.x = (float_t)split_u;
    a3->data.split_uv.y = (float_t)split_v;
    a3->data.unk0 = unk0;
    a3->data.unk1 = unk1;

    vec2_rcp(a3->data.split_uv, a3->data.split_uv);
    vec4_mult_scalar(a3->data.color, (float_t)(1.0 / 255.0), a3->data.color);

    uv_max_count = (int32_t)(split_u * split_v);
    if (uv_max_count)
        uv_max_count--;

    switch (uv_index_type) {
    case GLITTER_UV_INDEX_FIXED:
    case GLITTER_UV_INDEX_RANDOM:
    case GLITTER_UV_INDEX_FORWARD:
    case GLITTER_UV_INDEX_REVERSE:
    case GLITTER_UV_INDEX_USER:
        a3->data.uv_index &= uv_max_count;
        break;
    case GLITTER_UV_INDEX_INITIAL_RANDOM_FIXED:
    case GLITTER_UV_INDEX_INITIAL_RANDOM_FORWARD:
    case GLITTER_UV_INDEX_INITIAL_RANDOM_REVERSE:
        if (uv_index < uv_index_start)
            a3->data.uv_index = uv_index_start;
        else if (uv_index_end >= 0 && uv_index > uv_index_end)
            a3->data.uv_index = uv_index_end;

        a3->data.uv_index = min(a3->data.uv_index, uv_max_count);
        break;
    }

    if (uv_index_end >= 0)
        a3->data.uv_index_count = uv_index_end - uv_index_start + 1;
    else
        a3->data.uv_index_count = uv_max_count - uv_index_start;

    if (a3->data.uv_index_count < 0)
        a3->data.uv_index_count = 0;
    return true;
}
