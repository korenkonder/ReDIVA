/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "emitter_inst.h"
#include "curve.h"
#include "effect_inst.h"
#include "particle_inst.h"
#include "random.h"

glitter_emitter_inst* FASTCALL glitter_emitter_inst_init(glitter_emitter* a1,
    glitter_scene* a2, glitter_effect_inst* a3) {
    glitter_particle** i;
    glitter_particle_inst* particle;

    glitter_emitter_inst* ei = force_malloc(sizeof(glitter_emitter_inst));
    ei->emitter = a1;

    ei->data = a1->data;
    ei->translation = a1->translation;
    ei->rotation = a1->rotation;
    ei->scale = *(vec3*)&a1->scale;
    mat4_identity(&ei->mat);
    mat4_identity(&ei->mat_no_scale);
    ei->scale_all = 1.0f;
    ei->emission_interval = ei->data.emission_interval;
    ei->particles_per_emission = ei->data.particles_per_emission;
    ei->frame = -ei->data.start_time;
    if (ei->data.emission_interval >= -0.000001f)
        ei->emission = fabs(ei->data.emission_interval) <= 0.000001f
        ? GLITTER_EMITTER_EMISSION_ON_START : GLITTER_EMITTER_EMISSION_ON_TIMER;
    else
        ei->emission = GLITTER_EMITTER_EMISSION_ON_END;
    ei->loop = ei->data.flags & 1 ? true : false;

    vector_ptr_glitter_particle_inst_expand(&ei->particles, a1->particles.end - a1->particles.begin);
    for (i = a1->particles.begin; i != a1->particles.end; i++) {
        if (!*i)
            continue;

        particle = glitter_particle_inst_init(*i, a2, a3);
        vector_ptr_glitter_particle_inst_append_element(&ei->particles, &particle);
    }

    ei->random = glitter_random_get();
    glitter_random_set(glitter_random_get() + 1);
    return ei;
}

void FASTCALL glitter_emitter_inst_copy(glitter_emitter_inst* a1, glitter_emitter_inst* a2, glitter_scene* a3) {
    int64_t particles_count; // rbp
    int64_t i; // rbx

    a2->translation = a1->translation;
    a2->rotation = a1->rotation;
    a2->scale = a1->scale;
    a2->mat = a1->mat;
    a2->mat_no_scale = a1->mat_no_scale;
    a2->scale_all = a1->scale_all;
    a2->emission_timer = a1->emission_timer;
    a2->emission_interval = a1->emission_interval;
    a2->particles_per_emission = a1->particles_per_emission;
    a2->random = a1->random;
    a2->loop = a1->loop;
    a2->emission = a1->emission;
    a2->frame = a1->frame;
    a2->ended = a1->ended;

    particles_count = a1->particles.end - a1->particles.begin;
    if (particles_count != a2->particles.end - a2->particles.begin)
        return;

    for (i = 0; i < particles_count; ++i)
        glitter_particle_inst_copy(a1->particles.begin[i], a2->particles.begin[i], a3);
}

void FASTCALL glitter_emitter_inst_emit(glitter_emitter_inst* a1, glitter_scene* a2) {
    int32_t count;
    glitter_particle_inst** i;

    if (a1->data.type == GLITTER_EMITTER_POLYGON)
        count = a1->data.data.polygon.count;
    else
        count = 1;

    for (i = a1->particles.begin; i != a1->particles.end; ++i)
        glitter_particle_inst_emit(*i, a2, a1, (int32_t)a1->particles_per_emission, count);
}

