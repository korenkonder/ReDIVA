/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"
#include "../render_context.hpp"

extern render_context* rctx_ptr;

namespace Glitter {
    EmitterInst::EmitterInst(Emitter* emit, Random* random) : emission_timer(), flags(), random() {
        emitter = emit;

        random_ptr = random;
        data = emit->data;
        translation = emit->translation;
        rotation = emit->rotation;
        scale = emit->scale;
        mat = mat4_identity;
        mat_rot = mat4_identity;
        scale_all = 1.0f;
        emission_interval = data.emission_interval;
        particles_per_emission = data.particles_per_emission;
        frame = -(float_t)data.start_time;
        if (data.emission_interval >= -0.000001f)
            emission = data.emission_interval <= 0.000001f
                ? EMITTER_EMISSION_ON_START : EMITTER_EMISSION_ON_TIMER;
        else
            emission = EMITTER_EMISSION_ON_END;
        loop = data.flags & EMITTER_LOOP ? true : false;
    }

    EmitterInst::~EmitterInst() {

    }

    F2EmitterInst::F2EmitterInst(Emitter* emit,
        F2EffectInst* eff_inst, float_t emission) : EmitterInst(emit, eff_inst->random_ptr) {
        switch (emit->data.type) {
        case EMITTER_BOX:
        case EMITTER_CYLINDER:
        case EMITTER_SPHERE:
        case EMITTER_POLYGON:
            break;
        default:
            return;
        }

        particles.reserve(emit->particles.size());
        for (Particle*& i : emit->particles) {
            if (!i)
                continue;

            F2ParticleInst* particle = new F2ParticleInst(i, eff_inst, this, random_ptr, emission);
            if (particle)
                particles.push_back(particle);
        }

        random = random_ptr->GetValue();
        random_ptr->F2StepValue();
    }

    F2EmitterInst::~F2EmitterInst() {
        for (F2ParticleInst*& i : particles)
            delete i;
    }

    void F2EmitterInst::Copy(F2EmitterInst* dst, float_t emission) {
        dst->translation = translation;
        dst->rotation = rotation;
        dst->scale = scale;
        dst->mat = mat;
        dst->mat_rot = mat_rot;
        dst->scale_all = scale_all;
        dst->emission_timer = emission_timer;
        dst->emission_interval = emission_interval;
        dst->particles_per_emission = particles_per_emission;
        dst->random = random;
        dst->loop = loop;
        dst->emission = this->emission;
        dst->frame = frame;
        dst->flags = flags;

        if (particles.size() == dst->particles.size()) {
            size_t count = particles.size();
            for (size_t i = 0; i < count; i++)
                particles.data()[i]->Copy(dst->particles.data()[i], emission);
        }
    }

    void F2EmitterInst::Ctrl(GPM, GLT, F2EffectInst* eff_inst, float_t delta_frame) {
        if (frame < 0.0f)
            return;

        if (loop)
            if ((float_t)data.loop_end_time < 0.0f || frame < (float_t)data.loop_end_time) {
                float_t life_time = (float_t)data.life_time;
                if (frame >= life_time) {
                    frame -= life_time;
                    if (emission == EMITTER_EMISSION_EMITTED && emission_interval > 0.0f)
                        emission = EMITTER_EMISSION_ON_TIMER;
                }
            }
            else {
                float_t loop_time = (float_t)(data.loop_end_time - data.loop_start_time);
                float_t loop_end_time = (float_t)data.loop_end_time;
                while (frame > loop_end_time)
                    frame -= loop_time;
            }

        GetValue(GLT_VAL);
        rotation += data.rotation_add * delta_frame;
        CtrlMat(GPM_VAL, GLT_VAL, eff_inst);
    }

    void F2EmitterInst::CtrlInit(GPM, GLT, F2EffectInst* eff_inst, float_t delta_frame) {
        if (frame < 0.0f)
            return;

        if (loop)
            if ((float_t)data.loop_end_time < 0.0f || frame < (float_t)data.loop_end_time) {
                float_t life_time = (float_t)data.life_time;
                if (frame >= life_time)
                    frame -= life_time;
            }
            else {
                float_t loop_time = (float_t)(data.loop_end_time - data.loop_start_time);
                float_t loop_end_time = (float_t)data.loop_end_time;
                while (frame > loop_end_time)
                    frame -= loop_time;
            }

        if (frame < 0.0f)
            return;

        GetValue(GLT_VAL);
        rotation += data.rotation_add * delta_frame;
        CtrlMat(GPM_VAL, GLT_VAL, eff_inst);
    }

