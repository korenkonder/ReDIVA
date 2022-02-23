/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "emitter_inst_x.h"
#include "curve_x.h"
#include "particle_inst_x.h"
#include "random_x.h"

static void glitter_x_emitter_inst_emit_particle(glitter_emitter_inst* a1, float_t emission);
static void glitter_x_emitter_inst_get_value(glitter_emitter_inst* a1);

glitter_emitter_inst* glitter_x_emitter_inst_init(glitter_emitter* a1,
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

    ei->random_ptr = &a2->random_shared;
    if (a2->data.flags & GLITTER_EFFECT_USE_SEED)
        ei->random = glitter_x_random_get_value(ei->random_ptr) + 1;
    else if (ei->data.flags & GLITTER_EMITTER_USE_SEED)
        ei->random = ei->data.seed;
    else
        ei->random = glitter_x_random_get_value(ei->random_ptr);
    ei->step = 1;
    glitter_x_random_step_value(ei->random_ptr);

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
    if (ei->data.timer == GLITTER_EMITTER_TIMER_BY_TIME)
        if (ei->data.emission_interval >= -0.000001f)
            ei->emission = ei->data.emission_interval <= 0.000001f
            ? GLITTER_EMITTER_EMISSION_ON_START : GLITTER_EMITTER_EMISSION_ON_TIMER;
        else
            ei->emission = GLITTER_EMITTER_EMISSION_ON_END;
    else
        ei->emission = GLITTER_EMITTER_EMISSION_ON_TIMER;
    ei->loop = ei->data.flags & GLITTER_EMITTER_LOOP ? true : false;

    vector_old_ptr_glitter_particle_inst_reserve(&ei->particles, vector_old_length(a1->particles));
    for (i = a1->particles.begin; i != a1->particles.end; i++) {
        if (!*i)
            continue;

        particle = glitter_x_particle_inst_init(*i, a2, ei, ei->random_ptr, emission);
        if (particle)
            vector_old_ptr_glitter_particle_inst_push_back(&ei->particles, &particle);
    }
    return ei;
}

