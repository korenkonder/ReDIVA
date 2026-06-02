/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "render.hpp"
#include "renderer/dof.hpp"
#include "renderer/transparency.hpp"
#include "rob/rob.hpp"
#include "gl_rend_state.hpp"
#include "gl_state.hpp"
#include "reflect_full.hpp"
#include "render_context.hpp"
#include "render_manager.hpp"
#include "shader_ft.hpp"
#include "stage.hpp"
#include "stage_modern.hpp"
#include "static_var.hpp"
#include "texture.hpp"

extern bool task_stage_is_modern;

extern render_context* rctx_ptr;

extern dof_pv dof_pv_data;

#define MLAA_MAX_EDGE_DETECTION_LEN (3)
#define MLAA_GRID_SIDE_LEN (2 * MLAA_MAX_EDGE_DETECTION_LEN + 1)
#define MLAA_SIDE_LEN (5 * MLAA_GRID_SIDE_LEN)

namespace rndr {
    // Added
    static void get_sun_transform(mat4& transform, const vec3& position, float_t radius, const cam_data& cam);

    Render::ExposureCharaData::ExposureCharaData() : spot_weight(), query(), query_data() {
        reset();
    }

    Render::ExposureCharaData::~ExposureCharaData() {

    }

    void Render::ExposureCharaData::reset() {
        for (GLuint& i : query_data)
            i = -1;
        spot_weight = 0.0f;
    }

    Render::CaptureData::CaptureData() : txhd() {
        type = CAPTURE_MAX;
    }

    Render::CaptureData::~CaptureData() {

    }

    Render::Capture::Capture() : capture() {

    }

    Render::Capture::~Capture() {

    }

    // 0x1404AD770
    void Render::draw_quad_simple(render_data_context& rend_data_ctx, float_t trnsl) {
        //NOT IMPL!
    }

    // 0x1404AD870
    void Render::draw_quad_sun(render_data_context& rend_data_ctx, const mat4& transform, const vec4& emission) {
        sun_quad_shader_data shader_data = {};
        shader_data.g_emission = emission;

        mat4 tmp;
        mat4_transpose(&transform, &tmp);
        shader_data.g_transform[0] = tmp.row0;
        shader_data.g_transform[1] = tmp.row1;
        shader_data.g_transform[2] = tmp.row2;
        shader_data.g_transform[3] = tmp.row3;
        rend_data_ctx.state.write_uniform_buffer(rctx_ptr->sun_quad_ubo, shader_data);

        rend_data_ctx.state.bind_uniform_buffer_base(0, rctx_ptr->sun_quad_ubo);
        rend_data_ctx.state.bind_vertex_array(rctx_ptr->common_vao);
        rend_data_ctx.state.draw_arrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    // 0x1404AD650
    void Render::draw_quad_ghost(int32_t index, float_t opacity, mat4* mat, float_t*& data) {
        const float_t x0 = index & 0x01 ? 0.5f : 0.0f;
        const float_t x1 = index & 0x01 ? 1.0f : 0.5f;
        const float_t y0 = index & 0x02 ? 0.5f : 0.0f;
        const float_t y1 = index & 0x02 ? 1.0f : 0.5f;

        vec4 p0 = { -0.5f, -0.5f, x0, y0 };
        vec4 p1 = {  0.5f, -0.5f, x1, y0 };
        vec4 p2 = { -0.5f,  0.5f, x0, y1 };
        vec4 p3 = {  0.5f,  0.5f, x1, y1 };
        mat4_transform_point(mat, (vec2*)&p0, (vec2*)&p0);
        mat4_transform_point(mat, (vec2*)&p1, (vec2*)&p1);
        mat4_transform_point(mat, (vec2*)&p2, (vec2*)&p2);
        mat4_transform_point(mat, (vec2*)&p3, (vec2*)&p3);

        *(vec4*)&data[0] = p0;
        data[4] = opacity;
        *(vec4*)&data[5] = p1;
        data[9] = opacity;
        *(vec4*)&data[10] = p2;
        data[14] = opacity;
        *(vec4*)&data[15] = p2;
        data[19] = opacity;
        *(vec4*)&data[20] = p1;
        data[24] = opacity;
        *(vec4*)&data[25] = p3;
        data[29] = opacity;
        data += 30;
    }

    // 0x1404AF2A0
    void Render::measure_exposure(render_data_context& rend_data_ctx, const cam_data& cam) {
        render_context* rctx = rctx_ptr;

        bool v2 = false;
        if (!reset_exposure) {
            ExposureCharaData* chara_data = exposure_chara_data;
            for (int32_t i = 0; i < ROB_ID_MAX; i++, chara_data++) {
                rob_chara* rob_chr = get_rob_management()->get_rob((ROB_ID)i);
                if (!rob_chr || !rob_chr->get_disp())
                    continue;

                vec4 v34 = { 0.05f, 0.0f, -0.04f, 1.0f };
                vec4 v33 = { 1.0f, 0.0f, 0.0f, 0.0f };

                mat4 v44 = mat4_identity;
                rob_chr->sub_1405163C0(4, v44);

                mat4 v42;
                mat4 v43;
                mat4_mul(&v44, &cam.view_mat, &v42);
                mat4_mul(&v42, &cam.proj_mat, &v43);

                vec4 v41;
                vec4 v40;
                mat4_transform_vector(&v42, &v34, &v41);
                mat4_transform_vector(&v43, &v34, &v40);

                float_t v12 = 1.0f / v40.w;
                float_t v13 = v40.x * v12;
                float_t v14 = v40.y * v12;
                if (v41.z >= 0.0f || fabsf(v13) >= 1.0f || fabsf(v14) >= 1.0f)
                    continue;

                float_t v31 = 0.5f - (fabsf(v14) - 1.0f) * 2.5f;
                float_t v32 = 0.5f - (fabsf(v13) - 1.0f) * 2.5f;

                float_t v16 = min_def(min_def(v31, v32), 1.0f);

                vec4 v39;
                mat4_transform_vector(&v42, &v33, &v39);

                float_t v17 = (v39.z + 1.0f) * 0.5f;
                if (v17 > 0.8f)
                    v17 = 0.8f;
                else if (v17 < 0.2f)
                    v17 = 0.0f;

                float_t v18 = (float_t)scr_height / (float_t)scr_width;
                float_t v20 = tanf(cam.fov * 0.5f * DEG_TO_RAD_FLOAT);
                float_t v21 = 0.25f / sqrtf(powf(v20 * 3.4f, 2.0f) * (v41.z * v41.z));
                float_t v22;
                if (v21 < 0.055f)
                    v22 = max_def((v21 - 0.035f) * 50.0f, 0.0f);
                else if (v21 > 0.5f)
                    v22 = max_def(1.0f - (v21 - 0.5f) * 3.3333333f, 0.0f);
                else
                    v22 = 1.0f;

                chara_data->spot_coefficients[0].x = (v13 + v21 * 0.0f * v18 + 1.0f) * 0.5f;
                chara_data->spot_coefficients[0].y = (v14 + v21 * 0.1f + 1.0f) * 0.5f;
                chara_data->spot_coefficients[0].z = 0.0f;
                chara_data->spot_coefficients[0].w = 4.0f;
                chara_data->spot_coefficients[1].x = (v13 + v21 * 0.0f * v18 + 1.0f) * 0.5f;
                chara_data->spot_coefficients[1].y = (v14 - v21 * 0.3f + 1.0f) * 0.5f;
                chara_data->spot_coefficients[1].z = 0.0f;
                chara_data->spot_coefficients[1].w = 4.0f;
                chara_data->spot_coefficients[2].x = (v13 + v21 * -0.5f * v18 + 1.0f) * 0.5f;
                chara_data->spot_coefficients[2].y = (v14 + v21 * -0.5f + 1.0f) * 0.5f;
                chara_data->spot_coefficients[2].z = 0.0f;
                chara_data->spot_coefficients[2].w = 3.0f;
                chara_data->spot_coefficients[3].x = (v13 - v21 * 0.6f * v18 + 1.0f) * 0.5f;
                chara_data->spot_coefficients[3].y = (v14 - v21 * 0.1f + 1.0f) * 0.5f;
                chara_data->spot_coefficients[3].z = 0.0f;
                chara_data->spot_coefficients[3].w = 2.0f;
                chara_data->spot_coefficients[4].x = (v13 + v21 * 0.6f * v18 + 1.0f) * 0.5f;
                chara_data->spot_coefficients[4].y = (v14 - v21 * 0.1f + 1.0f) * 0.5f;
                chara_data->spot_coefficients[4].z = 0.0f;
                chara_data->spot_coefficients[4].w = 2.0f;
                chara_data->spot_coefficients[5].x = (v13 + v21 * 0.5f * v18 + 1.0f) * 0.5f;
                chara_data->spot_coefficients[5].y = (v14 + v21 * -0.5f + 1.0f) * 0.5f;
                chara_data->spot_coefficients[5].z = 0.0f;
                chara_data->spot_coefficients[5].w = 3.0f;
                chara_data->spot_coefficients[6].x = (v13 + v21 * 0.0f * v18 + 1.0f) * 0.5f;
                chara_data->spot_coefficients[6].y = (v14 - v21 * 0.8f + 1.0f) * 0.5f;
                chara_data->spot_coefficients[6].z = 0.0f;
                chara_data->spot_coefficients[6].w = 3.0f;
                chara_data->spot_weight = v16 * 1.6f * v17 * v22;
                if (chara_data->spot_weight > 0.0f && !chara_data->query_data[exposure_chara_write_index])
                    v2 = true;
            }
        }

        if (!v2 || reset_exposure) {
            exposure_measure_shader_data exposure_measure = {};
            exposure_measure.g_spot_weight = {
                exposure_chara_data[0].spot_weight,
                exposure_chara_data[1].spot_weight,
                exposure_chara_data[2].spot_weight,
                exposure_chara_data[3].spot_weight
            };

            ExposureCharaData* chara_data = exposure_chara_data;
            for (int32_t i = 0; i < 4 && i < ROB_ID_MAX; i++, chara_data++)
                for (int32_t j = 0; j < 8; j++)
                    exposure_measure.g_spot_coefficients[i * 8 + j] = chara_data->spot_coefficients[j];
            rend_data_ctx.state.write_uniform_buffer(rctx->exposure_measure_ubo, exposure_measure);

            rend_data_ctx.state.set_viewport(0, 0, exposure_tex_width, 1);
            work_fbo.begin_render(rend_data_ctx.state);

            rend_data_ctx.shader_flags.arr[U_EXPOSURE] = 1;
            shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_EXPOSURE);
            rend_data_ctx.state.bind_uniform_buffer_base(1, rctx->exposure_measure_ubo);
            rend_data_ctx.state.active_bind_texture_2d(0, blur_txhd[4]->glid);
            rend_data_ctx.state.active_bind_texture_2d(1, blur_txhd[2]->glid);
            rend_data_ctx.state.bind_sampler(0, rctx->render_samplers[2]);
            rend_data_ctx.state.bind_sampler(1, rctx->render_samplers[2]);
            draw_quad(rend_data_ctx, 1, 1, 1.0f, 1.0f,
                0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

            rend_data_ctx.state.active_bind_texture_2d(0, exposure_txhd->glid);
            if (reset_exposure)
                rend_data_ctx.state.copy_tex_sub_image_2d(GL_TEXTURE_2D, 0, 0, 0, 0, 0, exposure_tex_width, 1);
            else
                rend_data_ctx.state.copy_tex_sub_image_2d(GL_TEXTURE_2D, 0, exposure_write_index, 0, 0, 0, 1, 1);
            exposure_write_index = (exposure_write_index + 1) % exposure_tex_width;
        }

        rend_data_ctx.state.set_viewport(0, 0, 1, 1);
        exposure_avg_fbo.begin_render(rend_data_ctx.state);
        rend_data_ctx.shader_flags.arr[U_EXPOSURE] = 2;
        shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_EXPOSURE);
        rend_data_ctx.state.active_bind_texture_2d(0, exposure_txhd->glid);
        rend_data_ctx.state.bind_sampler(0, rctx->render_samplers[0]);
        draw_quad(rend_data_ctx, 1, 1, 1.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    }

    // 0x1404B11F0
    void Render::reduce_texture(render_data_context& rend_data_ctx) {
        render_context* rctx = rctx_ptr;

        rend_data_ctx.shader_flags.arr[U_REDUCE_TEX] = 1;
        shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_REDUCE);
        for (int32_t i = 1; i < fb_level - 1; i++) {
            fb_fbo[i].begin_render(rend_data_ctx.state);
            rend_data_ctx.state.active_bind_texture_2d(0, fb_fbo[i - 1].get_texture_glid());
            rend_data_ctx.state.bind_sampler(0, rctx->render_samplers[0]);
            rend_data_ctx.state.set_viewport(0, 0, fb_width[i], fb_height[i]);
            draw_quad(rend_data_ctx,
                fb_tex_width[i - 1], fb_tex_height[i - 1],
                fb_tex_width_scale, fb_tex_height_scale,
                0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        }

        rend_data_ctx.shader_flags.arr[U_REDUCE_TEX] = 3;
        shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_REDUCE);

        int32_t downsample = max_def(fb_level - 2, 0);
        rend_data_ctx.state.set_viewport(0, 0, fb_work_width[0], fb_work_height[0]);
        blur_fbo[0].begin_render(rend_data_ctx.state);
        rend_data_ctx.state.active_bind_texture_2d(0, fb_fbo[downsample].get_texture_glid());
        rend_data_ctx.state.bind_sampler(0, rctx->render_samplers[0]);
        draw_quad(rend_data_ctx,
            fb_tex_width[downsample], fb_tex_height[downsample],
            fb_tex_width_scale, fb_tex_height_scale,
            0.0f, 0.0f, 1.0f, 1.1f, 1.1f, 1.1f, 0.0f);

        work_fbo.begin_render(rend_data_ctx.state);
        rend_data_ctx.state.active_bind_texture_2d(0, blur_txhd[0]->glid);
        rend_data_ctx.state.bind_sampler(0, rctx->render_samplers[2]);
        for (int32_t i = 1; i < 5; i++) {
            float_t scale;
            if (i == 4) {
                scale = 1.0f;
                rend_data_ctx.shader_flags.arr[U_EXPOSURE] = 0;
                shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_EXPOSURE);
            }
            else
            {
                scale = 0.75;
                rend_data_ctx.shader_flags.arr[U_REDUCE_TEX] = 1;
                shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_REDUCE);
            }

