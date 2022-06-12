/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"
#include "../draw_task.h"
#include "../gl_state.h"
#include "../render_context.hpp"
#include "../shader.h"
#include "../shader_ft.h"
#include "../static_var.h"

namespace Glitter {
    RenderScene::RenderScene() : ctrl_quad(), ctrl_line(), ctrl_locus(),
        ctrl_mesh(), disp_quad(), disp_line(), disp_locus(), disp_mesh() {

    }

    RenderScene::~RenderScene() {

    }

    size_t RenderScene::GetCtrlCount(ParticleType type) {
        switch (type) {
        case PARTICLE_QUAD:
            return ctrl_quad;
        case PARTICLE_LINE:
            return ctrl_line;
        case PARTICLE_LOCUS:
            return ctrl_locus;
        case PARTICLE_MESH:
            return ctrl_mesh;
        default:
            return 0;
        }
    }

    size_t RenderScene::GetDispCount(ParticleType type) {
        switch (type) {
        case PARTICLE_QUAD:
            return disp_quad;
        case PARTICLE_LINE:
            return disp_line;
        case PARTICLE_LOCUS:
            return disp_locus;
        case PARTICLE_MESH:
            return disp_mesh;
        default:
            return 0;
        }
    }

    void RenderScene::CalcDispLocusSetPivot(Pivot pivot,
        float_t w, float_t* v00, float_t* v01) {
        switch (pivot) {
        case PIVOT_TOP_LEFT:
        case PIVOT_MIDDLE_LEFT:
        case PIVOT_BOTTOM_LEFT:
            *v00 = 0.0f;
            *v01 = w;
            break;
        case PIVOT_TOP_CENTER:
        case PIVOT_MIDDLE_CENTER:
        case PIVOT_BOTTOM_CENTER:
        default:
            *v00 = w * -0.5f;
            *v01 = w * 0.5f;
            break;
        case PIVOT_TOP_RIGHT:
        case PIVOT_MIDDLE_RIGHT:
        case PIVOT_BOTTOM_RIGHT:
            *v00 = -w;
            *v01 = 0.0f;
            break;
        }
    }

    void RenderScene::CalcDispQuadSetPivot(Pivot pivot,
        float_t w, float_t h, float_t* v00, float_t* v01, float_t* v10, float_t* v11) {
        switch (pivot) {
        case PIVOT_TOP_LEFT:
            *v00 = 0.0f;
            *v01 = w;
            *v10 = -h;
            *v11 = 0.0f;
            break;
        case PIVOT_TOP_CENTER:
            *v00 = w * -0.5f;
            *v01 = w * 0.5f;
            *v10 = -h;
            *v11 = 0.0f;
            break;
        case PIVOT_TOP_RIGHT:
            *v00 = -w;
            *v01 = 0.0f;
            *v10 = -h;
            *v11 = 0.0f;
            break;
        case PIVOT_MIDDLE_LEFT:
            *v00 = 0.0f;
            *v01 = w;
            *v10 = h * -0.5f;
            *v11 = h * 0.5f;
            break;
        case PIVOT_MIDDLE_CENTER:
        default:
            *v00 = w * -0.5f;
            *v01 = w * 0.5f;
            *v10 = h * -0.5f;
            *v11 = h * 0.5f;
            break;
        case PIVOT_MIDDLE_RIGHT:
            *v00 = -w;
            *v01 = 0.0f;
            *v10 = h * -0.5f;
            *v11 = h * 0.5f;
            break;
        case PIVOT_BOTTOM_LEFT:
            *v00 = 0.0f;
            *v01 = w;
            *v10 = 0.0f;
            *v11 = h;
            break;
        case PIVOT_BOTTOM_CENTER:
            *v00 = w * -0.5f;
            *v01 = w * 0.5f;
            *v10 = 0.0f;
            *v11 = h;
            break;
        case PIVOT_BOTTOM_RIGHT:
            *v00 = -w;
            *v01 = 0.0f;
            *v10 = 0.0f;
            *v11 = h;
            break;
        }
    }

    F2RenderScene::F2RenderScene() {
        groups.reserve(0x40);
    }

    F2RenderScene::~F2RenderScene() {
        for (F2RenderGroup*& i : groups)
            delete i;
    }

    void F2RenderScene::Append(F2RenderGroup* rend_group) {
        groups.push_back(rend_group);
    }

