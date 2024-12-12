/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "shadow.hpp"
#include "../../CRE/render_context.hpp"
#include "../dw.hpp"

extern render_context* rctx_ptr;

class ShadowDw : public dw::Shell {
public:
    dw::RowLayout* layout;
    dw::Slider* distance_slider;
    dw::Slider* shadow_ambient_slider;
    dw::Slider* shadow_range_factor_slider;
    float_t distance;
    int32_t near_blur;
    int32_t far_blur;
    float_t field_1AC;
    float_t field_1B0;
    float_t shadow_ambient;
    float_t shadow_range_factor;
    Shadow* shadow_ptr;

    ShadowDw();
    virtual ~ShadowDw() override;
    virtual void Draw() override;

    virtual void Hide() override;

    static void EnableSelfShadowCallback(dw::Widget* data);
    static void EnableShadowCallback(dw::Widget* data);
    static void DistanceCallback(dw::Widget* data);
    static void FarBlurCallback(dw::Widget* data);
    static void NearBlurCallback(dw::Widget* data);
    static void ShadowAmbientCallback(dw::Widget* data);
    static void ShadowRangeFactorCallback(dw::Widget* data);
    static void ShowStageShadowCallback(dw::Widget* data);
    static void ShowTextureCallback(dw::Widget* data);
};

ShadowDw* shadow_dw;

void shadow_dw_init() {
    if (!shadow_dw) {
        shadow_dw = new ShadowDw;
        shadow_dw->UpdateLayout();
        shadow_dw->sub_1402F38B0();
    }
    else
        shadow_dw->Disp();
}

