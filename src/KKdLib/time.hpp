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
    void get_timestamp();
};
