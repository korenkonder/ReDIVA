/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "shader.hpp"
#include "../KKdLib/io/file_stream.hpp"
#include "../KKdLib/io/path.hpp"
#include "../KKdLib/hash.hpp"
#include "../KKdLib/str_utils.hpp"
#include "gl_state.hpp"
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

shader_state_clip::shader_state_clip() {

}

shader_state_depth::shader_state_depth() {

}

shader_state_fog::shader_state_fog() {

}

shader_state_light::shader_state_light() {

}

shader_state_lightmodel::shader_state_lightmodel() {

}

shader_state_lightprod::shader_state_lightprod() {

}

shader_state_material::shader_state_material() {

}

shader_state_matrix_data::shader_state_matrix_data() {

}

shader_state_matrix::shader_state_matrix() {

}

shader_state_point::shader_state_point() {

}

shader_state_texgen::eye::eye() {

}

shader_state_texgen::object::object() {

}

shader_state_texgen::shader_state_texgen() {

}

shader_state_texenv::shader_state_texenv() {

}

shader_buffer::shader_buffer() {

}

shader_env::shader_env() {

}

shader_state::shader_state() {

}

shader_data::shader_data() : state_update_data(), state_matrix_update_data(),
env_update_data(), buffer_update_data(), state_ubo(), state_matrix_ubo(), env_ubo(), buffer_ubo() {

}

int32_t shader::bind(shader_set_data* set, uint32_t sub_index) {
    int32_t sub_shader_index = 0;
    if (num_sub < 1)
        return -1;

    for (shader_sub* i = sub; i->sub_index != sub_index; i++) {
        sub_shader_index++;
        if (sub_shader_index >= num_sub)
            return -1;
    }

    shader_sub* sub_shader = &sub[sub_shader_index];
    if (!sub_shader)
        return -1;

    int32_t unival_curr = 1;
    int32_t unival = 0;
    GLint uniform_val[16];
    if (num_uniform > 0) {
        const int32_t* vp_unival_max = sub_shader->vp_unival_max;
        const int32_t* fp_unival_max = sub_shader->fp_unival_max;

        int32_t i = 0;
        for (i = 0; i < num_uniform && i < 16; i++) {
            int32_t unival_max = use_permut[i]
                ? max_def(vp_unival_max[i], fp_unival_max[i]) : 0;
            unival += unival_curr * min_def(uniform_value[(int32_t)use_uniform[i]], unival_max);
            unival_curr *= unival_max + 1;

            int32_t unival_max_glsl = max_def(vp_unival_max[i], fp_unival_max[i]);
            uniform_val[i] = min_def(uniform_value[use_uniform[i]], unival_max_glsl);
        }

        for (; i < 16; i++)
            uniform_val[i] = 0;
    }

    gl_state_use_program(sub_shader->program[unival]);
    glUniform1iv(SHADER_MAX_PROGRAM_LOCAL_PARAMETERS * 2, 16, uniform_val);
    return 0;
}

void shader::unbind() {
    gl_state_use_program(0);
    gl_state_bind_uniform_buffer_base(0, 0);
    gl_state_bind_uniform_buffer_base(1, 0);
    gl_state_bind_uniform_buffer_base(2, 0);
    gl_state_bind_uniform_buffer_base(3, 0);
}

shader_set_data::shader_set_data() : shaders(), size(), data() {

}

void shader_set_data::draw_arrays(GLenum mode, GLint first, GLsizei count) {
    shader_update_data(this);
    glDrawArrays(mode, first, count);
}

void shader_set_data::draw_elements(GLenum mode,
    GLsizei count, GLenum type, const void* indices) {
    shader_update_data(this);
    glDrawElements(mode, count, type, indices);
}

void shader_set_data::draw_range_elements(GLenum mode,
    GLuint start, GLuint end, GLsizei count, GLenum type, const void* indices) {
    shader_update_data(this);
    glDrawRangeElements(mode, start, end, count, type, indices);
}

int32_t shader_set_data::get_index_by_name(const char* name) {
    for (size_t i = 0; i < size; i++)
        if (!str_utils_compare(shaders[i].name, name))
            return (int32_t)shaders[i].name_index;
    return -1;
}

void shader_set_data::load(farc* f, bool ignore_cache, bool not_load_cache,
    const char* name, const shader_table* shaders_table, const size_t size,
    const shader_bind_func* bind_func_table, const size_t bind_func_table_size) {
    if (!this || !f || !shaders_table || !size)
        return;

    bool shader_cache_changed = false;
    farc shader_cache_farc;
    wchar_t temp_buf[MAX_PATH];
    if (!ignore_cache && SUCCEEDED(SHGetFolderPathW(0, CSIDL_LOCAL_APPDATA, 0, 0, temp_buf))) {
        wcscat_s(temp_buf, sizeof(temp_buf) / sizeof(wchar_t), L"\\ReDIVA");
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
    shaders = force_malloc_s(shader, size);
    this->size = size;
    for (size_t i = 0; i < size; i++) {
        shader* shader = &shaders[i];
        shader->name = shaders_table[i].name;
        shader->name_index = shaders_table[i].name_index;
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
                free_def(vert_data);
                free_def(frag_data);
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
                        ? max_def(vp_unival_max[k], fp_unival_max[k]) : 0;
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
                                ? max_def(vp_unival_max[l], fp_unival_max[l]) : 0) + 1;
                            vec_vert[l] = (uint32_t)(min_def(m % unival_max, vp_unival_max[l]));
                            m /= unival_max;
                            vert_buf[vert_buf_pos + l] = (char)('0' + vec_vert[l]);
                        }

                        for (size_t l = 0, m = k; l < num_uniform; l++) {
                            size_t unival_max = (size_t)(shader->use_permut[l]
                                ? max_def(vp_unival_max[l], fp_unival_max[l]) : 0) + 1;
                            vec_frag[l] = (uint32_t)(min_def(m % unival_max, fp_unival_max[l]));
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
                    free_def(shader_cache_file->data);

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
                    free_def(binary);
                    k.binary = 0;
                    bin++;
                }
            }

            free_def(vert_data);
            free_def(frag_data);
            program_data_binary.clear();
        }
        vec_vert.clear();
        vec_frag.clear();

        for (size_t j = 0; j < bind_func_table_size; j++)
            if (shader->name_index == bind_func_table[j].index) {
                shader->bind_func = bind_func_table[j].bind_func;
                break;
            }
    }
    free_def(binary);
    free_def(temp_vert);
    free_def(temp_frag);

    if (shader_cache_changed)
        shader_cache_farc.write(temp_buf, FARC_COMPRESS_FArC, false);

    memset(&data, 0, sizeof(data));
    glGenBuffers(1, &data.state_ubo);
    gl_state_bind_uniform_buffer(data.state_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(data.state), (void*)&data.state, GL_DYNAMIC_DRAW);
    gl_state_bind_uniform_buffer(0);

    glGenBuffers(1, &data.state_matrix_ubo);
    gl_state_bind_uniform_buffer(data.state_matrix_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(data.state_matrix), (void*)&data.state_matrix, GL_DYNAMIC_DRAW);
    gl_state_bind_uniform_buffer(0);

    glGenBuffers(1, &data.env_ubo);
    gl_state_bind_uniform_buffer(data.env_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(data.env), (void*)&data.env, GL_DYNAMIC_DRAW);
    gl_state_bind_uniform_buffer(0);

    glGenBuffers(1, &data.buffer_ubo);
    gl_state_bind_uniform_buffer(data.buffer_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(data.buffer), (void*)&data.buffer, GL_DYNAMIC_DRAW);
    gl_state_bind_uniform_buffer(0);
}

