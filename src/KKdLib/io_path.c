/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "io_path.h"

bool path_check_path_exists(char* path) {
    DWORD ftyp = GetFileAttributesA(path);
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

bool path_check_file_exists(char* file_path) {
    DWORD ftyp = GetFileAttributesA(file_path);
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

bool path_check_directory_exists(char* dir_path) {
    DWORD ftyp = GetFileAttributesA(dir_path);
    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false;
    else
        return ftyp & FILE_ATTRIBUTE_DIRECTORY ? true : false;
}

bool path_wcheck_directory_exists(wchar_t* dir_path) {
    DWORD ftyp = GetFileAttributesW(dir_path);
    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false;
    else
        return ftyp & FILE_ATTRIBUTE_DIRECTORY ? true : false;
}

bool path_check_ends_with(char* str, char* mask) {
    size_t mask_len = strlen(mask);
    size_t len = strlen(str);
    char* t = str;
    while (t) {
        t = strstr(t, mask);
        if (t) {
            t += mask_len;
            if (t == str + len)
                return true;
        }
    }
    return false;
}

bool path_wcheck_ends_with(wchar_t* str, wchar_t* mask) {
    size_t mask_len = wcslen(mask);
    size_t len = wcslen(str);
    wchar_t* t = str;
    while (t) {
        t = wcsstr(t, mask);
        if (t) {
            t += mask_len;
            if (t == str + len)
                return true;
        }
    }
    return false;
}

void path_get_files(vector_ptr_char* files, char* dir_path) {
    vector_ptr_char_free(files, 0);

    size_t dir_len = strlen(dir_path);
    char* temp = force_malloc(dir_len + 2 + MAX_PATH);
    memcpy(temp, dir_path, dir_len);
    temp[dir_len] = L'\\';

    char* dir = force_malloc(dir_len + 3);
    memcpy(dir, dir_path, dir_len);
    dir[dir_len] = L'\\';
    dir[dir_len + 1] = L'*';
    dir[dir_len + 2] = 0;

    WIN32_FIND_DATAA fdata;
    memset(&fdata, 0, sizeof(WIN32_FIND_DATAA));
    size_t count = 0;
    HANDLE hFind = FindFirstFileA(dir, &fdata);
    if (hFind == INVALID_HANDLE_VALUE) {
        free(dir);
        free(temp);
        return;
    }

    do
        count++;
    while (FindNextFileA(hFind, &fdata));

    bool* found = force_malloc_s(sizeof(bool*), count);

    hFind = FindFirstFileA(dir, &fdata);
    size_t t_count = 0;
    for (size_t i = 0; i < count; i++) {
        size_t len = strlen(fdata.cFileName);
        memcpy(temp + dir_len + 1, fdata.cFileName, len);
        temp[dir_len + 1 + len] = L'\0';
        found[i] = path_check_file_exists(temp);

        if (found[i])
            t_count++;

        if (!FindNextFileA(hFind, &fdata))
            break;
    }

    if (!t_count) {
        free(found);
        free(dir);
        free(temp);
        return;
    }

    vector_ptr_char_append(files, t_count);
    hFind = FindFirstFileA(dir, &fdata);
    for (size_t i = 0, j = 0; i < count; i++) {
        if (found[i]) {
            size_t len = strlen(fdata.cFileName);
            char* file = force_malloc_s(sizeof(char*), len + 1);
            memcpy(file, fdata.cFileName, (len + 1));
            vector_ptr_char_push_back(files, &file);
            j++;
        }

        if (!FindNextFileA(hFind, &fdata))
            break;
    }
    free(found);
    free(dir);
    free(temp);
}

void path_wget_files(vector_ptr_wchar_t* files, wchar_t* dir_path) {
    vector_ptr_wchar_t_free(files, 0);

    size_t dir_len = wcslen(dir_path);
    wchar_t* dir = force_malloc_s(sizeof(wchar_t), dir_len + 3);
    memcpy(dir, dir_path, sizeof(wchar_t) * dir_len);
    dir[dir_len] = L'\\';
    dir[dir_len + 1] = L'*';
    dir[dir_len + 2] = 0;

    WIN32_FIND_DATAW fdata;
    memset(&fdata, 0, sizeof(WIN32_FIND_DATAW));
    size_t count = 0;
    HANDLE hFind = FindFirstFileW(dir, &fdata);
    if (hFind == INVALID_HANDLE_VALUE) {
        free(dir);
        return;
    }

    do
        count++;
    while (FindNextFileW(hFind, &fdata));

    bool* found = force_malloc_s(sizeof(bool), count);

    wchar_t* temp = force_malloc_s(sizeof(wchar_t), dir_len + 2 + MAX_PATH);
    memcpy(temp, dir_path, sizeof(wchar_t) * dir_len);
    temp[dir_len] = L'\\';
    hFind = FindFirstFileW(dir, &fdata);
    size_t t_count = 0;
    for (size_t i = 0; i < count; i++) {
        size_t len = wcslen(fdata.cFileName);
        memcpy(temp + dir_len + 1, fdata.cFileName, sizeof(wchar_t) * len);
        temp[dir_len + 1 + len] = L'\0';
        found[i] = path_wcheck_file_exists(temp);

        if (found[i])
            t_count++;

        if (!FindNextFileW(hFind, &fdata))
            break;
    }
    free(temp);

    if (!t_count) {
        free(found);
        free(dir);
        return;
    }

    vector_ptr_wchar_t_append(files, t_count);
    hFind = FindFirstFileW(dir, &fdata);
    for (size_t i = 0, j = 0; i < count; i++) {
        if (found[i]) {
            size_t len = wcslen(fdata.cFileName);
            wchar_t* file = force_malloc_s(sizeof(wchar_t*), len + 1);
            memcpy(file, fdata.cFileName, sizeof(wchar_t) * (len + 1));
            vector_ptr_wchar_t_push_back(files, &file);
            j++;
        }

        if (!FindNextFileW(hFind, &fdata))
            break;
    }
    free(found);
    free(dir);
}

char* path_get_extension(char* path) {
    char* t = strrchr(path, '\\');
    t = strrchr(t ? t : path, '.');
    size_t len = t ? t - path : 0;
    char* ext = force_malloc(len + 1);
    memcpy(ext, t ? t : path, len + 1);
    return ext;
}

wchar_t* path_wget_extension(wchar_t* path) {
    wchar_t* t = wcsrchr(path, L'\\');
    t = wcsrchr(t ? t : path, L'.');
    size_t len = t ? t - path : 0;
    wchar_t* ext = force_malloc_s(sizeof(wchar_t), len + 1);
    memcpy(ext, t ? t : path, sizeof(wchar_t) * (len + 1));
    return ext;
}

char* path_get_without_extension(char* path) {
    char* t = strrchr(path, '\\');
    t = strrchr(t ? t : path, '.');
    size_t len = t ? t - path : strlen(path);
    char* p = force_malloc(len + 1);
    memcpy(p, path, len);
    p[len] = '\0';
    return p;
}

wchar_t* path_wget_without_extension(wchar_t* path) {
    wchar_t* t = wcsrchr(path, L'\\');
    t = wcsrchr(t ? t : path, L'.');
    size_t len = t ? t - path : wcslen(path);
    wchar_t* p = force_malloc_s(sizeof(wchar_t), len + 1);
    memcpy(p, path, sizeof(wchar_t) * len);
    p[len] = L'\0';
    return p;
}

char* path_add_extension(char* path, char* ext) {
    if (!path)
        return 0;

    size_t len = strlen(path);
    size_t ext_len = ext ? strlen(ext) : 0;
    char* p = force_malloc(len + ext_len + 1);
    memcpy(p, path, len);
    memcpy(p + len, ext, ext_len);
    p[len + ext_len] = '\0';
    return p;
}

wchar_t* path_wadd_extension(wchar_t* path, wchar_t* ext) {
    if (!path)
        return 0;

    size_t len = wcslen(path);
    size_t ext_len = ext ? wcslen(ext) : 0;
    wchar_t* p = force_malloc_s(sizeof(wchar_t), len + ext_len + 1);
    memcpy(p, path, sizeof(wchar_t) * len);
    memcpy(p + len, ext, sizeof(wchar_t) * ext_len);
    p[len + ext_len] = L'\0';
    return p;
}
