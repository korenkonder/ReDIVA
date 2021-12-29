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

void post_process_init(post_process_struct* pp) {
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

    memset(pp, 0, sizeof(post_process_struct));
    pp->aa = post_process_aa_init();
    pp->blur = post_process_blur_init();
    pp->dof = post_process_dof_init();
    pp->exposure = post_process_exposure_init();
    pp->tone_map = post_process_tone_map_init();

    glGenSamplers(2, pp->samplers);
    glSamplerParameteri(pp->samplers[0], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(pp->samplers[0], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(pp->samplers[0], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glSamplerParameteri(pp->samplers[0], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glSamplerParameteri(pp->samplers[1], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(pp->samplers[1], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(pp->samplers[1], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glSamplerParameteri(pp->samplers[1], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    shader_glsl_param param;
    memset(&param, 0, sizeof(shader_glsl_param));
    param.name = "Alpha Layer";
    shader_glsl_load_string(&pp->alpha_layer_shader,
        (char*)alpha_layer_vert_shader, (char*)alpha_layer_frag_shader, 0, &param);
    post_process_reset(pp);
}

void post_process_apply(post_process_struct* pp, camera* cam, texture* light_proj_tex, int32_t npr_param) {
    fbo_blit(pp->render_texture.fbos[0],
        pp->pre_texture.fbos[0],
        0, 0, pp->render_width, pp->render_height,
        0, 0, pp->render_width, pp->render_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    for (int32_t i = 0; i < 8; i++)
        gl_state_bind_sampler(i, 0);

    post_process_apply_dof(pp->dof, &pp->render_texture, pp->samplers, cam);

    shader_state_matrix_set_mvp_separate(&shaders_ft,
        (mat4*)&mat4_identity, (mat4*)&mat4_identity, (mat4*)&mat4_identity);

    post_process_get_blur(pp->blur, &pp->render_texture);
    post_process_get_exposure(pp->exposure,
        pp->blur->tex[4].color_texture->texture, pp->blur->tex[2].color_texture->texture);
    post_process_apply_tone_map(pp->tone_map, &pp->render_texture, light_proj_tex, 0,
        &pp->render_texture, &pp->buf_texture, &pp->sss_contour_texture,
        pp->blur->tex[0].color_texture->texture,
        pp->exposure->exposure.color_texture->texture, npr_param);
    if (pp->mlaa)
        post_process_apply_mlaa(pp->aa, &pp->render_texture,
            &pp->buf_texture, pp->samplers, pp->parent_bone_node);

    for (int32_t i = 0; i < 16; i++) {
        if (!pp->render_textures_data[i])
            continue;

        render_texture_bind(&pp->render_textures[i], 0);

        texture* t = pp->render_textures_data[i];
        if (pp->render_width > t->width * 2ULL || pp->render_height > t->height * 2ULL)
            uniform_value[U_REDUCE] = 1;
        else
            uniform_value[U_REDUCE] = 0;

        glViewport(0, 0, t->width, t->height);
        gl_state_active_bind_texture_2d(0, pp->render_texture.color_texture->texture);
        gl_state_bind_sampler(0, pp->samplers[0]);
        shader_set(&shaders_ft, SHADER_FT_REDUCE);
        render_texture_draw_params(&shaders_ft, pp->render_width,
            pp->render_height, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    }

    render_texture_bind(&pp->screen_texture, 0);
    glViewport(pp->screen_x_offset, pp->screen_y_offset, pp->sprite_width, pp->sprite_height);
    if (pp->ssaa) {
        gl_state_active_bind_texture_2d(0, pp->render_texture.color_texture->texture);
        gl_state_bind_sampler(0, pp->samplers[0]);
        uniform_value[U01] = pp->parent_bone_node ? 1 : 0;
        uniform_value[U_REDUCE] = 0;
        shader_set(&shaders_ft, SHADER_FT_REDUCE);
        render_texture_draw_params(&shaders_ft, pp->render_width,
            pp->render_height, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        uniform_value[U01] = 0;
    }
    else {
        gl_state_active_bind_texture_2d(0, pp->render_texture.color_texture->texture);
        if (pp->mag_filter == POST_PROCESS_MAG_FILTER_NEAREST)
            gl_state_bind_sampler(0, pp->samplers[1]);
        else
            gl_state_bind_sampler(0, pp->samplers[0]);

        switch (pp->mag_filter) {
        case POST_PROCESS_MAG_FILTER_NEAREST:
        case POST_PROCESS_MAG_FILTER_BILINEAR:
        case POST_PROCESS_MAG_FILTER_SHARPEN_5_TAP:
        case POST_PROCESS_MAG_FILTER_SHARPEN_4_TAP:
        case POST_PROCESS_MAG_FILTER_CONE_4_TAP:
        case POST_PROCESS_MAG_FILTER_CONE_2_TAP:
            uniform_value[U_MAGNIFY] = pp->mag_filter;
            break;
        default:
            uniform_value[U_MAGNIFY] = 0;
            break;
        }

        shader_set(&shaders_ft, SHADER_FT_MAGNIFY);
        render_texture_draw_params(&shaders_ft, pp->render_width,
            pp->render_height, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    }
    shader_unbind();

    for (int32_t i = 0; i < 8; i++)
        gl_state_bind_sampler(i, 0);

    fbo_blit(pp->screen_texture.fbos[0], pp->post_texture.fbos[0],
        pp->screen_x_offset, pp->screen_y_offset, pp->sprite_width, pp->sprite_height,
        0, 0, pp->render_width, pp->render_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void post_process_init_fbo(post_process_struct* pp, int32_t render_width, int32_t render_height,
    int32_t sprite_width, int32_t sprite_height, int32_t screen_width, int32_t screen_height) {
    if (pp->render_width == render_width && pp->render_height == render_height
        && pp->screen_width == screen_width && pp->screen_height == screen_height)
        return;

    if (pp->render_width != render_width || pp->render_height != render_height) {
        post_process_aa_init_fbo(pp->aa, render_width, render_height);
        post_process_blur_init_fbo(pp->blur, render_width, render_height);
        post_process_dof_init_fbo(pp->dof, render_width, render_height);
        post_process_exposure_init_fbo(pp->exposure);
        post_process_tone_map_init_fbo(pp->tone_map);
        render_texture_init(&pp->render_texture, render_width,
            render_height, 0, GL_RGBA16F, GL_DEPTH24_STENCIL8);
        render_texture_init(&pp->buf_texture, render_width,
            render_height, 0, GL_RGBA16F, 0);
        render_texture_init(&pp->sss_contour_texture, render_width,
            render_height, 0, GL_RGBA16F, GL_DEPTH24_STENCIL8);
        render_texture_init(&pp->pre_texture, render_width,
            render_height, 0, GL_RGBA16F, 0);
        render_texture_init(&pp->post_texture, render_width,
            render_height, 0, GL_RGBA16F, 0);
        render_texture_init(&pp->fbo_texture, render_width,
            render_height, 0, GL_RGBA16F, 0);
        render_texture_init(&pp->alpha_layer_texture, render_width,
            render_height, 0, GL_RGBA16F, GL_DEPTH24_STENCIL8);
        gl_state_bind_texture_2d(pp->render_texture.depth_texture->texture);
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA,
            (GLint[]) { GL_RED, GL_RED, GL_RED, GL_ONE });
        gl_state_bind_texture_2d(pp->sss_contour_texture.depth_texture->texture);
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA,
            (GLint[]) { GL_RED, GL_RED, GL_RED, GL_ONE });
        gl_state_bind_texture_2d(0);
        pp->render_width = render_width;
        pp->render_height = render_height;
    }

    pp->sprite_width = sprite_width;
    pp->sprite_height = sprite_height;

    pp->screen_x_offset = (screen_width - sprite_width) / 2 + (screen_width - sprite_width) % 2;
    pp->screen_y_offset = (screen_height - sprite_height) / 2 + (screen_height - sprite_height) % 2;

    if (pp->screen_width != screen_width || pp->screen_height != screen_height) {
        render_texture_init(&pp->screen_texture, screen_width,
            screen_height, 0, GL_R11F_G11F_B10F, 0);
        pp->screen_width = screen_width;
        pp->screen_height = screen_height;
    }
}

int32_t post_process_movie_texture_set(post_process_struct* pp, texture* movie_texture) {
    if (!movie_texture)
        return -1;

    int32_t index = 0;
    while (pp->movie_textures_data[index])
        if (++index >= 1)
            return -1;

    pp->movie_textures_data[index] = movie_texture;
    render_texture_set_color_depth_textures(&pp->movie_textures[index],
        movie_texture->texture, 0, 0, false);
    return index;
}

void post_process_movie_texture_free(post_process_struct* pp, texture* movie_texture) {
    if (!movie_texture)
        return;

    int32_t index = 0;
    while (pp->movie_textures_data[index] != movie_texture)
        if (++index >= 1)
            return;

    pp->movie_textures_data[index] = 0;
    render_texture_free(&pp->movie_textures[index]);
}

int32_t post_process_render_texture_set(post_process_struct* pp, texture* render_texture, bool task_photo) {
    if (!render_texture)
        return -1;

    int32_t index = 0;
    if (task_photo)
        index = 15;
    else
        while (pp->render_textures_data[index])
            if (++index >= 15)
                return -1;

    pp->render_textures_data[index] = render_texture;
    render_texture_set_color_depth_textures(&pp->render_textures[index],
        render_texture->texture, 0, 0, false);
    return index;
}

void post_process_render_texture_free(post_process_struct* pp, texture* render_texture, bool task_photo) {
    if (!render_texture)
        return;

    int32_t index = 0;
    if (task_photo)
        index = 15;
    else
        while (pp->render_textures_data[index] != render_texture)
            if (++index >= 15)
                return;

    pp->render_textures_data[index] = 0;
    render_texture_free(&pp->render_textures[index]);
}

void post_process_reset(post_process_struct* pp) {
    pp->mlaa = true;
    pp->mag_filter = POST_PROCESS_MAG_FILTER_BILINEAR;
    post_process_dof_initialize_data(pp->dof, 0, 0);
    post_process_blur_initialize_data(pp->blur,
        &((vec3) { 2.0f, 2.0f, 2.0f }), &((vec3) { 1.0f, 1.0f, 1.0f }));
    post_process_tone_map_initialize_data(pp->tone_map, 2.0f, true,
        1.0f, 1, 1.0f, &((vec3) { 0.0f, 0.0f, 0.0f }), 0.0f, 0,
        &((vec3) { 0.0f, 0.0f, 0.0f }), &((vec3) { 1.0f, 1.0f, 1.0f }),
        TONE_MAP_YCC_EXPONENT);
}

void post_process_free(post_process_struct* pp) {
    post_process_aa_dispose(pp->aa);
    post_process_blur_dispose(pp->blur);
    post_process_dof_dispose(pp->dof);
    post_process_exposure_dispose(pp->exposure);
    post_process_tone_map_dispose(pp->tone_map);
    render_texture_free(&pp->render_texture);
    render_texture_free(&pp->buf_texture);
    render_texture_free(&pp->sss_contour_texture);
    render_texture_free(&pp->pre_texture);
    render_texture_free(&pp->post_texture);
    render_texture_free(&pp->fbo_texture);
    render_texture_free(&pp->alpha_layer_texture);
    render_texture_free(&pp->screen_texture);
    shader_glsl_free(&pp->alpha_layer_shader);

    if (pp->samplers[0]) {
        glDeleteSamplers(2, pp->samplers);
        pp->samplers[0] = 0;
    }
}
