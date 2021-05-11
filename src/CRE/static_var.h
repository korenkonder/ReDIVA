/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/vec.h"
#include "../KKdLib/mat.h"
#define GLEW_STATIC
#include <GLEW/glew.h>

extern int32_t sv_max_samples;
extern int32_t sv_max_texture_buffer_size;
extern int32_t sv_max_texture_size;
extern int32_t sv_max_texture_max_anisotropy;

extern bool sv_anisotropy_changed;
extern int32_t sv_anisotropy;

extern bool sv_samples_changed;
extern int32_t sv_samples;

extern bool sv_fxaa_changed;
extern bool sv_fxaa;

extern const vec3 sv_rgb_to_luma;
extern const mat3 sv_rgb_to_ypbpr;
extern const mat3 sv_ypbpr_to_rgb;

extern bool sv_can_use_msaa();
extern void sv_anisotropy_set(int32_t value);
extern void sv_samples_set(int32_t value);
extern void sv_fxaa_set(bool value);

extern void bind_index_tex1d(int32_t index, int32_t id);
extern void bind_index_tex2d(int32_t index, int32_t id);
extern void bind_index_tex2dms(int32_t index, int32_t id);
extern void bind_index_tex3d(int32_t index, int32_t id);
extern void bind_index_texcube(int32_t index, int32_t id);
extern void active_texture(int32_t index);
extern void bind_framebuffer(int32_t fbo);
extern void bind_vertex_array(int32_t vao);
extern void bind_array_buffer(int32_t vbo);
extern void bind_element_array_buffer(int32_t ebo);
extern void bind_uniform_buffer(int32_t ubo);
extern void bind_tex1d(int32_t id);
extern void bind_tex2d(int32_t id);
extern void bind_tex2dms(int32_t id);
extern void bind_tex3d(int32_t id);
extern void bind_texcube(int32_t id);
