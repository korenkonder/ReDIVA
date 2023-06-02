/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "pv_game_music.hpp"

pv_game_music* pv_game_music_ptr;

pv_game_music_args::pv_game_music_args() : type(), start(), field_2C() {

}

pv_game_music_args::~pv_game_music_args() {

}

pv_game_music_ogg::pv_game_music_ogg() : playback() {
    playback = ogg_playback_data_get(0);
}

pv_game_music_ogg::~pv_game_music_ogg() {
    if (playback) {
        playback->Reset();
        playback = 0;
    }
}

pv_game_music::pv_game_music() : flags(), pause(), channel_pair_volume(), fade_in(),
fade_out(), no_fade(), no_fade_remain(), fade_out_time_req(), fade_out_action_req(),
type(), start(), end(), play_on_end(), fade_in_time(), field_9C(), loaded(), ogg() {
    volume = 100;
    master_volume = 100;
    for (int32_t& i : channel_pair_volume)
        i = 100;
    fade_out_time = 3.0f;
}

pv_game_music::~pv_game_music() {

}

bool pv_game_music::check_args(int32_t type, std::string&& file_path, float_t start) {
    return args.type == type && !args.file_path.compare(file_path) && args.start == start;
}

void pv_game_music::ctrl(float_t delta_time) {
    if (no_fade && !pause) {
        no_fade_remain -= delta_time;
        if (no_fade_remain <= 0.0f) {
            fade_out.enable = true;
            fade_out.start = get_volume(0);
            fade_out.value = 0;
            fade_out.time = fade_out_time_req;
            fade_out.remain = fade_out_time_req;
            fade_out.action = fade_out_action_req;

            no_fade = false;
            no_fade_remain = 0.0f;
            fade_out_time_req = 0.0f;
            fade_out_action_req = PV_GAME_MUSIC_ACTION_NONE;
        }
    }

    if (fade_in.enable && !pause) {
        fade_in.remain -= delta_time;
        if (fade_in.remain > 0.0f) {
            int32_t value = fade_in.value;
            if (fade_in.time > 0.0f)
                value = (int32_t)((float_t)value - (float_t)(value
                    - fade_in.start) * (fade_in.remain / fade_in.time));
            set_volume_map(0, value);
        }
        else
            fade_in_end();
    }

    if (fade_out.enable && !pause) {
        fade_out.remain -= delta_time;
        if (fade_out.remain > 0.0f) {
            int32_t value = fade_out.value;
            if (fade_out.time > 0.0f)
                value = (int32_t)((float_t)value - (float_t)(value
                    - fade_out.start) * (fade_out.remain / fade_out.time));
            set_volume_map(0, value);
        }
        else
            fade_out_end();
    }

    if (flags & PV_GAME_MUSIC_OGG) {
        OggPlayback* playback = ogg->playback;
        if (playback->GetFileState() == OGG_FILE_HANDLER_FILE_STATE_STOPPED)
            playback->Stop();
        if (playback->GetFileState() == OGG_FILE_HANDLER_FILE_STATE_STOPPED)
            play_or_stop();
        OggPlayback::SetChannelPairVolumePan(playback);
    }
}

void pv_game_music::exclude_flags(pv_game_music_flags flags) {
    if ((this->flags & PV_GAME_MUSIC_OGG) && (flags & PV_GAME_MUSIC_OGG))
        ogg_free();
    enum_and(this->flags, ~flags);
}

void pv_game_music::fade_in_end() {
    if (!fade_in.enable)
        return;

    fade_in.enable = false;
    fade_in.remain = 0.0f;
    set_volume_map(0, fade_in.value);

    switch (fade_in.action) {
    case PV_GAME_MUSIC_ACTION_STOP:
        stop();
        break;
    case PV_GAME_MUSIC_ACTION_PAUSE:
        set_pause(1);
        break;
    case PV_GAME_MUSIC_ACTION_PLAY:
        play();
        break;
    }
}

void pv_game_music::fade_out_end() {
    if (!fade_out.enable)
        return;

    fade_out.enable = false;
    fade_out.remain = 0.0f;
    set_volume_map(0, fade_out.value);

    switch (fade_out.action) {
    case PV_GAME_MUSIC_ACTION_STOP:
        stop();
        break;
    case PV_GAME_MUSIC_ACTION_PAUSE:
        set_pause(1);
        break;
    case PV_GAME_MUSIC_ACTION_PLAY:
        play();
        break;
    }
}

