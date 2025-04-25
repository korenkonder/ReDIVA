/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "gl_rend_state.hpp"

struct gl_rend_state {
    GLuint program;
    GLuint active_texture_index;
    GLuint texture_binding_2d[32];
    GLuint texture_binding_cube_map[32];
    GLuint sampler_binding[32];
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
    GLfloat line_width;
    GLenum polygon_mode;
    GLboolean multisample;
    GLboolean primitive_restart;
    GLuint primitive_restart_index;
    gl_rend_state_rect scissor_box;
    GLboolean scissor_test;
    GLboolean stencil_test;
    GLenum stencil_func;
    GLenum stencil_fail;
    GLenum stencil_dpfail;
    GLenum stencil_dppass;
    GLuint stencil_mask;
    GLint stencil_ref;
    GLuint stencil_value_mask;
    gl_rend_state_rect viewport;

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
};

gl_rend_state gl_state_data[GL_REND_STATE_COUNT];

p_gl_rend_state::p_gl_rend_state(gl_rend_state_index index) : ptr(gl_state_data[index]) {

}

void p_gl_rend_state::active_bind_texture_2d(int32_t index, GLuint texture, bool force) {
    ptr.active_bind_texture_2d(index, texture, force);
}

void p_gl_rend_state::active_bind_texture_cube_map(int32_t index, GLuint texture, bool force) {
    ptr.active_bind_texture_cube_map(index, texture, force);
}

void p_gl_rend_state::active_texture(size_t index, bool force) {
    ptr.active_texture(index, force);
}

void p_gl_rend_state::begin_event(const char* message, int32_t length) {
    ptr.begin_event(message, length);
}

void p_gl_rend_state::bind_framebuffer(GLuint framebuffer, bool force) {
    ptr.bind_framebuffer(framebuffer, force);
}

void p_gl_rend_state::bind_read_framebuffer(GLuint framebuffer, bool force) {
    ptr.bind_read_framebuffer(framebuffer, force);
}

void p_gl_rend_state::bind_draw_framebuffer(GLuint framebuffer, bool force) {
    ptr.bind_draw_framebuffer(framebuffer, force);
}

void p_gl_rend_state::bind_vertex_array(GLuint array, bool force) {
    ptr.bind_vertex_array(array, force);
}

void p_gl_rend_state::bind_array_buffer(GLuint buffer, bool force) {
    ptr.bind_array_buffer(buffer, force);
}

void p_gl_rend_state::bind_element_array_buffer(GLuint buffer, bool force) {
    ptr.bind_element_array_buffer(buffer, force);
}

void p_gl_rend_state::bind_uniform_buffer(GLuint buffer, bool force) {
    ptr.bind_uniform_buffer(buffer, force);
}

void p_gl_rend_state::bind_uniform_buffer_base(GLuint index, GLuint buffer, bool force) {
    ptr.bind_uniform_buffer_base(index, buffer, force);
}

void p_gl_rend_state::bind_uniform_buffer_range(GLuint index,
    GLuint buffer, GLintptr offset, GLsizeiptr size, bool force) {
    ptr.bind_uniform_buffer_range(index, buffer, offset, size, force);
}

void p_gl_rend_state::bind_shader_storage_buffer(GLuint buffer, bool force) {
    ptr.bind_shader_storage_buffer(buffer, force);
}

void p_gl_rend_state::bind_shader_storage_buffer_base(GLuint index, GLuint buffer, bool force) {
    ptr.bind_shader_storage_buffer_base(index, buffer, force);
}

void p_gl_rend_state::bind_shader_storage_buffer_range(GLuint index,
    GLuint buffer, GLintptr offset, GLsizeiptr size, bool force) {
    ptr.bind_shader_storage_buffer_range(index, buffer, offset, size, force);
}

void p_gl_rend_state::bind_texture_2d(GLuint texture, bool force) {
    ptr.bind_texture_2d(texture, force);
}

void p_gl_rend_state::bind_texture_cube_map(GLuint texture, bool force) {
    ptr.bind_texture_cube_map(texture, force);
}

void p_gl_rend_state::bind_sampler(int32_t index, GLuint sampler, bool force) {
    ptr.bind_sampler(index, sampler, force);
}

