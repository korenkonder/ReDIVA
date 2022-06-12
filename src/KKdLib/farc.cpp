/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "farc.hpp"
#include "io/path.hpp"
#include "aes.hpp"
#include "deflate.hpp"
#include "hash.hpp"
#include "str_utils.hpp"

static const uint8_t key[] = {
    0x70, 0x72, 0x6F, 0x6A, 0x65, 0x63, 0x74, 0x5F,
    0x64, 0x69, 0x76, 0x61, 0x2E, 0x62, 0x69, 0x6E,
};

static const uint8_t key_ft[] = {
    0x13, 0x72, 0xD5, 0x7B, 0x6E, 0x9E, 0x31, 0xEB,
    0xA2, 0x39, 0xB8, 0x3C, 0x15, 0x57, 0xC6, 0xBB,
};

static errno_t farc_get_files(farc* f);
static void farc_pack_files(farc* f, stream& s, farc_compress_mode mode, bool get_files);
static errno_t farc_read_header(farc* f, stream& s);
static void farc_unpack_files(farc* f, stream& s, bool save);
static void farc_unpack_file(farc* f, stream& s, farc_file* ff);

farc::farc() : flags(), ft() {
    signature = FARC_FArC;
    compression_level = 12;
}

farc::~farc() {

}

farc_file::farc_file() : offset(), size(), size_compressed(),
data(), data_compressed(), flags(), data_changed() {

}

farc_file::~farc_file() {
    free(data);
    free(data_compressed);
}

void farc::add_file(const char* name) {
    size_t files_count = files.size();
    void** data_temp = force_malloc_s(void*, files_count * 2);
    for (size_t i = 0; i < files_count; i++) {
        data_temp[i * 2 + 0] = files[i].data;
        data_temp[i * 2 + 1] = files[i].data_compressed;
        files[i].data = 0;
        files[i].data_compressed = 0;
    }

    files.push_back({});
    if (name)
        files.back().name = name;

    for (size_t i = 0; i < files_count; i++) {
        files[i].data = data_temp[i * 2 + 0];
        files[i].data_compressed = data_temp[i * 2 + 1];
        data_temp[i * 2 + 0] = 0;
        data_temp[i * 2 + 1] = 0;
    }
    free(data_temp);
}

void farc::add_file(const wchar_t* name) {
    size_t files_count = files.size();
    void** data_temp = force_malloc_s(void*, files_count * 2);
    for (size_t i = 0; i < files_count; i++) {
        data_temp[i * 2 + 0] = files[i].data;
        data_temp[i * 2 + 1] = files[i].data_compressed;
        files[i].data = 0;
        files[i].data_compressed = 0;
    }

    files.push_back({});
    if (name) {
        char* name_temp = utf16_to_utf8(name);
        files.back().name = name_temp;
        free(name_temp);
    }

    for (size_t i = 0; i < files_count; i++) {
        files[i].data = data_temp[i * 2 + 0];
        files[i].data_compressed = data_temp[i * 2 + 1];
        data_temp[i * 2 + 0] = 0;
        data_temp[i * 2 + 1] = 0;
    }
    free(data_temp);
}

size_t farc::get_file_size(const char* name) {
    if (!name)
        return 0;

    for (farc_file& i : files)
        if (!str_utils_compare(i.name.c_str(), name))
            return i.size;
    return 0;
}

size_t farc::get_file_size(const wchar_t* name) {
    if (!name)
        return 0;

    char* name_temp = utf16_to_utf8(name);
    for (farc_file& i : files)
        if (!str_utils_compare(i.name.c_str(), name_temp)) {
            free(name_temp);
            return i.size;
        }
    free(name_temp);
    return 0;
}

void farc::read(const char* path, bool unpack, bool save) {
    if (!path)
        return;

    wchar_t* path_buf = utf8_to_utf16(path);
    read(path_buf, unpack, save);
    free(path_buf);
}

