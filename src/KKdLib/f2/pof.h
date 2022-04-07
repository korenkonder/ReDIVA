/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <vector>
#include "../default.h"
#include "../io/stream.h"

class pof {
public:
    std::vector<size_t> vec;

    pof();
    ~pof();

    void add(stream* s, size_t offset);
    void read(stream* s, bool shift_x = false);
    void write(stream* s, bool shift_x = false);
    uint32_t length(bool shift_x = false);
};

extern void io_write_offset_pof_add(stream* s, ssize_t val,
    int32_t offset, bool is_x, pof* pof);
extern void io_write_offset_f2_pof_add(stream* s, ssize_t val,
    int32_t offset, pof* pof);
extern void io_write_offset_x_pof_add(stream* s, ssize_t val, pof* pof);