    void F2EmitterInst::CtrlMat(GPM, GLT, F2EffectInst* eff_inst) {
        vec3 trans = translation;
        vec3 rot = rotation;
        vec3 scale = this->scale * scale_all;

        bool mult = true;
        mat4 dir_mat;
        switch (data.direction) {
        case DIRECTION_BILLBOARD:
            mat4_from_mat3(&rctx_ptr->camera->inv_view_mat3, &dir_mat);
            mat4_mul(&eff_inst->mat, &dir_mat, &dir_mat);
            mat4_clear_trans(&dir_mat, &dir_mat);
            break;
        case DIRECTION_Y_AXIS:
            mat4_rotate_y((float_t)-M_PI_2, &dir_mat);
            break;
        case DIRECTION_X_AXIS:
            mat4_rotate_x((float_t)-M_PI_2, &dir_mat);
            break;
        case DIRECTION_Z_AXIS:
            mat4_rotate_z((float_t)-M_PI_2, &dir_mat);
            break;
        case DIRECTION_BILLBOARD_Y_AXIS:
            mat4_rotate_y(rctx_ptr->camera->rotation.y, &dir_mat);
            break;
        default:
            mult = false;
            break;
        }

        mat4 mat;
        mat4_mul_translate(&eff_inst->mat, &trans, &mat);
        mat4_normalize_rotation(&mat, &mat);
        if (mult)
            mat4_mul(&dir_mat, &mat, &mat);
        mat4_mul_rotate_zyx(&mat, &rot, &mat);
        mat4_clear_trans(&mat, &mat_rot);
        mat4_scale_rot(&mat, &scale, &mat);
        this->mat = mat;
    }

    void F2EmitterInst::Emit(GPM, GLT, float_t delta_frame, float_t emission) {
        if (frame < 0.0f) {
            frame += delta_frame;
            return;
        }

        if (!(flags & EMITTER_INST_ENDED))
            if (this->emission == EMITTER_EMISSION_ON_TIMER) {
                if (data.timer == EMITTER_TIMER_BY_TIME) {
                    if (emission_timer >= 0.0f || emission_interval >= 0.0f) {
                        emission_timer -= delta_frame;
                        if (emission_timer <= 0.0f) {
                            EmitParticle(GPM_VAL, GLT_VAL, emission);
                            if (emission_interval > 0.0f)
                                emission_timer += emission_interval;
                            else
                                emission_timer = -1.0f;
                        }
                    }
                }
            }
            else if (this->emission == EMITTER_EMISSION_ON_START) {
                emission_timer -= delta_frame;
                if (emission_timer <= 0.0f) {
                    EmitParticle(GPM_VAL, GLT_VAL, emission);
                    this->emission = EMITTER_EMISSION_EMITTED;
                }
            }

        if (!loop && frame >= (float_t)data.life_time)
            Free(GPM_VAL, GLT_VAL, emission, false);
        frame += delta_frame;
    }

    void F2EmitterInst::EmitInit(GPM, GLT, F2EffectInst* eff_inst, float_t delta_frame, float_t emission) {
        if (frame < 0.0f) {
            frame += delta_frame;
            return;
        }

        if (!(flags & EMITTER_INST_ENDED)) {
            if (this->emission == EMITTER_EMISSION_ON_TIMER) {
                if (data.timer == EMITTER_TIMER_BY_TIME) {
                    if (emission_timer >= 0.0f || emission_interval >= 0.0f) {
                        emission_timer -= delta_frame;
                        if (emission_timer <= 0.0f) {
                            eff_inst->CtrlMat(GPM_VAL, GLT_VAL);
                            EmitParticle(GPM_VAL, GLT_VAL, emission);
                            if (emission_interval > 0.0f)
                                emission_timer += emission_interval;
                            else
                                emission_timer = -1.0f;
                        }
                    }
                }
            }
            else if (this->emission == EMITTER_EMISSION_ON_START) {
                eff_inst->CtrlMat(GPM_VAL, GLT_VAL);
                EmitParticle(GPM_VAL, GLT_VAL, emission);
                this->emission = EMITTER_EMISSION_EMITTED;
            }
        }

        if (!loop && frame >= (float_t)data.life_time)
            Free(GPM_VAL, GLT_VAL, emission, false);

        frame += delta_frame;
    }

