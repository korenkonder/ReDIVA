/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "media_source.hpp"
#include "media_foundation.hpp"
#include <mfapi.h>
#include <process.h>

namespace MoviePlayLib {
    HRESULT MediaSource::QueryInterface(const IID& riid, void** ppvObject) {
        if (!ppvObject)
            return E_INVALIDARG;

        if (riid == __uuidof(IMediaSource) || riid == __uuidof(IUnknown)) {
            *ppvObject = this;
            AddRef();
            return S_OK;
        }

        *ppvObject = 0;
        return E_NOINTERFACE;
    }

    ULONG MediaSource::AddRef() {
        return ++m_ref;
    }

    ULONG MediaSource::Release() {
        if (!--m_ref)
            Destroy();
        return m_ref;
    }

    HRESULT MediaSource::Shutdown() {
        MOVIE_PLAY_LIB_TRACE_BEGIN;
        m_lock.Acquire();
        m_sub.Acquire();
        _shutdown();
        m_sub.Release();
        m_lock.Release();
        MOVIE_PLAY_LIB_TRACE_END;
        return S_OK;
    }

    HRESULT MediaSource::Stop() {
        MOVIE_PLAY_LIB_TRACE_BEGIN;
        m_lock.Acquire();

        HRESULT hr;
        if (m_bShutdown)
            hr = MF_E_SHUTDOWN;
        else {
            hr = S_OK;

            if (m_bStarted) {
                m_bStarted = FALSE;

                hr = m_pSource->Stop();
                if (SUCCEEDED(hr))
                    WaitForSingleObject(m_hCommandEvent, INFINITE);
            }
        }

        m_lock.Release();
        MOVIE_PLAY_LIB_TRACE_END;
        return S_OK;
    }

    HRESULT MediaSource::Start(INT64 hnsTime) {
        MOVIE_PLAY_LIB_TRACE_BEGIN;
        m_lock.Acquire();

        HRESULT hr;
        if (m_bShutdown)
            hr = MF_E_SHUTDOWN;
        else {
            hr = S_OK;

            if (!m_bStarted) {
                PROPVARIANT propvar = {};
                m_bStarted = TRUE;
                propvar.hVal.QuadPart = hnsTime;
                propvar.vt = VT_I8;
                hr = m_pSource->Start(m_pPresentDesc, 0, &propvar);
                if (SUCCEEDED(hr))
                    WaitForSingleObject(m_hCommandEvent, INFINITE);
            }
        }

        m_lock.Release();
        MOVIE_PLAY_LIB_TRACE_END;
        return hr;
    }

    UINT64 MediaSource::GetDuration() {
        m_lock.Acquire();
        uint64_t hnsDuration = m_hnsDuration;
        m_lock.Release();
        return hnsDuration;
    }

    HRESULT MediaSource::GetAudioMediaType(IMFMediaType** ppType) {
        m_lock.Acquire();
        m_sub.Acquire();

        HRESULT hr;
        if (m_bShutdown)
            hr = MF_E_SHUTDOWN;
        else {
            hr = S_OK;

            if (m_dwAudioStreamID == -1)
                hr = MF_E_INVALIDSTREAMNUMBER;
            else if (m_pAudioStream)
                hr = m_pAudioStream->GetMediaType(ppType);
            else {
                *ppType = m_pAudioType;
                m_pAudioType->AddRef();
            }
        }

        m_sub.Release();
        m_lock.Release();
        return hr;
    }

    HRESULT MediaSource::GetVideoMediaType(IMFMediaType** ppType) {
        m_lock.Acquire();
        m_sub.Acquire();

        HRESULT hr;
        if (m_bShutdown)
            hr = MF_E_SHUTDOWN;
        else {
            hr = S_OK;

            if (m_dwVideoStreamID == -1)
                hr = MF_E_INVALIDSTREAMNUMBER;
            else if (m_pVideoStream)
                hr = m_pVideoStream->GetMediaType(ppType);
            else {
                *ppType = m_pVideoType;
                m_pVideoType->AddRef();
            }
        }

        m_sub.Release();
        m_lock.Release();
        return hr;
    }

    HRESULT MediaSource::SetAudioDecoder(IMediaTransform* pDecoder) {
        m_lock.Acquire();
        m_sub.Acquire();

        if (!m_bShutdown && m_dwAudioStreamID != -1) {
            if (m_pAudioDecoder) {
                m_pAudioDecoder->Release();
                m_pAudioDecoder = 0;
            }

            m_pAudioDecoder = pDecoder;
            pDecoder->AddRef();
        }

        m_sub.Release();
        m_lock.Release();
        return S_OK;
    }

