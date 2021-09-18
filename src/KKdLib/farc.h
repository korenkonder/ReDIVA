/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "io_stream.h"
#include "vector.h"

typedef enum farc_signature {
    FARC_FArc = 'FArc',
    FARC_FArC = 'FArC',
    FARC_FARC = 'FARC',
} farc_signature;

typedef enum farc_data_type {
    FARC_DATA_TYPE_GZIP = 0x2,
    FARC_DATA_TYPE_AES  = 0x4,
} farc_data_type;

typedef enum farc_compress_mode {
    FARC_COMPRESS_FArc          = 0,
    FARC_COMPRESS_FArC          = 1,
    FARC_COMPRESS_FARC          = 2,
    FARC_COMPRESS_FARC_GZIP     = 3,
    FARC_COMPRESS_FARC_AES      = 4,
    FARC_COMPRESS_FARC_GZIP_AES = 5,
} farc_compress_mode;

typedef struct farc_file {
    size_t offset;
    size_t size;
    size_t size_compressed;
    string name;
    void* data;
    void* data_compressed;
    farc_data_type type;
    bool data_changed;
} farc_file;

vector(farc_file)

typedef struct farc {
    wchar_t file_path[MAX_PATH];
    wchar_t directory_path[MAX_PATH];
    vector_farc_file files;
    farc_signature signature;
    farc_data_type type;
    int32_t compression_level;
    bool ft;
} farc;

extern farc* farc_init();
extern void farc_read(farc* f, char* path, bool unpack, bool save);
extern void farc_wread(farc* f, wchar_t* path, bool unpack, bool save);
extern farc_file* farc_read_file(farc* f, char* name);
extern farc_file* farc_wread_file(farc* f, wchar_t* name);
extern void farc_write(farc* f, char* path, farc_compress_mode mode, bool get_files);
extern void farc_wwrite(farc* f, wchar_t* path, farc_compress_mode mode, bool get_files);
extern void farc_dispose(farc* f);
