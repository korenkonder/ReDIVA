/*
    by korenkonder
    GitHub/GitLab: korenkonder

    MegaMix+ file keys provided by Skyth
*/

#include <string>
#include <vector>
#include "../KKdLib/default.hpp"
#include "../KKdLib/io/path.hpp"
#include "../KKdLib/io/stream.hpp"
#include "../KKdLib/aes.hpp"
#include "../KKdLib/str_utils.hpp"
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
        printf("MMPlusDec v0.1.2.0 by korenkonder. Key provided by Skyth\n");
        printf("Usage: MMPlusDec indir [outdir] [-e]\n");
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

    bool encrypt = (argc == 3 && !str_utils_compare(argv[2], L"-e"))
        || (argc == 4 && !str_utils_compare(argv[3], L"-e"));

    std::wstring outdir = argc >= 3 && str_utils_compare(argv[2], L"-e")
        && str_utils_compare(argv[1], argv[2]) ? argv[2]
        : (indir.substr(0, indir.size() - 1) + (encrypt ? L"_enc" : L"_dec"));

    if (outdir.back() != '\\')
        outdir += '\\';

    if (!path_check_directory_exists(outdir.c_str())
        && !CreateDirectoryW(outdir.c_str(), 0))
        return 0;

    int32_t cpuid_data[4];
    __cpuid(cpuid_data, 1);
    aes_ni = cpuid_data[2] & 0x2000000 ? true : false;

    aes256_ctx ctx;
    aes256_init_ctx(&ctx, mmplus_key);

    {
        std::vector<std::wstring> files;
        path_get_files(&files, indir.c_str());

        for (std::wstring& i : files) {
            std::wstring infile = indir + i;
            std::wstring outfile = outdir + i;


            stream in_file;
            stream out_file;
            in_file.open(infile.c_str(), L"rb");
            out_file.open(outfile.c_str(), L"wb");

            if (in_file.io.stream && out_file.io.stream) {
                printf(encrypt ? "Encrypting %ls\n" : "Decrypting %ls\n", i.c_str());

                aes256_ctx_set_iv(&ctx, mmplus_iv);

                size_t length = in_file.get_length();

                uint8_t buf[0x400];
                size_t size;
                if (encrypt) {
                    while (length && (size = in_file.read(buf, sizeof(buf)))) {
                        if (length == size) {
                            uint8_t align = (uint8_t)(0x10 - size % 0x10);
                            for (uint8_t i = 0; i < align; i++)
                                buf[size + i] = align;
                            size += align;
                        }

                        aes256_cbc_encrypt_buffer(&ctx, buf, size);
                        out_file.write(buf, size);

                        length -= size;
                    }
                }
                else {
                    while (length && (size = in_file.read(buf, sizeof(buf)))) {
                        aes256_cbc_decrypt_buffer(&ctx, buf, size);

                        if (length == size)
                            size -= buf[size - 1];

                        out_file.write(buf, size);

                        length -= size;
                    }
                }
            }
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

            stream in_file;
            stream out_file;
            in_file.open(infile.c_str(), L"rb");
            out_file.open(outfile.c_str(), L"wb");

            if (in_file.io.stream && out_file.io.stream) {
                printf(encrypt ? "Encrypting %ls\\%ls\n" : "Decrypting %ls\\%ls\n", i.c_str(), j.c_str());

                aes256_ctx_set_iv(&ctx, mmplus_iv);

                size_t length = in_file.get_length();

                uint8_t buf[0x400];
                size_t size;
                if (encrypt) {
                    while (length && (size = in_file.read(buf, sizeof(buf)))) {
                        if (length == size) {
                            uint8_t align = (uint8_t)(0x10 - size % 0x10);
                            for (uint8_t i = 0; i < align; i++)
                                buf[size + i] = align;
                            size += align;
                        }

                        aes256_cbc_encrypt_buffer(&ctx, buf, size);
                        out_file.write(buf, size);

                        length -= size;
                    }
                }
                else {
                    while (length && (size = in_file.read(buf, sizeof(buf)))) {
                        aes256_cbc_decrypt_buffer(&ctx, buf, size);

                        if (length == size)
                            size -= buf[size - 1];

                        out_file.write(buf, size);

                        length -= size;
                    }
                }
            }
        }
    }
    return 0;
}
