/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"

namespace Glitter {
    ParticleInst::ParticleInst() : particle() {

    }

    ParticleInst::~ParticleInst() {

    }

    F2ParticleInst::Data::Data() : data(), flags(),
        render_group(), random_ptr(), effect(), emitter(), parent(), particle() {
    }

    F2ParticleInst::Data::~Data() {

    }

    F2ParticleInst::F2ParticleInst(Particle* ptcl, F2EffectInst* eff_inst,
        F2EmitterInst* emit_inst, Random* random, float_t emission) {
        particle = ptcl;
        data.effect = eff_inst;
        data.emitter = emit_inst;
        data.particle = ptcl;
        data.data = ptcl->data;
        data.random_ptr = random;

        if (data.data.type != PARTICLE_LOCUS) {
            F2RenderGroup* rend_group = new F2RenderGroup(this);
            if (rend_group) {
                rend_group->alpha = eff_inst->GetAlpha();
                rend_group->fog = eff_inst->GetFog();
                if (data.data.type == PARTICLE_QUAD
                    && data.data.blend_mode == PARTICLE_BLEND_PUNCH_THROUGH)
                    rend_group->alpha = DRAW_PASS_3D_OPAQUE;

                if (data.data.emission >= min_emission)
                    rend_group->emission = data.data.emission;
                else if (eff_inst->data.emission >= min_emission)
                    rend_group->emission = eff_inst->data.emission;
                else
                    rend_group->emission = emission;
                data.render_group = rend_group;
                eff_inst->render_scene.Append(rend_group);
            }
        }
        else
            enum_or(data.flags, PARTICLE_INST_NO_CHILD);
    }

    F2ParticleInst::F2ParticleInst(F2ParticleInst* parent, float_t emission) {
        particle = parent->particle;
        data.effect = parent->data.effect;
        data.emitter = parent->data.emitter;
        data.parent = parent;

        data.random_ptr = parent->data.random_ptr;
        data.data = parent->data.data;

        data.particle = parent->data.particle;

        F2RenderGroup* rend_group = new F2RenderGroup(this);
        if (rend_group) {
            F2EffectInst* effect = parent->data.effect;
            rend_group->alpha = effect->GetAlpha();
            rend_group->fog = effect->GetFog();
            if (data.data.blend_mode == PARTICLE_BLEND_PUNCH_THROUGH
                && data.data.type == PARTICLE_QUAD)
                rend_group->alpha = DRAW_PASS_3D_OPAQUE;
            if (effect->data.emission >= min_emission)
                rend_group->emission = effect->data.emission;
            else
                rend_group->emission = emission;
            data.render_group = rend_group;
            effect->render_scene.Append(rend_group);
        }

        if (data.data.type == PARTICLE_LOCUS)
            enum_or(data.flags, PARTICLE_INST_NO_CHILD);
    }

    F2ParticleInst::~F2ParticleInst() {
        for (F2ParticleInst* i : data.children)
            delete i;

        if (data.render_group) {
            data.render_group->DeleteBuffers(true);
            data.render_group = 0;
        }
    }

    void F2ParticleInst::AccelerateParticle(GLT, RenderElement* rend_elem,
        float_t time, float_t delta_frame, Random* random) {
        rend_elem->translation_prev = rend_elem->translation;
        float_t delta_time = delta_frame * (float_t)(1.0 / 60.0);
        float_t diff_time = time - rend_elem->rebound_time - delta_time;

        vec3 acceleration;
        vec3_mult_scalar(rend_elem->acceleration, delta_time * (delta_time * 0.5f + diff_time), acceleration);
        float_t speed = rend_elem->deceleration * delta_time * (delta_time * 0.5f - diff_time) + rend_elem->speed;
        if (speed >= 0.01f) {
            vec3 direction;
            vec3_mult_scalar(rend_elem->direction, speed * delta_time, direction);
            vec3_add(acceleration, direction, acceleration);
        }

        vec3_add(rend_elem->translation, acceleration, rend_elem->translation);
        vec3_add(rend_elem->base_translation, acceleration, rend_elem->base_translation);

        if (data.data.flags & PARTICLE_REBOUND_PLANE
            && rend_elem->translation_prev.y > data.data.rebound_plane_y
            && rend_elem->translation.y <= data.data.rebound_plane_y) {
            float_t reflection_coeff = random->F2GetFloat(GLT_VAL,
                data.data.reflection_coeff_random) + data.data.reflection_coeff;
            rend_elem->rebound_time = time;

            vec3 direction;
            vec3_sub(rend_elem->translation, rend_elem->translation_prev, direction);
            vec3_mult_scalar(direction, reflection_coeff * 60.0f, direction);
            const vec3 reverse_y_dir = { 0.0f, -0.0f, 0.0f };
            vec3_xor(direction, reverse_y_dir, rend_elem->direction);

            rend_elem->translation.y = rend_elem->translation_prev.y;
        }
    }

