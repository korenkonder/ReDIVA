/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "shared.hpp"

namespace Vulkan {
    class DescriptorPool {
    protected:
        VkDevice device;
        VkDescriptorPool data;

    public:
        inline DescriptorPool() : device(), data() {

        }

        bool Create(VkDevice device, VkDescriptorPoolCreateFlags flags,
            uint32_t max_sets, uint32_t combined_image_sampler_descriptor_count,
            uint32_t uniform_buffer_descriptor_count, uint32_t storage_buffer_descriptor_count);
        void Destroy();

        inline operator VkDescriptorPool() const {
            return data;
        }
    };
}
