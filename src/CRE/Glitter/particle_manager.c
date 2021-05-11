/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "particle_manager.h"
#include "effect_group.h"
#include "effect_inst.h"
#include "file_reader.h"
#include "render_group.h"
#include "scene.h"
#include "../GlitterX/effect_inst_x.h"

glitter_particle_manager* FASTCALL glitter_particle_manager_init() {
    glitter_particle_manager* gpm = force_malloc(sizeof(glitter_particle_manager));
    gpm->delta_frame = 2.0f;
    gpm->emission = 1.0f;
    vector_ptr_glitter_scene_append(&gpm->scenes, 0x100);
    vector_ptr_glitter_file_reader_append(&gpm->file_readers, 0x100);
    vector_ptr_glitter_effect_group_append(&gpm->effect_groups, 0x100);
    return gpm;
}

void FASTCALL glitter_particle_manager_calc_draw(GPM,
    bool(FASTCALL* render_add_list_func)(glitter_particle_mesh*, vec4*, mat4*, mat4*)) {
    glitter_scene** i;
    glitter_scene* scene;
    glitter_effect_inst** j;

    /*if (glt_type == GLITTER_X)
        for (i = gpm->scenes.begin; i != gpm->scenes.end; i++) {
            if (!*i)
                continue;

            scene = *i;
            for (j = scene->effects.begin; j != scene->effects.end; j++)
                if (*j)
                    glitter_x_effect_inst_calc_draw(GPM_VAL, *j, render_add_list_func);
        }
    else*/
        for (i = gpm->scenes.begin; i != gpm->scenes.end; i++) {
            if (!*i)
                continue;

            scene = *i;
            for (j = scene->effects.begin; j != scene->effects.end; j++)
                if (*j)
                    glitter_effect_inst_calc_draw(GPM_VAL, *j, render_add_list_func);
        }
}

bool FASTCALL glitter_particle_manager_check_effect_group(GPM, uint64_t effect_group_hash) {
    glitter_effect_group** i;

    for (i = gpm->effect_groups.begin; i != gpm->effect_groups.end; i++) {
        if (!*i || (*i)->hash != effect_group_hash)
            continue;
        break;
    }
    return i != gpm->effect_groups.end;
}

bool FASTCALL glitter_particle_manager_check_file_reader(GPM, uint64_t effect_group_hash) {
    glitter_file_reader** i;

    for (i = gpm->file_readers.begin; i != gpm->file_readers.end; i++) {
        if (!*i || (*i)->hash != effect_group_hash)
            continue;
        break;
    }
    return i != gpm->file_readers.end;
}

bool FASTCALL glitter_particle_manager_check_scene(GPM, uint64_t effect_group_hash) {
    glitter_scene** i;

    for (i = gpm->scenes.begin; i != gpm->scenes.end; i++) {
        if (!*i || (*i)->hash != effect_group_hash)
            continue;
        break;
    }
    return i != gpm->scenes.end;
}

void FASTCALL glitter_particle_manager_draw(GPM, int32_t alpha) {
    glitter_scene** i;
    glitter_scene* scene;
    glitter_effect_inst** j;

    /*if (glt_type == GLITTER_X)
        for (i = gpm->scenes.begin; i != gpm->scenes.end; i++) {
            if (!*i)
                continue;

            scene = *i;
            for (j = scene->effects.begin; j != scene->effects.end; j++)
                if (*j)
                    glitter_x_effect_inst_draw(GPM_VAL, *j, alpha);
        }
    else*/
        for (i = gpm->scenes.begin; i != gpm->scenes.end; i++) {
            if (!*i)
                continue;

            scene = *i;
            for (j = scene->effects.begin; j != scene->effects.end; j++)
                if (*j)
                    glitter_effect_inst_draw(GPM_VAL, *j, alpha);
        }
}

bool FASTCALL glitter_particle_manager_free_effect_group(GPM, uint64_t effect_group_hash) {
    glitter_effect_group** i;
    glitter_effect_group* effect_group;

    for (i = gpm->effect_groups.begin; i != gpm->effect_groups.end;) {
        effect_group = *i;
        if (!effect_group || effect_group->hash != effect_group_hash) {
            i++;
            continue;
        }

        vector_ptr_glitter_effect_group_erase(&gpm->effect_groups,
            i - gpm->effect_groups.begin, glitter_effect_group_dispose);
        break;
    }
    return i != gpm->effect_groups.end;
}

