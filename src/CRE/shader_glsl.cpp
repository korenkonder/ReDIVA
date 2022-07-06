/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "shader_glsl.hpp"
#include "../KKdLib/hash.hpp"
#include "../KKdLib/str_utils.hpp"
#include "gl_state.hpp"
#include "static_var.hpp"

static const char* common_data_string =
"layout (binding = 0, std140) uniform common_data {\n"
"    vec4 res; //x=width, y=height, z=1/width, w=1/height\n"
"    mat4 vp;\n"
"    mat4 view;\n"
"    mat4 projection;\n"
"    vec3 view_pos;\n"
"} cmn_data;\n"
"\n"
"#define COMMON_DATA_RES (cmn_data.res)\n"
"#define COMMON_DATA_VP (cmn_data.vp)\n"
"#define COMMON_DATA_VIEW (cmn_data.view)\n"
"#define COMMON_DATA_PROJ (cmn_data.projection)\n"
"#define COMMON_DATA_VIEW_POS (cmn_data.view_pos)";

static GLuint program_log = 0;

static GLuint shader_compile(GLenum type, const char* data);
static GLint shader_get_uniform_block_index(GLint program, GLchar* name,
    std::vector<std::pair<uint64_t, GLint>>* uniform_block);
static GLint shader_get_uniform_location(GLint program, GLchar* name,
    std::vector<std::pair<uint64_t, GLint>>* uniform);
static char* shader_parse(char* data, const char* parse_string, const char* replace_string);
static char* shader_parse_define(char* data, shader_glsl_param* param);

shader_glsl::shader_glsl() : program() {

}

shader_glsl::~shader_glsl() {
    unload();
}

GLint shader_glsl::get_uniform_location(const char* name) {
    if (program < 1)
        return GL_INVALID_INDEX;

    return shader_get_uniform_location(program, (GLchar*)name, &uniform);
}

void shader_glsl::load(const char* vert, const char* frag,
    const char* geom, shader_glsl_param* param) {
    if ((!frag && !vert && !geom) || !param)
        return;

    uniform.clear();
    uniform_block.clear();

    char* frag_data = str_utils_copy(frag);
    char* vert_data = str_utils_copy(vert);
    char* geom_data = str_utils_copy(geom);

    frag_data = shader_parse_define(frag_data, param);
    vert_data = shader_parse_define(vert_data, param);
    geom_data = shader_parse_define(geom_data, param);

    vert_data = shader_parse(vert_data, "//COMMONDATA", common_data_string);
    frag_data = shader_parse(frag_data, "//COMMONDATA", common_data_string);
    geom_data = shader_parse(geom_data, "//COMMONDATA", common_data_string);

    GLuint frag_shad = shader_compile(GL_FRAGMENT_SHADER, frag_data);
    GLuint vert_shad = shader_compile(GL_VERTEX_SHADER, vert_data);
    GLuint geom_shad = shader_compile(GL_GEOMETRY_SHADER, geom_data);

    free(frag_data);
    free(vert_data);
    free(geom_data);

    program = glCreateProgram();
    if (frag_shad)
        glAttachShader(program, frag_shad);
    if (vert_shad)
        glAttachShader(program, vert_shad);
    if (geom_shad)
        glAttachShader(program, geom_shad);
    glLinkProgram(program);

    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar* info_log = force_malloc_s(GLchar, 0x10000);
        glGetProgramInfoLog(program, 0x10000, 0, info_log);
        printf("Program FBO Shader linking error: %s\n", param->name);
        printf(info_log);
        putchar('\n');
        free(info_log);
    }

    if (frag_shad)
        glDeleteShader(frag_shad);
    if (vert_shad)
        glDeleteShader(vert_shad);
    if (geom_shad)
        glDeleteShader(geom_shad);
}

