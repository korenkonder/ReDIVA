/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "farc.h"
#include "aes.h"
#include "io_deflate.h"
#include "io_path.h"
#include "utf8.h"

static const char key[] = {
    0x70, 0x72, 0x6F, 0x6A, 0x65, 0x63, 0x74, 0x5F,
    0x64, 0x69, 0x76, 0x61, 0x2E, 0x62, 0x69, 0x6E,
};

static const char keyFT[] = {
    0x13, 0x72, 0xD5, 0x7B, 0x6E, 0x9E, 0x31, 0xEB,
    0xA2, 0x39, 0xB8, 0x3C, 0x15, 0x57, 0xC6, 0xBB,
};

len_array_func(farc_file)

static errno_t farc_read_header(farc* f, stream* s);
static void farc_unpack_files(farc* f, stream* s, bool save);
static void farc_unpack_file(farc* f, stream* s, farc_file* ff);
static errno_t farc_get_files(farc* f);
static void farc_pack_files(farc* f, stream* s, farc_compress_mode mode);

farc* farc_init() {
    farc* f = force_malloc(sizeof(farc));
    return f;
}

void farc_dispose(farc* f) {
    if (!f)
        return;

    for (size_t i = 0; i < f->files.length; i++) {
        farc_file* ff = &f->files.data[i];
        free(ff->name);
        free(ff->data);
    }

    len_array_farc_file_free(&f->files);
    free(f->file_path);
    free(f->directory_path);
    free(f);
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

    memset(f, 0, sizeof(farc));

    wchar_t* full_path_buf = force_malloc_s(sizeof(wchar_t), MAX_PATH);
    full_path_buf = _wfullpath(full_path_buf, path, MAX_PATH);

    if (!full_path_buf)
        return;
    else if (!path_wcheck_file_exists(full_path_buf)) {
        free(full_path_buf);
        return;
    }

    size_t full_path_buf_len = wcslen(full_path_buf);
    f->file_path = force_malloc_s(sizeof(wchar_t), full_path_buf_len + 1);
    memcpy(f->file_path, full_path_buf, sizeof(wchar_t) * (full_path_buf_len + 1));
    free(full_path_buf);

    f->directory_path = path_wget_without_extension(f->file_path);

    stream* s = io_wopen(f->file_path, L"rb");
    if (s->io && !farc_read_header(f, s) && unpack)
        farc_unpack_files(f, s, save);
    io_dispose(s);
}

