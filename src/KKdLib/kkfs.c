/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "kkfs.h"
#include "io/path.h"
#include "hash.h"
#include <sysinfoapi.h>
#include <share.h>

static const uint8_t boot_code[] = {
    0x33, 0xC9, 0x8E, 0xD1, 0xBC, 0xF4, 0x7B, 0x8E, 0xC1, 0x8E, 0xD9, 0xBD, 0x00, 0x7C, 0x88, 0x56,
    0x40, 0x88, 0x4E, 0x02, 0x8A, 0x56, 0x40, 0xB4, 0x41, 0xBB, 0xAA, 0x55, 0xCD, 0x13, 0x72, 0x10,
    0x81, 0xFB, 0x55, 0xAA, 0x75, 0x0A, 0xF6, 0xC1, 0x01, 0x74, 0x05, 0xFE, 0x46, 0x02, 0xEB, 0x2D,
    0x8A, 0x56, 0x40, 0xB4, 0x08, 0xCD, 0x13, 0x73, 0x05, 0xB9, 0xFF, 0xFF, 0x8A, 0xF1, 0x66, 0x0F,
    0xB6, 0xC6, 0x40, 0x66, 0x0F, 0xB6, 0xD1, 0x80, 0xE2, 0x3F, 0xF7, 0xE2, 0x86, 0xCD, 0xC0, 0xED,
    0x06, 0x41, 0x66, 0x0F, 0xB7, 0xC9, 0x66, 0xF7, 0xE1, 0x66, 0x89, 0x46, 0xF8, 0x83, 0x7E, 0x16,
    0x00, 0x75, 0x39, 0x83, 0x7E, 0x2A, 0x00, 0x77, 0x33, 0x66, 0x8B, 0x46, 0x1C, 0x66, 0x83, 0xC0,
    0x0C, 0xBB, 0x00, 0x80, 0xB9, 0x01, 0x00, 0xE8, 0x2C, 0x00, 0xE9, 0xA8, 0x03, 0xA1, 0xF8, 0x7D,
    0x80, 0xC4, 0x7C, 0x8B, 0xF0, 0xAC, 0x84, 0xC0, 0x74, 0x17, 0x3C, 0xFF, 0x74, 0x09, 0xB4, 0x0E,
    0xBB, 0x07, 0x00, 0xCD, 0x10, 0xEB, 0xEE, 0xA1, 0xFA, 0x7D, 0xEB, 0xE4, 0xA1, 0x7D, 0x80, 0xEB,
    0xDF, 0x98, 0xCD, 0x16, 0xCD, 0x19, 0x66, 0x60, 0x80, 0x7E, 0x02, 0x00, 0x0F, 0x84, 0x20, 0x00,
    0x66, 0x6A, 0x00, 0x66, 0x50, 0x06, 0x53, 0x66, 0x68, 0x10, 0x00, 0x01, 0x00, 0xB4, 0x42, 0x8A,
    0x56, 0x40, 0x8B, 0xF4, 0xCD, 0x13, 0x66, 0x58, 0x66, 0x58, 0x66, 0x58, 0x66, 0x58, 0xEB, 0x33,
    0x66, 0x3B, 0x46, 0xF8, 0x72, 0x03, 0xF9, 0xEB, 0x2A, 0x66, 0x33, 0xD2, 0x66, 0x0F, 0xB7, 0x4E,
    0x18, 0x66, 0xF7, 0xF1, 0xFE, 0xC2, 0x8A, 0xCA, 0x66, 0x8B, 0xD0, 0x66, 0xC1, 0xEA, 0x10, 0xF7,
    0x76, 0x1A, 0x86, 0xD6, 0x8A, 0x56, 0x40, 0x8A, 0xE8, 0xC0, 0xE4, 0x06, 0x0A, 0xCC, 0xB8, 0x01,
    0x02, 0xCD, 0x13, 0x66, 0x61, 0x0F, 0x82, 0x74, 0xFF, 0x81, 0xC3, 0x00, 0x02, 0x66, 0x40, 0x49,
    0x75, 0x94, 0xC3, 0x42, 0x4F, 0x4F, 0x54, 0x4D, 0x47, 0x52, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x0D, 0x0A, 0x44, 0x69, 0x73, 0x6B, 0x20, 0x65, 0x72, 0x72, 0x6F, 0x72, 0xFF, 0x0D,
    0x0A, 0x50, 0x72, 0x65, 0x73, 0x73, 0x20, 0x61, 0x6E, 0x79, 0x20, 0x6B, 0x65, 0x79, 0x20, 0x74,
    0x6F, 0x20, 0x72, 0x65, 0x73, 0x74, 0x61, 0x72, 0x74, 0x0D, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAC, 0x01,
    0xB9, 0x01, 0x00, 0x00
};

