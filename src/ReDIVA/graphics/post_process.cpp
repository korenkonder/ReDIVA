/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "post_process.hpp"
#include "../../CRE/light_param.hpp"
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
    dw::Slider* saturate_coeff_slider;
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
    tone_map_method tone_map;
    float_t exposure;
    int32_t auto_exposure;
    float_t gamma;
    int32_t saturate_power;
    float_t saturate_coeff;
    int32_t taa;
    int32_t mlaa;
    rndr::Render::MagFilterType mag_filter;
    bool set_for_each_cut;
    vec3 lens;
    vec3 radius;
    vec3 intensity;
    rndr::Render* rend;

    dwPostProcess();
    virtual ~dwPostProcess() override;

    virtual void Hide() override;

    virtual void ResetData();

    void DofReset();

    static void AutoExposureCallback(dw::Widget* data);
    static void DofCallback(dw::Widget* data);
    static void ExposureCallback(dw::Widget* data);
    static void GammaCallback(dw::Widget* data);
    static void IntensityCallback(dw::Widget* data);
    static void LensCallback(dw::Widget* data);
    static void LoadCallback(dw::Widget* data);
    static void MagFilterCallback(dw::Widget* data);
    static void MorphologicalAACallback(dw::Widget* data);
    static void RadiusCallback(dw::Widget* data);
    static void SaturateCoeffCallback(dw::Widget* data);
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
    vec3 color;
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
    vec3 start;
    vec3 end;
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
        dw_post_process->sub_1402F38B0();
    }
    else
        dw_post_process->Disp();
}