ShadowDw::ShadowDw() {
    shadow_ptr = shadow_ptr_get();
    distance = shadow_ptr->distance;
    near_blur = shadow_ptr->near_blur;
    far_blur = shadow_ptr->far_blur;
    field_1AC = shadow_ptr->field_2D0;
    field_1B0 = shadow_ptr->field_2D4;
    shadow_ambient = shadow_ptr->shadow_ambient;
    shadow_range_factor = shadow_ptr->shadow_range_factor;

    SetText("SHADOW");

    rect.pos = 0.0f;
    SetSize({ 320.0f, 200.0f });

    const float_t glyph_height = font.GetFontGlyphHeight();

    layout = new dw::RowLayout(dw::HORIZONTAL);

    distance_slider = dw::Slider::Create(this, (dw::Flags)(dw::FLAG_800
        | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_height, "Distance");
    distance_slider->SetParams(distance, 0.01f, 20.0f, 1.0f, 0.1f, 1.0f);
    distance_slider->format = "%5.2f";
    distance_slider->AddSelectionListener(
        new dw::SelectionListenerOnHook(ShadowDw::DistanceCallback));

    dw::Slider* near_blur_slider = dw::Slider::Create(this, (dw::Flags)(dw::FLAG_800
        | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_height, "Near Blur");
    near_blur_slider->SetParams((float_t)near_blur, 0.0f, 10.0f, 1.0f, 1.0f, 1.0f);
    near_blur_slider->format = "%2.0f";
    near_blur_slider->AddSelectionListener(
        new dw::SelectionListenerOnHook(ShadowDw::NearBlurCallback));

    dw::Slider* far_blur_slider = dw::Slider::Create(this, dw::Flags(dw::FLAG_800
            | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_height, "Far  Blur");
    far_blur_slider->SetParams((float_t)far_blur, 1.0f, 10.0f, 1.0f, 1.0f, 1.0f);
    far_blur_slider->format = "%2.0f";
    far_blur_slider->AddSelectionListener(
        new dw::SelectionListenerOnHook(ShadowDw::FarBlurCallback));

    shadow_ambient_slider = dw::Slider::Create(this, (dw::Flags)(dw::FLAG_800
        | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_height, "Shadow Ambient");
    shadow_ambient_slider->SetParams(shadow_ambient, 0.0f, 1.0f, 0.1f, 0.025f, 0.1f);
    shadow_ambient_slider->format = "%5.3f";
    shadow_ambient_slider->AddSelectionListener(
        new dw::SelectionListenerOnHook(ShadowDw::ShadowAmbientCallback));

    const char* shadow_range_factor_text;
    if (dw::translate)
        shadow_range_factor_text = u8"Shadow Range Factor";
    else
        shadow_range_factor_text = u8"シャドウ範囲係数";

    shadow_range_factor_slider = dw::Slider::Create(this, (dw::Flags)(dw::FLAG_800
        | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_height, shadow_range_factor_text);
    shadow_range_factor_slider->SetParams(shadow_range_factor, 1.0f, 2.0f, 0.1f, 0.01f, 0.1f);
    shadow_range_factor_slider->format = "%4.2f";
    shadow_range_factor_slider->AddSelectionListener(
        new dw::SelectionListenerOnHook(ShadowDw::ShadowRangeFactorCallback));

    dw::Button* show_texture = new dw::Button(this, dw::CHECKBOX);
    show_texture->SetText("Show Texture");
    show_texture->SetValue(shadow_ptr->show_texture);
    show_texture->callback = ShadowDw::ShowTextureCallback;

    dw::Button* show_stage_shadow = new dw::Button(this, dw::CHECKBOX);
    show_stage_shadow->SetText("Show Stage Shadow");
    show_stage_shadow->SetValue(rctx_ptr->render_manager->show_stage_shadow);
    show_stage_shadow->callback = ShadowDw::ShowStageShadowCallback;

    dw::Button* enable_shadow = new dw::Button(this, dw::CHECKBOX);
    enable_shadow->SetText("ENABLE SHADOW");
    enable_shadow->SetValue(rctx_ptr->render_manager->shadow);
    enable_shadow->callback = ShadowDw::EnableShadowCallback;

    dw::Button* enable_self_shadow = new dw::Button(this, dw::CHECKBOX);
    enable_self_shadow->SetText("ENABLE SELF SHADOW");
    enable_self_shadow->SetValue(shadow_ptr_get()->self_shadow);
    enable_self_shadow->callback = ShadowDw::EnableSelfShadowCallback;
}

ShadowDw::~ShadowDw() {

}

void ShadowDw::Draw() {
    shadow_ambient = shadow_ptr->shadow_ambient;
    shadow_ambient_slider->SetValue(shadow_ambient);
    shadow_range_factor = shadow_ptr->shadow_range_factor;
    shadow_range_factor_slider->SetValue(shadow_range_factor);
    dw::Shell::Draw();
}

void ShadowDw::Hide() {
    SetDisp();
}

void ShadowDw::EnableSelfShadowCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button)
        shadow_ptr_get()->self_shadow = button->value;
}

void ShadowDw::EnableShadowCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button)
        rctx_ptr->render_manager->shadow = button->value;
}

void ShadowDw::DistanceCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider) {
        const float_t value = slider->GetValue();
        shadow_dw->distance = value;
        shadow_dw->shadow_ptr->set_distance(value);
    }
}

void ShadowDw::FarBlurCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider) {
        const int32_t value = (int32_t)slider->GetValue();
        shadow_dw->far_blur = value;
        shadow_dw->shadow_ptr->far_blur = value;
    }
}

void ShadowDw::NearBlurCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider) {
        const int32_t value = (int32_t)slider->GetValue();
        shadow_dw->near_blur = value;
        shadow_dw->shadow_ptr->near_blur = value;
    }
}

void ShadowDw::ShadowAmbientCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider) {
        const float_t value = slider->GetValue();
        shadow_dw->shadow_ambient = value;
        shadow_dw->shadow_ptr->shadow_ambient = value;
    }
}

void ShadowDw::ShadowRangeFactorCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider) {
        const float_t value = slider->GetValue();
        shadow_dw->shadow_range_factor = value;
        shadow_dw->shadow_ptr->shadow_range_factor = value;
    }
}

void ShadowDw::ShowStageShadowCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button)
        rctx_ptr->render_manager->show_stage_shadow = button->value;
}

void ShadowDw::ShowTextureCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button) {
        shadow_dw->shadow_ptr->show_texture = button->value;
        shadow_dw->shadow_ptr->clear_textures();
    }
}
