/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "../default.hpp"

extern bool path_check_path_exists(_In_z_ const char* path);
extern bool path_check_path_exists(_In_z_ const wchar_t* path);
extern bool path_check_file_exists(_In_z_ const char* path);
extern bool path_check_file_exists(_In_z_ const wchar_t* path);
extern bool path_check_directory_exists(_In_z_ const char* path);
extern bool path_check_directory_exists(_In_z_ const wchar_t* path);
extern std::vector<std::string> path_get_files(_In_z_ const char* path);
extern std::vector<std::wstring> path_get_files(_In_z_ const wchar_t* path);
extern std::vector<std::string> path_get_directories(
    _In_z_ const char* path, _In_opt_ const char** exclude_list = 0, _In_opt_ size_t exclude_count = 0);
extern std::vector<std::wstring> path_get_directories(
    _In_z_ const wchar_t* path, _In_opt_ const wchar_t** exclude_list = 0, _In_opt_ size_t exclude_count = 0);
extern std::vector<std::string> path_get_directories_recursive(
    _In_z_ const char* path, _In_opt_ const char** exclude_list = 0, _In_opt_ size_t exclude_count = 0);
extern std::vector<std::wstring> path_get_directories_recursive(
    _In_z_ const wchar_t* path, _In_opt_ const wchar_t** exclude_list = 0, _In_opt_ size_t exclude_count = 0);
extern void path_get_full_path(_Inout_ std::string& str);
extern void path_get_full_path(_Inout_ std::wstring& str);

extern bool path_create_file(_In_z_ const char* path);
extern bool path_create_file(_In_z_ const wchar_t* path);
extern bool path_create_directory(_In_z_ const char* path);
extern bool path_create_directory(_In_z_ const wchar_t* path);
extern bool path_delete_file(_In_z_ const char* path);
extern bool path_delete_file(_In_z_ const wchar_t* path);
extern bool path_delete_directory(_In_z_ const char* path);
extern bool path_delete_directory(_In_z_ const wchar_t* path);
extern bool path_copy_file(_In_z_ const char* src, _In_z_ const char* dst);
extern bool path_copy_file(_In_z_ const wchar_t* src, _In_z_ const wchar_t* dst);
extern _Check_return_ int32_t path_compare_files(_In_z_ const char* path_1, _In_z_ const char* path_2);
extern _Check_return_ int32_t path_compare_files(_In_z_ const wchar_t* path_1, _In_z_ const wchar_t* path_2);
extern bool path_fs_copy_file(_In_z_ const char* src, _In_z_ const char* dst);
extern bool path_fs_copy_file(_In_z_ const wchar_t* src, _In_z_ const wchar_t* dst);
extern bool path_move_file(_In_z_ const char* old_path, _In_z_ const char* new_path);
extern bool path_move_file(_In_z_ const wchar_t* old_path, _In_z_ const wchar_t* new_path);
