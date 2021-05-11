/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/char_buffer.h"
#include "../KKdLib/farc.h"
#include "../KKdLib/mat.h"
#include "../KKdLib/vec.h"
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
    wchar_t_buffer name;
    GLuint c_program;
    GLuint g_program;
} shader_model;

typedef struct shader_fbo {
    wchar_t_buffer name;
    GLuint program;
} shader_fbo;

typedef struct shader_model_update {
    char* frag_c;
    char* frag_g;
    char* geom;
    char* vert;
    shader_param param;
} shader_model_update;

extern void shader_fbo_load(shader_fbo* s, farc* f, shader_param* param);
extern void shader_fbo_use(shader_fbo* s);
extern GLint shader_fbo_get_uniform_location(shader_fbo* s, GLchar* name);
extern void shader_fbo_set_uniform_block_binding(shader_fbo* s, GLchar* name, GLint binding);
extern void shader_fbo_free(shader_fbo* s);
extern void shader_model_load(shader_model* s, shader_model_update* upd);
extern void shader_model_c_use(shader_model* s);
extern void shader_model_g_use(shader_model* s);
extern GLint shader_model_c_get_uniform_location(shader_model* s, GLchar* name);
extern GLint shader_model_g_get_uniform_location(shader_model* s, GLchar* name);
extern void shader_model_set_uniform_block_binding(shader_model* s, GLchar* name, GLint binding);
extern void shader_model_free(shader_model* s);

#define shader_fbo_set_bool(s, name, value) \
glUniform1i(shader_fbo_get_uniform_location(s, name), value ? 1 : 0)

#define shader_fbo_set_int(s, name, value) \
glUniform1i(shader_fbo_get_uniform_location(s, name), value)

#define shader_fbo_set_float(s, name, value) \
glUniform1f(shader_fbo_get_uniform_location(s, name), value)

#define shader_fbo_set_vec2i(s, name, x, y) \
glUniform2i(shader_fbo_get_uniform_location(s, name), x, y)

#define shader_fbo_set_vec2(s, name, x, y) \
glUniform2f(shader_fbo_get_uniform_location(s, name), x, y)

#define shader_fbo_set_vec3i(s, name, x, y, z) \
glUniform3i(shader_fbo_get_uniform_location(s, name), x, y, z)

#define shader_fbo_set_vec3(s, name, x, y, z) \
glUniform3f(shader_fbo_get_uniform_location(s, name), x, y, z)

#define shader_fbo_set_vec4i(s, name, x, y, z, w) \
glUniform4i(shader_fbo_get_uniform_location(s, name), x, y, z, w)

#define shader_fbo_set_vec4(s, name, x, y, z, w) \
glUniform4f(shader_fbo_get_uniform_location(s, name), x, y, z, w)

#define shader_fbo_set_mat3(s, name, transpose, mat) \
glUniformMatrix3fv(shader_fbo_get_uniform_location(s, name), 1, transpose, (GLfloat*)mat)

#define shader_fbo_set_mat4(s, name, transpose, mat) \
glUniformMatrix4fv(shader_fbo_get_uniform_location(s, name), 1, transpose, (GLfloat*)mat)

#define shader_fbo_set_int_array(s, name, count, value) \
glUniform1iv(shader_fbo_get_uniform_location(s, name), count, value)

#define shader_fbo_set_float_array(s, name, count, value) \
glUniform1fv(shader_fbo_get_uniform_location(s, name), count, value)

#define shader_fbo_set_vec2i_array(s, name, count, value) \
glUniform2iv(shader_fbo_get_uniform_location(s, name), count, (GLint*)value)

#define shader_fbo_set_vec2_array(s, name, count, value) \
glUniform2fv(shader_fbo_get_uniform_location(s, name), count, (GLfloat*)value)

#define shader_fbo_set_vec3i_array(s, name, count, value) \
glUniform3iv(shader_fbo_get_uniform_location(s, name), count, (GLint*)value)

