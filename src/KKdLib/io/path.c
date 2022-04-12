/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "path.h"
#include "../str_utils.h"

bool path_check_path_exists(const char* path) {
    wchar_t* path_temp = utf8_to_utf16(path);
    DWORD ftyp = GetFileAttributesW(path_temp);
    free(path_temp);
    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false;
    else
        return true;
}

bool path_check_path_exists(const wchar_t* path) {
    DWORD ftyp = GetFileAttributesW(path);
    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false;
    else
        return true;
}

bool path_check_file_exists(const char* path) {
    wchar_t* path_temp = utf8_to_utf16(path);
    DWORD ftyp = GetFileAttributesW(path_temp);
    free(path_temp);
    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false;
    else
        return ftyp & FILE_ATTRIBUTE_DIRECTORY ? false : true;
}

bool path_check_file_exists(const wchar_t* path) {
    DWORD ftyp = GetFileAttributesW(path);
    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false;
    else
        return ftyp & FILE_ATTRIBUTE_DIRECTORY ? false : true;
}

bool path_check_directory_exists(const char* path) {
    wchar_t* path_temp = utf8_to_utf16(path);
    DWORD ftyp = GetFileAttributesW(path_temp);
    free(path_temp);
    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false;
    else
        return ftyp & FILE_ATTRIBUTE_DIRECTORY ? true : false;
}

bool path_check_directory_exists(const wchar_t* path) {
    DWORD ftyp = GetFileAttributesW(path);
    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false;
    else
        return ftyp & FILE_ATTRIBUTE_DIRECTORY ? true : false;
}

void path_get_files(std::vector<std::string>* files, const char* path) {
    files->clear();
    files->shrink_to_fit();

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
        if (file_temp)
            files->push_back(std::string(file_temp));
        free(file_temp);
    } while (FindNextFileW(h, &fdata));
    FindClose(h);
    free(dir);
}

void path_get_files(std::vector<std::wstring>* files, const wchar_t* path) {
    files->clear();
    files->shrink_to_fit();

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

        std::wstring file = std::wstring(fdata.cFileName);
        files->push_back(file);
    } while (FindNextFileW(h, &fdata));
    FindClose(h);
    free(dir);
}

void path_get_directories(std::vector<std::string>* directories,
    const char* path, char** exclude_list, size_t exclude_count) {
    directories->clear();
    directories->shrink_to_fit();

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
        else if (!str_utils_compare(fdata.cFileName, L".")
            || !str_utils_compare(fdata.cFileName, L".."))
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
        if (directory_temp)
            directories->push_back(std::string(directory_temp));
        free(directory_temp);
    } while (FindNextFileW(h, &fdata));
    FindClose(h);
    free(temp);
    free(dir);
}

void path_get_directories(std::vector<std::wstring>* directories,
    const wchar_t* path, wchar_t** exclude_list, size_t exclude_count) {
    directories->clear();
    directories->shrink_to_fit();

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
        else if (!str_utils_compare(fdata.cFileName, L".")
            || !str_utils_compare(fdata.cFileName, L".."))
            continue;

        if (exclude_list && exclude_count) {
            size_t len = utf16_length(fdata.cFileName);
            memcpy(temp + dir_len + 1, fdata.cFileName, sizeof(wchar_t) * len);
            temp[dir_len + 1 + len] = L'\0';

            bool exclude = false;
            for (size_t i = 0; i < exclude_count; i++)
                if (!str_utils_compare(exclude_list[i], temp)) {
                    exclude = true;
                    break;
                }

            if (exclude)
                continue;
        }

        std::wstring directory = std::wstring(fdata.cFileName);
        directories->push_back(directory);
    } while (FindNextFileW(h, &fdata));
    FindClose(h);
    free(temp);
    free(dir);
}

