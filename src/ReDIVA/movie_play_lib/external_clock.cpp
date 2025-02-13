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
        return ++ref_count;
    }

    ULONG ExternalClock::Release() {
        if (!--ref_count)
            Destroy();
        return ref_count;
    }

    HRESULT ExternalClock::TimeEnd() {
        return S_OK;
    }

    HRESULT ExternalClock::TimeBegin() {
        return S_OK;
    }

    HRESULT ExternalClock::SetTime(INT64 value) {
        return S_OK;
    }

    INT64 ExternalClock::GetTime() {
        if (callback)
            return callback() / 100;
        return 0;
    }

    ExternalClock::ExternalClock(Callback callback) : ref_count(), callback(callback) {
        MOVIE_PLAY_LIB_PRINT_FUNC_BEGIN;
        MOVIE_PLAY_LIB_PRINT_FUNC_END;
    }

    ExternalClock::~ExternalClock() {
        MOVIE_PLAY_LIB_PRINT_FUNC_BEGIN;
        MOVIE_PLAY_LIB_PRINT_FUNC_END;
    }

    HRESULT ExternalClock::Create(Callback callback, IMediaClock*& ptr) {
        ExternalClock* p = new ExternalClock(callback);
        if (!p)
            return E_OUTOFMEMORY;

        ptr = p;
        p->AddRef();
        p->Release();
        return S_OK;
    }

    inline void ExternalClock::Destroy(ExternalClock* ptr) {
        if (!ptr)
            return;

        delete ptr;
    }
}
