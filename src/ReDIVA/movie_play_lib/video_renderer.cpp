/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "video_renderer.hpp"

namespace MoviePlayLib {
    HRESULT VideoRenderer::QueryInterface(const IID& riid, void** ppvObject) {
        if (!ppvObject)
            return E_INVALIDARG;

        if (riid == __uuidof(IMediaRenderer) || riid == __uuidof(IUnknown)) {
            *ppvObject = this;
            AddRef();
            return S_OK;
        }

        *ppvObject = 0;
        return E_NOINTERFACE;
    }

    ULONG VideoRenderer::AddRef() {
        return ++ref_count;
    }

    ULONG VideoRenderer::Release() {
        if (!--ref_count)
            Destroy();
        return ref_count;
    }

    HRESULT VideoRenderer::Shutdown() {
        lock.Acquire();
        if (mf_sample) {
            mf_sample->Release();
            mf_sample = 0;
        }
        shutdown = TRUE;
        lock.Release();
        return S_OK;
    }

    HRESULT VideoRenderer::Close() {
        lock.Acquire();
        HRESULT hr = shutdown ? MF_E_SHUTDOWN : S_OK;
        if (!shutdown)
            if (mf_sample) {
                mf_sample->Release();
                mf_sample = 0;
            }
        lock.Release();
        return hr;
    }

    HRESULT VideoRenderer::Flush() {
        lock.Acquire();
        HRESULT hr = shutdown ? MF_E_SHUTDOWN : S_OK;
        lock.Release();
        return hr;
    }

    HRESULT VideoRenderer::Open() {
        lock.Acquire();
        HRESULT hr = shutdown ? MF_E_SHUTDOWN : S_OK;
        lock.Release();
        return hr;
    }

    HRESULT VideoRenderer::SetMFSample(IMFSample* mf_sample) {
        lock.Acquire();
        HRESULT hr = shutdown ? MF_E_SHUTDOWN : 0;
        if (!shutdown) {
            if (mf_sample)
                mf_sample->Release();

            this->mf_sample = mf_sample;
            mf_sample->AddRef();

            sample_recieve = TRUE;
        }
        lock.Release();
        return hr;
    }

    VideoRenderer::VideoRenderer(IMediaClock* media_clock) : ref_count(),
        lock(), shutdown(), sample_recieve(), mf_sample() {
        MOVIE_PLAY_LIB_PRINT_FUNC_BEGIN;
        MOVIE_PLAY_LIB_PRINT_FUNC_END;
    }

    VideoRenderer::~VideoRenderer() {
        MOVIE_PLAY_LIB_PRINT_FUNC_BEGIN;
        MOVIE_PLAY_LIB_PRINT_FUNC_END;
    }

    HRESULT VideoRenderer::GetMFSample(IMFSample** ptr) {
        lock.Acquire();
        bool ret = !sample_recieve;
        sample_recieve = FALSE;

        *ptr = mf_sample;

        if (mf_sample)
            mf_sample->AddRef();
        lock.Release();
        return ret ? S_FALSE : S_OK;
    }

    HRESULT VideoRenderer::Create(IMediaClock* media_clock, VideoRenderer*& ptr) {
        VideoRenderer* p = new VideoRenderer(media_clock);
        if (!p)
            return E_OUTOFMEMORY;

        ptr = p;
        p->AddRef();
        p->Release();
        return S_OK;
    }

    inline void VideoRenderer::Destroy(VideoRenderer* ptr) {
        if (!ptr)
            return;

        delete ptr;
    }
}
