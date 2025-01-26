/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "nvenc_encoder.hpp"
#include <algorithm>
#include <vector>
#include <d3d11.h>

typedef NVENCSTATUS(NVENCAPI* PNVENCODEAPICREATEINSTANCEPROC)(NV_ENCODE_API_FUNCTION_LIST*);
PNVENCODEAPICREATEINSTANCEPROC nvEncodeAPICreateInstance;

HMODULE nvenc_dll;
int32_t nvenc_dll_init_count;

static void free_nvenc_dll();
static bool init_nvenc_dll();
static void print_nvenc_status(int32_t line, NVENCSTATUS status);

nvenc_encoder::nvenc_encoder(int32_t width, int32_t height,
    void* d3d_device, void* d3d_device_context, nvenc_encoder_format format, bool lossless,
    int32_t num_bframes, int32_t lookahead_depth, int32_t extra_output_delay) : init_params(),
    nvenc_config(), nvenc_api(), encoder(), encode_guid(), preset_guid(), width(), height(), d3d_device(),
    d3d_device_context(), registered_resources(), mapped_resources(), output_buffers(), buffer_format(),
    frame(), encoded_frame(), extra_output_delay(), num_buffers(), output_delay(), input_textures() {
    if (!init_nvenc_dll())
        return;

    nvenc_api = force_malloc<NV_ENCODE_API_FUNCTION_LIST>();
    nvenc_api->version = NV_ENCODE_API_FUNCTION_LIST_VER;
    print_nvenc_status(__LINE__, nvEncodeAPICreateInstance(nvenc_api));

    NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS session_ex_params = {};
    session_ex_params.version = NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS_VER;
    session_ex_params.apiVersion = NVENCAPI_VERSION;
    session_ex_params.device = d3d_device;
    session_ex_params.deviceType = NV_ENC_DEVICE_TYPE_DIRECTX;
    print_nvenc_status(__LINE__, nvenc_api->nvEncOpenEncodeSessionEx(&session_ex_params, &encoder));

    encode_guid = NV_ENC_CODEC_HEVC_GUID;
    preset_guid = NV_ENC_PRESET_P7_GUID;

    const int32_t num_max_bframes = get_capability_value(NV_ENC_CAPS_NUM_MAX_BFRAMES);
    const bool support_yuv444_encode = !!get_capability_value(NV_ENC_CAPS_SUPPORT_YUV444_ENCODE);
    const bool support_lossless_encode = !!get_capability_value(NV_ENC_CAPS_SUPPORT_LOSSLESS_ENCODE);
    const bool support_lookahead = !!get_capability_value(NV_ENC_CAPS_SUPPORT_LOOKAHEAD);
    const bool support_temporal_aq = !!get_capability_value(NV_ENC_CAPS_SUPPORT_TEMPORAL_AQ);
    const bool support_10bit_encode = !!get_capability_value(NV_ENC_CAPS_SUPPORT_10BIT_ENCODE);
    const bool support_weighted_prediction = !!get_capability_value(NV_ENC_CAPS_SUPPORT_WEIGHTED_PREDICTION);
    const NV_ENC_BFRAME_REF_MODE support_bframe_ref_mode
        = (NV_ENC_BFRAME_REF_MODE)get_capability_value(NV_ENC_CAPS_SUPPORT_BFRAME_REF_MODE);

    bool is_10_bit = false;
    bool is_444 = false;
    bool is_rgb = false;
    switch (format) {
    case NVENC_ENCODER_YUV420:
        is_10_bit = false;
        break;
    case NVENC_ENCODER_YUV420_10BIT:
        is_10_bit = true;
        break;
    case NVENC_ENCODER_YUV444:
        is_10_bit = false;
        is_444 = true;
        break;
    case NVENC_ENCODER_YUV444_10BIT:
        is_10_bit = true;
        is_444 = true;
        break;
    case NVENC_ENCODER_RGB:
        is_10_bit = false;
        is_444 = true;
        is_rgb = true;
        break;
    case NVENC_ENCODER_RGB_10BIT:
        is_10_bit = true;
        is_444 = true;
        is_rgb = true;
        break;
    }

    lossless &= support_lossless_encode;
    is_10_bit &= support_10bit_encode;
    is_444 &= support_yuv444_encode;

    buffer_format = is_rgb ? NV_ENC_BUFFER_FORMAT_AYUV : NV_ENC_BUFFER_FORMAT_ARGB;

    num_bframes = clamp_def(num_bframes, 0, num_max_bframes);
    extra_output_delay = max_def(extra_output_delay, 2);
    lookahead_depth = clamp_def(lookahead_depth, 0, 31);

    init_params = {};
    init_params.version = NV_ENC_INITIALIZE_PARAMS_VER;

    init_params.encodeGUID = encode_guid;
    init_params.presetGUID = preset_guid;
    init_params.tuningInfo = lossless ? NV_ENC_TUNING_INFO_LOSSLESS : NV_ENC_TUNING_INFO_HIGH_QUALITY;
    init_params.encodeWidth = width;
    init_params.encodeHeight = height;
    init_params.frameRateNum = 60;
    init_params.frameRateDen = 1;
    init_params.enablePTD = 1;
    if (support_weighted_prediction && !num_bframes)
        init_params.enableWeightedPrediction = 1;
    init_params.encodeConfig = &nvenc_config;

    NV_ENC_PRESET_CONFIG preset_config = {};
    preset_config.version = NV_ENC_PRESET_CONFIG_VER;
    preset_config.presetCfg.version = NV_ENC_CONFIG_VER;
    print_nvenc_status(__LINE__, nvenc_api->nvEncGetEncodePresetConfigEx(encoder,
        init_params.encodeGUID, init_params.presetGUID, init_params.tuningInfo, &preset_config));

    nvenc_config = preset_config.presetCfg;

    if (is_444)
        nvenc_config.profileGUID = NV_ENC_HEVC_PROFILE_FREXT_GUID;
    else if (is_10_bit)
        nvenc_config.profileGUID = NV_ENC_HEVC_PROFILE_MAIN10_GUID;
    else
        nvenc_config.profileGUID = NV_ENC_HEVC_PROFILE_MAIN_GUID;

    nvenc_config.frameIntervalP = 1 + num_bframes;

    NV_ENC_CONFIG_HEVC& hevc_config = nvenc_config.encodeCodecConfig.hevcConfig;
    hevc_config.chromaFormatIDC = is_444 ? 3 : 1;
    if (!lossless)
        hevc_config.useBFramesAsRef = (NV_ENC_BFRAME_REF_MODE)(support_bframe_ref_mode & NV_ENC_BFRAME_REF_MODE_MIDDLE);
    hevc_config.outputBitDepth = is_10_bit ? NV_ENC_BIT_DEPTH_10 : NV_ENC_BIT_DEPTH_8;
    hevc_config.inputBitDepth = is_10_bit ? NV_ENC_BIT_DEPTH_10 : NV_ENC_BIT_DEPTH_8;
    hevc_config.hevcVUIParameters.videoSignalTypePresentFlag = 1;
    hevc_config.hevcVUIParameters.videoFormat = NV_ENC_VUI_VIDEO_FORMAT_UNSPECIFIED;
    hevc_config.hevcVUIParameters.videoFullRangeFlag = 1;
    hevc_config.hevcVUIParameters.colourDescriptionPresentFlag = 1;
    hevc_config.hevcVUIParameters.colourPrimaries = NV_ENC_VUI_COLOR_PRIMARIES_BT709;
    hevc_config.hevcVUIParameters.transferCharacteristics = NV_ENC_VUI_TRANSFER_CHARACTERISTIC_BT709;
    hevc_config.hevcVUIParameters.colourMatrix = is_rgb
        ? NV_ENC_VUI_MATRIX_COEFFS_RGB : NV_ENC_VUI_MATRIX_COEFFS_BT709;

    nvenc_config.rcParams.rateControlMode = NV_ENC_PARAMS_RC_CONSTQP;
    nvenc_config.rcParams.constQP.qpInterP = lossless ? 0 : 20;
    nvenc_config.rcParams.constQP.qpInterB = lossless ? 0 : 20;
    nvenc_config.rcParams.constQP.qpIntra = lossless ? 0 : 20;
    nvenc_config.rcParams.multiPass = NV_ENC_TWO_PASS_FULL_RESOLUTION;

    if (!lossless) {
        nvenc_config.rcParams.enableAQ = 1;

        if (support_lookahead && lookahead_depth) {
            nvenc_config.rcParams.enableLookahead = 1;
            nvenc_config.rcParams.lookaheadDepth = lookahead_depth;
        }

        if (support_temporal_aq)
            nvenc_config.rcParams.enableTemporalAQ = 1;
    }

    nvenc_config.rcParams.enableNonRefP = 1;

    print_nvenc_status(__LINE__, nvenc_api->nvEncInitializeEncoder(encoder, &init_params));

    this->width = width;
    this->height = height;

    this->d3d_device = d3d_device;
    this->d3d_device_context = d3d_device_context;

    this->extra_output_delay = extra_output_delay;
    num_buffers = nvenc_config.frameIntervalP + nvenc_config.rcParams.lookaheadDepth + extra_output_delay;
    output_delay = num_buffers - 1;

    input_textures = force_malloc<void*>(num_buffers);
    registered_resources = force_malloc<NV_ENC_REGISTERED_PTR>(num_buffers);
    mapped_resources = force_malloc<NV_ENC_INPUT_PTR>(num_buffers);
    output_buffers = force_malloc<NV_ENC_OUTPUT_PTR>(num_buffers);

    for (int32_t i = 0; i < num_buffers; i++) {
        D3D11_TEXTURE2D_DESC tex_desc = {};
        tex_desc.Width = width;
        tex_desc.Height = height;
        tex_desc.MipLevels = 1;
        tex_desc.ArraySize = 1;
        tex_desc.Format = is_rgb ? DXGI_FORMAT_AYUV : DXGI_FORMAT_R8G8B8A8_UNORM;
        tex_desc.SampleDesc.Count = 1;
        tex_desc.SampleDesc.Quality = 0;
        tex_desc.Usage = D3D11_USAGE_DEFAULT;
        tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET;
        tex_desc.CPUAccessFlags = 0;
        tex_desc.MiscFlags = 0;

        ((ID3D11Device*)d3d_device)->CreateTexture2D(&tex_desc, NULL, &((ID3D11Texture2D**)input_textures)[i]);
    }

    for (int32_t i = 0; i < num_buffers; i++) {
        ID3D11Texture2D*& texture = ((ID3D11Texture2D**)input_textures)[i];

        D3D11_TEXTURE2D_DESC desc;
        texture->GetDesc(&desc);

        NV_ENC_REGISTER_RESOURCE res_params = {};
        res_params.version = NV_ENC_REGISTER_RESOURCE_VER;
        res_params.resourceToRegister = texture;
        res_params.resourceType = NV_ENC_INPUT_RESOURCE_TYPE_DIRECTX;
        res_params.width = desc.Width;
        res_params.height = desc.Height;
        res_params.pitch = desc.Width;
        res_params.bufferFormat = buffer_format;
        res_params.bufferUsage = NV_ENC_INPUT_IMAGE;

        print_nvenc_status(__LINE__, nvenc_api->nvEncRegisterResource(encoder, &res_params));

        registered_resources[i] = res_params.registeredResource;

        NV_ENC_MAP_INPUT_RESOURCE map_params = {};
        map_params.version = NV_ENC_MAP_INPUT_RESOURCE_VER;
        map_params.registeredResource = res_params.registeredResource;
        print_nvenc_status(__LINE__, nvenc_api->nvEncMapInputResource(encoder, &map_params));

        mapped_resources[i] = map_params.mappedResource;
    }

    for (int32_t i = 0; i < num_buffers; i++) {
        NV_ENC_CREATE_BITSTREAM_BUFFER bitstream_buffer_params = {};
        bitstream_buffer_params.version = NV_ENC_CREATE_BITSTREAM_BUFFER_VER;
        print_nvenc_status(__LINE__, nvenc_api->nvEncCreateBitstreamBuffer(encoder, &bitstream_buffer_params));

        output_buffers[i] = bitstream_buffer_params.bitstreamBuffer;
    }
}

