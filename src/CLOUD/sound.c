/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "sound.h"
#include "../CRE/timer.h"
#include <timeapi.h>

timer sound_timer;

extern bool close;

int32_t sound_main(void* arg) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    timeBeginPeriod(1);
    timer_init(&sound_timer, 60.0);

    bool state_wait = false;
    bool state_disposed = false;
    do {
        lock_lock(&state_lock);
        state_wait = state != RENDER_INITIALIZED;
        state_disposed = state == RENDER_DISPOSED;
        lock_unlock(&state_lock);
        if (state_disposed) {
            timer_dispose(&sound_timer);
            return 0;
        }
        timer_sleep(&sound_timer, 0.0625);
    } while (state_wait);

    bool local_close = false;
    timer_reset(&sound_timer);
    while (!close && !local_close) {
        timer_start_of_cycle(&sound_timer);
        lock_lock(&state_lock);
        local_close = state == RENDER_DISPOSED;
        lock_unlock(&state_lock);

        classes_process_sound(classes, classes_count);
        timer_end_of_cycle(&sound_timer);
    }
    timer_dispose(&sound_timer);
    return 0;
}
