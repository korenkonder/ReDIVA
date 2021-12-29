/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "vector.h"

#define STRING_LENGTH (32 - sizeof(size_t) * 2)
#define STRING_NULL_LENGTH (STRING_LENGTH - 1)
#define WSTRING_LENGTH ((32 - sizeof(size_t) * 2) / sizeof(wchar_t))
#define WSTRING_NULL_LENGTH (WSTRING_LENGTH - 1)

typedef struct string {
    union {
        char data[STRING_LENGTH];
        char* ptr;
    };
    size_t length;
    size_t capacity;
} string;

typedef struct wstring {
    union {
        wchar_t data[WSTRING_LENGTH];
        wchar_t* ptr;
    };
    size_t length;
    size_t capacity;
} wstring;

extern string string_empty;
extern wstring wstring_empty;

vector(string)
vector(wstring)
vector_ptr(string)
vector_ptr(wstring)

extern void string_init(string* s, char* str);
extern void string_init_length(string* s, char* str, size_t length);
extern char* string_data(string* s);
extern void string_add(string* s, char* str);
extern void string_add_length(string* s, char* str, size_t length);
extern void string_add_char(string* s, char c);
extern void string_copy(string* src, string* dst);
extern void string_copy_wstring(wstring* src, string* dst);
extern void string_replace(string* src, string* dst);
extern void string_replace_wstring(wstring* src, string* dst);
extern bool string_compare(string* str0, string* str1);
extern void string_free(string* s);
extern void wstring_init(wstring* s, wchar_t* str);
extern void wstring_init_length(wstring* s, wchar_t* str, size_t length);
extern wchar_t* wstring_data(wstring* s);
extern void wstring_add(wstring* s, wchar_t* str);
extern void wstring_add_length(wstring* s, wchar_t* str, size_t length);
extern void wstring_add_char(wstring* s, wchar_t c);
extern void wstring_copy(wstring* src, wstring* dst);
extern void wstring_copy_string(string* src, wstring* dst);
extern void wstring_replace(wstring* src, wstring* dst);
extern void wstring_replace_string(string* src, wstring* dst);
extern bool wstring_compare(wstring* str0, wstring* str1);
extern void wstring_free(wstring* s);
