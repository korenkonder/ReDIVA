/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../KKdLib/vec.hpp"
#include "light_param/face.hpp"
#include "light_param/fog.hpp"
#include "light_param/light.hpp"
#include "light_param/wind.hpp"
#include "light_param.hpp"
#include "camera.hpp"
#include "gl_state.hpp"
#include "light_param.hpp"
#include "object.hpp"
#include "post_process.hpp"
#include "render_texture.hpp"
#include "task.hpp"
#include "time.hpp"
#include "static_var.hpp"

#define MATRIX_BUFFER_COUNT 320
#define MATERIAL_LIST_COUNT 24
#define TEXTURE_PATTERN_COUNT 24
#define TEXTURE_TRANSFORM_COUNT 24

enum blur_filter_mode {
    BLUR_FILTER_4  = 0x00,
    BLUR_FILTER_9  = 0x01,
    BLUR_FILTER_16 = 0x02,
    BLUR_FILTER_32 = 0x03,
};

enum draw_object_type {
    DRAW_OBJECT_OPAQUE                          = 0x00,
    DRAW_OBJECT_TRANSLUCENT                     = 0x01,
    DRAW_OBJECT_TRANSLUCENT_NO_SHADOW           = 0x02,
    DRAW_OBJECT_TRANSPARENT                     = 0x03,
    DRAW_OBJECT_SHADOW_CHARA                    = 0x04,
    DRAW_OBJECT_SHADOW_STAGE                    = 0x05,
    DRAW_OBJECT_TYPE_6                          = 0x06,
    DRAW_OBJECT_TYPE_7                          = 0x07,
    DRAW_OBJECT_SHADOW_OBJECT_CHARA             = 0x08,
    DRAW_OBJECT_SHADOW_OBJECT_STAGE             = 0x09,
    DRAW_OBJECT_REFLECT_CHARA_OPAQUE            = 0x0A,
    DRAW_OBJECT_REFLECT_CHARA_TRANSLUCENT       = 0x0B,
    DRAW_OBJECT_REFLECT_CHARA_TRANSPARENT       = 0x0C,
    DRAW_OBJECT_REFLECT_OPAQUE                  = 0x0D,
    DRAW_OBJECT_REFLECT_TRANSLUCENT             = 0x0E,
    DRAW_OBJECT_REFLECT_TRANSPARENT             = 0x0F,
    DRAW_OBJECT_REFRACT_OPAQUE                  = 0x10,
    DRAW_OBJECT_REFRACT_TRANSLUCENT             = 0x11,
    DRAW_OBJECT_REFRACT_TRANSPARENT             = 0x12,
    DRAW_OBJECT_SSS                             = 0x13,
    DRAW_OBJECT_OPAQUE_ALPHA_ORDER_1            = 0x14,
    DRAW_OBJECT_TRANSPARENT_ALPHA_ORDER_1       = 0x15,
    DRAW_OBJECT_TRANSLUCENT_ALPHA_ORDER_1       = 0x16,
    DRAW_OBJECT_OPAQUE_ALPHA_ORDER_2            = 0x17,
    DRAW_OBJECT_TRANSPARENT_ALPHA_ORDER_2       = 0x18,
    DRAW_OBJECT_TRANSLUCENT_ALPHA_ORDER_2       = 0x19,
    DRAW_OBJECT_OPAQUE_ALPHA_ORDER_3            = 0x1A,
    DRAW_OBJECT_TRANSPARENT_ALPHA_ORDER_3       = 0x1B,
    DRAW_OBJECT_TRANSLUCENT_ALPHA_ORDER_3       = 0x1C,
    DRAW_OBJECT_PREPROCESS                     = 0x1D,
    DRAW_OBJECT_OPAQUE_LOCAL                    = 0x1E, // X
    DRAW_OBJECT_TRANSLUCENT_LOCAL               = 0x1F,
    DRAW_OBJECT_TRANSPARENT_LOCAL               = 0x20,
    DRAW_OBJECT_OPAQUE_ALPHA_ORDER_2_LOCAL      = 0x21,
    DRAW_OBJECT_TRANSPARENT_ALPHA_ORDER_2_LOCAL = 0x22,
    DRAW_OBJECT_TRANSLUCENT_ALPHA_ORDER_2_LOCAL = 0x23,
    DRAW_OBJECT_MAX                             = 0x24,
};