void shader_set_data::set(uint32_t index) {
    if (this && index && shaders && index != -1) {
        env_frag_set(0x18, (float_t)uniform_value[U_TEXTURE_BLEND], 0.0, 0.0, 0.0);
        shader* shader = &shaders[index];
        if (shader->bind_func)
            shader->bind_func(this, shader);
        else
            shader->bind(this, shader->sub[0].sub_index);
        gl_state_bind_uniform_buffer_range(0, data.state_ubo, 0, sizeof(data.state));
        gl_state_bind_uniform_buffer_range(1, data.state_matrix_ubo, 0, sizeof(data.state_matrix));
        gl_state_bind_uniform_buffer_range(2, data.env_ubo, 0, sizeof(data.env));
        gl_state_bind_uniform_buffer_range(3, data.buffer_ubo, 0, sizeof(data.buffer));
    }
    else
        shader::unbind();
}

void shader_set_data::unload() {
    glDeleteBuffers(1, &data.state_ubo);
    glDeleteBuffers(1, &data.state_matrix_ubo);
    glDeleteBuffers(1, &data.env_ubo);
    glDeleteBuffers(1, &data.buffer_ubo);
    memset(&data, 0, sizeof(data));

    for (size_t i = 0; i < size; i++) {
        shader* shader = &shaders[i];
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
                        ? max_def(vp_unival_max[k], fp_unival_max[k]) : 0;
                    unival_count += unival_curr * unival_max;
                    unival_curr *= unival_max + 1;
                }

                if (sub->program)
                    for (size_t k = 0; k < unival_count; k++)
                        glDeleteProgram(sub->program[k]);
                free_def(sub->program);
            }
            else {
                if (sub->program)
                    glDeleteProgram(sub->program[0]);
                free_def(sub->program);
            }
        }
        free_def(shader->sub);
    }
    free_def(shaders);
}

void shader_set_data::buffer_get(size_t index, vec4& data) {
    if (!this || index >= SHADER_MAX_PROGRAM_BUFFER_PARAMETERS)
        return;

    data = this->data.buffer.buffer[index];
}

void shader_set_data::buffer_get(size_t index, size_t count, vec4* data) {
    if (!this || index >= SHADER_MAX_PROGRAM_BUFFER_PARAMETERS
        || index + count > SHADER_MAX_PROGRAM_BUFFER_PARAMETERS || !data)
        return;

    memcpy(data, &this->data.buffer.buffer[index], sizeof(vec4) * count);
}

void shader_set_data::local_frag_get(size_t index, float_t* x, float_t* y, float_t* z, float_t* w) {
    if (!this || index >= SHADER_MAX_PROGRAM_LOCAL_PARAMETERS || !x || !y || !z || !w)
        return;

    GLfloat temp[4];
    glGetUniformfv(gl_state_get_program(), (GLint)(index + SHADER_MAX_PROGRAM_LOCAL_PARAMETERS), temp);
    *x = temp[0];
    *y = temp[1];
    *z = temp[2];
    *w = temp[3];
}

void shader_set_data::local_frag_get(size_t index, vec4& data) {
    if (!this || index >= SHADER_MAX_PROGRAM_LOCAL_PARAMETERS)
        return;

    GLfloat temp[4];
    glGetUniformfv(gl_state_get_program(), (GLint)(index + SHADER_MAX_PROGRAM_LOCAL_PARAMETERS), temp);
    data = { temp[0], temp[1], temp[2], temp[3] };
}

void shader_set_data::local_vert_get(size_t index, float_t* x, float_t* y, float_t* z, float_t* w) {
    if (!this || index >= SHADER_MAX_PROGRAM_LOCAL_PARAMETERS || !x || !y || !z || !w)
        return;

    GLfloat temp[4];
    glGetUniformfv(gl_state_get_program(), (GLint)index, temp);
    *x = temp[0];
    *y = temp[1];
    *z = temp[2];
    *w = temp[3];
}

void shader_set_data::local_vert_get(size_t index, vec4& data) {
    if (!this || index >= SHADER_MAX_PROGRAM_LOCAL_PARAMETERS)
        return;

    GLfloat temp[4];
    glGetUniformfv(gl_state_get_program(), (GLint)index, temp);
    data = { temp[0], temp[1], temp[2], temp[3] };
}

void shader_set_data::env_frag_get(size_t index, float_t* x, float_t* y, float_t* z, float_t* w) {
    if (!this || index >= SHADER_MAX_PROGRAM_ENV_PARAMETERS || !x || !y || !z || !w)
        return;

    vec4& data = this->data.env.frag[index];
    *x = data.x;
    *y = data.y;
    *z = data.z;
    *w = data.w;
}

void shader_set_data::env_frag_get(size_t index, vec4& data) {
    if (!this || index >= SHADER_MAX_PROGRAM_ENV_PARAMETERS)
        return;

    data = this->data.env.frag[index];
}

void shader_set_data::env_frag_get(size_t index, size_t count, vec4* data) {
    if (!this || index >= SHADER_MAX_PROGRAM_ENV_PARAMETERS
        || index + count > SHADER_MAX_PROGRAM_ENV_PARAMETERS || !data)
        return;

    memcpy(data, &this->data.env.frag[index], sizeof(vec4) * count);
}

void shader_set_data::env_vert_get(size_t index, float_t* x, float_t* y, float_t* z, float_t* w) {
    if (!this || index >= SHADER_MAX_PROGRAM_ENV_PARAMETERS || !x || !y || !z || !w)
        return;

    vec4 data = this->data.env.vert[index];
    *x = data.x;
    *y = data.y;
    *z = data.z;
    *w = data.w;
}

void shader_set_data::env_vert_get(size_t index, vec4& data) {
    if (!this || index >= SHADER_MAX_PROGRAM_ENV_PARAMETERS)
        return;

    data = this->data.env.vert[index];
}

