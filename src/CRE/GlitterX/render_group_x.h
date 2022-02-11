/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../Glitter/glitter.h"

extern glitter_render_group* glitter_x_render_group_init(glitter_particle_inst* a1);
extern void glitter_x_render_group_calc_disp(GPM, glitter_render_group* a1);
extern void glitter_x_render_group_ctrl(glitter_render_group* rg,
    float_t delta_frame, bool copy_mats);
extern bool glitter_x_render_group_cannot_disp(glitter_render_group* a1);
extern void glitter_x_render_group_disp(GPM, glitter_render_group* a1);
extern void glitter_x_render_group_delete_buffers(glitter_render_group* a1, bool a2);
extern void glitter_x_render_group_emit(glitter_render_group* a1,
    glitter_particle_inst_data* a2, glitter_emitter_inst* a3, int32_t dup_count, int32_t count);
extern void glitter_x_render_group_free(glitter_render_group* a1);
extern void glitter_x_render_group_dispose(glitter_render_group* rg);
