/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#define _CRT_SECURE_NO_WARNINGS
#include "app_farc.hpp"
#include "../KKdLib/io/path.hpp"
#include "../KKdLib/deflate.hpp"

struct farc_buf_data {
    uint8_t* data;
    size_t len;
    size_t size;
};

static const farc_aes_key farc_key = { {
        0x70, 0x72, 0x6F, 0x6A, 0x65, 0x63, 0x74, 0x5F,
        0x64, 0x69, 0x76, 0x61, 0x2E, 0x62, 0x69, 0x6E,
    }
};

static const farc_aes_key farc_ft_key = { {
        0x13, 0x72, 0xD5, 0x7B, 0x6E, 0x9E, 0x31, 0xEB,
        0xA2, 0x39, 0xB8, 0x3C, 0x15, 0x57, 0xC6, 0xBB,
    }
};

static bool farc_ft_decrypt_data(const farc_aes_key* key, const farc_buf_data& src, farc_buf_data& dst);
static bool farc_ft_encrypt_data(const farc_aes_key* key, const farc_buf_data& src, farc_buf_data& dst);

farc_params::farc_params() : compress(), encrypt(), key_set(), key() {
    align = 1;
}

void farc_params::set_key(const farc_aes_key* key) {
    encrypt = true;
    if (key) {
        key_set = true;
        this->key = *key;
    }
    else {
        key_set = false;
        this->key = {};
    }
}

farc_read_file::farc_read_file() : name(), offset(),
size(), original_size(), compressed(), encrypted() {

}

farc_read::farc_read() : data(), size(), field_10(), field_14(), type(), signature(),
header_length(), flags(), field_28(), alignment(), entry_padding(), header_padding() {

}

farc_read::~farc_read() {

}

const farc_read_file* farc_read::get_file(int32_t index) const {
    if (index < files.size())
        return &files.data()[index];
    return 0;
}

size_t farc_read::get_file_size(int32_t index) const {
    const farc_read_file* ff = get_file(index);
    if (ff)
        return ff->size;
    return 0;
}

const void* farc_read::get_file_data(int32_t index) const {
    const farc_read_file* ff = get_file(index);
    if (ff)
        return (const uint8_t*)data + ff->offset;
    return 0;
}

int32_t farc_read::get_file_index(const char* name) const {
    const farc_read_file* ff = files.data();
    size_t file_count = files.size();
    for (size_t i = 0; i < file_count; i++, ff++) {
        if (!strcmp(ff->name, name))
            return (int32_t)i;
    }
    return -1;
}

bool farc_read::get_file_is_plain(int32_t index) const {
    const farc_read_file* ff = get_file(index);
    if (!ff || ff->compressed || ff->encrypted)
        return false;
    return true;
}

const char* farc_read::get_file_name(int32_t index) const {
    const farc_read_file* ff = get_file(index);
    if (ff)
        return ff->name;
    return 0;
}

size_t farc_read::get_file_offset(int32_t index) const {
    const farc_read_file* ff = get_file(index);
    if (ff)
        return ff->offset;
    return 0;
}

size_t farc_read::get_file_original_size(int32_t index) const {
    const farc_read_file* ff = get_file(index);
    if (ff)
        return ff->original_size;
    return 0;
}

size_t farc_read::get_files_count() const {
    return files.size();
}

bool farc_read::init(const void* data, size_t size, const farc_aes_key* key) {
    reset();

    if (!data)
        return false;

    this->data = data;
    this->size = size;
    type = FARC_NORMAL;

    const uint8_t* d = (const uint8_t*)data;

    farc_signature _signature = (farc_signature)load_reverse_endianness_uint32_t(d + 0x00);
    if (_signature != FARC_FArc && _signature != FARC_FArC && _signature != FARC_FARC)
        return false;

    signature = _signature;

    if (signature == FARC_FArC) {
        type = FARC_COMPRESSED;
        enum_or(flags, FARC_GZIP);
    }
    else if (signature == FARC_FARC)
        type = FARC_EXTENDED;

    header_length = load_reverse_endianness_uint32_t(d + 0x04);
    d += 0x08;

    uint32_t offset = 0;
    if (type < FARC_EXTENDED) {
        alignment = load_reverse_endianness_uint32_t(d + offset);
        offset += sizeof(uint32_t);

        if ((flags & FARC_AES) && alignment < prj::Rijndael_Nlen)
            return false;
    }
    else {
        flags = (farc_flags)load_reverse_endianness_uint32_t(d + offset);
        offset += sizeof(uint32_t);

        if (flags & FARC_AES) {
            const farc_aes_key* _key = &farc_key;
            if (key)
                _key = key;
            if (!_key)
                return false;

            farc_params params;
            params.set_key(_key);

            rijndael = prj::shared_ptr<prj::Rijndael>(
                new prj::Rijndael(prj::Rijndael_Nb, prj::Rijndael_Nk128, params.key.data));
        }

        field_28 = load_reverse_endianness_uint32_t(d + offset);
        offset += sizeof(uint32_t);

        alignment = load_reverse_endianness_uint32_t(d + offset);
        offset += sizeof(uint32_t);

        if ((flags & FARC_AES) && alignment < prj::Rijndael_Nlen)
            return false;

        entry_padding = load_reverse_endianness_uint32_t(d + offset);
        offset += sizeof(uint32_t);

        header_padding = load_reverse_endianness_uint32_t(d + offset);
        offset += sizeof(uint32_t);

        if (offset + header_padding < offset)
            return false;

        offset += header_padding;

        if (offset > header_length)
            return false;
    }

    while (offset < header_length) {
        farc_read_file ff;
        strncpy_s(ff.name, sizeof(ff.name), (const char*)d + offset, sizeof(ff.name));
        offset += (uint32_t)(strlen(ff.name) + 1);

        ff.offset = load_reverse_endianness_uint32_t(d + offset);
        offset += sizeof(uint32_t);

        ff.size = load_reverse_endianness_uint32_t(d + offset);
        offset += sizeof(uint32_t);

        uint32_t original_size = 0;
        ff.original_size = 0;
        if (type >= FARC_COMPRESSED) {
            original_size = load_reverse_endianness_uint32_t(d + offset);
            offset += sizeof(uint32_t);

            ff.original_size = original_size;
            if (type >= FARC_EXTENDED) {
                if (offset > offset + entry_padding || offset + entry_padding > header_length)
                    break;

                offset += entry_padding;
            }
        }

        ff.compressed = !!original_size;
        ff.encrypted = !!(flags & FARC_AES);
        files.push_back(ff);
    }
    return true;
}

