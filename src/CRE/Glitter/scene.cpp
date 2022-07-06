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
#if defined(CRE_DEV)
        frame_rate = 0;
#endif
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
        if (~flags & SCENE_FLAG_3)
            return flags;

        for (SceneEffect& i : effects)
            if (i.ptr && i.disp && i.ptr->id == eff_inst->id) {
                i.ptr->Copy(eff_inst, dst->emission);
                return true;
            }
        return false;
    }

#if defined(CRE_DEV)
    void Scene::CalcDisp(GPM) {
        for (SceneEffect& i : effects)
            if (i.ptr && i.disp) {
                XEffectInst* eff_x = dynamic_cast<XEffectInst*>(i.ptr);
                if (!eff_x || GPM_VAL->draw_selected && GPM_VAL->effect && GPM_VAL->effect != i.ptr)
                    continue;

                eff_x->CalcDisp(GPM_VAL);
            }
    }
#endif

    void Scene::Ctrl(GPM, float_t delta_frame) {
        static int32_t call_count;
        for (SceneEffect& i : effects)
            if (i.ptr && i.disp)
                i.ptr->Ctrl(GPM_VAL, type, delta_frame, emission);
    }

    void Scene::Disp(GPM, DispType disp_type) {
        if (flags & SCENE_NOT_DISP)
            return;

        for (SceneEffect& i : effects)
            if (i.ptr && i.disp) {
#if defined(CRE_DEV)
                if (GPM_VAL->draw_selected && GPM_VAL->effect && GPM_VAL->effect != i.ptr)
                    continue;
#endif
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

    void Scene::GetFrame(float_t* frame, int32_t* life_time) {
        for (SceneEffect& i : effects)
            if (i.ptr && i.disp) {
                EffectInst* effect = i.ptr;
                if (effect && frame && life_time && effect->data.life_time > *life_time) {
                    if (*frame < effect->frame0)
                        *frame = effect->frame0;
                    *life_time = effect->data.life_time;
                }
            }
    }

    void Scene::GetStartEndFrame(int32_t* start_frame, int32_t* end_frame) {
        for (SceneEffect& i : effects) {
            if (!i.ptr || !i.disp)
                continue;

            F2EffectInst* effect_f2 = dynamic_cast<F2EffectInst*>(i.ptr);
            XEffectInst* effect_x = dynamic_cast<XEffectInst*>(i.ptr);
            if (effect_f2) {
                int32_t life_time = effect_f2->data.life_time;
                if (start_frame && effect_f2->data.appear_time < *start_frame)
                    *start_frame = effect_f2->data.appear_time;

                for (F2EmitterInst*& j : effect_f2->emitters) {
                    if (!j)
                        continue;

                    F2EmitterInst* emitter = j;
                    if (life_time < emitter->data.life_time)
                        life_time = emitter->data.life_time;
                }

                life_time += effect_f2->data.appear_time;

                if (end_frame && life_time > *end_frame)
                    *end_frame = life_time;
            }
            else if (effect_x) {
                int32_t life_time = effect_x->data.life_time;
                if (start_frame && effect_x->data.appear_time < *start_frame)
                    *start_frame = effect_x->data.appear_time;

                for (XEmitterInst*& j : effect_x->emitters) {
                    if (!j)
                        continue;

                    XEmitterInst* emitter = j;
                    if (life_time < emitter->data.life_time)
                        life_time = emitter->data.life_time;
                }

                life_time += effect_x->data.appear_time;

                if (end_frame && life_time > *end_frame)
                    *end_frame = life_time;
            }
        }
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

    bool Scene::HasEnded(bool a2) {
        for (SceneEffect& i : effects)
            if (i.ptr && i.disp && !i.ptr->HasEnded(a2))
                return false;
        return true;
    }

    void Scene::InitEffect(GPM, Effect* eff, size_t id, bool appear_now) {
        if (!eff)
            return;

        for (SceneEffect& i : effects)
            if (i.ptr && i.disp && i.ptr->id == id) {
                i.ptr->Reset(GPM_VAL, type, emission);
                return;
            }

        SceneEffect effect;
        if (type != Glitter::X)
            effect.ptr = new F2EffectInst(GPM_VAL, type, eff, id, emission, appear_now);
        else
            effect.ptr = new XEffectInst(GPM_VAL, eff, id, emission, appear_now);
        effect.disp = true;
        effects.push_back(effect);
    }

    bool Scene::ResetEffect(GPM, uint64_t effect_hash) {
        if (type == Glitter::FT && effect_hash == hash_fnv1a64m_empty
            || type != Glitter::FT && effect_hash == hash_murmurhash_empty) {
            for (SceneEffect& i : effects)
                if (i.disp && i.ptr)
                    i.ptr->Reset(GPM_VAL, type, emission);
            return true;
        }
        else
            for (SceneEffect& i : effects)
                if (i.disp && i.ptr && i.ptr->data.name_hash == effect_hash) {
                    i.ptr->Reset(GPM_VAL, type, emission);
                    return true;
                }
        return false;
    }

#if defined(CRE_DEV)
    void Scene::SetFrameRate(FrameRateControl* frame_rate) {
        this->frame_rate = frame_rate;
    }
#endif

    SceneCounter::SceneCounter(uint32_t counter) {
        this->index = 0;
        this->counter = counter;
    }

    SceneCounter::SceneCounter(uint32_t index, uint32_t counter) {
        this->index = index;
        this->counter = counter;
    }
}