dwPostProcess::dwPostProcess() {
    data = 0;
    rend = &rctx_ptr->render;

    tone_map = rend->get_tone_map();
    exposure = rend->get_exposure();
    auto_exposure = rend->get_auto_exposure();
    gamma = rend->get_gamma();
    saturate_power = rend->get_saturate_power();
    saturate_coeff = rend->get_saturate_coeff();
    taa = rend->get_taa();
    mlaa = rend->get_mlaa();
    mag_filter = rend->get_mag_filter();
    lens = rend->get_lens();
    radius = rend->get_radius();
    intensity = rend->get_intensity();
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
    taa_button->SetValue(!!taa);
    taa_button->callback = dwPostProcess::TemporalAACallback;

    mlaa_button = new dw::Button(this, dw::CHECKBOX);
    mlaa_button->SetText("Morphological AA");
    mlaa_button->SetValue(!!mlaa);
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

    dw::Button* tone_trans_button = new dw::Button(this, dw::FLAG_8);
    tone_trans_button->SetText("TONE TRANS");
    tone_trans_button->callback = (dw::Widget::Callback)dw_tone_trans_init;

    dw::Button* scene_fade_button = new dw::Button(this, dw::FLAG_8);
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
    tone_map_list->SetItemIndex(tone_map);
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

    saturate_coeff_slider = dw::Slider::Create(glow_param_group,
        dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_size.y, "Saturate2");
    saturate_coeff_slider->SetParams(saturate_coeff, 0.0f, 1.0f, 0.2f, 0.01f, 0.1f);
    saturate_coeff_slider->format = "%4.2f";
    saturate_coeff_slider->AddSelectionListener(new dw::SelectionListenerOnHook(dwPostProcess::SaturateCoeffCallback));

    (new dw::Label(glow_param_group))->SetText("FLARE");

    lens_sliders[0] = dw::Slider::Create(glow_param_group,
        dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_size.y, "Flare A");
    lens_sliders[0]->SetParams(lens.x, 0.0f, 1.0f, 0.2f, 0.01f, 0.1f);
    lens_sliders[0]->format = "%4.2f";
    lens_sliders[0]->AddSelectionListener(new dw::SelectionListenerOnHook(dwPostProcess::LensCallback));
    lens_sliders[0]->callback_data.i64 = 0;

    lens_sliders[1] = dw::Slider::Create(glow_param_group,
        dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_size.y, "Shaft A");
    lens_sliders[1]->SetParams(lens.y, 0.0f, 1.0f, 0.2f, 0.01f, 0.1f);
    lens_sliders[1]->format = "%4.2f";
    lens_sliders[1]->AddSelectionListener(new dw::SelectionListenerOnHook(dwPostProcess::LensCallback));
    lens_sliders[1]->callback_data.i64 = 1;

    lens_sliders[2] = dw::Slider::Create(glow_param_group,
        dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_size.y, "Ghost A");
    lens_sliders[2]->SetParams(lens.z, 0.0f, 1.0f, 0.2f, 0.01f, 0.1f);
    lens_sliders[2]->format = "%4.2f";
    lens_sliders[2]->AddSelectionListener(new dw::SelectionListenerOnHook(dwPostProcess::LensCallback));
    lens_sliders[2]->callback_data.i64 = 2;

    (new dw::Label(glow_param_group))->SetText("GLARE");

    radius_sliders[0] = dw::Slider::Create(glow_param_group,
        dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_size.y, "Radius R");
    radius_sliders[0]->SetParams(radius.x, 1.0f, 3.0f, 0.5f, 0.1f, 0.1f);
    radius_sliders[0]->format = "%4.2f";
    radius_sliders[0]->AddSelectionListener(new dw::SelectionListenerOnHook(dwPostProcess::RadiusCallback));
    radius_sliders[0]->callback_data.i64 = 0;

    radius_sliders[1] = dw::Slider::Create(glow_param_group,
        dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_size.y, "Radius G");
    radius_sliders[1]->SetParams(radius.y, 1.0f, 3.0f, 0.5f, 0.1f, 0.1f);
    radius_sliders[1]->format = "%4.2f";
    radius_sliders[1]->AddSelectionListener(new dw::SelectionListenerOnHook(dwPostProcess::RadiusCallback));
    radius_sliders[1]->callback_data.i64 = 1;

    radius_sliders[2] = dw::Slider::Create(glow_param_group,
        dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_size.y, "Radius B");
    radius_sliders[2]->SetParams(radius.z, 1.0f, 3.0f, 0.5f, 0.1f, 0.1f);
    radius_sliders[2]->format = "%4.2f";
    radius_sliders[2]->AddSelectionListener(new dw::SelectionListenerOnHook(dwPostProcess::RadiusCallback));
    radius_sliders[2]->callback_data.i64 = 2;

    intensity_sliders[0] = dw::Slider::Create(glow_param_group,
        dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_size.y, "Inten  R");
    intensity_sliders[0]->SetParams(intensity.x, 0.0f, 2.0f, 0.5f, 0.05f, 0.1f);
    intensity_sliders[0]->format = "%4.2f";
    intensity_sliders[0]->AddSelectionListener(new dw::SelectionListenerOnHook(dwPostProcess::IntensityCallback));
    intensity_sliders[0]->callback_data.i64 = 0;

    intensity_sliders[1] = dw::Slider::Create(glow_param_group,
        dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_size.y, "Inten  G");
    intensity_sliders[1]->SetParams(intensity.y, 0.0f, 2.0f, 0.5f, 0.05f, 0.1f);
    intensity_sliders[1]->format = "%4.2f";
    intensity_sliders[1]->AddSelectionListener(new dw::SelectionListenerOnHook(dwPostProcess::IntensityCallback));
    intensity_sliders[1]->callback_data.i64 = 1;

    intensity_sliders[2] = dw::Slider::Create(glow_param_group,
        dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 192.0f, glyph_size.y, "Inten  B");
    intensity_sliders[2]->SetParams(intensity.z, 0.0f, 2.0f, 0.5f, 0.05f, 0.1f);
    intensity_sliders[2]->format = "%4.2f";
    intensity_sliders[2]->AddSelectionListener(new dw::SelectionListenerOnHook(dwPostProcess::IntensityCallback));
    intensity_sliders[2]->callback_data.i64 = 2;

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

    save_button = new dw::Button(file_comp, dw::FLAG_8);
    save_button->SetText("SAVE");
    save_button->callback_data.v64 = data;
    save_button->callback = dwPostProcess::SaveCallback;

    load_button = new dw::Button(file_comp, dw::FLAG_8);
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
    rend = &rctx_ptr->render;

    exposure = rend->get_exposure();
    auto_exposure = rend->get_auto_exposure();
    gamma = rend->get_gamma();
    saturate_power = rend->get_saturate_power();
    saturate_coeff = rend->get_saturate_coeff();
    lens = rend->get_lens();
    radius = rend->get_radius();
    intensity = rend->get_intensity();

    exposure_slider->SetValue(exposure);
    auto_exposure_button->SetValue(auto_exposure != 0);
    gamma_slider->SetValue(gamma);
    saturate_power_slider->SetValue((float_t)saturate_power);
    saturate_coeff_slider->SetValue(saturate_coeff);

    for (int32_t i = 0; i < 3; i++)
        lens_sliders[i]->SetValue(((float_t*)&lens)[i]);

    for (int32_t i = 0; i < 3; i++)
        radius_sliders[i]->SetValue(((float_t*)&radius)[i]);

    for (int32_t i = 0; i < 3; i++)
        intensity_sliders[i]->SetValue(((float_t*)&intensity)[i]);

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
    const bool dof_enable_physical_dof = dw_post_process->dof_enable_physical_dof->value;

    dw_post_process->dof_enable_dof->SetEnabled(enable_dof);
    dw_post_process->dof_enable_physical_dof->SetEnabled(enable_dof);
    dw_post_process->dof_auto_focus->SetEnabled(enable_dof && dof_enable_physical_dof);
    dw_post_process->dof_distance_to_focus->SetEnabled(enable_dof && dof_enable_physical_dof);
    dw_post_process->dof_focal_length->SetEnabled(enable_dof && dof_enable_physical_dof);
    dw_post_process->dof_f_number->SetEnabled(enable_dof && dof_enable_physical_dof);
    dw_post_process->dof_f2_distance_to_focus->SetEnabled(enable_dof && !dof_enable_physical_dof);
    dw_post_process->dof_f2_focus_range->SetEnabled(enable_dof && !dof_enable_physical_dof);
    dw_post_process->dof_f2_fuzzing_range->SetEnabled(enable_dof && !dof_enable_physical_dof);
    dw_post_process->dof_f2_ratio->SetEnabled(enable_dof && !dof_enable_physical_dof);
    show_face_query_set(dw_post_process->show_face_query_button->value);
}

