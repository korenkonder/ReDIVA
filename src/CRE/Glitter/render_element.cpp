/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"

namespace Glitter {
    RenderElement::RenderElement() : alive(), uv_index(), disp(), frame(), life_time(), rebound_time(),
        frame_step_uv(), base_speed(), speed(), deceleration(), rot_z_cos(), rot_z_sin(),
        scale_all(), fade_out_frames(), fade_in_frames(), locus_history(), random(), step() {

    }

    void RenderElement::InitLocusHistory(GLT, F2ParticleInst* ptcl_inst, Random* random) {
        if (ptcl_inst->data.data.type == PARTICLE_LOCUS) {
            uint32_t locus_history_size = random->F2GetInt(GLT_VAL,
                -ptcl_inst->data.data.locus_history_size_random,
                ptcl_inst->data.data.locus_history_size_random)
                + ptcl_inst->data.data.locus_history_size;
            locus_history = new LocusHistory(locus_history_size);
        }
        else
            locus_history = 0;
    }

    void RenderElement::InitLocusHistory(XParticleInst* ptcl_inst, Random* random) {
        if (ptcl_inst->data.data.type == PARTICLE_LOCUS) {
            uint32_t locus_history_size = random->XGetInt(
                -ptcl_inst->data.data.locus_history_size_random,
                ptcl_inst->data.data.locus_history_size_random)
                + ptcl_inst->data.data.locus_history_size;
            locus_history = new LocusHistory(locus_history_size);
        }
        else
            locus_history = 0;
    }

    void RenderElement::InitMesh(GLT, F2EmitterInst* emit_inst,
        F2ParticleInst::Data* ptcl_inst_data, int32_t index, Random* random) {
        if (ptcl_inst_data->data.flags & (PARTICLE_EMITTER_LOCAL | PARTICLE_ROTATE_BY_EMITTER))
            base_translation = 0.0f;
        else
            mat4_get_translation(&emit_inst->mat, &base_translation);

        vec3 direction = ptcl_inst_data->data.direction;
        vec3 scale;
        if (ptcl_inst_data->data.flags & PARTICLE_EMITTER_LOCAL)
            scale = 1.0f;
        else
            scale = emit_inst->scale * emit_inst->scale_all;

        vec3 position = 0.0f;
        emit_inst->InitMesh(GLT_VAL, index, scale, position, direction, random);

        if (~ptcl_inst_data->data.flags & PARTICLE_EMITTER_LOCAL)
            mat4_mult_vec3(&emit_inst->mat_rot, &position, &position);

        base_translation += position;
        translation = base_translation;
        translation_prev = base_translation;

        direction = vec3::normalize(direction
            + random->F2GetVec3(GLT_VAL, ptcl_inst_data->data.direction_random));

        if (~ptcl_inst_data->data.flags & PARTICLE_EMITTER_LOCAL)
            mat4_mult_vec3(&emit_inst->mat_rot, &direction, &direction);
        base_direction = direction;
        this->direction = direction;

        float_t speed = random->F2GetFloat(GLT_VAL,
            ptcl_inst_data->data.speed_random) + ptcl_inst_data->data.speed;
        float_t deceleration = random->F2GetFloat(GLT_VAL,
            ptcl_inst_data->data.deceleration_random) + ptcl_inst_data->data.deceleration;
        this->speed = speed * 60.0f;
        this->deceleration = max_def(deceleration * 60.0f, 0.0f);

        this->acceleration = ptcl_inst_data->data.acceleration + ptcl_inst_data->data.gravity
            + random->F2GetVec3(GLT_VAL, ptcl_inst_data->data.acceleration_random);
    }

    void RenderElement::InitMesh(XEmitterInst* emit_inst,
        XParticleInst::Data* ptcl_inst_data, int32_t index, Random* random) {
        if (ptcl_inst_data->data.flags & (PARTICLE_EMITTER_LOCAL | PARTICLE_ROTATE_BY_EMITTER))
            base_translation = 0.0f;
        else
            mat4_get_translation(&emit_inst->mat, &base_translation);

        vec3 direction = ptcl_inst_data->data.direction;
        vec3 scale;
        if (ptcl_inst_data->data.flags & PARTICLE_EMITTER_LOCAL)
            scale = 1.0f;
        else
            scale = emit_inst->scale * emit_inst->scale_all;

        vec3 position = 0.0f;
        emit_inst->InitMesh(index, scale, position, direction, random);

        if (~ptcl_inst_data->data.flags & PARTICLE_EMITTER_LOCAL)
            mat4_mult_vec3(&emit_inst->mat_rot, &position, &position);

        base_translation += position;
        translation = base_translation;
        translation_prev = base_translation;

        direction = vec3::normalize(direction
            + random->XGetVec3(ptcl_inst_data->data.direction_random));

        if (~ptcl_inst_data->data.flags & PARTICLE_EMITTER_LOCAL)
            mat4_mult_vec3(&emit_inst->mat_rot, &direction, &direction);
        base_direction = direction;
        this->direction = direction;

        float_t speed = random->XGetFloat(
            ptcl_inst_data->data.speed_random) + ptcl_inst_data->data.speed;
        float_t deceleration = random->XGetFloat(
            ptcl_inst_data->data.deceleration_random) + ptcl_inst_data->data.deceleration;
        base_speed = speed;
        this->speed = speed;
        this->deceleration = max_def(deceleration, 0.0f);

        this->acceleration = ptcl_inst_data->data.acceleration + ptcl_inst_data->data.gravity
            + random->XGetVec3(ptcl_inst_data->data.acceleration_random);
    }
}
