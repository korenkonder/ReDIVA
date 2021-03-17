/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "particle.h"
#include "animation.h"
#include "curve.h"
#include "parse_file.h"

glitter_particle* FASTCALL glitter_particle_init() {
    glitter_particle* p = force_malloc(sizeof(glitter_particle));
    return p;
}

void FASTCALL glitter_particle_dispose(glitter_particle* p) {
    vector_ptr_glitter_curve_clear(&p->curve, (void*)&glitter_curve_dispose);
    vector_ptr_glitter_curve_dispose(&p->curve);
    free(p);
}

bool FASTCALL Glitter__Particle__ParseFile(glitter_file_reader* a1,
    f2_header* header, glitter_emitter* a3, glitter_effect* a4) {
    glitter_particle* particle; // rax
    int32_t count; // ecx

    if (!header->data_size || Glitter__ParseFile__ReverseSignatureEndianess(header) != 'PTCL')
        return false;

    particle = glitter_particle_init();
    if (!Glitter__Particle__UnpackFile(a1, (int64_t)Glitter__ParseFile__GetDataPointer(header),
        particle, Glitter__ParseFile__GetVersion(header), a4)) {
        glitter_particle_dispose(particle);
        return false;
    }

    header = Glitter__ParseFile__GetSubStructPointer(header);
    if (header)
        while (true) {
            if (Glitter__ParseFile__ReverseSignatureEndianess(header) != 'ENRS') {
                Glitter__Animation__ParseFile(a1, header, &particle->curve);
                break;
            }

            header = Glitter__ParseFile__CheckForEOFC(header);
            if (!header)
                break;
        }

    vector_ptr_glitter_particle_append_element(&a3->particles, &particle);
    count = particle->data.data.count;
    if (count < 1 && particle->data.data.type == GLITTER_PARTICLE_LOCUS)
        count = 30;
    else if (count < 1)
        count = 250;
    particle->data.max_count = 4 * count;
    return true;
}

