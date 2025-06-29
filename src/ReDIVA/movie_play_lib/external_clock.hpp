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
        RefCount m_ref;
        Callback m_callback;

    public:
        virtual HRESULT QueryInterface(const IID& riid, void** ppvObject) override;
        virtual ULONG AddRef() override;
        virtual ULONG Release() override;

        virtual HRESULT Stop() override;
        virtual HRESULT Start() override;
        virtual HRESULT SetTime(INT64 hnsTime) override;
        virtual INT64 GetTime() override;

        ExternalClock(Callback callback);
        virtual ~ExternalClock();

        static void Destroy(ExternalClock* ptr);

        inline void Destroy() {
            Destroy(this);
        }
    };

    extern HRESULT CreateExternalClock(ExternalClock::Callback callback, IMediaClock*& pp);
}