void shader_set_data::env_vert_get(size_t index, size_t count, vec4* data) {
    if (!this || index >= SHADER_MAX_PROGRAM_ENV_PARAMETERS
        || index + count > SHADER_MAX_PROGRAM_ENV_PARAMETERS || !data)
        return;

    memcpy(data, &this->data.env.vert[index], sizeof(vec4) * count);
}

void shader_set_data::state_clip_get_plane(size_t index, vec4& data) {
    if (!this || index >= SHADER_MAX_CLIP_PLANES)
        return;

    data = this->data.state.clip[index].plane;
}

void shader_set_data::state_depth_get_range(vec4& data) {
    if (!this)
        return;

    data = this->data.state.depth.range;
}

void shader_set_data::state_fog_get_color(vec4& data) {
    if (!this)
        return;

    data = this->data.state.fog.color;
}

void shader_set_data::state_fog_get_params(vec4& data) {
    if (!this)
        return;

    data = this->data.state.fog.params;
}

void shader_set_data::state_light_get_ambient(size_t index, vec4& data) {
    if (!this || index >= SHADER_MAX_LIGHTS)
        return;

    data = this->data.state.light[index].ambient;
}

void shader_set_data::state_light_get_diffuse(size_t index, vec4& data) {
    if (!this || index >= SHADER_MAX_LIGHTS)
        return;

    data = this->data.state.light[index].ambient;
}

void shader_set_data::state_light_get_specular(size_t index, vec4& data) {
    if (!this || index >= SHADER_MAX_LIGHTS)
        return;

    data = this->data.state.light[index].ambient;
}

void shader_set_data::state_light_get_position(size_t index, vec4& data) {
    if (!this || index >= SHADER_MAX_LIGHTS)
        return;

    data = this->data.state.light[index].ambient;
}

void shader_set_data::state_light_get_attenuation(size_t index, vec4& data) {
    if (!this || index >= SHADER_MAX_LIGHTS)
        return;

    data = this->data.state.light[index].ambient;
}

void shader_set_data::state_light_get_spot_direction(size_t index, vec4& data) {
    if (!this || index >= SHADER_MAX_LIGHTS)
        return;

    data = this->data.state.light[index].ambient;
}

void shader_set_data::state_light_get_half(size_t index, vec4& data) {
    if (!this || index >= SHADER_MAX_LIGHTS)
        return;

    data = this->data.state.light[index].ambient;
}

void shader_set_data::state_lightmodel_get_ambient(bool back, vec4& data) {
    if (!this)
        return;

    data = this->data.state.lightmodel[back ? 1 : 0].ambient;
}

void shader_set_data::state_lightmodel_get_scene_color(bool back, vec4& data) {
    if (!this)
        return;

    data = this->data.state.lightmodel[back ? 1 : 0].scene_color;
}

void shader_set_data::state_lightprod_get_ambient(bool back, size_t index, vec4& data) {
    if (!this || index >= SHADER_MAX_LIGHTS)
        return;

    shader_state_lightprod* lightprod = back
        ? this->data.state.lightprod_back : this->data.state.lightprod_front;
    data = lightprod[index].ambient;
}

void shader_set_data::state_lightprod_get_diffuse(bool back, size_t index, vec4& data) {
    if (!this || index >= SHADER_MAX_LIGHTS)
        return;

    shader_state_lightprod* lightprod = back
        ? this->data.state.lightprod_back : this->data.state.lightprod_front;
    data = lightprod[index].diffuse;
}

void shader_set_data::state_lightprod_get_specular(bool back, size_t index, vec4& data) {
    if (!this || index >= SHADER_MAX_LIGHTS)
        return;

    shader_state_lightprod* lightprod = back
        ? this->data.state.lightprod_back : this->data.state.lightprod_front;
    data = lightprod[index].specular;
}

void shader_set_data::state_material_get_ambient(bool back, vec4& data) {
    if (!this)
        return;

    data = this->data.state.material[back ? 1 : 0].ambient;
}

void shader_set_data::state_material_get_diffuse(bool back, vec4& data) {
    if (!this)
        return;

    data = this->data.state.material[back ? 1 : 0].diffuse;
}

void shader_set_data::state_material_get_specular(bool back, vec4& data) {
    if (!this)
        return;

    data = this->data.state.material[back ? 1 : 0].specular;
}

void shader_set_data::state_material_get_emission(bool back, vec4& data) {
    if (!this)
        return;

    data = this->data.state.material[back ? 1 : 0].emission;
}

void shader_set_data::state_material_get_shininess(bool back, vec4& data) {
    if (!this)
        return;

    data = this->data.state.material[back ? 1 : 0].shininess;
}

void shader_set_data::state_matrix_get_modelview(size_t index, mat4& data) {
    if (!this || index >= SHADER_MAX_VERTEX_UNITS)
        return;

    data = this->data.state_matrix.modelview[index].mat;
}

void shader_set_data::state_matrix_get_projection(mat4& data) {
    if (!this)
        return;

    data = this->data.state_matrix.projection.mat;
}

void shader_set_data::state_matrix_get_mvp(mat4& data) {
    if (!this)
        return;

    data = this->data.state_matrix.mvp.mat;
}

void shader_set_data::state_matrix_get_texture(size_t index, mat4& data) {
    if (!this || index >= SHADER_MAX_TEXTURE_COORDS)
        return;

    data = this->data.state_matrix.texture[index].mat;
}

void shader_set_data::state_matrix_get_palette(size_t index, mat4& data) {
    if (!this || index >= SHADER_MAX_PALETTE_MATRICES)
        return;

    data = this->data.state_matrix.palette[index].mat;
}

void shader_set_data::state_matrix_get_program(size_t index, mat4& data) {
    if (!this || index >= SHADER_MAX_PROGRAM_MATRICES)
        return;

    data = this->data.state_matrix.program[index].mat;
}

void shader_set_data::state_point_get_size(vec4& data) {
    if (!this)
        return;

    data = this->data.state.point.size;
}

void shader_set_data::state_point_get_attenuation(vec4& data) {
    if (!this)
        return;

    data = this->data.state.point.attenuation;
}

