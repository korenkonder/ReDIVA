/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "control.h"
#include "input.h"
#include "../KKdLib/io_path.h"
#include "../CRE/microui.h"
#include "../CRE/post_process.h"
#include "../CRE/task.h"
#include "../CRE/Glitter/glitter.h"
#include "../CRE/Glitter/particle_manager.h"

extern vector_task_render tasks_render;
extern vector_task_render_draw2d tasks_render_draw2d;
extern vector_task_render_draw3d tasks_render_draw3d;

extern bool close;
HANDLE control_lock = 0;
extern HANDLE input_lock;
extern HANDLE render_lock;
extern HWND window_handle;
extern mu_Context* muctx;
extern HANDLE mu_lock;
extern float_t mui_bg[3];

extern radius* rad;
extern intensity* inten;
extern tone_map_sat_gamma* tmsg;
extern tone_map_data* tmd;
extern glitter_particle_manager* gpm;

extern vec2i internal_2d_res;
extern vec2i internal_3d_res;
extern vec2i internal_res;
extern int32_t width;
extern int32_t height;

extern bool input_reset;
extern bool input_play_glitter;
extern bool input_stop_glitter;
extern bool input_auto_glitter;
extern bool input_pause_glitter;
extern float_t glitter_frame_counter;
extern wchar_t* glitter_file;

extern float_t frame_speed;

extern double_t render_freq;
extern double_t sound_freq;
extern double_t input_freq;
extern HANDLE render_freq_lock;
extern HANDLE sound_freq_lock;
extern HANDLE input_freq_lock;

static len_array_pointer_wchar_t glitter_files;

#define FREQ 60
#include "../CRE/timer.h"
timer_val(control);

static void get_files();
static void mui_process();

int32_t control_main(void* arg) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    timer_init(control, "Control");
    memset(&tasks_render, 0, sizeof(vector_task_render));

    while (!render_lock)
        msleep(0.0625);

    glitter_files = len_array_pointer_wchar_t_empty;
    get_files();

    while (!close) {
        timer_calc_pre(control);
        if (input_lock) {
            WaitForSingleObject(input_lock, INFINITE);
            if (window_handle == GetForegroundWindow()) {
                if (input_is_down(VK_CONTROL) && input_is_tapped('C'))
                    break;
                else if (input_is_down(VK_ESCAPE))
                    break;
            }
            ReleaseMutex(input_lock);
        }

        if (render_lock) {
            WaitForSingleObject(render_lock, INFINITE);
            vector_task_render_clear(&tasks_render);
            if (muctx && mu_lock) {
                WaitForSingleObject(mu_lock, INFINITE);
                mui_process();
                ReleaseMutex(mu_lock);
            }
            ReleaseMutex(render_lock);
        }

        if (control_freq_lock || render_freq_lock || sound_freq_lock || input_freq_lock) {
            if (control_freq_lock) {
                WaitForSingleObject(control_freq_lock, INFINITE);
                printf("Control: %04d ", (int32_t)round(control_freq));
                ReleaseMutex(control_freq_lock);
            }

            if (render_freq_lock) {
                WaitForSingleObject(render_freq_lock, INFINITE);
                printf("Render: %04d ", (int32_t)round(render_freq));
                ReleaseMutex(render_freq_lock);
            }

            if (sound_freq_lock) {
                WaitForSingleObject(sound_freq_lock, INFINITE);
                printf("Sound: %04d ", (int32_t)round(sound_freq));
                ReleaseMutex(sound_freq_lock);
            }

            if (input_freq_lock) {
                WaitForSingleObject(input_freq_lock, INFINITE);
                printf("Input: %04d ", (int32_t)round(input_freq));
                ReleaseMutex(input_freq_lock);
            }
            printf("\n");
        }
        double_t cycle_time = timer_calc_post(control);
        msleep(1000.0 / FREQ - cycle_time);
    }
    timer_dispose(control);
    close = true;
    return 0;
}

static char logbuf[64000];
static  int logbuf_updated = 0;

