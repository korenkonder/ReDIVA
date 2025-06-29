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
        return ++m_ref;
    }

    ULONG Player::Release() {
        if (!--m_ref)
            Destroy();
        return m_ref;
    }

    HRESULT Player::Open(const wchar_t* filePath) {
        m_lock.Acquire();

        if (m_pSession) {
            m_pSession->Shutdown();

            if (m_pSession) {
                m_pSession->Release();
                m_pSession = 0;
            }
        }

        HRESULT hr = 0;
        MediaSession* pSession = new MediaSession(hr, filePath, m_pClock);
        if (pSession) {
            if (SUCCEEDED(hr)) {
                m_pSession = pSession;
                pSession->AddRef();
            }
            else
                pSession->Shutdown();
            pSession->Release();
        }
        else
            hr = E_OUTOFMEMORY;

        m_lock.Release();
        return hr;
    }

    HRESULT Player::Open(const char* filePath) {
        if (!filePath)
            return E_INVALIDARG;

        int32_t path_len = (int32_t)utf8_length(filePath);
        if (path_len > 0 && path_len < MAX_PATH) {
            wchar_t buf[MAX_PATH];
            int32_t buf_len = MultiByteToWideChar(0, 0, filePath, path_len, buf, sizeof(buf) / sizeof(wchar_t));
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
        m_lock.Acquire();
        if (m_pSession) {
            m_pSession->Shutdown();

            if (m_pSession) {
                m_pSession->Release();
                m_pSession = 0;
            }
        }
        m_lock.Release();
        return S_OK;
    }

    HRESULT Player::SetTimeSource(IMediaClock* pClock) {
        m_lock.Acquire();
        HRESULT hr = S_OK;
        if (!m_pSession) {
            if (m_pClock) {
                m_pClock->Release();
                m_pClock = 0;
            }

            m_pClock = pClock;

            if (pClock)
                pClock->AddRef();
        }
        else
            hr = MF_E_INVALIDREQUEST;
        m_lock.Release();
        return hr;
    }

    Status Player::GetStatus() {
        m_lock.Acquire();
        Status status;
        if (m_pSession)
            status = m_pSession->GetStatus();
        else
            status = Status_NotInitialized;
        m_lock.Release();
        return status;
    }

    HRESULT Player::Play() {
        m_lock.Acquire();
        HRESULT hr;
        if (m_pSession)
            hr = m_pSession->Play();
        else
            hr = MF_E_NOT_INITIALIZED;
        m_lock.Release();
        return hr;
    }

    HRESULT Player::Pause() {
        m_lock.Acquire();
        HRESULT hr;
        if (m_pSession)
            hr = m_pSession->Pause();
        else
            hr = MF_E_NOT_INITIALIZED;
        m_lock.Release();
        return hr;
    }

    double_t Player::GetDuration() {
        m_lock.Acquire();
        double_t duration;
        if (m_pSession)
            duration = m_pSession->GetDuration();
        else
            duration = 0.0;
        m_lock.Release();
        return duration;
    }

    double_t Player::GetCurrentPosition() {
        m_lock.Acquire();
        double_t time;
        if (m_pSession)
            time = m_pSession->GetCurrentPosition();
        else
            time = 0.0;
        m_lock.Release();
        return time;
    }

    HRESULT Player::SetCurrentPosition(double_t pos) {
        m_lock.Acquire();
        HRESULT hr;
        if (m_pSession)
            hr = m_pSession->SetCurrentPosition(pos);
        else
            hr = S_OK;
        m_lock.Release();
        return hr;
    }

    HRESULT Player::GetVolumes(AudioVolumes* out_volumes) {
        m_lock.Acquire();
        HRESULT hr;
        if (m_pSession)
            hr = m_pSession->GetVolumes(out_volumes);
        else
            hr = S_OK;
        m_lock.Release();
        return hr;
    }

    HRESULT Player::SetVolumes(const AudioVolumes* in_volumes) {
        m_lock.Acquire();
        HRESULT hr;
        if (m_pSession)
            hr = m_pSession->SetVolumes(in_volumes);
        else
            hr = S_OK;
        m_lock.Release();
        return hr;
    }

    HRESULT Player::GetVideoInfo(VideoInfo* out_info) {
        m_lock.Acquire();
        HRESULT hr;
        if (m_pSession)
            hr = m_pSession->GetVideoInfo(out_info);
        else
            hr = S_OK;
        m_lock.Release();
        return hr;
    }

    HRESULT Player::GetTextureD3D9Ex(IDirect3DDevice9* pDevice, IDirect3DTexture9** ppOutTexture) {
        m_lock.Acquire();
        HRESULT hr;
        if (m_pSession)
            hr = m_pSession->GetTextureD3D9Ex(pDevice, ppOutTexture);
        else
            hr = S_OK;
        m_lock.Release();
        return hr;
    }

    HRESULT Player::GetTextureD3D11(ID3D11Device* pDevice, ID3D11Texture2D** ppOutTexture) {
        m_lock.Acquire();
        HRESULT hr;
        if (m_pSession)
            hr = m_pSession->GetTextureD3D11(pDevice, ppOutTexture);
        else
            hr = S_OK;
        m_lock.Release();
        return hr;
    }

    HRESULT Player::GetTextureOGL(IGLDXInteropTexture** ppOutTexture) {
        m_lock.Acquire();
        HRESULT hr;
        if (m_pSession)
            hr = m_pSession->GetTextureOGL(ppOutTexture);
        else
            hr = S_OK;
        m_lock.Release();
        return hr;
    }

    Player::Player() : m_ref(), m_lock(), m_pClock(), m_pSession() {
        MOVIE_PLAY_LIB_TRACE_BEGIN;
        MOVIE_PLAY_LIB_TRACE_END;
    }

    Player::~Player() {
        MOVIE_PLAY_LIB_TRACE_BEGIN;

        if (m_pSession) {
            m_pSession->Shutdown();

            if (m_pSession) {
                m_pSession->Release();
                m_pSession = 0;
            }
        }

        if (m_pClock) {
            m_pClock->Release();
            m_pClock = 0;
        }
        MOVIE_PLAY_LIB_TRACE_END;
    }

    HRESULT Player::Create(IPlayer*& pp) {
        Player* p = new Player;
        if (!p)
            return E_OUTOFMEMORY;

        pp = p;
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
