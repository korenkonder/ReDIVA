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
#include "Sampler.hpp"
#include "StorageBuffer.hpp"
#include "UniformBuffer.hpp"
#include "UnknownBuffer.hpp"
#include "VertexBuffer.hpp"
#include "WorkingBuffer.hpp"

struct shader_table;
struct shader_sub_table;

namespace Vulkan {
    constexpr uint32_t MAX_COMBINED_TEXTURE_IMAGE_UNITS = 32;
    constexpr uint32_t MAX_COLOR_ATTACHMENTS = 8;
    constexpr uint32_t MAX_DRAW_BUFFERS = 8;
    constexpr uint32_t MAX_SHADER_STORAGE_BUFFER_BINDINGS = 14;
    constexpr uint32_t MAX_UNIFORM_BUFFER_BINDINGS = 14;
    constexpr uint32_t MAX_VERTEX_ATTRIB_COUNT = 16;

    enum gl_buffer_flags {
        GL_BUFFER_FLAG_NONE                = (0 << 0u),
        GL_BUFFER_FLAG_MAPPED              = (1 << 0u),
        GL_BUFFER_FLAG_IMMUTABLE_STORAGE   = (1 << 1u),
        GL_BUFFER_FLAG_READ_DATA           = (1 << 2u),
        GL_BUFFER_FLAG_WRITE_DATA          = (1 << 3u),
        GL_BUFFER_FLAG_CLEAR_DATA          = (1 << 4u),
        GL_BUFFER_FLAG_MAP_READ_BIT        = (1 << 5u),
        GL_BUFFER_FLAG_MAP_WRITE_BIT       = (1 << 6u),
        GL_BUFFER_FLAG_DYNAMIC_STORAGE_BIT = (1 << 7u),

        GL_BUFFER_FLAG_UPDATE_DATA = GL_BUFFER_FLAG_CLEAR_DATA | GL_BUFFER_FLAG_WRITE_DATA,
    };

    enum gl_texture_flags {
        GL_TEXTURE_NONE       = (0 << 0u),
        GL_TEXTURE_ATTACHMENT = (1 << 0u),
    };

    struct gl_buffer {
        GLenum target;
        gl_buffer_flags flags;
        std::vector<uint8_t> data;

        gl_buffer();
        ~gl_buffer();

        void set_buffer(const VkDeviceSize size, const VkDeviceSize alignment,
            Vulkan::Buffer& buffer, bool& copy_working_buffer, Vulkan::WorkingBuffer& working_buffer);

        gl_buffer& operator=(const gl_buffer& other);

        static gl_buffer* get(GLuint buffer);
    };

    struct gl_framebuffer {
        Vulkan::Framebuffer framebuffer[2];
        prj::shared_ptr<class RenderPass> render_pass[2];

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
        prj::shared_ptr<class ShaderModule> vertex_shader_module;
        prj::shared_ptr<class ShaderModule> fragment_shader_module;
        prj::shared_ptr<class DescriptorPipeline> descriptor_pipeline;
        uint32_t sampler_count;
        uint32_t uniform_count;
        uint32_t storage_count;
        uint32_t push_constant_range_count;
        uint32_t fragment_output_count;
        bool enabled_attributes[Vulkan::MAX_VERTEX_ATTRIB_COUNT];
        int32_t attribute_sizes[Vulkan::MAX_VERTEX_ATTRIB_COUNT];

        const shader_table* shader;
        const shader_sub_table* sub_shader;
        uint32_t unival_arr[0x20];

        gl_program();

        void init();

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
        float_t lod_bias;
        float_t min_lod;
        float_t max_lod;
        float_t max_anisotropy;

        gl_sampler();
        gl_sampler(GLenum min_filter, GLenum mag_filter, GLenum wrap_s, GLenum wrap_t, GLenum wrap_r,
            const vec4& border_color, float_t lod_bias, float_t min_lod, float_t max_lod, float_t max_anisotropy);