bool farc_read::load_file_data(void* data, size_t size, int32_t index) const {
    if (!data)
        return false;

    const farc_read_file* ff = get_file(index);
    if (!ff)
        return false;

    const uint8_t* src = (const uint8_t*)get_file_data(index);
    if (!src || !ff->size)
        return false;

    std::vector<uint8_t> vec;
    if (ff->encrypted) {
        size_t size_align = align_val((size_t)ff->size, prj::Rijndael_Nlen);
        vec.resize(size_align);
        if (size_align) {
            uint8_t* dst = vec.data();
            size_t block_count = ((size_align - 1) / prj::Rijndael_Nlen) + 1;
            do {
                memcpy(dst, src, prj::Rijndael_Nlen);
                rijndael.get()->decrypt16(dst);
                src += prj::Rijndael_Nlen;
                dst += prj::Rijndael_Nlen;
                --block_count;
            } while (block_count);
        }
        src = (uint8_t*)vec.data();
    }

    if (ff->compressed) {
        size_t data_size = ff->original_size;
        if (size < data_size || deflate::decompress(src, ff->size, data, data_size, deflate::MODE_GZIP, 0) < 0)
            return false;
    }
    else {
        if (size < ff->size)
            return false;

        memmove(data, src, ff->size);
    }
    return true;
}

void farc_read::reset() {
    data = 0;
    size = 0;
    signature = (farc_signature)0;
    header_length = 0;
    flags = (farc_flags)0;
    field_28 = 0;
    alignment = 0;
    entry_padding = 0;
    header_padding = 0;
    files.clear();
    files.shrink_to_fit();
    rijndael.reset();
}

farc_ft_params::farc_ft_params() : compress(), encrypt(), key_set(), key(), normal() {
    align = 1;
    encrypt = true;
    align = prj::Rijndael_Nlen;
}

void farc_ft_params::set_key(const farc_aes_key* key) {
    encrypt = true;
    if (key) {
        key_set = true;
        this->key = *key;
    }
    else {
        key_set = false;
        this->key = {};
    }
}

farc_ft_read_file::farc_ft_read_file() : name(), offset(),
size(), original_size(), flags() {

}

farc_ft_read::farc_ft_read() : data(), size(), type(), signature(), header_length(),
flags(), field_20(), header_size(), alignment(), files_count(), entry_size() {

}

farc_ft_read::~farc_ft_read() {

}

const farc_ft_read_file* farc_ft_read::get_file(int32_t index) const {
    if (index < files.size())
        return &files.data()[index];
    return 0;
}

int32_t farc_ft_read::get_file_size(int32_t index) const {
    const farc_ft_read_file* ff = get_file(index);
    if (ff)
        return ff->size;
    return -1;
}

const void* farc_ft_read::get_file_data(int32_t index) const {
    const farc_ft_read_file* ff = get_file(index);
    if (ff)
        return (const uint8_t*)data + ff->offset;
    return 0;
}

int32_t farc_ft_read::get_file_index(const char* name) const {
    const farc_ft_read_file* ff = files.data();
    size_t file_count = files.size();
    for (size_t i = 0; i < file_count; i++, ff++) {
        if (!strcmp(ff->name, name))
            return (int32_t)i;
    }
    return -1;
}

const char* farc_ft_read::get_file_name(int32_t index) const {
    const farc_ft_read_file* ff = get_file(index);
    if (ff)
        return ff->name;
    return 0;
}

int32_t farc_ft_read::get_file_offset(int32_t index) const {
    const farc_ft_read_file* ff = get_file(index);
    if (ff)
        return ff->offset;
    return -1;
}

int32_t farc_ft_read::get_file_original_size(int32_t index) const {
    const farc_ft_read_file* ff = get_file(index);
    if (ff)
        return ff->original_size;
    return -1;
}

uint32_t farc_ft_read::get_files_count() const {
    return files_count;
}

struct farc_read_data {
    const uint8_t* base;
    const uint8_t* begin;
    const uint8_t* end;

    farc_read_data(const uint8_t* data, size_t size);

    bool check_end() const;
    int32_t read_farc_file_name(char* str, size_t size);
    int32_t read_uint32_t(uint32_t& value);
    size_t remain_size() const;
    void seek(size_t offset);
};

farc_read_data::farc_read_data(const uint8_t* data, size_t size) {
    base = data;
    begin = data;
    end = data + size;
}

inline bool farc_read_data::check_end() const {
    return begin != end;
}

int32_t farc_read_data::read_farc_file_name(char* str, size_t size) {
    const uint8_t* cur = begin;

    char* str_end = str + size;
    while (str != str_end && cur != end) {
        char ch = *cur++;
        *str++ = ch;

        if (!ch) {
            size_t len = cur - begin;
            seek(len);
            return (int32_t)(len - 1);
        }
    }

    seek(cur - begin);
    return -1;
}

inline size_t farc_read_data::remain_size() const {
    return end - begin;
}

