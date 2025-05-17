/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "DynamicBuffer.hpp"

namespace Vulkan {
    bool DynamicBuffer::Create(VmaAllocator allocator, size_t size) {
        curr_size = 0;
        offset = 0;
        return Buffer::Create(allocator, size,
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT
            | VK_IMAGE_USAGE_TRANSFER_DST_BIT
            | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
            | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
            | VK_BUFFER_USAGE_INDEX_BUFFER_BIT
            | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VMA_MEMORY_USAGE_AUTO,
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
            | VMA_ALLOCATION_CREATE_MAPPED_BIT);
    }
}
