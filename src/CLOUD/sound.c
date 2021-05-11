/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "sound.h"

extern bool close;

#define FREQ 60
#include "../CRE/timer.h"
timer_val(sound);

int32_t sound_main(void* arg) {
    timer_init(sound, "Sound");
    while (state != RENDER_INITIALIZED)
        msleep(sound_timer, 0.0625);

    while (!close) {
        timer_calc_pre(sound);
        classes_process_sound(classes, classes_count);
        double_t cycle_time = timer_calc_post(sound);
        msleep(sound_timer, 1000.0 / FREQ - cycle_time);
    }
    timer_dispose(sound);
    return 0;
}
