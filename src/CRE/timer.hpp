/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "lock.hpp"
#include "time.hpp"

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
    HANDLE timer_handle;

    timer(double_t freq);
    virtual ~timer();

    void start_of_cycle();
    void end_of_cycle();
    double_t get_freq();
    void set_freq(double_t freq);
    double_t get_freq_hist();
    double_t get_freq_ratio();
    void reset();
    void sleep(double_t msec);
};

extern LARGE_INTEGER performance_frequency;

extern HANDLE timer_handle_init();
extern void timer_handle_sleep(HANDLE timer, double_t msec);
extern void timer_handle_dispose(HANDLE timer);
