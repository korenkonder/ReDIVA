/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "../KKdLib/database/sprite.hpp"
#include "../KKdLib/io/file_stream.hpp"
#include "../KKdLib/io/memory_stream.hpp"
#include "../KKdLib/f2/struct.hpp"
#include "../KKdLib/post_process_table/dof.hpp"
#include "../KKdLib/a3da.hpp"
#include "../KKdLib/aes.hpp"
#include "../KKdLib/deflate.hpp"
#include "../KKdLib/dsc.hpp"
#include "../KKdLib/interpolation.hpp"
#include "../KKdLib/pv_exp.hpp"
#include "../KKdLib/str_utils.hpp"
#include "../CRE/auth_3d.hpp"
#include "../CRE/shader_ft.hpp"
#include "main.hpp"
#include "shared.hpp"
#include <intrin.h>
#include <processthreadsapi.h>
#include <timeapi.h>

bool ssse3;
bool sse41;
bool sse42;
bool aes_ni;
bool avx;
bool f16c;
bool avx2;

inline static void next_rand_uint8_t_pointer(uint8_t* arr, size_t length, uint32_t* state) {
    if (!arr || length < 1)
        return;

    for (size_t i = 0; i < length; i++) {
        uint32_t x = *state;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        *state = x;
        arr[i] = (uint8_t)x;
    }
}

struct dft_dsc_data {
    int32_t frames;
    dof dof;
    std::vector<int32_t> dof_index;

    dft_dsc_data();
    ~dft_dsc_data();
};

dft_dsc_data::dft_dsc_data() : frames() {

}

dft_dsc_data::~dft_dsc_data() {

}

static bool a3da_to_dof_data(const char* a3da_path, dft_dsc_data& data) {
    a3da cam_a3da_file;
    a3da::load_file(&cam_a3da_file, "DOF\\auth_3d\\", a3da_path, 0);

    if (!cam_a3da_file.dof.has_dof) {
        data.frames = 0;
        return false;
    }

    auth_3d cam_a3da;
    cam_a3da.load(&cam_a3da_file, 0, 0);

    data.frames = (int32_t)cam_a3da.play_control.size;

    data.dof.data.reserve(data.frames + 1ULL);
    data.dof_index.reserve(data.frames);

    data.dof.data.push_back({ (dof_flags)0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f });

    auth_3d_camera_root* cr = &cam_a3da.camera_root[0];

    for (int32_t i = 0; i < data.frames; i++) {
        cam_a3da.dof.model_transform.interpolate((float_t)i);
        bool enable = fabsf(cam_a3da.dof.model_transform.rotation_value.z) > 0.000001f;
        if (!enable) {
            data.dof_index.push_back(0);
            continue;
        }

        float_t focus = vec3::distance(cam_a3da.dof.model_transform.translation_value, cr->view_point_value);

        dof_data d;
        d.flags = (dof_flags)(DOF_QUALITY | DOF_RATIO | DOF_FUZZING_RANGE | DOF_FOCUS_RANGE | DOF_FOCUS);
        d.focus = focus;
        d.focus_range = cam_a3da.dof.model_transform.scale_value.x;
        d.fuzzing_range = cam_a3da.dof.model_transform.rotation_value.x;
        d.ratio = cam_a3da.dof.model_transform.rotation_value.y;
        d.quality = 1.0f;

        bool found = false;
        for (dof_data& j : data.dof.data)
            if (!memcmp(&d, &j, sizeof(dof_data))) {
                data.dof_index.push_back((int32_t)(&j - data.dof.data.data()));
                found = true;
                break;
            }

        if (found)
            continue;

        data.dof_index.push_back((int32_t)data.dof.data.size());
        data.dof.data.push_back(d);
    }
    return true;
}

