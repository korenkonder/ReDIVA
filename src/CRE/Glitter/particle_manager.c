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
    gpm->delta_frame = 2.0;
    vector_ptr_glitter_scene_expand(&gpm->scenes, 0x100);
    vector_ptr_glitter_file_reader_expand(&gpm->file_readers, 0x100);
    vector_ptr_glitter_effect_group_expand(&gpm->effect_groups, 0x100);
    return gpm;
}

void FASTCALL glitter_particle_manager_update_file_reader(glitter_particle_manager* gpm) {
    for (glitter_file_reader** i = gpm->file_readers.begin; i != gpm->file_readers.end;) {
        glitter_file_reader* file_reader = *i;
        if (!file_reader)
            goto copy_file_readers;

        if (Glitter__FileReader__Read(file_reader)) {
            vector_ptr_glitter_effect_group_append_element(&gpm->effect_groups,
                &file_reader->effect_group);
            glitter_file_reader_dispose(file_reader);

        copy_file_readers:
            vector_ptr_glitter_file_reader_remove(&gpm->file_readers, i - gpm->file_readers.begin);
        }
        else {
            if (file_reader->effect_group)
                glitter_effect_group_dispose(file_reader->effect_group);
            glitter_file_reader_dispose(file_reader);
            i++;
        }
    }
}

void FASTCALL glitter_particle_manager_update_scene(glitter_particle_manager* gpm) {
    gpm->delta_frame = get_frame_speed();
    for (glitter_scene** i = gpm->scenes.begin; i != gpm->scenes.end;) {
        glitter_scene* scene = *i;
        if (!scene)
            goto copy_scenes;

        if (Glitter__Scene__HasEnded(scene, true)) {
            if (scene)
                glitter_scene_dispose(scene);
            
        copy_scenes:
            vector_ptr_glitter_scene_remove(&gpm->scenes, i - gpm->scenes.begin);
        }
        else {
            Glitter__Scene__UpdateValueFrame(scene, gpm->delta_frame);
            Glitter__Scene__Emit(scene, gpm->delta_frame);
            Glitter__Scene__UpdateScene(scene, gpm->delta_frame);
            i++;
        }
    }
}

void FASTCALL glitter_particle_manager_draw(glitter_particle_manager* gpm, int32_t alpha) {
    for (glitter_scene** i = gpm->scenes.begin; i != gpm->scenes.end; i++) {
        if (!*i)
            continue;

        glitter_scene_sub* scene = &(*i)->sub;
        scene->disp_quad = 0;
        scene->disp_locus = 0;
        scene->disp_line = 0;

        for (glitter_render_group** j = scene->render_groups.begin; j != scene->render_groups.end; j++)
            if (*j && (*j)->alpha == alpha)
                Glitter__RenderGroup__Draw(scene, *j);
    }
}

void FASTCALL glitter_particle_manager_load_scene(glitter_particle_manager* gpm,
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
    }
}

void FASTCALL glitter_particle_manager_load_effect(glitter_particle_manager* gpm,
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
    }
}

void FASTCALL glitter_particle_manager_free_scene(glitter_particle_manager* gpm,
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
    }
}

void FASTCALL glitter_particle_manager_free_effect_group(glitter_particle_manager* gpm,
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
    }
}

void FASTCALL glitter_particle_manager_dispose(glitter_particle_manager* gpm) {
    vector_ptr_glitter_scene_clear(&gpm->scenes, (void*)&glitter_scene_dispose);
    vector_ptr_glitter_scene_dispose(&gpm->scenes);
    vector_ptr_glitter_file_reader_clear(&gpm->file_readers, (void*)&glitter_file_reader_dispose);
    vector_ptr_glitter_file_reader_dispose(&gpm->file_readers);
    vector_ptr_glitter_effect_group_clear(&gpm->effect_groups, (void*)&glitter_effect_group_dispose);
    vector_ptr_glitter_effect_group_dispose(&gpm->effect_groups);
    free(gpm);
}
