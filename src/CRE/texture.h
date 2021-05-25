/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/txp.h"
#define GLEW_STATIC
#include <GLEW/glew.h>

typedef enum texture_type {
    TEXTURE_NONE = 0,
    TEXTURE_1D,
    TEXTURE_2D,
    TEXTURE_3D,
    TEXTURE_CUBE,
} texture_type;

typedef enum texture_mode {
    TEXTURE_MODE_DEFAULT     = 0,
    TEXTURE_MODE_YCBCR_BC4   = 1,
    TEXTURE_MODE_YCBCR_BC5   = 2,
    TEXTURE_MODE_NORMAL_BC5U = 3,
    TEXTURE_MODE_NORMAL_BC5S = 4,
    TEXTURE_MODE_NORMAL_U    = 5,
    TEXTURE_MODE_NORMAL_S    = 6,
} texture_mode;

typedef struct texture_data {
    texture_type type;
    int32_t width;
    int32_t height;
    int32_t depth;
    void* data;
    GLenum pixel_type;
    GLenum pixel_format;
    GLenum pixel_internal_format;
    bool generate_mipmap;
    GLenum wrap_mode_s;
    GLenum wrap_mode_t;
    GLenum wrap_mode_r;
    GLenum min_filter;
    GLenum mag_filter;
} texture_data;

typedef struct texture_cube_sub_data {
    int32_t width;
    int32_t height;
    void* data;
    GLenum pixel_type;
    GLenum pixel_format;
} texture_cube_sub_data;

typedef struct texture_cube_data {
    texture_cube_sub_data px;
    texture_cube_sub_data nx;
    texture_cube_sub_data py;
    texture_cube_sub_data ny;
    texture_cube_sub_data pz;
    texture_cube_sub_data nz;
    GLenum pixel_internal_format;
    bool generate_mipmap;
    GLenum wrap_mode_s;
    GLenum wrap_mode_t;
    GLenum wrap_mode_r;
    GLenum min_filter;
    GLenum mag_filter;
} texture_cube_data;

typedef struct texture {
    int32_t id;
    texture_mode mode;
    texture_type type;
} texture;

typedef union texture_set_data {
    struct {
        texture_data* diffuse;
        texture_data* specular;
        texture_data* displacement;
        texture_data* normal;
        texture_data* albedo;
        texture_data* ao;
        texture_data* metallic;
        texture_data* roughness;
    };
    texture_data* tex[8];
} texture_set_data;

typedef union texture_set {
    struct {
        texture diffuse;
        texture specular;
        texture displacement;
        texture normal;
        texture albedo;
        texture ao;
        texture metallic;
        texture roughness;
    };
    texture tex[8];
} texture_set;

typedef struct texture_bone_mat_data {
    int32_t count;
    void* data;
} texture_bone_mat_data;

typedef struct texture_bone_mat {
    int32_t id;
} texture_bone_mat;

extern void texture_load(texture* tex, texture_data* data);
extern void texture_bind(texture* tex, int32_t index);
extern void texture_reset(texture* tex, int32_t index);
extern void texture_update(texture* tex);
extern void texture_free(texture* tex);
extern void texture_set_load(texture_set* tex, texture_set_data* data);
extern void texture_set_bind(texture_set* tex);
extern void texture_set_reset(texture_set* tex);
extern void texture_set_update(texture_set* tex);
extern void texture_set_free(texture_set* tex);
extern void texture_bone_mat_load(texture_bone_mat* tex, texture_bone_mat_data* data);
extern void texture_bone_mat_bind(texture_bone_mat* tex, int32_t index);
extern void texture_bone_mat_reset(texture_bone_mat* tex, int32_t index);
extern void texture_bone_mat_free(texture_bone_mat* tex);
extern bool texture_txp_load(vector_txp* tex, vector_int32_t* textures);
extern void texture_txp_unload(vector_int32_t* textures);