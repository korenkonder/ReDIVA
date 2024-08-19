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
        typedef std::pair<uint64_t, Buffer> Pair;
        Pair buffer_cache[WORKING_BUFFER_CACHE_SIZE];

        inline WorkingBuffer() : buffer_cache() {
        }

        inline void AddBuffer(uint64_t hash, const Buffer& buffer) {
            memmove(&buffer_cache[1], &buffer_cache[0],
                sizeof(Pair) * (WORKING_BUFFER_CACHE_SIZE - 1));
            buffer_cache[0].first = hash;
            buffer_cache[0].second = buffer;
        }

        inline VkDeviceSize GetOffset() const {
            return buffer_cache[0].second.GetOffset();
        }

        inline VkDeviceSize GetSize() const {
            return buffer_cache[0].second.GetSize();
        }

        inline bool FindBuffer(uint64_t hash, VkDeviceSize size) {
            for (uint32_t i = 0; i < WORKING_BUFFER_CACHE_SIZE; i++)
                if (buffer_cache[i].first && buffer_cache[i].second
                    && buffer_cache[i].first == hash
                    && buffer_cache[i].second.GetSize() == size) {
                    if (!i)
                        return true;

                    Pair temp = buffer_cache[i];
                    memmove(&buffer_cache[1], &buffer_cache[0], sizeof(Pair) * i);
                    buffer_cache[0] = temp;
                    return true;
                }
            return false;
        }

        inline void Reset() {
            for (uint32_t i = 0; i < WORKING_BUFFER_CACHE_SIZE; i++)
                buffer_cache[i] = {};
        }

        inline void SetBuffer(const Buffer& buffer) {
            buffer_cache[0].first = 0;
            buffer_cache[0].second = buffer;
        }

        inline operator VkBuffer() const {
            return buffer_cache[0].second;
        }
    };
}
