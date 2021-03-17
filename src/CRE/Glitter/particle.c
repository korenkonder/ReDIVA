/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "particle.h"
#include "animation.h"
#include "curve.h"

extern glitter_particle_manager* gpm;

static bool FASTCALL glitter_particle_pack_file(glitter_effect_group* a1,
    f2_struct* st, glitter_particle* a3, glitter_effect* effect, bool use_big_endian);
static bool FASTCALL glitter_particle_unpack_file(glitter_effect_group* a1,
    void* data, glitter_particle* a3, glitter_effect* effect, bool use_big_endian);

glitter_particle* FASTCALL glitter_particle_init() {
    glitter_particle* p = force_malloc(sizeof(glitter_particle));
    return p;
}

bool FASTCALL glitter_particle_parse_file(glitter_effect_group* a1,
    f2_struct* st, vector_ptr_glitter_particle* vec, glitter_effect* effect) {
    f2_struct* i;
    glitter_particle* particle;
    int32_t count;

    if (!st || !st->header.data_size)
        return false;

    particle = glitter_particle_init();
    particle->version = st->header.version;
    if (!glitter_particle_unpack_file(a1, st->data, particle, effect, st->header.use_big_endian)) {
        glitter_particle_dispose(particle);
        return false;
    }

    for (i = st->sub_structs.begin; i != st->sub_structs.end; i++) {
        if (!i->header.data_size)
            continue;

        if (i->header.signature == 0x4D494E41)
            glitter_animation_parse_file(i, &particle->curve);
    }
    count = particle->data.data.count;
    if (count < 1 && particle->data.data.type == GLITTER_PARTICLE_LOCUS)
        count = 30;
    else if (count < 1)
        count = 250;
    particle->data.max_count = 4 * count;
    vector_ptr_glitter_particle_push_back(vec, &particle);
    return true;
}

bool FASTCALL glitter_particle_unparse_file(glitter_effect_group* a1,
    f2_struct* st, glitter_particle* a3, glitter_effect* effect, bool use_big_endian) {
    if (!glitter_particle_pack_file(a1, st, a3, effect, use_big_endian))
        return false;

    f2_struct s;
    if (glitter_animation_unparse_file(&s, &a3->curve, use_big_endian))
        vector_f2_struct_push_back(&st->sub_structs, &s);
    return true;
}

void FASTCALL glitter_particle_dispose(glitter_particle* p) {
    vector_ptr_glitter_curve_free(&p->curve, (void*)glitter_curve_dispose);
    free(p);
}