#define shader_fbo_set_vec3_array(s, name, count, value) \
glUniform3fv(shader_fbo_get_uniform_location(s, name), count, (GLfloat*)value)

#define shader_fbo_set_vec4i_array(s, name, count, value) \
glUniform4iv(shader_fbo_get_uniform_location(s, name), count, (GLint*)value)

#define shader_fbo_set_vec4_array(s, name, count, value) \
glUniform4fv(shader_fbo_get_uniform_location(s, name), count, (GLfloat*)value)

#define shader_fbo_set_mat3_array(s, name, count, transpose, mat) \
glUniformMatrix3fv(shader_fbo_get_uniform_location(s, name), count, transpose, (GLfloat*)mat)

#define shader_fbo_set_mat4_array(s, name, count, transpose, mat) \
glUniformMatrix4fv(shader_fbo_get_uniform_location(s, name), count, transpose, (GLfloat*)mat)

#define shader_model_c_set_bool(s, name, value) \
glUniform1i(shader_model_c_get_uniform_location(s, name), value ? 1 : 0)

#define shader_model_c_set_int(s, name, value) \
glUniform1i(shader_model_c_get_uniform_location(s, name), value)

#define shader_model_c_set_float(s, name, value) \
glUniform1f(shader_model_c_get_uniform_location(s, name), value)

#define shader_model_c_set_vec2i(s, name, x, y) \
glUniform2i(shader_model_c_get_uniform_location(s, name), x, y)

#define shader_model_c_set_vec2(s, name, x, y) \
glUniform2f(shader_model_c_get_uniform_location(s, name), x, y)

#define shader_model_c_set_vec3i(s, name, x, y, z) \
glUniform3i(shader_model_c_get_uniform_location(s, name), x, y, z)

#define shader_model_c_set_vec3(s, name, x, y, z) \
glUniform3f(shader_model_c_get_uniform_location(s, name), x, y, z)

#define shader_model_c_set_vec4i(s, name, x, y, z, w) \
glUniform4i(shader_model_c_get_uniform_location(s, name), x, y, z, w)

#define shader_model_c_set_vec4(s, name, x, y, z, w) \
glUniform4f(shader_model_c_get_uniform_location(s, name), x, y, z, w)

#define shader_model_c_set_mat3(s, name, transpose, mat) \
glUniformMatrix3fv(shader_model_c_get_uniform_location(s, name), 1, transpose, (GLfloat*)mat)

#define shader_model_c_set_mat4(s, name, transpose, mat) \
glUniformMatrix4fv(shader_model_c_get_uniform_location(s, name), 1, transpose, (GLfloat*)mat)

#define shader_model_c_set_int_array(s, name, count, value) \
glUniform1iv(shader_model_c_get_uniform_location(s, name), count, value)

#define shader_model_c_set_float_array(s, name, count, value) \
glUniform1fv(shader_model_c_get_uniform_location(s, name), count, value)

#define shader_model_c_set_vec2i_array(s, name, count, value) \
glUniform2iv(shader_model_c_get_uniform_location(s, name), count, (GLint*)value)

#define shader_model_c_set_vec2_array(s, name, count, value) \
glUniform2fv(shader_model_c_get_uniform_location(s, name), count, (GLfloat*)value)

#define shader_model_c_set_vec3i_array(s, name, count, value) \
glUniform3iv(shader_model_c_get_uniform_location(s, name), count, (GLint*)value)

#define shader_model_c_set_vec3_array(s, name, count, value) \
glUniform3fv(shader_model_c_get_uniform_location(s, name), count, (GLfloat*)value)

#define shader_model_c_set_vec4i_array(s, name, count, value) \
glUniform4iv(shader_model_c_get_uniform_location(s, name), count, (GLint*)value)

#define shader_model_c_set_vec4_array(s, name, count, value) \
glUniform4fv(shader_model_c_get_uniform_location(s, name), count, (GLfloat*)value)

