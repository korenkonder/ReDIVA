/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "shader.h"
#include "../KKdLib/io/path.h"
#include "../KKdLib/hash.h"
#include "../KKdLib/str_utils.h"
#include "gl_state.h"
#include <shlobj_core.h>

struct program_binary {
    GLsizei length;
    GLenum binary_format;
    size_t binary;
};

static GLuint shader_compile_shader(GLenum type, const char* data, const char* file);
static GLuint shader_compile(const char* vert, const char* frag, const char* vp, const char* fp);
static GLuint shader_compile_binary(const char* vert, const char* frag, const char* vp, const char* fp,
    program_binary* bin, GLsizei* buffer_size, void** binary);
static bool shader_load_binary_shader(program_binary* bin, GLuint* program);
static bool shader_parse_define(const char* data, int32_t num_uniform,
    const int32_t* vp_unival_max, const int32_t* fp_unival_max,
    int32_t* uniform_value, char** temp, size_t* temp_size);
static char* shader_parse_include(char* data, farc* f);
static void shader_update_data(shader_set_data* set);

int32_t shader_bind(shader* shader, uint32_t sub_index) {
    int32_t num_sub = shader->num_sub;
    int32_t sub_shader_index = 0;
    if (num_sub < 1)
        return -1;

    for (shader_sub* i = shader->sub; i->sub_index != sub_index; i++) {
        sub_shader_index++;
        if (sub_shader_index >= num_sub)
            return -1;
    }

    shader_sub* sub_shader = &shader->sub[sub_shader_index];
    if (!sub_shader)
        return -1;

    int32_t num_uniform = shader->num_uniform;
    int32_t unival_curr = 1;
    int32_t unival = 0;
    GLint uniform_val[16];
    if (num_uniform > 0) {
        const int32_t* vp_unival_max = sub_shader->vp_unival_max;
        const int32_t* fp_unival_max = sub_shader->fp_unival_max;

        int32_t i = 0;
        for (i = 0; i < num_uniform && i < 16; i++) {
            int32_t unival_max = shader->use_permut[i]
                ? max(vp_unival_max[i], fp_unival_max[i]) : 0;
            unival += unival_curr * min(uniform_value[(int32_t)shader->use_uniform[i]], unival_max);
            unival_curr *= unival_max + 1;

            int32_t unival_max_glsl = max(vp_unival_max[i], fp_unival_max[i]);
            uniform_val[i] = min(uniform_value[shader->use_uniform[i]], unival_max_glsl);
        }

        for (; i < 16; i++)
            uniform_val[i] = 0;
    }

    gl_state_use_program(sub_shader->program[unival]);
    glUniform1iv(SHADER_MAX_PROGRAM_LOCAL_PARAMETERS * 2, 16, uniform_val);
    return 0;
}

inline void shader_draw_arrays(shader_set_data* set,
    GLenum mode, GLint first, GLsizei count) {
    shader_update_data(set);
    glDrawArrays(mode, first, count);
}

inline void shader_draw_elements(shader_set_data* set,
    GLenum mode, GLsizei count, GLenum type, const void* indices) {
    shader_update_data(set);
    glDrawElements(mode, count, type, indices);
}

inline void shader_draw_range_elements(shader_set_data* set,
    GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void* indices) {
    shader_update_data(set);
    glDrawRangeElements(mode, start, end, count, type, indices);
}

int32_t shader_get_index_by_name(shader_set_data* set, const char* name) {
    shader* shaders = set->shaders;
    for (size_t i = 0; i < set->size; i++)
        if (!str_utils_compare(shaders[i].name, name))
            return (int32_t)shaders[i].index;
    return -1;
}

