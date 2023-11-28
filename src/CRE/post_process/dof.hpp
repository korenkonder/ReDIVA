/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../KKdLib/vec.hpp"
#include "../GL/uniform_buffer.hpp"
#include "../camera.hpp"
#include "../fbo.hpp"
#include "../render_texture.hpp"
#include "../shared.hpp"

enum dof_debug_flags {
    DOF_DEBUG_USE_UI_PARAMS   = 0x01,
    DOF_DEBUG_ENABLE_DOF      = 0x02,
    DOF_DEBUG_ENABLE_PHYS_DOF = 0x04,
    DOF_DEBUG_AUTO_FOCUS      = 0x08,
};

struct dof_f2 {
    float_t focus;
    float_t focus_range;
    float_t fuzzing_range;
    float_t ratio;
};

struct dof_debug {
    dof_debug_flags flags;
    float_t focus;
    float_t focal_length;
    float_t f_number;
    dof_f2 f2;
};

struct dof_pv {
    bool enable;
    dof_f2 f2;
};

struct post_process_dof {
    int32_t width;
    int32_t height;
    GLuint textures[6];
    fbo fbo[4];
    GLuint samplers[2];
    GLuint vao;
    GL::UniformBuffer common_ubo;
    GL::UniformBuffer texcoords_ubo;

    post_process_dof();
    ~post_process_dof();

    void apply(RenderTexture* rt, RenderTexture* buf, camera* cam);
    void init_fbo(int32_t width, int32_t height);
};

extern dof_debug dof_debug_data;
extern dof_pv dof_pv_data;

extern void dof_debug_get(dof_debug* debug);
extern void dof_debug_set(dof_debug* debug = 0);
extern void dof_pv_get(dof_pv* pv);
extern void dof_pv_set(dof_pv* pv = 0);
