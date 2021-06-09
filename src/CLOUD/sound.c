/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "sound.h"
#include "../CRE/timer.h"

timer sound_timer;

extern bool close;

int32_t sound_main(void* arg) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    timer_init(&sound_timer, 60.0);
    bool state_wait = false;
    do {
        lock_lock(state_lock);
        state_wait = state != RENDER_INITIALIZED;
        lock_unlock(state_lock);
        msleep(sound_timer.timer, 0.0625);
    } while (state_wait);

    bool local_close = false;
    while (!close && !local_close) {
        timer_start_of_cycle(&sound_timer);
        lock_lock(state_lock);
        local_close = state == RENDER_DISPOSING || state == RENDER_DISPOSED;
        lock_unlock(state_lock);

        classes_process_sound(classes, classes_count);
        timer_end_of_cycle(&sound_timer);
    }
    timer_dispose(&sound_timer);
    return 0;
}
