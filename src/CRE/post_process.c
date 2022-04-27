/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "post_process.h"
#include "fbo.h"
#include "gl_state.h"
#include "render_texture.h"
#include "shader_ft.h"
#include "static_var.h"
#include "texture.h"

extern int32_t stage_index;

post_process::post_process() : ssaa(), mlaa(), parent_bone_node(), render_textures_data(),
movie_textures_data(), lens_flare_texture(), lens_shaft_texture(), lens_ghost_texture(),
lens_flare_count(), lens_flare_pos(), lens_shaft_scale(), lens_shaft_inv_scale(),
lens_flare_power(), field_A10(), lens_flare_appear_power(), render_width(), render_height(),
view_point(), interest(), view_point_prev(), interest_prev(), reset_exposure(), sprite_width(),
sprite_height(), screen_x_offset(), screen_y_offset(), screen_width(), screen_height(), mag_filter() {
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

    aa = post_process_aa_init();
    blur = post_process_blur_init();
    dof = post_process_dof_init();
    exposure = post_process_exposure_init();
    tone_map = post_process_tone_map_init();
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

    shader_glsl_param param;
    memset(&param, 0, sizeof(shader_glsl_param));
    param.name = "Alpha Layer";
    alpha_layer_shader.load(alpha_layer_vert_shader,
        alpha_layer_frag_shader, 0, &param);
    reset();
}

post_process::~post_process() {
    post_process_aa_dispose(aa);
    post_process_blur_dispose(blur);
    post_process_dof_dispose(dof);
    post_process_exposure_dispose(exposure);
    post_process_tone_map_dispose(tone_map);
    ::render_texture_free(&rend_texture);
    ::render_texture_free(&buf_texture);
    ::render_texture_free(&sss_contour_texture);
    ::render_texture_free(&pre_texture);
    ::render_texture_free(&post_texture);
    ::render_texture_free(&fbo_texture);
    ::render_texture_free(&alpha_layer_texture);
    ::render_texture_free(&screen_texture);
    alpha_layer_shader.unload();

    if (samplers[0]) {
        glDeleteSamplers(2, samplers);
        samplers[0] = 0;
    }
}

