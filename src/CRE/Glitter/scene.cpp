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
        delta_frame_history = 0.0f;
        fade_frame_left = -1.0f;
        fade_frame = -1.0f;
        skip = false;
        type = eff_group->type;
        effect_group = eff_group;
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
                    && GPM_VAL->selected_effect != i.ptr->effect)
                    continue;

                i.ptr->CalcDisp(GPM_VAL);
            }

#if SHARED_GLITTER_BUFFER
        size_t disp = 0;
        if (type != Glitter::X)
            for (SceneEffect& i : effects) {
                if (!i.ptr || !i.disp)
                    continue;

                F2EffectInst* eff_inst = (F2EffectInst*)i.ptr;
                disp += eff_inst->render_scene.disp_quad;
                disp += eff_inst->render_scene.disp_locus;
                disp += eff_inst->render_scene.disp_line;
            }
        else
            for (SceneEffect& i : effects) {
                if (!i.ptr || !i.disp)
                    continue;

                XEffectInst* eff_inst = (XEffectInst*)i.ptr;
                disp += eff_inst->render_scene.disp_quad;
                disp += eff_inst->render_scene.disp_locus;
                disp += eff_inst->render_scene.disp_line;
            }

        if (disp)
            effect_group->vbo.WriteMemory(0, effect_group->max_count * sizeof(Buffer), effect_group->buffer);
#endif
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

    void Scene::CheckUpdate(float_t delta_frame) {
        if (delta_frame > 0.0f)
            enum_and(flags, ~SCENE_PAUSE);
        else
            enum_or(flags, SCENE_PAUSE);

        for (SceneEffect& i : effects)
            if (i.ptr && i.disp)
                ((XEffectInst*)i.ptr)->CheckUpdate();
    }

    void Scene::Ctrl(GPM, float_t delta_frame) {
        if (type != Glitter::X) {
            for (SceneEffect& i : effects)
                if (i.ptr && i.disp)
                    ((F2EffectInst*)i.ptr)->Ctrl(GPM_VAL, type, delta_frame);

            for (SceneEffect& i : effects)
                if (i.ptr && i.disp)
                    ((F2EffectInst*)i.ptr)->Emit(GPM_VAL, type, delta_frame, emission);

            for (SceneEffect& i : effects)
                if (i.ptr && i.disp)
                    ((F2EffectInst*)i.ptr)->RenderSceneCtrl(type, delta_frame);
            return;
        }

        if (delta_frame < 0.0f)
            return;

        for (SceneEffect& i : effects) {
            if (!i.ptr || !i.disp)
                continue;

            XEffectInst* eff = (XEffectInst*)i.ptr;

            bool step = false;
            bool just_init = !!(eff->flags & EFFECT_INST_JUST_INIT);
            if (!(flags & SCENE_PAUSE) || just_init)
                step = true;

            float_t _delta_frame = 0.0f;
            if (!just_init)
                _delta_frame = delta_frame;

            float_t req_frame = eff->req_frame;
            eff->req_frame = 0.0f;

            float_t remain_frame = req_frame - _delta_frame;
            while (true) {
                if (step || _delta_frame > 0.0f) {
                    eff->CtrlFlags(_delta_frame);
                    eff->Ctrl(GPM_VAL, _delta_frame);
                    eff->Emit(GPM_VAL, _delta_frame, emission);
                    eff->RenderSceneCtrl(_delta_frame);
                }

                if (remain_frame > 10.0f) {
                    _delta_frame = 10.0f;
                    remain_frame -= 10.0f;
                }
                else if (remain_frame > 0.0f) {
                    _delta_frame = remain_frame;
                    remain_frame = -1.0f;
                }
                else
                    break;
            }
        }
    }

    void Scene::Disp(GPM, DispType disp_type) {
        if (flags & SCENE_NOT_DISP)
            return;

        for (SceneEffect& i : effects)
            if (i.ptr && i.disp) {
                if (GPM_VAL->draw_selected && GPM_VAL->selected_effect
                    && GPM_VAL->selected_effect != i.ptr->effect)
                    continue;

                i.ptr->Disp(GPM_VAL, disp_type);
            }
    }

    void Scene::DispMesh(GPM) {
        if (flags & SCENE_NOT_DISP)
            return;

        for (SceneEffect& i : effects)
            if (i.ptr && i.disp)
                i.ptr->DispMesh(GPM_VAL);
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

    void Scene::SetEnded() {
        if (!(flags & SCENE_ENDED)) {
            enum_and(flags, SCENE_ENDED);
            fade_frame_left = fade_frame;
        }
    }

    void Scene::SetExtAnimMat(mat4* mat, size_t id) {
        if (!id) {
            for (SceneEffect& i : effects)
                if (i.disp && i.ptr)
                    i.ptr->SetExtAnimMat(mat);
        }
        else
            for (SceneEffect& i : effects)
                if (i.disp && i.ptr && i.ptr->id == id) {
                    i.ptr->SetExtAnimMat(mat);
                    break;
                }
    }

    bool Scene::SetExtColor(float_t r, float_t g, float_t b, float_t a, bool set, uint64_t effect_hash) {
        if (type == Glitter::FT && effect_hash == hash_fnv1a64m_empty
            || type != Glitter::FT && effect_hash == hash_murmurhash_empty) {
            for (SceneEffect& i : effects)
                if (i.disp && i.ptr)
                    i.ptr->SetExtColor(r, g, b, a, set);
            return true;
        }
        else
            for (SceneEffect& i : effects)
                if (i.disp && i.ptr && i.ptr->data.name_hash == effect_hash) {
                    i.ptr->SetExtColor(r, g, b, a, set);
                    return true;
                }
        return false;
    }

    bool Scene::SetExtColorByID(float_t r, float_t g, float_t b, float_t a, bool set, size_t id) {
        if (!id) {
            for (SceneEffect& i : effects)
                if (i.disp && i.ptr)
                    i.ptr->SetExtColor(r, g, b, a, set);
            return true;
        }
        else
            for (SceneEffect& i : effects)
                if (i.disp && i.ptr && i.ptr->id == id) {
                    i.ptr->SetExtColor(r, g, b, a, set);
                    return true;
                }
        return false;
    }

    void Scene::SetExtScale(float_t scale, size_t id) {
        if (!id) {
            for (SceneEffect& i : effects)
                if (i.disp && i.ptr)
                    i.ptr->SetExtScale(scale);
        }
        else
            for (SceneEffect& i : effects)
                if (i.disp && i.ptr && i.ptr->id == id) {
                    i.ptr->SetExtScale(scale);
                    break;
                }
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