static bool FASTCALL glitter_particle_pack_file(glitter_effect_group* a1,
    f2_struct* st, glitter_particle* a3, glitter_effect* effect, bool use_big_endian) {
    size_t l;
    size_t d;
    glitter_particle_flag flags;

    if (a3->version < 2)
        return false;

    memset(st, 0, sizeof(f2_struct));
    
    l = 204;
    if (a3->version >= 3)
        l += 8;
    
    if (a3->data.data.type == GLITTER_PARTICLE_LOCUS || a3->data.data.type == GLITTER_PARTICLE_MESH)
        l += 4;
    l += 44;

    if (a1->version >= 7)
        l += 4;
    l += 12;

    l = align_val(l, 0x10);
    d = (size_t)force_malloc(l);
    st->length = l;
    st->data = (void*)d;

    flags = a3->data.data.flags;
    if (effect->data.flags & GLITTER_EFFECT_FLAG_LOCAL)
        flags &= ~GLITTER_PARTICLE_FLAG_LOCAL;
    if (effect->data.flags & GLITTER_EFFECT_FLAG_EMISSION)
        flags &= ~GLITTER_PARTICLE_FLAG_EMISSION;
    flags &= ~GLITTER_PARTICLE_FLAG_EMISSION;

    if (use_big_endian) {
        *(int32_t*)d = reverse_endianess_int32_t((int32_t)roundf(a3->data.data.life_time));
        *(int32_t*)(d + 4) = reverse_endianess_int32_t(a3->data.data.type);
        *(int32_t*)(d + 8) = reverse_endianess_int32_t(a3->data.data.draw_type);
        *(float_t*)(d + 12) = reverse_endianess_float_t(a3->data.data.rotation.x);
        *(float_t*)(d + 16) = reverse_endianess_float_t(a3->data.data.rotation.y);
        *(float_t*)(d + 20) = reverse_endianess_float_t(a3->data.data.rotation.z);
        *(float_t*)(d + 24) = reverse_endianess_float_t(a3->data.data.rotation_random.x);
        *(float_t*)(d + 28) = reverse_endianess_float_t(a3->data.data.rotation_random.y);
        *(float_t*)(d + 32) = reverse_endianess_float_t(a3->data.data.rotation_random.z);
        *(float_t*)(d + 36) = reverse_endianess_float_t(a3->data.data.rotation_add.x);
        *(float_t*)(d + 40) = reverse_endianess_float_t(a3->data.data.rotation_add.y);
        *(float_t*)(d + 44) = reverse_endianess_float_t(a3->data.data.rotation_add.z);
        *(float_t*)(d + 48) = reverse_endianess_float_t(a3->data.data.rotation_add_random.x);
        *(float_t*)(d + 52) = reverse_endianess_float_t(a3->data.data.rotation_add_random.y);
        *(float_t*)(d + 56) = reverse_endianess_float_t(a3->data.data.rotation_add_random.z);
        *(float_t*)(d + 60) = reverse_endianess_float_t(a3->data.data.scale.x);
        *(float_t*)(d + 64) = reverse_endianess_float_t(a3->data.data.scale.y);
        *(float_t*)(d + 68) = reverse_endianess_float_t(a3->data.data.scale.z);
        *(float_t*)(d + 72) = reverse_endianess_float_t(a3->data.data.scale_random.x);
        *(float_t*)(d + 76) = reverse_endianess_float_t(a3->data.data.scale_random.y);
        *(float_t*)(d + 80) = reverse_endianess_float_t(a3->data.data.scale_random.z);
        *(float_t*)(d + 84) = reverse_endianess_float_t(a3->data.data.z_offset);
        *(int32_t*)(d + 88) = reverse_endianess_int32_t(a3->data.data.pivot);
        *(int32_t*)(d + 92) = reverse_endianess_int32_t(flags);
        *(float_t*)(d + 96) = reverse_endianess_float_t(a3->data.data.speed);
        *(float_t*)(d + 100) = reverse_endianess_float_t(a3->data.data.speed_random);
        *(float_t*)(d + 104) = reverse_endianess_float_t(a3->data.data.deceleration);
        *(float_t*)(d + 108) = reverse_endianess_float_t(a3->data.data.deceleration_random);
        *(float_t*)(d + 112) = reverse_endianess_float_t(a3->data.data.direction.x);
        *(float_t*)(d + 116) = reverse_endianess_float_t(a3->data.data.direction.y);
        *(float_t*)(d + 120) = reverse_endianess_float_t(a3->data.data.direction.z);
        *(float_t*)(d + 124) = reverse_endianess_float_t(a3->data.data.direction_random.x);
        *(float_t*)(d + 128) = reverse_endianess_float_t(a3->data.data.direction_random.y);
        *(float_t*)(d + 132) = reverse_endianess_float_t(a3->data.data.direction_random.z);
        *(float_t*)(d + 136) = reverse_endianess_float_t(a3->data.data.gravitational_acceleration.x);
        *(float_t*)(d + 140) = reverse_endianess_float_t(a3->data.data.gravitational_acceleration.y);
        *(float_t*)(d + 144) = reverse_endianess_float_t(a3->data.data.gravitational_acceleration.z);
        *(float_t*)(d + 148) = reverse_endianess_float_t(a3->data.data.external_acceleration.x);
        *(float_t*)(d + 152) = reverse_endianess_float_t(a3->data.data.external_acceleration.y);
        *(float_t*)(d + 156) = reverse_endianess_float_t(a3->data.data.external_acceleration.z);
        *(float_t*)(d + 160) = reverse_endianess_float_t(a3->data.data.external_acceleration_random.x);
        *(float_t*)(d + 164) = reverse_endianess_float_t(a3->data.data.external_acceleration_random.y);
        *(float_t*)(d + 168) = reverse_endianess_float_t(a3->data.data.external_acceleration_random.z);
        *(float_t*)(d + 172) = reverse_endianess_float_t(a3->data.data.reflection_coeff);
        *(float_t*)(d + 176) = reverse_endianess_float_t(a3->data.data.reflection_coeff_random);
        *(float_t*)(d + 180) = reverse_endianess_float_t(a3->data.data.rebound_plane_y);
        *(float_t*)(d + 184) = reverse_endianess_float_t(a3->data.data.uv_scroll_add.x);
        *(float_t*)(d + 188) = reverse_endianess_float_t(a3->data.data.uv_scroll_add.y);
        *(float_t*)(d + 192) = reverse_endianess_float_t(a3->data.data.uv_scroll_add_scale);
        *(int32_t*)(d + 196) = reverse_endianess_int32_t(a3->data.data.sub_flags);
        *(int32_t*)(d + 200) = reverse_endianess_int32_t(a3->data.data.count);
    }
    else {
        *(int32_t*)d = (int32_t)roundf(a3->data.data.life_time);
        *(int32_t*)(d + 4) = a3->data.data.type;
        *(int32_t*)(d + 8) = a3->data.data.draw_type;
        *(vec3*)(d + 12) = a3->data.data.rotation;
        *(vec3*)(d + 24) = a3->data.data.rotation_random;
        *(vec3*)(d + 36) = a3->data.data.rotation_add;
        *(vec3*)(d + 48) = a3->data.data.rotation_add_random;
        *(vec3*)(d + 60) = a3->data.data.scale;
        *(vec3*)(d + 72) = a3->data.data.scale_random;
        *(float_t*)(d + 84) = a3->data.data.z_offset;
        *(int32_t*)(d + 88) = a3->data.data.pivot;
        *(int32_t*)(d + 92) = flags;
        *(float_t*)(d + 96) = a3->data.data.speed;
        *(float_t*)(d + 100) = a3->data.data.speed_random;
        *(float_t*)(d + 104) = a3->data.data.deceleration;
        *(float_t*)(d + 108) = a3->data.data.deceleration_random;
        *(vec3*)(d + 112) = a3->data.data.direction;
        *(vec3*)(d + 124) = a3->data.data.direction_random;
        *(vec3*)(d + 136) = a3->data.data.gravitational_acceleration;
        *(vec3*)(d + 148) = a3->data.data.external_acceleration;
        *(vec3*)(d + 160) = a3->data.data.external_acceleration_random;
        *(float_t*)(d + 172) = a3->data.data.reflection_coeff;
        *(float_t*)(d + 176) = a3->data.data.reflection_coeff_random;
        *(float_t*)(d + 180) = a3->data.data.rebound_plane_y;
        *(vec2*)(d + 184) = a3->data.data.uv_scroll_add;
        *(float_t*)(d + 192) = a3->data.data.uv_scroll_add_scale;
        *(int32_t*)(d + 196) = a3->data.data.sub_flags;
        *(int32_t*)(d + 200) = a3->data.data.count;
    }
    d += 204;

    if (a3->version >= 3) {
        if (use_big_endian) {
            //*(int32_t*)d = reverse_endianess_int32_t(a3->data.data.dword118);
            *(float_t*)(d + 4) = reverse_endianess_float_t(a3->data.data.emission);
        }
        else {
            //*(int32_t*)d = a3->data.data.dword118;
            *(float_t*)(d + 4) = a3->data.data.emission;
        }
        d += 8;
    }

    if (a3->data.data.type == GLITTER_PARTICLE_LOCUS || a3->data.data.type == GLITTER_PARTICLE_MESH) {
        if (use_big_endian) {
            *(uint16_t*)d = reverse_endianess_uint16_t(a3->data.data.locus_history_size);
            *(uint16_t*)(d + 2) = reverse_endianess_uint16_t(a3->data.data.locus_history_size_random);
        }
        else {
            *(uint16_t*)d = a3->data.data.locus_history_size;
            *(uint16_t*)(d + 2) = a3->data.data.locus_history_size_random;
        }
        d += 4;
    }

    if (use_big_endian) {
        *(uint64_t*)d = reverse_endianess_uint64_t(a3->data.data.tex_hash0);
        *(uint8_t*)(d + 8) = (uint8_t)roundf(clamp(a3->data.data.color.x, 0.0f, 1.0f) * 255.0f);
        *(uint8_t*)(d + 9) = (uint8_t)roundf(clamp(a3->data.data.color.y, 0.0f, 1.0f) * 255.0f);
        *(uint8_t*)(d + 10) = (uint8_t)roundf(clamp(a3->data.data.color.z, 0.0f, 1.0f) * 255.0f);
        *(uint8_t*)(d + 11) = (uint8_t)roundf(clamp(a3->data.data.color.w, 0.0f, 1.0f) * 255.0f);
        *(int32_t*)(d + 12) = reverse_endianess_int32_t(a3->data.data.blend_mode0);
        *(int32_t*)(d + 20) = reverse_endianess_int32_t(a3->data.data.split_u);
        *(int32_t*)(d + 24) = reverse_endianess_int32_t(a3->data.data.split_v);
        *(int32_t*)(d + 28) = reverse_endianess_int32_t(a3->data.data.uv_index_type);
        *(int16_t*)(d + 32) = reverse_endianess_int16_t(a3->data.data.uv_index);
        *(int16_t*)(d + 34) = reverse_endianess_int16_t((int16_t)roundf(a3->data.data.frame_step_uv));
        *(int32_t*)(d + 36) = reverse_endianess_int32_t(a3->data.data.uv_index_start);
        *(int32_t*)(d + 40) = reverse_endianess_int32_t(a3->data.data.uv_index_end);
    }
    else {
        *(uint64_t*)d = a3->data.data.tex_hash0;
        *(uint8_t*)(d + 8) = (uint8_t)roundf(clamp(a3->data.data.color.x, 0.0f, 1.0f) * 255.0f);
        *(uint8_t*)(d + 9) = (uint8_t)roundf(clamp(a3->data.data.color.y, 0.0f, 1.0f) * 255.0f);
        *(uint8_t*)(d + 10) = (uint8_t)roundf(clamp(a3->data.data.color.z, 0.0f, 1.0f) * 255.0f);
        *(uint8_t*)(d + 11) = (uint8_t)roundf(clamp(a3->data.data.color.w, 0.0f, 1.0f) * 255.0f);
        *(int32_t*)(d + 12) = a3->data.data.blend_mode0;
        *(int32_t*)(d + 20) = a3->data.data.split_u;
        *(int32_t*)(d + 24) = a3->data.data.split_v;
        *(int32_t*)(d + 28) = a3->data.data.uv_index_type;
        *(int16_t*)(d + 32) = a3->data.data.uv_index;
        *(int16_t*)(d + 34) = (int16_t)roundf(a3->data.data.frame_step_uv);
        *(int32_t*)(d + 36) = a3->data.data.uv_index_start;
        *(int32_t*)(d + 40) = a3->data.data.uv_index_end;
    }
    d += 44;

    if (a3->data.data.flags & GLITTER_PARTICLE_FLAG_SECOND_TEXTURE) {
        if (a1->version >= 7) {
            d += 4;
        }

        if (use_big_endian) {
            *(uint64_t*)d = reverse_endianess_uint64_t(a3->data.data.tex_hash1);
            *(int32_t*)(d + 8) = reverse_endianess_int32_t(a3->data.data.blend_mode1);
        }
        else {
            *(uint64_t*)d = a3->data.data.tex_hash1;
            *(int32_t*)(d + 8) = a3->data.data.blend_mode1;
        }
        d += 12;
    }

    st->header.signature = 0x4C435450;
    st->header.length = 0x20;
    st->header.use_big_endian = use_big_endian ? true : false;
    st->header.use_section_size = true;
    st->header.version = a3->version;
    return true;
}

