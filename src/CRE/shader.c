/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "shader.h"
#include "../KKdLib/hash.h"
#include "../KKdLib/str_utils.h"

static const char* process_mat_string =
    "uniform bool use_bones;\n"
    "\n"
    "struct bone_mat_struct {\n"
    "    mat4 mat;\n"
    "    mat3 mat_normal;\n"
    "};\n"
    "\n"
    "layout(std430, binding = 0) readonly buffer bone_mat_buf {\n"
    "    bone_mat_struct bone_data[];\n"
    "};"
    "\n"
    "void PROCESSMAT(in vec4 in_position, in vec3 in_normal, in vec3 in_tangent,\n"
    "    in ivec4 bone_index, in vec4 bone_weight,\n"
    "    out vec4 out_position, out vec3 out_normal, out vec3 out_tangent) {\n"
    "    vec4 P;\n"
    "    vec3 N;\n"
    "    vec3 T;\n"
    "    \n"
    "    if (use_bones && bone_index[0] != 0xFFFF) {\n"
    "        P = vec4(0.0);\n"
    "        N = vec3(0.0);\n"
    "        T = vec3(0.0);\n"
    "\n"
    "        int index = bone_index[0];\n"
    "        float weight = bone_weight[0];\n"
    "        P += (bone_data[index].mat * in_position) * weight;\n"
    "        N += (bone_data[index].mat_normal * in_normal) * weight;\n"
    "        T += (bone_data[index].mat_normal * in_tangent) * weight;\n"
    "\n"
    "        if (bone_weight[1] != 0.0 && bone_index[1] != 0xFFFF) {\n"
    "            index = bone_index[1];\n"
    "            weight = bone_weight[1];\n"
    "            P += (bone_data[index].mat * in_position) * weight;\n"
    "            N += (bone_data[index].mat_normal * in_normal) * weight;\n"
    "            T += (bone_data[index].mat_normal * in_tangent) * weight;\n"
    "\n"
    "            if (bone_weight[2] != 0.0 && bone_index[2] != 0xFFFF) {\n"
    "                index = bone_index[2];\n"
    "                weight = bone_weight[2];\n"
    "                P += (bone_data[index].mat * in_position) * weight;\n"
    "                N += (bone_data[index].mat_normal * in_normal) * weight;\n"
    "                T += (bone_data[index].mat_normal * in_tangent) * weight;\n"
    "\n"
    "                if (bone_weight[3] != 0.0 && bone_index[3] != 0xFFFF) {\n"
    "                    index = bone_index[3];\n"
    "                    weight = bone_weight[3];\n"
    "                    P += (bone_data[index].mat * in_position) * weight;\n"
    "                    N += (bone_data[index].mat_normal * in_normal) * weight;\n"
    "                    T += (bone_data[index].mat_normal * in_tangent) * weight;\n"
    "                }\n"
    "            }\n"
    "        }\n"
    "\n"
    "        N = normalize(N);\n"
    "        T = normalize(T);\n"
    "        T = normalize(T - dot(N, T) * N);\n"
    "    }\n"
    "    else {\n"
    "        P = in_position;\n"
    "        N = in_normal;\n"
    "        T = in_tangent;\n"
    "    }\n"
    "\n"
    "    P = model * P;\n"
    "    N = model_normal * N;\n"
    "    T = model_normal * T;\n"
    "\n"
    "    N = normalize(N);\n"
    "    T = normalize(T);\n"
    "    T = normalize(T - dot(N, T) * N);\n"
    "\n"
    "    out_position = P;\n"
    "    out_normal = N;\n"
    "    out_tangent = T;\n"
    "}";

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

static const char* material_struct_string = 
    "uniform struct material_param_struct {\n"
    "    vec4 ambient;\n"
    "    vec4 diffuse;\n"
    "    vec4 specular;\n"
    "    vec4 emission;\n"
    "    float shininess;\n"
    "} material_param;\n"
    "\n"
    "#define MATERIAL_AMBIENT (material_param.ambient)\n"
    "#define MATERIAL_DIFFUSE (material_param.diffuse)\n"
    "#define MATERIAL_SPECULAR (material_param.specular)\n"
    "#define MATERIAL_EMISSION (material_param.emission)\n"
    "#define MATERIAL_SHININESS (material_param.shininess)";

