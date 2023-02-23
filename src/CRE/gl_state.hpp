/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include <glad/glad.h>

struct gl_state_struct {
    GLint program;
    GLenum active_texture;
    GLuint active_texture_index;
    GLint texture_binding_2d[32];
    GLint texture_binding_cube_map[32];
    GLint sampler_binding[32];
    GLboolean blend;
    GLenum blend_src_rgb;
    GLenum blend_src_alpha;
    GLenum blend_dst_rgb;
    GLenum blend_dst_alpha;
    GLenum blend_mode_rgb;
    GLenum blend_mode_alpha;
    GLuint read_framebuffer_binding;
    GLuint draw_framebuffer_binding;
    GLuint vertex_array_binding;
    GLuint array_buffer_binding;
    GLuint element_array_buffer_binding;
    GLuint uniform_buffer_binding;
    GLuint uniform_buffer_bindings[14];
    GLintptr uniform_buffer_offsets[14];
    GLsizeiptr uniform_buffer_sizes[14];
    GLuint shader_storage_buffer_binding;
    GLuint shader_storage_buffer_bindings[14];
    GLintptr shader_storage_buffer_offsets[14];
    GLsizeiptr shader_storage_buffer_sizes[14];
    GLboolean color_mask[4];
    GLboolean cull_face;
    GLenum cull_face_mode;
    GLboolean depth_test;
    GLenum depth_func;
    GLboolean depth_mask;
    GLenum polygon_front_face_mode;
    GLenum polygon_back_face_mode;
    GLboolean multisample;
    GLboolean primitive_restart;
    GLuint primitive_restart_index;
    GLboolean scissor_test;
    GLboolean stencil_test;
    GLuint stencil_mask;
};

extern gl_state_struct gl_state;

extern void gl_state_active_bind_texture_2d(int32_t index, GLuint texture);
extern void gl_state_active_bind_texture_cube_map(int32_t index, GLuint texture);
extern void gl_state_active_texture(size_t index);
extern void gl_state_bind_framebuffer(GLuint framebuffer);
extern void gl_state_bind_read_framebuffer(GLuint framebuffer);
extern void gl_state_bind_draw_framebuffer(GLuint framebuffer);
extern void gl_state_bind_vertex_array(GLuint array, bool force = false);
extern void gl_state_bind_array_buffer(GLuint buffer, bool force = false);
extern void gl_state_bind_element_array_buffer(GLuint buffer, bool force = false);
extern void gl_state_bind_uniform_buffer(GLuint buffer, bool force = false);
extern void gl_state_bind_uniform_buffer_base(GLuint index, GLuint buffer, bool force = false);
extern void gl_state_bind_uniform_buffer_range(GLuint index,
    GLuint buffer, GLintptr offset, GLsizeiptr size, bool force = false);
extern void gl_state_bind_shader_storage_buffer(GLuint buffer, bool force = false);
extern void gl_state_bind_shader_storage_buffer_base(GLuint index, GLuint buffer, bool force = false);
extern void gl_state_bind_shader_storage_buffer_range(GLuint index,
    GLuint buffer, GLintptr offset, GLsizeiptr size, bool force = false);
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
extern void gl_state_disable_multisample();
extern void gl_state_disable_primitive_restart();
extern void gl_state_disable_scissor_test();
extern void gl_state_disable_stencil_test();
extern void gl_state_enable_blend();
extern void gl_state_enable_cull_face();
extern void gl_state_enable_depth_test();
extern void gl_state_enable_multisample();
extern void gl_state_enable_primitive_restart();
extern void gl_state_enable_scissor_test();
extern void gl_state_enable_stencil_test();
extern void gl_state_get();
extern void gl_state_get_all_gl_errors();
extern GLenum gl_state_get_error();
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
extern void gl_state_set_primitive_restart_index(GLuint index);
extern void gl_state_set_stencil_mask(GLuint mask);
extern void gl_state_use_program(GLuint program);