void shader_load(shader_set_data* set, farc* f, bool ignore_cache, bool not_load_cache,
    const char* name, const shader_table* shaders_table, const size_t size,
    const shader_bind_func* bind_func_table, const size_t bind_func_table_size) {
    if (!set || !f || !shaders_table || !size)
        return;

    bool shader_cache_changed = false;
    farc shader_cache_farc;
    wchar_t temp_buf[MAX_PATH];
    if (!ignore_cache && SUCCEEDED(SHGetFolderPathW(0, CSIDL_LOCAL_APPDATA, 0, 0, temp_buf))) {
        wcscat_s(temp_buf, sizeof(temp_buf) / sizeof(wchar_t), L"\\CLOUD");
        CreateDirectoryW(temp_buf, 0);

        wchar_t buf[MAX_PATH];
        swprintf_s(buf, sizeof(buf) / sizeof(wchar_t), L"\\%hs_shader_cache", name);
        wcscat_s(temp_buf, sizeof(temp_buf) / sizeof(wchar_t), buf);

        swprintf_s(buf, sizeof(buf) / sizeof(wchar_t), L"%ls.farc", temp_buf);
        if (path_check_file_exists(buf) && !not_load_cache)
            shader_cache_farc.read(buf, true, false);
    }

    GLsizei buffer_size = 0x20000;
    void* binary = force_malloc(buffer_size);
    size_t temp_vert_size = 0x10000;
    char* temp_vert = force_malloc_s(char, temp_vert_size);
    size_t temp_frag_size = 0x10000;
    char* temp_frag = force_malloc_s(char, temp_frag_size);
    std::vector<int32_t> vec_vert;
    std::vector<int32_t> vec_frag;
    std::vector<program_binary> program_data_binary;
    set->shaders = force_malloc_s(shader, size);
    set->size = size;
    for (size_t i = 0; i < size; i++) {
        shader* shader = &set->shaders[i];
        shader->name = shaders_table[i].name;
        shader->index = shaders_table[i].index;
        shader->num_sub = shaders_table[i].num_sub;
        shader->sub = force_malloc_s(shader_sub, shader->num_sub);
        shader->num_uniform = shaders_table[i].num_uniform;
        shader->use_uniform = shaders_table[i].use_uniform;
        shader->use_permut = shaders_table[i].use_permut;

        int32_t num_sub = shader->num_sub;
        const shader_sub_table* sub_table = shaders_table[i].sub;
        shader_sub* sub = shader->sub;
        vec_vert.resize(shader->num_uniform);
        vec_frag.resize(shader->num_uniform);
        for (size_t j = 0; j < num_sub; j++, sub++, sub_table++) {
            sub->sub_index = sub_table->sub_index;
            sub->vp_unival_max = sub_table->vp_unival_max;
            sub->fp_unival_max = sub_table->fp_unival_max;

            char vert_file_buf[MAX_PATH];
            strcpy_s(vert_file_buf, sizeof(vert_file_buf), sub_table->vp);
            strcat_s(vert_file_buf, sizeof(vert_file_buf), ".vert");
            farc_file* vert_ff = f->read_file(vert_file_buf);

            char* vert_data = 0;
            if (vert_ff && vert_ff->data) {
                vert_data = force_malloc_s(char, vert_ff->size + 1);
                if (vert_data) {
                    memcpy(vert_data, vert_ff->data, vert_ff->size);
                    vert_data[vert_ff->size] = 0;
                }
            }

            char frag_file_buf[MAX_PATH];
            strcpy_s(frag_file_buf, sizeof(frag_file_buf), sub_table->fp);
            strcat_s(frag_file_buf, sizeof(frag_file_buf), ".frag");
            farc_file* frag_ff = f->read_file(frag_file_buf);

            char* frag_data = 0;
            if (frag_ff && frag_ff->data) {
                frag_data = force_malloc_s(char, frag_ff->size + 1);
                if (frag_data) {
                    memcpy(frag_data, frag_ff->data, frag_ff->size);
                    frag_data[frag_ff->size] = 0;
                }
            }

            if (!vert_data || !frag_data) {
                free(vert_data);
                free(frag_data);
                continue;
            }

            uint64_t vert_file_name_cache = hash_utf8_fnv1a64m(vert_file_buf);
            uint64_t frag_file_name_cache = hash_utf8_fnv1a64m(frag_file_buf);
            for (int32_t i = 0; i < 64; i += 8)
                if (((vert_file_name_cache >> i) & 0xFF) == 0)
                    vert_file_name_cache |= 0xFFULL << i;

            for (int32_t i = 0; i < 64; i += 8)
                if (((frag_file_name_cache >> i) & 0xFF) == 0)
                    frag_file_name_cache |= 0xFFULL << i;

            char shader_cache_file_name[MAX_PATH];
            strcpy_s(shader_cache_file_name, sizeof(shader_cache_file_name), sub_table->vp);
            if (str_utils_compare(sub_table->vp, sub_table->fp)) {
                strcat_s(shader_cache_file_name, sizeof(shader_cache_file_name), ".");
                strcat_s(shader_cache_file_name, sizeof(shader_cache_file_name), sub_table->fp);
            }
            strcat_s(shader_cache_file_name, sizeof(shader_cache_file_name), ".bin");

            vert_data = shader_parse_include(vert_data, f);
            frag_data = shader_parse_include(frag_data, f);
            uint64_t vert_data_hash = hash_utf8_fnv1a64m(vert_data);
            uint64_t frag_data_hash = hash_utf8_fnv1a64m(frag_data);

            farc_file* shader_cache_file = shader_cache_farc.read_file(shader_cache_file_name);
            program_binary* bin = 0;
            if (!ignore_cache) {
                if (!shader_cache_file || !shader_cache_file->data)
                    /*printf("data error: %s %s\n", vert_file_buf, frag_file_buf)*/;
                else if (vert_data_hash != ((uint64_t*)shader_cache_file->data)[0]
                    || frag_data_hash != ((uint64_t*)shader_cache_file->data)[1])
                    /*printf("hash error: %s %s\n", vert_file_buf, frag_file_buf)*/;
                else
                    bin = (program_binary*)&((uint64_t*)shader_cache_file->data)[2];
            }

            if (shader->num_uniform > 0) {
                int32_t num_uniform = shader->num_uniform;
                size_t unival_curr = 1;
                size_t unival_count = 1;
                const int32_t* vp_unival_max = sub_table->vp_unival_max;
                const int32_t* fp_unival_max = sub_table->fp_unival_max;
                for (size_t k = 0; k < num_uniform; k++) {
                    size_t unival_max = shader->use_permut[k]
                        ? max(vp_unival_max[k], fp_unival_max[k]) : 0;
                    unival_count += unival_curr * unival_max;
                    unival_curr *= unival_max + 1;
                }

                if (!ignore_cache)
                    program_data_binary.reserve(unival_count);
                sub->program = force_malloc_s(GLuint, unival_count);
                if (sub->program) {
                    char vert_buf[MAX_PATH];
                    char frag_buf[MAX_PATH];

                    strcpy_s(vert_buf, sizeof(vert_buf), sub_table->vp);
                    size_t vert_buf_pos = utf8_length(vert_buf);
                    vert_buf[vert_buf_pos++] = '.';
                    vert_buf[vert_buf_pos] = 0;
                    memset(&vert_buf[vert_buf_pos], '0', num_uniform);
                    vert_buf[vert_buf_pos + num_uniform] = 0;
                    strcat_s(vert_buf, sizeof(vert_buf), ".vert");

                    strcpy_s(frag_buf, sizeof(frag_buf), sub_table->fp);
                    size_t frag_buf_pos = utf8_length(frag_buf);
                    frag_buf[frag_buf_pos++] = '.';
                    frag_buf[frag_buf_pos] = 0;
                    memset(&frag_buf[frag_buf_pos], '0', num_uniform);
                    frag_buf[frag_buf_pos + num_uniform] = 0;
                    strcat_s(frag_buf, sizeof(frag_buf), ".frag");

                    for (size_t k = 0; k < unival_count; k++) {
                        for (size_t l = 0, m = k; l < num_uniform; l++) {
                            size_t unival_max = (size_t)(shader->use_permut[l]
                                ? max(vp_unival_max[l], fp_unival_max[l]) : 0) + 1;
                            vec_vert[l] = (uint32_t)(min(m % unival_max, vp_unival_max[l]));
                            m /= unival_max;
                            vert_buf[vert_buf_pos + l] = (char)('0' + vec_vert[l]);
                        }

                        for (size_t l = 0, m = k; l < num_uniform; l++) {
                            size_t unival_max = (size_t)(shader->use_permut[l]
                                ? max(vp_unival_max[l], fp_unival_max[l]) : 0) + 1;
                            vec_frag[l] = (uint32_t)(min(m % unival_max, fp_unival_max[l]));
                            m /= unival_max;
                            frag_buf[frag_buf_pos + l] = (char)('0' + vec_frag[l]);
                        }

                        if (!bin || !shader_load_binary_shader(bin, &sub->program[k])) {
                            bool vert_succ = shader_parse_define(vert_data,
                                num_uniform, vp_unival_max, fp_unival_max,
                                vec_vert.data(), &temp_vert, &temp_vert_size);
                            bool frag_succ = shader_parse_define(frag_data,
                                num_uniform, vp_unival_max, fp_unival_max,
                                vec_frag.data(), &temp_frag, &temp_frag_size);

                            //printf("%s %s\n", vert_buf, frag_buf);
                            if (ignore_cache)
                                sub->program[k] = shader_compile(vert_succ ? temp_vert : 0,
                                    frag_succ ? temp_frag : 0, vert_buf, frag_buf);
                            else {
                                program_data_binary.push_back({});
                                sub->program[k] = shader_compile_binary(vert_succ ? temp_vert : 0,
                                    frag_succ ? temp_frag : 0, vert_buf, frag_buf,
                                    &program_data_binary.back(), &buffer_size, &binary);
                            }
                            shader_cache_changed |= sub->program[k] ? true : false;
                        }
                        else {
                            program_data_binary.push_back({});
                            program_binary* b = &program_data_binary.back();
                            b->length = bin->length;
                            b->binary_format = bin->binary_format;
                            b->binary = (size_t)force_malloc(bin->length);
                            memcpy((void*)b->binary, (void*)((size_t)bin + bin->binary), bin->length);
                        }

                        if (!ignore_cache && bin)
                            bin++;
                    }
                }
            }
            else {
                program_data_binary.reserve(1);
                sub->program = force_malloc_s(GLuint, 1);
                if (sub->program) {
                    char vert_buf[MAX_PATH];
                    char frag_buf[MAX_PATH];
                    strcpy_s(vert_buf, sizeof(vert_buf), sub_table->vp);
                    strcpy_s(frag_buf, sizeof(vert_buf), sub_table->fp);
                    strcat_s(vert_buf, sizeof(vert_buf), "..vert");
                    strcat_s(frag_buf, sizeof(vert_buf), "..frag");

                    if (!bin || !shader_load_binary_shader(bin, &sub->program[0])) {
                        bool vert_succ = shader_parse_define(vert_data,
                            0, 0, 0, 0, &temp_vert, &temp_vert_size);
                        bool frag_succ = shader_parse_define(frag_data,
                            0, 0, 0, 0, &temp_frag, &temp_frag_size);

                        //printf("%s %s\n", vert_buf, frag_buf);
                        if (ignore_cache)
                            sub->program[0] = shader_compile(vert_succ ? temp_vert : 0,
                                frag_succ ? temp_frag : 0, vert_buf, frag_buf);
                        else {
                            program_data_binary.push_back({});
                            sub->program[0] = shader_compile_binary(vert_succ ? temp_vert : 0,
                                frag_succ ? temp_frag : 0, vert_buf, frag_buf,
                                &program_data_binary.back(), &buffer_size, &binary);
                        }
                        shader_cache_changed |= sub->program[0] ? true : false;
                    }
                    else {
                        program_data_binary.push_back({});
                        program_binary* b = &program_data_binary.back();
                        b->length = bin->length;
                        b->binary_format = bin->binary_format;
                        b->binary = (size_t)force_malloc(bin->length);
                        memcpy((void*)b->binary, (void*)((size_t)bin + bin->binary), bin->length);
                    }

                    if (!ignore_cache && bin)
                        bin++;
                }
            }

            if (!ignore_cache) {
                if (!shader_cache_file) {
                    shader_cache_farc.add_file(shader_cache_file_name);
                    shader_cache_file = &shader_cache_farc.files.back();
                }
                else
                    free(shader_cache_file->data);

                size_t bin_count = program_data_binary.size();
                size_t bin_size = sizeof(uint64_t) * 2 + bin_count * sizeof(program_binary);
                for (program_binary& k : program_data_binary)
                    bin_size += k.length;
                shader_cache_file->data = force_malloc(bin_size);
                shader_cache_file->size = bin_size;
                shader_cache_file->data_changed = true;

                ((uint64_t*)shader_cache_file->data)[0] = vert_data_hash;
                ((uint64_t*)shader_cache_file->data)[1] = frag_data_hash;
                bin = (program_binary*)&((uint64_t*)shader_cache_file->data)[2];
                size_t bin_data_base = (size_t)shader_cache_file->data + sizeof(uint64_t) * 2;
                size_t bin_data = bin_data_base + bin_count * sizeof(program_binary);
                for (program_binary& k : program_data_binary) {
                    bin->length = k.length;
                    bin->binary_format = k.binary_format;
                    bin->binary = bin_data - bin_data_base;
                    memcpy((void*)bin_data, (void*)k.binary, k.length);
                    bin_data_base += sizeof(program_binary);
                    bin_data += k.length;
                    void* binary = (void*)k.binary;
                    free(binary);
                    k.binary = 0;
                    bin++;
                }
            }

            free(vert_data);
            free(frag_data);
            program_data_binary.clear();
        }
        vec_vert.clear();
        vec_frag.clear();

        for (size_t j = 0; j < bind_func_table_size; j++)
            if (shader->index == bind_func_table[j].index) {
                shader->bind_func = bind_func_table[j].bind_func;
                break;
            }
    }
    free(binary);
    free(temp_vert);
    free(temp_frag);

    if (shader_cache_changed)
        shader_cache_farc.write(temp_buf, FARC_COMPRESS_FArC, false);

    memset(&set->data, 0, sizeof(set->data));
    glGenBuffers(1, &set->data.state_ubo);
    gl_state_bind_uniform_buffer(set->data.state_ubo);
    glBufferData(GL_UNIFORM_BUFFER,
        sizeof(set->data.state), (void*)&set->data.state, GL_DYNAMIC_DRAW);
    gl_state_bind_uniform_buffer(0);

    glGenBuffers(1, &set->data.state_matrix_ubo);
    gl_state_bind_uniform_buffer(set->data.state_matrix_ubo);
    glBufferData(GL_UNIFORM_BUFFER,
        sizeof(set->data.state_matrix), (void*)&set->data.state_matrix, GL_DYNAMIC_DRAW);
    gl_state_bind_uniform_buffer(0);

    glGenBuffers(1, &set->data.env_ubo);
    gl_state_bind_uniform_buffer(set->data.env_ubo);
    glBufferData(GL_UNIFORM_BUFFER,
        sizeof(set->data.env), (void*)&set->data.env, GL_DYNAMIC_DRAW);
    gl_state_bind_uniform_buffer(0);

    glGenBuffers(1, &set->data.buffer_ubo);
    gl_state_bind_uniform_buffer(set->data.buffer_ubo);
    glBufferData(GL_UNIFORM_BUFFER,
        sizeof(set->data.buffer), (void*)&set->data.buffer, GL_DYNAMIC_DRAW);
    gl_state_bind_uniform_buffer(0);
}

void shader_free(shader_set_data* set) {
    glDeleteBuffers(1, &set->data.state_ubo);
    glDeleteBuffers(1, &set->data.state_matrix_ubo);
    glDeleteBuffers(1, &set->data.env_ubo);
    glDeleteBuffers(1, &set->data.buffer_ubo);
    memset(&set->data, 0, sizeof(set->data));

    size_t size = set->size;
    for (size_t i = 0; i < size; i++) {
        shader* shader = &set->shaders[i];
        int32_t num_sub = shader->num_sub;
        shader_sub* sub = shader->sub;
        for (size_t j = 0; j < num_sub; j++, sub++) {
            if (shader->num_uniform > 0) {
                int32_t num_uniform = shader->num_uniform;
                size_t unival_curr = 1;
                size_t unival_count = 1;
                const int32_t* vp_unival_max = sub->vp_unival_max;
                const int32_t* fp_unival_max = sub->fp_unival_max;
                for (size_t k = 0; k < num_uniform; k++) {
                    size_t unival_max = shader->use_permut[k]
                        ? max(vp_unival_max[k], fp_unival_max[k]) : 0;
                    unival_count += unival_curr * unival_max;
                    unival_curr *= unival_max + 1;
                }

                if (sub->program)
                    for (size_t k = 0; k < unival_count; k++)
                        glDeleteProgram(sub->program[k]);
                free(sub->program);
            }
            else {
                if (sub->program)
                    glDeleteProgram(sub->program[0]);
                free(sub->program);
            }
        }
        free(shader->sub);
    }
    free(set->shaders);
}

void shader_set(shader_set_data* set, uint32_t index) {
    if (set && index && set->shaders && index != -1) {
        shader_env_frag_set(set, 0x18,
            (float_t)uniform_value[U_TEXTURE_BLEND], 0.0, 0.0, 0.0);
        shader* shader = &set->shaders[index];
        if (shader->bind_func)
            shader->bind_func(set, shader);
        else
            shader_bind(shader, shader->sub[0].sub_index);
        gl_state_bind_uniform_buffer_base(0, set->data.state_ubo);
        gl_state_bind_uniform_buffer_base(1, set->data.state_matrix_ubo);
        gl_state_bind_uniform_buffer_base(2, set->data.env_ubo);
        gl_state_bind_uniform_buffer_base(3, set->data.buffer_ubo);
    }
    else
        shader_unbind();
}