void FASTCALL glitter_emitter_inst_emit_init(glitter_emitter_inst* a1,
    glitter_scene* a2, glitter_effect_inst* a3, float_t delta_frame) {
    if (a1->frame < 0.0f) {
        a1->frame += delta_frame;
        return;
    }

    if (!a1->ended) {
        if (a1->emission == GLITTER_EMITTER_EMISSION_ON_TIMER) {
            if (a1->emission_timer >= 0.0f || a1->emission_interval >= 0.0f) {
                a1->emission_timer -= delta_frame;
                if (a1->emission_timer <= 0.0f) {
                    glitter_effect_inst_update_mat(a3);
                    glitter_emitter_inst_emit(a1, a2);
                    if (a1->emission_interval > 0.0f)
                        a1->emission_timer += a1->emission_interval;
                    else
                        a1->emission_timer = -1.0f;
                }
            }
        }
        else if (a1->emission == GLITTER_EMITTER_EMISSION_ON_START) {
            glitter_effect_inst_update_mat(a3);
            glitter_emitter_inst_emit(a1, a2);
            a1->emission = GLITTER_EMITTER_EMISSION_EMITTED;
        }
    }

    if (!a1->loop && a1->data.life_time <= a1->frame)
        glitter_emitter_inst_free(a1, a2, 0);

    a1->frame += delta_frame;
}

void FASTCALL glitter_emitter_inst_emit_step(glitter_emitter_inst* a1,
    glitter_scene* a2, glitter_effect_inst* a3, float_t delta_frame) {
    if (a1->frame < 0.0f) {
        a1->frame += delta_frame;
        return;
    }

    if (!a1->ended) {
        if (a1->emission == GLITTER_EMITTER_EMISSION_ON_TIMER) {
            if (a1->emission_timer >= 0.0f || a1->emission_interval >= 0.0f) {
                a1->emission_timer -= delta_frame;
                if (a1->emission_timer <= 0.0f) {
                    glitter_emitter_inst_emit(a1, a2);
                    if (a1->emission_interval > 0.0f)
                        a1->emission_timer += a1->emission_interval;
                    else
                        a1->emission_timer = -1.0f;
                }
            }
        }
        else if (a1->emission == GLITTER_EMITTER_EMISSION_ON_START) {
            a1->emission_timer -= delta_frame;
            if (a1->emission_timer <= 0.0f) {
                glitter_emitter_inst_emit(a1, a2);
                a1->emission = GLITTER_EMITTER_EMISSION_EMITTED;
            }
        }
    }

    if (!a1->loop && a1->data.life_time <= a1->frame)
        glitter_emitter_inst_free(a1, a2, 0);

    a1->frame += delta_frame;
}

void FASTCALL glitter_emitter_inst_free(glitter_emitter_inst* a1, glitter_scene* a2, bool free) {
    glitter_particle_inst** i;

    if (a1->ended) {
        if (free)
            for (i = a1->particles.begin; i != a1->particles.end; ++i)
                glitter_particle_inst_free(*i, true);
    }
    else {
        if (a1->emission == GLITTER_EMITTER_EMISSION_ON_END) {
            glitter_emitter_inst_emit(a1, a2);
            a1->emission = GLITTER_EMITTER_EMISSION_EMITTED;
        }

        if (a1->loop && a1->data.loop_life_time >= 0.0f)
            a1->loop = false;

        a1->ended = true;
        for (i = a1->particles.begin; i != a1->particles.end; ++i)
            glitter_particle_inst_free(*i, false);
    }
}