static uint32_t kkfs_allocate_sectors(kkfs* fs, uint32_t sectors);
static uint32_t kkfs_allocate_sector(kkfs* fs);
static uint32_t kkfs_append_sector(kkfs* fs, uint32_t sector);
static uint32_t kkfs_calculate_key_hash(kkc* curse);
static bool kkfs_find(kkfs* fs, kkfs_data* data, wchar_t* path);
static bool kkfs_free(kkfs* fs, wchar_t* path);
static void kkfs_free_sector(kkfs* fs, uint32_t sector);
static uint64_t kkfs_get_current_time_ticks();
static uint32_t kkfs_get_free_sector(kkfs* fs, uint32_t sector);
static void kkfs_write_changes(kkfs* fs);
static void kkfs_write_first_sector(kkfs* fs);
static bool kkfs_write_kkfs_data(kkfs* fs, kkfs_data* data);
static void kkfs_write_sector_info(kkfs* fs);
inline static void next_rand_uint8_t_pointer(uint8_t* arr, size_t length, uint32_t* state);

kkfs* kkfs_init() {
    kkfs* fs = force_malloc(sizeof(kkfs));
    return fs;
}

void kkfs_initialize(kkfs* fs, char* path, uint32_t sector_size, size_t length,
    kkfs_directory_flags flags, kkc* curse, uint32_t parent_hash, kkc* parent_curse) {
    if (!fs || !path)
        return;

    wchar_t* path_buf = utf8_to_utf16(path);
    kkfs_winitialize(fs, path_buf, sector_size, length, flags, curse, parent_hash, parent_curse);
    free(path_buf);
}

void kkfs_winitialize(kkfs* fs, wchar_t* path, uint32_t sector_size, size_t length,
    kkfs_directory_flags flags, kkc* curse, uint32_t parent_hash, kkc* parent_curse) {
    if (!fs || !path)
        return;

    if (fs->io)
        fclose(fs->io);

    if (!sector_size)
        sector_size = KKFS_SECTOR_SIZE;
    else
        sector_size = align_val(sector_size, KKFS_SECTOR_SIZE);
    sector_size = clamp(sector_size, KKFS_SECTOR_SIZE, 0x2000);

    uint32_t sectors_count = (uint32_t)align_val_divide(length, sector_size, sector_size);
    if (!sectors_count)
        sectors_count = 1;

    uint64_t time_ticks = kkfs_get_current_time_ticks();
    uint32_t state = (uint32_t)((time_ticks >> 32) ^ time_ticks);

    size_t t = sectors_count * sizeof(kkfs_sector_info);
    t = align_val_divide(t, sector_size, sector_size);
    fs->sector_info = force_malloc_s(kkfs_sector_info, sectors_count);
    fs->sector_info[0] = KKFS_SECTOR_RESERVED;
    for (size_t i = 0; i < t; i++)
        fs->sector_info[1 + i] = KKFS_SECTOR_RESERVED;
    fs->sector_info[1 + t] = KKFS_SECTOR_END_OF_CHAIN;
    fs->sector_info_changed = true;

    memset(&fs->data, 0, sizeof(kkfs_struct));
    fs->data.jmp_code[0] = 0xEB;
    fs->data.jmp_code[1] = 0x58;
    fs->data.jmp_code[2] = 0x90;
    memcpy(fs->data.signature, "KKFS    ", 0x08);
    size_t path_length = utf16_length(path);
    memcpy(fs->data.header.name, path, sizeof(wchar_t) * (min(path_length, 0x0F) + 1));
    fs->data.header.name[0x0F] = 0;
    next_rand_uint8_t_pointer((uint8_t*)&fs->data.header.hash, 0x04, &state);
    fs->data.header.key_hash = kkfs_calculate_key_hash(curse);
    fs->data.header.flags = flags;
    fs->data.header.parent_hash = parent_hash;
    fs->data.header.parent_key_hash = kkfs_calculate_key_hash(parent_curse);
    fs->data.header.sector_size = sector_size;
    fs->data.header.sectors_count = sectors_count;
    memcpy(fs->data.code, (void*)boot_code, 0x1A4);
    fs->data.end_signature = 0xAA55;
    fs->data_changed = true;

    size_t root_directory_sector = (align_val(sizeof(kkfs_struct), sector_size)
        + align_val(sectors_count * sizeof(kkfs_sector_info), sector_size)) / sector_size;
    fs->current_directory_index = 0;
    fs->current_directory = force_malloc_s(kkfs_directory, KKFS_DIRECTORY_RECURSION);
    fs->current_directory[0].type = KKFS_DIRECTORY;
    fs->current_directory[0].data_sector = (uint32_t)root_directory_sector;
    fs->current_directory[0].flags = fs->data.header.flags;
    fs->cipher = curse;
    fs->io = _wfsopen(path, L"w+b", _SH_DENYNO);
    if (fs->io) {
        size_t length = fs->data.header.sectors_count;
        size_t temp = length * sizeof(kkfs_sector_info);
        size_t temp_aligned = align_val(temp, sector_size);

        uint8_t* td = force_malloc(sector_size);
        kkfs_write_changes(fs);
        for (size_t i = 1 + temp_aligned / sector_size; i < length; i++)
            fwrite(td, 1, sector_size, fs->io);
        free(td);
    }
}