    void F2EmitterInst::EmitParticle(GPM, GLT, float_t emission) {
        int32_t count;
        if (data.type == EMITTER_POLYGON)
            count = data.polygon.count;
        else
            count = 1;

        for (F2ParticleInst*& i : particles)
            i->Emit(GPM_VAL, GLT_VAL, (int32_t)prj::roundf(particles_per_emission), count, emission);
    }

    void F2EmitterInst::Free(GPM, GLT, float_t emission, bool free) {
        if (flags & EMITTER_INST_ENDED) {
            if (free)
                for (F2ParticleInst*& i : particles)
                    i->Free(true);
            return;
        }

        if (this->emission == EMITTER_EMISSION_ON_END) {
            EmitParticle(GPM_VAL, GLT_VAL, emission);
            this->emission = EMITTER_EMISSION_EMITTED;
        }

        if (loop && (float_t)data.loop_end_time >= 0.0f)
            loop = false;

        enum_or(flags, EMITTER_INST_ENDED);
        if (data.flags & EMITTER_KILL_ON_END || free)
            for (F2ParticleInst*& i : particles)
                i->Free(true);
        else
            for (F2ParticleInst*& i : particles)
                i->Free(false);
    }

    void F2EmitterInst::GetValue(GLT) {
        Animation* anim = &emitter->animation;
        size_t length = anim->curves.size();
        for (int32_t i = 0; i < length; i++) {
            Curve* curve = anim->curves.data()[i];
            float_t value;
            if (!curve->F2GetValue(GLT_VAL, frame, &value, random + i, random_ptr))
                continue;

            switch (curve->type) {
            case CURVE_TRANSLATION_X:
                translation.x = value;
                break;
            case CURVE_TRANSLATION_Y:
                translation.y = value;
                break;
            case CURVE_TRANSLATION_Z:
                translation.z = value;
                break;
            case CURVE_ROTATION_X:
                rotation.x = value;
                break;
            case CURVE_ROTATION_Y:
                rotation.y = value;
                break;
            case CURVE_ROTATION_Z:
                rotation.z = value;
                break;
            case CURVE_SCALE_X:
                scale.x = value;
                break;
            case CURVE_SCALE_Y:
                scale.y = value;
                break;
            case CURVE_SCALE_Z:
                scale.z = value;
                break;
            case CURVE_SCALE_ALL:
                scale_all = value;
                break;
            case CURVE_EMISSION_INTERVAL:
                emission_interval = value;
                break;
            case CURVE_PARTICLES_PER_EMISSION:
                particles_per_emission = value;
                break;
            }
        }
    }

    bool F2EmitterInst::HasEnded(bool a2) {
        if (!(flags & EMITTER_INST_ENDED))
            return false;
        else if (!a2)
            return true;

        for (F2ParticleInst*& i : particles)
            if (!i->HasEnded(a2))
                return false;
        return true;
    }