int32_t farc_read_data::read_uint32_t(uint32_t& value) {
    if (remain_size() >= sizeof(uint32_t)) {
        value = reverse_endianness_uint32_t(*(uint32_t*)begin);
        begin += min_def(sizeof(uint32_t), remain_size());
        return sizeof(uint32_t);
    }
    else {
        begin = end;
        return -1;
    }
}

inline void farc_read_data::seek(size_t offset) {
    begin += min_def(offset, remain_size());
}

bool farc_ft_read::init(const void* data, size_t size) {
    farc_buf_data dst;
    farc_buf_data src;

    reset();

    if (!data)
        return false;

    this->data = data;
    this->size = size;
    this->type = FARC_NORMAL;

    farc_read_data read_data((const uint8_t*)data, size);

    uint32_t _signature = 0;
    if (read_data.read_uint32_t(_signature) < 0)
        return false;
    else if ((_signature - FARC_FARC) & FARC_SIGNATURE_MASK)
        return false;
    else if (_signature == FARC_FARc) // Unsupported
        return false;
    signature = (farc_signature)_signature;

    if (signature == FARC_FArC) {
        type = FARC_COMPRESSED;
        enum_or(flags, FARC_GZIP);
    }
    else if (signature == FARC_FARC)
        type = FARC_EXTENDED;

    if (read_data.read_uint32_t(header_length) < 0)
        return false;

    if (read_data.remain_size() < header_length)
        return false;

    std::vector<uint8_t> temp;

    read_data.base = read_data.begin;
    read_data.end = &read_data.begin[header_length];
    if (type < FARC_EXTENDED) {
        if (read_data.read_uint32_t(alignment) < 0)
            return false;
    }
    else {
        if (read_data.read_uint32_t(*(uint32_t*)&flags) < 0 || read_data.read_uint32_t(field_20) < 0)
            return false;

        if ((flags & FARC_AES)) {
            const farc_aes_key* _key = &farc_ft_key;
            if (!_key)
                return false;

            aes_key = prj::shared_ptr<farc_aes_key>(new farc_aes_key);
            *aes_key.get() = *_key;
        }

        if (flags & FARC_AES) {
            src.data = (uint8_t*)read_data.begin;
            size_t len = this->header_length - sizeof(uint32_t) * 2;
            src.size = len;
            src.len = len;

            temp.resize(len);

            dst.data = temp.data();
            dst.len = 0;
            dst.size = temp.size();
            if (!farc_ft_decrypt_data(aes_key.get(), src, dst))
                return false;

            read_data.base = dst.data;
            read_data.begin = dst.data;
            read_data.end = &dst.data[dst.len];
        }

        size_t remain = read_data.remain_size();
        if (read_data.read_uint32_t(header_size) < 0)
            return false;

        if (remain < header_size
            || header_size < 0x10
            || read_data.read_uint32_t(alignment) < 0
            || read_data.read_uint32_t(files_count) < 0
            || read_data.read_uint32_t(entry_size) < 0
            || entry_size < 0x10)
            return false;

        read_data.seek(header_size - 0x10);
    }

    files.reserve(files_count);
    while (read_data.check_end()) {
        if (type >= FARC_EXTENDED && files.size() >= files_count)
            return false;

        farc_ft_read_file ff;
        memset(&ff, 0, sizeof(ff));
        if (read_data.read_farc_file_name(ff.name, sizeof(ff.name)) < 0)
            return false;

        if (type >= FARC_EXTENDED && read_data.remain_size() < entry_size)
            break;

        if (read_data.read_uint32_t(ff.offset) < 0 || read_data.read_uint32_t(ff.size) < 0)
            return false;

        ff.original_size = ff.size;
        if (type >= FARC_EXTENDED) {
            if (read_data.read_uint32_t(ff.original_size) < 0
                || read_data.read_uint32_t(*(uint32_t*)&ff.flags) < 0)
                return false;

            read_data.seek(entry_size - 0x10);
        }
        else if (type >= FARC_COMPRESSED) {
            if (read_data.read_uint32_t(ff.original_size) < 0)
                return false;

            if (ff.original_size)
                enum_or(ff.flags, FARC_GZIP);
            else
                ff.original_size = ff.size;
        }
        files.push_back(ff);
    }

    if (type < FARC_EXTENDED)
        files_count = (uint32_t)files.size();

    return true;
}

bool farc_ft_read::load_file_data(void* data, size_t size, int32_t index) const {
    if (!data)
        return false;

    const farc_ft_read_file* ff = get_file(index);
    if (!ff)
        return false;

    const uint8_t* src = (const uint8_t*)get_file_data(index);
    if (!src)
        return false;

    size_t buf_count;
    if (!(ff->flags & FARC_AES))
        buf_count = ff->flags & FARC_GZIP ? 1 : 0;
    else
        buf_count = ff->flags & FARC_GZIP ? 2 : 1;

    buf_count++;
    if (buf_count > 3)
        return false;

    if (buf_count == 1) {
        if (size >= ff->size) {
            memmove(data, src, ff->size);
            return 1;
        }
        return false;
    }

    std::vector<uint8_t> temp;
    temp.resize(ff->size);

    farc_buf_data buf[3];
    buf[0].data = (uint8_t*)src;
    buf[0].len = ff->size;
    buf[0].size = ff->size;

    for (size_t i = 1; i < buf_count; i++) {
        uint8_t* buf_data;
        size_t buf_size;
        if ((i % 2) == (buf_count % 2)) {
            buf_data = temp.data();
            buf_size = temp.size();
        }
        else {
            buf_data = (uint8_t*)data;
            buf_size = size;
        }
        buf[i].data = buf_data;
        buf[i].len = 0;
        buf[i].size = buf_size;
    }

    if (buf[buf_count - 1].data != data)
        return false;

    size_t buf_off = 0;
    if (ff->flags & FARC_AES) {
        if (buf_count <= 1)
            return false;

        if (!farc_ft_decrypt_data(aes_key.get(), buf[0], buf[1]))
            return false;

        buf_off = 1;
    }

    if (ff->flags & FARC_GZIP) {
        if (buf_off + 1 >= buf_count)
            return false;

        if (deflate::decompress(buf[buf_off].data, buf[buf_off].len,
            *(void**)&buf[buf_off + 1].data, buf[buf_off + 1].len, deflate::MODE_GZIP, 0) < 0)
            return false;
    }

    return true;
}

