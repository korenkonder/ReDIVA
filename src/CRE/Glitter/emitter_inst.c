/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "emitter_inst.h"
#include "curve.h"
#include "particle_inst.h"
#include "random.h"

static void GlitterF2EmitterInst_emit_particle(GPM, GLT,
    GlitterF2EmitterInst* a1, float_t emission);
static void GlitterF2EmitterInst_get_value(GLT, GlitterF2EmitterInst* a1);
static void GlitterF2EmitterInst_ctrl_mat(GPM, GLT,
    GlitterF2EmitterInst* a1, GlitterF2EffectInst* a2);

GlitterEmitterInst::GlitterEmitterInst(glitter_emitter* a1,
    glitter_random* random) : emission_timer(), flags(), random() {
    emitter = a1;

    random_ptr = random;
    data = a1->data;
    translation = a1->translation;
    rotation = a1->rotation;
    scale = a1->scale;
    mat = mat4_identity;
    mat_rot = mat4_identity;
    scale_all = 1.0f;
    emission_interval = data.emission_interval;
    particles_per_emission = data.particles_per_emission;
    frame = -(float_t)data.start_time;
    if (data.emission_interval >= -0.000001f)
        emission = data.emission_interval <= 0.000001f
        ? GLITTER_EMITTER_EMISSION_ON_START : GLITTER_EMITTER_EMISSION_ON_TIMER;
    else
        emission = GLITTER_EMITTER_EMISSION_ON_END;
    loop = data.flags & GLITTER_EMITTER_LOOP ? true : false;
}

GlitterEmitterInst::~GlitterEmitterInst() {

}

GlitterF2EmitterInst::GlitterF2EmitterInst(glitter_emitter* a1,
    GlitterF2EffectInst* a2, float_t emission) : GlitterEmitterInst(a1, a2->random_ptr) {
    switch (a1->data.type) {
    case GLITTER_EMITTER_BOX:
    case GLITTER_EMITTER_CYLINDER:
    case GLITTER_EMITTER_SPHERE:
    case GLITTER_EMITTER_POLYGON:
        break;
    default:
        return;
    }

    particles.reserve(a1->particles.size());
    for (glitter_particle*& i : a1->particles) {
        if (!i)
            continue;

        GlitterF2ParticleInst* particle = new GlitterF2ParticleInst(i, a2, this, random_ptr, emission);
        if (particle)
            particles.push_back(particle);
    }

    random = glitter_random_get_value(random_ptr);
    glitter_random_set_value(random_ptr, glitter_random_get_value(random_ptr) + 1);
}

GlitterF2EmitterInst::~GlitterF2EmitterInst() {
    for (GlitterF2ParticleInst*& i : particles)
        delete i;
}

void GlitterF2EmitterInst::Ctrl(GPM, GLT, GlitterF2EffectInst* a2, float_t delta_frame) {
    vec3 rotation_add;

    if (frame < 0.0f)
        return;

    if (loop)
        if (data.loop_end_time < 0.0f || frame < data.loop_end_time) {
            if (frame >= data.life_time) {
                frame -= data.life_time;
                if (emission == GLITTER_EMITTER_EMISSION_EMITTED && emission_interval > 0.0f)
                    emission = GLITTER_EMITTER_EMISSION_ON_TIMER;
            }
        }
        else
            while (frame > data.loop_end_time)
                frame -= data.loop_end_time - data.loop_start_time;

    GlitterF2EmitterInst_get_value(GLT_VAL, this);
    vec3_mult_scalar(data.rotation_add, delta_frame, rotation_add);
    vec3_add(rotation, rotation_add, rotation);
    GlitterF2EmitterInst_ctrl_mat(GPM_VAL, GLT_VAL, this, a2);
}

void GlitterF2EmitterInst::CtrlInit(GPM, GLT, GlitterF2EffectInst* a2, float_t delta_frame) {
    vec3 rotation_add;

    if (frame < 0.0f)
        return;

    if (loop)
        if (data.loop_end_time < 0.0f || frame < data.loop_end_time) {
            if (frame >= data.life_time)
                frame -= data.life_time;
        }
        else
            while (frame > data.loop_end_time)
                frame -= data.loop_end_time - data.loop_start_time;

    if (frame < 0.0f)
        return;

    GlitterF2EmitterInst_get_value(GLT_VAL, this);
    vec3_mult_scalar(data.rotation_add, delta_frame, rotation_add);
    vec3_add(rotation, rotation_add, rotation);
    GlitterF2EmitterInst_ctrl_mat(GPM_VAL, GLT_VAL, this, a2);
}

void GlitterF2EmitterInst::Emit(GPM, GLT, float_t delta_frame, float_t emission) {
    if (frame < 0.0f) {
        frame += delta_frame;
        return;
    }

    if (~flags & GLITTER_EMITTER_INST_ENDED)
        if (this->emission == GLITTER_EMITTER_EMISSION_ON_TIMER) {
            if (emission_timer >= 0.0f || emission_interval >= 0.0f) {
                emission_timer -= delta_frame;
                if (emission_timer <= 0.0) {
                    GlitterF2EmitterInst_emit_particle(GPM_VAL, GLT_VAL, this, emission);
                    if (emission_interval > 0.0f)
                        emission_timer += emission_interval;
                    else
                        emission_timer = -1.0;
                }
            }
        }
        else if (this->emission == GLITTER_EMITTER_EMISSION_ON_START) {
            emission_timer -= delta_frame;
            if (emission_timer <= 0.0) {
                GlitterF2EmitterInst_emit_particle(GPM_VAL, GLT_VAL, this, emission);
                this->emission = GLITTER_EMITTER_EMISSION_EMITTED;
            }
        }

    if (!loop && frame >= data.life_time)
        Free(GPM_VAL, GLT_VAL, emission, false);
    frame += delta_frame;
}

