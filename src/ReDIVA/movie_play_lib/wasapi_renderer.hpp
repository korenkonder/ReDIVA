/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "movie_play_lib.hpp"
#include "../../CRE/sound.hpp"

namespace MoviePlayLib {
    class WASAPIRenderer : public IAudioRenderer {
    protected:
        RefCount m_ref;
        SlimLock m_lock;
        BOOL m_bShutdown;
        BOOL m_bStarted;
        AudioVolumes m_volumes;
        uint32_t m_channelCount;
        uint32_t m_sampleFrequency;
        sound::wasapi::StreamingChannel* m_streamingChannel;
        IMFMediaBuffer* m_pBuffers[10];
        size_t m_bufferCount;
        IMFMediaBuffer* m_pCurrBuffer;
        const float_t* m_pbBuffer;
        size_t m_sampleCount;

    public:
        virtual HRESULT QueryInterface(const IID& riid, void** ppvObject) override;
        virtual ULONG AddRef() override;
        virtual ULONG Release() override;

        virtual HRESULT Shutdown() override;
        virtual HRESULT Close() override;
        virtual HRESULT Flush() override;
        virtual HRESULT Open() override;
        virtual HRESULT ProcessSample(IMFSample* pSample) override;

        virtual HRESULT GetVolumes(AudioVolumes* out_volumes) override;
        virtual HRESULT SetVolumes(const AudioVolumes* in_volumes) override;

        WASAPIRenderer(HRESULT& hr, int32_t sampleFrequency, int32_t channelCount);
        virtual ~WASAPIRenderer();

        void FillBuffer(sound_buffer_data* buffer, size_t samples_count);
        void Stop();

        static void Destroy(WASAPIRenderer* ptr);
        static void FillBufferStatic(sound_buffer_data* buffer, size_t sampleCount, void* data);

        inline void Destroy() {
            Destroy(this);
        }
    };

    extern HRESULT CreateWASAPIRenderer(IMediaClock* pClock,
        int32_t sampleFrequency, int32_t channelCount, IAudioRenderer*& pp);
}