void farc_ft_read::reset() {
    data = 0;
    size = 0;
    type = FARC_NORMAL;
    signature = (farc_signature)0;
    header_length = 0;
    flags = (farc_flags)0;
    field_20 = 0;
    header_size = 0;
    alignment = 0;
    files_count = 0;
    entry_size = 0;
    files.clear();
    files.shrink_to_fit();
    aes_key.reset();
}

farc_write_file::farc_write_file() : offset(), size(), original_size() {

}

farc_write_file::~farc_write_file() {

}

farc_write::farc_write() : type(), file_handle(), data_offset() {
    reset();
}

farc_write::~farc_write() {

}

bool farc_write::add_file(const void* data, size_t size, const std::string& name) {
    if (!data || !size || !name.size() || !file_handle)
        return false;

    farc_write_file ff;
    ff.name.assign(name);
    ff.offset = data_offset;
    ff.size = size;
    ff.original_size = 0;

    void* temp = 0;
    const void* comp_data = data;
    size_t comp_size = size;
    if (params.compress) {
        ff.size = deflate::compress_bound(size, deflate::MODE_GZIP) + (name.size() + 1);
        ff.original_size = size;
        int32_t res = deflate::compress_gzip(data, size, temp, ff.size, 9, name.c_str());
        if (!temp)
            return false;

        if (ff.size < ff.original_size && res >= 0) {
            comp_data = temp;
            comp_size = ff.size;
        }
        else {
            ff.size = size;
            ff.original_size = 0;
        }
    }

    write_data(comp_data, comp_size);

    files.push_back(ff);
    free_def(temp);
    return true;
}

size_t farc_write::get_align(size_t size) const {
    return (~(params.align - 1) & (params.align + size - 1)) - size;
}

bool farc_write::open(const std::string& path, const farc_params& params) {
    reset();

    if (!path.size() || !params.align || ((params.align - 1) & params.align))
        return false;

    this->path.assign(path);
    this->params = params;

    farc_type type = FARC_NORMAL;
    if (params.compress)
        type = FARC_COMPRESSED;
    if (params.encrypt)
        type = FARC_EXTENDED;
    this->type = type;

    if (params.encrypt) {
        this->params.align = max_def(this->params.align, prj::Rijndael_Nlen);

        const farc_aes_key* key = &this->params.key;
        if (!this->params.key_set)
            key = &farc_key;

        rijndael = prj::Rijndael(prj::Rijndael_Nb, prj::Rijndael_Nk128, key->data);
    }

    data_path.assign(this->path + ".data");

    file_handle = _wfopen(utf8_to_utf16(data_path).c_str(), L"wb"); // fopen(data_path.c_str(), "wb")
    if (!file_handle)
        return false;

    return true;
}

void farc_write::reset() {
    path.clear();
    params = farc_params();
    type = FARC_NORMAL;
    files.clear();
    data_path.clear();
    file_handle = 0;
    data_offset = 0;
    rijndael = prj::Rijndael();
}

bool farc_write::write_data(const void* data, size_t size) {
    if (params.encrypt) {
        size_t align = get_align(size);
        size_t size_align = align + size;
        if (size_align % prj::Rijndael_Nlen)
            return false;

        for (size_t offset = 0; offset < size_align; offset += prj::Rijndael_Nlen) {
            uint8_t buf[prj::Rijndael_Nlen];
            if (size >= offset + prj::Rijndael_Nlen)
                memmove(buf, (const uint8_t*)data + offset, prj::Rijndael_Nlen);
            else {
                memcpy(buf, "xxxxxxxxxxxxxxxx", prj::Rijndael_Nlen);
                if (offset < size)
                    memmove(buf, (const uint8_t*)data + offset, size - offset);
            }

            rijndael.encrypt16(buf);

            if (!fwrite(buf, 1, prj::Rijndael_Nlen, file_handle))
                return false;
        }

        data_offset += size_align;
        return true;
    }
    else {
        if (!fwrite(data, size, 1, file_handle))
            return false;

        size_t align = get_align(size);
        size_t size_align = align + size;
        for (size_t offset = 0; offset < align; offset++) {
            if (fputc('x', file_handle) < 0)
                return false;
        }

        data_offset += size_align;
        return true;
    }
}

bool farc_write::write_file() {
    std::string tmp_path = path + ".tmp";

    bool ret = false;
    if (file_handle) {
        fclose(file_handle);
        file_handle = 0;

        if (!path_check_file_exists(path.c_str()) || path_delete_file(path.c_str())) {
            FILE* f_dst = _wfopen(utf8_to_utf16(tmp_path).c_str(), L"wb"); // fopen(tmp_path.c_str(), "wb")
            if (f_dst && write_header(f_dst)) {
                FILE* f_src = _wfopen(utf8_to_utf16(data_path).c_str(), L"rb"); // fopen(data_path.c_str(), "rb");
                if (f_src) {
                    const size_t buf_size = 0x1000;
                    void* buf = force_malloc(buf_size);
                    if (buf) {
                        int64_t data_len = path_stat_get_file_size(data_path.c_str());
                        while (data_len > 0) {
                            size_t len = fread(buf, 1, buf_size, f_src);
                            if (!len || !fwrite(buf, len, 1, f_dst))
                                break;

                            data_len -= len;
                        }

                        if (!data_len)
                            ret = true;
                        free(buf);
                    }
                    fclose(f_src);
                }
                fclose(f_dst);
            }

            if (ret) {
                path_delete_file(path.c_str());
                if (!path_move_file(tmp_path.c_str(), path.c_str()))
                    ret = false;
            }
        }
    }

    path_delete_file(data_path.c_str());
    path_delete_file(tmp_path.c_str());
    reset();
    return ret;
}

