/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "shared.hpp"

namespace Vulkan {
    class Query {
    protected:
        VkDevice device;
        VkQueryPool data;
        bool began;
        bool needs_reset;

    public:
        inline Query() : device(), data(), began(), needs_reset() {

        }

        bool Create(VkDevice device, VkQueryPoolCreateFlags flags, VkQueryType query_type);
        void Destroy();

        void Begin(VkCommandBuffer command_buffer);
        void End(VkCommandBuffer command_buffer);
        bool GetBegan();
        void GetResult(size_t size, void* data, VkQueryResultFlags flags);
        void Reset(VkCommandBuffer command_buffer);
    };
}