    void F2EmitterInst::InitMesh(GLT, int32_t index, const vec3& scale,
        vec3& position, vec3& direction, Random* random) {
        switch (data.type) {
        case EMITTER_BOX: {
            position = random->F2GetVec3(GLT_VAL, data.box.size * scale * 0.5f);
        } break;
        case EMITTER_CYLINDER: {
            float_t radius = data.cylinder.radius * scale.x;
            if (!data.cylinder.on_edge)
                radius = random->F2GetFloat(GLT_VAL, 0.0f, radius);

            float_t angle = random->F2GetFloat(GLT_VAL, data.cylinder.start_angle, data.cylinder.end_angle);

            vec3 dir;
            dir.x = cosf(angle);
            dir.y = 0.0f;
            dir.z = sinf(angle);

            position.x = dir.x * radius;
            position.y = random->F2GetFloat(GLT_VAL, data.cylinder.height * scale.y * 0.5f);
            position.z = dir.z * radius;

            if (data.cylinder.direction == EMITTER_EMISSION_DIRECTION_OUTWARD)
                direction = dir;
            else if (data.cylinder.direction == EMITTER_EMISSION_DIRECTION_INWARD) {
                const vec3 reverse_xz_dir = { -0.0f, 0.0f, -0.0f };
                direction = dir ^ reverse_xz_dir;
            }
        } break;
        case EMITTER_SPHERE: {
            float_t radius = data.sphere.radius * scale.x;
            if (!data.sphere.on_edge)
                radius = random->F2GetFloat(GLT_VAL, 0.0f, radius);

            float_t longitude = random->F2GetFloat(GLT_VAL, data.sphere.longitude * 0.5f);
            float_t latitude = (float_t)M_PI_2 - random->F2GetFloat(GLT_VAL, 0.0f, data.sphere.latitude);

            vec3 dir;
            dir.x = sinf(longitude) * cosf(latitude);
            dir.y = sinf(latitude);
            dir.z = cosf(longitude) * cosf(latitude);

            position = dir * radius;
            if (data.sphere.direction == EMITTER_EMISSION_DIRECTION_OUTWARD)
                direction = dir;
            else if (data.sphere.direction == EMITTER_EMISSION_DIRECTION_INWARD)
                direction = -dir;
        } break;
        case EMITTER_POLYGON: {
            float_t radius = data.polygon.size * scale.x;
            float_t angle = (float_t)index * 360.0f
                / (float_t)data.polygon.count * DEG_TO_RAD_FLOAT + (float_t)M_PI_2;

            position.x = sinf(angle) * radius;
            position.y = 0.0f;
        } break;
        }
    }

    void F2EmitterInst::RenderGroupCtrl(GLT, float_t delta_frame) {
        for (F2ParticleInst*& i : particles)
            i->RenderGroupCtrl(GLT_VAL, delta_frame);
    }

    void F2EmitterInst::Reset() {
        loop = data.flags & EMITTER_LOOP ? true : false;
        frame = -(float_t)data.start_time;
        flags = EMITTER_INST_NONE;
        emission_timer = 0.0f;
        if (emission_interval >= -0.000001f)
            emission = fabsf(emission_interval) <= 0.000001f
            ? EMITTER_EMISSION_ON_START : EMITTER_EMISSION_ON_TIMER;
        else
            emission = EMITTER_EMISSION_ON_END;

        for (F2ParticleInst*& i : particles)
            i->Reset();
    }

    XEmitterInst::XEmitterInst(Emitter* emit,
        XEffectInst* eff_inst, float_t emission) : EmitterInst(emit, &eff_inst->random_shared) {
        counter = 0;
        step = 0;
        switch (emit->data.type) {
        case EMITTER_BOX:
        case EMITTER_CYLINDER:
        case EMITTER_SPHERE:
        case EMITTER_POLYGON:
            break;
        default:
            return;
        }

        if (data.timer != EMITTER_TIMER_BY_TIME)
            this->emission = EMITTER_EMISSION_ON_TIMER;

        if (eff_inst->data.flags & EFFECT_USE_SEED)
            random = random_ptr->GetValue() + 1;
        else if (data.flags & EMITTER_USE_SEED)
            random = data.seed;
        else
            random = random_ptr->GetValue();
        step = 1;
        random_ptr->XStepValue();

        particles.reserve(emit->particles.size());
        for (Particle*& i : emit->particles) {
            if (!i)
                continue;

            XParticleInst* particle = new XParticleInst(i, eff_inst, this, random_ptr, emission);
            if (particle)
                particles.push_back(particle);
        }
    }

    XEmitterInst::~XEmitterInst() {
        for (XParticleInst*& i : particles)
            delete i;
    }

    bool XEmitterInst::CheckUseCamera() {
        if (!particles.size())
            return false;

        switch (data.direction) {
        case DIRECTION_BILLBOARD:
        case DIRECTION_BILLBOARD_Y_AXIS:
            return true;
        }

        for (XParticleInst*& i : particles)
            if (i->CheckUseCamera())
                return true;
        return false;
    }

    void XEmitterInst::Copy(XEmitterInst* dst, float_t emission) {
        dst->translation = translation;
        dst->rotation = rotation;
        dst->scale = scale;
        dst->mat = mat;
        dst->mat_rot = mat_rot;
        dst->scale_all = scale_all;
        dst->emission_timer = emission_timer;
        dst->emission_interval = emission_interval;
        dst->particles_per_emission = particles_per_emission;
        dst->random = random;
        dst->loop = loop;
        dst->emission = this->emission;
        dst->frame = frame;
        dst->flags = flags;

        if (particles.size() == dst->particles.size()) {
            size_t count = particles.size();
            for (size_t i = 0; i < count; i++)
                particles.data()[i]->Copy(dst->particles.data()[i], emission);
        }
    }