    void F2ParticleInst::Copy(F2ParticleInst* dst, float_t emission) {
        dst->data.flags = data.flags;
        if (data.render_group && dst->data.render_group)
            data.render_group->Copy(dst->data.render_group);

        for (F2ParticleInst*& i : dst->data.children)
            if (i) {
                delete i;
                i = 0;
            }
        dst->data.children.clear();

        for (F2ParticleInst*& i : data.children)
            if (i) {
                F2ParticleInst* child = new F2ParticleInst(this, emission);
                dst->data.children.push_back(child);
                i->Copy(child, emission);
            }
    }

    void F2ParticleInst::Emit(GPM, GLT, int32_t dup_count, int32_t count, float_t emission) {
        if (data.flags & PARTICLE_INST_ENDED)
            return;

        F2ParticleInst* ptcl = this;
        while (!ptcl->data.parent && ptcl->data.flags & PARTICLE_INST_NO_CHILD) {
            F2ParticleInst* particle = new F2ParticleInst(ptcl, emission);
            if (particle)
                ptcl->data.children.push_back(particle);
            else
                return;

            ptcl = particle;
            if (particle->data.flags & PARTICLE_INST_ENDED)
                return;
        }

        if (ptcl->data.render_group)
            ptcl->data.render_group->Emit(GPM_VAL, GLT_VAL,
                &ptcl->data, ptcl->data.emitter, dup_count, count);
    }

