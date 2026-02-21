/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"
#include "../../CRE/Glitter/glitter.hpp"
#include "../../CRE/render_context.hpp"
#include "../dw.hpp"

extern render_context* rctx_ptr;

class GlitterDw : public dw::Shell {
public:
    dw::Label* labels[10];

    GlitterDw();
    virtual ~GlitterDw() override;
    virtual void Draw() override;

    virtual void Hide() override;
};

GlitterDw* glitter_dw;

void glitter_dw_init() {
    if (!glitter_dw) {
        glitter_dw = new GlitterDw;
        glitter_dw->LimitPosDisp();
    }
    else
        glitter_dw->Disp();
}

GlitterDw::GlitterDw() {
    SetText("Glitter");

    rect.pos = 0.0;
    SetSize(320.0f);

    for (int32_t i = 0; i < 10; i++) {
        labels[i] = new dw::Label(this, dw::FLAG_4000);
        labels[i]->SetText("QUAD  (ctrl): 01234");
    }

    dw::Composite::UpdateLayout();
}

GlitterDw::~GlitterDw() {

}

void GlitterDw::Draw() {
    char buf[0x100];

    sprintf_s(buf, sizeof(buf), "QUAD  (ctrl): %5lld",
        Glitter::glt_particle_manager->GetCtrlCount(Glitter::PARTICLE_QUAD));
    labels[0]->SetText(buf);

    sprintf_s(buf, sizeof(buf), "      (disp): %5lld",
        Glitter::glt_particle_manager->GetDispCount(Glitter::PARTICLE_QUAD));
    labels[1]->SetText(buf);

    sprintf_s(buf, sizeof(buf), "LOCUS (ctrl): %5lld",
        Glitter::glt_particle_manager->GetCtrlCount(Glitter::PARTICLE_LINE));
    labels[2]->SetText(buf);

    sprintf_s(buf, sizeof(buf), "      (disp): %5lld",
        Glitter::glt_particle_manager->GetDispCount(Glitter::PARTICLE_LINE));
    labels[3]->SetText(buf);

    sprintf_s(buf, sizeof(buf), "LINE  (ctrl): %5lld",
        Glitter::glt_particle_manager->GetCtrlCount(Glitter::PARTICLE_LOCUS));
    labels[4]->SetText(buf);

    sprintf_s(buf, sizeof(buf), "      (disp): %5lld",
        Glitter::glt_particle_manager->GetDispCount(Glitter::PARTICLE_LOCUS));
    labels[5]->SetText(buf);

    sprintf_s(buf, sizeof(buf), "MESH  (ctrl): %5lld",
        Glitter::glt_particle_manager->GetCtrlCount(Glitter::PARTICLE_MESH));
    labels[6]->SetText(buf);

    sprintf_s(buf, sizeof(buf), "      (disp): %5lld",
        Glitter::glt_particle_manager->GetDispCount(Glitter::PARTICLE_MESH));
    labels[7]->SetText(buf);

    sprintf_s(buf, sizeof(buf), "TOTAL (ctrl): %5lld",
        Glitter::glt_particle_manager->GetCtrlCount(Glitter::PARTICLE_QUAD)
        + Glitter::glt_particle_manager->GetCtrlCount(Glitter::PARTICLE_LINE)
        + Glitter::glt_particle_manager->GetCtrlCount(Glitter::PARTICLE_LOCUS)
        + Glitter::glt_particle_manager->GetCtrlCount(Glitter::PARTICLE_MESH));
    labels[8]->SetText(buf);

    sprintf_s(buf, sizeof(buf), "      (disp): %5lld",
        Glitter::glt_particle_manager->GetDispCount(Glitter::PARTICLE_QUAD)
        + Glitter::glt_particle_manager->GetDispCount(Glitter::PARTICLE_LINE)
        + Glitter::glt_particle_manager->GetDispCount(Glitter::PARTICLE_LOCUS)
        + Glitter::glt_particle_manager->GetDispCount(Glitter::PARTICLE_MESH));
    labels[9]->SetText(buf);

    dw::Shell::Draw();
}

void GlitterDw::Hide() {
    SetDisp();
}
