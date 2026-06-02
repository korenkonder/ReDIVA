/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "post_process.hpp"
#include "../../CRE/renderer/dof.hpp"
#include "../../CRE/light_param.hpp"
#include "../../CRE/render.hpp"
#include "../../CRE/render_context.hpp"
#include "../../CRE/stage.hpp"
#include "../dw.hpp"

extern render_context* rctx_ptr;

class dwPostProcess : public dw::Shell {
public:
    dw::ListBox* tone_map_list;
    dw::Slider* exposure_slider;
    dw::Slider* gamma_slider;
    dw::Slider* saturate_power_slider;
    dw::Slider* saturate_coef_slider;
    dw::ListBox* mag_filter_list;
    dw::Button* taa_button;
    dw::Button* mlaa_button;
    dw::Button* auto_exposure_button;
    dw::Button* set_for_each_cut_button;
    dw::Slider* lens_sliders[3];
    dw::Slider* radius_sliders[3];
    dw::Slider* intensity_sliders[3];
    void* data;
    dw::Button* save_button;
    dw::Button* load_button;
    dw::Slider* dof_distance_to_focus;
    dw::Slider* dof_focal_length;
    dw::Slider* dof_f_number;
    dw::Slider* dof_f2_distance_to_focus;
    dw::Slider* dof_f2_focus_range;
    dw::Slider* dof_f2_fuzzing_range;
    dw::Slider* dof_f2_ratio;
    dw::Button* dof_use_ui_params;
    dw::Button* dof_enable_dof;
    dw::Button* dof_enable_physical_dof;
    dw::Button* dof_auto_focus;
    dw::Button* show_face_query_button;
    int32_t tone_map_method;
    float_t exposure;
    int32_t auto_exposure;
    float_t gamma;
    int32_t saturate_power;
    float_t saturate_coef;
    int32_t temporal_anti_alias;
    int32_t morphological_anti_alias;
    int32_t mag_filter;
    bool set_for_each_cut;
    float_t flare_coef[3];
    float_t sigma[3];
    float_t intensity[3];
    rndr::Render* rend;

    dwPostProcess();
    virtual ~dwPostProcess() override;

    virtual void Hide() override;

    virtual void ResetData();

    void DofReset();

    static void AutoExposureCallback(dw::Widget* data);
    static void DofCallback(dw::Widget* data);
    static void ExposureCallback(dw::Widget* data);
    static void FlareCallback(dw::Widget* data);
    static void GammaCallback(dw::Widget* data);
    static void IntensityCallback(dw::Widget* data);
    static void LoadCallback(dw::Widget* data);
    static void MagFilterCallback(dw::Widget* data);
    static void MorphologicalAACallback(dw::Widget* data);
    static void RadiusCallback(dw::Widget* data);
    static void SaturateCoefCallback(dw::Widget* data);
    static void SaturatePowerCallback(dw::Widget* data);
    static void SaveCallback(dw::Widget* data);
    static void SetForEachCutCallback(dw::Widget* data);
    static void TemporalAACallback(dw::Widget* data);
    static void ToneMapCallback(dw::Widget* data);
};

class dwSceneFade : public dw::Shell {
public:
    dw::Slider* color_slider[3];
    dw::Slider* alpha_slider;
    dw::ListBox* blend_func_list;
    float_t color[3];
    float_t alpha;
    int32_t blend_func;
    rndr::Render* rend;

    dwSceneFade();
    virtual ~dwSceneFade() override;

    virtual void Hide() override;

    void ResetData();
    void SetData();

    static void AlphaCallback(dw::Widget* data);
    static void BlendFuncCallback(dw::Widget* data);
    static void ColorCallback(dw::Widget* data);
    static void ResetCallback(dw::Widget* data);
};

class dwToneTrans : public dw::Shell {
public:
    dw::Slider* start_slider[3];
    dw::Slider* end_slider[3];
    float_t start[3];
    float_t end[3];
    rndr::Render* rend;

    dwToneTrans();
    virtual ~dwToneTrans() override;

    virtual void Hide() override;

    void ResetData();
    void SetData();

    static void EndCallback(dw::Widget* data);
    static void ResetCallback(dw::Widget* data);
    static void StartCallback(dw::Widget* data);
};

dwPostProcess* dw_post_process;
dwSceneFade* dw_scene_fade;
dwToneTrans* dw_tone_trans;

static void dw_scene_fade_init();
static void dw_tone_trans_init();

