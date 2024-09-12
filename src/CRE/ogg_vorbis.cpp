/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "ogg_vorbis.hpp"
#include "../KKdLib/str_utils.hpp"
#include "../KKdLib/vec.hpp"
#include "data.hpp"
#include <intrin.h>

struct ogg_file_handler_storage {
    std::mutex mtx;
    bool clear;
    std::list<p_OggFileHandler*> list;

    ogg_file_handler_storage();
    ~ogg_file_handler_storage();

    void free_data();
};

static int32_t ogg_file_counter = 0;
static int32_t ogg_file_handler_counter = 0;

#define OGG_PLAYBACK_DATA_COUNT 6
static OggPlayback* ogg_playback_data[OGG_PLAYBACK_DATA_COUNT];

static int32_t ogg_file_handler_storage_init_count = 0;
static ogg_file_handler_storage* ogg_file_handler_storage_ptr;

static p_OggFileHandler* ogg_file_handler_storage_get_ogg_file_handler(size_t index);

OggFileBufferChannelData::OggFileBufferChannelData() : left(), right() {

}

OggFileBufferData::OggFileBufferData() {
    time = -1.0;
}

OggFileBuffer::OggFileBuffer() :  first_sample(), last_sample() {
    size = 0x40999;
    data.resize(size);
}

OggFileBuffer::~OggFileBuffer() {

}

void OggFileBuffer::Reset() {
    data.clear();
    data.resize(size);
    first_sample = 0;
    last_sample = 0;
}

OggFile::OggFile() : file(), info(), comments() {
    loop_begin = -1;
    loop_end = -1;

    thread = new std::thread(OggFile::ThreadMain, this);
    if (thread) {
        wchar_t buf[0x80];
        swprintf_s(buf, sizeof(buf) / sizeof(wchar_t), L"Ogg File #%d", ogg_file_counter++);
        SetThreadDescription((HANDLE)thread->native_handle(), buf);
    }
}

OggFile::~OggFile() {
    Reset();
    if (thread) {
        thread_state = 1;
        thread->join();
        delete thread;
        thread = 0;
    }
}

void OggFile::Ctrl(size_t samples_count) {
    std::unique_lock<std::mutex> u_lock(file_mtx);
    if (!file || !info || !comments)
        return;

    OggFileBufferData* _buffer_data = buffer.data.data();
    size_t _buffer_size = buffer.data.size();

    while (samples_count) {
        size_t free_samples_count = GetBufferFreeSamplesCount();
        if (free_samples_count <= 0)
            break;

        int64_t samples_to_read = min_def(samples_count, free_samples_count);
        int64_t position = ov_pcm_tell(file);
        double_t time_start = ov_time_tell(file);
        if (loop_end >= 0 && position + samples_to_read > loop_end) {
            if (loop_end - position <= 0) {
                size_t seek = 0i64;
                if (loop_begin >= 0)
                    seek = loop_begin;
                ov_pcm_seek_lap(file, seek);
            }
            else
                samples_to_read = loop_end - position;
        }

        float_t** pcm_channels = 0;
        int32_t bitstream = 0;
        size_t samples_read = ov_read_float(file, &pcm_channels, (int32_t)samples_to_read, &bitstream);
        if (samples_read < 1 || !pcm_channels)
            break;

        int32_t channels = info->channels;
        bool mono = channels == 1;

        std::unique_lock<std::mutex> u_lock(mtx);
        for (size_t i = 0; i < samples_read; i++) {
            OggFileBufferData data;
            data.time = (double_t)(int32_t)i / (double_t)info->rate + time_start;
            if (channels > 0) {
                if (mono) {
                    float_t value = pcm_channels[0][i];
                    data.channel_pair[0].left = value;
                    data.channel_pair[0].right = value;
                }
                else {
                    float_t** _pcm_channels = pcm_channels;
                    for (int32_t j = 0; j < channels; j += 2, _pcm_channels += 2) {
                        int32_t pair = j / 2;
                        if (pair >= 4)
                            break;

                        data.channel_pair[pair].left = _pcm_channels[0][i];
                        data.channel_pair[pair].right = _pcm_channels[1][i];
                    }
                }
            }

            if (_buffer_size) {
                size_t last_sample = buffer.last_sample;
                size_t new_last_sample = last_sample + 1;
                if (new_last_sample >= buffer.size)
                    new_last_sample = 0;
                if (new_last_sample != buffer.first_sample) {
                    _buffer_data[last_sample] = data;
                    buffer.last_sample = new_last_sample;
                }
            }
        }
        samples_count -= samples_read;
    }
}

