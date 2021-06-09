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
#define GLEW_STATIC
#include <GLEW/glew.h>

#define SHADER_PARAM_NUM_PARAMS 12

typedef struct shader_param {
    wchar_t* name;
    wchar_t* frag;
    wchar_t* geom;
    wchar_t* vert;
    char* param[SHADER_PARAM_NUM_PARAMS];
} shader_param;

typedef struct shader_model {
    wstring name;
    GLuint program;
    vector_uint64_t uniform_name_buf;
    vector_int32_t uniform_location_buf;
    vector_uint64_t uniform_block_name_buf;
    vector_int32_t uniform_block_index_buf;
} shader_model;

typedef struct shader_fbo {
    wstring name;
    GLuint program;
    vector_uint64_t uniform_name_buf;
    vector_int32_t uniform_location_buf;
    vector_uint64_t uniform_block_name_buf;
    vector_int32_t uniform_block_index_buf;
} shader_fbo;

typedef struct shader_model_data {
    char* frag;
    char* geom;
    char* vert;
    shader_param param;
} shader_model_data;

extern void shader_fbo_load(shader_fbo* s, farc* f, shader_param* param);
extern void shader_fbo_load_file(shader_fbo* s, char* vert_path,
    char* frag_path, char* geom_path, shader_param* param);
extern void shader_fbo_wload_file(shader_fbo* s, wchar_t* vert_path,
    wchar_t* frag_path, wchar_t* geom_path, shader_param* param);
extern void shader_fbo_load_string(shader_fbo* s, char* vert_data,
    char* frag_data, char* geom_data, shader_param* param);
extern void shader_fbo_use(shader_fbo* s);
extern GLint shader_fbo_get_uniform_location(shader_fbo* s, GLchar* name);
extern void shader_fbo_set_uniform_block_binding(shader_fbo* s, GLchar* name, GLint binding);
extern void shader_fbo_free(shader_fbo* s);
extern void shader_model_load(shader_model* s, shader_model_data* upd);
extern void shader_model_use(shader_model* s);
extern GLint shader_model_get_uniform_location(shader_model* s, GLchar* name);
extern void shader_model_set_uniform_block_binding(shader_model* s, GLchar* name, GLint binding);
extern void shader_model_free(shader_model* s);

#define shader_fbo_set_bool(s, name, value) \
{ \
    int32_t location = shader_fbo_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_fbo_use(s); \
        glUniform1i(location, value ? 1 : 0); \
    } \
}

#define shader_fbo_set_int(s, name, value) \
{ \
    int32_t location = shader_fbo_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_fbo_use(s); \
        glUniform1i(location, value); \
    } \
}

#define shader_fbo_set_float(s, name, value) \
{ \
    int32_t location = shader_fbo_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_fbo_use(s); \
        glUniform1f(location, value); \
    } \
}

#define shader_fbo_set_vec2i(s, name, value) \
{ \
    int32_t location = shader_fbo_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_fbo_use(s); \
        glUniform2i(location, (value).x, (value).y); \
    } \
}

#define shader_fbo_set_vec2(s, name, value) \
{ \
    int32_t location = shader_fbo_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_fbo_use(s); \
        glUniform2f(location, (value).x, (value).y); \
    } \
}

#define shader_fbo_set_vec3i(s, name, value) \
{ \
    int32_t location = shader_fbo_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_fbo_use(s); \
        glUniform3i(location, (value).x, (value).y, (value).z); \
    } \
}

#define shader_fbo_set_vec3(s, name, value) \
{ \
    int32_t location = shader_fbo_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_fbo_use(s); \
        glUniform3f(location, (value).x, (value).y, (value).z); \
    } \
}

#define shader_fbo_set_vec4i(s, name, value) \
{ \
    int32_t location = shader_fbo_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_fbo_use(s); \
        glUniform4i(location, (value).x, (value).y, (value).z, (value).w); \
    } \
}

#define shader_fbo_set_vec4(s, name, value) \
{ \
    int32_t location = shader_fbo_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_fbo_use(s); \
        glUniform4f(location, (value).x, (value).y, (value).z, (value).w); \
    } \
}

#define shader_fbo_set_mat3(s, name, transpose, value) \
{ \
    int32_t location = shader_fbo_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_fbo_use(s); \
        glUniformMatrix3fv(location, 1, transpose, (GLfloat*)&(value)); \
    } \
}

#define shader_fbo_set_mat4(s, name, transpose, value) \
{ \
    int32_t location = shader_fbo_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_fbo_use(s); \
        glUniformMatrix4fv(location, 1, transpose, (GLfloat*)&(value)); \
    } \
}

#define shader_fbo_set_int_array(s, name, count, value) \
{ \
    int32_t location = shader_fbo_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_fbo_use(s); \
        glUniform1iv(location, count, value); \
    } \
}

#define shader_fbo_set_float_array(s, name, count, value) \
{ \
    int32_t location = shader_fbo_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_fbo_use(s); \
        glUniform1fv(location, count, value); \
    } \
}

#define shader_fbo_set_vec2i_array(s, name, count, value) \
{ \
    int32_t location = shader_fbo_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_fbo_use(s); \
        glUniform2iv(location, count, (GLint*)value); \
    } \
}

#define shader_fbo_set_vec2_array(s, name, count, value) \
{ \
    int32_t location = shader_fbo_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_fbo_use(s); \
        glUniform2fv(location, count, (GLfloat*)value); \
    } \
}

#define shader_fbo_set_vec3i_array(s, name, count, value) \
{ \
    int32_t location = shader_fbo_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_fbo_use(s); \
        glUniform3iv(location, count, (GLint*)value); \
    } \
}

