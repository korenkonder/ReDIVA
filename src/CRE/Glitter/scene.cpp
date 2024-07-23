/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"

namespace Glitter {
    Scene::Scene(SceneCounter counter, uint64_t hash, EffectGroup* eff_group, bool a5) {
        this->counter = counter;
        this->hash = hash;
        flags = SCENE_NONE;
        emission = 1.0f;
        type = eff_group->type;
        effect_group = eff_group;
        delta_frame_history = 0.0f;
        skip = false;
        fade_frame = -1.0f;
        fade_frame_left = -1.0f;
        frame_rate = 0;

        if (eff_group) {
            effects.reserve(eff_group->effects.size());
            emission = eff_group->emission;
            if (a5)
                enum_or(flags, SCENE_FLAG_3);
        }
    }

    Scene::~Scene() {
        for (SceneEffect& i : effects)
            delete i.ptr;
    }

    bool Scene::Copy(Glitter::EffectInst* eff_inst, Glitter::Scene* dst) {
        if (!(flags & SCENE_FLAG_3))
            return false;

        for (SceneEffect& i : effects)
            if (i.ptr && i.disp && i.ptr->id == eff_inst->id) {
                i.ptr->Copy(eff_inst, dst->emission);
                return true;
            }
        return false;
    }

    void Scene::CalcDisp(GPM) {
        for (SceneEffect& i : effects)
            if (i.ptr && i.disp) {
                if (GPM_VAL->draw_selected && GPM_VAL->selected_effect
                    && GPM_VAL->selected_effect != i.ptr)
                    continue;

                i.ptr->CalcDisp(GPM_VAL);
            }
    }

    bool Scene::CanDisp(DispType disp_type, bool a3) {
        for (SceneEffect& i : effects)
            if (i.ptr && i.disp) {
                XEffectInst* eff_x = dynamic_cast<XEffectInst*>(i.ptr);
                if (eff_x && eff_x->render_scene.CanDisp(disp_type, a3))
                    return true;
            }
        return false;
    }

    void Scene::Ctrl(GPM, float_t delta_frame) {
        static int32_t call_count;
        for (SceneEffect& i : effects) {
            if (!i.ptr || !i.disp)
                continue;

            EffectInst* eff = i.ptr;
            bool v13 = false;
            bool v14 = false;
            if (v14)
                v13 = true;

            float_t _delta_frame = delta_frame;
            if (v14)
                _delta_frame = 0.0f;

            float_t req_frame = eff->req_frame;
            eff->req_frame = 0.0f;

            req_frame -= _delta_frame;
            while (true) {
                while (true) {
                    if (v13 || _delta_frame > 0.0f)
                        eff->Ctrl(GPM_VAL, type, _delta_frame, emission);

                    if (req_frame <= 10.0f)
                        break;

                    _delta_frame = 10.0f;
                    req_frame -= 10.0f;
                }

                if (req_frame <= 0.0f)
                    break;

                _delta_frame = req_frame;
                req_frame = -1.0f;
            }

            //eff->Ctrl(GPM_VAL, type, delta_frame, emission);
        }
    }

    void Scene::Disp(GPM, DispType disp_type) {
        if (flags & SCENE_NOT_DISP)
            return;

        for (SceneEffect& i : effects)
            if (i.ptr && i.disp) {
                if (GPM_VAL->draw_selected && GPM_VAL->selected_effect
                    && GPM_VAL->selected_effect != i.ptr)
                    continue;

                i.ptr->Disp(GPM_VAL, disp_type);
            }
    }

    size_t Scene::GetCtrlCount(ParticleType ptcl_type) {
        size_t ctrl = 0;
        for (SceneEffect& i : effects)
            if (i.ptr && i.disp)
                ctrl += i.ptr->GetCtrlCount(ptcl_type);
        return ctrl;
    }

    size_t Scene::GetDispCount(ParticleType ptcl_type) {
        size_t disp = 0;
        for (SceneEffect& i : effects)
            if (i.ptr && i.disp)
                disp += i.ptr->GetDispCount(ptcl_type);
        return disp;
    }

    float_t Scene::GetFrameLifeTime(int32_t* life_time, size_t id) {
        float_t frame = 0.0f;
        if (!id)
            for (SceneEffect& i : effects) {
                if (!i.disp || !i.ptr)
                    continue;

                EffectInst* eff_inst = i.ptr;
                if (frame < eff_inst->frame0)
                    frame = eff_inst->frame0;

                if (life_time && *life_time < eff_inst->data.life_time)
                    *life_time = eff_inst->data.life_time;
            }
        else
            for (SceneEffect& i : effects) {
                if (!i.disp || !i.ptr || !i.ptr->id == id)
                    continue;

                EffectInst* eff_inst = i.ptr;
                if (frame < eff_inst->frame0)
                    frame = eff_inst->frame0;

                if (life_time && *life_time < eff_inst->data.life_time)
                    *life_time = eff_inst->data.life_time;
                break;
            }
        return frame;
    }

    bool Scene::FreeEffect(GPM, uint64_t effect_hash, bool free) {
        if (type == Glitter::FT && effect_hash == hash_fnv1a64m_empty
            || type != Glitter::FT && effect_hash == hash_murmurhash_empty) {
            for (SceneEffect& i : effects)
                if (i.disp && i.ptr)
                    i.ptr->Free(GPM_VAL, type, emission, free);
            return true;
        }

        for (SceneEffect& i : effects)
            if (i.disp && i.ptr && i.ptr->data.name_hash == effect_hash) {
                i.ptr->Free(GPM_VAL, type, emission, free);
                return true;
            }
        return false;
    }