size_t OggFile::FillBufferData(OggFileBufferData* buffer_data, size_t samples_count) {
    std::unique_lock<std::mutex> u_lock(mtx);
    OggFileBufferData* _buffer_data = buffer.data.data();
    size_t _buffer_size = buffer.data.size();
    size_t _samples_count = 0;
    size_t first_sample = buffer.first_sample;
    size_t last_sample = buffer.last_sample;
    for (; samples_count && first_sample != last_sample;
        samples_count--, buffer_data++, _samples_count++) {
        if (_buffer_size)
            *buffer_data = _buffer_data[first_sample];
        else
            *buffer_data = {};

        if (_buffer_size) {
            if (first_sample != last_sample) {
                _buffer_data[first_sample++] = {};
                if (first_sample >= buffer.size)
                    first_sample = 0;
            }
        }
    }
    buffer.first_sample = first_sample;
    return _samples_count;
}

size_t OggFile::GetBufferFreeSamplesCount() {
    std::unique_lock<std::mutex> u_lock(mtx);
    size_t last_sample = buffer.last_sample;
    size_t first_sample = buffer.first_sample;
    size_t samples_count;
    if (last_sample < first_sample)
        samples_count = buffer.size - first_sample + last_sample;
    else
        samples_count = last_sample - first_sample;
    return buffer.size - samples_count - 1;
}

size_t OggFile::GetBufferSamplesCount() {
    std::unique_lock<std::mutex> u_lock(mtx);
    size_t last_sample = buffer.last_sample;
    size_t first_sample = buffer.first_sample;
    if (last_sample < first_sample)
        return buffer.size - first_sample + last_sample;
    else
        return last_sample - first_sample;
}

int32_t OggFile::GetChannelPairsCount() {
    std::unique_lock<std::mutex> u_lock(file_mtx);
    if (info)
        return (info->channels + 1) / 2;
    return 0;
}

double_t OggFile::GetDuration() {
    std::unique_lock<std::mutex> u_lock(file_mtx);
    if (file)
        return ov_time_total(file, -1);
    return 0.0;
}

int32_t OggFile::GetRate() {
    std::unique_lock<std::mutex> u_lock(file_mtx);
    if (info)
        return info->rate;
    return 0;
}

bool OggFile::HasLoop() {
    std::unique_lock<std::mutex> u_lock(file_mtx);
    return loop_end >= 0;
}

bool OggFile::OpenFile(const char* path, double_t time_seek) {
    {
        std::unique_lock<std::mutex> u_lock(file_mtx);
        if (file)
            goto Reset;

        file = force_malloc<OggVorbis_File>();
        if (!file)
            goto Reset;

        if (ov_fopen(path, file) < 0)
            goto Reset;

        info = ov_info(file, -1);
        comments = ov_comment(file, -1);
        loop_begin = -1;
        loop_end = -1;

        if (!info || !comments) {
            info = 0;
            comments = 0;
            goto Reset;
        }

        char** user_comments = comments->user_comments;
        while (*user_comments) {
            std::string user_comment = *user_comments++;
            for (char& c : user_comment)
                if (c >= 'A' && c <= 'Z')
                    c += 0x20;

            if (user_comment.size() >= 10 && !memcmp(user_comment.c_str(), "loop_begin", 10)) {
                const char* t = strchr(user_comment.c_str(), '=');
                if (t) {
                    t++;
                    sscanf_s(t, "%lld", &loop_begin);
                }
            }
            else if (user_comment.size() >= 8 && !memcmp(user_comment.c_str(), "loop_end", 8)) {
                const char* t = strchr(user_comment.c_str(), '=');
                if (t) {
                    t++;
                    sscanf_s(t, "%lld", &loop_end);
                }
            }
        }

        if (ov_time_seek(this->file, time_seek) >= 0)
            return true;
    }

Reset:
    Reset();
    return false;
}

