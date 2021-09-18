/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "lock.h"

#define HISTORY_COUNT 0x08

typedef struct timer {
    double_t history[HISTORY_COUNT];
    uint8_t history_counter;
    LARGE_INTEGER curr_time;
    LARGE_INTEGER prev_time;
    double_t freq;
    double_t freq_hist;
    lock freq_lock;
    lock freq_hist_lock;
    HANDLE timer;
} timer;

extern LARGE_INTEGER performance_frequency;

extern void timer_init(timer* t, double_t freq);
extern void timer_start_of_cycle(timer* t);
extern void timer_end_of_cycle(timer* t);
extern double_t timer_get_freq(timer* t);
extern void timer_set_freq(timer* t, double_t freq);
extern double_t timer_get_freq_hist(timer* t);
extern void timer_dispose(timer* t);

extern double_t timer_get_msec(LARGE_INTEGER t);
extern HANDLE create_timer();
extern void dispose_timer(HANDLE timer);
extern void msleep(HANDLE timer, double_t msec);
