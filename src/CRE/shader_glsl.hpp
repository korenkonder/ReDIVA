/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "../KKdLib/default.hpp"
#include "../KKdLib/farc.hpp"
#include "../KKdLib/mat.hpp"
#include "../KKdLib/vec.hpp"
#include <glad/glad.h>

#define SHADER_PARAM_NUM_PARAMS 12

struct shader_glsl_param {
    const char* name;
    char* frag;
    char* vert;
    char* param[SHADER_PARAM_NUM_PARAMS];
};

struct shader_glsl {
    std::string name;
    GLuint program;
    std::vector<std::pair<uint64_t, GLint>> uniform;
    std::vector<std::pair<uint64_t, GLint>> uniform_block;

    shader_glsl();
    virtual ~shader_glsl();

    GLint get_uniform_location(const char* name);
    void load(const char* vert_data, const char* frag_data,
        const char* geom_data, shader_glsl_param* param);
    void load_file(const char* vert_path, const char* frag_path,
        const char* geom_path, shader_glsl_param* param);
    void load_file(const wchar_t* vert_path, const wchar_t* frag_path,
        const wchar_t* geom_path, shader_glsl_param* param);
    void set(const char* name, bool value);
    void set(const char* name, int32_t value);
    void set(const char* name, float_t value);
    void set(const char* name, vec2i& value);
    void set(const char* name, int32_t x, int32_t y);
    void set(const char* name, vec2& value);
    void set(const char* name, float_t x, float_t y);
    void set(const char* name, vec3i& value);
    void set(const char* name, int32_t x, int32_t y, int32_t z);
    void set(const char* name, vec3& value);
    void set(const char* name, float_t x, float_t y, float_t z);
    void set(const char* name, vec4i& value);
    void set(const char* name, vec4iu& value);
    void set(const char* name, int32_t x, int32_t y, int32_t z, int32_t w);
    void set(const char* name, vec4& value);
    void set(const char* name, vec4u& value);
    void set(const char* name, float_t x, float_t y, float_t z, float_t w);
    void set(const char* name, bool transpose, mat3& value);
    void set(const char* name, bool transpose, mat4& value);
    void set(const char* name, bool transpose, mat4u& value);
    void set(const char* name, size_t count, int32_t* value);
    void set(const char* name, size_t count, float_t* value);
    void set(const char* name, size_t count, vec2i* value);
    void set(const char* name, size_t count, vec2* value);
    void set(const char* name, size_t count, vec3i* value);
    void set(const char* name, size_t count, vec3* value);
    void set(const char* name, size_t count, vec4i* value);
    void set(const char* name, size_t count, vec4iu* value);
    void set(const char* name, size_t count, vec4* value);
    void set(const char* name, size_t count, vec4u* value);
    void set(const char* name, size_t count, bool transpose, mat3* value);
    void set(const char* name, size_t count, bool transpose, mat4* value);
    void set(const char* name, size_t count, bool transpose, mat4u* value);
    void set_uniform_block_binding(GLchar* name, GLint binding);
    void unload();
    void use();
};