inline void shader_unbind() {
    gl_state_use_program(0);
    gl_state_bind_uniform_buffer_base(0, 0);
    gl_state_bind_uniform_buffer_base(1, 0);
    gl_state_bind_uniform_buffer_base(2, 0);
    gl_state_bind_uniform_buffer_base(3, 0);
}

inline void shader_buffer_get_ptr(shader_set_data* set,
    size_t index, vec4* data) {
    if (!set || index >= SHADER_MAX_PROGRAM_BUFFER_PARAMETERS || !data)
        return;

    memcpy(data, &set->data.buffer.buffer[index], sizeof(vec4));
}

inline void shader_buffer_get_ptr_array(shader_set_data* set,
    size_t index, size_t count, vec4* data) {
    if (!set || index >= SHADER_MAX_PROGRAM_BUFFER_PARAMETERS
        || index + count > SHADER_MAX_PROGRAM_BUFFER_PARAMETERS || !data)
        return;

    memcpy(data, &set->data.buffer.buffer[index], sizeof(vec4) * count);
}

inline void shader_local_frag_get(shader_set_data* set,
    size_t index, float_t* x, float_t* y, float_t* z, float_t* w) {
    if (!set || index >= SHADER_MAX_PROGRAM_LOCAL_PARAMETERS || !x || !y || !z || !w)
        return;

    GLfloat temp[4];
    glGetUniformfv(gl_state_get_program(), (GLint)(index + SHADER_MAX_PROGRAM_LOCAL_PARAMETERS), temp);
    *x = temp[0];
    *y = temp[1];
    *z = temp[2];
    *w = temp[3];
}

inline void shader_local_frag_get_ptr(shader_set_data* set,
    size_t index, vec4* data) {
    if (!set || index >= SHADER_MAX_PROGRAM_LOCAL_PARAMETERS || !data)
        return;

    GLfloat temp[4];
    glGetUniformfv(gl_state_get_program(), (GLint)(index + SHADER_MAX_PROGRAM_LOCAL_PARAMETERS), temp);
    data->x = temp[0];
    data->y = temp[1];
    data->z = temp[2];
    data->w = temp[3];
}

inline void shader_local_vert_get(shader_set_data* set,
    size_t index, float_t* x, float_t* y, float_t* z, float_t* w) {
    if (!set || index >= SHADER_MAX_PROGRAM_LOCAL_PARAMETERS || !x || !y || !z || !w)
        return;

    GLfloat temp[4];
    glGetUniformfv(gl_state_get_program(), (GLint)index, temp);
    *x = temp[0];
    *y = temp[1];
    *z = temp[2];
    *w = temp[3];
}

inline void shader_local_vert_get_ptr(shader_set_data* set,
    size_t index, vec4* data) {
    if (!set || index >= SHADER_MAX_PROGRAM_LOCAL_PARAMETERS || !data)
        return;

    GLfloat temp[4];
    glGetUniformfv(gl_state_get_program(), (GLint)index, temp);
    data->x = temp[0];
    data->y = temp[1];
    data->z = temp[2];
    data->w = temp[3];
}

inline void shader_env_frag_get(shader_set_data* set,
    size_t index, float_t* x, float_t* y, float_t* z, float_t* w) {
    if (!set || index >= SHADER_MAX_PROGRAM_ENV_PARAMETERS || !x || !y || !z || !w)
        return;

    vec4 data = set->data.env.frag[index];
    *x = data.x;
    *y = data.y;
    *z = data.z;
    *w = data.w;
}

inline void shader_env_frag_get_ptr(shader_set_data* set,
    size_t index, vec4* data) {
    if (!set || index >= SHADER_MAX_PROGRAM_ENV_PARAMETERS || !data)
        return;

    *data = set->data.env.frag[index];
}

inline void shader_env_frag_get_ptr_array(shader_set_data* set,
    size_t index, size_t count, vec4* data) {
    if (!set || index >= SHADER_MAX_PROGRAM_ENV_PARAMETERS
        || index + count > SHADER_MAX_PROGRAM_ENV_PARAMETERS || !data)
        return;

    memcpy(data, &set->data.env.frag[index], sizeof(vec4) * count);
}

inline void shader_env_vert_get(shader_set_data* set,
    size_t index, float_t* x, float_t* y, float_t* z, float_t* w) {
    if (!set || index >= SHADER_MAX_PROGRAM_ENV_PARAMETERS || !x || !y || !z || !w)
        return;

    vec4 data = set->data.env.vert[index];
    *x = data.x;
    *y = data.y;
    *z = data.z;
    *w = data.w;
}

inline void shader_env_vert_get_ptr(shader_set_data* set,
    size_t index, vec4* data) {
    if (!set || index >= SHADER_MAX_PROGRAM_ENV_PARAMETERS || !data)
        return;

    *data = set->data.env.vert[index];
}

inline void shader_env_vert_get_ptr_array(shader_set_data* set,
    size_t index, size_t count, vec4* data) {
    if (!set || index >= SHADER_MAX_PROGRAM_ENV_PARAMETERS
        || index + count > SHADER_MAX_PROGRAM_ENV_PARAMETERS || !data)
        return;

    memcpy(data, &set->data.env.vert[index], sizeof(vec4) * count);
}

inline void shader_state_clip_get_plane(shader_set_data* set,
    size_t index, vec4* data) {
    if (!set || index >= SHADER_MAX_CLIP_PLANES || !data)
        return;

    *data = set->data.state.clip[index].plane;
}

inline void shader_state_depth_get_range(shader_set_data* set,
    vec4* data) {
    if (!set || !data)
        return;

    *data = set->data.state.depth.range;
}

inline void shader_state_fog_get_color(shader_set_data* set,
    vec4* data) {
    if (!set || !data)
        return;

    *data = set->data.state.fog.color;
}

inline void shader_state_fog_get_params(shader_set_data* set,
    vec4* data) {
    if (!set || !data)
        return;

    *data = set->data.state.fog.params;
}

inline void shader_state_light_get_ambient(shader_set_data* set,
    size_t index, vec4* data) {
    if (!set || index >= SHADER_MAX_LIGHTS || !data)
        return;

    *data = set->data.state.light[index].ambient;
}

inline void shader_state_light_get_diffuse(shader_set_data* set,
    size_t index, vec4* data) {
    if (!set || index >= SHADER_MAX_LIGHTS || !data)
        return;

    *data = set->data.state.light[index].ambient;
}

inline void shader_state_light_get_specular(shader_set_data* set,
    size_t index, vec4* data) {
    if (!set || index >= SHADER_MAX_LIGHTS || !data)
        return;

    *data = set->data.state.light[index].ambient;
}

inline void shader_state_light_get_position(shader_set_data* set,
    size_t index, vec4* data) {
    if (!set || index >= SHADER_MAX_LIGHTS || !data)
        return;

    *data = set->data.state.light[index].ambient;
}

inline void shader_state_light_get_attenuation(shader_set_data* set,
    size_t index, vec4* data) {
    if (!set || index >= SHADER_MAX_LIGHTS || !data)
        return;

    *data = set->data.state.light[index].ambient;
}

inline void shader_state_light_get_spot_direction(shader_set_data* set,
    size_t index, vec4* data) {
    if (!set || index >= SHADER_MAX_LIGHTS || !data)
        return;

    *data = set->data.state.light[index].ambient;
}

inline void shader_state_light_get_half(shader_set_data* set,
    size_t index, vec4* data) {
    if (!set || index >= SHADER_MAX_LIGHTS || !data)
        return;

    *data = set->data.state.light[index].ambient;
}

inline void shader_state_lightmodel_get_ambient(shader_set_data* set,
    bool back, vec4* data) {
    if (!set || !data)
        return;

    *data = set->data.state.lightmodel[back ? 1 : 0].ambient;
}

inline void shader_state_lightmodel_get_scene_color(shader_set_data* set,
    bool back, vec4* data) {
    if (!set || !data)
        return;

    *data = set->data.state.lightmodel[back ? 1 : 0].scene_color;
}

inline void shader_state_lightprod_get_ambient(shader_set_data* set,
    bool back, size_t index, vec4* data) {
    if (!set || index >= SHADER_MAX_LIGHTS || !data)
        return;

    shader_state_lightprod* lightprod = back
        ? set->data.state.lightprod_back : set->data.state.lightprod_front;
    *data = lightprod[index].ambient;
}

inline void shader_state_lightprod_get_diffuse(shader_set_data* set,
    bool back, size_t index, vec4* data) {
    if (!set || index >= SHADER_MAX_LIGHTS || !data)
        return;

    shader_state_lightprod* lightprod = back
        ? set->data.state.lightprod_back : set->data.state.lightprod_front;
    *data = lightprod[index].diffuse;
}

inline void shader_state_lightprod_get_specular(shader_set_data* set,
    bool back, size_t index, vec4* data) {
    if (!set || index >= SHADER_MAX_LIGHTS || !data)
        return;

    shader_state_lightprod* lightprod = back
        ? set->data.state.lightprod_back : set->data.state.lightprod_front;
    *data = lightprod[index].specular;
}

inline void shader_state_material_get_ambient(shader_set_data* set,
    bool back, vec4* data) {
    if (!set || !data)
        return;

    *data = set->data.state.material[back ? 1 : 0].ambient;
}

inline void shader_state_material_get_diffuse(shader_set_data* set,
    bool back, vec4* data) {
    if (!set || !data)
        return;

    *data = set->data.state.material[back ? 1 : 0].diffuse;
}

inline void shader_state_material_get_specular(shader_set_data* set,
    bool back, vec4* data) {
    if (!set || !data)
        return;

    *data = set->data.state.material[back ? 1 : 0].specular;
}

inline void shader_state_material_get_emission(shader_set_data* set,
    bool back, vec4* data) {
    if (!set || !data)
        return;

    *data = set->data.state.material[back ? 1 : 0].emission;
}

inline void shader_state_material_get_shininess(shader_set_data* set,
    bool back, vec4* data) {
    if (!set || !data)
        return;

    *data = set->data.state.material[back ? 1 : 0].shininess;
}

inline void shader_state_matrix_get_modelview(shader_set_data* set,
    size_t index, mat4* data) {
    if (!set || index >= SHADER_MAX_VERTEX_UNITS || !data)
        return;

    *data = set->data.state_matrix.modelview[index].mat;
}

inline void shader_state_matrix_get_projection(shader_set_data* set,
    mat4* data) {
    if (!set || !data)
        return;

    *data = set->data.state_matrix.projection.mat;
}

inline void shader_state_matrix_get_mvp(shader_set_data* set,
    mat4* data) {
    if (!set || !data)
        return;

    *data = set->data.state_matrix.mvp.mat;
}

