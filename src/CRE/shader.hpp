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
#include "Vulkan/buffer.hpp"
#include "static_var.hpp"

#define SHADER_MAX_LIGHTS 8
#define SHADER_MAX_PALETTE_MATRICES 32
#define SHADER_MAX_PROGRAM_ENV_PARAMETERS 256
#define SHADER_MAX_PROGRAM_LOCAL_PARAMETERS 64
#define SHADER_MAX_PROGRAM_BUFFER_PARAMETERS 1024
#define SHADER_MAX_PROGRAM_MATRICES 8
#define SHADER_MAX_TEXTURE_COORDS 8
#define SHADER_MAX_UNIFORM_BLOCK_SIZE 65536
#define SHADER_MAX_UNIFORM_VALUES 16

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

struct shader_opengl;
struct shader_vulkan;
struct shader_set_data;

typedef std::pair<VkShaderModule, VkShaderModule> shader_vulkan_pair;

typedef int32_t (*PFNSHADERGETINDEXFUNCPROC)(const char* name);

typedef void (*PFNSHADEROPENGLBINDFUNCPROC)(shader_set_data* set, shader_opengl* shad);
typedef shader_vulkan_pair(*PFNSHADERVULKANGETFUNCPROC)(shader_set_data* set, shader_vulkan* shad);

struct shader_opengl_bind_func {
    uint32_t name_index;
    PFNSHADEROPENGLBINDFUNCPROC bind_func;
};

struct shader_vulkan_get_func {
    uint32_t name_index;
    PFNSHADERVULKANGETFUNCPROC get_func;
};

struct shader_opengl_sub_shader {
    GLuint program;
    int32_t uniform_val[SHADER_MAX_UNIFORM_VALUES];
    bool uniform_val_update;
};

struct shader_opengl_sub {
    uint32_t sub_index;
    const int32_t* vp_unival_max;
    const int32_t* fp_unival_max;
    shader_opengl_sub_shader* shaders;
};

struct shader_opengl {
    const char* name;
    uint32_t name_index;
    int32_t num_sub;
    shader_opengl_sub* sub;
    int32_t num_uniform;
    const uniform_name* use_uniform;
    const bool* use_permut;
    PFNSHADEROPENGLBINDFUNCPROC bind_func;

    int32_t bind(shader_set_data* set, uint32_t sub_index);

    static bool parse_define(const char* data, char** temp, size_t* temp_size);
    static bool parse_define(const char* data, int32_t num_uniform,
        int32_t* uniform_value, char** temp, size_t* temp_size);
    static char* parse_include(char* data, farc* f);
    static void unbind();
};

struct shader_opengl_data {
    size_t size;
    shader_opengl* shaders;
    shader_opengl_sub_shader* curr_shader;
    GLboolean primitive_restart;
    GLuint primitive_restart_index;

    PFNSHADERGETINDEXFUNCPROC get_index_by_name;

    shader_opengl_data(farc* f, bool ignore_cache,
        const char* name, const shader_table* shaders_table, const size_t size,
        const shader_opengl_bind_func* bind_func_table, const size_t bind_func_table_size,
        PFNSHADERGETINDEXFUNCPROC get_index_by_name);
    ~shader_opengl_data();
};

struct shader_vulkan_sub_shader {
    VkShaderModule shader_module;
    int32_t uniform_val[SHADER_MAX_UNIFORM_VALUES];
    bool uniform_val_update;
};

struct shader_vulkan_sub {
    uint32_t sub_index;
    const int32_t* vp_unival_max;
    const int32_t* fp_unival_max;
    shader_vulkan_sub_shader* vp;
    shader_vulkan_sub_shader* fp;
};

struct shader_vulkan {
    const char* name;
    uint32_t name_index;
    int32_t num_sub;
    shader_vulkan_sub* sub;
    int32_t num_uniform;
    const uniform_name* use_uniform;
    const bool* use_permut;
    PFNSHADERVULKANGETFUNCPROC get_func;

    shader_vulkan_pair get(shader_set_data* set, uint32_t sub_index);
};

struct shader_vulkan_data {
    size_t size;
    shader_vulkan* shaders;
    shader_vulkan_sub_shader* curr_vp;
    shader_vulkan_sub_shader* curr_fp;
    VkBool32 primitive_restart;

    int32_t uniform_val[SHADER_MAX_UNIFORM_VALUES];

    VkDevice device;
    VmaAllocator allocator;

    PFNSHADERGETINDEXFUNCPROC get_index_by_name;

    shader_vulkan_data(VkDevice device, VmaAllocator allocator, farc* f,
        const shader_table* shaders_table, const size_t size,
        const shader_vulkan_get_func* get_func_table, const size_t get_func_table_size,
        PFNSHADERGETINDEXFUNCPROC get_index_by_name);
    ~shader_vulkan_data();
};

struct shader_set_data {
    shader_opengl_data* opengl_data;
    shader_vulkan_data* vulkan_data;

    shader_set_data();

    void disable_primitive_restart();
    void draw_arrays(GLenum mode, GLint first, GLsizei count);
    void draw_elements(GLenum mode,
        GLsizei count, GLenum type, const void* indices);
    void draw_range_elements(GLenum mode,
        GLuint start, GLuint end, GLsizei count, GLenum type, const void* indices);
    void enable_primitive_restart();
    int32_t get_index_by_name(const char* name);
    std::pair<VkShaderModule, VkShaderModule> get_vulkan_shader(uint32_t index);
    void load_opengl(farc* f, bool ignore_cache, const char* name,
        const shader_table* shaders_table, const size_t size,
        const shader_opengl_bind_func* bind_func_table, const size_t bind_func_table_size,
        PFNSHADERGETINDEXFUNCPROC get_index_by_name);
    void load_vulkan(VkDevice device, VmaAllocator allocator, farc* f,
        const shader_table* shaders_table, const size_t size,
        const shader_vulkan_get_func* get_func_table, const size_t get_func_table_size,
        PFNSHADERGETINDEXFUNCPROC get_index_by_name);
    void set_opengl_shader(uint32_t index);
    void unload_opengl();
    void unload_vulkan();
};
