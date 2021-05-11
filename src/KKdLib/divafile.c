/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "divafile.h"
#include "aes.h"
#include "io_stream.h"
#include "str_utils.h"

static const uint8_t key[] = {
    0x66, 0x69, 0x6C, 0x65, 0x20, 0x61, 0x63, 0x63,
    0x65, 0x73, 0x73, 0x20, 0x64, 0x65, 0x6E, 0x79
};

void divafile_decrypt(char* file) {
    wchar_t* file_buf = char_string_to_wchar_t_string(file);
    divafile_wdecrypt(file_buf);
    free(file_buf);
}

void divafile_wdecrypt(wchar_t* file) {
    wchar_t* file_temp = str_utils_wadd(file, L"_dec");
    stream* s_enc = io_wopen(file, L"rb");
    uint64_t signature = io_read_uint64_t(s_enc);
    if (signature == 0x454C494641564944) {
        uint32_t stream_length = io_read_uint32_t(s_enc);
        uint32_t file_length = io_read_uint32_t(s_enc);
        void* data = force_malloc(stream_length);
        io_read(s_enc, data, stream_length);

        struct aes_ctx ctx;
        aes_init_ctx(&ctx, key);
        aes_ecb_decrypt_buffer(&ctx, data, stream_length);

        stream* s_dec = io_wopen(file_temp, L"wb");
        io_write(s_dec, data, min(file_length, stream_length));
        free(data);
        io_dispose(s_dec);
    }
    io_dispose(s_enc);
    free(file_temp);
}

void divafile_encrypt(char* file) {
    wchar_t* file_buf = char_string_to_wchar_t_string(file);
    divafile_wencrypt(file_buf);
    free(file_buf);
}

void divafile_wencrypt(wchar_t* file) {
    wchar_t* file_temp = str_utils_wadd(file, L"_enc");
    stream* s_dec = io_wopen(file, L"rb");
    size_t len = s_dec->length;
    size_t len_align = align_val(len, 0x10);

    void* data = force_malloc(len_align);
    io_read(s_dec, data, len);

    struct aes_ctx ctx;
    aes_init_ctx(&ctx, key);
    aes_ecb_encrypt_buffer(&ctx, data, len_align);

    stream* s_enc = io_wopen(file_temp, L"wb");
    io_write_uint64_t(s_enc, 0x454C494641564944);
    io_write_uint32_t(s_enc, (uint32_t)len_align);
    io_write_uint32_t(s_enc, (uint32_t)len);
    io_write(s_enc, data, len_align);
    io_dispose(s_enc);

    free(data);

    io_dispose(s_dec);
    free(file_temp);
}