void pv_game_music::file_load(int32_t type, std::string&& file_path, bool play_on_end,
    float_t start, float_t end, float_t fade_in_time, float_t fade_out_time, bool a9) {
    this->type = type;
    this->file_path.assign(file_path);
    this->start = start;
    this->end = end;
    this->play_on_end = play_on_end;
    this->fade_out_time = fade_out_time;
    this->fade_in_time = fade_in_time;
    this->field_9C = a9;

    if (fade_in_time > 0.0f) {
        fade_in.start = 0;
        fade_in.value = 100;
        fade_in.time = fade_in_time;
        fade_in.remain = fade_in_time;
        fade_in.action = PV_GAME_MUSIC_ACTION_NONE;
        fade_in.enable = true;
    }

    if (end <= 0.0f) {
        fade_out.start = get_volume(0);
        fade_out.value = 0;
        fade_out.time = 0.0f;
        fade_out.remain = 0.0f;
        fade_out.action = play_on_end
            ? PV_GAME_MUSIC_ACTION_PLAY : PV_GAME_MUSIC_ACTION_STOP;
    }
    else {
        no_fade = true;
        no_fade_remain = end + fade_in_time;
        fade_out_time_req = fade_out_time;
        fade_out_action_req = play_on_end
            ? PV_GAME_MUSIC_ACTION_PLAY : PV_GAME_MUSIC_ACTION_STOP;
    }
}

int32_t pv_game_music::get_master_volume(int32_t index) {
    if (!index)
        return master_volume;
    return 0;
}

int32_t pv_game_music::get_volume(int32_t index) {
    if (!index)
        return volume;
    return 0;
}

int32_t pv_game_music::include_flags(pv_game_music_flags flags) {
    if (!(this->flags & PV_GAME_MUSIC_OGG) && (flags & PV_GAME_MUSIC_OGG))
        ogg_init();
    enum_or(this->flags, flags);
    return 0;
}

int32_t pv_game_music::load(int32_t type, std::string&& file_path, bool wait_load, float_t time, bool a6) {
    if (type == 4 && (set_ogg_args(std::string(file_path), time, wait_load) < 0))
        return -6;

    reset();
    reset_args();
    set_args(type, std::string(file_path), time, a6);
    pause = true;
    return 0;
}

void pv_game_music::ogg_free() {
    if (ogg) {
        delete ogg;
        ogg = 0;
    }
}

int32_t pv_game_music::ogg_init() {
    if (ogg)
        return 0;

    ogg = new pv_game_music_ogg;
    return ogg->playback ? 0 : -1;
}

int32_t pv_game_music::ogg_load(std::string&& file_path, float_t start) {
    if (!(flags & PV_GAME_MUSIC_OGG))
        return -2;

    start = max_def(start, 0.0f);

    OggPlayback* playback = ogg->playback;

    OggFileHandlerFileState file_state = playback->GetFileState();
    OggFileHandlerPauseState pause_state = playback->GetPauseState();

    if (!check_args(4, std::string(file_path), start)
        || file_state != OGG_FILE_HANDLER_FILE_STATE_PLAYING
        || pause_state != OGG_FILE_HANDLER_PAUSE_STATE_PAUSE) {
        playback->Stop();
        playback->SetLoadTimeSeek(start);
        playback->SetPath(file_path);
    }

    playback->SetPauseState(OGG_FILE_HANDLER_PAUSE_STATE_PLAY);
    ogg->file_path.assign(file_path);
    loaded = true;
    return 0;
}

int32_t pv_game_music::ogg_reset() {
    sound_stream_array_reset();
    include_flags(PV_GAME_MUSIC_OGG);
    return 0;
}

int32_t pv_game_music::ogg_stop() {
    if (!(flags & PV_GAME_MUSIC_OGG))
        return -2;

    ogg->playback->Stop();
    ogg->file_path.clear();
    loaded = false;
    return 0;
}

int32_t pv_game_music::play() {
    return play(type, std::string(file_path), play_on_end, start, end, fade_in_time, fade_out_time, false);
}

int32_t pv_game_music::play(int32_t type, std::string&& file_path, bool play_on_end,
    float_t start, float_t end, float_t fade_in_time, float_t fade_out_time, bool a9) {
    set_volume_map(0, 100);
    if (type == 4 && ogg_load(std::string(file_path), start) < 0)
        return -7;

    reset();
    reset_args();
    file_load(type, std::string(file_path), play_on_end, start, end, fade_in_time, fade_out_time, a9);
    pause = false;
    return 0;
}

void pv_game_music::play_fade_in(int32_t type, std::string&& file_path, float_t start,
    float_t end, bool play_on_end, bool a7, float_t fade_out_time, bool a10) {
    play(type, std::string(file_path), play_on_end, start, end, 0.0f, fade_out_time, a10);
}

int32_t pv_game_music::play_or_stop() {
    if (play_on_end)
        return play();
    else
        return stop();
}

void pv_game_music::reset() {
    fade_in = {};
    fade_out = {};
    no_fade = false;
    no_fade_remain = 0.0f;
    fade_out_time_req = 0.0f;
    fade_out_action_req = PV_GAME_MUSIC_ACTION_NONE;
    type = 0;
    file_path.clear();
    start = 0.0f;
    end = 0.0f;
    play_on_end = false;
    fade_in_time = 0.0f;
    fade_out_time = 3.0f;
    field_9C = false;
}

