/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "buffer.hpp"

namespace Vulkan {
    Buffer::Buffer() : allocator(), data(), allocation() {

    }

    Buffer::~Buffer() {

    }

    bool Buffer::Create(VmaAllocator allocator, VkDeviceSize size,
        VkBufferUsageFlags usage, VmaMemoryUsage memory_usage, VmaAllocationCreateFlags flags) {
        return Create(allocator, size, usage, VK_SHARING_MODE_EXCLUSIVE, memory_usage, flags);
    }

    bool Buffer::Create(VmaAllocator allocator, VkDeviceSize size,
        VkBufferUsageFlags usage, VkSharingMode sharing_mode,
        VmaMemoryUsage memory_usage, VmaAllocationCreateFlags flags) {
        if (allocation)
            return true;

        VkBufferCreateInfo buffer_create_info = {};
        buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_create_info.size = size;
        buffer_create_info.usage = usage;
        buffer_create_info.sharingMode = sharing_mode;

        VmaAllocationCreateInfo allocation_create_info = {};
        allocation_create_info.usage = memory_usage;
        allocation_create_info.flags = flags;

        if (vmaCreateBuffer(allocator, &buffer_create_info,
            &allocation_create_info, &data, &allocation, 0) != VK_SUCCESS) {
            this->allocator = 0;
            data = 0;
            allocation = 0;
            return false;
        }

        this->allocator = allocator;
        return true;
    }

    void Buffer::Destroy() {
        if (allocation)
            vmaDestroyBuffer(allocator, data, allocation);

        this->allocator = 0;
        data = 0;
        allocation = 0;
    }

    void* Buffer::MapMemory() {
        void* data;
        vmaMapMemory(allocator, allocation, &data);
        return data;
    }

    void Buffer::UnmapMemory() {
        vmaUnmapMemory(allocator, allocation);
    }

    void Buffer::WriteMapMemory(const void* data, size_t offset, size_t size) {
        void* _data;
        vmaMapMemory(allocator, allocation, &_data);
        memcpy((void*)((size_t)_data + offset), data, size);
        vmaUnmapMemory(allocator, allocation);
    }
}
