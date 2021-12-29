/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.h"

vector_func(glitter_curve_key)
vector_ptr_func(glitter_curve)
vector_ptr_func(glitter_effect)
vector_ptr_func(glitter_effect_group)
vector_ptr_func(glitter_emitter)
vector_ptr_func(glitter_emitter_inst)
vector_ptr_func(glitter_file_reader)
vector_func(glitter_locus_history_data)
vector_ptr_func(glitter_particle)
vector_ptr_func(glitter_particle_inst)
vector_ptr_func(glitter_render_group)
vector_func(glitter_scene_effect)
vector_ptr_func(glitter_scene)

const float_t glitter_min_emission = 0.01f;

const glitter_curve_type_flags glitter_effect_curve_flags = 0
    | GLITTER_CURVE_TYPE_TRANSLATION_XYZ
    | GLITTER_CURVE_TYPE_ROTATION_XYZ
    | GLITTER_CURVE_TYPE_SCALE_XYZ
    | GLITTER_CURVE_TYPE_SCALE_ALL;

const glitter_curve_type_flags glitter_emitter_curve_flags = 0
    | GLITTER_CURVE_TYPE_TRANSLATION_XYZ
    | GLITTER_CURVE_TYPE_ROTATION_XYZ
    | GLITTER_CURVE_TYPE_SCALE_XYZ
    | GLITTER_CURVE_TYPE_SCALE_ALL
    | GLITTER_CURVE_TYPE_EMISSION_INTERVAL
    | GLITTER_CURVE_TYPE_PARTICLES_PER_EMISSION;

const glitter_curve_type_flags glitter_particle_curve_flags = 0
    | GLITTER_CURVE_TYPE_TRANSLATION_XYZ
    | GLITTER_CURVE_TYPE_ROTATION_XYZ
    | GLITTER_CURVE_TYPE_SCALE_XYZ
    | GLITTER_CURVE_TYPE_SCALE_ALL
    | GLITTER_CURVE_TYPE_COLOR_RGBA
    | GLITTER_CURVE_TYPE_UV_SCROLL;

const glitter_curve_type_flags glitter_particle_x_curve_flags = 0
    | GLITTER_CURVE_TYPE_TRANSLATION_XYZ
    | GLITTER_CURVE_TYPE_ROTATION_XYZ
    | GLITTER_CURVE_TYPE_SCALE_XYZ
    | GLITTER_CURVE_TYPE_SCALE_ALL
    | GLITTER_CURVE_TYPE_COLOR_RGBA
    | GLITTER_CURVE_TYPE_COLOR_RGB_SCALE
    | GLITTER_CURVE_TYPE_UV_SCROLL
    | GLITTER_CURVE_TYPE_UV_SCROLL_2ND;

const glitter_direction glitter_emitter_direction_types[] = {
    [GLITTER_EMITTER_DIRECTION_BILLBOARD]        = GLITTER_DIRECTION_BILLBOARD,
    [GLITTER_EMITTER_DIRECTION_BILLBOARD_Y_AXIS] = GLITTER_DIRECTION_BILLBOARD_Y_AXIS,
    [GLITTER_EMITTER_DIRECTION_X_AXIS]           = GLITTER_DIRECTION_X_AXIS,
    [GLITTER_EMITTER_DIRECTION_Y_AXIS]           = GLITTER_DIRECTION_Y_AXIS,
    [GLITTER_EMITTER_DIRECTION_Z_AXIS]           = GLITTER_DIRECTION_Z_AXIS,
    [GLITTER_EMITTER_DIRECTION_EFFECT_ROTATION]  = GLITTER_DIRECTION_EFFECT_ROTATION,
};

const size_t glitter_emitter_direction_types_count =
    sizeof(glitter_emitter_direction_types) / sizeof(glitter_direction);

const glitter_direction glitter_emitter_direction_default_direction =
    GLITTER_DIRECTION_EFFECT_ROTATION;
const glitter_emitter_direction glitter_emitter_direction_default =
    GLITTER_EMITTER_DIRECTION_EFFECT_ROTATION;

