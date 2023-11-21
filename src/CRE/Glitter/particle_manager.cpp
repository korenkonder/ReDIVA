/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"
#include "../render_context.hpp"

extern render_context* rctx_ptr;

namespace Glitter {
    GltParticleManager* glt_particle_manager;

    Camera::Camera() : rotation_y() {

    }

    GltParticleManager::GltParticleManager() :
#if defined(CRE_DEV)
        selected_scene(), selected_effect(), selected_emitter(), selected_particle(),
#endif
        bone_data(), frame_rate(), cam(), flags(),
        scene_load_counter(), texture_counter(), random(), counter(), draw_selected() {
        emission = 1.5f;
        delta_frame = 2.0f;
        draw_all = true;
        draw_all_mesh = true;
        scenes.reserve(0x100);
        file_readers.reserve(0x100);
        batch_ubo.Create(sizeof(BatchShaderData));
    }

    GltParticleManager::~GltParticleManager() {
        FreeEffects();
        batch_ubo.Destroy();
    }

    bool GltParticleManager::Init() {
        frame_rate = get_sys_frame_rate();
        return true;
    }

    bool GltParticleManager::Ctrl() {
        if (flags & PARTICLE_MANAGER_READ_FILES) {
            for (std::vector<FileReader*>::iterator i = file_readers.begin(); i != file_readers.end();)
                if (!*i || (*i)->ReadFarc(this)) {
                    delete* i;
                    i = file_readers.erase(i);
                }
                else
                    i++;

            if (!file_readers.size())
                enum_and(flags, ~PARTICLE_MANAGER_READ_FILES);
        }

        if (!(flags & PARTICLE_MANAGER_PAUSE)) {
            if (frame_rate)
                delta_frame = frame_rate->GetDeltaFrame();
            else
                delta_frame = get_delta_frame();

            CtrlScenes();
        }
        return false;
    }

    bool GltParticleManager::Dest() {
        return true;
    }

    void GltParticleManager::Disp() {
        if (flags & PARTICLE_MANAGER_NOT_DISP)
            return;

        for (Scene*& i : scenes)
            if (i)
                i->CalcDisp(this);
    }

    void GltParticleManager::Basic() {
        BasicEffectGroups();
    }

    bool GltParticleManager::AppendEffectGroup(uint64_t hash, EffectGroup* eff_group, FileReader* file_read) {
        if (effect_groups.find(hash) != effect_groups.end())
            return false;

        eff_group->load_count = file_read->load_count;
        eff_group->emission = file_read->emission;
        if (file_read->emission <= 0.0f)
            eff_group->emission = emission;
        eff_group->hash = hash;

        effect_groups.insert({ hash, eff_group });
        return true;
    }

    void GltParticleManager::BasicEffectGroups() {
        for (std::map<uint64_t, EffectGroup*>::iterator i = effect_groups.begin(); i != effect_groups.end(); ) {
            if (i->second->load_count > 0) {
                i++;
                continue;
            }

            uint64_t hash = i->first;
            for (std::vector<Scene*>::iterator j = scenes.begin(); j != scenes.end();) {
                Scene* scene = *j;
                if (!scene || scene->hash != hash) {
                    j++;
                    continue;
                }

                delete* j;
                j = scenes.erase(j);
            }

            delete i->second;
            i = effect_groups.erase(i);
        }

#if defined(CRE_DEV)
        bool local = false;
        for (std::vector<Scene*>::iterator i = scenes.begin(); i != scenes.end();) {
            Scene* scene = *i;
            if (!(scene->flags & SCENE_ENDED)) {
                if (!local && scene->CanDisp(DISP_LOCAL, true))
                    local = true;
                i++;
                continue;
            }

            if (scene->fade_frame_left > 0.0f) {
                scene->fade_frame_left -= delta_frame;
                if (scene->fade_frame_left <= 0.0f)
                    scene->fade_frame_left = -1.0f;
            }

            bool free_scene = true;
            for (std::vector<SceneEffect>::iterator j = scene->effects.begin(); j != scene->effects.end();)
                if (!j->ptr || scene->fade_frame_left < 0.0f) {
                    delete j->ptr;
                    j = scene->effects.erase(j);
                }
                else {
                    free_scene = false;
                    j++;
                }

            if (free_scene) {
                delete* i;
                i = scenes.erase(i);
            }
            else
                i++;
        }

        if (local)
            enum_or(flags, PARTICLE_MANAGER_LOCAL);
        else
            enum_and(flags, ~PARTICLE_MANAGER_LOCAL);
#endif
    }

