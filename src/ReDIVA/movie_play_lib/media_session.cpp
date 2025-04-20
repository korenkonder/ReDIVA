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
        return ++ref_count;
    }

    ULONG MediaSession::Release() {
        if (!--ref_count)
            Destroy();
        return ref_count;
    }

    MediaSession::MediaSession(HRESULT& hr, const wchar_t* path, IMediaClock* media_clock) : ref_count(), lock(),
        dwQueue(), state(), process(), media_stats_lock(), media_clock(), media_source(), audio_media_transform(),
        video_media_transform(), wasapi_renderer(), video_renderer(), interop_texture(), duration(),
        audio_sample_time(INT64_MAX), video_sample_time(INT64_MAX), hThread(), hTimer(), hEvent(), async_callback(this) {
        MOVIE_PLAY_LIB_PRINT_FUNC_BEGIN;
        mf_media_session_count_increment();

        LARGE_INTEGER due_time;
        CommandArgs* command_args;
        if (FAILED(hr))
            goto End;

        if (media_clock) {
            this->media_clock = media_clock;
            media_clock->AddRef();
        }
        else
            hr = MediaClock::Create(this->media_clock);

        if (FAILED(hr))
            goto End;

        hEvent = CreateEventA(0, TRUE, FALSE, 0);
        if (!hEvent)
            hr = HRESULT_FROM_WIN32(GetLastError());

        if (FAILED(hr))
            goto End;

        hTimer = CreateWaitableTimerA(0, FALSE, 0);
        if (!hTimer)
            hr = HRESULT_FROM_WIN32(GetLastError());

        if (FAILED(hr))
            goto End;

        due_time = {};
        if (!SetWaitableTimer(hTimer, &due_time, 10, 0, 0, FALSE))
            hr = HRESULT_FROM_WIN32(GetLastError());

        if (FAILED(hr))
            goto End;

        hThread = (HANDLE)_beginthreadex(0, 0, (_beginthreadex_proc_type)MediaSession::ThreadMain, this, 0, 0);
        if (!hThread)
            hr = HRESULT_FROM_WIN32(GetLastError());

        if (FAILED(hr))
            goto End;

        hr = MFAllocateWorkQueue(&dwQueue);
        command_args = 0;
        if (SUCCEEDED(hr)) {
            hr = CommandArgs::Create(CommandArgs::Type::Open, command_args);
            if (SUCCEEDED(hr)) {
                hr = command_args->SetPath(path);
                if (SUCCEEDED(hr))
                    hr = MFPutWorkItem(dwQueue, &async_callback, command_args);
            }
        }

        if (command_args) {
            command_args->Release();
            command_args = 0;
        }

    End:
        MOVIE_PLAY_LIB_PRINT_FUNC_END;
    }

    MediaSession::~MediaSession() {
        MOVIE_PLAY_LIB_PRINT_FUNC_BEGIN;
        Shutdown();

        if (hThread) {
            SetEvent(hEvent);
            WaitForSingleObject(hThread, INFINITE);
            CloseHandle(hThread);
            hThread = 0;
        }

        if (hTimer) {
            CloseHandle(hTimer);
            hTimer = 0;
        }

        if (hEvent) {
            CloseHandle(hEvent);
            hEvent = 0;
        }

        if (media_clock) {
            media_clock->Release();
            media_clock = 0;
        }

        MFUnlockWorkQueue(dwQueue);
        dwQueue = 0;

        mf_media_session_count_decrement();
        MOVIE_PLAY_LIB_PRINT_FUNC_END;
    }

    HRESULT MediaSession::AsyncCallback(IMFAsyncResult* mf_async_result) {
        IUnknown* object = 0;
        CommandArgs* command_args = 0;
        HRESULT hr = mf_async_result->GetState(&object);
        if (FAILED(hr))
            goto End;

        hr = object->QueryInterface(IID_PPV_ARGS(&command_args));
        if (FAILED(hr))
            goto End;

        switch (command_args->GetType()) {
        case CommandArgs::Type::Open:
            hr = open(command_args->GetPath());
            break;
        case CommandArgs::Type::SetPosition:
            hr = set_position(command_args->GetPosition());

            state.SetInnerStateAdvance(state.state_inner);
            break;
        case CommandArgs::Type::Play:
            if (wasapi_renderer)
                wasapi_renderer->Open();
            if (video_renderer)
                video_renderer->Open();

            SetProcessEnable();

            Update();

            media_clock->TimeBegin();

            state.SetInnerStateAdvance(State::Play);
            break;
        case CommandArgs::Type::Pause:
            media_clock->TimeEnd();

            SetProcessDisable();

            if (wasapi_renderer)
                wasapi_renderer->Flush();
            if (video_renderer)
                video_renderer->Flush();

            state.SetInnerStateAdvance(State::Pause);
            break;
        case CommandArgs::Type::Close:
            Shutdown();
            break;
        }

    End:
        if (command_args) {
            command_args->Release();
            command_args = 0;
        }

        if (object) {
            object->Release();
            object = 0;
        }

        if (FAILED(hr))
            media_stats_lock.SetCurrError(hr, this);

        return mf_async_result->SetStatus(hr);
    }

    HRESULT MediaSession::Close() {
        lock.Acquire();
        if (interop_texture) {
            interop_texture->Release();
            interop_texture = 0;
        }

        if (dwQueue) {
            CommandArgs* command_args = 0;
            HRESULT hr = CommandArgs::Create(CommandArgs::Type::Close, command_args);
            if (SUCCEEDED(hr))
                MFPutWorkItem(dwQueue, &async_callback, command_args);

            if (command_args) {
                command_args->Release();
                command_args = 0;
            }
        }
        lock.Release();
        return S_OK;
    }

    HRESULT MediaSession::Play() {
        lock.Acquire();

        HRESULT hr = S_OK;
        switch (state.state) {
        case State::None:
        case State::Init:
            hr = MF_E_INVALIDREQUEST;
            break;
        case State::Open:
        case State::Wait:
        case State::Pause:
        case State::Stop: {
            CommandArgs* command_args = 0;
            hr = CommandArgs::Create(CommandArgs::Type::Play, command_args);
            if (SUCCEEDED(hr))
                hr = MFPutWorkItem(dwQueue, &async_callback, command_args);

            if (command_args) {
                command_args->Release();
                command_args = 0;
            }

            if (SUCCEEDED(hr))
                state.SetStateAdvance(State::Play);
        } break;
        }

        lock.Release();
        return hr;
    }

    HRESULT MediaSession::Pause() {
        lock.Acquire();

        HRESULT hr = S_OK;
        switch (state.state) {
        case State::None:
        case State::Init:
            hr = MF_E_INVALIDREQUEST;
            break;
        case State::Open:
        case State::Wait:
        case State::Play:
        case State::Stop: {
            CommandArgs* command_args = 0;
            hr = CommandArgs::Create(CommandArgs::Type::Pause, command_args);
            if (SUCCEEDED(hr))
                hr = MFPutWorkItem(dwQueue, &async_callback, command_args);

            if (command_args) {
                command_args->Release();
                command_args = 0;
            }

            if (SUCCEEDED(hr)) {
                state.lock.Acquire();
                state.state = State::Pause;
                state.command_index_queue++;
                state.lock.Release();
            }
        } break;
        }

        lock.Release();
        return hr;
    }

    HRESULT MediaSession::SetTime(double_t value) {
        lock.Acquire();

        HRESULT hr = S_OK;
        switch (state.state) {
        case State::None:
        case State::Init:
            hr = MF_E_INVALIDREQUEST;
            break;
        case State::Open:
        case State::Wait:
        case State::Pause:
        case State::Play:
        case State::Stop: {
            CommandArgs* command_args = 0;
            hr = CommandArgs::Create(CommandArgs::Type::Pause, command_args);
            if (SUCCEEDED(hr)) {
                hr = command_args->SetPosition((int64_t)prj::floor(value * 10000000.0));
                if (SUCCEEDED(hr))
                    hr = MFPutWorkItem(dwQueue, &async_callback, command_args);
            }

            if (command_args) {
                command_args->Release();
                command_args = 0;
            }

            if (SUCCEEDED(hr))
                state.SetStateAdvance(state.state);
        } break;
        }

        lock.Release();
        return hr;
    }

    State MediaSession::GetState() {
        return state.state;
    }

    double_t MediaSession::GetDuration() {
        return (double_t)duration * 0.0000001;
    }

    double_t MediaSession::GetTime() {
        return (double_t)media_clock->GetTime() * 0.0000001;
    }

    HRESULT MediaSession::GetAudioParams(AudioParams* value) {
        if (!value)
            return E_POINTER;
        else if (wasapi_renderer)
            return wasapi_renderer->GetParams(value);
        return S_OK;
    }

    HRESULT MediaSession::SetAudioParams(const AudioParams* value) {
        if (!value)
            return E_POINTER;
        else if (wasapi_renderer)
            return wasapi_renderer->SetParams(value);
        return S_OK;
    }

    HRESULT MediaSession::GetVideoParams(VideoParams* value) {
        if (!value)
            return E_POINTER;

        MoviePlayLib::MediaStats stats;
        media_stats_lock.GetStats(stats);

        value->width = stats.width;
        value->height = stats.height;
        value->frame_size_width = stats.frame_size_width;
        value->frame_size_height = stats.frame_size_height;
        value->frame_rate = stats.frame_rate;
        value->pixel_aspect_ratio = stats.pixel_aspect_ratio;
        return S_OK;
    }

    HRESULT MediaSession::GetD3D9Texture(IDirect3DDevice9* d3d_device, IDirect3DTexture9** ptr) {
        if (!d3d_device || !ptr)
            return E_POINTER;

        *ptr = 0;

        HRESULT hr = S_FALSE;
        if (!video_renderer)
            return hr;

        Update();

        IMFSample* mf_sample = 0;
        HRESULT mf_sample_hr = video_renderer->GetMFSample(&mf_sample);
        if (mf_sample) {
            const int64_t time_begin = GetTimestamp();

            HANDLE shared_handle = 0;
            IMFMediaBuffer* mf_media_buffer = 0;
            IDirect3DTexture9* dx_texture = 0;
            D3DSURFACE_DESC desc = {};

            hr = mf_sample->GetUINT64(TextureSharedHandleGUID, (UINT64*)&shared_handle);
            if (FAILED(hr))
                goto End;

            hr = mf_sample->GetBufferByIndex(0, &mf_media_buffer);
            if (FAILED(hr))
                goto End;

            hr = mf_media_buffer->QueryInterface(IID_PPV_ARGS(&dx_texture));
            if (FAILED(hr))
                goto End;

            hr = dx_texture->GetLevelDesc(0, &desc);
            if (SUCCEEDED(hr))
                hr = d3d_device->CreateTexture(desc.Width, desc.Height, 1,
                    desc.Usage, desc.Format, desc.Pool, ptr, &shared_handle);

        End:
            if (dx_texture) {
                dx_texture->Release();
                dx_texture = 0;
            }

            if (mf_media_buffer) {
                mf_media_buffer->Release();
                mf_media_buffer = 0;
            }

            if (mf_sample) {
                mf_sample->Release();
                mf_sample = 0;
            }

            const int64_t time_end = GetTimestamp();

            media_stats_lock.SetVideoOutputTime(CalcTimeMsec(time_begin, time_end));
        }

        if (SUCCEEDED(hr))
            hr = mf_sample_hr;
        return hr;
    }

    HRESULT MediaSession::GetD3D11Texture(ID3D11Device* d3d_device, ID3D11Texture2D** ptr) {
        if (!d3d_device || !ptr)
            return E_POINTER;

        *ptr = 0;

        HRESULT hr = S_FALSE;
        if (!video_renderer)
            return hr;

        Update();

        IMFSample* mf_sample = 0;
        HRESULT mf_sample_hr = video_renderer->GetMFSample(&mf_sample);
        if (mf_sample) {
            const int64_t time_begin = GetTimestamp();

            HANDLE shared_handle = 0;
            hr = mf_sample->GetUINT64(TextureSharedHandleGUID, (UINT64*)&shared_handle);
            if (SUCCEEDED(hr))
                hr = d3d_device->OpenSharedResource(shared_handle, IID_PPV_ARGS(ptr));

            if (mf_sample) {
                mf_sample->Release();
                mf_sample = 0;
            }

            const int64_t time_end = GetTimestamp();

            media_stats_lock.SetVideoOutputTime(CalcTimeMsec(time_begin, time_end));
        }

        if (SUCCEEDED(hr))
            hr = mf_sample_hr;
        return hr;
    }

    HRESULT MediaSession::GetGLDXIntreropTexture(IGLDXInteropTexture** ptr) {
        if (!ptr)
            return E_POINTER;

        *ptr = 0;

        HRESULT hr = S_FALSE;
        if (!video_renderer)
            return hr;

        Update();

        if (!interop_texture) {
            hr = GLDXInteropTexture::Create(interop_texture);
            if (FAILED(hr))
                return hr;
        }

        IMFSample* mf_sample = 0;
        HRESULT mf_sample_hr = video_renderer->GetMFSample(&mf_sample);
        if (mf_sample) {
            const int64_t time_begin = GetTimestamp();

            hr = interop_texture->SetMFSample(mf_sample);
            if (SUCCEEDED(hr)) {
                *ptr = interop_texture;
                interop_texture->AddRef();
            }

            if (mf_sample) {
                mf_sample->Release();
                mf_sample = 0;
            }

            const int64_t time_end = GetTimestamp();

            media_stats_lock.SetVideoOutputTime(CalcTimeMsec(time_begin, time_end));
        }

        if (SUCCEEDED(hr))
            hr = mf_sample_hr;
        return hr;
    }

    void MediaSession::GetSamplesCount() {
        uint32_t audio_mf_samples_wait_count = 0;
        uint32_t video_mf_samples_wait_count = 0;
        uint32_t audio_mf_samples_count = 0;
        uint32_t video_mf_samples_count = 0;

        BOOL stop = FALSE;
        int64_t time = media_clock->GetTime();

        if (audio_media_transform) {
            audio_mf_samples_wait_count = audio_media_transform->GetMFSamplesWaitCount();
            audio_mf_samples_count = audio_media_transform->GetMFSamplesCount();
            if (audio_media_transform->CanShutdown()
                && (audio_sample_time == INT64_MAX || audio_sample_time <= time))
                stop = TRUE;
        }

        if (video_media_transform) {
            video_mf_samples_wait_count = video_media_transform->GetMFSamplesWaitCount();
            video_mf_samples_count = video_media_transform->GetMFSamplesCount();
            if (video_media_transform->CanShutdown()
                && (video_sample_time == INT64_MAX || video_sample_time <= time))
                stop = TRUE;
        }

        if (stop)
            state.SetInnerState(State::Stop);

        media_stats_lock.SetMFSamplesCount(audio_mf_samples_wait_count,
            video_mf_samples_wait_count, audio_mf_samples_count, video_mf_samples_count);
    }

    void MediaSession::Process(const char* name, IMediaTransform* media_transform,
        IMediaRenderer* media_renderer, int64_t process_time) {
        if (!media_transform)
            return;

        int64_t time = media_clock->GetTime();
        for (int64_t sample_time = media_transform->GetSampleTime();
            sample_time - process_time <= time;
            sample_time = media_transform->GetSampleTime()) {
            IMFSample* mf_sample = 0;
            media_transform->GetMFSample(mf_sample);
            if (!mf_sample)
                break;

            int64_t sample_duration = 0;
            mf_sample->GetSampleDuration(&sample_duration);
            if (sample_time + sample_duration > time) {
                if (media_renderer)
                    media_renderer->SetMFSample(mf_sample);

                if (mf_sample) {
                    mf_sample->Release();
                    mf_sample = 0;
                }

                media_transform->SignalEvent();
                if (media_transform == audio_media_transform) {
                    audio_sample_time = sample_time + sample_duration;

                    media_stats_lock.SetAudioTimeAdvance((double_t)(time - sample_time) * 0.0001);
                }
                else {
                    video_sample_time = sample_time + sample_duration;

                    media_stats_lock.SetVideoTimeAdvance((double_t)(time - sample_time) * 0.0001);
                }
            }

            if (mf_sample) {
                mf_sample->Release();
                mf_sample = 0;
            }
        }
    }

    void MediaSession::SetProcessDisable() {
        lock.Acquire();
        process = FALSE;
        lock.Release();
    }

    void MediaSession::SetProcessEnable() {
        lock.Acquire();
        process = TRUE;
        lock.Release();
    }

    void MediaSession::Shutdown() {
        SetProcessDisable();

        if (hThread) {
            SetEvent(hEvent);
            WaitForSingleObject(hThread, INFINITE);
        }

        if (video_renderer)
            video_renderer->Shutdown();
        if (wasapi_renderer)
            wasapi_renderer->Shutdown();
        if (video_media_transform)
            video_media_transform->Shutdown();
        if (audio_media_transform)
            audio_media_transform->Shutdown();
        if (media_source)
            media_source->Shutdown();

        if (wasapi_renderer) {
            wasapi_renderer->Release();
            wasapi_renderer = 0;
        }

        if (video_renderer) {
            video_renderer->Release();
            video_renderer = 0;
        }

        if (video_media_transform) {
            video_media_transform->Release();
            video_media_transform = 0;
        }

        if (audio_media_transform) {
            audio_media_transform->Release();
            audio_media_transform = 0;
        }

        if (media_source) {
            media_source->Release();
            media_source = 0;
        }

        state.SetInnerState(State::None);
    }

    void MediaSession::Update() {
        lock.Acquire();
        if (process) {
            if (state.state_inner == State::Play) {
                Process("Audio", audio_media_transform, wasapi_renderer, 500000);
                Process("Video", video_media_transform, video_renderer, 100000);
                GetSamplesCount();
            }
        }

        if (media_stats_lock.HasError())
            state.SetInnerState(State::None);
        lock.Release();
    }

    HRESULT MediaSession::WaitSamplesLoad(uint32_t min_audio_samples_count, uint32_t min_video_samples_count) {
        ULONGLONG tick_start = GetTickCount64();
        while (true) {
            BOOL ready = TRUE;
            if (audio_media_transform && !audio_media_transform->CanShutdown()
                && audio_media_transform->GetMFSamplesCount() < min_audio_samples_count)
                ready = FALSE;

            if (video_media_transform && !video_media_transform->CanShutdown()
                && video_media_transform->GetMFSamplesCount() < min_audio_samples_count)
                ready = FALSE;

            if (ready)
                break;

            if (GetTickCount64() - tick_start >= 1000)
                return S_FALSE;

            Sleep(0);
        }
        return S_OK;
    }

    HRESULT MediaSession::open(const wchar_t* url) {
        MOVIE_PLAY_LIB_PRINT_FUNC_BEGIN;
        IMFMediaType* audio_mf_media_type = 0;
        IMFMediaType* video_mf_media_type = 0;
        IDirect3DDeviceManager9* d3d_device_manager = 0;
        IDirect3DDevice9Ex* d3d_device = 0;
        HRESULT hr;

        const int64_t time0_begin = GetTimestamp();
        hr = MediaSource::Create(&media_stats_lock, media_clock, url, media_source);
        if (SUCCEEDED(hr)) {
            media_source->GetAudioMFMediaType(&audio_mf_media_type);
            media_source->GetVideoMFMediaType(&video_mf_media_type);
        }

        if (video_mf_media_type && SUCCEEDED(hr)) {
            IDirect3D9Ex* d3d_device_temp = 0;
            hr = Direct3DCreate9Ex(D3D_SDK_VERSION, &d3d_device_temp);

            D3DPRESENT_PARAMETERS present_param = {};
            present_param.BackBufferWidth = 1;
            present_param.BackBufferHeight = 1;
            present_param.BackBufferFormat = D3DFMT_UNKNOWN;
            present_param.Windowed = TRUE;
            present_param.SwapEffect = D3DSWAPEFFECT_COPY;
            present_param.Flags = D3DPRESENTFLAG_VIDEO;

            UINT reset_token = 0;
            D3DCAPS9 caps = {};
            if (SUCCEEDED(hr))
                hr = d3d_device_temp->GetDeviceCaps(0, D3DDEVTYPE_HAL, &caps);

            DWORD d3d_device_flags;
            if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
                d3d_device_flags = D3DCREATE_NOWINDOWCHANGES | D3DCREATE_HARDWARE_VERTEXPROCESSING
                    | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE;
            else
                d3d_device_flags = D3DCREATE_NOWINDOWCHANGES | D3DCREATE_SOFTWARE_VERTEXPROCESSING
                    | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE;

            if (SUCCEEDED(hr)) {
                hr = d3d_device_temp->CreateDeviceEx(0, D3DDEVTYPE_HAL, present_param.hDeviceWindow,
                    d3d_device_flags, &present_param, 0, &d3d_device);
                if (SUCCEEDED(hr)) {
                    hr = DXVA2CreateDirect3DDeviceManager9(&reset_token, &d3d_device_manager);
                    if (SUCCEEDED(hr))
                        hr = d3d_device_manager->ResetDevice(d3d_device, reset_token);
                }
            }

            if (d3d_device_temp) {
                d3d_device_temp->Release();
                d3d_device_temp = 0;
            }

            if (SUCCEEDED(hr)) {
                hr = VideoDecoder::Create(&media_stats_lock, media_clock, media_source,
                    d3d_device_manager, d3d_device, video_media_transform);
                if (SUCCEEDED(hr)) {
                    hr = media_source->SetVideoMediaTransform(video_media_transform);
                    if (SUCCEEDED(hr))
                        hr = VideoRenderer::Create(media_clock, video_renderer);
                }
            }
        }

        if (audio_mf_media_type && SUCCEEDED(hr)) {
            hr = AudioDecoder::Create(&media_stats_lock, media_clock, media_source, audio_media_transform);
            if (SUCCEEDED(hr)) {
                hr = media_source->SetAudioMediaTransform(audio_media_transform);
                if (SUCCEEDED(hr)) {
                    uint32_t channels = 0;
                    uint32_t sample_rate = 0;
                    hr = audio_mf_media_type->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, &channels);
                    if (SUCCEEDED(hr)) {
                        hr = audio_mf_media_type->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &sample_rate);
                        if (SUCCEEDED(hr))
                            hr = WASAPIRenderer::Create(media_clock, sample_rate, channels, wasapi_renderer);
                    }
                }
            }
        }

        if ((video_mf_media_type || audio_mf_media_type) && SUCCEEDED(hr)) {
            duration = media_source->GetDuration();
            media_stats_lock.SetSessionInfo((double_t)duration * 0.0000001,
                !!audio_media_transform, !!video_media_transform);
        }

        if (d3d_device) {
            d3d_device->Release();
            d3d_device = 0;
        }

        if (d3d_device_manager) {
            d3d_device_manager->Release();
            d3d_device_manager = 0;
        }

        if (video_mf_media_type) {
            video_mf_media_type->Release();
            video_mf_media_type = 0;
        }

        if (audio_mf_media_type) {
            audio_mf_media_type->Release();
            audio_mf_media_type = 0;
        }
        const int64_t time0_end = GetTimestamp();

        const double_t time0 = CalcTimeMsec(time0_begin, time0_end);

        const int64_t time1_begin = GetTimestamp();
        if (SUCCEEDED(hr)) {
            hr = media_clock->TimeEnd();
            if (SUCCEEDED(hr)) {
                hr = media_clock->SetTime(0);
                if (SUCCEEDED(hr))
                    hr = media_source->Start(0i64);
            }

            if (audio_media_transform && SUCCEEDED(hr))
                hr = audio_media_transform->Open();

            if (video_media_transform && SUCCEEDED(hr))
                hr = video_media_transform->Open();

            if (wasapi_renderer && SUCCEEDED(hr))
                hr = wasapi_renderer->Open();

            if (video_renderer && SUCCEEDED(hr))
                hr = video_renderer->Open();
        }
        const int64_t time1_end = GetTimestamp();

        const double_t time1 = CalcTimeMsec(time1_begin, time1_end);

        const int64_t time2_begin = GetTimestamp();
        if (SUCCEEDED(hr))
            hr = WaitSamplesLoad(5, 5);
        const int64_t time2_end = GetTimestamp();

        const double_t time2 = CalcTimeMsec(time2_begin, time2_end);

        if (SUCCEEDED(hr))
            state.SetInnerState(State::Open);
        else
            state.SetInnerState(State::None);
        MOVIE_PLAY_LIB_PRINT_FUNC_END;
        return hr;
    }

    HRESULT MediaSession::set_position(int64_t time) {
        MOVIE_PLAY_LIB_PRINT_FUNC_BEGIN;
        time = clamp_def(time, 0, duration);

        const int64_t time_begin = GetTimestamp();

        HRESULT hr = media_clock->TimeEnd();
        if (SUCCEEDED(hr))
            hr = media_source->Stop();

        SetProcessDisable();

        if (audio_media_transform && SUCCEEDED(hr))
            hr = audio_media_transform->Flush();
        if (video_media_transform && SUCCEEDED(hr))
            hr = video_media_transform->Flush();
        if (wasapi_renderer && SUCCEEDED(hr))
            hr = wasapi_renderer->Flush();
        if (video_renderer && SUCCEEDED(hr))
            hr = video_renderer->Flush();

        if (audio_media_transform && SUCCEEDED(hr))
            hr = audio_media_transform->Close();
        if (video_media_transform && SUCCEEDED(hr))
            hr = video_media_transform->Close();
        if (wasapi_renderer && SUCCEEDED(hr))
            hr = wasapi_renderer->Close();
        if (video_renderer && SUCCEEDED(hr))
            hr = video_renderer->Close();

        if (SUCCEEDED(hr))
            hr = media_clock->SetTime(time);

        if (audio_media_transform && SUCCEEDED(hr))
            hr = audio_media_transform->Open();
        if (video_media_transform && SUCCEEDED(hr))
            hr = video_media_transform->Open();
        if (wasapi_renderer && SUCCEEDED(hr))
            hr = wasapi_renderer->Open();
        if (video_renderer && SUCCEEDED(hr))
            hr = video_renderer->Open();

        if (FAILED(hr))
            goto End;

        hr = media_source->Start(time);
        if (FAILED(hr))
            goto End;

        hr = WaitSamplesLoad(2, 2);
        if (SUCCEEDED(hr)) {
            IMediaTransform* media_transform = 0;
            if (video_media_transform && video_media_transform->GetMFSamplesCount())
                media_transform = video_media_transform;
            else if (audio_media_transform && audio_media_transform->GetMFSamplesCount())
                media_transform = audio_media_transform;

            if (media_transform)
                time = media_transform->GetSampleTime();

            hr = media_clock->SetTime(time);
        }

    End:
        const int64_t time_end = GetTimestamp();

        const double_t _time = CalcTimeMsec(time_begin, time_end);

        if (SUCCEEDED(hr) && state.state_inner == State::Play) {
            SetProcessEnable();
            hr = media_clock->TimeBegin();
        }
        MOVIE_PLAY_LIB_PRINT_FUNC_END;
        return hr;
    }

    inline void MediaSession::Destroy(MediaSession* ptr) {
        if (!ptr)
            return;

        delete ptr;
    }

    uint32_t __stdcall MediaSession::ThreadMain(MediaSession* media_session) {
        DWORD task_index = 0;
        HANDLE avrt_handle = AvSetMmThreadCharacteristicsW(L"Audio", &task_index);
        if (!avrt_handle)
#pragma warning(suppress: 6031)
            GetLastError();

        HANDLE handles[2];
        handles[0] = media_session->hTimer;
        handles[1] = media_session->hEvent;
        while (!WaitForMultipleObjects(2, handles, 0, INFINITE))
            media_session->Update();

        if (avrt_handle)
            AvRevertMmThreadCharacteristics(avrt_handle);
        return 0;
    }
}
