/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "shader.h"
#include "../CRE/hash.h"
#include "../KKdLib/str_utils.h"

static const char* process_mat_string =
    "uniform bool use_bones;\n"
    "\n"
    "uniform sampler2D BoneMatrix;\n"
    "\n"
    "vec4 P = vec4(0.0);\n"
    "vec3 N = vec3(0.0);\n"
    "vec3 T = vec3(0.0);\n"
    "\n"
    "vec4 position;\n"
    "vec3 normal;\n"
    "vec3 tangent;\n"
    "\n"
    "void getBone(int bone_index, float bone_weight)\n"
    "{\n"
    "    vec4 v0 = texelFetch(BoneMatrix, ivec2(0, bone_index), 0);\n"
    "    vec4 v1 = texelFetch(BoneMatrix, ivec2(1, bone_index), 0);\n"
    "    vec4 v2 = texelFetch(BoneMatrix, ivec2(2, bone_index), 0);\n"
    "    vec4 v3 = texelFetch(BoneMatrix, ivec2(3, bone_index), 0);\n"
    "    vec3 v4 = texelFetch(BoneMatrix, ivec2(4, bone_index), 0).xyz;\n"
    "    vec3 v5 = texelFetch(BoneMatrix, ivec2(5, bone_index), 0).xyz;\n"
    "    vec3 v6 = texelFetch(BoneMatrix, ivec2(6, bone_index), 0).xyz;\n"
    "    mat4 model_bone = mat4(v0, v1, v2, v3);\n"
    "    mat3 model_bone_normal = mat3(v4, v5, v6);\n"
    "    P += (model_bone * position) * bone_weight;\n"
    "    N += (model_bone_normal * normal) * bone_weight;\n"
    "    T += (model_bone_normal * tangent) * bone_weight;\n"
    "}\n"
    "\n"
    "void PROCESSMAT()\n"
    "{\n"
    "    if (use_bones && (aBoneWeight[0] != 0.0 || aBoneWeight[1] != 0.0 || aBoneWeight[2] != 0.0 || aBoneWeight[3] != 0.0))\n"
    "    {\n"
    "        P = vec4(0.0);\n"
    "        N = vec3(0.0);\n"
    "        T = vec3(0.0);\n"
    "        if (aBoneWeight[0] != 0.0)\n"
    "            getBone(int(aBoneIndex[0]), aBoneWeight[0]);\n"
    "        if (aBoneWeight[1] != 0.0)\n"
    "            getBone(int(aBoneIndex[1]), aBoneWeight[1]);\n"
    "        if (aBoneWeight[2] != 0.0)\n"
    "            getBone(int(aBoneIndex[2]), aBoneWeight[2]);\n"
    "        if (aBoneWeight[3] != 0.0)\n"
    "            getBone(int(aBoneIndex[3]), aBoneWeight[3]);\n"
    "\n"
    "        N = normalize(N);\n"
    "        T = normalize(T);\n"
    "        T = normalize(T - dot(N, T) * N);\n"
    "    }\n"
    "    else\n"
    "    {\n"
    "        P = position;\n"
    "        N = normal;\n"
    "        T = tangent;\n"
    "    }\n"
    "\n"
    "    P = model * P;\n"
    "    N = model_normal * N;\n"
    "    T = model_normal * T;\n"
    "\n"
    "    N = normalize(N);\n"
    "    T = normalize(T);\n"
    "    T = normalize(T - dot(N, T) * N);\n"
    "}";

