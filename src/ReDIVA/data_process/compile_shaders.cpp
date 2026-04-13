/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "compile_shaders.hpp"
#include "../../KKdLib/io/file_stream.hpp"
#include "../../KKdLib/farc.hpp"
#include "../../KKdLib/hash.hpp"
#include "../../CRE/shader_dev.hpp"
#include "../../CRE/shader_ft.hpp"
#include <processthreadsapi.h>

struct program_spv {
    size_t size;
    size_t spv;
    uint64_t hash;
};

void compile_shaders(farc* f, farc* of, const shader_table* shaders_table, const size_t size, bool debug) {
    if (!f || !shaders_table || !size)
        return;

    wchar_t temp_path[MAX_PATH];
    if (FAILED(GetTempPathW(MAX_PATH, temp_path)))
        return;

    wchar_t vulkan_sdk_path[MAX_PATH];
    if (FAILED(GetEnvironmentVariableW(L"VULKAN_SDK", vulkan_sdk_path, MAX_PATH)))
        return;

    HANDLE std_output_handle = 0;
    HANDLE std_error_handle = 0;
#if DEBUG
    std_output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    std_error_handle = GetStdHandle(STD_ERROR_HANDLE);
#endif

    const char* options;
    if (debug)
        options = "-O -Werror -g";
    else
        options = "-O -Werror";

    char vert_buf[MAX_PATH];
    char frag_buf[MAX_PATH];
    char vert_file_buf[MAX_PATH];
    char frag_file_buf[MAX_PATH];
    wchar_t cmd_temp[0x800];
    char vert_bin_buf[MAX_PATH];
    char frag_bin_buf[MAX_PATH];
    wchar_t glsl_vert_file_buf[MAX_PATH];
    wchar_t glsl_frag_file_buf[MAX_PATH];
    wchar_t spv_vert_file_buf[MAX_PATH];
    wchar_t spv_frag_file_buf[MAX_PATH];

    GLsizei buffer_size = 0x20000;
    void* spv = force_malloc(buffer_size);
    std::string temp_vert;
    std::string temp_frag;
    std::vector<int32_t> vec_vert;
    std::vector<int32_t> vec_frag;
    std::vector<program_spv> program_data_vert_spv;
    std::vector<program_spv> program_data_frag_spv;
    for (size_t i = 0; i < size; i++) {
        const shader_table* shader_table = &shaders_table[i];

        int32_t num_sub = shader_table->num_sub;
        const shader_sub_table* sub_table = shaders_table[i].sub;
        vec_vert.resize(shader_table->num_uniform);
        vec_frag.resize(shader_table->num_uniform);
        int32_t* vec_vert_data = vec_vert.data();
        int32_t* vec_frag_data = vec_frag.data();
        for (int32_t j = 0; j < num_sub; j++, sub_table++) {
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
            for (int32_t k = 0; k < shader_table->num_uniform; k++) {
                if (sub_table->vp_unival_max[k] > 0)
                    uniform_vert_flags |= 1 << k;
                if (sub_table->fp_unival_max[k] > 0)
                    uniform_frag_flags |= 1 << k;
            }

            char uniform_vert_flags_buf[9];
            char uniform_frag_flags_buf[9];
            for (int32_t k = 0, l = 7; k < 32; k += 4, l--) {
                int32_t vert_flags_digit = (uniform_vert_flags >> k) & 0x0F;
                if (vert_flags_digit >= 0x00 && vert_flags_digit <= 0x09)
                    uniform_vert_flags_buf[l] = (char)('0' + vert_flags_digit);
                else
                    uniform_vert_flags_buf[l] = (char)('A' + (vert_flags_digit - 0x0A));

                int32_t frag_flags_digit = (uniform_frag_flags >> k) & 0x0F;
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

            vert_data = shader::parse_include(vert_data, f);
            frag_data = shader::parse_include(frag_data, f);

            strcpy_s(vert_bin_buf, sizeof(vert_bin_buf), sub_table->vp);
            strcat_s(vert_bin_buf, sizeof(vert_bin_buf), ".vert.bin");

            strcpy_s(frag_bin_buf, sizeof(frag_bin_buf), sub_table->fp);
            strcat_s(frag_bin_buf, sizeof(frag_bin_buf), ".frag.bin");

            bool has_vert_bin = of->has_file(vert_bin_buf);
            bool has_frag_bin = of->has_file(frag_bin_buf);

            if ((!has_vert_bin || !has_frag_bin) && shader_table->num_uniform > 0
                && (sub_table->vp_unival_max[0] != -1 || sub_table->fp_unival_max[0] != -1)) {
                int32_t num_uniform = shader_table->num_uniform;
                int32_t unival_vp_curr = 1;
                int32_t unival_vp_count = 1;
                int32_t unival_fp_curr = 1;
                int32_t unival_fp_count = 1;
                const int32_t* vp_unival_max = sub_table->vp_unival_max;
                const int32_t* fp_unival_max = sub_table->fp_unival_max;
                for (int32_t k = 0; k < num_uniform; k++) {
                    const int32_t unival_vp_max = vp_unival_max[k];
                    const int32_t unival_fp_max = fp_unival_max[k];
                    unival_vp_count += unival_vp_curr * unival_vp_max;
                    unival_fp_count += unival_fp_curr * unival_fp_max;
                    unival_vp_curr *= unival_vp_max + 1;
                    unival_fp_curr *= unival_fp_max + 1;
                }

                if (!has_vert_bin) {
                    program_data_vert_spv.reserve(unival_vp_count);

                    strcpy_s(vert_buf, sizeof(vert_buf), sub_table->vp);
                    size_t vert_buf_pos = utf8_length(vert_buf);
                    vert_buf[vert_buf_pos++] = '.';
                    vert_buf[vert_buf_pos] = 0;
                    memset(&vert_buf[vert_buf_pos], '0', num_uniform);
                    vert_buf[vert_buf_pos + num_uniform] = 0;
                    strcat_s(vert_buf, sizeof(vert_buf), ".vert.spv");
                    int32_t vert_buf_len = (int32_t)utf8_length(vert_buf);

                    for (int32_t k = 0; k < unival_vp_count; k++) {
                        for (int32_t l = 0, m = k; l < num_uniform; l++) {
                            int32_t unival_max = vp_unival_max[l] + 1;
                            vec_vert_data[l] = min_def(m % unival_max, vp_unival_max[l]);
                            m /= unival_max;
                            vert_buf[vert_buf_pos + l] = (char)('0' + vec_vert_data[l]);
                        }

                        shader::parse_define(vert_data,
                            num_uniform, vec_vert_data, temp_vert, true);

                        swprintf_s(glsl_vert_file_buf, sizeof(glsl_vert_file_buf)
                            / sizeof(wchar_t), L"%s%.*S", temp_path, vert_buf_len - 4, vert_buf);
                        swprintf_s(spv_vert_file_buf, sizeof(spv_vert_file_buf)
                            / sizeof(wchar_t), L"%s%S", temp_path, vert_buf);

                        file_stream fs;
                        fs.open(glsl_vert_file_buf, L"wb");
                        fs.write_string(temp_vert);
                        fs.close();

                        swprintf_s(cmd_temp, sizeof(cmd_temp) / sizeof(wchar_t),
                            L"%s\\Bin\\glslc.exe -DSPIRV %S --target-spv=spv1.0 -o \"%s\" \"%s\"",
                            vulkan_sdk_path, options, spv_vert_file_buf, glsl_vert_file_buf);

                        STARTUPINFO si;
                        PROCESS_INFORMATION pi;

                        ZeroMemory(&si, sizeof(si));
                        si.cb = sizeof(si);
                        si.dwFlags |= STARTF_USESTDHANDLES;
                        si.hStdError = std_error_handle;
                        si.hStdOutput = std_output_handle;
                        ZeroMemory(&pi, sizeof(pi));
                        CreateProcessW(0, cmd_temp, 0, 0, FALSE, CREATE_NO_WINDOW, 0, 0, &si, &pi);
                        WaitForSingleObject(pi.hProcess, INFINITE);
                        CloseHandle(pi.hProcess);
                        CloseHandle(pi.hThread);

                        fs.open(spv_vert_file_buf, L"rb");
                        if (fs.check_not_null()) {
                            size_t size = fs.get_length();
                            void* data = force_malloc(size);
                            fs.read(data, size);
                            fs.close();

                            program_data_vert_spv.push_back({ size, (size_t)data });
                        }
                        else {
                            printf("There was an error while compiling %.*s\n", vert_buf_len - 4, vert_buf);
                            program_data_vert_spv.push_back({ 0, 0 });
                        }

                        DeleteFileW(glsl_vert_file_buf);
                        DeleteFileW(spv_vert_file_buf);
                    }
                }

                if (!has_frag_bin) {
                    program_data_frag_spv.reserve(unival_fp_count);

                    strcpy_s(frag_buf, sizeof(frag_buf), sub_table->fp);
                    size_t frag_buf_pos = utf8_length(frag_buf);
                    frag_buf[frag_buf_pos++] = '.';
                    frag_buf[frag_buf_pos] = 0;
                    memset(&frag_buf[frag_buf_pos], '0', num_uniform);
                    frag_buf[frag_buf_pos + num_uniform] = 0;
                    strcat_s(frag_buf, sizeof(frag_buf), ".frag.spv");
                    int32_t frag_buf_len = (int32_t)utf8_length(frag_buf);

                    for (int32_t k = 0; k < unival_fp_count; k++) {
                        for (int32_t l = 0, m = k; l < num_uniform; l++) {
                            int32_t unival_max = fp_unival_max[l] + 1;
                            vec_frag_data[l] = (uint32_t)(min_def(m % unival_max, fp_unival_max[l]));
                            m /= unival_max;
                            frag_buf[frag_buf_pos + l] = (char)('0' + vec_frag_data[l]);
                        }

                        shader::parse_define(frag_data,
                            num_uniform, vec_frag_data, temp_frag, true);

                        swprintf_s(glsl_frag_file_buf, sizeof(glsl_frag_file_buf)
                            / sizeof(wchar_t), L"%s%.*S", temp_path, frag_buf_len - 4, frag_buf);
                        swprintf_s(spv_frag_file_buf, sizeof(spv_frag_file_buf)
                            / sizeof(wchar_t), L"%s%S", temp_path, frag_buf);

                        file_stream fs;
                        fs.open(glsl_frag_file_buf, L"wb");
                        fs.write_string(temp_frag);
                        fs.close();

                        swprintf_s(cmd_temp, sizeof(cmd_temp) / sizeof(wchar_t),
                            L"%s\\Bin\\glslc.exe -DSPIRV %S --target-spv=spv1.0 -o \"%s\" \"%s\"",
                            vulkan_sdk_path, options, spv_frag_file_buf, glsl_frag_file_buf);

                        STARTUPINFO si;
                        PROCESS_INFORMATION pi;

                        ZeroMemory(&si, sizeof(si));
                        si.cb = sizeof(si);
                        si.dwFlags |= STARTF_USESTDHANDLES;
                        si.hStdError = std_error_handle;
                        si.hStdOutput = std_output_handle;
                        ZeroMemory(&pi, sizeof(pi));
                        CreateProcessW(0, cmd_temp, 0, 0, FALSE, CREATE_NO_WINDOW, 0, 0, &si, &pi);
                        WaitForSingleObject(pi.hProcess, INFINITE);
                        CloseHandle(pi.hProcess);
                        CloseHandle(pi.hThread);

                        fs.open(spv_frag_file_buf, L"rb");
                        if (fs.check_not_null()) {
                            size_t size = fs.get_length();
                            void* data = force_malloc(size);
                            fs.read(data, size);
                            fs.close();

                            program_data_frag_spv.push_back({ size, (size_t)data });
                        }
                        else {
                            printf("There was an error while compiling %.*s\n", frag_buf_len - 4, frag_buf);
                            program_data_frag_spv.push_back({ 0, 0 });
                        }

                        DeleteFileW(glsl_frag_file_buf);
                        DeleteFileW(spv_frag_file_buf);
                    }
                }
            }
            else if (!has_vert_bin || !has_frag_bin) {
                if (!has_vert_bin) {
                    program_data_vert_spv.reserve(1);

                    strcpy_s(vert_buf, sizeof(vert_buf), sub_table->vp);
                    strcat_s(vert_buf, sizeof(vert_buf), "..vert.spv");
                    int32_t vert_buf_len = (int32_t)utf8_length(vert_buf);

                    shader::parse_define(vert_data, temp_vert, true);

                    swprintf_s(glsl_vert_file_buf, sizeof(glsl_vert_file_buf)
                        / sizeof(wchar_t), L"%s%.*S", temp_path, vert_buf_len - 4, vert_buf);
                    swprintf_s(spv_vert_file_buf, sizeof(spv_vert_file_buf)
                        / sizeof(wchar_t), L"%s%S", temp_path, vert_buf);

                    file_stream fs;
                    fs.open(glsl_vert_file_buf, L"wb");
                    fs.write_string(temp_vert);
                    fs.close();

                    swprintf_s(cmd_temp, sizeof(cmd_temp) / sizeof(wchar_t),
                        L"%s\\Bin\\glslc.exe -DSPIRV %S --target-spv=spv1.0 -o \"%s\" \"%s\"",
                        vulkan_sdk_path, options, spv_vert_file_buf, glsl_vert_file_buf);

                    STARTUPINFO si;
                    PROCESS_INFORMATION pi;

                    ZeroMemory(&si, sizeof(si));
                    si.cb = sizeof(si);
                    si.dwFlags |= STARTF_USESTDHANDLES;
                    si.hStdError = std_error_handle;
                    si.hStdOutput = std_output_handle;
                    ZeroMemory(&pi, sizeof(pi));
                    CreateProcessW(0, cmd_temp, 0, 0, FALSE, CREATE_NO_WINDOW, 0, 0, &si, &pi);
                    WaitForSingleObject(pi.hProcess, INFINITE);
                    CloseHandle(pi.hProcess);
                    CloseHandle(pi.hThread);

                    fs.open(spv_vert_file_buf, L"rb");
                    if (fs.check_not_null()) {
                        size_t size = fs.get_length();
                        void* data = force_malloc(size);
                        fs.read(data, size);
                        fs.close();

                        program_data_vert_spv.push_back({ size, (size_t)data });
                    }
                    else {
                        printf("There was an error while compiling %.*s\n", vert_buf_len - 4, vert_buf);
                        program_data_vert_spv.push_back({ 0, 0 });
                    }

                    DeleteFileW(glsl_vert_file_buf);
                    DeleteFileW(spv_vert_file_buf);
                }

                if (!has_frag_bin) {
                    program_data_frag_spv.reserve(1);

                    strcpy_s(frag_buf, sizeof(frag_buf), sub_table->fp);
                    strcat_s(frag_buf, sizeof(frag_buf), "..frag.spv");
                    int32_t frag_buf_len = (int32_t)utf8_length(frag_buf);

                    shader::parse_define(frag_data, temp_frag, true);

                    swprintf_s(glsl_frag_file_buf, sizeof(glsl_frag_file_buf)
                        / sizeof(wchar_t), L"%s%.*S", temp_path, frag_buf_len - 4, frag_buf);
                    swprintf_s(spv_frag_file_buf, sizeof(spv_frag_file_buf)
                        / sizeof(wchar_t), L"%s%S", temp_path, frag_buf);

                    file_stream fs;
                    fs.open(glsl_frag_file_buf, L"wb");
                    fs.write_string(temp_frag);
                    fs.close();

                    swprintf_s(cmd_temp, sizeof(cmd_temp) / sizeof(wchar_t),
                        L"%s\\Bin\\glslc.exe -DSPIRV %S --target-spv=spv1.0 -o \"%s\" \"%s\"",
                        vulkan_sdk_path, options, spv_frag_file_buf, glsl_frag_file_buf);

                    STARTUPINFO si;
                    PROCESS_INFORMATION pi;

                    ZeroMemory(&si, sizeof(si));
                    si.cb = sizeof(si);
                    si.dwFlags |= STARTF_USESTDHANDLES;
                    si.hStdError = std_error_handle;
                    si.hStdOutput = std_output_handle;
                    ZeroMemory(&pi, sizeof(pi));
                    CreateProcessW(0, cmd_temp, 0, 0, FALSE, CREATE_NO_WINDOW, 0, 0, &si, &pi);
                    WaitForSingleObject(pi.hProcess, INFINITE);
                    CloseHandle(pi.hProcess);
                    CloseHandle(pi.hThread);

                    fs.open(spv_frag_file_buf, L"rb");
                    if (fs.check_not_null()) {
                        size_t size = fs.get_length();
                        void* data = force_malloc(size);
                        fs.read(data, size);
                        fs.close();

                        program_data_frag_spv.push_back({ size, (size_t)data });
                    }
                    else {
                        printf("There was an error while compiling %.*s\n", frag_buf_len - 4, frag_buf);
                        program_data_frag_spv.push_back({ 0, 0 });
                    }

                    DeleteFileW(glsl_frag_file_buf);
                    DeleteFileW(spv_frag_file_buf);
                }
            }

            if (!has_vert_bin) {
                farc_file* shader_vert_file = of->add_file(vert_bin_buf);
                size_t vert_spv_count = program_data_vert_spv.size();
                size_t vert_spv_size = sizeof(uint64_t) + vert_spv_count * sizeof(program_spv);
                for (program_spv& k : program_data_vert_spv)
                    vert_spv_size += align_val(k.size, 0x04);
                shader_vert_file->data = force_malloc(vert_spv_size);
                shader_vert_file->size = vert_spv_size;
                shader_vert_file->compressed = true;
                shader_vert_file->data_changed = true;

                ((uint64_t*)shader_vert_file->data)[0] = vert_file_name_hash;
                program_spv* vert_spv = (program_spv*)&((uint64_t*)shader_vert_file->data)[1];
                size_t vert_spv_data_base = (size_t)shader_vert_file->data + sizeof(uint64_t);
                size_t vert_spv_data = vert_spv_data_base + vert_spv_count * sizeof(program_spv);
                for (program_spv& k : program_data_vert_spv) {
                    vert_spv->size = k.size;
                    vert_spv->spv = vert_spv_data - vert_spv_data_base;
                    vert_spv->hash = hash_xxh3_64bits((void*)k.spv, k.size);
                    memcpy((void*)vert_spv_data, (void*)k.spv, k.size);
                    vert_spv_data_base += sizeof(program_spv);
                    vert_spv_data += align_val(k.size, 0x04);
                    void* spv = (void*)k.spv;
                    free_def(spv);
                    k.spv = 0;
                    vert_spv++;
                }
            }

            if (!has_frag_bin) {
                farc_file* shader_frag_file = of->add_file(frag_bin_buf);
                size_t frag_spv_count = program_data_frag_spv.size();
                size_t frag_spv_size = sizeof(uint64_t) + frag_spv_count * sizeof(program_spv);
                for (program_spv& k : program_data_frag_spv)
                    frag_spv_size += align_val(k.size, 0x04);
                shader_frag_file->data = force_malloc(frag_spv_size);
                shader_frag_file->size = frag_spv_size;
                shader_frag_file->compressed = true;
                shader_frag_file->data_changed = true;

                ((uint64_t*)shader_frag_file->data)[0] = frag_file_name_hash;
                program_spv* frag_spv = (program_spv*)&((uint64_t*)shader_frag_file->data)[1];
                size_t frag_spv_data_base = (size_t)shader_frag_file->data + sizeof(uint64_t);
                size_t frag_spv_data = frag_spv_data_base + frag_spv_count * sizeof(program_spv);
                for (program_spv& k : program_data_frag_spv) {
                    frag_spv->size = k.size;
                    frag_spv->spv = frag_spv_data - frag_spv_data_base;
                    frag_spv->hash = hash_xxh3_64bits((void*)k.spv, k.size);
                    memcpy((void*)frag_spv_data, (void*)k.spv, k.size);
                    frag_spv_data_base += sizeof(program_spv);
                    frag_spv_data += align_val(k.size, 0x04);
                    void* spv = (void*)k.spv;
                    free_def(spv);
                    k.spv = 0;
                    frag_spv++;
                }
            }

            free_def(vert_data);
            free_def(frag_data);
            program_data_vert_spv.clear();
            program_data_frag_spv.clear();
        }
        vec_vert.clear();
        vec_frag.clear();
    }
    free_def(spv);
}

void compile_all_shaders(bool debug) {
    GLAD_GL_VERSION_4_1 = 1;
    GLAD_GL_VERSION_4_2 = 1;
    GLAD_GL_VERSION_4_3 = 1;
    GLAD_GL_VERSION_4_4 = 1;
    GLAD_GL_VERSION_4_5 = 1;
    GLAD_GL_VERSION_4_6 = 1;
    {
        farc f;
        f.read("rom\\ft_shaders.farc", true, false);

        farc of;
        compile_shaders(&f, &of, shader_ft_table, shader_ft_table_size, debug);
        of.write("rom\\ft_shaders_spirv.farc", FARC_FArC, FARC_NONE, false, false);
    }

    {
        farc f;
        f.read("rom\\dev_shaders.farc", true, false);

        farc of;
        compile_shaders(&f, &of, shader_dev_table, shader_dev_table_size, debug);
        of.write("rom\\dev_shaders_spirv.farc", FARC_FArC, FARC_NONE, false, false);
    }
    GLAD_GL_VERSION_4_1 = 0;
    GLAD_GL_VERSION_4_2 = 0;
    GLAD_GL_VERSION_4_3 = 0;
    GLAD_GL_VERSION_4_4 = 0;
    GLAD_GL_VERSION_4_5 = 0;
    GLAD_GL_VERSION_4_6 = 0;
}