void dw_post_process_init() {
    if (!dw_post_process) {
        dw_post_process = new dwPostProcess;
        dw_post_process->UpdateLayout();
        dw_post_process->LimitPosDisp();
    }
    else
        dw_post_process->Disp();
}

dwPostProcess::dwPostProcess() {
    data = 0;
    rend = rctx_ptr->render;

    rend->get_tone_map_method(&tone_map_method);
    rend->get_exposure(&exposure);
    rend->get_auto_exposure(&auto_exposure);
    rend->get_gamma(&gamma);
    rend->get_saturate_power(&saturate_power);
    rend->get_saturate_coef(&saturate_coef);
    rend->get_temporal_aa(&temporal_anti_alias);
    rend->get_morphological_aa(&morphological_anti_alias);
    rend->get_mag_filter(&mag_filter);
    rend->get_flare_coef(flare_coef);
    rend->get_sigma(sigma);
    rend->get_intensity(intensity);
    set_for_each_cut = false;

    SetText("POST PROCESS");

    rect.pos = 0.0f;
    SetSize({ 200.0f, 320.0f });

    vec2 glyph_size = font.GetFontGlyphSize();

    (new dw::Label(this))->SetText("MAG filter");

    mag_filter_list = new dw::ListBox(this, dw::MULTISELECT);
    mag_filter_list->AddItem("nearest");
    mag_filter_list->AddItem("bilinear");
    mag_filter_list->AddItem("sharpen(5tap)");
    mag_filter_list->AddItem("sharpen(4tap)");
    mag_filter_list->AddItem("cone(4tap)");
    mag_filter_list->AddItem("cone(2tap)");
    mag_filter_list->SetItemIndex(mag_filter);

    mag_filter_list->AddSelectionListener(new dw::SelectionListenerOnHook(dwPostProcess::MagFilterCallback));

    taa_button = new dw::Button(this, dw::CHECKBOX);
    taa_button->SetText("Temporal AA");
    taa_button->SetValue(!!temporal_anti_alias);
    taa_button->callback = dwPostProcess::TemporalAACallback;

    mlaa_button = new dw::Button(this, dw::CHECKBOX);
    mlaa_button->SetText("Morphological AA");
    mlaa_button->SetValue(!!morphological_anti_alias);
    mlaa_button->callback = dwPostProcess::MorphologicalAACallback;

    const char* set_for_each_cut_text;
    if (dw::translate)
        set_for_each_cut_text = u8"Set for each PV cut";
    else
        set_for_each_cut_text = u8"PVのカットごとに設定";

    set_for_each_cut_button = new dw::Button(this, dw::CHECKBOX);
    set_for_each_cut_button->SetText(set_for_each_cut_text);
    set_for_each_cut_button->SetValue(set_for_each_cut);
    set_for_each_cut_button->callback = dwPostProcess::SetForEachCutCallback;

    dw::Button* tone_trans_button = new dw::Button(this);
    tone_trans_button->SetText("TONE TRANS");
    tone_trans_button->callback = (dw::Widget::Callback)dw_tone_trans_init;

    dw::Button* scene_fade_button = new dw::Button(this);
    scene_fade_button->SetText("SCENE FADE");
    scene_fade_button->callback = (dw::Widget::Callback)dw_scene_fade_init;

    dw::Group* glow_param_group = new dw::Group(this);
    glow_param_group->SetText("GLOW PARAM");

    dw::Composite* tone_map_comp = new dw::Composite(glow_param_group);
    tone_map_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    (new dw::Label(tone_map_comp))->SetText("TONE MAP");

    tone_map_list = new dw::ListBox(tone_map_comp, dw::MULTISELECT);
    tone_map_list->AddItem("YCC EXPONENT");
    tone_map_list->AddItem("RGB LINEAR");
    tone_map_list->AddItem("RGB LINEAR2");
    tone_map_list->SetItemIndex(tone_map_method);
    tone_map_list->AddSelectionListener(new dw::SelectionListenerOnHook(dwPostProcess::ToneMapCallback));

    exposure_slider = dw::Slider::Create(glow_param_group,
        dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_size.y, "Exposure ");
    exposure_slider->SetParams(exposure, 0.0f, 4.0f, 1.0f, 0.02f, 0.1f);
    exposure_slider->format = "%4.2f";
    exposure_slider->AddSelectionListener(new dw::SelectionListenerOnHook(dwPostProcess::ExposureCallback));

    dw::Composite* glow_param_checkbox_comp = new dw::Composite(glow_param_group);
    glow_param_checkbox_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    auto_exposure_button = new dw::Button(glow_param_checkbox_comp, dw::CHECKBOX);
    auto_exposure_button->SetText("AUTO exposure");
    auto_exposure_button->SetValue(!!auto_exposure);
    auto_exposure_button->callback = dwPostProcess::AutoExposureCallback;

    show_face_query_button = new dw::Button(glow_param_checkbox_comp, dw::CHECKBOX);
    show_face_query_button->SetText("show face query");
    show_face_query_button->SetValue(false);
    show_face_query_button->callback = dwPostProcess::DofCallback;
    show_face_query_button->callback_data.v64 = this;

    gamma_slider = dw::Slider::Create(glow_param_group,
        dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_size.y, "Gamma");
    gamma_slider->SetParams(gamma, 0.2f, 2.2f, 0.5f, 0.01f, 0.1f);
    gamma_slider->format = "%4.2f";
    gamma_slider->AddSelectionListener(new dw::SelectionListenerOnHook(dwPostProcess::GammaCallback));

    saturate_power_slider = dw::Slider::Create(glow_param_group,
        dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_size.y, "Saturate1");
    saturate_power_slider->SetParams((float_t)saturate_power, 1.0f, 6.0f, 1.0f, 1.0f, 1.0f);
    saturate_power_slider->format = "%3.1f";
    saturate_power_slider->AddSelectionListener(new dw::SelectionListenerOnHook(dwPostProcess::SaturatePowerCallback));

    saturate_coef_slider = dw::Slider::Create(glow_param_group,
        dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_size.y, "Saturate2");
    saturate_coef_slider->SetParams(saturate_coef, 0.0f, 1.0f, 0.2f, 0.01f, 0.1f);
    saturate_coef_slider->format = "%4.2f";
    saturate_coef_slider->AddSelectionListener(new dw::SelectionListenerOnHook(dwPostProcess::SaturateCoefCallback));

    (new dw::Label(glow_param_group))->SetText("FLARE");

    for (int32_t i = 0; i < 3; i++) {
        static const char* flare_name[] = {
            "Flare A",
            "Shaft A",
            "Ghost A",
        };

        lens_sliders[i] = dw::Slider::Create(glow_param_group,
            dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_size.y, flare_name[i]);
        lens_sliders[i]->SetParams(flare_coef[i], 0.0f, 1.0f, 0.2f, 0.01f, 0.1f);
        lens_sliders[i]->format = "%4.2f";
        lens_sliders[i]->AddSelectionListener(new dw::SelectionListenerOnHook(dwPostProcess::FlareCallback));
        lens_sliders[i]->callback_data.i64 = i;
    }

    (new dw::Label(glow_param_group))->SetText("GLARE");

    for (int32_t i = 0; i < 3; i++) {
        static const char* radius_name[] = {
            "Radius R",
            "Radius G",
            "Radius B",
        };

        radius_sliders[i] = dw::Slider::Create(glow_param_group,
            dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_size.y, radius_name[i]);
        radius_sliders[i]->SetParams(sigma[i], 1.0f, 3.0f, 0.5f, 0.1f, 0.1f);
        radius_sliders[i]->format = "%4.2f";
        radius_sliders[i]->AddSelectionListener(new dw::SelectionListenerOnHook(dwPostProcess::RadiusCallback));
        radius_sliders[i]->callback_data.i64 = i;
    }

    for (int32_t i = 0; i < 3; i++) {
        static const char* intensity_name[] = {
            "Inten  R",
            "Inten  G",
            "Inten  B",
        };

        intensity_sliders[i] = dw::Slider::Create(glow_param_group,
            dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_size.y, intensity_name[i]);
        intensity_sliders[i]->SetParams(intensity[i], 0.0f, 2.0f, 0.5f, 0.05f, 0.1f);
        intensity_sliders[i]->format = "%4.2f";
        intensity_sliders[i]->AddSelectionListener(new dw::SelectionListenerOnHook(dwPostProcess::IntensityCallback));
        intensity_sliders[i]->callback_data.i64 = i;
    }

    (new dw::Label(glow_param_group))->SetText("DOF");

    dof_use_ui_params = new dw::Button(glow_param_group, dw::CHECKBOX);
    dof_use_ui_params->SetText("use UI params");
    dof_use_ui_params->SetValue(false);
    dof_use_ui_params->callback = dwPostProcess::DofCallback;
    dof_use_ui_params->callback_data.v64 = this;

    dof_enable_dof = new dw::Button(glow_param_group, dw::CHECKBOX);
    dof_enable_dof->SetText("enable DOF");
    dof_enable_dof->SetValue(false);
    dof_enable_dof->SetEnabled(false);
    dof_enable_dof->callback = dwPostProcess::DofCallback;
    dof_enable_dof->callback_data.v64 = this;

    dof_enable_physical_dof = new dw::Button(glow_param_group, dw::CHECKBOX);
    dof_enable_physical_dof->SetText("enable physical DOF");
    dof_enable_physical_dof->SetValue(false);
    dof_enable_physical_dof->SetEnabled(false);
    dof_enable_physical_dof->callback = dwPostProcess::DofCallback;
    dof_enable_physical_dof->callback_data.v64 = this;

    dof_auto_focus = new dw::Button(glow_param_group, dw::CHECKBOX);
    dof_auto_focus->SetText("auto focus");
    dof_auto_focus->SetEnabled(false);
    dof_auto_focus->callback = dwPostProcess::DofCallback;
    dof_auto_focus->callback_data.v64 = this;

    dof_distance_to_focus = dw::Slider::Create(glow_param_group,
        dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_size.y, "distance to focus[m]");
    dof_distance_to_focus->SetParams(10.0f, 0.01f, 30.0f, 0.5f, 0.01f, 1.0f);
    dof_distance_to_focus->format = "%4.2f";
    dof_distance_to_focus->SetEnabled(false);
    dof_distance_to_focus->AddSelectionListener(new dw::SelectionListenerOnHook(dwPostProcess::DofCallback));
    dof_distance_to_focus->callback_data.v64 = this;

    dof_focal_length = dw::Slider::Create(glow_param_group,
        dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_size.y, "focal length[mm]");
    dof_focal_length->SetParams(50.0f, 0.1f, 100.0f, 0.5f, 0.01f, 1.0f);
    dof_focal_length->format = "%4.2f";
    dof_focal_length->SetEnabled(false);
    dof_focal_length->AddSelectionListener(new dw::SelectionListenerOnHook(dwPostProcess::DofCallback));
    dof_focal_length->callback_data.v64 = this;

    dof_f_number = dw::Slider::Create(glow_param_group,
        dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_size.y, "F-Number");
    dof_f_number->SetParams(8.0f, 0.1f, 40.0f, 0.5f, 0.01f, 1.0f);
    dof_f_number->format = "%4.2f";
    dof_f_number->SetEnabled(false);
    dof_f_number->AddSelectionListener(new dw::SelectionListenerOnHook(dwPostProcess::DofCallback));
    dof_f_number->callback_data.v64 = this;

    dof_f2_distance_to_focus = dw::Slider::Create(glow_param_group,
        dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_size.y, "f2 distance to focus[m]");
    dof_f2_distance_to_focus->SetParams(0.0f, 0.0f, 30.0f, 0.5f, 0.01f, 1.0f);
    dof_f2_distance_to_focus->format = "%4.2f";
    dof_f2_distance_to_focus->SetEnabled(false);
    dof_f2_distance_to_focus->AddSelectionListener(new dw::SelectionListenerOnHook(dwPostProcess::DofCallback));
    dof_f2_distance_to_focus->callback_data.v64 = this;

    dof_f2_focus_range = dw::Slider::Create(glow_param_group,
        dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_size.y, "f2 focus range[m]");
    dof_f2_focus_range->SetParams(1.0f, 0.0f, 10.0f, 0.5f, 0.01f, 1.0f);
    dof_f2_focus_range->format = "%4.2f";
    dof_f2_focus_range->SetEnabled(false);
    dof_f2_focus_range->AddSelectionListener(new dw::SelectionListenerOnHook(dwPostProcess::DofCallback));
    dof_f2_focus_range->callback_data.v64 = this;

    dof_f2_fuzzing_range = dw::Slider::Create(glow_param_group,
        dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_size.y, "f2 fuzzing range[m]");
    dof_f2_fuzzing_range->SetParams(1.0f, 0.0f, 10.0f, 0.5f, 0.01f, 1.0f);
    dof_f2_fuzzing_range->format = "%4.2f";
    dof_f2_fuzzing_range->SetEnabled(false);
    dof_f2_fuzzing_range->AddSelectionListener(new dw::SelectionListenerOnHook(dwPostProcess::DofCallback));
    dof_f2_fuzzing_range->callback_data.v64 = this;

    dof_f2_ratio = dw::Slider::Create(glow_param_group,
        dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_size.y, "f2 ratio");
    dof_f2_ratio->SetParams(1.0f, 0.0f, 1.0f, 0.5f, 0.01f, 1.0f);
    dof_f2_ratio->format = "%4.2f";
    dof_f2_ratio->SetEnabled(false);
    dof_f2_ratio->AddSelectionListener(new dw::SelectionListenerOnHook(dwPostProcess::DofCallback));
    dof_f2_ratio->callback_data.v64 = this;

    dof_use_ui_params->SetFont(dw::p_font_type_6x12);
    dof_enable_dof->SetFont(dw::p_font_type_6x12);
    dof_enable_physical_dof->SetFont(dw::p_font_type_6x12);
    dof_auto_focus->SetFont(dw::p_font_type_6x12);
    show_face_query_button->SetFont(dw::p_font_type_6x12);

    dof_distance_to_focus->SetFont(dw::p_font_type_6x12);
    dof_focal_length->SetFont(dw::p_font_type_6x12);
    dof_f_number->SetFont(dw::p_font_type_6x12);
    dof_f2_distance_to_focus->SetFont(dw::p_font_type_6x12);
    dof_f2_focus_range->SetFont(dw::p_font_type_6x12);
    dof_f2_fuzzing_range->SetFont(dw::p_font_type_6x12);
    dof_f2_ratio->SetFont(dw::p_font_type_6x12);

    DofReset();

    dw::Composite* file_comp = new dw::Composite(glow_param_group);
    file_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    save_button = new dw::Button(file_comp);
    save_button->SetText("SAVE");
    save_button->callback_data.v64 = data;
    save_button->callback = dwPostProcess::SaveCallback;

    load_button = new dw::Button(file_comp);
    load_button->SetText("LOAD");
    load_button->callback_data.v64 = data;
    load_button->callback = dwPostProcess::LoadCallback;
}

