/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "media_session.hpp"
#include "../../KKdLib/prj/math.hpp"
#include "audio_decoder.hpp"
#include "command_args.hpp"
#include "media_clock.hpp"
#include "media_source.hpp"
#include "video_decoder.hpp"
#include <avrt.h>
#pragma comment(lib, "avrt.lib")
#include <dxva2api.h>
#pragma comment(lib, "dxva2.lib")
#include <mfapi.h>
#pragma comment(lib, "mfplat.lib")

extern int32_t mf_media_session_count;
extern void mf_media_session_count_increment();
extern void mf_media_session_count_decrement();

namespace MoviePlayLib {
    HRESULT MediaSession::QueryInterface(const IID& riid, void** ppvObject) {
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

    ULONG MediaSession::AddRef() {
        return ++m_ref;
    }

    ULONG MediaSession::Release() {
        if (!--m_ref)
            Destroy();
        return m_ref;
    }

    MediaSession::MediaSession(HRESULT& hr, const wchar_t* filePath, IMediaClock* pClock) : m_ref(), m_lock(),
        m_workQID(), m_status(), m_bScheduleStart(), m_rStat(), m_pClock(), m_pSource(), m_pAudioDecoder(),
        m_pVideoDecoder(), m_pAudioRenderer(), m_pVideoRenderer(), m_pInteropTexture(), m_hnsDuration(),
        m_hnsAudioPresentEndTime(INT64_MAX), m_hnsVideoPresentEndTime(INT64_MAX), m_hIntervalThread(),
        m_hIntervalTimer(), m_hQuitEvent(), m_asynccb_OnProcessCommand(&MediaSession::_async_callback_func, this) {
        MOVIE_PLAY_LIB_TRACE_BEGIN;
        mf_media_session_count_increment();

        LARGE_INTEGER dueTime;
        CommandArgs* pCommandArgs;
        if (FAILED(hr))
            goto End;

        if (pClock) {
            m_pClock = pClock;
            pClock->AddRef();
        }
        else
            hr = CreateClock(m_pClock);

        if (FAILED(hr))
            goto End;

        m_hQuitEvent = CreateEventA(0, TRUE, FALSE, 0);
        if (!m_hQuitEvent)
            hr = HRESULT_FROM_WIN32(GetLastError());

        if (FAILED(hr))
            goto End;

        m_hIntervalTimer = CreateWaitableTimerA(0, FALSE, 0);
        if (!m_hIntervalTimer)
            hr = HRESULT_FROM_WIN32(GetLastError());

        if (FAILED(hr))
            goto End;

        dueTime = {};
        if (!SetWaitableTimer(m_hIntervalTimer, &dueTime, 10, 0, 0, FALSE))
            hr = HRESULT_FROM_WIN32(GetLastError());

        if (FAILED(hr))
            goto End;

        m_hIntervalThread = (HANDLE)_beginthreadex(0, 0, (_beginthreadex_proc_type)MediaSession::_thread_proc, this, 0, 0);
        if (!m_hIntervalThread)
            hr = HRESULT_FROM_WIN32(GetLastError());

        if (FAILED(hr))
            goto End;

        hr = MFAllocateWorkQueue(&m_workQID);
        pCommandArgs = 0;
        if (SUCCEEDED(hr)) {
            hr = CreateCommandArgs(MediaSession::Command_Open, pCommandArgs);
            if (SUCCEEDED(hr)) {
                hr = pCommandArgs->SetString(filePath);
                if (SUCCEEDED(hr))
                    hr = MFPutWorkItem(m_workQID, &m_asynccb_OnProcessCommand, pCommandArgs);
            }
        }

        if (pCommandArgs) {
            pCommandArgs->Release();
            pCommandArgs = 0;
        }

    End:
        MOVIE_PLAY_LIB_TRACE_END;
    }

    MediaSession::~MediaSession() {
        MOVIE_PLAY_LIB_TRACE_BEGIN;
        _on_shutdown();

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

        if (m_hQuitEvent) {
            CloseHandle(m_hQuitEvent);
            m_hQuitEvent = 0;
        }

        if (m_pClock) {
            m_pClock->Release();
            m_pClock = 0;
        }

        MFUnlockWorkQueue(m_workQID);
        m_workQID = 0;

        mf_media_session_count_decrement();
        MOVIE_PLAY_LIB_TRACE_END;
    }

    HRESULT MediaSession::OnProcessCommand(IMFAsyncResult* pAsyncResult) {
        IUnknown* pUnk = 0;
        CommandArgs* pCommandArgs = 0;
        HRESULT hr = pAsyncResult->GetState(&pUnk);
        if (FAILED(hr))
            goto End;

        hr = pUnk->QueryInterface(IID_PPV_ARGS(&pCommandArgs));
        if (FAILED(hr))
            goto End;

        switch (pCommandArgs->GetCommand()) {
        case MediaSession::Command_Open:
            hr = open(pCommandArgs->GetString());
            break;
        case MediaSession::Command_SetPosition:
            hr = set_position(pCommandArgs->GetValue());

            m_status.SetInnerStateAdvance(m_status.m_innerState);
            break;
        case MediaSession::Command_Play:
            if (m_pAudioRenderer)
                m_pAudioRenderer->Open();
            if (m_pVideoRenderer)
                m_pVideoRenderer->Open();

            Scheduler_Start();

            OnProcessSchedule();

            m_pClock->Start();

            m_status.SetInnerStateAdvance(Status_Stopping);
            break;
        case MediaSession::Command_Pause:
            m_pClock->Stop();

            Scheduler_Stop();

            if (m_pAudioRenderer)
                m_pAudioRenderer->Flush();
            if (m_pVideoRenderer)
                m_pVideoRenderer->Flush();

            m_status.SetInnerStateAdvance(Status_Started);
            break;
        case MediaSession::Command_Shutdown:
            _on_shutdown();
            break;
        }

    End:
        if (pCommandArgs) {
            pCommandArgs->Release();
            pCommandArgs = 0;
        }

        if (pUnk) {
            pUnk->Release();
            pUnk = 0;
        }

        if (FAILED(hr))
            m_rStat.SetError(hr, this);

        return pAsyncResult->SetStatus(hr);
    }

    HRESULT MediaSession::Shutdown() {
        m_lock.Acquire();
        if (m_pInteropTexture) {
            m_pInteropTexture->Release();
            m_pInteropTexture = 0;
        }

        if (m_workQID) {
            CommandArgs* pCommandArgs = 0;
            HRESULT hr = CreateCommandArgs(MediaSession::Command_Shutdown, pCommandArgs);
            if (SUCCEEDED(hr))
                MFPutWorkItem(m_workQID, &m_asynccb_OnProcessCommand, pCommandArgs);

            if (pCommandArgs) {
                pCommandArgs->Release();
                pCommandArgs = 0;
            }
        }
        m_lock.Release();
        return S_OK;
    }

    HRESULT MediaSession::Play() {
        m_lock.Acquire();

        HRESULT hr = S_OK;
        switch (m_status.m_outerState) {
        case Status_NotInitialized:
        case Status_Initializing:
            hr = MF_E_INVALIDREQUEST;
            break;
        case Status_Initialized:
        case Status_Starting:
        case Status_Started:
        case Status_Stopped: {
            CommandArgs* pCommandArgs = 0;
            hr = CreateCommandArgs(MediaSession::Command_Play, pCommandArgs);
            if (SUCCEEDED(hr))
                hr = MFPutWorkItem(m_workQID, &m_asynccb_OnProcessCommand, pCommandArgs);

            if (pCommandArgs) {
                pCommandArgs->Release();
                pCommandArgs = 0;
            }

            if (SUCCEEDED(hr))
                m_status.SetOuterState(Status_Stopping);
        } break;
        }

        m_lock.Release();
        return hr;
    }

    HRESULT MediaSession::Pause() {
        m_lock.Acquire();

        HRESULT hr = S_OK;
        switch (m_status.m_outerState) {
        case Status_NotInitialized:
        case Status_Initializing:
            hr = MF_E_INVALIDREQUEST;
            break;
        case Status_Initialized:
        case Status_Starting:
        case Status_Stopping:
        case Status_Stopped: {
            CommandArgs* pCommandArgs = 0;
            hr = CreateCommandArgs(MediaSession::Command_Pause, pCommandArgs);
            if (SUCCEEDED(hr))
                hr = MFPutWorkItem(m_workQID, &m_asynccb_OnProcessCommand, pCommandArgs);

            if (pCommandArgs) {
                pCommandArgs->Release();
                pCommandArgs = 0;
            }

            if (SUCCEEDED(hr))
                m_status.SetOuterState(Status_Started);
        } break;
        }

        m_lock.Release();
        return hr;
    }

    HRESULT MediaSession::SetCurrentPosition(double_t pos) {
        m_lock.Acquire();

        HRESULT hr = S_OK;
        switch (m_status.m_outerState) {
        case Status_NotInitialized:
        case Status_Initializing:
            hr = MF_E_INVALIDREQUEST;
            break;
        case Status_Initialized:
        case Status_Starting:
        case Status_Started:
        case Status_Stopping:
        case Status_Stopped: {
            CommandArgs* pCommandArgs = 0;
            hr = CreateCommandArgs(MediaSession::Command_SetPosition, pCommandArgs);
            if (SUCCEEDED(hr)) {
                hr = pCommandArgs->SetValue((int64_t)prj::floor(pos * 10000000.0));
                if (SUCCEEDED(hr))
                    hr = MFPutWorkItem(m_workQID, &m_asynccb_OnProcessCommand, pCommandArgs);
            }

            if (pCommandArgs) {
                pCommandArgs->Release();
                pCommandArgs = 0;
            }

            if (SUCCEEDED(hr))
                m_status.SetOuterState(m_status.m_outerState);
        } break;
        }

        m_lock.Release();
        return hr;
    }

    Status MediaSession::GetStatus() {
        return m_status.m_outerState;
    }

    double_t MediaSession::GetDuration() {
        return (double_t)m_hnsDuration * 0.0000001;
    }

    double_t MediaSession::GetCurrentPosition() {
        return (double_t)m_pClock->GetTime() * 0.0000001;
    }

    HRESULT MediaSession::GetVolumes(AudioVolumes* out_volumes) {
        if (!out_volumes)
            return E_POINTER;
        else if (m_pAudioRenderer)
            return m_pAudioRenderer->GetVolumes(out_volumes);
        return S_OK;
    }

    HRESULT MediaSession::SetVolumes(const AudioVolumes* in_volumes) {
        if (!in_volumes)
            return E_POINTER;
        else if (m_pAudioRenderer)
            return m_pAudioRenderer->SetVolumes(in_volumes);
        return S_OK;
    }

    HRESULT MediaSession::GetVideoInfo(VideoInfo* out_info) {
        if (!out_info)
            return E_POINTER;

        MoviePlayLib::PlayerStat stats;
        m_rStat.GetStats(stats);

        out_info->present_width = stats.present_width;
        out_info->present_height = stats.present_height;
        out_info->raw_width = stats.raw_width;
        out_info->raw_height = stats.raw_height;
        out_info->framerate = stats.framerate;
        out_info->pixelaspect = stats.pixelaspect;
        return S_OK;
    }

    HRESULT MediaSession::GetTextureD3D9Ex(IDirect3DDevice9* pDevice, IDirect3DTexture9** ppOutTexture) {
        if (!pDevice || !ppOutTexture)
            return E_POINTER;

        *ppOutTexture = 0;

        HRESULT hr = S_FALSE;
        if (!m_pVideoRenderer)
            return hr;

        OnProcessSchedule();

        IMFSample* pSample = 0;
        HRESULT hrSample = m_pVideoRenderer->GetSample(&pSample);
        if (pSample) {
            const int64_t timeBegin = GetTimestamp();

            HANDLE wddmShareHandle = 0;
            IMFMediaBuffer* pMediaBuffer = 0;
            IDirect3DTexture9* pDXTexture = 0;
            D3DSURFACE_DESC desc = {};

            hr = pSample->GetUINT64(WDDM_SHARED_HANDLE, (UINT64*)&wddmShareHandle);
            if (FAILED(hr))
                goto End;

            hr = pSample->GetBufferByIndex(0, &pMediaBuffer);
            if (FAILED(hr))
                goto End;

            hr = pMediaBuffer->QueryInterface(IID_PPV_ARGS(&pDXTexture));
            if (FAILED(hr))
                goto End;

            hr = pDXTexture->GetLevelDesc(0, &desc);
            if (SUCCEEDED(hr))
                hr = pDevice->CreateTexture(desc.Width, desc.Height, 1,
                    desc.Usage, desc.Format, desc.Pool, ppOutTexture, &wddmShareHandle);

        End:
            if (pDXTexture) {
                pDXTexture->Release();
                pDXTexture = 0;
            }

            if (pMediaBuffer) {
                pMediaBuffer->Release();
                pMediaBuffer = 0;
            }

            if (pSample) {
                pSample->Release();
                pSample = 0;
            }

            const int64_t timeEnd = GetTimestamp();

            m_rStat.SetVideoLockElapse(CalcTimeMsec(timeBegin, timeEnd));
        }

        if (SUCCEEDED(hr))
            hr = hrSample;
        return hr;
    }

    HRESULT MediaSession::GetTextureD3D11(ID3D11Device* pDevice, ID3D11Texture2D** ppOutTexture) {
        if (!pDevice || !ppOutTexture)
            return E_POINTER;

        *ppOutTexture = 0;

        HRESULT hr = S_FALSE;
        if (!m_pVideoRenderer)
            return hr;

        OnProcessSchedule();

        IMFSample* pSample = 0;
        HRESULT hrSample = m_pVideoRenderer->GetSample(&pSample);
        if (pSample) {
            const int64_t timeBegin = GetTimestamp();

            HANDLE wddmShareHandle = 0;
            hr = pSample->GetUINT64(WDDM_SHARED_HANDLE, (UINT64*)&wddmShareHandle);
            if (SUCCEEDED(hr))
                hr = pDevice->OpenSharedResource(wddmShareHandle, IID_PPV_ARGS(ppOutTexture));

            if (pSample) {
                pSample->Release();
                pSample = 0;
            }

            const int64_t timeEnd = GetTimestamp();

            m_rStat.SetVideoLockElapse(CalcTimeMsec(timeBegin, timeEnd));
        }

        if (SUCCEEDED(hr))
            hr = hrSample;
        return hr;
    }

    HRESULT MediaSession::GetTextureOGL(IGLDXInteropTexture** ppOutTexture) {
        if (!ppOutTexture)
            return E_POINTER;

        *ppOutTexture = 0;

        HRESULT hr = S_FALSE;
        if (!m_pVideoRenderer)
            return hr;

        OnProcessSchedule();

        if (!m_pInteropTexture) {
            hr = CreateGLDXInteropTexture(m_pInteropTexture);
            if (FAILED(hr))
                return hr;
        }

        IMFSample* pSample = 0;
        HRESULT hrSample = m_pVideoRenderer->GetSample(&pSample);
        if (pSample) {
            const int64_t timeBegin = GetTimestamp();

            hr = m_pInteropTexture->SetSample(pSample);
            if (SUCCEEDED(hr)) {
                *ppOutTexture = m_pInteropTexture;
                m_pInteropTexture->AddRef();
            }

            if (pSample) {
                pSample->Release();
                pSample = 0;
            }

            const int64_t timeEnd = GetTimestamp();

            m_rStat.SetVideoLockElapse(CalcTimeMsec(timeBegin, timeEnd));
        }

        if (SUCCEEDED(hr))
            hr = hrSample;
        return hr;
    }

    void MediaSession::OnProcessSchedule() {
        m_lock.Acquire();
        if (m_bScheduleStart) {
            if (m_status.m_innerState == Status_Stopping) {
                schedule_sample("Audio", m_pAudioDecoder, m_pAudioRenderer, 500000);
                schedule_sample("Video", m_pVideoDecoder, m_pVideoRenderer, 100000);
                check_end_of_stream();
            }
        }

        if (m_rStat.HasError())
            m_status.SetInnerState(Status_NotInitialized);
        m_lock.Release();
    }

    void MediaSession::Scheduler_Stop() {
        m_lock.Acquire();
        m_bScheduleStart = FALSE;
        m_lock.Release();
    }

    void MediaSession::Scheduler_Start() {
        m_lock.Acquire();
        m_bScheduleStart = TRUE;
        m_lock.Release();
    }

    void MediaSession::_on_shutdown() {
        Scheduler_Stop();

        if (m_hIntervalThread) {
            SetEvent(m_hQuitEvent);
            WaitForSingleObject(m_hIntervalThread, INFINITE);
        }

        if (m_pVideoRenderer)
            m_pVideoRenderer->Shutdown();
        if (m_pAudioRenderer)
            m_pAudioRenderer->Shutdown();
        if (m_pVideoDecoder)
            m_pVideoDecoder->Shutdown();
        if (m_pAudioDecoder)
            m_pAudioDecoder->Shutdown();
        if (m_pSource)
            m_pSource->Shutdown();

        if (m_pAudioRenderer) {
            m_pAudioRenderer->Release();
            m_pAudioRenderer = 0;
        }

        if (m_pVideoRenderer) {
            m_pVideoRenderer->Release();
            m_pVideoRenderer = 0;
        }

        if (m_pVideoDecoder) {
            m_pVideoDecoder->Release();
            m_pVideoDecoder = 0;
        }

        if (m_pAudioDecoder) {
            m_pAudioDecoder->Release();
            m_pAudioDecoder = 0;
        }

        if (m_pSource) {
            m_pSource->Release();
            m_pSource = 0;
        }

        m_status.SetInnerState(Status_NotInitialized);
    }

    void MediaSession::check_end_of_stream() {
        uint32_t source_audio_queue_count = 0;
        uint32_t source_video_queue_count = 0;
        uint32_t decode_audio_queue_count = 0;
        uint32_t decode_video_queue_count = 0;

        BOOL stop = FALSE;
        int64_t time = m_pClock->GetTime();

        if (m_pAudioDecoder) {
            source_audio_queue_count = m_pAudioDecoder->GetInputQueueCount();
            decode_audio_queue_count = m_pAudioDecoder->GetOutputQueueCount();
            if (m_pAudioDecoder->IsEndOfStream()
                && (m_hnsAudioPresentEndTime == INT64_MAX || m_hnsAudioPresentEndTime <= time))
                stop = TRUE;
        }

        if (m_pVideoDecoder) {
            source_video_queue_count = m_pVideoDecoder->GetInputQueueCount();
            decode_video_queue_count = m_pVideoDecoder->GetOutputQueueCount();
            if (m_pVideoDecoder->IsEndOfStream()
                && (m_hnsVideoPresentEndTime == INT64_MAX || m_hnsVideoPresentEndTime <= time))
                stop = TRUE;
        }

        if (stop)
            m_status.SetInnerState(Status_Stopped);

        m_rStat.SetSamplesCount(source_audio_queue_count,
            source_video_queue_count, decode_audio_queue_count, decode_video_queue_count);
    }

    HRESULT MediaSession::open(const wchar_t* filePath) {
        MOVIE_PLAY_LIB_TRACE_BEGIN;
        IMFMediaType* pAudioMediaType = 0;
        IMFMediaType* pVideoMediaType = 0;
        IDirect3DDeviceManager9* pDeviceManager = 0;
        IDirect3DDevice9Ex* pDevice = 0;
        HRESULT hr;

        const int64_t time0Begin = GetTimestamp();
        hr = CreateMediaSource(m_rStat, m_pClock, filePath, m_pSource);
        if (SUCCEEDED(hr)) {
            m_pSource->GetAudioMediaType(&pAudioMediaType);
            m_pSource->GetVideoMediaType(&pVideoMediaType);
        }

        if (pVideoMediaType && SUCCEEDED(hr)) {
            IDirect3D9Ex* pDeviceTemp = 0;
            hr = Direct3DCreate9Ex(D3D_SDK_VERSION, &pDeviceTemp);

            D3DPRESENT_PARAMETERS presentParam = {};
            presentParam.BackBufferWidth = 1;
            presentParam.BackBufferHeight = 1;
            presentParam.BackBufferFormat = D3DFMT_UNKNOWN;
            presentParam.Windowed = TRUE;
            presentParam.SwapEffect = D3DSWAPEFFECT_COPY;
            presentParam.Flags = D3DPRESENTFLAG_VIDEO;

            UINT resetToken = 0;
            D3DCAPS9 caps = {};
            if (SUCCEEDED(hr))
                hr = pDeviceTemp->GetDeviceCaps(0, D3DDEVTYPE_HAL, &caps);

            DWORD behaviorFlags;
            if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
                behaviorFlags = D3DCREATE_NOWINDOWCHANGES | D3DCREATE_HARDWARE_VERTEXPROCESSING
                    | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE;
            else
                behaviorFlags = D3DCREATE_NOWINDOWCHANGES | D3DCREATE_SOFTWARE_VERTEXPROCESSING
                    | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE;

            if (SUCCEEDED(hr)) {
                hr = pDeviceTemp->CreateDeviceEx(0, D3DDEVTYPE_HAL, presentParam.hDeviceWindow,
                    behaviorFlags, &presentParam, 0, &pDevice);
                if (SUCCEEDED(hr)) {
                    hr = DXVA2CreateDirect3DDeviceManager9(&resetToken, &pDeviceManager);
                    if (SUCCEEDED(hr))
                        hr = pDeviceManager->ResetDevice(pDevice, resetToken);
                }
            }

            if (pDeviceTemp) {
                pDeviceTemp->Release();
                pDeviceTemp = 0;
            }

            if (SUCCEEDED(hr)) {
                hr = CreateVideoDecoder(m_rStat, m_pClock, m_pSource,
                    pDeviceManager, pDevice, m_pVideoDecoder);
                if (SUCCEEDED(hr)) {
                    hr = m_pSource->SetVideoDecoder(m_pVideoDecoder);
                    if (SUCCEEDED(hr))
                        hr = CreateVideoRenderer(m_pClock, m_pVideoRenderer);
                }
            }
        }

        if (pAudioMediaType && SUCCEEDED(hr)) {
            hr = CreateAudioDecoder(m_rStat, m_pClock, m_pSource, m_pAudioDecoder);
            if (SUCCEEDED(hr)) {
                hr = m_pSource->SetAudioDecoder(m_pAudioDecoder);
                if (SUCCEEDED(hr)) {
                    uint32_t channelCount = 0;
                    uint32_t sampleFrequency = 0;
                    hr = pAudioMediaType->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, &channelCount);
                    if (SUCCEEDED(hr)) {
                        hr = pAudioMediaType->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &sampleFrequency);
                        if (SUCCEEDED(hr))
                            hr = CreateWASAPIRenderer(m_pClock, sampleFrequency, channelCount, m_pAudioRenderer);
                    }
                }
            }
        }

