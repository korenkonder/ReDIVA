/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "StagingBuffer.hpp"

namespace Vulkan {
    bool StagingBuffer::Create(VmaAllocator allocator, size_t size) {
        curr_size = 0;
        offset = 0;
        return Buffer::Create(allocator, size,
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT
            | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
            VMA_MEMORY_USAGE_AUTO,
            VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT
            | VMA_ALLOCATION_CREATE_MAPPED_BIT);
    }
}
