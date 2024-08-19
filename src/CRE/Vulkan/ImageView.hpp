/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "shared.hpp"

namespace Vulkan {
    class ImageView {
    protected:
        VkDevice device;
        VkImageView data;

    public:
        inline ImageView() : device(), data() {

        }

        bool Create(VkDevice device, VkImageViewCreateFlags flags, VkImage image,
            VkImageViewType view_type, VkFormat format, VkImageAspectFlags aspect_mask,
            uint32_t base_mip_level, uint32_t level_count, uint32_t base_array_layer, uint32_t layer_count,
            const VkComponentMapping& components = {});
        void Destroy();

        inline operator VkImageView() const {
            return data;
        }
    };
}