static void write_log(const char* text) {
    if (logbuf[0])
        strcat_s(logbuf, 64000, "\n");
    strcat_s(logbuf, 64000, text);
    logbuf_updated = 1;
}

static int mu_slider_uint8_t(mu_Context* ctx, uint8_t* value, int32_t low, int32_t high) {
    static float_t tmp;
    mu_push_id(ctx, &value, sizeof(value));
    tmp = *value;
    int res = mu_slider_ex(ctx, &tmp, (mu_Real)low, (mu_Real)high, 1, "%.0f", MU_OPT_ALIGNCENTER);
    *value = (uint8_t)tmp;
    mu_pop_id(ctx);
    return res;
}

static int mu_slider_uint8_t_label(mu_Context* ctx, uint8_t* value, int32_t low, int32_t high, const char* label) {
    static float_t tmp;
    mu_push_id(ctx, &value, sizeof(value));
    tmp = *value;
    int res = mu_slider_ex(ctx, &tmp, (mu_Real)low, (mu_Real)high, 1, label, MU_OPT_ALIGNCENTER);
    *value = (uint8_t)tmp;
    mu_pop_id(ctx);
    return res;
}

#define mu_slider_step(ctx, value, low, high, step) \
mu_slider_ex(ctx, value, low, high, step, MU_SLIDER_FMT, MU_OPT_ALIGNCENTER);

