/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "control.h"
#include "input.h"
#include "../KKdLib/io_path.h"
#include "../CRE/task.h"
extern vector_task_render tasks_render;
extern vector_task_render_draw2d tasks_render_draw2d;
extern vector_task_render_draw3d tasks_render_draw3d;

extern bool close;
HANDLE control_lock = 0;
extern HANDLE input_lock;
extern HANDLE render_lock;
extern HWND window_handle;
extern HANDLE mu_lock;
extern HANDLE mu_input_lock;
extern vector_wchar_t mu_input;
extern vector_char mu_input_ansi;

extern int32_t width;
extern int32_t height;
extern bool input_reset;
extern float_t frame_speed;

extern double_t render_freq;
extern double_t sound_freq;
extern double_t input_freq;
extern HANDLE render_freq_lock;
extern HANDLE sound_freq_lock;
extern HANDLE input_freq_lock;

#define FREQ 60
#include "../CRE/timer.h"
timer_val(control);

static void mui_process();

int32_t control_main(void* arg) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    timer_init(control, "Control");
    memset(&tasks_render, 0, sizeof(vector_task_render));

    while (!render_lock)
        msleep(control_timer, 0.0625);

    while (!close) {
        timer_calc_pre(control);
        if (input_lock) {
            WaitForSingleObject(input_lock, INFINITE);
            if (window_handle == GetForegroundWindow()) {
                if (input_is_down(VK_CONTROL) && input_is_tapped('C'))
                    break;
            }
            ReleaseMutex(input_lock);
        }

        if (render_lock) {
            WaitForSingleObject(render_lock, INFINITE);
            vector_task_render_clear(&tasks_render);
            if (muctx && mu_lock && input_lock) {
                WaitForSingleObject(input_lock, INFINITE);
                WaitForSingleObject(mu_lock, INFINITE);
                mui_process();
                ReleaseMutex(mu_lock);
                ReleaseMutex(input_lock);
            }
            ReleaseMutex(render_lock);
        }

        for (size_t i = 0; i < classes_count; i++)
            if (classes[i].control)
                classes[i].control();

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
        msleep(control_timer, 1000.0 / FREQ - cycle_time);
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

static void mui_process() {
    if (mu_input_lock && window_handle == GetForegroundWindow()) {
        POINT position = input_mouse_position();
        double_t scroll_x = input_mouse_scroll_x() * -50.0;
        double_t scroll_y = input_mouse_scroll_y() * -50.0;
        input_mouse_reset_scroll();
        mu_input_mousemove(muctx, position.x, position.y);
        mu_input_scroll(muctx, (int)round(scroll_x), (int)round(scroll_y));

        if (input_is_tapped(VK_LBUTTON))
            mu_input_mousedown(muctx, MU_MOUSE_LEFT);
        else if (input_is_released(VK_LBUTTON))
            mu_input_mouseup(muctx, MU_MOUSE_LEFT);

        if (input_is_tapped(VK_RBUTTON))
            mu_input_mousedown(muctx, MU_MOUSE_RIGHT);
        else if (input_is_released(VK_RBUTTON))
            mu_input_mouseup(muctx, MU_MOUSE_RIGHT);

        if (input_is_tapped(VK_MBUTTON))
            mu_input_mousedown(muctx, MU_MOUSE_MIDDLE);
        else if (input_is_released(VK_MBUTTON))
            mu_input_mouseup(muctx, MU_MOUSE_MIDDLE);

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
        else if (input_is_released(VK_BACK))
            mu_input_keyup(muctx, MU_KEY_BACKSPACE);
        
        if (input_is_tapped(VK_ESCAPE))
            muctx->focus = 0;

        WaitForSingleObject(mu_input_lock, INFINITE);
        mu_input_text(muctx, mu_input_ansi.begin);
        vector_wchar_t_clear(&mu_input);
        mu_input.end++;
        mu_input.end[-1] = 0;
        vector_char_clear(&mu_input_ansi);
        mu_input_ansi.end++;
        mu_input_ansi.end[-1] = 0;
        ReleaseMutex(mu_input_lock);
    }
    
    mu_begin(muctx);
    for (size_t i = 0; i < classes_count; i++)
        if (classes[i].mui)
            classes[i].mui();

    if (input_is_tapped(VK_RBUTTON))
        mu_open_popup(muctx, "context menu");

    if (mu_begin_window_ex(muctx, "context menu",
        mu_rect(0, 0, 180, 120),
        MU_OPT_NOTITLE | MU_OPT_POPUP | MU_OPT_CLOSED, false, true, false)) {
        mu_Rect body = muctx->layout_stack.items[muctx->layout_stack.idx - 1].body;
        mu_layout_row(muctx, 1, (int[]) { body.w }, 0);
        for (size_t i = 0; i < classes_count; i++) {
            if (!classes[i].enabled || !classes[i].init || !classes[i].name || ~classes[i].flags & CLASSES_IN_CONTEXT_MENU)
                continue;

            mu_push_id(muctx, &i, sizeof(i));
            if (*classes[i].enabled)
                mu_draw_control_text(muctx, classes[i].name, mu_layout_next(muctx), MU_COLOR_TEXT, 0);
            else if (mu_button_ex(muctx, classes[i].name, 0, 0))
                classes[i].init();
            mu_pop_id(muctx);
        }
        mu_end_window(muctx);
    }
    mu_end(muctx);
}
