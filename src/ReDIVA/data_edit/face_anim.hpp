/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../config.hpp"
#include "../../CRE/auth_3d.hpp"
#include "../../KKdLib/database/object.hpp"
#include "../../KKdLib/database/texture.hpp"
#include "../../KKdLib/hash.hpp"
#include "../task_window.hpp"

#if FACE_ANIM
class FaceAnim : public app::TaskWindow {
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

        void patch();
        void reset();
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

    FaceAnim();
    virtual ~FaceAnim() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void window() override;

    void reset();
};

extern FaceAnim face_anim;
#endif
