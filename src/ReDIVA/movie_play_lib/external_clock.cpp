/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "external_clock.hpp"

namespace MoviePlayLib {
    HRESULT ExternalClock::QueryInterface(const IID& riid, void** ppvObject) {
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

    ULONG ExternalClock::AddRef() {
        return ++m_ref;
    }

    ULONG ExternalClock::Release() {
        if (!--m_ref)
            Destroy();
        return m_ref;
    }

    HRESULT ExternalClock::Stop() {
        return S_OK;
    }

    HRESULT ExternalClock::Start() {
        return S_OK;
    }

    HRESULT ExternalClock::SetTime(INT64 hnsTime) {
        return S_OK;
    }

    INT64 ExternalClock::GetTime() {
        if (m_callback)
            return m_callback() / 100;
        return 0;
    }

    ExternalClock::ExternalClock(Callback callback) : m_ref(), m_callback(callback) {
        MOVIE_PLAY_LIB_TRACE_BEGIN;
        MOVIE_PLAY_LIB_TRACE_END;
    }

    ExternalClock::~ExternalClock() {
        MOVIE_PLAY_LIB_TRACE_BEGIN;
        MOVIE_PLAY_LIB_TRACE_END;
    }

    inline void ExternalClock::Destroy(ExternalClock* ptr) {
        if (!ptr)
            return;

        delete ptr;
    }

    HRESULT CreateExternalClock(ExternalClock::Callback callback, IMediaClock*& pp) {
        ExternalClock* p = new ExternalClock(callback);
        if (!p)
            return E_OUTOFMEMORY;

        pp = p;
        p->AddRef();
        p->Release();
        return S_OK;
    }
}
