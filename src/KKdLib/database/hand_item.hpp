/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <list>
#include <map>
#include <string>
#include <vector>
#include "../default.hpp"
#include "../image.hpp"
#include "../vec.hpp"

struct hnd_itm_data {
    std::string objname_left;
    std::string objname_right;
    std::string item_str;
    std::string item_name;
    uint32_t file_size;
    std::string hand_motion;
    float_t hand_scale;
    int32_t uid;

    hnd_itm_data();
    ~hnd_itm_data();
};

struct hnd_itm {
    bool ready;

    std::vector<hnd_itm_data> data;

    hnd_itm();
    ~hnd_itm();

    void read(const char* path);
    void read(const wchar_t* path);
    void read(const void* data, size_t size);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* size);

    static bool load_file(void* data, const char* dir, const char* file, uint32_t hash);
};
