/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "manager.hpp"
#include "../../KKdLib/prj/algorithm.hpp"
#include "../../KKdLib/prj/shared_ptr.hpp"
#include "../../KKdLib/hash.hpp"
#include "../static_var.hpp"
#include "DescriptorPipeline.hpp"
#include "DynamicBuffer.hpp"
#include "Pipeline.hpp"
#include "RenderPass.hpp"
#include "Sampler.hpp"
#include "StagingBuffer.hpp"
#include <unordered_map>
#include <vector>

struct descriptor_pipeline_data {
    uint32_t sampler_count;
    uint32_t uniform_count;
    uint32_t storage_count;
    uint8_t pad[4];
    uint64_t bindings_hash;
    uint64_t push_constant_ranges_hash;

    inline descriptor_pipeline_data() : sampler_count(), uniform_count(),
        storage_count(), pad(), bindings_hash(), push_constant_ranges_hash() {

    }

    inline descriptor_pipeline_data(uint32_t sampler_count, uint32_t uniform_count,
        uint32_t storage_count, uint64_t bindings_hash, uint64_t push_constant_ranges_hash) : pad() {
        this->sampler_count = sampler_count;
        this->uniform_count = uniform_count;
        this->storage_count = storage_count;
        this->bindings_hash = bindings_hash;
        this->push_constant_ranges_hash = push_constant_ranges_hash;
    }
};

template <>
class std::hash<descriptor_pipeline_data> {
public:
    size_t operator()(const descriptor_pipeline_data& format) const {
        return (size_t)hash_xxh3_64bits(&format, sizeof(format));
    }
};

constexpr bool operator==(const descriptor_pipeline_data& left, const descriptor_pipeline_data& right) {
    return left.sampler_count == right.sampler_count && left.uniform_count == right.uniform_count
        && left.storage_count == right.storage_count && left.bindings_hash == right.bindings_hash
        && left.push_constant_ranges_hash == right.push_constant_ranges_hash;
}

struct pipeline_data {
    uint64_t stages_hash;
    uint64_t vertex_input_info_binding_hash;
    uint64_t vertex_input_info_attribute_hash;
    uint64_t input_assembly_state_hash;
    uint64_t rasterization_state_hash;
    uint64_t depth_stencil_state_hash;
    uint64_t color_blend_attachmen_hash;
    bool line_width;
    bool stencil;
    uint8_t pad[6];
    VkPipelineLayout layout;
    VkRenderPass render_pass;

    inline pipeline_data() : stages_hash(), vertex_input_info_binding_hash(),
        vertex_input_info_attribute_hash(), input_assembly_state_hash(),
        rasterization_state_hash(), depth_stencil_state_hash(), color_blend_attachmen_hash(),
        line_width(), stencil(), pad(), layout(), render_pass() {

    }

    inline pipeline_data(uint64_t stages_hash,
        uint64_t vertex_input_info_binding_hash, uint64_t vertex_input_info_attribute_hash,
        uint64_t input_assembly_state_hash, uint64_t rasterization_state_hash,
        uint64_t depth_stencil_state_hash, uint64_t color_blend_attachmen_hash,
        bool line_width, bool stencil, VkPipelineLayout layout, VkRenderPass render_pass) : pad() {
        this->stages_hash = stages_hash;
        this->vertex_input_info_binding_hash = vertex_input_info_binding_hash;
        this->vertex_input_info_attribute_hash = vertex_input_info_attribute_hash;
        this->input_assembly_state_hash = input_assembly_state_hash;
        this->rasterization_state_hash = rasterization_state_hash;
        this->depth_stencil_state_hash = depth_stencil_state_hash;
        this->color_blend_attachmen_hash = color_blend_attachmen_hash;
        this->line_width = line_width;
        this->stencil = stencil;
        this->layout = layout;
        this->render_pass = render_pass;
    }
};

template <>
class std::hash<pipeline_data> {
public:
    size_t operator()(const pipeline_data& format) const {
        return (size_t)hash_xxh3_64bits(&format, sizeof(format));
    }
};

