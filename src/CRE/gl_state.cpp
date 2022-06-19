/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "gl_state.hpp"

gl_state_struct gl_state;

inline void gl_state_active_bind_texture_2d(int32_t index, GLuint texture) {
    if (gl_state.texture_binding_2d[index] != texture) {
        if (gl_state.active_texture_index != index) {
            glActiveTexture((GLenum)(GL_TEXTURE0 + index));
            gl_state.active_texture = (GLenum)(GL_TEXTURE0 + index);
            gl_state.active_texture_index = (GLuint)index;
        }
        glBindTexture(GL_TEXTURE_2D, texture);
        gl_state.texture_binding_2d[index] = texture;
    }
}

inline void gl_state_active_bind_texture_cube_map(int32_t index, GLuint texture) {
    if (gl_state.texture_binding_cube_map[index] != texture) {
        if (gl_state.active_texture_index != index) {
            glActiveTexture((GLenum)(GL_TEXTURE0 + index));
            gl_state.active_texture = (GLenum)(GL_TEXTURE0 + index);
            gl_state.active_texture_index = (GLuint)index;
        }
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        gl_state.texture_binding_cube_map[index] = texture;
    }
}

inline void gl_state_active_texture(size_t index) {
    if (gl_state.active_texture_index != index) {
        glActiveTexture((GLenum)(GL_TEXTURE0 + index));
        gl_state.active_texture = (GLenum)(GL_TEXTURE0 + index);
        gl_state.active_texture_index = (GLuint)index;
    }
}

inline void gl_state_bind_framebuffer(GLuint framebuffer) {
    if (gl_state.framebuffer_binding != framebuffer) {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        gl_state.framebuffer_binding = framebuffer;
    }
}

inline void gl_state_bind_vertex_array(GLuint array) {
    if (gl_state.vertex_array_binding != array) {
        glBindVertexArray(array);
        gl_state.vertex_array_binding = array;
    }
}

inline void gl_state_bind_array_buffer(GLuint buffer) {
    if (gl_state.array_buffer_binding != buffer) {
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        gl_state.array_buffer_binding = buffer;
    }
}

inline void gl_state_bind_element_array_buffer(GLuint buffer) {
    if (gl_state.element_array_buffer_binding != buffer) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
        gl_state.element_array_buffer_binding = buffer;
    }
}

inline void gl_state_bind_uniform_buffer(GLuint buffer) {
    if (gl_state.uniform_buffer_binding[gl_state.uniform_buffer_index] != buffer) {
        glBindBuffer(GL_UNIFORM_BUFFER, buffer);
        gl_state.uniform_buffer_binding[gl_state.uniform_buffer_index] = buffer;
    }
}

inline void gl_state_bind_uniform_buffer_base(GLuint index, GLuint buffer) {
    if (gl_state.uniform_buffer_binding[index] != buffer) {
        glBindBufferBase(GL_UNIFORM_BUFFER, index, buffer);
        gl_state.uniform_buffer_binding[index] = buffer;
        gl_state.uniform_buffer_offset[index] = 0;
        gl_state.uniform_buffer_size[index] = -1;
        gl_state.uniform_buffer_index = index;
    }
}

inline void gl_state_bind_uniform_buffer_range(GLuint index,
    GLuint buffer, GLintptr offset, GLsizeiptr size) {
    if (gl_state.uniform_buffer_binding[index] != buffer
        || gl_state.uniform_buffer_offset[index] != offset
        || gl_state.uniform_buffer_size[index] != size) {
        glBindBufferRange(GL_UNIFORM_BUFFER, index, buffer, offset, size);
        gl_state.uniform_buffer_binding[index] = buffer;
        gl_state.uniform_buffer_offset[index] = offset;
        gl_state.uniform_buffer_size[index] = size;
        gl_state.uniform_buffer_index = index;
    }
}

inline void gl_state_bind_shader_storage_buffer(GLuint buffer) {
    if (gl_state.shader_storage_buffer_binding[gl_state.shader_storage_buffer_index] != buffer) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
        gl_state.shader_storage_buffer_binding[gl_state.shader_storage_buffer_index] = buffer;
    }
}

inline void gl_state_bind_shader_storage_buffer_base(GLuint index, GLuint buffer) {
    if (gl_state.shader_storage_buffer_binding[index] != buffer) {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, buffer);
        gl_state.shader_storage_buffer_binding[index] = buffer;
        gl_state.shader_storage_buffer_offset[index] = 0;
        gl_state.shader_storage_buffer_size[index] = -1;
        gl_state.shader_storage_buffer_index = index;
    }
}

