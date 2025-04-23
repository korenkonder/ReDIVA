/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "gl_rend_state.hpp"

gl_rend_state_struct gl_rend_state;

void gl_rend_state_struct::active_bind_texture_2d(int32_t index, GLuint texture, bool force) {
    if (force || texture_binding_2d[index] != texture) {
        if (force || active_texture_index != index) {
            glActiveTexture((GLenum)(GL_TEXTURE0 + index));
            active_texture_index = (GLuint)index;
        }
        glBindTexture(GL_TEXTURE_2D, texture);
        texture_binding_2d[index] = texture;
    }
}

void gl_rend_state_struct::active_bind_texture_cube_map(int32_t index, GLuint texture, bool force) {
    if (force || texture_binding_cube_map[index] != texture) {
        if (force || active_texture_index != index) {
            glActiveTexture((GLenum)(GL_TEXTURE0 + index));
            active_texture_index = (GLuint)index;
        }
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        texture_binding_cube_map[index] = texture;
    }
}

void gl_rend_state_struct::active_texture(size_t index, bool force) {
    if (force || active_texture_index != index) {
        glActiveTexture((GLenum)(GL_TEXTURE0 + index));
        active_texture_index = (GLuint)index;
    }
}

void gl_rend_state_struct::bind_framebuffer(GLuint framebuffer, bool force) {
    if (force || read_framebuffer_binding != framebuffer
        || draw_framebuffer_binding != framebuffer) {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        read_framebuffer_binding = framebuffer;
        draw_framebuffer_binding = framebuffer;
    }
}

void gl_rend_state_struct::bind_read_framebuffer(GLuint framebuffer, bool force) {
    if (force || read_framebuffer_binding != framebuffer) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
        read_framebuffer_binding = framebuffer;
    }
}

void gl_rend_state_struct::bind_draw_framebuffer(GLuint framebuffer, bool force) {
    if (force || draw_framebuffer_binding != framebuffer) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
        draw_framebuffer_binding = framebuffer;
    }
}

void gl_rend_state_struct::bind_vertex_array(GLuint array, bool force) {
    if (force || vertex_array_binding != array) {
        glBindVertexArray(array);
        vertex_array_binding = array;
    }
}

void gl_rend_state_struct::bind_array_buffer(GLuint buffer, bool force) {
    if (force || array_buffer_binding != buffer) {
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        array_buffer_binding = buffer;
    }
}

void gl_rend_state_struct::bind_element_array_buffer(GLuint buffer, bool force) {
    if (force || element_array_buffer_binding != buffer) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
        element_array_buffer_binding = buffer;
    }
}

void gl_rend_state_struct::bind_uniform_buffer(GLuint buffer, bool force) {
    if (force || uniform_buffer_binding != buffer) {
        glBindBuffer(GL_UNIFORM_BUFFER, buffer);
        uniform_buffer_binding = buffer;
    }
}

void gl_rend_state_struct::bind_uniform_buffer_base(GLuint index, GLuint buffer, bool force) {
    if (force || uniform_buffer_bindings[index] != buffer) {
        glBindBufferBase(GL_UNIFORM_BUFFER, index, buffer);
        uniform_buffer_binding = buffer;
        uniform_buffer_bindings[index] = buffer;
        uniform_buffer_offsets[index] = 0;
        uniform_buffer_sizes[index] = -1;
    }
}

void gl_rend_state_struct::bind_uniform_buffer_range(GLuint index,
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

void gl_rend_state_struct::bind_shader_storage_buffer(GLuint buffer, bool force) {
    if (force || shader_storage_buffer_binding != buffer) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
        shader_storage_buffer_binding = buffer;
    }
}

void gl_rend_state_struct::bind_shader_storage_buffer_base(GLuint index, GLuint buffer, bool force) {
    if (force || shader_storage_buffer_bindings[index] != buffer) {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, buffer);
        shader_storage_buffer_binding = buffer;
        shader_storage_buffer_bindings[index] = buffer;
        shader_storage_buffer_offsets[index] = 0;
        shader_storage_buffer_sizes[index] = -1;
    }
}

void gl_rend_state_struct::bind_shader_storage_buffer_range(GLuint index,
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

void gl_rend_state_struct::bind_texture_2d(GLuint texture, bool force) {
    if (texture_binding_2d[active_texture_index] != texture) {
        glBindTexture(GL_TEXTURE_2D, texture);
        texture_binding_2d[active_texture_index] = texture;
    }
}

void gl_rend_state_struct::bind_texture_cube_map(GLuint texture, bool force) {
    if (force || texture_binding_cube_map[active_texture_index] != texture) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        texture_binding_cube_map[active_texture_index] = texture;
    }
}

void gl_rend_state_struct::bind_sampler(int32_t index, GLuint sampler, bool force) {
    if (force || sampler_binding[index] != sampler) {
        glBindSampler(index, sampler);
        sampler_binding[index] = sampler;
    }
}

