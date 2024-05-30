/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "data.hpp"
#include "../KKdLib/io/file_stream.hpp"
#include "../KKdLib/io/memory_stream.hpp"
#include "../KKdLib/io/path.hpp"
#include "../KKdLib/hash.hpp"
#include "../KKdLib/str_utils.hpp"
#include "mdata_manager.hpp"

data_struct* data_list;

static char default_config[] = "#AFT\n\n\n";

static void data_free_inner(data_struct* ds);
static void data_load_inner(stream& s);
static void data_load_glitter_list(data_struct* ds, const char* path);

void data_struct_init() {
    data_list = new data_struct[8];
    for (int32_t i = DATA_AFT; i < DATA_MAX; i++)
        data_list[i].type = (data_type)i;
}

void data_struct_load(const char* path) {
    file_stream s;
    s.open(path, "rb");
    if (s.check_not_null())
        data_load_inner(s);
    else {
        s.close();

        memory_stream ms;
        ms.open(default_config, 7);
        data_load_inner(ms);
    }
}

void data_struct_load(const wchar_t* path) {
    file_stream s;
    s.open(path, L"rb");
    if (s.check_not_null())
        data_load_inner(s);
    else {
        s.close();

        memory_stream ms;
        ms.open(default_config, 7);
        data_load_inner(ms);
    }
}

void data_struct_load_db() {
    if (data_list[DATA_AFT].ready) {
        data_struct* ds = &data_list[DATA_AFT];
        data_ft* d = &ds->data_ft;

        for (const std::string& i : mdata_manager_get()->GetPrefixes()) {
            std::string file(i);
            file.append("aet_db.bin");

            aet_database_file aet_db_file;
            aet_db_file.modern = false;
            ds->load_file(&aet_db_file, "rom/2d/", file.c_str(),
                aet_database_file::load_file);
            d->aet_db.add(&aet_db_file);
        }

        for (const std::string& i : mdata_manager_get()->GetPrefixes()) {
            std::string file(i);
            file.append("auth_3d_db.bin");

            auth_3d_database_file auth_3d_db_file;
            ds->load_file(&auth_3d_db_file, "rom/auth_3d/", file.c_str(),
                auth_3d_database_file::load_file);
            d->auth_3d_db.add(&auth_3d_db_file, !!i.size());
        }

        {
            bone_database* bone_data = &d->bone_data;
            *bone_data = bone_database();
            bone_data->modern = false;
            ds->load_file(bone_data, "rom/",
                "bone_data.bin", bone_database::load_file);
        }

        for (const std::string& i : mdata_manager_get()->GetPrefixes()) {
            std::string file(i);
            file.append("mot_db.farc");

            motion_database_file mot_db_file;
            ds->load_file(&mot_db_file, "rom/rob/", file.c_str(),
                motion_database_file::load_file);
            d->mot_db.add(&mot_db_file);
        }

        for (const std::string& i : mdata_manager_get()->GetPrefixes()) {
            std::string file(i);
            file.append("obj_db.bin");

            object_database_file obj_db_file;
            obj_db_file.modern = false;
            ds->load_file(&obj_db_file, "rom/objset/", file.c_str(),
                object_database_file::load_file);
            d->obj_db.add(&obj_db_file);
        }

        for (const std::string& i : mdata_manager_get()->GetPrefixes()) {
            std::string file(i);
            file.append("spr_db.bin");

            sprite_database_file spr_db_file;
            spr_db_file.modern = false;
            ds->load_file(&spr_db_file, "rom/2d/", file.c_str(),
                sprite_database_file::load_file);
            d->spr_db.add(&spr_db_file);
        }

        for (const std::string& i : mdata_manager_get()->GetPrefixes()) {
            std::string file(i);
            file.append("stage_data.bin");

            stage_database_file stage_data_file;
            stage_data_file.modern = false;
            ds->load_file(&stage_data_file, "rom/", file.c_str(),
                stage_database_file::load_file);
            d->stage_data.add(&stage_data_file);
        }

        for (const std::string& i : mdata_manager_get()->GetPrefixes()) {
            std::string file(i);
            file.append("tex_db.bin");

            texture_database_file tex_db_file;
            tex_db_file.modern = false;
            ds->load_file(&tex_db_file, "rom/objset/", file.c_str(),
                texture_database_file::load_file);
            d->tex_db.add(&tex_db_file);
        }
    }

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
}

void data_struct_free() {
    if (data_list[DATA_AFT].ready) {
        data_struct* ds = &data_list[DATA_AFT];
        data_ft* d = &ds->data_ft;
    }

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

    for (int32_t i = DATA_AFT; i < DATA_MAX; i++)
        data_free_inner(&data_list[i]);
    delete[] data_list;
}

