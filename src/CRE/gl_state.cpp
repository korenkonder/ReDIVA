/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "gl_state.hpp"

gl_state_struct gl_state;

void gl_state_active_bind_texture_2d(int32_t index, GLuint texture, bool force) {
    if (force || gl_state.texture_binding_2d[index] != texture) {
        if (force || gl_state.active_texture_index != index) {
#ifdef USE_OPENGL
            if (!Vulkan::use)
                glActiveTexture((GLenum)(GL_TEXTURE0 + index));
#endif
            gl_state.active_texture = (GLenum)(GL_TEXTURE0 + index);
            gl_state.active_texture_index = (GLuint)index;
        }
        glBindTexture(GL_TEXTURE_2D, texture);
        gl_state.texture_binding_2d[index] = texture;
    }
}

void gl_state_active_bind_texture_cube_map(int32_t index, GLuint texture, bool force) {
    if (force || gl_state.texture_binding_cube_map[index] != texture) {
        if (force || gl_state.active_texture_index != index) {
#ifdef USE_OPENGL
            if (!Vulkan::use)
                glActiveTexture((GLenum)(GL_TEXTURE0 + index));
#endif
            gl_state.active_texture = (GLenum)(GL_TEXTURE0 + index);
            gl_state.active_texture_index = (GLuint)index;
        }
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        gl_state.texture_binding_cube_map[index] = texture;
    }
}

void gl_state_active_texture(size_t index, bool force) {
    if (force || gl_state.active_texture_index != index) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glActiveTexture((GLenum)(GL_TEXTURE0 + index));
#endif
        gl_state.active_texture = (GLenum)(GL_TEXTURE0 + index);
        gl_state.active_texture_index = (GLuint)index;
    }
}

void gl_state_bind_framebuffer(GLuint framebuffer, bool force) {
    if (force || gl_state.framebuffer_binding != framebuffer
        || gl_state.read_framebuffer_binding != framebuffer
        || gl_state.draw_framebuffer_binding != framebuffer) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
#endif
        gl_state.framebuffer_binding = framebuffer;
        gl_state.read_framebuffer_binding = framebuffer;
        gl_state.draw_framebuffer_binding = framebuffer;
    }
}

void gl_state_bind_read_framebuffer(GLuint framebuffer, bool force) {
    if (force || gl_state.read_framebuffer_binding != framebuffer) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
#endif
        gl_state.read_framebuffer_binding = framebuffer;
    }
}

void gl_state_bind_draw_framebuffer(GLuint framebuffer, bool force) {
    if (force || gl_state.draw_framebuffer_binding != framebuffer) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
#endif
        gl_state.draw_framebuffer_binding = framebuffer;
    }
}

void gl_state_bind_vertex_array(GLuint array, bool force) {
    if (force || gl_state.vertex_array_binding != array) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glBindVertexArray(array);
#endif
        gl_state.vertex_array_binding = array;
    }
}

void gl_state_bind_array_buffer(GLuint buffer, bool force) {
    if (force || gl_state.array_buffer_binding != buffer) {
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        gl_state.array_buffer_binding = buffer;
    }
}

void gl_state_bind_element_array_buffer(GLuint buffer, bool force) {
    if (force || gl_state.element_array_buffer_binding != buffer) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
        gl_state.element_array_buffer_binding = buffer;
    }
}

void gl_state_bind_uniform_buffer(GLuint buffer, bool force) {
    if (force || gl_state.uniform_buffer_binding != buffer) {
        glBindBuffer(GL_UNIFORM_BUFFER, buffer);
        gl_state.uniform_buffer_binding = buffer;
    }
}

void gl_state_bind_uniform_buffer_base(GLuint index, GLuint buffer, bool force) {
    if (force || gl_state.uniform_buffer_bindings[index] != buffer) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glBindBufferBase(GL_UNIFORM_BUFFER, index, buffer);
#endif
        gl_state.uniform_buffer_binding = buffer;
        gl_state.uniform_buffer_bindings[index] = buffer;
        gl_state.uniform_buffer_offsets[index] = 0;
        gl_state.uniform_buffer_sizes[index] = -1;
    }
}