bool p_gl_rend_state::check_uniform_buffer_binding() {
    return ptr.check_uniform_buffer_binding();
}

bool p_gl_rend_state::check_uniform_buffer_binding_base(size_t index) {
    return ptr.check_uniform_buffer_binding_base(index);
}

bool p_gl_rend_state::check_shader_storage_buffer_binding() {
    return ptr.check_shader_storage_buffer_binding();
}

bool p_gl_rend_state::check_shader_storage_buffer_binding_base(size_t index) {
    return ptr.check_shader_storage_buffer_binding_base(index);
}

bool p_gl_rend_state::check_texture_binding_2d(size_t index) {
    return ptr.check_texture_binding_2d(index);
}

bool p_gl_rend_state::check_texture_binding_cube_map(size_t index) {
    return ptr.check_texture_binding_cube_map(index);
}

bool p_gl_rend_state::check_sampler_binding(int32_t index, GLuint sampler) {
    return ptr.check_sampler_binding(index, sampler);
}

void p_gl_rend_state::disable_blend(bool force) {
    ptr.disable_blend(force);
}

void p_gl_rend_state::disable_cull_face(bool force) {
    ptr.disable_cull_face(force);
}

void p_gl_rend_state::disable_depth_test(bool force) {
    ptr.disable_depth_test(force);
}

void p_gl_rend_state::disable_multisample(bool force) {
    ptr.disable_multisample(force);
}

void p_gl_rend_state::disable_primitive_restart(bool force) {
    ptr.disable_primitive_restart(force);
}

void p_gl_rend_state::disable_scissor_test(bool force) {
    ptr.disable_scissor_test(force);
}

void p_gl_rend_state::disable_stencil_test(bool force) {
    ptr.disable_stencil_test(force);
}

void p_gl_rend_state::draw_arrays(GLenum mode, GLint first, GLsizei count) {
    glDrawArrays(mode, first, count);
}

void p_gl_rend_state::draw_elements(GLenum mode,
    GLsizei count, GLenum type, const void* indices) {
    switch (mode) {
    case GL_TRIANGLE_STRIP:
        uint32_t index;
        switch (type) {
        case GL_UNSIGNED_BYTE:
            index = 0xFF;
            break;
        case GL_UNSIGNED_SHORT:
            index = 0xFFFF;
            break;
        case GL_UNSIGNED_INT:
        default:
            index = 0xFFFFFFFF;
            break;
        }

        enable_primitive_restart();
        set_primitive_restart_index(index);
        break;
    }

    glDrawElements(mode, count, type, indices);

    switch (mode) {
    case GL_TRIANGLE_STRIP:
        disable_primitive_restart();
        break;
    }
}

void p_gl_rend_state::draw_range_elements(GLenum mode,
    GLuint start, GLuint end, GLsizei count, GLenum type, const void* indices) {
    switch (mode) {
    case GL_TRIANGLE_STRIP:
        uint32_t index;
        switch (type) {
        case GL_UNSIGNED_BYTE:
            index = 0xFF;
            break;
        case GL_UNSIGNED_SHORT:
            index = 0xFFFF;
            break;
        case GL_UNSIGNED_INT:
        default:
            index = 0xFFFFFFFF;
            break;
        }

        enable_primitive_restart();
        set_primitive_restart_index(index);
        break;
    }

    glDrawRangeElements(mode, start, end, count, type, indices);

    switch (mode) {
    case GL_TRIANGLE_STRIP:
        disable_primitive_restart();
        break;
    }
}

void p_gl_rend_state::enable_blend(bool force) {
    ptr.enable_blend(force);
}

void p_gl_rend_state::enable_cull_face(bool force) {
    ptr.enable_cull_face(force);
}

void p_gl_rend_state::enable_depth_test(bool force) {
    ptr.enable_depth_test(force);
}

void p_gl_rend_state::enable_multisample(bool force) {
    ptr.enable_multisample(force);
}

void p_gl_rend_state::enable_primitive_restart(bool force) {
    ptr.enable_primitive_restart(force);
}

void p_gl_rend_state::enable_scissor_test(bool force) {
    ptr.enable_scissor_test(force);
}

void p_gl_rend_state::enable_stencil_test(bool force) {
    ptr.enable_stencil_test(force);
}

