/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "default.hpp"

inline _Check_return_ int32_t str_utils_compare(_In_z_ const char* str0, _In_z_ const char* str1) {
    return strcmp(str0, str1);
}

inline _Check_return_ int32_t str_utils_compare(_In_z_ const wchar_t* str0, _In_z_ const wchar_t* str1) {
    return wcscmp(str0, str1);
}

extern _Check_return_ bool str_utils_check_ends_with(
    _In_z_ const char* str, _In_z_ const char* mask);
extern _Check_return_ bool str_utils_check_ends_with(
    _In_z_ const wchar_t* str, _In_z_ const wchar_t* mask);
extern _Check_return_ _Ret_maybenull_ const char* str_utils_get_next_int32_t(
    _In_opt_z_ const char* str, _Out_ int32_t& value, _In_ const char split);
extern _Check_return_ _Ret_maybenull_ const wchar_t* str_utils_get_next_int32_t(
    _In_opt_z_ const wchar_t* str, _Out_ int32_t& value, _In_ const wchar_t split);
extern _Check_return_ _Ret_maybenull_ const char* str_utils_get_next_float_t(
    _In_opt_z_ const char* str, _Out_ float_t& value, _In_ const char split);
extern _Check_return_ _Ret_maybenull_ const wchar_t* str_utils_get_next_float_t(
    _In_opt_z_ const wchar_t* str, _Out_ float_t& value, _In_ const wchar_t split);
extern _Check_return_ _Ret_maybenull_ const char* str_utils_get_next_string(
    _In_opt_z_ const char* str, _Out_ std::string& value, _In_ const char split);
extern _Check_return_ _Ret_maybenull_ const wchar_t* str_utils_get_next_string(
    _In_opt_z_ const wchar_t* str, _Out_ std::wstring& value, _In_ const wchar_t split);
extern _Check_return_ _Ret_maybenull_ char* str_utils_split_get_right(
    _In_z_ const char* str, _In_ const char split);
extern _Check_return_ _Ret_maybenull_ wchar_t* str_utils_split_get_right(
    _In_z_ const wchar_t* str, _In_ const wchar_t split);
extern _Check_return_ _Ret_maybenull_ char* str_utils_split_get_left(
    _In_z_ const char* str, _In_ const char split);
extern _Check_return_ _Ret_maybenull_ wchar_t* str_utils_split_get_left(
    _In_z_ const wchar_t* str, _In_ const wchar_t split);
extern _Check_return_ _Ret_maybenull_ char* str_utils_split_get_right_include(
    _In_z_ const char* str, _In_ const char split);
extern _Check_return_ _Ret_maybenull_ wchar_t* str_utils_split_get_right_include(
    _In_z_ const wchar_t* str, _In_ const wchar_t split);
extern _Check_return_ _Ret_maybenull_ char* str_utils_split_get_left_include(
    _In_z_ const char* str, _In_ const char split);
extern _Check_return_ _Ret_maybenull_ wchar_t* str_utils_split_get_left_include(
    _In_z_ const wchar_t* str, _In_ const wchar_t split);
extern _Check_return_ _Ret_maybenull_ char* str_utils_split_right_get_right(
    _In_z_ const char* str, _In_ const char split);
extern _Check_return_ _Ret_maybenull_ wchar_t* str_utils_split_right_get_right(
    _In_z_ const wchar_t* str, _In_ const wchar_t split);
extern _Check_return_ _Ret_maybenull_ char* str_utils_split_right_get_left(
    _In_z_ const char* str, _In_ const char split);
extern _Check_return_ _Ret_maybenull_ wchar_t* str_utils_split_right_get_left(
    _In_z_ const wchar_t* str, _In_ const wchar_t split);
extern _Check_return_ _Ret_maybenull_ char* str_utils_split_right_get_right_include(
    _In_z_ const char* str, _In_ const char split);
extern _Check_return_ _Ret_maybenull_ wchar_t* str_utils_split_right_get_right_include(
    _In_z_ const wchar_t* str, _In_ const wchar_t split);
extern _Check_return_ _Ret_maybenull_ char* str_utils_split_right_get_left_include(
    _In_z_ const char* str, _In_ const char split);
extern _Check_return_ _Ret_maybenull_ wchar_t* str_utils_split_right_get_left_include(
    _In_z_ const wchar_t* str, _In_ const wchar_t split);
extern _Check_return_ _Ret_maybenull_ char* str_utils_get_extension(
    _In_z_ const char* str);
extern _Check_return_ _Ret_maybenull_ wchar_t* str_utils_get_extension(
    _In_z_ const wchar_t* str);
extern _Check_return_ _Ret_maybenull_ char* str_utils_get_without_extension(
    _In_z_ const char* str);
extern _Check_return_ _Ret_maybenull_ wchar_t* str_utils_get_without_extension(
    _In_z_ const wchar_t* str);
extern _Check_return_ _Ret_maybenull_ char* str_utils_add(
    _In_z_ const char* str0, _In_z_ const char* str1);
extern _Check_return_ _Ret_maybenull_ wchar_t* str_utils_add(
    _In_z_ const wchar_t* str0, _In_z_ const wchar_t* str1);
extern _Check_return_ _Ret_maybenull_ char* str_utils_copy(
    _In_z_ const char* str);
extern _Check_return_ _Ret_maybenull_ wchar_t* str_utils_copy(
    _In_z_ const wchar_t* str);
extern _Check_return_ int32_t str_utils_compare_length(
    _In_z_ const char* str0, _In_ size_t str0_len, _In_z_ const char* str1, _In_ size_t str1_len);
extern _Check_return_ int32_t str_utils_compare_length(
    _In_z_ const wchar_t* str0, _In_ size_t str0_len, _In_z_ const wchar_t* str1, _In_ size_t str1_len);
extern _Check_return_ size_t str_utils_get_substring_offset(
    _In_z_ const char* str0, _In_ size_t str0_len, _In_ size_t str0_off,
    _In_z_ const char* str1, _In_ size_t str1_len);
extern _Check_return_ size_t str_utils_get_substring_offset(
    _In_z_ const wchar_t* str0, _In_ size_t str0_len, _In_ size_t str0_off,
    _In_z_ const wchar_t* str1, _In_ size_t str1_len);
extern _Check_return_ bool str_utils_text_file_parse(
    _In_ const void* data, _In_ size_t size, _Out_ char*& buf, _Out_ char**& lines, _Out_ size_t& count);
