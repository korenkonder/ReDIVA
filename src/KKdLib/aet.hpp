/*
    by korenkonder
    GitHub/GitLab: korenkonder

    Original: AetSet.bt Version: 5.0 by samyuu
*/

#pragma once

#include <string>
#include <vector>
#include "default.hpp"
#include "kf.hpp"

enum aet_blend_mode : uint8_t {
    AET_BLEND_MODE_NONE                    =  0, // Unsupported
    AET_BLEND_MODE_COPY                    =  1, // Unsupported
    AET_BLEND_MODE_BEHIND                  =  2, // Unsupported
    AET_BLEND_MODE_NORMAL                  =  3,
    AET_BLEND_MODE_DISSOLVE                =  4, // Unsupported
    AET_BLEND_MODE_ADD                     =  5,
    AET_BLEND_MODE_MULTIPLY                =  6,
    AET_BLEND_MODE_SCREEN                  =  7,
    AET_BLEND_MODE_OVERLAY                 =  8, // Unsupported
    AET_BLEND_MODE_SOFT_LIGHT              =  9, // Unsupported
    AET_BLEND_MODE_HARD_LIGHT              = 10, // Unsupported
    AET_BLEND_MODE_DARKER                  = 11, // Unsupported
    AET_BLEND_MODE_LIGHTEN                 = 12, // Unsupported
    AET_BLEND_MODE_CLASSIC_DIFFERENCE      = 13, // Unsupported
    AET_BLEND_MODE_HUE                     = 14, // Unsupported
    AET_BLEND_MODE_SATURATION              = 15, // Unsupported
    AET_BLEND_MODE_COLOR                   = 16, // Unsupported
    AET_BLEND_MODE_LUMINOSITY              = 17, // Unsupported
    AET_BLEND_MODE_STENCIL_ALPHA           = 18, // Unsupported
    AET_BLEND_MODE_STENCIL_LUMA            = 19, // Unsupported
    AET_BLEND_MODE_SILHOUETTE_ALPHA        = 20, // Unsupported
    AET_BLEND_MODE_SILHOUETTE_LUMA         = 21, // Unsupported
    AET_BLEND_MODE_LUMINESCENT_PREMULTYPLY = 22, // Unsupported
    AET_BLEND_MODE_ALPHA_ADD               = 23, // Unsupported
    AET_BLEND_MODE_CLASSIC_COLOR_DODGE     = 24, // Unsupported
    AET_BLEND_MODE_CLASSIC_COLOR_BURN      = 25, // Unsupported
    AET_BLEND_MODE_EXCLUSION               = 26, // Unsupported
    AET_BLEND_MODE_DIFFERENCE              = 27, // Unsupported
    AET_BLEND_MODE_COLOR_DODGE             = 28, // Unsupported
    AET_BLEND_MODE_COLOR_BURN              = 29, // Unsupported
    AET_BLEND_MODE_LINEAR_DODGE            = 30, // Unsupported
    AET_BLEND_MODE_LINEAR_BURN             = 31, // Unsupported
    AET_BLEND_MODE_LINEAR_LIGHT            = 32, // Unsupported
    AET_BLEND_MODE_VIVID_LIGHT             = 33, // Unsupported
    AET_BLEND_MODE_PIN_LIGHT               = 34, // Unsupported
    AET_BLEND_MODE_HARD_MIX                = 35, // Unsupported
    AET_BLEND_MODE_LIGHTER_COLOR           = 36, // Unsupported
    AET_BLEND_MODE_DARKER_COLOR            = 37, // Unsupported
    AET_BLEND_MODE_SUBTRACT                = 38, // Unsupported
    AET_BLEND_MODE_DIVIDE                  = 39, // Unsupported
};

enum aet_item_type : uint8_t {
    AET_ITEM_TYPE_NONE        = 0,
    AET_ITEM_TYPE_VIDEO       = 1,
    AET_ITEM_TYPE_AUDIO       = 2,
    AET_ITEM_TYPE_COMPOSITION = 3,
};

enum aet_layer_flags : uint16_t {
    AET_LAYER_FLAGS_VIDEO_ACTIVE                 = 0x000001,
    AET_LAYER_FLAGS_AUDIO_ACTIVE                 = 0x000002,
    AET_LAYER_FLAGS_EFFECTS_ACTIVE               = 0x000004,
    AET_LAYER_FLAGS_MOTION_BLUR                  = 0x000008,
    AET_LAYER_FLAGS_FRAME_BLENDING               = 0x000010,
    AET_LAYER_FLAGS_LOCKED                       = 0x000020,
    AET_LAYER_FLAGS_SHY                          = 0x000040,
    AET_LAYER_FLAGS_COLLAPSE                     = 0x000080,
    AET_LAYER_FLAGS_AUTO_ORIENT_ROTATION         = 0x000100,
    AET_LAYER_FLAGS_ADJUSTMENT_LAYER             = 0x000200,
    AET_LAYER_FLAGS_TIME_REMAPPING               = 0x000400,
    AET_LAYER_FLAGS_LAYER_IS_3D                  = 0x000800,
    AET_LAYER_FLAGS_LOOK_AT_CAMERA               = 0x001000,
    AET_LAYER_FLAGS_LOOK_AT_POINT_OF_INTEREST    = 0x002000,
    AET_LAYER_FLAGS_SOLO                         = 0x004000,
    AET_LAYER_FLAGS_MARKERS_LOCKED               = 0x008000,
    // Should be there but they don't fit in 16 bits
    //AET_LAYER_FLAGS_NULL_LAYER                   = 0x010000,
    //AET_LAYER_FLAGS_HIDE_LOCKED_MASKS            = 0x020000,
    //AET_LAYER_FLAGS_GUIDE_LAYER                  = 0x040000,
    //AET_LAYER_FLAGS_ADVANCED_FRAME_BLENDING      = 0x080000,
    //AET_LAYER_FLAGS_SUB_LAYERS_RENDER_SEPARATELY = 0x100000,
    //AET_LAYER_FLAGS_ENVIRONMENT_LAYER            = 0x200000,
};