void farc::read(const wchar_t* path, bool unpack, bool save) {
    if (!path)
        return;

    files.clear();
    files.shrink_to_fit();

    wchar_t full_path_buf[MAX_PATH];
    wchar_t* full_path = _wfullpath(full_path_buf, path, MAX_PATH);

    if (!full_path)
        return;
    else if (!path_check_file_exists(full_path_buf))
        return;

    char* dir_temp = utf16_to_utf8(full_path_buf);
    size_t dir_temp_len = utf8_length(dir_temp);
    file_path = std::string(dir_temp, dir_temp_len);
    directory_path = std::string(dir_temp, dir_temp_len);
    free(dir_temp);

    const char* dot = strrchr(directory_path.c_str(), '.');
    if (dot)
        directory_path = directory_path.substr(0, dot - directory_path.c_str());

    stream s;
    s.open(file_path.c_str(), "rb");
    if (s.io.stream && !farc_read_header(this, s) && unpack)
        farc_unpack_files(this, s, save);
}

void farc::read(const void* data, size_t size, bool unpack) {
    if (!data || !size)
        return;

    files.clear();
    files.shrink_to_fit();

    file_path = {};
    directory_path = {};

    stream s;
    s.open(data, size);
    if (!farc_read_header(this, s) && unpack)
        farc_unpack_files(this, s, false);
}

farc_file* farc::read_file(const char* name) {
    if (!name)
        return 0;

    uint32_t name_hash = hash_utf8_murmurhash(name);
    for (farc_file& i : files) {
        if (hash_string_murmurhash(&i.name) != name_hash)
            continue;

        if (!i.data && i.data_compressed)
            deflate::decompress(i.data_compressed, i.size_compressed,
                &i.data, &i.size, deflate::MODE_GZIP);
        else if (!i.data) {
            stream s;
            s.open(file_path.c_str(), "rb");
            if (s.io.stream)
                farc_unpack_file(this, s, &i);
        }
        return &i;
    }
    return 0;
}

farc_file* farc::read_file(const wchar_t* name) {
    if (!name)
        return 0;

    uint32_t name_hash = hash_utf16_murmurhash(name);
    for (farc_file& i : files) {
        if (hash_string_murmurhash(&i.name) != name_hash)
            continue;

        if (!i.data && i.data_compressed)
            deflate::decompress(i.data_compressed, i.size_compressed,
                &i.data, &i.size, deflate::MODE_GZIP);
        else if (!i.data) {
            stream s;
            s.open(file_path.c_str(), "rb");
            if (s.io.stream)
                farc_unpack_file(this, s, &i);
        }
        return &i;
    }
    return 0;
}

farc_file* farc::read_file(uint32_t hash) {
    if (!hash || hash == hash_murmurhash_empty)
        return 0;

    for (farc_file& i : files) {
        const char* l_str = i.name.c_str();
        const char* t = strrchr(l_str, '.');
        size_t l_len = i.name.size();
        if (t)
            l_len = t - l_str;

        if (hash_murmurhash(l_str, l_len) != hash)
            continue;

        if (!i.data && i.data_compressed)
            deflate::decompress(i.data_compressed, i.size_compressed,
                &i.data, &i.size, deflate::MODE_GZIP);
        else if (!i.data) {
            stream s;
            s.open(file_path.c_str(), "rb");
            if (s.io.stream)
                farc_unpack_file(this, s, &i);
        }
        return &i;
    }
    return 0;
}

void farc::write(const char* path, farc_compress_mode mode, bool get_files) {
    if (!path)
        return;

    wchar_t* path_buf = utf8_to_utf16(path);
    write(path_buf, mode, get_files);
    free(path_buf);
}

