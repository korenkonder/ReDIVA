/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include <unknwn.h>
#include <mferror.h>
#include <mfobjects.h>
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#include <atomic>

struct texture;

#define MOVIE_PLAY_LIB_PRINT_FUNC_BEGIN MoviePlayLib::PrintDebug(L">%S(%p)\n", __FUNCTION__, this)
#define MOVIE_PLAY_LIB_PRINT_FUNC_END   MoviePlayLib::PrintDebug(L"<%S(%p)\n", __FUNCTION__, this)

namespace MoviePlayLib {
    enum class State {
        None = 0x0,
        Init,
        Open,
        Wait,
        Pause,
        Play,
        Stop,
        Max,
    };

    struct AudioParams {
        float_t spk_l_volume;
        float_t spk_r_volume;
        float_t field_8;
        float_t field_C;
        float_t hph_l_volume;
        float_t hph_r_volume;
    };

    struct Lock : SRWLOCK {
        inline Lock() {
            InitializeSRWLock(this);
        }

        inline void Acquire() {
#pragma warning(suppress: 26110)
            AcquireSRWLockExclusive(this);
        }

        inline void Release() {
#pragma warning(suppress: 26110)
            ReleaseSRWLockExclusive(this);
        }
    };

    struct MediaStats {
        double_t duration;
        bool audio_present;
        bool video_present;
        uint32_t width;
        uint32_t height;
        uint32_t frame_size_width;
        uint32_t frame_size_height;
        float_t frame_rate;
        float_t pixel_aspect_ratio;
        uint32_t sample_rate;
        uint32_t channels;
        HRESULT curr_error;
        IUnknown* curr_object;
        uint32_t audio_mf_samples_wait_count;
        uint32_t video_mf_samples_wait_count;
        uint32_t audio_mf_samples_count;
        uint32_t video_mf_samples_count;
        double_t audio_sample_time;
        double_t video_sample_time;
        double_t audio_process_sample_time;
        double_t video_process_sample_time;
        double_t audio_process_output_time;
        double_t video_process_output_time;
        double_t video_process_blt_time;
        double_t audio_time_advance;
        double_t video_time_advance;
        double_t audio_output_time;
        double_t video_output_time;
    };

    struct MediaStatsLock {
    protected:
        Lock lock;
        __int64 field_8;
        MediaStats stats;

    public:
        inline MediaStatsLock() : field_8(), stats() {

        }

        void GetStats(MediaStats& value);
        bool HasError();
        void SetAudioParams(uint32_t sample_rate, uint32_t channels);
        void SetAudioProcessOutput(double_t output_time, double_t sample_time);
        void SetAudioSampleTime(double_t value);
        void SetAudioTimeAdvance(double_t value);
        void SetCurrError(HRESULT hr, IUnknown* object);
        void SetMFSamplesCount(
            uint32_t audio_mf_samples_wait_count, uint32_t video_mf_samples_wait_count,
            uint32_t audio_mf_samples_count, uint32_t video_mf_samples_count);
        void SetSessionInfo(double_t duration, bool audio_present, bool video_present);
        void SetVideoOutputTime(double_t value);
        void SetVideoParams(uint32_t width, uint32_t height, uint32_t frame_size_width,
            uint32_t frame_size_height, float_t frame_rate, float_t pixel_aspect_ratio);
        void SetVideoProcessOutput(double_t output_time, double_t blt_time, double_t sample_time);
        void SetVideoSampleTime(double_t value);
        void SetVideoTimeAdvance(double_t value);
    };

    struct RefCount : std::atomic<ULONG> {
        RefCount() : std::atomic<ULONG>(1) {

        }
    };

    struct VideoParams {
        int32_t width;
        int32_t height;
        int32_t frame_size_width;
        int32_t frame_size_height;
        float_t frame_rate;
        float_t pixel_aspect_ratio;
    };

    class DECLSPEC_UUID("BE82EF2E-4B9F-4470-9C6F-F78E002FCDEF")
        IMediaRenderer : public IUnknown {
    public:
        virtual HRESULT Shutdown() = 0;
        virtual HRESULT Close() = 0;
        virtual HRESULT Flush() = 0;
        virtual HRESULT Open() = 0;
        virtual HRESULT SetMFSample(IMFSample* mf_sample) = 0;
    };

    class DECLSPEC_UUID("4A5C73B9-FBFE-4A00-A298-FD3FAB0C5AF1")
        IAudioRenderer : public IMediaRenderer {
    public:
        virtual HRESULT GetParams(AudioParams* value) = 0;
        virtual HRESULT SetParams(const AudioParams* value) = 0;
    };

    class DECLSPEC_UUID("D19AC022-EA49-4B6F-9D51-673C1C15ECE8")
        IGLDXInteropTexture : public IUnknown {
    public:
        virtual texture* GetTexture() = 0;
    };