bool data_struct::check_directory_exists(const char* dir) {
    if (path_check_directory_exists(dir))
        return true;

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

    std::string dir_temp(dir, dir_len);

    char* t = (char*)dir_temp.c_str();
    while (t = strchr(t, '/'))
        *t = '\\';

    std::string temp;
    for (data_struct_path& i : data_paths)
        for (data_struct_directory& j : i.data) {
            temp.assign(j.path);
            temp.push_back('\\');
            temp.append(dir_temp);

            if (path_check_directory_exists(temp.c_str()))
                return true;
        }
    return false;
}

bool data_struct::check_file_exists(const char* path) {
    const char* t = strrchr(path, '/');
    if (!t)
        t = strrchr(path, '\\');

    if (t) {
        std::string dir(path, t - path + 1);
        return check_file_exists(dir.c_str(), t + 1);
    }
    return false;
}

bool data_struct::check_file_exists(const char* dir, const char* file) {
    size_t file_len = utf8_length(file);

    if (path_check_directory_exists(dir)) {
        const char* t = strrchr(file, '.');
        size_t t_len;
        if (t)
            t_len = t - file;
        else
            t_len = file_len;

        uint32_t h = hash_murmurhash(file, t_len, 0, false, false);
        std::string path(dir);
        path.append(file);
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

    std::string dir_temp(dir, dir_len);

    char* t = (char*)dir_temp.c_str();
    while (t = strchr(t, '/'))
        *t = '\\';

    std::string temp;
    for (data_struct_path& i : data_paths)
        for (data_struct_directory& j : i.data) {
            temp.assign(j.path);
            temp.push_back('\\');
            temp.append(dir_temp);
            temp.append(file, file_len);

            if (path_check_file_exists(temp.c_str()))
                return true;
        }

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

    std::string dir_temp(dir, dir_len);

    char* t = (char*)dir_temp.c_str();
    while (t = strchr(t, '/'))
        *t = '\\';

    std::string temp;
    for (data_struct_path& i : data_paths)
        for (data_struct_directory& j : i.data) {
            temp.assign(j.path);
            temp.push_back('\\');
            temp.append(dir_temp);

            std::vector<std::string> files = path_get_files(temp.c_str());
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
                if (l == hash)
                    return true;
        }

    if (f2)
        return check_file_exists("rom/data/", hash);
    return false;
}

std::vector<data_struct_file> data_struct::get_directory_files(const char* dir) {
    std::vector<data_struct_file> files;
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
        return {};

    while (*dir == '/' || *dir == '\\') {
        dir++;
        dir_len--;
    }

    std::string dir_temp(dir, dir_len);

    char* t = (char*)dir_temp.c_str();
    while (t = strchr(t, '/'))
        *t = '\\';

    std::string temp;
    for (data_struct_path& i : data_paths)
        for (data_struct_directory& j : i.data) {
            temp.assign(j.path);
            temp.push_back('\\');
            temp.append(dir_temp);

            std::vector<std::string> path_files = path_get_files(temp.c_str());
            if (!path_files.size())
                continue;

            files.reserve(path_files.size());
            for (std::string& k : path_files) {
                bool found = false;
                for (data_struct_file& l : files)
                    if (k == l.name) {
                        found = true;
                        break;
                    }

                if (found)
                    continue;

                data_struct_file f;
                f.path.assign(temp);
                f.name.assign(k);
                files.push_back(f);
            }
        }
    files.shrink_to_fit();
    return files;
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

    std::string dir_temp(dir, dir_len);

    char* t = (char*)dir_temp.c_str();
    while (t = strchr(t, '/'))
        *t = '\\';

    std::string temp;
    for (data_struct_path& i : data_paths)
        for (data_struct_directory& j : i.data) {
            temp.assign(j.path);
            temp.push_back('\\');
            temp.append(dir_temp);

            std::vector<std::string> files = path_get_files(temp.c_str());
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
                    file.assign(l);
                    return true;
                }
            }
        }

    if (f2)
        return get_file("rom/data/", hash, ext, file);
    return false;
}

bool data_struct::get_file_path(std::string& path) {
    size_t pos = path.find('/');
    if (pos) {
        std::string dir(path, 0, pos + 1);
        std::string file(path, pos + 1);
        return get_file_path(dir.c_str(), file.c_str(), path);
    }

    pos = path.find('\\');
    if (pos) {
        std::string dir(path, 0, pos + 1);
        std::string file(path, pos + 1);
        return get_file_path(dir.c_str(), file.c_str(), path);
    }
    return false;
}

