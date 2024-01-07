/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "sound.hpp"
#include "../KKdLib/io/file_stream.hpp"
#include "../KKdLib/key_val.hpp"
#include "../KKdLib/str_utils.hpp"
#include "../KKdLib/time.hpp"
#include "../KKdLib/vec.hpp"
#include "data.hpp"
#include "ogg_vorbis.hpp"
#include <functiondiscoverykeys_devpkey.h>
#include <timeapi.h>

struct ima_storage {
    int32_t step_index;
    int32_t current;

    inline ima_storage() : step_index(), current() {

    }
};

struct SoundCueQueueVolume {
    float_t min;
    float_t max;
};

struct sound_stream {
    std::string path;
    OggPlayback* ogg_playback;
    float_t current_volume;
    float_t duration;
    float_t time;
    bool pause;
    uint32_t file_loading_frames;
    uint32_t file_ready_frames;
    float_t load_time_seek;
    int32_t volume_trans;
    float_t target_volume;
    int32_t state;
    int32_t play_state;

    sound_stream();
    ~sound_stream();

    bool check_state();
    void ctrl();
    void ctrl_playback();
    void reset();
    void set_current_volume(float_t value);
    bool set_path(const char* path, bool pause = false);
    bool set_path(const char* path, float_t time, bool pause = false);
    bool set_path_playback();
    bool set_pause(bool value);
    void set_target_volume(float_t value, int32_t frames);
    bool stop();
    bool stop_playback();
};

static void sound_stream_array_init();
static void sound_stream_array_ctrl();
static sound_stream* sound_stream_array_get(int32_t index);
static void sound_stream_array_free();

SoundWork* sound_work;
sound::wasapi::System* sound_wasapi_system_data;
std::map<std::string, WaveAudio> wave_audio_storage_data;

static int32_t sound_cue_counter = 0;
static int32_t sound_wasapi_system_counter = 0;

static sound_stream* sound_stream_array;
static float_t sound_stream_volume = 1.0f;

static const int8_t ima_index_table[] = {
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
};

static const int16_t ima_step_table[] = {
        7,     8,     9,    10,    11,    12,    13,    14,
       16,    17,    19,    21,    23,    25,    28,    31,
       34,    37,    41,    45,    50,    55,    60,    66,
       73,    80,    88,    97,   107,   118,   130,   143,
      157,   173,   190,   209,   230,   253,   279,   307,
      337,   371,   408,   449,   494,   544,   598,   658,
      724,   796,   876,   963,  1060,  1166,  1282,  1411,
     1552,  1707,  1878,  2066,  2272,  2499,  2749,  3024,
     3327,  3660,  4026,  4428,  4871,  5358,  5894,  6484,
     7132,  7845,  8630,  9493, 10442, 11487, 12635, 13899,
    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794,
    32767
};

static const SoundCueQueueVolume sound_cue_queue_volume_array[SOUND_WORK_SE_QUEUE_COUNT] = {
    { 0.0f, 1.0f, },
    { 0.0f, 1.0f, },
    { 0.0f, 1.0f, },
    { 0.3f, 1.0f, },
    { 0.3f, 1.0f, },
};

static size_t ima_decode(int16_t* dst, size_t dst_size, uint8_t* data, size_t samples_count,
    size_t channels, ima_storage* storage, size_t storage_size);

namespace sound {
    namespace wasapi {
        System::System() : wave_format(), pEnumerator(), pDevice(), pAudioClient(),
            samples_count(), pRenderClient(), pClockAdjustment(), hEvent(),
            channels(), sample_rate(), bit_depth(), mixer(), thread(), format() {

        }

        System::~System() {
            Reset();
        }