    void F2RenderScene::CalcDispLine(F2RenderGroup* rend_group) {
        if (!rend_group->elements || !rend_group->vbo || rend_group->ctrl < 1)
            return;

        size_t count = 0;
        RenderElement* elem = rend_group->elements;
        for (size_t i = rend_group->ctrl; i > 0; i--, elem++) {
            if (!elem->alive)
                continue;

            if (elem->locus_history) {
                size_t length = elem->locus_history->data.size();
                if (length > 1)
                    count += length;
            }
        }

        if (!count || count > rend_group->max_count)
            return;

        bool has_scale = false;
        vec3 scale = vec3_null;
        if (rend_group->flags & PARTICLE_EMITTER_LOCAL) {
            mat4_get_scale(&rend_group->mat, &scale);
            vec3_sub(scale, vec3_identity, scale);
            if (!(has_scale |= fabsf(scale.x) > 0.000001f ? true : false))
                scale.x = 0.0f;
            if (!(has_scale |= fabsf(scale.y) > 0.000001f ? true : false))
                scale.y = 0.0f;
            if (!(has_scale |= fabsf(scale.z) > 0.000001f ? true : false))
                scale.z = 0.0f;
            mat4_normalize_rotation(&rend_group->mat, &rend_group->mat_draw);
        }
        else
            rend_group->mat_draw = mat4_identity;

        gl_state_bind_array_buffer(rend_group->vbo);
        Buffer* buf = (Buffer*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        elem = rend_group->elements;
        size_t disp = 0;
        rend_group->draw_list.clear();
        for (size_t i = rend_group->ctrl, index = 0; i > 0; elem++) {
            if (!elem->alive)
                continue;

            i--;
            LocusHistory* hist = elem->locus_history;
            if (!elem->disp || !hist || hist->data.size() < 2)
                continue;

            size_t j = 0;
            if (has_scale)
                for (LocusHistory::Data& hist_data : hist->data) {
                    vec3 pos = hist_data.translation;
                    vec3 pos_diff;
                    vec3_sub(pos, elem->base_translation, pos_diff);
                    vec3_mult(pos_diff, scale, pos_diff);
                    vec3_add(pos, pos_diff, buf->position);
                    buf->uv = vec2_null;
                    buf->color = hist_data.color;
                    j++;
                    buf++;
                }
            else
                for (LocusHistory::Data& hist_data : hist->data) {
                    buf->position = hist_data.translation;
                    buf->uv = vec2_null;
                    buf->color = hist_data.color;
                    j++;
                    buf++;
                }

            if (j > 0) {
                disp += j;
                rend_group->draw_list.push_back({ (GLint)index, (GLsizei)j });
                index += j;
            }
        }
        rend_group->disp = disp;
        glUnmapBuffer(GL_ARRAY_BUFFER);
        gl_state_bind_array_buffer(0);
    }

    void F2RenderScene::Ctrl(GLT, float_t delta_frame) {
        ctrl_quad = 0;
        ctrl_line = 0;
        ctrl_locus = 0;
        ctrl_mesh = 0;

        for (F2RenderGroup*& i : groups)
            if (i) {
                switch (i->type) {
                case PARTICLE_QUAD:
                    ctrl_quad += i->ctrl;
                    break;
                case PARTICLE_LINE:
                    ctrl_line += i->ctrl;
                    break;
                case PARTICLE_LOCUS:
                    ctrl_locus += i->ctrl;
                    break;
                }

                i->Ctrl(GLT_VAL, delta_frame, true);
            }
    }

    void F2RenderScene::CalcDispLocus(GPM, F2RenderGroup* rend_group) {
        if (!rend_group->elements || !rend_group->vbo || rend_group->ctrl < 1)
            return;

        size_t count = 0;
        RenderElement* elem = rend_group->elements;
        for (size_t i = rend_group->ctrl; i > 0; i--, elem++) {
            if (!elem->alive)
                continue;

            if (elem->locus_history) {
                LocusHistory* hist = elem->locus_history;
                size_t length = elem->locus_history->data.size();
                if (length > 1)
                    count += 2 * length;
            }
        }

        if (!count || count > rend_group->max_count)
            return;

        vec3 x_vec = { 1.0f, 0.0f, 0.0f };
        if (rend_group->flags & PARTICLE_LOCAL) {
            mat4 mat;
            mat4_mult(&((render_context*)GPM_VAL->rctx)->camera->inv_view, &rend_group->mat, &mat);
            mat4_mult(&((render_context*)GPM_VAL->rctx)->camera->view, &mat, &mat);
            mat4_mult(&mat, &((render_context*)GPM_VAL->rctx)->camera->inv_view, &rend_group->mat_draw);
        }
        else
            rend_group->mat_draw = mat4_identity;

        bool has_scale = false;
        vec3 scale = vec3_null;
        mat3 model_mat;
        if (rend_group->flags & PARTICLE_EMITTER_LOCAL) {
            mat4_get_scale(&rend_group->mat, &scale);
            if (rend_group->flags & PARTICLE_SCALE)
                x_vec.x = scale.x;
            vec3_sub(scale, vec3_identity, scale);
            if (!(has_scale |= fabsf(scale.x) > 0.000001f ? true : false))
                scale.x = 0.0f;
            if (!(has_scale |= fabsf(scale.y) > 0.000001f ? true : false))
                scale.y = 0.0f;
            if (!(has_scale |= fabsf(scale.z) > 0.000001f ? true : false))
                scale.z = 0.0f;
            mat4_mult_vec3(&rend_group->mat_rot, &scale, &scale);

            mat4 mat;
            mat4_normalize_rotation(&rend_group->mat, &mat);
            mat3_from_mat4(&mat, &model_mat);
        }
        else
            model_mat = mat3_identity;

        mat3_mult_vec(&((render_context*)GPM_VAL->rctx)->camera->inv_view_mat3, &x_vec, &x_vec);

        gl_state_bind_array_buffer(rend_group->vbo);
        Buffer* buf = (Buffer*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        elem = rend_group->elements;
        size_t disp = 0;
        rend_group->draw_list.clear();
        for (size_t i = rend_group->ctrl, index = 0; i > 0; elem++) {
            if (!elem->alive)
                continue;

            i--;
            LocusHistory* hist = elem->locus_history;
            if (!elem->disp || !hist || hist->data.size() < 2)
                continue;

            float_t uv_u = elem->uv.x + elem->uv_scroll.x;
            float_t uv_u_2nd = elem->uv.x + elem->uv_scroll.x + rend_group->split_uv.x;
            float_t uv_v_2nd = elem->uv.y + elem->uv_scroll.y + rend_group->split_uv.y;
            float_t uv_v_scale = rend_group->split_uv.y / (float_t)(hist->data.size() - 1);

            uv_v_2nd = 1.0f - uv_v_2nd;

            size_t len = elem->locus_history->data.size();
            size_t j = 0;
            if (has_scale)
                for (LocusHistory::Data& hist_data : hist->data) {
                    vec3 pos = hist_data.translation;
                    vec3 pos_diff;
                    vec3_sub(pos, elem->base_translation, pos_diff);
                    vec3_mult(pos_diff, scale, pos_diff);
                    mat3_mult_vec(&model_mat, &pos_diff, &pos_diff);
                    vec3_add(pos, pos_diff, pos);

                    float_t v00;
                    float_t v01;
                    CalcDispLocusSetPivot(rend_group->pivot,
                        hist_data.scale * elem->scale.x * elem->scale_all,
                        &v00, &v01);

                    vec3_mult_scalar(x_vec, v00, buf[0].position);
                    vec3_add(buf[0].position, pos, buf[0].position);
                    buf[0].uv.x = uv_u;
                    buf[0].uv.y = uv_v_2nd + (float_t)j * uv_v_scale;
                    buf[0].color = hist_data.color;

                    vec3_mult_scalar(x_vec, v01, buf[1].position);
                    vec3_add(buf[1].position, pos, buf[1].position);
                    buf[1].uv.x = uv_u_2nd;
                    buf[1].uv.y = uv_v_2nd + (float_t)j * uv_v_scale;
                    buf[1].color = hist_data.color;
                    j++;
                    buf += 2;
                }
            else
                for (LocusHistory::Data& hist_data : hist->data) {
                    vec3 pos = hist_data.translation;

                    float_t v00;
                    float_t v01;
                    CalcDispLocusSetPivot(rend_group->pivot,
                        hist_data.scale * elem->scale.x * elem->scale_all,
                        &v00, &v01);

                    vec3_mult_scalar(x_vec, v00, buf[0].position);
                    vec3_add(buf[0].position, pos, buf[0].position);
                    buf[0].uv.x = uv_u;
                    buf[0].uv.y = uv_v_2nd + (float_t)j * uv_v_scale;
                    buf[0].color = hist_data.color;

                    vec3_mult_scalar(x_vec, v01, buf[1].position);
                    vec3_add(buf[1].position, pos, buf[1].position);
                    buf[1].uv.x = uv_u_2nd;
                    buf[1].uv.y = uv_v_2nd + (float_t)j * uv_v_scale;
                    buf[1].color = hist_data.color;
                    j++;
                    buf += 2;
                }

            if (j > 0) {
                disp += j;
                rend_group->draw_list.push_back({ (GLint)index, (GLsizei)(j * 2) });
                index += j * 2;
            }
        }
        rend_group->disp = disp;
        glUnmapBuffer(GL_ARRAY_BUFFER);
        gl_state_bind_array_buffer(0);
    }

    void F2RenderScene::CalcDispQuad(GPM, F2RenderGroup* rend_group) {
        if (!rend_group->elements || !rend_group->vbo || rend_group->ctrl < 1)
            return;

        mat4 model_mat;
        mat4 view_mat;
        mat4 inv_view_mat;
        if (rend_group->flags & PARTICLE_EMITTER_LOCAL) {
            model_mat = rend_group->mat;
            mat4_normalize_rotation(&model_mat, &view_mat);
            mat4_mult(&view_mat, &((render_context*)GPM_VAL->rctx)->camera->view, &view_mat);
            mat4_inverse(&view_mat, &inv_view_mat);
        }
        else {
            model_mat = mat4_identity;
            view_mat = ((render_context*)GPM_VAL->rctx)->camera->view;
            inv_view_mat = ((render_context*)GPM_VAL->rctx)->camera->inv_view;
        }

        if (rend_group->flags & PARTICLE_LOCAL) {
            mat4_mult(&inv_view_mat, &rend_group->mat, &inv_view_mat);
            mat4_mult(&view_mat, &inv_view_mat, &view_mat);
            mat4_inverse(&view_mat, &inv_view_mat);
        }

        mat4_mult(&view_mat, &((render_context*)GPM_VAL->rctx)->camera->inv_view, &rend_group->mat_draw);

        mat4 dir_mat;
        switch (rend_group->draw_type) {
        case DIRECTION_BILLBOARD:
            mat4_clear_trans(&model_mat, &dir_mat);
            mat4_mult(&dir_mat, &inv_view_mat, &dir_mat);
            mat4_clear_trans(&dir_mat, &dir_mat);
            break;
        case DIRECTION_EMITTER_DIRECTION:
            mat4_clear_trans(&rend_group->mat_rot, &dir_mat);
            break;
        case DIRECTION_PREV_POSITION:
        case DIRECTION_EMIT_POSITION:
        case DIRECTION_PREV_POSITION_DUP:
            if (rend_group->flags & PARTICLE_EMITTER_LOCAL)
                mat4_clear_trans(&rend_group->mat_rot, &dir_mat);
            else
                dir_mat = mat4_identity;
            break;
        case DIRECTION_Y_AXIS:
            mat4_rotate_y((float_t)-M_PI_2, &dir_mat);
            break;
        case DIRECTION_X_AXIS:
            mat4_rotate_x((float_t)-M_PI_2, &dir_mat);
            break;
        case DIRECTION_Z_AXIS:
            mat4_rotate_z((float_t)-M_PI_2, &dir_mat);
            break;
        case DIRECTION_BILLBOARD_Y_AXIS:
            mat4_rotate_y(((render_context*)GPM_VAL->rctx)->camera->rotation.y, &dir_mat);
            break;
        default:
            dir_mat = mat4_identity;
            break;
        }

        switch (rend_group->draw_type) {
        case DIRECTION_PREV_POSITION:
        case DIRECTION_EMIT_POSITION:
        case DIRECTION_PREV_POSITION_DUP:
            CalcDispQuadDirectionRotation(rend_group, &model_mat, &dir_mat);
            break;
        default:
            CalcDispQuadNormal(GPM_VAL, rend_group, &model_mat, &dir_mat);
            break;
        }
    }

    void F2RenderScene::CalcDispQuadDirectionRotation(
        F2RenderGroup* rend_group, mat4* model_mat, mat4* dir_mat) {
        mat4 inv_model_mat;
        mat4_inverse(model_mat, &inv_model_mat);
        mat4_clear_trans(&inv_model_mat, &inv_model_mat);

        vec3 x_vec_base = { 1.0f, 0.0f, 0.0f };
        vec3 y_vec_base = { 0.0f, 1.0f, 0.0f };

        mat4_mult_vec3(&inv_model_mat, &x_vec_base, &x_vec_base);
        mat4_mult_vec3(&inv_model_mat, &y_vec_base, &y_vec_base);

        vec3 up_vec;
        mat4(*rotate_func)(F2RenderGroup*, RenderElement*, vec3*);
        if (rend_group->draw_type == DIRECTION_EMIT_POSITION) {
            up_vec = { 0.0f, 0.0f, 1.0f };
            rotate_func = F2RenderGroup::RotateToEmitPosition;
        }
        else {
            up_vec = { 0.0f, 1.0f, 0.0f };
            rotate_func = F2RenderGroup::RotateToPrevPosition;
        }

        vec3 scale;
        mat4_get_scale(model_mat, &scale);

        gl_state_bind_array_buffer(rend_group->vbo);
        Buffer* buf = (Buffer*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        bool use_scale = rend_group->flags & PARTICLE_SCALE ? true : false;
        RenderElement* elem = rend_group->elements;
        size_t disp = 0;
        for (size_t i = rend_group->ctrl, j_max = 1024; i > 0; i -= j_max) {
            j_max = min(i, j_max);
            for (size_t j = j_max; j > 0; elem++) {
                if (!elem->alive)
                    continue;
                j--;

                if (!elem->disp)
                    continue;

                vec2 scale_particle;
                vec2_mult(*(vec2*)&elem->scale, elem->scale_particle, scale_particle);
                vec2_mult_scalar(scale_particle, elem->scale_all, scale_particle);

                if (fabsf(scale_particle.x) < 0.000001f || fabsf(scale_particle.y) < 0.000001f)
                    continue;

                vec3 pos = elem->translation;

                float_t v00;
                float_t v01;
                float_t v10;
                float_t v11;
                CalcDispQuadSetPivot(rend_group->pivot,
                    scale_particle.x, scale_particle.y,
                    &v00, &v01, &v10, &v11);

                vec2 pos_add[4];
                vec2 uv_add[4];
                pos_add[0].x = v00;
                pos_add[0].y = v11;
                uv_add[0].x = 0.0f;
                uv_add[0].y = 0.0f;
                pos_add[1].x = v00;
                pos_add[1].y = v10;
                uv_add[1].x = 0.0f;
                uv_add[1].y = rend_group->split_uv.y;
                pos_add[2].x = v01;
                pos_add[2].y = v10;
                uv_add[2].x = rend_group->split_uv.x;
                uv_add[2].y = rend_group->split_uv.y;
                pos_add[3].x = v01;
                pos_add[3].y = v11;
                uv_add[3].x = rend_group->split_uv.x;
                uv_add[3].y = 0.0f;

                vec2 base_uv;
                vec2_add(elem->uv_scroll, elem->uv, base_uv);

                mat4 mat = rotate_func(rend_group, elem, &up_vec);
                mat4_clear_trans(&mat, &mat);
                mat4_mult(dir_mat, &mat, &mat);
                mat4_clear_trans(&mat, &mat);

                vec3 x_vec;
                mat4_mult_vec3(&mat, &x_vec_base, &x_vec);
                vec3 y_vec;
                mat4_mult_vec3(&mat, &y_vec_base, &y_vec);

                if (use_scale) {
                    vec3_mult(x_vec, scale, x_vec);
                    vec3_mult(y_vec, scale, y_vec);
                }

                float_t rot_z_cos = cosf(elem->rotation.z);
                float_t rot_z_sin = sinf(elem->rotation.z);
                for (int32_t k = 0; k < 4; k++, buf++) {
                    vec3 x_vec_rot;
                    vec3_mult_scalar(x_vec, pos_add[k].x * rot_z_cos - pos_add[k].y * rot_z_sin, x_vec_rot);
                    vec3 y_vec_rot;
                    vec3_mult_scalar(y_vec, pos_add[k].x * rot_z_sin + pos_add[k].y * rot_z_cos, y_vec_rot);

                    vec3 pos_add_rot;
                    vec3_add(x_vec_rot, y_vec_rot, pos_add_rot);
                    vec3_add(pos, pos_add_rot, buf->position);

                    vec2_add(base_uv, uv_add[k], buf->uv);
                    buf->color = elem->color;
                }
                disp++;
            }
        }
        rend_group->disp = disp;
        glUnmapBuffer(GL_ARRAY_BUFFER);
        gl_state_bind_array_buffer(0);
    }

    void F2RenderScene::CalcDispQuadNormal(GPM,
        F2RenderGroup* rend_group, mat4* model_mat, mat4* dir_mat) {
        mat4 inv_model_mat;
        mat4_inverse(model_mat, &inv_model_mat);
        mat4_clear_trans(&inv_model_mat, &inv_model_mat);

        vec3 x_vec = { 1.0f, 0.0f, 0.0f };
        vec3 y_vec = { 0.0f, 1.0f, 0.0f };
        bool use_z_offset = false;
        vec3 dist_to_cam = vec3_null;
        mat4 z_offset_inv_mat = mat4_identity;
        if (fabsf(rend_group->z_offset) > 0.000001f) {
            use_z_offset = true;
            mat4_get_translation(model_mat, &dist_to_cam);
            vec3_sub(((render_context*)GPM_VAL->rctx)->camera->view_point, dist_to_cam, dist_to_cam);
            if (rend_group->flags & PARTICLE_EMITTER_LOCAL) {
                mat4_normalize_rotation(model_mat, &z_offset_inv_mat);
                mat4_inverse(&z_offset_inv_mat, &z_offset_inv_mat);
            }
        }

        bool has_scale = false;
        bool emitter_local = false;
        vec3 scale = vec3_null;
        if (rend_group->flags & PARTICLE_EMITTER_LOCAL) {
            mat4_get_scale(model_mat, &scale);
            if (rend_group->flags & PARTICLE_SCALE) {
                x_vec.x = scale.x;
                y_vec.y = scale.y;
            }
            vec3_sub(scale, vec3_identity, scale);
            if (!(has_scale |= fabsf(scale.x) > 0.000001f ? true : false))
                scale.x = 0.0f;
            if (!(has_scale |= fabsf(scale.y) > 0.000001f ? true : false))
                scale.y = 0.0f;
            if (!(has_scale |= fabsf(scale.z) > 0.000001f ? true : false))
                scale.z = 0.0f;
            mat4_mult_vec3(&rend_group->mat_rot, &scale, &scale);
            emitter_local = true;
        }

        vec3 ext_anim_scale;
        float_t some_scale = 0.0f;
        if (rend_group->GetExtAnimScale(&ext_anim_scale, &some_scale)) {
            if (some_scale <= 0.0f)
                some_scale = 1.0f;

            vec2_add_scalar(*(vec2*)&ext_anim_scale, some_scale, *(vec2*)&ext_anim_scale);
            x_vec.x *= ext_anim_scale.x;
            y_vec.y *= ext_anim_scale.y;
        }

        if (rend_group->draw_type != DIRECTION_BILLBOARD) {
            mat4_mult_vec3(dir_mat, &x_vec, &x_vec);
            mat4_mult_vec3(dir_mat, &y_vec, &y_vec);
        }
        else
            mat4_mult(&inv_model_mat, dir_mat, &inv_model_mat);

        mat4_mult_vec3(&inv_model_mat, &x_vec, &x_vec);
        mat4_mult_vec3(&inv_model_mat, &y_vec, &y_vec);

        gl_state_bind_array_buffer(rend_group->vbo);
        Buffer* buf = (Buffer*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        RenderElement* elem = rend_group->elements;
        size_t disp = 0;
        if (rend_group->draw_type == DIRECTION_PARTICLE_ROTATION)
            for (size_t i = rend_group->ctrl, j_max = 1024; i > 0; i -= j_max) {
                j_max = min(i, j_max);
                for (size_t j = j_max; j > 0; elem++) {
                    if (!elem->alive)
                        continue;
                    j--;

                    if (!elem->disp)
                        continue;

                    vec2 scale_particle;
                    vec2_mult(*(vec2*)&elem->scale, elem->scale_particle, scale_particle);
                    vec2_mult_scalar(scale_particle, elem->scale_all, scale_particle);

                    if (fabsf(scale_particle.x) < 0.000001f || fabsf(scale_particle.y) < 0.000001f)
                        continue;

                    vec3 pos = elem->translation;
                    if (has_scale) {
                        vec3 pos_diff;
                        vec3_sub(pos, elem->base_translation, pos_diff);
                        vec3_mult(pos_diff, scale, pos_diff);
                        mat4_mult_vec3(&inv_model_mat, &pos_diff, &pos_diff);
                        vec3_add(pos, pos_diff, pos);
                    }

                    float_t v00;
                    float_t v01;
                    float_t v11;
                    float_t v10;
                    CalcDispQuadSetPivot(rend_group->pivot,
                        scale_particle.x, scale_particle.y,
                        &v00, &v01, &v10, &v11);

                    vec2 pos_add[4];
                    vec2 uv_add[4];
                    pos_add[0].x = v00;
                    pos_add[0].y = v11;
                    uv_add[0].x = 0.0f;
                    uv_add[0].y = 0.0f;
                    pos_add[1].x = v00;
                    pos_add[1].y = v10;
                    uv_add[1].x = 0.0f;
                    uv_add[1].y = rend_group->split_uv.y;
                    pos_add[2].x = v01;
                    pos_add[2].y = v10;
                    uv_add[2].x = rend_group->split_uv.x;
                    uv_add[2].y = rend_group->split_uv.y;
                    pos_add[3].x = v01;
                    pos_add[3].y = v11;
                    uv_add[3].x = rend_group->split_uv.x;
                    uv_add[3].y = 0.0f;

                    vec2 base_uv;
                    vec2_add(elem->uv_scroll, elem->uv, base_uv);
                    if (use_z_offset) {
                        vec3 z_offset_dir;
                        vec3_sub(dist_to_cam, pos, z_offset_dir);
                        vec3_normalize(z_offset_dir, z_offset_dir);

                        if (emitter_local)
                            mat4_mult_vec3(&z_offset_inv_mat, &z_offset_dir, &z_offset_dir);

                        vec3_mult_scalar(z_offset_dir, rend_group->z_offset, z_offset_dir);
                        vec3_add(pos, z_offset_dir, pos);
                    }

                    mat3 ptc_rot;
                    mat3_rotate(elem->rotation.x, elem->rotation.y, elem->rotation.z, &ptc_rot);
                    for (int32_t k = 0; k < 4; k++, buf++) {
                        vec3 x_vec_rot;
                        x_vec_rot.x = x_vec.x * pos_add[k].x;
                        x_vec_rot.y = y_vec.y * pos_add[k].y;
                        x_vec_rot.z = 0.0f;
                        mat3_mult_vec(&ptc_rot, &x_vec_rot, &x_vec_rot);
                        vec3_add(x_vec_rot, pos, buf->position);
                        vec2_add(base_uv, uv_add[k], buf->uv);
                        buf->color = elem->color;
                    }
                    disp++;
                }
            }
        else
            for (size_t i = rend_group->ctrl, j_max = 1024; i > 0; i -= j_max) {
                j_max = min(i, j_max);
                for (size_t j = j_max; j > 0; elem++) {
                    if (!elem->alive)
                        continue;
                    j--;

                    if (!elem->disp)
                        continue;

                    vec2 scale_particle;
                    vec2_mult(*(vec2*)&elem->scale, elem->scale_particle, scale_particle);
                    vec2_mult_scalar(scale_particle, elem->scale_all, scale_particle);

                    if (fabsf(scale_particle.x) < 0.000001f || fabsf(scale_particle.y) < 0.000001f)
                        continue;

                    vec3 pos = elem->translation;
                    if (has_scale) {
                        vec3 pos_diff;
                        vec3_sub(pos, elem->base_translation, pos_diff);
                        vec3_mult(pos_diff, scale, pos_diff);
                        mat4_mult_vec3(&inv_model_mat, &pos_diff, &pos_diff);
                        vec3_add(pos, pos_diff, pos);
                    }

                    float_t v00;
                    float_t v01;
                    float_t v11;
                    float_t v10;
                    CalcDispQuadSetPivot(rend_group->pivot,
                        scale_particle.x, scale_particle.y,
                        &v00, &v01, &v10, &v11);

                    vec2 pos_add[4];
                    vec2 uv_add[4];
                    pos_add[0].x = v00;
                    pos_add[0].y = v11;
                    uv_add[0].x = 0.0f;
                    uv_add[0].y = 0.0f;
                    pos_add[1].x = v00;
                    pos_add[1].y = v10;
                    uv_add[1].x = 0.0f;
                    uv_add[1].y = rend_group->split_uv.y;
                    pos_add[2].x = v01;
                    pos_add[2].y = v10;
                    uv_add[2].x = rend_group->split_uv.x;
                    uv_add[2].y = rend_group->split_uv.y;
                    pos_add[3].x = v01;
                    pos_add[3].y = v11;
                    uv_add[3].x = rend_group->split_uv.x;
                    uv_add[3].y = 0.0f;

                    vec2 base_uv;
                    vec2_add(elem->uv_scroll, elem->uv, base_uv);
                    if (use_z_offset) {
                        vec3 z_offset_dir;
                        vec3_sub(dist_to_cam, pos, z_offset_dir);
                        vec3_normalize(z_offset_dir, z_offset_dir);

                        if (emitter_local)
                            mat4_mult_vec3(&z_offset_inv_mat, &z_offset_dir, &z_offset_dir);

                        vec3_mult_scalar(z_offset_dir, rend_group->z_offset, z_offset_dir);
                        vec3_add(pos, z_offset_dir, pos);
                    }

                    float_t rot_z_cos = cosf(elem->rotation.z);
                    float_t rot_z_sin = sinf(elem->rotation.z);
                    for (int32_t k = 0; k < 4; k++, buf++) {
                        vec3 x_vec_rot;
                        vec3_mult_scalar(x_vec, rot_z_cos * pos_add[k].x - rot_z_sin * pos_add[k].y, x_vec_rot);
                        vec3 y_vec_rot;
                        vec3_mult_scalar(y_vec, rot_z_sin * pos_add[k].x + rot_z_cos * pos_add[k].y, y_vec_rot);
                        vec3_add(x_vec_rot, y_vec_rot, x_vec_rot);
                        vec3_add(x_vec_rot, pos, buf->position);
                        vec2_add(base_uv, uv_add[k], buf->uv);
                        buf->color = elem->color;
                    }
                    disp++;
                }
            }
        rend_group->disp = disp;
        glUnmapBuffer(GL_ARRAY_BUFFER);
        gl_state_bind_array_buffer(0);
    }

    void F2RenderScene::Disp(GPM, draw_pass_3d_type alpha) {
        disp_quad = 0;
        disp_line = 0;
        disp_locus = 0;
        disp_mesh = 0;

        F2EffectInst* eff = dynamic_cast<F2EffectInst*>(GPM_VAL->effect);
        F2EmitterInst* emit = dynamic_cast<F2EmitterInst*>(GPM_VAL->emitter);
        F2ParticleInst* ptcl = dynamic_cast<F2ParticleInst*>(GPM_VAL->particle);
        for (F2RenderGroup*& i : groups) {
            if (!i)
                continue;

            F2RenderGroup* rend_group = i;
            if ((rend_group)->alpha != alpha
                || (rend_group->CannotDisp() && !GPM_VAL->draw_all))
                continue;

#if !defined(CRE_DEV)
            Disp(GPM_VAL, rend_group);
#else
            if (!GPM_VAL->draw_selected || !eff) {
                Disp(GPM_VAL, rend_group);
            }
            else if ((eff && ptcl) || (eff && !emit)) {
                if (!ptcl || rend_group->particle == ptcl)
                    Disp(GPM_VAL, rend_group);
            }
            else if (emit)
                for (F2ParticleInst*& i : emit->particles) {
                    if (!i)
                        continue;

                    F2ParticleInst* particle = i;
                    if (rend_group->particle == particle)
                        Disp(GPM_VAL, rend_group);

                    for (F2ParticleInst*& j : particle->data.children)
                        if (j && rend_group->particle == j)
                            Disp(GPM_VAL, rend_group);
                }
#endif
        }
    }

    void F2RenderScene::Disp(GPM, F2RenderGroup* rend_group) {
        switch (rend_group->type) {
        case PARTICLE_QUAD:
        case PARTICLE_LINE:
        case PARTICLE_LOCUS:
            break;
        default:
            return;
        }

        rend_group->disp = 0;
        switch (rend_group->type) {
        case PARTICLE_QUAD:
            CalcDispQuad(GPM_VAL, rend_group);
            disp_quad += rend_group->disp;
            break;
        case PARTICLE_LINE:
            CalcDispLine(rend_group);
            disp_line += rend_group->disp;
            break;
        case PARTICLE_LOCUS:
            CalcDispLocus(GPM_VAL, rend_group);
            disp_locus += rend_group->disp;
            break;
        }

        if (rend_group->disp < 1)
            return;

        gl_state_enable_blend();
        switch (rend_group->blend_mode) {
        case PARTICLE_BLEND_ADD:
            gl_state_set_blend_func(GL_SRC_ALPHA, GL_ONE);
            break;
        case PARTICLE_BLEND_MULTIPLY:
            gl_state_set_blend_func(GL_ZERO, GL_SRC_COLOR);
            break;
        default:
            gl_state_set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        }
        gl_state_set_blend_equation(GL_FUNC_ADD);

        if (rend_group->type != PARTICLE_LINE && rend_group->texture) {
            gl_state_active_bind_texture_2d(0, rend_group->texture);
            if (rend_group->mask_texture) {
                gl_state_active_bind_texture_2d(1, rend_group->mask_texture);

                uniform_value[U_TEXTURE_COUNT] = 2;
                switch (rend_group->mask_blend_mode) {
                default:
                    uniform_value[U_TEXTURE_BLEND] = 0;
                    break;
                case PARTICLE_BLEND_MULTIPLY:
                    uniform_value[U_TEXTURE_BLEND] = 1;
                    break;
                case PARTICLE_BLEND_ADD:
                    uniform_value[U_TEXTURE_BLEND] = 2;
                    break;
                }
            }
            else {
                gl_state_active_bind_texture_2d(1, 0);
                uniform_value[U_TEXTURE_COUNT] = 1;
                uniform_value[U_TEXTURE_BLEND] = 0;
            }
        }
        else {
            gl_state_active_bind_texture_2d(0, 0);
            gl_state_active_bind_texture_2d(1, 0);
            uniform_value[U_TEXTURE_COUNT] = 0;
            uniform_value[U_TEXTURE_BLEND] = 0;
        }

        switch (rend_group->type) {
        case PARTICLE_QUAD:
            switch (rend_group->fog) {
            default:
                uniform_value[U_FOG_HEIGHT] = 0;
                break;
            case FOG_DEPTH:
                uniform_value[U_FOG_HEIGHT] = 1;
                break;
            case FOG_HEIGHT:
                uniform_value[U_FOG_HEIGHT] = 2;
                break;
            }

            if (rend_group->blend_mode == PARTICLE_BLEND_PUNCH_THROUGH) {
                uniform_value[U_ALPHA_BLEND] = 1;
                gl_state_enable_depth_test();
                gl_state_set_depth_func(GL_LESS);
                gl_state_set_depth_mask(GL_TRUE);
            }
            else {
                uniform_value[U_ALPHA_BLEND] = rend_group->alpha != DRAW_PASS_3D_OPAQUE ? 2 : 0;
                gl_state_enable_depth_test();
                gl_state_set_depth_func(GL_LESS);
                gl_state_set_depth_mask(GL_FALSE);
            }

            if (rend_group->draw_type == DIRECTION_BILLBOARD) {
                gl_state_enable_cull_face();
                gl_state_set_cull_face_mode(GL_BACK);
            }
            else
                gl_state_disable_cull_face();
            break;
        case PARTICLE_LINE:
            uniform_value[U_FOG_HEIGHT] = 0;
            uniform_value[U_ALPHA_BLEND] = 2;

            gl_state_enable_depth_test();
            gl_state_set_depth_func(GL_LESS);
            gl_state_set_depth_mask(GL_FALSE);
            gl_state_enable_cull_face();
            gl_state_set_cull_face_mode(GL_BACK);
            break;
        case PARTICLE_LOCUS:
            uniform_value[U_FOG_HEIGHT] = 0;
            uniform_value[U_ALPHA_BLEND] = 2;

            gl_state_enable_depth_test();
            gl_state_set_depth_func(GL_LESS);
            gl_state_set_depth_mask(GL_FALSE);
            gl_state_disable_cull_face();
            break;
        }

        float_t emission = 1.0f;
        if (rend_group->flags & PARTICLE_EMISSION
            || rend_group->blend_mode == PARTICLE_BLEND_TYPICAL)
            emission = rend_group->emission;

        shaders_ft.state_material_set_emission(false, emission, emission, emission, 1.0f);
        shaders_ft.state_matrix_set_mvp(rend_group->mat_draw,
            ((render_context*)GPM_VAL->rctx)->camera->view,
            ((render_context*)GPM_VAL->rctx)->camera->projection);
        shaders_ft.state_matrix_set_texture(0, mat4_identity);
        shaders_ft.state_matrix_set_texture(1, mat4_identity);
        shaders_ft.env_vert_set(3, vec4_identity);

        shaders_ft.set(SHADER_FT_GLITTER_PT);
        switch (rend_group->type) {
        case PARTICLE_QUAD: {
            static const GLsizei buffer_size = sizeof(Buffer);

            gl_state_bind_array_buffer(rend_group->vbo);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, buffer_size,
                (void*)offsetof(Buffer, position)); // Pos
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, buffer_size,
                (void*)offsetof(Buffer, color));    // Color
            glEnableVertexAttribArray(8);
            glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, buffer_size,
                (void*)offsetof(Buffer, uv));       // TexCoord0
            glEnableVertexAttribArray(9);
            glVertexAttribPointer(9, 2, GL_FLOAT, GL_FALSE, buffer_size,
                (void*)offsetof(Buffer, uv));       // TexCoord1
            gl_state_bind_array_buffer(0);

            gl_state_bind_element_array_buffer(rend_group->ebo);
            shaders_ft.draw_elements(GL_TRIANGLES, (GLsizei)(6 * rend_group->disp), GL_UNSIGNED_INT, 0);
            gl_state_bind_element_array_buffer(0);

            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(3);
            glDisableVertexAttribArray(8);
            glDisableVertexAttribArray(9);

        } break;
        case PARTICLE_LINE:
        case PARTICLE_LOCUS: {
            static const GLsizei buffer_size = sizeof(Buffer);

            gl_state_bind_array_buffer(rend_group->vbo);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, buffer_size,
                (void*)offsetof(Buffer, position)); // Pos
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, buffer_size,
                (void*)offsetof(Buffer, color));    // Color
            glEnableVertexAttribArray(8);
            glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, buffer_size,
                (void*)offsetof(Buffer, uv));       // TexCoord0
            glEnableVertexAttribArray(9);
            glVertexAttribPointer(9, 2, GL_FLOAT, GL_FALSE, buffer_size,
                (void*)offsetof(Buffer, uv));       // TexCoord1
            gl_state_bind_array_buffer(0);

            const GLenum mode = rend_group->type == PARTICLE_LINE ? GL_LINE_STRIP : GL_TRIANGLE_STRIP;
            for (std::pair<GLint, GLsizei>& i : rend_group->draw_list)
                shaders_ft.draw_arrays(mode, i.first, i.second);

            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(3);
            glDisableVertexAttribArray(8);
            glDisableVertexAttribArray(9);

        } break;
        }
        gl_state_disable_blend();
        gl_state_enable_cull_face();
        gl_state_disable_depth_test();
    }

    XRenderScene::XRenderScene() {

    }

    XRenderScene::~XRenderScene() {
        for (XRenderGroup*& i : groups)
            delete i;
    }

    void XRenderScene::Append(XRenderGroup* rend_group) {
        groups.push_back(rend_group);
    }

    void XRenderScene::CalcDisp(GPM) {
        XEffectInst* eff = dynamic_cast<XEffectInst*>(GPM_VAL->effect);
        XEmitterInst* emit = dynamic_cast<XEmitterInst*>(GPM_VAL->emitter);
        XParticleInst* ptcl = dynamic_cast<XParticleInst*>(GPM_VAL->particle);
        for (XRenderGroup*& i : groups) {
            if (!i)
                continue;

            XRenderGroup* rend_group = i;
            if (rend_group->CannotDisp() && !GPM_VAL->draw_all)
                continue;

#if !defined(CRE_DEV)
            CalcDisp(GPM_VAL, rend_group);
#else
            if (!GPM_VAL->draw_selected || !eff) {
                CalcDisp(GPM_VAL, rend_group);
            }
            else if ((eff && ptcl) || (eff && !emit)) {
                if (!ptcl || rend_group->particle == ptcl)
                    CalcDisp(GPM_VAL, rend_group);
            }
            else if (emit)
                for (XParticleInst*& i : emit->particles) {
                    if (!i)
                        continue;

                    XParticleInst* particle = i;
                    if (rend_group->particle == particle)
                        CalcDisp(GPM_VAL, rend_group);

                    for (XParticleInst*& j : particle->data.children)
                        if (j && rend_group->particle == j)
                            CalcDisp(GPM_VAL, rend_group);
                }
#endif
        }
    }

    void XRenderScene::CalcDisp(GPM, XRenderGroup* rend_group) {
        disp_mesh = 0;

        switch (rend_group->type) {
        case PARTICLE_MESH:
            break;
        default:
            return;
        }

        rend_group->disp = 0;
        switch (rend_group->type) {
        case PARTICLE_MESH:
            CalcDispMesh(GPM_VAL, rend_group);
            disp_mesh += rend_group->disp;
            break;
        }
    }

    void XRenderScene::CalcDispLine(XRenderGroup* rend_group) {
        if (!rend_group->elements || !rend_group->vbo || rend_group->ctrl < 1)
            return;

        size_t count = 0;
        RenderElement* elem = rend_group->elements;
        for (size_t i = rend_group->ctrl; i > 0; i--, elem++) {
            if (!elem->alive)
                continue;

            if (elem->locus_history) {
                size_t length = elem->locus_history->data.size();
                if (length > 1)
                    count += length;
            }
        }

        if (!count || count > rend_group->max_count)
            return;

        bool has_scale = false;
        vec3 scale = vec3_null;
        if (rend_group->flags & PARTICLE_EMITTER_LOCAL) {
            rend_group->GetEmitterScale(&scale);
            if (!(has_scale |= fabsf(scale.x) > 0.000001f ? true : false))
                scale.x = 0.0f;
            if (!(has_scale |= fabsf(scale.y) > 0.000001f ? true : false))
                scale.y = 0.0f;
            if (!(has_scale |= fabsf(scale.z) > 0.000001f ? true : false))
                scale.z = 0.0f;
            mat4_normalize_rotation(&rend_group->mat, &rend_group->mat_draw);
        }
        else
            rend_group->mat_draw = mat4_identity;

        gl_state_bind_array_buffer(rend_group->vbo);
        Buffer* buf = (Buffer*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        elem = rend_group->elements;
        size_t disp = 0;
        rend_group->draw_list.clear();
        for (size_t i = rend_group->ctrl, index = 0; i > 0; elem++) {
            if (!elem->alive)
                continue;

            i--;
            LocusHistory* hist = elem->locus_history;
            if (!elem->disp || !hist || hist->data.size() < 2)
                continue;

            size_t j = 0;
            if (has_scale)
                for (LocusHistory::Data& hist_data : hist->data) {
                    vec3 pos = hist_data.translation;
                    vec3 pos_diff;
                    vec3_sub(pos, elem->base_translation, pos_diff);
                    vec3_mult(pos_diff, scale, pos_diff);
                    vec3_add(pos, pos_diff, buf->position);
                    buf->uv = vec2_null;
                    buf->color = hist_data.color;
                    j++;
                    buf++;
                }
            else
                for (LocusHistory::Data& hist_data : hist->data) {
                    buf->position = hist_data.translation;
                    buf->uv = vec2_null;
                    buf->color = hist_data.color;
                    j++;
                    buf++;
                }

            if (j > 0) {
                disp += j;
                rend_group->draw_list.push_back({ (GLint)index, (GLsizei)j });
                index += j;
            }
        }
        rend_group->disp = disp;
        glUnmapBuffer(GL_ARRAY_BUFFER);
        gl_state_bind_array_buffer(0);
    }

    void XRenderScene::CalcDispLocus(GPM, XRenderGroup* rend_group) {
        if (!rend_group->elements || !rend_group->vbo || rend_group->ctrl < 1)
            return;

        size_t count = 0;
        RenderElement* elem = rend_group->elements;
        for (size_t i = rend_group->ctrl; i > 0; i--, elem++) {
            if (!elem->alive)
                continue;

            if (elem->locus_history) {
                size_t length = elem->locus_history->data.size();
                if (length > 1)
                    count += 2 * length;
            }
        }

        if (!count || count > rend_group->max_count)
            return;

        vec3 x_vec = { 1.0f, 0.0f, 0.0f };
        if (rend_group->flags & PARTICLE_LOCAL) {
            mat4 mat;
            mat4_mult(&((render_context*)GPM_VAL->rctx)->camera->inv_view, &rend_group->mat, &mat);
            mat4_mult(&((render_context*)GPM_VAL->rctx)->camera->view, &mat, &mat);
            mat4_mult(&mat, &((render_context*)GPM_VAL->rctx)->camera->inv_view, &rend_group->mat_draw);
        }
        else
            rend_group->mat_draw = mat4_identity;

        mat3 model_mat;
        bool has_scale = false;
        vec3 scale = vec3_null;
        if (rend_group->flags & PARTICLE_EMITTER_LOCAL) {
            mat4_get_scale(&rend_group->mat, &scale);
            if (rend_group->flags & PARTICLE_SCALE)
                x_vec.x = scale.x;
            vec3_sub(scale, vec3_identity, scale);
            if (!(has_scale |= fabsf(scale.x) > 0.000001f ? true : false))
                scale.x = 0.0f;
            if (!(has_scale |= fabsf(scale.y) > 0.000001f ? true : false))
                scale.y = 0.0f;
            if (!(has_scale |= fabsf(scale.z) > 0.000001f ? true : false))
                scale.z = 0.0f;

            mat3_from_mat4(&rend_group->mat, &model_mat);
            mat3_normalize_rotation(&model_mat, &model_mat);
        }
        else
            model_mat = mat3_identity;

        mat3_mult_vec(&((render_context*)GPM_VAL->rctx)->camera->inv_view_mat3, &x_vec, &x_vec);

        gl_state_bind_array_buffer(rend_group->vbo);
        Buffer* buf = (Buffer*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        elem = rend_group->elements;
        size_t disp = 0;
        rend_group->draw_list.clear();
        for (size_t i = rend_group->ctrl, index = 0; i > 0; elem++) {
            if (!elem->alive)
                continue;

            i--;
            LocusHistory* hist = elem->locus_history;
            if (!elem->disp || !hist || hist->data.size() < 2)
                continue;

            float_t uv_u = elem->uv.x + elem->uv_scroll.x;
            float_t uv_u_2nd = elem->uv.x + elem->uv_scroll.x + rend_group->split_uv.x;
            float_t uv_v_2nd = elem->uv.y + elem->uv_scroll.y + rend_group->split_uv.y;
            float_t uv_v_scale = rend_group->split_uv.y / (float_t)(hist->data.size() - 1);

            uv_v_2nd = 1.0f - uv_v_2nd;

            size_t j = 0;
            if (has_scale)
                for (LocusHistory::Data& hist_data : hist->data) {
                    vec3 pos = hist_data.translation;
                    vec3 pos_diff;
                    vec3_sub(pos, elem->base_translation, pos_diff);
                    vec3_mult(pos_diff, scale, pos_diff);
                    mat3_mult_vec(&model_mat, &pos_diff, &pos_diff);
                    vec3_add(pos, pos_diff, pos);

                    float_t v00;
                    float_t v01;
                    CalcDispLocusSetPivot(rend_group->pivot,
                        hist_data.scale * elem->scale.x * elem->scale_all,
                        &v00, &v01);

                    vec3_mult_scalar(x_vec, v00, buf[0].position);
                    vec3_add(buf[0].position, pos, buf[0].position);
                    buf[0].uv.x = uv_u;
                    buf[0].uv.y = uv_v_2nd + (float_t)j * uv_v_scale;
                    buf[0].color = hist_data.color;

                    vec3_mult_scalar(x_vec, v01, buf[1].position);
                    vec3_add(buf[1].position, pos, buf[1].position);
                    buf[1].uv.x = uv_u_2nd;
                    buf[1].uv.y = uv_v_2nd + (float_t)j * uv_v_scale;
                    buf[1].color = hist_data.color;
                    j++;
                    buf += 2;
                }
            else
                for (LocusHistory::Data& hist_data : hist->data) {
                    vec3 pos = hist_data.translation;

                    float_t v00;
                    float_t v01;
                    CalcDispLocusSetPivot(rend_group->pivot,
                        hist_data.scale * elem->scale.x * elem->scale_all,
                        &v00, &v01);

                    vec3_mult_scalar(x_vec, v00, buf[0].position);
                    vec3_add(buf[0].position, pos, buf[0].position);
                    buf[0].uv.x = uv_u;
                    buf[0].uv.y = uv_v_2nd + (float_t)j * uv_v_scale;
                    buf[0].color = hist_data.color;

                    vec3_mult_scalar(x_vec, v01, buf[1].position);
                    vec3_add(buf[1].position, pos, buf[1].position);
                    buf[1].uv.x = uv_u_2nd;
                    buf[1].uv.y = uv_v_2nd + (float_t)j * uv_v_scale;
                    buf[1].color = hist_data.color;
                    j++;
                    buf += 2;
                }

            if (j > 0) {
                disp += j;
                rend_group->draw_list.push_back({ (GLint)index, (GLsizei)(j * 2) });
                index += j * 2;
            }
        }
        rend_group->disp = disp;
        glUnmapBuffer(GL_ARRAY_BUFFER);
        gl_state_bind_array_buffer(0);
    }

    void XRenderScene::CalcDispMesh(GPM, XRenderGroup* rend_group) {
        if (rend_group->object_name_hash == hash_murmurhash_empty
            || rend_group->object_name_hash == 0xFFFFFFFF)
            return;

        bool has_scale = false;
        bool emitter_local = false;
        vec3 emit_scale = vec3_null;
        mat4 model_mat;
        mat4 view_mat;
        mat4 inv_view_mat;
        if (rend_group->flags & PARTICLE_EMITTER_LOCAL) {
            model_mat = rend_group->mat;
            mat4_normalize_rotation(&model_mat, &view_mat);
            mat4_mult(&view_mat, &((render_context*)GPM_VAL->rctx)->camera->view, &view_mat);
            mat4_inverse(&view_mat, &inv_view_mat);

            emitter_local = true;
            if (rend_group->flags & PARTICLE_SCALE) {
                rend_group->GetEmitterScale(&emit_scale);
                if (fabsf(emit_scale.x) > 0.000001f
                    || fabsf(emit_scale.y) > 0.000001f || fabsf(emit_scale.z) > 0.000001f)
                    has_scale = true;
            }
        }
        else {
            model_mat = mat4_identity;
            view_mat = ((render_context*)GPM_VAL->rctx)->camera->view;
            inv_view_mat = ((render_context*)GPM_VAL->rctx)->camera->inv_view;
        }

        bool draw_local = false;
        if (rend_group->flags & PARTICLE_LOCAL) {
            mat4_mult(&inv_view_mat, &rend_group->mat, &inv_view_mat);
            mat4_mult(&view_mat, &inv_view_mat, &view_mat);
            mat4_inverse(&view_mat, &inv_view_mat);
            draw_local = true;
        }
        mat4_mult(&view_mat, &((render_context*)GPM_VAL->rctx)->camera->inv_view, &rend_group->mat_draw);

        mat4 dir_mat = mat4_identity;
        vec3 up_vec = { 0.0f, 0.0f, 1.0f };
        bool billboard = false;
        bool emitter_rotation = false;
        mat4(*rotate_func)(XRenderGroup*, RenderElement*, vec3*, vec3*) = 0;
        switch (rend_group->draw_type) {
        case DIRECTION_BILLBOARD:
            mat4_clear_trans(&model_mat, &dir_mat);
            mat4_mult(&dir_mat, &inv_view_mat, &dir_mat);
            mat4_clear_trans(&dir_mat, &dir_mat);
            billboard = true;
            break;
        case DIRECTION_EMITTER_DIRECTION:
            dir_mat = rend_group->mat_rot;
            break;
        case DIRECTION_PREV_POSITION:
        case DIRECTION_PREV_POSITION_DUP:
            rotate_func = XRenderGroup::RotateMeshToPrevPosition;
            break;
        case DIRECTION_EMIT_POSITION:
            rotate_func = XRenderGroup::RotateMeshToEmitPosition;
            break;
        case DIRECTION_Y_AXIS:
            mat4_rotate_y((float_t)M_PI_2, &dir_mat);
            break;
        case DIRECTION_X_AXIS:
            mat4_rotate_x((float_t)-M_PI_2, &dir_mat);
            break;
        case DIRECTION_BILLBOARD_Y_AXIS:
            mat4_rotate_y(((render_context*)GPM_VAL->rctx)->camera->rotation.y, &dir_mat);
            break;
        case DIRECTION_EMITTER_ROTATION:
            emitter_rotation = true;
            break;
        }

        vec3 ext_anim_scale;
        float_t some_scale = 0.0f;
        if (rend_group->GetExtAnimScale(&ext_anim_scale, &some_scale)) {
            if (!has_scale) {
                emit_scale = vec3_identity;
                has_scale = true;
            }

            if (some_scale >= 0.0f) {
                vec3_add_scalar(ext_anim_scale, some_scale, ext_anim_scale);
                vec3_mult(emit_scale, ext_anim_scale, emit_scale);
            }
            else
                vec3_add(emit_scale, ext_anim_scale, emit_scale);
        }

        render_context* rctx = (render_context*)GPM_VAL->rctx;
        object_data* object_data = &rctx->object_data;

        RenderElement* elem = rend_group->elements;
        size_t disp = 0;
        for (size_t i = rend_group->ctrl, j_max = 1024; i > 0; i -= j_max) {
            j_max = min(i, j_max);
            for (size_t j = j_max; j > 0; elem++) {
                if (!elem->alive)
                    continue;
                j--;

                if (!elem->disp)
                    continue;

                vec3 trans = elem->translation;
                vec3 rot = elem->rotation;
                vec3 scale;
                vec3_mult_scalar(elem->scale, elem->scale_all, scale);
                if (has_scale)
                    vec3_mult(scale, emit_scale, scale);

                if (emitter_local)
                    mat4_mult_vec3_trans(&model_mat, &trans, &trans);

                mat4 mat;
                if (billboard) {
                    if (draw_local)
                        mat = mat4_identity;
                    else
                        mat = dir_mat;
                }
                else if (rotate_func)
                    mat = rotate_func(rend_group, elem, &up_vec, &trans);
                else if (emitter_rotation)
                    mat = elem->mat;
                else
                    mat = dir_mat;

                mat4_rotate(rot.x, rot.y, rot.z, &mat);
                mat4_scale_rot(&mat, scale.x, scale.y, scale.z, &mat);
                mat4_set_translation(&mat, &trans);

                mat4 uv_mat[2];
                mat4_translate(elem->uv_scroll.x, -elem->uv_scroll.y, 0.0f, &uv_mat[0]);
                mat4_translate(elem->uv_scroll_2nd.x, -elem->uv_scroll_2nd.y, 0.0f, &uv_mat[1]);

                Particle* particle = rend_group->particle->data.particle;
                object_info object_info;
                object_info.set_id = (uint32_t)particle->data.mesh.object_set_name_hash;
                object_info.id = (uint32_t)particle->data.mesh.object_name_hash;
                obj* obj = object_storage_get_obj(object_info);
                if (!obj)
                    continue;

                object_data->texture_pattern_count = 0;
                int32_t ttc = 0;
                texture_transform_struct* tt = object_data->texture_transform_array;
                for (uint32_t i = 0; i < obj->num_mesh; i++) {
                    obj_mesh* mesh = &obj->mesh_array[i];
                    for (uint32_t j = 0; j < mesh->num_submesh; j++) {
                        obj_sub_mesh* sub_mesh = &mesh->submesh_array[j];
                        obj_material* material = &obj->material_array[sub_mesh->material_index].material;

                        int32_t l = 0;
                        for (obj_material_texture_data& k : material->texdata) {
                            obj_material_texture_data* texture = &k;
                            if (texture->shader_info.m.tex_type != OBJ_MATERIAL_TEXTURE_COLOR)
                                continue;

                            tt->id = texture->tex_index;
                            tt->mat = uv_mat[l];
                            ttc++;
                            tt++;

                            if (++l == 2 || ttc == 24)
                                break;
                        }

                        if (ttc == 24)
                            break;
                    }

                    if (ttc == 24)
                        break;
                }
                object_data->texture_transform_count = ttc;

                if (rend_group->flags & PARTICLE_LOCAL)
                    mat4_mult(&mat, &((render_context*)GPM_VAL->rctx)->camera->inv_view, &mat);

                vec4u color = elem->color;
                if (draw_task_add_draw_object_by_object_info_color(rctx, &mat,
                    object_info, color.x, color.y, color.z, color.w))
                    disp++;
                elem->mat_draw = mat;

                object_data->texture_pattern_count = 0;
                object_data->texture_transform_count = 0;
            }
        }
        rend_group->disp = disp;
    }

    void XRenderScene::CalcDispQuad(GPM, XRenderGroup* rend_group) {
        mat4 model_mat;
        mat4 dir_mat;
        mat4 view_mat;
        mat4 inv_view_mat;

        if (!rend_group->elements || !rend_group->vbo || rend_group->ctrl < 1)
            return;

        if (rend_group->flags & PARTICLE_EMITTER_LOCAL) {
            model_mat = rend_group->mat;
            if (rend_group->draw_type == DIRECTION_EMITTER_ROTATION
                || rend_group->draw_type == DIRECTION_PARTICLE_ROTATION)
                mat4_clear_rot(&rend_group->mat, &view_mat);
            else
                mat4_normalize_rotation(&rend_group->mat, &view_mat);
            mat4_mult(&view_mat, &((render_context*)GPM_VAL->rctx)->camera->view, &view_mat);
            mat4_inverse(&view_mat, &inv_view_mat);
        }
        else {
            model_mat = mat4_identity;
            view_mat = ((render_context*)GPM_VAL->rctx)->camera->view;
            inv_view_mat = ((render_context*)GPM_VAL->rctx)->camera->inv_view;
        }

        if (rend_group->flags & PARTICLE_LOCAL) {
            mat4_mult(&inv_view_mat, &rend_group->mat, &inv_view_mat);
            mat4_mult(&view_mat, &inv_view_mat, &view_mat);
            mat4_inverse(&view_mat, &inv_view_mat);
        }
        mat4_mult(&view_mat, &((render_context*)GPM_VAL->rctx)->camera->inv_view, &rend_group->mat_draw);

        switch (rend_group->draw_type) {
        case DIRECTION_BILLBOARD:
            mat4_clear_trans(&model_mat, &dir_mat);
            mat4_mult(&dir_mat, &inv_view_mat, &dir_mat);
            mat4_clear_trans(&dir_mat, &dir_mat);
            break;
        case DIRECTION_EMITTER_DIRECTION:
            dir_mat = rend_group->mat_rot;
            break;
        case DIRECTION_Y_AXIS:
            mat4_rotate_y((float_t)M_PI_2, &dir_mat);
            break;
        case DIRECTION_X_AXIS:
            mat4_rotate_x((float_t)-M_PI_2, &dir_mat);
            break;
        case DIRECTION_BILLBOARD_Y_AXIS:
            mat4_rotate_y(((render_context*)GPM_VAL->rctx)->camera->rotation.y, &dir_mat);
            break;
        default:
            dir_mat = mat4_identity;
            break;
        }

        switch (rend_group->draw_type) {
        case DIRECTION_PREV_POSITION:
        case DIRECTION_EMIT_POSITION:
        case DIRECTION_PREV_POSITION_DUP:
            CalcDispQuadDirectionRotation(rend_group, &model_mat);
            break;
        default:
            CalcDispQuadNormal(GPM_VAL, rend_group, &model_mat, &dir_mat);
            break;
        }
    }

    void XRenderScene::CalcDispQuadDirectionRotation(
        XRenderGroup* rend_group, mat4* model_mat) {
        mat4 inv_model_mat;
        mat4_inverse(model_mat, &inv_model_mat);
        mat4_clear_trans(&inv_model_mat, &inv_model_mat);

        vec3 x_vec_base = { 1.0f, 0.0f, 0.0f };
        vec3 y_vec_base = { 0.0f, 1.0f, 0.0f };

        mat4_mult_vec3(&inv_model_mat, &x_vec_base, &x_vec_base);
        mat4_mult_vec3(&inv_model_mat, &y_vec_base, &y_vec_base);

        vec3 up_vec;
        mat4(*rotate_func)(XRenderGroup*, RenderElement*, vec3*);
        if (rend_group->draw_type == DIRECTION_EMIT_POSITION) {
            up_vec = { 0.0f, 0.0f, 1.0f };
            rotate_func = XRenderGroup::RotateToEmitPosition;
        }
        else {
            up_vec = { 0.0f, 1.0f, 0.0f };
            rotate_func = XRenderGroup::RotateToPrevPosition;
        }

        bool use_scale = false;
        vec3 scale = vec3_identity;
        if (rend_group->flags & PARTICLE_EMITTER_LOCAL
            && rend_group->GetEmitterScale(&scale))
            use_scale = rend_group->flags & PARTICLE_SCALE ? true : false;

        gl_state_bind_array_buffer(rend_group->vbo);
        Buffer* buf = (Buffer*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        RenderElement* elem = rend_group->elements;
        size_t disp = 0;
        for (size_t i = rend_group->ctrl, j_max = 1024; i > 0; i -= j_max) {
            j_max = min(i, j_max);
            for (size_t j = j_max; j > 0; elem++) {
                if (!elem->alive)
                    continue;
                j--;

                if (!elem->disp)
                    continue;

                vec2 scale_particle;
                vec2_mult(*(vec2*)&elem->scale, elem->scale_particle, scale_particle);
                vec2_mult_scalar(scale_particle, elem->scale_all, scale_particle);

                if (fabsf(scale_particle.x) < 0.000001f || fabsf(scale_particle.y) < 0.000001f)
                    continue;

                vec3 pos = elem->translation;

                if (use_scale)
                    vec3_mult(pos, scale, pos);

                float_t v00;
                float_t v01;
                float_t v10;
                float_t v11;
                CalcDispQuadSetPivot(rend_group->pivot,
                    scale_particle.x, scale_particle.y,
                    &v00, &v01, &v10, &v11);

                vec2 pos_add[4];
                vec2 uv_add[4];
                pos_add[0].x = v00;
                pos_add[0].y = v11;
                uv_add[0].x = 0.0f;
                uv_add[0].y = 0.0f;
                pos_add[1].x = v00;
                pos_add[1].y = v10;
                uv_add[1].x = 0.0f;
                uv_add[1].y = rend_group->split_uv.y;
                pos_add[2].x = v01;
                pos_add[2].y = v10;
                uv_add[2].x = rend_group->split_uv.x;
                uv_add[2].y = rend_group->split_uv.y;
                pos_add[3].x = v01;
                pos_add[3].y = v11;
                uv_add[3].x = rend_group->split_uv.x;
                uv_add[3].y = 0.0f;

                vec2 base_uv;
                vec2_add(elem->uv_scroll, elem->uv, base_uv);

                mat4 mat = rotate_func(rend_group, elem, &up_vec);
                vec3 x_vec;
                mat4_mult_vec3(&mat, &x_vec_base, &x_vec);
                vec3 y_vec;
                mat4_mult_vec3(&mat, &y_vec_base, &y_vec);

                if (use_scale) {
                    vec3_mult(x_vec, scale, x_vec);
                    vec3_mult(y_vec, scale, y_vec);
                }

                float_t rot_z_cos = cosf(elem->rotation.z);
                float_t rot_z_sin = sinf(elem->rotation.z);
                for (int32_t k = 0; k < 4; k++, buf++) {
                    vec3 x_vec_rot;
                    vec3_mult_scalar(x_vec, pos_add[k].x * rot_z_cos - pos_add[k].y * rot_z_sin, x_vec_rot);
                    vec3 y_vec_rot;
                    vec3_mult_scalar(y_vec, pos_add[k].x * rot_z_sin + pos_add[k].y * rot_z_cos, y_vec_rot);

                    vec3 pos_add_rot;
                    vec3_add(x_vec_rot, y_vec_rot, pos_add_rot);
                    vec3_add(pos, pos_add_rot, buf->position);

                    vec2_add(base_uv, uv_add[k], buf->uv);
                    buf->color = elem->color;
                }
                disp++;
            }
        }
        rend_group->disp = disp;
        glUnmapBuffer(GL_ARRAY_BUFFER);
        gl_state_bind_array_buffer(0);
    }

    void XRenderScene::CalcDispQuadNormal(GPM,
        XRenderGroup* rend_group, mat4* model_mat, mat4* dir_mat) {
        mat4 inv_model_mat;
        mat4_inverse(model_mat, &inv_model_mat);
        mat4_clear_trans(&inv_model_mat, &inv_model_mat);

        vec3 x_vec = { 1.0f, 0.0f, 0.0f };
        vec3 y_vec = { 0.0f, 1.0f, 0.0f };
        bool use_z_offset = false;
        vec3 dist_to_cam = vec3_null;
        mat4 z_offset_inv_mat = mat4_identity;
        if (fabsf(rend_group->z_offset) > 0.000001f) {
            use_z_offset = true;
            mat4_get_translation(model_mat, &dist_to_cam);
            vec3_sub(((render_context*)GPM_VAL->rctx)->camera->view_point, dist_to_cam, dist_to_cam);
            if (rend_group->flags & PARTICLE_EMITTER_LOCAL) {
                mat4_normalize_rotation(model_mat, &z_offset_inv_mat);
                mat4_inverse(&z_offset_inv_mat, &z_offset_inv_mat);
            }
        }

        bool has_scale = false;
        bool emitter_local = false;
        vec3 scale = vec3_null;
        if (rend_group->flags & PARTICLE_EMITTER_LOCAL) {
            rend_group->GetEmitterScale(&scale);
            if (rend_group->flags & PARTICLE_SCALE) {
                x_vec.x = scale.x;
                y_vec.y = scale.y;
            }
            if (!(has_scale |= fabsf(scale.x) > 0.000001f ? true : false))
                scale.x = 0.0f;
            if (!(has_scale |= fabsf(scale.y) > 0.000001f ? true : false))
                scale.y = 0.0f;
            if (!(has_scale |= fabsf(scale.z) > 0.000001f ? true : false))
                scale.z = 0.0f;
            emitter_local = true;
        }

        vec3 ext_anim_scale;
        float_t some_scale = 0.0f;
        if (rend_group->GetExtAnimScale(&ext_anim_scale, &some_scale)) {
            if (some_scale <= 0.0f)
                some_scale = 1.0f;

            vec2_add_scalar(*(vec2*)&ext_anim_scale, some_scale, *(vec2*)&ext_anim_scale);
            x_vec.x *= ext_anim_scale.x;
            y_vec.y *= ext_anim_scale.y;
        }

        if (rend_group->draw_type != DIRECTION_BILLBOARD) {
            mat4_mult_vec3(dir_mat, &x_vec, &x_vec);
            mat4_mult_vec3(dir_mat, &y_vec, &y_vec);
        }
        else
            mat4_mult(&inv_model_mat, dir_mat, &inv_model_mat);

        mat4_mult_vec3(&inv_model_mat, &x_vec, &x_vec);
        mat4_mult_vec3(&inv_model_mat, &y_vec, &y_vec);

        gl_state_bind_array_buffer(rend_group->vbo);
        Buffer* buf = (Buffer*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        RenderElement* elem = rend_group->elements;
        size_t disp = 0;
        if (rend_group->draw_type == DIRECTION_PARTICLE_ROTATION)
            for (size_t i = rend_group->ctrl, j_max = 1024; i > 0; i -= j_max) {
                j_max = min(i, j_max);
                for (size_t j = j_max; j > 0; elem++) {
                    if (!elem->alive)
                        continue;
                    j--;

                    if (!elem->disp)
                        continue;

                    vec2 scale_particle;
                    vec2_mult(*(vec2*)&elem->scale, elem->scale_particle, scale_particle);
                    vec2_mult_scalar(scale_particle, elem->scale_all, scale_particle);

                    if (fabsf(scale_particle.x) < 0.000001f || fabsf(scale_particle.y) < 0.000001f)
                        continue;

                    vec3 pos = elem->translation;
                    if (has_scale)
                        vec3_mult(pos, scale, pos);

                    float_t v00;
                    float_t v01;
                    float_t v11;
                    float_t v10;
                    CalcDispQuadSetPivot(rend_group->pivot,
                        scale_particle.x, scale_particle.y,
                        &v00, &v01, &v10, &v11);

                    vec2 pos_add[4];
                    vec2 uv_add[4];
                    pos_add[0].x = v00;
                    pos_add[0].y = v11;
                    uv_add[0].x = 0.0f;
                    uv_add[0].y = 0.0f;
                    pos_add[1].x = v00;
                    pos_add[1].y = v10;
                    uv_add[1].x = 0.0f;
                    uv_add[1].y = rend_group->split_uv.y;
                    pos_add[2].x = v01;
                    pos_add[2].y = v10;
                    uv_add[2].x = rend_group->split_uv.x;
                    uv_add[2].y = rend_group->split_uv.y;
                    pos_add[3].x = v01;
                    pos_add[3].y = v11;
                    uv_add[3].x = rend_group->split_uv.x;
                    uv_add[3].y = 0.0f;

                    vec2 base_uv;
                    vec2_add(elem->uv_scroll, elem->uv, base_uv);
                    if (use_z_offset) {
                        vec3 z_offset_dir;
                        vec3_sub(dist_to_cam, pos, z_offset_dir);
                        vec3_normalize(z_offset_dir, z_offset_dir);

                        if (emitter_local)
                            mat4_mult_vec3(&z_offset_inv_mat, &z_offset_dir, &z_offset_dir);

                        vec3_mult_scalar(z_offset_dir, rend_group->z_offset, z_offset_dir);
                        vec3_add(pos, z_offset_dir, pos);
                    }

                    mat3 ptc_rot;
                    mat3_rotate(elem->rotation.x, elem->rotation.y, elem->rotation.z, &ptc_rot);
                    for (int32_t k = 0; k < 4; k++, buf++) {
                        vec3 x_vec_rot;
                        x_vec_rot.x = x_vec.x * pos_add[k].x;
                        x_vec_rot.y = y_vec.y * pos_add[k].y;
                        x_vec_rot.z = 0.0f;
                        mat3_mult_vec(&ptc_rot, &x_vec_rot, &x_vec_rot);
                        vec3_add(x_vec_rot, pos, buf->position);
                        vec2_add(base_uv, uv_add[k], buf->uv);
                        buf->color = elem->color;
                    }
                    disp++;
                }
            }
        else
            for (size_t i = rend_group->ctrl, j_max = 1024; i > 0; i -= j_max) {
                j_max = min(i, j_max);
                for (size_t j = j_max; j > 0; elem++) {
                    if (!elem->alive)
                        continue;
                    j--;

                    if (!elem->disp)
                        continue;

                    vec2 scale_particle;
                    vec2_mult(*(vec2*)&elem->scale, elem->scale_particle, scale_particle);
                    vec2_mult_scalar(scale_particle, elem->scale_all, scale_particle);

                    if (fabsf(scale_particle.x) < 0.000001f || fabsf(scale_particle.y) < 0.000001f)
                        continue;

                    vec3 pos = elem->translation;
                    if (has_scale)
                        vec3_mult(pos, scale, pos);

                    float_t v00;
                    float_t v01;
                    float_t v11;
                    float_t v10;
                    CalcDispQuadSetPivot(rend_group->pivot,
                        scale_particle.x, scale_particle.y,
                        &v00, &v01, &v10, &v11);

                    vec2 pos_add[4];
                    vec2 uv_add[4];
                    pos_add[0].x = v00;
                    pos_add[0].y = v11;
                    uv_add[0].x = 0.0f;
                    uv_add[0].y = 0.0f;
                    pos_add[1].x = v00;
                    pos_add[1].y = v10;
                    uv_add[1].x = 0.0f;
                    uv_add[1].y = rend_group->split_uv.y;
                    pos_add[2].x = v01;
                    pos_add[2].y = v10;
                    uv_add[2].x = rend_group->split_uv.x;
                    uv_add[2].y = rend_group->split_uv.y;
                    pos_add[3].x = v01;
                    pos_add[3].y = v11;
                    uv_add[3].x = rend_group->split_uv.x;
                    uv_add[3].y = 0.0f;

                    vec2 base_uv;
                    vec2_add(elem->uv_scroll, elem->uv, base_uv);
                    if (use_z_offset) {
                        vec3 z_offset_dir;
                        vec3_sub(dist_to_cam, pos, z_offset_dir);
                        vec3_normalize(z_offset_dir, z_offset_dir);

                        if (emitter_local)
                            mat4_mult_vec3(&z_offset_inv_mat, &z_offset_dir, &z_offset_dir);

                        vec3_mult_scalar(z_offset_dir, rend_group->z_offset, z_offset_dir);
                        vec3_add(pos, z_offset_dir, pos);
                    }

                    float_t rot_z_cos = cosf(elem->rotation.z);
                    float_t rot_z_sin = sinf(elem->rotation.z);
                    for (int32_t k = 0; k < 4; k++, buf++) {
                        vec3 x_vec_rot;
                        vec3_mult_scalar(x_vec, rot_z_cos * pos_add[k].x - rot_z_sin * pos_add[k].y, x_vec_rot);
                        vec3 y_vec_rot;
                        vec3_mult_scalar(y_vec, rot_z_sin * pos_add[k].x + rot_z_cos * pos_add[k].y, y_vec_rot);
                        vec3_add(x_vec_rot, y_vec_rot, x_vec_rot);
                        vec3_add(x_vec_rot, pos, buf->position);
                        vec2_add(base_uv, uv_add[k], buf->uv);
                        buf->color = elem->color;
                    }
                    disp++;
                }
            }
        rend_group->disp = disp;
        glUnmapBuffer(GL_ARRAY_BUFFER);
        gl_state_bind_array_buffer(0);
    }

    void XRenderScene::Ctrl(float_t delta_frame, bool copy_mats) {
        ctrl_quad = 0;
        ctrl_line = 0;
        ctrl_locus = 0;
        ctrl_mesh = 0;

        for (XRenderGroup*& i : groups)
            if (i) {
                switch (i->type) {
                case PARTICLE_QUAD:
                    ctrl_quad += i->ctrl;
                    break;
                case PARTICLE_LINE:
                    ctrl_line += i->ctrl;
                    break;
                case PARTICLE_LOCUS:
                    ctrl_locus += i->ctrl;
                    break;
                case PARTICLE_MESH:
                    ctrl_mesh += i->ctrl;
                    break;
                }

                i->Ctrl(delta_frame, copy_mats);
            }
    }

    void XRenderScene::Disp(GPM, draw_pass_3d_type alpha) {
        disp_quad = 0;
        disp_line = 0;
        disp_locus = 0;

        XEffectInst* eff = dynamic_cast<XEffectInst*>(GPM_VAL->effect);
        XEmitterInst* emit = dynamic_cast<XEmitterInst*>(GPM_VAL->emitter);
        XParticleInst* ptcl = dynamic_cast<XParticleInst*>(GPM_VAL->particle);
        for (XRenderGroup* i : groups) {
            if (!i)
                continue;

            XRenderGroup* rend_group = i;
            if ((rend_group)->alpha != alpha || rend_group->CannotDisp() && !GPM_VAL->draw_all)
                continue;

#if !defined(CRE_DEV)
            Disp(GPM_VAL, rend_group);
#else
            if (!GPM_VAL->draw_selected || !eff) {
                Disp(GPM_VAL, rend_group);
            }
            else if ((eff && ptcl) || (eff && !emit)) {
                if (!ptcl || rend_group->particle == ptcl)
                    Disp(GPM_VAL, rend_group);
            }
            else if (emit)
                for (XParticleInst*& i : emit->particles) {
                    if (!i)
                        continue;

                    XParticleInst* particle = i;
                    if (rend_group->particle == particle)
                        Disp(GPM_VAL, rend_group);

                    for (XParticleInst*& j : particle->data.children)
                        if (j && rend_group->particle == j)
                            Disp(GPM_VAL, rend_group);
                }
#endif
        }
    }

    void XRenderScene::Disp(GPM, XRenderGroup* rend_group) {
        switch (rend_group->type) {
        case PARTICLE_QUAD:
        case PARTICLE_LINE:
        case PARTICLE_LOCUS:
            break;
        default:
            return;
        }

        rend_group->disp = 0;
        switch (rend_group->type) {
        case PARTICLE_QUAD:
            CalcDispQuad(GPM_VAL, rend_group);
            disp_quad += rend_group->disp;
            break;
        case PARTICLE_LINE:
            CalcDispLine(rend_group);
            disp_line += rend_group->disp;
            break;
        case PARTICLE_LOCUS:
            CalcDispLocus(GPM_VAL, rend_group);
            disp_locus += rend_group->disp;
            break;
        }

        if (rend_group->disp < 1)
            return;

        gl_state_enable_blend();
        switch (rend_group->blend_mode) {
        case PARTICLE_BLEND_ADD:
            gl_state_set_blend_func(GL_SRC_ALPHA, GL_ONE);
            break;
        case PARTICLE_BLEND_MULTIPLY:
            gl_state_set_blend_func(GL_ZERO, GL_SRC_COLOR);
            break;
        default:
            gl_state_set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        }
        gl_state_set_blend_equation(GL_FUNC_ADD);

        if (rend_group->type != PARTICLE_LINE && rend_group->texture) {
            gl_state_active_bind_texture_2d(0, rend_group->texture);
            if (rend_group->mask_texture) {
                gl_state_active_bind_texture_2d(1, rend_group->mask_texture);

                uniform_value[U_TEXTURE_COUNT] = 2;
                switch (rend_group->mask_blend_mode) {
                default:
                    uniform_value[U_TEXTURE_BLEND] = 0;
                    break;
                case PARTICLE_BLEND_MULTIPLY:
                    uniform_value[U_TEXTURE_BLEND] = 1;
                    break;
                case PARTICLE_BLEND_ADD:
                    uniform_value[U_TEXTURE_BLEND] = 2;
                    break;
                }
            }
            else {
                gl_state_active_bind_texture_2d(1, 0);
                uniform_value[U_TEXTURE_COUNT] = 1;
                uniform_value[U_TEXTURE_BLEND] = 0;
            }
        }
        else {
            gl_state_active_bind_texture_2d(0, 0);
            gl_state_active_bind_texture_2d(1, 0);
            uniform_value[U_TEXTURE_COUNT] = 0;
            uniform_value[U_TEXTURE_BLEND] = 0;
        }

        switch (rend_group->fog) {
        default:
            uniform_value[U_FOG_HEIGHT] = 0;
            break;
        case FOG_DEPTH:
            uniform_value[U_FOG_HEIGHT] = 1;
            break;
        case FOG_HEIGHT:
            uniform_value[U_FOG_HEIGHT] = 2;
            break;
        }

        if (rend_group->blend_mode == PARTICLE_BLEND_PUNCH_THROUGH)
            uniform_value[U_ALPHA_BLEND] = rend_group->alpha != DRAW_PASS_3D_OPAQUE ? 3 : 1;
        else
            uniform_value[U_ALPHA_BLEND] = rend_group->alpha != DRAW_PASS_3D_OPAQUE ? 2 : 0;

        if (~rend_group->particle->data.flags & PARTICLE_DEPTH_TEST) {
            gl_state_enable_depth_test();
            gl_state_set_depth_func(GL_LESS);
        }
        else
            gl_state_disable_depth_test();

        if (rend_group->blend_mode == PARTICLE_BLEND_PUNCH_THROUGH)
            gl_state_set_depth_mask(GL_TRUE);
        else
            gl_state_set_depth_mask(GL_FALSE);

        if (rend_group->draw_type == DIRECTION_BILLBOARD && !rend_group->use_culling) {
            gl_state_enable_cull_face();
            gl_state_set_cull_face_mode(GL_BACK);
        }
        else
            gl_state_disable_cull_face();

        float_t emission = 1.0f;
        if (rend_group->flags & PARTICLE_EMISSION || rend_group->blend_mode == PARTICLE_BLEND_TYPICAL)
            emission = rend_group->emission;

        shaders_ft.state_material_set_emission(false, emission, emission, emission, 1.0f);
        shaders_ft.state_matrix_set_mvp(rend_group->mat_draw,
            ((render_context*)GPM_VAL->rctx)->camera->view,
            ((render_context*)GPM_VAL->rctx)->camera->projection);
        shaders_ft.state_matrix_set_texture(0, mat4_identity);
        shaders_ft.state_matrix_set_texture(1, mat4_identity);
        shaders_ft.env_vert_set(3, vec4_identity);

        shaders_ft.set(SHADER_FT_GLITTER_PT);
        switch (rend_group->type) {
        case PARTICLE_QUAD: {
            static const GLsizei buffer_size = sizeof(Buffer);

            gl_state_bind_array_buffer(rend_group->vbo);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, buffer_size,
                (void*)offsetof(Buffer, position)); // Pos
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, buffer_size,
                (void*)offsetof(Buffer, color));    // Color
            glEnableVertexAttribArray(8);
            glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, buffer_size,
                (void*)offsetof(Buffer, uv));       // TexCoord0
            glEnableVertexAttribArray(9);
            glVertexAttribPointer(9, 2, GL_FLOAT, GL_FALSE, buffer_size,
                (void*)offsetof(Buffer, uv));       // TexCoord1
            gl_state_bind_array_buffer(0);

            gl_state_bind_element_array_buffer(rend_group->ebo);
            shaders_ft.draw_elements(GL_TRIANGLES, (GLsizei)(6 * rend_group->disp), GL_UNSIGNED_INT, 0);
            gl_state_bind_element_array_buffer(0);

            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(3);
            glDisableVertexAttribArray(8);
            glDisableVertexAttribArray(9);
        } break;
        case PARTICLE_LINE:
        case PARTICLE_LOCUS: {
            static const GLsizei buffer_size = sizeof(Buffer);

            gl_state_bind_array_buffer(rend_group->vbo);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, buffer_size,
                (void*)offsetof(Buffer, position)); // Pos
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, buffer_size,
                (void*)offsetof(Buffer, color));    // Color
            glEnableVertexAttribArray(8);
            glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, buffer_size,
                (void*)offsetof(Buffer, uv));       // TexCoord0
            glEnableVertexAttribArray(9);
            glVertexAttribPointer(9, 2, GL_FLOAT, GL_FALSE, buffer_size,
                (void*)offsetof(Buffer, uv));       // TexCoord1
            gl_state_bind_array_buffer(0);

            const GLenum mode = rend_group->type == PARTICLE_LINE ? GL_LINE_STRIP : GL_TRIANGLE_STRIP;
            for (std::pair<GLint, GLsizei>& i : rend_group->draw_list)
                shaders_ft.draw_arrays(mode, i.first, i.second);

            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(3);
            glDisableVertexAttribArray(8);
            glDisableVertexAttribArray(9);

        } break;
        }
        gl_state_disable_blend();
        gl_state_disable_cull_face();
        gl_state_disable_depth_test();
    }

    void XRenderGroup::DeleteBuffers(bool a2) {
        if (particle) {
            if (!a2)
                particle->data.render_group = 0;
            particle = 0;
        }

        if (ebo) {
            glDeleteBuffers(1, &ebo);
            ebo = 0;
        }

        if (vbo) {
            glDeleteBuffers(1, &vbo);
            vbo = 0;
        }

        if (!a2 && elements) {
            Free();
            free(elements);
        }
    }
}
