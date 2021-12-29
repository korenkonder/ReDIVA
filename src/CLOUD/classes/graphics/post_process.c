/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "post_process.h"
#include "../../../CRE/stage.h"
#include "../imgui_helper.h"

extern int32_t width;
extern int32_t height;
extern bool input_locked;

const char* graphics_post_process_window_title = "Post Process##Graphics";

bool graphics_post_process_init(class_data* data, render_context* rctx) {
    graphics_post_process_dispose(data);
    data->data = rctx;
    return true;
}

void graphics_post_process_imgui(class_data* data) {
    ImGuiIO* io = igGetIO();
    ImGuiStyle* style = igGetStyle();
    ImFont* font = igGetFont();

    float_t w = min((float_t)width, 400.0f);
    float_t h = min((float_t)height, 790.0f);

    igSetNextWindowPos(ImVec2_Empty, ImGuiCond_Appearing, ImVec2_Empty);
    igSetNextWindowSize((ImVec2) { w, h }, ImGuiCond_Appearing);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    data->imgui_focus = false;
    bool open = data->flags & CLASS_HIDDEN ? false : true;
    bool collapsed = !igBegin(graphics_post_process_window_title, &open, window_flags);
    if (!open) {
        data->flags |= CLASS_HIDE;
        goto End;
    }
    else if (collapsed)
        goto End;

    render_context* rctx = data->data;
    post_process_struct* pp = &rctx->post_process;

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

    imguiColumnComboBox("MAG Filter", mag_filter_labels, POST_PROCESS_MAG_FILTER_MAX,
        (int32_t*)&pp->mag_filter, 0, false, &data->imgui_focus);

    imguiCheckbox("Morphological AA", &pp->mlaa);

    imguiSetColumnSpace(1.0f / 4.0f);
    if (igTreeNode_Str("Tone Trans")) {
        vec3 tone_trans_start;
        post_process_tone_map_get_tone_trans_start(tone_map, &tone_trans_start);
        imguiColumnSliderVec3("Start", &tone_trans_start, 0.01f, 0.0f, 1.0f, "%.2f", 0, true);
        post_process_tone_map_set_tone_trans_start(tone_map, &tone_trans_start);

        vec3 tone_trans_end;
        post_process_tone_map_get_tone_trans_end(tone_map, &tone_trans_end);
        imguiColumnSliderVec3("End", &tone_trans_end, 0.01f, 0.0f, 1.0f, "%.2f", 0, true);
        post_process_tone_map_set_tone_trans_end(tone_map, &tone_trans_end);
        igTreePop();
    }

    if (igTreeNode_Str("Scene Fade")) {
        float_t scene_fade_alpha = post_process_tone_map_get_scene_fade_alpha(tone_map);
        imguiColumnSliderFloat("Alpha", &scene_fade_alpha, 0.01f, 0.0f, 1.0f, "%.2f", 0, true);
        post_process_tone_map_set_scene_fade_alpha(tone_map, scene_fade_alpha);

        vec3 scene_fade_color;
        post_process_tone_map_get_scene_fade_color(tone_map, &scene_fade_color);
        imguiColumnSliderVec3("Color", &scene_fade_color, 0.01f, 0.0f, 1.0f, "%.2f", 0, true);
        post_process_tone_map_set_scene_fade_color(tone_map, &scene_fade_color);

        const char* scene_fade_blend_func_labels[] = {
            "0: OVER",
            "1: MULTI",
            "2: PLUS",
        };

        int32_t scene_fade_blend_func = post_process_tone_map_get_scene_fade_blend_func(tone_map);
        imguiColumnComboBox("Blend Func", scene_fade_blend_func_labels, 3,
            &scene_fade_blend_func, 0, false, &data->imgui_focus);
        post_process_tone_map_set_scene_fade_blend_func(tone_map, scene_fade_blend_func);
        igTreePop();
    }

    if (igTreeNodeEx_Str("Glow Param", ImGuiTreeNodeFlags_DefaultOpen)) {
        const char* tone_map_method_labels[] = {
            "YCC EXPONENT",
            "RGB LINEAR",
            "RGB LINEAR2",
        };

        int32_t tone_map_method = post_process_tone_map_get_tone_map_method(tone_map);
        imguiColumnComboBox("Tone Map", tone_map_method_labels, 3,
            &tone_map_method, 0, false, &data->imgui_focus);
        post_process_tone_map_set_tone_map_method(tone_map, tone_map_method);

        float_t exposure = post_process_tone_map_get_exposure(tone_map);
        imguiColumnSliderFloat("Exposure", &exposure, 0.02f, 0.0f, 4.0f, "%.2f", 0, true);
        exposure = roundf(exposure / 0.02f) * 0.02f;
        post_process_tone_map_set_exposure(tone_map, exposure);

        bool auto_exposure = post_process_tone_map_get_auto_exposure(tone_map);
        imguiCheckbox("Auto Exposure", &auto_exposure);
        post_process_tone_map_set_auto_exposure(tone_map, auto_exposure);

        float_t gamma = post_process_tone_map_get_gamma(tone_map);
        imguiColumnSliderFloat("Gamma", &gamma, 0.01f, 0.2f, 2.2f, "%.2f", 0, true);
        post_process_tone_map_set_gamma(tone_map, gamma);

        int32_t saturate1 = post_process_tone_map_get_saturate_power(tone_map);
        imguiColumnSliderInt("Saturate 1", &saturate1, 1, 6, "%d", 0, true);
        post_process_tone_map_set_saturate_power(tone_map, saturate1);

        float_t saturate2 = post_process_tone_map_get_saturate_coeff(tone_map);
        imguiColumnSliderFloat("Saturate 2", &saturate2, 0.01f, 0.0f, 1.0f, "%.2f", 0, true);
        post_process_tone_map_set_saturate_coeff(tone_map, saturate2);
        igTreePop();
    }

    if (igTreeNodeEx_Str("Glare", ImGuiTreeNodeFlags_DefaultOpen)) {
        vec3 radius;
        post_process_blur_get_radius(blur, &radius);
        imguiColumnSliderFloat("Radius R", &radius.x, 0.1f, 1.0f, 3.0f, "%.2f", 0, true);
        imguiColumnSliderFloat("Radius G", &radius.y, 0.1f, 1.0f, 3.0f, "%.2f", 0, true);
        imguiColumnSliderFloat("Radius B", &radius.z, 0.1f, 1.0f, 3.0f, "%.2f", 0, true);
        post_process_blur_set_radius(blur, &radius);

        vec3 intensity;
        post_process_blur_get_intensity(blur, &intensity);
        imguiColumnSliderFloat("Inten  R", &intensity.x, 0.05f, 0.0f, 2.0f, "%.2f", 0, true);
        imguiColumnSliderFloat("Inten  G", &intensity.y, 0.05f, 0.0f, 2.0f, "%.2f", 0, true);
        imguiColumnSliderFloat("Inten  B", &intensity.z, 0.05f, 0.0f, 2.0f, "%.2f", 0, true);
        post_process_blur_set_intensity(blur, &intensity);
        igTreePop();
    }
    imguiSetDefaultColumnSpace();

    if (igTreeNodeEx_Str("DOF", ImGuiTreeNodeFlags_DefaultOpen)) {
        bool use_ui_params = dof->data.debug.flags & DOF_DEBUG_USE_UI_PARAMS;
        bool phys = use_ui_params && dof->data.debug.flags & DOF_DEBUG_ENABLE_PHYS_DOF;
        bool f2 = use_ui_params && ~dof->data.debug.flags & DOF_DEBUG_ENABLE_PHYS_DOF;

        float_t alpha = style->Alpha * imgui_alpha_disabled_scale;

        imguiCheckboxFlags_UintPtr("use UI params",
            (uint32_t*)&dof->data.debug.flags, DOF_DEBUG_USE_UI_PARAMS);

        if (!use_ui_params) {
            igPushItemFlag(ImGuiItemFlags_Disabled, true);
            igPushStyleVar_Float(ImGuiStyleVar_Alpha, alpha);
        }
        imguiCheckboxFlags_UintPtr("enable DOF",
            (uint32_t*)&dof->data.debug.flags, DOF_DEBUG_ENABLE_DOF);
        imguiCheckboxFlags_UintPtr("enable physical DOF",
            (uint32_t*)&dof->data.debug.flags, DOF_DEBUG_ENABLE_PHYS_DOF);
        if (!use_ui_params) {
            igPopItemFlag();
            igPopStyleVar(1);
        }

        if (!phys) {
            igPushItemFlag(ImGuiItemFlags_Disabled, true);
            igPushStyleVar_Float(ImGuiStyleVar_Alpha, alpha);
        }
        imguiCheckboxFlags_UintPtr("auto focus",
            (uint32_t*)&dof->data.debug.flags, DOF_DEBUG_AUTO_FOCUS);
        if (!phys) {
            igPopItemFlag();
            igPopStyleVar(1);
        }

        imguiSetColumnSpace(16.0f / 31.0f);
        if (!phys) {
            igPushItemFlag(ImGuiItemFlags_Disabled, true);
            igPushStyleVar_Float(ImGuiStyleVar_Alpha, alpha);
        }
        imguiColumnSliderFloat("distance to focus[m]",
            &dof->data.debug.distance_to_focus, 0.01f, 0.01f, 30.0f, "%.2f", 0, true);
        imguiColumnSliderFloat("focal length[mm]",
            &dof->data.debug.focal_length, 0.01f, 0.1f, 100.0f, "%.2f", 0, true);
        imguiColumnSliderFloat("F-Number",
            &dof->data.debug.f_number, 0.01f, 0.1f, 40.0f, "%.2f", 0, true);
        if (!phys) {
            igPopItemFlag();
            igPopStyleVar(1);
        }

        if (!f2) {
            igPushItemFlag(ImGuiItemFlags_Disabled, true);
            igPushStyleVar_Float(ImGuiStyleVar_Alpha, alpha);
        }
        imguiColumnSliderFloat("f2 distance to focus[m]",
            &dof->data.debug.f2.distance_to_focus, 0.01f, 0.01f, 30.0f, "%.2f", 0, true);
        imguiColumnSliderFloat("f2 focus range[m]",
            &dof->data.debug.f2.focus_range, 0.01f, 0.0f, 10.0f, "%.2f", 0, true);
        imguiColumnSliderFloat("f2 fuzzing range[m]",
            &dof->data.debug.f2.fuzzing_range, 0.01f, 0.0f, 10.0f, "%.2f", 0, true);
        imguiColumnSliderFloat("f2 ratio",
            &dof->data.debug.f2.ratio, 0.01f, 0.0f, 1.0f, "%.2f", 0, true);
        if (!f2) {
            igPopItemFlag();
            igPopStyleVar(1);
        }
        imguiSetDefaultColumnSpace();
        igTreePop();
    }

    if (imguiButton("Reset Post Process", ImVec2_Empty) && rctx->stage) {
        stage* stage = rctx->stage;
        light_param_data* light_param = light_param_storage_get_light_param_data(stage->light_param_name);
        post_process_reset(pp);
        if (light_param->glow.ready)
            render_context_light_param_data_glow_set(rctx, &light_param->glow);
    }

    data->imgui_focus |= igIsWindowFocused(0);

End:
    igEnd();
}

void graphics_post_process_input(class_data* data) {
    input_locked |= data->imgui_focus;
}

void graphics_post_process_render(class_data* data) {

}

bool graphics_post_process_dispose(class_data* data) {
    data->data = 0;
    return true;
}
