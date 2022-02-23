/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "shader_glsl.h"
#include "../KKdLib/hash.h"
#include "../KKdLib/str_utils.h"
#include "gl_state.h"
#include "static_var.h"

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
            snprintf(temp, sizeof(temp), "#define %s\n", param->param[i]);
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
            snprintf(temp, sizeof(temp), "#define %s\n", param->param[i]);
            memcpy(temp_data + pos, temp, temp_len[i]);
        }
        pos += temp_len[i];
    }
    memcpy(temp_data + pos, def, len_b);
    free(data);
    return temp_data;
}

static char* shader_parse(char* data, char* parse_string, const char* replace_string) {
    if (!data)
        return data;

    char* def = strstr(data, parse_string);
    if (!def)
        return data;

    size_t len_a = def - data;
    def += utf8_length(parse_string);
    size_t len_b = utf8_length((char*)replace_string);
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

inline static char* shader_parse(char* data, const char* parse_string, const char* replace_string) {
    return shader_parse(data, (char*)parse_string, replace_string);
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

static GLint shader_get_uniform_location(GLint program, GLchar* name,
    vector_old_uint64_t* uniform_name_buf, vector_old_int32_t* uniform_location_buf) {
    uint64_t hash = hash_utf8_fnv1a64m(name, false);
    for (uint64_t* i = uniform_name_buf->begin; i != uniform_name_buf->end; i++)
        if (*i == hash)
            return uniform_location_buf->begin[i - uniform_name_buf->begin];

    GLint location = glGetUniformLocation(program, name);
    if (location == GL_INVALID_INDEX) {
        if (program_log != program) {
            printf("Program %d:\n", program);
            program_log = program;
        }
        printf("    Location for \"%s\" not found\n", name);
    }
    vector_old_uint64_t_push_back(uniform_name_buf, &hash);
    vector_old_int32_t_push_back(uniform_location_buf, &location);
    return location;

}

static GLint shader_get_uniform_block_index(GLint program, GLchar* name,
    vector_old_uint64_t* uniform_block_name_buf, vector_old_int32_t* uniform_block_index_buf) {
    uint64_t hash = hash_utf8_fnv1a64m(name, false);
    for (uint64_t* i = uniform_block_name_buf->begin; i != uniform_block_name_buf->end; i++)
        if (*i == hash)
            return uniform_block_index_buf->begin[i - uniform_block_name_buf->begin];

    GLint index = glGetUniformBlockIndex(program, name);
    if (index == GL_INVALID_INDEX) {
        if (program_log != program) {
            printf("Program %d:\n", program);
            program_log = program;
        }
        printf("    Block Index for \"%s\" not found\n", name);
    }
    vector_old_uint64_t_push_back(uniform_block_name_buf, &hash);
    vector_old_int32_t_push_back(uniform_block_index_buf, &index);
    return index;
}

void shader_glsl_load(shader_glsl* s, farc* f, shader_glsl_param* param) {
    if (!s || !f || !param)
        return;

    if (s->program)
        glDeleteProgram(s->program);

    vector_old_uint64_t_clear(&s->uniform_name_buf, 0);
    vector_old_int32_t_clear(&s->uniform_location_buf, 0);
    vector_old_uint64_t_clear(&s->uniform_block_name_buf, 0);
    vector_old_int32_t_clear(&s->uniform_block_index_buf, 0);

    char temp[0x1000];
    temp[0] = 0;
    strcat_s(temp, sizeof(temp), param->name);
    for (size_t i = 0; i < SHADER_PARAM_NUM_PARAMS; i++)
        if (param->param[i]) {
            strcat_s(temp, sizeof(temp), "#");
            strcat_s(temp, sizeof(temp), param->param[i]);
        }
        else
            break;
    string_init(&s->name, temp);

    char* temp_frag = str_utils_add(param->frag, ".frag");
    char* temp_vert = str_utils_add(param->vert, ".vert");
    farc_file* frag = f->read_file(temp_frag);
    farc_file* vert = f->read_file(temp_vert);
    size_t frag_length = !frag ? 0 : frag->size;
    size_t vert_length = !vert ? 0 : vert->size;
    char* frag_data = !frag ? 0 : force_malloc_s(char, frag_length + 1);
    char* vert_data = !vert ? 0 : force_malloc_s(char, vert_length + 1);

    if (frag) {
        memcpy(frag_data, frag->data, frag_length);
        frag_data[frag_length] = 0;
    }

    if (vert) {
        memcpy(vert_data, vert->data, vert_length);
        vert_data[vert_length] = 0;
    }

    free(temp_frag);
    free(temp_vert);

    frag_data = shader_parse_define(frag_data, param);
    vert_data = shader_parse_define(vert_data, param);

    vert_data = shader_parse(vert_data, "//COMMONDATA", common_data_string);
    frag_data = shader_parse(frag_data, "//COMMONDATA", common_data_string);

    GLuint frag_shad = shader_compile(GL_FRAGMENT_SHADER, frag_data);
    GLuint vert_shad = shader_compile(GL_VERTEX_SHADER, vert_data);

    s->program = glCreateProgram();
    if (frag_shad)
        glAttachShader(s->program, frag_shad);
    if (vert_shad)
        glAttachShader(s->program, vert_shad);
    glLinkProgram(s->program);

    GLint success = 0;
    glGetProgramiv(s->program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar* info_log = force_malloc_s(GLchar, 0x10000);
        glGetProgramInfoLog(s->program, 0x10000, 0, info_log);
        printf("Program FBO Shader linking error: %s\n", param->name);
        printf(info_log);
        putchar('\n');
        free(info_log);
    }

    free(frag_data);
    free(vert_data);

    if (frag_shad)
        glDeleteShader(frag_shad);
    if (vert_shad)
        glDeleteShader(vert_shad);
}

void shader_glsl_load_file(shader_glsl* s, char* vert_path,
    char* frag_path, char* geom_path, shader_glsl_param* param) {
    if (!s || (!vert_path && !frag_path && !geom_path) || !param)
        return;

    vector_old_uint64_t_clear(&s->uniform_name_buf, 0);
    vector_old_int32_t_clear(&s->uniform_location_buf, 0);
    vector_old_uint64_t_clear(&s->uniform_block_name_buf, 0);
    vector_old_int32_t_clear(&s->uniform_block_index_buf, 0);

    stream st;
    size_t l;

    char* vert = 0;
    char* frag = 0;
    char* geom = 0;

    io_open(&st, vert_path, "rb");
    if (st.io.stream) {
        l = st.length;
        vert = force_malloc_s(char, l + 1);
        io_read(&st, vert, l);
        vert[l] = 0;
    }
    io_free(&st);

    io_open(&st, frag_path, "rb");
    if (st.io.stream) {
        l = st.length;
        frag = force_malloc_s(char, l + 1);
        io_read(&st, frag, l);
        frag[l] = 0;
    }
    io_free(&st);

    io_open(&st, geom_path, "rb");
    if (st.io.stream) {
        l = st.length;
        geom = force_malloc_s(char, l + 1);
        io_read(&st, geom, l);
        geom[l] = 0;
    }
    io_free(&st);

    shader_glsl_load_string(s, vert, frag, geom, param);
    free(vert);
    free(frag);
}

void shader_glsl_wload_file(shader_glsl* s, wchar_t* vert_path,
    wchar_t* frag_path, wchar_t* geom_path, shader_glsl_param* param) {
    if (!s || (!vert_path && !frag_path && !geom_path) || !param)
        return;

    vector_old_uint64_t_clear(&s->uniform_name_buf, 0);
    vector_old_int32_t_clear(&s->uniform_location_buf, 0);
    vector_old_uint64_t_clear(&s->uniform_block_name_buf, 0);
    vector_old_int32_t_clear(&s->uniform_block_index_buf, 0);

    stream st;
    size_t l;

    char* vert = 0;
    char* frag = 0;
    char* geom = 0;

    io_wopen(&st, vert_path, L"rb");
    if (st.io.stream) {
        l = st.length;
        vert = force_malloc_s(char, l + 1);
        io_read(&st, vert, l);
        vert[l] = 0;
    }
    io_free(&st);

    io_wopen(&st, frag_path, L"rb");
    if (st.io.stream) {
        l = st.length;
        frag = force_malloc_s(char, l + 1);
        io_read(&st, frag, l);
        frag[l] = 0;
    }
    io_free(&st);

    io_wopen(&st, geom_path, L"rb");
    if (st.io.stream) {
        l = st.length;
        geom = force_malloc_s(char, l + 1);
        io_read(&st, geom, l);
        geom[l] = 0;
    }
    io_free(&st);

    shader_glsl_load_string(s, vert, frag, geom, param);
    free(vert);
    free(frag);
}

void shader_glsl_load_string(shader_glsl* s, char* vert,
    char* frag, char* geom, shader_glsl_param* param) {
    if (!s || (!frag && !vert && !geom) || !param)
        return;

    vector_old_uint64_t_clear(&s->uniform_name_buf, 0);
    vector_old_int32_t_clear(&s->uniform_location_buf, 0);
    vector_old_uint64_t_clear(&s->uniform_block_name_buf, 0);
    vector_old_int32_t_clear(&s->uniform_block_index_buf, 0);

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

    s->program = glCreateProgram();
    if (frag_shad)
        glAttachShader(s->program, frag_shad);
    if (vert_shad)
        glAttachShader(s->program, vert_shad);
    if (geom_shad)
        glAttachShader(s->program, geom_shad);
    glLinkProgram(s->program);

    GLint success = 0;
    glGetProgramiv(s->program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar* info_log = force_malloc_s(GLchar, 0x10000);
        glGetProgramInfoLog(s->program, 0x10000, 0, info_log);
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

inline void shader_glsl_use(shader_glsl* s) {
    gl_state_use_program(s ? s->program : 0);
}

GLint shader_glsl_get_uniform_location(shader_glsl* s, GLchar* name) {
    if (s->program < 1)
        return GL_INVALID_INDEX;

    return shader_get_uniform_location(s->program, name,
        &s->uniform_name_buf, &s->uniform_location_buf);
}

void shader_glsl_set_uniform_block_binding(shader_glsl* s, GLchar* name, GLint binding) {
    GLint index = shader_get_uniform_block_index(s->program, name,
        &s->uniform_block_name_buf, &s->uniform_block_index_buf);
    if (index != GL_INVALID_INDEX)
        glUniformBlockBinding(s->program, index, binding);
}

void shader_glsl_free(shader_glsl* s) {
    if (s->program) {
        glDeleteProgram(s->program);
        s->program = 0;
    }
    string_free(&s->name);
    vector_old_uint64_t_free(&s->uniform_name_buf, 0);
    vector_old_int32_t_free(&s->uniform_location_buf, 0);
    vector_old_uint64_t_free(&s->uniform_block_name_buf, 0);
    vector_old_int32_t_free(&s->uniform_block_index_buf, 0);
}
