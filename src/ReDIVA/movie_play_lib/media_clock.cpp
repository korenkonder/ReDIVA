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
        return ++m_ref;
    }

    ULONG MediaClock::Release() {
        if (!--m_ref)
            Destroy();
        return m_ref;
    }

    HRESULT MediaClock::Stop() {
        m_lock.Acquire();
        if (m_bStarted) {
            m_hnsTime = _get_time();
            m_llOffset = GetTimestamp();
            m_bStarted = false;
        }
        m_lock.Release();
        return S_OK;
    }

    HRESULT MediaClock::Start() {
        m_lock.Acquire();
        if (!m_bStarted) {
            m_llOffset = GetTimestamp();
            m_bStarted = true;
        }
        m_lock.Release();
        return S_OK;
    }

    HRESULT MediaClock::SetTime(INT64 hnsTime) {
        m_lock.Acquire();
        m_hnsTime = hnsTime;
        m_llOffset = GetTimestamp();
        m_lock.Release();
        return S_OK;
    }

    INT64 MediaClock::GetTime() {
        m_lock.Acquire();
        int64_t time = _get_time();
        m_lock.Release();
        return time;
    }

    MediaClock::MediaClock() : m_ref(), m_lock(), m_bStarted(), m_hnsTime(), m_llOffset() {
        MOVIE_PLAY_LIB_TRACE_BEGIN;
        MOVIE_PLAY_LIB_TRACE_END;
    }

    MediaClock::~MediaClock() {
        MOVIE_PLAY_LIB_TRACE_BEGIN;
        MOVIE_PLAY_LIB_TRACE_END;
    }

    inline void MediaClock::Destroy(MediaClock* ptr) {
        if (!ptr)
            return;

        delete ptr;
    }

    int64_t MediaClock::_get_time() {
        if (!m_bStarted)
            return m_hnsTime;

        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);

        return m_hnsTime + MFllMulDiv(GetTimestamp() - m_llOffset, 10000000, freq.QuadPart, 0);
    }

    HRESULT CreateClock(IMediaClock*& pp) {
        MediaClock* p = new MediaClock;
        if (!p)
            return E_OUTOFMEMORY;

        pp = p;
        p->AddRef();
        p->Release();
        return S_OK;
    }
}