    void XEmitterInst::Ctrl(GPM, XEffectInst* eff_inst, float_t delta_frame) {
        if (frame < 0.0f)
            return;

        if (loop) {
            if ((float_t)data.loop_end_time < 0.0f || frame < (float_t)data.loop_end_time) {
                float_t life_time = (float_t)data.life_time;
                if (frame >= life_time) {
                    if (life_time > 0.0f)
                        while (frame >= life_time)
                            frame -= life_time;
                    else
                        frame = 0.0f;

                    if (emission == EMITTER_EMISSION_EMITTED && emission_interval > 0.0f)
                        emission = EMITTER_EMISSION_ON_TIMER;
                }
            }
            else {
                float_t loop_time = (float_t)(data.loop_end_time - data.loop_start_time);
                if (loop_time > 0.0f) {
                    float_t loop_end_time = (float_t)data.loop_end_time;
                    while (frame >= loop_end_time)
                        frame -= loop_time;
                }
                else
                    frame = 0.0f;
            }
        }

        GetValue();

        rotation += data.rotation_add * delta_frame;

        vec3 trans_prev = {};
        bool has_dist = false;
        if (data.timer == EMITTER_TIMER_BY_DISTANCE && flags & EMITTER_INST_HAS_DISTANCE) {
            mat4_get_translation(&mat, &trans_prev);
            has_dist = true;
        }

        vec3 trans = translation;
        vec3 rot = rotation;
        vec3 scale = this->scale * scale_all;

        mat4 mat = eff_inst->mat;
        mat4_mul_translate(&mat, &trans, &mat);
        mat4 mat_rot;
        if (data.direction == DIRECTION_EFFECT_ROTATION) {
            mat4_normalize_rotation(&mat, &mat);
            mat_rot = eff_inst->mat_rot;
        }
        else
            mat_rot = eff_inst->mat_rot_eff_rot;

        bool mult = true;
        mat4 dir_mat;
        switch (data.direction) {
        case DIRECTION_BILLBOARD: {
            if (eff_inst->data.flags & EFFECT_SCREEN) {
                dir_mat = rctx_ptr->camera->view;
                mat4_clear_trans(&dir_mat, &dir_mat);
                mat4_mul(&dir_mat, &mat, &dir_mat);
            }
            else
                dir_mat = mat;

            mat4 inv_view_mat;
            mat4_from_mat3(&rctx_ptr->camera->inv_view_mat3, &inv_view_mat);
            mat4_mul(&dir_mat, &inv_view_mat, &dir_mat);
            mat4_clear_trans(&dir_mat, &dir_mat);
        } break;
        case DIRECTION_Y_AXIS:
            mat4_rotate_y((float_t)M_PI_2, &dir_mat);
            break;
        case DIRECTION_X_AXIS:
            mat4_rotate_x((float_t)-M_PI_2, &dir_mat);
            break;
        case DIRECTION_BILLBOARD_Y_AXIS:
            mat4_rotate_y(rctx_ptr->camera->rotation.y, &dir_mat);
            break;
        default:
            mult = false;
            break;
        }

        if (mult) {
            mat4_mul(&dir_mat, &mat, &mat);
            mat4_mul(&dir_mat, &mat_rot, &mat_rot);
        }

        mat4_mul_rotate_zyx(&mat, &rot, &mat);
        mat4_mul_rotate_zyx(&mat_rot, &rot, &mat_rot);
        mat4_scale_rot(&mat, &scale, &mat);
        this->mat = mat;
        this->mat_rot = mat_rot;

        if (has_dist) {
            vec3 trans;
            mat4_get_translation(&mat, &trans);
            emission_timer -= vec3::distance(trans, trans_prev);
        }

        if (!(flags & EMITTER_INST_HAS_DISTANCE))
            enum_or(flags, EMITTER_INST_HAS_DISTANCE);
    }

