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

GltParticleManager::GltParticleManager() :scene(), effect(), emitter(), particle(), rctx(),
data(), bone_data(), sys_frame_rate(), flags(), scene_counter(), emission(1.5f), delta_frame(2.0f),
texture_counter(), random(), counter(), cam_projection(), cam_view(), cam_inv_view(), cam_inv_view_mat3(),
cam_view_point(), cam_rotation_y(), draw_all(true), draw_all_mesh(true), draw_selected() {
    scenes.reserve(0x100);
    file_readers.reserve(0x100);
}

GltParticleManager::~GltParticleManager() {
    FreeEffects();
}

bool GltParticleManager::Init() {
    this->sys_frame_rate = &::sys_frame_rate;
    return true;
}

bool GltParticleManager::Ctrl() {
    if (flags & GLITTER_PARTICLE_MANAGER_READ_FILES) {
        for (glitter_file_reader** i = file_readers.begin()._Ptr; i != file_readers.end()._Ptr;)
            if (!*i || (*i)->ReadFarc(this, emission)) {
                delete* i;
                *i = 0;
                file_readers.erase(file_readers.begin() + (i - file_readers.data()));
            }
            else
                i++;

        if (!file_readers.size())
            enum_and(flags, ~GLITTER_PARTICLE_MANAGER_READ_FILES);
    }

    if (~flags & GLITTER_PARTICLE_MANAGER_PAUSE)
        CtrlScenes();
    return false;
}

bool GltParticleManager::Dest() {
    return true;
}

#if defined(CRE_DEV)
void GltParticleManager::Disp() {
    if (flags & GLITTER_PARTICLE_MANAGER_NOT_DISP)
        return;

    for (glitter_scene*& i : scenes)
        if (i)
            glitter_scene_calc_disp(this, i);
}
#endif

void GltParticleManager::Basic() {
    BasicEffectGroups();
}

bool GltParticleManager::AppendEffectGroup(uint64_t hash, glitter_effect_group* eff_group, glitter_file_reader* file_read) {
    if (effect_groups.find(hash) != effect_groups.end())
        return false;

    eff_group->load_count = file_read->load_count;
    eff_group->emission = file_read->emission;
    if (file_read->emission <= 0.0f)
        eff_group->emission = emission;

    effect_groups.insert({ hash, eff_group });
    return true;
}

void GltParticleManager::BasicEffectGroups() {
    for (std::map<uint64_t, glitter_effect_group*>::iterator i = effect_groups.begin(); i != effect_groups.end(); ) {
        if (i->second->load_count > 0) {
            i++;
            continue;
        }

        uint64_t hash = i->first;
        for (glitter_scene** j = scenes.begin()._Ptr; j != scenes.end()._Ptr;) {
            scene = *j;
            if (!scene || scene->hash != hash) {
                j++;
                continue;
            }

            delete* j;
            *j = 0;
            scenes.erase(scenes.begin() + (j - scenes.data()));
        }
        i = effect_groups.erase(i);
    }
}

uint64_t GltParticleManager::CalculateHash(const char* str) {
    return hash_utf8_fnv1a64m(str, false);
}

bool GltParticleManager::CheckNoFileReaders(uint64_t hash) {
    std::map<uint64_t, glitter_effect_group*>::iterator elem = effect_groups.find(hash);
    if (elem != effect_groups.end()) {
        if (file_readers.size())
            return true;

        for (glitter_file_reader*& i : file_readers)
            if (i && i->hash == hash)
                return false;
        return true;
    }
    return false;
}

void GltParticleManager::CtrlScenes() {
    delta_frame = get_delta_frame();
    if (sys_frame_rate)
        delta_frame *= sys_frame_rate->frame_speed;

    for (glitter_scene** i = scenes.begin()._Ptr; i != scenes.end()._Ptr;) {
        if (!*i || glitter_scene_has_ended(*i, 1)) {
            delete* i;
            *i = 0;
            scenes.erase(scenes.begin() + (i - scenes.data()));
        }
#if defined(CRE_DEV)
        else if (~(*i)->flags & GLITTER_SCENE_EDITOR) {
            glitter_scene_ctrl(this, *i, delta_frame);
            i++;
        }
        else
            i++;
#else
        else {
            glitter_scene_ctrl(this, *i, delta_frame);
            i++;
        }
#endif
    }
}