static void a3da_to_dft_dsc(int32_t pv_id) {
    char buf[0x200];

    sprintf_s(buf, sizeof(buf), "DOF\\script\\pv_%03d_hard.dsc", pv_id);
    file_stream s_src_dsc;
    s_src_dsc.open(buf, "rb");

    size_t src_dsc_length = s_src_dsc.get_length();
    void* src_dsc_data = force_malloc(src_dsc_length);
    s_src_dsc.read(src_dsc_data, src_dsc_length);
    s_src_dsc.close();

    dsc d_src;
    d_src.parse(src_dsc_data, src_dsc_length, DSC_FT);
    if (src_dsc_data)
        free(src_dsc_data);
    d_src.rebuild();

    dsc_get_func_id src_get_func_id = dsc_type_get_dsc_get_func_id(d_src.type);

    int32_t src_time_func_id = src_get_func_id("TIME");
    int32_t src_pv_branch_mode_func_id = src_get_func_id("PV_BRANCH_MODE");
    int32_t src_data_camera_func_id = src_get_func_id("DATA_CAMERA");

    int32_t time = -1;
    int32_t pv_branch_mode = -1;
    int32_t pv_branch_mode_2nd_camera_time = 9999999;
    for (dsc_data& i : d_src.data) {
        if (i.func == src_time_func_id) {
            time = ((int32_t*)d_src.get_func_data(&i))[0];
            continue;
        }
        else if (i.func == src_pv_branch_mode_func_id)
            pv_branch_mode = ((int32_t*)d_src.get_func_data(&i))[0];
        else if (i.func == src_data_camera_func_id) {
            int32_t camera = ((int32_t*)d_src.get_func_data(&i))[0];
            if (camera && pv_branch_mode == 2 && pv_branch_mode_2nd_camera_time == 9999999) {
                pv_branch_mode_2nd_camera_time = time;
                break;
            }
        }
    }

    sprintf_s(buf, sizeof(buf), "CAMPV%03d_BASE.a3da", pv_id);

    dft_dsc_data dft_data;
    bool base_cam = a3da_to_dof_data(buf, dft_data);

    size_t base_index_end = dft_data.dof_index.size();
    int32_t base_frames = dft_data.frames;

    sprintf_s(buf, sizeof(buf), "CAMPV%03d_PARTS02.a3da", pv_id);

    bool parts02_cam = a3da_to_dof_data(buf, dft_data);

    size_t parts02_index_end = dft_data.dof_index.size();
    int32_t parts02_frames = dft_data.frames;

    dsc d = dsc(DSC_FT, 0x15122517);

    dsc_get_func_id get_func_id = dsc_type_get_dsc_get_func_id(d.type);
    dsc_get_func_length get_func_length = d.get_dsc_get_func_length();
    dsc_get_func_name get_func_name = dsc_type_get_dsc_get_func_name(d.type);

    int32_t time_func_id = get_func_id("TIME");
    int32_t pv_branch_mode_func_id = get_func_id("PV_BRANCH_MODE");
    int32_t dof_func_id = get_func_id("DOF");

    const char* time_func_name = get_func_name(time_func_id);
    const char* pv_branch_mode_func_name = get_func_name(pv_branch_mode_func_id);
    const char* dof_func_name = get_func_name(dof_func_id);

    int32_t time_func_length = get_func_length(time_func_id);
    int32_t pv_branch_mode_func_length = get_func_length(pv_branch_mode_func_id);
    int32_t dof_func_length = get_func_length(dof_func_id);

    *d.add_func(pv_branch_mode_func_name,
        pv_branch_mode_func_id, pv_branch_mode_func_length) = 0;

    int32_t last_dof_index = -1;
    for (int32_t i = 0; i < base_frames; i++) {
        if (last_dof_index == dft_data.dof_index[i])
            continue;

        int32_t time = (int32_t)((double_t)i * (100000.0 / 60.0)); // 00000

        int32_t* time_func_data = (int32_t*)d.add_func(time_func_name,
            time_func_id, time_func_length);
        time_func_data[0] = time;

        if (time >= pv_branch_mode_2nd_camera_time) {
            int32_t* pv_branch_mode_data = (int32_t*)d.add_func(pv_branch_mode_func_name,
                pv_branch_mode_func_id, pv_branch_mode_func_length);
            pv_branch_mode_data[0] = 1;
        }

        int32_t* dof_func_data = (int32_t*)d.add_func(dof_func_name,
            dof_func_id, dof_func_length);
        dof_func_data[0] = dft_data.dof_index[i] ? 0x01 : 0x00;
        dof_func_data[1] = dft_data.dof_index[i];
        dof_func_data[2] = 0x00;

        last_dof_index = dft_data.dof_index[i];
    }

    last_dof_index = -1;
    for (int32_t i = 0; i < parts02_frames; i++) {
        if (last_dof_index == dft_data.dof_index[i + base_index_end])
            continue;

        int32_t time = (int32_t)((double_t)i * (100000.0 / 60.0)); // 00000
        time += pv_branch_mode_2nd_camera_time;

        if (time < pv_branch_mode_2nd_camera_time)
            continue;

        int32_t* time_func_data = (int32_t*)d.add_func(time_func_name,
            time_func_id, time_func_length);
        time_func_data[0] = time;

        int32_t* pv_branch_mode_data = (int32_t*)d.add_func(pv_branch_mode_func_name,
            pv_branch_mode_func_id, pv_branch_mode_func_length);
        pv_branch_mode_data[0] = 2;

        int32_t* dof_func_data = (int32_t*)d.add_func(dof_func_name,
            dof_func_id, dof_func_length);
        dof_func_data[0] = dft_data.dof_index[i + base_index_end] ? 0x01 : 0x00;
        dof_func_data[1] = dft_data.dof_index[i + base_index_end];
        dof_func_data[2] = 0x00;

        last_dof_index = dft_data.dof_index[i + base_index_end];
    }

    d.rebuild();

    void* dsc_data = 0;
    size_t dsc_length = 0;
    d.unparse(&dsc_data, &dsc_length);

    sprintf_s(buf, sizeof(buf), "DOF\\pv_script\\pv_%03d_dof.dsc", pv_id);
    file_stream s_dsc;
    s_dsc.open(buf, "wb");
    s_dsc.write(dsc_data, dsc_length);
    s_dsc.close();

    free_def(dsc_data);

    sprintf_s(buf, sizeof(buf), "DOF\\post_process_table\\pv%03d.dft", pv_id);
    dft_data.dof.write(buf);
}

