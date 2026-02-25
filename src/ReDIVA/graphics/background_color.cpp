/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "background_color.hpp"
#include "../../CRE/gl.hpp"
#include "../../CRE/clear_color.hpp"
#include "../imgui_helper.hpp"
#include "../task_window.hpp"

class GraphicsBackColor : public app::TaskWindow {
public:
    bool exit;

    GraphicsBackColor();
    virtual ~GraphicsBackColor() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void window() override;
};

GraphicsBackColor graphics_back_color;

void graphics_background_color_init() {
    app::TaskWork::add_task(&graphics_back_color, "GRAPHICS_BACK_COLOR");
}

GraphicsBackColor::GraphicsBackColor() : exit() {

}

GraphicsBackColor::~GraphicsBackColor() {

}

bool GraphicsBackColor::init() {
    exit = false;
    return true;
}

bool GraphicsBackColor::ctrl() {
    return exit;
}

bool GraphicsBackColor::dest() {
    return true;
}

void GraphicsBackColor::window() {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    extern int32_t width;
    extern int32_t height;

    float_t w = min_def((float_t)width, 360.0f);
    float_t h = min_def((float_t)height, 100.0f);

    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    focus = false;
    bool open = true;
    if (!ImGui::Begin("Background Color##Graphics", &open, 0)) {
        ImGui::End();
        return;
    }
    else if (!open) {
        exit = true;
        ImGui::End();
        return;
    }

    ImGuiColorEditFlags color_edit_flags = 0;
    color_edit_flags |= ImGuiColorEditFlags_NoLabel;
    color_edit_flags |= ImGuiColorEditFlags_NoSidePreview;
    color_edit_flags |= ImGuiColorEditFlags_NoDragDrop;

    ImGui::GetContentRegionAvailSetNextItemWidth();
    vec4 _clear_color = clear_color;
    if (ImGui::ColorEdit4("##Background Color", (float_t*)&_clear_color, color_edit_flags))
        clear_color = _clear_color;
    ImGui::Checkbox("Set Clear Color", &set_clear_color);

    if (ImGui::ButtonEnterKeyPressed("Reset Color")) {
        clear_color = 0xFF606060;
        set_clear_color = true;
    }

    focus |= ImGui::IsWindowFocused();
    ImGui::End();
}
