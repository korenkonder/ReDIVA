/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "data.hpp"
#include "../KKdLib/io/path.hpp"
#include "../KKdLib/io/stream.hpp"
#include "../KKdLib/hash.hpp"
#include "../KKdLib/str_utils.hpp"

data_struct* data_list;

static char default_config[] = "#AFT\n\n\n";

static void data_free_inner(data_struct* ds);
static void data_load_inner(stream& s);
#if defined(CRE_DEV)
static void data_load_glitter_list(data_struct* ds, const char* path);
#endif

void data_struct_init() {
    data_list = new data_struct[8];
    for (int32_t i = DATA_AFT; i < DATA_MAX; i++)
        data_list[i].type = (data_type)i;
}

void data_struct_load(const char* path) {
    stream s;
    s.open(path, "rb");
    if (s.io.stream)
        data_load_inner(s);
    else {
        s.close();
        s.open(default_config, 7);
        data_load_inner(s);
    }
}

void data_struct_load(const wchar_t* path) {
    stream s;
    s.open(path, L"rb");
    if (s.io.stream)
        data_load_inner(s);
    else {
        s.close();
        s.open(default_config, 7);
        data_load_inner(s);
    }
}

void data_struct_free() {
    if (data_list[DATA_AFT].ready) {
        data_struct* ds = &data_list[DATA_AFT];
        data_ft* d = &ds->data_ft;
    }
#if defined(CRE_DEV)
    if (data_list[DATA_F2BE].ready) {
        data_struct* ds = &data_list[DATA_F2BE];
        data_f2* d = &ds->data_f2;
    }

    if (data_list[DATA_F2LE].ready) {
        data_struct* ds = &data_list[DATA_F2LE];
        data_f2* d = &ds->data_f2;
    }

    if (data_list[DATA_FT].ready) {
        data_struct* ds = &data_list[DATA_FT];
        data_ft* d = &ds->data_ft;
    }

    if (data_list[DATA_M39].ready) {
        data_struct* ds = &data_list[DATA_M39];
        data_ft* d = &ds->data_ft;
    }

    if (data_list[DATA_VRFL].ready) {
        data_struct* ds = &data_list[DATA_VRFL];
        data_x* d = &ds->data_x;
    }

    if (data_list[DATA_X].ready) {
        data_struct* ds = &data_list[DATA_X];
        data_x* d = &ds->data_x;
    }

    if (data_list[DATA_XHD].ready) {
        data_struct* ds = &data_list[DATA_XHD];
        data_x* d = &ds->data_x;
    }
#endif

    for (int32_t i = DATA_AFT; i < DATA_MAX; i++)
        data_free_inner(&data_list[i]);
    delete[] data_list;
}

bool data_struct::check_file_exists(const char* path) {
    const char* t = strrchr(path, '/');
    if (t) {
        std::string dir = std::string(path, t - path + 1);
        return check_file_exists(dir.c_str(), t + 1);
    }

    t = strrchr(path, '\\');
    if (t) {
        std::string dir = std::string(path, t - path + 1);
        return check_file_exists(dir.c_str(), t + 1);
    }
    return false;
}

