/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "Image.hpp"
#include "CommandBuffer.hpp"

namespace Vulkan {
    bool Image::Create(VmaAllocator allocator, VkImageCreateFlags flags, uint32_t width, uint32_t height,
        uint32_t mip_levels, uint32_t array_layers, VkFormat format, VkImageTiling tiling,
        VkImageUsageFlags usage, VmaMemoryUsage memory_usage, VmaAllocationCreateFlags allocation_flags) {
        Destroy();

        VkImageCreateInfo image_create_info = {};
        image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_create_info.flags = flags;
        image_create_info.imageType = VK_IMAGE_TYPE_2D;
        image_create_info.extent.width = width;
        image_create_info.extent.height = height;
        image_create_info.extent.depth = 1;
        image_create_info.mipLevels = mip_levels;
        image_create_info.arrayLayers = array_layers;
        image_create_info.format = format;
        image_create_info.tiling = tiling;
        image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_create_info.usage = usage;
        image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocation_create_info = {};
        allocation_create_info.usage = memory_usage;
        allocation_create_info.flags = allocation_flags;

        if (vmaCreateImage(allocator, &image_create_info,
            &allocation_create_info, &data, &allocation, 0) != VK_SUCCESS) {
            this->allocator = 0;
            data = 0;
            image_layout = 0;
            allocation = 0;
            return false;
        }

        if (*(uint64_t*)&data == 0xf448ed0000001349)
            printf("");

        this->allocator = allocator;
        this->mip_levels = mip_levels;
        this->array_layers = array_layers;

        image_layout = force_malloc<VkImageLayout>((size_t)mip_levels * array_layers);
        for (uint32_t i = 0; i < mip_levels; i++)
            for (uint32_t j = 0; j < array_layers; j++)
                image_layout[array_layers * i + j] = VK_IMAGE_LAYOUT_UNDEFINED;
        return true;
    }

    void Image::Destroy() {
        free_def(image_layout);
        image_layout = 0;

        CommandBuffer::AddImage(Vulkan::current_command_buffer, *this);

        allocator = 0;
        data = 0;
        allocation = 0;
    }

    VkImageLayout Image::GetImageLayout(uint32_t level, uint32_t layer) {
        return image_layout[array_layers * level + layer];
    }

    void Image::SetImageLayout(VkImageLayout image_layout) {
        for (uint32_t i = 0; i < mip_levels; i++)
            for (uint32_t j = 0; j < array_layers; j++)
                this->image_layout[array_layers * i + j] = image_layout;
    }

    void Image::SetImageLayout(VkImageLayout image_layout, uint32_t level, uint32_t layer) {
        this->image_layout[array_layers * level + layer] = image_layout;
    }

    std::pair<VkAccessFlags, VkPipelineStageFlags> getVkPipelineStageFlagsVkAccessFlags(VkImageLayout image_layout) {
        switch (image_layout) {
        case VK_IMAGE_LAYOUT_UNDEFINED:
            return {
                0,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
            };
        case VK_IMAGE_LAYOUT_GENERAL:
            return {
                0,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
            };
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            return {
                VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
                | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
            };
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            return {
                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT
                | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
                | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT
            };
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
            return {
                VK_ACCESS_SHADER_READ_BIT
                | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
                VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
                | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
                | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
                | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT
            };
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            return {
                VK_ACCESS_SHADER_READ_BIT,
                VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
                | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
            };
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            return {
                VK_ACCESS_TRANSFER_READ_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT
            };
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            return {
                VK_ACCESS_TRANSFER_WRITE_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT
            };
        case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
            return {
                VK_ACCESS_TRANSFER_WRITE_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT
            };
        }
        return { 0, 0 };
    }

    bool Image::PipelineBarrier(VkCommandBuffer command_buffer, VkImage image,
        VkImageAspectFlags aspect_mask, uint32_t level_count, uint32_t layer_count,
        VkImageLayout old_layout, VkImageLayout new_layout) {
        if (old_layout == new_layout)
            return true;

        VkImageMemoryBarrier barrier;
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.pNext = 0;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = 0;
        barrier.oldLayout = old_layout;
        barrier.newLayout = new_layout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = aspect_mask;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = level_count;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = layer_count;

        std::pair<VkAccessFlags, VkPipelineStageFlags> src = getVkPipelineStageFlagsVkAccessFlags(old_layout);
        std::pair<VkAccessFlags, VkPipelineStageFlags> dst = getVkPipelineStageFlagsVkAccessFlags(new_layout);
        if (!src.second || !dst.second)
            return false;

        barrier.srcAccessMask = src.first;
        barrier.dstAccessMask = dst.first;

        Vulkan::end_render_pass(command_buffer);

        vkCmdPipelineBarrier(command_buffer, src.second, dst.second, 0, 0, 0, 0, 0, 1, &barrier);
        return true;
    }