bool FASTCALL Glitter__Particle__UnpackFile(glitter_file_reader* a1,
    int64_t data, glitter_particle* a3, uint32_t ptcl_version, glitter_effect* a5) {
    uint8_t r;
    uint8_t b;
    uint8_t g;
    uint8_t a;
    int32_t blend_mode0; // ebx
    int32_t uv_index_start; // ebp
    int32_t frame_step_uv; // esi
    int32_t uv_index; // edi
    uint64_t tex_hash0; // rax
    int32_t uv_index_end; // er12
    int32_t uv_index_type;
    int32_t uv_max_count;
    int32_t v31; // ecx
    uint64_t v32; // r15
    int32_t split_u; // [rsp+14h] [rbp-44h]
    int32_t split_v; // [rsp+18h] [rbp-40h]

    a3->data.data.life_time = (float_t)*(int32_t*)data;
    a3->data.data.type = *(int32_t*)(data + 4);
    a3->data.data.draw_type = *(int32_t*)(data + 8);
    if (ptcl_version < 2)
        return false;
    a3->data.data.rotation = *(vec3*)(data + 12);
    a3->data.data.rotation_random = *(vec3*)(data + 24);
    a3->data.data.rotation_add = *(vec3*)(data + 36);
    a3->data.data.rotation_add_random = *(vec3*)(data + 48);
    a3->data.data.position_offset = *(vec3*)(data + 60);
    a3->data.data.position_offset_random = *(vec3*)(data + 72);
    a3->data.data.z_offset = *(float_t*)(data + 84);
    a3->data.data.pivot = *(int32_t*)(data + 88);
    a3->data.data.flags = *(int32_t*)(data + 92);
    if (a5->data.flags & GLITTER_EFFECT_FLAG_LOCAL)
        a3->data.data.flags |= GLITTER_PARTICLE_FLAG_LOCAL;
    if (a5->data.flags & GLITTER_EFFECT_FLAG_EMISSION)
        a3->data.data.flags |= GLITTER_PARTICLE_FLAG_EMISSION;
    a3->data.data.speed = *(float_t*)(data + 96);
    a3->data.data.speed_random = *(float_t*)(data + 100);
    a3->data.data.deceleration = *(float_t*)(data + 104);
    a3->data.data.deceleration_random = *(float_t*)(data + 108);
    a3->data.data.direction = *(vec3*)(data + 112);
    a3->data.data.direction_random = *(vec3*)(data + 124);
    a3->data.data.gravitational_acceleration = *(vec3*)(data + 136);
    a3->data.data.external_acceleration = *(vec3*)(data + 148);
    a3->data.data.external_acceleration_random = *(vec3*)(data + 160);
    a3->data.data.reflection_coeff = *(float_t*)(data + 172);
    a3->data.data.reflection_coeff_random = *(float_t*)(data + 176);
    a3->data.data.rebound_plane_y = *(float_t*)(data + 180);
    a3->data.data.uv_scroll_add = *(vec2*)(data + 184);
    a3->data.data.uv_scroll_add_scale = *(float_t*)(data + 192);
    a3->data.data.sub_flags = *(int32_t*)(data + 196);
    a3->data.data.count = *(int32_t*)(data + 200);
    a3->data.data.locus_history_size = 0;
    a3->data.data.locus_history_size_random = 0;
    a3->data.data.dword118 = 0;
    a3->data.data.emission = 0;

    data = (int64_t)(data + 204);
    if (ptcl_version >= 3) {
        a3->data.data.dword118 = *(int32_t*)data;
        a3->data.data.emission = *(float_t*)(data + 4);
        if (a3->data.data.emission >= 0.0099999998f)
            a3->data.data.flags |= GLITTER_PARTICLE_FLAG_EMISSION;
        data += 8;
    }

    if (a3->data.data.type == GLITTER_PARTICLE_LOCUS || a3->data.data.type == GLITTER_PARTICLE_MESH) {
        a3->data.data.locus_history_size = *(uint16_t*)data;
        a3->data.data.locus_history_size_random = *(uint16_t*)(data + 2);
        data += 4;
    }

    a3->data.data.texture0 = 0;
    a3->data.data.texture1 = 0;
    a3->data.data.blend_mode1 = 1;
    a3->data.data.dword138[0] = 0;
    a3->data.data.tex_hash0 = 0xCBF29CE44FD0BFC1;
    a3->data.data.tex_hash1 = 0xCBF29CE44FD0BFC1;
    if (!data)
        return false;

    tex_hash0 = *(uint64_t*)data;
    r = *(uint8_t*)(data + 8);
    g = *(uint8_t*)(data + 9);
    b = *(uint8_t*)(data + 10);
    a = *(uint8_t*)(data + 11);
    blend_mode0 = *(int32_t*)(data + 12);
    split_u = *(int32_t*)(data + 20);
    split_v = *(int32_t*)(data + 24);
    uv_index_type = *(int32_t*)(data + 28);
    uv_index = *(int16_t*)(data + 32);
    frame_step_uv = *(int16_t*)(data + 34);
    uv_index_start = *(int32_t*)(data + 36);
    uv_index_end = *(int32_t*)(data + 40);
    a3->data.data.tex_hash0 = tex_hash0;
    a3->data.data.color.x = (float_t)r / 255.0f;
    a3->data.data.color.y = (float_t)g / 255.0f;
    a3->data.data.color.z = (float_t)b / 255.0f;
    a3->data.data.color.w = (float_t)a / 255.0f;
    a3->data.data.color_int = (uint32_t)b | ((uint32_t)g << 8) | ((uint32_t)r << 16) | ((uint32_t)a << 24);
    a3->data.data.blend_mode0 = blend_mode0;
    a3->data.data.split_u = (uint8_t)split_u;
    a3->data.data.split_v = (uint8_t)split_v;
    a3->data.data.uv_index_type = uv_index_type;
    a3->data.data.uv_index = max(uv_index, 0);
    a3->data.data.frame_step_uv = (float_t)frame_step_uv;
    a3->data.data.uv_index_start = max(uv_index_start, 0);
    a3->data.data.uv_index_end = uv_index_end;
    a3->data.data.split_uv.x = 1.0f / (float_t)(uint8_t)split_u;
    a3->data.data.split_uv.y = 1.0f / (float_t)(uint8_t)split_v;

    uv_max_count = (int32_t)(uint8_t)split_u * (uint8_t)split_v;
    if (uv_index_type < 8 && (0x9D >> uv_index_type) & 1)
        a3->data.data.uv_index %= uv_max_count;
    else {
        v31 = a3->data.data.uv_index_start;
        if (a3->data.data.uv_index < v31)
            a3->data.data.uv_index = v31;
        else if (uv_index_end >= 0 && a3->data.data.uv_index > uv_index_end)
            a3->data.data.uv_index = uv_index_end;

        a3->data.data.uv_index = max(a3->data.data.uv_index, uv_max_count ? uv_max_count - 1 : 0);
    }

    if (uv_index_end >= 0)
        a3->data.data.uv_index_count = uv_index_end - a3->data.data.uv_index_start + 1;
    else
        a3->data.data.uv_index_count = uv_max_count - a3->data.data.uv_index_start + 1;

    if (a3->data.data.uv_index_count < 0)
        a3->data.data.uv_index_count = 0;

    if (a3->data.data.flags & GLITTER_PARTICLE_FLAG_SECOND_TEXTURE) {
        v32 = a1->version < 7 ? data + 44 : data + 48;
        if (v32) {
            a3->data.data.tex_hash1 = *(uint64_t*)v32;
            a3->data.data.blend_mode1 = *(int32_t*)(v32 + 8);
        }
    }
    return true;
}