void dwPostProcess::ExposureCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider) {
        dw_post_process->exposure = slider->GetValue();
        dw_post_process->rend->set_exposure(dw_post_process->exposure);
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
        ((float_t*)&dw_post_process->intensity)[slider->callback_data.i32] = slider->GetValue();
        dw_post_process->rend->set_intensity(dw_post_process->intensity);
    }
}

void dwPostProcess::LensCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider) {
        ((float_t*)&dw_post_process->lens)[slider->callback_data.i32] = slider->GetValue();
        dw_post_process->rend->set_lens(dw_post_process->lens);
    }
}

void dwPostProcess::LoadCallback(dw::Widget* data) {

}

void dwPostProcess::MagFilterCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider) {
        dw_post_process->mag_filter = (rndr::Render::MagFilterType)(int32_t)slider->GetValue();
        dw_post_process->rend->set_mag_filter(dw_post_process->mag_filter);
    }
}

void dwPostProcess::MorphologicalAACallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button) {
        dw_post_process->mlaa = button->value ? 1 : 0;
        dw_post_process->rend->set_mlaa(dw_post_process->mlaa);
    }
}

void dwPostProcess::RadiusCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider) {
        ((float_t*)&dw_post_process->radius)[slider->callback_data.i32] = slider->GetValue();
        dw_post_process->rend->set_radius(dw_post_process->radius);
    }
}

