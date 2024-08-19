/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "DescriptorPipeline.hpp"
#include "../../KKdLib/hash.hpp"

namespace Vulkan {
    DescriptorPipeline::DescriptorSetCollection::DescriptorSetCollection()
        : data(), used(), count(), frame(), hash() {
    }

    void DescriptorPipeline::DescriptorSetCollection::Destroy(VkDevice device, VkDescriptorPool descriptor_pool) {
        if (data[2]) {
            vkFreeDescriptorSets(device, descriptor_pool, 1, &data[2]);
            data[2] = 0;
        }

        if (data[1]) {
            vkFreeDescriptorSets(device, descriptor_pool, 1, &data[1]);
            data[1] = 0;
        }

        if (data[0]) {
            vkFreeDescriptorSets(device, descriptor_pool, 1, &data[0]);
            data[0] = 0;
        }
    }

    DescriptorPipeline::DescriptorPipeline(VkDevice device, VkDescriptorPool descriptor_pool,
        uint32_t sampler_count, uint32_t uniform_count, uint32_t storage_count,
        const VkDescriptorSetLayoutBinding* bindings) {

        this->device = device;
        this->descriptor_pool = descriptor_pool;

        descriptor_set_layouts[0] = 0;
        descriptor_set_layouts[1] = 0;
        descriptor_set_layouts[2] = 0;
        descriptor_set_layout_indices[0] = -1;
        descriptor_set_layout_indices[1] = -1;
        descriptor_set_layout_indices[2] = -1;
        pipeline_layout = 0;

        uint32_t set_layout_count = 0;
        if (sampler_count) {
            VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info;
            descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            descriptor_set_layout_create_info.pNext = 0;
            descriptor_set_layout_create_info.flags = 0;
            descriptor_set_layout_create_info.bindingCount = sampler_count;
            descriptor_set_layout_create_info.pBindings = bindings;

            vkCreateDescriptorSetLayout(device, &descriptor_set_layout_create_info,
                0, &descriptor_set_layouts[set_layout_count]);
            descriptor_set_layout_indices[0] = set_layout_count++;
        }

        if (uniform_count) {
            VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info;
            descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            descriptor_set_layout_create_info.pNext = 0;
            descriptor_set_layout_create_info.flags = 0;
            descriptor_set_layout_create_info.bindingCount = uniform_count;
            descriptor_set_layout_create_info.pBindings = bindings + sampler_count;

            vkCreateDescriptorSetLayout(device, &descriptor_set_layout_create_info,
                0, &descriptor_set_layouts[set_layout_count]);
            descriptor_set_layout_indices[1] = set_layout_count++;
        }

        if (storage_count) {
            VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info;
            descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            descriptor_set_layout_create_info.pNext = 0;
            descriptor_set_layout_create_info.flags = 0;
            descriptor_set_layout_create_info.bindingCount = storage_count;
            descriptor_set_layout_create_info.pBindings = bindings + sampler_count + uniform_count;

            vkCreateDescriptorSetLayout(device, &descriptor_set_layout_create_info,
                0, &descriptor_set_layouts[set_layout_count]);
            descriptor_set_layout_indices[2] = set_layout_count++;
        }

        if (set_layout_count) {
            VkPipelineLayoutCreateInfo pipeline_layout_create_info;
            pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipeline_layout_create_info.pNext = 0;
            pipeline_layout_create_info.flags = 0;
            pipeline_layout_create_info.setLayoutCount = set_layout_count;
            pipeline_layout_create_info.pSetLayouts = descriptor_set_layouts;
            pipeline_layout_create_info.pushConstantRangeCount = 0;
            pipeline_layout_create_info.pPushConstantRanges = 0;

            if (vkCreatePipelineLayout(device, &pipeline_layout_create_info, 0, &pipeline_layout) == VK_SUCCESS)
                return;
        }

        if (pipeline_layout) {
            vkDestroyPipelineLayout(device, pipeline_layout, 0);
            pipeline_layout = 0;
        }

        for (DescriptorPipeline::DescriptorSetCollection& i : descriptor_sets)
            i.Destroy(device, descriptor_pool);
        descriptor_sets.clear();

        if (descriptor_set_layouts[2]) {
            vkDestroyDescriptorSetLayout(device, descriptor_set_layouts[2], 0);
            descriptor_set_layouts[2] = 0;
        }

        if (descriptor_set_layouts[1]) {
            vkDestroyDescriptorSetLayout(device, descriptor_set_layouts[1], 0);
            descriptor_set_layouts[1] = 0;
        }

        if (descriptor_set_layouts[0]) {
            vkDestroyDescriptorSetLayout(device, descriptor_set_layouts[0], 0);
            descriptor_set_layouts[0] = 0;
        }

        descriptor_set_layout_indices[2] = -1;
        descriptor_set_layout_indices[1] = -1;
        descriptor_set_layout_indices[0] = -1;
        this->descriptor_pool = 0;
        this->device = 0;
    }

