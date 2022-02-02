/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "../KKdLib/kkfs.h"
#include "../KKdLib/a3da.h"
#include "../KKdLib/f2/struct.h"
#include "../CRE/auth_3d.h"
#include "elf.h"
#include "main.h"
#include "shared.h"
#include "unedat.h"
#include <intrin.h>
#include <processthreadsapi.h>
#include <timeapi.h>

bool aes_ni;

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

static void kkfs_test() {
    uint32_t state = 105632;
    uint8_t data[0x2000];
    next_rand_uint8_t_pointer(data, 0x2000, &state);

    kkc* c = kkc_init();
    kkc_init_from_seed(c, state, KKC_KEY_128);
    kkc_prepare_cipher_table(c);
    kkfs* fs = kkfs_init();
    kkfs_winitialize(fs, L"test", 0x200, 0x4000000, 0, c, 0, 0);
    kkfs_wcreate_directory(fs, L"test_dir_super_duper_long_name1", 0);
    kkfs_wcreate_directory(fs, L"test_dir2", 0);
    kkfs_wenter_directory(fs, L"test_dir2");
    kkfs_wcreate_directory(fs, L"test_dir3", 0);
    kkfs_exit_directory(fs);
    kkfs_wenter_directory(fs, L"test_dir2");
    kkfs_wenter_directory(fs, L"test_dir3");
    kkfs_wcreate_directory(fs, L"test_dir4", 0);
    kkfs_wcreate_directory(fs, L"test_dir5", 0);
    kkfs_wcreate_directory(fs, L"test_dir6", 0);
    kkfs_wcreate_directory(fs, L"test_dir7", 0);
    kkfs_wcreate_directory(fs, L"test_dir8", 0);
    kkfs_wcreate_directory(fs, L"test_dir9", 0);
    kkfs_wcreate_file(fs, L"test_file1", 0x1000, KKFS_FILE_CIPHER);
    kkfs_wcreate_file(fs, L"test_file2", 0x1000, KKFS_FILE_CIPHER);
    kkfs_wwrite_file(fs, L"test_file1", data, 0x2000);
    kkfs_wwrite_file(fs, L"test_file2", data, 0x1000);
    kkfs_wdelete_directory(fs, L"test_dir4");
    kkfs_wdelete_file(fs, L"test_file");
    kkfs_exit_directory(fs);
    kkfs_exit_directory(fs);
    kkfs_close(fs);
    kkfs_dispose(fs);

    void* read_data;
    size_t read_size;
    fs = kkfs_init();
    kkfs_wopen(fs, L"test", c, 0, 0);
    kkfs_wenter_directory(fs, L"test_dir2");
    kkfs_wenter_directory(fs, L"test_dir3");
    kkfs_wread_file(fs, L"test_file1", &read_data, &read_size);
    kkfs_close(fs);
    kkfs_dispose(fs);
    free(read_data);
    kkc_dispose(c);
}

static void decrypt_edat() {
    uint8_t klic[16] = { 0xe2, 0x2d, 0x52, 0x46, 0x10, 0xa4, 0x4b, 0xb1,
                         0xb5, 0x00, 0xf0, 0x66, 0xe7, 0xa0, 0xed, 0x49 };

    stream f0;
    stream f1;
    io_open(&f0, "misc\\shader_ps3.edat", "rb");
    io_open(&f1, "misc\\shader_ps3.dat", "wb");
    if (f0.io.stream && f1.io.stream)
        DecryptEDAT(&f0, &f1, 8, "misc\\EP0177-NPEB02013_00-PJDF2MOMJTNSDAYO.rap", klic);
    io_free(&f0);
    io_free(&f1);
}

