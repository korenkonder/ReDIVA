/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../Glitter/glitter.h"

extern void FASTCALL glitter_x_render_scene_append(glitter_render_scene* rs,
    glitter_render_group* rg);
extern void FASTCALL glitter_x_render_scene_calc_draw(GPM, glitter_render_scene* rs,
    bool(FASTCALL* render_add_list_func)(glitter_particle_mesh*, vec4*, mat4*, mat4*));
extern void FASTCALL glitter_x_render_scene_draw(GPM, glitter_render_scene* rs, int32_t alpha);
extern void FASTCALL glitter_x_render_scene_free(glitter_render_scene* rs);
extern void FASTCALL glitter_x_render_scene_update(glitter_render_scene* rs,
    float_t delta_frame, bool copy_mats);
