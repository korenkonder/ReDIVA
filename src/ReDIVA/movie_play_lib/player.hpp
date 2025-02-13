/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "movie_play_lib.hpp"
#include "media_session.hpp"

namespace MoviePlayLib {
    class Player : public IPlayer {
    protected:
        RefCount ref_count;
        Lock lock;
        IMediaClock* media_clock;
        MediaSession* media_session;

    public:
        virtual HRESULT QueryInterface(const IID& riid, void** ppvObject) override;
        virtual ULONG AddRef() override;
        virtual ULONG Release() override;

        virtual HRESULT Open(const wchar_t* path) override;
        virtual HRESULT Open(const char* path) override;
        virtual HRESULT Close() override;
        virtual HRESULT SetMediaClock(IMediaClock* media_clock) override;
        virtual State GetState() override;
        virtual HRESULT Play() override;
        virtual HRESULT Pause() override;
        virtual double_t GetDuration() override;
        virtual double_t GetTime() override;
        virtual HRESULT SetTime(double_t value) override;
        virtual HRESULT GetAudioParams(AudioParams* value) override;
        virtual HRESULT SetAudioParams(const AudioParams* value) override;
        virtual HRESULT GetVideoParams(VideoParams* value) override;
        virtual HRESULT GetD3D9Texture(IDirect3DDevice9* d3d_device, IDirect3DTexture9** ptr) override;
        virtual HRESULT GetD3D11Texture(ID3D11Device* d3d_device, ID3D11Texture2D** ptr) override;
        virtual HRESULT GetGLDXIntreropTexture(IGLDXInteropTexture** value) override;

        Player();
        virtual ~Player();

        static HRESULT Create(IPlayer*& ptr);
        static void Destroy(Player* ptr);

        inline void Destroy() {
            Destroy(this);
        }
    };
}
