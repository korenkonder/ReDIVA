/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "exposure.hpp"
#include "../rob/rob.hpp"
#include "../fbo.hpp"
#include "../gl_state.hpp"
#include "../post_process.hpp"
#include "../shader_ft.hpp"

struct exposure_measure_shader_data {
    vec4 g_spot_weight;
    vec4 g_spot_coefficients[32];
};

static void sub_1405163C0(rob_chara* rob_chr, int32_t a2, mat4* mat);

post_process_exposure_chara_data::post_process_exposure_chara_data() : spot_weight(), query(), query_data() {
    for (GLuint& i : query_data)
        i = -1;
}

post_process_exposure::post_process_exposure() : exposure_history_counter(),
chara_data(), query_index() {
    chara_data = new post_process_exposure_chara_data[ROB_CHARA_COUNT];
    exposure_measure_ubo.Create(sizeof(exposure_measure_shader_data));
}

post_process_exposure::~post_process_exposure() {
    if (!this)
        return;

    exposure_measure_ubo.Destroy();

    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++)
        glDeleteQueries(3, chara_data[i].query);

    delete chara_data;
}

void post_process_exposure::get_exposure(camera* cam, int32_t render_width,
    int32_t render_height, bool reset_exposure, GLuint in_tex_0, GLuint in_tex_1) {
    bool v2 = false;
    if (!reset_exposure) {
        for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
            memset(&chara_data[i].spot_coefficients,
                0, sizeof(chara_data[i].spot_coefficients));
            chara_data[i].spot_weight = 0.0f;
        }

        post_process_exposure_chara_data* v6 = chara_data;
        float_t v18 = (float_t)render_height / (float_t)render_width;
        float_t v20 = powf(tanf((float_t)(cam->fov_rad * 0.5f)) * 3.4f, 2.0f);
        for (int32_t i = 0; i < ROB_CHARA_COUNT; i++, v6++) {
            rob_chara* rob_chr = rob_chara_array_get(i);
            if (!rob_chr || !rob_chr->is_visible())
                continue;

            vec4 v34 = { 0.05f, 0.0f, -0.04f, 1.0f };
            vec4 v33 = { 1.0f, 0.0f, 0.0f, 0.0f };

            mat4 v44;
            sub_1405163C0(rob_chr, 4, &v44);

            mat4 v42 = cam->view;
            mat4 v43 = cam->projection;
            mat4_mult(&v44, &v42, &v42);
            mat4_mult(&v42, &v43, &v43);

            vec4 v41;
            vec4 v40;
            mat4_mult_vec(&v42, &v34, &v41);
            mat4_mult_vec(&v43, &v34, &v40);

            float_t v12 = 1.0f / v40.w;
            float_t v13 = v40.x * v12;
            float_t v14 = v40.y * v12;
            if (v41.z >= 0.0f || fabsf(v13) >= 1.0f || fabsf(v14) >= 1.0f)
                continue;

            float_t v31 = 0.5f - (fabsf(v14) - 1.0f) * 2.5f;
            float_t v32 = 0.5f - (fabsf(v13) - 1.0f) * 2.5f;

            float_t v16;
            if (v32 <= v31)
                v16 = v32;
            else
                v16 = v31;

            if (v16 > 1.0f)
                v16 = 1.0f;

            vec4 v39;
            mat4_mult_vec(&v42, &v33, &v39);

            float_t v17 = (v39.z + 1.0f) * 0.5f;
            if (v17 > 0.8f)
                v17 = 0.8f;
            else if (v17 < 0.2f)
                v17 = 0.0f;

            float_t v21 = 0.25f / sqrtf(powf(v20 * 3.4f, 2.0f) * (v41.z * v41.z));
            float_t v22;
            if (v21 < 0.055f) {
                v22 = (v21 - 0.035f) * 50.0f;
                if (v22 < 0.0f)
                    v22 = 0.0f;
            }
            else if (v21 > 0.5f) {
                v22 = 1.0f - (v21 - 0.5f) * 3.3333333f;
                if (v22 < 0.0f)
                    v22 = 0.0f;
            }
            else
                v22 = 1.0f;

            v6->spot_coefficients[0].x = (v13 + v21 * 0.0f * v18 + 1.0f) * 0.5f;
            v6->spot_coefficients[0].y = (v14 + v21 * 0.1f + 1.0f) * 0.5f;
            v6->spot_coefficients[0].z = 0.0f;
            v6->spot_coefficients[0].w = 4.0f;
            v6->spot_coefficients[1].x = (v13 + v21 * 0.0f * v18 + 1.0f) * 0.5f;
            v6->spot_coefficients[1].y = (v14 - v21 * 0.3f + 1.0f) * 0.5f;
            v6->spot_coefficients[1].z = 0.0f;
            v6->spot_coefficients[1].w = 4.0f;
            v6->spot_coefficients[2].x = (v13 + v21 * -0.5f * v18 + 1.0f) * 0.5f;
            v6->spot_coefficients[2].y = (v14 + v21 * -0.5f + 1.0f) * 0.5f;
            v6->spot_coefficients[2].z = 0.0f;
            v6->spot_coefficients[2].w = 3.0f;
            v6->spot_coefficients[3].x = (v13 - v21 * 0.6f * v18 + 1.0f) * 0.5f;
            v6->spot_coefficients[3].y = (v14 - v21 * 0.1f + 1.0f) * 0.5f;
            v6->spot_coefficients[3].z = 0.0f;
            v6->spot_coefficients[3].w = 2.0f;
            v6->spot_coefficients[4].x = (v13 + v21 * 0.6f * v18 + 1.0f) * 0.5f;
            v6->spot_coefficients[4].y = (v14 - v21 * 0.1f + 1.0f) * 0.5f;
            v6->spot_coefficients[4].z = 0.0f;
            v6->spot_coefficients[4].w = 2.0f;
            v6->spot_coefficients[5].x = (v13 + v21 * 0.5f * v18 + 1.0f) * 0.5f;
            v6->spot_coefficients[5].y = (v14 + v21 * -0.5f + 1.0f) * 0.5f;
            v6->spot_coefficients[5].z = 0.0f;
            v6->spot_coefficients[5].w = 3.0f;
            v6->spot_coefficients[6].x = (v13 + v21 * 0.0f * v18 + 1.0f) * 0.5f;
            v6->spot_coefficients[6].y = (v14 - v21 * 0.8f + 1.0f) * 0.5f;
            v6->spot_coefficients[6].z = 0.0f;
            v6->spot_coefficients[6].w = 3.0f;
            v6->spot_weight = v16 * 1.6f * v17 * v22;
            if (v6->spot_weight > 0.0f && !v6->query_data[query_index])
                v2 = true;
        }
    }

    if (!v2 || reset_exposure) {
        exposure_measure_shader_data exposure_measure = {};
        exposure_measure.g_spot_weight = {
            chara_data[0].spot_weight,
            chara_data[1].spot_weight,
            chara_data[2].spot_weight,
            chara_data[3].spot_weight
        };

        post_process_exposure_chara_data* chara_data = this->chara_data;
        for (int32_t i = 0; i < 4 && i < ROB_CHARA_COUNT; i++, chara_data++)
            for (int32_t j = 0; j < 8; j++)
                exposure_measure.g_spot_coefficients[i * 8 + j] = chara_data->spot_coefficients[j];
        exposure_measure_ubo.WriteMapMemory(exposure_measure);

        if (reset_exposure)
            glViewport(0, 0, 32, 1);
        else
            glViewport(exposure_history_counter, 0, 1, 1);
        exposure_history.Bind();

        uniform_value[U_EXPOSURE] = 1;
        shaders_ft.set(SHADER_FT_EXPOSURE);
        exposure_measure_ubo.Bind(1);
        gl_state_active_bind_texture_2d(0, in_tex_0);
        gl_state_active_bind_texture_2d(1, in_tex_1);
        RenderTexture::DrawQuad(&shaders_ft, reset_exposure ? 32 : 1, 1);
        exposure_history_counter++;
        exposure_history_counter %= 32;
    }

    glViewport(0, 0, 1, 1);
    exposure.Bind();
    uniform_value[U_EXPOSURE] = 2;
    shaders_ft.set(SHADER_FT_EXPOSURE);
    gl_state_active_bind_texture_2d(0, exposure_history.color_texture->tex);
    RenderTexture::DrawQuad(&shaders_ft, 1, 1);
}