void p_gl_rend_state::end_event() {
    ptr.end_event();
}

void p_gl_rend_state::get() {
    ptr.get();
}

GLuint p_gl_rend_state::get_program() {
    return ptr.get_program();
}

gl_rend_state_rect p_gl_rend_state::get_scissor() {
    return ptr.get_scissor();
}

void p_gl_rend_state::get_scissor(GLint& x, GLint& y, GLsizei& width, GLsizei& height) {
    ptr.get_scissor(x, y, width, height);
}

gl_rend_state_rect p_gl_rend_state::get_viewport() {
    return ptr.get_viewport();
}

void p_gl_rend_state::get_viewport(GLint& x, GLint& y, GLsizei& width, GLsizei& height) {
    ptr.get_viewport(x, y, width, height);
}

void p_gl_rend_state::set_blend_func(GLenum src, GLenum dst, bool force) {
    ptr.set_blend_func(src, dst, force);
}

void p_gl_rend_state::set_blend_func_separate(GLenum src_rgb, GLenum dst_rgb,
    GLenum src_alpha, GLenum dst_alpha, bool force) {
    ptr.set_blend_func_separate(src_rgb, dst_rgb, src_alpha, dst_alpha, force);
}

void p_gl_rend_state::set_blend_equation(GLenum mode, bool force) {
    ptr.set_blend_equation(mode, force);
}

void p_gl_rend_state::set_blend_equation_separate(
    GLenum mode_rgb, GLenum mode_alpha, bool force) {
    ptr.set_blend_equation_separate(mode_rgb, mode_alpha, force);
}

void p_gl_rend_state::set_color_mask(GLboolean red, GLboolean green,
    GLboolean blue, GLboolean alpha, bool force) {
    ptr.set_color_mask(red, green, blue, alpha, force);
}

void p_gl_rend_state::set_cull_face_mode(GLenum mode, bool force) {
    ptr.set_cull_face_mode(mode, force);
}

void p_gl_rend_state::set_depth_func(GLenum func, bool force) {
    ptr.set_depth_func(func, force);
}

void p_gl_rend_state::set_depth_mask(GLboolean flag, bool force) {
    ptr.set_depth_mask(flag, force);
}

void p_gl_rend_state::set_line_width(GLfloat width, bool force) {
    ptr.set_line_width(width, force);
}

void p_gl_rend_state::set_polygon_mode(GLenum face, GLenum mode, bool force) {
    ptr.set_polygon_mode(face, mode, force);
}

void p_gl_rend_state::set_primitive_restart_index(GLuint index, bool force) {
    ptr.set_primitive_restart_index(index, force);
}

void p_gl_rend_state::set_scissor(const gl_rend_state_rect& rect, bool force) {
    ptr.set_scissor(rect, force);
}

void p_gl_rend_state::set_scissor(GLint x, GLint y, GLsizei width, GLsizei height, bool force) {
    ptr.set_scissor(x, y, width, height, force);
}

void p_gl_rend_state::set_stencil_func(GLenum func, GLint ref, GLuint mask, bool force) {
    ptr.set_stencil_func(func, ref, mask, force);
}

void p_gl_rend_state::set_stencil_mask(GLuint mask, bool force) {
    ptr.set_stencil_mask(mask, force);
}

void p_gl_rend_state::set_stencil_op(GLenum sfail, GLenum dpfail, GLenum dppass, bool force) {
    ptr.set_stencil_op(sfail, dpfail, dppass, force);
}

void p_gl_rend_state::set_viewport(const gl_rend_state_rect& rect, bool force) {
    ptr.set_viewport(rect, force);
}

void p_gl_rend_state::set_viewport(GLint x, GLint y, GLsizei width, GLsizei height, bool force) {
    ptr.set_viewport(x, y, width, height, force);
}

void p_gl_rend_state::use_program(GLuint program, bool force) {
    ptr.use_program(program, force);
}

void gl_rend_state::active_bind_texture_2d(int32_t index, GLuint texture, bool force) {
    if (force || texture_binding_2d[index] != texture) {
        if (force || active_texture_index != index) {
            glActiveTexture((GLenum)(GL_TEXTURE0 + index));
            active_texture_index = (GLuint)index;
        }
        glBindTexture(GL_TEXTURE_2D, texture);
        texture_binding_2d[index] = texture;
    }
}