bool data_struct::check_file_exists(const char* dir, const char* file) {
    if (path_check_directory_exists(dir)) {
        const char* t = strrchr(file, '.');
        size_t t_len;
        if (t)
            t_len = t - file;
        else
            t_len = utf8_length(file);

        uint32_t h = hash_murmurhash(file, t_len, 0, false, false);
        std::string path = std::string(dir) + file;
        if (path_check_file_exists(path.c_str()))
            return true;
    }

    size_t dir_len = utf8_length(dir);
    if (dir_len >= 2 && !memcmp(dir, "./", 2)) {
        dir += 2;
        dir_len -= 2;
    }

    bool f2 = false;
    if (dir_len >= 5 && !memcmp(dir, "root+", 5)) {
        dir += 5;
        dir_len -= 5;
        f2 = true;
    }
    else if (dir_len >= 3 && !memcmp(dir, "rom", 3)) {
        dir += 3;
        dir_len -= 3;
    }
    else
        return false;

    while (*dir == '/' || *dir == '\\') {
        dir++;
        dir_len--;
    }

    size_t max_len = 0;
    for (data_struct_path& i : data_paths)
        for (data_struct_directory& j : i.data)
            if (max_len < j.path.size())
                max_len = j.path.size();

    char* dir_temp = force_malloc_s(char, dir_len + 1);
    memcpy(dir_temp, dir, dir_len + 1);

    char* t = dir_temp;
    while (t = strchr(t, '/'))
        *t = '\\';

    char* temp = force_malloc_s(char, max_len + dir_len + 2);
    for (data_struct_path& i : data_paths)
        for (data_struct_directory& j : i.data) {
            const char* path = j.path.c_str();
            size_t path_len = j.path.size();

            memcpy(temp, path, path_len);
            memcpy(&temp[path_len], "\\", 2);
            if (dir_len)
                memcpy(&temp[path_len + 1], dir_temp, dir_len + 1);

            std::string path_temp = std::string(temp) + file;
            if (path_check_file_exists(path_temp.c_str())) {
                free(dir_temp);
                free(temp);
                return true;
            }
        }

    free(dir_temp);
    free(temp);

    if (f2)
        return check_file_exists("rom/data/", file);
    return false;
}

bool data_struct::check_file_exists(const char* dir, uint32_t hash) {
    size_t dir_len = utf8_length(dir);
    if (dir_len >= 2 && !memcmp(dir, "./", 2)) {
        dir += 2;
        dir_len -= 2;
    }

    bool f2 = false;
    if (dir_len >= 5 && !memcmp(dir, "root+", 5)) {
        dir += 5;
        dir_len -= 5;
        f2 = true;
    }
    else if (dir_len >= 3 && !memcmp(dir, "rom", 3)) {
        dir += 3;
        dir_len -= 3;
    }
    else
        return false;

    while (*dir == '/' || *dir == '\\') {
        dir++;
        dir_len--;
    }

    size_t max_len = 0;
    for (data_struct_path& i : data_paths)
        for (data_struct_directory& j : i.data)
            if (max_len < j.path.size())
                max_len = j.path.size();

    char* dir_temp = force_malloc_s(char, dir_len + 1);
    memcpy(dir_temp, dir, dir_len + 1);

    char* t = dir_temp;
    while (t = strchr(t, '/'))
        *t = '\\';

    char* temp = force_malloc_s(char, max_len + dir_len + 2);
    for (data_struct_path& i : data_paths)
        for (data_struct_directory& j : i.data) {
            const char* path = j.path.c_str();
            size_t path_len = j.path.size();

            memcpy(temp, path, path_len);
            memcpy(&temp[path_len], "\\", 2);
            if (dir_len)
                memcpy(&temp[path_len + 1], dir_temp, dir_len + 1);

            std::vector<std::string> files;
            path_get_files(&files, temp);
            if (!files.size())
                continue;

            std::vector<uint32_t> files_murmurhash;
            files_murmurhash.reserve(files.size());
            for (std::string& l : files) {
                const char* t = strrchr(l.c_str(), '.');
                size_t t_len = l.size();
                if (t)
                    t_len = t - l.c_str();
                t = l.c_str();

                files_murmurhash.push_back(hash_murmurhash(t, t_len));
            }

            for (uint32_t& l : files_murmurhash)
                if (l == hash) {
                    free(dir_temp);
                    free(temp);
                    return true;
                }
        }

    free(dir_temp);
    free(temp);

    if (f2)
        return check_file_exists("rom/data/", hash);
    return false;
}

