/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../KKdLib/io/stream.hpp"
#include <string>
#include <nvenc/nvEncodeAPI.h>
#include <d3d11.h>

class nvenc_encoder {
public:
    NV_ENC_INITIALIZE_PARAMS init_params;
    NV_ENC_CONFIG nvenc_config;

    NV_ENCODE_API_FUNCTION_LIST* nvenc_api;
    HMODULE nvenc_dll;

    void* encoder;
    GUID encoder_guid;
    GUID preset_guid;

    NV_ENC_OUTPUT_PTR output_buffer;

    uint64_t frame_time_stamp;

    int32_t width;
    int32_t height;

    NV_ENC_REGISTERED_PTR* registered_resources;
    NV_ENC_INPUT_PTR* mapped_resources;
    int32_t num_resources;

    nvenc_encoder(int32_t width, int32_t height, void* d3d_device,
        void** textures, int32_t num_textures);
    ~nvenc_encoder();

    void map_resource(int32_t tex_index);
    void unmap_resource(int32_t tex_index);
    void write_frame(int32_t tex_index, stream* s);
};