static void mui_glitter_contol(mu_Context* muctx) {
    int32_t w;
    int32_t h;

    w = min(width / 3, 360);
    h = min(height, 540);

    if (mu_begin_window_ex(muctx, "Post Process",
        mu_rect(width - w, 0, w, h),
        MU_OPT_NOINTERACT | MU_OPT_NORESIZE | MU_OPT_NOCLOSE, true)) {
        if (mu_begin_treenode(muctx, "Tone Trans")) {
            mu_Rect body = muctx->layout_stack.items[muctx->layout_stack.idx - 1].body;
            int sw = (int)((body.w - (52 + 2) - (16 + 2) * 2 - (muctx->style->spacing + 2) * 6) / 3.0f);

            mu_layout_begin_column(muctx);
            mu_layout_row(muctx, 6, (int[]) { 52, sw, 16, sw, 16, sw }, 0);

            vec3 tone_trans_start = *tone_map_data_get_tone_trans_start(tmd);
            mu_label(muctx, "Start R:");
            mu_slider_step(muctx, &tone_trans_start.x, 0.0f, 1.0f, 0.01f);
            mu_label(muctx, "G:");
            mu_slider_step(muctx, &tone_trans_start.y, 0.0f, 1.0f, 0.01f);
            mu_label(muctx, "B:");
            mu_slider_step(muctx, &tone_trans_start.z, 0.0f, 1.0f, 0.01f);
            tone_map_data_set_tone_trans_start(tmd, &tone_trans_start);

            vec3 tone_trans_end = *tone_map_data_get_tone_trans_end(tmd);
            mu_label(muctx, "End   R:");
            mu_slider_step(muctx, &tone_trans_end.x, 0.0f, 1.0f, 0.01f);
            mu_label(muctx, "G:");
            mu_slider_step(muctx, &tone_trans_end.y, 0.0f, 1.0f, 0.01f);
            mu_label(muctx, "B:");
            mu_slider_step(muctx, &tone_trans_end.z, 0.0f, 1.0f, 0.01f);
            tone_map_data_set_tone_trans_end(tmd, &tone_trans_end);
            mu_layout_end_column(muctx);
            mu_end_treenode(muctx);
        }

        if (mu_begin_treenode(muctx, "Scene Fade")) {
            mu_Rect body = muctx->layout_stack.items[muctx->layout_stack.idx - 1].body;
            int sw = (int)((body.w - (70 + 2) - (16 + 2) * 2 - (muctx->style->spacing + 2) * 6) / 3.0f);

            mu_layout_begin_column(muctx);
            mu_layout_row(muctx, 2, (int[]) { 68, -1 }, 0);

            float_t scene_fade_alpha = tone_map_data_get_scene_fade_alpha(tmd);
            mu_label(muctx, "Alpha:");
            mu_slider_step(muctx, &scene_fade_alpha, 0.0f, 1.0f, 0.01f);
            tone_map_data_set_scene_fade_alpha(tmd, scene_fade_alpha);

            mu_layout_row(muctx, 6, (int[]) { 68, sw, 16, sw, 16, sw }, 0);
            
            vec3 scene_fade_color = *tone_map_data_get_scene_fade_color(tmd);
            mu_label(muctx, "Color R:");
            mu_slider_step(muctx, &scene_fade_color.x, 0.0f, 1.0f, 0.01f);
            mu_label(muctx, "G:");
            mu_slider_step(muctx, &scene_fade_color.y, 0.0f, 1.0f, 0.01f);
            mu_label(muctx, "B:");
            mu_slider_step(muctx, &scene_fade_color.z, 0.0f, 1.0f, 0.01f);
            tone_map_data_set_scene_fade_color(tmd, &scene_fade_color);

            mu_layout_row(muctx, 2, (int[]) { 68, -1 }, 0);

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
            mu_layout_end_column(muctx);
            mu_end_treenode(muctx);
        }

        if (mu_begin_treenode_ex(muctx, "Glow Param", MU_OPT_EXPANDED)) {
            mu_layout_begin_column(muctx);
            mu_layout_row(muctx, 2, (int[]) { 64, -1 }, 0);

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
            mu_layout_end_column(muctx);

            bool auto_exposure = tone_map_data_get_auto_exposure(tmd);
            mu_checkbox(muctx, "Auto Exposure", &auto_exposure);
            tone_map_data_set_auto_exposure(tmd, auto_exposure);

            mu_layout_begin_column(muctx);
            mu_layout_row(muctx, 2, (int[]) { 64, -1 }, 0);
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
            mu_layout_end_column(muctx);
            mu_end_treenode(muctx);
        }

        if (mu_begin_treenode_ex(muctx, "Glare", MU_OPT_EXPANDED)) {
            mu_layout_begin_column(muctx);
            mu_layout_row(muctx, 2, (int[]) { 52, -1 }, 0);

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
            mu_layout_end_column(muctx);
            mu_end_treenode(muctx);
        }
        mu_end_window(muctx);
    }

    w = min(width / 3, 360);
    h = min(height - 84 - muctx->style->title_height, 300);

    if (mu_begin_window_ex(muctx, "Glitter",
        mu_rect(0, 0, w, h),
        MU_OPT_NOINTERACT | MU_OPT_NORESIZE | MU_OPT_NOCLOSE, true)) {
        mu_layout_row(muctx, 2, (int[]) { 32, -1 }, 0);
        mu_label(muctx, "File:");
        char* glitter_file_temp = wchar_t_string_to_char_string(glitter_file);
        if (mu_button(muctx, glitter_file_temp ? glitter_file_temp : "...")) {
            mu_Container* cnt = mu_get_container(muctx, "glitter file");
            muctx->hover_root = muctx->next_hover_root = cnt;
            cnt->rect = mu_rect(muctx->mouse_pos.x, muctx->mouse_pos.y, 240, 360);
            cnt->open = 1;
            mu_bring_to_front(muctx, cnt);
        }
        free(glitter_file_temp);

        if (mu_begin_window_ex(muctx, "glitter file",
            mu_rect(0, 0, 0, 0),
            MU_OPT_POPUP | MU_OPT_NORESIZE | MU_OPT_NOTITLE | MU_OPT_CLOSED, false)) {
            mu_Rect body = muctx->layout_stack.items[muctx->layout_stack.idx - 1].body;
            mu_layout_row(muctx, 1, (int[]) { body.w }, 0);
            for (size_t i = 0; i < glitter_files.length; i++) {
                char* temp = wchar_t_string_to_char_string(glitter_files.data[i]);
                mu_push_id(muctx, temp, strlen(temp));
                if (glitter_file && !wcscmp(glitter_files.data[i], glitter_file))
                    mu_draw_control_text(muctx, temp, mu_layout_next(muctx), MU_COLOR_TEXT, MU_OPT_ALIGNCENTER);
                else if (mu_button(muctx, temp))
                    glitter_file = glitter_files.data[i];
                free(temp);
                mu_pop_id(muctx);
            }
            mu_end_window(muctx);
        }

        mu_layout_row(muctx, 1, (int[]) { -1 }, 0);
        if (mu_button(muctx, "Reset Camera (R)"))
            input_reset = true;

        if (mu_button(muctx, "Play (F)"))
            input_play_glitter = true;

        if (mu_button(muctx, "Stop (V)"))
            input_stop_glitter = true;

        mu_checkbox(muctx, "Auto (T)", &input_auto_glitter);
        mu_checkbox(muctx, "Pause (G)", &input_pause_glitter);

        size_t ctrl;
        size_t disp;
        float_t frame;
        float_t life_time;
        char buf[0x40];

        mu_layout_row(muctx, 1, (int[]) { -1 }, 0);
        glitter_particle_manager_get_time(gpm, &frame, &life_time);
        sprintf_s(buf, sizeof(buf), "%.0f - %.0f/%.0f", glitter_frame_counter, frame, life_time);
        mu_label(muctx, buf);

        mu_layout_row(muctx, 2, (int[]) { 40, -1 }, 0);
        ctrl = glitter_particle_manager_get_ctrl_count(gpm, GLITTER_PARTICLE_QUAD);
        disp = glitter_particle_manager_get_disp_count(gpm, GLITTER_PARTICLE_QUAD);
        mu_label(muctx, "Quad: ");
        sprintf_s(buf, sizeof(buf), "ctrl%lld, disp%lld", ctrl, disp);
        mu_label(muctx, buf);

        ctrl = glitter_particle_manager_get_ctrl_count(gpm, GLITTER_PARTICLE_LOCUS);
        disp = glitter_particle_manager_get_disp_count(gpm, GLITTER_PARTICLE_LOCUS);
        mu_label(muctx, "Locus: ");
        sprintf_s(buf, sizeof(buf), "ctrl%lld, disp%lld", ctrl, disp);
        mu_label(muctx, buf);

        ctrl = glitter_particle_manager_get_ctrl_count(gpm, GLITTER_PARTICLE_LINE);
        disp = glitter_particle_manager_get_disp_count(gpm, GLITTER_PARTICLE_LINE);
        mu_label(muctx, "Line: ");
        sprintf_s(buf, sizeof(buf), "ctrl%lld, disp%lld", ctrl, disp);
        mu_label(muctx, buf);

        mu_layout_row(muctx, 2, (int[]) { 80, -1 }, 0);
        mu_label(muctx, "Frame Speed: ");
        mu_slider_step(muctx, &frame_speed, 0.0f, 2.0f, 0.01f);
        mu_end_window(muctx);
    }

    w = min(width / 3, 360);
    h = 84 + muctx->style->title_height;

    if (mu_begin_window_ex(muctx, "Background Color",
        mu_rect(0, height - h, w, h),
        MU_OPT_NOINTERACT | MU_OPT_NORESIZE | MU_OPT_NOCLOSE, true)) {
        mu_layout_row(muctx, 2, (int[]) { -78, -1 }, 74);

        mu_layout_begin_column(muctx);
        mu_layout_row(muctx, 2, (int[]) { 46, -1 }, 0);

        mu_Color c = mu_color_real(mui_bg[0], mui_bg[1], mui_bg[2], 1.0f);
        mu_label(muctx, "Red:");
        mu_slider_uint8_t(muctx, &c.r, 0, 255);
        mu_label(muctx, "Green:");
        mu_slider_uint8_t(muctx, &c.g, 0, 255);
        mu_label(muctx, "Blue:");
        mu_slider_uint8_t(muctx, &c.b, 0, 255);
        mu_layout_end_column(muctx);

        mu_Rect r = mu_layout_next(muctx);
        mu_draw_rect(muctx, r, c);
        char buf[32];
        sprintf_s(buf, 32, "#%02X%02X%02X", c.r, c.g, c.b);
        mu_draw_control_text(muctx, buf, r, MU_COLOR_TEXT, MU_OPT_ALIGNCENTER);
        mui_bg[0] = c.r * (1.0f / 255.0f);
        mui_bg[1] = c.g * (1.0f / 255.0f);
        mui_bg[2] = c.b * (1.0f / 255.0f);
        mu_end_window(muctx);
    }
}

