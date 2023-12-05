/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../KKdLib/mat.hpp"
#include "../../KKdLib/vec.hpp"
#include "../object.hpp"
#include "../render_context.hpp"

namespace mdl {
    extern void draw(render_context* rctx, obj_primitive_type primitive_type, uint32_t count,
        uint16_t start, uint16_t end, obj_index_format index_format, size_t indices);
    extern void draw_etc_obj(render_context* rctx, mdl::EtcObj* etc);
    extern void draw_sub_mesh(render_context* rctx, const ObjSubMeshArgs* args, const mat4* model,
        void(*func)(render_context* rctx, const ObjSubMeshArgs* args));
    /*extern void draw_sub_mesh_show_vector(render_context* rctx,
        const ObjSubMeshArgs* args, const mat4* model, int32_t show_vector);*/
    extern void draw_sub_mesh_default(render_context* rctx, const ObjSubMeshArgs* args);
    extern void draw_sub_mesh_default_instanced(render_context* rctx,
        const ObjSubMeshArgs* args, const mat4* mat);
    extern void draw_sub_mesh_sss(render_context* rctx, const ObjSubMeshArgs* args);
    extern void draw_sub_mesh_reflect(render_context* rctx, const ObjSubMeshArgs* args);
    extern void draw_sub_mesh_reflect_reflect_map(render_context* rctx, const ObjSubMeshArgs* args);
    extern void draw_sub_mesh_shadow(render_context* rctx, const ObjSubMeshArgs* args);
    extern void draw_sub_mesh_translucent(render_context* rctx, const ObjSubMeshArgs* args);
};

extern void draw_object_model_mat_load(render_context* rctx, const mat4& mat);
extern void model_mat_face_camera_position(const mat4* view, const mat4* src, mat4* dst);
extern void model_mat_face_camera_view(const mat4* view, const mat4* src, mat4* dst);
