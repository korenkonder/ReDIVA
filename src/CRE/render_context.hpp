/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../KKdLib/time.hpp"
#include "../KKdLib/vec.hpp"
#include "GL/array_buffer.hpp"
#include "GL/element_array_buffer.hpp"
#include "GL/uniform_buffer.hpp"
#include "light_param/face.hpp"
#include "light_param/fog.hpp"
#include "light_param/light.hpp"
#include "light_param/wind.hpp"
#include "light_param.hpp"
#include "camera.hpp"
#include "color.hpp"
#include "gl_state.hpp"
#include "light_param.hpp"
#include "object.hpp"
#include "post_process.hpp"
#include "render_texture.hpp"
#include "task.hpp"
#include "static_var.hpp"

#define MATRIX_BUFFER_COUNT 320
#define MATERIAL_LIST_COUNT 24
#define TEXTURE_PATTERN_COUNT 24
#define TEXTURE_TRANSFORM_COUNT 24

enum blur_filter_mode {
    BLUR_FILTER_4 = 0,
    BLUR_FILTER_9,
    BLUR_FILTER_16,
    BLUR_FILTER_32,
    BLUR_FILTER_MAX,
};

enum reflect_refract_resolution_mode {
    REFLECT_REFRACT_RESOLUTION_256x256 = 0,
    REFLECT_REFRACT_RESOLUTION_512x256,
    REFLECT_REFRACT_RESOLUTION_512x512,
    REFLECT_REFRACT_RESOLUTION_MAX,
};

enum shadow_type_enum {
    SHADOW_CHARA = 0,
    SHADOW_STAGE,
    SHADOW_MAX,
};

namespace mdl {
    enum EtcObjType {
        ETC_OBJ_TEAPOT = 0,
        ETC_OBJ_GRID,
        ETC_OBJ_CUBE,
        ETC_OBJ_SPHERE,
        ETC_OBJ_PLANE,
        ETC_OBJ_CONE,
        ETC_OBJ_LINE,
        ETC_OBJ_CROSS,
        ETC_OBJ_CAPSULE,  // Added
        ETC_OBJ_ELLIPSE,  // Added
        ETC_OBJ_CYLINDER, // Added
        ETC_OBJ_MAX,
    };

    enum ObjKind {
        OBJ_KIND_NORMAL = 0,
        OBJ_KIND_ETC,
        OBJ_KIND_USER,
        OBJ_KIND_TRANSLUCENT,
        OBJ_KIND_MAX,
    };

    enum ObjFlags : uint32_t {
        OBJ_SHADOW                = 0x00000001,
        OBJ_2                     = 0x00000002,
        OBJ_4                     = 0x00000004,
        OBJ_8                     = 0x00000008,
        OBJ_10                    = 0x00000010,
        OBJ_20                    = 0x00000020,
        OBJ_40                    = 0x00000040,
        OBJ_SHADOW_OBJECT         = 0x00000080,
        OBJ_CHARA_REFLECT         = 0x00000100,
        OBJ_REFLECT               = 0x00000200,
        OBJ_REFRACT               = 0x00000400,
        OBJ_800                   = 0x00000800,
        OBJ_TRANSLUCENT_NO_SHADOW = 0x00001000,
        OBJ_SSS                   = 0x00002000,
        OBJ_4000                  = 0x00004000,
        OBJ_8000                  = 0x00008000,
        OBJ_ALPHA_ORDER_1         = 0x00010000,
        OBJ_ALPHA_ORDER_2         = 0x00020000,
        OBJ_ALPHA_ORDER_3         = 0x00040000,
        OBJ_80000                 = 0x00080000,
        OBJ_100000                = 0x00100000,
        OBJ_200000                = 0x00200000,
        OBJ_400000                = 0x00400000,
        OBJ_800000                = 0x00800000,
        OBJ_USER                  = 0x01000000,
        OBJ_2000000               = 0x02000000,
        OBJ_4000000               = 0x04000000,
        OBJ_8000000               = 0x08000000,
        OBJ_10000000              = 0x10000000,
        OBJ_20000000              = 0x20000000,
        OBJ_40000000              = 0x40000000,
        OBJ_NO_TRANSLUCENCY       = 0x80000000,
    };

