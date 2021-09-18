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
#include <glad/glad.h>

#define SHADER_PARAM_NUM_PARAMS 12

typedef struct shader_glsl_param {
    char* name;
    char* frag;
    char* vert;
    char* param[SHADER_PARAM_NUM_PARAMS];
} shader_glsl_param;

typedef struct shader_glsl {
    string name;
    GLuint program;
    vector_uint64_t uniform_name_buf;
    vector_int32_t uniform_location_buf;
    vector_uint64_t uniform_block_name_buf;
    vector_int32_t uniform_block_index_buf;
} shader_glsl;

extern void shader_glsl_load(shader_glsl* s, farc* f, shader_glsl_param* param);
extern void shader_glsl_load_file(shader_glsl* s, char* vert_path,
    char* frag_path, char* geom_path, shader_glsl_param* param);
extern void shader_glsl_wload_file(shader_glsl* s, wchar_t* vert_path,
    wchar_t* frag_path, wchar_t* geom_path, shader_glsl_param* param);
extern void shader_glsl_load_string(shader_glsl* s, char* vert_data,
    char* frag_data, char* geom_data, shader_glsl_param* param);
extern void shader_glsl_use(shader_glsl* s);
extern GLint shader_glsl_get_uniform_location(shader_glsl* s, GLchar* name);
extern void shader_glsl_set_uniform_block_binding(shader_glsl* s, GLchar* name, GLint binding);
extern void shader_glsl_free(shader_glsl* s);

#define shader_glsl_set_bool(s, name, value) \
{ \
    int32_t location = shader_glsl_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_glsl_use(s); \
        glUniform1i(location, value ? 1 : 0); \
    } \
}

#define shader_glsl_set_int(s, name, value) \
{ \
    int32_t location = shader_glsl_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_glsl_use(s); \
        glUniform1i(location, value); \
    } \
}

#define shader_glsl_set_float(s, name, value) \
{ \
    int32_t location = shader_glsl_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_glsl_use(s); \
        glUniform1f(location, value); \
    } \
}

#define shader_glsl_set_vec2i(s, name, value) \
{ \
    int32_t location = shader_glsl_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_glsl_use(s); \
        glUniform2i(location, (value).x, (value).y); \
    } \
}

#define shader_glsl_set_vec2i_value(s, name, x, y) \
{ \
    int32_t location = shader_glsl_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_glsl_use(s); \
        glUniform2i(location, (x), (y)); \
    } \
}

#define shader_glsl_set_vec2(s, name, value) \
{ \
    int32_t location = shader_glsl_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_glsl_use(s); \
        glUniform2f(location, (value).x, (value).y); \
    } \
}

#define shader_glsl_set_vec2_value(s, name, x, y) \
{ \
    int32_t location = shader_glsl_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_glsl_use(s); \
        glUniform2f(location, (x), (y)); \
    } \
}

#define shader_glsl_set_vec3i(s, name, value) \
{ \
    int32_t location = shader_glsl_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_glsl_use(s); \
        glUniform3i(location, (value).x, (value).y, (value).z); \
    } \
}

#define shader_glsl_set_vec3i_value(s, name, x, y, z) \
{ \
    int32_t location = shader_glsl_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_glsl_use(s); \
        glUniform3i(location, (x), (y), (z)); \
    } \
}

#define shader_glsl_set_vec3(s, name, value) \
{ \
    int32_t location = shader_glsl_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_glsl_use(s); \
        glUniform3f(location, (value).x, (value).y, (value).z); \
    } \
}

#define shader_glsl_set_vec3_value(s, name, x, y, z) \
{ \
    int32_t location = shader_glsl_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_glsl_use(s); \
        glUniform3f(location, (x), (y), (z)); \
    } \
}

#define shader_glsl_set_vec4i(s, name, value) \
{ \
    int32_t location = shader_glsl_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_glsl_use(s); \
        glUniform4i(location, (value).x, (value).y, (value).z, (value).w); \
    } \
}

#define shader_glsl_set_vec4i_value(s, name, x, y, z, w) \
{ \
    int32_t location = shader_glsl_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_glsl_use(s); \
        glUniform4i(location, (x), (y), (z), (w)); \
    } \
}

#define shader_glsl_set_vec4(s, name, value) \
{ \
    int32_t location = shader_glsl_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_glsl_use(s); \
        glUniform4f(location, (value).x, (value).y, (value).z, (value).w); \
    } \
}

#define shader_glsl_set_vec4_value(s, name, x, y, z, w) \
{ \
    int32_t location = shader_glsl_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_glsl_use(s); \
        glUniform4f(location, (x), (y), (z), (w)); \
    } \
}

#define shader_glsl_set_mat3(s, name, transpose, value) \
{ \
    int32_t location = shader_glsl_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_glsl_use(s); \
        glUniformMatrix3fv(location, 1, transpose, (GLfloat*)&(value)); \
    } \
}

#define shader_glsl_set_mat4(s, name, transpose, value) \
{ \
    int32_t location = shader_glsl_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_glsl_use(s); \
        glUniformMatrix4fv(location, 1, transpose, (GLfloat*)&(value)); \
    } \
}

#define shader_glsl_set_int_array(s, name, count, value) \
{ \
    int32_t location = shader_glsl_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_glsl_use(s); \
        glUniform1iv(location, count, value); \
    } \
}

#define shader_glsl_set_float_array(s, name, count, value) \
{ \
    int32_t location = shader_glsl_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_glsl_use(s); \
        glUniform1fv(location, count, value); \
    } \
}

#define shader_glsl_set_vec2i_array(s, name, count, value) \
{ \
    int32_t location = shader_glsl_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_glsl_use(s); \
        glUniform2iv(location, count, (GLint*)value); \
    } \
}

#define shader_glsl_set_vec2_array(s, name, count, value) \
{ \
    int32_t location = shader_glsl_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_glsl_use(s); \
        glUniform2fv(location, count, (GLfloat*)value); \
    } \
}

#define shader_glsl_set_vec3i_array(s, name, count, value) \
{ \
    int32_t location = shader_glsl_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_glsl_use(s); \
        glUniform3iv(location, count, (GLint*)value); \
    } \
}

#define shader_glsl_set_vec3_array(s, name, count, value) \
{ \
    int32_t location = shader_glsl_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_glsl_use(s); \
        glUniform3fv(location, count, (GLfloat*)value); \
    } \
}

#define shader_glsl_set_vec4i_array(s, name, count, value) \
{ \
    int32_t location = shader_glsl_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_glsl_use(s); \
        glUniform4iv(location, count, (GLint*)value); \
    } \
}

    #define shader_glsl_set_vec4_array(s, name, count, value) \
{ \
    int32_t location = shader_glsl_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_glsl_use(s); \
        glUniform4fv(location, count, (GLfloat*)value); \
    } \
}

#define shader_glsl_set_mat3_array(s, name, count, transpose, value) \
{ \
    int32_t location = shader_glsl_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_glsl_use(s); \
        glUniformMatrix3fv(location, count, transpose, (GLfloat*)value); \
    } \
}

#define shader_glsl_set_mat4_array(s, name, count, transpose, value) \
{ \
    int32_t location = shader_glsl_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_glsl_use(s); \
        glUniformMatrix4fv(location, count, transpose, (GLfloat*)value); \
    } \
}
