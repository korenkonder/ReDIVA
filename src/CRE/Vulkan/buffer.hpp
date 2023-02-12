/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include <vulkan/vulkan.h>
#define VMA_VULKAN_VERSION 1000000
#include <vk_mem_alloc.h>

namespace Vulkan {
    class Buffer {
    protected:
        VmaAllocator allocator;

    public:
        VkBuffer data;
        VmaAllocation allocation;

        Buffer();
        ~Buffer();

        bool Create(VmaAllocator allocator, VkDeviceSize size,
            VkBufferUsageFlags usage, VmaMemoryUsage memory_usage, VmaAllocationCreateFlags flags = 0);
        bool Create(VmaAllocator allocator, VkDeviceSize size,
            VkBufferUsageFlags usage, VkSharingMode sharing_mode,
            VmaMemoryUsage memory_usage, VmaAllocationCreateFlags flags = 0);
        void Destroy();
        void* MapMemory();
        void UnmapMemory();
        void WriteMapMemory(const void* data, size_t offset, size_t size);
    };
}
