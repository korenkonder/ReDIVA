/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "Query.hpp"

namespace Vulkan {
    bool Query::Create(VkDevice device, VkQueryPoolCreateFlags flags, VkQueryType query_type) {
        VkQueryPoolCreateInfo query_pool_create_info;
        query_pool_create_info.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
        query_pool_create_info.pNext = 0;
        query_pool_create_info.flags = flags;
        query_pool_create_info.queryType = query_type;
        query_pool_create_info.queryCount = 1;
        query_pool_create_info.pipelineStatistics = 0;

        this->device = device;
        began = false;
        needs_reset = true;
        if (vkCreateQueryPool(device, &query_pool_create_info, 0, &data) == VK_SUCCESS)
            return true;

        this->device = 0;
        data = 0;
        needs_reset = false;
        return false;
    }

    void Query::Destroy() {
        if (data) {
            vkDestroyQueryPool(device, data, 0);
            data = 0;
        }
        device = 0;
        began = false;
        needs_reset = false;
    }

    void Query::Begin(VkCommandBuffer command_buffer) {
        if (!began) {
            vkCmdBeginQuery(command_buffer, data, 0, 0);
            began = true;
        }
    }

    void Query::End(VkCommandBuffer command_buffer) {
        if (began) {
            vkCmdEndQuery(command_buffer, data, 0);
            began = false;
            needs_reset = true;
        }
    }

    bool Query::GetBegan() {
        return began;
    }

    void Query::GetResult(size_t size, void* data, VkQueryResultFlags flags) {
        vkGetQueryPoolResults(device, this->data, 0, 1, size, data, size, flags);
    }

    void Query::Reset(VkCommandBuffer command_buffer) {
        if (needs_reset) {
            vkCmdResetQueryPool(command_buffer, data, 0, 1);
            needs_reset = false;
        }
    }
}
