/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "Buffer.hpp"

namespace Vulkan {
    class DataBuffer : public Buffer {
    public:
        VkDeviceSize curr_size;
        VkDeviceSize offset;

        inline DataBuffer() : curr_size(), offset() {

        }

        bool Create(VmaAllocator allocator, size_t size, bool uniform);
    };
}
