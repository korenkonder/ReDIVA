/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"

extern bool str_utils_check_ends_with(char* str, char* mask);
extern bool str_utils_wcheck_ends_with(wchar_t* str, wchar_t* mask);
extern char* str_utils_split_get_right(char* str, char split);
extern wchar_t* str_utils_wsplit_get_right(wchar_t* str, wchar_t split);
extern char* str_utils_split_get_left(char* str, char split);
extern wchar_t* str_utils_wsplit_get_left(wchar_t* str, wchar_t split);
extern char* str_utils_split_get_right_include(char* str, char split);
extern wchar_t* str_utils_wsplit_get_right_include(wchar_t* str, wchar_t split);
extern char* str_utils_split_get_left_include(char* str, char split);
extern wchar_t* str_utils_wsplit_get_left_include(wchar_t* str, wchar_t split);
extern char* str_utils_split_right_get_right(char* str, char split);
extern wchar_t* str_utils_wsplit_right_get_right(wchar_t* str, wchar_t split);
extern char* str_utils_split_right_get_left(char* str, char split);
extern wchar_t* str_utils_wsplit_right_get_left(wchar_t* str, wchar_t split);
extern char* str_utils_split_right_get_right_include(char* str, char split);
extern wchar_t* str_utils_wsplit_right_get_right_include(wchar_t* str, wchar_t split);
extern char* str_utils_split_right_get_left_include(char* str, char split);
extern wchar_t* str_utils_wsplit_right_get_left_include(wchar_t* str, wchar_t split);
extern char* str_utils_get_extension(char* str);
extern wchar_t* str_utils_wget_extension(wchar_t* str);
extern char* str_utils_get_without_extension(char* str);
extern wchar_t* str_utils_wget_without_extension(wchar_t* str);
extern char* str_utils_add(char* str0, char* str1);
extern wchar_t* str_utils_wadd(wchar_t* str0, wchar_t* str1);
extern char* str_utils_copy(char* str);
extern wchar_t* str_utils_wcopy(wchar_t* str);
