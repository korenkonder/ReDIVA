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
            IMFGetService* mf_get_service = this;
            *ppvObject = mf_get_service;
            mf_get_service->AddRef();
            return S_OK;
        }
        else if (riid == __uuidof(IDirect3DTexture9) && d3d_texture) {
            d3d_texture->AddRef();
            *ppvObject = d3d_texture;
            return S_OK;
        }
        else if (riid == __uuidof(IDirect3DSurface9) && d3d_surface) {
            d3d_surface->AddRef();
            *ppvObject = d3d_surface;
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
        return ++ref_count;
    }

    ULONG DXSurfaceBuffer::Release() {
        if (!--ref_count)
            Destroy();
        return ref_count;
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

    DXSurfaceBuffer::DXSurfaceBuffer(HRESULT& hr, IUnknown* object) : ref_count(), d3d_texture(), d3d_surface() {
        hr = object->QueryInterface(IID_PPV_ARGS(&d3d_texture));
        if (SUCCEEDED(hr))
            hr = d3d_texture->GetSurfaceLevel(0, &d3d_surface);
        else {
            hr = object->QueryInterface(IID_PPV_ARGS(&d3d_surface));
            if (SUCCEEDED(hr))
                d3d_surface->GetContainer(IID_PPV_ARGS(&d3d_texture));
        }
    }

    DXSurfaceBuffer::~DXSurfaceBuffer() {
        if (d3d_texture) {
            d3d_texture->Release();
            d3d_texture = 0;
        }

        if (d3d_surface) {
            d3d_surface->Release();
            d3d_surface = 0;
        }
    }

    HRESULT DXSurfaceBuffer::Create(IUnknown* object, IMFMediaBuffer*& ptr) {
        HRESULT hr = S_OK;
        DXSurfaceBuffer* p = new DXSurfaceBuffer(hr, object);
        if (!p)
            return E_OUTOFMEMORY;

        if (SUCCEEDED(hr)) {
            ptr = p;
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