constexpr bool operator==(const pipeline_data& left, const pipeline_data& right) {
    return left.stages_hash == right.stages_hash
        && left.vertex_input_info_binding_hash == right.vertex_input_info_binding_hash
        && left.vertex_input_info_attribute_hash == right.vertex_input_info_attribute_hash
        && left.input_assembly_state_hash == right.input_assembly_state_hash
        && left.rasterization_state_hash == right.rasterization_state_hash
        && left.depth_stencil_state_hash == right.depth_stencil_state_hash
        && left.color_blend_attachmen_hash == right.color_blend_attachmen_hash
        && left.layout == right.layout && left.render_pass == right.render_pass;
}

struct render_pass_data {
    uint64_t color_formats_hash;
    GLenum depth_format;
    bool depth_read_only;
    uint8_t pad[3];

    inline render_pass_data() : color_formats_hash(), depth_format(), depth_read_only(), pad() {

    }

    inline render_pass_data(uint64_t color_formats_hash, GLenum depth_format, bool depth_read_only) : pad() {
        this->color_formats_hash = color_formats_hash;
        this->depth_format = depth_format;
        this->depth_read_only = depth_read_only;
    }
};

template <>
class std::hash<render_pass_data> {
public:
    size_t operator()(const render_pass_data& format) const {
        return (size_t)hash_xxh3_64bits(&format, sizeof(format));
    }
};

constexpr bool operator==(const render_pass_data& left, const render_pass_data& right) {
    return left.color_formats_hash == right.color_formats_hash
        && left.depth_format == right.depth_format;
}

namespace Vulkan {
    struct manager {
        struct free_framebuffer_data {
            VkDevice device;
            VkFramebuffer framebuffer;
            const VkAllocationCallbacks* allocator;

            inline free_framebuffer_data() : device(), framebuffer(), allocator() {

            }

            inline free_framebuffer_data(VkDevice device,
                VkFramebuffer framebuffer, const VkAllocationCallbacks* allocator) {
                this->device = device;
                this->framebuffer = framebuffer;
                this->allocator = allocator;
            }
        };

        struct free_image_view_data {
            VkDevice device;
            VkImageView image_view;
            const VkAllocationCallbacks* allocator;

            inline free_image_view_data() : device(), image_view(), allocator() {

            }

            inline free_image_view_data(VkDevice device,
                VkImageView image_view, const VkAllocationCallbacks* allocator) {
                this->device = device;
                this->image_view = image_view;
                this->allocator = allocator;
            }
        };

        struct frame_data {
            std::vector<free_framebuffer_data> free_framebuffers;
            std::vector<free_image_view_data> free_image_views;
            std::vector<Vulkan::DynamicBuffer> dynamic_buffers;
            std::vector<Vulkan::StagingBuffer> staging_buffers;

            frame_data();
            ~frame_data();

            void ctrl();
        };

        std::vector<frame_data> frames_data;
        uint32_t frame;

        std::unordered_map<descriptor_pipeline_data,
            prj::shared_ptr<Vulkan::DescriptorPipeline>> descriptor_pipelines;
        std::unordered_map<pipeline_data, prj::shared_ptr<Vulkan::Pipeline>> pipelines;
        std::unordered_map<render_pass_data, prj::shared_ptr<Vulkan::RenderPass>> render_passes;
        std::unordered_map<uint64_t, prj::shared_ptr<Vulkan::Sampler>> samplers;

        manager(uint32_t max_frames);
        ~manager();

        void free_framebuffer(VkDevice device,
            VkFramebuffer framebuffer, const VkAllocationCallbacks* allocator);
        void free_image_view(VkDevice device,
            VkImageView image_view, const VkAllocationCallbacks* allocator);
        prj::shared_ptr<Vulkan::DescriptorPipeline> get_descriptor_pipeline(
            uint32_t sampler_count, uint32_t uniform_count, uint32_t storage_count,
            const VkDescriptorSetLayoutBinding* bindings,
            uint32_t push_constant_range_count, VkPushConstantRange* push_constant_ranges);
        frame_data& get_frame_data();
        prj::shared_ptr<Vulkan::Pipeline> get_pipeline(uint32_t stage_count,
            const VkPipelineShaderStageCreateInfo* stages,
            uint32_t vertex_input_binding_description_count,
            const VkVertexInputBindingDescription* vertex_input_binding_descriptions,
            uint32_t vertex_input_attribute_description_count,
            const VkVertexInputAttributeDescription* vertex_input_attribute_descriptions,
            const VkPipelineInputAssemblyStateCreateInfo* input_assembly_state,
            const VkPipelineRasterizationStateCreateInfo* rasterization_state,
            const VkPipelineDepthStencilStateCreateInfo* depth_stencil_state,
            uint32_t color_blend_attachment_count,
            const VkPipelineColorBlendAttachmentState* color_blend_attachments,
            bool line_width, bool stencil, VkPipelineLayout layout, VkRenderPass render_pass);
        prj::shared_ptr<Vulkan::RenderPass> get_render_pass(GLenum* color_formats,
            uint32_t color_format_count, GLenum depth_format, bool depth_read_only);
        prj::shared_ptr<Vulkan::Sampler> get_sampler(const gl_sampler& sampler_data);
        Vulkan::Buffer get_dynamic_buffer(VkDeviceSize size, VkDeviceSize alignment);
        Vulkan::Buffer get_staging_buffer(VkDeviceSize size, VkDeviceSize alignment);
        void next_frame(uint32_t frame);
        void reset_descriptor_pipelines_descriptor_set_collections();
    };

