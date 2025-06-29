/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../CRE/sprite.hpp"
#include "../CRE/task.hpp"
#include "../CRE/texture.hpp"
#include "movie_play_lib/movie_play_lib.hpp"

class TaskMovie : public app::Task {
public:
    enum class State {
        Wait = 0,
        Init,
        Disp,
        Stop,
        Shutdown,
    };

    enum class DispType {
        None = 0,
        SpriteTextute,
        Textute,
        Max,
    };

    struct PlayerVideoParams {
        int64_t present_width;
        int64_t present_height;
        int64_t raw_width;
        int64_t raw_height;

        inline PlayerVideoParams() : present_width(), present_height(), raw_width(), raw_height() {

        }
    };

    struct DispParams {
        rectangle rect;
        resolution_mode resolution_mode;
        float_t scale;
        uint32_t field_18;
        int32_t index;

        DispParams();
    };

    struct SprParams {
        DispParams disp;
        spr::SprPrio prio;

        SprParams();
    };

    struct Player {
        bool pause;
        float_t volume;
        bool set_audio_params;
        State state;
        MoviePlayLib::IPlayer* player;
        MoviePlayLib::IMediaClock* external_clock;
        MoviePlayLib::Status player_state;
        MoviePlayLib::VideoInfo video_params;
        MoviePlayLib::IGLDXInteropTexture* interop_texture;
        double_t duration;
        double_t time;

        Player();

        void Ctrl();
        void GetGLTexture(texture*& pp);
        void UpdateGLTexture(TaskMovie::PlayerVideoParams* player_video_params);

        static void Destroy(Player* ptr);

        inline void Destroy() {
            Destroy(this);
        }
    };

    std::string path;
    DispType disp_type;
    SprParams spr_params;
    State state;
    Player* player;
    PlayerVideoParams* player_video_params;
    texture* tex;
    bool pause;
    bool wait_play;

    TaskMovie();
    virtual ~TaskMovie() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void disp() override;

    bool CheckDisp();
    bool CheckState();
    void CtrlPlayer();
    texture* GetTexture();
    void Load(const std::string& path);
    void LoadWait(const std::string& path);
    bool Play();
    bool Reset(const TaskMovie::SprParams& spr_params);
    void SetPause(bool value);
    bool SetVolume(float_t value);
    bool Shutdown();
    void Start(const std::string& path);
    bool Unload();

    static int64_t GetElapsedTime();
};

extern const int32_t TASK_MOVIE_COUNT;

extern TaskMovie* task_movie_get(int32_t index = 0);

extern void movie_current_time_set(int64_t value);
extern void movie_external_clock_set_true();
extern void movie_play_time_set(int64_t value);
extern void movie_play_time_set_begin(int64_t value);