void shader_glsl::load_file(const char* vert_path, const char* frag_path,
    const char* geom_path, shader_glsl_param* param) {
    if ((!vert_path && !frag_path && !geom_path) || !param)
        return;

    uniform.clear();
    uniform_block.clear();

    stream st;
    size_t l;

    char* vert = 0;
    char* frag = 0;
    char* geom = 0;

    st.open(vert_path, "rb");
    if (st.io.stream) {
        l = st.length;
        vert = force_malloc_s(char, l + 1);
        st.read(vert, l);
        vert[l] = 0;
    }
    st.close();

    st.open(frag_path, "rb");
    if (st.io.stream) {
        l = st.length;
        frag = force_malloc_s(char, l + 1);
        st.read(frag, l);
        frag[l] = 0;
    }
    st.close();

    st.open(geom_path, "rb");
    if (st.io.stream) {
        l = st.length;
        geom = force_malloc_s(char, l + 1);
        st.read(geom, l);
        geom[l] = 0;
    }
    st.close();

    load(vert, frag, geom, param);
    free(vert);
    free(frag);
}

void shader_glsl::load_file(const wchar_t* vert_path, const wchar_t* frag_path,
    const wchar_t* geom_path, shader_glsl_param* param) {
    if ((!vert_path && !frag_path && !geom_path) || !param)
        return;

    uniform.clear();
    uniform_block.clear();

    stream st;
    size_t l;

    char* vert = 0;
    char* frag = 0;
    char* geom = 0;

    st.open(vert_path, L"rb");
    if (st.io.stream) {
        l = st.length;
        vert = force_malloc_s(char, l + 1);
        st.read(vert, l);
        vert[l] = 0;
    }
    st.close();

    st.open(frag_path, L"rb");
    if (st.io.stream) {
        l = st.length;
        frag = force_malloc_s(char, l + 1);
        st.read(frag, l);
        frag[l] = 0;
    }
    st.close();

    st.open(geom_path, L"rb");
    if (st.io.stream) {
        l = st.length;
        geom = force_malloc_s(char, l + 1);
        st.read(geom, l);
        geom[l] = 0;
    }
    st.close();

    load(vert, frag, geom, param);
    free(vert);
    free(frag);
}

void shader_glsl::set(const char* name, bool value) {
    int32_t location = get_uniform_location(name);
    if (location != GL_INVALID_INDEX) {
        use();
        glUniform1i(location, value ? 1 : 0);
    }
}

void shader_glsl::set(const char* name, int32_t value) {
    int32_t location = get_uniform_location(name);
    if (location != GL_INVALID_INDEX) {
        use();
        glUniform1i(location, value);
    }
}

void shader_glsl::set(const char* name, float_t value) {
    int32_t location = get_uniform_location(name);
    if (location != GL_INVALID_INDEX) {
        use();
        glUniform1f(location, value);
    }
}

void shader_glsl::set(const char* name, vec2i& value) {
    int32_t location = get_uniform_location(name);
    if (location != GL_INVALID_INDEX) {
        use();
        glUniform2i(location, value.x, value.y);
    }
}

void shader_glsl::set(const char* name, int32_t x, int32_t y) {
    int32_t location = get_uniform_location(name);
    if (location != GL_INVALID_INDEX) {
        use();
        glUniform2i(location, x, y);
    }
}

void shader_glsl::set(const char* name, vec2& value) {
    int32_t location = get_uniform_location(name);
    if (location != GL_INVALID_INDEX) {
        use();
        glUniform2f(location, value.x, value.y);
    }
}

void shader_glsl::set(const char* name, float_t x, float_t y) {
    int32_t location = get_uniform_location(name);
    if (location != GL_INVALID_INDEX) {
        use();
        glUniform2f(location, x, y);
    }
}

void shader_glsl::set(const char* name, vec3i& value) {
    int32_t location = get_uniform_location(name);
    if (location != GL_INVALID_INDEX) {
        use();
        glUniform3i(location, value.x, value.y, value.z);
    }
}

void shader_glsl::set(const char* name, int32_t x, int32_t y, int32_t z) {
    int32_t location = get_uniform_location(name);
    if (location != GL_INVALID_INDEX) {
        use();
        glUniform3i(location, x, y, z);
    }
}

void shader_glsl::set(const char* name, vec3& value) {
    int32_t location = get_uniform_location(name);
    if (location != GL_INVALID_INDEX) {
        use();
        glUniform3f(location, value.x, value.y, value.z);
    }
}

void shader_glsl::set(const char* name, float_t x, float_t y, float_t z) {
    int32_t location = get_uniform_location(name);
    if (location != GL_INVALID_INDEX) {
        use();
        glUniform3f(location, x, y, z);
    }
}

