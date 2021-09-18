/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "time.h"

extern LARGE_INTEGER performance_frequency;
extern double_t inv_performance_frequency_msec;
extern double_t inv_performance_frequency_sec;

inline double_t time_struct_calc_time(time_struct* t) {
    LARGE_INTEGER timestamp;
    if (!QueryPerformanceCounter(&timestamp))
        return 0;

    return (double_t)(timestamp.QuadPart - t->timestamp.QuadPart)
        * inv_performance_frequency_msec;
}

inline void time_struct_get_timestamp(time_struct* t) {
    if (!QueryPerformanceCounter(&t->timestamp))
        t->timestamp.QuadPart = 0;
}
