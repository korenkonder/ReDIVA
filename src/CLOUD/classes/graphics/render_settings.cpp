/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "render_settings.hpp"
#include "../../../CRE/lock.hpp"
#include "../../../CRE/static_var.hpp"
#include "../../../CRE/timer.hpp"
#include "../imgui_helper.hpp"

extern int32_t width;
extern int32_t height;

extern timer* render_timer;
extern timer* sound_timer;

extern size_t render_scale_table_count;
extern double_t render_get_scale();
extern void render_set_scale(double_t value);
extern int32_t render_get_scale_index();
extern void render_set_scale_index(int32_t index);

const char* graphics_render_settings_window_title = "Render Settings##Graphics";

inline static void render_settings_freq_to_string(double_t freq,
    char* freq_str, size_t freq_str_len, char* ms_str, size_t ms_str_len) {
    int32_t freq_int = (int32_t)freq;
    int32_t freq_frac = (int32_t)(fmod(freq, 1.0) * 1000.0);
    double_t ms = 1000.0 / freq;
    int32_t ms_int = (int32_t)ms;
    int32_t ms_frac = (int32_t)(fmod(ms, 1.0) * 1000.0);
    sprintf_s(freq_str, freq_str_len, "%4d.%03d", freq_int, freq_frac);
    sprintf_s(ms_str, ms_str_len, "%4d.%03d", ms_int, ms_frac);
}

bool graphics_render_settings_init(class_data* data, render_context* rctx) {
    graphics_render_settings_dispose(data);
    return true;
}

void graphics_render_settings_imgui(class_data* data) {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    float_t w = 280.0f;
#if defined(CLOUD_DEV)
    float_t h = 196.0f;
#else
    float_t h = 86.0f;
#endif

    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Appearing);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    data->imgui_focus = false;
    bool open = data->flags & CLASS_HIDDEN ? false : true;
    bool collapsed = !ImGui::Begin(graphics_render_settings_window_title, &open, window_flags);
    if (!open) {
        enum_or(data->flags, CLASS_HIDE);
        ImGui::End();
        return;
    }
    else if (collapsed) {
        ImGui::End();
        return;
    }

    double_t scale = render_get_scale() * 100.0;
    int32_t scale_index = render_get_scale_index();
    char buf[0x80];
    sprintf_s(buf, sizeof(buf), "%g%%%%", scale);
    if (ImGui::ColumnSliderInt("Scale", &scale_index, 0,
        (int32_t)render_scale_table_count, buf, ImGuiSliderFlags_NoInput, true))
        render_set_scale_index(scale_index);

#if defined(CLOUD_DEV)
    ImGui::Separator();

    double_t render_freq = render_timer->get_freq();
    double_t sound_freq = sound_timer->get_freq();
    double_t render_freq_hist = render_timer->get_freq_hist();
    double_t sound_freq_hist = sound_timer->get_freq_hist();

    char render_freq_str[0x20];
    char sound_freq_str[0x20];
    char render_ms_str[0x20];
    char sound_ms_str[0x20];
    render_settings_freq_to_string(render_freq, render_freq_str,
        sizeof(render_freq_str), render_ms_str, sizeof(render_ms_str));
    render_settings_freq_to_string(sound_freq, sound_freq_str,
        sizeof(sound_freq_str), sound_ms_str, sizeof(sound_ms_str));

    char render_freq_hist_str[0x20];
    char sound_freq_hist_str[0x20];
    char render_ms_hist_str[0x20];
    char sound_ms_hist_str[0x20];
    render_settings_freq_to_string(render_freq_hist, render_freq_hist_str,
        sizeof(render_freq_hist_str), render_ms_hist_str, sizeof(render_ms_hist_str));
    render_settings_freq_to_string(sound_freq_hist, sound_freq_hist_str,
        sizeof(sound_freq_hist_str), sound_ms_hist_str, sizeof(sound_ms_hist_str));

    ImGui::Text("Targ:   Render /    Sound");
    ImGui::Text("freq: %s / %s", render_freq_str, sound_freq_str);
    ImGui::Text("  ms: %s / %s", render_ms_str, sound_ms_str);

    ImGui::Separator();

    ImGui::Text("Curr:   Render /    Sound");
    ImGui::Text("freq: %s / %s", render_freq_hist_str, sound_freq_hist_str);
    ImGui::Text("  ms: %s / %s", render_ms_hist_str, sound_ms_hist_str);
#endif

    ImGui::Separator();

    if (ImGui::ButtonEnterKeyPressed("Reset Render Settings"))
        render_set_scale(1.0);

    data->imgui_focus |= ImGui::IsWindowFocused();
    ImGui::End();
}

bool graphics_render_settings_dispose(class_data* data) {
    return true;
}