inline void gl_state_bind_shader_storage_buffer_range(GLuint index,
    GLuint buffer, GLintptr offset, GLsizeiptr size) {
    if (gl_state.shader_storage_buffer_binding[index] != buffer
        || gl_state.shader_storage_buffer_offset[index] != offset
        || gl_state.shader_storage_buffer_size[index] != size) {
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, index, buffer, offset, size);
        gl_state.shader_storage_buffer_binding[index] = buffer;
        gl_state.shader_storage_buffer_offset[index] = offset;
        gl_state.shader_storage_buffer_size[index] = size;
        gl_state.shader_storage_buffer_index = index;
    }
}

inline void gl_state_bind_texture_2d(GLuint texture) {
    if (gl_state.texture_binding_2d[gl_state.active_texture_index] != texture) {
        glBindTexture(GL_TEXTURE_2D, texture);
        gl_state.texture_binding_2d[gl_state.active_texture_index] = texture;
    }
}

inline void gl_state_bind_texture_cube_map(GLuint texture) {
    if (gl_state.texture_binding_cube_map[gl_state.active_texture_index] != texture) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        gl_state.texture_binding_cube_map[gl_state.active_texture_index] = texture;
    }
}

inline void gl_state_bind_sampler(int32_t index, GLuint sampler) {
    if (gl_state.sampler_binding[index] != sampler) {
        glBindSampler(index, sampler);
        gl_state.sampler_binding[index] = sampler;
    }
}

inline bool gl_state_check_uniform_buffer_binding() {
    return gl_state.uniform_buffer_binding[gl_state.uniform_buffer_index] != 0;
}

inline bool gl_state_check_uniform_buffer_binding_base(size_t index) {
    return gl_state.uniform_buffer_binding[index] != 0;
}

inline bool gl_state_check_shader_storage_buffer_binding() {
    return gl_state.shader_storage_buffer_binding[gl_state.shader_storage_buffer_index] != 0;
}

inline bool gl_state_check_shader_storage_buffer_binding_base(size_t index) {
    return gl_state.shader_storage_buffer_binding[index] != 0;
}

inline bool gl_state_check_texture_binding_2d(size_t index) {
    return gl_state.texture_binding_2d[index] != 0;
}

inline bool gl_state_check_texture_binding_cube_map(size_t index) {
    return gl_state.texture_binding_cube_map[index] != 0;
}

inline bool gl_state_check_sampler_binding(int32_t index, GLuint sampler) {
    return gl_state.sampler_binding[index] != 0;
}

inline void gl_state_disable_blend() {
    if (gl_state.blend) {
        glDisable(GL_BLEND);
        gl_state.blend = GL_FALSE;
    }
}

inline void gl_state_disable_cull_face() {
    if (gl_state.cull_face) {
        glDisable(GL_CULL_FACE);
        gl_state.cull_face = GL_FALSE;
    }
}

inline void gl_state_disable_depth_test() {
    if (gl_state.depth_test) {
        glDisable(GL_DEPTH_TEST);
        gl_state.depth_test = GL_FALSE;
    }
}

inline void gl_state_disable_primitive_restart() {
    if (!gl_state.primitive_restart) {
        glDisable(GL_PRIMITIVE_RESTART);
        gl_state.primitive_restart = GL_FALSE;
    }
}

inline void gl_state_disable_scissor_test() {
    if (gl_state.scissor_test) {
        glDisable(GL_SCISSOR_TEST);
        gl_state.scissor_test = GL_FALSE;
    }
}

inline void gl_state_disable_stencil_test() {
    if (gl_state.stencil_test) {
        glDisable(GL_STENCIL_TEST);
        gl_state.stencil_test = GL_FALSE;
    }
}

inline void gl_state_enable_blend() {
    if (!gl_state.blend) {
        glEnable(GL_BLEND);
        gl_state.blend = GL_TRUE;
    }
}

inline void gl_state_enable_cull_face() {
    if (!gl_state.cull_face) {
        glEnable(GL_CULL_FACE);
        gl_state.cull_face = GL_TRUE;
    }
}

inline void gl_state_enable_depth_test() {
    if (!gl_state.depth_test) {
        glEnable(GL_DEPTH_TEST);
        gl_state.depth_test = GL_TRUE;
    }
}

inline void gl_state_enable_primitive_restart() {
    if (!gl_state.primitive_restart) {
        glEnable(GL_PRIMITIVE_RESTART);
        gl_state.primitive_restart = GL_TRUE;
    }
}