void OggFile::Reset() {
    std::unique_lock<std::mutex> u_lock(file_mtx);
    if (file) {
        ov_clear(file);
        free(file);
    }

    buffer.Reset();
    file = 0;
    info = 0;
    comments = 0;
    loop_begin = -1;
    loop_end = -1;
}

void OggFile::ThreadMain(OggFile* of) {
    if (!of)
        return;

    waitable_timer timer;
    while (!of->thread_state) {
        int32_t rate = 44100;
        {
            std::unique_lock<std::mutex> u_lock(of->file_mtx);
            if (of->info)
                rate = of->info->rate;
        }
        of->Ctrl(rate);
        timer.sleep(8);
    }
    of->thread_state = 0;
}

OggFileHandler::OggFileHandler(size_t index) : load_time_seek(), channel_pairs_count(),
duration(), time(), channel_pairs_count_dup(), duration_dup(), time_dup(),
channel_pair_volume_pan(), master_volume(), channel_pair_volume(),  req_time(), loop() {
    this->index = index;

    for (int32_t i = 0; i < 4; i++)
        for (int32_t j = 0; j < 2; j++)
            for (int32_t k = 0; k < 4; k++)
                channel_pair_volume_pan[i][j][k] = -10000;

    channel_pair_volume_pan[0][0][0] = 0;
    channel_pair_volume_pan[0][1][1] = 0;
    rate = 1;

    thread = new std::thread(OggFileHandler::ThreadMain, this);
    if (thread) {
        wchar_t buf[0x80];
        swprintf_s(buf, sizeof(buf) / sizeof(wchar_t), L"Ogg File Handler #%d", ogg_file_handler_counter++);
        SetThreadDescription((HANDLE)thread->native_handle(), buf);
    }
}

OggFileHandler::~OggFileHandler() {
    Reset();
    if (thread) {
        thread_state = 1;
        thread->join();
        delete thread;
        thread = 0;
    }
}

void OggFileHandler::Ctrl() {
    int32_t _playback_state = 0;
    {
        std::unique_lock<std::mutex> u_lock(mtx);
        _playback_state = playback_state;
        playback_state = OGG_FILE_HANDLER_PLAYBACK_STATE_NONE;
        switch (_playback_state) {
        case OGG_FILE_HANDLER_PLAYBACK_STATE_LOAD:
            Reset(false);
            file_state = OGG_FILE_HANDLER_FILE_STATE_LOADING;
            OpenFile();
            file_state = OGG_FILE_HANDLER_FILE_STATE_READY;
            req_time = 0.0;
            ReadBufferProt(0, 0);
            break;
        case OGG_FILE_HANDLER_PLAYBACK_STATE_UNLOAD:
            Reset(true);
            break;
        }
    }

    sound::wasapi::System* sound_wasapi_system = sound_wasapi_system_data_get();
    if (sound_wasapi_system) {
        sound::wasapi::Mixer* mixer = sound_wasapi_system->mixer;
        if (mixer) {
            sound::wasapi::StreamingChannel* streaming_channel = mixer->GetStreamingChannel(index);
            if (streaming_channel) {
                switch (_playback_state) {
                case OGG_FILE_HANDLER_PLAYBACK_STATE_LOAD:
                    if (sound_wasapi_system->pClockAdjustment)
                        sound_wasapi_system->pClockAdjustment->SetSampleRate((float_t)rate);

                    streaming_channel->SetMasterVolume(1.0f);
                    streaming_channel->SetCallback(OggFileHandler::FillBufferStatic, this);
                    break;
                case OGG_FILE_HANDLER_PLAYBACK_STATE_UNLOAD:
                    streaming_channel->reset_state = 1;
                    break;
                }
            }
        }
    }

    int32_t _channel_pairs_count = 0;
    double_t _duration = 0.0;
    double_t _time = 0.0;

    {
        std::unique_lock<std::mutex> u_lock(file_mtx);
        _channel_pairs_count = channel_pairs_count;
        _duration = duration;
        _time = time;
    }

    {
        std::unique_lock<std::mutex> u_lock(dup_mtx);
        channel_pairs_count_dup = _channel_pairs_count;
        duration_dup = _duration;
        time_dup = _time;
    }
}

