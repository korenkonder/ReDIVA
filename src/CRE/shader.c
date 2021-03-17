/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "shader.h"
#include "../CRE/hash.h"
#include "../KKdLib/io_path.h"
#include "../KKdLib/len_array.h"

static int32_t program = 0;

static char* shader_parse_define(char* data, shader_param* param) {
    if (!data || !param)
        return data;

    const size_t s = SHADER_PARAM_NUM_PARAMS;

    size_t temp_len[SHADER_PARAM_NUM_PARAMS];
    char temp[0x100];
    memset(temp, 0, 0x100);

    for (size_t i = 0; i < s; i++)
        if (param->param[i]) {
            sprintf_s(temp, 0x100, "#define %s\n", param->param[i]);
            temp_len[i] = strlen(temp);
        }
        else
            temp_len[i] = 0;

    char* def = strstr(data, "//DEF\n");
    if (!def)
        return data;
        size_t len_b = def - data;
    def += 5;
    size_t len_a = strlen(def);
    size_t len = 0;
    for (size_t i = 0; i < s; i++)
        len += temp_len[i];
    if (!len)
        def++;
    len += len_b + len_a;

    char* temp_data = force_malloc(len + 1);
    size_t pos = 0;
    memcpy(temp_data + pos, data, len_b);
    pos += len_b;
    for (size_t i = 0; i < s; i++) {
        if (param->param[i]) {
            sprintf_s(temp, 0x100, "#define %s\n", param->param[i]);
            memcpy(temp_data + pos, temp, len_b);
        }
        pos += temp_len[i];
    }
    memcpy(temp_data + pos, def, len_a);
    free(data);
    return temp_data;
}

static GLuint shader_compile(GLenum type, char* data) {
    if (!data)
        return 0;

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &data, 0);
    glCompileShader(shader);
    return shader;
}

void shader_fbo_load(shader_fbo* s, farc* f, shader_param* param) {
    wchar_t_buffer_init(&s->name, param->name);
    wchar_t* temp_frag = path_wadd_extension(param->frag, L".frag");
    wchar_t* temp_vert = path_wadd_extension(param->vert, L".vert");
    wchar_t* temp_geom = path_wadd_extension(param->geom, L".geom");
    farc_file* frag = farc_wread_file(f, temp_frag);
    farc_file* vert = farc_wread_file(f, temp_vert);
    farc_file* geom = farc_wread_file(f, temp_geom);
    size_t frag_length = !frag ? 0 : frag->size_uncompressed;
    size_t vert_length = !vert ? 0 : vert->size_uncompressed;
    size_t geom_length = !geom ? 0 : geom->size_uncompressed;
    char* frag_data = !frag ? 0 : force_malloc(frag_length + 1);
    char* vert_data = !vert ? 0 : force_malloc(vert_length + 1);
    char* geom_data = !geom ? 0 : force_malloc(geom_length + 1);

    if (frag) {
        memcpy(frag_data, frag->data, frag_length);
        frag_data[frag_length] = 0;
    }

    if (vert) {
        memcpy(vert_data, vert->data, vert_length);
        vert_data[vert_length] = 0;
    }

    if (geom) {
        memcpy(geom_data, geom->data, geom_length);
        geom_data[geom_length] = 0;
    }

    free(temp_frag);
    free(temp_vert);
    free(temp_geom);

    frag_data = shader_parse_define(frag_data, param);
    vert_data = shader_parse_define(vert_data, param);
    geom_data = shader_parse_define(geom_data, param);

    GLuint frag_shad = shader_compile(GL_FRAGMENT_SHADER, frag_data);
    GLuint vert_shad = shader_compile(GL_VERTEX_SHADER, vert_data);
    GLuint geom_shad = shader_compile(GL_GEOMETRY_SHADER, geom_data);

    free(frag_data);
    free(vert_data);
    free(geom_data);

    GLint success;
    GLchar info_log[0x200];
    s->program = glCreateProgram();
    if (frag_shad)
        glAttachShader(s->program, frag_shad);
    if (vert_shad)
        glAttachShader(s->program, vert_shad);
    if (geom_shad)
        glAttachShader(s->program, geom_shad);
    glLinkProgram(s->program);

    glGetProgramiv(s->program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(s->program, 0x200, 0, info_log);
        wprintf(L"Program Shader linking error: %ls\n", param->name);
        printf("%s\n", info_log);
    }

    if (frag_shad)
        glDeleteShader(frag_shad);
    if (vert_shad)
        glDeleteShader(vert_shad);
    if (geom_shad)
        glDeleteShader(geom_shad);
}

void shader_fbo_use(shader_fbo* s) {
    if (s) {
        if (program == s->program)
            return;

        glUseProgram(s->program);
        program = s->program;
    }
    else if (program) {
        glUseProgram(0);
        program = 0;
    }
}

GLint shader_fbo_get_uniform_location(shader_fbo* s, GLchar* name) {
    if (s->program < 1)
        return -1;

    shader_fbo_use(s);
    GLint location = glGetUniformLocation(s->program, name);
    if (location == -1)
        printf("Location \"%s\" in program %d isn't found\n", name, s->program);;
    return location;
}

void shader_fbo_set_uniform_block_binding(shader_fbo* s, GLchar* name, GLint binding) {
    GLint index = glGetUniformBlockIndex(s->program, name);
    if (index > -1)
        glUniformBlockBinding(s->program, index, binding);
}

void shader_fbo_free(shader_fbo* s) {
    if (s->program) {
        glDeleteProgram(s->program);
        s->program = 0;
    }
    wchar_t_buffer_dispose(&s->name);
}