void FASTCALL glitter_particle_manager_free_effect_groups(GPM) {
    vector_ptr_glitter_effect_group_clear(&gpm->effect_groups, glitter_effect_group_dispose);
}

bool FASTCALL glitter_particle_manager_free_scene(GPM, uint64_t effect_group_hash) {
    glitter_scene** i;
    glitter_scene* scene;

    for (i = gpm->scenes.begin; i != gpm->scenes.end;) {
        scene = *i;
        if (!scene || scene->hash != effect_group_hash) {
            i++;
            continue;
        }

        vector_ptr_glitter_scene_erase(&gpm->scenes, i - gpm->scenes.begin, glitter_scene_dispose);
        break;
    }
    return i != gpm->scenes.end;
}

void FASTCALL glitter_particle_manager_free_scenes(GPM) {
    vector_ptr_glitter_scene_clear(&gpm->scenes, glitter_scene_dispose);
}

size_t FASTCALL glitter_particle_manager_get_ctrl_count(GPM, glitter_particle_type ptcl_type) {
    size_t ctrl;
    glitter_scene** i;
    glitter_scene* scene;
    glitter_effect_inst** j;

    ctrl = 0;
    for (i = gpm->scenes.begin; i != gpm->scenes.end; i++) {
        if (!*i)
            continue;

        scene = *i;
        for (j = scene->effects.begin; j != scene->effects.end; j++)
            if (*j)
                ctrl += glitter_effect_inst_get_ctrl_count(*j, ptcl_type);
    }
    return ctrl;
}

size_t FASTCALL glitter_particle_manager_get_disp_count(GPM, glitter_particle_type ptcl_type) {
    size_t disp;
    glitter_scene** i;
    glitter_scene* scene;
    glitter_effect_inst** j;

    disp = 0;
    for (i = gpm->scenes.begin; i != gpm->scenes.end; i++) {
        if (!*i)
            continue;

        scene = *i;
        for (j = scene->effects.begin; j != scene->effects.end; j++)
            if (*j)
                disp += glitter_effect_inst_get_disp_count(*j, ptcl_type);
    }
    return disp;
}

void glitter_particle_manager_get_frame(GPM, float_t* frame, float_t* life_time) {
    glitter_scene* scene;
    glitter_effect_inst** i;
    glitter_effect_inst* effect;

    if (frame)
        *frame = 0.0f;

    if (life_time)
        *life_time = 0.0f;

    if (gpm->scenes.begin == gpm->scenes.end)
        return;

    scene = *gpm->scenes.begin;
    for (i = scene->effects.begin; i != scene->effects.end; i++) {
        effect = *i;
        if (effect && frame && life_time && (float_t)effect->data.life_time > *life_time) {
            if (*frame < effect->frame)
                *frame = effect->frame;
            *life_time = (float_t)effect->data.life_time;
        }
    }
}

void glitter_particle_manager_get_start_end_frame(GPM, float_t* start_frame, float_t* end_frame) {
    glitter_scene* scene;
    glitter_effect_inst** i;
    glitter_effect_inst* effect;
    glitter_emitter_inst** j;
    glitter_emitter_inst* emitter;
    float_t life_time;

    if (start_frame)
        *start_frame = 0.0f;

    if (end_frame)
        *end_frame = 0.0f;

    if (gpm->scenes.begin == gpm->scenes.end || !*gpm->scenes.begin)
        return;

    if (start_frame)
        *start_frame = 32767.0f;

    scene = *gpm->scenes.begin;
    for (i = scene->effects.begin; i != scene->effects.end; i++) {
        if (!*i)
            continue;

        effect = *i;
        life_time = (float_t)effect->data.life_time;
        if (start_frame && (float_t)effect->data.appear_time < *start_frame)
            *start_frame = (float_t)effect->data.appear_time;

        for (j = effect->emitters.begin; j != effect->emitters.end; j++) {
            if (!*j)
                continue;

            emitter = *j;
            if (life_time < emitter->data.life_time)
                life_time = (float_t)emitter->data.life_time;
        }

        life_time += (float_t)effect->data.appear_time;

        if (end_frame && life_time > *end_frame)
            *end_frame = life_time;
    }
}

