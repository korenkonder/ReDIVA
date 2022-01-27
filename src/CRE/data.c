/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "data.h"
#include "../KKdLib/io/path.h"
#include "../KKdLib/io/stream.h"
#include "../KKdLib/str_utils.h"

vector_func(data_struct_file)
vector_func(data_struct_directory)
vector_func(data_struct_path)

data_struct data_list[DATA_MAX];

static char default_config[] = "#AFT\n\n\n";

static void data_free_inner(data_struct* ds);
static void data_load_inner(stream* s);
#if defined(CRE_DEV)
static void data_load_glitter_list(data_struct* ds, char* path);
#endif

void data_struct_init() {
    memset(data_list, 0, sizeof(data_struct) * DATA_MAX);
    for (data_type i = DATA_AFT; i < DATA_MAX; i++)
        data_list[i].type = i;
}

void data_struct_load(char* path) {
    stream s;
    io_open(&s, path, "rb");
    if (s.io.stream)
        data_load_inner(&s);
    else {
        io_free(&s);
        io_mopen(&s, default_config, 7);
        data_load_inner(&s);
    }
    io_free(&s);
}

void data_struct_wload(wchar_t* path) {
    stream s;
    io_wopen(&s, path, L"rb");
    if (s.io.stream)
        data_load_inner(&s);
    else {
        io_free(&s);
        io_mopen(&s, default_config, 7);
        data_load_inner(&s);
    }
    io_free(&s);
}

void data_struct_get_directory_files(data_struct* ds, char* dir, vector_data_struct_file* data_files) {
    vector_data_struct_file_clear(data_files, data_struct_file_free);

    size_t dir_len = utf8_length(dir);
    if (dir_len >= 2 && !memcmp(dir, "./", 2)) {
        dir += 2;
        dir_len -= 2;
    }

    if (dir_len < 3 || memcmp(dir, "rom", 3))
        return;

    dir += 3;
    dir_len -= 3;

    while (*dir == '/' || *dir == '\\') {
        dir++;
        dir_len--;
    }

    size_t max_len = 0;
    for (data_struct_path* i = ds->data_paths.begin; i != ds->data_paths.end; i++)
        for (data_struct_directory* j = i->data.begin; j != i->data.end; j++)
            if (max_len < j->path.length)
                max_len = j->path.length;

    char* dir_temp = force_malloc(dir_len + 1);
    memcpy(dir_temp, dir, dir_len + 1);

    char* t = dir_temp;
    while (t = strchr(t, '/'))
        *t = '\\';

    char* temp = force_malloc(max_len + dir_len + 2);
    for (data_struct_path* i = ds->data_paths.begin; i != ds->data_paths.end; i++)
        for (data_struct_directory* j = i->data.begin; j != i->data.end; j++) {
            char* path = string_data(&j->path);
            size_t path_len = j->path.length;

            memcpy(temp, path, path_len);
            memcpy(&temp[path_len], "\\", 2);
            if (dir_len)
                memcpy(&temp[path_len + 1], dir_temp, dir_len + 1);

            vector_string files = vector_empty(string);
            path_get_files(&files, temp);
            for (string* l = files.begin; l != files.end; l++) {
                char* l_str = string_data(l);
                size_t len = l->length;
                bool found = false;
                for (data_struct_file* i = data_files->begin; i != data_files->end; i++)
                    if (!memcmp(l_str, string_data(&i->name), min(len, i->name.length) + 1)) {
                        found = true;
                        break;
                    }

                if (found)
                    continue;

                data_struct_file f;
                string_init(&f.path, temp);
                f.name = *l;
                *l = string_empty;
                vector_data_struct_file_push_back(data_files, &f);
            }
            vector_string_free(&files, string_free);
        }

    free(dir_temp);
    free(temp);
}

