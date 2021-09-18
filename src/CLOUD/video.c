/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "video.h"
#include "../CRE/timer.h"
#include "../CRE/video_x264.h"

timer video_timer;

extern bool close;
lock video_lock;

int32_t video_main(void* arg) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
    timer_init(&video_timer, 60.0);

    lock_init(&video_lock);
    if (!lock_check_init(&video_lock))
        goto End;

    bool state_wait = false;
    bool state_disposed = false;
    do {
        lock_lock(&state_lock);
        state_wait = state != RENDER_INITIALIZED;
        state_disposed =  state == RENDER_DISPOSING || state == RENDER_DISPOSED;
        lock_unlock(&state_lock);
        if (state_disposed) {
            lock_free(&video_lock);
            goto End;
        }
        msleep(video_timer.timer, 0.0625);
    } while (state_wait);

    //data_player_init();

    bool local_close = false;
    while (!close && !local_close) {
        timer_start_of_cycle(&video_timer);
        lock_lock(&state_lock);
        local_close = state == RENDER_DISPOSING || state == RENDER_DISPOSED;
        lock_unlock(&state_lock);

        classes_process_video(classes, classes_count);
        timer_end_of_cycle(&video_timer);
    }
    lock_free(&video_lock);
End:
    timer_dispose(&video_timer);
    return 0;
}