    void XEmitterInst::CtrlInit(XEffectInst* eff_inst, float_t delta_frame) {
        if (frame < 0.0f)
            return;

        if (loop) {
            if ((float_t)data.loop_end_time < 0.0f || frame < (float_t)data.loop_end_time) {
                float_t life_time = (float_t)data.life_time;
                if (frame >= life_time)
                    frame -= life_time;
            }
            else {
                float_t loop_time = (float_t)(data.loop_end_time - data.loop_start_time);
                float_t loop_end_time = (float_t)data.loop_end_time;
                while (frame > loop_end_time)
                    frame -= loop_time;
            }
        }

        GetValue();

        rotation += data.rotation_add * delta_frame;

        if (data.timer == EMITTER_TIMER_BY_DISTANCE && flags & EMITTER_INST_HAS_DISTANCE) {
            vec3 trans_prev;
            mat4 mat;
            mat4 mat_rot;

            if (data.direction == DIRECTION_EFFECT_ROTATION)
                mat_rot = eff_inst->mat_rot_eff_rot;
            else
                mat_rot = eff_inst->mat_rot;

            mat4_get_translation(&mat, &trans_prev);
            vec3 trans = translation;
            vec3 rot = rotation;
            mat = eff_inst->mat;
            mat4_mul_translate(&mat, &trans, &mat);
            mat4_mul_rotate_zyx(&mat, &rot, &mat);
            mat4_mul_rotate_zyx(&mat_rot, &rot, &mat_rot);
            this->mat = mat;
            this->mat_rot = mat_rot;

            mat4_get_translation(&mat, &trans);
            emission_timer -= vec3::distance(trans, trans_prev);
        }
        enum_or(flags, EMITTER_INST_HAS_DISTANCE);
    }

    void XEmitterInst::CtrlMat(GPM, XEffectInst* eff_inst) {
        vec3 trans = translation;
        vec3 rot = rotation;
        vec3 scale = this->scale * scale_all;

        mat4 mat = eff_inst->mat;
        mat4_mul_translate(&mat, &trans, &mat);
        mat4 mat_rot;
        if (data.direction == DIRECTION_EFFECT_ROTATION) {
            mat4_normalize_rotation(&mat, &mat);
            mat_rot = eff_inst->mat_rot;
        }
        else
            mat_rot = eff_inst->mat_rot_eff_rot;

        bool mult = true;
        mat4 dir_mat;
        switch (data.direction) {
        case DIRECTION_BILLBOARD: {
            mat4_from_mat3(&rctx_ptr->camera->inv_view_mat3, &dir_mat);
            mat4_mul(&dir_mat, &mat, &dir_mat);
            mat4_clear_trans(&dir_mat, &dir_mat);
        } break;
        case DIRECTION_Y_AXIS:
            mat4_rotate_y((float_t)M_PI_2, &dir_mat);
            break;
        case DIRECTION_X_AXIS:
            mat4_rotate_x((float_t)-M_PI_2, &dir_mat);
            break;
        case DIRECTION_BILLBOARD_Y_AXIS:
            mat4_rotate_y(rctx_ptr->camera->rotation.y, &dir_mat);
            break;
        default:
            mult = false;
            break;
        }

        if (mult) {
            mat4_mul(&dir_mat, &mat, &mat);
            mat4_mul(&dir_mat, &mat_rot, &mat_rot);
        }

        mat4_mul_rotate_zyx(&mat, &rot, &mat);
        mat4_mul_rotate_zyx(&mat_rot, &rot, &mat_rot);
        mat4_scale_rot(&mat, &scale, &mat);
        this->mat = mat;
        this->mat_rot = mat_rot;
    }

    void XEmitterInst::Emit(float_t delta_frame, float_t emission) {
        if (frame < 0.0f) {
            frame += delta_frame;
            return;
        }

        if (!(flags & EMITTER_INST_ENDED))
            if (this->emission == EMITTER_EMISSION_ON_TIMER) {
                if (data.timer == EMITTER_TIMER_BY_DISTANCE) {
                    if (emission_timer <= 0.0f || emission_interval >= 0.0f)
                        while (emission_timer <= 0.0f) {
                            EmitParticle(emission, -emission_timer);
                            emission_timer += emission_interval;
                            if (emission_timer < -1000000.0f)
                                emission_timer = -1000000.0f;

                            if (emission_interval <= 0.0f)
                                break;
                        }
                }
                else if (data.timer == EMITTER_TIMER_BY_TIME)
                    if (emission_timer >= 0.0f || emission_interval >= 0.0f) {
                        emission_timer -= delta_frame;
                        if (emission_timer <= 0.0f) {
                            EmitParticle(emission, -emission_timer);
                            if (emission_interval > 0.0f)
                                emission_timer += emission_interval;
                            else
                                emission_timer = -1.0f;
                        }
                    }
            }
            else if (this->emission == EMITTER_EMISSION_ON_START
                && data.timer == EMITTER_TIMER_BY_TIME) {
                emission_timer -= delta_frame;
                if (emission_timer <= 0.0f) {
                    EmitParticle(emission, -emission_timer);
                    this->emission = EMITTER_EMISSION_EMITTED;
                }
            }

        if (!loop && frame >= (float_t)data.life_time)
            Free(emission, false);

        frame += delta_frame;
    }

