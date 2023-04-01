/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#if defined(ReDIVA_DEV)
#pragma once

#include "config.hpp"
#include "../CRE/auth_3d.hpp"
#include "../KKdLib/database/object.hpp"
#include "../KKdLib/database/texture.hpp"
#include "../KKdLib/hash.hpp"
#include "task_window.hpp"

#if DATA_EDIT
class DataEdit : public app::TaskWindow {
public:
    struct Auth3D {
        string_hash category;
        string_hash file;
        string_hash object_set;
        auth_3d_id id;

        object_database obj_db;
        texture_database tex_db;

        Auth3D();
        ~Auth3D();

        void Patch();
        void Reset();
    };

    int32_t chara_id;
    int32_t state;
    uint32_t mot_set_id;
    uint32_t motion_id;
    Auth3D auth_3d;
    std::string ogg_path;
    bool play;
    bool sound_play;
    int32_t frame;
    int32_t frame_count;

    DataEdit();
    virtual ~DataEdit() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Window() override;

    void Reset();
};

extern DataEdit data_edit;
#endif
#endif