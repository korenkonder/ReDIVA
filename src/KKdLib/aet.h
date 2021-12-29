/*
    by korenkonder
    GitHub/GitLab: korenkonder

    Original: AetSet.bt Version: 5.0 by samyuu
*/

#pragma once

#include "default.h"
#include "kf.h"
#include "string.h"
#include "vec.h"

typedef enum aet_layer_quality {
    AET_LAYER_QUALITY_NONE      = 0,
    AET_LAYER_QUALITY_WIREFRAME = 1,
    AET_LAYER_QUALITY_DRAFT     = 2,
    AET_LAYER_QUALITY_BEST      = 3,
} aet_layer_quality;

typedef enum aet_item_type {
    AET_ITEM_TYPE_NONE        = 0,
    AET_ITEM_TYPE_VIDEO       = 1,
    AET_ITEM_TYPE_AUDIO       = 2,
    AET_ITEM_TYPE_COMPOSITION = 3,
} aet_item_type;

typedef enum aet_blend_mode {
    AET_BLEND_MODE_NONE                     = 0,
    AET_BLEND_MODE_COPY                     = 1,
    AET_BLEND_MODE_BEHIND                   = 2,
    AET_BLEND_MODE_NORMAL                   = 3,
    AET_BLEND_MODE_DISSOLVE                 = 4,
    AET_BLEND_MODE_ADD                      = 5,
    AET_BLEND_MODE_MULTIPLY                 = 6,
    AET_BLEND_MODE_SCREEN                   = 7,
    AET_BLEND_MODE_OVERLAY                  = 8,
    AET_BLEND_MODE_SOFT_LIGHT               = 9,
    AET_BLEND_MODE_HARD_LIGHT               = 10,
    AET_BLEND_MODE_DARKER                   = 11,
    AET_BLEND_MODE_LIGHTEN                  = 12,
    AET_BLEND_MODE_CLASSIC_DIFFERENCE       = 13,
    AET_BLEND_MODE_HUE                      = 14,
    AET_BLEND_MODE_SATURATION               = 15,
    AET_BLEND_MODE_COLOR                    = 16,
    AET_BLEND_MODE_LUMINOSITY               = 17,
    AET_BLEND_MODE_STENCIL_ALPHA            = 18,
    AET_BLEND_MODE_STENCIL_LUMA             = 19,
    AET_BLEND_MODE_SILHOUETTE_ALPHA         = 20,
    AET_BLEND_MODE_SILHOUETTE_LUMA          = 21,
    AET_BLEND_MODE_LUMINESCENT_PREMULTYPLY  = 22,
    AET_BLEND_MODE_ALPHA_ADD                = 23,
    AET_BLEND_MODE_CLASSIC_COLOR_DODGE      = 24,
    AET_BLEND_MODE_CLASSIC_COLOR_BURN       = 25,
    AET_BLEND_MODE_EXCLUSION                = 26,
    AET_BLEND_MODE_DIFFERENCE               = 27,
    AET_BLEND_MODE_COLOR_DODGE              = 28,
    AET_BLEND_MODE_COLOR_BURN               = 29,
    AET_BLEND_MODE_LINEAR_DODGE             = 30,
    AET_BLEND_MODE_LINEAR_BURN              = 31,
    AET_BLEND_MODE_LINEAR_LIGHT             = 32,
    AET_BLEND_MODE_VIVID_LIGHT              = 33,
    AET_BLEND_MODE_PIN_LIGHT                = 34,
    AET_BLEND_MODE_HARD_MIX                 = 35,
    AET_BLEND_MODE_LIGHTER_COLOR            = 36,
    AET_BLEND_MODE_DARKER_COLOR             = 37,
    AET_BLEND_MODE_SUBTRACT                 = 38,
    AET_BLEND_MODE_DIVIDE                   = 39,
} aet_blend_mode;

typedef struct aet_layer_flags {
    // Toggles layer visuals on or off
    uint16_t video_active : 1;
    // Toggles layer sounds on or off
    uint16_t audio_active : 1;
    uint16_t effects_active : 1;
    // Toggles motion blur on or off for the layer
    uint16_t motion_blur : 1;
    uint16_t frame_blending : 1;
    // Locks layer contents, preventing all changes
    uint16_t locked : 1;
    // Hides the current layer when the "Hide Shy Layers" composition switch is selected
    uint16_t shy : 1;
    uint16_t collapse : 1;
    uint16_t auto_orient_rotation : 1;
    // Identifies the layer as an adjustment layer
    uint16_t adjustment_layer : 1;
    uint16_t time_remapping : 1;
    // Identifies the layer as a 3D layer
    uint16_t layer_is_3d : 1;
    uint16_t look_at_camera : 1;
    uint16_t look_at_point_of_interest : 1;
    // Includes the current layer in previews and renders, ignoring layers without this switch set
    uint16_t solo : 1;
    uint16_t markers_locked : 1;
} aet_layer_flags;

