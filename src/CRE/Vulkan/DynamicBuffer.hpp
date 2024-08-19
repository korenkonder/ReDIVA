/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "Buffer.hpp"

namespace Vulkan {
    class DynamicBuffer : public Buffer {
    public:
        VkBufferUsageFlags usage;
        VkDeviceSize curr_size;
        uint32_t empty_frame_count;

        inline DynamicBuffer() : usage(), curr_size(), empty_frame_count() {

        }

        bool Create(VmaAllocator allocator, size_t size, VkBufferUsageFlags usage);
    };
}
