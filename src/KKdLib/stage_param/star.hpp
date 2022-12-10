/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.hpp"
#include <string>

struct stage_param_star_utc {
    int32_t year;
    int32_t month;
    int32_t day;
    int32_t hour;
    int32_t minute;
    float_t second;

    stage_param_star_utc();
};

struct stage_param_star_modifiers {
    float_t field_0;
    float_t field_4;
    float_t field_8;
    float_t field_C;
    float_t size_max;

    stage_param_star_modifiers();
};

struct stage_param_star {
    bool ready;

    std::string milky_way_texture_name;
    stage_param_star_utc utc;
    float_t rotation_y_deg;
    float_t observer_north_latitude_deg;
    float_t observer_east_longitude_deg;
    stage_param_star_modifiers modifiers[2];

    stage_param_star();
    ~stage_param_star();

    void read(const char* path);
    void read(const wchar_t* path);
    void read(const void* data, size_t size);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* size);

    static bool load_file(void* data, const char* path, const char* file, uint32_t hash);
};
