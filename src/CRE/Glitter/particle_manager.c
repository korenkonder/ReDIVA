/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "particle_manager.h"
#include "effect_group.h"
#include "file_reader.h"
#include "render_group.h"
#include "scene.h"

glitter_particle_manager* FASTCALL glitter_particle_manager_init() {
    glitter_particle_manager* gpm = force_malloc(sizeof(glitter_particle_manager));
    gpm->delta_frame = 2.0f;
    vector_ptr_glitter_scene_expand(&gpm->scenes, 0x100);
    vector_ptr_glitter_file_reader_expand(&gpm->file_readers, 0x100);
    vector_ptr_glitter_effect_group_expand(&gpm->effect_groups, 0x100);
    return gpm;
}

bool FASTCALL glitter_particle_manager_check_effect_group(glitter_particle_manager* gpm,
    uint64_t effect_group_hash) {
    glitter_effect_group** i;

    for (i = gpm->effect_groups.begin; i != gpm->effect_groups.end; i++) {
        if (!*i || (*i)->hash != effect_group_hash)
            continue;
        break;
    }
    return i != gpm->effect_groups.end;
}

bool FASTCALL glitter_particle_manager_check_file_reader(glitter_particle_manager* gpm,
    uint64_t effect_group_hash) {
    glitter_file_reader** i;

    for (i = gpm->file_readers.begin; i != gpm->file_readers.end; i++) {
        if (!*i || (*i)->hash != effect_group_hash)
            continue;
        break;
    }
    return i != gpm->file_readers.end;
}

bool FASTCALL glitter_particle_manager_check_scene(glitter_particle_manager* gpm,
    uint64_t effect_group_hash) {
    glitter_scene** i;

    for (i = gpm->scenes.begin; i != gpm->scenes.end; i++) {
        if (!*i || (*i)->hash != effect_group_hash)
            continue;
        break;
    }
    return i != gpm->scenes.end;
}

void FASTCALL glitter_particle_manager_draw(glitter_particle_manager* gpm, int32_t alpha) {
    for (glitter_scene** i = gpm->scenes.begin; i != gpm->scenes.end; i++) {
        if (!*i)
            continue;

        glitter_scene_sub* scene = &(*i)->sub;
        for (glitter_render_group** j = scene->render_groups.begin; j != scene->render_groups.end; j++)
            if (*j && (*j)->alpha == alpha)
                glitter_render_group_draw(scene, *j);
    }
}

bool FASTCALL glitter_particle_manager_free_effect_group(glitter_particle_manager* gpm,
    uint64_t effect_group_hash) {
    glitter_effect_group** i;
    glitter_effect_group* effect_group;

    for (i = gpm->effect_groups.begin; i != gpm->effect_groups.end; i++) {
        effect_group = *i;
        if (effect_group && effect_group->hash != effect_group_hash)
            continue;

        if (effect_group)
            glitter_effect_group_dispose(effect_group);
        memmove(i, i + 1, sizeof(glitter_effect_group**) * (gpm->effect_groups.end - (i + 1)));
        gpm->effect_groups.end--;
        break;
    }
    return i != gpm->effect_groups.end;
}

bool FASTCALL glitter_particle_manager_free_scene(glitter_particle_manager* gpm,
    uint64_t effect_group_hash) {
    glitter_scene** i;
    glitter_scene* scene;

    for (i = gpm->scenes.begin; i != gpm->scenes.end;) {
        scene = *i;
        if (scene && scene->hash != effect_group_hash) {
            i++;
            continue;
        }

        if (scene)
            glitter_scene_dispose(scene);
        memmove(i, i + 1, sizeof(glitter_scene**) * (gpm->scenes.end - (i + 1)));
        gpm->scenes.end--;
        break;
    }
    return i != gpm->scenes.end;
}

size_t FASTCALL glitter_particle_manager_get_ctrl_count(glitter_particle_manager* gpm,
    glitter_particle_type type) {
    size_t ctrl;
    glitter_scene** i;

    ctrl = 0;
    switch (type) {
    case GLITTER_PARTICLE_QUAD:
        for (i = gpm->scenes.begin; i != gpm->scenes.end; i++)
            ctrl += (*i)->sub.ctrl_quad;
        break;
    case GLITTER_PARTICLE_LINE:
        for (i = gpm->scenes.begin; i != gpm->scenes.end; i++)
            ctrl += (*i)->sub.ctrl_line;
        break;
    case GLITTER_PARTICLE_LOCUS:
        for (i = gpm->scenes.begin; i != gpm->scenes.end; i++)
            ctrl += (*i)->sub.ctrl_locus;
        break;
    }
    return ctrl;
}

size_t FASTCALL glitter_particle_manager_get_disp_count(glitter_particle_manager* gpm,
    glitter_particle_type type) {
    size_t disp;
    glitter_scene** i;

    disp = 0;
    switch (type) {
    case GLITTER_PARTICLE_QUAD:
        for (i = gpm->scenes.begin; i != gpm->scenes.end; i++)
            disp += (*i)->sub.disp_quad;
        break;
    case GLITTER_PARTICLE_LINE:
        for (i = gpm->scenes.begin; i != gpm->scenes.end; i++)
            disp += (*i)->sub.disp_line;
        break;
    case GLITTER_PARTICLE_LOCUS:
        for (i = gpm->scenes.begin; i != gpm->scenes.end; i++)
            disp += (*i)->sub.disp_locus;
        break;
    }
    return disp;
}

void glitter_particle_manager_get_time(glitter_particle_manager* gpm, float_t* frame, float_t* life_time) {
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
        if (effect && frame && life_time && effect->data.life_time > *life_time) {
            if (*frame < effect->frame0)
                *frame = effect->frame0;
            *life_time = effect->data.life_time;
        }
    }
}

