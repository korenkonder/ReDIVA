/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "scene.h"
#include "../GlitterX/effect_inst_x.h"
#include "effect_inst.h"

glitter_scene::glitter_scene(glitter_scene_counter counter, uint64_t hash, glitter_effect_group* a4, bool a5) : counter(0) {
    effects = {};
    this->counter = counter;
    this->hash = hash;
    flags = GLITTER_SCENE_NONE;
    emission = 1.0f;
    type = a4->type;
    effect_group = a4;
    delta_frame_history = 0.0f;
    skip = false;
    if (a4) {
        vector_old_glitter_scene_effect_reserve(&effects, vector_old_length(a4->effects));
        emission = a4->emission;
        if (a5)
            enum_or(flags, GLITTER_SCENE_FLAG_4);
    }
}

glitter_scene::~glitter_scene() {
    if (type == GLITTER_X) {
        for (glitter_scene_effect* i = effects.begin; i != effects.end; ++i)
            if (i->ptr)
                glitter_x_effect_inst_dispose(i->ptr);
    }
    else {
        for (glitter_scene_effect* i = effects.begin; i != effects.end; ++i)
            if (i->ptr)
                glitter_effect_inst_dispose(i->ptr);
    }
    vector_old_glitter_scene_effect_free(&effects, 0);
}

#if defined(CRE_DEV)
void glitter_scene_calc_disp(GPM, glitter_scene* scene) {
    if (scene->type == GLITTER_X) {
        for (glitter_scene_effect* i = scene->effects.begin; i != scene->effects.end; i++)
            if (i->ptr && i->disp) {
                if (GPM_VAL->draw_selected && GPM_VAL->effect && GPM_VAL->effect != i->ptr)
                    continue;
                glitter_x_effect_inst_calc_disp(GPM_VAL, i->ptr);
            }
    }
}
#endif

void glitter_scene_ctrl(GPM, glitter_scene* scene, float_t delta_frame) {
    glitter_scene_effect* i;

    if (scene->type == GLITTER_X) {
        for (i = scene->effects.begin; i != scene->effects.end; ++i)
            if (i->ptr && i->disp)
                glitter_x_effect_inst_ctrl(GPM_VAL, i->ptr, delta_frame, scene->emission);
    }
    else {
        for (i = scene->effects.begin; i != scene->effects.end; ++i)
            if (i->ptr && i->disp)
                glitter_effect_inst_ctrl(GPM_VAL, scene->type, i->ptr, delta_frame, scene->emission);
    }
}

void glitter_scene_disp(GPM, glitter_scene* scene, draw_pass_3d_type alpha) {
    if (scene->type == GLITTER_X) {
        for (glitter_scene_effect* i = scene->effects.begin; i != scene->effects.end; i++)
            if (i->ptr && i->disp) {
#if defined(CRE_DEV)
                if (GPM_VAL->draw_selected && GPM_VAL->effect && GPM_VAL->effect != i->ptr)
                    continue;
#endif
                glitter_x_effect_inst_disp(GPM_VAL, i->ptr, alpha);
            }
    }
    else {
        for (glitter_scene_effect* i = scene->effects.begin; i != scene->effects.end; i++)
            if (i->ptr && i->disp) {
#if defined(CRE_DEV)
                if (GPM_VAL->draw_selected && GPM_VAL->effect && GPM_VAL->effect != i->ptr)
                    continue;
#endif
                glitter_effect_inst_disp(GPM_VAL, i->ptr, alpha);
            }
    }
}

size_t glitter_scene_get_ctrl_count(glitter_scene* scene, glitter_particle_type ptcl_type) {
    size_t ctrl;
    glitter_scene_effect* j;

    ctrl = 0;
    for (j = scene->effects.begin; j != scene->effects.end; j++)
        if (j->ptr && j->disp)
            ctrl += glitter_effect_inst_get_ctrl_count(j->ptr, ptcl_type);
    return ctrl;
}

size_t glitter_scene_get_disp_count(glitter_scene* scene, glitter_particle_type ptcl_type) {
    size_t disp;
    glitter_scene_effect* j;

    disp = 0;
    for (j = scene->effects.begin; j != scene->effects.end; j++)
        if (j->ptr && j->disp)
            disp += glitter_effect_inst_get_disp_count(j->ptr, ptcl_type);
    return disp;
}

