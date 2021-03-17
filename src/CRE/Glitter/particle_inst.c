/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "particle_inst.h"
#include "curve.h"
#include "effect_inst.h"
#include "random.h"
#include "render_group.h"

glitter_particle_inst* FASTCALL glitter_particle_inst_init(glitter_particle* a1,
    glitter_scene* a2, glitter_effect_inst* a3) {
    glitter_render_group* render_group;

    glitter_particle_inst* pi = force_malloc(sizeof(glitter_particle_inst));
    pi->particle = a1;
    pi->mat = mat4_identity;
    pi->mat_no_scale = mat4_identity;
    pi->sub.effect_inst = a3;
    pi->sub.particle = a1;
    pi->sub.data = a1->data.data;

    if (pi->sub.data.type != GLITTER_PARTICLE_LOCUS) {
        render_group = glitter_particle_inst_init_render_group(&a2->sub, pi);
        render_group->alpha = glitter_effect_inst_get_alpha(a3);
        render_group->fog = glitter_effect_inst_get_fog(a3);
        if (pi->sub.data.blend_mode0 == GLITTER_PARTICLE_BLEND_PUNCH_THROUGH && pi->sub.data.type == GLITTER_PARTICLE_QUAD)
            render_group->alpha = 0;

        if (pi->sub.data.emission >= 0.0099999998f)
            render_group->emission = pi->sub.data.emission;
        else if (a3->data.emission >= 0.0099999998f)
            render_group->emission = a3->data.emission;
        else
            render_group->emission = a2->emission;
        pi->sub.render_group = render_group;
    }
    else
        pi->sub.flags |= GLITTER_PARTICLE_INST_FLAG_NO_CHILD;
    return pi;
}

void FASTCALL glitter_particle_inst_copy(glitter_particle_inst* a1, glitter_particle_inst* a2, glitter_scene* a3) {
    glitter_particle_inst* particle;
    glitter_particle_inst** i;

    a2->mat = a1->mat;
    a2->mat_no_scale = a1->mat_no_scale;
    a2->sub.flags = a1->sub.flags;
    if (a1->sub.render_group && a2->sub.render_group)
        glitter_render_group_copy(a1->sub.render_group, a2->sub.render_group);

    vector_ptr_glitter_particle_inst_clear(&a2->sub.particle_insts, (void*)glitter_particle_inst_dispose);
    for (i = a1->sub.particle_insts.begin; i != a1->sub.particle_insts.end; i++) {
        particle = glitter_particle_inst_init_child(a3, a1);
        vector_ptr_glitter_particle_inst_push_back(&a2->sub.particle_insts, &particle);

        particle->mat = a1->mat;
        particle->mat_no_scale = a1->mat_no_scale;
        glitter_particle_inst_copy(*i, particle, a3);
    }
}

void FASTCALL glitter_particle_inst_emit(glitter_particle_inst* a1,
    glitter_scene* a2, glitter_emitter_inst* a3, int32_t a4, int32_t count) {
    glitter_particle_inst* particle_inst;

    if (a1->sub.flags & GLITTER_PARTICLE_INST_FLAG_ENDED)
        return;

    while (!a1->sub.parent && a1->sub.flags & GLITTER_PARTICLE_INST_FLAG_NO_CHILD) {
        particle_inst = glitter_particle_inst_init_child(a2, a1);
        vector_ptr_glitter_particle_inst_push_back(&a1->sub.particle_insts, &particle_inst);
        particle_inst->mat = a1->mat;
        particle_inst->mat_no_scale = a1->mat_no_scale;
        a1 = particle_inst;
        if (particle_inst->sub.flags & GLITTER_PARTICLE_INST_FLAG_ENDED)
            return;
    }

    if (a1->sub.render_group)
        glitter_render_group_emit(a1->sub.render_group, &a1->sub, a3, a4, count);
}

void FASTCALL glitter_particle_inst_free(glitter_particle_inst* a1, bool free) {
    glitter_particle_inst** i;

    a1->sub.flags |= GLITTER_PARTICLE_INST_FLAG_ENDED;
    if (free && a1->sub.render_group)
        glitter_render_group_free(a1->sub.render_group);

    for (i = a1->sub.particle_insts.begin; i != a1->sub.particle_insts.end; i++)
        glitter_particle_inst_free(*i, free);
}

bool FASTCALL glitter_particle_inst_get_value(glitter_particle_inst* a1,
    glitter_render_group_sub* a2, float_t frame) {
    vec3 translation;
    bool v4; // r13
    int64_t length; // rbp
    glitter_curve* curve; // rdi
    float_t value; // [rsp+40h] [rbp-39h] BYREF
    bool visible;

    visible = true;
    value = 0.0f;
    v4 = false;
    translation = vec3_null;
    length = a1->particle->curve.end - a1->particle->curve.begin;
    if (length) {
        for (int32_t i = 0; i < length; i++) {
            curve = a1->particle->curve.begin[i];
            if (!glitter_curve_get_value(curve, frame, &value, a2->random + i))
                continue;

            switch (curve->type) {
            case GLITTER_CURVE_TRANSLATION_X:
                translation.x = value;
                v4 = true;
                break;
            case GLITTER_CURVE_TRANSLATION_Y:
                translation.y = value;
                v4 = true;
                break;
            case GLITTER_CURVE_TRANSLATION_Z:
                translation.z = value;
                v4 = true;
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
                if (value < 0.0099999998f)
                    visible = false;
                break;
            case GLITTER_CURVE_U_SCROLL:
                a2->uv_scroll.x = value;
                break;
            case GLITTER_CURVE_V_SCROLL:
                a2->uv_scroll.y = value;
                break;
            }
        }
    }

    if (a1->sub.data.flags & GLITTER_PARTICLE_FLAG_TRANSLATE_BY_EMITTER || v4) {
        vec3_add(translation, a2->base_translation, translation);
        if (a1->sub.data.flags & GLITTER_PARTICLE_FLAG_TRANSLATE_BY_EMITTER) {
            mat4_mult_vec3(&a2->mat, &translation, &translation);
            vec3_add(translation, *(vec3*)&a2->mat.row3, translation);
        }
        a2->translation = translation;
    }
    return visible;
}