    bool Image::PipelineBarrier(VkCommandBuffer command_buffer, Vulkan::Image& image,
        VkImageAspectFlags aspect_mask, uint32_t level_count, uint32_t layer_count, VkImageLayout new_layout) {
        VkImageLayout old_layout = image.GetImageLayout(0, 0);
        if (old_layout == new_layout)
            return true;

        VkImageMemoryBarrier barrier;
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.pNext = 0;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = 0;
        barrier.oldLayout = old_layout;
        barrier.newLayout = new_layout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = aspect_mask;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = level_count;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = layer_count;

        std::pair<VkAccessFlags, VkPipelineStageFlags> src = getVkPipelineStageFlagsVkAccessFlags(old_layout);
        std::pair<VkAccessFlags, VkPipelineStageFlags> dst = getVkPipelineStageFlagsVkAccessFlags(new_layout);
        if (!src.second || !dst.second)
            return false;

        barrier.srcAccessMask = src.first;
        barrier.dstAccessMask = dst.first;

        Vulkan::end_render_pass(command_buffer);

        vkCmdPipelineBarrier(command_buffer, src.second, dst.second, 0, 0, 0, 0, 0, 1, &barrier);
        image.SetImageLayout(new_layout);
        return true;
    }

    bool Image::PipelineBarrierSingle(VkCommandBuffer command_buffer, VkImage image,
        VkImageAspectFlags aspect_mask, uint32_t level, uint32_t layer,
        VkImageLayout old_layout, VkImageLayout new_layout) {
        if (old_layout == new_layout)
            return true;

        VkImageMemoryBarrier barrier;
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.pNext = 0;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = 0;
        barrier.oldLayout = old_layout;
        barrier.newLayout = new_layout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = aspect_mask;
        barrier.subresourceRange.baseMipLevel = level;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = layer;
        barrier.subresourceRange.layerCount = 1;

        std::pair<VkAccessFlags, VkPipelineStageFlags> src = getVkPipelineStageFlagsVkAccessFlags(old_layout);
        std::pair<VkAccessFlags, VkPipelineStageFlags> dst = getVkPipelineStageFlagsVkAccessFlags(new_layout);
        if (!src.second || !dst.second)
            return false;

        barrier.srcAccessMask = src.first;
        barrier.dstAccessMask = dst.first;

        Vulkan::end_render_pass(command_buffer);

        vkCmdPipelineBarrier(command_buffer, src.second, dst.second, 0, 0, 0, 0, 0, 1, &barrier);
        return true;
    }

    bool Image::PipelineBarrierSingle(VkCommandBuffer command_buffer, Vulkan::Image& image,
        VkImageAspectFlags aspect_mask, uint32_t level, uint32_t layer, VkImageLayout new_layout) {
        VkImageLayout old_layout = image.GetImageLayout(level, layer);
        if (old_layout == new_layout)
            return true;

        VkImageMemoryBarrier barrier;
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.pNext = 0;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = 0;
        barrier.oldLayout = old_layout;
        barrier.newLayout = new_layout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = aspect_mask;
        barrier.subresourceRange.baseMipLevel = level;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = layer;
        barrier.subresourceRange.layerCount = 1;

        std::pair<VkAccessFlags, VkPipelineStageFlags> src = getVkPipelineStageFlagsVkAccessFlags(old_layout);
        std::pair<VkAccessFlags, VkPipelineStageFlags> dst = getVkPipelineStageFlagsVkAccessFlags(new_layout);
        if (!src.second || !dst.second)
            return false;

        barrier.srcAccessMask = src.first;
        barrier.dstAccessMask = dst.first;

        Vulkan::end_render_pass(command_buffer);

        vkCmdPipelineBarrier(command_buffer, src.second, dst.second, 0, 0, 0, 0, 0, 1, &barrier);
        image.SetImageLayout(new_layout, level, layer);
        return true;
    }

}
