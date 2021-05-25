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
    gpm->random.value = 0;
    gpm->random.step = 1;
    vector_ptr_glitter_scene_append(&GPM_VAL->scenes, 0x100);
    vector_ptr_glitter_file_reader_append(&GPM_VAL->file_readers, 0x100);
    vector_ptr_glitter_effect_group_append(&GPM_VAL->effect_groups, 0x100);
    return gpm;
}

void FASTCALL glitter_particle_manager_calc_draw(GPM,
    bool(FASTCALL* render_add_list_func)(glitter_particle_mesh*, vec4*, mat4*, mat4*)) {
    glitter_scene** i;
    glitter_scene* scene;
    glitter_scene_effect* j;

    for (i = GPM_VAL->scenes.begin; i != GPM_VAL->scenes.end; i++) {
        if (!*i)
            continue;

        scene = *i;
        if (scene->type == GLITTER_X) {
            for (j = scene->effects.begin; j != scene->effects.end; j++)
                if (j->ptr && j->draw)
                    glitter_x_effect_inst_calc_draw(GPM_VAL, j->ptr, render_add_list_func);
        }
        else {
            for (j = scene->effects.begin; j != scene->effects.end; j++)
                if (j->ptr && j->draw)
                    glitter_effect_inst_calc_draw(GPM_VAL, j->ptr);
        }
    }
}

bool FASTCALL glitter_particle_manager_check_effect_group(GPM, uint64_t effect_group_hash) {
    glitter_effect_group** i;

    for (i = GPM_VAL->effect_groups.begin; i != GPM_VAL->effect_groups.end; i++) {
        if (!*i || (*i)->hash != effect_group_hash)
            continue;
        break;
    }
    return i != GPM_VAL->effect_groups.end;
}

bool FASTCALL glitter_particle_manager_check_file_reader(GPM, uint64_t effect_group_hash) {
    glitter_file_reader** i;

    for (i = GPM_VAL->file_readers.begin; i != GPM_VAL->file_readers.end; i++) {
        if (!*i || (*i)->hash != effect_group_hash)
            continue;
        break;
    }
    return i != GPM_VAL->file_readers.end;
}

bool FASTCALL glitter_particle_manager_check_scene(GPM, uint64_t effect_group_hash) {
    glitter_scene** i;

    for (i = GPM_VAL->scenes.begin; i != GPM_VAL->scenes.end; i++) {
        if (!*i || (*i)->hash != effect_group_hash)
            continue;
        break;
    }
    return i != GPM_VAL->scenes.end;
}

void FASTCALL glitter_particle_manager_draw(GPM, int32_t alpha) {
    glitter_scene** i;
    glitter_scene* scene;
    glitter_scene_effect* j;

    for (i = GPM_VAL->scenes.begin; i != GPM_VAL->scenes.end; i++) {
        if (!*i)
            continue;

        scene = *i;
        if (scene->type == GLITTER_X) {
            for (j = scene->effects.begin; j != scene->effects.end; j++)
                if (j->ptr && j->draw)
                    glitter_x_effect_inst_draw(GPM_VAL, j->ptr, alpha);
        }
        else {
            for (j = scene->effects.begin; j != scene->effects.end; j++)
                if (j->ptr && j->draw)
                    glitter_effect_inst_draw(GPM_VAL, j->ptr, alpha);
        }
    }
}

bool FASTCALL glitter_particle_manager_free_effect_group(GPM, uint64_t effect_group_hash) {
    glitter_effect_group** i;
    glitter_effect_group* effect_group;

    for (i = GPM_VAL->effect_groups.begin; i != GPM_VAL->effect_groups.end;) {
        effect_group = *i;
        if (!effect_group || effect_group->hash != effect_group_hash) {
            i++;
            continue;
        }

        vector_ptr_glitter_effect_group_erase(&GPM_VAL->effect_groups,
            i - GPM_VAL->effect_groups.begin, glitter_effect_group_dispose);
        break;
    }
    return i != GPM_VAL->effect_groups.end;
}

void FASTCALL glitter_particle_manager_free_effect_groups(GPM) {
    vector_ptr_glitter_effect_group_clear(&GPM_VAL->effect_groups, glitter_effect_group_dispose);
}

bool FASTCALL glitter_particle_manager_free_scene(GPM, uint64_t effect_group_hash) {
    glitter_scene** i;
    glitter_scene* scene;

    for (i = GPM_VAL->scenes.begin; i != GPM_VAL->scenes.end;) {
        scene = *i;
        if (!scene || scene->hash != effect_group_hash) {
            i++;
            continue;
        }

        vector_ptr_glitter_scene_erase(&GPM_VAL->scenes, i - GPM_VAL->scenes.begin, glitter_scene_dispose);
        break;
    }
    return i != GPM_VAL->scenes.end;
}

void FASTCALL glitter_particle_manager_free_scenes(GPM) {
    vector_ptr_glitter_scene_clear(&GPM_VAL->scenes, glitter_scene_dispose);
}