void GltParticleManager::Disp(draw_pass_3d_type draw_pass_type) {
    if (flags & GLITTER_PARTICLE_MANAGER_NOT_DISP)
        return;

    for (glitter_scene*& i : scenes) {
        if (!i)
            continue;

#if defined(CRE_DEV)
        if (draw_selected && scene && scene != i)
            continue;
#endif
        glitter_scene_disp(this, i, draw_pass_type);
    }
}

void GltParticleManager::FreeEffects() {
    scenes.clear();
    file_readers.clear();
}

void GltParticleManager::FreeSceneEffect(glitter_scene_counter scene_counter, uint64_t hash) {
    if (scene_counter) {
        for (glitter_scene** i = scenes.begin()._Ptr; i != scenes.end()._Ptr;) {
            if (!*i || scene_counter.counter != (*i)->counter.counter) {
                i++;
                continue;
            }

            if (!scene_counter.index || scene_counter.index
                && (glitter_scene_free_effect(this, *i, scene_counter.index, true), glitter_scene_has_ended(*i, 0))) {
                delete *i;
                *i = 0;
                scenes.erase(scenes.begin() + (i - scenes.data()));
            }
        }
    }
    else if (hash != hash_fnv1a64m_empty && hash != hash_murmurhash_empty) {
        for (glitter_scene*& i : scenes)
            if (i && glitter_scene_free_effect(this, i, hash, true))
                break;
    }
}

void GltParticleManager::FreeScenes() {
    for (glitter_scene*& i : scenes)
        if (i)
            glitter_scene_free_effect(this, i, i->type == GLITTER_FT
                ? hash_fnv1a64m_empty : hash_murmurhash_empty, true);
}

size_t GltParticleManager::GetCtrlCount(glitter_particle_type type) {
    size_t ctrl = 0;
    for (glitter_scene*& i : scenes)
        if (i)
            ctrl += glitter_scene_get_ctrl_count(i, type);
    return ctrl;
}

size_t GltParticleManager::GetDispCount(glitter_particle_type type) {
    size_t disp = 0;
    for (glitter_scene*& i : scenes)
        if (i)
            disp += glitter_scene_get_disp_count(i, type);
    return disp;
}

glitter_effect_group* GltParticleManager::GetEffectGroup(uint64_t hash) {
    std::map<uint64_t, glitter_effect_group*>::iterator elem = effect_groups.find(hash);
    if (elem != effect_groups.end())
        return elem->second;
    return 0;
}

const char* GltParticleManager::GetEffectName(uint64_t hash, int32_t index) {
    std::map<uint64_t, glitter_effect_group*>::iterator elem = effect_groups.find(hash);
    if (elem == effect_groups.end())
        return 0;

    glitter_effect_group* eff_group = elem->second;
    if (index >= vector_old_length(eff_group->effects))
        return 0;
    return eff_group->effects.begin[index]->name;
}

size_t GltParticleManager::GetEffectsCount(uint64_t hash) {
    std::map<uint64_t, glitter_effect_group*>::iterator elem = effect_groups.find(hash);
    if (elem == effect_groups.end())
        return 0;

    glitter_effect_group* eff_group = elem->second;
    return vector_old_length(eff_group->effects);
}

bool GltParticleManager::GetPause() {
    return flags & GLITTER_PARTICLE_MANAGER_PAUSE ? true : false;
}

glitter_scene* GltParticleManager::GetScene(uint64_t effect_group_hash) {
    for (glitter_scene*& i : scenes)
        if (i && i->hash == effect_group_hash)
            return i;
    return 0;
}

glitter_scene* GltParticleManager::GetScene(glitter_scene_counter scene_counter) {
    for (glitter_scene*& i : scenes)
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

    for (glitter_scene*& i : scenes)
        if (i && i->hash == effect_group_hash) {
            if (start_frame)
                *start_frame = INT16_MAX;
            glitter_scene_get_start_end_frame(i, start_frame, end_frame);
            return;
        }
}

void GltParticleManager::GetStartEndFrame(int32_t* start_frame,
    int32_t* end_frame, glitter_scene_counter scene_counter) {
    if (start_frame)
        *start_frame = 0;

    if (end_frame)
        *end_frame = 0;

    if (!scenes.size())
        return;

    for (glitter_scene*& i : scenes)
        if (i && i->counter == scene_counter) {
            if (start_frame)
                *start_frame = INT16_MAX;
            glitter_scene_get_start_end_frame(i, start_frame, end_frame);
            return;
        }
}

