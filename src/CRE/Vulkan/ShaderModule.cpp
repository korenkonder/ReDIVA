/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "ShaderModule.hpp"

namespace Vulkan {
    ShaderModule::ShaderModule(VkDevice device, const void* data, size_t size) {
        VkShaderModuleCreateInfo shader_module_create_info;
        shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shader_module_create_info.pNext = 0;
        shader_module_create_info.flags = 0;
        shader_module_create_info.codeSize = size;
        shader_module_create_info.pCode = (const uint32_t*)data;

        this->device = device;
        if (vkCreateShaderModule(device, &shader_module_create_info, 0, &this->data) == VK_SUCCESS)
            return;

        this->device = 0;
        this->data = 0;
    }

    ShaderModule::~ShaderModule() {
        if (data) {
            vkDestroyShaderModule(device, data, 0);
            data = 0;
        }
        device = 0;
    }
}
