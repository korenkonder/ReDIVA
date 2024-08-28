/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "shader.hpp"
#include "../KKdLib/io/file_stream.hpp"
#include "../KKdLib/io/path.hpp"
#include "../KKdLib/prj/shared_ptr.hpp"
#include "../KKdLib/prj/vector_pair.hpp"
#include "../KKdLib/hash.hpp"
#include "../KKdLib/str_utils.hpp"
#include "Vulkan/gl_wrap.hpp"
#include "Vulkan/Manager.hpp"
#include "Vulkan/ShaderModule.hpp"
#include "gl_state.hpp"
#include <shlobj_core.h>

struct program_binary {
    GLsizei length;
    GLenum binary_format;
    size_t binary;
    uint64_t hash;
};

struct program_spv {
    size_t size;
    size_t spv;
    uint64_t hash;
};

#ifdef USE_OPENGL
static GLuint shader_compile_shader(GLenum type, const char* data, const char* file);
static GLuint shader_compile(const char* vert, const char* frag, const char* vp, const char* fp);
static GLuint shader_compile_binary(const char* vert, const char* frag, const char* vp, const char* fp,
    program_binary* bin, GLsizei* buffer_size, void** binary);
#endif
static bool shader_load_binary_shader(program_binary* bin, GLuint* program, const char* vp, const char* fp);
static prj::shared_ptr<Vulkan::ShaderModule> shader_load_spv_shader(program_spv* spv, const char* shader);
static bool shader_update_data(shader_set_data* set, GLenum mode, GLenum type, const void* indices);

int32_t shader::bind(shader_set_data* set, uint32_t sub_index) {
    set->curr_program = 0;
    set->vp_desc = 0;
    set->fp_desc = 0;
    set->unival_hash = 0;

    if (num_sub < 1)
        return -1;

    int32_t sub_shader_index = 0;
    for (shader_sub* i = sub; i->sub_index != sub_index; i++)
        if (++sub_shader_index >= num_sub)
            return -1;

    shader_sub* sub_shader = &sub[sub_shader_index];

    int32_t unival_shad_curr = 1;
    int32_t unival_shad = 0;
    uint64_t unival_hash = 0;
    if (num_uniform > 0) {
        uint32_t unival_arr[0x20];

        const int32_t* vp_unival_max = sub_shader->vp_unival_max;
        const int32_t* fp_unival_max = sub_shader->fp_unival_max;

        int32_t i = 0;
        for (i = 0; i < num_uniform; i++) {
            const int32_t unival = uniform_value[use_uniform[i]];
            const int32_t unival_max = max_def(vp_unival_max[i], fp_unival_max[i]);
            unival_shad += unival_shad_curr * min_def(unival, unival_max);
            unival_shad_curr *= unival_max + 1;
            unival_arr[i] = unival;
        }

        unival_hash = hash_xxh3_64bits(unival_arr, sizeof(uint32_t) * num_uniform);
    }

    GLuint program = sub_shader->programs[unival_shad];
    set->curr_program = program;
    set->vp_desc = sub_shader->vp_desc;
    set->fp_desc = sub_shader->fp_desc;
    set->unival_hash = unival_hash;

    gl_state_use_program(program);
    return 0;
}

static void parse_define_inner(std::string& temp, bool vulkan) {
    if (!vulkan) {
        size_t off = 0;
        while (true) {
            size_t pos_set = temp.find("set = ", off);
            size_t pos_binding = temp.find("binding = ", off);
            if (pos_set == -1 || pos_binding == -1)
                break;

            temp.erase(pos_set, pos_binding - pos_set);
            off = pos_set + 10;
        }
    }
    else {
        size_t off = 0;
        while (true) {
            size_t pos_result_position = temp.find("result_position = ", off);
            if (pos_result_position == -1)
                break;

            size_t pos_end = temp.find(";", pos_result_position + 18);
            if (pos_end == -1)
                break;

            temp.insert(pos_end + 1, " result_position.z = (result_position.z + result_position.w) * 0.5;");
            off = pos_end + 67;
        }

        off = 0;
        while (true) {
            size_t pos = temp.find("gl_VertexID", off);
            if (pos == -1)
                break;

            temp.replace(pos, 11, "gl_VertexIndex");
            off = pos + 14;
        }

        off = 0;
        while (true) {
            size_t pos = temp.find("layout(set = 1, binding = 0) uniform Shader", off);
            if (pos == -1)
                break;

            temp.replace(pos, 43, "layout(push_constant) uniform Shader");
            off = pos + 36;
        }
    }
}

bool shader::parse_define(const char* data, std::string& temp, bool vulkan) {
    if (!data)
        return false;

    const char* def = strstr(data, "//DEF\n");
    if (def) {
        temp.clear();

        size_t len_a = def - data;
        def += 5;
        if (*def == '\n')
            def++;

        size_t len_b = utf8_length(def);

        size_t pos = 0;
        temp.insert(pos, data, len_a);
        pos += len_a;
        temp.insert(pos, def, len_b);
        pos += len_b;
    }
    else
        temp.assign(data);

    parse_define_inner(temp, vulkan);
    return true;
}

bool shader::parse_define(const char* data, int32_t num_uniform,
    int32_t* uniform_value, std::string& temp, bool vulkan) {
    if (!data)
        return false;

    const char* def = strstr(data, "//DEF\n");
    if (def) {
        temp.clear();

        size_t len_a = def - data;
        def += 5;
        if (*def == '\n')
            def++;

        size_t len_b = utf8_length(def);

        size_t pos = 0;
        temp.insert(pos, data, len_a);
        pos += len_a;
        for (int32_t i = 0; i < num_uniform; i++) {
            char t[0x100];
            int32_t len = sprintf_s(t, sizeof(t), "#define _%d %d\n", i, uniform_value[i]);
            temp.insert(pos, t, len);
            pos += len;
        }
        temp.insert(pos, def, len_b);
        pos += len_b;
    }
    else
        temp.assign(data);

    parse_define_inner(temp, vulkan);
    return true;
}

