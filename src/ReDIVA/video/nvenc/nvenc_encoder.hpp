/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../../KKdLib/default.hpp"
#include "../../../KKdLib/prj/stack_allocator.hpp"
#include "../video_packet.hpp"
#include <nvenc/nvEncodeAPI.h>

enum nvenc_encoder_format {
    NVENC_ENCODER_YUV420 = 0,
    NVENC_ENCODER_YUV420_10BIT,
    NVENC_ENCODER_YUV444,
    NVENC_ENCODER_YUV444_10BIT,
    NVENC_ENCODER_RGB,
    NVENC_ENCODER_RGB_10BIT,
    NVENC_ENCODER_MAX,
};

class nvenc_encoder {
private:
    NV_ENC_INITIALIZE_PARAMS init_params;
    NV_ENC_CONFIG nvenc_config;

    NV_ENCODE_API_FUNCTION_LIST* nvenc_api;

    void* encoder;
    GUID encode_guid;
    GUID preset_guid;

    int32_t width;
    int32_t height;

    void* d3d_device;
    void* d3d_device_context;

    void** input_textures;
    NV_ENC_REGISTERED_PTR* registered_resources;
    NV_ENC_INPUT_PTR* mapped_resources;
    NV_ENC_OUTPUT_PTR* output_buffers;
    NV_ENC_BUFFER_FORMAT buffer_format;

    uint32_t frame;
    int32_t encoded_frame;

    int32_t extra_output_delay;
    int32_t num_buffers;
    int32_t output_delay;

public:
    nvenc_encoder(int32_t width, int32_t height, void* d3d_device, void* d3d_device_context,
        nvenc_encoder_format format = NVENC_ENCODER_YUV420, bool lossless = false,
        int32_t num_bframes = 3, int32_t lookahead_depth = 15, int32_t extra_output_delay = 3);
    ~nvenc_encoder();

    void end_encode(video_packet_handler* packet_handler);
    void encode_frame(video_packet_handler* packet_handler, void* texture);

private:
    int32_t get_capability_value(NV_ENC_CAPS caps_to_query);
    void map_resource(int32_t index);
    void unmap_resource(int32_t index);
    void write_packet(video_packet_handler* packet_handler, bool output_delay);
};
