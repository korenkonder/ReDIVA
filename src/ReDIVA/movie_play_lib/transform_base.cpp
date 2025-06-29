/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "transform_base.hpp"
#include <avrt.h>
#pragma comment(lib, "avrt.lib")
#include <process.h>

namespace MoviePlayLib {
    HRESULT TransformBase::QueryInterface(const IID& riid, void** ppvObject) {
        if (!ppvObject)
            return E_INVALIDARG;

        if (riid == __uuidof(IMediaTransform) || riid == __uuidof(IUnknown)) {
            *ppvObject = this;
            AddRef();
            return S_OK;
        }

        *ppvObject = 0;
        return E_NOINTERFACE;
    }

    ULONG TransformBase::AddRef() {
        return ++m_ref;
    }

    ULONG TransformBase::Release() {
        if (!--m_ref)
            Destroy();
        return m_ref;
    }

    HRESULT TransformBase::Shutdown() {
        m_lock.Acquire();
        _on_shutdown();
        m_lock.Release();
        return S_OK;
    }

    HRESULT TransformBase::Close() {
        m_lock.Acquire();
        if (!m_bShutdown)
            _on_flush();
        HRESULT hr = _on_flush();
        m_lock.Release();
        return hr;
    }

    HRESULT TransformBase::Flush() {
        m_lock.Acquire();
        HRESULT hr = m_bShutdown ? MF_E_SHUTDOWN : S_OK;
        m_lock.Release();
        return hr;
    }

    HRESULT TransformBase::Open() {
        m_lock.Acquire();
        HRESULT hr = m_bShutdown ? MF_E_SHUTDOWN : S_OK;
        if (!m_bShutdown && !m_bStarted) {
            m_bStarted = TRUE;
            hr = _on_start();
        }
        m_lock.Release();
        return hr;
    }

    HRESULT TransformBase::GetMediaType(IMFMediaType** pType) {
        m_lock.Acquire();
        HRESULT hr = m_bShutdown ? MF_E_SHUTDOWN : S_OK;
        if (!m_bShutdown)
            hr = m_pTransform->GetOutputCurrentType(0, pType);
        m_lock.Release();
        return hr;
    }

    void TransformBase::PushSample(IMFSample* pSample) {
        if (pSample)
            m_inputQueue.push(pSample, false);
        else
            m_inputQueue.set_eos();

        SetEvent(m_hWaitEvent);

        if (pSample) {
            int64_t sampleTime = 0;
            pSample->GetSampleTime(&sampleTime);
            _on_input_sample((double_t)sampleTime * 0.0000001);
        }
    }

    BOOL TransformBase::RequestSample() {
        return SetEvent(m_hWaitEvent);
    }

    BOOL TransformBase::IsEndOfStream() {
        return m_outputQueue.is_eos();
    }

    UINT32 TransformBase::GetInputQueueCount() {
        return (uint32_t)m_inputQueue.size();
    }

    UINT32 TransformBase::GetOutputQueueCount() {
        return (uint32_t)m_outputQueue.size();
    }

    INT64 TransformBase::PeekSampleTime() {
        return m_outputQueue.peek_time();
    }

    void TransformBase::GetSample(IMFSample*& ppOutSample) {
        ppOutSample = m_outputQueue.pop();
    }

    TransformBase::TransformBase(HRESULT& hr, PlayerStat_& rStat,
        IMediaClock* pClock, IMediaSource* pSource) : m_ref(), m_lock(), m_rStat(rStat),
        m_pClock(), m_pSource(), m_pTransform(), m_decodeState(), m_bShutdown(), m_bStarted(),
        m_hIntervalThread(), m_hWaitEvent(), m_inputQueue(), m_outputQueue() {
        if (SUCCEEDED(hr)) {
            m_hWaitEvent = CreateEventA(0, FALSE, FALSE, 0);
            if (!m_hWaitEvent)
                hr = HRESULT_FROM_WIN32(GetLastError());

            if (SUCCEEDED(hr)) {
                m_hIntervalThread = (HANDLE)_beginthreadex(0, 0, (_beginthreadex_proc_type)TransformBase::_thread_proc, this, 0, 0);
                if (!m_hIntervalThread)
                    hr = HRESULT_FROM_WIN32(GetLastError());

                if (SUCCEEDED(hr)) {
                    m_pClock = pClock;
                    pClock->AddRef();
                    m_pSource = pSource;
                    pSource->AddRef();
                }
            }
        }
    }

