/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.hpp"
#include "prj/shared_ptr.hpp"
#include "prj/stack_allocator.hpp"
#include "txp.hpp"

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
    RESOLUTION_MODE_MAX      = 0x12,

    // MM+
    /*RESOLUTION_MODE_QVGA          = 0x00,
    RESOLUTION_MODE_VGA           = 0x01,
    RESOLUTION_MODE_SVGA          = 0x02,
    RESOLUTION_MODE_XGA           = 0x03,
    RESOLUTION_MODE_SXGA          = 0x04,
    RESOLUTION_MODE_SXGAPlus      = 0x05,
    RESOLUTION_MODE_UXGA          = 0x06,
    RESOLUTION_MODE_WVGA          = 0x07,
    RESOLUTION_MODE_WSVGA         = 0x08,
    RESOLUTION_MODE_WXGA          = 0x09,
    RESOLUTION_MODE_FWXGA         = 0x0A,
    RESOLUTION_MODE_WUXGA         = 0x0B,
    RESOLUTION_MODE_WQXGA         = 0x0C,
    RESOLUTION_MODE_HD            = 0x0D,
    RESOLUTION_MODE_FHD           = 0x0E,
    RESOLUTION_MODE_UHD           = 0x0F,
    RESOLUTION_MODE_3KatUHD       = 0x10,
    RESOLUTION_MODE_3K            = 0x11,
    RESOLUTION_MODE_QHD           = 0x12,
    RESOLUTION_MODE_WQVGA         = 0x13,
    RESOLUTION_MODE_qHD           = 0x14,
    RESOLUTION_MODE_XGAPlus       = 0x15,
    RESOLUTION_MODE_1176x664      = 0x16,
    RESOLUTION_MODE_1200x960      = 0x17,
    RESOLUTION_MODE_WXGA1280x900  = 0x18,
    RESOLUTION_MODE_SXGAMinus     = 0x19,
    RESOLUTION_MODE_FWXGA1366x768 = 0x1A,
    RESOLUTION_MODE_WXGAPlus      = 0x1B,
    RESOLUTION_MODE_HDPlus        = 0x1C,
    RESOLUTION_MODE_WSXGA         = 0x1D,
    RESOLUTION_MODE_WSXGAPlus     = 0x1E,
    RESOLUTION_MODE_1920x1440     = 0x1F,
    RESOLUTION_MODE_QWXGA         = 0x20,
    RESOLUTION_MODE_MAX           = 0x21,*/
};

namespace spr {
    struct SprInfo {
        uint32_t texid;
        int32_t rotate;
        float_t su;
        float_t sv;
        float_t eu;
        float_t ev;
        float_t px;
        float_t py;
        float_t width;
        float_t height;
    };
};

struct SpriteData {
    uint32_t attr;
    resolution_mode resolution_mode;
};

struct spr_set {
    bool ready;
    bool modern;
    bool big_endian;
    bool is_x;

    uint32_t flag;
    int32_t num_of_texture;
    int32_t num_of_sprite;
    spr::SprInfo* sprinfo;
    const char** texname;
    const char** sprname;
    SpriteData* sprdata;

    txp_set* txp;

    spr_set();

    void move_data(spr_set* set_src, prj::shared_ptr<prj::stack_allocator> alloc);
    void pack_file(void** data, size_t* size);
    void unpack_file(prj::shared_ptr<prj::stack_allocator> alloc, const void* data, size_t size, bool modern);
};
