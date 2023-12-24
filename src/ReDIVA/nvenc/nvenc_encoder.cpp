/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "nvenc_encoder.hpp"
#include <algorithm>
#include <vector>
#include <cstdint>

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

nvenc_encoder::nvenc_encoder(int32_t width, int32_t height, void* d3d_device) : init_params(),
nvenc_config(), nvenc_api(), encoder(), encoder_guid(), preset_guid(), output_buffer(), frame_time_stamp() {
    nvenc_dll = LoadLibraryA("nvEncodeAPI64.dll");
    if (!nvenc_dll)
        return;

    typedef NVENCSTATUS(NVENCAPI* PNVENCODEAPICREATEINSTANCEPROC)(NV_ENCODE_API_FUNCTION_LIST*);
    PNVENCODEAPICREATEINSTANCEPROC nvEncodeAPICreateInstance
        = (PNVENCODEAPICREATEINSTANCEPROC)GetProcAddress(nvenc_dll, "NvEncodeAPICreateInstance");

    nvenc_api = force_malloc<NV_ENCODE_API_FUNCTION_LIST>();
    nvenc_api->version = NV_ENCODE_API_FUNCTION_LIST_VER;
    print_nvenc_status(__LINE__, nvEncodeAPICreateInstance(nvenc_api));

    NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS params = {};
    params.version = NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS_VER;
    params.apiVersion = NVENCAPI_VERSION;
    params.device = d3d_device;
    params.deviceType = NV_ENC_DEVICE_TYPE_DIRECTX;
    print_nvenc_status(__LINE__, nvenc_api->nvEncOpenEncodeSessionEx(&params, &encoder));

    encoder_guid = NV_ENC_CODEC_HEVC_GUID;
    preset_guid = NV_ENC_PRESET_P7_GUID;

    init_params = {};
    init_params.version = NV_ENC_INITIALIZE_PARAMS_VER;

    init_params.encodeGUID = encoder_guid;
    init_params.presetGUID = preset_guid;
    init_params.tuningInfo = NV_ENC_TUNING_INFO_HIGH_QUALITY;
    init_params.encodeWidth = width;
    init_params.encodeHeight = height;
    init_params.frameRateNum = 60;
    init_params.frameRateDen = 1;
    init_params.enablePTD = 1;
    init_params.encodeConfig = &nvenc_config;

    NV_ENC_PRESET_CONFIG preset_config = {};
    preset_config.version = NV_ENC_PRESET_CONFIG_VER;
    preset_config.presetCfg.version = NV_ENC_CONFIG_VER;
    print_nvenc_status(__LINE__, nvenc_api->nvEncGetEncodePresetConfigEx(encoder,
        init_params.encodeGUID, init_params.presetGUID, init_params.tuningInfo, &preset_config));

    nvenc_config = preset_config.presetCfg;

    nvenc_config.profileGUID = NV_ENC_HEVC_PROFILE_MAIN10_GUID;
    nvenc_config.gopLength = 10;
    nvenc_config.frameIntervalP = 1;

    NV_ENC_CONFIG_HEVC& hevc_config = nvenc_config.encodeCodecConfig.hevcConfig;
    hevc_config.idrPeriod = 10;
    hevc_config.pixelBitDepthMinus8 = 2;
    hevc_config.hevcVUIParameters.videoSignalTypePresentFlag = 1;
    hevc_config.hevcVUIParameters.videoFormat = NV_ENC_VUI_VIDEO_FORMAT_UNSPECIFIED;
    hevc_config.hevcVUIParameters.videoFullRangeFlag = 1;
    hevc_config.hevcVUIParameters.colourDescriptionPresentFlag = 1;
    hevc_config.hevcVUIParameters.colourPrimaries = NV_ENC_VUI_COLOR_PRIMARIES_BT709;
    hevc_config.hevcVUIParameters.transferCharacteristics = NV_ENC_VUI_TRANSFER_CHARACTERISTIC_BT709;
    hevc_config.hevcVUIParameters.colourMatrix = NV_ENC_VUI_MATRIX_COEFFS_BT709;

    nvenc_config.rcParams.rateControlMode = NV_ENC_PARAMS_RC_CONSTQP;
    nvenc_config.rcParams.constQP.qpIntra = 10;
    nvenc_config.rcParams.constQP.qpInterB = 10;
    nvenc_config.rcParams.constQP.qpIntra = 10;
    nvenc_config.rcParams.multiPass = NV_ENC_TWO_PASS_FULL_RESOLUTION;

    print_nvenc_status(__LINE__, nvenc_api->nvEncInitializeEncoder(encoder, &init_params));

    NV_ENC_CREATE_BITSTREAM_BUFFER bitstream_buffer_params = {};
    bitstream_buffer_params.version = NV_ENC_CREATE_BITSTREAM_BUFFER_VER;
    bitstream_buffer_params.size = init_params.encodeHeight * init_params.encodeWidth * 3 / 2;
    bitstream_buffer_params.memoryHeap = NV_ENC_MEMORY_HEAP_SYSMEM_CACHED;

    print_nvenc_status(__LINE__, nvenc_api->nvEncCreateBitstreamBuffer(encoder, &bitstream_buffer_params));

    output_buffer = bitstream_buffer_params.bitstreamBuffer;
}

nvenc_encoder::~nvenc_encoder() {
    print_nvenc_status(__LINE__, nvenc_api->nvEncDestroyBitstreamBuffer(encoder, output_buffer));

    print_nvenc_status(__LINE__, nvenc_api->nvEncDestroyEncoder(encoder));
    FreeLibrary(nvenc_dll);
    free_def(nvenc_api);
}

void nvenc_encoder::write_frame(ID3D11Texture2D* texture, stream* s) {
    D3D11_TEXTURE2D_DESC desc;
    texture->GetDesc(&desc);

    const NV_ENC_BUFFER_FORMAT buffer_format = NV_ENC_BUFFER_FORMAT_ABGR;

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

    NV_ENC_MAP_INPUT_RESOURCE map_params = {};
    map_params.version = NV_ENC_MAP_INPUT_RESOURCE_VER;
    map_params.registeredResource = res_params.registeredResource;
    map_params.inputResource = (void*)texture;
    print_nvenc_status(__LINE__, nvenc_api->nvEncMapInputResource(encoder, &map_params));

    NV_ENC_PIC_PARAMS params{};
    params.version = NV_ENC_PIC_PARAMS_VER;
    params.inputWidth = desc.Width;
    params.inputHeight = desc.Height;
    params.inputPitch = params.inputWidth;
    params.inputBuffer = map_params.mappedResource;
    params.outputBitstream = output_buffer;
    params.bufferFmt = buffer_format;
    params.pictureStruct = NV_ENC_PIC_STRUCT_FRAME;
    print_nvenc_status(__LINE__, nvenc_api->nvEncEncodePicture(encoder, &params));

    print_nvenc_status(__LINE__, nvenc_api->nvEncUnmapInputResource(encoder, map_params.mappedResource));
    print_nvenc_status(__LINE__, nvenc_api->nvEncUnregisterResource(encoder, res_params.registeredResource));

    NV_ENC_LOCK_BITSTREAM lock_params{};
    lock_params.version = NV_ENC_LOCK_BITSTREAM_VER;
    lock_params.outputBitstream = output_buffer;

    print_nvenc_status(__LINE__, nvenc_api->nvEncLockBitstream(encoder, &lock_params));
    s->write(lock_params.bitstreamBufferPtr, lock_params.bitstreamSizeInBytes);
    print_nvenc_status(__LINE__, nvenc_api->nvEncUnlockBitstream(encoder, &output_buffer));
}
