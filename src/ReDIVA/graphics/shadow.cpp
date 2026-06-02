/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "shadow.hpp"
#include "../../CRE/render_context.hpp"
#include "../../CRE/render_manager.hpp"
#include "../../CRE/shadow.hpp"
#include "../dw.hpp"

extern render_context* rctx_ptr;

class ShadowDw : public dw::Shell {
public:
    dw::RowLayout* layout;
    dw::Slider* distance_slider;
    dw::Slider* shadow_ambient_slider;
    dw::Slider* shadow_range_slider;
    float_t dist_attn;
    int32_t near_blur_repeat;
    int32_t far_blur_repeat;
    float_t polyoffset_factor;
    float_t polyoffset_unit;
    float_t shadow_ambient;
    float_t shadow_range;
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
    static void ShadowRangeCallback(dw::Widget* data);
    static void ShowStageShadowCallback(dw::Widget* data);
    static void ShowTextureCallback(dw::Widget* data);
};

ShadowDw* shadow_dw;

void shadow_dw_init() {
    if (!shadow_dw) {
        shadow_dw = new ShadowDw;
        shadow_dw->UpdateLayout();
        shadow_dw->LimitPosDisp();
    }
    else
        shadow_dw->Disp();
}

ShadowDw::ShadowDw() {
    shadow_ptr = get_shadow();
    dist_attn = shadow_ptr->get_dist_attn();
    near_blur_repeat = shadow_ptr->get_near_blur_repeat();
    far_blur_repeat = shadow_ptr->get_far_blur_repeat();
    shadow_ptr->get_polyoffset(&polyoffset_factor, &polyoffset_unit);
    shadow_ambient = shadow_ptr->get_shadow_ambient();
    shadow_range = shadow_ptr->get_shadow_range();

    SetText("SHADOW");

    rect.pos = 0.0f;
    SetSize({ 320.0f, 200.0f });

    const float_t glyph_height = font.GetFontGlyphHeight();

    layout = new dw::RowLayout(dw::HORIZONTAL);

    distance_slider = dw::Slider::Create(this, (dw::Flags)(dw::FLAG_800
        | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_height, "Distance");
    distance_slider->SetParams(dist_attn, 0.01f, 20.0f, 1.0f, 0.1f, 1.0f);
    distance_slider->format = "%5.2f";
    distance_slider->AddSelectionListener(
        new dw::SelectionListenerOnHook(ShadowDw::DistanceCallback));

    dw::Slider* near_blur_slider = dw::Slider::Create(this, (dw::Flags)(dw::FLAG_800
        | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_height, "Near Blur");
    near_blur_slider->SetParams((float_t)near_blur_repeat, 0.0f, 10.0f, 1.0f, 1.0f, 1.0f);
    near_blur_slider->format = "%2.0f";
    near_blur_slider->AddSelectionListener(
        new dw::SelectionListenerOnHook(ShadowDw::NearBlurCallback));

    dw::Slider* far_blur_repeat_slider = dw::Slider::Create(this, dw::Flags(dw::FLAG_800
            | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_height, "Far  Blur");
    far_blur_repeat_slider->SetParams((float_t)far_blur_repeat, 1.0f, 10.0f, 1.0f, 1.0f, 1.0f);
    far_blur_repeat_slider->format = "%2.0f";
    far_blur_repeat_slider->AddSelectionListener(
        new dw::SelectionListenerOnHook(ShadowDw::FarBlurCallback));

    shadow_ambient_slider = dw::Slider::Create(this, (dw::Flags)(dw::FLAG_800
        | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_height, "Shadow Ambient");
    shadow_ambient_slider->SetParams(shadow_ambient, 0.0f, 1.0f, 0.1f, 0.025f, 0.1f);
    shadow_ambient_slider->format = "%5.3f";
    shadow_ambient_slider->AddSelectionListener(
        new dw::SelectionListenerOnHook(ShadowDw::ShadowAmbientCallback));

    const char* shadow_range_text;
    if (dw::translate)
        shadow_range_text = u8"Shadow Range Factor";
    else
        shadow_range_text = u8"シャドウ範囲係数";

    shadow_range_slider = dw::Slider::Create(this, (dw::Flags)(dw::FLAG_800
        | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_height, shadow_range_text);
    shadow_range_slider->SetParams(shadow_range, 1.0f, 2.0f, 0.1f, 0.01f, 0.1f);
    shadow_range_slider->format = "%4.2f";
    shadow_range_slider->AddSelectionListener(
        new dw::SelectionListenerOnHook(ShadowDw::ShadowRangeCallback));

    dw::Button* show_texture = new dw::Button(this, dw::CHECKBOX);
    show_texture->SetText("Show Texture");
    show_texture->SetValue(shadow_ptr->is_enable_show_textures());
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
    enable_self_shadow->SetValue(get_shadow()->get_self_shadow_sw());
    enable_self_shadow->callback = ShadowDw::EnableSelfShadowCallback;
}

ShadowDw::~ShadowDw() {

}

void ShadowDw::Draw() {
    shadow_ambient = shadow_ptr->get_shadow_ambient();
    shadow_ambient_slider->SetValue(shadow_ambient);
    shadow_range = shadow_ptr->get_shadow_range();
    shadow_range_slider->SetValue(shadow_range);
    dw::Shell::Draw();
}

void ShadowDw::Hide() {
    SetDisp();
}

void ShadowDw::EnableSelfShadowCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button)
        get_shadow()->set_self_shadow_sw(button->value);
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
        shadow_dw->dist_attn = value;
        shadow_dw->shadow_ptr->set_dist_attn(value);
    }
}

void ShadowDw::FarBlurCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider) {
        const int32_t value = (int32_t)slider->GetValue();
        shadow_dw->far_blur_repeat = value;
        shadow_dw->shadow_ptr->set_far_blur_repeat(value);
    }
}

void ShadowDw::NearBlurCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider) {
        const int32_t value = (int32_t)slider->GetValue();
        shadow_dw->near_blur_repeat = value;
        shadow_dw->shadow_ptr->set_near_blur_repeat(value);
    }
}

void ShadowDw::ShadowAmbientCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider) {
        const float_t value = slider->GetValue();
        shadow_dw->shadow_ambient = value;
        shadow_dw->shadow_ptr->set_shadow_ambient(value);
    }
}

void ShadowDw::ShadowRangeCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider) {
        const float_t value = slider->GetValue();
        shadow_dw->shadow_range = value;
        shadow_dw->shadow_ptr->set_shadow_range(value);
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
        if (button->value)
            shadow_dw->shadow_ptr->enable_show_textures();
        else
            shadow_dw->shadow_ptr->disable_show_textures();
        render_data_context rend_data_ctx(GL_REND_STATE_PRE_3D);
        shadow_dw->shadow_ptr->clear(rend_data_ctx.state);
    }
}
