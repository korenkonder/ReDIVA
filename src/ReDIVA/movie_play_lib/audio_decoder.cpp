/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "audio_decoder.hpp"
#include "media_foundation.hpp"
#include <mfapi.h>

namespace MoviePlayLib {
    AudioDecoder::AudioDecoder(HRESULT& hr, MediaStatsLock* media_stats_lock,
        IMediaClock* media_clock, IMediaSource* media_source)
        : TransformBase(hr, media_stats_lock, media_clock, media_source),
        output_sub_type(MFAudioFormat_Float), sample_rate(), channels(), buffer_size() {
        MOVIE_PLAY_LIB_PRINT_FUNC_BEGIN;
        if (SUCCEEDED(hr)) {
            IMFMediaType* mf_media_type = 0;
            hr = media_source->GetAudioMFMediaType(&mf_media_type);
            if (SUCCEEDED(hr))
                hr = GetMFTransform(mf_media_type, output_sub_type, mf_transform);

            if (mf_media_type) {
                mf_media_type->Release();
                mf_media_type = 0;
            }

            if (SUCCEEDED(hr))
                hr = InitFromMediaSource();
        }
        MOVIE_PLAY_LIB_PRINT_FUNC_END;
    }

    AudioDecoder::~AudioDecoder() {
        MOVIE_PLAY_LIB_PRINT_FUNC_BEGIN;
        MOVIE_PLAY_LIB_PRINT_FUNC_END;
    }

    HRESULT AudioDecoder::ProcessOutput() {
        if (sample_list_active.GetSamplesCount() >= 10)
            return S_FALSE;

        MFT_OUTPUT_DATA_BUFFER output_sample = {};
        DWORD output_status = 0;
        IMFMediaBuffer* mf_media_buffer = 0;
        IMFSample* mf_sample = 0;

        if (SUCCEEDED(MFCreateMemoryBuffer(buffer_size, &mf_media_buffer))
            && SUCCEEDED(MFCreateSample(&mf_sample)) && SUCCEEDED(mf_sample->AddBuffer(mf_media_buffer))) {
            output_sample.pSample = mf_sample;
            mf_sample->AddRef();
        }

        if (mf_media_buffer) {
            mf_media_buffer->Release();
            mf_media_buffer = 0;
        }

        if (mf_sample) {
            mf_sample->Release();
            mf_sample = 0;
        }

        const int64_t process_output_begin = GetTimestamp();
        HRESULT hr = mf_transform->ProcessOutput(0, 1, &output_sample, &output_status);
        const int64_t process_output_end = GetTimestamp();

        int64_t sample_time;
        if (FAILED(hr)) {
            switch (hr) {
            case MF_E_TRANSFORM_STREAM_CHANGE:
                hr = InitFromMediaSource();
                break;
            case MF_E_TRANSFORM_NEED_MORE_INPUT:
                hr = S_OK;
                if (stream_state == 2)
                    stream_state = 3;
                else
                    stream_state = 0;
                break;
            default:
                hr = S_FALSE;
                stream_state = 0;
                break;
            }
            goto End;
        }

        sample_list_active.AddSample(mf_sample, false);

        sample_time = 0;
        output_sample.pSample->GetSampleTime(&sample_time);

        media_stats_lock->SetAudioProcessOutput(
            CalcTimeMsec(process_output_begin, process_output_end), (double_t)sample_time * 0.0000001);

    End:
        if (output_sample.pSample) {
            output_sample.pSample->Release();
            output_sample.pSample = 0;
        }

        if (output_sample.pEvents) {
            output_sample.pEvents->Release();
            output_sample.pEvents = 0;
        }
        return hr;
    }

    void AudioDecoder::SetSampleTime(double_t value) {
        media_stats_lock->SetAudioSampleTime(value);
    }

    HRESULT AudioDecoder::InitFromMediaSource() {
        IMFMediaType* video_mf_media_type = 0;
        IMFMediaType* mf_media_type = 0;

        HRESULT hr;
        hr = media_source->GetAudioMFMediaType(&video_mf_media_type);
        if (SUCCEEDED(hr))
            hr = MoviePlayLib::GetMFMediaType(mf_transform, output_sub_type, mf_media_type);

        uint32_t buffer_size = 0;
        MFT_OUTPUT_STREAM_INFO stream_info = {};
        if (SUCCEEDED(hr)) {
            hr = this->mf_transform->GetOutputStreamInfo(0, &stream_info);
            buffer_size = stream_info.cbSize;
        }
        this->buffer_size = buffer_size;

        uint32_t bits_per_sample = 0;
        uint32_t block_alignment = 0;
        uint32_t avg_bytes_per_second = 0;
        if (FAILED(hr))
            goto End;

        hr = mf_media_type->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, &channels);
        if (FAILED(hr))
            goto End;

        hr = mf_media_type->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &sample_rate);
        if (FAILED(hr))
            goto End;

        hr = mf_media_type->GetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, &bits_per_sample);
        if (FAILED(hr))
            goto End;

        hr = mf_media_type->GetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, &block_alignment);
        if (FAILED(hr))
            goto End;

        hr = mf_media_type->GetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, &avg_bytes_per_second);
        if (SUCCEEDED(hr))
            media_stats_lock->SetAudioParams(sample_rate, channels);

    End:
        if (mf_media_type) {
            mf_media_type->Release();
            mf_media_type = 0;
        }

        if (video_mf_media_type) {
            video_mf_media_type->Release();
            video_mf_media_type = 0;
        }
        return hr;
    }

    HRESULT AudioDecoder::Create(MediaStatsLock* media_stats_lock,
        IMediaClock* media_clock, IMediaSource* media_source, IMediaTransform*& ptr) {
        HRESULT hr;
        AudioDecoder* p = new AudioDecoder(hr, media_stats_lock, media_clock, media_source);
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

    inline void AudioDecoder::Destroy(AudioDecoder* ptr) {
        if (!ptr)
            return;

        delete ptr;
    }
}
