/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "../KKdLib/default.hpp"
#include "../KKdLib/farc.hpp"
#include "../KKdLib/mat.hpp"
#include "../KKdLib/vec.hpp"
#include "static_var.hpp"

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
#define SHADER_MAX_UNIFORM_VALUES 16
#define SHADER_MAX_VERTEX_UNITS 4

struct shader_state_clip {
    vec4 plane;

    shader_state_clip();
};

struct shader_state_depth {
    vec4 range;

    shader_state_depth();
};

struct shader_state_fog {
    vec4 color;
    vec4 params;

    shader_state_fog();
};

struct shader_state_light {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 position;
    vec4 attenuation;
    vec4 spot_direction;
    vec4 half;

    shader_state_light();
};

struct shader_state_lightmodel {
    vec4 ambient;
    vec4 scene_color;
    shader_state_lightmodel();

};

struct shader_state_lightprod {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    shader_state_lightprod();
};

struct shader_state_material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 emission;
    vec4 shininess;

    shader_state_material();
};

struct shader_state_matrix_data {
    mat4 mat;
    mat4 inv;
    mat4 trans;
    mat4 invtrans;

    shader_state_matrix_data();
};

struct shader_state_matrix {
    shader_state_matrix_data modelview[SHADER_MAX_VERTEX_UNITS];
    shader_state_matrix_data projection;
    shader_state_matrix_data mvp;
    shader_state_matrix_data texture[SHADER_MAX_TEXTURE_COORDS];
    shader_state_matrix_data palette[SHADER_MAX_PALETTE_MATRICES];
    shader_state_matrix_data program[SHADER_MAX_PROGRAM_MATRICES];

    shader_state_matrix();
};

struct shader_state_point {
    vec4 size;
    vec4 attenuation;

    shader_state_point();
};

struct shader_state_texgen {
    struct eye {
        vec4 s;
        vec4 t;
        vec4 r;
        vec4 q;

        eye();
    } eye;

    struct object {
        vec4 s;
        vec4 t;
        vec4 r;
        vec4 q;

        object();
    } object;

    shader_state_texgen();
};

struct shader_state_texenv {
    vec4 color;

    shader_state_texenv();
};

struct shader_buffer {
    vec4 buffer[SHADER_MAX_PROGRAM_PARAMETER_BUFFER_SIZE / sizeof(vec4)];

    shader_buffer();
};

struct shader_env {
    vec4 frag[SHADER_MAX_PROGRAM_ENV_PARAMETERS];
    vec4 vert[SHADER_MAX_PROGRAM_ENV_PARAMETERS];

    shader_env();
};

struct shader_state {
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

    shader_state();
};

struct shader_local;
struct shader_local_uniform;

struct shader_data {
    shader_state state;
    shader_state_matrix state_matrix;
    shader_env env;
    shader_buffer buffer;
    shader_local* local;
    shader_local_uniform* local_uniform;
    bool state_update_data;
    bool state_matrix_update_data;
    bool env_update_data;
    bool buffer_update_data;
    bool* local_update_data;
    bool* local_uniform_update_data;
    GLuint state_ubo;
    GLuint state_matrix_ubo;
    GLuint env_ubo;
    GLuint buffer_ubo;

    shader_data();
};

struct shader;
struct shader_set_data;

typedef void (*PFNSHADERBINDFUNCPROC)(shader_set_data* set, shader* shad);

struct shader_bind_func {
    uint32_t name_index;
    PFNSHADERBINDFUNCPROC bind_func;
};

struct shader_local {
    vec4 vert[SHADER_MAX_PROGRAM_LOCAL_PARAMETERS];
    vec4 frag[SHADER_MAX_PROGRAM_LOCAL_PARAMETERS];
};

struct shader_local_uniform {
    int32_t uniform_val[SHADER_MAX_UNIFORM_VALUES];
};

struct shader_sub_shader {
    GLuint program;
    shader_local data;
    shader_local_uniform data_uniform;
    bool data_update[2];
    bool data_uniform_update;
};