/*#if defined(ReDIVA_DEV)
struct program_spv {
    size_t size;
    size_t spv;
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
        options = "-O -g";
    else
        options = "-O";

    GLsizei buffer_size = 0x20000;
    void* spv = force_malloc(buffer_size);
    size_t temp_vert_size = 0x10000;
    char* temp_vert = force_malloc<char>(temp_vert_size);
    size_t temp_frag_size = 0x10000;
    char* temp_frag = force_malloc<char>(temp_frag_size);
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
        for (size_t j = 0; j < num_sub; j++, sub_table++) {
            char vert_file_buf[MAX_PATH];
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

            char frag_file_buf[MAX_PATH];
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

            vert_data = shader::parse_include(vert_data, f);
            frag_data = shader::parse_include(frag_data, f);

            vert_data = shader::parse_include(vert_data, f);
            frag_data = shader::parse_include(frag_data, f);

            wchar_t cmd_temp[0x800];
            char vert_buf[MAX_PATH];
            char frag_buf[MAX_PATH];
            char vert_bin_buf[MAX_PATH];
            char frag_bin_buf[MAX_PATH];
            wchar_t glsl_vert_file_buf[MAX_PATH];
            wchar_t glsl_frag_file_buf[MAX_PATH];
            wchar_t spv_vert_file_buf[MAX_PATH];
            wchar_t spv_frag_file_buf[MAX_PATH];

            strcpy_s(vert_bin_buf, sizeof(vert_bin_buf), sub_table->vp);
            strcat_s(vert_bin_buf, sizeof(vert_bin_buf), ".vert.bin");

            strcpy_s(frag_bin_buf, sizeof(frag_bin_buf), sub_table->fp);
            strcat_s(frag_bin_buf, sizeof(frag_bin_buf), ".frag.bin");

            bool has_vert_bin = of->has_file(vert_bin_buf);
            bool has_frag_bin = of->has_file(frag_bin_buf);

            if ((!has_vert_bin || !has_frag_bin) && shader_table->num_uniform > 0
                && (sub_table->vp_unival_max[0] != -1 || sub_table->fp_unival_max[0] != -1)) {
                int32_t num_uniform = shader_table->num_uniform;
                size_t unival_vp_curr = 1;
                size_t unival_vp_count = 1;
                size_t unival_fp_curr = 1;
                size_t unival_fp_count = 1;
                const int32_t* vp_unival_max = sub_table->vp_unival_max;
                const int32_t* fp_unival_max = sub_table->fp_unival_max;
                for (size_t k = 0; k < num_uniform; k++) {
                    const size_t unival_vp_max = shader_table->use_permut[k] ? vp_unival_max[k] : 0;
                    const size_t unival_fp_max = shader_table->use_permut[k] ? fp_unival_max[k] : 0;
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

                    for (size_t k = 0; k < unival_vp_count; k++) {
                        for (size_t l = 0, m = k; l < num_uniform; l++) {
                            size_t unival_max = (size_t)(shader_table->use_permut[l] ? vp_unival_max[l] : 0) + 1;
                            vec_vert_data[l] = (uint32_t)(min_def(m % unival_max, vp_unival_max[l]));
                            m /= unival_max;
                            vert_buf[vert_buf_pos + l] = (char)('0' + vec_vert_data[l]);
                        }

                        shader::parse_define(vert_data, num_uniform,
                            vec_vert_data, &temp_vert, &temp_vert_size);

                        swprintf_s(glsl_vert_file_buf, sizeof(glsl_vert_file_buf)
                            / sizeof(wchar_t), L"%s%.*S", temp_path, vert_buf_len - 4, vert_buf);
                        swprintf_s(spv_vert_file_buf, sizeof(spv_vert_file_buf)
                            / sizeof(wchar_t), L"%s%S", temp_path, vert_buf);

                        file_stream fs;
                        fs.open(glsl_vert_file_buf, L"wb");
                        fs.write_utf8_string(temp_vert);
                        fs.close();

                        swprintf_s(cmd_temp, sizeof(cmd_temp) / sizeof(wchar_t),
                            L"%s\\Bin\\glslc.exe -DSPIRV %S --target-spv=spv1.0 -o \"%s\" \"%s\"",
                            vulkan_sdk_path, options, spv_vert_file_buf, glsl_vert_file_buf);

                        STARTUPINFO si;
                        PROCESS_INFORMATION pi;

                        ZeroMemory(&si, sizeof(si));
                        si.cb = sizeof(si);
                        si.dwFlags |= STARTF_USESTDHANDLES;
                        si.hStdError = std_output_handle;
                        si.hStdOutput = std_error_handle;
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
                            printf("There was an error while compiling %.*s\n", vert_buf_len - 4, vert_buf );
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

                    for (size_t k = 0; k < unival_fp_count; k++) {
                        for (size_t l = 0, m = k; l < num_uniform; l++) {
                            size_t unival_max = (size_t)(shader_table->use_permut[l] ? fp_unival_max[l] : 0) + 1;
                            vec_frag_data[l] = (uint32_t)(min_def(m % unival_max, fp_unival_max[l]));
                            m /= unival_max;
                            frag_buf[frag_buf_pos + l] = (char)('0' + vec_frag_data[l]);
                        }
                        shader::parse_define(frag_data, num_uniform,
                            vec_frag_data, &temp_frag, &temp_frag_size);

                        swprintf_s(glsl_frag_file_buf, sizeof(glsl_frag_file_buf)
                            / sizeof(wchar_t), L"%s%.*S", temp_path, frag_buf_len - 4, frag_buf);
                        swprintf_s(spv_frag_file_buf, sizeof(spv_frag_file_buf)
                            / sizeof(wchar_t), L"%s%S", temp_path, frag_buf);

                        file_stream fs;
                        fs.open(glsl_frag_file_buf, L"wb");
                        fs.write_utf8_string(temp_frag);
                        fs.close();

                        swprintf_s(cmd_temp, sizeof(cmd_temp) / sizeof(wchar_t),
                            L"%s\\Bin\\glslc.exe -DSPIRV %S --target-spv=spv1.0 -o \"%s\" \"%s\"",
                            vulkan_sdk_path, options, spv_frag_file_buf, glsl_frag_file_buf);

                        STARTUPINFO si;
                        PROCESS_INFORMATION pi;

                        ZeroMemory(&si, sizeof(si));
                        si.cb = sizeof(si);
                        si.dwFlags |= STARTF_USESTDHANDLES;
                        si.hStdError = std_output_handle;
                        si.hStdOutput = std_error_handle;
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

                    shader::parse_define(vert_data, &temp_vert, &temp_vert_size);

                    swprintf_s(glsl_vert_file_buf, sizeof(glsl_vert_file_buf)
                        / sizeof(wchar_t), L"%s%.*S", temp_path, vert_buf_len - 4, vert_buf);
                    swprintf_s(spv_vert_file_buf, sizeof(spv_vert_file_buf)
                        / sizeof(wchar_t), L"%s%S", temp_path, vert_buf);

                    file_stream fs;
                    fs.open(glsl_vert_file_buf, L"wb");
                    fs.write_utf8_string(temp_vert);
                    fs.close();

                    swprintf_s(cmd_temp, sizeof(cmd_temp) / sizeof(wchar_t),
                        L"%s\\Bin\\glslc.exe -DSPIRV %S --target-spv=spv1.0 -o \"%s\" \"%s\"",
                        vulkan_sdk_path, options, spv_vert_file_buf, glsl_vert_file_buf);

                    STARTUPINFO si;
                    PROCESS_INFORMATION pi;

                    ZeroMemory(&si, sizeof(si));
                    si.cb = sizeof(si);
                    si.dwFlags |= STARTF_USESTDHANDLES;
                    si.hStdError = std_output_handle;
                    si.hStdOutput = std_error_handle;
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

                    shader::parse_define(frag_data, &temp_frag, &temp_frag_size);

                    swprintf_s(glsl_frag_file_buf, sizeof(glsl_frag_file_buf)
                        / sizeof(wchar_t), L"%s%.*S", temp_path, frag_buf_len - 4, frag_buf);
                    swprintf_s(spv_frag_file_buf, sizeof(spv_frag_file_buf)
                        / sizeof(wchar_t), L"%s%S", temp_path, frag_buf);

                    file_stream fs;
                    fs.open(glsl_frag_file_buf, L"wb");
                    fs.write_utf8_string(temp_frag);
                    fs.close();

                    swprintf_s(cmd_temp, sizeof(cmd_temp) / sizeof(wchar_t),
                        L"%s\\Bin\\glslc.exe -DSPIRV %S --target-spv=spv1.0 -o \"%s\" \"%s\"",
                        vulkan_sdk_path, options, spv_frag_file_buf, glsl_frag_file_buf);

                    STARTUPINFO si;
                    PROCESS_INFORMATION pi;

                    ZeroMemory(&si, sizeof(si));
                    si.cb = sizeof(si);
                    si.dwFlags |= STARTF_USESTDHANDLES;
                    si.hStdError = std_output_handle;
                    si.hStdOutput = std_error_handle;
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

                        program_data_frag_spv.push_back({ size,(size_t)data });
                    }
                    else {
                        printf("There was an error while compiling %.*s\n", frag_buf_len - 4, vert_buf);
                        program_data_frag_spv.push_back({ 0, 0 });
                    }

                    DeleteFileW(glsl_frag_file_buf);
                    DeleteFileW(spv_frag_file_buf);
                }
            }

            if (!has_vert_bin) {
                farc_file* shader_vert_file = of->add_file(vert_bin_buf);
                size_t vert_spv_count = program_data_vert_spv.size();
                size_t vert_spv_size = vert_spv_count * sizeof(program_spv);
                for (program_spv& k : program_data_vert_spv)
                    vert_spv_size += align_val(k.size, 0x10);
                shader_vert_file->data = force_malloc(vert_spv_size);
                shader_vert_file->size = vert_spv_size;
                shader_vert_file->compressed = true;
                shader_vert_file->data_changed = true;

                program_spv* vert_spv = (program_spv*)shader_vert_file->data;
                size_t vert_spv_data_base = (size_t)shader_vert_file->data;
                size_t vert_spv_data_offset = vert_spv_count * sizeof(program_spv);
                for (program_spv& k : program_data_vert_spv) {
                    vert_spv->size = k.size;
                    vert_spv->spv = vert_spv_data_offset;
                    memcpy((void*)(vert_spv_data_base + vert_spv_data_offset), (void*)k.spv, k.size);
                    vert_spv_data_offset += align_val(k.size, 0x10);
                    void* spv_data = (void*)k.spv;
                    free_def(spv_data);
                    k.spv = 0;
                    vert_spv++;
                }
            }

            if (!has_frag_bin) {
                farc_file* shader_frag_file = of->add_file(frag_bin_buf);
                size_t frag_spv_count = program_data_frag_spv.size();
                size_t frag_spv_size = frag_spv_count * sizeof(program_spv);
                for (program_spv& k : program_data_frag_spv)
                    frag_spv_size += align_val(k.size, 0x10);
                shader_frag_file->data = force_malloc(frag_spv_size);
                shader_frag_file->size = frag_spv_size;
                shader_frag_file->compressed = true;
                shader_frag_file->data_changed = true;

                program_spv* frag_spv = (program_spv*)shader_frag_file->data;
                size_t frag_spv_data_base = (size_t)shader_frag_file->data;
                size_t frag_spv_data_offset = frag_spv_count * sizeof(program_spv);
                for (program_spv& k : program_data_frag_spv) {
                    frag_spv->size = k.size;
                    frag_spv->spv = frag_spv_data_offset;
                    memcpy((void*)(frag_spv_data_base + frag_spv_data_offset), (void*)k.spv, k.size);
                    frag_spv_data_offset += align_val(k.size, 0x10);
                    void* spv_data = (void*)k.spv;
                    free_def(spv_data);
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
    free_def(temp_vert);
    free_def(temp_frag);
}

void compile_all_shaders(bool debug) {
    farc f;
    f.read("rom\\ft_shaders.farc", true, false);

    farc of;
    compile_shaders(&f, &of, shader_ft_table, shader_ft_table_size, debug);
    of.write("rom\\ft_shaders_spirv", FARC_FArC, FARC_NONE, false);
}
#endif*/

