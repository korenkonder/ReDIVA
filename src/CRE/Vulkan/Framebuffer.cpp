/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "Framebuffer.hpp"
#include "manager.hpp"

namespace Vulkan {
    bool Framebuffer::Create(VkDevice device, VkFramebufferCreateFlags flags, VkRenderPass render_pass,
        uint32_t attachment_count, const VkImageView* attachments, uint32_t width, uint32_t height) {
        Destroy();

        VkFramebufferCreateInfo framebuffer_create_info;
        framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_create_info.pNext = 0;
        framebuffer_create_info.flags = 0;
        framebuffer_create_info.renderPass = render_pass;
        framebuffer_create_info.attachmentCount = attachment_count;
        framebuffer_create_info.pAttachments = attachments;
        framebuffer_create_info.width = width;
        framebuffer_create_info.height = height;
        framebuffer_create_info.layers = 1;

        this->device = device;
        extent.width = width;
        extent.height = height;
        if (vkCreateFramebuffer(device, &framebuffer_create_info, 0, &data) == VK_SUCCESS)
            return true;

        this->device = 0;
        data = 0;
        extent = {};
        return false;
    }

    void Framebuffer::Destroy() {
        extent = {};
        if (data) {
            manager_free_framebuffer(device, data, 0);
            data = 0;
        }
        device = 0;
    }
}
