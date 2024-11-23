/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "x_save.hpp"
#include "f2/header.hpp"
#include "io/file_stream.hpp"
#include "aes.hpp"
#include "deflate.hpp"

static const uint8_t savedata_key[] = {
    0xA4, 0xB7, 0x31, 0xD0, 0x33, 0xFB, 0x4A, 0x63, 0x9D, 0xD2, 0x46, 0xA5, 0x05, 0xCD, 0x4B, 0xE5,
    0xF2, 0x10, 0xEE, 0x05, 0xC3, 0x56, 0x45, 0x3A, 0xAF, 0x22, 0x5C, 0x88, 0xA0, 0x9F, 0xB6, 0x8A,
};

static const uint32_t savedata_version = 0xE589B8;

static bool x_save_decode(std::vector<uint8_t>& data, void*& dec, size_t& dec_len);
static bool x_save_encode(std::vector<uint8_t>& data, void*& enc, size_t& enc_len, x_save_encode_flags flags);

bool x_save_decode(const char* in_path, const char* out_path) {
    std::vector<uint8_t> data;
    {
        file_stream ifs;
        ifs.open(in_path, "rb");
        if (ifs.check_null() || ifs.get_length() <= F2_HEADER_DEFAULT_LENGTH)
            return false;

        data.resize(ifs.get_length());

        ifs.read(data.data(), data.size());
        ifs.close();
    }

    void* dec = 0;
    size_t dec_len = 0;
    if (!x_save_decode(data, dec, dec_len))
        return false;

    if (dec && dec_len) {
        file_stream ofs;
        ofs.open(out_path, "wb");
        ofs.write(dec, dec_len);
        ofs.close();
    }

    if (dec)
        free(dec);
    return true;
}

bool x_save_decode(const wchar_t* in_path, const wchar_t* out_path) {
    std::vector<uint8_t> data;
    {
        file_stream ifs;
        ifs.open(in_path, L"rb");
        if (ifs.check_null() || ifs.get_length() <= F2_HEADER_DEFAULT_LENGTH)
            return false;

        data.resize(ifs.get_length());

        ifs.read(data.data(), data.size());
        ifs.close();
    }

    void* dec = 0;
    size_t dec_len = 0;
    if (!x_save_decode(data, dec, dec_len))
        return false;

    if (dec && dec_len) {
        file_stream ofs;
        ofs.open(out_path, L"wb");
        ofs.write(dec, dec_len);
        ofs.close();
    }

    if (dec)
        free(dec);
    return true;
}

bool x_save_encode(const char* in_path, const char* out_path, x_save_encode_flags flags) {
    std::vector<uint8_t> data;
    {
        file_stream ifs;
        ifs.open(in_path, "rb");
        if (ifs.check_null())
            return false;

        data.resize(ifs.get_length());

        ifs.read(data.data(), data.size());
        ifs.close();
    }

    void* enc;
    size_t enc_len = 0;
    if (!x_save_encode(data, enc, enc_len, flags))
        return false;

    if (enc && enc_len) {
        file_stream ofs;
        ofs.open(out_path, "wb");
        ofs.write(enc, enc_len);
        ofs.close();
    }

    if (enc)
        free(enc);
    return true;
}

bool x_save_encode(const wchar_t* in_path, const wchar_t* out_path, x_save_encode_flags flags) {
    std::vector<uint8_t> data;
    {
        file_stream ifs;
        ifs.open(in_path, L"rb");
        if (ifs.check_null())
            return false;

        data.resize(ifs.get_length());

        ifs.read(data.data(), data.size());
        ifs.close();
    }

    void* enc;
    size_t enc_len = 0;
    if (!x_save_encode(data, enc, enc_len, flags))
        return false;

    if (enc && enc_len) {
        file_stream ofs;
        ofs.open(out_path, L"wb");
        ofs.write(enc, enc_len);
        ofs.close();
    }

    if (enc)
        free(enc);
    return true;
}

