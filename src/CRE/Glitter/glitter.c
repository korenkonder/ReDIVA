/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.h"

vector_func(glitter_curve_key)
vector_ptr_func(glitter_curve)
vector_ptr_func(glitter_effect)
vector_ptr_func(glitter_effect_group)
vector_ptr_func(glitter_effect_inst)
vector_ptr_func(glitter_emitter)
vector_ptr_func(glitter_emitter_inst)
vector_ptr_func(glitter_file_reader)
vector_func(glitter_locus_history_data)
vector_ptr_func(glitter_particle)
vector_ptr_func(glitter_particle_inst)
vector_ptr_func(glitter_render_group)
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
