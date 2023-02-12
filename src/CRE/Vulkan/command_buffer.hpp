/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include <vulkan/vulkan.h>

namespace Vulkan {
    class CommandBuffer {
    protected:
        VkDevice device;
        VkCommandPool command_pool;

    public:
        VkCommandBuffer data;

        CommandBuffer();
        ~CommandBuffer();

        bool Allocate(VkDevice device, VkCommandPool command_pool,
            VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        void Free();
    };
}
