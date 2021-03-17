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
    vector_ptr_glitter_render_group_resize(&s->sub.render_groups, 0x40);
    if (eg) {
        vector_ptr_glitter_effect_inst_resize(&s->effects, eg->effects.end - eg->effects.begin);
        s->emission = eg->emission;
    }
    return s;
}

bool FASTCALL glitter_scene_copy(glitter_scene* a1, glitter_effect_inst* a2, glitter_scene* a3) {
    glitter_effect_inst** i;

    if (a1->effects.begin == a1->effects.end)
        return false;

    for (i = a1->effects.begin; i != a1->effects.end; i++)
        if (*i && (*i)->id == a2->id) {
            glitter_effect_inst_copy(*i, a2, a3);
            return true;
        }
    return false;
}

void FASTCALL glitter_scene_emit(glitter_scene* a1, float_t frame_speed) {
    glitter_effect_inst** i;

    for (i = a1->effects.begin; i != a1->effects.end; ++i)
        if (*i)
            glitter_effect_inst_emit(*i, a1, frame_speed);
}

bool FASTCALL glitter_scene_has_ended(glitter_scene* scene, bool a2) {
    glitter_effect_inst** i;

    for (i = scene->effects.begin; i != scene->effects.end; i++)
        if (*i && !glitter_effect_inst_has_ended(*i, a2))
            return false;
    return true;
}

void FASTCALL glitter_scene_init_effect(glitter_scene* a1,
    glitter_effect* a2, size_t id, bool appear_now) {
    glitter_effect_inst** i;
    glitter_effect_inst* effect;

    if (!a2)
        return;

    for (i = a1->effects.begin; i != a1->effects.end; i++)
        if (*i && (*i)->id == id) {
            glitter_effect_inst_reset(*i, a1);
            return;
        }

    effect = glitter_effect_inst_init(a2, a1, id, appear_now);
    vector_ptr_glitter_effect_inst_push_back(&a1->effects, &effect);
}

void FASTCALL glitter_scene_update_scene(glitter_scene* a1, float_t delta_frame) {
    glitter_render_group** i;
    glitter_render_group* render_group;
    glitter_scene_sub* sub;

    sub = &a1->sub;
    sub->ctrl_quad = 0;
    sub->ctrl_line = 0;
    sub->ctrl_locus = 0;
    for (i = sub->render_groups.begin; i != sub->render_groups.end; i++) {
        if (!*i)
            continue;

        render_group = *i;
        switch (render_group->type) {
        case GLITTER_PARTICLE_QUAD:
            sub->ctrl_quad += render_group->ctrl;
            break;
        case GLITTER_PARTICLE_LINE:
            sub->ctrl_line += render_group->ctrl;
            break;
        case GLITTER_PARTICLE_LOCUS:
            sub->ctrl_locus += render_group->ctrl;
            break;
        }
        glitter_render_group_copy_from_particle(render_group, delta_frame, true);
    }
}

void FASTCALL glitter_scene_update_value_frame(glitter_scene* a1, float_t delta_frame) {
    glitter_effect_inst** i;

    for (i = a1->effects.begin; i != a1->effects.end; ++i)
        if (*i)
            glitter_effect_inst_update_value_frame(*i, delta_frame);
}

void FASTCALL glitter_scene_dispose(glitter_scene* s) {
    vector_ptr_glitter_effect_inst_free(&s->effects, (void*)glitter_effect_inst_dispose);
    vector_ptr_glitter_render_group_free(&s->sub.render_groups, (void*)glitter_render_group_dispose);
    free(s);
}