bool FASTCALL glitter_particle_manager_load_effect(GPM, uint64_t effect_group_hash, uint64_t effect_hash) {
    glitter_effect_group** i;
    glitter_effect_group* effect_group;
    glitter_scene** j;
    glitter_scene* scene;
    glitter_effect** k;
    glitter_effect* effect;
    size_t id;

    for (i = gpm->effect_groups.begin; i != gpm->effect_groups.end; i++) {
        effect_group = *i;
        if (!effect_group || effect_group->hash != effect_group_hash)
            continue;

        for (j = gpm->scenes.begin; j != gpm->scenes.end; j++) {
            scene = *j;
            if (scene && scene->hash == effect_group_hash)
                break;
        }

        if (j == gpm->scenes.end) {
            scene = glitter_scene_init(effect_group);
            vector_ptr_glitter_scene_push_back(&gpm->scenes, &scene);
        }

        for (k = effect_group->effects.begin, id = 1; k != effect_group->effects.end; k++, id++) {
            effect = *k;
            if (effect && effect->data.name_hash == effect_hash)
                glitter_scene_init_effect(GPM_VAL, scene, effect, id, false);
        }
        break;
    }
    return i != gpm->effect_groups.end;
}

bool FASTCALL glitter_particle_manager_load_scene(GPM, uint64_t effect_group_hash) {
    glitter_effect_group** i;
    glitter_effect_group* effect_group;
    glitter_scene** j;
    glitter_scene* scene;
    glitter_effect** k;
    size_t id;

    for (i = gpm->effect_groups.begin; i != gpm->effect_groups.end; i++) {
        effect_group = *i;
        if (!effect_group || effect_group->hash != effect_group_hash)
            continue;

        for (j = gpm->scenes.begin; j != gpm->scenes.end; j++) {
            scene = *j;
            if (scene && scene->hash == effect_group_hash)
                break;
        }

        if (j == gpm->scenes.end) {
            scene = glitter_scene_init(effect_group);
            vector_ptr_glitter_scene_push_back(&gpm->scenes, &scene);
        }

        for (k = effect_group->effects.begin, id = 1; k != effect_group->effects.end; k++, id++)
            glitter_scene_init_effect(GPM_VAL, scene, *k, id, false);
        break;
    }
    return i != gpm->effect_groups.end;
}

void FASTCALL glitter_particle_manager_set_frame(GPM, glitter_effect_group* effect_group,
    glitter_scene** scene, float_t curr_frame, float_t prev_frame, uint32_t counter, bool reset) {
    if (curr_frame < prev_frame || reset) {
        for (glitter_scene** i = gpm->scenes.begin; i != gpm->scenes.end; ) {
            if (*i != *scene) {
                i++;
                continue;
            }

            vector_ptr_glitter_scene_erase(&gpm->scenes, i - gpm->scenes.begin, glitter_scene_dispose);
        }
        gpm->counter = counter;

        glitter_scene* s = glitter_scene_init(effect_group);
        vector_ptr_glitter_scene_push_back(&gpm->scenes, &s);
        glitter_effect_group* eg = effect_group;
        glitter_effect** i;
        size_t id = 1;
        for (i = eg->effects.begin, id = 1; i != eg->effects.end; i++, id++)
            if (*i)
                glitter_scene_init_effect(GPM_VAL, s, *i, id, false);
        *scene = s;
        prev_frame = 0.0f;
    }

    float_t delta = curr_frame - prev_frame;
    float_t delta_frame = 1.0f;
    for (; delta > 0.0f; delta -= delta_frame) {
        if (delta_frame > delta)
            delta_frame = delta;

        glitter_scene* s = *scene;
        if (s && !glitter_scene_has_ended(s, true))
            glitter_scene_update_value_frame(GPM_VAL, s, delta_frame);
    }
}

