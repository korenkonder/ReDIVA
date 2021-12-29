/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "divafile.h"
#include "aes.h"
#include "str_utils.h"

static const uint8_t key[] = {
    0x66, 0x69, 0x6C, 0x65, 0x20, 0x61, 0x63, 0x63,
    0x65, 0x73, 0x73, 0x20, 0x64, 0x65, 0x6E, 0x79
};

void divafile_decrypt(char* path) {
    wchar_t* file_buf = utf8_to_utf16(path);
    divafile_wdecrypt(file_buf);
    free(file_buf);
}

void divafile_wdecrypt(wchar_t* path) {
    wchar_t* file_temp = str_utils_wadd(path, L"_dec");
    stream s_enc;
    io_wopen(&s_enc, path, L"rb");
    if (s_enc.io.stream) {
        uint64_t signature = io_read_uint64_t(&s_enc);
        if (signature == 0x454C494641564944) {
            uint32_t stream_length = io_read_uint32_t(&s_enc);
            uint32_t file_length = io_read_uint32_t(&s_enc);
            void* data = force_malloc(stream_length);
            io_read(&s_enc, data, stream_length);

            struct aes_ctx ctx;
            aes_init_ctx(&ctx, (uint8_t*)key);
            aes_ecb_decrypt_buffer(&ctx, data, stream_length);

            stream s_dec;
            io_wopen(&s_dec, file_temp, L"wb");
            io_write(&s_dec, data, min(file_length, stream_length));
            io_free(&s_dec);
            free(data);
        }
    }
    io_free(&s_enc);
    free(file_temp);
}

void divafile_mdecrypt(void* enc_data, void** dec_data, size_t* dec_size) {
    if (!enc_data || !dec_data || !dec_size)
        return;

    *dec_data = 0;
    *dec_size = 0;

    size_t d = (size_t)enc_data;

    uint64_t signature = *(uint64_t*)d;
    if (signature != 0x454C494641564944)
        return;

    uint32_t stream_length = *(uint32_t*)(d + 8);
    uint32_t file_length = *(uint32_t*)(d + 12);
    void* data = force_malloc(stream_length);
    memcpy(data, (void*)(d + 16), stream_length);

    struct aes_ctx ctx;
    aes_init_ctx(&ctx, (uint8_t*)key);
    aes_ecb_decrypt_buffer(&ctx, data, stream_length);

    *dec_data = data;
    *dec_size = file_length;
}

void divafile_sdecrypt(stream* s) {
    if (!s)
        return;

    size_t pos = io_get_position(s);
    uint64_t signature = io_read_uint64_t(s);
    if (signature != 0x454C494641564944) {
        io_set_position(s, pos, SEEK_SET);
        return;
    }

    uint32_t stream_length = io_read_uint32_t(s);
    uint32_t file_length = io_read_uint32_t(s);
    void* data = force_malloc(stream_length);
    io_read(s, data, stream_length);

    struct aes_ctx ctx;
    aes_init_ctx(&ctx, (uint8_t*)key);
    aes_ecb_decrypt_buffer(&ctx, data, stream_length);

    io_free(s);
    io_mopen(s, data, file_length);
    free(data);
}

void divafile_encrypt(char* path) {
    wchar_t* file_buf = utf8_to_utf16(path);
    divafile_wencrypt(file_buf);
    free(file_buf);
}

void divafile_wencrypt(wchar_t* path) {
    wchar_t* file_temp = str_utils_wadd(path, L"_enc");
    stream s_dec;
    io_wopen(&s_dec, path, L"rb");
    if (s_dec.io.stream) {
        size_t len = s_dec.length;
        size_t len_align = align_val(len, 0x10);

        void* data = force_malloc(len_align);
        io_read(&s_dec, data, len);

        struct aes_ctx ctx;
        aes_init_ctx(&ctx, (uint8_t*)key);
        aes_ecb_encrypt_buffer(&ctx, data, len_align);

        stream s_enc;
        io_wopen(&s_enc, file_temp, L"wb");
        io_write_uint64_t(&s_enc, 0x454C494641564944);
        io_write_uint32_t(&s_enc, (uint32_t)len_align);
        io_write_uint32_t(&s_enc, (uint32_t)len);
        io_write(&s_enc, data, len_align);
        io_free(&s_enc);
        free(data);
    }
    io_free(&s_dec);
    free(file_temp);
}

void divafile_mencrypt(void* dec_data, size_t dec_size, void** enc_data, size_t* enc_size) {
    if (!dec_data || !dec_size || !enc_data || !enc_size)
        return;

    size_t len = dec_size;
    size_t len_align = align_val(len, 0x10);

    void* data = force_malloc(len_align + 0x10);
    size_t d = (size_t)data;
    memcpy((void*)(d + 16), dec_data, len);

    struct aes_ctx ctx;
    aes_init_ctx(&ctx, (uint8_t*)key);
    aes_ecb_encrypt_buffer(&ctx, (void*)(d + 16), len_align);

    *(uint64_t*)d = 0x454C494641564944;
    *(uint32_t*)(d + 8) = (uint32_t)len_align;
    *(uint32_t*)(d + 12) = (uint32_t)len;

    *enc_data = data;
    *enc_size = len_align + 0x10;
}
