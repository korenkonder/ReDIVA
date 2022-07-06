/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "default.hpp"
#include "io/stream.hpp"

enum farc_signature {
    FARC_FArc = 'FArc',
    FARC_FArC = 'FArC',
    FARC_FARC = 'FARC',
};

enum farc_flags {
    FARC_GZIP = 0x02,
    FARC_AES  = 0x04,
};

enum farc_compress_mode {
    FARC_COMPRESS_FArc          = 0,
    FARC_COMPRESS_FArC          = 1,
    FARC_COMPRESS_FARC          = 2,
    FARC_COMPRESS_FARC_GZIP     = 3,
    FARC_COMPRESS_FARC_AES      = 4,
    FARC_COMPRESS_FARC_GZIP_AES = 5,
};

struct farc_file {
    std::string name;
    size_t offset;
    size_t size;
    size_t size_compressed;
    void* data;
    void* data_compressed;
    bool compressed;
    bool encrypted;
    bool data_changed;

    farc_file();
    ~farc_file();
};

struct farc {
    std::string file_path;
    std::string directory_path;
    std::vector<farc_file> files;
    farc_signature signature;
    farc_flags flags;
    int32_t compression_level;
    bool ft;

    farc();
    virtual ~farc();

    void add_file(const char* name = 0);
    void add_file(const wchar_t* name);
    size_t get_file_size(const char* name);
    size_t get_file_size(const wchar_t* name);
    void read(const char* path, bool unpack, bool save);
    void read(const wchar_t* path, bool unpack, bool save);
    void read(const void* data, size_t size, bool unpack);
    farc_file* read_file(const char* name);
    farc_file* read_file(const wchar_t* name);
    farc_file* read_file(uint32_t hash);
    void write(const char* path, farc_compress_mode mode, bool get_files);
    void write(const wchar_t* path, farc_compress_mode mode, bool get_files);
    void write(void** data, size_t* size, farc_compress_mode mode);

    static bool load_file(void* data, const char* path, const char* file, uint32_t hash);
};
