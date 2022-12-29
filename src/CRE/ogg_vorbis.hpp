/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../CRE/waitable_timer.hpp"
#include "lock.hpp"
#include "sound.hpp"
#include <mutex>
#include <thread>
#include <string>
#include <vector>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

enum AudioBitFormat {
    AUDIO_BIT_FORMAT_S16 = 0,
    AUDIO_BIT_FORMAT_S24,
    AUDIO_BIT_FORMAT_S32,
    AUDIO_BIT_FORMAT_F32,
};

enum OggFileHandlerFileState {
    OGG_FILE_HANDLER_FILE_STATE_NONE = 0,
    OGG_FILE_HANDLER_FILE_STATE_LOADING,
    OGG_FILE_HANDLER_FILE_STATE_READY,
    OGG_FILE_HANDLER_FILE_STATE_PLAYING,
    OGG_FILE_HANDLER_FILE_STATE_STOPPING,
    OGG_FILE_HANDLER_FILE_STATE_STOPPED,
    OGG_FILE_HANDLER_FILE_STATE_MAX,
};

enum OggFileHandlerPauseState {
    OGG_FILE_HANDLER_PAUSE_STATE_PLAY = 0,
    OGG_FILE_HANDLER_PAUSE_STATE_PAUSE,
    OGG_FILE_HANDLER_PAUSE_STATE_MAX,
};

enum OggFileHandlerPlaybackState {
    OGG_FILE_HANDLER_PLAYBACK_STATE_NONE = 0,
    OGG_FILE_HANDLER_PLAYBACK_STATE_LOAD,
    OGG_FILE_HANDLER_PLAYBACK_STATE_UNLOAD,
    OGG_FILE_HANDLER_PLAYBACK_STATE_MAX,
};

struct OggFileBufferChannelData {
    float_t left;
    float_t right;

    OggFileBufferChannelData();
};

struct OggFileBufferData {
    double_t time;
    OggFileBufferChannelData channel_pair[4];

    OggFileBufferData();
};

struct OggFileBuffer {
    size_t size;
    std::vector<OggFileBufferData> data;
    size_t first_sample;
    size_t last_sample;

    OggFileBuffer();
    ~OggFileBuffer();

    void Reset();
};

struct OggFile {
    std::mutex mtx;
    lock<uint32_t> thread_state;
    std::thread* thread;
    OggFileBuffer buffer;
    OggVorbis_File* file;
    vorbis_info* info;
    vorbis_comment* comments;
    int64_t loop_begin;
    int64_t loop_end;
    std::mutex file_mtx;

    OggFile();
    ~OggFile();

    void Ctrl(size_t samples_count);
    size_t FillBufferData(OggFileBufferData* buffer_data, size_t samples_count);
    size_t GetBufferFreeSamplesCount();
    size_t GetBufferSamplesCount();
    int32_t GetChannelPairsCount();
    double_t GetDuration();
    int32_t GetRate();
    bool HasLoop();
    bool OpenFile(const char* path, double_t time_seek);
    void Reset();

    static void ThreadMain(OggFile* of);
};

struct OggFileHandler {
    std::mutex mtx;
    lock<uint32_t> thread_state;
    std::thread* thread;
    size_t index;
    OggFile file;
    std::mutex file_mtx;
    std::string path;
    float_t load_time_seek;
    int32_t channel_pairs_count;
    double_t duration;
    double_t time;
    std::mutex dup_mtx;
    int32_t channel_pairs_count_dup;
    double_t duration_dup;
    double_t time_dup;
    std::mutex volume_mtx;
    int32_t channel_pair_volume_pan[4][2][4];
    int32_t master_volume;
    int32_t channel_pair_volume[4];
    lock<OggFileHandlerFileState> file_state;
    lock<OggFileHandlerPlaybackState> playback_state;
    lock<OggFileHandlerPauseState> pause_state;
    double_t req_time;
    int32_t rate;
    bool loop;
    std::vector<OggFileBufferData> buffer_data;

    OggFileHandler(size_t index = 0);
    ~OggFileHandler();

    void Ctrl();
    int32_t GetChannelPairsCount();
    float_t GetDuration();
    OggFileHandlerFileState GetFileState();
    OggFileHandlerPauseState GetPauseState();
    float_t GetTime();
    void FillBuffer(sound_buffer_data* buffer, size_t samples_count);
    void OpenFile();
    void ReadBuffer(sound_buffer_data* buffer, size_t samples_count);
    void ReadBufferProt(sound_buffer_data* buffer, size_t samples_count);
    void Reset(bool reset = false);
    void SetChannelPairVolumePan(size_t src_channel_pair,
        int32_t src_channel, int32_t dst_channel, int32_t value);
    void SetFileState(OggFileHandlerFileState state);
    void SetLoadTimeSeek(float_t value);
    void SetMasterVolume(int32_t value);
    void SetPath(std::string& path);
    void SetPath(std::string&& path);
    void SetPauseState(OggFileHandlerPauseState value);
    void SetPlaybackState(OggFileHandlerPlaybackState value);

    static void FillBufferStatic(sound_buffer_data* buffer, size_t samples_count, void* data);
    static bool LoadFile(void* data, const char* path, const char* file, uint32_t hash);
    static void ThreadMain(OggFileHandler* ofh);
};

struct p_OggFileHandler {
    OggFileHandler* ptr;

    p_OggFileHandler(OggFileHandler* ptr);
    ~p_OggFileHandler();

    int32_t GetChannelPairsCount();
    float_t GetDuration();
    OggFileHandlerFileState GetFileState();
    OggFileHandlerPauseState GetPauseState();
    float_t GetTime();
    void SetChannelPairVolumePan(size_t src_channel_pair,
        int32_t src_channel, int32_t dst_channel, int32_t value);
    void SetLoadTimeSeek(float_t value);
    void SetMasterVolume(int32_t value);
    void SetPath(std::string& path);
    void SetPath(std::string&& path);
    void SetPauseState(OggFileHandlerPauseState value);
    void SetPlaybackState(OggFileHandlerPlaybackState value);
};

struct OggPlayback {
    size_t index;
    int32_t state;
    p_OggFileHandler* ogg_file_handler;
    float_t load_time_seek;

    OggPlayback(size_t index);
    ~OggPlayback();

    int32_t GetChannelPairsCount();
    float_t GetDuration();
    OggFileHandlerFileState GetFileState();
    OggFileHandlerPauseState GetPauseState();
    float_t GetTime();
    void Reset();
    void SetChannelPairVolumePan(size_t src_channel_pair,
        int32_t src_channel, int32_t dst_channel, int32_t value);
    void SetLoadTimeSeek(float_t value);
    void SetMasterVolume(int32_t value);
    void SetPath(std::string& path);
    void SetPath(std::string&& path);
    void SetPauseState(OggFileHandlerPauseState value);
    void SetPlaybackState(OggFileHandlerPlaybackState value);

    static bool LoadFile(void* data, const char* path, const char* file, uint32_t hash);
    static void SetChannelPairVolumePan(OggPlayback* op);
};

extern void ogg_playback_data_init();
extern void ogg_playback_data_free();
extern OggPlayback* ogg_playback_data_get(size_t index);

extern void ogg_file_handler_storage_init();
extern void ogg_file_handler_storage_free();
