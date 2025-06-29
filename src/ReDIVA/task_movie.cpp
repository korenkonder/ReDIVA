/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "task_movie.hpp"
#include "../CRE/data.hpp"
#include "../CRE/gl_state.hpp"
#include "movie_play_lib/movie_play_lib.hpp"
#include "movie_play_lib/external_clock.hpp"
#include "movie_play_lib/player.hpp"
#include <atomic>

struct TaskMoviePlayerNoInterop {
    ID3D11Device* d3d11_device;
    ID3D11DeviceContext* d3d11_device_context;
    ID3D11Texture2D* d3d11_texture;
    texture* d3d11_tex;
    std::vector<uint8_t> d3d11_tex_data;

    TaskMoviePlayerNoInterop();
    ~TaskMoviePlayerNoInterop();

    void Create();
    void Ctrl(TaskMovie* task_movie, TaskMovie::Player* player,
        TaskMovie::PlayerVideoParams* player_video_params);
    void GetTextureD3D11(TaskMovie::Player* player, texture*& pp);
    void Release();
    void ReleaseTexture();
    void UpdateD3D11Texture(TaskMovie::Player* player, TaskMovie::PlayerVideoParams* player_video_params);

    static TaskMoviePlayerNoInterop* Get(TaskMovie::Player* player);
};

const int32_t TASK_MOVIE_COUNT = 2;

std::atomic_int32_t movie_external_clock;
std::atomic_int64_t movie_current_time;
std::atomic_int64_t movie_play_time;

TaskMovie task_movie[TASK_MOVIE_COUNT];
std::map<TaskMovie::Player*, TaskMoviePlayerNoInterop> task_movie_player_no_interop;

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
    player_state = MoviePlayLib::Status_Starting;
}

void TaskMovie::Player::Ctrl() {
    player_state = MoviePlayLib::Status_Starting;
    time = 0.0f;

    if (player && !dword_1411A1840) {
        player_state = player->GetStatus();
        time = player->GetCurrentPosition();
    }

    switch (player_state) {
    case MoviePlayLib::Status_NotInitialized:
    case MoviePlayLib::Status_Initializing:
        state = TaskMovie::State::Init;
        break;
    case MoviePlayLib::Status_Initialized:
        player->GetVideoInfo(&video_params);

        duration = player->GetDuration();
        player->Play();

        state = TaskMovie::State::Init;
        break;
    case MoviePlayLib::Status_Starting:
        state = TaskMovie::State::Wait;
        break;
    case MoviePlayLib::Status_Started:
        if (!pause)
            player->Play();

        state = TaskMovie::State::Disp;
        break;
    case MoviePlayLib::Status_Stopping:
        {
            TaskMoviePlayerNoInterop* no_interop = TaskMoviePlayerNoInterop::Get(this);
            if (no_interop) {
                if (no_interop->d3d11_texture && no_interop->d3d11_tex && pause)
                    player->Pause();

                state = TaskMovie::State::Disp;
                break;
            }
        }

        if (interop_texture && interop_texture->GetGLTexture() && pause)
            player->Pause();

        state = TaskMovie::State::Disp;
        break;
    case MoviePlayLib::Status_Stopped:
        if (time <= 0.0f || time >= duration)
            state = TaskMovie::State::Stop;
        break;
    }

    if (state == TaskMovie::State::Disp && player && set_audio_params) {
        MoviePlayLib::AudioVolumes volumes;
        volumes.spk_l_volume = volume;
        volumes.spk_r_volume = volume;
        volumes.field_8 = 0.0f;
        volumes.field_C = 0.0f;
        volumes.hph_l_volume = volume;
        volumes.hph_r_volume = volume;
        if (player->SetVolumes(&volumes) >= 0)
            set_audio_params = false;
    }
}

