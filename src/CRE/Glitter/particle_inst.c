/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "particle_inst.h"
#include "effect_inst.h"
#include "render_group.h"
#include "render_scene.h"

GlitterParticleInst::GlitterParticleInst() : particle() {

}

GlitterParticleInst::~GlitterParticleInst() {

}

GlitterF2ParticleInstData::GlitterF2ParticleInstData() : data(), flags(),
render_group(), random_ptr(), effect(), emitter(), parent(), particle() {
}

GlitterF2ParticleInstData::~GlitterF2ParticleInstData() {

}

GlitterF2ParticleInst::GlitterF2ParticleInst(glitter_particle* a1, GlitterF2EffectInst* a2,
    GlitterF2EmitterInst* a3, glitter_random* random, float_t emission) {
    particle = a1;
    data.effect = a2;
    data.emitter = a3;
    data.particle = a1;
    data.data = a1->data;
    data.random_ptr = random;

    if (data.data.type != GLITTER_PARTICLE_LOCUS) {
        GlitterF2RenderGroup* rg = new GlitterF2RenderGroup(this);
        if (rg) {
            rg->alpha = a2->GetAlpha();
            rg->fog = a2->GetFog();
            if (data.data.type == GLITTER_PARTICLE_QUAD
                && data.data.blend_mode == GLITTER_PARTICLE_BLEND_PUNCH_THROUGH)
                rg->alpha = DRAW_PASS_3D_OPAQUE;

            if (data.data.emission >= glitter_min_emission)
                rg->emission = data.data.emission;
            else if (a2->data.emission >= glitter_min_emission)
                rg->emission = a2->data.emission;
            else
                rg->emission = emission;
            data.render_group = rg;
            a2->render_scene.Append(rg);
        }
    }
    else
        enum_or(data.flags, GLITTER_PARTICLE_INST_NO_CHILD);
}

GlitterF2ParticleInst::GlitterF2ParticleInst(GlitterF2ParticleInst* a1, float_t emission) {
    particle = a1->particle;
    data.effect = a1->data.effect;
    data.emitter = a1->data.emitter;
    data.parent = a1;

    data.random_ptr = a1->data.random_ptr;
    data.data = a1->data.data;

    data.particle = a1->data.particle;

    GlitterF2RenderGroup* rg = new GlitterF2RenderGroup(this);
    if (rg) {
        GlitterF2EffectInst* effect = (GlitterF2EffectInst*)a1->data.effect;
        rg->alpha = effect->GetAlpha();
        rg->fog = effect->GetFog();
        if (data.data.blend_mode == GLITTER_PARTICLE_BLEND_PUNCH_THROUGH
            && data.data.type == GLITTER_PARTICLE_QUAD)
            rg->alpha = DRAW_PASS_3D_OPAQUE;
        if (effect->data.emission >= glitter_min_emission)
            rg->emission = effect->data.emission;
        else
            rg->emission = emission;
        data.render_group = rg;
        effect->render_scene.Append(rg);
    }

    if (data.data.type == GLITTER_PARTICLE_LOCUS)
        enum_or(data.flags, GLITTER_PARTICLE_INST_NO_CHILD);
}

GlitterF2ParticleInst::~GlitterF2ParticleInst() {
    for (GlitterF2ParticleInst* i : data.children)
        delete i;

    if (data.render_group) {
        data.render_group->DeleteBuffers(true);
        data.render_group = 0;
    }
}

void GlitterF2ParticleInst::Emit(GPM, GLT, int32_t dup_count, int32_t count, float_t emission) {
    if (data.flags & GLITTER_PARTICLE_INST_ENDED)
        return;

    GlitterF2ParticleInst* ptcl = this;
    while (!ptcl->data.parent && ptcl->data.flags & GLITTER_PARTICLE_INST_NO_CHILD) {
        GlitterF2ParticleInst* particle = new GlitterF2ParticleInst(ptcl, emission);
        if (particle)
            ptcl->data.children.push_back(particle);
        else
            return;

        ptcl = particle;
        if (particle->data.flags & GLITTER_PARTICLE_INST_ENDED)
            return;
    }

    if (ptcl->data.render_group)
        ptcl->data.render_group->Emit(GPM_VAL, GLT_VAL,
            &ptcl->data, ptcl->data.emitter, dup_count, count);
}

void GlitterF2ParticleInst::Free(bool free) {
    enum_or(data.flags, GLITTER_PARTICLE_INST_ENDED);
    if (free && data.render_group)
        data.render_group->Free();

    for (GlitterF2ParticleInst*& i : data.children)
        i->Free(free);
}

bool GlitterF2ParticleInst::HasEnded(bool a2) {
    if (~data.flags & GLITTER_PARTICLE_INST_ENDED)
        return false;
    else if (!a2)
        return true;

    if (~data.flags & GLITTER_PARTICLE_INST_NO_CHILD || data.parent) {
        if (data.render_group && data.render_group->ctrl > 0)
            return false;
        return true;
    }

    for (GlitterF2ParticleInst*& i : data.children)
        if (!i->HasEnded(a2))
            return false;
    return true;
}

void GlitterF2ParticleInst::Reset() {
    data.flags = (glitter_particle_inst_flag)0;
    if (data.render_group)
        data.render_group->Free();

    for (GlitterF2ParticleInst*& i : data.children)
        i->Reset();
}
