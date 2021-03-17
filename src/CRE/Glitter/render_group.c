/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "render_group.h"
#include "counter.h"
#include "curve.h"
#include "effect_val.h"
#include "emitter_inst.h"
#include "locus_history.h"
#include "particle_inst.h"
#include "..\shader.h"

extern shader_fbo particle_shader;

glitter_render_group* FASTCALL glitter_render_group_init(int32_t count, glitter_particle_inst* a2) {
    int64_t size;

    glitter_render_group* rg = force_malloc(sizeof(glitter_render_group));
    rg->split_u = 1;
    rg->split_v = 1;
    rg->split_uv = (vec2){ 1.0f, 1.0f };
    rg->count = count;
    mat4_identity(&rg->mat);
    mat4_identity(&rg->mat_no_scale);
    rg->max_count = 6 * count;
    rg->particle_inst = a2;
    rg->alpha = 1;
    rg->emission = 1.0;
    rg->blend_mode0 = 1;
    rg->blend_mode1 = 1;

    rg->sub = force_malloc_s(sizeof(glitter_render_group_sub), count);
    if (rg->particle_inst && rg->sub && rg->max_count > 0) {
        size = sizeof(glitter_buffer) * rg->max_count;
        rg->buffer = force_malloc(size);
        if (!rg->buffer)
            rg->max_count = 0;

        glGenBuffers(1, &rg->buffer_index);
        glGenVertexArrays(1, &rg->vao);
        if (rg->buffer_index && rg->vao) {
            glBindBuffer(GL_ARRAY_BUFFER, rg->buffer_index);
            glBufferData(GL_ARRAY_BUFFER, (int32_t)size, rg->buffer, GL_DYNAMIC_DRAW);
            glBindVertexArray(rg->vao);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(glitter_buffer), (void*)0x00);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(glitter_buffer), (void*)0x0C);
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 4, GL_FLOAT, false, sizeof(glitter_buffer), (void*)0x14);
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }
    else {
        rg->count = 0;
        rg->max_count = 0;
    }
    return rg;
}

void FASTCALL glitter_render_group_dispose(glitter_render_group* rg) {
    Glitter__RenderGroup__DeleteBuffers(rg, false);
    free(rg);
}

void FASTCALL Glitter__RenderGroup__LocusHistoryAppend(glitter_locus_history* a1,
    glitter_render_group_sub* a2, glitter_particle_inst* a3) {
    glitter_locus_history_data* v5;
    int64_t v6;
    int64_t v7;
    glitter_locus_history_data locus_history;
    vec3 temp;
    vec3 temp1;

    v5 = a1->data.begin;
    v6 = a1->data.end - a1->data.begin;
    if (a3->sub.data.flags & GLITTER_PARTICLE_FLAG_USE_MODEL_MAT)
        temp = *(vec3*)&a3->mat.row3;
    else
        temp = a2->translation;

    locus_history.translation = temp;
    locus_history.color = a2->color;
    locus_history.scale = a2->scale.x * a2->position_offset.x * a2->scale_all;
    if (v6 < 1)
        vector_glitter_locus_history_data_append_element(&a1->data, &locus_history);
    else if (v6 == 1) {
        locus_history.translation = v5->translation;
        if (a1->data.capacity_end - a1->data.begin > 1)
            vector_glitter_locus_history_data_append_element(&a1->data, &locus_history);
        v5->translation = temp;
    }
    else {
        temp1 = v5[v6 - 1].translation;

        for (v7 = v6 - 1; v7 > 0; v7--)
            v5[v7].translation = v5[v7 - 1].translation;

        if (v6 < a1->data.capacity_end - a1->data.begin) {
            locus_history.translation = temp1;
            vector_glitter_locus_history_data_append_element(&a1->data, &locus_history);
        }
        v5->translation = temp;
    }
}

glitter_render_group_sub* FASTCALL Glitter__RenderGroup__AddControl(glitter_render_group* a1,
    glitter_render_group_sub* a2) {
    int64_t v3;
    int64_t v4;

    if (!a2)
        a2 = a1->sub;

    v3 = a1->count - (a2 - a1->sub);
    v4 = 0;
    if (v3 <= 0)
        return 0;

    while (a2->alive) {
        v4++;
        a2++;
        if (v4 >= v3)
            return 0;
    }

    a2->alive = true;
    a1->ctrl++;
    return a2;
}

void FASTCALL Glitter__RenderGroup__Free(glitter_render_group* a1) {
    glitter_render_group_sub* sub;
    int32_t i;

    sub = a1->sub;
    for (i = 0; i < a1->count; i++, sub++) {
        sub->alive = false;
        if (sub->locus_history) {
            glitter_locus_history_dispose(sub->locus_history);
            sub->locus_history = 0;
        }
    }
    a1->ctrl = 0;
}

void FASTCALL Glitter__RenderGroup__Copy(glitter_render_group* a1, glitter_render_group* a2) {
    a2->flags = a1->flags;
    a2->type = a1->type;
    a2->draw_type = a1->draw_type;
    a2->blend_mode0 = a1->blend_mode0;
    a2->blend_mode1 = a1->blend_mode1;
    a2->pivot = a1->pivot;
    a2->split_u = a1->split_u;
    a2->split_v = a1->split_v;
    a2->split_uv = a1->split_uv;
    a2->z_offset = a1->z_offset;
    a2->count = min(a2->count, a1->count);
    a2->ctrl = a1->ctrl;
    a2->texture0 = a1->texture0;
    a2->texture1 = a1->texture1;
    a2->frame = a1->frame;
    a2->mat = a1->mat;
    a2->mat_no_scale = a1->mat_no_scale;
    a2->has_texture1 = a1->has_texture1;
    if (a2->count == a1->count) {
        memmove(a2->sub, a1->sub, sizeof(glitter_render_group_sub) * a2->count);
        a2->ctrl = a1->ctrl;
    }
}

void FASTCALL Glitter__RenderGroup__Emit(glitter_render_group* a1,
    glitter_particle_inst_data* a2, glitter_emitter_inst* a3, int32_t a4, int32_t count) {
    glitter_render_group_sub* v8; // rbp
    int64_t i; // r13
    int32_t index; // edi

    for (v8 = 0, i = a4; i > 0; i--)
        for (index = 0; index < count; index++, v8 = v8 + 1) {
            v8 = Glitter__RenderGroup__AddControl(a1, v8);
            if (!v8)
                break;

            Glitter__RenderGroup__EmitInit(a1->particle_inst, v8, a3, a2, index);
        }
}

bool FASTCALL Glitter__RenderGroup__GetA3DAScale(glitter_render_group* a1, vec3* a2) {
    glitter_effect_inst* effect_inst; // rcx

    if (!a1->particle_inst)
        return false;

    if (a1->particle_inst->sub.effect_inst)
        effect_inst = a1->particle_inst->sub.effect_inst;
    else {
        if (!a1->particle_inst->sub.parent)
            return false;

        effect_inst = a1->particle_inst->sub.parent->sub.effect_inst;
        if (!effect_inst)
            return false;
    }

    if (!(effect_inst->flags & 0x800))
        return false;

    if (a2)
        *a2 = effect_inst->a3da_scale;
    return true;
}

