/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "farc.h"
#include "aes.h"
#include "io_deflate.h"
#include "io_path.h"
#include "str_utils.h"
#include "utf8.h"

static const char key[] = {
    0x70, 0x72, 0x6F, 0x6A, 0x65, 0x63, 0x74, 0x5F,
    0x64, 0x69, 0x76, 0x61, 0x2E, 0x62, 0x69, 0x6E,
};

static const char keyFT[] = {
    0x13, 0x72, 0xD5, 0x7B, 0x6E, 0x9E, 0x31, 0xEB,
    0xA2, 0x39, 0xB8, 0x3C, 0x15, 0x57, 0xC6, 0xBB,
};

vector_func(farc_file)

static errno_t farc_get_files(farc* f);
static void farc_pack_files(farc* f, stream* s, farc_compress_mode mode, bool get_files);
static errno_t farc_read_header(farc* f, stream* s);
static void farc_unpack_files(farc* f, stream* s, bool save);
static void farc_unpack_file(farc* f, stream* s, farc_file* ff);

farc* farc_init() {
    farc* f = force_malloc(sizeof(farc));
    return f;
}

void farc_read(farc* f, char* path, bool unpack, bool save) {
    if (!f || !path)
        return;

    wchar_t* path_buf = char_string_to_wchar_t_string(path);
    farc_wread(f, path_buf, unpack, save);
    free(path_buf);
}

void farc_wread(farc* f, wchar_t* path, bool unpack, bool save) {
    if (!f || !path)
        return;

    for (farc_file* i = f->files.begin; i != f->files.end; i++) {
        free(i->name);
        free(i->data);
    }

    vector_farc_file_free(&f->files);
    memset(f, 0, sizeof(farc));

    wchar_t full_path_buf[MAX_PATH];
    wchar_t* full_path = _wfullpath(full_path_buf, path, MAX_PATH);

    if (!full_path)
        return;
    else if (!path_wcheck_file_exists(full_path_buf))
        return;

    size_t full_path_buf_len = wcslen(full_path_buf);
    wcsncpy_s(f->file_path, MAX_PATH, full_path_buf, full_path_buf_len + 1);
    wcsncpy_s(f->directory_path, MAX_PATH, full_path_buf, full_path_buf_len + 1);
    wchar_t* dot = wcsrchr(f->directory_path, L'.');
    if (dot)
        memset(dot, 0, wcslen(dot) * 2 + 2);

    stream* s = io_wopen(f->file_path, L"rb");
    if (s->io.stream && !farc_read_header(f, s) && unpack)
        farc_unpack_files(f, s, save);
    io_dispose(s);
}

farc_file* farc_read_file(farc* f, char* name) {
    if (!f || !name)
        return 0;

    wchar_t* name_buf = char_string_to_wchar_t_string(name);
    farc_file* ff = farc_wread_file(f, name_buf);
    free(name_buf);
    return ff;
}

farc_file* farc_wread_file(farc* f, wchar_t* name) {
    if (!f || !name)
        return 0;

    for (farc_file* i = f->files.begin; i != f->files.end; i++)
        if (!wcscmp(i->name, name))
            return i;
    return 0;
}

void farc_write(farc* f, char* path, farc_compress_mode mode, bool get_files) {
    if (!f || !path)
        return;

    wchar_t* path_buf = char_string_to_wchar_t_string(path);
    farc_wwrite(f, path_buf, mode, get_files);
    free(path_buf);
}

void farc_wwrite(farc* f, wchar_t* path, farc_compress_mode mode, bool get_files) {
    if (!f || !path)
        return;

    if (get_files) {
        for (farc_file* i = f->files.begin; i != f->files.end; i++) {
            free(i->name);
            free(i->data);
        }

        vector_farc_file_free(&f->files);
        memset(f, 0, sizeof(farc));
    }

    wchar_t full_path_buf[MAX_PATH];
    wchar_t* full_path = _wfullpath(full_path_buf, path, MAX_PATH);

    if (!full_path)
        return;
    else if (get_files && !path_wcheck_directory_exists(full_path_buf))
        return;

    size_t full_path_buf_len = wcslen(full_path_buf);
    wcsncpy_s(f->directory_path, MAX_PATH, full_path_buf, full_path_buf_len + 1);
    wcsncpy_s(f->file_path, MAX_PATH, full_path_buf, full_path_buf_len + 1);
    wcsncat_s(f->file_path, MAX_PATH, L".farc", 6);

    if (!get_files || (get_files && !farc_get_files(f))) {
        stream* s = io_wopen(f->file_path, L"wb");
        if (s->io.stream)
            farc_pack_files(f, s, mode, get_files);
        io_dispose(s);
    }
}

