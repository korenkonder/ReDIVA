/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include <vulkan/vulkan.h>

namespace Vulkan {
    class Fence {
    protected:
        VkDevice device;

    public:
        VkFence data;

        Fence();
        ~Fence();

        bool Create(VkDevice device, VkFenceCreateFlags flags = 0);
        void Destroy();
        void Reset();
        void WaitFor();
    };
}
