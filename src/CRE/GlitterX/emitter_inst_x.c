/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "emitter_inst_x.h"
#include "curve_x.h"
#include "particle_inst_x.h"
#include "random_x.h"

static void GlitterXEmitterInst_emit_particle(GlitterXEmitterInst* a1, float_t emission);
static void GlitterXEmitterInst_get_value(GlitterXEmitterInst* a1);

GlitterXEmitterInst::GlitterXEmitterInst(glitter_emitter* a1,
    GlitterXEffectInst* a2, float_t emission) : GlitterEmitterInst(a1, &a2->random_shared) {
    counter = 0;
    step = 0;
    switch (a1->data.type) {
    case GLITTER_EMITTER_BOX:
    case GLITTER_EMITTER_CYLINDER:
    case GLITTER_EMITTER_SPHERE:
    case GLITTER_EMITTER_POLYGON:
        break;
    default:
        return;
    }

    if (data.timer != GLITTER_EMITTER_TIMER_BY_TIME)
        this->emission = GLITTER_EMITTER_EMISSION_ON_TIMER;

    if (a2->data.flags & GLITTER_EFFECT_USE_SEED)
        random = glitter_x_random_get_value(random_ptr) + 1;
    else if (data.flags & GLITTER_EMITTER_USE_SEED)
        random = data.seed;
    else
        random = glitter_x_random_get_value(random_ptr);
    step = 1;
    glitter_x_random_step_value(random_ptr);

    particles.reserve(a1->particles.size());
    for (glitter_particle*& i : a1->particles) {
        if (!i)
            continue;

        GlitterXParticleInst* particle = new GlitterXParticleInst(i, a2, this, random_ptr, emission);
        if (particle)
            particles.push_back(particle);
    }
}

GlitterXEmitterInst::~GlitterXEmitterInst() {
    for (GlitterXParticleInst*& i : particles)
        delete i;
}

void GlitterXEmitterInst::Ctrl(GPM, GlitterXEffectInst* a2, float_t delta_frame) {
    bool mult;
    vec3 trans;
    vec3 rot;
    vec3 scale;
    vec3 rotation_add;
    vec3 trans_prev;
    bool has_dist;
    mat4 mat;
    mat4 mat_rot;
    mat4 mat1;
    mat4 dir_mat;

    if (frame < 0.0f)
        return;

    if (loop) {
        float_t loop_time = (float_t)(data.loop_end_time - data.loop_start_time);
        if (data.loop_end_time < 0.0f || frame < data.loop_end_time) {
            if (frame >= data.life_time) {
                if (data.life_time > 0.0f)
                    while (frame >= data.life_time)
                        frame -= data.life_time;
                else
                    frame = 0.0f;

                if (emission == GLITTER_EMITTER_EMISSION_EMITTED && emission_interval > 0.0f)
                    emission = GLITTER_EMITTER_EMISSION_ON_TIMER;
            }
        }
        else if (loop_time > 0.0f)
            while (frame >= data.loop_end_time)
                frame -= loop_time;
        else
            frame = 0.0f;
    }

    GlitterXEmitterInst_get_value(this);
    vec3_mult_scalar(data.rotation_add, delta_frame, rotation_add);
    vec3_add(rotation, rotation_add, rotation);

    trans = translation;
    rot = rotation;
    vec3_mult_scalar(this->scale, scale_all, scale);

    trans_prev = vec3_null;
    has_dist = false;
    if (data.timer == GLITTER_EMITTER_TIMER_BY_DISTANCE && flags & GLITTER_EMITTER_INST_HAS_DISTANCE) {
        mat4_get_translation(&this->mat, &trans_prev);
        has_dist = true;
    }

    mat = a2->mat;
    mat4_translate_mult(&mat, trans.x, trans.y, trans.z, &mat);
    mat4_normalize_rotation(&mat, &mat);

    if (data.direction == GLITTER_DIRECTION_EFFECT_ROTATION)
        mat_rot = a2->mat_rot_eff_rot;
    else {
        mat4_clear_rot(&mat, &mat);
        mat_rot = a2->mat_rot;
    }

    mult = true;
    switch (data.direction) {
    case GLITTER_DIRECTION_BILLBOARD:
        if (a2->data.flags & GLITTER_EFFECT_LOCAL) {
            mat1 = GPM_VAL->cam_view;
            mat4_clear_trans(&mat1, &mat1);
            mat4_mult(&mat1, &mat, &mat1);
        }
        else
            mat1 = mat;

        mat4_from_mat3(&GPM_VAL->cam_inv_view_mat3, &dir_mat);
        mat4_mult(&mat1, &dir_mat, &dir_mat);
        mat4_clear_trans(&dir_mat, &dir_mat);
        break;
    case GLITTER_DIRECTION_Y_AXIS:
        mat4_rotate_y((float_t)M_PI_2, &dir_mat);
        break;
    case GLITTER_DIRECTION_X_AXIS:
        mat4_rotate_x((float_t)-M_PI_2, &dir_mat);
        break;
    case GLITTER_DIRECTION_BILLBOARD_Y_AXIS:
        mat4_rotate_y(GPM_VAL->cam_rotation_y, &dir_mat);
        break;
    default:
        mult = false;
        break;
    }

    if (mult) {
        mat4_mult(&dir_mat, &mat, &mat);
        mat4_mult(&dir_mat, &mat_rot, &mat_rot);
    }
    mat4_rot(&mat, rot.x, rot.y, rot.z, &mat);
    mat4_rot(&mat_rot, rot.x, rot.y, rot.z, &mat_rot);
    mat4_scale_rot(&mat, scale.x, scale.y, scale.z, &mat);
    this->mat = mat;
    this->mat_rot = mat_rot;

    if (has_dist) {
        mat4_get_translation(&mat, &trans);

        float_t trans_dist;
        vec3_distance(trans, trans_prev, trans_dist);
        emission_timer -= trans_dist;
    }

    enum_or(flags, GLITTER_EMITTER_INST_HAS_DISTANCE);
}