void farc_dispose(farc* f) {
    if (!f)
        return;

    for (farc_file* i = f->files.begin; i != f->files.end; i++) {
        free(i->name);
        free(i->data);
    }

    vector_farc_file_free(&f->files);
    free(f);
}

static errno_t farc_get_files(farc* f) {
    for (farc_file* i = f->files.begin; i != f->files.end; i++) {
        free(i->data);
        free(i->name);
    }

    vector_farc_file_free(&f->files);

    vector_ptr_wchar_t files = (vector_ptr_wchar_t){ 0, 0, 0 };
    path_wget_files(&files, f->directory_path);
    if (files.end - files.begin < 1) {
        vector_ptr_wchar_t_free(&files, 0);
        return -1;
    }

    vector_farc_file_append(&f->files, files.end - files.begin);
    f->files.end = &f->files.begin[files.end - files.begin];
     for (farc_file* i = f->files.begin; i != f->files.end; i++) {
         memset(i, 0, sizeof(farc_file));
         size_t len = wcslen(files.begin[i - f->files.begin]);
         i->name = force_malloc_s(wchar_t, len + 1);
         memcpy(i->name, files.begin[i - f->files.begin], sizeof(wchar_t) * (len + 1));
    }
    vector_ptr_wchar_t_free(&files, 0);
    return 0;
}