nvenc_encoder::~nvenc_encoder() {
    for (int32_t i = 0; i < num_buffers; i++)
        print_nvenc_status(__LINE__, nvenc_api->nvEncDestroyBitstreamBuffer(encoder, output_buffers[i]));

    free_def(output_buffers);

    for (int32_t i = 0; i < num_buffers; i++) {
        unmap_resource(i);
        print_nvenc_status(__LINE__, nvenc_api->nvEncUnregisterResource(encoder, registered_resources[i]));
    }

    free_def(mapped_resources);
    free_def(registered_resources);

    for (int32_t i = 0; i < num_buffers; i++)
        ((ID3D11Texture2D**)input_textures)[i]->Release();

    free_def(input_textures);

    print_nvenc_status(__LINE__, nvenc_api->nvEncDestroyEncoder(encoder));
    free_def(nvenc_api);

    free_nvenc_dll();
}

void nvenc_encoder::end_encode(stream* s) {
    NV_ENC_PIC_PARAMS pic_params = {};
    pic_params.version = NV_ENC_PIC_PARAMS_VER;
    pic_params.encodePicFlags = NV_ENC_PIC_FLAG_EOS;
    print_nvenc_status(__LINE__, nvenc_api->nvEncEncodePicture(encoder, &pic_params));

    write_packet(s, false);
}

