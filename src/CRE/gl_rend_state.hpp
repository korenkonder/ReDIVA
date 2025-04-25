/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "gl.hpp"

constexpr int32_t GL_REND_STATE_COUNT = 4;

struct gl_rend_state_rect {
    GLint x;
    GLint y;
    GLsizei width;
    GLsizei height;
};

struct p_gl_rend_state {
    struct gl_rend_state& ptr;

    p_gl_rend_state(int32_t index);

    void active_bind_texture_2d(int32_t index, GLuint texture, bool force = false);
    void active_bind_texture_cube_map(int32_t index, GLuint texture, bool force = false);
    void active_texture(size_t index, bool force = false);
    void begin_event(const char* message, int32_t length);
    void bind_framebuffer(GLuint framebuffer, bool force = false);
    void bind_read_framebuffer(GLuint framebuffer, bool force = false);
    void bind_draw_framebuffer(GLuint framebuffer, bool force = false);
    void bind_vertex_array(GLuint array, bool force = false);
    void bind_array_buffer(GLuint buffer, bool force = false);
    void bind_element_array_buffer(GLuint buffer, bool force = false);
    void bind_uniform_buffer(GLuint buffer, bool force = false);
    void bind_uniform_buffer_base(GLuint index, GLuint buffer, bool force = false);
    void bind_uniform_buffer_range(GLuint index,
        GLuint buffer, GLintptr offset, GLsizeiptr size, bool force = false);
    void bind_shader_storage_buffer(GLuint buffer, bool force = false);
    void bind_shader_storage_buffer_base(GLuint index, GLuint buffer, bool force = false);
    void bind_shader_storage_buffer_range(GLuint index,
        GLuint buffer, GLintptr offset, GLsizeiptr size, bool force = false);
    void bind_texture_2d(GLuint texture, bool force = false);
    void bind_texture_cube_map(GLuint texture, bool force = false);
    void bind_sampler(int32_t index, GLuint sampler, bool force = false);
    bool check_uniform_buffer_binding();
    bool check_uniform_buffer_binding_base(size_t index);
    bool check_shader_storage_buffer_binding();
    bool check_shader_storage_buffer_binding_base(size_t index);
    bool check_texture_binding_2d(size_t index);
    bool check_texture_binding_cube_map(size_t index);
    bool check_sampler_binding(int32_t index, GLuint sampler);
    void disable_blend(bool force = false);
    void disable_cull_face(bool force = false);
    void disable_depth_test(bool force = false);
    void disable_multisample(bool force = false);
    void disable_primitive_restart(bool force = false);
    void disable_scissor_test(bool force = false);
    void disable_stencil_test(bool force = false);
    void draw_arrays(GLenum mode, GLint first, GLsizei count);
    void draw_elements(GLenum mode,
        GLsizei count, GLenum type, const void* indices);
    void draw_range_elements(GLenum mode,
        GLuint start, GLuint end, GLsizei count, GLenum type, const void* indices);
    void enable_blend(bool force = false);
    void enable_cull_face(bool force = false);
    void enable_depth_test(bool force = false);
    void enable_multisample(bool force = false);
    void enable_primitive_restart(bool force = false);
    void enable_scissor_test(bool force = false);
    void enable_stencil_test(bool force = false);
    void end_event();
    void get();
    GLuint get_program();
    gl_rend_state_rect get_scissor();
    void get_scissor(GLint& x, GLint& y, GLsizei& width, GLsizei& height);
    gl_rend_state_rect get_viewport();
    void get_viewport(GLint& x, GLint& y, GLsizei& width, GLsizei& height);
    void set_blend_func(GLenum src, GLenum dst, bool force = false);
    void set_blend_func_separate(GLenum src_rgb, GLenum dst_rgb,
        GLenum src_alpha, GLenum dst_alpha, bool force = false);
    void set_blend_equation(GLenum mode, bool force = false);
    void set_blend_equation_separate(
        GLenum mode_rgb, GLenum mode_alpha, bool force = false);
    void set_color_mask(GLboolean red, GLboolean green,
        GLboolean blue, GLboolean alpha, bool force = false);
    void set_cull_face_mode(GLenum mode, bool force = false);
    void set_depth_func(GLenum func, bool force = false);
    void set_depth_mask(GLboolean flag, bool force = false);
    void set_line_width(GLfloat width, bool force = false);
    void set_polygon_mode(GLenum face, GLenum mode, bool force = false);
    void set_primitive_restart_index(GLuint index, bool force = false);
    void set_scissor(const gl_rend_state_rect& rect, bool force = false);
    void set_scissor(GLint x, GLint y, GLsizei width, GLsizei height, bool force = false);
    void set_stencil_func(GLenum func, GLint ref, GLuint mask, bool force = false);
    void set_stencil_mask(GLuint mask, bool force = false);
    void set_stencil_op(GLenum sfail, GLenum dpfail, GLenum dppass, bool force = false);
    void set_viewport(const gl_rend_state_rect& rect, bool force = false);
    void set_viewport(GLint x, GLint y, GLsizei width, GLsizei height, bool force = false);
    void use_program(GLuint program, bool force = false);

    template <size_t size>
    inline void begin_event(const char(&str)[size]) {
        begin_event(str, (GLsizei)(size - 1));
    }
};
