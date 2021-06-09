/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "static_var.h"
#define GLEW_STATIC
#include <GLEW/glew.h>

int32_t sv_max_texture_buffer_size = 0x100000;
int32_t sv_max_texture_size = 2048;
int32_t sv_max_texture_max_anisotropy = 1;

bool sv_anisotropy_changed = false;
int32_t sv_old_anisotropy = 1;
int32_t sv_anisotropy = 1;

bool sv_fxaa_changed = false;
bool sv_old_fxaa = false;
bool sv_fxaa = false;

bool sv_fxaa_preset_changed = false;
int32_t sv_old_fxaa_preset = 5;
int32_t sv_fxaa_preset = 5;

int32_t uniform_value[U_MAX];

static int32_t active_tex;
static int32_t current_fbo;
static int32_t current_vao;
static int32_t current_vbo;
static int32_t current_ebo;
static int32_t current_ubo;
static int32_t current_ssbo;
static int32_t current_ubo_binding[12];
static int32_t current_ssbo_binding[12];
static size_t current_ubo_binding_offset[12];
static size_t current_ssbo_binding_offset[12];
static size_t current_ubo_binding_size[12];
static size_t current_ssbo_binding_size[12];
static int32_t current_tex1d[32];
static int32_t current_tex2d[32];
static int32_t current_tex3d[32];
static int32_t current_texcube[32];

const vec3 sv_rgb_to_luma = {
    0.2126f, 0.7152f, 0.0722f
};

const mat3 sv_rgb_to_ypbpr = {
    { 0.21260f, -0.11458f,  0.50000f },
    { 0.71520f, -0.38542f, -0.45415f },
    { 0.07220f,  0.50000f, -0.04585f },
};

const mat3 sv_ypbpr_to_rgb = {
    { 1.00000f,  1.00000f, 1.00000f },
    { 0.00000f, -0.18732f, 1.85560f },
    { 1.57480f, -0.46812f, 0.00000f },
};

void sv_anisotropy_set(int32_t value) {
    sv_anisotropy = 1 << (int32_t)roundf(log2f((float_t)clamp(value, 1, sv_max_texture_max_anisotropy)));
    if (sv_anisotropy != sv_old_anisotropy)
        sv_anisotropy_changed = true;
    sv_old_anisotropy = sv_anisotropy;
}

void sv_fxaa_set(bool value) {
    sv_fxaa = value;
    if (sv_fxaa != sv_old_fxaa)
        sv_fxaa_changed = true;
    sv_old_fxaa = sv_fxaa;
}

void sv_fxaa_preset_set(int32_t value) {
    sv_fxaa_preset = clamp(value, 3, 5);
    if (sv_fxaa_preset != sv_old_fxaa_preset)
        sv_fxaa_changed = true;
    sv_old_fxaa_preset = sv_fxaa_preset;
}

inline void bind_index_tex1d(int32_t index, int32_t id) {
    if (current_tex1d[index] != id) {
        active_texture(index);
        glBindTexture(GL_TEXTURE_1D, id);
        current_tex1d[index] = id;
    }
}

inline void bind_index_tex2d(int32_t index, int32_t id) {
    if (current_tex2d[index] != id) {
        active_texture(index);
        glBindTexture(GL_TEXTURE_2D, id);
        current_tex2d[index] = id;
    }
}

inline void bind_index_tex3d(int32_t index, int32_t id) {
    if (current_tex3d[index] != id) {
        active_texture(index);
        glBindTexture(GL_TEXTURE_3D, id);
        current_tex3d[index] = id;
    }
}

inline void bind_index_texcube(int32_t index, int32_t id) {
    if (current_texcube[index] != id) {
        active_texture(index);
        glBindTexture(GL_TEXTURE_CUBE_MAP, id);
        current_texcube[index] = id;
    }
}

inline void active_texture(int32_t index) {
    if (active_tex != index) {
        glActiveTexture(GL_TEXTURE0 + index);
        active_tex = index;
    }
}

