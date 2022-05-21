/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"

namespace Glitter {
    Particle::Particle(GLT) : data() {
        version = GLT_VAL == Glitter::X ? 0x05 : 0x03;
        data.pivot = PIVOT_MIDDLE_CENTER;
        data.scale = vec3_identity;
        data.reflection_coeff = 1.0f;
        data.color = vec4u_identity;
        data.uv_index = 0;
        data.uv_index_start = 0;
        data.uv_index_end = 1;
        data.uv_scroll_add_scale = 1.0f;
        data.uv_scroll_2nd_add_scale = 1.0f;
        data.split_uv = vec2_identity;
        data.split_u = 1;
        data.split_v = 1;
        data.sub_flags = PARTICLE_SUB_USE_CURVE;
        data.blend_mode = PARTICLE_BLEND_TYPICAL;
        data.mask_blend_mode = PARTICLE_BLEND_TYPICAL;
        data.tex_hash = GLT_VAL != Glitter::FT
            ? hash_murmurhash_empty : hash_fnv1a64m_empty;
        data.mask_tex_hash = GLT_VAL != Glitter::FT
            ? hash_murmurhash_empty : hash_fnv1a64m_empty;

        if (GLT_VAL == Glitter::X) {
            data.mesh.object_set_name_hash = hash_murmurhash_empty;
            data.mesh.object_name_hash = hash_murmurhash_empty;
            //data.mesh.mesh_name[0] = 0;
            //data.mesh.sub_mesh_hash = hash_murmurhash_empty;
        }
    }

    Particle::~Particle() {

    }
}

/*
#include "particle.hpp"
#include "animation.hpp"
#include "curve.hpp"

static bool glitter_particle_pack_file(GLT, EffectGroup* a1,
    f2_struct* st, glitter_particle* a3, glitter_effect* effect);

bool glitter_particle_unparse_file(GLT, EffectGroup* a1,
    f2_struct* st, glitter_particle* a3, glitter_effect* effect) {
    if (!glitter_particle_pack_file(GLT_VAL, a1, st, a3, effect))
        return false;

    glitter_curve_type_flags flags = (glitter_curve_type_flags)0;
    if (a1->type == Glitter::X)
        flags = glitter_particle_x_curve_flags;
    else
        flags = glitter_particle_curve_flags;

    if (a3->data.type != PARTICLE_MESH) {
        enum_and(flags, ~CURVE_TYPE_UV_SCROLL_2ND);
        if (a3->data.draw_type != DIRECTION_PARTICLE_ROTATION)
            enum_and(flags, ~(CURVE_TYPE_ROTATION_X | CURVE_TYPE_ROTATION_Y));
    }

    f2_struct s;
    if (glitter_animation_unparse_file(GLT_VAL, &s, &a3->animation, flags))
        st->sub_structs.push_back(s);
    return true;
}

static bool glitter_particle_pack_file(GLT,
    EffectGroup* a1, f2_struct* st, glitter_particle* a3, glitter_effect* effect) {
    size_t l;
    size_t d;
    glitter_particle_flag flags;

    if (a3->version < 2)
        return false;

    l = 0;

    uint32_t o;
    enrs e;
    enrs_entry ee;

    ee = { 0, 1, 204, 1 };
    ee.append(0, 51, ENRS_DWORD);
    e.vec.push_back(ee);
    l += o = 204;

    if (a3->version == 3) {
        ee = { o, 1, 8, 1 };
        ee.append(0, 2, ENRS_DWORD);
        e.vec.push_back(ee);
        l += o = 8;
    }

    if (a3->data.type == PARTICLE_LOCUS || a3->data.type == PARTICLE_MESH) {
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
    if (a1->version >= 7) {
        ee.count++;
        ee.size += 4;
        ee.append.push_back(0, 1, ENRS_DWORD);
        e.vec.push_back(ee);
        l += o = 48;
    }
    else {
        e.vec.push_back(ee);
        l += o = 44;
    }

    ee = { o, 2, 12, 1 };
    ee.append.push_back(0, 1, ENRS_QWORD);
    ee.append.push_back(0, 1, ENRS_DWORD);
    e.vec.push_back(ee);
    l += o = 12;

    l = align_val(l, 0x10);
    st->data.resize(l);
    d = (size_t)st->data.data();
    st->enrs = e;

    flags = a3->data.flags;
    if (effect->data.flags & EFFECT_LOCAL)
        enum_and(flags, ~PARTICLE_LOCAL);
    if (effect->data.flags & EFFECT_EMISSION)
        enum_and(flags, ~PARTICLE_EMISSION);

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

    if (a3->data.type == PARTICLE_LOCUS || a3->data.type == PARTICLE_MESH) {
        *(uint16_t*)d = a3->data.locus_history_size;
        *(uint16_t*)(d + 2) = a3->data.locus_history_size_random;
        d += 4;
    }

    *(uint64_t*)d = a3->data.tex_hash;
    *(uint8_t*)(d + 8) = (uint8_t)roundf(clamp(a3->data.color.x, 0.0f, 1.0f) * 255.0f);
    *(uint8_t*)(d + 9) = (uint8_t)roundf(clamp(a3->data.color.y, 0.0f, 1.0f) * 255.0f);
    *(uint8_t*)(d + 10) = (uint8_t)roundf(clamp(a3->data.color.z, 0.0f, 1.0f) * 255.0f);
    *(uint8_t*)(d + 11) = (uint8_t)roundf(clamp(a3->data.color.w, 0.0f, 1.0f) * 255.0f);
    *(int32_t*)(d + 12) = a3->data.blend_mode;
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

    if (a3->data.flags & PARTICLE_TEXTURE_MASK) {
        *(uint64_t*)d = a3->data.mask_tex_hash;
        *(int32_t*)(d + 8) = a3->data.mask_blend_mode;
        d += 12;
    }
    else {
        *(uint64_t*)d = 0;
        *(int32_t*)(d + 8) = 0;
        d += 12;
    }

    st->header.signature = reverse_endianness_uint32_t('PTCL');
    st->header.length = 0x20;
    st->header.use_big_endian = false;
    st->header.use_section_size = true;
    st->header.version = a3->version;
    return true;
}
*/