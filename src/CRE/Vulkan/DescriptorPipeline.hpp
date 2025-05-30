/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "shared.hpp"
#include <vector>

namespace Vulkan {
    class DescriptorPipeline {
    public:
        struct DescriptorSetCollection {
            VkDescriptorSet data[3];
            bool used;
            uint8_t count;
            uint32_t frame;
            uint64_t hash;

            DescriptorSetCollection();

            void Destroy(VkDevice device, VkDescriptorPool descriptor_pool);
        };

    protected:
        VkDevice device;
        VkDescriptorPool descriptor_pool;
        VkDescriptorSetLayout descriptor_set_layouts[3];
        std::vector<DescriptorSetCollection> descriptor_sets;
        VkPipelineLayout pipeline_layout;

    public:
        DescriptorPipeline(VkDevice device, VkDescriptorPool descriptor_pool,
            uint32_t sampler_count, uint32_t uniform_count, uint32_t storage_count,
            const VkDescriptorSetLayoutBinding* bindings,
            uint32_t push_constant_range_count, VkPushConstantRange* push_constant_ranges);
        ~DescriptorPipeline();

        DescriptorSetCollection* GetDescriptorSetCollection(uint32_t frame, uint64_t hash);
        VkPipelineLayout GetPipelineLayout();
        void ResetSetCollectionsState();

        DescriptorPipeline(const DescriptorPipeline&) = delete;
        DescriptorPipeline& operator=(const DescriptorPipeline&) = delete;
    };
}