bool FASTCALL glitter_particle_inst_has_ended(glitter_particle_inst* particle, bool a2) {
    glitter_particle_inst** i;

    if (~particle->sub.flags & GLITTER_PARTICLE_INST_FLAG_ENDED)
        return false;
    else if (!a2)
        return true;

    if (~particle->sub.flags & GLITTER_PARTICLE_INST_FLAG_NO_CHILD || particle->sub.parent) {
        if (particle->sub.render_group && particle->sub.render_group->ctrl > 0)
            return false;
        return true;
    }

    for (i = particle->sub.particle_insts.begin; i != particle->sub.particle_insts.end; i++)
        if (!glitter_particle_inst_has_ended(*i, a2))
            return false;
    return true;
}

glitter_particle_inst* FASTCALL glitter_particle_inst_init_child(glitter_scene* a1, glitter_particle_inst* a2) {
    glitter_render_group* render_group;
    glitter_effect_inst* effect;

    glitter_particle_inst* pi = force_malloc(sizeof(glitter_particle_inst));
    pi->particle = a2->particle;
    pi->mat = mat4_identity;
    pi->mat_no_scale = mat4_identity;
    pi->sub.parent = a2;
    vector_ptr_glitter_particle_inst_resize(&pi->sub.particle_insts, 0x200);

    pi->sub.data = a2->sub.data;

    pi->sub.particle = a2->sub.particle;
    render_group = glitter_particle_inst_init_render_group(&a1->sub, pi);
    effect = a2->sub.effect_inst;
    render_group->alpha = glitter_effect_inst_get_alpha(effect);
    render_group->fog = glitter_effect_inst_get_fog(effect);
    if (pi->sub.data.blend_mode0 == GLITTER_PARTICLE_BLEND_PUNCH_THROUGH && pi->sub.data.type == GLITTER_PARTICLE_QUAD)
        render_group->alpha = 0;
    if (effect->data.emission >= 0.0099999998f)
        render_group->emission = effect->data.emission;
    else
        render_group->emission = a1->emission;
    pi->sub.render_group = render_group;

    if (pi->sub.data.type == GLITTER_PARTICLE_LOCUS)
        pi->sub.flags |= GLITTER_PARTICLE_INST_FLAG_NO_CHILD;
    return pi;
}

glitter_render_group* FASTCALL glitter_particle_inst_init_render_group(glitter_scene_sub* a1,
    glitter_particle_inst* a2) {
    int32_t count;
    glitter_render_group* render_group;

    count = a2->sub.data.count;
    if (count < 1) {
        count = 250;
        if (a2->sub.data.type == GLITTER_PARTICLE_LOCUS)
            count = 30;
    }

    render_group = glitter_render_group_init(count, a2);
    vector_ptr_glitter_render_group_push_back(&a1->render_groups, &render_group);
    return render_group;
}

void FASTCALL glitter_particle_inst_dispose(glitter_particle_inst* pi) {
    if (pi->sub.render_group) {
        glitter_render_group_delete_buffers(pi->sub.render_group, true);
        pi->sub.render_group = 0;
    }

    vector_ptr_glitter_particle_inst_free(&pi->sub.particle_insts, (void*)glitter_particle_inst_dispose);
    free(pi);
}

void FASTCALL glitter_particle_inst_render_group_init(glitter_particle_inst* particle, float_t delta_frame) {
    glitter_particle_inst** i;

    if (~particle->sub.flags & GLITTER_PARTICLE_INST_FLAG_NO_CHILD || particle->sub.parent) {
        if (particle->sub.render_group)
            glitter_render_group_copy_from_particle(particle->sub.render_group, delta_frame, false);
    }
    else
        for (i = particle->sub.particle_insts.begin; i != particle->sub.particle_insts.end; i++)
            glitter_particle_inst_render_group_init(*i, delta_frame);
}

void FASTCALL glitter_particle_inst_reset(glitter_particle_inst* a1) {
    glitter_particle_inst** i; // rbx

    a1->sub.flags = 0;
    if (a1->sub.render_group)
        glitter_render_group_free(a1->sub.render_group);

    for (i = a1->sub.particle_insts.begin; i != a1->sub.particle_insts.end; ++i)
        glitter_particle_inst_reset(*i);
}

void FASTCALL glitter_particle_inst_set_mat(glitter_particle_inst* a1) {
    glitter_particle_inst** i;

    for (i = a1->sub.particle_insts.begin; i != a1->sub.particle_insts.end; i++) {
        (*i)->mat = a1->mat;
        (*i)->mat_no_scale = a1->mat_no_scale;
    }
}