dwPostProcess::~dwPostProcess() {

}

void dwPostProcess::Hide() {
    dw::Shell::SetDisp();
}

void dwPostProcess::ResetData() {
    rend = rctx_ptr->render;

    rend->get_exposure(&exposure);
    rend->get_auto_exposure(&auto_exposure);
    rend->get_gamma(&gamma);
    rend->get_saturate_power(&saturate_power);
    rend->get_saturate_coef(&saturate_coef);
    rend->get_flare_coef(flare_coef);
    rend->get_sigma(sigma);
    rend->get_intensity(intensity);

    exposure_slider->SetValue(exposure);
    auto_exposure_button->SetValue(auto_exposure != 0);
    gamma_slider->SetValue(gamma);
    saturate_power_slider->SetValue((float_t)saturate_power);
    saturate_coef_slider->SetValue(saturate_coef);

    for (int32_t i = 0; i < 3; i++)
        lens_sliders[i]->SetValue(flare_coef[i]);

    for (int32_t i = 0; i < 3; i++)
        radius_sliders[i]->SetValue(sigma[i]);

    for (int32_t i = 0; i < 3; i++)
        intensity_sliders[i]->SetValue(intensity[i]);

    DofReset();
}

void dwPostProcess::DofReset() {
    dof_debug debug;
    dof_debug_get(&debug);
    dof_use_ui_params->SetValue(debug.flags & DOF_DEBUG_USE_UI_PARAMS);
    dof_enable_dof->SetValue((debug.flags & DOF_DEBUG_ENABLE_DOF) != 0);
    dof_enable_physical_dof->SetValue((debug.flags & DOF_DEBUG_ENABLE_PHYS_DOF) != 0);
    dof_auto_focus->SetValue((debug.flags & DOF_DEBUG_AUTO_FOCUS) != 0);
    dof_distance_to_focus->SetValue(debug.focus);
    dof_focal_length->SetValue(debug.focal_length * 1000.0f);
    dof_f_number->SetValue(debug.f_number);
    dof_f2_distance_to_focus->SetValue(debug.f2.focus);
    dof_f2_focus_range->SetValue(debug.f2.focus_range);
    dof_f2_fuzzing_range->SetValue(debug.f2.fuzzing_range);
    dof_f2_ratio->SetValue(debug.f2.ratio);
    show_face_query_button->SetValue(show_face_query_get());
}

