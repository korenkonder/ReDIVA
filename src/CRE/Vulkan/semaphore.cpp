/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "semaphore.hpp"

namespace Vulkan {
    Semaphore::Semaphore() : device(), data() {

    }

    Semaphore::~Semaphore() {

    }

    bool Semaphore::Create(VkDevice device, VkSemaphoreCreateFlags flags) {
        if (data)
            return true;

        VkSemaphoreCreateInfo semaphore_create_info;
        semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphore_create_info.pNext = 0;
        semaphore_create_info.flags = flags;

        if (vkCreateSemaphore(device, &semaphore_create_info, 0, &data) != VK_SUCCESS) {
            this->device = 0;
            data = 0;
            return false;
        }

        this->device = device;
        return true;
    }

    void Semaphore::Destroy() {
        if (data) {
            vkDestroySemaphore(device, data, 0);
            data = 0;
        }
        device = 0;
    }
}