void data_struct::get_directory_files(const char* dir, std::vector<data_struct_file>& data_files) {
    data_files.clear();
    data_files.shrink_to_fit();

    size_t dir_len = utf8_length(dir);
    if (dir_len >= 2 && !memcmp(dir, "./", 2)) {
        dir += 2;
        dir_len -= 2;
    }

    bool f2 = false;
    if (dir_len >= 5 && !memcmp(dir, "root+", 5)) {
        dir += 5;
        dir_len -= 5;
        f2 = true;
    }
    else if (dir_len >= 3 && !memcmp(dir, "rom", 3)) {
        dir += 3;
        dir_len -= 3;
    }
    else
        return;

    while (*dir == '/' || *dir == '\\') {
        dir++;
        dir_len--;
    }

    size_t max_len = 0;
    for (data_struct_path& i : data_paths)
        for (data_struct_directory& j : i.data)
            if (max_len < j.path.size())
                max_len = j.path.size();

    char* dir_temp = force_malloc_s(char, dir_len + 1);
    memcpy(dir_temp, dir, dir_len + 1);

    char* t = dir_temp;
    while (t = strchr(t, '/'))
        *t = '\\';

    char* temp = force_malloc_s(char, max_len + dir_len + 2);
    for (data_struct_path& i : data_paths)
        for (data_struct_directory& j : i.data) {
            const char* path = j.path.c_str();
            size_t path_len = j.path.size();

            memcpy(temp, path, path_len);
            memcpy(&temp[path_len], "\\", 2);
            if (dir_len)
                memcpy(&temp[path_len + 1], dir_temp, dir_len + 1);

            std::vector<std::string> files;
            path_get_files(&files, temp);
            if (!files.size())
                continue;

            for (std::string& k : files) {
                bool found = false;
                for (data_struct_file& l : data_files)
                    if (k == l.name) {
                        found = true;
                        break;
                    }

                if (found)
                    continue;

                data_struct_file f;
                f.path = temp;
                f.name = k;
                data_files.push_back(f);
            }
        }

    free(dir_temp);
    free(temp);
}

bool data_struct::get_file(const char* dir, uint32_t hash, const char* ext, std::string& file) {
    size_t dir_len = utf8_length(dir);
    if (dir_len >= 2 && !memcmp(dir, "./", 2)) {
        dir += 2;
        dir_len -= 2;
    }

    bool f2 = false;
    if (dir_len >= 5 && !memcmp(dir, "root+", 5)) {
        dir += 5;
        dir_len -= 5;
        f2 = true;
    }
    else if (dir_len >= 3 && !memcmp(dir, "rom", 3)) {
        dir += 3;
        dir_len -= 3;
    }
    else
        return false;

    while (*dir == '/' || *dir == '\\') {
        dir++;
        dir_len--;
    }

    size_t max_len = 0;
    for (data_struct_path& i : data_paths)
        for (data_struct_directory& j : i.data)
            if (max_len < j.path.size())
                max_len = j.path.size();

    char* dir_temp = force_malloc_s(char, dir_len + 1);
    memcpy(dir_temp, dir, dir_len + 1);

    char* t = dir_temp;
    while (t = strchr(t, '/'))
        *t = '\\';

    char* temp = force_malloc_s(char, max_len + dir_len + 2);
    for (data_struct_path& i : data_paths)
        for (data_struct_directory& j : i.data) {
            const char* path = j.path.c_str();
            size_t path_len = j.path.size();

            memcpy(temp, path, path_len);
            memcpy(&temp[path_len], "\\", 2);
            if (dir_len)
                memcpy(&temp[path_len + 1], dir_temp, dir_len + 1);

            std::vector<std::string> files;
            path_get_files(&files, temp);
            if (!files.size())
                continue;

            for (std::string& l : files) {
                const char* t = strrchr(l.c_str(), '.');
                if (str_utils_compare(t, ext))
                    continue;

                size_t t_len = l.size();
                if (t)
                    t_len = t - l.c_str();
                t = l.c_str();

                if (hash_murmurhash(t, t_len) == hash) {
                    file = l;
                    free(dir_temp);
                    free(temp);
                    return true;
                }
            }
        }

    free(dir_temp);
    free(temp);

    if (f2)
        return get_file("rom/data/", hash, ext, file);
    return false;
}

