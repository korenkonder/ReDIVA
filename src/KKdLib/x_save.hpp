/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.hpp"

enum x_save_encode_flags {
    X_SAVE_ENCODE_GZIP    = 0x01,
    X_SAVE_ENCODE_AES     = 0x02,
    X_SAVE_ENCODE_XOR     = 0x04,
    X_SAVE_ENCODE_CRC     = 0x08,
    X_SAVE_ENCODE_DEFAULT = X_SAVE_ENCODE_CRC | X_SAVE_ENCODE_XOR | X_SAVE_ENCODE_AES | X_SAVE_ENCODE_GZIP,
};

extern bool x_save_decode(const char* in_path, const char* out_path);
extern bool x_save_decode(const wchar_t* in_path, const wchar_t* out_path);
extern bool x_save_encode(const char* in_path, const char* out_path,
    x_save_encode_flags flags = X_SAVE_ENCODE_DEFAULT);
extern bool x_save_encode(const wchar_t* in_path, const wchar_t* out_path,
    x_save_encode_flags flags = X_SAVE_ENCODE_DEFAULT);
