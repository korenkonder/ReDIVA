/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "Semaphore.hpp"

namespace Vulkan {
    bool Semaphore::Create(VkDevice device, VkSemaphoreCreateFlags flags) {
        Destroy();

        VkSemaphoreCreateInfo semaphore_create_info;
        semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphore_create_info.pNext = 0;
        semaphore_create_info.flags = flags;

        this->device = device;
        if (vkCreateSemaphore(device, &semaphore_create_info, 0, &data) == VK_SUCCESS)
            return true;

        this->device = 0;
        data = 0;
        return false;
    }

    void Semaphore::Destroy() {
        if (data) {
            vkDestroySemaphore(device, data, 0);
            data = 0;
        }
        device = 0;
    }
}
