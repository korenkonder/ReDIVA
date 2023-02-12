/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "shared.hpp"
#include "shader.hpp"
#include "texture.hpp"

struct render_texture {
    texture* color_texture;
    texture* depth_texture;
    int32_t binding;
    int32_t max_level;
    GLuint fbos[16];
    GLuint rbo;
    GLuint field_2C;

    render_texture();
    ~render_texture();

    int32_t bind(int32_t index = 0);
    void draw(bool depth = false);
    void free();
    int32_t init(int32_t width, int32_t height,
        int32_t max_level, GLenum color_format, GLenum depth_format);
    int32_t set_color_depth_textures(GLuint color_texture,
        int32_t max_level, GLuint depth_texture, bool stencil = false);

    static void draw(shader_set_data* set);
    static void draw_custom();
    static void draw_quad(shader_set_data* set, int32_t width, int32_t height, float_t scale = 1.0f,
        float_t param_x = 1.0f, float_t param_y = 1.0f, float_t param_z = 1.0f, float_t param_w = 1.0f);
};

extern void render_texture_opengl_data_init();
extern void render_texture_opengl_data_free();

extern void render_texture_vulkan_data_init(VkDevice device,
    VmaAllocator allocator, VkCommandPool command_pool, VkQueue queue);
extern void render_texture_vulkan_data_get_vertex_input_attribute_descriptions(
    const VkVertexInputAttributeDescription*& attribute_descriptions, uint32_t& attribute_description_count);
extern void render_texture_vulkan_data_get_vertex_input_binding_descriptions(
    const VkVertexInputBindingDescription*& binding_descriptions, uint32_t& binding_description_count);
extern void render_texture_vulkan_data_free(VkDevice device, VmaAllocator allocator);
