/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "shared.hpp"
#include "shader.hpp"
#include "texture.hpp"

struct RenderTexture {
    texture* color_texture;
    texture* depth_texture;
    int32_t binding;
    int32_t max_level;
    GLuint fbos[16];
    GLuint rbo;
    GLuint field_2C;

    RenderTexture();
    ~RenderTexture();

    int32_t Bind(int32_t index = 0);
    void Free();
    int32_t Init(int32_t width, int32_t height,
        int32_t max_level, GLenum color_format, GLenum depth_format);
    int32_t SetColorDepthTextures(GLuint color_texture,
        int32_t max_level = 0, GLuint depth_texture = 0, bool stencil = false);

    static void Draw(shader_set_data* set);
    static void DrawCustom();
    static void DrawQuad(shader_set_data* set,
        int32_t width, int32_t height, float_t scale_x = 1.0f, float_t scale_y = 1.0f,
        float_t param_x = 1.0f, float_t param_y = 1.0f, float_t param_z = 1.0f, float_t param_w = 1.0f);

    inline GLuint GetColorTex() {
        return color_texture->tex;
    };
    
    inline GLuint GetDepthTex() {
        return depth_texture->tex;
    };
    
    inline int32_t GetHeight() {
        return color_texture->height;
    };
    
    inline int32_t GetWidth() {
        return color_texture->width;
    };

    inline void SetViewport() {
        glViewport(0, 0, GetWidth(), GetHeight());
    };
};

extern void render_texture_data_init();
extern void render_texture_data_free();