bool FASTCALL glitter_particle_manager_load_effect(glitter_particle_manager* gpm,
    uint64_t effect_group_hash, uint64_t effect_hash) {
    glitter_effect_group** i;
    glitter_effect_group* effect_group;
    glitter_scene** j;
    glitter_scene* scene;
    glitter_effect** k;
    glitter_effect* effect;
    int32_t id;

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
            vector_ptr_glitter_scene_append_element(&gpm->scenes, &scene);
        }

        for (k = effect_group->effects.begin, id = 1; k != effect_group->effects.end; k++, id++) {
            effect = *k;
            if (effect && effect->data.name_hash == effect_hash)
                glitter_scene_init_effect(scene, effect, id, false);
        }
        break;
    }
    return i != gpm->effect_groups.end;
}

bool FASTCALL glitter_particle_manager_load_scene(glitter_particle_manager* gpm,
    uint64_t effect_group_hash) {
    glitter_effect_group** i;
    glitter_effect_group* effect_group;
    glitter_scene** j;
    glitter_scene* scene;
    glitter_effect** k;
    int32_t id;

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
            vector_ptr_glitter_scene_append_element(&gpm->scenes, &scene);
        }

        for (k = effect_group->effects.begin, id = 1; k != effect_group->effects.end; k++, id++)
            glitter_scene_init_effect(scene, *k, id, false);
        break;
    }
    return i != gpm->effect_groups.end;
}

void FASTCALL glitter_particle_manager_reset_scene_disp_counter(glitter_particle_manager* gpm) {
    for (glitter_scene** i = gpm->scenes.begin; i != gpm->scenes.end; i++) {
        if (!*i)
            continue;

        glitter_scene* scene = *i;
        scene->sub.disp_quad = 0;
        scene->sub.disp_locus = 0;
        scene->sub.disp_line = 0;
    }
}

bool FASTCALL glitter_particle_manager_test_load_effect(glitter_particle_manager* gpm,
    uint64_t effect_group_hash, uint64_t effect_hash) {
    glitter_effect_group** i;
    glitter_effect_group* effect_group;
    glitter_scene** j;
    glitter_scene* scene;
    glitter_effect** k;
    glitter_effect* effect;
    int32_t id;

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
            vector_ptr_glitter_scene_append_element(&gpm->scenes, &scene);
        }

        for (k = effect_group->effects.begin, id = 1; k != effect_group->effects.end; k++, id++) {
            effect = *k;
            if (effect && effect->data.name_hash == effect_hash)
                glitter_scene_init_effect(scene, effect, id, true);
        }
        break;
    }
    return i != gpm->effect_groups.end;
}

bool FASTCALL glitter_particle_manager_test_load_scene(glitter_particle_manager* gpm,
    uint64_t effect_group_hash) {
    glitter_effect_group** i;
    glitter_effect_group* effect_group;
    glitter_scene** j;
    glitter_scene* scene;
    glitter_effect** k;
    int32_t id;

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
            vector_ptr_glitter_scene_append_element(&gpm->scenes, &scene);
        }

        for (k = effect_group->effects.begin, id = 1; k != effect_group->effects.end; k++, id++)
            glitter_scene_init_effect(scene, *k, id, true);
        break;
    }
    return i != gpm->effect_groups.end;
}

void FASTCALL glitter_particle_manager_update_file_reader(glitter_particle_manager* gpm) {
    for (glitter_file_reader** i = gpm->file_readers.begin; i != gpm->file_readers.end;) {
        if (*i) {
            glitter_file_reader* file_reader = *i;
            if (glitter_file_reader_read(file_reader))
                vector_ptr_glitter_effect_group_append_element(&gpm->effect_groups, &file_reader->effect_group);
            else if (file_reader->effect_group)
                glitter_effect_group_dispose(file_reader->effect_group);
            glitter_file_reader_dispose(file_reader);
        }
        vector_ptr_glitter_file_reader_remove(&gpm->file_readers, i - gpm->file_readers.begin);
    }
}

void FASTCALL glitter_particle_manager_update_scene(glitter_particle_manager* gpm) {
    gpm->delta_frame = get_frame_speed();
    for (glitter_scene** i = gpm->scenes.begin; i != gpm->scenes.end;) {
        glitter_scene* scene = *i;
        if (!scene)
            goto copy_scenes;

        if (glitter_scene_has_ended(scene, true)) {
            if (scene)
                glitter_scene_dispose(scene);

        copy_scenes:
            vector_ptr_glitter_scene_remove(&gpm->scenes, i - gpm->scenes.begin);
        }
        else {
            glitter_scene_update_value_frame(scene, gpm->delta_frame);
            glitter_scene_emit(scene, gpm->delta_frame);
            glitter_scene_update_scene(scene, gpm->delta_frame);
            i++;
        }
    }
}

void FASTCALL glitter_particle_manager_dispose(glitter_particle_manager* gpm) {
    vector_ptr_glitter_scene_clear(&gpm->scenes, (void*)glitter_scene_dispose);
    vector_ptr_glitter_scene_dispose(&gpm->scenes);
    vector_ptr_glitter_file_reader_clear(&gpm->file_readers, (void*)glitter_file_reader_dispose);
    vector_ptr_glitter_file_reader_dispose(&gpm->file_readers);
    vector_ptr_glitter_effect_group_clear(&gpm->effect_groups, (void*)glitter_effect_group_dispose);
    vector_ptr_glitter_effect_group_dispose(&gpm->effect_groups);
    free(gpm);
}
