/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include <vulkan/vulkan.h>

namespace Vulkan {
    class ImageView {
    protected:
        VkDevice device;

    public:
        VkImageView data;

        ImageView();
        ~ImageView();

        bool Create(VkDevice device, VkImageViewCreateFlags flags,
            VkImage image, VkImageViewType view_type, VkFormat format,
            VkImageSubresourceRange& sub_resource_range);
        bool Create(VkDevice device, VkImageViewCreateFlags flags,
            VkImage image, VkImageViewType view_type, VkFormat format,
            VkComponentMapping& components, VkImageSubresourceRange& sub_resource_range);
        void Destroy();
    };
}