/*
void decrypt_x_save_data() {
    uint8_t key[] = {
        0xA4, 0xB7, 0x31, 0xD0, 0x33, 0xFB, 0x4A, 0x63, 0x9D, 0xD2, 0x46, 0xA5, 0x05, 0xCD, 0x4B, 0xE5,
        0xF2, 0x10, 0xEE, 0x05, 0xC3, 0x56, 0x45, 0x3A, 0xAF, 0x22, 0x5C, 0x88, 0xA0, 0x9F, 0xB6, 0x8A,
    };

    aes256_ctx aes;
    aes256_init_ctx(&aes, key);

    file_stream ifs;
    ifs.open("SECURE.BIN", "rb");

    std::vector<uint8_t> data;
    data.resize(ifs.get_length());

    ifs.read(data.data(), data.size());
    ifs.close();

    struct savedata_header {
        uint32_t signature;
        uint32_t data_size;
        uint32_t length;
        uint32_t flags;
        uint8_t unk10;
        uint8_t crc8;
        uint16_t crc16;
        uint32_t section_size;
        uint32_t unk18;
        uint32_t unk1C;
        uint8_t data[];
    };

    savedata_header* head = (savedata_header*)data.data();

    uint8_t* a1 = data.data();
    uint8_t* v5 = a1 + head->length;
    uint8_t* v8 = a1;
    uint8_t* v9 = v5 + head->section_size;

    uint8_t v10 = 0xA5;
    while (v8 != a1 + 8)
        v10 ^= *v8++;

    if (!v10 || v10 == 0xFF)
        v10 = 1;

    while (v5 != v9) {
        uint8_t v12 = *v5;
        *v5 ^= v10;
        v10 = v12;
        v5++;
    }

    aes256_ecb_decrypt_buffer(&aes, data.data() + head->length, data.size() - head->length);

    file_stream ofs;
    ofs.open("SECURE.BIN_DEC", "wb");
    ofs.write(data.data(), data.size());
    ofs.close();

    void* dst = 0;
    size_t dst_len = head->unk1C;
    deflate::decompress(data.data() + head->length, data.size() - head->length,
        &dst, &dst_len, deflate::MODE_GZIP);

    file_stream ufs;
    ufs.open("SECURE.BIN_DEC_DEFLATE", "wb");
    ufs.write(dst, dst_len);
    ufs.close();

    if (dst)
        free(dst);
}*/