enum draw_pass_type {
    DRAW_PASS_SHADOW       = 0x00,
    DRAW_PASS_SS_SSS       = 0x01,
    DRAW_PASS_TYPE_2       = 0x02,
    DRAW_PASS_REFLECT      = 0x03,
    DRAW_PASS_REFRACT      = 0x04,
    DRAW_PASS_PRE_PROCESS  = 0x05,
    DRAW_PASS_CLEAR        = 0x06,
    DRAW_PASS_PRE_SPRITE   = 0x07,
    DRAW_PASS_3D           = 0x08,
    DRAW_PASS_SHOW_VECTOR  = 0x09,
    DRAW_PASS_POST_PROCESS = 0x0A,
    DRAW_PASS_SPRITE       = 0x0B,
    DRAW_PASS_TYPE_12      = 0x0C,
    DRAW_PASS_MAX          = 0x0D,
};

enum draw_task_flags : uint32_t {
    DRAW_TASK_SHADOW                = 0x00000001,
    DRAW_TASK_2                     = 0x00000002,
    DRAW_TASK_4                     = 0x00000004,
    DRAW_TASK_8                     = 0x00000008,
    DRAW_TASK_10                    = 0x00000010,
    DRAW_TASK_20                    = 0x00000020,
    DRAW_TASK_40                    = 0x00000040,
    DRAW_TASK_SHADOW_OBJECT         = 0x00000080,
    DRAW_TASK_CHARA_REFLECT         = 0x00000100,
    DRAW_TASK_REFLECT               = 0x00000200,
    DRAW_TASK_REFRACT               = 0x00000400,
    DRAW_TASK_800                   = 0x00000800,
    DRAW_TASK_TRANSLUCENT_NO_SHADOW = 0x00001000,
    DRAW_TASK_SSS                   = 0x00002000,
    DRAW_TASK_4000                  = 0x00004000,
    DRAW_TASK_8000                  = 0x00008000,
    DRAW_TASK_ALPHA_ORDER_1         = 0x00010000,
    DRAW_TASK_ALPHA_ORDER_2         = 0x00020000,
    DRAW_TASK_ALPHA_ORDER_3         = 0x00040000,
    DRAW_TASK_80000                 = 0x00080000,
    DRAW_TASK_100000                = 0x00100000,
    DRAW_TASK_200000                = 0x00200000,
    DRAW_TASK_400000                = 0x00400000,
    DRAW_TASK_800000                = 0x00800000,
    DRAW_TASK_PREPROCESS           = 0x01000000,
    DRAW_TASK_2000000               = 0x02000000,
    DRAW_TASK_4000000               = 0x04000000,
    DRAW_TASK_8000000               = 0x08000000,
    DRAW_TASK_10000000              = 0x10000000,
    DRAW_TASK_20000000              = 0x20000000,
    DRAW_TASK_40000000              = 0x40000000,
    DRAW_TASK_NO_TRANSLUCENCY       = 0x80000000,
};

enum draw_task_type {
    DRAW_TASK_OBJECT             = 0x00,
    DRAW_TASK_OBJECT_PRIMITIVE   = 0x01,
    DRAW_TASK_OBJECT_PREPROCESS = 0x02,
    DRAW_TASK_OBJECT_TRANSLUCENT = 0x03,
};

enum reflect_refract_resolution_mode {
    REFLECT_REFRACT_RESOLUTION_256x256 = 0x00,
    REFLECT_REFRACT_RESOLUTION_512x256 = 0x01,
    REFLECT_REFRACT_RESOLUTION_512x512 = 0x02,
};

enum shadow_type_enum {
    SHADOW_CHARA = 0x00,
    SHADOW_STAGE = 0x01,
    SHADOW_MAX   = 0x02,
};

struct render_context;
struct shadow;

struct draw_state_stats {
    int32_t object_draw_count;
    int32_t object_translucent_draw_count;
    int32_t object_reflect_draw_count;
    int32_t field_C;
    int32_t field_10;
    int32_t draw_count;
    int32_t draw_triangle_count;
    int32_t field_1C;

    draw_state_stats();

    void reset();
};

struct sss_data {
    bool init;
    bool enable;
    bool npr_contour;
    render_texture textures[4];
    vec4 param;

    sss_data();
    ~sss_data();
};

struct draw_preprocess {
    int32_t type;
    void(*func)(void*);
    void* data;
};