bool data_struct::load_file(void* data, const char* path,
    bool (*load_func)(void* data, const char* path, const  char* file, uint32_t hash)) {
    const char* t = strrchr(path, '/');
    if (t) {
        std::string dir = std::string(path, t - path + 1);
        return load_file(data, dir.c_str(), t + 1, load_func);
    }

    t = strrchr(path, '\\');
    if (t) {
        std::string dir = std::string(path, t - path + 1);
        return load_file(data, dir.c_str(), t + 1, load_func);
    }
    return false;
}

bool data_struct::load_file(void* data, const char* dir, const char* file,
    bool (*load_func)(void* data, const char* path, const char* file, uint32_t hash)) {
    if (path_check_directory_exists(dir)) {
        const char* t = strrchr(file, '.');
        size_t t_len;
        if (t)
            t_len = t - file;
        else
            t_len = utf8_length(file);

        uint32_t h = hash_murmurhash(file, t_len);
        bool ret = load_func(data, dir, file, h);
        if (ret)
            return true;
    }

    size_t dir_len = utf8_length(dir);
    if (dir_len >= 2 && !memcmp(dir, "./", 2)) {
        dir += 2;
        dir_len -= 2;
    }

    bool f2 = false;
    if (dir_len >= 5 && !memcmp(dir, "root+", 5)) {
        dir += 5;
        dir_len -= 5;
        f2 = true;
    }
    else if (dir_len >= 3 && !memcmp(dir, "rom", 3)) {
        dir += 3;
        dir_len -= 3;
    }
    else
        return false;

    while (*dir == '/' || *dir == '\\') {
        dir++;
        dir_len--;
    }

    size_t max_len = 0;
    for (data_struct_path& i : data_paths)
        for (data_struct_directory& j : i.data)
            if (max_len < j.path.size())
                max_len = j.path.size();

    char* dir_temp = force_malloc_s(char, dir_len + 1);
    memcpy(dir_temp, dir, dir_len + 1);

    char* t = dir_temp;
    while(t = strchr(t, '/'))
        *t = '\\';

    char* temp = force_malloc_s(char, max_len + dir_len + 2);
    for (data_struct_path& i : data_paths)
        for (data_struct_directory& j : i.data) {
            const char* path = j.path.c_str();
            size_t path_len = j.path.size();

            memcpy(temp, path, path_len);
            memcpy(&temp[path_len], "\\", 2);
            if (dir_len)
                memcpy(&temp[path_len + 1], dir_temp, dir_len + 1);

            std::string path_temp = std::string(temp) + file;
            if (path_check_file_exists(path_temp.c_str())) {
                const char* l_str = file;
                const char* t = strrchr(l_str, '.');
                size_t l_len = utf8_length(file);
                if (t)
                    l_len = t - l_str;

                uint32_t h = hash_murmurhash(l_str, l_len);
                if (load_func(data, temp, l_str, h)) {
                    free(dir_temp);
                    free(temp);
                    return true;
                }
            }
        }

    free(dir_temp);
    free(temp);

    if (f2)
        return load_file(data, "rom/data/", file, load_func);
    return false;
}

