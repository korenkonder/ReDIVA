/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "wasapi_renderer.hpp"

namespace MoviePlayLib {
    HRESULT WASAPIRenderer::QueryInterface(const IID& riid, void** ppvObject) {
        if (!ppvObject)
            return E_INVALIDARG;

        if (riid == __uuidof(IAudioRenderer) || riid == __uuidof(IUnknown)) {
            *ppvObject = this;
            AddRef();
            return S_OK;
        }

        *ppvObject = 0;
        return E_NOINTERFACE;
    }

    ULONG WASAPIRenderer::AddRef() {
        return ++ref_count;
    }

    ULONG WASAPIRenderer::Release() {
        if (!--ref_count)
            Destroy();
        return ref_count;
    }

    HRESULT WASAPIRenderer::Shutdown() {
        lock.Acquire();
        Stop();

        if (streaming_channel) {
            streaming_channel->SetResetOne();
            streaming_channel = 0;
        }

        shutdown = TRUE;
        sample_recieve = FALSE;
        lock.Release();
        return 0;
    }

    HRESULT WASAPIRenderer::Close() {
        lock.Acquire();
        HRESULT hr = shutdown ? MF_E_SHUTDOWN : S_OK;
        if (!shutdown)
            Stop();
        lock.Release();
        return hr;
    }

    HRESULT WASAPIRenderer::Flush() {
        lock.Acquire();
        HRESULT hr = shutdown ? MF_E_SHUTDOWN : S_OK;
        if (!shutdown && sample_recieve)
            sample_recieve = FALSE;
        lock.Release();
        return hr;
    }

    HRESULT WASAPIRenderer::Open() {
        lock.Acquire();
        HRESULT hr = shutdown ? MF_E_SHUTDOWN : S_OK;
        if (!shutdown && !sample_recieve)
            sample_recieve = TRUE;
        lock.Release();
        return hr;
    }

    HRESULT WASAPIRenderer::SetMFSample(IMFSample* mf_sample) {
        lock.Acquire();

        HRESULT hr = shutdown ? MF_E_SHUTDOWN : S_OK;
        if (!shutdown) {
            IMFMediaBuffer* mf_media_buffer = 0;
            DWORD current_length = 0;
            hr = mf_sample->ConvertToContiguousBuffer(&mf_media_buffer);
            if (FAILED(hr))
                goto End;

            hr = mf_media_buffer->GetCurrentLength(&current_length);
            if (FAILED(hr))
                goto End;

            if (current_length / (sizeof(float_t) * channels) && media_buffer_count < 10) {
                media_buffers[media_buffer_count] = mf_media_buffer;
                media_buffers[media_buffer_count]->AddRef();
                media_buffer_count++;
            }

        End:
            if (mf_media_buffer) {
                mf_media_buffer->Release();
                mf_media_buffer = 0;
            }
        }

        lock.Release();
        return hr;
    }

    HRESULT WASAPIRenderer::GetParams(AudioParams* value) {
        lock.Acquire();
        *value = params;
        lock.Release();
        return S_OK;
    }

    HRESULT WASAPIRenderer::SetParams(const AudioParams* value) {
        lock.Acquire();
        params = *value;
        lock.Release();
        return S_OK;
    }

    WASAPIRenderer::WASAPIRenderer(HRESULT& hr, int32_t sample_rate, int32_t channels) : ref_count(),
        lock(), shutdown(), sample_recieve(), params(), channels(channels), sample_rate(sample_rate),
        streaming_channel(), media_buffers(), media_buffer_count(), curr_media_buffer(), buffer(), samples_count() {
        MOVIE_PLAY_LIB_PRINT_FUNC_BEGIN;
        if (SUCCEEDED(hr)) {
            params.spk_l_volume = 1.0f;
            params.spk_r_volume = 1.0f;
            params.field_8 = 1.0f;
            params.field_C = 1.0f;
            params.hph_l_volume = 1.0f;
            params.hph_r_volume = 1.0f;
            memset(media_buffers, 0, sizeof(media_buffers));

            sound::wasapi::System* system = sound_wasapi_system_data_get();
            if (system) {
                if (sample_rate != system->config.sample_rate)
                    hr = MF_E_INVALIDTYPE;

                sound::wasapi::Mixer* mixer = system->mixer;
                if (mixer) {
                    size_t streaming_channels_count = mixer->streaming_channels_count;
                    if (streaming_channels_count) {
                        sound::wasapi::StreamingChannel* streaming_channel
                            = mixer->GetStreamingChannel(streaming_channels_count - 1);
                        this->streaming_channel = streaming_channel;
                        if (streaming_channel) {
                            streaming_channel->SetMasterVolume(1.0f);
                            streaming_channel->SetCallback(WASAPIRenderer::FillBufferStatic, this);
                        }
                        else
                            hr = E_FAIL;
                    }
                }
            }
        }
        MOVIE_PLAY_LIB_PRINT_FUNC_END;
    }

