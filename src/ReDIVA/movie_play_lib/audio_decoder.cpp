/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "audio_decoder.hpp"
#include "media_foundation.hpp"
#include <mfapi.h>

namespace MoviePlayLib {
    AudioDecoder::AudioDecoder(HRESULT& hr, PlayerStat_& rStat, IMediaClock* pClock, IMediaSource* pSource)
        : TransformBase(hr, rStat, pClock, pSource),
        m_formatSubType(MFAudioFormat_Float), m_sampleFrequency(), m_channelCount(), m_cbBufferBytes() {
        MOVIE_PLAY_LIB_TRACE_BEGIN;
        if (SUCCEEDED(hr)) {
            IMFMediaType* pType = 0;
            hr = pSource->GetAudioMediaType(&pType);
            if (SUCCEEDED(hr))
                hr = CreateDecoder(pType, m_formatSubType, m_pTransform);

            if (pType) {
                pType->Release();
                pType = 0;
            }

            if (SUCCEEDED(hr))
                hr = _on_format_changed();
        }
        MOVIE_PLAY_LIB_TRACE_END;
    }

    AudioDecoder::~AudioDecoder() {
        MOVIE_PLAY_LIB_TRACE_BEGIN;
        MOVIE_PLAY_LIB_TRACE_END;
    }

    HRESULT AudioDecoder::_process_output() {
        if (m_outputQueue.size() >= 10)
            return S_FALSE;

        MFT_OUTPUT_DATA_BUFFER outputSample = {};
        DWORD dwStatus = 0;
        IMFMediaBuffer* pBuffer = 0;
        IMFSample* pSample = 0;

        if (SUCCEEDED(MFCreateMemoryBuffer(m_cbBufferBytes, &pBuffer))
            && SUCCEEDED(MFCreateSample(&pSample)) && SUCCEEDED(pSample->AddBuffer(pBuffer))) {
            outputSample.pSample = pSample;
            pSample->AddRef();
        }

        if (pBuffer) {
            pBuffer->Release();
            pBuffer = 0;
        }

        if (pSample) {
            pSample->Release();
            pSample = 0;
        }

        const int64_t decodeAudioBegin = GetTimestamp();
        HRESULT hr = m_pTransform->ProcessOutput(0, 1, &outputSample, &dwStatus);
        const int64_t decodeAudioEnd = GetTimestamp();

        int64_t hnsSampleTime;
        if (FAILED(hr)) {
            switch (hr) {
            case MF_E_TRANSFORM_STREAM_CHANGE:
                hr = _on_format_changed();
                break;
            case MF_E_TRANSFORM_NEED_MORE_INPUT:
                hr = S_OK;
                if (m_decodeState == DecodeState_DrainEndOfStream)
                    m_decodeState = DecodeState_EndOfStream;
                else
                    m_decodeState = DecodeState_NeedMoreInput;
                break;
            default:
                hr = S_FALSE;
                m_decodeState = DecodeState_NeedMoreInput;
                break;
            }
            goto End;
        }

        m_outputQueue.push(pSample, false);

        hnsSampleTime = 0;
        outputSample.pSample->GetSampleTime(&hnsSampleTime);

        m_rStat.SetAudioDecodeTime(
            CalcTimeMsec(decodeAudioBegin, decodeAudioEnd), (double_t)hnsSampleTime * 0.0000001);

    End:
        if (outputSample.pSample) {
            outputSample.pSample->Release();
            outputSample.pSample = 0;
        }

        if (outputSample.pEvents) {
            outputSample.pEvents->Release();
            outputSample.pEvents = 0;
        }
        return hr;
    }

    void AudioDecoder::_on_input_sample(double_t sampleTime) {
        m_rStat.SetSourceAudioSampleTime(sampleTime);
    }

    inline void AudioDecoder::Destroy(AudioDecoder* ptr) {
        if (!ptr)
            return;

        delete ptr;
    }

    HRESULT AudioDecoder::_on_format_changed() {
        IMFMediaType* pInputType = 0;
        IMFMediaType* pOutputType = 0;

        HRESULT hr;
        hr = m_pSource->GetAudioMediaType(&pInputType);
        if (SUCCEEDED(hr))
            hr = MoviePlayLib::SelectDecoderOutputFormat(m_pTransform, m_formatSubType, pOutputType);

        uint32_t cbBufferBytes = 0;
        MFT_OUTPUT_STREAM_INFO streamInfo = {};
        if (SUCCEEDED(hr)) {
            hr = m_pTransform->GetOutputStreamInfo(0, &streamInfo);
            cbBufferBytes = streamInfo.cbSize;
        }
        m_cbBufferBytes = cbBufferBytes;

        uint32_t bitsPerSample = 0;
        uint32_t blockAlignment = 0;
        uint32_t avgBytesPerSecond = 0;
        if (FAILED(hr))
            goto End;

        hr = pOutputType->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, &m_channelCount);
        if (FAILED(hr))
            goto End;

        hr = pOutputType->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &m_sampleFrequency);
        if (FAILED(hr))
            goto End;

        hr = pOutputType->GetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, &bitsPerSample);
        if (FAILED(hr))
            goto End;

        hr = pOutputType->GetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, &blockAlignment);
        if (FAILED(hr))
            goto End;

        hr = pOutputType->GetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, &avgBytesPerSecond);
        if (SUCCEEDED(hr))
            m_rStat.SetAudioParams(m_sampleFrequency, m_channelCount);

    End:
        if (pOutputType) {
            pOutputType->Release();
            pOutputType = 0;
        }

        if (pInputType) {
            pInputType->Release();
            pInputType = 0;
        }
        return hr;
    }

    HRESULT CreateAudioDecoder(PlayerStat_& rStat,
        IMediaClock* pClock, IMediaSource* pSource, IMediaTransform*& pp) {
        HRESULT hr;
        AudioDecoder* p = new AudioDecoder(hr, rStat, pClock, pSource);
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