void dwPostProcess::SaturateCoeffCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider) {
        dw_post_process->saturate_coeff = slider->GetValue();
        dw_post_process->rend->set_saturate_coeff(dw_post_process->saturate_coeff, 0, false);
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
        dw_post_process->taa = button->value ? 1 : 0;
        dw_post_process->rend->set_taa(dw_post_process->taa);
    }
}

void dwPostProcess::ToneMapCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider) {
        dw_post_process->tone_map = (tone_map_method)(int32_t)slider->GetValue();
        dw_post_process->rend->set_tone_map(dw_post_process->tone_map);
    }
}

dwSceneFade::dwSceneFade() {
    rend = &rctx_ptr->render;

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

    color_slider[0] = dw::Slider::Create(color_comp,
        dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 100.0f, glyph_size.y, " R ");
    color_slider[0]->SetParams(0.0f, 0.0f, 1.0f, 0.1f, 0.01f, 0.1f);
    color_slider[0]->format = "%4.2f";
    color_slider[0]->AddSelectionListener(new dw::SelectionListenerOnHook(dwSceneFade::ColorCallback));
    color_slider[0]->callback_data.i64 = 0;

    color_slider[1] = dw::Slider::Create(color_comp,
        dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 100.0f, glyph_size.y, " G ");
    color_slider[1]->SetParams(0.0f, 0.0f, 1.0f, 0.1f, 0.01f, 0.1f);
    color_slider[1]->format = "%4.2f";
    color_slider[1]->AddSelectionListener(new dw::SelectionListenerOnHook(dwSceneFade::ColorCallback));
    color_slider[1]->callback_data.i64 = 1;

    color_slider[2] = dw::Slider::Create(color_comp,
        dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 100.0f, glyph_size.y, " B ");
    color_slider[2]->SetParams(0.0f, 0.0f, 1.0f, 0.1f, 0.01f, 0.1f);
    color_slider[2]->format = "%4.2f";
    color_slider[2]->AddSelectionListener(new dw::SelectionListenerOnHook(dwSceneFade::ColorCallback));
    color_slider[2]->callback_data.i64 = 2;

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

    dw::Button* reset_button = new dw::Button(reset_comp, dw::FLAG_8);
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
    color = rend->get_scene_fade_color();
    alpha = rend->get_scene_fade_alpha();
    blend_func = rend->get_scene_fade_blend_func();

    for (int32_t i = 0; i < 3; i++)
        color_slider[i]->SetValue(((float_t*)&color)[i]);

    alpha_slider->SetValue(alpha);
    blend_func_list->SetItemIndex(blend_func);
}

void dwSceneFade::SetData() {
    rend->set_scene_fade_alpha(alpha, 0);
    rend->set_scene_fade_color(color, 0);
    rend->set_scene_fade_blend_func(blend_func, 0);
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
        ((float_t*)&dw_scene_fade->color)[slider->callback_data.i32] = slider->GetValue();
    dw_scene_fade->SetData();
}

void dwSceneFade::ResetCallback(dw::Widget* data) {
    rctx_ptr->render.reset_scene_fade(0);
    dw_scene_fade->ResetData();
}

