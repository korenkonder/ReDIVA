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
        RefCount m_ref;
        SlimLock m_lock;
        IMediaClock* m_pClock;
        MediaSession* m_pSession;

    public:
        virtual HRESULT QueryInterface(const IID& riid, void** ppvObject) override;
        virtual ULONG AddRef() override;
        virtual ULONG Release() override;

        virtual HRESULT Open(const wchar_t* filePath) override;
        virtual HRESULT Open(const char* filePath) override;
        virtual HRESULT Close() override;
        virtual HRESULT SetTimeSource(IMediaClock* pClock) override;
        virtual Status GetStatus() override;
        virtual HRESULT Play() override;
        virtual HRESULT Pause() override;
        virtual double_t GetDuration() override;
        virtual double_t GetCurrentPosition() override;
        virtual HRESULT SetCurrentPosition(double_t pos) override;
        virtual HRESULT GetVolumes(AudioVolumes* out_volumes) override;
        virtual HRESULT SetVolumes(const AudioVolumes* in_volumes) override;
        virtual HRESULT GetVideoInfo(VideoInfo* out_info) override;
        virtual HRESULT GetTextureD3D9Ex(IDirect3DDevice9* pDevice, IDirect3DTexture9** ppOutTexture) override;
        virtual HRESULT GetTextureD3D11(ID3D11Device* pDevice, ID3D11Texture2D** ppOutTexture) override;
        virtual HRESULT GetTextureOGL(IGLDXInteropTexture** ppOutTexture) override;

        Player();
        virtual ~Player();

        static HRESULT Create(IPlayer*& pp);
        static void Destroy(Player* ptr);

        inline void Destroy() {
            Destroy(this);
        }
    };
}
