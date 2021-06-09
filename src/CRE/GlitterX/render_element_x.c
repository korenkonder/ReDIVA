/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "render_element_x.h"
#include "counter_x.h"
#include "curve_x.h"
#include "emitter_inst_x.h"
#include "locus_history_x.h"
#include "particle_inst_x.h"
#include "random_x.h"

static void FASTCALL glitter_x_render_element_accelerate(glitter_particle_inst* a1,
    glitter_render_element* a2, float_t delta_frame, glitter_random* random);
static void FASTCALL glitter_render_element_init_mesh_by_type(glitter_render_element* a1,
    glitter_particle_inst_data* a2, glitter_emitter_inst* a3, int32_t index, glitter_random* random);
static void FASTCALL glitter_x_render_element_step_uv(glitter_particle_inst* a1,
    glitter_render_element* a2, float_t delta_frame, glitter_random* random);

void FASTCALL glitter_x_render_element_emit(glitter_render_element* a1,
    glitter_particle_inst_data* a2, glitter_emitter_inst* a3,
    int32_t index, uint8_t step, glitter_random* random) {
    a1->random = glitter_x_random_get_value(random);
    a1->frame = 0.0f;
    a1->rebound_frame = 0.0f;
    a1->uv = vec2_null;
    a1->uv_index = a2->data.uv_index;

    a1->step = step;
    a1->fade_in_frames = glitter_x_random_get_float(random, (float_t)a2->data.fade_in_random)
        + (float_t)a2->data.fade_in;
    if (a1->fade_in_frames < 0.0f)
        a1->fade_in_frames = 0.0f;

    a1->fade_out_frames = glitter_x_random_get_float(random, (float_t)a2->data.fade_out_random)
        + (float_t)a2->data.fade_out;
    if (a1->fade_out_frames < 0.0f)
        a1->fade_out_frames = 0.0f;

    a1->life_time = glitter_x_random_get_float(random, (float_t)a2->data.life_time_random)
        + (float_t)a2->data.life_time;
    if (a1->life_time < 0.0f)
        a1->life_time = 0.0f;

    a1->life_time += a1->fade_in_frames + a1->fade_out_frames;

    a1->color = a2->data.color;
    if (a2->data.draw_type == GLITTER_DIRECTION_PARTICLE_ROTATION) {
        a1->rotation.x = glitter_x_random_get_float(random, a2->data.rotation_random.x)
            + a2->data.rotation.x;
        a1->rotation_add.x = glitter_x_random_get_float(random, a2->data.rotation_add_random.x)
            + a2->data.rotation_add.x;
        a1->rotation.y = glitter_x_random_get_float(random, a2->data.rotation_random.y)
            + a2->data.rotation.y;
        a1->rotation_add.y = glitter_x_random_get_float(random, a2->data.rotation_add_random.y)
            + a2->data.rotation_add.y;
    }
    else {
        a1->rotation.x = 0.0f;
        a1->rotation.y = 0.0f;
        a1->rotation_add.x = 0.0f;
        a1->rotation_add.y = 0.0f;
    }
    a1->rotation.z = glitter_x_random_get_float(random, a2->data.rotation_random.z)
        + a2->data.rotation.z;
    a1->rotation_add.z = glitter_x_random_get_float(random, a2->data.rotation_add_random.z)
        + a2->data.rotation_add.z;
    a1->uv_scroll = vec2_null;
    a1->uv_scroll_2nd = vec2_null;
    a1->scale = vec3_identity;
    a1->scale_all = 1.0f;
    a1->frame_step_uv = (float_t)a2->data.frame_step_uv;

    a1->scale_particle.x = glitter_x_random_get_float(random, a2->data.scale_random.x)
        + a2->data.scale.x;
    if (a2->data.flags & GLITTER_PARTICLE_SCALE_Y_BY_X) {
        a1->scale_particle.y = a1->scale_particle.x;
        glitter_x_random_step_value(random);
    }
    else
        a1->scale_particle.y = glitter_x_random_get_float(random, a2->data.scale_random.y)
        + a2->data.scale.y;

    int32_t max_uv = a2->data.split_u * a2->data.split_v;
    if (max_uv > 1 && a2->data.uv_index_count > 1) {
        switch (a2->data.uv_index_type) {
        case GLITTER_UV_INDEX_INITIAL_RANDOM_FIXED:
        case GLITTER_UV_INDEX_INITIAL_RANDOM_FORWARD:
        case GLITTER_UV_INDEX_INITIAL_RANDOM_REVERSE:
            a1->uv_index += glitter_x_random_get_int(random, a2->data.uv_index_count);
            break;
        }
        a1->uv_index = min(a1->uv_index, a2->data.uv_index_end);
        a1->uv.x = (float_t)(a1->uv_index % a2->data.split_u) * a2->data.split_uv.x;
        a1->uv.y = (float_t)(a1->uv_index / a2->data.split_u) * a2->data.split_uv.y;
    }

    glitter_render_element_init_mesh_by_type(a1, a2, a3, index, random);

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
        uint32_t locus_history_size = glitter_x_random_get_int_min_max(random,
            -a2->data.locus_history_size_random, a2->data.locus_history_size_random)
            + a2->data.locus_history_size;
        a1->locus_history = glitter_x_locus_history_init(locus_history_size);
    }
    glitter_x_random_step_value(random);
}

