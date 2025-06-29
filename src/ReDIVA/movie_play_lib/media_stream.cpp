/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "media_stream.hpp"
#include "media_foundation.hpp"
#include "tracked_sample.hpp"

namespace MoviePlayLib {
    HRESULT MediaStream::QueryInterface(const IID& riid, void** ppvObject) {
        if (!ppvObject)
            return E_INVALIDARG;

        if (riid == __uuidof(IUnknown)) {
            *ppvObject = this;
            AddRef();
            return S_OK;
        }

        *ppvObject = 0;
        return E_NOINTERFACE;
    }

    ULONG MediaStream::AddRef() {
        return ++m_ref;
    }

    ULONG MediaStream::Release() {
        if (!--m_ref)
            Destroy();
        return m_ref;
    }

    MediaStream::MediaStream(HRESULT& hr, PlayerStat_& rStat, IMediaClock* pClock,
        IMFMediaStream* pStream, HANDLE hRequestEvent, IMediaTransform* pDecoder)
        : m_ref(), m_lock(), m_rStat(rStat), m_pClock(), m_pStream(),
        m_pDecoder(), m_bShutdown(), m_bRequested(), m_hRequestEvent(hRequestEvent), m_hnsLastSampleTime(),
        m_OnEvent(&MediaStream::_async_callback_func, this) {
        MOVIE_PLAY_LIB_TRACE_BEGIN;
        if (SUCCEEDED(hr)) {
            m_pClock = pClock;
            pClock->AddRef();

            m_pStream = pStream;
            pStream->AddRef();

            m_pDecoder = pDecoder;
            pDecoder->AddRef();

            if (SUCCEEDED(hr))
                hr = m_pStream->BeginGetEvent(&m_OnEvent, 0);
        }
        MOVIE_PLAY_LIB_TRACE_END;
    }

    MediaStream::~MediaStream() {

    }

    HRESULT MediaStream::OnEvent(IMFAsyncResult* pAsyncResult) {
        m_lock.Acquire();
        HRESULT hrStatus = 0;
        MediaEventType mediaEventType = 0;
        IMFMediaEvent* pEvent = 0;

        HRESULT hr;
        hr = m_pStream->EndGetEvent(pAsyncResult, &pEvent);
        if (FAILED(hr))
            goto End;

        hr = pEvent->GetStatus(&hrStatus);
        if (FAILED(hr))
            goto End;

        hr = pEvent->GetType(&mediaEventType);
        if (FAILED(hr))
            goto End;

        switch (mediaEventType) {
        case MEError:
            hr = E_FAIL;
            if (FAILED(hrStatus))
                hr = hrStatus;
            break;
        case MEStreamStarted:
            if (!m_bShutdown) {
                m_bRequested = FALSE;
                m_hnsLastSampleTime = 0;
                _request_read();
            }
            break;
        case MEEndOfStream:
            if (!m_bShutdown)
                m_pDecoder->PushSample(0);
            break;
        case MEMediaSample:
            if (!m_bShutdown) {
                TrackedSample* pTrackedSample = 0;
                hr = MFMediaEventQueryInterface(pEvent, __uuidof(TrackedSample), (void**)&pTrackedSample);
                if (SUCCEEDED(hr)) {
                    int64_t hnsLastSampleTime = 0;
                    if (SUCCEEDED(pTrackedSample->GetSampleTime(&hnsLastSampleTime))) {
                        m_pDecoder->PushSample(pTrackedSample);
                        m_hnsLastSampleTime = hnsLastSampleTime;
                    }
                }

                if (pTrackedSample) {
                    pTrackedSample->Release();
                    pTrackedSample = 0;
                }

                m_bRequested = FALSE;
                SetEvent(m_hRequestEvent);
            }
            break;
        }

    End:
        if (pEvent) {
            pEvent->Release();
            pEvent = 0;
        }

        if (!m_bShutdown && SUCCEEDED(hr))
            hr = m_pStream->BeginGetEvent(&m_OnEvent, 0);

        if (FAILED(hr)) {
            m_rStat.SetError(hr, this);
            m_pDecoder->PushSample(0);
        }

        hr = pAsyncResult->SetStatus(hr);
        m_lock.Release();
        return hr;
    }

    HRESULT MediaStream::Shutdown() {
        MOVIE_PLAY_LIB_TRACE_BEGIN;
        m_lock.Acquire();
        m_pDecoder->PushSample(0);
        m_bShutdown = TRUE;
        m_lock.Release();
        MOVIE_PLAY_LIB_TRACE_END;
        return S_OK;
    }

    HRESULT MediaStream::GetMediaType(IMFMediaType** ppOutMediaType) {
        m_lock.Acquire();
        HRESULT hr = m_bShutdown ? MF_E_SHUTDOWN : S_OK;
        if (!m_bShutdown) {
            IMFStreamDescriptor* pStreamDescriptor = 0;
            IMFMediaTypeHandler* pMediaTypeHandler = 0;
            hr = m_pStream->GetStreamDescriptor(&pStreamDescriptor);
            if (SUCCEEDED(hr)) {
                hr = pStreamDescriptor->GetMediaTypeHandler(&pMediaTypeHandler);
                if (SUCCEEDED(hr))
                    hr = pMediaTypeHandler->GetCurrentMediaType(ppOutMediaType);
            }

            if (pMediaTypeHandler) {
                pMediaTypeHandler->Release();
                pMediaTypeHandler = 0;
            }

            if (pStreamDescriptor) {
                pStreamDescriptor->Release();
                pStreamDescriptor = 0;
            }
        }
        m_lock.Release();
        return hr;
    }

    HRESULT MediaStream::RequestRead() {
        m_lock.Acquire();
        HRESULT hr = m_bShutdown ? MF_E_SHUTDOWN : S_OK;
        _request_read();
        m_lock.Release();
        return hr;
    }

    HRESULT MediaStream::_async_callback_func(IMFAsyncResult* pAsyncResult) {
        return OnEvent(pAsyncResult);
    }

    void MediaStream::_request_read() {
        if (!m_bShutdown && !m_bRequested
            && m_hnsLastSampleTime < m_pClock->GetTime() + 50000000
            && SUCCEEDED(m_pStream->RequestSample(0)))
            m_bRequested = TRUE;
    }

    HRESULT CreateMediaStream(PlayerStat_& rStat, IMediaClock* pClock,
        IMFMediaStream* pStream, HANDLE hRequestEvent, IMediaTransform* pDecoder, MediaStream*& pp) {
        HRESULT hr = S_OK;
        MediaStream* p = new MediaStream(hr, rStat, pClock, pStream, hRequestEvent, pDecoder);
        if (!p)
            return E_OUTOFMEMORY;

        if (SUCCEEDED(hr)) {
            pp = p;
            p->AddRef();
        }
        else
            p->Shutdown();

        p->Release();
        return S_OK;
    }

    inline void MediaStream::Destroy(MediaStream* ptr) {
        if (!ptr)
            return;

        delete ptr;
    }
}
