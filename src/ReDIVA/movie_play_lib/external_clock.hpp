/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "movie_play_lib.hpp"

namespace MoviePlayLib {
    class ExternalClock : public IMediaClock {
    public:
        typedef int64_t(*Callback)();

    protected:
        RefCount ref_count;
        Callback callback;

    public:
        virtual HRESULT QueryInterface(const IID& riid, void** ppvObject) override;
        virtual ULONG AddRef() override;
        virtual ULONG Release() override;

        virtual HRESULT TimeEnd() override;
        virtual HRESULT TimeBegin() override;
        virtual HRESULT SetTime(INT64 value) override;
        virtual INT64 GetTime() override;

        ExternalClock(Callback callback);
        virtual ~ExternalClock();

        static HRESULT Create(Callback callback, IMediaClock*& ptr);
        static void Destroy(ExternalClock* ptr);

        inline void Destroy() {
            Destroy(this);
        }
    };
}