void gl_state_bind_uniform_buffer_range(GLuint index,
    GLuint buffer, GLintptr offset, GLsizeiptr size, bool force) {
    if (force || gl_state.uniform_buffer_bindings[index] != buffer
        || gl_state.uniform_buffer_offsets[index] != offset
        || gl_state.uniform_buffer_sizes[index] != size) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glBindBufferRange(GL_UNIFORM_BUFFER, index, buffer, offset, size);
#endif
        gl_state.uniform_buffer_binding = buffer;
        gl_state.uniform_buffer_bindings[index] = buffer;
        gl_state.uniform_buffer_offsets[index] = offset;
        gl_state.uniform_buffer_sizes[index] = size;
    }
}

void gl_state_bind_shader_storage_buffer(GLuint buffer, bool force) {
    if (force || gl_state.shader_storage_buffer_binding != buffer) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
        gl_state.shader_storage_buffer_binding = buffer;
    }
}

void gl_state_bind_shader_storage_buffer_base(GLuint index, GLuint buffer, bool force) {
    if (force || gl_state.shader_storage_buffer_bindings[index] != buffer) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, buffer);
#endif
        gl_state.shader_storage_buffer_binding = buffer;
        gl_state.shader_storage_buffer_bindings[index] = buffer;
        gl_state.shader_storage_buffer_offsets[index] = 0;
        gl_state.shader_storage_buffer_sizes[index] = -1;
    }
}

void gl_state_bind_shader_storage_buffer_range(GLuint index,
    GLuint buffer, GLintptr offset, GLsizeiptr size, bool force) {
    if (force || gl_state.shader_storage_buffer_bindings[index] != buffer
        || gl_state.shader_storage_buffer_offsets[index] != offset
        || gl_state.shader_storage_buffer_sizes[index] != size) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glBindBufferRange(GL_SHADER_STORAGE_BUFFER, index, buffer, offset, size);
#endif
        gl_state.shader_storage_buffer_binding = buffer;
        gl_state.shader_storage_buffer_bindings[index] = buffer;
        gl_state.shader_storage_buffer_offsets[index] = offset;
        gl_state.shader_storage_buffer_sizes[index] = size;
    }
}

void gl_state_bind_texture_2d(GLuint texture, bool force) {
    if (gl_state.texture_binding_2d[gl_state.active_texture_index] != texture) {
        glBindTexture(GL_TEXTURE_2D, texture);
        gl_state.texture_binding_2d[gl_state.active_texture_index] = texture;
    }
}

void gl_state_bind_texture_cube_map(GLuint texture, bool force) {
    if (force || gl_state.texture_binding_cube_map[gl_state.active_texture_index] != texture) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        gl_state.texture_binding_cube_map[gl_state.active_texture_index] = texture;
    }
}

void gl_state_bind_sampler(int32_t index, GLuint sampler, bool force) {
    if (force || gl_state.sampler_binding[index] != sampler) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glBindSampler(index, sampler);
#endif
        gl_state.sampler_binding[index] = sampler;
    }
}

bool gl_state_check_uniform_buffer_binding() {
    return gl_state.uniform_buffer_binding != 0;
}

bool gl_state_check_uniform_buffer_binding_base(size_t index) {
    return gl_state.uniform_buffer_bindings[index] != 0;
}

bool gl_state_check_shader_storage_buffer_binding() {
    return gl_state.shader_storage_buffer_binding != 0;
}

bool gl_state_check_shader_storage_buffer_binding_base(size_t index) {
    return gl_state.shader_storage_buffer_bindings[index] != 0;
}

bool gl_state_check_texture_binding_2d(size_t index) {
    return gl_state.texture_binding_2d[index] != 0;
}

bool gl_state_check_texture_binding_cube_map(size_t index) {
    return gl_state.texture_binding_cube_map[index] != 0;
}

bool gl_state_check_sampler_binding(int32_t index, GLuint sampler) {
    return gl_state.sampler_binding[index] != 0;
}

void gl_state_disable_blend(bool force) {
    if (force || gl_state.blend) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glDisable(GL_BLEND);
#endif
        gl_state.blend = GL_FALSE;
    }
}

