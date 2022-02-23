/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "path.h"
#include "../str_utils.h"

bool path_check_path_exists(char* path) {
    wchar_t* path_temp = utf8_to_utf16(path);
    DWORD ftyp = GetFileAttributesW(path_temp);
    free(path_temp);
    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false;
    else
        return true;
}

inline bool path_check_path_exists(const char* path) {
    return path_check_path_exists((char*)path);
}

bool path_wcheck_path_exists(wchar_t* path) {
    DWORD ftyp = GetFileAttributesW(path);
    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false;
    else
        return true;
}

inline bool path_wcheck_path_exists(const wchar_t* path) {
    return path_wcheck_path_exists((wchar_t*)path);

}

bool path_check_file_exists(char* path) {
    wchar_t* path_temp = utf8_to_utf16(path);
    DWORD ftyp = GetFileAttributesW(path_temp);
    free(path_temp);
    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false;
    else
        return ftyp & FILE_ATTRIBUTE_DIRECTORY ? false : true;
}

inline bool path_check_file_exists(const char* path) {
    return path_check_file_exists((char*)path);
}

bool path_wcheck_file_exists(wchar_t* path) {
    DWORD ftyp = GetFileAttributesW(path);
    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false;
    else
        return ftyp & FILE_ATTRIBUTE_DIRECTORY ? false : true;
}

inline bool path_wcheck_file_exists(const wchar_t* path) {
    return path_wcheck_file_exists((wchar_t*)path);
}

bool path_check_directory_exists(char* path) {
    wchar_t* path_temp = utf8_to_utf16(path);
    DWORD ftyp = GetFileAttributesW(path_temp);
    free(path_temp);
    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false;
    else
        return ftyp & FILE_ATTRIBUTE_DIRECTORY ? true : false;
}

inline bool path_check_directory_exists(const char* path) {
    return path_check_directory_exists((char*)path);
}

bool path_wcheck_directory_exists(wchar_t* path) {
    DWORD ftyp = GetFileAttributesW(path);
    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false;
    else
        return ftyp & FILE_ATTRIBUTE_DIRECTORY ? true : false;
}

inline bool path_wcheck_directory_exists(const wchar_t* path) {
    return path_wcheck_directory_exists((wchar_t*)path);
}

