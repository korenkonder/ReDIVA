/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../CRE/ogg_vorbis.hpp"

enum pv_game_music_action {
    PV_GAME_MUSIC_ACTION_NONE = 0,
    PV_GAME_MUSIC_ACTION_STOP,
    PV_GAME_MUSIC_ACTION_PAUSE,
    PV_GAME_MUSIC_ACTION_PLAY,
    PV_GAME_MUSIC_ACTION_MAX,
};

enum pv_game_music_flags : uint8_t {
    PV_GAME_MUSIC_FLAG_1  = 0x01,
    PV_GAME_MUSIC_AIX     = 0x02,
    PV_GAME_MUSIC_OGG     = 0x04,
    PV_GAME_MUSIC_FLAG_8  = 0x08,
    PV_GAME_MUSIC_FLAG_10 = 0x10,
    PV_GAME_MUSIC_FLAG_20 = 0x20,
    PV_GAME_MUSIC_FLAG_40 = 0x40,
    PV_GAME_MUSIC_FLAG_80 = 0x80,

    PV_GAME_MUSIC_ALL     = 0xFF,
};

struct pv_game_music_fade {
    int32_t start;
    int32_t value;
    float_t time;
    float_t remain;
    pv_game_music_action action;
    bool enable;
};

struct pv_game_music_args {
    int32_t type;
    std::string file_path;
    float_t start;
    bool field_2C;

    pv_game_music_args();
    ~pv_game_music_args();
};

struct pv_game_music_ogg {
    OggPlayback* playback;
    std::string file_path;

    pv_game_music_ogg();
    ~pv_game_music_ogg();
};

struct pv_game_music {
    pv_game_music_flags flags;
    bool pause;
    int32_t volume;
    int32_t master_volume;
    int32_t channel_pair_volume[4];
    pv_game_music_fade fade_in;
    pv_game_music_fade fade_out;
    bool no_fade;
    float_t no_fade_remain;
    float_t fade_out_time_req;
    pv_game_music_action fade_out_action_req;
    int32_t type;
    std::string file_path;
    float_t start;
    float_t end;
    bool play_on_end;
    float_t fade_in_time;
    float_t fade_out_time;
    bool field_9C;
    pv_game_music_args args;
    bool loaded;
    pv_game_music_ogg* ogg;

    pv_game_music();
    ~pv_game_music();

    bool check_args(int32_t type, std::string&& file_path, float_t start);
    void ctrl(float_t delta_time);
    void exclude_flags(pv_game_music_flags flags);
    void fade_in_end();
    void fade_out_end();
    void file_load(int32_t type, std::string&& file_path, bool play_on_end,
        float_t start, float_t end, float_t fade_in_time, float_t fade_out_time, bool a9);
    int32_t get_master_volume(int32_t index);
    int32_t get_volume(int32_t index);
    int32_t include_flags(pv_game_music_flags flags);
    int32_t load(int32_t type, std::string&& file_path, bool wait_load, float_t time, bool a6);
    void ogg_free();
    int32_t ogg_init();
    int32_t ogg_load(std::string&& file_path, float_t start);
    int32_t ogg_reset();
    int32_t ogg_stop();
    int32_t play();
    int32_t play(int32_t type, std::string&& file_path, bool play_on_end,
        float_t start, float_t end, float_t fade_in_time, float_t fade_out_time, bool a9);
    void play_fade_in(int32_t type, std::string&& file_path, float_t start,
        float_t end, bool play_on_end, bool a7, float_t fade_out_time, bool a10);
    int32_t play_or_stop();
    void reset();
    void reset_args();
    void set_args(int32_t type, std::string&& file_path, float_t start, bool a5);
    int32_t set_channel_pair_volume(int32_t channel_pair, int32_t value);
    int32_t set_channel_pair_volume_map(int32_t channel_pair, int32_t value);
    int32_t set_fade_out(float_t time, bool stop);
    int32_t set_master_volume(int32_t value);
    int32_t set_ogg_args(std::string&& file_path, float_t start, bool wait_load);
    int32_t set_ogg_pause_state(uint8_t pause_state);
    int32_t set_pause(int32_t pause);
    void set_volume_map(int32_t index, int32_t value);
    int32_t stop();
    void stop_reset_flags();
};

extern void pv_game_music_init();
extern pv_game_music* pv_game_music_get();
extern void pv_game_music_free();
