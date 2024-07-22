/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.hpp"

struct time_struct {
    LARGE_INTEGER timestamp;
    double_t inv_freq;

    time_struct();

    double_t calc_time();
    double_t calc_time_get_timestamp();
    int64_t calc_time_int();
    int64_t calc_time_int_get_timestamp();
    void get_timestamp();
};
