/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "len_array_pointer.h"

extern bool path_check_path_exists(char* path);
extern bool path_wcheck_path_exists(wchar_t* path);
extern bool path_check_file_exists(char* file_path);
extern bool path_wcheck_file_exists(wchar_t* file_path);
extern bool path_check_directory_exists(char* dir_path);
extern bool path_wcheck_directory_exists(wchar_t* dir_path);
extern bool path_check_ends_with(char* str, char* mask);
extern bool path_wcheck_ends_with(wchar_t* str, wchar_t* mask);
extern void path_get_files(len_array_pointer_char* files, char* dir_path);
extern void path_wget_files(len_array_pointer_wchar_t* files, wchar_t* dir_path);
extern char* path_get_extension(char* path);
extern wchar_t* path_wget_extension(wchar_t* path);
extern char* path_get_without_extension(char* path);
extern wchar_t* path_wget_without_extension(wchar_t* path);
extern char* path_add_extension(char* path, char* ext);
extern wchar_t* path_wadd_extension(wchar_t* path, wchar_t* ext);
