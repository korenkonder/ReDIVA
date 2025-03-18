/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "video_decoder.hpp"
#include "dx_surface_buffer.hpp"
#include "media_foundation.hpp"
#include "tracked_sample.hpp"
#include <mfapi.h>

namespace MoviePlayLib {
    VideoDecoder::VideoDecoder(HRESULT& hr, MediaStatsLock* media_stats_lock, IMediaClock* media_clock,
        IMediaSource* media_source, IDirect3DDeviceManager9* d3d_device_manager, IDirect3DDevice9* d3d_device)
        : TransformBase(hr, media_stats_lock, media_clock, media_source), sub_type(MFVideoFormat_NV12),
        d3d_device_manager(), d3d_device(), dx_video_processor(), buffer_size(), width(), height(),
        frame_size_width(), frame_size_height(), frame_rate(), frame_rate_numerator(), frame_rate_denominator(),
        pixel_aspect_ratio_numerator(), pixel_aspect_ratio_denominator(), video_area(), dxva2_video_desc(),
        dxva2_video_process_blt_params(), dxva2_video_sample(), sample_list(), async_callback(this) {
        MOVIE_PLAY_LIB_PRINT_FUNC_BEGIN;
        if (SUCCEEDED(hr)) {
            IMFMediaType* media_type = 0;
            hr = media_source->GetVideoMFMediaType(&media_type);
            if (SUCCEEDED(hr))
                hr = GetMFTransform(media_type, sub_type, mf_transform);

            if (media_type) {
                media_type->Release();
                media_type = 0;
            }

            if (SUCCEEDED(hr)) {
                this->d3d_device_manager = d3d_device_manager;
                d3d_device_manager->AddRef();
                this->d3d_device = d3d_device;
                d3d_device->AddRef();

                if (SUCCEEDED(hr)) {
                    hr = mf_transform->ProcessMessage(MFT_MESSAGE_SET_D3D_MANAGER, (ULONG_PTR)d3d_device_manager);
                    if (SUCCEEDED(hr))
                        hr = InitFromMediaSource();
                }
            }
        }
        MOVIE_PLAY_LIB_PRINT_FUNC_END;
    }

    VideoDecoder::~VideoDecoder() {
        MOVIE_PLAY_LIB_PRINT_FUNC_BEGIN;
        MOVIE_PLAY_LIB_PRINT_FUNC_END;
    }