inline void shader_state_matrix_get_texture(shader_set_data* set,
    size_t index, mat4* data) {
    if (!set || index >= SHADER_MAX_TEXTURE_COORDS || !data)
        return;

    *data = set->data.state_matrix.texture[index].mat;
}

inline void shader_state_matrix_get_palette(shader_set_data* set,
    size_t index, mat4* data) {
    if (!set || index >= SHADER_MAX_PALETTE_MATRICES || !data)
        return;

    *data = set->data.state_matrix.palette[index].mat;
}

inline void shader_state_matrix_get_program(shader_set_data* set,
    size_t index, mat4* data) {
    if (!set || index >= SHADER_MAX_PROGRAM_MATRICES || !data)
        return;

    *data = set->data.state_matrix.program[index].mat;
}

inline void shader_state_point_get_size(shader_set_data* set,
    vec4* data) {
    if (!set || !data)
        return;

    *data = set->data.state.point.size;
}

inline void shader_state_point_get_attenuation(shader_set_data* set,
    vec4* data) {
    if (!set || !data)
        return;

    *data = set->data.state.point.attenuation;
}

inline void shader_state_texgen_get_eye_s(shader_set_data* set,
    size_t index, vec4* data) {
    if (!set || index >= SHADER_MAX_TEXTURE_UNITS || !data)
        return;

    *data = set->data.state.texgen[index].eye_s;
}

inline void shader_state_texgen_get_eye_t(shader_set_data* set,
    size_t index, vec4* data) {
    if (!set || index >= SHADER_MAX_TEXTURE_UNITS || !data)
        return;

    *data = set->data.state.texgen[index].eye_t;
}

inline void shader_state_texgen_get_eye_r(shader_set_data* set,
    size_t index, vec4* data) {
    if (!set || index >= SHADER_MAX_TEXTURE_UNITS || !data)
        return;

    *data = set->data.state.texgen[index].eye_r;
}

inline void shader_state_texgen_get_eye_q(shader_set_data* set,
    size_t index, vec4* data) {
    if (!set || index >= SHADER_MAX_TEXTURE_UNITS || !data)
        return;

    *data = set->data.state.texgen[index].eye_q;
}

inline void shader_state_texgen_get_object_s(shader_set_data* set,
    size_t index, vec4* data) {
    if (!set || index >= SHADER_MAX_TEXTURE_UNITS || !data)
        return;

    *data = set->data.state.texgen[index].object_s;
}

inline void shader_state_texgen_get_object_t(shader_set_data* set,
    size_t index, vec4* data) {
    if (!set || index >= SHADER_MAX_TEXTURE_UNITS || !data)
        return;

    *data = set->data.state.texgen[index].object_t;
}

inline void shader_state_texgen_get_object_r(shader_set_data* set,
    size_t index, vec4* data) {
    if (!set || index >= SHADER_MAX_TEXTURE_UNITS || !data)
        return;

    *data = set->data.state.texgen[index].object_r;
}

inline void shader_state_texgen_get_object_q(shader_set_data* set,
    size_t index, vec4* data) {
    if (!set || index >= SHADER_MAX_TEXTURE_UNITS || !data)
        return;

    *data = set->data.state.texgen[index].object_q;
}

inline void shader_state_texenv_get_color(shader_set_data* set,
    size_t index, vec4* data) {
    if (!set || index >= SHADER_MAX_TEXTURE_UNITS || !data)
        return;

    *data = set->data.state.texenv[index].color;
}

inline void shader_buffer_set_ptr(shader_set_data* set,
    size_t index, const vec4* data) {
    if (!set || index >= SHADER_MAX_PROGRAM_BUFFER_PARAMETERS || !data)
        return;

    if (!memcmp(&set->data.buffer.buffer[index], data, sizeof(vec4)))
        return;

    set->data.buffer.buffer[index] = *data;
    set->data.buffer_update_data = true;
}

inline void shader_buffer_set_ptr_array(shader_set_data* set,
    size_t index, size_t count, const vec4* data) {
    if (!set || index >= SHADER_MAX_PROGRAM_BUFFER_PARAMETERS
        || index + count > SHADER_MAX_PROGRAM_BUFFER_PARAMETERS || !data)
        return;

    if (!memcmp(&set->data.buffer.buffer[index], data, sizeof(vec4) * count))
        return;

    memcpy(&set->data.buffer.buffer[index], data, sizeof(vec4) * count);
    set->data.buffer_update_data = true;
}

inline void shader_local_frag_set(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!set || index >= SHADER_MAX_PROGRAM_LOCAL_PARAMETERS)
        return;

    glUniform4f((GLint)(index + SHADER_MAX_PROGRAM_LOCAL_PARAMETERS), x, y, z, w);
}

inline void shader_local_frag_set_ptr(shader_set_data* set,
    size_t index, const vec4* data) {
    if (!set || index >= SHADER_MAX_PROGRAM_LOCAL_PARAMETERS || !data)
        return;

    glUniform4f((GLint)(index + SHADER_MAX_PROGRAM_LOCAL_PARAMETERS), data->x, data->y, data->z, data->w);
}

inline void shader_local_frag_set_ptr_array(shader_set_data* set,
    size_t index, size_t count, const vec4* data) {
    if (!set || index >= SHADER_MAX_PROGRAM_LOCAL_PARAMETERS
        || index + count > SHADER_MAX_PROGRAM_LOCAL_PARAMETERS || !data)
        return;

    glUniform4fv((GLint)(index + SHADER_MAX_PROGRAM_LOCAL_PARAMETERS),
        (GLsizei)count, (const GLfloat*)data);
}

inline void shader_local_vert_set(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!set || index >= SHADER_MAX_PROGRAM_LOCAL_PARAMETERS)
        return;

    glUniform4f((GLint)index, x, y, z, w);
}

inline void shader_local_vert_set_ptr(shader_set_data* set,
    size_t index, const vec4* data) {
    if (!set || index >= SHADER_MAX_PROGRAM_LOCAL_PARAMETERS || !data)
        return;

    glUniform4f((GLint)index, data->x, data->y, data->z, data->w);
}

inline void shader_local_vert_set_ptr_array(shader_set_data* set,
    size_t index, size_t count, const vec4* data) {
    if (!set || index >= SHADER_MAX_PROGRAM_LOCAL_PARAMETERS
        || index + count > SHADER_MAX_PROGRAM_LOCAL_PARAMETERS || !data)
        return;

    glUniform4fv((GLint)index, (GLsizei)count, (const GLfloat*)data);
}

inline void shader_env_frag_set(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!set || index >= SHADER_MAX_PROGRAM_ENV_PARAMETERS)
        return;

    vec4 data;
    data.x = x;
    data.y = y;
    data.z = z;
    data.w = w;
    if (!memcmp(&set->data.env.frag[index], &data, sizeof(vec4)))
        return;

    set->data.env.frag[index] = data;
    set->data.env_update_data = true;
}

inline void shader_env_frag_set_ptr(shader_set_data* set,
    size_t index, const vec4* data) {
    if (!set || index >= SHADER_MAX_PROGRAM_ENV_PARAMETERS || !data)
        return;

    if (!memcmp(&set->data.env.frag[index], data, sizeof(vec4)))
        return;

    set->data.env.frag[index] = *data;
    set->data.env_update_data = true;
}

inline void shader_env_frag_set_ptr_array(shader_set_data* set,
    size_t index, size_t count, const vec4* data) {
    if (!set || index >= SHADER_MAX_PROGRAM_ENV_PARAMETERS
        || index + count > SHADER_MAX_PROGRAM_ENV_PARAMETERS || !data)
        return;

    if (!memcmp(&set->data.env.frag[index], data, sizeof(vec4) * count))
        return;

    memcpy(&set->data.env.frag[index], data, sizeof(vec4) * count);
    set->data.env_update_data = true;
}

inline void shader_env_vert_set(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!set || index >= SHADER_MAX_PROGRAM_ENV_PARAMETERS)
        return;

    vec4 data;
    data.x = x;
    data.y = y;
    data.z = z;
    data.w = w;
    if (!memcmp(&set->data.env.vert[index], &data, sizeof(vec4)))
        return;

    set->data.env.vert[index] = data;
    set->data.env_update_data = true;
}

inline void shader_env_vert_set_ptr(shader_set_data* set,
    size_t index, const vec4* data) {
    if (!set || index >= SHADER_MAX_PROGRAM_ENV_PARAMETERS || !data)
        return;

    if (!memcmp(&set->data.env.vert[index], data, sizeof(vec4)))
        return;

    set->data.env.vert[index] = *data;
    set->data.env_update_data = true;
}

inline void shader_env_vert_set_ptr_array(shader_set_data* set,
    size_t index, size_t count, const vec4* data) {
    if (!set || index >= SHADER_MAX_PROGRAM_ENV_PARAMETERS
        || index + count > SHADER_MAX_PROGRAM_ENV_PARAMETERS || !data)
        return;

    if (!memcmp(&set->data.env.vert[index], data, sizeof(vec4) * count))
        return;

    memcpy(&set->data.env.vert[index], data, sizeof(vec4) * count);
    set->data.env_update_data = true;
}

inline void shader_state_clip_set_plane(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!set || index >= SHADER_MAX_CLIP_PLANES)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&set->data.state.clip[index].plane, &data, sizeof(vec4)))
        return;

    set->data.state.clip[index].plane = data;
    set->data.state_update_data = true;
}

inline void shader_state_clip_set_plane_ptr(shader_set_data* set,
    size_t index, const vec4* data) {
    if (!set || index >= SHADER_MAX_CLIP_PLANES || !data)
        return;

    if (!memcmp(&set->data.state.clip[index].plane, data, sizeof(vec4)))
        return;

    set->data.state.clip[index].plane = *data;
    set->data.state_update_data = true;
}

inline void shader_state_depth_set_range(shader_set_data* set,
    float_t x, float_t y, float_t z, float_t w) {
    if (!set)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&set->data.state.depth.range, &data, sizeof(vec4)))
        return;

    set->data.state.depth.range = data;
    set->data.state_update_data = true;
}

inline void shader_state_depth_set_range_ptr(shader_set_data* set,
    const vec4* data) {
    if (!set || !data)
        return;

    if (!memcmp(&set->data.state.depth.range, data, sizeof(vec4)))
        return;

    set->data.state.depth.range = *data;
    set->data.state_update_data = true;
}

inline void shader_state_fog_set_color(shader_set_data* set,
    float_t x, float_t y, float_t z, float_t w) {
    if (!set)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&set->data.state.fog.color, &data, sizeof(vec4)))
        return;

    set->data.state.fog.color = data;
    set->data.state_update_data = true;
}

inline void shader_state_fog_set_color_ptr(shader_set_data* set,
    const vec4* data) {
    if (!set || !data)
        return;

    if (!memcmp(&set->data.state.fog.color, data, sizeof(vec4)))
        return;

    set->data.state.fog.color = *data;
    set->data.state_update_data = true;
}

