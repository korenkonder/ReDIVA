/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.h"
#include "../../KKdLib/vec.h"
#include "../camera.h"
#include "../fbo.hpp"
#include "../render_texture.h"
#include "../shared.h"

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

struct post_process_dof_data {
    dof_debug debug;
    dof_pv pv;
};

struct post_process_dof {
    post_process_dof_data data;
    int32_t width;
    int32_t height;
    GLuint textures[6];
    fbo fbo[4];
    GLuint vao;
    GLuint program[9];
    GLuint ubo[2];

    post_process_dof();
    virtual ~post_process_dof();

    void apply(render_texture* rt, GLuint* samplers, camera* cam);
    void init_fbo(int32_t width, int32_t height);
    void initialize_data(dof_debug* debug, dof_pv* pv);
    void get_dof_debug(dof_debug* debug);
    void set_dof_debug(dof_debug* debug);
    void get_dof_pv(dof_pv* pv);
    void set_dof_pv(dof_pv* pv);
};