    void XEmitterInst::EmitInit(GPM, XEffectInst* eff_inst, float_t delta_frame, float_t emission) {
        if (frame < 0.0f) {
            frame += delta_frame;
            return;
        }

        if (!(flags & EMITTER_INST_ENDED)) {
            if (this->emission == EMITTER_EMISSION_ON_TIMER) {
                if (data.timer == EMITTER_TIMER_BY_DISTANCE) {
                    if (emission_timer <= 0.0f) {
                        EmitParticle(emission, -emission_timer);
                        emission_timer += emission_timer;
                        if (emission_timer < -1000000.0f)
                            emission_timer = -1000000.0f;
                    }
                }
                else if (data.timer == EMITTER_TIMER_BY_TIME) {
                    if (emission_timer >= 0.0f || emission_interval >= 0.0f) {
                        emission_timer -= delta_frame;
                        if (emission_timer <= 0.0f) {
                            eff_inst->CtrlMat(GPM_VAL);
                            EmitParticle(emission, -emission_timer);
                            if (emission_interval > 0.0f)
                                emission_timer += emission_interval;
                            else
                                emission_timer = -1.0;
                        }
                    }
                }
            }
            else if (this->emission == EMITTER_EMISSION_ON_START) {
                eff_inst->CtrlMat(GPM_VAL);
                EmitParticle(emission, 0.0f);
                this->emission = EMITTER_EMISSION_EMITTED;
            }
        }

        if (!loop && frame >= (float_t)data.life_time)
            Free(emission, false);

        frame += delta_frame;
    }

    void XEmitterInst::EmitParticle(float_t emission, float_t frame) {
        int32_t count;
        if (data.type == EMITTER_POLYGON)
            count = data.polygon.count;
        else
            count = 1;

        for (XParticleInst*& i : particles)
            if (i)
                i->Emit((int32_t)prj::roundf(particles_per_emission), count, emission, frame);
    }

    void XEmitterInst::Free(float_t emission, bool free) {
        if (flags & EMITTER_INST_ENDED) {
            if (free)
                for (XParticleInst*& i : particles)
                    i->Free(true);
            return;
        }

        if (this->emission == EMITTER_EMISSION_ON_END) {
            EmitParticle(emission, 0.0f);
            this->emission = EMITTER_EMISSION_EMITTED;
        }

        if (loop && data.loop_end_time >= 0.0f)
            loop = false;

        enum_or(flags, EMITTER_INST_ENDED);
        if (data.flags & EMITTER_KILL_ON_END || free)
            for (XParticleInst*& i : particles)
                i->Free(true);
        else
            for (XParticleInst*& i : particles)
                i->Free(false);
    }

    void XEmitterInst::GetValue() {
        Animation* anim = &emitter->animation;
        size_t length = anim->curves.size();
        for (int32_t i = 0; i < length; i++) {
            Curve* curve = anim->curves.data()[i];
            float_t value;
            if (!curve->XGetValue(frame, &value, random + i, random_ptr))
                continue;

            switch (curve->type) {
            case CURVE_TRANSLATION_X:
                translation.x = value;
                break;
            case CURVE_TRANSLATION_Y:
                translation.y = value;
                break;
            case CURVE_TRANSLATION_Z:
                translation.z = value;
                break;
            case CURVE_ROTATION_X:
                rotation.x = value;
                break;
            case CURVE_ROTATION_Y:
                rotation.y = value;
                break;
            case CURVE_ROTATION_Z:
                rotation.z = value;
                break;
            case CURVE_SCALE_X:
                scale.x = value;
                break;
            case CURVE_SCALE_Y:
                scale.y = value;
                break;
            case CURVE_SCALE_Z:
                scale.z = value;
                break;
            case CURVE_SCALE_ALL:
                scale_all = value;
                break;
            case CURVE_EMISSION_INTERVAL:
                emission_interval = value;
                break;
            case CURVE_PARTICLES_PER_EMISSION:
                particles_per_emission = value;
                break;
            }
        }
    }