static errno_t farc_read_header(farc* f, stream* s) {
    if (!f || !s)
        return -1;

    f->signature = io_read_int32_t(s);
    if (f->signature != FARC_FArc && f->signature != FARC_FArC && f->signature != FARC_FARC)
        return -2;

    f->ft = false;

    int32_t header_length = io_read_int32_t_reverse_endianess(s, true);
    if (f->signature == FARC_FARC) {
        f->type = (farc_type)io_read_int32_t_reverse_endianess(s, true);
        io_read_int32_t(s);
        int32_t farc_mode = io_read_int32_t_reverse_endianess(s, true);

        f->ft = (f->type & FARC_DATA_AES) && (farc_mode & (farc_mode - 1));
    }

    f->files = len_array_farc_file_empty;
    if (f->ft) {
        io_set_position(s, 0x10, IO_SEEK_SET);

        header_length -= 0x08;
        uint8_t* d_t = force_malloc(header_length);
        io_read(s, d_t, header_length);

        struct aes_ctx ctx;
        aes_init_ctx(&ctx, keyFT);
        aes_cbc_decrypt_buffer(&ctx, d_t, header_length);

        uint8_t* dt = d_t;
        dt += 0x10 + sizeof(int32_t);
        f->files.length = *(int32_t*)dt;
        reverse_endianess_int32_t(f->files.length);
        if (f->files.length == 1) {
            dt += sizeof(int32_t);
            f->files.length = *(int32_t*)dt;
            reverse_endianess_int32_t(f->files.length);
        }
        else
            f->files.length = 0;
        dt += sizeof(int32_t) * 2;

        if (f->files.length == 0) {
            size_t count = 0;
            uint8_t* position = dt;
            while (dt - d_t < header_length) {
                while (*dt)
                    dt++;
                dt++;
                dt += sizeof(int32_t) * 4;
                count++;
            }
            dt = position;
            f->files.length = count;
        }
        f->files.data = force_malloc_s(sizeof(farc_file), f->files.length);
        f->files.fulllength = f->files.length;

        char* file = force_malloc(BUF_SIZE);
        for (size_t i = 0; i < f->files.length; i++) {
            size_t f_len = 0;
            while (*dt) {
                file[f_len] = *dt;
                f_len++;
                dt++;
            }
            dt++;
            file[f_len] = 0;

            farc_file* ff = &f->files.data[i];
            ff->name = utf8_decode(file);

            ff->offset = *(int32_t*)dt;
            dt += sizeof(int32_t);
            ff->size_compressed = *(int32_t*)dt;
            dt += sizeof(int32_t);
            ff->size_uncompressed = *(int32_t*)dt;
            dt += sizeof(int32_t);
            ff->type = (farc_type)*(int32_t*)dt;
            dt += sizeof(int32_t);
            reverse_endianess_int32_t(ff->offset);
            reverse_endianess_int32_t(ff->size_compressed);
            reverse_endianess_int32_t(ff->size_uncompressed);
            reverse_endianess_int32_t(ff->type);
        }
        free(file);
        free(d_t);
    }
    else {
        if (f->signature == FARC_FARC && io_read_int32_t_reverse_endianess(s, true) == 1)
            f->files.length = io_read_int32_t_reverse_endianess(s, true);

        io_read_int32_t(s);

        if (f->files.length == 0) {
            size_t count = 0;
            size_t position = io_get_position(s);
            while (io_get_position(s) < header_length) {
                while (io_read_char(s));
                io_read_int32_t(s);
                if (f->signature != FARC_FArc)
                    io_read_int32_t(s);
                io_read_int32_t(s);
                count++;
            }
            io_set_position(s, position, IO_SEEK_SET);
            f->files.length = count;
        }
        f->files.data = force_malloc_s(sizeof(farc_file), f->files.length);
        f->files.fulllength = f->files.length;

        char* file = force_malloc(BUF_SIZE);
        for (size_t i = 0; i < f->files.length; i++) {
            char c;
            size_t f_len = 0;
            while (c = io_read_char(s)) {
                file[f_len] = c;
                f_len++;
            }
            file[f_len] = 0;

            farc_file* ff = &f->files.data[i];
            ff->name = utf8_decode(file);

            ff->offset = io_read_int32_t_reverse_endianess(s, true);
            if (f->signature != FARC_FArc)
                ff->size_compressed = io_read_int32_t_reverse_endianess(s, true);
            ff->size_uncompressed = io_read_int32_t_reverse_endianess(s, true);
        }
        free(file);
    }
    return 0;
}

static void farc_unpack_files(farc* f, stream* s, bool save) {
    if (!f || !s)
        return;

    if (!save) {
        for (size_t i = 0; i < f->files.length; i++)
            farc_unpack_file(f, s, &f->files.data[i]);
        return;
    }

    size_t max_path_len = 0;
    size_t dir_len = wcslen(f->directory_path);
    for (size_t i = 0; i < f->files.length; i++) {
        size_t path_len = dir_len + 1 + wcslen(f->files.data->name);
        if (max_path_len < path_len)
            max_path_len = path_len;
    }

    CreateDirectoryW(f->directory_path, 0);
    wchar_t* temp_path = force_malloc_s(sizeof(wchar_t), max_path_len + 1);
    memcpy(temp_path, f->directory_path, sizeof(wchar_t) * dir_len);
    temp_path[dir_len] = L'\\';

    for (size_t i = 0; i < f->files.length; i++) {
        farc_file* ff = &f->files.data[i];
        farc_unpack_file(f, s, ff);

        size_t path_len = wcslen(ff->name);
        memcpy(temp_path + dir_len + 1, ff->name, sizeof(wchar_t) * path_len);
        temp_path[dir_len + 1 + path_len] = L'\0';

        if (ff->data) {
            stream* temp_s = io_wopen(temp_path, L"wb");
            if (temp_s->io)
                io_write(temp_s, ff->data, ff->size_uncompressed);
            io_dispose(temp_s);
            free(ff->data);
        }
    }
    free(temp_path);
}

