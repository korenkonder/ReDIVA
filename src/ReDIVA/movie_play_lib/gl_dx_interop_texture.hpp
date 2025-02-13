/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "movie_play_lib.hpp"

namespace MoviePlayLib {
    class GLDXInteropTexture : public IGLDXInteropTexture {
    public:
        struct Texture {
            HANDLE share_handle;
            HANDLE object;
            texture* tex;
        };

    protected:
        RefCount ref_count;
        int32_t thread_id;
        HGLRC wgl_ctx;
        HANDLE device;
        IDirect3DDevice9* d3d_device;
        uint32_t video_index;
        uint32_t index;
        uint32_t count;
        Texture textures[10];

    public:
        virtual HRESULT QueryInterface(const IID& riid, void** ppvObject) override;
        virtual ULONG AddRef() override;
        virtual ULONG Release() override;

        virtual texture* GetTexture() override;

        GLDXInteropTexture(HRESULT& result);
        virtual ~GLDXInteropTexture();

        virtual HRESULT SetMFSample(IMFSample* mf_sample);

        void CloseDevice();
        bool LoadTexture(Texture& gl_dx_tex, void* dxObject, HANDLE shareHandle);
        void ReleaseTexture(Texture& gl_dx_tex);

        static HRESULT Create(GLDXInteropTexture*& ptr);
        static void Destroy(GLDXInteropTexture* ptr);

        inline void Destroy() {
            Destroy(this);
        }
    };
}
