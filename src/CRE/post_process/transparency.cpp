/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "transparency.hpp"
#include "../../KKdLib/str_utils.hpp"
#include "../rob/rob.hpp"
#include "../gl_state.hpp"
#include "../shader_ft.hpp"

static void post_process_transparency_free_fbo(post_process_transparency* transparency);

post_process_transparency::post_process_transparency() : sampler(), vao() {

}

post_process_transparency::~post_process_transparency() {
    post_process_transparency_free_fbo(this);
}

void post_process_transparency::combine(RenderTexture* rt,
    RenderTexture* buf_rt, float_t alpha, void* rctx) {
    transparency_batch_shader_data shader_data = {};
    shader_data.g_opacity = { alpha, 0.0f, 0.0f, 0.0f };
    ((render_context*)rctx)->transparency_batch_ubo.WriteMemory(shader_data);

    gl_state_disable_blend();
    gl_state_disable_depth_test();
    buf_rt->Bind();
    shaders_ft.set(SHADER_FT_TRANSPARENCY);
    ((render_context*)rctx)->transparency_batch_ubo.Bind(0);
    gl_state_active_bind_texture_2d(0, fbo.textures[0]);
    gl_state_active_bind_texture_2d(1, rt->GetColorTex());
    RenderTexture::DrawCustom();
    gl_state_enable_depth_test();

    fbo::blit(buf_rt->fbos[0], rt->fbos[0],
        0, 0, buf_rt->GetWidth(), buf_rt->GetHeight(),
        0, 0, rt->GetWidth(), rt->GetHeight(),
        GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void post_process_transparency::copy(RenderTexture* rt) {
    fbo::blit(rt->fbos[0], fbo.buffer,
        0, 0, rt->GetWidth(), rt->GetHeight(),
        0, 0, fbo.width, fbo.height,
        GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void post_process_transparency::init_fbo(GLuint color_texture, GLuint depth_texture, int32_t width, int32_t height) {
    if (fbo.textures && (fbo.textures[0] == color_texture
        && fbo.textures[1] == depth_texture
        && fbo.width == width && fbo.height == height))
        return;

    post_process_transparency_free_fbo(this);

    GLuint color_textures[1];
    color_textures[0] = color_texture;
    fbo.init_data(width, height, color_textures, 1, depth_texture);

    glGenSamplers(1, &sampler);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenVertexArrays(1, &vao);
}

static void post_process_transparency_free_fbo(post_process_transparency* transparency) {
    if (transparency->vao) {
        glDeleteVertexArrays(1, &transparency->vao);
        transparency->vao = 0;
    }

    if (transparency->sampler) {
        glDeleteSamplers(1, &transparency->sampler);
        transparency->sampler = 0;
    }

    transparency->fbo.free_data();
}