    enum ObjType {
        OBJ_TYPE_OPAQUE = 0,
        OBJ_TYPE_TRANSLUCENT,
        OBJ_TYPE_TRANSLUCENT_NO_SHADOW,
        OBJ_TYPE_TRANSPARENT,
        OBJ_TYPE_SHADOW_CHARA,
        OBJ_TYPE_SHADOW_STAGE,
        OBJ_TYPE_TYPE_6,
        OBJ_TYPE_TYPE_7,
        OBJ_TYPE_SHADOW_OBJECT_CHARA,
        OBJ_TYPE_SHADOW_OBJECT_STAGE,
        OBJ_TYPE_REFLECT_CHARA_OPAQUE,
        OBJ_TYPE_REFLECT_CHARA_TRANSLUCENT,
        OBJ_TYPE_REFLECT_CHARA_TRANSPARENT,
        OBJ_TYPE_REFLECT_OPAQUE,
        OBJ_TYPE_REFLECT_TRANSLUCENT,
        OBJ_TYPE_REFLECT_TRANSPARENT,
        OBJ_TYPE_REFRACT_OPAQUE,
        OBJ_TYPE_REFRACT_TRANSLUCENT,
        OBJ_TYPE_REFRACT_TRANSPARENT,
        OBJ_TYPE_SSS,
        OBJ_TYPE_OPAQUE_ALPHA_ORDER_1,
        OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_1,
        OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_1,
        OBJ_TYPE_OPAQUE_ALPHA_ORDER_2,
        OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_2,
        OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_2,
        OBJ_TYPE_OPAQUE_ALPHA_ORDER_3,
        OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_3,
        OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_3,
        OBJ_TYPE_USER,
        OBJ_TYPE_OPAQUE_LOCAL,                    // X
        OBJ_TYPE_TRANSLUCENT_LOCAL,
        OBJ_TYPE_TRANSPARENT_LOCAL,
        OBJ_TYPE_OPAQUE_ALPHA_ORDER_2_LOCAL,
        OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_2_LOCAL,
        OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_2_LOCAL,
        OBJ_TYPE_MAX,
    };
}

namespace rndr {
    enum RenderPassID {
        RND_PASSID_SHADOW = 0,
        RND_PASSID_SS_SSS,
        RND_PASSID_2,
        RND_PASSID_REFLECT,
        RND_PASSID_REFRACT,
        RND_PASSID_PRE_PROCESS,
        RND_PASSID_CLEAR,
        RND_PASSID_PRE_SPRITE,
        RND_PASSID_3D,
        RND_PASSID_SHOW_VECTOR,
        RND_PASSID_POST_PROCESS,
        RND_PASSID_SPRITE,
        RND_PASSID_12,
        RND_PASSID_NUM,
    };
}

struct render_context;
struct Shadow;

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
    RenderTexture textures[4];
    vec4 param;

    sss_data();
    ~sss_data();
};

struct draw_state {
    draw_state_stats stats;
    draw_state_stats stats_prev;
    bool wireframe;
    bool wireframe_overlay;
    bool light;
    bool self_shadow;
    bool shader_debug_flag;
    bool use_global_material;
    bool fog_height;
    bool ex_data_mat;
    bool shader;
    int32_t shader_index;
    int32_t show;
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

struct draw_pre_process {
    int32_t type;
    void(*func)(void*);
    void* data;
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

namespace mdl {
    struct DispManager;
    struct ObjSubMeshArgs;

