/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "sound.hpp"
#include "../CRE/timer.hpp"
#include <timeapi.h>

timer* sound_timer;

extern bool close;

int32_t sound_main(void* arg) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    timeBeginPeriod(1);
    sound_timer = new timer(60.0);

    bool state_wait = false;
    bool state_disposed = false;
    do {
        lock_lock(&state_lock);
        state_wait = state != RENDER_INITIALIZING;
        state_disposed = state == RENDER_DISPOSING;
        lock_unlock(&state_lock);
        if (state_disposed) {
            delete sound_timer;
            return 0;
        }
        sound_timer->sleep(0.0625);
    } while (state_wait);

    lock_lock(&state_lock);
    enum_or(thread_flags, THREAD_SOUND);
    lock_unlock(&state_lock);

    bool local_close = false;
    sound_timer->reset();
    while (!close && !local_close) {
        sound_timer->start_of_cycle();
        lock_lock(&state_lock);
        local_close = state == RENDER_DISPOSING;
        lock_unlock(&state_lock);

        classes_process_sound(classes, classes_count);
        sound_timer->end_of_cycle();
    }
    delete sound_timer;

    lock_lock(&state_lock);
    enum_and(thread_flags, ~THREAD_SOUND);
    lock_unlock(&state_lock);
    return 0;
}