void path_get_files(vector_old_string* files, char* path) {
    vector_old_string_free(files, string_free);

    wchar_t* dir_temp = utf8_to_utf16(path);
    size_t dir_len = utf16_length(dir_temp);
    if (!dir_temp)
        return;

    wchar_t* dir;
    if (dir_temp[dir_len - 1] != L'\\') {
        dir = force_malloc_s(wchar_t, dir_len + 3);
        memcpy(dir, dir_temp, sizeof(wchar_t) * dir_len);
        dir[dir_len] = L'\\';
        dir[dir_len + 1] = L'*';
        dir[dir_len + 2] = 0;
    }
    else {
        dir = force_malloc_s(wchar_t, dir_len + 2);
        memcpy(dir, dir_temp, sizeof(wchar_t) * dir_len);
        dir[dir_len] = L'*';
        dir[dir_len + 1] = 0;
    }
    free(dir_temp);

    WIN32_FIND_DATAW fdata;
    memset(&fdata, 0, sizeof(WIN32_FIND_DATAW));
    HANDLE h = FindFirstFileW(dir, &fdata);
    if (h == INVALID_HANDLE_VALUE) {
        free(dir);
        return;
    }

    do {
        if (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;

        char* file_temp = utf16_to_utf8(fdata.cFileName);
        string* file = vector_old_string_reserve_back(files);
        string_init(file, file_temp);
        free(file_temp);
    } while (FindNextFileW(h, &fdata));
    FindClose(h);
    free(dir);
}

inline void path_get_files(vector_old_string* files, const char* path) {
    path_get_files(files, (char*)path);
}

void path_wget_files(vector_old_wstring* files, wchar_t* path) {
    vector_old_wstring_free(files, wstring_free);

    size_t dir_len = utf16_length(path);

    wchar_t* dir;
    if (path[dir_len - 1] != L'\\') {
        dir = force_malloc_s(wchar_t, dir_len + 3);
        memcpy(dir, path, sizeof(wchar_t) * dir_len);
        dir[dir_len] = L'\\';
        dir[dir_len + 1] = L'*';
        dir[dir_len + 2] = 0;
    }
    else {
        dir = force_malloc_s(wchar_t, dir_len + 2);
        memcpy(dir, path, sizeof(wchar_t) * dir_len);
        dir[dir_len] = L'*';
        dir[dir_len + 1] = 0;
    }

    WIN32_FIND_DATAW fdata;
    memset(&fdata, 0, sizeof(WIN32_FIND_DATAW));
    HANDLE h = FindFirstFileW(dir, &fdata);
    if (h == INVALID_HANDLE_VALUE) {
        free(dir);
        return;
    }

    do {
        if (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;

        wstring* file = vector_old_wstring_reserve_back(files);
        wstring_init(file, fdata.cFileName);
    } while (FindNextFileW(h, &fdata));
    FindClose(h);
    free(dir);
}

inline void path_wget_files(vector_old_wstring* files, const wchar_t* path) {
    path_wget_files(files, (wchar_t*)path);
}

void path_get_directories(vector_old_string* directories, char* path,
    char** exclude_list, size_t exclude_count) {
    vector_old_string_free(directories, string_free);

    wchar_t* dir_temp = utf8_to_utf16(path);
    size_t dir_len = utf16_length(dir_temp);
    if (!dir_temp)
        return;

    wchar_t* dir;
    wchar_t* temp;
    if (dir_temp[dir_len - 1] != L'\\') {
        dir = force_malloc_s(wchar_t, dir_len + 3);
        memcpy(dir, dir_temp, sizeof(wchar_t) * dir_len);
        dir[dir_len] = L'\\';
        dir[dir_len + 1] = L'*';
        dir[dir_len + 2] = 0;

        temp = force_malloc_s(wchar_t, dir_len + 2 + MAX_PATH);
        memcpy(temp, dir_temp, sizeof(wchar_t) * dir_len);
        temp[dir_len] = L'\\';
        dir_len++;
    }
    else {
        dir = force_malloc_s(wchar_t, dir_len + 2);
        memcpy(dir, dir_temp, sizeof(wchar_t) * dir_len);
        dir[dir_len] = L'*';
        dir[dir_len + 1] = 0;

        temp = force_malloc_s(wchar_t, dir_len + 1 + MAX_PATH);
        memcpy(temp, dir_temp, sizeof(wchar_t) * dir_len);
    }
    free(dir_temp);

    WIN32_FIND_DATAW fdata;
    memset(&fdata, 0, sizeof(WIN32_FIND_DATAW));
    HANDLE h = FindFirstFileW(dir, &fdata);
    if (h == INVALID_HANDLE_VALUE) {
        free(dir);
        free(temp);
        return;
    }

    do {
        if (~fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;
        else if (!str_utils_wcompare(fdata.cFileName, L".")
            || !str_utils_wcompare(fdata.cFileName, L".."))
            continue;

        if (exclude_list && exclude_count) {
            size_t len = utf16_length(fdata.cFileName);
            memcpy(temp + dir_len, fdata.cFileName, sizeof(wchar_t) * len);
            temp[dir_len + len] = L'\0';

            bool exclude = false;
            char* temp_utf8 = utf16_to_utf8(temp);
            for (size_t i = 0; i < exclude_count; i++)
                if (!str_utils_compare(exclude_list[i], temp_utf8)) {
                    exclude = true;
                    break;
                }
            free(temp_utf8);

            if (exclude)
                continue;
        }


        char* directory_temp = utf16_to_utf8(fdata.cFileName);
        string* directory = vector_old_string_reserve_back(directories);
        string_init(directory, directory_temp);
        free(directory_temp);
    } while (FindNextFileW(h, &fdata));
    FindClose(h);
    free(temp);
    free(dir);
}

inline void path_get_directories(vector_old_string* directories, const char* path,
    char** exclude_list, size_t exclude_count) {
    path_get_directories(directories, (char*)path, exclude_list, exclude_count);
}

void path_wget_directories(vector_old_wstring* directories, wchar_t* path,
    wchar_t** exclude_list, size_t exclude_count) {
    vector_old_wstring_free(directories, wstring_free);

    size_t dir_len = utf16_length(path);

    wchar_t* dir;
    wchar_t* temp;
    if (path[dir_len - 1] != L'\\') {
        dir = force_malloc_s(wchar_t, dir_len + 3);
        memcpy(dir, path, sizeof(wchar_t) * dir_len);
        dir[dir_len] = L'\\';
        dir[dir_len + 1] = L'*';
        dir[dir_len + 2] = 0;

        temp = force_malloc_s(wchar_t, dir_len + 2 + MAX_PATH);
        memcpy(temp, path, sizeof(wchar_t) * dir_len);
        temp[dir_len] = L'\\';
        dir_len++;
    }
    else {
        dir = force_malloc_s(wchar_t, dir_len + 2);
        memcpy(dir, path, sizeof(wchar_t) * dir_len);
        dir[dir_len] = L'*';
        dir[dir_len + 1] = 0;

        temp = force_malloc_s(wchar_t, dir_len + 1 + MAX_PATH);
        memcpy(temp, path, sizeof(wchar_t) * dir_len);
    }

    WIN32_FIND_DATAW fdata;
    memset(&fdata, 0, sizeof(WIN32_FIND_DATAW));
    HANDLE h = FindFirstFileW(dir, &fdata);
    if (h == INVALID_HANDLE_VALUE) {
        free(dir);
        free(temp);
        return;
    }

    do {
        if (~fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;
        else if (!str_utils_wcompare(fdata.cFileName, L".")
            || !str_utils_wcompare(fdata.cFileName, L".."))
            continue;

        if (exclude_list && exclude_count) {
            size_t len = utf16_length(fdata.cFileName);
            memcpy(temp + dir_len + 1, fdata.cFileName, sizeof(wchar_t) * len);
            temp[dir_len + 1 + len] = L'\0';

            bool exclude = false;
            for (size_t i = 0; i < exclude_count; i++)
                if (!str_utils_wcompare(exclude_list[i], temp)) {
                    exclude = true;
                    break;
                }

            if (exclude)
                continue;
        }

        wstring* directory = vector_old_wstring_reserve_back(directories);
        wstring_init(directory, fdata.cFileName);
    } while (FindNextFileW(h, &fdata));
    FindClose(h);
    free(temp);
    free(dir);
}

inline void path_wget_directories(vector_old_wstring* directories, const wchar_t* path,
    wchar_t** exclude_list, size_t exclude_count) {
    path_wget_directories(directories, (wchar_t*)path, exclude_list, exclude_count);
}

void path_get_directories_recursive(vector_old_string* directories, char* path,
    char** exclude_list, size_t exclude_count) {
    vector_old_string_free(directories, string_free);

    wchar_t* dir_temp = utf8_to_utf16(path);
    size_t dir_len = utf16_length(dir_temp);
    if (!dir_temp)
        return;

    wchar_t* dir;
    wchar_t* temp;
    if (dir_temp[dir_len - 1] != L'\\') {
        dir = force_malloc_s(wchar_t, dir_len + 3);
        memcpy(dir, dir_temp, sizeof(wchar_t) * dir_len);
        dir[dir_len] = L'\\';
        dir[dir_len + 1] = L'*';
        dir[dir_len + 2] = 0;

        temp = force_malloc_s(wchar_t, dir_len + 2 + MAX_PATH);
        memcpy(temp, dir_temp, sizeof(wchar_t) * dir_len);
        temp[dir_len] = L'\\';
        dir_len++;
    }
    else {
        dir = force_malloc_s(wchar_t, dir_len + 2);
        memcpy(dir, dir_temp, sizeof(wchar_t) * dir_len);
        dir[dir_len] = L'*';
        dir[dir_len + 1] = 0;

        temp = force_malloc_s(wchar_t, dir_len + 1 + MAX_PATH);
        memcpy(temp, dir_temp, sizeof(wchar_t) * dir_len);
    }
    free(dir_temp);

    WIN32_FIND_DATAW fdata;
    memset(&fdata, 0, sizeof(WIN32_FIND_DATAW));
    HANDLE h = FindFirstFileW(dir, &fdata);
    if (h == INVALID_HANDLE_VALUE) {
        free(dir);
        free(temp);
        return;
    }

    vector_old_string temp_vec = vector_old_empty(string);
    do {
        if (~fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;
        else if (!str_utils_wcompare(fdata.cFileName, L".")
            || !str_utils_wcompare(fdata.cFileName, L".."))
            continue;

        if (exclude_list && exclude_count) {
            size_t len = utf16_length(fdata.cFileName);
            memcpy(temp + dir_len + 1, fdata.cFileName, sizeof(wchar_t) * len);
            temp[dir_len + 1 + len] = L'\0';

            bool exclude = false;
            char* temp_utf8 = utf16_to_utf8(temp);
            for (size_t i = 0; i < exclude_count; i++)
                if (!str_utils_compare(exclude_list[i], temp_utf8)) {
                    exclude = true;
                    break;
                }
            free(temp_utf8);

            if (exclude)
                continue;
        }

        char* directory_temp = utf16_to_utf8(fdata.cFileName);
        string* directory = vector_old_string_reserve_back(&temp_vec);
        string_init(directory, directory_temp);
        free(directory_temp);
    } while (FindNextFileW(h, &fdata));
    FindClose(h);
    free(temp);
    free(dir);

    size_t max_len = 0;
    for (string* i = temp_vec.begin; i != temp_vec.end; i++) {
        size_t len = i->length;
        if (max_len < len)
            max_len = len;
    }

    size_t path_length = utf8_length(path);
    char* path_temp = force_malloc_s(char, path_length + max_len + 2);
    memcpy(path_temp, path, path_length);
    path_temp[path_length] = '\\';
    for (string* i = temp_vec.begin; i != temp_vec.end; i++) {
        memcpy(&path_temp[path_length + 1], string_data(i), i->length + 1);

        vector_old_string temp = vector_old_empty(string);
        path_get_directories_recursive(&temp, path_temp, exclude_list, exclude_count);

        vector_old_string_push_back(directories, i);

        if (vector_old_length(temp) < 1) {
            vector_old_string_free(&temp, string_free);
            continue;
        }

        max_len = 0;
        for (string* j = temp.begin; j != temp.end; j++) {
            size_t len = j->length;
            if (max_len < len)
                max_len = len;
        }

        size_t sub_path_length = i->length;
        char* sub_path_temp = force_malloc_s(char, sub_path_length + max_len + 1);
        if (sub_path_temp) {
            memcpy(sub_path_temp, string_data(i), sub_path_length);
            sub_path_temp[sub_path_length] = '\\';
            for (string* j = temp.begin; j != temp.end; j++) {
                memcpy(&sub_path_temp[sub_path_length + 1], string_data(j), j->length);

                string* directory = vector_old_string_reserve_back(&temp_vec);
                string_init_length(directory, sub_path_temp, sub_path_length + 1 + j->length);
            }
        }
        free(sub_path_temp);

        vector_old_string_free(&temp, string_free);
    }
    free(path_temp);
    vector_old_string_free(&temp_vec, 0);
}

inline void path_get_directories_recursive(vector_old_string* directories, const char* path,
    char** exclude_list, size_t exclude_count) {
    path_get_directories_recursive(directories, (char*)path, exclude_list, exclude_count);
}

void path_wget_directories_recursive(vector_old_wstring* directories, wchar_t* path,
    wchar_t** exclude_list, size_t exclude_count) {
    vector_old_wstring_free(directories, wstring_free);

    size_t dir_len = utf16_length(path);

    wchar_t* dir;
    wchar_t* temp;
    if (path[dir_len - 1] != L'\\') {
        dir = force_malloc_s(wchar_t, dir_len + 3);
        memcpy(dir, path, sizeof(wchar_t) * dir_len);
        dir[dir_len] = L'\\';
        dir[dir_len + 1] = L'*';
        dir[dir_len + 2] = 0;

        temp = force_malloc_s(wchar_t, dir_len + 2 + MAX_PATH);
        memcpy(temp, path, sizeof(wchar_t) * dir_len);
        temp[dir_len] = L'\\';
        dir_len++;
    }
    else {
        dir = force_malloc_s(wchar_t, dir_len + 2);
        memcpy(dir, path, sizeof(wchar_t) * dir_len);
        dir[dir_len] = L'*';
        dir[dir_len + 1] = 0;

        temp = force_malloc_s(wchar_t, dir_len + 1 + MAX_PATH);
        memcpy(temp, path, sizeof(wchar_t) * dir_len);
    }

    WIN32_FIND_DATAW fdata;
    memset(&fdata, 0, sizeof(WIN32_FIND_DATAW));
    HANDLE h = FindFirstFileW(dir, &fdata);
    if (h == INVALID_HANDLE_VALUE) {
        free(dir);
        free(temp);
        return;
    }

    vector_old_wstring temp_vec = vector_old_empty(wstring);
    do {
        if (~fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;
        else if (!str_utils_wcompare(fdata.cFileName, L".")
            || !str_utils_wcompare(fdata.cFileName, L".."))
            continue;

        if (exclude_list && exclude_count) {
            size_t len = utf16_length(fdata.cFileName);
            memcpy(temp + dir_len + 1, fdata.cFileName, sizeof(wchar_t) * len);
            temp[dir_len + 1 + len] = L'\0';

            bool exclude = false;
            for (size_t i = 0; i < exclude_count; i++)
                if (!str_utils_wcompare(exclude_list[i], temp)) {
                    exclude = true;
                    break;
                }

            if (exclude)
                continue;
        }

        wstring* directory = vector_old_wstring_reserve_back(&temp_vec);
        wstring_init(directory, fdata.cFileName);
    } while (FindNextFileW(h, &fdata));
    FindClose(h);
    free(temp);
    free(dir);

    size_t max_len = 0;
    for (wstring* i = temp_vec.begin; i != temp_vec.end; i++) {
        size_t len = i->length;
        if (max_len < len)
            max_len = len;
    }

    size_t path_length = utf16_length(path);
    wchar_t* path_temp = force_malloc_s(wchar_t, path_length + max_len + 2);
    memcpy(path_temp, path, path_length);
    path_temp[path_length] = '\\';
    for (wstring* i = temp_vec.begin; i != temp_vec.end; i++) {
        memcpy(&path_temp[path_length + 1], wstring_data(i), sizeof(wchar_t) * (i->length + 1));

        vector_old_wstring temp = vector_old_empty(wstring);
        path_wget_directories_recursive(&temp, path_temp, exclude_list, exclude_count);

        vector_old_wstring_push_back(directories, i);

        if (vector_old_length(temp) < 1) {
            vector_old_wstring_free(&temp, wstring_free);
            continue;
        }

        max_len = 0;
        for (wstring* j = temp.begin; j != temp.end; j++) {
            size_t len = j->length;
            if (max_len < len)
                max_len = len;
        }

        size_t sub_path_length = i->length;
        wchar_t* sub_path_temp = force_malloc_s(wchar_t, sub_path_length + max_len + 1);
        if (sub_path_temp) {
            memcpy(sub_path_temp, wstring_data(i), sub_path_length);
            sub_path_temp[sub_path_length] = '\\';
            for (wstring* j = temp.begin; j != temp.end; j++) {
                memcpy(&sub_path_temp[sub_path_length + 1], wstring_data(j), sizeof(wchar_t) * j->length);

                wstring* directory = vector_old_wstring_reserve_back(directories);
                wstring_init_length(directory, sub_path_temp, sub_path_length + 1 + j->length);
            }
        }
        free(sub_path_temp);

        vector_old_wstring_free(&temp, wstring_free);
    }
    free(path_temp);
    vector_old_wstring_free(&temp_vec, 0);
}

inline void path_wget_directories_recursive(vector_old_wstring* directories, const wchar_t* path,
    wchar_t** exclude_list, size_t exclude_count) {
    path_wget_directories_recursive(directories, (wchar_t*)path, exclude_list, exclude_count);
}
