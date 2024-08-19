/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "shared.hpp"

namespace Vulkan {
    class Image {
    friend class CommandBuffer;
    protected:
        VmaAllocator allocator;
        VkImage data;
        uint32_t mip_levels;
        uint32_t array_layers;
        VkImageLayout* image_layout;
        VmaAllocation allocation;

    public:
        inline Image() : allocator(), data(), image_layout(), mip_levels(), array_layers(), allocation() {

        }

        bool Create(VmaAllocator allocator, VkImageCreateFlags flags, uint32_t width, uint32_t height,
            uint32_t mip_levels, uint32_t array_layers, VkFormat format, VkImageTiling tiling,
            VkImageUsageFlags usage, VmaMemoryUsage memory_usage, VmaAllocationCreateFlags allocation_flags);
        void Destroy();
        VkImageLayout GetImageLayout(uint32_t level, uint32_t layer);
        void SetImageLayout(VkImageLayout image_layout);
        void SetImageLayout(VkImageLayout image_layout, uint32_t level, uint32_t layer);

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

        static bool PipelineBarrier(VkCommandBuffer command_buffer, VkImage image,
            VkImageAspectFlags aspect_mask, uint32_t level_count, uint32_t layer_count,
            VkImageLayout old_layout, VkImageLayout new_layout);
        static bool PipelineBarrier(VkCommandBuffer command_buffer, Vulkan::Image& image,
            VkImageAspectFlags aspect_mask, uint32_t level_count, uint32_t layer_count, VkImageLayout new_layout);
        static bool PipelineBarrierSingle(VkCommandBuffer command_buffer, VkImage image,
            VkImageAspectFlags aspect_mask, uint32_t level, uint32_t layer,
            VkImageLayout old_layout, VkImageLayout new_layout);
        static bool PipelineBarrierSingle(VkCommandBuffer command_buffer, Vulkan::Image& image,
            VkImageAspectFlags aspect_mask, uint32_t level, uint32_t layer, VkImageLayout new_layout);

        inline operator VkImage() const {
            return data;
        }
    };
}
