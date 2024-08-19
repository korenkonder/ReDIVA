/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include <Volk/volk.h>
#define VMA_VULKAN_VERSION 1000000
#include <vma/vk_mem_alloc.h>

namespace Vulkan {
    extern bool use;

    extern VkDevice current_device;

    extern VkQueue current_graphics_queue;
    extern VkQueue current_present_queue;

    extern VmaAllocator current_allocator;

    extern VkCommandPool current_command_pool;

    extern VkCommandBuffer current_command_buffer;

    extern VkDescriptorPool current_descriptor_pool;

    extern VkFramebuffer current_framebuffer;
    extern VkRenderPass current_render_pass;

    extern VkImage current_swapchain_image;

    extern const char* get_error_string(VkResult error_code);
    extern void end_render_pass(VkCommandBuffer command_buffer);
}

#define VK_CHECK_RESULT(f) { \
	VkResult res = (f); \
	if (res != VK_SUCCESS) { \
        const char* error_string = Vulkan::get_error_string(res); \
        printf("Fatal error: VkResult is \"%s\" in \"%s\" at line \"%s\"\n", error_string, __FILE__, __LINE__); \
		assert(res == VK_SUCCESS); \
	} \
}