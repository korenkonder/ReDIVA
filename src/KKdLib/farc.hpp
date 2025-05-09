/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "default.hpp"

enum farc_signature {
    FARC_FArc = 'FArc',
    FARC_FArC = 'FArC',
    FARC_FARC = 'FARC',
};

enum farc_flags {
    FARC_NONE = 0x00,
    FARC_GZIP = 0x02,
    FARC_AES  = 0x04,
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

    inline farc_file() : offset(), size(), size_compressed(), data(),
        data_compressed(), compressed(), encrypted(), data_changed() {

    }

    inline ~farc_file() {
        if (data)
            free(data);
        if (data_compressed)
            free(data_compressed);
    }
};

struct farc {
    std::string file_path;
    std::string directory_path;
    std::vector<farc_file> files;
    farc_signature signature;
    farc_flags flags;
    int32_t compression_level;
    uint32_t alignment;
    union {
        uint32_t entry_padding;
        uint32_t entry_size;
    };
    union {
        uint32_t header_padding;
        uint32_t header_size;
    };
    bool ft;

    farc(farc_signature signature = FARC_FArC, farc_flags flags = (farc_flags)0, bool ft = false);
    ~farc();

    farc_file* add_file(const char* name);
    farc_file* add_file(const wchar_t* name);
    const char* get_file_name(uint32_t hash);
    size_t get_file_size(const char* name);
    size_t get_file_size(const wchar_t* name);
    size_t get_file_size(uint32_t hash);
    bool has_file(const char* name);
    bool has_file(const wchar_t* name);
    bool has_file(uint32_t hash);
    void read(const char* path, bool unpack, bool save);
    void read(const wchar_t* path, bool unpack, bool save);
    void read(const void* data, size_t size, bool unpack);
    farc_file* read_file(const char* name);
    farc_file* read_file(const wchar_t* name);
    farc_file* read_file(uint32_t hash);
    void write(const char* path, farc_signature signature,
        farc_flags flags, bool add_extension, bool get_files);
    void write(const wchar_t* path, farc_signature signature,
        farc_flags flags, bool add_extension, bool get_files);
    void write(void** data, size_t* size, farc_signature signature,
        farc_flags flags);

    static bool load_file(void* data, const char* dir, const char* file, uint32_t hash);
};
