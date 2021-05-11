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

static void FASTCALL glitter_render_element_accelerate(GPM, glitter_particle_inst* a1,
    glitter_render_element* a2, float_t delta_frame, glitter_random* random);
static void FASTCALL glitter_render_element_step_uv(GPM, glitter_particle_inst* a1,
    glitter_render_element* a2, float_t delta_frame, glitter_random* random);

void FASTCALL glitter_render_element_emit(GPM,
    glitter_particle_inst* a1, glitter_render_element* a2, glitter_emitter_inst* a3,
    glitter_particle_inst_data* a4, int32_t index, uint8_t step, glitter_random* random) {
    float_t speed;
    float_t deceleration;
    vec3 direction;
    vec3 base_translation;
    vec3 scale;
    vec3 external_acceleration;
    vec3 direction_random;
    int32_t max_uv;
    bool copy_mat;

    a2->random.value = glitter_random_get_value(random);
    a2->random.step = step;
    a2->frame = 0.0f;
    a2->rebound_frame = 0.0f;
    a2->uv = vec2_null;
    a2->uv_index = a4->data.uv_index;

    a2->fade_in_frames = 0.0f;
    a2->fade_out_frames = 0.0f;
    a2->life_time = (float_t)a4->data.life_time;

    a2->color = a4->data.color;
    if (a4->data.draw_type == GLITTER_DIRECTION_PARTICLE_ROTATION) {
        a2->rotation.x = glitter_random_get_float(GPM_VAL, random, a4->data.rotation_random.x)
            + a4->data.rotation.x;
        a2->rotation_add.x = glitter_random_get_float(GPM_VAL, random, a4->data.rotation_add_random.x)
            + a4->data.rotation_add.x;
        a2->rotation.y = glitter_random_get_float(GPM_VAL, random, a4->data.rotation_random.y)
            + a4->data.rotation.y;
        a2->rotation_add.y = glitter_random_get_float(GPM_VAL, random, a4->data.rotation_add_random.y)
            + a4->data.rotation_add.y;
    }
    else {
        a2->rotation.x = 0.0f;
        a2->rotation.y = 0.0f;
        a2->rotation_add.x = 0.0f;
        a2->rotation_add.y = 0.0f;
    }
    a2->rotation.z = glitter_random_get_float(GPM_VAL, random, a4->data.rotation_random.z)
        + a4->data.rotation.z;
    a2->rotation_add.z = glitter_random_get_float(GPM_VAL, random, a4->data.rotation_add_random.z)
        + a4->data.rotation_add.z;
    a2->uv_scroll = vec2_null;
    a2->uv_scroll_2nd = vec2_null;
    a2->scale = vec3_identity;
    a2->scale_all = 1.0f;
    a2->frame_step_uv = (float_t)a4->data.frame_step_uv;

    a2->scale_particle.x = glitter_random_get_float(GPM_VAL, random, a4->data.scale_random.x)
        + a4->data.scale.x;
    if (a4->data.flags & GLITTER_PARTICLE_SCALE_Y_BY_X)
        a2->scale_particle.y = a2->scale_particle.x;
    else
        a2->scale_particle.y = glitter_random_get_float(GPM_VAL, random, a4->data.scale_random.y)
        + a4->data.scale.y;

    max_uv = a4->data.split_u * a4->data.split_v;
    if (max_uv > 1 && a4->data.uv_index_count > 1) {
        switch (a4->data.uv_index_type) {
        case GLITTER_UV_INDEX_INITIAL_RANDOM_FIXED:
        case GLITTER_UV_INDEX_INITIAL_RANDOM_FORWARD:
        case GLITTER_UV_INDEX_INITIAL_RANDOM_REVERSE:
            a2->uv_index += glitter_random_get_int(GPM_VAL, random, a4->data.uv_index_count);
            break;
        }
        a2->uv_index = min(a2->uv_index, a4->data.uv_index_end);
        a2->uv.x = (float_t)(a2->uv_index % a1->data.data.split_u) * a1->data.data.split_uv.x;
        a2->uv.y = (float_t)(a2->uv_index / a1->data.data.split_u) * a1->data.data.split_uv.y;
    }

    if (a4->data.flags & (GLITTER_PARTICLE_EMITTER_LOCAL | GLITTER_PARTICLE_ROTATE_BY_EMITTER))
        a2->base_translation = vec3_null;
    else
        mat4_get_translation(&a3->mat, &a2->base_translation);

    direction = a4->data.direction;
    base_translation = vec3_null;
    if (a4->data.flags & GLITTER_PARTICLE_EMITTER_LOCAL) {
        scale = vec3_identity;
        glitter_emitter_inst_get_mesh_by_type(GPM_VAL, a3, index, &scale, &base_translation, &direction, random);
    }
    else {
        vec3_mult_scalar(a3->scale, a3->scale_all, scale);
        glitter_emitter_inst_get_mesh_by_type(GPM_VAL, a3, index, &scale, &base_translation, &direction, random);
        mat4_mult_vec3(&a3->mat_no_scale, &base_translation, &base_translation);
    }

    vec3_add(a2->base_translation, base_translation, a2->base_translation);
    a2->translation = a2->base_translation;
    a2->translation_prev = a2->base_translation;
    glitter_random_get_vec3(GPM_VAL, random, &a4->data.direction_random, &direction_random);
    vec3_add(direction, direction_random, direction);
    vec3_normalize(direction, direction);

    if (~a4->data.flags & GLITTER_PARTICLE_EMITTER_LOCAL)
        mat4_mult_vec3(&a3->mat_no_scale, &direction, &direction);
    a2->base_direction = direction;
    a2->direction = direction;

    speed = glitter_random_get_float(GPM_VAL, random, a4->data.speed_random) + a4->data.speed;
    deceleration = glitter_random_get_float(GPM_VAL, random, a4->data.deceleration_random) + a4->data.deceleration;
    a2->speed = speed * 60.0f;
    a2->deceleration = max(deceleration * 60.0f, 0.0f);

    glitter_random_get_vec3(GPM_VAL, random, &a4->data.acceleration_random, &external_acceleration);
    vec3_add(external_acceleration, a4->data.acceleration, external_acceleration);
    vec3_add(external_acceleration, a4->data.gravity, a2->acceleration);

    copy_mat = false;
    if (a4->data.flags & GLITTER_PARTICLE_ROTATE_BY_EMITTER
        || a4->data.draw_type == GLITTER_DIRECTION_EMITTER_ROTATION) {
        switch (a3->data.type) {
        case GLITTER_EMITTER_CYLINDER:
            if (a3->data.cylinder.direction != GLITTER_EMITTER_EMISSION_DIRECTION_NONE) {
                float_t length;
                vec3_length(a2->direction, length);
                copy_mat = length <= 0.000001f;
            }
            else
                copy_mat = true;
            break;
        case GLITTER_EMITTER_SPHERE:
            if (a3->data.sphere.direction != GLITTER_EMITTER_EMISSION_DIRECTION_NONE) {
                float_t length;
                vec3_length(a2->direction, length);
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
        a2->mat = a3->mat;
        if (a4->data.flags & GLITTER_PARTICLE_EMITTER_LOCAL)
            a2->mat.row3 = (vec4){ 0.0f, 0.0f, 0.0f, 1.0f };
    }
    else
        a2->mat = mat4_identity;

    if (a1->data.data.type == GLITTER_PARTICLE_LOCUS) {
        uint32_t locus_history_size = glitter_random_get_int_min_max(GPM_VAL, random,
            -a1->data.data.locus_history_size_random, a1->data.data.locus_history_size_random)
            + a1->data.data.locus_history_size;
        a2->locus_history = glitter_locus_history_init(locus_history_size);
    }
    glitter_random_step_value(random);
}

void FASTCALL glitter_render_element_free(glitter_render_element* a1) {
    a1->alive = false;
    if (a1->locus_history) {
        glitter_locus_history_dispose(a1->locus_history);
        a1->locus_history = 0;
    }
}

void FASTCALL glitter_render_element_get_value(GPM,
    glitter_render_group* a1, glitter_render_element* a2, float_t delta_frame) {
    glitter_particle_inst* v4;
    vec2 uv_scroll;
    vec2 uv_scroll_2nd;
    vec3 translation;
    bool has_translation;
    size_t length;
    glitter_curve* curve;
    float_t value;
    float_t color_scale;
    
    glitter_random_set_step(a1->random_ptr, 1);
    v4 = a1->particle;
    if (!v4 || !((v4->data.data.flags & GLITTER_PARTICLE_LOOP
        && !glitter_particle_inst_has_ended(v4, false)) || a2->frame <= a2->life_time)) {
        a1->ctrl--;
        a2->alive = false;
        if (a2->locus_history) {
            glitter_locus_history_dispose(a2->locus_history);
            a2->locus_history = 0;
        }
        return;
    }

    glitter_render_element_accelerate(GPM_VAL, v4, a2, delta_frame, a1->random_ptr);
    if (v4->data.data.draw_type == GLITTER_DIRECTION_PARTICLE_ROTATION) {
        a2->rotation.x += a2->rotation_add.x * delta_frame;
        a2->rotation.y += a2->rotation_add.y * delta_frame;
    }
    a2->rotation.z += a2->rotation_add.z * delta_frame;
    vec2_mult_scalar(v4->data.data.uv_scroll_add,
        v4->data.data.uv_scroll_add_scale * delta_frame, uv_scroll);
    vec2_add(a2->uv_scroll, uv_scroll, a2->uv_scroll);
    vec2_mult_scalar(v4->data.data.uv_scroll_2nd_add,
        v4->data.data.uv_scroll_2nd_add_scale * delta_frame, uv_scroll_2nd);
    vec2_add(a2->uv_scroll_2nd, uv_scroll_2nd, a2->uv_scroll_2nd);
    glitter_render_element_step_uv(GPM_VAL, v4, a2, delta_frame, a1->random_ptr);
    a2->color = (vec4){ -1.0f, -1.0f, -1.0f, -1.0f };
    color_scale = -1.0f;
    a2->draw = true;

    if (v4->data.data.sub_flags & GLITTER_PARTICLE_SUB_USE_CURVE) {
        value = 0.0f;
        has_translation = false;
        translation = vec3_null;

        vector_ptr_glitter_curve curv = v4->particle->curve;
        length = curv.end - curv.begin;
        for (size_t i = 0; i < length; i++) {
            curve = curv.begin[i];
            if (!glitter_curve_get_value(GPM_VAL, curve, a2->frame, &value,
                (int32_t)(a2->random.value + i), a1->random_ptr))
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
                if (fabsf(value) < 0.000001f)
                    a2->draw = false;
                break;
            case GLITTER_CURVE_SCALE_Y:
                a2->scale.y = value;
                if (fabsf(value) < 0.000001f)
                    a2->draw = false;
                break;
            case GLITTER_CURVE_SCALE_Z:
                a2->scale.z = value;
                if (fabsf(value) < 0.000001f)
                    a2->draw = false;
                break;
            case GLITTER_CURVE_SCALE_ALL:
                a2->scale_all = value;
                if (fabsf(value) < 0.000001f)
                    a2->draw = false;
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
                    a2->draw = false;
                break;
            case GLITTER_CURVE_U_SCROLL:
                a2->uv_scroll.x = value;
                break;
            case GLITTER_CURVE_V_SCROLL:
                a2->uv_scroll.y = value;
                break;
            }
        }

        if (v4->data.data.flags & GLITTER_PARTICLE_ROTATE_BY_EMITTER || has_translation) {
            vec3_add(translation, a2->base_translation, translation);
            if (v4->data.data.flags & GLITTER_PARTICLE_ROTATE_BY_EMITTER)
                mat4_mult_vec3_trans(&a2->mat, &translation, &translation);
            a2->translation = translation;
        }
    }

    if (a2->draw) {
        if (a2->color.x < 0.0f)
            a2->color.x = v4->data.data.color.x;
        if (a2->color.y < 0.0f)
            a2->color.y = v4->data.data.color.y;
        if (a2->color.z < 0.0f)
            a2->color.z = v4->data.data.color.z;
        if (a2->color.w < 0.0f)
            a2->color.w = v4->data.data.color.w;

        if (color_scale >= 0.0f)
            vec3_mult_scalar(*(vec3*)&a2->color, color_scale, *(vec3*)&a2->color);

        if (a2->fade_in_frames > 0.0 && (a2->life_time - a2->frame) < a2->fade_in_frames)
            a2->color.w *= (a2->life_time - a2->frame) / a2->fade_in_frames;
        else if (a2->fade_out_frames > 0.0 && a2->frame < a2->fade_out_frames)
            a2->color.w *= a2->frame / a2->fade_out_frames;

        if (a2->color.w < 0.01f)
            a2->draw = false;
    }

    if (v4->data.data.type == GLITTER_PARTICLE_LOCUS)
        glitter_locus_history_append(GPM_VAL, a2->locus_history, a2, v4);

    a2->frame += delta_frame;
    if (v4->data.data.flags & GLITTER_PARTICLE_LOOP && a2->frame >= a2->life_time)
        a2->frame -= a2->life_time;
}

void FASTCALL glitter_render_element_rotate_to_position(mat3* mat,
    glitter_render_group* a2, glitter_render_element* a3) {
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
        *mat = mat3_identity;
        vec1 = (vec3){ 0.0f, 0.0f, 1.0f };
        axis_angle_from_vectors(&axis, &angle, &vec1, &vec2);
        mat3_mult_axis_angle(mat, mat, &axis, angle);
    }
    else
        mat3_rotate_z((float_t)M_PI, mat);
}