        void System::Init(size_t se_channels_count, size_t streaming_channels_count,
            bool separate_speakers_headphones) {
            if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator),
                0, 1, __uuidof(IMMDeviceEnumerator), (LPVOID*)&pEnumerator))
                || FAILED(pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice)))
                return;

            if (FAILED(pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pAudioClient)))
                return;

            WAVEFORMATEXTENSIBLE* mix_format;
            if (FAILED(pAudioClient->GetMixFormat((WAVEFORMATEX**)&mix_format)))
                return;

            int32_t channels = separate_speakers_headphones ? 4 : 2;
            int32_t sample_rate;
            int32_t bit_depth;
            if (mix_format->Format.wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
                wave_format = *mix_format;
                sample_rate = mix_format->Format.nSamplesPerSec;
                bit_depth = mix_format->Samples.wValidBitsPerSample;
            }
            else {
                wave_format.Format = mix_format->Format;
                sample_rate = mix_format->Format.nSamplesPerSec;
                bit_depth = 16;
            }

            if (mix_format->Format.wFormatTag == WAVE_FORMAT_EXTENSIBLE
                && wave_format.SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT) {
                bit_depth = 32;
                format = AUDIO_FORMAT_F32;
                wave_format.SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
            }
            else {
                switch (bit_depth) {
                default:
                    bit_depth = 16;
                case 16:
                    format = AUDIO_FORMAT_I16;
                    break;
                case 24:
                    format = AUDIO_FORMAT_I24;
                    break;
                case 32:
                    format = AUDIO_FORMAT_I32;
                    break;
                }

                wave_format.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
            }

            wave_format.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
            wave_format.Format.nChannels = (WORD)channels;
            wave_format.Format.nSamplesPerSec = (DWORD)sample_rate;
            wave_format.Format.nAvgBytesPerSec = (DWORD)(sample_rate * (bit_depth / 8) * channels);
            wave_format.Format.nBlockAlign = (WORD)((bit_depth / 8) * channels);
            wave_format.Format.wBitsPerSample = (WORD)bit_depth;
            wave_format.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
            wave_format.Samples.wValidBitsPerSample = (WORD)bit_depth;
            wave_format.dwChannelMask = channels == 4 ? KSAUDIO_SPEAKER_QUAD : KSAUDIO_SPEAKER_STEREO;

            WAVEFORMATEXTENSIBLE* closest_format;
            if (FAILED(pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED,
                &wave_format.Format, (WAVEFORMATEX**)&closest_format))) {
                wave_format = {};
                return;
            }

            this->channels = channels;
            this->sample_rate = sample_rate;
            this->bit_depth = bit_depth;

            mixer = new Mixer(this);
            if (!mixer)
                return;

            REFERENCE_TIME hnsPeriod;
            if (FAILED(pAudioClient->GetDevicePeriod(&hnsPeriod, 0)))
                return;

            if (pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
                AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY
                | AUDCLNT_STREAMFLAGS_RATEADJUST | AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
                hnsPeriod, 0, &wave_format.Format, 0) == AUDCLNT_ERR(0x019)) {
                if (FAILED(pAudioClient->GetBufferSize(&samples_count)))
                    return;

                pAudioClient->Release();

                if (FAILED(pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pAudioClient)))
                    return;

                hnsPeriod = (REFERENCE_TIME)((double_t)(int32_t)samples_count
                    * 10000000.0 / (double_t)sample_rate + 0.5);

                if (FAILED(pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
                    AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY
                    | AUDCLNT_STREAMFLAGS_RATEADJUST | AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
                    hnsPeriod, 0, &wave_format.Format, 0)))
                    return;
            }

            if (FAILED(pAudioClient->GetBufferSize(&samples_count)))
                return;

            // Rounding to prev base 2
            uint32_t _samples_count = samples_count;
            _samples_count--;
            _samples_count |= _samples_count >> 1;
            _samples_count |= _samples_count >> 2;
            _samples_count |= _samples_count >> 4;
            _samples_count |= _samples_count >> 8;
            _samples_count |= _samples_count >> 16;
            _samples_count++;
            while (_samples_count + (_samples_count >> 1) >= samples_count)
                _samples_count >>= 1;
            samples_count = _samples_count;

            if (!mixer->Init(se_channels_count, streaming_channels_count, samples_count))
                return;

            hEvent = CreateEventW(0, 0, 0, 0);
            if (!hEvent)
                return;

            thread = new std::thread(System::ThreadMain, this);
            if (thread) {
                wchar_t buf[0x80];
                swprintf_s(buf, sizeof(buf) / sizeof(wchar_t), L"sound::wasapi::System #%d", sound_wasapi_system_counter++);
                SetThreadDescription((HANDLE)thread->native_handle(), buf);
            }

            if (!thread || !SetThreadPriority((HANDLE)thread->native_handle(), THREAD_PRIORITY_HIGHEST))
                return;

            if (FAILED(pAudioClient->SetEventHandle(hEvent)))
                return;

            if (SUCCEEDED(pAudioClient->GetService(__uuidof(IAudioRenderClient), (void**)&pRenderClient))) {
                void* data = 0;
                if (SUCCEEDED(pRenderClient->GetBuffer(samples_count, (BYTE**)&data))
                    && SUCCEEDED(pRenderClient->ReleaseBuffer(samples_count, AUDCLNT_BUFFERFLAGS_SILENT)))
                    pAudioClient->Start();
            }

            if (SUCCEEDED(pAudioClient->GetService(__uuidof(IAudioClockAdjustment), (void**)&pClockAdjustment)))
                pClockAdjustment->SetSampleRate((float_t)44100);
        }

        void System::Reset() {
            if (thread) {
                thread_state.set(1);
                thread->join();
                delete thread;
                thread = 0;
            }

            if (pAudioClient)
                pAudioClient->Stop();

            if (pClockAdjustment) {
                pClockAdjustment->Release();
                pClockAdjustment = 0;
            }

            if (pRenderClient) {
                pRenderClient->Release();
                pRenderClient = 0;
            }

            if (pAudioClient) {
                pAudioClient->Release();
                pAudioClient = 0;
            }

            if (pDevice) {
                pDevice->Release();
                pDevice = 0;
            }

            if (pEnumerator) {
                pEnumerator->Release();
                pEnumerator = 0;
            }

            if (hEvent) {
                CloseHandle(hEvent);
                hEvent = 0;
            }

            if (mixer) {
                delete mixer;
                mixer = 0;
            }
        }

        void System::ThreadMain(System* system) {
            if (!system)
                return;

            while (!system->thread_state.get()) {
                WaitForSingleObject(system->hEvent, -1);
                void* data = 0;
                if (SUCCEEDED(system->pRenderClient->GetBuffer(system->samples_count, (BYTE**)&data))) {
                    if (system->mixer && data)
                        system->mixer->FillBuffer(data,
                            system->samples_count, false, false, system->format);

                    if (FAILED(system->pRenderClient->ReleaseBuffer(system->samples_count, 0)))
                        break;
                }
            }
            system->thread_state.set(0);
        }

        Mixer::Mixer(System* system) : se_channels(), se_channels_count(),
            streaming_channels(), streaming_channels_count(), mix_buffer(), mix_buffer_size() {
            this->system = system;
            master_volume = 1.0f;
            channels_volume[0] = 1.0f;
            channels_volume[1] = 1.0f;
            channels_volume[2] = 1.0f;
            channels_volume[3] = 1.0f;
        }

        Mixer::~Mixer() {
            Reset();
        }

        void Mixer::FillBuffer(void* buffer, size_t samples_count,
            bool disable_headphones_volume, bool invert_phase, AudioFormat format) {
            if (!system)
                return;

            vec4 spk_hph_volume = 0.0f;

            {
                std::unique_lock<std::mutex> u_lock(volume_mtx);
                spk_hph_volume = master_volume * *(vec4*)channels_volume;
            }

            if (disable_headphones_volume)
                *(vec2*)&spk_hph_volume.z = *(vec2*)&spk_hph_volume.x;
            else if (invert_phase)
                spk_hph_volume = -spk_hph_volume;

            memset(mix_buffer, 0, mix_buffer_size);
            if (streaming_channels)
                for (size_t i = 0; i < streaming_channels_count; i++)
                    streaming_channels[i].FillBuffer(mix_buffer, samples_count, 1.0f);

            if (se_channels)
                for (size_t i = 0; i < se_channels_count; i++)
                    se_channels[i].FillBuffer(mix_buffer, samples_count, 1.0f);

            sound_buffer_data* _mix_buffer = mix_buffer;
            switch (format) {
            case AUDIO_FORMAT_I16: {
                int16_t* _buffer = (int16_t*)buffer;
                switch (system->channels) {
                case 2:
                    for (size_t i = samples_count; i; i--, _mix_buffer++) {
                        vec2 hph = *(vec2*)&_mix_buffer->hph_l * *(vec2*)&spk_hph_volume.z * (float_t)0x7FFF;
                        vec2i hph_i32;
                        vec2_to_vec2i(hph, hph_i32);
                        *(vec2i*)_buffer = vec2i::clamp(hph_i32, -0x8000, 0x7FFF);
                        _buffer += 2;
                    }
                    break;
                case 4:
                    for (size_t i = samples_count; i; i--, _mix_buffer++) {
                        vec4 spk_hph = *(vec4*)_mix_buffer * spk_hph_volume * (float_t)0x7FFF;
                        vec4i spk_hph_i32;
                        vec4_to_vec4i(spk_hph, spk_hph_i32);
                        *(vec4i*)_buffer = vec4i::clamp(spk_hph_i32, -0x8000, 0x7FFF);
                        _buffer += 4;
                    }
                    break;
                }
            } break;
            case AUDIO_FORMAT_I24: {
                int8_t* _buffer = (int8_t*)buffer;
                switch (system->channels) {
                case 2:
                    for (size_t i = samples_count; i; i--, _mix_buffer++) {
                        vec2 hph = *(vec2*)&_mix_buffer->hph_l * *(vec2*)&spk_hph_volume.z * (float_t)0x7FFFFF;
                        vec2i hph_i32;
                        vec2_to_vec2i(hph, hph_i32);
                        hph_i32 = vec2i::clamp(hph_i32, -0x800000, 0x7FFFFF);
                        *(uint16_t*)&_buffer[0] = (uint16_t)hph_i32.x;
                        *(uint8_t*)&_buffer[2] = (uint8_t)(hph_i32.x >> 16);
                        *(uint16_t*)&_buffer[3] = (uint16_t)hph_i32.y;
                        *(uint8_t*)&_buffer[5] = (uint8_t)(hph_i32.y >> 16);
                        _buffer += 6;
                    }
                    break;
                case 4:
                    for (size_t i = samples_count; i; i--, _mix_buffer++) {
                        vec4 spk_hph = *(vec4*)_mix_buffer * spk_hph_volume * (float_t)0x7FFFFF;
                        vec4i spk_hph_i32;
                        vec4_to_vec4i(spk_hph, spk_hph_i32);
                        spk_hph_i32 = vec4i::clamp(spk_hph_i32, -0x800000, 0x7FFFFF);
                        *(uint16_t*)&_buffer[0] = (uint16_t)spk_hph_i32.x;
                        *(uint8_t*)&_buffer[2] = (uint8_t)(spk_hph_i32.x >> 16);
                        *(uint16_t*)&_buffer[3] = (uint16_t)spk_hph_i32.y;
                        *(uint8_t*)&_buffer[5] = (uint8_t)(spk_hph_i32.y >> 16);
                        *(uint16_t*)&_buffer[6] = (uint16_t)spk_hph_i32.z;
                        *(uint8_t*)&_buffer[8] = (uint8_t)(spk_hph_i32.z >> 16);
                        *(uint16_t*)&_buffer[9] = (uint16_t)spk_hph_i32.w;
                        *(uint8_t*)&_buffer[11] = (uint8_t)(spk_hph_i32.w >> 16);
                        _buffer += 12;
                    }
                    break;
                }
            } break;
            case AUDIO_FORMAT_I32: {
                int32_t* _buffer = (int32_t*)buffer;
                switch (system->channels) {
                case 2:
                    for (size_t i = samples_count; i; i--, _mix_buffer++) {
                        vec2 hph = *(vec2*)&_mix_buffer->hph_l * *(vec2*)&spk_hph_volume.z;
                        int64_t hph_l = (int64_t)((double_t)hph.x * (double_t)0x7FFFFFFF);
                        int64_t hph_r = (int64_t)((double_t)hph.y * (double_t)0x7FFFFFFF);
                        *_buffer++ = (int32_t)clamp_def(hph_l, -0x80000000LL, 0x7FFFFFFFLL);
                        *_buffer++ = (int32_t)clamp_def(hph_r, -0x80000000LL, 0x7FFFFFFFLL);
                    }
                    break;
                case 4:
                    for (size_t i = samples_count; i; i--, _mix_buffer++) {
                        vec4 spk_hph = *(vec4*)_mix_buffer * spk_hph_volume;
                        int64_t spk_l = (int64_t)((double_t)spk_hph.x * (double_t)0x7FFFFFFF);
                        int64_t spk_r = (int64_t)((double_t)spk_hph.y * (double_t)0x7FFFFFFF);
                        int64_t hph_l = (int64_t)((double_t)spk_hph.z * (double_t)0x7FFFFFFF);
                        int64_t hph_r = (int64_t)((double_t)spk_hph.w * (double_t)0x7FFFFFFF);
                        *_buffer++ = (int32_t)clamp_def(spk_l, -0x80000000LL, 0x7FFFFFFFLL);
                        *_buffer++ = (int32_t)clamp_def(spk_r, -0x80000000LL, 0x7FFFFFFFLL);
                        *_buffer++ = (int32_t)clamp_def(hph_l, -0x80000000LL, 0x7FFFFFFFLL);
                        *_buffer++ = (int32_t)clamp_def(hph_r, -0x80000000LL, 0x7FFFFFFFLL);
                    }
                    break;
                }
            } break;
            case AUDIO_FORMAT_F32: {
                float_t* _buffer = (float_t*)buffer;
                switch (system->channels) {
                case 2:
                    for (size_t i = samples_count; i; i--, _mix_buffer++, _buffer += 2)
                        *(vec2*)_buffer = *(vec2*)&_mix_buffer->hph_l * *(vec2*)&spk_hph_volume.z;
                    break;
                case 4:
                    for (size_t i = samples_count; i; i--, _mix_buffer++, _buffer += 4)
                        *(vec4*)_buffer = *(vec4*)&_mix_buffer * spk_hph_volume;
                    break;
                }
            } break;
            }
        }

        SEChannel* Mixer::GetSEChannel(size_t channel) {
            if (channel < se_channels_count && se_channels)
                return &se_channels[channel];
            return 0;
        }

        StreamingChannel* Mixer::GetStreamingChannel(size_t channel) {
            if (channel < streaming_channels_count && streaming_channels)
                return &streaming_channels[channel];
            return 0;
        }

        bool Mixer::Init(size_t se_channels_count, size_t streaming_channels_count, size_t samples_count) {
            if (this->se_channels_count || this->streaming_channels_count) {
                Reset();
                return false;
            }

            mix_buffer_size = sizeof(sound_buffer_data) * samples_count;
            mix_buffer = (sound_buffer_data*)malloc(mix_buffer_size);
            if (!mix_buffer) {
                Reset();
                return false;
            }
            memset(mix_buffer, 0, mix_buffer_size);

            if (se_channels_count) {
                se_channels = new SEChannel[se_channels_count];
                if (!se_channels) {
                    Reset();
                    return false;
                }

                for (size_t i = 0; i < se_channels_count; i++)
                    se_channels[i].Init(this);
            }
            this->se_channels_count = se_channels_count;

            if (streaming_channels_count) {
                streaming_channels = new StreamingChannel[streaming_channels_count];
                if (!streaming_channels) {
                    Reset();
                    return false;
                }

                for (size_t i = 0; i < streaming_channels_count; i++)
                    streaming_channels[i].Init(this, samples_count);
            }
            this->streaming_channels_count = streaming_channels_count;
            return true;
        }

        void Mixer::Reset() {
            if (streaming_channels) {
                for (size_t i = 0; i < streaming_channels_count; i++)
                    streaming_channels[i].Reset();

                delete[] streaming_channels;
                streaming_channels = 0;
            }
            streaming_channels_count = 0;

            if (se_channels) {
                for (size_t i = 0; i < se_channels_count; i++)
                    se_channels[i].Reset();

                delete[] se_channels;
                se_channels = 0;
            }
            se_channels_count = 0;

            if (mix_buffer) {
                free(mix_buffer);
                mix_buffer = 0;
            }
            mix_buffer_size = 0;
        }

        void Mixer::SetChannelsVolume(int32_t mask, float_t value) {
            std::unique_lock<std::mutex> u_lock(volume_mtx);
            for (int32_t i = 0; i < 4; i++)
                if (mask & (1 << i))
                    channels_volume[i] = value;
        }

        void Mixer::SetMasterVolume(float_t value) {
            std::unique_lock<std::mutex> u_lock(volume_mtx);
            master_volume = value;
        }

        SEChannel::SEChannel() : mixer(), buffer(), buffer_size(), channels(),
            samples_count(), sample_rate(), loop_start(), loop_end(), current_sample() {
            master_volume = 1.0f;
            channels_volume[0] = 1.0f;
            channels_volume[1] = 1.0f;
            channels_volume[2] = 1.0f;
            channels_volume[3] = 1.0f;
        }

        SEChannel::~SEChannel() {
            Reset();
        }

        void SEChannel::FillBuffer(sound_buffer_data* buffer, size_t samples_count, float_t volume) {
            std::unique_lock<std::mutex> u_lock(mtx);
            if (!play_state.get() || !buffer)
                return;

            float_t master_volume = this->master_volume;
            float_t spk_l_volume = master_volume * channels_volume[0];
            float_t spk_r_volume = master_volume * channels_volume[1];
            float_t hph_l_volume = master_volume * channels_volume[2];
            float_t hph_r_volume = master_volume * channels_volume[3];

            vec4 channels_volume(spk_l_volume, spk_r_volume, hph_l_volume, hph_r_volume);

            float_t* se_buffer = this->buffer;
            size_t _samples_count = this->samples_count;
            size_t loop_start = this->loop_start;
            size_t loop_end = this->loop_end;
            size_t current_sample = this->current_sample;
            bool loop = !!(loop_start || loop_end);
            if (channels == 1)
                for (size_t i = samples_count; i && current_sample < _samples_count; i--, buffer++) {
                    float_t* _buffer = se_buffer + current_sample;
                    *(vec4*)buffer = channels_volume * _buffer[0] + *(vec4*)buffer;

                    current_sample++;
                    if (loop && current_sample > loop_end)
                        current_sample = loop_start;
                }
            else if (channels == 2)
                for (size_t i = samples_count; i && current_sample < _samples_count; i--, buffer++) {
                    float_t* _buffer = se_buffer + current_sample * 2;
                    *(vec4*)buffer = channels_volume * vec4(_buffer[0], _buffer[1],
                        _buffer[0], _buffer[1]) + *(vec4*)buffer;

                    current_sample++;
                    if (loop && current_sample > loop_end)
                        current_sample = loop_start;
                }
            else if (channels == 4)
                for (size_t i = samples_count; i && current_sample < _samples_count; i--, buffer++) {
                    float_t* _buffer = se_buffer + current_sample * 4;
                    *(vec4*)buffer = channels_volume * *(vec4*)_buffer + *(vec4*)buffer;

                    current_sample++;
                    if (loop && current_sample > loop_end)
                        current_sample = loop_start;
                }

            this->current_sample = current_sample;

            if (current_sample >= _samples_count)
                ResetData();
        }

        bool SEChannel::Init(Mixer* mixer) {
            bool ret = false;
            {
                std::unique_lock<std::mutex> u_lock(mtx);
                if (!this->mixer) {
                    this->mixer = mixer;
                    buffer = 0;
                    buffer_size = 0;
                    ret = true;
                }
            }

            if (!ret)
                Reset();
            return ret;
        }

        float_t* SEChannel::InitBuffer(size_t channels, size_t samples_count, size_t sample_rate) {
            std::unique_lock<std::mutex> u_lock(mtx);
            ResetData();
            buffer = (float_t*)malloc(sizeof(float_t) * samples_count * channels);
            if (!buffer)
                return 0;

            buffer_size = sizeof(float_t) * samples_count * channels;
            memset(buffer, 0, buffer_size);
            this->channels = channels;
            this->sample_rate = sample_rate;
            this->samples_count = samples_count;
            loop_start = 0;
            loop_end = 0;
            return buffer;
        }

        bool SEChannel::Play(size_t loop_start, size_t loop_end) {
            std::unique_lock<std::mutex> u_lock(mtx);
            if (!buffer)
                return false;

            this->loop_start = loop_start;
            this->loop_end = loop_end;
            current_sample = 0;
            play_state.set(1);
            return true;
        }

        void SEChannel::Reset() {
            std::unique_lock<std::mutex> u_lock(mtx);
            ResetData();
        }

        void SEChannel::ResetData() {
            play_state.set(0);
            if (buffer) {
                free(buffer);
                buffer = 0;
            }
            buffer_size = 0;
            channels = 0;
            samples_count = 0;
            sample_rate = 0;
            loop_start = 0;
            loop_end = 0;
            current_sample = 0;
        }

        void SEChannel::ResetDataProt() {
            if (!play_state.get())
                return;

            std::unique_lock<std::mutex> u_lock(mtx);
            ResetData();
        }

        void SEChannel::SetChannelsVolume(int32_t mask, float_t value) {
            std::unique_lock<std::mutex> u_lock(mtx);
            for (int32_t i = 0; i < 4; i++)
                if (mask & (1 << i))
                    channels_volume[i] = value;
        }

        void SEChannel::SetMasterVolume(float_t value) {
            std::unique_lock<std::mutex> u_lock(mtx);
            master_volume = value;
        }

        StreamingChannel::StreamingChannel() : mixer(), buffer(),
            buffer_size(), callback_func(), callback_data() {
            master_volume = 1.0f;
            channels_volume[0] = 1.0f;
            channels_volume[1] = 1.0f;
            channels_volume[2] = 1.0f;
            channels_volume[3] = 1.0f;
        }

        StreamingChannel::~StreamingChannel() {
            Reset();
        }

        void StreamingChannel::FillBuffer(sound_buffer_data* buffer, size_t samples_count, float_t volume) {
            if (playing_state.get() && reset_state.get()) {
                std::unique_lock<std::mutex> u_lock(mtx);
                ResetData();
            }

            if (!playing_state.get() || !this->buffer || !buffer)
                return;

            float_t spk_l_volume = 0.0f;
            float_t spk_r_volume = 0.0f;
            float_t hph_l_volume = 0.0f;
            float_t hph_r_volume = 0.0f;
            {
                std::unique_lock<std::mutex> u_lock(mtx);
                if (!callback_func)
                    return;

                memset(this->buffer, 0, buffer_size);

                callback_func(this->buffer, samples_count, callback_data);

                float_t master_volume = this->master_volume;
                spk_l_volume = master_volume * channels_volume[0];
                spk_r_volume = master_volume * channels_volume[1];
                hph_l_volume = master_volume * channels_volume[2];
                hph_r_volume = master_volume * channels_volume[3];
            }

            vec4 channels_volume(spk_l_volume, spk_r_volume, hph_l_volume, hph_r_volume);

            sound_buffer_data* _buffer = this->buffer;
            for (size_t i = samples_count / 4; i; i--, buffer += 4, _buffer += 4) {
                ((vec4*)buffer)[0] = channels_volume * ((vec4*)_buffer)[0] + ((vec4*)buffer)[0];
                ((vec4*)buffer)[1] = channels_volume * ((vec4*)_buffer)[1] + ((vec4*)buffer)[1];
                ((vec4*)buffer)[2] = channels_volume * ((vec4*)_buffer)[2] + ((vec4*)buffer)[2];
                ((vec4*)buffer)[3] = channels_volume * ((vec4*)_buffer)[3] + ((vec4*)buffer)[3];
            }

            for (size_t i = samples_count % 4; i; i--, buffer++, _buffer++)
                *(vec4*)buffer = channels_volume * *(vec4*)_buffer + *(vec4*)buffer;
        }

        bool StreamingChannel::Init(Mixer* mixer, size_t samples_count) {
            if (this->mixer) {
                Reset();
                return false;
            }

            this->mixer = mixer;
            buffer_size = sizeof(sound_buffer_data) * samples_count;
            buffer = (sound_buffer_data*)malloc(buffer_size);
            if (!buffer) {
                Reset();
                return false;
            }

            memset(buffer, 0, buffer_size);
            return true;
        }

        void StreamingChannel::Reset() {
            reset_state.set(1);
            FillBuffer(0, 0, 0.0f);
            if (buffer) {
                free(buffer);
                buffer = 0;
            }
            mixer = 0;
            buffer_size = 0;
        }

        void StreamingChannel::ResetData() {
            playing_state.set(0);
            reset_state.set(0);
            callback_func = 0;
            callback_data = 0;
        }

        bool StreamingChannel::SetCallback(void(*func)(sound_buffer_data* buffer,
            size_t samples_count, void* data), void* data) {
            std::unique_lock<std::mutex> u_lock(mtx);
            ResetData();
            callback_func = func;
            callback_data = data;
            playing_state.set(1);
            return true;
        }

        void StreamingChannel::SetChannelsVolume(int32_t mask, float_t value) {
            std::unique_lock<std::mutex> u_lock(mtx);
            for (int32_t i = 0; i < 4; i++)
                if (mask & (1 << i))
                    channels_volume[i] = value;
        }

        void StreamingChannel::SetMasterVolume(float_t value) {
            std::unique_lock<std::mutex> u_lock(mtx);
            master_volume = value;
        }
    }
}