void farc::write(const wchar_t* path, farc_compress_mode mode, bool get_files) {
    if (!path)
        return;

    if (get_files) {
        files.clear();
        files.shrink_to_fit();
    }

    wchar_t full_path_buf[MAX_PATH];
    wchar_t* full_path = _wfullpath(full_path_buf, path, MAX_PATH);

    if (!full_path)
        return;
    else if (get_files && !path_check_directory_exists(full_path_buf))
        return;

    char* dir_temp = utf16_to_utf8(full_path_buf);
    size_t dir_temp_len = utf8_length(dir_temp);
    directory_path = std::string(dir_temp, dir_temp_len);
    file_path = std::string(dir_temp, dir_temp_len);
    file_path += ".farc", 6;
    free(dir_temp);

    if (!get_files || (get_files && !farc_get_files(this))) {
        stream s;
        s.open(file_path.c_str(), "wb");
        if (s.io.stream)
            farc_pack_files(this, s, mode, get_files);
    }
}

void farc::write(void** data, size_t* size, farc_compress_mode mode) {
    if (!data || !size)
        return;

    directory_path = {};
    file_path = {};

    stream s;
    s.open();
    farc_pack_files(this, s, mode, false);
}

bool farc::load_file(void* data, const char* path, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);
    if (file_len >= 5 && memcmp(&file[file_len - 5], ".farc", 6))
        return false;

    size_t path_len = utf8_length(path);
    if (path_len + file_len + 2 > 0x1000)
        return false;

    char buf[0x1000];
    memcpy(buf, path, path_len);
    memcpy(buf + path_len, file, file_len + 1);
    if (!path_check_file_exists(buf))
        return false;

    farc* f = (farc*)data;
    f->read(buf, true, false);
    return !!f->files.size();
}

static errno_t farc_get_files(farc* f) {
    f->files.clear();
    f->files.shrink_to_fit();

    std::vector<std::string> files;
    path_get_files(&files, f->directory_path.c_str());
    if (files.size() < 1)
        return -1;

    f->files = std::vector<farc_file>(files.size());
    for (farc_file& i : f->files)
        i.name = files[&i - f->files.data()];
    return 0;
}

