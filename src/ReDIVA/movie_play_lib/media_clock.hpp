/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "movie_play_lib.hpp"

namespace MoviePlayLib {
    class MediaClock : public IMediaClock {
    protected:
        RefCount m_ref;
        SlimLock m_lock;
        BOOL m_bStarted;
        int64_t m_hnsTime;
        int64_t m_llOffset;

    public:
        virtual HRESULT QueryInterface(const IID& riid, void** ppvObject) override;
        virtual ULONG AddRef() override;
        virtual ULONG Release() override;

        virtual HRESULT Stop() override;
        virtual HRESULT Start() override;
        virtual HRESULT SetTime(INT64 hnsTime) override;
        virtual INT64 GetTime() override;

        MediaClock();
        virtual ~MediaClock();

        static void Destroy(MediaClock* ptr);

        inline void Destroy() {
            Destroy(this);
        }

    protected:
        int64_t _get_time();
    };

    extern HRESULT CreateClock(IMediaClock*& pp);
}