static const char* tex_decode_string =
    "uniform sampler2D[8] material;\n"
    "uniform int[8] tex_mode;\n"
    "\n"
    "// Tex ID\n"
    "// 0 - diffuse\n"
    "// 1 - specular\n"
    "// 2 - displacement\n"
    "// 3 - normal\n"
    "// 4 - albedo\n"
    "// 5 - ao\n"
    "// 6 - metallic\n"
    "// 7 - roughness\n"
    "\n"
    "// Tex Mode\n"
    "// 0 - default\n"
    "// 1 - YCbCr BC4\n"
    "// 2 - YCbCr BC5\n"
    "// 3 - Normal Map BC5 Unsigned\n"
    "// 4 - Normal Map BC5 Signed\n"
    "// 5 - Normal Map Unsigned\n"
    "// 6 - Normal Map Signed\n"
    "\n"
    "const vec3 _red_coef_709 = vec3( 1.5748, 1.0,  0.0000);\n"
    "const vec3 _grn_coef_709 = vec3(-0.4681, 1.0, -0.1873);\n"
    "const vec3 _blu_coef_709 = vec3( 0.0000, 1.0,  1.8556);\n"
    "\n"
    "const float cbcr_mult = 256.0 / 255.0;\n"
    "const float cbcr_sub = 128.5 / 255.0;\n"
    "\n"
    "vec4 TEXDECODE(int id, vec2 uv)\n"
    "{\n"
    "    vec4 data = vec4(0.0);\n"
    "    int mode = tex_mode[id];\n"
    "    if (mode == 1)\n"
    "    {\n"
    "        vec3 tmp;\n"
    "        tmp.y = textureLod(material[id], uv, 0).x;\n"
    "        tmp.x = textureLod(material[id], uv, 1).x;\n"
    "        tmp.z = textureLod(material[id], uv, 2).x;\n"
    "        tmp.xz = tmp.xz * cbcr_mult - cbcr_sub;\n"
    "\n"
    "        vec4 c;\n"
    "        c.r = dot(tmp.xyz, _red_coef_709);\n"
    "        c.g = dot(tmp.xyz, _grn_coef_709);\n"
    "        c.b = dot(tmp.xyz, _blu_coef_709);\n"
    "        c.a = 1.0;\n"
    "        data = c;\n"
    "    }\n"
    "    else if (mode == 2)\n"
    "    {\n"
    "        vec4 tmp;\n"
    "        tmp.yw = textureLod(material[id], uv, 0).xy;\n"
    "        tmp.xz = textureLod(material[id], uv, 1).yx;\n"
    "        tmp.xz = tmp.xz * cbcr_mult - cbcr_sub;\n"
    "\n"
    "        vec4 c;\n"
    "        c.r = dot(tmp.xyz, _red_coef_709);\n"
    "        c.g = dot(tmp.xyz, _grn_coef_709);\n"
    "        c.b = dot(tmp.xyz, _blu_coef_709);\n"
    "        c.a = tmp.a;\n"
    "        data = c;\n"
    "    }\n"
    "    else if (mode == 3)\n"
    "    {\n"
    "        vec3 normal;\n"
    "        normal.xy = texture(material[id], uv).xy * 2.0 - 1.0;\n"
    "        normal.z = sqrt(1.0 - normal.x * normal.x - normal.y * normal.y);\n"
    "        data = vec4(normal, 0.0);\n"
    "    }\n"
    "    else if (mode == 4)\n"
    "    {\n"
    "        vec3 normal;\n"
    "        normal.xy = texture(material[id], uv).xy;\n"
    "        normal.z = sqrt(1.0 - normal.x * normal.x - normal.y * normal.y);\n"
    "        data = vec4(normal, 0.0);\n"
    "    }\n"
    "    else if (mode == 5)\n"
    "    {\n"
    "        vec3 normal = texture(material[id], uv).xyz * 2.0 - 1.0;\n"
    "        data = vec4(normal, 0.0);\n"
    "    }\n"
    "    else\n"
    "        data = texture(material[id], uv);\n"
    "    return data;\n"
    "}";

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
            sprintf_s(temp, sizeof(temp), "#define %s\n", param->param[i]);
            temp_len[i] = strlen(temp);
        }
        else
            temp_len[i] = 0;

    char* def = strstr(data, "//DEF\n");
    if (!def)
        return data;

    size_t len_a = def - data;
    def += 5;
    size_t len_b = strlen(def);
    size_t len = 0;
    for (size_t i = 0; i < s; i++)
        len += temp_len[i];
    if (!len)
        def++;
    len += len_a + len_b;

    char* temp_data = force_malloc(len + 1);
    size_t pos = 0;
    memcpy(temp_data + pos, data, len_a);
    pos += len_a;
    for (size_t i = 0; i < s; i++) {
        if (param->param[i]) {
            sprintf_s(temp, sizeof(temp), "#define %s\n", param->param[i]);
            memcpy(temp_data + pos, temp, len_a);
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
    def += strlen(parse_string);
    size_t len_b = strlen(replace_string);
    size_t len_c = strlen(def);
    size_t len = len_a + len_b + len_c;

    char* temp_data = force_malloc(len + 1);
    size_t pos = 0;
    memcpy(temp_data + pos, data, len_a);
    pos += len_a;
    memcpy(temp_data + pos, replace_string, len_b);
    pos += len_b;
    memcpy(temp_data + pos, def, len_c);
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
    if (!s || !f || !param)
        return;

    wchar_t temp[0x1000];
    temp[0] = 0;
    wcscat_s(temp, sizeof(temp) / sizeof(wchar_t), param->name);
    for (size_t i = 0; i < SHADER_PARAM_NUM_PARAMS; i++)
        if (param->param[i]) {
            wchar_t* t = char_string_to_wchar_t_string(param->param[i]);
            wcscat_s(temp, sizeof(temp) / sizeof(wchar_t), L"#");
            wcscat_s(temp, sizeof(temp) / sizeof(wchar_t), t);
            free(t);
        }
        else
            break;
    wchar_t_buffer_init(&s->name, temp);

    wchar_t* temp_frag = str_utils_wadd(param->frag, L".frag");
    wchar_t* temp_vert = str_utils_wadd(param->vert, L".vert");
    wchar_t* temp_geom = str_utils_wadd(param->geom, L".geom");
    farc_file* frag = farc_wread_file(f, temp_frag);
    farc_file* vert = farc_wread_file(f, temp_vert);
    farc_file* geom = farc_wread_file(f, temp_geom);
    size_t frag_length = !frag ? 0 : frag->size;
    size_t vert_length = !vert ? 0 : vert->size;
    size_t geom_length = !geom ? 0 : geom->size;
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
        printf("Program FBO Shader linking error: ");
        wprintf(param->name);
        putchar('\n');
        printf(info_log);
        putchar('\n');
    }

    if (frag_shad)
        glDeleteShader(frag_shad);
    if (vert_shad)
        glDeleteShader(vert_shad);
    if (geom_shad)
        glDeleteShader(geom_shad);
}