void gl_state_disable_cull_face(bool force) {
    if (force || gl_state.cull_face) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glDisable(GL_CULL_FACE);
#endif
        gl_state.cull_face = GL_FALSE;
    }
}

void gl_state_disable_depth_test(bool force) {
    if (force || gl_state.depth_test) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glDisable(GL_DEPTH_TEST);
#endif
        gl_state.depth_test = GL_FALSE;
    }
}

void gl_state_disable_multisample(bool force) {
    if (force || gl_state.multisample) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glDisable(GL_MULTISAMPLE);
#endif
        gl_state.multisample = GL_FALSE;
    }
}

void gl_state_disable_primitive_restart(bool force) {
    if (force || gl_state.primitive_restart) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glDisable(GL_PRIMITIVE_RESTART);
#endif
        gl_state.primitive_restart = GL_FALSE;
    }
}

void gl_state_disable_scissor_test(bool force) {
    if (force || gl_state.scissor_test) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glDisable(GL_SCISSOR_TEST);
#endif
        gl_state.scissor_test = GL_FALSE;
    }
}

void gl_state_disable_stencil_test(bool force) {
    if (force || gl_state.stencil_test) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glDisable(GL_STENCIL_TEST);
#endif
        gl_state.stencil_test = GL_FALSE;
    }
}

void gl_state_enable_blend(bool force) {
    if (force || !gl_state.blend) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glEnable(GL_BLEND);
#endif
        gl_state.blend = GL_TRUE;
    }
}

void gl_state_enable_cull_face(bool force) {
    if (force || !gl_state.cull_face) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glEnable(GL_CULL_FACE);
#endif
        gl_state.cull_face = GL_TRUE;
    }
}

void gl_state_enable_depth_test(bool force) {
    if (force || !gl_state.depth_test) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glEnable(GL_DEPTH_TEST);
#endif
        gl_state.depth_test = GL_TRUE;
    }
}

void gl_state_enable_multisample(bool force) {
    if (force || !gl_state.multisample) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glEnable(GL_MULTISAMPLE);
#endif
        gl_state.multisample = GL_TRUE;
    }
}

void gl_state_enable_primitive_restart(bool force) {
    if (force || !gl_state.primitive_restart) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glEnable(GL_PRIMITIVE_RESTART);
#endif
        gl_state.primitive_restart = GL_TRUE;
    }
}

void gl_state_enable_scissor_test(bool force) {
    if (force || !gl_state.scissor_test) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glEnable(GL_SCISSOR_TEST);
#endif
        gl_state.scissor_test = GL_TRUE;
    }
}

void gl_state_enable_stencil_test(bool force) {
    if (force || !gl_state.stencil_test) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glEnable(GL_STENCIL_TEST);
#endif
        gl_state.stencil_test = GL_TRUE;
    }
}