void GlitterXEmitterInst::CtrlInit(GlitterXEffectInst* a2, float_t delta_frame) {
    if (frame < 0.0f)
        return;

    if (loop) {
        float_t loop_time = (float_t)(data.loop_end_time - data.loop_start_time);
        if (data.loop_end_time < 0.0f || frame < data.loop_end_time) {
            if (frame >= data.life_time) {
                if (data.life_time > 0.0f)
                    while (frame >= data.life_time)
                        frame -= data.life_time;
                else
                    frame = 0.0f;

                if (emission == GLITTER_EMITTER_EMISSION_EMITTED && emission_interval > 0.0f)
                    emission = GLITTER_EMITTER_EMISSION_ON_TIMER;
            }
        }
        else if (loop_time > 0.0f)
            while (frame >= data.loop_end_time)
                frame -= loop_time;
        else
            frame = 0.0f;
    }

    GlitterXEmitterInst_get_value(this);

    vec3 rotation_add;
    vec3_mult_scalar(data.rotation_add, delta_frame, rotation_add);
    vec3_add(rotation, rotation_add, rotation);
    if (data.timer == GLITTER_EMITTER_TIMER_BY_DISTANCE && flags & GLITTER_EMITTER_INST_HAS_DISTANCE) {
        vec3 trans_prev;
        mat4 mat;
        mat4 mat_rot;

        if (data.direction == GLITTER_DIRECTION_EFFECT_ROTATION)
            mat_rot = a2->mat_rot_eff_rot;
        else
            mat_rot = a2->mat_rot;

        mat4_get_translation(&mat, &trans_prev);
        vec3 trans = translation;
        vec3 rot = rotation;
        mat = a2->mat;
        mat4_translate_mult(&mat, trans.x, trans.y, trans.z, &mat);
        mat4_rot(&mat, rot.x, rot.y, rot.z, &mat);
        mat4_rot(&mat_rot, rot.x, rot.y, rot.z, &mat_rot);
        this->mat = mat;
        this->mat_rot = mat_rot;

        float_t trans_dist;
        mat4_get_translation(&mat, &trans);
        vec3_distance(trans, trans_prev, trans_dist);
        emission_timer -= trans_dist;
    }
    enum_or(flags, GLITTER_EMITTER_INST_HAS_DISTANCE);
}

void GlitterXEmitterInst::Emit(float_t delta_frame, float_t emission) {
    if (frame < 0.0f) {
        frame += delta_frame;
        return;
    }

    if (~flags & GLITTER_EMITTER_INST_ENDED)
        if (this->emission == GLITTER_EMITTER_EMISSION_ON_TIMER) {
            if (data.timer == GLITTER_EMITTER_TIMER_BY_DISTANCE) {
                if (emission_timer <= 0.0f || emission_interval >= 0.0f)
                    while (emission_timer <= 0.0f) {
                        GlitterXEmitterInst_emit_particle(this, emission);
                        emission_timer += emission_interval;
                        if (emission_timer < -1000000.0f)
                            emission_timer = -1000000.0f;

                        if (emission_interval <= 0.0f)
                            break;
                    }
            }
            else if (data.timer == GLITTER_EMITTER_TIMER_BY_TIME)
                if (emission_timer >= 0.0f || emission_interval >= 0.0f) {
                    emission_timer -= delta_frame;
                    if (emission_timer <= 0.0f) {
                        GlitterXEmitterInst_emit_particle(this, emission);
                        if (emission_interval > 0.0)
                            emission_timer += emission_interval;
                        else
                            emission_timer = -1.0f;
                    }
                }
        }
        else if (this->emission == GLITTER_EMITTER_EMISSION_ON_START
            && data.timer == GLITTER_EMITTER_TIMER_BY_TIME) {
            emission_timer -= delta_frame;
            if (emission_timer <= 0.0) {
                GlitterXEmitterInst_emit_particle(this, emission);
                this->emission = GLITTER_EMITTER_EMISSION_EMITTED;
            }
        }

    if (!loop && frame >= data.life_time)
        Free(emission, false);

    frame += delta_frame;
}

