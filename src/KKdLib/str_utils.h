/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "string.h"

extern bool str_utils_check_ends_with(char* str, char* mask);
extern bool str_utils_check_ends_with(char* str, const char* mask);
extern bool str_utils_check_ends_with(const char* str, char* mask);
extern bool str_utils_check_ends_with(const char* str, const char* mask);
extern bool str_utils_wcheck_ends_with(wchar_t* str, wchar_t* mask);
extern bool str_utils_wcheck_ends_with(wchar_t* str, const wchar_t* mask);
extern bool str_utils_wcheck_ends_with(const wchar_t* str, wchar_t* mask);
extern bool str_utils_wcheck_ends_with(const wchar_t* str, const wchar_t* mask);
extern char* str_utils_get_next_int32_t(char* str, int32_t* value, char separator);
extern char* str_utils_get_next_int32_t(const char* str, int32_t* value, char separator);
extern wchar_t* str_utils_wget_next_int32_t(wchar_t* str, int32_t* value, wchar_t separator);
extern wchar_t* str_utils_wget_next_int32_t(const wchar_t* str, int32_t* value, wchar_t separator);
extern char* str_utils_get_next_float_t(char* str, float_t* value, char separator);
extern char* str_utils_get_next_float_t(const char* str, float_t* value, char separator);
extern wchar_t* str_utils_wget_next_float_t(wchar_t* str, float_t* value, wchar_t separator);
extern wchar_t* str_utils_wget_next_float_t(const wchar_t* str, float_t* value, wchar_t separator);
extern char* str_utils_get_next_string(char* str, string* value, char separator);
extern char* str_utils_get_next_string(const char* str, string* value, char separator);
extern wchar_t* str_utils_wget_next_string(wchar_t* str, wstring* value, wchar_t separator);
extern wchar_t* str_utils_wget_next_string(const wchar_t* str, wstring* value, wchar_t separator);
extern char* str_utils_split_get_right(char* str, char split);
extern char* str_utils_split_get_right(const char* str, char split);
extern wchar_t* str_utils_wsplit_get_right(wchar_t* str, wchar_t split);
extern wchar_t* str_utils_wsplit_get_right(const wchar_t* str, wchar_t split);
extern char* str_utils_split_get_left(char* str, char split);
extern char* str_utils_split_get_left(const char* str, char split);
extern wchar_t* str_utils_wsplit_get_left(wchar_t* str, wchar_t split);
extern wchar_t* str_utils_wsplit_get_left(const wchar_t* str, wchar_t split);
extern char* str_utils_split_get_right_include(char* str, char split);
extern char* str_utils_split_get_right_include(const char* str, char split);
extern wchar_t* str_utils_wsplit_get_right_include(wchar_t* str, wchar_t split);
extern wchar_t* str_utils_wsplit_get_right_include(const wchar_t* str, wchar_t split);
extern char* str_utils_split_get_left_include(char* str, char split);
extern char* str_utils_split_get_left_include(const char* str, char split);
extern wchar_t* str_utils_wsplit_get_left_include(wchar_t* str, wchar_t split);
extern wchar_t* str_utils_wsplit_get_left_include(const wchar_t* str, wchar_t split);
extern char* str_utils_split_right_get_right(char* str, char split);
extern char* str_utils_split_right_get_right(const char* str, char split);
extern wchar_t* str_utils_wsplit_right_get_right(wchar_t* str, wchar_t split);
extern wchar_t* str_utils_wsplit_right_get_right(const wchar_t* str, wchar_t split);
extern char* str_utils_split_right_get_left(char* str, char split);
extern char* str_utils_split_right_get_left(const char* str, char split);
extern wchar_t* str_utils_wsplit_right_get_left(wchar_t* str, wchar_t split);
extern wchar_t* str_utils_wsplit_right_get_left(const wchar_t* str, wchar_t split);
extern char* str_utils_split_right_get_right_include(char* str, char split);
extern char* str_utils_split_right_get_right_include(const char* str, char split);
extern wchar_t* str_utils_wsplit_right_get_right_include(wchar_t* str, wchar_t split);
extern wchar_t* str_utils_wsplit_right_get_right_include(const wchar_t* str, wchar_t split);
extern char* str_utils_split_right_get_left_include(char* str, char split);
extern char* str_utils_split_right_get_left_include(const char* str, char split);
extern wchar_t* str_utils_wsplit_right_get_left_include(wchar_t* str, wchar_t split);
extern wchar_t* str_utils_wsplit_right_get_left_include(const wchar_t* str, wchar_t split);
extern char* str_utils_get_extension(char* str);
extern char* str_utils_get_extension(const char* str);
extern wchar_t* str_utils_wget_extension(wchar_t* str);
extern wchar_t* str_utils_wget_extension(const wchar_t* str);
extern char* str_utils_get_without_extension(char* str);
extern char* str_utils_get_without_extension(const char* str);
extern wchar_t* str_utils_wget_without_extension(wchar_t* str);
extern wchar_t* str_utils_wget_without_extension(const wchar_t* str);
extern char* str_utils_add(char* str0, char* str1);
extern char* str_utils_add(char* str0, const char* str1);
extern char* str_utils_add(const char* str0, char* str1);
extern char* str_utils_add(const char* str0, const char* str1);
extern wchar_t* str_utils_wadd(wchar_t* str0, wchar_t* str1);
extern wchar_t* str_utils_wadd(wchar_t* str0, const wchar_t* str1);
extern wchar_t* str_utils_wadd(const wchar_t* str0, wchar_t* str1);
extern wchar_t* str_utils_wadd(const wchar_t* str0, const wchar_t* str1);
extern char* str_utils_copy(char* str);
extern char* str_utils_copy(const char* str);
extern wchar_t* str_utils_wcopy(wchar_t* str);
extern wchar_t* str_utils_wcopy(const wchar_t* str);
extern int32_t str_utils_compare(char* str0, char* str1);
extern int32_t str_utils_compare(char* str0, const char* str1);
extern int32_t str_utils_compare(const char* str0, char* str1);
extern int32_t str_utils_compare(const char* str0, const char* str1);
extern int32_t str_utils_wcompare(wchar_t* str0, wchar_t* str1);
extern int32_t str_utils_wcompare(wchar_t* str0, const wchar_t* str1);
extern int32_t str_utils_wcompare(const wchar_t* str0, wchar_t* str1);
extern int32_t str_utils_wcompare(const wchar_t* str0, const wchar_t* str1);
extern int32_t str_utils_compare_length(char* str0, size_t str0_len, char* str1, size_t str1_len);
extern int32_t str_utils_compare_length(char* str0, size_t str0_len, const char* str1, size_t str1_len);
extern int32_t str_utils_compare_length(const char* str0, size_t str0_len, char* str1, size_t str1_len);
extern int32_t str_utils_compare_length(const char* str0, size_t str0_len, const char* str1, size_t str1_len);
extern int32_t str_utils_wcompare_length(wchar_t* str0, size_t str0_len, wchar_t* str1, size_t str1_len);
extern int32_t str_utils_wcompare_length(wchar_t* str0, size_t str0_len, const wchar_t* str1, size_t str1_len);
extern int32_t str_utils_wcompare_length(const wchar_t* str0, size_t str0_len, wchar_t* str1, size_t str1_len);
extern int32_t str_utils_wcompare_length(const wchar_t* str0, size_t str0_len, const wchar_t* str1, size_t str1_len);
extern size_t str_utils_get_substring_offset(char* str0, size_t str0_len,
    size_t str0_off, char* str1, size_t str1_len);
extern size_t str_utils_wget_substring_offset(wchar_t* str0, size_t str0_len,
    size_t str0_off, wchar_t* str1, size_t str1_len);
extern bool str_utils_text_file_parse(void* data, size_t length,
    char** buf, char*** lines, size_t* count);