    HRESULT MediaSource::SetVideoDecoder(IMediaTransform* pDecoder) {
        m_lock.Acquire();
        m_sub.Acquire();

        if (!m_bShutdown && m_dwVideoStreamID != -1) {
            if (m_pVideoDecoder) {
                m_pVideoDecoder->Release();
                m_pVideoDecoder = 0;
            }

            m_pVideoDecoder = pDecoder;
            pDecoder->AddRef();
        }

        m_sub.Release();
        m_lock.Release();
        return S_OK;
    }

    MediaSource::MediaSource(HRESULT& hr, PlayerStat_& rStat,
        IMediaClock* pClock, const wchar_t* filePath) : m_ref(), m_lock(), m_sub(), m_rStat(rStat),
        m_pClock(), m_pSource(), m_pAudioType(), m_pVideoType(), m_pAudioStream(),
        m_pVideoStream(), m_pAudioDecoder(), m_pVideoDecoder(), m_pPresentDesc(),
        m_dwAudioStreamID(-1), m_dwVideoStreamID(-1), m_bShutdown(), m_bStarted(), m_hnsDuration(),
        m_hIntervalThread(), m_hIntervalTimer(), m_hQuitEvent(), m_hRequestEvent(), m_hCommandEvent(),
        m_OnEvent(&MediaSource::_async_callback_func, this) {
        LARGE_INTEGER due_time;

        MOVIE_PLAY_LIB_TRACE_BEGIN;
        if (FAILED(hr))
            goto End;

        m_pClock = pClock;
        pClock->AddRef();

        if (FAILED(hr))
            goto End;

        m_hQuitEvent = CreateEventA(0, TRUE, FALSE, 0);
        if (!m_hQuitEvent)
            hr = HRESULT_FROM_WIN32(GetLastError());

        if (FAILED(hr))
            goto End;

        m_hCommandEvent = CreateEventA(0, FALSE, FALSE, 0);
        if (!m_hCommandEvent)
            hr = HRESULT_FROM_WIN32(GetLastError());

        if (FAILED(hr))
            goto End;

        m_hRequestEvent = CreateEventA(0, FALSE, FALSE, 0);
        if (!m_hRequestEvent)
            hr = HRESULT_FROM_WIN32(GetLastError());

        if (FAILED(hr))
            goto End;

        m_hIntervalTimer = CreateWaitableTimerA(0, FALSE, 0);
        if (!m_hIntervalTimer)
            hr = HRESULT_FROM_WIN32(GetLastError());

        if (FAILED(hr))
            goto End;

        due_time = {};
        if (!SetWaitableTimer(m_hIntervalTimer, &due_time, 50, 0, 0, FALSE))
            hr = HRESULT_FROM_WIN32(GetLastError());

        if (FAILED(hr))
            goto End;

        m_hIntervalThread = (HANDLE)_beginthreadex(0, 0, (_beginthreadex_proc_type)MediaSource::_thread_proc, this, 0, 0);
        if (!m_hIntervalThread)
            hr = HRESULT_FROM_WIN32(GetLastError());

        if (FAILED(hr))
            goto End;

        hr = _open(filePath);
        if (SUCCEEDED(hr))
            hr = m_pSource->BeginGetEvent(&m_OnEvent, 0);
    End:
        MOVIE_PLAY_LIB_TRACE_END;
    }

    MediaSource::~MediaSource() {
        MOVIE_PLAY_LIB_TRACE_BEGIN;
        _shutdown();

        if (m_pVideoDecoder) {
            m_pVideoDecoder->Release();
            m_pVideoDecoder = 0;
        }

        if (m_pAudioDecoder) {
            m_pAudioDecoder->Release();
            m_pAudioDecoder = 0;
        }

        if (m_pVideoStream) {
            m_pVideoStream->Release();
            m_pVideoStream = 0;
        }

        if (m_pAudioStream) {
            m_pAudioStream->Release();
            m_pAudioStream = 0;
        }

        if (m_pVideoType) {
            m_pVideoType->Release();
            m_pVideoType = 0;
        }

        if (m_pAudioType) {
            m_pAudioType->Release();
            m_pAudioType = 0;
        }

        if (m_pPresentDesc) {
            m_pPresentDesc->Release();
            m_pPresentDesc = 0;
        }

        if (m_pSource) {
            m_pSource->Release();
            m_pSource = 0;
        }

        if (m_pClock) {
            m_pClock->Release();
            m_pClock = 0;
        }

        if (m_hIntervalThread) {
            SetEvent(m_hQuitEvent);
            WaitForSingleObject(m_hIntervalThread, INFINITE);
            CloseHandle(m_hIntervalThread);
            m_hIntervalThread = 0;
        }

        if (m_hIntervalTimer) {
            CloseHandle(m_hIntervalTimer);
            m_hIntervalTimer = 0;
        }

        if (m_hRequestEvent) {
            CloseHandle(m_hRequestEvent);
            m_hRequestEvent = 0;
        }

        if (m_hCommandEvent) {
            CloseHandle(m_hCommandEvent);
            m_hCommandEvent = 0;
        }

        if (m_hQuitEvent) {
            CloseHandle(m_hQuitEvent);
            m_hQuitEvent = 0;
        }
        MOVIE_PLAY_LIB_TRACE_END;
    }

