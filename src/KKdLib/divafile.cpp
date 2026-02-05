/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "divafile.hpp"
#include "io/file_stream.hpp"
#include "prj/rijndael.hpp"
#include "str_utils.hpp"

namespace divafile {
    static const uint8_t key[] = {
        0x66, 0x69, 0x6C, 0x65, 0x20, 0x61, 0x63, 0x63,
        0x65, 0x73, 0x73, 0x20, 0x64, 0x65, 0x6E, 0x79
    };

    void decrypt(const char* path) {
        wchar_t* file_buf = utf8_to_utf16(path);
        decrypt(file_buf);
        free_def(file_buf);
    }

    void decrypt(const wchar_t* path) {
        wchar_t* file_temp = str_utils_add(path, L"_dec");
        if (!file_temp)
            return;

        file_stream s_enc;
        s_enc.open(path, L"rb");
        if (s_enc.check_not_null()) {
            uint64_t signature = s_enc.read_uint64_t();
            if (signature == 0x454C494641564944) {
                uint32_t stream_length = s_enc.read_uint32_t();
                uint32_t file_length = s_enc.read_uint32_t();
                void* data = force_malloc(stream_length);
                s_enc.read(data, stream_length);

                prj::Rijndael rijndael(prj::Rijndael_Nb, prj::Rijndael_Nk128, key);
                for (size_t i = 0; i < stream_length; i += prj::Rijndael_Nlen)
                    rijndael.decrypt16((uint8_t*)data + i);

                file_stream s_dec;
                s_dec.open(file_temp, L"wb");
                s_dec.write(data, min_def(file_length, stream_length));
                free_def(data);
            }
        }
        free_def(file_temp);
    }

    void decrypt(void* enc_data, void** dec_data, size_t* dec_size) {
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

        prj::Rijndael rijndael(prj::Rijndael_Nb, prj::Rijndael_Nk128, key);
        for (size_t i = 0; i < stream_length; i += prj::Rijndael_Nlen)
            rijndael.decrypt16((uint8_t*)data + i);

        *dec_data = data;
        *dec_size = file_length;
    }

    void decrypt(stream& enc, memory_stream& dec) {
        size_t pos = enc.get_position();
        uint64_t signature = enc.read_uint64_t();
        if (signature != 0x454C494641564944) {
            std::vector<uint8_t> data;
            enc.set_position(0, SEEK_SET);
            int64_t length = enc.get_length();
            data.resize(length);
            enc.read(data.data(), length);
            enc.set_position(pos, SEEK_SET);
            dec.open(data);
            return;
        }

        uint32_t stream_length = enc.read_uint32_t();
        uint32_t file_length = enc.read_uint32_t();
        void* data = force_malloc(stream_length);
        enc.read(data, stream_length);

        prj::Rijndael rijndael(prj::Rijndael_Nb, prj::Rijndael_Nk128, key);
        for (size_t i = 0; i < stream_length; i += prj::Rijndael_Nlen)
            rijndael.decrypt16((uint8_t*)data + i);

        dec.open(data, file_length);
        free_def(data);
    }

    void encrypt(const char* path) {
        wchar_t* file_buf = utf8_to_utf16(path);
        encrypt(file_buf);
        free_def(file_buf);
    }

    void encrypt(const wchar_t* path) {
        wchar_t* file_temp = str_utils_add(path, L"_enc");
        if (!file_temp)
            return;

        file_stream s_dec;
        s_dec.open(path, L"rb");
        if (s_dec.check_not_null()) {
            size_t len = s_dec.length;
            size_t len_align = align_val(len, 0x10);

            void* data = force_malloc(len_align);
            s_dec.read(data, len);

            prj::Rijndael rijndael(prj::Rijndael_Nb, prj::Rijndael_Nk128, key);
            for (size_t i = 0; i < len_align; i += prj::Rijndael_Nlen)
                rijndael.encrypt16((uint8_t*)data + i);

            file_stream s_enc;
            s_enc.open(file_temp, L"wb");
            s_enc.write_uint64_t(0x454C494641564944);
            s_enc.write_uint32_t((uint32_t)len_align);
            s_enc.write_uint32_t((uint32_t)len);
            s_enc.write(data, len_align);
            free_def(data);
        }
        free_def(file_temp);
    }

    void encrypt(void* dec_data, size_t dec_size, void** enc_data, size_t* enc_size) {
        if (!dec_data || !dec_size || !enc_data || !enc_size)
            return;

        size_t len = dec_size;
        size_t len_align = align_val(len, 0x10);

        void* data = force_malloc(len_align + 0x10);
        size_t d = (size_t)data;
        memcpy((void*)(d + 16), dec_data, len);

        prj::Rijndael rijndael(prj::Rijndael_Nb, prj::Rijndael_Nk128, key);
        for (size_t i = 0; i < len_align; i += prj::Rijndael_Nlen)
            rijndael.encrypt16((uint8_t*)data + i);

        *(uint64_t*)d = 0x454C494641564944;
        *(uint32_t*)(d + 8) = (uint32_t)len_align;
        *(uint32_t*)(d + 12) = (uint32_t)len;

        *enc_data = data;
        *enc_size = len_align + 0x10;
    }
}
