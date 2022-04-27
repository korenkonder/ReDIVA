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

struct shader_glsl_param {
    const char* name;
    char* frag;
    char* vert;
    char* param[SHADER_PARAM_NUM_PARAMS];
};

struct shader_glsl {
    std::string name;
    GLuint program;
    std::vector<std::pair<uint64_t, GLint>> uniform;
    std::vector<std::pair<uint64_t, GLint>> uniform_block;

    shader_glsl();
    ~shader_glsl();

    GLint get_uniform_location(GLchar* name);
    void load(const char* vert_data, const char* frag_data,
        const char* geom_data, shader_glsl_param* param);
    void load_file(const char* vert_path, const char* frag_path,
        const char* geom_path, shader_glsl_param* param);
    void load_file(const wchar_t* vert_path, const wchar_t* frag_path,
        const wchar_t* geom_path, shader_glsl_param* param);
    void set_uniform_block_binding(GLchar* name, GLint binding);
    void unload();
    void use();
};


#define shader_glsl_set_bool(s, name, value) \
{ \
    int32_t location = (s)->get_uniform_location((GLchar*)name); \
    if (location != GL_INVALID_INDEX) { \
        (s)->use(); \
        glUniform1i(location, value ? 1 : 0); \
    } \
}

#define shader_glsl_set_int(s, name, value) \
{ \
    int32_t location = (s)->get_uniform_location((GLchar*)name); \
    if (location != GL_INVALID_INDEX) { \
        (s)->use(); \
        glUniform1i(location, value); \
    } \
}

#define shader_glsl_set_float(s, name, value) \
{ \
    int32_t location = (s)->get_uniform_location((GLchar*)name); \
    if (location != GL_INVALID_INDEX) { \
        (s)->use(); \
        glUniform1f(location, value); \
    } \
}

#define shader_glsl_set_vec2i(s, name, value) \
{ \
    int32_t location = (s)->get_uniform_location((GLchar*)name); \
    if (location != GL_INVALID_INDEX) { \
        (s)->use(); \
        glUniform2i(location, (value).x, (value).y); \
    } \
}

#define shader_glsl_set_vec2i_value(s, name, x, y) \
{ \
    int32_t location = (s)->get_uniform_location((GLchar*)name); \
    if (location != GL_INVALID_INDEX) { \
        (s)->use(); \
        glUniform2i(location, (x), (y)); \
    } \
}

#define shader_glsl_set_vec2(s, name, value) \
{ \
    int32_t location = (s)->get_uniform_location((GLchar*)name); \
    if (location != GL_INVALID_INDEX) { \
        (s)->use(); \
        glUniform2f(location, (value).x, (value).y); \
    } \
}

#define shader_glsl_set_vec2_value(s, name, x, y) \
{ \
    int32_t location = (s)->get_uniform_location((GLchar*)name); \
    if (location != GL_INVALID_INDEX) { \
        (s)->use(); \
        glUniform2f(location, (x), (y)); \
    } \
}

#define shader_glsl_set_vec3i(s, name, value) \
{ \
    int32_t location = (s)->get_uniform_location((GLchar*)name); \
    if (location != GL_INVALID_INDEX) { \
        (s)->use(); \
        glUniform3i(location, (value).x, (value).y, (value).z); \
    } \
}

#define shader_glsl_set_vec3i_value(s, name, x, y, z) \
{ \
    int32_t location = (s)->get_uniform_location((GLchar*)name); \
    if (location != GL_INVALID_INDEX) { \
        (s)->use(); \
        glUniform3i(location, (x), (y), (z)); \
    } \
}

#define shader_glsl_set_vec3(s, name, value) \
{ \
    int32_t location = (s)->get_uniform_location((GLchar*)name); \
    if (location != GL_INVALID_INDEX) { \
        (s)->use(); \
        glUniform3f(location, (value).x, (value).y, (value).z); \
    } \
}

#define shader_glsl_set_vec3_value(s, name, x, y, z) \
{ \
    int32_t location = (s)->get_uniform_location((GLchar*)name); \
    if (location != GL_INVALID_INDEX) { \
        (s)->use(); \
        glUniform3f(location, (x), (y), (z)); \
    } \
}

#define shader_glsl_set_vec4i(s, name, value) \
{ \
    int32_t location = (s)->get_uniform_location((GLchar*)name); \
    if (location != GL_INVALID_INDEX) { \
        (s)->use(); \
        glUniform4i(location, (value).x, (value).y, (value).z, (value).w); \
    } \
}

