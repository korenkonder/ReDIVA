/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"

extern LARGE_INTEGER performance_frequency;

#define HISTORY_COUNT 0x10

#define timer_val(name) \
double_t name##_history[HISTORY_COUNT]; \
uint8_t name##_history_counter; \
LARGE_INTEGER name##_curr_time, name##_prev_time; \
double_t name##_freq = 0.0; \
HANDLE name##_freq_lock = 0

#define timer_init(name, mutex_name) \
memset(name##_history, 0, sizeof(double_t) * HISTORY_COUNT); \
name##_history_counter = 0; \
name##_curr_time.QuadPart = 0; \
name##_prev_time.QuadPart = 0; \
name##_freq = 0; \
name##_freq_lock = CreateMutexW(0, false, L##mutex_name##L" Freq")

#define timer_calc_pre(name) \
if (name##_freq_lock) { \
    WaitForSingleObject(name##_freq_lock, INFINITE); \
    double_t time = timer_get_msec(name##_prev_time); \
    QueryPerformanceCounter(&name##_prev_time); \
    double_t freq = 0; \
    for (uint8_t i = 0; i < name##_history_counter; i++) \
        freq += name##_history[i]; \
    freq += name##_history[name##_history_counter] = 1000.0 / time; \
    for (uint8_t i = name##_history_counter + 1; i < HISTORY_COUNT; i++) \
        freq += name##_history[i]; \
    name##_freq = freq / HISTORY_COUNT; \
    name##_history_counter++; \
    if (name##_history_counter >= HISTORY_COUNT) \
        name##_history_counter = 0; \
    ReleaseMutex(name##_freq_lock); \
}

#define timer_calc_post(name) \
timer_get_msec(name##_prev_time)

#define timer_dispose(name) \
if (name##_freq_lock) \
    CloseHandle(name##_freq_lock); \
name##_freq_lock = 0

extern double_t timer_get_msec(LARGE_INTEGER t);
extern void msleep(double_t msec);
