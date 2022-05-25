/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"

namespace Glitter {
    GltParticleManager glt_particle_manager;

    GltParticleManager::GltParticleManager() :scene(), effect(), emitter(), particle(), rctx(),
    bone_data(), frame_rate(), flags(), scene_load_counter(), emission(1.5f), delta_frame(2.0f),
    texture_counter(), random(), counter(), draw_all(true), draw_all_mesh(true), draw_selected() {
        scenes.reserve(0x100);
        file_readers.reserve(0x100);
    }

    GltParticleManager::~GltParticleManager() {
        FreeEffects();
    }

    bool GltParticleManager::Init() {
        frame_rate = &sys_frame_rate;
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

        if (~flags & PARTICLE_MANAGER_PAUSE) {
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

    #if defined(CRE_DEV)
    void GltParticleManager::Disp() {
        if (flags & PARTICLE_MANAGER_NOT_DISP)
            return;

        for (Scene*& i : scenes)
            if (i)
                i->CalcDisp(this);
    }
    #endif

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
                scene = *j;
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
    }

    uint64_t GltParticleManager::CalculateHash(const char* str) {
        return hash_utf8_fnv1a64m(str);
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

    int32_t GltParticleManager::CounterGet() {
        return counter;
    }

    void GltParticleManager::CounterIncrement() {
        counter++;
    }

    void GltParticleManager::CtrlScenes() {
        for (std::vector<Scene*>::iterator i = scenes.begin(); i != scenes.end();) {
            Scene* scene = *i;
            if (!scene || scene->HasEnded(true)) {
                delete scene;
                i = scenes.erase(i);
            }
    #if defined(CRE_DEV)
            else if (~scene->flags & SCENE_EDITOR) {
                float_t delta_frame = this->delta_frame;
                if (scene->frame_rate)
                    delta_frame *= scene->frame_rate->frame_speed;
                scene->Ctrl(this, delta_frame);
                i++;
            }
            else
                i++;
    #else
            else {
                scene->Ctrl(this, delta_frame);
                i++;
            }
    #endif
        }
    }

    void GltParticleManager::Disp(draw_pass_3d_type draw_pass_type) {
        if (flags & PARTICLE_MANAGER_NOT_DISP)
            return;

        for (Scene*& i : scenes) {
            if (!i)
                continue;

    #if defined(CRE_DEV)
            if (draw_selected && scene && scene != i)
                continue;
    #endif
            i->Disp(this, draw_pass_type);
        }
    }

    void GltParticleManager::FreeEffects() {
        scenes.clear();
        file_readers.clear();
    }

    void GltParticleManager::FreeSceneEffect(SceneCounter scene_counter, uint64_t hash, bool force_kill) {
        if (scene_counter) {
            for (std::vector<Scene*>::iterator i = scenes.begin(); i != scenes.end();) {
                Scene* scene = *i;
                if (!scene || scene_counter.counter != scene->counter.counter) {
                    i++;
                    continue;
                }

                if (!scene_counter.index || scene_counter.index
                    && (scene->FreeEffect(this, scene_counter.index, force_kill), scene->HasEnded(false))) {
                    delete scene;
                    i = scenes.erase(i);
                }
            }
        }
        else if (hash != hash_fnv1a64m_empty && hash != hash_murmurhash_empty) {
            for (Scene*& i : scenes)
                if (i && i->FreeEffect(this, hash, force_kill))
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
            if (i && i->counter == scene_counter)
                return i;
        return 0;
    }

    void GltParticleManager::GetStartEndFrame(int32_t* start_frame,
        int32_t* end_frame, uint64_t effect_group_hash) {
        if (start_frame)
            *start_frame = 0;

        if (end_frame)
            *end_frame = 0;

        if (!scenes.size())
            return;

        for (Scene*& i : scenes)
            if (i && i->hash == effect_group_hash) {
                if (start_frame)
                    *start_frame = INT16_MAX;
                i->GetStartEndFrame(start_frame, end_frame);
                return;
            }
    }

    void GltParticleManager::GetStartEndFrame(int32_t* start_frame,
        int32_t* end_frame, SceneCounter scene_counter) {
        if (start_frame)
            *start_frame = 0;

        if (end_frame)
            *end_frame = 0;

        if (!scenes.size())
            return;

        for (Scene*& i : scenes)
            if (i && i->counter == scene_counter) {
                if (start_frame)
                    *start_frame = INT16_MAX;
                i->GetStartEndFrame(start_frame, end_frame);
                return;
            }
    }

    float_t GltParticleManager::GetSceneFrameLifeTime(SceneCounter scene_counter, int32_t* life_time) {
        if (life_time)
            *life_time = 0;

        if (scenes.size() < 1)
            return 0.0f;

        Scene* scene = 0;
        for (Scene*& i : scenes)
            if (i && scene_counter == i->counter) {
                scene = i;
                break;
            }

        if (!scene)
            return 0.0f;

        float_t frame = 0.0f;
        if (life_time)
            for (SceneEffect& i : scene->effects) {
                if (!i.disp || !i.ptr)
                    continue;

                EffectInst* eff_inst = i.ptr;
                if (eff_inst->data.life_time > *life_time) {
                    if (frame < eff_inst->frame0)
                        frame = eff_inst->frame0;
                    *life_time = eff_inst->data.life_time;
                }
            }
        return frame;
    }

    SceneCounter GltParticleManager::GetSceneCounter(uint8_t index) {
        if (scene_load_counter + 1 < 0xFFFFFF && (~flags & PARTICLE_MANAGER_RESET_SCENE_COUNTER)) {
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
                    if (i && scene_load_counter == i->counter) {
                        found = true;
                        break;
                    }

                if (!found)
                    break;

                if (v7 >= 300)
                    return SceneCounter(0);
            }
        }
        return SceneCounter(index, counter);
    }

    SceneCounter GltParticleManager::Load(uint64_t effect_group_hash, uint64_t effect_hash, bool use_existing) {
        if (effect_group_hash == hash_fnv1a64m_empty || effect_group_hash == hash_murmurhash_empty) {
            if (effect_hash == hash_fnv1a64m_empty || effect_hash == hash_murmurhash_empty)
                return SceneCounter(0);

            if (use_existing)
                for (Scene*& i : scenes)
                    if (i && i->ResetEffect(this, effect_hash))
                        return i->counter;

            if (effect_groups.size())
                return SceneCounter(0);

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
                return SceneCounter(0);
        }

        std::map<uint64_t, EffectGroup*>::iterator elem = effect_groups.find(effect_group_hash);
        if (elem == effect_groups.end())
            return SceneCounter(0);

        EffectGroup* eff_group = elem->second;
        if (eff_group->not_loaded)
            return SceneCounter(0);

        if (use_existing)
            for (Scene*& i : scenes) {
                if (!i || i->hash != effect_group_hash)
                    continue;

                int32_t id = 1;
                if (effect_hash == hash_fnv1a64m_empty || effect_hash == hash_murmurhash_empty)
                    for (Effect*& j : eff_group->effects)
                        i->InitEffect(this, j, id++, false);
                else
                    for (Effect*& j : eff_group->effects) {
                        if (effect_hash == j->data.name_hash) {
                            i->InitEffect(this, j, id, false);
                            break;
                        }
                        id++;
                    }
                return i->counter;
            }

        SceneCounter counter = GetSceneCounter(0);
        if (!counter)
            return SceneCounter(0);

        Scene* scene = new Scene(counter, effect_group_hash, eff_group, 0);
        if (!scene)
            return SceneCounter(0);

        int32_t id = 1;
        if (effect_hash == hash_fnv1a64m_empty || effect_hash == hash_murmurhash_empty) {
            for (Effect*& i : eff_group->effects)
                if (i)
                    scene->InitEffect(this, i, id++, false);
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
        scenes.push_back(scene);
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

        Scene* scene = 0;
        if (scenes.size())
            for (Scene*& i : scenes)
                if (i && i->hash == effect_group_hash) {
                    int32_t id = 1;
                    for (Effect*& j : eff_group->effects)
                        if (j)
                            i->InitEffect(this, j, id++, appear_now);
                    return i->counter;
                }

        SceneCounter counter = GetSceneCounter(0);
        if (counter) {
            Scene* scene = new Scene(counter, effect_group_hash, eff_group, false);
            if (scene) {
                int32_t id = 1;
                for (Effect*& i : eff_group->effects)
                    if (i)
                        scene->InitEffect(this, i, id++, appear_now);
                scenes.push_back(scene);
                return counter;
            }
        }
        return SceneCounter(0);
    }

    SceneCounter GltParticleManager::LoadSceneEffect(uint64_t hash, bool appear_now) {
        if (hash == hash_fnv1a64m_empty || hash == hash_murmurhash_empty)
            return SceneCounter(0);

        if (scenes.size())
            for (Scene*& i : scenes)
                if (i && i->ResetEffect(this, hash))
                    return i->counter;

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
                return SceneCounter(0);

            for (Scene*& j : scenes)
                if (j && j->hash == i.first) {
                    size_t id = 1;
                    for (Effect*& k : v8->effects) {
                        if (!j)
                            continue;

                        if (k->data.name_hash == hash) {
                            j->InitEffect(this, k, id, appear_now);
                            break;
                        }
                        id++;
                    }
                    return j->counter;
                }

            SceneCounter counter = GetSceneCounter(0);
            if (!counter)
                return SceneCounter(0);

            Scene* scene = new Scene(counter, i.first, i.second, 0);
            if (!scene)
                return SceneCounter(0);

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
            scenes.push_back(scene);
            return counter;
        }
        return SceneCounter(0);
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

    void GltParticleManager::SetSceneEffectName(uint64_t hash, uint64_t effect_hash, const char* name) {
        if (hash == hash_fnv1a64m_empty || hash == hash_murmurhash_empty)
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
                if (!i || i->hash != hash)
                    continue;

                for (SceneEffect& j : i->effects)
                    if (j.disp && j.ptr && j.ptr->data.name_hash == effect_hash) {
                        j.ptr->name = name;
                        return;
                    }
            }
    }

    void GltParticleManager::SetSceneFrameRate(uint64_t hash, FrameRateControl* frame_rate) {
        for (Scene*& i : scenes)
            if (i && i->hash == hash)
                i->SetFrameRate(frame_rate);
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
}

