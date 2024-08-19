/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "shared.hpp"

namespace Vulkan {
    class Fence {
    protected:
        VkDevice device;
        VkFence data;

    public:
        inline Fence() : device(), data() {

        }

        bool Create(VkDevice device, VkFenceCreateFlags flags);
        void Destroy();
        void Reset();
        void WaitFor();

        inline operator VkFence() const {
            return data;
        }
    };
}