bool data_struct::get_file_path(const char* dir, const char* file, std::string& path) {
    size_t file_len = utf8_length(file);

    if (path_check_directory_exists(dir)) {
        const char* t = strrchr(file, '.');
        size_t t_len;
        if (t)
            t_len = t - file;
        else
            t_len = file_len;

        uint32_t h = hash_murmurhash(file, t_len, 0, false, false);
        std::string _path(dir);
        _path.append(file);
        if (path_check_file_exists(_path.c_str())) {
            path.assign(_path);
            return true;
        }
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

    std::string dir_temp(dir, dir_len);

    char* t = (char*)dir_temp.c_str();
    while (t = strchr(t, '/'))
        *t = '\\';

    std::string temp;
    for (data_struct_path& i : data_paths)
        for (data_struct_directory& j : i.data) {
            temp.assign(j.path);
            temp.push_back('\\');
            temp.append(dir_temp);
            temp.append(file, file_len);

            if (path_check_file_exists(temp.c_str())) {
                path.assign(temp);
                return true;
            }
        }

    if (f2)
        return get_file_path("rom/data/", file, path);
    return false;
}

bool data_struct::load_file(void* data, const char* path,
    bool (*load_func)(void* data, const char* dir, const char* file, uint32_t hash)) {
    const char* t = strrchr(path, '/');
    if (!t)
        t = strrchr(path, '\\');

    if (t) {
        std::string dir(path, t - path + 1);
        return load_file(data, dir.c_str(), t + 1, load_func);
    }
    return false;
}

bool data_struct::load_file(void* data, const char* dir, const char* file,
    bool (*load_func)(void* data, const char* dir, const char* file, uint32_t hash)) {
    size_t file_len = utf8_length(file);

    if (path_check_directory_exists(dir)) {
        const char* t = strrchr(file, '.');
        size_t t_len;
        if (t)
            t_len = t - file;
        else
            t_len = file_len;

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

    std::string dir_temp(dir, dir_len);

    char* t = (char*)dir_temp.c_str();
    while(t = strchr(t, '/'))
        *t = '\\';

    std::string temp;
    for (data_struct_path& i : data_paths)
        for (data_struct_directory& j : i.data) {
            temp.assign(j.path);
            temp.push_back('\\');
            temp.append(dir_temp);
            temp.append(file, file_len);

            if (path_check_file_exists(temp.c_str())) {
                temp.resize(temp.size() - file_len);

                const char* l_str = file;
                const char* t = strrchr(l_str, '.');
                size_t l_len;
                if (t)
                    l_len = t - l_str;
                else
                    l_len = file_len;

                uint32_t h = hash_murmurhash(l_str, l_len);
                if (load_func(data, temp.c_str(), l_str, h))
                    return true;
            }
        }

    if (f2)
        return load_file(data, "rom/data/", file, load_func);
    return false;
}

bool data_struct::load_file(void* data, const char* dir, uint32_t hash, const char* ext,
    bool (*load_func)(void* data, const char* dir, const char* file, uint32_t hash)) {
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

    std::string dir_temp(dir, dir_len);

    char* t = (char*)dir_temp.c_str();
    while (t = strchr(t, '/'))
        *t = '\\';

    std::string temp;
    for (data_struct_path& i : data_paths)
        for (data_struct_directory& j : i.data) {
            temp.assign(j.path);
            temp.push_back('\\');
            temp.append(dir_temp);

            std::vector<std::string> files = path_get_files(temp.c_str());
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
                    && load_func(data, temp.c_str(), l.c_str(), hash))
                    return true;
            }
        }

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

data_f2::data_f2() {

}

data_f2::~data_f2() {

}

data_ft::data_ft() {

}

data_ft::~data_ft() {

}

data_x::data_x() {

}

data_x::~data_x() {

}

static void data_free_inner(data_struct* ds) {
    ds->data_paths.clear();
    ds->data_paths.shrink_to_fit();

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
    ds->ready = false;
}

static void data_load_inner(stream& s) {
    size_t length = s.length;
    void* data = force_malloc(length);
    s.read(data, length);

    char* buf;
    char** lines;
    size_t count;
    if (!str_utils_text_file_parse(data, length, buf, lines, count)) {
        free_def(data);
        return;
    }
    free_def(data);

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
        else
            continue;

        data_free_inner(ds);

        if (i + 2 >= count)
            continue;

        char* data_paths = lines[i + 1];
        char* glitter_list = lines[i + 2];
        i++;

        if (!str_utils_check_ends_with(glitter_list, ".glitter.txt"))
            continue;

        data_load_glitter_list(ds, glitter_list);

        char* t = data_paths;
        size_t count = 1;
        while (t = strchr(t, ';')) {
            *t++ = 0;
            count++;
        }

        size_t main_rom_len = utf8_length(main_rom);
        size_t add_data_len = utf8_length(add_data);
        size_t add_data_rom_len = utf8_length(add_data_rom);

        std::string main_rom_path;
        std::string add_data_rom_path;

        ssize_t* t_len = force_malloc<ssize_t>(count);
        if (!t_len) {
            data_free_inner(ds);
            continue;
        }

        t = data_paths;
        for (size_t j = 0; j < count; j++)
            t += (t_len[j] = utf8_length(t)) + 1;

        ds->data_paths.reserve(count);
        for (size_t j = 0; j < count; j++) {
            ds->data_paths.push_back({});
            data_struct_path& data_path = ds->data_paths.back();
            t -= t_len[count - j - 1] + 1;
            data_path.path.assign(t, t_len[count - j - 1]);
            path_get_full_path(data_path.path);
        }
        free_def(t_len);

        for (data_struct_path& j : ds->data_paths) {
            const char* data_path = j.path.c_str();
            size_t data_path_length = j.path.size();
            if (!main_rom_len)
                main_rom_path.assign(j.path);
            else if (data_path_length) {
                main_rom_path.assign(j.path);
                main_rom_path.push_back('\\');
                main_rom_path.append(main_rom, main_rom_len);
            }
            else
                main_rom_path.assign(main_rom, main_rom_len);

            if (!add_data_len)
                add_data_rom_path.assign(j.path);
            else if (data_path_length) {
                add_data_rom_path.assign(j.path);
                add_data_rom_path.push_back('\\');
                add_data_rom_path.append(add_data, add_data_len);
            }
            else
                add_data_rom_path.assign(add_data, add_data_len);

            path_get_full_path(main_rom_path);
            path_get_full_path(add_data_rom_path);

            if (main_rom_path.size() && main_rom_path.back() == '\\')
                main_rom_path.pop_back();

            if (add_data_rom_path.size() && add_data_rom_path.back() == '\\')
                add_data_rom_path.pop_back();

            const char* exclude_list[1];
            exclude_list[0] = main_rom_path.c_str();
            std::vector<std::string> data_directories
                = path_get_directories(add_data_rom_path.c_str(), exclude_list, 1);

            std::string data_dir_path;
            if (data_directories.size()) {
                j.data.reserve(data_directories.size() + 1);
                data_dir_path.assign(add_data_rom_path);
                data_dir_path.push_back('\\');
                for (std::vector<std::string>::reverse_iterator k = data_directories.rbegin();
                    k != data_directories.rend(); k++) {
                    j.data.push_back({});
                    data_struct_directory& data_dir = j.data.back();
                    data_dir.path.assign(data_dir_path);
                    data_dir.path.append(*k);
                    if (add_data_rom_len) {
                        data_dir.path.push_back('\\');
                        data_dir.path.append(add_data_rom, add_data_rom_len);
                    }
                    data_dir.name.assign(*k);
                }
            }
            else
                j.data.reserve(1);

            j.data.push_back({});
            data_struct_directory& data_dir = j.data.back();
            data_dir.path.assign(main_rom_path);
        }
        ds->ready = true;
    }
    free_def(buf);
    free_def(lines);
}