bool data_struct_load_file(data_struct* ds, void* data, char* dir, char* file,
    bool (*load_func)(void* data, char* path, char* file, uint32_t hash)) {
    size_t dir_len = utf8_length(dir);
    if (dir_len >= 2 && !memcmp(dir, "./", 2)) {
        dir += 2;
        dir_len -= 2;
    }

    if (dir_len < 3 || memcmp(dir, "rom", 3))
        return false;

    dir += 3;
    dir_len -= 3;

    while (*dir == '/' || *dir == '\\') {
        dir++;
        dir_len--;
    }

    size_t max_len = 0;
    for (data_struct_path* i = ds->data_paths.begin; i != ds->data_paths.end; i++)
        for (data_struct_directory* j = i->data.begin; j != i->data.end; j++)
            if (max_len < j->path.length)
                max_len = j->path.length;

    char* dir_temp = force_malloc(dir_len + 1);
    memcpy(dir_temp, dir, dir_len + 1);

    char* t = dir_temp;
    while(t = strchr(t, '/'))
        *t = '\\';

    char* temp = force_malloc(max_len + dir_len + 2);
    for (data_struct_path* i = ds->data_paths.begin; i != ds->data_paths.end; i++)
        for (data_struct_directory* j = i->data.begin; j != i->data.end; j++) {
            char* path = string_data(&j->path);
            size_t path_len = j->path.length;

            memcpy(temp, path, path_len);
            memcpy(&temp[path_len], "\\", 2);
            if (dir_len)
                memcpy(&temp[path_len + 1], dir_temp, dir_len + 1);

            vector_string files = vector_empty(string);
            path_get_files(&files, temp);

            bool ret = false;
            for (string* l = files.begin; l != files.end; l++) {
                if (str_utils_compare(string_data(l), file))
                    continue;

                char* l_str = string_data(l);
                char* t = strrchr(l_str, '.');
                size_t l_len = l->length;
                if (t)
                    l_len = t - l_str - 1;

                uint32_t h = hash_murmurhash(l_str, l_len, 0, false, false);
                ret = load_func(data, temp, l_str, h);
                break;
            }
            vector_string_free(&files, string_free);

            if (ret) {
                free(dir_temp);
                free(temp);
                return true;
            }
        }

    free(dir_temp);
    free(temp);
    return false;
}

bool data_struct_load_file_by_hash(data_struct* ds, void* data, char* dir, uint32_t hash,
    bool (*load_func)(void* data, char* path, char* file, uint32_t hash)) {
    size_t dir_len = utf8_length(dir);
    if (dir_len >= 2 && !memcmp(dir, "./", 2)) {
        dir += 2;
        dir_len -= 2;
    }

    if (dir_len < 3 || memcmp(dir, "rom", 3))
        return false;

    dir += 3;
    dir_len -= 3;

    while (*dir == '/' || *dir == '\\') {
        dir++;
        dir_len--;
    }

    size_t max_len = 0;
    for (data_struct_path* i = ds->data_paths.begin; i != ds->data_paths.end; i++)
        for (data_struct_directory* j = i->data.begin; j != i->data.end; j++)
            if (max_len < j->path.length)
                max_len = j->path.length;

    char* dir_temp = force_malloc(dir_len + 1);
    memcpy(dir_temp, dir, dir_len + 1);

    char* t = dir_temp;
    while (t = strchr(t, '/'))
        *t = '\\';

    char* temp = force_malloc(max_len + dir_len + 2);
    for (data_struct_path* i = ds->data_paths.begin; i != ds->data_paths.end; i++)
        for (data_struct_directory* j = i->data.begin; j != i->data.end; j++) {
            char* path = string_data(&j->path);
            size_t path_len = j->path.length;

            memcpy(temp, path, path_len);
            memcpy(&temp[path_len], "\\", 2);
            if (dir_len)
                memcpy(&temp[path_len + 1], dir_temp, dir_len + 1);

            vector_string files = vector_empty(string);
            path_get_files(&files, temp);

            vector_uint32_t files_murmurhash = vector_empty(uint32_t);
            vector_uint32_t_reserve(&files_murmurhash, vector_length(files));
            for (string* l = files.begin; l != files.end; l++) {
                char* t = strrchr(string_data(l), '.');
                size_t t_len = l->length;
                if (t)
                    t_len = t - string_data(l);
                t = string_data(l);

                uint32_t h = hash_murmurhash(t, t_len, 0, false, false);
                vector_uint32_t_push_back(&files_murmurhash, &h);
            }

            bool ret = false;
            for (uint32_t* l = files_murmurhash.begin; l != files_murmurhash.end; l++)
                if (*l == hash) {
                    string* file = &files.begin[l - files_murmurhash.begin];
                    ret = load_func(data, temp, string_data(file), hash);
                    break;
                }
            vector_string_free(&files, string_free);
            vector_uint32_t_free(&files_murmurhash, 0);

            if (ret) {
                free(dir_temp);
                free(temp);
                return true;
            }
        }

    free(dir_temp);
    free(temp);
    return false;
}

