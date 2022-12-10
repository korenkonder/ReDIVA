/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../KKdLib/vec.hpp"

enum resolution_mode {
    RESOLUTION_MODE_QVGA     = 0x00,
    RESOLUTION_MODE_VGA      = 0x01,
    RESOLUTION_MODE_SVGA     = 0x02,
    RESOLUTION_MODE_XGA      = 0x03,
    RESOLUTION_MODE_SXGA     = 0x04,
    RESOLUTION_MODE_SXGAPlus = 0x05,
    RESOLUTION_MODE_UXGA     = 0x06,
    RESOLUTION_MODE_WVGA     = 0x07,
    RESOLUTION_MODE_WSVGA    = 0x08,
    RESOLUTION_MODE_WXGA     = 0x09,
    RESOLUTION_MODE_FWXGA    = 0x0A,
    RESOLUTION_MODE_WUXGA    = 0x0B,
    RESOLUTION_MODE_WQXGA    = 0x0C,
    RESOLUTION_MODE_HD       = 0x0D,
    RESOLUTION_MODE_FHD      = 0x0E,
    RESOLUTION_MODE_QHD      = 0x0F,
    RESOLUTION_MODE_WQVGA    = 0x10,
    RESOLUTION_MODE_qHD      = 0x11,
    RESOLUTION_MODE_END      = 0x12,
    RESOLUTION_MODE_13       = 0x13,
};

struct resolution_table_struct {
    int32_t width_full;
    int32_t height_full;
    int32_t field_8;
    int32_t x_offset;
    int32_t y_offset;
    int32_t width;
    int32_t height;
};

struct resolution_struct {
    resolution_mode resolution_mode;
    int32_t x_offset;
    int32_t y_offset;
    int32_t width;
    int32_t height;
    double_t aspect;
    int32_t field_20;
};

extern const resolution_table_struct resolution_table[];

extern vec2 resolution_mode_get_scale(const resolution_mode dst_mode, const resolution_mode src_mode);
extern void resolution_mode_scale_pos(vec2& dst_pos, const resolution_mode dst_mode,
    const vec2& src_pos, const resolution_mode src_mode);
