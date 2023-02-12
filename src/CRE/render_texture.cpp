/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "render_texture.hpp"
#include "Vulkan/buffer.hpp"
#include "gl_state.hpp"
#include "render_context.hpp"
#include "texture.hpp"

static GLuint render_texture_vao;

static VkVertexInputAttributeDescription render_texture_vertex_input_attribute_descriptions[16];
static VkVertexInputBindingDescription render_texture_vertex_input_binding_descriptions[2];
static Vulkan::Buffer render_texture_vertex_data;
static Vulkan::Buffer render_texture_dummy_data;

static bool render_texture_data_initialized;
static uint32_t render_texture_counter;

static int32_t render_texture_framebuffer_init(render_texture* rt, int32_t max_level);
static int32_t render_texture_framebuffer_set_texture(render_texture* rrt,
    GLuint color_texture, int32_t level, GLuint depth_texture, bool stencil);

render_texture::render_texture() : color_texture(),
depth_texture(), binding(), max_level(), fbos(), rbo(), field_2C() {

}

render_texture::~render_texture() {
    free();
}

int32_t render_texture::bind(int32_t index) {
    if (index < 0 || index > max_level)
        return -1;

    gl_state_bind_framebuffer(fbos[index]);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return -1;

    glGetError();
    return 0;
}