void data_struct_free() {
    if (data_list[DATA_AFT].ready) {
        data_struct* ds = &data_list[DATA_AFT];
        data_ft* d = &ds->data_ft;

        auth_3d_database_free(&d->auth_3d_db);
        bone_database_free(&d->bone_data);
        motion_database_free(&d->mot_db);
        object_database_free(&d->obj_db);
        stage_database_free(&d->stage_data);
        texture_database_free(&d->tex_db);

        auth_3d_database_file_free(&d->base_auth_3d_db);
        motion_database_free(&d->base_mot_db);
        object_database_free(&d->base_obj_db);
        stage_database_free(&d->base_stage_data);
        texture_database_free(&d->base_tex_db);
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

        bone_database_free(&d->bone_data);
    }

    if (data_list[DATA_X].ready) {
        data_struct* ds = &data_list[DATA_X];
        data_x* d = &ds->data_x;

        bone_database_free(&d->bone_data);
    }

    if (data_list[DATA_XHD].ready) {
        data_struct* ds = &data_list[DATA_XHD];
        data_x* d = &ds->data_x;

        bone_database_free(&d->bone_data);
    }
#endif

    for (data_type i = DATA_AFT; i < DATA_MAX; i++)
        data_free_inner(&data_list[i]);
}

void data_struct_directory_free(data_struct_directory* data_dir) {
    string_free(&data_dir->path);
    string_free(&data_dir->name);
}

void data_struct_path_free(data_struct_path* data_path) {
    string_free(&data_path->path);
    vector_data_struct_directory_free(&data_path->data, data_struct_directory_free);
}

void data_struct_file_free(data_struct_file* file) {
    string_free(&file->path);
    string_free(&file->name);
}

static void data_free_inner(data_struct* ds) {
    vector_data_struct_path_free(&ds->data_paths, data_struct_path_free);

#if defined(CRE_DEV)
    vector_string_free(&ds->glitter_list_names, string_free);
    switch (ds->type) {
    case DATA_F2LE:
    case DATA_F2BE:
    case DATA_VRFL:
    case DATA_X:
    case DATA_XHD:
        vector_uint32_t_free(&ds->glitter_list_murmurhash, 0);
        break;
    case DATA_AFT:
    case DATA_FT:
    case DATA_M39:
        vector_uint64_t_free(&ds->glitter_list_fnv1a64m, 0);
        break;
    }
#endif
    ds->ready = false;
}

