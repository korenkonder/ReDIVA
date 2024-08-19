/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "Fence.hpp"

namespace Vulkan {
    bool Fence::Create(VkDevice device, VkFenceCreateFlags flags) {
        Destroy();

        VkFenceCreateInfo fence_create_info;
        fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_create_info.pNext = 0;
        fence_create_info.flags = flags;

        this->device = device;
        if (vkCreateFence(device, &fence_create_info, 0, &data) == VK_SUCCESS)
            return true;

        this->device = 0;
        data = 0;
        return false;
    }

    void Fence::Destroy() {
        if (data) {
            vkDestroyFence(device, data, 0);
            data = 0;
        }
        device = 0;
    }

    void Fence::Reset() {
        vkResetFences(device, 1, &data);
    }

    void Fence::WaitFor() {
        vkWaitForFences(device, 1, &data, VK_TRUE, UINT64_MAX);
    }
}