inline void bind_framebuffer(int32_t framebuffer) {
    if (current_fbo != framebuffer) {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        current_fbo = framebuffer;
    }
}

inline void bind_vertex_array(int32_t array) {
    if (current_vao != array) {
        glBindVertexArray(array);
        current_vao = array;
    }
}

inline void bind_array_buffer(int32_t buffer) {
    if (current_vbo != buffer) {
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        current_vbo = buffer;
    }
}

inline void bind_element_array_buffer(int32_t buffer) {
    if (current_ebo != buffer) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
        current_ebo = buffer;
    }
}

inline void bind_uniform_buffer(int32_t buffer) {
    if (current_ubo != buffer) {
        glBindBuffer(GL_UNIFORM_BUFFER, buffer);
        current_ubo = buffer;
    }
}

inline void bind_shader_storage_buffer(int32_t buffer) {
    if (current_ssbo != buffer) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
        current_ssbo = buffer;
    }
}

inline void bind_uniform_buffer_base(int32_t index, int32_t buffer) {
    if (current_ubo_binding[index] != buffer) {
        glBindBufferBase(GL_UNIFORM_BUFFER, index, buffer);
        current_ubo_binding[index] = buffer;
        current_ubo_binding_offset[index] = 0;
        current_ubo_binding_size[index] = -1;
    }
}

inline void bind_shader_storage_buffer_base(int32_t index, int32_t buffer) {
    if (current_ssbo_binding[index] != buffer) {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, buffer);
        current_ssbo_binding[index] = buffer;
        current_ssbo_binding_offset[index] = 0;
        current_ssbo_binding_size[index] = -1;
    }
}

inline void bind_uniform_buffer_range(int32_t index, int32_t buffer, size_t offset, size_t size) {
    if (current_ubo_binding[index] != buffer
        || current_ubo_binding_offset[index] != offset
        || current_ubo_binding_size[index] != size) {
        glBindBufferRange(GL_UNIFORM_BUFFER, index, buffer, offset, size);
        current_ubo_binding[index] = buffer;
        current_ubo_binding_offset[index] = offset;
        current_ubo_binding_size[index] = size;
    }
}

inline void bind_shader_storage_buffer_range(int32_t index, int32_t buffer, size_t offset, size_t size) {
    if (current_ssbo_binding[index] != buffer
        || current_ssbo_binding_offset[index] != offset
        || current_ssbo_binding_size[index] != size) {
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, index, buffer, offset, size);
        current_ssbo_binding[index] = buffer;
        current_ssbo_binding_offset[index] = offset;
        current_ssbo_binding_size[index] = size;
    }
}

inline void bind_tex1d(int32_t id) {
    if (current_tex1d[active_tex] != id) {
        glBindTexture(GL_TEXTURE_1D, id);
        current_tex1d[active_tex] = id;
    }
}

inline void bind_tex2d(int32_t id) {
    if (current_tex2d[active_tex] != id) {
        glBindTexture(GL_TEXTURE_2D, id);
        current_tex2d[active_tex] = id;
    }
}

inline void bind_tex3d(int32_t id) {
    if (current_tex3d[active_tex] != id) {
        glBindTexture(GL_TEXTURE_3D, id);
        current_tex3d[active_tex] = id;
    }
}

inline void bind_texcube(int32_t id) {
    if (current_texcube[active_tex] != id) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, id);
        current_texcube[active_tex] = id;
    }
}

inline bool check_uniform_buffer_binding_set(int32_t index) {
    return current_ubo_binding[index] != 0;
}

inline bool check_shader_storage_buffer_binding_set(int32_t index) {
    return current_ssbo_binding[index] != 0;
}

inline bool check_index_tex1d_set(int32_t index) {
    return current_tex1d[index] != 0;
}

inline bool check_index_tex2d_set(int32_t index) {
    return current_tex2d[index] != 0;
}

inline bool check_index_tex3d_set(int32_t index) {
    return current_tex3d[index] != 0;
}

inline bool check_index_texcube_set(int32_t index) {
    return current_texcube[index] != 0;
}
