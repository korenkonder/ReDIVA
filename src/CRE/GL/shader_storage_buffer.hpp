/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once
#include <glad/glad.h>
#include "../gl_state.hpp"

namespace GL {
    struct ShaderStorageBuffer {
    private:
        GLuint buffer;

    public:
        inline ShaderStorageBuffer() : buffer() {

        }

        inline ~ShaderStorageBuffer() {

        }

        inline void Create(size_t size) {
            if (buffer)
                return;

            if (GLAD_GL_VERSION_4_5) {
                glCreateBuffers(1, &buffer);
                glNamedBufferStorage(buffer, size, 0, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
            }
            else {
                glGenBuffers(1, &buffer);
                gl_state_bind_shader_storage_buffer(buffer);
                if (GLAD_GL_VERSION_4_4)
                    glBufferStorage(GL_SHADER_STORAGE_BUFFER, (GLsizeiptr)size,
                        0, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
                else
                    glBufferData(GL_SHADER_STORAGE_BUFFER, (GLsizeiptr)size, 0, GL_DYNAMIC_DRAW);
            }

        }

        inline void Create(size_t size, const void* data) {
            if (buffer)
                return;

            if (GLAD_GL_VERSION_4_5) {
                glCreateBuffers(1, &buffer);
                glNamedBufferStorage(buffer, (GLsizeiptr)size, data, 0);
            }
            else {
                glGenBuffers(1, &buffer);
                gl_state_bind_shader_storage_buffer(buffer);
                if (GLAD_GL_VERSION_4_4)
                    glBufferStorage(GL_SHADER_STORAGE_BUFFER, (GLsizeiptr)size, data, 0);
                else
                    glBufferData(GL_SHADER_STORAGE_BUFFER, (GLsizeiptr)size, data, GL_STATIC_DRAW);
            }
        }

        inline void Bind(int32_t index) {
            gl_state_bind_shader_storage_buffer_base(index, buffer);
        }

        inline void Destroy() {
            if (buffer) {
                glDeleteBuffers(1, &buffer);
                buffer = 0;
            }
        }

        inline void* MapMemory() {
            if (!buffer)
                return 0;

            void* data;
            if (GLAD_GL_VERSION_4_5)
                data = glMapNamedBuffer(buffer, GL_WRITE_ONLY);
            else {
                gl_state_bind_array_buffer(buffer);
                data = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
            }

            if (data)
                return data;

            if (!GLAD_GL_VERSION_4_5)
                gl_state_bind_array_buffer(0);
            return 0;
        }

        inline void UnmapMemory() {
            if (!buffer)
                return;

            if (GLAD_GL_VERSION_4_5)
                glUnmapNamedBuffer(buffer);
            else {
                glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
                gl_state_bind_array_buffer(0);
            }
        }

        inline void WriteMemory(size_t offset, size_t size, const void* data) {
            if (!buffer)
                return;

            if (GLAD_GL_VERSION_4_5)
                glNamedBufferSubData(buffer, (GLsizeiptr)offset, (GLsizeiptr)size, data);
            else {
                gl_state_bind_shader_storage_buffer(buffer);
                glBufferSubData(GL_SHADER_STORAGE_BUFFER, (GLsizeiptr)offset, (GLsizeiptr)size, data);
            }
        }

        template<typename T>
        inline void WriteMemory(T& data) {
            if (!buffer)
                return;

            if (GLAD_GL_VERSION_4_5)
                glNamedBufferSubData(buffer, 0, sizeof(T), &data);
            else {
                gl_state_bind_shader_storage_buffer(buffer);
                glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(T), &data);
            }
        }
    };
}