static bool x_save_decode(std::vector<uint8_t>& data, void*& dec, size_t& dec_len) {
    f2_header* head = (f2_header*)data.data();

    bool reset_flags = head->attrib.get_gzip() || head->attrib.get_xor_data();

    uint32_t version = 0;
    if (head)
        version = head->version;
    if (version != (savedata_version & 0x7FFFFFF))
        return false;

    if (head->attrib.get_crc()) {
        if (!head->validate_crc())
            return false;

        if (reset_flags)
            head->reset_crc();
    }

    if (head->attrib.get_xor_data())
        head->remove_xor();

    if (head->attrib.get_aes()) {
        uint8_t* section_data = head->get_section_data();
        uint32_t section_size = head->get_section_size();
        if (section_data && section_size == align_val(section_size, 0x20)) {
            aes256_ctx aes;
            aes256_init_ctx(&aes, savedata_key);
            aes256_ecb_decrypt_buffer(&aes, section_data, section_size);
            head->attrib.set_aes(false);
        }
    }

    if (head->attrib.get_gzip()) {
        uint8_t* section_data = head->get_section_data();
        uint32_t section_size = head->get_section_size();
        if (!head->custom || !section_data || !section_size)
            return false;

        dec = 0;
        dec_len = head->custom;
        if (deflate::decompress(section_data, section_size, &dec, &dec_len, deflate::MODE_GZIP) < 0
            || !dec_len || dec_len < head->custom) {
            if (dec)
                free(dec);
            return false;
        }

        if (reset_flags)
            head->attrib.set_gzip(false);
    }
    else {
        dec = malloc(head->custom);
        if (dec) {
            dec_len = head->custom;
            memset(dec, 0, head->custom);
            memcpy(dec, head->get_section_data(), min_def(head->custom, head->get_section_size()));
        }
    }
    return true;
}

static bool x_save_encode(std::vector<uint8_t>& data, void*& enc, size_t& enc_len, x_save_encode_flags flags) {
    enc_len = align_val(data.size(), 0x20) + F2_HEADER_EXTENDED_LENGTH;
    enc = malloc(enc_len);

    if (!enc || enc_len <= F2_HEADER_EXTENDED_LENGTH) {
        if (enc)
            free(enc);
        return false;
    }

    memset(enc, 0, enc_len);

    f2_header* head = new (enc) f2_header('DATA',
        (uint32_t)(enc_len - F2_HEADER_DEFAULT_LENGTH), 0, true);

    uint32_t size = (uint32_t)data.size();
    if (head)
        head->custom = size;
    if (head)
        head->version = savedata_version & 0x7FFFFFF;

    if (flags & X_SAVE_ENCODE_GZIP) {
        void* comp = 0;
        size_t comp_len = 0;

        if (deflate::compress(data.data(), data.size(), &comp, &comp_len, 12, deflate::MODE_GZIP) < 0
            || !comp_len || head->get_section_size() < comp_len) {
            if (comp)
                free(comp);

            if (size > head->get_section_size()) {
                if (enc)
                    free(enc);
                return false;
            }

            memcpy(head->get_section_data(), data.data(), size);
        }
        else {
            memcpy(head->get_section_data(), comp, comp_len);
            if (comp)
                free(comp);

            uint32_t comp_size = align_val((uint32_t)comp_len, 0x20);
            head->set_section_size(comp_size);
            head->set_data_size(comp_size);
            enc_len = (size_t)comp_size + F2_HEADER_DEFAULT_LENGTH;
            head->attrib.set_gzip(true);
        }
    }
    else {
        if (size > head->get_section_size()) {
            if (enc)
                free(enc);
            return false;
        }

        memcpy(head->get_section_data(), data.data(), size);
    }

    if (flags & X_SAVE_ENCODE_AES) {
        uint8_t* section_data = head->get_section_data();
        uint32_t section_size = head->get_section_size();
        if (section_data && section_size == align_val(section_size, 0x20)) {
            aes256_ctx aes;
            aes256_init_ctx(&aes, savedata_key);
            aes256_ecb_encrypt_buffer(&aes, section_data, section_size);
            head->attrib.set_aes(true);
        }
    }

    if (flags & X_SAVE_ENCODE_XOR)
        head->apply_xor();

    if (flags & X_SAVE_ENCODE_CRC)
        head->calc_crc();
    return true;
}