size_t FASTCALL glitter_particle_manager_get_ctrl_count(GPM, glitter_particle_type ptcl_type) {
    size_t ctrl;
    glitter_scene** i;

    ctrl = 0;
    for (i = GPM_VAL->scenes.begin; i != GPM_VAL->scenes.end; i++)
        if (*i)
            ctrl += glitter_scene_get_ctrl_count(*i, ptcl_type);
    return ctrl;
}

size_t FASTCALL glitter_particle_manager_get_disp_count(GPM, glitter_particle_type ptcl_type) {
    size_t disp;
    glitter_scene** i;

    disp = 0;
    for (i = GPM_VAL->scenes.begin; i != GPM_VAL->scenes.end; i++)
        if (*i)
            disp += glitter_scene_get_disp_count(*i, ptcl_type);
    return disp;
}

void glitter_particle_manager_get_frame(GPM, float_t* frame, float_t* life_time) {
    if (frame)
        *frame = 0.0f;

    if (life_time)
        *life_time = 0.0f;

    if (GPM_VAL->scenes.begin == GPM_VAL->scenes.end)
        return;

    glitter_scene_get_frame(*GPM_VAL->scenes.begin, frame, life_time);
}

void glitter_particle_manager_get_start_end_frame(GPM, float_t* start_frame, float_t* end_frame) {
    if (start_frame)
        *start_frame = 0.0f;

    if (end_frame)
        *end_frame = 0.0f;

    if (GPM_VAL->scenes.begin == GPM_VAL->scenes.end || !*GPM_VAL->scenes.begin)
        return;

    if (start_frame)
        *start_frame = 32767.0f;

    glitter_scene_get_start_end_frame(*GPM_VAL->scenes.begin, start_frame, end_frame);
}

bool FASTCALL glitter_particle_manager_load_effect(GPM, uint64_t effect_group_hash, uint64_t effect_hash) {
    glitter_effect_group** i;
    glitter_effect_group* effect_group;
    glitter_scene** j;
    glitter_scene* scene;
    glitter_effect** k;
    glitter_effect* effect;
    size_t id;

    for (i = GPM_VAL->effect_groups.begin; i != GPM_VAL->effect_groups.end; i++) {
        effect_group = *i;
        if (!effect_group || effect_group->hash != effect_group_hash)
            continue;

        for (j = GPM_VAL->scenes.begin; j != GPM_VAL->scenes.end; j++) {
            scene = *j;
            if (scene && scene->hash == effect_group_hash)
                break;
        }

        if (j == GPM_VAL->scenes.end) {
            scene = glitter_scene_init(effect_group);
            vector_ptr_glitter_scene_push_back(&GPM_VAL->scenes, &scene);
        }

        for (k = effect_group->effects.begin, id = 1; k != effect_group->effects.end; k++, id++) {
            effect = *k;
            if (effect && effect->data.name_hash == effect_hash)
                glitter_scene_init_effect(GPM_VAL, scene, effect, &GPM_VAL->random, id, false);
        }
        break;
    }
    return i != GPM_VAL->effect_groups.end;
}

bool FASTCALL glitter_particle_manager_load_scene(GPM, uint64_t effect_group_hash) {
    glitter_effect_group** i;
    glitter_effect_group* effect_group;
    glitter_scene** j;
    glitter_scene* scene;
    glitter_effect** k;
    size_t id;

    for (i = GPM_VAL->effect_groups.begin; i != GPM_VAL->effect_groups.end; i++) {
        effect_group = *i;
        if (!effect_group || effect_group->hash != effect_group_hash)
            continue;

        for (j = GPM_VAL->scenes.begin; j != GPM_VAL->scenes.end; j++) {
            scene = *j;
            if (scene && scene->hash == effect_group_hash)
                break;
        }

        if (j == GPM_VAL->scenes.end) {
            scene = glitter_scene_init(effect_group);
            vector_ptr_glitter_scene_push_back(&GPM_VAL->scenes, &scene);
        }

        for (k = effect_group->effects.begin, id = 1; k != effect_group->effects.end; k++, id++)
            glitter_scene_init_effect(GPM_VAL, scene, *k, &GPM_VAL->random, id, false);
        break;
    }
    return i != GPM_VAL->effect_groups.end;
}

