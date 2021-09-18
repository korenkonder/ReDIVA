/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"

#define STRING_LENGTH 24
#define WSTRING_LENGTH 12

typedef struct string {
    union {
        char data[STRING_LENGTH];
        char* ptr;
    };
    uint64_t length;
} string;

typedef struct wstring {
    union {
        wchar_t data[WSTRING_LENGTH];
        wchar_t* ptr;
    };
    uint64_t length;
} wstring;

extern void string_init(string* s, char* str);
extern char* string_data(string* s);
extern void string_add(string* s, char* str);
extern void string_copy(string* src, string* dst);
extern void string_copy_wstring(wstring* src, string* dst);
extern size_t string_length(string* s);
extern void string_free(string* s);
extern void wstring_init(wstring* s, wchar_t* str);
extern wchar_t* wstring_access(wstring* s);
extern void wstring_add(wstring* s, wchar_t* str);
extern void wstring_copy(wstring* src, wstring* dst);
extern void wstring_copy_string(string* src, wstring* dst);
extern size_t wstring_length(wstring* s);
extern void wstring_free(wstring* s);