struct draw_pass {
    bool enable[DRAW_PASS_MAX];
    shadow* shadow_ptr;
    bool reflect;
    bool refract;
    int32_t reflect_blur_num;
    blur_filter_mode reflect_blur_filter;
    bool wait_for_gpu;
    double_t cpu_time[DRAW_PASS_MAX];
    double_t gpu_time[DRAW_PASS_MAX];
    time_struct time;
    bool shadow;
    bool opaque_z_sort;
    bool alpha_z_sort;
    bool draw_pass_3d[DRAW_PASS_3D_MAX];
    stage_data_reflect_type reflect_type;
    int32_t tex_index[11];
    render_texture render_textures[8];
    GLuint multisample_framebuffer;
    GLuint multisample_renderbuffer;
    bool multisample;
    int32_t show_vector_flags;
    float_t show_vector_length;
    float_t show_vector_z_offset;
    bool field_2F8;
    std::list<draw_preprocess> preprocess;
    texture* effect_texture;
    int32_t npr_param;
    bool field_31C;
    bool field_31D;
    bool field_31E;
    bool field_31F;
    bool field_320;
    bool npr;
    sss_data sss_data;
    GLuint samplers[18];

    draw_pass();
    ~draw_pass();

    void add_preprocess(int32_t type, void(*func)(void*), void* data);
    void clear_preprocess(int32_t type);
    render_texture& get_render_texture(int32_t index);
    void resize(int32_t width, int32_t height);
    void set_enable(draw_pass_type type, bool value);
    void set_effect_texture(texture* value);
    void set_multisample(bool value);
    void set_npr_param(int32_t value);
    void set_reflect(bool value);
    void set_reflect_blur(int32_t reflect_blur_num, blur_filter_mode reflect_blur_filter);
    void set_reflect_resolution_mode(reflect_refract_resolution_mode mode);
    void set_reflect_type(stage_data_reflect_type type);
    void set_refract(bool value);
    void set_refract_resolution_mode(reflect_refract_resolution_mode mode);
    void set_shadow_false();
    void set_shadow_true();
};

struct draw_state {
    draw_state_stats stats;
    draw_state_stats stats_prev;
    bool wireframe;
    bool wireframe_overlay;
    bool light;
    bool self_shadow;
    bool field_45;
    bool use_global_material;
    bool fog_height;
    bool ex_data_mat;
    bool shader;
    int32_t shader_index;
    int32_t field_50;
    float_t bump_depth;
    float_t intensity;
    float_t reflectivity;
    float_t reflect_uv_scale;
    float_t refract_uv_scale;
    int32_t field_68;
    float_t fresnel;

    draw_state();

    void set_fog_height(bool value);
};

struct material_list_struct {
    uint32_t hash;
    vec4 blend_color;
    vec4u8 has_blend_color;
    vec4 emission;
    vec4u8 has_emission;

    material_list_struct();
    material_list_struct(uint32_t hash, vec4& blend_color,
        vec4u8& has_blend_color, vec4& emission, vec4u8& has_emission);
};

struct texture_pattern_struct {
    texture_id src;
    texture_id dst;

    texture_pattern_struct();
    texture_pattern_struct(texture_id src, texture_id dst);
};

struct texture_transform_struct {
    uint32_t id;
    mat4 mat;

    texture_transform_struct();
    texture_transform_struct(uint32_t id, mat4& mat);
};

struct draw_object;

struct draw_object {
    obj_sub_mesh* sub_mesh;
    obj_mesh* mesh;
    obj_material_data* material;
    std::vector<GLuint>* textures;
    int32_t mat_count;
    mat4* mats;
    GLuint vertex_array;
    GLuint array_buffer;
    GLuint element_array_buffer;
    bool set_blend_color;
    bool chara_color;
    vec4 blend_color;
    vec4 emission;
    bool self_shadow;
    shadow_type_enum shadow;
    GLuint morph_array_buffer;
    float_t morph_value;
    int32_t texture_pattern_count;
    texture_pattern_struct texture_pattern_array[TEXTURE_PATTERN_COUNT];
    vec4 texture_color_coeff;
    vec4 texture_color_offset;
    vec4 texture_specular_coeff;
    vec4 texture_specular_offset;
    int32_t texture_transform_count;
    texture_transform_struct texture_transform_array[TEXTURE_TRANSFORM_COUNT];
    int32_t instances_count;
    mat4* instances_mat;
    void(*draw_object_func)(draw_object*);
    bool vertex_attrib_array[16];
};

