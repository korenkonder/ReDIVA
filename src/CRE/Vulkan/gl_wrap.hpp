/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../KKdLib/prj/shared_ptr.hpp"
#include "../../KKdLib/vec.hpp"
#include "../gl.hpp"
#include "shared.hpp"
#include "Framebuffer.hpp"
#include "Image.hpp"
#include "ImageView.hpp"
#include "IndexBuffer.hpp"
#include "Query.hpp"
#include "RenderPass.hpp"
#include "Sampler.hpp"
#include "ShaderModule.hpp"
#include "StorageBuffer.hpp"
#include "UniformBuffer.hpp"
#include "VertexBuffer.hpp"
#include "WorkingBuffer.hpp"

namespace Vulkan {
    constexpr uint32_t MAX_COLOR_ATTACHMENTS = 8;
    constexpr uint32_t MAX_DRAW_BUFFERS = 8;
    constexpr uint32_t MAX_VERTEX_ATTRIB_COUNT = 16;

    enum gl_buffer_flags {
        GL_BUFFER_FLAG_NONE                = 0x00,
        GL_BUFFER_FLAG_MAPPED              = 0x01,
        GL_BUFFER_FLAG_IMMUTABLE_STORAGE   = 0x02,
        GL_BUFFER_FLAG_UPDATE_DATA         = 0x04,
        GL_BUFFER_FLAG_MAP_READ_BIT        = 0x08,
        GL_BUFFER_FLAG_MAP_WRITE_BIT       = 0x10,
        GL_BUFFER_FLAG_DYNAMIC_STORAGE_BIT = 0x20,
    };

    struct gl_buffer {
        GLenum target;
        gl_buffer_flags flags;
        std::vector<uint8_t> data;

        gl_buffer();
        ~gl_buffer();

        gl_buffer& operator=(const gl_buffer& other);

        static gl_buffer* get(GLuint buffer);
    };

    struct gl_framebuffer {
        Vulkan::Framebuffer framebuffer[2];
        prj::shared_ptr<Vulkan::RenderPass> render_pass[2];

        GLuint color_attachments[Vulkan::MAX_COLOR_ATTACHMENTS];
        GLint color_attachment_levels[Vulkan::MAX_COLOR_ATTACHMENTS];
        Vulkan::ImageView color_attachment_image_views[Vulkan::MAX_COLOR_ATTACHMENTS];
        GLuint depth_attachment;
        GLint depth_attachment_level;
        Vulkan::ImageView depth_attachment_image_view;
        GLenum draw_buffers[Vulkan::MAX_DRAW_BUFFERS];
        GLenum read_buffer;
        bool update_color_attachment;
        bool update_depth_attachment;
        bool update_framebuffer;

        gl_framebuffer();

        GLuint get_draw_buffer_texture(uint32_t index = 0);
        GLuint get_read_buffer_texture();
        void release();

        static gl_framebuffer* get(GLuint framebuffer, bool update_data = true);
    };

    struct gl_index_buffer {
        Vulkan::IndexBuffer index_buffer;
        bool copy_working_buffer;
        Vulkan::WorkingBuffer working_buffer;

        gl_index_buffer();

        static gl_index_buffer* get(GLuint buffer);
    };

    struct gl_program {
        prj::shared_ptr<Vulkan::ShaderModule> vertex_shader_module;
        prj::shared_ptr<Vulkan::ShaderModule> fragment_shader_module;

        gl_program();

        static gl_program* get(GLuint program);
    };

    struct gl_query {
        GLenum target;
        Vulkan::Query query;

        gl_query();

        static gl_query* get(GLuint query);
    };

    struct gl_sampler {
        GLenum min_filter;
        GLenum mag_filter;
        GLenum wrap_s;
        GLenum wrap_t;
        GLenum wrap_r;
        vec4 border_color;
        float_t max_anisotropy;

        gl_sampler();
        gl_sampler(GLenum min_filter, GLenum mag_filter,
            GLenum wrap_s, GLenum wrap_t, GLenum wrap_r,
            const vec4& border_color, float_t max_anisotropy);

