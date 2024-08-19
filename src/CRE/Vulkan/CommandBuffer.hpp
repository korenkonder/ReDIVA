/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "shared.hpp"
#include "Buffer.hpp"
#include "Fence.hpp"
#include "Image.hpp"
#include <vector>

namespace Vulkan {
    class CommandBuffer {
    protected:
        VkDevice device;
        VkCommandPool command_pool;
        VkCommandBuffer data;
        bool began;

    public:
        inline CommandBuffer() : device(), command_pool(), data(), began() {

        }

        inline CommandBuffer(VkDevice device, VkCommandPool command_pool, VkCommandBuffer data) {
            this->device = device;
            this->command_pool = command_pool;
            this->data = data;
            began = true;
        }

        inline ~CommandBuffer() {

        }

        bool Create(VkDevice device, VkCommandPool command_pool);
        void Destroy();

        bool BeginOneTimeSubmit();

        void CopyBuffer(Vulkan::Buffer& src_buffer, Vulkan::Buffer& dst_buffer,
            VkDeviceSize src_offset, VkDeviceSize dst_offset, VkDeviceSize size);
        void CopyBufferToImage(Vulkan::Buffer& src_buffer, VkDeviceSize src_buffer_offset,
            Vulkan::Image& dst_image, VkImageAspectFlags aspect_mask, uint32_t level, uint32_t layer,
            int32_t x_offset, int32_t y_offset, uint32_t width, uint32_t height);
        void CopyImageToBuffer(Vulkan::Image& src_image, Vulkan::Buffer& dst_buffer,
            VkDeviceSize dst_buffer_offset, VkImageAspectFlags aspect_mask, uint32_t level, uint32_t layer,
            int32_t x_offset, int32_t y_offset, uint32_t width, uint32_t height);
        bool End();
        bool Sumbit(VkQueue queue, VkFence fence = 0);

        static void AddBuffer(VkCommandBuffer command_buffer, Vulkan::Buffer buffer);
        static void AddImage(VkCommandBuffer command_buffer, Vulkan::Image image);
        static void FreeData(VkCommandBuffer command_buffer);

        inline operator VkCommandBuffer() const {
            return data;
        }

        inline operator const VkCommandBuffer* () const {
            return &data;
        }
    };
}
