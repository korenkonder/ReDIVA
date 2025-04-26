/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "Sampler.hpp"

namespace Vulkan {
    Sampler::Sampler(VkDevice device, VkSamplerCreateFlags flags, VkFilter mag_filter,
        VkFilter min_filter, VkSamplerMipmapMode mipmap_mode, float_t mip_lod_bias,
        VkSamplerAddressMode address_mode_u, VkSamplerAddressMode address_mode_v,
        VkSamplerAddressMode address_mode_w, float_t min_lod, float_t max_lod,
        float_t max_anisotropy, VkBorderColor border_color) {
        VkSamplerCreateInfo sampler_create_info = {};
        sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler_create_info.pNext = 0;
        sampler_create_info.flags = flags;
        sampler_create_info.magFilter = mag_filter;
        sampler_create_info.minFilter = min_filter;
        sampler_create_info.mipmapMode = mipmap_mode;
        sampler_create_info.addressModeU = address_mode_u;
        sampler_create_info.addressModeV = address_mode_v;
        sampler_create_info.addressModeW = address_mode_w;
        sampler_create_info.mipLodBias = mip_lod_bias;
        sampler_create_info.anisotropyEnable = max_anisotropy > 1.0f;
        sampler_create_info.maxAnisotropy = min_def(max_anisotropy, 16.0f);
        sampler_create_info.compareEnable = VK_FALSE;
        sampler_create_info.compareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        sampler_create_info.minLod = min_lod;
        sampler_create_info.maxLod = max_lod;
        sampler_create_info.borderColor = border_color;
        sampler_create_info.unnormalizedCoordinates = VK_FALSE;

        this->device = device;
        if (vkCreateSampler(device, &sampler_create_info, 0, &data) == VK_SUCCESS)
            return;

        this->device = 0;
        data = 0;
    }

    Sampler::~Sampler() {
        if (data) {
            vkDestroySampler(device, data, 0);
            data = 0;
        }
        device = 0;
    }
}
