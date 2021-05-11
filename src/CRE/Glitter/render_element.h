/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern void FASTCALL glitter_render_element_emit(GPM,
    glitter_particle_inst* a1, glitter_render_element* a2, glitter_emitter_inst* a3,
    glitter_particle_inst_data* a4, int32_t index, uint8_t step, glitter_random* random);
extern void FASTCALL glitter_render_element_free(glitter_render_element* a1);
extern void FASTCALL glitter_render_element_get_value(GPM, glitter_render_group* a1,
    glitter_render_element* a2, float_t delta_frame);
extern void FASTCALL glitter_render_element_rotate_to_position(mat3* mat,
    glitter_render_group* a2, glitter_render_element* a3);
extern void FASTCALL glitter_render_element_rotate_to_prev_position(mat3* mat,
    glitter_render_group* a2, glitter_render_element* a3);
extern void FASTCALL glitter_render_element_x_rotate_to_position(mat3* mat,
    glitter_render_group* a2, glitter_render_element* a3, vec3* pos, vec3* direction);
extern void FASTCALL glitter_render_element_x_rotate_to_prev_position(mat3* mat,
    glitter_render_group* a2, glitter_render_element* a3, vec3* pos, vec3* direction);