void post_process_exposure::get_exposure_chara_data(void* pp_data, camera* cam) {
    shader::unbind();

    post_process* pp = (post_process*)pp_data;

    gl_state_set_color_mask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    gl_state_set_depth_mask(GL_FALSE);
    gl_state_disable_cull_face();

    shaders_ft.set(SHADER_FT_SUN_NO_TEXTURED);
    gl_state_bind_vertex_array(pp->query_vao);
    pp->sun_quad_ubo.Bind(0);

    post_process_exposure_chara_data* chara = chara_data;
    int32_t query_index = (this->query_index + 1) % 3;
    this->query_index = query_index;
    int32_t next_query_index = (query_index + 2) % 3;
    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++, chara++) {
        rob_chara* rob_chr = rob_chara_array_get(i);
        if (!rob_chr || !rob_chr->is_visible())
            continue;

        float_t max_face_depth = rob_chr->get_max_face_depth();

        mat4 mat = mat4_identity;
        sub_1405163C0(rob_chr, 4, &mat);
        mat4_mult(&mat, &cam->view, &mat);
        mat4_translate_mult(&mat, max_face_depth + 0.1f, 0.0f, -0.06f, &mat);
        mat4_clear_rot(&mat, &mat);
        mat4_scale_rot(&mat, 0.0035f, &mat);
        mat4_mult(&mat, &cam->projection, &mat);

        sun_quad_shader_data shader_data = {};
        mat4_transpose(&mat, &mat);
        shader_data.g_transform[0] = mat.row0;
        shader_data.g_transform[1] = mat.row1;
        shader_data.g_transform[2] = mat.row2;
        shader_data.g_transform[3] = mat.row3;
        shader_data.g_emission = 0.0f;
        pp->sun_quad_ubo.WriteMapMemory(shader_data);

        glBeginQuery(GL_SAMPLES_PASSED, chara->query[next_query_index]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glEndQuery(GL_SAMPLES_PASSED);

        if (chara->query_data[next_query_index] == -1)
            chara->query_data[next_query_index] = 0;

        if (chara->query_data[query_index] != -1) {
            int32_t res = 0;
            glGetQueryObjectiv(chara->query[query_index], GL_QUERY_RESULT_AVAILABLE, &res);
            if (res)
                glGetQueryObjectuiv(chara->query[query_index], GL_QUERY_RESULT, chara->query_data);
            else
                chara->query_data[query_index] = 0;
        }
    }

    gl_state_bind_vertex_array(0);

    gl_state_set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    gl_state_set_depth_mask(GL_TRUE);
    gl_state_enable_cull_face();
}

void post_process_exposure::init_fbo() {
    if (!this)
        return;

    exposure_history_counter = 0;

    if (!exposure_history.color_texture)
        exposure_history.Init(32, 1, 0, GL_R32F, 0);
    if (!exposure.color_texture)
        exposure.Init(2, 2, 0, GL_R32F, 0);

    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++)
        if (!chara_data[i].query[0])
            glGenQueries(3, chara_data[i].query);
}

static void sub_1405163C0(rob_chara* rob_chr, int32_t a2, mat4* mat) {
    if (a2 >= 0 && a2 <= 26)
        *mat = rob_chr->data.field_1E68.field_78[a2];
}