void render_texture::draw(bool depth) {
    gl_state_active_bind_texture_2d(0, color_texture->tex);
    if (depth && depth_texture->tex)
        gl_state_active_bind_texture_2d(1, depth_texture->tex);
    gl_state_bind_vertex_array(render_texture_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    gl_state_bind_vertex_array(0);
}

void render_texture::free() {
    if (depth_texture) {
        texture_free(depth_texture);
        depth_texture = 0;
    }

    if (color_texture) {
        texture_free(color_texture);
        color_texture = 0;
    }

    if (rbo) {
        glDeleteRenderbuffers(1, &rbo);
        rbo = 0;
    }

    if (field_2C) {
        glDeleteRenderbuffers(1, &field_2C);
        field_2C = 0;
    }

    if (fbos[0]) {
        glDeleteFramebuffers(max_level + 1, fbos);
        memset(fbos, 0, sizeof(fbos));
    }
    max_level = 0;
}

int32_t render_texture::init(int32_t width, int32_t height,
    int32_t max_level, GLenum color_format, GLenum depth_format) {
    if (max_level < 0)
        return -1;

    max_level = min_def(max_level, 15);
    if (max_level < 0)
        return 0;
    free();

    GLuint color_texture;
    if (color_format) {
        this->color_texture = texture_load_tex_2d(texture_id(0x23, render_texture_counter),
            color_format, width, height, max_level, 0, 0);
        if (!this->color_texture)
            return -1;

        render_texture_counter++;
        color_texture = this->color_texture->tex;
        gl_state_bind_texture_2d(this->color_texture->tex);
        if (color_format == GL_RGBA32F) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        gl_state_bind_texture_2d(0);
    }
    else {
        this->color_texture = 0;
        color_texture = 0;
    }

    GLuint depth_texture;
    bool stencil;
    if (depth_format) {
        this->depth_texture = texture_load_tex_2d(texture_id(0x23, render_texture_counter),
            depth_format, width, height, 0, 0, 0);
        if (!this->depth_texture)
            return -1;

        render_texture_counter++;
        depth_texture = this->depth_texture->tex;
        stencil = depth_format == GL_DEPTH24_STENCIL8;
    }
    else {
        this->depth_texture = 0;
        depth_texture = 0;
        stencil = false;
    }
    this->max_level = max_level;

    if (render_texture_framebuffer_init(this, max_level) >= 0) {
        for (int32_t i = 0; i <= max_level; i++)
            if (render_texture_framebuffer_set_texture(this, color_texture, i, depth_texture, stencil) < 0)
                return -1;
    }
    return 0;
}

int32_t render_texture::set_color_depth_textures(GLuint color_texture,
    int32_t max_level, GLuint depth_texture, bool stencil) {
    int32_t error = 0;
    this->max_level = max_level;
    if (!fbos[0])
        error = render_texture_framebuffer_init(this, max_level);
    render_texture_framebuffer_set_texture(this, color_texture, max_level, depth_texture, stencil);
    return error;
}

void render_texture::draw(shader_set_data* set) {
    gl_state_bind_vertex_array(render_texture_vao);
    set->draw_arrays(GL_TRIANGLE_STRIP, 0, 4);
}

void render_texture::draw_custom() {
    gl_state_bind_vertex_array(render_texture_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void render_texture::draw_quad(shader_set_data* set, int32_t width, int32_t height,
    float_t scale, float_t param_x, float_t param_y, float_t param_z, float_t param_w) {
    extern render_context* rctx_ptr;

    float_t w = (float_t)max_def(width, 1);
    float_t h = (float_t)max_def(height, 1);
    quad_shader_data quad = {};
    quad.g_texcoord_modifier = { 0.5f, 0.5f, 0.5f, 0.5f }; // x * 0.5 + 0.5
    quad.g_texel_size = { scale / w, scale / h, w, h };
    quad.g_color = { param_x, param_y, param_z, param_w };
    quad.g_texture_lod = 0.0f;

    rctx_ptr->quad_ubo.WriteMapMemory(quad);
    rctx_ptr->quad_ubo.Bind(0);
    gl_state_bind_vertex_array(render_texture_vao);
    set->draw_arrays(GL_TRIANGLE_STRIP, 0, 4);
}

void render_texture_opengl_data_init() {
    if (render_texture_data_initialized)
        return;

    glGenVertexArrays(1, &render_texture_vao);

    render_texture_data_initialized = true;
}

void render_texture_opengl_data_free() {
    if (!render_texture_data_initialized)
        return;

    glDeleteVertexArrays(1, &render_texture_vao);

    render_texture_data_initialized = false;
}

void render_texture_vulkan_data_init(VkDevice device,
    VmaAllocator allocator, VkCommandPool command_pool, VkQueue queue) {
    if (render_texture_data_initialized)
        return;

    const float_t vertex_data[] = {
        -1.0f,  1.0f,  0.0f,  1.0f,
        -1.0f, -3.0f,  0.0f, -1.0f,
         3.0f,  1.0f,  2.0f,  1.0f,
    };
    
    const float_t dummy_data[] = {
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
    };

    const size_t vertex_data_size = sizeof(vertex_data);
    const size_t dummy_data_size = sizeof(dummy_data);

    VkVertexInputAttributeDescription* attribute_descriptions = render_texture_vertex_input_attribute_descriptions;
    attribute_descriptions[0] = { 0, 0, VK_FORMAT_R32G32_SFLOAT, 0 };
    attribute_descriptions[1] = { 1, 1, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(float_t) * 4 };
    attribute_descriptions[2] = { 2, 1, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(float_t) * 4 };
    attribute_descriptions[3] = { 3, 1, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(float_t) * 8 };
    attribute_descriptions[4] = { 4, 1, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(float_t) * 8 };
    attribute_descriptions[5] = { 5, 1, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(float_t) * 4 };
    attribute_descriptions[6] = { 6, 1, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(float_t) * 4 };
    attribute_descriptions[7] = { 7, 1, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(float_t) * 4 };
    attribute_descriptions[8] = { 8, 0, VK_FORMAT_R32G32_SFLOAT, 0 };
    attribute_descriptions[9] = { 9, 0, VK_FORMAT_R32G32_SFLOAT, 0 };
    attribute_descriptions[10] = { 10, 1, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(float_t) * 4 };
    attribute_descriptions[11] = { 11, 1, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(float_t) * 4 };
    attribute_descriptions[12] = { 12, 1, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(float_t) * 4 };
    attribute_descriptions[13] = { 13, 1, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(float_t) * 4 };
    attribute_descriptions[14] = { 14, 1, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(float_t) * 4 };
    attribute_descriptions[15] = { 15, 1, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(float_t) * 4 };

    VkVertexInputBindingDescription* binding_descriptions = render_texture_vertex_input_binding_descriptions;
    binding_descriptions[0] = { 0, sizeof(float_t) * 4, VK_VERTEX_INPUT_RATE_VERTEX };
    binding_descriptions[1] = { 1, sizeof(float_t) * 4, VK_VERTEX_INPUT_RATE_INSTANCE };

    render_texture_vertex_data.Create(allocator, vertex_data_size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT
        | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VMA_MEMORY_USAGE_AUTO,
        VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT);
    
    render_texture_dummy_data.Create(allocator, dummy_data_size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT
        | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VMA_MEMORY_USAGE_AUTO,
        VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT);

    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = command_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer;
    vkAllocateCommandBuffers(device, &alloc_info, &command_buffer);

    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    Vulkan::Buffer staging_buffer;
    staging_buffer.Create(allocator, max_def(vertex_data_size, dummy_data_size),
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VMA_MEMORY_USAGE_AUTO,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
        VMA_ALLOCATION_CREATE_MAPPED_BIT);

    vkBeginCommandBuffer(command_buffer, &begin_info);

    staging_buffer.WriteMapMemory(vertex_data, 0, vertex_data_size);

    VkBufferCopy copy_region = {};
    copy_region.size = vertex_data_size;
    vkCmdCopyBuffer(command_buffer, staging_buffer.data, render_texture_vertex_data.data, 1, &copy_region);

    staging_buffer.WriteMapMemory(dummy_data, 0, dummy_data_size);

    copy_region = {};
    copy_region.size = dummy_data_size;
    vkCmdCopyBuffer(command_buffer, staging_buffer.data, render_texture_dummy_data.data, 1, &copy_region);

    vkEndCommandBuffer(command_buffer);

    staging_buffer.Destroy();

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;

    vkQueueSubmit(queue, 1, &submit_info, 0);
    vkQueueWaitIdle(queue);

    vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);

    render_texture_data_initialized = true;
}

void render_texture_vulkan_data_get_vertex_input_attribute_descriptions(
    const VkVertexInputAttributeDescription*& attribute_descriptions, uint32_t& attribute_description_count) {
    attribute_descriptions = render_texture_vertex_input_attribute_descriptions;
    attribute_description_count = sizeof(render_texture_vertex_input_attribute_descriptions)
        / sizeof(VkVertexInputAttributeDescription);
}

void render_texture_vulkan_data_get_vertex_input_binding_descriptions(
    const VkVertexInputBindingDescription*& binding_descriptions, uint32_t& binding_description_count) {
    binding_descriptions = render_texture_vertex_input_binding_descriptions;
    binding_description_count = sizeof(render_texture_vertex_input_binding_descriptions)
        / sizeof(VkVertexInputBindingDescription);
}

void render_texture_vulkan_data_free(VkDevice device, VmaAllocator allocator) {
    if (!render_texture_data_initialized)
        return;

    render_texture_dummy_data.Destroy();
    render_texture_vertex_data.Destroy();

    memset(render_texture_vertex_input_binding_descriptions,
        0, sizeof(render_texture_vertex_input_binding_descriptions));
    memset(render_texture_vertex_input_attribute_descriptions,
        0, sizeof(render_texture_vertex_input_attribute_descriptions));

    render_texture_data_initialized = false;
}

static int32_t render_texture_framebuffer_init(render_texture* rt, int32_t max_level) {
    glGenFramebuffers(max_level + 1, rt->fbos);
    return -(glGetError() != GL_ZERO);
}

static int32_t render_texture_framebuffer_set_texture(render_texture* rt,
    GLuint color_texture, int32_t level, GLuint depth_texture, bool stencil) {
    if (level < 0 || level > rt->max_level)
        return -1;

    gl_state_bind_framebuffer(rt->fbos[level]);
    glGetError();

    if (color_texture) {
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_texture, level);
        glGetError();
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
    }
    else {
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0, 0);
        glGetError();
        glDrawBuffer(GL_ZERO);
        glReadBuffer(GL_ZERO);
    }
    glGetError();

    if (level == 0) {
        if (stencil) {
            if (depth_texture)
                glFramebufferTexture(GL_FRAMEBUFFER,
                    GL_DEPTH_STENCIL_ATTACHMENT, depth_texture, 0);
            else if (rt->rbo) {
                glBindRenderbuffer(GL_RENDERBUFFER, rt->rbo);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                    GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rt->rbo);
            }
            else
                glFramebufferTexture(GL_FRAMEBUFFER,
                    GL_DEPTH_STENCIL_ATTACHMENT, 0, 0);
        }
        else {
            if (depth_texture)
                glFramebufferTexture(GL_FRAMEBUFFER,
                    GL_DEPTH_ATTACHMENT, depth_texture, 0);
            else if (rt->rbo) {
                glBindRenderbuffer(GL_RENDERBUFFER, rt->rbo);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                    GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rt->rbo);
            }
            else
                glFramebufferTexture(GL_FRAMEBUFFER,
                    GL_DEPTH_ATTACHMENT, 0, 0);
        }
        glGetError();
    }

    int32_t ret = 0;
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        ret = -1;
    gl_state_bind_framebuffer(0);
    glGetError();
    return ret;
}