void post_process::apply(camera* cam, texture* light_proj_tex, int32_t npr_param) {
    fbo_blit(rend_texture.fbos[0],
        pre_texture.fbos[0],
        0, 0, render_width, render_height,
        0, 0, render_width, render_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    for (int32_t i = 0; i < 8; i++)
        gl_state_bind_sampler(i, 0);

    post_process_apply_dof(dof, &rend_texture, samplers, cam);

    shader_state_matrix_set_mvp_separate(&shaders_ft,
        &mat4_identity, &mat4_identity, &mat4_identity);

    post_process_get_blur(blur, &rend_texture);
    post_process_get_exposure(exposure, cam, render_width, render_height, reset_exposure,
        blur->tex[4].color_texture->tex, blur->tex[2].color_texture->tex);
    post_process_apply_tone_map(tone_map, &rend_texture, light_proj_tex, 0,
        &rend_texture, &buf_texture, &sss_contour_texture,
        blur->tex[0].color_texture->tex,
        exposure->exposure.color_texture->tex, npr_param);
    if (mlaa)
        post_process_apply_mlaa(aa, &rend_texture,
            &buf_texture, samplers, parent_bone_node);

    for (int32_t i = 0; i < 16; i++) {
        if (!render_textures_data[i])
            continue;

        render_texture_bind(&render_textures[i], 0);

        texture* t = render_textures_data[i];
        if (render_width > t->width * 2ULL || render_height > t->height * 2ULL)
            uniform_value[U_REDUCE] = 1;
        else
            uniform_value[U_REDUCE] = 0;

        glViewport(0, 0, t->width, t->height);
        gl_state_active_bind_texture_2d(0, rend_texture.color_texture->tex);
        gl_state_bind_sampler(0, samplers[0]);
        shader_set(&shaders_ft, SHADER_FT_REDUCE);
        render_texture_draw_params(&shaders_ft, render_width,
            render_height, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    }

    fbo_blit(rend_texture.fbos[0],
        post_texture.fbos[0],
        0, 0, render_width, render_height,
        0, 0, render_width, render_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    render_texture_bind(&screen_texture, 0);
    glViewport(screen_x_offset, screen_y_offset, sprite_width, sprite_height);
    if (ssaa) {
        gl_state_active_bind_texture_2d(0, rend_texture.color_texture->tex);
        gl_state_bind_sampler(0, samplers[0]);
        uniform_value[U01] = parent_bone_node ? 1 : 0;
        uniform_value[U_REDUCE] = 0;
        shader_set(&shaders_ft, SHADER_FT_REDUCE);
        render_texture_draw_params(&shaders_ft, render_width,
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

        shader_set(&shaders_ft, SHADER_FT_MAGNIFY);
        render_texture_draw_params(&shaders_ft, render_width,
            render_height, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    }
    shader_unbind();

    for (int32_t i = 0; i < 8; i++)
        gl_state_bind_sampler(i, 0);

    fbo_blit(screen_texture.fbos[0], fbo_texture.fbos[0],
        screen_x_offset, screen_y_offset, sprite_width, sprite_height,
        0, 0, render_width, render_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void post_process::ctrl(camera* cam) {
    view_point_prev = view_point;
    interest_prev = interest;
    camera_get_view_point(cam, &view_point);
    camera_get_interest(cam, &interest);

    stage_index_prev = stage_index;
    stage_index = ::stage_index;

    bool reset_exposure = cam->fast_change_hist1 && !cam->fast_change_hist0;
    this->reset_exposure = reset_exposure;
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

void post_process::init_fbo(int32_t render_width, int32_t render_height,
    int32_t sprite_width, int32_t sprite_height, int32_t screen_width, int32_t screen_height) {
    if (this->render_width == render_width && this->render_height == render_height
        && this->screen_width == screen_width && this->screen_height == screen_height)
        return;

    if (this->render_width != render_width || this->render_height != render_height) {
        post_process_aa_init_fbo(aa, render_width, render_height);
        post_process_blur_init_fbo(blur, render_width, render_height);
        post_process_dof_init_fbo(dof, render_width, render_height);
        post_process_exposure_init_fbo(exposure);
        post_process_tone_map_init_fbo(tone_map);
        render_texture_init(&rend_texture, render_width,
            render_height, 0, GL_RGBA16F, GL_DEPTH24_STENCIL8);
        render_texture_init(&buf_texture, render_width,
            render_height, 0, GL_RGBA16F, 0);
        render_texture_init(&sss_contour_texture, render_width,
            render_height, 0, GL_RGBA16F, GL_DEPTH24_STENCIL8);
        render_texture_init(&pre_texture, render_width,
            render_height, 0, GL_RGBA16F, 0);
        render_texture_init(&post_texture, render_width,
            render_height, 0, GL_RGBA16F, 0);
        render_texture_init(&alpha_layer_texture, render_width,
            render_height, 0, GL_RGBA16F, GL_DEPTH24_STENCIL8);
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
        render_texture_init(&fbo_texture, screen_width,
            screen_height, 0, GL_RGBA16F, 0);
        render_texture_init(&screen_texture, screen_width,
            screen_height, 0, GL_R11F_G11F_B10F, 0);
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
    render_texture_set_color_depth_textures(&movie_textures[index],
        movie_texture->tex, 0, 0, false);
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
    ::render_texture_free(&movie_textures[index]);
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
    render_texture_set_color_depth_textures(&render_textures[index],
        render_texture->tex, 0, 0, false);
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
    ::render_texture_free(&render_textures[index]);
}

void post_process::reset() {
    mlaa = true;
    mag_filter = POST_PROCESS_MAG_FILTER_BILINEAR;
    post_process_dof_initialize_data(dof, 0, 0);
    vec3 radius = { 2.0f, 2.0f, 2.0f };
    vec3 intensity = { 1.0f, 1.0f, 1.0f };
    post_process_blur_initialize_data(blur, &radius, &intensity);
    vec3 scene_fade_color = { 1.0f, 1.0f, 1.0f };
    vec3 tone_trans_start = { 0.0f, 0.0f, 0.0f };
    vec3 tone_trans_end = { 1.0f, 1.0f, 1.0f };
    post_process_tone_map_initialize_data(tone_map, 2.0f, true, 1.0f, 1, 1.0f,
        &scene_fade_color, 0.0f, 0, &tone_trans_start, &tone_trans_end, TONE_MAP_YCC_EXPONENT);
    reset_exposure = true;
}
