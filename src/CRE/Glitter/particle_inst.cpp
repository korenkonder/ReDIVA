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

    ParticleInstF2::Data::Data() : data(), flags(),
        render_group(), random_ptr(), effect(), emitter(), parent(), particle() {
    }

    ParticleInstF2::Data::~Data() {

    }

    ParticleInstF2::ParticleInstF2(Particle* ptcl, EffectInstF2* eff_inst,
        EmitterInstF2* emit_inst, Random* random, float_t emission) {
        particle = ptcl;
        data.effect = eff_inst;
        data.emitter = emit_inst;
        data.particle = ptcl;
        data.data = ptcl->data;
        data.random_ptr = random;

        if (data.data.type != PARTICLE_LOCUS) {
            RenderGroupF2* rend_group = new RenderGroupF2(this);
            if (rend_group) {
                rend_group->disp_type = eff_inst->GetDispType();
                rend_group->fog_type = eff_inst->GetFog();

                if (data.data.type == PARTICLE_QUAD
                    && data.data.blend_mode == PARTICLE_BLEND_PUNCH_THROUGH)
                    rend_group->disp_type = DISP_OPAQUE;

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
            enum_or(data.flags, PARTICLE_INST_LOCUS);
    }

    ParticleInstF2::ParticleInstF2(ParticleInstF2* parent, float_t emission) {
        particle = parent->particle;
        data.effect = parent->data.effect;
        data.emitter = parent->data.emitter;
        data.parent = parent;

        data.random_ptr = parent->data.random_ptr;
        data.data = parent->data.data;

        data.particle = parent->data.particle;

        RenderGroupF2* rend_group = new RenderGroupF2(this);
        if (rend_group) {
            EffectInstF2* effect = parent->data.effect;
            rend_group->disp_type = effect->GetDispType();
            rend_group->fog_type = effect->GetFog();

            if (data.data.blend_mode == PARTICLE_BLEND_PUNCH_THROUGH
                && data.data.type == PARTICLE_QUAD)
                rend_group->disp_type = DISP_OPAQUE;

            if (effect->data.emission >= min_emission)
                rend_group->emission = effect->data.emission;
            else
                rend_group->emission = emission;

            data.render_group = rend_group;
            effect->render_scene.Append(rend_group);
        }

        if (data.data.type == PARTICLE_LOCUS)
            enum_or(data.flags, PARTICLE_INST_LOCUS);
    }

    ParticleInstF2::~ParticleInstF2() {
        if (data.render_group) {
            data.render_group->DeleteBuffers(true);
            data.render_group = 0;
        }

        for (ParticleInstF2*& i : data.children)
            if (i) {
                delete i;
                i = 0;
            }
    }

    void ParticleInstF2::AccelerateParticle(GLT, RenderElement* rend_elem,
        float_t time, float_t delta_frame, Random* random) {
        rend_elem->translation_prev = rend_elem->translation;
        float_t delta_time = delta_frame * (float_t)(1.0 / 60.0);
        float_t diff_time = time - rend_elem->rebound_time - delta_time;

        vec3 acceleration = rend_elem->acceleration * (delta_time * (delta_time * 0.5f + diff_time));
        float_t speed = rend_elem->deceleration * (delta_time * (delta_time * 0.5f - diff_time)) + rend_elem->speed;
        if (speed >= 0.01f)
            acceleration += rend_elem->direction * (speed * delta_time);

        rend_elem->translation += acceleration;
        rend_elem->base_translation += acceleration;

        if (data.data.flags & PARTICLE_REBOUND_PLANE
            && rend_elem->translation_prev.y > data.data.rebound_plane_y
            && rend_elem->translation.y <= data.data.rebound_plane_y) {
            float_t reflection_coeff = (random->F2GetFloat(GLT_VAL,
                data.data.reflection_coeff_random) + data.data.reflection_coeff) * 60.0f;
            rend_elem->rebound_time = time;

            vec3 direction = rend_elem->translation - rend_elem->translation_prev;
            const vec3 reverse_y_dir = { 0.0f, -0.0f, 0.0f };
            rend_elem->direction = (direction * reflection_coeff) ^ reverse_y_dir;

            rend_elem->translation.y = rend_elem->translation_prev.y;
        }
    }

    void ParticleInstF2::Copy(ParticleInstF2* dst, float_t emission) {
        dst->data.flags = data.flags;
        if (data.render_group && dst->data.render_group)
            data.render_group->Copy(dst->data.render_group);

        for (ParticleInstF2*& i : dst->data.children)
            if (i) {
                delete i;
                i = 0;
            }
        dst->data.children.clear();

        for (ParticleInstF2*& i : data.children)
            if (i) {
                ParticleInstF2* child = new ParticleInstF2(this, emission);
                dst->data.children.push_back(child);
                i->Copy(child, emission);
            }
    }

    void ParticleInstF2::Emit(GPM, GLT, int32_t dup_count, int32_t count, float_t emission) {
        if (data.flags & PARTICLE_INST_ENDED)
            return;

        ParticleInstF2* ptcl = this;
        while (!ptcl->data.parent && ptcl->data.flags & PARTICLE_INST_LOCUS) {
            ParticleInstF2* particle = new ParticleInstF2(ptcl, emission);
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
                &ptcl->data.data, ptcl->data.emitter, dup_count, count);
    }

    void ParticleInstF2::EmitParticle(GPM, GLT, RenderElement* rend_elem, EmitterInstF2* emit_inst,
        Particle::Data* ptcl_data, int32_t index, Random* random) {
        counter.Increment();
        random->SetValue(counter.GetValue());
        rend_elem->random = random->F2GetInt(GLT_VAL, Random::F2GetMax(GLT_VAL));
        rend_elem->frame = 0.0f;
        rend_elem->rebound_time = 0.0f;
        rend_elem->uv = 0.0f;
        rend_elem->uv_index = ptcl_data->uv_index;

        rend_elem->fade_in_frames = 0.0f;
        rend_elem->fade_out_frames = 0.0f;
        rend_elem->life_time = (float_t)ptcl_data->life_time;

        rend_elem->color = ptcl_data->color;
        if (ptcl_data->draw_type == DIRECTION_PARTICLE_ROTATION) {
            rend_elem->rotation.x = random->F2GetFloat(GLT_VAL, ptcl_data->rotation_random.x)
                + ptcl_data->rotation.x;
            rend_elem->rotation_add.x = random->F2GetFloat(GLT_VAL, ptcl_data->rotation_add_random.x)
                + ptcl_data->rotation_add.x;
            rend_elem->rotation.y = random->F2GetFloat(GLT_VAL, ptcl_data->rotation_random.y)
                + ptcl_data->rotation.y;
            rend_elem->rotation_add.y = random->F2GetFloat(GLT_VAL, ptcl_data->rotation_add_random.y)
                + ptcl_data->rotation_add.y;
        }
        else {
            rend_elem->rotation.x = 0.0f;
            rend_elem->rotation.y = 0.0f;
            rend_elem->rotation_add.x = 0.0f;
            rend_elem->rotation_add.y = 0.0f;
        }
        rend_elem->rot_z_cos = 1.0f;
        rend_elem->rot_z_sin = 0.0f;
        rend_elem->rotation.z = random->F2GetFloat(GLT_VAL, ptcl_data->rotation_random.z)
            + ptcl_data->rotation.z;
        rend_elem->rotation_add.z = random->F2GetFloat(GLT_VAL, ptcl_data->rotation_add_random.z)
            + ptcl_data->rotation_add.z;
        rend_elem->uv_scroll = 0.0f;
        rend_elem->uv_scroll_2nd = 0.0f;
        rend_elem->scale = 1.0f;
        rend_elem->scale_all = 1.0f;
        rend_elem->frame_step_uv = (float_t)ptcl_data->frame_step_uv;

        rend_elem->scale_particle.x = random->F2GetFloat(GLT_VAL, ptcl_data->scale_random.x)
            + ptcl_data->scale.x;
        if (ptcl_data->flags & PARTICLE_SCALE_Y_BY_X)
            rend_elem->scale_particle.y = rend_elem->scale_particle.x;
        else
            rend_elem->scale_particle.y = random->F2GetFloat(GLT_VAL, ptcl_data->scale_random.y)
                + ptcl_data->scale.y;

        int32_t max_uv = ptcl_data->split_u * ptcl_data->split_v;
        if (max_uv > 1 && ptcl_data->uv_index_count > 1) {
            switch (ptcl_data->uv_index_type) {
            case UV_INDEX_INITIAL_RANDOM_FIXED:
            case UV_INDEX_INITIAL_RANDOM_FORWARD:
            case UV_INDEX_INITIAL_RANDOM_REVERSE:
                rend_elem->uv_index = ptcl_data->uv_index_start
                    + random->F2GetInt(GLT_VAL, ptcl_data->uv_index_count);
                break;
            }
            rend_elem->uv_index = min_def(rend_elem->uv_index, ptcl_data->uv_index_end);
            rend_elem->uv.x = (float_t)(rend_elem->uv_index
                % ptcl_data->split_u) * ptcl_data->split_uv.x;
            rend_elem->uv.y = (float_t)(rend_elem->uv_index
                / ptcl_data->split_u) * ptcl_data->split_uv.y;
        }

        rend_elem->InitMesh(GLT_VAL, emit_inst, ptcl_data, index, random);

        bool copy_mat = false;
        if (ptcl_data->flags & PARTICLE_ROTATE_BY_EMITTER
            || ptcl_data->draw_type == DIRECTION_EMITTER_ROTATION) {
            switch (emit_inst->data.type) {
            case EMITTER_CYLINDER:
                if (emit_inst->data.cylinder.direction != EMITTER_EMISSION_DIRECTION_NONE)
                    copy_mat = vec3::length(rend_elem->direction) <= 0.000001f;
                else
                    copy_mat = true;
                break;
            case EMITTER_SPHERE:
                if (emit_inst->data.sphere.direction != EMITTER_EMISSION_DIRECTION_NONE)
                    copy_mat = vec3::length(rend_elem->direction) <= 0.000001f;
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
            if (ptcl_data->flags & PARTICLE_EMITTER_LOCAL)
                mat4_clear_trans(&rend_elem->mat, &rend_elem->mat);
        }
        else
            rend_elem->mat = mat4_identity;

        rend_elem->InitLocusHistory(GLT_VAL, this, random);
        random->F2StepValue();
    }

    void ParticleInstF2::Free(bool free) {
        enum_or(data.flags, PARTICLE_INST_ENDED);
        if (free && data.render_group)
            data.render_group->Free();

        for (ParticleInstF2*& i : data.children)
            i->Free(free);
    }

    void ParticleInstF2::GetColor(Glitter::RenderElement* rend_elem) {
        float_t r = rend_elem->color.x;
        if (r < 0.0f)
            r = data.data.color.x;

        float_t g = rend_elem->color.y;
        if (g < 0.0f)
            g = data.data.color.y;

        float_t b = rend_elem->color.z;
        if (b < 0.0f)
            b = data.data.color.z;

        float_t a = rend_elem->color.w;
        if (a < 0.0f)
            a = data.data.color.w;

        GetExtColor(r, g, b, a);

        rend_elem->color.x = r;
        rend_elem->color.y = g;
        rend_elem->color.z = b;
        rend_elem->color.w = a;

        if (a < 0.01f)
            rend_elem->disp = false;
    }

    bool ParticleInstF2::GetExtAnimScale(vec3* ext_anim_scale, float_t* ext_scale) {
        if (data.effect)
            return data.effect->GetExtAnimScale(ext_anim_scale, ext_scale);
        else if (data.parent && data.parent->data.effect)
            return data.parent->data.effect->GetExtAnimScale(ext_anim_scale, ext_scale);
        else
            return false;
    }

    void ParticleInstF2::GetExtColor(float_t& r, float_t& g, float_t& b, float_t& a) {
        if (data.effect)
            data.effect->GetExtColor(r, g, b, a);
        else if (data.parent && data.parent->data.effect)
            data.parent->data.effect->GetExtColor(r, g, b, a);
    }

    bool ParticleInstF2::GetValue(GLT, RenderElement* rend_elem, float_t frame, Random* random) {
        float_t value = 0.0f;
        bool has_translation = false;
        vec3 translation = 0.0f;

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
                    return false;
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
            translation += rend_elem->base_translation;
            if (data.data.flags & PARTICLE_ROTATE_BY_EMITTER)
                mat4_transform_point(&rend_elem->mat, &translation, &translation);
            rend_elem->translation = translation;
        }
        return true;
    }

    bool ParticleInstF2::HasEnded(bool a2) {
        if (!(data.flags & PARTICLE_INST_ENDED))
            return false;
        else if (!a2)
            return true;

        if (!(data.flags & PARTICLE_INST_LOCUS) || data.parent) {
            if (data.render_group && data.render_group->ctrl > 0)
                return false;
            return true;
        }

        for (ParticleInstF2*& i : data.children)
            if (!i->HasEnded(a2))
                return false;
        return true;
    }

    void ParticleInstF2::RenderGroupCtrl(GLT, float_t delta_frame) {
        if (data.parent || !(data.flags & PARTICLE_INST_LOCUS)) {
            if (data.render_group)
                data.render_group->Ctrl(GLT_VAL, delta_frame, true);
            return;
        }

        for (ParticleInstF2*& i : data.children)
            i->RenderGroupCtrl(GLT_VAL, delta_frame);
    }

    void ParticleInstF2::Reset() {
        data.flags = (ParticleInstFlag)0;
        if (data.render_group)
            data.render_group->Free();

        for (ParticleInstF2*& i : data.children)
            i->Reset();
    }

    void ParticleInstF2::StepUVParticle(GLT, RenderElement* rend_elem, float_t delta_frame, Random* random) {
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

    ParticleInstX::Data::Data() : data(), flags(),
        render_group(), random_ptr(), effect(), emitter(), parent(), particle() {

    }

    ParticleInstX::Data::~Data() {

    }

    ParticleInstX::ParticleInstX(Particle* ptcl, EffectInstX* eff_inst,
        EmitterInstX* emit_inst, Random* random, float_t emission) {
        particle = ptcl;
        data.effect = eff_inst;
        data.emitter = emit_inst;
        data.particle = ptcl;
        data.data = ptcl->data;
        data.random_ptr = random;

        if (data.data.type == PARTICLE_QUAD || data.data.type == PARTICLE_MESH) {
            RenderGroupX* rend_group = new RenderGroupX(this);
            if (rend_group) {
                rend_group->disp_type = eff_inst->GetDispType();
                rend_group->fog_type = eff_inst->GetFog();

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
            enum_or(data.flags, PARTICLE_INST_LOCUS);
    }

    ParticleInstX::ParticleInstX(ParticleInstX* parent, float_t emission) {
        particle = parent->particle;
        data.effect = parent->data.effect;
        data.emitter = parent->data.emitter;
        data.parent = parent;

        data.random_ptr = parent->data.random_ptr;
        data.data = parent->data.data;

        data.particle = parent->data.particle;

        RenderGroupX* rend_group = new RenderGroupX(this);
        if (rend_group) {
            EffectInstX* effect = parent->data.effect;
            rend_group->disp_type = effect->GetDispType();
            rend_group->fog_type = effect->GetFog();

            if (effect->data.emission >= min_emission)
                rend_group->emission = effect->data.emission;
            else
                rend_group->emission = emission;

            data.render_group = rend_group;
            effect->render_scene.Append(rend_group);
        }

        if (data.data.type == PARTICLE_LOCUS)
            enum_or(data.flags, PARTICLE_INST_LOCUS);
    }

    ParticleInstX::~ParticleInstX() {
        if (data.render_group) {
            data.render_group->DeleteBuffers(true);
            data.render_group = 0;
        }

        for (ParticleInstX*& i : data.children)
            if (i) {
                delete i;
                i = 0;
            }
    }

    void ParticleInstX::AccelerateParticle(RenderElement* rend_elem,
        float_t delta_frame, Random* random) {
        rend_elem->translation_prev = rend_elem->translation;
        float_t time = rend_elem->frame * (float_t)(1.0 / 60.0);
        float_t diff_time = time - rend_elem->rebound_time - (float_t)(1.0 / 60.0);
        float_t speed = rend_elem->speed * delta_frame;
        diff_time = max_def(diff_time, 0.0f) * (float_t)(1.0 / 60.0);

        vec3 acceleration = rend_elem->acceleration * ((diff_time + (float_t)(1.0 / 7200.0)) * delta_frame);
        if (speed > 0.0f)
            acceleration += rend_elem->direction * speed;

        rend_elem->translation += acceleration;
        rend_elem->base_translation += acceleration;

        if (data.data.flags & PARTICLE_REBOUND_PLANE
            && rend_elem->translation_prev.y > data.data.rebound_plane_y
            && rend_elem->translation.y <= data.data.rebound_plane_y) {
            float_t reflection_coeff = (random->XGetFloat(
                data.data.reflection_coeff_random) + data.data.reflection_coeff) / rend_elem->speed;
            rend_elem->rebound_time = time;

            vec3 direction = rend_elem->translation - rend_elem->translation_prev;
            if (delta_frame > 0.0f)
                direction /= delta_frame;
            const vec3 reverse_y_dir = { 0.0f, -0.0f, 0.0f };
            rend_elem->direction = (direction * reflection_coeff) ^ reverse_y_dir;

            rend_elem->translation.y = rend_elem->translation_prev.y;
        }

        speed = rend_elem->base_speed + (float_t)(1.0 / 7200.0)
            * rend_elem->deceleration - diff_time * rend_elem->deceleration;
        rend_elem->speed = max_def(speed, 0.0f);
    }

    bool ParticleInstX::CheckUseCamera() {
        Particle* ptcl = data.particle;
        if (!ptcl)
            return false;

        switch (ptcl->data.draw_type) {
        case DIRECTION_BILLBOARD:
        case DIRECTION_BILLBOARD_Y_AXIS:
            return true;
        }

        if (ptcl->data.type != PARTICLE_QUAD || fabsf(ptcl->data.z_offset) <= 0.000001f)
            return false;

        switch (ptcl->data.draw_type) {
        case DIRECTION_BILLBOARD:
        case DIRECTION_EMITTER_DIRECTION:
        case DIRECTION_Y_AXIS:
        case DIRECTION_X_AXIS:
        case DIRECTION_Z_AXIS:
        case DIRECTION_BILLBOARD_Y_AXIS:
        case DIRECTION_EMITTER_ROTATION:
        case DIRECTION_EFFECT_ROTATION:
        case DIRECTION_PARTICLE_ROTATION:
            return true;
        }
        return false;
    }

    void ParticleInstX::Copy(ParticleInstX* dst, float_t emission) {
        dst->data.flags = data.flags;
        if (data.render_group && dst->data.render_group)
            data.render_group->Copy(dst->data.render_group);

        for (ParticleInstX*& i : dst->data.children)
            if (i) {
                delete i;
                i = 0;
            }
        dst->data.children.clear();

        for (ParticleInstX*& i : data.children)
            if (i) {
                ParticleInstX* child = new ParticleInstX(this, emission);
                dst->data.children.push_back(child);
                i->Copy(child, emission);
            }
    }

    void ParticleInstX::Emit(int32_t dup_count, int32_t count, float_t emission, float_t frame) {
        if (data.flags & PARTICLE_INST_ENDED)
            return;

        ParticleInstX* ptcl = this;
        while (!ptcl->data.parent && ptcl->data.flags & PARTICLE_INST_LOCUS) {
            ParticleInstX* particle = new ParticleInstX(ptcl, emission);
            if (particle)
                ptcl->data.children.push_back(particle);
            else
                return;

            ptcl = particle;
            if (particle->data.flags & PARTICLE_INST_ENDED)
                return;
        }

        if (ptcl->data.render_group)
            ptcl->data.render_group->Emit(&ptcl->data.data, ptcl->data.emitter, dup_count, count, frame);
    }

    void ParticleInstX::EmitParticle(RenderElement* rend_elem, EmitterInstX* emit_inst,
        Particle::Data* ptcl_data, int32_t index, uint8_t step, Random* random) {
        rend_elem->random = random->GetValue();
        rend_elem->frame = 0.0f;
        rend_elem->rebound_time = 0.0f;
        rend_elem->uv = 0.0f;
        rend_elem->uv_index = ptcl_data->uv_index;

        rend_elem->step = step;
        rend_elem->fade_in_frames = random->XGetFloat((float_t)ptcl_data->fade_in_random)
            + (float_t)ptcl_data->fade_in;
        if (rend_elem->fade_in_frames < 0.0f)
            rend_elem->fade_in_frames = 0.0f;

        rend_elem->fade_out_frames = random->XGetFloat((float_t)ptcl_data->fade_out_random)
            + (float_t)ptcl_data->fade_out;
        if (rend_elem->fade_out_frames < 0.0f)
            rend_elem->fade_out_frames = 0.0f;

        rend_elem->life_time = random->XGetFloat((float_t)ptcl_data->life_time_random)
            + (float_t)ptcl_data->life_time;
        if (rend_elem->life_time < 0.0f)
            rend_elem->life_time = 0.0f;

        rend_elem->life_time += rend_elem->fade_in_frames + rend_elem->fade_out_frames;

        rend_elem->color = ptcl_data->color;
        if (ptcl_data->draw_type == DIRECTION_PARTICLE_ROTATION
            || ptcl_data->type == PARTICLE_MESH) {
            rend_elem->rotation.x = random->XGetFloat(ptcl_data->rotation_random.x)
                + ptcl_data->rotation.x;
            rend_elem->rotation_add.x = random->XGetFloat(ptcl_data->rotation_add_random.x)
                + ptcl_data->rotation_add.x;
            rend_elem->rotation.y = random->XGetFloat(ptcl_data->rotation_random.y)
                + ptcl_data->rotation.y;
            rend_elem->rotation_add.y = random->XGetFloat(ptcl_data->rotation_add_random.y)
                + ptcl_data->rotation_add.y;
        }
        else {
            rend_elem->rotation.x = 0.0f;
            rend_elem->rotation.y = 0.0f;
            rend_elem->rotation_add.x = 0.0f;
            rend_elem->rotation_add.y = 0.0f;
        }
        rend_elem->rot_z_cos = 1.0f;
        rend_elem->rot_z_sin = 0.0f;
        rend_elem->rotation.z = random->XGetFloat(ptcl_data->rotation_random.z)
            + ptcl_data->rotation.z;
        rend_elem->rotation_add.z = random->XGetFloat(ptcl_data->rotation_add_random.z)
            + ptcl_data->rotation_add.z;
        rend_elem->uv_scroll = 0.0f;
        rend_elem->uv_scroll_2nd = 0.0f;
        rend_elem->scale = 1.0f;
        rend_elem->scale_all = 1.0f;
        rend_elem->frame_step_uv = (float_t)ptcl_data->frame_step_uv;

        rend_elem->scale_particle.x = random->XGetFloat(ptcl_data->scale_random.x)
            + ptcl_data->scale.x;
        if (ptcl_data->flags & PARTICLE_SCALE_Y_BY_X) {
            rend_elem->scale_particle.y = rend_elem->scale_particle.x;
            random->XStepValue();
        }
        else
            rend_elem->scale_particle.y = random->XGetFloat(ptcl_data->scale_random.y)
                + ptcl_data->scale.y;

        int32_t max_uv = ptcl_data->split_u * ptcl_data->split_v;
        if (max_uv > 1 && ptcl_data->uv_index_count > 1) {
            switch (ptcl_data->uv_index_type) {
            case UV_INDEX_INITIAL_RANDOM_FIXED:
            case UV_INDEX_INITIAL_RANDOM_FORWARD:
            case UV_INDEX_INITIAL_RANDOM_REVERSE:
                rend_elem->uv_index = ptcl_data->uv_index_start
                    + random->XGetInt(ptcl_data->uv_index_count);
                break;
            }
            rend_elem->uv_index = min_def(rend_elem->uv_index, ptcl_data->uv_index_end);
            rend_elem->uv.x = (float_t)(rend_elem->uv_index
                % ptcl_data->split_u) * ptcl_data->split_uv.x;
            rend_elem->uv.y = (float_t)(rend_elem->uv_index
                / ptcl_data->split_u) * ptcl_data->split_uv.y;
        }

        rend_elem->InitMesh(emit_inst, ptcl_data, index, random);

        bool copy_mat = false;
        if (ptcl_data->flags & PARTICLE_ROTATE_BY_EMITTER
            || ptcl_data->draw_type == DIRECTION_EMITTER_ROTATION) {
            switch (emit_inst->data.type) {
            case EMITTER_CYLINDER:
                if (emit_inst->data.cylinder.direction != EMITTER_EMISSION_DIRECTION_NONE)
                    copy_mat = vec3::length(rend_elem->direction) <= 0.000001f;
                else
                    copy_mat = true;
                break;
            case EMITTER_SPHERE:
                if (emit_inst->data.sphere.direction != EMITTER_EMISSION_DIRECTION_NONE)
                    copy_mat = vec3::length(rend_elem->direction) <= 0.000001f;
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
            if (ptcl_data->flags & PARTICLE_EMITTER_LOCAL)
                mat4_clear_trans(&rend_elem->mat, &rend_elem->mat);
        }
        else
            rend_elem->mat = mat4_identity;

        if (ptcl_data->type == PARTICLE_LOCUS) {
            uint32_t locus_history_size = random->XGetInt(
                -ptcl_data->locus_history_size_random,
                ptcl_data->locus_history_size_random)
                + ptcl_data->locus_history_size;
            rend_elem->locus_history = new LocusHistory(locus_history_size);
        }
        random->XStepValue();
    }

    void ParticleInstX::GetColor(Glitter::RenderElement* rend_elem, float_t color_scale) {
        float_t r = rend_elem->color.x;
        if (r < 0.0f)
            r = data.data.color.x;

        float_t g = rend_elem->color.y;
        if (g < 0.0f)
            g = data.data.color.y;

        float_t b = rend_elem->color.z;
        if (b < 0.0f)
            b = data.data.color.z;

        float_t a = rend_elem->color.w;
        if (a < 0.0f)
            a = data.data.color.w;

        if (color_scale >= 0.0f) {
            r *= color_scale;
            g *= color_scale;
            b *= color_scale;
        }

        if (rend_elem->fade_out_frames > 0.0f
            && rend_elem->life_time < (rend_elem->frame + rend_elem->fade_out_frames))
            a *= (rend_elem->life_time - rend_elem->frame) / rend_elem->fade_out_frames;
        else if (rend_elem->fade_in_frames > 0.0f && rend_elem->frame < rend_elem->fade_in_frames)
            a *= rend_elem->frame / rend_elem->fade_in_frames;

        GetExtColor(r, g, b, a);

        rend_elem->color.x = r;
        rend_elem->color.y = g;
        rend_elem->color.z = b;
        rend_elem->color.w = a;

        if (a < 0.01f)
            rend_elem->disp = false;
    }

    bool ParticleInstX::GetExtAnimScale(vec3* ext_anim_scale, float_t* ext_scale) {
        if (data.effect)
            return data.effect->GetExtAnimScale(ext_anim_scale, ext_scale);
        else if (data.parent && data.parent->data.effect)
            return data.parent->data.effect->GetExtAnimScale(ext_anim_scale, ext_scale);
        else
            return false;
    }

    void ParticleInstX::GetExtColor(float_t& r, float_t& g, float_t& b, float_t& a) {
        if (data.effect)
            data.effect->GetExtColor(r, g, b, a);
        else if (data.parent && data.parent->data.effect)
            data.parent->data.effect->GetExtColor(r, g, b, a);
    }

    bool ParticleInstX::GetUseCamera() {
        if (data.effect)
            return data.effect->GetUseCamera();
        else if (data.parent && data.parent->data.effect)
            return data.parent->data.effect->GetUseCamera();
        else
            return false;
    }

    bool ParticleInstX::GetValue(RenderElement* rend_elem, float_t frame, Random* random, float_t* color_scale) {
        float_t value = 0.0f;
        vec3 translation = 0.0f;
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
                if (fabsf(value) <= 0.000001f) {
                    rend_elem->disp = false;
                    return false;
                }
                break;
            case CURVE_SCALE_Y:
                rend_elem->scale.y = value;
                if (fabsf(value) <= 0.000001f) {
                    rend_elem->disp = false;
                    return false;
                }
                break;
            case CURVE_SCALE_Z:
                rend_elem->scale.z = value;
                if (fabsf(value) <= 0.000001f) {
                    rend_elem->disp = false;
                    return false;
                }
                break;
            case CURVE_SCALE_ALL:
                rend_elem->scale_all = value;
                if (fabsf(value) <= 0.000001f) {
                    rend_elem->disp = false;
                    return false;
                }
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
                    return false;
                }
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
            translation += rend_elem->base_translation;
            if (data.data.flags & PARTICLE_ROTATE_BY_EMITTER)
                mat4_transform_point(&rend_elem->mat, &translation, &translation);
            rend_elem->translation = translation;
        }
        return true;
    }

    void ParticleInstX::Free(bool free) {
        enum_or(data.flags, PARTICLE_INST_ENDED);
        if (free && data.render_group)
            data.render_group->Free();

        for (ParticleInstX*& i : data.children)
            i->Free(free);
    }

    bool ParticleInstX::HasEnded(bool a2) {
        if (!(data.flags & PARTICLE_INST_ENDED))
            return false;
        else if (!a2)
            return true;

        if (!(data.flags & PARTICLE_INST_LOCUS) || data.parent) {
            if (data.render_group && data.render_group->ctrl > 0)
                return false;
            return true;
        }

        for (ParticleInstX*& i : data.children)
            if (!i->HasEnded(a2))
                return false;
        return true;
    }

    void ParticleInstX::RenderGroupCtrl(float_t delta_frame) {
        if (data.parent || !(data.flags & PARTICLE_INST_LOCUS)) {
            if (data.render_group)
                data.render_group->Ctrl(delta_frame, true);
            return;
        }

        for (ParticleInstX*& i : data.children)
            i->RenderGroupCtrl(delta_frame);
    }

    void ParticleInstX::Reset() {
        data.flags = (ParticleInstFlag)0;
        if (data.render_group)
            data.render_group->Free();

        for (ParticleInstX*& i : data.children)
            i->Reset();
    }

    void ParticleInstX::StepUVParticle(RenderElement* rend_elem, float_t delta_frame, Random* random) {
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