void dwPostProcess::AutoExposureCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button) {
        dw_post_process->auto_exposure = button->value;
        dw_post_process->rend->set_auto_exposure(dw_post_process->auto_exposure);
    }
}

void dwPostProcess::DofCallback(dw::Widget* data) {
    dwPostProcess* dw_post_process = (dwPostProcess*)data->callback_data.v64;

    dof_debug debug;
    debug.flags = (dof_debug_flags)0;
    if (dw_post_process->dof_use_ui_params->value)
        enum_or(debug.flags, DOF_DEBUG_USE_UI_PARAMS);
    if (dw_post_process->dof_enable_dof->value)
        enum_or(debug.flags, DOF_DEBUG_ENABLE_DOF);
    if (dw_post_process->dof_enable_physical_dof->value)
        enum_or(debug.flags, DOF_DEBUG_ENABLE_PHYS_DOF);
    if (dw_post_process->dof_auto_focus->value)
        enum_or(debug.flags, DOF_DEBUG_AUTO_FOCUS);

    debug.focus = dw_post_process->dof_distance_to_focus->GetValue();
    debug.focal_length = dw_post_process->dof_focal_length->GetValue() * 0.001f;
    debug.f_number = dw_post_process->dof_f_number->GetValue();
    debug.f2.focus = dw_post_process->dof_f2_distance_to_focus->GetValue();
    debug.f2.focus_range = dw_post_process->dof_f2_focus_range->GetValue();
    debug.f2.fuzzing_range = dw_post_process->dof_f2_fuzzing_range->GetValue();
    debug.f2.ratio = dw_post_process->dof_f2_ratio->GetValue();
    dof_debug_set(&debug);

    const bool enable_dof = dw_post_process->dof_use_ui_params->value;
    const bool enable_physical_dof = dw_post_process->dof_enable_physical_dof->value;

    dw_post_process->dof_enable_dof->SetEnabled(enable_dof);
    dw_post_process->dof_enable_physical_dof->SetEnabled(enable_dof);
    dw_post_process->dof_auto_focus->SetEnabled(enable_dof && enable_physical_dof);
    dw_post_process->dof_distance_to_focus->SetEnabled(enable_dof && enable_physical_dof);
    dw_post_process->dof_focal_length->SetEnabled(enable_dof && enable_physical_dof);
    dw_post_process->dof_f_number->SetEnabled(enable_dof && enable_physical_dof);
    dw_post_process->dof_f2_distance_to_focus->SetEnabled(enable_dof && !enable_physical_dof);
    dw_post_process->dof_f2_focus_range->SetEnabled(enable_dof && !enable_physical_dof);
    dw_post_process->dof_f2_fuzzing_range->SetEnabled(enable_dof && !enable_physical_dof);
    dw_post_process->dof_f2_ratio->SetEnabled(enable_dof && !enable_physical_dof);
    show_face_query_set(dw_post_process->show_face_query_button->value);
}

