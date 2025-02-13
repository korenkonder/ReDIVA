/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "media_clock.hpp"
#include <mfapi.h>

namespace MoviePlayLib {
    HRESULT MediaClock::QueryInterface(const IID& riid, void** ppvObject) {
        if (!ppvObject)
            return E_INVALIDARG;

        if (riid == __uuidof(IMediaClock) || riid == __uuidof(IUnknown)) {
            *ppvObject = this;
            AddRef();
            return S_OK;
        }

        *ppvObject = 0;
        return E_NOINTERFACE;
    }

    ULONG MediaClock::AddRef() {
        return ++ref_count;
    }

    ULONG MediaClock::Release() {
        if (!--ref_count)
            Destroy();
        return ref_count;
    }

    HRESULT MediaClock::TimeEnd() {
        lock.Acquire();
        if (time_begin_init) {
            time = GetTimeInner();
            time_begin = GetTimestamp();
            time_begin_init = false;
        }
        lock.Release();
        return S_OK;
    }

    HRESULT MediaClock::TimeBegin() {
        lock.Acquire();
        if (!time_begin_init) {
            time_begin = GetTimestamp();
            time_begin_init = true;
        }
        lock.Release();
        return S_OK;
    }

    HRESULT MediaClock::SetTime(INT64 value) {
        lock.Acquire();
        time = value;
        time_begin = GetTimestamp();
        lock.Release();
        return S_OK;
    }

    INT64 MediaClock::GetTime() {
        lock.Acquire();
        int64_t res = GetTimeInner();
        lock.Release();
        return res;
    }

    MediaClock::MediaClock() : ref_count(), lock(), time_begin_init(), time(), time_begin() {
        MOVIE_PLAY_LIB_PRINT_FUNC_BEGIN;
        MOVIE_PLAY_LIB_PRINT_FUNC_END;
    }

    MediaClock::~MediaClock() {
        MOVIE_PLAY_LIB_PRINT_FUNC_BEGIN;
        MOVIE_PLAY_LIB_PRINT_FUNC_END;
    }

    int64_t MediaClock::GetTimeInner() {
        if (!time_begin_init)
            return time;

        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);

        return time + MFllMulDiv(GetTimestamp() - time_begin, 10000000, freq.QuadPart, 0);
    }

    HRESULT MediaClock::Create(IMediaClock*& ptr) {
        MediaClock* p = new MediaClock;
        if (!p)
            return E_OUTOFMEMORY;

        ptr = p;
        p->AddRef();
        p->Release();
        return S_OK;
    }

    inline void MediaClock::Destroy(MediaClock* ptr) {
        if (!ptr)
            return;

        delete ptr;
    }
}
