/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "render_scene_x.h"
#include "render_group_x.h"

GlitterXRenderScene::GlitterXRenderScene() {

}

GlitterXRenderScene::~GlitterXRenderScene() {
    for (GlitterXRenderGroup*& i : groups)
        delete i;
}

void GlitterXRenderScene::Append(GlitterXRenderGroup* rg) {
    groups.push_back(rg);
}

void GlitterXRenderScene::CalcDisp(GPM) {
    GlitterXEffectInst* eff = dynamic_cast<GlitterXEffectInst*>(GPM_VAL->effect);
    GlitterXEmitterInst* emit = dynamic_cast<GlitterXEmitterInst*>(GPM_VAL->emitter);
    GlitterXParticleInst* ptcl = dynamic_cast<GlitterXParticleInst*>(GPM_VAL->particle);
    for (GlitterXRenderGroup*& i : groups) {
        if (!i)
            continue;

        GlitterXRenderGroup* rg = i;
        if (rg->CannotDisp() && !GPM_VAL->draw_all)
            continue;

#if !defined(CRE_DEV)
        GlitterXRenderGroup_calc_disp(GPM_VAL, rg);
#else
        if (!GPM_VAL->draw_selected || !eff) {
            rg->CalcDisp(GPM_VAL);
        }
        else if ((eff && ptcl) || (eff && !emit)) {
            if (!ptcl || rg->particle == ptcl)
                rg->CalcDisp(GPM_VAL);
        }
        else if (emit)
            for (GlitterXParticleInst*& i : emit->particles) {
                if (!i)
                    continue;

                GlitterXParticleInst* particle = i;
                if (rg->particle == particle)
                    rg->CalcDisp(GPM_VAL);


                for (GlitterXParticleInst*& j : particle->data.children)
                    if (j && rg->particle == j)
                        rg->CalcDisp(GPM_VAL);
            }
#endif
    }
}

void GlitterXRenderScene::Ctrl(float_t delta_frame, bool copy_mats) {
    for (GlitterXRenderGroup*& i : groups)
        if (i)
            i->Ctrl(delta_frame, copy_mats);
}

void GlitterXRenderScene::Disp(GPM, draw_pass_3d_type alpha) {
    GlitterXEffectInst* eff = dynamic_cast<GlitterXEffectInst*>(GPM_VAL->effect);
    GlitterXEmitterInst* emit = dynamic_cast<GlitterXEmitterInst*>(GPM_VAL->emitter);
    GlitterXParticleInst* ptcl = dynamic_cast<GlitterXParticleInst*>(GPM_VAL->particle);
    for (GlitterXRenderGroup* i : groups) {
        if (!i)
            continue;

        GlitterXRenderGroup* rg = i;
        if ((rg)->alpha != alpha || rg->CannotDisp() && !GPM_VAL->draw_all)
            continue;

#if !defined(CRE_DEV)
        GlitterXRenderGroup_disp(GPM_VAL, rg);
#else
        if (!GPM_VAL->draw_selected || !eff) {
            rg->Disp(GPM_VAL);
        }
        else if ((eff && ptcl) || (eff && !emit)) {
            if (!ptcl || rg->particle == ptcl)
                rg->Disp(GPM_VAL);
        }
        else if (emit)
            for (GlitterXParticleInst*& i : emit->particles) {
                if (!i)
                    continue;

                GlitterXParticleInst* particle = i;
                if (rg->particle == particle)
                    rg->Disp(GPM_VAL);

                for (GlitterXParticleInst*& j : particle->data.children)
                    if (j && rg->particle == j)
                        rg->Disp(GPM_VAL);
            }
#endif
    }
}

size_t GlitterXRenderScene::GetCtrlCount(glitter_particle_type type) {
    size_t ctrl = 0;
    for (GlitterXRenderGroup*& i : groups) {
        if (!i)
            continue;

        GlitterXRenderGroup* rg = i;
        if (rg->type == type)
            ctrl += rg->ctrl;
    }
    return ctrl;
}

size_t GlitterXRenderScene::GetDispCount(glitter_particle_type type) {
    size_t disp = 0;
    for (GlitterXRenderGroup* i : groups) {
        if (!i)
            continue;

        GlitterXRenderGroup* rg = i;
        if (rg->type == type)
            disp += rg->disp;
    }
    return disp;
}