static void a3da_to_dft_dsc(char* a3da_path, int32_t pv_id) {
    a3da cam_a3da_file;
    a3da_init(&cam_a3da_file);
    a3da_load_file(&cam_a3da_file, "", a3da_path, 0);

    if (!cam_a3da_file.dof.has_dof) {
        a3da_free(&cam_a3da_file);
        return;
    }

    typedef struct dof_data {
        int32_t flags;
        float_t focus;
        float_t focus_range;
        float_t fuzzing_range;
        float_t ratio;
        float_t quality;
    } dof_data;

    auth_3d cam_a3da;
    auth_3d_init(&cam_a3da);
    auth_3d_load(&cam_a3da, &cam_a3da_file, 0, 0);
    a3da_free(&cam_a3da_file);

    int32_t frames = (int32_t)cam_a3da.play_control.size;

    dof_data* dof = force_malloc_s(dof_data, frames + 1ULL);
    int32_t* dof_index = force_malloc_s(int32_t, frames);
    size_t count = 0;

    *dof++ = (dof_data){ 0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

    auth_3d_camera_root* cr = &cam_a3da.camera_root.begin[0];

    for (int32_t i = 0; i < frames; i++) {
        auth_3d_time_step(&cam_a3da, (mat4*)&mat4_identity);
        bool enable = fabs(cam_a3da.dof.model_transform.rotation_value.z) > 0.000001f;
        if (!enable) {
            dof_index[i] = 0;
            continue;
        }

        float_t focus;
        vec3_distance(cam_a3da.dof.model_transform.translation_value, cr->view_point_value, focus);

        dof_data d;
        d.flags = 0x1F;
        d.focus = focus;
        d.focus_range = cam_a3da.dof.model_transform.scale_value.y;
        d.fuzzing_range = cam_a3da.dof.model_transform.rotation_value.y;
        d.ratio = cam_a3da.dof.model_transform.rotation_value.z;
        d.quality = 1.0f;

        bool found = false;
        for (size_t j = 0; j < count; j++)
            if (!memcmp(&d, &dof[j], sizeof(dof_data))) {
                dof_index[i] = (int32_t)j;
                found = true;
                break;
            }

        if (found)
            continue;

        dof[count] = d;
        dof_index[i] = (int32_t)count;
        count++;
    }

    dof--;

    auth_3d_free(&cam_a3da);

    char buf[0x100];
    sprintf_s(buf, sizeof(buf), "pv_%03d_dof.dsc", pv_id);

    stream s_dsc;
    io_open(&s_dsc, buf, "wb");
    io_write_uint32_t(&s_dsc, 0x15122517);
    int32_t last_dof_index = -1;
    for (int32_t i = 0; i < frames; i++) {
        if (last_dof_index == dof_index[i])
            continue;

        io_write_int32_t(&s_dsc, 0x01);
        io_write_int32_t(&s_dsc, (int32_t)((double_t)i * (100000.0 / 60.0))); // 00000
        io_write_int32_t(&s_dsc, 0x5F);
        io_write_int32_t(&s_dsc, dof_index[i] ? 0x01 : 0x00);
        io_write_int32_t(&s_dsc, dof_index[i]);
        io_write_int32_t(&s_dsc, 0x00);
        last_dof_index = dof_index[i];
    }
    io_free(&s_dsc);

    stream s_dft;
    io_mopen(&s_dft, 0, 0);
    uint32_t off;
    vector_enrs_entry e = vector_empty(enrs_entry);
    enrs_entry ee;
    vector_size_t pof = vector_empty(size_t);

    ee = (enrs_entry){ 0, 1, 8, 1, vector_empty(enrs_sub_entry) };
    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_DWORD });
    vector_enrs_entry_push_back(&e, &ee);
    off = 8;
    off = align_val(off, 0x10);

    ee = (enrs_entry){ off, 1, 24, (int32_t)count, vector_empty(enrs_sub_entry) };
    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 6, ENRS_DWORD });
    vector_enrs_entry_push_back(&e, &ee);
    off = (uint32_t)(count * 24ULL);
    off = align_val(off, 0x10);

    io_write_uint32_t(&s_dft, (int32_t)count);
    io_write_offset_f2_pof_add(&s_dft, 0x10, 0x40, &pof);
    io_align_write(&s_dft, 0x10);
    io_write(&s_dft, dof, sizeof(dof_data) * count);

    f2_struct st;
    memset(&st, 0, sizeof(f2_struct));

    io_align_write(&s_dft, 0x10);
    io_mcopy(&s_dft, &st.data, &st.length);
    io_free(&s_dft);

    st.enrs = e;
    st.pof = pof;

    st.header.signature = reverse_endianness_uint32_t('DOFT');
    st.header.length = 0x40;
    st.header.use_big_endian = false;
    st.header.use_section_size = true;
    st.header.inner_signature = 0x03;

    sprintf_s(buf, sizeof(buf), "pv%03d.dft", pv_id);
    f2_struct_write(&st, buf, true, false);
    f2_struct_free(&st);

    free(dof);
    free(dof_index);
}

bool close;

typedef struct thread {
    HANDLE handle;
    DWORD id;
} thread;

int32_t wmain(int32_t argc, wchar_t** argv) {
    //ShowWindow(GetConsoleWindow(), SW_HIDE);
    timeBeginPeriod(1);
    SetProcessDPIAware();

    int32_t cpuid_data[4];
    __cpuid(cpuid_data, 1);
    aes_ni = cpuid_data[2] & 0x2000000 ? true : false;

    //a3da_to_dft_dsc("CAMPV269_BASE.a3da", 269);

    render_init_struct ris;
    ris.res.x = 0;
    ris.res.y = 0;
    ris.scale = 0.0f;

    thread input, render, sound;
    HANDLE h[3];
    h[0] = input.handle = CreateThread(0, 0, &input_main, 0, 0, &input.id);
    h[1] = render.handle = CreateThread(0, 0, &render_main, &ris, 0, &render.id);
    h[2] = sound.handle = CreateThread(0, 0, &sound_main, 0, 0, &sound.id);

    if (input.handle)
        SetThreadDescription(input.handle, L"Input Thread");
    if (render.handle)
        SetThreadDescription(render.handle, L"Render Thread");
    if (sound.handle)
        SetThreadDescription(sound.handle, L"Sound Thread");

    lock_init(&state_lock);
    state = RENDER_UNINITIALIZED;
    if (input.handle && render.handle && sound.handle)
        WaitForMultipleObjects(3, h, true, INFINITE);
    lock_free(&state_lock);

    if (input.handle)
        CloseHandle(input.handle);
    if (render.handle)
        CloseHandle(render.handle);
    if (sound.handle)
        CloseHandle(sound.handle);
    timeEndPeriod(1);
    return 0;
}

int32_t wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
    _In_ PWSTR pCmdLine, _In_ int32_t nCmdShow) {
    return wmain(__argc, __wargv);
}