bool data_struct::load_file(void* data, const char* dir, uint32_t hash, const char* ext,
    bool (*load_func)(void* data, const char* path, const char* file, uint32_t hash)) {
    size_t dir_len = utf8_length(dir);
    if (dir_len >= 2 && !memcmp(dir, "./", 2)) {
        dir += 2;
        dir_len -= 2;
    }

    bool f2 = false;
    if (dir_len >= 5 && !memcmp(dir, "root+", 5)) {
        dir += 5;
        dir_len -= 5;
        f2 = true;
    }
    else if (dir_len >= 3 && !memcmp(dir, "rom", 3)) {
        dir += 3;
        dir_len -= 3;
    }
    else
        return false;

    dir += 3;
    dir_len -= 3;

    while (*dir == '/' || *dir == '\\') {
        dir++;
        dir_len--;
    }

    size_t max_len = 0;
    for (data_struct_path& i : data_paths)
        for (data_struct_directory& j : i.data)
            if (max_len < j.path.size())
                max_len = j.path.size();

    char* dir_temp = force_malloc_s(char, dir_len + 1);
    memcpy(dir_temp, dir, dir_len + 1);

    char* t = dir_temp;
    while (t = strchr(t, '/'))
        *t = '\\';

    char* temp = force_malloc_s(char, max_len + dir_len + 2);
    for (data_struct_path& i : data_paths)
        for (data_struct_directory& j : i.data) {
            const char* path = j.path.c_str();
            size_t path_len = j.path.size();

            memcpy(temp, path, path_len);
            memcpy(&temp[path_len], "\\", 2);
            if (dir_len)
                memcpy(&temp[path_len + 1], dir_temp, dir_len + 1);

            std::vector<std::string> files;
            path_get_files(&files, temp);
            if (!files.size())
                continue;

            for (std::string& l : files) {
                const char* t = strrchr(l.c_str(), '.');
                if (str_utils_compare(t, ext))
                    continue;

                size_t t_len = l.size();
                if (t)
                    t_len = t - l.c_str();
                t = l.c_str();

                if (hash_murmurhash(t, t_len) == hash
                    && load_func(data, temp, l.c_str(), hash)) {
                    free(dir_temp);
                    free(temp);
                    return true;
                }
            }
        }

    free(dir_temp);
    free(temp);

    if (f2)
        return load_file(data, "rom/data/", hash, ext, load_func);
    return false;
}

data_struct::data_struct() : type(), ready() {

}

data_struct::~data_struct() {

}

data_struct_file::data_struct_file() {

}

data_struct_file::~data_struct_file() {

}

data_struct_directory::data_struct_directory() {

}

data_struct_directory::~data_struct_directory() {

}

data_struct_path::data_struct_path() {

}

data_struct_path::~data_struct_path() {

}

#if defined(CRE_DEV)
data_f2::data_f2() {

}

data_f2::~data_f2() {

}
#endif

data_ft::data_ft() {

}

data_ft::~data_ft() {

}

#if defined(CRE_DEV)
data_x::data_x() {

}

data_x::~data_x() {

}
#endif

static void data_free_inner(data_struct* ds) {
    ds->data_paths.clear();
    ds->data_paths.shrink_to_fit();

#if defined(CRE_DEV)
    ds->glitter_list_names.clear();
    ds->glitter_list_names.shrink_to_fit();
    switch (ds->type) {
    case DATA_F2LE:
    case DATA_F2BE:
    case DATA_VRFL:
    case DATA_X:
    case DATA_XHD:
        ds->glitter_list_murmurhash.clear();
        ds->glitter_list_murmurhash.shrink_to_fit();
        break;
    case DATA_AFT:
    case DATA_FT:
    case DATA_M39:
        ds->glitter_list_fnv1a64m.clear();
        ds->glitter_list_fnv1a64m.shrink_to_fit();
        break;
    }
#endif
    ds->ready = false;
}