    HRESULT MediaSource::OnEvent(IMFAsyncResult* pAsyncResult) {
        m_sub.Acquire();
        HRESULT hrStatus = 0;
        MediaEventType mediaEventType = 0;
        IMFMediaEvent* pEvent = 0;

        HRESULT hr;
        hr = m_pSource->EndGetEvent(pAsyncResult, &pEvent);
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
            if (hrStatus < 0)
                hr = hrStatus;
            break;
        case MESourceStarted:
        case MESourceStopped:
            SetEvent(m_hCommandEvent);
            break;
        case MENewStream: {
            if (m_bShutdown)
                break;

            IMFMediaStream* pMediaStream = 0;
            IMFStreamDescriptor* pStreamDescriptor = 0;
            DWORD pStreamIdentifier = 0;
            MediaStream** pStream = 0;
            IMediaTransform* pDecoder = 0;

            hr = MFMediaEventQueryInterface(pEvent, IID_PPV_ARGS(&pMediaStream));
            if (FAILED(hr))
                goto MENewStreamEnd;

            hr = pMediaStream->GetStreamDescriptor(&pStreamDescriptor);
            if (FAILED(hr))
                goto MENewStreamEnd;

            hr = pStreamDescriptor->GetStreamIdentifier(&pStreamIdentifier);
            if (FAILED(hr))
                goto MENewStreamEnd;

            if (pStreamIdentifier == m_dwAudioStreamID) {
                if (m_pAudioStream) {
                    m_pAudioStream->Release();
                    m_pAudioStream = 0;
                }

                pStream = &m_pAudioStream;
                pDecoder = m_pAudioDecoder;
            }
            else if (pStreamIdentifier == m_dwVideoStreamID) {
                if (m_pVideoStream) {
                    m_pVideoStream->Release();
                    m_pVideoStream = 0;
                }

                pStream = &m_pVideoStream;
                pDecoder = m_pVideoDecoder;
            }
            else
                goto MENewStreamEnd;

            if (pDecoder)
                hr = CreateMediaStream(m_rStat, m_pClock,
                    pMediaStream, m_hRequestEvent, pDecoder, *pStream);

        MENewStreamEnd:
            if (pStreamDescriptor) {
                pStreamDescriptor->Release();
                pStreamDescriptor = 0;
            }

            if (pMediaStream) {
                pMediaStream->Release();
                pMediaStream = 0;
            }
        } break;
        }

    End:
        if (pEvent) {
            pEvent->Release();
            pEvent = 0;
        }

        if (!m_bShutdown && SUCCEEDED(hr))
            hr = m_pSource->BeginGetEvent(&m_OnEvent, 0);

        if (FAILED(hr)) {
            m_rStat.SetError(hr, this);

            if (m_pVideoStream)
                m_pVideoStream->Shutdown();
            if (m_pAudioStream)
                m_pAudioStream->Shutdown();
        }

