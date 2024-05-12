/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "print.hpp"

void printf_divagl(_In_z_ _Printf_format_string_ const char* const fmt, ...) {
    char buf[0x1000];
    va_list args;
    va_start(args, fmt);
    sprintf_s(buf, sizeof(buf), "[DivaGL] %s\n", fmt);
    vprintf(buf, args);
    va_end(args);
}

void printf_proxy(_In_z_ _Printf_format_string_ const char* const fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}