void shader_glsl::set(const char* name, vec4i& value) {
    int32_t location = get_uniform_location(name);
    if (location != GL_INVALID_INDEX) {
        use();
        glUniform4i(location, value.x, value.y, value.z, value.w);
    }
}

void shader_glsl::set(const char* name, int32_t x, int32_t y, int32_t z, int32_t w) {
    int32_t location = get_uniform_location(name);
    if (location != GL_INVALID_INDEX) {
        use();
        glUniform4i(location, x, y, z, w);
    }
}

void shader_glsl::set(const char* name, vec4& value) {
    int32_t location = get_uniform_location(name);
    if (location != GL_INVALID_INDEX) {
        use();
        glUniform4f(location, value.x, value.y, value.z, value.w);
    }
}

void shader_glsl::set(const char* name, float_t x, float_t y, float_t z, float_t w) {
    int32_t location = get_uniform_location(name);
    if (location != GL_INVALID_INDEX) {
        use();
        glUniform4f(location, x, y, z, w);
    }
}

void shader_glsl::set(const char* name, bool transpose, mat3& value) {
    int32_t location = get_uniform_location(name);
    if (location != GL_INVALID_INDEX) {
        use();
        glUniformMatrix3fv(location, 1, transpose, (GLfloat*)&value);
    }
}

void shader_glsl::set(const char* name, bool transpose, mat4& value) {
    int32_t location = get_uniform_location(name);
    if (location != GL_INVALID_INDEX) {
        use();
        glUniformMatrix4fv(location, 1, transpose, (GLfloat*)&value);
    }
}

void shader_glsl::set(const char* name, size_t count, int32_t* value) {
    int32_t location = get_uniform_location(name);
    if (location != GL_INVALID_INDEX) {
        use();
        glUniform1iv(location, (GLsizei)count, value);
    }
}

void shader_glsl::set(const char* name, size_t count, float_t* value) {
    int32_t location = get_uniform_location(name);
    if (location != GL_INVALID_INDEX) {
        use();
        glUniform1fv(location, (GLsizei)count, value);
    }
}

void shader_glsl::set(const char* name, size_t count, vec2i* value) {
    int32_t location = get_uniform_location(name);
    if (location != GL_INVALID_INDEX) {
        use();
        glUniform2iv(location, (GLsizei)count, (GLint*)value);
    }
}

void shader_glsl::set(const char* name, size_t count, vec2* value) {
    int32_t location = get_uniform_location(name);
    if (location != GL_INVALID_INDEX) {
        use();
        glUniform2fv(location, (GLsizei)count, (GLfloat*)value);
    }
}

void shader_glsl::set(const char* name, size_t count, vec3i* value) {
    int32_t location = get_uniform_location(name);
    if (location != GL_INVALID_INDEX) {
        use();
        glUniform3iv(location, (GLsizei)count, (GLint*)value);
    }
}

void shader_glsl::set(const char* name, size_t count, vec3* value) {
    int32_t location = get_uniform_location(name);
    if (location != GL_INVALID_INDEX) {
        use();
        glUniform3fv(location, (GLsizei)count, (GLfloat*)value);
    }
}

void shader_glsl::set(const char* name, size_t count, vec4i* value) {
    int32_t location = get_uniform_location(name);
    if (location != GL_INVALID_INDEX) {
        use();
        glUniform4iv(location, (GLsizei)count, (GLint*)value);
    }
}

void shader_glsl::set(const char* name, size_t count, vec4* value) {
    int32_t location = get_uniform_location(name);
    if (location != GL_INVALID_INDEX) {
        use();
        glUniform4fv(location, (GLsizei)count, (GLfloat*)value);
    }
}

void shader_glsl::set(const char* name, size_t count, bool transpose, mat3* value) {
    int32_t location = get_uniform_location(name);
    if (location != GL_INVALID_INDEX) {
        use();
        glUniformMatrix3fv(location, (GLsizei)count, transpose, (GLfloat*)value);
    }
}

void shader_glsl::set(const char* name, size_t count, bool transpose, mat4* value) {
    int32_t location = get_uniform_location(name);
    if (location != GL_INVALID_INDEX) {
        use();
        glUniformMatrix4fv(location, (GLsizei)count, transpose, (GLfloat*)value);
    }
}