static void farc_pack_files(farc* f, stream& s, farc_compress_mode mode, bool get_files) {
    bool plain = false;
    for (farc_file& i : f->files) {
        char* ext = str_utils_get_extension(i.name.c_str());
        bool is_a3da = str_utils_compare(ext, ".a3da") == 0;
        bool is_diva = str_utils_compare(ext, ".diva") == 0;
        bool is_vag = str_utils_compare(ext, ".vag") == 0;
        free(ext);

        if (is_a3da || is_diva || is_vag) {
            plain = true;
            break;
        }
    }

    if (plain)
        mode = FARC_COMPRESS_FArc;

    switch (mode) {
    case FARC_COMPRESS_FARC_GZIP:
        f->flags = FARC_GZIP;
        break;
    case FARC_COMPRESS_FARC_AES:
        f->flags = FARC_AES;
        break;
    case FARC_COMPRESS_FARC_GZIP_AES:
        f->flags = (farc_flags)(FARC_GZIP | FARC_AES);
        break;
    default:
        f->flags = (farc_flags)0;
        break;
    }

    size_t header_length = 0;
    switch (mode) {
    case FARC_COMPRESS_FARC:
    case FARC_COMPRESS_FARC_GZIP:
    case FARC_COMPRESS_FARC_AES:
    case FARC_COMPRESS_FARC_GZIP_AES:
        header_length += sizeof(int32_t) * 5;
        break;
    default:
        header_length += sizeof(int32_t);
        break;
    }

    if (mode == FARC_COMPRESS_FArc)
        for (farc_file& i : f->files) {
            header_length += i.name.size() + 1;
            header_length += sizeof(int32_t) * 2;
        }
    else
        for (farc_file& i : f->files) {
            header_length += i.name.size() + 1;
            header_length += sizeof(int32_t) * 3;
        }

    size_t align = header_length + 8;
    s.set_position(align_val(align, 0x10), SEEK_SET);
    size_t dir_len = f->directory_path.size();

    aes128_ctx ctx;
    if (mode == FARC_COMPRESS_FARC_AES || mode == FARC_COMPRESS_FARC_GZIP_AES)
        aes128_init_ctx(&ctx, key);

    f->compression_level = clamp(f->compression_level, 0, 12);
    if (get_files) {
        char* temp = force_malloc_s(char, dir_len + 2 + MAX_PATH);
        memcpy(temp, f->directory_path.c_str(), sizeof(char) * dir_len);
        temp[dir_len] = '\\';
        for (farc_file& i : f->files) {
            if (i.name.size()) {
                memcpy(temp + dir_len + 1, i.name.c_str(), sizeof(char) * i.name.size());
                temp[dir_len + 1 + i.name.size()] = '\0';
            }

            i.offset = s.get_position();

            stream s_t;
            s_t.open(temp, "rb");
            if (!s_t.io.stream) {
                continue;
            }

            s_t.set_position(0, SEEK_END);
            size_t file_len = s_t.get_position();
            s_t.set_position(0, SEEK_SET);

            i.size = file_len;

            void* t = force_malloc(file_len);
            s_t.read(t, file_len);

            if (mode == FARC_COMPRESS_FARC_AES || mode == FARC_COMPRESS_FARC_GZIP_AES) {
                void* t1;
                size_t t1_len;
                if (mode == FARC_COMPRESS_FARC_GZIP_AES) {
                    deflate::compress(t, file_len, &t1, &t1_len,
                        f->compression_level, deflate::MODE_GZIP);
                    free(t);
                }
                else {
                    t1 = t;
                    t1_len = file_len;
                }

                size_t t2_len = align_val(t1_len, 0x10);
                size_t a = t2_len - t1_len;
                void* t2 = force_malloc(t2_len);
                memcpy(t2, t1, t1_len);
                free(t1);

                for (size_t j = 0; j < a; j++)
                    ((uint8_t*)t2)[t1_len + j] = 0x78;

                aes128_ecb_encrypt_buffer(&ctx, (uint8_t*)t2, t2_len);

                s.write(t2, t2_len);
                i.size_compressed = t1_len;
                t = t2;
            }
            else if (mode == FARC_COMPRESS_FArC || mode == FARC_COMPRESS_FARC_GZIP) {
                void* t1;
                size_t t1_len;
                deflate::compress(t, file_len, &t1, &t1_len,
                    f->compression_level, deflate::MODE_GZIP);
                free(t);
                s.write(t1, t1_len);
                size_t a = align_val(t1_len, 0x10) - t1_len;
                for (size_t j = 0; j < a; j++)
                    s.write_uint8_t(0x78);
                i.size_compressed = t1_len;
                t = t1;
            }
            else {
                s.write(t, file_len);
                size_t a = align_val(file_len, 0x10) - file_len;
                for (size_t j = 0; j < a; j++)
                    s.write_uint8_t(0x00);
            }
            free(t);
            i.data = 0;
            i.data_changed = false;
        }
        free(temp);
    }
    else
        for (farc_file& i : f->files) {
            i.offset = s.get_position();
            size_t file_len = i.size;

            if (mode == FARC_COMPRESS_FARC_AES || mode == FARC_COMPRESS_FARC_GZIP_AES) {
                void* t1;
                size_t t1_len;
                if (mode == FARC_COMPRESS_FARC_GZIP_AES) {
                    if (!i.data_compressed || i.data_changed) {
                        free(i.data_compressed);
                        deflate::compress(i.data, file_len, &i.data_compressed,
                            &i.size_compressed, f->compression_level, deflate::MODE_GZIP);
                    }
                    t1 = i.data_compressed;
                    t1_len = i.size_compressed;
                }
                else {
                    free(i.data_compressed);
                    t1 = i.data;
                    t1_len = i.size;
                }

                size_t t2_len = align_val(t1_len, 0x10);
                size_t a = t2_len - t1_len;
                void* t2 = force_malloc(t2_len);
                memcpy(t2, t1, t1_len);

                for (size_t j = 0; j < a; j++)
                    ((uint8_t*)t2)[t1_len + j] = 0x78;

                aes128_ecb_encrypt_buffer(&ctx, (uint8_t*)t2, t2_len);

                s.write(t2, t2_len);
                free(t2);
            }
            else if (mode == FARC_COMPRESS_FArC || mode == FARC_COMPRESS_FARC_GZIP) {
                if (!i.data_compressed || i.data_changed) {
                    free(i.data_compressed);
                    deflate::compress(i.data, file_len, &i.data_compressed,
                        &i.size_compressed, f->compression_level, deflate::MODE_GZIP);
                }
                s.write(i.data_compressed, i.size_compressed);
                size_t a = align_val(i.size_compressed, 0x10) - i.size_compressed;
                for (size_t j = 0; j < a; j++)
                    s.write_uint8_t(0x78);
            }
            else {
                free(i.data_compressed);
                s.write(i.data, file_len);
                size_t a = align_val(file_len, 0x10) - file_len;
                for (size_t j = 0; j < a; j++)
                    s.write_uint8_t(0x00);
            }
            i.data_changed = false;
        }

    s.set_position(0, SEEK_SET);
    switch (mode) {
    case FARC_COMPRESS_FArc:
    default:
        s.write_int32_t_reverse_endianness(FARC_FArc, true);
        s.write_int32_t_reverse_endianness((int32_t)header_length, true);
        s.write_int32_t_reverse_endianness(0x01, true);
        break;
    case FARC_COMPRESS_FArC:
        s.write_int32_t_reverse_endianness(FARC_FArC, true);
        s.write_int32_t_reverse_endianness((int32_t)header_length, true);
        s.write_int32_t_reverse_endianness(0x01, true);
        break;
    case FARC_COMPRESS_FARC:
    case FARC_COMPRESS_FARC_GZIP:
    case FARC_COMPRESS_FARC_AES:
    case FARC_COMPRESS_FARC_GZIP_AES:
        s.write_int32_t_reverse_endianness(FARC_FARC, true);
        s.write_int32_t_reverse_endianness((int32_t)header_length, true);
        s.write_int32_t_reverse_endianness(f->flags, true);
        s.write_int32_t_reverse_endianness(0x00, true);
        s.write_int32_t_reverse_endianness(0x40, true);
        s.write_int32_t_reverse_endianness(0x00, true);
        s.write_int32_t_reverse_endianness(0x00, true);
        break;
    }

    if (mode == FARC_COMPRESS_FArc)
        for (farc_file& i : f->files) {
            s.write_string_null_terminated(i.name);
            s.write_int32_t_reverse_endianness((int32_t)i.offset, true);
            s.write_int32_t_reverse_endianness((int32_t)i.size, true);
        }
    else
        for (farc_file& i : f->files) {
            s.write_string_null_terminated(i.name);
            s.write_int32_t_reverse_endianness((int32_t)i.offset, true);
            s.write_int32_t_reverse_endianness((int32_t)i.size_compressed, true);
            s.write_int32_t_reverse_endianness((int32_t)i.size, true);
        }

    align = align_val(align, 0x10) - align;
    if (mode == FARC_COMPRESS_FArc) {
        uint8_t padding[] = {
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        };
        s.write(padding, align);
    }
    else {
        uint8_t padding[] = {
            0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78,
            0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78,
        };
        s.write(padding, align);
    }
}