void pv_game_music::reset_args() {
    args.type = 0;
    args.file_path.clear();
    args.start = 0.0f;
    args.field_2C = false;
}

void pv_game_music::set_args(int32_t type, std::string&& file_path, float_t start, bool a5) {
    args.type = type;
    args.file_path.assign(file_path);
    args.start = start;
    args.field_2C = a5;
}

int32_t pv_game_music::set_channel_pair_volume(int32_t channel_pair, int32_t value) {
    if (!(flags & PV_GAME_MUSIC_OGG))
        return -2;

    ogg->playback->SetChannelPairVolume(channel_pair, ratio_to_db((float_t)value * 0.01f));
    return 0;
}

int32_t pv_game_music::set_channel_pair_volume_map(int32_t channel_pair, int32_t value) {
    if (channel_pair >= 0 && channel_pair <= 3) {
        set_channel_pair_volume(channel_pair, value);
        if (!channel_pair)
            set_channel_pair_volume_map(2, value);
        else if (channel_pair == 1)
            set_channel_pair_volume_map(3, value);
        channel_pair_volume[channel_pair] = value;
        return 0;
    }
    return -1;
}

int32_t pv_game_music::set_fade_out(float_t time, bool stop) {
    reset();
    if (time == 0.0f)
        pv_game_music::stop();
    else {
        fade_out.start = get_volume(0);
        fade_out.value = 0;
        fade_out.time = time;
        fade_out.remain = time;
        fade_out.enable = true;
        fade_out.action = stop ? PV_GAME_MUSIC_ACTION_STOP
            : PV_GAME_MUSIC_ACTION_PAUSE;
    }
    return 0;
}

int32_t pv_game_music::set_master_volume(int32_t value) {
    if (!(flags & PV_GAME_MUSIC_OGG))
        return -2;

    ogg->playback->SetMasterVolume(ratio_to_db(
        (float_t)(value * get_master_volume(0) / 100) * 0.01f));
    return 0;
}

int32_t pv_game_music::set_ogg_args(std::string&& file_path, float_t start, bool wait_load) {
    if (!(flags & PV_GAME_MUSIC_OGG))
        return -2;

    start = max_def(start, 0.0f);

    OggPlayback* playback = ogg->playback;

    OggFileHandlerFileState file_state = playback->GetFileState();
    OggFileHandlerPauseState pause_state = playback->GetPauseState();

    if (!check_args(4, std::string(file_path), start)
        || file_state != OGG_FILE_HANDLER_FILE_STATE_PLAYING
        || pause_state != OGG_FILE_HANDLER_PAUSE_STATE_PAUSE) {
        playback->Stop();
        playback->SetLoadTimeSeek(start);
        playback->SetPath(file_path);
        playback->SetPauseState(OGG_FILE_HANDLER_PAUSE_STATE_PAUSE);
        ogg->file_path.assign(file_path);

        if (wait_load) {
            OggFileHandlerFileState file_state = playback->GetFileState();
            while (file_state != OGG_FILE_HANDLER_FILE_STATE_PLAYING
                && file_state != OGG_FILE_HANDLER_FILE_STATE_STOPPED
                && file_state != OGG_FILE_HANDLER_FILE_STATE_MAX)
                file_state = playback->GetFileState();
        }
    }
    return 0;
}

int32_t pv_game_music::set_ogg_pause_state(uint8_t pause_state) {
    if ((flags & PV_GAME_MUSIC_OGG) == 0)
        return -2;

    OggPlayback* playback = ogg->playback;
    if (playback->GetFileState() == OGG_FILE_HANDLER_FILE_STATE_PLAYING)
        playback->SetPauseState((OggFileHandlerPauseState)pause_state);
    return 0;
}

int32_t pv_game_music::set_pause(int32_t pause) {
    if ((flags & PV_GAME_MUSIC_OGG) != 0)
        set_ogg_pause_state(pause != 0);
    pause = pause == 1;
    return 0;
}

void pv_game_music::set_volume_map(int32_t index, int32_t value) {
    if (!index) {
        if (flags & PV_GAME_MUSIC_OGG)
            set_master_volume(value);
        volume = value;
    }
}

int32_t pv_game_music::stop() {
    if (flags & PV_GAME_MUSIC_OGG)
        ogg_stop();
    reset();
    reset_args();
    pause = false;
    return 0;
}

void pv_game_music::stop_reset_flags() {
    stop();
    exclude_flags(PV_GAME_MUSIC_ALL);
}

void pv_game_music_init() {
    if (!pv_game_music_ptr)
        pv_game_music_ptr = new pv_game_music;
}

pv_game_music* pv_game_music_get() {
    return pv_game_music_ptr;
}

void pv_game_music_free() {
    if (pv_game_music_ptr) {
        delete pv_game_music_ptr;
        pv_game_music_ptr = 0;
    }
}