void gl_rend_state::active_bind_texture_cube_map(int32_t index, GLuint texture, bool force) {
    if (force || texture_binding_cube_map[index] != texture) {
        if (force || active_texture_index != index) {
            glActiveTexture((GLenum)(GL_TEXTURE0 + index));
            active_texture_index = (GLuint)index;
        }
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        texture_binding_cube_map[index] = texture;
    }
}

void gl_rend_state::active_texture(size_t index, bool force) {
    if (force || active_texture_index != index) {
        glActiveTexture((GLenum)(GL_TEXTURE0 + index));
        active_texture_index = (GLuint)index;
    }
}

void gl_rend_state::begin_event(const char* message, int32_t length) {
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, (GLsizei)length, message);
}

void gl_rend_state::bind_framebuffer(GLuint framebuffer, bool force) {
    if (force || read_framebuffer_binding != framebuffer
        || draw_framebuffer_binding != framebuffer) {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        read_framebuffer_binding = framebuffer;
        draw_framebuffer_binding = framebuffer;
    }
}

void gl_rend_state::bind_read_framebuffer(GLuint framebuffer, bool force) {
    if (force || read_framebuffer_binding != framebuffer) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
        read_framebuffer_binding = framebuffer;
    }
}

void gl_rend_state::bind_draw_framebuffer(GLuint framebuffer, bool force) {
    if (force || draw_framebuffer_binding != framebuffer) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
        draw_framebuffer_binding = framebuffer;
    }
}

void gl_rend_state::bind_vertex_array(GLuint array, bool force) {
    if (force || vertex_array_binding != array) {
        glBindVertexArray(array);
        vertex_array_binding = array;
    }
}

void gl_rend_state::bind_array_buffer(GLuint buffer, bool force) {
    if (force || array_buffer_binding != buffer) {
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        array_buffer_binding = buffer;
    }
}

void gl_rend_state::bind_element_array_buffer(GLuint buffer, bool force) {
    if (force || element_array_buffer_binding != buffer) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
        element_array_buffer_binding = buffer;
    }
}

void gl_rend_state::bind_uniform_buffer(GLuint buffer, bool force) {
    if (force || uniform_buffer_binding != buffer) {
        glBindBuffer(GL_UNIFORM_BUFFER, buffer);
        uniform_buffer_binding = buffer;
    }
}

void gl_rend_state::bind_uniform_buffer_base(GLuint index, GLuint buffer, bool force) {
    if (force || uniform_buffer_bindings[index] != buffer) {
        glBindBufferBase(GL_UNIFORM_BUFFER, index, buffer);
        uniform_buffer_binding = buffer;
        uniform_buffer_bindings[index] = buffer;
        uniform_buffer_offsets[index] = 0;
        uniform_buffer_sizes[index] = -1;
    }
}

void gl_rend_state::bind_uniform_buffer_range(GLuint index,
    GLuint buffer, GLintptr offset, GLsizeiptr size, bool force) {
    if (force || uniform_buffer_bindings[index] != buffer
        || uniform_buffer_offsets[index] != offset
        || uniform_buffer_sizes[index] != size) {
        glBindBufferRange(GL_UNIFORM_BUFFER, index, buffer, offset, size);
        uniform_buffer_binding = buffer;
        uniform_buffer_bindings[index] = buffer;
        uniform_buffer_offsets[index] = offset;
        uniform_buffer_sizes[index] = size;
    }
}

void gl_rend_state::bind_shader_storage_buffer(GLuint buffer, bool force) {
    if (force || shader_storage_buffer_binding != buffer) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
        shader_storage_buffer_binding = buffer;
    }
}

void gl_rend_state::bind_shader_storage_buffer_base(GLuint index, GLuint buffer, bool force) {
    if (force || shader_storage_buffer_bindings[index] != buffer) {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, buffer);
        shader_storage_buffer_binding = buffer;
        shader_storage_buffer_bindings[index] = buffer;
        shader_storage_buffer_offsets[index] = 0;
        shader_storage_buffer_sizes[index] = -1;
    }
}

