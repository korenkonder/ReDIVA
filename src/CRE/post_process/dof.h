/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.h"
#include "../../KKdLib/vec.h"
#include "../camera.h"
#include "../fbo.h"
#include "../render_texture.h"
#include "../shared.h"

typedef enum dof_debug_flags {
    DOF_DEBUG_USE_UI_PARAMS = 0x1,
    DOF_DEBUG_ENABLE_DOF = 0x2,
    DOF_DEBUG_ENABLE_PHYS_DOF = 0x4,
    DOF_DEBUG_AUTO_FOCUS = 0x8,
} dof_debug_flags;

typedef struct dof_f2 {
    float_t distance_to_focus;
    float_t focus_range;
    float_t fuzzing_range;
    float_t ratio;
} dof_f2;

typedef struct dof_debug {
    dof_debug_flags flags;
    float_t distance_to_focus;
    float_t focal_length;
    float_t f_number;
    dof_f2 f2;
} dof_debug;

typedef struct dof_pv {
    bool enable;
    dof_f2 f2;
} dof_pv;

typedef struct post_process_dof_data {
    dof_debug debug;
    dof_pv pv;
} post_process_dof_data;

typedef struct post_process_dof {
    post_process_dof_data data;
    int32_t width;
    int32_t height;
    GLuint textures[6];
    fbo_struct fbo[4];
    GLuint vao;
    GLuint program[9];
    GLuint ubo[2];
} post_process_dof;

extern post_process_dof* post_process_dof_init();
extern void post_process_apply_dof(post_process_dof* dof,
    render_texture* rt, GLuint* samplers, camera* cam);
extern void post_process_dof_init_fbo(post_process_dof* dof,
    int32_t width, int32_t height);
extern void post_process_dof_dispose(post_process_dof* dof);

extern void post_process_dof_initialize_data(post_process_dof* dof, dof_debug* debug, dof_pv* pv);
extern void post_process_dof_get_dof_debug(post_process_dof* dof, dof_debug* debug);
extern void post_process_dof_set_dof_debug(post_process_dof* dof, dof_debug* debug);
extern void post_process_dof_get_dof_pv(post_process_dof* dof, dof_pv* pv);
extern void post_process_dof_set_dof_pv(post_process_dof* dof, dof_pv* pv);