static void data_load_glitter_list(data_struct* ds, const char* path) {
    file_stream s;
    s.open(path, "rb");
    size_t length = s.length;
    uint8_t* data = force_malloc<uint8_t>(length);
    s.read(data, length);
    s.close();

    char* buf;
    char** lines;
    size_t count;
    if (str_utils_text_file_parse(data, length, buf, lines, count)) {
        for (size_t i = 0; i < count; i++) {
            char* t = strstr(lines[i], "#(?)");
            if (t)
                *t = 0;
        }

        std::vector<std::string>& glitter_list_names = ds->glitter_list_names;
        glitter_list_names.reserve(count);
        for (size_t i = 0; i < count; i++) {
            size_t len = utf8_length(lines[i]);
            std::string name(lines[i], min_def(len, 0x7F));
            glitter_list_names.push_back(name);
        }

        switch (ds->type) {
        case DATA_F2LE:
        case DATA_F2BE:
        case DATA_VRFL:
        case DATA_X:
        case DATA_XHD:
            ds->glitter_list_murmurhash.reserve(count);
            for (std::string& i : ds->glitter_list_names) {
                uint32_t hash = hash_string_murmurhash(i);
                ds->glitter_list_murmurhash.push_back(hash, i.c_str());
            }
            ds->glitter_list_murmurhash.sort();
            break;
        case DATA_AFT:
        case DATA_FT:
        case DATA_M39:
            ds->glitter_list_fnv1a64m.reserve(count);
            for (std::string& i : ds->glitter_list_names) {
                uint64_t hash = hash_string_fnv1a64m(i);
                ds->glitter_list_fnv1a64m.push_back(hash, i.c_str());
            }
            ds->glitter_list_fnv1a64m.sort();
            break;
        }

        free_def(buf);
        free_def(lines);
    }
    free_def(data);
}
