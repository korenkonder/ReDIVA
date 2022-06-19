/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "pv_exp.hpp"
#include "f2/struct.hpp"
#include "io/path.hpp"
#include "io/stream.hpp"
#include "str_utils.hpp"

static void pv_exp_classic_read_inner(pv_exp* exp, stream& s);
static void pv_exp_classic_write_inner(pv_exp* exp, stream& s);
static void pv_exp_modern_read_inner(pv_exp* exp, stream& s, uint32_t header_length);
static void pv_exp_modern_write_inner(pv_exp* exp, stream& s);

pv_exp_mot::pv_exp_mot() : face_data(), face_cl_data() {

}

pv_exp_mot::~pv_exp_mot() {
    if (face_data) {
        delete[] face_data;
        face_data = 0;
    }

    if (face_cl_data) {
        delete[] face_cl_data;
        face_cl_data = 0;
    }
}

pv_exp::pv_exp() : ready(), modern(), is_x() {

}

pv_exp::~pv_exp() {

}

void pv_exp::read(const char* path, bool modern) {
    if (!path)
        return;

    if (!modern) {
        char* path_bin = str_utils_add(path, ".bin");
        if (path_check_file_exists(path_bin)) {
            stream s;
            s.open(path_bin, "rb");
            if (s.io.stream) {
                uint8_t* data = force_malloc_s(uint8_t, s.length);
                s.read(data, s.length);
                stream s_bin;
                s_bin.open(data, s.length);
                pv_exp_classic_read_inner(this, s_bin);
                free(data);
            }
        }
        free(path_bin);
    }
    else {
        char* path_dex = str_utils_add(path, ".dex");
        if (path_check_file_exists(path_dex)) {
            f2_struct st;
            st.read(path_dex);
            if (st.header.signature == reverse_endianness_uint32_t('EXPC')) {
                stream s_expc;
                s_expc.open(st.data);
                s_expc.is_big_endian = st.header.use_big_endian;
                pv_exp_modern_read_inner(this, s_expc, st.header.length);
            }
        }
        free(path_dex);
    }
}

void pv_exp::read(const wchar_t* path, bool modern) {
    if (!path)
        return;

    if (!modern) {
        wchar_t* path_bin = str_utils_add(path, L".bin");
        if (path_check_file_exists(path_bin)) {
            stream s;
            s.open(path_bin, L"rb");
            if (s.io.stream) {
                uint8_t* data = force_malloc_s(uint8_t, s.length);
                s.read(data, s.length);
                stream s_bin;
                s_bin.open(data, s.length);
                pv_exp_classic_read_inner(this, s_bin);
                free(data);
            }
        }
        free(path_bin);
    }
    else {
        wchar_t* path_dex = str_utils_add(path, L".dex");
        if (path_check_file_exists(path_dex)) {
            f2_struct st;
            st.read(path_dex);
            if (st.header.signature == reverse_endianness_uint32_t('EXPC')) {
                stream s_expc;
                s_expc.open(st.data);
                s_expc.is_big_endian = st.header.use_big_endian;
                pv_exp_modern_read_inner(this, s_expc, st.header.length);
            }
        }
        free(path_dex);
    }
}

void pv_exp::read(const void* data, size_t size, bool modern) {
    if (!data || !size)
        return;

    if (!modern) {
        stream s;
        s.open(data, size);
        pv_exp_classic_read_inner(this, s);
    }
    else {
        f2_struct st;
        st.read(data, size);
        if (st.header.signature == reverse_endianness_uint32_t('EXPC')) {
            stream s_expc;
            s_expc.open(st.data);
            s_expc.is_big_endian = st.header.use_big_endian;
            pv_exp_modern_read_inner(this, s_expc, st.header.length);
        }
    }
}

void pv_exp::write(const char* path) {
    if (!path || !ready)
        return;

    if (!modern) {
        char* path_bin = str_utils_add(path, ".bin");
        stream s;
        s.open(path_bin, "wb");
        if (s.io.stream)
            pv_exp_classic_write_inner(this, s);
        free(path_bin);
    }
    else {
        char* path_dex = str_utils_add(path, ".dex");
        stream s;
        s.open(path_dex, "wb");
        if (s.io.stream)
            pv_exp_modern_write_inner(this, s);
        free(path_dex);
    }
}

void pv_exp::write(const wchar_t* path) {
    if (!path || !ready)
        return;

    if (!modern) {
        wchar_t* path_bin = str_utils_add(path, L".bin");
        stream s;
        s.open(path_bin, L"wb");
        if (s.io.stream)
            pv_exp_classic_write_inner(this, s);
        free(path_bin);
    }
    else {
        wchar_t* path_dex = str_utils_add(path, L".dex");
        stream s;
        s.open(path_dex, L"wb");
        if (s.io.stream)
            pv_exp_modern_write_inner(this, s);
        free(path_dex);
    }
}

void pv_exp::write(void** data, size_t* size) {
    if (!data || !size || !ready)
        return;

    stream s;
    s.open();
    if (!modern)
        pv_exp_classic_write_inner(this, s);
    else
        pv_exp_modern_write_inner(this, s);
    s.align_write(0x10);
    s.copy(data, size);
}

bool pv_exp::load_file(void* data, const char* path, const char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    const char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    std::string s = path + std::string(file, file_len);

    pv_exp* exp = (pv_exp*)data;
    exp->read(s.c_str(), exp->modern);

    return exp->ready;
}

static void pv_exp_classic_read_inner(pv_exp* exp, stream& s) {

}

static void pv_exp_classic_write_inner(pv_exp* exp, stream& s) {

}

static void pv_exp_modern_read_inner(pv_exp* exp, stream& s, uint32_t header_length) {

}

static void pv_exp_modern_write_inner(pv_exp* exp, stream& s) {

}