sound_db_farc::sound_db_farc() : ready() {

}

sound_db_farc::~sound_db_farc() {

}

bool sound_db_farc::load() {
    if (ready)
        return false;
    else if (file_handler.check_not_ready())
        return true;

    farc.read(file_handler.get_data(), file_handler.get_size());
    if (farc.files.size() && sound_work->ParseProperty(this))
        ready = true;
    return false;
}

bool sound_db_farc::read(const char* file_path) {
    if (ready)
        unload();

    this->file_path.assign(file_path);
    return file_handler.read_file(&data_list[DATA_AFT], file_path);
}

bool sound_db_farc::unload() {
    sound_work_release_farc_se(file_path.c_str());
    if (!sound_work->UnloadProperty(file_path.c_str()))
        return false;

    file_path.clear();
    ready = false;
    file_handler.reset();
    return true;
}

sound_db_property::sound_db_property() : farc(),
volume(), loop_start(), loop_end(), release_time()/*, field_48()*/ {

}

sound_db_property::~sound_db_property() {

}

SoundCue::SoundCue() : thread(), property(), counter(), release_time(), se_channel() {
    queue_index = SOUND_WORK_SE_QUEUE_COUNT;
    volume = new SoundCueVolume(this);
}

SoundCue::~SoundCue() {
    if (thread) {
        thread_state.set(1);
        cnd.notify_one();
        thread->join();
        delete thread;
        thread = 0;
    }

    if (volume) {
        delete volume;
        volume = 0;
    }
}