enum aet_layer_quality : uint8_t {
    AET_LAYER_QUALITY_NONE      = 0,
    AET_LAYER_QUALITY_WIREFRAME = 1,
    AET_LAYER_QUALITY_DRAFT     = 2,
    AET_LAYER_QUALITY_BEST      = 3,
};

enum aet_track_matte : uint8_t {
    AET_TRACK_MATTE_NONE      = 0,
    AET_TRACK_MATTE_ALPHA     = 1,
    AET_TRACK_MATTE_NOT_ALPHA = 2,
    AET_TRACK_MATTE_LUMA      = 3,
    AET_TRACK_MATTE_NOT_LUMA  = 4,
};

enum aet_transfer_flags : uint8_t {
    AET_TRANSFRER_FLAGS_PRESERVE_ALPHA     = 0x01,
    AET_TRANSFRER_FLAGS_RANDOMIZE_DISSOLVE = 0x02,
};

struct aet_marker {
    float_t time;
    std::string name;

    aet_marker();
    ~aet_marker();
};

typedef std::vector<kft2> aet_fcurve;

struct aet_layer_video_3d {
    aet_fcurve anchor_z;
    aet_fcurve pos_z;
    aet_fcurve dir_x;
    aet_fcurve dir_y;
    aet_fcurve dir_z;
    aet_fcurve rot_x;
    aet_fcurve rot_y;
    aet_fcurve scale_z;

    aet_layer_video_3d();
    ~aet_layer_video_3d();
};

struct aet_transfer_mode {
    aet_blend_mode mode;
    aet_transfer_flags flag;
    aet_track_matte matte;

    aet_transfer_mode();
};

struct aet_layer_video {
    aet_transfer_mode transfer_mode;
    aet_fcurve anchor_x;
    aet_fcurve anchor_y;
    aet_fcurve pos_x;
    aet_fcurve pos_y;
    aet_fcurve rot_z;
    aet_fcurve scale_x;
    aet_fcurve scale_y;
    aet_fcurve opacity;
    aet_layer_video_3d _3d;
    bool _3d_init;

    aet_layer_video();
    ~aet_layer_video();
};

struct aet_layer_audio {
    aet_fcurve volume_l;
    aet_fcurve volume_r;
    aet_fcurve pan_l;
    aet_fcurve pan_r;

    aet_layer_audio();
    ~aet_layer_audio();
};

struct aet_color {
    uint8_t r;
    uint8_t g;
    uint8_t b;

    aet_color();
};

struct aet_video_src {
    std::string sprite_name;
    int32_t sprite_index;

    aet_video_src();
    ~aet_video_src();
};

struct aet_video {
    aet_color color;
    uint16_t width;
    uint16_t height;
    float_t fpf;
    std::vector<aet_video_src> sources;

    aet_video();
    ~aet_video();
};

struct aet_audio {
    uint32_t sound_index;

    aet_audio();
};

struct aet_layer {
    std::string name;
    float_t start_time;
    float_t end_time;
    float_t offset_time;
    float_t time_scale;
    aet_layer_flags flags;
    aet_layer_quality quality;
    aet_item_type item_type;
    void* item;
    aet_layer* parent;
    std::vector<aet_marker> markers;
    aet_layer_video video;
    bool video_init;
    aet_layer_audio audio;
    bool audio_init;

    aet_layer();
    ~aet_layer();
};

typedef std::vector<aet_layer> aet_comp;

struct aet_camera {
    aet_fcurve eye_x;
    aet_fcurve eye_y;
    aet_fcurve eye_z;
    aet_fcurve pos_x;
    aet_fcurve pos_y;
    aet_fcurve pos_z;
    aet_fcurve dir_x;
    aet_fcurve dir_y;
    aet_fcurve dir_z;
    aet_fcurve rot_x;
    aet_fcurve rot_y;
    aet_fcurve rot_z;
    aet_fcurve zoom;

    aet_camera();
    ~aet_camera();
};

struct aet_scene {
    std::string name;
    float_t start_time;
    float_t end_time;
    float_t fps;
    aet_color color;
    uint32_t width;
    uint32_t height;
    aet_camera camera;
    bool camera_init;
    std::vector<aet_comp> comp;
    std::vector<aet_video> video;
    std::vector<aet_audio> audio;

    aet_scene();
    ~aet_scene();
};

struct aet_set {
    bool ready;
    bool modern;
    bool big_endian;
    bool is_x;

    std::vector<aet_scene> scenes;

    aet_set();
    ~aet_set();

    void pack_file(void** data, size_t* size);
    void unpack_file(const void* data, size_t size, bool modern);
};