void gl_rend_state::bind_shader_storage_buffer_range(GLuint index,
    GLuint buffer, GLintptr offset, GLsizeiptr size, bool force) {
    if (force || shader_storage_buffer_bindings[index] != buffer
        || shader_storage_buffer_offsets[index] != offset
        || shader_storage_buffer_sizes[index] != size) {
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, index, buffer, offset, size);
        shader_storage_buffer_binding = buffer;
        shader_storage_buffer_bindings[index] = buffer;
        shader_storage_buffer_offsets[index] = offset;
        shader_storage_buffer_sizes[index] = size;
    }
}

void gl_rend_state::bind_texture_2d(GLuint texture, bool force) {
    if (texture_binding_2d[active_texture_index] != texture) {
        glBindTexture(GL_TEXTURE_2D, texture);
        texture_binding_2d[active_texture_index] = texture;
    }
}

void gl_rend_state::bind_texture_cube_map(GLuint texture, bool force) {
    if (force || texture_binding_cube_map[active_texture_index] != texture) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        texture_binding_cube_map[active_texture_index] = texture;
    }
}

void gl_rend_state::bind_sampler(int32_t index, GLuint sampler, bool force) {
    if (force || sampler_binding[index] != sampler) {
        glBindSampler(index, sampler);
        sampler_binding[index] = sampler;
    }
}

bool gl_rend_state::check_uniform_buffer_binding() {
    return uniform_buffer_binding != 0;
}

bool gl_rend_state::check_uniform_buffer_binding_base(size_t index) {
    return uniform_buffer_bindings[index] != 0;
}

bool gl_rend_state::check_shader_storage_buffer_binding() {
    return shader_storage_buffer_binding != 0;
}

bool gl_rend_state::check_shader_storage_buffer_binding_base(size_t index) {
    return shader_storage_buffer_bindings[index] != 0;
}

bool gl_rend_state::check_texture_binding_2d(size_t index) {
    return texture_binding_2d[index] != 0;
}

bool gl_rend_state::check_texture_binding_cube_map(size_t index) {
    return texture_binding_cube_map[index] != 0;
}

bool gl_rend_state::check_sampler_binding(int32_t index, GLuint sampler) {
    return sampler_binding[index] != 0;
}

void gl_rend_state::disable_blend(bool force) {
    if (force || blend) {
        glDisable(GL_BLEND);
        blend = GL_FALSE;
    }
}

void gl_rend_state::disable_cull_face(bool force) {
    if (force || cull_face) {
        glDisable(GL_CULL_FACE);
        cull_face = GL_FALSE;
    }
}

void gl_rend_state::disable_depth_test(bool force) {
    if (force || depth_test) {
        glDisable(GL_DEPTH_TEST);
        depth_test = GL_FALSE;
    }
}

void gl_rend_state::disable_multisample(bool force) {
    if (force || multisample) {
        glDisable(GL_MULTISAMPLE);
        multisample = GL_FALSE;
    }
}

void gl_rend_state::disable_primitive_restart(bool force) {
    if (force || primitive_restart) {
        glDisable(GL_PRIMITIVE_RESTART);
        primitive_restart = GL_FALSE;
    }
}

void gl_rend_state::disable_scissor_test(bool force) {
    if (force || scissor_test) {
        glDisable(GL_SCISSOR_TEST);
        scissor_test = GL_FALSE;
    }
}

void gl_rend_state::disable_stencil_test(bool force) {
    if (force || stencil_test) {
        glDisable(GL_STENCIL_TEST);
        stencil_test = GL_FALSE;
    }
}

void gl_rend_state::enable_blend(bool force) {
    if (force || !blend) {
        glEnable(GL_BLEND);
        blend = GL_TRUE;
    }
}

void gl_rend_state::enable_cull_face(bool force) {
    if (force || !cull_face) {
        glEnable(GL_CULL_FACE);
        cull_face = GL_TRUE;
    }
}

void gl_rend_state::enable_depth_test(bool force) {
    if (force || !depth_test) {
        glEnable(GL_DEPTH_TEST);
        depth_test = GL_TRUE;
    }
}

void gl_rend_state::enable_multisample(bool force) {
    if (force || !multisample) {
        glEnable(GL_MULTISAMPLE);
        multisample = GL_TRUE;
    }
}

