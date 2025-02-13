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
        RefCount ref_count;
        Lock lock;
        BOOL shutdown;
        BOOL sample_recieve;
        AudioParams params;
        uint32_t channels;
        int32_t sample_rate;
        sound::wasapi::StreamingChannel* streaming_channel;
        IMFMediaBuffer* media_buffers[10];
        size_t media_buffer_count;
        IMFMediaBuffer* curr_media_buffer;
        const float_t* buffer;
        size_t samples_count;

    public:
        virtual HRESULT QueryInterface(const IID& riid, void** ppvObject) override;
        virtual ULONG AddRef() override;
        virtual ULONG Release() override;

        virtual HRESULT Shutdown() override;
        virtual HRESULT Close() override;
        virtual HRESULT Flush() override;
        virtual HRESULT Open() override;
        virtual HRESULT SetMFSample(IMFSample* mf_sample) override;

        virtual HRESULT GetParams(AudioParams* value) override;
        virtual HRESULT SetParams(const AudioParams* value) override;

        WASAPIRenderer(HRESULT& hr, int32_t sample_rate, int32_t channels);
        virtual ~WASAPIRenderer();

        void FillBuffer(sound_buffer_data* buffer, size_t samples_count);
        void Stop();

        static HRESULT Create(IMediaClock* media_clock,
            int32_t sample_rate, int32_t channels, WASAPIRenderer*& ptr);
        static void Destroy(WASAPIRenderer* ptr);
        static void FillBufferStatic(sound_buffer_data* buffer, size_t samples_count, void* data);

        inline void Destroy() {
            Destroy(this);
        }
    };
}
