/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern void glitter_render_scene_append(glitter_render_scene* rs,
    glitter_render_group* rg);
extern void glitter_render_scene_calc_draw(GPM, glitter_render_scene* rs);
extern void glitter_render_scene_draw(GPM, glitter_render_scene* rs, alpha_pass_type alpha);
extern size_t glitter_render_scene_get_ctrl_count(glitter_render_scene* rs,
    glitter_particle_type type);
extern size_t glitter_render_scene_get_disp_count(glitter_render_scene* rs,
    glitter_particle_type type);
extern void glitter_render_scene_free(glitter_render_scene* rs);
extern void glitter_render_scene_update(GLT,
    glitter_render_scene* rs, float_t delta_frame);
