/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "main.hpp"
#include "data_process/compile_shaders.hpp"
#include <intrin.h>
#include <timeapi.h>

bool cpu_caps_ssse3;
bool cpu_caps_sse41;
bool cpu_caps_sse42;
bool cpu_caps_aes_ni;
bool cpu_caps_avx;
bool cpu_caps_f16c;
bool cpu_caps_avx2;

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
                stru_140EDA5B0.start_up_mode = 0;
            else
                stru_140EDA5B0.start_up_mode = 1;
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
    if (argc >= 2 && !wcscmp(argv[1], L"--compile-spir-v")) {
        compile_all_shaders(argc >= 3 && !wcscmp(argv[2], L"-d"));
        return 0;
    }

    bool vulkan = false;
    const wchar_t* config_path = 0;
    if (argc >= 2)
        for (int32_t i = 1; i < argc; i++) {
            if (!wcscmp(argv[i], L"--vulkan"))
                vulkan = true;
            else if (!wcscmp(argv[i], L"--config") && i + 1 < argc)
                config_path = argv[++i];
        }

    //ShowWindow(GetConsoleWindow(), SW_HIDE);
    timeBeginPeriod(1);
    SetProcessDPIAware();

    int32_t cpuid_data[4] = {};
    __cpuid(cpuid_data, 1);
    cpu_caps_ssse3 = !!(cpuid_data[2] & (1 << 9));
    cpu_caps_sse41 = !!(cpuid_data[2] & (1 << 19));
    cpu_caps_sse42 = !!(cpuid_data[2] & (1 << 20));
    cpu_caps_aes_ni = !!(cpuid_data[2] & (1 << 25));
    cpu_caps_avx = !!(cpuid_data[2] & (1 << 28));
    cpu_caps_f16c = !!(cpuid_data[2] & (1 << 29));

    int32_t cpuidex_data[4] = {};
    __cpuidex(cpuidex_data, 7, 0);
    cpu_caps_avx2 = !!(cpuidex_data[1] & (1 << 5));

    app_main({ vulkan, config_path });

    timeEndPeriod(1);
    return 0;
}

int32_t wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
    _In_ PWSTR pCmdLine, _In_ int32_t nCmdShow) {
    return wmain(__argc, __wargv);
}
