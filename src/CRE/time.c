/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "time.h"

static double_t time_struct_get_freq();

inline void time_struct_init(time_struct* t) {
    if (!QueryPerformanceCounter(&t->timestamp))
        t->timestamp.QuadPart = 0;
    t->inv_freq = time_struct_get_freq();
}

inline double_t time_struct_calc_time(time_struct* t) {
    LARGE_INTEGER timestamp;
    if (!QueryPerformanceCounter(&timestamp))
        return 0;

    return (double_t)(timestamp.QuadPart - t->timestamp.QuadPart) * t->inv_freq;
}

inline void time_struct_get_timestamp(time_struct* t) {
    if (!QueryPerformanceCounter(&t->timestamp))
        t->timestamp.QuadPart = 0;
}

static double_t time_struct_get_freq() {
    LARGE_INTEGER freq;
    if (QueryPerformanceFrequency(&freq))
        return 1000.0 / (double_t)freq.LowPart;
    return 0.0;
}
