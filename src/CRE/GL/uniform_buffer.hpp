/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../gl.hpp"

namespace GL {
    struct UniformBuffer {
    private:
        GLuint buffer;

    public:
        inline UniformBuffer() : buffer() {

        }

        inline UniformBuffer(GLuint buffer) : buffer(buffer) {

        }

        inline ~UniformBuffer() {

        }

        void Bind(int32_t index, bool force = false);
        void Create(size_t size);
        void Create(size_t size, const void* data, bool dynamic = false);
        void Destroy();
        void* MapMemory();
        void UnmapMemory();
        void WriteMemory(size_t offset, size_t size, const void* data);

        inline bool IsNull() {
            return !buffer;
        }

        inline bool NotNull() {
            return !!buffer;
        }

        template<typename T>
        inline void WriteMemory(T& data) {
            WriteMemory(0, sizeof(T), &data);
        }

        inline operator GLuint() const {
            return buffer;
        }
    };
}