void shader_set_data::state_texgen_get_eye_s(size_t index, vec4& data) {
    if (!this || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    data = this->data.state.texgen[index].eye.s;
}

void shader_set_data::state_texgen_get_eye_t(size_t index, vec4& data) {
    if (!this || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    data = this->data.state.texgen[index].eye.t;
}

void shader_set_data::state_texgen_get_eye_r(size_t index, vec4& data) {
    if (!this || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    data = this->data.state.texgen[index].eye.r;
}

void shader_set_data::state_texgen_get_eye_q(size_t index, vec4& data) {
    if (!this || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    data = this->data.state.texgen[index].eye.q;
}

void shader_set_data::state_texgen_get_object_s(size_t index, vec4& data) {
    if (!this || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    data = this->data.state.texgen[index].object.s;
}

void shader_set_data::state_texgen_get_object_t(size_t index, vec4& data) {
    if (!this || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    data = this->data.state.texgen[index].object.t;
}

void shader_set_data::state_texgen_get_object_r(size_t index, vec4& data) {
    if (!this || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    data = this->data.state.texgen[index].object.r;
}

void shader_set_data::state_texgen_get_object_q(size_t index, vec4& data) {
    if (!this || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    data = this->data.state.texgen[index].object.q;
}

void shader_set_data::state_texenv_get_color(size_t index, vec4& data) {
    if (!this || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    data = this->data.state.texenv[index].color;
}

void shader_set_data::buffer_set(size_t index, const vec4& data) {
    if (!this || index >= SHADER_MAX_PROGRAM_BUFFER_PARAMETERS)
        return;

    if (!memcmp(&this->data.buffer.buffer[index], &data, sizeof(vec4)))
        return;

    this->data.buffer.buffer[index] = data;
    this->data.buffer_update_data = true;
}

void shader_set_data::buffer_set(size_t index, size_t count, const vec4* data) {
    if (!this || index >= SHADER_MAX_PROGRAM_BUFFER_PARAMETERS
        || index + count > SHADER_MAX_PROGRAM_BUFFER_PARAMETERS || !data)
        return;

    if (!memcmp(&this->data.buffer.buffer[index], data, sizeof(vec4) * count))
        return;

    memcpy(&this->data.buffer.buffer[index], data, sizeof(vec4) * count);
    this->data.buffer_update_data = true;
}

void shader_set_data::local_frag_set(size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!this || index >= SHADER_MAX_PROGRAM_LOCAL_PARAMETERS)
        return;

    glUniform4f((GLint)(index + SHADER_MAX_PROGRAM_LOCAL_PARAMETERS), x, y, z, w);
}

void shader_set_data::local_frag_set(size_t index, const vec4& data) {
    if (!this || index >= SHADER_MAX_PROGRAM_LOCAL_PARAMETERS)
        return;

    glUniform4f((GLint)(index + SHADER_MAX_PROGRAM_LOCAL_PARAMETERS), data.x, data.y, data.z, data.w);
}

void shader_set_data::local_frag_set(size_t index, size_t count, const vec4* data) {
    if (!this || index >= SHADER_MAX_PROGRAM_LOCAL_PARAMETERS
        || index + count > SHADER_MAX_PROGRAM_LOCAL_PARAMETERS || !data)
        return;

    glUniform4fv((GLint)(index + SHADER_MAX_PROGRAM_LOCAL_PARAMETERS),
        (GLsizei)count, (const GLfloat*)data);
}

void shader_set_data::local_vert_set(size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!this || index >= SHADER_MAX_PROGRAM_LOCAL_PARAMETERS)
        return;

    glUniform4f((GLint)index, x, y, z, w);
}

void shader_set_data::local_vert_set(size_t index, const vec4& data) {
    if (!this || index >= SHADER_MAX_PROGRAM_LOCAL_PARAMETERS)
        return;

    glUniform4f((GLint)index, data.x, data.y, data.z, data.w);
}

void shader_set_data::local_vert_set(size_t index, size_t count, const vec4* data) {
    if (!this || index >= SHADER_MAX_PROGRAM_LOCAL_PARAMETERS
        || index + count > SHADER_MAX_PROGRAM_LOCAL_PARAMETERS || !data)
        return;

    glUniform4fv((GLint)index, (GLsizei)count, (const GLfloat*)data);
}

void shader_set_data::env_frag_set(size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!this || index >= SHADER_MAX_PROGRAM_ENV_PARAMETERS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&this->data.env.frag[index], &data, sizeof(vec4)))
        return;

    this->data.env.frag[index] = data;
    this->data.env_update_data = true;
}

void shader_set_data::env_frag_set(size_t index, const vec4& data) {
    if (!this || index >= SHADER_MAX_PROGRAM_ENV_PARAMETERS)
        return;

    if (!memcmp(&this->data.env.frag[index], &data, sizeof(vec4)))
        return;

    this->data.env.frag[index] = data;
    this->data.env_update_data = true;
}

void shader_set_data::env_frag_set(size_t index, size_t count, const vec4* data) {
    if (!this || index >= SHADER_MAX_PROGRAM_ENV_PARAMETERS
        || index + count > SHADER_MAX_PROGRAM_ENV_PARAMETERS)
        return;

    if (!memcmp(&this->data.env.frag[index], data, sizeof(vec4) * count))
        return;

    memcpy(&this->data.env.frag[index], data, sizeof(vec4) * count);
    this->data.env_update_data = true;
}

void shader_set_data::env_vert_set(size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!this || index >= SHADER_MAX_PROGRAM_ENV_PARAMETERS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&this->data.env.vert[index], &data, sizeof(vec4)))
        return;

    this->data.env.vert[index] = data;
    this->data.env_update_data = true;
}

void shader_set_data::env_vert_set(size_t index, const vec4& data) {
    if (!this || index >= SHADER_MAX_PROGRAM_ENV_PARAMETERS)
        return;

    if (!memcmp(&this->data.env.vert[index], &data, sizeof(vec4)))
        return;

    this->data.env.vert[index] = data;
    this->data.env_update_data = true;
}

void shader_set_data::env_vert_set(size_t index, size_t count, const vec4* data) {
    if (!this || index >= SHADER_MAX_PROGRAM_ENV_PARAMETERS
        || index + count > SHADER_MAX_PROGRAM_ENV_PARAMETERS || !data)
        return;

    if (!memcmp(&this->data.env.vert[index], data, sizeof(vec4) * count))
        return;

    memcpy(&this->data.env.vert[index], data, sizeof(vec4) * count);
    this->data.env_update_data = true;
}

void shader_set_data::state_clip_set_plane(size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!this || index >= SHADER_MAX_CLIP_PLANES)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&this->data.state.clip[index].plane, &data, sizeof(vec4)))
        return;

    this->data.state.clip[index].plane = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_clip_set_plane(size_t index, const vec4& data) {
    if (!this || index >= SHADER_MAX_CLIP_PLANES)
        return;

    if (!memcmp(&this->data.state.clip[index].plane, &data, sizeof(vec4)))
        return;

    this->data.state.clip[index].plane = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_depth_set_range(float_t x, float_t y, float_t z, float_t w) {
    if (!this)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&this->data.state.depth.range, &data, sizeof(vec4)))
        return;

    this->data.state.depth.range = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_depth_set_range(const vec4& data) {
    if (!this)
        return;

    if (!memcmp(&this->data.state.depth.range, &data, sizeof(vec4)))
        return;

    this->data.state.depth.range = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_fog_set_color(float_t x, float_t y, float_t z, float_t w) {
    if (!this)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&this->data.state.fog.color, &data, sizeof(vec4)))
        return;

    this->data.state.fog.color = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_fog_set_color(const vec4& data) {
    if (!this)
        return;

    if (!memcmp(&this->data.state.fog.color, &data, sizeof(vec4)))
        return;

    this->data.state.fog.color = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_fog_set_params(float_t x, float_t y, float_t z, float_t w) {
    if (!this)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&this->data.state.fog.params, &data, sizeof(vec4)))
        return;

    this->data.state.fog.params = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_fog_set_params(const vec4& data) {
    if (!this)
        return;

    if (!memcmp(&this->data.state.fog.params, &data, sizeof(vec4)))
        return;

    this->data.state.fog.params = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_light_set_ambient(size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!this || index >= SHADER_MAX_LIGHTS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&this->data.state.light[index].ambient, &data, sizeof(vec4)))
        return;

    this->data.state.light[index].ambient = data;
    this->data.state_update_data = true;

    vec4 lightprod_front = this->data.state.light[index].ambient * this->data.state.material[0].ambient;
    vec4 lightprod_back = this->data.state.light[index].ambient * this->data.state.material[1].ambient;
    state_lightprod_set_ambient(false, index, lightprod_front);
    state_lightprod_set_ambient(true, index, lightprod_back);
}