static errno_t farc_read_header(farc* f, stream& s) {
    if (!f || s.io.check_null())
        return -1;

    s.set_position(0, SEEK_SET);
    f->signature = (farc_signature)s.read_int32_t_reverse_endianness(true);
    if (f->signature != FARC_FArc && f->signature != FARC_FArC && f->signature != FARC_FARC)
        return -2;

    f->ft = false;

    int32_t header_length = s.read_int32_t_reverse_endianness(true);
    if (f->signature == FARC_FARC) {
        f->flags = (farc_flags)s.read_int32_t_reverse_endianness(true);
        s.read_int32_t();
        int32_t farc_mode = s.read_int32_t_reverse_endianness(true);

        f->ft = (f->flags & FARC_AES) && (farc_mode & (farc_mode - 1));
        if (f->ft) {
            header_length -= 0x08;
            s.set_position(-0x04, SEEK_CUR);
        }
        else
            header_length -= 0x0C;
    }

    f->files.clear();
    f->files.shrink_to_fit();

    uint8_t* d_t;
    uint8_t* dt;
    int32_t length = 0;

    dt = d_t = force_malloc_s(uint8_t, header_length);
    s.read(d_t, header_length);
    if (f->ft) {
        aes128_ctx ctx;
        aes128_init_ctx_iv(&ctx, key_ft, dt);
        dt += 0x10;
        aes128_cbc_decrypt_buffer(&ctx, dt, (size_t)header_length - 0x10);
        dt += sizeof(int32_t);
    }

    bool has_per_file_flags = false;
    if (f->signature == FARC_FARC) {
        if (load_reverse_endianness_int32_t((void*)dt) == 1) {
            dt += sizeof(int32_t);
            length = load_reverse_endianness_int32_t((void*)dt);
            has_per_file_flags = true;
        }
        dt += sizeof(int32_t);
    }
    dt += sizeof(int32_t);

    if (length == 0) {
        size_t count = 0;
        uint8_t* position = dt;
        size_t size;
        if (has_per_file_flags)
            size = sizeof(int32_t) * 4;
        else if (f->signature != FARC_FArc)
            size = sizeof(int32_t) * 3;
        else
            size = sizeof(int32_t) * 2;

        while (dt - d_t < header_length) {
            while (*dt++);
            dt += size;
            count++;
        }
        dt = position;
        length = (int32_t)count;
    }

    f->files = std::vector<farc_file>(length);
    if (has_per_file_flags)
        for (farc_file& i : f->files) {
            i.name = (char*)dt;
            dt += i.name.size() + 1;
            i.offset = (size_t)load_reverse_endianness_int32_t((void*)dt);
            i.size_compressed = (size_t)load_reverse_endianness_int32_t((void*)(dt + 4));
            i.size = (size_t)load_reverse_endianness_int32_t((void*)(dt + 8));
            i.flags = (farc_flags)load_reverse_endianness_int32_t((void*)(dt + 12));
            dt += sizeof(int32_t) * 4;
        }
    else if (f->signature != FARC_FArc)
        for (farc_file& i : f->files) {
            i.name = (char*)dt;
            dt += i.name.size() + 1;
            i.offset = (size_t)load_reverse_endianness_int32_t((void*)dt);
            i.size_compressed = (size_t)load_reverse_endianness_int32_t((void*)(dt + 4));
            i.size = (size_t)load_reverse_endianness_int32_t((void*)(dt + 8));
            dt += sizeof(int32_t) * 3;
        }
    else
        for (farc_file& i : f->files) {
            i.name = (char*)dt;
            dt += i.name.size() + 1;
            i.offset = (size_t)load_reverse_endianness_int32_t((void*)dt);
            i.size = (size_t)load_reverse_endianness_int32_t((void*)(dt + 4));
            dt += sizeof(int32_t) * 2;
        }
    free(d_t);
    return 0;
}

