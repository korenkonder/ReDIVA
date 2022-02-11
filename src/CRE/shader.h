/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/string.h"
#include "../KKdLib/farc.h"
#include "../KKdLib/mat.h"
#include "../KKdLib/vec.h"
#include "../KKdLib/vector.h"
#include "static_var.h"

#define SHADER_MAX_CLIP_PLANES 8
#define SHADER_MAX_LIGHTS 8
#define SHADER_MAX_PALETTE_MATRICES 32
#define SHADER_MAX_PROGRAM_ENV_PARAMETERS 256
#define SHADER_MAX_PROGRAM_LOCAL_PARAMETERS 256
#define SHADER_MAX_PROGRAM_BUFFER_PARAMETERS 1024
#define SHADER_MAX_PROGRAM_PARAMETER_BUFFER_SIZE 16384
#define SHADER_MAX_PROGRAM_MATRICES 8
#define SHADER_MAX_TEXTURE_COORDS 8
#define SHADER_MAX_TEXTURE_IMAGE_UNITS 32
#define SHADER_MAX_TEXTURE_UNITS 4
#define SHADER_MAX_UNIFORM_BLOCK_SIZE 65536
#define SHADER_MAX_VERTEX_UNITS 4

typedef struct shader_state_clip {
    vec4 plane;
} shader_state_clip;

typedef struct shader_state_depth {
    vec4 range;
} shader_state_depth;

typedef struct shader_state_fog {
    vec4 color;
    vec4 params;
} shader_state_fog;

typedef struct shader_state_light {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 position;
    vec4 attenuation;
    vec4 spot_direction;
    vec4 half;
} shader_state_light;

typedef struct shader_state_lightmodel {
    vec4 ambient;
    vec4 scene_color;
} shader_state_lightmodel;

typedef struct shader_state_lightprod {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
} shader_state_lightprod;

typedef struct shader_state_material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 emission;
    vec4 shininess;
} shader_state_material;

typedef struct shader_state_matrix_data {
    mat4 mat;
    mat4 inv;
    mat4 trans;
    mat4 invtrans;
} shader_state_matrix_data;

typedef struct shader_state_matrix {
    shader_state_matrix_data modelview[SHADER_MAX_VERTEX_UNITS];
    shader_state_matrix_data projection;
    shader_state_matrix_data mvp;
    shader_state_matrix_data texture[SHADER_MAX_TEXTURE_COORDS];
    shader_state_matrix_data palette[SHADER_MAX_PALETTE_MATRICES];
    shader_state_matrix_data program[SHADER_MAX_PROGRAM_MATRICES];
} shader_state_matrix;

typedef struct shader_state_point {
    vec4 size;
    vec4 attenuation;
} shader_state_point;

typedef struct shader_state_texgen {
    union {
        vec4 eye[4];
        struct {
            vec4 eye_s;
            vec4 eye_t;
            vec4 eye_r;
            vec4 eye_q;
        };
    };

    union {
        vec4 object[4];
        struct {
            vec4 object_s;
            vec4 object_t;
            vec4 object_r;
            vec4 object_q;
        };
    };
} shader_state_texgen;

typedef struct shader_state_texenv {
    vec4 color;
} shader_state_texenv;

typedef struct shader_buffer {
    vec4 buffer[SHADER_MAX_PROGRAM_PARAMETER_BUFFER_SIZE / sizeof(vec4)];
} shader_buffer;

typedef struct shader_env {
    vec4 frag[SHADER_MAX_PROGRAM_ENV_PARAMETERS];
    vec4 vert[SHADER_MAX_PROGRAM_ENV_PARAMETERS];
} shader_env;

typedef struct shader_state {
    shader_state_material material[2];
    shader_state_light light[SHADER_MAX_LIGHTS];
    shader_state_lightmodel lightmodel[2];
    shader_state_lightprod lightprod_front[SHADER_MAX_LIGHTS];
    shader_state_lightprod lightprod_back[SHADER_MAX_LIGHTS];
    shader_state_texgen texgen[SHADER_MAX_TEXTURE_UNITS];
    shader_state_texenv texenv[SHADER_MAX_TEXTURE_UNITS];
    shader_state_fog fog;
    shader_state_clip clip[SHADER_MAX_CLIP_PLANES];
    shader_state_point point;
    shader_state_depth depth;
} shader_state;

typedef struct shader_data {
    shader_state state;
    shader_state_matrix state_matrix;
    shader_env env;
    shader_buffer buffer;
    bool state_update_data;
    bool state_matrix_update_data;
    bool env_update_data;
    bool buffer_update_data;
    GLuint state_ubo;
    GLuint state_matrix_ubo;
    GLuint env_ubo;
    GLuint buffer_ubo;
} shader_data;

