/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "movie_play_lib.hpp"

namespace MoviePlayLib {
    class MediaClock : public IMediaClock {
    protected:
        RefCount ref_count;
        Lock lock;
        BOOL time_begin_init;
        int64_t time;
        int64_t time_begin;

    public:
        virtual HRESULT QueryInterface(const IID& riid, void** ppvObject) override;
        virtual ULONG AddRef() override;
        virtual ULONG Release() override;

        virtual HRESULT TimeEnd() override;
        virtual HRESULT TimeBegin() override;
        virtual HRESULT SetTime(INT64 value) override;
        virtual INT64 GetTime() override;

        MediaClock();
        virtual ~MediaClock();

        int64_t GetTimeInner();

        static HRESULT Create(IMediaClock*& ptr);
        static void Destroy(MediaClock* ptr);

        inline void Destroy() {
            Destroy(this);
        }
    };
}