char* shader::parse_include(char* data, farc* f) {
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

    char** temp = force_malloc<char*>(count);
    size_t* temp_len = force_malloc<size_t>(count);
    char** temp_ptr0 = force_malloc<char*>(count);
    char** temp_ptr1 = force_malloc<char*>(count);
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
        char* t = force_malloc<char>(s + 1);
        if (!t)
            continue;

        memcpy(t, i0, s);
        t[s] = 0;

        farc_file* ff = f->read_file(t);
        free_def(t);
        if (!ff)
            continue;

        t = force_malloc<char>(ff->size + 1);
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

    char* temp_data = force_malloc<char>(len + 1);
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

void shader::unbind() {
    gl_state_use_program(0);
}

shader_set_data::shader_set_data() : size(), shaders(), curr_program(),
primitive_restart(), primitive_restart_index(), get_index_by_name_func(),
get_name_by_index_func(), vp_desc(), fp_desc(), unival_hash() {

}

void shader_set_data::disable_primitive_restart() {
    primitive_restart = false;
}

void shader_set_data::draw_arrays(GLenum mode, GLint first, GLsizei count) {
    if (!shader_update_data(this, mode, GL_ZERO, 0))
        return;

    if (Vulkan::use)
        vkCmdDraw(Vulkan::current_command_buffer, count, 1, first, 0);
#ifdef USE_OPENGL
    else
        glDrawArrays(mode, first, count);
#endif
}

void shader_set_data::draw_elements(GLenum mode,
    GLsizei count, GLenum type, const void* indices) {
    switch (mode) {
    case GL_TRIANGLE_STRIP:
        uint32_t index;
        switch (type) {
        case GL_UNSIGNED_BYTE:
            index = 0xFF;
            break;
        case GL_UNSIGNED_SHORT:
            index = 0xFFFF;
            break;
        case GL_UNSIGNED_INT:
        default:
            index = 0xFFFFFFFF;
            break;
        }

        enable_primitive_restart();

        if (primitive_restart_index != index)
            primitive_restart_index = index;
        break;
    }

    if (!shader_update_data(this, mode, type, indices))
        return;

    if (Vulkan::use)
        vkCmdDrawIndexed(Vulkan::current_command_buffer, count, 1, 0, 0, 0);
#ifdef USE_OPENGL
    else
        glDrawElements(mode, count, type, indices);
#endif

    switch (mode) {
    case GL_TRIANGLE_STRIP:
        disable_primitive_restart();
        break;
    }
}

void shader_set_data::draw_range_elements(GLenum mode,
    GLuint start, GLuint end, GLsizei count, GLenum type, const void* indices) {
    switch (mode) {
    case GL_TRIANGLE_STRIP:
        uint32_t index;
        switch (type) {
        case GL_UNSIGNED_BYTE:
            index = 0xFF;
            break;
        case GL_UNSIGNED_SHORT:
            index = 0xFFFF;
            break;
        case GL_UNSIGNED_INT:
        default:
            index = 0xFFFFFFFF;
            break;
        }

        enable_primitive_restart();

        if (primitive_restart_index != index)
            primitive_restart_index = index;
        break;
    }

    if (!shader_update_data(this, mode, type, indices))
        return;

    if (Vulkan::use)
        vkCmdDrawIndexed(Vulkan::current_command_buffer, count, 1, 0, 0, 0);
#ifdef USE_OPENGL
    else
        glDrawRangeElements(mode, start, end, count, type, indices);
#endif

    switch (mode) {
    case GL_TRIANGLE_STRIP:
        disable_primitive_restart();
        break;
    }
}

void shader_set_data::enable_primitive_restart() {
    primitive_restart = true;
}

int32_t shader_set_data::get_index_by_name(const char* name) {
    if (get_index_by_name_func) {
        int32_t index = get_index_by_name_func(name);
        if (index != -1)
            return index;
    }

    for (size_t i = 0; i < size; i++)
        if (!str_utils_compare(shaders[i].name, name))
            return (int32_t)shaders[i].name_index;
    return -1;
}

const char* shader_set_data::get_name_by_index(int32_t index) {
    if (get_name_by_index_func) {
        const char* name = get_name_by_index_func(index);
        if (name)
            return name;
    }

    if (index >= 0 && index < size)
        return shaders[index].name;
    return 0;
}

void shader_set_data::load(farc* f, bool ignore_cache,
    const char* name, const shader_table* shaders_table, const size_t size,
    const shader_bind_func* bind_func_table, const size_t bind_func_table_size,
    PFNSHADERGETINDEXFUNCPROC get_index_by_name, PFNSHADERGETNAMEFUNCPROC get_name_by_index) {
    if (!this || !f || !shaders_table || !size)
        return;

    if (Vulkan::use) {
        char vert_buf[MAX_PATH];
        char frag_buf[MAX_PATH];
        char vert_file_buf[MAX_PATH];
        char frag_file_buf[MAX_PATH];
        char vert_bin_buf[MAX_PATH];
        char frag_bin_buf[MAX_PATH];

        prj::vector_pair<uint64_t, prj::shared_ptr<Vulkan::ShaderModule>> vec_shader_module;
        shader* shaders = force_malloc<shader>(size);
        this->shaders = shaders;
        this->size = size;
        for (size_t i = 0; i < size; i++) {
            shader* shader = &shaders[i];
            shader->name = shaders_table[i].name;
            shader->name_index = shaders_table[i].name_index;
            shader->num_sub = shaders_table[i].num_sub;
            shader->sub = force_malloc<shader_sub>(shader->num_sub);
            shader->num_uniform = shaders_table[i].num_uniform;
            shader->use_uniform = shaders_table[i].use_uniform;

            int32_t num_sub = shader->num_sub;
            const shader_sub_table* sub_table = shaders_table[i].sub;
            shader_sub* sub = shader->sub;
            const uniform_name* use_uniform = shader->use_uniform;
            for (int32_t j = 0; j < num_sub; j++, sub++, sub_table++) {
                sub->sub_index = sub_table->sub_index;
                sub->vp_unival_max = sub_table->vp_unival_max;
                sub->fp_unival_max = sub_table->fp_unival_max;
                strcpy_s(vert_file_buf, sizeof(vert_file_buf), sub_table->vp);
                strcat_s(vert_file_buf, sizeof(vert_file_buf), ".vert");

                strcpy_s(frag_file_buf, sizeof(frag_file_buf), sub_table->fp);
                strcat_s(frag_file_buf, sizeof(frag_file_buf), ".frag");

                sub->vp_desc = sub_table->vp_desc;
                sub->fp_desc = sub_table->fp_desc;

                uint32_t uniform_vert_flags = 0;
                uint32_t uniform_frag_flags = 0;
                for (int32_t k = 0; k < shader->num_uniform; k++) {
                    if (sub_table->vp_unival_max[k] > 0)
                        uniform_vert_flags |= 1 << k;
                    if (sub_table->fp_unival_max[k] > 0)
                        uniform_frag_flags |= 1 << k;
                }

                char uniform_vert_flags_buf[9];
                char uniform_frag_flags_buf[9];
                for (int32_t k = 0, l = 7; k < 32; k += 4, l--) {
                    int32_t vert_flags_digit = (uniform_vert_flags >> k) & 0xF;
                    if (vert_flags_digit >= 0x00 && vert_flags_digit <= 0x09)
                        uniform_vert_flags_buf[l] = (char)('0' + vert_flags_digit);
                    else
                        uniform_vert_flags_buf[l] = (char)('A' + (vert_flags_digit - 0x0A));

                    int32_t frag_flags_digit = (uniform_frag_flags >> k) & 0xF;
                    if (frag_flags_digit >= 0x00 && frag_flags_digit <= 0x09)
                        uniform_frag_flags_buf[l] = (char)('0' + frag_flags_digit);
                    else
                        uniform_frag_flags_buf[l] = (char)('A' + (frag_flags_digit - 0x0A));
                }
                uniform_vert_flags_buf[8] = 0;
                uniform_frag_flags_buf[8] = 0;

                strcpy_s(vert_buf, sizeof(vert_buf), vert_file_buf);
                strcpy_s(frag_buf, sizeof(frag_buf), frag_file_buf);
                strcat_s(vert_buf, sizeof(vert_buf), ".");
                strcat_s(frag_buf, sizeof(frag_buf), ".");
                strcat_s(vert_buf, sizeof(vert_buf), uniform_vert_flags_buf);
                strcat_s(frag_buf, sizeof(frag_buf), uniform_frag_flags_buf);

                uint64_t vert_file_name_hash = hash_utf8_xxh3_64bits(vert_buf);
                uint64_t frag_file_name_hash = hash_utf8_xxh3_64bits(frag_buf);

                strcpy_s(vert_bin_buf, sizeof(vert_bin_buf), sub_table->vp);
                strcat_s(vert_bin_buf, sizeof(vert_bin_buf), ".vert.bin");

                strcpy_s(frag_bin_buf, sizeof(frag_bin_buf), sub_table->fp);
                strcat_s(frag_bin_buf, sizeof(frag_bin_buf), ".frag.bin");

                farc_file* shader_vert_file = f->read_file(vert_bin_buf);

                program_spv* vert_spv = 0;
                if (!shader_vert_file || !shader_vert_file->data)
                    printf_debug("Vertex shader not found: %s\n", vert_bin_buf);
                else if (vert_file_name_hash != ((uint64_t*)shader_vert_file->data)[0])
                    printf_debug("Vertex shader flags not equal: %s\n", vert_file_buf);
                else
                    vert_spv = (program_spv*)&((uint64_t*)shader_vert_file->data)[1];

                farc_file* shader_frag_file = f->read_file(frag_bin_buf);

                program_spv* frag_spv = 0;
                if (!shader_frag_file || !shader_frag_file->data)
                    printf_debug("Fragment shader not found: %s\n", frag_bin_buf);
                else if (frag_file_name_hash != ((uint64_t*)shader_frag_file->data)[0])
                    printf_debug("Fragment shader flags not equal: %s\n", frag_file_buf);
                else
                    frag_spv = (program_spv*)&((uint64_t*)shader_frag_file->data)[1];

                if (shader->num_uniform > 0
                    && (sub_table->vp_unival_max[0] != -1 || sub_table->fp_unival_max[0] != -1)) {
                    int32_t num_uniform = shader->num_uniform;
                    int32_t unival_shad_curr = 1;
                    int32_t unival_vp_curr = 1;
                    int32_t unival_fp_curr = 1;
                    int32_t unival_shad_count = 1;
                    int32_t unival_vp_count = 1;
                    int32_t unival_fp_count = 1;
                    const int32_t* vp_unival_max = sub_table->vp_unival_max;
                    const int32_t* fp_unival_max = sub_table->fp_unival_max;
                    for (int32_t k = 0; k < num_uniform; k++) {
                        const int32_t unival_max = max_def(vp_unival_max[k], fp_unival_max[k]);
                        const int32_t unival_vp_max = vp_unival_max[k];
                        const int32_t unival_fp_max = fp_unival_max[k];
                        unival_shad_count += unival_shad_curr * unival_max;
                        unival_vp_count += unival_vp_curr * unival_vp_max;
                        unival_fp_count += unival_fp_curr * unival_fp_max;
                        unival_shad_curr *= unival_max + 1;
                        unival_vp_curr *= unival_vp_max + 1;
                        unival_fp_curr *= unival_fp_max + 1;
                    }

                    GLuint* programs = force_malloc<GLuint>(unival_shad_count);
                    sub->programs = programs;
                    if (programs) {
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

                        vec_shader_module.reserve((size_t)unival_vp_count + unival_fp_count);

                        for (int32_t k = 0; k < unival_vp_count; k++) {
                            uint64_t vp_hash = 0x00;
                            for (int32_t l = 0, m = k; l < num_uniform; l++) {
                                int32_t unival_max = vp_unival_max[l] + 1;
                                int32_t vp_digit = m % unival_max;
                                vp_hash = (vp_hash << 3) | (uint32_t)vp_digit;
                                m /= unival_max;
                                vert_buf[vert_buf_pos + l] = (char)('0' + vp_digit);
                            }

                            prj::shared_ptr<Vulkan::ShaderModule> vp_shader_module;
                            if (vert_spv) {
                                vp_shader_module = shader_load_spv_shader(&vert_spv[k], vert_buf);
                                if (!vp_shader_module)
                                    vp_shader_module.reset();
                            }
                            vec_shader_module.push_back(vp_hash, vp_shader_module);
                        }

                        for (int32_t k = 0; k < unival_fp_count; k++) {
                            uint64_t fp_hash = 0x01;
                            for (int32_t l = 0, m = k; l < num_uniform; l++) {
                                int32_t unival_max = fp_unival_max[l] + 1;
                                int32_t fp_digit = m % unival_max;
                                fp_hash = (fp_hash << 3) | (uint32_t)fp_digit;
                                m /= unival_max;
                                frag_buf[frag_buf_pos + l] = (char)('0' + fp_digit);
                            }

                            prj::shared_ptr<Vulkan::ShaderModule> fp_shader_module;
                            if (frag_spv) {
                                fp_shader_module = shader_load_spv_shader(&frag_spv[k], frag_buf);
                                if (!fp_shader_module)
                                    fp_shader_module.reset();
                            }
                            vec_shader_module.push_back(fp_hash, fp_shader_module);
                        }

                        vec_shader_module.sort();

                        for (int32_t k = 0; k < unival_shad_count; k++) {
                            uint64_t vp_hash = 0x00;
                            uint64_t fp_hash = 0x01;
                            for (int32_t l = 0, m = k; l < num_uniform; l++) {
                                int32_t unival_max = max_def(vp_unival_max[l], fp_unival_max[l]) + 1;
                                vp_hash = (vp_hash << 3) | (uint32_t)min_def(m % unival_max, vp_unival_max[l]);
                                fp_hash = (fp_hash << 3) | (uint32_t)min_def(m % unival_max, fp_unival_max[l]);
                                m /= unival_max;
                            }

                            auto elem_vp = vec_shader_module.find(vp_hash);
                            auto elem_fp = vec_shader_module.find(fp_hash);
                            if (elem_vp != vec_shader_module.end() && elem_fp != vec_shader_module.end()
                                && elem_vp->second && elem_fp->second) {
                                programs[k] = glCreateProgram();
                                Vulkan::gl_program* vk_program = Vulkan::gl_program::get(programs[k]);
                                vk_program->vertex_shader_module = elem_vp->second;
                                vk_program->fragment_shader_module = elem_fp->second;
                            }
                        }
                        vec_shader_module.clear();
                    }
                }
                else {
                    GLuint* programs = force_malloc<GLuint>();
                    sub->programs = programs;
                    if (programs) {
                        strcpy_s(vert_buf, sizeof(vert_buf), sub_table->vp);
                        strcpy_s(frag_buf, sizeof(vert_buf), sub_table->fp);
                        strcat_s(vert_buf, sizeof(vert_buf), "..vert");
                        strcat_s(frag_buf, sizeof(vert_buf), "..frag");

                        prj::shared_ptr<Vulkan::ShaderModule> vp_shader_module;
                        if (vert_spv) {
                            vp_shader_module = shader_load_spv_shader(&vert_spv[0], vert_buf);
                            if (!vp_shader_module)
                                vp_shader_module.reset();
                        }

                        prj::shared_ptr<Vulkan::ShaderModule> fp_shader_module;
                        if (frag_spv) {
                            fp_shader_module = shader_load_spv_shader(&frag_spv[0], frag_buf);
                            if (!fp_shader_module)
                                fp_shader_module.reset();
                        }

                        if (vp_shader_module && fp_shader_module) {
                            programs[0] = glCreateProgram();
                            Vulkan::gl_program* vk_program = Vulkan::gl_program::get(programs[0]);
                            vk_program->vertex_shader_module = vp_shader_module;
                            vk_program->fragment_shader_module = fp_shader_module;
                        }
                    }
                }
            }

            for (size_t j = 0; j < bind_func_table_size; j++)
                if (shader->name_index == bind_func_table[j].name_index) {
                    shader->bind_func = bind_func_table[j].bind_func;
                    break;
                }
        }
    }
#ifdef USE_OPENGL
    else {
        wchar_t temp_buf[MAX_PATH];
        if (FAILED(SHGetFolderPathW(0, CSIDL_LOCAL_APPDATA, 0, 0, temp_buf)))
            return;

        wcscat_s(temp_buf, sizeof(temp_buf) / sizeof(wchar_t), L"\\ReDIVA");
        path_create_directory(temp_buf);

        wchar_t buf[MAX_PATH];
        swprintf_s(buf, sizeof(buf) / sizeof(wchar_t), L"\\%hs_shader_cache", name);
        wcscat_s(temp_buf, sizeof(temp_buf) / sizeof(wchar_t), buf);

        bool shader_cache_changed = false;
        farc shader_cache_farc;
        swprintf_s(buf, sizeof(buf) / sizeof(wchar_t), L"%ls.farc", temp_buf);
        if (!ignore_cache && path_check_file_exists(buf))
            shader_cache_farc.read(buf, true, false);

        char vert_buf[MAX_PATH];
        char frag_buf[MAX_PATH];
        char vert_file_buf[MAX_PATH];
        char frag_file_buf[MAX_PATH];
        char shader_cache_file_name[MAX_PATH];

        GLsizei buffer_size = 0x100000;
        void* binary = force_malloc(buffer_size);
        std::string temp_vert;
        std::string temp_frag;
        std::vector<int32_t> vec_vert;
        std::vector<int32_t> vec_frag;
        std::vector<program_binary> program_data_binary;
        shader* shaders = force_malloc<shader>(size);
        this->shaders = shaders;
        this->size = size;
        for (size_t i = 0; i < size; i++) {
            shader* shader = &shaders[i];
            shader->name = shaders_table[i].name;
            shader->name_index = shaders_table[i].name_index;
            shader->num_sub = shaders_table[i].num_sub;
            shader->sub = force_malloc<shader_sub>(shader->num_sub);
            shader->num_uniform = shaders_table[i].num_uniform;
            shader->use_uniform = shaders_table[i].use_uniform;

            int32_t num_sub = shader->num_sub;
            const shader_sub_table* sub_table = shaders_table[i].sub;
            shader_sub* sub = shader->sub;
            vec_vert.resize(shader->num_uniform);
            vec_frag.resize(shader->num_uniform);
            int32_t* vec_vert_data = vec_vert.data();
            int32_t* vec_frag_data = vec_frag.data();
            for (int32_t j = 0; j < num_sub; j++, sub++, sub_table++) {
                sub->sub_index = sub_table->sub_index;
                sub->vp_unival_max = sub_table->vp_unival_max;
                sub->fp_unival_max = sub_table->fp_unival_max;

                strcpy_s(vert_file_buf, sizeof(vert_file_buf), sub_table->vp);
                strcat_s(vert_file_buf, sizeof(vert_file_buf), ".vert");
                farc_file* vert_ff = f->read_file(vert_file_buf);

                char* vert_data = 0;
                if (vert_ff && vert_ff->data) {
                    vert_data = force_malloc<char>(vert_ff->size + 1);
                    if (vert_data) {
                        memcpy(vert_data, vert_ff->data, vert_ff->size);
                        vert_data[vert_ff->size] = 0;
                    }
                }

                strcpy_s(frag_file_buf, sizeof(frag_file_buf), sub_table->fp);
                strcat_s(frag_file_buf, sizeof(frag_file_buf), ".frag");
                farc_file* frag_ff = f->read_file(frag_file_buf);

                char* frag_data = 0;
                if (frag_ff && frag_ff->data) {
                    frag_data = force_malloc<char>(frag_ff->size + 1);
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

                uint32_t uniform_vert_flags = 0;
                uint32_t uniform_frag_flags = 0;
                for (int32_t k = 0; k < shader->num_uniform; k++) {
                    if (sub_table->vp_unival_max[k] > 0)
                        uniform_vert_flags |= 1 << k;
                    if (sub_table->fp_unival_max[k] > 0)
                        uniform_frag_flags |= 1 << k;
                }

                char uniform_vert_flags_buf[9];
                char uniform_frag_flags_buf[9];
                for (int32_t k = 0, l = 7; k < 32; k += 4, l--) {
                    int32_t vert_flags_digit = (uniform_vert_flags >> k) & 0xF;
                    if (vert_flags_digit >= 0x00 && vert_flags_digit <= 0x09)
                        uniform_vert_flags_buf[l] = (char)('0' + vert_flags_digit);
                    else
                        uniform_vert_flags_buf[l] = (char)('A' + (vert_flags_digit - 0x0A));

                    int32_t frag_flags_digit = (uniform_frag_flags >> k) & 0xF;
                    if (frag_flags_digit >= 0x00 && frag_flags_digit <= 0x09)
                        uniform_frag_flags_buf[l] = (char)('0' + frag_flags_digit);
                    else
                        uniform_frag_flags_buf[l] = (char)('A' + (frag_flags_digit - 0x0A));
                }
                uniform_vert_flags_buf[8] = 0;
                uniform_frag_flags_buf[8] = 0;

                strcpy_s(vert_buf, sizeof(vert_buf), vert_file_buf);
                strcpy_s(frag_buf, sizeof(frag_buf), frag_file_buf);
                strcat_s(vert_buf, sizeof(vert_buf), ".");
                strcat_s(frag_buf, sizeof(frag_buf), ".");
                strcat_s(vert_buf, sizeof(vert_buf), uniform_vert_flags_buf);
                strcat_s(frag_buf, sizeof(frag_buf), uniform_frag_flags_buf);

                uint64_t vert_file_name_hash = hash_utf8_xxh3_64bits(vert_buf);
                uint64_t frag_file_name_hash = hash_utf8_xxh3_64bits(frag_buf);

                strcpy_s(shader_cache_file_name, sizeof(shader_cache_file_name), sub_table->vp);
                if (str_utils_compare(sub_table->vp, sub_table->fp)) {
                    strcat_s(shader_cache_file_name, sizeof(shader_cache_file_name), ".");
                    strcat_s(shader_cache_file_name, sizeof(shader_cache_file_name), sub_table->fp);
                }
                strcat_s(shader_cache_file_name, sizeof(shader_cache_file_name), ".bin");

                vert_data = shader::parse_include(vert_data, f);
                frag_data = shader::parse_include(frag_data, f);
                uint64_t vert_data_hash = hash_utf8_xxh3_64bits(vert_data);
                uint64_t frag_data_hash = hash_utf8_xxh3_64bits(frag_data);

                farc_file* shader_cache_file = shader_cache_farc.read_file(shader_cache_file_name);
                program_binary* bin = 0;
                if (!ignore_cache) {
                    if (!shader_cache_file || !shader_cache_file->data)
                        printf_debug("Shader not compiled: %s %s\n", vert_file_buf, frag_file_buf);
                    else if (vert_file_name_hash != ((uint64_t*)shader_cache_file->data)[0]
                        || frag_file_name_hash != ((uint64_t*)shader_cache_file->data)[1])
                        printf_debug("Shader flags not equal: %s %s\n", vert_file_buf, frag_file_buf);
                    else if (vert_data_hash != ((uint64_t*)shader_cache_file->data)[2]
                        || frag_data_hash != ((uint64_t*)shader_cache_file->data)[3])
                        printf_debug("Shader hash not equal: %s %s\n", vert_file_buf, frag_file_buf);
                    else
                        bin = (program_binary*)&((uint64_t*)shader_cache_file->data)[4];
                }

                if (shader->num_uniform > 0
                    && (sub_table->vp_unival_max[0] != -1 || sub_table->fp_unival_max[0] != -1)) {
                    int32_t num_uniform = shader->num_uniform;
                    int32_t unival_shad_curr = 1;
                    int32_t unival_shad_count = 1;
                    const int32_t* vp_unival_max = sub_table->vp_unival_max;
                    const int32_t* fp_unival_max = sub_table->fp_unival_max;
                    for (int32_t k = 0; k < num_uniform; k++) {
                        const int32_t unival_max = max_def(vp_unival_max[k], fp_unival_max[k]);
                        unival_shad_count += unival_shad_curr * unival_max;
                        unival_shad_curr *= unival_max + 1;
                    }

                    if (!ignore_cache)
                        program_data_binary.reserve(unival_shad_count);
                    GLuint* programs = force_malloc<GLuint>(unival_shad_count);
                    sub->programs = programs;
                    if (programs) {
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

                        for (int32_t k = 0; k < unival_shad_count; k++) {
                            for (int32_t l = 0, m = k; l < num_uniform; l++) {
                                int32_t unival_max = max_def(vp_unival_max[l], fp_unival_max[l]) + 1;
                                vec_vert_data[l] = min_def(m % unival_max, vp_unival_max[l]);
                                m /= unival_max;
                                vert_buf[vert_buf_pos + l] = (char)('0' + vec_vert_data[l]);
                            }

                            for (int32_t l = 0, m = k; l < num_uniform; l++) {
                                int32_t unival_max = max_def(vp_unival_max[l], fp_unival_max[l]) + 1;
                                vec_frag_data[l] = min_def(m % unival_max, fp_unival_max[l]);
                                m /= unival_max;
                                frag_buf[frag_buf_pos + l] = (char)('0' + vec_frag_data[l]);
                            }

                            if (!bin || !bin->binary_format || !bin->length
                                || !shader_load_binary_shader(bin, &programs[k], vert_buf, frag_buf)) {
                                bool vert_succ = shader::parse_define(vert_data,
                                    num_uniform, vec_vert_data, temp_vert);
                                bool frag_succ = shader::parse_define(frag_data,
                                    num_uniform, vec_frag_data, temp_frag);

                                if (ignore_cache)
                                    programs[k] = shader_compile(vert_succ ? temp_vert.c_str() : 0,
                                        frag_succ ? temp_frag.c_str() : 0, vert_buf, frag_buf);
                                else {
                                    program_data_binary.push_back({});
                                    programs[k] = shader_compile_binary(vert_succ ? temp_vert.c_str() : 0,
                                        frag_succ ? temp_frag.c_str() : 0, vert_buf, frag_buf,
                                        &program_data_binary.back(), &buffer_size, &binary);
                                }
                                shader_cache_changed |= programs[k] ? true : false;
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
                    GLuint* programs = force_malloc<GLuint>();
                    sub->programs = programs;
                    if (programs) {
                        strcpy_s(vert_buf, sizeof(vert_buf), sub_table->vp);
                        strcpy_s(frag_buf, sizeof(vert_buf), sub_table->fp);
                        strcat_s(vert_buf, sizeof(vert_buf), "..vert");
                        strcat_s(frag_buf, sizeof(vert_buf), "..frag");

                        if (!bin || !bin->binary_format || !bin->length
                            || !shader_load_binary_shader(bin, &programs[0], vert_buf, frag_buf)) {
                            bool vert_succ = shader::parse_define(vert_data, temp_vert);
                            bool frag_succ = shader::parse_define(frag_data, temp_frag);

                            if (ignore_cache)
                                programs[0] = shader_compile(vert_succ ? temp_vert.c_str() : 0,
                                    frag_succ ? temp_frag.c_str() : 0, vert_buf, frag_buf);
                            else {
                                program_data_binary.push_back({});
                                programs[0] = shader_compile_binary(vert_succ ? temp_vert.c_str() : 0,
                                    frag_succ ? temp_frag.c_str() : 0, vert_buf, frag_buf,
                                    &program_data_binary.back(), &buffer_size, &binary);
                            }
                            shader_cache_changed |= programs[0] ? true : false;
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
                    if (!shader_cache_file)
                        shader_cache_file = shader_cache_farc.add_file(shader_cache_file_name);
                    else
                        free_def(shader_cache_file->data);

                    size_t bin_count = program_data_binary.size();
                    size_t bin_size = sizeof(uint64_t) * 4 + bin_count * sizeof(program_binary);
                    for (program_binary& k : program_data_binary)
                        bin_size += align_val(k.length, 0x04);
                    shader_cache_file->data = force_malloc(bin_size);
                    shader_cache_file->size = bin_size;
                    shader_cache_file->compressed = true;
                    shader_cache_file->data_changed = true;

                    ((uint64_t*)shader_cache_file->data)[0] = vert_file_name_hash;
                    ((uint64_t*)shader_cache_file->data)[1] = frag_file_name_hash;
                    ((uint64_t*)shader_cache_file->data)[2] = vert_data_hash;
                    ((uint64_t*)shader_cache_file->data)[3] = frag_data_hash;
                    bin = (program_binary*)&((uint64_t*)shader_cache_file->data)[4];
                    size_t bin_data_base = (size_t)shader_cache_file->data + sizeof(uint64_t) * 4;
                    size_t bin_data = bin_data_base + bin_count * sizeof(program_binary);
                    for (program_binary& k : program_data_binary) {
                        bin->length = k.length;
                        bin->binary_format = k.binary_format;
                        bin->binary = bin_data - bin_data_base;
                        bin->hash = hash_xxh3_64bits((void*)k.binary, k.length);
                        memcpy((void*)bin_data, (void*)k.binary, k.length);
                        bin_data_base += sizeof(program_binary);
                        bin_data += align_val(k.length, 0x04);
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
                if (shader->name_index == bind_func_table[j].name_index) {
                    shader->bind_func = bind_func_table[j].bind_func;
                    break;
                }
        }
        free_def(binary);

        if (shader_cache_changed)
            shader_cache_farc.write(temp_buf, FARC_FArC, FARC_NONE, true, false);
    }
#endif

    this->get_index_by_name_func = get_index_by_name;
    this->get_name_by_index_func = get_name_by_index;
}

void shader_set_data::set(uint32_t index) {
    if (this && index && index != -1) {
        shader* shader = &shaders[index];
        if (shader->bind_func)
            shader->bind_func(this, shader);
        else
            shader->bind(this, shader->sub[0].sub_index);
    }
    else
        shader::unbind();
}

void shader_set_data::unload() {
    size_t size = this->size;
    shader* shaders = this->shaders;
    for (size_t i = 0; i < size; i++) {
        shader* shader = &shaders[i];
        if (!shader->sub)
            continue;

        int32_t num_sub = shader->num_sub;
        shader_sub* sub = shader->sub;
        for (int32_t j = 0; j < num_sub; j++, sub++) {
            int32_t unival_shad_count = 1;
            if (shader->num_uniform > 0) {
                int32_t num_uniform = shader->num_uniform;
                int32_t unival_shad_curr = 1;
                const int32_t* vp_unival_max = sub->vp_unival_max;
                const int32_t* fp_unival_max = sub->fp_unival_max;
                for (int32_t k = 0; k < num_uniform; k++) {
                    const int32_t unival_max = max_def(vp_unival_max[k], fp_unival_max[k]);
                    unival_shad_count += unival_shad_curr * unival_max;
                    unival_shad_curr *= unival_max + 1;
                }
            }

            if (sub->programs) {
                GLuint* programs = sub->programs;
                for (int32_t k = 0; k < unival_shad_count; k++)
                    glDeleteProgram(programs[k]);
                free(programs);
                sub->programs = 0;
            }
        }
        free(shader->sub);
        shader->sub = 0;
    }
    free_def(shaders);
    this->shaders = 0;

    get_index_by_name_func = 0;
    get_name_by_index_func = 0;
}

#ifdef USE_OPENGL
static GLuint shader_compile_shader(GLenum type, const char* data, const char* file) {
    if (!data)
        return 0;

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &data, 0);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        GLchar* info_log = force_malloc<GLchar>(length);
        glGetShaderInfoLog(shader, length, 0, info_log);
        const char* type_str = "Unknown";
        switch (type) {
        case GL_FRAGMENT_SHADER:
            type_str = "Fragment";
            break;
        case GL_VERTEX_SHADER:
            type_str = "Vertex";
            break;
        }
        printf_debug("%s shader compile error:\nfile: %s\n%s\n", type_str, file, info_log);

        wchar_t temp_buf[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathW(0, CSIDL_LOCAL_APPDATA, 0, 0, temp_buf))) {
            wcscat_s(temp_buf, sizeof(temp_buf) / sizeof(wchar_t), L"\\ReDIVA");
            temp_buf[sizeof(temp_buf) / sizeof(wchar_t) - 1] = 0;
            path_create_directory(temp_buf);

            wchar_t buf[MAX_PATH];
            swprintf_s(buf, sizeof(buf) / sizeof(wchar_t),
                L"%ls\\shader_error", temp_buf);
            buf[sizeof(buf) / sizeof(wchar_t) - 1] = 0;
            path_create_directory(buf);

            swprintf_s(buf, sizeof(buf) / sizeof(wchar_t),
                L"%ls\\shader_error\\%hs", temp_buf, file);
            buf[sizeof(buf) / sizeof(wchar_t) - 1] = 0;

            file_stream s;
            s.open(buf, L"wb");
            s.write_utf8_string(data);
            s.write_utf8_string("\n/*\n");
            s.write_utf8_string(info_log);
            s.write_utf8_string("*/\n");
        }

        free_def(info_log);
        glDeleteShader(shader);
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
        GLint length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        GLchar* info_log = force_malloc<GLchar>(length);
        glGetProgramInfoLog(program, length, 0, info_log);
        printf_debug("Program Shader Permut linking error:\nvp: %s; fp: %s\n%s\n", vp, fp, info_log);

        wchar_t temp_buf[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathW(0, CSIDL_LOCAL_APPDATA, 0, 0, temp_buf))) {
            wcscat_s(temp_buf, sizeof(temp_buf) / sizeof(wchar_t), L"\\ReDIVA");
            temp_buf[sizeof(temp_buf) / sizeof(wchar_t) - 1] = 0;
            path_create_directory(temp_buf);

            wchar_t buf[MAX_PATH];
            swprintf_s(buf, sizeof(buf) / sizeof(wchar_t),
                L"%ls\\shader_error", temp_buf);
            buf[sizeof(buf) / sizeof(wchar_t) - 1] = 0;
            path_create_directory(buf);

            swprintf_s(buf, sizeof(buf) / sizeof(wchar_t),
                L"%ls\\shader_error\\%hs", temp_buf, vp);
            buf[sizeof(buf) / sizeof(wchar_t) - 1] = 0;

            file_stream s;
            s.open(buf, L"wb");
            s.write_utf8_string(vert);
            s.write_utf8_string("\n/*\n");
            s.write_utf8_string(info_log);
            s.write_utf8_string("*/\n");
            s.close();

            swprintf_s(buf, sizeof(buf) / sizeof(wchar_t),
                L"%ls\\shader_error\\%hs", temp_buf, fp);
            buf[sizeof(buf) / sizeof(wchar_t) - 1] = 0;

            s.open(buf, L"wb");
            s.write_utf8_string(frag);
            s.write_utf8_string("\n/*\n");
            s.write_utf8_string(info_log);
            s.write_utf8_string("*/\n");
            s.close();
        }

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
        GLint length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        GLchar* info_log = force_malloc<GLchar>(length);
        glGetProgramInfoLog(program, length, 0, info_log);
        printf_debug("Program Shader Permut linking error:\nvp: %s; fp: %s\n%s\n", vp, fp, info_log);

        wchar_t temp_buf[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathW(0, CSIDL_LOCAL_APPDATA, 0, 0, temp_buf))) {
            wcscat_s(temp_buf, sizeof(temp_buf) / sizeof(wchar_t), L"\\ReDIVA");
            temp_buf[sizeof(temp_buf) / sizeof(wchar_t) - 1] = 0;
            path_create_directory(temp_buf);

            wchar_t buf[MAX_PATH];
            swprintf_s(buf, sizeof(buf) / sizeof(wchar_t),
                L"%ls\\shader_error", temp_buf);
            buf[sizeof(buf) / sizeof(wchar_t) - 1] = 0;
            path_create_directory(buf);

            swprintf_s(buf, sizeof(buf) / sizeof(wchar_t),
                L"%ls\\shader_error\\%hs", temp_buf, vp);
            buf[sizeof(buf) / sizeof(wchar_t) - 1] = 0;

            file_stream s;
            s.open(buf, L"wb");
            s.write_utf8_string(vert);
            s.write_utf8_string("\n/*\n");
            s.write_utf8_string(info_log);
            s.write_utf8_string("*/\n");
            s.close();

            swprintf_s(buf, sizeof(buf) / sizeof(wchar_t),
                L"%ls\\shader_error\\%hs", temp_buf, fp);
            buf[sizeof(buf) / sizeof(wchar_t) - 1] = 0;

            s.open(buf, L"wb");
            s.write_utf8_string(frag);
            s.write_utf8_string("\n/*\n");
            s.write_utf8_string(info_log);
            s.write_utf8_string("*/\n");
            s.close();
        }

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
            if (!gl_state_get_error())
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

static bool shader_load_binary_shader(program_binary* bin, GLuint* program, const char* vp, const char* fp) {
    if (bin->hash != hash_xxh3_64bits((void*)((size_t)bin + bin->binary), bin->length)) {
        printf_debug("Compiled binary hash could not be validated: %s %s\n", vp, fp);
        return false;
    }

    *program = glCreateProgram();
    glProgramBinary(*program, bin->binary_format, (void*)((size_t)bin + bin->binary), bin->length);
    GLint success = 0;
    glGetProgramiv(*program, GL_LINK_STATUS, &success);
    if (!success) {
        glDeleteProgram(*program);
        *program = 0;
        return false;
    }
    return true;
}
#endif

static prj::shared_ptr<Vulkan::ShaderModule> shader_load_spv_shader(program_spv* spv, const char* shader) {
    if (spv->hash != hash_xxh3_64bits((void*)((size_t)spv + spv->spv), spv->size)) {
        printf_debug("Compiled binary hash could not be validated: %s\n", shader);
        return {};
    }

    return prj::shared_ptr<Vulkan::ShaderModule>(new Vulkan::ShaderModule(
        Vulkan::current_device, (const void*)((size_t)spv + spv->spv), spv->size));
}

static bool shader_update_data(shader_set_data* set, GLenum mode, GLenum type, const void* indices) {
    if (!set || !set->curr_program)
        return false;

    if (set->primitive_restart) {
        gl_state_enable_primitive_restart();
        gl_state_set_primitive_restart_index(set->primitive_restart_index);
    }
    else
        gl_state_disable_primitive_restart();

    if (!Vulkan::use)
        return true;
    else if (!set->vp_desc || !set->fp_desc)
        return false;

    Vulkan::gl_program* vk_program = Vulkan::gl_program::get(set->curr_program);
    if (!vk_program)
        return false;

    Vulkan::gl_vertex_array* vk_vao = Vulkan::gl_vertex_array::get(gl_state.vertex_array_binding);
    if (!vk_vao || type && !vk_vao->index_buffer_binding.buffer)
        return false;

    VkPipelineShaderStageCreateInfo shader_stages[2] = {};

    VkPipelineShaderStageCreateInfo& vert_shader_stage_info = shader_stages[0];
    vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_shader_stage_info.module = *vk_program->vertex_shader_module.get();
    vert_shader_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo& frag_shader_stage_info = shader_stages[1];
    frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_shader_stage_info.module = *vk_program->fragment_shader_module.get();
    frag_shader_stage_info.pName = "main";

    uint32_t sampler_count = 0;
    uint32_t uniform_count = 0;
    uint32_t storage_count = 0;
    uint32_t push_constant_range_count = 0;
    uint32_t fragment_output_count = 0;

    bool enabled_attributes[Vulkan::MAX_VERTEX_ATTRIB_COUNT] = {};
    int32_t attribute_sizes[Vulkan::MAX_VERTEX_ATTRIB_COUNT] = {};

    const shader_description* vp_desc = set->vp_desc;
    while (vp_desc->type != SHADER_DESCRIPTION_NONE && vp_desc->type != SHADER_DESCRIPTION_END
        && vp_desc->type != SHADER_DESCRIPTION_MAX) {
        const shader_description* desc = vp_desc++;
        if (desc->use_uniform != U_INVALID && !uniform_value[desc->use_uniform])
            continue;

        switch (desc->type) {
        case SHADER_DESCRIPTION_VERTEX_INPUT:
            enabled_attributes[desc->binding] = true;
            attribute_sizes[desc->binding] = desc->data;
            break;
        case SHADER_DESCRIPTION_SAMPLER:
            sampler_count++;
            break;
        case SHADER_DESCRIPTION_UNIFORM:
            if (desc->binding == -1)
                push_constant_range_count++;
            else
                uniform_count++;
            break;
        case SHADER_DESCRIPTION_STORAGE:
            storage_count++;
            break;
        }
    }

    const shader_description* fp_desc = set->fp_desc;
    while (fp_desc->type != SHADER_DESCRIPTION_NONE && fp_desc->type != SHADER_DESCRIPTION_END
        && fp_desc->type != SHADER_DESCRIPTION_MAX) {
        const shader_description* desc = fp_desc++;
        if (desc->use_uniform != U_INVALID && !uniform_value[desc->use_uniform])
            continue;

        switch (desc->type) {
        case SHADER_DESCRIPTION_SAMPLER:
            sampler_count++;
            break;
        case SHADER_DESCRIPTION_UNIFORM:
            if (desc->binding == -1)
                push_constant_range_count++;
            else
                uniform_count++;
            break;
        case SHADER_DESCRIPTION_STORAGE:
            storage_count++;
            break;
        case SHADER_DESCRIPTION_FRAGMENT_OUTPUT:
            fragment_output_count++;
            break;
        }
    }

    const uint64_t vp_desc_hash = hash_xxh3_64bits(set->vp_desc,
        sizeof(shader_description) * (vp_desc - set->vp_desc));
    const uint64_t fp_desc_hash = hash_xxh3_64bits(set->fp_desc,
        sizeof(shader_description) * (fp_desc - set->fp_desc));
    const uint64_t unival_hash = set->unival_hash;

    prj::shared_ptr<Vulkan::DescriptorPipeline> descriptor_pipeline
        = Vulkan::manager_get_descriptor_pipeline(vp_desc_hash, fp_desc_hash, unival_hash);

    if (!descriptor_pipeline.get()) {
        const uint32_t sampler_max_count = sampler_count;
        const uint32_t uniform_max_count = uniform_count;
        const uint32_t storage_max_count = storage_count;

        VkDescriptorSetLayoutBinding* bindings = force_malloc<VkDescriptorSetLayoutBinding>(
            (size_t)sampler_max_count + uniform_max_count + storage_max_count + push_constant_range_count);
        VkDescriptorSetLayoutBinding* sampler_bindings = bindings;
        VkDescriptorSetLayoutBinding* uniform_bindings = sampler_bindings + sampler_max_count;
        VkDescriptorSetLayoutBinding* storage_bindings = bindings + sampler_max_count + uniform_max_count;
        VkPushConstantRange* push_constant_ranges = (VkPushConstantRange*)(bindings
            + sampler_max_count + uniform_max_count + storage_max_count);
        VkDescriptorSetLayoutBinding* sampler_binding = sampler_bindings;
        VkDescriptorSetLayoutBinding* uniform_binding = uniform_bindings;
        VkDescriptorSetLayoutBinding* storage_binding = storage_bindings;
        VkPushConstantRange* push_constant_range = push_constant_ranges;

        vp_desc = set->vp_desc;
        while (vp_desc->type != SHADER_DESCRIPTION_NONE && vp_desc->type != SHADER_DESCRIPTION_END
            && vp_desc->type != SHADER_DESCRIPTION_MAX) {
            const shader_description* desc = vp_desc++;
            if (desc->use_uniform != U_INVALID && !uniform_value[desc->use_uniform])
                continue;

            bool found = false;
            switch (desc->type) {
            case SHADER_DESCRIPTION_SAMPLER:
                sampler_count = (uint32_t)(sampler_binding - sampler_bindings);
                for (uint32_t i = 0; i < sampler_count; i++)
                    if (sampler_bindings[i].descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
                        && sampler_bindings[i].binding == desc->binding) {
                        found = true;
                        break;
                    }

                if (!found) {
                    sampler_binding->binding = desc->binding;
                    sampler_binding->descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    sampler_binding->descriptorCount = 1;
                    sampler_binding->stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
                    sampler_binding->pImmutableSamplers = 0;
                    sampler_binding++;
                }
                break;
            case SHADER_DESCRIPTION_UNIFORM:
                if (desc->binding == -1) {
                    push_constant_range_count = (uint32_t)(push_constant_range - push_constant_ranges);
                    for (uint32_t i = 0; i < push_constant_range_count; i++)
                        if (push_constant_ranges[i].size == desc->data) {
                            push_constant_ranges[i].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
                            found = true;
                            break;
                        }

                    if (!found && !push_constant_range_count) {
                        push_constant_range->stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
                        push_constant_range->offset = 0;
                        push_constant_range->size = desc->data;
                        push_constant_range++;
                    }
                    break;
                }

                uniform_count = (uint32_t)(uniform_binding - uniform_bindings);
                for (uint32_t i = 0; i < uniform_count; i++)
                    if (uniform_bindings[i].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
                        && uniform_bindings[i].binding == desc->binding) {
                        found = true;
                        break;
                    }

                if (!found) {
                    uniform_binding->binding = desc->binding;
                    uniform_binding->descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
                    uniform_binding->descriptorCount = 1;
                    uniform_binding->stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
                    uniform_binding->pImmutableSamplers = 0;
                    uniform_binding++;
                }
                break;
            case SHADER_DESCRIPTION_STORAGE:
                storage_count = (uint32_t)(storage_binding - storage_bindings);
                for (uint32_t i = 0; i < storage_count; i++)
                    if (storage_bindings[i].descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC
                        && storage_bindings[i].binding == desc->binding) {
                        found = true;
                        break;
                    }

                if (!found) {
                    storage_binding->binding = desc->binding;
                    storage_binding->descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
                    storage_binding->descriptorCount = 1;
                    storage_binding->stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
                    storage_binding->pImmutableSamplers = 0;
                    storage_binding++;
                }
                break;
            }
        }

        fp_desc = set->fp_desc;
        while (fp_desc->type != SHADER_DESCRIPTION_NONE && fp_desc->type != SHADER_DESCRIPTION_END
            && fp_desc->type != SHADER_DESCRIPTION_MAX) {
            const shader_description* desc = fp_desc++;
            if (desc->use_uniform != U_INVALID && !uniform_value[desc->use_uniform])
                continue;

            bool found = false;
            switch (desc->type) {
            case SHADER_DESCRIPTION_SAMPLER:
                sampler_count = (uint32_t)(sampler_binding - sampler_bindings);
                for (uint32_t i = 0; i < sampler_count; i++)
                    if (sampler_bindings[i].descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
                        && sampler_bindings[i].binding == desc->binding) {
                        sampler_bindings[i].stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
                        found = true;
                        break;
                    }

                if (!found) {
                    sampler_binding->descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    sampler_binding->binding = desc->binding;
                    sampler_binding->descriptorCount = 1;
                    sampler_binding->stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
                    sampler_binding->pImmutableSamplers = 0;
                    sampler_binding++;
                }
                break;
            case SHADER_DESCRIPTION_UNIFORM:
                if (desc->binding == -1) {
                    push_constant_range_count = (uint32_t)(push_constant_range - push_constant_ranges);
                    for (uint32_t i = 0; i < push_constant_range_count; i++)
                        if (push_constant_ranges[i].size == desc->data) {
                            push_constant_ranges[i].stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
                            found = true;
                            break;
                        }

                    if (!found && !push_constant_range_count) {
                        push_constant_range->stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
                        push_constant_range->offset = 0;
                        push_constant_range->size = desc->data;
                        push_constant_range++;
                    }
                    break;
                }

                uniform_count = (uint32_t)(uniform_binding - uniform_bindings);
                for (uint32_t i = 0; i < uniform_count; i++)
                    if (uniform_bindings[i].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
                        && uniform_bindings[i].binding == desc->binding) {
                        uniform_bindings[i].stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
                        found = true;
                        break;
                    }

                if (!found) {
                    uniform_binding->descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
                    uniform_binding->binding = desc->binding;
                    uniform_binding->descriptorCount = 1;
                    uniform_binding->stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
                    uniform_binding->pImmutableSamplers = 0;
                    uniform_binding++;
                }
                break;
            case SHADER_DESCRIPTION_STORAGE:
                storage_count = (uint32_t)(storage_binding - storage_bindings);
                for (uint32_t i = 0; i < storage_count; i++)
                    if (storage_bindings[i].descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC
                        && storage_bindings[i].binding == desc->binding) {
                        storage_bindings[i].stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
                        found = true;
                        break;
                    }

                if (!found) {
                    storage_binding->descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
                    storage_binding->binding = desc->binding;
                    storage_binding->descriptorCount = 1;
                    storage_binding->stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
                    storage_binding->pImmutableSamplers = 0;
                    storage_binding++;
                }
                break;
            }
        }

        sampler_count = (uint32_t)(sampler_binding - sampler_bindings);
        uniform_count = (uint32_t)(uniform_binding - uniform_bindings);
        storage_count = (uint32_t)(storage_binding - storage_bindings);
        push_constant_range_count = (uint32_t)(push_constant_range - push_constant_ranges);

        if (uniform_count)
            memmove(bindings + sampler_count,
                bindings + sampler_max_count,
                uniform_count * sizeof(VkDescriptorSetLayoutBinding));
        if (storage_count)
            memmove(bindings + sampler_count + uniform_count,
                bindings + sampler_max_count + uniform_max_count,
                storage_count * sizeof(VkDescriptorSetLayoutBinding));
        if (push_constant_range_count)
            memmove(bindings + sampler_count + uniform_count + storage_count,
                bindings + sampler_max_count + uniform_max_count + storage_max_count,
                push_constant_range_count * sizeof(VkPushConstantRange));

        descriptor_pipeline
            = Vulkan::manager_get_descriptor_pipeline(vp_desc_hash, fp_desc_hash, unival_hash,
                sampler_count, uniform_count, storage_count,
                bindings, push_constant_range_count, push_constant_ranges);

        free_def(bindings);
    }

    uint32_t binding_description_count = 0;
    VkVertexInputBindingDescription binding_descriptions[Vulkan::MAX_VERTEX_ATTRIB_COUNT];
    uint32_t attribute_description_count = 0;
    VkVertexInputAttributeDescription attribute_descriptions[Vulkan::MAX_VERTEX_ATTRIB_COUNT];
    {
        uint32_t binding = 0;
        for (Vulkan::gl_vertex_buffer_binding_data& i : vk_vao->vertex_buffer_bindings) {
            if (!i.buffer)
                continue;

            VkVertexInputBindingDescription& binding_desc = binding_descriptions[binding_description_count++];
            binding_desc.binding = binding;
            binding_desc.stride = i.stride;
            binding_desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            binding++;
        }

        bool use_dummy = false;
        for (uint32_t i = 0; i < Vulkan::MAX_VERTEX_ATTRIB_COUNT; i++) {
            if (!enabled_attributes[i])
                continue;
            else if (vk_vao->vertex_attribs[i].binding != -1) {
                VkVertexInputAttributeDescription& attribute_desc
                    = attribute_descriptions[attribute_description_count++];
                attribute_desc.location = i;
                attribute_desc.binding = vk_vao->vertex_attribs[i].binding;
                attribute_desc.format = vk_vao->vertex_attribs[i].format;
                attribute_desc.offset = vk_vao->vertex_attribs[i].offset;
                continue;
            }

            use_dummy = true;

            uint32_t offset;
            if (vk_vao->vertex_attribs[i].generic_value == vec4(0.0f, 0.0f, 0.0f, 0.0f))
                offset = sizeof(float_t) * 4 * 0;
            else if (vk_vao->vertex_attribs[i].generic_value != vec4(1.0f, 1.0f, 1.0f, 1.0f))
                offset = sizeof(float_t) * 4 * 1;
            else
                offset = sizeof(float_t) * 4 * 2;

            VkFormat format;
            switch (attribute_sizes[i]) {
            case 1:
                format = VK_FORMAT_R32_SFLOAT;
                break;
            case 2:
                format = VK_FORMAT_R32G32_SFLOAT;
                break;
            case 3:
                format = VK_FORMAT_R32G32B32_SFLOAT;
                break;
            case 4:
            default:
                format = VK_FORMAT_R32G32B32A32_SFLOAT;
                break;
            }

            VkVertexInputAttributeDescription& attribute_desc
                = attribute_descriptions[attribute_description_count++];
            attribute_desc.location = i;
            attribute_desc.binding = binding;
            attribute_desc.format = format;
            attribute_desc.offset = offset;
        }

        if (use_dummy) {
            VkVertexInputBindingDescription& binding_desc = binding_descriptions[binding_description_count++];
            binding_desc.binding = binding;
            binding_desc.stride = 0;
            binding_desc.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
        }
    }

    VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount = binding_description_count;
    vertex_input_info.pVertexBindingDescriptions = binding_descriptions;
    vertex_input_info.vertexAttributeDescriptionCount = attribute_description_count;
    vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions;

    VkPipelineInputAssemblyStateCreateInfo input_assembly_state = {};
    input_assembly_state.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    switch (mode) {
    case GL_LINES:
        input_assembly_state.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        input_assembly_state.primitiveRestartEnable = VK_FALSE;
        break;
    case GL_LINE_STRIP:
        input_assembly_state.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
        input_assembly_state.primitiveRestartEnable = set->primitive_restart ? VK_TRUE : VK_FALSE;
        break;
    case GL_TRIANGLES:
        input_assembly_state.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly_state.primitiveRestartEnable = VK_FALSE;
        break;
    case GL_TRIANGLE_STRIP:
        input_assembly_state.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        input_assembly_state.primitiveRestartEnable = set->primitive_restart ? VK_TRUE : VK_FALSE;
        break;
    case GL_TRIANGLE_FAN:
        input_assembly_state.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
        input_assembly_state.primitiveRestartEnable = set->primitive_restart ? VK_TRUE : VK_FALSE;
        break;
    default:
        return false;
    }

    VkRect2D viewport_scissor_rect[2];
    viewport_scissor_rect[0] = *(VkRect2D*)&gl_state.viewport;
    viewport_scissor_rect[1] = gl_state.scissor_test
        ? *(VkRect2D*)&gl_state.scissor_box : *(VkRect2D*)&gl_state.viewport;

    VkPipelineRasterizationStateCreateInfo rasterization_state = {};
    rasterization_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization_state.depthClampEnable = VK_FALSE;
    rasterization_state.rasterizerDiscardEnable = VK_FALSE;
    rasterization_state.polygonMode = Vulkan::get_polygon_mode(gl_state.polygon_mode);
    rasterization_state.lineWidth = gl_state.line_width;
    rasterization_state.cullMode = Vulkan::get_cull_mode_flags(
        gl_state.cull_face ? gl_state.cull_face_mode : GL_NONE);
    rasterization_state.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterization_state.depthBiasEnable = VK_FALSE;

    VkPipelineDepthStencilStateCreateInfo depth_stencil_state = {};
    depth_stencil_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil_state.depthTestEnable = gl_state.depth_test ? VK_TRUE : VK_FALSE;
    depth_stencil_state.depthWriteEnable = gl_state.depth_test && gl_state.depth_mask ? VK_TRUE : VK_FALSE;
    depth_stencil_state.depthCompareOp = Vulkan::get_compare_op(
        gl_state.depth_test ? gl_state.depth_func : GL_ALWAYS);
    depth_stencil_state.depthBoundsTestEnable = VK_FALSE;
    depth_stencil_state.stencilTestEnable = VK_FALSE;
    depth_stencil_state.minDepthBounds = 0.0f;
    depth_stencil_state.maxDepthBounds = 1.0f;

    uint32_t color_blend_attachment_count = fragment_output_count;
    VkPipelineColorBlendAttachmentState* color_blend_attachments
        = force_malloc<VkPipelineColorBlendAttachmentState>(color_blend_attachment_count);
    for (uint32_t i = 0; i < color_blend_attachment_count; i++) {
        VkPipelineColorBlendAttachmentState& color_blend_attachment = color_blend_attachments[i];
        VkColorComponentFlags color_write_mask = 0;
        color_write_mask |= gl_state.color_mask[0] ? VK_COLOR_COMPONENT_R_BIT : 0;
        color_write_mask |= gl_state.color_mask[1] ? VK_COLOR_COMPONENT_G_BIT : 0;
        color_write_mask |= gl_state.color_mask[2] ? VK_COLOR_COMPONENT_B_BIT : 0;
        color_write_mask |= gl_state.color_mask[3] ? VK_COLOR_COMPONENT_A_BIT : 0;
        color_blend_attachment.colorWriteMask = color_write_mask;
        color_blend_attachment.blendEnable = gl_state.blend ? VK_TRUE : VK_FALSE;
        color_blend_attachment.srcColorBlendFactor = Vulkan::get_blend_factor(gl_state.blend_src_rgb);
        color_blend_attachment.dstColorBlendFactor = Vulkan::get_blend_factor(gl_state.blend_dst_rgb);
        color_blend_attachment.colorBlendOp = Vulkan::get_blend_op(gl_state.blend_mode_rgb);
        color_blend_attachment.srcAlphaBlendFactor = Vulkan::get_blend_factor(gl_state.blend_src_alpha);
        color_blend_attachment.dstAlphaBlendFactor = Vulkan::get_blend_factor(gl_state.blend_dst_alpha);
        color_blend_attachment.alphaBlendOp = Vulkan::get_blend_op(gl_state.blend_mode_alpha);
    }

    Vulkan::DescriptorPipeline* vk_descriptor_pipeline = descriptor_pipeline.get();

    VkPipelineLayout pipeline_layout = vk_descriptor_pipeline->GetPipelineLayout();

    extern VkRenderPassBeginInfo vulkan_swapchain_render_pass_info;
    VkRenderPass render_pass;
    if (gl_state.draw_framebuffer_binding)
        render_pass = *Vulkan::gl_framebuffer::get(gl_state.draw_framebuffer_binding)->render_pass.get();
    else
        render_pass = vulkan_swapchain_render_pass_info.renderPass;

    if (Vulkan::current_render_pass != render_pass)
        Vulkan::end_render_pass(Vulkan::current_command_buffer);

    VkPipeline pipeline = *Vulkan::manager_get_pipeline(2, shader_stages,
        binding_description_count, binding_descriptions,
        attribute_description_count, attribute_descriptions, &input_assembly_state,
        viewport_scissor_rect, &rasterization_state, &depth_stencil_state,
        color_blend_attachment_count, color_blend_attachments, pipeline_layout, render_pass).get();

    free_def(color_blend_attachments);

    size_t descriptor_infos_size = sizeof(VkDescriptorImageInfo) * sampler_count
        + sizeof(VkDescriptorBufferInfo) * ((size_t)uniform_count + storage_count)
        + sizeof(uint32_t) * ((size_t)sampler_count + uniform_count + storage_count)
        + sizeof(std::pair<uint32_t, uint32_t>) * ((size_t)uniform_count + storage_count)
        + sizeof(uint32_t) * ((size_t)uniform_count + storage_count);
    void* descriptor_infos = force_malloc(descriptor_infos_size);

    VkDescriptorImageInfo* sampler_infos = (VkDescriptorImageInfo*)descriptor_infos;
    VkDescriptorImageInfo* sampler_info = sampler_infos;

    VkDescriptorBufferInfo* uniform_infos = (VkDescriptorBufferInfo*)(sampler_infos + sampler_count);
    VkDescriptorBufferInfo* uniform_info = uniform_infos;

    VkDescriptorBufferInfo* storage_infos = (VkDescriptorBufferInfo*)(uniform_infos + uniform_count);
    VkDescriptorBufferInfo* storage_info = storage_infos;

    uint32_t* sampler_info_bindings = (uint32_t*)(storage_infos + storage_count);
    uint32_t* sampler_info_binding = sampler_info_bindings;

    uint32_t* uniform_info_bindings = (uint32_t*)(sampler_info_bindings + sampler_count);
    uint32_t* uniform_info_binding = uniform_info_bindings;

    uint32_t* storage_info_bindings = (uint32_t*)(uniform_info_bindings + uniform_count);
    uint32_t* storage_info_binding = storage_info_bindings;

    std::pair<uint32_t, uint32_t>* dynamic_infos
        = (std::pair<uint32_t, uint32_t>*)(storage_info_bindings + storage_count);
    std::pair<uint32_t, uint32_t>* dynamic_info = dynamic_infos;

    uint32_t* dynamic_offsets = (uint32_t*)(dynamic_infos + uniform_count + storage_count);

    uint8_t* push_constant_data = 0;
    uint32_t push_constant_data_size = 0;
    VkShaderStageFlags push_constant_stage_flags = 0;

    vp_desc = set->vp_desc;
    while (vp_desc->type != SHADER_DESCRIPTION_NONE && vp_desc->type != SHADER_DESCRIPTION_END
        && vp_desc->type != SHADER_DESCRIPTION_MAX) {
        const shader_description* desc = vp_desc++;
        if (desc->use_uniform != U_INVALID && !uniform_value[desc->use_uniform])
            continue;

        bool found = false;
        switch (desc->type) {
        case SHADER_DESCRIPTION_SAMPLER:
            sampler_count = (uint32_t)(sampler_info - sampler_infos);
            for (uint32_t i = 0; i < sampler_count; i++)
                if (sampler_info_bindings[i] == desc->binding) {
                    found = true;
                    break;
                }

            if (!found) {
                GLuint texture = 0;
                switch (desc->data) {
                case 0:
                    texture = gl_state.texture_binding_2d[desc->binding];
                    break;
                case 1:
                    texture = gl_state.texture_binding_cube_map[desc->binding];
                    break;
                }

                Vulkan::gl_texture* vk_tex = Vulkan::gl_texture::get(texture);
                if (!vk_tex)
                    break;

                Vulkan::gl_sampler* sampler_data = &vk_tex->sampler_data;
                GLuint sampler = gl_state.sampler_binding[desc->binding];
                if (sampler) {
                    Vulkan::gl_sampler* vk_samp = Vulkan::gl_sampler::get(sampler);
                    if (vk_samp)
                        sampler_data = vk_samp;
                }

                const VkImageAspectFlags aspect_mask = Vulkan::get_aspect_mask(vk_tex->internal_format);
                const int32_t level_count = vk_tex->max_mipmap_level + 1;
                const int32_t layer_count = vk_tex->target == GL_TEXTURE_CUBE_MAP ? 6 : 1;

                Vulkan::Image::PipelineBarrier(Vulkan::current_command_buffer, vk_tex->image,
                    aspect_mask, level_count, layer_count, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

                sampler_info->sampler = *Vulkan::manager_get_sampler(*sampler_data).get();
                sampler_info->imageView = vk_tex->get_image_view();
                sampler_info->imageLayout = vk_tex->image.GetImageLayout(0, 0);
                sampler_info++;
                *sampler_info_binding++ = desc->binding;
            }
            break;
        case SHADER_DESCRIPTION_UNIFORM:
            if (desc->binding == -1) {
                Vulkan::gl_buffer* vk_buf = Vulkan::gl_buffer::get(gl_state.uniform_buffer_bindings[0]);
                if (!vk_buf)
                    break;

                if (!push_constant_data) {
                    push_constant_data = vk_buf->data.data();
                    push_constant_data_size = (uint32_t)vk_buf->data.size();
                }
                push_constant_stage_flags |= VK_SHADER_STAGE_VERTEX_BIT;
                break;
            }

            uniform_count = (uint32_t)(uniform_info - uniform_infos);
            for (uint32_t i = 0; i < uniform_count; i++)
                if (uniform_info_bindings[i] == desc->binding) {
                    found = true;
                    break;
                }

            if (!found) {
                Vulkan::gl_uniform_buffer* vk_ub = Vulkan::gl_uniform_buffer::get(
                    gl_state.uniform_buffer_bindings[desc->binding]);
                if (!vk_ub)
                    break;

                const GLintptr gl_offset = gl_state.uniform_buffer_offsets[desc->binding];
                const GLsizeiptr gl_size = gl_state.uniform_buffer_sizes[desc->binding];

                VkDeviceSize offset = vk_ub->working_buffer.GetOffset() + (VkDeviceSize)gl_offset;
                VkDeviceSize range = gl_size != -1 ? (VkDeviceSize)gl_size : desc->data;

                uniform_info->buffer = vk_ub->working_buffer;
                uniform_info->offset = 0;
                uniform_info->range = range;
                uniform_info++;
                *uniform_info_binding++ = desc->binding;

                dynamic_info->first = desc->binding & 0x7FFFFFFF;
                dynamic_info->second = (uint32_t)offset;
                dynamic_info++;
            }
            break;
        case SHADER_DESCRIPTION_STORAGE:
            storage_count = (uint32_t)(storage_info - storage_infos);
            for (uint32_t i = 0; i < storage_count; i++)
                if (storage_info_bindings[i] == desc->binding) {
                    found = true;
                    break;
                }

            if (!found) {
                GLuint buffer = gl_state.shader_storage_buffer_bindings[desc->binding];
                Vulkan::gl_storage_buffer* vk_sb = Vulkan::gl_storage_buffer::get(buffer);
                if (!vk_sb)
                    break;

                const GLintptr gl_offset = gl_state.shader_storage_buffer_offsets[desc->binding];
                const GLsizeiptr gl_size = gl_state.shader_storage_buffer_sizes[desc->binding];

                VkDeviceSize offset = vk_sb->working_buffer.GetOffset() + (VkDeviceSize)gl_offset;
                VkDeviceSize range = gl_size != -1 ? (VkDeviceSize)gl_size : desc->data;

                storage_info->buffer = vk_sb->working_buffer;
                storage_info->offset = 0;
                storage_info->range = range;
                storage_info++;
                *storage_info_binding++ = desc->binding;

                dynamic_info->first = 0x80000000 | (desc->binding & 0x7FFFFFFF);
                dynamic_info->second = (uint32_t)offset;
                dynamic_info++;
            }
            break;
        }
    }

    fp_desc = set->fp_desc;
    while (fp_desc->type != SHADER_DESCRIPTION_NONE && fp_desc->type != SHADER_DESCRIPTION_END
        && fp_desc->type != SHADER_DESCRIPTION_MAX) {
        const shader_description* desc = fp_desc++;
        if (desc->use_uniform != U_INVALID && !uniform_value[desc->use_uniform])
            continue;

        bool found = false;
        switch (desc->type) {
        case SHADER_DESCRIPTION_SAMPLER:
            sampler_count = (uint32_t)(sampler_info - sampler_infos);
            for (uint32_t i = 0; i < sampler_count; i++)
                if (sampler_info_bindings[i] == desc->binding) {
                    found = true;
                    break;
                }

            if (!found) {
                GLuint texture = 0;
                switch (desc->data) {
                case 0:
                    texture = gl_state.texture_binding_2d[desc->binding];
                    break;
                case 1:
                    texture = gl_state.texture_binding_cube_map[desc->binding];
                    break;
                }

                Vulkan::gl_texture* vk_tex = Vulkan::gl_texture::get(texture);
                if (!vk_tex)
                    break;

                Vulkan::gl_sampler* sampler_data = &vk_tex->sampler_data;
                GLuint sampler = gl_state.sampler_binding[desc->binding];
                if (sampler) {
                    Vulkan::gl_sampler* vk_samp = Vulkan::gl_sampler::get(sampler);
                    if (vk_samp)
                        sampler_data = vk_samp;
                }

                const VkImageAspectFlags aspect_mask = Vulkan::get_aspect_mask(vk_tex->internal_format);
                const int32_t level_count = vk_tex->max_mipmap_level + 1;
                const int32_t layer_count = vk_tex->target == GL_TEXTURE_CUBE_MAP ? 6 : 1;

                Vulkan::Image::PipelineBarrier(Vulkan::current_command_buffer, vk_tex->image,
                    aspect_mask, level_count, layer_count, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

                sampler_info->sampler = *Vulkan::manager_get_sampler(*sampler_data).get();
                sampler_info->imageView = vk_tex->get_image_view();
                sampler_info->imageLayout = vk_tex->image.GetImageLayout(0, 0);
                sampler_info++;
                *sampler_info_binding++ = desc->binding;
            }
            break;
        case SHADER_DESCRIPTION_UNIFORM:
            if (desc->binding == -1) {
                Vulkan::gl_buffer* vk_buf = Vulkan::gl_buffer::get(gl_state.uniform_buffer_bindings[0]);
                if (!vk_buf)
                    break;

                if (!push_constant_data) {
                    push_constant_data = vk_buf->data.data();
                    push_constant_data_size = (uint32_t)vk_buf->data.size();
                }
                push_constant_stage_flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
                break;
            }

            uniform_count = (uint32_t)(uniform_info - uniform_infos);
            for (uint32_t i = 0; i < uniform_count; i++)
                if (uniform_info_bindings[i] == desc->binding) {
                    found = true;
                    break;
                }

            if (!found) {
                Vulkan::gl_uniform_buffer* vk_ub = Vulkan::gl_uniform_buffer::get(
                    gl_state.uniform_buffer_bindings[desc->binding]);
                if (!vk_ub)
                    break;

                const GLintptr gl_offset = gl_state.uniform_buffer_offsets[desc->binding];
                const GLsizeiptr gl_size = gl_state.uniform_buffer_sizes[desc->binding];

                VkDeviceSize offset = vk_ub->working_buffer.GetOffset() + (VkDeviceSize)gl_offset;
                VkDeviceSize range = gl_size != -1 ? (VkDeviceSize)gl_size : desc->data;

                uniform_info->buffer = vk_ub->working_buffer;
                uniform_info->offset = 0;
                uniform_info->range = range;
                uniform_info++;
                *uniform_info_binding++ = desc->binding;

                dynamic_info->first = desc->binding & 0x7FFFFFFF;
                dynamic_info->second = (uint32_t)offset;
                dynamic_info++;
            }
            break;
        case SHADER_DESCRIPTION_STORAGE:
            storage_count = (uint32_t)(storage_info - storage_infos);
            for (uint32_t i = 0; i < storage_count; i++)
                if (storage_info_bindings[i] == desc->binding) {
                    found = true;
                    break;
                }

            if (!found) {
                GLuint buffer = gl_state.shader_storage_buffer_bindings[desc->binding];
                Vulkan::gl_storage_buffer* vk_sb = Vulkan::gl_storage_buffer::get(buffer);
                if (!vk_sb)
                    break;

                const GLintptr gl_offset = gl_state.shader_storage_buffer_offsets[desc->binding];
                const GLsizeiptr gl_size = gl_state.shader_storage_buffer_sizes[desc->binding];

                VkDeviceSize offset = vk_sb->working_buffer.GetOffset() + (VkDeviceSize)gl_offset;
                VkDeviceSize range = gl_size != -1 ? (VkDeviceSize)gl_size : desc->data;

                storage_info->buffer = vk_sb->working_buffer;
                storage_info->offset = 0;
                storage_info->range = range;
                storage_info++;
                *storage_info_binding++ = desc->binding;

                dynamic_info->first = 0x80000000 | (desc->binding & 0x7FFFFFFF);
                dynamic_info->second = (uint32_t)offset;
                dynamic_info++;
            }
            break;
        }
    }

    sampler_count = (uint32_t)(sampler_info - sampler_infos);
    uniform_count = (uint32_t)(uniform_info - uniform_infos);
    storage_count = (uint32_t)(storage_info - storage_infos);

    Vulkan::DescriptorPipeline::DescriptorSetCollection* descriptor_set_collection
        = vk_descriptor_pipeline->GetDescriptorSetCollection(Vulkan::manager_get_frame(),
            hash_xxh3_64bits(descriptor_infos, descriptor_infos_size));
    if (!descriptor_set_collection) {
        free_def(descriptor_infos);
        return false;
    }

    if (!descriptor_set_collection->used && (sampler_count + uniform_count + storage_count)) {
        uint32_t descriptor_write_count = sampler_count + uniform_count + storage_count;
        VkWriteDescriptorSet* descriptor_writes = force_malloc<VkWriteDescriptorSet>(descriptor_write_count);
        VkWriteDescriptorSet* descriptor_write = descriptor_writes;

        VkDescriptorSet* descriptor_set = descriptor_set_collection->data;
        VkDescriptorSet sampler_descriptor_set = 0;
        if (sampler_count) {
            sampler_descriptor_set = *descriptor_set++;
            if (!sampler_descriptor_set) {
                free_def(descriptor_infos);
                return false;
            }
        }

        VkDescriptorSet uniform_descriptor_set = 0;
        if (uniform_count) {
            uniform_descriptor_set = *descriptor_set++;
            if (!uniform_descriptor_set) {
                free_def(descriptor_infos);
                return false;
            }
        }

        VkDescriptorSet storage_descriptor_set = 0;
        if (storage_count) {
            storage_descriptor_set = *descriptor_set++;
            if (!storage_descriptor_set) {
                free_def(descriptor_infos);
                return false;
            }
        }

        for (uint32_t i = 0; i < sampler_count; i++) {
            descriptor_write->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptor_write->pNext = 0;
            descriptor_write->dstSet = sampler_descriptor_set;
            descriptor_write->dstBinding = sampler_info_bindings[i];
            descriptor_write->dstArrayElement = 0;
            descriptor_write->descriptorCount = 1;
            descriptor_write->descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptor_write->pImageInfo = &sampler_infos[i];
            descriptor_write->pBufferInfo = 0;
            descriptor_write->pTexelBufferView = 0;
            descriptor_write++;
        }

        for (uint32_t i = 0; i < uniform_count; i++) {
            descriptor_write->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptor_write->pNext = 0;
            descriptor_write->dstSet = uniform_descriptor_set;
            descriptor_write->dstBinding = uniform_info_bindings[i];
            descriptor_write->dstArrayElement = 0;
            descriptor_write->descriptorCount = 1;
            descriptor_write->descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            descriptor_write->pImageInfo = 0;
            descriptor_write->pBufferInfo = &uniform_infos[i];
            descriptor_write->pTexelBufferView = 0;
            descriptor_write++;
        }

        for (uint32_t i = 0; i < storage_count; i++) {
            descriptor_write->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptor_write->pNext = 0;
            descriptor_write->dstSet = storage_descriptor_set;
            descriptor_write->dstBinding = storage_info_bindings[i];
            descriptor_write->dstArrayElement = 0;
            descriptor_write->descriptorCount = 1;
            descriptor_write->descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
            descriptor_write->pImageInfo = 0;
            descriptor_write->pBufferInfo = &storage_infos[i];
            descriptor_write->pTexelBufferView = 0;
            descriptor_write++;
        }

        vkUpdateDescriptorSets(Vulkan::current_device, descriptor_write_count, descriptor_writes, 0, 0);
        descriptor_set_collection->used = true;

        free_def(descriptor_writes);
    }

    GLuint query = Vulkan::gl_wrap_manager_get_query_samples_passed();
    if (query) {
        Vulkan::end_render_pass(Vulkan::current_command_buffer);
        Vulkan::gl_query::get(query)->query.Reset(Vulkan::current_command_buffer);
    }

    if (gl_state.draw_framebuffer_binding) {
        Vulkan::gl_framebuffer* vk_fbo = Vulkan::gl_framebuffer::get(gl_state.draw_framebuffer_binding);
        if (!vk_fbo->framebuffer)
            return false;

        if (Vulkan::current_framebuffer != vk_fbo->framebuffer) {
            Vulkan::end_render_pass(Vulkan::current_command_buffer);

            VkRenderPassBeginInfo render_pass_info = {};
            render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            render_pass_info.renderPass = *vk_fbo->render_pass.get();
            render_pass_info.framebuffer = vk_fbo->framebuffer;
            render_pass_info.renderArea.offset = { 0, 0 };
            render_pass_info.renderArea.extent = vk_fbo->framebuffer.GetExtent();
            render_pass_info.clearValueCount = 0;
            render_pass_info.pClearValues = 0;

            for (uint32_t i = 0; i < Vulkan::MAX_DRAW_BUFFERS; i++) {
                GLenum draw_buffer = vk_fbo->draw_buffers[i];
                if (!draw_buffer || draw_buffer < GL_COLOR_ATTACHMENT0
                    || draw_buffer >= GL_COLOR_ATTACHMENT0 + Vulkan::MAX_COLOR_ATTACHMENTS)
                    continue;

                GLuint color_attachment = vk_fbo->color_attachments[draw_buffer - GL_COLOR_ATTACHMENT0];
                if (color_attachment) {
                    Vulkan::gl_texture* vk_tex = Vulkan::gl_texture::get(color_attachment);
                    Vulkan::Image::PipelineBarrier(Vulkan::current_command_buffer,
                        vk_tex->image, Vulkan::get_aspect_mask(vk_tex->internal_format),
                        vk_tex->max_mipmap_level + 1, 1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
                }
            }

            if (vk_fbo->depth_attachment) {
                Vulkan::gl_texture* vk_tex = Vulkan::gl_texture::get(vk_fbo->depth_attachment);
                Vulkan::Image::PipelineBarrier(Vulkan::current_command_buffer,
                    vk_tex->image, Vulkan::get_aspect_mask(vk_tex->internal_format),
                    vk_tex->max_mipmap_level + 1, 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
            }

            vkCmdBeginRenderPass(Vulkan::current_command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
            Vulkan::current_framebuffer = vk_fbo->framebuffer;
            Vulkan::current_render_pass = *vk_fbo->render_pass.get();
        }
    }
    else {
        if (Vulkan::current_framebuffer != vulkan_swapchain_render_pass_info.framebuffer) {
            Vulkan::end_render_pass(Vulkan::current_command_buffer);

            vkCmdBeginRenderPass(Vulkan::current_command_buffer, &vulkan_swapchain_render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
            Vulkan::current_framebuffer = vulkan_swapchain_render_pass_info.framebuffer;
            Vulkan::current_render_pass = vulkan_swapchain_render_pass_info.renderPass;
        }
    }

    if (push_constant_stage_flags && push_constant_data_size)
        vkCmdPushConstants(Vulkan::current_command_buffer, pipeline_layout,
            push_constant_stage_flags, 0, push_constant_data_size, push_constant_data);

    std::sort(dynamic_infos, dynamic_info,
        [](const std::pair<uint32_t, uint32_t>& left,
            const std::pair<uint32_t, uint32_t>& right) { return left.first <= right.first; });

    uint32_t dynamic_offset_count = (uint32_t)(dynamic_info - dynamic_infos);
    for (uint32_t i = 0; i < dynamic_offset_count; i++)
        dynamic_offsets[i] = dynamic_infos[i].second;

    vkCmdBindDescriptorSets(Vulkan::current_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipeline_layout, 0, descriptor_set_collection->count, descriptor_set_collection->data,
        dynamic_offset_count, dynamic_offsets);

    free_def(descriptor_infos);

    {
        uint32_t binding_count = 0;
        for (Vulkan::gl_vertex_array_vertex_attrib& i : vk_vao->vertex_attribs)
            if (i.binding != -1)
                binding_count++;

        int32_t count = 0;
        VkBuffer buffers[Vulkan::MAX_VERTEX_ATTRIB_COUNT] = {};
        VkDeviceSize offsets[Vulkan::MAX_VERTEX_ATTRIB_COUNT] = {};

        for (Vulkan::gl_vertex_buffer_binding_data& i : vk_vao->vertex_buffer_bindings) {
            Vulkan::gl_vertex_buffer* vk_vb = Vulkan::gl_vertex_buffer::get(i.buffer);
            if (vk_vb) {
                buffers[count] = vk_vb->working_buffer;
                offsets[count] = vk_vb->working_buffer.GetOffset() + i.offset;
                count++;
            }
        }

        if (binding_count < Vulkan::MAX_VERTEX_ATTRIB_COUNT) {
            buffers[count] = Vulkan::gl_wrap_manager_get_dummy_vertex_buffer();
            offsets[count] = 0;
            count++;
        }

        if (count)
            vkCmdBindVertexBuffers(Vulkan::current_command_buffer, 0, count, buffers, offsets);
    }

    if (vk_vao->index_buffer_binding.buffer)
        switch (type) {
        case GL_UNSIGNED_SHORT: {
            Vulkan::gl_index_buffer* vk_ib = Vulkan::gl_index_buffer::get(vk_vao->index_buffer_binding.buffer);
            if (vk_ib) {
                VkBuffer buffer = vk_ib->working_buffer;
                VkDeviceSize offset = vk_ib->working_buffer.GetOffset() + (size_t)indices;
                vkCmdBindIndexBuffer(Vulkan::current_command_buffer, buffer, offset, VK_INDEX_TYPE_UINT16);
            }
        } break;
        case GL_UNSIGNED_INT: {
            Vulkan::gl_index_buffer* vk_ib = Vulkan::gl_index_buffer::get(vk_vao->index_buffer_binding.buffer);
            if (vk_ib) {
                VkBuffer buffer = vk_ib->working_buffer;
                VkDeviceSize offset = vk_ib->working_buffer.GetOffset() + (size_t)indices;
                vkCmdBindIndexBuffer(Vulkan::current_command_buffer, buffer, offset, VK_INDEX_TYPE_UINT32);
            }
        } break;
        }

    vkCmdBindPipeline(Vulkan::current_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    GLuint query = Vulkan::gl_wrap_manager_get_query_samples_passed();
    if (query)
        Vulkan::gl_query::get(query)->query.Begin(Vulkan::current_command_buffer);
    return true;
}
