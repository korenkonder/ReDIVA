/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "post_process.h"
#include "../../../CRE/post_process.h"
#include "../../../CRE/dof.h"
#include "../imgui_helper.h"

typedef struct post_process_struct {
    bool dispose;
    bool disposed;
    bool imgui_focus;
} post_process_struct;

extern int32_t width;
extern int32_t height;
extern bool input_locked;

const char* post_process_window_title = "Post Process";

extern dof_struct* dof;
extern radius* rad;
extern intensity* inten;
extern tone_map* tm;

bool post_process_enabled = false;
static post_process_struct post_process;

extern void post_process_dispose() {
    post_process_enabled = false;
    post_process.imgui_focus = false;
    post_process.dispose = false;
    post_process.disposed = true;
}

extern void post_process_init() {
    if (post_process_enabled || post_process.dispose)
        post_process_dispose();

    post_process_enabled = true;
}

extern void post_process_imgui() {
    if (!post_process_enabled) {
        if (!post_process.disposed)
            post_process.dispose = true;
        return;
    }
    else if (post_process.disposed)
        post_process_enabled = true;

    ImGuiIO* io = igGetIO();
    ImGuiStyle* style = igGetStyle();
    ImFont* font = igGetFont();

    float_t w = min((float_t)width / 4.0f, 360.0f);
    float_t h = min((float_t)height, 740.0f);

    igSetNextWindowPos(ImVec2_Empty, ImGuiCond_Appearing, ImVec2_Empty);
    igSetNextWindowSize((ImVec2) { w, h }, ImGuiCond_Appearing);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;

    post_process.imgui_focus = false;
    if (!igBegin(post_process_window_title, &post_process_enabled, window_flags)) {
        post_process.dispose = true;
        igEnd();
        return;
    }

    if (igTreeNode_Str("Tone Trans")) {
        vec3 tone_trans_start = *tone_map_get_tone_trans_start(tm);
        imguiColumnSliderVec3("Start", &tone_trans_start, 0.0f, 1.0f, "%.2f", 0);
        tone_map_set_tone_trans_start(tm, &tone_trans_start);

        vec3 tone_trans_end = *tone_map_get_tone_trans_end(tm);
        imguiColumnSliderVec3("End", &tone_trans_end, 0.0f, 1.0f, "%.2f", 0);
        tone_map_set_tone_trans_end(tm, &tone_trans_end);
        igTreePop();
    }

    if (igTreeNode_Str("Scene Fade")) {
        float_t scene_fade_alpha = tone_map_get_scene_fade_alpha(tm);
        imguiColumnSliderFloat("Alpha", &scene_fade_alpha, 0.0f, 1.0f, "%.2f", 0);
        tone_map_set_scene_fade_alpha(tm, scene_fade_alpha);

        vec3 scene_fade_color = *tone_map_get_scene_fade_color(tm);
        imguiColumnSliderVec3("Color", &scene_fade_color, 0.0f, 1.0f, "%.2f", 0);
        tone_map_set_scene_fade_color(tm, &scene_fade_color);

        const char* scene_fade_blend_func_labels[] = {
            "0: OVER",
            "1: MULTI",
            "2: PLUS",
        };

        int32_t scene_fade_blend_func = tone_map_get_scene_fade_blend_func(tm);
        imguiColumnComboBox("Blend Func", scene_fade_blend_func_labels, 3,
            &scene_fade_blend_func, 0, false, &post_process.imgui_focus);
        tone_map_set_scene_fade_blend_func(tm, scene_fade_blend_func);
        igTreePop();
    }

    if (igTreeNodeEx_Str("Glow Param", ImGuiTreeNodeFlags_DefaultOpen)) {
        const char* tone_map_method_labels[] = {
            "YCC EXPONENT",
            "RGB LINEAR",
            "RGB LINEAR2",
        };

        int32_t tone_map_method = tone_map_get_tone_map_method(tm);
        imguiColumnComboBox("Tone Map", tone_map_method_labels, 3,
            &tone_map_method, 0, false, &post_process.imgui_focus);
        tone_map_set_tone_map_method(tm, tone_map_method);

        float_t exposure = tone_map_get_exposure(tm);
        imguiColumnSliderFloat("Exposure", &exposure, 0.0f, 4.0f, "%.2f", 0);
        exposure = roundf(exposure / 0.02f) * 0.02f;
        tone_map_set_exposure(tm, exposure);

        bool auto_exposure = tone_map_get_auto_exposure(tm);
        imguiCheckbox("Auto Exposure", &auto_exposure);
        tone_map_set_auto_exposure(tm, auto_exposure);

        float_t gamma = tone_map_get_gamma(tm);
        imguiColumnSliderFloat("Gamma", &gamma, 0.2f, 2.2f, "%.2f", 0);
        tone_map_set_gamma(tm, gamma);

        int32_t saturate1 = tone_map_get_saturate1(tm);
        imguiColumnSliderInt("Saturate 1", &saturate1, 1, 6, "%d", 0);
        tone_map_set_saturate1(tm, saturate1);

        float_t saturate2 = tone_map_get_saturate2(tm);
        imguiColumnSliderFloat("Saturate 2", &saturate2, 0.0f, 1.0f, "%.2f", 0);
        tone_map_set_saturate2(tm, saturate2);
        igTreePop();
    }

    if (igTreeNodeEx_Str("Glare", ImGuiTreeNodeFlags_DefaultOpen)) {
        vec3 radius = *radius_get(rad);
        imguiColumnSliderFloat("Radius R", &radius.x, 1.0f, 3.0f, "%.2f", 0);
        imguiColumnSliderFloat("Radius G", &radius.y, 1.0f, 3.0f, "%.2f", 0);
        imguiColumnSliderFloat("Radius B", &radius.z, 1.0f, 3.0f, "%.2f", 0);
        radius_set(rad, &radius);

        vec3 intensity = *intensity_get(inten);
        imguiColumnSliderFloat("Inten  R", &intensity.x, 0.0f, 2.0f, "%.2f", 0);
        imguiColumnSliderFloat("Inten  G", &intensity.y, 0.0f, 2.0f, "%.2f", 0);
        imguiColumnSliderFloat("Inten  B", &intensity.z, 0.0f, 2.0f, "%.2f", 0);
        intensity_set(inten, &intensity);
        igTreePop();
    }

    if (igTreeNodeEx_Str("DOF", ImGuiTreeNodeFlags_DefaultOpen)) {
        bool use_ui_params = dof->debug.flags & DOF_DEBUG_USE_UI_PARAMS;
        bool phys = use_ui_params && dof->debug.flags & DOF_DEBUG_ENABLE_PHYS_DOF;
        bool f2 = use_ui_params && ~dof->debug.flags & DOF_DEBUG_ENABLE_PHYS_DOF;

        float_t alpha = style->Alpha * imgui_alpha_disabled_scale;

        imguiCheckboxFlags_UintPtr("use UI params", (uint32_t*)&dof->debug.flags, DOF_DEBUG_USE_UI_PARAMS);

        if (!use_ui_params) {
            igPushItemFlag(ImGuiItemFlags_Disabled, true);
            igPushStyleVar_Float(ImGuiStyleVar_Alpha, alpha);
        }
        imguiCheckboxFlags_UintPtr("enable DOF", (uint32_t*)&dof->debug.flags, DOF_DEBUG_ENABLE_DOF);
        imguiCheckboxFlags_UintPtr("enable physical DOF", (uint32_t*)&dof->debug.flags, DOF_DEBUG_ENABLE_PHYS_DOF);
        if (!use_ui_params) {
            igPopItemFlag();
            igPopStyleVar(1);
        }

        if (!phys) {
            igPushItemFlag(ImGuiItemFlags_Disabled, true);
            igPushStyleVar_Float(ImGuiStyleVar_Alpha, alpha);
        }
        imguiCheckboxFlags_UintPtr("auto focus", (uint32_t*)&dof->debug.flags, DOF_DEBUG_AUTO_FOCUS);
        if (!phys) {
            igPopItemFlag();
            igPopStyleVar(1);
        }

        imguiSetColumnSpace(16.0f / 31.0f);
        if (!phys) {
            igPushItemFlag(ImGuiItemFlags_Disabled, true);
            igPushStyleVar_Float(ImGuiStyleVar_Alpha, alpha);
        }
        imguiColumnSliderFloat("distance to focus[m]", &dof->debug.distance_to_focus, 0.01f, 30.0f, "%.2f", 0);
        imguiColumnSliderFloat("focal length[mm]", &dof->debug.focal_length, 0.1f, 100.0f, "%.2f", 0);
        imguiColumnSliderFloat("F-Number", &dof->debug.f_number, 0.1f, 40.0f, "%.2f", 0);
        if (!phys) {
            igPopItemFlag();
            igPopStyleVar(1);
        }

        if (!f2) {
            igPushItemFlag(ImGuiItemFlags_Disabled, true);
            igPushStyleVar_Float(ImGuiStyleVar_Alpha, alpha);
        }
        imguiColumnSliderFloat("f2 distance to focus[m]", &dof->debug.f2.distance_to_focus, 0.01f, 30.0f, "%.2f", 0);
        imguiColumnSliderFloat("f2 focus range[m]", &dof->debug.f2.focus_range, 0.0f, 10.0f, "%.2f", 0);
        imguiColumnSliderFloat("f2 fuzzing range[m]", &dof->debug.f2.fuzzing_range, 0.0f, 10.0f, "%.2f", 0);
        imguiColumnSliderFloat("f2 ratio", &dof->debug.f2.ratio, 0.0f, 1.0f, "%.2f", 0);
        if (!f2) {
            igPopItemFlag();
            igPopStyleVar(1);
        }
        imguiSetDefaultColumnSpace();
        igTreePop();
    }

    if (imguiButton("Reset Post Process", ImVec2_Empty)) {
        dof_initialize(dof, 0, 0);
        radius_initialize(rad, (vec3[]) { 2.0f, 2.0f, 2.0f });
        intensity_initialize(inten, (vec3[]) { 1.0f, 1.0f, 1.0f });
        tone_map_initialize(tm, 2.0f, true, 1.0f, 1, 1.0f, (vec3[]) { 0.0f, 0.0f, 0.0f}, 0.0f, 0,
            (vec3[]) { 0.0f, 0.0f, 0.0f }, (vec3[]) { 1.0f, 1.0f, 1.0f }, 0);
    }

    post_process.imgui_focus |= igIsWindowFocused(0);
    igEnd();
}

void post_process_input() {
    if (!post_process_enabled)
        return;

    input_locked |= post_process.imgui_focus;
}

extern void post_process_render() {
    if (post_process.dispose) {
        post_process_dispose();
        return;
    }
    else if (!post_process_enabled)
        return;
}