void TaskMovie::Player::Destroy(Player* ptr) {
    if (!ptr)
        return;

    {
        auto elem = task_movie_player_no_interop.find(ptr);
        if (elem != task_movie_player_no_interop.end()) {
            elem->second.Release();
            task_movie_player_no_interop.erase(elem);
        }
    }

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

void TaskMovie::Player::GetGLTexture(texture*& pp) {
    if (!interop_texture)
        return;

    texture* tex = interop_texture->GetGLTexture();
    if (!tex)
        return;

    pp = tex;
    pp->width = video_params.raw_width;
    pp->height = video_params.raw_height;
}

void TaskMovie::Player::UpdateGLTexture(TaskMovie::PlayerVideoParams* player_video_params) {
    if (interop_texture) {
        interop_texture->Release();
        interop_texture = 0;
    }

    if (player)
        player->GetTextureOGL(&interop_texture);

    if (!player_video_params)
        return;

    if (state == TaskMovie::State::Disp) {
        player_video_params->present_width = video_params.present_width;
        player_video_params->present_height = video_params.present_height;
        player_video_params->raw_width = video_params.raw_width;
        player_video_params->raw_height = video_params.raw_height;
    }
    else {
        player_video_params->present_width = 0;
        player_video_params->present_height = 0;
        player_video_params->raw_width = 0;
        player_video_params->raw_height = 0;
    }
}

TaskMovie::TaskMovie() : player(), player_video_params(), tex(), pause(), wait_play() {
    disp_type = TaskMovie::DispType::SpriteTextute;
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
        TaskMoviePlayerNoInterop* no_interop = TaskMoviePlayerNoInterop::Get(player);
        if (no_interop) {
            no_interop->Ctrl(this, player, player_video_params);
            return false;
        }

        if (player->interop_texture) {
            player->interop_texture->Release();
            player->interop_texture = 0;
        }

        player->UpdateGLTexture(player_video_params);

        if (player_video_params->raw_width && player_video_params->raw_height) {
            if (!tex)
                tex = sub_14041E560();

            if (disp_type != TaskMovie::DispType::None)
                player->GetGLTexture(tex);
        }

        if (player && !player->interop_texture
            && !no_interop && disp_type != TaskMovie::DispType::None) {
            HRESULT hr = player->player->GetTextureOGL(&player->interop_texture);
            if (hr < S_FALSE)
                task_movie_player_no_interop.insert({ player, {} }).first->second.Create();
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
    if (state != TaskMovie::State::Disp || disp_type != TaskMovie::DispType::SpriteTextute || !tex)
        return;

    float_t width = (float_t)player_video_params->present_width;
    if (player_video_params->present_width < 0)
        width += (float_t)UINT64_MAX;
    if (fabsf(width) <= 0.000001f)
        return;

    float_t height = (float_t)player_video_params->present_height;
    if (player_video_params->present_height < 0)
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
        || player->state == TaskMovie::State::Shutdown;
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
        TaskMoviePlayerNoInterop* no_interop = TaskMoviePlayerNoInterop::Get(player);
        if (no_interop)
            no_interop->ReleaseTexture();

        if (player->interop_texture) {
            player->interop_texture->Release();
            player->interop_texture = 0;
        }

        tex = 0;

        if (no_interop)
            no_interop->ReleaseTexture();

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
        MoviePlayLib::CreateExternalClock(TaskMovie::GetElapsedTime, player->external_clock);

    MoviePlayLib::Player::Create(player->player);

    if (player->player) {
        player->player->SetTimeSource(player->external_clock);
        player->player->Open(_path.c_str());
    }
}

bool TaskMovie::Unload() {
    bool res = TaskMovie::Shutdown();
    disp_type = TaskMovie::DispType::SpriteTextute;
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

TaskMoviePlayerNoInterop::TaskMoviePlayerNoInterop() : d3d11_device(),
d3d11_device_context(), d3d11_texture(), d3d11_tex() {

}

TaskMoviePlayerNoInterop::~TaskMoviePlayerNoInterop() {

}

void TaskMoviePlayerNoInterop::Create() {
    D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, 0, 0, 0,
        D3D11_SDK_VERSION, &d3d11_device, 0, &d3d11_device_context);
}

void TaskMoviePlayerNoInterop::Ctrl(TaskMovie* task_movie,
    TaskMovie::Player* player, TaskMovie::PlayerVideoParams* player_video_params) {
    UpdateD3D11Texture(player, player_video_params);

    if (player_video_params->raw_width && player_video_params->raw_height) {
        if (!task_movie->tex)
            task_movie->tex = sub_14041E560();

        if (task_movie->disp_type != TaskMovie::DispType::None)
            GetTextureD3D11(player, task_movie->tex);
    }
}

void TaskMoviePlayerNoInterop::GetTextureD3D11(TaskMovie::Player* player, texture*& pp) {
    if (!d3d11_texture)
        return;

    texture* tex = d3d11_tex;
    if (!tex)
        return;

    D3D11_MAPPED_SUBRESOURCE mapped_res = {};
    if (SUCCEEDED(d3d11_device_context->Map(d3d11_texture, 0, D3D11_MAP_READ, 0, &mapped_res))) {
        const void* data = mapped_res.pData;
        const size_t dx_row_pitch = mapped_res.RowPitch;
        const size_t gl_row_pitch = sizeof(uint8_t) * 4 * tex->width;
        if (gl_row_pitch != dx_row_pitch) {
            if (d3d11_tex_data.size() < gl_row_pitch * tex->height)
                d3d11_tex_data.resize(gl_row_pitch * tex->height);

            const ssize_t row_pitch = min_def(dx_row_pitch, gl_row_pitch);
            const uint8_t* src = (const uint8_t*)data;
            uint8_t* dst = d3d11_tex_data.data();
            for (int32_t y = 0; y < tex->height; y++, dst += gl_row_pitch, src += dx_row_pitch)
                memcpy(dst, src, row_pitch);
            data = d3d11_tex_data.data();
        }

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        if (GLAD_GL_VERSION_4_5) {
            glTextureSubImage2D(tex->glid, 0, 0, 0, tex->width, tex->height,
                GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        else {
            gl_state.bind_texture_2d(tex->glid);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, tex->width, tex->height,
                GL_RGBA, GL_UNSIGNED_BYTE, data);
            gl_state.bind_texture_2d(0);
        }
        d3d11_device_context->Unmap(d3d11_texture, 0);
    }

    pp = tex;
    pp->width = player->video_params.raw_width;
    pp->height = player->video_params.raw_height;
}

void TaskMoviePlayerNoInterop::Release() {
    d3d11_tex_data.clear();
    d3d11_tex_data.shrink_to_fit();

    if (d3d11_tex) {
        texture_release(d3d11_tex);
        d3d11_tex = 0;
    }

    if (d3d11_texture) {
        d3d11_texture->Release();
        d3d11_texture = 0;
    }

    if (d3d11_device_context) {
        d3d11_device_context->Release();
        d3d11_device_context = 0;
    }

    if (d3d11_device) {
        d3d11_device->Release();
        d3d11_device = 0;
    }
}

void TaskMoviePlayerNoInterop::ReleaseTexture() {
    d3d11_tex_data.clear();
    d3d11_tex_data.shrink_to_fit();

    if (d3d11_tex) {
        texture_release(d3d11_tex);
        d3d11_tex = 0;
    }

    if (d3d11_texture) {
        d3d11_texture->Release();
        d3d11_texture = 0;
    }
}

void TaskMoviePlayerNoInterop::UpdateD3D11Texture(TaskMovie::Player* player,
    TaskMovie::PlayerVideoParams* player_video_params) {
    ID3D11Texture2D* d3d_texture = 0;
    if (player->player)
        player->player->GetTextureD3D11(d3d11_device, &d3d_texture);

    if (!d3d_texture)
        return;

    D3D11_TEXTURE2D_DESC desc;
    d3d_texture->GetDesc(&desc);

    if (!d3d11_tex || (d3d11_tex->width != desc.Width || d3d11_tex->height != desc.Height)) {
        if (d3d11_texture) {
            d3d11_texture->Release();
            d3d11_texture = 0;
        }

        d3d11_tex_data.clear();
        d3d11_tex_data.shrink_to_fit();

        if (d3d11_tex) {
            texture_release(d3d11_tex);
            d3d11_tex = 0;
        }

        static uint32_t counter = 0x02;
        if (counter < 0x02)
            counter = 0x02;
        d3d11_tex = texture_load_tex_2d(texture_id(0x2F, counter), GL_RGBA8, desc.Width, desc.Height, 0, 0, 0);
        counter++;

        if (d3d11_tex) {
            gl_state.bind_texture_2d(d3d11_tex->glid);
            const GLint swizzle[] = { GL_BLUE, GL_GREEN, GL_RED, GL_ALPHA };
            glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
            gl_state.bind_texture_2d(0);
        }

        D3D11_TEXTURE2D_DESC tex_desc = {};
        tex_desc.Width = desc.Width;
        tex_desc.Height = desc.Height;
        tex_desc.MipLevels = 1;
        tex_desc.ArraySize = 1;
        tex_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        tex_desc.SampleDesc.Count = 1;
        tex_desc.SampleDesc.Quality = 0;
        tex_desc.Usage = D3D11_USAGE_STAGING;
        tex_desc.BindFlags = 0;
        tex_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        tex_desc.MiscFlags = 0;

        d3d11_device->CreateTexture2D(&tex_desc, 0, &d3d11_texture);
    }

    if (d3d11_texture)
        d3d11_device_context->CopyResource(d3d11_texture, d3d_texture);

    if (d3d_texture) {
        d3d_texture->Release();
        d3d_texture = 0;
    }

    if (!player_video_params)
        return;

    if (player->state == TaskMovie::State::Disp) {
        player_video_params->present_width = player->video_params.present_width;
        player_video_params->present_height = player->video_params.present_height;
        player_video_params->raw_width = player->video_params.raw_width;
        player_video_params->raw_height = player->video_params.raw_height;
    }
    else {
        player_video_params->present_width = 0;
        player_video_params->present_height = 0;
        player_video_params->raw_width = 0;
        player_video_params->raw_height = 0;
    }
}

inline TaskMoviePlayerNoInterop* TaskMoviePlayerNoInterop::Get(TaskMovie::Player* player) {
    auto elem = task_movie_player_no_interop.find(player);
    if (elem != task_movie_player_no_interop.end())
        return &elem->second;
    return 0;
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
