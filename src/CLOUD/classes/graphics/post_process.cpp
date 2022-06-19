/*
/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "post_process.hpp"
#include "../../../CRE/stage.hpp"
#include "../imgui_helper.hpp"

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

    float_t w = min((float_t)width, 400.0f);
    float_t h = min((float_t)height, 790.0f);

    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Appearing);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    data->imgui_focus = false;
    bool open = data->flags & CLASS_HIDDEN ? false : true;
    bool collapsed = !ImGui::Begin(graphics_post_process_window_title, &open, window_flags);
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
    post_process* pp = &rctx->post_process;

    post_process_blur* blur = pp->blur;
    post_process_dof* dof = pp->dof;
    post_process_tone_map* tone_map = pp->tone_map;

    static const char* mag_filter_labels[] = {
        "nearest",
        "bilinear",
        "sharpen(5tap)",
        "sharpen(4tap)",
        "cone(4tap)",
        "cone(2tap)",
    };

    ImGui::ColumnComboBox("MAG Filter", mag_filter_labels, POST_PROCESS_MAG_FILTER_MAX,
        (int32_t*)&pp->mag_filter, 0, false, &data->imgui_focus);

    ImGui::CheckboxEnterKeyPressed("Morphological AA", &pp->mlaa);

    ImGui::SetColumnSpace(1.0f / 4.0f);
    if (ImGui::TreeNode("Tone Trans")) {
        vec3 tone_trans_start = tone_map->get_tone_trans_start();
        ImGui::ColumnSliderVec3("Start", &tone_trans_start, 0.01f, 0.0f, 1.0f, "%.2f", 0, true);
        tone_map->set_tone_trans_start(tone_trans_start);

        vec3 tone_trans_end = tone_map->get_tone_trans_end();
        ImGui::ColumnSliderVec3("End", &tone_trans_end, 0.01f, 0.0f, 1.0f, "%.2f", 0, true);
        tone_map->set_tone_trans_end(tone_trans_end);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Scene Fade")) {
        float_t scene_fade_alpha = tone_map->get_scene_fade_alpha();
        ImGui::ColumnSliderFloat("Alpha", &scene_fade_alpha, 0.01f, 0.0f, 1.0f, "%.2f", 0, true);
        tone_map->set_scene_fade_alpha(scene_fade_alpha);

        vec3 scene_fade_color = tone_map->get_scene_fade_color();
        ImGui::ColumnSliderVec3("Color", &scene_fade_color, 0.01f, 0.0f, 1.0f, "%.2f", 0, true);
        tone_map->set_scene_fade_color(scene_fade_color);

        const char* scene_fade_blend_func_labels[] = {
            "0: OVER",
            "1: MULTI",
            "2: PLUS",
        };

        int32_t scene_fade_blend_func = tone_map->get_scene_fade_blend_func();
        ImGui::ColumnComboBox("Blend Func", scene_fade_blend_func_labels, 3,
            &scene_fade_blend_func, 0, false, &data->imgui_focus);
        tone_map->set_scene_fade_blend_func(scene_fade_blend_func);
        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Glow Param", ImGuiTreeNodeFlags_DefaultOpen)) {
        const char* tone_map_method_labels[] = {
            "YCC EXPONENT",
            "RGB LINEAR",
            "RGB LINEAR2",
        };

        int32_t _tone_map_method = tone_map->get_tone_map_method();
        ImGui::ColumnComboBox("Tone Map", tone_map_method_labels, 3,
            &_tone_map_method, 0, false, &data->imgui_focus);
        tone_map->set_tone_map_method((tone_map_method)_tone_map_method);

        float_t exposure = tone_map->get_exposure();
        ImGui::ColumnSliderFloat("Exposure", &exposure, 0.02f, 0.0f, 4.0f, "%.2f", 0, true);
        exposure = roundf(exposure / 0.02f) * 0.02f;
        tone_map->set_exposure(exposure);

        bool auto_exposure = tone_map->get_auto_exposure();
        ImGui::CheckboxEnterKeyPressed("Auto Exposure", &auto_exposure);
        tone_map->set_auto_exposure(auto_exposure);

        float_t gamma = tone_map->get_gamma();
        ImGui::ColumnSliderFloat("Gamma", &gamma, 0.01f, 0.2f, 2.2f, "%.2f", 0, true);
        tone_map->set_gamma(gamma);

        int32_t saturate1 = tone_map->get_saturate_power();
        ImGui::ColumnSliderInt("Saturate 1", &saturate1, 1, 6, "%d", 0, true);
        tone_map->set_saturate_power(saturate1);

        float_t saturate2 = tone_map->get_saturate_coeff();
        ImGui::ColumnSliderFloat("Saturate 2", &saturate2, 0.01f, 0.0f, 1.0f, "%.2f", 0, true);
        tone_map->set_saturate_coeff(saturate2);
        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Glare", ImGuiTreeNodeFlags_DefaultOpen)) {
        vec3 radius = blur->get_radius();
        ImGui::ColumnSliderFloat("Radius R", &radius.x, 0.1f, 1.0f, 3.0f, "%.2f", 0, true);
        ImGui::ColumnSliderFloat("Radius G", &radius.y, 0.1f, 1.0f, 3.0f, "%.2f", 0, true);
        ImGui::ColumnSliderFloat("Radius B", &radius.z, 0.1f, 1.0f, 3.0f, "%.2f", 0, true);
        blur->set_radius(radius);

        vec3 intensity = blur->get_intensity();
        ImGui::ColumnSliderFloat("Inten  R", &intensity.x, 0.05f, 0.0f, 2.0f, "%.2f", 0, true);
        ImGui::ColumnSliderFloat("Inten  G", &intensity.y, 0.05f, 0.0f, 2.0f, "%.2f", 0, true);
        ImGui::ColumnSliderFloat("Inten  B", &intensity.z, 0.05f, 0.0f, 2.0f, "%.2f", 0, true);
        blur->set_intensity(intensity);
        ImGui::TreePop();
    }
    ImGui::SetDefaultColumnSpace();

    if (ImGui::TreeNodeEx("DOF", ImGuiTreeNodeFlags_DefaultOpen)) {
        bool use_ui_params = dof->data.debug.flags & DOF_DEBUG_USE_UI_PARAMS;
        bool phys = use_ui_params && dof->data.debug.flags & DOF_DEBUG_ENABLE_PHYS_DOF;
        bool f2 = use_ui_params && ~dof->data.debug.flags & DOF_DEBUG_ENABLE_PHYS_DOF;

        float_t alpha = style.Alpha * ImGui::AlphaDisabledScale;

        ImGui::CheckboxFlagsEnterKeyPressed("use UI params",
            (uint32_t*)&dof->data.debug.flags, DOF_DEBUG_USE_UI_PARAMS);

        if (!use_ui_params) {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
        }
        ImGui::CheckboxFlagsEnterKeyPressed("enable DOF",
            (uint32_t*)&dof->data.debug.flags, DOF_DEBUG_ENABLE_DOF);
        ImGui::CheckboxFlagsEnterKeyPressed("enable physical DOF",
            (uint32_t*)&dof->data.debug.flags, DOF_DEBUG_ENABLE_PHYS_DOF);
        if (!use_ui_params) {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
        }

        if (!phys) {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
        }
        ImGui::CheckboxFlagsEnterKeyPressed("auto focus",
            (uint32_t*)&dof->data.debug.flags, DOF_DEBUG_AUTO_FOCUS);
        if (!phys) {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
        }

        ImGui::SetColumnSpace(16.0f / 31.0f);
        if (!phys) {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
        }
        ImGui::ColumnSliderFloat("distance to focus[m]",
            &dof->data.debug.focus, 0.01f, 0.01f, 30.0f, "%.2f", 0, true);

        float_t focal_length = dof->data.debug.focal_length * 1000.0f;
        ImGui::ColumnSliderFloat("focal length[mm]",
            &focal_length, 0.01f, 0.1f, 100.0f, "%.2f", 0, true);
        dof->data.debug.focal_length = focal_length / 1000.0f;

        ImGui::ColumnSliderFloat("F-Number",
            &dof->data.debug.f_number, 0.01f, 0.1f, 40.0f, "%.2f", 0, true);
        if (!phys) {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
        }

        if (!f2) {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
        }
        ImGui::ColumnSliderFloat("f2 distance to focus[m]",
            &dof->data.debug.f2.focus, 0.01f, 0.01f, 30.0f, "%.2f", 0, true);
        ImGui::ColumnSliderFloat("f2 focus range[m]",
            &dof->data.debug.f2.focus_range, 0.01f, 0.0f, 10.0f, "%.2f", 0, true);
        ImGui::ColumnSliderFloat("f2 fuzzing range[m]",
            &dof->data.debug.f2.fuzzing_range, 0.01f, 0.0f, 10.0f, "%.2f", 0, true);
        ImGui::ColumnSliderFloat("f2 ratio",
            &dof->data.debug.f2.ratio, 0.01f, 0.0f, 1.0f, "%.2f", 0, true);
        if (!f2) {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
        }
        ImGui::SetDefaultColumnSpace();
        ImGui::TreePop();
    }

    if (ImGui::ButtonEnterKeyPressed("Reset Post Process", { 0, 0 })) {
        stage* stg = task_stage_get_current_stage();
        light_param_storage_data_set_stage(stg->index);
    }

    data->imgui_focus |= ImGui::IsWindowFocused();
    ImGui::End();
}

bool graphics_post_process_dispose(class_data* data) {
    data->data = 0;
    return true;
}