void OggFileHandler::FillBuffer(sound_buffer_data* buffer, size_t samples_count) {
    int32_t _file_state = file_state;
    file_state = OGG_FILE_HANDLER_FILE_STATE_NONE;

    int32_t _playback_state = playback_state;
    playback_state = OGG_FILE_HANDLER_PLAYBACK_STATE_NONE;

    if (_file_state == OGG_FILE_HANDLER_FILE_STATE_PLAYING
        && _playback_state != OGG_FILE_HANDLER_PLAYBACK_STATE_UNLOAD) {
        std::unique_lock<std::mutex> u_lock(mtx);
        ReadBufferProt(buffer, samples_count);
    }
}

int32_t OggFileHandler::GetChannelPairsCount() {
    std::unique_lock<std::mutex> u_lock(dup_mtx);
    return channel_pairs_count_dup;
}

float_t OggFileHandler::GetDuration() {
    std::unique_lock<std::mutex> u_lock(dup_mtx);
    return (float_t)duration_dup;
}

OggFileHandlerFileState OggFileHandler::GetFileState() {
    return file_state;
}

OggFileHandlerPauseState OggFileHandler::GetPauseState() {
    return pause_state;
}

float_t OggFileHandler::GetTime() {
    std::unique_lock<std::mutex> u_lock(dup_mtx);
    return (float_t)time_dup;
}

void OggFileHandler::OpenFile() {
    std::unique_lock<std::mutex> u_lock(file_mtx);
    if (file.OpenFile(path.c_str(), load_time_seek)) {
        channel_pairs_count = file.GetChannelPairsCount();
        duration = file.GetDuration();
        time = 0.0;
        rate = file.GetRate();
        loop = file.HasLoop();
    }
}

void OggFileHandler::ReadBuffer(sound_buffer_data* buffer, size_t samples_count) {
    if (!samples_count)
        return;

    vec4 channel_pair_volume_pan[4][2];
    float_t channel_pair_volume[4];
    {
        std::unique_lock<std::mutex> u_lock(volume_mtx);
        float_t master_volume = db_to_ratio(this->master_volume);
        for (int32_t i = 0; i < 4; i++) {
            for (int32_t j = 0; j < 2; j++)
                for (int32_t k = 0; k < 4; k++)
                    ((float_t*)&channel_pair_volume_pan[i][j])[k]
                        = db_to_ratio(this->channel_pair_volume_pan[i][j][k]);
            channel_pair_volume[i] = db_to_ratio(this->channel_pair_volume[i]) * master_volume;
        }
    }

    size_t samples_count_in_buffer = file.GetBufferSamplesCount();
    if (samples_count > samples_count_in_buffer) {
        file.Ctrl(samples_count - samples_count_in_buffer);
        file.GetBufferSamplesCount();
    }

    buffer_data.resize(samples_count);
    if (!buffer_data.size())
        return;

    OggFileBufferData* _buffer_data = buffer_data.data();
    size_t _samples_count = file.FillBufferData(_buffer_data, samples_count);
    if (_samples_count)
        req_time = _buffer_data->time;
    else {
        double_t _samples_count = (double_t)(int32_t)samples_count;
        if (samples_count < 0)
            _samples_count += 1.844674407370955e19;
        req_time += _samples_count / (double_t)rate;
    }

    if (_samples_count) {
        for (size_t i = _samples_count; i; i--, buffer++, _buffer_data++) {
            OggFileBufferChannelData* channel_pair = _buffer_data->channel_pair;
            vec4 value = 0.0f;
            for (size_t j = 0; j < 4; j++, channel_pair++) {
                value += (channel_pair_volume_pan[j][0] * (channel_pair_volume[j] * channel_pair->left));
                value += (channel_pair_volume_pan[j][1] * (channel_pair_volume[j] * channel_pair->right));
            }
            *(vec4*)buffer = value;
        }
    }
    else
        memset(buffer, 0, sizeof(sound_buffer_data) * samples_count);
}