static void farc_unpack_files(farc* f, stream& s, bool save) {
    if (!f || s.io.check_null())
        return;

    for (farc_file& i : f->files)
        farc_unpack_file(f, s, &i);

    if (!save)
        return;

    size_t max_path_len = 0;
    size_t dir_len = f->directory_path.size();
    for (farc_file& i : f->files) {
        size_t path_len = dir_len + 1 + utf8_to_utf16_length(i.name.c_str());
        if (max_path_len < path_len)
            max_path_len = path_len;
    }

    wchar_t* dir_temp = utf8_to_utf16(f->directory_path.c_str());
    CreateDirectoryW(dir_temp, 0);
    free(dir_temp);

    char* temp_path = force_malloc_s(char, max_path_len + 1);
    memcpy(temp_path, f->directory_path.c_str(), sizeof(char) * dir_len);
    temp_path[dir_len] = '\\';

    for (farc_file& i : f->files) {
        if (i.name.c_str()) {
            memcpy(temp_path + dir_len + 1, i.name.c_str(), sizeof(wchar_t) * i.name.size());
            temp_path[dir_len + 1 + i.name.size()] = '\0';
        }

        if (i.data) {
            stream temp_s;
            temp_s.open(temp_path, "wb");
            if (temp_s.io.stream)
                temp_s.write(i.data, i.size);
            free(i.data);
        }
    }
    free(temp_path);
}