    struct ObjSubMeshArgs {
        const obj_sub_mesh* sub_mesh;
        const obj_mesh* mesh;
        const obj_material_data* material;
        const std::vector<GLuint>* textures;
        int32_t mat_count;
        const mat4* mats;
        //GLuint vertex_buffer;
        obj_mesh_vertex_buffer* vertex_buffer;
        //GLuint index_buffer;
        obj_mesh_index_buffer* index_buffer;
        bool set_blend_color;
        bool chara_color;
        vec4 blend_color;
        vec4 emission;
        bool self_shadow;
        shadow_type_enum shadow;
        //GLuint morph_vertex_buffer;
        obj_mesh_vertex_buffer* morph_vertex_buffer;
        float_t morph_weight;
        int32_t texture_pattern_count;
        texture_pattern_struct texture_pattern_array[TEXTURE_PATTERN_COUNT];
        vec4 texture_color_coefficients;
        vec4 texture_color_offset;
        vec4 texture_specular_coefficients;
        vec4 texture_specular_offset;
        int32_t texture_transform_count;
        texture_transform_struct texture_transform_array[TEXTURE_TRANSFORM_COUNT];
        int32_t instances_count;
        const mat4* instances_mat;
        void(*func)(const mdl::ObjSubMeshArgs*);
    };

    struct EtcObjTeapot {
        float_t size;

        EtcObjTeapot();
    };

    struct EtcObjGrid {
        int32_t w;
        int32_t h;
        int32_t ws;
        int32_t hs;

        EtcObjGrid();
    };

    struct EtcObjCube {
        vec3 size;
        bool wire;

        EtcObjCube();
    };

    struct EtcObjSphere {
        float_t radius;
        int32_t slices;
        int32_t stacks;
        bool wire;

        EtcObjSphere();
    };

    struct EtcObjPlane {
        int32_t w;
        int32_t h;

        EtcObjPlane();
    };

    struct EtcObjCone {
        float_t base;
        float_t height;
        int32_t slices;
        int32_t stacks;
        bool wire;

        EtcObjCone();
    };

    struct EtcObjLine {
        vec3 pos[2];

        EtcObjLine();
    };

    struct EtcObjCross {
        float_t size;

        EtcObjCross();
    };

    struct EtcObjCapsule { // Added
        float_t radius;
        int32_t slices;
        int32_t stacks;
        bool wire;
        vec3 pos[2];

        EtcObjCapsule();
    };

    struct EtcObjEllipse { // Added
        float_t radius;
        int32_t slices;
        int32_t stacks;
        bool wire;
        vec3 pos[2];

        EtcObjEllipse();
    };

    struct EtcObjCylinder { // Added
        float_t base;
        float_t top;
        float_t height;
        int32_t slices;
        int32_t stacks;
        bool wire;

        EtcObjCylinder();
    };

    struct EtcObj {
        union Data {
            EtcObjTeapot teapot;
            EtcObjGrid grid;
            EtcObjCube cube;
            EtcObjSphere sphere;
            EtcObjPlane plane;
            EtcObjCone cone;
            EtcObjLine line;
            EtcObjCross cross;
            EtcObjCapsule capsule; // Added
            EtcObjEllipse ellipse; // Added
            EtcObjCylinder cylinder; // Added

            Data();
        };

        EtcObjType type;
        color4u8 color;
        //bool fog;
        bool constant; // Added
        Data data;
        GLsizei count; // Added
        size_t offset; // Added

        EtcObj();
        void init(EtcObjType type);
    };

    typedef void(*UserArgsFunc)(render_context* rctx, void* data);

    struct UserArgs {
        UserArgsFunc func;
        void* data;
    };

    struct ObjTranslucentArgs {
        uint32_t count;
        ObjSubMeshArgs* sub_mesh[40];
    };

    struct ObjData {
        union Args {
            ObjSubMeshArgs sub_mesh;
            EtcObj etc;
            UserArgs user;
            ObjTranslucentArgs translucent;
        };

