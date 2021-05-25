/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "static_var.h"

int32_t sv_max_samples = 1;
int32_t sv_max_texture_buffer_size = 0x100000;
int32_t sv_max_texture_size = 2048;
int32_t sv_max_texture_max_anisotropy = 1;

bool sv_anisotropy_changed = false;
int32_t sv_old_anisotropy = 1;
int32_t sv_anisotropy = 1;

bool sv_samples_changed = false;
int32_t sv_old_samples = 1;
int32_t sv_samples = 1;

bool sv_fxaa_changed = false;
bool sv_old_fxaa = false;
bool sv_fxaa = false;

static int32_t active_tex;
static int32_t current_fbo;
static int32_t current_vao;
static int32_t current_vbo;
static int32_t current_ebo;
static int32_t current_ubo;
static int32_t current_tex1d[32];
static int32_t current_tex2d[32];
static int32_t current_tex2dms[32];
static int32_t current_tex3d[32];
static int32_t current_texcube[32];

const vec3 sv_rgb_to_luma = {
    0.2126f, 0.7152f, 0.0722f
};

const mat3 sv_rgb_to_ypbpr = {
    {0.21260f, -0.11458f, 0.50000f},
    {0.71520f, -0.38542f, -0.45415f},
    {0.07220f, 0.50000f, -0.04585f},
};

const mat3 sv_ypbpr_to_rgb = {
    {1.00000f, 1.00000f, 1.00000f},
    {0.00000f, -0.18732f, 1.85560f},
    {1.57480f, -0.46812f, 0.00000f},
};

inline bool sv_can_use_msaa() {
    return sv_max_samples > 1;
}

void sv_anisotropy_set(int32_t value) {
    sv_samples = 1 << (int32_t)log2(clamp(value, 1, sv_max_samples));
    if (sv_anisotropy != sv_old_anisotropy)
        sv_anisotropy_changed = true;
    sv_old_anisotropy = sv_anisotropy;
}

void sv_samples_set(int32_t value) {
    sv_samples = 1 << (int32_t)log2(clamp(value, 1, sv_max_samples));
    if (sv_samples != sv_old_samples)
        sv_samples_changed = true;
    sv_old_samples = sv_samples;
}

void sv_fxaa_set(bool value) {
    sv_fxaa = value;
    if (sv_fxaa != sv_old_fxaa)
        sv_fxaa_changed = true;
    sv_old_fxaa = sv_fxaa;
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

inline void bind_index_tex2dms(int32_t index, int32_t id) {
    if (current_tex2dms[index] != id) {
        active_texture(index);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, id);
        current_tex2dms[index] = id;
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

inline void bind_framebuffer(int32_t fbo) {
    if (current_fbo != fbo) {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        current_fbo = fbo;
    }
}

inline void bind_vertex_array(int32_t vao) {
    if (current_vao != vao) {
        glBindVertexArray(vao);
        current_vao = vao;
    }
}

inline void bind_array_buffer(int32_t vbo) {
    if (current_vbo != vbo) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        current_vbo = vbo;
    }
}

inline void bind_element_array_buffer(int32_t ebo) {
    if (current_ebo != ebo) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        current_ebo = ebo;
    }
}

inline void bind_uniform_buffer(int32_t ubo) {
    if (current_ubo != ubo) {
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        current_ubo = ubo;
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

inline void bind_tex2dms(int32_t id) {
    if (current_tex2dms[active_tex] != id) {
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, id);
        current_tex2dms[active_tex] = id;
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