float_t GltParticleManager::GetSceneFrameLifeTime(glitter_scene_counter scene_counter, int32_t* life_time) {
    if (life_time)
        *life_time = 0;

    if (scenes.size() < 1)
        return 0.0f;

    glitter_scene* scene = 0;
    for (glitter_scene*& i : scenes)
        if (i && scene_counter == i->counter) {
            scene = i;
            break;
        }

    if (!scene)
        return 0.0f;

    float_t frame = 0.0f;
    if (life_time)
        for (glitter_scene_effect* i = scene->effects.begin; i != scene->effects.end; i++) {
            if (!i->disp || !i->ptr)
                continue;

            glitter_effect_inst* eff_inst = i->ptr;
            if (eff_inst->data.life_time > *life_time) {
                if (frame < eff_inst->frame0)
                    frame = eff_inst->frame0;
                *life_time = eff_inst->data.life_time;
            }
        }
    return frame;
}

glitter_scene_counter GltParticleManager::GetSceneCounter(uint8_t index) {
    if (scene_counter + 1 < 0xFFFFFF && (~flags & GLITTER_PARTICLE_MANAGER_RESET_SCENE_COUNTER)) {
        scene_counter++;
        return glitter_scene_counter(index, scene_counter);
    }

    if (scenes.size()) {
        enum_and(flags, ~GLITTER_PARTICLE_MANAGER_RESET_SCENE_COUNTER);
        scene_counter = 1;
        return glitter_scene_counter(index, scene_counter);
    }

    uint32_t counter = 0;
    for (glitter_scene*& i : scenes)
        if (i && counter < i->counter)
            counter = i->counter;
    counter++;

    if (counter < 0xFFFFFF)
        enum_and(flags, ~GLITTER_PARTICLE_MANAGER_RESET_SCENE_COUNTER);
    else {
        enum_or(flags, GLITTER_PARTICLE_MANAGER_RESET_SCENE_COUNTER);
        for (uint32_t v7 = 1; ; v7++) {
            bool found = false;
            for (glitter_scene*& i : scenes)
                if (i && scene_counter == i->counter) {
                    found = true;
                    break;
                }

            if (!found)
                break;

            if (v7 >= 300)
                return glitter_scene_counter(0);
        }
    }
    return glitter_scene_counter(index, counter);
}

glitter_scene_counter GltParticleManager::Load(uint64_t effect_group_hash, uint64_t effect_hash, bool use_existing) {
    if (effect_group_hash == hash_fnv1a64m_empty || effect_group_hash == hash_murmurhash_empty) {
        if (effect_hash == hash_fnv1a64m_empty || effect_hash == hash_murmurhash_empty)
            return glitter_scene_counter(0);

        if (use_existing)
            for (glitter_scene*& i : scenes)
                if (i && glitter_scene_reset_effect(this, i, effect_hash))
                    return i->counter;

        if (effect_groups.size())
            return glitter_scene_counter(0);

        bool found = false;
        for (std::pair<uint64_t, glitter_effect_group*> i : effect_groups) {
            for (glitter_effect** j = i.second->effects.begin; j != i.second->effects.end; j++)
                if ((*j)->data.name_hash == effect_hash
                    && i.first != hash_fnv1a64m_empty && i.first != hash_murmurhash_empty) {
                    found = true;
                    break;
                }

            if (found)
                break;
        }

        if (!found)
            return glitter_scene_counter(0);
    }

    std::map<uint64_t, glitter_effect_group*>::iterator elem = effect_groups.find(effect_group_hash);
    if (elem == effect_groups.end())
        return glitter_scene_counter(0);

    glitter_effect_group* eff_group = elem->second;
    if (eff_group->field_3C)
        return glitter_scene_counter(0);

    if (use_existing)
        for (glitter_scene*& i : scenes) {
            if (!i || i->hash != effect_group_hash)
                continue;

            int32_t id = 1;
            if (effect_hash == hash_fnv1a64m_empty || effect_hash == hash_murmurhash_empty)
                for (glitter_effect** j = eff_group->effects.begin; j != eff_group->effects.end; j++, id++)
                    glitter_scene_init_effect(this, i, *j, id, false);
            else
                for (glitter_effect** j = eff_group->effects.begin; j != eff_group->effects.end; j++, id++)
                    if (effect_hash == (*j)->data.name_hash) {
                        glitter_scene_init_effect(this, i, *j, id, false);
                        break;
                    }
            return i->counter;
        }

    glitter_scene_counter counter = GetSceneCounter(0);
    if (!counter)
        return glitter_scene_counter(0);

    glitter_scene* scene = new glitter_scene(counter, effect_group_hash, eff_group, 0);
    if (!scene)
        return glitter_scene_counter(0);

    int32_t id = 1;
    if (effect_hash == hash_fnv1a64m_empty || effect_hash == hash_murmurhash_empty)
        for (glitter_effect** i = eff_group->effects.begin; i != eff_group->effects.end; i++, id++)
            glitter_scene_init_effect(this, scene, *i, id, false);
    else
        for (glitter_effect** i = eff_group->effects.begin; i != eff_group->effects.end; i++, id++)
            if (effect_hash == (*i)->data.name_hash) {
                glitter_scene_init_effect(this, scene, *i, id, false);
                break;
            }
    scenes.push_back(scene);
    return counter;
}