void kkfs_open(kkfs* fs, char* path, kkc* curse, uint64_t parent_hash, kkc* parent_curse) {
    if (!fs || !path)
        return;

    wchar_t* path_buf = utf8_to_utf16(path);
    kkfs_wopen(fs, path_buf, curse, parent_hash, parent_curse);
    free(path_buf);
}

void kkfs_wopen(kkfs* fs, wchar_t* path, kkc* curse, uint64_t parent_hash, kkc* parent_curse) {
    if (!fs || !path)
        return;

    if (fs->io)
        fclose(fs->io);

    if (!path_wcheck_file_exists(path))
        return;

    fs->cipher = curse;

    fs->io = _wfsopen(path, L"r+b", _SH_DENYNO);
    if (fs->io) {
        fread(&fs->data, sizeof(kkfs_struct), 1, fs->io);
        if (memcmp(fs->data.signature, "KKFS    ", 0x08)
            || fs->data.header.key_hash != kkfs_calculate_key_hash(fs->cipher)
            || fs->data.header.parent_hash != parent_hash
            || fs->data.header.parent_key_hash != kkfs_calculate_key_hash(parent_curse)) {
            fclose(fs->io);
            fs->cipher = 0;
            fs->io = 0;
        }
        else {
            uint32_t sector_size = fs->data.header.sector_size;
            uint32_t sectors_count = fs->data.header.sectors_count;
            size_t root_directory_sector = (align_val(sizeof(kkfs_struct), sector_size)
                + align_val(sectors_count * sizeof(kkfs_sector_info), sector_size)) / sector_size;
            fs->current_directory_index = 0;
            fs->current_directory = force_malloc_s(kkfs_directory, KKFS_DIRECTORY_RECURSION);
            fs->current_directory[0].data_sector = (uint32_t)root_directory_sector;
            fs->current_directory[0].flags = fs->data.header.flags;

            fs->sector_info = force_malloc_s(kkfs_sector_info, sectors_count);
            _fseeki64(fs->io, KKFS_SECTOR_SIZE, SEEK_SET);
            fread(fs->sector_info, sizeof(kkfs_sector_info), sectors_count, fs->io);
        }
    }
}