static void farc_pack_files(farc* f, stream* s, farc_compress_mode mode, bool get_files) {
    bool plain = false;
    for (farc_file* i = f->files.begin; i != f->files.end; i++) {
        wchar_t* ext = str_utils_wget_extension(i->name);
        bool is_a3da = wcscmp(ext, L".a3da") == 0;
        bool is_diva = wcscmp(ext, L".diva") == 0;
        bool is_farc = wcscmp(ext, L".farc") == 0;
        bool is_vag = wcscmp(ext, L".vag") == 0;
        free(ext);

        if (is_a3da || is_diva || is_farc || is_vag) {
            plain = true;
            break;
        }
    }

    if (plain)
        mode = FARC_COMPRESS_FArc;

    switch (mode) {
    case FARC_COMPRESS_FARC_GZIP:
        f->type = FARC_DATA_GZIP;
        break;
    case FARC_COMPRESS_FARC_AES:
        f->type = FARC_DATA_AES;
        break;
    case FARC_COMPRESS_FARC_GZIP_AES:
        f->type = FARC_DATA_GZIP | FARC_DATA_AES;
        break;
    default:
        f->type = 0;
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

    char** files_utf8 = force_malloc_s(char*, f->files.end - f->files.begin);
    if (mode == FARC_COMPRESS_FArc)
        for (farc_file* i = f->files.begin; i != f->files.end; i++) {
            files_utf8[i - f->files.begin] = utf8_encode(i->name);
            header_length += strlen(files_utf8[i - f->files.begin]) + 1;
            header_length += sizeof(int32_t) * 2;
        }
    else
        for (farc_file* i = f->files.begin; i != f->files.end; i++) {
            files_utf8[i - f->files.begin] = utf8_encode(i->name);
            header_length += strlen(files_utf8[i - f->files.begin]) + 1;
            header_length += sizeof(int32_t) * 3;
        }

    size_t align = header_length + 8;
    io_set_position(s, align_val(align, 0x10), IO_SEEK_SET);
    size_t dir_len = wcslen(f->directory_path);

    struct aes_ctx ctx;
    if (mode == FARC_COMPRESS_FARC_AES || mode == FARC_COMPRESS_FARC_GZIP_AES)
        aes_init_ctx(&ctx, key);

    if (get_files) {
        wchar_t* temp = force_malloc_s(wchar_t, dir_len + 2 + MAX_PATH);
        memcpy(temp, f->directory_path, sizeof(wchar_t) * dir_len);
        temp[dir_len] = L'\\';
        for (farc_file* i = f->files.begin; i != f->files.end; i++) {
            memcpy(temp + dir_len + 1, i->name, sizeof(wchar_t) * (wcslen(i->name) + 1));
            i->offset = io_get_position(s);

            stream* s_t = io_wopen(temp, L"rb");
            if (!s_t->io.stream) {
                io_dispose(s_t);
                continue;
            }

            io_set_position(s_t, 0, IO_SEEK_END);
            size_t file_len = io_get_position(s_t);
            io_set_position(s_t, 0, IO_SEEK_SET);

            i->size = file_len;

            void* t = force_malloc(file_len);
            io_read(s_t, t, file_len);

            if (mode == FARC_COMPRESS_FARC_AES || mode == FARC_COMPRESS_FARC_GZIP_AES) {
                void* t1;
                size_t t1_len;
                if (mode == FARC_COMPRESS_FARC_GZIP_AES) {
                    deflate_compress(t, file_len, &t1, &t1_len, 9, DEFLATE_MODE_GZIP);
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

                aes_ecb_encrypt_buffer(&ctx, t2, t2_len);

                io_write(s, t2, t2_len);
                i->size_compressed = t1_len;
                t = t2;
            }
            else if (mode == FARC_COMPRESS_FArC || mode == FARC_COMPRESS_FARC_GZIP) {
                void* t1;
                size_t t1_len;
                deflate_compress(t, file_len, &t1, &t1_len, 12, DEFLATE_MODE_GZIP);
                free(t);
                io_write(s, t1, t1_len);
                size_t a = align_val(t1_len, 0x10) - t1_len;
                for (size_t j = 0; j < a; j++)
                    io_write_uint8_t(s, 0x78);
                i->size_compressed = t1_len;
                t = t1;
            }
            else {
                io_write(s, t, file_len);
                size_t a = align_val(file_len, 0x10) - file_len;
                for (size_t j = 0; j < a; j++)
                    io_write_uint8_t(s, 0x00);
            }
            free(t);
            io_dispose(s_t);
        }
        free(temp);
    }
    else
        for (farc_file* i = f->files.begin; i != f->files.end; i++) {
            i->offset = io_get_position(s);
            size_t file_len = i->size;

            void* t = i->data;
            if (mode == FARC_COMPRESS_FARC_AES || mode == FARC_COMPRESS_FARC_GZIP_AES) {
                void* t1;
                size_t t1_len;
                if (mode == FARC_COMPRESS_FARC_GZIP_AES) {
                    deflate_compress(t, file_len, &t1, &t1_len, 12, DEFLATE_MODE_GZIP);
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

                aes_ecb_encrypt_buffer(&ctx, t2, t2_len);

                io_write(s, t2, t2_len);
                i->size_compressed = t1_len;
                t = t2;
            }
            else if (mode == FARC_COMPRESS_FArC || mode == FARC_COMPRESS_FARC_GZIP) {
                void* t1;
                size_t t1_len;
                deflate_compress(t, file_len, &t1, &t1_len, 12, DEFLATE_MODE_GZIP);
                free(t);
                io_write(s, t1, t1_len);
                size_t a = align_val(t1_len, 0x10) - t1_len;
                for (size_t j = 0; j < a; j++)
                    io_write_uint8_t(s, 0x78);
                i->size_compressed = t1_len;
                t = t1;
            }
            else {
                io_write(s, t, file_len);
                size_t a = align_val(file_len, 0x10) - file_len;
                for (size_t j = 0; j < a; j++)
                    io_write_uint8_t(s, 0x00);
            }
            i->data = t;
        }

    io_set_position(s, 0, IO_SEEK_SET);
    switch (mode) {
    case FARC_COMPRESS_FARC:
    case FARC_COMPRESS_FARC_GZIP:
    case FARC_COMPRESS_FARC_AES:
    case FARC_COMPRESS_FARC_GZIP_AES:
        io_write_int32_t(s, FARC_FARC);
        io_write_int32_t_reverse_endianness(s, (int32_t)header_length, true);
        io_write_int32_t_reverse_endianness(s, f->type, true);
        io_write_int32_t_reverse_endianness(s, 0x00, true);
        io_write_int32_t_reverse_endianness(s, 0x40, true);
        io_write_int32_t_reverse_endianness(s, 0x00, true);
        io_write_int32_t_reverse_endianness(s, 0x00, true);
        break;
    case FARC_COMPRESS_FArC:
        io_write_int32_t(s, FARC_FArC);
        io_write_int32_t_reverse_endianness(s, (int32_t)header_length, true);
        io_write_int32_t_reverse_endianness(s, 0x01, true);
        break;
    default:
        io_write_int32_t(s, FARC_FArc);
        io_write_int32_t_reverse_endianness(s, (int32_t)header_length, true);
        io_write_int32_t_reverse_endianness(s, 0x01, true);
        break;
    }

    if (mode == FARC_COMPRESS_FArc)
        for (farc_file* i = f->files.begin; i != f->files.end; i++) {
            io_write(s, files_utf8[i - f->files.begin], strlen(files_utf8[i - f->files.begin]));
            io_write_uint8_t(s, 0x00);
            io_write_int32_t_reverse_endianness(s, (int32_t)i->offset, true);
            io_write_int32_t_reverse_endianness(s, (int32_t)i->size, true);
        }
    else
        for (farc_file* i = f->files.begin; i != f->files.end; i++) {
            io_write(s, files_utf8[i - f->files.begin], strlen(files_utf8[i - f->files.begin]));
            io_write_uint8_t(s, 0x00);
            io_write_int32_t_reverse_endianness(s, (int32_t)i->offset, true);
            io_write_int32_t_reverse_endianness(s, (int32_t)i->size_compressed, true);
            io_write_int32_t_reverse_endianness(s, (int32_t)i->size, true);
        }

    align = align_val(align, 0x10) - align;
    if (mode == FARC_COMPRESS_FArc)
        for (size_t i = 0; i < align; i++)
            io_write_uint8_t(s, 0x00);
    else
        for (size_t i = 0; i < align; i++)
            io_write_uint8_t(s, 0x78);

    for (farc_file* i = f->files.begin; i != f->files.end; i++)
        free(files_utf8[i - f->files.begin]);
    free(files_utf8);
}

static errno_t farc_read_header(farc* f, stream* s) {
    if (!f || !s)
        return -1;

    io_set_position(s, 0, IO_SEEK_SET);
    f->signature = io_read_int32_t(s);
    if (f->signature != FARC_FArc && f->signature != FARC_FArC && f->signature != FARC_FARC)
        return -2;

    f->ft = false;

    int32_t header_length = io_read_int32_t_reverse_endianness(s, true);
    if (f->signature == FARC_FARC) {
        f->type = (farc_type)io_read_int32_t_reverse_endianness(s, true);
        io_read_int32_t(s);
        int32_t farc_mode = io_read_int32_t_reverse_endianness(s, true);

        f->ft = (f->type & FARC_DATA_AES) && (farc_mode & (farc_mode - 1));
        if (f->ft)
            header_length -= 0x08;
        else
            header_length -= 0x0C;
    }

    for (farc_file* i = f->files.begin; i != f->files.end; i++) {
        free(i->data);
        free(i->name);
    }
    vector_farc_file_free(&f->files);

    uint8_t* d_t;
    uint8_t* dt;
    int32_t length = 0;

    dt = d_t = force_malloc(header_length);
    io_read(s, d_t, header_length);
    if (f->ft) {
        io_set_position(s, 0x10, IO_SEEK_SET);

        struct aes_ctx ctx;
        aes_init_ctx(&ctx, keyFT);
        aes_cbc_decrypt_buffer(&ctx, d_t, header_length);
        dt += 0x10 + sizeof(int32_t);
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
            while (*dt)
                dt++;
            dt++;
            dt += size;
            count++;
        }
        dt = position;
        length = (int32_t)count;
    }

    vector_farc_file_append(&f->files, length);
    f->files.end = &f->files.begin[length];

    char* file = force_malloc(BUF_SIZE);
    if (has_per_file_flags)
        for (farc_file* i = f->files.begin; i != f->files.end; i++) {
            size_t f_len = 0;
            while (*dt)
                file[f_len++] = *dt++;
            dt++;
            file[f_len] = 0;

            i->name = utf8_decode(file);
            i->offset = (size_t)load_reverse_endianness_int32_t((void*)dt);
            i->size_compressed = (size_t)load_reverse_endianness_int32_t((void*)(dt + 4));
            i->size = (size_t)load_reverse_endianness_int32_t((void*)(dt + 8));
            i->type = load_reverse_endianness_int32_t((void*)(dt + 12));
            dt += sizeof(int32_t) * 4;
        }
    else if (f->signature != FARC_FArc)
        for (farc_file* i = f->files.begin; i != f->files.end; i++) {
            size_t f_len = 0;
            while (*dt)
                file[f_len++] = *dt++;
            dt++;
            file[f_len] = 0;

            i->name = utf8_decode(file);
            i->offset = (size_t)load_reverse_endianness_int32_t((void*)dt);
            i->size_compressed = (size_t)load_reverse_endianness_int32_t((void*)(dt + 4));
            i->size = (size_t)load_reverse_endianness_int32_t((void*)(dt + 8));
            dt += sizeof(int32_t) * 3;
        }
    else
        for (farc_file* i = f->files.begin; i != f->files.end; i++) {
            size_t f_len = 0;
            while (*dt)
                file[f_len++] = *dt++;
            dt++;
            file[f_len] = 0;

            i->name = utf8_decode(file);
            i->offset = (size_t)load_reverse_endianness_int32_t((void*)dt);
            i->size = (size_t)load_reverse_endianness_int32_t((void*)(dt + 4));
            dt += sizeof(int32_t) * 2;
        }
    free(file);
    return 0;
}

static void farc_unpack_files(farc* f, stream* s, bool save) {
    if (!f || !s)
        return;

    if (!save) {
        for (farc_file* i = f->files.begin; i != f->files.end; i++)
            farc_unpack_file(f, s, i);
        return;
    }

    size_t max_path_len = 0;
    size_t dir_len = wcslen(f->directory_path);
    for (farc_file* i = f->files.begin; i != f->files.end; i++) {
        size_t path_len = dir_len + 1 + wcslen(i->name);
        if (max_path_len < path_len)
            max_path_len = path_len;
    }

    CreateDirectoryW(f->directory_path, 0);
    wchar_t* temp_path = force_malloc_s(wchar_t, max_path_len + 1);
    memcpy(temp_path, f->directory_path, sizeof(wchar_t) * dir_len);
    temp_path[dir_len] = L'\\';

    for (farc_file* i = f->files.begin; i != f->files.end; i++) {
        farc_unpack_file(f, s, i);

        size_t path_len = wcslen(i->name);
        memcpy(temp_path + dir_len + 1, i->name, sizeof(wchar_t) * path_len);
        temp_path[dir_len + 1 + path_len] = L'\0';

        if (i->data) {
            stream* temp_s = io_wopen(temp_path, L"wb");
            if (temp_s->io.stream)
                io_write(temp_s, i->data, i->size);
            io_dispose(temp_s);
            free(i->data);
        }
    }
    free(temp_path);
}

static void farc_unpack_file(farc* f, stream* s, farc_file* ff) {
    if (!f || !s)
        return;

    if (ff->data)
        free(ff->data);

    ff->data = force_malloc(ff->size);
    io_set_position(s, ff->offset, IO_SEEK_SET);

    if (f->signature != FARC_FARC) {
        if (f->signature == FARC_FArC) {
            void* temp_comp = force_malloc(ff->size_compressed);
            io_read(s, temp_comp, ff->size_compressed);
            deflate_decompress(temp_comp, ff->size_compressed,
                &ff->data, ff->size, DEFLATE_MODE_GZIP);
            free(temp_comp);
        }
        else
            io_read(s, ff->data, ff->size);
    }
    else {
        bool aes = (f->type | ff->type) & FARC_DATA_AES;
        bool gzip = (f->type | ff->type) & FARC_DATA_GZIP;
        if (aes | gzip) {
            size_t size_comp = aes ? align_val(ff->size_compressed, 0x10LL) : ff->size_compressed;
            void* temp_comp = force_malloc(size_comp);
            io_read(s, temp_comp, size_comp);

            if (aes) {
                struct aes_ctx ctx;
                if (f->ft) {
                    aes_init_ctx(&ctx, keyFT);
                    aes_cbc_decrypt_buffer(&ctx, temp_comp, size_comp);
                }
                else {
                    aes_init_ctx(&ctx, key);
                    aes_ecb_decrypt_buffer(&ctx, temp_comp, size_comp);
                }
            }

            if (aes && f->ft) {
                if (gzip)
                    deflate_decompress((void*)((uint64_t)temp_comp + 0x10), size_comp - 0x10,
                        &ff->data, ff->size, DEFLATE_MODE_GZIP);
                else
                    memcpy(ff->data, (void*)((uint64_t)temp_comp + 0x10), ff->size);
            }
            else {
                if (gzip)
                    deflate_decompress(temp_comp, size_comp,
                        &ff->data, ff->size, DEFLATE_MODE_GZIP);
                else
                    memcpy(ff->data, temp_comp, ff->size);
            }
            free(temp_comp);
        }
        else
            io_read(s, ff->data, ff->size);
    }
}
