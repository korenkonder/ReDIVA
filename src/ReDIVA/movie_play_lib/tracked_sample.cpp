/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "tracked_sample.hpp"
#include <mfapi.h>
#pragma comment(lib, "mfplat.lib")

namespace MoviePlayLib {
    HRESULT TrackedSample::QueryInterface(const IID& riid, void** ppvObject) {
        if (!ppvObject)
            return E_INVALIDARG;

        if (riid == __uuidof(TrackedSample)) {
            *ppvObject = this;
            AddRef();
            return S_OK;
        }
        else if (riid == __uuidof(IMFTrackedSample)) {
            IMFTrackedSample* mf_tracked_sample = this;
            *ppvObject = mf_tracked_sample;
            AddRef();
            return S_OK;
        }
        else if (riid == __uuidof(IMFAsyncResult)) {
            IMFAsyncResult* mf_async_result = this;
            *ppvObject = mf_async_result;
            AddRef();
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

    ULONG TrackedSample::AddRef() {
        return ++ref_count;
    }

    ULONG TrackedSample::Release() {
        if (!--ref_count) {
            if (mf_async_callback) {
                AddRef();

                IMFAsyncCallback* mf_async_callback = this->mf_async_callback;
                this->mf_async_callback = 0;
                mf_async_callback->Invoke(this);
                mf_async_callback->Release();
                return Release();
            }
            Destroy();
        }
        return ref_count;
    }

    HRESULT TrackedSample::GetItem(const IID& guidKey, PROPVARIANT* pValue) {
        return mf_attributes->GetItem(guidKey, pValue);
    }

    HRESULT TrackedSample::GetItemType(const GUID& guidKey, MF_ATTRIBUTE_TYPE* pType) {
        return mf_attributes->GetItemType(guidKey, pType);
    }

    HRESULT TrackedSample::CompareItem(const GUID& guidKey, const PROPVARIANT& Value, BOOL* pbResult) {
        return mf_attributes->CompareItem(guidKey, Value, pbResult);
    }

    HRESULT TrackedSample::Compare(IMFAttributes* pTheirs, MF_ATTRIBUTES_MATCH_TYPE MatchType, BOOL* pbResult) {
        return mf_attributes->Compare(pTheirs, MatchType, pbResult);
    }

    HRESULT TrackedSample::GetUINT32(const GUID& guidKey, UINT32* punValue) {
        return mf_attributes->GetUINT32(guidKey, punValue);
    }

    HRESULT TrackedSample::GetUINT64(const GUID& guidKey, UINT64* punValue) {
        return mf_attributes->GetUINT64(guidKey, punValue);
    }

    HRESULT TrackedSample::GetDouble(const GUID& guidKey, double* pfValue) {
        return mf_attributes->GetDouble(guidKey, pfValue);
    }

    HRESULT TrackedSample::GetGUID(const GUID& guidKey, GUID* pguidValue) {
        return mf_attributes->GetGUID(guidKey, pguidValue);
    }

    HRESULT TrackedSample::GetStringLength(const GUID& guidKey, UINT32* pcchLength) {
        return mf_attributes->GetStringLength(guidKey, pcchLength);
    }

    HRESULT TrackedSample::GetString(const GUID& guidKey, LPWSTR pwszValue, UINT32 cchBufSize, UINT32* pcchLength) {
        return mf_attributes->GetString(guidKey, pwszValue, cchBufSize, pcchLength);
    }

    HRESULT TrackedSample::GetAllocatedString(const GUID& guidKey, LPWSTR* ppwszValue, UINT32* pcchLength) {
        return mf_attributes->GetAllocatedString(guidKey, ppwszValue, pcchLength);
    }

    HRESULT TrackedSample::GetBlobSize(const GUID& guidKey, UINT32* pcbBlobSize) {
        return mf_attributes->GetBlobSize(guidKey, pcbBlobSize);
    }

    HRESULT TrackedSample::GetBlob(const GUID& guidKey, UINT8* pBuf, UINT32 cbBufSize, UINT32* pcbBlobSize) {
        return mf_attributes->GetBlob(guidKey, pBuf, cbBufSize, pcbBlobSize);
    }

    HRESULT TrackedSample::GetAllocatedBlob(const GUID& guidKey, UINT8** ppBuf, UINT32* pcbSize) {
        return mf_attributes->GetAllocatedBlob(guidKey, ppBuf, pcbSize);
    }

    HRESULT TrackedSample::GetUnknown(const GUID& guidKey, const IID& riid, LPVOID* ppv) {
        return mf_attributes->GetUnknown(guidKey, riid, ppv);
    }

    HRESULT TrackedSample::SetItem(const GUID& guidKey, const PROPVARIANT& Value) {
        return mf_attributes->SetItem(guidKey, Value);
    }

    HRESULT TrackedSample::DeleteItem(const GUID& guidKey) {
        return mf_attributes->DeleteItem(guidKey);
    }

    HRESULT TrackedSample::DeleteAllItems() {
        return mf_attributes->DeleteAllItems();
    }

    HRESULT TrackedSample::SetUINT32(const GUID& guidKey, UINT32 unValue) {
        return mf_attributes->SetUINT32(guidKey, unValue);
    }

    HRESULT TrackedSample::SetUINT64(const GUID& guidKey, UINT64 unValue) {
        return mf_attributes->SetUINT64(guidKey, unValue);
    }

    HRESULT TrackedSample::SetDouble(const GUID& guidKey, double fValue) {
        return mf_attributes->SetDouble(guidKey, fValue);
    }

    HRESULT TrackedSample::SetGUID(const GUID& guidKey, const GUID& guidValue) {
        return mf_attributes->SetGUID(guidKey, guidValue);
    }

    HRESULT TrackedSample::SetString(const GUID& guidKey, LPCWSTR wszValue) {
        return mf_attributes->SetString(guidKey, wszValue);
    }

    HRESULT TrackedSample::SetBlob(const GUID& guidKey, const UINT8* pBuf, UINT32 cbBufSize) {
        return mf_attributes->SetBlob(guidKey, pBuf, cbBufSize);
    }

    HRESULT TrackedSample::SetUnknown(const GUID& guidKey, IUnknown* pUnknown) {
        return mf_attributes->SetUnknown(guidKey, pUnknown);
    }

    HRESULT TrackedSample::LockStore() {
        return mf_attributes->LockStore();
    }

    HRESULT TrackedSample::UnlockStore() {
        return mf_attributes->UnlockStore();
    }

    HRESULT TrackedSample::GetCount(UINT32* pcItems) {
        return mf_attributes->GetCount(pcItems);
    }

    HRESULT TrackedSample::GetItemByIndex(UINT32 unIndex, GUID* pguidKey, PROPVARIANT* pValue) {
        return mf_attributes->GetItemByIndex(unIndex, pguidKey, pValue);
    }

    HRESULT TrackedSample::CopyAllItems(IMFAttributes* pDest) {
        return mf_attributes->CopyAllItems(pDest);
    }

    HRESULT TrackedSample::GetSampleFlags(DWORD* pdwSampleFlags) {
        *pdwSampleFlags = sample_flags;
        return S_OK;
    }

    HRESULT TrackedSample::SetSampleFlags(DWORD dwSampleFlags) {
        sample_flags = dwSampleFlags;
        return S_OK;
    }

    HRESULT TrackedSample::GetSampleTime(LONGLONG* phnsSampleTime) {
        *phnsSampleTime = sample_time;
        return S_OK;
    }

    HRESULT TrackedSample::SetSampleTime(LONGLONG hnsSampleTime) {
        sample_time = hnsSampleTime;
        return S_OK;
    }

    HRESULT TrackedSample::GetSampleDuration(LONGLONG* phnsSampleDuration) {
        *phnsSampleDuration = sample_duration;
        return S_OK;
    }

    HRESULT TrackedSample::SetSampleDuration(LONGLONG hnsSampleDuration) {
        sample_duration = hnsSampleDuration;
        return S_OK;
    }

    HRESULT TrackedSample::GetBufferCount(DWORD* pdwBufferCount) {
        *pdwBufferCount = 1;
        return S_OK;
    }

    HRESULT TrackedSample::GetBufferByIndex(DWORD dwIndex, IMFMediaBuffer** ppBuffer) {
        if (dwIndex != 0)
            return E_INVALIDARG;

        *ppBuffer = mf_media_buffer;
        mf_media_buffer->AddRef();
        return S_OK;
    }

    HRESULT TrackedSample::ConvertToContiguousBuffer(IMFMediaBuffer** ppBuffer) {
        return E_NOTIMPL;
    }

    HRESULT TrackedSample::AddBuffer(IMFMediaBuffer* pBuffer) {
        return E_NOTIMPL;
    }

    HRESULT TrackedSample::RemoveBufferByIndex(DWORD dwIndex) {
        return E_NOTIMPL;
    }

    HRESULT TrackedSample::RemoveAllBuffers() {
        return E_NOTIMPL;
    }

    HRESULT TrackedSample::GetTotalLength(DWORD* pcbTotalLength) {
        return E_NOTIMPL;
    }

    HRESULT TrackedSample::CopyToBuffer(IMFMediaBuffer* pBuffer) {
        return E_NOTIMPL;
    }

    HRESULT TrackedSample::SetAllocator(IMFAsyncCallback* pSampleAllocator, IUnknown* pUnkState) {
        if (mf_async_callback) {
            mf_async_callback->Release();
            mf_async_callback = 0;
        }

        if (pSampleAllocator) {
            mf_async_callback = pSampleAllocator;
            pSampleAllocator->AddRef();
        }
        return S_OK;
    }

    HRESULT TrackedSample::GetState(IUnknown** ppunkState) {
        return E_NOTIMPL;
    }

    HRESULT TrackedSample::GetStatus() {
        return E_NOTIMPL;
    }

    HRESULT TrackedSample::SetStatus(HRESULT hrStatus) {
        return E_NOTIMPL;
    }

    HRESULT TrackedSample::GetObject(IUnknown** ppObject) {
        return E_NOTIMPL;
    }

    IUnknown* TrackedSample::GetStateNoAddRef() {
        return 0;
    }

    TrackedSample::TrackedSample(HRESULT& hr, IMFMediaBuffer* mf_media_buffer) : ref_count(), mf_attributes(),
        mf_media_buffer(), mf_async_callback(), sample_time(), sample_duration(), sample_flags() {
        hr = MFCreateAttributes(&mf_attributes, 1);
        if (SUCCEEDED(hr)) {
            this->mf_media_buffer = mf_media_buffer;
            mf_media_buffer->AddRef();
        }
    }

    TrackedSample::~TrackedSample() {
        if (mf_attributes) {
            mf_attributes->Release();
            mf_attributes = 0;
        }

        if (mf_media_buffer) {
            mf_media_buffer->Release();
            mf_media_buffer = 0;
        }

        if (mf_async_callback) {
            mf_async_callback->Release();
            mf_async_callback = 0;
        }
    }

    HRESULT TrackedSample::Create(IMFMediaBuffer* mf_media_buffer, uint32_t video_index, IMFSample*& ptr) {
        HRESULT hr = S_OK;
        TrackedSample* p = new TrackedSample(hr, mf_media_buffer);
        if (!p)
            return E_OUTOFMEMORY;

        if (SUCCEEDED(hr)) {
            hr = p->SetUINT32(VideoIndexGUID, video_index);
            if (SUCCEEDED(hr)) {
                ptr = p;
                p->AddRef();
            }
        }

        p->Release();
        return S_OK;
    }

    inline void TrackedSample::Destroy(TrackedSample* ptr) {
        if (!ptr)
            return;

        delete ptr;
    }
}