typedef struct aet_transfer_flags {
    uint8_t preserve_alpha : 1;
    uint8_t randomize_dissolve : 1;
} aet_transfer_flags;

typedef enum aet_track_matte {
    AET_TRACK_MATTE_NONE      = 0,
    AET_TRACK_MATTE_ALPHA     = 1,
    AET_TRACK_MATTE_NOT_ALPHA = 2,
    AET_TRACK_MATTE_LUMA      = 3,
    AET_TRACK_MATTE_NOT_LUMA  = 4,
} aet_track_matte;

typedef struct aet_audio {
    uint32_t sound_id;
} aet_audio;

typedef struct aet_video_source {
    string name;
    int32_t name_offset;
    uint32_t id;
} aet_video_source;

typedef struct aet_layer_audio {
    vector_kft2 volume_left;
    vector_kft2 volume_right;
    vector_kft2 pan_left;
    vector_kft2 pan_right;
} aet_layer_audio;

typedef struct aet_layer_transfer_mode {
    aet_blend_mode blend_mode;
    aet_transfer_flags transfer_flags;
    aet_track_matte track_matte;
} aet_layer_transfer_mode;

typedef struct aet_layer_video_3d {
    vector_kft2 anchor_z;
    vector_kft2 position_z;
    vector_kft2 direction_x;
    vector_kft2 direction_y;
    vector_kft2 direction_z;
    vector_kft2 rotation_x;
    vector_kft2 rotation_y;
    vector_kft2 scale_z;
} aet_layer_video_3d;

typedef struct aet_layer_video {
    aet_layer_transfer_mode transfer_mode;
    uint8_t padding;
    vector_kft2 anchor_x;
    vector_kft2 anchor_y;
    vector_kft2 positionx;
    vector_kft2 position_y;
    vector_kft2 rotation;
    vector_kft2 scale_x;
    vector_kft2 scale_y;
    vector_kft2 opacit_y;
    aet_layer_video_3d* video_3d;
} aet_layer_video;

typedef struct aet_marker {
    float_t frame;
    string name;
    int32_t name_offset;
} aet_marker;

vector(aet_marker)
vector(aet_layer_video)
vector(aet_layer_audio)

typedef struct aet_layer {
    string name;
    int32_t name_offset;
    float_t start_frame;
    float_t end_frame;
    float_t offset_frame;
    float_t time_scale;
    aet_layer_flags flags;
    aet_layer_quality quality;
    aet_item_type type;
    int32_t video_item_offset;
    int32_t parent_layer_offset;
    vector_aet_marker marker;
    vector_aet_layer_video video;
    vector_aet_layer_audio audio;

    int32_t DataID;
} aet_layer;

typedef struct aet_composition {
    int32_t count;
    int32_t offset;
    aet_layer* elements;
} aet_composition;

vector(aet_video_source);

typedef struct aet_video {
    int32_t offset;
    uint32_t color;
    uint16_t width;
    uint16_t height;
    float_t frames;
    vector_aet_video_source video_source;
} aet_video;

typedef struct aet_camera {
    vector_kft2 eye_x;
    vector_kft2 eye_y;
    vector_kft2 eye_z;
    vector_kft2 position_x;
    vector_kft2 position_y;
    vector_kft2 position_z;
    vector_kft2 direction_x;
    vector_kft2 direction_y;
    vector_kft2 direction_z;
    vector_kft2 rotation_x;
    vector_kft2 rotation_y;
    vector_kft2 rotation_z;
    vector_kft2 zoom;
} aet_camera;

vector(aet_camera)
vector(aet_composition)
vector(aet_video)
vector(aet_audio)

typedef struct aet_scene {
    string name;
    int32_t name_offset;
    float_t start_frame;
    float_t end_frame;
    float_t frame_rate;
    uint32_t back_color;
    uint32_t width;
    uint32_t height;
    vector_aet_camera camera;
    vector_aet_composition composition;
    vector_aet_video video;
    vector_aet_audio audio;
} aet_scene;

typedef struct aet_header {
    aet_scene scene;
} aet_header;