    HRESULT VideoDecoder::ProcessOutput() {
        if (sample_list.GetSamplesCount() < 3)
            return TRUE;

        IMFSample* mf_sample = sample_list.PopSample();
        if (!mf_sample)
            return TRUE;

        HRESULT hr = MFSampleSetAllocator(mf_sample, &async_callback);

        MFT_OUTPUT_DATA_BUFFER output_sample = {};
        DWORD output_status = 0;

        int64_t process_output_begin;
        HRESULT process_output_hr;
        int64_t process_output_end;

        int64_t sample_time;
        int64_t sample_duration;
        DWORD sample_flags;
        IDirect3DSurface9* src_surface;
        IDirect3DSurface9* dst_surface;
        IMFMediaBuffer* mf_media_buffer;
        IMFGetService* mf_get_service;
        if (FAILED(hr))
            goto End;

        process_output_begin = GetTimestamp();
        process_output_hr = mf_transform->ProcessOutput(0, 1, &output_sample, &output_status);
        process_output_end = GetTimestamp();

        if (FAILED(process_output_hr)) {
            switch (process_output_hr) {
            case MF_E_TRANSFORM_STREAM_CHANGE:
                hr = InitFromMediaSource();
                break;
            case MF_E_TRANSFORM_NEED_MORE_INPUT:
                hr = S_OK;
                if (stream_state == 2)
                    stream_state = 3;
                else
                    stream_state = 0;
                break;
            default:
                hr = S_FALSE;
                stream_state = 0;
                break;
            }
            goto End;
        }

        sample_time = 0;
        sample_duration = 0;
        sample_flags = 0;
        hr = output_sample.pSample->GetSampleTime(&sample_time);
        if (SUCCEEDED(hr)) {
            hr = output_sample.pSample->GetSampleDuration(&sample_duration);
            if (SUCCEEDED(hr))
                hr = output_sample.pSample->GetSampleFlags(&sample_flags);
        }

        if (sample_duration < 1)
            sample_duration = frame_rate;

        if (FAILED(hr))
            goto End;

        hr = mf_sample->SetSampleTime(sample_time);
        if (FAILED(hr))
            goto End;

        hr = mf_sample->SetSampleDuration(sample_duration);
        if (FAILED(hr))
            goto End;

        hr = mf_sample->SetSampleFlags(sample_flags);
        if (FAILED(hr))
            goto End;

        mf_media_buffer = 0;
        dst_surface = 0;
        hr = mf_sample->GetBufferByIndex(0, &mf_media_buffer);
        if (SUCCEEDED(hr))
            hr = mf_media_buffer->QueryInterface(__uuidof(IDirect3DSurface9), (void**)&dst_surface);

        if (mf_media_buffer) {
            mf_media_buffer->Release();
            mf_media_buffer = 0;
        }

        mf_media_buffer = 0;
        mf_get_service = 0;
        src_surface = 0;
        if (SUCCEEDED(hr)) {
            hr = output_sample.pSample->GetBufferByIndex(0, &mf_media_buffer);
            if (SUCCEEDED(hr)) {
                hr = mf_media_buffer->QueryInterface(__uuidof(IMFGetService), (void**)&mf_get_service);
                if (SUCCEEDED(hr))
                    hr = mf_get_service->GetService(MR_BUFFER_SERVICE,
                        __uuidof(IDirect3DSurface9), (void**)&src_surface);
            }
        }

        if (mf_get_service) {
            mf_get_service->Release();
            mf_get_service = 0;
        }

        if (mf_media_buffer) {
            mf_media_buffer->Release();
            mf_media_buffer = 0;
        }

        if (SUCCEEDED(hr)) {
            dxva2_video_sample.SrcSurface = src_surface;
            const int64_t video_process_blt_begin = GetTimestamp();
            hr = dx_video_processor->VideoProcessBlt(dst_surface, &dxva2_video_process_blt_params, &dxva2_video_sample, 1, 0);
            const int64_t video_process_blt_end = GetTimestamp();
            if (SUCCEEDED(hr)) {
                sample_list_active.AddSample(mf_sample, false);

                LARGE_INTEGER freq;
                QueryPerformanceFrequency(&freq);
                media_stats_lock->SetVideoProcessOutput(
                    CalcTimeMsec(process_output_begin, process_output_end),
                    CalcTimeMsec(video_process_blt_begin, video_process_blt_end),
                    (double_t)sample_time * 0.0000001);
            }
        }

        if (src_surface) {
            src_surface->Release();
            src_surface = 0;
        }

        if (dst_surface) {
            dst_surface->Release();
            dst_surface = 0;
        }

    End:
        if (output_sample.pSample) {
            output_sample.pSample->Release();
            output_sample.pSample = 0;
        }

        if (output_sample.pEvents) {
            output_sample.pEvents->Release();
            output_sample.pEvents = 0;
        }

        mf_sample->Release();
        return hr;
    }

    void VideoDecoder::SetSampleTime(double_t value) {
        media_stats_lock->SetVideoSampleTime(value);
    }

    HRESULT VideoDecoder::AsyncCallback(IMFAsyncResult* mf_async_result) {
        TrackedSample* tracked_sample = 0;
        if (SUCCEEDED(mf_async_result->QueryInterface(__uuidof(TrackedSample),
            (void**)&tracked_sample)) && tracked_sample)
            sample_list.AddSample(tracked_sample, true);

        if (tracked_sample) {
            tracked_sample->Release();
            tracked_sample = 0;
        }
        return S_OK;
    }