void FASTCALL Glitter__RenderGroup__sub_1403A6CE0(glitter_render_group_sub* a1,
    glitter_emitter_inst* a2, glitter_particle_inst_data* a3, int32_t index) {
    float_t speed; // xmm0_4
    float_t deceleration; // xmm0_4
    vec3 direction; // [rsp+30h] [rbp-89h] BYREF
    vec3 base_translation; // [rsp+40h] [rbp-79h] BYREF
    vec3 scale; // [rsp+70h] [rbp-49h] BYREF
    vec3 external_acceleration; // [rsp+80h] [rbp-39h] BYREF
    vec3 direction_random; // [rsp+90h] [rbp-29h] BYREF

    if (a3->data.flags & GLITTER_PARTICLE_FLAG_USE_MODEL_MAT || a3->data.flags & GLITTER_PARTICLE_FLAG_TRANSLATE_BY_EMITTER)
        a1->base_translation = (vec3){ 0.0f, 0.0f, 0.0f };
    else
        a1->base_translation = *(vec3*)&a2->mat.row3;

    scale = (vec3){ 1.0f, 1.0f, 1.0f };
    direction = a3->data.direction;
    if (!(a3->data.flags & GLITTER_PARTICLE_FLAG_USE_MODEL_MAT))
        vec3_mult_scalar(a2->scale, a2->scale_all, scale);

    base_translation = (vec3){ 0.0f, 0.0f, 0.0f };
    Glitter__EmitterInst__EffectVal__GetFloatType(a2, index, &scale, &base_translation, &direction);

    if (!(a3->data.flags & GLITTER_PARTICLE_FLAG_USE_MODEL_MAT))
        mat4_mult_vec3(&a2->mat_no_scale, &base_translation, &base_translation);

    vec3_add(a1->base_translation, base_translation, a1->base_translation);
    a1->translation = a1->base_translation;
    a1->translation_prev = a1->base_translation;
    Glitter__EffectVal__GetFloatVec3Clamp(&a3->data.direction_random, &direction_random);
    vec3_add(direction, direction_random, direction);
    vec3_normalize(direction, direction);

    if (!(a3->data.flags & GLITTER_PARTICLE_FLAG_USE_MODEL_MAT))
        mat4_mult_vec3(&a2->mat_no_scale, &direction, &direction);

    a1->direction = direction;
    speed = (Glitter__EffectVal__GetFloatClamp(a3->data.speed_random) + a3->data.speed) * 60.0f;
    deceleration = (Glitter__EffectVal__GetFloatClamp(a3->data.deceleration_random) + a3->data.deceleration) * 60.0f;
    a1->speed = speed;
    a1->deceleration = max(deceleration, 0.0f);

    Glitter__EffectVal__GetFloatVec3Clamp(&a3->data.external_acceleration_random, &external_acceleration);
    vec3_add(external_acceleration, a3->data.external_acceleration, external_acceleration);
    vec3_add(external_acceleration, a3->data.gravitational_acceleration, a1->acceleration);
}

void FASTCALL Glitter__RenderGroup__EmitInit(glitter_particle_inst* a1,
    glitter_render_group_sub* a2, glitter_emitter_inst* a3, glitter_particle_inst_data* a4, int32_t index) {
    int32_t max_uv; // ecx
    bool v19; // zf
    uint32_t locus_history_size;

    a2->uv_index = a4->data.uv_index;
    Glitter__Counter__Increment();
    Glitter__EffectVal__Set(Glitter__Counter__Get());
    a2->effect_val = Glitter__EffectVal__GetInt(Glitter__EffectVal__GetMax());
    a2->frame = 0.0;
    a2->rebound_time = 0.0;
    a2->uv = (vec2){ 0.0f, 0.0f };
    a2->life_time = a4->data.life_time;
    a2->color = a4->data.color;
    if (a4->data.draw_type == GLITTER_DIRECTION_PARTICLE_ROTATION) {
        a2->rotation.x = Glitter__EffectVal__GetFloatClamp(a4->data.rotation_random.x)
            + a4->data.rotation.x;
        a2->rotation_add.x = Glitter__EffectVal__GetFloatClamp(a4->data.rotation_add_random.x)
            + a4->data.rotation_add.x;
        a2->rotation.y = Glitter__EffectVal__GetFloatClamp(a4->data.rotation_random.y)
            + a4->data.rotation.y;
        a2->rotation_add.y = Glitter__EffectVal__GetFloatClamp(a4->data.rotation_add_random.y)
            + a4->data.rotation_add.y;
    }
    else {
        a2->rotation.x = 0.0f;
        a2->rotation.y = 0.0f;
        a2->rotation_add.x = 0.0f;
        a2->rotation_add.y = 0.0f;
    }
    a2->rot_z_cos = 1.0f;
    a2->rot_z_sin = 0.0f;
    a2->rotation.z = Glitter__EffectVal__GetFloatClamp(a4->data.rotation_random.z)
        + a4->data.rotation.z;
    a2->rotation_add.z = Glitter__EffectVal__GetFloatClamp(a4->data.rotation_add_random.z)
        + a4->data.rotation_add.z;
    a2->uv_scroll = (vec2){ 0.0f, 0.0f };
    a2->scale = (vec3){ 1.0f, 1.0f, 1.0f };
    a2->scale_all = 1.0f;
    a2->frame_step_uv = a4->data.frame_step_uv;

    a2->position_offset.x = Glitter__EffectVal__GetFloatClamp(a4->data.position_offset_random.x)
        + a4->data.position_offset.x;
    if (a4->data.flags & GLITTER_PARTICLE_FLAG_POSITION_OFFSET_SAME)
        a2->position_offset.y = a2->position_offset.x;
    else
        a2->position_offset.y = Glitter__EffectVal__GetFloatClamp(a4->data.position_offset_random.y)
        + a4->data.position_offset.y;

    max_uv = a4->data.split_u * a4->data.split_v;
    if (max_uv && max_uv - 1 > 1) {
        if (((a4->data.uv_index_type - 1) & 0xFFFFFFFA) == 0
            && a4->data.uv_index_type != 2 && a4->data.uv_index_count > 1)
            a2->uv_index = a4->data.uv_index_start + Glitter__EffectVal__GetInt(a4->data.uv_index_count);

        a2->uv.x = (float_t)(a2->uv_index % a1->sub.data.split_u) * a1->sub.data.split_uv.x;
        a2->uv.y = (float_t)(a2->uv_index / a1->sub.data.split_u) * a1->sub.data.split_uv.y;
    }

    Glitter__RenderGroup__sub_1403A6CE0(a2, a3, a4, index);

    if (a4->data.flags & GLITTER_PARTICLE_FLAG_TRANSLATE_BY_EMITTER
        || a4->data.draw_type == GLITTER_DIRECTION_EMITTER_ROTATION) {
        switch (a3->data.type) {
        case GLITTER_EMITTER_CYLINDER:
            v19 = a3->data.data.cylinder.direction == GLITTER_EMITTER_EMISSION_DIRECTION_PARTICLE_VELOCITY;
            if (!v19) {
                float_t length;
                vec3_length(a2->direction, length);
                v19 = length <= 0.000001f;
            }
            break;
        case GLITTER_EMITTER_SPHERE:
            v19 = a3->data.data.sphere.direction == GLITTER_EMITTER_EMISSION_DIRECTION_PARTICLE_VELOCITY;
            if (!v19) {
                float_t length;
                vec3_length(a2->direction, length);
                v19 = length <= 0.000001f;
            }
            break;
        default:
            v19 = true;
            break;
        }

        if (v19) {
            a2->mat = a3->mat;
            if (a4->data.flags & GLITTER_PARTICLE_FLAG_USE_MODEL_MAT)
                a2->mat.row3 = (vec4){ 0.0f, 0.0f, 0.0, 1.0f };
        }
        else
            mat4_identity(&a2->mat);
    }
    else
        mat4_identity(&a2->mat);

    if (a1->sub.data.type == GLITTER_PARTICLE_LOCUS) {
        locus_history_size = Glitter__EffectVal__Clamp(-a1->sub.data.locus_history_size_random,
            a1->sub.data.locus_history_size_random) + a1->sub.data.locus_history_size;
        a2->locus_history = glitter_locus_history_init(locus_history_size);
    }
    Glitter__EffectVal__Set(Glitter__EffectVal__Get() + 1);
}

void FASTCALL Glitter__RenderGroup__ClampColor(glitter_particle_inst* a1,
    float_t* r, float_t* g, float_t* b, float_t* a) {
    glitter_effect_inst* effect_inst;

    if (a1->sub.effect_inst)
        effect_inst = a1->sub.effect_inst;
    else {
        if (!a1->sub.parent)
            return;
        effect_inst = a1->sub.parent->sub.effect_inst;
        if (!effect_inst)
            return;
    }

    if (!(effect_inst->flags & 0x400))
        return;

    if (effect_inst->flags & 0x200) {
        if (effect_inst->min_color.x >= 0.0)
            *r = effect_inst->min_color.x;
        if (effect_inst->min_color.y >= 0.0)
            *g = effect_inst->min_color.y;
        if (effect_inst->min_color.z >= 0.0)
            *b = effect_inst->min_color.z;
        if (effect_inst->min_color.w >= 0.0)
            *a = effect_inst->min_color.w;
    }
    else {
        *r += effect_inst->min_color.x;
        *g += effect_inst->min_color.y;
        *b += effect_inst->min_color.z;
        *a += effect_inst->min_color.w;
    }

    if (*r < 0.0)
        *r = 0.0;
    if (*g < 0.0)
        *g = 0.0;
    if (*b < 0.0)
        *b = 0.0;
    if (*a < 0.0)
        *a = 0.0;
}