uint64_t GltParticleManager::LoadFile(GLT, const char* file, const char* path, float_t emission, bool init_scene) {
    uint64_t effect_group_hash;
    uint64_t empty_hash;
    if (GLT_VAL == GLITTER_FT) {
        effect_group_hash = hash_utf8_fnv1a64m(file, false);
        if (effect_group_hash == hash_fnv1a64m_empty)
            return effect_group_hash;
        empty_hash = hash_fnv1a64m_empty;
    }
    else {
        effect_group_hash = hash_utf8_murmurhash(file, 0, false);
        if (effect_group_hash == hash_murmurhash_empty)
            return effect_group_hash;
        empty_hash = hash_murmurhash_empty;
    }

    std::map<uint64_t, glitter_effect_group*>::iterator elem = effect_groups.find(effect_group_hash);
    if (elem != effect_groups.end()) {
        glitter_effect_group* eff_group = elem->second;
        eff_group->emission = emission;
        if (eff_group->emission <= 0.0f)
            eff_group->emission = this->emission;
        eff_group->load_count++;
        return effect_group_hash;
    }

    for (glitter_file_reader*& i : file_readers)
        if (i && i->hash == effect_group_hash) {
            i->load_count++;
            i->emission = emission;
            return effect_group_hash;
        }

    glitter_file_reader* fr = new glitter_file_reader(GLT_VAL);
    if (!fr)
        return empty_hash;

    fr->init_scene = init_scene;

    if (!path)
        path = "rom/particle/";

    if (!glitter_file_reader_load_farc(fr, path, file, effect_group_hash)) {
        delete fr;
        return empty_hash;
    }

    fr->emission = emission;
    file_readers.push_back(fr);
    enum_or(flags, GLITTER_PARTICLE_MANAGER_READ_FILES);
    return effect_group_hash;
}

glitter_scene_counter GltParticleManager::LoadScene(uint64_t effect_group_hash, uint64_t effect_hash, bool appear_now) {
    if (effect_group_hash == hash_murmurhash_empty || effect_group_hash == hash_fnv1a64m_empty)
        return LoadSceneEffect(effect_hash, appear_now);

    std::map<uint64_t, glitter_effect_group*>::iterator elem = effect_groups.find(effect_group_hash);
    if (elem == effect_groups.end())
        return false;

    glitter_effect_group* eff_group = elem->second;
    if (eff_group->field_3C)
        return false;

    glitter_scene* scene = 0;
    if (scenes.size())
        for (glitter_scene*& i : scenes)
            if (i && i->hash == effect_group_hash) {
                int32_t id = 1;
                for (glitter_effect** j = eff_group->effects.begin; j != eff_group->effects.end; j++)
                    if (*j)
                        glitter_scene_init_effect(this, i, *j, id++, appear_now);
                return i->counter;
            }

    glitter_scene_counter counter = GetSceneCounter(0);
    if (counter) {
        glitter_scene* scene = new glitter_scene(counter, effect_group_hash, eff_group, false);
        if (scene) {
            int32_t id = 1;
            for (glitter_effect** i = eff_group->effects.begin; i != eff_group->effects.end; i++)
                if (*i)
                    glitter_scene_init_effect(this, scene, *i, id++, appear_now);
            scenes.push_back(scene);
            return counter;
        }
    }
    return glitter_scene_counter(0);
}