typedef struct shader shader;

typedef struct shader_set_data {
    shader* shaders;
    size_t size;
    shader_data data;
} shader_set_data;

typedef void (* PFNSHADERPERMUTBINDFUNCPROC)(shader_set_data* set, shader* shader);

typedef struct shader_bind_func {
    uint32_t index;
    PFNSHADERPERMUTBINDFUNCPROC bind_func;
} shader_bind_func;

typedef struct shader_sub {
    uint32_t sub_index;
    const int32_t* vp_unival_max;
    const int32_t* fp_unival_max;
    GLuint* program;
} shader_sub;

typedef struct shader_sub_table {
    uint32_t sub_index;
    const int32_t* vp_unival_max;
    const int32_t* fp_unival_max;
    const char* vp;
    const char* fp;
} shader_sub_table;

typedef struct shader_table {
    const char* name;
    uint32_t index;
    int32_t num_sub;
    const shader_sub_table* sub;
    int32_t num_uniform;
    const uniform_name* use_uniform;
    const bool* use_permut;
} shader_table;

struct shader {
    const char* name;
    uint32_t index;
    int32_t num_sub;
    shader_sub* sub;
    int32_t num_uniform;
    const uniform_name* use_uniform;
    const bool* use_permut;
    PFNSHADERPERMUTBINDFUNCPROC bind_func;
};

extern int32_t shader_bind(shader* shader, uint32_t sub_index);
extern void shader_draw_arrays(shader_set_data* set,
    GLenum mode, GLint first, GLsizei count);
extern void shader_draw_elements(shader_set_data* set,
    GLenum mode, GLsizei count, GLenum type, const void* indices);
extern void shader_draw_range_elements(shader_set_data* set,
    GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void* indices);
extern int32_t shader_get_index_by_name(shader_set_data* set, char* name);
extern int32_t shader_get_index_by_name(shader_set_data* set, const char* name);
extern void shader_load(shader_set_data* set, farc* f, bool ignore_cache, bool not_load_cache,
    char* name, const shader_table** shaders_table, const size_t size,
    const shader_bind_func* bind_func_table, const size_t bind_func_table_size);
extern void shader_load(shader_set_data* set, farc* f, bool ignore_cache, bool not_load_cache,
    const char* name, const shader_table** shaders_table, const size_t size,
    const shader_bind_func* bind_func_table, const size_t bind_func_table_size);
extern void shader_free(shader_set_data* set);
extern void shader_set(shader_set_data* set, uint32_t index);
extern void shader_unbind();

