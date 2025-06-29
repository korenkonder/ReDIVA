/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "../../CRE/texture.hpp"
#include "gl_dx_interop_texture.hpp"
#include <glad/glad_wgl.h>

namespace MoviePlayLib {
    static uint16_t gl_dx_interop_texture_counter = 0;

    HRESULT GLDXInteropTexture::QueryInterface(const IID& riid, void** ppvObject) {
        if (!ppvObject)
            return E_INVALIDARG;

        if (riid == __uuidof(IGLDXInteropTexture) || riid == __uuidof(IUnknown)) {
            *ppvObject = this;
            AddRef();
            return S_OK;
        }

        *ppvObject = 0;
        return E_NOINTERFACE;
    }

    ULONG GLDXInteropTexture::AddRef() {
        return ++m_ref;
    }

    ULONG GLDXInteropTexture::Release() {
        ULONG ref = --m_ref;
        if (ref == 1)
            _unlock_texture();
        else if (!ref) {
            _unlock_texture();
            Destroy();
        }
        return ref;
    }

    texture* GLDXInteropTexture::GetGLTexture() {
        if (m_lockIndex >= 0 && m_lockIndex < m_count)
            return m_textures[m_lockIndex].pGLTex;
        return 0;
    }

    GLDXInteropTexture::GLDXInteropTexture(HRESULT& hr) : m_ref(), m_tid(), m_hGLContext(),
        m_hGLDevice(), m_pDXDevice(), m_token(), m_lockIndex(-1), m_count(), m_textures() {
        MOVIE_PLAY_LIB_TRACE_BEGIN;
        memset(m_textures, 0, sizeof(m_textures));
        if (SUCCEEDED(hr)) {
            if (!GLAD_WGL_NV_DX_interop2)
                hr = HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
            else if (SUCCEEDED(hr)) {
                m_tid = GetCurrentThreadId();
                m_hGLContext = wglGetCurrentContext();
                if (!m_hGLContext)
                    hr = HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);
            }
        }
        MOVIE_PLAY_LIB_TRACE_END;
    }

    GLDXInteropTexture::~GLDXInteropTexture() {
        MOVIE_PLAY_LIB_TRACE_BEGIN;
        MOVIE_PLAY_LIB_TRACE_END;
    }

    HRESULT GLDXInteropTexture::SetSample(IMFSample* pSample) {
        _unlock_texture();

        IMFMediaBuffer* pMediaBuffer = 0;
        IDirect3DTexture9* pDXTexture = 0;
        IDirect3DDevice9* pDXDevice = 0;

        HRESULT hr;
        HANDLE wddmShareHandle;
        uint32_t token;
        uint32_t lockIndex;

        hr = pSample->GetBufferByIndex(0, &pMediaBuffer);
        if (FAILED(hr))
            goto End;

        wddmShareHandle = 0;
        hr = pSample->GetUINT64(WDDM_SHARED_HANDLE, (UINT64*)&wddmShareHandle);
        if (FAILED(hr))
            goto End;

        token = 0;
        hr = pSample->GetUINT32(SAMPLE_RESET_TOKEN, &token);
        if (FAILED(hr))
            goto End;

        hr = pMediaBuffer->QueryInterface(IID_PPV_ARGS(&pDXTexture));
        if (FAILED(hr))
            goto End;

        hr = pDXTexture->GetDevice(&pDXDevice);
        if (FAILED(hr))
            goto End;

        if (m_token != token || m_pDXDevice != pDXDevice) {
            _close_device();

            m_token = token;
            m_pDXDevice = pDXDevice;

            m_hGLDevice = wglDXOpenDeviceNV(m_pDXDevice);

            if (!m_hGLDevice)
                hr = HRESULT_FROM_WIN32(GetLastError());
        }
        if (FAILED(hr))
            goto End;

        for (lockIndex = 0; lockIndex < m_count; lockIndex++)
            if (m_textures[lockIndex].hShareHandle == wddmShareHandle)
                break;

        if (lockIndex == m_count) {
            if (m_count >= 10) {
                hr = E_OUTOFMEMORY;
                goto End;
            }

            if (_open_texture(m_textures[lockIndex], pDXTexture, wddmShareHandle))
                m_count++;
            else {
                hr = HRESULT_FROM_WIN32(GetLastError());
                _close_texture(m_textures[lockIndex]);
            }
        }

        if (FAILED(hr))
            goto End;

        if (lockIndex < m_count && wglDXLockObjectsNV(m_hGLDevice, 1, &m_textures[lockIndex].hGLTexture))
            m_lockIndex = lockIndex;
        else
            hr = HRESULT_FROM_WIN32(GetLastError());

    End:
        if (m_pDXDevice) {
            m_pDXDevice->Release();
            m_pDXDevice = 0;
        }

        if (pDXTexture) {
            pDXTexture->Release();
            pDXTexture = 0;
        }

        if (pMediaBuffer) {
            pMediaBuffer->Release();
            pMediaBuffer = 0;
        }
        return hr;
    }

    inline void GLDXInteropTexture::Destroy(GLDXInteropTexture* ptr) {
        if (!ptr)
            return;

        delete ptr;
    }

    void GLDXInteropTexture::_close_device() {
        if (m_count) {
            for (uint32_t i = 0; i < m_count; i++)
                _close_texture(m_textures[i]);
            m_count = 0;
        }

        if (m_hGLDevice) {
            wglDXCloseDeviceNV(m_hGLDevice);
            m_hGLDevice = 0;
        }
    }

    void GLDXInteropTexture::_close_texture(TEXTURE& tx) {
        if (tx.hGLTexture) {
            wglDXUnregisterObjectNV(m_hGLDevice, tx.hGLTexture);
            tx.hGLTexture = 0;
        }

        if (tx.pGLTex) {
            texture_release(tx.pGLTex);
            tx.pGLTex = 0;
        }
        tx.hShareHandle = 0;
    }

    bool GLDXInteropTexture::_open_texture(TEXTURE& tx, IDirect3DTexture9* pDXTexture, HANDLE wddmShareHandle) {
        tx.pGLTex = texture_load_tex_2d(texture_id(0x24, gl_dx_interop_texture_counter), GL_RGBA8, 1280, 720, 0, 0, 0);
        if (!tx.pGLTex)
            return false;

        gl_dx_interop_texture_counter++;

        glDeleteTextures(1, &tx.pGLTex->glid);
        glGenTextures(1, &tx.pGLTex->glid);

        wglDXSetResourceShareHandleNV(pDXTexture, wddmShareHandle);
        tx.hShareHandle = wddmShareHandle;
        tx.hGLTexture = wglDXRegisterObjectNV(m_hGLDevice, pDXTexture,
            tx.pGLTex->glid, GL_TEXTURE_2D, WGL_ACCESS_READ_ONLY_NV);
        return !!tx.hGLTexture;
    }

    inline void GLDXInteropTexture::_unlock_texture() {
        GetCurrentThreadId();
        wglGetCurrentContext();

        if (m_lockIndex >= 0 && m_lockIndex < m_count) {
            wglDXUnlockObjectsNV(m_hGLDevice, 1, &m_textures[m_lockIndex].hGLTexture);
            m_lockIndex = -1;
        }
    }

    HRESULT CreateGLDXInteropTexture(GLDXInteropTexture*& pp) {
        HRESULT hr = S_OK;
        GLDXInteropTexture* p = new GLDXInteropTexture(hr);
        if (!p)
            return E_OUTOFMEMORY;

        if (SUCCEEDED(hr)) {
            pp = p;
            p->AddRef();
        }
        p->Release();
        return hr;
    }
}