        static gl_sampler* get(GLuint program);
    };

    struct gl_state_rect {
        GLint x;
        GLint y;
        GLsizei width;
        GLsizei height;
    };

    struct gl_state_struct {
        vec4 clear_color;
        float_t clear_depth;
        int32_t clear_stencil;
        int32_t pack_alignment;
        int32_t unpack_alignment;
        GLuint query_samples_passed;

        GLuint program;
        GLuint active_texture_index;
        GLuint texture_binding_2d[Vulkan::MAX_COMBINED_TEXTURE_IMAGE_UNITS];
        GLuint texture_binding_cube_map[Vulkan::MAX_COMBINED_TEXTURE_IMAGE_UNITS];
        GLuint sampler_binding[Vulkan::MAX_COMBINED_TEXTURE_IMAGE_UNITS];
        GLboolean blend;
        GLenum blend_src_rgb;
        GLenum blend_src_alpha;
        GLenum blend_dst_rgb;
        GLenum blend_dst_alpha;
        GLenum blend_mode_rgb;
        GLenum blend_mode_alpha;
        GLuint read_framebuffer_binding;
        GLuint draw_framebuffer_binding;
        GLuint vertex_array_binding;
        GLuint array_buffer_binding;
        GLuint element_array_buffer_binding;
        GLuint uniform_buffer_binding;
        GLuint uniform_buffer_bindings[Vulkan::MAX_SHADER_STORAGE_BUFFER_BINDINGS];
        GLintptr uniform_buffer_offsets[Vulkan::MAX_SHADER_STORAGE_BUFFER_BINDINGS];
        GLsizeiptr uniform_buffer_sizes[Vulkan::MAX_SHADER_STORAGE_BUFFER_BINDINGS];
        GLuint shader_storage_buffer_binding;
        GLuint shader_storage_buffer_bindings[Vulkan::MAX_UNIFORM_BUFFER_BINDINGS];
        GLintptr shader_storage_buffer_offsets[Vulkan::MAX_UNIFORM_BUFFER_BINDINGS];
        GLsizeiptr shader_storage_buffer_sizes[Vulkan::MAX_UNIFORM_BUFFER_BINDINGS];
        GLboolean color_mask[4];
        GLboolean cull_face;
        GLenum cull_face_mode;
        GLboolean depth_test;
        GLenum depth_func;
        GLboolean depth_mask;
        GLfloat line_width;
        GLenum polygon_mode;
        GLboolean multisample;
        GLboolean primitive_restart;
        GLuint primitive_restart_index;
        gl_state_rect scissor_box;
        GLboolean scissor_test;
        GLboolean stencil_test;
        GLenum stencil_func;
        GLenum stencil_fail;
        GLenum stencil_dpfail;
        GLenum stencil_dppass;
        GLuint stencil_mask;
        GLint stencil_ref;
        GLuint stencil_value_mask;
        gl_state_rect viewport;
        bool viewport_set;

        gl_state_struct();
    };

    struct gl_storage_buffer {
        Vulkan::StorageBuffer storage_buffer;
        bool copy_working_buffer;
        bool shader_write;
        VkPipelineStageFlags shader_write_flags;
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
        gl_texture_flags flags;
        gl_sampler sampler_data;

        gl_texture();

        VkImageView get_image_view();

        static gl_texture* get(GLuint texture, bool update_data = true,
            gl_texture_flags attachment = GL_TEXTURE_NONE);

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
        bool shader_write;
        VkPipelineStageFlags shader_write_flags;
        Vulkan::WorkingBuffer working_buffer;

        gl_uniform_buffer();

        static gl_uniform_buffer* get(GLuint buffer);
    };

    struct gl_unknown_buffer {
        Vulkan::UnknownBuffer unknown_buffer;

        gl_unknown_buffer();

        static gl_unknown_buffer* get(GLuint buffer);
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
    extern VkStencilOp get_stencil_op(GLenum op);
}
