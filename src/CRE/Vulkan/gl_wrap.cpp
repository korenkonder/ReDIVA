/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "gl_wrap.hpp"
#include "../../KKdLib/hash.hpp"
#include "../gl_state.hpp"
#include "../static_var.hpp"
#include "../texture.hpp"
#include "CommandBuffer.hpp"
#include "Manager.hpp"
#include <list>
#include <unordered_map>

namespace Vulkan {
    struct gl_texture_data {
        struct tex_data {
            std::vector<uint8_t> data;

            tex_data();
            ~tex_data();

            void set_data(GLint internal_format, GLint xoffset, GLint yoffset,
                GLsizei width, GLsizei height, GLenum format, GLenum type, const void* data);

            static bool convert(GLenum internal_format,
                GLsizei width, GLsizei height, GLenum format, GLenum src_type, GLenum dst_type,
                const void* src_data, void* dst_data, ssize_t alignment, bool unpack);

            tex_data& operator=(const tex_data& other);
        };

        GLenum target;
        GLenum internal_format;
        GLsizei width;
        GLsizei height;
        uint32_t level_count;
        std::vector<tex_data> data[6];

        gl_texture_data();
        ~gl_texture_data();

        tex_data* get_tex_data(uint32_t level, uint32_t layer, bool get = false);
        bool valid();

        gl_texture_data& operator=(const gl_texture_data& other);

        static gl_texture_data* get(GLuint texture);
    };

    struct gl_wrap_manager {
        GLuint buffer_counter;
        GLuint framebuffer_counter;
        GLuint program_counter;
        GLuint query_counter;
        GLuint sampler_counter;
        GLuint texture_counter;
        GLuint vertex_array_counter;
        Vulkan::VertexBuffer dummy_vertex_buffer;
        std::unordered_map<GLuint, gl_buffer> gl_buffers;
        std::vector<GLenum> gl_errors;
        std::unordered_map<GLuint, gl_framebuffer> gl_framebuffers;
        std::unordered_map<GLuint, gl_index_buffer> gl_index_buffers;
        std::unordered_map<GLuint, gl_program> gl_programs;
        std::unordered_map<GLuint, gl_query> gl_queries;
        std::unordered_map<GLuint, gl_sampler> gl_samplers;
        std::unordered_map<GLuint, gl_storage_buffer> gl_storage_buffers;
        std::unordered_map<GLuint, gl_texture> gl_textures;
        std::unordered_map<GLuint, gl_texture_data> gl_texture_datas;
        std::unordered_map<GLuint, gl_uniform_buffer> gl_uniform_buffers;
        std::unordered_map<GLuint, gl_vertex_array> gl_vertex_arrays;
        std::unordered_map<GLuint, gl_vertex_buffer> gl_vertex_buffers;

        vec4 clear_color;
        float_t clear_depth;
        int32_t pack_alignment;
        int32_t unpack_alignment;
        GLuint query_samples_passed;

        gl_wrap_manager();
        ~gl_wrap_manager();

        GLuint create_buffer();
        GLuint create_framebuffer();
        GLuint create_program();
        GLuint create_query();
        GLuint create_sampler();
        GLuint create_texture();
        GLuint create_vertex_array();
        GLenum get_error();
        void push_error(GLenum error);
        void release_buffer(GLuint buffer);
        void release_framebuffer(GLuint framebuffer);
        void release_program(GLuint sampler);
        void release_query(GLuint query);
        void release_sampler(GLuint sampler);
        void release_texture(GLuint texture);
        void release_vertex_array(GLuint array);
        void update_buffers();
    };

    gl_wrap_manager* gl_wrap_manager_ptr;