    uint64_t GltParticleManager::CalculateHash(const char* str) {
        return hash_utf8_fnv1a64m(str);
    }

#if defined(CRE_DEV)
    bool GltParticleManager::CheckHasLocalEffect() {
        return!!(flags & PARTICLE_MANAGER_LOCAL);
    }
#endif

    bool GltParticleManager::CheckHasFileReader(uint64_t hash) {
        for (FileReader*& i : file_readers)
            if (i && i->hash == hash)
                return true;
        return false;
    }

    bool GltParticleManager::CheckNoFileReaders(uint64_t hash) {
        std::map<uint64_t, EffectGroup*>::iterator elem = effect_groups.find(hash);
        if (elem != effect_groups.end()) {
            if (file_readers.size())
                return true;

            for (FileReader*& i : file_readers)
                if (i && i->hash == hash)
                    return false;
            return true;
        }
        return false;
    }

    bool GltParticleManager::CheckSceneEnded(SceneCounter scene_counter) {
        if (!scene_counter)
            return false;

        for (Scene*& i : scenes)
            if (i && i->counter.counter == scene_counter.counter) {
                if (!scene_counter.index) {
                    if (i->type == Glitter::X)
                        return !(i->flags & SCENE_ENDED) && !i->HasEnded(true);
                    else
                        return !i->HasEnded(true);
                }
                else
                    return i->HasEnded(scene_counter.index, true);
            }
        return false;
    }

#if defined(CRE_DEV)
    void GltParticleManager::CheckSceneHasLocalEffect(Scene* sc) {
        if (sc->type != Glitter::X)
            return;

        if (flags & PARTICLE_MANAGER_LOCAL && sc->CanDisp(DISP_LOCAL, false))
            enum_or(flags, PARTICLE_MANAGER_LOCAL);

        sc->fade_frame = 30.0f;
    }
#endif

    int32_t GltParticleManager::CounterGet() {
        return counter;
    }

    void GltParticleManager::CounterIncrement() {
        counter++;
    }

    void GltParticleManager::CtrlScenes() {
        camera* c = rctx_ptr->camera;
        c->update_data();
        cam.projection = c->projection;
        cam.view = c->view;
        cam.inv_view = c->inv_view;
        cam.inv_view_mat3 = c->inv_view_mat3;
        cam.view_point = c->view_point;
        cam.rotation_y = c->rotation.y;

        for (std::vector<Scene*>::iterator i = scenes.begin(); i != scenes.end();) {
            Scene* scene = *i;
    #if defined(CRE_DEV)
            if (!scene || scene->type != Glitter::X && scene->HasEnded(true)) {
                delete scene;
                i = scenes.erase(i);
            }
            else if (scene->type == Glitter::X && scene->flags & SCENE_ENDED
                || !(scene->flags & SCENE_EDITOR)) {
                float_t delta_frame;
                if (scene->frame_rate)
                    delta_frame = scene->frame_rate->GetDeltaFrame();
                else
                    delta_frame = this->delta_frame;
                scene->Ctrl(this, delta_frame);
                i++;
            }
            else
                i++;
    #else
            if (!scene || scene->HasEnded(true)) {
                delete scene;
                i = scenes.erase(i);
            }
            else {
                scene->Ctrl(this, delta_frame);
                i++;
            }
    #endif
        }
    }

    void GltParticleManager::DispScenes(DispType disp_type) {
        if (flags & PARTICLE_MANAGER_NOT_DISP)
            return;

        camera* c = rctx_ptr->camera;
        c->update_data();
        cam.projection = c->projection;
        cam.view = c->view;
        cam.inv_view = c->inv_view;
        cam.inv_view_mat3 = c->inv_view_mat3;
        cam.view_point = c->view_point;
        cam.rotation_y = c->rotation.y;

        for (Scene*& i : scenes) {
            if (!i)
                continue;

    #if defined(CRE_DEV)
            if (draw_selected && selected_scene && selected_scene != i)
                continue;
    #endif
            i->Disp(this, disp_type);
        }

        gl_state_bind_vertex_array(0);
        gl_state_disable_blend();
        gl_state_enable_cull_face();
        gl_state_disable_depth_test();
    }

