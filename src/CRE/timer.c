/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "timer.h"

inline void timer_init(timer* t, double_t freq) {
    for (size_t i = 0; i < HISTORY_COUNT; i++)
        t->history[i] = freq;
    t->history_counter = 0;
    time_struct_init(&t->curr_time);
    time_struct_init(&t->prev_time);
    lock_init(&t->freq_lock);
    lock_init(&t->freq_hist_lock);
    lock_lock(&t->freq_lock);
    t->freq = freq;
    lock_unlock(&t->freq_lock);
    lock_lock(&t->freq_hist_lock);
    t->freq_hist = freq;
    lock_unlock(&t->freq_hist_lock);
    t->timer = timer_handle_init();
}

inline void timer_start_of_cycle(timer* t) {
    double_t time = time_struct_calc_time(&t->prev_time);
    time_struct_get_timestamp(&t->prev_time);
    double_t freq = 0;
    for (uint8_t i = 0; i < t->history_counter; i++)
        freq += t->history[i];
    freq += t->history[t->history_counter] = 1000.0 / time;
    for (uint8_t i = t->history_counter + 1; i < HISTORY_COUNT; i++)
        freq += t->history[i];
    t->history_counter++;
    if (t->history_counter >= HISTORY_COUNT)
        t->history_counter = 0;
    lock_lock(&t->freq_hist_lock);
    t->freq_hist = freq / HISTORY_COUNT;
    lock_unlock(&t->freq_hist_lock);
}

inline void timer_end_of_cycle(timer* t) {
    double_t msec = 1000.0 / timer_get_freq(t) - time_struct_calc_time(&t->prev_time);
    timer_handle_sleep(t->timer, msec);
}

inline double_t timer_get_freq(timer* t) {
    double_t freq = 0.0;
    lock_lock(&t->freq_lock);
    freq = t->freq;
    lock_unlock(&t->freq_lock);
    return freq;
}

inline void timer_set_freq(timer* t, double_t freq) {
    lock_lock(&t->freq_lock);
    t->freq = freq;
    lock_unlock(&t->freq_lock);
}

inline double_t timer_get_freq_hist(timer* t) {
    double_t freq = 0.0;
    lock_lock(&t->freq_hist_lock);
    freq = t->freq_hist;
    lock_unlock(&t->freq_hist_lock);
    return freq;
}

inline double_t timer_get_freq_ratio(timer* t) {
    double_t freq = 0.0;
    lock_lock(&t->freq_lock);
    freq = t->freq;
    lock_unlock(&t->freq_lock);
    lock_lock(&t->freq_hist_lock);
    freq /= t->freq_hist;
    lock_unlock(&t->freq_hist_lock);
    return freq;
}

inline void timer_reset(timer* t) {
    time_struct_get_timestamp(&t->curr_time);
    time_struct_get_timestamp(&t->prev_time);
}

inline void timer_sleep(timer* t, double_t msec) {
    timer_handle_sleep(t->timer, msec);
}

inline void timer_dispose(timer* t) {
    lock_free(&t->freq_lock);
    lock_free(&t->freq_hist_lock);
    timer_handle_dispose(t->timer);
}

inline HANDLE timer_handle_init() {
    return CreateWaitableTimerW(0, 0, 0);
}

inline void timer_handle_sleep(HANDLE timer, double_t msec) {
    if (msec <= 0.0)
        return;

    if (timer) {
        LARGE_INTEGER t;
        t.QuadPart = (LONGLONG)round(msec * -10000.0);
        SetWaitableTimer(timer, &t, 0, 0, 0, 0);
        WaitForSingleObject(timer, INFINITE);
    }
    else {
        DWORD msec_dw = (DWORD)round(msec);
        if (msec_dw)
            Sleep(msec_dw);
    }
}

inline void timer_handle_dispose(HANDLE timer) {
    if (timer)
        CloseHandle(timer);
}
