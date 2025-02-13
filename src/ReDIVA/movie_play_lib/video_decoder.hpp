/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "movie_play_lib.hpp"
#include "dxva2.hpp"
#include "transform_base.hpp"

namespace MoviePlayLib {
    class VideoDecoder : public TransformBase {
    protected:
        GUID sub_type;
        IDirect3DDeviceManager9* d3d_device_manager;
        IDirect3DDevice9* d3d_device;
        IDirectXVideoProcessor* dx_video_processor;
        uint32_t buffer_size;
        uint32_t width;
        uint32_t height;
        uint32_t frame_size_width;
        uint32_t frame_size_height;
        int64_t frame_rate;
        uint32_t frame_rate_numerator;
        uint32_t frame_rate_denominator;
        uint32_t pixel_aspect_ratio_numerator;
        uint32_t pixel_aspect_ratio_denominator;
        MFVideoArea video_area;
        DXVA2_VideoDesc dxva2_video_desc;
        DXVA2_VideoProcessBltParams dxva2_video_process_blt_params;
        DXVA2_VideoSample dxva2_video_sample;
        MFSampleList sample_list;
        AsyncCallback<VideoDecoder> async_callback;

    public:
        VideoDecoder(HRESULT& hr, MediaStatsLock* media_stats_lock, IMediaClock* media_clock,
            IMediaSource* media_source, IDirect3DDeviceManager9* d3d_device_manager, IDirect3DDevice9* d3d_device);
        virtual ~VideoDecoder() override;

        virtual HRESULT ProcessOutput() override;
        virtual void SetSampleTime(double_t value) override;

        virtual HRESULT AsyncCallback(IMFAsyncResult* mf_async_result);

        HRESULT InitFromMediaSource();

        static HRESULT Create(MediaStatsLock* media_stats_lock, IMediaClock* media_clock, IMediaSource* media_source,
            IDirect3DDeviceManager9* d3d_device_manager, IDirect3DDevice9* d3d_device, IMediaTransform*& ptr);
        static void Destroy(VideoDecoder* ptr);

        inline void Destroy() {
            Destroy(this);
        }
    };
}