bool kkfs_create_directory(kkfs* fs, char* path, kkfs_directory_flags flags) {
    if (!fs || !path || fs->data.header.flags & KKFS_DIRECTORY_READ_ONLY)
        return false;

    wchar_t* path_buf = utf8_to_utf16(path);
    bool result = kkfs_wcreate_directory(fs, path_buf, flags);
    free(path_buf);
    return result;
}

bool kkfs_wcreate_directory(kkfs* fs, wchar_t* path, kkfs_directory_flags flags) {
    if (!fs || !path || fs->data.header.flags & KKFS_DIRECTORY_READ_ONLY)
        return false;

    uint32_t free_sector = kkfs_allocate_sector(fs);
    if (!free_sector)
        return false;

    uint64_t time_ticks = kkfs_get_current_time_ticks();
    uint32_t state = (uint32_t)((time_ticks >> 32) ^ time_ticks);

    kkfs_data data;
    memset(&data, 0, sizeof(kkfs_data));
    data.type = KKFS_DIRECTORY;
    data.dir.data_sector = free_sector;
    size_t path_length = utf16_length(path) + 1;
    memcpy(data.dir.name, path, sizeof(wchar_t) * min(path_length, KKFS_NAME_LENGTH));
    data.dir.flags = flags;

    bool result = kkfs_write_kkfs_data(fs, &data);
    if (!result)
        kkfs_free_sector(fs, free_sector);
    fflush(fs->io);
    return result;
}

bool kkfs_enter_directory(kkfs* fs, char* path) {
    if (!fs || !path || fs->current_directory_index >= KKFS_DIRECTORY_RECURSION)
        return false;

    wchar_t* path_buf = utf8_to_utf16(path);
    bool result = kkfs_wenter_directory(fs, path_buf);
    free(path_buf);
    return result;
}

bool kkfs_wenter_directory(kkfs* fs, wchar_t* path) {
    if (!fs || !path || fs->current_directory_index >= KKFS_DIRECTORY_RECURSION)
        return false;

    kkfs_data data;
    memset(&data, 0, sizeof(kkfs_data));
    if (!kkfs_find(fs, &data, path) || data.type != KKFS_DIRECTORY)
        return false;

    fs->current_directory_index++;
    fs->current_directory[fs->current_directory_index] = data.dir;
    return true;
}

bool kkfs_exit_directory(kkfs* fs) {
    if (!fs || !fs->current_directory_index)
        return false;

    memset(&fs->current_directory[fs->current_directory_index], 0, sizeof(kkfs_directory));
    fs->current_directory_index--;
    return true;
}

bool kkfs_delete_directory(kkfs* fs, char* path) {
    if (!fs || !path || fs->data.header.flags & KKFS_DIRECTORY_READ_ONLY)
        return false;

    wchar_t* path_buf = utf8_to_utf16(path);
    bool result = kkfs_wdelete_directory(fs, path_buf);
    free(path_buf);
    return result;
}

bool kkfs_wdelete_directory(kkfs* fs, wchar_t* path) {
    if (!fs || !path || fs->data.header.flags & KKFS_DIRECTORY_READ_ONLY)
        return false;

    kkfs_data data;
    memset(&data, 0, sizeof(kkfs_data));
    if (!kkfs_find(fs, &data, path) || data.type != KKFS_DIRECTORY)
        return false;

    return false;
}

bool kkfs_create_file(kkfs* fs, char* path, uint64_t size, kkfs_file_flags flags) {
    if (!fs || !path || fs->data.header.flags & KKFS_DIRECTORY_READ_ONLY)
        return false;

    wchar_t* path_buf = utf8_to_utf16(path);
    bool result = kkfs_wcreate_file(fs, path_buf, size, flags);
    free(path_buf);
    return result;
}