struct shader_sub {
    uint32_t sub_index;
    const int32_t* vp_unival_max;
    const int32_t* fp_unival_max;
    shader_sub_shader* shaders;
};

struct shader_sub_table {
    uint32_t sub_index;
    const int32_t* vp_unival_max;
    const int32_t* fp_unival_max;
    const char* vp;
    const char* fp;
};

struct shader_table {
    const char* name;
    uint32_t name_index;
    int32_t num_sub;
    const shader_sub_table* sub;
    int32_t num_uniform;
    const uniform_name* use_uniform;
    const bool* use_permut;
};

struct shader {
    const char* name;
    uint32_t name_index;
    int32_t num_sub;
    shader_sub* sub;
    int32_t num_uniform;
    const uniform_name* use_uniform;
    const bool* use_permut;
    PFNSHADERBINDFUNCPROC bind_func;

    int32_t bind(shader_set_data* set, uint32_t sub_index);

    static bool parse_define(const char* data, int32_t num_uniform,
        const int32_t* vp_unival_max, const int32_t* fp_unival_max,
        int32_t* uniform_value, char** temp, size_t* temp_size);
    static char* parse_include(char* data, farc* f);
    static void unbind();
};

struct shader_set_data {
    shader* shaders;
    size_t size;
    shader_data data;
    GLboolean primitive_restart;
    GLuint primitive_restart_index;

    shader_set_data();

    void disable_primitive_restart();
    void draw_arrays(GLenum mode, GLint first, GLsizei count);
    void draw_elements(GLenum mode,
        GLsizei count, GLenum type, const void* indices);
    void draw_range_elements(GLenum mode,
        GLuint start, GLuint end, GLsizei count, GLenum type, const void* indices);
    void enable_primitive_restart();
    int32_t get_index_by_name(const char* name);
    void load(farc* f, bool ignore_cache, const char* name,
        const shader_table* shaders_table, const size_t size,
        const shader_bind_func* bind_func_table, const size_t bind_func_table_size);
    void set(uint32_t index);
    void set_primitive_restart_index(GLuint index);
    void unload();

    void buffer_get(size_t index, vec4& data);
    void buffer_get(size_t index, size_t count, vec4* data);
    void local_frag_get(size_t index, float_t* x, float_t* y, float_t* z, float_t* w);
    void local_frag_get(size_t index, vec4& data);
    void local_vert_get(size_t index, float_t* x, float_t* y, float_t* z, float_t* w);
    void local_vert_get(size_t index, vec4& data);
    void env_frag_get(size_t index, float_t* x, float_t* y, float_t* z, float_t* w);
    void env_frag_get(size_t index, vec4& data);
    void env_frag_get(size_t index, size_t count, vec4* data);
    void env_vert_get(size_t index, float_t* x, float_t* y, float_t* z, float_t* w);
    void env_vert_get(size_t index, vec4& data);
    void env_vert_get(size_t index, size_t count, vec4* data);
    void state_clip_get_plane(size_t index, vec4& data);
    void state_depth_get_range(vec4& data);
    void state_fog_get_color(vec4& data);
    void state_fog_get_params(vec4& data);
    void state_light_get_ambient(size_t index, vec4& data);
    void state_light_get_diffuse(size_t index, vec4& data);
    void state_light_get_specular(size_t index, vec4& data);
    void state_light_get_position(size_t index, vec4& data);
    void state_light_get_attenuation(size_t index, vec4& data);
    void state_light_get_spot_direction(size_t index, vec4& data);
    void state_light_get_half(size_t index, vec4& data);
    void state_lightmodel_get_ambient(bool back, vec4& data);
    void state_lightmodel_get_scene_color(bool back, vec4& data);
    void state_lightprod_get_ambient(bool back, size_t index, vec4& data);
    void state_lightprod_get_diffuse(bool back, size_t index, vec4& data);
    void state_lightprod_get_specular(bool back, size_t index, vec4& data);
    void state_material_get_ambient(bool back, vec4& data);
    void state_material_get_diffuse(bool back, vec4& data);
    void state_material_get_specular(bool back, vec4& data);
    void state_material_get_emission(bool back, vec4& data);
    void state_material_get_shininess(bool back, vec4& data);
    void state_matrix_get_modelview(size_t index, mat4& data);
    void state_matrix_get_projection(mat4& data);
    void state_matrix_get_mvp(mat4& data);
    void state_matrix_get_texture(size_t index, mat4& data);
    void state_matrix_get_palette(size_t index, mat4& data);
    void state_matrix_get_program(size_t index, mat4& data);
    void state_point_get_size(vec4& data);
    void state_point_get_attenuation(vec4& data);
    void state_texgen_get_eye_s(size_t index, vec4& data);
    void state_texgen_get_eye_t(size_t index, vec4& data);
    void state_texgen_get_eye_r(size_t index, vec4& data);
    void state_texgen_get_eye_q(size_t index, vec4& data);
    void state_texgen_get_object_s(size_t index, vec4& data);
    void state_texgen_get_object_t(size_t index, vec4& data);
    void state_texgen_get_object_r(size_t index, vec4& data);
    void state_texgen_get_object_q(size_t index, vec4& data);
    void state_texenv_get_color(size_t index, vec4& data);

