/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "particle_inst.h"
#include "effect_inst.h"
#include "render_group.h"
#include "render_scene.h"

static glitter_particle_inst* glitter_particle_inst_init_child(glitter_particle_inst* a1,
    float_t emission);

glitter_particle_inst* glitter_particle_inst_init(glitter_particle* a1,
    glitter_effect_inst* a2, glitter_emitter_inst* a3, glitter_random* random, float_t emission) {
    glitter_render_group* rg;

    glitter_particle_inst* pi = force_malloc_s(glitter_particle_inst, 1);
    pi->particle = a1;
    pi->data.effect = a2;
    pi->data.emitter = a3;
    pi->data.particle = a1;
    pi->data.data = a1->data;
    pi->data.random_ptr = random;

    if (pi->data.data.type != GLITTER_PARTICLE_LOCUS) {
        rg = glitter_render_group_init(pi);
        if (rg) {
            rg->alpha = glitter_effect_inst_get_alpha(a2);
            rg->fog = glitter_effect_inst_get_fog(a2);
            if (pi->data.data.type == GLITTER_PARTICLE_QUAD
                && pi->data.data.blend_mode == GLITTER_PARTICLE_BLEND_PUNCH_THROUGH)
                rg->alpha = DRAW_PASS_3D_OPAQUE;

            if (pi->data.data.draw_flags & GLITTER_PARTICLE_DRAW_NO_BILLBOARD_CULL)
                rg->use_culling = false;
            else
                rg->use_culling = true;

            if (pi->data.data.emission >= glitter_min_emission)
                rg->emission = pi->data.data.emission;
            else if (a2->data.emission >= glitter_min_emission)
                rg->emission = a2->data.emission;
            else
                rg->emission = emission;
            pi->data.render_group = rg;
            glitter_render_scene_append(&a2->render_scene, rg);
        }
    }
    else
        enum_or(pi->data.flags, GLITTER_PARTICLE_INST_NO_CHILD);
    return pi;
}

void glitter_particle_inst_emit(GPM, GLT,
    glitter_particle_inst* a1, int32_t dup_count, int32_t count, float_t emission) {
    glitter_particle_inst* particle;

    if (a1->data.flags & GLITTER_PARTICLE_INST_ENDED)
        return;

    while (!a1->data.parent && a1->data.flags & GLITTER_PARTICLE_INST_NO_CHILD) {
        particle = glitter_particle_inst_init_child(a1, emission);
        if (particle)
            vector_old_ptr_glitter_particle_inst_push_back(&a1->data.children, &particle);
        else
            return;

        a1 = particle;
        if (particle->data.flags & GLITTER_PARTICLE_INST_ENDED)
            return;
    }

    if (a1->data.render_group)
        glitter_render_group_emit(GPM_VAL, GLT_VAL,
            a1->data.render_group, &a1->data, a1->data.emitter, dup_count, count);
}

void glitter_particle_inst_free(glitter_particle_inst* a1, bool free) {
    glitter_particle_inst** i;

    enum_or(a1->data.flags, GLITTER_PARTICLE_INST_ENDED);
    if (free && a1->data.render_group)
        glitter_render_group_free(a1->data.render_group);

    for (i = a1->data.children.begin; i != a1->data.children.end; i++)
        glitter_particle_inst_free(*i, free);
}

bool glitter_particle_inst_has_ended(glitter_particle_inst* particle, bool a2) {
    glitter_particle_inst** i;

    if (~particle->data.flags & GLITTER_PARTICLE_INST_ENDED)
        return false;
    else if (!a2)
        return true;

    if (~particle->data.flags & GLITTER_PARTICLE_INST_NO_CHILD || particle->data.parent) {
        if (particle->data.render_group && particle->data.render_group->ctrl > 0)
            return false;
        return true;
    }

    for (i = particle->data.children.begin; i != particle->data.children.end; i++)
        if (!glitter_particle_inst_has_ended(*i, a2))
            return false;
    return true;
}

void glitter_particle_inst_dispose(glitter_particle_inst* pi) {
    if (pi->data.render_group) {
        glitter_render_group_delete_buffers(pi->data.render_group, true);
        pi->data.render_group = 0;
    }

    vector_old_ptr_glitter_particle_inst_free(&pi->data.children, glitter_particle_inst_dispose);
    free(pi);
}

void glitter_particle_inst_reset(glitter_particle_inst* a1) {
    glitter_particle_inst** i;

    a1->data.flags = (glitter_particle_inst_flag)0;
    if (a1->data.render_group)
        glitter_render_group_free(a1->data.render_group);

    for (i = a1->data.children.begin; i != a1->data.children.end; ++i)
        glitter_particle_inst_reset(*i);
}

static glitter_particle_inst* glitter_particle_inst_init_child(glitter_particle_inst* a1,
    float_t emission) {
    glitter_render_group* rg;
    glitter_effect_inst* effect;

    glitter_particle_inst* pi = force_malloc_s(glitter_particle_inst, 1);
    pi->particle = a1->particle;
    pi->data.effect = a1->data.effect;
    pi->data.emitter = a1->data.emitter;
    pi->data.parent = a1;

    pi->data.random_ptr = a1->data.random_ptr;
    pi->data.data = a1->data.data;

    pi->data.particle = a1->data.particle;
    rg = glitter_render_group_init(pi);
    if (rg) {
        effect = a1->data.effect;
        rg->alpha = glitter_effect_inst_get_alpha(effect);
        rg->fog = glitter_effect_inst_get_fog(effect);
        if (pi->data.data.blend_mode == GLITTER_PARTICLE_BLEND_PUNCH_THROUGH
            && pi->data.data.type == GLITTER_PARTICLE_QUAD)
            rg->alpha = DRAW_PASS_3D_OPAQUE;
        if (effect->data.emission >= glitter_min_emission)
            rg->emission = effect->data.emission;
        else
            rg->emission = emission;
        pi->data.render_group = rg;
        glitter_render_scene_append(&a1->data.effect->render_scene, rg);
    }

    if (pi->data.data.type == GLITTER_PARTICLE_LOCUS)
        enum_or(pi->data.flags, GLITTER_PARTICLE_INST_NO_CHILD);
    return pi;
}
