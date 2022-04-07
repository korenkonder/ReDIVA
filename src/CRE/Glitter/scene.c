/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "scene.h"
#include "../GlitterX/effect_inst_x.h"
#include "effect_inst.h"

GlitterScene::GlitterScene(GlitterSceneCounter counter, uint64_t hash, GlitterEffectGroup* a4, bool a5) : counter(0) {
    this->counter = counter;
    this->hash = hash;
    flags = GLITTER_SCENE_NONE;
    emission = 1.0f;
    type = a4->type;
    effect_group = a4;
    delta_frame_history = 0.0f;
    skip = false;
    effects = {};
#if defined(CRE_DEV)
    frame_rate = 0;
#endif
    if (a4) {
        effects.reserve(a4->effects.size());
        emission = a4->emission;
        if (a5)
            enum_or(flags, GLITTER_SCENE_FLAG_4);
    }
}

GlitterScene::~GlitterScene() {
    for (GlitterSceneEffect& i : effects)
        delete i.ptr;
}

#if defined(CRE_DEV)
void GlitterScene::CalcDisp(GPM) {
    for (GlitterSceneEffect& i : effects)
        if (i.ptr && i.disp) {
            GlitterXEffectInst* ex = dynamic_cast<GlitterXEffectInst*>(i.ptr);
            if (!ex || GPM_VAL->draw_selected && GPM_VAL->effect && GPM_VAL->effect != i.ptr)
                continue;
            ex->CalcDisp(GPM_VAL);
        }
}
#endif

void GlitterScene::Ctrl(GPM, float_t delta_frame) {
    static int32_t call_count;
    for (GlitterSceneEffect& i : effects)
        if (i.ptr && i.disp)
            i.ptr->Ctrl(GPM_VAL, type, delta_frame, emission);
}

void GlitterScene::Disp(GPM, draw_pass_3d_type alpha) {
    for (GlitterSceneEffect& i : effects)
        if (i.ptr && i.disp) {
#if defined(CRE_DEV)
            if (GPM_VAL->draw_selected && GPM_VAL->effect && GPM_VAL->effect != i.ptr)
                continue;
#endif
            i.ptr->Disp(GPM_VAL, alpha);
        }
}

size_t GlitterScene::GetCtrlCount(glitter_particle_type ptcl_type) {
    size_t ctrl = 0;
    for (GlitterSceneEffect& i : effects)
        if (i.ptr && i.disp)
            ctrl += i.ptr->GetCtrlCount(ptcl_type);
    return ctrl;
}

size_t GlitterScene::GetDispCount(glitter_particle_type ptcl_type) {
    size_t disp = 0;
    for (GlitterSceneEffect& i : effects)
        if (i.ptr && i.disp)
            disp += i.ptr->GetDispCount(ptcl_type);
    return disp;
}

void GlitterScene::GetFrame(float_t* frame, int32_t* life_time) {
    for (GlitterSceneEffect& i : effects)
        if (i.ptr && i.disp) {
            GlitterEffectInst* effect = i.ptr;
            if (effect && frame && life_time && effect->data.life_time > *life_time) {
                if (*frame < effect->frame0)
                    *frame = effect->frame0;
                *life_time = effect->data.life_time;
            }
        }
}

void GlitterScene::GetStartEndFrame(int32_t* start_frame, int32_t* end_frame) {
    for (GlitterSceneEffect& i : effects) {
        if (!i.ptr || !i.disp)
            continue;

        GlitterF2EffectInst* effect_f2 = dynamic_cast<GlitterF2EffectInst*>(i.ptr);
        GlitterXEffectInst* effect_x = dynamic_cast<GlitterXEffectInst*>(i.ptr);
        if (effect_f2) {
            int32_t life_time = effect_f2->data.life_time;
            if (start_frame && effect_f2->data.appear_time < *start_frame)
                *start_frame = effect_f2->data.appear_time;

            for (GlitterF2EmitterInst*& j : effect_f2->emitters) {
                if (!j)
                    continue;

                GlitterF2EmitterInst* emitter = j;
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

            for (GlitterXEmitterInst*& j : effect_x->emitters) {
                if (!j)
                    continue;

                GlitterXEmitterInst* emitter = j;
                if (life_time < emitter->data.life_time)
                    life_time = emitter->data.life_time;
            }

            life_time += effect_x->data.appear_time;

            if (end_frame && life_time > *end_frame)
                *end_frame = life_time;
        }
    }
}

bool GlitterScene::FreeEffect(GPM, uint64_t effect_hash, bool free) {
    if (type == GLITTER_FT && effect_hash == hash_fnv1a64m_empty
        || type != GLITTER_FT && effect_hash == hash_murmurhash_empty) {
        for (GlitterSceneEffect& i : effects)
            if (i.disp && i.ptr)
                i.ptr->Free(GPM_VAL, type, emission, free);
        return true;
    }

    for (GlitterSceneEffect& i : effects)
        if (i.disp && i.ptr && i.ptr->data.name_hash == effect_hash) {
            i.ptr->Free(GPM_VAL, type, emission, free);
            return true;
        }
    return false;
}

bool GlitterScene::HasEnded(bool a2) {
    for (GlitterSceneEffect& i : effects)
        if (i.ptr && i.disp && !i.ptr->HasEnded(a2))
            return false;
    return true;
}

void GlitterScene::InitEffect(GPM, glitter_effect* a2, size_t id, bool appear_now) {
    if (!a2)
        return;

    for (GlitterSceneEffect& i : effects)
        if (i.ptr && i.disp && i.ptr->id == id) {
            i.ptr->Reset(GPM_VAL, type, emission);
            return;
        }

    GlitterSceneEffect effect;
    if (type != GLITTER_X)
        effect.ptr = new GlitterF2EffectInst(GPM_VAL, type, a2, id, emission, appear_now);
    else
        effect.ptr = new GlitterXEffectInst(GPM_VAL, a2, id, emission, appear_now);
    effect.disp = true;
    effects.push_back(effect);
}

bool GlitterScene::ResetEffect(GPM, uint64_t effect_hash) {
    if (type == GLITTER_FT && effect_hash == hash_fnv1a64m_empty
        || type != GLITTER_FT && effect_hash == hash_murmurhash_empty) {
        for (GlitterSceneEffect& i : effects)
            if (i.disp && i.ptr)
                i.ptr->Reset(GPM_VAL, type, emission);
        return true;
    }
    else
        for (GlitterSceneEffect& i : effects)
            if (i.disp && i.ptr && i.ptr->data.name_hash == effect_hash) {
                i.ptr->Reset(GPM_VAL, type, emission);
                return true;
            }
    return false;
}

#if defined(CRE_DEV)
void GlitterScene::SetFrameRate(FrameRateControl* frame_rate) {
    this->frame_rate = frame_rate;
}
#endif

GlitterSceneCounter::GlitterSceneCounter(uint32_t counter) {
    this->index = 0;
    this->counter = counter;
}

GlitterSceneCounter::GlitterSceneCounter(uint32_t index, uint32_t counter) {
    this->index = index;
    this->counter = counter;
}
