/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "path.hpp"
#include "../str_utils.hpp"
#include "file_stream.hpp"
#include <fstream>

bool path_check_path_exists(const char* path) {
    wchar_t* path_temp = utf8_to_utf16(path);
    DWORD ftyp = GetFileAttributesW(path_temp);
    free_def(path_temp);
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
    free_def(path_temp);
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
    free_def(path_temp);
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

std::vector<std::string> path_get_files(const char* path) {
    wchar_t* dir_temp = utf8_to_utf16(path);
    size_t dir_len = utf16_length(dir_temp);
    if (!dir_temp)
        return {};

    std::wstring dir;
    dir.assign(dir_temp, dir_len);
    if (dir.size() && dir.back() != L'\\' && dir.back() != L'/')
        dir.push_back(L'\\');
    dir.push_back(L'*');
    free_def(dir_temp);

    WIN32_FIND_DATAW fdata = {};
    HANDLE h = FindFirstFileW(dir.c_str(), &fdata);
    if (h == INVALID_HANDLE_VALUE)
        return {};

    std::vector<std::string> files;
    do {
        if (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;

        char* file_temp = utf16_to_utf8(fdata.cFileName);
        if (file_temp) {
            files.push_back(file_temp);
            free(file_temp);
        }
    } while (FindNextFileW(h, &fdata));
    FindClose(h);
    return files;
}

std::vector<std::wstring> path_get_files(const wchar_t* path) {
    size_t dir_len = utf16_length(path);

    std::wstring dir;
    dir.assign(path, dir_len);
    if (dir.size() && dir.back() != L'\\' && dir.back() != L'/')
        dir.push_back(L'\\');
    dir.push_back(L'*');

    WIN32_FIND_DATAW fdata = {};
    HANDLE h = FindFirstFileW(dir.c_str(), &fdata);
    if (h == INVALID_HANDLE_VALUE)
        return {};

    std::vector<std::wstring> files;
    do {
        if (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;

        files.push_back(fdata.cFileName);
    } while (FindNextFileW(h, &fdata));
    FindClose(h);
    return files;
}

std::vector<std::string> path_get_directories(
    const char* path, const char** exclude_list, size_t exclude_count) {
    wchar_t* dir_temp = utf8_to_utf16(path);
    size_t dir_len = utf16_length(dir_temp);
    if (!dir_temp)
        return {};

    std::wstring dir;
    std::wstring temp;
    dir.assign(dir_temp, dir_len);
    if (dir.size() && dir.back() != L'\\' && dir.back() != L'/')
        dir.push_back(L'\\');
    temp.assign(dir);
    dir.push_back(L'*');
    free_def(dir_temp);

    WIN32_FIND_DATAW fdata = {};
    HANDLE h = FindFirstFileW(dir.c_str(), &fdata);
    if (h == INVALID_HANDLE_VALUE)
        return {};

    std::vector<std::string> directories;
    do {
        if (!(fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            || !str_utils_compare(fdata.cFileName, L".")
            || !str_utils_compare(fdata.cFileName, L".."))
            continue;

        if (exclude_list && exclude_count) {
            size_t len = utf16_length(fdata.cFileName);
            temp.append(fdata.cFileName, len);
            std::string temp_utf8 = utf16_to_utf8(temp);

            bool exclude = false;
            for (size_t i = 0; i < exclude_count; i++) {
                size_t exclude_path_len = utf8_length(exclude_list[i]);
                size_t pos = temp_utf8.rfind(exclude_list[i], -1, exclude_path_len);
                if (pos != -1 && pos == temp_utf8.size() - exclude_path_len) {
                    exclude = true;
                    break;
                }
            }
            temp.resize(temp.size() - len);

            if (exclude)
                continue;
        }

        char* directory_temp = utf16_to_utf8(fdata.cFileName);
        if (directory_temp)
            directories.push_back(directory_temp);
        free_def(directory_temp);
    } while (FindNextFileW(h, &fdata));
    FindClose(h);
    return directories;
}

std::vector<std::wstring> path_get_directories(
    const wchar_t* path, const wchar_t** exclude_list, size_t exclude_count) {
    size_t dir_len = utf16_length(path);

    std::wstring dir;
    std::wstring temp;
    dir.assign(path, dir_len);
    if (dir.size() && dir.back() != L'\\' && dir.back() != L'/')
        dir.push_back(L'\\');
    temp.assign(dir);
    dir.push_back(L'*');

    WIN32_FIND_DATAW fdata = {};
    HANDLE h = FindFirstFileW(dir.c_str(), &fdata);
    if (h == INVALID_HANDLE_VALUE)
        return {};

    std::vector<std::wstring> directories;
    do {
        if (!(fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            || !str_utils_compare(fdata.cFileName, L".")
            || !str_utils_compare(fdata.cFileName, L".."))
            continue;

        if (exclude_list && exclude_count) {
            size_t len = utf16_length(fdata.cFileName);
            temp.append(fdata.cFileName, len);

            bool exclude = false;
            for (size_t i = 0; i < exclude_count; i++) {
                size_t exclude_path_len = utf16_length(exclude_list[i]);
                size_t pos = temp.rfind(exclude_list[i], -1, exclude_path_len);
                if (pos != -1 && pos == temp.size() - exclude_path_len) {
                    exclude = true;
                    break;
                }
            }
            temp.resize(temp.size() - len);

            if (exclude)
                continue;
        }

        directories.push_back(fdata.cFileName);
    } while (FindNextFileW(h, &fdata));
    FindClose(h);
    return directories;
}

std::vector<std::string> path_get_directories_recursive(
    const char* path, const char** exclude_list, size_t exclude_count) {
    wchar_t* dir_temp = utf8_to_utf16(path);
    size_t dir_len = utf16_length(dir_temp);
    if (!dir_temp)
        return {};

    std::wstring dir;
    std::wstring temp;
    dir.assign(dir_temp, dir_len);
    if (dir.size() && dir.back() != L'\\' && dir.back() != L'/')
        dir.push_back(L'\\');
    temp.assign(dir);
    dir.push_back(L'*');
    free_def(dir_temp);

    WIN32_FIND_DATAW fdata = {};
    HANDLE h = FindFirstFileW(dir.c_str(), &fdata);
    if (h == INVALID_HANDLE_VALUE)
        return {};

    std::vector<std::string> temp_vec;
    do {
        if (!(fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            || !str_utils_compare(fdata.cFileName, L".")
            || !str_utils_compare(fdata.cFileName, L".."))
            continue;

        if (exclude_list && exclude_count) {
            size_t len = utf16_length(fdata.cFileName);
            temp.append(fdata.cFileName, len);
            std::string temp_utf8 = utf16_to_utf8(temp);

            bool exclude = false;
            for (size_t i = 0; i < exclude_count; i++) {
                size_t exclude_path_len = utf8_length(exclude_list[i]);
                size_t pos = temp_utf8.rfind(exclude_list[i], -1, exclude_path_len);
                if (pos != -1 && pos == temp_utf8.size() - exclude_path_len) {
                    exclude = true;
                    break;
                }
            }
            temp.resize(temp.size() - len);

            if (exclude)
                continue;
        }

        char* directory_temp = utf16_to_utf8(fdata.cFileName);
        temp_vec.push_back(directory_temp);
        free_def(directory_temp);
    } while (FindNextFileW(h, &fdata));
    FindClose(h);

    size_t max_len = 0;
    for (std::string& i : temp_vec) {
        size_t len = i.size();
        if (max_len < len)
            max_len = len;
    }

    std::vector<std::string> directories;
    std::string path_temp;
    path_temp.assign(path);
    path_temp += '\\';
    for (std::string& i : temp_vec) {
        path_temp.append(i);
        std::vector<std::string> temp = path_get_directories_recursive(
            path_temp.c_str(), exclude_list, exclude_count);
        path_temp.resize(path_temp.size() - i.size());

        directories.push_back(i);

        if (temp.size() < 1)
            continue;

        max_len = 0;
        for (std::string& j : temp) {
            size_t len = j.size();
            if (max_len < len)
                max_len = len;
        }

        if (i.size()) {
            std::string sub_path_temp;
            sub_path_temp.assign(i);
            sub_path_temp += '\\';
            for (std::string& j : temp)
                directories.push_back(sub_path_temp + j);
        }
    }
    return directories;
}

std::vector<std::wstring> path_get_directories_recursive(
    const wchar_t* path, const wchar_t** exclude_list, size_t exclude_count) {
    size_t dir_len = utf16_length(path);

    std::wstring dir;
    std::wstring temp;
    dir.assign(path, dir_len);
    if (dir.size() && dir.back() != L'\\' && dir.back() != L'/')
        dir.push_back(L'\\');
    temp.assign(dir);
    dir.push_back(L'*');

    WIN32_FIND_DATAW fdata = {};
    HANDLE h = FindFirstFileW(dir.c_str(), &fdata);
    if (h == INVALID_HANDLE_VALUE)
        return {};

    std::vector<std::wstring> temp_vec;
    do {
        if (!(fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            || !str_utils_compare(fdata.cFileName, L".")
            || !str_utils_compare(fdata.cFileName, L".."))
            continue;

        if (exclude_list && exclude_count) {
            size_t len = utf16_length(fdata.cFileName);
            temp.append(fdata.cFileName, len);

            bool exclude = false;
            for (size_t i = 0; i < exclude_count; i++) {
                size_t exclude_path_len = utf16_length(exclude_list[i]);
                size_t pos = temp.rfind(exclude_list[i], -1, exclude_path_len);
                if (pos != -1 && pos == temp.size() - exclude_path_len) {
                    exclude = true;
                    break;
                }
            }
            temp.resize(temp.size() - len);

            if (exclude)
                continue;
        }

        std::wstring directory = std::wstring(fdata.cFileName);
        temp_vec.push_back(directory);
    } while (FindNextFileW(h, &fdata));
    FindClose(h);

    size_t max_len = 0;
    for (std::wstring& i : temp_vec) {
        size_t len = i.size();
        if (max_len < len)
            max_len = len;
    }

    std::vector<std::wstring> directories;
    std::wstring path_temp;
    path_temp.assign(path);
    path_temp.push_back(L'\\');
    for (std::wstring& i : temp_vec) {
        path_temp.append(i);
        std::vector<std::wstring> temp = path_get_directories_recursive(
            path_temp.c_str(), exclude_list, exclude_count);
        path_temp.resize(path_temp.size() - i.size());

        directories.push_back(i);

        if (temp.size() < 1)
            continue;

        max_len = 0;
        for (std::wstring& j : temp) {
            size_t len = j.size();
            if (max_len < len)
                max_len = len;
        }

        if (i.size()) {
            std::wstring sub_path_temp;
            sub_path_temp.assign(i);
            sub_path_temp.push_back(L'\\');
            for (std::wstring& j : temp)
                directories.push_back(sub_path_temp + j);
        }
    }
    return directories;
}

void path_get_full_path(std::string& str) {
    wchar_t buf[MAX_PATH * 2];
    buf[0] = 0;
    wchar_t* utf16_temp = utf8_to_utf16(str.c_str());
    if (utf16_temp)
        GetFullPathNameW(utf16_temp, MAX_PATH * 2, buf, 0);
    free_def(utf16_temp);

    char* utf0_temp = utf16_to_utf8(buf);
    if (utf0_temp)
        str.assign(utf0_temp);
    free_def(utf0_temp);
}

void path_get_full_path(std::wstring& str) {
    wchar_t buf[MAX_PATH * 2];
    buf[0] = 0;
    GetFullPathNameW(str.c_str(), MAX_PATH * 2, buf, 0);
    str.assign(buf);
}

bool path_create_file(const char* path) {
    file_stream fs;
    fs.open(path, "wb");
    bool ret = fs.check_not_null();
    fs.close();
    return ret;
}

bool path_create_file(const wchar_t* path) {
    file_stream fs;
    fs.open(path, L"wb");
    bool ret = fs.check_not_null();
    fs.close();
    return ret;
}

bool path_create_directory(const char* path) {
    wchar_t* path_temp = utf8_to_utf16(path);
    const wchar_t* _path = path_temp;
    while (true) {
        const wchar_t* c = wcschr(_path, L'\\');
        if (!c)
            c = wcschr(_path, L'/');

        if (!c)
            break;

        _path = c + 1;
        std::wstring temp(path_temp, c - path_temp);
        if (!path_check_directory_exists(temp.c_str()) && !CreateDirectoryW(temp.c_str(), 0)) {
            free_def(path_temp);
            return false;
        }
    }

    bool ret = path_check_directory_exists(path_temp) || CreateDirectoryW(path_temp, 0);
    free_def(path_temp);
    return ret;
}

bool path_create_directory(const wchar_t* path) {
    const wchar_t* _path = path;
    while (true) {
        const wchar_t* c = wcschr(_path, L'\\');
        if (!c)
            c = wcschr(_path, L'/');

        if (!c)
            break;

        _path = c + 1;
        std::wstring temp(path, c - path);
        if (!path_check_directory_exists(temp.c_str()) && !CreateDirectoryW(temp.c_str(), 0))
            return false;
    }

    return path_check_directory_exists(path) || CreateDirectoryW(path, 0);
}

bool path_delete_file(const char* path) {
    wchar_t* path_temp = utf8_to_utf16(path);
    bool ret = !path_check_file_exists(path_temp) || DeleteFileW(path_temp);
    free_def(path_temp);
    return ret;
}

bool path_delete_file(const wchar_t* path) {
    return !path_check_file_exists(path) || DeleteFileW(path);
}

bool path_delete_directory(const char* path) {
    std::string dir;
    dir.assign(path);
    if (dir.size() && dir.back() != L'\\' && dir.back() != L'/')
        dir.push_back(L'\\');

    std::vector<std::string> files = path_get_files(path);
    for (std::string& i : files)
        path_delete_file((dir + i).c_str());

    std::vector<std::string> directories = path_get_directories(path);
    for (std::string& i : directories)
        path_delete_directory((dir + i).c_str());

    wchar_t* path_temp = utf8_to_utf16(path);
    bool ret = !path_check_directory_exists(path_temp) || RemoveDirectoryW(path_temp);
    free_def(path_temp);
    return ret;
}

bool path_delete_directory(const wchar_t* path) {
    std::wstring dir;
    dir.assign(path);
    if (dir.size() && dir.back() != L'\\' && dir.back() != L'/')
        dir.push_back(L'\\');

    std::vector<std::wstring> files = path_get_files(path);
    for (std::wstring& i : files)
        path_delete_file((dir + i).c_str());

    std::vector<std::wstring> directories = path_get_directories(path);
    for (std::wstring& i : directories)
        path_delete_directory((dir + i).c_str());

    return !path_check_directory_exists(path) || RemoveDirectoryW(path);
}

bool path_copy_file(const char* src, const char* dst) {
    if (!path_check_file_exists(src)
        || path_check_directory_exists(dst))
        return false;

    wchar_t* src_temp = utf8_to_utf16(src);
    wchar_t* dst_temp = utf8_to_utf16(dst);
    std::ifstream ifs(src_temp, std::ios::in | std::ios::binary);
    std::ofstream ofs(dst_temp, std::ios::out | std::ios::trunc | std::ios::binary);
    free_def(src_temp);
    free_def(dst_temp);

    if (!ifs.is_open() || !ofs.is_open())
        return false;

    ofs << ifs.rdbuf();

    ifs.close();
    ofs.close();
    return true;
}

bool path_copy_file(const wchar_t* src, const wchar_t* dst) {
    if (!path_check_file_exists(src)
        || path_check_directory_exists(dst))
        return false;

    std::ifstream ifs(src, std::ios::in | std::ios::binary);
    std::ofstream ofs(dst, std::ios::out | std::ios::trunc | std::ios::binary);

    if (!ifs.is_open() || !ofs.is_open())
        return false;

    ofs << ifs.rdbuf();

    ifs.close();
    ofs.close();
    return true;
}

int32_t path_compare_files(const char* path_1, const char* path_2) {
    wchar_t* path_1_temp = utf8_to_utf16(path_1);
    wchar_t* path_2_temp = utf8_to_utf16(path_2);

    int32_t ret;
    if (!path_check_path_exists(path_1_temp))
        ret = -1;
    else if (!path_check_path_exists(path_2_temp))
        ret = 1;
    else if (!path_check_file_exists(path_1_temp) || !path_check_file_exists(path_2_temp))
        ret = -1;
    else {
        WIN32_FILE_ATTRIBUTE_DATA fad_1;
        WIN32_FILE_ATTRIBUTE_DATA fad_2;
        GetFileAttributesExW(path_1_temp, GetFileExInfoStandard, &fad_1);
        GetFileAttributesExW(path_2_temp, GetFileExInfoStandard, &fad_2);

        LARGE_INTEGER size_1;
        size_1.HighPart = fad_1.nFileSizeHigh;
        size_1.LowPart = fad_1.nFileSizeLow;

        LARGE_INTEGER size_2;
        size_2.HighPart = fad_2.nFileSizeHigh;
        size_2.LowPart = fad_2.nFileSizeLow;

        if (size_1.QuadPart == size_2.QuadPart) {
            ret = 0;

            std::ifstream ifs_1(path_1_temp, std::ios::in | std::ios::binary);
            std::ifstream ifs_2(path_2_temp, std::ios::in | std::ios::binary);
            while (ifs_1.good() && ifs_2.good()) {
                char buf_1[0x200];
                char buf_2[0x200];
                ifs_1.read(buf_1, sizeof(buf_1));
                ifs_2.read(buf_2, sizeof(buf_2));
                if (ifs_1.gcount() != ifs_2.gcount()) {
                    ret = -1;
                    break;
                }
                else if (memcmp(buf_1, buf_2, ifs_1.gcount())) {
                    ret = 1;
                    break;
                }
            }
        }
        else
            ret = 1;
    }

    free_def(path_1_temp);
    free_def(path_2_temp);
    return ret;
}

int32_t path_compare_files(const wchar_t* path_1, const wchar_t* path_2) {
    int32_t ret;
    if (!path_check_path_exists(path_1))
        ret = -1;
    else if (!path_check_path_exists(path_2))
        ret = 1;
    else if (!path_check_file_exists(path_1) || !path_check_file_exists(path_2))
        ret = -1;
    else {
        WIN32_FILE_ATTRIBUTE_DATA fad_1;
        WIN32_FILE_ATTRIBUTE_DATA fad_2;
        GetFileAttributesExW(path_1, GetFileExInfoStandard, &fad_1);
        GetFileAttributesExW(path_2, GetFileExInfoStandard, &fad_2);

        LARGE_INTEGER size_1;
        size_1.HighPart = fad_1.nFileSizeHigh;
        size_1.LowPart = fad_1.nFileSizeLow;

        LARGE_INTEGER size_2;
        size_2.HighPart = fad_2.nFileSizeHigh;
        size_2.LowPart = fad_2.nFileSizeLow;

        if (size_1.QuadPart == size_2.QuadPart) {
            ret = 0;

            std::ifstream ifs_1(path_1, std::ios::in | std::ios::binary);
            std::ifstream ifs_2(path_2, std::ios::in | std::ios::binary);
            while (ifs_1.good() && ifs_2.good()) {
                char buf_1[0x200];
                char buf_2[0x200];
                ifs_1.read(buf_1, sizeof(buf_1));
                ifs_2.read(buf_2, sizeof(buf_2));
                if (ifs_1.gcount() != ifs_2.gcount()) {
                    ret = -1;
                    break;
                }
                else if (memcmp(buf_1, buf_2, ifs_1.gcount())) {
                    ret = 1;
                    break;
                }
            }
        }
        else
            ret = 1;
    }
    return ret;
}

bool path_fs_copy_file(const char* src, const char* dst) {
    std::string _dst(dst);
    _dst.append(".fs_copy_file.tmp");
    if (path_copy_file(src, _dst.c_str()))
        return path_move_file(_dst.c_str(), dst);
    return false;
}

bool path_fs_copy_file(const wchar_t* src, const wchar_t* dst) {
    std::wstring _dst(dst);
    _dst.append(L".fs_copy_file.tmp");
    if (path_copy_file(src, _dst.c_str()))
        return path_move_file(_dst.c_str(), dst);
    return false;
}

bool path_move_file(const char* old_path, const char* new_path) {
    wchar_t* old_path_temp = utf8_to_utf16(old_path);
    wchar_t* new_path_temp = utf8_to_utf16(new_path);
    path_delete_file(new_path_temp);
    bool ret = MoveFileW(old_path_temp, new_path_temp);
    free_def(old_path_temp);
    free_def(new_path_temp);
    return ret;
}

bool path_move_file(const wchar_t* old_path, const wchar_t* new_path) {
    path_delete_file(new_path);
    return MoveFileW(old_path, new_path);
}