void GlitterXEmitterInst::Free(float_t emission, bool free) {
    if (flags & GLITTER_EMITTER_INST_ENDED) {
        if (free)
            for (GlitterXParticleInst*& i : particles)
                i->Free(true);
        return;
    }

    if (this->emission == GLITTER_EMITTER_EMISSION_ON_END) {
        GlitterXEmitterInst_emit_particle(this, emission);
        this->emission = GLITTER_EMITTER_EMISSION_EMITTED;
    }

    if (loop && data.loop_end_time >= 0.0f)
        loop = false;

    enum_or(flags, GLITTER_EMITTER_INST_ENDED);
    if (data.flags & GLITTER_EMITTER_KILL_ON_END || free)
        for (GlitterXParticleInst*& i : particles)
            i->Free(true);
    else
        for (GlitterXParticleInst*& i : particles)
            i->Free(false);
}

bool GlitterXEmitterInst::HasEnded(bool a2) {
    if (~flags & GLITTER_EMITTER_INST_ENDED)
        return false;
    else if (!a2)
        return true;

    for (GlitterXParticleInst*& i : particles)
        if (!i->HasEnded(a2))
            return false;
    return true;
}

uint8_t GlitterXEmitterInst::RandomGetStep() {
    step = (step + 2) % 60;
    return step;
}

void GlitterXEmitterInst::RandomSetValue() {
    counter += 11;
    counter %= 30000;
    glitter_x_random_set_value(random_ptr, random + counter);
}

void GlitterXEmitterInst::Reset() {
    loop = data.flags & GLITTER_EMITTER_LOOP ? true : false;
    frame = -(float_t)data.start_time;
    flags = GLITTER_EMITTER_INST_NONE;
    emission_timer = 0.0f;
    if (emission_interval >= -0.000001f)
        emission = fabsf(emission_interval) <= 0.000001f
        ? GLITTER_EMITTER_EMISSION_ON_START : GLITTER_EMITTER_EMISSION_ON_TIMER;
    else
        emission = GLITTER_EMITTER_EMISSION_ON_END;

    for (GlitterXParticleInst*& i : particles)
        i->Reset();
}

static void GlitterXEmitterInst_emit_particle(GlitterXEmitterInst* a1, float_t emission) {
    int32_t count;
    if (a1->data.type == GLITTER_EMITTER_POLYGON)
        count = a1->data.polygon.count;
    else
        count = 1;

    for (GlitterXParticleInst*& i : a1->particles)
        if (i)
            i->Emit((int32_t)roundf(a1->particles_per_emission), count, emission);
}

static void GlitterXEmitterInst_get_value(GlitterXEmitterInst* a1) {
    int64_t length;
    GlitterCurve* curve;
    float_t value;

    length = a1->emitter->animation.curves.size();
    if (!length)
        return;

    for (int32_t i = 0; i < length; i++) {
        curve = a1->emitter->animation.curves.data()[i];
        if (!glitter_x_curve_get_value(curve, a1->frame,
            &value, a1->random + i, a1->random_ptr))
            continue;

        switch (curve->type) {
        case GLITTER_CURVE_TRANSLATION_X:
            a1->translation.x = value;
            break;
        case GLITTER_CURVE_TRANSLATION_Y:
            a1->translation.y = value;
            break;
        case GLITTER_CURVE_TRANSLATION_Z:
            a1->translation.z = value;
            break;
        case GLITTER_CURVE_ROTATION_X:
            a1->rotation.x = value;
            break;
        case GLITTER_CURVE_ROTATION_Y:
            a1->rotation.y = value;
            break;
        case GLITTER_CURVE_ROTATION_Z:
            a1->rotation.z = value;
            break;
        case GLITTER_CURVE_SCALE_X:
            a1->scale.x = value;
            break;
        case GLITTER_CURVE_SCALE_Y:
            a1->scale.y = value;
            break;
        case GLITTER_CURVE_SCALE_Z:
            a1->scale.z = value;
            break;
        case GLITTER_CURVE_SCALE_ALL:
            a1->scale_all = value;
            break;
        case GLITTER_CURVE_EMISSION_INTERVAL:
            a1->emission_interval = value;
            break;
        case GLITTER_CURVE_PARTICLES_PER_EMISSION:
            a1->particles_per_emission = value;
            break;
        }
    }
}
