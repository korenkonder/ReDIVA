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
        RefCount m_ref;
        SlimLock m_lock;
        PlayerStat_& m_rStat;
        IMediaClock* m_pClock;
        IMFMediaStream* m_pStream;
        IMediaTransform* m_pDecoder;
        BOOL m_bShutdown;
        BOOL m_bRequested;
        HANDLE m_hRequestEvent;
        int64_t m_hnsLastSampleTime;
        AsyncCallback<MediaStream> m_OnEvent;

    public:
        virtual HRESULT QueryInterface(const IID& riid, void** ppvObject) override;
        virtual ULONG AddRef() override;
        virtual ULONG Release() override;

        MediaStream(HRESULT& hr, PlayerStat_& rStat, IMediaClock* pClock,
            IMFMediaStream* pStream, HANDLE hRequestEvent, IMediaTransform* pDecoder);
        virtual ~MediaStream();

        virtual HRESULT OnEvent(IMFAsyncResult* pAsyncResult);
        virtual HRESULT Shutdown();
        virtual HRESULT GetMediaType(IMFMediaType** ppOutMediaType);
        virtual HRESULT RequestRead();

        static void Destroy(MediaStream* ptr);

        inline void Destroy() {
            Destroy(this);
        }

    protected:
        virtual HRESULT _async_callback_func(IMFAsyncResult* pAsyncResult);

        void _request_read();
    };

    extern HRESULT CreateMediaStream(PlayerStat_& rStat, IMediaClock* pClock,
        IMFMediaStream* pStream, HANDLE hRequestEvent, IMediaTransform* pDecoder, MediaStream*& pp);
}