    bool Scene::FreeEffectByID(GPM, size_t id, bool free) {
        if (!id) {
            for (SceneEffect& i : effects)
                if (i.disp && i.ptr)
                    i.ptr->Free(GPM_VAL, type, emission, free);
            return true;
        }

        for (SceneEffect& i : effects)
            if (i.disp && i.ptr && i.ptr->id == id) {
                i.ptr->Free(GPM_VAL, type, emission, free);
                return true;
            }
        return false;
    }

    bool Scene::HasEnded(bool a2) {
        for (SceneEffect& i : effects)
            if (i.ptr && i.disp && !i.ptr->HasEnded(a2))
                return false;
        return true;
    }

    bool Scene::HasEnded(size_t id, bool a3) {
        for (SceneEffect& i : effects)
            if (i.ptr && i.disp && i.ptr->id == id && !i.ptr->HasEnded(a3))
                return false;
        return true;
    }

    void Scene::InitEffect(GPM, Effect* eff, size_t id, bool appear_now, uint8_t load_flags) {
        if (!eff)
            return;

        bool init = !!(flags & SCENE_FLAG_3);
        if (!init)
            for (SceneEffect& i : effects)
                if (i.ptr && i.disp && i.ptr->id == id) {
                    i.ptr->Reset(GPM_VAL, type, this);
                    return;
                }

        SceneEffect effect;
        if (type != Glitter::X)
            effect.ptr = new F2EffectInst(GPM_VAL, type, eff, id, this, appear_now, init);
        else
            effect.ptr = new XEffectInst(GPM_VAL, eff, id, this, appear_now, init, load_flags);
        effect.disp = true;
        effects.push_back(effect);

        if (type == Glitter::X)
            enum_and(flags, ~SCENE_ENDED);
    }

    bool Scene::ResetCheckInit(GPM, float_t* init_delta_frame) {
        if (!(flags & SCENE_FLAG_3) || !effects.size())
            return false;

        for (SceneEffect& i : effects)
            if (i.disp && i.ptr && i.ptr->ResetCheckInit(GPM_VAL, type, this, init_delta_frame))
                return true;
        return false;
    }

    bool Scene::ResetEffect(GPM, uint64_t effect_hash, size_t* id) {
        if (type == Glitter::X) {
            if (effect_hash == hash_murmurhash_empty) {
                for (SceneEffect& i : effects)
                    if (i.ptr)
                        delete i.ptr;
                effects.clear();
            }
            else {
                for (auto i = effects.begin(); i != effects.end();)
                    if (!i->ptr) {
                        delete i->ptr;
                        i = effects.erase(i);
                    }
                    else if (i->ptr->data.name_hash == effect_hash) {
                        if (id)
                            *id = i->ptr->id;

                        delete i->ptr;
                        i = effects.erase(i);
                        break;
                    }
                    else
                        i++;
            }
            return false;
        }

        if (type == Glitter::FT && effect_hash == hash_fnv1a64m_empty
            || type != Glitter::FT && effect_hash == hash_murmurhash_empty) {
            for (SceneEffect& i : effects)
                if (i.disp && i.ptr)
                    i.ptr->Reset(GPM_VAL, type, this);
            return true;
        }
        else
            for (SceneEffect& i : effects)
                if (i.disp && i.ptr && i.ptr->data.name_hash == effect_hash) {
                    i.ptr->Reset(GPM_VAL, type, this);

                    enum_and(flags, ~SCENE_ENDED);
                    return true;
                }
        return false;
    }

    bool Scene::SetExtColor(bool set, uint64_t effect_hash, float_t r, float_t g, float_t b, float_t a) {
        if (type == Glitter::FT && effect_hash == hash_fnv1a64m_empty
            || type != Glitter::FT && effect_hash == hash_murmurhash_empty) {
            for (SceneEffect& i : effects)
                if (i.disp && i.ptr)
                    i.ptr->SetExtColor(set, r, g, b, a);
            return true;
        }
        else
            for (SceneEffect& i : effects)
                if (i.disp && i.ptr && i.ptr->data.name_hash == effect_hash) {
                    i.ptr->SetExtColor(set, r, g, b, a);
                    return true;
                }
        return false;
    }

    bool Scene::SetExtColorByID(bool set, size_t id, float_t r, float_t g, float_t b, float_t a) {
        if (!id) {
            for (SceneEffect& i : effects)
                if (i.disp && i.ptr)
                    i.ptr->SetExtColor(set, r, g, b, a);
            return true;
        }
        else
            for (SceneEffect& i : effects)
                if (i.disp && i.ptr && i.ptr->id == id) {
                    i.ptr->SetExtColor(set, r, g, b, a);
                    return true;
                }
        return false;
    }

    void Scene::SetFrameRate(FrameRateControl* frame_rate) {
        this->frame_rate = frame_rate;
    }

    void Scene::SetReqFrame(size_t id, float_t req_frame) {
        if (!id) {
            for (SceneEffect& i : effects)
                if (i.disp && i.ptr)
                    i.ptr->req_frame = req_frame;
        }
        else
            for (SceneEffect& i : effects)
                if (i.disp && i.ptr && i.ptr->id == id) {
                    i.ptr->req_frame = req_frame;
                    break;
                }
    }

    SceneCounter::SceneCounter(uint32_t counter) {
        this->index = 0;
        this->counter = counter;
    }

    SceneCounter::SceneCounter(uint32_t index, uint32_t counter) {
        this->index = index;
        this->counter = counter;
    }
}
