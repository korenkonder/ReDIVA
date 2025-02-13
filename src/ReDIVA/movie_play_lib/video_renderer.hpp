/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "movie_play_lib.hpp"

namespace MoviePlayLib {
    class VideoRenderer : public IMediaRenderer {
    protected:
        RefCount ref_count;
        Lock lock;
        BOOL shutdown;
        BOOL sample_recieve;
        IMFSample* mf_sample;

    public:
        virtual HRESULT QueryInterface(const IID& riid, void** ppvObject) override;
        virtual ULONG AddRef() override;
        virtual ULONG Release() override;

        virtual HRESULT Shutdown() override;
        virtual HRESULT Close() override;
        virtual HRESULT Flush() override;
        virtual HRESULT Open() override;
        virtual HRESULT SetMFSample(IMFSample* mf_sample) override;

        VideoRenderer(IMediaClock* media_clock);
        virtual ~VideoRenderer();

        virtual HRESULT GetMFSample(IMFSample** ptr);

        static HRESULT Create(IMediaClock* media_clock, VideoRenderer*& ptr);
        static void Destroy(VideoRenderer* ptr);

        inline void Destroy() {
            Destroy(this);
        }
    };
}
