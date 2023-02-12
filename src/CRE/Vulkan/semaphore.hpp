/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include <vulkan/vulkan.h>

namespace Vulkan {
    class Semaphore {
    protected:
        VkDevice device;

    public:
        VkSemaphore data;

        Semaphore();
        ~Semaphore();

        bool Create(VkDevice device, VkSemaphoreCreateFlags flags = 0);
        void Destroy();
    };
}
