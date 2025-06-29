/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "movie_play_lib.hpp"
#include "sample_queue.hpp"
#include <mftransform.h>

namespace MoviePlayLib {
    class TransformBase : public IMediaTransform {
    public:
        enum DecodeState {
            DecodeState_NeedMoreInput = 0,
            DecodeState_ProcessOutput,
            DecodeState_DrainEndOfStream,
            DecodeState_EndOfStream,
        };

    protected:
        RefCount m_ref;
        SlimLock m_lock;
        PlayerStat_& m_rStat;
        IMediaClock* m_pClock;
        IMediaSource* m_pSource;
        IMFTransform* m_pTransform;
        DecodeState m_decodeState;
        BOOL m_bShutdown;
        BOOL m_bStarted;
        HANDLE m_hIntervalThread;
        HANDLE m_hWaitEvent;
        SampleQueue m_inputQueue;
        SampleQueue m_outputQueue;

    public:
        virtual HRESULT QueryInterface(const IID& riid, void** ppvObject) override;
        virtual ULONG AddRef() override;
        virtual ULONG Release() override;

        virtual HRESULT Shutdown() override;
        virtual HRESULT Close() override;
        virtual HRESULT Flush() override;
        virtual HRESULT Open() override;
        virtual HRESULT GetMediaType(IMFMediaType** pType) override;
        virtual void PushSample(IMFSample* pSample) override;
        virtual BOOL RequestSample() override;
        virtual BOOL IsEndOfStream() override;
        virtual UINT32 GetInputQueueCount() override;
        virtual UINT32 GetOutputQueueCount() override;
        virtual INT64 PeekSampleTime() override;
        virtual void GetSample(IMFSample*& ppOutSample) override;

        TransformBase(HRESULT& hr, PlayerStat_& rStat,
            IMediaClock* pClock, IMediaSource* pSource);
        virtual ~TransformBase();

    protected:
        virtual void _on_shutdown();
        virtual HRESULT _on_flush();
        virtual HRESULT _on_drain();
        virtual HRESULT _on_stop();
        virtual HRESULT _on_start();
        virtual HRESULT _process_input();
        virtual HRESULT _process_output() = 0;
        virtual void _on_input_sample(double_t sampleTime) = 0;
        virtual BOOL _on_process();

    public:
        static void Destroy(TransformBase* ptr);

        inline void Destroy() {
            Destroy(this);
        }

    protected:
        static uint32_t __stdcall _thread_proc(TransformBase* transform_base);
    };
}
