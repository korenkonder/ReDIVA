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
        return ++m_ref;
    }

    ULONG VideoRenderer::Release() {
        if (!--m_ref)
            Destroy();
        return m_ref;
    }

    HRESULT VideoRenderer::Shutdown() {
        m_lock.Acquire();
        if (m_pSample) {
            m_pSample->Release();
            m_pSample = 0;
        }
        m_bShutdown = TRUE;
        m_lock.Release();
        return S_OK;
    }

    HRESULT VideoRenderer::Close() {
        m_lock.Acquire();
        HRESULT hr = m_bShutdown ? MF_E_SHUTDOWN : S_OK;
        if (!m_bShutdown)
            if (m_pSample) {
                m_pSample->Release();
                m_pSample = 0;
            }
        m_lock.Release();
        return hr;
    }

    HRESULT VideoRenderer::Flush() {
        m_lock.Acquire();
        HRESULT hr = m_bShutdown ? MF_E_SHUTDOWN : S_OK;
        m_lock.Release();
        return hr;
    }

    HRESULT VideoRenderer::Open() {
        m_lock.Acquire();
        HRESULT hr = m_bShutdown ? MF_E_SHUTDOWN : S_OK;
        m_lock.Release();
        return hr;
    }

    HRESULT VideoRenderer::ProcessSample(IMFSample* pSample) {
        m_lock.Acquire();
        HRESULT hr = m_bShutdown ? MF_E_SHUTDOWN : 0;
        if (!m_bShutdown) {
            if (pSample)
                pSample->Release();

            m_pSample = pSample;
            pSample->AddRef();

            m_bUpdate = TRUE;
        }
        m_lock.Release();
        return hr;
    }

    VideoRenderer::VideoRenderer() : m_ref(), m_lock(), m_bShutdown(), m_bUpdate(), m_pSample() {
        MOVIE_PLAY_LIB_TRACE_BEGIN;
        MOVIE_PLAY_LIB_TRACE_END;
    }

    VideoRenderer::~VideoRenderer() {
        MOVIE_PLAY_LIB_TRACE_BEGIN;
        MOVIE_PLAY_LIB_TRACE_END;
    }

    HRESULT VideoRenderer::GetSample(IMFSample** ppOutSample) {
        m_lock.Acquire();
        bool ret = !m_bUpdate;
        m_bUpdate = FALSE;

        *ppOutSample = m_pSample;

        if (m_pSample)
            m_pSample->AddRef();
        m_lock.Release();
        return ret ? S_FALSE : S_OK;
    }

    inline void VideoRenderer::Destroy(VideoRenderer* ptr) {
        if (!ptr)
            return;

        delete ptr;
    }

    HRESULT CreateVideoRenderer(IMediaClock* pClock, VideoRenderer*& pp) {
        VideoRenderer* p = new VideoRenderer;
        if (!p)
            return E_OUTOFMEMORY;

        pp = p;
        p->AddRef();
        p->Release();
        return S_OK;
    }
}
