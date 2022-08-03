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

static void sub_1405163C0(rob_chara* rob_chr, int32_t a2, mat4* mat);

post_process_exposure::post_process_exposure() : exposure_history_counter(),
chara_data(), query_index() {
    chara_data = new post_process_exposure_chara_data[ROB_CHARA_COUNT];
    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
        memset(&chara_data[i], 0, sizeof(post_process_exposure_chara_data));
        chara_data[i].query_data[0] = -1;
        chara_data[i].query_data[1] = -1;
        chara_data[i].query_data[2] = -1;
    }
}

post_process_exposure::~post_process_exposure() {
    if (!this)
        return;

    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++)
        glDeleteQueries(3, chara_data[i].query);

    delete chara_data;
}

void post_process_exposure::get_exposure(camera* cam, int32_t render_width,
    int32_t render_height, bool reset_exposure, GLuint in_tex_0, GLuint in_tex_1) {
    bool v2 = false;
    if (!reset_exposure) {
        for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
            memset(&chara_data[i].field_0, 0, sizeof(chara_data[i].field_0));
            chara_data[i].field_80 = 0.0f;
        }

        post_process_exposure_chara_data* v6 = chara_data;
        float_t v18 = (float_t)render_height / (float_t)render_width;
        float_t v20 = powf(tanf((float_t)(cam->fov_rad * 0.5f)) * 3.4f, 2.0f);
        for (int32_t i = 0; i < ROB_CHARA_COUNT; i++, v6++) {
            rob_chara* rob_chr = rob_chara_array_get(i);
            if (!rob_chr || ~rob_chr->data.field_0 & 1)
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

            v6->field_0[0].x = (v13 + v21 * 0.0f * v18 + 1.0f) * 0.5f;
            v6->field_0[0].y = (v14 + v21 * 0.1f + 1.0f) * 0.5f;
            v6->field_0[0].z = 0.0f;
            v6->field_0[0].w = 4.0f;
            v6->field_0[1].x = (v13 + v21 * 0.0f * v18 + 1.0f) * 0.5f;
            v6->field_0[1].y = (v14 - v21 * 0.3f + 1.0f) * 0.5f;
            v6->field_0[1].z = 0.0f;
            v6->field_0[1].w = 4.0f;
            v6->field_0[2].x = (v13 + v21 * -0.5f * v18 + 1.0f) * 0.5f;
            v6->field_0[2].y = (v14 + v21 * -0.5f + 1.0f) * 0.5f;
            v6->field_0[2].z = 0.0f;
            v6->field_0[2].w = 3.0f;
            v6->field_0[3].x = (v13 - v21 * 0.6f * v18 + 1.0f) * 0.5f;
            v6->field_0[3].y = (v14 - v21 * 0.1f + 1.0f) * 0.5f;
            v6->field_0[3].z = 0.0f;
            v6->field_0[3].w = 2.0f;
            v6->field_0[4].x = (v13 + v21 * 0.6f * v18 + 1.0f) * 0.5f;
            v6->field_0[4].y = (v14 - v21 * 0.1f + 1.0f) * 0.5f;
            v6->field_0[4].z = 0.0f;
            v6->field_0[4].w = 2.0f;
            v6->field_0[5].x = (v13 + v21 * 0.5f * v18 + 1.0f) * 0.5f;
            v6->field_0[5].y = (v14 + v21 * -0.5f + 1.0f) * 0.5f;
            v6->field_0[5].z = 0.0f;
            v6->field_0[5].w = 3.0f;
            v6->field_0[6].x = (v13 + v21 * 0.0f * v18 + 1.0f) * 0.5f;
            v6->field_0[6].y = (v14 - v21 * 0.8f + 1.0f) * 0.5f;
            v6->field_0[6].z = 0.0f;
            v6->field_0[6].w = 3.0f;
            v6->field_80 = v16 * 1.6f * v17 * v22;
            if (v6->field_80 > 0.0f && !v6->query_data[query_index])
                v2 = true;
        }
    }

    if (!v2 || reset_exposure) {
        uniform_value[U_EXPOSURE] = 1;
        render_texture::shader_set(&shaders_ft, SHADER_FT_EXPOSURE);
        shaders_ft.local_frag_set(1,
            chara_data[0].field_80,
            chara_data[1].field_80,
            chara_data[2].field_80,
            chara_data[3].field_80);

        post_process_exposure_chara_data* v27 = chara_data;
        for (int32_t i = 0; i < 4 && i < ROB_CHARA_COUNT; i++, v27++)
            shaders_ft.local_frag_set(4ULL + (size_t)i * 8, 8, v27->field_0);

        if (reset_exposure)
            glViewport(0, 0, 32, 1);
        else
            glViewport(exposure_history_counter, 0, 1, 1);
        exposure_history.bind();
        gl_state_active_bind_texture_2d(0, in_tex_0);
        gl_state_active_bind_texture_2d(1, in_tex_1);
        render_texture::draw_params(&shaders_ft,
            reset_exposure ? 32 : 1, 1, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        exposure_history_counter++;
        exposure_history_counter %= 32;
    }

    glViewport(0, 0, 1, 1);
    exposure.bind();
    gl_state_active_bind_texture_2d(0, exposure_history.color_texture->tex);
    render_texture::draw_params(&shaders_ft, 1, 1, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
}

void post_process_exposure::get_exposure_chara_data(void* pp, camera* cam) {
    shader::unbind();

    gl_state_set_color_mask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    gl_state_set_depth_mask(GL_FALSE);

    post_process_exposure_chara_data* v3 = chara_data;
    int32_t v4 = query_index++ % 3;
    int32_t v7 = (v4 + 2) % 3;
    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++, v3++) {
        rob_chara* rob_chr = rob_chara_array_get(i);
        if (!rob_chr || ~rob_chr->data.field_0 & 1)
            continue;

        float_t max_face_depth = rob_chr->get_max_face_depth();

        mat4 mat = mat4_identity;
        sub_1405163C0(rob_chr, 4, &mat);
        mat4_mult(&mat, &cam->view, &mat);
        mat4_translate_mult(&mat, max_face_depth + 0.1f, 0.0f, -0.06f, &mat);
        mat4_clear_rot(&mat, &mat);
        mat4_mult(&mat, &cam->projection, &mat);

        ((post_process*)pp)->draw_query_samples(v3->query[v7], 0.0035f, mat);

        if (v3->query_data[v7] == -1)
            v3->query_data[v7] = 0;

        if (v3->query_data[v4] != -1) {
            int32_t v14 = 0;
            glGetQueryObjectiv(v3->query[v4], GL_QUERY_RESULT_AVAILABLE, &v14);
            if (v14)
                glGetQueryObjectuiv(v3->query[v4], GL_QUERY_RESULT, v3->query_data);
            else
                v3->query_data[v4] = 0;
        }
    }

    gl_state_set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    gl_state_set_depth_mask(GL_TRUE);
}

void post_process_exposure::init_fbo() {
    if (!this)
        return;

    exposure_history_counter = 0;

    if (!exposure_history.color_texture)
        exposure_history.init(32, 1, 0, GL_R32F, 0);
    if (!exposure.color_texture)
        exposure.init(1, 1, 0, GL_R32F, 0);

    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++)
        if (!chara_data[i].query[0])
            glGenQueries(3, chara_data[i].query);
}

static void sub_1405163C0(rob_chara* rob_chr, int32_t a2, mat4* mat) {
    if (a2 >= 0 && a2 <= 26)
        *mat = rob_chr->data.field_1E68.field_78[a2];
}
