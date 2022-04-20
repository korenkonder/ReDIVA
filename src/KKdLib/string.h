/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "vector.h"

#define STRING_LENGTH (32 - sizeof(ssize_t) * 2)
#define STRING_NULL_LENGTH (STRING_LENGTH - 1)
#define WSTRING_LENGTH ((32 - sizeof(ssize_t) * 2) / sizeof(wchar_t))
#define WSTRING_NULL_LENGTH (WSTRING_LENGTH - 1)

typedef struct string {
    union {
        char data[STRING_LENGTH];
        char* ptr;
    };
    ssize_t length;
    ssize_t capacity;
} string;

typedef struct wstring {
    union {
        wchar_t data[WSTRING_LENGTH];
        wchar_t* ptr;
    };
    ssize_t length;
    ssize_t capacity;
} wstring;

extern const string string_empty;
extern const wstring wstring_empty;

vector_old(string)
vector_old(wstring)
vector_old_ptr(string)
vector_old_ptr(wstring)

extern void string_init(string* s);
extern void string_init(string* s, const char* str);
extern void string_init_length(string* s, ssize_t length);
extern void string_init_length(string* s, const char* str, ssize_t length);
extern char* string_data(string* s);
extern const char* string_data(const string* s);
extern void string_add(string* s, const char* str);
extern void string_add_length(string* s, const char* str, ssize_t length);
extern void string_add_char(string* s, char c);
extern void string_copy(string* src, string* dst);
extern void string_copy_wstring(wstring* src, string* dst);
extern void string_replace(string* src, string* dst);
extern void string_replace_wstring(wstring* src, string* dst);
extern bool string_compare(string* str0, string* str1);
extern void string_free(string* s);
extern void wstring_init(wstring* s);
extern void wstring_init(wstring* s, const wchar_t* str);
extern void wstring_init_length(wstring* s, ssize_t length);
extern void wstring_init_length(wstring* s, const wchar_t* str, ssize_t length);
extern wchar_t* wstring_data(wstring* s);
extern void wstring_add(wstring* s, const wchar_t* str);
extern void wstring_add_length(wstring* s, const wchar_t* str, ssize_t length);
extern void wstring_add_char(wstring* s, wchar_t c);
extern void wstring_copy(wstring* src, wstring* dst);
extern void wstring_copy_string(string* src, wstring* dst);
extern void wstring_replace(wstring* src, wstring* dst);
extern void wstring_replace_string(string* src, wstring* dst);
extern bool wstring_compare(wstring* str0, wstring* str1);
extern void wstring_free(wstring* s);