inline void shader_fbo_use(shader_fbo* s) {
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
    if (!s || !upd)
        return;

    wchar_t temp[0x1000];
    temp[0] = 0;
    wcscat_s(temp, sizeof(temp) / sizeof(wchar_t), upd->param.name);
    for (size_t i = 0; i < SHADER_PARAM_NUM_PARAMS; i++)
        if (upd->param.param[i]) {
            wchar_t* t = char_string_to_wchar_t_string(upd->param.param[i]);
            wcscat_s(temp, sizeof(temp) / sizeof(wchar_t), L"#");
            wcscat_s(temp, sizeof(temp) / sizeof(wchar_t), t);
            free(t);
        }
        else
            break;
    wchar_t_buffer_init(&s->name, temp);

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

    vert_data = shader_parse(vert_data, "//PROCESSMAT", process_mat_string);

    frag_c_data = shader_parse(frag_c_data, "//TEXDECODE", tex_decode_string);
    frag_g_data = shader_parse(frag_g_data, "//TEXDECODE", tex_decode_string);
    vert_data = shader_parse(vert_data, "//TEXDECODE", tex_decode_string);
    geom_data = shader_parse(geom_data, "//TEXDECODE", tex_decode_string);

    GLuint frag_c_shad = shader_compile(GL_FRAGMENT_SHADER, frag_c_data);
    GLuint frag_g_shad = shader_compile(GL_FRAGMENT_SHADER, frag_g_data);
    GLuint vert_shad = shader_compile(GL_VERTEX_SHADER, vert_data);
    GLuint geom_shad = shader_compile(GL_GEOMETRY_SHADER, geom_data);

    free(frag_c_data);
    free(frag_g_data);
    free(vert_data);
    free(geom_data);

    GLint success;
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
        GLchar* info_log = force_malloc(0x10000);
        glGetProgramInfoLog(s->c_program, 0x10000, 0, info_log);
        printf("Program C Shader linking error: ");
        wprintf(upd->param.name);
        putchar('\n');
        printf(info_log);
        putchar('\n');
        free(info_log);
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
        GLchar* info_log = force_malloc(0x10000);
        glGetProgramInfoLog(s->g_program, 0x200, 0, info_log);
        printf("Program G Shader linking error: ");
        wprintf(upd->param.name);
        putchar('\n');
        printf(info_log);
        putchar('\n');
        free(info_log);
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

inline void shader_model_c_use(shader_model* s) {
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

inline void shader_model_g_use(shader_model* s) {
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
        printf("Location \"%s\" in program %d isn't found\n", name, s->c_program);;
    return location;
}

GLint shader_model_g_get_uniform_location(shader_model* s, GLchar* name) {
    if (s->g_program < 1)
        return -1;

    shader_model_g_use(s);
    GLint location = glGetUniformLocation(s->g_program, name);
    if (location == -1)
        printf("Location \"%s\" in program %d isn't found\n", name, s->g_program);;
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