bool kkfs_wcreate_file(kkfs* fs, wchar_t* path, uint64_t size, kkfs_file_flags flags) {
    if (!fs || !path || fs->data.header.flags & KKFS_DIRECTORY_READ_ONLY)
        return false;

    uint32_t sector_size = fs->data.header.sector_size;
    uint32_t free_sector = kkfs_allocate_sectors(fs,
        (uint32_t)align_val_divide(size, sector_size, sector_size));
    if (!free_sector)
        return false;

    uint64_t time_ticks = kkfs_get_current_time_ticks();
    uint32_t state = (uint32_t)((time_ticks >> 32) ^ time_ticks);

    kkfs_data data;
    memset(&data, 0, sizeof(kkfs_data));
    data.type = KKFS_FILE;
    data.file.data_sector = free_sector;
    size_t path_length = utf16_length(path) + 1;
    memcpy(data.file.name, path, sizeof(wchar_t) * min(path_length, KKFS_NAME_LENGTH));
    data.file.size = size;
    data.file.flags = flags;
    if (flags & KKFS_FILE_CIPHER)
        next_rand_uint8_t_pointer((uint8_t*)&data.file.iv, 0x4, &state);
    else
        data.file.iv = 0;

    bool result = kkfs_write_kkfs_data(fs, &data);
    if (!result)
        kkfs_free_sector(fs, free_sector);
    return result;
}

bool kkfs_read_file(kkfs* fs, char* path, void** data, size_t* data_length) {
    if (!fs || !path || !data || !data_length)
        return false;

    wchar_t* path_buf = utf8_to_utf16(path);
    bool result = kkfs_wread_file(fs, path_buf, data, data_length);
    free(path_buf);
    return result;
}

bool kkfs_wread_file(kkfs* fs, wchar_t* path, void** data, size_t* data_length) {
    if (!fs || !path || !data || !data_length)
        return false;

    *data = 0;
    *data_length = 0;

    kkfs_data dat;
    memset(&dat, 0, sizeof(kkfs_data));
    if (!kkfs_find(fs, &dat, path) || dat.type != KKFS_FILE)
        return false;

    uint32_t sector_size = fs->data.header.sector_size;
    void* buf = force_malloc(sector_size);

    *data = force_malloc(dat.file.size);
    *data_length = dat.file.size;

    _fseeki64(fs->io, dat.file.data_sector * (uint64_t)sector_size, SEEK_SET);
    for (size_t i = 0; i < dat.file.size; i += sector_size) {
        fread(buf, 1, sector_size, fs->io);

        if (fs->cipher && dat.file.flags & KKFS_FILE_CIPHER) {
            kkc_reset_iv(fs->cipher, dat.file.iv + (uint32_t)(i / sector_size));
            kkc_decipher(fs->cipher, KKC_MODE_PAST, buf, buf, sector_size);
        }

        memcpy(((uint8_t*)*data + i), buf, min(dat.file.size - i, sector_size));

        uint64_t sector = _ftelli64(fs->io) / fs->data.header.sector_size;
        uint32_t info = (uint32_t)fs->sector_info[sector - 1];
        if (info > KKFS_SECTOR_RESERVED && info != KKFS_SECTOR_END_OF_CHAIN) {
            sector = info;
            _fseeki64(fs->io, sector * fs->data.header.sector_size, SEEK_SET);
        }
    }
    free(buf);
    return true;
}

bool kkfs_write_file(kkfs* fs, char* path, void* data, size_t data_length) {
    if (!fs || !path || !data || fs->data.header.flags & KKFS_DIRECTORY_READ_ONLY)
        return false;

    wchar_t* path_buf = utf8_to_utf16(path);
    bool result = kkfs_wwrite_file(fs, path_buf, data, data_length);
    free(path_buf);
    return result;
}

