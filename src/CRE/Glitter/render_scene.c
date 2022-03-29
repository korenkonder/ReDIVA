/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "render_scene.h"
#include "render_group.h"

GlitterF2RenderScene::GlitterF2RenderScene() {

}

GlitterF2RenderScene::~GlitterF2RenderScene() {
    for (GlitterF2RenderGroup*& i : groups)
        delete i;
}

void GlitterF2RenderScene::Append(GlitterF2RenderGroup* rg) {
    groups.push_back(rg);
}

void GlitterF2RenderScene::Ctrl(GLT, float_t delta_frame) {
    for (GlitterF2RenderGroup*& i : groups)
        if (i)
            i->Ctrl(GLT_VAL, delta_frame, true);
}

void GlitterF2RenderScene::Disp(GPM, draw_pass_3d_type alpha) {
    GlitterF2EffectInst* eff = dynamic_cast<GlitterF2EffectInst*>(GPM_VAL->effect);
    GlitterF2EmitterInst* emit = dynamic_cast<GlitterF2EmitterInst*>(GPM_VAL->emitter);
    GlitterF2ParticleInst* ptcl = dynamic_cast<GlitterF2ParticleInst*>(GPM_VAL->particle);
    for (GlitterF2RenderGroup*& i : groups) {
        if (!i)
            continue;

        GlitterF2RenderGroup* rg = i;
        if ((rg)->alpha != alpha
            || (rg->CannotDisp() && !GPM_VAL->draw_all))
            continue;

#if !defined(CRE_DEV)
        GlitterF2RenderDraw_draw(GPM_VAL, rg);
#else
        if (!GPM_VAL->draw_selected || !eff) {
            rg->Disp(GPM_VAL);
        }
        else if ((eff && ptcl) || (eff && !emit)) {
            if (!ptcl || rg->particle == ptcl)
                rg->Disp(GPM_VAL);
        }
        else if (emit)
            for (GlitterF2ParticleInst*& i : emit->particles) {
                if (!i)
                    continue;

                GlitterF2ParticleInst* particle = i;
                if (rg->particle == particle)
                    rg->Disp(GPM_VAL);

                for (GlitterF2ParticleInst*& j : particle->data.children)
                    if (j && rg->particle == j)
                        rg->Disp(GPM_VAL);
            }
#endif
    }
}

size_t GlitterF2RenderScene::GetCtrlCount(glitter_particle_type type) {
    size_t ctrl = 0;
    for (GlitterF2RenderGroup*& i : groups) {
        if (!i)
            continue;

        GlitterF2RenderGroup* rg = i;
        if (rg->type == type)
            ctrl += rg->ctrl;
    }
    return ctrl;
}

size_t GlitterF2RenderScene::GetDispCount(glitter_particle_type type) {
    size_t disp = 0;
    for (GlitterF2RenderGroup* i : groups) {
        if (!i)
            continue;

        GlitterF2RenderGroup* rg = i;
        if (rg->type == type)
            disp += rg->disp;
    }
    return disp;
}
