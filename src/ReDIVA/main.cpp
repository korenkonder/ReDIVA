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

/*void read_arg(int32_t argc, wchar_t** argv) {
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
            s_main_info.debug_mode = 1;
        else if (!i->compare(L"-t"))
            sys_am_set_test_mode_starting(true);
        else  if (!i->compare(L"-m")) {
            if (++i == i_end)
                break;

            s_main_info.start_mode = _wtoi(i->c_str());
        }
        else if (!i->compare(L"-s")) {
            if (++i == i_end)
                break;

            s_main_info.start_mode_sub = _wtoi(i->c_str());
        }
        else if (!i->compare(L"-sm") || !i->compare(L"--start-up-mode")) {
            if (++i == i_end)
                break;

            if (i->compare(L"server"))
                s_main_info.start_up_mode = START_UP_MODE_SERVER;
            else
                s_main_info.start_up_mode = START_UP_MODE_SATELLITE;
        }
        else if (!i->compare(L"-ve"))
            s_main_info.vsync_emu = 1;
        else if (!i->compare(L"-fs") || !i->compare(L"-f"))
            s_main_info.full_screen = 1;
        else if (!i->compare(L"-w"))
            s_main_info.full_screen = 0;
        else if (!i->compare(L"-aa"))
            s_main_info.anti_alias = 1;
        else if (!i->compare(L"-qvga")) {
            s_main_info.screen_mode = SCREEN_MODE_QVGA;
            s_main_info.min_render = 0;
        }
        else if (!i->compare(L"-vga")) {
            s_main_info.screen_mode = SCREEN_MODE_VGA;
            s_main_info.min_render = 0;
        }
        else if (!i->compare(L"-wvga")) {
            s_main_info.screen_mode = SCREEN_MODE_WVGA;
            s_main_info.min_render = 0;
        }
        else if (!i->compare(L"-svga")) {
            s_main_info.screen_mode = SCREEN_MODE_SVGA;
            s_main_info.min_render = 0;
        }
        else if (!i->compare(L"-xga")) {
            s_main_info.screen_mode = SCREEN_MODE_XGA;
            s_main_info.min_render = 0;
        }
        else if (!i->compare(L"-wxga")) {
            s_main_info.screen_mode = SCREEN_MODE_WXGA;
            s_main_info.min_render = 1;
        }
        else if (!i->compare(L"-wxga_dbd")) {
            s_main_info.screen_mode = SCREEN_MODE_WXGA;
            s_main_info.min_render = 0;
        }
        else if (!i->compare(L"-uxga")) {
            s_main_info.screen_mode = SCREEN_MODE_UXGA;
            s_main_info.min_render = 0;
        }
        else if (!i->compare(L"-wuxga")) {
            s_main_info.screen_mode = SCREEN_MODE_WUXGA;
            s_main_info.min_render = 0;
        }
        else if (!i->compare(L"-wqxga")) {
            s_main_info.screen_mode = SCREEN_MODE_WQXGA;
            s_main_info.min_render = 0;
        }
        else if (!i->compare(L"-hdtv720")) {
            s_main_info.screen_mode = SCREEN_MODE_HD;
            s_main_info.min_render = 1;
        }
        else if (!i->compare(L"-hdtv720_dbd")) {
            s_main_info.screen_mode = SCREEN_MODE_HD;
            s_main_info.min_render = 0;
        }
        else if (!i->compare(L"-hdtv1080")) {
            s_main_info.screen_mode = SCREEN_MODE_FHD;
            s_main_info.min_render = 0;
        }
        else if (!i->compare(L"-wqhd")) {
            s_main_info.screen_mode = SCREEN_MODE_QHD;
            s_main_info.min_render = 0;
        }
        else if (!i->compare(L"-ss")) {
            if (++i == i_end)
                break;

            s_main_info.ss_flag = 1;
            s_main_info.field_80 = 0;
            s_main_info.field_40.assign(*i);
            s_main_info.field_60.assign(s_main_info.field_40);
            s_main_info.anti_alias = 1;
        }
        else if (!i->compare(L"-no_aa")) {
            s_main_info.anti_alias = 0;
            continue;
        }
        else if (!i->compare(L"-ssformat")) {
            if (++i == i_end)
                break;

            if (!i->compare(L"ppm"))
                s_main_info.ss_suffix = SS_SUFFIX_PPM;
            else if (!i->compare(L"bmp"))
                s_main_info.ss_suffix = SS_SUFFIX_BMP;
            else if (!i->compare(L"tga"))
                s_main_info.ss_suffix = SS_SUFFIX_TGA;
            else if (!i->compare(L"yuy2"))
                s_main_info.ss_suffix = SS_SUFFIX_YUY2;
            else if (!i->compare(L"jpg"))
                s_main_info.ss_suffix = SS_SUFFIX_JPEG;
        }
        else if (!i->compare(L"-ss4x")) {
            if (++i == i_end)
                break;

            s_main_info.ss_flag = 1;
            s_main_info.field_80 = 0;
            s_main_info.field_40.assign(*i);
            s_main_info.field_60.assign(s_main_info.field_40);
            s_main_info.ss_mode = SS_MODE_SIZE_4X;
            s_main_info.anti_alias = 1;
        }
        else if (!i->compare(L"-ss36x"));
        else if (i->compare(L"-ssalphamask"))
            s_main_info.ss_alpha_mask = 1;
        else if (!i->compare(L"-fix_pv_timer"))
            s_main_info.fix_pv_timer = 1;
        else if (!i->compare(L"-ps3"))
            s_main_info.ps3 = 1;
        else if (!i->compare(L"-dev_rom"))
            s_main_info.dev_rom = 1;
        else if (!i->compare(L"-pv_full_open"))
            s_main_info.pv_full_open = 1;
        else if (!i->compare(L"-no_local_rom"))
            s_main_info.local_rom = 0;
        else if (!i->compare(L"-pcv"))
            s_main_info.pcv = 1;
    }

    screen_shot_impl_ptr->set_format(s_main_info.ss_suffix);

    if (s_main_info.anti_alias || s_main_info.ss_flag)
        s_main_info.min_render = 0;

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

    int32_t max_minor_ver = 6;
    bool vulkan = false;
    const wchar_t* config_path = 0;
    if (argc >= 2)
        for (int32_t i = 1; i < argc; i++) {
            if (!wcscmp(argv[i], L"--max-minor-ver") && i + 1 < argc)
                max_minor_ver = _wtoi(argv[++i]);
            else if (!wcscmp(argv[i], L"--vulkan"))
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

    app_main({ max_minor_ver, vulkan, config_path });

    timeEndPeriod(1);
    return 0;
}

int32_t wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
    _In_ PWSTR pCmdLine, _In_ int32_t nCmdShow) {
    return wmain(__argc, __wargv);
}
