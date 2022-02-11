/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "particle_manager.h"
#include "../GlitterX/effect_inst_x.h"
#include "effect_group.h"
#include "effect_inst.h"
#include "file_reader.h"
#include "render_group.h"
#include "scene.h"

glitter_particle_manager* glitter_particle_manager_init() {
    glitter_particle_manager* gpm = force_malloc_s(glitter_particle_manager, 1);
    gpm->delta_frame = 2.0f;
    gpm->emission = 1.0f;
    gpm->random.value = 0;
    gpm->random.step = 1;
    vector_ptr_glitter_scene_reserve(&GPM_VAL->scenes, 0x100);
    vector_ptr_glitter_file_reader_reserve(&GPM_VAL->file_readers, 0x100);
    vector_ptr_glitter_effect_group_reserve(&GPM_VAL->effect_groups, 0x100);
    return gpm;
}

void glitter_particle_manager_calc_disp(GPM) {
    glitter_scene** i;
    glitter_scene* scene;
    glitter_scene_effect* j;

    for (i = GPM_VAL->scenes.begin; i != GPM_VAL->scenes.end; i++) {
        if (!*i)
            continue;

        scene = *i;
#if defined(CRE_DEV)
        if (GPM_VAL->draw_selected && GPM_VAL->scene && GPM_VAL->scene != scene)
            continue;
#endif
        if (scene->type == GLITTER_X) {
            for (j = scene->effects.begin; j != scene->effects.end; j++)
                if (j->ptr && j->disp) {
#if defined(CRE_DEV)
                    if (GPM_VAL->draw_selected && GPM_VAL->effect && GPM_VAL->effect != j->ptr)
                        continue;
#endif
                    glitter_x_effect_inst_calc_disp(GPM_VAL, j->ptr);
                }
        }
        else {
            for (j = scene->effects.begin; j != scene->effects.end; j++)
                if (j->ptr && j->disp) {
#if defined(CRE_DEV)
                    if (GPM_VAL->draw_selected && GPM_VAL->effect && GPM_VAL->effect != j->ptr)
                        continue;
#endif
                    glitter_effect_inst_calc_disp(GPM_VAL, j->ptr);
                }
        }
    }
}

bool glitter_particle_manager_check_effect_group(GPM, uint64_t effect_group_hash) {
    glitter_effect_group** i;

    for (i = GPM_VAL->effect_groups.begin; i != GPM_VAL->effect_groups.end; i++) {
        if (!*i || (*i)->hash != effect_group_hash)
            continue;
        break;
    }
    return i != GPM_VAL->effect_groups.end;
}

bool glitter_particle_manager_check_file_reader(GPM, uint64_t effect_group_hash) {
    glitter_file_reader** i;

    for (i = GPM_VAL->file_readers.begin; i != GPM_VAL->file_readers.end; i++) {
        if (!*i || (*i)->hash != effect_group_hash)
            continue;
        break;
    }
    return i != GPM_VAL->file_readers.end;
}

bool glitter_particle_manager_check_scene(GPM, uint64_t effect_group_hash) {
    glitter_scene** i;

    for (i = GPM_VAL->scenes.begin; i != GPM_VAL->scenes.end; i++) {
        if (!*i || (*i)->hash != effect_group_hash)
            continue;
        break;
    }
    return i != GPM_VAL->scenes.end;
}