    class DECLSPEC_UUID("D4FA34F6-D03C-4F21-8591-6858161CA5D7")
        IMediaClock : public IUnknown {
    public:
        virtual HRESULT TimeEnd() = 0;
        virtual HRESULT TimeBegin() = 0;
        virtual HRESULT SetTime(INT64 value) = 0;
        virtual INT64 GetTime() = 0;
    };

    class DECLSPEC_UUID("0D41578B-4514-407D-B654-870BCCDC630A")
        IMediaTransform : public IUnknown {
    public:
        virtual HRESULT Shutdown() = 0;
        virtual HRESULT Close() = 0;
        virtual HRESULT Flush() = 0;
        virtual HRESULT Open() = 0;
        virtual HRESULT GetMFMediaType(IMFMediaType** mf_media_type) = 0;
        virtual void SendMFSample(IMFSample* mf_sample) = 0;
        virtual BOOL SignalEvent() = 0;
        virtual BOOL CanShutdown() = 0;
        virtual UINT32 GetMFSamplesWaitCount() = 0;
        virtual UINT32 GetMFSamplesCount() = 0;
        virtual INT64 GetSampleTime() = 0;
        virtual void GetMFSample(IMFSample*& mf_sample) = 0;
    };

    class DECLSPEC_UUID("0D41578B-4514-407D-B654-870BCCDC630A")
        IMediaSource : public IUnknown {
    public:
        virtual HRESULT Shutdown() = 0;
        virtual HRESULT Stop() = 0;
        virtual HRESULT Start(INT64 time) = 0;
        virtual UINT64 GetDuration() = 0;
        virtual HRESULT GetAudioMFMediaType(IMFMediaType** ptr) = 0;
        virtual HRESULT GetVideoMFMediaType(IMFMediaType** ptr) = 0;
        virtual HRESULT SetAudioMediaTransform(IMediaTransform* media_transform) = 0;
        virtual HRESULT SetVideoMediaTransform(IMediaTransform* media_transform) = 0;
    };

    class DECLSPEC_UUID("12AC5FCE-C816-47FE-84B4-4915D6B42FA7")
        IPlayer : public IUnknown {
    public:
        virtual HRESULT Open(const wchar_t* path) = 0;
        virtual HRESULT Open(const char* path) = 0;
        virtual HRESULT Close() = 0;
        virtual HRESULT SetMediaClock(IMediaClock* media_clock) = 0;
        virtual State GetState() = 0;
        virtual HRESULT Play() = 0;
        virtual HRESULT Pause() = 0;
        virtual double_t GetDuration() = 0;
        virtual double_t GetTime() = 0;
        virtual HRESULT SetTime(double_t value) = 0;
        virtual HRESULT GetAudioParams(AudioParams* value) = 0;
        virtual HRESULT SetAudioParams(const AudioParams* value) = 0;
        virtual HRESULT GetVideoParams(VideoParams* value) = 0;
        virtual HRESULT GetD3D9Texture(IDirect3DDevice9* d3d_device, IDirect3DTexture9** ptr) = 0;
        virtual HRESULT GetD3D11Texture(ID3D11Device* d3d_device, ID3D11Texture2D** ptr) = 0;
        virtual HRESULT GetGLDXIntreropTexture(IGLDXInteropTexture** value) = 0;
    };

    template<typename T>
    class AsyncCallback : public IMFAsyncCallback {
    public:
        HRESULT(*func)(T* ptr, IMFAsyncResult* mf_async_result);
        T* data;

        virtual HRESULT QueryInterface(const IID& riid, void** ppvObject) override {
            if (!ppvObject)
                return E_INVALIDARG;

            if (riid == __uuidof(IUnknown) || riid == __uuidof(IMFAsyncCallback)) {
                *ppvObject = this;
                AddRef();
                return S_OK;
            }

            *ppvObject = 0;
            return E_NOINTERFACE;
        }

        virtual ULONG AddRef() override {
            return data->AddRef();
        }

        virtual ULONG Release() override {
            return data->Release();
        }

        virtual HRESULT GetParameters(DWORD* pdwFlags, DWORD* pdwQueue) override {
            return E_NOTIMPL;
        }

        virtual HRESULT Invoke(IMFAsyncResult* pAsyncResult) override {
            return func(data, pAsyncResult);
        }

        AsyncCallback(T* data) : func(Callback), data(data) {

        }

        static HRESULT Callback(T* data, IMFAsyncResult* mf_async_result) {
            return data->AsyncCallback(mf_async_result);
        }
    };

    inline double_t CalcTimeMsec(int64_t begin, int64_t end) {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        return (double_t)(end - begin) / (double_t)freq.QuadPart * 1000.0;
    }

    inline int64_t GetTimestamp() {
        LARGE_INTEGER timestamp = {};
        QueryPerformanceCounter(&timestamp);
        return timestamp.QuadPart;
    }

    extern const GUID TextureSharedHandleGUID;
    extern const GUID VideoIndexGUID;

    extern void PrintDebug(const wchar_t* fmt, ...);
}