void dwPostProcess::ExposureCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider) {
        dw_post_process->exposure = slider->GetValue();
        dw_post_process->rend->set_exposure(dw_post_process->exposure);
    }
}

void dwPostProcess::FlareCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider) {
        dw_post_process->flare_coef[slider->callback_data.i32] = slider->GetValue();
        dw_post_process->rend->set_flare_coef(dw_post_process->flare_coef);
    }
}

void dwPostProcess::GammaCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider) {
        dw_post_process->gamma = slider->GetValue();
        dw_post_process->rend->set_gamma(dw_post_process->gamma);
    }
}

void dwPostProcess::IntensityCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider) {
        dw_post_process->intensity[slider->callback_data.i32] = slider->GetValue();
        dw_post_process->rend->set_intensity(dw_post_process->intensity);
    }
}

void dwPostProcess::LoadCallback(dw::Widget* data) {

}

void dwPostProcess::MagFilterCallback(dw::Widget* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box) {
        dw_post_process->mag_filter = (int32_t)list_box->list->selected_item;
        dw_post_process->rend->set_mag_filter(dw_post_process->mag_filter);
    }
}

void dwPostProcess::MorphologicalAACallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button) {
        dw_post_process->morphological_anti_alias = button->value ? 1 : 0;
        dw_post_process->rend->set_morphological_aa(dw_post_process->morphological_anti_alias);
    }
}

