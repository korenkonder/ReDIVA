/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.hpp"
#include "waitable_timer.hpp"
#include <atomic>

#define HISTORY_COUNT 0x08

struct timer {
    double_t history[HISTORY_COUNT];
    uint8_t history_counter;
    LARGE_INTEGER curr_time;
    LARGE_INTEGER prev_time;
    double_t inv_freq;
    std::atomic<double_t> freq;
    std::atomic<double_t> freq_hist;
    waitable_timer wait_timer;

    timer(double_t freq);
    ~timer();

    void start_of_cycle();
    void end_of_cycle();
    double_t get_freq();
    void set_freq(double_t value);
    double_t get_freq_hist();
    void reset();
    void sleep(double_t msec);
};