bool FASTCALL glitter_particle_manager_test_load_effect(GPM,
    uint64_t effect_group_hash, uint64_t effect_hash) {
    glitter_effect_group** i;
    glitter_effect_group* effect_group;
    glitter_scene** j;
    glitter_scene* scene;
    glitter_effect** k;
    glitter_effect* effect;
    size_t id;

    for (i = gpm->effect_groups.begin; i != gpm->effect_groups.end; i++) {
        effect_group = *i;
        if (!effect_group || effect_group->hash != effect_group_hash)
            continue;

        for (j = gpm->scenes.begin; j != gpm->scenes.end; j++) {
            scene = *j;
            if (scene && scene->hash == effect_group_hash)
                break;
        }

        if (j == gpm->scenes.end) {
            scene = glitter_scene_init(effect_group);
            vector_ptr_glitter_scene_push_back(&gpm->scenes, &scene);
        }

        for (k = effect_group->effects.begin, id = 1; k != effect_group->effects.end; k++, id++) {
            effect = *k;
            if (effect && effect->data.name_hash == effect_hash)
                glitter_scene_init_effect(GPM_VAL, scene, effect, id, true);
        }
        break;
    }
    return i != gpm->effect_groups.end;
}

bool FASTCALL glitter_particle_manager_test_load_scene(GPM, uint64_t effect_group_hash, bool appear_now) {
    glitter_effect_group** i;
    glitter_effect_group* effect_group;
    glitter_scene** j;
    glitter_scene* scene;
    glitter_effect** k;
    size_t id;

    for (i = gpm->effect_groups.begin; i != gpm->effect_groups.end; i++) {
        effect_group = *i;
        if (!effect_group || effect_group->hash != effect_group_hash)
            continue;

        for (j = gpm->scenes.begin; j != gpm->scenes.end; j++) {
            scene = *j;
            if (scene && scene->hash == effect_group_hash)
                break;
        }

        if (j == gpm->scenes.end) {
            scene = glitter_scene_init(effect_group);
            vector_ptr_glitter_scene_push_back(&gpm->scenes, &scene);
        }

        for (k = effect_group->effects.begin, id = 1; k != effect_group->effects.end; k++, id++)
            glitter_scene_init_effect(GPM_VAL, scene, *k, id, appear_now);
        break;
    }
    return i != gpm->effect_groups.end;
}

void FASTCALL glitter_particle_manager_update_file_reader(GPM) {
    for (glitter_file_reader** i = gpm->file_readers.begin; i != gpm->file_readers.end;) {
        if (*i) {
            glitter_file_reader* file_reader = *i;
            if (glitter_file_reader_read(GPM_VAL, file_reader)) {
                file_reader->effect_group->emission = file_reader->emission;
                if (file_reader->emission <= 0.0)
                    file_reader->effect_group->emission = gpm->emission;
                vector_ptr_glitter_effect_group_push_back(&gpm->effect_groups, &file_reader->effect_group);
            }
            else if (file_reader->effect_group)
                glitter_effect_group_dispose(file_reader->effect_group);
        }
        vector_ptr_glitter_file_reader_erase(&gpm->file_readers,
            i - gpm->file_readers.begin, glitter_file_reader_dispose);
    }
}

void FASTCALL glitter_particle_manager_update_scene(GPM) {
    gpm->delta_frame = get_frame_speed();
    for (glitter_scene** i = gpm->scenes.begin; i != gpm->scenes.end;) {
        glitter_scene* scene = *i;
        if (scene && !glitter_scene_has_ended(scene, true)) {
            glitter_scene_update_value_frame(GPM_VAL, scene, gpm->delta_frame);
            i++;
        }
        else
            vector_ptr_glitter_scene_erase(&gpm->scenes, i - gpm->scenes.begin, glitter_scene_dispose);
    }
}

void FASTCALL glitter_particle_manager_dispose(glitter_particle_manager* gpm) {
    vector_ptr_glitter_scene_free(&gpm->scenes, glitter_scene_dispose);
    vector_ptr_glitter_file_reader_free(&gpm->file_readers, glitter_file_reader_dispose);
    vector_ptr_glitter_effect_group_free(&gpm->effect_groups, glitter_effect_group_dispose);
    free(gpm);
}
