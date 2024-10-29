/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "global_material.hpp"
#include "../../CRE/light_param/face.hpp"
#include "../../CRE/light_param.hpp"
#include "../../CRE/render_context.hpp"
#include "../dw.hpp"

extern render_context* rctx_ptr;

class GlobalMaterialDw : public dw::Shell {
public:
    GlobalMaterialDw();
    virtual ~GlobalMaterialDw() override;
    virtual void Draw() override;

    virtual void Hide() override;

    static void BumpDepthCallback(dw::Widget* data);
    static void IntensityCallback(dw::Widget* data);
    static void ReflectUVScaleCallback(dw::Widget* data);
    static void ReflectivityCallback(dw::Widget* data);
    static void RefractUVScaleCallback(dw::Widget* data);
    static void UseGlobalMaterialCallback(dw::Widget* data);
};

GlobalMaterialDw* global_material_dw;

void global_material_dw_init() {
    if (!global_material_dw) {
        global_material_dw = new GlobalMaterialDw;
        global_material_dw->sub_1402F38B0();
        global_material_dw->UpdateLayout();
    }
    else
        global_material_dw->Disp();
}

GlobalMaterialDw::GlobalMaterialDw() {
    SetText("GLOBAL MATERIAL");

    rect.pos = 0.0f;
    SetSize({ 320.0f, 200.0f });

    global_material_struct& global_material = rctx_ptr->draw_state->global_material;

    const float_t glyph_height = font.GetFontGlyphHeight();

    dw::Button* v5 = new dw::Button(this, dw::CHECKBOX);
    v5->SetText("USE GLOBAL MATERIAL");
    v5->SetValue(rctx_ptr->draw_state->get_use_global_material());
    v5->callback = GlobalMaterialDw::UseGlobalMaterialCallback;

    dw::Slider* v8 = dw::Slider::Create(this, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 192.0f, glyph_height, "BumpDepth");
    v8->SetParams(global_material.bump_depth, 0.0f, 1.0f, 0.1f, 0.01f, 0.1f);
    v8->format = "%5.2f";
    v8->AddSelectionListener(new dw::SelectionListenerOnHook(GlobalMaterialDw::BumpDepthCallback));

    dw::Slider* v11 = dw::Slider::Create(this, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 192.0f, glyph_height, "Intensity");
    v11->SetParams(global_material.intensity, 0.0f, 100.0f, 10.0f, 0.01f, 0.1f);
    v11->format = "%5.2f";
    v11->AddSelectionListener(new dw::SelectionListenerOnHook(GlobalMaterialDw::IntensityCallback));

    dw::Slider* v13 = dw::Slider::Create(this, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 192.0f, glyph_height, "Reflectivity");
    v13->SetParams(global_material.reflectivity, 0.0f, 2.0f, 0.2f, 0.01f, 0.1f);
    v13->format = "%5.2f";
    v13->AddSelectionListener(new dw::SelectionListenerOnHook(GlobalMaterialDw::ReflectivityCallback));

    dw::Slider* v16 = dw::Slider::Create(this, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 192.0f, glyph_height, "Reflect UV Scale");
    v16->SetParams(global_material.reflect_uv_scale, 0.0f, 1.0f, 0.1f, 0.01f, 0.1f);
    v16->format = "%5.2f";
    v16->AddSelectionListener(new dw::SelectionListenerOnHook(GlobalMaterialDw::ReflectUVScaleCallback));

    dw::Slider* v19 = dw::Slider::Create(this, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 192.0f, glyph_height, "Refract UV Scale");
    v19->SetParams(global_material.refract_uv_scale, 0.0f, 1.0f, 0.1f, 0.01f, 0.1f);
    v19->format = "%5.2f";
    v19->AddSelectionListener(new dw::SelectionListenerOnHook(GlobalMaterialDw::RefractUVScaleCallback));
}

GlobalMaterialDw::~GlobalMaterialDw() {

}

void GlobalMaterialDw::Draw() {
    dw::Shell::Draw();
}

void GlobalMaterialDw::Hide() {
    SetDisp();
}

void GlobalMaterialDw::BumpDepthCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider)
        rctx_ptr->draw_state->global_material.bump_depth = slider->scroll_bar->value;
}

void GlobalMaterialDw::IntensityCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider)
        rctx_ptr->draw_state->global_material.intensity = slider->scroll_bar->value;
}

void GlobalMaterialDw::ReflectUVScaleCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider)
        rctx_ptr->draw_state->global_material.reflect_uv_scale = slider->scroll_bar->value;
}

void GlobalMaterialDw::ReflectivityCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider)
        rctx_ptr->draw_state->global_material.reflectivity = slider->scroll_bar->value;
}

void GlobalMaterialDw::RefractUVScaleCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider)
        rctx_ptr->draw_state->global_material.refract_uv_scale = slider->scroll_bar->value;
}

void GlobalMaterialDw::UseGlobalMaterialCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button)
        rctx_ptr->draw_state->set_use_global_material(button->value);
}
