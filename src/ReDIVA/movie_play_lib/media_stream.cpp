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
        return ++ref_count;
    }

    ULONG MediaStream::Release() {
        if (!--ref_count)
            Destroy();
        return ref_count;
    }

    MediaStream::MediaStream(HRESULT& hr, MediaStatsLock* media_stats_lock, IMediaClock* media_clock,
        IMFMediaStream* mf_media_stream, HANDLE hEvent, IMediaTransform* media_transform)
        : ref_count(), lock(), media_stats_lock(media_stats_lock), media_clock(), mf_media_stream(),
        media_transform(), shutdown(), sample_recieve(), hEvent(hEvent), sample_time(), async_callback(this) {
        MOVIE_PLAY_LIB_PRINT_FUNC_BEGIN;
        if (SUCCEEDED(hr)) {
            this->media_clock = media_clock;
            media_clock->AddRef();

            this->mf_media_stream = mf_media_stream;
            mf_media_stream->AddRef();

            this->media_transform = media_transform;
            media_transform->AddRef();

            if (SUCCEEDED(hr))
                hr = this->mf_media_stream->BeginGetEvent(&async_callback, 0);
        }
        MOVIE_PLAY_LIB_PRINT_FUNC_END;
    }

    MediaStream::~MediaStream() {

    }

    HRESULT MediaStream::AsyncCallback(IMFAsyncResult* mf_async_result) {
        lock.Acquire();
        HRESULT media_event_status = 0;
        MediaEventType media_event_type = 0;
        IMFMediaEvent* mf_media_event = 0;

        HRESULT hr;
        hr = mf_media_stream->EndGetEvent(mf_async_result, &mf_media_event);
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
            if (FAILED(media_event_status))
                hr = media_event_status;
            break;
        case MEStreamStarted:
            if (!shutdown) {
                sample_recieve = FALSE;
                sample_time = 0;
                RequestSampleInner();
            }
            break;
        case MEEndOfStream:
            if (!shutdown)
                media_transform->SendMFSample(0);
            break;
        case MEMediaSample:
            if (!shutdown) {
                TrackedSample* tracked_sample = 0;
                hr = MFMediaEventQueryInterface(mf_media_event, __uuidof(TrackedSample), (void**)&tracked_sample);
                if (SUCCEEDED(hr)) {
                    int64_t sample_time = 0;
                    if (SUCCEEDED(tracked_sample->GetSampleTime(&sample_time))) {
                        media_transform->SendMFSample(tracked_sample);
                        this->sample_time = sample_time;
                    }
                }

                if (tracked_sample) {
                    tracked_sample->Release();
                    tracked_sample = 0;
                }

                sample_recieve = FALSE;
                SetEvent(hEvent);
            }
            break;
        }

    End:
        if (mf_media_event) {
            mf_media_event->Release();
            mf_media_event = 0;
        }

        if (!shutdown && SUCCEEDED(hr))
            hr = mf_media_stream->BeginGetEvent(&async_callback, 0);

        if (FAILED(hr)) {
            media_stats_lock->SetCurrError(hr, this);
            media_transform->SendMFSample(0);
        }

        hr = mf_async_result->SetStatus(hr);
        lock.Release();
        return hr;
    }

    HRESULT MediaStream::Shutdown() {
        MOVIE_PLAY_LIB_PRINT_FUNC_BEGIN;
        lock.Acquire();
        media_transform->SendMFSample(0);
        shutdown = TRUE;
        lock.Release();
        MOVIE_PLAY_LIB_PRINT_FUNC_END;
        return S_OK;
    }

    HRESULT MediaStream::GetMFMediaType(IMFMediaType** mf_media_type) {
        lock.Acquire();
        HRESULT hr = shutdown ? MF_E_SHUTDOWN : S_OK;
        if (!shutdown) {
            IMFStreamDescriptor* mf_stream_descriptor = 0;
            IMFMediaTypeHandler* mf_media_type_handler = 0;
            hr = mf_media_stream->GetStreamDescriptor(&mf_stream_descriptor);
            if (SUCCEEDED(hr)) {
                hr = mf_stream_descriptor->GetMediaTypeHandler(&mf_media_type_handler);
                if (SUCCEEDED(hr))
                    hr = mf_media_type_handler->GetCurrentMediaType(mf_media_type);
            }

            if (mf_media_type_handler) {
                mf_media_type_handler->Release();
                mf_media_type_handler = 0;
            }

            if (mf_stream_descriptor) {
                mf_stream_descriptor->Release();
                mf_stream_descriptor = 0;
            }
        }
        lock.Release();
        return hr;
    }

    HRESULT MediaStream::RequestSample() {
        lock.Acquire();
        HRESULT hr = shutdown ? MF_E_SHUTDOWN : S_OK;
        RequestSampleInner();
        lock.Release();
        return hr;
    }

    void MediaStream::RequestSampleInner() {
        if (!shutdown && !sample_recieve
            && sample_time < media_clock->GetTime() + 50000000
            && SUCCEEDED(mf_media_stream->RequestSample(0)))
            sample_recieve = TRUE;
    }

    HRESULT MediaStream::Create(MediaStatsLock* media_stats_lock, IMediaClock* media_clock,
        IMFMediaStream* mf_media_stream, HANDLE hEvent, IMediaTransform* media_transform, MediaStream*& ptr) {
        HRESULT hr = S_OK;
        MediaStream* p = new MediaStream(hr, media_stats_lock, media_clock, mf_media_stream, hEvent, media_transform);
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

    inline void MediaStream::Destroy(MediaStream* ptr) {
        if (!ptr)
            return;

        delete ptr;
    }
}
