/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "particle_inst_x.h"
#include "effect_inst_x.h"
#include "render_group_x.h"
#include "render_scene_x.h"

GlitterXParticleInstData::GlitterXParticleInstData() : data(), flags(),
render_group(), random_ptr(), effect(), emitter(), parent(), particle() {

}

GlitterXParticleInstData::~GlitterXParticleInstData() {

}

GlitterXParticleInst::GlitterXParticleInst(glitter_particle* a1, GlitterXEffectInst* a2,
    GlitterXEmitterInst* a3, glitter_random* random, float_t emission) {
    particle = a1;
    data.effect = a2;
    data.emitter = a3;
    data.particle = a1;
    data.data = a1->data;
    data.random_ptr = random;

    if (data.data.type == GLITTER_PARTICLE_QUAD || data.data.type == GLITTER_PARTICLE_MESH) {
        GlitterXRenderGroup* rg = new GlitterXRenderGroup(this);
        if (rg) {
            rg->alpha = a2->GetAlpha();
            rg->fog = a2->GetFog();

            if (data.data.draw_flags & GLITTER_PARTICLE_DRAW_NO_BILLBOARD_CULL)
                rg->use_culling = false;
            else
                rg->use_culling = true;

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

GlitterXParticleInst::GlitterXParticleInst(GlitterXParticleInst* a1, float_t emission) {
    particle = a1->particle;
    data.effect = a1->data.effect;
    data.emitter = a1->data.emitter;
    data.parent = a1;

    data.random_ptr = a1->data.random_ptr;
    data.data = a1->data.data;

    data.particle = a1->data.particle;

    GlitterXRenderGroup* rg = new GlitterXRenderGroup(this);
    if (rg) {
        GlitterXEffectInst* effect = (GlitterXEffectInst*)a1->data.effect;
        rg->alpha = effect->GetAlpha();
        rg->fog = effect->GetFog();
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

GlitterXParticleInst::~GlitterXParticleInst() {
    for (GlitterXParticleInst*& i : data.children)
        delete i;

    if (data.render_group) {
        data.render_group->DeleteBuffers(true);
        data.render_group = 0;
    }
}

void GlitterXParticleInst::Emit(int32_t dup_count, int32_t count, float_t emission) {
    if (data.flags & GLITTER_PARTICLE_INST_ENDED)
        return;

    GlitterXParticleInst* ptcl = this;
    while (!ptcl->data.parent && ptcl->data.flags & GLITTER_PARTICLE_INST_NO_CHILD) {
        GlitterXParticleInst* particle = new GlitterXParticleInst(ptcl, emission);
        if (particle)
            ptcl->data.children.push_back(particle);
        else
            return;

        ptcl = particle;
        if (particle->data.flags & GLITTER_PARTICLE_INST_ENDED)
            return;
    }

    if (ptcl->data.render_group)
        ptcl->data.render_group->Emit(&ptcl->data, ptcl->data.emitter, dup_count, count);
}

void GlitterXParticleInst::Free(bool free) {
    enum_or(data.flags, GLITTER_PARTICLE_INST_ENDED);
    if (free && data.render_group)
        data.render_group->Free();

    for (GlitterXParticleInst*& i : data.children)
        i->Free(free);
}

bool GlitterXParticleInst::HasEnded(bool a2) {
    if (~data.flags & GLITTER_PARTICLE_INST_ENDED)
        return false;
    else if (!a2)
        return true;

    if (~data.flags & GLITTER_PARTICLE_INST_NO_CHILD || data.parent) {
        if (data.render_group && data.render_group->ctrl > 0)
            return false;
        return true;
    }

    for (GlitterXParticleInst*& i : data.children)
        if (!i->HasEnded(a2))
            return false;
    return true;
}

void GlitterXParticleInst::Reset() {
    data.flags = (glitter_particle_inst_flag)0;
    if (data.render_group)
        data.render_group->Free();

    for (GlitterXParticleInst*& i : data.children)
        i->Reset();
}