void OggFileHandler::ReadBufferProt(sound_buffer_data* buffer, size_t samples_count) {
    file_state = OGG_FILE_HANDLER_FILE_STATE_PLAYING;
    if (pause_state != OGG_FILE_HANDLER_PAUSE_STATE_PLAY)
        samples_count = 0;

    bool process = false;
    {
        std::unique_lock<std::mutex> u_lock(file_mtx);
        time = req_time;
        process = req_time < duration;
    }

    if (process || loop)
        ReadBuffer(buffer, samples_count);
    else
        playback_state = OGG_FILE_HANDLER_PLAYBACK_STATE_UNLOAD;
}

void OggFileHandler::Reset(bool reset) {
    if (reset) {
        SetFileState(OGG_FILE_HANDLER_FILE_STATE_STOPPING);
        std::unique_lock<std::mutex> u_lock(file_mtx);
        channel_pairs_count = 0;
        duration = 0.0;
        time = 0.0;
    }

    file.Reset();
    if (reset) {
        SetFileState(OGG_FILE_HANDLER_FILE_STATE_STOPPED);
        SetFileState(OGG_FILE_HANDLER_FILE_STATE_NONE);
    }
}

void OggFileHandler::SetChannelPairVolume(size_t channel_pair, int32_t value) {
    if (channel_pair < 0 || channel_pair >= 4)
        return;

    value = clamp_def(value, -10000, 0);

    std::unique_lock<std::mutex> u_lock(volume_mtx);
    channel_pair_volume[channel_pair] = value;
}

void OggFileHandler::SetChannelPairVolumePan(size_t src_channel_pair,
    int32_t src_channel, int32_t dst_channel, int32_t value) {
    if (src_channel_pair < 0 || src_channel_pair >= 4 || src_channel < 0
        || src_channel >= 2 || dst_channel < 0 || dst_channel >= 4)
        return;

    value = clamp_def(value, -10000, 0);

    std::unique_lock<std::mutex> u_lock(volume_mtx);
    channel_pair_volume_pan[src_channel_pair][src_channel][dst_channel] = value;
}

void OggFileHandler::SetFileState(OggFileHandlerFileState state) {
    file_state = state;
    if (state == OGG_FILE_HANDLER_PLAYBACK_STATE_MAX)
        Reset(false);
}

void OggFileHandler::SetLoadTimeSeek(float_t value) {
    std::unique_lock<std::mutex> u_lock(file_mtx);
    load_time_seek = value;
}

void OggFileHandler::SetMasterVolume(int32_t value) {
    value = clamp_def(value, -10000, 0);
    std::unique_lock<std::mutex> u_lock(volume_mtx);
    master_volume = value;
}

void OggFileHandler::SetPath(const std::string& path) {
    std::unique_lock<std::mutex> u_lock(mtx);
    file_state = OGG_FILE_HANDLER_FILE_STATE_LOADING;
    playback_state = OGG_FILE_HANDLER_PLAYBACK_STATE_LOAD;

    {
        std::unique_lock<std::mutex> u_lock(file_mtx);
        this->path.assign(path);
        channel_pairs_count = 0;
        duration = 0.0;
        time = 0.0;
    }
}

