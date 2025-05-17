/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "Buffer.hpp"
#include "CommandBuffer.hpp"

namespace Vulkan {
    class StorageBuffer : public Buffer {
    public:
        inline StorageBuffer() {

        }

        inline bool Create(VmaAllocator allocator, size_t size) {
            return Buffer::Create(allocator, size,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT
                | VK_BUFFER_USAGE_TRANSFER_DST_BIT
                | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                VMA_MEMORY_USAGE_AUTO, 0);
        }
    };
}
