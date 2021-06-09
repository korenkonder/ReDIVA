/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern void FASTCALL glitter_render_element_emit(GPM, GLT, glitter_render_element* a1,
    glitter_particle_inst_data* a2, glitter_emitter_inst* a3, int32_t index, glitter_random* random);
extern void FASTCALL glitter_render_element_free(glitter_render_element* a1);
extern void FASTCALL glitter_render_element_update(GLT, glitter_render_group* a1,
    glitter_render_element* a2, float_t delta_frame);
extern void FASTCALL glitter_render_element_rotate_to_emit_position(mat4* mat,
    glitter_render_group* a2, glitter_render_element* a3, vec3* vec);
extern void FASTCALL glitter_render_element_rotate_to_prev_position(mat4* mat,
    glitter_render_group* a2, glitter_render_element* a3, vec3* vec);
