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

post_process::post_process() : ssaa(), mlaa(), ss_alpha_mask(), aet_back_tex(),
render_textures_data(), movie_textures_data(), aet_back(), texture_counter(), lens_shaft_query(),
lens_flare_query(), lens_shaft_query_data(), lens_flare_query_data(), lens_flare_query_index(),
lens_flare_texture(), lens_shaft_texture(), lens_ghost_texture(), lens_ghost_count(),
lens_flare_pos(), lens_shaft_scale(), lens_shaft_inv_scale(), lens_flare_power(),
field_A10(), lens_flare_appear_power(), render_width(), render_height(), view_point(),
interest(), view_point_prev(), interest_prev(), reset_exposure(), sprite_width(), sprite_height(),
screen_x_offset(), screen_y_offset(), screen_width(), screen_height(), mag_filter() {
    aa = new post_process_aa();
    blur = new post_process_blur();
    dof = new post_process_dof();
    exposure = new post_process_exposure();
    tone_map = new post_process_tone_map();
    stage_index = -1;
    stage_index_prev = -1;

    glGenSamplers(2, samplers);
    glSamplerParameteri(samplers[0], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(samplers[0], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(samplers[0], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(samplers[0], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(samplers[1], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(samplers[1], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(samplers[1], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(samplers[1], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    const float_t query_verts[] = {
        -1.0f, -1.0f, 0.15f, 0.15f,
         1.0f, -1.0f, 0.85f, 0.15f,
        -1.0f,  1.0f, 0.15f, 0.85f,
         1.0f,  1.0f, 0.85f, 0.85f,
    };

    glGenVertexArrays(1, &query_vao);
    gl_state_bind_vertex_array(query_vao);

    glGenBuffers(1, &query_vbo);
    gl_state_bind_array_buffer(query_vbo, true);
    if (GLAD_GL_VERSION_4_4)
        glBufferStorage(GL_ARRAY_BUFFER, sizeof(query_verts), query_verts, 0);
    else
        glBufferData(GL_ARRAY_BUFFER, sizeof(query_verts), query_verts, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 16, (void*)0);  // Pos
    glVertexAttrib4f(3, 1.0f, 1.0f, 1.0f, 1.0f);                    // Color0
    glVertexAttrib4f(4, 1.0f, 1.0f, 1.0f, 1.0f);                    // Color1
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, 16, (void*)8);  // TexCoord0

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
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 16, (void*)0);  // Pos
    glVertexAttrib4f(3, 1.0f, 1.0f, 1.0f, 1.0f);                    // Color0
    glVertexAttrib4f(4, 1.0f, 1.0f, 1.0f, 1.0f);                    // Color1
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, 16, (void*)8);  // TexCoord0
    gl_state_bind_array_buffer(0);
    gl_state_bind_vertex_array(0);

    if (!lens_shaft_query[0])
        glGenQueries(3, lens_shaft_query);

    if (!lens_flare_query[0])
        glGenQueries(3, lens_flare_query);

    sun_quad_ubo.Create(sizeof(sun_quad_shader_data));

    reset();
}

post_process::~post_process() {
    if (aet_back_tex)
        texture_free(aet_back_tex);

    if (aa) {
        delete aa;
        aa = 0;
    }

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

    if (samplers[0]) {
        glDeleteSamplers(2, samplers);
        samplers[0] = 0;
    }

    if (query_vao) {
        glDeleteVertexArrays(1, &query_vao);
        query_vao = 0;
    }

    if (query_vbo) {
        glDeleteBuffers(1, &query_vbo);
        query_vbo = 0;
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
    fbo::blit(rend_texture.fbos[0], pre_texture.fbos[0],
        0, 0, render_width, render_height,
        0, 0, render_width, render_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    for (int32_t i = 0; i < 8; i++)
        gl_state_bind_sampler(i, 0);

    dof->apply(&rend_texture, &buf_texture, samplers, cam);

    draw_lens_ghost(&rend_texture);

    blur->get_blur(&rend_texture);
    exposure->get_exposure(cam, render_width, render_height, reset_exposure,
        blur->tex[4].color_texture->tex, blur->tex[2].color_texture->tex);
    tone_map->apply(&rend_texture, light_proj_tex, (texture*)(aet_back ? aet_back_tex : 0),
        &rend_texture, &buf_texture,/* &sss_contour_texture,*/
        blur->tex[0].color_texture->tex,
        exposure->exposure.color_texture->tex, npr_param, this);
    if (mlaa)
        aa->apply_mlaa(&rend_texture, &buf_texture, samplers, ss_alpha_mask);

    for (int32_t i = 0; i < 16; i++) {
        if (!render_textures_data[i])
            continue;

        render_textures[i].bind();

        texture* t = render_textures_data[i];
        if (render_width > t->width * 2ULL || render_height > t->height * 2ULL)
            uniform_value[U_REDUCE] = 1;
        else
            uniform_value[U_REDUCE] = 0;

        glViewport(0, 0, t->width, t->height);
        gl_state_active_bind_texture_2d(0, rend_texture.color_texture->tex);
        gl_state_bind_sampler(0, samplers[0]);
        shaders_ft.set_opengl_shader(SHADER_FT_REDUCE);
        render_texture::draw_quad(&shaders_ft, render_width, render_height);
    }

    fbo::blit(rend_texture.fbos[0], post_texture.fbos[0],
        0, 0, render_width, render_height,
        0, 0, render_width, render_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    screen_texture.bind();
    glViewport(screen_x_offset, screen_y_offset, sprite_width, sprite_height);
    if (ssaa) {
        gl_state_active_bind_texture_2d(0, rend_texture.color_texture->tex);
        gl_state_bind_sampler(0, samplers[0]);
        uniform_value[U_ALPHA_MASK] = ss_alpha_mask ? 1 : 0;
        uniform_value[U_REDUCE] = 0;
        shaders_ft.set_opengl_shader(SHADER_FT_REDUCE);
        render_texture::draw_quad(&shaders_ft, render_width, render_height);
        uniform_value[U_ALPHA_MASK] = 0;
    }
    else {
        gl_state_active_bind_texture_2d(0, rend_texture.color_texture->tex);
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

        shaders_ft.set_opengl_shader(SHADER_FT_MAGNIFY);
        render_texture::draw_quad(&shaders_ft, render_width, render_height);
    }
    shader_opengl::unbind();

    for (int32_t i = 0; i < 8; i++)
        gl_state_bind_sampler(i, 0);

    fbo::blit(screen_texture.fbos[0], fbo_texture.fbos[0],
        screen_x_offset, screen_y_offset, sprite_width, sprite_height,
        0, 0, render_width, render_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
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

    mat4_mult_vec(&cam->view_projection, &v44, &v44);

    float_t v13 = 1.0f / v44.w;
    float_t v14 = v44.x * v13;
    float_t v15 = v44.y * v13;
    float_t v16 = (float_t)render_width * 0.01f / (float_t)render_height;
    v14 = clamp_def(v14, -0.99f, 0.99f);
    v15 = clamp_def(v15, v16 - 1.0f, 1.0f - v16);
    v44.x = v14 * v44.w;
    v44.y = v15 * v44.w;

    mat4_mult_vec(&cam->inv_view_projection, &v44, &v44);
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
    shaders_ft.set_opengl_shader(SHADER_FT_SUN);
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
    mat4_translate_mult(&cam->view, &position, &mat);
    mat4_clear_rot(&mat, &mat);
    mat4_scale_rot(&mat, v24 * 0.2f, &mat);

    mat4_transpose(&mat, &mat);
    shader_data.g_transform[0] = mat.row0;
    shader_data.g_transform[1] = mat.row1;
    shader_data.g_transform[2] = mat.row2;
    shader_data.g_transform[3] = mat.row3;
    sun_quad_ubo.WriteMapMemory(shader_data);

    glBeginQuery(GL_SAMPLES_PASSED, lens_shaft_query[next_query_index]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glEndQuery(GL_SAMPLES_PASSED);

    mat4_translate_mult(&cam->view, (vec3*)&v44, &mat);
    mat4_clear_rot(&mat, &mat);
    mat4_scale_rot(&mat, v24 * 2.0f, &mat);

    mat4_transpose(&mat, &mat);
    shader_data.g_transform[0] = mat.row0;
    shader_data.g_transform[1] = mat.row1;
    shader_data.g_transform[2] = mat.row2;
    shader_data.g_transform[3] = mat.row3;
    sun_quad_ubo.WriteMapMemory(shader_data);

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

        mat4_translate_mult(&cam->view, &position, &mat);
        mat4_clear_rot(&mat, &mat);
        mat4_scale_rot(&mat, v24 * 1.1f, &mat);

        shaders_ft.draw_arrays(GL_TRIANGLE_STRIP, 0, 4);

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
    mat4_mult_vec2_trans(mat, (vec2*)&p0, (vec2*)&p0);
    mat4_mult_vec2_trans(mat, (vec2*)&p1, (vec2*)&p1);
    mat4_mult_vec2_trans(mat, (vec2*)&p2, (vec2*)&p2);
    mat4_mult_vec2_trans(mat, (vec2*)&p3, (vec2*)&p3);

    *(vec4*)&data[0] = p0;
    data[4] = opacity;
    *(vec4*)&data[5] = p1;
    data[9] = opacity;
    *(vec4*)&data[10] = p2;
    data[14] = opacity;
    *(vec4*)&data[15] = p1;
    data[19] = opacity;
    *(vec4*)&data[20] = p2;
    data[24] = opacity;
    *(vec4*)&data[25] = p3;
    data[29] = opacity;
    data += 30;
}

void post_process::draw_lens_ghost(render_texture* rt) {
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

    const float_t lens_ghost = tone_map->data.lens_ghost;
    const int32_t lens_ghost_count = this->lens_ghost_count;
    for (int32_t i = 0; i < lens_ghost_count; i++) {
        float_t opacity = v9 * v14[i] * lens_ghost;

        float_t scale = (v9a * 0.03f + 0.02f) * v15[i];

        mat4 mat;
        mat4_translate(v13[i] * v7 + 0.5f, v13[i] * v8 + 0.5f, 0.0f, &mat);
        mat4_scale_rot(&mat, scale, scale * aspect, 1.0f, &mat);
        mat4_rotate_z_mult_sin_cos(&mat, angle_sin, angle_cos, &mat);
        make_ghost_quad((uint8_t)(i & 0x03), opacity, &mat, data);
    }

    if (GLAD_GL_VERSION_4_5)
        glUnmapNamedBuffer(lens_ghost_vbo);
    else {
        glUnmapBuffer(GL_ARRAY_BUFFER);
        gl_state_bind_array_buffer(0);
    }

    glViewport(0, 0, rt->color_texture->width, rt->color_texture->height);
    rt->bind();
    gl_state_enable_blend();
    gl_state_set_blend_func(GL_ONE, GL_ONE);

    uniform_value[U_REDUCE] = 4;
    shaders_ft.set_opengl_shader(SHADER_FT_REDUCE);
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
        && this->screen_width == screen_width && this->screen_height == screen_height)
        return;

    if (this->render_width != render_width || this->render_height != render_height) {
        aa->init_fbo(render_width, render_height);
        blur->init_fbo(render_width, render_height);
        dof->init_fbo(render_width, render_height);
        exposure->init_fbo();
        tone_map->init_fbo();
        rend_texture.init(render_width, render_height, 0, GL_RGBA16F, GL_DEPTH_COMPONENT32F);
        buf_texture.init(render_width, render_height, 0, GL_RGBA16F, 0);
        sss_contour_texture.init(render_width, render_height, 0, GL_RGBA8, GL_DEPTH_COMPONENT32F);
        if (aet_back_tex)
            texture_free(aet_back_tex);
        aet_back_tex = texture_load_tex_2d(texture_id(0x25, texture_counter++), GL_RGBA8, render_width, render_height, 0, 0, false);
        aet_back_texture.set_color_depth_textures(aet_back_tex->tex, 0, rend_texture.depth_texture->tex);
        pre_texture.init(render_width, render_height, 0, GL_RGBA16F, 0);
        post_texture.init(render_width, render_height, 0, GL_RGBA16F, 0);
        transparency_texture.init(render_width, render_height, 0, GL_RGBA16F, GL_DEPTH_COMPONENT32F);
        gl_state_bind_texture_2d(rend_texture.depth_texture->tex);
        GLint swizzle[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
        gl_state_bind_texture_2d(sss_contour_texture.depth_texture->tex);
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
        gl_state_bind_texture_2d(0);
        this->render_width = render_width;
        this->render_height = render_height;
    }

    this->sprite_width = sprite_width;
    this->sprite_height = sprite_height;

    screen_x_offset = (screen_width - sprite_width) / 2 + (screen_width - sprite_width) % 2;
    screen_y_offset = (screen_height - sprite_height) / 2 + (screen_height - sprite_height) % 2;

    if (this->screen_width != screen_width || this->screen_height != screen_height) {
        fbo_texture.init(screen_width, screen_height, 0, GL_RGBA16F, 0);
        screen_texture.init(screen_width, screen_height, 0, GL_RGBA16F, 0); // Was GL_R11F_G11F_B10F
        this->screen_width = screen_width;
        this->screen_height = screen_height;
    }
}

int32_t post_process::movie_texture_set(texture* movie_texture) {
    if (!movie_texture)
        return -1;

    int32_t index = 0;
    while (movie_textures_data[index])
        if (++index >= 1)
            return -1;

    movie_textures_data[index] = movie_texture;
    movie_textures[index].set_color_depth_textures(movie_texture->tex, 0, 0);
    return index;
}

void post_process::movie_texture_free(texture* movie_texture) {
    if (!movie_texture)
        return;

    int32_t index = 0;
    while (movie_textures_data[index] != movie_texture)
        if (++index >= 1)
            return;

    movie_textures_data[index] = 0;
    movie_textures[index].free();
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
    render_textures[index].set_color_depth_textures(render_texture->tex, 0, 0);
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
    render_textures[index].free();
}

void post_process::reset() {
    mlaa = true;
    mag_filter = POST_PROCESS_MAG_FILTER_BILINEAR;
    dof->initialize_data(0, 0);
    const vec3 radius = { 2.0f, 2.0f, 2.0f };
    const vec3 intensity = { 1.0f, 1.0f, 1.0f };
    blur->initialize_data(radius, intensity);
    const vec3 scene_fade_color = { 1.0f, 1.0f, 1.0f };
    const vec3 tone_trans_start = { 0.0f, 0.0f, 0.0f };
    const vec3 tone_trans_end = { 1.0f, 1.0f, 1.0f };
    tone_map->initialize_data(2.0f, true, 1.0f, 1, 1.0f, scene_fade_color, 0.0f,
        0, tone_trans_start, tone_trans_end, TONE_MAP_YCC_EXPONENT);
    reset_exposure = true;
}

void post_process::set_render_texture(bool aet_back) {
    if (aet_back) {
        aet_back_texture.bind();
        this->aet_back = 1;
    }
    else
        rend_texture.bind();
    glViewport(0, 0, render_width, render_height);
}