void FASTCALL Glitter__RenderGroup__GetColor(glitter_particle_inst* a1, glitter_render_group_sub* a2) {
    float_t r;
    float_t g;
    float_t b;
    float_t a;

    r = a2->color.x;
    if (r < 0.0)
        r = a1->sub.data.color.x;
    g = a2->color.y;
    if (g < 0.0)
        g = a1->sub.data.color.y;
    b = a2->color.z;
    if (b < 0.0)
        b = a1->sub.data.color.z;
    a = a2->color.w;
    if (a < 0.0)
        a = a1->sub.data.color.w;
    Glitter__RenderGroup__ClampColor(a1, &r, &g, &b, &a);
    a2->color.x = r;
    a2->color.y = g;
    a2->color.z = b;
    a2->color.w = a;
}

void FASTCALL Glitter__RenderGroup__Accelerate(glitter_particle_inst* a1,
    glitter_render_group_sub* a2, float_t rebound_time, float_t delta_frame) {
    vec3 acceleration;
    vec3 direction;
    float_t delta_time;
    float_t reflection_coeff;
    float_t deceleration;

    a2->translation_prev = a2->translation;
    delta_time = delta_frame * (float_t)(1.0 / 60.0);
    rebound_time -= a2->rebound_time + delta_time;
    deceleration = a2->deceleration * delta_time * (delta_time * 0.5f - rebound_time) + a2->speed;
    vec3_mult_scalar(a2->acceleration, delta_time * (delta_time * 0.5f + rebound_time), acceleration);
    if (deceleration >= 0.0099999998f) {
        vec3_mult_scalar(a2->direction, deceleration * delta_time, direction);
        vec3_add(acceleration, direction, acceleration);
    }

    vec3_add(a2->translation, acceleration, a2->translation);
    vec3_add(a2->base_translation, acceleration, a2->base_translation);
    if (a1->sub.data.flags & GLITTER_PARTICLE_FLAG_REBOUND_PLANE
        && a2->translation_prev.y > a1->sub.data.rebound_plane_y
        && a2->translation.y <= a1->sub.data.rebound_plane_y) {
        reflection_coeff = Glitter__EffectVal__GetFloatClamp(a1->sub.data.reflection_coeff_random)
            + a1->sub.data.reflection_coeff;
        a2->rebound_time = rebound_time;
        vec3_sub(a2->translation, a2->translation_prev, direction);
        vec3_mult_scalar(direction, reflection_coeff * 60.0f, direction);
        vec3_xor(direction, ((vec3){ 0.0f, -0.0f, 0.0f }), a2->direction);
        a2->translation.y = a2->translation_prev.y;
    }
}

void FASTCALL Glitter__RenderGroup__CopyFromParticle(glitter_render_group* render_group,
    float_t delta_frame, bool a3) {
    glitter_particle_inst* particle_inst; // rax
    glitter_particle_inst_data* sub;
    int32_t ctrl; // esi
    int32_t i;

    if (!render_group->particle_inst)
        return;

    particle_inst = render_group->particle_inst;
    sub = &particle_inst->sub;
    render_group->blend_mode0 = sub->data.blend_mode0;
    render_group->blend_mode1 = sub->data.blend_mode1;
    render_group->texture0 = sub->data.texture0;
    render_group->texture1 = sub->data.texture1;
    render_group->split_u = sub->data.split_u;
    render_group->split_v = sub->data.split_v;
    render_group->split_uv = sub->data.split_uv;
    render_group->type = sub->data.type;
    render_group->draw_type = sub->data.draw_type;
    render_group->z_offset = sub->data.z_offset;
    render_group->pivot = sub->data.pivot;
    render_group->flags = sub->data.flags;

    memcpy(render_group->dword48, sub->data.dword138, 32);
    if (!a3) {
        render_group->mat = particle_inst->mat;
        render_group->mat_no_scale = particle_inst->mat_no_scale;
    }

    render_group->has_texture1 = sub->data.texture1 != 0;
    for (ctrl = render_group->ctrl, i = 0; ctrl > 0; i++) {
        if (!render_group->sub[i].alive)
            continue;

        Glitter__RenderGroup__GetValue(render_group, &render_group->sub[i], delta_frame);
        ctrl--;
    }
    render_group->frame += delta_frame;
}

void FASTCALL Glitter__RenderGroup__GetValue(glitter_render_group* a1,
    glitter_render_group_sub* a2, float_t delta_frame) {
    glitter_particle_inst* v4; // rcx
    vec2 uv_scroll;
    bool visible;

    v4 = a1->particle_inst;
    if ((v4->sub.data.flags & GLITTER_PARTICLE_FLAG_LOOP
        && !Glitter__ParticleInst__HasEnded(v4, false)) || a2->frame <= a2->life_time) {
        Glitter__RenderGroup__Accelerate(v4, a2, a2->frame / 60.0f, delta_frame);
        if (v4->sub.data.draw_type == GLITTER_DIRECTION_PARTICLE_ROTATION) {
            a2->rotation.x += a2->rotation_add.x * delta_frame;
            a2->rotation.y += a2->rotation_add.y * delta_frame;
        }
        a2->rotation.z += a2->rotation_add.z * delta_frame;
        vec2_mult_scalar(v4->sub.data.uv_scroll_add, v4->sub.data.uv_scroll_add_scale * delta_frame, uv_scroll);
        vec2_add(a2->uv_scroll, uv_scroll, a2->uv_scroll);
        Glitter__RenderGroup__StepUV(v4, a2, delta_frame);
        a2->color = (vec4){ -1.0f, -1.0f, -1.0f, -1.0f };

        visible = true;
        if (v4->sub.data.sub_flags & GLITTER_PARTICLE_SUB_FLAG_GET_VALUE)
            visible = Glitter__ParticleInst__GetValue(v4, a2, a2->frame) != 0;

        if (v4->sub.data.draw_type == GLITTER_DIRECTION_PARTICLE_ROTATION
            || fabs(a2->rotation.z) <= 0.000001f) {
            a2->rot_z_cos = 1.0f;
            a2->rot_z_sin = 0.0f;
        }
        else {
            a2->rot_z_cos = cosf(a2->rotation.z);
            a2->rot_z_sin = sinf(a2->rotation.z);
        }

        if (visible)
            Glitter__RenderGroup__GetColor(v4, a2);

        if (v4->sub.data.type == GLITTER_PARTICLE_LOCUS)
            Glitter__RenderGroup__LocusHistoryAppend(a2->locus_history, a2, v4);

        a2->frame += delta_frame;
        if (v4->sub.data.flags & GLITTER_PARTICLE_FLAG_LOOP && a2->frame >= a2->life_time)
            a2->frame -= a2->life_time;
        return;
    }
    else {
        a2->alive = false;
        if (a2->locus_history) {
            glitter_locus_history_dispose(a2->locus_history);
            a2->locus_history = 0;
        }
        a1->ctrl--;
    }
}

