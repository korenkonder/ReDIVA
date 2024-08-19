/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "shared.hpp"

namespace Vulkan {
    class ShaderModule {
    protected:
        VkDevice device;
        VkShaderModule data;

    public:
        ShaderModule(VkDevice device, const void* data, size_t size);
        ~ShaderModule();

        inline operator VkShaderModule() const {
            return data;
        }
    };
}
