/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "shared.hpp"

namespace Vulkan {
    class Framebuffer {
    protected:
        VkDevice device;
        VkFramebuffer data;
        VkExtent2D extent;

    public:
        inline Framebuffer() : device(), data(), extent() {

        }

        bool Create(VkDevice device, VkFramebufferCreateFlags flags, VkRenderPass render_pass,
            uint32_t attachment_count, const VkImageView* attachments, uint32_t width, uint32_t height);
        void Destroy();

        inline VkExtent2D GetExtent() const {
            return extent;
        }

        inline operator VkFramebuffer() const {
            return data;
        }
    };
}