dwToneTrans::dwToneTrans() {
    rend = &rctx_ptr->render;

    SetText("TONE TRANS");

    vec2 glyph_size = font.GetFontGlyphSize();

    dw::Composite* start_comp = new dw::Composite(this);
    start_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    (new dw::Label(start_comp))->SetText("START ");

    start_slider[0] = dw::Slider::Create(start_comp,
        dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 100.0f, glyph_size.y, " R ");
    start_slider[0]->SetParams(0.0f, 0.0f, 1.0f, 0.1f, 0.01f, 0.1f);
    start_slider[0]->format = "%4.2f";
    start_slider[0]->AddSelectionListener(new dw::SelectionListenerOnHook(dwToneTrans::StartCallback));
    start_slider[0]->callback_data.i64 = 0;

    start_slider[1] = dw::Slider::Create(start_comp,
        dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 100.0f, glyph_size.y, " G ");
    start_slider[1]->SetParams(0.0f, 0.0f, 1.0f, 0.1f, 0.01f, 0.1f);
    start_slider[1]->format = "%4.2f";
    start_slider[1]->AddSelectionListener(new dw::SelectionListenerOnHook(dwToneTrans::StartCallback));
    start_slider[1]->callback_data.i64 = 1;

    start_slider[2] = dw::Slider::Create(start_comp,
        dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 100.0f, glyph_size.y, " B ");
    start_slider[2]->SetParams(0.0f, 0.0f, 1.0f, 0.1f, 0.01f, 0.1f);
    start_slider[2]->format = "%4.2f";
    start_slider[2]->AddSelectionListener(new dw::SelectionListenerOnHook(dwToneTrans::StartCallback));
    start_slider[2]->callback_data.i64 = 2;

    dw::Composite* end_comp = new dw::Composite(this);
    end_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    (new dw::Label(end_comp))->SetText("END   ");

    end_slider[0] = dw::Slider::Create(end_comp,
        dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 100.0f, glyph_size.y, " R ");
    end_slider[0]->SetParams(1.0f, 0.0f, 1.0f, 0.1f, 0.01f, 0.1f);
    end_slider[0]->format = "%4.2f";
    end_slider[0]->AddSelectionListener(new dw::SelectionListenerOnHook(dwToneTrans::EndCallback));
    end_slider[0]->callback_data.i64 = 0;

    end_slider[1] = dw::Slider::Create(end_comp,
        dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 100.0f, glyph_size.y, " G ");
    end_slider[1]->SetParams(1.0f, 0.0f, 1.0f, 0.1f, 0.01f, 0.1f);
    end_slider[1]->format = "%4.2f";
    end_slider[1]->AddSelectionListener(new dw::SelectionListenerOnHook(dwToneTrans::EndCallback));
    end_slider[1]->callback_data.i64 = 1;

    end_slider[2] = dw::Slider::Create(end_comp,
        dw::Flags(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 100.0f, glyph_size.y, " B ");
    end_slider[2]->SetParams(1.0f, 0.0f, 1.0f, 0.1f, 0.01f, 0.1f);
    end_slider[2]->format = "%4.2f";
    end_slider[2]->AddSelectionListener(new dw::SelectionListenerOnHook(dwToneTrans::EndCallback));
    end_slider[2]->callback_data.i64 = 2;

    dw::Composite* reset_comp = new dw::Composite(this);
    reset_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    dw::Button* reset_button = new dw::Button(reset_comp, dw::FLAG_8);
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
        start_slider[i]->SetValue(((float_t*)&start)[i]);
        end_slider[i]->SetValue(((float_t*)&end)[i]);
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
        if (((float_t*)&dw_tone_trans->start)[index] != value)
            ((float_t*)&dw_tone_trans->end)[index] = value;
    }
    dw_tone_trans->SetData();
}

void dwToneTrans::ResetCallback(dw::Widget* data) {
    rctx_ptr->render.reset_tone_trans(0);
    dw_tone_trans->ResetData();
}

void dwToneTrans::StartCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider) {
        float_t value = slider->GetValue();
        int32_t index = slider->callback_data.i32;
        if (((float_t*)&dw_tone_trans->end)[index] != value)
            ((float_t*)&dw_tone_trans->start)[index] = value;
    }
    dw_tone_trans->SetData();
}

static void dw_scene_fade_init() {
    if (!dw_scene_fade) {
        dw_scene_fade = new dwSceneFade;
        dw_scene_fade->UpdateLayout();
        dw_scene_fade->sub_1402F38B0();
    }
    else
        dw_scene_fade->Disp();
}

static void dw_tone_trans_init() {
    if (!dw_tone_trans) {
        dw_tone_trans = new dwToneTrans;
        dw_tone_trans->UpdateLayout();
        dw_tone_trans->sub_1402F38B0();
    }
    else
        dw_tone_trans->Disp();
}
