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
    struct StatusValue {
        SlimLock m_lock;
        std::atomic<Status> m_outerState;
        std::atomic<Status> m_innerState;
        int32_t m_outerToken;
        int32_t m_innerToken;

        inline StatusValue() : m_lock(), m_outerState(Status_Initializing),
            m_innerState(Status_Initializing), m_outerToken(), m_innerToken() {

        }

        inline void SetOuterState(Status state) {
            m_lock.Acquire();
            m_outerState = state;
            m_outerToken++;
            m_lock.Release();
        }

        inline void SetInnerState(Status state) {
            m_lock.Acquire();
            m_innerState = state;
            if (m_innerToken == m_outerToken)
                m_outerState = state;
            m_lock.Release();
        }

        inline void SetInnerStateAdvance(Status state) {
            m_lock.Acquire();
            m_innerState = state;
            if (++m_innerToken == m_outerToken)
                m_outerState = state;
            m_lock.Release();
        }
    };

    class MediaSession : public IUnknown {
    public:
        enum Command {
            Command_Open = 0,
            Command_SetPosition,
            Command_Play,
            Command_Pause,
            Command_Shutdown,
        };

    protected:
        RefCount m_ref;
        SlimLock m_lock;
        DWORD m_workQID;
        StatusValue m_status;
        BOOL m_bScheduleStart;
        PlayerStat_ m_rStat;
        IMediaClock* m_pClock;
        IMediaSource* m_pSource;
        IMediaTransform* m_pAudioDecoder;
        IMediaTransform* m_pVideoDecoder;
        IAudioRenderer* m_pAudioRenderer;
        VideoRenderer* m_pVideoRenderer;
        GLDXInteropTexture* m_pInteropTexture;
        int64_t m_hnsDuration;
        int64_t m_hnsAudioPresentEndTime;
        int64_t m_hnsVideoPresentEndTime;
        HANDLE m_hIntervalThread;
        HANDLE m_hIntervalTimer;
        HANDLE m_hQuitEvent;
        AsyncCallback<MediaSession> m_asynccb_OnProcessCommand;

    public:
        virtual HRESULT QueryInterface(const IID& riid, void** ppvObject) override;
        virtual ULONG AddRef() override;
        virtual ULONG Release() override;

        MediaSession(HRESULT& hr, const wchar_t* filePath, IMediaClock* media_clock = 0);
        virtual ~MediaSession();

        virtual HRESULT OnProcessCommand(IMFAsyncResult* pAsyncResult);
        virtual HRESULT Shutdown();
        virtual HRESULT Play();
        virtual HRESULT Pause();
        virtual HRESULT SetCurrentPosition(double_t pos);
        virtual Status GetStatus();
        virtual double_t GetDuration();
        virtual double_t GetCurrentPosition();
        virtual HRESULT GetVolumes(AudioVolumes* out_volumes);
        virtual HRESULT SetVolumes(const AudioVolumes* in_volumes);
        virtual HRESULT GetVideoInfo(VideoInfo* out_info);
        virtual HRESULT GetTextureD3D9Ex(IDirect3DDevice9* pDevice, IDirect3DTexture9** ppOutTexture);
        virtual HRESULT GetTextureD3D11(ID3D11Device* pDevice, ID3D11Texture2D** ppOutTexture);
        virtual HRESULT GetTextureOGL(IGLDXInteropTexture** ppOutTexture);

        void OnProcessSchedule();
        void Scheduler_Stop();
        void Scheduler_Start();

        void _on_shutdown();
        void check_end_of_stream();
        HRESULT open(const wchar_t* filePath);
        void schedule_sample(const char* label, IMediaTransform* pDecoder,
            IMediaRenderer* pRenderer, const int64_t preload);
        HRESULT set_position(int64_t hnsTime);
        HRESULT wait_buffering(const uint32_t audioCount, const uint32_t videoCount);

        static void Destroy(MediaSession* ptr);

        inline void Destroy() {
            Destroy(this);
        }

    protected:
        virtual HRESULT _async_callback_func(IMFAsyncResult* pAsyncResult);

        static uint32_t __stdcall _thread_proc(MediaSession* media_session);
    };
}
