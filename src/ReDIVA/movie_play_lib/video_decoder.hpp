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
        GUID m_formatSubType;
        IDirect3DDeviceManager9* m_pD3D9Manager;
        IDirect3DDevice9Ex* m_pD3D9Device;
        IDirectXVideoProcessor* m_pProcessor;
        uint32_t m_cbBufferBytes;
        uint32_t m_yuvWidth;
        uint32_t m_yuvHeight;
        uint32_t m_frameWidth;
        uint32_t m_frameHeight;
        int64_t m_frameDuration;
        MFRatio m_frameRate;
        MFRatio m_pixelAspect;
        MFVideoArea m_videoArea;
        DXVA2_VideoDesc m_videoDesc;
        DXVA2_VideoProcessBltParams m_bltParams;
        DXVA2_VideoSample m_videoSample;
        SampleQueue m_pool;
        AsyncCallback<VideoDecoder> m_asynccb_OnReturnSample;

    public:
        VideoDecoder(HRESULT& hr, PlayerStat_& rStat, IMediaClock* pClock, IMediaSource* pSource,
            IDirect3DDeviceManager9* pDeviceManager, IDirect3DDevice9Ex* pDevice);
        virtual ~VideoDecoder() override;

    protected:
        virtual void _on_shutdown() override;
        virtual HRESULT _process_output() override;
        virtual void _on_input_sample(double_t sampleTime) override;

    public:
        virtual HRESULT OnReturnSample(IMFAsyncResult* pAsyncResult);

        static void Destroy(VideoDecoder* ptr);

        inline void Destroy() {
            Destroy(this);
        }

    protected:
        virtual HRESULT _async_callback_func(IMFAsyncResult* pAsyncResult);

        HRESULT _on_format_changed();
    };

    extern HRESULT CreateVideoDecoder(PlayerStat_& rStat, IMediaClock* pClock, IMediaSource* pSource,
        IDirect3DDeviceManager9* pDeviceManager, IDirect3DDevice9Ex* pDevice, IMediaTransform*& pp);
}
