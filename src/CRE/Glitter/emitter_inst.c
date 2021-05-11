/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "emitter_inst.h"
#include "curve.h"
#include "particle_inst.h"
#include "random.h"
#include "../camera.h"

extern camera* cam;

static void FASTCALL glitter_emitter_inst_emit_particle(GPM,
    glitter_emitter_inst* a1, float_t emission);
static void FASTCALL glitter_emitter_inst_get_value(GPM,
    glitter_emitter_inst* a1, float_t frame);
static void FASTCALL glitter_emitter_inst_update_mat(GPM,
    glitter_emitter_inst* a1, glitter_effect_inst* a2);

glitter_emitter_inst* FASTCALL glitter_emitter_inst_init(GPM,
    glitter_emitter* a1, glitter_effect_inst* a2, float_t emission) {
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

    glitter_emitter_inst* ei = force_malloc(sizeof(glitter_emitter_inst));
    ei->emitter = a1;

    ei->data = a1->data;
    ei->random_ptr = &a2->random_shared;
    ei->random.step = 1;
    ei->translation = a1->translation;
    ei->rotation = a1->rotation;
    ei->scale = a1->scale;
    ei->mat = mat4_identity;
    ei->mat_no_scale = mat4_identity;
    ei->scale_all = 1.0f;
    ei->emission_interval = ei->data.emission_interval;
    ei->particles_per_emission = ei->data.particles_per_emission;
    ei->frame = (float_t)-ei->data.start_time;
    if (ei->data.emission_interval >= -0.000001f)
        ei->emission = fabs(ei->data.emission_interval) <= 0.000001f
        ? GLITTER_EMITTER_EMISSION_ON_START : GLITTER_EMITTER_EMISSION_ON_TIMER;
    else
        ei->emission = GLITTER_EMITTER_EMISSION_ON_END;
    ei->loop = ei->data.flags & GLITTER_EMITTER_LOOP ? true : false;

    vector_ptr_glitter_particle_inst_append(&ei->particles, a1->particles.end - a1->particles.begin);
    for (i = a1->particles.begin; i != a1->particles.end; i++) {
        if (!*i)
            continue;

        particle = glitter_particle_inst_init(GPM_VAL, *i, a2, ei, ei->random_ptr, emission);
        vector_ptr_glitter_particle_inst_push_back(&ei->particles, &particle);
    }

    ei->random.value = glitter_random_get_value(ei->random_ptr);
    glitter_random_step_value(ei->random_ptr);
    return ei;
}

void FASTCALL glitter_emitter_inst_free(GPM, glitter_emitter_inst* a1, float_t emission) {
    glitter_particle_inst** i;

    if (a1->ended)
        return;

    if (a1->emission == GLITTER_EMITTER_EMISSION_ON_END) {
        glitter_emitter_inst_emit_particle(GPM_VAL, a1, emission);
        a1->emission = GLITTER_EMITTER_EMISSION_EMITTED;
    }

    if (a1->loop && a1->data.loop_end_time >= 0.0f)
        a1->loop = false;

    a1->ended = true;
    if (a1->data.flags & GLITTER_EMITTER_KILL_ON_END)
        for (i = a1->particles.begin; i != a1->particles.end; ++i)
            glitter_particle_inst_free(*i, true);
    else
        for (i = a1->particles.begin; i != a1->particles.end; ++i)
            glitter_particle_inst_free(*i, false);
}

void FASTCALL glitter_emitter_inst_emit(GPM,
    glitter_emitter_inst* a1, float_t delta_frame, float_t emission) {
    if (a1->frame < 0.0f) {
        a1->frame += delta_frame;
        return;
    }

    if (!a1->ended) {
        if (a1->emission == GLITTER_EMITTER_EMISSION_ON_TIMER) {
            if (a1->emission_timer >= 0.0f || a1->emission_interval >= 0.0f) {
                a1->emission_timer -= delta_frame;
                while (a1->emission_timer <= 0.0) {
                    glitter_emitter_inst_emit_particle(GPM_VAL, a1, emission);
                    if (a1->emission_interval > 0.0f)
                        a1->emission_timer += a1->emission_interval;
                    else
                        a1->emission_timer = -1.0;
                }
            }
        }
        else if (a1->emission == GLITTER_EMITTER_EMISSION_ON_START && a1->data.timer == GLITTER_EMITTER_TIMER_BY_TIME) {
            a1->emission_timer -= delta_frame;
            if (a1->emission_timer < 0.0) {
                glitter_emitter_inst_emit_particle(GPM_VAL, a1, emission);
                a1->emission = GLITTER_EMITTER_EMISSION_EMITTED;
            }
        }
    }

    if (!a1->loop && a1->frame >= a1->data.life_time)
        glitter_emitter_inst_free(GPM_VAL, a1, emission);
    a1->frame += delta_frame;
}