    void GltParticleManager::FreeEffects() {
        scenes.clear();
        file_readers.clear();
    }

    void GltParticleManager::FreeSceneEffect(SceneCounter scene_counter, bool force_kill) {
        if (!scene_counter)
            return;

        for (std::vector<Scene*>::iterator i = scenes.begin(); i != scenes.end();) {
            Scene* scene = *i;
            if (!scene || scene_counter.counter != scene->counter.counter) {
                i++;
                continue;
            }

            bool free = true;
            if (scene_counter.index) {
                scene->FreeEffectByID(this, scene_counter.index, force_kill);
                free = scene->HasEnded(true);
            }

            if (free) {
#if defined(CRE_DEV)
                if (!(scene->flags & SCENE_ENDED)) {
                    enum_or(scene->flags, SCENE_ENDED);
                    scene->fade_frame_left = scene->fade_frame;
                }
                i++;
#else
                delete scene;
                i = scenes.erase(i);
#endif
            }
            break;
        }
    }

    void GltParticleManager::FreeSceneEffect(uint64_t effect_group_hash, uint64_t effect_hash, bool force_kill) {
        if (effect_hash == hash_fnv1a64m_empty || effect_hash == hash_murmurhash_empty)
            return;

        if (effect_group_hash == hash_fnv1a64m_empty || effect_group_hash == hash_murmurhash_empty) {
            for (Scene*& i : scenes)
                if (i && i->FreeEffect(this, effect_hash, force_kill))
                    break;
        }
        else
            for (Scene*& i : scenes)
                if (i && i->hash == effect_group_hash && i->FreeEffect(this, effect_hash, force_kill))
                    break;
    }

    void GltParticleManager::FreeScene(uint64_t effect_group_hash) {
        for (Scene*& i : scenes)
            if (i && i->hash == effect_group_hash) {
                i->FreeEffect(this, i->type == Glitter::FT
                    ? hash_fnv1a64m_empty : hash_murmurhash_empty, true);
                break;
            }
    }

    void GltParticleManager::FreeScenes() {
        for (Scene*& i : scenes)
            if (i)
                i->FreeEffect(this, i->type == Glitter::FT
                    ? hash_fnv1a64m_empty : hash_murmurhash_empty, true);
    }

    size_t GltParticleManager::GetCtrlCount(ParticleType type) {
        size_t ctrl = 0;
        for (Scene*& i : scenes)
            if (i)
                ctrl += i->GetCtrlCount(type);
        return ctrl;
    }

    size_t GltParticleManager::GetDispCount(ParticleType type) {
        size_t disp = 0;
        for (Scene*& i : scenes)
            if (i)
                disp += i->GetDispCount(type);
        return disp;
    }

    EffectGroup* GltParticleManager::GetEffectGroup(uint64_t hash) {
        std::map<uint64_t, EffectGroup*>::iterator elem = effect_groups.find(hash);
        if (elem != effect_groups.end())
            return elem->second;
        return 0;
    }

    const char* GltParticleManager::GetEffectName(uint64_t hash, int32_t index) {
        std::map<uint64_t, EffectGroup*>::iterator elem = effect_groups.find(hash);
        if (elem == effect_groups.end())
            return 0;

        EffectGroup* eff_group = elem->second;
        if (index >= eff_group->effects.size())
            return 0;
        return eff_group->effects.data()[index]->name.c_str();
    }

    size_t GltParticleManager::GetEffectsCount(uint64_t hash) {
        std::map<uint64_t, EffectGroup*>::iterator elem = effect_groups.find(hash);
        if (elem == effect_groups.end())
            return 0;

        EffectGroup* eff_group = elem->second;
        return eff_group->effects.size();
    }

    bool GltParticleManager::GetPause() {
        return flags & PARTICLE_MANAGER_PAUSE ? true : false;
    }

    Scene* GltParticleManager::GetScene(uint64_t hash) {
        for (Scene*& i : scenes)
            if (i && i->hash == hash)
                return i;
        return 0;
    }

    Scene* GltParticleManager::GetScene(SceneCounter scene_counter) {
        for (Scene*& i : scenes)
            if (i && i->counter.counter == scene_counter.counter)
                return i;
        return 0;
    }

