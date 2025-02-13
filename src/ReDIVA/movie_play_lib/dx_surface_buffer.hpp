/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "movie_play_lib.hpp"
#include <mfidl.h>

namespace MoviePlayLib {
    class DXSurfaceBuffer : public IMFMediaBuffer, public IMFGetService {
    protected:
        RefCount ref_count;
        IDirect3DTexture9* d3d_texture;
        IDirect3DSurface9* d3d_surface;

    public:
        virtual HRESULT QueryInterface(const IID& riid, void** ppvObject) override;
        virtual ULONG AddRef() override;
        virtual ULONG Release() override;

        virtual HRESULT Lock(BYTE** ppbBuffer, DWORD* pcbMaxLength, DWORD* pcbCurrentLength) override;
        virtual HRESULT Unlock() override;
        virtual HRESULT GetCurrentLength(DWORD* pcbCurrentLength) override;
        virtual HRESULT SetCurrentLength(DWORD cbCurrentLength) override;
        virtual HRESULT GetMaxLength(DWORD* pcbMaxLength) override;

        virtual HRESULT GetService(const GUID& guidService, const IID& riid, LPVOID* ppvObject) override;

        DXSurfaceBuffer(HRESULT& hr, IUnknown* object);
        virtual ~DXSurfaceBuffer();

        static HRESULT Create(IUnknown* object, IMFMediaBuffer*& ptr);
        static void Destroy(DXSurfaceBuffer* ptr);

        inline void Destroy() {
            Destroy(this);
        }
    };
}
