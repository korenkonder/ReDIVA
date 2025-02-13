/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "movie_play_lib.hpp"
#include "gl_dx_interop_texture.hpp"
#include "video_renderer.hpp"
#include "wasapi_renderer.hpp"

namespace MoviePlayLib {
    struct MediaSessionState {
        Lock lock;
        std::atomic<State> state;
        std::atomic<State> state_inner;
        int32_t command_index_queue;
        int32_t command_index;

        inline MediaSessionState() : lock(), state(State::Init),
            state_inner(State::Init), command_index_queue(), command_index() {

        }

        inline void SetStateAdvance(State state) {
            lock.Acquire();
            this->state = state;
            command_index_queue++;
            lock.Release();
        }

        inline void SetInnerState(State state) {
            lock.Acquire();
            state_inner = state;
            if (command_index == command_index_queue)
                this->state = state;
            lock.Release();
        }

        inline void SetInnerStateAdvance(State state) {
            lock.Acquire();
            state_inner = state;
            if (++command_index == command_index_queue)
                this->state = state;
            lock.Release();
        }
    };

    class MediaSession : public IUnknown {
    protected:
        RefCount ref_count;
        Lock lock;
        DWORD dwQueue;
        MediaSessionState state;
        BOOL process;
        MediaStatsLock media_stats_lock;
        IMediaClock* media_clock;
        IMediaSource* media_source;
        IMediaTransform* audio_media_transform;
        IMediaTransform* video_media_transform;
        WASAPIRenderer* wasapi_renderer;
        VideoRenderer* video_renderer;
        GLDXInteropTexture* interop_texture;
        int64_t duration;
        int64_t audio_sample_time;
        int64_t video_sample_time;
        HANDLE hThread;
        HANDLE hTimer;
        HANDLE hEvent;
        AsyncCallback<MediaSession> async_callback;

    public:
        virtual HRESULT QueryInterface(const IID& riid, void** ppvObject) override;
        virtual ULONG AddRef() override;
        virtual ULONG Release() override;

        MediaSession(HRESULT& hr, const wchar_t* path, IMediaClock* media_clock = 0);
        virtual ~MediaSession();

        virtual HRESULT AsyncCallback(IMFAsyncResult* mf_async_result);
        virtual HRESULT Close();
        virtual HRESULT Play();
        virtual HRESULT Pause();
        virtual HRESULT SetTime(double_t value);
        virtual State GetState();
        virtual double_t GetDuration();
        virtual double_t GetTime();
        virtual HRESULT GetAudioParams(AudioParams* value);
        virtual HRESULT SetAudioParams(const AudioParams* value);
        virtual HRESULT GetVideoParams(VideoParams* value);
        virtual HRESULT GetD3D9Texture(IDirect3DDevice9* d3d_device, IDirect3DTexture9** ptr);
        virtual HRESULT GetD3D11Texture(ID3D11Device* d3d_device, ID3D11Texture2D** ptr);
        virtual HRESULT GetGLDXIntreropTexture(IGLDXInteropTexture** ptr);

        void GetSamplesCount();
        void Process(const char* name, IMediaTransform* media_transform,
            IMediaRenderer* media_renderer, int64_t process_time);
        void SetProcessDisable();
        void SetProcessEnable();
        void Shutdown();
        void Update();
        HRESULT WaitSamplesLoad(uint32_t min_audio_samples_count, uint32_t min_video_samples_count);

        HRESULT open(const wchar_t* url);
        HRESULT set_position(int64_t time);

        static void Destroy(MediaSession* ptr);
        static uint32_t __stdcall ThreadMain(MediaSession* media_session);

        inline void Destroy() {
            Destroy(this);
        }
    };
}
