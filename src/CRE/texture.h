/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#define GLEW_STATIC
#include <GLEW/glew.h>

typedef enum texture_type {
    TEXTURE_NONE = 0,
    TEXTURE_1D,
    TEXTURE_2D,
    TEXTURE_3D,
    TEXTURE_CUBE,
} texture_type;

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
