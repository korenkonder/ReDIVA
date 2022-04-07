/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "exposure.h"
#include "../fbo.h"
#include "../gl_state.h"
#include "../post_process.h"
#include "../rob.h"
#include "../shader_ft.h"

typedef struct struc_188 {
    vec4u field_0[8];
    float_t field_80;
    GLuint field_84[3];
    GLuint field_90[3];
} struc_188;

static void post_process_exposure_free_fbo(post_process_exposure* exp);
static void sub_1405163C0(rob_chara* rob_chr, int32_t a2, mat4* mat);

post_process_exposure* post_process_exposure_init() {
    post_process_exposure* exp = force_malloc_s(post_process_exposure, 1);
    return exp;
}

void post_process_get_exposure(post_process_exposure* exp, camera* cam, int32_t render_width,
    int32_t render_height, bool reset_exposure, GLuint in_tex_0, GLuint in_tex_1) {
    if (!exp)
        return;

    struc_188 field_648[ROB_CHARA_COUNT];

    for (struc_188& i : field_648)
        i = {};

    bool v2 = false;
    float_t v18 = (float_t)render_height / (float_t)render_width;
    float_t v20 = powf(tanf((float_t)(cam->fov_rad * 0.5f)) * 3.4f, 2.0f);
    if (reset_exposure)
        goto LABEL_30;

    for (struc_188& i : field_648) {
        rob_chara* rob_chr = rob_chara_array_get((int32_t)(&i - field_648));
        if (!rob_chr || ~rob_chr->data.field_0 & 0x01)
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
        mat4_mult_vec(&v42, &v34, &v41);

        vec4 v40;
        mat4_mult_vec(&v43, &v34, &v40);

        float_t v11 = v41.z;
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
        v16 = min(v16, 1.0f);

        vec4 v39;
        mat4_mult_vec(&v42, &v33, &v39);

        float_t v17 = (v39.z + 1.0f) * 0.5f;
        v17 = clamp(v17, 0.0f, 0.8f);
        float_t v21 = 0.25f / sqrtf(v20 * (v11 * v11));

        float_t v22;
        if (v21 >= 0.055f) {
            v22 = 1.0f;
            if (v21 > 0.5f)
                v22 = 1.0f - (v21 - 0.5f) * (float_t)(10.0 / 3.0f);
        }
        else
            v22 = (v21 - 0.035f) * 50.0f;
        v22 = max(v22, 0.0f);

        float_t v23 = (v13 + 1.0f) * 0.5f;
        float_t v24 = (v21 * -0.5f + v14 + 1.0f) * 0.5f;
        float_t v25 = (v14 - v21 * 0.1f + 1.0f) * 0.5f;

        i.field_80 = v16 * 1.6f * v17 * v22;
        i.field_0[0].x = v23;
        i.field_0[0].y = (v21 * 0.1f + v14 + 1.0f) * 0.5f;
        i.field_0[0].z = 0.0f;
        i.field_0[0].w = 4.0f;
        i.field_0[1].x = v23;
        i.field_0[1].y = (v14 - v21 * 0.3f + 1.0f) * 0.5f;
        i.field_0[1].z = 0.0f;
        i.field_0[1].w = 4.0f;
        i.field_0[2].x = (v21 * -0.5f * v18 + v13 + 1.0f) * 0.5f;
        i.field_0[2].y = v24;
        i.field_0[2].z = 0.0f;
        i.field_0[2].w = 3.0f;
        i.field_0[3].x = (v13 - v21 * 0.6f * v18 + 1.0f) * 0.5f;
        i.field_0[3].y = v25;
        i.field_0[3].z = 0.0f;
        i.field_0[3].w = 2.0f;
        i.field_0[4].x = (v21 * 0.6f * v18 + v13 + 1.0f) * 0.5f;
        i.field_0[4].y = v25;
        i.field_0[4].z = 0.0f;
        i.field_0[4].w = 2.0f;
        i.field_0[5].x = (v21 * 0.5f * v18 + v13 + 1.0f) * 0.5f;
        i.field_0[5].y = v24;
        i.field_0[5].z = 0.0f;
        i.field_0[5].w = 3.0f;
        i.field_0[6].x = v23;
        i.field_0[6].y = (v14 - v21 * 0.8f + 1.0f) * 0.5f;
        i.field_0[6].z = 0.0f;
        i.field_0[6].w = 3.0f;
        if (i.field_80 > 0.0f && false/*!i.field_90[pp->field_9F4]*/)
            v2 = true;
    }

    if (!v2 || reset_exposure) {
    LABEL_30:
        uniform_value[U_EXPOSURE] = 1;
        render_texture_shader_set(&shaders_ft, SHADER_FT_EXPOSURE);
        shader_local_frag_set(&shaders_ft, 1,
            field_648[0].field_80,
            field_648[1].field_80,
            field_648[2].field_80,
            field_648[3].field_80);
        
        int32_t index = 0;
        for (struc_188& i : field_648) {
            shader_local_frag_set_ptr_array(&shaders_ft, 4ULL + index * 8ULL, 8, (vec4*)field_648->field_0);
            if (++index >= 4)
                break;
        }

        if (reset_exposure)
            glViewport(0, 0, 32, 1);
        else
            glViewport(exp->exposure_history_counter, 0, 1, 1);
        render_texture_bind(&exp->exposure_history, 0);
        gl_state_active_bind_texture_2d(0, in_tex_0);
        gl_state_active_bind_texture_2d(1, in_tex_1);
        render_texture_draw_params(&shaders_ft,
            reset_exposure ? 32 : 1, 1, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        exp->exposure_history_counter++;
        exp->exposure_history_counter %= 32;
    }

    uniform_value[U_EXPOSURE] = 2;
    render_texture_shader_set(&shaders_ft, SHADER_FT_EXPOSURE);

    glViewport(0, 0, 1, 1);
    render_texture_bind(&exp->exposure, 0);
    gl_state_active_bind_texture_2d(0, exp->exposure_history.color_texture->tex);
    render_texture_draw_params(&shaders_ft, 1, 1, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
}

void post_process_exposure_init_fbo(post_process_exposure* exp) {
    if (!exp)
        return;

    exp->exposure_history_counter = 0;

    if (!exp->exposure_history.color_texture)
        render_texture_init(&exp->exposure_history, 32, 1, 0, GL_R32F, 0);
    if (!exp->exposure.color_texture)
        render_texture_init(&exp->exposure, 1, 1, 0, GL_R32F, 0);
}

void post_process_exposure_dispose(post_process_exposure* exp) {
    if (!exp)
        return;

    post_process_exposure_free_fbo(exp);
    free(exp);
}

static void post_process_exposure_free_fbo(post_process_exposure* exp) {
    render_texture_free(&exp->exposure_history);
    render_texture_free(&exp->exposure);
}

static void sub_1405163C0(rob_chara* rob_chr, int32_t a2, mat4* mat) {
    if (a2 >= 0 && a2 <= 26)
        *mat = rob_chr->data.field_1E68.field_78[a2];
}