void gl_state_get() {
#ifdef USE_OPENGL
    if (Vulkan::use)
        return;

    glGetIntegerv(GL_CURRENT_PROGRAM, &gl_state.program);
    glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&gl_state.active_texture);
    gl_state.active_texture_index = gl_state.active_texture - GL_TEXTURE0;
    for (GLuint i = 0; i < 32; i++) {
        glActiveTexture((GLenum)(GL_TEXTURE0 + i));
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &gl_state.texture_binding_2d[i]);
        glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &gl_state.texture_binding_cube_map[i]);
        glGetIntegerv(GL_SAMPLER_BINDING, &gl_state.sampler_binding[i]);
    }
    glActiveTexture(gl_state.active_texture);

    glGetBooleanv(GL_BLEND, &gl_state.blend);
    glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&gl_state.blend_src_rgb);
    glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&gl_state.blend_src_alpha);
    glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&gl_state.blend_dst_rgb);
    glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&gl_state.blend_dst_alpha);
    glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&gl_state.blend_mode_rgb);
    glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&gl_state.blend_mode_alpha);

    glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&gl_state.framebuffer_binding);
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, (GLint*)&gl_state.read_framebuffer_binding);
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, (GLint*)&gl_state.draw_framebuffer_binding);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, (GLint*)&gl_state.vertex_array_binding);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (GLint*)&gl_state.array_buffer_binding);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, (GLint*)&gl_state.element_array_buffer_binding);

    glGetIntegerv(GL_UNIFORM_BUFFER_BINDING, (GLint*)&gl_state.uniform_buffer_binding);
    for (GLuint i = 0; i < 14; i++) {
        glGetIntegeri_v(GL_UNIFORM_BUFFER_BINDING, i, (GLint*)&gl_state.uniform_buffer_bindings[i]);
        glGetIntegeri_v(GL_UNIFORM_BUFFER_START, i, (GLint*)&gl_state.uniform_buffer_offsets[i]);
        glGetIntegeri_v(GL_UNIFORM_BUFFER_SIZE, i, (GLint*)&gl_state.uniform_buffer_sizes[i]);
    }

    glGetIntegerv(GL_SHADER_STORAGE_BUFFER_BINDING, (GLint*)&gl_state.shader_storage_buffer_binding);
    for (GLuint i = 0; i < 14; i++) {
        glGetIntegeri_v(GL_SHADER_STORAGE_BUFFER_BINDING, i, (GLint*)&gl_state.shader_storage_buffer_bindings[i]);
        glGetIntegeri_v(GL_SHADER_STORAGE_BUFFER_START, i, (GLint*)&gl_state.shader_storage_buffer_offsets[i]);
        glGetIntegeri_v(GL_SHADER_STORAGE_BUFFER_SIZE, i, (GLint*)&gl_state.shader_storage_buffer_sizes[i]);
    }

    glGetBooleanv(GL_COLOR_WRITEMASK, gl_state.color_mask);
    glGetBooleanv(GL_CULL_FACE, &gl_state.cull_face);
    glGetIntegerv(GL_CULL_FACE_MODE, (GLint*)&gl_state.cull_face_mode);
    glGetBooleanv(GL_DEPTH_TEST, &gl_state.depth_test);
    glGetIntegerv(GL_DEPTH_FUNC, (GLint*)&gl_state.depth_func);
    glGetBooleanv(GL_DEPTH_WRITEMASK, &gl_state.depth_mask);
    glGetFloatv(GL_LINE_WIDTH, &gl_state.line_width);
    glGetBooleanv(GL_MULTISAMPLE, &gl_state.multisample);
    glGetBooleanv(GL_PRIMITIVE_RESTART, &gl_state.primitive_restart);
    glGetIntegerv(GL_PRIMITIVE_RESTART_INDEX, (GLint*)&gl_state.primitive_restart_index);
    glGetIntegerv(GL_SCISSOR_BOX, (GLint*)&gl_state.scissor_box);
    glGetBooleanv(GL_SCISSOR_TEST, &gl_state.scissor_test);
    glGetBooleanv(GL_STENCIL_TEST, &gl_state.stencil_test);
    glGetIntegerv(GL_STENCIL_FUNC, (GLint*)&gl_state.stencil_func);
    glGetIntegerv(GL_STENCIL_VALUE_MASK, (GLint*)&gl_state.stencil_value_mask);
    glGetIntegerv(GL_STENCIL_FAIL, (GLint*)&gl_state.stencil_fail);
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, (GLint*)&gl_state.stencil_dpfail);
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, (GLint*)&gl_state.stencil_dppass);
    glGetIntegerv(GL_STENCIL_REF, &gl_state.stencil_ref);
    glGetIntegerv(GL_STENCIL_WRITEMASK, (GLint*)&gl_state.stencil_mask);
    glGetIntegerv(GL_VIEWPORT, (GLint*)&gl_state.viewport);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    gl_state.polygon_mode = GL_FILL;
#endif
}

void gl_state_get_all_gl_errors() {
    GLenum error;
    while (error = glGetError())
        printf_debug("GL Error: 0x%04X\n", error);
}

GLenum gl_state_get_error() {
    GLenum error = glGetError();
    if (error)
        printf_debug("GL Error: 0x%04X\n", error);
    return error;
}

GLuint gl_state_get_program() {
    return gl_state.program;
}

gl_state_rect gl_state_get_scissor() {
    return gl_state.scissor_box;
}

void gl_state_get_scissor(GLint& x, GLint& y, GLsizei& width, GLsizei& height) {
    x = gl_state.scissor_box.x;
    y = gl_state.scissor_box.y;
    width = gl_state.scissor_box.width;
    height = gl_state.scissor_box.height;
}