void shader_set_data::state_light_set_ambient(size_t index, const vec4& data) {
    if (!this || index >= SHADER_MAX_LIGHTS)
        return;

    if (!memcmp(&this->data.state.light[index].ambient, &data, sizeof(vec4)))
        return;

    this->data.state.light[index].ambient = data;
    this->data.state_update_data = true;

    vec4 lightprod_front = this->data.state.light[index].ambient * this->data.state.material[0].ambient;
    vec4 lightprod_back = this->data.state.light[index].ambient * this->data.state.material[1].ambient;
    state_lightprod_set_ambient(false, index, lightprod_front);
    state_lightprod_set_ambient(true, index, lightprod_back);
}

void shader_set_data::state_light_set_diffuse(size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!this || index >= SHADER_MAX_LIGHTS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&this->data.state.light[index].diffuse, &data, sizeof(vec4)))
        return;

    this->data.state.light[index].diffuse = data;
    this->data.state_update_data = true;

    vec4 lightprod_front = this->data.state.light[index].diffuse * this->data.state.material[0].diffuse;
    vec4 lightprod_back = this->data.state.light[index].diffuse * this->data.state.material[1].diffuse;
    state_lightprod_set_diffuse(false, index, lightprod_front);
    state_lightprod_set_diffuse(true, index, lightprod_back);
}

void shader_set_data::state_light_set_diffuse(size_t index, const vec4& data) {
    if (!this || index >= SHADER_MAX_LIGHTS)
        return;

    if (!memcmp(&this->data.state.light[index].diffuse, &data, sizeof(vec4)))
        return;

    this->data.state.light[index].diffuse = data;
    this->data.state_update_data = true;

    vec4 lightprod_front = this->data.state.light[index].diffuse * this->data.state.material[0].diffuse;
    vec4 lightprod_back = this->data.state.light[index].diffuse * this->data.state.material[1].diffuse;
    state_lightprod_set_diffuse(false, index, lightprod_front);
    state_lightprod_set_diffuse(true, index, lightprod_back);
}

void shader_set_data::state_light_set_specular(size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!this || index >= SHADER_MAX_LIGHTS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&this->data.state.light[index].specular, &data, sizeof(vec4)))
        return;

    this->data.state.light[index].specular = data;
    this->data.state_update_data = true;

    vec4 lightprod_front = this->data.state.light[index].specular * this->data.state.material[0].diffuse;
    vec4 lightprod_back = this->data.state.light[index].specular * this->data.state.material[1].diffuse;
    state_lightprod_set_specular(false, index, lightprod_front);
    state_lightprod_set_specular(true, index, lightprod_back);
}

void shader_set_data::state_light_set_specular(size_t index, const vec4& data) {
    if (!this || index >= SHADER_MAX_LIGHTS)
        return;

    if (!memcmp(&this->data.state.light[index].specular, &data, sizeof(vec4)))
        return;

    this->data.state.light[index].specular = data;
    this->data.state_update_data = true;

    vec4 lightprod_front = this->data.state.light[index].specular * this->data.state.material[0].diffuse;
    vec4 lightprod_back = this->data.state.light[index].specular * this->data.state.material[1].diffuse;
    state_lightprod_set_specular(false, index, lightprod_front);
    state_lightprod_set_specular(true, index, lightprod_back);
}

