/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "shared.hpp"

namespace Vulkan {
    class Sampler {
    protected:
        VkDevice device;
        VkSampler data;

    public:
        Sampler(VkDevice device, VkSamplerCreateFlags flags, VkFilter mag_filter,
            VkFilter min_filter, VkSamplerMipmapMode mipmap_mode, float_t mip_lod_bias,
            VkSamplerAddressMode address_mode_u, VkSamplerAddressMode address_mode_v,
            VkSamplerAddressMode address_mode_w, float_t min_lod, float_t max_lod,
            float_t max_anisotropy, VkBorderColor border_color);
        ~Sampler();

        inline operator VkSampler() const {
            return data;
        }

        Sampler(const Sampler&) = delete;
        Sampler& operator=(const Sampler&) = delete;
    };
}