bool kkfs_wwrite_file(kkfs* fs, wchar_t* path, void* data, size_t data_length) {
    if (!fs || !path || !data || fs->data.header.flags & KKFS_DIRECTORY_READ_ONLY)
        return false;

    kkfs_data dat;
    memset(&dat, 0, sizeof(kkfs_data));
    if (!kkfs_find(fs, &dat, path) || dat.type != KKFS_FILE)
        return false;

    if (dat.file.size < data_length) {
        kkfs_wdelete_file(fs, path);
        kkfs_wcreate_file(fs, path, data_length, dat.file.flags);
        memset(&dat, 0, sizeof(kkfs_data));
        kkfs_find(fs, &dat, path);
    }

    uint32_t sector_size = fs->data.header.sector_size;
    void* buf = force_malloc(sector_size);

    _fseeki64(fs->io, dat.file.data_sector * (uint64_t)sector_size, SEEK_SET);
    for (size_t i = 0; i < dat.file.size; i += sector_size) {
        if (dat.file.size - i < sector_size) {
            memcpy(buf, ((uint8_t*)data + i), dat.file.size - i);
            memset(buf, 0, sector_size - (dat.file.size - i));
        }
        else
            memcpy(buf, ((uint8_t*)data + i), sector_size);

        if (fs->cipher && dat.file.flags & KKFS_FILE_CIPHER) {
            kkc_reset_iv(fs->cipher, dat.file.iv + (uint32_t)(i / sector_size));
            kkc_cipher(fs->cipher, KKC_MODE_PAST, buf, buf, sector_size);
        }

        fwrite(buf, 1, sector_size, fs->io);

        uint64_t sector = _ftelli64(fs->io) / fs->data.header.sector_size;
        uint32_t info = (uint32_t)fs->sector_info[sector - 1];
        if (info > KKFS_SECTOR_RESERVED && info != KKFS_SECTOR_END_OF_CHAIN) {
            sector = info;
            _fseeki64(fs->io, sector * (uint64_t)sector_size, SEEK_SET);
        }
    }
    free(buf);

    return true;
}

bool kkfs_delete_file(kkfs* fs, char* path) {
    if (!fs || !path || fs->data.header.flags & KKFS_DIRECTORY_READ_ONLY)
        return false;

    wchar_t* path_buf = utf8_to_utf16(path);
    bool result = kkfs_wdelete_file(fs, path_buf);
    free(path_buf);
    return result;
}

bool kkfs_wdelete_file(kkfs* fs, wchar_t* path) {
    if (!fs || !path || fs->data.header.flags & KKFS_DIRECTORY_READ_ONLY)
        return false;

    kkfs_data data;
    memset(&data, 0, sizeof(kkfs_data));
    if (!kkfs_find(fs, &data, path) || data.type != KKFS_FILE)
        return false;

    kkfs_free(fs, path);
    kkfs_free_sector(fs, data.file.data_sector);
    return true;
}

void kkfs_close(kkfs* fs) {
    if (!fs)
        return;

    kkfs_write_changes(fs);

    if (fs->io)
        fclose(fs->io);
    fs->io = 0;
}

void kkfs_dispose(kkfs* fs) {
    if (!fs)
        return;

    free(fs->current_directory);
    free(fs->sector_info);
    free(fs);
}

static uint32_t kkfs_allocate_sectors(kkfs* fs, uint32_t sectors) {
    if (!sectors)
        return 0;

    fs->free_sector = kkfs_get_free_sector(fs, fs->free_sector);
    if (!fs->free_sector)
        return 0;

    fs->sector_info_changed = true;
    fs->sector_info[fs->free_sector] = KKFS_SECTOR_END_OF_CHAIN;
    uint32_t first_sector = fs->free_sector;
    for (size_t i = 1; i < sectors; i++) {
        kkfs_append_sector(fs, 0);
        if (!fs->free_sector) {
            kkfs_free_sector(fs, first_sector);
            return 0;
        }
    }
    return first_sector;
}

static uint32_t kkfs_allocate_sector(kkfs* fs) {
    fs->free_sector = kkfs_get_free_sector(fs, fs->free_sector);
    if (fs->free_sector) {
        fs->sector_info_changed = true;
        fs->sector_info[fs->free_sector] = KKFS_SECTOR_END_OF_CHAIN;
    }
    return fs->free_sector;
}

static uint32_t kkfs_append_sector(kkfs* fs, uint32_t sector) {
    uint32_t prev_sector = sector ? sector : fs->free_sector;
    fs->free_sector = kkfs_get_free_sector(fs, prev_sector);
    if (fs->free_sector) {
        fs->sector_info_changed = true;
        fs->sector_info[prev_sector] = fs->free_sector;
    }
    fs->sector_info[fs->free_sector] = KKFS_SECTOR_END_OF_CHAIN;
    return fs->free_sector;
}

static uint32_t kkfs_calculate_key_hash(kkc* curse) {
    if (!curse)
        return 0;

    size_t kl = (size_t)curse->key.type * 8;
    return hash_murmurhash((uint8_t*)curse->key.data, kl, 0, false, false);
}

