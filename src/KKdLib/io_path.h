/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "string.h"
#include "vector.h"

extern bool path_check_path_exists(char* path);
extern bool path_wcheck_path_exists(wchar_t* path);
extern bool path_check_file_exists(char* path);
extern bool path_wcheck_file_exists(wchar_t* path);
extern bool path_check_directory_exists(char* path);
extern bool path_wcheck_directory_exists(wchar_t* path);
extern void path_get_files(vector_string* files, char* path);
extern void path_wget_files(vector_wstring* files, wchar_t* path);