bool SoundCue::CanPlay() {
    if (se_channel && se_channel->play_state.get())
        return true;
    return !!load_state.get();
}

void SoundCue::Ctrl() {
    std::unique_lock<std::mutex> u_lock(mtx);
    while (!thread_state.get()) {
        if (!data_state.get())
            cnd.wait(u_lock);

        int32_t _data_state = data_state.get();
        data_state.set(SOUND_CUE_DATA_STATE_NONE);
        switch (_data_state) {
        case SOUND_CUE_DATA_STATE_LOAD:
            LoadData();
            break;
        case SOUND_CUE_DATA_STATE_RESET:
            ResetData();
            break;
        }
    }
    thread_state.set(0);
}

int32_t SoundCue::Load(int32_t queue_index, const char* name, float_t volume) {
    ReleaseProt(true);
    std::unique_lock<std::mutex> u_lock(mtx);
    if (!se_channel)
        return 0;

    sound_db_property* property = sound_work->FindProperty(name);
    if (!property)
        return 0;

    this->name.assign(name);
    this->property = property;
    this->queue_index = queue_index;
    this->volume->SetValue(volume);

    Play();
    load_state.set(1);
    data_state.set(SOUND_CUE_DATA_STATE_LOAD);
    cnd.notify_one();
    counter = sound_work->counter;
    if (++sound_work->counter <= -1)
        sound_work->counter = 1;
    return counter;
}

void SoundCue::LoadData() {
    WaveAudio* _wave_audio = 0;
    if (!property || !property->farc) {
        load_state.set(0);
        return;
    }

    WaveAudio* wave_audio = wave_audio_storage_get_wave_audio(name);
    if (!wave_audio) {
        _wave_audio = new WaveAudio;
        if (_wave_audio && _wave_audio->Read(property->farc, property->file_name.c_str()))
            wave_audio = _wave_audio;
    }

    if (wave_audio) {
        float_t* buffer = se_channel->InitBuffer(wave_audio->data.channels,
            wave_audio->data.samples_count, wave_audio->data.sample_rate);
        if (buffer) {
            memmove(buffer, wave_audio->buffer, wave_audio->buffer_size);
            se_channel->Play(property->loop_start, property->loop_end);
        }
    }

    if (_wave_audio) {
        _wave_audio->Reset();
        delete _wave_audio;
    }
    load_state.set(0);
}

void SoundCue::Play() {
    if (se_channel && !property)
        return;

    bool master_volume_set = false;
    float_t master_volume = property->volume * volume->value;

    if (volume->set) {
        volume->set = false;
        master_volume_set = true;
    }

    if (release_time > 0.0f) {
        double_t release_time = this->release_time * 1000.0;
        double_t current_time = time.calc_time();
        double_t remain_time;
        if (current_time >= release_time) {
            remain_time = 0.0;
            Release(true);
        }
        else
            remain_time = release_time - current_time;

        master_volume_set = true;
        master_volume = (float_t)(remain_time / release_time) * master_volume;
    }

    if (se_channel) {
        if (master_volume_set)
            se_channel->SetMasterVolume(master_volume);

        if (!queue_index)
            se_channel->SetChannelsVolume(0x03, get_max_speakers_volume());
    }
    return;
}

