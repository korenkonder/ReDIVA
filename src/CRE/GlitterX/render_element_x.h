/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../Glitter/glitter.h"

extern void FASTCALL glitter_x_render_element_emit(glitter_render_element* a1,
    glitter_particle_inst_data* a2, glitter_emitter_inst* a3,
    int32_t index, uint8_t step, glitter_random* random);
extern void FASTCALL glitter_x_render_element_free(glitter_render_element* a1);
extern void FASTCALL glitter_x_render_element_rotate_mesh_to_position(mat4* mat,
    glitter_render_group* a2, glitter_render_element* a3, vec3* vec);
extern void FASTCALL glitter_x_render_element_rotate_mesh_to_prev_position(mat4* mat,
    glitter_render_group* a2, glitter_render_element* a3, vec3* vec);
extern void FASTCALL glitter_x_render_element_rotate_to_position(mat3* mat,
    glitter_render_group* a2, glitter_render_element* a3, vec3* vec);
extern void FASTCALL glitter_x_render_element_rotate_to_prev_position(mat3* mat,
    glitter_render_group* a2, glitter_render_element* a3, vec3* vec);
extern void FASTCALL glitter_x_render_element_update(glitter_render_group* a1,
    glitter_render_element* a2, float_t delta_frame);
