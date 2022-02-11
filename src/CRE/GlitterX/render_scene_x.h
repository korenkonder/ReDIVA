/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../Glitter/glitter.h"

extern void glitter_x_render_scene_append(glitter_render_scene* rs,
    glitter_render_group* rg);
extern void glitter_x_render_scene_calc_disp(GPM, glitter_render_scene* rs);
extern void glitter_x_render_scene_ctrl(glitter_render_scene* rs,
    float_t delta_frame, bool copy_mats);
extern void glitter_x_render_scene_disp(GPM, glitter_render_scene* rs, draw_pass_3d_type alpha);
extern void glitter_x_render_scene_free(glitter_render_scene* rs);
