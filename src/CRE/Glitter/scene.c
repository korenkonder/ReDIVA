/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "scene.h"
#include "effect_inst.h"
#include "../GlitterX/effect_inst_x.h"

glitter_scene* FASTCALL glitter_scene_init(glitter_effect_group* eg) {
    glitter_scene* s = force_malloc(sizeof(glitter_scene));
    s->emission = 1.0f;
    s->effect_group = eg;
    s->hash = eg->hash;
    s->type = eg->type;
    if (eg)
        s->emission = eg->emission;
    s->delta_frame_history = 0.0f;
    s->skip = false;
    return s;
}

size_t FASTCALL glitter_scene_get_ctrl_count(glitter_scene* scene, glitter_particle_type ptcl_type) {
    size_t ctrl;
    glitter_scene_effect* j;

    ctrl = 0;
    for (j = scene->effects.begin; j != scene->effects.end; j++)
        if (j->ptr && j->draw)
            ctrl += glitter_effect_inst_get_ctrl_count(j->ptr, ptcl_type);
    return ctrl;
}

size_t FASTCALL glitter_scene_get_disp_count(glitter_scene* scene, glitter_particle_type ptcl_type) {
    size_t disp;
    glitter_scene_effect* j;

    disp = 0;
    for (j = scene->effects.begin; j != scene->effects.end; j++)
        if (j->ptr && j->draw)
            disp += glitter_effect_inst_get_disp_count(j->ptr, ptcl_type);
    return disp;
}

void glitter_scene_get_frame(glitter_scene* scene, float_t* frame, int32_t* life_time) {
    glitter_scene_effect* j;
    glitter_effect_inst* effect;

    for (j = scene->effects.begin; j != scene->effects.end; j++)
        if (j->ptr && j->draw) {
            effect = j->ptr;
            if (effect && frame && life_time && effect->data.life_time > *life_time) {
                if (*frame < effect->frame0)
                    *frame = effect->frame0;
                *life_time = effect->data.life_time;
            }
        }
}

void glitter_scene_get_start_end_frame(glitter_scene* scene, int32_t* start_frame, int32_t* end_frame) {
    glitter_scene_effect* i;
    glitter_effect_inst* effect;
    glitter_emitter_inst** j;
    glitter_emitter_inst* emitter;
    int32_t life_time;

    for (i = scene->effects.begin; i != scene->effects.end; i++) {
        if (!i->ptr || !i->draw)
            continue;

        effect = i->ptr;
        life_time = effect->data.life_time;
        if (start_frame && effect->data.appear_time < *start_frame)
            *start_frame = effect->data.appear_time;

        for (j = effect->emitters.begin; j != effect->emitters.end; j++) {
            if (!*j)
                continue;

            emitter = *j;
            if (life_time < emitter->data.life_time)
                life_time = emitter->data.life_time;
        }

        life_time += effect->data.appear_time;

        if (end_frame && life_time > *end_frame)
            *end_frame = life_time;
    }
}

bool FASTCALL glitter_scene_has_ended(glitter_scene* scene, bool a2) {
    glitter_scene_effect* i;

    if (scene->type == GLITTER_X) {
        for (i = scene->effects.begin; i != scene->effects.end; i++)
            if (i->ptr && i->draw && !glitter_x_effect_inst_has_ended(i->ptr, a2))
                return false;
    }
    else {
        for (i = scene->effects.begin; i != scene->effects.end; i++)
            if (i->ptr && i->draw && !glitter_effect_inst_has_ended(i->ptr, a2))
                return false;
    }
    return true;
}

void FASTCALL glitter_scene_init_effect(GPM, glitter_scene* scene,
    glitter_effect* a2, glitter_random* random, size_t id, bool appear_now) {
    glitter_scene_effect* i;
    glitter_scene_effect effect;

    if (!a2)
        return;

    if (scene->type == GLITTER_X) {
        for (i = scene->effects.begin; i != scene->effects.end; i++)
            if (i->ptr && i->draw && i->ptr->id == id) {
                glitter_x_effect_inst_reset(i->ptr, scene->emission);
                return;
            }

        effect.ptr = glitter_x_effect_inst_init(GPM_VAL, a2, id, scene->emission, appear_now);
    }
    else {
        for (i = scene->effects.begin; i != scene->effects.end; i++)
            if (i->ptr && i->draw && i->ptr->id == id) {
                glitter_effect_inst_reset(GPM_VAL, scene->type, i->ptr, scene->emission);
                return;
            }

        effect.ptr = glitter_effect_inst_init(GPM_VAL, scene->type, a2, random, id, scene->emission, appear_now);
    }
    effect.draw = true;

    vector_glitter_scene_effect_push_back(&scene->effects, &effect);
}

void FASTCALL glitter_scene_update(GPM, glitter_scene* scene, float_t delta_frame) {
    glitter_scene_effect* i;

    if (scene->type == GLITTER_X) {
        for (i = scene->effects.begin; i != scene->effects.end; ++i)
            if (i->ptr && i->draw)
                glitter_x_effect_inst_update(GPM_VAL, i->ptr, delta_frame, scene->emission);
    }
    else {
        for (i = scene->effects.begin; i != scene->effects.end; ++i)
            if (i->ptr && i->draw)
                glitter_effect_inst_update(GPM_VAL, scene->type, i->ptr, delta_frame, scene->emission);
    }
}

void FASTCALL glitter_scene_dispose(glitter_scene* s) {
    for (glitter_scene_effect* i = s->effects.begin; i != s->effects.end; ++i)
        if (i->ptr && i->draw)
            glitter_effect_inst_dispose(i->ptr);
    vector_glitter_scene_effect_free(&s->effects);
    free(s);
}