bool farc_write::write_header(FILE* f) {
    if (!f)
        return false;

    farc_flags flags = (farc_flags)0;
    if (params.compress)
        enum_or(flags, FARC_GZIP);
    else
        enum_and(flags, ~FARC_GZIP);

    if (params.encrypt)
        enum_or(flags, FARC_AES);
    else
        enum_and(flags, ~FARC_AES);

    uint32_t align = (uint32_t)params.align;

    const size_t signature_length = sizeof(uint32_t) * 2;

    size_t header_length;
    if (type < FARC_EXTENDED)
        header_length = sizeof(uint32_t);
    else
        header_length = sizeof(uint32_t) * 5;

    for (const farc_write_file& i : files) {
        header_length += sizeof(uint32_t) * 2 + 1 + i.name.size();
        if (type >= FARC_COMPRESSED)
            header_length += sizeof(uint32_t);
    }

    size_t header_align = get_align(header_length + signature_length);

    farc_signature signature = FARC_FArc;
    if (type == FARC_COMPRESSED)
        signature = FARC_FArC;
    else if (type == FARC_EXTENDED)
        signature = FARC_FARC;

    uint32_t tmp;
    tmp = reverse_endianness_uint32_t(signature);
    if (!fwrite(&tmp, sizeof(uint32_t), 1, f))
        return false;

    tmp = reverse_endianness_uint32_t((uint32_t)header_length);
    if (!fwrite(&tmp, sizeof(uint32_t), 1, f))
        return false;

    if (type < FARC_EXTENDED) {
        tmp = reverse_endianness_uint32_t(align);
        if (!fwrite(&tmp, sizeof(uint32_t), 1, f))
            return false;
    }
    else {
        tmp = reverse_endianness_uint32_t(flags);
        if (!fwrite(&tmp, sizeof(uint32_t), 1, f))
            return false;

        tmp = reverse_endianness_uint32_t(0);
        if (!fwrite(&tmp, sizeof(uint32_t), 1, f))
            return false;

        tmp = reverse_endianness_uint32_t(align);
        if (!fwrite(&tmp, sizeof(uint32_t), 1, f))
            return false;

        tmp = reverse_endianness_uint32_t(0);
        if (!fwrite(&tmp, sizeof(uint32_t), 1, f))
            return false;

        tmp = reverse_endianness_uint32_t(0);
        if (!fwrite(&tmp, sizeof(uint32_t), 1, f))
            return false;
    }

    for (const farc_write_file& i : files) {
        if (!fwrite(i.name.c_str(), i.name.size(), 1, f))
            break;
        else if (fputc('\0', f) < 0)
            break;

        tmp = reverse_endianness_uint32_t(
            (uint32_t)(signature_length + header_length + header_align + i.offset));
        if (!fwrite(&tmp, sizeof(uint32_t), 1, f))
            break;
        tmp = reverse_endianness_uint32_t((uint32_t)i.size);
        if (!fwrite(&tmp, sizeof(uint32_t), 1, f))
            break;
        if (type >= FARC_COMPRESSED)
        {
            tmp = reverse_endianness_uint32_t((uint32_t)i.original_size);
            if (!fwrite(&tmp, sizeof(uint32_t), 1, f))
                break;
        }
    }

    for (; header_align; header_align--)
        if (fputc('x', f) < 0)
            return false;

    return true;
}

p_farc_write::p_farc_write() : ptr() {
    ptr = new farc_write;
}

p_farc_write::~p_farc_write() {
    if (ptr) {
        delete ptr;
        ptr = 0;
    }
}

bool p_farc_write::add_file(const void* data, size_t size, const std::string& file) {
    return ptr->add_file(data, size, file);
}

bool p_farc_write::open(const std::string& path, const farc_params& params) {
    return ptr->open(path, params);
}

bool p_farc_write::open(const std::string& path, bool compress, size_t align) {
    farc_params params;
    params.compress = compress;
    params.align = align;
    return ptr->open(path, params);
}

bool p_farc_write::write_file() {
    return ptr->write_file();
}

farc_ft_write_file::farc_ft_write_file() : offset(), size(), original_size(), flags() {

}

farc_ft_write_file::~farc_ft_write_file() {

}

farc_ft_write::farc_ft_write() : type(), file_handle() {
    std::random_device rd;
    mt_rand.seed(rd());

    reset();
}

farc_ft_write::~farc_ft_write() {

}

bool farc_ft_write::add_file(const void* data, size_t size, const std::string& name) {
    if (!data || !size || !name.size() || size != (uint32_t)size || !file_handle)
        return false;

    farc_ft_write_file ff;
    ff.name.assign(name);
    ff.offset = (uint32_t)ftell(file_handle);
    ff.size = (uint32_t)size;
    ff.original_size = (uint32_t)size;

    void* temp = 0;
    const void* comp_data = data;
    size_t comp_size = size;
    if (params.compress) {
        size_t ff_size = deflate::compress_bound(size, deflate::MODE_GZIP) + (name.size() + 1);
        int32_t res = deflate::compress_gzip(data, size, temp, ff_size, 9, name.c_str());
        if (ff_size < size && res >= 0) {
            comp_data = temp;
            comp_size = ff_size;

            ff.size = (uint32_t)ff_size;
            enum_or(ff.flags, FARC_GZIP);
        }
    }

    size_t act_size = 0;
    bool encrypted = false;
    if (!write_data(comp_data, comp_size, act_size, encrypted)) {
        free_def(temp);
        return false;
    }

    if (encrypted)
        enum_or(ff.flags, FARC_AES);
    else
        enum_and(ff.flags, ~FARC_AES);

    ff.size = (uint32_t)act_size;

    files.push_back(ff);
    free_def(temp);
    return true;
}

