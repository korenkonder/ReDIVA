/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "Buffer.hpp"
#include "CommandBuffer.hpp"

namespace Vulkan {
    bool Buffer::Create(VmaAllocator allocator, VkDeviceSize size,
        VkBufferUsageFlags usage, VkSharingMode sharing_mode,
        VmaMemoryUsage memory_usage, VmaAllocationCreateFlags flags) {
        Destroy();

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
            offset = 0;
            this->size = 0;
            allocation = 0;
            return false;
        }

        this->allocator = allocator;
        offset = 0;
        this->size = size;
        return true;
    }

    void Buffer::Destroy() {
        if (allocator)
            CommandBuffer::AddBuffer(Vulkan::current_command_buffer, *this);

        allocator = 0;
        size = 0;
        offset = 0;
        data = 0;
        allocation = 0;
    }

    void Buffer::Copy(VkCommandBuffer command_buffer, VkBuffer src_buffer, VkBuffer dst_buffer,
        VkDeviceSize src_offset, VkDeviceSize dst_offset, VkDeviceSize size) {
        Vulkan::end_render_pass(command_buffer);

        VkBufferCopy copy_region = {};
        copy_region.srcOffset = src_offset;
        copy_region.dstOffset = dst_offset;
        copy_region.size = size;
        vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);
    }

    void Buffer::CopyToImage(VkCommandBuffer command_buffer,
        VkBuffer src_buffer, VkDeviceSize src_buffer_offset, VkImage dst_image,
        VkImageAspectFlags aspect_mask, uint32_t level, uint32_t layer,
        int32_t x_offset, int32_t y_offset, uint32_t width, uint32_t height) {
        Vulkan::end_render_pass(command_buffer);

        VkBufferImageCopy region = {};
        region.bufferOffset = src_buffer_offset;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = aspect_mask;
        region.imageSubresource.mipLevel = level;
        region.imageSubresource.baseArrayLayer = layer;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = { x_offset, y_offset, 0 };
        region.imageExtent = { width, height, 1 };

        vkCmdCopyBufferToImage(command_buffer, src_buffer, dst_image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    }

    void Buffer::CopyFromImage(VkCommandBuffer command_buffer,
        VkImage src_image, VkBuffer dst_buffer, VkDeviceSize dst_buffer_offset,
        VkImageAspectFlags aspect_mask, uint32_t level, uint32_t layer,
        int32_t x_offset, int32_t y_offset, uint32_t width, uint32_t height) {
        Vulkan::end_render_pass(command_buffer);

        VkBufferImageCopy region = {};
        region.bufferOffset = dst_buffer_offset;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = aspect_mask;
        region.imageSubresource.mipLevel = level;
        region.imageSubresource.baseArrayLayer = layer;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = { x_offset, y_offset, 0 };
        region.imageExtent = { width, height, 1 };

        vkCmdCopyImageToBuffer(command_buffer, src_image,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst_buffer, 1, &region);
    }
}
