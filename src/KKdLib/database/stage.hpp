/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "../default.hpp"
#include "object.hpp"

enum stage_data_format {
    STAGE_DATA_UNK = 0,
    STAGE_DATA_AC,
    STAGE_DATA_F,
    STAGE_DATA_FT,
};

enum stage_data_flags {
    STAGE_DATA_FLAG_1              = 0x1,
    STAGE_DATA_LIGHT_CHARA_AMBIENT = 0x2,
    STAGE_DATA_FLAG_4              = 0x4,
    STAGE_DATA_FLAG_8              = 0x8,
};

enum stage_data_reflect_resolution_mode {
    STAGE_DATA_REFLECT_RESOLUTION_256X256 = 0x00,
    STAGE_DATA_REFLECT_RESOLUTION_512x256 = 0x01,
    STAGE_DATA_REFLECT_RESOLUTION_512x512 = 0x02,
};

enum stage_data_refract_resolution_mode {
    STAGE_DATA_REFRACT_RESOLUTION_256X256 = 0x00,
    STAGE_DATA_REFRACT_RESOLUTION_512X256 = 0x01,
    STAGE_DATA_REFRACT_RESOLUTION_512X512 = 0x02,
};

enum stage_data_reflect_type {
    STAGE_DATA_REFLECT_DISABLE     = 0x00,
    STAGE_DATA_REFLECT_NORMAL      = 0x01,
    STAGE_DATA_REFLECT_REFLECT_MAP = 0x02,
};

enum stage_data_blur_filter_mode {
    STAGE_DATA_BLUR_FILTER_4  = 0x00,
    STAGE_DATA_BLUR_FILTER_9  = 0x01,
    STAGE_DATA_BLUR_FILTER_16 = 0x02,
    STAGE_DATA_BLUR_FILTER_32 = 0x03,
};

struct stage_data_reflect {
    stage_data_reflect_resolution_mode mode;
    int32_t blur_num;
    stage_data_blur_filter_mode blur_filter;
};

struct stage_data_refract {
    stage_data_refract_resolution_mode mode;
};

struct stage_effects {
    int32_t field_0[8];
    int32_t field_20[16];
};

struct stage_data_file {
    int32_t id;
    std::string name;
    std::string auth_3d_name;
    uint32_t object_set_id;
    object_info object_ground;
    object_info object_ring;
    object_info object_sky;
    object_info object_shadow;
    object_info object_reflect;
    object_info object_refract;
    uint32_t lens_flare_texture;
    uint32_t lens_shaft_texture;
    uint32_t lens_ghost_texture;
    float_t lens_shaft_inv_scale;
    uint32_t unknown;
    uint32_t render_texture;
    uint32_t movie_texture;
    std::string collision_file_path;
    stage_data_reflect_type reflect_type;
    bool refract_enable;
    bool reflect;
    stage_data_reflect reflect_data;
    bool refract;
    stage_data_refract refract_data;
    stage_data_flags flags;
    float_t ring_rectangle_x;
    float_t ring_rectangle_y;
    float_t ring_rectangle_width;
    float_t ring_rectangle_height;
    float_t ring_height;
    float_t ring_out_height;
    stage_effects effects;
    std::vector<int32_t> auth_3d_ids;

    stage_data_file();
    ~stage_data_file();
};

struct stage_data_modern_file {
    uint32_t hash;
    std::string name;
    std::string auth_3d_name;
    object_info object_ground;
    object_info object_sky;
    object_info object_shadow;
    object_info object_reflect;
    object_info object_refract;
    float_t lens_shaft_inv_scale;
    uint32_t unknown;
    uint32_t render_texture;
    uint32_t render_texture_flag;
    uint32_t movie_texture;
    uint32_t movie_texture_flag;
    uint32_t field_04;
    uint32_t field_04_flag;
    uint32_t field_05;
    uint32_t field_05_flag;
    uint32_t field_06;
    uint32_t field_06_flag;
    uint32_t field_07;
    uint32_t field_07_flag;
    uint32_t field_08;
    uint32_t field_09;
    uint32_t field_10;
    size_t field_11;
    size_t field_12;
    float_t ring_rectangle_x;
    float_t ring_rectangle_y;
    float_t ring_rectangle_width;
    float_t ring_rectangle_height;
    float_t ring_height;
    float_t ring_out_height;
    uint32_t field_13;
    stage_effects effects;
    std::vector<uint32_t> auth_3d_ids;