void FASTCALL Glitter__RenderGroup__StepUV(glitter_particle_inst* a1,
    glitter_render_group_sub* a2, float_t delta_frame) {
    int32_t max_uv;

    if (a1->sub.data.frame_step_uv <= 0.0f)
        return;

    while (a2->frame_step_uv <= 0.0) {
        max_uv = a1->sub.data.split_u * a1->sub.data.split_v;
        if (max_uv)
            max_uv = max_uv - 1;

        switch (a1->sub.data.uv_index_type) {
        case GLITTER_UV_INDEX_RANDOM:
            a2->uv_index = a1->sub.data.uv_index_start;
            if (a1->sub.data.uv_index_count > 1)
                a2->uv_index += a1->sub.data.uv_index_start
                    + Glitter__EffectVal__GetInt(a1->sub.data.uv_index_count);
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

        a2->uv.x = (a2->uv_index % a1->sub.data.split_u) * a1->sub.data.split_uv.x;
        a2->uv.y = (a2->uv_index / a1->sub.data.split_u) * a1->sub.data.split_uv.y;
        a2->frame_step_uv += a1->sub.data.frame_step_uv;
    }
    a2->frame_step_uv -= delta_frame;
}

void FASTCALL Glitter__RenderGroup__DeleteBuffers(glitter_render_group* a1, bool a2) {
    if (a1->particle_inst) {
        if (!a2)
            a1->particle_inst->sub.render_group = 0;
        a1->particle_inst = 0;
    }

    if (a1->buffer_index) {
        glDeleteBuffers(1, &a1->buffer_index);
        a1->buffer_index = 0;
    }

    if (a1->vao) {
        glDeleteVertexArrays(1, &a1->vao);
        a1->vao = 0;
    }
    free(a1->buffer);

    if (!a2 && a1->sub) {
        Glitter__RenderGroup__Free(a1);
        free(a1->sub);
    }
}

void FASTCALL Glitter__RenderGroup__DrawQuad__RotateToAxis1(mat4* mat,
    glitter_render_group* a2, glitter_render_group_sub* a3) {
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
    mat4_rotate_z((float_t)M_PI, mat);

    if (fabsf(vec2.y) >= 0.000001f) {
        vec1 = (vec3){ 0.0f, 1.0f, 0.0f };
        axis_angle_from_vectors(&axis, &angle, &vec1, &vec2);
        mat4_mult_axis_angle(mat, mat, &axis, angle);
    }
}

void FASTCALL Glitter__RenderGroup__DrawQuad__RotateToAxis2(mat4* mat,
    glitter_render_group* a2, glitter_render_group_sub* a3) {
    vec3 vec;
    vec3 vec1;
    vec3 vec2;
    float_t angle;
    vec3 axis;

    if (a2->flags & GLITTER_PARTICLE_FLAG_USE_MODEL_MAT)
        vec = a3->translation;
    else
        vec3_sub(*(vec3*)&a2->mat.row3, a3->translation, vec);

    if (fabsf(vec.y) >= 0.000001f) {
        vec1 = (vec3){ 0.0f, 0.0f, 1.0f };
        mat4_identity(mat);
        vec3_normalize(vec, vec2);
        axis_angle_from_vectors(&axis, &angle, &vec1, &vec2);
        mat4_mult_axis_angle(mat, mat, &axis, angle);
    }
    else
        mat4_rotate_z((float_t)M_PI, mat);
}

void FASTCALL Glitter__RenderGroup__Draw__SetPivot(glitter_pivot a1,
    float_t a2, float_t a3, float_t* v00, float_t* v01, float_t* v10, float_t* v11) {
    switch (a1) {
    case GLITTER_PIVOT_TOP_LEFT:
        *v00 = 0.0;
        *v01 = a2;
        *v10 = -a3;
        *v11 = 0.0;
        break;
    case GLITTER_PIVOT_TOP_CENTER:
        *v00 = a2 * -0.5f;
        *v01 = a2 * 0.5f;
        *v10 = -a3;
        *v11 = 0.0f;
        break;
    case GLITTER_PIVOT_TOP_RIGHT:
        *v00 = -a2;
        *v01 = 0.0f;
        *v10 = -a3;
        *v11 = 0.0f;
        break;
    case GLITTER_PIVOT_MIDDLE_LEFT:
        *v00 = 0.0;
        *v01 = a2;
        *v10 = a3 * -0.5f;
        *v11 = a3 * 0.5f;
        break;
    case GLITTER_PIVOT_MIDDLE_CENTER:
    default:
        *v00 = a2 * -0.5f;
        *v01 = a2 * 0.5f;
        *v10 = a3 * -0.5f;
        *v11 = a3 * 0.5f;
        break;
    case GLITTER_PIVOT_MIDDLE_RIGHT:
        *v00 = -a2;
        *v01 = 0.0f;
        *v10 = a3 * -0.5f;
        *v11 = a3 * 0.5f;
        break;
    case GLITTER_PIVOT_BOTTOM_LEFT:
        *v00 = 0.0f;
        *v01 = a2;
        *v10 = 0.0f;
        *v11 = a3;
        break;
    case GLITTER_PIVOT_BOTTOM_CENTER:
        *v00 = a2 * -0.5f;
        *v01 = a2 * 0.5f;
        *v10 = 0.0f;
        *v11 = a3;
        break;
    case GLITTER_PIVOT_BOTTOM_RIGHT:
        *v00 = -a2;
        *v01 = 0.0f;
        *v10 = 0.0f;
        *v11 = a3;
        break;
    }
}

void FASTCALL Glitter__RenderGroup__Draw(glitter_scene_sub* a1, glitter_render_group* a2) {
    switch (a2->type) {
    case GLITTER_PARTICLE_QUAD:
        Glitter__RenderGroup__DrawQuad(a1, a2);
        break;
    case GLITTER_PARTICLE_LINE:
        Glitter__RenderGroup__DrawLine(a1, a2);
        break;
    case GLITTER_PARTICLE_LOCUS:
        Glitter__RenderGroup__DrawLocus(a1, a2);
        break;
    }
}

void FASTCALL Glitter__RenderGroup__DrawLine(glitter_scene_sub* a1, glitter_render_group* a2) {
    int64_t v4; // er14
    int32_t v6; // edi
    int32_t v7; // esi
    glitter_render_group_sub* v8; // rbx
    vec3 v14; // xmm8_4
    vec3 v15;
    vec3 v16;
    bool v17;
    bool v18;
    bool v19;
    bool v20;
    glitter_buffer* v23; // rbx
    int32_t v24; // esi
    int32_t v25; // eax
    glitter_render_group_sub* v27; // r15
    glitter_locus_history* v30; // r12
    glitter_locus_history_data* v32; // rdx
    int32_t v42; // ecx
    float_t emission; // [rsp+20h] [rbp-28h] BYREF
    mat4 mat;
    vector_int32_t vec_key;
    vector_int32_t vec_val;

    if (!a2->sub || !a2->buffer || a2->ctrl <= 0)
        return;

    v4 = 0;
    v6 = a2->ctrl;
    v7 = 0;
    v8 = a2->sub;
    while (v7 < v6) {
        if (v8->alive) {
            if (v8->locus_history) {
                int64_t length = v8->locus_history->data.end - v8->locus_history->data.begin;
                if (length > 1)
                    v4 += length;
            }
            v6--;
        }
        v7++;
        v8++;
    }

    if (!v4 || v4 > a2->max_count)
        return;

    glEnablei(GL_BLEND, 0);
    switch (a2->blend_mode0) {
    case 2:
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ZERO, GL_ONE);
        break;
    case 3:
    default:
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        break;
    case 4:
        glBlendFuncSeparate(GL_ZERO, GL_SRC_COLOR, GL_ZERO, GL_ONE);
        break;
    }
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(false);
    if (a2->flags & GLITTER_PARTICLE_FLAG_EMISSION || a2->blend_mode0 == 1)
        emission = a2->emission;
    else
        emission = 1.0f;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glitter_shader_flags[0] = 0;
    glitter_shader_flags[1] = 0;
    glitter_shader_flags[2] = 0;
    glitter_shader_flags[3] = 2;
    shader_fbo_use(&particle_shader);
    shader_fbo_set_int_array(&particle_shader, "mode", 4, glitter_shader_flags);
    shader_fbo_set_float(&particle_shader, "emission", emission);;
    glEnable(GL_CULL_FACE);

    mat4_identity(&mat);
    v14 = (vec3){ 0.0f, 0.0f, 0.0f };
    v17 = false;
    v18 = false;
    v19 = false;
    if (a2->flags & GLITTER_PARTICLE_FLAG_USE_MODEL_MAT) {
        vec4_length(a2->mat.row0, v14.x);
        vec4_length(a2->mat.row1, v14.y);
        vec4_length(a2->mat.row2, v14.z);
        vec3_sub(v14, ((vec3) { 1.0f, 1.0f, 1.0f }), v14);
        v17 = fabsf(v14.x) > 0.000001f ? true : false;
        v18 = fabsf(v14.y) > 0.000001f ? true : false;
        v19 = fabsf(v14.z) > 0.000001f ? true : false;
        mat4_normalize_rotation(&a2->mat, &mat);
    }
    shader_fbo_set_mat4(&particle_shader, "model", false, &mat);
    v20 = v17 | v18 | v19;

    glBindBuffer(GL_ARRAY_BUFFER, a2->buffer_index);
    v23 = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    vec_key.begin = vec_key.end = vec_key.capacity_end = 0;
    vec_val.begin = vec_val.end = vec_val.capacity_end = 0;
    vector_int32_t_expand(&vec_key, 0x80);
    vector_int32_t_expand(&vec_val, 0x80);
    if (a2->ctrl > 0) {
        v24 = 0;
        v42 = 0;
        v25 = a2->ctrl;
        v27 = a2->sub;
        while (v25 > 0) {
            if (!v27->alive) {
                v27++;
                continue;
            }

            v30 = v27->locus_history;
            if (v30->data.end - v30->data.begin < 2) {
                v25--;
                v27++;
            }

            for (v32 = v30->data.begin; v30->data.end != v32; v32++, v23++, v24++) {
                v15 = v32->translation;
                if (v20) {
                    vec3_sub(v15, v27->base_translation, v16);
                    vec3_mult(v16, v14, v16);
                    if (!v17)
                        v16.x = 0.0f;
                    if (!v18)
                        v16.y = 0.0f;
                    if (!v19)
                        v16.z = 0.0f;
                    vec3_add(v15, v16, v15);
                }
                v23->position = v15;
                v23->uv = (vec2){ 0.0f, 0.0f };
                v23->color = v32->color;
            }

            vector_int32_t_append_element(&vec_key, &v42);
            v42 = v24 - v42;
            vector_int32_t_append_element(&vec_val, &v42);
            v42 = v24;

            v25--;
            v27++;
        }
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glBindVertexArray(a2->vao);
    if (v24 > 0) {
        a1->disp_line += v24;
        glMultiDrawArrays(GL_LINE_STRIP, vec_key.begin, vec_val.begin, (GLsizei)(vec_key.end - vec_key.begin));
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisablei(GL_BLEND, 0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_ALPHA_TEST);
    glAlphaFunc(GL_ALWAYS, 0.0);
    free(vec_key.begin);
    free(vec_val.begin);
}

void FASTCALL Glitter__RenderGroup__DrawLocus(glitter_scene_sub* a1, glitter_render_group* a2) {
    int64_t v4; // r14
    int32_t v6; // edi
    int32_t v7; // esi
    glitter_render_group_sub* v8; // rbx
    vec3 v16; // xmm13_4
    bool v22;
    bool v23;
    bool v24;
    bool v25;
    int32_t v28; // er12
    int32_t v29; // ecx
    glitter_render_group_sub* v32; // r14
    glitter_locus_history* v33; // rax
    float_t v35; // xmm12_4
    float_t v36; // xmm9_4
    float_t v37; // xmm10_4
    float_t v38; // xmm11_4
    glitter_buffer* v41; // rsi
    vec3 v43; // xmm6_4
    float_t emission; // [rsp+20h] [rbp-28h] BYREF
    vec3 v73; // [rsp+58h] [rbp-A8h]
    uint32_t v78; // [rsp+98h] [rbp-68h]
    float_t v10; // [rsp+B0h] [rbp-50h] FORCED
    float_t v11; // [rsp+C0h] [rbp-40h] BYREF
    float_t v00; // [rsp+C8h] [rbp-38h] BYREF
    float_t v01; // [rsp+CCh] [rbp-34h] BYREF
    vec3 scale;
    glitter_locus_history_data* v95; // [rsp+128h] [rbp+28h] BYREF
    mat4 mat2; // [rsp+130h] [rbp+30h] BYREF
    mat4 mat3; // [rsp+170h] [rbp+70h] BYREF
    mat4 mat1; // [rsp+1B0h] [rbp+B0h] BYREF
    vector_int32_t vec_key;
    vector_int32_t vec_val;

    if (!a2->sub || a2->ctrl <= 0)
        return;

    for (v4 = 0, v6 = a2->ctrl, v7 = 0, v8 = a2->sub; v7 < v6; v7++, v8++) {
        if (!v8->alive)
            continue;

        if (v8->locus_history) {
            int64_t length = v8->locus_history->data.end - v8->locus_history->data.begin;
            if (length > 1)
                v4 += 2 * length;
        }
        v6--;
    }

    if (!v4 || v4 > a2->max_count || !a2->buffer)
        return;

    glEnablei(GL_BLEND, 0);
    switch (a2->blend_mode0) {
    case 2:
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ZERO, GL_ONE);
        break;
    case 3:
    default:
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        break;
    case 4:
        glBlendFuncSeparate(GL_ZERO, GL_SRC_COLOR, GL_ZERO, GL_ONE);
        break;
    }
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(false);
    if (a2->flags & GLITTER_PARTICLE_FLAG_EMISSION || a2->blend_mode0 == 1)
        emission = a2->emission;
    else
        emission = 1.0f;
    
    glActiveTexture(GL_TEXTURE0);
    if (a2->texture0) {
        glBindTexture(GL_TEXTURE_2D, a2->texture0);
        glActiveTexture(GL_TEXTURE1);
        if (a2->texture1) {
            glBindTexture(GL_TEXTURE_2D, a2->texture1);

            switch (a2->blend_mode1) {
            case 2:
                glitter_shader_flags[0] = 2;
                glitter_shader_flags[1] = 2;
                break;
            case 3:
            default:
                glitter_shader_flags[0] = 2;
                glitter_shader_flags[1] = 0;
                break;
            case 4:
                glitter_shader_flags[0] = 2;
                glitter_shader_flags[1] = 1;
                break;
            }
        }
        else {
            glBindTexture(GL_TEXTURE_2D, 0);
            glitter_shader_flags[0] = 1;
            glitter_shader_flags[1] = 0;
        }
    }
    else {
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        glitter_shader_flags[0] = 0;
        glitter_shader_flags[1] = 0;
    }

    glitter_shader_flags[2] = 0;
    glitter_shader_flags[3] = 2;
    shader_fbo_use(&particle_shader);
    shader_fbo_set_int_array(&particle_shader, "mode", 4, glitter_shader_flags);
    shader_fbo_set_float(&particle_shader, "emission", emission);;
    v16 = (vec3){ 0.0f, 0.0f, 0.0f };
    scale = (vec3){ 1.0f, 0.0f, 0.0f };
    mat1 = cam->view;
    if (a2->flags & GLITTER_PARTICLE_FLAG_LOCAL) {
        mat4_inverse(&mat1, &mat2);
        mat4_mult(&mat2, &a2->mat, &mat2);
        mat4_mult(&mat1, &mat2, &mat1);
    }
    mat4_inverse(&cam->view, &mat2);
    mat4_mult(&mat1, &mat2, &mat2);
    shader_fbo_set_mat4(&particle_shader, "model", false, &mat2);

    mat4_identity(&mat2);
    v73 = (vec3){ 0.0f, 0.0f, 0.0f };
    v22 = false;
    v23 = false;
    v24 = false;
    if (a2->flags & GLITTER_PARTICLE_FLAG_USE_MODEL_MAT) {
        vec4_length(a2->mat.row0, v16.x);
        vec4_length(a2->mat.row1, v16.y);
        vec4_length(a2->mat.row2, v16.z);
        if (a2->flags & GLITTER_PARTICLE_FLAG_SCALE)
            scale.x = v16.x;
        vec3_sub(v16, ((vec3) { 1.0f, 1.0f, 1.0f }), v16);
        mat4_mult_vec3(&a2->mat_no_scale, &v16, &v16);
        v22 = fabsf(v16.x) > 0.000001f ? true : false;
        v23 = fabsf(v16.y) > 0.000001f ? true : false;
        v24 = fabsf(v16.z) > 0.000001f ? true : false;
        mat4_normalize_rotation(&a2->mat, &mat2);
    }
    mat4_identity(&mat3);
    mat3.row3 = mat2.row3;
    v25 = v22 | v23 | v24;

    mat1.row3 = (vec4){ 0.0f, 0.0f, 0.0f, 1.0f };
    mat4_inverse(&mat1, &mat1);
    mat4_mult(&mat3, &mat1, &mat1);
    mat1.row3 = (vec4){ 0.0f, 0.0f, 0.0f, 1.0f };

    mat4_mult_vec3(&mat1, &scale, &scale);

    glBindBuffer(GL_ARRAY_BUFFER, a2->buffer_index);
    v41 = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    vec_key.begin = vec_key.end = vec_key.capacity_end = 0;
    vec_val.begin = vec_val.end = vec_val.capacity_end = 0;
    vector_int32_t_expand(&vec_key, 0x80);
    vector_int32_t_expand(&vec_val, 0x80);
    v78 = 0;
    v00 = 0.0f;
    v01 = 0.0f;

    v78 = 0;
    for (v29 = a2->ctrl, v32 = a2->sub; v29 > 0; v32++) {
        if (!v32->alive)
            continue;

        v29--;
        v33 = v32->locus_history;
        if (!v33 || v33->data.end - v33->data.begin <= 1)
            continue;

        v35 = v32->uv.x + v32->uv_scroll.x;
        v36 = v32->uv.x + a2->split_uv.x + v32->uv_scroll.x;
        v37 = v32->uv.y + a2->split_uv.y + v32->uv_scroll.y;
        v38 = a2->split_uv.y / (float_t)(v33->data.end - v33->data.begin - 1);

        for (v28 = 0, v95 = v33->data.begin; v95 != v33->data.end; v28++, v41 += 2, v95++) {
            v43 = v95->translation;
            if (v25) {
                vec3_sub(v43, v32->base_translation, v73);
                vec3_mult(v73, v16, v73);
                if (!v22)
                    v73.x = 0.0f;
                if (!v23)
                    v73.y = 0.0f;
                if (!v24)
                    v73.z = 0.0f;
                mat4_mult_vec3(&mat2, &v73, &v73);
                vec3_add(v43, v73, v43);
            }

            Glitter__RenderGroup__Draw__SetPivot(a2->pivot,
                v95->scale * v32->scale.x * v32->scale_all,
                v32->scale.y * v32->position_offset.y * v32->scale_all,
                &v00, &v01, &v10, &v11);

            vec3_mult_scalar(scale, v00, v41[0].position);
            vec3_add(v41[0].position, v43, v41[0].position);
            v41[0].uv.x = v35;
            v41[0].uv.y = 1.0f - (float_t)(v37 - (double_t)v28 * v38);
            v41[0].color = v95->color;

            vec3_mult_scalar(scale, v01, v41[1].position);
            vec3_add(v41[1].position, v43, v41[1].position);
            v41[1].uv.x = v36;
            v41[1].uv.y = 1.0f - (float_t)(v37 - (double_t)v28 * v38);
            v41[1].color = v95->color;
        }

        if (v28 > 0) {
            a1->disp_locus += v28;
            vector_int32_t_append_element(&vec_key, &v78);
            v78 = 2 * v28;
            vector_int32_t_append_element(&vec_val, &v78);
        }
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glBindVertexArray(a2->vao);
    glMultiDrawArrays(GL_TRIANGLE_STRIP, vec_key.begin, vec_val.begin, (GLsizei)(vec_key.end - vec_key.begin));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisablei(GL_BLEND, 0);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_ALPHA_TEST);
    glAlphaFunc(GL_ALWAYS, 0.0);
    free(vec_key.begin);
    free(vec_val.begin);
}

void FASTCALL Glitter__RenderGroup__DrawQuad(glitter_scene_sub* a1, glitter_render_group* a2) {
    mat4 mat1;
    mat4 mat2;
    mat4 mat3;
    float_t emission;

    if (a2->ctrl <= 0 || a2->ctrl * 6 > a2->max_count || !a2->buffer || !a2->sub)
        return;

    glEnablei(GL_BLEND, 0);
    switch (a2->blend_mode0) {
    case 2:
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ZERO, GL_ONE);
        break;
    case 3:
    default:
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        break;
    case 4:
        glBlendFuncSeparate(GL_ZERO, GL_SRC_COLOR, GL_ZERO, GL_ONE);
        break;
    }

    if (a2->flags & GLITTER_PARTICLE_FLAG_EMISSION || a2->blend_mode0 == 1)
        emission = a2->emission;
    else
        emission = 1.0f;

    glActiveTexture(GL_TEXTURE0);
    if (a2->texture0) {
        glBindTexture(GL_TEXTURE_2D, a2->texture0);
        glActiveTexture(GL_TEXTURE1);
        if (a2->texture1) {
            glBindTexture(GL_TEXTURE_2D, a2->texture1);

            switch (a2->blend_mode1) {
            case 2:
                glitter_shader_flags[0] = 2;
                glitter_shader_flags[1] = 2;
                break;
            case 3:
            default:
                glitter_shader_flags[0] = 2;
                glitter_shader_flags[1] = 0;
                break;
            case 4:
                glitter_shader_flags[0] = 2;
                glitter_shader_flags[1] = 1;
                break;
            }
        }
        else {
            glBindTexture(GL_TEXTURE_2D, 0);
            glitter_shader_flags[0] = 1;
            glitter_shader_flags[1] = 0;
        }
    }
    else {
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        glitter_shader_flags[0] = 0;
        glitter_shader_flags[1] = 0;
    }

    switch (a2->fog) {
    default:
        glitter_shader_flags[2] = 0;
        break;
    case 1:
        glitter_shader_flags[2] = 1;
        break;
    case 2:
        glitter_shader_flags[2] = 2;
        break;
    }

    if (a2->blend_mode0 == 5) {
        glEnable(GL_ALPHA_TEST);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(true);
        glitter_shader_flags[3] = 1;
    }
    else {
        glEnable(GL_ALPHA_TEST);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(false);
        glEnablei(GL_BLEND, 0);
        glitter_shader_flags[3] = a2->alpha ? 2 : 0;
    }

    shader_fbo_use(&particle_shader);
    shader_fbo_set_int_array(&particle_shader, "mode", 4, glitter_shader_flags);
    shader_fbo_set_float(&particle_shader, "emission", emission);

    if (a2->flags & GLITTER_PARTICLE_FLAG_USE_MODEL_MAT) {
        mat1 = a2->mat;
        mat4_normalize_rotation(&mat1, &mat2);
        mat4_mult(&mat2, &cam->view, &mat2);
    }
    else {
        mat4_identity(&mat1);
        mat2 = cam->view;
    }

    if (a2->flags & GLITTER_PARTICLE_FLAG_LOCAL) {
        mat4_inverse(&mat2, &mat3);
        mat4_mult(&mat3, &a2->mat, &mat3);
        mat4_mult(&mat2, &mat3, &mat2);
    }

    mat4_inverse(&cam->view, &mat3);
    mat4_mult(&mat2, &mat3, &mat3);
    shader_fbo_set_mat4(&particle_shader, "model", false, &mat3);

    if (a2->draw_type == GLITTER_DIRECTION_BILLBOARD)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);

    switch (a2->draw_type) {
    case GLITTER_DIRECTION_BILLBOARD:
        mat2.row3 = (vec4){ 0.0f, 0.0f, 0.0f, 1.0f };
        mat4_inverse(&mat2, &mat2);
        mat4_mult(&mat1, &mat2, &mat2);
        mat2.row3 = (vec4){ 0.0f, 0.0f, 0.0f, 1.0f };
        break;
    case GLITTER_DIRECTION_EMITTER_DIRECTION:
        mat2 = a2->mat_no_scale;
        break;
    case 2:
    case 3:
    case 8:
        if (a2->flags & GLITTER_PARTICLE_FLAG_USE_MODEL_MAT) {
            mat2 = a2->mat_no_scale;
            mat2.row3 = (vec4){ 0.0f, 0.0f, 0.0f, 1.0f };
        }
        else
            mat4_identity(&mat2);
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
        mat4_identity(&mat2);
        break;
    }

    switch (a2->draw_type) {
    case 2:
    case 8:
        Glitter__RenderGroup__DrawQuadSub2(a1, a2, &mat1, &mat2, &Glitter__RenderGroup__DrawQuad__RotateToAxis1);
        break;
    case 3:
        Glitter__RenderGroup__DrawQuadSub2(a1, a2, &mat1, &mat2, &Glitter__RenderGroup__DrawQuad__RotateToAxis2);
        break;
    default:
        Glitter__RenderGroup__DrawQuadSub1(a1, a2, &mat1, &mat2);
        break;
    }
}