    void F2ParticleInst::EmitParticle(GPM, GLT, RenderElement* rend_elem, F2EmitterInst* emit_inst,
        F2ParticleInst::Data* ptcl_inst_data, int32_t index, Random* random) {
        GPM_VAL->CounterIncrement();
        random->SetValue(GPM_VAL->CounterGet());
        rend_elem->random = random->F2GetInt(GLT_VAL, Random::F2GetMax(GLT_VAL));
        rend_elem->frame = 0.0f;
        rend_elem->rebound_time = 0.0f;
        rend_elem->uv = vec2_null;
        rend_elem->uv_index = ptcl_inst_data->data.uv_index;

        rend_elem->fade_in_frames = 0.0f;
        rend_elem->fade_out_frames = 0.0f;
        rend_elem->life_time = (float_t)ptcl_inst_data->data.life_time;

        rend_elem->color = ptcl_inst_data->data.color;
        if (ptcl_inst_data->data.draw_type == DIRECTION_PARTICLE_ROTATION) {
            rend_elem->rotation.x = random->F2GetFloat(GLT_VAL, ptcl_inst_data->data.rotation_random.x)
                + ptcl_inst_data->data.rotation.x;
            rend_elem->rotation_add.x = random->F2GetFloat(GLT_VAL, ptcl_inst_data->data.rotation_add_random.x)
                + ptcl_inst_data->data.rotation_add.x;
            rend_elem->rotation.y = random->F2GetFloat(GLT_VAL, ptcl_inst_data->data.rotation_random.y)
                + ptcl_inst_data->data.rotation.y;
            rend_elem->rotation_add.y = random->F2GetFloat(GLT_VAL, ptcl_inst_data->data.rotation_add_random.y)
                + ptcl_inst_data->data.rotation_add.y;
        }
        else {
            rend_elem->rotation.x = 0.0f;
            rend_elem->rotation.y = 0.0f;
            rend_elem->rotation_add.x = 0.0f;
            rend_elem->rotation_add.y = 0.0f;
        }
        rend_elem->rotation.z = random->F2GetFloat(GLT_VAL, ptcl_inst_data->data.rotation_random.z)
            + ptcl_inst_data->data.rotation.z;
        rend_elem->rotation_add.z = random->F2GetFloat(GLT_VAL,
            ptcl_inst_data->data.rotation_add_random.z) + ptcl_inst_data->data.rotation_add.z;
        rend_elem->uv_scroll = vec2_null;
        rend_elem->uv_scroll_2nd = vec2_null;
        rend_elem->scale = vec3_identity;
        rend_elem->scale_all = 1.0f;
        rend_elem->frame_step_uv = (float_t)ptcl_inst_data->data.frame_step_uv;

        rend_elem->scale_particle.x = random->F2GetFloat(GLT_VAL, ptcl_inst_data->data.scale_random.x)
            + ptcl_inst_data->data.scale.x;
        if (ptcl_inst_data->data.flags & PARTICLE_SCALE_Y_BY_X)
            rend_elem->scale_particle.y = rend_elem->scale_particle.x;
        else
            rend_elem->scale_particle.y = random->F2GetFloat(GLT_VAL, ptcl_inst_data->data.scale_random.y)
            + ptcl_inst_data->data.scale.y;

        int32_t max_uv = ptcl_inst_data->data.split_u * ptcl_inst_data->data.split_v;
        if (max_uv > 1 && ptcl_inst_data->data.uv_index_count > 1) {
            switch (ptcl_inst_data->data.uv_index_type) {
            case UV_INDEX_INITIAL_RANDOM_FIXED:
            case UV_INDEX_INITIAL_RANDOM_FORWARD:
            case UV_INDEX_INITIAL_RANDOM_REVERSE:
                rend_elem->uv_index += random->F2GetInt(GLT_VAL, ptcl_inst_data->data.uv_index_count);
                break;
            }
            rend_elem->uv_index = min(rend_elem->uv_index, ptcl_inst_data->data.uv_index_end);
            rend_elem->uv.x = (float_t)(rend_elem->uv_index
                % ptcl_inst_data->data.split_u) * ptcl_inst_data->data.split_uv.x;
            rend_elem->uv.y = (float_t)(rend_elem->uv_index
                / ptcl_inst_data->data.split_u) * ptcl_inst_data->data.split_uv.y;
        }

        rend_elem->InitMesh(GLT_VAL, emit_inst, ptcl_inst_data, index, random);

        bool copy_mat = false;
        if (ptcl_inst_data->data.flags & PARTICLE_ROTATE_BY_EMITTER
            || ptcl_inst_data->data.draw_type == DIRECTION_EMITTER_ROTATION) {
            switch (emit_inst->data.type) {
            case EMITTER_CYLINDER:
                if (emit_inst->data.cylinder.direction != EMITTER_EMISSION_DIRECTION_NONE) {
                    float_t length;
                    vec3_length(rend_elem->direction, length);
                    copy_mat = length <= 0.000001f;
                }
                else
                    copy_mat = true;
                break;
            case EMITTER_SPHERE:
                if (emit_inst->data.sphere.direction != EMITTER_EMISSION_DIRECTION_NONE) {
                    float_t length;
                    vec3_length(rend_elem->direction, length);
                    copy_mat = length <= 0.000001f;
                }
                else
                    copy_mat = true;
                break;
            default:
                copy_mat = true;
                break;
            }
        }

        if (copy_mat) {
            rend_elem->mat = emit_inst->mat;
            if (ptcl_inst_data->data.flags & PARTICLE_EMITTER_LOCAL)
                mat4_clear_trans(&rend_elem->mat, &rend_elem->mat);
        }
        else
            rend_elem->mat = mat4_identity;

        rend_elem->InitLocusHistory(GLT_VAL, this, random);
        random->F2StepValue();
    }
    
    void F2ParticleInst::Free(bool free) {
        enum_or(data.flags, PARTICLE_INST_ENDED);
        if (free && data.render_group)
            data.render_group->Free();

        for (F2ParticleInst*& i : data.children)
            i->Free(free);
    }

