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
        return ++ref_count;
    }

    ULONG MediaSource::Release() {
        if (!--ref_count)
            Destroy();
        return ref_count;
    }

    HRESULT MediaSource::Shutdown() {
        MOVIE_PLAY_LIB_PRINT_FUNC_BEGIN;
        lock.Acquire();
        async_lock.Acquire();

        shutdown = TRUE;
        start = FALSE;

        if (video_media_stream)
            video_media_stream->Shutdown();
        if (audio_media_stream)
            audio_media_stream->Shutdown();
        if (mf_media_source)
            mf_media_source->Shutdown();

        async_lock.Release();
        lock.Release();
        MOVIE_PLAY_LIB_PRINT_FUNC_END;
        return S_OK;
    }

    HRESULT MediaSource::Stop() {
        MOVIE_PLAY_LIB_PRINT_FUNC_BEGIN;
        lock.Acquire();

        HRESULT hr;
        if (shutdown)
            hr = MF_E_SHUTDOWN;
        else {
            hr = S_OK;

            if (start) {
                start = FALSE;

                hr = mf_media_source->Stop();
                if (SUCCEEDED(hr))
                    WaitForSingleObject(hMediaSourceEvent, INFINITE);
            }
        }

        lock.Release();
        MOVIE_PLAY_LIB_PRINT_FUNC_END;
        return S_OK;
    }

    HRESULT MediaSource::Start(INT64 time) {
        MOVIE_PLAY_LIB_PRINT_FUNC_BEGIN;
        lock.Acquire();

        HRESULT hr;
        if (shutdown)
            hr = MF_E_SHUTDOWN;
        else {
            hr = S_OK;

            if (!start) {
                PROPVARIANT propvar = {};
                start = TRUE;
                propvar.hVal.QuadPart = time;
                propvar.vt = VT_I8;
                hr = mf_media_source->Start(mf_presentation_descriptor, 0, &propvar);
                if (SUCCEEDED(hr))
                    WaitForSingleObject(hMediaSourceEvent, INFINITE);
            }
        }

        lock.Release();
        MOVIE_PLAY_LIB_PRINT_FUNC_END;
        return hr;
    }

    UINT64 MediaSource::GetDuration() {
        lock.Acquire();
        uint64_t value = this->duration;
        lock.Release();
        return value;
    }

    HRESULT MediaSource::GetAudioMFMediaType(IMFMediaType** ptr) {
        lock.Acquire();
        async_lock.Acquire();

        HRESULT hr;
        if (shutdown)
            hr = MF_E_SHUTDOWN;
        else {
            hr = S_OK;

            if (audio_mf_stream_identifier == -1)
                hr = MF_E_INVALIDSTREAMNUMBER;
            else if (audio_media_stream)
                hr = audio_media_stream->GetMFMediaType(ptr);
            else {
                *ptr = audio_mf_media_type;
                audio_mf_media_type->AddRef();
            }
        }

        async_lock.Release();
        lock.Release();
        return hr;
    }

    HRESULT MediaSource::GetVideoMFMediaType(IMFMediaType** ptr) {
        lock.Acquire();
        async_lock.Acquire();

        HRESULT hr;
        if (shutdown)
            hr = MF_E_SHUTDOWN;
        else {
            hr = S_OK;

            if (video_mf_stream_identifier == -1)
                hr = MF_E_INVALIDSTREAMNUMBER;
            else if (video_media_stream)
                hr = video_media_stream->GetMFMediaType(ptr);
            else {
                *ptr = video_mf_media_type;
                video_mf_media_type->AddRef();
            }
        }

        async_lock.Release();
        lock.Release();
        return hr;
    }

    HRESULT MediaSource::SetAudioMediaTransform(IMediaTransform* media_transform) {
        lock.Acquire();
        async_lock.Acquire();

        if (!shutdown && audio_mf_stream_identifier != -1) {
            if (audio_media_transform) {
                audio_media_transform->Release();
                audio_media_transform = 0;
            }

            audio_media_transform = media_transform;
            media_transform->AddRef();
        }

        async_lock.Release();
        lock.Release();
        return S_OK;
    }

    HRESULT MediaSource::SetVideoMediaTransform(IMediaTransform* media_transform) {
        lock.Acquire();
        async_lock.Acquire();

        if (!shutdown && video_mf_stream_identifier != -1) {
            if (video_media_transform) {
                video_media_transform->Release();
                video_media_transform = 0;
            }

            video_media_transform = media_transform;
            media_transform->AddRef();
        }

        async_lock.Release();
        lock.Release();
        return S_OK;
    }

    MediaSource::MediaSource(HRESULT& hr, MediaStatsLock* media_stats_lock,
        IMediaClock* media_clock, const wchar_t* url) : ref_count(), lock(), async_lock(), media_stats_lock(),
        media_clock(), mf_media_source(), audio_mf_media_type(), video_mf_media_type(), audio_media_stream(),
        video_media_stream(), audio_media_transform(), video_media_transform(), mf_presentation_descriptor(),
        audio_mf_stream_identifier(-1), video_mf_stream_identifier(-1), shutdown(), start(), duration(),
        hThread(), hTimer(), hManualEvent(), hMediaStreamEvent(), hMediaSourceEvent(), async_callback(this) {
        LARGE_INTEGER due_time;

        MOVIE_PLAY_LIB_PRINT_FUNC_BEGIN;
        if (FAILED(hr))
            goto End;

        this->media_clock = media_clock;
        media_clock->AddRef();

        if (FAILED(hr))
            goto End;

        hManualEvent = CreateEventA(0, TRUE, FALSE, 0);
        if (!hManualEvent)
            hr = HRESULT_FROM_WIN32(GetLastError());

        if (FAILED(hr))
            goto End;

        hMediaSourceEvent = CreateEventA(0, FALSE, FALSE, 0);
        if (!hMediaSourceEvent)
            hr = HRESULT_FROM_WIN32(GetLastError());

        if (FAILED(hr))
            goto End;

        hMediaStreamEvent = CreateEventA(0, FALSE, FALSE, 0);
        if (!hMediaStreamEvent)
            hr = HRESULT_FROM_WIN32(GetLastError());

        if (FAILED(hr))
            goto End;

        hTimer = CreateWaitableTimerA(0, FALSE, 0);
        if (!hTimer)
            hr = HRESULT_FROM_WIN32(GetLastError());

        if (FAILED(hr))
            goto End;

        due_time = {};
        if (!SetWaitableTimer(hTimer, &due_time, 50, 0, 0, FALSE))
            hr = HRESULT_FROM_WIN32(GetLastError());

        if (FAILED(hr))
            goto End;

        hThread = (HANDLE)_beginthreadex(0, 0, (_beginthreadex_proc_type)MediaSource::ThreadMain, this, 0, 0);
        if (!hThread)
            hr = HRESULT_FROM_WIN32(GetLastError());

        if (FAILED(hr))
            goto End;

        hr = LoadURL(url);
        if (SUCCEEDED(hr))
            hr = mf_media_source->BeginGetEvent(&async_callback, 0);
    End:
        MOVIE_PLAY_LIB_PRINT_FUNC_END;
    }

    MediaSource::~MediaSource() {
        MOVIE_PLAY_LIB_PRINT_FUNC_BEGIN;
        shutdown = TRUE;
        start = FALSE;

        if (video_media_stream)
            video_media_stream->Shutdown();
        if (audio_media_stream)
            audio_media_stream->Shutdown();
        if (mf_media_source)
            mf_media_source->Shutdown();

        if (video_media_transform) {
            video_media_transform->Release();
            video_media_transform = 0;
        }

        if (audio_media_transform) {
            audio_media_transform->Release();
            audio_media_transform = 0;
        }

        if (video_media_stream) {
            video_media_stream->Release();
            video_media_stream = 0;
        }

        if (audio_media_stream) {
            audio_media_stream->Release();
            audio_media_stream = 0;
        }

        if (video_mf_media_type) {
            video_mf_media_type->Release();
            video_mf_media_type = 0;
        }

        if (audio_mf_media_type) {
            audio_mf_media_type->Release();
            audio_mf_media_type = 0;
        }

        if (mf_presentation_descriptor) {
            mf_presentation_descriptor->Release();
            mf_presentation_descriptor = 0;
        }

        if (mf_media_source) {
            mf_media_source->Release();
            mf_media_source = 0;
        }

        if (media_clock) {
            media_clock->Release();
            media_clock = 0;
        }

        if (hThread) {
            SetEvent(hManualEvent);
            WaitForSingleObject(hThread, INFINITE);
            CloseHandle(hThread);
            hThread = 0;
        }

        if (hTimer) {
            CloseHandle(hTimer);
            hTimer = 0;
        }

        if (hMediaStreamEvent) {
            CloseHandle(hMediaStreamEvent);
            hMediaStreamEvent = 0;
        }

        if (hMediaSourceEvent) {
            CloseHandle(hMediaSourceEvent);
            hMediaSourceEvent = 0;
        }

        if (hManualEvent) {
            CloseHandle(hManualEvent);
            hManualEvent = 0;
        }
        MOVIE_PLAY_LIB_PRINT_FUNC_END;
    }

    HRESULT MediaSource::AsyncCallback(IMFAsyncResult* mf_async_result) {
        async_lock.Acquire();
        HRESULT media_event_status = 0;
        MediaEventType media_event_type = 0;
        IMFMediaEvent* mf_media_event = 0;

        HRESULT hr;
        hr = mf_media_source->EndGetEvent(mf_async_result, &mf_media_event);
        if (FAILED(hr))
            goto End;

        hr = mf_media_event->GetStatus(&media_event_status);
        if (FAILED(hr))
            goto End;

        hr = mf_media_event->GetType(&media_event_type);
        if (FAILED(hr))
            goto End;

        switch (media_event_type) {
        case MEError:
            hr = E_FAIL;
            if (media_event_status < 0)
                hr = media_event_status;
            break;
        case MESourceStarted:
        case MESourceStopped:
            SetEvent(hMediaSourceEvent);
            break;
        case MENewStream: {
            if (shutdown)
                break;

            IMFMediaStream* mf_media_stream = 0;
            IMFStreamDescriptor* mf_stream_decriptor = 0;
            DWORD mf_stream_identifier = 0;
            MoviePlayLib::MediaStream** media_stream = 0;
            IMediaTransform* media_transform = 0;

            hr = MFMediaEventQueryInterface(mf_media_event, IID_PPV_ARGS(&mf_media_stream));
            if (FAILED(hr))
                goto MENewStreamEnd;

            hr = mf_media_stream->GetStreamDescriptor(&mf_stream_decriptor);
            if (FAILED(hr))
                goto MENewStreamEnd;

            hr = mf_stream_decriptor->GetStreamIdentifier(&mf_stream_identifier);
            if (FAILED(hr))
                goto MENewStreamEnd;

            if (mf_stream_identifier == audio_mf_stream_identifier) {
                if (audio_media_stream) {
                    audio_media_stream->Release();
                    audio_media_stream = 0;
                }

                media_stream = &audio_media_stream;
                media_transform = audio_media_transform;
            }
            else if (mf_stream_identifier == video_mf_stream_identifier) {
                if (video_media_stream) {
                    video_media_stream->Release();
                    video_media_stream = 0;
                }

                media_stream = &video_media_stream;
                media_transform = video_media_transform;
            }
            else
                goto MENewStreamEnd;

            if (media_transform)
                hr = MediaStream::Create(media_stats_lock, media_clock,
                    mf_media_stream, hMediaStreamEvent, media_transform, *media_stream);

        MENewStreamEnd:
            if (mf_stream_decriptor) {
                mf_stream_decriptor->Release();
                mf_stream_decriptor = 0;
            }

            if (mf_media_stream) {
                mf_media_stream->Release();
                mf_media_stream = 0;
            }
        } break;
        }

    End:
        if (mf_media_event) {
            mf_media_event->Release();
            mf_media_event = 0;
        }

        if (!shutdown && SUCCEEDED(hr))
            hr = mf_media_source->BeginGetEvent(&async_callback, 0);

        if (FAILED(hr)) {
            media_stats_lock->SetCurrError(hr, this);

            if (video_media_stream)
                video_media_stream->Shutdown();
            if (audio_media_stream)
                audio_media_stream->Shutdown();
        }

        hr = mf_async_result->SetStatus(hr);
        async_lock.Release();
        return hr;
    }

    HRESULT MediaSource::LoadURL(const wchar_t* url) {
        HRESULT hr = S_OK;
        if (!mf_media_source) {
            IMFSourceResolver* mf_source_resolver = 0;
            IUnknown* object = 0;
            MF_OBJECT_TYPE object_type = MF_OBJECT_INVALID;

            hr = MFCreateSourceResolver(&mf_source_resolver);
            if (SUCCEEDED(hr)) {
                hr = mf_source_resolver->CreateObjectFromURL(url, MF_RESOLUTION_MEDIASOURCE, 0, &object_type, &object);
                if (SUCCEEDED(hr))
                    hr = object->QueryInterface(IID_PPV_ARGS(&mf_media_source));
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
            hr = mf_media_source->CreatePresentationDescriptor(&mf_presentation_descriptor);
            if (SUCCEEDED(hr)) {
                hr = mf_presentation_descriptor->GetStreamDescriptorCount(&v28);
                if (SUCCEEDED(hr))
                    hr = mf_presentation_descriptor->GetUINT64(MF_PD_DURATION, &duration);
            }
        }

        if (FAILED(hr))
            return hr;

        for (DWORD index = 0; index < v28 && SUCCEEDED(hr); index++) {
            BOOL selected = FALSE;
            IMFStreamDescriptor* mf_stream_decriptor = 0;
            IMFMediaTypeHandler* mf_media_type_handler = 0;
            DWORD mf_stream_identifier = 0;

            IMFMediaType* mf_media_type = 0;
            GUID guid_major_type = {};
            GUID guid_format_subtype = {};
            hr = mf_presentation_descriptor->GetStreamDescriptorByIndex(index, &selected, &mf_stream_decriptor);
            if (SUCCEEDED(hr)) {
                hr = mf_stream_decriptor->GetMediaTypeHandler(&mf_media_type_handler);
                if (SUCCEEDED(hr)) {
                    hr = mf_stream_decriptor->GetStreamIdentifier(&mf_stream_identifier);
                    if (SUCCEEDED(hr)) {
                        hr = mf_media_type_handler->GetMajorType(&guid_major_type);
                        if (SUCCEEDED(hr)) {
                            hr = mf_media_type_handler->GetMediaTypeByIndex(0, &mf_media_type);
                            if (SUCCEEDED(hr))
                                hr = mf_media_type->GetGUID(MF_MT_SUBTYPE, &guid_format_subtype);
                        }
                    }
                }
            }

            BOOL select_stream = FALSE;
            if (SUCCEEDED(hr)) {
                if (audio_mf_stream_identifier == -1
                    && guid_major_type == MFMediaType_Audio
                    && (guid_format_subtype == MFAudioFormat_WMAudioV8
                        || guid_format_subtype == MFAudioFormat_WMAudioV9
                        || guid_format_subtype == MFAudioFormat_WMAudio_Lossless
                        || guid_format_subtype == MFAudioFormat_AAC
                        || guid_format_subtype == MFAudioFormat_Vorbis
                        || guid_format_subtype == MFAudioFormat_MPEG
                        || guid_format_subtype == MFAudioFormat_MP3)) {
                    audio_mf_media_type = mf_media_type;
                    audio_mf_stream_identifier = mf_stream_identifier;
                    mf_media_type->AddRef();
                    select_stream = TRUE;
                }
                if (video_mf_stream_identifier == -1
                    && guid_major_type == MFMediaType_Video
                    && (guid_format_subtype == MFVideoFormat_WMV1
                        || guid_format_subtype == MFVideoFormat_WMV2
                        || guid_format_subtype == MFVideoFormat_WMV3
                        || guid_format_subtype == MFVideoFormat_WVC1
                        || guid_format_subtype == MFVideoFormat_H264
                        || guid_format_subtype == MFVideoFormat_VP80
                        || guid_format_subtype == MFVideoFormat_MPG1
                        || guid_format_subtype == MFVideoFormat_MPEG2)) {
                    video_mf_media_type = mf_media_type;
                    video_mf_stream_identifier = mf_stream_identifier;
                    mf_media_type->AddRef();
                    select_stream = TRUE;
                }

                if (select_stream)
                    hr = mf_presentation_descriptor->SelectStream(index);
                else
                    hr = mf_presentation_descriptor->DeselectStream(index);
            }

            if (mf_media_type) {
                mf_media_type->Release();
                mf_media_type = 0;
            }

            if (mf_media_type_handler) {
                mf_media_type_handler->Release();
                mf_media_type_handler = 0;
            }

            if (mf_stream_decriptor) {
                mf_stream_decriptor->Release();
                mf_stream_decriptor = 0;
            }
        }
        return hr;
    }

    HRESULT MediaSource::Create(MediaStatsLock* media_stats_lock,
        IMediaClock* media_clock, const wchar_t* url, IMediaSource*& ptr) {
        HRESULT hr = S_OK;
        MediaSource* p = new MediaSource(hr, media_stats_lock, media_clock, url);
        if (!p)
            return E_OUTOFMEMORY;

        if (SUCCEEDED(hr)) {
            ptr = p;
            p->AddRef();
        }
        else
            p->Shutdown();

        p->Release();
        return S_OK;
    }

    inline void MediaSource::Destroy(MediaSource* ptr) {
        if (!ptr)
            return;

        delete ptr;
    }

    uint32_t __stdcall MediaSource::ThreadMain(MediaSource* media_source) {
        HANDLE handles[3];
        handles[0] = media_source->hManualEvent;
        handles[1] = media_source->hMediaStreamEvent;
        handles[2] = media_source->hTimer;
        while (WaitForMultipleObjects(3, handles, 0, INFINITE) - 1 <= 1) {
            media_source->lock.Acquire();
            media_source->async_lock.Acquire();

            if (!media_source->shutdown && media_source->start) {
                if (media_source->audio_media_stream)
                    media_source->audio_media_stream->RequestSample();
                if (media_source->video_media_stream)
                    media_source->video_media_stream->RequestSample();
            }

            media_source->async_lock.Release();
            media_source->lock.Release();
        }
        return 0;
    }
}