static const char* tex_decode_string =
    "uniform sampler2D material[5];\n"
    "uniform samplerCube cube_map;\n"
    "uniform int tex_mode[6];\n"
    "\n"
    "// Tex ID\n"
    "// 0 - color\n"
    "// 1 - color mask / translucency\n"
    "// 2 - normal\n"
    "// 3 - specular\n"
    "// 4 - transparency\n"
    "// 5 - cube map\n"
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
    "#define TEXDECODE_COLOR(uv) TEXDECODE(0, uv)\n"
    "#define TEXDECODE_COLOR_MASK(uv) TEXDECODE(1, uv)\n"
    "#define TEXDECODE_TRANSLUCENCY(uv) TEXDECODE(1, uv)\n"
    "#define TEXDECODE_NORMAL(uv) TEXDECODE(2, uv)\n"
    "#define TEXDECODE_SPECULAR(uv) TEXDECODE(3, uv)\n"
    "#define TEXDECODE_TRANSPARENCY(uv) TEXDECODE(4, uv)\n"
    "#define TEXDECODE_CUBEMAP(uv) texture(cube_map, uv)\n"
    "\n"
    "const vec3 _red_coef_709 = vec3( 1.5748, 1.0,  0.0000);\n"
    "const vec3 _grn_coef_709 = vec3(-0.4681, 1.0, -0.1873);\n"
    "const vec3 _blu_coef_709 = vec3( 0.0000, 1.0,  1.8556);\n"
    "\n"
    "const float cbcr_mult = 1.003922; // 256.0 / 255.0\n"
    "const float cbcr_sub = 0.503929; // 128.501895 / 255.0\n"
    "\n"
    "vec4 TEXDECODE(int id, vec2 uv) {\n"
    "    vec4 data = vec4(0.0);\n"
    "    int mode = tex_mode[id];\n"
    "    if (mode == 1) {\n"
    "        vec3 tmp;\n"
    "        tmp.y = textureLod(material[id], uv, 0.0).x;\n"
    "        tmp.x = textureLod(material[id], uv, 1.0).x;\n"
    "        tmp.z = textureLod(material[id], uv, 2.0).x;\n"
    "        tmp.xz = tmp.xz * cbcr_mult - cbcr_sub;\n"
    "\n"
    "        vec4 c;\n"
    "        c.r = dot(tmp.xyz, _red_coef_709);\n"
    "        c.g = dot(tmp.xyz, _grn_coef_709);\n"
    "        c.b = dot(tmp.xyz, _blu_coef_709);\n"
    "        c.a = 1.0;\n"
    "        data = c;\n"
    "    }\n"
    "    else if (mode == 2) {\n"
    "        vec4 tmp;\n"
    "        tmp.yw = textureLod(material[id], uv, 0.0).xy;\n"
    "        tmp.xz = textureLod(material[id], uv, 1.0).yx;\n"
    "        tmp.xz = tmp.xz * cbcr_mult - cbcr_sub;\n"
    "\n"
    "        vec4 c;\n"
    "        c.r = dot(tmp.xyz, _red_coef_709);\n"
    "        c.g = dot(tmp.xyz, _grn_coef_709);\n"
    "        c.b = dot(tmp.xyz, _blu_coef_709);\n"
    "        c.a = tmp.a;\n"
    "        data = c;\n"
    "    }\n"
    "    else if (mode == 3) {\n"
    "        vec3 normal;\n"
    "        normal.xy = texture(material[id], uv).xy * 2.0 - 1.0;\n"
    "        normal.z = sqrt(1.0 - normal.x * normal.x - normal.y * normal.y);\n"
    "        data = vec4(normal, 0.0);\n"
    "    }\n"
    "    else if (mode == 4) {\n"
    "        vec3 normal;\n"
    "        normal.xy = texture(material[id], uv).xy;\n"
    "        normal.z = sqrt(1.0 - normal.x * normal.x - normal.y * normal.y);\n"
    "        data = vec4(normal, 0.0);\n"
    "    }\n"
    "    else if (mode == 5) {\n"
    "        vec3 normal = texture(material[id], uv).xyz * 2.0 - 1.0;\n"
    "        data = vec4(normal, 0.0);\n"
    "    }\n"
    "    else if (mode == 6) {\n"
    "        vec3 normal = texture(material[id], uv).xyz;\n"
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
            snprintf(temp, sizeof(temp), "#define %s\n", param->param[i]);
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
            snprintf(temp, sizeof(temp), "#define %s\n", param->param[i]);
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

