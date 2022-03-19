/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "../default.h"

extern bool path_check_path_exists(char* path);
extern bool path_check_path_exists(const char* path);
extern bool path_check_path_exists(wchar_t* path);
extern bool path_check_path_exists(const wchar_t* path);
extern bool path_check_file_exists(char* path);
extern bool path_check_file_exists(const char* path);
extern bool path_check_file_exists(wchar_t* path);
extern bool path_check_file_exists(const wchar_t* path);
extern bool path_check_directory_exists(char* path);
extern bool path_check_directory_exists(const char* path);
extern bool path_check_directory_exists(wchar_t* path);
extern bool path_check_directory_exists(const wchar_t* path);
extern void path_get_files(std::vector<std::string>* files, char* path);
extern void path_get_files(std::vector<std::string>* files, const char* path);
extern void path_get_files(std::vector<std::wstring>* files, wchar_t* path);
extern void path_get_files(std::vector<std::wstring>* files, const wchar_t* path);
extern void path_get_directories(std::vector<std::string>* directories, char* path,
    char** exclude_list, size_t exclude_count);
extern void path_get_directories(std::vector<std::string>* directories, const char* path,
    char** exclude_list, size_t exclude_count);
extern void path_get_directories(std::vector<std::wstring>* directories, wchar_t* path,
    wchar_t** exclude_list, size_t exclude_count);
extern void path_get_directories(std::vector<std::wstring>* directories, const  wchar_t* path,
    wchar_t** exclude_list, size_t exclude_count);
extern void path_get_directories_recursive(std::vector<std::string>* directories, char* path,
    char** exclude_list, size_t exclude_count);
extern void path_get_directories_recursive(std::vector<std::string>* directories, const char* path,
    char** exclude_list, size_t exclude_count);
extern void path_get_directories_recursive(std::vector<std::wstring>* directories, wchar_t* path,
    wchar_t** exclude_list, size_t exclude_count);
extern void path_get_directories_recursive(std::vector<std::wstring>* directories, const  wchar_t* path,
    wchar_t** exclude_list, size_t exclude_count);