void dwPostProcess::RadiusCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider) {
        dw_post_process->sigma[slider->callback_data.i32] = slider->GetValue();
        dw_post_process->rend->set_sigma(dw_post_process->sigma);
    }
}

void dwPostProcess::SaturateCoefCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider) {
        dw_post_process->saturate_coef = slider->GetValue();
        dw_post_process->rend->set_saturate_coef(dw_post_process->saturate_coef, 0, false);
    }
}

void dwPostProcess::SaturatePowerCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider) {
        dw_post_process->saturate_power = (int32_t)slider->GetValue();
        dw_post_process->rend->set_saturate_power(dw_post_process->saturate_power);
    }
}

void dwPostProcess::SaveCallback(dw::Widget* data) {

}

void dwPostProcess::SetForEachCutCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button)
        dw_post_process->set_for_each_cut = button->value;
}

void dwPostProcess::TemporalAACallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button) {
        dw_post_process->temporal_anti_alias = button->value ? 1 : 0;
        dw_post_process->rend->set_temporal_aa(dw_post_process->temporal_anti_alias);
    }
}

void dwPostProcess::ToneMapCallback(dw::Widget* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box) {
        dw_post_process->tone_map_method = (int32_t)list_box->list->selected_item;
        dw_post_process->rend->set_tone_map_method(dw_post_process->tone_map_method);
    }
}

