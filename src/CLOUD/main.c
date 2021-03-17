/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "../KKdLib/kkfs.h"
#include "../KKdLib/mat.h"
#include "../KKdLib/vec.h"
#include "elf.h"
#include "main.h"
#include "unedat.h"
#include <processthreadsapi.h>

FORCE_INLINE void next_rand_uint8_t_pointer(uint8_t* arr, size_t length, uint32_t* state) {
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
    char data[0x2000];
    next_rand_uint8_t_pointer(data, 0x2000, &state);

    kkc* c = kkc_init();
    kkc_init_from_seed_type_mode(c, state, KKC_KEY_TYPE_128, KKC_KEY_MODE_PAST);
    kkc_prepare_cursing_table(c);
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
    kkfs_wcreate_file(fs, L"test_file1", 0x1000, KKFS_FILE_CURSED);
    kkfs_wcreate_file(fs, L"test_file2", 0x1000, KKFS_FILE_CURSED);
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

    stream* f0 = io_open("shader_ps3.edat", "rb");
    stream* f1 = io_open("shader_ps3.dat", "wb");
    if (f0 && f1) {
        DecryptEDAT(f0, f1, 8, "EP0177-NPEB02013_00-PJDF2MOMJTNSDAYO.rap", klic);
        io_dispose(f0);
        io_dispose(f1);
    }
}

bool close;

extern LARGE_INTEGER performance_frequency;

typedef struct thread {
    HANDLE handle;
    DWORD id;
} thread;

wchar_t* glitter_test_file;

int wmain(int argc, wchar_t** argv) {
    if (argc <= 1)
        //return;
        glitter_test_file = 0;
        //glitter_file = L"eff_pv249_kirakira";
    else
        glitter_test_file = argv[1];

    //ShowWindow(GetConsoleWindow(), SW_HIDE);
    QueryPerformanceFrequency(&performance_frequency);

    render_init_struct ris;
    ris.res.x = 0;
    ris.res.y = 0;
    ris.internal_res.x = 0;
    ris.internal_res.y = 0;
    ris.scale = 0.0f;

    thread control, input, render, sound;
    HANDLE h[4];
    h[0] = control.handle = CreateThread(0, 0, &control_main, 0, 0, &control.id);
    h[1] = sound.handle = CreateThread(0, 0, &sound_main, 0, 0, &sound.id);
    h[2] = render.handle = CreateThread(0, 0, &render_main, &ris, 0, &render.id);
    h[3] = input.handle = CreateThread(0, 0, &input_main, 0, 0, &input.id);

    if (control.handle && input.handle && render.handle && sound.handle)
        WaitForMultipleObjects(4, h, true, INFINITE);

    if (control.handle)
        CloseHandle(control.handle);
    if (input.handle)
        CloseHandle(input.handle);
    if (render.handle)
        CloseHandle(render.handle);
    if (sound.handle)
        CloseHandle(sound.handle);
    return 0;
}

int wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow) {
    return wmain(__argc, __wargv);
}