static void data_load_inner(stream& s) {
    size_t length = s.length;
    void* data = force_malloc(length);
    s.read(data, length);

    char* buf;
    char** lines;
    size_t count;
    if (!str_utils_text_file_parse(data, length, &buf, &lines, &count)) {
        free(data);
        return;
    }
    free(data);

    for (size_t i = 0; i < count; i++) {
        char* s = lines[i];
        data_struct* ds = 0;
        const char* main_rom = "rom";
        const char* add_data_rom = "rom";
        const char* add_data = "";
        if (!str_utils_compare(s, "#AFT")) {
            ds = &data_list[DATA_AFT];
            add_data = "mdata";
        }
#if defined(CRE_DEV)
        else if (!str_utils_compare(s, "#F2BE")) {
            ds = &data_list[DATA_F2BE];
            add_data = "dlc";
        }
        else if (!str_utils_compare(s, "#F2LE")) {
            ds = &data_list[DATA_F2LE];
            add_data = "dlc";
        }
        else if (!str_utils_compare(s, "#FT")) {
            ds = &data_list[DATA_FT];
            add_data = "mdata";
        }
        else if (!str_utils_compare(s, "#M39")) {
            ds = &data_list[DATA_M39];
            add_data = "rom_switch_dlc";
        }
        else if (!str_utils_compare(s, "#VRFL")) {
            ds = &data_list[DATA_VRFL];
            main_rom = "cnt_rom";
            add_data_rom = "";
        }
        else if (!str_utils_compare(s, "#X")) {
            ds = &data_list[DATA_X];
            main_rom = "cnt_rom";
            add_data_rom = "";
        }
        else if (!str_utils_compare(s, "#XHD")) {
            ds = &data_list[DATA_XHD];
            main_rom = "cnt_rom";
            add_data_rom = "";
        }
#endif
        else
            continue;

        data_free_inner(ds);

#if defined(CRE_DEV)
        if (i + 2 >= count)
            continue;

        char* data_paths = lines[i + 1];
        char* glitter_list = lines[i + 2];
        i++;

        if (!str_utils_check_ends_with(glitter_list, ".glitter.txt"))
            continue;

        data_load_glitter_list(ds, glitter_list);
#else
        char* data_paths;
        if (i + 1 < count)
            data_paths = lines[i + 1];
        else if (i < count)
            data_paths = (char*)".";
        else
            continue;
        i++;
#endif

        char* t = data_paths;
        size_t count = 1;
        while (t = strchr(t, ';')) {
            *t++ = 0;
            count++;
        }

        size_t max_len = 0;
        t = data_paths;
        for (size_t j = 0; j < count; j++) {
            size_t len = utf8_length(t);
            if (max_len < len)
                max_len = len;
            t += len + 1;
        }

        size_t main_rom_len = utf8_length(main_rom);
        size_t add_data_len = utf8_length(add_data);
        size_t add_data_rom_len = utf8_length(add_data_rom);

        char* main_rom_path = force_malloc_s(char, max_len + main_rom_len + 3);
        if (!main_rom_path) {
            data_free_inner(ds);
            continue;
        }

        char* add_data_rom_path = force_malloc_s(char, max_len + add_data_len + 3);
        if (!add_data_rom_path) {
            free(main_rom_path);
            data_free_inner(ds);
            continue;
        }

        ssize_t* t_len = force_malloc_s(ssize_t, count);
        if (!t_len) {
            free(main_rom_path);
            free(add_data_rom_path);
            data_free_inner(ds);
            continue;
        }

        t = data_paths;
        for (size_t j = 0; j < count; j++)
            t += (t_len[j] = utf8_length(t)) + 1;

        ds->data_paths.reserve(count);
        for (size_t j = 0; j < count; j++) {
            data_struct_path data_path;
            t -= t_len[count - j - 1] + 1;
            data_path.path = std::string(t, t_len[count - j - 1]);
            data_path.data = {};
            ds->data_paths.push_back(data_path);
        }
        free(t_len);

        for (data_struct_path& j : ds->data_paths) {
            const char* data_path = j.path.c_str();
            size_t data_path_length = j.path.size();
            if (!main_rom_len)
                memcpy(main_rom_path, data_path, data_path_length);
            else if (data_path_length) {
                memcpy(main_rom_path, data_path, data_path_length);
                main_rom_path[data_path_length] = '\\';
                memcpy(&main_rom_path[data_path_length + 1], main_rom, main_rom_len + 1);
            }
            else
                memcpy(main_rom_path, main_rom, main_rom_len + 1);

            if (!add_data_len)
                memcpy(add_data_rom_path, data_path, data_path_length);
            else if (data_path_length) {
                memcpy(add_data_rom_path, data_path, data_path_length);
                add_data_rom_path[data_path_length] = '\\';
                memcpy(&add_data_rom_path[data_path_length + 1], add_data, add_data_len + 1);
            }
            else
                memcpy(add_data_rom_path, add_data, add_data_len + 1);

            size_t main_rom_path_len = utf8_length(main_rom_path);
            if (main_rom_path_len && main_rom_path[main_rom_path_len - 1] == '\\') {
                main_rom_path[main_rom_path_len - 1] = 0;
                main_rom_path_len--;
            }

            size_t add_data_rom_path_len = utf8_length(add_data_rom_path);
            if (add_data_rom_path_len && add_data_rom_path[add_data_rom_path_len - 1] == '\\') {
                add_data_rom_path[add_data_rom_path_len - 1] = 0;
                add_data_rom_path_len--;
            }

            std::vector<std::string> data_directories;
            path_get_directories(&data_directories, add_data_rom_path, &main_rom_path, 1);

            max_len = 0;
            for (std::string& k : data_directories) {
                size_t len = k.size();
                if (max_len < len)
                    max_len = len;
            }

            char* data_dir_path_temp = force_malloc_s(char, add_data_rom_path_len + max_len + add_data_rom_len + 3);
            if (data_dir_path_temp) {
                j.data.reserve(data_directories.size() + 1);
                memcpy(data_dir_path_temp, add_data_rom_path, add_data_rom_path_len);
                data_dir_path_temp[add_data_rom_path_len] = '\\';
                for (std::vector<std::string>::reverse_iterator k = data_directories.rbegin();
                    k != data_directories.rend(); k++) {
                    memcpy(&data_dir_path_temp[add_data_rom_path_len + 1], k->c_str(), k->size() + 1);
                    if (add_data_rom_len) {
                        data_dir_path_temp[add_data_rom_path_len + 1 + k->size()] = '\\';
                        memcpy(&data_dir_path_temp[add_data_rom_path_len + 1 + k->size() + 1],
                            add_data_rom, add_data_rom_len + 1);
                    }

                    data_struct_directory data_dir;
                    data_dir.path = data_dir_path_temp;
                    data_dir.name = *k;
                    j.data.push_back(data_dir);
                }
            }
            else
                j.data.reserve(1);
            free(data_dir_path_temp);

            data_struct_directory data_dir;
            data_dir.path = main_rom_path;
            data_dir.name = {};
            j.data.push_back(data_dir);
            free(data_dir_path_temp);
        }
        free(main_rom_path);
        free(add_data_rom_path);
        ds->ready = true;
    }
    free(buf);
    free(lines);

    if (data_list[DATA_AFT].ready) {
        data_struct* ds = &data_list[DATA_AFT];
        data_ft* d = &ds->data_ft;

        static const char* prefixes[] = {
            "",
            "mdata_",
        };

        for (const char* i : prefixes) {
            std::string file = std::string(i) + "auth_3d_db.bin";

            auth_3d_database_file auth_3d_db_file;
            ds->load_file(&auth_3d_db_file, "rom/auth_3d/", file.c_str(),
                auth_3d_database_file::load_file);
            d->auth_3d_db.add(&auth_3d_db_file);
        }

        {
            bone_database* bone_data = &d->bone_data;
            *bone_data = bone_database();
            bone_data->modern = false;
            ds->load_file(bone_data, "rom/",
                "bone_data.bin", bone_database::load_file);
        }

        for (const char* i : prefixes) {
            std::string file = std::string(i) + "mot_db.farc";

            motion_database_file mot_db_file;
            ds->load_file(&mot_db_file, "rom/rob/", file.c_str(),
                motion_database_file::load_file);
            d->mot_db.add(&mot_db_file);
        }

        for (const char* i : prefixes) {
            std::string file = std::string(i) + "obj_db.bin";

            object_database_file obj_db_file;
            obj_db_file.modern = false;
            ds->load_file(&obj_db_file, "rom/objset/", file.c_str(),
                object_database_file::load_file);
            d->obj_db.add(&obj_db_file);
        }

        for (const char* i : prefixes) {
            std::string file = std::string(i) + "stage_data.bin";

            stage_database_file stage_data_file;
            stage_data_file.modern = false;
            ds->load_file(&stage_data_file, "rom/", file.c_str(),
                stage_database_file::load_file);
            d->stage_data.add(&stage_data_file);
        }

        for (const char* i : prefixes) {
            std::string file = std::string(i) + "tex_db.bin";

            texture_database_file tex_db_file;
            tex_db_file.modern = false;
            ds->load_file(&tex_db_file, "rom/objset/", file.c_str(),
                texture_database_file::load_file);
            d->tex_db.add(&tex_db_file);
        }
    }

#if defined(CRE_DEV)
    if (data_list[DATA_F2BE].ready) {
        data_struct* ds = &data_list[DATA_F2BE];
    }

    if (data_list[DATA_F2LE].ready) {
        data_struct* ds = &data_list[DATA_F2LE];
    }

    if (data_list[DATA_FT].ready) {
        data_struct* ds = &data_list[DATA_FT];
    }

    if (data_list[DATA_M39].ready) {
        data_struct* ds = &data_list[DATA_M39];
    }

    if (data_list[DATA_VRFL].ready) {
        data_struct* ds = &data_list[DATA_VRFL];
        data_f2* d = &ds->data_f2;

        {
            bone_database* bone_data = &d->bone_data;
            *bone_data = bone_database();
            bone_data->modern = true;
            ds->load_file(bone_data, "root+/",
                "bone_data.bon", bone_database::load_file);
        }
    }

    if (data_list[DATA_X].ready) {
        data_struct* ds = &data_list[DATA_X];
        data_f2* d = &ds->data_f2;

        {
            bone_database* bone_data = &d->bone_data;
            *bone_data = bone_database();
            bone_data->modern = true;
            ds->load_file(bone_data, "root+/",
                "bone_data.bon", bone_database::load_file);
        }
    }

    if (data_list[DATA_XHD].ready) {
        data_struct* ds = &data_list[DATA_XHD];
        data_f2* d = &ds->data_f2;

        {
            bone_database* bone_data = &d->bone_data;
            *bone_data = bone_database();
            bone_data->modern = true;
            ds->load_file(bone_data, "root+/",
                "bone_data.bon", bone_database::load_file);
        }
    }
#endif
}

