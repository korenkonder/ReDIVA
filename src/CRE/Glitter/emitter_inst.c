/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "emitter_inst.h"
#include "curve.h"
#include "particle_inst.h"
#include "random.h"

static void glitter_emitter_inst_emit_particle(GPM, GLT,
    glitter_emitter_inst* a1, float_t emission);
static void glitter_emitter_inst_get_value(GLT, glitter_emitter_inst* a1);
static void glitter_emitter_inst_ctrl_mat(GPM, GLT,
    glitter_emitter_inst* a1, glitter_effect_inst* a2);

glitter_emitter_inst* glitter_emitter_inst_init(glitter_emitter* a1,
    glitter_effect_inst* a2, float_t emission) {
    glitter_particle** i;
    glitter_particle_inst* particle;

    switch (a1->data.type) {
    case GLITTER_EMITTER_BOX:
    case GLITTER_EMITTER_CYLINDER:
    case GLITTER_EMITTER_SPHERE:
    case GLITTER_EMITTER_POLYGON:
        break;
    default:
        return 0;
    }

    glitter_emitter_inst* ei = force_malloc_s(glitter_emitter_inst, 1);
    ei->emitter = a1;

    ei->random_ptr = a2->random_ptr;
    ei->data = a1->data;
    ei->translation = a1->translation;
    ei->rotation = a1->rotation;
    ei->scale = a1->scale;
    ei->mat = mat4_identity;
    ei->mat_rot = mat4_identity;
    ei->scale_all = 1.0f;
    ei->emission_interval = ei->data.emission_interval;
    ei->particles_per_emission = ei->data.particles_per_emission;
    ei->frame = (float_t)-ei->data.start_time;
    if (ei->data.emission_interval >= -0.000001f)
        ei->emission = ei->data.emission_interval <= 0.000001f
        ? GLITTER_EMITTER_EMISSION_ON_START : GLITTER_EMITTER_EMISSION_ON_TIMER;
    else
        ei->emission = GLITTER_EMITTER_EMISSION_ON_END;
    ei->loop = ei->data.flags & GLITTER_EMITTER_LOOP ? true : false;

    vector_old_ptr_glitter_particle_inst_reserve(&ei->particles, vector_old_length(a1->particles));
    for (i = a1->particles.begin; i != a1->particles.end; i++) {
        if (!*i)
            continue;

        particle = glitter_particle_inst_init(*i, a2, ei, ei->random_ptr, emission);
        if (particle)
            vector_old_ptr_glitter_particle_inst_push_back(&ei->particles, &particle);
    }

    ei->random = glitter_random_get_value(ei->random_ptr);
    glitter_random_set_value(ei->random_ptr, glitter_random_get_value(ei->random_ptr) + 1);
    return ei;
}

void glitter_emitter_inst_emit(GPM, GLT,
    glitter_emitter_inst* a1, float_t delta_frame, float_t emission) {
    if (a1->frame < 0.0f) {
        a1->frame += delta_frame;
        return;
    }

    if (~a1->flags & GLITTER_EMITTER_INST_ENDED)
        if (a1->emission == GLITTER_EMITTER_EMISSION_ON_TIMER) {
            if (a1->emission_timer >= 0.0f || a1->emission_interval >= 0.0f) {
                a1->emission_timer -= delta_frame;
                if (a1->emission_timer <= 0.0) {
                    glitter_emitter_inst_emit_particle(GPM_VAL, GLT_VAL, a1, emission);
                    if (a1->emission_interval > 0.0f)
                        a1->emission_timer += a1->emission_interval;
                    else
                        a1->emission_timer = -1.0;
                }
            }
        }
        else if (a1->emission == GLITTER_EMITTER_EMISSION_ON_START) {
            a1->emission_timer -= delta_frame;
            if (a1->emission_timer <= 0.0) {
                glitter_emitter_inst_emit_particle(GPM_VAL, GLT_VAL, a1, emission);
                a1->emission = GLITTER_EMITTER_EMISSION_EMITTED;
            }
        }

    if (!a1->loop && a1->frame >= a1->data.life_time)
        glitter_emitter_inst_free(GPM_VAL, GLT_VAL, a1, emission, false);
    a1->frame += delta_frame;
}

void glitter_emitter_inst_free(GPM, GLT, glitter_emitter_inst* a1, float_t emission, bool free) {
    glitter_particle_inst** i;

    if (a1->flags & GLITTER_EMITTER_INST_ENDED) {
        if (free)
            for (i = a1->particles.begin; i != a1->particles.end; ++i)
                glitter_particle_inst_free(*i, true);
        return;
    }

    if (a1->emission == GLITTER_EMITTER_EMISSION_ON_END) {
        glitter_emitter_inst_emit_particle(GPM_VAL, GLT_VAL, a1, emission);
        a1->emission = GLITTER_EMITTER_EMISSION_EMITTED;
    }

    if (a1->loop && a1->data.loop_end_time >= 0.0f)
        a1->loop = false;

    enum_or(a1->flags, GLITTER_EMITTER_INST_ENDED);
    if (a1->data.flags & GLITTER_EMITTER_KILL_ON_END || free)
        for (i = a1->particles.begin; i != a1->particles.end; i++)
            glitter_particle_inst_free(*i, true);
    else
        for (i = a1->particles.begin; i != a1->particles.end; i++)
            glitter_particle_inst_free(*i, false);
}

