/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "ImageView.hpp"
#include "Manager.hpp"

namespace Vulkan {
    bool ImageView::Create(VkDevice device, VkImageViewCreateFlags flags, VkImage image,
        VkImageViewType view_type, VkFormat format, VkImageAspectFlags aspect_mask,
        uint32_t base_mip_level, uint32_t level_count, uint32_t base_array_layer, uint32_t layer_count,
        const VkComponentMapping& components) {
        if (data)
            Destroy();

        VkImageViewCreateInfo image_view_create_info;
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.pNext = 0;
        image_view_create_info.flags = flags;
        image_view_create_info.image = image;
        image_view_create_info.viewType = view_type;
        image_view_create_info.format = format;
        image_view_create_info.components = components;
        image_view_create_info.subresourceRange.aspectMask = aspect_mask;
        image_view_create_info.subresourceRange.baseMipLevel = base_mip_level;
        image_view_create_info.subresourceRange.levelCount = level_count;
        image_view_create_info.subresourceRange.baseArrayLayer = base_array_layer;
        image_view_create_info.subresourceRange.layerCount = layer_count;

        this->device = device;
        if (vkCreateImageView(device, &image_view_create_info, 0, &data) == VK_SUCCESS)
            return true;

        this->device = 0;
        data = 0;
        return false;
    }

    void ImageView::Destroy() {
        if (data) {
            Vulkan::manager_free_image_view(device, data, 0);
            data = 0;
        }
        device = 0;
    }
}
