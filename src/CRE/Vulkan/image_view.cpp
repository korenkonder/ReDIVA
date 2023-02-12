/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "image_view.hpp"

namespace Vulkan {
    ImageView::ImageView() : device(), data() {

    }

    ImageView::~ImageView() {

    }

    bool ImageView::Create(VkDevice device, VkImageViewCreateFlags flags,
        VkImage image, VkImageViewType view_type, VkFormat format,
        VkImageSubresourceRange& sub_resource_range) {
        if (data)
            return true;

        VkImageViewCreateInfo image_view_create_info;
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.pNext = 0;
        image_view_create_info.flags = flags;
        image_view_create_info.image = image;
        image_view_create_info.viewType = view_type;
        image_view_create_info.format = format;
        image_view_create_info.components = {};
        image_view_create_info.subresourceRange = sub_resource_range;

        if (vkCreateImageView(device, &image_view_create_info, 0, &data) != VK_SUCCESS) {
            this->device = 0;
            data = 0;
            return false;
        }

        this->device = device;
        return true;
    }
    
    bool ImageView::Create(VkDevice device, VkImageViewCreateFlags flags,
        VkImage image, VkImageViewType view_type, VkFormat format,
        VkComponentMapping& components, VkImageSubresourceRange& sub_resource_range) {
        Destroy();

        VkImageViewCreateInfo image_view_create_info;
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.pNext = 0;
        image_view_create_info.flags = flags;
        image_view_create_info.image = image;
        image_view_create_info.viewType = view_type;
        image_view_create_info.format = format;
        image_view_create_info.components = components;
        image_view_create_info.subresourceRange = sub_resource_range;

        if (vkCreateImageView(device, &image_view_create_info, 0, &data) != VK_SUCCESS) {
            this->device = 0;
            data = 0;
            return false;
        }

        this->device = device;
        return true;
    }

    void ImageView::Destroy() {
        if (data) {
            vkDestroyImageView(device, data, 0);
            data = 0;
        }
        device = 0;
    }
}
