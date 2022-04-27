/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"

namespace Glitter {
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
            base_translation = vec3_null;
        else
            mat4_get_translation(&emit_inst->mat, &base_translation);

        vec3 direction = ptcl_inst_data->data.direction;
        vec3 scale;
        if (ptcl_inst_data->data.flags & PARTICLE_EMITTER_LOCAL)
            scale = vec3_identity;
        else
            vec3_mult_scalar(emit_inst->scale, emit_inst->scale_all, scale);

        vec3 position = vec3_null;
        emit_inst->InitMesh(GLT_VAL, index, &scale, &position, &direction, random);

        if (~ptcl_inst_data->data.flags & PARTICLE_EMITTER_LOCAL)
            mat4_mult_vec3(&emit_inst->mat_rot, &position, &position);

        vec3_add(base_translation, position, base_translation);
        translation = base_translation;
        translation_prev = base_translation;

        vec3 direction_random;
        random->F2GetVec3(GLT_VAL, &ptcl_inst_data->data.direction_random, &direction_random);
        vec3_add(direction, direction_random, direction);
        vec3_normalize(direction, direction);

        if (~ptcl_inst_data->data.flags & PARTICLE_EMITTER_LOCAL)
            mat4_mult_vec3(&emit_inst->mat_rot, &direction, &direction);
        base_direction = direction;
        this->direction = direction;

        float_t speed = random->F2GetFloat(GLT_VAL,
            ptcl_inst_data->data.speed_random) + ptcl_inst_data->data.speed;
        float_t deceleration = random->F2GetFloat(GLT_VAL,
            ptcl_inst_data->data.deceleration_random) + ptcl_inst_data->data.deceleration;
        this->speed = speed * 60.0f;
        this->deceleration = max(deceleration * 60.0f, 0.0f);

        vec3 acceleration;
        random->F2GetVec3(GLT_VAL, &ptcl_inst_data->data.acceleration_random, &acceleration);
        vec3_add(acceleration, ptcl_inst_data->data.acceleration, acceleration);
        vec3_add(acceleration, ptcl_inst_data->data.gravity, this->acceleration);
    }

    void RenderElement::InitMesh(XEmitterInst* emit_inst,
        XParticleInst::Data* ptcl_inst_data, int32_t index, Random* random) {
        if (ptcl_inst_data->data.flags & (PARTICLE_EMITTER_LOCAL | PARTICLE_ROTATE_BY_EMITTER))
            base_translation = vec3_null;
        else
            mat4_get_translation(&emit_inst->mat, &base_translation);

        vec3 direction = ptcl_inst_data->data.direction;
        vec3 scale;
        if (ptcl_inst_data->data.flags & PARTICLE_EMITTER_LOCAL)
            scale = vec3_identity;
        else
            vec3_mult_scalar(emit_inst->scale, emit_inst->scale_all, scale);

        vec3 position = vec3_null;
        emit_inst->InitMesh(index, &scale, &position, &direction, random);

        if (~ptcl_inst_data->data.flags & PARTICLE_EMITTER_LOCAL)
            mat4_mult_vec3(&emit_inst->mat_rot, &position, &position);

        vec3_add(base_translation, position, base_translation);
        translation = base_translation;
        translation_prev = base_translation;

        vec3 direction_random;
        random->XGetVec3(&ptcl_inst_data->data.direction_random, &direction_random);
        vec3_add(direction, direction_random, direction);
        vec3_normalize(direction, direction);

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
        this->deceleration = max(deceleration, 0.0f);

        vec3 acceleration;
        random->XGetVec3(&ptcl_inst_data->data.acceleration_random, &acceleration);
        vec3_add(acceleration, ptcl_inst_data->data.acceleration, acceleration);
        vec3_add(acceleration, ptcl_inst_data->data.gravity, this->acceleration);
    }
}