bool glitter_emitter_inst_has_ended(glitter_emitter_inst* emitter, bool a2) {
    glitter_particle_inst** i;

    if (~emitter->flags & GLITTER_EMITTER_INST_ENDED)
        return false;
    else if (!a2)
        return true;

    for (i = emitter->particles.begin; i != emitter->particles.end; i++)
        if (!glitter_particle_inst_has_ended(*i, a2))
            return false;
    return true;
}

void glitter_emitter_inst_reset(glitter_emitter_inst* a1) {
    glitter_particle_inst** i;

    a1->loop = a1->data.flags & GLITTER_EMITTER_LOOP ? true : false;
    a1->frame = (float_t)-a1->data.start_time;
    a1->flags = GLITTER_EMITTER_INST_NONE;
    a1->emission_timer = 0.0f;
    if (a1->emission_interval >= -0.000001f)
        a1->emission = fabsf(a1->emission_interval) <= 0.000001f
        ? GLITTER_EMITTER_EMISSION_ON_START : GLITTER_EMITTER_EMISSION_ON_TIMER;
    else
        a1->emission = GLITTER_EMITTER_EMISSION_ON_END;

    for (i = a1->particles.begin; i != a1->particles.end; ++i)
        glitter_particle_inst_reset(*i);
}

void glitter_emitter_inst_ctrl(GPM, GLT,
    glitter_emitter_inst* a1, glitter_effect_inst* a2, float_t delta_frame) {
    vec3 rotation_add;

    if (a1->frame < 0.0f)
        return;

    if (a1->loop)
        if (a1->data.loop_end_time < 0.0f || a1->frame < a1->data.loop_end_time) {
            if (a1->frame >= a1->data.life_time) {
                a1->frame -= a1->data.life_time;
                if (a1->emission == GLITTER_EMITTER_EMISSION_EMITTED && a1->emission_interval > 0.0f)
                    a1->emission = GLITTER_EMITTER_EMISSION_ON_TIMER;
            }
        }
        else
            while (a1->frame > a1->data.loop_end_time)
                a1->frame -= a1->data.loop_end_time - a1->data.loop_start_time;

    glitter_emitter_inst_get_value(GLT_VAL, a1);
    vec3_mult_scalar(a1->data.rotation_add, delta_frame, rotation_add);
    vec3_add(a1->rotation, rotation_add, a1->rotation);
    glitter_emitter_inst_ctrl_mat(GPM_VAL, GLT_VAL, a1, a2);
}

void glitter_emitter_inst_ctrl_init(GPM, GLT,
    glitter_emitter_inst* a1, glitter_effect_inst* a2, float_t delta_frame) {
    vec3 rotation_add;

    if (a1->frame < 0.0f)
        return;

    if (a1->loop)
        if (a1->data.loop_end_time < 0.0f || a1->frame < a1->data.loop_end_time) {
            if (a1->frame >= a1->data.life_time)
                a1->frame -= a1->data.life_time;
        }
        else
            while (a1->frame > a1->data.loop_end_time)
                a1->frame -= a1->data.loop_end_time - a1->data.loop_start_time;

    if (a1->frame < 0.0f)
        return;

    glitter_emitter_inst_get_value(GLT_VAL, a1);
    vec3_mult_scalar(a1->data.rotation_add, delta_frame, rotation_add);
    vec3_add(a1->rotation, rotation_add, a1->rotation);
    glitter_emitter_inst_ctrl_mat(GPM_VAL, GLT_VAL, a1, a2);
}

void glitter_emitter_inst_dispose(glitter_emitter_inst* ei) {
    vector_old_ptr_glitter_particle_inst_free(&ei->particles, glitter_particle_inst_dispose);
    free(ei);
}

static void glitter_emitter_inst_emit_particle(GPM, GLT,
    glitter_emitter_inst* a1, float_t emission) {
    int32_t count;
    glitter_particle_inst** i;

    if (a1->data.type == GLITTER_EMITTER_POLYGON)
        count = a1->data.polygon.count;
    else
        count = 1;

    for (i = a1->particles.begin; i != a1->particles.end; ++i)
        if (*i)
            glitter_particle_inst_emit(GPM_VAL, GLT_VAL, *i,
                (int32_t)roundf(a1->particles_per_emission), count, emission);
}

static void glitter_emitter_inst_get_value(GLT, glitter_emitter_inst* a1) {
    int64_t length;
    glitter_curve* curve;
    float_t value;

    length = vector_old_length(a1->emitter->animation);
    if (!length)
        return;

    for (int32_t i = 0; i < length; i++) {
        curve = a1->emitter->animation.begin[i];
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

static void glitter_emitter_inst_ctrl_mat(GPM, GLT,
    glitter_emitter_inst* a1, glitter_effect_inst* a2) {
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