void nvenc_encoder::encode_frame(stream* s, void* texture) {
    const int32_t index = frame % num_buffers;

    ((ID3D11DeviceContext*)d3d_device_context)->CopyResource(
        (ID3D11Texture2D*)input_textures[index],
        (ID3D11Texture2D*)texture);

    map_resource(index);

    NV_ENC_PIC_PARAMS pic_params = {};
    pic_params.version = NV_ENC_PIC_PARAMS_VER;
    pic_params.inputWidth = width;
    pic_params.inputHeight = height;
    pic_params.inputPitch = pic_params.inputWidth;
    pic_params.frameIdx = frame;
    pic_params.inputBuffer = mapped_resources[index];
    pic_params.outputBitstream = output_buffers[index];
    pic_params.bufferFmt = buffer_format;
    pic_params.pictureStruct = NV_ENC_PIC_STRUCT_FRAME;

    NVENCSTATUS encode_status = nvenc_api->nvEncEncodePicture(encoder, &pic_params);

    if (encode_status == NV_ENC_SUCCESS || encode_status == NV_ENC_ERR_NEED_MORE_INPUT) {
        frame++;
        write_packet(s, true);
    }
    else
        print_nvenc_status(__LINE__, encode_status);
}

int32_t nvenc_encoder::get_capability_value(NV_ENC_CAPS caps_to_query) {
    if (!encoder)
        return 0;

    NV_ENC_CAPS_PARAM caps_param = {};
    caps_param.version = NV_ENC_CAPS_PARAM_VER;
    caps_param.capsToQuery = caps_to_query;

    int32_t val = 0;
    nvenc_api->nvEncGetEncodeCaps(encoder, encode_guid, &caps_param, &val);
    return val;
}