/*void process_edit_dsc(const char* path, const char* file,
    int32_t chara_id, dsc& d, pv_exp& exp, prj::stack_allocator* exp_alloc) {
    char buf[0x200];
    strcpy_s(buf, sizeof(buf), path);
    strcat_s(buf, sizeof(buf), file);
    strcat_s(buf, sizeof(buf), ".dsc");

    file_stream s_src_dsc;
    s_src_dsc.open(buf, "rb");

    size_t src_dsc_length = s_src_dsc.get_length();
    void* src_dsc_data = force_malloc(src_dsc_length);
    s_src_dsc.read(src_dsc_data, src_dsc_length);
    s_src_dsc.close();

    dsc d_src;
    d_src.parse(src_dsc_data, src_dsc_length, DSC_FT);
    if (src_dsc_data)
        free(src_dsc_data);
    d_src.rebuild();

    dsc_get_func_id src_get_func_id = dsc_type_get_dsc_get_func_id(d_src.type);

    int32_t src_time_func_id = src_get_func_id("TIME");
    int32_t src_set_chara_func_id = src_get_func_id("SET_CHARA");
    int32_t src_pv_branch_mode_func_id = src_get_func_id("PV_BRANCH_MODE");
    int32_t src_edit_eye_anim_func_id = src_get_func_id("EDIT_EYE_ANIM");
    int32_t src_edit_expression_func_id = src_get_func_id("EDIT_EXPRESSION");
    int32_t src_edit_mouth_anim_func_id = src_get_func_id("EDIT_MOUTH_ANIM");
    int32_t src_edit_eyelid_anim_func_id = src_get_func_id("EDIT_EYELID_ANIM");

    dsc_get_func_id get_func_id = dsc_type_get_dsc_get_func_id(d.type);
    dsc_get_func_length get_func_length = d.get_dsc_get_func_length();
    dsc_get_func_name get_func_name = dsc_type_get_dsc_get_func_name(d.type);

    int32_t time_func_id = get_func_id("TIME");
    int32_t mouth_anim_func_id = get_func_id("MOUTH_ANIM");
    int32_t pv_branch_mode_func_id = get_func_id("PV_BRANCH_MODE");

    const char* time_func_name = get_func_name(time_func_id);
    const char* mouth_anim_func_name = get_func_name(mouth_anim_func_id);
    const char* pv_branch_mode_func_name = get_func_name(pv_branch_mode_func_id);

    int32_t time_func_length = get_func_length(time_func_id);
    int32_t mouth_anim_func_length = get_func_length(mouth_anim_func_id);
    int32_t pv_branch_mode_func_length = get_func_length(pv_branch_mode_func_id);

    std::vector<pv_exp_data> face_data;
    std::vector<pv_exp_data> face_cl_data;

    int32_t time = -1;
    int32_t pv_branch_mode = -1;
    int32_t mouth_anim_time = -1;
    int32_t mouth_anim_pv_branch_mode = -1;
    int32_t set_chara = 0;
    for (::dsc_data& i : d_src.data) {
        if (i.func == src_time_func_id)
            time = ((int32_t*)d_src.get_func_data(&i))[0];
        else if (i.func == src_set_chara_func_id)
            set_chara = ((int32_t*)d_src.get_func_data(&i))[0];
        else if (i.func == src_pv_branch_mode_func_id)
            pv_branch_mode = ((int32_t*)d_src.get_func_data(&i))[0];
        else if (set_chara != chara_id)
            continue;
        else if (i.func == src_edit_expression_func_id) {
            uint32_t* data = d_src.get_func_data(&i);

            float_t frame = (float_t)(int32_t)((int64_t)time * 60 / 100000);
            int32_t expression_id = (int32_t)data[0];
            float_t blend_duration = (float_t)(int32_t)data[1] * 0.001f * 60.0f;

            pv_exp_data d;
            d.frame = frame;
            d.type = 0;
            d.id = expression_id;
            d.value = 1.0f;
            d.trans = blend_duration;
            face_data.push_back(d);
        }
        else if (i.func == src_edit_mouth_anim_func_id) {
            if (mouth_anim_pv_branch_mode != pv_branch_mode) {
                mouth_anim_pv_branch_mode = pv_branch_mode;

                int32_t* pv_branch_mode_func_data = (int32_t*)d.add_func(
                    pv_branch_mode_func_name, pv_branch_mode_func_id, pv_branch_mode_func_length);
                pv_branch_mode_func_data[0] = pv_branch_mode;
            }

            if (mouth_anim_time != time) {
                mouth_anim_time = time;

                int32_t* time_func_data = (int32_t*)d.add_func(
                    time_func_name, time_func_id, time_func_length);
                time_func_data[0] = time;
            }

            uint32_t* data = d_src.get_func_data(&i);

            int32_t* mouth_anim_func_data = (int32_t*)d.add_func(
                time_func_name, mouth_anim_func_id, mouth_anim_func_length);
            mouth_anim_func_data[0] = chara_id;
            mouth_anim_func_data[1] = 0;
            mouth_anim_func_data[2] = (int32_t)data[0];
            mouth_anim_func_data[3] = (int32_t)data[1];
            mouth_anim_func_data[4] = 1000;
        }
        else if (i.func == src_edit_eyelid_anim_func_id) {
            uint32_t* data = d_src.get_func_data(&i);

            float_t frame = (float_t)(int32_t)((int64_t)time * 60 / 100000);
            int32_t v342 = (int32_t)data[0];
            float_t blend_duration = (float_t)(int32_t)data[1] * 0.001f * 60.0f;
            float_t value = (float_t)(int32_t)data[2] * 0.001f;

            int32_t v346;
            switch (v342) {
            case 0:
                v346 = 2;
                break;
            case 1:
            default:
                v346 = 2;
                break;
            case 2:
                v346 = 1;
                break;
            case 3:
                v346 = 3;
                break;
            }

            pv_exp_data d;
            d.frame = frame;
            d.type = 1;
            d.id = v346;
            d.value = value;
            d.trans = blend_duration;
            face_cl_data.push_back(d);
        }
        else
            printf("Unsupported: %s\n", i.name);
    }

    face_data.push_back(pv_exp_data_null);
    face_cl_data.push_back(pv_exp_data_null);

    sprintf_s(buf, sizeof(buf), "PV3400_SMS_P%d_00", chara_id + 1);

    pv_exp_mot* exp_mot = &exp.motion_data[chara_id];
    exp_mot->face_data = exp_alloc->allocate<pv_exp_data>(face_data.data(), face_data.size());
    exp_mot->face_cl_data = exp_alloc->allocate<pv_exp_data>(face_cl_data.data(), face_cl_data.size());
    exp_mot->name = exp_alloc->allocate<char>(buf, utf8_length(buf) + 1);
}

void process_edit_dsc() {
    const char* path = "misc\\zamza\\";
    const int32_t chara_count = 5;

    char buf[0x200];

    strcpy_s(buf, sizeof(buf), path);
    strcat_s(buf, sizeof(buf), "pv_3400_base.dsc");

    file_stream s_base_dsc;
    s_base_dsc.open(buf, "rb");

    size_t src_dsc_length = s_base_dsc.get_length();
    void* src_dsc_data = force_malloc(src_dsc_length);
    s_base_dsc.read(src_dsc_data, src_dsc_length);
    s_base_dsc.close();

    dsc d_base;
    d_base.parse(src_dsc_data, src_dsc_length, DSC_FT);
    if (src_dsc_data)
        free(src_dsc_data);
    d_base.rebuild();

    dsc d = dsc(DSC_FT, 0x15122517);

    d.merge(1, d_base);

    d.rebuild();

    prj::stack_allocator* exp_alloc= new prj::stack_allocator;

    pv_exp exp;
    exp.motion_data = exp_alloc->allocate<pv_exp_mot>(chara_count);
    exp.motion_num = chara_count;

    for (int32_t i = 0; i < chara_count; i++) {
        sprintf_s(buf, sizeof(buf), "pv_3400_p%d", i + 1);
        process_edit_dsc(path, buf, i, d, exp, exp_alloc);
    }

    exp.ready = true;

    void* exp_data = 0;
    size_t exp_length = 0;
    exp.pack_file(&exp_data, &exp_length);

    delete exp_alloc;

    strcpy_s(buf, sizeof(buf), path);
    strcat_s(buf, sizeof(buf), "exp_PV3400.bin");

    file_stream s_exp;
    s_exp.open(buf, "wb");
    s_exp.write(exp_data, exp_length);
    s_exp.close();

    d.rebuild();

    void* dsc_data = 0;
    size_t dsc_length = 0;
    d.unparse(&dsc_data, &dsc_length);

    strcpy_s(buf, sizeof(buf), path);
    strcat_s(buf, sizeof(buf), "pv_3400.dsc");

    file_stream s_dsc;
    s_dsc.open(buf, "wb");
    s_dsc.write(dsc_data, dsc_length);
    s_dsc.close();

    if (dsc_data)
        free(dsc_data);
}*/

