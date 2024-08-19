/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "shared.hpp"

namespace Vulkan {
    class RenderPass {
    protected:
        VkDevice device;
        VkRenderPass data;

    public:
        RenderPass(VkDevice device, VkRenderPassCreateFlags flags,
            uint32_t attachment_count, const VkAttachmentDescription* sttachments,
            uint32_t subpass_count, const VkSubpassDescription* subpasses,
            uint32_t dependency_count, const VkSubpassDependency* dependencies);
        ~RenderPass();

        inline operator VkRenderPass() const {
            return data;
        }

        RenderPass(const RenderPass&) = delete;
        RenderPass& operator=(const RenderPass&) = delete;
    };
}
