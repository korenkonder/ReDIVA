/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include <vulkan/vulkan.h>

namespace Vulkan {
    class Framebuffer {
    protected:
        VkDevice device;

    public:
        VkFramebuffer data;

        Framebuffer();
        ~Framebuffer();

        bool Create(VkDevice device, VkFramebufferCreateFlags flags,
            VkRenderPass render_pass, uint32_t attachment_count, const VkImageView* attachments,
            uint32_t width, uint32_t height, uint32_t layers);
        void Destroy();
    };
}
