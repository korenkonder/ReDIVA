/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "timer.h"

LARGE_INTEGER performance_frequency;

double_t timer_get_msec(LARGE_INTEGER t) {
    LARGE_INTEGER curr_time;
    QueryPerformanceCounter(&curr_time);
    return (curr_time.QuadPart - t.QuadPart) * 1000.0 / performance_frequency.QuadPart;
}

HANDLE create_timer() {
    return CreateWaitableTimerW(0, 0, 0);
}

void dispose_timer(HANDLE timer) {
    if (timer)
        CloseHandle(timer);
}

void msleep(HANDLE timer, double_t msec) {
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