void FASTCALL Glitter__RenderGroup__DrawQuadSub1(glitter_scene_sub* a1,
    glitter_render_group* a2, mat4* a3, mat4* a4) {
    glitter_render_group_sub* v9; // rdi
    int32_t i; // er12
    int32_t count; // er15
    int32_t v33; // er14
    glitter_buffer* v34; // rbx
    vec3 v35; // xmm6_4
    bool v40;
    bool v41;
    bool v42;
    bool v43;
    int32_t j; // er12
    vec3 v49; // xmm11_4
    int64_t v65; // er15
    vec2* v66; // r14
    bool v118; // [rsp+45h] [rbp-BBh]
    vec3 v119; // [rsp+48h] [rbp-B8h]
    vec3 scale; // [rsp+5Ch] [rbp-A4h]
    vec3 v124; // [rsp+68h] [rbp-98h] BYREF
    vec3 v125; // [rsp+78h] [rbp-88h] BYREF
    vec3 v129; // [rsp+B4h] [rbp-4Ch]
    float_t v01; // [rsp+E0h] [rbp-20h] BYREF
    float_t v11; // [rsp+F8h] [rbp-8h] BYREF
    float_t v10; // [rsp+FCh] [rbp-4h] BYREF
    float_t v00; // [rsp+100h] [rbp+0h] BYREF
    vec2 v144[8]; // [rsp+120h] [rbp+20h] BYREF
    vec3 v145; // [rsp+190h] [rbp+90h] BYREF
    mat4 a1a; // [rsp+200h] [rbp+100h] BYREF
    mat4 mat; // [rsp+240h] [rbp+140h] BYREF
    mat4 v157; // [rsp+280h] [rbp+180h] BYREF
    glitter_buffer buf[4];
    vec4 color;
    vec2 uv_scroll;
    float_t rot_z_cos;
    float_t rot_z_sin;

    memset(buf, 0, sizeof(glitter_buffer) * 4);
    mat4_inverse(a3, &a1a);
    a1a.row3 = (vec4){ 0.0f, 0.0f, 0.0f, 1.0f };
    v35 = (vec3){ 0.0f, 0.0f, 0.0f };
    v124 = (vec3){ 1.0f, 0.0f, 0.0f };
    v125 = (vec3){ 0.0f, 1.0f, 0.0f };
    v118 = false;
    scale = (vec3){ 0.0f, 0.0f, 0.0f };
    v129 = (vec3){ 0.0f, 0.0f, 0.0f };
    mat4_identity(&mat);
    if (fabsf(a2->z_offset) > 0.000001f) {
        v118 = true;
        vec3_sub(cam->position, *(vec3*)&a3->row3, v129);
        if (a2->flags & GLITTER_PARTICLE_FLAG_USE_MODEL_MAT) {
            mat4_normalize_rotation(a3, &mat);
            mat4_inverse(&mat, &mat);
        }
    }

    if (a2->flags & GLITTER_PARTICLE_FLAG_USE_MODEL_MAT) {
        vec4_length(a3->row0, v145.x);
        vec4_length(a3->row1, v145.y);
        vec4_length(a3->row2, v145.z);
        if (a2->flags & GLITTER_PARTICLE_FLAG_SCALE) {
            v124.x = v145.x;
            v125.y = v145.y;
        }
        vec3_sub(v145, ((vec3) { 1.0f, 1.0f, 1.0f }), v145);
        mat4_mult_vec3(&a2->mat_no_scale, &v145, &scale);
    }

    v119 = (vec3){ 0.0f, 0.0f, 0.0f };
    if (Glitter__RenderGroup__GetA3DAScale(a2, &v119)) {
        v124.x *= v119.x + 1.0f;
        v125.y *= v119.y + 1.0f;
    }

    if (a2->draw_type != GLITTER_DIRECTION_BILLBOARD) {
        mat4_mult_vec3(a4, &v124, &v124);
        mat4_mult_vec3(a4, &v125, &v125);
    }
    else
        mat4_mult(&a1a, a4, &a1a);

    mat4_mult_vec3(&a1a, &v124, &v124);
    mat4_mult_vec3(&a1a, &v125, &v125);

    glBindBuffer(GL_ARRAY_BUFFER, a2->buffer_index);
    v34 = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    v40 = fabsf(scale.x) > 0.000001f ? true : false;
    v41 = fabsf(scale.y) > 0.000001f ? true : false;
    v42 = fabsf(scale.z) > 0.000001f ? true : false;
    v43 = v40 | v41 | v42;

    v9 = a2->sub;
    count = 0;
    if (a2->draw_type == GLITTER_DIRECTION_PARTICLE_ROTATION)
        for (i = a2->ctrl, v33 = 1024; i > 0; i -= v33) {
            v33 = min(i, v33);
            for (j = v33; j > 0; v9++) {
                if (!v9->alive)
                    continue;
                j--;

                if (v9->color.w < 0.0099999998f)
                    continue;
                count++;

                color = v9->color;
                uv_scroll = v9->uv_scroll;
                v49 = v9->translation;
                if (v43) {
                    vec3_sub(v49, v9->base_translation, v119);
                    vec3_mult(v119, scale, v119);
                    if (!v40)
                        v119.x = 0.0f;
                    if (!v41)
                        v119.y = 0.0f;
                    if (!v42)
                        v119.z = 0.0f;
                    mat4_mult_vec3(&a1a, &v119, &v119);
                    vec3_add(v49, v119, v49);
                }
                Glitter__RenderGroup__Draw__SetPivot(a2->pivot,
                    v9->scale.x * v9->position_offset.x * v9->scale_all,
                    v9->scale.y * v9->position_offset.y * v9->scale_all,
                    &v00, &v01, &v10, &v11);

                v144[0].x = v00;
                v144[0].y = v11;
                v144[1].x = v9->uv.x;
                v144[1].y = v9->uv.y;
                v144[2].x = v00;
                v144[2].y = v10;
                v144[3].x = v9->uv.x;
                v144[3].y = v9->uv.y + a2->split_uv.y;
                v144[4].x = v01;
                v144[4].y = v10;
                v144[5].x = v9->uv.x + a2->split_uv.x;
                v144[5].y = v9->uv.y + a2->split_uv.y;
                v144[6].x = v01;
                v144[6].y = v11;
                v144[7].x = v9->uv.x + a2->split_uv.x;
                v144[7].y = v9->uv.y;
                if (v118) {
                    vec3_sub(v129, v49, v35);
                    vec3_normalize(v35, v35);

                    if (a2->flags & GLITTER_PARTICLE_FLAG_USE_MODEL_MAT)
                        mat4_mult_vec3(&mat, &v35, &v35);

                    vec3_mult_scalar(v35, a2->z_offset, v35);
                }

                v66 = v144;
                mat4_rotate(v9->rotation.x, v9->rotation.y, v9->rotation.z, &v157);
                for (v65 = 0; v65 < 4; v65++, v66 += 2) {
                    v145.x = v124.x * v66[0].x;
                    v145.y = v125.y * v66[0].y;
                    v145.z = 0.0;
                    mat4_mult_vec3(&v157, &v145, &v145);
                    vec3_add(v145, v49, v145);
                    vec3_add(v145, v35, buf[v65].position);
                    vec2_add(v66[1], uv_scroll, buf[v65].uv);
                    buf[v65].color = color;
                }

                v34[0] = buf[0];
                v34[1] = buf[1];
                v34[2] = buf[2];
                v34[3] = buf[0];
                v34[4] = buf[2];
                v34[5] = buf[3];
                v34 += 6;
            }
        }
    else
        for (i = a2->ctrl, v33 = 1024; i > 0; i -= v33) {
            v33 = min(i, v33);
            for (j = v33; j > 0; v9++) {
                if (!v9->alive)
                    continue;
                j--;

                if (v9->color.w < 0.0099999998f)
                    continue;
                count++;

                color = v9->color;
                uv_scroll = v9->uv_scroll;
                v49 = v9->translation;
                if (v43) {
                    vec3_sub(v49, v9->base_translation, v119);
                    vec3_mult(v119, scale, v119);
                    if (!v40)
                        v119.x = 0.0f;
                    if (!v41)
                        v119.y = 0.0f;
                    if (!v42)
                        v119.z = 0.0f;
                    mat4_mult_vec3(&a1a, &v119, &v119);
                    vec3_add(v49, v119, v49);
                }
                Glitter__RenderGroup__Draw__SetPivot(a2->pivot,
                    v9->scale.x * v9->position_offset.x * v9->scale_all,
                    v9->scale.y * v9->position_offset.y * v9->scale_all,
                    &v00, &v01, &v10, &v11);

                v144[0].x = v00;
                v144[0].y = v11;
                v144[1].x = v9->uv.x;
                v144[1].y = v9->uv.y;
                v144[2].x = v00;
                v144[2].y = v10;
                v144[3].x = v9->uv.x;
                v144[3].y = v9->uv.y + a2->split_uv.y;
                v144[4].x = v01;
                v144[4].y = v10;
                v144[5].x = v9->uv.x + a2->split_uv.x;
                v144[5].y = v9->uv.y + a2->split_uv.y;
                v144[6].x = v01;
                v144[6].y = v11;
                v144[7].x = v9->uv.x + a2->split_uv.x;
                v144[7].y = v9->uv.y;
                if (v118) {
                    vec3_sub(v129, v49, v35);
                    vec3_normalize(v35, v35);

                    if (a2->flags & GLITTER_PARTICLE_FLAG_USE_MODEL_MAT)
                        mat4_mult_vec3(&mat, &v35, &v35);

                    vec3_mult_scalar(v35, a2->z_offset, v35);
                }

                v66 = v144;
                rot_z_cos = v9->rot_z_cos;
                rot_z_sin = v9->rot_z_sin;
                for (v65 = 0; v65 < 4; v65++, v66 += 2) {
                    vec3_mult_scalar(v125, rot_z_sin * v66[0].x + rot_z_cos * v66[0].y, v119);
                    vec3_mult_scalar(v124, rot_z_cos * v66[0].x - rot_z_sin * v66[0].y, v145);
                    vec3_add(v145, v119, v145);
                    vec3_add(v145, v49, v145);
                    vec3_add(v145, v35, buf[v65].position);
                    vec2_add(v66[1], uv_scroll, buf[v65].uv);
                    buf[v65].color = color;
                }

                v34[0] = buf[0];
                v34[1] = buf[1];
                v34[2] = buf[2];
                v34[3] = buf[0];
                v34[4] = buf[2];
                v34[5] = buf[3];
                v34 += 6;
            }
        }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glBindVertexArray(a2->vao);
    Glitter__RenderGroup__DrawQuads(a1, count);
}