        if ((pVideoMediaType || pAudioMediaType) && SUCCEEDED(hr)) {
            m_hnsDuration = m_pSource->GetDuration();
            m_rStat.SetSessionInfo((double_t)m_hnsDuration * 0.0000001,
                !!m_pAudioDecoder, !!m_pVideoDecoder);
        }

        if (pDevice) {
            pDevice->Release();
            pDevice = 0;
        }

        if (pDeviceManager) {
            pDeviceManager->Release();
            pDeviceManager = 0;
        }

        if (pVideoMediaType) {
            pVideoMediaType->Release();
            pVideoMediaType = 0;
        }

        if (pAudioMediaType) {
            pAudioMediaType->Release();
            pAudioMediaType = 0;
        }
        const int64_t time0End = GetTimestamp();

        const double_t time0 = CalcTimeMsec(time0Begin, time0End);

        const int64_t time1Begin = GetTimestamp();
        if (SUCCEEDED(hr)) {
            hr = m_pClock->Stop();
            if (SUCCEEDED(hr)) {
                hr = m_pClock->SetTime(0);
                if (SUCCEEDED(hr))
                    hr = m_pSource->Start(0i64);
            }

            if (m_pAudioDecoder && SUCCEEDED(hr))
                hr = m_pAudioDecoder->Open();

            if (m_pVideoDecoder && SUCCEEDED(hr))
                hr = m_pVideoDecoder->Open();

            if (m_pAudioRenderer && SUCCEEDED(hr))
                hr = m_pAudioRenderer->Open();

            if (m_pVideoRenderer && SUCCEEDED(hr))
                hr = m_pVideoRenderer->Open();
        }
        const int64_t time1End = GetTimestamp();

