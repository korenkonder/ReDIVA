/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.h"
#include "../string.h"
#include "../vector.h"

extern bool path_check_path_exists(char* path);
extern bool path_check_path_exists(const char* path);
extern bool path_wcheck_path_exists(wchar_t* path);
extern bool path_wcheck_path_exists(const wchar_t* path);
extern bool path_check_file_exists(char* path);
extern bool path_check_file_exists(const char* path);
extern bool path_wcheck_file_exists(wchar_t* path);
extern bool path_wcheck_file_exists(const wchar_t* path);
extern bool path_check_directory_exists(char* path);
extern bool path_check_directory_exists(const char* path);
extern bool path_wcheck_directory_exists(wchar_t* path);
extern bool path_wcheck_directory_exists(const wchar_t* path);
extern void path_get_files(vector_old_string* files, char* path);
extern void path_get_files(vector_old_string* files, char* path);
extern void path_wget_files(vector_old_wstring* files, wchar_t* path);
extern void path_wget_files(vector_old_wstring* files, const wchar_t* path);
extern void path_get_directories(vector_old_string* directories, char* path,
    char** exclude_list, size_t exclude_count);
extern void path_get_directories(vector_old_string* directories, const char* path,
    char** exclude_list, size_t exclude_count);
extern void path_wget_directories(vector_old_wstring* directories, wchar_t* path,
    wchar_t** exclude_list, size_t exclude_count);
extern void path_wget_directories(vector_old_wstring* directories, const  wchar_t* path,
    wchar_t** exclude_list, size_t exclude_count);
extern void path_get_directories_recursive(vector_old_string* directories, char* path,
    char** exclude_list, size_t exclude_count);
extern void path_get_directories_recursive(vector_old_string* directories, const char* path,
    char** exclude_list, size_t exclude_count);
extern void path_wget_directories_recursive(vector_old_wstring* directories, wchar_t* path,
    wchar_t** exclude_list, size_t exclude_count);
extern void path_wget_directories_recursive(vector_old_wstring* directories, const  wchar_t* path,
    wchar_t** exclude_list, size_t exclude_count);