void OggFileHandler::SetPath(std::string&& path) {
    SetPath(path);
}

void OggFileHandler::SetPauseState(OggFileHandlerPauseState value) {
    pause_state = value;
}

void OggFileHandler::SetPlaybackState(OggFileHandlerPlaybackState value) {
    playback_state = value;
}

void OggFileHandler::FillBufferStatic(sound_buffer_data* buffer, size_t samples_count, void* data) {
    if (data)
        ((OggFileHandler*)data)->FillBuffer(buffer, samples_count);
}

bool OggFileHandler::LoadFile(void* data, const char* path, const char* file, uint32_t hash) {
    OggFileHandler* ofh = (OggFileHandler*)data;

    std::string _path;
    _path.assign(path);
    _path.append(file);

    ofh->SetPath(_path.c_str());
    return true;
}

void OggFileHandler::ThreadMain(OggFileHandler* ofh) {
    if (!ofh)
        return;

    waitable_timer timer;
    while (!ofh->thread_state) {
        ofh->Ctrl();
        timer.sleep(8);
    }
    ofh->playback_state = OGG_FILE_HANDLER_PLAYBACK_STATE_UNLOAD;
    ofh->Ctrl();
    ofh->thread_state = 0;
}

p_OggFileHandler::p_OggFileHandler(OggFileHandler* ptr) {
    this->ptr = ptr;
}

p_OggFileHandler::~p_OggFileHandler() {
    if (ptr) {
        delete ptr;
        ptr = 0;
    }

    std::list<p_OggFileHandler*>& list = ogg_file_handler_storage_ptr->list;
    for (auto i = list.begin(); i != list.end();)
        if (*i == this) {
            list.erase(i);
            break;
        }
        else
            i++;
}

int32_t p_OggFileHandler::GetChannelPairsCount() {
    if (ptr)
        return ptr->GetChannelPairsCount();
    return 0;
}

float_t p_OggFileHandler::GetDuration() {
    if (ptr)
        return ptr->GetDuration();
    return 0.0f;
}

OggFileHandlerFileState p_OggFileHandler::GetFileState() {
    if (ptr)
        return ptr->GetFileState();
    return OGG_FILE_HANDLER_FILE_STATE_MAX;
}

OggFileHandlerPauseState p_OggFileHandler::GetPauseState() {
    if (ptr)
        return ptr->GetPauseState();
    return OGG_FILE_HANDLER_PAUSE_STATE_MAX;
}

float_t p_OggFileHandler::GetTime() {
    if (ptr)
        return ptr->GetTime();
    return 0.0f;
}

void p_OggFileHandler::SetChannelPairVolume(size_t channel_pair, int32_t value) {
    if (ptr)
        ptr->SetChannelPairVolume(channel_pair, value);
}

void p_OggFileHandler::SetChannelPairVolumePan(size_t src_channel_pair,
    int32_t src_channel, int32_t dst_channel, int32_t value) {
    if (ptr)
        ptr->SetChannelPairVolumePan(src_channel_pair, src_channel, dst_channel, value);
}

void p_OggFileHandler::SetLoadTimeSeek(float_t value) {
    if (ptr)
        ptr->SetLoadTimeSeek(value);
}

void p_OggFileHandler::SetMasterVolume(int32_t value) {
    if (ptr)
        ptr->SetMasterVolume(value);
}

void p_OggFileHandler::SetPath(const std::string& path) {
    if (ptr)
        ptr->SetPath(path);
}

void p_OggFileHandler::SetPath(std::string&& path) {
    if (ptr)
        ptr->SetPath(path);
}

void p_OggFileHandler::SetPauseState(OggFileHandlerPauseState value) {
    if (ptr)
        ptr->SetPauseState(value);
}

void p_OggFileHandler::SetPlaybackState(OggFileHandlerPlaybackState value) {
    if (ptr)
        ptr->SetPlaybackState(value);
}