        static gl_sampler* get(GLuint program);
    };

    struct gl_storage_buffer {
        Vulkan::StorageBuffer storage_buffer;
        bool copy_working_buffer;
        Vulkan::WorkingBuffer working_buffer;

        gl_storage_buffer();

        static gl_storage_buffer* get(GLuint buffer);
    };

    struct gl_texture {
        GLenum target;
        GLint base_mipmap_level;
        GLint max_mipmap_level;
        GLenum internal_format;
        GLsizei width;
        GLsizei height;
        Vulkan::Image image;
        Vulkan::ImageView image_view;
        Vulkan::ImageView sample_image_view;
        uint32_t level_count;
        VkComponentMapping components;
        bool attachment;
        gl_sampler sampler_data;

        gl_texture();

        VkImageView get_image_view();

        static gl_texture* get(GLuint texture, bool update_data = true, bool attachment = false);

        inline uint32_t get_layer_count() const {
            return target == GL_TEXTURE_CUBE_MAP ? 6 : 1;
        }

        inline uint32_t get_level_count() const {
            return level_count;
        }
    };

    struct gl_uniform_buffer {
        Vulkan::UniformBuffer uniform_buffer;
        bool copy_working_buffer;
        Vulkan::WorkingBuffer working_buffer;

        gl_uniform_buffer();

        static gl_uniform_buffer* get(GLuint buffer);
    };

    struct gl_vertex_buffer_binding_data {
        GLuint buffer;
        uint32_t offset;
        uint32_t stride;

        gl_vertex_buffer_binding_data();
    };

    constexpr bool operator==(const gl_vertex_buffer_binding_data& left,
        const gl_vertex_buffer_binding_data& right) {
        return left.buffer == right.buffer
            && left.offset == right.offset && left.stride == right.stride;
    }

    struct gl_index_buffer_binding_data {
        GLuint buffer;

        gl_index_buffer_binding_data();
    };

    struct gl_vertex_array_vertex_attrib {
        uint32_t binding;
        VkFormat format;
        uint32_t offset;
        bool enabled;
        vec4 generic_value;

        gl_vertex_array_vertex_attrib();
    };

    struct gl_vertex_array {
        gl_vertex_buffer_binding_data vertex_buffer_bindings[Vulkan::MAX_VERTEX_ATTRIB_COUNT];
        gl_index_buffer_binding_data index_buffer_binding;

        gl_vertex_array_vertex_attrib vertex_attribs[Vulkan::MAX_VERTEX_ATTRIB_COUNT];

        gl_vertex_array();

        void set_vertex_attrib(uint32_t index, VkFormat format, uint32_t stride, uint32_t offset);
        void reset_vertex_attrib(uint32_t index);

    public:
        static gl_vertex_array* get(GLuint array);
    };

    struct gl_vertex_buffer {
        Vulkan::VertexBuffer vertex_buffer;
        bool copy_working_buffer;
        Vulkan::WorkingBuffer working_buffer;

        gl_vertex_buffer();

        static gl_vertex_buffer* get(GLuint buffer);
    };

    extern void gl_wrap_manager_init(Vulkan::CommandBuffer& command_buffer);
    extern VkBuffer gl_wrap_manager_get_dummy_vertex_buffer();
    extern GLuint gl_wrap_manager_get_query_samples_passed();
    extern void gl_wrap_manager_update_buffers();
    extern void gl_wrap_manager_load_funcs();
    extern void gl_wrap_manager_free();

    extern VkImageAspectFlags get_aspect_mask(GLenum internal_format);
    extern VkBlendFactor get_blend_factor(GLenum factor);
    extern VkBlendOp get_blend_op(GLenum mode);
    extern VkCompareOp get_compare_op(GLenum func);
    extern VkComponentSwizzle get_component_swizzle(GLenum swizzle, GLenum comp);
    extern VkCullModeFlags get_cull_mode_flags(GLenum mode);
    extern VkFormat get_format(GLenum internal_format);
    extern VkImageViewType get_image_view_type(GLenum target);
    extern VkPolygonMode get_polygon_mode(GLenum mode);
}
