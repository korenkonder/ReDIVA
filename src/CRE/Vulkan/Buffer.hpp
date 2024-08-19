/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "shared.hpp"

namespace Vulkan {
    class Buffer {
    friend class CommandBuffer;
    protected:
        VmaAllocator allocator;
        VkBuffer data;
        VkDeviceSize offset;
        VkDeviceSize size;
        VmaAllocation allocation;

    public:
        inline Buffer() : allocator(), data(), offset(), size(), allocation() {

        }

        bool Create(VmaAllocator allocator, VkDeviceSize size,
            VkBufferUsageFlags usage, VkSharingMode sharing_mode,
            VmaMemoryUsage memory_usage, VmaAllocationCreateFlags flags);
        void Destroy();

        inline bool Create(VmaAllocator allocator, VkDeviceSize size,
            VkBufferUsageFlags usage, VmaMemoryUsage memory_usage, VmaAllocationCreateFlags flags) {
            return Create(allocator, size, usage, VK_SHARING_MODE_EXCLUSIVE, memory_usage, flags);
        }

        inline bool IsNull() const {
            return !data;
        }

        inline VkDeviceSize GetOffset() const {
            return offset;
        }

        inline VkDeviceSize GetSize() const {
            return size;
        }

        inline void* MapMemory() {
            void* data;
            if (vmaMapMemory(allocator, allocation, &data) == VK_SUCCESS)
                return data;
            vmaUnmapMemory(allocator, allocation);
            return 0;
        }

        inline void* MapMemory(size_t offset, size_t length) {
            void* data;
            if (vmaMapMemory(allocator, allocation, &data) == VK_SUCCESS)
                return (void*)((size_t)data + offset);
            vmaUnmapMemory(allocator, allocation);
            return 0;
        }

        inline bool NotNull() {
            return !!data;
        }

        inline void ReadMemory(size_t offset, size_t size, void* data) {
            void* _data;
            if (vmaMapMemory(allocator, allocation, &_data) == VK_SUCCESS)
                memcpy(data, (void*)((size_t)_data + offset), size);
            vmaUnmapMemory(allocator, allocation);
        }

        template<typename T>
        inline void ReadMemory(T& data) {
            void* _data;
            if (vmaMapMemory(allocator, allocation, &_data) == VK_SUCCESS)
                memcpy(&data, _data, sizeof(T));
            vmaUnmapMemory(allocator, allocation);
        }

        inline void SetOffset(VkDeviceSize offset) {
            this->offset = offset;
        }

        inline void SetSize(VkDeviceSize size) {
            this->size = size;
        }

        inline void UnmapMemory() {
            vmaUnmapMemory(allocator, allocation);
        }

        inline void WriteMemory(size_t offset, size_t size, const void* data) {
            void* _data;
            if (vmaMapMemory(allocator, allocation, &_data) == VK_SUCCESS)
                memcpy((void*)((size_t)_data + offset), data, size);
            vmaUnmapMemory(allocator, allocation);
        }

        template<typename T>
        inline void WriteMemory(T& data) {
            void* _data;
            if (vmaMapMemory(allocator, allocation, &_data) == VK_SUCCESS)
                memcpy(_data, &data, sizeof(T));
            vmaUnmapMemory(allocator, allocation);
        }

        static void Copy(VkCommandBuffer command_buffer, VkBuffer src_buffer, VkBuffer dst_buffer,
            VkDeviceSize src_offset, VkDeviceSize dst_offset, VkDeviceSize size);
        static void CopyToImage(VkCommandBuffer command_buffer,
            VkBuffer src_buffer, VkDeviceSize src_buffer_offset, VkImage dst_image,
            VkImageAspectFlags aspect_mask, uint32_t level, uint32_t layer,
            int32_t x_offset, int32_t y_offset, uint32_t width, uint32_t height);
        static void CopyFromImage(VkCommandBuffer command_buffer,
            VkImage src_image, VkBuffer dst_buffer, VkDeviceSize dst_buffer_offset,
            VkImageAspectFlags aspect_mask, uint32_t level, uint32_t layer,
            int32_t x_offset, int32_t y_offset, uint32_t width, uint32_t height);

        inline operator VkBuffer() const {
            return data;
        }
    };
}
