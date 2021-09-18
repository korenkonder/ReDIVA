/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#if defined(VIDEO)
#include "data_player.h"
#include "imgui_helper.h"
#include "../../CRE/fbo_hdr.h"
#include "../../CRE/gl_state.h"
#include "../../CRE/lock.h"
#include "../../CRE/static_var.h"
#include "../../CRE/timer.h"
#include "../../CRE/video_x264.h"

typedef struct data_player_struct {
    bool dispose;
    bool disposed;
    bool imgui_focus;
    video_x264 vid;
    int32_t frame;
    int32_t prev_frame;
    lock lock;
    HANDLE timer;
} data_player_struct;

extern int32_t width;
extern int32_t height;
extern fbo_hdr* hfbo;
extern bool input_locked;

const char* data_player_window_title = "Data Player";

bool data_player_enabled = true;
static data_player_struct data_player;

void data_player_dispose() {
    if (data_player.disposed)
        return;

    if (data_player.vid.error_state >= 0) {
        video_x264_flush(&data_player.vid);
        io_free(&data_player.vid.io);
    }
    video_x264_close(&data_player.vid);
    lock_free(&data_player.lock);
    dispose_timer(data_player.timer);
    data_player_enabled = false;
    data_player.imgui_focus = false;
    data_player.dispose = false;
    data_player.disposed = true;
}

void data_player_draw() {
    if (!data_player_enabled)
        return;

    bool ret = false;
    lock_lock(&data_player.lock);
    ret = data_player.frame != data_player.prev_frame;
    if (!data_player.vid.data)
        ret = true;
    lock_unlock(&data_player.lock);

    if (ret)
        return;

    lock_lock(&data_player.lock);
    gl_state_bind_texture_2d(hfbo->color.color_texture->texture);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_SHORT, data_player.vid.data);
    data_player.vid.fmt = VIDEO_X264_PIC_FORMAT_RGBA16;
    video_x264_load_image_data(&data_player.vid);
    data_player.frame++;
    lock_unlock(&data_player.lock);
}

void data_player_init() {
    bool dispose = false;
    lock_lock(&data_player.lock);
    if (data_player_enabled || data_player.dispose)
        dispose = true;
    lock_unlock(&data_player.lock);

    if (dispose)
        data_player_dispose();

    video_x264_init_data init;
    init.width = hfbo->width;
    init.height = hfbo->height;
    init.fps = 60.0;
    init.flip = true;
    init.preset = VIDEO_X264_PRESET_MEDIUM;
    init.tune = VIDEO_X264_TUNE_NONE;
    init.profile = VIDEO_X264_PROFILE_HIGH;
    init.color_space = VIDEO_X264_COLOR_SPACE_I444;
    init.bit_depth = VIDEO_X264_BIT_DEPTH_10_BIT;
    init.rate_control.method = VIDEO_X264_RATE_CONTROL_CRF;
    init.rate_control.crf = 0.0f;

    video_x264_init(&data_player.vid, &init);
    if (data_player.vid.error_state >= 0)
        io_open(&data_player.vid.io, "test.264", "wb");
    data_player.frame = 0;
    data_player.prev_frame = 0;
    data_player.timer = create_timer();
    lock_init(&data_player.lock);
    data_player_enabled = true;
}

void data_player_imgui() {
    bool ret = false;
    lock_lock(&data_player.lock);
    if (!data_player_enabled) {
        if (!data_player.disposed)
            data_player.dispose = true;
        ret = true;
    }
    else if (data_player.disposed)
        data_player.disposed = false;
    lock_unlock(&data_player.lock);

    if (ret)
        return;

    lock_lock(&data_player.lock);
    float_t w = min((float_t)width / 4.0f, 360.0f);
    float_t h = min((float_t)height, 100.0f);

    igSetNextWindowPos(ImVec2_Empty, ImGuiCond_Appearing, ImVec2_Empty);
    igSetNextWindowSize((ImVec2) { w, h }, ImGuiCond_Appearing);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;

    data_player.imgui_focus = false;
    if (!igBegin(data_player_window_title, &data_player_enabled, window_flags)) {
        data_player.dispose = true;
        goto End;
    }

    data_player.imgui_focus |= igIsWindowFocused(0);

End:
    igEnd();
    lock_unlock(&data_player.lock);
}

void data_player_input() {
    bool ret = false;
    lock_lock(&data_player.lock);
    if (!data_player_enabled)
        ret = true;
    lock_unlock(&data_player.lock);

    if (ret)
        return;

    input_locked |= data_player.imgui_focus;
}

void data_player_render() {
    bool dispose = false;
    bool ret = false;
    lock_lock(&data_player.lock);
    if (data_player.dispose) {
        dispose = true;
        ret = true;
    }
    else if (!data_player_enabled)
        ret = true;
    lock_unlock(&data_player.lock);

    if (dispose)
        data_player_dispose();

    if (ret)
        return;
}

void data_player_video() {
    bool ret = false;
    lock_lock(&data_player.lock);
    if (!data_player_enabled)
        ret = true;

    if (!ret && data_player.vid.error_state < 0)
        ret = true;
    lock_unlock(&data_player.lock);

    if (ret)
        return;

    lock_lock(&data_player.lock);
    if (data_player.frame != data_player.prev_frame) {
        video_x264_encode(&data_player.vid, data_player.prev_frame);
        data_player.prev_frame++;
    }
    lock_unlock(&data_player.lock);
}
#endif
