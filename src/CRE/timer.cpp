/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "timer.hpp"

timer::timer(double_t freq) : history() {
    for (double_t& i : history)
        i = freq;
    history_counter = 0;
    this->freq = freq;
    this->freq_hist = freq;
    lock_init(&freq_lock);
    lock_init(&freq_hist_lock);
    timer_handle = timer_handle_init();
}

timer::~timer() {
    lock_free(&freq_lock);
    lock_free(&freq_hist_lock);
    timer_handle_dispose(timer_handle);
}

void timer::start_of_cycle() {
    double_t time = prev_time.calc_time();
    prev_time.get_timestamp();
    double_t freq = 0;
    for (uint8_t i = 0; i < history_counter; i++)
        freq += history[i];
    freq += history[history_counter] = 1000.0 / time;
    for (uint8_t i = history_counter + 1; i < HISTORY_COUNT; i++)
        freq += history[i];
    history_counter++;
    if (history_counter >= HISTORY_COUNT)
        history_counter = 0;
    lock_lock(&freq_hist_lock);
    freq_hist = freq / HISTORY_COUNT;
    lock_unlock(&freq_hist_lock);
}

void timer::end_of_cycle() {
    double_t msec = 1000.0 / get_freq() - prev_time.calc_time();
    timer_handle_sleep(timer_handle, msec);
}

double_t timer::get_freq() {
    double_t freq = 0.0;
    lock_lock(&freq_lock);
    freq = this->freq;
    lock_unlock(&freq_lock);
    return freq;
}

void timer::set_freq(double_t freq) {
    lock_lock(&freq_lock);
    this->freq = freq;
    lock_unlock(&freq_lock);
}

double_t timer::get_freq_hist() {
    double_t freq = 0.0;
    lock_lock(&freq_hist_lock);
    freq = freq_hist;
    lock_unlock(&freq_hist_lock);
    return freq;
}

double_t timer::get_freq_ratio() {
    double_t freq = 0.0;
    lock_lock(&freq_lock);
    freq = this->freq;
    lock_unlock(&freq_lock);
    lock_lock(&freq_hist_lock);
    freq /= freq_hist;
    lock_unlock(&freq_hist_lock);
    return freq;
}

void timer::reset() {
    curr_time.get_timestamp();
    prev_time.get_timestamp();
}

void timer::sleep(double_t msec) {
    timer_handle_sleep(timer_handle, msec);
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
