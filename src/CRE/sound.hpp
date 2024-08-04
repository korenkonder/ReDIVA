/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../KKdLib/time.hpp"
#include "../KKdLib/farc.hpp"
#include "file_handler.hpp"
#include <atomic>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <mmreg.h>
#include <mmsystem.h>

struct sound_buffer_data {
    float_t spk_l;
    float_t spk_r;
    float_t hph_l;
    float_t hph_r;
};

namespace sound {
    namespace wasapi {
        enum AudioFormat {
            AUDIO_FORMAT_I16 = 0,
            AUDIO_FORMAT_I24,
            AUDIO_FORMAT_I32,
            AUDIO_FORMAT_F32,
        };

        struct Mixer;
        struct SEChannel;
        struct StreamingChannel;

        struct System {
            WAVEFORMATEXTENSIBLE wave_format;
            IMMDeviceEnumerator* pEnumerator;
            IMMDevice* pDevice;
            IAudioClient* pAudioClient;
            uint32_t samples_count;
            IAudioRenderClient* pRenderClient;
            IAudioClockAdjustment* pClockAdjustment;
            HANDLE hEvent;
            int32_t channels;
            int32_t sample_rate;
            int32_t bit_depth;
            Mixer* mixer;
            std::thread* thread;
            std::atomic_uint32_t thread_state;
            AudioFormat format;

            System();
            virtual ~System();

            void Init(size_t se_channels_count, size_t streaming_channels_count,
                bool separate_speakers_headphones = false);
            void Reset();

            static void ThreadMain(System* system);
        };

        struct Mixer {
            System* system;
            SEChannel* se_channels;
            size_t se_channels_count;
            StreamingChannel* streaming_channels;
            size_t streaming_channels_count;
            sound_buffer_data* mix_buffer;
            size_t mix_buffer_size;
            std::mutex volume_mtx;
            float_t master_volume;
            float_t channels_volume[4];

            Mixer(System* system);
            virtual ~Mixer();

            void FillBuffer(void* buffer, size_t samples_count,
                bool disable_headphones_volume, bool invert_phase, AudioFormat format);
            SEChannel* GetSEChannel(size_t channel);
            StreamingChannel* GetStreamingChannel(size_t channel);
            bool Init(size_t se_channels_count, size_t streaming_channels_count, size_t samples_count);
            void Reset();
            void SetChannelsVolume(int32_t mask, float_t value);
            void SetMasterVolume(float_t value);
        };

        struct SEChannel {
            Mixer* mixer;
            std::atomic_uint32_t play_state;
            std::mutex mtx;
            float_t master_volume;
            float_t channels_volume[4];
            float_t* buffer;
            size_t buffer_size;
            size_t channels;
            size_t samples_count;
            size_t sample_rate;
            size_t loop_start;
            size_t loop_end;
            size_t current_sample;

            SEChannel();
            virtual ~SEChannel();

            void FillBuffer(sound_buffer_data* buffer, size_t samples_count, float_t volume);
            bool Init(Mixer* mixer);
            float_t* InitBuffer(size_t channels, size_t samples_count, size_t sample_rate);
            bool Play(size_t loop_start = 0, size_t loop_end = 0);
            void Reset();
            void ResetData();
            void ResetDataProt();
            void SetChannelsVolume(int32_t mask, float_t value);
            void SetMasterVolume(float_t value);
        };

        struct StreamingChannel {
            Mixer* mixer;
            sound_buffer_data* buffer;
            size_t buffer_size;
            std::atomic_uint32_t playing_state;
            std::atomic_uint32_t reset_state;
            std::mutex mtx;
            float_t master_volume;
            float_t channels_volume[4];
            void(*callback_func)(sound_buffer_data* buffer, size_t samples_count, void* data);
            void* callback_data;

            StreamingChannel();
            virtual ~StreamingChannel();