void GlitterF2EmitterInst::Free(GPM, GLT, float_t emission, bool free) {
    if (flags & GLITTER_EMITTER_INST_ENDED) {
        if (free)
            for (GlitterF2ParticleInst*& i : particles)
                i->Free(true);
        return;
    }

    if (this->emission == GLITTER_EMITTER_EMISSION_ON_END) {
        GlitterF2EmitterInst_emit_particle(GPM_VAL, GLT_VAL, this, emission);
        this->emission = GLITTER_EMITTER_EMISSION_EMITTED;
    }

    if (loop && data.loop_end_time >= 0.0f)
        loop = false;

    enum_or(flags, GLITTER_EMITTER_INST_ENDED);
    if (data.flags & GLITTER_EMITTER_KILL_ON_END || free)
        for (GlitterF2ParticleInst*& i : particles)
            i->Free(true);
    else
        for (GlitterF2ParticleInst*& i : particles)
            i->Free(false);
}

bool GlitterF2EmitterInst::HasEnded(bool a2) {
    if (~flags & GLITTER_EMITTER_INST_ENDED)
        return false;
    else if (!a2)
        return true;

    for (GlitterF2ParticleInst*& i : particles)
        if (!i->HasEnded(a2))
            return false;
    return true;
}

void GlitterF2EmitterInst::Reset() {
    loop = data.flags & GLITTER_EMITTER_LOOP ? true : false;
    frame = -(float_t)data.start_time;
    flags = GLITTER_EMITTER_INST_NONE;
    emission_timer = 0.0f;
    if (emission_interval >= -0.000001f)
        emission = fabsf(emission_interval) <= 0.000001f
        ? GLITTER_EMITTER_EMISSION_ON_START : GLITTER_EMITTER_EMISSION_ON_TIMER;
    else
        emission = GLITTER_EMITTER_EMISSION_ON_END;

    for (GlitterF2ParticleInst*& i : particles)
        i->Reset();
}

static void GlitterF2EmitterInst_emit_particle(GPM, GLT,
    GlitterF2EmitterInst* a1, float_t emission) {
    int32_t count;
    if (a1->data.type == GLITTER_EMITTER_POLYGON)
        count = a1->data.polygon.count;
    else
        count = 1;

    for (GlitterF2ParticleInst*& i : a1->particles)
        i->Emit(GPM_VAL, GLT_VAL, (int32_t)roundf(a1->particles_per_emission), count, emission);
}

static void GlitterF2EmitterInst_get_value(GLT, GlitterF2EmitterInst* a1) {
    int64_t length;
    GlitterCurve* curve;
    float_t value;

    length = a1->emitter->animation.curves.size();
    if (!length)
        return;

    for (int32_t i = 0; i < length; i++) {
        curve = a1->emitter->animation.curves.data()[i];
        if (!glitter_curve_get_value(GLT_VAL, curve, a1->frame,
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

static void GlitterF2EmitterInst_ctrl_mat(GPM, GLT,
    GlitterF2EmitterInst* a1, GlitterF2EffectInst* a2) {
    bool mult;
    vec3 trans;
    vec3 rot;
    vec3 scale;
    mat4 mat;
    mat4 dir_mat;

    trans = a1->translation;
    rot = a1->rotation;
    vec3_mult_scalar(a1->scale, a1->scale_all, scale);

    mult = true;
    switch (a1->data.direction) {
    case GLITTER_DIRECTION_BILLBOARD:
        mat4_from_mat3(&GPM_VAL->cam_inv_view_mat3, &dir_mat);
        mat4_mult(&a2->mat, &dir_mat, &dir_mat);
        mat4_clear_trans(&dir_mat, &dir_mat);
        break;
    case GLITTER_DIRECTION_Y_AXIS:
        mat4_rotate_y((float_t)-M_PI_2, &dir_mat);
        break;
    case GLITTER_DIRECTION_X_AXIS:
        mat4_rotate_x((float_t)-M_PI_2, &dir_mat);
        break;
    case GLITTER_DIRECTION_Z_AXIS:
        mat4_rotate_z((float_t)-M_PI_2, &dir_mat);
        break;
    case GLITTER_DIRECTION_BILLBOARD_Y_AXIS:
        mat4_rotate_y(GPM_VAL->cam_rotation_y, &dir_mat);
        break;
    default:
        mult = false;
        break;
    }

    mat4_translate_mult(&a2->mat, trans.x, trans.y, trans.z, &mat);
    mat4_normalize_rotation(&mat, &mat);
    if (mult)
        mat4_mult(&dir_mat, &mat, &mat);
    mat4_rot(&mat, rot.x, rot.y, rot.z, &mat);
    mat4_clear_trans(&mat, &a1->mat_rot);
    mat4_scale_rot(&mat, scale.x, scale.y, scale.z, &a1->mat);
}