static void data_load_inner(stream* s) {
    size_t length = s->length;
    void* data = force_malloc(length);
    io_read(s, data, length);

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
        char* main_rom = "rom";
        char* add_data_rom = "rom";
        char* add_data = "";
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
            data_paths = ".";
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

        char* main_rom_path = force_malloc(max_len + main_rom_len + 3);
        if (!main_rom_path) {
            data_free_inner(ds);
            continue;
        }

        char* add_data_rom_path = force_malloc(max_len + add_data_len + 3);
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

        vector_data_struct_path_reserve(&ds->data_paths, count);
        for (size_t j = 0; j < count; j++) {
            data_struct_path data_path;
            t -= t_len[count - j - 1] + 1;
            string_init_length(&data_path.path, t, t_len[count - j - 1]);
            data_path.data = vector_empty(data_struct_directory);
            vector_data_struct_path_push_back(&ds->data_paths, &data_path);
        }
        free(t_len);

        for (data_struct_path* j = ds->data_paths.begin; j != ds->data_paths.end; j++) {
            size_t data_path_length = j->path.length;
            if (!main_rom_len)
                memcpy(main_rom_path, string_data(&j->path), data_path_length);
            else if (data_path_length) {
                memcpy(main_rom_path, string_data(&j->path), data_path_length);
                main_rom_path[data_path_length] = '\\';
                memcpy(&main_rom_path[data_path_length + 1], main_rom, main_rom_len + 1);
            }
            else
                memcpy(main_rom_path, main_rom, main_rom_len + 1);

            if (!add_data_len)
                memcpy(add_data_rom_path, string_data(&j->path), data_path_length);
            else if (data_path_length) {
                memcpy(add_data_rom_path, string_data(&j->path), data_path_length);
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

            vector_string data_directories = vector_empty(string);
            path_get_directories(&data_directories, add_data_rom_path, &main_rom_path, 1);

            max_len = 0;
            for (string* k = data_directories.begin; k != data_directories.end; k++) {
                size_t len = k->length;
                if (max_len < len)
                    max_len = len;
            }

            char* data_dir_path_temp = force_malloc(add_data_rom_path_len + max_len + add_data_rom_len + 3);
            if (data_dir_path_temp) {
                vector_data_struct_directory_reserve(&j->data, vector_length(data_directories) + 1);
                memcpy(data_dir_path_temp, add_data_rom_path, add_data_rom_path_len);
                data_dir_path_temp[add_data_rom_path_len] = '\\';
                for (string* k = data_directories.end - 1; k != data_directories.begin - 1; k--) {
                    memcpy(&data_dir_path_temp[add_data_rom_path_len + 1], string_data(k), k->length + 1);
                    if (add_data_rom_len) {
                        data_dir_path_temp[add_data_rom_path_len + 1 + k->length] = '\\';
                        memcpy(&data_dir_path_temp[add_data_rom_path_len + 1 + k->length + 1],
                            add_data_rom, add_data_rom_len + 1);
                    }

                    data_struct_directory data_dir;
                    string_init(&data_dir.path, data_dir_path_temp);
                    data_dir.name = *k;
                    vector_data_struct_directory_push_back(&j->data, &data_dir);
                }
                vector_string_free(&data_directories, 0);
            }
            else {
                vector_data_struct_directory_reserve(&j->data, 1);
                vector_string_free(&data_directories, string_free);
            }
            free(data_dir_path_temp);

            data_struct_directory data_dir;
            string_init(&data_dir.path, main_rom_path);
            data_dir.name = string_empty;
            vector_data_struct_directory_push_back(&j->data, &data_dir);
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

        auth_3d_database_file* base_auth_3d_db = &d->base_auth_3d_db;
        auth_3d_database_file_init(base_auth_3d_db);
        data_struct_load_file(ds, base_auth_3d_db, "rom/auth_3d/",
            "auth_3d_db.bin", auth_3d_database_file_load_file);

        auth_3d_database_file mdata_auth_3d_db;
        auth_3d_database_file_init(&mdata_auth_3d_db);
        data_struct_load_file(ds, &mdata_auth_3d_db, "rom/auth_3d/",
            "mdata_auth_3d_db.bin", auth_3d_database_file_load_file);

        auth_3d_database* auth_3d_db = &d->auth_3d_db;
        auth_3d_database_init(auth_3d_db);
        auth_3d_database_merge_mdata(auth_3d_db, base_auth_3d_db, &mdata_auth_3d_db);
        auth_3d_database_file_free(&mdata_auth_3d_db);

        bone_database* bone_data = &d->bone_data;
        bone_database_init(bone_data);
        bone_data->modern = false;
        data_struct_load_file(ds, bone_data, "rom/",
            "bone_data.bin", bone_database_load_file);

        motion_database* base_mot_db = &d->base_mot_db;
        motion_database_init(base_mot_db);
        data_struct_load_file(ds, base_mot_db, "rom/rob/",
            "mot_db.farc", motion_database_load_file);

        motion_database mdata_mot_db;
        motion_database_init(&mdata_mot_db);
        data_struct_load_file(ds, &mdata_mot_db, "rom/rob/",
            "mdata_mot_db.farc", motion_database_load_file);

        motion_database* mot_db = &d->mot_db;
        motion_database_init(mot_db);
        motion_database_merge_mdata(mot_db, base_mot_db, &mdata_mot_db);
        motion_database_free(&mdata_mot_db);

        object_database* base_obj_db = &d->base_obj_db;
        object_database_init(base_obj_db);
        base_obj_db->modern = false;
        data_struct_load_file(ds, base_obj_db, "rom/objset/",
            "obj_db.bin", object_database_load_file);

        object_database mdata_obj_db;
        object_database_init(&mdata_obj_db);
        mdata_obj_db.modern = false;
        data_struct_load_file(ds, &mdata_obj_db, "rom/objset/",
            "mdata_obj_db.bin", object_database_load_file);

        object_database* obj_db = &d->obj_db;
        object_database_init(obj_db);
        object_database_merge_mdata(obj_db, base_obj_db, &mdata_obj_db);
        object_database_free(&mdata_obj_db);
        
        stage_database* base_stage_data = &d->base_stage_data;
        stage_database_init(base_stage_data);
        base_stage_data->modern = false;
        data_struct_load_file(ds, base_stage_data, "rom/",
            "stage_data.bin", stage_database_load_file);

        stage_database mdata_stage_data;
        stage_database_init(&mdata_stage_data);
        mdata_stage_data.modern = false;
        data_struct_load_file(ds, &mdata_stage_data, "rom/",
            "mdata_stage_data.bin", stage_database_load_file);

        stage_database* stage_data = &d->stage_data;
        stage_database_init(stage_data);
        stage_database_merge_mdata(stage_data, base_stage_data, &mdata_stage_data);
        stage_database_free(&mdata_stage_data);

        texture_database* base_tex_db = &d->base_tex_db;
        texture_database_init(base_tex_db);
        base_tex_db->modern = false;
        data_struct_load_file(ds, base_tex_db, "rom/objset/",
            "tex_db.bin", texture_database_load_file);

        texture_database mdata_tex_db;
        texture_database_init(&mdata_tex_db);
        mdata_tex_db.modern = false;
        data_struct_load_file(ds, &mdata_tex_db, "rom/objset/",
            "mdata_tex_db.bin", texture_database_load_file);

        texture_database* tex_db = &d->tex_db;
        texture_database_init(tex_db);
        texture_database_merge_mdata(tex_db, base_tex_db, &mdata_tex_db);
        texture_database_free(&mdata_tex_db);
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

        bone_database* bone_data = &d->bone_data;
        bone_database_init(bone_data);
        bone_data->modern = true;
        data_struct_load_file(ds, bone_data, "rom/",
            "bone_data.bon", bone_database_load_file);
    }

    if (data_list[DATA_X].ready) {
        data_struct* ds = &data_list[DATA_X];
        data_f2* d = &ds->data_f2;

        bone_database* bone_data = &d->bone_data;
        bone_database_init(bone_data);
        bone_data->modern = true;
        data_struct_load_file(ds, bone_data, "rom/",
            "bone_data.bon", bone_database_load_file);
    }

    if (data_list[DATA_XHD].ready) {
        data_struct* ds = &data_list[DATA_XHD];
        data_f2* d = &ds->data_f2;

        bone_database* bone_data = &d->bone_data;
        bone_database_init(bone_data);
        bone_data->modern = true;
        data_struct_load_file(ds, bone_data, "rom/",
            "bone_data.bon", bone_database_load_file);
    }
#endif
}

#if defined(CRE_DEV)
static void data_load_glitter_list(data_struct* ds, char* path) {
    stream s;
    io_open(&s, path, "rb");
    size_t length = s.length;
    uint8_t* data = force_malloc(length);
    io_read(&s, data, length);
    io_free(&s);

    char* buf;
    char** lines;
    size_t count;
    if (str_utils_text_file_parse(data, length, &buf, &lines, &count)) {
        for (size_t i = 0; i < count; i++) {
            char* t = strstr(lines[i], "#(?)");
            if (t)
                *t = 0;
        }

        vector_string_reserve(&ds->glitter_list_names, count);
        for (size_t i = 0; i < count; i++) {
            size_t len = utf8_length(lines[i]);
            string s;
            string_init_length(&s, lines[i], min(len, 0x7F));
            vector_string_push_back(&ds->glitter_list_names, &s);
        }

        switch (ds->type) {
        case DATA_F2LE:
        case DATA_F2BE:
        case DATA_VRFL:
        case DATA_X:
        case DATA_XHD:
            vector_uint32_t_reserve(&ds->glitter_list_murmurhash, count);
            for (size_t i = 0; i < count; i++) {
                size_t len = ds->glitter_list_names.begin[i].length;
                uint32_t hash = hash_murmurhash(lines[i], len, 0, false, false);
                vector_uint32_t_push_back(&ds->glitter_list_murmurhash, &hash);
            }
            break;
        case DATA_AFT:
        case DATA_FT:
        case DATA_M39:
            vector_uint64_t_reserve(&ds->glitter_list_fnv1a64m, count);
            for (size_t i = 0; i < count; i++) {
                size_t len = ds->glitter_list_names.begin[i].length;
                uint64_t hash = hash_fnv1a64m(lines[i], len, false);
                vector_uint64_t_push_back(&ds->glitter_list_fnv1a64m, &hash);
            }
            break;
        }

        free(buf);
        free(lines);
    }
    free(data);
}
#endif
