/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <random>
#include "../KKdLib/default.hpp"
#include "../KKdLib/io/stream.hpp"
#include "../KKdLib/prj/rijndael.hpp"
#include "../KKdLib/prj/shared_ptr.hpp"
#include "../KKdLib/farc.hpp"

enum farc_type {
    FARC_NORMAL = 0,
    FARC_COMPRESSED,
    FARC_EXTENDED,
};

struct farc_aes_key {
    uint8_t data[prj::Rijndael_Nb * sizeof(uint32_t)];
};

struct farc_params {
    size_t align;
    bool compress;
    bool encrypt;
    bool key_set;
    farc_aes_key key;

    farc_params();

    void set_key(const farc_aes_key* key);
};

struct farc_read_file {
    char name[128];
    uint32_t offset;
    uint32_t size;
    uint32_t original_size;
    bool compressed;
    bool encrypted;

    farc_read_file();
};

struct farc_read {
    const void* data;
    size_t size;
    int32_t field_10;
    int32_t field_14;
    farc_type type;
    farc_signature signature;
    uint32_t header_length;
    farc_flags flags;
    uint32_t field_28;
    uint32_t alignment;
    uint32_t entry_padding;
    uint32_t header_padding;
    std::vector<farc_read_file> files;
    prj::shared_ptr<prj::Rijndael> rijndael;

    farc_read();
    ~farc_read();

    const farc_read_file* get_file(int32_t index) const;
    size_t get_file_size(int32_t index) const;
    const void* get_file_data(int32_t index) const;
    int32_t get_file_index(const char* name) const;
    bool get_file_is_plain(int32_t index) const;
    const char* get_file_name(int32_t index) const;
    size_t get_file_offset(int32_t index) const;
    size_t get_file_original_size(int32_t index) const;
    size_t get_files_count() const;
    bool init(const void* data, size_t size, const farc_aes_key* key = 0);
    bool load_file_data(void* data, size_t size, int32_t index) const;
    void reset();
};

struct farc_ft_params {
    size_t align;
    bool compress;
    bool encrypt;
    bool key_set;
    farc_aes_key key;
    bool normal;

    farc_ft_params();

    void set_key(const farc_aes_key* key);
};

struct farc_ft_read_file {
    char name[128];
    uint32_t offset;
    uint32_t size;
    uint32_t original_size;
    farc_flags flags;

    farc_ft_read_file();
};

struct farc_ft_read {
    const void* data;
    size_t size;
    farc_type type;
    farc_signature signature;
    uint32_t header_length;
    farc_flags flags;
    uint32_t field_20;
    uint32_t header_size;
    uint32_t alignment;
    uint32_t files_count;
    uint32_t entry_size;
    std::vector<farc_ft_read_file> files;
    prj::shared_ptr<farc_aes_key> aes_key;

    farc_ft_read();
    ~farc_ft_read();

    const farc_ft_read_file* get_file(int32_t index) const;
    int32_t get_file_size(int32_t index) const;
    const void* get_file_data(int32_t index) const;
    int32_t get_file_index(const char* name) const;
    const char* get_file_name(int32_t index) const;
    int32_t get_file_offset(int32_t index) const;
    int32_t get_file_original_size(int32_t index) const;
    uint32_t get_files_count() const;
    bool init(const void* data, size_t size);
    bool load_file_data(void* data, size_t size, int32_t index) const;
    void reset();
};

struct farc_write_file {
    std::string name;
    size_t offset;
    size_t size;
    size_t original_size;

    farc_write_file();
    ~farc_write_file();
};

struct farc_write {
    std::string path;
    farc_params params;
    farc_type type;
    std::vector<farc_write_file> files;
    std::string data_path;
    FILE* file_handle;
    size_t data_offset;
    prj::Rijndael rijndael;

    farc_write();
    ~farc_write();

    bool add_file(const void* data, size_t size, const std::string& name);
    size_t get_align(size_t size) const;
    bool open(const std::string& path, const farc_params& params);
    void reset();
    bool write_data(const void* data, size_t size);
    bool write_file();
    bool write_header(FILE* f);
};

struct p_farc_write {
    farc_write* ptr;

    p_farc_write();
    ~p_farc_write();

    bool add_file(const void* data, size_t size, const std::string& file);
    bool open(const std::string& path, const farc_params& params);
    bool open(const std::string& path, bool compress, size_t align);
    bool write_file();
};

struct farc_ft_write_file {
    std::string name;
    uint32_t offset;
    uint32_t size;
    uint32_t original_size;
    farc_flags flags;

    farc_ft_write_file();
    ~farc_ft_write_file();
};

struct farc_ft_write {
    std::string path;
    farc_ft_params params;
    farc_type type;
    std::vector<farc_ft_write_file> files;
    std::string data_path;
    FILE* file_handle;
    std::mt19937 mt_rand;
    prj::shared_ptr<farc_aes_key> aes_key;

    farc_ft_write();
    ~farc_ft_write();

    bool add_file(const void* data, size_t size, const std::string& name);
    size_t get_align(size_t size) const;
    bool open(const std::string& path, const farc_ft_params& params);
    void reset();
    bool write_data(const void* data, size_t size, size_t& act_size, bool& encrypted);
    bool write_file();
    bool write_header(FILE* f);
    bool write_pad(size_t align, bool random, FILE* f);
};

struct p_farc_ft_write {
    farc_ft_write* ptr;

    p_farc_ft_write();
    ~p_farc_ft_write();

    bool add_file(const void* data, size_t size, const std::string& file);
    bool open(const std::string& path, const farc_ft_params& params);
    bool open(const std::string& path, bool compress, size_t align);
    bool write_file();
};
