/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "render_element.h"
#include "counter.h"
#include "curve.h"
#include "emitter_inst.h"
#include "locus_history.h"
#include "particle_inst.h"
#include "random.h"

static void glitter_render_element_accelerate(GLT, GlitterF2ParticleInst* a1,
    glitter_render_element* a2, float_t delta_frame, glitter_random* random);
static void glitter_render_element_init_mesh_by_type(GLT, glitter_render_element* a1,
    GlitterF2ParticleInstData* a2, GlitterF2EmitterInst* a3, int32_t index, glitter_random* random);
static void glitter_render_element_step_uv(GLT, GlitterF2ParticleInst* a1,
    glitter_render_element* a2, float_t delta_frame, glitter_random* random);

void glitter_render_element_emit(GPM, GLT, glitter_render_element* a1,
    GlitterF2ParticleInstData* a2, GlitterF2EmitterInst* a3, int32_t index, glitter_random* random) {
    glitter_random_set_value(random, glitter_counter_get(GPM_VAL));
    a1->random = glitter_random_get_int(GLT_VAL, random, glitter_random_get_max(GLT_VAL));
    a1->frame = 0.0f;
    a1->rebound_frame = 0.0f;
    a1->uv = vec2_null;
    a1->uv_index = a2->data.uv_index;

    a1->fade_in_frames = 0.0f;
    a1->fade_out_frames = 0.0f;
    a1->life_time = (float_t)a2->data.life_time;

    a1->color = a2->data.color;
    if (a2->data.draw_type == GLITTER_DIRECTION_PARTICLE_ROTATION) {
        a1->rotation.x = glitter_random_get_float(GLT_VAL, random, a2->data.rotation_random.x)
            + a2->data.rotation.x;
        a1->rotation_add.x = glitter_random_get_float(GLT_VAL, random, a2->data.rotation_add_random.x)
            + a2->data.rotation_add.x;
        a1->rotation.y = glitter_random_get_float(GLT_VAL, random, a2->data.rotation_random.y)
            + a2->data.rotation.y;
        a1->rotation_add.y = glitter_random_get_float(GLT_VAL, random, a2->data.rotation_add_random.y)
            + a2->data.rotation_add.y;
    }
    else {
        a1->rotation.x = 0.0f;
        a1->rotation.y = 0.0f;
        a1->rotation_add.x = 0.0f;
        a1->rotation_add.y = 0.0f;
    }
    a1->rotation.z = glitter_random_get_float(GLT_VAL, random, a2->data.rotation_random.z)
        + a2->data.rotation.z;
    a1->rotation_add.z = glitter_random_get_float(GLT_VAL, random, a2->data.rotation_add_random.z)
        + a2->data.rotation_add.z;
    a1->uv_scroll = vec2_null;
    a1->uv_scroll_2nd = vec2_null;
    a1->scale = vec3_identity;
    a1->scale_all = 1.0f;
    a1->frame_step_uv = (float_t)a2->data.frame_step_uv;

    a1->scale_particle.x = glitter_random_get_float(GLT_VAL, random, a2->data.scale_random.x)
        + a2->data.scale.x;
    if (a2->data.flags & GLITTER_PARTICLE_SCALE_Y_BY_X)
        a1->scale_particle.y = a1->scale_particle.x;
    else
        a1->scale_particle.y = glitter_random_get_float(GLT_VAL, random, a2->data.scale_random.y)
        + a2->data.scale.y;

    int32_t max_uv = a2->data.split_u * a2->data.split_v;
    if (max_uv > 1 && a2->data.uv_index_count > 1) {
        switch (a2->data.uv_index_type) {
        case GLITTER_UV_INDEX_INITIAL_RANDOM_FIXED:
        case GLITTER_UV_INDEX_INITIAL_RANDOM_FORWARD:
        case GLITTER_UV_INDEX_INITIAL_RANDOM_REVERSE:
            a1->uv_index += glitter_random_get_int(GLT_VAL, random, a2->data.uv_index_count);
            break;
        }
        a1->uv_index = min(a1->uv_index, a2->data.uv_index_end);
        a1->uv.x = (float_t)(a1->uv_index % a2->data.split_u) * a2->data.split_uv.x;
        a1->uv.y = (float_t)(a1->uv_index / a2->data.split_u) * a2->data.split_uv.y;
    }

    glitter_render_element_init_mesh_by_type(GLT_VAL, a1, a2, a3, index, random);

    bool copy_mat = false;
    if (a2->data.flags & GLITTER_PARTICLE_ROTATE_BY_EMITTER
        || a2->data.draw_type == GLITTER_DIRECTION_EMITTER_ROTATION) {
        switch (a3->data.type) {
        case GLITTER_EMITTER_CYLINDER:
            if (a3->data.cylinder.direction != GLITTER_EMITTER_EMISSION_DIRECTION_NONE) {
                float_t length;
                vec3_length(a1->direction, length);
                copy_mat = length <= 0.000001f;
            }
            else
                copy_mat = true;
            break;
        case GLITTER_EMITTER_SPHERE:
            if (a3->data.sphere.direction != GLITTER_EMITTER_EMISSION_DIRECTION_NONE) {
                float_t length;
                vec3_length(a1->direction, length);
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
        a1->mat = a3->mat;
        if (a2->data.flags & GLITTER_PARTICLE_EMITTER_LOCAL)
            mat4_clear_trans(&a1->mat, &a1->mat);
    }
    else
        a1->mat = mat4_identity;

    if (a2->data.type == GLITTER_PARTICLE_LOCUS) {
        uint32_t locus_history_size = glitter_random_get_int_min_max(GLT_VAL, random,
            -a2->data.locus_history_size_random, a2->data.locus_history_size_random)
            + a2->data.locus_history_size;
        a1->locus_history = new GlitterLocusHistory(locus_history_size);
    }
    glitter_random_set_value(random, glitter_random_get_value(random) + 1);
}

void glitter_render_element_ctrl(GLT,
    GlitterF2RenderGroup* a1, glitter_render_element* a2, float_t delta_frame) {
    GlitterF2ParticleInst* particle;
    vec2 uv_scroll;
    vec3 translation;
    bool has_translation;
    size_t length;
    GlitterCurve* curve;
    float_t value;

    particle = a1->particle;
    if (!particle || (particle->data.data.flags & GLITTER_PARTICLE_LOOP
        && particle->HasEnded(false)) || a2->frame >= a2->life_time) {
        a1->ctrl--;
        glitter_render_element_free(a2);
        return;
    }

    glitter_render_element_accelerate(GLT_VAL, particle, a2, delta_frame, a1->random_ptr);
    if (particle->data.data.draw_type == GLITTER_DIRECTION_PARTICLE_ROTATION) {
        a2->rotation.x += a2->rotation_add.x * delta_frame;
        a2->rotation.y += a2->rotation_add.y * delta_frame;
    }
    a2->rotation.z += a2->rotation_add.z * delta_frame;
    vec2_mult_scalar(particle->data.data.uv_scroll_add,
        particle->data.data.uv_scroll_add_scale * delta_frame, uv_scroll);
    vec2_add(a2->uv_scroll, uv_scroll, a2->uv_scroll);
    glitter_render_element_step_uv(GLT_VAL, particle, a2, delta_frame, a1->random_ptr);
    a2->color = { -1.0f, -1.0f, -1.0f, -1.0f };
    a2->disp = true;

    if (particle->data.data.sub_flags & GLITTER_PARTICLE_SUB_USE_CURVE) {
        value = 0.0f;
        has_translation = false;
        translation = vec3_null;

        GlitterAnimation* anim = &particle->particle->animation;
        length = anim->curves.size();
        for (int32_t i = 0; i < length; i++) {
            curve = anim->curves.data()[i];
            if (!glitter_curve_get_value(GLT_VAL, curve, a2->frame,
                &value, a2->random + i, a1->random_ptr))
                continue;

            switch (curve->type) {
            case GLITTER_CURVE_TRANSLATION_X:
                translation.x = value;
                has_translation = true;
                break;
            case GLITTER_CURVE_TRANSLATION_Y:
                translation.y = value;
                has_translation = true;
                break;
            case GLITTER_CURVE_TRANSLATION_Z:
                translation.z = value;
                has_translation = true;
                break;
            case GLITTER_CURVE_ROTATION_X:
                a2->rotation.x = value;
                break;
            case GLITTER_CURVE_ROTATION_Y:
                a2->rotation.y = value;
                break;
            case GLITTER_CURVE_ROTATION_Z:
                a2->rotation.z = value;
                break;
            case GLITTER_CURVE_SCALE_X:
                a2->scale.x = value;
                break;
            case GLITTER_CURVE_SCALE_Y:
                a2->scale.y = value;
                break;
            case GLITTER_CURVE_SCALE_Z:
                a2->scale.z = value;
                break;
            case GLITTER_CURVE_SCALE_ALL:
                a2->scale_all = value;
                break;
            case GLITTER_CURVE_COLOR_R:
                a2->color.x = value;
                break;
            case GLITTER_CURVE_COLOR_G:
                a2->color.y = value;
                break;
            case GLITTER_CURVE_COLOR_B:
                a2->color.z = value;
                break;
            case GLITTER_CURVE_COLOR_A:
                a2->color.w = value;
                if (value < 0.01f)
                    a2->disp = false;
                break;
            case GLITTER_CURVE_U_SCROLL:
                a2->uv_scroll.x = value;
                break;
            case GLITTER_CURVE_V_SCROLL:
                a2->uv_scroll.y = value;
                break;
            }
        }

        if (particle->data.data.flags & GLITTER_PARTICLE_ROTATE_BY_EMITTER || has_translation) {
            vec3_add(translation, a2->base_translation, translation);
            if (particle->data.data.flags & GLITTER_PARTICLE_ROTATE_BY_EMITTER)
                mat4_mult_vec3_trans(&a2->mat, &translation, &translation);
            a2->translation = translation;
        }
    }

    if (a2->disp) {
        if (a2->color.x < 0.0f)
            a2->color.x = particle->data.data.color.x;
        if (a2->color.y < 0.0f)
            a2->color.y = particle->data.data.color.y;
        if (a2->color.z < 0.0f)
            a2->color.z = particle->data.data.color.z;
        if (a2->color.w < 0.0f)
            a2->color.w = particle->data.data.color.w;

        if (a2->color.w < 0.01f)
            a2->disp = false;
    }

    if (particle->data.data.type == GLITTER_PARTICLE_LOCUS)
        a2->locus_history->Append(a2, particle);

    a2->frame += delta_frame;
    if (particle->data.data.flags & GLITTER_PARTICLE_LOOP && a2->frame >= a2->life_time)
        a2->frame -= a2->life_time;
}

void glitter_render_element_rotate_to_emit_position(mat4* mat,
    GlitterF2RenderGroup* a2, glitter_render_element* a3, vec3* vec) {
    vec3 vec1;
    vec3 vec2;
    float_t angle;
    vec3 axis;

    if (a2->flags & GLITTER_PARTICLE_EMITTER_LOCAL)
        vec2 = a3->translation;
    else {
        mat4_get_translation(&a2->mat, &vec2);
        vec3_sub(vec2, a3->translation, vec2);
    }
    vec3_normalize(vec2, vec2);

    if (fabsf(vec2.y) >= 0.000001f) {
        *mat = mat4_identity;
        vec1 = *vec;
        axis_angle_from_vectors(&axis, &angle, &vec1, &vec2);
        mat4_mult_axis_angle(mat, mat, &axis, angle);
    }
    else
        mat4_rotate_z((float_t)M_PI, mat);
}

void glitter_render_element_rotate_to_prev_position(mat4* mat,
    GlitterF2RenderGroup* a2, glitter_render_element* a3, vec3* vec) {
    vec3 vec1;
    vec3 vec2;
    float_t angle;
    vec3 axis;
    float_t length;

    vec3_sub(a3->translation, a3->translation_prev, vec2);
    vec3_length_squared(vec2, length);
    if (length < 0.000001f)
        vec2 = a3->translation;
    vec3_normalize(vec2, vec2);

    if (fabsf(vec2.y) >= 0.000001f) {
        mat4_rotate_z((float_t)M_PI, mat);
        vec1 = *vec;
        axis_angle_from_vectors(&axis, &angle, &vec1, &vec2);
        mat4_mult_axis_angle(mat, mat, &axis, angle);
    }
    else
        mat4_rotate_z((float_t)M_PI, mat);
}

void glitter_render_element_free(glitter_render_element* a1) {
    a1->alive = false;
    delete a1->locus_history;
    a1->locus_history = 0;
}

static void glitter_render_element_accelerate(GLT, GlitterF2ParticleInst* a1,
    glitter_render_element* a2, float_t delta_frame, glitter_random* random) {
    vec3 acceleration;
    vec3 direction;
    float_t diff_time;
    float_t delta_time;
    float_t reflection_coeff;
    float_t speed;

    a2->translation_prev = a2->translation;
    delta_time = delta_frame * (float_t)(1.0 / 60.0);
    diff_time = (a2->frame - a2->rebound_frame - delta_frame) * (float_t)(1.0 / 60.0);
    vec3_mult_scalar(a2->acceleration, delta_time * (delta_time * 0.5f + diff_time), acceleration);
    speed = a2->deceleration * delta_time * (delta_time * 0.5f - diff_time) + a2->speed;
    if (speed >= 0.01f) {
        vec3_mult_scalar(a2->direction, speed * delta_time, direction);
        vec3_add(acceleration, direction, acceleration);
    }
    vec3_add(a2->translation, acceleration, a2->translation);
    vec3_add(a2->base_translation, acceleration, a2->base_translation);
    if (a1->data.data.flags & GLITTER_PARTICLE_REBOUND_PLANE
        && a2->translation_prev.y > a1->data.data.rebound_plane_y
        && a2->translation.y <= a1->data.data.rebound_plane_y) {
        reflection_coeff = glitter_random_get_float(GLT_VAL, random,
            a1->data.data.reflection_coeff_random) + a1->data.data.reflection_coeff;
        a2->rebound_frame = a2->frame;
        vec3_sub(a2->translation, a2->translation_prev, direction);
        vec3_mult_scalar(direction, reflection_coeff * 60.0f, direction);
        const vec3 reverse_y_dir = { 0.0f, -0.0f, 0.0f };
        vec3_xor(direction, reverse_y_dir, a2->direction);
        a2->translation.y = a2->translation_prev.y;
    }
}

static void glitter_render_element_init_mesh_by_type(GLT, glitter_render_element* a1,
    GlitterF2ParticleInstData* a2, GlitterF2EmitterInst* a3, int32_t index, glitter_random* random) {
    float_t radius;
    float_t angle;
    float_t longitude;
    float_t latitude;
    vec3 dir;
    float_t speed;
    float_t deceleration;
    vec3 direction;
    vec3 base_translation;
    vec3 scale;
    vec3 acceleration;
    vec3 direction_random;

    if (a2->data.flags & (GLITTER_PARTICLE_EMITTER_LOCAL | GLITTER_PARTICLE_ROTATE_BY_EMITTER))
        a1->base_translation = vec3_null;
    else
        mat4_get_translation(&a3->mat, &a1->base_translation);

    direction = a2->data.direction;
    base_translation = vec3_null;
    if (a2->data.flags & GLITTER_PARTICLE_EMITTER_LOCAL)
        scale = vec3_identity;
    else
        vec3_mult_scalar(a3->scale, a3->scale_all, scale);

    switch (a3->data.type) {
    case GLITTER_EMITTER_BOX:
        vec3_mult(a3->data.box.size, scale, dir);
        vec3_mult_scalar(dir, 0.5f, dir);
        glitter_random_get_vec3(GLT_VAL, random, &dir, &base_translation);
        break;
    case GLITTER_EMITTER_CYLINDER:
        radius = a3->data.cylinder.radius * scale.x;
        if (!a3->data.cylinder.on_edge)
            radius = glitter_random_get_float_min_max(GLT_VAL, random, 0.0f, radius);
        angle = glitter_random_get_float_min_max(GLT_VAL, random,
            a3->data.cylinder.start_angle, a3->data.cylinder.end_angle);
        dir.x = cosf(angle);
        dir.y = 0.0f;
        dir.z = sinf(angle);
        base_translation.x = dir.x * radius;
        base_translation.y = glitter_random_get_float(GLT_VAL, random,
            a3->data.cylinder.height * scale.y * 0.5f);
        base_translation.z = dir.z * radius;
        if (a3->data.cylinder.direction == GLITTER_EMITTER_EMISSION_DIRECTION_OUTWARD)
            direction = dir;
        else if (a3->data.cylinder.direction == GLITTER_EMITTER_EMISSION_DIRECTION_INWARD) {
            const vec3 reverse_xz_dir = { -0.0f, 0.0f, -0.0f };
            vec3_xor(dir, reverse_xz_dir, direction);
        }
        break;
    case GLITTER_EMITTER_SPHERE:
        radius = a3->data.sphere.radius * scale.x;
        if (!a3->data.sphere.on_edge)
            radius = glitter_random_get_float_min_max(GLT_VAL, random, 0.0f, radius);
        longitude = glitter_random_get_float(GLT_VAL, random, a3->data.sphere.longitude * 0.5f);
        latitude = (float_t)M_PI_2 - glitter_random_get_float_min_max(GLT_VAL, random,
            0.0f, a3->data.sphere.latitude);
        dir.x = sinf(longitude) * cosf(latitude);
        dir.y = sinf(latitude);
        dir.z = cosf(longitude) * cosf(latitude);
        vec3_mult_scalar(dir, radius, base_translation);
        if (a3->data.sphere.direction == GLITTER_EMITTER_EMISSION_DIRECTION_OUTWARD)
            direction = dir;
        else if (a3->data.sphere.direction == GLITTER_EMITTER_EMISSION_DIRECTION_INWARD)
            vec3_negate(dir, direction);
        break;
    case GLITTER_EMITTER_POLYGON:
        base_translation.x = sinf((float_t)index / (float_t)a3->data.polygon.count * 2.0f
            * (float_t)M_PI + (float_t)M_PI_2) * a3->data.polygon.size * scale.x;
        base_translation.y = 0.0f;
        break;
    }

    if (~a2->data.flags & GLITTER_PARTICLE_EMITTER_LOCAL)
        mat4_mult_vec3(&a3->mat_rot, &base_translation, &base_translation);

    vec3_add(a1->base_translation, base_translation, a1->base_translation);
    a1->translation = a1->base_translation;
    a1->translation_prev = a1->base_translation;
    glitter_random_get_vec3(GLT_VAL, random, &a2->data.direction_random, &direction_random);
    vec3_add(direction, direction_random, direction);
    vec3_normalize(direction, direction);

    if (~a2->data.flags & GLITTER_PARTICLE_EMITTER_LOCAL)
        mat4_mult_vec3(&a3->mat_rot, &direction, &direction);
    a1->base_direction = direction;
    a1->direction = direction;

    speed = glitter_random_get_float(GLT_VAL, random, a2->data.speed_random) + a2->data.speed;
    deceleration = glitter_random_get_float(GLT_VAL, random, a2->data.deceleration_random) + a2->data.deceleration;
    a1->speed = speed * 60.0f;
    a1->deceleration = max(deceleration * 60.0f, 0.0f);

    glitter_random_get_vec3(GLT_VAL, random, &a2->data.acceleration_random, &acceleration);
    vec3_add(acceleration, a2->data.acceleration, acceleration);
    vec3_add(acceleration, a2->data.gravity, a1->acceleration);
}

static void glitter_render_element_step_uv(GLT, GlitterF2ParticleInst* a1,
    glitter_render_element* a2, float_t delta_frame, glitter_random* random) {
    if (a1->data.data.frame_step_uv <= 0.0f)
        return;

    int32_t max_uv = a1->data.data.split_u * a1->data.data.split_v;
    if (max_uv)
        max_uv--;

    while (a2->frame_step_uv <= 0.0f) {
        switch (a1->data.data.uv_index_type) {
        case GLITTER_UV_INDEX_RANDOM:
            a2->uv_index = a1->data.data.uv_index_start;
            if (a1->data.data.uv_index_count > 1)
                a2->uv_index += glitter_random_get_int(GLT_VAL, random, a1->data.data.uv_index_count);
            break;
        case GLITTER_UV_INDEX_FORWARD:
        case GLITTER_UV_INDEX_INITIAL_RANDOM_FORWARD:
            a2->uv_index = (uint8_t)(max_uv & (a2->uv_index + 1));
            break;
        case GLITTER_UV_INDEX_REVERSE:
        case GLITTER_UV_INDEX_INITIAL_RANDOM_REVERSE:
            a2->uv_index = (uint8_t)(max_uv & (a2->uv_index - 1));
            break;
        }

        a2->uv.x = (float_t)(a2->uv_index % a1->data.data.split_u) * a1->data.data.split_uv.x;
        a2->uv.y = (float_t)(a2->uv_index / a1->data.data.split_u) * a1->data.data.split_uv.y;
        a2->frame_step_uv += a1->data.data.frame_step_uv;
    }
    a2->frame_step_uv -= delta_frame;
}