/*void scan_arguments(int32_t argc, wchar_t** argv) {
    std::vector<std::wstring> arguments;
    arguments.assign(argv + 1, argv + argc);

    bool show_help = false;
    auto i_begin = arguments.begin();
    auto i_end = arguments.end();
    for (auto i = i_begin; i != i_end; i++) {
        if (i->find(L"-", 0, 1))
            continue;

        if (!i->compare(L"--help"))
            show_help = true;
        else if (!i->compare(L"-debug"))
            stru_140EDA5B0.debug = 1;
        else if (!i->compare(L"-t"))
            test_mode_set(1);
        else  if (!i->compare(L"-m")) {
            if (++i == i_end)
                break;

            stru_140EDA5B0.mode = _wtoi(i->c_str());
        }
        else if (!i->compare(L"-s")) {
            if (++i == i_end)
                break;

            stru_140EDA5B0.sub_mode = _wtoi(i->c_str());
        }
        else if (!i->compare(L"-sm") || !i->compare(L"--start-up-mode")) {
            if (++i == i_end)
                break;

            if (i->compare(L"server"))
                stru_140EDA5B0.field_1C = 0;
            else
                stru_140EDA5B0.field_1C = 1;
        }
        else if (!i->compare(L"-ve"))
            stru_140EDA5B0.vsync_emulation = 1;
        else if (!i->compare(L"-fs") || !i->compare(L"-f"))
            stru_140EDA5B0.full_screen = 1;
        else if (!i->compare(L"-w"))
            stru_140EDA5B0.full_screen = 0;
        else if (!i->compare(L"-aa"))
            stru_140EDA5B0.ssaa = 1;
        else if (!i->compare(L"-qvga")) {
            stru_140EDA5B0.res_launch = RESOLUTION_MODE_QVGA;
            stru_140EDA5B0.hd_res = 0;
        }
        else if (!i->compare(L"-vga")) {
            stru_140EDA5B0.res_launch = RESOLUTION_MODE_VGA;
            stru_140EDA5B0.hd_res = 0;
        }
        else if (!i->compare(L"-wvga")) {
            stru_140EDA5B0.res_launch = RESOLUTION_MODE_WVGA;
            stru_140EDA5B0.hd_res = 0;
        }
        else if (!i->compare(L"-svga")) {
            stru_140EDA5B0.res_launch = RESOLUTION_MODE_SVGA;
            stru_140EDA5B0.hd_res = 0;
        }
        else if (!i->compare(L"-xga")) {
            stru_140EDA5B0.res_launch = RESOLUTION_MODE_XGA;
            stru_140EDA5B0.hd_res = 0;
        }
        else if (!i->compare(L"-wxga")) {
            stru_140EDA5B0.res_launch = RESOLUTION_MODE_WXGA;
            stru_140EDA5B0.hd_res = 1;
        }
        else if (!i->compare(L"-wxga_dbd")) {
            stru_140EDA5B0.res_launch = RESOLUTION_MODE_WXGA;
            stru_140EDA5B0.hd_res = 0;
        }
        else if (!i->compare(L"-uxga")) {
            stru_140EDA5B0.res_launch = RESOLUTION_MODE_UXGA;
            stru_140EDA5B0.hd_res = 0;
        }
        else if (!i->compare(L"-wuxga")) {
            stru_140EDA5B0.res_launch = RESOLUTION_MODE_WUXGA;
            stru_140EDA5B0.hd_res = 0;
        }
        else if (!i->compare(L"-wqxga")) {
            stru_140EDA5B0.res_launch = RESOLUTION_MODE_WQXGA;
            stru_140EDA5B0.hd_res = 0;
        }
        else if (!i->compare(L"-hdtv720")) {
            stru_140EDA5B0.res_launch = RESOLUTION_MODE_HD;
            stru_140EDA5B0.hd_res = 1;
        }
        else if (!i->compare(L"-hdtv720_dbd")) {
            stru_140EDA5B0.res_launch = RESOLUTION_MODE_HD;
            stru_140EDA5B0.hd_res = 0;
        }
        else if (!i->compare(L"-hdtv1080")) {
            stru_140EDA5B0.res_launch = RESOLUTION_MODE_FHD;
            stru_140EDA5B0.hd_res = 0;
        }
        else if (!i->compare(L"-wqhd")) {
            stru_140EDA5B0.res_launch = RESOLUTION_MODE_QHD;
            stru_140EDA5B0.hd_res = 0;
        }
        else if (!i->compare(L"-ss")) {
            if (++i == i_end)
                break;

            stru_140EDA5B0.screen_shot = 1;
            stru_140EDA5B0.field_80 = 0;
            stru_140EDA5B0.field_40.assign(*i);
            stru_140EDA5B0.field_60.assign(stru_140EDA5B0.field_40);
            stru_140EDA5B0.ssaa = 1;
        }
        else if (!i->compare(L"-no_aa")) {
            stru_140EDA5B0.ssaa = 0;
            continue;
        }
        else if (!i->compare(L"-ssformat")) {
            if (++i == i_end)
                break;

            if (!i->compare(L"ppm"))
                stru_140EDA5B0.ss_format = 0;
            else if (!i->compare(L"bmp"))
                stru_140EDA5B0.ss_format = 1;
            else if (!i->compare(L"tga"))
                stru_140EDA5B0.ss_format = 2;
            else if (!i->compare(L"yuy2"))
                stru_140EDA5B0.ss_format = 3;
            else if (!i->compare(L"jpg"))
                stru_140EDA5B0.ss_format = 4;
        }
        else if (!i->compare(L"-ss4x")) {
            if (++i == i_end)
                break;

            stru_140EDA5B0.screen_shot = 1;
            stru_140EDA5B0.field_80 = 0;
            stru_140EDA5B0.field_40.assign(*i);
            stru_140EDA5B0.field_60.assign(stru_140EDA5B0.field_40);
            stru_140EDA5B0.screen_shot_4x = 1;
            stru_140EDA5B0.ssaa = 1;
        }
        else if (!i->compare(L"-ss36x"));
        else if (i->compare(L"-ssalphamask"))
            stru_140EDA5B0.ss_alpha_mask = 1;
        else if (!i->compare(L"-fix_pv_timer"))
            stru_140EDA5B0.fix_pv_timer = 1;
        else if (!i->compare(L"-ps3"))
            stru_140EDA5B0.ps3 = 1;
        else if (!i->compare(L"-dev_rom"))
            stru_140EDA5B0.dev_rom = 1;
        else if (!i->compare(L"-pv_full_open"))
            stru_140EDA5B0.pv_full_open = 1;
        else if (!i->compare(L"-no_local_rom"))
            stru_140EDA5B0.local_rom = 0;
        else if (!i->compare(L"-pcv"))
            stru_140EDA5B0.pcv = 1;
    }

    sub_140558EB0(screen_shot_impl_ptr, stru_140EDA5B0.ss_format);
    if (stru_140EDA5B0.ssaa || stru_140EDA5B0.screen_shot)
        stru_140EDA5B0.hd_res = 0;

    if (show_help) {
        printf("Usage: %ls [option]...\n", *argv);
        printf("\t--help     Disp Usage\n");
        printf("\t-debug     Debug Mode(use gdb)\n");
        printf("\t-t         Test Mode Start\n");
        printf("\t-m <no>    Mode No\n");
        printf("\t-s <no>    Sub Mode No\n");
        printf("\t-sm <server|satellite>    Sub Mode No\n");
        printf("\t-ss <base> Screen Shot\n");
        printf("\t-ss4x <base> Screen Shot (4x screen size)\n");
        printf("\t-ssformat <ppm|bmp|tga|yuy2|jpg> Screen Shot file format\n");
        printf("\t-ssalphamask Enable Chara Alpha Mask\n");
        printf("\t-ve <0|1>  Use Vsync Emulation\n");
        printf("\t-fs        Full Screen Mode\n");
        printf("\t-f         Full Screen Mode\n");
        printf("\t-w         Window Mode\n");
        printf("\t-ps3       Use rom_ps3 Data\n");
        printf("\t-aa        Enable Anti-Alias\n");
        printf("\t-vga       VGA(640x480) Mode\n");
        printf("\t-xga       XGA(1024x768) Mode\n");
        printf("\t-wxga      WXGA(1280x768) Mode\n");
        printf("\t-wxga_dbd  WXGA(1280x768) dot by dot Mode\n");
        printf("\t-wxga2     WXGA(1360x768) Mode\n");
        exit(0);
    }
}*/