        ObjKind kind;
        mat4 mat;
        float_t view_z;
        float_t radius;
        Args args;

        void init_etc(DispManager* disp_manager, const mat4* mat, mdl::EtcObj* etc);
        void init_sub_mesh(DispManager* disp_manager, const mat4* mat, float_t radius,
            obj_sub_mesh* sub_mesh, obj_mesh* mesh, obj_material_data* material, std::vector<GLuint>* textures,
            int32_t mat_count, mat4* mats, /*GLuint vertex_buffer, GLuint index_buffer,*/
            obj_mesh_vertex_buffer* vertex_buffer, obj_mesh_index_buffer* index_buffer, vec4* blend_color,
            vec4* emission, /*GLuint morph_vertex_buffer,*/ obj_mesh_vertex_buffer* morph_vertex_buffer,
            int32_t instances_count, mat4* instances_mat, void(*func)(const ObjSubMeshArgs*));
        void init_translucent(const mat4* mat, ObjTranslucentArgs* translucent);
        void init_user(const mat4* mat, UserArgsFunc func, void* data);
    };
}

struct light_proj {
    bool enable;
    RenderTexture shadow_texture[2];
    RenderTexture draw_texture;
    uint32_t texture_id;

    light_proj(int32_t width, int32_t height);
    ~light_proj();

    void resize(int32_t width, int32_t height);
    bool set(render_context* rctx);

    static bool set_mat(render_context* rctx, bool set_mat);
};

struct morph_struct {
    object_info object;
    float_t weight;

    morph_struct();
};

struct texture_data_struct {
    int32_t field_0;
    vec3 texture_color_coefficients;
    vec3 texture_color_offset;
    vec3 texture_specular_coefficients;
    vec3 texture_specular_offset;

    texture_data_struct();
};

namespace mdl {
    struct CullingCheck {
        struct Info {
            int32_t objects;
            int32_t meshes;
            int32_t submesh_array;
        };

        Info passed;
        Info culled;
        Info passed_prev;
        Info culled_prev;

        bool(*func)(obj_bounding_sphere*, mat4*);
    };

    struct DispManager {
        struct vertex_array {
            //GLuint vertex_buffer;
            obj_mesh_vertex_buffer* vertex_buffer;
            //GLuint morph_vertex_buffer;
            obj_mesh_vertex_buffer* morph_vertex_buffer;
            //GLuint index_buffer;
            obj_mesh_index_buffer* index_buffer;
            int32_t alive_time;
            GLuint vertex_array;
            bool vertex_attrib_array[16];
            obj_vertex_format vertex_format;
            GLsizei size_vertex;
            bool compressed;
            GLuint vertex_attrib_buffer_binding[16];
            int32_t texcoord_array[2];
        };

        struct etc_vertex_array {
            GL::ArrayBuffer vertex_buffer;
            GL::ElementArrayBuffer index_buffer;
            int32_t alive_time;
            GLuint vertex_array;
            EtcObj::Data data;
            EtcObjType type;
            GLsizei count;
            size_t offset;
            GLsizei wire_count;
            size_t wire_offset;
            size_t max_vtx;
            size_t max_idx;
        };

        mdl::ObjFlags obj_flags;
        shadow_type_enum shadow_type;
        int32_t field_8;
        int32_t field_C;
        std::vector<mdl::ObjData*> obj[mdl::OBJ_TYPE_MAX];
        mdl::CullingCheck culling;
        int32_t put_index;
        bool show_alpha_center;
        bool show_mat_center;
        bool object_culling;
        bool object_sort;
        bool chara_color;
        int32_t buff_offset;
        int32_t buff_max;
        int32_t buff_size;
        void* buff;
        morph_struct morph;
        int32_t texture_pattern_count;
        texture_pattern_struct texture_pattern_array[TEXTURE_PATTERN_COUNT];
        vec4 texture_color_coefficients;
        vec4 texture_color_offset;
        vec4 texture_specular_coefficients;
        vec4 texture_specular_offset;
        float_t wet_param;
        int32_t texture_transform_count;
        texture_transform_struct texture_transform_array[TEXTURE_TRANSFORM_COUNT];
        int32_t material_list_count;
        material_list_struct material_list_array[MATERIAL_LIST_COUNT];
        std::vector<vertex_array> vertex_array_cache;
        std::vector<etc_vertex_array> etc_vertex_array_cache;

