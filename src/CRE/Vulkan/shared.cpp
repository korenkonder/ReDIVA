/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#define VOLK_IMPLEMENTATION
#define VMA_IMPLEMENTATION
#include "shared.hpp"
#include "gl_wrap.hpp"

namespace Vulkan {
    bool use;
    bool use_wide_lines;

    VkDevice current_device;

    VkQueue current_graphics_queue;
    VkQueue current_present_queue;

    VmaAllocator current_allocator;

    VkCommandPool current_command_pool;

    VkCommandBuffer current_command_buffer;

    VkDescriptorPool current_descriptor_pool;

    VkFramebuffer current_framebuffer;
    VkRenderPass current_render_pass;

    VkImage current_swapchain_image;

    const char* get_error_string(VkResult error_code) {
        switch (error_code) {
#define ERROR_STRING(r) case VK_##r: return "VK_"#r
            ERROR_STRING(NOT_READY);
            ERROR_STRING(TIMEOUT);
            ERROR_STRING(EVENT_SET);
            ERROR_STRING(EVENT_RESET);
            ERROR_STRING(INCOMPLETE);
            ERROR_STRING(ERROR_OUT_OF_HOST_MEMORY);
            ERROR_STRING(ERROR_OUT_OF_DEVICE_MEMORY);
            ERROR_STRING(ERROR_INITIALIZATION_FAILED);
            ERROR_STRING(ERROR_DEVICE_LOST);
            ERROR_STRING(ERROR_MEMORY_MAP_FAILED);
            ERROR_STRING(ERROR_LAYER_NOT_PRESENT);
            ERROR_STRING(ERROR_EXTENSION_NOT_PRESENT);
            ERROR_STRING(ERROR_FEATURE_NOT_PRESENT);
            ERROR_STRING(ERROR_INCOMPATIBLE_DRIVER);
            ERROR_STRING(ERROR_TOO_MANY_OBJECTS);
            ERROR_STRING(ERROR_FORMAT_NOT_SUPPORTED);
            ERROR_STRING(ERROR_SURFACE_LOST_KHR);
            ERROR_STRING(ERROR_NATIVE_WINDOW_IN_USE_KHR);
            ERROR_STRING(SUBOPTIMAL_KHR);
            ERROR_STRING(ERROR_OUT_OF_DATE_KHR);
            ERROR_STRING(ERROR_INCOMPATIBLE_DISPLAY_KHR);
            ERROR_STRING(ERROR_VALIDATION_FAILED_EXT);
            ERROR_STRING(ERROR_INVALID_SHADER_NV);
            ERROR_STRING(ERROR_INCOMPATIBLE_SHADER_BINARY_EXT);
#undef ERROR_STRING
        default:
            return "VK_UNKNOWN_ERROR";
        }
    }

    void end_render_pass(VkCommandBuffer command_buffer) {
        GLuint query = Vulkan::gl_wrap_manager_get_query_samples_passed();
        if (query && Vulkan::gl_query::get(query)->query.GetBegan())
            glEndQuery(GL_SAMPLES_PASSED);

        if (Vulkan::current_render_pass) {
            vkCmdEndRenderPass(command_buffer);
            Vulkan::current_render_pass = 0;
        }

        Vulkan::current_framebuffer = 0;
    }
}
