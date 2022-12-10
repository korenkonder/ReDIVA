/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "fog.hpp"
#include "../../imgui_helper.hpp"

struct data_view_fog {
    render_context* rctx;
    int32_t group;

    data_view_fog();
    ~data_view_fog();
};

extern int32_t width;
extern int32_t height;

static const char* graphics_fog_window_title = "Fog##Graphics";

bool graphics_fog_init(class_data* data, render_context* rctx) {
    graphics_fog_dispose(data);
    data_view_fog* fog = new data_view_fog;
    if (fog)
        fog->rctx = rctx;
    data->data = fog;
    return true;
}

void graphics_fog_imgui(class_data* data) {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    float_t w = min_def((float_t)width, 300.0f);
    float_t h = min_def((float_t)height, 344.0f);

    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Appearing);

    data->imgui_focus = false;
    bool open = data->flags & CLASS_HIDDEN ? false : true;
    bool collapsed = !ImGui::Begin(graphics_fog_window_title, &open, 0);
    if (!open) {
        enum_or(data->flags, CLASS_HIDE);
        ImGui::End();
        return;
    }
    else if (collapsed) {
        ImGui::End();
        return;
    }

    data_view_fog* fog = (data_view_fog*)data->data;
    if (!fog) {
        ImGui::End();
        return;
    }

    render_context* rctx = fog->rctx;
    ::fog* fog_data = &rctx->fog[fog->group];

    if (ImGui::ButtonEnterKeyPressed("Reset"))
        light_param_data_storage_data_set_stage(
            light_param_data_storage_data_get_stage_index(), LIGHT_PARAM_DATA_STORAGE_FOG);

    const char* fog_group_labels[] = {
        "0: DEPTH",
        "1: HEIGHT",
        "2: BUMP",
    };

    ImGui::TextCentered("GROUP");
    ImGui::GetContentRegionAvailSetNextItemWidth();
    ImGui::ComboBox("##Group", fog_group_labels, 3,
        &fog->group, 0, false, &data->imgui_focus);

    const char* fog_type_labels[] = {
        "NONE",
        "LINEAR",
        "EXP",
        "EXP2",
    };

    ImGui::TextCentered("TYPE");

    fog_type type = fog_data->get_type();
    ImGui::GetContentRegionAvailSetNextItemWidth();
    ImGui::ComboBox("##Type", fog_type_labels, FOG_EXP2,
        (int32_t*)&type, 0, true, &data->imgui_focus);
    fog_data->set_type(type);

    ImGui::TextCentered("DENSITY");

    float_t density = fog_data->get_density();
    ImGui::GetContentRegionAvailSetNextItemWidth();
    ImGui::SliderFloatButton("##Density", &density, 0.01f, 0.0f, 1.0f, 0.1f, "%.3f", 0);
    fog_data->set_density(density);

    ImGui::TextCentered("LINEAR");

    ImGui::PushID("Linear");
    float_t start = fog_data->get_start();
    ImGui::ColumnSliderFloatButton("START", &start, 0.1f, -100.0f, 1000.0f, 1.0f, "%.2f", 0);
    fog_data->set_start(start);

    float_t end = fog_data->get_end();
    ImGui::ColumnSliderFloatButton("END", &end, 0.1f, -100.0f, 1000.0f, 1.0f, "%.2f", 0);
    fog_data->set_end(end);
    ImGui::PopID();

    ImGui::TextCentered("COLOR");

    ImGui::PushID("Color");
    vec4 color;
    fog_data->get_color(color);
    ImGui::GetContentRegionAvailSetNextItemWidth();
    ImGui::ColumnSliderFloatButton("R", &color.x, 0.01f, 0.0f, 8.0f, 0.1f, "%.2f", 0);
    ImGui::ColumnSliderFloatButton("G", &color.y, 0.01f, 0.0f, 8.0f, 0.1f, "%.2f", 0);
    ImGui::ColumnSliderFloatButton("B", &color.z, 0.01f, 0.0f, 8.0f, 0.1f, "%.2f", 0);
    fog_data->set_color(color);
    ImGui::PopID();

    ImGui::GetContentRegionAvailSetNextItemWidth();

    data->imgui_focus |= ImGui::IsWindowFocused();
    ImGui::End();
}

bool graphics_fog_dispose(class_data* data) {
    data_view_fog* fog = (data_view_fog*)data->data;
    if (fog)
        delete fog;
    data->data = 0;
    return true;
}

data_view_fog::data_view_fog() : rctx(), group() {

}

data_view_fog::~data_view_fog() {

}
