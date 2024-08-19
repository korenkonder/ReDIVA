/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "shared.hpp"

namespace Vulkan {
    class Pipeline {
    protected:
        VkDevice device;
        VkPipeline data;

    public:
        Pipeline(VkDevice device, VkPipelineCreateFlags flags, uint32_t stage_count,
            const VkPipelineShaderStageCreateInfo* stages,
            uint32_t vertex_input_binding_description_count,
            const VkVertexInputBindingDescription* vertex_input_binding_descriptions,
            uint32_t vertex_input_attribute_description_count,
            const VkVertexInputAttributeDescription* vertex_input_attribute_descriptions,
            const VkPipelineInputAssemblyStateCreateInfo* input_assembly_state,
            const VkPipelineViewportStateCreateInfo* viewport_state,
            const VkPipelineRasterizationStateCreateInfo* rasterization_state,
            const VkPipelineDepthStencilStateCreateInfo* depth_stencil_state,
            uint32_t color_blend_attachment_count,
            const VkPipelineColorBlendAttachmentState* color_blend_attachments,
            VkPipelineLayout layout, VkRenderPass render_pass);
        ~Pipeline();

        inline operator VkPipeline() const {
            return data;
        }

        Pipeline(const Pipeline&) = delete;
        Pipeline& operator=(const Pipeline&) = delete;
    };
}