#if defined(CRE_DEV)
static void data_load_glitter_list(data_struct* ds, const char* path) {
    stream s;
    s.open(path, "rb");
    size_t length = s.length;
    uint8_t* data = force_malloc_s(uint8_t, length);
    s.read(data, length);
    s.close();

    char* buf;
    char** lines;
    size_t count;
    if (str_utils_text_file_parse(data, length, &buf, &lines, &count)) {
        for (size_t i = 0; i < count; i++) {
            char* t = strstr(lines[i], "#(?)");
            if (t)
                *t = 0;
        }

        std::vector<std::string>& glitter_list_names = ds->glitter_list_names;
        glitter_list_names.reserve(count);
        for (size_t i = 0; i < count; i++) {
            size_t len = utf8_length(lines[i]);
            std::string name = std::string(lines[i], min(len, 0x7F));
            glitter_list_names.push_back(name);
        }

        switch (ds->type) {
        case DATA_F2LE:
        case DATA_F2BE:
        case DATA_VRFL:
        case DATA_X:
        case DATA_XHD:
            ds->glitter_list_murmurhash.reserve(count);
            for (size_t i = 0; i < count; i++) {
                uint32_t hash = hash_string_murmurhash(ds->glitter_list_names[i]);
                ds->glitter_list_murmurhash.push_back(hash);
            }
            break;
        case DATA_AFT:
        case DATA_FT:
        case DATA_M39:
            ds->glitter_list_fnv1a64m.reserve(count);
            for (size_t i = 0; i < count; i++) {
                uint64_t hash = hash_string_fnv1a64m(ds->glitter_list_names[i]);
                ds->glitter_list_fnv1a64m.push_back(hash);
            }
            break;
        }

        free(buf);
        free(lines);
    }
    free(data);
}
#endif
