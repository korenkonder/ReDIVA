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

    nvenc_encoder(int32_t width, int32_t height, void* d3d_device);
    ~nvenc_encoder();

    void write_frame(ID3D11Texture2D* texture, stream* s);
};
