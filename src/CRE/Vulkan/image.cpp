/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "image.hpp"

namespace Vulkan {
    Image::Image() : allocator(), data(), allocation() {

    }

    Image::~Image() {

    }

    bool Image::Create(VmaAllocator allocator,
        uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
        VkImageUsageFlags usage, VmaMemoryUsage memory_usage, VmaAllocationCreateFlags flags) {
        if (allocation)
            return true;

        VkImageCreateInfo image_create_info = {};
        image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_create_info.imageType = VK_IMAGE_TYPE_2D;
        image_create_info.extent.width = width;
        image_create_info.extent.height = height;
        image_create_info.extent.depth = 1;
        image_create_info.mipLevels = 1;
        image_create_info.arrayLayers = 1;
        image_create_info.format = format;
        image_create_info.tiling = tiling;
        image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_create_info.usage = usage;
        image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocation_create_info = {};
        allocation_create_info.usage = memory_usage;
        allocation_create_info.flags = flags;

        if (vmaCreateImage(allocator, &image_create_info,
            &allocation_create_info, &data, &allocation, 0) != VK_SUCCESS) {
            this->allocator = 0;
            data = 0;
            allocation = 0;
            return false;
        }

        this->allocator = allocator;
        return true;
    }

    void Image::Destroy() {
        if (allocation)
            vmaDestroyImage(allocator, data, allocation);

        this->allocator = 0;
        data = 0;
        allocation = 0;
    }

    void* Image::MapMemory() {
        void* data;
        vmaMapMemory(allocator, allocation, &data);
        return data;
    }

    void Image::UnmapMemory() {
        vmaUnmapMemory(allocator, allocation);
    }

    void Image::WriteMapMemory(const void* data, size_t offset, size_t size) {
        void* _data;
        vmaMapMemory(allocator, allocation, &_data);
        memcpy((void*)((size_t)_data + offset), data, size);
        vmaUnmapMemory(allocator, allocation);
    }
}
