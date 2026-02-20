/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "DescriptorPool.hpp"
#include "manager.hpp"

namespace Vulkan {
    bool DescriptorPool::Create(VkDevice device, VkDescriptorPoolCreateFlags flags,
        uint32_t max_sets, uint32_t combined_image_sampler_descriptor_count,
        uint32_t uniform_buffer_descriptor_count, uint32_t storage_buffer_descriptor_count) {
        Destroy();

        uint32_t pool_size_count = 0;
        VkDescriptorPoolSize pool_sizes[5] = {};
        if (combined_image_sampler_descriptor_count) {
            pool_sizes[pool_size_count].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            pool_sizes[pool_size_count].descriptorCount = combined_image_sampler_descriptor_count * max_sets;
            pool_size_count++;
        }

        if (uniform_buffer_descriptor_count) {
            pool_sizes[pool_size_count].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            pool_sizes[pool_size_count].descriptorCount = uniform_buffer_descriptor_count * max_sets;
            pool_size_count++;
        }

        if (storage_buffer_descriptor_count) {
            pool_sizes[pool_size_count].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
            pool_sizes[pool_size_count].descriptorCount = storage_buffer_descriptor_count * max_sets;
            pool_size_count++;
        }

        VkDescriptorPoolCreateInfo descriptor_pool_create_info;
        descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptor_pool_create_info.pNext = 0;
        descriptor_pool_create_info.flags = flags;
        descriptor_pool_create_info.maxSets = max_sets;
        descriptor_pool_create_info.poolSizeCount = pool_size_count;
        descriptor_pool_create_info.pPoolSizes = pool_sizes;

        this->device = device;
        if (vkCreateDescriptorPool(device, &descriptor_pool_create_info, 0, &data) == VK_SUCCESS)
            return true;

        this->device = 0;
        data = 0;
        return false;
    }

    void DescriptorPool::Destroy() {
        if (data) {
            vkDestroyDescriptorPool(device, data, 0);
            data = 0;
        }
        device = 0;
    }
}