            rend_data_ctx.state.set_viewport(0, 0, fb_work_width[i], fb_work_height[i]);
            draw_quad(rend_data_ctx, fb_work_width[i - 1], fb_work_height[i - 1],
                1.0f, 1.0f, 0.0f, 0.0f, scale, 1.0f, 1.0f, 1.0f, 1.0f);
            rend_data_ctx.state.active_bind_texture_2d(0, blur_txhd[i]->glid);
            rend_data_ctx.state.bind_sampler(0, rctx->render_samplers[2]);
            rend_data_ctx.state.copy_tex_sub_image_2d(GL_TEXTURE_2D,
                0, 0, 0, 0, 0, fb_work_width[i], fb_work_height[i]);
        }
    }

    // 0x1404AEAF0
    void Render::handle_blur_texture(render_data_context& rend_data_ctx) {
        render_context* rctx = rctx_ptr;

        rend_data_ctx.shader_flags.arr[U_ALPHA_MASK] = 0;
        rend_data_ctx.shader_flags.arr[U_GAUSS] = 0;
        shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_GAUSS);
        set_gauss_coef(rend_data_ctx, 1.0f, 1.0f, 7, 0.8f, 1.0f);
        rend_data_ctx.state.bind_uniform_buffer_base(1, rctx->gaussian_coef_ubo);

        work_fbo.begin_render(rend_data_ctx.state);
        for (int32_t i = 1; i < 4; i++) {
            rend_data_ctx.state.set_viewport(0, 0, fb_work_width[i], fb_work_height[i]);
            rend_data_ctx.state.active_bind_texture_2d(0, blur_txhd[i]->glid);
            rend_data_ctx.state.bind_sampler(0, rctx->render_samplers[2]);
            draw_quad(rend_data_ctx, fb_work_width[i], fb_work_height[i],
                1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f);
            rend_data_ctx.state.copy_tex_sub_image_2d(GL_TEXTURE_2D,
                0, 0, 0, 0, 0, fb_work_width[i], fb_work_height[i]);
        }

        for (int32_t i = 1; i < 4; i++) {
            rend_data_ctx.state.set_viewport(0, 0, fb_work_width[i], fb_work_height[i]);
            rend_data_ctx.state.active_bind_texture_2d(0, blur_txhd[i]->glid);
            rend_data_ctx.state.bind_sampler(0, rctx->render_samplers[2]);
            draw_quad(rend_data_ctx, fb_work_width[i], fb_work_height[i],
                1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
            rend_data_ctx.state.copy_tex_sub_image_2d(GL_TEXTURE_2D,
                0, 0, 0, 0, 0, fb_work_width[i], fb_work_height[i]);
        }

        rend_data_ctx.shader_flags.arr[U_GAUSS] = 1;
        shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_GAUSS);

        rend_data_ctx.state.set_viewport(0, 0, fb_work_width[0], fb_work_height[0]);
        rend_data_ctx.state.active_bind_texture_2d(0, blur_txhd[0]->glid);
        rend_data_ctx.state.bind_sampler(0, rctx->render_samplers[2]);
        draw_quad(rend_data_ctx, fb_work_width[0], fb_work_height[0],
            1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            filter_inten[0] * 0.5f, filter_inten[1] * 0.5f, filter_inten[2] * 0.5f, 1.0f);

        rend_data_ctx.shader_flags.arr[U_REDUCE_TEX] = 7;
        shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_REDUCE);

        rend_data_ctx.state.set_viewport(0, 0, fb_work_width[0], fb_work_height[0]);
        blur_fbo[0].begin_render(rend_data_ctx.state);
        rend_data_ctx.state.active_bind_texture_2d(0, work_fbo.get_texture_glid());
        rend_data_ctx.state.active_bind_texture_2d(1, blur_txhd[1]->glid);
        rend_data_ctx.state.active_bind_texture_2d(2, blur_txhd[2]->glid);
        rend_data_ctx.state.active_bind_texture_2d(3, blur_txhd[3]->glid);
        rend_data_ctx.state.bind_sampler(0, rctx->render_samplers[2]);
        rend_data_ctx.state.bind_sampler(1, rctx->render_samplers[2]);
        rend_data_ctx.state.bind_sampler(2, rctx->render_samplers[2]);
        rend_data_ctx.state.bind_sampler(3, rctx->render_samplers[2]);
        draw_quad(rend_data_ctx, fb_work_width[3], fb_work_height[3],
            1.0f, 1.0f, 0.0f, 0.0f, 0.25f, 0.15f, 0.25f, 0.25f, 0.25f);
        bloom_tex = blur_txhd[0]->glid;
    }

    // 0x1404AFFD0
    static float_t mlaa_calc_area_tex_val(int32_t a1, int32_t a2) {
        float_t v2 = (float_t)(-0.5 / ((float_t)a2 - 0.5));
        float_t v3 = (float_t)a1 * v2 + 0.5f;
        if (a1 >= a2 - 1)
            return (v3 * 0.5f) * 0.5f;
        else
            return ((float_t)(a1 + 1) * v2 + 0.5f + v3) * 0.5f;
    };

    // 0x1404B2870
    void Render::tone_map(render_data_context& rend_data_ctx, texture* light_proj_tex, int32_t npr_param) {
        render_context* rctx = rctx_ptr;

        rend_data_ctx.state.begin_event("PostProcess::tone_map");

        set_tone_map_texture(rend_data_ctx.state);

        int32_t fade_blend_func = this->fade_blend_func[fade_index];

        vec4 fade_color;
        fade_color.x = this->fade_color[fade_index][0];
        fade_color.y = this->fade_color[fade_index][1];
        fade_color.z = this->fade_color[fade_index][2];
        fade_color.w = fade_rate[fade_index];
        if (fade_color.w <= 0.01)
            fade_color.w = 0.0f;
        else if (fade_blend_func == 1 || fade_blend_func == 2)
            *(vec3*)&fade_color *= fade_color.w;

        float_t litproj_quality = 1.0f;

        GLuint litproj_tex = 0;
        if (light_proj_tex)
            litproj_tex = light_proj_tex->glid;

        GLuint composite_back_tex = 0;
        if (composite_back)
            composite_back_tex = composite_back_txhd->glid;

        vec4 exposure;
        exposure.x = this->exposure * exposure_rate;
        exposure.y = 0.0625f;
        exposure.z = this->exposure * exposure_rate * 0.5f;
        exposure.w = auto_exposure ? 1.0f : 0.0f;

        float_t flare_coef = (this->flare_coef[0] * 2.0f) * (flare_alpha_offset + flare_alpha);
        float_t shaft_coef = this->flare_coef[1] * 2.0f;

        vec3 tone_trans_scale = vec3(this->tone_trans_scale[tone_trans_index][0],
            this->tone_trans_scale[tone_trans_index][1], this->tone_trans_scale[tone_trans_index][2]);
        vec3 tone_trans_offset = vec3(this->tone_trans_offset[tone_trans_index][0],
            this->tone_trans_offset[tone_trans_index][1], this->tone_trans_offset[tone_trans_index][2]);
        vec2 flare_pos = *(vec2*)&this->flare_pos;

        tone_map(rend_data_ctx, 0, 0, fb_width[0], fb_height[0],
            fb_fbo[0].get_texture_glid(), bloom_tex, composite_back_tex, litproj_tex, litproj_quality,
            fb_width_org, fb_height_org, fb_tex_width_scale, fb_tex_height_scale,
            exposure, tone_trans_scale, tone_trans_offset, fade_blend_func, fade_color,
            tone_map_method, gamma, flare_pos, flare_scale, flare_coef, shaft_coef,
            contour_fbo->get_texture_glid(), contour_fbo->get_depth_texture_glid(), fb_fbo[0].get_depth_texture_glid(),
            npr_param == 1, false, rctx->render_manager->npr_mask);
        rend_data_ctx.state.end_event();
    }

    // Added
    void Render::tone_map(render_data_context& rend_data_ctx, int64_t a3, int64_t a4,
        int32_t render_width, int32_t render_height, GLuint color_tex, GLuint bloom_tex,
        GLuint composite_back_tex, GLuint litproj_tex, float_t litproj_quality, int32_t width, int32_t height,
        float_t s0, float_t t0, const vec4& exposure, const vec3& tone_scale, const vec3& tone_offset,
        int32_t fade_blend_func, const vec4& fade_color, int32_t tone_map, float_t gamma,
        const vec2& flare_pos, float_t flare_scale, float_t flare_coef, float_t shaft_coef,
        GLuint contour_color_tex, GLuint contour_depth_tex, GLuint scene_depth_tex,
        bool npr1, bool a31, bool npr_mask) {
        render_context* rctx = rctx_ptr;

        rend_data_ctx.shader_flags.arr[U_TONE_MAP_METHOD] = (int32_t)tone_map;
        rend_data_ctx.shader_flags.arr[U_FLARE] = 0;
        rend_data_ctx.shader_flags.arr[U_COMPOSITE_BACK] = 0;
        rend_data_ctx.shader_flags.arr[U_LIGHT_PROJ] = 0;

        rend_data_ctx.state.active_bind_texture_2d(0, color_tex);
        rend_data_ctx.state.bind_sampler(0, rctx->render_samplers[1]);
        rend_data_ctx.state.active_bind_texture_2d(1, bloom_tex);
        rend_data_ctx.state.bind_sampler(1, rctx->render_samplers[2]);
        rend_data_ctx.state.active_bind_texture_2d(2, tone_map_tex);
        rend_data_ctx.state.bind_sampler(2, rctx->render_samplers[0]);
        rend_data_ctx.state.active_bind_texture_2d(3, exposure_avg_tex);
        rend_data_ctx.state.bind_sampler(3, rctx->render_samplers[3]);

        if (composite_back) {
            rend_data_ctx.shader_flags.arr[U_COMPOSITE_BACK] = 1;
            rend_data_ctx.state.active_bind_texture_2d(6, composite_back_tex);
        }
        else
            rend_data_ctx.state.active_bind_texture_2d(6, rctx->empty_texture_2d->glid);
        rend_data_ctx.state.bind_sampler(6, rctx->render_samplers[2]);

        if (litproj_tex) {
            rend_data_ctx.shader_flags.arr[U_LIGHT_PROJ] = 1;
            rend_data_ctx.state.active_bind_texture_2d(7, litproj_tex);
        }
        else
            rend_data_ctx.state.active_bind_texture_2d(7, rctx->empty_texture_2d->glid);
        rend_data_ctx.state.bind_sampler(7, rctx->render_samplers[2]);

        struct tonemap_flare {
            vec3 transform_x;
            float_t coef;
            vec3 transform_y;

            inline tonemap_flare() : coef() {

            }
        };

        struct tonemap_texcoord_transforms {
            tonemap_flare flare;
            float_t param;

            inline tonemap_texcoord_transforms() : param() {

            }
        };

        float_t v35 = (float_t)render_width / (float_t)render_width * s0;
        float_t v36 = (float_t)render_height / (float_t)render_height * t0;

        tonemap_texcoord_transforms texcoord_transforms[2];

        if (flare_textures[0]) {
            const float_t aspect = (float_t)height * (1.0f / (float_t)width);

            rend_data_ctx.shader_flags.arr[U_FLARE] = 1;
            rend_data_ctx.state.active_bind_texture_2d(4, flare_textures[0]);
            rend_data_ctx.state.bind_sampler(4, rctx->render_samplers[2]);

            mat4 mat;
            mat4_translate(0.5f, 0.5f, 0.0f, &mat);
            mat4_scale_rot(&mat, 0.75f, 0.75f, 1.0f, &mat);
            mat4_mul_rotate_z(&mat, flare_pos.x * (1.0f / (float_t)width)
                * (float_t)(25.0 * DEG_TO_RAD_FLOAT), &mat);
            mat4_mul_translate(&mat, flare_pos.x * (-1.0f / (float_t)width),
                (flare_pos.y - (float_t)height) * (1.0f / (float_t)width), 0.0f, &mat);
            mat4_scale_rot(&mat, 0.5f, 0.5f * aspect, 1.0f, &mat);
            mat4_mul_translate(&mat, 1.0f, 1.0f, 0.0f, &mat);

            texcoord_transforms[0].flare.transform_x = vec3(mat.row0.x, mat.row1.x, mat.row3.x);
            texcoord_transforms[0].flare.coef = flare_coef;
            texcoord_transforms[0].flare.transform_y = vec3(mat.row0.y, mat.row1.y, mat.row3.y);

            if (flare_scale < 50.0f) {
                rend_data_ctx.shader_flags.arr[U_FLARE] = 2;
                rend_data_ctx.state.active_bind_texture_2d(5, flare_textures[1]);
                rend_data_ctx.state.bind_sampler(5, rctx->render_samplers[2]);

                mat4_translate(0.5f, 0.5f, 0.0f, &mat);
                mat4_scale_rot(&mat, flare_scale, flare_scale, 1.0f, &mat);
                mat4_mul_rotate_z(&mat, flare_pos.x * (1.0f / (float_t)width)
                    * (float_t)(60.0 * DEG_TO_RAD_FLOAT), &mat);
                mat4_mul_translate(&mat, flare_pos.x * (-1.0f / (float_t)width),
                    (flare_pos.y - (float_t)height) * (1.0f / (float_t)width), 0.0f, &mat);
                mat4_scale_rot(&mat, 0.5f, 0.5f * aspect, 1.0f, &mat);
                mat4_mul_translate(&mat, 1.0f, 1.0f, 0.0f, &mat);

                texcoord_transforms[1].flare.transform_x = vec3(mat.row0.x, mat.row1.x, mat.row3.x);
                texcoord_transforms[1].flare.coef = shaft_coef;
                texcoord_transforms[1].flare.transform_y = vec3(mat.row0.y, mat.row1.y, mat.row3.y);
            }
            else {
                rend_data_ctx.state.active_bind_texture_2d(5, rctx->empty_texture_2d->glid);
                rend_data_ctx.state.bind_sampler(5, rctx->render_samplers[2]);
            }
        }
        else {
            rend_data_ctx.state.active_bind_texture_2d(4, rctx->empty_texture_2d->glid);
            rend_data_ctx.state.bind_sampler(4, rctx->render_samplers[2]);
            rend_data_ctx.state.active_bind_texture_2d(5, rctx->empty_texture_2d->glid);
            rend_data_ctx.state.bind_sampler(5, rctx->render_samplers[2]);
        }

        if (tone_map == 0 && composite_back_tex && npr1) {
            rend_data_ctx.state.active_bind_texture_2d(14, scene_depth_tex);
            rend_data_ctx.state.bind_sampler(14, rctx->render_samplers[1]);
        }
        else {
            rend_data_ctx.state.active_bind_texture_2d(14, rctx->empty_texture_2d->glid);
            rend_data_ctx.state.bind_sampler(14, rctx->render_samplers[2]);
        }

        texcoord_transforms[0].param = litproj_quality;
        texcoord_transforms[1].param = litproj_tex ? 1.0f : 0.0f;

        tone_map_shader_data shader_data;
        shader_data.g_exposure = exposure;
        shader_data.g_fade_color = fade_color;
        *(vec3*)&shader_data.g_tone_scale = tone_scale;
        shader_data.g_tone_scale.w = (float_t)fade_blend_func;
        *(vec3*)&shader_data.g_tone_offset = tone_offset;
        shader_data.g_tone_offset.w = gamma > 0.0f ? 2.0f / (gamma * 3.0f) : 0.0f;
        memcpy(shader_data.g_texcoord_transforms, texcoord_transforms, sizeof(vec4) * 4);

        rend_data_ctx.state.write_uniform_buffer(rctx->tone_map_ubo, shader_data);

        rend_data_ctx.state.set_viewport(0, 0, render_width, render_height);
        scr_fbo[num_scr_tex - 1].begin_render(rend_data_ctx.state);
        shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_TONEMAP);
        rend_data_ctx.state.bind_uniform_buffer_base(1, rctx->tone_map_ubo);
        draw_quad(rend_data_ctx, width, height, v35, v36, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        rend_data_ctx.state.active_bind_texture_2d(2, 0);
    }

    // 0x1404B0110
    void Render::mlaa_calc_area_value(float_t& val_left, float_t& val_right,
        int32_t cross1, int32_t cross2, int32_t dleft, int32_t dright) {
        int32_t d = dleft + dright + 1;
        float_t _val_left = 0.0f;
        float_t _val_right = 0.0f;
        switch (cross2) {
        case 0:
            switch (cross1) {
            case 1:
                _val_right = mlaa_calc_area_tex_val(dleft, min_def(d, MLAA_GRID_SIDE_LEN));
                break;
            case 3:
                _val_left = mlaa_calc_area_tex_val(dleft, min_def(d, MLAA_GRID_SIDE_LEN));
                break;
            }
            break;
        case 1:
            switch (cross1) {
            case 0:
                _val_right = mlaa_calc_area_tex_val(dright, min_def(d, MLAA_GRID_SIDE_LEN));
                break;
            case 1:
                if (d % 2) {
                    int32_t v12 = d / 2 + 1;
                    int32_t v15 = d / 2;
                    if (dleft < v15)
                        _val_right = mlaa_calc_area_tex_val(dleft, v12);
                    else if (dright < v15)
                        _val_right = mlaa_calc_area_tex_val(dright, v12);
                    else
                        _val_right = mlaa_calc_area_tex_val(v15, v12) * 2.0f;
                }
                else {
                    int32_t v12 = d / 2;
                    float_t v14 = -0.5f / (float_t)v12;
                    if (dleft >= v12)
                        _val_right = ((float_t)(dright * 2 + 1) * v14 + 1.0f) * 0.5f;
                    else
                        _val_right = ((float_t)(dleft * 2 + 1) * v14 + 1.0f) * 0.5f;
                }
                break;
            case 3:
                if (d % 2) {
                    int32_t v12 = d / 2 + 1;
                    int32_t v15 = d / 2;
                    if (dleft < v15)
                        _val_left = mlaa_calc_area_tex_val(dleft, v12);
                    else if (dright < v15)
                        _val_right = mlaa_calc_area_tex_val(dright, v12);
                    else
                        _val_left = _val_right = mlaa_calc_area_tex_val(v15, v12);
                }
                else {
                    int32_t v12 = d / 2;
                    float_t v14 = -0.5f / (float_t)v12;
                    if (dleft < v12)
                        _val_left = ((float_t)(dleft * 2 + 1) * v14 + 1.0f) * 0.5f;
                    else
                        _val_right = ((float_t)(dright * 2 + 1) * v14 + 1.0f) * 0.5f;
                }
                break;
            case 4:
                if (d % 2) {
                    int32_t v12 = d / 2 + 1;
                    int32_t v15 = d / 2;
                    if (dleft < v15)
                        _val_left = mlaa_calc_area_tex_val(dleft, v12);
                    else if (dright < v15)
                        _val_right = mlaa_calc_area_tex_val(dright, v12);
                    else
                        _val_left = _val_right = mlaa_calc_area_tex_val(v15, v12);
                }
                else {
                    int32_t v12 = d / 2;
                    float_t v14 = -0.5f / (float_t)v12;
                    if (dleft < v12)
                        _val_left = ((float_t)(dleft * 2 + 1) * v14 + 1.0f) * 0.5f;
                    else
                        _val_right = ((float_t)(dright * 2 + 1) * v14 + 1.0f) * 0.5f;
                }
                break;
            }
            break;
        case 3:
            switch (cross1) {
            case 0:
                _val_left = mlaa_calc_area_tex_val(dright, min_def(d, MLAA_GRID_SIDE_LEN));
                break;
            case 1:
                if (d % 2) {
                    int32_t v12 = d / 2 + 1;
                    int32_t v15 = d / 2;
                    if (dleft < v15)
                        _val_right = mlaa_calc_area_tex_val(dleft, v12);
                    else if (dright < v15)
                        _val_left = mlaa_calc_area_tex_val(dright, v12);
                    else
                        _val_left = _val_right = mlaa_calc_area_tex_val(v15, v12);
                }
                else {
                    int32_t v12 = d / 2;
                    float_t v14 = -0.5f / (float_t)v12;
                    if (dleft < v12)
                        _val_right = ((float_t)(dleft * 2 + 1) * v14 + 1.0f) * 0.5f;
                    else
                        _val_left = ((float_t)(dright * 2 + 1) * v14 + 1.0f) * 0.5f;
                }
                break;
            case 3:
                if (d % 2) {
                    int32_t v12 = d / 2 + 1;
                    int32_t v15 = d / 2;
                    if (dleft < v15)
                        _val_left = mlaa_calc_area_tex_val(dleft, v12);
                    else if (dright < v15)
                        _val_left = mlaa_calc_area_tex_val(dright, v12);
                    else
                        _val_left = mlaa_calc_area_tex_val(v15, v12) * 2.0f;
                }
                else {
                    int32_t v12 = d / 2;
                    float_t v14 = -0.5f / (float_t)v12;
                    if (dleft < v12)
                        _val_left = ((float_t)(dleft * 2 + 1) * v14 + 1.0f) * 0.5f;
                    else
                        _val_left = ((float_t)(dright * 2 + 1) * v14 + 1.0f) * 0.5f;
                }
                break;
            case 4:
                if (d % 2) {
                    int32_t v12 = d / 2 + 1;
                    int32_t v15 = d / 2;
                    if (dleft < v15)
                        _val_right = mlaa_calc_area_tex_val(dleft, v12);
                    else if (dright < v15)
                        _val_left = mlaa_calc_area_tex_val(dright, v12);
                    else
                        _val_left = _val_right = mlaa_calc_area_tex_val(v15, v12);
                }
                else {
                    int32_t v12 = d / 2;
                    float_t v14 = -0.5f / (float_t)v12;
                    if (dleft < v12)
                        _val_right = ((float_t)(dleft * 2 + 1) * v14 + 1.0f) * 0.5f;
                    else
                        _val_left = ((float_t)(dright * 2 + 1) * v14 + 1.0f) * 0.5f;
                }
                break;
            }
            break;
        case 4:
            switch (cross1) {
            case 1:
                if (d % 2) {
                    int32_t v12 = d / 2 + 1;
                    int32_t v15 = d / 2;
                    if (dleft < v15)
                        _val_right = mlaa_calc_area_tex_val(dleft, v12);
                    else if (dright < v15)
                        _val_left = mlaa_calc_area_tex_val(dright, v12);
                    else
                        _val_left = _val_right = mlaa_calc_area_tex_val(v15, v12);
                }
                else {
                    int32_t v12 = d / 2;
                    float_t v14 = -0.5f / (float_t)v12;
                    if (dleft < v12)
                        _val_right = ((float_t)(dleft * 2 + 1) * v14 + 1.0f) * 0.5f;
                    else
                        _val_left = ((float_t)(dright * 2 + 1) * v14 + 1.0f) * 0.5f;
                }
                break;
            case 3:
                if (d % 2) {
                    int32_t v12 = d / 2 + 1;
                    int32_t v15 = d / 2;
                    if (dleft < v15)
                        _val_left = mlaa_calc_area_tex_val(dleft, v12);
                    else if (dright < v15)
                        _val_right = mlaa_calc_area_tex_val(dright, v12);
                    else
                        _val_left = _val_right = mlaa_calc_area_tex_val(v15, v12);
                }
                else {
                    int32_t v12 = d / 2;
                    float_t v14 = -0.5f / (float_t)v12;
                    if (dleft < v12)
                        _val_left = ((float_t)(dleft * 2 + 1) * v14 + 1.0f) * 0.5f;
                    else
                        _val_right = ((float_t)(dright * 2 + 1) * v14 + 1.0f) * 0.5f;
                }
                break;
            }
            break;
        }

        val_left = _val_left;
        val_right = _val_right;
    }

    // 0x1404B0040
    void Render::mlaa_calc_area_texture(uint8_t* data, int32_t cross1, int32_t cross2) {
        uint8_t* _data = &data[(MLAA_SIDE_LEN * MLAA_GRID_SIDE_LEN * cross2 + MLAA_GRID_SIDE_LEN * cross1) * 2];
        for (int32_t dright = 0; dright < MLAA_GRID_SIDE_LEN; dright++) {
            for (int32_t dleft = 0; dleft < MLAA_GRID_SIDE_LEN; dleft++) {
                float_t val_left;
                float_t val_right;
                mlaa_calc_area_value(val_left, val_right, cross1, cross2, dleft, dright);
                _data[0] = (uint8_t)(val_left * 255.9f);
                _data[1] = (uint8_t)(val_right * 255.9f);
                _data += 2;
            }
            _data += MLAA_GRID_SIDE_LEN * 4 * 2;
        }
    }

    // 0x1404B0440
    void Render::mlaa_gen_area_texture() {
        uint8_t* data = force_malloc<uint8_t>(MLAA_SIDE_LEN * MLAA_SIDE_LEN * 2);
        if (!data)
            return;

        for (int32_t cross2 = 0; cross2 < 5; cross2++)
            for (int32_t cross1 = 0; cross1 < 5; cross1++)
                mlaa_calc_area_texture(data, cross1, cross2);

        glGenTextures(1, &mlaa_area_tex);
        gl_state.bind_texture_2d(mlaa_area_tex);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, MLAA_SIDE_LEN, MLAA_SIDE_LEN, 0, GL_RG, GL_UNSIGNED_BYTE, data);
        gl_state.bind_texture_2d(0);
        free_def(data);
    }

    // 0x1404AFB30
    void Render::mlaa(render_data_context& rend_data_ctx,
        int32_t destination, int32_t source, int32_t ss_alpha_mask) {
        rend_data_ctx.state.begin_event("PostProcess::mlaa");
        if (morphological_anti_alias) {
            mlaa_fbo.begin_render(rend_data_ctx.state);
            rend_data_ctx.state.active_bind_texture_2d(0, scr_txhd[source]->glid);
            rend_data_ctx.state.bind_sampler(0, rctx_ptr->render_samplers[1]);
            rend_data_ctx.shader_flags.arr[U_MLAA] = 0;
            shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_MLAA);
            draw_quad(rend_data_ctx, fb_tex_width[0], fb_tex_height[0],
                fb_tex_width_scale, fb_tex_height_scale,
                0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

            user_fbo.begin_render(rend_data_ctx.state);
            rend_data_ctx.state.active_bind_texture_2d(0, mlaa_fbo.get_texture_glid());
            rend_data_ctx.state.active_bind_texture_2d(1, mlaa_area_tex);
            rend_data_ctx.state.bind_sampler(0, rctx_ptr->render_samplers[0]);
            rend_data_ctx.state.bind_sampler(1, rctx_ptr->render_samplers[3]);
            rend_data_ctx.shader_flags.arr[U_MLAA] = 1;
            rend_data_ctx.shader_flags.arr[U_MLAA_SEARCH] = 2;
            shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_MLAA);
            draw_quad(rend_data_ctx, fb_tex_width[0], fb_tex_height[0],
                fb_tex_width_scale, fb_tex_height_scale,
                0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

            scr_fbo[destination].begin_render(rend_data_ctx.state);
            rend_data_ctx.state.active_bind_texture_2d(0, scr_txhd[source]->glid);
            rend_data_ctx.state.active_bind_texture_2d(1, user_fbo.get_texture_glid());
            rend_data_ctx.state.bind_sampler(0, rctx_ptr->render_samplers[1]);
            rend_data_ctx.state.bind_sampler(1, rctx_ptr->render_samplers[1]);
            rend_data_ctx.shader_flags.arr[U_MLAA] = 2;
            rend_data_ctx.shader_flags.arr[U_ALPHA_MASK] = ss_alpha_mask ? 1 : 0;
            shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_MLAA);
            draw_quad(rend_data_ctx, fb_tex_width[0], fb_tex_height[0],
                fb_tex_width_scale, fb_tex_height_scale,
                0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
            rend_data_ctx.shader_flags.arr[U_ALPHA_MASK] = 0;
            rend_data_ctx.state.active_bind_texture_2d(0, 0);
            rend_data_ctx.state.active_bind_texture_2d(1, 0);
        }
        else {
            scr_fbo[destination].begin_render(rend_data_ctx.state);
            rend_data_ctx.state.active_bind_texture_2d(0, scr_txhd[source]->glid);
            rend_data_ctx.state.bind_sampler(0, rctx_ptr->render_samplers[1]);
            rend_data_ctx.shader_flags.arr[U_REDUCE_TEX] = 0;
            shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_REDUCE);
            draw_quad(rend_data_ctx, fb_tex_width[0], fb_tex_height[0],
                fb_tex_width_scale, fb_tex_height_scale,
                0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        }
        rend_data_ctx.state.end_event();
    }

    // 0x1404B1DD0
    void Render::set_gauss_coef(render_data_context& rend_data_ctx, float_t start, float_t step,
        int32_t kernel_size, float_t radius_scale, float_t intensity_scale) {
        auto calculate_gaussian_kernel = [&](float_t* gaussian_kernel,
            float_t sigma, int32_t stride, int32_t offset) {
                float_t first_val = (start - step * 0.5f) * 2.0f;
                for (int32_t i = 0; i < kernel_size; i++)
                    gaussian_kernel[i * stride + offset] = 0.0f;

                float_t temp_gaussian_kernel[20];
                float_t s = sigma * radius_scale;
                s = 1.0f / (2.0f * s * s);

                double_t sum = first_val;
                temp_gaussian_kernel[0] = first_val;
                float_t val = start;
                for (size_t i = 1; i < kernel_size; i++) {
                    sum += temp_gaussian_kernel[i] = expf(-(val * val * s)) * step;
                    val += step;
                }

                sum = 1.0f / sum;
                for (size_t i = 0; i < kernel_size; i++)
                    gaussian_kernel[i * stride + offset] = (float_t)(temp_gaussian_kernel[i] * sum);
        };

        vec3 gauss[20];
        calculate_gaussian_kernel((float_t*)gauss, sigma[0], 3, 0);
        calculate_gaussian_kernel((float_t*)gauss, sigma[1], 3, 1);
        calculate_gaussian_kernel((float_t*)gauss, sigma[2], 3, 2);

        const vec3 inten = vec3(filter_inten[0], filter_inten[1], filter_inten[2]) * (intensity_scale * 0.5f);

        gaussian_coef_shader_data gaussian_coef = {};
        for (int32_t i = 0; i < kernel_size && i < 8; i++) {
            *(vec3*)&gaussian_coef.g_coef[i] = gauss[i] * inten;
            gaussian_coef.g_coef[i].w = 0.0f;
        }
        rend_data_ctx.state.write_uniform_buffer(rctx_ptr->gaussian_coef_ubo, gaussian_coef);
    }

    // 0x1404B23E0
    void Render::set_tone_map_texture(p_gl_rend_state& p_gl_rend_st) {
        if (!recalc_tone_map)
            return;

        recalc_tone_map = 0;
        saturate_lock = 0;

        const float_t tone_map_scale = (float_t)(1.0 / (double_t)TONE_MAP_SAT_GAMMA_SAMPLES);

        vec2 tex_data[16 * TONE_MAP_SAT_GAMMA_SAMPLES];
        float_t gamma_power = gamma * gamma_rate * 1.5f;
        int32_t saturate_power = this->saturate_power;
        float_t saturate_coef = this->saturate_coef[saturate_index];

        tex_data[0].x = 0.0f;
        tex_data[0].y = 0.0f;
        for (int32_t i = 1; i < tone_map_tex_width; i++) {
            float_t gamma = powf(1.0f - expf((float_t)-i * tone_map_scale), gamma_power);
            float_t saturation = gamma * 2.0f - 1.0f;
            for (int32_t j = 0; j < saturate_power; j++) {
                saturation *= saturation;
                saturation *= saturation;
                saturation *= saturation;
                saturation *= saturation;
            }

            tex_data[i].x = gamma;
            tex_data[i].y = gamma * saturate_coef
                * ((float_t)TONE_MAP_SAT_GAMMA_SAMPLES / (float_t)i) * (1.0f - saturation);
        }

        if (GLAD_GL_VERSION_4_5)
            p_gl_rend_st.texture_sub_image_2d(tone_map_tex, 0, 0, 0,
                tone_map_tex_width, 1, GL_RG, GL_FLOAT, tex_data);
        else {
            p_gl_rend_st.bind_texture_2d(tone_map_tex);
            p_gl_rend_st.tex_sub_image_2d(GL_TEXTURE_2D, 0, 0, 0,
                tone_map_tex_width, 1, GL_RG, GL_FLOAT, tex_data);
        }
    }

    // 0x1404A8380
    Render::Render() : min_res(), ss_alpha_mask(), scr_txhd(), composite_back_txhd(), blur_txhd(),
        bloom_tex(), exposure_avg_tex(), exposure_txhd(), tone_map_tex(), mlaa_area_tex(),
        tex_count(), sun_queries_center(), sun_queries_sphere(), sun_pixels_center(), sun_pixels_sphere(),
        query_read_index(), flare_textures(), num_ghosts(), scr_width(), scr_height(), fb_width_org(), fb_height_org(),
        fb_width(), fb_height(), fb_tex_width(), fb_tex_height(), fb_tex_width_scale(), fb_tex_height_scale(),
        fb_work_width(), fb_work_height(), scr_index(), scr_disp_index(), reset_exposure(),
        view_x(), view_y(), view_w(), view_h(), saturate_lock(), exposure_write_index(), exposure_chara_write_index(),
        flare_scale(), flare_alpha(), flare_alpha_enable(), flare_alpha_offset(), fb_copy_txhd(),
        fb_movie_txhd(), composite_back(), dof(), transparency_tex(), transparency(),
        saturate_index(), fade_index(), tone_trans_index(), saturate_coef(),
        fade_color(), fade_rate(), fade_blend_func(), tone_trans_scale(), tone_trans_offset(),
        tone_trans_start(), tone_trans_end(), tone_map_method(), mag_filter(), fade_tex_type(), update() {
        exposure = 2.0f;
        exposure_rate = 1.0f;
        auto_exposure = 1;

        scr_tex_rate = -1.0f;

        stage_num = -1;
        prev_stage_num = -1;

        reset_exposure = true;

        gamma = 1.0f;
        gamma_rate = 1.0f;

        mag_filter = MAG_FILTER_BILINEAR;

        saturate_power = 1;

        for (int32_t i = 0; i < 2; i++) {
            set_saturate_coef_default(i);
            set_fade_color_default(i);
            set_tone_trans_default(i);
        }

        fade_tex_rate = -1.0f;

        recalc_tone_map = 1;

        flare_scale_default = 1.0f;
        flare_coef[0] = 1.0f;
        flare_coef[1] = 1.0f;
        flare_coef[2] = 1.0f;

        sigma[0] = 2.0f;
        sigma[1] = 2.0f;
        sigma[2] = 2.0f;
        filter_inten[0] = 1.0f;
        filter_inten[1] = 1.0f;
        filter_inten[2] = 1.0f;

        for (GLuint& i : sun_pixels_center)
            i = -1;

        for (GLuint& i : sun_pixels_sphere)
            i = -1;
    }

    // 0x1404A8B20
    Render::~Render() {
        if (composite_back_txhd)
            texture_release(composite_back_txhd);

        if (dof) {
            delete dof;
            dof = 0;
        }

        if (transparency) {
            delete transparency;
            transparency = 0;
        }
    }

    // 0x1404A9790
    void Render::calc_scr_tex_rate() {
        scr_tex_rate = 1.0f;

        float_t pos_dist = vec3::distance(cam_pos, prev_cam_pos);
        float_t intr_dist = vec3::distance(cam_intr, prev_cam_intr);
        float_t dist = max_def(pos_dist, intr_dist);

        if (!reset_exposure) {
            if (dist < 0.0001f)
                scr_tex_rate = 0.5f;
            else if (dist < 0.005f)
                scr_tex_rate = 0.75f;
            else if (dist < 0.03f)
                scr_tex_rate = 0.875f;
            else if (dist <= 0.05f && cam_blur)
                scr_tex_rate = 0.999f;
        }
    }

    // 0x1404AB0C0
    void Render::create_render_buffer(int32_t width, int32_t height,
        int32_t aa, int32_t minify, int32_t ss_alpha_mask) {
        anti_alias = aa;
        min_render = minify;
        this->ss_alpha_mask = ss_alpha_mask;

        morphological_anti_alias = 1;
        temporal_anti_alias = aa || ss_alpha_mask ? 0 : 1;
        mag_filter = minify ? MAG_FILTER_SHARPEN_4_TAP : MAG_FILTER_BILINEAR;

        scr_width = width;
        scr_height = height;

        if (aa) {
            fb_width_org = 2 * width;
            fb_height_org = 2 * height;
        }
        else {
            fb_width_org = width;
            fb_height_org = height;
        }

        view_x = 0;
        view_y = 0;

        view_w = width;
        view_h = height;

        fb_work_width[0] = 256;
        fb_work_height[0] = 144;
        fb_work_width[1] = 128;
        fb_work_height[1] = 72;
        fb_work_width[2] = 64;
        fb_work_height[2] = 36;
        fb_work_width[3] = 32;
        fb_work_height[3] = 18;
        fb_work_width[4] = 8;
        fb_work_height[4] = 8;

        calc_draw_size(true, 1);

        fb_fbo[0].create_texture(fb_tex_width[0], fb_tex_height[0], 0, GL_RGBA16F, GL_DEPTH_COMPONENT24);

        gl_state.bind_texture_2d(fb_fbo[0].get_texture_glid());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        gl_state.bind_texture_2d(fb_fbo[0].get_depth_texture_glid());
        GLint swizzle[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
        gl_state.bind_texture_2d(0);

        for (int32_t i = 0; i < num_scr_tex; i++) {
            scr_txhd[i] = texture_load_tex_2d(texture_id(0x25, tex_count++),
                GL_RGBA8, fb_tex_width[0], fb_tex_height[0], 0, 0, 0);
            scr_fbo[i].attach_texture(scr_txhd[i]->glid, 0, fb_fbo[0].get_depth_texture_glid());
        }

        composite_back_txhd = texture_load_tex_2d(texture_id(0x25, tex_count++),
            GL_RGBA8, fb_tex_width[0], fb_tex_height[0], 0, 0, 0);
        composite_back_fbo.attach_texture(composite_back_txhd->glid, 0, fb_fbo[0].get_depth_texture_glid());

        mlaa_fbo.create_texture(fb_tex_width[0], fb_tex_height[0], 0, GL_RGBA8, GL_DEPTH_COMPONENT24);
        user_fbo.create_texture(fb_tex_width[0], fb_tex_height[0], 0, GL_RGBA8, GL_ZERO);
        contour_fbo = &mlaa_fbo;

        gl_state.bind_texture_2d(mlaa_fbo.get_depth_texture_glid());
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
        gl_state.bind_texture_2d(0);

        for (int32_t i = 1; i < fb_level; i++) {
            fb_fbo[i].create_texture(fb_tex_width[i], fb_tex_height[i], 0, GL_RGBA16F, GL_ZERO);

            gl_state.bind_texture_2d(fb_fbo[i].get_texture_glid());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            gl_state.bind_texture_2d(0);
        }

        work_fbo.create_texture(fb_work_width[0], fb_work_height[0], 0, GL_RGBA16F, GL_ZERO);

        gl_state.bind_texture_2d(work_fbo.get_texture_glid());
        const vec4 border_color = 0.0f;
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, (GLfloat*)&border_color);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        gl_state.bind_texture_2d(0);

        fade_fbo.create_texture(fade_tex_width, fade_tex_height, 0, GL_RGBA8, GL_ZERO);

        dof = new renderer::DOF3(fb_tex_width[0], fb_tex_height[0]);

        for (int32_t i = 0; i < 1; i++)
            transparency_tex[i] = texture_load_tex_2d(texture_id(0x25, tex_count++),
                GL_RGBA16F, fb_tex_width[0], fb_tex_height[0], 0, 0, 0);

        transparency = new renderer::Transparency(transparency_tex[0]->glid,
            fb_fbo[0].get_depth_texture_glid(), fb_tex_width[0], fb_tex_height[0]);
    }

    // 0x1404A9FF0
    void Render::create_other() {
        for (int32_t i = 0; i < num_blur_tex; i++) {
            blur_txhd[i] = texture_load_tex_2d(texture_id(0x25, tex_count++),
                GL_RGBA16F, fb_work_width[i], fb_work_height[i], 0, 0, 0);
            gl_state.bind_texture_2d(blur_txhd[i]->glid);

            const vec4 border_color = 0.0f;
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, (GLfloat*)&border_color);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        }
        gl_state.bind_texture_2d(0);

        blur_fbo[0].attach_texture(blur_txhd[0]->glid);

        exposure_txhd = texture_load_tex_2d(texture_id(0x25, tex_count++),
            GL_RGBA16F, exposure_tex_width, 2, 0, 0, 0);

        glGenTextures(1, &exposure_avg_tex);
        gl_state.bind_texture_2d(exposure_avg_tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 2, 2, 0, GL_RGBA, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        gl_state.bind_texture_2d(0);

        exposure_avg_fbo.attach_texture(exposure_avg_tex);

        glGenTextures(1, &tone_map_tex);
        gl_state.bind_texture_2d(tone_map_tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, tone_map_tex_width, 1, 0, GL_RG, GL_HALF_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        const GLint swizzle[] = { GL_RED, GL_RED, GL_RED, GL_GREEN };
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
        gl_state.bind_texture_2d(0);

        mlaa_gen_area_texture();

        glGenQueries(num_queries, sun_queries_center);
        glGenQueries(num_queries, sun_queries_sphere);

        for (GLuint& i : sun_pixels_center)
            i = -1;

        for (GLuint& i : sun_pixels_sphere)
            i = -1;

        for (ExposureCharaData& i : exposure_chara_data) {
            glGenQueries(num_queries, i.query);
            for (GLuint& j : i.query_data)
                j = -1;
        }
    }

    // 0x1404AB900
    void Render::destroy() {
        if (transparency) {
            delete transparency;
            transparency = 0;
        }

        for (int32_t i = 0; i < 1; i++)
            if (transparency_tex[i]) {
                texture_release(transparency_tex[i]);
                transparency_tex[i] = 0;
                tex_count--;
            }

        if (dof) {
            delete dof;
            dof = 0;
        }

        if (!tex_count)
            return;

        for (int32_t i = 0; i < blur_level; i++)
            fb_fbo[i].destroy();

        for (int32_t i = 0; i < num_scr_tex; i++) {
            scr_fbo[i].destroy();

            if (scr_txhd[i]) {
                texture_release(scr_txhd[i]);
                scr_txhd[i] = 0;
                tex_count--;
            }
        }

        composite_back_fbo.destroy();

        if (composite_back_txhd) {
            texture_release(composite_back_txhd);
            composite_back_txhd = 0;
            tex_count--;
        }

        mlaa_fbo.destroy();
        user_fbo.destroy();

        contour_fbo = 0;

        work_fbo.destroy();
        fade_fbo.destroy();

        for (int32_t i = 0; i < num_blur_tex; i++)
            blur_fbo[i].destroy();

        for (int32_t i = 0; i < num_blur_tex; i++)
            if (blur_txhd[i]) {
                texture_release(blur_txhd[i]);
                blur_txhd[i] = 0;
                tex_count--;
            }

        if (exposure_txhd) {
            texture_release(exposure_txhd);
            exposure_txhd = 0;
            tex_count--;
        }

        exposure_avg_fbo.destroy();

        if (exposure_avg_tex) {
            glDeleteTextures(1, &exposure_avg_tex);
            exposure_avg_tex = 0;
        }

        if (tone_map_tex) {
            glDeleteTextures(1, &tone_map_tex);
            tone_map_tex = 0;
        }

        if (mlaa_area_tex) {
            glDeleteTextures(1, &mlaa_area_tex);
            mlaa_area_tex = 0;
        }

        glDeleteQueries(num_queries, sun_queries_center);
        glDeleteQueries(num_queries, sun_queries_sphere);

        for (ExposureCharaData& i : exposure_chara_data) {
            glDeleteQueries(num_queries, i.query);
            i.query[0] = 0;
        }
    }

    // 0x1404A9480
    void Render::calc_draw_size(bool first_time, int32_t min_res) {
        const int32_t min_render_widths[4] = {
            1280, 1024, 1024, 960,
        };

        const int32_t min_render_heights[4] = {
            720, 576, 576, 544,
        };

        min_res = min_def(min_res, 3);

        if (first_time)
            this->min_res = min_res;
        else
            min_res = max_def(min_res, this->min_res);

        min_res = clamp_def(min_res, 0, 3);

        int32_t min_render_width = min_render_widths[min_res];
        int32_t min_render_height = min_render_heights[min_res];
        if (min_render && (view_w > min_render_width || view_h > min_render_height)) {
            fb_width[0] = min_render_width;
            fb_height[0] = min_render_height;

            if (first_time) {
                fb_width_org = fb_width[0];
                fb_height_org = fb_height[0];
            }
        }
        else {
            float_t round_val = anti_alias ? 1.0f : 16.0f;
            fb_width[0] = (int32_t)(prj::ceilf((float_t)view_w
                * (float_t)fb_width_org / (float_t)scr_width * (1.0f / round_val)) * round_val);
            fb_height[0] = (int32_t)(prj::ceilf((float_t)view_h
                * (float_t)fb_height_org / (float_t)scr_height * (1.0f / round_val)) * round_val);
        }

        int32_t level = 1;
        for (int32_t i = 0; i < fb_level_max - 1; i++, level++) {
            int32_t width = (fb_width[i] + 1) / 2;
            int32_t height = (fb_height[i] + 1) / 2;
            if (width <= fb_work_width[0] && height <= fb_work_height[0])
                break;

            fb_width[level] = width;
            fb_height[level] = height;
        }
        fb_level = level;

        if (first_time) {
            blur_level = level;
            for (int32_t i = 0; i < level; i++) {
                fb_tex_width[i] = fb_width[i];
                fb_tex_height[i] = fb_height[i];
            }
        }

        fb_tex_width_scale = (float_t)fb_width[0] / (float_t)fb_tex_width[0];
        fb_tex_height_scale = (float_t)fb_height[0] / (float_t)fb_tex_height[0];

        for (int32_t i = 0; i < GL_REND_STATE_COUNT; i++) {
            render_data_context rend_data_ctx((gl_rend_state_index)i);
            rend_data_ctx.set_scene_framebuffer_size(fb_width[0], fb_height[0], fb_width[0], fb_height[0]);
        }

        scr_tex_rate = -1.0f;
    }

    // 0x1404A9350
    void Render::begin_render(p_gl_rend_state& p_gl_rend_st, bool composite_back) {
        if (composite_back) {
            composite_back_fbo.begin_render(p_gl_rend_st);
            this->composite_back = true;
        }
        else
            fb_fbo[0].begin_render(p_gl_rend_st);
        p_gl_rend_st.set_viewport(0, 0, fb_width[0], fb_height[0]);
    }

    // 0x1404AE680
    void Render::end_render(p_gl_rend_state& p_gl_rend_st) {
        fb_fbo[0].end_render(p_gl_rend_st);
    }

    // 0x1404A93B0
    void Render::begin_render_transparency(render_data_context& rend_data_ctx, RenderTexture* rt) {
        transparency->begin_render(rend_data_ctx, rt->get_texture_glid());
    }

    // 0x1404AE6A0
    void Render::end_render_transparency(render_data_context& rend_data_ctx, RenderTexture* rt, float_t alpha) {
        transparency->end_render(rend_data_ctx, rt, alpha);
    }

    // 0x1404B0860
    void Render::pre_proc(render_data_context& rend_data_ctx) {
        cam_data& cam = rctx_ptr->render_manager->cam;

        prev_cam_pos = cam_pos;
        prev_cam_intr = cam_intr;
        cam_pos = cam.get_view_point();
        cam_intr = cam.get_interest();

        prev_stage_num = stage_num;
        if (task_stage_is_modern)
            stage_num = (int32_t)task_stage_modern_get_current_stage_hash();
        else
            stage_num = task_stage_get_current_stage_index();

        prev_cam_view_proj = cam_view_proj;
        cam_view_proj = cam.get_view_proj_mat();

        reset_exposure = rctx_ptr->camera->fast_change_hist1 && !rctx_ptr->camera->fast_change_hist0;
        if (reset_exposure) {
            float_t pos_dist = vec3::distance(cam_pos, prev_cam_pos);

            vec3 dir = vec3::normalize(cam_intr - cam_pos);
            vec3 dir_prev = vec3::normalize(prev_cam_intr - prev_cam_pos);

            float_t dir_diff_angle = vec3::dot(dir, dir_prev);
            if (dir_diff_angle < 0.5f)
                dir_diff_angle = 0.0f;
            if (pos_dist < dir_diff_angle * 0.4f)
                reset_exposure = false;
        }
        else if (stage_num != prev_stage_num)
            reset_exposure = true;

        if (fb_movie_txhd[0]) {
            extern texture* task_movie_get_texture(int32_t index = 0);
            texture* tex = task_movie_get_texture();
            if (tex) {
                fb_movie_fbo[0].begin_render(rend_data_ctx.state);
                rend_data_ctx.state.active_bind_texture_2d(0, tex->glid);
                rend_data_ctx.state.bind_sampler(0, rctx_ptr->render_samplers[0]);
                rend_data_ctx.state.set_viewport(0, 0, fb_movie_txhd[0]->width, fb_movie_txhd[0]->height);
                rend_data_ctx.shader_flags.arr[U_REDUCE_TEX] = 0;
                shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_REDUCE);
                draw_quad(rend_data_ctx, tex->width, tex->height,
                    1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
                fb_movie_fbo[0].end_render(rend_data_ctx.state);
            }
        }

        for (int32_t i = 0; i < GL_REND_STATE_COUNT; i++) {
            render_data_context rend_data_ctx((gl_rend_state_index)i);
            rend_data_ctx.set_scene_framebuffer_size(fb_width[0], fb_height[0], fb_width[0], fb_height[0]);
        }
    }

    // 0x1404B0850
    void Render::post_proc() {
        flare_textures[0] = 0;
        composite_back = 0;
    }

    // 0x1404AD3A0
    void Render::draw_quad(render_data_context& rend_data_ctx,
        int32_t texel_w, int32_t texel_h, float_t s0, float_t t0, float_t s1, float_t t1,
        float_t scale, float_t param_x, float_t param_y, float_t param_z, float_t param_w) {
        s0 -= s1;
        t0 -= t1;

        const float_t w = (float_t)max_def(texel_w, 1);
        const float_t h = (float_t)max_def(texel_h, 1);
        quad_shader_data quad = {};
        quad.g_texcoord_modifier = { 0.5f * s0, 0.5f * t0, 0.5f * s0 + s1, 0.5f * t0 + t1 }; // x * 0.5 * y0 + 0.5 * y0 + y1
        quad.g_texel_size = { scale / w, scale / h, w, h };
        quad.g_color = { param_x, param_y, param_z, param_w };
        quad.g_texture_lod = 0.0f;

        rend_data_ctx.state.write_uniform_buffer(rctx_ptr->sun_quad_ubo, quad);
        rend_data_ctx.state.bind_uniform_buffer_base(0, rctx_ptr->sun_quad_ubo);
        rend_data_ctx.state.bind_vertex_array(rctx_ptr->common_vao);
        rend_data_ctx.state.draw_arrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    // 0x1404B0F50
    void Render::query_chara_exposure(render_data_context& rend_data_ctx, const cam_data& cam) {
        shader::unbind(rend_data_ctx.state);

        rend_data_ctx.state.set_color_mask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        rend_data_ctx.state.set_depth_mask(GL_FALSE);
        rend_data_ctx.state.disable_cull_face();

        shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_SUN_NO_TEXTURED);
        rend_data_ctx.state.bind_vertex_array(rctx_ptr->common_vao);
        rend_data_ctx.state.bind_uniform_buffer_base(0, rctx_ptr->sun_quad_ubo);

        ExposureCharaData* chara_data = exposure_chara_data;
        int32_t chara_write_index = (exposure_chara_write_index + 1) % num_queries;
        exposure_chara_write_index = chara_write_index;
        int32_t next_query_index = (chara_write_index + 2) % num_queries;
        for (int32_t i = 0; i < ROB_ID_MAX; i++, chara_data++) {
            rob_chara* rob_chr = get_rob_management()->get_rob((ROB_ID)i);
            if (!rob_chr || !rob_chr->get_disp())
                continue;

            float_t max_face_depth = rob_chr->get_face_depth();

            mat4 mat = mat4_identity;
            rob_chr->sub_1405163C0(4, mat);
            mat4_mul(&mat, &cam.get_view_mat(), &mat);
            mat4_mul_translate(&mat, max_face_depth + 0.1f, 0.0f, -0.06f, &mat);
            mat4_clear_rot(&mat, &mat);
            mat4_scale_rot(&mat, 0.0035f, &mat);
            mat4_mul(&mat, &cam.get_proj_mat(), &mat);

            sun_quad_shader_data shader_data = {};
            mat4_transpose(&mat, &mat);
            shader_data.g_transform[0] = mat.row0;
            shader_data.g_transform[1] = mat.row1;
            shader_data.g_transform[2] = mat.row2;
            shader_data.g_transform[3] = mat.row3;
            shader_data.g_emission = 0.0f;
            rend_data_ctx.state.write_uniform_buffer(rctx_ptr->sun_quad_ubo, shader_data);

            rend_data_ctx.state.begin_query(GL_SAMPLES_PASSED, chara_data->query[next_query_index]);
            rend_data_ctx.state.draw_arrays(GL_TRIANGLE_STRIP, 0, 4);
            rend_data_ctx.state.end_query(GL_SAMPLES_PASSED);

            if (chara_data->query_data[next_query_index] == -1)
                chara_data->query_data[next_query_index] = 0;

            if (chara_data->query_data[chara_write_index] != -1) {
                int32_t res = 0;
                glGetQueryObjectiv(chara_data->query[chara_write_index],
                    GL_QUERY_RESULT_AVAILABLE, &res);
                if (res)
                    glGetQueryObjectuiv(chara_data->query[chara_write_index],
                        GL_QUERY_RESULT, chara_data->query_data);
                else
                    chara_data->query_data[chara_write_index] = 0;
            }
        }

        rend_data_ctx.state.bind_vertex_array(0);

        rend_data_ctx.state.set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        rend_data_ctx.state.set_depth_mask(GL_TRUE);
        rend_data_ctx.state.enable_cull_face();
    }

    // 0x1404AE4B0
    void Render::draw_sun_request(const GLuint* flares, float_t scale, int32_t ghosts) {
        flare_textures[0] = 0;
        if (flares && flares[0] && flares[0] != -1) {
            flare_textures[0] = flares[0];
            flare_textures[1] = flares[1];
            flare_textures[2] = flares[2];
            num_ghosts = 16; //ghosts;
            flare_scale_default = scale;
        }
    }

    // 0x1404AD960
    void Render::draw_sun(render_data_context& rend_data_ctx, const cam_data& cam) {
        static float_t flt_1411ACB84 = sinf(0.01365909819f);
        static float_t flt_1411ACB8C = tanf(0.01365909819f) * 2.0f * 0.94f;

        if (!flare_textures[0])
            return;

        GLuint tex = get_objset_gen_textures_id(5, 4549);
        if (tex == -1)
            return;

        float_t v5 = tanf(cam.get_fov() * 0.5f);
        light_set* set = &rctx_ptr->light_set[LIGHT_SET_MAIN];
        light_data* data = &set->lights[LIGHT_SUN];

        vec3 position;
        vec4 emission;
        if (data->get_type() == LIGHT_PARALLEL) {
            data->get_position(position);
            data->get_diffuse(emission);
        }
        else {
            set->lights[LIGHT_STAGE].get_position(position);
            position = vec3::normalize(position) * 500.0f;
            set->lights[LIGHT_STAGE].get_ibl_color0(emission);
            data->set_type(LIGHT_PARALLEL);
            data->set_position(position);
            data->set_diffuse(emission);
        }

        vec4 v45;
        v45.x = position.x;
        v45.y = position.y;
        v45.z = position.z;
        v45.w = 1.0f;
        mat4_transform_vector(&cam.get_view_proj_mat(), &v45, &v45);

        float_t v13 = 1.0f / v45.w;
        float_t v14 = v45.x * v13;
        float_t v15 = v45.y * v13;
        float_t v16 = (float_t)scr_width * 0.01f / (float_t)scr_height;
        v14 = clamp_def(v14, -0.99f, 0.99f);
        v15 = max_def(v15, v16 - 1.0f);
        v15 = min_def(v15, 1.0f - v16);
        v45.x = v14 * v45.w;
        v45.y = v15 * v45.w;

        mat4 inv_view_proj_mat;
        mat4_invert(&cam.get_view_proj_mat(), &inv_view_proj_mat);
        mat4_transform_vector(&inv_view_proj_mat, &v45, &v45);

        calc_screen_pos_r(&cam.get_view_proj_mat(),
            rctx_ptr->camera->depth, &flare_pos, &position, 0.0f, false);

        float_t v17 = flare_pos.x - (float_t)scr_width * 0.5f;
        float_t v19 = v5 / ((float_t)scr_height * 0.5f);
        float_t v20 = flare_pos.y - (float_t)scr_height * 0.5f;

        float_t v22 = sqrtf((v17 * v19) * (v17 * v19) + (v20 * v19) * (v20 * v19) + 1.0f);
        float_t v23 = (v22 * v22 * 0.5f)
            * ((float_t)fb_width[0] * (flt_1411ACB8C / v5))
            * ((float_t)fb_height[0] * (flt_1411ACB8C / v5));

        float_t sun_radius = vec3::distance(position, cam_pos) * flt_1411ACB84;
        *(vec3*)&emission *= 1.0f / (1.0f - cosf((float_t)(3.0 * DEG_TO_RAD)));

        rend_data_ctx.state.active_bind_texture_2d(0, tex);
        shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_SUN);

        int32_t query_index = (query_read_index + 1) % num_queries;
        query_read_index = query_index;
        int32_t next_query_index = (query_index + 2) % num_queries;

        if (sun_pixels_center[next_query_index] == -1)
            sun_pixels_center[next_query_index] = 0;

        if (sun_pixels_sphere[next_query_index] == -1)
            sun_pixels_sphere[next_query_index] = 0;

        rend_data_ctx.state.set_color_mask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        rend_data_ctx.state.set_depth_mask(GL_FALSE);

        mat4 center_transform;
        get_sun_transform(center_transform, position, sun_radius * 0.2f, cam);
        rend_data_ctx.state.begin_query(GL_SAMPLES_PASSED, sun_queries_center[next_query_index]);
        draw_quad_sun(rend_data_ctx, center_transform, emission);
        rend_data_ctx.state.end_query(GL_SAMPLES_PASSED);

        mat4 sphere_transform;
        get_sun_transform(sphere_transform, *(vec3*)&v45, sun_radius * 2.0f, cam);
        rend_data_ctx.state.begin_query(GL_SAMPLES_PASSED, sun_queries_sphere[next_query_index]);
        draw_quad_sun(rend_data_ctx, sphere_transform, emission);
        rend_data_ctx.state.end_query(GL_SAMPLES_PASSED);

        rend_data_ctx.state.set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        rend_data_ctx.state.set_depth_mask(GL_TRUE);

        if (sun_pixels_center[query_index] != -1) {
            int32_t res = 0;
            glGetQueryObjectiv(sun_queries_center[query_index], GL_QUERY_RESULT_AVAILABLE, &res);
            if (res)
                glGetQueryObjectuiv(sun_queries_center[query_index], GL_QUERY_RESULT, &sun_pixels_center[query_index]);
        }

        if (sun_pixels_sphere[query_index] != -1) {
            int32_t res = 0;
            glGetQueryObjectiv(sun_queries_sphere[query_index], GL_QUERY_RESULT_AVAILABLE, &res);
            if (res)
                glGetQueryObjectuiv(sun_queries_sphere[query_index], GL_QUERY_RESULT, &sun_pixels_sphere[query_index]);
        }

        if (emission.x + emission.y + emission.z > 0.0f) {
            rend_data_ctx.state.enable_blend();
            rend_data_ctx.state.set_blend_func(GL_ONE, GL_ONE);
            rend_data_ctx.state.set_depth_mask(GL_FALSE);

            mat4 sun_transform;
            get_sun_transform(sun_transform, position, sun_radius * 1.1f, cam);
            draw_quad_sun(rend_data_ctx, sun_transform, emission);

            rend_data_ctx.state.disable_blend();
            rend_data_ctx.state.set_blend_func(GL_ONE, GL_ZERO);
            rend_data_ctx.state.set_depth_mask(GL_TRUE);
        }

        rend_data_ctx.state.active_bind_texture_2d(0, 0);
        rend_data_ctx.state.bind_vertex_array(0);

        if (flare_alpha_offset <= 0.01f)
            flare_alpha_offset = 0.0f;
        else
            flare_alpha_offset *= 0.93f;

        flare_scale = 100.0f;
        flare_alpha = 0.0f;
        float_t flare_pixels = 0.0f;
        if (flare_pos.z >= 0.0f
            || flare_pos.x < (float_t)-scr_width || flare_pos.x > (float_t)(scr_width * 2)
            || flare_pos.y < (float_t)-scr_height || flare_pos.y > (float_t)(scr_height * 2))
            flare_textures[0] = 0;
        else {
            for (GLuint& i : sun_pixels_sphere)
                if (i != -1)
                    flare_pixels += min_def((float_t)i, v23);
            flare_pixels /= v23 * 3.0f;
            flare_alpha = powf(flare_pixels, 0.7f);

            if (flare_pixels > 0.0f && sun_pixels_center[query_index]) {
                float_t v42 = max_def(flare_pixels - 0.4f, 0.005f);
                flare_scale = 0.6f / v42 * flare_scale_default * (v5 * 3.4f);
            }
        }

        if (flare_pos.z >= 0.0f
            || flare_pos.x < (float_t)-12 || flare_pos.x > (float_t)(scr_width + 12)
            || flare_pos.y < (float_t)-12 || flare_pos.y > (float_t)(scr_height + 12))
            flare_alpha_enable = 0.0f;
        else if (flare_pos.x > (float_t)-2 && flare_pos.x < (float_t)(scr_width + 2)
            && flare_pos.y > (float_t)-2 && flare_pos.y < (float_t)(scr_height + 2)) {
            if (flare_pixels > 0.4f && flare_alpha_enable == 0.0f
                && flare_alpha_offset < 0.02f && !reset_exposure)
                flare_alpha_offset = 8.0f;
            flare_alpha_enable = 1.0f;
        }
    }

    // 0x1404ACF80
    void Render::draw_ghost(render_data_context& rend_data_ctx) {
        static const float_t pos_scale_array[16] = {
            -0.70f, -0.30f,  0.35f,  0.50f,
            -0.45f, -0.80f,  0.20f,  0.41f,
            -0.17f, -0.10f,  0.06f,  0.10f,
             0.14f,  0.04f, -0.13f, -0.22f,
        };

        static const float_t opacity_array[16] = {
            0.8f, 1.0f, 1.0f, 1.0f,
            0.4f, 0.5f, 0.8f, 0.8f,
            0.6f, 0.7f, 0.8f, 0.7f,
            0.8f, 0.7f, 0.6f, 0.8f,
        };

        static const float_t scale_array[16] = {
             1.3f, 1.5f, 1.00f, 1.1f,
             2.5f, 0.8f, 0.50f, 0.5f,
             0.7f, 0.4f, 0.35f, 0.5f,
             0.4f, 0.3f, 0.60f, 0.4f,
        };

        if (!flare_textures[0] || num_ghosts <= 0)
            return;

        if (num_ghosts > 16)
            num_ghosts = 16;

        const float_t aspect = (float_t)fb_width[0] / (float_t)fb_height[0];

        const float_t v7 = (flare_pos.x - (float_t)(scr_width / 2)) / (float_t)scr_width;
        const float_t v8 = -((flare_pos.y - (float_t)(scr_height / 2)) / (float_t)scr_height);
        const float_t v9 = (float_t)((1.1 - sqrtf(v8 * v8 + v7 * v7)) * flare_alpha);
        if (v9 < 0.001f)
            return;

        const float_t v9a = v9 * v9;

        float_t angle = atan2f(v8, v7) - (float_t)M_PI_2;
        const float_t angle_sin = sinf(angle);
        const float_t angle_cos = cosf(angle);

        float_t* data = (float_t*)rend_data_ctx.state.map_array_buffer(rctx_ptr->lens_ghost_vbo);
        if (!data)
            return;

        const float_t lens_ghost = flare_coef[2];
        const int32_t num_ghosts = this->num_ghosts;
        for (int32_t i = 0; i < num_ghosts; i++) {
            float_t opacity = v9 * opacity_array[i] * lens_ghost;

            float_t scale = (v9a * 0.03f + 0.02f) * scale_array[i];

            mat4 mat;
            mat4_translate(pos_scale_array[i] * v7 + 0.5f, pos_scale_array[i] * v8 + 0.5f, 0.0f, &mat);
            mat4_scale_rot(&mat, scale, scale * aspect, 1.0f, &mat);
            mat4_mul_rotate_z(&mat, angle_sin, angle_cos, &mat);
            draw_quad_ghost(i & 0x03, opacity, &mat, data);
        }

        rend_data_ctx.state.unmap_array_buffer(rctx_ptr->lens_ghost_vbo);

        rend_data_ctx.state.set_viewport(0, 0, fb_width[0], fb_height[0]);
        fb_fbo[0].begin_render(rend_data_ctx.state);
        rend_data_ctx.state.enable_blend();
        rend_data_ctx.state.set_blend_func(GL_ONE, GL_ONE);

        rend_data_ctx.shader_flags.arr[U_REDUCE_TEX] = 4;
        shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_REDUCE);
        rend_data_ctx.state.active_bind_texture_2d(0, flare_textures[2]);
        rend_data_ctx.state.bind_sampler(0, rctx_ptr->render_samplers[0]);
        rend_data_ctx.state.bind_vertex_array(rctx_ptr->lens_ghost_vao);
        rend_data_ctx.state.draw_arrays(GL_TRIANGLES, 0, (GLsizei)(num_ghosts * 6LL));
        rend_data_ctx.state.bind_vertex_array(0);

        rend_data_ctx.state.disable_blend();
        rend_data_ctx.state.set_blend_func(GL_ONE, GL_ZERO);
    }

    // 0x1404AC270
    void Render::draw_flare(render_data_context& rend_data_ctx,
        const cam_data& cam, texture* light_proj_tex, int32_t npr_param) {
        render_context* rctx = rctx_ptr;

        for (int32_t i = 0; i < 8; i++)
            rend_data_ctx.state.bind_sampler(i, 0);

        dof->apply(rend_data_ctx, &fb_fbo[0], &rctx_ptr->render_buffer);

        draw_ghost(rend_data_ctx);

        reduce_texture(rend_data_ctx);

        bloom_tex = 0;
        handle_blur_texture(rend_data_ctx);

        measure_exposure(rend_data_ctx, cam);
        tone_map(rend_data_ctx, light_proj_tex, npr_param);

        mlaa(rend_data_ctx, scr_index, num_scr_tex - 1, ss_alpha_mask);

        for (int32_t i = 0; i < num_fb_copy; i++) {
            if (!fb_copy_txhd[i])
                continue;

            fb_copy_fbo[i].begin_render(rend_data_ctx.state);

            const texture* t = fb_copy_txhd[i];
            if (fb_width[0] > t->width * 2 || fb_height[0] > t->height * 2)
                rend_data_ctx.shader_flags.arr[U_REDUCE_TEX] = 1;
            else
                rend_data_ctx.shader_flags.arr[U_REDUCE_TEX] = 0;

            rend_data_ctx.state.set_viewport(0, 0, t->width, t->height);
            rend_data_ctx.state.active_bind_texture_2d(0, scr_txhd[scr_index]->glid);
            rend_data_ctx.state.bind_sampler(0, rctx->render_samplers[0]);
            shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_REDUCE);
            draw_quad(rend_data_ctx, fb_tex_width[0], fb_tex_height[0],
                fb_tex_width_scale, fb_tex_height_scale,
                0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        }

        process_capture(rend_data_ctx, fb_fbo[0].get_texture_glid(),
            fb_width[0], fb_height[0], scr_txhd[scr_index]->glid);

        if (temporal_anti_alias) {
            if (scr_tex_rate >= 0.0f)
                calc_scr_tex_rate();
            else
                scr_tex_rate = 1.0f;
        }

        if (scr_tex_rate < 0.0f || scr_tex_rate >= 1.0f)
            scr_disp_index = scr_index;
        else {
            rend_data_ctx.state.begin_event("taa_or_blur");
            bool blur;
            GLuint sampler;
            if (scr_tex_rate > 0.99f) {
                blur = true;
                sampler = rctx->render_samplers[0];
            }
            else {
                blur = false;
                sampler = rctx->render_samplers[1];
            }

            scr_disp_index = num_scr_tex - 1;
            scr_fbo[num_scr_tex - 1].begin_render(rend_data_ctx.state);
            rend_data_ctx.state.set_viewport(0, 0, fb_width[0], fb_height[0]);
            rend_data_ctx.state.active_bind_texture_2d(0, scr_txhd[scr_index]->glid);
            rend_data_ctx.state.bind_sampler(0, sampler);

            if (blur) {
                rend_data_ctx.state.active_bind_texture_2d(2, fb_fbo[0].get_depth_texture()->glid);
                rend_data_ctx.state.bind_sampler(2, sampler);

                mat4 mat;
                mat4_invert(&cam_view_proj, &mat);
                mat4_mul(&prev_cam_view_proj, &mat, &mat);
                rend_data_ctx.shader_flags.arr[U_REDUCE_TEX] = 6;

                camera_blur_shader_data shader_data = {};
                shader_data.g_transform[0] = mat.row0;
                shader_data.g_transform[1] = mat.row1;
                shader_data.g_transform[2] = mat.row2;
                shader_data.g_transform[3] = mat.row3;
                rend_data_ctx.state.write_uniform_buffer(rctx->camera_blur_ubo, shader_data);
                rend_data_ctx.state.bind_uniform_buffer_base(1, rctx->camera_blur_ubo);
            }
            else {
                int32_t scr_index = this->scr_index + 1;
                if (scr_index > temporal_anti_alias)
                    scr_index = 0;
                rend_data_ctx.state.active_bind_texture_2d(1, scr_txhd[scr_index]->glid);
                rend_data_ctx.state.bind_sampler(1, sampler);
                rend_data_ctx.shader_flags.arr[U_REDUCE_TEX] = 5;
            }

            shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_REDUCE);

            draw_quad(rend_data_ctx, fb_tex_width[0], fb_tex_height[0],
                fb_tex_width_scale, fb_tex_height_scale,
                0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, scr_tex_rate);
            rend_data_ctx.state.end_event();
        }

        if (temporal_anti_alias) {
            int32_t scr_index = this->scr_index + 1;
            if (scr_index > temporal_anti_alias)
                scr_index = 0;
            this->scr_index = scr_index;
        }

        rctx->screen_buffer.begin_render(rend_data_ctx.state);
        rend_data_ctx.state.set_viewport(0, 0, rctx->view_w, rctx->view_h);
        if (anti_alias) {
            rend_data_ctx.state.active_bind_texture_2d(0, scr_txhd[scr_disp_index]->glid);
            rend_data_ctx.state.bind_sampler(0, rctx->render_samplers[0]);
            rend_data_ctx.shader_flags.arr[U_ALPHA_MASK] = ss_alpha_mask ? 1 : 0;
            rend_data_ctx.shader_flags.arr[U_REDUCE_TEX] = 0;
            shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_REDUCE);
            draw_quad(rend_data_ctx, fb_tex_width[0], fb_tex_height[0],
                fb_tex_width_scale, fb_tex_height_scale,
                0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
            rend_data_ctx.shader_flags.arr[U_ALPHA_MASK] = 0;
        }
        else {
            rend_data_ctx.state.active_bind_texture_2d(0, scr_txhd[scr_disp_index]->glid);
            if (mag_filter == MAG_FILTER_NEAREST)
                rend_data_ctx.state.bind_sampler(0, rctx->render_samplers[1]);
            else
                rend_data_ctx.state.bind_sampler(0, rctx->render_samplers[0]);

            switch (mag_filter) {
            case MAG_FILTER_NEAREST:
            case MAG_FILTER_BILINEAR:
            default:
                rend_data_ctx.shader_flags.arr[U_MAGNIFY] = 0;
                break;
            case MAG_FILTER_SHARPEN_5_TAP:
                rend_data_ctx.shader_flags.arr[U_MAGNIFY] = 2;
                break;
            case MAG_FILTER_SHARPEN_4_TAP:
                rend_data_ctx.shader_flags.arr[U_MAGNIFY] = 3;
                break;
            case MAG_FILTER_CONE_4_TAP:
                rend_data_ctx.shader_flags.arr[U_MAGNIFY] = 4;
                break;
            case MAG_FILTER_CONE_2_TAP:
                rend_data_ctx.shader_flags.arr[U_MAGNIFY] = 5;
                break;
            }

            shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_MAGNIFY);
            draw_quad(rend_data_ctx, fb_tex_width[0], fb_tex_height[0],
                fb_tex_width_scale, fb_tex_height_scale,
                0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        }
        shader::unbind(rend_data_ctx.state);

        for (int32_t i = 0; i < 8; i++)
            rend_data_ctx.state.bind_sampler(i, 0);
    }

    // 0x1404ABDA0
    void Render::draw_contour(render_data_context& rend_data_ctx, const cam_data& cam) {
        render_context* rctx = rctx_ptr;

        if (reflect_draw)
            rctx->reflect_buffer.begin_render(rend_data_ctx.state);
        else
            contour_fbo->begin_render(rend_data_ctx.state);

        rend_data_ctx.state.enable_depth_test();
        rend_data_ctx.state.set_depth_func(GL_ALWAYS);
        rend_data_ctx.state.set_depth_mask(GL_TRUE);
        if (reflect_draw) {
            RenderTexture& refl_tex = rctx->render_manager->get_render_texture(0);
            rend_data_ctx.state.set_viewport(0, 0, refl_tex.get_width(), refl_tex.get_height());
            rend_data_ctx.state.active_bind_texture_2d(0, refl_tex.get_texture_glid());
            rend_data_ctx.state.bind_sampler(0, rctx->render_samplers[1]);
            rend_data_ctx.state.active_bind_texture_2d(1, refl_tex.get_depth_texture_glid());
            rend_data_ctx.state.bind_sampler(1, rctx->render_samplers[1]);
        }
        else {
            rend_data_ctx.state.set_viewport(0, 0, fb_width[0], fb_height[0]);
            rend_data_ctx.state.active_bind_texture_2d(0, fb_fbo[0].get_texture_glid());
            rend_data_ctx.state.bind_sampler(0, rctx->render_samplers[1]);
            rend_data_ctx.state.active_bind_texture_2d(1, fb_fbo[0].get_depth_texture_glid());
            rend_data_ctx.state.bind_sampler(1, rctx->render_samplers[1]);
        }
        rend_data_ctx.state.active_texture(0);

        float_t v3 = 1.0f / tanf(cam.get_fov() * 0.5f);

        vec3 direction = cam.get_interest() - cam.get_view_point();
        float_t length = vec3::length(direction);
        float_t v7 = direction.y;
        if (length != 0.0f)
            v7 /= length;

        float_t v9 = fabsf(v7) - 0.1f;
        if (v9 < 0.0f)
            v9 = 0.0f;

        contour_coef_shader_data shader_data = {};
        shader_data.g_contour = { v9 * 0.004f + 0.0027f, 0.003f, v3 * 0.35f, 0.0008f };
        const double_t max_distance = cam.get_max_distance();
        const double_t min_distance = cam.get_min_distance();
        shader_data.g_near_far = {
            (float_t)(max_distance * (1.0 / (max_distance - min_distance))),
            (float_t)(-(max_distance * min_distance) * (1.0 / (max_distance - min_distance))),
            (float_t)min_distance, (float_t)max_distance
        };
        rend_data_ctx.state.write_uniform_buffer(rctx->contour_coef_ubo, shader_data);

        shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_CONTOUR);
        rend_data_ctx.state.bind_uniform_buffer_base(2, rctx->contour_coef_ubo);

        if (reflect_draw) {
            RenderTexture& refl_tex = rctx->render_manager->get_render_texture(0);
            draw_quad(rend_data_ctx, refl_tex.get_width(), refl_tex.get_height(),
                1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        }
        else
            draw_quad(rend_data_ctx,
                fb_tex_width[0], fb_tex_height[0],
                fb_tex_width_scale, fb_tex_height_scale,
                0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    }

    // 0x1404B1560
    int32_t Render::register_fb_copy(const texture* txhd, bool task_photo) {
        if (!txhd)
            return -1;

        int32_t index = 0;
        if (task_photo)
            index = num_fb_copy - 1;
        else
            while (fb_copy_txhd[index])
                if (++index >= num_fb_copy - 1)
                    return -1;

        fb_copy_txhd[index] = txhd;
        fb_copy_fbo[index].attach_texture(txhd->glid);
        return index;
    }

    // 0x1404B1880
    void Render::remove_fb_copy(const texture* txhd, bool task_photo) {
        if (!txhd)
            return;

        int32_t index = 0;
        if (task_photo)
            index = num_fb_copy - 1;
        else
            while (fb_copy_txhd[index] != txhd)
                if (++index >= num_fb_copy - 1)
                    return;

        fb_copy_txhd[index] = 0;
        fb_copy_fbo[index].destroy();
    }

    // 0x1404B16A0
    int32_t Render::register_fb_movie(const texture* txhd) {
        if (!txhd)
            return -1;

        int32_t index = 0;
        while (fb_movie_txhd[index])
            if (++index >= num_fb_movie)
                return -1;

        fb_movie_txhd[index] = txhd;
        fb_movie_fbo[index].attach_texture(txhd->glid);
        return index;
    }

    // 0x1404B18F0
    void Render::remove_fb_movie(const texture* txhd) {
        if (!txhd)
            return;

        int32_t index = 0;
        while (fb_movie_txhd[index] != txhd)
            if (++index >= num_fb_movie)
                return;

        fb_movie_txhd[index] = 0;
        fb_movie_fbo[index].destroy();
    }

    // 0x1404B05C0
    void Render::perspective(vec2& offset, float_t scale) const {
        if (temporal_anti_alias) {
            const float_t taa_offset = scr_index == 1 ? -0.25f : 0.25f;
            const float_t _offset = taa_offset * scale * -2.0f;
            offset = _offset / vec2((float_t)fb_width[0], (float_t)fb_height[0]);
        }
        else
            offset = 0.0f;
    }

    // 0x1404AF200
    void Render::init_capture() {
        for (Render::Capture& i : capture) {
            for (Render::CaptureData& j : i.data) {
                j.txhd = 0;
                j.type = CAPTURE_MAX;
            }

            i.capture = false;
        }
    }

    // 0x1404AE6C0
    void Render::free_capture() {
        for (Render::Capture& i : capture) {
            for (Render::CaptureData& j : i.data) {
                j.txhd = 0;
                j.fbo.destroy();
                j.type = CAPTURE_MAX;
            }

            i.capture = false;
        }
    }

    // 0x1404B1600
    int32_t Render::register_fb_man_cap(CaptureSlot slot, Render::CaptureType type, texture* txhd) {
        if (!txhd || slot < 0 || slot >= CAPTURE_SLOT_MAX)
            return -1;

        for (Render::CaptureData& i : capture[slot].data)
            if (!i.txhd) {
                i.fbo.attach_texture(txhd->glid);
                i.txhd = txhd;
                i.type = type;
                return (int32_t)(&i - capture[slot].data);
            }
        return -1;
    }

    // 0x1404B30A0
    bool Render::unregister_fb_man_cap(CaptureSlot slot, texture* txhd) {
        if (!txhd || slot < 0 || slot >= CAPTURE_SLOT_MAX)
            return false;

        for (Render::CaptureData& i : capture[slot].data)
            if (i.txhd == txhd) {
                i.fbo.destroy();
                i.txhd = 0;
                i.type = CAPTURE_MAX;
                return true;
            }
        return false;
    }

    // 0x1404AF260
    bool Render::is_fb_man_cap() {
        for (int32_t i = CAPTURE_SLOT_A; i < CAPTURE_SLOT_MAX; i++)
            if (capture[i].capture)
                return true;
        return false;
    }

    // 0x1404B1DA0
    bool Render::set_frame_texture_aoto_cap(bool value) {
        capture[0].capture = value;
        return true;
    }

    // 0x1404A9C00
    bool Render::capture_slot_enable(CaptureSlot slot) {
        if (slot < CAPTURE_SLOT_A || slot >= CAPTURE_SLOT_MAX)
            return false;

        capture[slot].capture = true;
        return true;
    }

    // 0x1404B0D40
    void Render::process_capture(render_data_context& rend_data_ctx,
        GLuint pre_pp_tex, int32_t wight, int32_t height, GLuint post_pp_tex) {
        for (Render::Capture& i : capture) {
            if (!i.capture)
                continue;

            for (auto& j : i.data) {
                if (!j.txhd || j.fbo.begin_render(rend_data_ctx.state) < 0)
                    continue;

                texture* txhd = j.txhd;

                rend_data_ctx.state.active_bind_texture_2d(0, j.type == CAPTURE_PRE_PP ? pre_pp_tex : post_pp_tex);
                rend_data_ctx.state.bind_sampler(0, rctx_ptr->render_samplers[0]);
                rend_data_ctx.state.set_viewport(0, 0, txhd->width, txhd->height);
                rend_data_ctx.shader_flags.arr[U_REDUCE_TEX] = 0;
                shaders_ft.set(rend_data_ctx.state, rend_data_ctx.shader_flags, SHADER_FT_REDUCE);
                draw_quad(rend_data_ctx, txhd->width, txhd->height,
                    1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
                j.fbo.end_render(rend_data_ctx.state);
            }

            if (&i - capture)
                i.capture = false;
        }
    }

    // 0x1404B23C0
    void Render::set_tone_map_method(int32_t value) {
        tone_map_method = (ToneMapMethod)value;
        update = 1;
    }

    // 0x1404B1BC0
    void Render::set_exposure(float_t value) {
        exposure = value;
        update = 1;
    }

    // Missing
    void Render::set_exposure_rate(float_t value) {
        exposure_rate = value;
        update = 1;
    }

    // 0x1404B1B10
    void Render::set_auto_exposure(int32_t value) {
        auto_exposure = value;
        update = 1;
    }

    // 0x1404B1DB0
    void Render::set_gamma(float_t value) {
        gamma = value;
        recalc_tone_map = 1;
        update = 1;
    }

    // Missing
    void Render::set_gamma_rate(float_t value) {
        gamma_rate = value;
        recalc_tone_map = 1;
        update = 1;
    }

    // 0x1404B2350
    void Render::set_saturate_power(int32_t value) {
        saturate_power = value;
        recalc_tone_map = 1;
        update = 1;
    }

    // 0x1404B22B0
    void Render::set_saturate_coef(float_t value, int32_t index, bool lock) {
        if (saturate_lock)
            return;

        saturate_coef[index] = value;
        recalc_tone_map = 1;
        update = 1;

        if (lock) {
            saturate_lock = 1;
            saturate_index = 0;
        }
        else if (index == 1)
            saturate_index = 1;
    }

    // 0x1404B2310
    void Render::set_saturate_coef_default(int32_t index, bool lock) {
        set_saturate_coef(1.0f, index, lock);
        if (index == 1)
            saturate_index = 0;
    }

    // 0x1404B21F0
    void Render::set_mag_filter(int32_t value) {
        mag_filter = (MagFilterType)value;
        update = 1;
    }

    // 0x1404B23A0
    void Render::set_temporal_aa(int32_t value) {
        temporal_anti_alias = value;
        scr_tex_rate = -1.0f;
        update = 1;
    }

    // 0x1404B2210
    void Render::set_morphological_aa(int32_t value) {
        morphological_anti_alias = value;
        update = 1;
    }

    // 0x1404B1B30
    void Render::set_cam_blur(int32_t value) {
        cam_blur = value;
        update = 1;
    }

    // 0x1404B1C10
    void Render::set_fade_color(const vec4* in_value, int32_t index) {
        fade_color[index][0] = in_value->x;
        fade_color[index][1] = in_value->y;
        fade_color[index][2] = in_value->z;
        fade_rate[index] = in_value->w;

        update = 1;
        if (index == 1)
            fade_index = 1;
    }

    // 0x1404B1C60
    void Render::set_fade_color(const float_t* value, int32_t index) {
        fade_color[index][0] = value[0];
        fade_color[index][1] = value[1];
        fade_color[index][2] = value[2];

        update = 1;
        if (index == 1)
            fade_index = 1;
    }

    // 0x1404B1D00
    void Render::set_fade_rate(float_t value, int32_t index) {
        fade_rate[index] = value;

        update = 1;
        if (index == 1)
            fade_index = 1;
    }

    // 0x1404B1CB0
    void Render::set_fade_color_default(int32_t index) {
        fade_color[index][0] = 0.0f;
        fade_color[index][1] = 0.0f;
        fade_color[index][2] = 0.0f;
        fade_rate[index] = 0.0f;
        fade_blend_func[index] = 0;

        update = 1;
        if (index == 1)
            fade_index = 0;
    }

    // 0x1404B1BE0
    void Render::set_fade_blend_func(int32_t value, int32_t index) {
        if (value < 0 || value > 2)
            return;

        fade_blend_func[index] = value;

        update = 1;
        if (index == 1)
            fade_index = 1;
    }

    // 0x1404B2690
    void Render::set_tone_trans(const float_t* start, const float_t* end, int32_t index) {
        for (int32_t i = 0; i < 3; i++) {
            tone_trans_start[index][i] = start[i];
            tone_trans_end[index][i] = end[i];

            float_t range = end[i] - start[i];
            if (fabsf(range) < 0.0001f) {
                set_tone_trans_default(0);
                return;
            }

            float_t scale = 1.0f / range;
            tone_trans_scale[index][i] = 1.0f;
            tone_trans_offset[index][i] = -(scale * start[i]);
        }

        update = 1;
        if (index == 1)
            tone_trans_index = 1;
    }

    // 0x1404B2780
    void Render::set_tone_trans_default(int32_t index) {
        for (int32_t i = 0; i < 3; i++) {
            tone_trans_scale[index][i] = 1.0f;
            tone_trans_offset[index][i] = 0.0f;
            tone_trans_start[index][i] = 0.0f;
            tone_trans_end[index][i] = 1.0f;
        }

        update = 1;
        if (index == 1)
            tone_trans_index = 0;
    }

    // 0x1404B1D30
    void Render::set_fade_tex_rate(float_t value) {
        fade_tex_rate = value;
        update = 1;
    }

    // 0x1404B1D50
    void Render::set_fade_tex_type(int32_t value) {
        if (value < 0 || value > 5)
            value = 0;

        fade_tex_type = value;
        update = 1;
    }

    // 0x1404B1D70
    void Render::set_flare_coef(const float_t* value) {
        flare_coef[0] = value[0];
        flare_coef[1] = value[1];
        flare_coef[2] = value[2];
        update = 1;
    }

    // 0x1404B2370
    void Render::set_sigma(const float_t* value) {
        sigma[0] = value[0];
        sigma[1] = value[1];
        sigma[2] = value[2];
        update = 1;
    }

    // 0x1404B21C0
    void Render::set_intensity(const float_t* value) {
        filter_inten[0] = value[0];
        filter_inten[1] = value[1];
        filter_inten[2] = value[2];
        update = 1;
    }

    // 0x1404B27E0
    void Render::set_viewport(int32_t x, int32_t y, int32_t w, int32_t h) {
        view_x = x;
        view_y = y;
        view_w = min_def(w, scr_width);
        view_h = min_def(h, scr_height);

        calc_draw_size(false, -1);
        update = 1;
    }

    // 0x1404AEA90
    void Render::get_tone_map_method(int32_t* value) const {
        *value = tone_map_method;
    }

    // 0x1404AE7D0
    void Render::get_exposure(float_t* value) const {
        *value = exposure;
    }

    // Missing
    void Render::get_exposure_rate(float_t* value) const {
        *value = exposure_rate;
    }

    // 0x1404AE750
    void Render::get_auto_exposure(int32_t* value) const {
        *value = auto_exposure;
    }

    // 0x1404AE870
    void Render::get_gamma(float_t* value) const {
        *value = gamma;
    }

    // Missing
    void Render::get_gamma_rate(float_t* value) const {
        *value = gamma_rate;
    }

    // 0x1404AEA50
    void Render::get_saturate_power(int32_t* value) const {
        *value = saturate_power;
    }

    // 0x1404AEA40
    void Render::get_saturate_coef(float_t* value) const {
        *value = saturate_coef[0];
    }

    // 0x1404AE9B0
    void Render::get_mag_filter(int32_t* value) const {
        *value = mag_filter;
    }

    // 0x1404AEA80
    void Render::get_temporal_aa(int32_t* value) const {
        *value = temporal_anti_alias;
    }

    // 0x1404AE9C0
    void Render::get_morphological_aa(int32_t* value) const {
        *value = morphological_anti_alias;
    }

    // 0x1404AE760
    void Render::get_cam_blur(int32_t* value) const {
        *value = cam_blur;
    }

    // 0x1404AE7F0
    void Render::get_fade_color(vec4* value) const {
        value->x = fade_color[0][0];
        value->y = fade_color[0][1];
        value->z = fade_color[0][2];
        value->w = fade_rate[0];
    }

    // 0x1404AE820
    void Render::get_fade_color(float_t* value) const {
        value[0] = fade_color[0][0];
        value[1] = fade_color[0][1];
        value[2] = fade_color[0][2];
    }

    // 0x1404AE840
    void Render::get_fade_rate(float_t* value) const {
        *value = fade_rate[0];
    }

    // 0x1404AE7E0
    void Render::get_fade_blend_func(int32_t* value) const {
        *value = fade_blend_func[0];
    }

    // 0x1404AEAA0
    void Render::get_tone_trans(float_t* start_arg, float_t* end_arg) const {
        start_arg[0] = tone_trans_start[0][0];
        end_arg[0] = tone_trans_end[0][0];
        start_arg[1] = tone_trans_start[0][1];
        end_arg[1] = tone_trans_end[0][1];
        start_arg[2] = tone_trans_start[0][2];
        end_arg[2] = tone_trans_end[0][2];
    }

    // 0x1404AE850
    void Render::get_flare_coef(float_t* value) const {
        value[0] = flare_coef[0];
        value[1] = flare_coef[1];
        value[2] = flare_coef[2];
    }

    // 0x1404AEA60
    void Render::get_sigma(float_t* value) const {
        value[0] = sigma[0];
        value[1] = sigma[1];
        value[2] = sigma[2];
    }

    // 0x1404AE990
    void Render::get_intensity(float_t* value) const {
        value[0] = filter_inten[0];
        value[1] = filter_inten[1];
        value[2] = filter_inten[2];
    }

    // 0x1404AE9E0 
    void Render::get_render_param(float_t* fb_width_arg, float_t* fb_height_arg,
        float_t* fb_tex_width_arg, float_t* fb_tex_height_arg) const {
        if (fb_width_arg)
            *fb_width_arg = (float_t)fb_width[0];
        if (fb_height_arg)
            *fb_height_arg = (float_t)fb_height[0];
        if (fb_tex_width_arg)
            *fb_tex_width_arg = (float_t)fb_tex_width[0];
        if (fb_tex_height_arg)
            *fb_tex_height_arg = (float_t)fb_tex_height[0];
    }

    // 0x1404AE530
    void Render::enable_dof_set(bool value) {
        dof_pv_data.enable = value;
    }

    // Added
    void Render::update_dof_set(bool value) {
        dof_pv_data.update = value;
    }

    // Inlined
    bool Render::get_dof_enable() {
        return dof_pv_data.enable;
    }

    // Inlined, Added
    bool Render::get_dof_update() {
        return dof_pv_data.update;
    }

    // 0x1404AE780
    void Render::get_dof_data(float_t& focus, float_t& focus_range, float_t& fuzzing_range, float_t& ratio) {
        focus = dof_pv_data.f2.focus;
        focus_range = dof_pv_data.f2.focus_range;
        fuzzing_range = dof_pv_data.f2.fuzzing_range;
        ratio = dof_pv_data.f2.ratio;
    }

    // 0x1404B1B40
    void Render::set_dof_data(float_t focus, float_t focus_range, float_t fuzzing_range, float_t ratio) {
        dof_pv_data.f2.focus = focus;
        dof_pv_data.f2.focus_range = focus_range;
        dof_pv_data.f2.ratio = ratio;
        dof_pv_data.f2.fuzzing_range = fuzzing_range;
    }

    // Added
    void Render::resize(int32_t width, int32_t height) {
        scr_width = width;
        scr_height = height;

        if (anti_alias) {
            fb_width_org = 2 * width;
            fb_height_org = 2 * height;
        }
        else {
            fb_width_org = width;
            fb_height_org = height;
        }

        view_x = 0;
        view_y = 0;

        view_w = width;
        view_h = height;

        calc_draw_size(true, 1);

        fb_fbo[0].create_texture(fb_tex_width[0], fb_tex_height[0], 0, GL_RGBA16F, GL_DEPTH_COMPONENT24);

        gl_state.bind_texture_2d(fb_fbo[0].get_texture_glid());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        gl_state.bind_texture_2d(fb_fbo[0].get_depth_texture_glid());
        GLint swizzle[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);

        for (int32_t i = 0; i < num_scr_tex; i++) {
            texture_id id = scr_txhd[i]->id;
            texture_release(scr_txhd[i]);
            scr_txhd[i] = texture_load_tex_2d(id, GL_RGBA8,
                fb_tex_width[0], fb_tex_height[0], 0, 0, 0);
            scr_fbo[i].attach_texture(scr_txhd[i]->glid, 0, fb_fbo[0].get_depth_texture_glid());
        }

        texture_id composite_back_tex_id = composite_back_txhd->id;
        texture_release(composite_back_txhd);
        composite_back_txhd = texture_load_tex_2d(composite_back_tex_id, GL_RGBA8,
            fb_tex_width[0], fb_tex_height[0], 0, 0, 0);
        composite_back_fbo.attach_texture(composite_back_txhd->glid, 0, fb_fbo[0].get_depth_texture_glid());

        mlaa_fbo.create_texture(fb_tex_width[0], fb_tex_height[0], 0, GL_RGBA8, GL_DEPTH_COMPONENT24);
        user_fbo.create_texture(fb_tex_width[0], fb_tex_height[0], 0, GL_RGBA8, GL_ZERO);
        contour_fbo = &mlaa_fbo;

        gl_state.bind_texture_2d(mlaa_fbo.get_depth_texture_glid());
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);

        for (int32_t i = 1; i < fb_level; i++) {
            fb_fbo[i].create_texture(fb_tex_width[i], fb_tex_height[i], 0, GL_RGBA16F, GL_ZERO);

            gl_state.bind_texture_2d(fb_fbo[i].get_texture_glid());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }

        dof->resize(fb_tex_width[0], fb_tex_height[0]);

        for (int32_t i = 0; i < 1; i++) {
            texture_id id = transparency_tex[i]->id;
            texture_release(transparency_tex[i]);
            transparency_tex[i] = texture_load_tex_2d(id, GL_RGBA16F,
                fb_tex_width[0], fb_tex_height[0], 0, 0, 0);
        }

        transparency->resize(transparency_tex[0]->glid, fb_fbo[0].get_depth_texture_glid(),
            fb_tex_width[0], fb_tex_height[0]);
    }

    // Added
    static void get_sun_transform(mat4& transform, const vec3& position, float_t radius, const cam_data& cam) {
        /*mat4_mul_translate(&cam.get_view_mat(), &position, &transform);
        mat4_clear_rot(&transform, &transform);
        mat4_scale_rot(&transform, radius, &transform);
        mat4_mul(&transform, &cam.get_proj_mat(), &transform);*/

        mat4 mat;
        mat4_invert_rotation_fast(&cam.get_view_mat(), &mat);
        mat4_scale_rot(&mat, radius, &mat);
        mat4_set_translation(&mat, &position);
        mat4_mul(&mat, &cam.get_view_proj_mat(), &transform);
    }
}
