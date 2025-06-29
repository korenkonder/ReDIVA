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
        RefCount m_ref;
        SlimLock m_lock;
        SlimLock m_sub;
        PlayerStat_& m_rStat;
        IMediaClock* m_pClock;
        IMFMediaSource* m_pSource;
        IMFPresentationDescriptor* m_pPresentDesc;
        IMFMediaType* m_pAudioType;
        IMFMediaType* m_pVideoType;
        MediaStream* m_pAudioStream;
        MediaStream* m_pVideoStream;
        IMediaTransform* m_pAudioDecoder;
        IMediaTransform* m_pVideoDecoder;
        DWORD m_dwAudioStreamID;
        DWORD m_dwVideoStreamID;
        BOOL m_bShutdown;
        BOOL m_bStarted;
        uint64_t m_hnsDuration;
        HANDLE m_hIntervalThread;
        HANDLE m_hIntervalTimer;
        HANDLE m_hQuitEvent;
        HANDLE m_hRequestEvent;
        HANDLE m_hCommandEvent;
        AsyncCallback<MediaSource> m_OnEvent;

    public:
        virtual HRESULT QueryInterface(const IID& riid, void** ppvObject) override;
        virtual ULONG AddRef() override;
        virtual ULONG Release() override;

        virtual HRESULT Shutdown() override;
        virtual HRESULT Stop() override;
        virtual HRESULT Start(INT64 hnsTime) override;
        virtual UINT64 GetDuration() override;
        virtual HRESULT GetAudioMediaType(IMFMediaType** ppType) override;
        virtual HRESULT GetVideoMediaType(IMFMediaType** ppType) override;
        virtual HRESULT SetAudioDecoder(IMediaTransform* pDecoder) override;
        virtual HRESULT SetVideoDecoder(IMediaTransform* pDecoder) override;

        MediaSource(HRESULT& hr, PlayerStat_& rStat,
            IMediaClock* pClock, const wchar_t* filePath);
        virtual ~MediaSource();

        virtual HRESULT OnEvent(IMFAsyncResult* pAsyncResult);

        static void Destroy(MediaSource* ptr);

        inline void Destroy() {
            Destroy(this);
        }

    protected:
        virtual HRESULT _async_callback_func(IMFAsyncResult* pAsyncResult);

        HRESULT _open(const wchar_t* filePath);

        static uint32_t __stdcall _thread_proc(MoviePlayLib::MediaSource* media_source);
    };

    extern HRESULT CreateMediaSource(PlayerStat_& rStat,
        IMediaClock* pClock, const wchar_t* filePath, IMediaSource*& pp);
}