void glitter_x_emitter_inst_ctrl(GPM,
    glitter_emitter_inst* a1, glitter_effect_inst* a2, float_t delta_frame) {
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

    if (a1->frame < 0.0f)
        return;

    if (a1->loop) {
        float_t loop_time = (float_t)(a1->data.loop_end_time - a1->data.loop_start_time);
        if (a1->data.loop_end_time < 0.0f || a1->frame < a1->data.loop_end_time) {
            if (a1->frame >= a1->data.life_time) {
                if (a1->data.life_time > 0.0f)
                    while (a1->frame >= a1->data.life_time)
                        a1->frame -= a1->data.life_time;
                else
                    a1->frame = 0.0f;

                if (a1->emission == GLITTER_EMITTER_EMISSION_EMITTED && a1->emission_interval > 0.0f)
                    a1->emission = GLITTER_EMITTER_EMISSION_ON_TIMER;
            }
        }
        else if (loop_time > 0.0f)
            while (a1->frame >= a1->data.loop_end_time)
                a1->frame -= loop_time;
        else
            a1->frame = 0.0f;
    }

    glitter_x_emitter_inst_get_value(a1);
    vec3_mult_scalar(a1->data.rotation_add, delta_frame, rotation_add);
    vec3_add(a1->rotation, rotation_add, a1->rotation);

    trans = a1->translation;
    rot = a1->rotation;
    vec3_mult_scalar(a1->scale, a1->scale_all, scale);

    trans_prev = vec3_null;
    has_dist = false;
    if (a1->data.timer == GLITTER_EMITTER_TIMER_BY_DISTANCE && a1->flags & GLITTER_EMITTER_INST_HAS_DISTANCE) {
        mat4_get_translation(&a1->mat, &trans_prev);
        has_dist = true;
    }

    mat4_translate_mult(&a2->mat, trans.x, trans.y, trans.z, &mat);
    mat4_normalize_rotation(&mat, &mat);

    if (a1->data.direction == GLITTER_DIRECTION_EFFECT_ROTATION)
        mat_rot = a2->mat_rot_eff_rot;
    else {
        mat4_clear_rot(&mat, &mat);
        mat_rot = a2->mat_rot;
    }

    mult = true;
    switch (a1->data.direction) {
    case GLITTER_DIRECTION_BILLBOARD:
        if (a2->data.flags & GLITTER_EFFECT_LOCAL) {
            mat4_clear_trans(&GPM_VAL->cam_view, &mat1);
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
    mat4_rot(&mat_rot, rot.x, rot.y, rot.z, &a1->mat_rot);
    mat4_scale_rot(&mat, scale.x, scale.y, scale.z, &mat);
    a1->mat = mat;

    if (has_dist) {
        mat4_get_translation(&mat, &trans);

        float_t trans_dist;
        vec3_distance(trans, trans_prev, trans_dist);
        a1->emission_timer -= trans_dist;
    }

    enum_or(a1->flags, GLITTER_EMITTER_INST_HAS_DISTANCE);
}

void glitter_x_emitter_inst_ctrl_init(glitter_emitter_inst* a1,
    glitter_effect_inst* a2, float_t delta_frame) {
    if (a1->frame < 0.0f)
        return;

    if (a1->loop) {
        float_t loop_time = (float_t)(a1->data.loop_end_time - a1->data.loop_start_time);
        if (a1->data.loop_end_time < 0.0f || a1->frame < a1->data.loop_end_time) {
            if (a1->frame >= a1->data.life_time) {
                if (a1->data.life_time > 0.0f)
                    while (a1->frame >= a1->data.life_time)
                        a1->frame -= a1->data.life_time;
                else
                    a1->frame = 0.0f;

                if (a1->emission == GLITTER_EMITTER_EMISSION_EMITTED && a1->emission_interval > 0.0f)
                    a1->emission = GLITTER_EMITTER_EMISSION_ON_TIMER;
            }
        }
        else if (loop_time > 0.0f)
            while (a1->frame >= a1->data.loop_end_time)
                a1->frame -= loop_time;
        else
            a1->frame = 0.0f;
    }

    glitter_x_emitter_inst_get_value(a1);

    vec3 rotation_add;
    vec3_mult_scalar(a1->data.rotation_add, delta_frame, rotation_add);
    vec3_add(a1->rotation, rotation_add, a1->rotation);
    if (a1->data.timer == GLITTER_EMITTER_TIMER_BY_DISTANCE && a1->flags & GLITTER_EMITTER_INST_HAS_DISTANCE) {
        vec3 trans_prev;
        mat4 mat;
        mat4 mat_ext_anim;

        if (a1->data.direction == GLITTER_DIRECTION_EFFECT_ROTATION)
            mat_ext_anim = a2->mat_rot_eff_rot;
        else
            mat_ext_anim = a2->mat_rot;

        mat4_get_translation(&a1->mat, &trans_prev);
        vec3 trans = a1->translation;
        vec3 rot = a1->rotation;
        mat4_translate_mult(&a2->mat, trans.x, trans.y, trans.z, &mat);
        mat4_rot(&mat, rot.x, rot.y, rot.z, &mat);
        mat4_rot(&mat_ext_anim, rot.x, rot.y, rot.z, &a1->mat_rot);
        a1->mat = mat;

        float_t trans_dist;
        mat4_get_translation(&mat, &trans);
        vec3_distance(trans, trans_prev, trans_dist);
        a1->emission_timer -= trans_dist;
    }
    enum_or(a1->flags, GLITTER_EMITTER_INST_HAS_DISTANCE);
}

void glitter_x_emitter_inst_emit(glitter_emitter_inst* a1,
    float_t delta_frame, float_t emission) {
    if (a1->frame < 0.0f) {
        a1->frame += delta_frame;
        return;
    }

    if (~a1->flags & GLITTER_EMITTER_INST_ENDED)
        if (a1->emission == GLITTER_EMITTER_EMISSION_ON_TIMER) {
            if (a1->data.timer == GLITTER_EMITTER_TIMER_BY_DISTANCE) {
                if (a1->emission_timer <= 0.0f || a1->emission_interval >= 0.0f)
                    while (a1->emission_timer <= 0.0f) {
                        glitter_x_emitter_inst_emit_particle(a1, emission);
                        a1->emission_timer += a1->emission_interval;
                        if (a1->emission_timer < -1000000.0f)
                            a1->emission_timer = -1000000.0f;

                        if (a1->emission_interval <= 0.0f)
                            break;
                    }
            }
            else if (a1->data.timer == GLITTER_EMITTER_TIMER_BY_TIME)
                if (a1->emission_timer >= 0.0f || a1->emission_interval >= 0.0f) {
                    a1->emission_timer -= delta_frame;
                    if (a1->emission_timer <= 0.0f) {
                        glitter_x_emitter_inst_emit_particle(a1, emission);
                        if (a1->emission_interval > 0.0)
                            a1->emission_timer += a1->emission_interval;
                        else
                            a1->emission_timer = -1.0f;
                    }
                }
        }
        else if (a1->emission == GLITTER_EMITTER_EMISSION_ON_START
            && a1->data.timer == GLITTER_EMITTER_TIMER_BY_TIME) {
            a1->emission_timer -= delta_frame;
            if (a1->emission_timer <= 0.0) {
                glitter_x_emitter_inst_emit_particle(a1, emission);
                a1->emission = GLITTER_EMITTER_EMISSION_EMITTED;
            }
        }

    if (!a1->loop && a1->frame >= a1->data.life_time)
        glitter_x_emitter_inst_free(a1, emission, false);

    a1->frame += delta_frame;
}

void glitter_x_emitter_inst_free(glitter_emitter_inst* a1, float_t emission, bool free) {
    glitter_particle_inst** i;

    if (a1->flags & GLITTER_EMITTER_INST_ENDED) {
        for (i = a1->particles.begin; i != a1->particles.end; ++i)
            glitter_x_particle_inst_free(*i, true);
        return;
    }

    if (a1->emission == GLITTER_EMITTER_EMISSION_ON_END) {
        glitter_x_emitter_inst_emit_particle(a1, emission);
        a1->emission = GLITTER_EMITTER_EMISSION_EMITTED;
    }

    if (a1->loop && a1->data.loop_end_time >= 0.0f)
        a1->loop = false;

    enum_or(a1->flags, GLITTER_EMITTER_INST_ENDED);
    if (a1->data.flags & GLITTER_EMITTER_KILL_ON_END || free)
        for (i = a1->particles.begin; i != a1->particles.end; ++i)
            glitter_x_particle_inst_free(*i, true);
    else
        for (i = a1->particles.begin; i != a1->particles.end; ++i)
            glitter_x_particle_inst_free(*i, false);
}

bool glitter_x_emitter_inst_has_ended(glitter_emitter_inst* emitter, bool a2) {
    glitter_particle_inst** i;

    if (~emitter->flags & GLITTER_EMITTER_INST_ENDED)
        return false;
    else if (!a2)
        return true;

    for (i = emitter->particles.begin; i != emitter->particles.end; i++)
        if (!glitter_x_particle_inst_has_ended(*i, a2))
            return false;
    return true;
}

uint8_t glitter_x_emitter_inst_random_get_step(glitter_emitter_inst* a1) {
    a1->step = (a1->step + 2) % 60;
    return a1->step;
}

void glitter_x_emitter_inst_random_set_value(glitter_emitter_inst* a1) {
    a1->counter += 11;
    a1->counter %= 30000;
    glitter_x_random_set_value(a1->random_ptr, a1->random + a1->counter);
}

void glitter_x_emitter_inst_reset(glitter_emitter_inst* a1) {
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
        glitter_x_particle_inst_reset(*i);
}

void glitter_x_emitter_inst_dispose(glitter_emitter_inst* ei) {
    vector_old_ptr_glitter_particle_inst_free(&ei->particles, glitter_x_particle_inst_dispose);
    free(ei);
}

static void glitter_x_emitter_inst_emit_particle(glitter_emitter_inst* a1, float_t emission) {
    int32_t count;
    glitter_particle_inst** i;

    if (a1->data.type == GLITTER_EMITTER_POLYGON)
        count = a1->data.polygon.count;
    else
        count = 1;

    for (i = a1->particles.begin; i != a1->particles.end; i++)
        if (*i)
            glitter_x_particle_inst_emit(*i,
                (int32_t)roundf(a1->particles_per_emission), count, emission);
}

static void glitter_x_emitter_inst_get_value(glitter_emitter_inst* a1) {
    int64_t length;
    glitter_curve* curve;
    float_t value;

    length = vector_old_length(a1->emitter->animation);
    if (!length)
        return;

    for (int32_t i = 0; i < length; i++) {
        curve = a1->emitter->animation.begin[i];
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