static void farc_unpack_file(farc* f, stream& s, farc_file* ff) {
    if (!f || s.io.check_null())
        return;

    if (ff->data)
        free(ff->data);

    s.set_position(ff->offset, SEEK_SET);

    if (f->signature != FARC_FARC) {
        if (f->signature != FARC_FArC) {
            ff->data_compressed = 0;
            ff->data = force_malloc(ff->size);
            s.read(ff->data, ff->size);
        }
        else if (ff->data_compressed)
            deflate::decompress(ff->data_compressed, ff->size_compressed,
                &ff->data, &ff->size, deflate::MODE_GZIP);
        else {
            ff->data_compressed = force_malloc(ff->size_compressed);
            s.read(ff->data_compressed, ff->size_compressed);
            deflate::decompress(ff->data_compressed, ff->size_compressed,
                &ff->data, &ff->size, deflate::MODE_GZIP);
        }
    }
    else {
        bool aes = (f->flags | ff->flags) & FARC_AES;
        bool gzip = (f->flags | ff->flags) & FARC_GZIP;
        if (!aes && !gzip) {
            ff->data_compressed = 0;
            ff->data = force_malloc(ff->size);
            s.read(ff->data, ff->size);
        }
        else if (ff->data_compressed)
            deflate::decompress(ff->data_compressed, ff->size_compressed,
                &ff->data, &ff->size, deflate::MODE_GZIP);
        else {
            size_t temp_s = aes ? align_val(ff->size_compressed, 0x10) : ff->size_compressed;
            void* temp = force_malloc(temp_s);
            s.read(temp, temp_s);

            void* t = temp;
            if (aes)
                if (f->ft) {
                    ff->size_compressed -= 0x10;
                    temp_s -= 0x10;
                    t = (void*)((uint64_t)t + 0x10);

                    aes128_ctx ctx;
                    aes128_init_ctx_iv(&ctx, key_ft, (uint8_t*)temp);
                    aes128_cbc_decrypt_buffer(&ctx, (uint8_t*)t, temp_s);
                }
                else {
                    aes128_ctx ctx;
                    aes128_init_ctx(&ctx, key);
                    aes128_ecb_decrypt_buffer(&ctx, (uint8_t*)t, temp_s);
                }

            if (!gzip) {
                ff->data_compressed = 0;
                ff->data = force_malloc(ff->size);
                memcpy(ff->data, t, ff->size);
            }
            else {
                ff->data_compressed = force_malloc(ff->size_compressed);
                memcpy(ff->data_compressed, t, ff->size_compressed);
                deflate::decompress(ff->data_compressed, ff->size_compressed,
                    &ff->data, &ff->size, deflate::MODE_GZIP);
            }
            free(temp);
        }
    }
    ff->data_changed = false;
}
