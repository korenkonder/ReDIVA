/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.h"
#include "../string.h"
#include "../vector.h"
#include "object.h"

typedef enum stage_data_format {
    STAGE_DATA_UNK = 0,
    STAGE_DATA_AC,
    STAGE_DATA_F,
    STAGE_DATA_FT,
} stage_data_format;

typedef enum stage_data_flags {
    STAGE_DATA_FLAG_1              = 0x1,
    STAGE_DATA_LIGHT_CHARA_AMBIENT = 0x2,
    STAGE_DATA_FLAG_4              = 0x4,
    STAGE_DATA_FLAG_8              = 0x8,
} stage_data_flags;

typedef enum stage_data_reflect_resolution_mode {
    STAGE_DATA_REFLECT_RESOLUTION_256X256 = 0x00,
    STAGE_DATA_REFLECT_RESOLUTION_512x256 = 0x01,
    STAGE_DATA_REFLECT_RESOLUTION_512x512 = 0x02,
} stage_data_reflect_resolution_mode;

typedef enum stage_data_refract_resolution_mode {
    STAGE_DATA_REFRACT_RESOLUTION_256X256 = 0x00,
    STAGE_DATA_REFRACT_RESOLUTION_512X256 = 0x01,
    STAGE_DATA_REFRACT_RESOLUTION_512X512 = 0x02,
} stage_data_refract_resolution_mode;

typedef enum stage_data_reflect_type {
    STAGE_DATA_REFLECT_DISABLE     = 0x00,
    STAGE_DATA_REFLECT_NORMAL      = 0x01,
    STAGE_DATA_REFLECT_REFLECT_MAP = 0x02,
} stage_data_reflect_type;

typedef enum stage_data_blur_filter_mode {
    STAGE_DATA_BLUR_FILTER_4  = 0x00,
    STAGE_DATA_BLUR_FILTER_9  = 0x01,
    STAGE_DATA_BLUR_FILTER_16 = 0x02,
    STAGE_DATA_BLUR_FILTER_32 = 0x03,
} stage_data_blur_filter_mode;

typedef struct stage_data_reflect {
    stage_data_reflect_resolution_mode mode;
    int32_t blur_num;
    stage_data_blur_filter_mode blur_filter;
} stage_data_reflect;

typedef struct stage_data_refract {
    stage_data_refract_resolution_mode mode;
} stage_data_refract;

typedef struct stage_effects {
    int32_t field_0[8];
    int32_t parent_bone_node[16];
} stage_effects;

typedef struct stage_data {
    int32_t id;
    string name;
    string auth_3d_name;
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
    string collision_file_path;
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
    int32_t auth_3d_count;
    int32_t* auth_3d_ids;
} stage_data;

typedef struct stage_data_modern {
    uint64_t id;
    string name;
    string auth_3d_name;
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
    int32_t auth_3d_count;
    int32_t* auth_3d_ids;
} stage_data_modern;

vector_old(stage_data)
vector_old(stage_data_modern)

typedef struct stage_database {
    bool ready;
    bool modern;
    union {
        bool is_x;
        stage_data_format format;
    };

    union {
        vector_old_stage_data stage_data;
        vector_old_stage_data_modern stage_modern;
    };
} stage_database;

extern void stage_database_init(stage_database* stage_data);
extern void stage_database_read(stage_database* stage_data, const char* path, bool modern);
extern void stage_database_read(stage_database* stage_data, const wchar_t* path, bool modern);
extern void stage_database_read(stage_database* stage_data, const void* data, size_t length, bool modern);
extern void stage_database_write(stage_database* stage_data, const char* path);
extern void stage_database_write(stage_database* stage_data, const wchar_t* path);
extern void stage_database_write(stage_database* stage_data, void** data, size_t* length);
extern bool stage_database_load_file(void* data, const char* path, const char* file, uint32_t hash);
extern void stage_database_merge_mdata(stage_database* stage_data,
    stage_database* base_stage_data, stage_database* mdata_stage_data);
extern void stage_database_split_mdata(stage_database* stage_data,
    stage_database* base_stage_data, stage_database* mdata_stage_data);
extern void stage_database_free(stage_database* stage_data);

extern void stage_data_free(stage_data* data);
extern void stage_data_modern_free(stage_data_modern* data);