void shader_set_data::state_light_set_position(size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!this || index >= SHADER_MAX_LIGHTS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&this->data.state.light[index].position, &data, sizeof(vec4)))
        return;

    this->data.state.light[index].position = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_light_set_position(size_t index, const vec4& data) {
    if (!this || index >= SHADER_MAX_LIGHTS)
        return;

    if (!memcmp(&this->data.state.light[index].position, &data, sizeof(vec4)))
        return;

    this->data.state.light[index].position = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_light_set_attenuation(size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!this || index >= SHADER_MAX_LIGHTS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&this->data.state.light[index].attenuation, &data, sizeof(vec4)))
        return;

    this->data.state.light[index].attenuation = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_light_set_attenuation(size_t index, const vec4& data) {
    if (!this || index >= SHADER_MAX_LIGHTS)
        return;

    if (!memcmp(&this->data.state.light[index].attenuation, &data, sizeof(vec4)))
        return;

    this->data.state.light[index].attenuation = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_light_set_spot_direction(size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!this || index >= SHADER_MAX_LIGHTS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&this->data.state.light[index].spot_direction, &data, sizeof(vec4)))
        return;

    this->data.state.light[index].spot_direction = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_light_set_spot_direction(size_t index, const vec4& data) {
    if (!this || index >= SHADER_MAX_LIGHTS)
        return;

    if (!memcmp(&this->data.state.light[index].spot_direction, &data, sizeof(vec4)))
        return;

    this->data.state.light[index].spot_direction = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_light_set_half(size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!this || index >= SHADER_MAX_LIGHTS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&this->data.state.light[index].half, &data, sizeof(vec4)))
        return;

    this->data.state.light[index].half = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_light_set_half(size_t index, const vec4& data) {
    if (!this || index >= SHADER_MAX_LIGHTS)
        return;

    if (!memcmp(&this->data.state.light[index].half, &data, sizeof(vec4)))
        return;

    this->data.state.light[index].half = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_lightmodel_set_ambient(bool back, float_t x, float_t y, float_t z, float_t w) {
    if (!this)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&this->data.state.lightmodel[back ? 1 : 0].ambient, &data, sizeof(vec4)))
        return;

    this->data.state.lightmodel[back ? 1 : 0].ambient = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_lightmodel_set_ambient(bool back, const vec4& data) {
    if (!this)
        return;

    if (!memcmp(&this->data.state.lightmodel[back ? 1 : 0].ambient, &data, sizeof(vec4)))
        return;

    this->data.state.lightmodel[back ? 1 : 0].ambient = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_lightmodel_set_scene_color(bool back, float_t x, float_t y, float_t z, float_t w) {
    if (!this)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&this->data.state.lightmodel[back ? 1 : 0].scene_color, &data, sizeof(vec4)))
        return;

    this->data.state.lightmodel[back ? 1 : 0].scene_color = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_lightmodel_set_scene_color(bool back, const vec4& data) {
    if (!this)
        return;

    if (!memcmp(&this->data.state.lightmodel[back ? 1 : 0].scene_color, &data, sizeof(vec4)))
        return;

    this->data.state.lightmodel[back ? 1 : 0].scene_color = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_lightprod_set_ambient(bool back,
    size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!this || index >= SHADER_MAX_LIGHTS)
        return;

    shader_state_lightprod* lightprod = back
        ? data.state.lightprod_back : data.state.lightprod_front;

    vec4 data = { x, y, z, w };
    if (!memcmp(&lightprod[index].ambient, &data, sizeof(vec4)))
        return;

    lightprod[index].ambient = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_lightprod_set_ambient(bool back, size_t index, const vec4& data) {
    if (!this || index >= SHADER_MAX_LIGHTS)
        return;

    shader_state_lightprod* lightprod = back
        ? this->data.state.lightprod_back : this->data.state.lightprod_front;

    if (!memcmp(&lightprod[index].ambient, &data, sizeof(vec4)))
        return;

    lightprod[index].ambient = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_lightprod_set_diffuse(bool back,
    size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!this || index >= SHADER_MAX_LIGHTS)
        return;

    shader_state_lightprod* lightprod = back
        ? data.state.lightprod_back : data.state.lightprod_front;

    vec4 data = { x, y, z, w };
    if (!memcmp(&lightprod[index].diffuse, &data, sizeof(vec4)))
        return;

    lightprod[index].diffuse = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_lightprod_set_diffuse(bool back, size_t index, const vec4& data) {
    if (!this || index >= SHADER_MAX_LIGHTS)
        return;

    shader_state_lightprod* lightprod = back
        ? this->data.state.lightprod_back : this->data.state.lightprod_front;

    if (!memcmp(&lightprod[index].diffuse, &data, sizeof(vec4)))
        return;

    lightprod[index].diffuse = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_lightprod_set_specular(bool back,
    size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!this || index >= SHADER_MAX_LIGHTS)
        return;

    shader_state_lightprod* lightprod = back
        ? data.state.lightprod_back : data.state.lightprod_front;

    vec4 data = { x, y, z, w };
    if (!memcmp(&lightprod[index].specular, &data, sizeof(vec4)))
        return;

    lightprod[index].specular = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_lightprod_set_specular(bool back,
    size_t index, const vec4& data) {
    if (!this || index >= SHADER_MAX_LIGHTS)
        return;

    shader_state_lightprod* lightprod = back
        ? this->data.state.lightprod_back : this->data.state.lightprod_front;

    if (!memcmp(&lightprod[index].specular, &data, sizeof(vec4)))
        return;

    lightprod[index].specular = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_material_set_ambient(bool back, float_t x, float_t y, float_t z, float_t w) {
    if (!this)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&this->data.state.material[back ? 1 : 0].ambient, &data, sizeof(vec4)))
        return;

    this->data.state.material[back ? 1 : 0].ambient = data;
    this->data.state_update_data = true;

    for (int32_t index = 0; index < SHADER_MAX_LIGHTS; index++) {
        vec4 lightprod = this->data.state.light[index].ambient
            * this->data.state.material[back ? 1 : 0].ambient;
        state_lightprod_set_ambient(back, index, lightprod);
    }
}

void shader_set_data::state_material_set_ambient(bool back, const vec4& data) {
    if (!this)
        return;

    if (!memcmp(&this->data.state.material[back ? 1 : 0].ambient, &data, sizeof(vec4)))
        return;

    this->data.state.material[back ? 1 : 0].ambient = data;
    this->data.state_update_data = true;

    for (int32_t index = 0; index < SHADER_MAX_LIGHTS; index++) {
        vec4 lightprod = this->data.state.light[index].ambient
            * this->data.state.material[back ? 1 : 0].ambient;
        state_lightprod_set_ambient(back, index, lightprod);
    }
}

void shader_set_data::state_material_set_diffuse(bool back, float_t x, float_t y, float_t z, float_t w) {
    if (!this)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&this->data.state.material[back ? 1 : 0].diffuse, &data, sizeof(vec4)))
        return;

    this->data.state.material[back ? 1 : 0].diffuse = data;
    this->data.state_update_data = true;

    for (int32_t index = 0; index < SHADER_MAX_LIGHTS; index++) {
        vec4 lightprod = this->data.state.light[index].diffuse
            * this->data.state.material[back ? 1 : 0].diffuse;
        state_lightprod_set_diffuse(back, index, lightprod);
    }
}

void shader_set_data::state_material_set_diffuse(bool back, const vec4& data) {
    if (!this)
        return;

    if (!memcmp(&this->data.state. material[back ? 1 : 0].diffuse, &data, sizeof(vec4)))
        return;

    this->data.state. material[back ? 1 : 0].diffuse = data;
    this->data.state_update_data = true;

    for (int32_t index = 0; index < SHADER_MAX_LIGHTS; index++) {
        vec4 lightprod = this->data.state.light[index].diffuse
            * this->data.state.material[back ? 1 : 0].diffuse;
        state_lightprod_set_diffuse(back, index, lightprod);
    }
}

void shader_set_data::state_material_set_specular(bool back, float_t x, float_t y, float_t z, float_t w) {
    if (!this)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&this->data.state.material[back ? 1 : 0].specular, &data, sizeof(vec4)))
        return;

    this->data.state.material[back ? 1 : 0].specular = data;
    this->data.state_update_data = true;

    for (int32_t index = 0; index < SHADER_MAX_LIGHTS; index++) {
        vec4 lightprod = this->data.state.light[index].specular
            * this->data.state.material[back ? 1 : 0].specular;
        state_lightprod_set_specular(back, index, lightprod);
    }
}