void FASTCALL glitter_emitter_inst_get_mesh_by_type(glitter_emitter_inst* a1,
    int32_t index, vec3* scale, vec3* base_translation, vec3* direction) {
    float_t radius; // xmm8_4
    float_t angle; // xmm6_4
    float_t v15; // xmm7_4
    float_t v16; // xmm0_4
    float_t v17; // xmm2_4
    float_t v18; // xmm7_4
    float_t v19; // xmm6_4
    float_t longitude; // xmm8_4
    float_t latitude; // xmm6_4
    float_t v26; // xmm7_4
    vec3 v27;

    switch (a1->data.type) {
    case GLITTER_EMITTER_BOX:
        base_translation->x = glitter_random_get_float_clamp_min_max(a1->data.data.box.size.x * scale->x * 0.5f);
        base_translation->y = glitter_random_get_float_clamp_min_max(a1->data.data.box.size.y * scale->y * 0.5f);
        base_translation->z = glitter_random_get_float_clamp_min_max(a1->data.data.box.size.z * scale->z * 0.5f);
        break;
    case GLITTER_EMITTER_CYLINDER:
        radius = a1->data.data.cylinder.radius * scale->x;
        if (!a1->data.data.cylinder.plain)
            radius = glitter_random_get_float_clamp(0.0f, radius);
        angle = glitter_random_get_float_clamp(a1->data.data.cylinder.start_angle, a1->data.data.cylinder.end_angle);
        v15 = cosf(angle);
        v16 = sinf(angle);
        v17 = 1.0f / sqrtf(v16 * v16 + v15 * v15);
        v18 = v15 * v17;
        v19 = v16 * v17;
        base_translation->x = v18 * radius;
        base_translation->y = glitter_random_get_float_clamp_min_max(a1->data.data.cylinder.height * scale->y * 0.5f);
        base_translation->z = v19 * radius;
        if (a1->data.data.cylinder.direction == GLITTER_EMITTER_EMISSION_DIRECTION_INWARD) {
            direction->x = v18;
            direction->y = 0.0f;
            direction->z = v19;
        }
        else if(a1->data.data.cylinder.direction == GLITTER_EMITTER_EMISSION_DIRECTION_OUTWARD) {
            direction->x = -v18;
            direction->y = 0.0f;
            direction->z = -v19;
        }
        break;
    case GLITTER_EMITTER_SPHERE:
        radius = a1->data.data.sphere.radius * scale->x;
        if (!a1->data.data.sphere.plain)
            radius = glitter_random_get_float_clamp(0.0f, radius);
        longitude = glitter_random_get_float_clamp(a1->data.data.sphere.longitude * -0.5f,
            a1->data.data.sphere.longitude * 0.5f);
        latitude = (float_t)M_PI_2 - glitter_random_get_float_clamp(0.0f, a1->data.data.sphere.latitude);
        v26 = cosf(latitude);
        v27.x = sinf(longitude) * v26;
        v27.y = sinf(latitude);
        v27.z = cosf(longitude) * v26;
        vec3_mult_scalar(v27, radius, *base_translation);
        if (a1->data.data.sphere.direction == GLITTER_EMITTER_EMISSION_DIRECTION_INWARD)
            *direction = v27;
        else if (a1->data.data.sphere.direction == GLITTER_EMITTER_EMISSION_DIRECTION_OUTWARD)
            vec3_negate(v27, *direction);
        break;
    case GLITTER_EMITTER_MESH:
        break;
    case GLITTER_EMITTER_POLYGON:
        base_translation->x = sinf(((float_t)index / (float_t)a1->data.data.polygon.count * 2.0f)
            / (float_t)M_PI + (float_t)M_PI_2) * a1->data.data.polygon.scale * scale->x;
        base_translation->y = 0.0f;
        break;
    }
}