bool gl_rend_state_struct::check_uniform_buffer_binding() {
    return uniform_buffer_binding != 0;
}

bool gl_rend_state_struct::check_uniform_buffer_binding_base(size_t index) {
    return uniform_buffer_bindings[index] != 0;
}

bool gl_rend_state_struct::check_shader_storage_buffer_binding() {
    return shader_storage_buffer_binding != 0;
}

bool gl_rend_state_struct::check_shader_storage_buffer_binding_base(size_t index) {
    return shader_storage_buffer_bindings[index] != 0;
}

bool gl_rend_state_struct::check_texture_binding_2d(size_t index) {
    return texture_binding_2d[index] != 0;
}

bool gl_rend_state_struct::check_texture_binding_cube_map(size_t index) {
    return texture_binding_cube_map[index] != 0;
}

bool gl_rend_state_struct::check_sampler_binding(int32_t index, GLuint sampler) {
    return sampler_binding[index] != 0;
}

void gl_rend_state_struct::disable_blend(bool force) {
    if (force || blend) {
        glDisable(GL_BLEND);
        blend = GL_FALSE;
    }
}

void gl_rend_state_struct::disable_cull_face(bool force) {
    if (force || cull_face) {
        glDisable(GL_CULL_FACE);
        cull_face = GL_FALSE;
    }
}

void gl_rend_state_struct::disable_depth_test(bool force) {
    if (force || depth_test) {
        glDisable(GL_DEPTH_TEST);
        depth_test = GL_FALSE;
    }
}

void gl_rend_state_struct::disable_multisample(bool force) {
    if (force || multisample) {
        glDisable(GL_MULTISAMPLE);
        multisample = GL_FALSE;
    }
}

void gl_rend_state_struct::disable_primitive_restart(bool force) {
    if (force || primitive_restart) {
        glDisable(GL_PRIMITIVE_RESTART);
        primitive_restart = GL_FALSE;
    }
}

void gl_rend_state_struct::disable_scissor_test(bool force) {
    if (force || scissor_test) {
        glDisable(GL_SCISSOR_TEST);
        scissor_test = GL_FALSE;
    }
}

void gl_rend_state_struct::disable_stencil_test(bool force) {
    if (force || stencil_test) {
        glDisable(GL_STENCIL_TEST);
        stencil_test = GL_FALSE;
    }
}

void gl_rend_state_struct::enable_blend(bool force) {
    if (force || !blend) {
        glEnable(GL_BLEND);
        blend = GL_TRUE;
    }
}

void gl_rend_state_struct::enable_cull_face(bool force) {
    if (force || !cull_face) {
        glEnable(GL_CULL_FACE);
        cull_face = GL_TRUE;
    }
}

void gl_rend_state_struct::enable_depth_test(bool force) {
    if (force || !depth_test) {
        glEnable(GL_DEPTH_TEST);
        depth_test = GL_TRUE;
    }
}

void gl_rend_state_struct::enable_multisample(bool force) {
    if (force || !multisample) {
        glEnable(GL_MULTISAMPLE);
        multisample = GL_TRUE;
    }
}

void gl_rend_state_struct::enable_primitive_restart(bool force) {
    if (force || !primitive_restart) {
        glEnable(GL_PRIMITIVE_RESTART);
        primitive_restart = GL_TRUE;
    }
}

void gl_rend_state_struct::enable_scissor_test(bool force) {
    if (force || !scissor_test) {
        glEnable(GL_SCISSOR_TEST);
        scissor_test = GL_TRUE;
    }
}

void gl_rend_state_struct::enable_stencil_test(bool force) {
    if (force || !stencil_test) {
        glEnable(GL_STENCIL_TEST);
        stencil_test = GL_TRUE;
    }
}

