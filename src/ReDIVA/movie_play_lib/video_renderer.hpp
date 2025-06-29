/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "movie_play_lib.hpp"

namespace MoviePlayLib {
    class VideoRenderer : public IMediaRenderer {
    protected:
        RefCount m_ref;
        SlimLock m_lock;
        BOOL m_bShutdown;
        BOOL m_bUpdate;
        IMFSample* m_pSample;

    public:
        virtual HRESULT QueryInterface(const IID& riid, void** ppvObject) override;
        virtual ULONG AddRef() override;
        virtual ULONG Release() override;

        virtual HRESULT Shutdown() override;
        virtual HRESULT Close() override;
        virtual HRESULT Flush() override;
        virtual HRESULT Open() override;
        virtual HRESULT ProcessSample(IMFSample* pSample) override;

        VideoRenderer();
        virtual ~VideoRenderer();

        virtual HRESULT GetSample(IMFSample** ppOutSample);

        static void Destroy(VideoRenderer* ptr);

        inline void Destroy() {
            Destroy(this);
        }
    };

    extern HRESULT CreateVideoRenderer(IMediaClock* pClock, VideoRenderer*& pp);
}