void SoundCue::PlayProt() {
    std::unique_lock<std::mutex> u_lock(mtx);
    Play();
}

void SoundCue::Release(bool force_release) {
    if (!property || fabsf(property->release_time) <= 0.000001f || force_release) {
        data_state.set(SOUND_CUE_DATA_STATE_RESET);
        cnd.notify_one();
        queue_index = SOUND_WORK_SE_QUEUE_COUNT;
        name.clear();
        property = 0;
        counter = 0;
        release_time = 0.0f;
    }
    else
        release_time = property->release_time;

    time.get_timestamp();
}

void SoundCue::ReleaseProt(bool force_release) {
    std::unique_lock<std::mutex> u_lock(mtx);
    Release(force_release);
}

void SoundCue::Reset() {
    ReleaseProt(true);

    if (thread) {
        thread_state.set(1);
        cnd.notify_one();
        thread->join();
        delete thread;
        thread = 0;
    }
}

void SoundCue::ResetData() {
    if (se_channel)
        se_channel->ResetDataProt();
}

void SoundCue::SetSEChannel(sound::wasapi::SEChannel* se_channel) {
    this->se_channel = se_channel;
    if (se_channel)
        se_channel->SetMasterVolume(1.0f);

    thread = new std::thread(SoundCue::ThreadMain, this);
    if (thread) {
        wchar_t buf[0x80];
        swprintf_s(buf, sizeof(buf) / sizeof(wchar_t), L"SoundCue #%d", sound_cue_counter++);
        SetThreadDescription((HANDLE)thread->native_handle(), buf);
    }
}

void SoundCue::ThreadMain(SoundCue* cue) {
    if (cue)
        cue->Ctrl();
}

WaveAudioData::WaveAudioData() : format(), channels(), sample_rate(), bit_depth(),
samples_count(), position(), data_size(), has_loop(), loop_start(), loop_end() {

}

WaveAudioData::~WaveAudioData() {

}

bool WaveAudioData::Read(WaveAudioDataFileMemoryStream* fms) {
    Reset();
    return ReadDiva(fms)/* || ReadRiff(fms)*/;
}

bool WaveAudioData::ReadData(const void* data, size_t size) {
    if (!this || !data || !size)
        return false;

    WaveAudioDataFileMemoryStream fms(data, size);
    return Read(&fms);
}

bool WaveAudioData::ReadDiva(WaveAudioDataFileMemoryStream* fms) {
    struct DivaAudioHeader {
        uint32_t signature;
        uint8_t pad_4[4];
        uint32_t size;
        uint32_t sample_rate;
        uint32_t samples_count;
        uint32_t loop_start;
        uint32_t loop_end;
        uint8_t channels;
        uint8_t pad_1D[3];
        uint8_t reserved1[32];
    } header;

    if (fms->file) {
        if (fseek(fms->file, 0, SEEK_SET))
            return false;
    }
    else {
        if (!fms->data || !fms->size)
            return false;
        fms->position = 0;
    }

    size_t read_bytes = 0;
    if (fms->file)
        read_bytes = fread(&header, sizeof(DivaAudioHeader), 1, fms->file);
    else if (fms->data && fms->size && fms->position + sizeof(DivaAudioHeader) <= fms->size) {
        memmove(&header, (uint8_t*)fms->data + fms->position, sizeof(DivaAudioHeader));
        read_bytes = sizeof(DivaAudioHeader);
    }

    if (!read_bytes || header.signature != reverse_endianness_uint32_t('DIVA'))
        return false;

    format = WAVE_AUDIO_DATA_FORMAT_DIVA;
    channels = header.channels;
    sample_rate = header.sample_rate;
    bit_depth = 0;
    samples_count = header.samples_count;
    position = 0;
    data_size = 0;
    has_loop = header.loop_start < header.loop_end;
    loop_start = header.loop_start;
    loop_end = header.loop_end;
    return true;
}

void WaveAudioData::Reset() {
    format = WAVE_AUDIO_DATA_FORMAT_NONE;
    channels = 0;
    sample_rate = 0;
    bit_depth = 0;
    samples_count = 0;
    position = 0;
    data_size = 0;
    has_loop = false;
    loop_start = 0;
    loop_end = 0;
}

WaveAudio::WaveAudio() : buffer(), buffer_size() {

}

bool WaveAudio::Read(sound_db_farc* farc, const char* file_name) {
    if (!this)
        return false;

    if (!farc) {
        Reset();
        return false;
    }

    ::farc* f = &farc->farc;
    farc_file* ff = f->read_file(file_name);
    if (!ff || !ff->data || !ff->size) {
        Reset();
        return false;
    }

    data.ReadData(ff->data, ff->size);

    size_t buffer_size = sizeof(float_t) * data.samples_count * data.channels;
    float_t* buffer = (float_t*)malloc(buffer_size);
    this->buffer_size = buffer_size;
    this->buffer = buffer;

    if (!buffer) {
        Reset();
        return false;
    }

    bool ret = false;
    memset(buffer, 0, buffer_size);
    if (data.format == WAVE_AUDIO_DATA_FORMAT_DIVA) {
        size_t buf_size = sizeof(int16_t) * data.samples_count * data.channels;
        int16_t* buf = (int16_t*)malloc(buf_size);
        if (!buf) {
            Reset();
            return false;
        }

        memset(buf, 0, buf_size);

        {
            std::vector<ima_storage> storage(data.channels);
            data.samples_count = ima_decode(buf, buf_size, (uint8_t*)ff->data + data.position,
                data.samples_count, data.channels, storage.data(), storage.size());
        }

        int16_t* _buf = buf;
        float_t* buffer = this->buffer;
        size_t channels = data.channels;
        size_t samples_count = data.samples_count;
        if (channels == 1)
            for (size_t i = samples_count; i; i--, buffer++, _buf++)
                *buffer = (float_t)*_buf * (float_t)(1.0 / (double_t)0x7FFF);
        else if (channels == 2)
            for (size_t i = samples_count; i; i--, buffer += 2, _buf += 2) {
                vec2 value;
                vec2i16_to_vec2(*(vec2i16*)_buf, value);
                *(vec2*)buffer = value * (float_t)(1.0 / (double_t)0x7FFF);
            }
        else if (channels == 4)
            for (size_t i = samples_count; i; i--, buffer += 4, _buf += 4) {
                vec4 value;
                vec4i16_to_vec4(*(vec4i16*)_buf, value);
                *(vec4*)buffer = value * (float_t)(1.0 / (double_t)0x7FFF);
            }
        else
            for (size_t i = samples_count; i; i--)
                for (size_t j = channels; j; j--, buffer++, _buf++)
                    *buffer++ = (float_t)*_buf++ * (float_t)(1.0 / (double_t)0x7FFF);

        free(buf);
        ret = true;
    }

    if (!ret) {
        Reset();
        return false;
    }
    return true;
}

void WaveAudio::Reset() {
    if (buffer) {
        free(buffer);
        buffer = 0;
    }
    buffer_size = 0;
    data.Reset();
}

SoundWork::SoundWork() : se_queue_enable(), stream_enable(), se_queue_volume() {
    counter = 1;
    speakers_volume = get_max_speakers_volume();
    speakers_volume_changed = true;
    headphones_volume = get_min_headphones_volume();
    headphones_volume_changed = true;
    se_volume = 1.0f;

    for (bool& i : se_queue_enable)
        i = true;

    for (bool& i : stream_enable)
        i = true;

    for (int32_t i = 0; i < SOUND_WORK_SE_QUEUE_COUNT; i++)
        se_queue_volume[i] = sound_cue_queue_volume_array_get_max(i);
}

SoundWork::~SoundWork() {

}

sound_db_property* SoundWork::FindProperty(const char* name) {
    auto elem = properties.find(name);
    if (elem != properties.end())
        return &elem->second;
    return 0;
}