void FASTCALL glitter_emitter_inst_get_value(glitter_emitter_inst* a1, float_t frame, int32_t random) {
    int64_t length;
    glitter_curve* curve;
    float_t value;

    length = a1->emitter->curve.end - a1->emitter->curve.begin;
    if (!length)
        return;

    for (int32_t i = 0; i < length; i++) {
        curve = a1->emitter->curve.begin[i];
        if (!glitter_curve_get_value(curve, frame, &value, random + i))
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

void FASTCALL glitter_emitter_inst_render_group_init(glitter_emitter_inst* emitter, float_t delta_frame) {
    glitter_particle_inst** i;

    for (i = emitter->particles.begin; i != emitter->particles.end; ++i)
        glitter_particle_inst_render_group_init(*i, delta_frame);
}

void FASTCALL glitter_emitter_inst_reset(glitter_emitter_inst* a1) {
    glitter_particle_inst** i; // rbx

    a1->loop = a1->data.flags & 1 ? true : false;
    a1->frame = 0.0f;
    a1->ended = false;
    if (a1->emission_interval >= -0.000001f)
        a1->emission = fabsf(a1->emission_interval) <= 0.000001f
        ? GLITTER_EMITTER_EMISSION_ON_START : GLITTER_EMITTER_EMISSION_ON_TIMER;
    else
        a1->emission = GLITTER_EMITTER_EMISSION_ON_END;

    for (i = a1->particles.begin; i != a1->particles.end; ++i)
        glitter_particle_inst_reset(*i);
}

void FASTCALL glitter_emitter_inst_update_mat(glitter_emitter_inst* a1, glitter_effect_inst* a2) {
    vec3 scale;
    bool v12; // bl
    int64_t v14; // rax
    int64_t v15; // r14
    glitter_particle_inst* v17; // rbx
    mat4 mat1;
    mat4 mat2;

    vec3_mult_scalar(a1->scale, a1->scale_all, scale);
    mat1 = a2->mat;

    v12 = true;
    switch (a1->data.direction) {
    case GLITTER_DIRECTION_BILLBOARD:
        mat2 = cam->view;
        mat2.row3 = (vec4){ 0.0f, 0.0f, 0.0f, 1.0f };
        mat4_inverse(&mat2, &mat2);
        mat4_mult(&mat1, &mat2, &mat2);
        mat2.row3 = (vec4){ 0.0f, 0.0f, 0.0f, 1.0f };
        break;
    case GLITTER_DIRECTION_Y_AXIS:
        mat4_rotate_y((float_t)-M_PI_2, &mat2);
        break;
    case GLITTER_DIRECTION_X_AXIS:
        mat4_rotate_x((float_t)-M_PI_2, &mat2);
        break;
    case GLITTER_DIRECTION_Z_AXIS:
        mat4_rotate_z((float_t)-M_PI_2, &mat2);
        break;
    case GLITTER_DIRECTION_BILLBOARD_Y_ONLY:
        mat4_rotate_y(cam->rotation.y, &mat2);
        break;
    default:
        v12 = false;
        break;
    }

    mat4_translate_mult(&mat1, a1->translation.x, a1->translation.y, a1->translation.z, &mat1);
    mat4_normalize_rotation(&mat1, &mat1);
    if (v12)
        mat4_mult(&mat2, &mat1, &mat1);
    mat4_rot(&mat1, a1->rotation.x, a1->rotation.y, a1->rotation.z, &mat1);
    a1->mat_no_scale = mat1;
    mat4_scale_rot(&mat1, scale.x, scale.y, scale.z, &a1->mat);

    v14 = a1->particles.end - a1->particles.begin;
    for (v15 = 0; v14 > 0; v15++, v14--) {
        if (!a1->particles.begin[v15])
            continue;

        v17 = a1->particles.begin[v15];
        v17->mat = a1->mat;
        v17->mat_no_scale = a1->mat_no_scale;
        v17->mat_no_scale.row3 = (vec4){ 0.0f, 0.0f, 0.0f, 1.0f };
        glitter_particle_inst_set_mat(v17);
    }
}

void FASTCALL glitter_emitter_inst_update_value_frame(glitter_emitter_inst* a1,
    glitter_effect_inst* a2, float_t delta_frame) {
    int64_t v7; // r14
    vec3 rotation_add;
    vec3 scale;
    bool v22; // si
    int64_t v24; // rbp
    glitter_particle_inst* v26; // rsi
    mat4 mat1;
    mat4 mat2;

    if (a1->frame < 0.0f)
        return;

    if (a1->loop) {
        if (a1->data.loop_life_time < 0.0f || a1->frame < a1->data.loop_life_time) {
            if (a1->frame >= a1->data.life_time) {
                a1->frame -= a1->data.life_time;
                if (a1->emission == GLITTER_EMITTER_EMISSION_EMITTED && a1->emission_interval > 0.0f)
                    a1->emission = GLITTER_EMITTER_EMISSION_ON_TIMER;
            }
        }
        else
            while (a1->frame > a1->data.loop_life_time)
                a1->frame -= a1->data.loop_life_time - a1->data.loop_start_time;
    }

    glitter_emitter_inst_get_value(a1, a1->frame, a1->random);
    vec3_mult_scalar(a1->data.rotation_add, delta_frame, rotation_add);
    vec3_add(a1->rotation, rotation_add, a1->rotation);

    vec3_mult_scalar(a1->scale, a1->scale_all, scale);
    mat1 = a2->mat;

    v22 = true;
    switch (a1->data.direction) {
    case GLITTER_DIRECTION_BILLBOARD:
        mat2 = cam->view;
        mat2.row3 = (vec4){ 0.0f, 0.0f, 0.0f, 1.0f };
        mat4_inverse(&mat2, &mat2);
        mat4_mult(&mat1, &mat2, &mat2);
        mat2.row3 = (vec4){ 0.0f, 0.0f, 0.0f, 1.0f };
        break;
    case GLITTER_DIRECTION_Y_AXIS:
        mat4_rotate_y((float_t)-M_PI_2, &mat2);
        break;
    case GLITTER_DIRECTION_X_AXIS:
        mat4_rotate_x((float_t)-M_PI_2, &mat2);
        break;
    case GLITTER_DIRECTION_Z_AXIS:
        mat4_rotate_z((float_t)-M_PI_2, &mat2);
        break;
    case GLITTER_DIRECTION_BILLBOARD_Y_ONLY:
        mat4_rotate_y(cam->rotation.y, &mat2);
        break;
    default:
        v22 = false;
        break;
    }

    mat4_translate_mult(&mat1, a1->translation.x, a1->translation.y, a1->translation.z, &mat1);
    mat4_normalize_rotation(&mat1, &mat1);
    if (v22)
        mat4_mult(&mat2, &mat1, &mat1);
    mat4_rot(&mat1, a1->rotation.x, a1->rotation.y, a1->rotation.z, &mat1);
    a1->mat_no_scale = mat1;
    mat4_scale_rot(&mat1, scale.x, scale.y, scale.z, &a1->mat);

    v24 = a1->particles.end - a1->particles.begin;
    for (v7 = 0; v24 > 0; v7++, v24--) {
        if (!a1->particles.begin[v7])
            continue;

        v26 = a1->particles.begin[v7];
        v26->mat = a1->mat;
        v26->mat_no_scale = a1->mat_no_scale;
        v26->mat_no_scale.row3 = (vec4){ 0.0f, 0.0f, 0.0f, 1.0f };
        glitter_particle_inst_set_mat(v26);
    }
}

void FASTCALL glitter_emitter_inst_update_value_init(glitter_emitter_inst* a1, float_t frame, float_t delta_frame) {
    float_t v3;
    float_t v6;
    vec3 vec;

    if (a1->frame < 0.0f)
        return;

    v3 = a1->frame;
    if (a1->loop) {
        v6 = a1->data.loop_life_time;
        if (v6 < 0.0f || v3 < v6) {
            if (v3 >= a1->data.life_time)
                a1->frame = v3 - a1->data.life_time;
        }
        else if (v3 > v6) {
            do
                v3 = v3 - (v6 - a1->data.loop_start_time);
            while (v3 > v6);
            a1->frame = v3;
        }
    }

    if (a1->frame < 0.0f)
        return;

    glitter_emitter_inst_get_value(a1, a1->frame, a1->random);
    vec3_mult_scalar(a1->data.rotation_add, delta_frame, vec);
    vec3_add(a1->rotation, vec, a1->rotation);
}

void FASTCALL glitter_emitter_inst_dispose(glitter_emitter_inst* ei) {
    vector_ptr_glitter_particle_inst_clear(&ei->particles, (void*)&glitter_particle_inst_dispose);
    vector_ptr_glitter_particle_inst_dispose(&ei->particles);
    free(ei);
}
