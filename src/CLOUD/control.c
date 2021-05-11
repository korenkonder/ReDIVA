/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "control.h"
#include "input.h"
#include "../KKdLib/io_path.h"
#include "../CRE/lock.h"
#include "../CRE/task.h"
extern vector_task_render tasks_render;
extern vector_task_render_draw2d tasks_render_draw2d;
extern vector_task_render_draw3d tasks_render_draw3d;

extern bool close;
HANDLE control_lock;
lock_extern_val(render_lock);
extern HWND window_handle;

extern int32_t width;
extern int32_t height;
extern bool input_reset;
extern float_t frame_speed;

extern double_t render_freq;
extern double_t sound_freq;
extern double_t input_freq;
lock_extern_val(render_freq_lock);
lock_extern_val(sound_freq_lock);
lock_extern_val(input_freq_lock);

#define FREQ 60
#include "../CRE/timer.h"
timer_val(control);

int32_t control_main(void* arg) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    timer_init(control, "Control");

    while (!lock_check_init(render_lock))
        msleep(control_timer, 0.0625);

    while (!close) {
        timer_calc_pre(control);
        lock_lock(render_lock);
        lock_unlock(render_lock);

        classes_process_control(classes, classes_count);

        if (lock_check_init(control_freq_lock) || lock_check_init(render_freq_lock)
            || lock_check_init(sound_freq_lock) || lock_check_init(input_freq_lock)) {
            char buf[0x1000];
            int32_t len = 0;
            lock_lock(control_freq_lock);
            len += snprintf(buf + len, sizeof(buf) - len, "Control: %04d ", (int32_t)round(control_freq));
            lock_unlock(control_freq_lock);

            lock_lock(render_freq_lock);
            len += snprintf(buf + len, sizeof(buf) - len, "Render: %04d ", (int32_t)round(render_freq));
            lock_unlock(render_freq_lock);

            lock_lock(render_freq_lock);
            len += snprintf(buf + len, sizeof(buf) - len, "Sound: %04d ", (int32_t)round(sound_freq));
            lock_unlock(render_freq_lock);

            lock_lock(input_freq_lock);
            len += snprintf(buf + len, sizeof(buf) - len, "Input: %04d ", (int32_t)round(input_freq));
            lock_unlock(input_freq_lock);
            len += snprintf(buf + len, sizeof(buf) - len, "\n");
            printf(buf);
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