void FASTCALL glitter_render_element_rotate_to_prev_position(mat3* mat,
    glitter_render_group* a2, glitter_render_element* a3) {
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
        mat3_rotate_z((float_t)M_PI, mat);
        vec1 = (vec3){ 0.0f, 1.0f, 0.0f };
        axis_angle_from_vectors(&axis, &angle, &vec1, &vec2);
        mat3_mult_axis_angle(mat, mat, &axis, angle);
    }
    else
        mat3_rotate_z((float_t)M_PI, mat);
}

static void FASTCALL glitter_render_element_accelerate(GPM, glitter_particle_inst* a1,
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
        reflection_coeff = glitter_random_get_float(GPM_VAL, random,
            a1->data.data.reflection_coeff_random) + a1->data.data.reflection_coeff;
        a2->rebound_frame = a2->frame;
        vec3_sub(a2->translation, a2->translation_prev, direction);
        vec3_mult_scalar(direction, reflection_coeff * 60.0f, direction);
        vec3_xor(direction, ((vec3){ 0.0f, -0.0f, 0.0f }), a2->direction);
        a2->translation.y = a2->translation_prev.y;
    }
}

static void FASTCALL glitter_render_element_get_color(glitter_particle_inst* a1,
    float_t* r, float_t* g, float_t* b, float_t* a) {
    glitter_effect_inst* effect;

    effect = a1->data.effect;
    if (!effect) {
        if (!a1->data.parent)
            return;
        effect = a1->data.parent->data.effect;
        if (!effect)
            return;
    }
}

static void FASTCALL glitter_render_element_step_uv(GPM, glitter_particle_inst* a1,
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
                a2->uv_index += glitter_random_get_int(GPM_VAL, random, a1->data.data.uv_index_count);
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