    void F2ParticleInst::GetValue(GLT, RenderElement* rend_elem, float_t frame, Random* random) {
        float_t value = 0.0f;
        bool has_translation = false;
        vec3 translation = vec3_null;

        Animation* anim = &particle->animation;
        size_t length = anim->curves.size();
        for (int32_t i = 0; i < length; i++) {
            Curve* curve = anim->curves.data()[i];
            if (!curve->F2GetValue(GLT_VAL, rend_elem->frame,
                &value, rend_elem->random + i, random))
                continue;

            switch (curve->type) {
            case CURVE_TRANSLATION_X:
                translation.x = value;
                has_translation = true;
                break;
            case CURVE_TRANSLATION_Y:
                translation.y = value;
                has_translation = true;
                break;
            case CURVE_TRANSLATION_Z:
                translation.z = value;
                has_translation = true;
                break;
            case CURVE_ROTATION_X:
                rend_elem->rotation.x = value;
                break;
            case CURVE_ROTATION_Y:
                rend_elem->rotation.y = value;
                break;
            case CURVE_ROTATION_Z:
                rend_elem->rotation.z = value;
                break;
            case CURVE_SCALE_X:
                rend_elem->scale.x = value;
                break;
            case CURVE_SCALE_Y:
                rend_elem->scale.y = value;
                break;
            case CURVE_SCALE_Z:
                rend_elem->scale.z = value;
                break;
            case CURVE_SCALE_ALL:
                rend_elem->scale_all = value;
                break;
            case CURVE_COLOR_R:
                rend_elem->color.x = value;
                break;
            case CURVE_COLOR_G:
                rend_elem->color.y = value;
                break;
            case CURVE_COLOR_B:
                rend_elem->color.z = value;
                break;
            case CURVE_COLOR_A:
                rend_elem->color.w = value;
                if (value < 0.01f) {
                    rend_elem->disp = false;
                    return;
                }
                break;
            case CURVE_U_SCROLL:
                rend_elem->uv_scroll.x = value;
                break;
            case CURVE_V_SCROLL:
                rend_elem->uv_scroll.y = value;
                break;
            }
        }

        if (data.data.flags & PARTICLE_ROTATE_BY_EMITTER || has_translation) {
            vec3_add(translation, rend_elem->base_translation, translation);
            if (data.data.flags & PARTICLE_ROTATE_BY_EMITTER)
                mat4_mult_vec3_trans(&rend_elem->mat, &translation, &translation);
            rend_elem->translation = translation;
        }
    }

    bool F2ParticleInst::HasEnded(bool a2) {
        if (~data.flags & PARTICLE_INST_ENDED)
            return false;
        else if (!a2)
            return true;

        if (~data.flags & PARTICLE_INST_NO_CHILD || data.parent) {
            if (data.render_group && data.render_group->ctrl > 0)
                return false;
            return true;
        }

        for (F2ParticleInst*& i : data.children)
            if (!i->HasEnded(a2))
                return false;
        return true;
    }

    void F2ParticleInst::Reset() {
        data.flags = (ParticleInstFlag)0;
        if (data.render_group)
            data.render_group->Free();

        for (F2ParticleInst*& i : data.children)
            i->Reset();
    }

    void F2ParticleInst::StepUVParticle(GLT, RenderElement* rend_elem, float_t delta_frame, Random* random) {
        if (data.data.frame_step_uv <= 0.0f)
            return;

        int32_t max_uv = data.data.split_u * data.data.split_v;
        if (max_uv)
            max_uv--;

        while (rend_elem->frame_step_uv <= 0.0f) {
            switch (data.data.uv_index_type) {
            case UV_INDEX_RANDOM:
                rend_elem->uv_index = data.data.uv_index_start;
                if (data.data.uv_index_count > 1)
                    rend_elem->uv_index += random->F2GetInt(GLT_VAL, data.data.uv_index_count);
                break;
            case UV_INDEX_FORWARD:
            case UV_INDEX_INITIAL_RANDOM_FORWARD:
                rend_elem->uv_index = (uint8_t)(max_uv & (rend_elem->uv_index + 1));
                break;
            case UV_INDEX_REVERSE:
            case UV_INDEX_INITIAL_RANDOM_REVERSE:
                rend_elem->uv_index = (uint8_t)(max_uv & (rend_elem->uv_index - 1));
                break;
            }

            rend_elem->uv.x = (float_t)(rend_elem->uv_index % data.data.split_u) * data.data.split_uv.x;
            rend_elem->uv.y = (float_t)(rend_elem->uv_index / data.data.split_u) * data.data.split_uv.y;
            rend_elem->frame_step_uv += data.data.frame_step_uv;
        }
        rend_elem->frame_step_uv -= delta_frame;
    }

    XParticleInst::Data::Data() : data(), flags(),
        render_group(), random_ptr(), effect(), emitter(), parent(), particle() {

    }

    XParticleInst::Data::~Data() {

    }