size_t farc_ft_write::get_align(size_t size) const {
    return (~(params.align - 1) & (params.align + size - 1)) - size;
}

bool farc_ft_write::open(const std::string& path, const farc_ft_params& params) {
    reset();

    if (!path.size() || !params.align || ((params.align - 1) & params.align))
        return false;

    this->path.assign(path);
    this->params = params;

    if (this->params.normal) {
        type = FARC_NORMAL;
        if (this->params.compress)
            type = FARC_COMPRESSED;
    }
    else
        type = FARC_EXTENDED;

    if (params.encrypt) {
        type = FARC_EXTENDED;

        const farc_aes_key* key = &this->params.key;
        if (!this->params.key_set)
            key = &farc_ft_key;

        aes_key = prj::shared_ptr<farc_aes_key>(new farc_aes_key);
        *aes_key.get() = *key;
    }

    data_path.assign(this->path + ".data");

    file_handle = _wfopen(utf8_to_utf16(data_path).c_str(), L"wb"); // fopen(data_path.c_str(), "wb")
    if (!file_handle)
        return false;

    return true;
}

void farc_ft_write::reset() {
    path.clear();
    params = farc_ft_params();
    type = FARC_NORMAL;
    files.clear();
    data_path.clear();
    file_handle = 0;
    aes_key.reset();
}

bool farc_ft_write::write_data(const void* data, size_t size, size_t& act_size, bool& encrypted) {
    if (!params.encrypt) {
        if (fwrite(data, size, 1, file_handle) && write_pad(get_align(size), false, file_handle)) {
            act_size = size;
            encrypted = false;
            return true;
        }
        else
            return false;
    }

    farc_buf_data src;
    src.data = (uint8_t*)data;
    src.size = size;
    src.len = size;

    std::vector<uint8_t> temp;
    size_t align = align_val(size, prj::Rijndael_Nlen) - size;
    if (!align)
        align = prj::Rijndael_Nlen;
    temp.resize(prj::Rijndael_Nlen + size + align);

    farc_buf_data dst;
    dst.data = temp.data();
    dst.len = 0;
    dst.size = temp.size();
    if (farc_ft_encrypt_data(aes_key.get(), src, dst) && fwrite(dst.data, dst.len, 1, file_handle)
        && write_pad(get_align(dst.len), true, file_handle)) {
        act_size = dst.len;
        encrypted = true;
        return true;
    }

    return false;
}

bool farc_ft_write::write_file() {
    std::string tmp_path = path + ".tmp";

    bool ret = false;
    if (file_handle) {
        fclose(file_handle);
        file_handle = 0;

        if (!path_check_file_exists(path.c_str()) || path_delete_file(path.c_str())) {
            FILE* f_dst = _wfopen(utf8_to_utf16(tmp_path).c_str(), L"wb"); // fopen(tmp_path.c_str(), "wb")
            if (f_dst && write_header(f_dst)) {
                FILE* f_src = _wfopen(utf8_to_utf16(data_path).c_str(), L"rb"); // fopen(data_path.c_str(), "rb");
                if (f_src) {
                    const size_t buf_size = 0x1000;
                    void* buf = force_malloc(buf_size);
                    if (buf) {
                        int64_t data_len = path_stat_get_file_size(data_path.c_str());
                        while (data_len > 0) {
                            size_t len = fread(buf, 1, buf_size, f_src);
                            if (!len || !fwrite(buf, len, 1, f_dst))
                                break;

                            data_len -= len;
                        }

                        if (!data_len)
                            ret = true;
                        free(buf);
                    }
                    fclose(f_src);
                }
                fclose(f_dst);
            }

            if (ret) {
                path_delete_file(path.c_str());
                if (!path_move_file(tmp_path.c_str(), path.c_str()))
                    ret = false;
            }
        }
    }

    path_delete_file(data_path.c_str());
    path_delete_file(tmp_path.c_str());
    reset();
    return ret;
}

struct farc_write_data_range {
    uint8_t* begin;
    uint8_t* end;
};

struct farc_write_data {
    uint8_t* base;
    uint8_t* begin;
    uint8_t* end;

    farc_write_data(uint8_t* data, size_t size);

    inline size_t remain_size() const;
    farc_write_data* write_range(const farc_write_data_range& range);
    inline int32_t write_uint32_t(uint32_t value);
};

inline size_t farc_write_data::remain_size() const {
    return end - begin;
}

farc_write_data* farc_write_data::write_range(const farc_write_data_range& range) {
    size_t size = min_def((size_t)(range.end - range.begin), remain_size());
    memmove(begin, range.begin, size);
    begin += size;
    return this;
}

inline int32_t farc_write_data::write_uint32_t(uint32_t value) {
    farc_write_data_range range;
    range.begin = (uint8_t*)&value;
    range.end = (uint8_t*)&value + sizeof(uint32_t);
    if (remain_size() >= sizeof(uint32_t)) {
        write_range(range);
        return sizeof(uint32_t);
    }
    else
        return -1;
}

farc_write_data::farc_write_data(uint8_t* data, size_t size) {
    base = data;
    begin = data;
    end = data + size;
}