    void GltParticleManager::GetSceneStartEndFrame(int32_t* start_frame,
        int32_t* end_frame, SceneCounter scene_counter) {
        if (start_frame)
            *start_frame = 0;

        if (end_frame)
            *end_frame = 0;

        if (!scenes.size())
            return;

        for (Scene*& i : scenes)
            if (i && i->counter.counter == scene_counter.counter) {
                if (start_frame)
                    *start_frame = INT16_MAX;
                i->GetStartEndFrame(start_frame, end_frame);
                return;
            }
    }

    float_t GltParticleManager::GetSceneFrameLifeTime(SceneCounter scene_counter, int32_t* life_time) {
        if (life_time)
            *life_time = 0;

        for (Scene*& i : scenes)
            if (i && i->counter.counter == scene_counter.counter)
                return i->GeFrameLifeTime(life_time, scene_counter.index);
        return 0.0f;
    }

    SceneCounter GltParticleManager::GetSceneCounter(uint8_t index) {
        if (scene_load_counter + 1 < 0xFFFFFF && !(flags & PARTICLE_MANAGER_RESET_SCENE_COUNTER)) {
            scene_load_counter++;
            return SceneCounter(index, scene_load_counter);
        }

        if (scenes.size()) {
            enum_and(flags, ~PARTICLE_MANAGER_RESET_SCENE_COUNTER);
            scene_load_counter = 1;
            return SceneCounter(index, scene_load_counter);
        }

        uint32_t counter = 0;
        for (Scene*& i : scenes)
            if (i && counter < i->counter)
                counter = i->counter;
        counter++;

        if (counter < 0xFFFFFF)
            enum_and(flags, ~PARTICLE_MANAGER_RESET_SCENE_COUNTER);
        else {
            enum_or(flags, PARTICLE_MANAGER_RESET_SCENE_COUNTER);
            for (uint32_t v7 = 1; ; v7++) {
                bool found = false;
                for (Scene*& i : scenes)
                    if (i && scene_load_counter == i->counter.counter) {
                        found = true;
                        break;
                    }

                if (!found)
                    break;

                if (v7 >= 300)
                    return 0;
            }
        }
        return SceneCounter(index, counter);
    }

    SceneCounter GltParticleManager::Load(uint64_t effect_group_hash, uint64_t effect_hash, bool use_existing) {
        if (effect_group_hash == hash_fnv1a64m_empty || effect_group_hash == hash_murmurhash_empty) {
            if (effect_hash == hash_fnv1a64m_empty || effect_hash == hash_murmurhash_empty)
                return 0;

            if (use_existing) {
                size_t id = 0;
                for (Scene*& i : scenes) {
                    if (i && i->ResetEffect(this, effect_hash, &id)) {
                        CheckSceneHasLocalEffect(i);

                        SceneCounter counter = i->counter;
                        if (i->type == Glitter::X)
                            counter.index = (uint8_t)id;
                        return counter;
                    }
                }
            }

            if (effect_groups.size())
                return 0;

            bool found = false;
            for (std::pair<uint64_t, EffectGroup*> i : effect_groups) {
                for (Effect*& j : i.second->effects)
                    if (j->data.name_hash == effect_hash
                        && i.first != hash_fnv1a64m_empty && i.first != hash_murmurhash_empty) {
                        found = true;
                        break;
                    }

                if (found)
                    break;
            }

            if (!found)
                return 0;
        }

        std::map<uint64_t, EffectGroup*>::iterator elem = effect_groups.find(effect_group_hash);
        if (elem == effect_groups.end())
            return 0;

        EffectGroup* eff_group = elem->second;
        if (eff_group->not_loaded)
            return 0;

        if (use_existing)
            for (Scene*& i : scenes) {
                if (!i || i->hash != effect_group_hash)
                    continue;

                size_t id = 1;
                if (effect_hash == hash_fnv1a64m_empty || effect_hash == hash_murmurhash_empty) {
                    for (Effect*& j : eff_group->effects)
                        i->InitEffect(this, j, id++, false);

                    if (i->type == Glitter::X)
                        id = 0;
                }
                else
                    for (Effect*& j : eff_group->effects) {
                        if (effect_hash == j->data.name_hash) {
                            i->InitEffect(this, j, id, false);
                            break;
                        }
                        id++;
                    }
                CheckSceneHasLocalEffect(i);

                SceneCounter counter = i->counter;
                if (i->type == Glitter::X)
                    counter.index = (uint8_t)id;
                return counter;
            }

        SceneCounter counter = GetSceneCounter();
        if (!counter)
            return 0;

        Scene* scene = new Scene(counter, effect_group_hash, eff_group, 0);
        if (!scene)
            return 0;

        size_t id = 1;
        if (effect_hash == hash_fnv1a64m_empty || effect_hash == hash_murmurhash_empty) {
            for (Effect*& i : eff_group->effects)
                if (i)
                    scene->InitEffect(this, i, id++, false);

            if (scene->type == Glitter::X)
                id = 0;
        }
        else
            for (Effect*& i : eff_group->effects) {
                if (!i)
                    continue;

                if (effect_hash == i->data.name_hash) {
                    scene->InitEffect(this, i, id, false);
                    break;
                }
                id++;
            }
        CheckSceneHasLocalEffect(scene);
        scenes.push_back(scene);

        if (scene->type == Glitter::X)
            counter.index = (uint8_t)id;
        return counter;
    }

