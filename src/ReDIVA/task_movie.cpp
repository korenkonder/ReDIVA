/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "task_movie.hpp"
#include "../CRE/data.hpp"
#include "movie_play_lib/movie_play_lib.hpp"
#include "movie_play_lib/external_clock.hpp"
#include "movie_play_lib/player.hpp"
#include <atomic>

const int32_t TASK_MOVIE_COUNT = 2;

std::atomic_int32_t movie_external_clock;
std::atomic_int64_t movie_current_time;
std::atomic_int64_t movie_play_time;

TaskMovie task_movie[TASK_MOVIE_COUNT];

static int32_t dword_1411A1840 = 0;

static bool movie_external_clock_get();
static void movie_reset();

static texture* sub_14041E560();

inline TaskMovie::DispParams::DispParams() : rect(), resolution_mode(), field_18() {
    scale = -1.0f;
    index = -1;
}

TaskMovie::SprParams::SprParams() {
    prio = spr::SPR_PRIO_07;

    resolution_struct* res_wind = res_window_get();
    disp.rect.size.x = (float_t)res_wind->width;
    disp.rect.size.y = (float_t)res_wind->height;
    disp.resolution_mode = res_wind->resolution_mode;
}

TaskMovie::Player::Player() : pause(), volume(), state(), player(),
external_clock(), video_params(), interop_texture(), duration(), time() {
    set_audio_params = true;
    player_state = MoviePlayLib::State::Wait;
}

void TaskMovie::Player::Ctrl() {
    player_state = MoviePlayLib::State::Wait;
    time = 0.0f;

    if (player && !dword_1411A1840) {
        player_state = player->GetState();
        time = player->GetTime();
    }

    switch (player_state) {
    case MoviePlayLib::State::None:
    case MoviePlayLib::State::Init:
        state = TaskMovie::State::Init;
        break;
    case MoviePlayLib::State::Open:
        player->GetVideoParams(&video_params);

        duration = player->GetDuration();
        player->Play();

        state = TaskMovie::State::Init;
        break;
    case MoviePlayLib::State::Wait:
        state = TaskMovie::State::Wait;
        break;
    case MoviePlayLib::State::Pause:
        if (!pause)
            player->Play();

        state = TaskMovie::State::Disp;
        break;
    case MoviePlayLib::State::Play:
        if (interop_texture && interop_texture->GetTexture() && pause)
            player->Pause();

        state = TaskMovie::State::Disp;
        break;
    case MoviePlayLib::State::Stop:
        if (time <= 0.0f || time >= duration)
            state = TaskMovie::State::Stop;
        break;
    }

    if (state == TaskMovie::State::Disp && player && set_audio_params) {
        MoviePlayLib::AudioParams audio_params;
        audio_params.spk_l_volume = volume;
        audio_params.spk_r_volume = volume;
        audio_params.field_8 = 0.0f;
        audio_params.field_C = 0.0f;
        audio_params.hph_l_volume = volume;
        audio_params.hph_r_volume = volume;
        if (player->SetAudioParams(&audio_params) >= 0)
            set_audio_params = false;
    }
}

void TaskMovie::Player::Destroy(Player* ptr) {
    if (!ptr)
        return;

    if (ptr->interop_texture) {
        ptr->interop_texture->Release();
        ptr->interop_texture = 0;
    }

    if (ptr->player) {
        ptr->player->Release();
        ptr->player = 0;
    }

    if (ptr->external_clock) {
        ptr->external_clock->Release();
        ptr->external_clock = 0;
    }

    delete ptr;
}

void TaskMovie::Player::GetInteropTexture(texture*& ptr) {
    if (!interop_texture)
        return;

    texture* tex = interop_texture->GetTexture();
    if (!tex)
        return;

    ptr = tex;
    ptr->width = video_params.frame_size_width;
    ptr->height = video_params.frame_size_height;
}

void TaskMovie::Player::UpdateInteropTexture(TaskMovie::PlayerVideoParams* player_video_params) {
    if (interop_texture) {
        interop_texture->Release();
        interop_texture = 0;
    }

    if (player)
        player->GetGLDXIntreropTexture(&interop_texture);

    if (!player_video_params)
        return;

    if (state == TaskMovie::State::Disp) {
        player_video_params->width = video_params.width;
        player_video_params->height = video_params.height;
        player_video_params->frame_size_width = video_params.frame_size_width;
        player_video_params->frame_size_height = video_params.frame_size_height;
    }
    else {
        player_video_params->width = 0;
        player_video_params->height = 0;
        player_video_params->frame_size_width = 0;
        player_video_params->frame_size_height = 0;
    }
}

