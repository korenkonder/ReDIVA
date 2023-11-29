/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "post_process.hpp"
#include "rob/rob.hpp"
#include "fbo.hpp"
#include "gl_state.hpp"
#include "render_texture.hpp"
#include "shader_ft.hpp"
#include "stage.hpp"
#include "stage_modern.hpp"
#include "static_var.hpp"
#include "texture.hpp"

extern bool task_stage_is_modern;

extern render_context* rctx_ptr;

static void post_process_generate_area_texture(post_process* pp);
static void post_process_calculate_area_texture_data(uint8_t* data, int32_t cross1, int32_t cross2);
static void post_process_calculate_area_texture_data_area(float_t* val_left,
    float_t* val_right, int32_t cross1, int32_t cross2, int32_t dleft, int32_t dright);

post_process_frame_texture_render_texture::post_process_frame_texture_render_texture() : texture() {
    type = POST_PROCESS_FRAME_TEXTURE_MAX;
}

post_process_frame_texture_render_texture::~post_process_frame_texture_render_texture() {

}

post_process_frame_texture::post_process_frame_texture() : capture() {

}

post_process_frame_texture::~post_process_frame_texture() {

}

post_process::post_process() : ssaa(), mlaa(), ss_alpha_mask(), aet_back_tex(), render_textures_data(),
movie_textures_data(), aet_back(), mlaa_area_texture(), sss_contour_texture(), texture_counter(),
lens_shaft_query(), lens_flare_query(), lens_shaft_query_data(), lens_flare_query_data(),
lens_flare_query_index(), lens_flare_texture(), lens_shaft_texture(), lens_ghost_texture(),
lens_ghost_count(), lens_flare_pos(), lens_shaft_scale(), lens_shaft_inv_scale(), lens_flare_power(),
field_A10(), lens_flare_appear_power(), render_width(), render_height(), view_point(),
interest(), view_point_prev(), interest_prev(), reset_exposure(), sprite_width(), sprite_height(),
screen_x_offset(), screen_y_offset(), screen_width(), screen_height(), mag_filter() {
    blur = new post_process_blur();
    dof = new post_process_dof();
    exposure = new post_process_exposure();
    tone_map = new post_process_tone_map();
    transparency = new post_process_transparency();
    stage_index = -1;
    stage_index_prev = -1;

    GLuint sampler;
    glGenSamplers(3, samplers);
    sampler = samplers[0];
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    sampler = samplers[1];
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    static const vec4 border_color = 0.0f;

    sampler = samplers[2];
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glSamplerParameterfv(sampler, GL_TEXTURE_BORDER_COLOR, (GLfloat*)&border_color);

    glGenVertexArrays(1, &query_vao);

    glGenVertexArrays(1, &lens_ghost_vao);
    gl_state_bind_vertex_array(lens_ghost_vao);

    glGenBuffers(1, &lens_ghost_vbo);
    gl_state_bind_array_buffer(lens_ghost_vbo, true);
    if (GLAD_GL_VERSION_4_4)
        glBufferStorage(GL_ARRAY_BUFFER, sizeof(float_t) * 5 * (6 * 16),
            0, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
    else
        glBufferData(GL_ARRAY_BUFFER, sizeof(float_t) * 5 * (6 * 16), 0, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float_t) * 5, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float_t) * 5, (void*)(sizeof(float_t) * 2));
    gl_state_bind_array_buffer(0);
    gl_state_bind_vertex_array(0);

    post_process_generate_area_texture(this);

    if (!lens_shaft_query[0])
        glGenQueries(3, lens_shaft_query);

    if (!lens_flare_query[0])
        glGenQueries(3, lens_flare_query);

    sun_quad_ubo.Create(sizeof(sun_quad_shader_data));

    for (GLuint& i : lens_shaft_query_data)
        i = -1;

    for (GLuint& i : lens_flare_query_data)
        i = -1;

    reset();
}

post_process::~post_process() {
    if (aet_back_tex)
        texture_free(aet_back_tex);

    if (blur) {
        delete blur;
        blur = 0;
    }

    if (dof) {
        delete dof;
        dof = 0;
    }

    if (exposure) {
        delete exposure;
        exposure = 0;
    }

    if (tone_map) {
        delete tone_map;
        tone_map = 0;
    }

    if (transparency) {
        delete transparency;
        transparency = 0;
    }

    if (mlaa_area_texture) {
        glDeleteTextures(1, &mlaa_area_texture);
        mlaa_area_texture = 0;
    }

    if (samplers[0]) {
        glDeleteSamplers(3, samplers);
        samplers[0] = 0;
    }

    if (query_vao) {
        glDeleteVertexArrays(1, &query_vao);
        query_vao = 0;
    }

    if (lens_ghost_vao) {
        glDeleteVertexArrays(1, &lens_ghost_vao);
        lens_ghost_vao = 0;
    }

    if (lens_ghost_vbo) {
        glDeleteBuffers(1, &lens_ghost_vbo);
        lens_ghost_vbo = 0;
    }

    if (lens_shaft_query[0]) {
        glDeleteQueries(3, lens_shaft_query);
        lens_shaft_query[0] = 0;
    }

    if (lens_flare_query[0]) {
        glDeleteQueries(3, lens_flare_query);
        lens_flare_query[0] = 0;
    }

    sun_quad_ubo.Destroy();
}