int32_t wmain(int32_t argc, wchar_t** argv) {
/*#if defined(ReDIVA_DEV)
    if (argc >= 2 && !wcscmp(argv[1], L"--compile-spir-v")) {
        compile_all_shaders(argc >= 3 && !wcscmp(argv[2], L"-d"));
        return 0;
    }
#endif*/

    /*process_edit_dsc();
    return 0;*/

    //ShowWindow(GetConsoleWindow(), SW_HIDE);
    timeBeginPeriod(1);
    SetProcessDPIAware();

    int32_t cpuid_data[4] = {};
    __cpuid(cpuid_data, 1);
    ssse3 = !!(cpuid_data[2] & (1 << 9));
    sse41 = !!(cpuid_data[2] & (1 << 19));
    sse42 = !!(cpuid_data[2] & (1 << 20));
    aes_ni = !!(cpuid_data[2] & (1 << 25));
    avx = !!(cpuid_data[2] & (1 << 28));
    f16c = !!(cpuid_data[2] & (1 << 29));

    int32_t cpuidex_data[4] = {};
    __cpuidex(cpuidex_data, 7, 0);
    avx2 = !!(cpuidex_data[1] & (1 << 5));

    //a3da_to_dft_dsc(269);

    render_init_struct ris;

    render_main(&ris);

    timeEndPeriod(1);
    return 0;
}

int32_t wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
    _In_ PWSTR pCmdLine, _In_ int32_t nCmdShow) {
    return wmain(__argc, __wargv);
}
