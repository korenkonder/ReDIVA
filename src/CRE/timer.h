/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "lock.h"
#include "time.h"

#define HISTORY_COUNT 0x08

struct timer {
    double_t history[HISTORY_COUNT];
    uint8_t history_counter;
    time_struct curr_time;
    time_struct prev_time;
    double_t freq;
    double_t freq_hist;
    lock freq_lock;
    lock freq_hist_lock;
    HANDLE timer;
};

extern LARGE_INTEGER performance_frequency;

extern void timer_init(timer* t, double_t freq);
extern void timer_start_of_cycle(timer* t);
extern void timer_end_of_cycle(timer* t);
extern double_t timer_get_freq(timer* t);
extern void timer_set_freq(timer* t, double_t freq);
extern double_t timer_get_freq_hist(timer* t);
extern double_t timer_get_freq_ratio(timer* t);
extern void timer_reset(timer* t);
extern void timer_sleep(timer* t, double_t msec);
extern void timer_dispose(timer* t);

extern HANDLE timer_handle_init();
extern void timer_handle_sleep(HANDLE timer, double_t msec);
extern void timer_handle_dispose(HANDLE timer);
