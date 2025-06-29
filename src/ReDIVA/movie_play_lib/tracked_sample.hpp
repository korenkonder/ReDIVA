/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#undef UNICODE
#include "movie_play_lib.hpp"
#include <mfidl.h>

namespace MoviePlayLib {
    class DECLSPEC_UUID("C40A00F2-B93A-4D80-AE8C-5A1C634F58E4")
        TrackedSample : public IMFSample, public IMFTrackedSample, public IMFAsyncResult {
    protected:
        RefCount m_ref;
        IMFAttributes* m_pAttributes;
        IMFMediaBuffer* m_pBuffer;
        IMFAsyncCallback* m_pCallback;
        int64_t m_hnsSampleTime;
        int64_t m_hnsSampleDuration;
        uint32_t m_dwSampleFlags;

    public:
        virtual HRESULT QueryInterface(const IID& riid, void** ppvObject) override;
        virtual ULONG AddRef() override;
        virtual ULONG Release() override;

        virtual HRESULT GetItem(const GUID& guidKey, PROPVARIANT* pValue) override;
        virtual HRESULT GetItemType(const GUID& guidKey, MF_ATTRIBUTE_TYPE* pType) override;
        virtual HRESULT CompareItem(const GUID& guidKey, const PROPVARIANT& Value, BOOL* pbResult) override;
        virtual HRESULT Compare(IMFAttributes* pTheirs, MF_ATTRIBUTES_MATCH_TYPE MatchType, BOOL* pbResult) override;
        virtual HRESULT GetUINT32(const GUID& guidKey, UINT32* punValue) override;
        virtual HRESULT GetUINT64(const GUID& guidKey, UINT64* punValue) override;
        virtual HRESULT GetDouble(const GUID& guidKey, double* pfValue) override;
        virtual HRESULT GetGUID(const GUID& guidKey, GUID* pguidValue) override;
        virtual HRESULT GetStringLength(const GUID& guidKey, UINT32* pcchLength) override;
        virtual HRESULT GetString(const GUID& guidKey, LPWSTR pwszValue, UINT32 cchBufSize, UINT32* pcchLength) override;
        virtual HRESULT GetAllocatedString(const GUID& guidKey, LPWSTR* ppwszValue, UINT32* pcchLength) override;
        virtual HRESULT GetBlobSize(const GUID& guidKey, UINT32* pcbBlobSize) override;
        virtual HRESULT GetBlob(const GUID& guidKey, UINT8* pBuf, UINT32 cbBufSize, UINT32* pcbBlobSize) override;
        virtual HRESULT GetAllocatedBlob(const GUID& guidKey, UINT8** ppBuf, UINT32* pcbSize) override;
        virtual HRESULT GetUnknown(const GUID& guidKey, const IID& riid, LPVOID* ppv) override;
        virtual HRESULT SetItem(const GUID& guidKey, const PROPVARIANT& Value) override;
        virtual HRESULT DeleteItem(const GUID& guidKey) override;
        virtual HRESULT DeleteAllItems() override;
        virtual HRESULT SetUINT32(const GUID& guidKey, UINT32 unValue) override;
        virtual HRESULT SetUINT64(const GUID& guidKey, UINT64 unValue) override;
        virtual HRESULT SetDouble(const GUID& guidKey, double fValue) override;
        virtual HRESULT SetGUID(const GUID& guidKey, const GUID& guidValue) override;
        virtual HRESULT SetString(const GUID& guidKey, LPCWSTR wszValue) override;
        virtual HRESULT SetBlob(const GUID& guidKey, const UINT8* pBuf, UINT32 cbBufSize) override;
        virtual HRESULT SetUnknown(const GUID& guidKey, IUnknown* pUnknown) override;
        virtual HRESULT LockStore() override;
        virtual HRESULT UnlockStore() override;
        virtual HRESULT GetCount(UINT32* pcItems) override;
        virtual HRESULT GetItemByIndex(UINT32 unIndex, GUID* pguidKey, PROPVARIANT* pValue) override;
        virtual HRESULT CopyAllItems(IMFAttributes* pDest) override;

        virtual HRESULT GetSampleFlags(DWORD* pdwSampleFlags) override;
        virtual HRESULT SetSampleFlags(DWORD dwSampleFlags) override;
        virtual HRESULT GetSampleTime(LONGLONG* phnsSampleTime) override;
        virtual HRESULT SetSampleTime(LONGLONG hnsSampleTime) override;
        virtual HRESULT GetSampleDuration(LONGLONG* phnsSampleDuration) override;
        virtual HRESULT SetSampleDuration(LONGLONG hnsSampleDuration) override;
        virtual HRESULT GetBufferCount(DWORD* pdwBufferCount) override;
        virtual HRESULT GetBufferByIndex(DWORD dwIndex, IMFMediaBuffer** ppBuffer) override;
        virtual HRESULT ConvertToContiguousBuffer(IMFMediaBuffer** ppBuffer) override;
        virtual HRESULT AddBuffer(IMFMediaBuffer* pBuffer) override;
        virtual HRESULT RemoveBufferByIndex(DWORD dwIndex) override;
        virtual HRESULT RemoveAllBuffers() override;
        virtual HRESULT GetTotalLength(DWORD* pcbTotalLength) override;
        virtual HRESULT CopyToBuffer(IMFMediaBuffer* pBuffer) override;

        virtual HRESULT SetAllocator(IMFAsyncCallback* pSampleAllocator, IUnknown* pUnkState) override;

        virtual HRESULT GetState(IUnknown** ppunkState) override;
        virtual HRESULT GetStatus() override;
        virtual HRESULT SetStatus(HRESULT hrStatus) override;
        virtual HRESULT GetObject(IUnknown** ppObject) override;
        virtual IUnknown* GetStateNoAddRef() override;

        TrackedSample(HRESULT& hr, IMFMediaBuffer* pBuffer);
        virtual ~TrackedSample();

        static void Destroy(TrackedSample* ptr);

        inline void Destroy() {
            Destroy(this);
        }
    };

    extern HRESULT CreateTrackedSample(IMFMediaBuffer* pBuffer, uint32_t token, IMFSample*& ppOutSample);
}