static GLuint shader_compile(GLenum type, const char* data) {
    if (!data)
        return 0;

    GLint success;
    GLchar* info_log = force_malloc(0x10000);
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &data, 0);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 0x10000, 0, info_log);
        printf("Shader compile error: ");
        printf(info_log);
        putchar('\n');
    }
    free(info_log);
    return shader;
}

static GLint shader_get_uniform_location(GLint program, GLchar* name,
    vector_uint64_t* uniform_name_buf, vector_int32_t* uniform_location_buf) {
    uint64_t hash = hash_char_fnv1a64(name);
    for (uint64_t* i = uniform_name_buf->begin; i != uniform_name_buf->end; i++)
        if (*i == hash)
            return uniform_location_buf->begin[i - uniform_name_buf->begin];

    GLint location = glGetUniformLocation(program, name);
    if (location == GL_INVALID_INDEX)
        printf("Location \"%s\" in program %d isn't found\n", name, program);
    vector_uint64_t_push_back(uniform_name_buf, &hash);
    vector_int32_t_push_back(uniform_location_buf, &location);
    return location;

}

static GLint shader_get_uniform_block_index(GLint program, GLchar* name,
    vector_uint64_t* uniform_block_name_buf, vector_int32_t* uniform_block_index_buf) {
    uint64_t hash = hash_char_fnv1a64(name);
    for (uint64_t* i = uniform_block_name_buf->begin; i != uniform_block_name_buf->end; i++)
        if (*i == hash)
            return uniform_block_index_buf->begin[i - uniform_block_name_buf->begin];

    GLint index = glGetUniformBlockIndex(program, name);
    if (index == GL_INVALID_INDEX)
        printf("Block Index \"%s\" in program %d isn't found\n", name, program);
    vector_uint64_t_push_back(uniform_block_name_buf, &hash);
    vector_int32_t_push_back(uniform_block_index_buf, &index);
    return index;
}

void shader_fbo_load(shader_fbo* s, farc* f, shader_param* param) {
    if (!s || !f || !param)
        return;

    if (s->program)
        glDeleteProgram(s->program);

    vector_uint64_t_clear(&s->uniform_name_buf);
    vector_int32_t_clear(&s->uniform_location_buf);
    vector_uint64_t_clear(&s->uniform_block_name_buf);
    vector_int32_t_clear(&s->uniform_block_index_buf);

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
    wstring_init(&s->name, temp);

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

    vert_data = shader_parse(vert_data, "//COMMONDATA", common_data_string);
    frag_data = shader_parse(frag_data, "//COMMONDATA", common_data_string);
    geom_data = shader_parse(geom_data, "//COMMONDATA", common_data_string);

    GLuint frag_shad = shader_compile(GL_FRAGMENT_SHADER, frag_data);
    GLuint vert_shad = shader_compile(GL_VERTEX_SHADER, vert_data);
    GLuint geom_shad = shader_compile(GL_GEOMETRY_SHADER, geom_data);

    GLint success;
    GLchar* info_log = force_malloc(0x10000);
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
        glGetProgramInfoLog(s->program, 0x10000, 0, info_log);
        printf("Program FBO Shader linking error: ");
        wprintf(param->name);
        putchar('\n');
        printf(info_log);
        putchar('\n');
    }
    free(info_log);

    free(frag_data);
    free(vert_data);
    free(geom_data);

    if (frag_shad)
        glDeleteShader(frag_shad);
    if (vert_shad)
        glDeleteShader(vert_shad);
    if (geom_shad)
        glDeleteShader(geom_shad);
}