static bool kkfs_find(kkfs* fs, kkfs_data* data, wchar_t* path) {
    size_t pos;
    size_t sector;
    kkfs_data dat;
    uint32_t info;

    size_t path_length = min(KKFS_NAME_LENGTH, utf16_length(path) + 1);
    kkfs_directory* curr_dir = &fs->current_directory[fs->current_directory_index];
    size_t data_sector = curr_dir->data_sector;

    _fseeki64(fs->io, data_sector * fs->data.header.sector_size, SEEK_SET);
    do {
        fread(&dat, sizeof(kkfs_data), 1, fs->io);
        pos = _ftelli64(fs->io);

        if (dat.type == KKFS_DIRECTORY || dat.type == KKFS_FILE) {
            wchar_t* name = dat.type == KKFS_DIRECTORY ? dat.dir.name : dat.file.name;
            if (!memcmp(name, path, sizeof(wchar_t) * path_length)) {
                *data = dat;
                return true;
            }
        }

        sector = pos / fs->data.header.sector_size;
        if (!(pos % fs->data.header.sector_size)) {
            info = (uint32_t)fs->sector_info[sector - 1];
            if (info > KKFS_SECTOR_RESERVED && info != KKFS_SECTOR_END_OF_CHAIN) {
                sector = info;
                pos = sector * fs->data.header.sector_size;

                if (dat.type)
                    _fseeki64(fs->io, pos, SEEK_SET);
            }
        }
    } while (dat.type);
    return false;
}

static bool kkfs_free(kkfs* fs, wchar_t* path) {
    size_t pos;
    size_t sector;
    kkfs_data dat;
    uint32_t info;
    uint64_t time_ticks;

    size_t path_length = min(KKFS_NAME_LENGTH, utf16_length(path) + 1);
    kkfs_directory* curr_dir = &fs->current_directory[fs->current_directory_index];
    size_t data_sector = curr_dir->data_sector;

    _fseeki64(fs->io, data_sector * fs->data.header.sector_size, SEEK_SET);
    do {
        fread(&dat, sizeof(kkfs_data), 1, fs->io);
        pos = _ftelli64(fs->io);

        if (dat.type == KKFS_DIRECTORY || dat.type == KKFS_FILE) {
            wchar_t* name = dat.type == KKFS_DIRECTORY ? dat.dir.name : dat.file.name;
            if (!memcmp(name, path, sizeof(wchar_t) * path_length)) {
                time_ticks = kkfs_get_current_time_ticks();

                memset(&dat, 0, sizeof(kkfs_data));
                dat.type = KKFS_DELETED;
                _fseeki64(fs->io, pos - sizeof(kkfs_data), SEEK_SET);
                fwrite(&dat, sizeof(kkfs_data), 1, fs->io);
                return true;
            }
        }

        sector = pos / fs->data.header.sector_size;
        if (!(pos % fs->data.header.sector_size)) {
            info = (uint32_t)fs->sector_info[sector - 1];
            if (info > KKFS_SECTOR_RESERVED && info != KKFS_SECTOR_END_OF_CHAIN) {
                sector = info;
                pos = sector * fs->data.header.sector_size;

                if (dat.type)
                    _fseeki64(fs->io, pos, SEEK_SET);
            }
        }
    } while (dat.type);
    return false;
}

static void kkfs_free_sector(kkfs* fs, uint32_t sector) {
    uint32_t info;
    do {
        info = (uint32_t)fs->sector_info[sector];
        fs->sector_info[sector] = KKFS_SECTOR_FREE;
        sector = info;
    } while (info > KKFS_SECTOR_RESERVED && info != KKFS_SECTOR_END_OF_CHAIN);
}

static uint64_t kkfs_get_current_time_ticks() {
    FILETIME time;
    GetSystemTimeAsFileTime(&time);
    return *(uint64_t*)&time + 504911232000000000;
}

