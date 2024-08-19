/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "Buffer.hpp"

namespace Vulkan {
    class StagingBuffer : public Buffer {
    public:
        VkDeviceSize curr_size;
        uint32_t empty_frame_count;

        inline StagingBuffer() : curr_size(), empty_frame_count() {

        }

        bool Create(VmaAllocator allocator, size_t size);
    };
}