void FASTCALL glitter_x_render_element_free(glitter_render_element* a1) {
    a1->alive = false;
    if (a1->locus_history) {
        glitter_x_locus_history_dispose(a1->locus_history);
        a1->locus_history = 0;
    }
}

void FASTCALL glitter_x_render_element_rotate_mesh_to_emit_position(mat4* mat,
    glitter_render_group* a2, glitter_render_element* a3, vec3* vec, vec3* trans) {
    vec3 vec1;
    vec3 vec2;
    float_t angle;
    vec3 axis;
    float_t length;

    mat4_get_translation(&a2->mat, &vec2);
    vec3_sub(*trans, vec2, vec2);
    vec3_length_squared(vec2, length);
    if (length < 0.000001f)
        vec2 = (vec3){ 0.0f, 1.0f, 0.0f };
    vec3_normalize(vec2, vec2);

    vec1 = *vec;
    axis_angle_from_vectors(&axis, &angle, &vec1, &vec2);
    mat4_mult_axis_angle((mat4*)&mat4_identity, mat, &axis, angle);
}

void FASTCALL glitter_x_render_element_rotate_mesh_to_prev_position(mat4* mat,
    glitter_render_group* a2, glitter_render_element* a3, vec3* vec, vec3* trans) {
    vec3 vec1;
    vec3 vec2;
    float_t angle;
    vec3 axis;
    float_t length;

    vec3_sub(a3->translation, a3->translation_prev, vec2);
    vec3_length_squared(vec2, length);
    if (length < 0.000001f)
        vec2 = a3->base_direction;
    vec3_normalize(vec2, vec2);

    vec1 = *vec;
    axis_angle_from_vectors(&axis, &angle, &vec1, &vec2);
    mat4_mult_axis_angle((mat4*)&mat4_identity, mat, &axis, angle);
}

void FASTCALL glitter_x_render_element_rotate_to_emit_position(mat3* mat,
    glitter_render_group* a2, glitter_render_element* a3, vec3* vec) {
    vec3 trans;
    vec3 vec1;
    vec3 vec2;
    float_t angle;
    vec3 axis;
    float_t length;

    trans = a3->translation;
    mat4_get_translation(&a2->mat, &vec2);
    vec3_sub(trans, vec2, vec2);
    vec3_length_squared(vec2, length);
    if (length < 0.000001f)
        vec2 = (vec3){ 0.0f, 1.0f, 0.0f };
    vec3_normalize(vec2, vec2);

    vec1 = *vec;
    axis_angle_from_vectors(&axis, &angle, &vec1, &vec2);
    mat3_mult_axis_angle((mat3*)&mat3_identity, mat, &axis, angle);
}

