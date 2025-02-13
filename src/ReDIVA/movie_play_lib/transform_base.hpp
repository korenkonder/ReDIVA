/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "movie_play_lib.hpp"
#include "mf_sample_list.hpp"
#include <mftransform.h>

namespace MoviePlayLib {
    class TransformBase : public IMediaTransform {
    protected:
        RefCount ref_count;
        Lock lock;
        MediaStatsLock* media_stats_lock;
        IMediaClock* media_clock;
        IMediaSource* media_source;
        IMFTransform* mf_transform;
        int32_t stream_state;
        BOOL shutdown;
        BOOL end_streaming;
        HANDLE hThread;
        HANDLE hEvent;
        MFSampleList sample_list_wait;
        MFSampleList sample_list_active;

    public:
        virtual HRESULT QueryInterface(const IID& riid, void** ppvObject) override;
        virtual ULONG AddRef() override;
        virtual ULONG Release() override;

        virtual HRESULT Shutdown() override;
        virtual HRESULT Close() override;
        virtual HRESULT Flush() override;
        virtual HRESULT Open() override;
        virtual HRESULT GetMFMediaType(IMFMediaType** mf_media_type) override;
        virtual void SendMFSample(IMFSample* mf_sample) override;
        virtual BOOL SignalEvent() override;
        virtual BOOL CanShutdown() override;
        virtual UINT32 GetMFSamplesWaitCount() override;
        virtual UINT32 GetMFSamplesCount() override;
        virtual INT64 GetSampleTime() override;
        virtual void GetMFSample(IMFSample*& mf_sample) override;

        TransformBase(HRESULT& hr, MediaStatsLock* media_stats_lock,
            IMediaClock* media_clock, IMediaSource* media_source);
        virtual ~TransformBase();

        virtual void Free();
        virtual HRESULT CommandFlush();
        virtual HRESULT NotifyEndOfStream();
        virtual HRESULT NotifyEndStreaming();
        virtual HRESULT Start();
        virtual HRESULT ProcessInput();
        virtual HRESULT ProcessOutput() = 0;
        virtual void SetSampleTime(double_t value) = 0;
        virtual BOOL Playback();

        static void Destroy(TransformBase* ptr);
        static uint32_t __stdcall ThreadMain(TransformBase* transform_base);

        inline void Destroy() {
            Destroy(this);
        }
    };
}
