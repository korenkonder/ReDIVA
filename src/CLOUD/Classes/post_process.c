/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "post_process.h"
#include "../../CRE/microui.h"
#include "../../CRE/post_process.h"

extern mu_Context* muctx;

extern int32_t width;
extern int32_t height;

const char* post_process_window_title = "Post Process";

extern radius* rad;
extern intensity* inten;
extern tone_map_sat_gamma* tmsg;
extern tone_map_data* tmd;

post_process_struct post_process;

extern void post_process_dispose() {
    post_process.enabled = false;
    post_process.dispose = false;
    post_process.disposed = true;
}

extern void post_process_init() {
    post_process.enabled = true;
}

extern void post_process_control() {
    if (!post_process.enabled)
        return;
}

extern void post_process_input() {
    if (!post_process.enabled)
        return;
}

extern void post_process_mui() {
    if (!post_process.enabled)
        return;
    else if (post_process.disposed) {
        mu_Container* cnt = mu_get_container(muctx, post_process_window_title);
        muctx->hover_root = muctx->next_hover_root = cnt;
        cnt->rect = mu_rect(0, 0, 0, 0);
        cnt->open = true;
        mu_bring_to_front(muctx, cnt);
        post_process.disposed = false;
    }

    int32_t w = min(width / 3, 360);
    int32_t h = 540;

    if (!mu_begin_window_ex(muctx, post_process_window_title,
        mu_rect(0, 0, w, h),
        0 /*MU_OPT_NOINTERACT | MU_OPT_NOCLOSE*/, false, true, false)) {
        post_process.dispose = true;
        return;
    }

    if (mu_begin_treenode(muctx, "Tone Trans")) {
        int32_t tw0 = mu_misc_get_max_text_width(muctx, (char* []) {
            "Start", "End"
        }, 2);
        int32_t tw1 = mu_misc_get_max_text_width(muctx, (char* []) {
            "R:"
        }, 1);
        int32_t tw2 = mu_misc_get_max_text_width(muctx, (char* []) {
            "G:"
        }, 1);
        int32_t tw3 = mu_misc_get_max_text_width(muctx, (char* []) {
            "B:"
        }, 1);
        mu_Rect body = muctx->layout_stack.items[muctx->layout_stack.idx - 1].body;
        int32_t sw = (int)((body.w - (tw0 + 4) - (tw1 + 2) - (tw2 + 2) - (tw3 + 2) - (muctx->style->spacing + 2) * 7) / 3.0f);

        mu_layout_row(muctx, 7, (int[]) { tw0, tw1, sw, tw2, sw, tw3, sw }, 0);

        vec3 tone_trans_start = *tone_map_data_get_tone_trans_start(tmd);
        mu_label(muctx, "Start");
        mu_label(muctx, "R:");
        mu_slider_step(muctx, &tone_trans_start.x, 0.0f, 1.0f, 0.01f);
        mu_label(muctx, "G:");
        mu_slider_step(muctx, &tone_trans_start.y, 0.0f, 1.0f, 0.01f);
        mu_label(muctx, "B:");
        mu_slider_step(muctx, &tone_trans_start.z, 0.0f, 1.0f, 0.01f);
        tone_map_data_set_tone_trans_start(tmd, &tone_trans_start);

        vec3 tone_trans_end = *tone_map_data_get_tone_trans_end(tmd);
        mu_label(muctx, "End");
        mu_label(muctx, "R:");
        mu_slider_step(muctx, &tone_trans_end.x, 0.0f, 1.0f, 0.01f);
        mu_label(muctx, "G:");
        mu_slider_step(muctx, &tone_trans_end.y, 0.0f, 1.0f, 0.01f);
        mu_label(muctx, "B:");
        mu_slider_step(muctx, &tone_trans_end.z, 0.0f, 1.0f, 0.01f);
        tone_map_data_set_tone_trans_end(tmd, &tone_trans_end);
        mu_end_treenode(muctx);
    }

    if (mu_begin_treenode(muctx, "Scene Fade")) {
        int32_t tw0 = mu_misc_get_max_text_width(muctx, (char* []) {
            "Alpha:", "Color", "Blend Func:"
        }, 3);
        int32_t tw1 = mu_misc_get_max_text_width(muctx, (char* []) {
            "R:"
        }, 1);
        int32_t tw2 = mu_misc_get_max_text_width(muctx, (char* []) {
            "G:"
        }, 1);
        int32_t tw3 = mu_misc_get_max_text_width(muctx, (char* []) {
            "B:"
        }, 1);
        mu_Rect body = muctx->layout_stack.items[muctx->layout_stack.idx - 1].body;
        int32_t sw = (int)((body.w - (tw0 + 4) - (tw1 + 2) - (tw2 + 2) - (tw3 + 2) - (muctx->style->spacing + 2) * 7) / 3.0f);

        mu_layout_row(muctx, 2, (int[]) { tw0, -1 }, 0);

        float_t scene_fade_alpha = tone_map_data_get_scene_fade_alpha(tmd);
        mu_label(muctx, "Alpha:");
        mu_slider_step(muctx, &scene_fade_alpha, 0.0f, 1.0f, 0.01f);
        tone_map_data_set_scene_fade_alpha(tmd, scene_fade_alpha);

        mu_layout_row(muctx, 7, (int[]) { tw0, tw1, sw, tw2, sw, tw3, sw }, 0);

        vec3 scene_fade_color = *tone_map_data_get_scene_fade_color(tmd);
        mu_label(muctx, "Color");
        mu_label(muctx, "R:");
        mu_slider_step(muctx, &scene_fade_color.x, 0.0f, 1.0f, 0.01f);
        mu_label(muctx, "G:");
        mu_slider_step(muctx, &scene_fade_color.y, 0.0f, 1.0f, 0.01f);
        mu_label(muctx, "B:");
        mu_slider_step(muctx, &scene_fade_color.z, 0.0f, 1.0f, 0.01f);
        tone_map_data_set_scene_fade_color(tmd, &scene_fade_color);

        mu_layout_row(muctx, 2, (int[]) { tw0, -1 }, 0);

        const char* blend_func_label;
        uint8_t scene_fade_blend_func = (uint8_t)tone_map_data_get_scene_fade_blend_func(tmd);
        switch (scene_fade_blend_func) {
        case 0:
        default:
            blend_func_label = "0: OVER";
            break;
        case 1:
            blend_func_label = "1: MULTI";
            break;
        case 2:
            blend_func_label = "2: PLUS";
            break;
        }
        mu_label(muctx, "Blend Func:");
        mu_slider_uint8_t_label(muctx, &scene_fade_blend_func, 0, 2, blend_func_label);
        tone_map_data_set_scene_fade_blend_func(tmd, scene_fade_blend_func);
        mu_end_treenode(muctx);
    }

    if (mu_begin_treenode_ex(muctx, "Glow Param", MU_OPT_EXPANDED)) {
        int32_t tw0 = mu_misc_get_max_text_width(muctx, (char* []) {
            "Tone Map:", "Exposure:", "Gamma:", "Saturate 1:", "Saturate 2:"
        }, 5);

        mu_layout_row(muctx, 2, (int[]) { tw0, -1 }, 0);

        const char* tone_map_method_label;
        uint8_t tone_map_method = (uint8_t)tone_map_data_get_tone_map_method(tmd);
        switch (tone_map_method) {
        case 0:
        default:
            tone_map_method_label = "YCC EXPONENT";
            break;
        case 1:
            tone_map_method_label = "RGB LINEAR";
            break;
        case 2:
            tone_map_method_label = "RGB LINEAR2";
            break;
        }
        mu_label(muctx, "Tone Map:");
        mu_slider_uint8_t_label(muctx, &tone_map_method, 0, 2, tone_map_method_label);
        tone_map_data_set_tone_map_method(tmd, tone_map_method);

        mu_layout_row(muctx, 1, (int[]) { -1 }, 0);
        bool auto_exposure = tone_map_data_get_auto_exposure(tmd);
        mu_checkbox(muctx, "Auto Exposure", &auto_exposure);
        tone_map_data_set_auto_exposure(tmd, auto_exposure);

        mu_layout_row(muctx, 2, (int[]) { tw0, -1 }, 0);
        float_t exposure = tone_map_data_get_exposure(tmd);
        mu_label(muctx, "Exposure:");
        mu_slider_step(muctx, &exposure, 0.0f, 4.0f, 0.02f);
        tone_map_data_set_exposure(tmd, exposure);

        float_t gamma = tone_map_sat_gamma_get_gamma(tmsg);
        mu_label(muctx, "Gamma:");
        mu_slider_step(muctx, &gamma, 0.2f, 2.2f, 0.01f);
        tone_map_sat_gamma_set_gamma(tmsg, gamma);

        uint8_t saturate1 = tone_map_sat_gamma_get_saturate1(tmsg);
        mu_label(muctx, "Saturate 1:");
        mu_slider_uint8_t(muctx, &saturate1, 1, 6);
        tone_map_sat_gamma_set_saturate1(tmsg, saturate1);

        float_t saturate2 = tone_map_sat_gamma_get_saturate2(tmsg);
        mu_label(muctx, "Saturate 2:");
        mu_slider_step(muctx, &saturate2, 0.0f, 1.0f, 0.01f);
        tone_map_sat_gamma_set_saturate2(tmsg, saturate2);
        mu_end_treenode(muctx);
    }

    if (mu_begin_treenode_ex(muctx, "Glare", MU_OPT_EXPANDED)) {
        int32_t tw0 = mu_misc_get_max_text_width(muctx, (char* []) {
            "Radius R:", "Radius G:", "Radius B:", "Inten  R:", "Inten  G:", "Inten  B:"
        }, 6);

        mu_layout_row(muctx, 2, (int[]) { tw0, -1 }, 0);

        vec3 radius = *radius_get(rad);
        mu_label(muctx, "Radius R:");
        mu_slider_step(muctx, &radius.x, 1.0f, 3.0f, 0.01f);
        mu_label(muctx, "Radius G:");
        mu_slider_step(muctx, &radius.y, 1.0f, 3.0f, 0.01f);
        mu_label(muctx, "Radius B:");
        mu_slider_step(muctx, &radius.z, 1.0f, 3.0f, 0.01f);
        radius_set(rad, &radius);

        vec3 intensity = *intensity_get(inten);
        mu_label(muctx, "Inten  R:");
        mu_slider_step(muctx, &intensity.x, 0.0f, 2.0f, 0.01f);
        mu_label(muctx, "Inten  G:");
        mu_slider_step(muctx, &intensity.y, 0.0f, 2.0f, 0.01f);
        mu_label(muctx, "Inten  B:");
        mu_slider_step(muctx, &intensity.z, 0.0f, 2.0f, 0.01f);
        intensity_set(inten, &intensity);
        mu_end_treenode(muctx);
    }
    mu_end_window(muctx);
}

extern void post_process_render() {
    if (post_process.dispose) {
        post_process_dispose();
        return;
    }
    else if (!post_process.enabled)
        return;
}

extern void post_process_sound() {
    if (!post_process.enabled)
        return;
}
