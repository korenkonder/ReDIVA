/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "scene.h"
#include "effect_inst.h"
#include "render_group.h"

glitter_scene* FASTCALL glitter_scene_init(glitter_effect_group* eg) {
    glitter_scene* s = force_malloc(sizeof(glitter_scene));
    s->emission = 1.0f;
    s->effect_group = eg;
    s->hash = eg->hash;
    vector_ptr_glitter_render_group_expand(&s->sub.render_groups, 0x40);
    if (eg) {
        vector_ptr_glitter_effect_inst_expand(&s->effects, eg->effects.end - eg->effects.begin);
        s->emission = s->effect_group->emission;
    }
    return s;
}

void FASTCALL glitter_scene_init_effect(glitter_scene* a1, glitter_effect* a2, int32_t id, bool a4) {
    glitter_effect_inst** i;
    glitter_effect_inst* effect;

    if (!a2)
        return;

    for (i = a1->effects.begin; i != a1->effects.end; i++)
        if (*i && (*i)->id == id) {
            Glitter__EffectInst__Reset(*i, a1);
            return;
        }

    effect = glitter_effect_inst_init(a2, a1, id, a4);
    vector_ptr_glitter_effect_inst_append_element(&a1->effects, &effect);
}

void FASTCALL glitter_scene_dispose(glitter_scene* s) {
    vector_ptr_glitter_effect_inst_clear(&s->effects, (void*)&glitter_effect_inst_dispose);
    vector_ptr_glitter_effect_inst_dispose(&s->effects);
    vector_ptr_glitter_render_group_clear(&s->sub.render_groups, (void*)&glitter_render_group_dispose);
    vector_ptr_glitter_render_group_dispose(&s->sub.render_groups);
    free(s);
}

void FASTCALL Glitter__Scene__UpdateSceneSub(glitter_scene_sub* a1, float_t delta_frame) {
    glitter_render_group** i;
    glitter_render_group* render_group;

    a1->ctrl_quad = 0;
    a1->ctrl_line = 0;
    a1->ctrl_locus = 0;
    for (i = a1->render_groups.begin; i != a1->render_groups.end; i++) {
        if (!*i)
            continue;

        render_group = *i;
        switch (render_group->type) {
        case GLITTER_PARTICLE_QUAD:
            a1->ctrl_quad += render_group->ctrl;
            break;
        case GLITTER_PARTICLE_LINE:
            a1->ctrl_line += render_group->ctrl;
            break;
        case GLITTER_PARTICLE_LOCUS:
            a1->ctrl_locus += render_group->ctrl;
            break;
        }
        Glitter__RenderGroup__CopyFromParticle(render_group, delta_frame, false);
    }
}

bool FASTCALL Glitter__Scene__Copy(glitter_scene* a1, glitter_effect_inst* a2, glitter_scene* a3) {
    glitter_effect_inst** i;

    if (a1->effects.begin == a1->effects.end)
        return false;

    for (i = a1->effects.begin; i != a1->effects.end; i++)
        if (*i && (*i)->id == a2->id) {
            Glitter__EffectInst__Copy(*i, a2, a3);
            return true;
        }
    return false;
}

bool FASTCALL Glitter__Scene__HasEnded(glitter_scene* scene, bool a2) {
    glitter_effect_inst** i;

    for (i = scene->effects.begin; i != scene->effects.end; i++)
        if (*i && !Glitter__EffectInst__HasEnded(*i, a2))
            return false;
    return true;
}

void FASTCALL Glitter__Scene__Emit(glitter_scene* a1, float_t frame_speed) {
    glitter_effect_inst** i;

    for (i = a1->effects.begin; i != a1->effects.end; ++i)
        if (*i)
            Glitter__EffectInst__Emit(*i, a1, frame_speed);
}

void FASTCALL Glitter__Scene__UpdateScene(glitter_scene* a1, float_t delta_frame) {
    Glitter__Scene__UpdateSceneSub(&a1->sub, delta_frame);
}

void FASTCALL Glitter__Scene__UpdateValueFrame(glitter_scene* a1, float_t delta_frame) {
    glitter_effect_inst** i;

    for (i = a1->effects.begin; i != a1->effects.end; ++i)
        if (*i)
            Glitter__EffectInst__UpdateValueFrame(*i, delta_frame);
}
