/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../Glitter/glitter.h"

extern void glitter_x_render_element_emit(glitter_render_element* a1,
    GlitterXParticleInstData* a2, GlitterXEmitterInst* a3,
    int32_t index, uint8_t step, glitter_random* random);
extern void glitter_x_render_element_ctrl(GlitterXRenderGroup* a1,
    glitter_render_element* a2, float_t delta_frame);
extern void glitter_x_render_element_free(glitter_render_element* a1);
extern void glitter_x_render_element_rotate_mesh_to_emit_position(mat4* mat,
    GlitterXRenderGroup* a2, glitter_render_element* a3, vec3* vec, vec3* trans);
extern void glitter_x_render_element_rotate_mesh_to_prev_position(mat4* mat,
    GlitterXRenderGroup* a2, glitter_render_element* a3, vec3* vec, vec3* trans);
extern void glitter_x_render_element_rotate_to_emit_position(mat3* mat,
    GlitterXRenderGroup* a2, glitter_render_element* a3, vec3* vec);
extern void glitter_x_render_element_rotate_to_prev_position(mat3* mat,
    GlitterXRenderGroup* a2, glitter_render_element* a3, vec3* vec);