void nvenc_encoder::map_resource(int32_t index) {
    if (mapped_resources[index])
        return;

    NV_ENC_MAP_INPUT_RESOURCE map_params = {};
    map_params.version = NV_ENC_MAP_INPUT_RESOURCE_VER;
    map_params.registeredResource = registered_resources[index];
    print_nvenc_status(__LINE__, nvenc_api->nvEncMapInputResource(encoder, &map_params));

    mapped_resources[index] = map_params.mappedResource;
}

void nvenc_encoder::unmap_resource(int32_t index) {
    if (!mapped_resources[index])
        return;

    print_nvenc_status(__LINE__, nvenc_api->nvEncUnmapInputResource(encoder, mapped_resources[index]));

    mapped_resources[index] = 0;
}

void nvenc_encoder::write_packet(stream* s, bool output_delay) {
    const int32_t end_encoded_frame = output_delay ? frame - this->output_delay : frame;
    for (; encoded_frame < end_encoded_frame; encoded_frame++) {
        const int32_t index = encoded_frame % num_buffers;

        NV_ENC_LOCK_BITSTREAM lock_params{};
        lock_params.version = NV_ENC_LOCK_BITSTREAM_VER;
        lock_params.outputBitstream = output_buffers[index];
        lock_params.doNotWait = false;

        print_nvenc_status(__LINE__, nvenc_api->nvEncLockBitstream(encoder, &lock_params));
        s->write(lock_params.bitstreamBufferPtr, lock_params.bitstreamSizeInBytes);
        print_nvenc_status(__LINE__, nvenc_api->nvEncUnlockBitstream(encoder, lock_params.outputBitstream));
        s->flush();

        unmap_resource(index);
    }
}

