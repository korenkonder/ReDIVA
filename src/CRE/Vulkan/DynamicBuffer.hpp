/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "Buffer.hpp"

namespace Vulkan {
    class DynamicBuffer : public Buffer {
    public:
        VkDeviceSize curr_size;
        uint32_t empty_frame_count;

        inline DynamicBuffer() : curr_size(), empty_frame_count() {

        }

        bool Create(VmaAllocator allocator, size_t size);
    };
}