    WASAPIRenderer::~WASAPIRenderer() {
        MOVIE_PLAY_LIB_PRINT_FUNC_BEGIN;
        MOVIE_PLAY_LIB_PRINT_FUNC_END;
    }

    void WASAPIRenderer::FillBuffer(sound_buffer_data* buffer, size_t samples_count) {
        lock.Acquire();

        const int8_t* channel_map = 0;
        switch (channels) {
        case 1: {
            static const int8_t channel_map_mono[] = { 0, 0, 0, 0 };
            channel_map = channel_map_mono;
        } break;
        case 2: {
            static const int8_t channel_map_stereo[] = { 0, 1, 0, 1 };
            channel_map = channel_map_stereo;
        } break;
        case 4: {
            static const int8_t channel_map_quad[] = { 0, 1, 2, 3 };
            channel_map = channel_map_quad;
        } break;
        }

        if (channel_map && !shutdown && sample_recieve)
            while (samples_count) {
                while (!curr_media_buffer) {
                    if (!media_buffer_count)
                        break;

                    curr_media_buffer = media_buffers[0];
                    for (size_t j = 1; j < media_buffer_count; j++)
                        media_buffers[j - 1] = media_buffers[j];
                    media_buffer_count--;

                    BYTE* buffer = 0;
                    DWORD current_length = 0;
                    if (SUCCEEDED(curr_media_buffer->Lock(&buffer, 0, &current_length))) {
                        this->buffer = (float_t*)buffer;
                        this->samples_count = current_length / (4ULL * channels);
                    }
                    else {
                        curr_media_buffer->Release();
                        curr_media_buffer = 0;
                    }
                }

                if (!curr_media_buffer)
                    break;

                size_t _samples_count = min_def(samples_count, this->samples_count);

                const float_t* src_buffer = this->buffer;
                for (size_t v20 = _samples_count; v20; v20--, src_buffer += channels, buffer++) {
                    buffer->spk_l = src_buffer[channel_map[0]] * this->params.spk_l_volume;
                    buffer->spk_r = src_buffer[channel_map[1]] * this->params.spk_r_volume;
                    buffer->hph_l = src_buffer[channel_map[2]] * this->params.hph_l_volume;
                    buffer->hph_r = src_buffer[channel_map[3]] * this->params.hph_r_volume;
                }
                this->buffer = src_buffer;

                this->samples_count -= _samples_count;
                samples_count -= _samples_count;

                if (!this->samples_count) {
                    curr_media_buffer->Unlock();
                    curr_media_buffer->Release();
                    curr_media_buffer = 0;
                    this->buffer = 0;
                    this->samples_count = 0;
                }
            }

        while (samples_count--)
            *buffer++ = {};

        lock.Release();
    }

    void WASAPIRenderer::Stop() {
        if (curr_media_buffer) {
            curr_media_buffer->Unlock();
            curr_media_buffer->Release();
            curr_media_buffer = 0;

            buffer = 0;
            samples_count = 0;
        }

        for (size_t i = 0; i < media_buffer_count; i++) {
            media_buffers[i]->Release();
            media_buffers[i] = 0;
        }
        media_buffer_count = 0;
    }

    HRESULT WASAPIRenderer::Create(IMediaClock* media_clock,
        int32_t sample_rate, int32_t channels, WASAPIRenderer*& ptr) {
        HRESULT hr = S_OK;
        WASAPIRenderer* p = new WASAPIRenderer(hr, sample_rate, channels);
        if (!p)
            return E_OUTOFMEMORY;

        if (SUCCEEDED(hr)) {
            ptr = p;
            p->AddRef();
        }
        else
            p->Shutdown();

        p->Release();
        return S_OK;
    }

    inline void WASAPIRenderer::Destroy(WASAPIRenderer* ptr) {
        if (!ptr)
            return;

        delete ptr;
    }

    void WASAPIRenderer::FillBufferStatic(sound_buffer_data* buffer, size_t samples_count, void* data) {
        if (data)
            ((WASAPIRenderer*)data)->FillBuffer(buffer, samples_count);
    }
}