    XParticleInst::XParticleInst(Particle* ptcl, XEffectInst* eff_inst,
        XEmitterInst* emit_inst, Random* random, float_t emission) {
        particle = ptcl;
        data.effect = eff_inst;
        data.emitter = emit_inst;
        data.particle = ptcl;
        data.data = ptcl->data;
        data.random_ptr = random;

        if (data.data.type == PARTICLE_QUAD || data.data.type == PARTICLE_MESH) {
            XRenderGroup* rend_group = new XRenderGroup(this);
            if (rend_group) {
                rend_group->alpha = eff_inst->GetAlpha();
                rend_group->fog = eff_inst->GetFog();

                if (data.data.draw_flags & PARTICLE_DRAW_NO_BILLBOARD_CULL)
                    rend_group->use_culling = false;
                else
                    rend_group->use_culling = true;

                if (data.data.emission >= min_emission)
                    rend_group->emission = data.data.emission;
                else if (eff_inst->data.emission >= min_emission)
                    rend_group->emission = eff_inst->data.emission;
                else
                    rend_group->emission = emission;
                data.render_group = rend_group;
                eff_inst->render_scene.Append(rend_group);
            }
        }
        else
            enum_or(data.flags, PARTICLE_INST_NO_CHILD);
    }

    XParticleInst::XParticleInst(XParticleInst* parent, float_t emission) {
        particle = parent->particle;
        data.effect = parent->data.effect;
        data.emitter = parent->data.emitter;
        data.parent = parent;

        data.random_ptr = parent->data.random_ptr;
        data.data = parent->data.data;

        data.particle = parent->data.particle;

        XRenderGroup* rend_group = new XRenderGroup(this);
        if (rend_group) {
            XEffectInst* effect = (XEffectInst*)parent->data.effect;
            rend_group->alpha = effect->GetAlpha();
            rend_group->fog = effect->GetFog();
            if (effect->data.emission >= min_emission)
                rend_group->emission = effect->data.emission;
            else
                rend_group->emission = emission;
            data.render_group = rend_group;
            effect->render_scene.Append(rend_group);
        }

        if (data.data.type == PARTICLE_LOCUS)
            enum_or(data.flags, PARTICLE_INST_NO_CHILD);
    }

    XParticleInst::~XParticleInst() {
        for (XParticleInst*& i : data.children)
            delete i;

        if (data.render_group) {
            data.render_group->DeleteBuffers(true);
            data.render_group = 0;
        }
    }

    void XParticleInst::AccelerateParticle(RenderElement* rend_elem,
        float_t delta_frame, Random* random) {
        rend_elem->translation_prev = rend_elem->translation;
        float_t time = rend_elem->frame * (float_t)(1.0 / 60.0);
        float_t diff_time = time - rend_elem->rebound_time - (float_t)(1.0 / 60.0);
        float_t speed = rend_elem->speed * delta_frame;
        diff_time = max(diff_time, 0.0f) * (float_t)(1.0 / 60.0);

        vec3 acceleration;
        vec3_mult_scalar(rend_elem->acceleration,
            (diff_time + (float_t)(1.0 / 7200.0)) * delta_frame, acceleration);
        if (speed > 0.0f) {
            vec3 direction;
            vec3_mult_scalar(rend_elem->direction, speed, direction);
            vec3_add(acceleration, direction, acceleration);
        }

        vec3_add(rend_elem->translation, acceleration, rend_elem->translation);
        vec3_add(rend_elem->base_translation, acceleration, rend_elem->base_translation);

        if (data.data.flags & PARTICLE_REBOUND_PLANE
            && rend_elem->translation_prev.y > data.data.rebound_plane_y
            && rend_elem->translation.y <= data.data.rebound_plane_y) {
            float_t reflection_coeff = random->XGetFloat(
                data.data.reflection_coeff_random) + data.data.reflection_coeff;
            rend_elem->rebound_time = time;

            vec3 direction;
            vec3_sub(rend_elem->translation, rend_elem->translation_prev, direction);
            if (delta_frame > 0.0f)
                vec3_div_scalar(direction, delta_frame, direction);
            vec3_mult_scalar(direction, reflection_coeff / rend_elem->speed, direction);
            const vec3 reverse_y_dir = { 0.0f, -0.0f, 0.0f };
            vec3_xor(direction, reverse_y_dir, rend_elem->direction);

            rend_elem->translation.y = rend_elem->translation_prev.y;
        }

        speed = rend_elem->base_speed + (float_t)(1.0 / 7200.0)
            * rend_elem->deceleration - diff_time * rend_elem->deceleration;
        rend_elem->speed = max(speed, 0.0f);
    }