    bool XEmitterInst::HasEnded(bool a2) {
        if (!(flags & EMITTER_INST_ENDED))
            return false;
        else if (!a2)
            return true;

        for (XParticleInst*& i : particles)
            if (!i->HasEnded(a2))
                return false;
        return true;
    }

    void XEmitterInst::InitMesh(int32_t index, const vec3& scale,
        vec3& position, vec3& direction, Random* random) {
        switch (emitter->data.type) {
        case EMITTER_BOX: {
            position = random->XGetVec3(data.box.size * scale * 0.5f);
        } break;
        case EMITTER_CYLINDER: {
            float_t radius = data.cylinder.radius * scale.x;
            if (!data.cylinder.on_edge)
                radius = random->XGetFloat(0.0f, radius);

            float_t angle = random->XGetFloat(data.cylinder.start_angle, data.cylinder.end_angle);

            vec3 dir;
            dir.x = cosf(angle);
            dir.y = 0.0f;
            dir.z = sinf(angle);

            position.x = dir.x * radius;
            position.y = random->XGetFloat(data.cylinder.height * scale.y * 0.5f);
            position.z = dir.z * radius;

            if (data.cylinder.direction == EMITTER_EMISSION_DIRECTION_OUTWARD)
                direction = dir;
            else if (data.cylinder.direction == EMITTER_EMISSION_DIRECTION_INWARD) {
                const vec3 reverse_xz_dir = { -0.0f, 0.0f, -0.0f };
                direction = dir ^ reverse_xz_dir;
            }
        } break;
        case EMITTER_SPHERE: {
            float_t radius = data.sphere.radius * scale.x;
            if (!data.sphere.on_edge)
                radius = random->XGetFloat(0.0f, radius);

            float_t longitude = random->XGetFloat(data.sphere.longitude * 0.5f);
            float_t latitude = (float_t)M_PI_2 - random->XGetFloat(0.0f, data.sphere.latitude);

            vec3 dir;
            dir.x = sinf(longitude) * cosf(latitude);
            dir.y = sinf(latitude);
            dir.z = cosf(longitude) * cosf(latitude);

            position = dir * radius;
            if (data.sphere.direction == EMITTER_EMISSION_DIRECTION_OUTWARD)
                direction = dir;
            else if (data.sphere.direction == EMITTER_EMISSION_DIRECTION_INWARD)
                direction = -dir;
        } break;
        case EMITTER_POLYGON: {
            float_t radius = data.polygon.size * scale.x;
            float_t angle = ((float_t)index * 360.0f
                / (float_t)data.polygon.count + 90.0f) * DEG_TO_RAD_FLOAT;

            vec3 dir;
            dir.x = cosf(angle);
            dir.y = 0.0f;
            dir.z = sinf(angle);

            position = dir * radius;
            if (data.sphere.direction == EMITTER_EMISSION_DIRECTION_OUTWARD)
                direction = dir;
            else if (data.sphere.direction == EMITTER_EMISSION_DIRECTION_INWARD)
                direction = -dir;
        } break;
        }
    }

    uint8_t XEmitterInst::RandomGetStep() {
        step = (step + 2) % 60;
        return step;
    }

    void XEmitterInst::RandomStepValue() {
        counter += 11;
        counter %= 30000;
        random_ptr->SetValue(random + counter);
    }

    void XEmitterInst::RenderGroupCtrl(float_t delta_frame) {
        for (XParticleInst*& i : particles)
            i->RenderGroupCtrl(delta_frame);
    }

    void XEmitterInst::Reset() {
        loop = data.flags & EMITTER_LOOP ? true : false;
        frame = -(float_t)data.start_time;
        flags = EMITTER_INST_NONE;
        emission_timer = 0.0f;
        if (emission_interval >= -0.000001f)
            emission = fabsf(emission_interval) <= 0.000001f
            ? EMITTER_EMISSION_ON_START : EMITTER_EMISSION_ON_TIMER;
        else
            emission = EMITTER_EMISSION_ON_END;

        for (XParticleInst*& i : particles)
            i->Reset();
    }
}
