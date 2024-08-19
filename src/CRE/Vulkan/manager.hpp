/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "shared.hpp"
#include "gl_wrap.hpp"
#include "DescriptorPipeline.hpp"
#include "DynamicBuffer.hpp"
#include "Pipeline.hpp"
#include "StagingBuffer.hpp"

namespace Vulkan {
    extern void manager_init(uint32_t max_frames);
    extern void manager_free_descriptor_pipelines();
    extern void manager_free_framebuffer(VkDevice device,
        VkFramebuffer framebuffer, const VkAllocationCallbacks* allocator);
    extern void manager_free_image_view(VkDevice device,
        VkImageView image_view, const VkAllocationCallbacks* allocator);
    extern void manager_free_pipelines();
    extern void manager_free_render_passes();
    extern void manager_free_samplers();
    extern prj::shared_ptr<Vulkan::DescriptorPipeline> manager_get_descriptor_pipeline(
        uint64_t vp_desc_hash, uint64_t fp_desc_hash, uint64_t unival_hash);
    extern prj::shared_ptr<Vulkan::DescriptorPipeline> manager_get_descriptor_pipeline(
        uint64_t vp_desc_hash, uint64_t fp_desc_hash, uint64_t unival_hash,
        uint32_t sampler_count, uint32_t uniform_count, uint32_t storage_count,
        const VkDescriptorSetLayoutBinding* bindings);
    extern uint32_t manager_get_frame();
    extern prj::shared_ptr<Vulkan::Pipeline> manager_get_pipeline(uint32_t stage_count,
        const VkPipelineShaderStageCreateInfo* stages,
        uint32_t vertex_input_binding_descriptions_count,
        const VkVertexInputBindingDescription* vertex_input_binding_descriptions,
        uint32_t vertex_input_attribute_description_count,
        const VkVertexInputAttributeDescription* vertex_input_attribute_descriptions,
        const VkPipelineInputAssemblyStateCreateInfo* input_assembly_state,
        const VkRect2D viewport_scissor_rect[2],
        const VkPipelineRasterizationStateCreateInfo* rasterization_state,
        const VkPipelineDepthStencilStateCreateInfo* depth_stencil_state,
        uint32_t color_blend_attachment_count,
        const VkPipelineColorBlendAttachmentState* color_blend_attachments,
        VkPipelineLayout layout, VkRenderPass render_pass);
    extern prj::shared_ptr<Vulkan::RenderPass> manager_get_render_pass(
        GLenum* color_formats, uint32_t color_format_count, GLenum depth_format, bool stencil = false);
    extern prj::shared_ptr<Vulkan::Sampler> manager_get_sampler(const gl_sampler& sampler_data);
    Vulkan::Buffer manager_get_dynamic_buffer(VkDeviceSize size, VkDeviceSize alignment = 0x10);
    Vulkan::Buffer manager_get_dynamic_uniform_buffer(VkDeviceSize size, VkDeviceSize alignment = 0x10);
    Vulkan::Buffer manager_get_staging_buffer(VkDeviceSize size, VkDeviceSize alignment = 0x10);
    extern void manager_next_frame(uint32_t frame);
    extern void manager_reset_descriptor_pipelines_descriptor_set_collections();
    extern void manager_free();
}