    HRESULT VideoDecoder::InitFromMediaSource() {
        IMFMediaType* video_mf_media_type = 0;
        IMFMediaType* mf_media_type = 0;

        HRESULT hr;
        hr = media_source->GetVideoMFMediaType(&video_mf_media_type);
        if (SUCCEEDED(hr))
            hr = MoviePlayLib::GetMFMediaType(mf_transform, sub_type, mf_media_type);

        uint32_t buffer_size = 0;
        MFT_OUTPUT_STREAM_INFO stream_info = {};
        if (SUCCEEDED(hr)) {
            hr = mf_transform->GetOutputStreamInfo(0, &stream_info);
            buffer_size = stream_info.cbSize;
        }
        this->buffer_size = buffer_size;

        if (SUCCEEDED(hr)) {
            uint64_t frame_size = 0;
            hr = mf_media_type->GetUINT64(MF_MT_FRAME_SIZE, &frame_size);
            if (SUCCEEDED(hr)) {
                frame_size_height = (uint32_t)frame_size;
                frame_size_width = (uint32_t)(frame_size >> 32);
            }
        }

        width = frame_size_width;
        height = frame_size_height;

        if (SUCCEEDED(hr)) {
            uint64_t frame_rate = 0;
            if (SUCCEEDED(mf_media_type->GetUINT64(MF_MT_FRAME_RATE, &frame_rate))) {
                frame_rate_denominator = (uint32_t)frame_rate;
                frame_rate_numerator = (uint32_t)(frame_rate >> 32);
            }
            else {
                frame_rate_numerator = 60000;
                frame_rate_denominator = 1001;
            }

            this->frame_rate = MFllMulDiv(frame_rate_denominator, 10000000, frame_rate_numerator, 0);

            if (SUCCEEDED(mf_media_type->GetBlob(MF_MT_MINIMUM_DISPLAY_APERTURE,
                (UINT8*)&video_area, sizeof(video_area), 0))) {
                if (video_area.Area.cx > 0 && video_area.Area.cy > 0) {
                    frame_size_width = video_area.Area.cx;
                    frame_size_height = video_area.Area.cy;
                }
            }
            else {
                video_area.OffsetX = {};
                video_area.OffsetY = {};
                video_area.Area.cx = frame_size_width;
                video_area.Area.cy = frame_size_height;
            }

            uint64_t pixel_aspect_ratio = 0;
            if (SUCCEEDED(mf_media_type->GetUINT64(MF_MT_PIXEL_ASPECT_RATIO, &pixel_aspect_ratio))) {
                pixel_aspect_ratio_denominator = (uint32_t)pixel_aspect_ratio;
                pixel_aspect_ratio_numerator = (uint32_t)(pixel_aspect_ratio >> 32);
            }
            else {
                pixel_aspect_ratio_numerator = 1;
                pixel_aspect_ratio_denominator = 1;
            }

            dxva2_video_desc = {};
            dxva2_video_desc.SampleWidth = frame_size_width;
            dxva2_video_desc.SampleHeight = frame_size_height;

            dxva2_video_desc.SampleFormat.SampleFormat = DXVA2_SampleProgressiveFrame;
            dxva2_video_desc.SampleFormat.VideoChromaSubsampling
                = DXVA2_VideoChromaSubsampling_ProgressiveChroma | DXVA2_VideoChromaSubsampling_Vertically_Cosited;
            dxva2_video_desc.SampleFormat.VideoLighting = DXVA2_VideoLighting_dim;

            uint32_t nominal_range;
            if (FAILED(mf_media_type->GetUINT32(MF_MT_VIDEO_NOMINAL_RANGE, &nominal_range)))
                nominal_range = DXVA2_NominalRange_Normal;
            dxva2_video_desc.SampleFormat.NominalRange = nominal_range;

            uint32_t video_primaries;
            if (FAILED(mf_media_type->GetUINT32(MF_MT_VIDEO_PRIMARIES, &video_primaries)))
                video_primaries = DXVA2_VideoPrimaries_BT709;
            dxva2_video_desc.SampleFormat.VideoPrimaries = video_primaries;

            uint32_t video_transfer_matrix;
            if (FAILED(mf_media_type->GetUINT32(MF_MT_YUV_MATRIX, &video_transfer_matrix)))
                video_transfer_matrix = DXVA2_VideoTransferMatrix_BT709;
            dxva2_video_desc.SampleFormat.VideoTransferMatrix = video_transfer_matrix;

            uint32_t video_transfer_function;
            if (FAILED(mf_media_type->GetUINT32(MF_MT_TRANSFER_FUNCTION, &video_transfer_function)))
                video_transfer_function = DXVA2_VideoTransFunc_709;
            dxva2_video_desc.SampleFormat.VideoTransferFunction = video_transfer_function;

            dxva2_video_desc.Format = (D3DFORMAT)sub_type.Data1;
            dxva2_video_desc.InputSampleFreq.Numerator = frame_rate_numerator;
            dxva2_video_desc.InputSampleFreq.Denominator = frame_rate_denominator;
            dxva2_video_desc.OutputFrameFreq.Numerator = frame_rate_numerator;
            dxva2_video_desc.OutputFrameFreq.Denominator = frame_rate_denominator;

            if (dx_video_processor) {
                dx_video_processor->Release();
                dx_video_processor = 0;
            }

            hr = GetDirectXVideoProcessor(d3d_device_manager, &dxva2_video_desc, dx_video_processor);
            if (SUCCEEDED(hr)) {
                DXVA2_ValueRange proc_amp_brightness = {};
                DXVA2_ValueRange proc_amp_contrast = {};
                DXVA2_ValueRange proc_amp_hue = {};
                DXVA2_ValueRange proc_amp_saturation = {};
                dx_video_processor->GetProcAmpRange(DXVA2_ProcAmp_Brightness, &proc_amp_brightness);
                dx_video_processor->GetProcAmpRange(DXVA2_ProcAmp_Contrast, &proc_amp_contrast);
                dx_video_processor->GetProcAmpRange(DXVA2_ProcAmp_Hue, &proc_amp_hue);
                dx_video_processor->GetProcAmpRange(DXVA2_ProcAmp_Saturation, &proc_amp_saturation);

                dxva2_video_process_blt_params = {};
                dxva2_video_process_blt_params.TargetFrame = 0;
                dxva2_video_process_blt_params.TargetRect.left = 0;
                dxva2_video_process_blt_params.TargetRect.top = 0;
                dxva2_video_process_blt_params.TargetRect.right = frame_size_width;
                dxva2_video_process_blt_params.TargetRect.bottom = frame_size_height;
                dxva2_video_process_blt_params.BackgroundColor = { 0x8000, 0x8000, 0x1000, 0xFFFF };
                dxva2_video_process_blt_params.DestFormat.SampleFormat = DXVA2_SampleProgressiveFrame;
                dxva2_video_process_blt_params.DestFormat.VideoChromaSubsampling = DXVA2_VideoChromaSubsampling_Unknown;
                dxva2_video_process_blt_params.DestFormat.NominalRange = DXVA2_NominalRange_Normal;
                dxva2_video_process_blt_params.DestFormat.VideoTransferMatrix = DXVA2_VideoTransferMatrix_Unknown;
                dxva2_video_process_blt_params.DestFormat.VideoLighting = DXVA2_VideoLighting_dim;
                dxva2_video_process_blt_params.DestFormat.VideoPrimaries = DXVA2_VideoPrimaries_Unknown;
                dxva2_video_process_blt_params.DestFormat.VideoTransferFunction = DXVA2_VideoTransFunc_Unknown;
                dxva2_video_process_blt_params.ProcAmpValues.Brightness = proc_amp_brightness.DefaultValue;
                dxva2_video_process_blt_params.ProcAmpValues.Contrast = proc_amp_contrast.DefaultValue;
                dxva2_video_process_blt_params.ProcAmpValues.Hue = proc_amp_hue.DefaultValue;
                dxva2_video_process_blt_params.ProcAmpValues.Saturation = proc_amp_saturation.DefaultValue;
                dxva2_video_process_blt_params.Alpha.ll = 0x10000;

                dxva2_video_sample = {};
                dxva2_video_sample.Start = 0;
                dxva2_video_sample.End = 0;
                dxva2_video_sample.SampleFormat = dxva2_video_desc.SampleFormat;
                dxva2_video_sample.SrcSurface = 0;
                dxva2_video_sample.SrcRect.left = video_area.OffsetX.value;
                dxva2_video_sample.SrcRect.top = video_area.OffsetY.value;
                dxva2_video_sample.SrcRect.right = video_area.OffsetX.value + frame_size_width;
                dxva2_video_sample.SrcRect.bottom = video_area.OffsetY.value + frame_size_height;
                dxva2_video_sample.DstRect.left = 0;
                dxva2_video_sample.DstRect.top = 0;
                dxva2_video_sample.DstRect.right = frame_size_width;
                dxva2_video_sample.DstRect.bottom = frame_size_height;
                dxva2_video_sample.PlanarAlpha.ll = 0x10000;

                uint32_t video_index = sample_list.ClearList();
                for (int32_t i = 0; i < 10; i++) {
                    IMFSample* mf_sample = 0;
                    IMFMediaBuffer* mf_media_buffer = 0;
                    IDirect3DTexture9* d3d_texture = 0;
                    HANDLE shared_handle = 0;
                    if (SUCCEEDED(hr)) {
                        hr = d3d_device->CreateTexture(frame_size_width, frame_size_height,
                            1, 1, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &d3d_texture, &shared_handle);
                        if (SUCCEEDED(hr)) {
                            hr = DXSurfaceBuffer::Create(d3d_texture, mf_media_buffer);
                            if (SUCCEEDED(hr)) {
                                hr = TrackedSample::Create(mf_media_buffer, video_index, mf_sample);
                                if (SUCCEEDED(hr)) {
                                    hr = mf_sample->SetUINT64(TextureSharedHandleGUID, (UINT64)shared_handle);
                                    if (SUCCEEDED(hr))
                                        sample_list.AddSample(mf_sample, false);
                                }
                            }
                        }

                        if (d3d_texture) {
                            d3d_texture->Release();
                            d3d_texture = 0;
                        }

                        if (mf_sample) {
                            mf_sample->Release();
                            mf_sample = 0;
                        }

                        if (mf_media_buffer) {
                            mf_media_buffer->Release();
                            mf_media_buffer = 0;
                        }
                    }
                }

                if (SUCCEEDED(hr))
                    media_stats_lock->SetVideoParams(frame_size_width, frame_size_height, frame_size_width, frame_size_height,
                        (float_t)(int32_t)frame_rate_numerator / (float_t)(int32_t)frame_rate_denominator,
                        (float_t)(int32_t)pixel_aspect_ratio_numerator / (float_t)(int32_t)pixel_aspect_ratio_denominator);
            }
        }

        if (mf_media_type) {
            mf_media_type->Release();
            mf_media_type = 0;
        }

        if (video_mf_media_type) {
            video_mf_media_type->Release();
            video_mf_media_type = 0;
        }
        return hr;
    }

    HRESULT VideoDecoder::Create(MediaStatsLock* media_stats_lock, IMediaClock* media_clock,
        IMediaSource* media_source, IDirect3DDeviceManager9* d3d_device_manager,
        IDirect3DDevice9* d3d_device, IMediaTransform*& ptr) {
        HRESULT hr = S_OK;
        VideoDecoder* p = new VideoDecoder(hr, media_stats_lock,
            media_clock, media_source, d3d_device_manager, d3d_device);
        if (!p)
            return E_OUTOFMEMORY;

        if (SUCCEEDED(hr)) {
            ptr = p;
            p->AddRef();
        }
        else
            p->Shutdown();

        p->Release();
        return S_OK;
    }

    inline void VideoDecoder::Destroy(VideoDecoder* ptr) {
        if (!ptr)
            return;

        delete ptr;
    }
}