bool farc_ft_write::write_header(FILE* f) {
    if (!f)
        return true;

    farc_flags flags = (farc_flags)0;
    if (params.compress)
        enum_or(flags, FARC_GZIP);
    else
        enum_and(flags, ~FARC_GZIP);

    if (params.encrypt)
        enum_or(flags, FARC_AES);
    else
        enum_and(flags, ~FARC_AES);

    uint32_t align = (uint32_t)params.align;

    const size_t signature_length = sizeof(uint32_t) * 2;

    size_t header_length;
    if (type < FARC_EXTENDED) {
        header_length = sizeof(uint32_t);

        size_t entry_length = sizeof(uint32_t) * 3 + 1;
        if (type != FARC_COMPRESSED)
            entry_length = sizeof(uint32_t) * 2 + 1;

        for (const farc_ft_write_file& i : files)
            header_length += entry_length + i.name.size();
    }
    else {
        header_length = sizeof(uint32_t) * 6;

        for (const farc_ft_write_file& i : files)
            header_length += sizeof(uint32_t) * 4 + 1 + i.name.size();
    }

    uint32_t header_length_pad = (uint32_t)header_length;
    bool encrypt = !!(flags & FARC_AES);
    if (encrypt) {
        size_t _header_length = header_length - signature_length;
        size_t align = align_val(_header_length, prj::Rijndael_Nlen) - _header_length;
        if (!align)
            align = prj::Rijndael_Nlen;
        header_length_pad = (uint32_t)(signature_length + _header_length + align + prj::Rijndael_Nlen);
    }

    size_t header_align = get_align(signature_length + header_length_pad);
    uint32_t data_offset = (uint32_t)(signature_length + header_length_pad + header_align);

    std::vector<uint8_t> v45;
    v45.resize(header_length - signature_length + prj::Rijndael_Nlen);

    farc_write_data write_data(v45.data(), v45.size());

    farc_signature signature = FARC_FArc;
    if (type == FARC_COMPRESSED)
        signature = FARC_FArC;
    else if (type == FARC_EXTENDED)
        signature = FARC_FARC;

    if (write_data.write_uint32_t(reverse_endianness_uint32_t(signature)) < 0)
        return false;

    if (write_data.write_uint32_t(reverse_endianness_uint32_t((uint32_t)header_length_pad)) < 0)
        return false;

    if (type < FARC_EXTENDED) {
        if (write_data.write_uint32_t(reverse_endianness_uint32_t(align)) < 0)
            return false;
    }
    else {
        if (write_data.write_uint32_t(reverse_endianness_uint32_t(flags)) < 0)
            return false;

        if (write_data.write_uint32_t(reverse_endianness_uint32_t(0)) < 0)
            return false;

        if (write_data.write_uint32_t(reverse_endianness_uint32_t((uint32_t)prj::Rijndael_Nlen)) < 0)
            return false;

        if (write_data.write_uint32_t(reverse_endianness_uint32_t(align)) < 0)
            return false;

        if (write_data.write_uint32_t(reverse_endianness_uint32_t((uint32_t)files.size())) < 0)
            return false;

        if (write_data.write_uint32_t(reverse_endianness_uint32_t((uint32_t)(sizeof(uint32_t) * 4))) < 0)
            return false;
    }

    for (farc_ft_write_file& i : files) {
        write_data.write_range({ (uint8_t*)i.name.data(), (uint8_t*)i.name.data() + i.name.size() + 1 });

        if (write_data.write_uint32_t(reverse_endianness_uint32_t(data_offset + i.offset)) < 0)
            return false;

        if (write_data.write_uint32_t(reverse_endianness_uint32_t(i.size)) < 0)
            return false;

        if (type >= FARC_EXTENDED) {
            if (write_data.write_uint32_t(reverse_endianness_uint32_t(i.original_size)) < 0)
                return false;

            if (write_data.write_uint32_t(reverse_endianness_uint32_t(i.flags)) < 0)
                return false;
        }
        else if (type >= FARC_COMPRESSED) {
            uint32_t original_size;
            if (i.flags & FARC_GZIP)
                original_size = i.original_size;
            else
                original_size = 0;

            if (write_data.write_uint32_t(reverse_endianness_uint32_t(original_size)) < 0)
                return false;
        }
    }

    if (encrypt) {
        if (fwrite(write_data.base, prj::Rijndael_Nlen, 1, f)) {
            farc_buf_data src;
            src.data = write_data.base + prj::Rijndael_Nlen;
            src.size = write_data.begin - write_data.base - prj::Rijndael_Nlen;
            src.len = src.size;

            std::vector<uint8_t> temp;
            temp.resize(header_length_pad - signature_length);

            farc_buf_data dst;
            dst.data = temp.data();
            dst.len = 0;
            dst.size = temp.size();
            if (!farc_ft_encrypt_data(aes_key.get(), src, dst) || dst.len != dst.size)
                return false;

            if (!fwrite(dst.data, dst.len, 1, f))
                return false;

            if (write_pad(header_align, encrypt, file_handle))
                return true;
        }
    }

    if (!fwrite(write_data.base, write_data.begin - write_data.base, 1, f))
        return false;

    if (write_pad(header_align, encrypt, file_handle))
        return true;

    return false;
}

bool farc_ft_write::write_pad(size_t align, bool random, FILE* f) {
    for (size_t offset = 0; offset < align; offset += sizeof(uint32_t)) {
        uint32_t pad;
        if (random)
            pad = reverse_endianness_uint32_t(mt_rand());
        else
            pad = 0x78787878;

        size_t size = min_def(align - offset, sizeof(uint32_t));
        if (!fwrite(&pad, size, 1, f))
            return false;
    }
    return true;
}

p_farc_ft_write::p_farc_ft_write() : ptr() {
    ptr = new farc_ft_write;
}

p_farc_ft_write::~p_farc_ft_write() {
    if (ptr) {
        delete ptr;
        ptr = 0;
    }
}