static void get_files() {
    len_array_pointer_wchar_t_free(&glitter_files);
    path_wget_files(&glitter_files, L"rom\\particle");
    for (size_t i = 0; i < glitter_files.length; i++) {
        if (path_wcheck_ends_with(glitter_files.data[i], L".farc")) {
            wchar_t* temp = path_wget_without_extension(glitter_files.data[i]);
            free(glitter_files.data[i]);
            glitter_files.data[i] = temp;
        }
        else
            free(glitter_files.data[i]);
    }

    size_t glitter_files_count = 0;
    for (size_t i = 0; i < glitter_files.length; i++)
        if (glitter_files.data[i])
            glitter_files.data[glitter_files_count++] = glitter_files.data[i];
    glitter_files.length = glitter_files_count;
}

static void mui_process() {
    if (window_handle == GetForegroundWindow()) {
        POINT position = input_mouse_position();
        mu_input_mousemove(muctx, position.x, position.y);
        if (input_is_tapped(VK_LBUTTON))
            mu_input_mousedown(muctx, position.x, position.y, MU_MOUSE_LEFT);
        else if (input_is_released(VK_LBUTTON))
            mu_input_mouseup(muctx, position.x, position.y, MU_MOUSE_LEFT);

        if (input_is_tapped(VK_RBUTTON))
            mu_input_mousedown(muctx, position.x, position.y, MU_MOUSE_RIGHT);
        else if (input_is_released(VK_RBUTTON))
            mu_input_mouseup(muctx, position.x, position.y, MU_MOUSE_RIGHT);

        if (input_is_tapped(VK_MBUTTON))
            mu_input_mousedown(muctx, position.x, position.y, MU_MOUSE_MIDDLE);
        else if (input_is_released(VK_MBUTTON))
            mu_input_mouseup(muctx, position.x, position.y, MU_MOUSE_MIDDLE);

        if (input_is_tapped(VK_SHIFT) || input_is_tapped(VK_LSHIFT) || input_is_tapped(VK_RSHIFT))
            mu_input_keydown(muctx, MU_KEY_SHIFT);
        else if (input_is_released(VK_SHIFT) || input_is_released(VK_SHIFT) || input_is_released(VK_SHIFT))
            mu_input_keyup(muctx, MU_KEY_SHIFT);

        if (input_is_tapped(VK_CONTROL) || input_is_tapped(VK_LCONTROL) || input_is_tapped(VK_RCONTROL))
            mu_input_keydown(muctx, MU_KEY_CTRL);
        else if (input_is_released(VK_CONTROL) || input_is_released(VK_CONTROL) || input_is_released(VK_CONTROL))
            mu_input_keyup(muctx, MU_KEY_CTRL);

        if (input_is_tapped(VK_MENU) || input_is_tapped(VK_LMENU) || input_is_tapped(VK_RMENU))
            mu_input_keydown(muctx, MU_KEY_ALT);
        else if (input_is_released(VK_MENU) || input_is_released(VK_LMENU) || input_is_released(VK_RMENU))
            mu_input_keyup(muctx, MU_KEY_ALT);

        if (input_is_tapped(VK_RETURN))
            mu_input_keydown(muctx, MU_KEY_RETURN);
        else if (input_is_released(VK_RETURN))
            mu_input_keyup(muctx, MU_KEY_RETURN);

        if (input_is_tapped(VK_BACK))
            mu_input_keydown(muctx, MU_KEY_BACKSPACE);
        else if (input_is_released(VK_RETURN))
            mu_input_keyup(muctx, MU_KEY_BACKSPACE);
    }
    
    mu_begin(muctx);
    mui_glitter_contol(muctx);
    mu_end(muctx);
}
