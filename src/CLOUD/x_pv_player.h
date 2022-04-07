/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#if defined(CLOUD_DEV)
#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>
#include "../KKdLib/pvpp.h"
#include "../KKdLib/pvsr.h"
#include "../CRE/auth_3d.h"
#include "../CRE/rob.h"
#include "../CRE/task.h"

typedef enum x_pv_player_frame_data_type {
    X_PV_PLAYER_FRAME_DATA_NONE = 0,
    X_PV_PLAYER_FRAME_DATA_PV_END,
    X_PV_PLAYER_FRAME_DATA_BPM,
    X_PV_PLAYER_FRAME_DATA_SONG_EFFECT,
    X_PV_PLAYER_FRAME_DATA_STAGE_EFFECT,
    X_PV_PLAYER_FRAME_DATA_STAGE_EFFECT_ONE_SHOT,
    X_PV_PLAYER_FRAME_DATA_STAGE_CHANGE_EFFECT,
    X_PV_PLAYER_FRAME_DATA_STAGE_CHANGE_EFFECT_END,
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

typedef struct x_pv_player_stage_effect {
    int32_t prev;
    int32_t next;
} x_pv_player_stage_effect;

typedef struct x_pv_player_frame_data {
    int32_t frame;
    x_pv_player_frame_data_type type;
    union {
        int32_t bpm;
        x_pv_player_song_effect song_effect;
        x_pv_player_stage_effect stage_effect;
        int32_t stage_effect_one_shot;
    };
} x_pv_player_frame_data;

class x_pv_player : public Task {
public:
    int32_t pv_id;
    int32_t stage_id;
    pvpp* pp;
    pvsr* sr;

    uint32_t effpv_objset;
    uint32_t stgpv_objset;
    uint32_t stgpvhrc_objset;

    int32_t state;
    int32_t frame;
    int32_t rob_chara_ids[ROB_CHARA_COUNT];

    x_pv_player_glitter* pv_glitter;
    x_pv_player_glitter* stage_glitter;

    std::vector<x_pv_player_frame_data> frame_data;
    x_pv_player_frame_data* curr_frame_data;

    object_database obj_db;
    texture_database tex_db;
    stage_database stage_data;

    std::vector<uint32_t> stage_hashes;
    std::vector<stage_data_modern*> stages_data;

    std::string stage_category;
    std::string pv_category;
    std::string light_category;
    std::string camera_category;

    uint32_t stage_category_hash;
    uint32_t pv_category_hash;

    std::set<std::string> stage_auth_3d_names;
    std::set<std::string> pv_auth_3d_names;

    std::map<uint32_t, int32_t> pv_auth_3d_ids;
    std::map<uint32_t, int32_t> stage_auth_3d_ids;
    int32_t light_auth_3d_id;
    int32_t camera_auth_3d_id;

    x_pv_player();
    virtual ~x_pv_player() override;
    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;

    void Load(int32_t pv_id, int32_t stage_id);
    void Unload();
};

extern x_pv_player x_pv_player_data;

#endif