    void XParticleInst::Copy(XParticleInst* dst, float_t emission) {

    }

    void XParticleInst::Emit(int32_t dup_count, int32_t count, float_t emission) {
        if (data.flags & PARTICLE_INST_ENDED)
            return;

        XParticleInst* ptcl = this;
        while (!ptcl->data.parent && ptcl->data.flags & PARTICLE_INST_NO_CHILD) {
            XParticleInst* particle = new XParticleInst(ptcl, emission);
            if (particle)
                ptcl->data.children.push_back(particle);
            else
                return;

            ptcl = particle;
            if (particle->data.flags & PARTICLE_INST_ENDED)
                return;
        }

        if (ptcl->data.render_group)
            ptcl->data.render_group->Emit(&ptcl->data, ptcl->data.emitter, dup_count, count);
    }

    void XParticleInst::EmitParticle(RenderElement* rend_elem, XEmitterInst* emit_inst,
        XParticleInst::Data* ptcl_inst_data, int32_t index, uint8_t step, Random* random) {
        rend_elem->random = random->GetValue();
        rend_elem->frame = 0.0f;
        rend_elem->rebound_time = 0.0f;
        rend_elem->uv = vec2_null;
        rend_elem->uv_index = ptcl_inst_data->data.uv_index;

        rend_elem->step = step;
        rend_elem->fade_in_frames = random->XGetFloat((float_t)ptcl_inst_data->data.fade_in_random)
            + (float_t)ptcl_inst_data->data.fade_in;
        if (rend_elem->fade_in_frames < 0.0f)
            rend_elem->fade_in_frames = 0.0f;

        rend_elem->fade_out_frames = random->XGetFloat((float_t)ptcl_inst_data->data.fade_out_random)
            + (float_t)ptcl_inst_data->data.fade_out;
        if (rend_elem->fade_out_frames < 0.0f)
            rend_elem->fade_out_frames = 0.0f;

        rend_elem->life_time = random->XGetFloat((float_t)ptcl_inst_data->data.life_time_random)
            + (float_t)ptcl_inst_data->data.life_time;
        if (rend_elem->life_time < 0.0f)
            rend_elem->life_time = 0.0f;

        rend_elem->life_time += rend_elem->fade_in_frames + rend_elem->fade_out_frames;

        rend_elem->color = ptcl_inst_data->data.color;
        if (ptcl_inst_data->data.draw_type == DIRECTION_PARTICLE_ROTATION
            || ptcl_inst_data->data.type == PARTICLE_MESH) {
            rend_elem->rotation.x = random->XGetFloat(ptcl_inst_data->data.rotation_random.x)
                + ptcl_inst_data->data.rotation.x;
            rend_elem->rotation_add.x = random->XGetFloat(ptcl_inst_data->data.rotation_add_random.x)
                + ptcl_inst_data->data.rotation_add.x;
            rend_elem->rotation.y = random->XGetFloat(ptcl_inst_data->data.rotation_random.y)
                + ptcl_inst_data->data.rotation.y;
            rend_elem->rotation_add.y = random->XGetFloat(ptcl_inst_data->data.rotation_add_random.y)
                + ptcl_inst_data->data.rotation_add.y;
        }
        else {
            rend_elem->rotation.x = 0.0f;
            rend_elem->rotation.y = 0.0f;
            rend_elem->rotation_add.x = 0.0f;
            rend_elem->rotation_add.y = 0.0f;
        }
        rend_elem->rotation.z = random->XGetFloat(ptcl_inst_data->data.rotation_random.z)
            + ptcl_inst_data->data.rotation.z;
        rend_elem->rotation_add.z = random->XGetFloat(ptcl_inst_data->data.rotation_add_random.z)
            + ptcl_inst_data->data.rotation_add.z;
        rend_elem->uv_scroll = vec2_null;
        rend_elem->uv_scroll_2nd = vec2_null;
        rend_elem->scale = vec3_identity;
        rend_elem->scale_all = 1.0f;
        rend_elem->frame_step_uv = (float_t)ptcl_inst_data->data.frame_step_uv;

        rend_elem->scale_particle.x = random->XGetFloat(ptcl_inst_data->data.scale_random.x)
            + ptcl_inst_data->data.scale.x;
        if (ptcl_inst_data->data.flags & PARTICLE_SCALE_Y_BY_X) {
            rend_elem->scale_particle.y = rend_elem->scale_particle.x;
            random->XStepValue();
        }
        else
            rend_elem->scale_particle.y = random->XGetFloat(ptcl_inst_data->data.scale_random.y)
            + ptcl_inst_data->data.scale.y;

        int32_t max_uv = ptcl_inst_data->data.split_u * ptcl_inst_data->data.split_v;
        if (max_uv > 1 && ptcl_inst_data->data.uv_index_count > 1) {
            switch (ptcl_inst_data->data.uv_index_type) {
            case UV_INDEX_INITIAL_RANDOM_FIXED:
            case UV_INDEX_INITIAL_RANDOM_FORWARD:
            case UV_INDEX_INITIAL_RANDOM_REVERSE:
                rend_elem->uv_index += random->XGetInt(ptcl_inst_data->data.uv_index_count);
                break;
            }
            rend_elem->uv_index = min(rend_elem->uv_index, ptcl_inst_data->data.uv_index_end);
            rend_elem->uv.x = (float_t)(rend_elem->uv_index
                % ptcl_inst_data->data.split_u) * ptcl_inst_data->data.split_uv.x;
            rend_elem->uv.y = (float_t)(rend_elem->uv_index
                / ptcl_inst_data->data.split_u) * ptcl_inst_data->data.split_uv.y;
        }

        rend_elem->InitMesh(emit_inst, ptcl_inst_data, index, random);

        bool copy_mat = false;
        if (ptcl_inst_data->data.flags & PARTICLE_ROTATE_BY_EMITTER
            || ptcl_inst_data->data.draw_type == DIRECTION_EMITTER_ROTATION) {
            switch (emit_inst->data.type) {
            case EMITTER_CYLINDER:
                if (emit_inst->data.cylinder.direction != EMITTER_EMISSION_DIRECTION_NONE) {
                    float_t length;
                    vec3_length(rend_elem->direction, length);
                    copy_mat = length <= 0.000001f;
                }
                else
                    copy_mat = true;
                break;
            case EMITTER_SPHERE:
                if (emit_inst->data.sphere.direction != EMITTER_EMISSION_DIRECTION_NONE) {
                    float_t length;
                    vec3_length(rend_elem->direction, length);
                    copy_mat = length <= 0.000001f;
                }
                else
                    copy_mat = true;
                break;
            default:
                copy_mat = true;
                break;
            }
        }

        if (copy_mat) {
            rend_elem->mat = emit_inst->mat;
            if (ptcl_inst_data->data.flags & PARTICLE_EMITTER_LOCAL)
                mat4_clear_trans(&rend_elem->mat, &rend_elem->mat);
        }
        else
            rend_elem->mat = mat4_identity;

        if (ptcl_inst_data->data.type == PARTICLE_LOCUS) {
            uint32_t locus_history_size = random->XGetInt(
                -ptcl_inst_data->data.locus_history_size_random,
                ptcl_inst_data->data.locus_history_size_random)
                + ptcl_inst_data->data.locus_history_size;
            rend_elem->locus_history = new LocusHistory(locus_history_size);
        }
        random->XStepValue();
    }

