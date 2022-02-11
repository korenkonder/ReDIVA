/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/mat.h"
#include "../KKdLib/vec.h"
#include "../KKdLib/vector.h"
#include "object.h"
#include "render_context.h"

typedef struct texture_data_struct {
    int32_t field_0;
    vec3 texture_color_coeff;
    vec3 texture_color_offset;
    vec3 texture_specular_coeff;
    vec3 texture_specular_offset;
} texture_data_struct;

extern void draw_object_draw(render_context* rctx, draw_object* disp, mat4* model,
    void(*draw_object_func)(render_context* rctx, draw_object* disp), int32_t show_vector);
extern void draw_object_draw_default(render_context* rctx, draw_object* disp);
extern void draw_object_draw_sss(render_context* rctx, draw_object* disp);
extern void draw_object_draw_reflect(render_context* rctx, draw_object* disp);
extern void draw_object_draw_reflect_type_2(render_context* rctx, draw_object* disp);
extern void draw_object_draw_shadow(render_context* rctx, draw_object* disp);
extern void draw_object_draw_translucent(render_context* rctx, draw_object* disp);
extern void draw_object_model_mat_load(render_context* rctx, mat4* mat);
extern void model_mat_face_camera_position(mat4* view, mat4* src, mat4* dst);
extern void model_mat_face_camera_view(mat4* view, mat4* src, mat4* dst);
extern void object_sub_mesh_draw(render_context* rctx, object_primitive_type primitive_type,
    uint32_t count, uint16_t start, uint16_t end, object_index_format index_format, size_t indices);