bool p_farc_ft_write::add_file(const void* data, size_t size, const std::string& file) {
    return ptr->add_file(data, size, file);
}

bool p_farc_ft_write::open(const std::string& path, const farc_ft_params& params) {
    return ptr->open(path, params);
}

bool p_farc_ft_write::open(const std::string& path, bool compress, size_t align) {
    farc_ft_params params;
    params.compress = compress;
    params.align = align;
    return ptr->open(path, params);
}

bool p_farc_ft_write::write_file() {
    return ptr->write_file();
}

static size_t check_pkcs7_padding(const uint8_t* buff16) {
    size_t pad = buff16[prj::Rijndael_Nlen - 1];
    const uint8_t* end = buff16 + prj::Rijndael_Nlen;
    if ((int64_t)pad - 1 > prj::Rijndael_Nlen - 1)
        return 0;

    for (const uint8_t* ptr = end - pad; ptr != end; ptr++)
        if (*ptr != pad)
            return 0;
    return pad;
}

static bool farc_ft_decrypt_data(const farc_aes_key* key, const farc_buf_data& src, farc_buf_data& dst) {
    prj::Rijndael rijndael(prj::Rijndael_Nb, prj::Rijndael_Nk128, key->data);

    if (src.len < prj::Rijndael_Nlen)
        return false;

    uint8_t iv[prj::Rijndael_Nlen];
    memcpy(iv, src.data, prj::Rijndael_Nlen);
    size_t offset = min_def(src.len, prj::Rijndael_Nlen);

    size_t src_len = src.len - offset;
    size_t dst_len = dst.size;
    if (!dst_len)
        return false;

    if (src_len % prj::Rijndael_Nlen)
        return false;

    size_t src_block_count = src_len / prj::Rijndael_Nlen - 1;
    size_t src_len_align = src_block_count * prj::Rijndael_Nlen;
    if (dst_len < src_len_align)
        return false;

    memcpy(dst.data, src.data + offset, src_len_align + prj::Rijndael_Nlen);

    uint8_t next_iv[prj::Rijndael_Nlen];
    for (size_t i = 0; i < src_len_align; i += prj::Rijndael_Nlen) {
        memcpy(next_iv, dst.data + i, prj::Rijndael_Nlen);
        rijndael.decrypt16(dst.data + i);
        for (uint32_t j = 0; j < prj::Rijndael_Nlen / sizeof(uint32_t); j++)
            ((uint32_t*)(dst.data + i))[j] ^= ((uint32_t*)iv)[j];
        memcpy(iv, next_iv, prj::Rijndael_Nlen);
    }

    rijndael.decrypt16(dst.data + src_len_align);
    for (uint32_t j = 0; j < prj::Rijndael_Nlen / sizeof(uint32_t); j++)
        ((uint32_t*)(dst.data + src_len_align))[j] ^= ((uint32_t*)iv)[j];

    size_t pad = check_pkcs7_padding(dst.data + src_len_align);

    if (dst_len < src_len_align + prj::Rijndael_Nlen - pad)
        return false;

    dst_len = src_len - pad;

    size_t size = dst.size;
    if (dst.size < dst_len)
        return false;

    dst.len = dst_len;
    return true;
}

static bool farc_ft_encrypt_data(const farc_aes_key* key, const farc_buf_data& src, farc_buf_data& dst) {
    prj::Rijndael rijndael(prj::Rijndael_Nb, prj::Rijndael_Nk128, key->data);

    if (dst.size < prj::Rijndael_Nk128 * sizeof(uint32_t))
        return false;

    std::random_device rd;
    std::mt19937 mt_rand;
    mt_rand.seed(rd());

    uint8_t iv[prj::Rijndael_Nlen];
    for (int32_t i = 0; i < prj::Rijndael_Nlen; i += sizeof(uint32_t))
        store_reverse_endianness_uint32_t(&iv[i], mt_rand());

    memcpy(dst.data, iv, prj::Rijndael_Nlen);

    size_t offset = prj::Rijndael_Nlen;
    size_t src_len = src.len;
    size_t dst_len = dst.size;
    if (!dst_len || dst_len < prj::Rijndael_Nlen)
        return false;

    dst_len -= prj::Rijndael_Nlen;

    size_t src_block_count = src_len / prj::Rijndael_Nlen;
    size_t src_len_align = src_block_count * prj::Rijndael_Nlen;
    if (dst_len < src_len_align)
        return false;

    size_t pad = prj::Rijndael_Nlen;
    if (src_len % prj::Rijndael_Nlen)
        pad = prj::Rijndael_Nlen - (src_len - src_len_align);
    if (dst_len < src_len + pad)
        return false;

    dst_len = src_len + pad;

    memcpy(dst.data + offset, src.data, src_len);
    for (size_t i = 0; i < src_len_align; i += prj::Rijndael_Nlen) {
        for (uint32_t j = 0; j < prj::Rijndael_Nlen / sizeof(uint32_t); j++)
            ((uint32_t*)(dst.data + offset + i))[j] ^= ((uint32_t*)iv)[j];
        rijndael.encrypt16(dst.data + offset + i);
        memcpy(iv, dst.data + offset + i, prj::Rijndael_Nlen);
    }

    memset(dst.data + offset + src_len, (uint8_t)pad, pad);
    for (uint32_t i = 0; i < prj::Rijndael_Nlen / sizeof(uint32_t); i++)
        ((uint32_t*)(dst.data + offset + src_len_align))[i] ^= ((uint32_t*)iv)[i];
    rijndael.encrypt16(dst.data + offset + src_len_align);
    memcpy(iv, dst.data + offset + src_len_align, prj::Rijndael_Nlen);

    if (dst.size < dst_len)
        return false;

    dst.len = dst_len + prj::Rijndael_Nlen;
    return true;
}