        hr = pAsyncResult->SetStatus(hr);
        m_sub.Release();
        return hr;
    }

    inline void MediaSource::Destroy(MediaSource* ptr) {
        if (!ptr)
            return;

        delete ptr;
    }

    HRESULT MediaSource::_async_callback_func(IMFAsyncResult* pAsyncResult) {
        return OnEvent(pAsyncResult);
    }

    HRESULT MediaSource::_open(const wchar_t* filePath) {
        HRESULT hr = S_OK;
        if (!m_pSource) {
            IMFSourceResolver* mf_source_resolver = 0;
            IUnknown* object = 0;
            MF_OBJECT_TYPE object_type = MF_OBJECT_INVALID;

            hr = MFCreateSourceResolver(&mf_source_resolver);
            if (SUCCEEDED(hr)) {
                hr = mf_source_resolver->CreateObjectFromURL(filePath, MF_RESOLUTION_MEDIASOURCE, 0, &object_type, &object);
                if (SUCCEEDED(hr))
                    hr = object->QueryInterface(IID_PPV_ARGS(&m_pSource));
            }

            if (object) {
                object->Release();
                object = 0;
            }

            if (mf_source_resolver) {
                mf_source_resolver->Release();
                mf_source_resolver = 0;
            }
        }

        DWORD v28 = 0;
        if (SUCCEEDED(hr)) {
            hr = m_pSource->CreatePresentationDescriptor(&m_pPresentDesc);
            if (SUCCEEDED(hr)) {
                hr = m_pPresentDesc->GetStreamDescriptorCount(&v28);
                if (SUCCEEDED(hr))
                    hr = m_pPresentDesc->GetUINT64(MF_PD_DURATION, &m_hnsDuration);
            }
        }

        if (FAILED(hr))
            return hr;

        for (DWORD index = 0; index < v28 && SUCCEEDED(hr); index++) {
            BOOL selected = FALSE;
            IMFStreamDescriptor* pStreamDescriptor = 0;
            IMFMediaTypeHandler* pMediaTypeHandler = 0;
            DWORD pStreamIdentifier = 0;

            IMFMediaType* pType = 0;
            GUID guidMajorType = {};
            GUID guidFormatSubType = {};
            hr = m_pPresentDesc->GetStreamDescriptorByIndex(index, &selected, &pStreamDescriptor);
            if (SUCCEEDED(hr)) {
                hr = pStreamDescriptor->GetMediaTypeHandler(&pMediaTypeHandler);
                if (SUCCEEDED(hr)) {
                    hr = pStreamDescriptor->GetStreamIdentifier(&pStreamIdentifier);
                    if (SUCCEEDED(hr)) {
                        hr = pMediaTypeHandler->GetMajorType(&guidMajorType);
                        if (SUCCEEDED(hr)) {
                            hr = pMediaTypeHandler->GetMediaTypeByIndex(0, &pType);
                            if (SUCCEEDED(hr))
                                hr = pType->GetGUID(MF_MT_SUBTYPE, &guidFormatSubType);
                        }
                    }
                }
            }

            BOOL selectStream = FALSE;
            if (SUCCEEDED(hr)) {
                if (m_dwAudioStreamID == -1
                    && guidMajorType == MFMediaType_Audio
                    && (guidFormatSubType == MFAudioFormat_WMAudioV8
                        || guidFormatSubType == MFAudioFormat_WMAudioV9
                        || guidFormatSubType == MFAudioFormat_WMAudio_Lossless
                        || guidFormatSubType == MFAudioFormat_AAC
                        || guidFormatSubType == MFAudioFormat_Vorbis
                        || guidFormatSubType == MFAudioFormat_MPEG
                        || guidFormatSubType == MFAudioFormat_MP3)) {
                    m_pAudioType = pType;
                    m_dwAudioStreamID = pStreamIdentifier;
                    pType->AddRef();
                    selectStream = TRUE;
                }
                if (m_dwVideoStreamID == -1
                    && guidMajorType == MFMediaType_Video
                    && (guidFormatSubType == MFVideoFormat_WMV1
                        || guidFormatSubType == MFVideoFormat_WMV2
                        || guidFormatSubType == MFVideoFormat_WMV3
                        || guidFormatSubType == MFVideoFormat_WVC1
                        || guidFormatSubType == MFVideoFormat_H264
                        || guidFormatSubType == MFVideoFormat_VP80
                        || guidFormatSubType == MFVideoFormat_MPG1
                        || guidFormatSubType == MFVideoFormat_MPEG2)) {
                    m_pVideoType = pType;
                    m_dwVideoStreamID = pStreamIdentifier;
                    pType->AddRef();
                    selectStream = TRUE;
                }

                if (selectStream)
                    hr = m_pPresentDesc->SelectStream(index);
                else
                    hr = m_pPresentDesc->DeselectStream(index);
            }

            if (pType) {
                pType->Release();
                pType = 0;
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
        return hr;
    }

    inline void MediaSource::_shutdown() {
        m_bShutdown = TRUE;
        m_bStarted = FALSE;

        if (m_pVideoStream)
            m_pVideoStream->Shutdown();
        if (m_pAudioStream)
            m_pAudioStream->Shutdown();
        if (m_pSource)
            m_pSource->Shutdown();
    }

    uint32_t MediaSource::_thread_proc(MediaSource* media_source) {
        HANDLE pHandles[3];
        pHandles[0] = media_source->m_hQuitEvent;
        pHandles[1] = media_source->m_hRequestEvent;
        pHandles[2] = media_source->m_hIntervalTimer;
        while (WaitForMultipleObjects(3, pHandles, 0, INFINITE) - 1 <= 1) {
            media_source->m_lock.Acquire();
            media_source->m_sub.Acquire();

            if (!media_source->m_bShutdown && media_source->m_bStarted) {
                if (media_source->m_pAudioStream)
                    media_source->m_pAudioStream->RequestRead();
                if (media_source->m_pVideoStream)
                    media_source->m_pVideoStream->RequestRead();
            }

            media_source->m_sub.Release();
            media_source->m_lock.Release();
        }
        return 0;
    }

    HRESULT CreateMediaSource(PlayerStat_& rStat,
        IMediaClock* pClock, const wchar_t* filePath, IMediaSource*& pp) {
        HRESULT hr = S_OK;
        MediaSource* p = new MediaSource(hr, rStat, pClock, filePath);
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
}