    stage_data_modern_file();
    ~stage_data_modern_file();
};

struct stage_data {
    int32_t id;
    std::string name;
    uint32_t name_hash;
    std::string auth_3d_name;
    uint32_t object_set_id;
    object_info object_ground;
    object_info object_ring;
    object_info object_sky;
    object_info object_shadow;
    object_info object_reflect;
    object_info object_refract;
    uint32_t lens_flare_texture;
    uint32_t lens_shaft_texture;
    uint32_t lens_ghost_texture;
    float_t lens_shaft_inv_scale;
    uint32_t unknown;
    uint32_t render_texture;
    uint32_t movie_texture;
    std::string collision_file_path;
    stage_data_reflect_type reflect_type;
    bool refract_enable;
    bool reflect;
    stage_data_reflect reflect_data;
    bool refract;
    stage_data_refract refract_data;
    stage_data_flags flags;
    float_t ring_rectangle_x;
    float_t ring_rectangle_y;
    float_t ring_rectangle_width;
    float_t ring_rectangle_height;
    float_t ring_height;
    float_t ring_out_height;
    stage_effects effects;
    std::vector<int32_t> auth_3d_ids;

    stage_data();
    ~stage_data();
};

struct stage_data_modern {
    uint32_t hash;
    std::string name;
    std::string auth_3d_name;
    uint32_t auth_3d_name_hash;
    object_info object_ground;
    object_info object_sky;
    object_info object_shadow;
    object_info object_reflect;
    object_info object_refract;
    float_t lens_shaft_inv_scale;
    uint32_t unknown;
    uint32_t render_texture;
    uint32_t render_texture_flag;
    uint32_t movie_texture;
    uint32_t movie_texture_flag;
    uint32_t field_04;
    uint32_t field_04_flag;
    uint32_t field_05;
    uint32_t field_05_flag;
    uint32_t field_06;
    uint32_t field_06_flag;
    uint32_t field_07;
    uint32_t field_07_flag;
    uint32_t field_08;
    uint32_t field_09;
    uint32_t field_10;
    size_t field_11;
    size_t field_12;
    float_t ring_rectangle_x;
    float_t ring_rectangle_y;
    float_t ring_rectangle_width;
    float_t ring_rectangle_height;
    float_t ring_height;
    float_t ring_out_height;
    uint32_t field_13;
    stage_effects effects;
    std::vector<uint32_t> auth_3d_ids;

    stage_data_modern();
    ~stage_data_modern();
};

struct stage_database_file {
    bool ready;
    bool modern;
    bool is_x;
    stage_data_format format;

    std::vector<::stage_data_file> stage_data;
    std::vector<stage_data_modern_file> stage_modern;

    stage_database_file();
    virtual ~stage_database_file();

    void read(const char* path, bool modern);
    void read(const wchar_t* path, bool modern);
    void read(const void* data, size_t size, bool modern);
    void write(const char* path);
    void write(const wchar_t* path);
    void write(void** data, size_t* size);

    static bool load_file(void* data, const char* path, const char* file, uint32_t hash);
};

struct stage_database {
    std::vector<::stage_data> stage_data;
    std::vector<stage_data_modern> stage_modern;

    stage_database();
    virtual ~stage_database();

    void add(stage_database_file* stage_data_file);

    ::stage_data* get_stage_data(int32_t stage_index);
    ::stage_data_modern* get_stage_data_modern(uint32_t stage_hash);
    int32_t get_stage_index(const char* name);
};