#define shader_fbo_set_vec3_array(s, name, count, value) \
{ \
    int32_t location = shader_fbo_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_fbo_use(s); \
        glUniform3fv(location, count, (GLfloat*)value); \
    } \
}

#define shader_fbo_set_vec4i_array(s, name, count, value) \
{ \
    int32_t location = shader_fbo_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_fbo_use(s); \
        glUniform4iv(location, count, (GLint*)value); \
    } \
}

    #define shader_fbo_set_vec4_array(s, name, count, value) \
{ \
    int32_t location = shader_fbo_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_fbo_use(s); \
        glUniform4fv(location, count, (GLfloat*)value); \
    } \
}

#define shader_fbo_set_mat3_array(s, name, count, transpose, value) \
{ \
    int32_t location = shader_fbo_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_fbo_use(s); \
        glUniformMatrix3fv(location, count, transpose, (GLfloat*)value); \
    } \
}

#define shader_fbo_set_mat4_array(s, name, count, transpose, value) \
{ \
    int32_t location = shader_fbo_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_fbo_use(s); \
        glUniformMatrix4fv(location, count, transpose, (GLfloat*)value); \
    } \
}

#define shader_model_set_bool(s, name, value) \
{ \
    int32_t location = shader_model_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_model_use(s); \
        glUniform1i(location, value ? 1 : 0); \
    } \
}

#define shader_model_set_int(s, name, value) \
{ \
    int32_t location = shader_model_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_model_use(s); \
        glUniform1i(location, value); \
    } \
}

#define shader_model_set_float(s, name, value) \
{ \
    int32_t location = shader_model_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_model_use(s); \
        glUniform1f(location, value); \
    } \
}

#define shader_model_set_vec2i(s, name, value) \
{ \
    int32_t location = shader_model_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_model_use(s); \
        glUniform2i(location, (value).x, (value).y); \
    } \
}

#define shader_model_set_vec2(s, name, value) \
{ \
    int32_t location = shader_model_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_model_use(s); \
        glUniform2f(location, (value).x, (value).y); \
    } \
}

#define shader_model_set_vec3i(s, name, value) \
{ \
    int32_t location = shader_model_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_model_use(s); \
        glUniform3i(location, (value).x, (value).y, (value).z); \
    } \
}

#define shader_model_set_vec3(s, name, value) \
{ \
    int32_t location = shader_model_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_model_use(s); \
        glUniform3f(location, (value).x, (value).y, (value).z); \
    } \
}

#define shader_model_set_vec4i(s, name, value) \
{ \
    int32_t location = shader_model_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_model_use(s); \
        glUniform4i(location, (value).x, (value).y, (value).z, (value).w); \
    } \
}

#define shader_model_set_vec4(s, name, value) \
{ \
    int32_t location = shader_model_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_model_use(s); \
        glUniform4f(location, (value).x, (value).y, (value).z, (value).w); \
    } \
}

#define shader_model_set_mat3(s, name, transpose, value) \
{ \
    int32_t location = shader_model_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_model_use(s); \
        glUniformMatrix3fv(location, 1, transpose, (GLfloat*)&(value)); \
    } \
}

#define shader_model_set_mat4(s, name, transpose, value) \
{ \
    int32_t location = shader_model_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_model_use(s); \
        glUniformMatrix4fv(location, 1, transpose, (GLfloat*)&(value)); \
    } \
}

#define shader_model_set_int_array(s, name, count, value) \
{ \
    int32_t location = shader_model_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_model_use(s); \
        glUniform1iv(location, count, value); \
    } \
}

#define shader_model_set_float_array(s, name, count, value) \
{ \
    int32_t location = shader_model_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_model_use(s); \
        glUniform1fv(location, count, value); \
    } \
}

#define shader_model_set_vec2i_array(s, name, count, value) \
{ \
    int32_t location = shader_model_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_model_use(s); \
        glUniform2iv(location, count, (GLint*)value); \
    } \
}

#define shader_model_set_vec2_array(s, name, count, value) \
{ \
    int32_t location = shader_model_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_model_use(s); \
        glUniform2fv(location, count, (GLfloat*)value); \
    } \
}

#define shader_model_set_vec3i_array(s, name, count, value) \
{ \
    int32_t location = shader_model_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_model_use(s); \
        glUniform3iv(location, count, (GLint*)value); \
    } \
}

#define shader_model_set_vec3_array(s, name, count, value) \
{ \
    int32_t location = shader_model_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_model_use(s); \
        glUniform3fv(location, count, (GLfloat*)value); \
    } \
}

#define shader_model_set_vec4i_array(s, name, count, value) \
{ \
    int32_t location = shader_model_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_model_use(s); \
        glUniform4iv(location, count, (GLint*)value); \
    } \
}

#define shader_model_set_vec4_array(s, name, count, value) \
{ \
    int32_t location = shader_model_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_model_use(s); \
        glUniform4fv(location, count, (GLfloat*)value); \
    } \
}

#define shader_model_set_mat3_array(s, name, count, transpose, value) \
{ \
    int32_t location = shader_model_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_model_use(s); \
        glUniformMatrix3fv(location, count, transpose, (GLfloat*)value); \
    } \
}

#define shader_model_set_mat4_array(s, name, count, transpose, value) \
{ \
    int32_t location = shader_model_get_uniform_location(s, name); \
    if (location != GL_INVALID_INDEX) { \
        shader_model_use(s); \
        glUniformMatrix4fv(location, count, transpose, (GLfloat*)value); \
    } \
}
