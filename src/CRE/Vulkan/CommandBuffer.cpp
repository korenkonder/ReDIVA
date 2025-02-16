/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "CommandBuffer.hpp"
#include <unordered_map>

namespace Vulkan {
    std::unordered_map<VkCommandBuffer, std::vector<Vulkan::Buffer>> buffers;
    std::unordered_map<VkCommandBuffer, std::vector<Vulkan::Image>> images;

    bool CommandBuffer::Create(VkDevice device, VkCommandPool command_pool) {
        Destroy();

        VkCommandBufferAllocateInfo alloc_info;
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.pNext = 0;
        alloc_info.commandPool = command_pool;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandBufferCount = 1;

        this->device = device;
        this->command_pool = command_pool;
        began = false;
        if (vkAllocateCommandBuffers(device, &alloc_info, &data) == VK_SUCCESS)
            return true;

        this->device = 0;
        this->command_pool = 0;
        data = 0;
        return false;
    }

    void CommandBuffer::Destroy() {
        if (began) {
            End();
            began = false;
        }

        if (data) {
            vkFreeCommandBuffers(device, command_pool, 1, &data);
            data = 0;
        }
        device = 0;
        command_pool = 0;
    }

    bool CommandBuffer::BeginOneTimeSubmit() {
        if (began)
            return true;

        VkCommandBufferBeginInfo begin_info;
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.pNext = 0;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        begin_info.pInheritanceInfo = 0;

        if (vkBeginCommandBuffer(data, &begin_info) == VK_SUCCESS) {
            began = true;
            return true;
        }

        began = false;
        return false;
    }

    void CommandBuffer::CopyBuffer(Vulkan::Buffer& src_buffer, Vulkan::Buffer& dst_buffer,
        VkDeviceSize src_offset, VkDeviceSize dst_offset, VkDeviceSize size) {
        if (began)
            Vulkan::Buffer::Copy(data, src_buffer, dst_buffer, src_offset, dst_offset, size);
    }

    void CommandBuffer::CopyBufferToImage(Vulkan::Buffer& src_buffer, VkDeviceSize src_buffer_offset,
        Vulkan::Image& dst_image, VkImageAspectFlags aspect_mask, uint32_t level, uint32_t layer,
        int32_t x_offset, int32_t y_offset, uint32_t width, uint32_t height) {
        if (began)
            Vulkan::Buffer::CopyToImage(data, src_buffer, src_buffer_offset, dst_image,
                aspect_mask, level, layer, x_offset, y_offset, width, height);
    }

    void CommandBuffer::CopyImageToBuffer(Vulkan::Image& src_image, Vulkan::Buffer& dst_buffer,
        VkDeviceSize dst_buffer_offset, VkImageAspectFlags aspect_mask, uint32_t level, uint32_t layer,
        int32_t x_offset, int32_t y_offset, uint32_t width, uint32_t height) {
        if (began) {
            const VkImageLayout old_layout = src_image.GetImageLayout(level, layer);
            const VkImageLayout new_layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

            Vulkan::Image::PipelineBarrierSingle(data, src_image, aspect_mask, 0, 0, new_layout);
            Vulkan::Buffer::CopyFromImage(data, src_image, dst_buffer, dst_buffer_offset,
                aspect_mask, level, layer, x_offset, y_offset, width, height);
            Vulkan::Image::PipelineBarrierSingle(data, src_image, aspect_mask, 0, 0, old_layout);
        }
    }

    bool CommandBuffer::End() {
        if (vkEndCommandBuffer(data) == VK_SUCCESS) {
            began = false;
            return true;
        }

        return false;
    }

    bool CommandBuffer::Sumbit(VkQueue queue, VkFence fence) {
        if (began) {
            End();
            began = false;
        }

        VkSubmitInfo submit_info = {};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &data;

        bool ret = vkQueueSubmit(queue, 1, &submit_info, fence) == VK_SUCCESS;
        if (fence)
            vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
        else
            vkQueueWaitIdle(queue);

        CommandBuffer::FreeData(data);
        return ret;
    }

    void CommandBuffer::AddBuffer(VkCommandBuffer command_buffer, Vulkan::Buffer buffer) {
        if (!buffer.allocation)
            return;

        auto elem = buffers.find(command_buffer);
        if (elem != buffers.end()) {
            elem->second.push_back(buffer);
            return;
        }

        buffers.insert({ command_buffer, {} }).first->second.push_back(buffer);
    }

    void CommandBuffer::AddImage(VkCommandBuffer command_buffer, Vulkan::Image image) {
        if (!image.allocation)
            return;

        auto elem = images.find(command_buffer);
        if (elem != images.end()) {
            elem->second.push_back(image);
            return;
        }

        images.insert({ command_buffer, {} }).first->second.push_back(image);
    }

    void CommandBuffer::FreeData(VkCommandBuffer command_buffer) {
        auto elem_buffers = buffers.find(command_buffer);
        if (elem_buffers != buffers.end()) {
            for (Vulkan::Buffer& i : elem_buffers->second)
                vmaDestroyBuffer(i.allocator, i.data, i.allocation);
            buffers.erase(elem_buffers);
        }

        auto elem_images = images.find(command_buffer);
        if (elem_images != images.end()) {
            for (Vulkan::Image& i : elem_images->second)
                vmaDestroyImage(i.allocator, i.data, i.allocation);
            images.erase(elem_images);
        }
    }
}