            void FillBuffer(sound_buffer_data* buffer, size_t samples_count, float_t volume);
            bool Init(Mixer* mixer, size_t samples_count);
            void Reset();
            void ResetData();
            bool SetCallback(void(*func)(sound_buffer_data* buffer,
                size_t samples_count, void* data), void* data);
            void SetChannelsVolume(int32_t mask, float_t value);
            void SetMasterVolume(float_t value);
        };
    }
}

enum SoundCueDataState {
    SOUND_CUE_DATA_STATE_NONE = 0,
    SOUND_CUE_DATA_STATE_LOAD,
    SOUND_CUE_DATA_STATE_RESET,
    SOUND_CUE_DATA_STATE_MAX,
};

enum WaveAudioDataFormat {
    WAVE_AUDIO_DATA_FORMAT_NONE = 0,
    WAVE_AUDIO_DATA_FORMAT_RIFF = 1,
    WAVE_AUDIO_DATA_FORMAT_DIVA = 3,
};

struct sound_db_farc {
    std::string file_path;
    bool ready;
    p_file_handler file_handler;
    farc farc;

    sound_db_farc();
    ~sound_db_farc();

    bool load();
    bool read(const char* file_path);
    bool unload();
};

struct sound_db_property {
    sound_db_farc* farc;
    std::string file_name;
    float_t volume;
    size_t loop_start;
    size_t loop_end;
    float_t release_time;
    //std::string field_28;
    //int32_t field_48;

    sound_db_property();
    ~sound_db_property();
};

struct SoundCue;

struct SoundCueVolume {
    SoundCue* cue;
    float_t value;
    bool set;

    inline SoundCueVolume(SoundCue* cue) : cue(cue), value(), set() {

    }

    inline void SetValue(float_t value) {
        this->value = value;
        set = true;
    }
};

struct SoundCue {
    std::atomic_uint32_t thread_state;
    std::atomic_uint32_t data_state;
    std::thread* thread;
    std::mutex mtx;
    std::condition_variable cnd;
    SoundCueVolume* volume;
    int32_t queue_index;
    std::string name;
    sound_db_property* property;
    int32_t counter;
    float_t release_time;
    time_struct time;
    std::atomic_uint32_t load_state;
    sound::wasapi::SEChannel* se_channel;

    SoundCue();
    ~SoundCue();

    bool CanPlay();
    void Ctrl();
    int32_t Load(int32_t queue_index, const char* name, float_t volume);
    void LoadData();
    void Play();
    void PlayProt();
    void Release(bool force_release);
    void ReleaseProt(bool force_release);
    void Reset();
    void ResetData();
    void SetSEChannel(sound::wasapi::SEChannel* se_channel);

    static void ThreadMain(SoundCue* cue);
};

struct WaveAudioDataFileMemoryStream {
    FILE* file;
    const void* data;
    size_t size;
    size_t position;

    inline WaveAudioDataFileMemoryStream(const void* data, size_t size) : file(),
        data(data), size(size), position() {

    }
};

struct WaveAudioData {
    WaveAudioDataFormat format;
    size_t channels;
    size_t sample_rate;
    size_t bit_depth;
    size_t samples_count;
    size_t position;
    size_t data_size;
    bool has_loop;
    size_t loop_start;
    size_t loop_end;

    WaveAudioData();
    ~WaveAudioData();

    bool Read(WaveAudioDataFileMemoryStream* fms);
    bool ReadData(const void* data, size_t size);
    bool ReadDiva(WaveAudioDataFileMemoryStream* fms);
    void Reset();
};

struct WaveAudio {
    float_t* buffer;
    size_t buffer_size;
    WaveAudioData data;

    WaveAudio();

    bool Read(sound_db_farc* farc, const char* file_name);
    void Reset();
};

#define SOUND_WORK_SE_QUEUE_COUNT 5
#define SOUND_WORK_STREAM_COUNT 3
#define SOUND_WORK_SE_CHANNELS_COUNT 32
#define SOUND_WORK_STREAMING_CHANNELS_COUNT 4

