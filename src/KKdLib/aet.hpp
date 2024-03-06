/*
    by korenkonder
    GitHub/GitLab: korenkonder

    Original: AetSet.bt Version: 5.0 by samyuu
*/

#pragma once

#include "default.hpp"
#include "prj/shared_ptr.hpp"
#include "prj/stack_allocator.hpp"

enum aet_blend_mode : uint8_t {
    AET_BLEND_MODE_NONE = 0,
    AET_BLEND_MODE_COPY,
    AET_BLEND_MODE_BEHIND,
    AET_BLEND_MODE_NORMAL,
    AET_BLEND_MODE_DISSOLVE,
    AET_BLEND_MODE_ADD,
    AET_BLEND_MODE_MULTIPLY,
    AET_BLEND_MODE_SCREEN,
    AET_BLEND_MODE_OVERLAY,
    AET_BLEND_MODE_SOFT_LIGHT,
    AET_BLEND_MODE_HARD_LIGHT,
    AET_BLEND_MODE_DARKEN,
    AET_BLEND_MODE_LIGHTEN,
    AET_BLEND_MODE_CLASSIC_DIFFERENCE,
    AET_BLEND_MODE_HUE,
    AET_BLEND_MODE_SATURATION,
    AET_BLEND_MODE_COLOR,
    AET_BLEND_MODE_LUMINOSITY,
    AET_BLEND_MODE_STENCIL_ALPHA,
    AET_BLEND_MODE_STENCIL_LUMA,
    AET_BLEND_MODE_SILHOUETTE_ALPHA,
    AET_BLEND_MODE_SILHOUETTE_LUMA,
    AET_BLEND_MODE_LUMINESCENT_PREMUL,
    AET_BLEND_MODE_ALPHA_ADD,
    AET_BLEND_MODE_CLASSIC_COLOR_DODGE,
    AET_BLEND_MODE_CLASSIC_COLOR_BURN,
    AET_BLEND_MODE_EXCLUSION,
    AET_BLEND_MODE_DIFFERENCE,
    AET_BLEND_MODE_COLOR_DODGE,
    AET_BLEND_MODE_COLOR_BURN,
    AET_BLEND_MODE_LINEAR_DODGE,
    AET_BLEND_MODE_LINEAR_BURN,
    AET_BLEND_MODE_LINEAR_LIGHT,
    AET_BLEND_MODE_VIVID_LIGHT,
    AET_BLEND_MODE_PIN_LIGHT,
    AET_BLEND_MODE_HARD_MIX,
    AET_BLEND_MODE_LIGHTER_COLOR,
    AET_BLEND_MODE_DARKER_COLOR,
    AET_BLEND_MODE_SUBTRACT,
    AET_BLEND_MODE_DIVIDE,
};

enum aet_layer_quality : uint8_t {
    AET_LAYER_QUALITY_NONE = 0,
    AET_LAYER_QUALITY_WIREFRAME,
    AET_LAYER_QUALITY_DRAFT,
    AET_LAYER_QUALITY_BEST,
};

enum aet_item_type : uint8_t {
    AET_ITEM_TYPE_NONE = 0,
    AET_ITEM_TYPE_VIDEO,
    AET_ITEM_TYPE_AUDIO,
    AET_ITEM_TYPE_COMPOSITION,
};

struct aet_layer_flag {
    uint16_t video_active : 1;
    uint16_t audio_active : 1;
    uint16_t effects_active : 1;
    uint16_t motion_blur : 1;
    uint16_t frame_blending : 1;
    uint16_t locked : 1;
    uint16_t shy : 1;
    uint16_t collapse : 1;
    uint16_t auto_orient_rotation : 1;
    uint16_t adjustment_layer : 1;
    uint16_t time_remapping : 1;
    uint16_t layer_is_3d : 1;
    uint16_t look_at_camera : 1;
    uint16_t look_at_point_of_interest : 1;
    uint16_t solo : 1;
    uint16_t markers_locked : 1;
    // Should be there but they don't fit in 16 bits
    //uint16_t null_layer : 1;
    //uint16_t hide_locked_masks : 1;
    //uint16_t guide_layer : 1;
    //uint16_t advanced_frame_blending : 1;
    //uint16_t sub_layers_render_separately : 1;
    //uint16_t environment_layer : 1;
};

struct aet_fcurve {
    uint32_t keys_count;
    const float_t* keys;

    aet_fcurve();

    float_t interpolate(float_t frame) const;
};

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
};

struct aet_transfer_mode {
    aet_blend_mode mode;
    uint8_t flag;
    uint8_t matte;

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
    const aet_layer_video_3d* _3d;

    aet_layer_video();
};

struct aet_layer_audio {
    aet_fcurve volume_l;
    aet_fcurve volume_r;
    aet_fcurve pan_l;
    aet_fcurve pan_r;

    aet_layer_audio();
};

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
};

struct aet_video_src {
    const char* sprite_name;
    int32_t sprite_index;

    aet_video_src();
};

struct aet_video {
    uint8_t color[3];
    uint16_t width;
    uint16_t height;
    float_t fpf;
    uint32_t sources_count;
    const aet_video_src* sources;

    aet_video();
};

struct aet_audio {
    uint32_t sound_index;

    aet_audio();
};

struct aet_comp;

union aet_item {
    const void* none;
    const aet_video* video;
    const aet_audio* audio;
    const aet_comp* comp;

    inline aet_item() : none() {

    }

    inline aet_item(const aet_video* video) {
        this->video = video;
    }

    inline aet_item(const aet_audio* audio) {
        this->audio = audio;
    }

    inline aet_item(const aet_comp* comp) {
        this->comp = comp;
    }
};

struct aet_marker {
    float_t time;
    const char* name;

    aet_marker();
};

#pragma warning(push)
#pragma warning(disable: 26495)
struct aet_layer {
    const char* name;
    float_t start_time;
    float_t end_time;
    float_t offset_time;
    float_t time_scale;
    aet_layer_flag flags;
    aet_layer_quality quality;
    aet_item_type item_type;
    aet_item item;
    const aet_layer* parent;
    uint32_t markers_count;
    const aet_marker* markers;
    const aet_layer_video* video;
    const aet_layer_audio* audio;

    aet_layer();
};
#pragma warning(pop)

struct aet_comp {
    uint32_t layers_count;
    const aet_layer* layers;

    aet_comp();
};

struct aet_scene {
    const char* name;
    float_t start_time;
    float_t end_time;
    float_t fps;
    uint8_t color[3];
    uint32_t width;
    uint32_t height;
    const aet_camera* camera;
    uint32_t comp_count;
    const aet_comp* comp;
    uint32_t video_count;
    const aet_video* video;
    uint32_t audio_count;
    const aet_audio* audio;

    aet_scene();

    static const aet_comp* get_root_comp(const aet_scene* scene);
};

struct aet_set {
    bool ready;
    bool modern;
    bool big_endian;
    bool is_x;

    uint32_t scenes_count;
    const aet_scene** scenes;

    aet_set();

    void move_data(aet_set* set_src, prj::shared_ptr<prj::stack_allocator> alloc);
    void pack_file(void** data, size_t* size);
    void unpack_file(prj::shared_ptr<prj::stack_allocator> alloc, const void* data, size_t size, bool modern);
};