static void farc_unpack_file(farc* f, stream* s, farc_file* ff) {
    if (!f || !s)
        return;

    if (ff->data)
        free(ff->data);

    ff->data = force_malloc(ff->size_uncompressed);
    io_set_position(s, ff->offset, IO_SEEK_SET);

    if (f->signature != FARC_FARC) {
        if (f->signature == FARC_FArC) {
            void* temp_comp = force_malloc(ff->size_compressed);
            io_read(s, temp_comp, ff->size_compressed);
            deflate_decompress(temp_comp, ff->size_compressed,
                &ff->data, ff->size_uncompressed, DEFLATE_MODE_GZIP);
            free(temp_comp);
        }
        else
            io_read(s, ff->data, ff->size_uncompressed);
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
                        &ff->data, ff->size_uncompressed, DEFLATE_MODE_GZIP);
                else
                    memcpy(ff->data, (void*)((uint64_t)temp_comp + 0x10), ff->size_uncompressed);
            }
            else {
                if (gzip)
                    deflate_decompress(temp_comp, size_comp,
                        &ff->data, ff->size_uncompressed, DEFLATE_MODE_GZIP);
                else
                    memcpy(ff->data, temp_comp, ff->size_uncompressed);
            }
            free(temp_comp);
        }
        else
            io_read(s, ff->data, ff->size_uncompressed);
    }
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

    for (size_t i = 0; i < f->files.length; i++)
        if (!wcscmp(f->files.data[i].name, name))
            return &f->files.data[i];
    return 0;
}

void farc_write(farc* f, char* path, farc_compress_mode mode) {
    if (!f || !path)
        return;

    wchar_t* path_buf = char_string_to_wchar_t_string(path);
    farc_wwrite(f, path_buf, mode);
    free(path_buf);
}

void farc_wwrite(farc* f, wchar_t* path, farc_compress_mode mode) {
    if (!f || !path)
        return;

    memset(f, 0, sizeof(farc));

    wchar_t* full_path_buf = force_malloc_s(sizeof(wchar_t), MAX_PATH);
    full_path_buf = _wfullpath(full_path_buf, path, MAX_PATH);

    if (!full_path_buf)
        return;
    else if (!path_wcheck_directory_exists(full_path_buf)) {
        free(full_path_buf);
        return;
    }

    size_t full_path_buf_len = wcslen(full_path_buf);
    f->directory_path = force_malloc_s(sizeof(wchar_t), full_path_buf_len + 1);
    memcpy(f->directory_path, full_path_buf, sizeof(wchar_t) * (full_path_buf_len + 1));
    free(full_path_buf);

    f->file_path = path_wadd_extension(f->directory_path, L".farc");

    if (!farc_get_files(f)) {
        stream* s = io_wopen(f->file_path, L"wb");
        if (s->io)
            farc_pack_files(f, s, mode);
        io_dispose(s);
    }
}

static errno_t farc_get_files(farc* f) {
    f->files.data = 0;
    f->files.length = f->files.fulllength = 0;

    len_array_pointer_wchar_t files;
    path_wget_files(&files, f->directory_path);
    if (!files.length)
        return -1;

    f->files.length = f->files.fulllength = files.length;
    f->files.data = force_malloc_s(sizeof(farc_file), files.length);
    for (size_t i = 0; i < files.length; i++) {
        farc_file* ff = &f->files.data[i];
        memset(ff, 0, sizeof(farc_file));
        size_t len = wcslen(files.data[i]);
        ff->name = force_malloc_s(sizeof(wchar_t), len + 1);
        memcpy(ff->name, files.data[i], sizeof(wchar_t) * (len + 1));
    }
    len_array_pointer_wchar_t_free(&files);
    return 0;
}

