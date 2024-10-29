/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "render_settings.hpp"
#include "../../KKdLib/timer.hpp"
#include "../app.hpp"
#include "../dw.hpp"

class RenderSettingsDw : public dw::Shell {
public:
    dw::Label* labels[2];
    dw::Slider* scale_slider;
    char scale_slider_buf[0x20];

    RenderSettingsDw();
    virtual ~RenderSettingsDw() override;
    virtual void Draw() override;

    virtual void Hide() override;

    static void ResetScaleCallback(dw::Widget* data);
    static void ScaleCallback(dw::Widget* data);
};

RenderSettingsDw* render_settings_dw;

extern timer* render_timer;

static void render_settings_freq_to_string(double_t freq,
    char* freq_str, size_t freq_str_len, char* ms_str, size_t ms_str_len);
static void render_settings_scale_to_string(char* str, size_t str_len);

void render_settings_dw_init() {
    if (!render_settings_dw) {
        render_settings_dw = new RenderSettingsDw;
        render_settings_dw->sub_1402F38B0();
    }
    else
        render_settings_dw->Disp();
}

RenderSettingsDw::RenderSettingsDw() : scale_slider_buf() {
    SetText("Render Settings");

    rect.pos = 0.0;
    SetSize({ 280.0f, 140.0f });

    render_settings_scale_to_string(scale_slider_buf, sizeof(scale_slider_buf));

    dw::Group* scale_group = new dw::Group(this);
    scale_group->SetText("Scale");

    scale_slider = dw::Slider::Create(scale_group);
    scale_slider->SetText("");
    scale_slider->format = scale_slider_buf;
    scale_slider->SetParams((float_t)app_get_render_scale_index(),
        0.0f, (float_t)(RENDER_SCALE_MAX - 1), 1.0f, 1.0f, 1.0f);
    scale_slider->AddSelectionListener(new dw::SelectionListenerOnHook(RenderSettingsDw::ScaleCallback));

    dw::Button* reset_scale_button = new dw::Button(scale_group, dw::FLAG_8);
    reset_scale_button->SetText("Reset Scale");
    reset_scale_button->callback = RenderSettingsDw::ResetScaleCallback;

    dw::Group* stats_group = new dw::Group(this);
    stats_group->SetText("Render Stats");

    (new dw::Label(stats_group, dw::FLAG_4000))->SetText("Render:   Target /  Current");

    labels[0] = new dw::Label(stats_group, dw::FLAG_4000);
    labels[0]->SetText("  freq: 0000.000 / 0000.000");
    labels[1] = new dw::Label(stats_group, dw::FLAG_4000);
    labels[1]->SetText("    ms: 0000.000 / 0000.000");

    dw::Composite::UpdateLayout();
}

RenderSettingsDw::~RenderSettingsDw() {

}

void RenderSettingsDw::Draw() {
    char buf[0x100];

    const double_t render_freq = render_timer->get_freq();
    const double_t render_freq_hist = render_timer->get_freq_hist();

    char render_freq_str[0x20];
    char render_ms_str[0x20];
    render_settings_freq_to_string(render_freq, render_freq_str,
        sizeof(render_freq_str), render_ms_str, sizeof(render_ms_str));

    char render_freq_hist_str[0x20];
    char render_ms_hist_str[0x20];
    render_settings_freq_to_string(render_freq_hist, render_freq_hist_str,
        sizeof(render_freq_hist_str), render_ms_hist_str, sizeof(render_ms_hist_str));

    sprintf_s(buf, sizeof(buf), "  freq: %s / %s", render_freq_str, render_freq_hist_str);
    labels[0]->SetText(buf);

    sprintf_s(buf, sizeof(buf), "    ms: %s / %s", render_ms_str, render_ms_hist_str);
    labels[1]->SetText(buf);

    render_settings_scale_to_string(scale_slider_buf, sizeof(scale_slider_buf));

    dw::Shell::Draw();
}

void RenderSettingsDw::Hide() {
    SetDisp();
}

void RenderSettingsDw::ResetScaleCallback(dw::Widget* data) {
    app_set_render_scale_index(RENDER_SCALE_100);
    render_settings_dw->scale_slider->SetValue((float_t)RENDER_SCALE_100);
}

void RenderSettingsDw::ScaleCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider)
        app_set_render_scale_index((int32_t)slider->GetValue());
}

inline static void render_settings_freq_to_string(double_t freq,
    char* freq_str, size_t freq_str_len, char* ms_str, size_t ms_str_len) {
    const int32_t freq_int = (int32_t)freq;
    const int32_t freq_frac = (int32_t)(fmod(freq, 1.0) * 1000.0);
    const double_t ms = 1000.0 / freq;
    const int32_t ms_int = (int32_t)ms;
    const int32_t ms_frac = (int32_t)(fmod(ms, 1.0) * 1000.0);
    sprintf_s(freq_str, freq_str_len, "%4d.%03d", freq_int, freq_frac);
    sprintf_s(ms_str, ms_str_len, "%4d.%03d", ms_int, ms_frac);
}

inline static void render_settings_scale_to_string(char* str, size_t str_len) {
    const double_t scale = app_get_render_scale() * 100.0;
    const int32_t scale_int = (int32_t)scale;
    const int32_t scale_frac = (int32_t)(fmod(scale, 1.0) * 100.0);
    sprintf_s(str, str_len, "%3d.%02d", scale_int, scale_frac);
}
