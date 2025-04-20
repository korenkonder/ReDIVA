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
        return ++ref_count;
    }

    ULONG GLDXInteropTexture::Release() {
        ULONG ref_count = --this->ref_count;
        if (ref_count == 1)
            UnlockCurrentTexture();
        else if (!ref_count) {
            UnlockCurrentTexture();
            Destroy();
        }
        return ref_count;
    }

    texture* GLDXInteropTexture::GetTexture() {
        if (index >= 0 && index < count)
            return textures[index].tex;
        return 0;
    }

    GLDXInteropTexture::GLDXInteropTexture(HRESULT& hr) : ref_count(), thread_id(), wgl_ctx(),
        device(), d3d_device(), video_index(), index(-1), count(), textures() {
        MOVIE_PLAY_LIB_PRINT_FUNC_BEGIN;
        memset(textures, 0, sizeof(textures));
        if (SUCCEEDED(hr)) {
            if (!GLAD_WGL_NV_DX_interop2)
                hr = HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
            else if (SUCCEEDED(hr)) {
                thread_id = GetCurrentThreadId();
                wgl_ctx = wglGetCurrentContext();
                if (!wgl_ctx)
                    hr = HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);
            }
        }
        MOVIE_PLAY_LIB_PRINT_FUNC_END;
    }

    GLDXInteropTexture::~GLDXInteropTexture() {
        MOVIE_PLAY_LIB_PRINT_FUNC_BEGIN;
        MOVIE_PLAY_LIB_PRINT_FUNC_END;
    }

    HRESULT GLDXInteropTexture::SetMFSample(IMFSample* mf_sample) {
        UnlockCurrentTexture();

        IMFMediaBuffer* mf_media_buffer = 0;
        IDirect3DTexture9* d3d_texture = 0;
        IDirect3DDevice9* d3d_device = 0;

        HRESULT hr;
        HANDLE share_handle;
        uint32_t video_index;
        uint32_t index;

        hr = mf_sample->GetBufferByIndex(0, &mf_media_buffer);
        if (FAILED(hr))
            goto End;

        share_handle = 0;
        hr = mf_sample->GetUINT64(TextureSharedHandleGUID, (UINT64*)&share_handle);
        if (FAILED(hr))
            goto End;

        video_index = 0;
        hr = mf_sample->GetUINT32(VideoIndexGUID, &video_index);
        if (FAILED(hr))
            goto End;

        hr = mf_media_buffer->QueryInterface(IID_PPV_ARGS(&d3d_texture));
        if (FAILED(hr))
            goto End;

        hr = d3d_texture->GetDevice(&d3d_device);
        if (FAILED(hr))
            goto End;

        if (this->video_index != video_index || this->d3d_device != d3d_device) {
            CloseDevice();

            this->video_index = video_index;
            this->d3d_device = d3d_device;

            device = wglDXOpenDeviceNV(d3d_device);

            if (!device)
                hr = HRESULT_FROM_WIN32(GetLastError());
        }
        if (FAILED(hr))
            goto End;

        for (index = 0; index < count; index++)
            if (textures[index].share_handle == share_handle)
                break;

        if (index == count) {
            if (count >= 10) {
                hr = E_OUTOFMEMORY;
                goto End;
            }

            if (LoadTexture(textures[index], d3d_texture, share_handle))
                count++;
            else {
                hr = HRESULT_FROM_WIN32(GetLastError());
                ReleaseTexture(textures[index]);
            }
        }

        if (FAILED(hr))
            goto End;

        if (index < count && wglDXLockObjectsNV(device, 1, &textures[index].object))
            this->index = index;
        else
            hr = HRESULT_FROM_WIN32(GetLastError());

    End:
        if (d3d_device) {
            d3d_device->Release();
            d3d_device = 0;
        }

        if (d3d_texture) {
            d3d_texture->Release();
            d3d_texture = 0;
        }

        if (mf_media_buffer) {
            mf_media_buffer->Release();
            mf_media_buffer = 0;
        }
        return hr;
    }

    void GLDXInteropTexture::CloseDevice() {
        if (count) {
            for (uint32_t i = 0; i < count; i++)
                ReleaseTexture(textures[i]);
            count = 0;
        }

        if (device) {
            wglDXCloseDeviceNV(device);
            device = 0;
        }
    }

    bool GLDXInteropTexture::LoadTexture(Texture& gl_dx_tex, void* dxObject, HANDLE share_handle) {
        gl_dx_tex.tex = texture_load_tex_2d(texture_id(0x24, gl_dx_interop_texture_counter), GL_RGBA8, 1280, 720, 0, 0, 0);
        if (!gl_dx_tex.tex)
            return false;

        gl_dx_interop_texture_counter++;

        glDeleteTextures(1, &gl_dx_tex.tex->glid);
        glGenTextures(1, &gl_dx_tex.tex->glid);

        wglDXSetResourceShareHandleNV(dxObject, share_handle);
        gl_dx_tex.share_handle = share_handle;
        gl_dx_tex.object = wglDXRegisterObjectNV(device, dxObject,
            gl_dx_tex.tex->glid, GL_TEXTURE_2D, WGL_ACCESS_READ_ONLY_NV);
        return !!gl_dx_tex.object;
    }

    void GLDXInteropTexture::ReleaseTexture(Texture& gl_dx_tex) {
        if (gl_dx_tex.object) {
            wglDXUnregisterObjectNV(device, gl_dx_tex.object);
            gl_dx_tex.object = 0;
        }

        if (gl_dx_tex.tex) {
            texture_release(gl_dx_tex.tex);
            gl_dx_tex.tex = 0;
        }
        gl_dx_tex.share_handle = 0;
    }

    inline void GLDXInteropTexture::UnlockCurrentTexture() {
        GetCurrentThreadId();
        wglGetCurrentContext();

        if (index >= 0 && index < count) {
            wglDXUnlockObjectsNV(device, 1, &textures[index].object);
            index = -1;
        }
    }

    HRESULT GLDXInteropTexture::Create(GLDXInteropTexture*& ptr) {
        HRESULT hr = S_OK;
        GLDXInteropTexture* p = new GLDXInteropTexture(hr);
        if (!p)
            return E_OUTOFMEMORY;

        if (SUCCEEDED(hr)) {
            ptr = p;
            p->AddRef();
        }
        p->Release();
        return hr;
    }

    inline void GLDXInteropTexture::Destroy(GLDXInteropTexture* ptr) {
        if (!ptr)
            return;

        delete ptr;
    }
}
