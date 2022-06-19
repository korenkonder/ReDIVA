/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "post_process.hpp"
#include "fbo.hpp"
#include "gl_state.hpp"
#include "render_texture.hpp"
#include "shader_ft.hpp"
#include "stage.hpp"
#include "stage_modern.hpp"
#include "static_var.hpp"
#include "texture.hpp"

extern bool task_stage_is_modern;

post_process::post_process() : ssaa(), mlaa(), parent_bone_node(), render_textures_data(),
movie_textures_data(), lens_flare_texture(), lens_shaft_texture(), lens_ghost_texture(),
lens_flare_count(), lens_flare_pos(), lens_shaft_scale(), lens_shaft_inv_scale(),
lens_flare_power(), field_A10(), lens_flare_appear_power(), render_width(), render_height(),
view_point(), interest(), view_point_prev(), interest_prev(), reset_exposure(), sprite_width(),
sprite_height(), screen_x_offset(), screen_y_offset(), screen_width(), screen_height(), mag_filter() {
    static const char* query_vert_shader =
        "#version 430 core\n"
        "layout(location = 0) in vec4 a_position;\n"
        "\n"
        "uniform mat4 mat;\n"
        "uniform float scale;\n"
        "\n"
        "void main() {\n"
        "    vec4 pos = a_position;\n"
        "    pos.xy *= scale;\n"
        "    gl_Position = mat * pos;\n"
        "}\n";

    static const char* query_frag_shader =
        "#version 430 core\n"
        "layout(location = 0) out vec4 result;\n"
        "\n"
        "void main() {\n"
        "    result = vec4(0.0); \n"
        "}\n";

    static const char* alpha_layer_vert_shader =
        "#version 430 core\n"
        "out VertexData {\n"
        "    vec2 texcoord;\n"
        "} result;\n"
        "\n"
        "void main() {\n"
        "    gl_Position.x = -1.0 + float(gl_VertexID / 2) * 4.0;\n"
        "    gl_Position.y = 1.0 - float(gl_VertexID % 2) * 4.0;\n"
        "    gl_Position.z = 0.0;\n"
        "    gl_Position.w = 1.0;\n"
        "    result.texcoord = gl_Position.xy * 0.5 + 0.5;\n"
        "}\n";

    static const char* alpha_layer_frag_shader =
        "#version 430\n"
        "layout(binding = 0) uniform sampler2D g_layerd_color;\n"
        "layout(binding = 1) uniform sampler2D g_base_color;\n"
        "\n"
        "layout(location = 0) uniform float g_opacity;\n"
        "\n"
        "in VertexData{\n"
        "    vec2 texcoord;\n"
        "}frg;\n"
        "\n"
        "layout(location = 0) out vec4 result;\n"
        "\n"
        "void main(){\n"
        "    vec4 cl = textureLod(g_layerd_color, frg.texcoord, 0.f);\n"
        "    vec4 cb = textureLod(g_base_color, frg.texcoord, 0.f);\n"
        "    result = mix(cb, cl, g_opacity.x);\n"
        "}\n";

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
    glSamplerParameteri(samplers[0], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glSamplerParameteri(samplers[0], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glSamplerParameteri(samplers[1], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(samplers[1], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(samplers[1], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glSamplerParameteri(samplers[1], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    const float_t verts_quad[] = {
        -1.0f,  1.0f, 0.15f, 0.85f,
        -1.0f, -1.0f, 0.15f, 0.15f,
         1.0f, -1.0f, 0.85f, 0.15f,
         1.0f,  1.0f, 0.85f, 0.85f,
    };

    glGenVertexArrays(1, &query_vao);
    glGenBuffers(1, &query_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, query_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts_quad), verts_quad, GL_STATIC_DRAW);

    gl_state_bind_vertex_array(query_vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 16, (void*)0);  // Pos
    glVertexAttrib4f(3, 1.0f, 1.0f, 1.0f, 1.0f);                    // Color0
    glVertexAttrib4f(4, 1.0f, 1.0f, 1.0f, 1.0f);                    // Color1
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, 16, (void*)8);  // TexCoord0
    gl_state_bind_vertex_array(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    shader_glsl_param param = {};
    param.name = "Exposure Chara";
    query_shader.load(query_vert_shader,
        query_frag_shader, 0, &param);

    param = {};
    param.name = "Alpha Layer";
    alpha_layer_shader.load(alpha_layer_vert_shader,
        alpha_layer_frag_shader, 0, &param);
    reset();
}

post_process::~post_process() {
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

    query_shader.unload();
    alpha_layer_shader.unload();
}

void post_process::apply(camera* cam, texture* light_proj_tex, int32_t npr_param) {
    fbo::blit(rend_texture.fbos[0], pre_texture.fbos[0],
        0, 0, render_width, render_height,
        0, 0, render_width, render_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    for (int32_t i = 0; i < 8; i++)
        gl_state_bind_sampler(i, 0);

    dof->apply(&rend_texture, samplers, cam);

    shaders_ft.state_matrix_set_mvp(mat4_identity, mat4_identity, mat4_identity);

    blur->get_blur(&rend_texture);
    exposure->get_exposure(cam, render_width, render_height, reset_exposure,
        blur->tex[4].color_texture->tex, blur->tex[2].color_texture->tex);
    tone_map->apply(&rend_texture, light_proj_tex, 0,
        &rend_texture, &buf_texture, &sss_contour_texture,
        blur->tex[0].color_texture->tex,
        exposure->exposure.color_texture->tex, npr_param);
    if (mlaa)
        aa->apply_mlaa(&rend_texture, &buf_texture, samplers, parent_bone_node);

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
        shaders_ft.set(SHADER_FT_REDUCE);
        render_texture::draw_params(&shaders_ft, render_width,
            render_height, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    }

    fbo::blit(rend_texture.fbos[0], post_texture.fbos[0],
        0, 0, render_width, render_height,
        0, 0, render_width, render_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    screen_texture.bind();
    glViewport(screen_x_offset, screen_y_offset, sprite_width, sprite_height);
    if (ssaa) {
        gl_state_active_bind_texture_2d(0, rend_texture.color_texture->tex);
        gl_state_bind_sampler(0, samplers[0]);
        uniform_value[U01] = parent_bone_node ? 1 : 0;
        uniform_value[U_REDUCE] = 0;
        shaders_ft.set(SHADER_FT_REDUCE);
        render_texture::draw_params(&shaders_ft, render_width,
            render_height, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        uniform_value[U01] = 0;
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
        case POST_PROCESS_MAG_FILTER_SHARPEN_5_TAP:
        case POST_PROCESS_MAG_FILTER_SHARPEN_4_TAP:
        case POST_PROCESS_MAG_FILTER_CONE_4_TAP:
        case POST_PROCESS_MAG_FILTER_CONE_2_TAP:
            uniform_value[U_MAGNIFY] = mag_filter;
            break;
        default:
            uniform_value[U_MAGNIFY] = 0;
            break;
        }

        shaders_ft.set(SHADER_FT_MAGNIFY);
        render_texture::draw_params(&shaders_ft, render_width,
            render_height, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    }
    shader::unbind();

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
        float_t view_point_dist;
        vec3_distance(view_point, view_point_prev, view_point_dist);

        vec3 dir;
        vec3_sub(interest, view_point, dir);
        vec3_normalize(dir, dir);

        vec3 dir_prev;
        vec3_sub(interest_prev, view_point_prev, dir_prev);
        vec3_normalize(dir_prev, dir_prev);

        float_t dir_diff_angle;
        vec3_dot(dir, dir_prev, dir_diff_angle);
        if (dir_diff_angle < 0.5f)
            dir_diff_angle = 0.0f;
        if (view_point_dist < dir_diff_angle * 0.4f)
            reset_exposure = false;
    }
    else if (stage_index != stage_index_prev)
        reset_exposure = true;
}

void post_process::draw_query_samples(GLuint query, float_t scale, mat4& mat) {
    glBeginQuery(GL_SAMPLES_PASSED, query);
    gl_state_bind_vertex_array(query_vao);
    query_shader.set("mat", false, mat);
    query_shader.set("scale", scale);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    gl_state_bind_vertex_array(0);
    glEndQuery(GL_SAMPLES_PASSED);
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
        rend_texture.init(render_width, render_height, 0, GL_RGBA16F, GL_DEPTH24_STENCIL8);
        buf_texture.init(render_width, render_height, 0, GL_RGBA16F, 0);
        sss_contour_texture.init(render_width, render_height, 0, GL_RGBA16F, GL_DEPTH24_STENCIL8);
        pre_texture.init(render_width, render_height, 0, GL_RGBA16F, 0);
        post_texture.init(render_width, render_height, 0, GL_RGBA16F, 0);
        alpha_layer_texture.init(render_width, render_height, 0, GL_RGBA16F, GL_DEPTH24_STENCIL8);
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
        screen_texture.init(screen_width, screen_height, 0, GL_R11F_G11F_B10F, 0);
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
    movie_textures[index].set_color_depth_textures(movie_texture->tex, 0, 0, false);
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
    movie_textures[index].free_data();
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
    render_textures[index].set_color_depth_textures(render_texture->tex, 0, 0, false);
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
    render_textures[index].free_data();
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