static bool FASTCALL glitter_particle_unpack_file(glitter_effect_group* a1,
    void* data, glitter_particle* a3, glitter_effect* effect, bool use_big_endian) {
    uint8_t r;
    uint8_t b;
    uint8_t g;
    uint8_t a;
    int32_t blend_mode0;
    int32_t uv_index_start;
    int32_t frame_step_uv;
    int32_t uv_index;
    uint64_t tex_hash0;
    int32_t uv_index_end;
    int32_t uv_index_type;
    int32_t uv_max_count;
    uint8_t split_u;
    uint8_t split_v;
    size_t d;

    d = (size_t)data;
    if (use_big_endian) {
        a3->data.data.life_time = (float_t)reverse_endianess_int32_t(*(int32_t*)d);
        a3->data.data.type = reverse_endianess_int32_t(*(int32_t*)(d + 4));
        a3->data.data.draw_type = reverse_endianess_int32_t(*(int32_t*)(d + 8));
    }
    else {
        a3->data.data.life_time = (float_t) * (int32_t*)d;
        a3->data.data.type = *(int32_t*)(d + 4);
        a3->data.data.draw_type = *(int32_t*)(d + 8);
    }
    
    if (a3->version < 2)
        return false;

    if (use_big_endian) {
        a3->data.data.rotation.x = reverse_endianess_float_t(*(float_t*)(d + 12));
        a3->data.data.rotation.y = reverse_endianess_float_t(*(float_t*)(d + 16));
        a3->data.data.rotation.z = reverse_endianess_float_t(*(float_t*)(d + 20));
        a3->data.data.rotation_random.x = reverse_endianess_float_t(*(float_t*)(d + 24));
        a3->data.data.rotation_random.y = reverse_endianess_float_t(*(float_t*)(d + 28));
        a3->data.data.rotation_random.z = reverse_endianess_float_t(*(float_t*)(d + 32));
        a3->data.data.rotation_add.x = reverse_endianess_float_t(*(float_t*)(d + 36));
        a3->data.data.rotation_add.y = reverse_endianess_float_t(*(float_t*)(d + 40));
        a3->data.data.rotation_add.z = reverse_endianess_float_t(*(float_t*)(d + 44));
        a3->data.data.rotation_add_random.x = reverse_endianess_float_t(*(float_t*)(d + 48));
        a3->data.data.rotation_add_random.y = reverse_endianess_float_t(*(float_t*)(d + 52));
        a3->data.data.rotation_add_random.z = reverse_endianess_float_t(*(float_t*)(d + 56));
        a3->data.data.scale.x = reverse_endianess_float_t(*(float_t*)(d + 60));
        a3->data.data.scale.y = reverse_endianess_float_t(*(float_t*)(d + 64));
        a3->data.data.scale.z = reverse_endianess_float_t(*(float_t*)(d + 68));
        a3->data.data.scale_random.x = reverse_endianess_float_t(*(float_t*)(d + 72));
        a3->data.data.scale_random.y = reverse_endianess_float_t(*(float_t*)(d + 76));
        a3->data.data.scale_random.z = reverse_endianess_float_t(*(float_t*)(d + 80));
        a3->data.data.z_offset = reverse_endianess_float_t(*(float_t*)(d + 84));
        a3->data.data.pivot = reverse_endianess_int32_t(*(int32_t*)(d + 88));
        a3->data.data.flags = reverse_endianess_int32_t(*(int32_t*)(d + 92));
    }
    else {
        a3->data.data.rotation = *(vec3*)(d + 12);
        a3->data.data.rotation_random = *(vec3*)(d + 24);
        a3->data.data.rotation_add = *(vec3*)(d + 36);
        a3->data.data.rotation_add_random = *(vec3*)(d + 48);
        a3->data.data.scale = *(vec3*)(d + 60);
        a3->data.data.scale_random = *(vec3*)(d + 72);
        a3->data.data.z_offset = *(float_t*)(d + 84);
        a3->data.data.pivot = *(int32_t*)(d + 88);
        a3->data.data.flags = *(int32_t*)(d + 92);
    }
    
    if (effect->data.flags & GLITTER_EFFECT_FLAG_LOCAL)
        a3->data.data.flags |= GLITTER_PARTICLE_FLAG_LOCAL;
    if (effect->data.flags & GLITTER_EFFECT_FLAG_EMISSION)
        a3->data.data.flags |= GLITTER_PARTICLE_FLAG_EMISSION;

    if (use_big_endian) {
        a3->data.data.speed = reverse_endianess_float_t(*(float_t*)(d + 96));
        a3->data.data.speed_random = reverse_endianess_float_t(*(float_t*)(d + 100));
        a3->data.data.deceleration = reverse_endianess_float_t(*(float_t*)(d + 104));
        a3->data.data.deceleration_random = reverse_endianess_float_t(*(float_t*)(d + 108));
        a3->data.data.direction.x = reverse_endianess_float_t(*(float_t*)(d + 112));
        a3->data.data.direction.y = reverse_endianess_float_t(*(float_t*)(d + 116));
        a3->data.data.direction.z = reverse_endianess_float_t(*(float_t*)(d + 120));
        a3->data.data.direction_random.x = reverse_endianess_float_t(*(float_t*)(d + 124));
        a3->data.data.direction_random.y = reverse_endianess_float_t(*(float_t*)(d + 128));
        a3->data.data.direction_random.z = reverse_endianess_float_t(*(float_t*)(d + 132));
        a3->data.data.gravitational_acceleration.x = reverse_endianess_float_t(*(float_t*)(d + 136));
        a3->data.data.gravitational_acceleration.y = reverse_endianess_float_t(*(float_t*)(d + 140));
        a3->data.data.gravitational_acceleration.z = reverse_endianess_float_t(*(float_t*)(d + 144));
        a3->data.data.external_acceleration.x = reverse_endianess_float_t(*(float_t*)(d + 148));
        a3->data.data.external_acceleration.y = reverse_endianess_float_t(*(float_t*)(d + 152));
        a3->data.data.external_acceleration.z = reverse_endianess_float_t(*(float_t*)(d + 156));
        a3->data.data.external_acceleration_random.x = reverse_endianess_float_t(*(float_t*)(d + 160));
        a3->data.data.external_acceleration_random.y = reverse_endianess_float_t(*(float_t*)(d + 164));
        a3->data.data.external_acceleration_random.z = reverse_endianess_float_t(*(float_t*)(d + 168));
        a3->data.data.reflection_coeff = reverse_endianess_float_t(*(float_t*)(d + 172));
        a3->data.data.reflection_coeff_random = reverse_endianess_float_t(*(float_t*)(d + 176));
        a3->data.data.rebound_plane_y = reverse_endianess_float_t(*(float_t*)(d + 180));
        a3->data.data.uv_scroll_add.x = reverse_endianess_float_t(*(float_t*)(d + 184));
        a3->data.data.uv_scroll_add.y = reverse_endianess_float_t(*(float_t*)(d + 188));
        a3->data.data.uv_scroll_add_scale = reverse_endianess_float_t(*(float_t*)(d + 192));
        a3->data.data.sub_flags = reverse_endianess_int32_t(*(int32_t*)(d + 196));
        a3->data.data.count = reverse_endianess_int32_t(*(int32_t*)(d + 200));
    }
    else {
        a3->data.data.speed = *(float_t*)(d + 96);
        a3->data.data.speed_random = *(float_t*)(d + 100);
        a3->data.data.deceleration = *(float_t*)(d + 104);
        a3->data.data.deceleration_random = *(float_t*)(d + 108);
        a3->data.data.direction = *(vec3*)(d + 112);
        a3->data.data.direction_random = *(vec3*)(d + 124);
        a3->data.data.gravitational_acceleration = *(vec3*)(d + 136);
        a3->data.data.external_acceleration = *(vec3*)(d + 148);
        a3->data.data.external_acceleration_random = *(vec3*)(d + 160);
        a3->data.data.reflection_coeff = *(float_t*)(d + 172);
        a3->data.data.reflection_coeff_random = *(float_t*)(d + 176);
        a3->data.data.rebound_plane_y = *(float_t*)(d + 180);
        a3->data.data.uv_scroll_add = *(vec2*)(d + 184);
        a3->data.data.uv_scroll_add_scale = *(float_t*)(d + 192);
        a3->data.data.sub_flags = *(int32_t*)(d + 196);
        a3->data.data.count = *(int32_t*)(d + 200);
    }
    d += 204;

    a3->data.data.locus_history_size = 0;
    a3->data.data.locus_history_size_random = 0;
    //a3->data.data.dword118 = 0;
    a3->data.data.emission = 0;

    if (a3->version >= 3) {
        if (use_big_endian) {
            //a3->data.data.dword118 = reverse_endianess_int32_t(*(int32_t*)d);
            a3->data.data.emission = reverse_endianess_float_t(*(float_t*)(d + 4));
        }
        else {
            //a3->data.data.dword118 = *(int32_t*)d;
            a3->data.data.emission = *(float_t*)(d + 4);
        }

        if (a3->data.data.emission >= 0.0099999998f)
            a3->data.data.flags |= GLITTER_PARTICLE_FLAG_EMISSION;
        d += 8;
    }

    if (a3->data.data.type == GLITTER_PARTICLE_LOCUS || a3->data.data.type == GLITTER_PARTICLE_MESH) {
        if (use_big_endian) {
            a3->data.data.locus_history_size = reverse_endianess_uint16_t(*(uint16_t*)d);
            a3->data.data.locus_history_size_random = reverse_endianess_uint16_t(*(uint16_t*)(d + 2));
        }
        else {
            a3->data.data.locus_history_size = *(uint16_t*)d;
            a3->data.data.locus_history_size_random = *(uint16_t*)(d + 2);
        }
        d += 4;
    }

    a3->data.data.texture0 = 0;
    a3->data.data.texture1 = 0;
    //a3->data.data.dword138[0] = 0;

    if (use_big_endian) {
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
    }
    else {
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
    }
    d += 44;

    a3->data.data.tex_hash0 = tex_hash0;
    a3->data.data.tex_hash1 = gpm->f2 ? 0x0CAD3078 : 0xCBF29CE44FD0BFC1;
    a3->data.data.color.x = (float_t)r * (float_t)(1.0 / 255.0);
    a3->data.data.color.y = (float_t)g * (float_t)(1.0 / 255.0);
    a3->data.data.color.z = (float_t)b * (float_t)(1.0 / 255.0);
    a3->data.data.color.w = (float_t)a * (float_t)(1.0 / 255.0);
    a3->data.data.blend_mode0 = blend_mode0;
    a3->data.data.blend_mode1 = GLITTER_PARTICLE_BLEND_TYPICAL;
    a3->data.data.split_u = split_u;
    a3->data.data.split_v = split_v;
    a3->data.data.uv_index_type = uv_index_type;
    a3->data.data.uv_index = max(uv_index, 0);
    a3->data.data.frame_step_uv = (float_t)frame_step_uv;
    a3->data.data.uv_index_start = max(uv_index_start, 0);
    a3->data.data.uv_index_end = uv_index_end;
    a3->data.data.split_uv.x = 1.0f / (float_t)split_u;
    a3->data.data.split_uv.y = 1.0f / (float_t)split_v;

    uv_max_count = (int32_t)(split_u * split_v);
    if (uv_max_count)
        uv_max_count--;

    switch (uv_index_type) {
    case GLITTER_UV_INDEX_FIXED:
    case GLITTER_UV_INDEX_RANDOM:
    case GLITTER_UV_INDEX_FORWARD:
    case GLITTER_UV_INDEX_REVERSE:
    case GLITTER_UV_INDEX_USER:
        a3->data.data.uv_index &= uv_max_count;
        break;
    case GLITTER_UV_INDEX_INITIAL_RANDOM_FIXED:
    case GLITTER_UV_INDEX_INITIAL_RANDOM_FORWARD:
    case GLITTER_UV_INDEX_INITIAL_RANDOM_REVERSE:
        if (a3->data.data.uv_index < a3->data.data.uv_index_start)
            a3->data.data.uv_index = a3->data.data.uv_index_start;
        else if (uv_index_end >= 0 && a3->data.data.uv_index > uv_index_end)
            a3->data.data.uv_index = uv_index_end;

        a3->data.data.uv_index = max(a3->data.data.uv_index, uv_max_count);
        break;
    }

    if (uv_index_end >= 0)
        a3->data.data.uv_index_count = uv_index_end - a3->data.data.uv_index_start + 1;
    else
        a3->data.data.uv_index_count = uv_max_count - a3->data.data.uv_index_start;

    if (a3->data.data.uv_index_count < 0)
        a3->data.data.uv_index_count = 0;

    if (a3->data.data.flags & GLITTER_PARTICLE_FLAG_SECOND_TEXTURE) {
        if (a1->version >= 7)
            d += 4;

        if (use_big_endian) {
            a3->data.data.tex_hash1 = reverse_endianess_uint64_t(*(uint64_t*)d);
            a3->data.data.blend_mode1 = reverse_endianess_int32_t(*(int32_t*)(d + 8));
        }
        else {
            a3->data.data.tex_hash1 = *(uint64_t*)d;
            a3->data.data.blend_mode1 = *(int32_t*)(d + 8);
        }
    }
    return true;
}
