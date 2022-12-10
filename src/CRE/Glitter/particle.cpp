/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"

namespace Glitter {
    Particle::Data::Data() : flags(), life_time(), life_time_random(), fade_in(), fade_in_random(),
        fade_out(), fade_out_random(), type(), pivot(), draw_type(), z_offset(), speed(), speed_random(),
        deceleration(), deceleration_random(), reflection_coeff(), reflection_coeff_random(), rebound_plane_y(),
        uv_index_type(), uv_index(), frame_step_uv(), uv_index_start(), uv_index_end(), uv_index_count(),
        uv_scroll_add_scale(), uv_scroll_2nd_add_scale(), split_u(), split_v(), blend_mode(), mask_blend_mode(),
        sub_flags(), count(), locus_history_size(), locus_history_size_random(), draw_flags(),
        emission(), tex_hash(), mask_tex_hash(), texture(), mask_texture(), unk0(), unk1(), mesh() {

    }

    Particle::Particle(GLT) : data() {
        version = GLT_VAL == Glitter::X ? 0x05 : 0x03;
        data.pivot = PIVOT_MIDDLE_CENTER;
        data.scale = 1.0f;
        data.reflection_coeff = 1.0f;
        data.color = 1.0f;
        data.uv_index = 0;
        data.uv_index_start = 0;
        data.uv_index_end = 1;
        data.uv_scroll_add_scale = 1.0f;
        data.uv_scroll_2nd_add_scale = 1.0f;
        data.split_uv = 1.0f;
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