void gl_rend_state::enable_primitive_restart(bool force) {
    if (force || !primitive_restart) {
        glEnable(GL_PRIMITIVE_RESTART);
        primitive_restart = GL_TRUE;
    }
}

void gl_rend_state::enable_scissor_test(bool force) {
    if (force || !scissor_test) {
        glEnable(GL_SCISSOR_TEST);
        scissor_test = GL_TRUE;
    }
}

void gl_rend_state::enable_stencil_test(bool force) {
    if (force || !stencil_test) {
        glEnable(GL_STENCIL_TEST);
        stencil_test = GL_TRUE;
    }
}

void gl_rend_state::get() {
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*)&program);
    GLenum active_texture = GL_TEXTURE0;
    glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&active_texture);
    active_texture_index = active_texture - GL_TEXTURE0;

    if (GLAD_GL_VERSION_4_5) {
        for (GLuint i = 0; i < 32; i++) {
            glGetIntegeri_v(GL_TEXTURE_BINDING_2D, i, (GLint*)&texture_binding_2d[i]);
            glGetIntegeri_v(GL_TEXTURE_BINDING_CUBE_MAP, i, (GLint*)&texture_binding_cube_map[i]);
            glGetIntegeri_v(GL_SAMPLER_BINDING, i, (GLint*)&sampler_binding[i]);
        }
    }
    else {
        for (GLuint i = 0; i < 32; i++) {
            glActiveTexture((GLenum)(GL_TEXTURE0 + i));
            glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*)&texture_binding_2d[i]);
            glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, (GLint*)&texture_binding_cube_map[i]);
            glGetIntegerv(GL_SAMPLER_BINDING, (GLint*)&sampler_binding[i]);
        }
        glActiveTexture(active_texture);
    }

    glGetBooleanv(GL_BLEND, &blend);
    glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&blend_src_rgb);
    glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&blend_src_alpha);
    glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&blend_dst_rgb);
    glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&blend_dst_alpha);
    glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&blend_mode_rgb);
    glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&blend_mode_alpha);

    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, (GLint*)&read_framebuffer_binding);
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, (GLint*)&draw_framebuffer_binding);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, (GLint*)&vertex_array_binding);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (GLint*)&array_buffer_binding);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, (GLint*)&element_array_buffer_binding);

    glGetIntegerv(GL_UNIFORM_BUFFER_BINDING, (GLint*)&uniform_buffer_binding);
    for (GLuint i = 0; i < 14; i++) {
        glGetIntegeri_v(GL_UNIFORM_BUFFER_BINDING, i, (GLint*)&uniform_buffer_bindings[i]);
        glGetInteger64i_v(GL_UNIFORM_BUFFER_START, i, (GLint64*)&uniform_buffer_offsets[i]);
        glGetInteger64i_v(GL_UNIFORM_BUFFER_SIZE, i, (GLint64*)&uniform_buffer_sizes[i]);
    }

    glGetIntegerv(GL_SHADER_STORAGE_BUFFER_BINDING, (GLint*)&shader_storage_buffer_binding);
    for (GLuint i = 0; i < 14; i++) {
        glGetIntegeri_v(GL_SHADER_STORAGE_BUFFER_BINDING, i, (GLint*)&shader_storage_buffer_bindings[i]);
        glGetInteger64i_v(GL_SHADER_STORAGE_BUFFER_START, i, (GLint64*)&shader_storage_buffer_offsets[i]);
        glGetInteger64i_v(GL_SHADER_STORAGE_BUFFER_SIZE, i, (GLint64*)&shader_storage_buffer_sizes[i]);
    }

    glGetBooleanv(GL_COLOR_WRITEMASK, color_mask);
    glGetBooleanv(GL_CULL_FACE, &cull_face);
    glGetIntegerv(GL_CULL_FACE_MODE, (GLint*)&cull_face_mode);
    glGetBooleanv(GL_DEPTH_TEST, &depth_test);
    glGetIntegerv(GL_DEPTH_FUNC, (GLint*)&depth_func);
    glGetBooleanv(GL_DEPTH_WRITEMASK, &depth_mask);
    glGetFloatv(GL_LINE_WIDTH, &line_width);
    glGetBooleanv(GL_MULTISAMPLE, &multisample);
    glGetBooleanv(GL_PRIMITIVE_RESTART, &primitive_restart);
    glGetIntegerv(GL_PRIMITIVE_RESTART_INDEX, (GLint*)&primitive_restart_index);
    glGetIntegerv(GL_SCISSOR_BOX, (GLint*)&scissor_box);
    glGetBooleanv(GL_SCISSOR_TEST, &scissor_test);
    glGetBooleanv(GL_STENCIL_TEST, &stencil_test);
    glGetIntegerv(GL_STENCIL_FUNC, (GLint*)&stencil_func);
    glGetIntegerv(GL_STENCIL_VALUE_MASK, (GLint*)&stencil_value_mask);
    glGetIntegerv(GL_STENCIL_FAIL, (GLint*)&stencil_fail);
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, (GLint*)&stencil_dpfail);
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, (GLint*)&stencil_dppass);
    glGetIntegerv(GL_STENCIL_REF, &stencil_ref);
    glGetIntegerv(GL_STENCIL_WRITEMASK, (GLint*)&stencil_mask);
    glGetIntegerv(GL_VIEWPORT, (GLint*)&viewport);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    polygon_mode = GL_FILL;
}

