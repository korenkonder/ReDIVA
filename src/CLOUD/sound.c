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
    while (!close) {
        timer_calc_pre(sound);
        double_t cycle_time = timer_calc_post(sound);
        msleep(1000.0 / FREQ - cycle_time);
    }
    timer_dispose(sound);
    return 0;
}