#define shader_model_c_set_mat3_array(s, name, count, transpose, mat) \
glUniformMatrix3fv(shader_model_c_get_uniform_location(s, name), count, transpose, (GLfloat*)mat)

#define shader_model_c_set_mat4_array(s, name, count, transpose, mat) \
glUniformMatrix4fv(shader_model_c_get_uniform_location(s, name), count, transpose, (GLfloat*)mat)

#define shader_model_g_set_bool(s, name, value) \
glUniform1i(shader_model_g_get_uniform_location(s, name), value ? 1 : 0)

#define shader_model_g_set_int(s, name, value) \
glUniform1i(shader_model_g_get_uniform_location(s, name), value)

#define shader_model_g_set_float(s, name, value) \
glUniform1f(shader_model_g_get_uniform_location(s, name), value)

#define shader_model_g_set_vec2i(s, name, x, y) \
glUniform2i(shader_model_g_get_uniform_location(s, name), x, y)

#define shader_model_g_set_vec2(s, name, x, y) \
glUniform2f(shader_model_g_get_uniform_location(s, name), x, y)

#define shader_model_g_set_vec3i(s, name, x, y, z) \
glUniform3i(shader_model_g_get_uniform_location(s, name), x, y, z)

#define shader_model_g_set_vec3(s, name, x, y, z) \
glUniform3f(shader_model_g_get_uniform_location(s, name), x, y, z)

#define shader_model_g_set_vec4i(s, name, x, y, z, w) \
glUniform4i(shader_model_g_get_uniform_location(s, name), x, y, z, w)

#define shader_model_g_set_vec4(s, name, x, y, z, w) \
glUniform4f(shader_model_g_get_uniform_location(s, name), x, y, z, w)

#define shader_model_g_set_mat3(s, name, transpose, mat) \
glUniformMatrix3fv(shader_model_g_get_uniform_location(s, name), 1, transpose, (GLfloat*)mat)

#define shader_model_g_set_mat4(s, name, transpose, mat) \
glUniformMatrix4fv(shader_model_g_get_uniform_location(s, name), 1, transpose, (GLfloat*)mat)

#define shader_model_g_set_int_array(s, name, count, value) \
glUniform1iv(shader_model_g_get_uniform_location(s, name), count, value)

#define shader_model_g_set_float_array(s, name, count, value) \
glUniform1fv(shader_model_g_get_uniform_location(s, name), count, value)

#define shader_model_g_set_vec2i_array(s, name, count, value) \
glUniform2iv(shader_model_g_get_uniform_location(s, name), count, (GLint*)value)

#define shader_model_g_set_vec2_array(s, name, count, value) \
glUniform2fv(shader_model_g_get_uniform_location(s, name), count, (GLfloat*)value)

#define shader_model_g_set_vec3i_array(s, name, count, value) \
glUniform3iv(shader_model_g_get_uniform_location(s, name), count, (GLint*)value)

#define shader_model_g_set_vec3_array(s, name, count, value) \
glUniform3fv(shader_model_g_get_uniform_location(s, name), count, (GLfloat*)value)

#define shader_model_g_set_vec4i_array(s, name, count, value) \
glUniform4iv(shader_model_g_get_uniform_location(s, name), count, (GLint*)value)

#define shader_model_g_set_vec4_array(s, name, count, value) \
glUniform4fv(shader_model_g_get_uniform_location(s, name), count, (GLfloat*)value)

#define shader_model_g_set_mat3_array(s, name, count, transpose, mat) \
glUniformMatrix3fv(shader_model_g_get_uniform_location(s, name), count, transpose, (GLfloat*)mat)

#define shader_model_g_set_mat4_array(s, name, count, transpose, mat) \
glUniformMatrix4fv(shader_model_g_get_uniform_location(s, name), count, transpose, (GLfloat*)mat)