    static void gl_wrap_manager_begin_query(GLenum target, GLuint id);
    static void gl_wrap_manager_bind_buffer(GLenum target, GLuint buffer);
    static void gl_wrap_manager_bind_texture(GLenum target, GLuint texture);
    static void gl_wrap_manager_blit_framebuffer(
        GLint src_x0, GLint src_y0, GLint src_x1, GLint src_y1,
        GLint dst_x0, GLint dst_y0, GLint dst_x1, GLint dst_y1, GLbitfield mask, GLenum filter);
    static void gl_wrap_manager_blit_named_framebuffer(GLuint readFramebuffer, GLuint drawFramebuffer,
        GLint src_x0, GLint src_y0, GLint src_x1, GLint src_y1,
        GLint dst_x0, GLint dst_y0, GLint dst_x1, GLint dst_y1, GLbitfield mask, GLenum filter);
    static void gl_wrap_manager_buffer_data(
        GLenum target, GLsizeiptr size, const void* data, GLenum usage);
    static void gl_wrap_manager_buffer_storage(
        GLenum target, GLsizeiptr size, const void* data, GLbitfield flags);
    static void gl_wrap_manager_buffer_sub_data(
        GLenum target, GLintptr offset, GLsizeiptr size, const void* data);
    static GLenum gl_wrap_manager_check_framebuffer_status(GLenum target);
    static void gl_wrap_manager_clear(GLbitfield mask);
    static void gl_wrap_manager_clear_buffer(GLenum buffer, GLint drawbuffer, const GLfloat* value);
    static void gl_wrap_manager_clear_color(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
    static void gl_wrap_manager_clear_depth(GLfloat depth);
    static void gl_wrap_manager_clear_named_framebuffer(GLuint framebuffer,
        GLenum buffer, GLint drawbuffer, const GLfloat* value);
    static void gl_wrap_manager_compressed_tex_image_2d(GLenum target, GLint level,
        GLenum internal_format, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data);
    static void gl_wrap_manager_compressed_tex_sub_image_2d(GLenum target, GLint level,
        GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data);
    static void gl_wrap_manager_copy_image_sub_data(
        GLuint src_name, GLenum src_target, GLint src_level, GLint src_x, GLint src_y, GLint src_z,
        GLuint dst_name, GLenum dst_target, GLint dst_level, GLint dst_x, GLint dst_y, GLint dst_z,
        GLsizei src_width, GLsizei src_height, GLsizei src_depth);
    static void gl_wrap_manager_copy_tex_sub_image_2d(GLenum target, GLint level,
        GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
    static void gl_wrap_manager_create_buffers(GLsizei n, GLuint* buffers);
    static GLuint gl_wrap_manager_create_program();
    static void gl_wrap_manager_delete_buffers(GLsizei n, const GLuint* buffers);
    static void gl_wrap_manager_delete_framebuffers(GLsizei n, const GLuint* framebuffers);
    static void gl_wrap_manager_delete_program(GLuint program);
    static void gl_wrap_manager_delete_queries(GLsizei n, const GLuint* ids);
    static void gl_wrap_manager_delete_samplers(GLsizei n, const GLuint* samplers);
    static void gl_wrap_manager_delete_textures(GLsizei n, const GLuint* textures);
    static void gl_wrap_manager_delete_vertex_arrays(GLsizei n, const GLuint* arrays);
    static void gl_wrap_manager_disable_vertex_attrib_array(GLuint index);
    static void gl_wrap_manager_draw_buffer(GLenum buf);
    static void gl_wrap_manager_draw_buffers(GLsizei n, const GLenum* bufs);
    static void gl_wrap_manager_enable_vertex_attrib_array(GLuint index);
    static void gl_wrap_manager_end_query(GLenum target);
    static void gl_wrap_manager_finish();
    static void gl_wrap_manager_framebuffer_texture(GLenum target,
        GLenum attachment, GLuint texture, GLint level);
    static void gl_wrap_manager_gen_buffers(GLsizei n, GLuint* buffers);
    static void gl_wrap_manager_gen_framebuffers(GLsizei n, GLuint* framebuffers);
    static void gl_wrap_manager_gen_queries(GLsizei n, GLuint* ids);
    static void gl_wrap_manager_gen_samplers(GLsizei n, GLuint* samplers);
    static void gl_wrap_manager_gen_textures(GLsizei n, GLuint* textures);
    static void gl_wrap_manager_gen_vertex_arrays(GLsizei n, GLuint* arrays);
    static void gl_wrap_manager_generate_mipmap(GLenum target);
    static void gl_wrap_manager_generate_texture_mipmap(GLuint texture);
    static void gl_wrap_manager_get_compressed_tex_image(GLenum target, GLint level, void* img);
    static GLenum gl_wrap_manager_get_error();
    static void gl_wrap_manager_get_float(GLenum pname, GLfloat* data);
    static void gl_wrap_manager_get_query_object(GLuint id, GLenum pname, GLint* params);
    static void gl_wrap_manager_get_query_object(GLuint id, GLenum pname, GLuint* params);
    static void gl_wrap_manager_get_tex_image(GLenum target,
        GLint level, GLenum format, GLenum type, void* pixels);
    static void* gl_wrap_manager_map_buffer(GLenum target, GLenum access);
    static void* gl_wrap_manager_map_named_buffer(GLuint buffer, GLenum access);
    static void gl_wrap_manager_named_buffer_data(
        GLuint buffer, GLsizeiptr size, const void* data, GLenum usage);
    static void gl_wrap_manager_named_buffer_storage(
        GLuint buffer, GLsizeiptr size, const void* data, GLbitfield flags);
    static void gl_wrap_manager_named_buffer_sub_data(
        GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data);
    static void gl_wrap_manager_pixel_store(GLenum pname, GLint param);
    static void gl_wrap_manager_pop_debug_group();
    static void gl_wrap_manager_push_debug_group(GLenum source, GLuint id, GLsizei length, const GLchar* message);
    static void gl_wrap_manager_read_buffer(GLenum src);
    static void gl_wrap_manager_sampler_parameter(GLuint sampler, GLenum pname, GLfloat param);
    static void gl_wrap_manager_sampler_parameter(GLuint sampler, GLenum pname, GLint param);
    static void gl_wrap_manager_sampler_parameter(GLuint sampler, GLenum pname, const GLfloat* params);
    static void gl_wrap_manager_sampler_parameter(GLuint sampler, GLenum pname, const GLint* params);
    static void gl_wrap_manager_tex_image_2d(GLenum target, GLint level, GLint internal_format,
        GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels);
    static void gl_wrap_manager_tex_parameter(GLenum target, GLenum pname, GLfloat param);
    static void gl_wrap_manager_tex_parameter(GLenum target, GLenum pname, GLint param);
    static void gl_wrap_manager_tex_parameter(GLenum target, GLenum pname, const GLfloat* params);
    static void gl_wrap_manager_tex_parameter(GLenum target, GLenum pname, const GLint* params);
    static void gl_wrap_manager_tex_sub_image_2d(GLenum target,
        GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
        GLenum format, GLenum type, const void* pixels);
    static void gl_wrap_manager_texture_parameter(GLuint texture, GLenum pname, GLfloat param);
    static void gl_wrap_manager_texture_parameter(GLuint texture, GLenum pname, GLint param);
    static void gl_wrap_manager_texture_parameter(GLuint texture, GLenum pname, const GLfloat* params);
    static void gl_wrap_manager_texture_parameter(GLuint texture, GLenum pname, const GLint* params);
    static void gl_wrap_manager_texture_sub_image_2d(GLuint texture,
        GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
        GLenum format, GLenum type, const void* pixels);
    static GLboolean gl_wrap_manager_unmap_buffer(GLenum target);
    static GLboolean gl_wrap_manager_unmap_named_buffer(GLuint buffer);
    static void gl_wrap_manager_vertex_attrib(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    static void gl_wrap_manager_vertex_attrib_int_pointer(GLuint index, GLint size,
        GLenum type, GLsizei stride, const void* pointer);
    static void gl_wrap_manager_vertex_attrib_pointer(GLuint index, GLint size,
        GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);

    gl_buffer::gl_buffer() : target(), flags() {

    }

    gl_buffer::~gl_buffer() {

    }

    gl_buffer& gl_buffer::operator=(const gl_buffer& other) {
        target = other.target;
        flags = other.flags;
        data.assign(other.data.begin(), other.data.end());
        return *this;
    }

    gl_buffer* gl_buffer::get(GLuint buffer) {
        if (!buffer)
            return 0;

        auto elem = gl_wrap_manager_ptr->gl_buffers.find(buffer);
        if (elem != gl_wrap_manager_ptr->gl_buffers.end())
            return &elem->second;
        return 0;
    }

    gl_framebuffer::gl_framebuffer() : render_pass(), color_attachments(),
        color_attachment_levels(), depth_attachment(), depth_attachment_level(),
        draw_buffers(), update_color_attachment(), update_depth_attachment(), update_framebuffer() {
        draw_buffers[0] = GL_COLOR_ATTACHMENT0;
        for (uint32_t i = 1; i < Vulkan::MAX_DRAW_BUFFERS; i++)
            draw_buffers[i] = GL_NONE;
        read_buffer = GL_COLOR_ATTACHMENT0;
    }

    GLuint gl_framebuffer::get_draw_buffer_texture(uint32_t index) {
        if (index >= Vulkan::MAX_DRAW_BUFFERS)
            return 0;

        GLenum draw_buffer = draw_buffers[index];
        if (draw_buffer && draw_buffer >= GL_COLOR_ATTACHMENT0
            && draw_buffer < GL_COLOR_ATTACHMENT0 + Vulkan::MAX_COLOR_ATTACHMENTS)
            return color_attachments[draw_buffer - GL_COLOR_ATTACHMENT0];
        return 0;
    }

    GLuint gl_framebuffer::get_read_buffer_texture() {
        if (read_buffer && read_buffer >= GL_COLOR_ATTACHMENT0
            && read_buffer < GL_COLOR_ATTACHMENT0 + Vulkan::MAX_COLOR_ATTACHMENTS)
            return color_attachments[read_buffer - GL_COLOR_ATTACHMENT0];
        return 0;
    }

    void gl_framebuffer::release() {
        framebuffer.Destroy();
        render_pass.reset();

        update_framebuffer = false;
        update_depth_attachment = false;
        update_color_attachment = false;

        read_buffer = GL_COLOR_ATTACHMENT0;
        draw_buffers[0] = GL_COLOR_ATTACHMENT0;
        for (uint32_t i = 1; i < Vulkan::MAX_DRAW_BUFFERS; i++)
            draw_buffers[i] = GL_NONE;

        depth_attachment_image_view.Destroy();
        depth_attachment_level = 0;
        depth_attachment = 0;

        for (uint32_t i = 0; i < Vulkan::MAX_COLOR_ATTACHMENTS; i++) {
            color_attachment_image_views[i].Destroy();
            color_attachment_levels[i] = 0;
            color_attachments[i] = 0;
        }
    }

    gl_framebuffer* gl_framebuffer::get(GLuint framebuffer, bool update_data) {
        if (!framebuffer)
            return 0;

        auto elem = gl_wrap_manager_ptr->gl_framebuffers.find(framebuffer);
        if (elem == gl_wrap_manager_ptr->gl_framebuffers.end())
            return 0;

        gl_framebuffer* vk_fbo = &elem->second;
        if (!update_data)
            return vk_fbo;

        if (vk_fbo->update_color_attachment) {
            for (uint32_t i = 0; i < Vulkan::MAX_COLOR_ATTACHMENTS; i++)
                vk_fbo->color_attachment_image_views[i].Destroy();
  
            for (int32_t i = 0; i < Vulkan::MAX_COLOR_ATTACHMENTS; i++) {
                gl_texture* vk_tex = gl_texture::get(vk_fbo->color_attachments[i]);
                if (vk_tex && vk_tex->get_level_count() > 0) {
                    const VkImageViewType image_view_type = Vulkan::get_image_view_type(vk_tex->target);
                    const VkFormat format = Vulkan::get_format(vk_tex->internal_format);
                    const VkImageAspectFlags aspect_mask = Vulkan::get_aspect_mask(vk_tex->internal_format);

                    const int32_t layer_count = vk_tex->get_layer_count();
                    vk_fbo->color_attachment_image_views[i].Create(Vulkan::current_device, 0, vk_tex->image,
                        image_view_type, format, aspect_mask, 0, 1, 0, layer_count);
                }
            }

            GLenum depth_format = GL_ZERO;
            if (vk_fbo->depth_attachment) {
                gl_texture* vk_tex = gl_texture::get(vk_fbo->depth_attachment);
                depth_format = vk_tex ? vk_tex->internal_format : GL_ZERO;
            }

            vk_fbo->update_color_attachment = false;
            vk_fbo->update_framebuffer = true;
        }

        if (vk_fbo->update_depth_attachment) {
            vk_fbo->depth_attachment_image_view.Destroy();

            gl_texture* vk_tex = gl_texture::get(vk_fbo->depth_attachment);
            if (vk_tex && vk_tex->get_level_count() > 0) {
                const VkImageViewType image_view_type = Vulkan::get_image_view_type(vk_tex->target);
                const VkFormat format = Vulkan::get_format(vk_tex->internal_format);
                const VkImageAspectFlags aspect_mask = Vulkan::get_aspect_mask(vk_tex->internal_format);

                const int32_t layer_count = vk_tex->get_layer_count();
                vk_fbo->depth_attachment_image_view.Create(Vulkan::current_device, 0, vk_tex->image,
                    image_view_type, format, aspect_mask, 0, 1, 0, layer_count);
            }

            vk_fbo->update_depth_attachment = false;
            vk_fbo->update_framebuffer = true;
        }

        if (vk_fbo->update_framebuffer) {
            GLenum color_formats[Vulkan::MAX_DRAW_BUFFERS] = {};
            uint32_t color_format_count = 0;
            VkImageView image_views[Vulkan::MAX_DRAW_BUFFERS + 1] = {};
            uint32_t image_view_count = 0;

            uint32_t width = 0;
            uint32_t height = 0;

            for (uint32_t i = 0; i < Vulkan::MAX_DRAW_BUFFERS; i++) {
                GLuint texture = vk_fbo->get_draw_buffer_texture(i);
                gl_texture* vk_tex = gl_texture::get(texture);
                if (!vk_tex)
                    continue;

                if (!width && !height) {
                    width = vk_tex->width;
                    height = vk_tex->height;
                }

                color_formats[color_format_count] = vk_tex->internal_format;
                image_views[image_view_count] = vk_tex->image_view;

                for (uint32_t j = 0; j < Vulkan::MAX_COLOR_ATTACHMENTS; j++)
                    if (vk_fbo->color_attachments[j] == texture) {
                        if (vk_fbo->color_attachment_image_views[j])
                            image_views[color_format_count] = vk_fbo->color_attachment_image_views[j];
                        break;
                    }
                color_format_count++;
                image_view_count++;
            }

            GLenum depth_format = GL_ZERO;
            if (vk_fbo->depth_attachment) {
                gl_texture* vk_tex = gl_texture::get(vk_fbo->depth_attachment);
                if (vk_tex) {
                    depth_format = vk_tex->internal_format;
                    image_views[image_view_count] = vk_tex->image_view;

                    if (!width && !height) {
                        width = vk_tex->width;
                        height = vk_tex->height;
                    }

                    if (vk_fbo->depth_attachment_image_view)
                        image_views[image_view_count] = vk_fbo->depth_attachment_image_view;
                    image_view_count++;
                }
            }

            vk_fbo->render_pass = Vulkan::manager_get_render_pass(
                color_formats, color_format_count, depth_format);
            vk_fbo->framebuffer.Create(Vulkan::current_device, 0,
                *vk_fbo->render_pass.get(), image_view_count, image_views, width, height);

            vk_fbo->update_framebuffer = false;
        }
        return vk_fbo;
    }

    gl_index_buffer::gl_index_buffer() : copy_working_buffer() {

    }

    gl_index_buffer* gl_index_buffer::get(GLuint buffer) {
        if (!buffer)
            return 0;

        auto elem_buffer = gl_wrap_manager_ptr->gl_buffers.find(buffer);
        if (elem_buffer == gl_wrap_manager_ptr->gl_buffers.end())
            return 0;

        gl_buffer* vk_buf = &elem_buffer->second;

        auto elem = gl_wrap_manager_ptr->gl_index_buffers.find(buffer);
        if (elem == gl_wrap_manager_ptr->gl_index_buffers.end()) {
            vk_buf->target = GL_ELEMENT_ARRAY_BUFFER;
            elem = gl_wrap_manager_ptr->gl_index_buffers.insert({ buffer, {} }).first;
        }

        gl_index_buffer* vk_ib = &elem->second;

        const VkDeviceSize size = vk_buf->data.size();
        const VkDeviceSize alignment = 0x40;
        if (!vk_ib->index_buffer || vk_ib->index_buffer.GetSize() < size)
            vk_ib->index_buffer.Create(Vulkan::current_allocator, size);

        if (elem_buffer->second.flags & Vulkan::GL_BUFFER_FLAG_UPDATE_DATA) {
            const uint64_t hash = hash_xxh3_64bits(vk_buf->data.data(), size);
            if (vk_ib->copy_working_buffer && vk_ib->working_buffer.FindBuffer(hash, size))
                return vk_ib;

            Vulkan::Buffer dynamic_buffer = Vulkan::manager_get_dynamic_buffer(size, alignment);
            dynamic_buffer.WriteMemory(dynamic_buffer.GetOffset(), size, vk_buf->data.data());
            enum_and(elem_buffer->second.flags, ~Vulkan::GL_BUFFER_FLAG_UPDATE_DATA);

            vk_ib->working_buffer.AddBuffer(hash, dynamic_buffer);
            vk_ib->copy_working_buffer = true;
        }
        else if (!vk_ib->copy_working_buffer)
            vk_ib->working_buffer.SetBuffer(vk_ib->index_buffer);

        return vk_ib;
    }

    gl_program::gl_program() {

    }

    gl_program* gl_program::get(GLuint program) {
        if (!program)
            return 0;

        auto elem = gl_wrap_manager_ptr->gl_programs.find(program);
        if (elem != gl_wrap_manager_ptr->gl_programs.end())
            return &elem->second;
        return 0;
    }

    gl_query::gl_query() : target() {

    }

    gl_query* gl_query::get(GLuint query) {
        if (!query)
            return 0;

        auto elem = gl_wrap_manager_ptr->gl_queries.find(query);
        if (elem != gl_wrap_manager_ptr->gl_queries.end())
            return &elem->second;
        return 0;
    }

    gl_sampler::gl_sampler() {
        mag_filter = GL_NEAREST_MIPMAP_LINEAR;
        min_filter = GL_LINEAR;
        wrap_s = GL_REPEAT;
        wrap_t = GL_REPEAT;
        wrap_r = GL_REPEAT;
        border_color = 0.0f;
        max_anisotropy = 1.0f;
    }

    gl_sampler::gl_sampler(GLenum min_filter, GLenum mag_filter,
        GLenum wrap_s, GLenum wrap_t, GLenum wrap_r,
        const vec4& border_color, float_t max_anisotropy) {
        this->mag_filter = mag_filter;
        this->min_filter = min_filter;
        this->wrap_s = wrap_s;
        this->wrap_t = wrap_t;
        this->wrap_r = wrap_r;
        this->border_color = border_color;
        this->max_anisotropy = max_anisotropy;
    }

    gl_sampler* gl_sampler::get(GLuint sampler) {
        if (!sampler)
            return 0;

        auto elem = gl_wrap_manager_ptr->gl_samplers.find(sampler);
        if (elem != gl_wrap_manager_ptr->gl_samplers.end())
            return &elem->second;
        return 0;
    }

    gl_storage_buffer::gl_storage_buffer() : copy_working_buffer() {

    }

    gl_storage_buffer* gl_storage_buffer::get(GLuint buffer) {
        if (!buffer)
            return 0;

        auto elem_buffer = gl_wrap_manager_ptr->gl_buffers.find(buffer);
        if (elem_buffer == gl_wrap_manager_ptr->gl_buffers.end())
            return 0;

        gl_buffer* vk_buf = &elem_buffer->second;

        auto elem = gl_wrap_manager_ptr->gl_storage_buffers.find(buffer);
        if (elem == gl_wrap_manager_ptr->gl_storage_buffers.end()) {
            vk_buf->target = GL_SHADER_STORAGE_BUFFER;
            elem = gl_wrap_manager_ptr->gl_storage_buffers.insert({ buffer, {} }).first;
        }

        gl_storage_buffer* vk_sb = &elem->second;

        const VkDeviceSize size = vk_buf->data.size();
        const VkDeviceSize alignment = sv_min_storage_buffer_alignment;
        if (!vk_sb->storage_buffer || vk_sb->storage_buffer.GetSize() < size)
            vk_sb->storage_buffer.Create(Vulkan::current_allocator, size);

        if (elem_buffer->second.flags & Vulkan::GL_BUFFER_FLAG_UPDATE_DATA) {
            const uint64_t hash = hash_xxh3_64bits(vk_buf->data.data(), size);
            if (vk_sb->copy_working_buffer && vk_sb->working_buffer.FindBuffer(hash, size))
                return vk_sb;

            Vulkan::Buffer dynamic_buffer = Vulkan::manager_get_dynamic_buffer(size, alignment);
            dynamic_buffer.WriteMemory(dynamic_buffer.GetOffset(), size, vk_buf->data.data());
            enum_and(elem_buffer->second.flags, ~Vulkan::GL_BUFFER_FLAG_UPDATE_DATA);

            vk_sb->working_buffer.AddBuffer(hash, dynamic_buffer);
            vk_sb->copy_working_buffer = true;
        }
        else if (!vk_sb->copy_working_buffer)
            vk_sb->working_buffer.SetBuffer(vk_sb->storage_buffer);

        return vk_sb;
    }

    gl_texture::gl_texture() : target(), base_mipmap_level(),
        internal_format(), width(), height(), level_count(), components() {
        max_mipmap_level = 1000;
    }

    VkImageView gl_texture::get_image_view() {
        return sample_image_view ? sample_image_view : image_view;
    }

    gl_texture* gl_texture::get(GLuint texture, bool update_data) {
        if (!texture)
            return 0;

        auto elem = gl_wrap_manager_ptr->gl_textures.find(texture);
        if (elem == gl_wrap_manager_ptr->gl_textures.end())
            return 0;

        gl_texture* vk_tex = &elem->second;
        gl_texture_data* vk_tex_data = gl_texture_data::get(texture);
        if (!vk_tex->image && (!vk_tex_data || !vk_tex_data->valid()))
            return vk_tex;

        bool create = false;
        if (!vk_tex->image) {
            if (!vk_tex_data || !vk_tex_data->valid())
                return vk_tex;

            create = true;
        }

        if (vk_tex_data && (vk_tex->internal_format != vk_tex_data->internal_format
            || vk_tex->width != vk_tex_data->width
            || vk_tex->height != vk_tex_data->height
            || vk_tex->level_count != vk_tex_data->level_count))
            create = true;

        if (create && update_data) {
            vk_tex->sample_image_view.Destroy();

            vk_tex->internal_format = vk_tex_data->internal_format;
            vk_tex->width = vk_tex_data->width;
            vk_tex->height = vk_tex_data->height;
            vk_tex->level_count = vk_tex_data->level_count;

            const VkImageViewType image_view_type = Vulkan::get_image_view_type(vk_tex->target);
            const VkFormat format = Vulkan::get_format(vk_tex->internal_format);
            const VkImageAspectFlags aspect_mask = Vulkan::get_aspect_mask(vk_tex->internal_format);

            const bool attachment = !vk_tex_data->get_tex_data(0, 0)->data.size();

            VkImageUsageFlags usage = 0;
            switch (format) {
            case VK_FORMAT_D24_UNORM_S8_UINT:
            case VK_FORMAT_D32_SFLOAT:
                if (attachment)
                    usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
                break;
            default:
                if (attachment)
                    usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
                break;
            }

            usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
            usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

            const VkImageCreateFlags flags = vk_tex->target == GL_TEXTURE_CUBE_MAP
                ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
            const int32_t level_count = vk_tex->get_level_count();
            const int32_t layer_count = vk_tex->get_layer_count();
            vk_tex->image.Create(Vulkan::current_allocator,
                flags, vk_tex->width, vk_tex->height, level_count, layer_count, format,
                VK_IMAGE_TILING_OPTIMAL, usage, VMA_MEMORY_USAGE_AUTO,
                attachment ? VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT : 0);
            vk_tex->image_view.Create(Vulkan::current_device, 0, vk_tex->image,
                image_view_type, format, aspect_mask, 0, level_count, 0, layer_count);
            Vulkan::Image::PipelineBarrier(Vulkan::current_command_buffer, vk_tex->image,
                aspect_mask, level_count, layer_count, VK_IMAGE_LAYOUT_GENERAL);
        }

        if (vk_tex_data && update_data) {
            const int32_t level_count = vk_tex->get_level_count();
            const int32_t layer_count = vk_tex->get_layer_count();
            for (int32_t i = 0; i < layer_count; i++)
                for (int32_t j = 0; j < level_count; j++) {
                    gl_texture_data::tex_data* tex_data = vk_tex_data->get_tex_data(j, i);
                    if (!tex_data || !tex_data->data.size())
                        continue;

                    const void* data = tex_data->data.data();
                    const VkDeviceSize size = tex_data->data.size();
                    Vulkan::Buffer staging_buffer = Vulkan::manager_get_staging_buffer(size);
                    staging_buffer.WriteMemory(staging_buffer.GetOffset(), size, data);

                    const VkImageViewType image_view_type = Vulkan::get_image_view_type(vk_tex->target);
                    const VkFormat format = Vulkan::get_format(vk_tex->internal_format);
                    const VkImageAspectFlags aspect_mask = Vulkan::get_aspect_mask(vk_tex->internal_format);

                    Vulkan::CommandBuffer cb(Vulkan::current_device,
                        Vulkan::current_command_pool, Vulkan::current_command_buffer);

                    const VkImageLayout old_layout = vk_tex->image.GetImageLayout(j, i);
                    const VkImageLayout new_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

                    Vulkan::Image::PipelineBarrierSingle(Vulkan::current_command_buffer,
                        vk_tex->image, aspect_mask, j, i, new_layout);
                    cb.CopyBufferToImage(staging_buffer, staging_buffer.GetOffset(),
                        vk_tex->image, aspect_mask, j, i, 0, 0,
                        max_def(vk_tex->width >> j, 1), max_def(vk_tex->height >> j, 1));
                    Vulkan::Image::PipelineBarrierSingle(Vulkan::current_command_buffer,
                        vk_tex->image, aspect_mask, j, i, old_layout);
                }

            gl_wrap_manager_ptr->gl_texture_datas.erase(texture);
        }

        if (vk_tex->image && !vk_tex->sample_image_view) {
            const VkImageViewType image_view_type = Vulkan::get_image_view_type(vk_tex->target);
            const VkFormat format = Vulkan::get_format(vk_tex->internal_format);
            const VkImageAspectFlags aspect_mask = Vulkan::get_aspect_mask(vk_tex->internal_format);

            const uint32_t base_mipmap_level = vk_tex->base_mipmap_level;
            const uint32_t level_count = min_def((uint32_t)(vk_tex->max_mipmap_level
                - base_mipmap_level) + 1, vk_tex->get_level_count());
            const uint32_t layer_count = vk_tex->get_layer_count();
            vk_tex->sample_image_view.Create(Vulkan::current_device, 0, vk_tex->image,
                image_view_type, format, aspect_mask & ~VK_IMAGE_ASPECT_STENCIL_BIT,
                base_mipmap_level, level_count, 0, layer_count, vk_tex->components);
        }

        return vk_tex;
    }

    gl_uniform_buffer::gl_uniform_buffer() : copy_working_buffer() {

    }

    gl_uniform_buffer* gl_uniform_buffer::get(GLuint buffer) {
        if (!buffer)
            return 0;

        auto elem_buffer = gl_wrap_manager_ptr->gl_buffers.find(buffer);
        if (elem_buffer == gl_wrap_manager_ptr->gl_buffers.end())
            return 0;

        gl_buffer* vk_buf = &elem_buffer->second;

        auto elem = gl_wrap_manager_ptr->gl_uniform_buffers.find(buffer);
        if (elem == gl_wrap_manager_ptr->gl_uniform_buffers.end()) {
            vk_buf->target = GL_UNIFORM_BUFFER;
            elem = gl_wrap_manager_ptr->gl_uniform_buffers.insert({ buffer, {} }).first;
        }

        gl_uniform_buffer* vk_ub = &elem->second;

        const VkDeviceSize size = vk_buf->data.size();
        const VkDeviceSize alignment = sv_min_uniform_buffer_alignment;
        if (!vk_ub->uniform_buffer || vk_ub->uniform_buffer.GetSize() < size)
            vk_ub->uniform_buffer.Create(Vulkan::current_allocator, size);

        if (elem_buffer->second.flags & Vulkan::GL_BUFFER_FLAG_UPDATE_DATA) {
            const uint64_t hash = hash_xxh3_64bits(vk_buf->data.data(), size);
            if (vk_ub->copy_working_buffer && vk_ub->working_buffer.FindBuffer(hash, size))
                return vk_ub;

            Vulkan::Buffer dynamic_buffer = Vulkan::manager_get_dynamic_buffer(size, alignment);
            dynamic_buffer.WriteMemory(dynamic_buffer.GetOffset(), size, vk_buf->data.data());
            enum_and(elem_buffer->second.flags, ~Vulkan::GL_BUFFER_FLAG_UPDATE_DATA);

            vk_ub->working_buffer.AddBuffer(hash, dynamic_buffer);
            vk_ub->copy_working_buffer = true;
        }
        else if (!vk_ub->copy_working_buffer)
            vk_ub->working_buffer.SetBuffer(vk_ub->uniform_buffer);

        return vk_ub;
    }

    gl_vertex_buffer_binding_data::gl_vertex_buffer_binding_data() : buffer(), offset(), stride() {

    }

    gl_index_buffer_binding_data::gl_index_buffer_binding_data() : buffer() {

    }

    gl_vertex_array_vertex_attrib::gl_vertex_array_vertex_attrib() : format(), offset(), enabled() {
        binding = -1;
        generic_value = { 0.0f, 0.0f, 0.0f, 1.0f };
    }

    gl_vertex_array::gl_vertex_array() {

    }

    void gl_vertex_array::reset_vertex_attrib(uint32_t index) {
        const uint32_t binding = vertex_attribs[index].binding;
        if (binding == -1)
            return;

        vertex_attribs[index].binding = -1;

        for (gl_vertex_array_vertex_attrib& i : vertex_attribs)
            if (&i - vertex_attribs != index && i.binding != -1 && i.binding == binding)
                return;

        vertex_buffer_bindings[binding] = {};

        uint32_t binding_count = 0;
        uint32_t bindings[Vulkan::MAX_VERTEX_ATTRIB_COUNT] = {};
        for (uint32_t i = 0; i < Vulkan::MAX_VERTEX_ATTRIB_COUNT; i++)
            if (vertex_buffer_bindings[i].buffer)
                bindings[binding_count++] = i;

        for (uint32_t i = 0; i < binding_count; i++) {
            const uint32_t old_binding = bindings[i];
            const uint32_t new_binding = i;
            vertex_buffer_bindings[new_binding] = vertex_buffer_bindings[old_binding];

            for (gl_vertex_array_vertex_attrib& j : vertex_attribs)
                if (j.binding != -1 && j.binding == old_binding)
                    j.binding = new_binding;
        }

        for (uint32_t i = binding_count; i < Vulkan::MAX_VERTEX_ATTRIB_COUNT; i++)
            vertex_buffer_bindings[i] = {};
    }

    void gl_vertex_array::set_vertex_attrib(uint32_t index, VkFormat format, uint32_t stride, uint32_t offset) {
        reset_vertex_attrib(index);

        uint32_t binding = -1;
        for (gl_vertex_buffer_binding_data& i : vertex_buffer_bindings)
            if (i.buffer == gl_state.array_buffer_binding && i.stride == i.stride
                && i.offset < offset && offset < i.offset + i.stride) {
                binding = (uint32_t)(&i - vertex_buffer_bindings);
                offset -= i.offset;
                break;
            }

        if (binding == -1) {
            bool binding_used[Vulkan::MAX_VERTEX_ATTRIB_COUNT] = {};

            for (gl_vertex_array_vertex_attrib& i : vertex_attribs)
                if (i.binding != -1)
                    binding_used[i.binding] = true;

            for (uint32_t i = 0; i < Vulkan::MAX_VERTEX_ATTRIB_COUNT; i++)
                if (!binding_used[i]) {
                    binding = i;
                    vertex_buffer_bindings[binding].buffer = gl_state.array_buffer_binding;
                    vertex_buffer_bindings[binding].offset = offset;
                    vertex_buffer_bindings[binding].stride = stride;
                    offset = 0;
                    break;
                }

            if (binding == -1)
                return;
        }

        vertex_attribs[index].binding = binding;
        vertex_attribs[index].format = format;
        vertex_attribs[index].offset = offset;
    }

    gl_vertex_array* gl_vertex_array::get(GLuint array) {
        if (!array)
            return 0;

        auto elem = gl_wrap_manager_ptr->gl_vertex_arrays.find(array);
        if (elem != gl_wrap_manager_ptr->gl_vertex_arrays.end())
            return &elem->second;
        return 0;
    }

    gl_vertex_buffer::gl_vertex_buffer() : copy_working_buffer() {

    }

    gl_vertex_buffer* gl_vertex_buffer::get(GLuint buffer) {
        if (!buffer)
            return 0;

        auto elem_buffer = gl_wrap_manager_ptr->gl_buffers.find(buffer);
        if (elem_buffer == gl_wrap_manager_ptr->gl_buffers.end())
            return 0;

        gl_buffer* vk_buf = &elem_buffer->second;

        auto elem = gl_wrap_manager_ptr->gl_vertex_buffers.find(buffer);
        if (elem == gl_wrap_manager_ptr->gl_vertex_buffers.end()) {
            vk_buf->target = GL_ELEMENT_ARRAY_BUFFER;
            elem = gl_wrap_manager_ptr->gl_vertex_buffers.insert({ buffer, {} }).first;
        }

        gl_vertex_buffer* vk_vb = &elem->second;

        const VkDeviceSize size = vk_buf->data.size();
        const VkDeviceSize alignment = 0x40;
        if (!vk_vb->vertex_buffer || vk_vb->vertex_buffer.GetSize() < size)
            vk_vb->vertex_buffer.Create(Vulkan::current_allocator, size);

        if (elem_buffer->second.flags & Vulkan::GL_BUFFER_FLAG_UPDATE_DATA) {
            const uint64_t hash = hash_xxh3_64bits(vk_buf->data.data(), size);
            if (vk_vb->copy_working_buffer && vk_vb->working_buffer.FindBuffer(hash, size))
                return vk_vb;

            Vulkan::Buffer dynamic_buffer = Vulkan::manager_get_dynamic_buffer(size, alignment);
            dynamic_buffer.WriteMemory(dynamic_buffer.GetOffset(), size, vk_buf->data.data());
            enum_and(elem_buffer->second.flags, ~Vulkan::GL_BUFFER_FLAG_UPDATE_DATA);

            vk_vb->working_buffer.AddBuffer(hash, dynamic_buffer);
            vk_vb->copy_working_buffer = true;
        }
        else if (!vk_vb->copy_working_buffer)
            vk_vb->working_buffer.SetBuffer(vk_vb->vertex_buffer);

        return vk_vb;
    }

    void gl_wrap_manager_init(Vulkan::CommandBuffer& command_buffer) {
        gl_wrap_manager_ptr = new gl_wrap_manager;

        {
            const float_t dummy_data[] = {
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f,
                1.0f, 1.0f, 1.0f, 1.0f,
            };

            const void* data = dummy_data;
            const VkDeviceSize size = sizeof(dummy_data);
            gl_wrap_manager_ptr->dummy_vertex_buffer.Create(Vulkan::current_allocator, size);
            Vulkan::Buffer staging_buffer = Vulkan::manager_get_staging_buffer(size);
            staging_buffer.WriteMemory(staging_buffer.GetOffset(), size, data);
            Vulkan::Buffer::Copy(Vulkan::current_command_buffer, staging_buffer,
                gl_wrap_manager_ptr->dummy_vertex_buffer, staging_buffer.GetOffset(), 0, size);
        }

        gl_state.program = 0;
        gl_state.active_texture = GL_TEXTURE0;
        gl_state.active_texture_index = 0;
        for (GLuint i = 0; i < Vulkan::MAX_COLOR_ATTACHMENTS; i++) {
            gl_state.texture_binding_2d[i] = 0;
            gl_state.texture_binding_cube_map[i] = 0;
            gl_state.sampler_binding[i] = 0;
        }

        gl_state.blend = GL_FALSE;
        gl_state.blend_src_rgb = GL_ONE;
        gl_state.blend_src_alpha = GL_ONE;
        gl_state.blend_dst_rgb = GL_ZERO;
        gl_state.blend_dst_alpha = GL_ZERO;
        gl_state.blend_mode_rgb = GL_FUNC_ADD;
        gl_state.blend_mode_alpha = GL_FUNC_ADD;

        gl_state.framebuffer_binding = 0;
        gl_state.read_framebuffer_binding = 0;
        gl_state.draw_framebuffer_binding = 0;
        gl_state.vertex_array_binding = 0;
        gl_state.array_buffer_binding = 0;
        gl_state.element_array_buffer_binding = 0;

        gl_state.uniform_buffer_binding = 0;
        for (GLuint i = 0; i < 14; i++) {
            gl_state.uniform_buffer_bindings[i] = 0;
            gl_state.uniform_buffer_offsets[i] = 0;
            gl_state.uniform_buffer_sizes[i] = 0;
        }

        gl_state.shader_storage_buffer_binding = 0;
        for (GLuint i = 0; i < 14; i++) {
            gl_state.shader_storage_buffer_bindings[i] = 0;
            gl_state.shader_storage_buffer_offsets[i] = 0;
            gl_state.shader_storage_buffer_sizes[i] = 0;
        }

        gl_state.color_mask[0] = GL_TRUE;
        gl_state.color_mask[1] = GL_TRUE;
        gl_state.color_mask[2] = GL_TRUE;
        gl_state.color_mask[3] = GL_TRUE;
        gl_state.cull_face = GL_FALSE;
        gl_state.cull_face_mode = GL_BACK;
        gl_state.depth_test = GL_FALSE;
        gl_state.depth_func = GL_LESS;
        gl_state.depth_mask = GL_TRUE;
        gl_state.line_width = 1.0f;
        gl_state.multisample = GL_FALSE;
        gl_state.primitive_restart = GL_FALSE;
        gl_state.primitive_restart_index = 0;
        gl_state.scissor_box.x = 0;
        gl_state.scissor_box.y = 0;
        gl_state.scissor_box.width = 0;
        gl_state.scissor_box.height = 0;
        gl_state.scissor_test = GL_FALSE;
        gl_state.stencil_test = GL_FALSE;
        gl_state.stencil_mask = 0xFFFFFFFF;
        gl_state.viewport.x = 0;
        gl_state.viewport.y = 0;
        gl_state.viewport.width = 0;
        gl_state.viewport.height = 0;
        gl_state.polygon_mode = GL_FILL;
    }

    VkBuffer gl_wrap_manager_get_dummy_vertex_buffer() {
        return gl_wrap_manager_ptr->dummy_vertex_buffer;
    }

    GLuint gl_wrap_manager_get_query_samples_passed() {
        return gl_wrap_manager_ptr->query_samples_passed;
    }

    void gl_wrap_manager_update_buffers() {
        gl_wrap_manager_ptr->update_buffers();
    }

    void gl_wrap_manager_load_funcs() {
        GLAD_GL_VERSION_1_0 = 1;
        GLAD_GL_VERSION_1_1 = 1;
        GLAD_GL_VERSION_1_2 = 1;
        GLAD_GL_VERSION_1_3 = 1;
        GLAD_GL_VERSION_1_4 = 1;
        GLAD_GL_VERSION_1_5 = 1;
        GLAD_GL_VERSION_2_0 = 1;
        GLAD_GL_VERSION_2_1 = 1;
        GLAD_GL_VERSION_3_0 = 1;
        GLAD_GL_VERSION_3_1 = 1;
        GLAD_GL_VERSION_3_2 = 1;
        GLAD_GL_VERSION_3_3 = 1;
        GLAD_GL_VERSION_3_0 = 1;
        GLAD_GL_VERSION_4_1 = 1;
        GLAD_GL_VERSION_4_2 = 1;
        GLAD_GL_VERSION_4_3 = 1;
        GLAD_GL_VERSION_4_4 = 1;
        GLAD_GL_VERSION_4_5 = 1;
        GLAD_GL_VERSION_4_6 = 1;

        glBeginQuery = gl_wrap_manager_begin_query;
        glBindBuffer = gl_wrap_manager_bind_buffer;
        glBindTexture = gl_wrap_manager_bind_texture;
        glBlitFramebuffer = gl_wrap_manager_blit_framebuffer;
        glBlitNamedFramebuffer = gl_wrap_manager_blit_named_framebuffer;
        glBufferData = gl_wrap_manager_buffer_data;
        glBufferStorage = gl_wrap_manager_buffer_storage;
        glBufferSubData = gl_wrap_manager_buffer_sub_data;
        glCheckFramebufferStatus = gl_wrap_manager_check_framebuffer_status;
        glClear = gl_wrap_manager_clear;
        glClearBufferfv = gl_wrap_manager_clear_buffer;
        glClearColor = gl_wrap_manager_clear_color;
        glClearDepthf = gl_wrap_manager_clear_depth;
        glCompressedTexImage2D = gl_wrap_manager_compressed_tex_image_2d;
        glCompressedTexSubImage2D = gl_wrap_manager_compressed_tex_sub_image_2d;
        glCopyImageSubData = gl_wrap_manager_copy_image_sub_data;
        glCopyTexSubImage2D = gl_wrap_manager_copy_tex_sub_image_2d;
        glCreateBuffers = gl_wrap_manager_create_buffers;
        glCreateProgram = gl_wrap_manager_create_program;
        glDeleteBuffers = gl_wrap_manager_delete_buffers;
        glDeleteFramebuffers = gl_wrap_manager_delete_framebuffers;
        glDeleteProgram = gl_wrap_manager_delete_program;
        glDeleteQueries = gl_wrap_manager_delete_queries;
        glDeleteSamplers = gl_wrap_manager_delete_samplers;
        glDeleteTextures = gl_wrap_manager_delete_textures;
        glDeleteVertexArrays = gl_wrap_manager_delete_vertex_arrays;
        glDisableVertexAttribArray = gl_wrap_manager_disable_vertex_attrib_array;
        glDrawBuffer = gl_wrap_manager_draw_buffer;
        glDrawBuffers = gl_wrap_manager_draw_buffers;
        glEnableVertexAttribArray = gl_wrap_manager_enable_vertex_attrib_array;
        glEndQuery = gl_wrap_manager_end_query;
        glFinish = gl_wrap_manager_finish;
        glFramebufferTexture = gl_wrap_manager_framebuffer_texture;
        glGenBuffers = gl_wrap_manager_gen_buffers;
        glGenFramebuffers = gl_wrap_manager_gen_framebuffers;
        glGenQueries = gl_wrap_manager_gen_queries;
        glGenSamplers = gl_wrap_manager_gen_samplers;
        glGenTextures = gl_wrap_manager_gen_textures;
        glGenVertexArrays = gl_wrap_manager_gen_vertex_arrays;
        glGenerateMipmap = gl_wrap_manager_generate_mipmap;
        glGenerateTextureMipmap = gl_wrap_manager_generate_texture_mipmap;
        glGetCompressedTexImage = gl_wrap_manager_get_compressed_tex_image;
        glGetError = gl_wrap_manager_get_error;
        glGetFloatv = gl_wrap_manager_get_float;
        glGetQueryObjectiv = gl_wrap_manager_get_query_object;
        glGetQueryObjectuiv = gl_wrap_manager_get_query_object;
        glGetTexImage = gl_wrap_manager_get_tex_image;
        glMapBuffer = gl_wrap_manager_map_buffer;
        glMapNamedBuffer = gl_wrap_manager_map_named_buffer;
        glNamedBufferData = gl_wrap_manager_named_buffer_data;
        glNamedBufferStorage = gl_wrap_manager_named_buffer_storage;
        glNamedBufferSubData = gl_wrap_manager_named_buffer_sub_data;
        glPixelStorei = gl_wrap_manager_pixel_store;
        glPopDebugGroup = gl_wrap_manager_pop_debug_group;
        glPushDebugGroup = gl_wrap_manager_push_debug_group;
        glReadBuffer = gl_wrap_manager_read_buffer;
        glSamplerParameterf = gl_wrap_manager_sampler_parameter;
        glSamplerParameterfv = gl_wrap_manager_sampler_parameter;
        glSamplerParameteri = gl_wrap_manager_sampler_parameter;
        glSamplerParameteriv = gl_wrap_manager_sampler_parameter;
        glTexImage2D = gl_wrap_manager_tex_image_2d;
        glTexParameterf = gl_wrap_manager_tex_parameter;
        glTexParameterfv = gl_wrap_manager_tex_parameter;
        glTexParameteri = gl_wrap_manager_tex_parameter;
        glTexParameteriv = gl_wrap_manager_tex_parameter;
        glTexSubImage2D = gl_wrap_manager_tex_sub_image_2d;
        glTextureParameterf = gl_wrap_manager_texture_parameter;
        glTextureParameterfv = gl_wrap_manager_texture_parameter;
        glTextureParameteri = gl_wrap_manager_texture_parameter;
        glTextureParameteriv = gl_wrap_manager_texture_parameter;
        glTextureSubImage2D = gl_wrap_manager_texture_sub_image_2d;
        glUnmapBuffer = gl_wrap_manager_unmap_buffer;
        glUnmapNamedBuffer = gl_wrap_manager_unmap_named_buffer;
        glVertexAttrib4f = gl_wrap_manager_vertex_attrib;
        glVertexAttribIPointer = gl_wrap_manager_vertex_attrib_int_pointer;
        glVertexAttribPointer = gl_wrap_manager_vertex_attrib_pointer;
    }

    void gl_wrap_manager_free() {
        gl_wrap_manager_ptr->dummy_vertex_buffer.Destroy();

        delete gl_wrap_manager_ptr;
    }

    VkImageAspectFlags get_aspect_mask(GLenum internal_format) {
        switch (Vulkan::get_format(internal_format)) {
        case VK_FORMAT_D24_UNORM_S8_UINT:
            return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        case VK_FORMAT_D32_SFLOAT:
            return VK_IMAGE_ASPECT_DEPTH_BIT;
        default:
            return VK_IMAGE_ASPECT_COLOR_BIT;
        }
    }

    VkBlendFactor get_blend_factor(GLenum factor) {
        switch (factor) {
        case GL_ZERO:
            return VK_BLEND_FACTOR_ZERO;
        case GL_ONE:
            return VK_BLEND_FACTOR_ONE;
        case GL_SRC_COLOR:
            return VK_BLEND_FACTOR_SRC_COLOR;
        case GL_ONE_MINUS_SRC_COLOR:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        case GL_DST_COLOR:
            return VK_BLEND_FACTOR_DST_COLOR;
        case GL_ONE_MINUS_DST_COLOR:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        case GL_SRC_ALPHA:
            return VK_BLEND_FACTOR_SRC_ALPHA;
        case GL_ONE_MINUS_SRC_ALPHA:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case GL_DST_ALPHA:
            return VK_BLEND_FACTOR_DST_ALPHA;
        case GL_ONE_MINUS_DST_ALPHA:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        case GL_CONSTANT_COLOR:
            return VK_BLEND_FACTOR_CONSTANT_COLOR;
        case GL_ONE_MINUS_CONSTANT_COLOR:
            return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
        case GL_CONSTANT_ALPHA:
            return VK_BLEND_FACTOR_CONSTANT_ALPHA;
        case GL_ONE_MINUS_CONSTANT_ALPHA:
            return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
        case GL_SRC_ALPHA_SATURATE:
            return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
        case GL_SRC1_COLOR:
            return VK_BLEND_FACTOR_SRC1_COLOR;
        case GL_ONE_MINUS_SRC1_COLOR:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
        case GL_SRC1_ALPHA:
            return VK_BLEND_FACTOR_SRC1_ALPHA;
        case GL_ONE_MINUS_SRC1_ALPHA:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
        default:
            return VK_BLEND_FACTOR_MAX_ENUM;
        }
    }

    VkBlendOp get_blend_op(GLenum mode) {
        switch (mode) {
        case GL_FUNC_ADD:
            return VK_BLEND_OP_ADD;
        case GL_FUNC_SUBTRACT:
            return VK_BLEND_OP_SUBTRACT;
        case GL_FUNC_REVERSE_SUBTRACT:
            return VK_BLEND_OP_REVERSE_SUBTRACT;
        case GL_MIN:
            return VK_BLEND_OP_MIN;
        case GL_MAX:
            return VK_BLEND_OP_MAX;
        default:
            return VK_BLEND_OP_MAX_ENUM;
        }
    }

    VkCompareOp get_compare_op(GLenum func) {
        switch (func) {
        case GL_NEVER:
            return VK_COMPARE_OP_NEVER;
        case GL_LESS:
            return VK_COMPARE_OP_LESS;
        case GL_EQUAL:
            return VK_COMPARE_OP_EQUAL;
        case GL_LEQUAL:
            return VK_COMPARE_OP_LESS_OR_EQUAL;
        case GL_GREATER:
            return VK_COMPARE_OP_GREATER;
        case GL_NOTEQUAL:
            return VK_COMPARE_OP_NOT_EQUAL;
        case GL_GEQUAL:
            return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case GL_ALWAYS:
            return VK_COMPARE_OP_ALWAYS;
        default:
            return VK_COMPARE_OP_MAX_ENUM;
        }
    }

    VkComponentSwizzle get_component_swizzle(GLenum swizzle, GLenum comp) {
        if (swizzle == comp)
            return VK_COMPONENT_SWIZZLE_IDENTITY;

        switch (swizzle) {
        case GL_ZERO:
            return VK_COMPONENT_SWIZZLE_ZERO;
        case GL_ONE:
            return VK_COMPONENT_SWIZZLE_ONE;
        case GL_RED:
            return VK_COMPONENT_SWIZZLE_R;
        case GL_GREEN:
            return VK_COMPONENT_SWIZZLE_G;
        case GL_BLUE:
            return VK_COMPONENT_SWIZZLE_B;
        case GL_ALPHA:
            return VK_COMPONENT_SWIZZLE_A;
        default:
            return VK_COMPONENT_SWIZZLE_MAX_ENUM;
        }
    }

    VkCullModeFlags get_cull_mode_flags(GLenum mode) {
        switch (mode) {
        case GL_NONE:
            return VK_CULL_MODE_NONE;
        case GL_FRONT:
            return VK_CULL_MODE_FRONT_BIT;
        case GL_BACK:
            return VK_CULL_MODE_BACK_BIT;
        case GL_FRONT_AND_BACK:
            return VK_CULL_MODE_FRONT_AND_BACK;
        default:
            return VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
        }
    }

    VkFormat get_format(GLenum internal_format) {
        switch (internal_format) {
        case GL_ALPHA8:
            return VK_FORMAT_R8_UNORM;
        case GL_LUMINANCE8:
            return VK_FORMAT_R8_UNORM;
        case GL_LUMINANCE8_ALPHA8:
            return VK_FORMAT_R8G8_UNORM;
        case GL_INTENSITY8:
            return VK_FORMAT_R8_UNORM;
        case GL_RGB5:
            return VK_FORMAT_R5G6B5_UNORM_PACK16;
        case GL_RGBA4:
            return VK_FORMAT_R4G4B4A4_UNORM_PACK16;
        case GL_RGB5_A1:
            return VK_FORMAT_R5G5B5A1_UNORM_PACK16;
        case GL_RGB8:
        case GL_RGBA8:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case GL_DEPTH_COMPONENT24:
            return VK_FORMAT_D24_UNORM_S8_UINT;
        case GL_R8:
            return VK_FORMAT_R8_UNORM;
        case GL_RG8:
            return VK_FORMAT_R8G8_UNORM;
        case GL_R32F:
            return VK_FORMAT_R32_SFLOAT;
        case GL_RG16F:
            return VK_FORMAT_R16G16_SFLOAT;
        case GL_RG32F:
            return VK_FORMAT_R32G32_SFLOAT;
        case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
            return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
        case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
            return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
        case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
            return VK_FORMAT_BC2_UNORM_BLOCK;
        case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
            return VK_FORMAT_BC3_UNORM_BLOCK;
        case GL_RGBA32F:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case GL_RGBA16F:
            return VK_FORMAT_R16G16B16A16_SFLOAT;
        case GL_DEPTH24_STENCIL8:
            return VK_FORMAT_D24_UNORM_S8_UINT;
        case GL_R11F_G11F_B10F:
            return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
        case GL_RGB9_E5:
            return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
        case GL_DEPTH_COMPONENT32F:
            return VK_FORMAT_D32_SFLOAT;
        case GL_COMPRESSED_RED_RGTC1:
            return VK_FORMAT_BC4_UNORM_BLOCK;
        case GL_COMPRESSED_RG_RGTC2:
            return VK_FORMAT_BC5_UNORM_BLOCK;
        default:
            return VK_FORMAT_UNDEFINED;
        }
    }

    VkImageViewType get_image_view_type(GLenum target) {
        switch (target) {
        case GL_TEXTURE_2D:
            return VK_IMAGE_VIEW_TYPE_2D;
        case GL_TEXTURE_CUBE_MAP:
            return VK_IMAGE_VIEW_TYPE_CUBE;
        default:
            return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
        }
    }

    VkPolygonMode get_polygon_mode(GLenum mode) {
        switch (mode) {
        case GL_POINT:
            return VK_POLYGON_MODE_POINT;
        case GL_LINE:
            return VK_POLYGON_MODE_LINE;
        case GL_FILL:
            return VK_POLYGON_MODE_FILL;
        default:
            return VK_POLYGON_MODE_MAX_ENUM;
        }
    }

    gl_texture_data::tex_data::tex_data() {

    }

    gl_texture_data::tex_data::~tex_data() {

    }

    void gl_texture_data::tex_data::set_data(GLint internal_format, GLint xoffset, GLint yoffset,
        GLsizei width, GLsizei height, GLenum format, GLenum type, const void* data) {
        this->data.clear();
        if (!data)
            return;

        GLenum _format;
        GLenum _type;
        texture_get_format_type_by_internal_format(internal_format, &_format, &_type);
        if (_format != format)
            return;

        this->data.resize(texture_get_size(internal_format, width, height));
        if (!gl_texture_data::tex_data::convert(internal_format, width, height, format,
            _type, type, data, this->data.data(), gl_wrap_manager_ptr->unpack_alignment, true))
            this->data.clear();
    }

    bool gl_texture_data::tex_data::convert(GLenum internal_format,
        GLsizei width, GLsizei height, GLenum format, GLenum src_type, GLenum dst_type,
        const void* src_data, void* dst_data, ssize_t alignment, bool unpack) {
        if (!src_type) {
            if (!format)
                return false;

            int32_t block_size;
            switch (internal_format) {
            case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
            case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
            case GL_COMPRESSED_RED_RGTC1:
                block_size = 8;
                break;
            case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
            case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
            case GL_COMPRESSED_RG_RGTC2:
                block_size = 16;
                break;
            default:
                return false;
            }


            memmove(dst_data, src_data, texture_get_size(internal_format, width, height));
            return true;
        }

        if (src_type == GL_UNSIGNED_BYTE && dst_type == GL_UNSIGNED_BYTE && format == GL_RGB) {
            if (unpack) {
                const uint8_t* src = (const uint8_t*)src_data;
                uint8_t* dst = (uint8_t*)dst_data;
                for (GLsizei y = 0; y < height; y++) {
                    for (GLsizei x = 0; x < width; x++) {
                        *dst++ = *src++;
                        *dst++ = *src++;
                        *dst++ = *src++;
                        *dst++ = 0xFF;
                    }

                    const int32_t src_size = width * 3;
                    if (src_size % alignment)
                        src += (ssize_t)alignment - src_size % alignment;
                }
            }
            else {
                const uint8_t* src = (const uint8_t*)src_data;
                uint8_t* dst = (uint8_t*)dst_data;
                for (GLsizei y = 0; y < height; y++) {
                    for (GLsizei x = 0; x < width; x++) {
                        *dst++ = *src++;
                        *dst++ = *src++;
                        *dst++ = *src++;
                        src++;
                    }

                    const int32_t dst_size = width * 4;
                    if (dst_size % alignment)
                        dst += (ssize_t)alignment - dst_size % alignment;
                }
            }
        }
        else if (src_type == GL_HALF_FLOAT && dst_type == GL_FLOAT) {
            int32_t comp_count = 1;
            if (format == GL_RG)
                comp_count = 2;
            else if (format == GL_RGB)
                comp_count = 3;
            else if (format == GL_RGBA)
                comp_count = 4;
            width *= comp_count;

            if (unpack) {
                const uint8_t* src = (const uint8_t*)src_data;
                uint8_t* dst = (uint8_t*)dst_data;
                for (GLsizei y = 0; y < height; y++) {
                    GLsizei x = 0;
                    for (; x < width / 4 * 4; x += 4, src += sizeof(vec4), dst += sizeof(vec4h))
                        vec4_to_vec4h(*(vec4*)src, *(vec4h*)dst);

                    for (; x < width; x++, src += sizeof(float_t), dst += sizeof(half_t))
                        *(half_t*)dst = float_to_half(*(float_t*)src);

                    const int32_t src_size = width * sizeof(half_t);
                    if (src_size % alignment)
                        src += (ssize_t)alignment - src_size % alignment;
                }
            }
            else {
                const uint8_t* src = (const uint8_t*)src_data;
                uint8_t* dst = (uint8_t*)dst_data;
                for (GLsizei y = 0; y < height; y++) {
                    GLsizei x = 0;
                    for (; x < width / 4 * 4; x += 4, src += sizeof(vec4), dst += sizeof(vec4h))
                        vec4_to_vec4h(*(vec4*)src, *(vec4h*)dst);

                    for (; x < width; x++, src += sizeof(float_t), dst += sizeof(half_t))
                        *(half_t*)dst = float_to_half(*(float_t*)src);

                    const int32_t dst_size = width * sizeof(float_t);
                    if (dst_size % alignment)
                        dst += (ssize_t)alignment - dst_size % alignment;
                }
            }
        }
        else if (src_type == GL_FLOAT && dst_type == GL_HALF_FLOAT) {
            int32_t comp_count = 1;
            if (format == GL_RG)
                comp_count = 2;
            else if (format == GL_RGB)
                comp_count = 3;
            else if (format == GL_RGBA)
                comp_count = 4;
            width *= comp_count;

            if (unpack) {
                const uint8_t* src = (const uint8_t*)src_data;
                uint8_t* dst = (uint8_t*)dst_data;
                for (GLsizei y = 0; y < height; y++) {
                    GLsizei x = 0;
                    for (; x < width / 4 * 4; x += 4, src += sizeof(vec4h), dst += sizeof(vec4))
                        vec4h_to_vec4(*(vec4h*)src, *(vec4*)dst);

                    for (; x < width; x++, src += sizeof(half_t), dst += sizeof(float_t))
                        *(float_t*)dst = half_to_float(*(half_t*)src);

                    const int32_t src_size = width * sizeof(float_t);
                    if (src_size % alignment)
                        src += (ssize_t)alignment - src_size % alignment;
                }
            }
            else {
                const uint8_t* src = (const uint8_t*)src_data;
                uint8_t* dst = (uint8_t*)dst_data;
                for (GLsizei y = 0; y < height; y++) {
                    GLsizei x = 0;
                    for (; x < width / 4 * 4; x += 4, src += sizeof(vec4h), dst += sizeof(vec4))
                        vec4h_to_vec4(*(vec4h*)src, *(vec4*)dst);

                    for (; x < width; x++, src += sizeof(half_t), dst += sizeof(float_t))
                        *(float_t*)dst = half_to_float(*(half_t*)src);

                    const int32_t dst_size = width * sizeof(half_t);
                    if (dst_size % alignment)
                        dst += (ssize_t)alignment - dst_size % alignment;
                }
            }
        }
        else {
            int32_t elem_size;
            switch (internal_format) {
            case GL_R8:
                elem_size = 1;
                break;
            case GL_RGB5:
            case GL_RGBA4:
            case GL_RGB5_A1:
            case GL_RG8:
                elem_size = 2;
                break;
            case GL_RGB8:
            case GL_RGBA8:
            case GL_R32F:
            case GL_RG16F:
            case GL_R11F_G11F_B10F:
            case GL_RGB9_E5:
                elem_size = 4;
                break;
            case GL_RGBA16F:
            case GL_RG32F:
                elem_size = 8;
                break;
            case GL_RGBA32F:
                elem_size = 16;
                break;
            default:
                return false;
            }

            if (unpack) {
                const uint8_t* src = (const uint8_t*)src_data;
                uint8_t* dst = (uint8_t*)dst_data;
                for (GLsizei y = 0; y < height; y++) {
                    int32_t size = elem_size * width;
                    memmove(dst, src, size);
                    dst += size;
                    src += size;

                    if (size % alignment)
                        src += (ssize_t)alignment - size % alignment;
                }
            }
            else {
                const uint8_t* src = (const uint8_t*)src_data;
                uint8_t* dst = (uint8_t*)dst_data;
                for (GLsizei y = 0; y < height; y++) {
                    int32_t size = elem_size * width;
                    memmove(dst, src, size);
                    dst += size;
                    src += size;

                    if (size % alignment)
                        dst += (ssize_t)alignment - size % alignment;
                }
            }
        }
        return true;
    }

    gl_texture_data::tex_data& gl_texture_data::tex_data::operator=(const gl_texture_data::tex_data& other) {
        data.assign(other.data.begin(), other.data.end());
        return *this;
    };

    gl_texture_data::gl_texture_data() : target(), level_count(), internal_format(), width(), height() {

    }

    gl_texture_data::~gl_texture_data() {

    }

    gl_texture_data::tex_data* gl_texture_data::get_tex_data(uint32_t level, uint32_t layer, bool get) {
        if (target != GL_TEXTURE_CUBE_MAP && layer
            || target == GL_TEXTURE_CUBE_MAP && (layer < 0 || layer >= 6))
            return 0;

        if (data[layer].size() <= level) {
            if (get)
                return 0;

            data[layer].resize(level + 1ULL);
            level_count = level + 1;
        }
        return &data[layer].data()[level];
    }

    bool gl_texture_data::valid() {
        return width && height;
    }

    gl_texture_data& gl_texture_data::operator=(const gl_texture_data& other) {
        target = other.target;
        internal_format = other.internal_format;
        width = other.width;
        height = other.height;
        level_count = other.level_count;
        for (uint32_t i = 0; i < 6; i++)
            data[i].assign(other.data[i].begin(), other.data[i].end());
        return *this;
    };

    gl_texture_data* gl_texture_data::get(GLuint texture) {
        if (!texture)
            return 0;

        auto elem = gl_wrap_manager_ptr->gl_texture_datas.find(texture);
        if (elem != gl_wrap_manager_ptr->gl_texture_datas.end())
            return &elem->second;
        return 0;
    }

    gl_wrap_manager::gl_wrap_manager() : buffer_counter(), framebuffer_counter(),
        query_counter(), program_counter(), sampler_counter(),
        texture_counter(), vertex_array_counter(), query_samples_passed() {
        clear_color = 0.0f;
        clear_depth = 1.0f;
        pack_alignment = 4;
        unpack_alignment = 4;
    }

    gl_wrap_manager::~gl_wrap_manager() {
        for (auto& i : gl_vertex_buffers)
            i.second.vertex_buffer.Destroy();
        for (auto& i : gl_uniform_buffers)
            i.second.uniform_buffer.Destroy();
        for (auto& i : gl_storage_buffers)
            i.second.storage_buffer.Destroy();
        for (auto& i : gl_queries)
            i.second.query.Destroy();
        for (auto& i : gl_index_buffers)
            i.second.index_buffer.Destroy();
        for (auto& i : gl_framebuffers)
            i.second.release();
        for (auto& i : gl_textures) {
            i.second.sample_image_view.Destroy();
            i.second.image_view.Destroy();
            i.second.image.Destroy();
        }
    }

    GLuint gl_wrap_manager::create_buffer() {
        if (!++buffer_counter)
            buffer_counter = 1;

        gl_buffers.insert({ buffer_counter, {} });
        return buffer_counter;
    }

    GLuint gl_wrap_manager::create_framebuffer() {
        if (!++framebuffer_counter)
            framebuffer_counter = 1;

        gl_framebuffers.insert({ framebuffer_counter, {} });
        return framebuffer_counter;
    }

    GLuint gl_wrap_manager::create_program() {
        if (!++program_counter)
            program_counter = 1;

        gl_programs.insert({ program_counter, {} });
        return program_counter;
    }

    GLuint gl_wrap_manager::create_query() {
        if (!++query_counter)
            query_counter = 1;

        gl_queries.insert({ query_counter, {} });
        return query_counter;
    }

    GLuint gl_wrap_manager::create_sampler() {
        if (!++sampler_counter)
            sampler_counter = 1;

        gl_samplers.insert({ sampler_counter, {} });
        return sampler_counter;
    }

    GLuint gl_wrap_manager::create_texture() {
        if (!++texture_counter)
            texture_counter = 1;

        gl_textures.insert({ texture_counter, {} });
        return texture_counter;
    }

    GLuint gl_wrap_manager::create_vertex_array() {
        if (!++vertex_array_counter)
            vertex_array_counter = 1;

        gl_vertex_arrays.insert({ vertex_array_counter, {} });
        return vertex_array_counter;
    }

    GLenum gl_wrap_manager::get_error() {
        if (!this || !gl_errors.size())
            return GL_NO_ERROR;

        GLenum error = gl_errors.back();
        gl_errors.pop_back();
        return error;
    }

    void gl_wrap_manager::push_error(GLenum error) {
        gl_errors.insert(gl_errors.begin(), error);
    }

    void gl_wrap_manager::release_buffer(GLuint buffer) {
        auto elem = gl_buffers.find(buffer);
        if (elem != gl_buffers.end()) {
            switch (elem->second.target) {
            case GL_ARRAY_BUFFER: {
                auto elem = gl_vertex_buffers.find(buffer);
                if (elem != gl_vertex_buffers.end()) {
                    elem->second.vertex_buffer.Destroy();
                    gl_vertex_buffers.erase(elem);
                }
            } break;
            case GL_ELEMENT_ARRAY_BUFFER: {
                auto elem = gl_index_buffers.find(buffer);
                if (elem != gl_index_buffers.end()) {
                    elem->second.index_buffer.Destroy();
                    gl_index_buffers.erase(elem);
                }
            } break;
            case GL_UNIFORM_BUFFER: {
                auto elem = gl_uniform_buffers.find(buffer);
                if (elem != gl_uniform_buffers.end()) {
                    elem->second.uniform_buffer.Destroy();
                    gl_uniform_buffers.erase(elem);
                }
            } break;
            case GL_SHADER_STORAGE_BUFFER: {
                auto elem = gl_storage_buffers.find(buffer);
                if (elem != gl_storage_buffers.end()) {
                    elem->second.storage_buffer.Destroy();
                    gl_storage_buffers.erase(elem);
                }
            } break;
            }

            gl_buffers.erase(elem);
        }
    }

    void gl_wrap_manager::release_framebuffer(GLuint framebuffer) {
        if (!framebuffer)
            return;

        auto elem = gl_framebuffers.find(framebuffer);
        if (elem != gl_framebuffers.end()) {
            elem->second.release();
            gl_framebuffers.erase(elem);
        }
    }

    void gl_wrap_manager::release_program(GLuint program) {
        if (!program)
            return;

        auto elem = gl_programs.find(program);
        if (elem != gl_programs.end()) {
            elem->second.vertex_shader_module.reset();
            elem->second.fragment_shader_module.reset();
            gl_programs.erase(elem);
        }
    }

    void gl_wrap_manager::release_query(GLuint query) {
        if (!query)
            return;

        auto elem = gl_queries.find(query);
        if (elem != gl_queries.end()) {
            elem->second.query.Destroy();
            gl_queries.erase(elem);
        }
    }

    void gl_wrap_manager::release_sampler(GLuint sampler) {
        if (!sampler)
            return;

        auto elem = gl_samplers.find(sampler);
        if (elem != gl_samplers.end())
            gl_samplers.erase(elem);
    }

    void gl_wrap_manager::release_texture(GLuint texture) {
        if (!texture)
            return;

        auto elem = gl_textures.find(texture);
        if (elem != gl_textures.end()) {
            elem->second.sample_image_view.Destroy();
            elem->second.image_view.Destroy();
            elem->second.image.Destroy();
            gl_textures.erase(elem);
        }

        auto elem_data = gl_texture_datas.find(texture);
        if (elem_data != gl_texture_datas.end())
            gl_texture_datas.erase(elem_data);
    }

    void gl_wrap_manager::release_vertex_array(GLuint array) {
        if (!array)
            return;

        auto elem = gl_vertex_arrays.find(array);
        if (elem != gl_vertex_arrays.end())
            gl_vertex_arrays.erase(elem);
    }

    void gl_wrap_manager::update_buffers() {
        for (auto& i : gl_index_buffers) {
            Vulkan::gl_index_buffer* vk_ib = &i.second;
            if (vk_ib->copy_working_buffer) {
                VkBuffer src_buffer = vk_ib->working_buffer;
                VkBuffer dst_buffer = vk_ib->index_buffer;
                VkDeviceSize src_offset = vk_ib->working_buffer.GetOffset();
                VkDeviceSize dst_offset = vk_ib->index_buffer.GetOffset();
                VkDeviceSize size = vk_ib->working_buffer.GetSize();
                Vulkan::Buffer::Copy(Vulkan::current_command_buffer,
                    src_buffer, dst_buffer, src_offset, dst_offset, size);

                VkBufferMemoryBarrier barrier;
                barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
                barrier.pNext = 0;
                barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_INDEX_READ_BIT;
                barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.buffer = dst_buffer;
                barrier.offset = dst_offset;
                barrier.size = size;

                vkCmdPipelineBarrier(Vulkan::current_command_buffer,
                    VK_PIPELINE_STAGE_HOST_BIT,
                    VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0, 0, 1, &barrier, 0, 0);
                vk_ib->copy_working_buffer = false;
            }
            vk_ib->working_buffer.Reset();
        }

        for (auto& i : gl_storage_buffers) {
            Vulkan::gl_storage_buffer* vk_sb = &i.second;
            if (vk_sb->copy_working_buffer) {
                VkBuffer src_buffer = vk_sb->working_buffer;
                VkBuffer dst_buffer = vk_sb->storage_buffer;
                VkDeviceSize src_offset = vk_sb->working_buffer.GetOffset();
                VkDeviceSize dst_offset = vk_sb->storage_buffer.GetOffset();
                VkDeviceSize size = vk_sb->working_buffer.GetSize();
                Vulkan::Buffer::Copy(Vulkan::current_command_buffer,
                    src_buffer, dst_buffer, src_offset, dst_offset, size);

                VkBufferMemoryBarrier barrier;
                barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
                barrier.pNext = 0;
                barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.buffer = dst_buffer;
                barrier.offset = dst_offset;
                barrier.size = size;

                vkCmdPipelineBarrier(Vulkan::current_command_buffer,
                    VK_PIPELINE_STAGE_HOST_BIT,
                    VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
                    | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, 0, 1, &barrier, 0, 0);
                vk_sb->copy_working_buffer = false;
            }
            vk_sb->working_buffer.Reset();
        }

        for (auto& i : gl_uniform_buffers) {
            Vulkan::gl_uniform_buffer* vk_ub = &i.second;
            if (vk_ub->copy_working_buffer) {
                VkBuffer src_buffer = vk_ub->working_buffer;
                VkBuffer dst_buffer = vk_ub->uniform_buffer;
                VkDeviceSize src_offset = vk_ub->working_buffer.GetOffset();
                VkDeviceSize dst_offset = vk_ub->uniform_buffer.GetOffset();
                VkDeviceSize size = vk_ub->working_buffer.GetSize();
                Vulkan::Buffer::Copy(Vulkan::current_command_buffer,
                    src_buffer, dst_buffer, src_offset, dst_offset, size);

                VkBufferMemoryBarrier barrier;
                barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
                barrier.pNext = 0;
                barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_UNIFORM_READ_BIT;
                barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.buffer = dst_buffer;
                barrier.offset = dst_offset;
                barrier.size = size;

                vkCmdPipelineBarrier(Vulkan::current_command_buffer,
                    VK_PIPELINE_STAGE_HOST_BIT,
                    VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
                    | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, 0, 1, &barrier, 0, 0);
                vk_ub->copy_working_buffer = false;
            }
            vk_ub->working_buffer.Reset();
        }

        for (auto& i : gl_vertex_buffers) {
            Vulkan::gl_vertex_buffer* vk_vb = &i.second;
            if (vk_vb->copy_working_buffer) {
                VkBuffer src_buffer = vk_vb->working_buffer;
                VkBuffer dst_buffer = vk_vb->vertex_buffer;
                VkDeviceSize src_offset = vk_vb->working_buffer.GetOffset();
                VkDeviceSize dst_offset = vk_vb->vertex_buffer.GetOffset();
                VkDeviceSize size = vk_vb->working_buffer.GetSize();
                Vulkan::Buffer::Copy(Vulkan::current_command_buffer,
                    src_buffer, dst_buffer, src_offset, dst_offset, size);

                VkBufferMemoryBarrier barrier;
                barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
                barrier.pNext = 0;
                barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
                barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.buffer = dst_buffer;
                barrier.offset = dst_offset;
                barrier.size = size;

                vkCmdPipelineBarrier(Vulkan::current_command_buffer,
                    VK_PIPELINE_STAGE_HOST_BIT,
                    VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0, 0, 1, &barrier, 0, 0);
                vk_vb->copy_working_buffer = false;
            }
            vk_vb->working_buffer.Reset();
        }
    }

    static void gl_wrap_manager_begin_query(GLenum target, GLuint id) {
        if (target != GL_SAMPLES_PASSED) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        gl_query* vk_que_curr = gl_query::get(gl_wrap_manager_ptr->query_samples_passed);
        if (vk_que_curr && vk_que_curr->target == target || !id || gl_wrap_manager_ptr->query_samples_passed == id) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        gl_query* vk_que = gl_query::get(id);
        if (!vk_que)
            return;
        else if (vk_que->target && vk_que->target != target) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        if (!vk_que->target) {
            vk_que->target = target;
            vk_que->query.Create(Vulkan::current_device, 0, VK_QUERY_TYPE_OCCLUSION);
        }

        gl_wrap_manager_ptr->query_samples_passed = id;
    }

    static void gl_wrap_manager_bind_buffer(GLenum target, GLuint buffer) {
        switch (target) {
        case GL_ARRAY_BUFFER: {
            auto elem = gl_wrap_manager_ptr->gl_vertex_buffers.find(buffer);
            if (elem == gl_wrap_manager_ptr->gl_vertex_buffers.end()) {
                auto elem = gl_wrap_manager_ptr->gl_buffers.find(buffer);
                if (elem == gl_wrap_manager_ptr->gl_buffers.end()) {
                    if (buffer)
                        gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
                    return;
                }

                elem->second.target = GL_ARRAY_BUFFER;
                gl_wrap_manager_ptr->gl_vertex_buffers.insert({ buffer, {} });
            }
        } break;
        case GL_ELEMENT_ARRAY_BUFFER: {
            auto elem = gl_wrap_manager_ptr->gl_index_buffers.find(buffer);
            if (elem == gl_wrap_manager_ptr->gl_index_buffers.end()) {
                auto elem = gl_wrap_manager_ptr->gl_buffers.find(buffer);
                if (elem == gl_wrap_manager_ptr->gl_buffers.end()) {
                    if (buffer)
                        gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
                    return;
                }

                elem->second.target = GL_ELEMENT_ARRAY_BUFFER;
                gl_wrap_manager_ptr->gl_index_buffers.insert({ buffer, {} });
            }

            gl_vertex_array* vk_vao = gl_vertex_array::get(gl_state.vertex_array_binding);
            if (vk_vao)
                vk_vao->index_buffer_binding.buffer = buffer;
        } break;
        case GL_UNIFORM_BUFFER: {
            auto elem = gl_wrap_manager_ptr->gl_uniform_buffers.find(buffer);
            if (elem == gl_wrap_manager_ptr->gl_uniform_buffers.end()) {
                auto elem = gl_wrap_manager_ptr->gl_buffers.find(buffer);
                if (elem == gl_wrap_manager_ptr->gl_buffers.end()) {
                    if (buffer)
                        gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
                    return;
                }

                elem->second.target = GL_UNIFORM_BUFFER;
                gl_wrap_manager_ptr->gl_uniform_buffers.insert({ buffer, {} });
            }
        } break;
        case GL_SHADER_STORAGE_BUFFER: {
            auto elem = gl_wrap_manager_ptr->gl_storage_buffers.find(buffer);
            if (elem == gl_wrap_manager_ptr->gl_storage_buffers.end()) {
                auto elem = gl_wrap_manager_ptr->gl_buffers.find(buffer);
                if (elem == gl_wrap_manager_ptr->gl_buffers.end()) {
                    if (buffer)
                        gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
                    return;
                }

                elem->second.target = GL_SHADER_STORAGE_BUFFER;
                gl_wrap_manager_ptr->gl_storage_buffers.insert({ buffer, {} });
            }
        } break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            break;
        }
    }

    static void gl_wrap_manager_bind_texture(GLenum target, GLuint texture) {
        if (!texture)
            return;

        switch (target) {
        case GL_TEXTURE_2D:
        case GL_TEXTURE_CUBE_MAP:
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        gl_texture* vk_tex = gl_texture::get(texture, false);
        if (!vk_tex) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }
        else if (vk_tex->target && vk_tex->target != target) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        vk_tex->target = target;
    }

    static void gl_wrap_manager_blit_framebuffer(
        GLint src_x0, GLint src_y0, GLint src_x1, GLint src_y1,
        GLint dst_x0, GLint dst_y0, GLint dst_x1, GLint dst_y1, GLbitfield mask, GLenum filter) {
        gl_wrap_manager_blit_named_framebuffer(gl_state.read_framebuffer_binding, gl_state.draw_framebuffer_binding,
            src_x0, src_y0, src_x1, src_y1, dst_x0, dst_y0, dst_x1, dst_y1, mask, filter);
    }

    static void gl_wrap_manager_blit_named_framebuffer(GLuint readFramebuffer, GLuint drawFramebuffer,
        GLint src_x0, GLint src_y0, GLint src_x1, GLint src_y1,
        GLint dst_x0, GLint dst_y0, GLint dst_x1, GLint dst_y1, GLbitfield mask, GLenum filter) {
        if (!readFramebuffer && !drawFramebuffer || (mask & ~GL_COLOR_BUFFER_BIT)) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        VkFilter vk_filter;
        switch (filter) {
        case GL_NEAREST:
            vk_filter = VK_FILTER_NEAREST;
            break;
        case GL_LINEAR:
            vk_filter = VK_FILTER_LINEAR;
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        gl_framebuffer* vk_src_fbo = gl_framebuffer::get(readFramebuffer);
        gl_framebuffer* vk_dst_fbo = gl_framebuffer::get(drawFramebuffer);

        if (!vk_src_fbo && !vk_dst_fbo) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        VkImage src_image;
        VkImage dst_image;
        VkImageLayout src_old_layout;
        VkImageLayout dst_old_layout;

        if (!vk_src_fbo) {
            src_image = Vulkan::current_swapchain_image;
            src_old_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        }
        else {
            Vulkan::Image& vk_src_image = gl_texture::get(vk_src_fbo->get_read_buffer_texture())->image;
            src_image = vk_src_image;
            src_old_layout = vk_src_image.GetImageLayout(0, 0);
        }

        if (!vk_dst_fbo) {
            dst_image = Vulkan::current_swapchain_image;
            dst_old_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        }
        else {
            Vulkan::Image& vk_dst_image = gl_texture::get(vk_dst_fbo->get_draw_buffer_texture())->image;
            dst_image = vk_dst_image;
            dst_old_layout = vk_dst_image.GetImageLayout(0, 0);
        }

        VkImageBlit image_blit_region;
        image_blit_region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_blit_region.srcSubresource.mipLevel = 0;
        image_blit_region.srcSubresource.baseArrayLayer = 0;
        image_blit_region.srcSubresource.layerCount = 1;
        image_blit_region.srcOffsets[0].x = src_x0;
        image_blit_region.srcOffsets[0].y = src_y0;
        image_blit_region.srcOffsets[0].z = 0;
        image_blit_region.srcOffsets[1].x = src_x1 - src_x0;
        image_blit_region.srcOffsets[1].y = src_y1 - src_y0;
        image_blit_region.srcOffsets[1].z = 1;
        image_blit_region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_blit_region.dstSubresource.mipLevel = 0;
        image_blit_region.dstSubresource.baseArrayLayer = 0;
        image_blit_region.dstSubresource.layerCount = 1;
        image_blit_region.dstOffsets[0].x = dst_x0;
        image_blit_region.dstOffsets[0].y = dst_y0;
        image_blit_region.dstOffsets[0].z = 0;
        image_blit_region.dstOffsets[1].x = dst_x1 - dst_x0;
        image_blit_region.dstOffsets[1].y = dst_y1 - dst_y0;
        image_blit_region.dstOffsets[1].z = 1;

        const VkImageLayout src_new_layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        const VkImageLayout dst_new_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

        Vulkan::Image::PipelineBarrierSingle(Vulkan::current_command_buffer, src_image,
            VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, src_old_layout, src_new_layout);
        Vulkan::Image::PipelineBarrierSingle(Vulkan::current_command_buffer, dst_image,
            VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, dst_old_layout, dst_new_layout);
        vkCmdBlitImage(Vulkan::current_command_buffer, src_image,
            src_new_layout, dst_image, dst_new_layout, 1, &image_blit_region, vk_filter);
        Vulkan::Image::PipelineBarrierSingle(Vulkan::current_command_buffer, src_image,
            VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, src_new_layout, src_old_layout);
        Vulkan::Image::PipelineBarrierSingle(Vulkan::current_command_buffer, dst_image,
            VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, dst_new_layout, dst_old_layout);
    }

    static void gl_wrap_manager_buffer_data(
        GLenum target, GLsizeiptr size, const void* data, GLenum usage) {
        switch (target) {
        case GL_ARRAY_BUFFER:
            gl_wrap_manager_named_buffer_data(
                gl_state.array_buffer_binding, size, data, usage);
            break;
        case GL_ELEMENT_ARRAY_BUFFER:
            gl_wrap_manager_named_buffer_data(
                gl_state.element_array_buffer_binding, size, data, usage);
            break;
        case GL_UNIFORM_BUFFER:
            gl_wrap_manager_named_buffer_data(
                gl_state.uniform_buffer_binding, size, data, usage);
            break;
        case GL_SHADER_STORAGE_BUFFER:
            gl_wrap_manager_named_buffer_data(
                gl_state.shader_storage_buffer_binding, size, data, usage);
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            break;
        };
    }

    static void gl_wrap_manager_buffer_storage(
        GLenum target, GLsizeiptr size, const void* data, GLbitfield flags) {
        switch (target) {
        case GL_ARRAY_BUFFER:
            gl_wrap_manager_named_buffer_storage(
                gl_state.array_buffer_binding, size, data, flags);
            break;
        case GL_ELEMENT_ARRAY_BUFFER:
            gl_wrap_manager_named_buffer_storage(
                gl_state.element_array_buffer_binding, size, data, flags);
            break;
        case GL_UNIFORM_BUFFER:
            gl_wrap_manager_named_buffer_storage(
                gl_state.uniform_buffer_binding, size, data, flags);
            break;
        case GL_SHADER_STORAGE_BUFFER:
            gl_wrap_manager_named_buffer_storage(
                gl_state.shader_storage_buffer_binding, size, data, flags);
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            break;
        };
    }

    static void gl_wrap_manager_buffer_sub_data(
        GLenum target, GLintptr offset, GLsizeiptr size, const void* data) {
        switch (target) {
        case GL_ARRAY_BUFFER:
            gl_wrap_manager_named_buffer_sub_data(
                gl_state.array_buffer_binding, offset, size, data);
            break;
        case GL_ELEMENT_ARRAY_BUFFER:
            gl_wrap_manager_named_buffer_sub_data(
                gl_state.element_array_buffer_binding, offset, size, data);
            break;
        case GL_UNIFORM_BUFFER:
            gl_wrap_manager_named_buffer_sub_data(
                gl_state.uniform_buffer_binding, offset, size, data);
            break;
        case GL_SHADER_STORAGE_BUFFER:
            gl_wrap_manager_named_buffer_sub_data(
                gl_state.shader_storage_buffer_binding, offset, size, data);
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            break;
        };
    }

    static GLenum gl_wrap_manager_check_framebuffer_status(GLenum target) {
        GLuint framebuffer;
        switch (target) {
        case GL_FRAMEBUFFER:
            framebuffer = gl_state.framebuffer_binding;
            break;
        case GL_READ_FRAMEBUFFER:
            framebuffer = gl_state.read_framebuffer_binding;
            break;
        case GL_DRAW_FRAMEBUFFER:
            framebuffer = gl_state.draw_framebuffer_binding;
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return 0;
        }

        gl_framebuffer* vk_fbo = gl_framebuffer::get(framebuffer, false);
        if (!vk_fbo) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return 0;
        }

        uint32_t attachment_count = 0;
        for (uint32_t i = 0; i <= Vulkan::MAX_COLOR_ATTACHMENTS; i++) {
            GLuint texture = i == Vulkan::MAX_COLOR_ATTACHMENTS
                ? vk_fbo->depth_attachment : vk_fbo->color_attachments[i];
            if (!texture)
                continue;

            gl_texture* vk_tex = gl_texture::get(texture);
            gl_texture_data* vk_tex_data = gl_texture_data::get(texture);
            if (!vk_tex->image && (!vk_tex_data || !vk_tex_data->valid()))
                return GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;

            if (i == Vulkan::MAX_COLOR_ATTACHMENTS)
                switch (vk_tex->image ? vk_tex->internal_format : vk_tex->internal_format) {
                case GL_DEPTH_COMPONENT16:
                case GL_DEPTH_COMPONENT24:
                case GL_DEPTH24_STENCIL8:
                case GL_DEPTH_COMPONENT32F:
                    break;
                default:
                    return GL_FRAMEBUFFER_UNSUPPORTED;
                }
            else
                switch (vk_tex->image ? vk_tex->internal_format : vk_tex->internal_format) {
                case GL_R8:
                case GL_RG8:
                case GL_RGBA8:
                case GL_R32F:
                case GL_RG16F:
                case GL_R11F_G11F_B10F:
                case GL_RGBA16F:
                case GL_RG32F:
                case GL_RGBA32F:
                    break;
                default:
                    return GL_FRAMEBUFFER_UNSUPPORTED;
                }

            attachment_count++;
        }

        if (!attachment_count)
            return GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT;

        for (uint32_t i = 0; i < Vulkan::MAX_DRAW_BUFFERS; i++)
            if (vk_fbo->draw_buffers[i] && !vk_fbo->get_draw_buffer_texture(i))
                return GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER;

        if (vk_fbo->read_buffer && !vk_fbo->get_read_buffer_texture())
            return GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER;

        return GL_FRAMEBUFFER_COMPLETE;
    }

    static void gl_wrap_manager_clear(GLbitfield mask) {
        if (mask & ~(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        if (mask & GL_COLOR_BUFFER_BIT) {
            const vec4& clear_color = gl_wrap_manager_ptr->clear_color;
            gl_framebuffer* vk_fbo = gl_framebuffer::get(gl_state.draw_framebuffer_binding);

            for (uint32_t i = 0; i < Vulkan::MAX_COLOR_ATTACHMENTS; i++) {
                VkImage image;
                VkImageLayout old_layout;

                if (!vk_fbo) {
                    image = Vulkan::current_swapchain_image;
                    old_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                }
                else {
                    if (!vk_fbo->color_attachments[i])
                        continue;

                    Vulkan::Image& vk_image = gl_texture::get(vk_fbo->color_attachments[i])->image;
                    image = vk_image;
                    old_layout = vk_image.GetImageLayout(0, 0);
                }

                VkClearColorValue clear_color_value;
                clear_color_value.float32[0] = clear_color.x;
                clear_color_value.float32[1] = clear_color.y;
                clear_color_value.float32[2] = clear_color.z;
                clear_color_value.float32[3] = clear_color.w;

                VkImageSubresourceRange range;
                range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                range.baseMipLevel = 0;
                range.levelCount = 1;
                range.baseArrayLayer = 0;
                range.layerCount = 1;

                const VkImageLayout new_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

                Vulkan::Image::PipelineBarrierSingle(Vulkan::current_command_buffer, image,
                    VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, old_layout, new_layout);
                vkCmdClearColorImage(Vulkan::current_command_buffer, image,
                    new_layout, &clear_color_value, 1, &range);
                Vulkan::Image::PipelineBarrierSingle(Vulkan::current_command_buffer, image,
                    VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, new_layout, old_layout);

                if (!vk_fbo)
                    break;
            }
        }

        if (mask & GL_DEPTH_BUFFER_BIT) {
            gl_framebuffer* vk_fbo = gl_framebuffer::get(gl_state.draw_framebuffer_binding);
            if (vk_fbo && vk_fbo->depth_attachment) {
                gl_texture* vk_tex = gl_texture::get(vk_fbo->depth_attachment);
                VkImage image = vk_tex->image;
                const VkImageLayout old_layout = vk_tex->image.GetImageLayout(0, 0);

                const VkImageAspectFlags aspect_mask = Vulkan::get_aspect_mask(vk_tex->internal_format);

                VkClearDepthStencilValue clear_depth_stencil_value;
                clear_depth_stencil_value.depth = gl_wrap_manager_ptr->clear_depth;
                clear_depth_stencil_value.stencil = 0;

                VkImageSubresourceRange range;
                range.aspectMask = aspect_mask;
                range.baseMipLevel = 0;
                range.levelCount = 1;
                range.baseArrayLayer = 0;
                range.layerCount = 1;

                const VkImageLayout new_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

                Vulkan::Image::PipelineBarrierSingle(Vulkan::current_command_buffer, image,
                    aspect_mask, 0, 0, old_layout, new_layout);
                vkCmdClearDepthStencilImage(Vulkan::current_command_buffer, image,
                    new_layout, &clear_depth_stencil_value, 1, &range);
                Vulkan::Image::PipelineBarrierSingle(Vulkan::current_command_buffer, image,
                    aspect_mask, 0, 0, new_layout, old_layout);
            }
        }
    }

    static void gl_wrap_manager_clear_buffer(GLenum buffer, GLint drawbuffer, const GLfloat* value) {
        gl_wrap_manager_clear_named_framebuffer(gl_state.draw_framebuffer_binding, buffer, drawbuffer, value);
    }

    static void gl_wrap_manager_clear_color(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
        vec4& clear_color = gl_wrap_manager_ptr->clear_color;
        clear_color.x = red;
        clear_color.y = green;
        clear_color.z = blue;
        clear_color.w = alpha;
    }

    static void gl_wrap_manager_clear_depth(GLfloat depth) {
        gl_wrap_manager_ptr->clear_depth = depth;
    }

    static void gl_wrap_manager_clear_named_framebuffer(GLuint framebuffer,
        GLenum buffer, GLint drawbuffer, const GLfloat* value) {
        VkImageAspectFlags aspect_mask;
        switch (buffer) {
        case GL_COLOR:
            aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;
            if (drawbuffer < 0 || drawbuffer >= Vulkan::MAX_COLOR_ATTACHMENTS) {
                gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
                return;
            }
            break;
        case GL_DEPTH:
            aspect_mask = VK_IMAGE_ASPECT_DEPTH_BIT;
            if (drawbuffer) {
                gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
                return;
            }
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        gl_framebuffer* vk_fbo = gl_framebuffer::get(framebuffer);
        if (framebuffer && !vk_fbo) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        VkImage image;
        VkImageLayout old_layout;

        if (!vk_fbo) {
            if (buffer != GL_COLOR)
                return;

            image = Vulkan::current_swapchain_image;
            old_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        }
        else {
            if (buffer == GL_DEPTH) {
                GLuint texture = vk_fbo->depth_attachment;
                if (!texture)
                    return;

                Vulkan::Image& vk_image = gl_texture::get(texture)->image;
                image = vk_image;
                old_layout = vk_image.GetImageLayout(0, 0);
            }
            else {
                GLuint texture = vk_fbo->get_draw_buffer_texture(drawbuffer);
                if (!texture)
                    return;

                Vulkan::Image& vk_image = gl_texture::get(texture)->image;
                image = vk_image;
                old_layout = vk_image.GetImageLayout(0, 0);
            }
        }

        VkImageSubresourceRange range;
        range.aspectMask = aspect_mask;
        range.baseMipLevel = 0;
        range.levelCount = 1;
        range.baseArrayLayer = 0;
        range.layerCount = 1;

        const VkImageLayout new_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

        Vulkan::Image::PipelineBarrierSingle(Vulkan::current_command_buffer, image,
            aspect_mask, 0, 0, old_layout, new_layout);
        switch (buffer) {
        case GL_COLOR: {
            VkClearColorValue clear_color_value;
            clear_color_value.float32[0] = value[0];
            clear_color_value.float32[1] = value[1];
            clear_color_value.float32[2] = value[2];
            clear_color_value.float32[3] = value[3];
            vkCmdClearColorImage(Vulkan::current_command_buffer, image,
                new_layout, &clear_color_value, 1, &range);
        } break;
        case GL_DEPTH: {
            VkClearDepthStencilValue clear_depth_stencil_value;
            clear_depth_stencil_value.depth = value[0];
            clear_depth_stencil_value.stencil = 0;
            vkCmdClearDepthStencilImage(Vulkan::current_command_buffer, image,
                new_layout, &clear_depth_stencil_value, 1, &range);
        } break;
        }
        Vulkan::Image::PipelineBarrierSingle(Vulkan::current_command_buffer, image,
            aspect_mask, 0, 0, new_layout, old_layout);
    }

    static void gl_wrap_manager_compressed_tex_image_2d(GLenum target, GLint level,
        GLenum internal_format, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data) {
        uint32_t layer;
        GLuint texture;
        switch (target) {
        case GL_TEXTURE_2D:
            layer = 0;
            texture = gl_state.texture_binding_2d[gl_state.active_texture_index];
            break;
        case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
            layer = 0;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
            layer = 1;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
            layer = 2;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
            layer = 3;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
            layer = 4;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
            layer = 5;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        if (target == GL_TEXTURE_CUBE_MAP && width != height) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        switch (internal_format) {
        case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
        case GL_COMPRESSED_RED_RGTC1:
        case GL_COMPRESSED_RG_RGTC2:
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        if (texture_get_size(internal_format, width, height) != imageSize
            || width < 0 || width > sv_max_texture_size || height < 0 || height > sv_max_texture_size
            || level < 0 || level >= (int32_t)log2(sv_max_texture_size) || border) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        gl_texture* vk_tex = gl_texture::get(texture, false);
        if (!vk_tex || vk_tex->target != target || vk_tex->internal_format
            && vk_tex->internal_format != internal_format) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        if (vk_tex->image && (max_def(vk_tex->width >> level, 1) != width
            || max_def(vk_tex->height >> level, 1) != height)) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        gl_texture_data* vk_tex_data = gl_texture_data::get(texture);
        if (!vk_tex_data) {
            vk_tex_data = &gl_wrap_manager_ptr->gl_texture_datas.insert({ texture, {} }).first->second;
            vk_tex_data->target = target;
            vk_tex_data->internal_format = internal_format;
        }

        if (!level) {
            vk_tex_data->width = width;
            vk_tex_data->height = height;
        }
        else if (!vk_tex_data->valid() || vk_tex_data->internal_format != internal_format) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }
        else if (max_def(vk_tex_data->width >> level, 1) != width
            || max_def(vk_tex_data->height >> level, 1) != height) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        GLenum format;
        GLenum type;
        texture_get_format_type_by_internal_format(internal_format, &format, &type);
        gl_texture_data::tex_data* tex_data = vk_tex_data->get_tex_data(level, layer);
        if (tex_data)
            tex_data->set_data(internal_format, 0, 0, width, height, format, type, data);
    }

    static void gl_wrap_manager_compressed_tex_sub_image_2d(GLenum target, GLint level,
        GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data) {
        uint32_t layer;
        GLuint texture;
        switch (target) {
        case GL_TEXTURE_2D:
            layer = 0;
            texture = gl_state.texture_binding_2d[gl_state.active_texture_index];
            break;
        case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
            layer = 0;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
            layer = 1;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
            layer = 2;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
            layer = 3;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
            layer = 4;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
            layer = 5;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        if (target == GL_TEXTURE_CUBE_MAP && width != height) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        switch (format) {
        case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
        case GL_COMPRESSED_RED_RGTC1:
        case GL_COMPRESSED_RG_RGTC2:
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        if (texture_get_size(format, width, height) != imageSize
            || width < 0 || width > sv_max_texture_size || height < 0 || height > sv_max_texture_size
            || level < 0 || level >= (int32_t)log2(sv_max_texture_size)) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        gl_texture* vk_tex = gl_texture::get(texture, false);
        if (!vk_tex) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }
        else if (vk_tex->target != target) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        if (vk_tex->image && (max_def(vk_tex->width >> level, 1) != width
            || max_def(vk_tex->height >> level, 1) != height) || xoffset || yoffset) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        gl_texture_data* vk_tex_data = gl_texture_data::get(texture);
        if (!vk_tex_data) {
            if (!vk_tex->internal_format) {
                gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
                return;
            }

            vk_tex_data = &gl_wrap_manager_ptr->gl_texture_datas.insert({ texture, {} }).first->second;
            vk_tex_data->target = target;
            vk_tex_data->internal_format = vk_tex->internal_format;

            if (!level) {
                vk_tex_data->width = width;
                vk_tex_data->height = height;
            }
        }
        else if (!vk_tex_data->valid() || vk_tex_data->internal_format != format) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }
        else if (max_def(vk_tex_data->width >> level, 1) != width
            || max_def(vk_tex_data->height >> level, 1) != height) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        GLenum _format;
        GLenum _type;
        texture_get_format_type_by_internal_format(vk_tex_data->internal_format, &_format, &_type);
        gl_texture_data::tex_data* tex_data = vk_tex_data->get_tex_data(level, layer);
        if (tex_data)
            tex_data->set_data(vk_tex->internal_format,
                xoffset, yoffset, width, height, _format, _type, data);
    }

    static void gl_wrap_manager_copy_image_sub_data(
        GLuint src_name, GLenum src_target, GLint src_level, GLint src_x, GLint src_y, GLint src_z,
        GLuint dst_name, GLenum dst_target, GLint dst_level, GLint dst_x, GLint dst_y, GLint dst_z,
        GLsizei src_width, GLsizei src_height, GLsizei src_depth) {
        if (!src_name && !dst_name || src_target != GL_TEXTURE_2D || src_target != dst_target) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        gl_texture* vk_src_tex = gl_texture::get(src_name);
        gl_texture* vk_dst_tex = gl_texture::get(dst_name);

        if (!vk_src_tex || !vk_dst_tex) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }
        else if (vk_src_tex->internal_format != vk_dst_tex->internal_format) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }
        else if (src_level < 0 || (int64_t)vk_src_tex->get_level_count() <= src_level
            || dst_level < 0 || (int64_t)vk_dst_tex->get_level_count() <= dst_level
            || src_x < 0 || src_y < 0 || src_z < 0 || src_x + src_width > vk_src_tex->width
            || src_y + src_height > vk_src_tex->height || src_z + src_depth > 1
            || dst_x < 0 || dst_y < 0 || dst_z < 0 || dst_x + src_width > vk_dst_tex->width
            || dst_y + src_height > vk_dst_tex->height || dst_z + src_depth > 1) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        switch (vk_src_tex->internal_format) {
        case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
        case GL_COMPRESSED_RED_RGTC1:
        case GL_COMPRESSED_RG_RGTC2:
            if (src_x % 4 || src_y % 4 || src_z % 4 || dst_x % 4 || dst_y % 4 || dst_z % 4
                || src_width % 4 || src_height % 4 || src_depth % 4) {
                gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
                return;
            }
            break;
        }

        VkImage src_image = vk_src_tex->image;
        VkImageLayout src_old_layout = vk_src_tex->image.GetImageLayout(0, 0);

        VkImage dst_image = vk_dst_tex->image;
        VkImageLayout dst_old_layout = vk_dst_tex->image.GetImageLayout(0, 0);

        VkImageCopy image_copy_region;
        image_copy_region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_copy_region.srcSubresource.mipLevel = src_level;
        image_copy_region.srcSubresource.baseArrayLayer = 0;
        image_copy_region.srcSubresource.layerCount = 1;
        image_copy_region.srcOffset.x = src_x;
        image_copy_region.srcOffset.y = src_y;
        image_copy_region.srcOffset.z = 0;
        image_copy_region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_copy_region.dstSubresource.mipLevel = dst_level;
        image_copy_region.dstSubresource.baseArrayLayer = 0;
        image_copy_region.dstSubresource.layerCount = 1;
        image_copy_region.dstOffset.x = dst_x;
        image_copy_region.dstOffset.y = dst_y;
        image_copy_region.dstOffset.z = 0;
        image_copy_region.extent.width = src_width;
        image_copy_region.extent.height = src_height;
        image_copy_region.extent.depth = src_depth;

        const VkImageLayout src_new_layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        const VkImageLayout dst_new_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

        Vulkan::Image::PipelineBarrierSingle(Vulkan::current_command_buffer, src_image,
            VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, src_old_layout, src_new_layout);
        Vulkan::Image::PipelineBarrierSingle(Vulkan::current_command_buffer, dst_image,
            VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, dst_old_layout, dst_new_layout);
        vkCmdCopyImage(Vulkan::current_command_buffer, src_image,
            src_new_layout, dst_image, dst_new_layout, 1, &image_copy_region);
        Vulkan::Image::PipelineBarrierSingle(Vulkan::current_command_buffer, src_image,
            VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, src_new_layout, src_old_layout);
        Vulkan::Image::PipelineBarrierSingle(Vulkan::current_command_buffer, dst_image,
            VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, dst_new_layout, dst_old_layout);
    }

    static void gl_wrap_manager_copy_tex_sub_image_2d(GLenum target, GLint level,
        GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height) {
        uint32_t layer;
        GLuint texture;
        switch (target) {
        case GL_TEXTURE_2D:
            layer = 0;
            texture = gl_state.texture_binding_2d[gl_state.active_texture_index];
            break;
        case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
            layer = 0;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
            layer = 1;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
            layer = 2;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
            layer = 3;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
            layer = 4;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
            layer = 5;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        if (target == GL_TEXTURE_CUBE_MAP && width != height) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }
        else if (width < 0 || width > sv_max_texture_size || height < 0 || height > sv_max_texture_size
            || level < 0 || level >= (int32_t)log2(sv_max_texture_size)) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        gl_texture* vk_tex = gl_texture::get(texture);
        if (!vk_tex || !vk_tex->image) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }
        else if (xoffset < 0 || (xoffset + width) > vk_tex->width
            || yoffset < 0 || (yoffset + height) > vk_tex->height) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        gl_framebuffer* vk_fbo = gl_framebuffer::get(gl_state.read_framebuffer_binding, false);

        VkImage src_image;
        VkImageLayout src_old_layout;

        if (!vk_fbo) {
            src_image = Vulkan::current_swapchain_image;
            src_old_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        }
        else {
            GLuint texture = vk_fbo->get_read_buffer_texture();
            if (!texture) {
                gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
                return;
            }

            Vulkan::Image& vk_image = gl_texture::get(texture)->image;
            src_image = vk_image;
            src_old_layout = vk_image.GetImageLayout(0, 0);
        }

        VkImage dst_image = vk_tex->image;
        VkImageLayout dst_old_layout = vk_tex->image.GetImageLayout(0, 0);

        VkImageCopy image_copy_region;
        image_copy_region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_copy_region.srcSubresource.mipLevel = 0;
        image_copy_region.srcSubresource.baseArrayLayer = 0;
        image_copy_region.srcSubresource.layerCount = 1;
        image_copy_region.srcOffset.x = x;
        image_copy_region.srcOffset.y = y;
        image_copy_region.srcOffset.z = 0;
        image_copy_region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_copy_region.dstSubresource.mipLevel = 0;
        image_copy_region.dstSubresource.baseArrayLayer = 0;
        image_copy_region.dstSubresource.layerCount = 1;
        image_copy_region.dstOffset.x = xoffset;
        image_copy_region.dstOffset.y = yoffset;
        image_copy_region.dstOffset.z = 0;
        image_copy_region.extent.width = width;
        image_copy_region.extent.height = height;
        image_copy_region.extent.depth = 1;

        const VkImageLayout src_new_layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        const VkImageLayout dst_new_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

        Vulkan::Image::PipelineBarrierSingle(Vulkan::current_command_buffer, src_image,
            VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, src_old_layout, src_new_layout);
        Vulkan::Image::PipelineBarrierSingle(Vulkan::current_command_buffer, dst_image,
            VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, dst_old_layout, dst_new_layout);
        vkCmdCopyImage(Vulkan::current_command_buffer, src_image,
            src_new_layout, dst_image, dst_new_layout, 1, &image_copy_region);
        Vulkan::Image::PipelineBarrierSingle(Vulkan::current_command_buffer, src_image,
            VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, src_new_layout, src_old_layout);
        Vulkan::Image::PipelineBarrierSingle(Vulkan::current_command_buffer, dst_image,
            VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, dst_new_layout, dst_old_layout);
    }

    static void gl_wrap_manager_create_buffers(GLsizei n, GLuint* buffers) {
        if (n < 0) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        for (GLsizei i = 0; i < n; i++)
            buffers[i] = gl_wrap_manager_ptr->create_buffer();
    }

    static GLuint gl_wrap_manager_create_program() {
        return gl_wrap_manager_ptr->create_program();
    }

    static void gl_wrap_manager_delete_buffers(GLsizei n, const GLuint* buffers) {
        if (n < 0) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        for (GLsizei i = 0; i < n; i++)
            gl_wrap_manager_ptr->release_buffer(buffers[i]);
    }

    static void gl_wrap_manager_delete_framebuffers(GLsizei n, const GLuint* framebuffers) {
        if (n < 0) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        for (GLsizei i = 0; i < n; i++)
            gl_wrap_manager_ptr->release_framebuffer(framebuffers[i]);
    }

    static void gl_wrap_manager_delete_program(GLuint program) {
        gl_wrap_manager_ptr->release_program(program);
    }

    static void gl_wrap_manager_delete_queries(GLsizei n, const GLuint* ids) {
        if (n < 0) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        for (GLsizei i = 0; i < n; i++)
            gl_wrap_manager_ptr->release_query(ids[i]);
    }

    static void gl_wrap_manager_delete_samplers(GLsizei n, const GLuint* samplers) {
        if (n < 0) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        for (GLsizei i = 0; i < n; i++)
            gl_wrap_manager_ptr->release_sampler(samplers[i]);
    }

    static void gl_wrap_manager_delete_textures(GLsizei n, const GLuint* textures) {
        if (n < 0) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        for (GLsizei i = 0; i < n; i++)
            gl_wrap_manager_ptr->release_texture(textures[i]);
    }

    static void gl_wrap_manager_delete_vertex_arrays(GLsizei n, const GLuint* arrays) {
        if (n < 0) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        for (GLsizei i = 0; i < n; i++)
            gl_wrap_manager_ptr->release_vertex_array(arrays[i]);
    }

    static void gl_wrap_manager_disable_vertex_attrib_array(GLuint index) {
        if (index >= Vulkan::MAX_VERTEX_ATTRIB_COUNT) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        gl_vertex_array* vk_vao = gl_vertex_array::get(gl_state.vertex_array_binding);
        if (!vk_vao) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        vk_vao->reset_vertex_attrib(index);

        vk_vao->vertex_attribs[index].enabled = false;
    }

    static void gl_wrap_manager_draw_buffer(GLenum buf) {
        gl_wrap_manager_draw_buffers(1, &buf);
    }

    static void gl_wrap_manager_draw_buffers(GLsizei n, const GLenum* bufs) {
        if (n < 0 || n >= Vulkan::MAX_DRAW_BUFFERS) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        gl_framebuffer* vk_fbo = gl_framebuffer::get(gl_state.framebuffer_binding, false);
        if (!vk_fbo) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        bool color_attachments[Vulkan::MAX_COLOR_ATTACHMENTS] = {};

        for (GLsizei i = 0; i < n && i < Vulkan::MAX_DRAW_BUFFERS; i++) {
            GLenum buf = bufs[i];
            if (!buf)
                continue;

            if (buf < GL_COLOR_ATTACHMENT0
                || buf >= GL_COLOR_ATTACHMENT0 + Vulkan::MAX_COLOR_ATTACHMENTS
                || color_attachments[buf - GL_COLOR_ATTACHMENT0]) {
                gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
                return;
            }

            color_attachments[buf - GL_COLOR_ATTACHMENT0] = true;
        }

        for (GLsizei i = 0; i < n && i < Vulkan::MAX_DRAW_BUFFERS; i++)
            if (vk_fbo->draw_buffers[i] != bufs[i]) {
                vk_fbo->draw_buffers[i] = bufs[i];
                vk_fbo->update_framebuffer = true;
            }
    }

    static void gl_wrap_manager_enable_vertex_attrib_array(GLuint index) {
        if (index >= Vulkan::MAX_VERTEX_ATTRIB_COUNT) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        gl_vertex_array* vk_vao = gl_vertex_array::get(gl_state.vertex_array_binding);
        if (!vk_vao) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        vk_vao->vertex_attribs[index].enabled = true;
    }

    static void gl_wrap_manager_end_query(GLenum target) {
        if (target != GL_SAMPLES_PASSED) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        if (!gl_wrap_manager_ptr->query_samples_passed) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        gl_query* vk_que = gl_query::get(gl_wrap_manager_ptr->query_samples_passed);
        if (!vk_que)
            return;

        vk_que->query.End(Vulkan::current_command_buffer);
        gl_wrap_manager_ptr->query_samples_passed = 0;
    }

    static void gl_wrap_manager_finish() {

    }

    static void gl_wrap_manager_framebuffer_texture(GLenum target,
        GLenum attachment, GLuint texture, GLint level) {
        if (target != GL_FRAMEBUFFER || attachment < GL_COLOR_ATTACHMENT0
            || (attachment > GL_COLOR_ATTACHMENT0 + Vulkan::MAX_COLOR_ATTACHMENTS
                && attachment != GL_DEPTH_ATTACHMENT)) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        gl_framebuffer* vk_fbo = gl_framebuffer::get(gl_state.framebuffer_binding, false);
        if (!vk_fbo) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }
        else if (level < 0) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        gl_texture* vk_tex = gl_texture::get(texture, false);
        gl_texture_data* vk_tex_data = gl_texture_data::get(texture);
        if (texture && (!vk_tex || level >= (int64_t)vk_tex->level_count && (!vk_tex_data
            || !vk_tex_data->valid() || level >= (int64_t)vk_tex_data->level_count))) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        if (attachment == GL_DEPTH_ATTACHMENT) {
            vk_fbo->depth_attachment = texture;
            vk_fbo->depth_attachment_level = texture ? level : 0;
            vk_fbo->update_depth_attachment = true;
        }
        else {
            uint32_t index = attachment - GL_COLOR_ATTACHMENT0;
            vk_fbo->color_attachments[index] = texture;
            vk_fbo->color_attachment_levels[index] = texture ? level : 0;
            vk_fbo->update_color_attachment = true;
        }
    }

    static void gl_wrap_manager_gen_buffers(GLsizei n, GLuint* buffers) {
        if (n < 0) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        for (GLsizei i = 0; i < n; i++)
            buffers[i] = gl_wrap_manager_ptr->create_buffer();
    }

    static void gl_wrap_manager_gen_framebuffers(GLsizei n, GLuint* framebuffers) {
        if (n < 0) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        for (GLsizei i = 0; i < n; i++)
            framebuffers[i] = gl_wrap_manager_ptr->create_framebuffer();
    }

    static void gl_wrap_manager_gen_queries(GLsizei n, GLuint* ids) {
        if (n < 0) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        for (GLsizei i = 0; i < n; i++)
            ids[i] = gl_wrap_manager_ptr->create_query();
    }

    static void gl_wrap_manager_gen_samplers(GLsizei n, GLuint* samplers) {
        if (n < 0) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        for (GLsizei i = 0; i < n; i++)
            samplers[i] = gl_wrap_manager_ptr->create_sampler();
    }

    static void gl_wrap_manager_gen_textures(GLsizei n, GLuint* textures) {
        if (n < 0) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        for (GLsizei i = 0; i < n; i++)
            textures[i] = gl_wrap_manager_ptr->create_texture();
    }

    static void gl_wrap_manager_gen_vertex_arrays(GLsizei n, GLuint* arrays) {
        if (n < 0) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        for (GLsizei i = 0; i < n; i++)
            arrays[i] = gl_wrap_manager_ptr->create_vertex_array();
    }

    static void gl_wrap_manager_generate_mipmap(GLenum target) {
        switch (target) {
        case GL_TEXTURE_2D:
            gl_wrap_manager_generate_texture_mipmap(
                gl_state.texture_binding_2d[gl_state.active_texture_index]);
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            break;
        }
    }

    static void gl_wrap_manager_generate_texture_mipmap(GLuint texture) {
        gl_texture* vk_tex = gl_texture::get(texture);
        if (!vk_tex) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }
        else if (vk_tex->target != GL_TEXTURE_2D
            || Vulkan::get_aspect_mask(vk_tex->internal_format) != VK_IMAGE_ASPECT_COLOR_BIT) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        const uint32_t level_count = vk_tex->get_level_count();

        Vulkan::Image& vk_image = vk_tex->image;
        VkImage image = vk_image;
        VkImageLayout old_layout = vk_image.GetImageLayout(0, 0);

        const VkImageLayout src_new_layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        const VkImageLayout dst_new_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

        for (uint32_t i = 1; i < level_count; i++) {
            VkImageBlit image_blit_region;
            image_blit_region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            image_blit_region.srcSubresource.mipLevel = i - 1;
            image_blit_region.srcSubresource.baseArrayLayer = 0;
            image_blit_region.srcSubresource.layerCount = 1;
            image_blit_region.srcOffsets[0].x = 0;
            image_blit_region.srcOffsets[0].y = 0;
            image_blit_region.srcOffsets[0].z = 0;
            image_blit_region.srcOffsets[1].x = (int32_t)(vk_tex->width >> (i - 1));
            image_blit_region.srcOffsets[1].y = (int32_t)(vk_tex->height >> (i - 1));
            image_blit_region.srcOffsets[1].z = 1;
            image_blit_region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            image_blit_region.dstSubresource.mipLevel = i;
            image_blit_region.dstSubresource.baseArrayLayer = 0;
            image_blit_region.dstSubresource.layerCount = 1;
            image_blit_region.dstOffsets[0].x = 0;
            image_blit_region.dstOffsets[0].y = 0;
            image_blit_region.dstOffsets[0].z = 0;
            image_blit_region.dstOffsets[1].x = (int32_t)(vk_tex->width >> i);
            image_blit_region.dstOffsets[1].y = (int32_t)(vk_tex->height >> i);
            image_blit_region.dstOffsets[1].z = 1;

            Vulkan::Image::PipelineBarrierSingle(Vulkan::current_command_buffer, vk_image,
                VK_IMAGE_ASPECT_COLOR_BIT, i - 1, 0, src_new_layout);
            Vulkan::Image::PipelineBarrierSingle(Vulkan::current_command_buffer, vk_image,
                VK_IMAGE_ASPECT_COLOR_BIT, i, 0, dst_new_layout);
            vkCmdBlitImage(Vulkan::current_command_buffer, image, src_new_layout,
                image, dst_new_layout, 1, &image_blit_region, VK_FILTER_LINEAR);
        }
        Vulkan::Image::PipelineBarrierSingle(Vulkan::current_command_buffer, vk_image,
            VK_IMAGE_ASPECT_COLOR_BIT, level_count - 1, 0, src_new_layout);
    }

    static void gl_wrap_manager_get_compressed_tex_image(GLenum target, GLint level, void* img) {
        uint32_t layer;
        GLuint texture;
        switch (target) {
        case GL_TEXTURE_2D:
            layer = 0;
            texture = gl_state.texture_binding_2d[gl_state.active_texture_index];
            break;
        case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
            layer = 0;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
            layer = 1;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
            layer = 2;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
            layer = 3;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
            layer = 4;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
            layer = 5;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        gl_texture* vk_tex = gl_texture::get(texture, false);
        if (!vk_tex || vk_tex->target != target) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        GLenum internal_format = GL_NONE;
        GLsizei width = 0;
        GLsizei height = 0;
        uint32_t level_count = 0;

        gl_texture_data* vk_tex_data = gl_texture_data::get(texture);
        if (vk_tex_data && vk_tex_data->valid()) {
            gl_texture_data::tex_data* tex_data = vk_tex_data->get_tex_data(level, layer, true);
            if (tex_data && tex_data->data.size()) {
                internal_format = vk_tex_data->internal_format;
                width = vk_tex_data->width;
                height = vk_tex_data->height;
                level_count = vk_tex_data->level_count;
            }
        }

        if (!internal_format) {
            internal_format = vk_tex->internal_format;
            width = vk_tex->width;
            height = vk_tex->height;
            level_count = vk_tex->level_count;
        }

        if (!internal_format) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }
        else if (level < 0 || level >= (int64_t)level_count) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        width = max_def(width >> level, 1);
        height = max_def(height >> level, 1);
        const int32_t size = texture_get_size(internal_format, width, height);

        if (vk_tex_data && vk_tex_data->valid()) {
            gl_texture_data::tex_data* tex_data = vk_tex_data->get_tex_data(level, layer, true);
            if (tex_data && tex_data->data.size()) {
                memmove(img, tex_data->data.data(), size);
                return;
            }
        }

        Vulkan::Buffer staging_buffer = Vulkan::manager_get_staging_buffer(size);

        const VkImageViewType image_view_type = Vulkan::get_image_view_type(vk_tex->target);
        const VkFormat format = Vulkan::get_format(vk_tex->internal_format);
        const VkImageAspectFlags aspect_mask = Vulkan::get_aspect_mask(vk_tex->internal_format);

        Vulkan::CommandBuffer cb(Vulkan::current_device,
            Vulkan::current_command_pool, Vulkan::current_command_buffer);

        const VkImageLayout old_layout = vk_tex->image.GetImageLayout(level, layer);
        const VkImageLayout new_layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

        Vulkan::Image::PipelineBarrierSingle(Vulkan::current_command_buffer,
            vk_tex->image, aspect_mask, level, layer, new_layout);
        cb.CopyImageToBuffer(vk_tex->image, staging_buffer, staging_buffer.GetOffset(),
            aspect_mask, level, layer, 0, 0, width, height);
        Vulkan::Image::PipelineBarrierSingle(Vulkan::current_command_buffer,
            vk_tex->image, aspect_mask, level, layer, old_layout);

        staging_buffer.ReadMemory(staging_buffer.GetOffset(), size, img);
    }

    static GLenum gl_wrap_manager_get_error() {
        return gl_wrap_manager_ptr->get_error();
    }

    static void gl_wrap_manager_get_float(GLenum pname, GLfloat* data) {
        switch (pname) {
        case GL_COLOR_CLEAR_VALUE: {
            const vec4& clear_color = gl_wrap_manager_ptr->clear_color;
            data[0] = clear_color.x;
            data[1] = clear_color.y;
            data[2] = clear_color.z;
            data[3] = clear_color.w;
        } break;
        case GL_DEPTH_CLEAR_VALUE:
            *data = gl_wrap_manager_ptr->clear_depth;
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            break;
        }
    }

    static void gl_wrap_manager_get_query_object(GLuint id, GLenum pname, GLint* params) {
        if (pname != GL_QUERY_RESULT_AVAILABLE) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        gl_query* vk_que = gl_query::get(id);
        if (!vk_que)
            return;

        uint32_t result[2] = {};
        vk_que->query.GetResult(sizeof(uint32_t) * 2, result, VK_QUERY_RESULT_WITH_AVAILABILITY_BIT);
        *params = result[0] ? GL_TRUE : GL_FALSE;
    }

    static void gl_wrap_manager_get_query_object(GLuint id, GLenum pname, GLuint* params) {
        if (pname != GL_QUERY_RESULT) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        gl_query* vk_que = gl_query::get(id);
        if (!vk_que)
            return;

        uint32_t result = 0;
        vk_que->query.GetResult(sizeof(uint32_t), &result, 0);
        *params = result;
    }

    static void gl_wrap_manager_get_tex_image(GLenum target,
        GLint level, GLenum format, GLenum type, void* pixels) {
        uint32_t layer;
        GLuint texture;
        switch (target) {
        case GL_TEXTURE_2D:
            layer = 0;
            texture = gl_state.texture_binding_2d[gl_state.active_texture_index];
            break;
        case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
            layer = 0;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
            layer = 1;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
            layer = 2;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
            layer = 3;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
            layer = 4;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
            layer = 5;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        switch (format) {
        case GL_RED:
        case GL_RG:
        case GL_RGB:
        case GL_RGBA:
        case GL_DEPTH_COMPONENT:
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        switch (type) {
        case GL_UNSIGNED_BYTE:
        case GL_HALF_FLOAT:
        case GL_FLOAT:
        case GL_UNSIGNED_SHORT_5_6_5:
        case GL_UNSIGNED_SHORT_5_6_5_REV:
        case GL_UNSIGNED_SHORT_4_4_4_4_REV:
        case GL_UNSIGNED_SHORT_1_5_5_5_REV:
        case GL_UNSIGNED_INT_10F_11F_11F_REV:
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        gl_texture* vk_tex = gl_texture::get(texture, false);
        if (!vk_tex || vk_tex->target != target) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        GLenum internal_format = GL_NONE;
        GLsizei width = 0;
        GLsizei height = 0;
        uint32_t level_count = 0;

        gl_texture_data* vk_tex_data = gl_texture_data::get(texture);
        if (vk_tex_data && vk_tex_data->valid()) {
            gl_texture_data::tex_data* tex_data = vk_tex_data->get_tex_data(level, layer, true);
            if (tex_data && tex_data->data.size()) {
                internal_format = vk_tex_data->internal_format;
                width = vk_tex_data->width;
                height = vk_tex_data->height;
                level_count = vk_tex_data->level_count;
            }
        }

        if (!internal_format) {
            internal_format = vk_tex->internal_format;
            width = vk_tex->width;
            height = vk_tex->height;
            level_count = vk_tex->level_count;
        }

        if (!internal_format) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }
        else if (level < 0 || level >= (int64_t)level_count) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        GLenum _format;
        GLenum _type;
        texture_get_format_type_by_internal_format(internal_format, &_format, &_type);
        if (!_format || !_type || _format != format && _type != type
            && _type != GL_FLOAT && _type != GL_HALF_FLOAT && type != GL_FLOAT && type != GL_HALF_FLOAT) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        if ((type == GL_UNSIGNED_SHORT_5_6_5 || type == GL_UNSIGNED_SHORT_5_6_5_REV) && format != GL_RGB
            || (type == GL_UNSIGNED_SHORT_4_4_4_4_REV || type == GL_UNSIGNED_SHORT_1_5_5_5_REV
                || type == GL_UNSIGNED_INT_5_9_9_9_REV) && format != GL_RGBA) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        width = max_def(width >> level, 1);
        height = max_def(height >> level, 1);
        const int32_t size = texture_get_size(internal_format, width, height);

        void* temp = 0;

        if (vk_tex_data && vk_tex_data->valid()) {
            gl_texture_data::tex_data* tex_data = vk_tex_data->get_tex_data(level, layer, true);
            if (tex_data && tex_data->data.size()) {
                temp = force_malloc(size);
                memmove(temp, tex_data->data.data(), size);
            }
        }

        if (!temp) {
            Vulkan::Buffer staging_buffer = Vulkan::manager_get_staging_buffer(size);

            const VkImageViewType image_view_type = Vulkan::get_image_view_type(vk_tex->target);
            const VkFormat format = Vulkan::get_format(vk_tex->internal_format);
            const VkImageAspectFlags aspect_mask = Vulkan::get_aspect_mask(vk_tex->internal_format);

            Vulkan::CommandBuffer cb(Vulkan::current_device,
                Vulkan::current_command_pool, Vulkan::current_command_buffer);

            const VkImageLayout old_layout = vk_tex->image.GetImageLayout(level, layer);
            const VkImageLayout new_layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

            Vulkan::Image::PipelineBarrierSingle(Vulkan::current_command_buffer,
                vk_tex->image, aspect_mask, level, layer, new_layout);
            cb.CopyImageToBuffer(vk_tex->image, staging_buffer, staging_buffer.GetOffset(),
                aspect_mask, level, layer, 0, 0, width, height);
            Vulkan::Image::PipelineBarrierSingle(Vulkan::current_command_buffer,
                vk_tex->image, aspect_mask, level, layer, old_layout);

            temp = force_malloc(size);
            staging_buffer.ReadMemory(staging_buffer.GetOffset(), size, temp);
        }


        if (!gl_texture_data::tex_data::convert(internal_format, width, height, format,
            _type, type, temp, pixels, gl_wrap_manager_ptr->pack_alignment, false)) {
            free_def(temp);
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }
        free_def(temp);
    }

    static void* gl_wrap_manager_map_buffer(GLenum target, GLenum access) {
        switch (target) {
        case GL_ARRAY_BUFFER:
            return gl_wrap_manager_map_named_buffer(gl_state.array_buffer_binding, access);
        case GL_ELEMENT_ARRAY_BUFFER:
            return gl_wrap_manager_map_named_buffer(gl_state.element_array_buffer_binding, access);
        case GL_UNIFORM_BUFFER:
            return gl_wrap_manager_map_named_buffer(gl_state.uniform_buffer_binding, access);
        case GL_SHADER_STORAGE_BUFFER:
            return gl_wrap_manager_map_named_buffer(gl_state.shader_storage_buffer_binding, access);
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return 0;
        };
    }

    static void* gl_wrap_manager_map_named_buffer(GLuint buffer, GLenum access) {
        switch (access) {
        case GL_READ_ONLY:
        case GL_WRITE_ONLY:
        case GL_READ_WRITE:
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return 0;
        }

        gl_buffer* vk_buf = gl_buffer::get(buffer);
        if (!vk_buf || (vk_buf->flags & Vulkan::GL_BUFFER_FLAG_MAPPED)
            || access == GL_READ_ONLY && !(vk_buf->flags & Vulkan::GL_BUFFER_FLAG_MAP_READ_BIT)
            || access == GL_WRITE_ONLY && !(vk_buf->flags & Vulkan::GL_BUFFER_FLAG_MAP_WRITE_BIT)
            || access == GL_READ_WRITE && (!(vk_buf->flags & Vulkan::GL_BUFFER_FLAG_MAP_READ_BIT)
                || !(vk_buf->flags & Vulkan::GL_BUFFER_FLAG_MAP_WRITE_BIT))) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return 0;
        }
        else if (!vk_buf->data.size()) {
            gl_wrap_manager_ptr->push_error(GL_OUT_OF_MEMORY);
            return 0;
        }

        enum_or(vk_buf->flags, Vulkan::GL_BUFFER_FLAG_MAPPED);
        return vk_buf->data.data();
    }

    static void gl_wrap_manager_named_buffer_data(
        GLuint buffer, GLsizeiptr size, const void* data, GLenum usage) {
        switch (usage) {
        case GL_STREAM_DRAW:
        case GL_STREAM_READ:
        case GL_STREAM_COPY:
        case GL_STATIC_DRAW:
        case GL_STATIC_READ:
        case GL_STATIC_COPY:
        case GL_DYNAMIC_DRAW:
        case GL_DYNAMIC_READ:
        case GL_DYNAMIC_COPY:
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        if (size < 0) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        gl_buffer* vk_buf = gl_buffer::get(buffer);
        if (!vk_buf || (vk_buf->flags & Vulkan::GL_BUFFER_FLAG_IMMUTABLE_STORAGE)) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        enum_or(vk_buf->flags, Vulkan::GL_BUFFER_FLAG_DYNAMIC_STORAGE_BIT
            | Vulkan::GL_BUFFER_FLAG_MAP_WRITE_BIT
            | Vulkan::GL_BUFFER_FLAG_MAP_READ_BIT);

        if (data)
            vk_buf->data.assign((const uint8_t*)data, (const uint8_t*)data + size);
        else
            vk_buf->data.assign(size, 0);
        enum_or(vk_buf->flags, Vulkan::GL_BUFFER_FLAG_UPDATE_DATA);
    }

    static void gl_wrap_manager_named_buffer_storage(
        GLuint buffer, GLsizeiptr size, const void* data, GLbitfield flags) {
        if (flags & ~(GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_READ_BIT)) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }
        else if (size < 0) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        gl_buffer* vk_buf = gl_buffer::get(buffer);
        if (!vk_buf || (vk_buf->flags & Vulkan::GL_BUFFER_FLAG_IMMUTABLE_STORAGE)) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        enum_or(vk_buf->flags, Vulkan::GL_BUFFER_FLAG_IMMUTABLE_STORAGE);
        if (flags & GL_MAP_READ_BIT)
            enum_or(vk_buf->flags, Vulkan::GL_BUFFER_FLAG_MAP_READ_BIT);
        if (flags & GL_MAP_WRITE_BIT)
            enum_or(vk_buf->flags, Vulkan::GL_BUFFER_FLAG_MAP_WRITE_BIT);
        if (flags & GL_DYNAMIC_STORAGE_BIT)
            enum_or(vk_buf->flags, Vulkan::GL_BUFFER_FLAG_DYNAMIC_STORAGE_BIT);

        if (data)
            vk_buf->data.assign((const uint8_t*)data, (const uint8_t*)data + size);
        else
            vk_buf->data.assign(size, 0);
        enum_or(vk_buf->flags, Vulkan::GL_BUFFER_FLAG_UPDATE_DATA);
    }

    static void gl_wrap_manager_named_buffer_sub_data(
        GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data) {
        gl_buffer* vk_buf = gl_buffer::get(buffer);
        if (!vk_buf || (vk_buf->flags & Vulkan::GL_BUFFER_FLAG_MAPPED)
            || (vk_buf->flags & Vulkan::GL_BUFFER_FLAG_IMMUTABLE_STORAGE)
            && !(vk_buf->flags & Vulkan::GL_BUFFER_FLAG_DYNAMIC_STORAGE_BIT)) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }
        else if (offset < 0 || size < 0 || offset + size > (ssize_t)vk_buf->data.size()) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        memmove(vk_buf->data.data() + offset, data, size);
        enum_or(vk_buf->flags, Vulkan::GL_BUFFER_FLAG_UPDATE_DATA);
    }

    static void gl_wrap_manager_pixel_store(GLenum pname, GLint param) {
        switch (pname) {
        case GL_PACK_ALIGNMENT:
            if (param != 1 && param != 2 && param != 4 && param != 8) {
                gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
                return;
            }

            gl_wrap_manager_ptr->pack_alignment = param;
            break;
        case GL_UNPACK_ALIGNMENT:
            if (param != 1 && param != 2 && param != 4 && param != 8) {
                gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
                return;
            }

            gl_wrap_manager_ptr->unpack_alignment = param;
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            break;
        }
    }

    static void gl_wrap_manager_pop_debug_group() {
        vkCmdEndDebugUtilsLabelEXT(Vulkan::current_command_buffer);
    }

    static void gl_wrap_manager_push_debug_group(GLenum source, GLuint id, GLsizei length, const GLchar* message) {
        VkDebugUtilsLabelEXT debug_utils_label;
        debug_utils_label.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
        debug_utils_label.pNext = 0;
        debug_utils_label.pLabelName = message;
        debug_utils_label.color[0] = 0.0f;
        debug_utils_label.color[1] = 0.0f;
        debug_utils_label.color[2] = 0.0f;
        debug_utils_label.color[3] = 0.0f;
        vkCmdBeginDebugUtilsLabelEXT(Vulkan::current_command_buffer, &debug_utils_label);
    }

    static void gl_wrap_manager_read_buffer(GLenum src) {
        if (src && src < GL_COLOR_ATTACHMENT0
            || src >= GL_COLOR_ATTACHMENT0 + Vulkan::MAX_COLOR_ATTACHMENTS) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        gl_framebuffer* vk_fbo = gl_framebuffer::get(gl_state.framebuffer_binding, false);
        if (!vk_fbo) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        vk_fbo->read_buffer = src;
    }

    static void gl_wrap_manager_sampler_parameter(GLuint sampler, GLenum pname, GLfloat param) {
        gl_sampler* vk_samp = gl_sampler::get(sampler);
        if (!vk_samp) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        switch (pname) {
        case GL_TEXTURE_MAX_ANISOTROPY_EXT:
            vk_samp->max_anisotropy = param;
            break;
        }
    }

    static void gl_wrap_manager_sampler_parameter(GLuint sampler, GLenum pname, GLint param) {
        gl_sampler* vk_samp = gl_sampler::get(sampler);
        if (!vk_samp) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        switch (pname) {
        case GL_TEXTURE_MAG_FILTER:
            vk_samp->mag_filter = param;
            break;
        case GL_TEXTURE_MIN_FILTER:
            vk_samp->min_filter = param;
            break;
        case GL_TEXTURE_WRAP_S:
            vk_samp->wrap_s = param;
            break;
        case GL_TEXTURE_WRAP_T:
            vk_samp->wrap_t = param;
            break;
        case GL_TEXTURE_WRAP_R:
            vk_samp->wrap_r = param;
            break;
        }
    }

    static void gl_wrap_manager_sampler_parameter(GLuint sampler, GLenum pname, const GLfloat* params) {
        gl_sampler* vk_samp = gl_sampler::get(sampler);
        if (!vk_samp) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        switch (pname) {
        case GL_TEXTURE_BORDER_COLOR: {
            vec4& border_color = vk_samp->border_color;
            border_color.x = params[0];
            border_color.y = params[1];
            border_color.z = params[2];
            border_color.w = params[3];
        } break;
        }
    }

    static void gl_wrap_manager_sampler_parameter(GLuint sampler, GLenum pname, const GLint* params) {
        gl_sampler* vk_samp = gl_sampler::get(sampler);
        if (!vk_samp) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }
    }

    static void gl_wrap_manager_tex_image_2d(GLenum target, GLint level, GLint internal_format,
        GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels) {
        uint32_t layer;
        GLuint texture;
        switch (target) {
        case GL_TEXTURE_2D:
            layer = 0;
            texture = gl_state.texture_binding_2d[gl_state.active_texture_index];
            break;
        case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
            layer = 0;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
            layer = 1;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
            layer = 2;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
            layer = 3;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
            layer = 4;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
            layer = 5;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        if (target == GL_TEXTURE_CUBE_MAP && width != height) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        switch (format) {
        case GL_RED:
        case GL_RG:
        case GL_RGB:
        case GL_RGBA:
        case GL_DEPTH_COMPONENT:
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        switch (type) {
        case GL_UNSIGNED_BYTE:
        case GL_HALF_FLOAT:
        case GL_FLOAT:
        case GL_UNSIGNED_SHORT_5_6_5:
        case GL_UNSIGNED_SHORT_5_6_5_REV:
        case GL_UNSIGNED_SHORT_4_4_4_4_REV:
        case GL_UNSIGNED_SHORT_1_5_5_5_REV:
        case GL_UNSIGNED_INT_10F_11F_11F_REV:
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        switch (internal_format) {
        case GL_RGB5:
        case GL_RGBA4:
        case GL_RGB5_A1:
        case GL_DEPTH_COMPONENT16:
        case GL_DEPTH_COMPONENT24:
        case GL_R8:
        case GL_RG8:
        case GL_RGB8:
        case GL_RGBA8:
        case GL_R32F:
        case GL_RG16F:
        case GL_RG32F:
        case GL_RGBA32F:
        case GL_RGBA16F:
        case GL_DEPTH24_STENCIL8:
        case GL_R11F_G11F_B10F:
        case GL_RGB9_E5:
        case GL_DEPTH_COMPONENT32F:
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        if ((type == GL_UNSIGNED_SHORT_5_6_5 || type == GL_UNSIGNED_SHORT_5_6_5_REV) && format != GL_RGB
            || (type == GL_UNSIGNED_SHORT_4_4_4_4_REV || type == GL_UNSIGNED_SHORT_1_5_5_5_REV
                || type == GL_UNSIGNED_INT_5_9_9_9_REV) && format != GL_RGBA
            || ((target != GL_TEXTURE_2D || format != GL_DEPTH_COMPONENT || pixels)
                && internal_format == GL_DEPTH_COMPONENT16 && internal_format != GL_DEPTH_COMPONENT24
                && internal_format == GL_DEPTH24_STENCIL8 && internal_format== GL_DEPTH_COMPONENT32F)) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        GLenum _format;
        GLenum _type;
        texture_get_format_type_by_internal_format(internal_format, &_format, &_type);
        if (!_format || !_type || _format != format && _type != type
            && _type != GL_FLOAT && _type != GL_HALF_FLOAT && type != GL_FLOAT && type != GL_HALF_FLOAT) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        if (width < 0 || width > sv_max_texture_size || height < 0 || height > sv_max_texture_size
            || level < 0 || level >= (int32_t)log2(sv_max_texture_size) || border) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        gl_texture* vk_tex = gl_texture::get(texture, false);
        if (!vk_tex || vk_tex->target != target || vk_tex->internal_format
            && vk_tex->internal_format != internal_format) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        if (vk_tex->image && (max_def(vk_tex->width >> level, 1) != width
            || max_def(vk_tex->height >> level, 1) != height)) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        gl_texture_data* vk_tex_data = gl_texture_data::get(texture);
        if (!vk_tex_data) {
            vk_tex_data = &gl_wrap_manager_ptr->gl_texture_datas.insert({ texture, {} }).first->second;
            vk_tex_data->target = target;
            vk_tex_data->internal_format = internal_format;
        }

        if (!level) {
            vk_tex_data->width = width;
            vk_tex_data->height = height;
        }
        else if (vk_tex_data->valid() && (max_def(vk_tex_data->width >> level, 1) != width
            || max_def(vk_tex_data->height >> level, 1) != height)) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        gl_texture_data::tex_data* tex_data = vk_tex_data->get_tex_data(level, layer);
        if (tex_data)
            tex_data->set_data(internal_format, 0, 0, width, height, format, type, pixels);
    }

    static void gl_wrap_manager_tex_parameter(GLenum target, GLenum pname, GLfloat param) {
        switch (target) {
        case GL_TEXTURE_2D:
            gl_wrap_manager_texture_parameter(
                gl_state.texture_binding_2d[gl_state.active_texture_index], pname, param);
            break;
        case GL_TEXTURE_CUBE_MAP:
            gl_wrap_manager_texture_parameter(
                gl_state.texture_binding_cube_map[gl_state.active_texture_index], pname, param);
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            break;
        }
    }

    static void gl_wrap_manager_tex_parameter(GLenum target, GLenum pname, GLint param) {
        switch (target) {
        case GL_TEXTURE_2D:
            gl_wrap_manager_texture_parameter(
                gl_state.texture_binding_2d[gl_state.active_texture_index], pname, param);
            break;
        case GL_TEXTURE_CUBE_MAP:
            gl_wrap_manager_texture_parameter(
                gl_state.texture_binding_cube_map[gl_state.active_texture_index], pname, param);
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            break;
        }
    }

    static void gl_wrap_manager_tex_parameter(GLenum target, GLenum pname, const GLfloat* params) {
        switch (target) {
        case GL_TEXTURE_2D:
            gl_wrap_manager_texture_parameter(
                gl_state.texture_binding_2d[gl_state.active_texture_index], pname, params);
            break;
        case GL_TEXTURE_CUBE_MAP:
            gl_wrap_manager_texture_parameter(
                gl_state.texture_binding_cube_map[gl_state.active_texture_index], pname, params);
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            break;
        }
    }

    static void gl_wrap_manager_tex_parameter(GLenum target, GLenum pname, const GLint* params) {
        switch (target) {
        case GL_TEXTURE_2D:
            gl_wrap_manager_texture_parameter(
                gl_state.texture_binding_2d[gl_state.active_texture_index], pname, params);
            break;
        case GL_TEXTURE_CUBE_MAP:
            gl_wrap_manager_texture_parameter(
                gl_state.texture_binding_cube_map[gl_state.active_texture_index], pname, params);
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            break;
        }
    }

    static void gl_wrap_manager_tex_sub_image_2d(GLenum target,
        GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
        GLenum format, GLenum type, const void* pixels) {
        uint32_t layer;
        GLuint texture;
        switch (target) {
        case GL_TEXTURE_2D:
            layer = 0;
            texture = gl_state.texture_binding_2d[gl_state.active_texture_index];
            break;
        case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
            layer = 0;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
            layer = 1;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
            layer = 2;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
            layer = 3;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
            layer = 4;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
            layer = 5;
            texture = gl_state.texture_binding_cube_map[gl_state.active_texture_index];
            target = GL_TEXTURE_CUBE_MAP;
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        if (target == GL_TEXTURE_CUBE_MAP && width != height) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        switch (format) {
        case GL_RED:
        case GL_RG:
        case GL_RGB:
        case GL_RGBA:
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        switch (type) {
        case GL_UNSIGNED_BYTE:
        case GL_HALF_FLOAT:
        case GL_FLOAT:
        case GL_UNSIGNED_SHORT_5_6_5:
        case GL_UNSIGNED_SHORT_5_6_5_REV:
        case GL_UNSIGNED_SHORT_4_4_4_4_REV:
        case GL_UNSIGNED_SHORT_1_5_5_5_REV:
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        if ((type == GL_UNSIGNED_SHORT_5_6_5 || type == GL_UNSIGNED_SHORT_5_6_5_REV) && format != GL_RGB
            || (type == GL_UNSIGNED_SHORT_4_4_4_4_REV || type == GL_UNSIGNED_SHORT_1_5_5_5_REV
                || type == GL_UNSIGNED_INT_5_9_9_9_REV) && format != GL_RGBA) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        if (xoffset || yoffset || width < 0 || width > sv_max_texture_size
            || height < 0 || height > sv_max_texture_size
            || level < 0 || level >= (int32_t)log2(sv_max_texture_size)) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        gl_texture* vk_tex = gl_texture::get(texture, false);
        if (!vk_tex) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }
        else if (vk_tex->target != target) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        if (vk_tex->image && (max_def(vk_tex->width >> level, 1) != width
            || max_def(vk_tex->height >> level, 1) != height
            || level < 0 || level >= (int64_t)vk_tex->get_level_count())) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        gl_texture_data* vk_tex_data = gl_texture_data::get(texture);
        if (!vk_tex_data) {
            if (!vk_tex->internal_format) {
                gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
                return;
            }

            vk_tex_data = &gl_wrap_manager_ptr->gl_texture_datas.insert({ texture, {} }).first->second;
            vk_tex_data->target = target;
            vk_tex_data->internal_format = vk_tex->internal_format;

            if (!level) {
                vk_tex_data->width = width;
                vk_tex_data->height = height;
            }
        }
        else if (vk_tex_data->valid() && (max_def(vk_tex_data->width >> level, 1) != width
            || max_def(vk_tex_data->height >> level, 1) != height)) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        GLenum _format;
        GLenum _type;
        texture_get_format_type_by_internal_format(vk_tex_data->internal_format, &_format, &_type);
        if (!_format || !_type || _format != format && _type != type
            && _type != GL_FLOAT && _type != GL_HALF_FLOAT && type != GL_FLOAT && type != GL_HALF_FLOAT) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        gl_texture_data::tex_data* tex_data = vk_tex_data->get_tex_data(level, layer);
        if (tex_data)
            tex_data->set_data(vk_tex->internal_format,
                xoffset, yoffset, width, height, format, type, pixels);
    }

    static void gl_wrap_manager_texture_parameter(GLuint texture, GLenum pname, GLfloat param) {
        gl_texture* vk_tex = gl_texture::get(texture, false);
        if (!vk_tex) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }
        else if (vk_tex->target != GL_TEXTURE_2D && vk_tex->target != GL_TEXTURE_CUBE_MAP) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        switch (pname) {
        case GL_TEXTURE_LOD_BIAS:
            break;
        case GL_TEXTURE_MAX_ANISOTROPY_EXT:
            vk_tex->sampler_data.max_anisotropy = param;
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            break;
        }
    }

    static void gl_wrap_manager_texture_parameter(GLuint texture, GLenum pname, const GLint param) {
        gl_texture* vk_tex = gl_texture::get(texture, false);
        if (!vk_tex) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }
        else if (vk_tex->target != GL_TEXTURE_2D && vk_tex->target != GL_TEXTURE_CUBE_MAP) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        switch (pname) {
        case GL_TEXTURE_MAG_FILTER:
            vk_tex->sampler_data.mag_filter = param;
            break;
        case GL_TEXTURE_MIN_FILTER:
            vk_tex->sampler_data.min_filter = param;
            break;
        case GL_TEXTURE_WRAP_S:
            vk_tex->sampler_data.wrap_s = param;
            break;
        case GL_TEXTURE_WRAP_T:
            vk_tex->sampler_data.wrap_t = param;
            break;
        case GL_TEXTURE_WRAP_R:
            vk_tex->sampler_data.wrap_r = param;
            break;
        case GL_TEXTURE_BASE_LEVEL:
            if (vk_tex->base_mipmap_level != param) {
                vk_tex->sample_image_view.Destroy();
                vk_tex->base_mipmap_level = param;
            }
            break;
        case GL_TEXTURE_MAX_LEVEL:
            if (vk_tex->max_mipmap_level != param) {
                vk_tex->sample_image_view.Destroy();
                vk_tex->max_mipmap_level = param;
            }
            break;
        case GL_TEXTURE_SWIZZLE_R: {
            VkComponentMapping components = vk_tex->components;
            components.r = Vulkan::get_component_swizzle(param, GL_RED);

            if (memcmp(&vk_tex->components, &components, sizeof(VkComponentMapping))) {
                vk_tex->sample_image_view.Destroy();
                vk_tex->components = components;
            }
        } break;
        case GL_TEXTURE_SWIZZLE_G: {
            VkComponentMapping components = vk_tex->components;
            components.g = Vulkan::get_component_swizzle(param, GL_GREEN);

            if (memcmp(&vk_tex->components, &components, sizeof(VkComponentMapping))) {
                vk_tex->sample_image_view.Destroy();
                vk_tex->components = components;
            }
        } break;
        case GL_TEXTURE_SWIZZLE_B: {
            VkComponentMapping components = vk_tex->components;
            components.b = Vulkan::get_component_swizzle(param, GL_BLUE);

            if (memcmp(&vk_tex->components, &components, sizeof(VkComponentMapping))) {
                vk_tex->sample_image_view.Destroy();
                vk_tex->components = components;
            }
        } break;
        case GL_TEXTURE_SWIZZLE_A: {
            VkComponentMapping components = vk_tex->components;
            components.a = Vulkan::get_component_swizzle(param, GL_ALPHA);

            if (memcmp(&vk_tex->components, &components, sizeof(VkComponentMapping))) {
                vk_tex->sample_image_view.Destroy();
                vk_tex->components = components;
            }
        } break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            break;
        }
    }

    static void gl_wrap_manager_texture_parameter(GLuint texture, GLenum pname, const GLfloat* params) {
        gl_texture* vk_tex = gl_texture::get(texture, false);
        if (!vk_tex) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }
        else if (vk_tex->target != GL_TEXTURE_2D && vk_tex->target != GL_TEXTURE_CUBE_MAP) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        switch (pname) {
        case GL_TEXTURE_BORDER_COLOR: {
            vec4& border_color = vk_tex->sampler_data.border_color;
            border_color.x = params[0];
            border_color.y = params[1];
            border_color.z = params[2];
            border_color.w = params[3];
        } break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            break;
        }
    }

    static void gl_wrap_manager_texture_parameter(GLuint texture, GLenum pname, const GLint* params) {
        gl_texture* vk_tex = gl_texture::get(texture, false);
        if (!vk_tex) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }
        else if (vk_tex->target != GL_TEXTURE_2D && vk_tex->target != GL_TEXTURE_CUBE_MAP) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        switch (pname) {
        case GL_TEXTURE_SWIZZLE_RGBA: {
            VkComponentMapping components;
            components.r = Vulkan::get_component_swizzle(params[0], GL_RED);
            components.g = Vulkan::get_component_swizzle(params[1], GL_GREEN);
            components.b = Vulkan::get_component_swizzle(params[2], GL_BLUE);
            components.a = Vulkan::get_component_swizzle(params[3], GL_ALPHA);

            if (memcmp(&vk_tex->components, &components, sizeof(VkComponentMapping))) {
                vk_tex->sample_image_view.Destroy();
                vk_tex->components = components;
            }
        } break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            break;
        }
    }

    static void gl_wrap_manager_texture_sub_image_2d(GLuint texture,
        GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
        GLenum format, GLenum type, const void* pixels) {
        switch (format) {
        case GL_RED:
        case GL_RG:
        case GL_RGB:
        case GL_RGBA:
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        switch (type) {
        case GL_UNSIGNED_BYTE:
        case GL_HALF_FLOAT:
        case GL_FLOAT:
        case GL_UNSIGNED_SHORT_5_6_5:
        case GL_UNSIGNED_SHORT_5_6_5_REV:
        case GL_UNSIGNED_SHORT_4_4_4_4_REV:
        case GL_UNSIGNED_SHORT_1_5_5_5_REV:
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        if ((type == GL_UNSIGNED_SHORT_5_6_5 || type == GL_UNSIGNED_SHORT_5_6_5_REV) && format != GL_RGB
            || (type == GL_UNSIGNED_SHORT_4_4_4_4_REV || type == GL_UNSIGNED_SHORT_1_5_5_5_REV
                || type == GL_UNSIGNED_INT_5_9_9_9_REV) && format != GL_RGBA) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        if (width < 0 || width > sv_max_texture_size || height < 0 || height > sv_max_texture_size
            || level < 0 || level >= (int32_t)log2(sv_max_texture_size)) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        gl_texture* vk_tex = gl_texture::get(texture, false);
        if (!vk_tex) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        if (vk_tex->image && (max_def(vk_tex->width >> level, 1) != width
            || max_def(vk_tex->height >> level, 1) != height)
            || level < 0 || level >= (int64_t)vk_tex->get_level_count() || xoffset || yoffset) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        gl_texture_data* vk_tex_data = gl_texture_data::get(texture);
        if (!vk_tex_data) {
            if (!vk_tex->internal_format) {
                gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
                return;
            }

            vk_tex_data = &gl_wrap_manager_ptr->gl_texture_datas.insert({ texture, {} }).first->second;
            vk_tex_data->target = vk_tex->target;
            vk_tex_data->internal_format = vk_tex->internal_format;

            if (!level) {
                vk_tex_data->width = width;
                vk_tex_data->height = height;
            }
        }
        else if (vk_tex_data->valid() && (max_def(vk_tex_data->width >> level, 1) != width
            || max_def(vk_tex_data->height >> level, 1) != height)) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        GLenum _format;
        GLenum _type;
        texture_get_format_type_by_internal_format(vk_tex_data->internal_format, &_format, &_type);
        if (!_format || !_type || _format != format && _type != type
            && _type != GL_FLOAT && _type != GL_HALF_FLOAT && type != GL_FLOAT && type != GL_HALF_FLOAT) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        gl_texture_data::tex_data* tex_data = vk_tex_data->get_tex_data(level, 0);
        if (tex_data)
            tex_data->set_data(vk_tex_data->internal_format,
                xoffset, yoffset, width, height, format, type, pixels);
    }

    static GLboolean gl_wrap_manager_unmap_buffer(GLenum target) {
        switch (target) {
        case GL_ARRAY_BUFFER:
            return gl_wrap_manager_unmap_named_buffer(gl_state.array_buffer_binding);
        case GL_ELEMENT_ARRAY_BUFFER:
            return gl_wrap_manager_unmap_named_buffer(gl_state.element_array_buffer_binding);
        case GL_UNIFORM_BUFFER:
            return gl_wrap_manager_unmap_named_buffer(gl_state.uniform_buffer_binding);
        case GL_SHADER_STORAGE_BUFFER:
            return gl_wrap_manager_unmap_named_buffer(gl_state.shader_storage_buffer_binding);
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return GL_FALSE;
        };
    }

    static GLboolean gl_wrap_manager_unmap_named_buffer(GLuint buffer) {
        gl_buffer* vk_buf = gl_buffer::get(buffer);
        if (!vk_buf || !(vk_buf->flags & Vulkan::GL_BUFFER_FLAG_MAPPED)) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return GL_FALSE;
        }

        enum_and(vk_buf->flags, ~Vulkan::GL_BUFFER_FLAG_MAPPED);
        enum_or(vk_buf->flags, Vulkan::GL_BUFFER_FLAG_UPDATE_DATA);
        return GL_TRUE;
    }

    static void gl_wrap_manager_vertex_attrib(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
        if (index >= Vulkan::MAX_VERTEX_ATTRIB_COUNT)
            return;

        gl_vertex_array* vk_vao = gl_vertex_array::get(gl_state.vertex_array_binding);
        if (!vk_vao)
            return;

        vec4& generic_value = vk_vao->vertex_attribs[index].generic_value;
        generic_value.x = x;
        generic_value.y = y;
        generic_value.z = z;
        generic_value.w = w;
    }

    static void gl_wrap_manager_vertex_attrib_int_pointer(GLuint index, GLint size,
        GLenum type, GLsizei stride, const void* pointer) {
        if (index >= Vulkan::MAX_VERTEX_ATTRIB_COUNT) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        VkFormat format = VK_FORMAT_UNDEFINED;
        switch (type) {
        case GL_BYTE:
            switch (size) {
            case 1:
                format = VK_FORMAT_R8_SINT;
                break;
            case 2:
                format = VK_FORMAT_R8G8_SINT;
                break;
            case 3:
                format = VK_FORMAT_R8G8B8_SINT;
                break;
            case 4:
                format = VK_FORMAT_R8G8B8A8_SINT;
            }
            break;
        case GL_UNSIGNED_BYTE:
            switch (size) {
            case 1:
                format = VK_FORMAT_R8_UINT;
                break;
            case 2:
                format = VK_FORMAT_R8G8_UINT;
                break;
            case 3:
                format = VK_FORMAT_R8G8B8_UINT;
                break;
            case 4:
                format = VK_FORMAT_R8G8B8A8_UINT;
            }
            break;
        case GL_SHORT:
            switch (size) {
            case 1:
                format = VK_FORMAT_R16_SINT;
                break;
            case 2:
                format = VK_FORMAT_R16G16_SINT;
                break;
            case 3:
                format = VK_FORMAT_R16G16B16_SINT;
                break;
            case 4:
                format = VK_FORMAT_R16G16B16A16_SINT;
                break;
            }
            break;
        case GL_UNSIGNED_SHORT:
            switch (size) {
            case 1:
                format = VK_FORMAT_R16_UINT;
                break;
            case 2:
                format = VK_FORMAT_R16G16_UINT;
                break;
            case 3:
                format = VK_FORMAT_R16G16B16_UINT;
                break;
            case 4:
                format = VK_FORMAT_R16G16B16A16_UINT;
                break;
            }
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        if (format == VK_FORMAT_UNDEFINED) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        gl_vertex_array* vk_vao = gl_vertex_array::get(gl_state.vertex_array_binding);
        if (!vk_vao || !gl_state.array_buffer_binding) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        vk_vao->set_vertex_attrib(index, format, stride, (uint32_t)(size_t)pointer);
    }

    static void gl_wrap_manager_vertex_attrib_pointer(GLuint index, GLint size,
        GLenum type, GLboolean normalized, GLsizei stride, const void* pointer) {
        if (index >= Vulkan::MAX_VERTEX_ATTRIB_COUNT || size < 1 || size > 4 || stride < 0) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_VALUE);
            return;
        }

        VkFormat format = VK_FORMAT_UNDEFINED;
        switch (type) {
        case GL_BYTE:
            if (normalized)
                switch (size) {
                case 1:
                    format = VK_FORMAT_R8_SNORM;
                    break;
                case 2:
                    format = VK_FORMAT_R8G8_SNORM;
                    break;
                case 3:
                    format = VK_FORMAT_R8G8B8_SNORM;
                    break;
                case 4:
                    format = VK_FORMAT_R8G8B8A8_SNORM;
                    break;
                }
            else
                switch (size) {
                case 1:
                    format = VK_FORMAT_R8_SINT;
                    break;
                case 2:
                    format = VK_FORMAT_R8G8_SINT;
                    break;
                case 3:
                    format = VK_FORMAT_R8G8B8_SINT;
                    break;
                case 4:
                    format = VK_FORMAT_R8G8B8A8_SINT;
                }
            break;
        case GL_UNSIGNED_BYTE:
            if (normalized)
                switch (size) {
                case 1:
                    format = VK_FORMAT_R8_UNORM;
                    break;
                case 2:
                    format = VK_FORMAT_R8G8_UNORM;
                    break;
                case 3:
                    format = VK_FORMAT_R8G8B8_UNORM;
                    break;
                case 4:
                    format = VK_FORMAT_R8G8B8A8_UNORM;
                    break;
                }
            else
                switch (size) {
                case 1:
                    format = VK_FORMAT_R8_UINT;
                    break;
                case 2:
                    format = VK_FORMAT_R8G8_UINT;
                    break;
                case 3:
                    format = VK_FORMAT_R8G8B8_UINT;
                    break;
                case 4:
                    format = VK_FORMAT_R8G8B8A8_UINT;
                }
            break;
        case GL_SHORT:
            if (normalized)
                switch (size) {
                case 1:
                    format = VK_FORMAT_R16_SNORM;
                    break;
                case 2:
                    format = VK_FORMAT_R16G16_SNORM;
                    break;
                case 3:
                    format = VK_FORMAT_R16G16B16_SNORM;
                    break;
                case 4:
                    format = VK_FORMAT_R16G16B16A16_SNORM;
                    break;
                }
            else
                switch (size) {
                case 1:
                    format = VK_FORMAT_R16_SINT;
                    break;
                case 2:
                    format = VK_FORMAT_R16G16_SINT;
                    break;
                case 3:
                    format = VK_FORMAT_R16G16B16_SINT;
                    break;
                case 4:
                    format = VK_FORMAT_R16G16B16A16_SINT;
                    break;
                }
            break;
        case GL_UNSIGNED_SHORT:
            if (normalized)
                switch (size) {
                case 1:
                    format = VK_FORMAT_R16_UNORM;
                    break;
                case 2:
                    format = VK_FORMAT_R16G16_UNORM;
                    break;
                case 3:
                    format = VK_FORMAT_R16G16B16_UNORM;
                    break;
                case 4:
                    format = VK_FORMAT_R16G16B16A16_UNORM;
                    break;
                }
            else
                switch (size) {
                case 1:
                    format = VK_FORMAT_R16_UINT;
                    break;
                case 2:
                    format = VK_FORMAT_R16G16_UINT;
                    break;
                case 3:
                    format = VK_FORMAT_R16G16B16_UINT;
                    break;
                case 4:
                    format = VK_FORMAT_R16G16B16A16_UINT;
                    break;
                }
            break;
        case GL_FLOAT:
            switch (size) {
            case 1:
                format = VK_FORMAT_R32_SFLOAT;
                break;
            case 2:
                format = VK_FORMAT_R32G32_SFLOAT;
                break;
            case 3:
                format = VK_FORMAT_R32G32B32_SFLOAT;
                break;
            case 4:
                format = VK_FORMAT_R32G32B32A32_SFLOAT;
                break;
            }
            break;
        case GL_HALF_FLOAT:
            switch (size) {
            case 1:
                format = VK_FORMAT_R16_SFLOAT;
                break;
            case 2:
                format = VK_FORMAT_R16G16_SFLOAT;
                break;
            case 3:
                format = VK_FORMAT_R16G16B16_SFLOAT;
                break;
            case 4:
                format = VK_FORMAT_R16G16B16A16_SFLOAT;
                break;
            }
            break;
        case GL_UNSIGNED_INT_2_10_10_10_REV:
            if (size == 4)
                format = VK_FORMAT_A2B10G10R10_UNORM_PACK32;
            break;
        case GL_INT_2_10_10_10_REV:
            if (size == 4)
                format = VK_FORMAT_A2B10G10R10_SNORM_PACK32;
            break;
        default:
            gl_wrap_manager_ptr->push_error(GL_INVALID_ENUM);
            return;
        }

        if (format == VK_FORMAT_UNDEFINED) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        gl_vertex_array* vk_vao = gl_vertex_array::get(gl_state.vertex_array_binding);
        if (!vk_vao || !gl_state.array_buffer_binding) {
            gl_wrap_manager_ptr->push_error(GL_INVALID_OPERATION);
            return;
        }

        vk_vao->set_vertex_attrib(index, format, stride, (uint32_t)(size_t)pointer);
    }
};