enum draw_primitive_type {
    DRAW_PRIMITIVE_TEAPOT = 0x00,
    DRAW_PRIMITIVE_TYPE_1 = 0x01,
    DRAW_PRIMITIVE_CUBE   = 0x02,
    DRAW_PRIMITIVE_SPHERE = 0x03,
    DRAW_PRIMITIVE_TYPE_4 = 0x04,
    DRAW_PRIMITIVE_CONE   = 0x05,
    DRAW_PRIMITIVE_LINE   = 0x06,
    DRAW_PRIMITIVE_TYPE_7 = 0x07,
};

struct draw_primitive_teapot {
    float_t size;
};

struct draw_primitive_type_1 {
    int32_t field_0;
    int32_t field_4;
    int32_t field_8;
    int32_t field_C;
};

struct draw_primitive_cube {
    vec3 size;
    bool wireframe;
};

struct draw_primitive_sphere {
    float_t radius;
    int32_t slices;
    int32_t stacks;
    bool wireframe;
};

struct draw_primitive_type_4 {
    int32_t field_0;
    int32_t field_4;
};

struct draw_primitive_cone {
    float_t base;
    float_t height;
    int32_t slices;
    int32_t stacks;
    bool wireframe;
};

struct draw_primitive_line {
    vec3 v0;
    vec3 v1;
};

struct draw_primitive_type_7 {
    float_t size;
};

union draw_primitive_union {
    draw_primitive_teapot teapot;
    draw_primitive_type_1 field_1;
    draw_primitive_cube cube;
    draw_primitive_sphere sphere;
    draw_primitive_type_4 type_4;
    draw_primitive_cone cone;
    draw_primitive_line line;
    draw_primitive_type_7 type_7;
};

struct draw_primitive {
    draw_primitive_type type;
    bool fog;
    vec4 color;
    draw_primitive_union data;
};

struct draw_task_preprocess {
    void(*func)(render_context* rctx, void* data);
    void* data;
};

struct draw_task_object_translucent {
    int32_t count;
    draw_object* objects[40];
};

union draw_task_union {
    draw_object object;
    draw_primitive primitive;
    draw_task_preprocess preprocess;
    draw_task_object_translucent object_translucent;
};

struct draw_task {
    draw_task_type type;
    mat4 mat;
    float_t depth;
    float_t bounding_radius;
    draw_task_union data;
};

struct light_proj {
    bool enable;
    render_texture shadow_texture[2];
    render_texture draw_texture;
    uint32_t texture_id;

    light_proj(int32_t width, int32_t height);
    ~light_proj();

    void resize(int32_t width, int32_t height);
    bool set(render_context* rctx);

    static void get_proj_mat(vec3* view_point, vec3* interest, float_t fov, mat4* mat);
    static bool set_mat(render_context* rctx, bool set_mat);
};

struct morph_struct {
    object_info object;
    float_t value;

    morph_struct();
};

struct object_data_buffer {
    int32_t offset;
    int32_t max_offset;
    int32_t size; //0x300000
    void* data;

    object_data_buffer();
    ~object_data_buffer();

    draw_task* add_draw_task(draw_task_type type);
    mat4* add_mat4(int32_t count);
    void reset();
};

struct object_data_culling_info {
    int32_t objects;
    int32_t meshes;
    int32_t submesh_array;
};

struct object_data_vertex_array {
    GLuint array_buffer;
    GLuint morph_array_buffer;
    GLuint element_array_buffer;
    int32_t alive_time;
    GLuint vertex_array;
    bool vertex_attrib_array[16];
    obj_vertex_format vertex_format;
    GLsizei size_vertex;
    bool compressed;
    GLuint vertex_attrib_buffer_binding[16];
    int32_t texcoord_array[2];
};

struct object_data {
    draw_task_flags draw_task_flags;
    shadow_type_enum shadow_type;
    int32_t field_8;
    int32_t field_C;
    std::vector<draw_task*> draw_task_array[DRAW_OBJECT_MAX];
    object_data_culling_info passed;
    object_data_culling_info culled;
    object_data_culling_info passed_prev;
    object_data_culling_info culled_prev;
    int32_t field_230;
    bool show_alpha_center;
    bool show_mat_center;
    bool object_culling;
    bool object_sort;
    bool chara_color;
    object_data_buffer buffer;
    morph_struct morph;
    int32_t texture_pattern_count;
    texture_pattern_struct texture_pattern_array[TEXTURE_PATTERN_COUNT];
    vec4 texture_color_coeff;
    vec4 texture_color_offset;
    vec4 texture_specular_coeff;
    vec4 texture_specular_offset;
    float_t wet_param;
    int32_t texture_transform_count;
    texture_transform_struct texture_transform_array[TEXTURE_TRANSFORM_COUNT];
    int32_t material_list_count;
    material_list_struct material_list_array[MATERIAL_LIST_COUNT];
    bool(*object_bounding_sphere_check_func)(obj_bounding_sphere*, camera*);
    std::vector<object_data_vertex_array> vertex_array_cache;