void shader_glsl::set_uniform_block_binding(GLchar* name, GLint binding) {
    GLint index = shader_get_uniform_block_index(program, name, &uniform_block);
    if (index != GL_INVALID_INDEX)
        glUniformBlockBinding(program, index, binding);
}

void shader_glsl::unload() {
    if (program) {
        glDeleteProgram(program);
        program = 0;
    }
}

void shader_glsl::use() {
    gl_state_use_program(program);
}

static GLuint shader_compile(GLenum type, const char* data) {
    if (!data)
        return 0;

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &data, 0);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar* info_log = force_malloc_s(GLchar, 0x10000);
        glGetShaderInfoLog(shader, 0x10000, 0, info_log);
        printf("Shader compile error: ");
        printf(info_log);
        putchar('\n');
        free(info_log);
    }
    return shader;
}

static GLint shader_get_uniform_block_index(GLint program, GLchar* name,
    std::vector<std::pair<uint64_t, GLint>>* uniform_block) {
    uint64_t hash = hash_utf8_fnv1a64m(name);
    for (std::pair<uint64_t, GLint>& i : *uniform_block)
        if (i.first == hash)
            return i.second;

    GLint index = glGetUniformBlockIndex(program, name);
    if (index == GL_INVALID_INDEX) {
        if (program_log != program) {
            printf("Program %d:\n", program);
            program_log = program;
        }
        printf("    Block Index for \"%s\" not found\n", name);
    }
    uniform_block->push_back({ hash, index });
    return index;
}

static GLint shader_get_uniform_location(GLint program, GLchar* name,
    std::vector<std::pair<uint64_t, GLint>>* uniform) {
    uint64_t hash = hash_utf8_fnv1a64m(name);
    for (std::pair<uint64_t, GLint>& i : *uniform)
        if (i.first == hash)
            return i.second;

    GLint location = glGetUniformLocation(program, name);
    if (location == GL_INVALID_INDEX) {
        if (program_log != program) {
            printf("Program %d:\n", program);
            program_log = program;
        }
        printf("    Location for \"%s\" not found\n", name);
    }
    uniform->push_back({ hash, location });
    return location;

}

static char* shader_parse(char* data, const char* parse_string, const char* replace_string) {
    if (!data)
        return data;

    char* def = strstr(data, parse_string);
    if (!def)
        return data;

    size_t len_a = def - data;
    def += utf8_length(parse_string);
    size_t len_b = utf8_length(replace_string);
    size_t len_c = utf8_length(def);
    size_t len = len_a + len_b + len_c;

    char* temp_data = force_malloc_s(char, len + 1);
    size_t pos = 0;
    memcpy(temp_data + pos, data, len_a);
    pos += len_a;
    memcpy(temp_data + pos, replace_string, len_b);
    pos += len_b;
    memcpy(temp_data + pos, def, len_c);
    free(data);
    return temp_data;
}

static char* shader_parse_define(char* data, shader_glsl_param* param) {
    if (!data || !param)
        return data;

    char* def = strstr(data, "//DEF\n");
    if (!def)
        return data;

    const size_t s = SHADER_PARAM_NUM_PARAMS;

    size_t temp_len[SHADER_PARAM_NUM_PARAMS];
    char temp[0x100];
    memset(temp, 0, 0x100);

    for (size_t i = 0; i < s; i++)
        if (param->param[i]) {
            sprintf_s(temp, sizeof(temp), "#define %s\n", param->param[i]);
            temp_len[i] = utf8_length(temp);
        }
        else
            temp_len[i] = 0;

    size_t len_a = def - data;
    def += 5;
    size_t len_b = utf8_length(def);
    size_t len = 0;
    for (size_t i = 0; i < s; i++)
        len += temp_len[i];
    if (!len)
        def++;
    len += len_a + len_b;

    char* temp_data = force_malloc_s(char, len + 1);
    size_t pos = 0;
    memcpy(temp_data + pos, data, len_a);
    pos += len_a;
    for (size_t i = 0; i < s; i++) {
        if (param->param[i]) {
            sprintf_s(temp, sizeof(temp), "#define %s\n", param->param[i]);
            memcpy(temp_data + pos, temp, temp_len[i]);
        }
        pos += temp_len[i];
    }
    memcpy(temp_data + pos, def, len_b);
    free(data);
    return temp_data;
}
