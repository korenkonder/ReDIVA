/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "control.h"
#include "input.h"
#include "../CRE/task.h"

extern vector_task_render tasks_render;
extern vector_task_render_draw2d tasks_render_draw2d;
extern vector_task_render_draw3d tasks_render_draw3d;

extern bool close;
HANDLE control_lock = 0;
extern HANDLE input_lock;
extern HANDLE render_lock;
extern HWND window_handle;

extern double_t render_freq;
extern double_t sound_freq;
extern double_t input_freq;
extern HANDLE render_freq_lock;
extern HANDLE sound_freq_lock;
extern HANDLE input_freq_lock;

#define FREQ 60
#include "../CRE/timer.h"
timer_val(control);

int32_t control_main(void* arg) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    timer_init(control, "Control");
    memset(&tasks_render, 0, sizeof(vector_task_render));

    while (!render_lock)
        msleep(0.0625);

    while (!close) {
        timer_calc_pre(control);
        if (input_lock) {
            WaitForSingleObject(input_lock, INFINITE);
            if (window_handle == GetForegroundWindow()) {
                if (input_keyboard_is_down(VK_CONTROL) && input_keyboard_is_tapped('C'))
                    break;
                else if (input_keyboard_is_down(VK_ESCAPE))
                    break;
            }
            ReleaseMutex(input_lock);
        }

        if (render_lock) {
            WaitForSingleObject(render_lock, INFINITE);
            vector_task_render_clear(&tasks_render);
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