    object_data();
    ~object_data();

    void add_vertex_array(draw_object* draw);
    void check_vertex_arrays();
    GLuint get_vertex_array(draw_object* draw);
    bool get_chara_color();
    ::draw_task_flags get_draw_task_flags();
    void get_material_list(int32_t& count, material_list_struct*& value);
    void get_morph(object_info* object, float_t* value);
    shadow_type_enum get_shadow_type();
    void get_texture_color_coeff(vec4& value);
    void get_texture_color_offset(vec4& value);
    void get_texture_pattern(int32_t& count, texture_pattern_struct*& value);
    void get_texture_specular_coeff(vec4& value);
    void get_texture_specular_offset(vec4& value);
    void get_texture_transform(int32_t& count, texture_transform_struct*& value);
    float_t get_wet_param();
    void reset();
    void set_chara_color(bool value = false);
    void set_draw_task_flags(::draw_task_flags flags = (::draw_task_flags)0);
    void set_material_list(int32_t count = 0, material_list_struct* value = 0);
    void set_morph(object_info object = {}, float_t value = 0.0f);
    void set_object_bounding_sphere_check_func(bool(*func)(obj_bounding_sphere*, camera*) = 0);
    void set_shadow_type(shadow_type_enum type = SHADOW_CHARA);
    void set_texture_color_coeff(vec4& value);
    void set_texture_color_offset(vec4& value);
    void set_texture_pattern(int32_t count = 0, texture_pattern_struct* value = 0);
    void set_texture_specular_coeff(vec4& value);
    void set_texture_specular_offset(vec4& value);
    void set_texture_transform(int32_t count = 0, texture_transform_struct* value = 0);
    void set_wet_param(float_t value = 0.0f);
};

struct render_context {
    ::camera* camera;
    draw_state draw_state;
    object_data object_data;
    draw_pass draw_pass;

    face face;
    fog fog[FOG_MAX];
    light_proj* litproj;
    light_set light_set[LIGHT_SET_MAX];

    post_process post_process;
    bool chara_reflect;
    bool chara_refract;

    mat4 view_mat;
    mat4 matrix_buffer[MATRIX_BUFFER_COUNT];

    render_context();
    ~render_context();

    void ctrl();
    void disp();
    void light_param_data_light_set(light_param_light* light);
    void light_param_data_fog_set(light_param_fog* f);
    void light_param_data_glow_set(light_param_glow* glow);
    void light_param_data_ibl_set(light_param_ibl* ibl, light_param_data_storage* storage);
    void light_param_data_wind_set(light_param_wind* w);
    void light_param_data_face_set(light_param_face* face);
};

struct shadow {
    render_texture field_8[7];
    render_texture* field_158[3];
    float_t view_region;
    float_t range;
    vec3 view_point[2];
    vec3 interest[2];
    vec3 field_1A8[2];
    float_t field_1C0[2];
    float_t field_1C8[2];
    std::vector<vec3> field_1D0[2];
    int32_t field_200[2];
    float_t field_208;
    vec3 direction;
    vec3 view_point_shared;
    vec3 interest_shared;
    mat4 view_mat[2];
    int32_t near_blur;
    blur_filter_mode blur_filter;
    int32_t far_blur;
    int32_t field_2BC;
    float_t distance;
    float_t field_2C4;
    float_t z_near;
    float_t z_far;
    float_t field_2D0;
    float_t field_2D4;
    float_t field_2D8;
    float_t field_2DC;
    float_t field_2E0;
    float_t ambient;
    bool field_2E8;
    int32_t field_2EC;
    bool field_2F0[2];
    bool self_shadow;
    bool blur_filter_enable[2];
    bool field_2F5;

    shadow();
    ~shadow();

    void ctrl(render_context* rctx);
    int32_t init_data();
};