        DispManager();
        ~DispManager();

        void add_vertex_array(ObjSubMeshArgs* args);
        void add_vertex_array(EtcObj* etc, mat4& mat);
        ObjData* alloc_data(ObjKind kind);
        mat4* alloc_data(int32_t count);
        void buffer_reset();
        void check_vertex_arrays();
        void draw(mdl::ObjType type, int32_t depth_mask = 0, bool a4 = true);
        void draw_translucent(mdl::ObjType type, int32_t alpha);
        /*void draw_show_vector(mdl::ObjType type, int32_t show_vector);*/
        void entry_list(ObjType type, ObjData* data);
        bool entry_obj(::obj* object, obj_mesh_vertex_buffer* obj_vertex_buf,
            obj_mesh_index_buffer* obj_index_buf, const mat4* mat,
            std::vector<GLuint>* textures, vec4* blend_color, mat4* bone_mat, ::obj* object_morph,
            obj_mesh_vertex_buffer* obj_morph_vertex_buf, int32_t instances_count,
            mat4* instances_mat, void(*func)(const mdl::ObjSubMeshArgs*), bool enable_bone_mat, bool local = false);
        void entry_obj_by_obj(const mat4* mat,
            ::obj* obj, std::vector<GLuint>* textures, obj_mesh_vertex_buffer* obj_vert_buf,
            obj_mesh_index_buffer* obj_index_buf, mat4* bone_mat, float_t alpha);
        bool entry_obj_by_object_info(const mat4* mat, object_info obj_info, mat4* bone_mat = 0);
        bool entry_obj_by_object_info(const mat4* mat, object_info obj_info,
            vec4* blend_color, mat4* bone_mat, int32_t instances_count,
            mat4* instances_mat, void(*func)(const mdl::ObjSubMeshArgs*), bool enable_bone_mat, bool local = false);
        bool entry_obj_by_object_info(const mat4* mat, object_info obj_info, float_t alpha, mat4* bone_mat = 0);
        bool entry_obj_by_object_info(const mat4* mat, object_info obj_info,
            float_t r, float_t g, float_t b, float_t a, mat4* bone_mat = 0, bool local = false);
        bool entry_obj_by_object_info(const mat4* mat, object_info obj_info,
            vec4* blend_color, mat4* bone_mat = 0, bool local = false);
        void entry_obj_by_object_info_object_skin(object_info obj_info,
            std::vector<texture_pattern_struct>* texture_pattern, texture_data_struct* texture_data, float_t alpha,
            mat4* matrices, mat4* ex_data_matrices, const mat4* mat, const mat4* global_mat);
        void entry_obj_etc(const mat4* mat, EtcObj* etc, bool local = false);
        void entry_obj_user(const mat4* mat, UserArgsFunc func, void* data, ObjType type);
        GLuint get_vertex_array(const ObjSubMeshArgs* args);
        GLuint get_vertex_array(const EtcObj* etc);
        bool get_chara_color();
        ObjFlags get_obj_flags();
        void get_material_list(int32_t& count, material_list_struct*& value);
        void get_morph(object_info& object, float_t& weight);
        int32_t get_obj_count(ObjType type);
        shadow_type_enum get_shadow_type();
        void get_texture_color_coeff(vec4& value);
        void get_texture_color_offset(vec4& value);
        void get_texture_pattern(int32_t& count, texture_pattern_struct*& value);
        void get_texture_specular_coeff(vec4& value);
        void get_texture_specular_offset(vec4& value);
        void get_texture_transform(int32_t& count, texture_transform_struct*& value);
        float_t get_wet_param();
        void obj_sort(mat4* view, ObjType type, int32_t compare_func);
        void refresh();
        void set_chara_color(bool value = false);
        void set_obj_flags(ObjFlags flags = (ObjFlags)0);
        void set_material_list(int32_t count = 0, material_list_struct* value = 0);
        void set_morph(object_info object = {}, float_t weight = 0.0f);
        void set_culling_finc(bool(*func)(obj_bounding_sphere*, mat4*) = 0);
        void set_shadow_type(shadow_type_enum type = SHADOW_CHARA);
        void set_texture_color_coefficients(vec4& value);
        void set_texture_color_offset(vec4& value);
        void set_texture_pattern(int32_t count = 0, texture_pattern_struct* value = 0);
        void set_texture_specular_coefficients(vec4& value);
        void set_texture_specular_offset(vec4& value);
        void set_texture_transform(int32_t count = 0, texture_transform_struct* value = 0);
        void set_wet_param(float_t value = 0.0f);
    };
}

namespace rndr {
    struct RenderManager {
        bool pass_sw[rndr::RND_PASSID_NUM];
        Shadow* shadow_ptr;
        bool reflect;
        bool refract;
        int32_t reflect_blur_num;
        blur_filter_mode reflect_blur_filter;
        bool sync_gpu;
        double_t cpu_time[rndr::RND_PASSID_NUM];
        double_t gpu_time[rndr::RND_PASSID_NUM];
        time_struct time;
        bool shadow;
        bool opaque_z_sort;
        bool alpha_z_sort;
        bool draw_pass_3d[DRAW_PASS_3D_MAX];
        bool show_ref_map;
        stage_data_reflect_type reflect_type;
        bool clear;
        int32_t tex_index[12]; // Extra for buf
        RenderTexture render_textures[9]; // Extra for buf
        GLuint multisample_framebuffer;
        GLuint multisample_renderbuffer;
        bool multisample;
        int32_t show_vector_flags;
        float_t show_vector_length;
        float_t show_vector_z_offset;
        bool field_2F8;
        std::list<draw_pre_process> pre_process;
        texture* effect_texture;
        int32_t npr_param;
        bool field_31C;
        bool field_31D;
        bool field_31E;
        bool field_31F;
        bool field_320;
        bool npr;
        sss_data sss_data;

