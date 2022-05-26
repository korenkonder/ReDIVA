/*
    by korenkonder
    GitHub/GitLab: korenkonder

    MegaMix+ file keys provided by Skyth
*/

#include <string>
#include <vector>
#include "../KKdLib/default.h"
#include "../KKdLib/io/path.h"
#include "../KKdLib/io/stream.h"
#include "../KKdLib/aes.h"
#include "../KKdLib/str_utils.h"
#include <intrin.h>

static const uint8_t mmplus_key[] = {
    0xCF, 0x53, 0xBF, 0x9C, 0x37, 0x67, 0xAF, 0xB0,
    0x35, 0x54, 0x4E, 0xB9, 0x96, 0xAA, 0x24, 0x39,
    0x26, 0x5D, 0x40, 0x89, 0x7E, 0xD0, 0x1C, 0x3A,
    0x6B, 0xA6, 0x5D, 0xD5, 0xFD, 0x6C, 0x19, 0xA3,
};

static const uint8_t mmplus_iv[] = {
    0xC2, 0x55, 0xFD, 0x73, 0xD8, 0x30, 0xFA, 0xEF,
    0xD5, 0x32, 0x08, 0x54, 0xA2, 0x26, 0x44, 0x14,
};

bool aes_ni;

int32_t wmain(int32_t argc, wchar_t** argv) {
    if (argc < 2) {
        printf("MMPlusDec v0.1.0.0 by korenkonder. Key provided by Skyth\n");
        printf("Usage: MMPlusDec indir [outdir]\n");
        return 0;
    }

    std::wstring indir = argv[1];
    if (!path_check_directory_exists(indir.c_str())) {
        wchar_t buf[MAX_PATH];
        GetCurrentDirectoryW(MAX_PATH, buf);
        indir = std::wstring(buf) + L"\\" + indir;
        if (!path_check_directory_exists(indir.c_str()))
            return 0;
    }

    if (indir.back() != '\\')
        indir += '\\';

    std::wstring outdir = argc == 3 && str_utils_compare(argv[1], argv[2])
        ? argv[2] : (indir.substr(0, indir.size() - 1) + L"_dec");

    if (outdir.back() != '\\')
        outdir += '\\';

    if (!path_check_directory_exists(outdir.c_str())
        && !CreateDirectoryW(outdir.c_str(), 0))
        return 0;

    int32_t cpuid_data[4];
    __cpuid(cpuid_data, 1);
    aes_ni = cpuid_data[2] & 0x2000000 ? true : false;

    {
        std::vector<std::wstring> files;
        path_get_files(&files, indir.c_str());

        for (std::wstring& i : files) {
            std::wstring infile = indir + i;
            std::wstring outfile = outdir + i;

            printf("Decrypting %ls\n", i.c_str());

            stream in_file;
            stream out_file;
            io_open(&in_file, infile.c_str(), L"rb");
            io_open(&out_file, outfile.c_str(), L"wb");

            if (in_file.io.stream && out_file.io.stream) {
                aes256_ctx ctx;
                aes256_init_ctx_iv(&ctx, mmplus_key, mmplus_iv);

                uint8_t buf[0x400];
                size_t size;
                while (size = io_read(&in_file, buf, sizeof(buf))) {
                    aes256_cbc_decrypt_buffer(&ctx, buf, size);
                    io_write(&out_file, buf, size);
                }
            }

            io_free(&in_file);
            io_free(&out_file);
        }
    }

    std::vector<std::wstring> directories;
    path_get_directories_recursive(&directories, indir.c_str(), 0, 0);
    for (std::wstring& i : directories) {
        std::wstring _indir = indir + i + L'\\';
        std::wstring _outdir = outdir + i + L'\\';

        if (!path_check_directory_exists(_outdir.c_str())
            && !CreateDirectoryW(_outdir.c_str(), 0))
            continue;

        std::vector<std::wstring> files;
        path_get_files(&files, _indir.c_str());

        for (std::wstring& j : files) {
            std::wstring infile = _indir + j;
            std::wstring outfile = _outdir + j;

            printf("Decrypting %ls\\%ls\n", i.c_str(), j.c_str());

            stream in_file;
            stream out_file;
            io_open(&in_file, infile.c_str(), L"rb");
            io_open(&out_file, outfile.c_str(), L"wb");

            if (in_file.io.stream && out_file.io.stream) {
                aes256_ctx ctx;
                aes256_init_ctx_iv(&ctx, mmplus_key, mmplus_iv);

                uint8_t buf[0x400];
                size_t size;
                while (size = io_read(&in_file, buf, sizeof(buf))) {
                    aes256_cbc_decrypt_buffer(&ctx, buf, size);
                    io_write(&out_file, buf, size);
                }
            }

            io_free(&in_file);
            io_free(&out_file);
        }
    }
    return 0;
}