void shader_fbo_load_file(shader_fbo* s, char* vert_path,
    char* frag_path, char* geom_path, shader_param* param) {
    if (!s || (!vert_path && !frag_path && !geom_path) || !param)
        return;

    vector_uint64_t_clear(&s->uniform_name_buf);
    vector_int32_t_clear(&s->uniform_location_buf);
    vector_uint64_t_clear(&s->uniform_block_name_buf);
    vector_int32_t_clear(&s->uniform_block_index_buf);

    stream* st;
    size_t l;

    char* vert = 0;
    char* frag = 0;
    char* geom = 0;

    st = io_open(vert_path, "r");
    if (st->io.stream) {
        l = st->length;
        vert = force_malloc(l + 1);
        io_read(st, vert, l);
        vert[l] = 0;
    }
    io_dispose(st);

    st = io_open(frag_path, "r");
    if (st->io.stream) {
        l = st->length;
        frag = force_malloc(l + 1);
        io_read(st, frag, l);
        frag[l] = 0;
    }
    io_dispose(st);

    st = io_open(geom_path, "r");
    if (st->io.stream) {
        l = st->length;
        geom = force_malloc(l + 1);
        io_read(st, geom, l);
        geom[l] = 0;
    }
    io_dispose(st);

    shader_fbo_load_string(s, vert, frag, geom, param);
    free(vert);
    free(frag);
}

void shader_fbo_wload_file(shader_fbo* s, wchar_t* vert_path,
    wchar_t* frag_path, wchar_t* geom_path, shader_param* param) {
    if (!s || (!vert_path && !frag_path && !geom_path) || !param)
        return;

    vector_uint64_t_clear(&s->uniform_name_buf);
    vector_int32_t_clear(&s->uniform_location_buf);
    vector_uint64_t_clear(&s->uniform_block_name_buf);
    vector_int32_t_clear(&s->uniform_block_index_buf);

    stream* st;
    size_t l;

    char* vert = 0;
    char* frag = 0;
    char* geom = 0;

    st = io_wopen(vert_path, L"r");
    if (st->io.stream) {
        l = st->length;
        vert = force_malloc(l + 1);
        io_read(st, vert, l);
        vert[l] = 0;
    }
    io_dispose(st);

    st = io_wopen(frag_path, L"r");
    if (st->io.stream) {
        l = st->length;
        frag = force_malloc(l + 1);
        io_read(st, frag, l);
        frag[l] = 0;
    }
    io_dispose(st);

    st = io_wopen(geom_path, L"r");
    if (st->io.stream) {
        l = st->length;
        geom = force_malloc(l + 1);
        io_read(st, geom, l);
        geom[l] = 0;
    }
    io_dispose(st);

    shader_fbo_load_string(s, vert, frag, geom, param);
    free(vert);
    free(frag);
}

void shader_fbo_load_string(shader_fbo* s, char* vert,
    char* frag, char* geom, shader_param* param) {
    if (!s || (!frag && !vert && !geom) || !param)
        return;

    vector_uint64_t_clear(&s->uniform_name_buf);
    vector_int32_t_clear(&s->uniform_location_buf);
    vector_uint64_t_clear(&s->uniform_block_name_buf);
    vector_int32_t_clear(&s->uniform_block_index_buf);

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

    GLint success;
    GLchar* info_log = force_malloc(0x10000);
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
        glGetProgramInfoLog(s->program, 0x10000, 0, info_log);
        printf("Program FBO Shader linking error: ");
        wprintf(param->name);
        putchar('\n');
        printf(info_log);
        putchar('\n');
    }
    free(info_log);

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
        return GL_INVALID_INDEX;

    return shader_get_uniform_location(s->program, name,
        &s->uniform_name_buf, &s->uniform_location_buf);
}

void shader_fbo_set_uniform_block_binding(shader_fbo* s, GLchar* name, GLint binding) {
    GLint index = shader_get_uniform_block_index(s->program, name,
        &s->uniform_block_name_buf, &s->uniform_block_index_buf);
    if (index != GL_INVALID_INDEX)
        glUniformBlockBinding(s->program, index, binding);
}

void shader_fbo_free(shader_fbo* s) {
    if (s->program) {
        glDeleteProgram(s->program);
        s->program = 0;
    }
    wstring_dispose(&s->name);
    vector_uint64_t_free(&s->uniform_name_buf);
    vector_int32_t_free(&s->uniform_location_buf);
    vector_uint64_t_free(&s->uniform_block_name_buf);
    vector_int32_t_free(&s->uniform_block_index_buf);
}