TaskMovie::TaskMovie() : player(), player_video_params(), tex(), pause(), wait_play() {
    disp_state = TaskMovie::DispState::Disp;
    state = TaskMovie::State::Wait;
}

TaskMovie::~TaskMovie() {

}

bool TaskMovie::init() {
    return true;
}

bool TaskMovie::ctrl() {
    state = TaskMovie::State::Wait;
    if (player) {
        player->Ctrl();
        state = player->state;
    }

    if (CheckState()) {
        if (player->interop_texture) {
            player->interop_texture->Release();
            player->interop_texture = 0;
        }

        player->UpdateInteropTexture(player_video_params);

        if (player_video_params->frame_size_width && player_video_params->frame_size_height) {
            if (!tex)
                tex = sub_14041E560();

            if (disp_state != TaskMovie::DispState::None)
                player->GetInteropTexture(tex);
        }
    }
    return false;
}

bool TaskMovie::dest() {
    Unload();

    if (player_video_params) {
        delete player_video_params;
        player_video_params = 0;
    }

    if (tex)
        tex = 0;

    if (player) {
        player->Destroy();
        player = 0;
    }

    movie_reset();
    return true;
}

void TaskMovie::disp() {
    if (state != TaskMovie::State::Disp || disp_state != TaskMovie::DispState::Disp || !tex)
        return;

    float_t width = (float_t)player_video_params->width;
    if (player_video_params->width < 0)
        width += (float_t)UINT64_MAX;
    if (fabsf(width) <= 0.000001f)
        return;

    float_t height = (float_t)player_video_params->height;
    if (player_video_params->height < 0)
        height += (float_t)UINT64_MAX;
    if (fabsf(height) <= 0.000001f)
        return;

    if (!tex->width || !tex->height)
        return;

    float_t scale = spr_params.disp.scale;

    vec2 sprite_size;
    sprite_size.x = width;
    sprite_size.y = height;

    float_t tex_x = 0.0f;
    float_t tex_y = (float_t)tex->height - height;
    float_t tex_width;
    float_t tex_height;

    if (scale <= 0.0f) {
        scale = max_def(spr_params.disp.rect.size.x / width, spr_params.disp.rect.size.y / height);

        tex_width = (1.0f / scale) * spr_params.disp.rect.size.x;
        tex_height = (1.0f / scale) * spr_params.disp.rect.size.y;

        sprite_size.x = tex_width;
        sprite_size.y = tex_height;

        tex_x = width * 0.5f - tex_width * 0.5f;
        tex_y = (float_t)tex->height - height * 0.5f - tex_height * 0.5f;
    }
    else {
        tex_width = width;
        tex_height = height;
    }

    data_struct* aft_data = &data_list[DATA_AFT];
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    spr::SprArgs args;
    args.Reset();
    args.texture = tex;
    args.SetSpriteSize(sprite_size);
    args.SetTexturePosSize(tex_x, tex_y, tex_width, tex_height);
    args.attr = (spr::SprAttr)(spr::SPR_ATTR_CTR_CC | spr::SPR_ATTR_FLIP_V);
    args.index = spr_params.disp.index;
    args.prio = spr_params.prio;
    args.resolution_mode_screen = spr_params.disp.resolution_mode;
    args.resolution_mode_sprite = spr_params.disp.resolution_mode;
    args.trans.x = spr_params.disp.rect.pos.x + spr_params.disp.rect.size.x * 0.5f;
    args.trans.y = spr_params.disp.rect.pos.y + spr_params.disp.rect.size.y * 0.5f;
    args.trans.z = 0.0f;
    args.scale.x = scale;
    args.scale.y = scale;
    args.scale.z = 1.0f;
    spr::put_sprite(args, aft_spr_db);
}

bool TaskMovie::CheckDisp() {
    if (!player || !wait_play)
        return false;

    player->Ctrl();

    return player->state == TaskMovie::State::Disp
        || player->state == TaskMovie::State::Max;
}

bool TaskMovie::CheckState() {
    return state == TaskMovie::State::Init || state == TaskMovie::State::Disp;
}