    uint64_t GltParticleManager::LoadFile(GLT, void* data, const char* file,
        const char* path, float_t emission, bool init_scene, object_database* obj_db) {
        uint64_t effect_group_hash;
        uint64_t empty_hash;
        if (GLT_VAL == Glitter::FT) {
            effect_group_hash = hash_utf8_fnv1a64m(file);
            if (effect_group_hash == hash_fnv1a64m_empty)
                return effect_group_hash;
            empty_hash = hash_fnv1a64m_empty;
        }
        else {
            effect_group_hash = hash_utf8_murmurhash(file);
            if (effect_group_hash == hash_murmurhash_empty)
                return effect_group_hash;
            empty_hash = hash_murmurhash_empty;
        }

        std::map<uint64_t, EffectGroup*>::iterator elem = effect_groups.find(effect_group_hash);
        if (elem != effect_groups.end()) {
            EffectGroup* eff_group = elem->second;
            eff_group->emission = emission;
            if (eff_group->emission <= 0.0f)
                eff_group->emission = this->emission;
            eff_group->load_count++;
            return effect_group_hash;
        }

        for (FileReader*& i : file_readers)
            if (i && i->hash == effect_group_hash) {
                i->load_count++;
                i->emission = emission;
                return effect_group_hash;
            }

        FileReader* fr = new FileReader(GLT_VAL);
        if (!fr)
            return empty_hash;

        fr->init_scene = init_scene;

        if (!path)
            path = GLT_VAL != Glitter::FT ? "root+/particle/" : "rom/particle/";

        if (!fr->LoadFarc(data, path, file, effect_group_hash, obj_db)) {
            delete fr;
            return empty_hash;
        }

        fr->emission = emission;
        file_readers.push_back(fr);
        enum_or(flags, PARTICLE_MANAGER_READ_FILES);
        return effect_group_hash;
    }

    SceneCounter GltParticleManager::LoadScene(uint64_t effect_group_hash, uint64_t effect_hash, bool appear_now) {
        if (effect_group_hash == hash_murmurhash_empty || effect_group_hash == hash_fnv1a64m_empty)
            return LoadSceneEffect(effect_hash, appear_now);

        std::map<uint64_t, EffectGroup*>::iterator elem = effect_groups.find(effect_group_hash);
        if (elem == effect_groups.end())
            return false;

        EffectGroup* eff_group = elem->second;
        if (eff_group->not_loaded)
            return false;

        if (scenes.size())
            for (Scene*& i : scenes)
                if (i && i->hash == effect_group_hash) {
                    size_t id = 1;
                    for (Effect*& j : eff_group->effects)
                        if (j)
                            i->InitEffect(this, j, id++, appear_now);
                    CheckSceneHasLocalEffect(i);

                    SceneCounter counter = i->counter;
                    if (i->type == Glitter::X)
                        counter.index = (uint8_t)id;
                    return counter;
                }

        SceneCounter counter = GetSceneCounter();
        if (!counter)
            return 0;

        Scene* scene = new Scene(counter, effect_group_hash, eff_group, false);
        if (!scene)
            return 0;

        size_t id = 1;
        for (Effect*& i : eff_group->effects)
            if (i)
                scene->InitEffect(this, i, id++, appear_now);
        CheckSceneHasLocalEffect(scene);
        scenes.push_back(scene);
        return counter;
    }