static void farc_pack_files(farc* f, stream* s, farc_compress_mode mode) {
    bool plain = false;
    for (size_t i = 0; i < f->files.length; i++) {
        farc_file* ff = &f->files.data[i];
        wchar_t* ext = path_wget_extension(ff->name);
        bool is_a3da = wcscmp(ext, L".a3da") == 0;
        bool is_diva = wcscmp(ext, L".diva") == 0;
        bool is_drs = wcscmp(ext, L".drs") == 0;
        bool is_dve = wcscmp(ext, L".dve") == 0;
        bool is_lst = wcscmp(ext, L".lst") == 0;
        bool is_farc = wcscmp(ext, L".farc") == 0;
        free(ext);

        if (is_a3da | is_diva | is_drs | is_dve | is_lst | is_farc) {
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

    char** files_utf8 = force_malloc_s(sizeof(char*), f->files.length);
    if (mode == FARC_COMPRESS_FArc)
        for (size_t i = 0; i < f->files.length; i++) {
            files_utf8[i] = utf8_encode(f->files.data[i].name);
            header_length += strlen(files_utf8[i]) + 1;
            header_length += sizeof(int32_t) * 2;
        }
    else
        for (size_t i = 0; i < f->files.length; i++) {
            files_utf8[i] = utf8_encode(f->files.data[i].name);
            header_length += strlen(files_utf8[i]) + 1;
            header_length += sizeof(int32_t) * 3;
        }

    size_t align = header_length + 8;
    io_set_position(s, align_val(align, 0x10), IO_SEEK_SET);
    size_t dir_len = wcslen(f->directory_path);
    wchar_t* temp = force_malloc_s(sizeof(wchar_t), dir_len + 2 + MAX_PATH);
    memcpy(temp, f->directory_path, sizeof(wchar_t) * dir_len);
    temp[dir_len] = L'\\';

    struct aes_ctx ctx;
    if (mode == FARC_COMPRESS_FARC_AES || mode == FARC_COMPRESS_FARC_GZIP_AES)
        aes_init_ctx(&ctx, key);

    for (size_t i = 0; i < f->files.length; i++) {
        farc_file* ff = &f->files.data[i];
        memcpy(temp + dir_len + 1, ff->name, sizeof(wchar_t) * (wcslen(ff->name) + 1));
        ff->offset = io_get_position(s);

        stream* s_t = io_wopen(temp, L"rb");
        if (s_t->io) {
            io_set_position(s_t, 0, IO_SEEK_END);
            size_t file_len = io_get_position(s_t);
            io_set_position(s_t, 0, IO_SEEK_SET);

            ff->size_uncompressed = (int32_t)file_len;

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
                ff->size_compressed = (int32_t)t1_len;
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
                ff->size_compressed = (int32_t)t1_len;
                t = t1;
            }
            else {
                io_write(s, t, file_len);
                size_t a = align_val(file_len, 0x10) - file_len;
                for (size_t j = 0; j < a; j++)
                    io_write_uint8_t(s, 0x00);
            }
            free(t);
        }
        io_dispose(s_t);
    }

    free(temp);

    io_set_position(s, 0, IO_SEEK_SET);
    switch (mode) {
    case FARC_COMPRESS_FARC:
    case FARC_COMPRESS_FARC_GZIP:
    case FARC_COMPRESS_FARC_AES:
    case FARC_COMPRESS_FARC_GZIP_AES:
        io_write_int32_t(s, FARC_FARC);
        io_write_int32_t_reverse_endianess(s, (int32_t)header_length, true);
        io_write_int32_t_reverse_endianess(s, f->type, true);
        io_write_int32_t_reverse_endianess(s, 0x00, true);
        io_write_int32_t_reverse_endianess(s, 0x10, true);
        io_write_int32_t_reverse_endianess(s, 0x00, true);
        io_write_int32_t_reverse_endianess(s, 0x00, true);
        break;
    case FARC_COMPRESS_FArC:
        io_write_int32_t(s, FARC_FArC);
        io_write_int32_t_reverse_endianess(s, (int32_t)header_length, true);
        io_write_int32_t_reverse_endianess(s, 0x10, true);
        break;
    default:
        io_write_int32_t(s, FARC_FArc);
        io_write_int32_t_reverse_endianess(s, (int32_t)header_length, true);
        io_write_int32_t_reverse_endianess(s, 0x10, true);
        break;
    }

    if (mode == FARC_COMPRESS_FArc)
        for (size_t i = 0; i < f->files.length; i++) {
            farc_file* ff = &f->files.data[i];
            io_write(s, files_utf8[i], strlen(files_utf8[i]));
            io_write_uint8_t(s, 0x00);
            io_write_int32_t_reverse_endianess(s, (int32_t)ff->offset, true);
            io_write_int32_t_reverse_endianess(s, (int32_t)ff->size_uncompressed, true);
        }
    else
        for (size_t i = 0; i < f->files.length; i++) {
            farc_file* ff = &f->files.data[i];
            io_write(s, files_utf8[i], strlen(files_utf8[i]));
            io_write_uint8_t(s, 0x00);
            io_write_int32_t_reverse_endianess(s, (int32_t)ff->offset, true);
            io_write_int32_t_reverse_endianess(s, (int32_t)ff->size_compressed, true);
            io_write_int32_t_reverse_endianess(s, (int32_t)ff->size_uncompressed, true);
        }

    align = align_val(align, 0x10) - align;
    if (mode == FARC_COMPRESS_FArc)
        for (size_t i = 0; i < align; i++)
            io_write_uint8_t(s, 0x00);
    else
        for (size_t i = 0; i < align; i++)
            io_write_uint8_t(s, 0x78);

    for (size_t i = 0; i < f->files.length; i++)
        free(files_utf8[i]);
    free(files_utf8);
}