void TaskMovie::CtrlPlayer() {
    if (!player)
        return;

    if (pause || wait_play) {
        player->pause = true;
        player->Ctrl();
    }
    else {
        player->pause = false;
        player->Ctrl();
    }
}

texture* TaskMovie::GetTexture() {
    if (app::TaskWork::check_task_ready(this) && CheckState())
        return tex;
    return 0;
}

void TaskMovie::Load(const std::string& path) {
    Shutdown();

    wait_play = false;

    CtrlPlayer();
    Start(path);
}

void TaskMovie::LoadWait(const std::string& path) {
    Shutdown();

    wait_play = true;

    CtrlPlayer();
    Start(path);
}

bool TaskMovie::Play() {
    if (!player || !wait_play)
        return false;

    wait_play = false;

    CtrlPlayer();
    return true;
}

bool TaskMovie::Reset(const TaskMovie::SprParams& spr_params) {
    this->spr_params = spr_params;

    bool ret;
    if (app::TaskWork::check_task_ready(this)) {
        if (player) {
            player->Destroy();
            player = 0;
        }

        player = new TaskMovie::Player;

        ret = true;
    }
    else {
        player = new TaskMovie::Player;
        player_video_params = new TaskMovie::PlayerVideoParams;

        ret = app::TaskWork::add_task(this, "TaskMovie");
    }

    SetVolume(1.0f);

    movie_reset();
    return ret;
}

void TaskMovie::SetPause(bool value) {
    pause = value;

    CtrlPlayer();
}

bool TaskMovie::SetVolume(float_t value) {
    if (!player)
        return false;

    player->volume = value;
    player->set_audio_params = true;
    return true;
}

bool TaskMovie::Shutdown() {
    TaskMovie::Player* player = this->player;

    bool res = false;
    if (player) {
        if (player->interop_texture) {
            player->interop_texture->Release();
            player->interop_texture = 0;
        }

        tex = 0;

        if (player->interop_texture) {
            player->interop_texture->Release();
            player->interop_texture = 0;
        }

        if (player->player) {
            player->player->Release();
            player->player = 0;
        }

        if (player->external_clock) {
            player->external_clock->Release();
            player->external_clock = 0;
        }

        res = true;
    }

    path.clear();
    return res;
}

void TaskMovie::Start(const std::string& path) {
    SetPause(false);

    if (!player)
        return;

    data_struct* aft_data = &data_list[DATA_AFT];

    std::string _path(path);
    aft_data->get_file_path(_path);
    this->path.assign(_path);

    player->external_clock = 0;
    if (movie_external_clock_get())
        MoviePlayLib::ExternalClock::Create(TaskMovie::GetElapsedTime, player->external_clock);

    MoviePlayLib::Player::Create(player->player);

    if (player->player) {
        player->player->SetMediaClock(player->external_clock);
        player->player->Open(_path.c_str());
    }
}

bool TaskMovie::Unload() {
    bool res = TaskMovie::Shutdown();
    disp_state = TaskMovie::DispState::Disp;
    return res;
}

int64_t TaskMovie::GetElapsedTime() {
    if (!movie_external_clock_get())
        return 0;

    return max_def(movie_current_time.load() - movie_play_time.load(), 0);
}

TaskMovie* task_movie_get(int32_t index) {
    if (index >= 0 && index < TASK_MOVIE_COUNT)
        return &task_movie[index];
    return &task_movie[0];
}

texture* task_movie_get_texture(int32_t index) {
    TaskMovie* task_movie = task_movie_get(index);
    return task_movie ? task_movie->tex : 0;
}

void movie_current_time_set(int64_t value) {
    if (movie_external_clock_get())
        movie_current_time = value;
}

void movie_external_clock_set_true() {
    movie_external_clock = 1;
}

void movie_play_time_set(int64_t value) {
    if (movie_external_clock_get())
        movie_play_time = value;
}

void movie_play_time_set_begin(int64_t value) {
    if (movie_external_clock_get() && movie_play_time < 0)
        movie_play_time = value;
}

static bool movie_external_clock_get() {
    return !!movie_external_clock;
}

static void movie_reset() {
    movie_external_clock = 0;
    movie_current_time = 0;
    movie_play_time = -1;
}

static texture* sub_14041E560() {
    return 0;
}
