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
            IMFTrackedSample* pTrackedSample = this;
            *ppvObject = pTrackedSample;
            AddRef();
            return S_OK;
        }
        else if (riid == __uuidof(IMFAsyncResult)) {
            IMFAsyncResult* pAsyncResult = this;
            *ppvObject = pAsyncResult;
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
        return ++m_ref;
    }

    ULONG TrackedSample::Release() {
        if (!--m_ref) {
            if (m_pCallback) {
                AddRef();

                IMFAsyncCallback* m_pCallback = this->m_pCallback;
                this->m_pCallback = 0;
                m_pCallback->Invoke(this);
                m_pCallback->Release();
                return Release();
            }
            Destroy();
        }
        return m_ref;
    }

    HRESULT TrackedSample::GetItem(const IID& guidKey, PROPVARIANT* pValue) {
        return m_pAttributes->GetItem(guidKey, pValue);
    }

    HRESULT TrackedSample::GetItemType(const GUID& guidKey, MF_ATTRIBUTE_TYPE* pType) {
        return m_pAttributes->GetItemType(guidKey, pType);
    }

    HRESULT TrackedSample::CompareItem(const GUID& guidKey, const PROPVARIANT& Value, BOOL* pbResult) {
        return m_pAttributes->CompareItem(guidKey, Value, pbResult);
    }

    HRESULT TrackedSample::Compare(IMFAttributes* pTheirs, MF_ATTRIBUTES_MATCH_TYPE MatchType, BOOL* pbResult) {
        return m_pAttributes->Compare(pTheirs, MatchType, pbResult);
    }

    HRESULT TrackedSample::GetUINT32(const GUID& guidKey, UINT32* punValue) {
        return m_pAttributes->GetUINT32(guidKey, punValue);
    }

    HRESULT TrackedSample::GetUINT64(const GUID& guidKey, UINT64* punValue) {
        return m_pAttributes->GetUINT64(guidKey, punValue);
    }

    HRESULT TrackedSample::GetDouble(const GUID& guidKey, double* pfValue) {
        return m_pAttributes->GetDouble(guidKey, pfValue);
    }

    HRESULT TrackedSample::GetGUID(const GUID& guidKey, GUID* pguidValue) {
        return m_pAttributes->GetGUID(guidKey, pguidValue);
    }

    HRESULT TrackedSample::GetStringLength(const GUID& guidKey, UINT32* pcchLength) {
        return m_pAttributes->GetStringLength(guidKey, pcchLength);
    }

    HRESULT TrackedSample::GetString(const GUID& guidKey, LPWSTR pwszValue, UINT32 cchBufSize, UINT32* pcchLength) {
        return m_pAttributes->GetString(guidKey, pwszValue, cchBufSize, pcchLength);
    }

    HRESULT TrackedSample::GetAllocatedString(const GUID& guidKey, LPWSTR* ppwszValue, UINT32* pcchLength) {
        return m_pAttributes->GetAllocatedString(guidKey, ppwszValue, pcchLength);
    }

    HRESULT TrackedSample::GetBlobSize(const GUID& guidKey, UINT32* pcbBlobSize) {
        return m_pAttributes->GetBlobSize(guidKey, pcbBlobSize);
    }

    HRESULT TrackedSample::GetBlob(const GUID& guidKey, UINT8* pBuf, UINT32 cbBufSize, UINT32* pcbBlobSize) {
        return m_pAttributes->GetBlob(guidKey, pBuf, cbBufSize, pcbBlobSize);
    }

    HRESULT TrackedSample::GetAllocatedBlob(const GUID& guidKey, UINT8** ppBuf, UINT32* pcbSize) {
        return m_pAttributes->GetAllocatedBlob(guidKey, ppBuf, pcbSize);
    }

    HRESULT TrackedSample::GetUnknown(const GUID& guidKey, const IID& riid, LPVOID* ppv) {
        return m_pAttributes->GetUnknown(guidKey, riid, ppv);
    }

    HRESULT TrackedSample::SetItem(const GUID& guidKey, const PROPVARIANT& Value) {
        return m_pAttributes->SetItem(guidKey, Value);
    }

    HRESULT TrackedSample::DeleteItem(const GUID& guidKey) {
        return m_pAttributes->DeleteItem(guidKey);
    }

    HRESULT TrackedSample::DeleteAllItems() {
        return m_pAttributes->DeleteAllItems();
    }

    HRESULT TrackedSample::SetUINT32(const GUID& guidKey, UINT32 unValue) {
        return m_pAttributes->SetUINT32(guidKey, unValue);
    }

    HRESULT TrackedSample::SetUINT64(const GUID& guidKey, UINT64 unValue) {
        return m_pAttributes->SetUINT64(guidKey, unValue);
    }

    HRESULT TrackedSample::SetDouble(const GUID& guidKey, double fValue) {
        return m_pAttributes->SetDouble(guidKey, fValue);
    }

    HRESULT TrackedSample::SetGUID(const GUID& guidKey, const GUID& guidValue) {
        return m_pAttributes->SetGUID(guidKey, guidValue);
    }

    HRESULT TrackedSample::SetString(const GUID& guidKey, LPCWSTR wszValue) {
        return m_pAttributes->SetString(guidKey, wszValue);
    }

    HRESULT TrackedSample::SetBlob(const GUID& guidKey, const UINT8* pBuf, UINT32 cbBufSize) {
        return m_pAttributes->SetBlob(guidKey, pBuf, cbBufSize);
    }

    HRESULT TrackedSample::SetUnknown(const GUID& guidKey, IUnknown* pUnknown) {
        return m_pAttributes->SetUnknown(guidKey, pUnknown);
    }

    HRESULT TrackedSample::LockStore() {
        return m_pAttributes->LockStore();
    }

    HRESULT TrackedSample::UnlockStore() {
        return m_pAttributes->UnlockStore();
    }

    HRESULT TrackedSample::GetCount(UINT32* pcItems) {
        return m_pAttributes->GetCount(pcItems);
    }

    HRESULT TrackedSample::GetItemByIndex(UINT32 unIndex, GUID* pguidKey, PROPVARIANT* pValue) {
        return m_pAttributes->GetItemByIndex(unIndex, pguidKey, pValue);
    }

    HRESULT TrackedSample::CopyAllItems(IMFAttributes* pDest) {
        return m_pAttributes->CopyAllItems(pDest);
    }

    HRESULT TrackedSample::GetSampleFlags(DWORD* pdwSampleFlags) {
        *pdwSampleFlags = m_dwSampleFlags;
        return S_OK;
    }

    HRESULT TrackedSample::SetSampleFlags(DWORD dwSampleFlags) {
        m_dwSampleFlags = dwSampleFlags;
        return S_OK;
    }

    HRESULT TrackedSample::GetSampleTime(LONGLONG* phnsSampleTime) {
        *phnsSampleTime = m_hnsSampleTime;
        return S_OK;
    }

    HRESULT TrackedSample::SetSampleTime(LONGLONG hnsSampleTime) {
        m_hnsSampleTime = hnsSampleTime;
        return S_OK;
    }

    HRESULT TrackedSample::GetSampleDuration(LONGLONG* phnsSampleDuration) {
        *phnsSampleDuration = m_hnsSampleDuration;
        return S_OK;
    }

    HRESULT TrackedSample::SetSampleDuration(LONGLONG hnsSampleDuration) {
        m_hnsSampleDuration = hnsSampleDuration;
        return S_OK;
    }

    HRESULT TrackedSample::GetBufferCount(DWORD* pdwBufferCount) {
        *pdwBufferCount = 1;
        return S_OK;
    }

    HRESULT TrackedSample::GetBufferByIndex(DWORD dwIndex, IMFMediaBuffer** ppBuffer) {
        if (dwIndex != 0)
            return E_INVALIDARG;

        *ppBuffer = m_pBuffer;
        m_pBuffer->AddRef();
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
        if (m_pCallback) {
            m_pCallback->Release();
            m_pCallback = 0;
        }

        if (pSampleAllocator) {
            m_pCallback = pSampleAllocator;
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

    TrackedSample::TrackedSample(HRESULT& hr, IMFMediaBuffer* pBuffer) : m_ref(), m_pAttributes(),
        m_pBuffer(), m_pCallback(), m_hnsSampleTime(), m_hnsSampleDuration(), m_dwSampleFlags() {
        hr = MFCreateAttributes(&m_pAttributes, 1);
        if (SUCCEEDED(hr)) {
            m_pBuffer = pBuffer;
            pBuffer->AddRef();
        }
    }

    TrackedSample::~TrackedSample() {
        if (m_pAttributes) {
            m_pAttributes->Release();
            m_pAttributes = 0;
        }

        if (m_pBuffer) {
            m_pBuffer->Release();
            m_pBuffer = 0;
        }

        if (m_pCallback) {
            m_pCallback->Release();
            m_pCallback = 0;
        }
    }

    inline void TrackedSample::Destroy(TrackedSample* ptr) {
        if (!ptr)
            return;

        delete ptr;
    }

    HRESULT CreateTrackedSample(IMFMediaBuffer* pBuffer, uint32_t token, IMFSample*& ppOutSample) {
        HRESULT hr = S_OK;
        TrackedSample* p = new TrackedSample(hr, pBuffer);
        if (!p)
            return E_OUTOFMEMORY;

        if (SUCCEEDED(hr)) {
            hr = p->SetUINT32(SAMPLE_RESET_TOKEN, token);
            if (SUCCEEDED(hr)) {
                ppOutSample = p;
                p->AddRef();
            }
        }

        p->Release();
        return S_OK;
    }
}