inline void gl_state_enable_scissor_test() {
    if (!gl_state.scissor_test) {
        glEnable(GL_SCISSOR_TEST);
        gl_state.scissor_test = GL_TRUE;
    }
}

inline void gl_state_enable_stencil_test() {
    if (!gl_state.stencil_test) {
        glEnable(GL_STENCIL_TEST);
        gl_state.stencil_test = GL_TRUE;
    }
}

void gl_state_get() {
    glGetIntegerv(GL_CURRENT_PROGRAM, &gl_state.program);
    glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&gl_state.active_texture);
    gl_state.active_texture_index = gl_state.active_texture - GL_TEXTURE0;
    for (GLuint i = 0; i < 32; i++) {
        glActiveTexture((GLenum)(GL_TEXTURE0 + i));
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &gl_state.texture_binding_2d[i]);
        glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &gl_state.texture_binding_cube_map[i]);
        glGetIntegeri_v(GL_SAMPLER_BINDING, i, &gl_state.sampler_binding[i]);
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
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, (GLint*)&gl_state.vertex_array_binding);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (GLint*)&gl_state.array_buffer_binding);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, (GLint*)&gl_state.element_array_buffer_binding);

    GLuint uniform_buffer_binding;
    glGetIntegerv(GL_UNIFORM_BUFFER_BINDING, (GLint*)&uniform_buffer_binding);
    for (GLuint i = 0; i < 14; i++) {
        glGetIntegeri_v(GL_UNIFORM_BUFFER_BINDING, i, (GLint*)&gl_state.uniform_buffer_binding[i]);
        glGetIntegeri_v(GL_UNIFORM_BUFFER_START, i, (GLint*)&gl_state.uniform_buffer_offset[i]);
        glGetIntegeri_v(GL_UNIFORM_BUFFER_SIZE, i, (GLint*)&gl_state.uniform_buffer_size[i]);
        if (uniform_buffer_binding == gl_state.uniform_buffer_binding[i])
            gl_state.uniform_buffer_index = i;
    }

    GLuint shader_storage_buffer_binding;
    glGetIntegerv(GL_SHADER_STORAGE_BUFFER_BINDING, (GLint*)&shader_storage_buffer_binding);
    for (GLuint i = 0; i < 14; i++) {
        glGetIntegeri_v(GL_SHADER_STORAGE_BUFFER_BINDING, i, (GLint*)&gl_state.shader_storage_buffer_binding[i]);
        glGetIntegeri_v(GL_SHADER_STORAGE_BUFFER_START, i, (GLint*)&gl_state.shader_storage_buffer_offset[i]);
        glGetIntegeri_v(GL_SHADER_STORAGE_BUFFER_SIZE, i, (GLint*)&gl_state.shader_storage_buffer_size[i]);
        if (shader_storage_buffer_binding == gl_state.shader_storage_buffer_binding[i])
            gl_state.shader_storage_buffer_index = i;
    }

    glGetBooleanv(GL_COLOR_WRITEMASK, gl_state.color_mask);
    glGetBooleanv(GL_CULL_FACE, &gl_state.cull_face);
    glGetIntegerv(GL_CULL_FACE_MODE, (GLint*)&gl_state.cull_face_mode);
    glGetBooleanv(GL_DEPTH_TEST, &gl_state.depth_test);
    glGetIntegerv(GL_DEPTH_FUNC, (GLint*)&gl_state.depth_func);
    glGetBooleanv(GL_DEPTH_WRITEMASK, &gl_state.depth_mask);
    glGetBooleanv(GL_PRIMITIVE_RESTART, &gl_state.primitive_restart);
    glGetBooleanv(GL_SCISSOR_TEST, &gl_state.scissor_test);
    glGetBooleanv(GL_STENCIL_TEST, &gl_state.stencil_test);
    glGetIntegerv(GL_STENCIL_WRITEMASK, (GLint*)&gl_state.stencil_mask);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    gl_state.polygon_front_face_mode = GL_FILL;
    gl_state.polygon_back_face_mode = GL_FILL;
}

inline void gl_state_get_all_gl_errors() {
    GLenum error;
    while (error = glGetError())
        printf("GL Error: 0x%04X\n", error);
}

inline GLuint gl_state_get_program() {
    return gl_state.program;
}

