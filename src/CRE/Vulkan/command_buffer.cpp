/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "command_buffer.hpp"

namespace Vulkan {
    CommandBuffer::CommandBuffer() : device(), command_pool(), data() {

    }

    CommandBuffer::~CommandBuffer() {

    }

    bool CommandBuffer::Allocate(VkDevice device, VkCommandPool command_pool, VkCommandBufferLevel level) {
        if (data)
            return true;

        VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
        command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        command_buffer_allocate_info.commandPool = command_pool;
        command_buffer_allocate_info.level = level;
        command_buffer_allocate_info.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(device, &command_buffer_allocate_info, &data) != VK_SUCCESS) {
            this->device = 0;
            this->command_pool = 0;
            data = 0;
            return false;
        }

        this->device = device;
        this->command_pool = command_pool;
        return true;
    }

    void CommandBuffer::Free() {
        if (data)
            vkFreeCommandBuffers(device, command_pool, 1, &data);

        device = 0;
        command_pool = 0;
        data = 0;
    }
}
