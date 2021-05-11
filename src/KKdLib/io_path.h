/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "vector.h"

extern bool path_check_path_exists(char* path);
extern bool path_wcheck_path_exists(wchar_t* path);
extern bool path_check_file_exists(char* file_path);
extern bool path_wcheck_file_exists(wchar_t* file_path);
extern bool path_check_directory_exists(char* dir_path);
extern bool path_wcheck_directory_exists(wchar_t* dir_path);
extern void path_get_files(vector_ptr_char* files, char* dir_path);
extern void path_wget_files(vector_ptr_wchar_t* files, wchar_t* dir_path);