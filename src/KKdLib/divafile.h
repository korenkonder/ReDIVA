/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "io/stream.h"

extern void divafile_decrypt(const char* path);
extern void divafile_decrypt(const wchar_t* path);
extern void divafile_decrypt(void* enc_data, void** dec_data, size_t* dec_size);
extern void divafile_decrypt(stream* s);
extern void divafile_encrypt(const char* path);
extern void divafile_encrypt(const wchar_t* path);
extern void divafile_encrypt(void* dec_data, size_t dec_size, void** enc_data, size_t* enc_size);
