/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "glitter.h"

extern void glitter_render_element_emit(GPM, GLT, glitter_render_element* a1,
    GlitterF2ParticleInstData* a2, GlitterF2EmitterInst* a3, int32_t index, glitter_random* random);
extern void glitter_render_element_ctrl(GLT, GlitterF2RenderGroup* a1,
    glitter_render_element* a2, float_t delta_frame);
extern void glitter_render_element_rotate_to_emit_position(mat4* mat,
    GlitterF2RenderGroup* a2, glitter_render_element* a3, vec3* vec);
extern void glitter_render_element_rotate_to_prev_position(mat4* mat,
    GlitterF2RenderGroup* a2, glitter_render_element* a3, vec3* vec);
extern void glitter_render_element_free(glitter_render_element* a1);