inline void gl_state_set_blend_func(GLenum src, GLenum dst) {
    if (gl_state.blend_src_rgb != src || gl_state.blend_dst_rgb != dst
        || gl_state.blend_src_alpha != GL_ONE || gl_state.blend_dst_alpha != GL_ONE_MINUS_SRC_ALPHA) {
        glBlendFuncSeparate(src, dst, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        gl_state.blend_src_rgb = src;
        gl_state.blend_dst_rgb = dst;
        gl_state.blend_src_alpha = GL_ONE;
        gl_state.blend_dst_alpha = GL_ONE_MINUS_SRC_ALPHA;
    }
}

inline void gl_state_set_blend_func_separate(GLenum src_rgb, GLenum dst_rgb,
    GLenum src_alpha, GLenum dst_alpha) {
    if (gl_state.blend_src_rgb != src_rgb || gl_state.blend_dst_rgb != dst_rgb
        || gl_state.blend_src_alpha != src_alpha || gl_state.blend_dst_alpha != dst_alpha) {
        glBlendFuncSeparate(src_rgb, dst_rgb, src_alpha, dst_alpha);
        gl_state.blend_src_rgb = src_rgb;
        gl_state.blend_dst_rgb = dst_rgb;
        gl_state.blend_src_alpha = src_alpha;
        gl_state.blend_dst_alpha = dst_alpha;
    }
}

inline void gl_state_set_blend_equation(GLenum mode) {
    if (gl_state.blend_mode_rgb != mode || gl_state.blend_mode_alpha != mode) {
        glBlendEquationSeparate(mode, mode);
        gl_state.blend_mode_rgb = mode;
        gl_state.blend_mode_alpha = mode;
    }
}

inline void gl_state_set_blend_equation_separate(GLenum mode_rgb, GLenum mode_alpha) {
    if (gl_state.blend_mode_rgb != mode_rgb || gl_state.blend_mode_alpha != mode_alpha) {
        glBlendEquationSeparate(mode_rgb, mode_alpha);
        gl_state.blend_mode_rgb = mode_rgb;
        gl_state.blend_mode_alpha = mode_alpha;
    }
}

inline void gl_state_set_color_mask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha) {
    if (gl_state.color_mask[0] != red || gl_state.color_mask[1] != green
        || gl_state.color_mask[2] != blue || gl_state.color_mask[3] != alpha) {
        glColorMask(red, green, blue, alpha);
        gl_state.color_mask[0] = red;
        gl_state.color_mask[1] = green;
        gl_state.color_mask[2] = blue;
        gl_state.color_mask[3] = alpha;
    }
}

inline void gl_state_set_cull_face_mode(GLenum mode) {
    if (gl_state.cull_face_mode != mode) {
        glCullFace(mode);
        gl_state.cull_face_mode = mode;
    }
}

inline void gl_state_set_depth_func(GLenum func) {
    if (gl_state.depth_func != func) {
        glDepthFunc(func);
        gl_state.depth_func = func;
    }
}

inline void gl_state_set_depth_mask(GLboolean flag) {
    if (gl_state.depth_mask != flag) {
        glDepthMask(flag);
        gl_state.depth_mask = flag;
    }
}

inline void gl_state_set_polygon_mode(GLenum face, GLenum mode) {
    switch (face) {
    case GL_FRONT:
        if (gl_state.polygon_front_face_mode != mode) {
            glPolygonMode(GL_FRONT, mode);
            gl_state.polygon_front_face_mode = mode;
        }
        break;
    case GL_BACK:
        if (gl_state.polygon_back_face_mode != mode) {
            glPolygonMode(GL_BACK, mode);
            gl_state.polygon_back_face_mode = mode;
        }
        break;
    case GL_FRONT_AND_BACK:
        if (gl_state.polygon_front_face_mode != mode && gl_state.polygon_back_face_mode != mode) {
            glPolygonMode(GL_FRONT_AND_BACK, mode);
            gl_state.polygon_front_face_mode = mode;
            gl_state.polygon_back_face_mode = mode;
        }
        else if (gl_state.polygon_front_face_mode != mode) {
            glPolygonMode(GL_FRONT, mode);
            gl_state.polygon_front_face_mode = mode;
        }
        else if (gl_state.polygon_back_face_mode != mode) {
            glPolygonMode(GL_BACK, mode);
            gl_state.polygon_back_face_mode = mode;
        }
        break;
    }
}

inline void gl_state_set_stencil_mask(GLuint mask) {
    if (gl_state.stencil_mask != mask) {
        glStencilMask(mask);
        gl_state.stencil_mask = mask;
    }
}

inline void gl_state_use_program(GLuint program) {
    if (gl_state.program != program) {
        glUseProgram(program);
        gl_state.program = program;
    }
}
