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

enum shader_description_type {
    SHADER_DESCRIPTION_NONE = 0,
    SHADER_DESCRIPTION_VERTEX_INPUT,
    SHADER_DESCRIPTION_SAMPLER,
    SHADER_DESCRIPTION_UNIFORM,
    SHADER_DESCRIPTION_STORAGE,
    SHADER_DESCRIPTION_FRAGMENT_OUTPUT,
    SHADER_DESCRIPTION_MAX,
    SHADER_DESCRIPTION_END = -1,
};

enum shader_description_storage {
    SHADER_DESCRIPTION_STORAGE_SIZE_MASK   = 0xFFFFFFF << 0,

    SHADER_DESCRIPTION_STORAGE_READ_ONLY   = 0x00 << 28,
    SHADER_DESCRIPTION_STORAGE_WRITE_ONLY  = 0x01 << 28,
    SHADER_DESCRIPTION_STORAGE_READ_WRITE  = 0x02 << 28,
    SHADER_DESCRIPTION_STORAGE_ACCESS_MASK = 0x03 << 28,
};

enum shader_description_uniform {
    SHADER_DESCRIPTION_UNIFORM_SIZE_MASK   = 0x1FFFFF << 0,

    SHADER_DESCRIPTION_UNIFORM_READ_ONLY   = 0x00 << 21,
    SHADER_DESCRIPTION_UNIFORM_WRITE_ONLY  = 0x01 << 21,
    SHADER_DESCRIPTION_UNIFORM_READ_WRITE  = 0x02 << 21,
    SHADER_DESCRIPTION_UNIFORM_ACCESS_MASK = 0x03 << 21,
};

struct shader_description {
    shader_description_type type;
    int32_t binding;
    int32_t data;
    uniform_name use_uniform;
};

struct shader_sub_table {
    uint32_t sub_index;
    const int32_t* vp_unival_max;
    const int32_t* fp_unival_max;
    const char* vp;
    const char* fp;
    const shader_description* vp_desc;
    const shader_description* fp_desc;
};

struct shader_table {
    const char* name;
    uint32_t name_index;
    int32_t num_sub;
    const shader_sub_table* sub;
    int32_t num_uniform;
    const uniform_name* use_uniform;
};

struct shader;
struct shader_set_data;

typedef int32_t (*PFNSHADERGETINDEXFUNCPROC)(const char* name);
typedef const char* (*PFNSHADERGETNAMEFUNCPROC)(int32_t index);

typedef void (*PFNSHADERBINDFUNCPROC)(struct p_gl_rend_state& p_gl_rend_st,
    struct uniform_value& shader_flags, shader_set_data* set, shader* shad);

struct shader_bind_func {
    uint32_t name_index;
    PFNSHADERBINDFUNCPROC bind_func;
};

struct shader_sub {
    uint32_t sub_index;
    const int32_t* vp_unival_max;
    const int32_t* fp_unival_max;
    GLuint* programs;
    const shader_description* fp_desc;
    const shader_description* vp_desc;
};

struct shader {
    const char* name;
    uint32_t name_index;
    int32_t num_sub;
    shader_sub* sub;
    int32_t num_uniform;
    const uniform_name* use_uniform;
    PFNSHADERBINDFUNCPROC bind_func;

    int32_t bind(struct p_gl_rend_state& p_gl_rend_st,
        const struct uniform_value& shader_flags, shader_set_data* set, uint32_t sub_index);

    static bool parse_define(const char* data, std::string& temp, bool vulkan = false);
    static bool parse_define(const char* data, int32_t num_uniform,
        int32_t* uniform_value, std::string& temp, bool vulkan = false);
    static char* parse_include(char* data, farc* f);
    static void unbind(struct p_gl_rend_state& p_gl_rend_st);
};

struct shader_set_data {
    size_t size;
    shader* shaders;

    PFNSHADERGETINDEXFUNCPROC get_index_by_name_func;
    PFNSHADERGETNAMEFUNCPROC get_name_by_index_func;

    shader_set_data();

    int32_t get_index_by_name(const char* name);
    const char* get_name_by_index(int32_t index);
    void load(farc* f, bool ignore_cache, const char* name,
        const shader_table* shaders_table, const size_t size,
        const shader_bind_func* bind_func_table, const size_t bind_func_table_size,
        PFNSHADERGETINDEXFUNCPROC get_index_by_name, PFNSHADERGETNAMEFUNCPROC get_name_by_index);
    void set(struct p_gl_rend_state& p_gl_rend_st, struct uniform_value& shader_flags, uint32_t index);
    void unload();
};