void glitter_scene_get_frame(glitter_scene* scene, float_t* frame, int32_t* life_time) {
    glitter_scene_effect* j;
    glitter_effect_inst* effect;

    for (j = scene->effects.begin; j != scene->effects.end; j++)
        if (j->ptr && j->disp) {
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
        if (!i->ptr || !i->disp)
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

bool glitter_scene_free_effect(GPM, glitter_scene* scene, uint64_t effect_hash, bool free) {
    glitter_type type = scene->type;
    float_t emission = scene->emission;
    if (type == GLITTER_X) {
        if (effect_hash == hash_murmurhash_empty) {
            for (glitter_scene_effect* i = scene->effects.begin; i != scene->effects.end; i++)
                if (i->disp && i->ptr)
                    glitter_x_effect_inst_free(i->ptr, emission, free);
            return true;
        }
        else if (vector_old_length(scene->effects) > 0)
            for (glitter_scene_effect* i = scene->effects.begin; i != scene->effects.end; i++)
                if (i->disp && i->ptr && i->ptr->data.name_hash == effect_hash) {
                    glitter_x_effect_inst_free(i->ptr, emission, free);
                    return true;
                }
    }
    else {
        if (type == GLITTER_FT && effect_hash == hash_fnv1a64m_empty
            || type != GLITTER_FT && effect_hash == hash_murmurhash_empty) {
            for (glitter_scene_effect* i = scene->effects.begin; i != scene->effects.end; i++)
                if (i->disp && i->ptr)
                    glitter_effect_inst_free(GPM_VAL, type, i->ptr, emission, free);
            return true;
        }
        else if (vector_old_length(scene->effects) > 0)
            for (glitter_scene_effect* i = scene->effects.begin; i != scene->effects.end; i++)
                if (i->disp && i->ptr && i->ptr->data.name_hash == effect_hash) {
                    glitter_effect_inst_free(GPM_VAL, type, i->ptr, emission, free);
                    return true;
                }
    }
    return false;
}

bool glitter_scene_has_ended(glitter_scene* scene, bool a2) {
    if (scene->type == GLITTER_X) {
        for (glitter_scene_effect* i = scene->effects.begin; i != scene->effects.end; i++)
            if (i->ptr && i->disp && !glitter_x_effect_inst_has_ended(i->ptr, a2))
                return false;
    }
    else {
        for (glitter_scene_effect* i = scene->effects.begin; i != scene->effects.end; i++)
            if (i->ptr && i->disp && !glitter_effect_inst_has_ended(i->ptr, a2))
                return false;
    }
    return true;
}

void glitter_scene_init_effect(GPM, glitter_scene* scene,
    glitter_effect* a2, size_t id, bool appear_now) {
    if (!a2)
        return;

    glitter_type type = scene->type;
    float_t emission = scene->emission;

    glitter_scene_effect effect;
    if (type == GLITTER_X) {
        for (glitter_scene_effect* i = scene->effects.begin; i != scene->effects.end; i++)
            if (i->ptr && i->disp && i->ptr->id == id) {
                glitter_x_effect_inst_reset(i->ptr, emission);
                return;
            }

        effect.ptr = glitter_x_effect_inst_init(GPM_VAL, a2, id, emission, appear_now);
    }
    else {
        for (glitter_scene_effect* i = scene->effects.begin; i != scene->effects.end; i++)
            if (i->ptr && i->disp && i->ptr->id == id) {
                glitter_effect_inst_reset(GPM_VAL, type, i->ptr, emission);
                return;
            }

        effect.ptr = glitter_effect_inst_init(GPM_VAL, type, a2, id, emission, appear_now);
    }
    effect.disp = true;

    vector_old_glitter_scene_effect_push_back(&scene->effects, &effect);
}

bool glitter_scene_reset_effect(GPM, glitter_scene* scene, uint64_t effect_hash) {
    glitter_type type = scene->type;
    float_t emission = scene->emission;
    if (type == GLITTER_X) {
        if (effect_hash == hash_murmurhash_empty) {
            for (glitter_scene_effect* i = scene->effects.begin; i != scene->effects.end; i++)
                if (i->disp && i->ptr)
                    glitter_x_effect_inst_reset(i->ptr, emission);
            return true;
        }
        else if (vector_old_length(scene->effects) > 0)
            for (glitter_scene_effect* i = scene->effects.begin; i != scene->effects.end; i++)
                if (i->disp && i->ptr && i->ptr->data.name_hash == effect_hash) {
                    glitter_x_effect_inst_reset(i->ptr, emission);
                    return true;
                }
    }
    else {
        if (type == GLITTER_FT && effect_hash == hash_fnv1a64m_empty
            || type != GLITTER_FT && effect_hash == hash_murmurhash_empty) {
            for (glitter_scene_effect* i = scene->effects.begin; i != scene->effects.end; i++)
                if (i->disp && i->ptr)
                    glitter_effect_inst_reset(GPM_VAL, type, i->ptr, emission);
            return true;
        }
        else if (vector_old_length(scene->effects) > 0)
            for (glitter_scene_effect* i = scene->effects.begin; i != scene->effects.end; i++)
                if (i->disp && i->ptr && i->ptr->data.name_hash == effect_hash) {
                    glitter_effect_inst_reset(GPM_VAL, type, i->ptr, emission);
                    return true;
                }
    }
    return false;
}

glitter_scene_counter::glitter_scene_counter(uint32_t counter) {
    this->index = 0;
    this->counter = counter;
}

glitter_scene_counter::glitter_scene_counter(uint32_t index, uint32_t counter) {
    this->index = index;
    this->counter = counter;
}