    manager* manager_ptr;

    void manager_init(uint32_t max_frames) {
        manager_ptr = new manager(max_frames);
    }

    void manager_free_descriptor_pipelines() {
        manager_ptr->descriptor_pipelines.clear();
    }

    void manager_free_framebuffer(VkDevice device,
        VkFramebuffer framebuffer, const VkAllocationCallbacks* allocator) {
        manager_ptr->free_framebuffer(device, framebuffer, allocator);
    }

    void manager_free_image_view(VkDevice device,
        VkImageView image_view, const VkAllocationCallbacks* allocator) {
        manager_ptr->free_image_view(device, image_view, allocator);
    }

    void manager_free_pipelines() {
        manager_ptr->pipelines.clear();
    }

    void manager_free_render_passes() {
        manager_ptr->render_passes.clear();
    }

    void manager_free_samplers() {
        manager_ptr->samplers.clear();
    }

    prj::shared_ptr<Vulkan::DescriptorPipeline> manager_get_descriptor_pipeline(
        uint32_t sampler_count, uint32_t uniform_count, uint32_t storage_count,
        const VkDescriptorSetLayoutBinding* bindings,
        uint32_t push_constant_range_count, VkPushConstantRange* push_constant_ranges) {
        return manager_ptr->get_descriptor_pipeline(
            sampler_count, uniform_count, storage_count, bindings,
            push_constant_range_count, push_constant_ranges);
    }

    uint32_t manager_get_frame() {
        return manager_ptr->frame;
    }

    prj::shared_ptr<Vulkan::Pipeline> manager_get_pipeline(uint32_t stage_count,
        const VkPipelineShaderStageCreateInfo* stages,
        uint32_t vertex_input_binding_description_count,
        const VkVertexInputBindingDescription* vertex_input_binding_descriptions,
        uint32_t vertex_input_attribute_description_count,
        const VkVertexInputAttributeDescription* vertex_input_attribute_descriptions,
        const VkPipelineInputAssemblyStateCreateInfo* input_assembly_state,
        const VkPipelineRasterizationStateCreateInfo* rasterization_state,
        const VkPipelineDepthStencilStateCreateInfo* depth_stencil_state,
        uint32_t color_blend_attachment_count,
        const VkPipelineColorBlendAttachmentState* color_blend_attachments,
        bool line_width, bool stencil, VkPipelineLayout layout, VkRenderPass render_pass) {
        return manager_ptr->get_pipeline(stage_count, stages,
            vertex_input_binding_description_count, vertex_input_binding_descriptions,
            vertex_input_attribute_description_count, vertex_input_attribute_descriptions,
            input_assembly_state, rasterization_state, depth_stencil_state,
            color_blend_attachment_count, color_blend_attachments,
            line_width, stencil, layout, render_pass);
    }

    prj::shared_ptr<Vulkan::RenderPass> manager_get_render_pass(
        GLenum* color_formats, uint32_t color_format_count,
        GLenum depth_format, bool depth_read_only) {
        return manager_ptr->get_render_pass(color_formats,
            color_format_count, depth_format, depth_read_only);
    }

    prj::shared_ptr<Vulkan::Sampler> manager_get_sampler(const gl_sampler& sampler_data) {
        return manager_ptr->get_sampler(sampler_data);
    }

