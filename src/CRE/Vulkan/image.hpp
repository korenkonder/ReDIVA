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
    class Image {
    protected:
        VmaAllocator allocator;

    public:
        VkImage data;
        VmaAllocation allocation;

        Image();
        ~Image();

        bool Create(VmaAllocator allocator,
            uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
            VkImageUsageFlags usage, VmaMemoryUsage memory_usage, VmaAllocationCreateFlags flags = 0);
        void Destroy();
        void* MapMemory();
        void UnmapMemory();
        void WriteMapMemory(const void* data, size_t offset, size_t size);
    };
}