bool SoundWork::ParseProperty(sound_db_farc* snd_db_farc) {
    if (!snd_db_farc)
        return false;

    farc& f = snd_db_farc->farc;
    farc_file* property_ff = f.read_file("property.txt");
    if (!property_ff || !property_ff->data || !property_ff->size)
        return false;

    key_val kv;
    kv.parse(property_ff->data, property_ff->size);

    int32_t max;
    float_t volume_bias;
    if (!kv.read("max", max) || !kv.read("volume_bias", volume_bias))
        return false;

    for (int32_t i = 0, j = 1; i < max; i++, j++) {
        if (!kv.open_scope_fmt("%zu", j))
            continue;

        std::string name;
        std::string file_name;
        if (!kv.read("name", name) || !kv.read("file_name", file_name)
            || !kv.has_key("volume") || !f.has_file(file_name.c_str())) {
            kv.close_scope();
            continue;
        }

        uint32_t loop_start = 0;
        if (kv.open_scope("loop_start")) {
            kv.read(loop_start);
            kv.close_scope();
        }

        uint32_t loop_end = 0;
        if (kv.open_scope("loop_end")) {
            kv.read(loop_end);
            kv.close_scope();
        }

        float_t release_time = 0.0f;
        if (kv.open_scope("release_time")) {
            kv.read(release_time);
            release_time *= 0.001f;
            kv.close_scope();
        }

        float_t volume = 0.0f;
        kv.read("volume", volume);

        sound_db_property property;
        property.farc = snd_db_farc;
        property.file_name.assign(file_name);
        property.volume = volume * volume_bias;
        property.loop_start = loop_start;
        property.loop_end = loop_end;
        property.release_time = release_time;
        properties.insert_or_assign(name, property);
        kv.close_scope();
    }
    return true;
}

bool SoundWork::UnloadProperty(const char* file_path) {
    for (auto i = properties.begin(); i != properties.end(); i++)
        if (!i->second.farc->file_path.compare(file_path)) {
            properties.erase(i);
            break;
        }
    return true;
}

sound_stream_info::sound_stream_info() : duration(), time() {

}

sound_stream_info::~sound_stream_info() {

}

float_t get_min_headphones_volume() {
    return 1.0f;//0.02f;
}

float_t get_max_headphones_volume() {
    return 0.45f;
}

float_t get_min_speakers_volume() {
    return 0.0f;
}

float_t get_max_speakers_volume() {
    return 0.8f;
}

float_t db_to_ratio(int32_t value) {
    if (value <= -10000)
        return 0.0f;
    else if (value >= 0)
        return 1.0f;
    else
        return powf(10.0, (float_t)value * 0.0005f);
}

int32_t ratio_to_db(float_t value) {
    if (value <= 0.0f)
        return -10000;
    else if (value >= 1.0f)
        return 0;
    else
        return (int32_t)(log10f(value) * 20.0f * 100.0f);
}

void sound_init() {
    IMMDeviceEnumerator* mmEnumerator;
    if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,
        CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&mmEnumerator))) {
        IMMDeviceCollection* collection;
        if (SUCCEEDED(mmEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &collection))) {
            UINT count;
            if (SUCCEEDED(collection->GetCount(&count))) {
                for (UINT i = 0; i < count; i++) {
                    IMMDevice* device;
                    if (FAILED(collection->Item(i, &device)))
                        continue;

                    wchar_t* wstrID;
                    if (SUCCEEDED(device->GetId((LPWSTR*)&wstrID))) {
                        IPropertyStore* store;
                        if (SUCCEEDED(device->OpenPropertyStore(STGM_READ, &store))) {
                            PROPVARIANT pv;

                            PropVariantInit(&pv);
                            if (SUCCEEDED(store->GetValue(PKEY_Device_FriendlyName, &pv)))
                                printf_debug("%ls\n", pv.pwszVal);
                            PropVariantClear(&pv);

                            store->Release();
                        }
                        CoTaskMemFree((LPVOID)wstrID);
                    }
                    device->Release();
                }
            }
            collection->Release();
        }
        mmEnumerator->Release();
    }

    if (!sound_work)
        sound_work = new SoundWork;

    if (!sound_wasapi_system_data)
        sound_wasapi_system_data = new sound::wasapi::System;

    sound_wasapi_system_data->Init(SOUND_WORK_SE_CHANNELS_COUNT, SOUND_WORK_STREAMING_CHANNELS_COUNT);

    sound::wasapi::Mixer* mixer = sound_wasapi_system_data->mixer;
    if (mixer) {
        mixer->SetMasterVolume(1.0f);
        mixer->SetChannelsVolume(0x03, 1.0f);
        mixer->SetChannelsVolume(0x0C, get_min_headphones_volume());

        size_t se_channels_count = mixer->se_channels_count;
        for (size_t i = 0; i < se_channels_count; i++) {
            sound::wasapi::SEChannel* se_channel = mixer->GetSEChannel(i);
            se_channel->SetChannelsVolume(0x03, get_max_speakers_volume());
            se_channel->SetChannelsVolume(0x0C, 1.0f);
        }

        size_t streaming_channels_count = mixer->streaming_channels_count;
        for (size_t i = 0; i < streaming_channels_count; i++) {
            sound::wasapi::StreamingChannel* streaming_channel = mixer->GetStreamingChannel(i);
            streaming_channel->SetChannelsVolume(0x03, get_max_speakers_volume());
            streaming_channel->SetChannelsVolume(0x0C, 1.0f);
        }

        SoundCue* sound_cue = sound_work->cues;
        for (size_t i = 0; i < se_channels_count
            && i < sizeof(sound_work->cues) / sizeof(SoundCue); i++, sound_cue++) {
           sound::wasapi::SEChannel* se_channel = mixer->GetSEChannel(i);
           if (se_channel)
                sound_cue->SetSEChannel(se_channel);
        }
    }

    sound_stream_array_init();
}

void sound_ctrl() {
    sound::wasapi::System* sound_wasapi_system = sound_wasapi_system_data_get();
    if (sound_wasapi_system) {
        sound::wasapi::Mixer* mixer = sound_wasapi_system->mixer;
        if (mixer) {
            if (sound_work->headphones_volume_changed)
                mixer->SetChannelsVolume(0x0C, sound_work_get_headphones_volume());

            if (sound_work->speakers_volume_changed) {
                size_t se_channels_count = mixer->se_channels_count;
                for (size_t i = 0; i < se_channels_count; i++) {
                    sound::wasapi::SEChannel* se_channel = mixer->GetSEChannel(i);
                    se_channel->SetChannelsVolume(0x03, sound_work_get_speakers_volume());
                }

                size_t streaming_channels_count = mixer->streaming_channels_count;
                for (size_t i = 0; i < streaming_channels_count; i++) {
                    sound::wasapi::StreamingChannel* streaming_channel = mixer->GetStreamingChannel(i);
                    streaming_channel->SetChannelsVolume(0x03, sound_work_get_speakers_volume());
                }
            }
        }
    }
    sound_work->speakers_volume_changed = false;
    sound_work->headphones_volume_changed = false;

    for (SoundCue& i : sound_work->cues) {
        if (i.CanPlay())
            i.PlayProt();
        else
            i.ReleaseProt(true);
    }

    sound_stream_array_ctrl();

    sound_work->names_list.clear();
}

void sound_free() {
    sound_stream_array_free();

    if (sound_work) {
        delete sound_work;
        sound_work = 0;
    }

    if (sound_wasapi_system_data) {
        delete sound_wasapi_system_data;
        sound_wasapi_system_data = 0;
    }
}

void sound_stream_array_reset() {
    for (size_t i = 0; i < SOUND_WORK_STREAM_COUNT; i++)
        sound_stream_array[i].reset();
}

bool sound_work_check_stream_state(int32_t index) {
    if (index < 0 || index >= SOUND_WORK_STREAM_COUNT || !sound_work->stream_enable[index])
        return false;

    sound_stream* stream = sound_stream_array_get(index);
    if (!stream)
        return false;

    return stream->check_state();
}

bool sound_work_cue_release(int32_t queue_index, const char* name, bool force_release) {
    if (!name)
        return false;

    SoundCue* cue = sound_work_get_cue(queue_index, name);
    if (cue) {
        cue->ReleaseProt(force_release);
        return true;
    }
    return false;
}

SoundCue* sound_work_get_cue(int32_t queue_index, const char* name) {
    for (SoundCue& i : sound_work->cues)
        if ((!i.se_channel || i.property) && i.queue_index == queue_index
            && !i.name.compare(name))
            return &i;
    return 0;
}

bool sound_work_get_cue_can_play(int32_t queue_index, const char* name) {
    if (!name)
        return false;

    SoundCue* cue = sound_work_get_cue(queue_index, name);
    if (cue)
        return cue->CanPlay();
    return false;
}