void FASTCALL glitter_x_render_element_rotate_to_prev_position(mat3* mat,
    glitter_render_group* a2, glitter_render_element* a3, vec3* vec) {
    vec3 vec1;
    vec3 vec2;
    float_t angle;
    vec3 axis;
    float_t length;

    vec3_sub(a3->translation, a3->translation_prev, vec2);
    vec3_length_squared(vec2, length);
    if (length < 0.000001f)
        vec2 = a3->base_direction;
    vec3_normalize(vec2, vec2);

    vec1 = *vec;
    axis_angle_from_vectors(&axis, &angle, &vec1, &vec2);
    mat3_mult_axis_angle((mat3*)&mat3_identity, mat, &axis, angle);
}

void FASTCALL glitter_x_render_element_update(glitter_render_group* a1,
    glitter_render_element* a2, float_t delta_frame) {

    glitter_particle_inst* particle;
    vec3 translation;
    bool has_translation;
    size_t length;
    glitter_curve* curve;
    float_t value;
    float_t color_scale;

    glitter_x_random_set_step(a1->random_ptr, a2->step);
    particle = a1->particle;
    if (!particle || (particle->data.data.flags & GLITTER_PARTICLE_LOOP
        && glitter_x_particle_inst_has_ended(particle, false)) || a2->frame >= a2->life_time) {
        a1->ctrl--;
        a2->alive = false;
        if (a2->locus_history) {
            glitter_x_locus_history_dispose(a2->locus_history);
            a2->locus_history = 0;
        }
        return;
    }

    glitter_x_render_element_accelerate(particle, a2, delta_frame, a1->random_ptr);

    if (particle->data.data.draw_type == GLITTER_DIRECTION_PARTICLE_ROTATION
        || particle->data.data.type == GLITTER_PARTICLE_MESH) {
        a2->rotation.x += a2->rotation_add.x * delta_frame;
        a2->rotation.y += a2->rotation_add.y * delta_frame;
    }
    a2->rotation.z += a2->rotation_add.z * delta_frame;

    vec2 uv_scroll;
    vec2_mult_scalar(particle->data.data.uv_scroll_add,
        particle->data.data.uv_scroll_add_scale * delta_frame, uv_scroll);
    if (uv_scroll.x != 0.0f)
        a2->uv_scroll.x = fmodf(a2->uv_scroll.x + uv_scroll.x, 1.0f);
    if (uv_scroll.y != 0.0f)
        a2->uv_scroll.y = fmodf(a2->uv_scroll.y + uv_scroll.y, 1.0f);

    if (particle->data.data.sub_flags & GLITTER_PARTICLE_SUB_UV_2ND_ADD) {
        vec2 uv_scroll_2nd;
        vec2_mult_scalar(particle->data.data.uv_scroll_2nd_add,
            particle->data.data.uv_scroll_2nd_add_scale * delta_frame, uv_scroll_2nd);
        if (uv_scroll_2nd.x != 0.0f)
            a2->uv_scroll_2nd.x = fmodf(a2->uv_scroll_2nd.x + uv_scroll_2nd.x, 1.0f);
        if (uv_scroll_2nd.y != 0.0f)
            a2->uv_scroll_2nd.y = fmodf(a2->uv_scroll_2nd.y + uv_scroll_2nd.y, 1.0f);
    }
    glitter_x_render_element_step_uv(particle, a2, delta_frame, a1->random_ptr);
    a2->color = (vec4){ -1.0f, -1.0f, -1.0f, -1.0f };
    color_scale = -1.0f;
    a2->draw = true;

    if (particle->data.data.sub_flags & GLITTER_PARTICLE_SUB_USE_CURVE) {
        value = 0.0f;
        has_translation = false;
        translation = vec3_null;

        glitter_animation curv = particle->particle->animation;
        length = curv.end - curv.begin;
        for (int32_t i = 0; i < length; i++) {
            curve = curv.begin[i];
            if (!glitter_x_curve_get_value(curve, a2->frame,
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
                    a2->draw = false;
                break;
            case GLITTER_CURVE_COLOR_RGB_SCALE:
                color_scale = value;
                break;
            case GLITTER_CURVE_U_SCROLL:
                a2->uv_scroll.x = value;
                break;
            case GLITTER_CURVE_V_SCROLL:
                a2->uv_scroll.y = value;
                break;
            case GLITTER_CURVE_U_SCROLL_2ND:
                a2->uv_scroll_2nd.x = value;
                break;
            case GLITTER_CURVE_V_SCROLL_2ND:
                a2->uv_scroll_2nd.y = value;
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

    if (a2->draw) {
        if (a2->color.x < 0.0f)
            a2->color.x = particle->data.data.color.x;
        if (a2->color.y < 0.0f)
            a2->color.y = particle->data.data.color.y;
        if (a2->color.z < 0.0f)
            a2->color.z = particle->data.data.color.z;
        if (a2->color.w < 0.0f)
            a2->color.w = particle->data.data.color.w;

        if (color_scale >= 0.0f)
            vec3_mult_scalar(*(vec3*)&a2->color, color_scale, *(vec3*)&a2->color);

        if (a2->fade_out_frames > 0.0 && (a2->life_time - a2->frame) < a2->fade_out_frames)
            a2->color.w *= (a2->life_time - a2->frame) / a2->fade_out_frames;
        else if (a2->fade_in_frames > 0.0 && a2->frame < a2->fade_in_frames)
            a2->color.w *= a2->frame / a2->fade_in_frames;

        if (a2->color.w < 0.01f)
            a2->draw = false;
    }

    if (particle->data.data.type == GLITTER_PARTICLE_LOCUS)
        glitter_x_locus_history_append(a2->locus_history, a2, particle);

    a2->frame += delta_frame;
    if (particle->data.data.flags & GLITTER_PARTICLE_LOOP && a2->frame >= a2->life_time)
        a2->frame -= a2->life_time;
}

static void FASTCALL glitter_x_render_element_accelerate(glitter_particle_inst* a1,
    glitter_render_element* a2, float_t delta_frame, glitter_random* random) {
    vec3 acceleration;
    vec3 direction;
    float_t time;
    float_t diff_time;
    float_t reflection_coeff;
    float_t speed;

    a2->translation_prev = a2->translation;
    time = a2->frame * (float_t)(1.0 / 60.0);
    diff_time = time - a2->rebound_time - (float_t)(1.0 / 60.0);
    speed = a2->speed * delta_frame;
    diff_time = max(diff_time, 0.0f) * (float_t)(1.0 / 60.0);
    vec3_mult_scalar(a2->acceleration, (diff_time + (float_t)(1.0 / 7200.0)) * delta_frame, acceleration);
    if (speed > 0.0f) {
        vec3_mult_scalar(a2->direction, speed, direction);
        vec3_add(acceleration, direction, acceleration);
    }
    vec3_add(a2->translation, acceleration, a2->translation);
    vec3_add(a2->base_translation, acceleration, a2->base_translation);
    if (a1->data.data.flags & GLITTER_PARTICLE_REBOUND_PLANE
        && a2->translation_prev.y > a1->data.data.rebound_plane_y
        && a2->translation.y <= a1->data.data.rebound_plane_y) {
        reflection_coeff = glitter_x_random_get_float(random,
            a1->data.data.reflection_coeff_random) + a1->data.data.reflection_coeff;
        a2->rebound_time = time;
        vec3_sub(a2->translation, a2->translation_prev, direction);
        if (delta_frame > 0.0f)
            vec3_div_scalar(direction, delta_frame, direction);
        vec3_mult_scalar(direction, reflection_coeff / a2->speed, direction);
        vec3_xor(direction, ((vec3){ 0.0f, -0.0f, 0.0f }), a2->direction);
        a2->translation.y = a2->translation_prev.y;
    }
    speed = a2->base_speed + (float_t)(1.0 / 7200.0) * a2->deceleration - diff_time * a2->deceleration;
    a2->speed = max(speed, 0.0f);
}

static void FASTCALL glitter_render_element_init_mesh_by_type(glitter_render_element* a1,
    glitter_particle_inst_data* a2, glitter_emitter_inst* a3, int32_t index, glitter_random* random) {
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

    switch (a3->emitter->data.type) {
    case GLITTER_EMITTER_BOX:
        vec3_mult(a3->data.box.size, scale, dir);
        vec3_mult_scalar(dir, 0.5f, dir);
        glitter_x_random_get_vec3(random, &dir, &base_translation);
        break;
    case GLITTER_EMITTER_CYLINDER:
        radius = a3->data.cylinder.radius * scale.x;
        if (!a3->data.cylinder.on_edge)
            radius = glitter_x_random_get_float_min_max(random, 0.0f, radius);
        angle = glitter_x_random_get_float_min_max(random,
            a3->data.cylinder.start_angle, a3->data.cylinder.end_angle);
        dir.x = cosf(angle);
        dir.y = 0.0f;
        dir.z = sinf(angle);
        base_translation.x = dir.x * radius;
        base_translation.y = glitter_x_random_get_float(random,
            a3->data.cylinder.height * scale.y * 0.5f);
        base_translation.z = dir.z * radius;
        if (a3->data.cylinder.direction == GLITTER_EMITTER_EMISSION_DIRECTION_OUTWARD)
            direction = dir;
        else if (a3->data.cylinder.direction == GLITTER_EMITTER_EMISSION_DIRECTION_INWARD)
            vec3_xor(dir, ((vec3){ -0.0f, 0.0f, -0.0f }), direction);
        break;
    case GLITTER_EMITTER_SPHERE:
        radius = a3->data.sphere.radius * scale.x;
        if (!a3->data.sphere.on_edge)
            radius = glitter_x_random_get_float_min_max(random, 0.0f, radius);
        longitude = glitter_x_random_get_float(random, a3->data.sphere.longitude * 0.5f);
        latitude = (float_t)M_PI_2 - glitter_x_random_get_float_min_max(random,
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
        radius = a3->data.polygon.size * scale.x;
        angle = (float_t)index / (float_t)a3->data.polygon.count * (float_t)(2.0 * M_PI) + (float_t)M_PI_2;

        dir.x = cosf(angle);
        dir.y = 0.0f;
        dir.z = sinf(angle);
        vec3_mult_scalar(dir, radius, base_translation);
        if (a3->data.sphere.direction == GLITTER_EMITTER_EMISSION_DIRECTION_OUTWARD)
            direction = dir;
        else if (a3->data.sphere.direction == GLITTER_EMITTER_EMISSION_DIRECTION_INWARD)
            vec3_negate(dir, direction);
        break;
    }

    if (~a2->data.flags & GLITTER_PARTICLE_EMITTER_LOCAL)
        mat4_mult_vec3(&a3->mat_rot, &base_translation, &base_translation);

    vec3_add(a1->base_translation, base_translation, a1->base_translation);
    a1->translation = a1->base_translation;
    a1->translation_prev = a1->base_translation;
    glitter_x_random_get_vec3(random, &a2->data.direction_random, &direction_random);
    vec3_add(direction, direction_random, direction);
    vec3_normalize(direction, direction);

    if (~a2->data.flags & GLITTER_PARTICLE_EMITTER_LOCAL)
        mat4_mult_vec3(&a3->mat_rot, &direction, &direction);
    a1->base_direction = direction;
    a1->direction = direction;

    speed = glitter_x_random_get_float(random, a2->data.speed_random) + a2->data.speed;
    deceleration = glitter_x_random_get_float(random, a2->data.deceleration_random) + a2->data.deceleration;
    a1->base_speed = speed;
    a1->speed = speed;
    a1->deceleration = max(deceleration, 0.0f);

    glitter_x_random_get_vec3(random, &a2->data.acceleration_random, &acceleration);
    vec3_add(acceleration, a2->data.acceleration, acceleration);
    vec3_add(acceleration, a2->data.gravity, a1->acceleration);
}

static void FASTCALL glitter_x_render_element_step_uv(glitter_particle_inst* a1,
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
                a2->uv_index += glitter_x_random_get_int(random, a1->data.data.uv_index_count);
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