    SceneCounter GltParticleManager::LoadSceneEffect(uint64_t hash, bool appear_now, uint8_t load_flags) {
        if (hash == hash_fnv1a64m_empty || hash == hash_murmurhash_empty)
            return 0;

        if (!(load_flags & 0x02) && scenes.size())
            for (Scene*& i : scenes)
                if (i && i->ResetEffect(this, hash)) {
                    CheckSceneHasLocalEffect(i);

                    size_t id = 0;
                    for (SceneEffect& j : i->effects)
                        if (j.ptr && j.disp && j.ptr->data.name_hash == hash) {
                            id = j.ptr->id;
                            break;
                        }

                    SceneCounter counter = i->counter;
                    if (i->type == Glitter::X)
                        counter.index = (uint8_t)id;
                    return counter;
                }

        for (std::pair<uint64_t, EffectGroup*> i : effect_groups) {
            EffectGroup* v8 = i.second;

            if (v8->effects.size() < 1)
                continue;

            bool found = false;
            for (Effect*& j : v8->effects)
                if (j && j->data.name_hash == hash) {
                    found = true;
                    break;
                }

            if (!found)
                continue;

            if (hash == hash_fnv1a64m_empty || hash == hash_murmurhash_empty)
                continue;

            if (v8->not_loaded)
                return 0;

            if (!(load_flags & 0x02))
                for (Scene*& j : scenes)
                    if (j && j->hash == i.first) {
                        size_t id = 1;
                        for (Effect*& k : v8->effects) {
                            if (!j)
                                continue;

                            if (k->data.name_hash == hash) {
                                j->InitEffect(this, k, id, appear_now, load_flags);
                                break;
                            }
                            id++;
                        }
                        CheckSceneHasLocalEffect(j);

                        SceneCounter counter = j->counter;
                        if (j->type == Glitter::X)
                            counter.index = (uint8_t)id;
                        return counter;
                    }

            SceneCounter counter = GetSceneCounter();
            if (!counter)
                return 0;

            Scene* scene = new Scene(counter, i.first, i.second, 0);
            if (!scene)
                return 0;

            size_t id = 1;
            for (Effect*& j : v8->effects) {
                if (!j)
                    continue;

                if (j->data.name_hash == hash) {
                    scene->InitEffect(this, j, id, appear_now);
                    break;
                }
                id++;
            }
            CheckSceneHasLocalEffect(scene);
            scenes.push_back(scene);

            if (scene->type == Glitter::X)
                counter.index = (uint8_t)id;
            return counter;
        }
        return 0;
    }

    SceneCounter GltParticleManager::LoadSceneEffect(uint64_t hash,
        const char* name, bool appear_now, uint8_t load_flags) {
        SceneCounter counter = LoadSceneEffect(hash, appear_now, load_flags);
        for (Scene*& i : scenes) {
            if (!i || i->counter.counter != counter.counter)
                continue;

            for (SceneEffect& j : i->effects)
                if (j.ptr && j.disp && j.ptr->id == counter.index) {
                    j.ptr->name.assign(name);
                    break;
                }
            break;
        }
        return counter;
    }

    bool GltParticleManager::SceneHasNotEnded(SceneCounter counter) {
        for (Scene*& i : scenes)
            if (i && i->counter.counter == counter.counter)
                return !i->HasEnded(true);
        return false;
    }

    void GltParticleManager::SetPause(bool value) {
        if (value)
            enum_or(flags, PARTICLE_MANAGER_PAUSE);
        else
            enum_and(flags, ~PARTICLE_MANAGER_PAUSE);
    }

    #if defined(CRE_DEV)
    void GltParticleManager::SetFrame(EffectGroup* effect_group,
        Scene** scene, float_t curr_frame, float_t prev_frame,
        uint32_t counter, Random* random, bool reset) {
        if (curr_frame < prev_frame || reset) {
            for (std::vector<Scene*>::iterator i = scenes.begin(); i != scenes.end();) {
                if (!*i || *i != *scene) {
                    i++;
                    continue;
                }

                delete* i;
                i = scenes.erase(i);
                break;
            }

            this->counter = counter;
            this->random = *random;

            LoadScene(effect_group->hash, effect_group->type != Glitter::FT
                ? hash_murmurhash_empty : hash_fnv1a64m_empty, false);
            *scene = GetScene(effect_group->hash);
            enum_or((*scene)->flags, SCENE_EDITOR);
            prev_frame = 0.0f;
        }

        float_t delta = curr_frame - prev_frame;
        float_t delta_frame = 1.0f;
        for (; delta > 0.0f; delta -= delta_frame) {
            if (delta_frame > delta)
                delta_frame = delta;

            Scene* s = *scene;
            if (s && !s->HasEnded(true))
                if (s->type == Glitter::F2) {
                    s->delta_frame_history += delta_frame;
                    if (!s->skip) {
                        s->Ctrl(this, s->delta_frame_history);
                        s->delta_frame_history = 0.0f;
                        s->skip = true;
                    }
                    else
                        s->skip = false;
                }
                else
                    s->Ctrl(this, delta_frame);
        }
    }
    #endif

