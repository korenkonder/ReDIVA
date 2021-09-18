/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "render_settings.h"
#include "../../../CRE/lock.h"
#include "../../../CRE/static_var.h"
#include "../../../CRE/timer.h"
#include "../imgui_helper.h"

typedef struct render_settings_struct {
    bool dispose;
    bool disposed;
    bool imgui_focus;
} render_settings_struct;

extern int32_t width;
extern int32_t height;
extern bool input_locked;

extern timer control_timer;
extern timer render_timer;
extern timer sound_timer;
extern timer input_timer;

extern double_t render_get_scale();
extern void render_set_scale(double_t value);
extern int32_t render_get_scale_index();
extern void render_set_scale_index(int32_t index);

const char* render_settings_window_title = "Render Settings";

bool render_settings_enabled = false;
static render_settings_struct render_settings;

inline static void render_settings_freq_to_string(double_t freq, char* freq_str, char* ms_str) {
    int32_t freq_int = (int32_t)freq;
    int32_t freq_frac = (int32_t)(fmod(freq, 1.0) * 1000.0);
    double_t ms = 1000.0 / freq;
    int32_t ms_int = (int32_t)ms;
    int32_t ms_frac = (int32_t)(fmod(ms, 1.0) * 1000.0);
    snprintf(freq_str, 0x20, "%4d.%03d", freq_int, freq_frac);
    snprintf(ms_str, 0x20, "%4d.%03d", ms_int, ms_frac);
}

void render_settings_dispose() {
    render_settings_enabled = false;
    render_settings.imgui_focus = false;
    render_settings.dispose = false;
    render_settings.disposed = true;
}

void render_settings_init() {
    if (render_settings_enabled || render_settings.dispose)
        render_settings_dispose();

    render_settings_enabled = true;
}

void render_settings_imgui() {
    if (!render_settings_enabled) {
        if (!render_settings.disposed)
            render_settings.dispose = true;
        return;
    }
    else if (render_settings.disposed)
        render_settings_enabled = true;

    ImGuiIO* io = igGetIO();
    ImGuiStyle* style = igGetStyle();
    ImFont* font = igGetFont();

    float_t w = min((float_t)width / 4.0f, 360.0f);
#if defined(CLOUD_DEV)
    float_t h = min((float_t)height, 248.0f);
#else
    float_t h = min((float_t)height, 164.0f);
#endif

    igSetNextWindowPos(ImVec2_Empty, ImGuiCond_Appearing, ImVec2_Empty);
    igSetNextWindowSize((ImVec2) { w, h }, ImGuiCond_Appearing);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;

    render_settings.imgui_focus = false;
    if (!igBegin(render_settings_window_title, &render_settings_enabled, window_flags)) {
        render_settings.dispose = true;
        igEnd();
        return;
    }

    double_t scale = render_get_scale() * 100.0;
    int32_t scale_index = render_get_scale_index();
    char buf[0x80];
    snprintf(buf, sizeof(buf), "%g%%%%", scale);
    if (imguiColumnSliderInt("Scale", &scale_index, 0, 7, buf, ImGuiSliderFlags_NoInput))
        render_set_scale_index(scale_index);

    int32_t fxaa_preset = sv_fxaa_preset;
    if (imguiColumnSliderInt("FXAA Preset", &fxaa_preset, 3, 5, "%d", ImGuiSliderFlags_NoInput))
        sv_fxaa_preset_set(fxaa_preset);

    bool fxaa = sv_fxaa;
    if (igCheckbox("FXAA", &fxaa))
        sv_fxaa_set(fxaa);

#if defined(CLOUD_DEV)
    igSeparator();

    double_t control_freq = timer_get_freq(&control_timer);
    double_t render_freq = timer_get_freq(&render_timer);
    double_t sound_freq = timer_get_freq(&sound_timer);
    double_t input_freq = timer_get_freq(&input_timer);
    double_t control_freq_hist = timer_get_freq_hist(&control_timer);
    double_t render_freq_hist = timer_get_freq_hist(&render_timer);
    double_t sound_freq_hist = timer_get_freq_hist(&sound_timer);
    double_t input_freq_hist = timer_get_freq_hist(&input_timer);

    char control_freq_str[0x20];
    char render_freq_str[0x20];
    char sound_freq_str[0x20];
    char input_freq_str[0x20];
    char control_ms_str[0x20];
    char render_ms_str[0x20];
    char sound_ms_str[0x20];
    char input_ms_str[0x20];
    render_settings_freq_to_string(control_freq, control_freq_str, control_ms_str);
    render_settings_freq_to_string(render_freq, render_freq_str, render_ms_str);
    render_settings_freq_to_string(sound_freq, sound_freq_str, sound_ms_str);
    render_settings_freq_to_string(input_freq, input_freq_str, input_ms_str);

    char control_freq_hist_str[0x20];
    char render_freq_hist_str[0x20];
    char sound_freq_hist_str[0x20];
    char input_freq_hist_str[0x20];
    char control_ms_hist_str[0x20];
    char render_ms_hist_str[0x20];
    char sound_ms_hist_str[0x20];
    char input_ms_hist_str[0x20];
    render_settings_freq_to_string(control_freq_hist, control_freq_hist_str, control_ms_hist_str);
    render_settings_freq_to_string(render_freq_hist, render_freq_hist_str, render_ms_hist_str);
    render_settings_freq_to_string(sound_freq_hist, sound_freq_hist_str, sound_ms_hist_str);
    render_settings_freq_to_string(input_freq_hist, input_freq_hist_str, input_ms_hist_str);

    igText("Targ:  Control /   Render /    Sound /    Input");
    igText("freq: %s / %s / %s / %s", control_freq_str, render_freq_str,
        sound_freq_str, input_freq_str);
    igText("  ms: %s / %s / %s / %s", control_ms_str, render_ms_str,
        sound_ms_str, input_ms_str);

    igSeparator();

    igText("Curr:  Control /   Render /    Sound /    Input");
    igText("freq: %s / %s / %s / %s", control_freq_hist_str, render_freq_hist_str,
        sound_freq_hist_str, input_freq_hist_str);
    igText("  ms: %s / %s / %s / %s", control_ms_hist_str, render_ms_hist_str,
        sound_ms_hist_str, input_ms_hist_str);
#endif

    igSeparator();

    if (imguiButton("Reset Render Settings", ImVec2_Empty)) {
        render_set_scale(1.0);
        sv_fxaa_preset_set(5);
        sv_fxaa_set(false);
    }

    render_settings.imgui_focus |= igIsWindowFocused(0);
    igEnd();
}

void render_settings_input() {
    if (!render_settings_enabled)
        return;

    input_locked |= render_settings.imgui_focus;
}

void render_settings_render() {
    if (render_settings.dispose) {
        render_settings_dispose();
        return;
    }
    else if (!render_settings_enabled)
        return;
}