inline void shader_state_fog_set_params(shader_set_data* set,
    float_t x, float_t y, float_t z, float_t w) {
    if (!set)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&set->data.state.fog.params, &data, sizeof(vec4)))
        return;

    set->data.state.fog.params = data;
    set->data.state_update_data = true;
}

inline void shader_state_fog_set_params_ptr(shader_set_data* set,
    const vec4* data) {
    if (!set || !data)
        return;

    if (!memcmp(&set->data.state.fog.params, data, sizeof(vec4)))
        return;

    set->data.state.fog.params = *data;
    set->data.state_update_data = true;
}

inline void shader_state_light_set_ambient(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!set || index >= SHADER_MAX_LIGHTS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&set->data.state.light[index].ambient, &data, sizeof(vec4)))
        return;

    set->data.state.light[index].ambient = data;
    set->data.state_update_data = true;

    vec4 lightprod_front;
    vec4 lightprod_back;
    vec4_mult(set->data.state.light[index].ambient,
        set->data.state.material[0].ambient, lightprod_front);
    vec4_mult(set->data.state.light[index].ambient,
        set->data.state.material[1].ambient, lightprod_back);
    shader_state_lightprod_set_ambient_ptr(set, false, index, &lightprod_front);
    shader_state_lightprod_set_ambient_ptr(set, true, index, &lightprod_back);
}

inline void shader_state_light_set_ambient_ptr(shader_set_data* set,
    size_t index, const vec4* data) {
    if (!set || index >= SHADER_MAX_LIGHTS || !data)
        return;

    if (!memcmp(&set->data.state.light[index].ambient, data, sizeof(vec4)))
        return;

    set->data.state.light[index].ambient = *data;
    set->data.state_update_data = true;

    vec4 lightprod_front;
    vec4 lightprod_back;
    vec4_mult(set->data.state.light[index].ambient,
        set->data.state.material[0].ambient, lightprod_front);
    vec4_mult(set->data.state.light[index].ambient,
        set->data.state.material[1].ambient, lightprod_back);
    shader_state_lightprod_set_ambient_ptr(set, false, index, &lightprod_front);
    shader_state_lightprod_set_ambient_ptr(set, true, index, &lightprod_back);
}

inline void shader_state_light_set_diffuse(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!set || index >= SHADER_MAX_LIGHTS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&set->data.state.light[index].diffuse, &data, sizeof(vec4)))
        return;

    set->data.state.light[index].diffuse = data;
    set->data.state_update_data = true;

    vec4 lightprod_front;
    vec4 lightprod_back;
    vec4_mult(set->data.state.light[index].diffuse,
        set->data.state.material[0].diffuse, lightprod_front);
    vec4_mult(set->data.state.light[index].diffuse,
        set->data.state.material[1].diffuse, lightprod_back);
    shader_state_lightprod_set_diffuse_ptr(set, false, index, &lightprod_front);
    shader_state_lightprod_set_diffuse_ptr(set, true, index, &lightprod_back);
}

inline void shader_state_light_set_diffuse_ptr(shader_set_data* set,
    size_t index, const vec4* data) {
    if (!set || index >= SHADER_MAX_LIGHTS || !data)
        return;

    if (!memcmp(&set->data.state.light[index].diffuse, data, sizeof(vec4)))
        return;

    set->data.state.light[index].diffuse = *data;
    set->data.state_update_data = true;

    vec4 lightprod_front;
    vec4 lightprod_back;
    vec4_mult(set->data.state.light[index].diffuse,
        set->data.state.material[0].diffuse, lightprod_front);
    vec4_mult(set->data.state.light[index].diffuse,
        set->data.state.material[1].diffuse, lightprod_back);
    shader_state_lightprod_set_diffuse_ptr(set, false, index, &lightprod_front);
    shader_state_lightprod_set_diffuse_ptr(set, true, index, &lightprod_back);
}

inline void shader_state_light_set_specular(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!set || index >= SHADER_MAX_LIGHTS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&set->data.state.light[index].specular, &data, sizeof(vec4)))
        return;

    set->data.state.light[index].specular = data;
    set->data.state_update_data = true;

    vec4 lightprod_front;
    vec4 lightprod_back;
    vec4_mult(set->data.state.light[index].specular,
        set->data.state.material[0].specular, lightprod_front);
    vec4_mult(set->data.state.light[index].specular,
        set->data.state.material[1].specular, lightprod_back);
    shader_state_lightprod_set_specular_ptr(set, false, index, &lightprod_front);
    shader_state_lightprod_set_specular_ptr(set, true, index, &lightprod_back);
}

inline void shader_state_light_set_specular_ptr(shader_set_data* set,
    size_t index, const vec4* data) {
    if (!set || index >= SHADER_MAX_LIGHTS || !data)
        return;

    if (!memcmp(&set->data.state.light[index].specular, data, sizeof(vec4)))
        return;

    set->data.state.light[index].specular = *data;
    set->data.state_update_data = true;

    vec4 lightprod_front;
    vec4 lightprod_back;
    vec4_mult(set->data.state.light[index].specular,
        set->data.state.material[0].specular, lightprod_front);
    vec4_mult(set->data.state.light[index].specular,
        set->data.state.material[1].specular, lightprod_back);
    shader_state_lightprod_set_specular_ptr(set, false, index, &lightprod_front);
    shader_state_lightprod_set_specular_ptr(set, true, index, &lightprod_back);
}

inline void shader_state_light_set_position(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!set || index >= SHADER_MAX_LIGHTS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&set->data.state.light[index].position, &data, sizeof(vec4)))
        return;

    set->data.state.light[index].position = data;
    set->data.state_update_data = true;
}

inline void shader_state_light_set_position_ptr(shader_set_data* set,
    size_t index, const vec4* data) {
    if (!set || index >= SHADER_MAX_LIGHTS || !data)
        return;

    if (!memcmp(&set->data.state.light[index].position, data, sizeof(vec4)))
        return;

    set->data.state.light[index].position = *data;
    set->data.state_update_data = true;
}

inline void shader_state_light_set_attenuation(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!set || index >= SHADER_MAX_LIGHTS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&set->data.state.light[index].attenuation, &data, sizeof(vec4)))
        return;

    set->data.state.light[index].attenuation = data;
    set->data.state_update_data = true;
}

inline void shader_state_light_set_attenuation_ptr(shader_set_data* set,
    size_t index, const vec4* data) {
    if (!set || index >= SHADER_MAX_LIGHTS || !data)
        return;

    if (!memcmp(&set->data.state.light[index].attenuation, data, sizeof(vec4)))
        return;

    set->data.state.light[index].attenuation = *data;
    set->data.state_update_data = true;
}

inline void shader_state_light_set_spot_direction(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!set || index >= SHADER_MAX_LIGHTS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&set->data.state.light[index].spot_direction, &data, sizeof(vec4)))
        return;

    set->data.state.light[index].spot_direction = data;
    set->data.state_update_data = true;
}

inline void shader_state_light_set_spot_direction_ptr(shader_set_data* set,
    size_t index, const vec4* data) {
    if (!set || index >= SHADER_MAX_LIGHTS || !data)
        return;

    if (!memcmp(&set->data.state.light[index].spot_direction, data, sizeof(vec4)))
        return;

    set->data.state.light[index].spot_direction = *data;
    set->data.state_update_data = true;
}

inline void shader_state_light_set_half(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!set || index >= SHADER_MAX_LIGHTS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&set->data.state.light[index].half, &data, sizeof(vec4)))
        return;

    set->data.state.light[index].half = data;
    set->data.state_update_data = true;
}

inline void shader_state_light_set_half_ptr(shader_set_data* set,
    size_t index, const vec4* data) {
    if (!set || index >= SHADER_MAX_LIGHTS || !data)
        return;

    if (!memcmp(&set->data.state.light[index].half, data, sizeof(vec4)))
        return;

    set->data.state.light[index].half = *data;
    set->data.state_update_data = true;
}

inline void shader_state_lightmodel_set_ambient(shader_set_data* set,
    bool back, float_t x, float_t y, float_t z, float_t w) {
    if (!set)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&set->data.state.lightmodel[back ? 1 : 0].ambient, &data, sizeof(vec4)))
        return;

    set->data.state.lightmodel[back ? 1 : 0].ambient = data;
    set->data.state_update_data = true;
}

inline void shader_state_lightmodel_set_ambient_ptr(shader_set_data* set,
    bool back, const vec4* data) {
    if (!set || !data)
        return;

    if (!memcmp(&set->data.state.lightmodel[back ? 1 : 0].ambient, data, sizeof(vec4)))
        return;

    set->data.state.lightmodel[back ? 1 : 0].ambient = *data;
    set->data.state_update_data = true;
}

inline void shader_state_lightmodel_set_scene_color(shader_set_data* set,
    bool back, float_t x, float_t y, float_t z, float_t w) {
    if (!set)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&set->data.state.lightmodel[back ? 1 : 0].scene_color, &data, sizeof(vec4)))
        return;

    set->data.state.lightmodel[back ? 1 : 0].scene_color = data;
    set->data.state_update_data = true;
}

inline void shader_state_lightmodel_set_scene_color_ptr(shader_set_data* set,
    bool back, const vec4* data) {
    if (!set || !data)
        return;

    if (!memcmp(&set->data.state.lightmodel[back ? 1 : 0].scene_color, data, sizeof(vec4)))
        return;

    set->data.state.lightmodel[back ? 1 : 0].scene_color = *data;
    set->data.state_update_data = true;
}

inline void shader_state_lightprod_set_ambient(shader_set_data* set,
    bool back, size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!set || index >= SHADER_MAX_LIGHTS)
        return;

    shader_state_lightprod* lightprod = back
        ? set->data.state.lightprod_back : set->data.state.lightprod_front;

    vec4 data = { x, y, z, w };
    if (!memcmp(&lightprod[index].ambient, &data, sizeof(vec4)))
        return;

    lightprod[index].ambient = data;
    set->data.state_update_data = true;
}

inline void shader_state_lightprod_set_ambient_ptr(shader_set_data* set,
    bool back, size_t index, const vec4* data) {
    if (!set || index >= SHADER_MAX_LIGHTS || !data)
        return;

    shader_state_lightprod* lightprod = back
        ? set->data.state.lightprod_back : set->data.state.lightprod_front;

    if (!memcmp(&lightprod[index].ambient, data, sizeof(vec4)))
        return;

    lightprod[index].ambient = *data;
    set->data.state_update_data = true;
}

inline void shader_state_lightprod_set_diffuse(shader_set_data* set,
    bool back, size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!set || index >= SHADER_MAX_LIGHTS)
        return;

    shader_state_lightprod* lightprod = back
        ? set->data.state.lightprod_back : set->data.state.lightprod_front;

    vec4 data = { x, y, z, w };
    if (!memcmp(&lightprod[index].diffuse, &data, sizeof(vec4)))
        return;

    lightprod[index].diffuse = data;
    set->data.state_update_data = true;
}