    void GltParticleManager::SetSceneEffectExtColor(SceneCounter scene_counter, bool set,
        uint64_t effect_hash, float_t r, float_t g, float_t b, float_t a) {
        for (Scene*& i : scenes)
            if (i && i->counter.counter == scene_counter.counter) {
                if (effect_hash == hash_fnv1a64m_empty || effect_hash == hash_murmurhash_empty)
                    i->SetExtColorByID(set, scene_counter.index, r, g, b, a);
                else
                    i->SetExtColor(set, effect_hash, r, g, b, a);
                break;
            }
    }

    #if defined(CRE_DEV)
    void GltParticleManager::SetSceneEffectName(uint64_t effect_group_hash, uint64_t effect_hash, const char* name) {
        if (effect_group_hash == hash_fnv1a64m_empty || effect_group_hash == hash_murmurhash_empty)
            for (Scene*& i : scenes) {
                if (!i)
                    continue;

                for (SceneEffect& j : i->effects)
                    if (j.disp && j.ptr && j.ptr->data.name_hash == effect_hash) {
                        j.ptr->name = name;
                        return;
                    }
            }
        else
            for (Scene*& i : scenes) {
                if (!i || i->hash != effect_group_hash)
                    continue;

                for (SceneEffect& j : i->effects)
                    if (j.disp && j.ptr && j.ptr->data.name_hash == effect_hash) {
                        j.ptr->name = name;
                        return;
                    }
            }
    }

    void GltParticleManager::SetSceneEffectReqFrame(SceneCounter scene_counter, float_t req_frame) {
        for (Scene*& i : scenes)
            if (i && i->counter.counter == scene_counter.counter) {
                i->SetReqFrame(scene_counter.index, req_frame);
                break;
            }
    }

    void GltParticleManager::SetSceneFrameRate(SceneCounter scene_counter, FrameRateControl* frame_rate) {
        for (Scene*& i : scenes)
            if (i && i->counter.counter == scene_counter.counter) {
                i->SetFrameRate(frame_rate);
                break;
            }
    }

    void GltParticleManager::SetSceneName(uint64_t hash, const char* name) {
        std::map<uint64_t, EffectGroup*>::iterator elem = effect_groups.find(hash);
        if (elem != effect_groups.end())
            elem->second->name = name;

        for (Scene*& i : scenes) {
            if (!i || i->hash != hash)
                continue;

            i->name = name;
            break;
        }
    }
    #endif

    void GltParticleManager::UnloadEffectGroup(uint64_t hash) {
        for (std::vector<FileReader*>::iterator i = file_readers.begin(); i != file_readers.end();)
            if (!*i || (*i)->hash == hash) {
                delete* i;
                i = file_readers.erase(i);
            }
            else
                i++;

        std::map<uint64_t, EffectGroup*>::iterator elem = effect_groups.find(hash);
        if (elem != effect_groups.end())
            elem->second->load_count--;
    }

    void GltParticleManager::sub_1403A53E0(float_t a2) {
        //field_D4 = a2;
        //if (a2 <= 0.0)
        //    field_D4 = -1.0;
    }

    void glt_particle_manager_init() {
        glt_particle_manager = new GltParticleManager;
    }

    bool glt_particle_manager_add_task() {
        return app::TaskWork::AddTask(glt_particle_manager, "GLITTER_TASK", 2);
    }

    bool glt_particle_manager_del_task() {
        return glt_particle_manager->DelTask();
    }

    void glt_particle_manager_free() {
        if (glt_particle_manager) {
            delete glt_particle_manager;
            glt_particle_manager = 0;
        }
    }

}

