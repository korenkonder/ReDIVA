/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "timer.h"

LARGE_INTEGER performance_frequency;
double_t inv_performance_frequency_msec;
double_t inv_performance_frequency_sec;

inline void timer_init(timer* t, double_t freq) {
    memset(t->history, 0, sizeof(double_t) * HISTORY_COUNT);
    t->history_counter = 0;
    t->curr_time.QuadPart = 0;
    t->prev_time.QuadPart = 0;
    t->freq = freq;
    t->freq_hist = 0.0;
    lock_init(&t->freq_lock);
    lock_init(&t->freq_hist_lock);
    t->timer = create_timer();
}

inline void timer_start_of_cycle(timer* t) {
    double_t time = timer_get_msec(t->prev_time);
    QueryPerformanceCounter(&t->prev_time);
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
    double_t msec = 1000.0 / timer_get_freq(t) - timer_get_msec(t->prev_time);
    msleep(t->timer, msec);
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

inline void timer_dispose(timer* t) {
    lock_free(&t->freq_lock);
    lock_free(&t->freq_hist_lock);
    dispose_timer(t->timer);
}

inline double_t timer_get_msec(LARGE_INTEGER t) {
    LARGE_INTEGER curr_time;
    QueryPerformanceCounter(&curr_time);
    return (curr_time.QuadPart - t.QuadPart) * inv_performance_frequency_msec;
}

inline HANDLE create_timer() {
    return CreateWaitableTimerW(0, 0, 0);
}

inline void dispose_timer(HANDLE timer) {
    if (timer)
        CloseHandle(timer);
}

inline void msleep(HANDLE timer, double_t msec) {
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
