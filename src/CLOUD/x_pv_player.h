/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#if defined(CLOUD_DEV)
#pragma once

#include <string>
#include <vector>
#include "../KKdLib/pvpp.h"
#include "../KKdLib/pvsr.h"
#include "../CRE/task.h"

typedef enum x_pv_player_frame_data_type {
    X_PV_PLAYER_FRAME_DATA_NONE = 0,
    X_PV_PLAYER_FRAME_DATA_BPM,
    X_PV_PLAYER_FRAME_DATA_SONG_EFFECT,
    X_PV_PLAYER_FRAME_DATA_STAGE_EFFECT,
} x_pv_player_frame_data_type;

class x_pv_player_glitter {
public:
    std::string name;
    uint32_t hash;

    x_pv_player_glitter(char* name = 0);
    ~x_pv_player_glitter();
};

typedef struct x_pv_player_song_effect {
    bool enable;
    int32_t id;
} x_pv_player_song_effect;

typedef struct x_pv_player_frame_data {
    int32_t frame;
    x_pv_player_frame_data_type type;
    union {
        int32_t bpm;
        x_pv_player_song_effect song_effect;
        int32_t stage_effect;
    };
} x_pv_player_frame_data;

typedef struct x_pv_player : public Task {
public:
    int32_t pv_id;
    int32_t stage_id;
    pvpp* pp;
    pvsr sr;

    x_pv_player_glitter pv_glitter;
    x_pv_player_glitter stage_glitter;
    std::vector<x_pv_player_frame_data> frame_data;

    x_pv_player();
    virtual ~x_pv_player() override;
    virtual bool Ctrl() override;
    virtual void Disp() override;

    void Load(int32_t pv_id, int32_t stage_id);
} x_pv_player;
#endif
