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
    HRESULT GetMFMediaType(IMFTransform* mf_transform, const GUID& sub_type, IMFMediaType*& ptr) {
        DWORD type_index = 0;
        HRESULT hr;
        IMFMediaType* mf_media_type = 0;
        do {
            if (mf_media_type) {
                mf_media_type->Release();
                mf_media_type = 0;
            }

            hr = mf_transform->GetOutputAvailableType(0, type_index, &mf_media_type);
            if (FAILED(hr))
                break;

            GUID _sub_type = {};
            if (SUCCEEDED(mf_media_type->GetGUID(MF_MT_SUBTYPE, &_sub_type)) && _sub_type == sub_type) {
                hr = mf_transform->SetOutputType(0, mf_media_type, 0);
                if (SUCCEEDED(hr)) {
                    ptr = mf_media_type;
                    ptr->AddRef();
                    break;
                }
            }
            type_index++;
        } while (true);

        if (mf_media_type) {
            mf_media_type->Release();
            mf_media_type = 0;
        }
        return hr;
    }

    HRESULT GetMFTransform(IMFMediaType* mf_media_type, const GUID& sub_type, IMFTransform*& ptr) {
        ptr = 0;

        GUID guid_major_type = {};
        GUID guid_format_subtype = {};
        IID* pclsidMFT = 0;
        UINT32 cMFTs = 0;
        IMFTransform* mf_transform = 0;

        HRESULT hr;
        hr = mf_media_type->GetGUID(MF_MT_MAJOR_TYPE, &guid_major_type);
        if (FAILED(hr))
            goto End;

        hr = mf_media_type->GetGUID(MF_MT_SUBTYPE, &guid_format_subtype);
        if (FAILED(hr))
            goto End;

        MFT_REGISTER_TYPE_INFO formats;
        formats.guidMajorType = guid_major_type;
        formats.guidSubtype = guid_format_subtype;
        MFT_REGISTER_TYPE_INFO output_type_info;
        output_type_info.guidMajorType = guid_major_type;
        output_type_info.guidSubtype = sub_type;
        hr = MFTEnum(guid_major_type == MFMediaType_Audio ? MFT_CATEGORY_AUDIO_DECODER
            : MFT_CATEGORY_VIDEO_DECODER, 0, &formats, &output_type_info, 0, &pclsidMFT, &cMFTs);
        if (FAILED(hr))
            goto End;

        if (cMFTs)
            hr = CoCreateInstance(*pclsidMFT, 0, 1, IID_PPV_ARGS(&mf_transform));

        if (FAILED(hr) || !mf_transform)
            goto End;

        hr = mf_transform->SetInputType(0, mf_media_type, 0);
        if (SUCCEEDED(hr)) {
            ptr = mf_transform;
            ptr->AddRef();
        }

    End:
        if (mf_transform) {
            mf_transform->Release();
            mf_transform = 0;
        }

        if (SUCCEEDED(hr) && !ptr)
            hr = MF_E_INVALIDTYPE;

        if (pclsidMFT)
            CoTaskMemFree(pclsidMFT);
        return hr;
    }

    HRESULT MFMediaEventQueryInterface(IMFMediaEvent* mf_media_event, const IID& iid, void** ppvObject) {
        PROPVARIANT pvar = {};
        HRESULT hr = mf_media_event->GetValue(&pvar);
        if (SUCCEEDED(hr) && pvar.vt == VT_UNKNOWN)
            hr = pvar.punkVal->QueryInterface(iid, ppvObject);
        PropVariantClear(&pvar);
        return hr;
    }

    HRESULT MFSampleSetAllocator(IMFSample* mf_sample, IMFAsyncCallback* sample_allocator) {
        IMFTrackedSample* mf_tracked_sample = 0;
        HRESULT hr = mf_sample->QueryInterface(IID_PPV_ARGS(&mf_tracked_sample));
        if (SUCCEEDED(hr))
            hr = mf_tracked_sample->SetAllocator(sample_allocator, 0);

        if (mf_tracked_sample) {
            mf_tracked_sample->Release();
            mf_tracked_sample = 0;
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