void shader_model_load(shader_model* s, shader_model_data* upd) {
    if (!s || !upd)
        return;

    if (s->program)
        glDeleteProgram(s->program);

    vector_uint64_t_clear(&s->uniform_name_buf);
    vector_int32_t_clear(&s->uniform_location_buf);
    vector_uint64_t_clear(&s->uniform_block_name_buf);
    vector_int32_t_clear(&s->uniform_block_index_buf);

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
    wstring_init(&s->name, temp);

    size_t frag_length = !upd->frag ? 0 : strlen(upd->frag);
    size_t vert_length = !upd->vert ? 0 : strlen(upd->vert);
    size_t geom_length = !upd->geom ? 0 : strlen(upd->geom);
    char* frag_data = !upd->frag ? 0 : force_malloc(frag_length + 1);
    char* vert_data = !upd->vert ? 0 : force_malloc(vert_length + 1);
    char* geom_data = !upd->geom ? 0 : force_malloc(geom_length + 1);

    if (upd->frag) {
        memcpy(frag_data, upd->frag, frag_length);
        frag_data[frag_length] = 0;
    }

    if (upd->vert) {
        memcpy(vert_data, upd->vert, vert_length);
        vert_data[vert_length] = 0;
    }

    if (upd->geom) {
        memcpy(geom_data, upd->geom, geom_length);
        geom_data[geom_length] = 0;
    }

    frag_data = shader_parse_define(frag_data, &upd->param);
    vert_data = shader_parse_define(vert_data, &upd->param);
    geom_data = shader_parse_define(geom_data, &upd->param);

    vert_data = shader_parse(vert_data, "//COMMONDATA", common_data_string);
    frag_data = shader_parse(frag_data, "//COMMONDATA", common_data_string);
    geom_data = shader_parse(geom_data, "//COMMONDATA", common_data_string);

    vert_data = shader_parse(vert_data, "//PROCESSMAT", process_mat_string);

    frag_data = shader_parse(frag_data, "//MATERIAL", material_struct_string);
    frag_data = shader_parse(frag_data, "//TEXDECODE", tex_decode_string);

    GLuint frag_shad = shader_compile(GL_FRAGMENT_SHADER, frag_data);
    GLuint vert_shad = shader_compile(GL_VERTEX_SHADER, vert_data);
    GLuint geom_shad = shader_compile(GL_GEOMETRY_SHADER, geom_data);

    free(frag_data);
    free(vert_data);
    free(geom_data);

    GLint success;
    GLchar* info_log = force_malloc(0x10000);
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
        glGetProgramInfoLog(s->program, 0x10000, 0, info_log);
        printf("Program Shader linking error: ");
        wprintf(upd->param.name);
        putchar('\n');
        printf(info_log);
        putchar('\n');
    }
    free(info_log);

    if (frag_shad)
        glDeleteShader(frag_shad);
    if (vert_shad)
        glDeleteShader(vert_shad);
    if (geom_shad)
        glDeleteShader(geom_shad);
}

inline void shader_model_use(shader_model* s) {
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

GLint shader_model_get_uniform_location(shader_model* s, GLchar* name) {
    if (s->program < 1)
        return GL_INVALID_INDEX;

    return shader_get_uniform_location(s->program, name,
        &s->uniform_name_buf, &s->uniform_location_buf);
}

void shader_model_set_uniform_block_binding(shader_model* s, GLchar* name, GLint binding) {
    GLint index = shader_get_uniform_block_index(s->program, name,
        &s->uniform_block_name_buf, &s->uniform_block_index_buf);
    if (index != GL_INVALID_INDEX)
        glUniformBlockBinding(s->program, index, binding);
}

void shader_model_free(shader_model* s) {
    if (s->program) {
        glDeleteProgram(s->program);
        s->program = 0;
    }
    wstring_dispose(&s->name);
    vector_uint64_t_free(&s->uniform_name_buf);
    vector_int32_t_free(&s->uniform_location_buf);
    vector_uint64_t_free(&s->uniform_block_name_buf);
    vector_int32_t_free(&s->uniform_block_index_buf);
}