glitter_scene_counter GltParticleManager::LoadSceneEffect(uint64_t hash, bool appear_now) {
    if (hash == hash_fnv1a64m_empty || hash == hash_murmurhash_empty)
        return glitter_scene_counter(0);

    if (scenes.size())
        for (glitter_scene*& i : scenes)
            if (i && glitter_scene_reset_effect(this, i, hash))
                return i->counter;

    for (std::pair<uint64_t, glitter_effect_group*> i : effect_groups) {
        glitter_effect_group* v8 = i.second;

        if (vector_old_length(v8->effects) < 1)
            continue;

        bool found = false;
        for (glitter_effect** j = v8->effects.begin; j != v8->effects.end; j++)
            if ((*j) && (*j)->data.name_hash == hash) {
                found = true;
                break;
            }

        if (!found)
            break;

        if (hash == hash_fnv1a64m_empty || hash == hash_murmurhash_empty)
            continue;

        if (v8->field_3C)
            return glitter_scene_counter(0);

        for (glitter_scene*& j : scenes)
            if (j && j->hash == i.first) {
                size_t id = 1;
                for (glitter_effect** k = v8->effects.begin; k != v8->effects.end; k++)
                    if ((*k) && (*k)->data.name_hash == hash)
                        glitter_scene_init_effect(this, j, *k, id++, appear_now);
                return j->counter;
            }

        glitter_scene_counter counter = GetSceneCounter(0);
        if (!counter)
            return glitter_scene_counter(0);

        glitter_scene* scene = new glitter_scene(counter, i.first, i.second, 0);
        if (!scene)
            return glitter_scene_counter(0);

        size_t id = 1;
        for (glitter_effect** k = v8->effects.begin; k != v8->effects.end; k++)
            if ((*k) && (*k)->data.name_hash == hash)
                glitter_scene_init_effect(this, scene, *k, id++, appear_now);
        scenes.push_back(scene);
        return counter;
    }
    return glitter_scene_counter(0);
}

bool GltParticleManager::SceneHasNotEnded(glitter_scene_counter counter) {
    for (glitter_scene*& i : scenes)
        if (i && i->counter.counter == counter.counter)
            return !glitter_scene_has_ended(i, true);
    return true;
}

void GltParticleManager::SetPause(bool value) {
    if (value)
        enum_or(flags, GLITTER_PARTICLE_MANAGER_PAUSE);
    else
        enum_and(flags, ~GLITTER_PARTICLE_MANAGER_PAUSE);
}

#if defined(CRE_DEV)
void GltParticleManager::SetFrame(glitter_effect_group* effect_group,
    glitter_scene** scene, float_t curr_frame, float_t prev_frame,
    uint32_t counter, glitter_random* random, bool reset) {
    if (curr_frame < prev_frame || reset) {
        for (glitter_scene** i = scenes.begin()._Ptr; i != scenes.end()._Ptr;) {
            if (!*i || *i != *scene) {
                i++;
                continue;
            }

            scenes.erase(scenes.begin() + (i - scenes.data()));
            break;
        }

        this->counter = counter;
        this->random = *random;

        LoadScene(effect_group->hash, effect_group->type != GLITTER_FT
            ? hash_murmurhash_empty : hash_fnv1a64m_empty, false);
        *scene = GetScene(effect_group->hash);
        enum_or((*scene)->flags, GLITTER_SCENE_EDITOR);
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
                    glitter_scene_ctrl(this, s, s->delta_frame_history);
                    s->delta_frame_history = 0.0f;
                    s->skip = true;
                }
                else
                    s->skip = false;
            }
            else
                glitter_scene_ctrl(this, s, delta_frame);
    }
}
#endif

void GltParticleManager::UnloadEffectGroup(uint64_t hash) {
    for (glitter_file_reader** i = file_readers.begin()._Ptr; i != file_readers.end()._Ptr;)
        if (!*i || (*i)->hash == hash) {
            delete* i;
            *i = 0;
            file_readers.erase(file_readers.begin() + (i - file_readers.data()));
        }
        else
            i++;

    std::map<uint64_t, glitter_effect_group*>::iterator elem = effect_groups.find(hash);
    if (elem != effect_groups.end())
        elem->second->load_count--;
}

void GltParticleManager::sub_1403A53E0(float_t a2) {
    /*field_D4 = a2;
    if (a2 <= 0.0)
        field_D4 = -1.0;*/
}