inline void shader_state_lightprod_set_diffuse_ptr(shader_set_data* set,
    bool back, size_t index, const vec4* data) {
    if (!set || index >= SHADER_MAX_LIGHTS || !data)
        return;

    shader_state_lightprod* lightprod = back
        ? set->data.state.lightprod_back : set->data.state.lightprod_front;

    if (!memcmp(&lightprod[index].diffuse, data, sizeof(vec4)))
        return;

    lightprod[index].diffuse = *data;
    set->data.state_update_data = true;
}

inline void shader_state_lightprod_set_specular(shader_set_data* set,
    bool back, size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!set || index >= SHADER_MAX_LIGHTS)
        return;

    shader_state_lightprod* lightprod = back
        ? set->data.state.lightprod_back : set->data.state.lightprod_front;

    vec4 data = { x, y, z, w };
    if (!memcmp(&lightprod[index].specular, &data, sizeof(vec4)))
        return;

    lightprod[index].specular = data;
    set->data.state_update_data = true;
}

inline void shader_state_lightprod_set_specular_ptr(shader_set_data* set,
    bool back, size_t index, const vec4* data) {
    if (!set || index >= SHADER_MAX_LIGHTS || !data)
        return;

    shader_state_lightprod* lightprod = back
        ? set->data.state.lightprod_back : set->data.state.lightprod_front;

    if (!memcmp(&lightprod[index].specular, data, sizeof(vec4)))
        return;

    lightprod[index].specular = *data;
    set->data.state_update_data = true;
}

inline void shader_state_material_set_ambient(shader_set_data* set,
    bool back, float_t x, float_t y, float_t z, float_t w) {
    if (!set)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&set->data.state.material[back ? 1 : 0].ambient, &data, sizeof(vec4)))
        return;

    set->data.state.material[back ? 1 : 0].ambient = data;
    set->data.state_update_data = true;

    vec4 lightprod;
    for (int32_t index = 0; index < SHADER_MAX_LIGHTS; index++) {
        vec4_mult(set->data.state.light[index].ambient,
            set->data.state.material[back ? 1 : 0].ambient, lightprod);
        shader_state_lightprod_set_ambient_ptr(set, back, index, &lightprod);
    }
}

inline void shader_state_material_set_ambient_ptr(shader_set_data* set,
    bool back, const vec4* data) {
    if (!set || !data)
        return;

    if (!memcmp(&set->data.state.material[back ? 1 : 0].ambient, data, sizeof(vec4)))
        return;

    set->data.state.material[back ? 1 : 0].ambient = *data;
    set->data.state_update_data = true;

    vec4 lightprod;
    for (int32_t index = 0; index < SHADER_MAX_LIGHTS; index++) {
        vec4_mult(set->data.state.light[index].ambient,
            set->data.state.material[back ? 1 : 0].ambient, lightprod);
        shader_state_lightprod_set_ambient_ptr(set, back, index, &lightprod);
    }
}

inline void shader_state_material_set_diffuse(shader_set_data* set,
    bool back, float_t x, float_t y, float_t z, float_t w) {
    if (!set)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&set->data.state.material[back ? 1 : 0].diffuse, &data, sizeof(vec4)))
        return;

    set->data.state.material[back ? 1 : 0].diffuse = data;
    set->data.state_update_data = true;

    vec4 lightprod;
    for (int32_t index = 0; index < SHADER_MAX_LIGHTS; index++) {
        vec4_mult(set->data.state.light[index].diffuse,
            set->data.state.material[back ? 1 : 0].diffuse, lightprod);
        shader_state_lightprod_set_diffuse_ptr(set, back, index, &lightprod);
    }
}

inline void shader_state_material_set_diffuse_ptr(shader_set_data* set,
    bool back, const vec4* data) {
    if (!set || !data)
        return;

    if (!memcmp(&set->data.state. material[back ? 1 : 0].diffuse, data, sizeof(vec4)))
        return;

    set->data.state. material[back ? 1 : 0].diffuse = *data;
    set->data.state_update_data = true;

    vec4 lightprod;
    for (int32_t index = 0; index < SHADER_MAX_LIGHTS; index++) {
        vec4_mult(set->data.state.light[index].diffuse,
            set->data.state.material[back ? 1 : 0].diffuse, lightprod);
        shader_state_lightprod_set_diffuse_ptr(set, back, index, &lightprod);
    }
}

inline void shader_state_material_set_specular(shader_set_data* set,
    bool back, float_t x, float_t y, float_t z, float_t w) {
    if (!set)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&set->data.state.material[back ? 1 : 0].specular, &data, sizeof(vec4)))
        return;

    set->data.state.material[back ? 1 : 0].specular = data;
    set->data.state_update_data = true;

    vec4 lightprod;
    for (int32_t index = 0; index < SHADER_MAX_LIGHTS; index++) {
        vec4_mult(set->data.state.light[index].specular,
            set->data.state.material[back ? 1 : 0].specular, lightprod);
        shader_state_lightprod_set_specular_ptr(set, back, index, &lightprod);
    }
}

inline void shader_state_material_set_specular_ptr(shader_set_data* set,
    bool back, const vec4* data) {
    if (!set || !data)
        return;

    if (!memcmp(&set->data.state.material[back ? 1 : 0].specular, data, sizeof(vec4)))
        return;

    set->data.state.material[back ? 1 : 0].specular = *data;
    set->data.state_update_data = true;

    vec4 lightprod;
    for (int32_t index = 0; index < SHADER_MAX_LIGHTS; index++) {
        vec4_mult(set->data.state.light[index].specular,
            set->data.state.material[back ? 1 : 0].specular, lightprod);
        shader_state_lightprod_set_specular_ptr(set, back, index, &lightprod);
    }
}

inline void shader_state_material_set_emission(shader_set_data* set,
    bool back, float_t x, float_t y, float_t z, float_t w) {
    if (!set)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&set->data.state.material[back ? 1 : 0].emission, &data, sizeof(vec4)))
        return;

    set->data.state.material[back ? 1 : 0].emission = data;
    set->data.state_update_data = true;
}

inline void shader_state_material_set_emission_ptr(shader_set_data* set,
    bool back, const vec4* data) {
    if (!set || !data)
        return;

    if (!memcmp(&set->data.state.material[back ? 1 : 0].emission, data, sizeof(vec4)))
        return;

    set->data.state.material[back ? 1 : 0].emission = *data;
    set->data.state_update_data = true;
}

inline void shader_state_material_set_shininess(shader_set_data* set,
    bool back, float_t x, float_t y, float_t z, float_t w) {
    if (!set)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&set->data.state.material[back ? 1 : 0].shininess, &data, sizeof(vec4)))
        return;

    set->data.state.material[back ? 1 : 0].shininess = data;
    set->data.state_update_data = true;
}

inline void shader_state_material_set_shininess_ptr(shader_set_data* set,
    bool back, const vec4* data) {
    if (!set || !data)
        return;

    if (!memcmp(&set->data.state.material[back ? 1 : 0].shininess, data, sizeof(vec4)))
        return;

    set->data.state.material[back ? 1 : 0].shininess = *data;
    set->data.state_update_data = true;
}

inline void shader_state_matrix_set_modelview(shader_set_data* set,
    size_t index, const mat4* data, bool mult) {
    if (!set || index >= SHADER_MAX_VERTEX_UNITS || !data)
        return;

    if (memcmp(&set->data.state_matrix.modelview[index].mat, data, sizeof(mat4))) {
        mat4 mat;
        mat4 mat_inv;
        mat4 mat_trans;
        mat4 mat_invtrans;

        mat = *data;
        mat4_inverse(&mat, &mat_inv);
        mat4_transpose(&mat, &mat_trans);
        mat4_transpose(&mat_inv, &mat_invtrans);
        set->data.state_matrix.modelview[index].mat = mat;
        set->data.state_matrix.modelview[index].inv = mat_inv;
        set->data.state_matrix.modelview[index].trans = mat_trans;
        set->data.state_matrix.modelview[index].invtrans = mat_invtrans;
        set->data.state_matrix_update_data = true;
    }

    if (mult && index == 0) {
        mat4 mat;
        mat4_mult(&set->data.state_matrix.modelview[0].mat,
            &set->data.state_matrix.projection.mat, &mat);
        shader_state_matrix_set_mvp(set, &mat);
    }
}

inline void shader_state_matrix_set_projection(shader_set_data* set,
    const mat4* data, bool mult) {
    if (!set || !data)
        return;

    if (memcmp(&set->data.state_matrix.projection.mat, data, sizeof(mat4))) {
        mat4 mat;
        mat4 mat_inv;
        mat4 mat_trans;
        mat4 mat_invtrans;

        mat = *data;
        mat4_inverse(&mat, &mat_inv);
        mat4_transpose(&mat, &mat_trans);
        mat4_transpose(&mat_inv, &mat_invtrans);
        set->data.state_matrix.projection.mat = mat;
        set->data.state_matrix.projection.inv = mat_inv;
        set->data.state_matrix.projection.trans = mat_trans;
        set->data.state_matrix.projection.invtrans = mat_invtrans;
        set->data.state_matrix_update_data = true;
    }

    if (mult) {
        mat4 mat;
        mat4_mult(&set->data.state_matrix.modelview[0].mat,
            &set->data.state_matrix.projection.mat, &mat);
        shader_state_matrix_set_mvp(set, &mat);
    }
}

inline void shader_state_matrix_set_mvp(shader_set_data* set,
    const mat4* data) {
    if (!set || !data)
        return;

    if (!memcmp(&set->data.state_matrix.mvp.mat, data, sizeof(mat4)))
        return;

    mat4 mat;
    mat4 mat_inv;
    mat4 mat_trans;
    mat4 mat_invtrans;

    mat = *data;
    mat4_inverse(&mat, &mat_inv);
    mat4_transpose(&mat, &mat_trans);
    mat4_transpose(&mat_inv, &mat_invtrans);
    set->data.state_matrix.mvp.mat = mat;
    set->data.state_matrix.mvp.inv = mat_inv;
    set->data.state_matrix.mvp.trans = mat_trans;
    set->data.state_matrix.mvp.invtrans = mat_invtrans;
    set->data.state_matrix_update_data = true;
}

inline void shader_state_matrix_set_modelview_separate(shader_set_data* set,
    size_t index, const mat4* model, const mat4* view, bool mult) {
    mat4 mv;
    mat4_mult(model, view, &mv);
    shader_state_matrix_set_modelview(set, index, &mv, true);
}

inline void shader_state_matrix_set_mvp_separate(shader_set_data* set,
    const mat4* model, const mat4* view, const mat4* projection) {
    mat4 mv;
    mat4_mult(model, view, &mv);
    shader_state_matrix_set_modelview(set, 0, &mv, false);
    shader_state_matrix_set_projection(set, projection, true);
}