void FASTCALL glitter_emitter_inst_get_mesh_by_type(GPM, glitter_emitter_inst* a1,
    int32_t index, vec3* scale, vec3* base_translation, vec3* direction, glitter_random* random) {
    float_t radius;
    float_t angle;
    float_t longitude;
    float_t latitude;
    vec3 dir;

    switch (a1->data.type) {
    case GLITTER_EMITTER_BOX:
        vec3_mult(a1->data.box.size, *scale, dir);
        vec3_mult_scalar(dir, 0.5f, dir);
        glitter_random_get_vec3(GPM_VAL, random, &dir, base_translation);
        break;
    case GLITTER_EMITTER_CYLINDER:
        radius = a1->data.cylinder.radius * scale->x;
        if (!a1->data.cylinder.on_edge)
            radius = glitter_random_get_float_min_max(GPM_VAL, random, 0.0f, radius);
        angle = glitter_random_get_float_min_max(GPM_VAL, random,
            a1->data.cylinder.start_angle, a1->data.cylinder.end_angle);
        dir.x = cosf(angle);
        dir.y = 0.0f;
        dir.z = sinf(angle);
        base_translation->x = dir.x * radius;
        base_translation->y = glitter_random_get_float(GPM_VAL, random,
            a1->data.cylinder.height * scale->y * 0.5f);
        base_translation->z = dir.z * radius;
        if (a1->data.cylinder.direction == GLITTER_EMITTER_EMISSION_DIRECTION_OUTWARD)
            *direction = dir;
        else if (a1->data.cylinder.direction == GLITTER_EMITTER_EMISSION_DIRECTION_INWARD)
            vec3_xor(dir, ((vec3){ -0.0f, 0.0f, -0.0f }), *direction);
        break;
    case GLITTER_EMITTER_SPHERE:
        radius = a1->data.sphere.radius * scale->x;
        if (!a1->data.sphere.on_edge)
            radius = glitter_random_get_float_min_max(GPM_VAL, random, 0.0f, radius);
        longitude = glitter_random_get_float_min_max(GPM_VAL, random,
            a1->data.sphere.longitude * -0.5f, a1->data.sphere.longitude * 0.5f);
        latitude = (float_t)M_PI_2 - glitter_random_get_float_min_max(GPM_VAL, random,
            0.0f, a1->data.sphere.latitude);
        dir.x = sinf(longitude) * cosf(latitude);
        dir.y = sinf(latitude);
        dir.z = cosf(longitude) * cosf(latitude);
        vec3_mult_scalar(dir, radius, *base_translation);
        if (a1->data.sphere.direction == GLITTER_EMITTER_EMISSION_DIRECTION_OUTWARD)
            *direction = dir;
        else if (a1->data.sphere.direction == GLITTER_EMITTER_EMISSION_DIRECTION_INWARD)
            vec3_negate(dir, *direction);
        break;
    case GLITTER_EMITTER_POLYGON:
        base_translation->x = sinf((float_t)index / (float_t)a1->data.polygon.count * 2.0f
            * (float_t)M_PI + (float_t)M_PI_2) * a1->data.polygon.size * scale->x;
        base_translation->y = 0.0f;
        break;
    }
}

bool FASTCALL glitter_emitter_inst_has_ended(glitter_emitter_inst* emitter, bool a2) {
    glitter_particle_inst** i;

    if (!emitter->ended)
        return false;
    else if (!a2)
        return true;

    for (i = emitter->particles.begin; i != emitter->particles.end; i++)
        if (!glitter_particle_inst_has_ended(*i, a2))
            return false;
    return true;
}

uint8_t FASTCALL glitter_emitter_inst_random_get_step(glitter_emitter_inst* emitter) {
    emitter->random.step++;
    emitter->random.step++;
    return (uint8_t)(emitter->random.step % 60);
}

void FASTCALL glitter_emitter_inst_reset(glitter_emitter_inst* a1) {
    glitter_particle_inst** i;

    a1->loop = a1->data.flags & GLITTER_EMITTER_LOOP ? true : false;
    a1->frame = (float_t)-a1->data.start_time;
    a1->ended = false;
    a1->emission_timer = 0.0f;
    if (a1->emission_interval >= -0.000001f)
        a1->emission = fabsf(a1->emission_interval) <= 0.000001f
        ? GLITTER_EMITTER_EMISSION_ON_START : GLITTER_EMITTER_EMISSION_ON_TIMER;
    else
        a1->emission = GLITTER_EMITTER_EMISSION_ON_END;

    for (i = a1->particles.begin; i != a1->particles.end; ++i)
        glitter_particle_inst_reset(*i);
}