void gl_rend_state::end_event() {
    glPopDebugGroup();
}

GLuint gl_rend_state::get_program() {
    return program;
}

gl_rend_state_rect gl_rend_state::get_scissor() {
    return scissor_box;
}

void gl_rend_state::get_scissor(GLint& x, GLint& y, GLsizei& width, GLsizei& height) {
    x = scissor_box.x;
    y = scissor_box.y;
    width = scissor_box.width;
    height = scissor_box.height;
}

gl_rend_state_rect gl_rend_state::get_viewport() {
    return viewport;
}

void gl_rend_state::get_viewport(GLint& x, GLint& y, GLsizei& width, GLsizei& height) {
    x = viewport.x;
    y = viewport.y;
    width = viewport.width;
    height = viewport.height;
}

void gl_rend_state::set_blend_func(GLenum src, GLenum dst, bool force) {
    if (force || blend_src_rgb != src || blend_dst_rgb != dst
        || blend_src_alpha != GL_ONE || blend_dst_alpha != GL_ONE_MINUS_SRC_ALPHA) {
        glBlendFuncSeparate(src, dst, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        blend_src_rgb = src;
        blend_dst_rgb = dst;
        blend_src_alpha = GL_ONE;
        blend_dst_alpha = GL_ONE_MINUS_SRC_ALPHA;
    }
}

void gl_rend_state::set_blend_func_separate(GLenum src_rgb, GLenum dst_rgb,
    GLenum src_alpha, GLenum dst_alpha, bool force) {
    if (force || blend_src_rgb != src_rgb || blend_dst_rgb != dst_rgb
        || blend_src_alpha != src_alpha || blend_dst_alpha != dst_alpha) {
        glBlendFuncSeparate(src_rgb, dst_rgb, src_alpha, dst_alpha);
        blend_src_rgb = src_rgb;
        blend_dst_rgb = dst_rgb;
        blend_src_alpha = src_alpha;
        blend_dst_alpha = dst_alpha;
    }
}

void gl_rend_state::set_blend_equation(GLenum mode, bool force) {
    if (force || blend_mode_rgb != mode || blend_mode_alpha != mode) {
        glBlendEquationSeparate(mode, mode);
        blend_mode_rgb = mode;
        blend_mode_alpha = mode;
    }
}

void gl_rend_state::set_blend_equation_separate(GLenum mode_rgb, GLenum mode_alpha, bool force) {
    if (force || blend_mode_rgb != mode_rgb || blend_mode_alpha != mode_alpha) {
        glBlendEquationSeparate(mode_rgb, mode_alpha);
        blend_mode_rgb = mode_rgb;
        blend_mode_alpha = mode_alpha;
    }
}

void gl_rend_state::set_color_mask(GLboolean red, GLboolean green,
    GLboolean blue, GLboolean alpha, bool force) {
    if (force || color_mask[0] != red || color_mask[1] != green
        || color_mask[2] != blue || color_mask[3] != alpha) {
        glColorMask(red, green, blue, alpha);
        color_mask[0] = red;
        color_mask[1] = green;
        color_mask[2] = blue;
        color_mask[3] = alpha;
    }
}

void gl_rend_state::set_cull_face_mode(GLenum mode, bool force) {
    if (force || cull_face_mode != mode) {
        glCullFace(mode);
        cull_face_mode = mode;
    }
}

void gl_rend_state::set_depth_func(GLenum func, bool force) {
    if (force || depth_func != func) {
        glDepthFunc(func);
        depth_func = func;
    }
}

void gl_rend_state::set_depth_mask(GLboolean flag, bool force) {
    if (force || depth_mask != flag) {
        glDepthMask(flag);
        depth_mask = flag;
    }
}

void gl_rend_state::set_line_width(GLfloat width, bool force) {
    if (force || line_width != width) {
        glLineWidth(width);
        line_width = width;
    }
}

void gl_rend_state::set_polygon_mode(GLenum face, GLenum mode, bool force) {
    switch (face) {
    case GL_FRONT:
        if (force || polygon_mode != mode) {
            glPolygonMode(GL_FRONT, mode);
            polygon_mode = mode;
        }
        break;
    case GL_BACK:
        if (force || polygon_mode != mode) {
            glPolygonMode(GL_BACK, mode);
            polygon_mode = mode;
        }
        break;
    case GL_FRONT_AND_BACK:
        if (force || polygon_mode != mode) {
            glPolygonMode(GL_FRONT_AND_BACK, mode);
            polygon_mode = mode;
        }
        break;
    }
}

void gl_rend_state::set_primitive_restart_index(GLuint index, bool force) {
    if (force || primitive_restart_index != index) {
        glPrimitiveRestartIndex(index);
        primitive_restart_index = index;
    }
}

void gl_rend_state::set_scissor(const gl_rend_state_rect& rect, bool force) {
    if (force || scissor_box.x != rect.x || scissor_box.y != rect.y
        || scissor_box.width != rect.width || scissor_box.height != rect.height) {
        glScissor(rect.x, rect.y, rect.width, rect.height);
        scissor_box = rect;
    }
}

void gl_rend_state::set_scissor(GLint x, GLint y, GLsizei width, GLsizei height, bool force) {
    if (force || scissor_box.x != x || scissor_box.y != y
        || scissor_box.width != width || scissor_box.height != height) {
        glScissor(x, y, width, height);
        scissor_box.x = x;
        scissor_box.y = y;
        scissor_box.width = width;
        scissor_box.height = height;
    }
}

void gl_rend_state::set_stencil_func(GLenum func, GLint ref, GLuint mask, bool force) {
    if (force || stencil_func != func || stencil_ref != ref || stencil_value_mask != mask) {
        glStencilFunc(func, ref, mask);
        stencil_func = func;
        stencil_ref = ref;
        stencil_value_mask = mask;
    }
}

void gl_rend_state::set_stencil_mask(GLuint mask, bool force) {
    if (force || stencil_mask != mask) {
        glStencilMask(mask);
        stencil_mask = mask;
    }
}

void gl_rend_state::set_stencil_op(GLenum sfail, GLenum dpfail, GLenum dppass, bool force) {
    if (force || stencil_fail != sfail || stencil_dpfail != dpfail || stencil_dppass != dppass) {
        glStencilOp(sfail, dpfail, dppass);
        stencil_fail = sfail;
        stencil_dpfail = dpfail;
        stencil_dppass = dppass;
    }
}

void gl_rend_state::set_viewport(const gl_rend_state_rect& rect, bool force) {
    if (force || viewport.x != rect.x || viewport.y != rect.y
        || viewport.width != rect.width || viewport.height != rect.height) {
        glViewport(rect.x, rect.y, rect.width, rect.height);
        viewport = rect;
    }
}

void gl_rend_state::set_viewport(GLint x, GLint y, GLsizei width, GLsizei height, bool force) {
    if (force || viewport.x != x || viewport.y != y
        || viewport.width != width || viewport.height != height) {
        glViewport(x, y, width, height);
        viewport.x = x;
        viewport.y = y;
        viewport.width = width;
        viewport.height = height;
    }
}

void gl_rend_state::use_program(GLuint program, bool force) {
    if (force || this->program != program) {
        glUseProgram(program);
        this->program = program;
    }
}
