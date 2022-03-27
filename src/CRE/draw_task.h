/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/mat.h"
#include "../KKdLib/vec.h"
#include "draw_object.h"
#include "draw_primitive.h"

extern void draw_task_draw_objects_by_type(render_context* rctx, draw_object_type type,
    int32_t a2, int32_t a3, bool a4, int32_t alpha);
extern void draw_task_draw_objects_by_type_translucent(render_context* rctx, bool opaque_enable,
    bool transparent_enable, bool translucent_enable, draw_object_type opaque,
    draw_object_type transparent, draw_object_type translucent);
extern int32_t draw_task_get_count(render_context* rctx, draw_object_type type);
extern bool draw_task_add_draw_object(render_context* rctx, obj* object,
    obj_mesh_vertex_buffer* obj_vertex_buf, obj_mesh_index_buffer* obj_index_buf, mat4* mat,
    std::vector<GLuint>* textures, vec4* blend_color, mat4* bone_mat, obj* object_morph,
    obj_mesh_vertex_buffer* obj_morph_vertex_buf, int32_t instances_count,
    mat4* instances_mat, void(*draw_object_func)(draw_object*), bool enable_bone_mat);
extern bool draw_task_add_draw_object_by_object_info(render_context* rctx, mat4* mat,
    object_info obj_info, vec4* blend_color, mat4* bone_mat, int32_t instances_count,
    mat4* instances_mat, void(*draw_object_func)(draw_object*), bool enable_bone_mat);
extern bool draw_task_add_draw_object_by_object_info_alpha(render_context* rctx, mat4* mat,
    object_info obj_info, float_t alpha);
extern bool draw_task_add_draw_object_by_object_info_alpha_bone_mat(render_context* rctx, mat4* mat,
    object_info obj_info, float_t alpha, mat4* bone_mat);
extern bool draw_task_add_draw_object_by_object_info_bone_mat(render_context* rctx, mat4* mat,
    object_info obj_info, mat4* bone_mat);
extern bool draw_task_add_draw_object_by_object_info_color(render_context* rctx, mat4* mat,
    object_info obj_info, float_t r, float_t g, float_t b, float_t a);
extern bool draw_task_add_draw_object_by_object_info_color_vec4(render_context* rctx, mat4* mat,
    object_info obj_info, vec4* blend_color);
extern void draw_task_add_draw_object_by_object_info_object_skin(render_context* rctx, object_info obj_info,
    std::vector<texture_pattern_struct>* texture_pattern, texture_data_struct* texture_data, float_t alpha,
    mat4* matrices, mat4* ex_data_matrices, mat4* mat, mat4* global_mat);
extern bool draw_task_add_draw_object_by_object_info_opaque(render_context* rctx,
    mat4* mat, object_info obj_info);
extern void draw_task_sort(render_context* rctx, draw_object_type type, int32_t compare_func);
extern int32_t obj_axis_aligned_bounding_box_check_visibility(
    obj_axis_aligned_bounding_box* aabb, camera* cam, mat4* mat);
extern int32_t object_bounding_sphere_check_visibility(obj_bounding_sphere* sphere,
    object_data* object_data, camera* cam, mat4* mat);