void path_get_directories_recursive(std::vector<std::string>* directories,
    const char* path, char** exclude_list, size_t exclude_count) {
    directories->clear();
    directories->shrink_to_fit();

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

    std::vector<std::string> temp_vec;
    do {
        if (~fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;
        else if (!str_utils_compare(fdata.cFileName, L".")
            || !str_utils_compare(fdata.cFileName, L".."))
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
        std::string directory = std::string(directory_temp);
        temp_vec.push_back(directory);
        free(directory_temp);
    } while (FindNextFileW(h, &fdata));
    FindClose(h);
    free(temp);
    free(dir);

    size_t max_len = 0;
    for (std::string& i : temp_vec) {
        size_t len = i.size();
        if (max_len < len)
            max_len = len;
    }

    size_t path_length = utf8_length(path);
    char* path_temp = force_malloc_s(char, path_length + max_len + 2);
    memcpy(path_temp, path, path_length);
    path_temp[path_length] = '\\';
    for (std::string& i : temp_vec) {
        memcpy(&path_temp[path_length + 1], i.c_str(), i.size() + 1);

        std::vector<std::string> temp;
        path_get_directories_recursive(&temp, path_temp, exclude_list, exclude_count);

        directories->push_back(i);

        if (temp.size() < 1)
            continue;

        max_len = 0;
        for (std::string& j : temp) {
            size_t len = j.size();
            if (max_len < len)
                max_len = len;
        }

        size_t sub_path_length = i.size();
        char* sub_path_temp = force_malloc_s(char, sub_path_length + max_len + 1);
        if (sub_path_temp) {
            memcpy(sub_path_temp, i.c_str(), sub_path_length);
            sub_path_temp[sub_path_length] = '\\';
            for (std::string& j : temp) {
                memcpy(&sub_path_temp[sub_path_length + 1], j.c_str(), j.size());

                std::string directory = std::string(sub_path_temp, sub_path_length + 1 + j.size());
                temp_vec.push_back(directory);
            }
        }
        free(sub_path_temp);
    }
    free(path_temp);
}

void path_get_directories_recursive(std::vector<std::wstring>* directories,
    const wchar_t* path, wchar_t** exclude_list, size_t exclude_count) {
    directories->clear();
    directories->shrink_to_fit();

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

    std::vector<std::wstring> temp_vec;
    do {
        if (~fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;
        else if (!str_utils_compare(fdata.cFileName, L".")
            || !str_utils_compare(fdata.cFileName, L".."))
            continue;

        if (exclude_list && exclude_count) {
            size_t len = utf16_length(fdata.cFileName);
            memcpy(temp + dir_len + 1, fdata.cFileName, sizeof(wchar_t) * len);
            temp[dir_len + 1 + len] = L'\0';

            bool exclude = false;
            for (size_t i = 0; i < exclude_count; i++)
                if (!str_utils_compare(exclude_list[i], temp)) {
                    exclude = true;
                    break;
                }

            if (exclude)
                continue;
        }

        std::wstring directory = std::wstring(fdata.cFileName);
        temp_vec.push_back(directory);
    } while (FindNextFileW(h, &fdata));
    FindClose(h);
    free(temp);
    free(dir);

    size_t max_len = 0;
    for (std::wstring& i : temp_vec) {
        size_t len = i.size();
        if (max_len < len)
            max_len = len;
    }

    size_t path_length = utf16_length(path);
    wchar_t* path_temp = force_malloc_s(wchar_t, path_length + max_len + 2);
    memcpy(path_temp, path, path_length);
    path_temp[path_length] = '\\';
    for (std::wstring& i : temp_vec) {
        memcpy(&path_temp[path_length + 1], i.c_str(), sizeof(wchar_t) * (i.size() + 1));

        std::vector<std::wstring> temp;
        path_get_directories_recursive(&temp, path_temp, exclude_list, exclude_count);

        directories->push_back(i);

        if (temp.size() < 1) 
            continue;

        max_len = 0;
        for (std::wstring& j : temp) {
            size_t len = j.size();
            if (max_len < len)
                max_len = len;
        }

        size_t sub_path_length = i.size();
        wchar_t* sub_path_temp = force_malloc_s(wchar_t, sub_path_length + max_len + 1);
        if (sub_path_temp) {
            memcpy(sub_path_temp, i.c_str(), sub_path_length);
            sub_path_temp[sub_path_length] = '\\';
            for (std::wstring& j : temp) {
                memcpy(&sub_path_temp[sub_path_length + 1], j.c_str(), sizeof(wchar_t) * j.size());

                std::wstring directory = std::wstring(sub_path_temp, sub_path_length + 1 + j.size());
                directories->push_back(directory);
            }
        }
        free(sub_path_temp);
    }
    free(path_temp);
}
