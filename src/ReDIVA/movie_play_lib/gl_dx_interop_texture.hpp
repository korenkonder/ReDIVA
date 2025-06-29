/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "movie_play_lib.hpp"

namespace MoviePlayLib {
    class GLDXInteropTexture : public IGLDXInteropTexture {
    public:
        struct TEXTURE {
            HANDLE hShareHandle;
            HANDLE hGLTexture;
            texture* pGLTex;
        };

    protected:
        RefCount m_ref;
        int32_t m_tid;
        HGLRC m_hGLContext;
        HANDLE m_hGLDevice;
        IDirect3DDevice9* m_pDXDevice;
        uint32_t m_token;
        uint32_t m_lockIndex;
        uint32_t m_count;
        TEXTURE m_textures[10];

    public:
        virtual HRESULT QueryInterface(const IID& riid, void** ppvObject) override;
        virtual ULONG AddRef() override;
        virtual ULONG Release() override;

        virtual texture* GetGLTexture() override;

        GLDXInteropTexture(HRESULT& hr);
        virtual ~GLDXInteropTexture();

        virtual HRESULT SetSample(IMFSample* pSample);

        static void Destroy(GLDXInteropTexture* ptr);

        inline void Destroy() {
            Destroy(this);
        }

    protected: 
        void _close_device();
        void _close_texture(TEXTURE& tx);
        bool _open_texture(TEXTURE& tx, IDirect3DTexture9* pDXTexture, HANDLE wddmShareHandle);
        void _unlock_texture();
    };

    extern HRESULT CreateGLDXInteropTexture(GLDXInteropTexture*& pp);
}
