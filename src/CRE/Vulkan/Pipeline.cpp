/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "Pipeline.hpp"

namespace Vulkan {
    Pipeline::Pipeline(VkDevice device, VkPipelineCreateFlags flags, uint32_t stage_count,
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
        bool line_width, bool stencil, VkPipelineLayout layout, VkRenderPass render_pass) {
        VkPipelineVertexInputStateCreateInfo vertex_input_state = {};
        vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_state.vertexBindingDescriptionCount = vertex_input_binding_description_count;
        vertex_input_state.pVertexBindingDescriptions = vertex_input_binding_descriptions;
        vertex_input_state.vertexAttributeDescriptionCount = vertex_input_attribute_description_count;
        vertex_input_state.pVertexAttributeDescriptions = vertex_input_attribute_descriptions;

        VkPipelineMultisampleStateCreateInfo multisample_state = {};
        multisample_state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisample_state.sampleShadingEnable = VK_FALSE;
        multisample_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendStateCreateInfo color_blend_state = {};
        color_blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blend_state.logicOpEnable = VK_FALSE;
        color_blend_state.logicOp = VK_LOGIC_OP_COPY;
        color_blend_state.attachmentCount = color_blend_attachment_count;
        color_blend_state.pAttachments = color_blend_attachments;
        color_blend_state.blendConstants[0] = 0.0f;
        color_blend_state.blendConstants[1] = 0.0f;
        color_blend_state.blendConstants[2] = 0.0f;
        color_blend_state.blendConstants[3] = 0.0f;

        uint32_t dynamic_state_count = 0;
        VkDynamicState dynamic_states[6];
        dynamic_states[dynamic_state_count++] = VK_DYNAMIC_STATE_VIEWPORT;
        dynamic_states[dynamic_state_count++] = VK_DYNAMIC_STATE_SCISSOR;

        if (line_width)
            dynamic_states[dynamic_state_count++] = VK_DYNAMIC_STATE_LINE_WIDTH;

        if (stencil) {
            dynamic_states[dynamic_state_count++] = VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK;
            dynamic_states[dynamic_state_count++] = VK_DYNAMIC_STATE_STENCIL_WRITE_MASK;
            dynamic_states[dynamic_state_count++] = VK_DYNAMIC_STATE_STENCIL_REFERENCE;
        }

        VkPipelineDynamicStateCreateInfo dynamic_state = {};
        dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state.dynamicStateCount = dynamic_state_count;
        dynamic_state.pDynamicStates = dynamic_states;

        VkGraphicsPipelineCreateInfo graphics_pipeline_create_info;
        graphics_pipeline_create_info.pNext = 0;
        graphics_pipeline_create_info.flags = flags;
        graphics_pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        graphics_pipeline_create_info.stageCount = stage_count;
        graphics_pipeline_create_info.pStages = stages;
        graphics_pipeline_create_info.pVertexInputState = &vertex_input_state;
        graphics_pipeline_create_info.pInputAssemblyState = input_assembly_state;
        graphics_pipeline_create_info.pTessellationState = 0;
        graphics_pipeline_create_info.pViewportState = viewport_state;
        graphics_pipeline_create_info.pRasterizationState = rasterization_state;
        graphics_pipeline_create_info.pMultisampleState = &multisample_state;
        graphics_pipeline_create_info.pDepthStencilState = depth_stencil_state;
        graphics_pipeline_create_info.pColorBlendState = &color_blend_state;
        graphics_pipeline_create_info.pDynamicState = &dynamic_state;
        graphics_pipeline_create_info.layout = layout;
        graphics_pipeline_create_info.renderPass = render_pass;
        graphics_pipeline_create_info.subpass = 0;
        graphics_pipeline_create_info.basePipelineHandle = 0;
        graphics_pipeline_create_info.basePipelineIndex = 0;

        this->device = device;
        if (vkCreateGraphicsPipelines(device, 0, 1, &graphics_pipeline_create_info, 0, &data) == VK_SUCCESS)
            return;

        this->device = 0;
        data = 0;
    }

    Pipeline::~Pipeline() {
        if (data) {
            vkDestroyPipeline(device, data, 0);
            data = 0;
        }
        device = 0;
    }
}