inline void shader_state_matrix_set_texture(shader_set_data* set,
    size_t index, const mat4* data) {
    if (!set || index >= SHADER_MAX_TEXTURE_COORDS || !data)
        return;

    if (!memcmp(&set->data.state_matrix.texture[index].mat, data, sizeof(mat4)))
        return;

    mat4 mat;
    mat4 mat_inv;
    mat4 mat_trans;
    mat4 mat_invtrans;

    mat = *data;
    mat4_inverse(&mat, &mat_inv);
    mat4_transpose(&mat, &mat_trans);
    mat4_transpose(&mat_inv, &mat_invtrans);
    set->data.state_matrix.texture[index].mat = mat;
    set->data.state_matrix.texture[index].inv = mat_inv;
    set->data.state_matrix.texture[index].trans = mat_trans;
    set->data.state_matrix.texture[index].invtrans = mat_invtrans;
    set->data.state_matrix_update_data = true;
}

inline void shader_state_matrix_set_palette(shader_set_data* set,
    size_t index, const mat4* data) {
    if (!set || index >= SHADER_MAX_PALETTE_MATRICES || !data)
        return;

    if (!memcmp(&set->data.state_matrix.palette[index].mat, data, sizeof(mat4)))
        return;

    mat4 mat;
    mat4 mat_inv;
    mat4 mat_trans;
    mat4 mat_invtrans;

    mat = *data;
    mat4_inverse(&mat, &mat_inv);
    mat4_transpose(&mat, &mat_trans);
    mat4_transpose(&mat_inv, &mat_invtrans);
    set->data.state_matrix.palette[index].mat = mat;
    set->data.state_matrix.palette[index].inv = mat_inv;
    set->data.state_matrix.palette[index].trans = mat_trans;
    set->data.state_matrix.palette[index].invtrans = mat_invtrans;
    set->data.state_matrix_update_data = true;
}

inline void shader_state_matrix_set_program(shader_set_data* set,
    size_t index, const mat4* data) {
    if (!set || index >= SHADER_MAX_PROGRAM_MATRICES || !data)
        return;

    if (!memcmp(&set->data.state_matrix.program[index].mat, data, sizeof(mat4)))
        return;

    mat4 mat;
    mat4 mat_inv;
    mat4 mat_trans;
    mat4 mat_invtrans;

    mat = *data;
    mat4_inverse(&mat, &mat_inv);
    mat4_transpose(&mat, &mat_trans);
    mat4_transpose(&mat_inv, &mat_invtrans);
    set->data.state_matrix.program[index].mat = mat;
    set->data.state_matrix.program[index].inv = mat_inv;
    set->data.state_matrix.program[index].trans = mat_trans;
    set->data.state_matrix.program[index].invtrans = mat_invtrans;
    set->data.state_matrix_update_data = true;
}

inline void shader_state_point_set_size(shader_set_data* set,
    float_t x, float_t y, float_t z, float_t w) {
    if (!set)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&set->data.state.point.size, &data, sizeof(vec4)))
        return;

    set->data.state.point.size = data;
    set->data.state_update_data = true;
}

inline void shader_state_point_set_size_ptr(shader_set_data* set,
    const vec4* data) {
    if (!set || !data)
        return;

    if (!memcmp(&set->data.state.point.size, data, sizeof(vec4)))
        return;

    set->data.state.point.size = *data;
    set->data.state_update_data = true;
}

inline void shader_state_point_set_attenuation(shader_set_data* set,
    float_t x, float_t y, float_t z, float_t w) {
    if (!set)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&set->data.state.point.attenuation, &data, sizeof(vec4)))
        return;

    set->data.state.point.attenuation = data;
    set->data.state_update_data = true;
}

inline void shader_state_point_set_attenuation_ptr(shader_set_data* set,
    const vec4* data) {
    if (!set || !data)
        return;

    if (!memcmp(&set->data.state.point.attenuation, data, sizeof(vec4)))
        return;

    set->data.state.point.attenuation = *data;
    set->data.state_update_data = true;
}

inline void shader_state_texgen_set_eye_s(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!set || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&set->data.state.texgen[index].eye_s, &data, sizeof(vec4)))
        return;

    set->data.state.texgen[index].eye_s = data;
    set->data.state_update_data = true;
}

inline void shader_state_texgen_set_eye_s_ptr(shader_set_data* set,
    size_t index, const vec4* data) {
    if (!set || index >= SHADER_MAX_TEXTURE_UNITS || !data)
        return;

    if (!memcmp(&set->data.state.texgen[index].eye_s, data, sizeof(vec4)))
        return;

    set->data.state.texgen[index].eye_s = *data;
    set->data.state_update_data = true;
}

inline void shader_state_texgen_set_eye_t(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!set || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&set->data.state.texgen[index].eye_t, &data, sizeof(vec4)))
        return;

    set->data.state.texgen[index].eye_t = data;
    set->data.state_update_data = true;
}

inline void shader_state_texgen_set_eye_t_ptr(shader_set_data* set,
    size_t index, const vec4* data) {
    if (!set || index >= SHADER_MAX_TEXTURE_UNITS || !data)
        return;

    if (!memcmp(&set->data.state.texgen[index].eye_t, data, sizeof(vec4)))
        return;

    set->data.state.texgen[index].eye_t = *data;
    set->data.state_update_data = true;
}

inline void shader_state_texgen_set_eye_r(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!set || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&set->data.state.texgen[index].eye_r, &data, sizeof(vec4)))
        return;

    set->data.state.texgen[index].eye_r = data;
    set->data.state_update_data = true;
}

inline void shader_state_texgen_set_eye_r_ptr(shader_set_data* set,
    size_t index, const vec4* data) {
    if (!set || index >= SHADER_MAX_TEXTURE_UNITS || !data)
        return;

    if (!memcmp(&set->data.state.texgen[index].eye_r, data, sizeof(vec4)))
        return;

    set->data.state.texgen[index].eye_r = *data;
    set->data.state_update_data = true;
}

inline void shader_state_texgen_set_eye_q(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!set || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&set->data.state.texgen[index].eye_q, &data, sizeof(vec4)))
        return;

    set->data.state.texgen[index].eye_q = data;
    set->data.state_update_data = true;
}

inline void shader_state_texgen_set_eye_q_ptr(shader_set_data* set,
    size_t index, const vec4* data) {
    if (!set || index >= SHADER_MAX_TEXTURE_UNITS || !data)
        return;

    if (!memcmp(&set->data.state.texgen[index].eye_q, data, sizeof(vec4)))
        return;

    set->data.state.texgen[index].eye_q = *data;
    set->data.state_update_data = true;
}

inline void shader_state_texgen_set_object_s(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!set || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&set->data.state.texgen[index].object_s, &data, sizeof(vec4)))
        return;

    set->data.state.texgen[index].object_s = data;
    set->data.state_update_data = true;
}

inline void shader_state_texgen_set_object_s_ptr(shader_set_data* set,
    size_t index, const vec4* data) {
    if (!set || index >= SHADER_MAX_TEXTURE_UNITS || !data)
        return;

    if (!memcmp(&set->data.state.texgen[index].object_s, data, sizeof(vec4)))
        return;

    set->data.state.texgen[index].object_s = *data;
    set->data.state_update_data = true;
}

inline void shader_state_texgen_set_object_t(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!set || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&set->data.state.texgen[index].object_t, &data, sizeof(vec4)))
        return;

    set->data.state.texgen[index].object_t = data;
    set->data.state_update_data = true;
}

inline void shader_state_texgen_set_object_t_ptr(shader_set_data* set,
    size_t index, const vec4* data) {
    if (!set || index >= SHADER_MAX_TEXTURE_UNITS || !data)
        return;

    if (!memcmp(&set->data.state.texgen[index].object_t, data, sizeof(vec4)))
        return;

    set->data.state.texgen[index].object_t = *data;
    set->data.state_update_data = true;
}

inline void shader_state_texgen_set_object_r(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!set || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&set->data.state.texgen[index].object_r, &data, sizeof(vec4)))
        return;

    set->data.state.texgen[index].object_r = data;
    set->data.state_update_data = true;
}

inline void shader_state_texgen_set_object_r_ptr(shader_set_data* set,
    size_t index, const vec4* data) {
    if (!set || index >= SHADER_MAX_TEXTURE_UNITS || !data)
        return;

    if (!memcmp(&set->data.state.texgen[index].object_r, data, sizeof(vec4)))
        return;

    set->data.state.texgen[index].object_r = *data;
    set->data.state_update_data = true;
}

inline void shader_state_texgen_set_object_q(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!set || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&set->data.state.texgen[index].object_q, &data, sizeof(vec4)))
        return;

    set->data.state.texgen[index].object_q = data;
    set->data.state_update_data = true;
}

inline void shader_state_texgen_set_object_q_ptr(shader_set_data* set,
    size_t index, const vec4* data) {
    if (!set || index >= SHADER_MAX_TEXTURE_UNITS || !data)
        return;

    if (!memcmp(&set->data.state.texgen[index].object_q, data, sizeof(vec4)))
        return;

    set->data.state.texgen[index].object_q = *data;
    set->data.state_update_data = true;
}

inline void shader_state_texenv_set_color(shader_set_data* set,
    size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!set || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&set->data.state.texenv[index].color, &data, sizeof(vec4)))
        return;

    set->data.state.texenv[index].color = data;
    set->data.state_update_data = true;
}

inline void shader_state_texenv_set_color_ptr(shader_set_data* set,
    size_t index, const vec4* data) {
    if (!set || index >= SHADER_MAX_TEXTURE_UNITS || !data)
        return;

    if (!memcmp(&set->data.state.texenv[index].color, data, sizeof(vec4)))
        return;

    set->data.state.texenv[index].color = *data;
    set->data.state_update_data = true;
}

static GLuint shader_compile_shader(GLenum type, const char* data, const char* file) {
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
        printf("Shader compile error:\n");
        printf("file: %s\n", file);
        printf(info_log);
        putchar('\n');
        free(info_log);
        glDeleteShader(shader);

#if defined(CRE_DEV)
        wchar_t temp_buf[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathW(0, CSIDL_LOCAL_APPDATA, 0, 0, temp_buf))) {
            wcscat_s(temp_buf, sizeof(temp_buf) / sizeof(wchar_t), L"\\CLOUD");
            temp_buf[sizeof(temp_buf) / sizeof(wchar_t) - 1] = 0;
            CreateDirectoryW(temp_buf, 0);

            wchar_t buf[MAX_PATH];
            swprintf_s(buf, sizeof(buf) / sizeof(wchar_t),
                L"%ls\\shader_error", temp_buf);
            buf[sizeof(buf) / sizeof(wchar_t) - 1] = 0;
            CreateDirectoryW(buf, 0);

            swprintf_s(buf, sizeof(buf) / sizeof(wchar_t),
                L"%ls\\shader_error\\%hs", temp_buf, file);
            buf[sizeof(buf) / sizeof(wchar_t) - 1] = 0;

            stream s;
            io_open(&s, buf, L"wb");
            io_write(&s, data, utf8_length(data));
            io_free(&s);
        }
