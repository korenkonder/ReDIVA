/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "player.hpp"

namespace MoviePlayLib {
    HRESULT Player::QueryInterface(const IID& riid, void** ppvObject) {
        if (!ppvObject)
            return E_INVALIDARG;

        if (riid == __uuidof(IPlayer) || riid == __uuidof(IUnknown)) {
            *ppvObject = this;
            AddRef();
            return S_OK;
        }

        *ppvObject = 0;
        return E_NOINTERFACE;
    }

    ULONG Player::AddRef() {
        return ++ref_count;
    }

    ULONG Player::Release() {
        if (!--ref_count)
            Destroy();
        return ref_count;
    }

    HRESULT Player::Open(const wchar_t* path) {
        lock.Acquire();

        if (media_session) {
            media_session->Close();

            if (media_session) {
                media_session->Release();
                media_session = 0;
            }
        }

        HRESULT hr = 0;
        MediaSession* media_session = new MediaSession(hr, path, media_clock);
        if (media_session) {
            if (SUCCEEDED(hr)) {
                this->media_session = media_session;
                media_session->AddRef();
            }
            else
                media_session->Close();
            media_session->Release();
        }
        else
            hr = E_OUTOFMEMORY;

        lock.Release();
        return hr;
    }

    HRESULT Player::Open(const char* path) {
        if (!path)
            return E_INVALIDARG;

        int32_t path_len = (int32_t)utf8_length(path);
        if (path_len > 0 && path_len < MAX_PATH) {
            wchar_t buf[MAX_PATH];
            int32_t buf_len = MultiByteToWideChar(0, 0, path, path_len, buf, sizeof(buf) / sizeof(wchar_t));
            if (buf_len > 0 && buf_len < MAX_PATH) {
                if (buf_len >= MAX_PATH)
                    __debugbreak();
                buf[buf_len] = 0;
                return Open(buf);
            }
        }
        return E_INVALIDARG;
    }

    HRESULT Player::Close() {
        lock.Acquire();
        if (media_session) {
            media_session->Close();

            if (media_session) {
                media_session->Release();
                media_session = 0;
            }
        }
        lock.Release();
        return S_OK;
    }

    HRESULT Player::SetMediaClock(IMediaClock* media_clock) {
        lock.Acquire();
        HRESULT hr = S_OK;
        if (!media_session) {
            if (this->media_clock) {
                this->media_clock->Release();
                this->media_clock = 0;
            }

            this->media_clock = media_clock;

            if (media_clock)
                media_clock->AddRef();
        }
        else
            hr = MF_E_INVALIDREQUEST;
        lock.Release();
        return hr;
    }

    State Player::GetState() {
        lock.Acquire();
        State state;
        if (media_session)
            state = media_session->GetState();
        else
            state = State::None;
        lock.Release();
        return state;
    }

    HRESULT Player::Play() {
        lock.Acquire();
        HRESULT hr;
        if (media_session)
            hr = media_session->Play();
        else
            hr = MF_E_NOT_INITIALIZED;
        lock.Release();
        return hr;
    }

    HRESULT Player::Pause() {
        lock.Acquire();
        HRESULT hr;
        if (media_session)
            hr = media_session->Pause();
        else
            hr = MF_E_NOT_INITIALIZED;
        lock.Release();
        return hr;
    }

    double_t Player::GetDuration() {
        lock.Acquire();
        double_t duration;
        if (media_session)
            duration = media_session->GetDuration();
        else
            duration = 0.0;
        lock.Release();
        return duration;
    }

    double_t Player::GetTime() {
        lock.Acquire();
        double_t time;
        if (media_session)
            time = media_session->GetTime();
        else
            time = 0.0;
        lock.Release();
        return time;
    }

    HRESULT Player::SetTime(double_t value) {
        lock.Acquire();
        HRESULT hr;
        if (media_session)
            hr = media_session->SetTime(value);
        else
            hr = S_OK;
        lock.Release();
        return hr;
    }

    HRESULT Player::GetAudioParams(AudioParams* value) {
        lock.Acquire();
        HRESULT hr;
        if (media_session)
            hr = media_session->GetAudioParams(value);
        else
            hr = S_OK;
        lock.Release();
        return hr;
    }

    HRESULT Player::SetAudioParams(const AudioParams* value) {
        lock.Acquire();
        HRESULT hr;
        if (media_session)
            hr = media_session->SetAudioParams(value);
        else
            hr = S_OK;
        lock.Release();
        return hr;
    }

    HRESULT Player::GetVideoParams(VideoParams* value) {
        lock.Acquire();
        HRESULT hr;
        if (media_session)
            hr = media_session->GetVideoParams(value);
        else
            hr = S_OK;
        lock.Release();
        return hr;
    }

    HRESULT Player::GetD3D9Texture(IDirect3DDevice9* d3d_device, IDirect3DTexture9** ptr) {
        lock.Acquire();
        HRESULT hr;
        if (media_session)
            hr = media_session->GetD3D9Texture(d3d_device, ptr);
        else
            hr = S_OK;
        lock.Release();
        return hr;
    }

    HRESULT Player::GetD3D11Texture(ID3D11Device* d3d_device, ID3D11Texture2D** ptr) {
        lock.Acquire();
        HRESULT hr;
        if (media_session)
            hr = media_session->GetD3D11Texture(d3d_device, ptr);
        else
            hr = S_OK;
        lock.Release();
        return hr;
    }

    HRESULT Player::GetGLDXIntreropTexture(IGLDXInteropTexture** value) {
        lock.Acquire();
        HRESULT hr;
        if (media_session)
            hr = media_session->GetGLDXIntreropTexture(value);
        else
            hr = S_OK;
        lock.Release();
        return hr;
    }

    Player::Player() : ref_count(), lock(), media_clock(), media_session() {
        MOVIE_PLAY_LIB_PRINT_FUNC_BEGIN;
        MOVIE_PLAY_LIB_PRINT_FUNC_END;
    }

    Player::~Player() {
        MOVIE_PLAY_LIB_PRINT_FUNC_BEGIN;

        if (media_session) {
            media_session->Close();

            if (media_session) {
                media_session->Release();
                media_session = 0;
            }
        }

        if (media_clock) {
            media_clock->Release();
            media_clock = 0;
        }
        MOVIE_PLAY_LIB_PRINT_FUNC_END;
    }

    HRESULT Player::Create(IPlayer*& ptr) {
        Player* p = new Player;
        if (!p)
            return E_OUTOFMEMORY;

        ptr = p;
        p->AddRef();
        p->Release();
        return S_OK;
    }

    inline void Player::Destroy(Player* ptr) {
        if (!ptr)
            return;

        delete ptr;
    }
}
