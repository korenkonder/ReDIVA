/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "dxva2.hpp"

namespace MoviePlayLib {
    HRESULT CreateVideoProcessor(IDirect3DDeviceManager9* pDeviceManager,
        const DXVA2_VideoDesc& videoDesc, IDirectXVideoProcessor*& ppOutProcessor) {
        HANDLE hDevice = 0;
        HRESULT hr;
        hr = pDeviceManager->OpenDeviceHandle(&hDevice);
        if (FAILED(hr))
            return hr;

        IDirectXVideoProcessorService* pProcessorService = 0;
        IDirectXVideoProcessor* pProcessor = 0;
        UINT guidCount = 0;
        GUID* guids = 0;
        hr = pDeviceManager->GetVideoService(hDevice, IID_PPV_ARGS(&pProcessorService));
        if (FAILED(hr))
            goto End;

        hr = pProcessorService->GetVideoProcessorDeviceGuids(&videoDesc, &guidCount, &guids);
        if (FAILED(hr) || !guids)
            goto End;

        for (UINT i = 0; i < guidCount && !pProcessor; i++) {
            UINT formatCount = 0;
            D3DFORMAT* formats = 0;
            hr = pProcessorService->GetVideoProcessorRenderTargets(
                guids[i], &videoDesc, &formatCount, &formats);
            if (SUCCEEDED(hr) && formats)
                for (UINT j = 0; j < formatCount && !pProcessor; j++)
                    if (formats[j] == D3DFMT_X8R8G8B8)
                        hr = pProcessorService->CreateVideoProcessor(guids[i],
                            &videoDesc, D3DFMT_X8R8G8B8, 1, &pProcessor);

            if (formats)
                CoTaskMemFree(formats);
        }

        if (pProcessor) {
            ppOutProcessor = pProcessor;
            pProcessor->AddRef();
            goto End;
        }
        else
            hr = MF_E_UNSUPPORTED_D3D_TYPE;

    End:
        if (guids) {
            CoTaskMemFree(guids);
            guids = 0;
        }

        if (pProcessor) {
            pProcessor->Release();
            pProcessor = 0;
        }

        if (pProcessorService) {
            pProcessorService->Release();
            pProcessorService = 0;
        }

        HRESULT _hr = pDeviceManager->CloseDeviceHandle(hDevice);
        if (SUCCEEDED(hr))
            hr = _hr;
        return hr;
    }
}
