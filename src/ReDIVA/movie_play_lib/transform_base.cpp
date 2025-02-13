/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "transform_base.hpp"
#include <avrt.h>
#pragma comment(lib, "avrt.lib")
#include <process.h>

namespace MoviePlayLib {
    HRESULT TransformBase::QueryInterface(const IID& riid, void** ppvObject) {
        if (!ppvObject)
            return E_INVALIDARG;

        if (riid == __uuidof(IMediaTransform) || riid == __uuidof(IUnknown)) {
            *ppvObject = this;
            AddRef();
            return S_OK;
        }

        *ppvObject = 0;
        return E_NOINTERFACE;
    }

    ULONG TransformBase::AddRef() {
        return ++ref_count;
    }

    ULONG TransformBase::Release() {
        if (!--ref_count)
            Destroy();
        return ref_count;
    }

    HRESULT TransformBase::Shutdown() {
        lock.Acquire();
        Free();
        lock.Release();
        return S_OK;
    }

    HRESULT TransformBase::Close() {
        lock.Acquire();
        if (!shutdown)
            CommandFlush();
        HRESULT hr = CommandFlush();
        lock.Release();
        return hr;
    }

    HRESULT TransformBase::Flush() {
        lock.Acquire();
        HRESULT hr = shutdown ? MF_E_SHUTDOWN : S_OK;
        lock.Release();
        return hr;
    }

    HRESULT TransformBase::Open() {
        lock.Acquire();
        HRESULT hr = shutdown ? MF_E_SHUTDOWN : S_OK;
        if (!shutdown && !end_streaming) {
            end_streaming = TRUE;
            hr = Start();
        }
        lock.Release();
        return hr;
    }

    HRESULT TransformBase::GetMFMediaType(IMFMediaType** mf_media_type) {
        lock.Acquire();
        HRESULT hr = shutdown ? MF_E_SHUTDOWN : S_OK;
        if (!shutdown)
            hr = mf_transform->GetOutputCurrentType(0, mf_media_type);
        lock.Release();
        return hr;
    }

    void TransformBase::SendMFSample(IMFSample* mf_sample) {
        if (mf_sample)
            sample_list_wait.AddSample(mf_sample, false);
        else
            sample_list_wait.SetShutdown();

        SetEvent(hEvent);

        if (mf_sample) {
            int64_t sample_time = 0;
            mf_sample->GetSampleTime(&sample_time);
            SetSampleTime((double_t)sample_time * 0.0000001);
        }
    }

    BOOL TransformBase::SignalEvent() {
        return SetEvent(hEvent);
    }

    BOOL TransformBase::CanShutdown() {
        return sample_list_active.CanShutdown();
    }

    UINT32 TransformBase::GetMFSamplesWaitCount() {
        return (uint32_t)sample_list_wait.GetSamplesCount();
    }

    UINT32 TransformBase::GetMFSamplesCount() {
        return (uint32_t)sample_list_active.GetSamplesCount();
    }

    INT64 TransformBase::GetSampleTime() {
        return sample_list_active.GetSampleTime();
    }

    void TransformBase::GetMFSample(IMFSample*& mf_sample) {
        mf_sample = sample_list_active.PopSample();
    }

    TransformBase::TransformBase(HRESULT& hr, MediaStatsLock* media_stats_lock,
        IMediaClock* media_clock, IMediaSource* media_source) : ref_count(), lock(),
        media_stats_lock(media_stats_lock), media_clock(), media_source(), mf_transform(), stream_state(),
        shutdown(), end_streaming(), hThread(), hEvent(), sample_list_wait(), sample_list_active() {
        if (SUCCEEDED(hr)) {
            hEvent = CreateEventA(0, FALSE, FALSE, 0);
            if (!hEvent)
                hr = HRESULT_FROM_WIN32(GetLastError());

            if (SUCCEEDED(hr)) {
                hThread = (HANDLE)_beginthreadex(0, 0, (_beginthreadex_proc_type)TransformBase::ThreadMain, this, 0, 0);
                if (!hThread)
                    hr = HRESULT_FROM_WIN32(GetLastError());

                if (SUCCEEDED(hr)) {
                    this->media_clock = media_clock;
                    media_clock->AddRef();
                    this->media_source = media_source;
                    media_source->AddRef();
                }
            }
        }
    }

