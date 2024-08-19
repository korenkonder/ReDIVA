/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "shared.hpp"

namespace Vulkan {
    class Semaphore {
    protected:
        VkDevice device;
        VkSemaphore data;

    public:
        inline Semaphore() : device(), data() {

        }

        bool Create(VkDevice device, VkSemaphoreCreateFlags flags);
        void Destroy();

        inline operator VkSemaphore() const {
            return data;
        }

        inline operator const VkSemaphore* () const {
            return &data;
        }
    };
}
