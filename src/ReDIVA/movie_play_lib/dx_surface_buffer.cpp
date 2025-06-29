/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "dx_surface_buffer.hpp"
#include <mfapi.h>
#pragma comment(lib, "strmiids.lib")

namespace MoviePlayLib {
    HRESULT DXSurfaceBuffer::QueryInterface(const IID& riid, void** ppvObject) {
        if (!ppvObject)
            return E_INVALIDARG;

        if (riid == __uuidof(IMFMediaBuffer)) {
            *ppvObject = this;
            AddRef();
            return S_OK;
        }
        else if (riid == __uuidof(IMFGetService)) {
            IMFGetService* pService = this;
            *ppvObject = pService;
            pService->AddRef();
            return S_OK;
        }
        else if (riid == __uuidof(IDirect3DTexture9) && m_pTexture) {
            m_pTexture->AddRef();
            *ppvObject = m_pTexture;
            return S_OK;
        }
        else if (riid == __uuidof(IDirect3DSurface9) && m_pSurface) {
            m_pSurface->AddRef();
            *ppvObject = m_pSurface;
            return S_OK;
        }
        else if (riid == __uuidof(IUnknown)) {
            *ppvObject = this;
            AddRef();
            return S_OK;
        }

        *ppvObject = 0;
        return E_NOINTERFACE;
    }

    ULONG DXSurfaceBuffer::AddRef() {
        return ++m_ref;
    }

    ULONG DXSurfaceBuffer::Release() {
        if (!--m_ref)
            Destroy();
        return m_ref;
    }

    HRESULT DXSurfaceBuffer::Lock(BYTE** ppbBuffer, DWORD* pcbMaxLength, DWORD* pcbCurrentLength) {
        return E_NOTIMPL;
    }

    HRESULT DXSurfaceBuffer::Unlock() {
        return E_NOTIMPL;
    }

    HRESULT DXSurfaceBuffer::GetCurrentLength(DWORD* pcbCurrentLength) {
        return E_NOTIMPL;
    }

    HRESULT DXSurfaceBuffer::SetCurrentLength(DWORD cbCurrentLength) {
        return E_NOTIMPL;
    }

    HRESULT DXSurfaceBuffer::GetMaxLength(DWORD* pcbMaxLength) {
        return E_NOTIMPL;
    }

    HRESULT DXSurfaceBuffer::GetService(const GUID& guidService, const IID& riid, LPVOID* ppvObject) {
        if (!ppvObject)
            return E_INVALIDARG;

        if (guidService == MR_BUFFER_SERVICE)
            return QueryInterface(riid, ppvObject);
        return MF_E_UNSUPPORTED_SERVICE;
    }

    DXSurfaceBuffer::DXSurfaceBuffer(HRESULT& hr, IDirect3DTexture9* pTex) : m_ref(), m_pTexture(), m_pSurface() {
        hr = pTex->QueryInterface(IID_PPV_ARGS(&m_pTexture));
        if (SUCCEEDED(hr))
            hr = m_pTexture->GetSurfaceLevel(0, &m_pSurface);
        else {
            hr = pTex->QueryInterface(IID_PPV_ARGS(&m_pSurface));
            if (SUCCEEDED(hr))
                m_pSurface->GetContainer(IID_PPV_ARGS(&m_pTexture));
        }
    }

    DXSurfaceBuffer::~DXSurfaceBuffer() {
        if (m_pTexture) {
            m_pTexture->Release();
            m_pTexture = 0;
        }

        if (m_pSurface) {
            m_pSurface->Release();
            m_pSurface = 0;
        }
    }

    HRESULT CreateDXSurfaceBuffer(IDirect3DTexture9* pTex, IMFMediaBuffer*& pp) {
        HRESULT hr = S_OK;
        DXSurfaceBuffer* p = new DXSurfaceBuffer(hr, pTex);
        if (!p)
            return E_OUTOFMEMORY;

        if (SUCCEEDED(hr)) {
            pp = p;
            p->AddRef();
        }
        p->Release();
        return S_OK;
    }

    inline void DXSurfaceBuffer::Destroy(DXSurfaceBuffer* ptr) {
        if (!ptr)
            return;

        delete ptr;
    }
}