void gl_rend_state_struct::get() {
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

GLuint gl_rend_state_struct::get_program() {
    return program;
}

gl_rend_state_rect gl_rend_state_struct::get_scissor() {
    return scissor_box;
}

void gl_rend_state_struct::get_scissor(GLint& x, GLint& y, GLsizei& width, GLsizei& height) {
    x = scissor_box.x;
    y = scissor_box.y;
    width = scissor_box.width;
    height = scissor_box.height;
}

gl_rend_state_rect gl_rend_state_struct::get_viewport() {
    return viewport;
}

void gl_rend_state_struct::get_viewport(GLint& x, GLint& y, GLsizei& width, GLsizei& height) {
    x = viewport.x;
    y = viewport.y;
    width = viewport.width;
    height = viewport.height;
}

void gl_rend_state_struct::set_blend_func(GLenum src, GLenum dst, bool force) {
    if (force || blend_src_rgb != src || blend_dst_rgb != dst
        || blend_src_alpha != GL_ONE || blend_dst_alpha != GL_ONE_MINUS_SRC_ALPHA) {
        glBlendFuncSeparate(src, dst, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        blend_src_rgb = src;
        blend_dst_rgb = dst;
        blend_src_alpha = GL_ONE;
        blend_dst_alpha = GL_ONE_MINUS_SRC_ALPHA;
    }
}

void gl_rend_state_struct::set_blend_func_separate(GLenum src_rgb, GLenum dst_rgb,
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

void gl_rend_state_struct::set_blend_equation(GLenum mode, bool force) {
    if (force || blend_mode_rgb != mode || blend_mode_alpha != mode) {
        glBlendEquationSeparate(mode, mode);
        blend_mode_rgb = mode;
        blend_mode_alpha = mode;
    }
}

void gl_rend_state_struct::set_blend_equation_separate(GLenum mode_rgb, GLenum mode_alpha, bool force) {
    if (force || blend_mode_rgb != mode_rgb || blend_mode_alpha != mode_alpha) {
        glBlendEquationSeparate(mode_rgb, mode_alpha);
        blend_mode_rgb = mode_rgb;
        blend_mode_alpha = mode_alpha;
    }
}

void gl_rend_state_struct::set_color_mask(GLboolean red, GLboolean green,
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

void gl_rend_state_struct::set_cull_face_mode(GLenum mode, bool force) {
    if (force || cull_face_mode != mode) {
        glCullFace(mode);
        cull_face_mode = mode;
    }
}

void gl_rend_state_struct::set_depth_func(GLenum func, bool force) {
    if (force || depth_func != func) {
        glDepthFunc(func);
        depth_func = func;
    }
}

void gl_rend_state_struct::set_depth_mask(GLboolean flag, bool force) {
    if (force || depth_mask != flag) {
        glDepthMask(flag);
        depth_mask = flag;
    }
}

void gl_rend_state_struct::set_line_width(GLfloat width, bool force) {
    if (force || line_width != width) {
        glLineWidth(width);
        line_width = width;
    }
}

void gl_rend_state_struct::set_polygon_mode(GLenum face, GLenum mode, bool force) {
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

void gl_rend_state_struct::set_primitive_restart_index(GLuint index, bool force) {
    if (force || primitive_restart_index != index) {
        glPrimitiveRestartIndex(index);
        primitive_restart_index = index;
    }
}

void gl_rend_state_struct::set_scissor(const gl_rend_state_rect& rect, bool force) {
    if (force || scissor_box.x != rect.x || scissor_box.y != rect.y
        || scissor_box.width != rect.width || scissor_box.height != rect.height) {
        glScissor(rect.x, rect.y, rect.width, rect.height);
        scissor_box = rect;
    }
}

void gl_rend_state_struct::set_scissor(GLint x, GLint y, GLsizei width, GLsizei height, bool force) {
    if (force || scissor_box.x != x || scissor_box.y != y
        || scissor_box.width != width || scissor_box.height != height) {
        glScissor(x, y, width, height);
        scissor_box.x = x;
        scissor_box.y = y;
        scissor_box.width = width;
        scissor_box.height = height;
    }
}

void gl_rend_state_struct::set_stencil_func(GLenum func, GLint ref, GLuint mask, bool force) {
    if (force || stencil_func != func || stencil_ref != ref || stencil_value_mask != mask) {
        glStencilFunc(func, ref, mask);
        stencil_func = func;
        stencil_ref = ref;
        stencil_value_mask = mask;
    }
}

void gl_rend_state_struct::set_stencil_mask(GLuint mask, bool force) {
    if (force || stencil_mask != mask) {
        glStencilMask(mask);
        stencil_mask = mask;
    }
}

void gl_rend_state_struct::set_stencil_op(GLenum sfail, GLenum dpfail, GLenum dppass, bool force) {
    if (force || stencil_fail != sfail || stencil_dpfail != dpfail || stencil_dppass != dppass) {
        glStencilOp(sfail, dpfail, dppass);
        stencil_fail = sfail;
        stencil_dpfail = dpfail;
        stencil_dppass = dppass;
    }
}

void gl_rend_state_struct::set_viewport(const gl_rend_state_rect& rect, bool force) {
    if (force || viewport.x != rect.x || viewport.y != rect.y
        || viewport.width != rect.width || viewport.height != rect.height) {
        glViewport(rect.x, rect.y, rect.width, rect.height);
        viewport = rect;
    }
}

void gl_rend_state_struct::set_viewport(GLint x, GLint y, GLsizei width, GLsizei height, bool force) {
    if (force || viewport.x != x || viewport.y != y
        || viewport.width != width || viewport.height != height) {
        glViewport(x, y, width, height);
        viewport.x = x;
        viewport.y = y;
        viewport.width = width;
        viewport.height = height;
    }
}

void gl_rend_state_struct::use_program(GLuint program, bool force) {
    if (force || this->program != program) {
        glUseProgram(program);
        this->program = program;
    }
}
