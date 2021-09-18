/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include <glad/glad.h>

extern void gl_state_active_bind_texture_2d(int32_t index, GLuint texture);
extern void gl_state_active_bind_texture_cube_map(int32_t index, GLuint texture);
extern void gl_state_active_texture(size_t index);
extern void gl_state_bind_framebuffer(GLuint framebuffer);
extern void gl_state_bind_vertex_array(GLuint array);
extern void gl_state_bind_uniform_buffer(GLuint buffer);
extern void gl_state_bind_uniform_buffer_base(GLuint index, GLuint buffer);
extern void gl_state_bind_uniform_buffer_range(GLuint index,
    GLuint buffer, GLintptr offset, GLsizeiptr size);
extern void gl_state_bind_shader_storage_buffer(GLuint buffer);
extern void gl_state_bind_shader_storage_buffer_base(GLuint index, GLuint buffer);
extern void gl_state_bind_shader_storage_buffer_range(GLuint index,
    GLuint buffer, GLintptr offset, GLsizeiptr size);
extern void gl_state_bind_texture_2d(GLuint texture);
extern void gl_state_bind_texture_cube_map(GLuint texture);
extern void gl_state_bind_sampler(int32_t index, GLuint sampler);
extern bool gl_state_check_uniform_buffer_binding();
extern bool gl_state_check_uniform_buffer_binding_base(size_t index);
extern bool gl_state_check_shader_storage_buffer_binding();
extern bool gl_state_check_shader_storage_buffer_binding_base(size_t index);
extern bool gl_state_check_texture_binding_2d(size_t index);
extern bool gl_state_check_texture_binding_cube_map(size_t index);
extern bool gl_state_check_sampler_binding(int32_t index, GLuint sampler);
extern void gl_state_disable_blend();
extern void gl_state_disable_cull_face();
extern void gl_state_disable_depth_test();
extern void gl_state_disable_stencil_test();
extern void gl_state_enable_blend();
extern void gl_state_enable_cull_face();
extern void gl_state_enable_depth_test();
extern void gl_state_enable_stencil_test();
extern void gl_state_get();
extern void gl_state_get_all_gl_errors();
extern GLuint gl_state_get_program();
extern void gl_state_set_blend_func(GLenum src, GLenum dst);
extern void gl_state_set_blend_func_separate(GLenum src_rgb, GLenum dst_rgb,
    GLenum src_alpha, GLenum dst_alpha);
extern void gl_state_set_blend_equation(GLenum mode);
extern void gl_state_set_blend_equation_separate(GLenum mode_rgb, GLenum mode_alpha);
extern void gl_state_set_color_mask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
extern void gl_state_set_cull_face_mode(GLenum mode);
extern void gl_state_set_depth_func(GLenum func);
extern void gl_state_set_depth_mask(GLboolean flag);
extern void gl_state_set_polygon_mode(GLenum face, GLenum mode);
extern void gl_state_set_stencil_mask(GLuint mask);
extern void gl_state_use_program(GLuint program);
