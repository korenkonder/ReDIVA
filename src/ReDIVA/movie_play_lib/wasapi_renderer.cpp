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
        return ++m_ref;
    }

    ULONG WASAPIRenderer::Release() {
        if (!--m_ref)
            Destroy();
        return m_ref;
    }

    HRESULT WASAPIRenderer::Shutdown() {
        m_lock.Acquire();
        Stop();

        if (m_streamingChannel) {
            m_streamingChannel->SetResetOne();
            m_streamingChannel = 0;
        }

        m_bShutdown = TRUE;
        m_bStarted = FALSE;
        m_lock.Release();
        return 0;
    }

    HRESULT WASAPIRenderer::Close() {
        m_lock.Acquire();
        HRESULT hr = m_bShutdown ? MF_E_SHUTDOWN : S_OK;
        if (!m_bShutdown)
            Stop();
        m_lock.Release();
        return hr;
    }

    HRESULT WASAPIRenderer::Flush() {
        m_lock.Acquire();
        HRESULT hr = m_bShutdown ? MF_E_SHUTDOWN : S_OK;
        if (!m_bShutdown && m_bStarted)
            m_bStarted = FALSE;
        m_lock.Release();
        return hr;
    }

    HRESULT WASAPIRenderer::Open() {
        m_lock.Acquire();
        HRESULT hr = m_bShutdown ? MF_E_SHUTDOWN : S_OK;
        if (!m_bShutdown && !m_bStarted)
            m_bStarted = TRUE;
        m_lock.Release();
        return hr;
    }

    HRESULT WASAPIRenderer::ProcessSample(IMFSample* pSample) {
        m_lock.Acquire();

        HRESULT hr = m_bShutdown ? MF_E_SHUTDOWN : S_OK;
        if (!m_bShutdown) {
            IMFMediaBuffer* m_pBuffer = 0;
            DWORD cbCurrentLength = 0;
            hr = pSample->ConvertToContiguousBuffer(&m_pBuffer);
            if (FAILED(hr))
                goto End;

            hr = m_pBuffer->GetCurrentLength(&cbCurrentLength);
            if (FAILED(hr))
                goto End;

            if (cbCurrentLength / (sizeof(float_t) * m_channelCount) && m_bufferCount < 10) {
                m_pBuffers[m_bufferCount] = m_pBuffer;
                m_pBuffers[m_bufferCount]->AddRef();
                m_bufferCount++;
            }

        End:
            if (m_pBuffer) {
                m_pBuffer->Release();
                m_pBuffer = 0;
            }
        }

        m_lock.Release();
        return hr;
    }

    HRESULT WASAPIRenderer::GetVolumes(AudioVolumes* out_volumes) {
        m_lock.Acquire();
        *out_volumes = m_volumes;
        m_lock.Release();
        return S_OK;
    }

    HRESULT WASAPIRenderer::SetVolumes(const AudioVolumes* in_volumes) {
        m_lock.Acquire();
        m_volumes = *in_volumes;
        m_lock.Release();
        return S_OK;
    }

    WASAPIRenderer::WASAPIRenderer(HRESULT& hr, int32_t sampleFrequency, int32_t channelCount)
        : m_ref(), m_lock(), m_bShutdown(), m_bStarted(), m_volumes(),
        m_channelCount(channelCount) , m_sampleFrequency(sampleFrequency),
        m_streamingChannel(), m_pBuffers(), m_bufferCount(), m_pCurrBuffer(), m_pbBuffer(), m_sampleCount() {
        MOVIE_PLAY_LIB_TRACE_BEGIN;
        if (SUCCEEDED(hr)) {
            m_volumes.spk_l_volume = 1.0f;
            m_volumes.spk_r_volume = 1.0f;
            m_volumes.field_8 = 1.0f;
            m_volumes.field_C = 1.0f;
            m_volumes.hph_l_volume = 1.0f;
            m_volumes.hph_r_volume = 1.0f;
            memset(m_pBuffers, 0, sizeof(m_pBuffers));

            sound::wasapi::System* system = sound_wasapi_system_data_get();
            if (system) {
                if (sampleFrequency != system->config.sample_rate)
                    hr = MF_E_INVALIDTYPE;

                sound::wasapi::Mixer* mixer = system->mixer;
                if (mixer) {
                    size_t streaming_channels_count = mixer->streaming_channels_count;
                    if (streaming_channels_count) {
                        sound::wasapi::StreamingChannel* m_streamingChannel
                            = mixer->GetStreamingChannel(streaming_channels_count - 1);
                        m_streamingChannel = m_streamingChannel;
                        if (m_streamingChannel) {
                            m_streamingChannel->SetMasterVolume(1.0f);
                            m_streamingChannel->SetCallback(WASAPIRenderer::FillBufferStatic, this);
                        }
                        else
                            hr = E_FAIL;
                    }
                }
            }
        }
        MOVIE_PLAY_LIB_TRACE_END;
    }

    WASAPIRenderer::~WASAPIRenderer() {
        MOVIE_PLAY_LIB_TRACE_BEGIN;
        MOVIE_PLAY_LIB_TRACE_END;
    }

    void WASAPIRenderer::FillBuffer(sound_buffer_data* buffer, size_t sampleCount) {
        m_lock.Acquire();

        const int8_t* channelMap = 0;
        switch (m_channelCount) {
        case 1: {
            static const int8_t channelMapMono[] = { 0, 0, 0, 0 };
            channelMap = channelMapMono;
        } break;
        case 2: {
            static const int8_t channelMapStereo[] = { 0, 1, 0, 1 };
            channelMap = channelMapStereo;
        } break;
        case 4: {
            static const int8_t channelMapQuad[] = { 0, 1, 2, 3 };
            channelMap = channelMapQuad;
        } break;
        }

        if (channelMap && !m_bShutdown && m_bStarted)
            while (sampleCount) {
                while (!m_pCurrBuffer) {
                    if (!m_bufferCount)
                        break;

                    m_pCurrBuffer = m_pBuffers[0];
                    for (size_t j = 1; j < m_bufferCount; j++)
                        m_pBuffers[j - 1] = m_pBuffers[j];
                    m_bufferCount--;

                    BYTE* pbBuffer = 0;
                    DWORD cbCurrentLength = 0;
                    if (SUCCEEDED(m_pCurrBuffer->Lock(&pbBuffer, 0, &cbCurrentLength))) {
                        m_pbBuffer = (float_t*)pbBuffer;
                        m_sampleCount = cbCurrentLength / (4ULL * m_channelCount);
                    }
                    else {
                        m_pCurrBuffer->Release();
                        m_pCurrBuffer = 0;
                    }
                }

                if (!m_pCurrBuffer)
                    break;

                size_t _sampleCount = min_def(sampleCount, m_sampleCount);

                const float_t* pbBuffer = m_pbBuffer;
                for (size_t remainSampleCount = _sampleCount; remainSampleCount;
                    remainSampleCount--, pbBuffer += m_channelCount, buffer++) {
                    buffer->spk_l = pbBuffer[channelMap[0]] * m_volumes.spk_l_volume;
                    buffer->spk_r = pbBuffer[channelMap[1]] * m_volumes.spk_r_volume;
                    buffer->hph_l = pbBuffer[channelMap[2]] * m_volumes.hph_l_volume;
                    buffer->hph_r = pbBuffer[channelMap[3]] * m_volumes.hph_r_volume;
                }
                m_pbBuffer = pbBuffer;

                m_sampleCount -= _sampleCount;
                sampleCount -= _sampleCount;

                if (!m_sampleCount) {
                    m_pCurrBuffer->Unlock();
                    m_pCurrBuffer->Release();
                    m_pCurrBuffer = 0;
                    m_pbBuffer = 0;
                    m_sampleCount = 0;
                }
            }

        while (sampleCount--)
            *buffer++ = {};

        m_lock.Release();
    }

    void WASAPIRenderer::Stop() {
        if (m_pCurrBuffer) {
            m_pCurrBuffer->Unlock();
            m_pCurrBuffer->Release();
            m_pCurrBuffer = 0;

            m_pbBuffer = 0;
            m_sampleCount = 0;
        }

        for (size_t i = 0; i < m_bufferCount; i++) {
            m_pBuffers[i]->Release();
            m_pBuffers[i] = 0;
        }
        m_bufferCount = 0;
    }

    inline void WASAPIRenderer::Destroy(WASAPIRenderer* ptr) {
        if (!ptr)
            return;

        delete ptr;
    }

    void WASAPIRenderer::FillBufferStatic(sound_buffer_data* buffer, size_t sampleCount, void* data) {
        if (data)
            ((WASAPIRenderer*)data)->FillBuffer(buffer, sampleCount);
    }

    HRESULT CreateWASAPIRenderer(IMediaClock* pClock,
        int32_t sampleFrequency, int32_t channelCount, IAudioRenderer*& pp) {
        HRESULT hr = S_OK;
        WASAPIRenderer* p = new WASAPIRenderer(hr, sampleFrequency, channelCount);
        if (!p)
            return E_OUTOFMEMORY;

        if (SUCCEEDED(hr)) {
            pp = p;
            p->AddRef();
        }
        else
            p->Shutdown();

        p->Release();
        return S_OK;
    }
}