OggPlayback::OggPlayback(size_t index) : state(), ogg_file_handler(), load_time_seek() {
    this->index = index;
}

OggPlayback::~OggPlayback() {
    Reset();
}

int32_t OggPlayback::GetChannelPairsCount() {
    if (state == 1 && ogg_file_handler)
        return ogg_file_handler->GetChannelPairsCount();
    return 0;
}

float_t OggPlayback::GetDuration() {
    if (state == 1 && ogg_file_handler)
        return ogg_file_handler->GetDuration();
    return 0.0f;
}

OggFileHandlerFileState OggPlayback::GetFileState() {
    if (state == 1 && ogg_file_handler)
        return ogg_file_handler->GetFileState();
    return OGG_FILE_HANDLER_FILE_STATE_MAX;
}

OggFileHandlerPauseState OggPlayback::GetPauseState() {
    if (state == 1 && ogg_file_handler)
        return ogg_file_handler->GetPauseState();
    return OGG_FILE_HANDLER_PAUSE_STATE_MAX;
}

float_t OggPlayback::GetTime() {
    if (state == 1 && ogg_file_handler)
        return ogg_file_handler->GetTime();
    return 0.0f;
}

void OggPlayback::Reset() {
    if (state == 1)
        delete ogg_file_handler;
    state = 0;
    ogg_file_handler = 0;
}

void OggPlayback::SetChannelPairVolume(size_t channel_pair, int32_t value) {
    if (state != 1 || !ogg_file_handler)
        return;

    ogg_file_handler->SetChannelPairVolume(channel_pair, value);
}

void OggPlayback::SetChannelPairVolumePan(size_t src_channel_pair,
    int32_t src_channel, int32_t dst_channel, int32_t value) {
    if (state != 1 || !ogg_file_handler)
        return;

    switch (src_channel) {
    case 0:
    case 1:
        break;
    default:
        src_channel = 2;
        break;
    }

    switch (dst_channel) {
    case 0:
    case 1:
    case 2:
    case 3:
        break;
    default:
        dst_channel = 4;
        break;
    }
    ogg_file_handler->SetChannelPairVolumePan(src_channel_pair, src_channel, dst_channel, value);
}

void OggPlayback::SetLoadTimeSeek(float_t value) {
    load_time_seek = value;
}

void OggPlayback::SetMasterVolume(int32_t value) {
    if (state == 1 && ogg_file_handler)
        ogg_file_handler->SetMasterVolume(value);
}

void OggPlayback::SetPath(const std::string& path) {
    data_list[DATA_AFT].load_file(this, path.c_str(), OggPlayback::LoadFile);
}

void OggPlayback::SetPath(std::string&& path) {
    SetPath(path);
}

void OggPlayback::SetPauseState(OggFileHandlerPauseState value) {
    if (state == 1 && ogg_file_handler)
        ogg_file_handler->SetPauseState(value);
}

void OggPlayback::SetPlaybackState(OggFileHandlerPlaybackState value) {
    if (state == 1 && ogg_file_handler)
        ogg_file_handler->SetPlaybackState(value);
}

void OggPlayback::Stop() {
    if (state == 1 && ogg_file_handler && ogg_file_handler->ptr)
        ogg_file_handler->SetPlaybackState(OGG_FILE_HANDLER_PLAYBACK_STATE_UNLOAD);
}

bool OggPlayback::LoadFile(void* data, const char* path, const char* file, uint32_t hash) {
    OggPlayback* op = (OggPlayback*)data;

    std::string _path;
    _path.assign(path);
    _path.append(file);

    if (_path.size() < 4)
        return false;

    std::string ext;
    ext.assign(_path.c_str() + (_path.size() - 4), 4);
    for (char& c : ext)
        if (c >= 'A' && c <= 'Z')
            c += 0x20;

    if (ext.compare(".ogg"))
        return false;

    if (op->state != 1) {
        op->Reset();
        op->ogg_file_handler = ogg_file_handler_storage_get_ogg_file_handler(op->index);
    }

    if (op->ogg_file_handler) {
        op->ogg_file_handler->SetLoadTimeSeek(op->load_time_seek);
        op->ogg_file_handler->SetPath(_path);
        op->state = 1;
    }
    return true;
}