    Vulkan::Buffer manager_get_dynamic_buffer(VkDeviceSize size, VkDeviceSize alignment) {
        return manager_ptr->get_dynamic_buffer(size, alignment);
    }

    Vulkan::Buffer manager_get_staging_buffer(VkDeviceSize size, VkDeviceSize alignment) {
        return manager_ptr->get_staging_buffer(size, alignment);
    }

    void manager_next_frame(uint32_t frame) {
        manager_ptr->next_frame(frame);
    }

    void manager_reset_descriptor_pipelines_descriptor_set_collections() {
        manager_ptr->reset_descriptor_pipelines_descriptor_set_collections();
    }

    void manager_free() {
        delete manager_ptr;
    }

    manager::frame_data::frame_data() {

    }

    manager::frame_data::~frame_data() {
        ctrl();

        for (Vulkan::StagingBuffer& i : staging_buffers)
            i.Destroy();
        for (Vulkan::DynamicBuffer& i : dynamic_buffers)
            i.Destroy();
    }

    void manager::frame_data::ctrl() {
        for (free_framebuffer_data& i : free_framebuffers)
            vkDestroyFramebuffer(i.device, i.framebuffer, i.allocator);
        free_framebuffers.clear();

        for (free_image_view_data& i : free_image_views)
            vkDestroyImageView(i.device, i.image_view, i.allocator);
        free_image_views.clear();

        for (Vulkan::DynamicBuffer& i : dynamic_buffers) {
            if (!i.curr_size)
                i.empty_frame_count++;
        }

        auto i_dyn_begin = dynamic_buffers.begin();
        auto i_dyn_end = dynamic_buffers.end();
        for (auto i = i_dyn_end; i != i_dyn_begin; ) {
            i--;

            if (!i->curr_size) {
                i->empty_frame_count++;

                if (i->empty_frame_count >= 60) {
                    i->Destroy();
                    i = dynamic_buffers.erase(i);
                    i_dyn_begin = dynamic_buffers.begin();
                    continue;
                }
            }
            else
                i->empty_frame_count = 0;

            i->curr_size = 0;
        }

        auto i_stg_begin = staging_buffers.begin();
        auto i_stg_end = staging_buffers.end();
        for (auto i = i_stg_end; i != i_stg_begin; ) {
            i--;

            if (!i->curr_size) {
                i->empty_frame_count++;

                if (i->empty_frame_count >= 60) {
                    i->Destroy();
                    i = staging_buffers.erase(i);
                    i_stg_begin = staging_buffers.begin();
                    continue;
                }
            }
            else
                i->empty_frame_count = 0;

            i->curr_size = 0;
        }
    }

    manager::manager(uint32_t max_frames) : frame() {
        frames_data.resize(max_frames);
    }

    manager::~manager() {

    }

    void manager::free_framebuffer(VkDevice device,
        VkFramebuffer framebuffer, const VkAllocationCallbacks* allocator) {
        if (framebuffer) {
            frame_data& frame_data = get_frame_data();
            frame_data.free_framebuffers.push_back({ device, framebuffer, allocator });
        }
    }

    void manager::free_image_view(VkDevice device,
        VkImageView image_view, const VkAllocationCallbacks* allocator) {
        if (image_view) {
            frame_data& frame_data = get_frame_data();
            frame_data.free_image_views.push_back({ device, image_view, allocator });
        }
    }

    prj::shared_ptr<Vulkan::DescriptorPipeline> manager::get_descriptor_pipeline(
        uint32_t sampler_count, uint32_t uniform_count, uint32_t storage_count,
        const VkDescriptorSetLayoutBinding* bindings,
        uint32_t push_constant_range_count, VkPushConstantRange* push_constant_ranges) {
        uint64_t bindings_hash = hash_xxh3_64bits(bindings,
            sizeof(VkDescriptorSetLayoutBinding) * ((size_t)sampler_count + uniform_count + storage_count ));
        uint64_t push_constant_ranges_hash = hash_xxh3_64bits(push_constant_ranges,
            sizeof(VkPushConstantRange) * push_constant_range_count);
        auto elem = descriptor_pipelines.find({ sampler_count, uniform_count,
            storage_count, bindings_hash, push_constant_ranges_hash });
        if (elem != descriptor_pipelines.end())
            return elem->second;

        prj::shared_ptr<Vulkan::DescriptorPipeline> descriptor_pipeline(
            new Vulkan::DescriptorPipeline(Vulkan::current_device, Vulkan::current_descriptor_pool,
                sampler_count, uniform_count, storage_count,
                bindings, push_constant_range_count, push_constant_ranges));
        descriptor_pipelines.insert({ { sampler_count, uniform_count, storage_count,
            bindings_hash, push_constant_ranges_hash }, descriptor_pipeline });
        return descriptor_pipeline;
    }

