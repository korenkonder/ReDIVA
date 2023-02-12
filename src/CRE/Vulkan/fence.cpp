/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "fence.hpp"

namespace Vulkan {
    Fence::Fence() : device(), data() {

    }

    Fence::~Fence() {

    }

    bool Fence::Create(VkDevice device, VkFenceCreateFlags flags) {
        if (data)
            return true;

        VkFenceCreateInfo fence_create_info;
        fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_create_info.pNext = 0;
        fence_create_info.flags = flags;

        if (vkCreateFence(device, &fence_create_info, 0, &data) != VK_SUCCESS) {
            this->device = 0;
            data = 0;
            return false;
        }

        this->device = device;
        return true;
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