void FASTCALL glitter_particle_manager_set_frame(GPM,
    glitter_effect_group* effect_group, glitter_scene** scene, float_t curr_frame,
    float_t prev_frame, uint32_t counter, glitter_random* random, bool reset) {
    if (curr_frame < prev_frame || reset) {
        for (glitter_scene** i = GPM_VAL->scenes.begin; i != GPM_VAL->scenes.end; ) {
            if (*i != *scene) {
                i++;
                continue;
            }

            vector_ptr_glitter_scene_erase(&GPM_VAL->scenes, i - GPM_VAL->scenes.begin, glitter_scene_dispose);
        }
        GPM_VAL->counter = counter;
        GPM_VAL->random = *random;

        glitter_scene* s = glitter_scene_init(effect_group);
        vector_ptr_glitter_scene_push_back(&GPM_VAL->scenes, &s);
        glitter_effect_group* eg = effect_group;
        glitter_effect** i;
        size_t id = 1;
        for (i = eg->effects.begin, id = 1; i != eg->effects.end; i++, id++)
            if (*i)
                glitter_scene_init_effect(GPM_VAL, s, *i, &GPM_VAL->random, id, false);
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
            if (s->type == GLITTER_F2) {
                s->delta_frame_history += delta_frame;
                if (!s->skip) {
                    glitter_scene_update(GPM_VAL, s, s->delta_frame_history);
                    s->delta_frame_history = 0.0f;
                    s->skip = true;
                }
                else
                    s->skip = false;
            }
            else
                glitter_scene_update(GPM_VAL, s, delta_frame);
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

    for (i = GPM_VAL->effect_groups.begin; i != GPM_VAL->effect_groups.end; i++) {
        effect_group = *i;
        if (!effect_group || effect_group->hash != effect_group_hash)
            continue;

        for (j = GPM_VAL->scenes.begin; j != GPM_VAL->scenes.end; j++) {
            scene = *j;
            if (scene && scene->hash == effect_group_hash)
                break;
        }

        if (j == GPM_VAL->scenes.end) {
            scene = glitter_scene_init(effect_group);
            vector_ptr_glitter_scene_push_back(&GPM_VAL->scenes, &scene);
        }

        for (k = effect_group->effects.begin, id = 1; k != effect_group->effects.end; k++, id++) {
            effect = *k;
            if (effect && effect->data.name_hash == effect_hash)
                glitter_scene_init_effect(GPM_VAL, scene, effect, &GPM_VAL->random, id, true);
        }
        break;
    }
    return i != GPM_VAL->effect_groups.end;
}

bool FASTCALL glitter_particle_manager_test_load_scene(GPM, uint64_t effect_group_hash, bool appear_now) {
    glitter_effect_group** i;
    glitter_effect_group* effect_group;
    glitter_scene** j;
    glitter_scene* scene;
    glitter_effect** k;
    size_t id;

    for (i = GPM_VAL->effect_groups.begin; i != GPM_VAL->effect_groups.end; i++) {
        effect_group = *i;
        if (!effect_group || effect_group->hash != effect_group_hash)
            continue;

        for (j = GPM_VAL->scenes.begin; j != GPM_VAL->scenes.end; j++) {
            scene = *j;
            if (scene && scene->hash == effect_group_hash)
                break;
        }

        if (j == GPM_VAL->scenes.end) {
            scene = glitter_scene_init(effect_group);
            vector_ptr_glitter_scene_push_back(&GPM_VAL->scenes, &scene);
        }

        for (k = effect_group->effects.begin, id = 1; k != effect_group->effects.end; k++, id++)
            glitter_scene_init_effect(GPM_VAL, scene, *k, &GPM_VAL->random, id, appear_now);
        break;
    }
    return i != GPM_VAL->effect_groups.end;
}

void FASTCALL glitter_particle_manager_update_file_reader(GPM) {
    for (glitter_file_reader** i = GPM_VAL->file_readers.begin; i != GPM_VAL->file_readers.end;) {
        if (*i) {
            glitter_file_reader* file_reader = *i;
            if (glitter_file_reader_read(file_reader, GPM_VAL->emission)) {
                file_reader->effect_group->emission = file_reader->emission;
                if (file_reader->emission <= 0.0)
                    file_reader->effect_group->emission = GPM_VAL->emission;
                vector_ptr_glitter_effect_group_push_back(&GPM_VAL->effect_groups, &file_reader->effect_group);
            }
            else if (file_reader->effect_group)
                glitter_effect_group_dispose(file_reader->effect_group);
        }
        vector_ptr_glitter_file_reader_erase(&GPM_VAL->file_readers,
            i - GPM_VAL->file_readers.begin, glitter_file_reader_dispose);
    }
}

void FASTCALL glitter_particle_manager_update_scene(GPM) {
    GPM_VAL->delta_frame = get_frame_speed();
    for (glitter_scene** i = GPM_VAL->scenes.begin; i != GPM_VAL->scenes.end;) {
        glitter_scene* scene = *i;
        if (scene && !glitter_scene_has_ended(scene, true)) {
            if (scene->type == GLITTER_F2) {
                scene->delta_frame_history += GPM_VAL->delta_frame;
                if (!scene->skip) {
                    glitter_scene_update(GPM_VAL, scene, scene->delta_frame_history);
                    scene->delta_frame_history = 0.0f;
                    scene->skip = true;
                }
                else
                    scene->skip = false;
            }
            else
                glitter_scene_update(GPM_VAL, scene, GPM_VAL->delta_frame);
            i++;
        }
        else
            vector_ptr_glitter_scene_erase(&GPM_VAL->scenes, i - GPM_VAL->scenes.begin, glitter_scene_dispose);
    }
}

void FASTCALL glitter_particle_manager_dispose(GPM) {
    vector_ptr_glitter_scene_free(&GPM_VAL->scenes, glitter_scene_dispose);
    vector_ptr_glitter_file_reader_free(&GPM_VAL->file_readers, glitter_file_reader_dispose);
    vector_ptr_glitter_effect_group_free(&GPM_VAL->effect_groups, glitter_effect_group_dispose);
    free(gpm);
}