#define shader_glsl_set_vec4i_value(s, name, x, y, z, w) \
{ \
    int32_t location = (s)->get_uniform_location((GLchar*)name); \
    if (location != GL_INVALID_INDEX) { \
        (s)->use(); \
        glUniform4i(location, (x), (y), (z), (w)); \
    } \
}

#define shader_glsl_set_vec4(s, name, value) \
{ \
    int32_t location = (s)->get_uniform_location((GLchar*)name); \
    if (location != GL_INVALID_INDEX) { \
        (s)->use(); \
        glUniform4f(location, (value).x, (value).y, (value).z, (value).w); \
    } \
}

#define shader_glsl_set_vec4_value(s, name, x, y, z, w) \
{ \
    int32_t location = (s)->get_uniform_location((GLchar*)name); \
    if (location != GL_INVALID_INDEX) { \
        (s)->use(); \
        glUniform4f(location, (x), (y), (z), (w)); \
    } \
}

#define shader_glsl_set_mat3(s, name, transpose, value) \
{ \
    int32_t location = (s)->get_uniform_location((GLchar*)name); \
    if (location != GL_INVALID_INDEX) { \
        (s)->use(); \
        glUniformMatrix3fv(location, 1, transpose, (GLfloat*)&(value)); \
    } \
}

#define shader_glsl_set_mat4(s, name, transpose, value) \
{ \
    int32_t location = (s)->get_uniform_location((GLchar*)name); \
    if (location != GL_INVALID_INDEX) { \
        (s)->use(); \
        glUniformMatrix4fv(location, 1, transpose, (GLfloat*)&(value)); \
    } \
}

#define shader_glsl_set_int_array(s, name, count, value) \
{ \
    int32_t location = (s)->get_uniform_location((GLchar*)name); \
    if (location != GL_INVALID_INDEX) { \
        (s)->use(); \
        glUniform1iv(location, count, value); \
    } \
}

#define shader_glsl_set_float_array(s, name, count, value) \
{ \
    int32_t location = (s)->get_uniform_location((GLchar*)name); \
    if (location != GL_INVALID_INDEX) { \
        (s)->use(); \
        glUniform1fv(location, count, value); \
    } \
}

#define shader_glsl_set_vec2i_array(s, name, count, value) \
{ \
    int32_t location = (s)->get_uniform_location((GLchar*)name); \
    if (location != GL_INVALID_INDEX) { \
        (s)->use(); \
        glUniform2iv(location, count, (GLint*)value); \
    } \
}

#define shader_glsl_set_vec2_array(s, name, count, value) \
{ \
    int32_t location = (s)->get_uniform_location((GLchar*)name); \
    if (location != GL_INVALID_INDEX) { \
        (s)->use(); \
        glUniform2fv(location, count, (GLfloat*)value); \
    } \
}

#define shader_glsl_set_vec3i_array(s, name, count, value) \
{ \
    int32_t location = (s)->get_uniform_location((GLchar*)name); \
    if (location != GL_INVALID_INDEX) { \
        (s)->use(); \
        glUniform3iv(location, count, (GLint*)value); \
    } \
}

#define shader_glsl_set_vec3_array(s, name, count, value) \
{ \
    int32_t location = (s)->get_uniform_location((GLchar*)name); \
    if (location != GL_INVALID_INDEX) { \
        (s)->use(); \
        glUniform3fv(location, count, (GLfloat*)value); \
    } \
}

#define shader_glsl_set_vec4i_array(s, name, count, value) \
{ \
    int32_t location = (s)->get_uniform_location((GLchar*)name); \
    if (location != GL_INVALID_INDEX) { \
        (s)->use(); \
        glUniform4iv(location, count, (GLint*)value); \
    } \
}

    #define shader_glsl_set_vec4_array(s, name, count, value) \
{ \
    int32_t location = (s)->get_uniform_location((GLchar*)name); \
    if (location != GL_INVALID_INDEX) { \
        (s)->use(); \
        glUniform4fv(location, count, (GLfloat*)value); \
    } \
}

#define shader_glsl_set_mat3_array(s, name, count, transpose, value) \
{ \
    int32_t location = (s)->get_uniform_location((GLchar*)name); \
    if (location != GL_INVALID_INDEX) { \
        (s)->use(); \
        glUniformMatrix3fv(location, count, transpose, (GLfloat*)value); \
    } \
}

#define shader_glsl_set_mat4_array(s, name, count, transpose, value) \
{ \
    int32_t location = (s)->get_uniform_location((GLchar*)name); \
    if (location != GL_INVALID_INDEX) { \
        (s)->use(); \
        glUniformMatrix4fv(location, count, transpose, (GLfloat*)value); \
    } \
}