        RenderManager();
        ~RenderManager();

        void add_pre_process(int32_t type, void(*func)(void*), void* data);
        void clear_pre_process(int32_t type);
        RenderTexture& get_render_texture(int32_t index);
        void resize(int32_t width, int32_t height);
        void set_effect_texture(texture* value);
        void set_multisample(bool value);
        void set_npr_param(int32_t value);
        void set_pass_sw(rndr::RenderPassID id, bool value);
        void set_reflect(bool value);
        void set_reflect_blur(int32_t reflect_blur_num, blur_filter_mode reflect_blur_filter);
        void set_reflect_resolution_mode(reflect_refract_resolution_mode mode);
        void set_reflect_type(stage_data_reflect_type type);
        void set_refract(bool value);
        void set_refract_resolution_mode(reflect_refract_resolution_mode mode);
        void set_shadow_false();
        void set_shadow_true();

        void render_all(render_context* rctx);

        void render_single_pass(rndr::RenderPassID id, render_context* rctx);

        void render_pass_begin();
        void render_pass_end(rndr::RenderPassID id);

        void pass_shadow(render_context* rctx);
        void pass_ss_sss(render_context* rctx);
        void pass_reflect(render_context* rctx);
        void pass_refract(render_context* rctx);
        void pass_pre_process(render_context* rctx);
        void pass_clear(render_context* rctx);
        void pass_pre_sprite(render_context* rctx);
        void pass_3d(render_context* rctx);
        void pass_show_vector(render_context* rctx);
        void pass_post_process(render_context* rctx);
        void pass_sprite(render_context* rctx);

