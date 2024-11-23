/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "../KKdLib/default.hpp"
#include "../KKdLib/io/path.hpp"
#include "../KKdLib/x_save.hpp"
#include <intrin.h>
#include <string>

bool cpu_caps_aes_ni;

static int32_t print_help();

int32_t wmain(int32_t argc, wchar_t** argv) {
    int32_t cpuid_data[4] = {};
    __cpuid(cpuid_data, 1);
    cpu_caps_aes_ni = !!(cpuid_data[2] & (1 << 25));

    if (argc < 3)
        return print_help();

    if (wcscmp(argv[1], L"d") && wcscmp(argv[1], L"e"))
        return print_help();

    if (!path_check_file_exists(argv[2])) {
        printf("Failed to find input file!\n");
        return 0;
    }

    std::wstring infile(argv[2]);
    std::wstring outfile;
    std::wstring options;

    if (argc == 4) {
        if (*argv[3] != '-')
            outfile.assign(argv[3]);
        else
            options.assign(argv[3] + 1);
    }
    else if (argc >= 4) {
        outfile.assign(argv[3]);
        if (*argv[4] == '-')
            options.assign(argv[4] + 1);
    }

    if (!wcscmp(argv[1], L"d")) {
        if (!outfile.size()) {
            size_t sep1_pos = infile.rfind('/');
            size_t sep2_pos = infile.rfind('\\');
            size_t pos = infile.rfind('.');
            if (pos != -1 && (sep1_pos != -1 && sep1_pos < pos || sep1_pos == -1)
                && (sep2_pos != -1 && sep2_pos < pos || sep2_pos == -1)) {
                outfile.assign(infile, 0, pos);
                outfile.append(L"_dec");
                outfile.append(infile, pos);
            }
            else {
                outfile.assign(infile);
                outfile.append(L"_dec");
            }
        }

        if (!x_save_decode(infile.c_str(), outfile.c_str()))
            printf("Failed to decode file!\n");
    }
    else if (!wcscmp(argv[1], L"e")) {
        if (!outfile.size()) {
            size_t sep1_pos = infile.rfind('/');
            size_t sep2_pos = infile.rfind('\\');
            size_t pos = infile.rfind('.');
            if (pos != -1 && (sep1_pos != -1 && sep1_pos < pos || sep1_pos == -1)
                && (sep2_pos != -1 && sep2_pos < pos || sep2_pos == -1)) {
                outfile.assign(infile, 0, pos);
                outfile.append(L"_enc");
                outfile.append(infile, pos);
            }
            else {
                outfile.assign(infile);
                outfile.append(L"_enc");
            }
        }

        if (!options.size())
            options.assign(L"gaxc");

        x_save_encode_flags flags = (x_save_encode_flags)0;
        for (const wchar_t& c : options)
            if (c == L'g')
                enum_or(flags, X_SAVE_ENCODE_GZIP);
            else if (c == L'a')
                enum_or(flags, X_SAVE_ENCODE_AES);
            else if (c == L'x')
                enum_or(flags, X_SAVE_ENCODE_XOR);
            else if (c == L'c')
                enum_or(flags, X_SAVE_ENCODE_CRC);

        if (!x_save_encode(infile.c_str(), outfile.c_str(), flags))
            printf("Failed to encode file!\n");
    }
    return 0;
}

static int32_t print_help() {
    printf("XSaveEncDec v0.1.0.0 by korenkonder.\n");
    printf("Usage (decode): XSaveEncDec d infile [outfile]\n");
    printf("Usage (encode): XSaveEncDec e infile [outfile] [-gaxc]\n");
    printf("\n");
    printf("Examples:\n");
    printf("  XSaveEncDec d in_file.bin out_file.bin\n"
        "    # Decode save file\n");
    printf("  XSaveEncDec e in_file.bin out_file.bin\n"
        "    # Encode save file with default option -gaxc\n"
        "    # Compress it, encrypt it, apply XOR and calculate CRC checksum\n");
    printf("  XSaveEncDec e in_file.bin out_file.bin -gc\n"
        "    # Encode save file. Compress it and calculate CRC checksum\n");
    printf("\n");
    printf("Encode options:\n");
    printf(" -g     compress data\n");
    printf(" -a     encrypt data\n");
    printf(" -x     apply XOR\n");
    printf(" -c     calculate CRC\n");
    return 0;
}
