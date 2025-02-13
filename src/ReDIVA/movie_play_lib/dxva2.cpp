/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "dxva2.hpp"

namespace MoviePlayLib {
    HRESULT GetDirectXVideoProcessor(IDirect3DDeviceManager9* d3d_device_manager,
        const DXVA2_VideoDesc* video_desc, IDirectXVideoProcessor*& ptr) {
        HANDLE output_type_info = 0;
        HRESULT hr;
        hr = d3d_device_manager->OpenDeviceHandle(&output_type_info);
        if (FAILED(hr))
            return hr;

        IDirectXVideoProcessorService* dx_video_processor_service = 0;
        IDirectXVideoProcessor* dx_video_processor = 0;
        UINT guid_count = 0;
        GUID* guids = 0;
        hr = d3d_device_manager->GetVideoService(output_type_info,
            __uuidof(IDirectXVideoProcessorService), (void**)&dx_video_processor_service);
        if (FAILED(hr))
            goto End;

        hr = dx_video_processor_service->GetVideoProcessorDeviceGuids(video_desc, &guid_count, &guids);
        if (FAILED(hr) || !guids)
            goto End;

        for (UINT i = 0; i < guid_count && !dx_video_processor; i++) {
            UINT format_count = 0;
            D3DFORMAT* formats = 0;
            hr = dx_video_processor_service->GetVideoProcessorRenderTargets(
                guids[i], video_desc, &format_count, &formats);
            if (SUCCEEDED(hr) && formats)
                for (UINT j = 0; j < format_count && !dx_video_processor; j++)
                    if (formats[j] == D3DFMT_X8R8G8B8)
                        hr = dx_video_processor_service->CreateVideoProcessor(guids[i],
                            video_desc, D3DFMT_X8R8G8B8, 1, &dx_video_processor);

            if (formats)
                CoTaskMemFree(formats);
        }

        if (dx_video_processor) {
            ptr = dx_video_processor;
            ptr->AddRef();
            goto End;
        }
        else
            hr = MF_E_UNSUPPORTED_D3D_TYPE;

    End:
        if (guids) {
            CoTaskMemFree(guids);
            guids = 0;
        }

        if (dx_video_processor) {
            dx_video_processor->Release();
            dx_video_processor = 0;
        }

        if (dx_video_processor_service) {
            dx_video_processor_service->Release();
            dx_video_processor_service = 0;
        }

        HRESULT _hr = d3d_device_manager->CloseDeviceHandle(output_type_info);
        if (SUCCEEDED(hr))
            hr = _hr;
        return hr;
    }
}
