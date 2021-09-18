/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "io_path.h"

bool path_check_path_exists(char* path) {
    wchar_t* path_temp = utf8_to_utf16(path);
    DWORD ftyp = GetFileAttributesW(path_temp);
    free(path_temp);
    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false;
    else
        return true;
}

bool path_wcheck_path_exists(wchar_t* path) {
    DWORD ftyp = GetFileAttributesW(path);
    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false;
    else
        return true;
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

bool path_wcheck_file_exists(wchar_t* file_path) {
    DWORD ftyp = GetFileAttributesW(file_path);
    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false;
    else
        return ftyp & FILE_ATTRIBUTE_DIRECTORY ? false : true;
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

bool path_wcheck_directory_exists(wchar_t* path) {
    DWORD ftyp = GetFileAttributesW(path);
    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false;
    else
        return ftyp & FILE_ATTRIBUTE_DIRECTORY ? true : false;
}

void path_get_files(vector_string* files, char* path) {
    vector_string_free(files);

    wchar_t* dir_temp = utf8_to_utf16(path);
    size_t dir_len = utf16_length(dir_temp);
    if (!dir_temp)
        return;

    wchar_t* dir = force_malloc_s(wchar_t, dir_len + 3);
    memcpy(dir, dir_temp, sizeof(wchar_t) * dir_len);
    dir[dir_len] = L'\\';
    dir[dir_len + 1] = L'*';
    dir[dir_len + 2] = 0;

    wchar_t* temp = force_malloc_s(wchar_t, dir_len + 2 + MAX_PATH);
    memcpy(temp, dir_temp, sizeof(wchar_t) * dir_len);
    temp[dir_len] = L'\\';
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
        size_t len = utf16_length(fdata.cFileName);
        memcpy(temp + dir_len + 1, fdata.cFileName, sizeof(wchar_t) * len);
        temp[dir_len + 1 + len] = L'\0';

        if (path_wcheck_file_exists(temp)) {
            char* file_temp = utf16_to_utf8(fdata.cFileName);
            string file;
            string_init(&file, file_temp);
            vector_string_push_back(files, &file);
            free(file_temp);
        }
    } while (FindNextFileW(h, &fdata));
    FindClose(h);
    free(temp);
    free(dir);
}

void path_wget_files(vector_wstring* files, wchar_t* path) {
    vector_wstring_free(files);

    size_t dir_len = utf16_length(path);
    wchar_t* dir = force_malloc_s(wchar_t, dir_len + 3);
    memcpy(dir, path, sizeof(wchar_t) * dir_len);
    dir[dir_len] = L'\\';
    dir[dir_len + 1] = L'*';
    dir[dir_len + 2] = 0;

    wchar_t* temp = force_malloc_s(wchar_t, dir_len + 2 + MAX_PATH);
    memcpy(temp, path, sizeof(wchar_t) * dir_len);
    temp[dir_len] = L'\\';

    WIN32_FIND_DATAW fdata;
    memset(&fdata, 0, sizeof(WIN32_FIND_DATAW));
    HANDLE h = FindFirstFileW(dir, &fdata);
    if (h == INVALID_HANDLE_VALUE) {
        free(dir);
        free(temp);
        return;
    }

    do {
        size_t len = utf16_length(fdata.cFileName);
        memcpy(temp + dir_len + 1, fdata.cFileName, sizeof(wchar_t) * len);
        temp[dir_len + 1 + len] = L'\0';

        if (path_wcheck_file_exists(temp)) {
            wstring file;
            wstring_init(&file, fdata.cFileName);
            vector_wstring_push_back(files, &file);
        }
    } while (FindNextFileW(h, &fdata));
    FindClose(h);
    free(temp);
    free(dir);
}
