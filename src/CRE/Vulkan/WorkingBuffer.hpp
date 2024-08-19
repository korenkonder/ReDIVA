/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "Buffer.hpp"

namespace Vulkan {
    constexpr uint32_t WORKING_BUFFER_CACHE_SIZE = 64;

    class WorkingBuffer {
    public:
        std::pair<uint64_t, Buffer> buffer_cache[WORKING_BUFFER_CACHE_SIZE];
        uint32_t current_buffer;
        Buffer default_buffer;

        inline WorkingBuffer() : buffer_cache() {
            current_buffer = -1;
        }

        inline void AddBuffer(uint64_t hash, const Buffer& buffer) {
            if (current_buffer == -1)
                current_buffer = 0;
            else
                current_buffer = (current_buffer + 1) % WORKING_BUFFER_CACHE_SIZE;
            buffer_cache[current_buffer].first = hash;
            buffer_cache[current_buffer].second = buffer;
        }

        inline VkDeviceSize GetOffset() const {
            if (current_buffer != -1)
                return buffer_cache[current_buffer].second.GetOffset();
            return default_buffer.GetOffset();
        }

        inline VkDeviceSize GetSize() const {
            if (current_buffer != -1)
                return buffer_cache[current_buffer].second.GetSize();
            return default_buffer.GetSize();
        }

        inline bool FindBuffer(uint64_t hash, VkDeviceSize size) {
            if (current_buffer == -1)
                return false;

            for (uint32_t i = 0; i < WORKING_BUFFER_CACHE_SIZE; i++)
                if (buffer_cache[i].second && buffer_cache[i].first == hash
                    && buffer_cache[i].second.GetSize() == size) {
                    current_buffer = i;
                    return true;
                }
            return false;
        }

        inline void Reset() {
            for (uint32_t i = 0; i < WORKING_BUFFER_CACHE_SIZE; i++)
                buffer_cache[i] = {};
            current_buffer = -1;
        }

        inline void SetDefaultBuffer(const Buffer& buffer) {
            current_buffer = -1;
            default_buffer = buffer;
        }

        inline operator VkBuffer() const {
            if (current_buffer != -1)
                return buffer_cache[current_buffer].second;
            return default_buffer;
        }
    };
}
