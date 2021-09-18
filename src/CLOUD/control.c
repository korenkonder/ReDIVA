/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "control.h"
#include "input.h"
#include "../KKdLib/io_path.h"
#include "../CRE/lock.h"
#include "../CRE/timer.h"

timer control_timer;

extern bool close;
HANDLE control_lock;
extern lock render_lock;
extern HWND window_handle;

extern int32_t width;
extern int32_t height;
extern bool input_reset;
extern float_t frame_speed;

int32_t control_main(void* arg) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    timer_init(&control_timer, 60.0);

    while (!lock_check_init(&render_lock))
        msleep(control_timer.timer, 0.0625);

    bool local_close = false;
    while (!close && !local_close) {
        timer_start_of_cycle(&control_timer);
        lock_lock(&state_lock);
        local_close = state == RENDER_DISPOSING || state == RENDER_DISPOSED;
        lock_unlock(&state_lock);

        classes_process_control(classes, classes_count);
        timer_end_of_cycle(&control_timer);
    }
    timer_dispose(&control_timer);
    close = true;
    return 0;
}
