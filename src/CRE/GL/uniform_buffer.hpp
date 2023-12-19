/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <glad/glad.h>
#include "../gl_state.hpp"

namespace GL {
    struct UniformBuffer {
    private:
        GLuint buffer;

    public:
        inline UniformBuffer() : buffer() {

        }

        inline ~UniformBuffer() {

        }

        inline void Bind(int32_t index, bool force = false) {
            gl_state_bind_uniform_buffer_base(index, buffer, force);
        }

        inline void Create(size_t size) {
            if (buffer)
                return;

            if (GLAD_GL_VERSION_4_5) {
                glCreateBuffers(1, &buffer);
                glNamedBufferStorage(buffer, (GLsizeiptr)size, 0, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
            }
            else {
                glGenBuffers(1, &buffer);
                gl_state_bind_uniform_buffer(buffer, true);
                if (GLAD_GL_VERSION_4_4)
                    glBufferStorage(GL_UNIFORM_BUFFER, (GLsizeiptr)size,
                        0, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
                else
                    glBufferData(GL_UNIFORM_BUFFER, (GLsizeiptr)size, 0, GL_DYNAMIC_DRAW);
            }
        }

        inline void Create(size_t size, const void* data, bool dynamic = false) {
            if (buffer)
                return;

            if (GLAD_GL_VERSION_4_5) {
                glCreateBuffers(1, &buffer);
                glNamedBufferStorage(buffer, (GLsizeiptr)size, data,
                    dynamic ? GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT : 0);
            }
            else {
                glGenBuffers(1, &buffer);
                gl_state_bind_uniform_buffer(buffer, true);
                if (GLAD_GL_VERSION_4_4)
                    glBufferStorage(GL_UNIFORM_BUFFER, (GLsizeiptr)size, data,
                        dynamic ? GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT : 0);
                else
                    glBufferData(GL_UNIFORM_BUFFER, (GLsizeiptr)size, data,
                        dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
            }
        }

        inline void Destroy() {
            if (buffer) {
                glDeleteBuffers(1, &buffer);
                buffer = 0;
            }
        }

        inline bool IsNull() {
            return !buffer;
        }

        inline void* MapMemory() {
            if (!buffer)
                return 0;

            void* data;
            if (GLAD_GL_VERSION_4_5)
                data = glMapNamedBuffer(buffer, GL_WRITE_ONLY);
            else {
                gl_state_bind_uniform_buffer(buffer);
                data = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
            }

            if (data)
                return data;

            if (GLAD_GL_VERSION_4_5)
                glUnmapNamedBuffer(buffer);
            else {
                glUnmapBuffer(GL_UNIFORM_BUFFER);
                gl_state_bind_uniform_buffer(0);
            }
            return 0;
        }

        inline bool NotNull() {
            return !!buffer;
        }

        inline void UnmapMemory() {
            if (!buffer)
                return;

            if (GLAD_GL_VERSION_4_5)
                glUnmapNamedBuffer(buffer);
            else {
                glUnmapBuffer(GL_UNIFORM_BUFFER);
                gl_state_bind_uniform_buffer(0);
            }
        }

        inline void WriteMemory(size_t offset, size_t size, const void* data) {
            if (!buffer)
                return;

            if (GLAD_GL_VERSION_4_5)
                glNamedBufferSubData(buffer, (GLsizeiptr)offset, (GLsizeiptr)size, data);
            else {
                gl_state_bind_uniform_buffer(buffer);
                glBufferSubData(GL_UNIFORM_BUFFER, (GLsizeiptr)offset, (GLsizeiptr)size, data);
            }
        }

        template<typename T>
        inline void WriteMemory(T& data) {
            if (!buffer)
                return;

            if (GLAD_GL_VERSION_4_5)
                glNamedBufferSubData(buffer, 0, sizeof(T), &data);
            else {
                gl_state_bind_uniform_buffer(buffer);
                glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(T), &data);
            }
        }
    };
}