    manager::frame_data& manager::get_frame_data() {
        return frames_data.data()[frame];
    }

    prj::shared_ptr<Vulkan::Pipeline> manager::get_pipeline(uint32_t stage_count,
        const VkPipelineShaderStageCreateInfo* stages,
        uint32_t vertex_input_binding_description_count,
        const VkVertexInputBindingDescription* vertex_input_binding_descriptions,
        uint32_t vertex_input_attribute_description_count,
        const VkVertexInputAttributeDescription* vertex_input_attribute_descriptions,
        const VkPipelineInputAssemblyStateCreateInfo* input_assembly_state,
        const VkPipelineRasterizationStateCreateInfo* rasterization_state,
        const VkPipelineDepthStencilStateCreateInfo* depth_stencil_state,
        uint32_t color_blend_attachment_count,
        const VkPipelineColorBlendAttachmentState* color_blend_attachments,
        bool line_width, bool stencil, VkPipelineLayout layout, VkRenderPass render_pass) {
        uint64_t stages_hash = hash_xxh3_64bits(stages,
            sizeof(VkPipelineShaderStageCreateInfo) * stage_count);
        uint64_t vertex_input_info_binding_hash = hash_xxh3_64bits(vertex_input_binding_descriptions,
            sizeof(VkVertexInputBindingDescription) * vertex_input_binding_description_count);
        uint64_t vertex_input_info_attribute_hash = hash_xxh3_64bits(vertex_input_attribute_descriptions,
            sizeof(VkVertexInputAttributeDescription) * vertex_input_attribute_description_count);
        uint64_t input_assembly_state_hash = hash_xxh3_64bits(input_assembly_state,
            sizeof(VkPipelineInputAssemblyStateCreateInfo));
        uint64_t rasterization_state_hash = hash_xxh3_64bits(rasterization_state,
            sizeof(VkPipelineRasterizationStateCreateInfo));
        uint64_t depth_stencil_state_hash = hash_xxh3_64bits(depth_stencil_state,
            sizeof(VkPipelineDepthStencilStateCreateInfo));
        uint64_t color_blend_attachmen_hash = hash_xxh3_64bits(color_blend_attachments,
            sizeof(VkPipelineColorBlendAttachmentState) * color_blend_attachment_count);

        auto elem = pipelines.find({ stages_hash, vertex_input_info_binding_hash, vertex_input_info_attribute_hash,
            input_assembly_state_hash, rasterization_state_hash,
            depth_stencil_state_hash, color_blend_attachmen_hash,
            line_width, stencil, layout, render_pass });
        if (elem != pipelines.end())
            return elem->second;

        VkPipelineViewportStateCreateInfo viewport_state = {};
        viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state.viewportCount = 1;
        viewport_state.scissorCount = 1;

        prj::shared_ptr<Vulkan::Pipeline> pipeline(new Vulkan::Pipeline(Vulkan::current_device, 0,
            stage_count, stages, vertex_input_binding_description_count, vertex_input_binding_descriptions,
            vertex_input_attribute_description_count, vertex_input_attribute_descriptions,
            input_assembly_state, &viewport_state, rasterization_state, depth_stencil_state,
            color_blend_attachment_count, color_blend_attachments, line_width, stencil, layout, render_pass));
        pipelines.insert({ { stages_hash, vertex_input_info_binding_hash, vertex_input_info_attribute_hash,
            input_assembly_state_hash, rasterization_state_hash,
            depth_stencil_state_hash, color_blend_attachmen_hash,
            line_width, stencil, layout, render_pass }, pipeline });
        return pipeline;
    }