    void XParticleInst::GetValue(RenderElement* rend_elem, float_t frame, Random* random, float_t* color_scale) {
        float_t value = 0.0f;
        vec3 translation = vec3_null;
        bool has_translation = false;

        Animation* anim = &particle->animation;
        size_t length = anim->curves.size();
        for (int32_t i = 0; i < length; i++) {
            Curve* curve = anim->curves.data()[i];
            if (!curve->XGetValue(rend_elem->frame, &value, rend_elem->random + i, random))
                continue;

            switch (curve->type) {
            case CURVE_TRANSLATION_X:
                translation.x = value;
                has_translation = true;
                break;
            case CURVE_TRANSLATION_Y:
                translation.y = value;
                has_translation = true;
                break;
            case CURVE_TRANSLATION_Z:
                translation.z = value;
                has_translation = true;
                break;
            case CURVE_ROTATION_X:
                rend_elem->rotation.x = value;
                break;
            case CURVE_ROTATION_Y:
                rend_elem->rotation.y = value;
                break;
            case CURVE_ROTATION_Z:
                rend_elem->rotation.z = value;
                break;
            case CURVE_SCALE_X:
                rend_elem->scale.x = value;
                break;
            case CURVE_SCALE_Y:
                rend_elem->scale.y = value;
                break;
            case CURVE_SCALE_Z:
                rend_elem->scale.z = value;
                break;
            case CURVE_SCALE_ALL:
                rend_elem->scale_all = value;
                break;
            case CURVE_COLOR_R:
                rend_elem->color.x = value;
                break;
            case CURVE_COLOR_G:
                rend_elem->color.y = value;
                break;
            case CURVE_COLOR_B:
                rend_elem->color.z = value;
                break;
            case CURVE_COLOR_A:
                rend_elem->color.w = value;
                if (value < 0.01f)
                    rend_elem->disp = false;
                break;
            case CURVE_COLOR_RGB_SCALE:
                *color_scale = value;
                break;
            case CURVE_U_SCROLL:
                rend_elem->uv_scroll.x = value;
                break;
            case CURVE_V_SCROLL:
                rend_elem->uv_scroll.y = value;
                break;
            case CURVE_U_SCROLL_2ND:
                rend_elem->uv_scroll_2nd.x = value;
                break;
            case CURVE_V_SCROLL_2ND:
                rend_elem->uv_scroll_2nd.y = value;
                break;
            }
        }

        if (data.data.flags & PARTICLE_ROTATE_BY_EMITTER || has_translation) {
            vec3_add(translation, rend_elem->base_translation, translation);
            if (data.data.flags & PARTICLE_ROTATE_BY_EMITTER)
                mat4_mult_vec3_trans(&rend_elem->mat, &translation, &translation);
            rend_elem->translation = translation;
        }
    }