struct SoundWork {
    std::map<std::string, sound_db_property> properties;
    sound_db_farc farcs[16];
    SoundCue cues[SOUND_WORK_SE_CHANNELS_COUNT];
    int32_t counter;
    std::vector<std::string> names_list;
    bool se_queue_enable[SOUND_WORK_SE_QUEUE_COUNT];
    bool stream_enable[SOUND_WORK_STREAM_COUNT];
    float_t speakers_volume;
    bool speakers_volume_changed;
    float_t headphones_volume;
    bool headphones_volume_changed;
    float_t se_queue_volume[5];
    float_t se_volume;

    SoundWork();
    virtual ~SoundWork();

    sound_db_property* FindProperty(const char* name);
    bool ParseProperty(sound_db_farc* snd_db_farc);
    bool UnloadProperty(const char* file_path);
};

struct sound_stream_info {
    float_t duration;
    float_t time;
    std::string path;

    sound_stream_info();
    ~sound_stream_info();
};

extern float_t get_min_headphones_volume();
extern float_t get_max_headphones_volume();
extern float_t get_min_speakers_volume();
extern float_t get_max_speakers_volume();

extern float_t db_to_ratio(int32_t value);
extern int32_t ratio_to_db(float_t value);

extern void sound_init();
extern void sound_ctrl();
extern void sound_free();

extern float_t sound_cue_queue_volume_array_get_max(int32_t queue_index);
extern float_t sound_cue_queue_volume_array_get_min(int32_t queue_index);

extern void sound_stream_array_reset();

extern bool sound_work_check_stream_state(int32_t index);
extern bool sound_work_cue_release(int32_t queue_index, const char* name, bool force_release = true);
extern SoundCue* sound_work_get_cue(int32_t queue_index, const char* name);
extern bool sound_work_get_cue_can_play(int32_t queue_index, const char* name);
extern sound_db_farc* sound_work_get_farc(const char* file_path);
extern bool sound_work_get_stream_info(sound_stream_info& info, int32_t index);
extern float_t sound_work_get_stream_volume(int32_t index);
extern float_t sound_work_get_headphones_volume();
extern float_t sound_work_get_speakers_volume();
extern bool sound_work_has_property(const char* name);
extern bool sound_work_load_farc(const char* file_path);
extern int32_t sound_work_play_se(int32_t queue_index, const char* name, float_t volume);
extern bool sound_work_play_stream(int32_t index, const char* path, bool pause = false);
extern bool sound_work_play_stream(int32_t index, const char* path, float_t time, bool pause = false);
extern bool sound_work_read_farc(const char* file_path);
extern void sound_work_release_farc_se(const char* file_path);
extern bool sound_work_release_se(const char* name, bool force_release = true);
extern bool sound_work_release_stream(int32_t index);
extern bool sound_work_stream_set_pause(int32_t index, bool value);
extern void sound_work_reset_all_se();
extern float_t sound_cue_queue_volume_array_get_max(int32_t queue_index);
extern float_t sound_cue_queue_volume_array_get_min(int32_t queue_index);
extern sound::wasapi::System* sound_wasapi_system_data_get();
extern void sound_work_set_headphones_volume(float_t value);
extern void sound_work_set_se_queue_volume(int32_t queue_index, float_t value);
extern void sound_work_set_speakers_volume(float_t value);
extern bool sound_work_set_stream_current_volume(int32_t index, float_t value);
extern bool sound_work_set_stream_target_volume(int32_t index, float_t value, int32_t frames);
extern bool sound_work_unload_farc(const char* file_path);

extern void wave_audio_storage_init();
extern void wave_audio_storage_clear();
extern WaveAudio* wave_audio_storage_get_wave_audio(const std::string& name);
extern bool wave_audio_storage_load_wave_audio(const std::string& name);
extern void wave_audio_storage_unload_wave_audio(const std::string& name);
extern void wave_audio_storage_free();
