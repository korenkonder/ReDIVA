/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "post_process.hpp"
#include "../../../CRE/light_param.hpp"
#include "../../../CRE/render_context.hpp"
#include "../../../CRE/stage.hpp"
#include "../../imgui_helper.hpp"

extern int32_t width;
extern int32_t height;

const char* graphics_post_process_window_title = "Post Process##Graphics";

bool graphics_post_process_init(class_data* data, render_context* rctx) {
    graphics_post_process_dispose(data);
    data->data = rctx;
    return true;
}

void graphics_post_process_imgui(class_data* data) {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    float_t w = min_def((float_t)width, 400.0f);
    float_t h = min_def((float_t)height, 790.0f);

    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Appearing);

    data->imgui_focus = false;
    bool open = data->flags & CLASS_HIDDEN ? false : true;
    bool collapsed = !ImGui::Begin(graphics_post_process_window_title, &open, 0);
    if (!open) {
        enum_or(data->flags, CLASS_HIDE);
        ImGui::End();
        return;
    }
    else if (collapsed) {
        ImGui::End();
        return;
    }

    render_context* rctx = (render_context*)data->data;
    if (!rctx) {
        ImGui::End();
        return;
    }

    rndr::Render* rend = &rctx->render;

    renderer::DOF3* dof = rend->dof;

    static const char* mag_filter_labels[] = {
        "nearest",
        "bilinear",
        "sharpen(5tap)",
        "sharpen(4tap)",
        "cone(4tap)",
        "cone(2tap)",
    };

    ImGui::ColumnComboBox("MAG Filter", mag_filter_labels, rndr::Render::MAG_FILTER_MAX,
        (int32_t*)&rend->mag_filter, 0, false, &data->imgui_focus);

    bool taa = !!rend->taa;
    if (ImGui::CheckboxEnterKeyPressed("Temporal AA", &taa))
        rend->taa = taa ? 1 : 0;
    
    bool mlaa = !!rend->mlaa;
    if (ImGui::CheckboxEnterKeyPressed("Morphological AA", &mlaa))
        rend->mlaa = mlaa ? 1 : 0;

    ImGui::SetColumnSpace(1.0f / 4.0f);
    if (ImGui::TreeNode("Tone Trans")) {
        vec3 tone_trans_start;
        vec3 tone_trans_end;
        bool tone_trans_change = false;
        rend->get_tone_trans(tone_trans_start, tone_trans_end);
        tone_trans_change |= ImGui::ColumnSliderVec3Button("Start",
            &tone_trans_start, 0.01f, 0.0f, 1.0f, 0.1f, "%.2f", 0);
        tone_trans_change |= ImGui::ColumnSliderVec3Button("End",
            &tone_trans_end, 0.01f, 0.0f, 1.0f, 0.1f, "%.2f", 0);
        if (tone_trans_change)
            rend->set_tone_trans(tone_trans_start, tone_trans_end, 0);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Scene Fade")) {
        float_t scene_fade_alpha = rend->get_scene_fade_alpha();
        if (ImGui::ColumnSliderFloatButton("Alpha",
            &scene_fade_alpha, 0.01f, 0.0f, 1.0f, 0.1f, "%.2f", 0))
            rend->set_scene_fade_alpha(scene_fade_alpha, 0);

        vec3 scene_fade_color = rend->get_scene_fade_color();
        if (ImGui::ColumnSliderVec3Button("Color",
            &scene_fade_color, 0.01f, 0.0f, 1.0f, 0.1f, "%.2f", 0))
            rend->set_scene_fade_color(scene_fade_color, 0);

        const char* scene_fade_blend_func_labels[] = {
            "0: OVER",
            "1: MULTI",
            "2: PLUS",
        };

        int32_t scene_fade_blend_func = rend->get_scene_fade_blend_func();
        if (ImGui::ColumnComboBox("Blend Func", scene_fade_blend_func_labels, 3,
            &scene_fade_blend_func, 0, false, &data->imgui_focus))
            rend->set_scene_fade_blend_func(scene_fade_blend_func, 0);
        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Glow Param", ImGuiTreeNodeFlags_DefaultOpen)) {
        const char* tone_map_method_labels[] = {
            "YCC EXPONENT",
            "RGB LINEAR",
            "RGB LINEAR2",
        };

        tone_map_method tone_map_method = rend->get_tone_map();
        if (ImGui::ColumnComboBox("Tone Map", tone_map_method_labels, TONE_MAP_MAX,
            (int32_t*)&tone_map_method, 0, false, &data->imgui_focus))
            rend->set_tone_map(tone_map_method);

        float_t exposure = rend->get_exposure();
        if (ImGui::ColumnSliderFloatButton("Exposure",
            &exposure, 0.02f, 0.0f, 4.0f, 0.1f, "%.2f", 0)) {
            exposure = prj::roundf(exposure / 0.02f) * 0.02f;
            rend->set_exposure(exposure);
        }

        bool auto_exposure = rend->get_auto_exposure();
        if (ImGui::CheckboxEnterKeyPressed("Auto Exposure", &auto_exposure))
            rend->set_auto_exposure(auto_exposure);

        float_t gamma = rend->get_gamma();
        if (ImGui::ColumnSliderFloatButton("Gamma", &gamma, 0.01f, 0.2f, 2.2f, 0.1f, "%.2f", 0))
            rend->set_gamma(gamma);

        int32_t saturate1 = rend->get_saturate_power();
        if (ImGui::ColumnSliderIntButton("Saturate1", &saturate1, 1, 6, "%d", 0))
            rend->set_saturate_power(saturate1);

        float_t saturate2 = rend->get_saturate_coeff();
        if (ImGui::ColumnSliderFloatButton("Saturate2", &saturate2, 0.01f, 0.0f, 1.0f, 0.1f, "%.2f", 0))
            rend->set_saturate_coeff(saturate2, 0, false);
        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Glare", ImGuiTreeNodeFlags_DefaultOpen)) {
        vec3 radius = rend->get_radius();
        bool radius_change = false;
        radius_change |= ImGui::ColumnSliderFloatButton("Radius R",
            &radius.x, 0.1f, 1.0f, 3.0f, 0.1f, "%.2f", 0);
        radius_change |= ImGui::ColumnSliderFloatButton("Radius G",
            &radius.y, 0.1f, 1.0f, 3.0f, 0.1f, "%.2f", 0);
        radius_change |= ImGui::ColumnSliderFloatButton("Radius B",
            &radius.z, 0.1f, 1.0f, 3.0f, 0.1f, "%.2f", 0);
        if (radius_change)
            rend->set_radius(radius);

        vec3 intensity = rend->get_intensity();
        bool intensity_change = false;
        intensity_change |= ImGui::ColumnSliderFloatButton("Inten  R",
            &intensity.x, 0.05f, 0.0f, 2.0f, 0.1f, "%.2f", 0);
        intensity_change |= ImGui::ColumnSliderFloatButton("Inten  G",
            &intensity.y, 0.05f, 0.0f, 2.0f, 0.1f, "%.2f", 0);
        intensity_change |= ImGui::ColumnSliderFloatButton("Inten  B",
            &intensity.z, 0.05f, 0.0f, 2.0f, 0.1f, "%.2f", 0);
        if (intensity_change)
            rend->set_intensity(intensity);
        ImGui::TreePop();
    }
    ImGui::SetDefaultColumnSpace();

    if (ImGui::TreeNodeEx("DOF", ImGuiTreeNodeFlags_DefaultOpen)) {
        dof_debug dof_debug_data;
        dof_debug_get(&dof_debug_data);
        bool use_ui_params = dof_debug_data.flags & DOF_DEBUG_USE_UI_PARAMS;
        bool phys = use_ui_params && dof_debug_data.flags & DOF_DEBUG_ENABLE_PHYS_DOF;
        bool f2 = use_ui_params && ~dof_debug_data.flags & DOF_DEBUG_ENABLE_PHYS_DOF;

        float_t alpha = style.Alpha * ImGui::AlphaDisabledScale;

        ImGui::CheckboxFlagsEnterKeyPressed("use UI params",
            (uint32_t*)&dof_debug_data.flags, DOF_DEBUG_USE_UI_PARAMS);

        if (!use_ui_params) {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
        }
        ImGui::CheckboxFlagsEnterKeyPressed("enable DOF",
            (uint32_t*)&dof_debug_data.flags, DOF_DEBUG_ENABLE_DOF);
        ImGui::CheckboxFlagsEnterKeyPressed("enable physical DOF",
            (uint32_t*)&dof_debug_data.flags, DOF_DEBUG_ENABLE_PHYS_DOF);
        if (!use_ui_params) {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
        }

        if (!phys) {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
        }
        ImGui::CheckboxFlagsEnterKeyPressed("auto focus",
            (uint32_t*)&dof_debug_data.flags, DOF_DEBUG_AUTO_FOCUS);
        if (!phys) {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
        }

        ImGui::SetColumnSpace(16.0f / 31.0f);
        if (!phys) {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
        }
        ImGui::ColumnSliderFloatButton("distance to focus[m]",
            &dof_debug_data.focus, 0.01f, 0.01f, 30.0f, 1.0f, "%.2f", 0);

        float_t focal_length = dof_debug_data.focal_length * 1000.0f;
        ImGui::ColumnSliderFloatButton("focal length[mm]",
            &focal_length, 0.01f, 0.1f, 100.0f, 1.0f, "%.2f", 0);
        dof_debug_data.focal_length = focal_length / 1000.0f;

        ImGui::ColumnSliderFloatButton("F-Number",
            &dof_debug_data.f_number, 0.01f, 0.1f, 40.0f, 1.0f, "%.2f", 0);
        if (!phys) {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
        }

        if (!f2) {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
        }
        ImGui::ColumnSliderFloatButton("f2 distance to focus[m]",
            &dof_debug_data.f2.focus, 0.01f, 0.01f, 30.0f, 1.0f, "%.2f", 0);
        ImGui::ColumnSliderFloatButton("f2 focus range[m]",
            &dof_debug_data.f2.focus_range, 0.01f, 0.0f, 10.0f, 1.0f, "%.2f", 0);
        ImGui::ColumnSliderFloatButton("f2 fuzzing range[m]",
            &dof_debug_data.f2.fuzzing_range, 0.01f, 0.0f, 10.0f, 1.0f, "%.2f", 0);
        ImGui::ColumnSliderFloatButton("f2 ratio",
            &dof_debug_data.f2.ratio, 0.01f, 0.0f, 1.0f, 1.0f, "%.2f", 0);
        if (!f2) {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
        }
        dof_debug_set(&dof_debug_data);
        ImGui::SetDefaultColumnSpace();
        ImGui::TreePop();
    }

    if (ImGui::ButtonEnterKeyPressed("Reset Post Process", { 0, 0 })) {
        stage* stg = task_stage_get_current_stage();
        if (stg)
            light_param_data_storage_data_set_stage(stg->index);
        dof_debug_set();
        extern render_context* rctx_ptr;
        rctx_ptr->render.set_dof_enable(false);
    }

    data->imgui_focus |= ImGui::IsWindowFocused();
    ImGui::End();
}

bool graphics_post_process_dispose(class_data* data) {
    data->data = 0;
    return true;
}