extern void shader_buffer_get_ptr(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_buffer_get_ptr_array(shader_set_data* set,
    size_t index, size_t count, vec4* data);
extern void shader_local_frag_get(shader_set_data* set,
    size_t index, float_t* x, float_t* y, float_t* z, float_t* w);
extern void shader_local_frag_get_ptr(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_local_vert_get(shader_set_data* set,
    size_t index, float_t* x, float_t* y, float_t* z, float_t* w);
extern void shader_local_vert_get_ptr(shader_set_data* set,
    size_t index, vec4* data);
extern void  shader_env_frag_get(shader_set_data* set,
    size_t index, float_t* x, float_t* y, float_t* z, float_t* w);
extern void  shader_env_frag_get_ptr(shader_set_data* set,
    size_t index, vec4* data);
extern void  shader_env_frag_get_ptr_array(shader_set_data* set,
    size_t index, size_t count, vec4* data);
extern void  shader_env_vert_get(shader_set_data* set,
    size_t index, float_t* x, float_t* y, float_t* z, float_t* w);
extern void  shader_env_vert_get_ptr(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_env_vert_get_ptr_array(shader_set_data* set,
    size_t index, size_t count, vec4* data);
extern void shader_state_clip_get_plane(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_state_depth_get_range(shader_set_data* set,
    vec4* data);
extern void shader_state_fog_get_color(shader_set_data* set,
    vec4* data);
extern void shader_state_fog_get_params(shader_set_data* set,
    vec4* data);
extern void shader_state_light_get_ambient(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_state_light_get_diffuse(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_state_light_get_specular(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_state_light_get_position(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_state_light_get_attenuation(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_state_light_get_spot_direction(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_state_light_get_half(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_state_lightmodel_get_ambient(shader_set_data* set,
    bool back, vec4* data);
extern void shader_state_lightmodel_get_scene_color(shader_set_data* set,
    bool back, vec4* data);
extern void shader_state_lightprod_get_ambient(shader_set_data* set,
    bool back, size_t index, vec4* data);
extern void shader_state_lightprod_get_diffuse(shader_set_data* set,
    bool back, size_t index, vec4* data);
extern void shader_state_lightprod_get_specular(shader_set_data* set,
    bool back, size_t index, vec4* data);
extern void shader_state_material_get_ambient(shader_set_data* set,
    bool back, vec4* data);
extern void shader_state_material_get_diffuse(shader_set_data* set,
    bool back, vec4* data);
extern void shader_state_material_get_specular(shader_set_data* set,
    bool back, vec4* data);
extern void shader_state_material_get_emission(shader_set_data* set,
    bool back, vec4* data);
extern void shader_state_material_get_shininess(shader_set_data* set,
    bool back, vec4* data);
extern void shader_state_matrix_get_modelview(shader_set_data* set,
    size_t index, mat4* data);
extern void shader_state_matrix_get_projection(shader_set_data* set,
    mat4* data);
extern void shader_state_matrix_get_mvp(shader_set_data* set,
    mat4* data);
extern void shader_state_matrix_get_texture(shader_set_data* set,
    size_t index, mat4* data);
extern void shader_state_matrix_get_palette(shader_set_data* set,
    size_t index, mat4* data);
extern void shader_state_matrix_get_program(shader_set_data* set,
    size_t index, mat4* data);
extern void shader_state_point_get_size(shader_set_data* set,
    vec4* data);
extern void shader_state_point_get_attenuation(shader_set_data* set,
    vec4* data);
extern void shader_state_texgen_get_eye_s(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_state_texgen_get_eye_t(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_state_texgen_get_eye_r(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_state_texgen_get_eye_q(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_state_texgen_get_object_s(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_state_texgen_get_object_t(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_state_texgen_get_object_r(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_state_texgen_get_object_q(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_state_texenv_get_color(shader_set_data* set,
    size_t index, vec4* data);

extern void shader_buffer_set_ptr(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_buffer_set_ptr_array(shader_set_data* set,
    size_t index, size_t count, vec4* data);
extern void shader_local_frag_set(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w);
extern void shader_local_frag_set_ptr(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_local_frag_set_ptr_array(shader_set_data* set,
    size_t index, size_t count, vec4* data);
extern void shader_local_vert_set(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w);
extern void shader_local_vert_set_ptr(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_local_vert_set_ptr_array(shader_set_data* set,
    size_t index, size_t count, vec4* data);
extern void shader_env_frag_set(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w);
extern void shader_env_frag_set_ptr(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_env_frag_set_ptr_array(shader_set_data* set,
    size_t index, size_t count, vec4* data);
extern void shader_env_vert_set(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w);
extern void shader_env_vert_set_ptr(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_env_vert_set_ptr_array(shader_set_data* set,
    size_t index, size_t count, vec4* data);
extern void shader_state_clip_set_plane(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w);
extern void shader_state_clip_set_plane_ptr(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_state_depth_set_range(shader_set_data* set,
    float_t x, float_t y, float_t z, float_t w);
extern void shader_state_depth_set_range_ptr(shader_set_data* set,
    vec4* data);
extern void shader_state_fog_set_color(shader_set_data* set,
    float_t x, float_t y, float_t z, float_t w);
extern void shader_state_fog_set_color_ptr(shader_set_data* set,
    vec4* data);
extern void shader_state_fog_set_params(shader_set_data* set,
    float_t x, float_t y, float_t z, float_t w);
extern void shader_state_fog_set_params_ptr(shader_set_data* set,
    vec4* data);
extern void shader_state_light_set_ambient(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w);
extern void shader_state_light_set_ambient_ptr(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_state_light_set_diffuse(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w);
extern void shader_state_light_set_diffuse_ptr(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_state_light_set_specular(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w);
extern void shader_state_light_set_specular_ptr(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_state_light_set_position(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w);
extern void shader_state_light_set_position_ptr(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_state_light_set_attenuation(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w);
extern void shader_state_light_set_attenuation_ptr(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_state_light_set_spot_direction(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w);
extern void shader_state_light_set_spot_direction_ptr(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_state_light_set_half(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w);
extern void shader_state_light_set_half_ptr(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_state_lightmodel_set_ambient(shader_set_data* set,
    bool back, float_t x, float_t y, float_t z, float_t w);
extern void shader_state_lightmodel_set_ambient_ptr(shader_set_data* set,
    bool back, vec4* data);
extern void shader_state_lightmodel_set_scene_color(shader_set_data* set,
    bool back, float_t x, float_t y, float_t z, float_t w);
extern void shader_state_lightmodel_set_scene_color_ptr(shader_set_data* set,
    bool back, vec4* data);
extern void shader_state_lightprod_set_ambient(shader_set_data* set,
    bool back, size_t index, float_t x, float_t y, float_t z, float_t w);
extern void shader_state_lightprod_set_ambient_ptr(shader_set_data* set,
    bool back, size_t index, vec4* data);
extern void shader_state_lightprod_set_diffuse(shader_set_data* set,
    bool back, size_t index, float_t x, float_t y, float_t z, float_t w);
extern void shader_state_lightprod_set_diffuse_ptr(shader_set_data* set,
    bool back, size_t index, vec4* data);
extern void shader_state_lightprod_set_specular(shader_set_data* set,
    bool back, size_t index, float_t x, float_t y, float_t z, float_t w);
extern void shader_state_lightprod_set_specular_ptr(shader_set_data* set,
    bool back, size_t index, vec4* data);
extern void shader_state_material_set_ambient(shader_set_data* set,
    bool back, float_t x, float_t y, float_t z, float_t w);
extern void shader_state_material_set_ambient_ptr(shader_set_data* set,
    bool back, vec4* data);
extern void shader_state_material_set_diffuse(shader_set_data* set,
    bool back, float_t x, float_t y, float_t z, float_t w);
extern void shader_state_material_set_diffuse_ptr(shader_set_data* set,
    bool back, vec4* data);
extern void shader_state_material_set_specular(shader_set_data* set,
    bool back, float_t x, float_t y, float_t z, float_t w);
extern void shader_state_material_set_specular_ptr(shader_set_data* set,
    bool back, vec4* data);
extern void shader_state_material_set_emission(shader_set_data* set,
    bool back, float_t x, float_t y, float_t z, float_t w);
extern void shader_state_material_set_emission_ptr(shader_set_data* set,
    bool back, vec4* data);
extern void shader_state_material_set_shininess(shader_set_data* set,
    bool back, float_t x, float_t y, float_t z, float_t w);
extern void shader_state_material_set_shininess_ptr(shader_set_data* set,
    bool back, vec4* data);
extern void shader_state_matrix_set_modelview(shader_set_data* set,
    size_t index, mat4* data, bool mult);
extern void shader_state_matrix_set_projection(shader_set_data* set,
    mat4* data, bool mult);
extern void shader_state_matrix_set_mvp(shader_set_data* set,
    mat4* data);
extern void shader_state_matrix_set_modelview_separate(shader_set_data* set,
    size_t index, mat4* model, mat4* view, bool mult);
extern void shader_state_matrix_set_mvp_separate(shader_set_data* set,
    mat4* model, mat4* view, mat4* projection);
extern void shader_state_matrix_set_texture(shader_set_data* set,
    size_t index, mat4* data);
extern void shader_state_matrix_set_palette(shader_set_data* set,
    size_t index, mat4* data);
extern void shader_state_matrix_set_program(shader_set_data* set,
    size_t index, mat4* data);
extern void shader_state_point_set_size(shader_set_data* set,
    float_t x, float_t y, float_t z, float_t w);
extern void shader_state_point_set_size_ptr(shader_set_data* set,
    vec4* data);
extern void shader_state_point_set_attenuation(shader_set_data* set,
    float_t x, float_t y, float_t z, float_t w);
extern void shader_state_point_set_attenuation_ptr(shader_set_data* set,
    vec4* data);
extern void shader_state_texgen_set_eye_s(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w);
extern void shader_state_texgen_set_eye_s_ptr(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_state_texgen_set_eye_t(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w);
extern void shader_state_texgen_set_eye_t_ptr(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_state_texgen_set_eye_r(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w);
extern void shader_state_texgen_set_eye_r_ptr(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_state_texgen_set_eye_q(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w);
extern void shader_state_texgen_set_eye_q_ptr(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_state_texgen_set_object_s(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w);
extern void shader_state_texgen_set_object_s_ptr(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_state_texgen_set_object_t(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w);
extern void shader_state_texgen_set_object_t_ptr(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_state_texgen_set_object_r(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w);
extern void shader_state_texgen_set_object_r_ptr(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_state_texgen_set_object_q(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w);
extern void shader_state_texgen_set_object_q_ptr(shader_set_data* set,
    size_t index, vec4* data);
extern void shader_state_texenv_set_color(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w);
extern void shader_state_texenv_set_color_ptr(shader_set_data* set,
    size_t index, vec4* data);