    DescriptorPipeline::~DescriptorPipeline() {
        if (pipeline_layout) {
            vkDestroyPipelineLayout(device, pipeline_layout, 0);
            pipeline_layout = 0;
        }

        for (DescriptorPipeline::DescriptorSetCollection& i : descriptor_sets)
            i.Destroy(device, descriptor_pool);
        descriptor_sets.clear();

        if (descriptor_set_layouts[2]) {
            vkDestroyDescriptorSetLayout(device, descriptor_set_layouts[2], 0);
            descriptor_set_layouts[2] = 0;
        }

        if (descriptor_set_layouts[1]) {
            vkDestroyDescriptorSetLayout(device, descriptor_set_layouts[1], 0);
            descriptor_set_layouts[1] = 0;
        }

        if (descriptor_set_layouts[0]) {
            vkDestroyDescriptorSetLayout(device, descriptor_set_layouts[0], 0);
            descriptor_set_layouts[0] = 0;
        }

        descriptor_set_layout_indices[2] = -1;
        descriptor_set_layout_indices[1] = -1;
        descriptor_set_layout_indices[0] = -1;
        descriptor_pool = 0;
        device = 0;
    }

    DescriptorPipeline::DescriptorSetCollection*
        DescriptorPipeline::GetDescriptorSetCollection(uint32_t frame, uint64_t hash) {
        for (DescriptorPipeline::DescriptorSetCollection& i : descriptor_sets)
            if (i.used && i.frame == frame && i.hash == hash)
                return &i;

        for (DescriptorPipeline::DescriptorSetCollection& i : descriptor_sets)
            if (!i.used && i.frame == frame) {
                i.hash = hash;
                return &i;
            }

        descriptor_sets.push_back({});
        DescriptorPipeline::DescriptorSetCollection* set_collection = &descriptor_sets.back();
        set_collection->count = 0;
        set_collection->frame = frame;
        set_collection->hash = hash;

        for (uint32_t i = 0; i < 3; i++)
            if (descriptor_set_layout_indices[i] != -1) {
                VkDescriptorSetAllocateInfo descriptor_set_allocate_info;
                descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
                descriptor_set_allocate_info.pNext = 0;
                descriptor_set_allocate_info.descriptorPool = descriptor_pool;
                descriptor_set_allocate_info.descriptorSetCount = 1;
                descriptor_set_allocate_info.pSetLayouts
                    = &descriptor_set_layouts[descriptor_set_layout_indices[i]];

                if (vkAllocateDescriptorSets(device, &descriptor_set_allocate_info,
                    &set_collection->data[set_collection->count++]) != VK_SUCCESS)
                    return 0;
            }
        return set_collection;
    }

    VkPipelineLayout DescriptorPipeline::GetPipelineLayout() {
        return pipeline_layout;
    }

    void DescriptorPipeline::ResetSetCollectionsState() {
        for (DescriptorPipeline::DescriptorSetCollection& i : descriptor_sets) {
            i.used = false;
            i.hash = 0;
        }
    }
}