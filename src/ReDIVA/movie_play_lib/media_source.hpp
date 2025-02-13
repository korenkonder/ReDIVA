/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "movie_play_lib.hpp"
#include "media_stream.hpp"

namespace MoviePlayLib {
    class MediaSource : public IMediaSource {
    protected:
        RefCount ref_count;
        Lock lock;
        Lock async_lock;
        MediaStatsLock* media_stats_lock;
        IMediaClock* media_clock;
        IMFMediaSource* mf_media_source;
        IMFPresentationDescriptor* mf_presentation_descriptor;
        IMFMediaType* audio_mf_media_type;
        IMFMediaType* video_mf_media_type;
        MediaStream* audio_media_stream;
        MediaStream* video_media_stream;
        IMediaTransform* audio_media_transform;
        IMediaTransform* video_media_transform;
        DWORD audio_mf_stream_identifier;
        DWORD video_mf_stream_identifier;
        BOOL shutdown;
        BOOL start;
        uint64_t duration;
        HANDLE hThread;
        HANDLE hTimer;
        HANDLE hManualEvent;
        HANDLE hMediaStreamEvent;
        HANDLE hMediaSourceEvent;
        AsyncCallback<MediaSource> async_callback;

    public:
        virtual HRESULT QueryInterface(const IID& riid, void** ppvObject) override;
        virtual ULONG AddRef() override;
        virtual ULONG Release() override;

        virtual HRESULT Shutdown() override;
        virtual HRESULT Stop() override;
        virtual HRESULT Start(INT64 time) override;
        virtual UINT64 GetDuration() override;
        virtual HRESULT GetAudioMFMediaType(IMFMediaType** ptr) override;
        virtual HRESULT GetVideoMFMediaType(IMFMediaType** ptr) override;
        virtual HRESULT SetAudioMediaTransform(IMediaTransform* media_transform) override;
        virtual HRESULT SetVideoMediaTransform(IMediaTransform* media_transform) override;

        MediaSource(HRESULT& hr, MediaStatsLock* media_stats_lock,
            IMediaClock* media_clock, const wchar_t* url);
        virtual ~MediaSource();

        virtual HRESULT AsyncCallback(IMFAsyncResult* mf_async_result);

        HRESULT LoadURL(const wchar_t* url);

        static HRESULT Create(MediaStatsLock* media_stats_lock,
            IMediaClock* media_clock, const wchar_t* url, IMediaSource*& ptr);
        static void Destroy(MediaSource* ptr);
        static uint32_t __stdcall ThreadMain(MoviePlayLib::MediaSource* media_source);

        inline void Destroy() {
            Destroy(this);
        }
    };
}