void OggPlayback::SetChannelPairVolumePan(OggPlayback* op) {
    if (!op)
        return;

    int32_t value = ratio_to_db(1.0);

    int32_t _channel_pairs_count = op->GetChannelPairsCount();
    if (!_channel_pairs_count)
        _channel_pairs_count = 2;

    if (_channel_pairs_count >= 3 && _channel_pairs_count <= 4) {
        for (int32_t i = 0; i < 2; i++) {
            op->SetChannelPairVolumePan(i, 0, 0, value);
            op->SetChannelPairVolumePan(i, 1, 1, value);
            op->SetChannelPairVolumePan(i, 0, 2, -10000);
            op->SetChannelPairVolumePan(i, 1, 3, -10000);
        }

        for (int32_t i = _channel_pairs_count - 2; i < _channel_pairs_count; i++) {
            op->SetChannelPairVolumePan(i, 0, 0, -10000);
            op->SetChannelPairVolumePan(i, 1, 1, -10000);
            op->SetChannelPairVolumePan(i, 0, 2, value);
            op->SetChannelPairVolumePan(i, 1, 3, value);
        }
    }
    else
        for (int32_t i = 0; i < _channel_pairs_count; i++) {
            op->SetChannelPairVolumePan(i, 0, 0, value);
            op->SetChannelPairVolumePan(i, 1, 1, value);
            op->SetChannelPairVolumePan(i, 0, 2, value);
            op->SetChannelPairVolumePan(i, 1, 3, value);
        }
}

void ogg_playback_data_init() {
    for (size_t i = 0; i < OGG_PLAYBACK_DATA_COUNT; i++)
        ogg_playback_data[i] = new OggPlayback(i);
}

void ogg_playback_data_free() {
    for (size_t i = 0; i < OGG_PLAYBACK_DATA_COUNT; i++)
        if (ogg_playback_data[i]) {
            delete ogg_playback_data[i];
            ogg_playback_data[i] = 0;
        }

    if (ogg_file_handler_storage_ptr)
        ogg_file_handler_storage_ptr->free_data();
}

OggPlayback* ogg_playback_data_get(size_t index) {
    if (index >= 0 && index < OGG_PLAYBACK_DATA_COUNT)
        return ogg_playback_data[index];
    return 0;
}

void ogg_file_handler_storage_init() {
    if (!ogg_file_handler_storage_init_count)
        ogg_file_handler_storage_ptr = new ogg_file_handler_storage;
    ogg_file_handler_storage_init_count++;
}

void ogg_file_handler_storage_free() {
    if (!--ogg_file_handler_storage_init_count) {
        delete ogg_file_handler_storage_ptr;
        ogg_file_handler_storage_ptr = 0;
    }
}

ogg_file_handler_storage::ogg_file_handler_storage() : clear() {

}

ogg_file_handler_storage::~ogg_file_handler_storage() {
    free_data();
}

void ogg_file_handler_storage::free_data() {
    if (clear)
        return;

    for (p_OggFileHandler*& i : list)
        delete i;
    clear = true;
}

static p_OggFileHandler* ogg_file_handler_storage_get_ogg_file_handler(size_t index) {
    OggFileHandler* ofh = new OggFileHandler(index);
    if (ofh) {
        p_OggFileHandler* pofh = new p_OggFileHandler(ofh);
        if (pofh) {
            std::unique_lock<std::mutex> u_lock(ogg_file_handler_storage_ptr->mtx);
            ogg_file_handler_storage_ptr->list.push_back(pofh);
            return pofh;
        }
        delete ofh;
    }
    return 0;
}