static uint32_t kkfs_get_free_sector(kkfs* fs, uint32_t sector) {
    if (sector) {
        for (size_t i = sector + 1ULL; i < fs->data.header.sectors_count; i++)
            if (fs->sector_info[i] == KKFS_SECTOR_FREE)
                return (uint32_t)i;

        for (size_t i = 0; i < sector; i++)
            if (fs->sector_info[i] == KKFS_SECTOR_FREE)
                return (uint32_t)i;
    }
    else
        for (size_t i = 0; i < fs->data.header.sectors_count; i++)
            if (fs->sector_info[i] == KKFS_SECTOR_FREE)
                return (uint32_t)i;
    return 0;
}

static void kkfs_write_changes(kkfs* fs) {
    if (fs->data.header.flags & KKFS_DIRECTORY_READ_ONLY)
        return;

    if (fs->data_changed) {
        kkfs_write_first_sector(fs);
        fs->data_changed = false;
    }

    if (fs->sector_info_changed) {
        kkfs_write_sector_info(fs);
        fs->sector_info_changed = false;
    }
}

static void kkfs_write_first_sector(kkfs* fs) {
    _fseeki64(fs->io, 0x00, SEEK_SET);
    fwrite(&fs->data, sizeof(kkfs_struct), 1, fs->io);
}

static bool kkfs_write_kkfs_data(kkfs* fs, kkfs_data* data) {
    size_t pos;
    size_t sector;
    kkfs_data dat;
    uint32_t info;
    uint64_t time_ticks;

    kkfs_directory* curr_dir = &fs->current_directory[fs->current_directory_index];
    size_t data_sector = curr_dir->data_sector;

    _fseeki64(fs->io, data_sector * fs->data.header.sector_size, SEEK_SET);
    do {
        fread(&dat, sizeof(kkfs_data), 1, fs->io);
        pos = _ftelli64(fs->io);

        if ((dat.type == KKFS_DIRECTORY || dat.type == KKFS_FILE)
            && (data->type == KKFS_DIRECTORY || data->type == KKFS_FILE)) {
            wchar_t* name0 = dat.type == KKFS_DIRECTORY ? dat.dir.name : dat.file.name;
            wchar_t* name1 = data->type == KKFS_DIRECTORY ? data->dir.name : data->file.name;
            if (!memcmp(name0, name1, sizeof(wchar_t) * KKFS_NAME_LENGTH))
                return false;
        }


        sector = pos / fs->data.header.sector_size;
        if (!(pos % fs->data.header.sector_size)) {
            info = (uint32_t)fs->sector_info[sector - 1];
            if (info > KKFS_SECTOR_RESERVED && info != KKFS_SECTOR_END_OF_CHAIN) {
                sector = info;
                pos = sector * fs->data.header.sector_size;

                if (dat.type == KKFS_DIRECTORY || dat.type == KKFS_FILE)
                    _fseeki64(fs->io, pos, SEEK_SET);
            }
        }
    } while (dat.type == KKFS_DIRECTORY || dat.type == KKFS_FILE);

    time_ticks = kkfs_get_current_time_ticks();
    if (!(pos % fs->data.header.sector_size))
        kkfs_append_sector(fs, (uint32_t)(sector - 1));

    if (dat.type == KKFS_EMPTY || dat.type == KKFS_DELETED)
        pos -= sizeof(kkfs_data);

    _fseeki64(fs->io, pos, SEEK_SET);
    fwrite(data, sizeof(kkfs_data), 1, fs->io);
    return true;
}

static void kkfs_write_sector_info(kkfs* fs) {
    _fseeki64(fs->io, fs->data.header.sector_size, SEEK_SET);
    size_t length = fs->data.header.sectors_count;
    size_t temp = length * sizeof(kkfs_sector_info);
    size_t temp_aligned = align_val(temp, fs->data.header.sector_size);

    uint8_t* td = force_malloc(fs->data.header.sector_size);
    fwrite(fs->sector_info, sizeof(kkfs_sector_info), length, fs->io);
    if (temp_aligned - temp)
        fwrite(td, 1, temp_aligned - temp, fs->io);
    free(td);
}

inline static void next_rand_uint8_t_pointer(uint8_t* arr, size_t length, uint32_t* state) {
    if (!arr || length < 1)
        return;

    for (size_t i = 0; i < length; i++) {
        uint32_t x = *state;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        *state = x;
        arr[i] = (uint8_t)x;
    }
}