void shader_set_data::state_material_set_specular(bool back, const vec4& data) {
    if (!this)
        return;

    if (!memcmp(&this->data.state.material[back ? 1 : 0].specular, &data, sizeof(vec4)))
        return;

    this->data.state.material[back ? 1 : 0].specular = data;
    this->data.state_update_data = true;

    for (int32_t index = 0; index < SHADER_MAX_LIGHTS; index++) {
        vec4 lightprod = this->data.state.light[index].specular
            * this->data.state.material[back ? 1 : 0].specular;
        state_lightprod_set_specular(back, index, lightprod);
    }
}

void shader_set_data::state_material_set_emission(bool back, float_t x, float_t y, float_t z, float_t w) {
    if (!this)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&this->data.state.material[back ? 1 : 0].emission, &data, sizeof(vec4)))
        return;

    this->data.state.material[back ? 1 : 0].emission = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_material_set_emission(bool back, const vec4& data) {
    if (!this)
        return;

    if (!memcmp(&this->data.state.material[back ? 1 : 0].emission, &data, sizeof(vec4)))
        return;

    this->data.state.material[back ? 1 : 0].emission = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_material_set_shininess(bool back, float_t x, float_t y, float_t z, float_t w) {
    if (!this)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&this->data.state.material[back ? 1 : 0].shininess, &data, sizeof(vec4)))
        return;

    this->data.state.material[back ? 1 : 0].shininess = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_material_set_shininess(bool back, const vec4& data) {
    if (!this)
        return;

    if (!memcmp(&this->data.state.material[back ? 1 : 0].shininess, &data, sizeof(vec4)))
        return;

    this->data.state.material[back ? 1 : 0].shininess = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_matrix_set_modelview(size_t index, const mat4& data, bool mult) {
    if (!this || index >= SHADER_MAX_VERTEX_UNITS)
        return;

    if (memcmp(&this->data.state_matrix.modelview[index].mat, &data, sizeof(mat4))) {
        mat4 mat;
        mat4 mat_inv;
        mat4 mat_trans;
        mat4 mat_invtrans;

        mat = data;
        mat4_inverse(&mat, &mat_inv);
        mat4_transpose(&mat, &mat_trans);
        mat4_transpose(&mat_inv, &mat_invtrans);
        this->data.state_matrix.modelview[index].mat = mat;
        this->data.state_matrix.modelview[index].inv = mat_inv;
        this->data.state_matrix.modelview[index].trans = mat_trans;
        this->data.state_matrix.modelview[index].invtrans = mat_invtrans;
        this->data.state_matrix_update_data = true;
    }

    if (mult && index == 0) {
        mat4 mat;
        mat4_mult(&this->data.state_matrix.modelview[0].mat,
            &this->data.state_matrix.projection.mat, &mat);
        state_matrix_set_mvp(mat);
    }
}

void shader_set_data::state_matrix_set_projection(const mat4& data, bool mult) {
    if (!this)
        return;

    if (memcmp(&this->data.state_matrix.projection.mat, &data, sizeof(mat4))) {
        mat4 mat;
        mat4 mat_inv;
        mat4 mat_trans;
        mat4 mat_invtrans;

        mat = data;
        mat4_inverse(&mat, &mat_inv);
        mat4_transpose(&mat, &mat_trans);
        mat4_transpose(&mat_inv, &mat_invtrans);
        this->data.state_matrix.projection.mat = mat;
        this->data.state_matrix.projection.inv = mat_inv;
        this->data.state_matrix.projection.trans = mat_trans;
        this->data.state_matrix.projection.invtrans = mat_invtrans;
        this->data.state_matrix_update_data = true;
    }

    if (mult) {
        mat4 mat;
        mat4_mult(&this->data.state_matrix.modelview[0].mat,
            &this->data.state_matrix.projection.mat, &mat);
        state_matrix_set_mvp(mat);
    }
}

void shader_set_data::state_matrix_set_mvp(const mat4& data) {
    if (!this )
        return;

    if (!memcmp(&this->data.state_matrix.mvp.mat, &data, sizeof(mat4)))
        return;

    mat4 mat;
    mat4 mat_inv;
    mat4 mat_trans;
    mat4 mat_invtrans;

    mat = data;
    mat4_inverse(&mat, &mat_inv);
    mat4_transpose(&mat, &mat_trans);
    mat4_transpose(&mat_inv, &mat_invtrans);
    this->data.state_matrix.mvp.mat = mat;
    this->data.state_matrix.mvp.inv = mat_inv;
    this->data.state_matrix.mvp.trans = mat_trans;
    this->data.state_matrix.mvp.invtrans = mat_invtrans;
    this->data.state_matrix_update_data = true;
}

void shader_set_data::state_matrix_set_modelview(size_t index, const mat4& model, const mat4& view, bool mult) {
    mat4 mv;
    mat4_mult(&model, &view, &mv);
    state_matrix_set_modelview(index, mv, true);
}

void shader_set_data::state_matrix_set_mvp(const mat4& model, const mat4& view, const mat4& projection) {
    mat4 mv;
    mat4_mult(&model, &view, &mv);
    state_matrix_set_modelview(0, mv, false);
    state_matrix_set_projection(projection, true);
}

void shader_set_data::state_matrix_set_texture(size_t index, const mat4& data) {
    if (!this || index >= SHADER_MAX_TEXTURE_COORDS)
        return;

    if (!memcmp(&this->data.state_matrix.texture[index].mat, &data, sizeof(mat4)))
        return;

    mat4 mat;
    mat4 mat_inv;
    mat4 mat_trans;
    mat4 mat_invtrans;

    mat = data;
    mat4_inverse(&mat, &mat_inv);
    mat4_transpose(&mat, &mat_trans);
    mat4_transpose(&mat_inv, &mat_invtrans);
    this->data.state_matrix.texture[index].mat = mat;
    this->data.state_matrix.texture[index].inv = mat_inv;
    this->data.state_matrix.texture[index].trans = mat_trans;
    this->data.state_matrix.texture[index].invtrans = mat_invtrans;
    this->data.state_matrix_update_data = true;
}

void shader_set_data::state_matrix_set_palette(size_t index, const mat4& data) {
    if (!this || index >= SHADER_MAX_PALETTE_MATRICES)
        return;

    if (!memcmp(&this->data.state_matrix.palette[index].mat, &data, sizeof(mat4)))
        return;

    mat4 mat;
    mat4 mat_inv;
    mat4 mat_trans;
    mat4 mat_invtrans;

    mat = data;
    mat4_inverse(&mat, &mat_inv);
    mat4_transpose(&mat, &mat_trans);
    mat4_transpose(&mat_inv, &mat_invtrans);
    this->data.state_matrix.palette[index].mat = mat;
    this->data.state_matrix.palette[index].inv = mat_inv;
    this->data.state_matrix.palette[index].trans = mat_trans;
    this->data.state_matrix.palette[index].invtrans = mat_invtrans;
    this->data.state_matrix_update_data = true;
}

void shader_set_data::state_matrix_set_program(size_t index, const mat4& data) {
    if (!this || index >= SHADER_MAX_PROGRAM_MATRICES)
        return;

    if (!memcmp(&this->data.state_matrix.program[index].mat, &data, sizeof(mat4)))
        return;

    mat4 mat;
    mat4 mat_inv;
    mat4 mat_trans;
    mat4 mat_invtrans;

    mat = data;
    mat4_inverse(&mat, &mat_inv);
    mat4_transpose(&mat, &mat_trans);
    mat4_transpose(&mat_inv, &mat_invtrans);
    this->data.state_matrix.program[index].mat = mat;
    this->data.state_matrix.program[index].inv = mat_inv;
    this->data.state_matrix.program[index].trans = mat_trans;
    this->data.state_matrix.program[index].invtrans = mat_invtrans;
    this->data.state_matrix_update_data = true;
}

void shader_set_data::state_point_set_size(float_t x, float_t y, float_t z, float_t w) {
    if (!this)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&this->data.state.point.size, &data, sizeof(vec4)))
        return;

    this->data.state.point.size = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_point_set_size(const vec4& data) {
    if (!this)
        return;

    if (!memcmp(&this->data.state.point.size, &data, sizeof(vec4)))
        return;

    this->data.state.point.size = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_point_set_attenuation(float_t x, float_t y, float_t z, float_t w) {
    if (!this)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&this->data.state.point.attenuation, &data, sizeof(vec4)))
        return;

    this->data.state.point.attenuation = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_point_set_attenuation(const vec4& data) {
    if (!this)
        return;

    if (!memcmp(&this->data.state.point.attenuation, &data, sizeof(vec4)))
        return;

    this->data.state.point.attenuation = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_texgen_set_eye_s(size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!this || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&this->data.state.texgen[index].eye.s, &data, sizeof(vec4)))
        return;

    this->data.state.texgen[index].eye.s = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_texgen_set_eye_s(size_t index, const vec4& data) {
    if (!this || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    if (!memcmp(&this->data.state.texgen[index].eye.s, &data, sizeof(vec4)))
        return;

    this->data.state.texgen[index].eye.s = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_texgen_set_eye_t(size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!this || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&this->data.state.texgen[index].eye.t, &data, sizeof(vec4)))
        return;

    this->data.state.texgen[index].eye.t = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_texgen_set_eye_t(size_t index, const vec4& data) {
    if (!this || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    if (!memcmp(&this->data.state.texgen[index].eye.t, &data, sizeof(vec4)))
        return;

    this->data.state.texgen[index].eye.t = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_texgen_set_eye_r(size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!this || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&this->data.state.texgen[index].eye.r, &data, sizeof(vec4)))
        return;

    this->data.state.texgen[index].eye.r = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_texgen_set_eye_r(size_t index, const vec4& data) {
    if (!this || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    if (!memcmp(&this->data.state.texgen[index].eye.r, &data, sizeof(vec4)))
        return;

    this->data.state.texgen[index].eye.r = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_texgen_set_eye_q(size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!this || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&this->data.state.texgen[index].eye.q, &data, sizeof(vec4)))
        return;

    this->data.state.texgen[index].eye.q = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_texgen_set_eye_q(size_t index, const vec4& data) {
    if (!this || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    if (!memcmp(&this->data.state.texgen[index].eye.q, &data, sizeof(vec4)))
        return;

    this->data.state.texgen[index].eye.q = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_texgen_set_object_s(size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!this || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&this->data.state.texgen[index].object.s, &data, sizeof(vec4)))
        return;

    this->data.state.texgen[index].object.s = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_texgen_set_object_s(size_t index, const vec4& data) {
    if (!this || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    if (!memcmp(&this->data.state.texgen[index].object.s, &data, sizeof(vec4)))
        return;

    this->data.state.texgen[index].object.s = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_texgen_set_object_t(size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!this || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&this->data.state.texgen[index].object.t, &data, sizeof(vec4)))
        return;

    this->data.state.texgen[index].object.t = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_texgen_set_object_t(size_t index, const vec4& data) {
    if (!this || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    if (!memcmp(&this->data.state.texgen[index].object.t, &data, sizeof(vec4)))
        return;

    this->data.state.texgen[index].object.t = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_texgen_set_object_r(size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!this || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&this->data.state.texgen[index].object.r, &data, sizeof(vec4)))
        return;

    this->data.state.texgen[index].object.r = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_texgen_set_object_r(size_t index, const vec4& data) {
    if (!this || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    if (!memcmp(&this->data.state.texgen[index].object.r, &data, sizeof(vec4)))
        return;

    this->data.state.texgen[index].object.r = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_texgen_set_object_q(size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!this || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&this->data.state.texgen[index].object.q, &data, sizeof(vec4)))
        return;

    this->data.state.texgen[index].object.q = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_texgen_set_object_q(size_t index, const vec4& data) {
    if (!this || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    if (!memcmp(&this->data.state.texgen[index].object.q, &data, sizeof(vec4)))
        return;

    this->data.state.texgen[index].object.q = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_texenv_set_color(size_t index, float_t x, float_t y, float_t z, float_t w) {
    if (!this || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    vec4 data = { x, y, z, w };
    if (!memcmp(&this->data.state.texenv[index].color, &data, sizeof(vec4)))
        return;

    this->data.state.texenv[index].color = data;
    this->data.state_update_data = true;
}

void shader_set_data::state_texenv_set_color(size_t index, const vec4& data) {
    if (!this || index >= SHADER_MAX_TEXTURE_UNITS)
        return;

    if (!memcmp(&this->data.state.texenv[index].color, &data, sizeof(vec4)))
        return;

    this->data.state.texenv[index].color = data;
    this->data.state_update_data = true;
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
        free_def(info_log);
        glDeleteShader(shader);

#if defined(CRE_DEV)
        wchar_t temp_buf[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathW(0, CSIDL_LOCAL_APPDATA, 0, 0, temp_buf))) {
            wcscat_s(temp_buf, sizeof(temp_buf) / sizeof(wchar_t), L"\\ReDIVA");
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

            file_stream s;
            s.open(buf, L"wb");
            s.write(data, utf8_length(data));
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
        free_def(info_log);
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
        free_def(info_log);
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

            free_def(*binary);
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
            free_def(*temp);
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

    const int32_t s = min_def(0x100, num_uniform);

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
        free_def(*temp);
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
        free_def(temp);
        free_def(temp_len);
        free_def(temp_ptr0);
        free_def(temp_ptr1);
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
        free_def(t);
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

    free_def(data);
    for (size_t i = 0; i < count; i++)
        free_def(temp[i]);
    free_def(temp);
    free_def(temp_len);
    free_def(temp_ptr0);
    free_def(temp_ptr1);
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
