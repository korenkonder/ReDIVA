/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.hpp"
#include "../io/stream.hpp"

extern const char* light_param_read_line(char* buf, int32_t size, const char* src);
extern void light_param_write_int32_t(stream& s, char* buf, size_t buf_size, int32_t value);
extern void light_param_write_float_t(stream& s, char* buf, size_t buf_size, float_t value);