    prj::shared_ptr<Vulkan::RenderPass> manager::get_render_pass(GLenum* color_formats,
        uint32_t color_format_count, GLenum depth_format, bool depth_read_only) {
        uint64_t color_formats_hash = hash_xxh3_64bits(color_formats,
            sizeof(GLenum) * color_format_count);

        auto elem = render_passes.find({ color_formats_hash, depth_format, depth_read_only });
        if (elem != render_passes.end())
            return elem->second;

        VkAttachmentReference* color_attachment_reference = force_malloc<VkAttachmentReference>(color_format_count);
        VkAttachmentReference depth_attachment_reference = {};

        VkSubpassDescription subpass_description = {};
        subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        if (color_format_count) {
            for (uint32_t i = 0; i < color_format_count; i++) {
                color_attachment_reference[i].attachment = i;
                color_attachment_reference[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            }

            subpass_description.colorAttachmentCount = color_format_count;
            subpass_description.pColorAttachments = color_attachment_reference;
        }
        else
            subpass_description.colorAttachmentCount = 0;

        if (depth_format) {
            depth_attachment_reference.attachment = 1;
            depth_attachment_reference.layout = depth_read_only
                ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
                : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            subpass_description.pDepthStencilAttachment = &depth_attachment_reference;
        }

        VkSubpassDependency subpass_dependency = {};
        subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        subpass_dependency.dstSubpass = 0;
        subpass_dependency.srcStageMask = 0;
        subpass_dependency.srcAccessMask = 0;
        subpass_dependency.dstStageMask = 0;
        subpass_dependency.dstAccessMask = 0;

        if (color_format_count) {
            subpass_dependency.srcStageMask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            subpass_dependency.dstStageMask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            subpass_dependency.dstAccessMask |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        }

        if (depth_format) {
            subpass_dependency.srcStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            subpass_dependency.dstStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            if (!depth_read_only)
                subpass_dependency.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        }

        int32_t attachment_count = 0;

        VkAttachmentDescription* attachments
            = force_malloc<VkAttachmentDescription>((size_t)color_format_count + 1);
        if (color_format_count)
            for (uint32_t i = 0; i < color_format_count; i++) {
                VkAttachmentDescription& color_attachment = attachments[attachment_count];
                color_attachment.flags = 0;
                color_attachment.format = Vulkan::get_format(color_formats[i]);
                color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
                color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
                color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                color_attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                color_attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                attachment_count++;
            }

        if (depth_format) {
            const bool stencil = depth_format == GL_DEPTH24_STENCIL8;
            VkAttachmentDescription& depth_attachment = attachments[attachment_count];
            depth_attachment.flags = 0;
            depth_attachment.format = Vulkan::get_format(depth_format);
            depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
            depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            depth_attachment.stencilLoadOp = stencil
                ? VK_ATTACHMENT_LOAD_OP_LOAD : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            depth_attachment.stencilStoreOp = stencil
                ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;
            depth_attachment.initialLayout = depth_read_only
                ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
                : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            depth_attachment.finalLayout = depth_read_only
                ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
                : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            attachment_count++;
        }

        prj::shared_ptr<Vulkan::RenderPass> render_pass(new Vulkan::RenderPass(Vulkan::current_device, 0,
            attachment_count, attachments, 1, &subpass_description, 1, &subpass_dependency));
        render_passes.insert({ { color_formats_hash, depth_format, depth_read_only }, render_pass });

        free_def(attachments);
        free_def(color_attachment_reference);
        return render_pass;
    }

    static VkSamplerAddressMode get_sampler_address_mode(GLenum wrap) {
        switch (wrap) {
        case GL_REPEAT:
        default:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case GL_CLAMP_TO_BORDER:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        case GL_CLAMP_TO_EDGE:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case GL_MIRRORED_REPEAT:
            return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case GL_MIRROR_CLAMP_TO_EDGE:
            return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
        }
    }

    prj::shared_ptr<Vulkan::Sampler> manager::get_sampler(const gl_sampler& sampler_data) {
        uint64_t sampler_hash = hash_xxh3_64bits(&sampler_data, sizeof(gl_sampler));

        auto elem = samplers.find(sampler_hash);
        if (elem != samplers.end())
            return elem->second;

        VkFilter vk_mag_filter = sampler_data.mag_filter != GL_NEAREST ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
        VkFilter vk_min_filter;
        VkSamplerMipmapMode vk_mipmap_mode;
        switch (sampler_data.min_filter) {
        case GL_NEAREST:
        case GL_NEAREST_MIPMAP_NEAREST:
            vk_min_filter = VK_FILTER_NEAREST;
            vk_mipmap_mode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
            break;
        case GL_LINEAR:
        case GL_LINEAR_MIPMAP_NEAREST:
        default:
            vk_min_filter = VK_FILTER_LINEAR;
            vk_mipmap_mode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
            break;
        case GL_NEAREST_MIPMAP_LINEAR:
            vk_min_filter = VK_FILTER_NEAREST;
            vk_mipmap_mode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            break;
        case GL_LINEAR_MIPMAP_LINEAR:
            vk_min_filter = VK_FILTER_LINEAR;
            vk_mipmap_mode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            break;
        }

        VkBorderColor vk_border_color;
        if (sampler_data.border_color == 0.0f)
            vk_border_color = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        else if (sampler_data.border_color == vec4(0.0f, 0.0f, 0.0f, 1.0f))
            vk_border_color = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
        else if (sampler_data.border_color == 1.0f)
            vk_border_color = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        else
            vk_border_color = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;

        prj::shared_ptr<Vulkan::Sampler> sampler(new Vulkan::Sampler(Vulkan::current_device, 0,
            vk_mag_filter, vk_min_filter, vk_mipmap_mode, sampler_data.lod_bias,
            get_sampler_address_mode(sampler_data.wrap_s),
            get_sampler_address_mode(sampler_data.wrap_t),
            get_sampler_address_mode(sampler_data.wrap_r),
            sampler_data.min_lod, sampler_data.max_lod,
            sampler_data.max_anisotropy, vk_border_color));
        samplers.insert({ sampler_hash, sampler });
        return sampler;
    }

    Vulkan::Buffer manager::get_dynamic_buffer(VkDeviceSize size, VkDeviceSize alignment) {
        frame_data& frame_data = get_frame_data();

        for (Vulkan::DynamicBuffer& i : frame_data.dynamic_buffers)
            if (align_val(i.curr_size, alignment) + align_val(size, alignment) <= i.GetSize()) {
                i.curr_size = align_val(i.curr_size, alignment);

                Vulkan::Buffer buffer = (Vulkan::Buffer)i;
                buffer.SetOffset(i.curr_size);
                buffer.SetSize(size);
                i.curr_size += align_val(size, alignment);
                return buffer;
            }

        VkDeviceSize  buffer_size = 0x1000000;
        while (buffer_size <= size)
            buffer_size *= 2;

        frame_data.dynamic_buffers.push_back({});
        Vulkan::DynamicBuffer& dynamic_buffer = frame_data.dynamic_buffers.back();
        dynamic_buffer.Create(Vulkan::current_allocator, buffer_size);

        Vulkan::Buffer buffer = (Vulkan::Buffer)dynamic_buffer;
        buffer.SetOffset(0);
        buffer.SetSize(size);
        dynamic_buffer.curr_size = align_val(size, alignment);
        return buffer;
    }

    Vulkan::Buffer manager::get_staging_buffer(VkDeviceSize size, VkDeviceSize alignment) {
        frame_data& frame_data = get_frame_data();

        for (Vulkan::StagingBuffer& i : frame_data.staging_buffers)
            if (align_val(i.curr_size, alignment) + align_val(size, alignment) <= i.GetSize()) {
                i.curr_size = align_val(i.curr_size, alignment);

                Vulkan::Buffer buffer = (Vulkan::Buffer)i;
                buffer.SetOffset(i.curr_size);
                buffer.SetSize(size);
                i.curr_size += align_val(size, alignment);
                return buffer;
            }

        VkDeviceSize buffer_size = 0x1000000;
        while (buffer_size <= size)
            buffer_size *= 2;

        frame_data.staging_buffers.push_back({});
        Vulkan::StagingBuffer& staging_buffer = frame_data.staging_buffers.back();
        staging_buffer.Create(Vulkan::current_allocator, buffer_size);

        Vulkan::Buffer buffer = (Vulkan::Buffer)staging_buffer;
        buffer.SetOffset(0);
        buffer.SetSize(size);
        staging_buffer.curr_size = align_val(size, alignment);
        return buffer;
    }

    void manager::next_frame(uint32_t frame) {
        this->frame = frame;
        get_frame_data().ctrl();
    }

    void manager::reset_descriptor_pipelines_descriptor_set_collections() {
        for (auto& i : descriptor_pipelines)
            i.second.get()->ResetSetCollectionsState();
    }
}