        const double_t time1 = CalcTimeMsec(time1Begin, time1End);

        const int64_t time2Begin = GetTimestamp();
        if (SUCCEEDED(hr))
            hr = wait_buffering(5, 5);
        const int64_t time2End = GetTimestamp();

        const double_t time2 = CalcTimeMsec(time2Begin, time2End);

        if (SUCCEEDED(hr))
            m_status.SetInnerState(Status_Initialized);
        else
            m_status.SetInnerState(Status_NotInitialized);
        MOVIE_PLAY_LIB_TRACE_END;
        return hr;
    }

    void MediaSession::schedule_sample(const char* label, IMediaTransform* pDecoder,
        IMediaRenderer* pRenderer, const int64_t preload) {
        if (!pDecoder)
            return;

        int64_t time = m_pClock->GetTime();
        for (int64_t hnsSampleTime = pDecoder->PeekSampleTime();
            hnsSampleTime - preload <= time;
            hnsSampleTime = pDecoder->PeekSampleTime()) {
            IMFSample* pSample = 0;
            pDecoder->GetSample(pSample);
            if (!pSample)
                break;

            int64_t hnsSampleDuration = 0;
            pSample->GetSampleDuration(&hnsSampleDuration);
            if (hnsSampleTime + hnsSampleDuration > time) {
                if (pRenderer)
                    pRenderer->ProcessSample(pSample);

                if (pSample) {
                    pSample->Release();
                    pSample = 0;
                }

                pDecoder->RequestSample();
                if (pDecoder == m_pAudioDecoder) {
                    m_hnsAudioPresentEndTime = hnsSampleTime + hnsSampleDuration;

                    m_rStat.SetScheduleAudioDrift((double_t)(time - hnsSampleTime) * 0.0001);
                }
                else {
                    m_hnsVideoPresentEndTime = hnsSampleTime + hnsSampleDuration;

                    m_rStat.SetScheduleVideoDrift((double_t)(time - hnsSampleTime) * 0.0001);
                }
            }

            if (pSample) {
                pSample->Release();
                pSample = 0;
            }
        }
    }

    HRESULT MediaSession::set_position(int64_t hnsTime) {
        MOVIE_PLAY_LIB_TRACE_BEGIN;
        hnsTime = clamp_def(hnsTime, 0, m_hnsDuration);

        const int64_t timeBegin = GetTimestamp();

        HRESULT hr = m_pClock->Stop();
        if (SUCCEEDED(hr))
            hr = m_pSource->Stop();

        Scheduler_Stop();

        if (m_pAudioDecoder && SUCCEEDED(hr))
            hr = m_pAudioDecoder->Flush();
        if (m_pVideoDecoder && SUCCEEDED(hr))
            hr = m_pVideoDecoder->Flush();
        if (m_pAudioRenderer && SUCCEEDED(hr))
            hr = m_pAudioRenderer->Flush();
        if (m_pVideoRenderer && SUCCEEDED(hr))
            hr = m_pVideoRenderer->Flush();

        if (m_pAudioDecoder && SUCCEEDED(hr))
            hr = m_pAudioDecoder->Close();
        if (m_pVideoDecoder && SUCCEEDED(hr))
            hr = m_pVideoDecoder->Close();
        if (m_pAudioRenderer && SUCCEEDED(hr))
            hr = m_pAudioRenderer->Close();
        if (m_pVideoRenderer && SUCCEEDED(hr))
            hr = m_pVideoRenderer->Close();

        if (SUCCEEDED(hr))
            hr = m_pClock->SetTime(hnsTime);

        if (m_pAudioDecoder && SUCCEEDED(hr))
            hr = m_pAudioDecoder->Open();
        if (m_pVideoDecoder && SUCCEEDED(hr))
            hr = m_pVideoDecoder->Open();
        if (m_pAudioRenderer && SUCCEEDED(hr))
            hr = m_pAudioRenderer->Open();
        if (m_pVideoRenderer && SUCCEEDED(hr))
            hr = m_pVideoRenderer->Open();

        if (FAILED(hr))
            goto End;

        hr = m_pSource->Start(hnsTime);
        if (FAILED(hr))
            goto End;

        hr = wait_buffering(2, 2);
        if (SUCCEEDED(hr)) {
            IMediaTransform* pDecoder = 0;
            if (m_pVideoDecoder && m_pVideoDecoder->GetOutputQueueCount())
                pDecoder = m_pVideoDecoder;
            else if (m_pAudioDecoder && m_pAudioDecoder->GetOutputQueueCount())
                pDecoder = m_pAudioDecoder;

            if (pDecoder)
                hnsTime = pDecoder->PeekSampleTime();

            hr = m_pClock->SetTime(hnsTime);
        }

    End:
        const int64_t timeEnd = GetTimestamp();

        const double_t _time = CalcTimeMsec(timeBegin, timeEnd);

        if (SUCCEEDED(hr) && m_status.m_innerState == Status_Stopping) {
            Scheduler_Start();
            hr = m_pClock->Start();
        }
        MOVIE_PLAY_LIB_TRACE_END;
        return hr;
    }

    HRESULT MediaSession::wait_buffering(const uint32_t audioCount, const uint32_t videoCount) {
        ULONGLONG tick_start = GetTickCount64();
        while (true) {
            BOOL ready = TRUE;
            if (m_pAudioDecoder && !m_pAudioDecoder->IsEndOfStream()
                && m_pAudioDecoder->GetOutputQueueCount() < audioCount)
                ready = FALSE;

            if (m_pVideoDecoder && !m_pVideoDecoder->IsEndOfStream()
                && m_pVideoDecoder->GetOutputQueueCount() < videoCount)
                ready = FALSE;

            if (ready)
                break;

            if (GetTickCount64() - tick_start >= 1000)
                return S_FALSE;

            Sleep(0);
        }
        return S_OK;
    }

    inline void MediaSession::Destroy(MediaSession* ptr) {
        if (!ptr)
            return;

        delete ptr;
    }

    uint32_t __stdcall MediaSession::_thread_proc(MediaSession* media_session) {
        DWORD task_index = 0;
        HANDLE avrt_handle = AvSetMmThreadCharacteristicsW(L"Audio", &task_index);
        if (!avrt_handle)
#pragma warning(suppress: 6031)
            GetLastError();

        HANDLE pHandles[2];
        pHandles[0] = media_session->m_hIntervalTimer;
        pHandles[1] = media_session->m_hQuitEvent;
        while (!WaitForMultipleObjects(2, pHandles, 0, INFINITE))
            media_session->OnProcessSchedule();

        if (avrt_handle)
            AvRevertMmThreadCharacteristics(avrt_handle);
        return 0;
    }

    HRESULT MediaSession::_async_callback_func(IMFAsyncResult* pAsyncResult) {
        return OnProcessCommand(pAsyncResult);
    }
}