void glitter_particle_manager_ctrl_file_reader(GPM) {
    for (glitter_file_reader** i = GPM_VAL->file_readers.begin; i != GPM_VAL->file_readers.end;) {
        if (*i) {
            glitter_file_reader* file_reader = *i;
            if (glitter_file_reader_read(GPM_VAL, file_reader, GPM_VAL->emission)) {
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

void glitter_particle_manager_ctrl_scene(GPM, float_t delta_frame) {
    GPM_VAL->delta_frame = delta_frame;
    for (glitter_scene** i = GPM_VAL->scenes.begin; i != GPM_VAL->scenes.end;) {
        glitter_scene* scene = *i;
        if (!scene || glitter_scene_has_ended(scene, true)) {
            vector_ptr_glitter_scene_erase(&GPM_VAL->scenes, i - GPM_VAL->scenes.begin, glitter_scene_dispose);
            continue;
        }

        if (scene->type == GLITTER_F2) {
            scene->delta_frame_history += GPM_VAL->delta_frame;
            if (!scene->skip) {
                glitter_scene_ctrl(GPM_VAL, scene, scene->delta_frame_history);
                scene->delta_frame_history = 0.0f;
                scene->skip = true;
            }
            else
                scene->skip = false;
        }
        else
            glitter_scene_ctrl(GPM_VAL, scene, GPM_VAL->delta_frame);
        i++;
    }
}

void glitter_particle_manager_ctrl_scene_by_hash(GPM,
    uint64_t effect_group_hash, float_t delta_frame) {
    for (glitter_scene** i = GPM_VAL->scenes.begin; i != GPM_VAL->scenes.end;) {
        glitter_scene* scene = *i;
        if (!scene || glitter_scene_has_ended(scene, true)) {
            vector_ptr_glitter_scene_erase(&GPM_VAL->scenes, i - GPM_VAL->scenes.begin, glitter_scene_dispose);
            continue;
        }

        if (scene->hash == effect_group_hash) {
            if (scene->type == GLITTER_F2) {
                scene->delta_frame_history += delta_frame;
                if (!scene->skip) {
                    glitter_scene_ctrl(GPM_VAL, scene, scene->delta_frame_history);
                    scene->delta_frame_history = 0.0f;
                    scene->skip = true;
                }
                else
                    scene->skip = false;
            }
            else
                glitter_scene_ctrl(GPM_VAL, scene, delta_frame);
            break;
        }
        i++;
    }
}

void glitter_particle_manager_disp(GPM, draw_pass_3d_type alpha) {
    if (GPM_VAL->no_draw)
        return;

    glitter_scene** i;
    glitter_scene* scene;
    glitter_scene_effect* j;

    for (i = GPM_VAL->scenes.begin; i != GPM_VAL->scenes.end; i++) {
        if (!*i)
            continue;

        scene = *i;
#if defined(CRE_DEV)
        if (GPM_VAL->draw_selected && GPM_VAL->scene && GPM_VAL->scene != scene)
            continue;
#endif
        if (scene->type == GLITTER_X) {
            for (j = scene->effects.begin; j != scene->effects.end; j++)
                if (j->ptr && j->disp) {
#if defined(CRE_DEV)
                    if (GPM_VAL->draw_selected && GPM_VAL->effect && GPM_VAL->effect != j->ptr)
                        continue;
#endif
                    glitter_x_effect_inst_disp(GPM_VAL, j->ptr, alpha);
                }
        }
        else {
            for (j = scene->effects.begin; j != scene->effects.end; j++)
                if (j->ptr && j->disp) {
#if defined(CRE_DEV)
                    if (GPM_VAL->draw_selected && GPM_VAL->effect && GPM_VAL->effect != j->ptr)
                        continue;
#endif
                    glitter_effect_inst_disp(GPM_VAL, j->ptr, alpha);
                }
        }
    }
}

bool glitter_particle_manager_free_effect_group(GPM, uint64_t effect_group_hash) {
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

void glitter_particle_manager_free_effect_groups(GPM) {
    vector_ptr_glitter_effect_group_clear(&GPM_VAL->effect_groups, glitter_effect_group_dispose);
}

bool glitter_particle_manager_free_scene(GPM, uint64_t effect_group_hash) {
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

void glitter_particle_manager_free_scenes(GPM) {
    vector_ptr_glitter_scene_clear(&GPM_VAL->scenes, glitter_scene_dispose);
}

size_t glitter_particle_manager_get_ctrl_count(GPM, glitter_particle_type ptcl_type) {
    size_t ctrl;
    glitter_scene** i;

    ctrl = 0;
    for (i = GPM_VAL->scenes.begin; i != GPM_VAL->scenes.end; i++)
        if (*i)
            ctrl += glitter_scene_get_ctrl_count(*i, ptcl_type);
    return ctrl;
}

size_t glitter_particle_manager_get_disp_count(GPM, glitter_particle_type ptcl_type) {
    size_t disp;
    glitter_scene** i;

    disp = 0;
    for (i = GPM_VAL->scenes.begin; i != GPM_VAL->scenes.end; i++)
        if (*i)
            disp += glitter_scene_get_disp_count(*i, ptcl_type);
    return disp;
}

glitter_effect_group* glitter_particle_manager_get_effect_group(GPM, uint64_t effect_group_hash) {
    glitter_effect_group** i;
    glitter_effect_group* effect_group;

    for (i = GPM_VAL->effect_groups.begin; i != GPM_VAL->effect_groups.end; i++) {
        effect_group = *i;
        if (effect_group && effect_group->hash == effect_group_hash)
            return effect_group;
    }
    return 0;
}

void glitter_particle_manager_get_frame(GPM, float_t* frame, int32_t* life_time) {
    if (frame)
        *frame = 0.0f;

    if (life_time)
        *life_time = 0;

    if (GPM_VAL->scenes.begin == GPM_VAL->scenes.end)
        return;

    glitter_scene_get_frame(*GPM_VAL->scenes.begin, frame, life_time);
}

glitter_scene* glitter_particle_manager_get_scene(GPM, uint64_t effect_group_hash) {
    glitter_scene** i;
    glitter_scene* scene;

    for (i = GPM_VAL->scenes.begin; i != GPM_VAL->scenes.end; i++) {
        scene = *i;
        if (scene && scene->hash == effect_group_hash)
            return scene;
    }
    return 0;
}

void glitter_particle_manager_get_start_end_frame(GPM, int32_t* start_frame, int32_t* end_frame) {
    if (start_frame)
        *start_frame = 0;

    if (end_frame)
        *end_frame = 0;

    if (GPM_VAL->scenes.begin == GPM_VAL->scenes.end || !*GPM_VAL->scenes.begin)
        return;

    if (start_frame)
        *start_frame = INT16_MAX;

    glitter_scene_get_start_end_frame(*GPM_VAL->scenes.begin, start_frame, end_frame);
}

bool glitter_particle_manager_load_effect(GPM, uint64_t effect_group_hash, uint64_t effect_hash) {
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
                glitter_scene_init_effect(GPM_VAL, scene, effect, id, false);
        }
        break;
    }
    return i != GPM_VAL->effect_groups.end;
}

bool glitter_particle_manager_load_scene(GPM, uint64_t effect_group_hash) {
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
            glitter_scene_init_effect(GPM_VAL, scene, *k, id, true);
        break;
    }
    return i != GPM_VAL->effect_groups.end;
}

void glitter_particle_manager_set_frame(GPM,
    glitter_effect_group* effect_group, glitter_scene** scene, float_t curr_frame,
    float_t prev_frame, uint32_t counter, glitter_random* random, bool reset) {
    if (curr_frame < prev_frame || reset) {
        for (glitter_scene** i = GPM_VAL->scenes.begin; i != GPM_VAL->scenes.end; i++)
            if (*i == *scene) {
                vector_ptr_glitter_scene_erase(&GPM_VAL->scenes, i - GPM_VAL->scenes.begin, glitter_scene_dispose);
                break;
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
            if (s->type == GLITTER_F2) {
                s->delta_frame_history += delta_frame;
                if (!s->skip) {
                    glitter_scene_ctrl(GPM_VAL, s, s->delta_frame_history);
                    s->delta_frame_history = 0.0f;
                    s->skip = true;
                }
                else
                    s->skip = false;
            }
            else
                glitter_scene_ctrl(GPM_VAL, s, delta_frame);
    }
}

void glitter_particle_manager_dispose(GPM) {
    vector_ptr_glitter_scene_free(&GPM_VAL->scenes, glitter_scene_dispose);
    vector_ptr_glitter_file_reader_free(&GPM_VAL->file_readers, glitter_file_reader_dispose);
    vector_ptr_glitter_effect_group_free(&GPM_VAL->effect_groups, glitter_effect_group_dispose);
    free(gpm);
}