gl_state_rect gl_state_get_viewport() {
    return gl_state.viewport;
}

void gl_state_get_viewport(GLint& x, GLint& y, GLsizei& width, GLsizei& height) {
    x = gl_state.viewport.x;
    y = gl_state.viewport.y;
    width = gl_state.viewport.width;
    height = gl_state.viewport.height;
}

void gl_state_set_blend_func(GLenum src, GLenum dst, bool force) {
    if (force || gl_state.blend_src_rgb != src || gl_state.blend_dst_rgb != dst
        || gl_state.blend_src_alpha != GL_ONE || gl_state.blend_dst_alpha != GL_ONE_MINUS_SRC_ALPHA) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glBlendFuncSeparate(src, dst, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
#endif
        gl_state.blend_src_rgb = src;
        gl_state.blend_dst_rgb = dst;
        gl_state.blend_src_alpha = GL_ONE;
        gl_state.blend_dst_alpha = GL_ONE_MINUS_SRC_ALPHA;
    }
}

void gl_state_set_blend_func_separate(GLenum src_rgb, GLenum dst_rgb,
    GLenum src_alpha, GLenum dst_alpha, bool force) {
    if (force || gl_state.blend_src_rgb != src_rgb || gl_state.blend_dst_rgb != dst_rgb
        || gl_state.blend_src_alpha != src_alpha || gl_state.blend_dst_alpha != dst_alpha) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glBlendFuncSeparate(src_rgb, dst_rgb, src_alpha, dst_alpha);
#endif
        gl_state.blend_src_rgb = src_rgb;
        gl_state.blend_dst_rgb = dst_rgb;
        gl_state.blend_src_alpha = src_alpha;
        gl_state.blend_dst_alpha = dst_alpha;
    }
}

void gl_state_set_blend_equation(GLenum mode, bool force) {
    if (force || gl_state.blend_mode_rgb != mode || gl_state.blend_mode_alpha != mode) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glBlendEquationSeparate(mode, mode);
#endif
        gl_state.blend_mode_rgb = mode;
        gl_state.blend_mode_alpha = mode;
    }
}

void gl_state_set_blend_equation_separate(GLenum mode_rgb, GLenum mode_alpha, bool force) {
    if (force || gl_state.blend_mode_rgb != mode_rgb || gl_state.blend_mode_alpha != mode_alpha) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glBlendEquationSeparate(mode_rgb, mode_alpha);
#endif
        gl_state.blend_mode_rgb = mode_rgb;
        gl_state.blend_mode_alpha = mode_alpha;
    }
}

void gl_state_set_color_mask(GLboolean red, GLboolean green,
    GLboolean blue, GLboolean alpha, bool force) {
    if (force || gl_state.color_mask[0] != red || gl_state.color_mask[1] != green
        || gl_state.color_mask[2] != blue || gl_state.color_mask[3] != alpha) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glColorMask(red, green, blue, alpha);
#endif
        gl_state.color_mask[0] = red;
        gl_state.color_mask[1] = green;
        gl_state.color_mask[2] = blue;
        gl_state.color_mask[3] = alpha;
    }
}

void gl_state_set_cull_face_mode(GLenum mode, bool force) {
    if (force || gl_state.cull_face_mode != mode) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glCullFace(mode);
#endif
        gl_state.cull_face_mode = mode;
    }
}

void gl_state_set_depth_func(GLenum func, bool force) {
    if (force || gl_state.depth_func != func) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glDepthFunc(func);
#endif
        gl_state.depth_func = func;
    }
}

void gl_state_set_depth_mask(GLboolean flag, bool force) {
    if (force || gl_state.depth_mask != flag) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glDepthMask(flag);
#endif
        gl_state.depth_mask = flag;
    }
}

void gl_state_set_line_width(GLfloat width, bool force) {
    if (force || gl_state.line_width != width) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glLineWidth(width);
#endif
        gl_state.line_width = width;
    }
}

