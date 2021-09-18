/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "kkc.h"
#include "vector.h"

#define KKFS_SECTOR_SIZE            0x200
#define KKFS_DIRECTORY_RECURSION    0x100

#define KKFS_NAME_LENGTH            0x16

typedef enum kkfs_sector_info {
    KKFS_SECTOR_FREE            = 0x00000000,
    KKFS_SECTOR_RESERVED        = 0x00000001,
    KKFS_SECTOR_END_OF_CHAIN    = 0xFFFFFFFF,
} kkfs_sector_info;

typedef enum kkfs_directory_flags {
    KKFS_DIRECTORY_READ_ONLY    = 0x1,
} kkfs_directory_flags;

typedef enum kkfs_file_flags {
    KKFS_FILE_READ_ONLY         = 0x1,
    KKFS_FILE_CIPHER            = 0x2,
} kkfs_file_flags;

typedef enum kkfs_type {
    KKFS_EMPTY                  = 0x00000000,
    KKFS_DELETED                = 0x00000001,
    KKFS_DIRECTORY              = 0xFFFFFFFE,
    KKFS_FILE                   = 0xFFFFFFFF,
} kkfs_type;

typedef struct kkfs_deleted {
    kkfs_type type : 2;
    uint8_t data[0x40 - sizeof(kkfs_type)];
} kkfs_deleted;

typedef struct kkfs_directory {
    kkfs_type type : 2;
    uint32_t data_sector : 30;
    wchar_t name[KKFS_NAME_LENGTH];
    uint8_t padding0[0x08];
    kkfs_directory_flags flags;
    uint8_t padding1[0x04];
} kkfs_directory;

typedef struct kkfs_file {
    kkfs_type type : 2;
    uint32_t data_sector : 30;
    wchar_t name[KKFS_NAME_LENGTH];
    uint64_t size;
    kkfs_file_flags flags;
    uint32_t iv;
} kkfs_file;

typedef union kkfs_data {
    kkfs_type type : 2;
    kkfs_deleted del;
    kkfs_directory dir;
    kkfs_file file;
} kkfs_data;

typedef struct kkfs_header {
    uint32_t hash;
    uint32_t key_hash;
    uint32_t parent_hash;
    uint32_t parent_key_hash;
    kkfs_directory_flags flags;
    uint8_t padding[0x4];
    uint32_t sector_size;
    uint32_t sectors_count;
    wchar_t name[0x10];
} kkfs_header;

typedef struct kkfs_struct {
    uint8_t jmp_code[3];
    char signature[0x08];
    uint8_t padding0[0x05];
    kkfs_header header;
    uint8_t padding1[0x0A];
    uint8_t code[0x1A4];
    uint16_t end_signature;
} kkfs_struct;

typedef struct kkfs {
    kkfs_struct data;
    kkfs_sector_info* sector_info;
    FILE* io;
    kkc* cipher;
    uint32_t current_directory_index;
    kkfs_directory* current_directory;
    uint32_t free_sector;
    bool data_changed;
    bool sector_info_changed;
} kkfs;

extern kkfs* kkfs_init();
extern void kkfs_initialize(kkfs* fs, char* path, uint32_t sector_size, size_t length,
    kkfs_directory_flags flags, kkc* curse, uint32_t parent_hash, kkc* parent_curse);
extern void kkfs_winitialize(kkfs* fs, wchar_t* path, uint32_t sector_size, size_t length,
    kkfs_directory_flags flags, kkc* curse, uint32_t parent_hash, kkc* parent_curse);
extern void kkfs_open(kkfs* fs, char* path, kkc* curse, uint64_t parent_hash, kkc* parent_curse);
extern void kkfs_wopen(kkfs* fs, wchar_t* path, kkc* curse, uint64_t parent_hash, kkc* parent_curse);
extern bool kkfs_create_directory(kkfs* fs, char* path, kkfs_directory_flags flags);
extern bool kkfs_wcreate_directory(kkfs* fs, wchar_t* path, kkfs_directory_flags flags);
extern bool kkfs_enter_directory(kkfs* fs, char* path);
extern bool kkfs_wenter_directory(kkfs* fs, wchar_t* path);
extern bool kkfs_exit_directory(kkfs* fs);
extern bool kkfs_delete_directory(kkfs* fs, char* path);
extern bool kkfs_wdelete_directory(kkfs* fs, wchar_t* path);
extern bool kkfs_create_file(kkfs* fs, char* path, uint64_t size, kkfs_file_flags flags);
extern bool kkfs_wcreate_file(kkfs* fs, wchar_t* path, uint64_t size, kkfs_file_flags flags);
extern bool kkfs_read_file(kkfs* fs, char* path, void** data, size_t* data_length);
extern bool kkfs_wread_file(kkfs* fs, wchar_t* path, void** data, size_t* data_length);
extern bool kkfs_write_file(kkfs* fs, char* path, void* data, size_t data_length);
extern bool kkfs_wwrite_file(kkfs* fs, wchar_t* path, void* data, size_t data_length);
extern bool kkfs_delete_file(kkfs* fs, char* path);
extern bool kkfs_wdelete_file(kkfs* fs, wchar_t* path);
extern void kkfs_close(kkfs* fs);
extern void kkfs_dispose(kkfs* fs);