        void pass_3d_contour(render_context* rctx);
        void pass_sprite_surf(render_context* rctx);
    };
}

struct contour_coef_shader_data {
    vec4 g_contour;
    vec4 g_near_far;
};

struct contour_params_shader_data {
    vec4 g_near_far;
};

struct filter_scene_shader_data {
    vec4 g_transform;
    vec4 g_texcoord;
};

struct esm_filter_batch_shader_data {
    vec4 g_params;
    vec4 g_gauss[2];
};

struct imgfilter_batch_shader_data {
    vec4 g_texture_lod;
    vec4 g_color_scale;
    vec4 g_color_offset;
};

struct glass_eye_batch_shader_data {
    vec4 g_ellipsoid_radius;
    vec4 g_ellipsoid_scale;
    vec4 g_tex_model_param;
    vec4 g_tex_offset;
    vec4 g_eb_radius;
    vec4 g_eb_tex_model_param;
    vec4 g_fresnel;
    vec4 g_refract1;
    vec4 g_refract2;
    vec4 g_iris_radius;
    vec4 g_cornea_radius;
    vec4 g_pupil_radius;
    vec4 g_tex_scale;
};

struct obj_scene_shader_data {
    vec4 g_irradiance_r_transforms[4];
    vec4 g_irradiance_g_transforms[4];
    vec4 g_irradiance_b_transforms[4];
    vec4 g_light_env_stage_diffuse;
    vec4 g_light_env_stage_specular;
    vec4 g_light_env_chara_diffuse;
    vec4 g_light_env_chara_ambient;
    vec4 g_light_env_chara_specular;
    vec4 g_light_env_reflect_diffuse;
    vec4 g_light_env_reflect_ambient;
    vec4 g_light_env_proj_diffuse;
    vec4 g_light_env_proj_specular;
    vec4 g_light_env_proj_position;
    vec4 g_light_stage_dir;
    vec4 g_light_stage_diff;
    vec4 g_light_stage_spec;
    vec4 g_light_chara_dir;
    vec4 g_light_chara_spec;
    vec4 g_light_chara_luce;
    vec4 g_light_chara_back;
    vec4 g_light_face_diff;
    vec4 g_chara_color_rim;
    vec4 g_chara_color0;
    vec4 g_chara_color1;
    vec4 g_chara_f_dir;
    vec4 g_chara_f_ambient;
    vec4 g_chara_f_diffuse;
    vec4 g_chara_tc_param;
    vec4 g_fog_depth_color;
    vec4 g_fog_height_params; //x=density, y=start, z=end, w=1/(end-start)
    vec4 g_fog_height_color;
    vec4 g_fog_bump_params; //x=density, y=start, z=end, w=1/(end-start)
    vec4 g_fog_state_params; //x=density, y=start, z=end, w=1/(end-start)
    vec4 g_normal_tangent_transforms[3];
    vec4 g_esm_param;
    vec4 g_self_shadow_receivers[6];
    vec4 g_shadow_ambient;
    vec4 g_shadow_ambient1;
    vec4 g_framebuffer_size;
    vec4 g_light_reflect_dir;
    vec4 g_clip_plane;
    vec4 g_npr_cloth_spec_color;
    vec4 g_view[3];
    vec4 g_view_inverse[3];
    vec4 g_projection_view[4];
    vec4 g_view_position;
    vec4 g_light_projection[4];

    void set_g_irradiance_r_transforms(const mat4& mat);
    void set_g_irradiance_g_transforms(const mat4& mat);
    void set_g_irradiance_b_transforms(const mat4& mat);
    void set_g_normal_tangent_transforms(const mat4& mat);
    void set_g_self_shadow_receivers(int32_t index, const mat4& mat);
    void set_g_light_projection(const mat4& mat);

