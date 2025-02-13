/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "movie_play_lib.hpp"
#include <mfidl.h>

namespace MoviePlayLib {
    class MediaStream : public IUnknown {
    protected:
        RefCount ref_count;
        Lock lock;
        MediaStatsLock* media_stats_lock;
        IMediaClock* media_clock;
        IMFMediaStream* mf_media_stream;
        IMediaTransform* media_transform;
        BOOL shutdown;
        BOOL sample_recieve;
        HANDLE hEvent;
        int64_t sample_time;
        AsyncCallback<MediaStream> async_callback;

    public:
        virtual HRESULT QueryInterface(const IID& riid, void** ppvObject) override;
        virtual ULONG AddRef() override;
        virtual ULONG Release() override;

        MediaStream(HRESULT& hr, MediaStatsLock* media_stats_lock, IMediaClock* media_clock,
            IMFMediaStream* mf_media_stream, HANDLE hEvent, IMediaTransform* media_transform);
        virtual ~MediaStream();

        virtual HRESULT AsyncCallback(IMFAsyncResult* mf_async_result);
        virtual HRESULT Shutdown();
        virtual HRESULT GetMFMediaType(IMFMediaType** mf_media_type);
        virtual HRESULT RequestSample();

        void RequestSampleInner();

        static HRESULT Create(MediaStatsLock* media_stats_lock, IMediaClock* media_clock,
            IMFMediaStream* mf_media_stream, HANDLE hEvent, IMediaTransform* media_transform, MediaStream*& ptr);
        static void Destroy(MediaStream* ptr);

        inline void Destroy() {
            Destroy(this);
        }
    };
}