    TransformBase::~TransformBase() {
        if (m_hIntervalThread) {
           SetEvent(m_hWaitEvent);
            WaitForSingleObject(m_hIntervalThread, INFINITE);
            CloseHandle(m_hIntervalThread);
            m_hIntervalThread = 0;
        }

        if (m_hWaitEvent) {
            CloseHandle(m_hWaitEvent);
            m_hWaitEvent = 0;
        }
    }

    void TransformBase::_on_shutdown() {
        _on_flush();

        if (m_pTransform) {
            m_pTransform->Release();
            m_pTransform = 0;
        }

        if (m_pSource) {
            m_pSource->Release();
            m_pSource = 0;
        }

        if (m_pClock) {
            m_pClock->Release();
            m_pClock = 0;
        }

        m_bShutdown = TRUE;
        m_bStarted = FALSE;
    }

    HRESULT TransformBase::_on_flush() {
        m_inputQueue.clear();
        m_outputQueue.clear();

        HRESULT hr;
        if (m_pTransform)
            hr = m_pTransform->ProcessMessage(MFT_MESSAGE_COMMAND_FLUSH, 0);
        else
            hr = S_OK;
        m_decodeState = DecodeState_NeedMoreInput;
        return hr;
    }

    HRESULT TransformBase::_on_drain() {
        HRESULT hr = m_pTransform->ProcessMessage(MFT_MESSAGE_NOTIFY_END_OF_STREAM, 0);
        if (SUCCEEDED(hr))
            hr = m_pTransform->ProcessMessage(MFT_MESSAGE_COMMAND_DRAIN, 0);
        return hr;
    }

    HRESULT TransformBase::_on_stop() {
        return m_pTransform->ProcessMessage(MFT_MESSAGE_NOTIFY_END_STREAMING, 0);
    }

    HRESULT TransformBase::_on_start() {
        HRESULT hr = m_pTransform->ProcessMessage(MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, 0);
        if (SUCCEEDED(hr))
            hr = m_pTransform->ProcessMessage(MFT_MESSAGE_NOTIFY_START_OF_STREAM, 0);
        m_decodeState = DecodeState_NeedMoreInput;

        m_inputQueue.reset_eos();
        m_outputQueue.reset_eos();
        return hr;
    }

    HRESULT TransformBase::_process_input() {
        while (!m_decodeState) {
            IMFSample* pSample = m_inputQueue.pop();
            if (pSample) {
                if (SUCCEEDED(m_pTransform->ProcessInput(0, pSample, 0)))
                    m_decodeState = DecodeState_ProcessOutput;
            }
            else {
                if (!m_inputQueue.is_eos())
                    return S_FALSE;

                _on_drain();
                m_decodeState = DecodeState_DrainEndOfStream;
            }

            if (pSample) {
                pSample->Release();
                pSample = 0;
            }
        }
        return S_OK;
    }

    BOOL TransformBase::_on_process() {
        BOOL shutdown = FALSE;
        HRESULT hr = S_OK;
        do {
            m_lock.Acquire();
            if (m_bShutdown) {
                hr = MF_E_SHUTDOWN;
                shutdown = TRUE;
            }
            else if (!m_bStarted)
                hr = S_FALSE;
            else if (m_decodeState || (hr = _process_input()) == S_OK) {
                if ((m_decodeState == DecodeState_NeedMoreInput || m_decodeState == DecodeState_EndOfStream
                    || (hr = _process_output()) == S_OK) && m_decodeState == DecodeState_EndOfStream) {
                    m_outputQueue.set_eos();
                    hr = S_FALSE;
                }
            }
            m_lock.Release();
            Sleep(0);
        } while (hr == S_OK);

        if (FAILED(hr)) {
            m_rStat.SetError(hr, this);
            m_outputQueue.set_eos();
        }
        return shutdown;
    }

    inline void TransformBase::Destroy(TransformBase* ptr) {
        if (!ptr)
            return;

        delete ptr;
    }

    uint32_t __stdcall TransformBase::_thread_proc(TransformBase* transform_base) {
        DWORD task_index = 0;
        HANDLE avrt_handle = AvSetMmThreadCharacteristicsW(L"Playback", &task_index);
        if (!avrt_handle)
#pragma warning(suppress: 6031)
            GetLastError();

        while (!WaitForSingleObject(transform_base->m_hWaitEvent, INFINITE))
            if (transform_base->_on_process())
                break;

        if (avrt_handle)
            AvRevertMmThreadCharacteristics(avrt_handle);
        return 0;
    }
}