    void set_projection_view(const mat4& view, const mat4& proj);
};

struct obj_batch_shader_data {
    vec4 g_transforms[4];
    vec4 g_worlds[3];
    vec4 g_worlds_invtrans[3];
    vec4 g_worldview[3];
    vec4 g_worldview_inverse[3];
    vec4 g_joint[3];
    vec4 g_joint_inverse[3];
    vec4 g_texcoord_transforms[4];
    vec4 g_blend_color;
    vec4 g_offset_color;
    vec4 g_material_state_diffuse;
    vec4 g_material_state_ambient;
    vec4 g_material_state_emission;
    vec4 g_material_state_shininess;
    vec4 g_material_state_specular;
    vec4 g_fresnel_coefficients;
    vec4 g_texture_color_coefficients;
    vec4 g_texture_color_offset;
    vec4 g_texture_specular_coefficients;
    vec4 g_texture_specular_offset;
    vec4 g_shininess;
    vec4 g_max_alpha;
    vec4 g_morph_weight;
    vec4 g_sss_param;
    vec4 g_bump_depth;
    vec4 g_intensity;
    vec4 g_reflect_uv_scale;
    vec4 g_texture_blend;

    void set_g_joint(const mat4& mat);
    void set_g_texcoord_transforms(int32_t index, const mat4& mat);

    void set_transforms(const mat4& model, const mat4& view, const mat4& proj);
};

struct obj_skinning_shader_data {
    vec4 g_joint_transforms[768];
};

struct quad_shader_data {
    vec4 g_texcoord_modifier;
    vec4 g_texel_size;
    vec4 g_color;
    vec4 g_texture_lod;
};

struct sprite_scene_shader_data {
    mat4 g_transform;
    vec4 g_framebuffer_size;
};

struct sss_filter_gaussian_coef_shader_data {
    vec4 g_param;
    vec4 g_coef[64];
};

struct transparency_batch_shader_data {
    vec4 g_opacity;
};

struct render_context {
    ::camera* camera;
    draw_state draw_state;
    mdl::DispManager disp_manager;
    rndr::RenderManager render_manager;

    face face;
    fog fog[FOG_MAX];
    light_proj* litproj;
    light_set light_set[LIGHT_SET_MAX];

    post_process post_process;
    bool chara_reflect;
    bool chara_refract;

    mat4 view_mat;
    mat4 proj_mat;
    mat4 vp_mat;
    vec4 g_near_far;

    mat4 matrix_buffer[MATRIX_BUFFER_COUNT];

    GLuint box_vao;
    GLuint box_vbo;

    GL::UniformBuffer contour_coef_ubo;
    GL::UniformBuffer contour_params_ubo;
    GL::UniformBuffer filter_scene_ubo;
    GL::UniformBuffer esm_filter_batch_ubo;
    GL::UniformBuffer imgfilter_batch_ubo;
    GL::UniformBuffer glass_eye_batch_ubo;
    GL::UniformBuffer quad_ubo;
    GL::UniformBuffer sprite_scene_ubo;
    GL::UniformBuffer sss_filter_gaussian_coef_ubo;
    GL::UniformBuffer transparency_batch_ubo;

    obj_scene_shader_data obj_scene;
    obj_batch_shader_data obj_batch;
    obj_skinning_shader_data obj_skinning;
    GL::UniformBuffer obj_scene_ubo;
    GL::UniformBuffer obj_batch_ubo;
    GL::UniformBuffer obj_skinning_ubo;

    GLuint empty_texture_2d;
    GLuint empty_texture_cube_map;

    GLuint samplers[18];
    GLuint sprite_samplers[3];

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

struct Shadow {
    RenderTexture render_textures[8]; // Extra for buf
    RenderTexture* curr_render_textures[4]; // Extra for buf
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

    Shadow();
    ~Shadow();

    void Ctrl(render_context* rctx);
    int32_t InitData();
    void Reset();
    void ResetData();
};
