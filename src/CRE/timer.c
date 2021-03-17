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

void msleep(double_t msec) {
    if (msec <= 0.0)
        return;

    HANDLE timer = CreateWaitableTimerW(0, 0, 0);
    if (timer) {
        LARGE_INTEGER t;
        t.QuadPart = (LONGLONG)round(msec * -10000.0);
        SetWaitableTimer(timer, &t, 0, 0, 0, 0);
        WaitForSingleObject(timer, INFINITE);
        CloseHandle(timer);
    }
    else {
        DWORD msec_dw = (DWORD)round(msec);
        if (msec_dw)
            Sleep(msec_dw);
    }
}