void shader_model_load(shader_model* s, shader_model_update* upd) {
    wchar_t_buffer_init(&s->name, upd->param.name);
    size_t frag_c_length = !upd->frag_c ? 0 : strlen(upd->frag_c);
    size_t frag_g_length = !upd->frag_g ? 0 : strlen(upd->frag_g);
    size_t vert_length = !upd->vert ? 0 : strlen(upd->vert);
    size_t geom_length = !upd->geom ? 0 : strlen(upd->geom);
    char* frag_c_data = !upd->frag_c ? 0 : force_malloc(frag_c_length + 1);
    char* frag_g_data = !upd->frag_g ? 0 : force_malloc(frag_g_length + 1);
    char* vert_data = !upd->vert ? 0 : force_malloc(vert_length + 1);
    char* geom_data = !upd->geom ? 0 : force_malloc(geom_length + 1);

    if (upd->frag_c) {
        memcpy(frag_c_data, upd->frag_c, frag_c_length);
        frag_c_data[frag_c_length] = 0;
    }

    if (upd->frag_g) {
        memcpy(frag_g_data, upd->frag_g, frag_g_length);
        frag_g_data[frag_g_length] = 0;
    }

    if (upd->vert) {
        memcpy(vert_data, upd->vert, vert_length);
        vert_data[vert_length] = 0;
    }

    if (upd->geom) {
        memcpy(geom_data, upd->geom, geom_length);
        geom_data[geom_length] = 0;
    }

    frag_c_data = shader_parse_define(frag_c_data, &upd->param);
    frag_g_data = shader_parse_define(frag_g_data, &upd->param);
    vert_data = shader_parse_define(vert_data, &upd->param);
    geom_data = shader_parse_define(geom_data, &upd->param);

    GLuint frag_c_shad = shader_compile(GL_FRAGMENT_SHADER, frag_c_data);
    GLuint frag_g_shad = shader_compile(GL_FRAGMENT_SHADER, frag_g_data);
    GLuint vert_shad = shader_compile(GL_VERTEX_SHADER, vert_data);
    GLuint geom_shad = shader_compile(GL_GEOMETRY_SHADER, geom_data);

    free(frag_c_data);
    free(frag_g_data);
    free(vert_data);
    free(geom_data);

    GLint success;
    GLchar info_log[0x200];
    s->c_program = glCreateProgram();
    s->g_program = glCreateProgram();
    if (frag_c_shad)
        glAttachShader(s->c_program, frag_c_shad);
    if (vert_shad)
        glAttachShader(s->c_program, vert_shad);
    if (geom_shad)
        glAttachShader(s->c_program, geom_shad);
    glLinkProgram(s->c_program);

    glGetProgramiv(s->c_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(s->c_program, 0x200, 0, info_log);
        wprintf(L"Program C Shader linking error: %ls\n", upd->param.name);
        printf("%s\n", info_log);
    }

    if (frag_g_shad)
        glAttachShader(s->g_program, frag_g_shad);
    if (vert_shad)
        glAttachShader(s->g_program, vert_shad);
    if (geom_shad)
        glAttachShader(s->g_program, geom_shad);
    glLinkProgram(s->g_program);

    glGetProgramiv(s->g_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(s->g_program, 0x200, 0, info_log);
        wprintf(L"Program G Shader linking error: %ls\n", upd->param.name);
        printf("%s\n", info_log);
    }

    if (frag_c_shad)
        glDeleteShader(frag_c_shad);
    if (frag_g_shad)
        glDeleteShader(frag_g_shad);
    if (vert_shad)
        glDeleteShader(vert_shad);
    if (geom_shad)
        glDeleteShader(geom_shad);
}

void shader_model_c_use(shader_model* s) {
    if (s) {
        if (program == s->c_program)
            return;

        glUseProgram(s->c_program);
        program = s->c_program;
    }
    else if (program) {
        glUseProgram(0);
        program = 0;
    }
}

void shader_model_g_use(shader_model* s) {
    if (s) {
        if (program == s->g_program)
            return;

        glUseProgram(s->g_program);
        program = s->g_program;
    }
    else if (program) {
        glUseProgram(0);
        program = 0;
    }
}

GLint shader_model_c_get_uniform_location(shader_model* s, GLchar* name) {
    if (s->c_program < 1)
        return -1;

    shader_model_c_use(s);
    GLint location = glGetUniformLocation(s->c_program, name);
    if (location == -1)
        printf("Location \"%s\" in program %d isn't found", name, s->c_program);;
    return location;
}

GLint shader_model_g_get_uniform_location(shader_model* s, GLchar* name) {
    if (s->g_program < 1)
        return -1;

    shader_model_g_use(s);
    GLint location = glGetUniformLocation(s->g_program, name);
    if (location == -1)
        printf("Location \"%s\" in program %d isn't found", name, s->g_program);;
    return location;
}

void shader_model_set_uniform_block_binding(shader_model* s, GLchar* name, GLint binding) {
    GLint index;
    index = glGetUniformBlockIndex(s->c_program, name);
    if (index > -1)
        glUniformBlockBinding(s->c_program, index, binding);

    index = glGetUniformBlockIndex(s->g_program, name);
    if (index > -1)
        glUniformBlockBinding(s->g_program, index, binding);
}

void shader_model_free(shader_model* s) {
    if (s->c_program) {
        glDeleteProgram(s->c_program);
        s->c_program = 0;
    }

    if (s->g_program) {
        glDeleteProgram(s->g_program);
        s->g_program = 0;
    }
    wchar_t_buffer_dispose(&s->name);
}