void FASTCALL Glitter__RenderGroup__DrawQuadSub2(glitter_scene_sub* a1, glitter_render_group* a2,
    mat4* a3, mat4* a4, void(FASTCALL* func)(mat4*, glitter_render_group*, glitter_render_group_sub*)) {
    glitter_render_group_sub* v9; // rdi
    int32_t i; // er15
    int32_t v14; // er14
    int32_t count; // esi
    glitter_buffer* v16; // rbx
    int32_t j; // esi
    float_t rot_z_cos; // xmm8_4
    float_t rot_z_sin; // xmm9_4
    vec2* v41; // rcx
    int64_t v42; // rdx
    vec3 v46; // xmm4_4
    vec3 v47; // xmm3_4
    float_t v11; // [rsp+7Ch] [rbp-84h] BYREF
    float_t v10; // [rsp+80h] [rbp-80h] BYREF
    float_t v01; // [rsp+84h] [rbp-7Ch] BYREF
    float_t v00; // [rsp+88h] [rbp-78h] BYREF
    vec2 v63[8]; // [rsp+A0h] [rbp-60h] BYREF
    vec3 v72; // [rsp+180h] [rbp+80h] BYREF
    vec3 v73; // [rsp+190h] [rbp+90h] BYREF
    mat4 mat; // [rsp+1C0h] [rbp+C0h] BYREF
    mat4 a1a; // [rsp+200h] [rbp+100h] BYREF
    vec3 scale;
    glitter_buffer buf[4];

    memset(buf, 0, sizeof(glitter_buffer) * 4);
    mat4_inverse(a3, &a1a);
    a1a.row3 = (vec4){ 0.0f, 0.0f, 0.0f, 1.0f };

    glBindBuffer(GL_ARRAY_BUFFER, a2->buffer_index);
    v16 = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    vec4_length(a3->row0, scale.x);
    vec4_length(a3->row1, scale.y);
    vec4_length(a3->row2, scale.z);

    v9 = a2->sub;
    count = 0;
    for (i = a2->ctrl, v14 = 1024; i > 0; i -= v14) {
        v14 = min(i, v14);
        for (j = v14; j > 0; v9++) {
            if (!v9->alive)
                continue;
            j--;

            if (v9->color.w < 0.0099999998f)
                continue;
            count++;

            Glitter__RenderGroup__Draw__SetPivot(a2->pivot,
                v9->scale.x * v9->position_offset.x * v9->scale_all,
                v9->scale.y * v9->position_offset.y * v9->scale_all,
                &v00, &v01, &v10, &v11);

            v63[0].x = v00;
            v63[0].y = v11;
            v63[1].x = v9->uv.x;
            v63[1].y = v9->uv.y;
            v63[2].x = v00;
            v63[2].y = v10;
            v63[3].x = v9->uv.x;
            v63[3].y = v9->uv.y + a2->split_uv.y;
            v63[4].x = v01;
            v63[4].y = v10;
            v63[5].x = v9->uv.x + a2->split_uv.x;
            v63[5].y = v9->uv.y + a2->split_uv.y;
            v63[6].x = v01;
            v63[6].y = v11;
            v63[7].x = v9->uv.x + a2->split_uv.x;
            v63[7].y = v9->uv.y;

            func(&mat, a2, v9);
            mat4_mult(a4, &mat, &mat);
            v73 = (vec3){ 1.0f, 0.0f, 0.0f };
            mat4_mult_vec3(&mat, &v73, &v73);
            mat4_mult_vec3(&a1a, &v73, &v73);
            v72 = (vec3){ 0.0f, 1.0f, 0.0f };
            mat4_mult_vec3(&mat, &v72, &v72);
            mat4_mult_vec3(&a1a, &v72, &v72);

            if (a2->flags & GLITTER_PARTICLE_FLAG_SCALE)
                vec3_mult(v73, scale, v73);

            rot_z_cos = cosf(v9->rotation.z);
            rot_z_sin = sinf(v9->rotation.z);
            for (v41 = v63, v42 = 0; v42 < 4; v41 += 2, v42++) {
                vec3_mult_scalar(v72, v41[0].x * rot_z_sin + v41[0].y * rot_z_cos, v47);
                vec3_mult_scalar(v73, v41[0].x * rot_z_cos - v41[0].y * rot_z_sin, v46);
                vec3_add(v47, v46, v47);
                vec3_add(v47, v9->translation, buf[v42].position);
                vec2_add(v9->uv_scroll, v41[1], buf[v42].uv);
                buf[v42].color = v9->color;
            }

            v16[0] = buf[0];
            v16[1] = buf[1];
            v16[2] = buf[2];
            v16[3] = buf[0];
            v16[4] = buf[2];
            v16[5] = buf[3];
            v16 += 6;
        }
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glBindVertexArray(a2->vao);
    Glitter__RenderGroup__DrawQuads(a1, count);
}

void FASTCALL Glitter__RenderGroup__DrawQuads(glitter_scene_sub* a1, int32_t count) {
    if (count > 0) {
        a1->disp_quad += count;
        glDrawArrays(GL_TRIANGLES, 0, 6 * count);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisablei(GL_BLEND, 0);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_ALPHA_TEST);
    glAlphaFunc(GL_ALWAYS, 0.0);
}
