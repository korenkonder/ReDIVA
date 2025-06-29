/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "media_foundation.hpp"
#include <mfapi.h>
#include <mfidl.h>
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfuuid.lib")

namespace MoviePlayLib {
    HRESULT CreateDecoder(IMFMediaType* pInputType, const GUID& outputSubType, IMFTransform*& ppOutTransform) {
        ppOutTransform = 0;

        GUID guidMajorType = {};
        GUID guidFormatSubType = {};
        IID* pclsidMFT = 0;
        UINT32 cMFTs = 0;
        IMFTransform* mf_transform = 0;

        HRESULT hr;
        hr = pInputType->GetGUID(MF_MT_MAJOR_TYPE, &guidMajorType);
        if (FAILED(hr))
            goto End;

        hr = pInputType->GetGUID(MF_MT_SUBTYPE, &guidFormatSubType);
        if (FAILED(hr))
            goto End;

        MFT_REGISTER_TYPE_INFO inputType;
        inputType.guidMajorType = guidMajorType;
        inputType.guidSubtype = guidFormatSubType;
        MFT_REGISTER_TYPE_INFO outputType;
        outputType.guidMajorType = guidMajorType;
        outputType.guidSubtype = outputSubType;
        hr = MFTEnum(guidMajorType == MFMediaType_Audio ? MFT_CATEGORY_AUDIO_DECODER
            : MFT_CATEGORY_VIDEO_DECODER, 0, &inputType, &outputType, 0, &pclsidMFT, &cMFTs);
        if (FAILED(hr))
            goto End;

        if (cMFTs)
            hr = CoCreateInstance(*pclsidMFT, 0, 1, IID_PPV_ARGS(&mf_transform));

        if (FAILED(hr) || !mf_transform)
            goto End;

        hr = mf_transform->SetInputType(0, pInputType, 0);
        if (SUCCEEDED(hr)) {
            ppOutTransform = mf_transform;
            mf_transform->AddRef();
        }

    End:
        if (mf_transform) {
            mf_transform->Release();
            mf_transform = 0;
        }

        if (SUCCEEDED(hr) && !ppOutTransform)
            hr = MF_E_INVALIDTYPE;

        if (pclsidMFT)
            CoTaskMemFree(pclsidMFT);
        return hr;
    }

    HRESULT MFMediaEventQueryInterface(IMFMediaEvent* pEvent, const IID& iid, void** ppvObject) {
        PROPVARIANT pvar = {};
        HRESULT hr = pEvent->GetValue(&pvar);
        if (SUCCEEDED(hr) && pvar.vt == VT_UNKNOWN)
            hr = pvar.punkVal->QueryInterface(iid, ppvObject);
        PropVariantClear(&pvar);
        return hr;
    }

    HRESULT MFSampleSetAllocator(IMFSample* pSample, IMFAsyncCallback* pAsyncCallback) {
        IMFTrackedSample* pTrackedSample = 0;
        HRESULT hr = pSample->QueryInterface(IID_PPV_ARGS(&pTrackedSample));
        if (SUCCEEDED(hr))
            hr = pTrackedSample->SetAllocator(pAsyncCallback, 0);

        if (pTrackedSample) {
            pTrackedSample->Release();
            pTrackedSample = 0;
        }
        return hr;
    }

    HRESULT SelectDecoderOutputFormat(IMFTransform* pTransform,
        const GUID& outputSubType, IMFMediaType*& ppOutMediaType) {
        DWORD dwTypeIndex = 0;
        HRESULT hr;
        IMFMediaType* pType = 0;
        do {
            if (pType) {
                pType->Release();
                pType = 0;
            }

            hr = pTransform->GetOutputAvailableType(0, dwTypeIndex, &pType);
            if (FAILED(hr))
                break;

            GUID subType = {};
            if (SUCCEEDED(pType->GetGUID(MF_MT_SUBTYPE, &subType)) && subType == outputSubType) {
                hr = pTransform->SetOutputType(0, pType, 0);
                if (SUCCEEDED(hr)) {
                    ppOutMediaType = pType;
                    pType->AddRef();
                    break;
                }
            }
            dwTypeIndex++;
        } while (true);

        if (pType) {
            pType->Release();
            pType = 0;
        }
        return hr;
    }
}

BOOL mf_init_status = FALSE;
std::atomic_int32_t mf_media_session_count = 0;

void mf_media_session_count_increment() {
    mf_media_session_count++;
}

void mf_media_session_count_decrement() {
    mf_media_session_count--;
}

HRESULT mf_init() {
    if (mf_init_status)
        return S_OK;

    HRESULT hr = MFStartup(0x20070u, 1u);
    if (SUCCEEDED(hr))
        mf_init_status = TRUE;
    return hr;
}

HRESULT mf_shutdown() {
    HRESULT hr = S_OK;
    const int64_t time_begin = MoviePlayLib::GetTimestamp();
    for (int32_t i = 0; i < 500; i++) {
        if (!mf_media_session_count)
            break;

        Sleep(10);
    }

    if (mf_init_status) {
        hr = MFShutdown();
        mf_init_status = FALSE;
    }
    const int64_t time_end = MoviePlayLib::GetTimestamp();

    const double_t time = MoviePlayLib::CalcTimeMsec(time_begin, time_end);
    return hr;
}