    TransformBase::~TransformBase() {
        if (hThread) {
           SetEvent(hEvent);
            WaitForSingleObject(hThread, INFINITE);
            CloseHandle(hThread);
            hThread = 0;
        }

        if (hEvent) {
            CloseHandle(hEvent);
            hEvent = 0;
        }
    }

    void TransformBase::Free() {
        CommandFlush();

        if (mf_transform) {
            mf_transform->Release();
            mf_transform = 0;
        }

        if (media_source) {
            media_source->Release();
            media_source = 0;
        }

        if (media_clock) {
            media_clock->Release();
            media_clock = 0;
        }

        shutdown = TRUE;
        end_streaming = FALSE;
    }

    HRESULT TransformBase::CommandFlush() {
        sample_list_wait.ClearList();
        sample_list_active.ClearList();

        HRESULT hr;
        if (mf_transform)
            hr = mf_transform->ProcessMessage(MFT_MESSAGE_COMMAND_FLUSH, 0);
        else
            hr = S_OK;
        stream_state = 0;
        return hr;
    }

    HRESULT TransformBase::NotifyEndOfStream() {
        HRESULT hr = mf_transform->ProcessMessage(MFT_MESSAGE_NOTIFY_END_OF_STREAM, 0);
        if (SUCCEEDED(hr))
            hr = mf_transform->ProcessMessage(MFT_MESSAGE_COMMAND_DRAIN, 0);
        return hr;
    }

    HRESULT TransformBase::NotifyEndStreaming() {
        return mf_transform->ProcessMessage(MFT_MESSAGE_NOTIFY_END_STREAMING, 0);
    }

    HRESULT TransformBase::Start() {
        HRESULT hr = mf_transform->ProcessMessage(MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, 0);
        if (SUCCEEDED(hr))
            hr = mf_transform->ProcessMessage(MFT_MESSAGE_NOTIFY_START_OF_STREAM, 0);
        stream_state = 0;

        sample_list_wait.ResetShutdown();
        sample_list_active.ResetShutdown();
        return hr;
    }

    HRESULT TransformBase::ProcessInput() {
        while (!stream_state) {
            IMFSample* mf_sample = sample_list_wait.PopSample();
            if (mf_sample) {
                if (SUCCEEDED(mf_transform->ProcessInput(0, mf_sample, 0)))
                    stream_state = 1;
            }
            else {
                if (!sample_list_wait.CanShutdown())
                    return S_FALSE;

                NotifyEndOfStream();
                stream_state = 2;
            }

            if (mf_sample) {
                mf_sample->Release();
                mf_sample = 0;
            }
        }
        return S_OK;
    }

    BOOL TransformBase::Playback() {
        BOOL shutdown = FALSE;
        HRESULT hr = S_OK;
        do
        {
            lock.Acquire();
            if (this->shutdown) {
                hr = MF_E_SHUTDOWN;
                shutdown = TRUE;
            }
            else if (!end_streaming)
                hr = S_FALSE;
            else if (stream_state || (hr = ProcessInput()) == S_OK) {
                if ((!stream_state || stream_state == 3
                    || (hr = ProcessOutput()) == S_OK) && stream_state == 3) {
                    sample_list_active.SetShutdown();
                    hr = S_FALSE;
                }
            }
            lock.Release();
            Sleep(0);
        } while (hr == S_OK);

        if (FAILED(hr)) {
            media_stats_lock->SetCurrError(hr, this);
            sample_list_active.SetShutdown();
        }
        return shutdown;
    }

    inline void TransformBase::Destroy(TransformBase* ptr) {
        if (!ptr)
            return;

        delete ptr;
    }

    uint32_t __stdcall TransformBase::ThreadMain(TransformBase* transform_base) {
        DWORD task_index = 0;
        HANDLE avrt_handle = AvSetMmThreadCharacteristicsW(L"Playback", &task_index);
        if (!avrt_handle)
#pragma warning(suppress: 6031)
            GetLastError();

        while (!WaitForSingleObject(transform_base->hEvent, INFINITE))
            if (transform_base->Playback())
                break;

        if (avrt_handle)
            AvRevertMmThreadCharacteristics(avrt_handle);
        return 0;
    }
}