    void XParticleInst::Free(bool free) {
        enum_or(data.flags, PARTICLE_INST_ENDED);
        if (free && data.render_group)
            data.render_group->Free();

        for (XParticleInst*& i : data.children)
            i->Free(free);
    }

    bool XParticleInst::HasEnded(bool a2) {
        if (~data.flags & PARTICLE_INST_ENDED)
            return false;
        else if (!a2)
            return true;

        if (~data.flags & PARTICLE_INST_NO_CHILD || data.parent) {
            if (data.render_group && data.render_group->ctrl > 0)
                return false;
            return true;
        }

        for (XParticleInst*& i : data.children)
            if (!i->HasEnded(a2))
                return false;
        return true;
    }

    void XParticleInst::Reset() {
        data.flags = (ParticleInstFlag)0;
        if (data.render_group)
            data.render_group->Free();

        for (XParticleInst*& i : data.children)
            i->Reset();
    }

    void XParticleInst::StepUVParticle(RenderElement* rend_elem, float_t delta_frame, Random* random) {
        if (data.data.frame_step_uv <= 0.0f)
            return;

        int32_t max_uv = data.data.split_u * data.data.split_v;
        if (max_uv)
            max_uv--;

        while (rend_elem->frame_step_uv <= 0.0f) {
            switch (data.data.uv_index_type) {
            case UV_INDEX_RANDOM:
                rend_elem->uv_index = data.data.uv_index_start;
                if (data.data.uv_index_count > 1)
                    rend_elem->uv_index += random->XGetInt(data.data.uv_index_count);
                break;
            case UV_INDEX_FORWARD:
            case UV_INDEX_INITIAL_RANDOM_FORWARD:
                rend_elem->uv_index = (uint8_t)(max_uv & (rend_elem->uv_index + 1));
                break;
            case UV_INDEX_REVERSE:
            case UV_INDEX_INITIAL_RANDOM_REVERSE:
                rend_elem->uv_index = (uint8_t)(max_uv & (rend_elem->uv_index - 1));
                break;
            }

            rend_elem->uv.x = (float_t)(rend_elem->uv_index % data.data.split_u) * data.data.split_uv.x;
            rend_elem->uv.y = (float_t)(rend_elem->uv_index / data.data.split_u) * data.data.split_uv.y;
            rend_elem->frame_step_uv += data.data.frame_step_uv;
        }
        rend_elem->frame_step_uv -= delta_frame;
    }
}