    void buffer_set(size_t index, const vec4& data);
    void buffer_set(size_t index, size_t count, const vec4* data);
    void local_frag_set(size_t index, float_t x, float_t y, float_t z, float_t w);
    void local_frag_set(size_t index, const vec4& data);
    void local_frag_set(size_t index, size_t count, const vec4* data);
    void local_vert_set(size_t index, float_t x, float_t y, float_t z, float_t w);
    void local_vert_set(size_t index, const vec4& data);
    void local_vert_set(size_t index, size_t count, const vec4* data);
    void env_frag_set(size_t index, float_t x, float_t y, float_t z, float_t w);
    void env_frag_set(size_t index, const vec4& data);
    void env_frag_set(size_t index, size_t count, const vec4* data);
    void env_vert_set(size_t index, float_t x, float_t y, float_t z, float_t w);
    void env_vert_set(size_t index, const vec4& data);
    void env_vert_set(size_t index, size_t count, const vec4* data);
    void state_clip_set_plane(size_t index, float_t x, float_t y, float_t z, float_t w);
    void state_clip_set_plane(size_t index, const vec4& data);
    void state_depth_set_range(float_t x, float_t y, float_t z, float_t w);
    void state_depth_set_range(const vec4& data);
    void state_fog_set_color(float_t x, float_t y, float_t z, float_t w);
    void state_fog_set_color(const vec4& data);
    void state_fog_set_params(float_t x, float_t y, float_t z, float_t w);
    void state_fog_set_params(const vec4& data);
    void state_light_set_ambient(size_t index, float_t x, float_t y, float_t z, float_t w);
    void state_light_set_ambient(size_t index, const vec4& data);
    void state_light_set_diffuse(size_t index, float_t x, float_t y, float_t z, float_t w);
    void state_light_set_diffuse(size_t index, const vec4& data);
    void state_light_set_specular(size_t index, float_t x, float_t y, float_t z, float_t w);
    void state_light_set_specular(size_t index, const vec4& data);
    void state_light_set_position(size_t index, float_t x, float_t y, float_t z, float_t w);
    void state_light_set_position(size_t index, const vec4& data);
    void state_light_set_attenuation(size_t index, float_t x, float_t y, float_t z, float_t w);
    void state_light_set_attenuation(size_t index, const vec4& data);
    void state_light_set_spot_direction(size_t index, float_t x, float_t y, float_t z, float_t w);
    void state_light_set_spot_direction(size_t index, const vec4& data);
    void state_light_set_half(size_t index, float_t x, float_t y, float_t z, float_t w);
    void state_light_set_half(size_t index, const vec4& data);
    void state_lightmodel_set_ambient(bool back, float_t x, float_t y, float_t z, float_t w);
    void state_lightmodel_set_ambient(bool back, const vec4& data);
    void state_lightmodel_set_scene_color(bool back, float_t x, float_t y, float_t z, float_t w);
    void state_lightmodel_set_scene_color(bool back, const vec4& data);
    void state_lightprod_set_ambient(bool back, size_t index, float_t x, float_t y, float_t z, float_t w);
    void state_lightprod_set_ambient(bool back, size_t index, const vec4& data);
    void state_lightprod_set_diffuse(bool back, size_t index, float_t x, float_t y, float_t z, float_t w);
    void state_lightprod_set_diffuse(bool back, size_t index, const vec4& data);
    void state_lightprod_set_specular(bool back, size_t index, float_t x, float_t y, float_t z, float_t w);
    void state_lightprod_set_specular(bool back, size_t index, const vec4& data);
    void state_material_set_ambient(bool back, float_t x, float_t y, float_t z, float_t w);
    void state_material_set_ambient(bool back, const vec4& data);
    void state_material_set_diffuse(bool back, float_t x, float_t y, float_t z, float_t w);
    void state_material_set_diffuse(bool back, const vec4& data);
    void state_material_set_specular(bool back, float_t x, float_t y, float_t z, float_t w);
    void state_material_set_specular(bool back, const vec4& data);
    void state_material_set_emission(bool back, float_t x, float_t y, float_t z, float_t w);
    void state_material_set_emission(bool back, const vec4& data);
    void state_material_set_shininess(bool back, float_t x, float_t y, float_t z, float_t w);
    void state_material_set_shininess(bool back, const vec4& data);
    void state_matrix_set_modelview(const mat4& data, bool mult);
    void state_matrix_set_modelview(size_t index, const mat4& data, bool mult);
    void state_matrix_set_projection(const mat4& data, bool mult);
    void state_matrix_set_mvp(const mat4& data);
    void state_matrix_set_modelview(const mat4& model, const mat4& view, bool mult);
    void state_matrix_set_modelview(size_t index, const mat4& model, const mat4& view, bool mult);
    void state_matrix_set_mvp(const mat4& modelview, const mat4& projection);
    void state_matrix_set_mvp(const mat4& model, const mat4& view, const mat4& projection);
    void state_matrix_set_texture(size_t index, const mat4& data);
    void state_matrix_set_palette(size_t index, const mat4& data);
    void state_matrix_set_program(size_t index, const mat4& data);
    void state_point_set_size(float_t x, float_t y, float_t z, float_t w);
    void state_point_set_size(const vec4& data);
    void state_point_set_attenuation(float_t x, float_t y, float_t z, float_t w);
    void state_point_set_attenuation(const vec4& data);
    void state_texgen_set_eye_s(size_t index, float_t x, float_t y, float_t z, float_t w);
    void state_texgen_set_eye_s(size_t index, const vec4& data);
    void state_texgen_set_eye_t(size_t index, float_t x, float_t y, float_t z, float_t w);
    void state_texgen_set_eye_t(size_t index, const vec4& data);
    void state_texgen_set_eye_r(size_t index, float_t x, float_t y, float_t z, float_t w);
    void state_texgen_set_eye_r(size_t index, const vec4& data);
    void state_texgen_set_eye_q(size_t index, float_t x, float_t y, float_t z, float_t w);
    void state_texgen_set_eye_q(size_t index, const vec4& data);
    void state_texgen_set_object_s(size_t index, float_t x, float_t y, float_t z, float_t w);
    void state_texgen_set_object_s(size_t index, const vec4& data);
    void state_texgen_set_object_t(size_t index, float_t x, float_t y, float_t z, float_t w);
    void state_texgen_set_object_t(size_t index, const vec4& data);
    void state_texgen_set_object_r(size_t index, float_t x, float_t y, float_t z, float_t w);
    void state_texgen_set_object_r(size_t index, const vec4& data);
    void state_texgen_set_object_q(size_t index, float_t x, float_t y, float_t z, float_t w);
    void state_texgen_set_object_q(size_t index, const vec4& data);
    void state_texenv_set_color(size_t index, float_t x, float_t y, float_t z, float_t w);
    void state_texenv_set_color(size_t index, const vec4& data);
};
