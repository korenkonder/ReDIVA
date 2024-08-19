/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "RenderPass.hpp"

namespace Vulkan {
    RenderPass::RenderPass(VkDevice device, VkRenderPassCreateFlags flags,
        uint32_t attachment_count, const VkAttachmentDescription* sttachments,
        uint32_t subpass_count, const VkSubpassDescription* subpasses,
        uint32_t dependency_count, const VkSubpassDependency* dependencies) {

        VkRenderPassCreateInfo render_pass_create_info;
        render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_create_info.pNext = 0;
        render_pass_create_info.flags = flags;
        render_pass_create_info.attachmentCount = attachment_count;
        render_pass_create_info.pAttachments = sttachments;
        render_pass_create_info.subpassCount = subpass_count;
        render_pass_create_info.pSubpasses = subpasses;
        render_pass_create_info.dependencyCount = dependency_count;
        render_pass_create_info.pDependencies = dependencies;

        this->device = device;
        if (vkCreateRenderPass(device, &render_pass_create_info, 0, &data) == VK_SUCCESS)
            return;

        this->device = 0;
        data = 0;
    }

    RenderPass::~RenderPass() {
        if (data) {
            vkDestroyRenderPass(device, data, 0);
            data = 0;
        }
        device = 0;
    }
}
