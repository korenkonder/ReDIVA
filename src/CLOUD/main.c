/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "../KKdLib/kkfs.h"
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

bool close;

extern LARGE_INTEGER performance_frequency;
extern double_t inv_performance_frequency_msec;
extern double_t inv_performance_frequency_sec;

typedef struct thread {
    HANDLE handle;
    DWORD id;
} thread;

int32_t wmain(int32_t argc, wchar_t** argv) {
    //ShowWindow(GetConsoleWindow(), SW_HIDE);
    timeBeginPeriod(1);
    QueryPerformanceFrequency(&performance_frequency);
    SetProcessDPIAware();

    inv_performance_frequency_msec = 1000.0 / (double_t)performance_frequency.QuadPart;
    inv_performance_frequency_sec = 1000000.0 / (double_t)performance_frequency.QuadPart;

    int32_t cpuid_data[4];
    __cpuid(cpuid_data, 1);
    aes_ni = cpuid_data[2] & 0x2000000 ? true : false;

    render_init_struct ris;
    ris.res.x = 0;
    ris.res.y = 0;
    ris.scale = 0.0f;

#if defined(VIDEO)
#if defined(DEBUG) || defined(DEBUG_DEV)
#pragma comment(lib, "libx264d.lib")
#else
#pragma comment(lib, "libx264.lib")
#endif

    thread control, input, render, sound, video;
    HANDLE h[5];
    h[0] = control.handle = CreateThread(0, 0, &control_main, 0, 0, &control.id);
    h[1] = input.handle = CreateThread(0, 0, &input_main, 0, 0, &input.id);
    h[2] = render.handle = CreateThread(0, 0, &render_main, &ris, 0, &render.id);
    h[3] = sound.handle = CreateThread(0, 0, &sound_main, 0, 0, &sound.id);
    h[4] = video.handle = CreateThread(0, 0, &video_main, 0, 0, &video.id);

    if (control.handle)
        SetThreadDescription(control.handle, L"Control Thread");
    if (input.handle)
        SetThreadDescription(input.handle, L"Input Thread");
    if (render.handle)
        SetThreadDescription(render.handle, L"Render Thread");
    if (sound.handle)
        SetThreadDescription(sound.handle, L"Sound Thread");
    if (video.handle)
        SetThreadDescription(sound.handle, L"Video Thread");

    lock_init(&state_lock);
    state = RENDER_UNINITIALIZED;
    if (control.handle && input.handle && render.handle && sound.handle && video.handle)
        WaitForMultipleObjects(5, h, true, INFINITE);
    lock_free(&state_lock);

    if (control.handle)
        CloseHandle(control.handle);
    if (input.handle)
        CloseHandle(input.handle);
    if (render.handle)
        CloseHandle(render.handle);
    if (sound.handle)
        CloseHandle(sound.handle);
    if (video.handle)
        CloseHandle(video.handle);
#else
    thread control, input, render, sound;
    HANDLE h[4];
    h[0] = control.handle = CreateThread(0, 0, &control_main, 0, 0, &control.id);
    h[1] = input.handle = CreateThread(0, 0, &input_main, 0, 0, &input.id);
    h[2] = render.handle = CreateThread(0, 0, &render_main, &ris, 0, &render.id);
    h[3] = sound.handle = CreateThread(0, 0, &sound_main, 0, 0, &sound.id);
    
    if (control.handle)
        SetThreadDescription(control.handle, L"Control Thread");
    if (input.handle)
        SetThreadDescription(input.handle, L"Input Thread");
    if (render.handle)
        SetThreadDescription(render.handle, L"Render Thread");
    if (sound.handle)
        SetThreadDescription(sound.handle, L"Sound Thread");

    lock_init(&state_lock);
    state = RENDER_UNINITIALIZED;
    if (control.handle && input.handle && render.handle && sound.handle)
        WaitForMultipleObjects(4, h, true, INFINITE);
    lock_free(&state_lock);

    if (control.handle)
        CloseHandle(control.handle);
    if (input.handle)
        CloseHandle(input.handle);
    if (render.handle)
        CloseHandle(render.handle);
    if (sound.handle)
        CloseHandle(sound.handle);
#endif
    timeEndPeriod(1);
    return 0;
}

int32_t wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
    _In_ PWSTR pCmdLine, _In_ int32_t nCmdShow) {
    return wmain(__argc, __wargv);
}