dwSceneFade::dwSceneFade() {
    rend = rctx_ptr->render;

    SetText("SCENE FADE");

    vec2 glyph_size = font.GetFontGlyphSize();

    dw::Composite* alpha_comp = new dw::Composite(this);
    alpha_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    alpha_slider = dw::Slider::Create(alpha_comp,
        dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 100.0f, glyph_size.y, "ALPHA");
    alpha_slider->SetParams(0.0f, 0.0f, 1.0f, 0.1f, 0.01f, 0.1f);
    alpha_slider->format = "%4.2f";
    alpha_slider->AddSelectionListener(new dw::SelectionListenerOnHook(dwSceneFade::AlphaCallback));

    dw::Composite* color_comp = new dw::Composite(this);
    color_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    (new dw::Label(color_comp))->SetText("COLOR ");

    for (int32_t i = 0; i < 3; i++) {
        static const char* color_name[] = {
            " R ",
            " G ",
            " B ",
        };

        color_slider[i] = dw::Slider::Create(color_comp,
            dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 100.0f, glyph_size.y, color_name[i]);
        color_slider[i]->SetParams(0.0f, 0.0f, 1.0f, 0.1f, 0.01f, 0.1f);
        color_slider[i]->format = "%4.2f";
        color_slider[i]->AddSelectionListener(new dw::SelectionListenerOnHook(dwSceneFade::ColorCallback));
        color_slider[i]->callback_data.i64 = i;
    }

    dw::Composite* blend_func_comp = new dw::Composite(this);
    blend_func_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    (new dw::Label(blend_func_comp))->SetText("BLEND FUNC");

    blend_func_list = new dw::ListBox(blend_func_comp, dw::MULTISELECT);
    blend_func_list->AddItem("0: OVER");
    blend_func_list->AddItem("1: MULTI");
    blend_func_list->AddItem("2: PLUS");
    blend_func_list->SetItemIndex(0);
    blend_func_list->AddSelectionListener(new dw::SelectionListenerOnHook(dwSceneFade::BlendFuncCallback));

    dw::Composite* reset_comp = new dw::Composite(this);
    reset_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    dw::Button* reset_button = new dw::Button(reset_comp);
    reset_button->SetText(" RESET ");
    reset_button->callback = dwSceneFade::ResetCallback;

    dw::Composite::UpdateLayout();
    ResetData();
}

dwSceneFade::~dwSceneFade() {

}

void dwSceneFade::Hide() {
    dw::Shell::SetDisp();
}

void dwSceneFade::ResetData() {
    rend->get_fade_color(color);
    rend->get_fade_rate(&alpha);
    rend->get_fade_blend_func(&blend_func);

    for (int32_t i = 0; i < 3; i++)
        color_slider[i]->SetValue(color[i]);
    alpha_slider->SetValue(alpha);
    blend_func_list->SetItemIndex(blend_func);
}

void dwSceneFade::SetData() {
    rend->set_fade_rate(alpha);
    rend->set_fade_color(color);
    rend->set_fade_blend_func(blend_func);
}