static void free_nvenc_dll() {
    if (nvenc_dll_init_count > 1) {
        nvenc_dll_init_count--;
        return;
    }

    nvenc_dll_init_count = 0;

    nvEncodeAPICreateInstance = 0;
    if (nvenc_dll)
        FreeLibrary(nvenc_dll);
    nvenc_dll = 0;
}

static bool init_nvenc_dll() {
    if (!nvenc_dll)
        nvenc_dll = LoadLibraryA("nvEncodeAPI64.dll");

    if (!nvenc_dll)
        return false;

    nvenc_dll_init_count++;

    if (!nvEncodeAPICreateInstance)
        nvEncodeAPICreateInstance = (PNVENCODEAPICREATEINSTANCEPROC)GetProcAddress(
            nvenc_dll, "NvEncodeAPICreateInstance");

    if (!nvEncodeAPICreateInstance)
        return false;

    return true;
}

static void print_nvenc_status(int32_t line, NVENCSTATUS status) {
    switch (status) {
#define ERR(s) \
case s: \
printf_debug("% 3d: %s\n", line, #s); \
break
    case NV_ENC_SUCCESS:
        break;
        ERR(NV_ENC_ERR_NO_ENCODE_DEVICE);
        ERR(NV_ENC_ERR_UNSUPPORTED_DEVICE);
        ERR(NV_ENC_ERR_INVALID_ENCODERDEVICE);
        ERR(NV_ENC_ERR_INVALID_DEVICE);
        ERR(NV_ENC_ERR_DEVICE_NOT_EXIST);
        ERR(NV_ENC_ERR_INVALID_PTR);
        ERR(NV_ENC_ERR_INVALID_EVENT);
        ERR(NV_ENC_ERR_INVALID_PARAM);
        ERR(NV_ENC_ERR_INVALID_CALL);
        ERR(NV_ENC_ERR_OUT_OF_MEMORY);
        ERR(NV_ENC_ERR_ENCODER_NOT_INITIALIZED);
        ERR(NV_ENC_ERR_UNSUPPORTED_PARAM);
        ERR(NV_ENC_ERR_LOCK_BUSY);
        ERR(NV_ENC_ERR_NOT_ENOUGH_BUFFER);
        ERR(NV_ENC_ERR_INVALID_VERSION);
        ERR(NV_ENC_ERR_MAP_FAILED);
        ERR(NV_ENC_ERR_NEED_MORE_INPUT);
        ERR(NV_ENC_ERR_ENCODER_BUSY);
        ERR(NV_ENC_ERR_EVENT_NOT_REGISTERD);
        ERR(NV_ENC_ERR_GENERIC);
        ERR(NV_ENC_ERR_INCOMPATIBLE_CLIENT_KEY);
        ERR(NV_ENC_ERR_UNIMPLEMENTED);
        ERR(NV_ENC_ERR_RESOURCE_REGISTER_FAILED);
        ERR(NV_ENC_ERR_RESOURCE_NOT_REGISTERED);
        ERR(NV_ENC_ERR_RESOURCE_NOT_MAPPED);
        ERR(NV_ENC_ERR_NEED_MORE_OUTPUT);
#undef ERR
    }
}