#endif
        return 0;
    }
    return shader;
}

static GLuint shader_compile(const char* vert, const char* frag, const char* vp, const char* fp) {
    GLuint vert_shad = shader_compile_shader(GL_VERTEX_SHADER, vert, vp);
    GLuint frag_shad = shader_compile_shader(GL_FRAGMENT_SHADER, frag, fp);

    GLuint program = glCreateProgram();
    if (vert_shad)
        glAttachShader(program, vert_shad);
    if (frag_shad)
        glAttachShader(program, frag_shad);
    glLinkProgram(program);

    if (vert_shad)
        glDeleteShader(vert_shad);
    if (frag_shad)
        glDeleteShader(frag_shad);

    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar* info_log = force_malloc_s(GLchar, 0x10000);
        glGetProgramInfoLog(program, 0x10000, 0, info_log);
        printf("Program Shader Permut linking error:\n");
        printf("vp: %s; fp: %s\n", vp, fp);
        printf(info_log);
        putchar('\n');
        free(info_log);
        glDeleteProgram(program);
        return 0;
    }
    else {
        gl_state_get_all_gl_errors();
        return program;
    }
}

static GLuint shader_compile_binary(const char* vert, const char* frag, const char* vp, const char* fp,
    program_binary* bin, GLsizei* buffer_size, void** binary) {
    memset(bin, 0, sizeof(*bin));

    GLuint vert_shad = shader_compile_shader(GL_VERTEX_SHADER, vert, vp);
    GLuint frag_shad = shader_compile_shader(GL_FRAGMENT_SHADER, frag, fp);

    GLuint program = glCreateProgram();
    if (vert_shad)
        glAttachShader(program, vert_shad);
    if (frag_shad)
        glAttachShader(program, frag_shad);
    glLinkProgram(program);

    if (vert_shad)
        glDeleteShader(vert_shad);
    if (frag_shad)
        glDeleteShader(frag_shad);

    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar* info_log = force_malloc_s(GLchar, 0x10000);
        glGetProgramInfoLog(program, 0x10000, 0, info_log);
        printf("Program Shader Permut linking error:\n");
        printf("vp: %s; fp: %s\n", vp, fp);
        printf(info_log);
        putchar('\n');
        free(info_log);
        glDeleteProgram(program);
        return 0;
    }
    else {
        gl_state_get_all_gl_errors();

        GLenum binary_format = 0x0;
        GLsizei length = 0;
        while (*buffer_size < 0x7FFFFFF) {
            glGetProgramBinary(program, *buffer_size, &length, &binary_format, *binary);
            if (!glGetError())
                break;

            free(*binary);
            *buffer_size <<= 1;
            *binary = force_malloc(*buffer_size);
        }

        bin->length = length;
        bin->binary_format = binary_format;
        bin->binary = (size_t)force_malloc(length);
        if (*binary)
            memcpy((void*)bin->binary, *binary, length);
        return program;
    }
}

static bool shader_load_binary_shader(program_binary* bin, GLuint* program) {
    *program = glCreateProgram();
    glProgramBinary(*program, bin->binary_format, (void*)((size_t)bin + bin->binary), bin->length);
    GLint success = 0;
    glGetProgramiv(*program, GL_LINK_STATUS, &success);
    if (!success) {
        glDeleteProgram(*program);
        *program = 0;
        //printf("load error: ");
        return false;
    }
    return true;
}

static bool shader_parse_define(const char* data, int32_t num_uniform,
    const int32_t* vp_unival_max, const int32_t* fp_unival_max,
    int32_t* uniform_value, char** temp, size_t* temp_size) {
    if (!data)
        return false;

    const char* def = strstr(data, "//DEF\n");
    if (!def)
        return str_utils_copy(data);

    if (!num_uniform || !vp_unival_max || !fp_unival_max || !uniform_value) {
        size_t len_a = def - data;
        def += 5;
        if (*def == '\n')
            def++;

        size_t len_b = utf8_length(def);
        size_t len = 0;
        len += len_a + len_b;

        if (len + 1 > *temp_size) {
            free(*temp);
            *temp_size = len + 1;
            *temp = force_malloc_s(char, *temp_size);
        }

        size_t pos = 0;
        memcpy(*temp + pos, data, len_a);
        pos += len_a;
        memcpy(*temp + pos, def, len_b);
        pos += len_b;
        (*temp)[pos] = 0;
        return true;
    }

    const int32_t s = min(0x100, num_uniform);

    size_t t_len[0x100];
    char t[0x100];
    memset(t, 0, 0x100);

    for (int32_t i = 0; i < s; i++) {
        sprintf_s(t, sizeof(t), "#define _%d %d\n", i, uniform_value[i]);
        t_len[i] = utf8_length(t);
    }

    size_t len_a = def - data;
    def += 5;
    size_t len_b = utf8_length(def);
    size_t len = 0;
    for (int32_t i = 0; i < s; i++)
        len += t_len[i];
    if (!len)
        def++;
    len += len_a + len_b;

    if (len + 1 > *temp_size) {
        free(*temp);
        *temp_size = len + 1;
        *temp = force_malloc_s(char, *temp_size);
    }

    size_t pos = 0;
    memcpy(*temp + pos, data, len_a);
    pos += len_a;
    for (int32_t i = 0; i < s; i++) {
        sprintf_s(t, sizeof(t), "#define _%d %d\n", i, uniform_value[i]);
        memcpy(*temp + pos, t, t_len[i]);
        pos += t_len[i];
    }
    memcpy(*temp + pos, def, len_b);
    pos += len_b;
    (*temp)[pos] = 0;
    return true;
}

static char* shader_parse_include(char* data, farc* f) {
    if (!data || !f)
        return data;

    char* data_end = data + utf8_length(data);
    char* i0 = strstr(data, "#include \"");
    char* i1 = i0 ? strstr(i0, "\"\n") : 0;
    if (!i0 || !i1)
        return data;

    size_t count = 1;
    while (i1 && (i0 = strstr(i1, "#include \""))) {
        i0 += 10;
        i1 = strstr(i0, "\"\n");
        if (i1)
            i1 += 1;
        count++;
    }

    char** temp = force_malloc_s(char*, count);
    size_t* temp_len = force_malloc_s(size_t, count);
    char** temp_ptr0 = force_malloc_s(char*, count);
    char** temp_ptr1 = force_malloc_s(char*, count);
    if (!temp || !temp_len || !temp_ptr0 || !temp_ptr1) {
        free(temp);
        free(temp_len);
        free(temp_ptr0);
        free(temp_ptr1);
        return data;
    }

    i1 = data;
    for (size_t i = 0; i < count; i++) {
        temp[i] = 0;
        i0 = i1 ? strstr(i1, "#include \"") : 0;
        i1 = i0 ? strstr(i0, "\"\n") : 0;
        if (!i0 || !i1)
            continue;

        temp_ptr0[i] = i0;
        temp_ptr1[i] = i1 + 1;
        i0 += 10;
        size_t s = i1 - i0;
        i1 += 2;
        char* t = force_malloc_s(char, s + 1);
        if (!t)
            continue;

        memcpy(t, i0, s);
        t[s] = 0;

        farc_file* ff = f->read_file(t);
        free(t);
        if (!ff)
            continue;

        t = force_malloc_s(char, ff->size + 1);
        if (t) {
            memcpy(t, ff->data, ff->size);
            t[ff->size] = 0;
        }
        temp[i] = t;
        temp_len[i] = ff->size;
    }

    size_t len = data_end - data;
    i1 = data;
    for (size_t i = 0; i < count; i++) {
        i0 = temp_ptr0[i];
        i1 = temp_ptr1[i];
        if (!i0 || !i1)
            continue;

        len -= i1 - i0;
        len += temp_len[i];
    }

    char* temp_data = force_malloc_s(char, len + 1);
    size_t pos = 0;
    memcpy(temp_data + pos, data, temp_ptr0[0] - data);
    pos += temp_ptr0[0] - data;
    for (int32_t i = 0; i < count; i++) {
        if (temp[i]) {
            size_t s = temp_len[i];
            memcpy(temp_data + pos, temp[i], s);
            pos += s;
        }

        if (i < count - 1 && temp_ptr1[i]) {
            size_t s = temp_ptr0[i + 1] - temp_ptr1[i];
            memcpy(temp_data + pos, temp_ptr1[i], s);
            pos += s;
        }
        else if (temp_ptr1[i]) {
            size_t s = data_end - temp_ptr1[i];
            memcpy(temp_data + pos, temp_ptr1[i], s);
            pos += s;
        }
    }
    temp_data[pos] = 0;

    free(data);
    for (size_t i = 0; i < count; i++)
        free(temp[i]);
    free(temp);
    free(temp_len);
    free(temp_ptr0);
    free(temp_ptr1);
    return temp_data;
}

static void shader_update_data(shader_set_data* set) {
    if (!set)
        return;

    shader_data* data = &set->data;
    if (!data->state_update_data && !data->state_matrix_update_data
        && !data->env_update_data && !data->buffer_update_data)
        return;

    GLuint current_uniform_index = gl_state.uniform_buffer_index;
    GLuint current_uniform = gl_state.uniform_buffer_binding[current_uniform_index];

    bool reset = false;
    if (data->state_update_data && data->state_ubo) {
        gl_state_bind_uniform_buffer(data->state_ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(data->state), &data->state);
        data->state_update_data = false;
        reset = true;
    }

    if (data->state_matrix_update_data && data->state_matrix_ubo) {
        gl_state_bind_uniform_buffer(data->state_matrix_ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(data->state_matrix), &data->state_matrix);
        data->state_matrix_update_data = false;
        reset = true;
    }

    if (data->env_update_data && data->env_ubo) {
        gl_state_bind_uniform_buffer(data->env_ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(data->env), &data->env);
        data->env_update_data = false;
        reset = true;
    }

    if (data->buffer_update_data && data->buffer_ubo) {
        gl_state_bind_uniform_buffer(data->buffer_ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(data->buffer.buffer), &data->buffer.buffer);
        data->buffer_update_data = false;
        reset = true;
    }

    if (reset)
        gl_state_bind_uniform_buffer_base(current_uniform_index, current_uniform);
}

#undef shader_update_state_data_buffer
#undef shader_update_state_data_buffer_array
#undef shader_update_state_data_buffer_array_of_array
#undef shader_update_state_data_buffer_array_array
#undef shader_update_state_matrix_data_buffer
#undef shader_update_env_vert_data_buffer
#undef shader_update_env_frag_data_buffer
#undef shader_update_buffer_data_buffer
