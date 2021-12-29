/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"
#include "io/stream.h"

extern void divafile_decrypt(char* path);
extern void divafile_wdecrypt(wchar_t* path);
extern void divafile_mdecrypt(void* enc_data, void** dec_data, size_t* dec_size);
extern void divafile_sdecrypt(stream* s);
extern void divafile_encrypt(char* path);
extern void divafile_wencrypt(wchar_t* path);
extern void divafile_mencrypt(void* dec_data, size_t dec_size, void** enc_data, size_t* enc_size);