sound_db_farc* sound_work_get_farc(const char* file_path) {
    for (sound_db_farc& i : sound_work->farcs)
        if (!i.file_path.compare(file_path))
            return &i;
    return 0;
}

bool sound_work_get_stream_info(sound_stream_info& info, int32_t index) {
    if (index < 0 || index >= SOUND_WORK_STREAM_COUNT || !sound_work->stream_enable[index])
        return false;

    sound_stream* stream = sound_stream_array_get(index);
    if (!stream)
        return false;

    info.duration = stream->duration;
    info.time = stream->time;
    info.path.assign(stream->path);
    return true;
}

float_t sound_work_get_stream_volume(int32_t index) {
    if (index < 0 || index >= SOUND_WORK_STREAM_COUNT || !sound_work->stream_enable[index])
        return false;

    sound_stream* stream = sound_stream_array_get(index);
    if (!stream)
        return 1.0f;

    return stream->current_volume;
}

float_t sound_work_get_headphones_volume() {
    return sound_work->headphones_volume;
}

float_t sound_work_get_speakers_volume() {
    return sound_work->speakers_volume;
}

bool sound_work_has_property(const char* name) {
    return !!sound_work->FindProperty(name);
}

bool sound_work_load_farc(const char* file_path) {
    if (!file_path)
        return false;

    sound_db_farc* snd_db_farc = sound_work_get_farc(file_path);
    if (snd_db_farc)
        return snd_db_farc->load();
    return false;
}

int32_t sound_work_play_se(int32_t queue_index, const char* name, float_t volume) {
    if (!str_utils_compare(name, "sys_kettei_aif"))
        name = "sys_kettei.aif";

    bool found = false;
    for (std::string& i : sound_work->names_list)
        if (!i.compare(name)) {
            found = true;
            break;
        }

    if (found || queue_index < 0 || queue_index >= SOUND_WORK_SE_QUEUE_COUNT || !sound_work->se_queue_enable[queue_index])
        return 0;

    SoundCue* cue = 0;
    for (SoundCue& i : sound_work->cues)
        if (i.se_channel && !i.property) {
            cue = &i;
            break;
        }

    if (!cue) {
        int32_t min_counter = sound_work->counter;
        for (SoundCue& i : sound_work->cues)
            if (!i.name.compare(name) && min_counter > i.counter) {
                cue = &i;
                min_counter = i.counter;
            }

        if (!cue) {
            int32_t min_counter = sound_work->counter;
            for (SoundCue& i : sound_work->cues)
                if (min_counter > i.counter) {
                    cue = &i;
                    min_counter = i.counter;
                }

            if (!cue)
                return 0;
        }

        cue->ReleaseProt(true);
    }

    float_t cue_volume = volume * sound_work->se_queue_volume[queue_index];
    if (queue_index < 3 || queue_index > 4)
        cue_volume *= sound_work->se_volume;

    int32_t counter = cue->Load(queue_index, name, cue_volume);
    if (counter)
        sound_work->names_list.push_back(name);
    return counter;
}

bool sound_work_play_stream(int32_t index, const char* path, bool pause) {
    if (index < 0 || index >= SOUND_WORK_STREAM_COUNT || !sound_work->stream_enable[index])
        return false;

    sound_stream* stream = sound_stream_array_get(index);
    if (!stream)
        return false;

    return stream->set_path(path, pause);
}

bool sound_work_play_stream(int32_t index, const char* path, float_t time, bool pause) {
    if (index < 0 || index >= SOUND_WORK_STREAM_COUNT || !sound_work->stream_enable[index])
        return false;

    sound_stream* stream = sound_stream_array_get(index);
    if (!stream)
        return false;

    return stream->set_path(path, time, pause);
}

bool sound_work_read_farc(const char* file_path) {
    if (!file_path || sound_work_get_farc(file_path))
        return false;

    sound_db_farc* snd_db_farc = 0;
    for (sound_db_farc& i : sound_work->farcs)
        if (!i.file_path.size()) {
            snd_db_farc = &i;
            break;
        }

    if (snd_db_farc)
        return snd_db_farc->read(file_path);
    return false;
}

void sound_work_release_farc_se(const char* file_path) {
    if (!file_path)
        return;

    for (SoundCue& i : sound_work->cues) {
        if (i.se_channel && !i.property || i.property->farc)
            continue;

        sound_db_farc* snd_db_farc = i.property->farc;
        if (!snd_db_farc)
            continue;

        if (!snd_db_farc->farc.file_path.compare(file_path))
            i.ReleaseProt(true);
    }
}

bool sound_work_release_se(const char* name, bool release) {
    if (!name)
        return false;

    bool found = false;
    for (SoundCue& i : sound_work->cues)
        if ((!i.se_channel || i.property) && !i.name.compare(name)) {
            i.ReleaseProt(release);
            found = true;
        }
    return found;
}

bool sound_work_release_stream(int32_t index) {
    if (index < 0 || index >= SOUND_WORK_STREAM_COUNT || !sound_work->stream_enable[index])
        return false;

    sound_stream* stream = sound_stream_array_get(index);
    if (!stream)
        return false;

    return stream->stop();
}

void sound_work_reset_all_se() {
    for (SoundCue& i : sound_work->cues)
        if ((!i.se_channel || i.property) && i.queue_index)
            i.ReleaseProt(true);
}

bool sound_work_stream_set_pause(int32_t index, bool value) {
    if (index < 0 || index >= SOUND_WORK_STREAM_COUNT || !sound_work->stream_enable[index])
        return false;

    sound_stream* stream = sound_stream_array_get(index);
    if (!stream)
        return false;

    return stream->set_pause(value);
}

float_t sound_cue_queue_volume_array_get_max(int32_t queue_index) {
    if (queue_index >= 0 && queue_index < SOUND_WORK_SE_QUEUE_COUNT)
        return sound_cue_queue_volume_array[queue_index].max;
    return 1.0f;
}

float_t sound_cue_queue_volume_array_get_min(int32_t queue_index) {
    if (queue_index >= 0 && queue_index < SOUND_WORK_SE_QUEUE_COUNT)
        return sound_cue_queue_volume_array[queue_index].min;
    return 0.0f;
}

sound::wasapi::System* sound_wasapi_system_data_get() {
    return sound_wasapi_system_data;
}

void sound_work_set_headphones_volume(float_t value) {
    float_t min = get_min_headphones_volume();
    float_t max = get_max_headphones_volume();
    value = clamp_def(value, min, max);
    if (fabsf(sound_work->headphones_volume - value) > 0.000001f) {
        sound_work->headphones_volume = value;
        sound_work->headphones_volume_changed = true;
    }
}

void sound_work_set_se_queue_volume(int32_t queue_index, float_t value) {
    if (queue_index < 0 || queue_index >= SOUND_WORK_SE_QUEUE_COUNT)
        return;

    const SoundCueQueueVolume& vol = sound_cue_queue_volume_array[queue_index];
    sound_work->se_queue_volume[queue_index] = clamp_def(value, vol.min, vol.max);
}

void sound_work_set_speakers_volume(float_t value) {
    float_t min = get_min_speakers_volume();
    float_t max = get_max_speakers_volume();
    value = clamp_def(value, min, max);
    if (fabsf(sound_work->speakers_volume - value) > 0.000001f) {
        sound_work->speakers_volume = value;
        sound_work->speakers_volume_changed = true;
    }
}

bool sound_work_set_stream_current_volume(int32_t index, float_t value) {
    if (index < 0 || index >= SOUND_WORK_STREAM_COUNT || !sound_work->stream_enable[index])
        return false;

    sound_stream* stream = sound_stream_array_get(index);
    if (!stream)
        return false;

    stream->set_current_volume(value);
    return true;
}

bool sound_work_set_stream_target_volume(int32_t index, float_t value, int32_t frames) {
    if (index < 0 || index >= SOUND_WORK_STREAM_COUNT || !sound_work->stream_enable[index])
        return false;

    sound_stream* stream = sound_stream_array_get(index);
    if (!stream)
        return false;

    stream->set_target_volume(value, frames);
    return true;
}

bool sound_work_unload_farc(const char* file_path) {
    if (!file_path)
        return false;

    sound_db_farc* snd_db_farc = sound_work_get_farc(file_path);
    if (snd_db_farc)
        return snd_db_farc->unload();
    return false;
}

void wave_audio_storage_init() {
    wave_audio_storage_data = {};
}

void wave_audio_storage_clear() {
    for (auto& i : wave_audio_storage_data)
        i.second.Reset();

    wave_audio_storage_data.clear();
}