void dwSceneFade::AlphaCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider)
        dw_scene_fade->alpha = slider->GetValue();
    dw_scene_fade->SetData();
}

void dwSceneFade::BlendFuncCallback(dw::Widget* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box)
        dw_scene_fade->blend_func = (int32_t)list_box->list->selected_item;
    dw_scene_fade->SetData();
}

void dwSceneFade::ColorCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider)
        dw_scene_fade->color[slider->callback_data.i32] = slider->GetValue();
    dw_scene_fade->SetData();
}

void dwSceneFade::ResetCallback(dw::Widget* data) {
    rctx_ptr->render->set_fade_color_default();
    dw_scene_fade->ResetData();
}

dwToneTrans::dwToneTrans() {
    rend = rctx_ptr->render;

    SetText("TONE TRANS");

    vec2 glyph_size = font.GetFontGlyphSize();

    dw::Composite* start_comp = new dw::Composite(this);
    start_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    (new dw::Label(start_comp))->SetText("START ");

    for (int32_t i = 0; i < 3; i++) {
        static const char* start_name[] = {
            " R ",
            " G ",
            " B ",
        };

        start_slider[i] = dw::Slider::Create(start_comp,
            dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 100.0f, glyph_size.y, start_name[i]);
        start_slider[i]->SetParams(0.0f, 0.0f, 1.0f, 0.1f, 0.01f, 0.1f);
        start_slider[i]->format = "%4.2f";
        start_slider[i]->AddSelectionListener(new dw::SelectionListenerOnHook(dwToneTrans::StartCallback));
        start_slider[i]->callback_data.i64 = i;
    }

    dw::Composite* end_comp = new dw::Composite(this);
    end_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    (new dw::Label(end_comp))->SetText("END   ");

    for (int32_t i = 0; i < 3; i++) {
        static const char* end_name[] = {
            " R ",
            " G ",
            " B ",
        };

        end_slider[i] = dw::Slider::Create(end_comp,
            dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 100.0f, glyph_size.y, end_name[i]);
        end_slider[i]->SetParams(1.0f, 0.0f, 1.0f, 0.1f, 0.01f, 0.1f);
        end_slider[i]->format = "%4.2f";
        end_slider[i]->AddSelectionListener(new dw::SelectionListenerOnHook(dwToneTrans::EndCallback));
        end_slider[i]->callback_data.i64 = i;
    }

    dw::Composite* reset_comp = new dw::Composite(this);
    reset_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    dw::Button* reset_button = new dw::Button(reset_comp);
    reset_button->SetText(" RESET ");
    reset_button->callback = dwToneTrans::ResetCallback;

    dw::Composite::UpdateLayout();
    ResetData();
}

dwToneTrans::~dwToneTrans() {

}

void dwToneTrans::Hide() {
    dw::Shell::SetDisp();
}

void dwToneTrans::ResetData() {
    rend->get_tone_trans(start, end);

    for (int32_t i = 0; i < 3; i++) {
        start_slider[i]->SetValue(start[i]);
        end_slider[i]->SetValue(end[i]);
    }
}

void dwToneTrans::SetData() {
    rend->set_tone_trans(start, end, 0);
}

void dwToneTrans::EndCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider) {
        float_t value = slider->GetValue();
        int32_t index = slider->callback_data.i32;
        if (dw_tone_trans->start[index] != value)
            dw_tone_trans->end[index] = value;
    }
    dw_tone_trans->SetData();
}

void dwToneTrans::ResetCallback(dw::Widget* data) {
    rctx_ptr->render->set_tone_trans_default();
    dw_tone_trans->ResetData();
}

void dwToneTrans::StartCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider) {
        float_t value = slider->GetValue();
        int32_t index = slider->callback_data.i32;
        if (dw_tone_trans->end[index] != value)
            dw_tone_trans->start[index] = value;
    }
    dw_tone_trans->SetData();
}

static void dw_scene_fade_init() {
    if (!dw_scene_fade) {
        dw_scene_fade = new dwSceneFade;
        dw_scene_fade->UpdateLayout();
        dw_scene_fade->LimitPosDisp();
    }
    else
        dw_scene_fade->Disp();
}

static void dw_tone_trans_init() {
    if (!dw_tone_trans) {
        dw_tone_trans = new dwToneTrans;
        dw_tone_trans->UpdateLayout();
        dw_tone_trans->LimitPosDisp();
    }
    else
        dw_tone_trans->Disp();
}
