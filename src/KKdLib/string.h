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

extern void string_init(string* c, char* str);
extern char* string_access(string* c);
extern void string_dispose(string* c);
extern void wstring_init(wstring* c, wchar_t* str);
extern wchar_t* wstring_access(wstring* c);
extern void wstring_dispose(wstring* c);