void gl_state_set_polygon_mode(GLenum face, GLenum mode, bool force) {
    switch (face) {
    case GL_FRONT:
        if (force || gl_state.polygon_mode != mode) {
#ifdef USE_OPENGL
            if (!Vulkan::use)
                glPolygonMode(GL_FRONT, mode);
#endif
            gl_state.polygon_mode = mode;
        }
        break;
    case GL_BACK:
        if (force || gl_state.polygon_mode != mode) {
#ifdef USE_OPENGL
            if (!Vulkan::use)
                glPolygonMode(GL_BACK, mode);
#endif
            gl_state.polygon_mode = mode;
        }
        break;
    case GL_FRONT_AND_BACK:
        if (force || gl_state.polygon_mode != mode) {
#ifdef USE_OPENGL
            if (!Vulkan::use)
                glPolygonMode(GL_FRONT_AND_BACK, mode);
#endif
            gl_state.polygon_mode = mode;
        }
        break;
    }
}

void gl_state_set_primitive_restart_index(GLuint index, bool force) {
    if (force || gl_state.primitive_restart_index != index) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glPrimitiveRestartIndex(index);
#endif
        gl_state.primitive_restart_index = index;
    }
}

void gl_state_set_scissor(const gl_state_rect& rect, bool force) {
    if (force || gl_state.scissor_box.x != rect.x || gl_state.scissor_box.y != rect.y
        || gl_state.scissor_box.width != rect.width || gl_state.scissor_box.height != rect.height) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glScissor(rect.x, rect.y, rect.width, rect.height);
#endif
        gl_state.scissor_box = rect;
    }
}

void gl_state_set_scissor(GLint x, GLint y, GLsizei width, GLsizei height, bool force) {
    if (force || gl_state.scissor_box.x != x || gl_state.scissor_box.y != y
        || gl_state.scissor_box.width != width || gl_state.scissor_box.height != height) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glScissor(x, y, width, height);
#endif
        gl_state.scissor_box.x = x;
        gl_state.scissor_box.y = y;
        gl_state.scissor_box.width = width;
        gl_state.scissor_box.height = height;
    }
}

void gl_state_set_stencil_func(GLenum func, GLint ref, GLuint mask, bool force) {
    if (force || gl_state.stencil_func != func
        || gl_state.stencil_ref != ref || gl_state.stencil_value_mask != mask) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glStencilFunc(func, ref, mask);
#endif
        gl_state.stencil_func = func;
        gl_state.stencil_ref = ref;
        gl_state.stencil_value_mask = mask;
    }
}

void gl_state_set_stencil_mask(GLuint mask, bool force) {
    if (force || gl_state.stencil_mask != mask) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glStencilMask(mask);
#endif
        gl_state.stencil_mask = mask;
    }
}

void gl_state_set_stencil_op(GLenum sfail, GLenum dpfail, GLenum dppass, bool force) {
    if (force || gl_state.stencil_fail != sfail
        || gl_state.stencil_dpfail != dpfail || gl_state.stencil_dppass != dppass) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glStencilOp(sfail, dpfail, dppass);
#endif
        gl_state.stencil_fail = sfail;
        gl_state.stencil_dpfail = dpfail;
        gl_state.stencil_dppass = dppass;
    }
}

void gl_state_set_viewport(const gl_state_rect& rect, bool force) {
    if (force || gl_state.viewport.x != rect.x || gl_state.viewport.y != rect.y
        || gl_state.viewport.width != rect.width || gl_state.viewport.height != rect.height) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glViewport(rect.x, rect.y, rect.width, rect.height);
#endif
        gl_state.viewport = rect;
    }
}

void gl_state_set_viewport(GLint x, GLint y, GLsizei width, GLsizei height, bool force) {
    if (force || gl_state.viewport.x != x || gl_state.viewport.y != y
        || gl_state.viewport.width != width || gl_state.viewport.height != height) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glViewport(x, y, width, height);
#endif
        gl_state.viewport.x = x;
        gl_state.viewport.y = y;
        gl_state.viewport.width = width;
        gl_state.viewport.height = height;
    }
}

void gl_state_use_program(GLuint program, bool force) {
    if (force || gl_state.program != program) {
#ifdef USE_OPENGL
        if (!Vulkan::use)
            glUseProgram(program);
#endif
        gl_state.program = program;
    }
}