WaveAudio* wave_audio_storage_get_wave_audio(const std::string& name) {
    auto elem = wave_audio_storage_data.find(name);
    if (elem != wave_audio_storage_data.end())
        return &elem->second;
    return 0;
}

bool wave_audio_storage_load_wave_audio(const std::string& name) {
    wave_audio_storage_unload_wave_audio(name);

    sound_db_property* property = sound_work->FindProperty(name.c_str());
    if (!property)
        return false;

    WaveAudio wave_audio;
    if (wave_audio.Read(property->farc, property->file_name.c_str())) {
        wave_audio_storage_data.insert({ name, wave_audio });
        return true;
    }
    return false;
}

void wave_audio_storage_unload_wave_audio(const std::string& name) {
    auto elem = wave_audio_storage_data.find(name);
    if (elem != wave_audio_storage_data.end()) {
        elem->second.Reset();
        wave_audio_storage_data.erase(elem);
    }
}

void wave_audio_storage_free() {
    for (auto& i : wave_audio_storage_data)
        i.second.Reset();

    wave_audio_storage_data.clear();
}

static size_t ima_decode(int16_t* dst, size_t dst_size, uint8_t* data, size_t samples_count,
    size_t channels, ima_storage* storage, size_t storage_size) {
    if (!dst || !dst_size || !data || !samples_count || !storage || channels > storage_size)
        return 0;

    bool odd_sample = false;
    size_t act_samp_count = 0;
    for (size_t i = min_def(samples_count, dst_size / sizeof(int16_t)); i; i--, act_samp_count++) {
        for (size_t j = channels, k = 0; j; j--, k++, odd_sample ^= true) {
            ima_storage& _storage = storage[k];
            int16_t step = ima_step_table[_storage.step_index];

            uint8_t nibble = (odd_sample ? *data : (*data >> 4)) & 0x0F;

            int32_t diff = step >> 3;
            if (nibble & 0x01)
                diff += step >> 2;
            if (nibble & 0x02)
                diff += step >> 1;
            if (nibble & 0x04)
                diff += step;
            if (nibble & 0x08)
                diff = -diff;

            int32_t current = _storage.current + diff;
            current = clamp_def(current, -0x8000, 0x7FFF);
            _storage.current = current;

            int32_t step_index = _storage.step_index + ima_index_table[nibble];
            _storage.step_index = clamp_def(step_index, 0, 88);

            *dst++ = current;

            if (odd_sample)
                data++;
        }
    }
    return act_samp_count;
}

sound_stream::sound_stream() : ogg_playback(), duration(), pause(), file_loading_frames(),
file_ready_frames(), load_time_seek(), volume_trans(), state(), play_state(), time() {
    current_volume = 1.0f;
    target_volume = 1.0f;
}

sound_stream::~sound_stream() {

}

bool sound_stream::check_state() {
    if (play_state == 1)
        return true;
    return state == 1;
}

void sound_stream::ctrl() {
    if (!check_state())
        return;

    if (play_state == 1 && volume_trans > 0) {
        current_volume += (target_volume - current_volume) / (float_t)volume_trans;
        volume_trans--;
    }

    if (path.size() < 4)
        return;

    std::string ext;
    ext.assign(path.c_str() + (path.size() - 4), 4);
    for (char& c : ext)
        if (c >= 'A' && c <= 'Z')
            c += 0x20;

    if (ext.compare(".ogg"))
        return;

    if (!ogg_playback) {
        size_t index = 0;
        for (size_t i = 0; i < SOUND_WORK_STREAM_COUNT; i++)
            if (&sound_stream_array[i] == this) {
                index = i;
                break;
            }

        ogg_playback = ogg_playback_data_get(index);
    }

    if (ogg_playback)
        ctrl_playback();
}

void sound_stream::ctrl_playback() {
    OggFileHandlerFileState file_state = OGG_FILE_HANDLER_FILE_STATE_NONE;
    if (ogg_playback) {
        file_state = ogg_playback->GetFileState();
        ogg_playback->SetMasterVolume(ratio_to_db(current_volume));
    }

    duration = 0.0f;
    time = 0.0f;
    play_state = 0;

    if (file_state == OGG_FILE_HANDLER_FILE_STATE_NONE
        || (file_state >= OGG_FILE_HANDLER_FILE_STATE_STOPPED
        && file_state <= OGG_FILE_HANDLER_FILE_STATE_MAX)) {
        play_state = 0;
        file_loading_frames = 0;
        file_ready_frames = 0;
        duration = 0.0f;
        time = 0.0f;
        if (state != 1) {
            state = 0;
            stop_playback();
        }
        else if (set_path_playback()) {
            play_state = 1;
            state = 0;
        }
        else
            stop_playback();
    }
    else if (file_state >= OGG_FILE_HANDLER_FILE_STATE_LOADING
        && file_state <= OGG_FILE_HANDLER_FILE_STATE_STOPPING) {
        play_state = 1;
        if (ogg_playback) {
            if (ogg_playback->GetPauseState() == OGG_FILE_HANDLER_PAUSE_STATE_PLAY) {
                file_loading_frames += file_state == OGG_FILE_HANDLER_FILE_STATE_LOADING;
                file_ready_frames += file_state == OGG_FILE_HANDLER_FILE_STATE_READY;
            }
            float_t _duration = ogg_playback->GetDuration();
            float_t _time = ogg_playback->GetTime();
            duration = _duration;
            if (_duration > 0.0f && _time > _duration)
                do
                    _time -= _duration;
            while (_time > _duration);
            time = _time;
        }

        if (state)
            stop_playback();
    }

    OggPlayback::SetChannelPairVolumePan(ogg_playback);
}

void sound_stream::reset() {
    state = 0;
    play_state = 0;
}

void sound_stream::set_current_volume(float_t value) {
    current_volume = clamp_def(value, 0.0f, 1.0f);
    volume_trans = 0;
}

bool sound_stream::set_path(const char* path, bool pause) {
    if (!pause) {
        bool paused = false;
        if (ogg_playback)
            paused = ogg_playback->GetPauseState() == OGG_FILE_HANDLER_PAUSE_STATE_PAUSE;

        if (!(this->path.compare(path) || !check_state() || !paused)) {
            if (ogg_playback)
                ogg_playback->SetPauseState(OGG_FILE_HANDLER_PAUSE_STATE_PLAY);
            pause = false;
            return true;
        }
    }

    this->path.assign(path);
    state = 1;
    this->pause = pause;
    current_volume = sound_stream_volume;
    load_time_seek = 0.0f;
    volume_trans = 0;
    target_volume = sound_stream_volume;
    return true;
}

bool sound_stream::set_path(const char* path, float_t time, bool pause) {
    bool res = set_path(path, pause);
    if (res)
        load_time_seek = time;
    return res;
}

bool sound_stream::set_path_playback() {
    if (!ogg_playback)
        return false;

    ogg_playback->SetPauseState(pause
        ? OGG_FILE_HANDLER_PAUSE_STATE_PAUSE : OGG_FILE_HANDLER_PAUSE_STATE_PLAY);
    ogg_playback->SetLoadTimeSeek(load_time_seek);
    ogg_playback->SetPath(path);
    return true;
}

bool sound_stream::set_pause(bool value) {
    if (!ogg_playback)
        return false;

    pause = value;
    ogg_playback->SetPauseState(value
        ? OGG_FILE_HANDLER_PAUSE_STATE_PAUSE : OGG_FILE_HANDLER_PAUSE_STATE_PLAY);
    return true;
}

void sound_stream::set_target_volume(float_t value, int32_t frames) {
    target_volume = value;
    volume_trans = max_def(frames, 1);
}

bool sound_stream::stop() {
    state = 2;
    return true;
}

bool sound_stream::stop_playback() {
    if (!ogg_playback)
        return false;

    ogg_playback->Stop();
    ogg_playback->SetPauseState(OGG_FILE_HANDLER_PAUSE_STATE_PLAY);
    return true;
}

static void sound_stream_array_init() {
    if (!sound_stream_array)
        sound_stream_array = new sound_stream[SOUND_WORK_STREAM_COUNT];
}

static void sound_stream_array_ctrl() {
    for (size_t i = 0; i < SOUND_WORK_STREAM_COUNT; i++)
        sound_stream_array[i].ctrl();
}

static sound_stream* sound_stream_array_get(int32_t index) {
    if (index >= 0 && index < SOUND_WORK_STREAM_COUNT)
        return &sound_stream_array[index];
    return 0;
}

static void sound_stream_array_free() {
    if (sound_stream_array) {
        delete[] sound_stream_array;
        sound_stream_array = 0;
    }
}