void FASTCALL glitter_emitter_inst_update_value_frame(GPM,
    glitter_emitter_inst* a1, glitter_effect_inst* a2, float_t delta_frame) {
    vec3 rotation_add;

    if (a1->frame < 0.0f)
        return;

    if (a1->loop) {
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
    }

    glitter_emitter_inst_get_value(GPM_VAL, a1, a1->frame);
    vec3_mult_scalar(a1->data.rotation_add, delta_frame, rotation_add);
    vec3_add(a1->rotation, rotation_add, a1->rotation);
    glitter_emitter_inst_update_mat(GPM_VAL, a1, a2);
}

void FASTCALL glitter_emitter_inst_update_value_init(GPM,
    glitter_emitter_inst* a1, glitter_effect_inst* a2, float_t delta_frame) {
    vec3 rotation_add;

    a1->has_distance = false;
    if (a1->frame < 0.0f)
        return;

    if (a1->loop) {
        if (a1->data.loop_end_time < 0.0f || a1->frame < a1->data.loop_end_time) {
            if (a1->frame >= a1->data.life_time)
                a1->frame -= a1->data.life_time;
        }
        else
            while (a1->frame > a1->data.loop_end_time)
                a1->frame -= a1->data.loop_end_time - a1->data.loop_start_time;
    }

    if (a1->frame < 0.0f)
        return;

    glitter_emitter_inst_get_value(GPM_VAL, a1, a1->frame);
    vec3_mult_scalar(a1->data.rotation_add, delta_frame, rotation_add);
    vec3_add(a1->rotation, rotation_add, a1->rotation);
    glitter_emitter_inst_update_mat(GPM_VAL, a1, a2);
}

void FASTCALL glitter_emitter_inst_dispose(glitter_emitter_inst* ei) {
    vector_ptr_glitter_particle_inst_free(&ei->particles, glitter_particle_inst_dispose);
    free(ei);
}

static void FASTCALL glitter_emitter_inst_emit_particle(GPM,
    glitter_emitter_inst* a1, float_t emission) {
    int32_t count;
    glitter_particle_inst** i;

    if (a1->data.type == GLITTER_EMITTER_POLYGON)
        count = a1->data.polygon.count;
    else
        count = 1;

    for (i = a1->particles.begin; i != a1->particles.end; ++i)
        glitter_particle_inst_emit(GPM_VAL, *i, (int32_t)roundf(a1->particles_per_emission), count, emission);
}

static void FASTCALL glitter_emitter_inst_get_value(GPM,
    glitter_emitter_inst* a1, float_t frame) {
    int64_t length;
    glitter_curve* curve;
    float_t value;

    length = a1->emitter->curve.end - a1->emitter->curve.begin;
    if (!length)
        return;

    for (int32_t i = 0; i < length; i++) {
        curve = a1->emitter->curve.begin[i];
        if (!glitter_curve_get_value(GPM_VAL, curve, frame, &value, a1->random.value + i, a1->random_ptr))
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

static void FASTCALL glitter_emitter_inst_update_mat(GPM,
    glitter_emitter_inst* a1, glitter_effect_inst* a2) {
    bool mult;
    vec3 trans;
    vec3 rot;
    vec3 scale;
    mat4 mat;
    mat4 direction_mat;

    trans = a1->translation;
    rot = a1->rotation;
    vec3_mult_scalar(a1->scale, a1->scale_all, scale);

    mult = true;
    switch (a1->data.direction) {
    case GLITTER_DIRECTION_BILLBOARD: {
        mat4_invrot(&cam->view, &direction_mat);
        direction_mat.row3 = (vec4){ 0.0f, 0.0f, 0.0f, 1.0f };
        mat4_mult(&a2->mat, &direction_mat, &direction_mat);
        direction_mat.row3 = (vec4){ 0.0f, 0.0f, 0.0f, 1.0f };
    } break;
    case GLITTER_DIRECTION_Y_AXIS:
        mat4_rotate_y((float_t)-M_PI_2, &direction_mat);
        break;
    case GLITTER_DIRECTION_X_AXIS:
        mat4_rotate_x((float_t)-M_PI_2, &direction_mat);
        break;
    case GLITTER_DIRECTION_Z_AXIS:
        mat4_rotate_z((float_t)-M_PI_2, &direction_mat);
        break;
    case GLITTER_DIRECTION_BILLBOARD_Y_ONLY:
        mat4_rotate_y(cam->rotation.y, &direction_mat);
        break;
    default:
        mult = false;
        break;
    }

    mat4_translate_mult(&a2->mat, trans.x, trans.y, trans.z, &mat);
    mat4_normalize_rotation(&mat, &mat);
    if (mult)
        mat4_mult(&direction_mat, &mat, &mat);
    mat4_rot(&mat, rot.x, rot.y, rot.z, &mat);
    mat4_scale_rot(&mat, scale.x, scale.y, scale.z, &a1->mat);
    a1->mat_no_scale = mat;
}