void post_process::apply(camera* cam, texture* light_proj_tex, int32_t npr_param) {
    for (int32_t i = 0; i < 8; i++)
        gl_state_bind_sampler(i, 0);

    dof->apply(&rend_texture, &buf_texture, cam);

    draw_lens_ghost(&rend_texture);

    get_frame_texture(rend_texture.GetColorTex(), POST_PROCESS_FRAME_TEXTURE_PRE_PP);

    blur->downsample(&rend_texture);
    blur->get_blur();
    exposure->get_exposure(cam, render_width, render_height, reset_exposure,
        blur->reduce_texture[4].GetColorTex(), blur->reduce_texture[2].GetColorTex());
    tone_map->apply(&rend_texture, light_proj_tex, (texture*)(aet_back ? aet_back_tex : 0),
        &rend_texture, &buf_texture, /* sss_contour_texture,*/
        blur->reduce_texture[0].GetColorTex(), exposure->exposure.GetColorTex(), npr_param, this);
    if (mlaa)
        apply_mlaa(samplers, ss_alpha_mask);

    for (int32_t i = 0; i < 16; i++) {
        if (!render_textures_data[i])
            continue;

        render_textures[i].Bind();

        texture* t = render_textures_data[i];
        if (render_width > t->width * 2ULL || render_height > t->height * 2ULL)
            uniform_value[U_REDUCE] = 1;
        else
            uniform_value[U_REDUCE] = 0;

        glViewport(0, 0, t->width, t->height);
        gl_state_active_bind_texture_2d(0, rend_texture.GetColorTex());
        gl_state_bind_sampler(0, samplers[0]);
        shaders_ft.set(SHADER_FT_REDUCE);
        RenderTexture::DrawQuad(&shaders_ft, render_width, render_height,
            1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    }

    get_frame_texture(rend_texture.GetColorTex(), POST_PROCESS_FRAME_TEXTURE_POST_PP);

    frame_texture_reset_capture();

    screen_texture.Bind();
    glViewport(0, 0, sprite_width, sprite_height);
    if (ssaa) {
        gl_state_active_bind_texture_2d(0, rend_texture.GetColorTex());
        gl_state_bind_sampler(0, samplers[0]);
        uniform_value[U_ALPHA_MASK] = ss_alpha_mask ? 1 : 0;
        uniform_value[U_REDUCE] = 0;
        shaders_ft.set(SHADER_FT_REDUCE);
        RenderTexture::DrawQuad(&shaders_ft, render_width, render_height,
            1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        uniform_value[U_ALPHA_MASK] = 0;
    }
    else {
        gl_state_active_bind_texture_2d(0, rend_texture.GetColorTex());
        if (mag_filter == POST_PROCESS_MAG_FILTER_NEAREST)
            gl_state_bind_sampler(0, samplers[1]);
        else
            gl_state_bind_sampler(0, samplers[0]);

        switch (mag_filter) {
        case POST_PROCESS_MAG_FILTER_NEAREST:
        case POST_PROCESS_MAG_FILTER_BILINEAR:
        default:
            uniform_value[U_MAGNIFY] = 0;
            break;
        case POST_PROCESS_MAG_FILTER_SHARPEN_5_TAP:
            uniform_value[U_MAGNIFY] = 2;
            break;
        case POST_PROCESS_MAG_FILTER_SHARPEN_4_TAP:
            uniform_value[U_MAGNIFY] = 3;
            break;
        case POST_PROCESS_MAG_FILTER_CONE_4_TAP:
            uniform_value[U_MAGNIFY] = 4;
            break;
        case POST_PROCESS_MAG_FILTER_CONE_2_TAP:
            uniform_value[U_MAGNIFY] = 5;
            break;
        }

        shaders_ft.set(SHADER_FT_MAGNIFY);
        RenderTexture::DrawQuad(&shaders_ft, render_width, render_height,
            1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    }
    shader::unbind();

    for (int32_t i = 0; i < 8; i++)
        gl_state_bind_sampler(i, 0);
}

void post_process::apply_mlaa(GLuint* samplers, int32_t ss_alpha_mask) {
    gl_state_begin_event("PostProcess::mlaa");
    mlaa_buffer.Bind();
    gl_state_active_bind_texture_2d(0, rend_texture.GetColorTex());
    gl_state_bind_sampler(0, samplers[1]);
    uniform_value[U_MLAA] = 0;
    shaders_ft.set(SHADER_FT_MLAA);
    RenderTexture::DrawQuad(&shaders_ft, render_width, render_height,
        1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

    temp_buffer.Bind();
    gl_state_active_bind_texture_2d(0, mlaa_buffer.GetColorTex());
    gl_state_active_bind_texture_2d(1, mlaa_area_texture);
    gl_state_bind_sampler(0, samplers[0]);
    gl_state_bind_sampler(1, samplers[1]);
    uniform_value[U_MLAA] = 1;
    uniform_value[U_MLAA_SEARCH] = 2;
    shaders_ft.set(SHADER_FT_MLAA);
    RenderTexture::DrawQuad(&shaders_ft, render_width, render_height,
        1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

    buf_texture.Bind();
    gl_state_active_bind_texture_2d(0, rend_texture.GetColorTex());
    gl_state_active_bind_texture_2d(1, temp_buffer.GetColorTex());
    gl_state_bind_sampler(0, samplers[1]);
    uniform_value[U_MLAA] = 2;
    uniform_value[U_ALPHA_MASK] = ss_alpha_mask ? 1 : 0;
    shaders_ft.set(SHADER_FT_MLAA);
    RenderTexture::DrawQuad(&shaders_ft, render_width, render_height,
        1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    uniform_value[U_ALPHA_MASK] = 0;
    gl_state_active_bind_texture_2d(0, 0);
    gl_state_active_bind_texture_2d(1, 0);

    fbo::blit(buf_texture.fbos[0], rend_texture.fbos[0],
        0, 0, buf_texture.GetWidth(), buf_texture.GetHeight(),
        0, 0, rend_texture.GetWidth(), rend_texture.GetHeight(), GL_COLOR_BUFFER_BIT, GL_LINEAR);
    gl_state_end_event();
}

void post_process::ctrl(camera* cam) {
    view_point_prev = view_point;
    interest_prev = interest;
    cam->get_view_point(view_point);
    cam->get_interest(interest);

    stage_index_prev = stage_index;
    if (task_stage_is_modern)
        stage_index = (int32_t)task_stage_modern_get_current_stage_hash();
    else
        stage_index = task_stage_get_current_stage_index();

    reset_exposure = cam->fast_change_hist1 && !cam->fast_change_hist0;
    if (reset_exposure) {
        float_t view_point_dist = vec3::distance(view_point, view_point_prev);

        vec3 dir = vec3::normalize(interest - view_point);
        vec3 dir_prev = vec3::normalize(interest_prev - view_point_prev);

        float_t dir_diff_angle = vec3::dot(dir, dir_prev);
        if (dir_diff_angle < 0.5f)
            dir_diff_angle = 0.0f;
        if (view_point_dist < dir_diff_angle * 0.4f)
            reset_exposure = false;
    }
    else if (stage_index != stage_index_prev)
        reset_exposure = true;
}

void post_process::draw_lens_flare(camera* cam) {
    static float_t flt_1411ACB84 = sinf(0.01365909819f);
    static float_t flt_1411ACB8C = tanf(0.01365909819f) * 2.0f * 0.94f;

    if (!lens_flare_texture)
        return;

    texture* tex = obj_database_get_obj_set_texture(5, 4549);
    if (!tex)
        return;

    float_t v5 = tanf((float_t)(cam->get_fov() * DEG_TO_RAD) * 0.5f);
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

    vec4 v44;
    *(vec3*)&v44 = position;
    v44.w = 1.0f;

    mat4_transform_vector(&cam->view_projection, &v44, &v44);

    float_t v13 = 1.0f / v44.w;
    float_t v14 = v44.x * v13;
    float_t v15 = v44.y * v13;
    float_t v16 = (float_t)render_width * 0.01f / (float_t)render_height;
    v14 = clamp_def(v14, -0.99f, 0.99f);
    v15 = clamp_def(v15, v16 - 1.0f, 1.0f - v16);
    v44.x = v14 * v44.w;
    v44.y = v15 * v44.w;

    mat4_transform_vector(&cam->inv_view_projection, &v44, &v44);
    ((pos_scale*)&lens_flare_pos)->get_screen_pos_scale(cam->view_projection, position, false);

    float_t v17 = lens_flare_pos.x - (float_t)render_width * 0.5f;
    float_t v19 = v5 / (float_t)render_height * 0.5f;
    float_t v20 = lens_flare_pos.y - (float_t)render_height * 0.5f;

    float_t v22 = sqrtf(v17 * v19 * v17 * v19 + v20 * v19 * v20 * v19 + 1.0f);
    float_t v23 = ((float_t)render_width * (flt_1411ACB8C / v5)) * (v22 * v22 * 0.5f)
        * ((float_t)render_height * (flt_1411ACB8C / v5));
    float_t v24 = vec3::distance(position, view_point) * flt_1411ACB84;
    emission *= 1.0f / (float_t)(1.0f - cosf((float_t)(3.0 * DEG_TO_RAD)));

    sun_quad_shader_data shader_data = {};
    shader_data.g_emission = emission;

    gl_state_active_bind_texture_2d(0, tex->tex);
    shaders_ft.set(SHADER_FT_SUN);
    gl_state_bind_vertex_array(query_vao);
    sun_quad_ubo.Bind(0);

    int32_t query_index = (lens_flare_query_index + 1) % 3;
    lens_flare_query_index = query_index;
    int32_t next_query_index = (query_index + 2) % 3;

    if (lens_shaft_query_data[next_query_index] == -1)
        lens_shaft_query_data[next_query_index] = 0;

    if (lens_flare_query_data[next_query_index] == -1)
        lens_flare_query_data[next_query_index] = 0;

    gl_state_set_color_mask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    gl_state_set_depth_mask(GL_FALSE);
    gl_state_disable_cull_face();

    mat4 mat;
    mat4_mul_translate(&cam->view, &position, &mat);
    mat4_clear_rot(&mat, &mat);
    mat4_scale_rot(&mat, v24 * 0.2f, &mat);
    mat4_mul(&mat, &cam->projection, &mat);

    mat4_transpose(&mat, &mat);
    shader_data.g_transform[0] = mat.row0;
    shader_data.g_transform[1] = mat.row1;
    shader_data.g_transform[2] = mat.row2;
    shader_data.g_transform[3] = mat.row3;
    sun_quad_ubo.WriteMemory(shader_data);

    glBeginQuery(GL_SAMPLES_PASSED, lens_shaft_query[next_query_index]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glEndQuery(GL_SAMPLES_PASSED);

    mat4_mul_translate(&cam->view, (vec3*)&v44, &mat);
    mat4_clear_rot(&mat, &mat);
    mat4_scale_rot(&mat, v24 * 2.0f, &mat);
    mat4_mul(&mat, &cam->projection, &mat);

    mat4_transpose(&mat, &mat);
    shader_data.g_transform[0] = mat.row0;
    shader_data.g_transform[1] = mat.row1;
    shader_data.g_transform[2] = mat.row2;
    shader_data.g_transform[3] = mat.row3;
    sun_quad_ubo.WriteMemory(shader_data);

    glBeginQuery(GL_SAMPLES_PASSED, lens_flare_query[next_query_index]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glEndQuery(GL_SAMPLES_PASSED);

    gl_state_set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    gl_state_set_depth_mask(GL_TRUE);
    gl_state_enable_cull_face();

    if (lens_shaft_query_data[query_index] != -1) {
        int32_t res = 0;
        glGetQueryObjectiv(lens_shaft_query[query_index], GL_QUERY_RESULT_AVAILABLE, &res);
        if (res)
            glGetQueryObjectuiv(lens_shaft_query[query_index], GL_QUERY_RESULT, &lens_shaft_query_data[query_index]);
    }

    if (lens_flare_query_data[query_index] != -1) {
        int32_t res = 0;
        glGetQueryObjectiv(lens_flare_query[query_index], GL_QUERY_RESULT_AVAILABLE, &res);
        if (res)
            glGetQueryObjectuiv(lens_flare_query[query_index], GL_QUERY_RESULT, &lens_flare_query_data[query_index]);
    }

    if (emission.x + emission.y + emission.z > 0.0f) {
        gl_state_enable_blend();
        gl_state_set_blend_func(GL_ONE, GL_ONE);
        gl_state_set_depth_mask(GL_FALSE);

        mat4_mul_translate(&cam->view, &position, &mat);
        mat4_clear_rot(&mat, &mat);
        mat4_scale_rot(&mat, v24 * 1.1f, &mat);
        mat4_mul(&mat, &cam->projection, &mat);

        mat4_transpose(&mat, &mat);
        shader_data.g_transform[0] = mat.row0;
        shader_data.g_transform[1] = mat.row1;
        shader_data.g_transform[2] = mat.row2;
        shader_data.g_transform[3] = mat.row3;
        sun_quad_ubo.WriteMemory(shader_data);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        gl_state_disable_blend();
        gl_state_set_blend_func(GL_ONE, GL_ZERO);
        gl_state_set_depth_mask(GL_TRUE);
    }

    gl_state_active_bind_texture_2d(0, 0);
    gl_state_bind_vertex_array(0);

    if (lens_flare_appear_power <= 0.01f)
        lens_flare_appear_power = 0.0;
    else
        lens_flare_appear_power *= 0.93f;
    lens_shaft_scale = 100.0f;
    lens_flare_power = 0.0f;

    float_t v34 = lens_flare_pos.x;
    float_t v35 = lens_flare_pos.y;
    float_t v32 = lens_flare_pos.z;

    float_t v33 = 0.0f;
    if (v32 >= 0.0f
        || v34 < (float_t)-render_width || v34 > (float_t)(2 * render_width)
        || v35 < (float_t)-render_height || v35 > (float_t)(2 * render_height))
        lens_flare_texture = 0;
    else {
        for (GLuint& i : lens_flare_query_data)
            if (i != -1)
                v33 += min_def((float_t)i, v23);
        v33 /= (v23 * 3.0f);
        lens_flare_power = powf(v33, 0.7f);

        if (v33 > 0.0f && lens_shaft_query_data[query_index]) {
            float_t v41 = max_def(v33 - 0.4f, 0.005f);
            lens_shaft_scale = ((0.6f / v41) * lens_shaft_inv_scale) * (v5 * 3.4f);
        }
    }

    if (v32 >= 0.0f
        || v34 < -12.0f || v34 > (float_t)(render_width + 12)
        || v35 < -12.0f || v35 > (float_t)(render_height + 12)) {
        field_A10 = 0.0f;
    }
    else if (v34 > -2.0f && v34 < (float_t)(render_width + 2)
        && v35 > -2.0f && v35 < (float_t)(render_height + 2)) {
        if (v33 > 0.4f && field_A10 == 0.0f && lens_flare_appear_power < 0.02f && !reset_exposure)
            lens_flare_appear_power = 8.0f;
        field_A10 = 1.0f;
    }
}

static void make_ghost_quad(uint8_t flags, float_t opacity, mat4* mat, float_t*& data) {
    const float_t x0 = flags & 0x01 ? 0.5f : 0.0f;
    const float_t x1 = flags & 0x01 ? 1.0f : 0.5f;
    const float_t y0 = flags & 0x02 ? 0.5f : 0.0f;
    const float_t y1 = flags & 0x02 ? 1.0f : 0.5f;

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

void post_process::draw_lens_ghost(RenderTexture* rt) {
    static const float_t v13[16] = {
        -0.70f, -0.30f,  0.35f,  0.50f,
        -0.45f, -0.80f,  0.20f,  0.41f,
         0.17f, -0.10f,  0.06f,  0.10f,
         0.14f,  0.04f, -0.13f, -0.22f,
    };

    static const float_t v14[16] = {
        0.8f, 1.0f, 1.0f, 1.0f,
        0.4f, 0.5f, 0.8f, 0.8f,
        0.6f, 0.7f, 0.8f, 0.7f,
        0.8f, 0.7f, 0.6f, 0.8f,
    };

    static const float_t v15[16] = {
         1.3f, 1.5f, 1.00f, 1.1f,
         2.5f, 0.8f, 0.50f, 0.5f,
         0.7f, 0.4f, 0.35f, 0.5f,
         0.4f, 0.3f, 0.60f, 0.4f,
    };

    if (!lens_flare_texture || lens_ghost_count <= 0)
        return;

    if (lens_ghost_count > 16)
        lens_ghost_count = 16;

    const float_t aspect = (float_t)render_width / (float_t)render_height;

    const float_t v7 = (lens_flare_pos.x - (float_t)(render_width / 2)) / (float_t)render_width;
    const float_t v8 = -((lens_flare_pos.y - (float_t)(render_height / 2)) / (float_t)render_height);
    const float_t v9 = (float_t)((1.1 - sqrtf(v8 * v8 + v7 * v7)) * lens_flare_power);
    if (v9 < 0.001f)
        return;

    const float_t v9a = v9 * v9;

    float_t angle = atan2f(v8, v7) - (float_t)M_PI_2;
    const float_t angle_sin = sinf(angle);
    const float_t angle_cos = cosf(angle);

    float_t* data;
    if (GLAD_GL_VERSION_4_5) {
        data = (float_t*)glMapNamedBuffer(lens_ghost_vbo, GL_WRITE_ONLY);
        if (!data) {
            glUnmapNamedBuffer(lens_ghost_vbo);
            return;
        }
    }
    else {
        gl_state_bind_array_buffer(lens_ghost_vbo);
        data = (float_t*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        if (!data) {
            glUnmapBuffer(GL_ARRAY_BUFFER);
            gl_state_bind_array_buffer(0);
            return;
        }
    }

    const float_t lens_ghost = tone_map->lens_ghost;
    const int32_t lens_ghost_count = this->lens_ghost_count;
    for (int32_t i = 0; i < lens_ghost_count; i++) {
        float_t opacity = v9 * v14[i] * lens_ghost;

        float_t scale = (v9a * 0.03f + 0.02f) * v15[i];

        mat4 mat;
        mat4_translate(v13[i] * v7 + 0.5f, v13[i] * v8 + 0.5f, 0.0f, &mat);
        mat4_scale_rot(&mat, scale, scale * aspect, 1.0f, &mat);
        mat4_mul_rotate_z(&mat, angle_sin, angle_cos, &mat);
        make_ghost_quad((uint8_t)(i & 0x03), opacity, &mat, data);
    }

    if (GLAD_GL_VERSION_4_5)
        glUnmapNamedBuffer(lens_ghost_vbo);
    else {
        glUnmapBuffer(GL_ARRAY_BUFFER);
        gl_state_bind_array_buffer(0);
    }

    rt->Bind();
    rt->SetViewport();
    gl_state_enable_blend();
    gl_state_set_blend_func(GL_ONE, GL_ONE);

    uniform_value[U_REDUCE] = 4;
    shaders_ft.set(SHADER_FT_REDUCE);
    gl_state_active_bind_texture_2d(0, lens_ghost_texture->tex);
    gl_state_bind_vertex_array(lens_ghost_vao);
    shaders_ft.draw_arrays(GL_TRIANGLES, 0, (GLsizei)(lens_ghost_count * 6LL));
    gl_state_bind_vertex_array(0);

    gl_state_disable_blend();
    gl_state_set_blend_func(GL_ONE, GL_ZERO);
}

void post_process::init_fbo(int32_t render_width, int32_t render_height,
    int32_t sprite_width, int32_t sprite_height, int32_t screen_width, int32_t screen_height) {
    if (this->render_width == render_width && this->render_height == render_height
        && this->sprite_width == sprite_width && this->sprite_height == sprite_height
        && this->screen_width == screen_width && this->screen_height == screen_height)
        return;

    if (this->render_width != render_width || this->render_height != render_height) {
        blur->init_fbo(render_width, render_height);
        dof->init_fbo(render_width, render_height);

        exposure->init_fbo();
        tone_map->init_fbo();

        rend_texture.Init(render_width, render_height, 0, GL_RGBA16F, GL_DEPTH_COMPONENT32F);
        buf_texture.Init(render_width, render_height, 0, GL_RGBA16F, 0);

        if (aet_back_tex)
            texture_free(aet_back_tex);
        aet_back_tex = texture_load_tex_2d(texture_id(0x25, texture_counter++), GL_RGBA8, render_width, render_height, 0, 0, false);
        aet_back_texture.SetColorDepthTextures(aet_back_tex->tex, 0, rend_texture.GetDepthTex());
        transparency_texture.Init(render_width, render_height, 0, GL_RGBA16F, 0);

        transparency->init_fbo(transparency_texture.GetColorTex(),
            rend_texture.GetDepthTex(), render_width, render_height);

        mlaa_buffer.Init(render_width, render_height, 0, GL_RGBA8, GL_DEPTH_COMPONENT32F);
        temp_buffer.Init(render_width, render_height, 0, GL_RGBA8, 0);
        sss_contour_texture = &mlaa_buffer;

        gl_state_bind_texture_2d(rend_texture.GetDepthTex());
        GLint swizzle[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);

        gl_state_bind_texture_2d(mlaa_buffer.GetDepthTex());
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
        gl_state_bind_texture_2d(0);

        this->render_width = render_width;
        this->render_height = render_height;
    }

    if (this->sprite_width != sprite_width || this->sprite_height != sprite_height) {
        screen_texture.Init(sprite_width, sprite_height, 0, GL_RGBA16F, 0); // Was GL_R11F_G11F_B10F
        this->sprite_width = sprite_width;
        this->sprite_height = sprite_height;
    }

    screen_x_offset = (screen_width - sprite_width) / 2 + (screen_width - sprite_width) % 2;
    screen_y_offset = (screen_height - sprite_height) / 2 + (screen_height - sprite_height) % 2;

    this->screen_width = screen_width;
    this->screen_height = screen_height;
}

bool post_process::frame_texture_cont_capture_set(bool value) {
    frame_texture[0].capture = value;
    return true;
}

void post_process::frame_texture_free() {
    for (post_process_frame_texture& i : frame_texture) {
        for (post_process_frame_texture_render_texture& j : i.render_textures) {
            j.texture = 0;
            j.render_texture.Free();
            j.type = POST_PROCESS_FRAME_TEXTURE_MAX;
        }

        i.capture = false;
    }
}

int32_t post_process::frame_texture_load(int32_t slot, post_process_frame_texture_type type, texture* tex) {
    if (!tex || slot < 0 || slot >= 6)
        return -1;

    for (post_process_frame_texture_render_texture& i : frame_texture[slot].render_textures)
        if (!i.texture) {
            i.render_texture.SetColorDepthTextures(tex->tex);
            i.texture = tex;
            i.type = type;
            return (int32_t)(&i - frame_texture[slot].render_textures);
        }
    return -1;
}

void post_process::frame_texture_reset() {
    for (post_process_frame_texture& i : frame_texture) {
        for (post_process_frame_texture_render_texture& j : i.render_textures) {
            j.texture = 0;
            j.type = POST_PROCESS_FRAME_TEXTURE_MAX;
        }

        i.capture = false;
    }
}

void post_process::frame_texture_reset_capture() {
    for (post_process_frame_texture& i : frame_texture)
        if (&i - frame_texture)
            i.capture = false;
}

bool post_process::frame_texture_slot_capture_set(int32_t index) {
    if (index < 1 || index >= 5)
        return false;

    frame_texture[index].capture = true;
    return true;
}

bool post_process::frame_texture_unload(int32_t slot, texture* tex) {
    if (!tex || slot < 0 || slot >= 6)
        return false;

    for (post_process_frame_texture_render_texture& i : frame_texture[slot].render_textures)
        if (i.texture == tex) {
            i.render_texture.Free();
            i.texture = 0;
            i.type = POST_PROCESS_FRAME_TEXTURE_MAX;
            return true;
        }
    return false;
}

int32_t post_process::movie_texture_set(texture* movie_texture) {
    if (!movie_texture)
        return -1;

    int32_t index = 0;
    while (movie_textures_data[index])
        if (++index >= 1)
            return -1;

    movie_textures_data[index] = movie_texture;
    movie_textures[index].SetColorDepthTextures(movie_texture->tex);
    return index;
}

void post_process::get_frame_texture(GLuint tex, post_process_frame_texture_type type) {
    for (post_process_frame_texture& i : frame_texture) {
        if (!i.capture)
            continue;

        for (auto& j : i.render_textures) {
            if (!j.texture || j.type != type || j.render_texture.Bind() < 0)
                continue;

            texture* dst_tex = j.texture;

            gl_state_active_bind_texture_2d(0, tex);
            gl_state_bind_sampler(0, samplers[0]);
            glViewport(0, 0, dst_tex->width, dst_tex->height);
            uniform_value[U_REDUCE] = 0;
            shaders_ft.set(SHADER_FT_REDUCE);
            RenderTexture::DrawQuad(&shaders_ft, dst_tex->width, dst_tex->height,
                1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
            gl_state_bind_framebuffer(0);
        }
    }
}

void post_process::movie_texture_free(texture* movie_texture) {
    if (!movie_texture)
        return;

    int32_t index = 0;
    while (movie_textures_data[index] != movie_texture)
        if (++index >= 1)
            return;

    movie_textures_data[index] = 0;
    movie_textures[index].Free();
}

int32_t post_process::render_texture_set(texture* render_texture, bool task_photo) {
    if (!render_texture)
        return -1;

    int32_t index = 0;
    if (task_photo)
        index = 15;
    else
        while (render_textures_data[index])
            if (++index >= 15)
                return -1;

    render_textures_data[index] = render_texture;
    render_textures[index].SetColorDepthTextures(render_texture->tex);
    return index;
}

void post_process::render_texture_free(texture* render_texture, bool task_photo) {
    if (!render_texture)
        return;

    int32_t index = 0;
    if (task_photo)
        index = 15;
    else
        while (render_textures_data[index] != render_texture)
            if (++index >= 15)
                return;

    render_textures_data[index] = 0;
    render_textures[index].Free();
}

void post_process::reset() {
    mlaa = true;
    mag_filter = POST_PROCESS_MAG_FILTER_BILINEAR;
    dof_debug_set(0);
    dof_pv_set(0);
    blur->initialize_data(2.0f, 1.0f);

    for (int32_t i = 0; i < 2; i++) {
        tone_map->reset_saturate_coeff(i, 0);
        tone_map->reset_scene_fade(i);
        tone_map->reset_tone_trans(i);
    }

    reset_exposure = true;
}

void post_process::set_render_texture(bool aet_back) {
    if (aet_back) {
        aet_back_texture.Bind();
        this->aet_back = 1;
    }
    else
        rend_texture.Bind();
    glViewport(0, 0, render_width, render_height);
}

#define MAX_EDGE_DETECTION_LEN (3)
#define GRID_SIDE_LEN (2 * MAX_EDGE_DETECTION_LEN + 1)
#define SIDE_LEN (5 * GRID_SIDE_LEN)

static void post_process_generate_area_texture(post_process* pp) {
    uint8_t* data = (uint8_t*)malloc(SIDE_LEN * SIDE_LEN * 2);
    if (!data)
        return;

    for (int32_t cross2 = 0; cross2 < 5; cross2++)
        for (int32_t cross1 = 0; cross1 < 5; cross1++)
            post_process_calculate_area_texture_data(data, cross1, cross2);

    glGenTextures(1, &pp->mlaa_area_texture);
    gl_state_bind_texture_2d(pp->mlaa_area_texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, SIDE_LEN, SIDE_LEN, 0, GL_RG, GL_UNSIGNED_BYTE, data);
    gl_state_bind_texture_2d(0);
    free(data);
}

static void post_process_calculate_area_texture_data(uint8_t* data, int32_t cross1, int32_t cross2) {
    uint8_t* _data = &data[(SIDE_LEN * GRID_SIDE_LEN * cross2 + GRID_SIDE_LEN * cross1) * 2];
    for (int32_t dright = 0; dright < GRID_SIDE_LEN; dright++) {
        for (int32_t dleft = 0; dleft < GRID_SIDE_LEN; dleft++) {
            float_t val_left = 0.0f;
            float_t val_right = 0.0f;
            post_process_calculate_area_texture_data_area(&val_left, &val_right, cross1, cross2, dleft, dright);
            _data[0] = (uint8_t)(val_left * 255.9f);
            _data[1] = (uint8_t)(val_right * 255.9f);
            _data += 2;
        }
        _data += GRID_SIDE_LEN * 4 * 2;
    }
}

static void post_process_calculate_area_texture_data_area(float_t* val_left,
    float_t* val_right, int32_t cross1, int32_t cross2, int32_t dleft, int32_t dright) {
    auto calc_area_tex_val = [&](int32_t a1, int32_t a2) {
        float_t v2 = (float_t)(-0.5 / ((float_t)a2 - 0.5));
        float_t v3 = (float_t)a1 * v2 + 0.5f;
        if (a1 >= a2 - 1)
            return (v3 * 0.5f) * 0.5f;
        else
            return ((float_t)(a1 + 1) * v2 + 0.5f + v3) * 0.5f;
    };

    int32_t dist = dleft + dright + 1;
    float_t _val_left = 0.0f;
    float_t _val_right = 0.0f;
    switch (cross2) {
    case 0:
        switch (cross1) {
        case 1:
            _val_right = calc_area_tex_val(dleft, min_def(dist, GRID_SIDE_LEN));
            break;
        case 3:
            _val_left = calc_area_tex_val(dleft, min_def(dist, GRID_SIDE_LEN));
            break;
        }
        break;
    case 1:
        switch (cross1) {
        case 0:
            _val_right = calc_area_tex_val(dright, min_def(dist, GRID_SIDE_LEN));
            break;
        case 1:
            if (dist % 2) {
                int32_t v12 = dist / 2 + 1;
                int32_t v15 = dist / 2;
                if (dleft < v15)
                    _val_right = calc_area_tex_val(dleft, v12);
                else if (dright < v15)
                    _val_right = calc_area_tex_val(dright, v12);
                else
                    _val_right = calc_area_tex_val(v15, v12) * 2.0f;
            }
            else {
                int32_t v12 = dist / 2;
                float_t v14 = -0.5f / (float_t)v12;
                if (dleft >= v12)
                    _val_right = ((float_t)(dright * 2 + 1) * v14 + 1.0f) * 0.5f;
                else
                    _val_right = ((float_t)(dleft * 2 + 1) * v14 + 1.0f) * 0.5f;
            }
            break;
        case 3:
        case 4:
            if (dist % 2) {
                int32_t v12 = dist / 2 + 1;
                int32_t v15 = dist / 2;
                if (dleft < v15)
                    _val_left = calc_area_tex_val(dleft, v12);
                else if (dright < v15)
                    _val_right = calc_area_tex_val(dright, v12);
                else
                    _val_left = _val_right = calc_area_tex_val(v15, v12);
            }
            else {
                int32_t v12 = dist / 2;
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
            _val_left = calc_area_tex_val(dright, min_def(dist, GRID_SIDE_LEN));
            break;
        case 1:
        case 4:
            if (dist % 2) {
                int32_t v12 = dist / 2 + 1;
                int32_t v15 = dist / 2;
                if (dleft < v15)
                    _val_right = calc_area_tex_val(dleft, v12);
                else if (dright < v15)
                    _val_left = calc_area_tex_val(dright, v12);
                else
                    _val_left = _val_right = calc_area_tex_val(v15, v12);
            }
            else {
                int32_t v12 = dist / 2;
                float_t v14 = -0.5f / (float_t)v12;
                if (dleft < v12)
                    _val_right = ((float_t)(dleft * 2 + 1) * v14 + 1.0f) * 0.5f;
                else
                    _val_left = ((float_t)(dright * 2 + 1) * v14 + 1.0f) * 0.5f;
            }
            break;
        case 3:
            if (dist % 2) {
                int32_t v12 = dist / 2 + 1;
                int32_t v15 = dist / 2;
                if (dleft < v15)
                    _val_left = calc_area_tex_val(dleft, v12);
                else if (dright < v15)
                    _val_left = calc_area_tex_val(dright, v12);
                else
                    _val_left = calc_area_tex_val(v15, v12) * 2.0f;
            }
            else {
                int32_t v12 = dist / 2;
                float_t v14 = -0.5f / (float_t)v12;
                if (dleft < v12)
                    _val_left = ((float_t)(dleft * 2 + 1) * v14 + 1.0f) * 0.5f;
                else
                    _val_left = ((float_t)(dright * 2 + 1) * v14 + 1.0f) * 0.5f;
            }
            break;
        }
        break;
    case 4:
        switch (cross1) {
        case 1:
            if (dist % 2) {
                int32_t v12 = dist / 2 + 1;
                int32_t v15 = dist / 2;
                if (dleft < v15)
                    _val_right = calc_area_tex_val(dleft, v12);
                else if (dright < v15)
                    _val_left = calc_area_tex_val(dright, v12);
                else
                    _val_left = _val_right = calc_area_tex_val(v15, v12);
            }
            else {
                int32_t v12 = dist / 2;
                float_t v14 = -0.5f / (float_t)v12;
                if (dleft < v12)
                    _val_right = ((float_t)(dleft * 2 + 1) * v14 + 1.0f) * 0.5f;
                else
                    _val_left = ((float_t)(dright * 2 + 1) * v14 + 1.0f) * 0.5f;
            }
            break;
        case 3:
            if (dist % 2) {
                int32_t v12 = dist / 2 + 1;
                int32_t v15 = dist / 2;
                if (dleft < v15)
                    _val_left = calc_area_tex_val(dleft, v12);
                else if (dright < v15)
                    _val_right = calc_area_tex_val(dright, v12);
                else
                    _val_left = _val_right = calc_area_tex_val(v15, v12);
            }
            else {
                int32_t v12 = dist / 2;
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

    *val_left = _val_left;
    *val_right = _val_right;
    return;
}