const glitter_direction glitter_particle_draw_types[] = {
    [GLITTER_PARTICLE_DRAW_TYPE_BILLBOARD]         = GLITTER_DIRECTION_BILLBOARD,
    [GLITTER_PARTICLE_DRAW_TYPE_BILLBOARD_Y_AXIS]  = GLITTER_DIRECTION_BILLBOARD_Y_AXIS,
    [GLITTER_PARTICLE_DRAW_TYPE_X_AXIS]            = GLITTER_DIRECTION_X_AXIS,
    [GLITTER_PARTICLE_DRAW_TYPE_Y_AXIS]            = GLITTER_DIRECTION_Y_AXIS,
    [GLITTER_PARTICLE_DRAW_TYPE_Z_AXIS]            = GLITTER_DIRECTION_Z_AXIS,
    [GLITTER_PARTICLE_DRAW_TYPE_EMITTER_DIRECTION] = GLITTER_DIRECTION_EMITTER_DIRECTION,
    [GLITTER_PARTICLE_DRAW_TYPE_EMITTER_ROTATION]  = GLITTER_DIRECTION_EMITTER_ROTATION,
    [GLITTER_PARTICLE_DRAW_TYPE_PARTICLE_ROTATION] = GLITTER_DIRECTION_PARTICLE_ROTATION,
    [GLITTER_PARTICLE_DRAW_TYPE_PREV_POSITION]     = GLITTER_DIRECTION_PREV_POSITION,
    [GLITTER_PARTICLE_DRAW_TYPE_PREV_POSITION_DUP] = GLITTER_DIRECTION_PREV_POSITION_DUP,
    [GLITTER_PARTICLE_DRAW_TYPE_EMIT_POSITION]     = GLITTER_DIRECTION_EMIT_POSITION,
};

const size_t glitter_particle_draw_types_count =
    sizeof(glitter_particle_draw_types) / sizeof(glitter_direction);

const glitter_direction glitter_particle_draw_type_default_direction =
    GLITTER_DIRECTION_BILLBOARD;
const glitter_particle_draw_type glitter_particle_draw_type_default =
    GLITTER_PARTICLE_DRAW_TYPE_BILLBOARD;

const glitter_particle_blend glitter_particle_blend_draw_types[] = {
    [GLITTER_PARTICLE_BLEND_DRAW_TYPICAL]  = GLITTER_PARTICLE_BLEND_TYPICAL,
    [GLITTER_PARTICLE_BLEND_DRAW_ADD]      = GLITTER_PARTICLE_BLEND_ADD,
    [GLITTER_PARTICLE_BLEND_DRAW_MULTIPLY] = GLITTER_PARTICLE_BLEND_MULTIPLY,
};

const size_t glitter_particle_blend_draw_types_count =
    sizeof(glitter_particle_blend_draw_types) / sizeof(glitter_particle_blend);

const glitter_particle_blend glitter_particle_blend_draw_default_blend =
    GLITTER_PARTICLE_BLEND_TYPICAL;
const glitter_particle_blend_draw glitter_particle_blend_draw_default =
    GLITTER_PARTICLE_BLEND_DRAW_TYPICAL;

const glitter_particle_blend glitter_particle_blend_mask_types[] = {
    [GLITTER_PARTICLE_BLEND_MASK_TYPICAL]  = GLITTER_PARTICLE_BLEND_TYPICAL,
    [GLITTER_PARTICLE_BLEND_MASK_ADD]      = GLITTER_PARTICLE_BLEND_ADD,
    [GLITTER_PARTICLE_BLEND_MASK_MULTIPLY] = GLITTER_PARTICLE_BLEND_MULTIPLY,
};

const size_t glitter_particle_blend_mask_types_count =
    sizeof(glitter_particle_blend_mask_types) / sizeof(glitter_particle_blend);

const glitter_particle_blend glitter_particle_blend_mask_default_blend =
    GLITTER_PARTICLE_BLEND_TYPICAL;
const glitter_particle_blend_mask glitter_particle_blend_mask_default =
    GLITTER_PARTICLE_BLEND_MASK_TYPICAL;

const glitter_pivot glitter_pivot_reverse[] = {
    [GLITTER_PIVOT_TOP_LEFT]      = GLITTER_PIVOT_BOTTOM_RIGHT,
    [GLITTER_PIVOT_TOP_CENTER]    = GLITTER_PIVOT_BOTTOM_CENTER,
    [GLITTER_PIVOT_TOP_RIGHT]     = GLITTER_PIVOT_BOTTOM_LEFT,
    [GLITTER_PIVOT_MIDDLE_LEFT]   = GLITTER_PIVOT_MIDDLE_RIGHT,
    [GLITTER_PIVOT_MIDDLE_CENTER] = GLITTER_PIVOT_MIDDLE_CENTER,
    [GLITTER_PIVOT_MIDDLE_RIGHT]  = GLITTER_PIVOT_MIDDLE_LEFT,
    [GLITTER_PIVOT_BOTTOM_LEFT]   = GLITTER_PIVOT_TOP_RIGHT,
    [GLITTER_PIVOT_BOTTOM_CENTER] = GLITTER_PIVOT_TOP_CENTER,
    [GLITTER_PIVOT_BOTTOM_RIGHT]  = GLITTER_PIVOT_TOP_LEFT,
};

GPM;
