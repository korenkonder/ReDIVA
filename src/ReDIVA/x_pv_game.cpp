/*
    by korenkonder
    GitHub/GitLab: korenkonder

    Some code is from LearnOpenGL
*/

#if defined(ReDIVA_DEV)
#include "x_pv_game.hpp"
#include <set>
#include "../CRE/light_param/light.hpp"
#include "../CRE/light_param.hpp"
#include "../CRE/data.hpp"
#include "../CRE/item_table.hpp"
#include "../CRE/object.hpp"
#include "../CRE/pv_expression.hpp"
#include "../CRE/pv_param.hpp"
#include "../KKdLib/farc.hpp"
#include "../KKdLib/interpolation.hpp"
#include "../KKdLib/sort.hpp"
#if BAKE_PNG
#include <lodepng/lodepng.h>
#endif
#include "imgui_helper.hpp"
#include "input.hpp"

enum dsc_x_func {
    DSC_X_END = 0,
    DSC_X_TIME,
    DSC_X_MIKU_MOVE,
    DSC_X_MIKU_ROT,
    DSC_X_MIKU_DISP,
    DSC_X_MIKU_SHADOW,
    DSC_X_TARGET,
    DSC_X_SET_MOTION,
    DSC_X_SET_PLAYDATA,
    DSC_X_EFFECT,
    DSC_X_FADEIN_FIELD,
    DSC_X_EFFECT_OFF,
    DSC_X_SET_CAMERA,
    DSC_X_DATA_CAMERA,
    DSC_X_CHANGE_FIELD,
    DSC_X_HIDE_FIELD,
    DSC_X_MOVE_FIELD,
    DSC_X_FADEOUT_FIELD,
    DSC_X_EYE_ANIM,
    DSC_X_MOUTH_ANIM,
    DSC_X_HAND_ANIM,
    DSC_X_LOOK_ANIM,
    DSC_X_EXPRESSION,
    DSC_X_LOOK_CAMERA,
    DSC_X_LYRIC,
    DSC_X_MUSIC_PLAY,
    DSC_X_MODE_SELECT,
    DSC_X_EDIT_MOTION,
    DSC_X_BAR_TIME_SET,
    DSC_X_SHADOWHEIGHT,
    DSC_X_EDIT_FACE,
    DSC_X_DUMMY,
    DSC_X_PV_END,
    DSC_X_SHADOWPOS,
    DSC_X_EDIT_LYRIC,
    DSC_X_EDIT_TARGET,
    DSC_X_EDIT_MOUTH,
    DSC_X_SET_CHARA,
    DSC_X_EDIT_MOVE,
    DSC_X_EDIT_SHADOW,
    DSC_X_EDIT_EYELID,
    DSC_X_EDIT_EYE,
    DSC_X_EDIT_ITEM,
    DSC_X_EDIT_EFFECT,
    DSC_X_EDIT_DISP,
    DSC_X_EDIT_HAND_ANIM,
    DSC_X_AIM,
    DSC_X_HAND_ITEM,
    DSC_X_EDIT_BLUSH,
    DSC_X_NEAR_CLIP,
    DSC_X_CLOTH_WET,
    DSC_X_LIGHT_ROT,
    DSC_X_SCENE_FADE,
    DSC_X_TONE_TRANS,
    DSC_X_SATURATE,
    DSC_X_FADE_MODE,
    DSC_X_AUTO_BLINK,
    DSC_X_PARTS_DISP,
    DSC_X_TARGET_FLYING_TIME,
    DSC_X_CHARA_SIZE,
    DSC_X_CHARA_HEIGHT_ADJUST,
    DSC_X_ITEM_ANIM,
    DSC_X_CHARA_POS_ADJUST,
    DSC_X_SCENE_ROT,
    DSC_X_EDIT_MOT_SMOOTH_LEN,
    DSC_X_PV_BRANCH_MODE,
    DSC_X_DATA_CAMERA_START,
    DSC_X_MOVIE_PLAY,
    DSC_X_MOVIE_DISP,
    DSC_X_WIND,
    DSC_X_OSAGE_STEP,
    DSC_X_OSAGE_MV_CCL,
    DSC_X_CHARA_COLOR,
    DSC_X_SE_EFFECT,
    DSC_X_CHARA_SHADOW_QUALITY,
    DSC_X_STAGE_SHADOW_QUALITY,
    DSC_X_COMMON_LIGHT,
    DSC_X_TONE_MAP,
    DSC_X_IBL_COLOR,
    DSC_X_REFLECTION,
    DSC_X_CHROMATIC_ABERRATION,
    DSC_X_STAGE_SHADOW,
    DSC_X_REFLECTION_QUALITY,
    DSC_X_PV_END_FADEOUT,
    DSC_X_CREDIT_TITLE,
    DSC_X_BAR_POINT,
    DSC_X_BEAT_POINT,
    DSC_X_RESERVE1,
    DSC_X_PV_AUTH_LIGHT_PRIORITY,
    DSC_X_PV_CHARA_LIGHT,
    DSC_X_PV_STAGE_LIGHT,
    DSC_X_TARGET_EFFECT,
    DSC_X_FOG,
    DSC_X_BLOOM,
    DSC_X_COLOR_CORRECTION,
    DSC_X_DOF,
    DSC_X_CHARA_ALPHA,
    DSC_X_AUTO_CAPTURE_BEGIN,
    DSC_X_MANUAL_CAPTURE,
    DSC_X_TOON_EDGE,
    DSC_X_SHIMMER,
    DSC_X_ITEM_ALPHA,
    DSC_X_MOVIE_CUT,
    DSC_X_EDIT_CAMERA_BOX,
    DSC_X_EDIT_STAGE_PARAM,
    DSC_X_EDIT_CHANGE_FIELD,
    DSC_X_MIKUDAYO_ADJUST,
    DSC_X_LYRIC_2,
    DSC_X_LYRIC_READ,
    DSC_X_LYRIC_READ_2,
    DSC_X_ANNOTATION,
    DSC_X_STAGE_EFFECT,
    DSC_X_SONG_EFFECT,
    DSC_X_SONG_EFFECT_ATTACH,
    DSC_X_LIGHT_AUTH,
    DSC_X_FADE,
    DSC_X_SET_STAGE_EFFECT_ENV,
    DSC_X_RESERVE2,
    DSC_X_COMMON_EFFECT_AET_FRONT,
    DSC_X_COMMON_EFFECT_AET_FRONT_LOW,
    DSC_X_COMMON_EFFECT_PARTICLE,
    DSC_X_SONG_EFFECT_ALPHA_SORT,
    DSC_X_LOOK_CAMERA_FACE_LIMIT,
    DSC_X_ITEM_LIGHT,
    DSC_X_CHARA_EFFECT,
    DSC_X_MARKER,
    DSC_X_CHARA_EFFECT_CHARA_LIGHT,
    DSC_X_ENABLE_COMMON_LIGHT_TO_CHARA,
    DSC_X_ENABLE_FXAA,
    DSC_X_ENABLE_TEMPORAL_AA,
    DSC_X_ENABLE_REFLECTION,
    DSC_X_BANK_BRANCH,
    DSC_X_BANK_END,
    DSC_X_NULL1,
    DSC_X_NULL2,
    DSC_X_NULL3,
    DSC_X_NULL4,
    DSC_X_NULL5,
    DSC_X_NULL6,
    DSC_X_NULL7,
    DSC_X_NULL8,
    DSC_X_VR_LIVE_MOVIE,
    DSC_X_VR_CHEER,
    DSC_X_VR_CHARA_PSMOVE,
    DSC_X_VR_MOVE_PATH,
    DSC_X_VR_SET_BASE,
    DSC_X_VR_TECH_DEMO_EFFECT,
    DSC_X_VR_TRANSFORM,
    DSC_X_GAZE,
    DSC_X_TECH_DEMO_GESUTRE,
    DSC_X_VR_CHEMICAL_LIGHT_COLOR,
    DSC_X_VR_LIVE_MOB,
    DSC_X_VR_LIVE_HAIR_OSAGE,
    DSC_X_VR_LIVE_LOOK_CAMERA,
    DSC_X_VR_LIVE_CHEER,
    DSC_X_VR_LIVE_GESTURE,
    DSC_X_VR_LIVE_CLONE,
    DSC_X_VR_LOOP_EFFECT,
    DSC_X_VR_LIVE_ONESHOT_EFFECT,
    DSC_X_VR_LIVE_PRESENT,
    DSC_X_VR_LIVE_TRANSFORM,
    DSC_X_VR_LIVE_FLY,
    DSC_X_VR_LIVE_CHARA_VOICE,
};

#define DOF_BAKE 0

#if DOF_BAKE
struct dof_cam {
    std::vector<float_t> position_x;
    std::vector<float_t> position_y;
    std::vector<float_t> position_z;
    std::vector<float_t> focus;
    std::vector<float_t> focus_range;
    std::vector<float_t> fuzzing_range;
    std::vector<float_t> ratio;
    std::vector<std::pair<int32_t, bool>> enable_frame;
    int32_t frame;
    bool enable;

    dof_cam();
    ~dof_cam();
};
#endif

#if DOF_BAKE
dof_cam dof_cam_data;
#endif
x_pv_game* x_pv_game_ptr;
XPVGameSelector x_pv_game_selector;

extern render_context* rctx_ptr;

static float_t dsc_time_to_frame(int64_t time);
static void x_pv_game_chara_item_alpha_callback(void* data, int32_t chara_id, int32_t type, float_t alpha);
static void x_pv_game_change_field(x_pv_game* xpvgm, int32_t field, int64_t dsc_time, int64_t curr_time);
static bool x_pv_game_dsc_process(x_pv_game* xpvgm, int64_t curr_time);
//static void x_pv_game_map_auth_3d_to_mot(x_pv_game* xpvgm, bool add_keys);
static void x_pv_game_reset_field(x_pv_game* xpvgm);

static void pv_game_dsc_data_find_playdata_item_anim(x_pv_game* xpvgm, int32_t chara_id);
static void pv_game_dsc_data_find_playdata_set_motion(x_pv_game* xpvgm, int32_t chara_id);
static void pv_game_dsc_data_find_set_motion(x_pv_game* xpvgm);

bool x_pv_bar_beat_data::compare_bar_time_less(float_t time) {
    return (time + 0.0001f) < bar_time;
}

x_pv_bar_beat::x_pv_bar_beat() : curr_time(), delta_time(), next_bar_time(),
curr_bar_time(), next_beat_time(), curr_beat_time(), bar(), counter() {
    reset();
}

x_pv_bar_beat::~x_pv_bar_beat() {

}

float_t x_pv_bar_beat::get_bar_current_frame() {
    return (curr_time - curr_bar_time) / (next_bar_time - curr_bar_time) * 120.0f;
}

float_t x_pv_bar_beat::get_delta_frame() {
    return delta_time / (next_bar_time - curr_bar_time) * 120.0f;
}

int32_t x_pv_bar_beat::get_bar_beat_from_time(int32_t* beat, float_t time) {
    if (!data.size()) {
        int32_t bar = (int32_t)(float_t)(time / divisor);
        if (beat)
            *beat = (int32_t)(4.0f * (time - (float_t)(int32_t)(time / divisor) * divisor) / divisor) + 1;
        return bar + 1;
    }

    x_pv_bar_beat_data* d = data.data();
    const size_t size = data.size();

    size_t length = size;
    size_t temp;
    while (length > 0)
        if (d[temp = length / 2].compare_bar_time_less(time))
            length = temp;
        else {
            d += temp + 1;
            length -= temp + 1;
        }

    if (d == data.data() + size) {
        float_t divisor = this->divisor;
        float_t diff_time = time - data.back().bar_time;

        if (size > 1)
            divisor = data[size - 1].bar_time - data[size - 2].bar_time;

        if (beat)
            *beat = (int32_t)((float_t)data.back().beat_count * (diff_time
                - (float_t)(int32_t)(diff_time / divisor) * divisor) / divisor) + 1;
        return (int32_t)(diff_time / divisor) + data.back().bar;
    }

    int32_t bar = d->bar - 1;
    if (beat) {
        *beat = 1;
        if (d == data.data())
            *beat = (int32_t)((float_t)d->beat_count * time / d->bar_time) + 1;
        else {
            int32_t v18 = d[-1].beat_count - 1;
            if (v18 >= 0) {
                float_t* v19 = &d[-1].beat_time[v18];
                for (; *v19 > time; v19--)
                    if (--v18 < 0)
                        return bar;
                *beat = v18 + 1;
            }
        }
    }
    return bar;
}

float_t x_pv_bar_beat::get_next_beat_time(float_t time) {
    x_pv_bar_beat_data* d = data.data();
    const size_t size = data.size();

    size_t length = size;
    size_t temp;
    while (length > 0)
        if (d[temp = length / 2].compare_bar_time_less(time))
            length = temp;
        else {
            d += temp + 1;
            length -= temp + 1;
        }

    if (d == data.data() + size) {
        float_t v12 = 0.0f;
        float_t v13 = divisor * 0.25f;
        if (size) {
            v12 = data[size - 1].bar_time;
            v13 = v12 - data[size - 2].beat_time[data[size - 2].beat_count - 1];
        }

        float_t beat_time = (float_t)((int32_t)((time - v12) / v13) + 1) * v13 + v12;
        if (beat_time < time + 0.00001f)
            beat_time += v13;
        return beat_time;
    }
    else if (d != data.data()) {
        int32_t v9 = d[-1].beat_count - 1;
        if (v9 >= 0) {
            float_t* beat_time = &d[-1].beat_time[v9];
            for (; time >= *beat_time; beat_time--)
                if (--v9 < 0)
                    return d->bar_time;
            return *beat_time;
        }
    }
    return d->bar_time;
}

float_t x_pv_bar_beat::get_time_from_bar_beat(int32_t bar, int32_t beat) {
    if (bar < 0)
        return 0.0f;

    x_pv_bar_beat_data* d = data.data();
    const size_t size = data.size();

    int32_t beat_count;
    if (size) {
        int32_t bar_diff = bar - d->bar;
        if (bar_diff < 0)
            beat_count = d->beat_count;
        else if (size > bar_diff)
            beat_count = d[bar_diff].beat_count;
        else
            beat_count = d[size - 1].beat_count;

        if (!beat_count)
            beat_count = 4;
    }
    else
        beat_count = 4;

    beat--;
    if (!size)
        return (float_t)beat * divisor / (float_t)beat_count + (bar - 1) * divisor;

    int32_t bar_diff = bar - d->bar;
    if (bar_diff < 0)
        return 0.0f;

    if (size <= bar_diff) {
        float_t divisor = this->divisor;
        if (size > 1)
            divisor = d[size - 1].bar_time - d[size - 2].bar_time;
        return  (float_t)beat * divisor / (float_t)beat_count
            + (bar - d[size - 1].bar) * divisor + d[size - 1].bar_time;
    }
    else if (beat < 0 || beat >= d[bar_diff].beat_count)
        return d[bar_diff].bar_time;
    else
        return d[bar_diff].beat_time[beat];
}

void x_pv_bar_beat::reset() {
    curr_time = 0.0f;
    delta_time = 0.0f;
    next_bar_time = 0.0f;
    curr_bar_time = 0.0f;
    next_beat_time = 0.0f;
    curr_beat_time = 0.0f;
    divisor = 2.0f;
    bar = 0;
    data.clear();
    data.shrink_to_fit();
}

void x_pv_bar_beat::reset_time() {
    curr_time = 0.0f;
    delta_time = 0.0f;

    x_pv_bar_beat_data* d = data.data();
    const size_t size = data.size();

    size_t length = size;
    size_t temp;
    while (length > 0)
        if (d[temp = length / 2].compare_bar_time_less(next_bar_time))
            length = temp;
        else {
            d += temp + 1;
            length -= temp + 1;
        }

    float_t next_bar_time;
    if (d == data.data() + data.size()) {
        float_t bar_time = 0.0f;
        float_t divisor = this->divisor;
        if (data.size())
            bar_time = data.back().bar_time;

        next_bar_time = (float_t)((int32_t)((-bar_time) / divisor) + 1) * divisor + bar_time;
        if (next_bar_time < 0.00001f)
            next_bar_time += divisor;
    }
    else
        next_bar_time = d->bar_time;

    curr_bar_time = 0.0f;
    this->next_bar_time = next_bar_time;
    curr_beat_time = 0.0f;
    next_beat_time = get_next_beat_time(0.0f);
    bar = 1;
    counter = 0;
}

void x_pv_bar_beat::set_frame(float_t curr_frame, float_t delta_frame) {
    set_time(curr_frame * (float_t)(1.0 / 60.0), delta_frame * (float_t)(1.0 / 60.0));
}

void x_pv_bar_beat::set_time(float_t curr_time, float_t delta_time) {
    if (next_bar_time <= 0.0f)
        return;

    this->curr_time = curr_time;
    this->delta_time = delta_time;
    counter = 0;

    float_t next_bar_time = this->next_bar_time;
    while (curr_time >= next_bar_time) {
        curr_bar_time = next_bar_time;
        bar++;
        counter++;

        x_pv_bar_beat_data* d = data.data();
        const size_t size = data.size();

        size_t length = size;
        size_t temp;
        while (length > 0)
            if (d[temp = length / 2].compare_bar_time_less(next_bar_time))
                length = temp;
            else {
                d += temp + 1;
                length -= temp + 1;
            }

        if (d == data.data() + data.size()) {
            float_t v11 = 0.0f;
            float_t v12 = divisor;
            if (data.size())
                v11 = data.back().bar_time;

            float_t _next_bar_time = (float_t)((int32_t)((next_bar_time - v11) / v12) + 1) * v12 + v11;
            if (_next_bar_time < next_bar_time + 0.00001f)
                _next_bar_time += v12;
            next_bar_time = _next_bar_time;
        }
        else
            next_bar_time = d->bar_time;
        this->next_bar_time = next_bar_time;
    }

    while (curr_time >= next_beat_time) {
        curr_beat_time = next_beat_time;
        next_beat_time = get_next_beat_time(next_beat_time);
    }
}

BPMFrameRateControl::BPMFrameRateControl() : bar_beat() {

}

BPMFrameRateControl::~BPMFrameRateControl() {

}

float_t BPMFrameRateControl::GetDeltaFrame() {
    if (bar_beat)
        return bar_beat->get_delta_frame();
    return 0.0f;
}

void BPMFrameRateControl::Reset() {
    bar_beat = 0;
    SetFrameSpeed(1.0f);
}

XPVFrameRateControl::XPVFrameRateControl() {

}

XPVFrameRateControl::~XPVFrameRateControl() {

}

float_t XPVFrameRateControl::GetDeltaFrame() {
    return get_delta_frame() * frame_speed;
}

void XPVFrameRateControl::Reset() {
    frame_speed = 1.0f;
}

x_pv_play_data_motion_data::x_pv_play_data_motion_data() : rob_chr(), current_time(),
duration(), start_pos(), end_pos(), start_rot(), end_rot() {
    mot_smooth_len = 12.0f;
}

x_pv_play_data_motion_data::~x_pv_play_data_motion_data() {

}

void x_pv_play_data_motion_data::reset() {
    rob_chr = 0;
    current_time = 0.0f;
    duration = 0.0f;
    start_pos = vec3_null;
    end_pos = vec3_null;
    start_rot = 0.0f;
    end_rot = 0.0f;
    mot_smooth_len = 12.0f;
    set_motion.clear();
    set_motion.shrink_to_fit();
    set_item.clear();
    set_item.shrink_to_fit();
}

x_pv_play_data::x_pv_play_data() : rob_chr(), disp() {

}

x_pv_play_data::~x_pv_play_data() {

}

void x_pv_play_data::reset() {
    motion.clear();
    motion.shrink_to_fit();
    rob_chr = 0;
    set_motion.clear();
    disp = true;
    motion_data.reset();
}

/*x_pv_game_a3da_to_mot_keys::x_pv_game_a3da_to_mot_keys() {

}

x_pv_game_a3da_to_mot_keys::~x_pv_game_a3da_to_mot_keys() {

}

x_pv_game_a3da_to_mot::x_pv_game_a3da_to_mot(int32_t auth_3d_id) {
    this->auth_3d_id = auth_3d_id;
    gblctr = -1;
    n_hara = -1;
    n_hara_y = -1;
    j_hara_wj = -1;
    n_kosi = -1;
    j_mune_wj = -1;
    n_mune_kl = -1;
    j_mune_b_wj = -1;
    j_kubi_wj = -1;
    n_kao = -1;
    j_kao_wj = -1;
    j_kao_normal_wj = -1;
    j_kao_close_wj = -1;
    j_kao_smile_wj = -1;
    j_kao_close_half_wj = -1;
    j_kao_smile_half_wj = -1;
    j_kuti_l_wj = -1;
    j_kuti_u_wj = -1;
    j_kuti_d_wj = -1;
    j_kuti_r_wj = -1;
    j_eye_r_wj = -1;
    j_eye_l_wj = -1;
    n_waki_l = -1;
    j_waki_l_wj = -1;
    n_kata_l = -1;
    j_kata_l_wj = -1;
    j_ude_l_wj = -1;
    j_te_l_wj = -1;
    j_te_sizen2_l_wj = -1;
    j_te_close_l_wj = -1;
    j_te_reset_l_wj = -1;
    n_waki_r = -1;
    j_waki_r_wj = -1;
    n_kata_r = -1;
    j_kata_r_wj = -1;
    j_ude_r_wj = -1;
    j_te_r_wj = -1;
    j_te_sizen2_r_wj = -1;
    j_te_close_r_wj = -1;
    j_te_reset_r_wj = -1;
    j_te_one_r_wj = -1;
    j_kosi_wj = -1;
    n_momo_l = -1;
    j_momo_l_wj = -1;
    j_sune_l_wj = -1;
    j_asi_l_wj = -1;
    n_momo_r = -1;
    j_momo_r_wj = -1;
    j_sune_r_wj = -1;
    j_asi_r_wj = -1;
}

x_pv_game_a3da_to_mot::~x_pv_game_a3da_to_mot() {

}
static int32_t get_bone_index(auth_3d_object_hrc* oh, const char* name) {
    uint32_t name_hash = hash_utf8_murmurhash(name);
    for (auth_3d_object_node& i : oh->node)
        if (hash_string_murmurhash(i.name) == name_hash)
            return (int32_t)(&i - oh->node.data());
    return -1;
}

void x_pv_game_a3da_to_mot::get_bone_indices(auth_3d_object_hrc* oh) {
    gblctr = get_bone_index(oh, "gblctr");
    n_hara = get_bone_index(oh, "n_hara");
    n_hara_y = get_bone_index(oh, "n_hara_y");
    j_hara_wj = get_bone_index(oh, "j_hara_wj");
    n_kosi = get_bone_index(oh, "n_kosi");
    j_mune_wj = get_bone_index(oh, "j_mune_wj");
    n_mune_kl = get_bone_index(oh, "n_mune_kl");
    j_mune_b_wj = get_bone_index(oh, "j_mune_b_wj");
    j_kubi_wj = get_bone_index(oh, "j_kubi_wj");
    n_kao = get_bone_index(oh, "n_kao");
    j_kao_wj = get_bone_index(oh, "j_kao_wj");
    j_kao_normal_wj = get_bone_index(oh, "j_kao_normal_wj");
    j_kao_close_wj = get_bone_index(oh, "j_kao_close_wj");
    j_kao_smile_wj = get_bone_index(oh, "j_kao_smile_wj");
    j_kao_close_half_wj = get_bone_index(oh, "j_kao_close_half_wj");
    j_kao_smile_half_wj = get_bone_index(oh, "j_kao_smile_half_wj");
    j_kuti_l_wj = get_bone_index(oh, "j_kuti_l_wj");
    j_kuti_u_wj = get_bone_index(oh, "j_kuti_u_wj");
    j_kuti_d_wj = get_bone_index(oh, "j_kuti_d_wj");
    j_kuti_r_wj = get_bone_index(oh, "j_kuti_r_wj");
    j_eye_r_wj = get_bone_index(oh, "j_eye_r_wj");
    j_eye_l_wj = get_bone_index(oh, "j_eye_l_wj");
    n_waki_l = get_bone_index(oh, "n_waki_l");
    j_waki_l_wj = get_bone_index(oh, "j_waki_l_wj");
    n_kata_l = get_bone_index(oh, "n_kata_l");
    j_kata_l_wj = get_bone_index(oh, "j_kata_l_wj");
    j_ude_l_wj = get_bone_index(oh, "j_ude_l_wj");
    j_te_l_wj = get_bone_index(oh, "j_te_l_wj");
    j_te_sizen2_l_wj = get_bone_index(oh, "j_te_sizen2_l_wj");
    j_te_close_l_wj = get_bone_index(oh, "j_te_close_l_wj");
    j_te_reset_l_wj = get_bone_index(oh, "j_te_reset_l_wj");
    n_waki_r = get_bone_index(oh, "n_waki_r");
    j_waki_r_wj = get_bone_index(oh, "j_waki_r_wj");
    n_kata_r = get_bone_index(oh, "n_kata_r");
    j_kata_r_wj = get_bone_index(oh, "j_kata_r_wj");
    j_ude_r_wj = get_bone_index(oh, "j_ude_r_wj");
    j_te_r_wj = get_bone_index(oh, "j_te_r_wj");
    j_te_sizen2_r_wj = get_bone_index(oh, "j_te_sizen2_r_wj");
    j_te_close_r_wj = get_bone_index(oh, "j_te_close_r_wj");
    j_te_reset_r_wj = get_bone_index(oh, "j_te_reset_r_wj");
    j_te_one_r_wj = get_bone_index(oh, "j_te_one_r_wj");
    j_kosi_wj = get_bone_index(oh, "j_kosi_wj");
    n_momo_l = get_bone_index(oh, "n_momo_l");
    j_momo_l_wj = get_bone_index(oh, "j_momo_l_wj");
    j_sune_l_wj = get_bone_index(oh, "j_sune_l_wj");
    j_asi_l_wj = get_bone_index(oh, "j_asi_l_wj");
    n_momo_r = get_bone_index(oh, "n_momo_r");
    j_momo_r_wj = get_bone_index(oh, "j_momo_r_wj");
    j_sune_r_wj = get_bone_index(oh, "j_sune_r_wj");
    j_asi_r_wj = get_bone_index(oh, "j_asi_r_wj");
}*/

x_pv_game_song_effect_auth_3d::x_pv_game_song_effect_auth_3d() {
    id = -1;
}

x_pv_game_song_effect_auth_3d::x_pv_game_song_effect_auth_3d(int32_t id) {
    this->id = id;
}

void x_pv_game_song_effect_auth_3d::reset() {
    id = -1;
}

x_pv_game_song_effect_glitter::x_pv_game_song_effect_glitter() : scene_counter(), field_8(), field_9() {
    reset();
}

void x_pv_game_song_effect_glitter::reset() {
    name.str.clear();
    name.str.shrink_to_fit();
    name.hash = hash_murmurhash_empty;
    scene_counter = 0;
    field_8 = false;
    for (bool& i : field_9)
        i = false;
}

x_pv_game_song_effect::x_pv_game_song_effect() : enable(), chara_id(), time() {
    reset();
}

x_pv_game_song_effect::~x_pv_game_song_effect() {

}

void x_pv_game_song_effect::reset() {
    enable = false;
    chara_id = -1;
    time = -1;
    auth_3d.clear();
    auth_3d.shrink_to_fit();
    glitter.clear();
    glitter.shrink_to_fit();
}

x_pv_game_camera::x_pv_game_camera() : state() {
    reset();
}

x_pv_game_camera::~x_pv_game_camera() {

}

void x_pv_game_camera::ctrl(float_t curr_time) {
    switch (state) {
    case 10: {
        if (!auth_3d_data_check_category_loaded(category.hash))
            break;

        int32_t id = auth_3d_data_load_hash(file.hash, &data_list[DATA_X], 0, 0);
        if (auth_3d_data_check_id_not_empty(&id)) {
            auth_3d_data_read_file_modern(&id);
            auth_3d_data_set_enable(&id, false);
            auth_3d_data_set_repeat(&id, false);
            auth_3d_data_set_paused(&id, false);
            auth_3d_data_set_frame_rate(&id, frame_rate_control);
        }

        auth_3d_id = id;
        rctx_ptr->camera->reset();
        state = 11;
    } break;
    case 11: {
        if (auth_3d_data_check_id_not_empty(&auth_3d_id)
            && auth_3d_data_check_id_loaded(&auth_3d_id))
            state = 20;
    } break;
    case 20: {
        int32_t& id = auth_3d_id;
        auth_3d_data_set_enable(&id, 1u);
        auth_3d_data_set_repeat(&id, false);
        auth_3d_data_set_paused(&id, 0);
        auth_3d_data_set_camera_root_update(&id, true);
        auth_3d_data_set_req_frame(&id, curr_time * 60.0f);
    } break;
    }
}

void x_pv_game_camera::load(int32_t pv_id, int32_t stage_id, FrameRateControl* frame_rate_control) {
    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "CAMPV%03d", pv_id);
    category = buf;

    if ((pv_id % 100) >= 25 && (pv_id % 100) <= 30 && stage_id >= 25 && stage_id <= 30)
        sprintf_s(buf, sizeof(buf), "CAMPV%03d_100", pv_id);
    else
        sprintf_s(buf, sizeof(buf), "CAMPV%03d_BASE", pv_id);
    file = buf;

    this->frame_rate_control = frame_rate_control;
}

void x_pv_game_camera::load_data() {
    if (state || !category.str.size())
        return;

    auth_3d_data_load_category(&data_list[DATA_X], category.c_str(), category.hash);
    state = 10;
}

void x_pv_game_camera::reset() {
    state = 0;
    category.str.clear();
    category.str.shrink_to_fit();
    category.hash = hash_murmurhash_empty;
    file.str.clear();
    file.str.shrink_to_fit();
    file.hash = hash_murmurhash_empty;
    auth_3d_id = -1;
}

void x_pv_game_camera::stop() {
    if (auth_3d_data_check_id_not_empty(&auth_3d_id))
        auth_3d_data_set_enable(&auth_3d_id, false);
}

void x_pv_game_camera::unload() {
    if (state != 20)
        return;

    auth_3d_data_unload_id(auth_3d_id, rctx_ptr);
    auth_3d_data_unload_category(category.hash);
    state = 0;
}

x_pv_game_effect::x_pv_game_effect() : flags(), state(),
play_param(), change_fields(), frame_rate_control() {
    reset();
}

x_pv_game_effect::~x_pv_game_effect() {

}

void x_pv_game_effect::ctrl(object_database* obj_db, texture_database* tex_db) {
    switch (state) {
    case 10: {
        bool wait_load = false;

        for (string_hash& i : pv_auth_3d)
            if (!auth_3d_data_check_category_loaded(i.hash))
                wait_load |= true;

        for (string_hash& i : pv_obj_set)
            if (object_storage_get_obj_set_handler(i.hash)
                && object_storage_load_obj_set_check_not_read(i.hash, obj_db, tex_db))
                wait_load |= true;

        if (wait_load)
            break;

        data_struct* x_data = &data_list[DATA_X];

        pvpp* play_param = this->play_param;
        size_t song_effect_count = song_effect.size();

        for (size_t i = 0; i < song_effect_count; i++) {
            x_pv_game_song_effect& song_effect = this->song_effect[i];
            pvpp_effect& effect = play_param->effect[i];
            for (string_hash& j : effect.auth_3d) {
                int32_t id = auth_3d_data_load_hash(j.hash, x_data, obj_db, tex_db);
                if (!auth_3d_data_check_id_not_empty(&id)) {
                    continue;
                }

                auth_3d_data_read_file_modern(&id);
                auth_3d_data_set_enable(&id, false);
                auth_3d_data_set_repeat(&id, false);
                auth_3d_data_set_paused(&id, false);
                auth_3d_data_set_visibility(&id, false);
                auth_3d_data_set_frame_rate(&id, frame_rate_control);
                song_effect.auth_3d.push_back(id);

            }
        }
        state = 11;
    } break;
    case 11: {
        bool wait_load = false;

        for (x_pv_game_song_effect& i : song_effect)
            for (x_pv_game_song_effect_auth_3d& j : i.auth_3d)
                if (auth_3d_data_check_id_not_empty(&j.id)
                    && !auth_3d_data_check_id_loaded(&j.id))
                    wait_load |= true;

        for (string_hash& i : pv_glitter)
            if (!Glitter::glt_particle_manager->CheckNoFileReaders(i.hash))
                wait_load |= true;

        if (wait_load)
            break;

        pvpp* play_param = this->play_param;
        size_t chara_count = play_param->chara.size();

        size_t song_effect_count = song_effect.size();

        for (size_t i = 0; i < song_effect_count; i++) {
            x_pv_game_song_effect& song_effect = this->song_effect[i];
            pvpp_effect& effect = play_param->effect[i];
            for (pvpp_glitter& j : effect.glitter) {
                x_pv_game_song_effect_glitter v132;
                v132.name = j.name;
                v132.scene_counter = 0;
                v132.field_8 = !!(j.unk2 & 0x01);
                for (bool& i : v132.field_9)
                    i = true;

                for (int32_t k = 0; k < chara_count && k < ROB_CHARA_COUNT; k++) {
                    bool v121 = false;
                    if (k < play_param->chara.size()) {
                        pvpp_chara& chara = play_param->chara[k];
                        for (pvpp_glitter& l : chara.glitter)
                            if (j.name.hash == l.name.hash) {
                                v121 = true;
                                break;
                            }
                    }
                    v132.field_9[k] = v121;
                }
                song_effect.glitter.push_back(v132);
            }
        }

        data_struct* x_data = &data_list[DATA_X];

        for (string_hash& i : pv_glitter) {
            Glitter::EffectGroup* eff_group = Glitter::glt_particle_manager->GetEffectGroup(i.hash);
            if (eff_group && eff_group->CheckModel())
                eff_group->LoadModel(x_data);
        }

        state = 12;
        break;
    }
    case 12: {
        bool wait_load = false;

        for (string_hash& i : pv_glitter) {
            Glitter::EffectGroup* eff_group = Glitter::glt_particle_manager->GetEffectGroup(i.hash);
            if (eff_group && eff_group->CheckLoadModel())
                wait_load |= true;
        }

        if (wait_load)
            break;

        state = 20;
    }
    case 20: {
        for (x_pv_game_song_effect& i : song_effect)
            if (i.enable)
                for (x_pv_game_song_effect_auth_3d& j : i.auth_3d)
                    auth_3d_data_set_chara_id(&j.id, i.chara_id);
    } break;
    }
}

void x_pv_game_effect::load(int32_t pv_id, pvpp* play_param, FrameRateControl* frame_rate_control) {
    this->play_param = play_param;
    this->frame_rate_control = frame_rate_control;

    size_t effect_count = play_param->effect.size();
    if (!effect_count)
        return;

    song_effect.resize(effect_count);

    for (size_t i = 0; i < effect_count; i++) {
        pvpp_effect& eff = play_param->effect[i];
        if (!eff.auth_3d.size())
            continue;

        char buf[0x100];
        size_t len = sprintf_s(buf, sizeof(buf), "A3D_EFFPV%03d", pv_id);
        pv_auth_3d.push_back({ std::string(buf, len), hash_murmurhash(buf, len) });

        len = sprintf_s(buf, sizeof(buf), "effpv%03d", 800 + (pv_id % 100));
        pv_obj_set.push_back({ std::string(buf, len), hash_murmurhash(buf, len) });
        break;
    }
}

void x_pv_game_effect::load_data(int32_t pv_id) {
    if (state)
        return;

    data_struct* x_data = &data_list[DATA_X];

    for (string_hash& i : pv_obj_set)
        object_storage_load_set_hash(x_data, i.hash);

    for (string_hash& i : pv_auth_3d)
        auth_3d_data_load_category(x_data, i.c_str(), i.hash);

    size_t effect_count = play_param->effect.size();
    for (size_t i = 0; i < effect_count; i++) {
        pvpp_effect& eff = play_param->effect[i];
        if (!eff.glitter.size())
            continue;

        char buf[0x100];
        size_t len = sprintf_s(buf, sizeof(buf), "eff_pv%03d_main", pv_id);
        uint32_t hash = (uint32_t)Glitter::glt_particle_manager->LoadFile(Glitter::X,
            x_data, buf, 0, -1.0f, false, 0);
        if (hash != hash_murmurhash_empty)
            pv_glitter.push_back({ buf, hash });
        break;
    }

    state = 10;
}

void x_pv_game_effect::reset() {
    flags = 0;
    state = 0;
    play_param = 0;
    change_fields = 0;
    pv_obj_set.clear();
    pv_obj_set.shrink_to_fit();
    pv_auth_3d.clear();
    pv_auth_3d.shrink_to_fit();
    pv_glitter.clear();
    pv_glitter.shrink_to_fit();
    song_effect.clear();
    song_effect.shrink_to_fit();
    frame_rate_control = 0;
}

void x_pv_game_effect::set_chara_id(int32_t index, int32_t chara_id, bool a4) {
    if (index < 0 || index >= song_effect.size())
        return;

    x_pv_game_song_effect& song_effect = this->song_effect[index];

    for (x_pv_game_song_effect_auth_3d& i : song_effect.auth_3d)
        auth_3d_data_set_chara_id(&i.id, chara_id);

    song_effect.chara_id = chara_id;
}

void x_pv_game_effect::set_song_effect(int32_t index, int64_t time) {
    if (state && state != 20)
        return;

    if (index < 0 || index >= song_effect.size())
        return;

    x_pv_game_song_effect& song_effect = this->song_effect[index];

    if (song_effect.enable) {
        set_song_effect_time_inner(index, time, false);
        return;
    }

    pvpp* play_param = this->play_param;
    size_t chara_count = play_param->chara.size();

    int32_t chara_id = play_param->effect[index].chara_id;
    if (chara_id < 0 && chara_id >= ROB_CHARA_COUNT)
        chara_id = -1;
    song_effect.chara_id = chara_id;

    for (x_pv_game_song_effect_auth_3d& i : song_effect.auth_3d) {
        auth_3d_data_set_enable(&i.id, true);
        auth_3d_data_set_repeat(&i.id, false);
        auth_3d_data_set_req_frame(&i.id, 0.0f);
        auth_3d_data_set_visibility(&i.id, true);
    }

    for (x_pv_game_song_effect_glitter& i : song_effect.glitter) {
        i.scene_counter = Glitter::glt_particle_manager->LoadSceneEffect(i.name.hash, i.name.c_str());
        if (!i.scene_counter)
            continue;

        Glitter::glt_particle_manager->SetSceneFrameRate(i.scene_counter, 0);
    }

    song_effect.enable = true;
    song_effect.time = time;

    set_song_effect_time_inner(index, time, false);

    for (x_pv_game_song_effect_auth_3d& i : song_effect.auth_3d)
        auth_3d_data_set_chara_id(&i.id, chara_id);
}

void x_pv_game_effect::set_song_effect_time(int32_t index, int64_t time, bool glitter) {
    if (index >= 0 && index < song_effect.size() && song_effect[index].enable)
        set_song_effect_time_inner(index, time, glitter);
}

void x_pv_game_effect::set_song_effect_time_inner(int32_t index, int64_t time, bool glitter) {
    if (index < 0 || index >= song_effect.size())
        return;

    x_pv_game_song_effect& song_effect = this->song_effect[index];

    float_t req_frame = (float_t)((double_t)(time - song_effect.time) * 0.000000001) * 60.0f;
    float_t max_frame = -1.0f;

    if (change_fields) {
        int64_t* key = change_fields->data();
        size_t length = change_fields->size();
        size_t temp;
        while (length > 0)
            if (time < key[temp = length / 2])
                length = temp;
            else {
                key += temp + 1;
                length -= temp + 1;
            }

        if (key != change_fields->data() + change_fields->size())
            max_frame = (float_t)((double_t)(*key - song_effect.time) * 0.000000001) * 60.0f - 1.0f;
    }

    for (x_pv_game_song_effect_auth_3d& i : song_effect.auth_3d) {
        auth_3d_data_set_req_frame(&i.id, req_frame);
        auth_3d_data_set_paused(&i.id, false);
        auth_3d_data_set_max_frame(&i.id, max_frame);
    }

    if (glitter)
        for (x_pv_game_song_effect_glitter& i : song_effect.glitter) {
            if (!i.scene_counter)
                continue;

            float_t frame = Glitter::glt_particle_manager->GetSceneFrameLifeTime(i.scene_counter, 0);
            Glitter::glt_particle_manager->SetSceneEffectReqFrame(i.scene_counter, req_frame - frame);
        }
}

void x_pv_game_effect::stop() {
    size_t song_effect_count = song_effect.size();
    for (size_t i = 0; i < song_effect_count; i++)
        stop_song_effect((int32_t)i, false);

    for (string_hash& i : pv_glitter)
        Glitter::glt_particle_manager->FreeScene(i.hash);
}

void x_pv_game_effect::stop_song_effect(int32_t index, bool free_glitter) {
    if ((state && state != 20) || index < 0 || index >= song_effect.size() || !song_effect[index].enable)
        return;

    x_pv_game_song_effect& song_effect = this->song_effect[index];

    for (x_pv_game_song_effect_auth_3d& i : song_effect.auth_3d) {
        int32_t& id = i.id;
        auth_3d_data_set_enable(&id, false);
        auth_3d_data_set_req_frame(&id, 0.0f);
        auth_3d_data_set_visibility(&id, false);
    }

    for (x_pv_game_song_effect_glitter& i : song_effect.glitter)
        if (i.scene_counter) {
            Glitter::glt_particle_manager->FreeSceneEffect(i.scene_counter, !free_glitter);
            i.scene_counter = 0;
        }

    song_effect.chara_id = -1;
    song_effect.enable = false;
}

void x_pv_game_effect::unload() {
    if (state && state != 20)
        return;

    size_t song_effect_count = song_effect.size();
    for (size_t i = 0; i < song_effect_count; i++)
        stop_song_effect((int32_t)i, false);

    for (string_hash& i : pv_glitter)
        Glitter::glt_particle_manager->FreeScene(i.hash);

    for (x_pv_game_song_effect& i : song_effect)
        for (x_pv_game_song_effect_auth_3d& j : i.auth_3d) {
            auth_3d_data_unload_id(j.id, rctx_ptr);
            j.id = -1;
        }

    for (string_hash& i : pv_auth_3d)
        auth_3d_data_unload_category(i.hash);

    for (string_hash& i : pv_obj_set)
        object_storage_unload_set(i.hash);

    for (string_hash& i : pv_glitter) {
        Glitter::EffectGroup* eff_group = Glitter::glt_particle_manager->GetEffectGroup(i.hash);
        if (eff_group)
            eff_group->FreeModel();

        Glitter::glt_particle_manager->UnloadEffectGroup(i.hash);
    }

    song_effect.clear();
    song_effect.resize(song_effect_count);
    state = 0;
}

x_pv_game_chara_effect_auth_3d::x_pv_game_chara_effect_auth_3d() : field_0(), src_chara(), dst_chara() {
    id = -1;
    time = -1;
    reset();
}

x_pv_game_chara_effect_auth_3d::~x_pv_game_chara_effect_auth_3d() {

}

void x_pv_game_chara_effect_auth_3d::reset() {
    field_0 = false;
    src_chara = CHARA_MAX;
    dst_chara = CHARA_MAX;
    file.str.clear();
    file.str.shrink_to_fit();
    file.hash = hash_murmurhash_empty;
    category.str.clear();
    category.str.shrink_to_fit();
    category.hash = hash_murmurhash_empty;
    object_set.str.clear();
    object_set.str.shrink_to_fit();
    object_set.hash = hash_murmurhash_empty;
    id = -1;
    time = -1;
}

x_pv_game_chara_effect::x_pv_game_chara_effect() : state(), change_fields(), frame_rate_control() {
    reset();
}

x_pv_game_chara_effect::~x_pv_game_chara_effect() {

}

void x_pv_game_chara_effect::ctrl(object_database* obj_db, texture_database* tex_db) {
    switch (state) {
    case 20: {
        bool wait_load = false;

        for (std::vector<x_pv_game_chara_effect_auth_3d>& i : auth_3d)
            for (x_pv_game_chara_effect_auth_3d j : i) {
                if (!j.category.str.size())
                    continue;

                if (!auth_3d_data_check_category_loaded(j.category.hash))
                    wait_load |= true;

                if (object_storage_get_obj_set_handler(j.object_set.hash)
                    && object_storage_load_obj_set_check_not_read(j.object_set.hash, obj_db, tex_db))
                    wait_load |= true;
            }

        if (wait_load)
            break;

        data_struct* x_data = &data_list[DATA_X];

        for (std::vector<x_pv_game_chara_effect_auth_3d>& i : auth_3d)
            for (x_pv_game_chara_effect_auth_3d j : i) {
                if (!j.category.str.size())
                    continue;

                int32_t id = auth_3d_data_load_hash(j.file.hash, x_data, obj_db, tex_db);
                if (!auth_3d_data_check_id_not_empty(&id)) {
                    j.id = -1;
                    continue;
                }

                auth_3d_data_read_file_modern(&id);
                auth_3d_data_set_enable(&id, false);
                auth_3d_data_set_repeat(&id, false);
                auth_3d_data_set_paused(&id, true);
                auth_3d_data_set_visibility(&id, false);
                auth_3d_data_set_frame_rate(&id, frame_rate_control);

                if (j.field_0)
                    auth_3d_data_set_src_dst_chara(&id, j.src_chara, j.dst_chara);

                j.id = id;
            }

        state = 21;
    } break;
    case 21: {
        bool wait_load = false;

        for (std::vector<x_pv_game_chara_effect_auth_3d>& i : auth_3d)
            for (x_pv_game_chara_effect_auth_3d j : i) {
                if (!j.category.str.size())
                    continue;

                if (auth_3d_data_check_id_not_empty(&j.id)
                    && !auth_3d_data_check_id_loaded(&j.id))
                    wait_load |= true;
            }

        if (!wait_load)
            state = 30;
    } break;
    }
}

void x_pv_game_chara_effect::load(int32_t pv_id, pvpp* play_param,
    FrameRateControl* frame_rate_control, chara_index charas[6]) {
    if (state)
        return;

    this->play_param = play_param;
    this->frame_rate_control = frame_rate_control;

    size_t chara_count = play_param->chara.size();
    for (int32_t i = 0; i < chara_count && i < ROB_CHARA_COUNT; i++) {
        pvpp_chara& chara = play_param->chara[i];

        pvpp_chara_effect& chara_effect = chara.chara_effect;

        ::chara_index src_chara = (::chara_index)chara_effect.base_chara;
        ::chara_index dst_chara = charas[i];

        /*if (dst_chara == CHARA_EXTRA)
            dst_chara = src_chara;*/

        std::string src_chara_str = chara_index_get_auth_3d_name(src_chara);
        std::string dst_chara_str = chara_index_get_auth_3d_name(dst_chara);

        for (pvpp_chara_effect_auth_3d& j : chara_effect.auth_3d) {
            x_pv_game_chara_effect_auth_3d auth_3d;
            auth_3d.field_0 = j.u00;
            auth_3d.src_chara = src_chara;
            auth_3d.dst_chara = dst_chara;

            std::string file = j.auth_3d.str;

            std::string object_set;
            if (!j.object_set.str.size()) {
                size_t pos = file.find("_");
                if (pos != -1)
                    object_set = file.substr(0, pos);
                else
                    object_set = file;
            }
            else
                object_set = j.object_set.str;


            if (src_chara != dst_chara) {
                size_t pos = 0;
                    pos = object_set.find(src_chara_str);
                if (pos && pos != -1)
                    object_set.replace(pos, src_chara_str.size(), dst_chara_str);

                if (!auth_3d.field_0) {
                    size_t pos = file.find(src_chara_str);
                    if (pos != -1)
                        file = file.replace(pos, src_chara_str.size(), dst_chara_str);
                }
            }

            std::string category;
            size_t pos = file.find("_");
            if (pos != -1)
                category = file.substr(0, pos);
            else
                category = file;

            auth_3d.file = file;
            auth_3d.category = "A3D_" + category;
            auth_3d.object_set = object_set;

            this->auth_3d[i].push_back(auth_3d);
        }
    }
    state = 10;
}

void x_pv_game_chara_effect::load_data() {
    if (state != 10)
        return;

    data_struct* x_data = &data_list[DATA_X];

    for (std::vector<x_pv_game_chara_effect_auth_3d>& i : auth_3d)
        for (x_pv_game_chara_effect_auth_3d& j : i) {
            if (!j.category.str.size())
                continue;

            auth_3d_data_load_category(x_data, j.category.c_str(), j.category.hash);
            object_storage_load_set_hash(x_data, j.object_set.hash);
        }

    state = 10;
}

void x_pv_game_chara_effect::reset() {
    state = 0;
    play_param = 0;
    change_fields = 0;
    frame_rate_control = 0;

    for (std::vector<x_pv_game_chara_effect_auth_3d>& i : auth_3d) {
        i.clear();
        i.shrink_to_fit();
    }
}

void x_pv_game_chara_effect::set_chara_effect(int32_t chara_id, int32_t index, int64_t time) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return;

    std::vector<x_pv_game_chara_effect_auth_3d>& auth_3d = this->auth_3d[chara_id];
    if (index < 0 || index >= auth_3d.size())
        return;

    int32_t& id = auth_3d[index].id;

    if (!auth_3d_data_check_id_not_empty(&id))
        return;

    if (auth_3d_data_get_enable(&id)) {
        float_t req_frame = (float_t)((double_t)(time - auth_3d[index].time) * 0.000000001) * 60.0f;
        auth_3d_data_set_req_frame(&id, req_frame);
    }
    else {
        auth_3d_data_set_enable(&id, true);
        auth_3d_data_set_paused(&id, false);
        auth_3d_data_set_repeat(&id, false);
        auth_3d_data_set_req_frame(&id, 0.0f);
        auth_3d_data_set_visibility(&id, true);
        auth_3d[index].time = time;
    }

    float_t max_frame = -1.0f;
    if (change_fields) {
        int64_t* key = change_fields->data();
        size_t length = change_fields->size();
        size_t temp;
        while (length > 0)
            if (time < key[temp = length / 2])
                length = temp;
            else {
                key += temp + 1;
                length -= temp + 1;
            }

        if (key != change_fields->data() + change_fields->size())
            max_frame = (float_t)((double_t)(*key - auth_3d[index].time) * 0.000000001) * 60.0f - 1.0f;
    }

    auth_3d_data_set_max_frame(&id, max_frame);
}

void x_pv_game_chara_effect::set_chara_effect_time(int32_t chara_id, int32_t index, int64_t time) {
    if (!state || chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return;

    for (x_pv_game_chara_effect_auth_3d& i : auth_3d[chara_id]) {
        if (!auth_3d_data_check_id_not_empty(&i.id) || !auth_3d_data_get_enable(&i.id))
            continue;

        float_t req_frame = (float_t)((double_t)(time - i.time) * 0.000000001) * 60.0f;
        auth_3d_data_set_req_frame(&i.id, req_frame);
        auth_3d_data_set_paused(&i.id, 0);
    }
}

void x_pv_game_chara_effect::stop() {
    if (!state)
        return;

    for (std::vector<x_pv_game_chara_effect_auth_3d>& i : auth_3d)
        for (x_pv_game_chara_effect_auth_3d& j : i)
            if (auth_3d_data_check_id_not_empty(&j.id))
                auth_3d_data_set_enable(&j.id, false);
}

void x_pv_game_chara_effect::stop_chara_effect(int32_t chara_id, int32_t index) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return;

    for (x_pv_game_chara_effect_auth_3d& i : auth_3d[chara_id])
        if (auth_3d_data_check_id_not_empty(&i.id))
            auth_3d_data_set_enable(&i.id, false);
}

void x_pv_game_chara_effect::unload() {
    if (!state)
        return;

    if (state == 10 || state == 30) {
        for (std::vector<x_pv_game_chara_effect_auth_3d>& i : auth_3d)
            for (x_pv_game_chara_effect_auth_3d& j : i) {
                if (!j.category.str.size())
                    continue;

                auth_3d_data_unload_id(j.id, rctx_ptr);
                auth_3d_data_unload_category(j.category.hash);
                object_storage_unload_set(j.object_set.hash);
            }
        state = 10;
    }

    for (std::vector<x_pv_game_chara_effect_auth_3d>& i : auth_3d) {
        i.clear();
        i.shrink_to_fit();
    }

    frame_rate_control = 0;
    play_param = 0;
    state = 0;
}

x_pv_game_dsc_data::x_pv_game_dsc_data() : dsc_data_ptr(), dsc_data_ptr_end(), time() {
    reset();
}

x_pv_game_dsc_data::~x_pv_game_dsc_data() {

}

void x_pv_game_dsc_data::ctrl(float_t curr_time, float_t delta_time) {

}

dsc_data* x_pv_game_dsc_data::find(int32_t func_name, int32_t* time,
    int32_t* pv_branch_mode, dsc_data* start, dsc_data* end) {
    int32_t _time = -1;
    for (dsc_data* i = start; i != end; i++)
        if (i->func == func_name) {
            if (time)
                *time = _time;
            return i;
        }
        else if (i->func == DSC_X_END)
            break;
        else if (i->func == DSC_X_TIME) {
            uint32_t* data = dsc.get_func_data(i);
            _time = (int32_t)data[0];
        }
        else if (i->func == DSC_X_PV_END)
            break;
        else if (i->func == DSC_X_PV_BRANCH_MODE) {
            if (pv_branch_mode) {
                uint32_t* data = dsc.get_func_data(i);
                *pv_branch_mode = (int32_t)data[0];
            }
        }
    return 0;
}

void x_pv_game_dsc_data::find_bar_beat(x_pv_bar_beat& bar_beat) {
    x_pv_bar_beat_data* bar_beat_data = 0;
    int32_t beat_counter = 0;

    int32_t time = -1;
    for (dsc_data& i : dsc.data) {
        if (i.func == DSC_X_END)
            break;

        uint32_t* data = dsc.get_func_data(&i);
        switch (i.func) {
        case DSC_X_TIME: {
            time = (int32_t)data[0];
        } break;
        case DSC_X_BAR_POINT: {
            bar_beat.data.push_back({});
            bar_beat_data = &bar_beat.data.back();
            *bar_beat_data = {};
            bar_beat_data->bar = (int32_t)data[0];
            bar_beat_data->bar_time = (float_t)time * (float_t)(1.0 / 100000.0);
            bar_beat_data->beat_counter = beat_counter;
        } break;
        case DSC_X_BEAT_POINT: {
            if (!bar_beat_data)
                break;

            bar_beat_data->beat_counter = ++beat_counter;
            bar_beat_data->beat_time[bar_beat_data->beat_count++] = (float_t)time * (float_t)(1.0 / 100000.0);
        } break;
        }
    }
}

void x_pv_game_dsc_data::find_change_fields(std::vector<int64_t>& change_fields) {
    int32_t pv_branch_mode = 0;
    int32_t time = -1;
    int32_t prev_time = -1;

    ::dsc_data* i = dsc.data.data();
    ::dsc_data* i_end = dsc.data.data() + dsc.data.size();
    while (true) {
        i = find(DSC_X_CHANGE_FIELD, &time, &pv_branch_mode, i, i_end);
        if (!i)
            break;

        if (time < 0) {
            time = prev_time;
            if (prev_time < 0)
                break;
        }

        change_fields.push_back(time);
        prev_time = time;
        i++;
    }
}

int64_t x_pv_game_dsc_data::find_pv_end() {
    int32_t pv_branch_mode = 0;
    int32_t pv_end_time = -1;
    int32_t end_time = -1;

    ::dsc_data* i = dsc.data.data();
    ::dsc_data* i_end = dsc.data.data() + dsc.data.size();
    find(DSC_X_PV_END, &pv_end_time, &pv_branch_mode, i, i_end);

    pv_branch_mode = 0;

    i = dsc.data.data();
    i_end = dsc.data.data() + dsc.data.size();
    find(DSC_X_END, &end_time, &pv_branch_mode, i, i_end);
    return max(pv_end_time, end_time);
}

void x_pv_game_dsc_data::find_stage_effects(std::vector<std::pair<int64_t, int32_t>>& stage_effects) {
    int32_t pv_branch_mode = 0;
    int32_t time = -1;
    int32_t prev_time = -1;

    ::dsc_data* i = dsc.data.data();
    ::dsc_data* i_end = dsc.data.data() + dsc.data.size();
    while (true) {
        i = find(DSC_X_STAGE_EFFECT, &time, &pv_branch_mode, i, i_end);
        if (!i)
            break;

        if (time < 0) {
            time = prev_time;
            if (prev_time < 0)
                break;
        }

        uint32_t* data = dsc.get_func_data(i);
        int32_t stage_effect = (int32_t)data[0];

        if (stage_effect < 8 || stage_effect > 9)
            stage_effects.push_back({ (int64_t)time * 10000, stage_effect });
        prev_time = time;
        i++;
    }
}

void x_pv_game_dsc_data::reset() {
    dsc.type = DSC_NONE;
    dsc.signature = 0;
    dsc.id = 0;
    dsc.data.clear();
    dsc.data.shrink_to_fit();
    dsc.data_buffer.clear();
    dsc.data_buffer.shrink_to_fit();
    dsc_data_ptr = 0;
    dsc_data_ptr_end = 0;
    time = -1;
}

void x_pv_game_dsc_data::unload() {
    dsc.type = DSC_NONE;
    dsc.signature = 0;
    dsc.id = 0;
    dsc.data.clear();
    dsc.data.shrink_to_fit();
    dsc.data_buffer.clear();
    dsc.data_buffer.shrink_to_fit();
    dsc_data_ptr = 0;
    dsc_data_ptr_end = 0;
    time = -1;
}

x_pv_game_data::x_pv_game_data() : pv_id(), play_param(), curr_time(), delta_time(), pv_end_time(),
time_float(), frame(), field_1C(), state(), stage_effect_index(), next_stage_effect_bar(), stage() {

}

x_pv_game_data::~x_pv_game_data() {
    if (play_param) {
        delete play_param;
        play_param = 0;
    }
}

void x_pv_game_data::ctrl(float_t curr_time, float_t delta_time) {
    this->curr_time = curr_time;
    this->delta_time = delta_time;

    switch (state) {
    case 10: {
        if (field_1C & 0x08) {
            state = 20;
            field_1C &= ~0x08;
        }
        else
            state = 0;
    } break;
    case 20: {
        effect.load_data(pv_id);
        chara_effect.load_data();
        pv_expression_file_load(&data_list[DATA_X], "root+/pv_expression/", exp_file.hash);
        state = 21;
    } break;
    case 21: {
        if (stage->state != 20)
            break;

        camera.load_data();

        state = 22;
    } break;
    case 22: {
        bool wait_load = false;

        wait_load |= pv_expression_file_check_not_ready(exp_file.hash);
        wait_load |= camera.state && camera.state != 20;
        wait_load |= effect.state && effect.state != 20;
        wait_load |= chara_effect.state && chara_effect.state != 10 && chara_effect.state != 30;

        if (wait_load)
            break;

        state = 23;
    } break;
    case 23: {
        dsc_data.find_bar_beat(bar_beat);
        bar_beat.reset_time();
        stage->bpm_frame_rate_control.bar_beat = &bar_beat;
        state = 24;
    } break;
    case 24: {
        dsc_data.find_stage_effects(stage_effects);

        int64_t pv_end_time = dsc_data.find_pv_end();
        if (pv_end_time >= 0)
            this->pv_end_time = (float_t)((double_t)pv_end_time * 0.000000001);
        state = 25;
    } break;
    case 25: {
        dsc_data.find_change_fields(change_fields);
        effect.change_fields = &change_fields;
        chara_effect.change_fields = &change_fields;
        state = 30;
    } break;
    case 30: {
        ctrl_stage_effect_index();
        bar_beat.set_time(this->curr_time, this->delta_time);
        frame++;
    } break;
    case 40: {
        //if (!sub_81254B9E())
            state = 0;
    } break;
    }

    camera.ctrl(this->curr_time);
    effect.ctrl(&obj_db, &tex_db);
    chara_effect.ctrl(&obj_db, &tex_db);
    field_1C &= ~0x02;
}

void x_pv_game_data::ctrl_stage_effect_index() {
    if (stage_effect_index >= stage_effects.size())
        return;

    std::pair<int64_t, int32_t>* curr_stage_effect = &stage_effects[stage_effect_index];
    if (next_stage_effect_bar > bar_beat.bar)
        return;

    stage->set_stage_effect(curr_stage_effect->second);
    stage_effect_index++;
    if (stage_effect_index >= stage_effects.size())
        return;

    std::pair<int64_t, int32_t>* next_stage_effect = &stage_effects[stage_effect_index];

    float_t time = (float_t)((double_t)next_stage_effect->first * 0.000000001);
    int32_t bar = bar_beat.get_bar_beat_from_time(0, time);
    next_stage_effect_bar = --bar;

    int32_t v14 = bar / 2 - 1;
    if (stage->check_stage_effect_has_change_effect(curr_stage_effect->second, next_stage_effect->second))
        v14 = bar / 2 - 2;
    if (v14 <= 0)
        v14 = 0;
    next_stage_effect_bar = 2 * v14 + 1;
}

void x_pv_game_data::load(int32_t pv_id, FrameRateControl* frame_rate_control, chara_index charas[6]) {
    if (state)
        return;

    data_struct* x_data = &data_list[DATA_X];

    effect.load(pv_id, play_param, frame_rate_control);
    chara_effect.load(pv_id, play_param, frame_rate_control, charas);

    char buf[0x100];
    sprintf_s(buf, sizeof(buf), "exp_PV%03d", pv_id);
    exp_file = { buf,  hash_utf8_murmurhash(buf) };

    camera.load(pv_id, stage->stage_id, frame_rate_control);

    state = 10;
}

void x_pv_game_data::reset() {
    pv_id = 0;
    if (play_param) {
        delete play_param;
        play_param = 0;
    }
    curr_time = 0.0f;
    delta_time = 0.0f;
    pv_end_time = 0.0f;
    time_float = 0.0f;
    frame = 0;
    field_1C = 0;
    state = 0;
    change_fields.clear();
    change_fields.shrink_to_fit();
    bar_beat.reset();
    stage_effect_index = 0;
    next_stage_effect_bar = 0;
    stage_effects.clear();
    stage_effects.shrink_to_fit();
    effect.reset();
    dsc_data.reset();
    exp_file.str.clear();
    exp_file.str.shrink_to_fit();
    exp_file.hash = hash_murmurhash_empty;
    //dof = {};
    stage = 0;
    obj_db.object_set.clear();
    obj_db.object_set.shrink_to_fit();
    tex_db.texture.clear();
    tex_db.texture.shrink_to_fit();
}

void x_pv_game_data::stop() {
    camera.stop();
    stage->reset_stage_effect();
    effect.stop();
    chara_effect.stop();
    dsc_data.reset();

    field_1C &= ~0xC0;
    if (state == 30)
        state = 0;
}

void x_pv_game_data::unload() {
    stop();
    camera.unload();
    effect.unload();
    chara_effect.unload();
    //dof = {};
    dsc_data.unload();
    pv_expression_file_unload(exp_file.hash);
    obj_db.object_set.clear();
    obj_db.object_set.shrink_to_fit();
    tex_db.texture.clear();
    tex_db.texture.shrink_to_fit();
    state = 40;
}

void x_pv_game_data::unload_if_state_is_0() {
    if (!state)
        unload();
}

x_pv_game_stage_effect_auth_3d::x_pv_game_stage_effect_auth_3d() : repeat(), field_1(), id() {
    reset();
}

void x_pv_game_stage_effect_auth_3d::reset() {
    repeat = false;
    field_1 = true;
    id = -1;
}

x_pv_game_stage_effect_glitter::x_pv_game_stage_effect_glitter() :
    scene_counter(), fade_time(), fade_time_left(), force_disp() {
    reset();
}

void x_pv_game_stage_effect_glitter::reset() {
    name.str.clear();
    name.str.shrink_to_fit();
    name.hash = hash_murmurhash_empty;
    scene_counter = 0;
    fade_time = 0.0f;
    fade_time_left = 0.0f;
    force_disp = false;
}

x_pv_game_stage_change_effect::x_pv_game_stage_change_effect() : enable(), bars_left(), bar_count() {

}

x_pv_game_stage_change_effect::~x_pv_game_stage_change_effect() {

}

void x_pv_game_stage_change_effect::reset() {
    enable = false;
    auth_3d.clear();
    auth_3d.shrink_to_fit();
    glitter.clear();
    glitter.shrink_to_fit();
    bars_left = 0;
    bar_count = 0;
}

x_pv_game_stage_effect::x_pv_game_stage_effect() : stage_effect(), set() {
    main_auth_3d_index = -1;
}

x_pv_game_stage_effect::~x_pv_game_stage_effect() {

}

void x_pv_game_stage_effect::reset() {
    stage_effect = 0;
    auth_3d.clear();
    auth_3d.shrink_to_fit();
    glitter.clear();
    glitter.shrink_to_fit();
    main_auth_3d_index = -1;
    set = false;
}


x_pv_game_stage_data::x_pv_game_stage_data() : flags(), state(), frame_rate_control() {
    reset();
}

x_pv_game_stage_data::~x_pv_game_stage_data() {

}

bool x_pv_game_stage_data::check_not_loaded() {
    return file_handler.check_not_ready() || (state && state != 2);
}

void x_pv_game_stage_data::ctrl(object_database* obj_db, texture_database* tex_db) {
    switch (state) {
    case 1: {
        if (task_stage_modern_check_not_loaded())
            break;

        bool wait_load = false;

        for (uint32_t& i : objhrc_hash)
            if (object_storage_get_obj_set_handler(i)
                && object_storage_load_obj_set_check_not_read(i, obj_db, tex_db))
                wait_load |= true;

        if (wait_load)
            break;

        state = 2;
    } break;
    }
}

void x_pv_game_stage_data::load(int32_t stage_id, FrameRateControl* frame_rate_control) {
    if (flags & 0x02)
        return;

    this->frame_rate_control = frame_rate_control;

    char buf[0x100];
    sprintf_s(buf, sizeof(buf), "pv_stgpv%03d.stg", stage_id);
    file_handler.read_file(&data_list[DATA_X], "root+/stage/", buf);

    task_stage_modern_load("X_PV_STAGE");

    size_t len = sprintf_s(buf, sizeof(buf), "STGPV%03d.stg", stage_id);
    obj_hash.push_back(hash_murmurhash(buf, len));

    flags |= 0x02;
}

void x_pv_game_stage_data::load_objects(object_database* obj_db, texture_database* tex_db) {
    if (~flags & 0x02 || file_handler.check_not_ready() || state && state != 2)
        return;

    const void* stg_data = file_handler.get_data();
    size_t stg_size = file_handler.get_size();

    stage_database_file stage_data_file;
    stage_data_file.read(stg_data, stg_size, true);
    stg_db.add(&stage_data_file);

    std::vector<stage_data_modern*> stage_data;

    obj_hash.clear();
    objhrc_hash.clear();
    for (stage_data_modern& i : stg_db.stage_modern) {
        stage_data.push_back(&i);

        std::string objhrc = i.name + "HRC";
        obj_hash.push_back(i.hash);
        objhrc_hash.push_back(hash_string_murmurhash(objhrc));
    }

    data_struct* x_data = &data_list[DATA_X];

    task_stage_modern_set_data(x_data, obj_db, tex_db, &stg_db);
    task_stage_modern_set_stage_hashes(&obj_hash, &stage_data);

    for (uint32_t& i : objhrc_hash)
        object_storage_load_set_hash(x_data, i);

    state = 1;
    flags |= 0x01;
}

void x_pv_game_stage_data::reset() {
    flags = 0x04;
    state = 0;
    file_handler.free_data();
    stg_db.stage_data.clear();
    stg_db.stage_data.shrink_to_fit();
    stg_db.stage_modern.clear();
    stg_db.stage_modern.shrink_to_fit();
    frame_rate_control = 0;
    obj_hash.clear();
    obj_hash.shrink_to_fit();
    stage_info.clear();
    stage_info.shrink_to_fit();
    objhrc_hash.clear();
    objhrc_hash.shrink_to_fit();
}

void x_pv_game_stage_data::set_default_stage() {
    if (~flags & 0x02 || state && state != 2)
        return;

    set_stage(hash_utf8_murmurhash("STGAETBACK"));
}

void x_pv_game_stage_data::set_stage(uint32_t hash) {
    if (~flags & 0x02 || state && state != 2)
        return;

    if (flags & 0x01) {
        task_stage_modern_get_loaded_stage_infos(&stage_info);
        flags &= ~0x01;
    }

    task_stage_modern_info stage_info;
    for (task_stage_modern_info& i : this->stage_info)
        if (task_stage_modern_has_stage_info(&i) && task_stage_modern_get_stage_hash(&i) == hash) {
            stage_info = i;
            break;
        }

    task_stage_modern_set_stage(&stage_info);
    if (task_stage_modern_has_stage_info(&stage_info))
        task_stage_modern_set_stage_display(&stage_info, !!(flags & 0x04));
    else
        task_stage_modern_current_set_stage_display(false);
}

void x_pv_game_stage_data::unload() {
    for (uint32_t& i : objhrc_hash)
        object_storage_unload_set(i);

    task_stage_modern_unload();

    flags &= ~0x02;
    state = 0;
    file_handler.free_data();
    stg_db.stage_data.clear();
    stg_db.stage_data.shrink_to_fit();
    stg_db.stage_modern.clear();
    stg_db.stage_modern.shrink_to_fit();
    frame_rate_control = 0;
    obj_hash.clear();
    obj_hash.shrink_to_fit();
    stage_info.clear();
    stage_info.shrink_to_fit();
    objhrc_hash.clear();
    objhrc_hash.shrink_to_fit();
}

x_pv_game_stage::x_pv_game_stage() : flags(), stage_id(), field_8(), state(), stage_resource(),
curr_stage_effect(), next_stage_effect(), stage_effect_transition_state() {
    reset();
}

x_pv_game_stage::~x_pv_game_stage() {
    if (stage_resource) {
        delete stage_resource;
        stage_resource = 0;
    }
}

bool x_pv_game_stage::check_stage_effect_has_change_effect(int32_t curr_stage_effect, int32_t next_stage_effect) {
    if (curr_stage_effect == 7)
        return false;

    curr_stage_effect--;
    next_stage_effect--;

    if (curr_stage_effect < 0 || next_stage_effect < 0)
        return false;

    size_t stage_effect_count = stage_resource->stage_effect.size();
    if (curr_stage_effect >= stage_effect_count || next_stage_effect >= stage_effect_count)
        return false;

    pvsr_stage_change_effect& stg_chg_eff = stage_resource->
        stage_change_effect[curr_stage_effect][next_stage_effect];
    if (stg_chg_eff.enable)
        return true;
    return false;
}

void x_pv_game_stage::ctrl(float_t delta_time) {
    switch (state) {
    case 10: {
        if (stage_data.check_not_loaded() || stage_resource_file_handler.check_not_ready())
            break;

        if (stage_data.flags & 0x02)
            stage_data.load_objects(&obj_db, &tex_db);

        const void* pvsr_data = stage_resource_file_handler.get_data();
        size_t pvsr_size = stage_resource_file_handler.get_size();

        pvsr* sr = new pvsr;
        sr->read(pvsr_data, pvsr_size);
        stage_resource = sr;

        if (sr->stage_effect.size() > 12)
            break;

        bool has_auth_3d = false;
        for (pvsr_stage_effect& i : sr->stage_effect)
            if (i.auth_3d.size()) {
                has_auth_3d = true;
                break;
            }

        if (has_auth_3d) {
            char buf[0x100];
            size_t len = sprintf_s(buf, sizeof(buf), "EFFSTGPV%03d", stage_id);
            stage_auth_3d.push_back(std::string(buf, len));
        }

        data_struct* x_data = &data_list[DATA_X];

        for (pvsr_effect& i : sr->effect) {
            uint32_t hash = (uint32_t)Glitter::glt_particle_manager->LoadFile(Glitter::X,
                x_data, i.name.c_str(), 0, i.emission, false, 0);
            if (hash != hash_murmurhash_empty)
                stage_glitter.push_back(hash);
        }

        for (string_hash& i : stage_auth_3d)
            auth_3d_data_load_category(x_data, i.c_str(), i.hash);

        state = 11;
    } break;
    case 11: {
        if (stage_data.check_not_loaded())
            break;

        bool wait_load = false;

        for (string_hash& i : stage_auth_3d)
            if (!auth_3d_data_check_category_loaded(i.hash))
                wait_load |= true;

        for (uint32_t& i : stage_glitter)
            if (!Glitter::glt_particle_manager->CheckNoFileReaders(i))
                wait_load |= true;

        if (wait_load)
            break;

        data_struct* x_data = &data_list[DATA_X];

        for (uint32_t& i : stage_glitter) {
            Glitter::EffectGroup* eff_group = Glitter::glt_particle_manager->GetEffectGroup(i);
            if (eff_group && eff_group->CheckModel())
                eff_group->LoadModel(x_data);
        }

        size_t stage_effect_count = stage_resource->stage_effect.size();

        for (int32_t i = 0; i < X_PV_GAME_STAGE_EFFECT_COUNT && i < stage_effect_count; i++) {
            pvsr_stage_effect& stg_eff = stage_resource->stage_effect[i];
            x_pv_game_stage_effect& eff = effect[i];

            eff.stage_effect = &stg_eff;

            size_t auth_3d_count = stg_eff.auth_3d.size();
            size_t glitter_count = stg_eff.glitter.size();

            eff.auth_3d.resize(auth_3d_count);
            eff.glitter.resize(glitter_count);

            for (size_t j = 0; j < auth_3d_count; j++) {
                pvsr_auth_3d& stg_eff_auth_3d = stg_eff.auth_3d[j];
                x_pv_game_stage_effect_auth_3d& eff_auth_3d = eff.auth_3d[j];

                eff_auth_3d.reset();

                int32_t stage_effect = i + 1;

                int32_t id = auth_3d_data_load_hash(stg_eff_auth_3d.name.hash, x_data, &obj_db, &tex_db);
                if (!auth_3d_data_check_id_not_empty(&id)) {
                    eff_auth_3d.id = -1;
                    continue;
                }

                auth_3d_data_read_file_modern(&id);
                auth_3d_data_set_enable(&id, false);
                auth_3d_data_set_repeat(&id, true);
                auth_3d_data_set_paused(&id, false);
                auth_3d_data_set_visibility(&id, false);
                auth_3d_data_set_frame_rate(&id, &bpm_frame_rate_control);

                eff_auth_3d.id = id;
                if (stage_effect >= 8 && stage_effect <= 9)
                    eff_auth_3d.repeat = false;
                else if (stage_effect == 7)
                    eff_auth_3d.repeat = !!(stg_eff_auth_3d.flags & PVSR_AUTH_3D_REPEAT);
                else
                    eff_auth_3d.repeat = true;

                if (stg_eff_auth_3d.flags & PVSR_AUTH_3D_MAIN)
                    eff.main_auth_3d_index = (int32_t)j;
            }

            for (size_t i = 0; i < glitter_count; i++) {
                pvsr_glitter& stg_eff_glt = stg_eff.glitter[i];
                x_pv_game_stage_effect_glitter& eff_glt = eff.glitter[i];

                eff_glt.reset();
                eff_glt.name = stg_eff_glt.name;
                eff_glt.fade_time = (float_t)stg_eff_glt.fade_time;
                eff_glt.force_disp = !!(stg_eff_glt.flags & PVSR_GLITTER_FORCE_DISP);
            }
        }

        for (int32_t i = 1; i <= X_PV_GAME_STAGE_EFFECT_COUNT; i++)
            for (int32_t j = 1; j <= X_PV_GAME_STAGE_EFFECT_COUNT; j++)
                load_change_effect(i, j);

        stage_data.set_default_stage();
        state = 12;
    }
    case 12: {
        bool wait_load = false;
        for (int32_t i = 0; i < X_PV_GAME_STAGE_EFFECT_COUNT; i++) {
            x_pv_game_stage_effect& eff = effect[i];
            for (x_pv_game_stage_effect_auth_3d& j : eff.auth_3d)
                if (auth_3d_data_check_id_not_empty(&j.id)
                    && !auth_3d_data_check_id_loaded(&j.id))
                    wait_load |= true;
        }

        for (int32_t i = 0; i < X_PV_GAME_STAGE_EFFECT_COUNT; i++)
            for (int32_t j = 0; j < X_PV_GAME_STAGE_EFFECT_COUNT; j++) {
                x_pv_game_stage_change_effect& chg_eff = change_effect[i][j];
                for (x_pv_game_stage_effect_auth_3d& k : chg_eff.auth_3d)
                    if (auth_3d_data_check_id_not_empty(&k.id)
                        && !auth_3d_data_check_id_loaded(&k.id))
                        wait_load |= true;
            }

        for (uint32_t& i : stage_glitter) {
            Glitter::EffectGroup* eff_group = Glitter::glt_particle_manager->GetEffectGroup(i);
            if (eff_group && eff_group->CheckLoadModel())
                wait_load |= true;
        }

        if (wait_load)
            break;

        state = 20;
    }
    case 20:
        ctrl_inner();
        break;
    case 30:
        state = 0;
        break;
    }

    stage_data.ctrl(&obj_db, &tex_db);
}

void x_pv_game_stage::ctrl_inner() {
    if (next_stage_effect && curr_stage_effect) {
        x_pv_bar_beat* bar_beat = bpm_frame_rate_control.bar_beat;
        if (!bar_beat)
            return;

        bool v6 = bar_beat && fabsf(bar_beat->delta_time) > 0.000001f
            && bar_beat->counter > 0 && !((bar_beat->bar - 1) % 2);

        int32_t curr_stg_eff = curr_stage_effect;
        if (curr_stg_eff >= 1 && curr_stg_eff <= 12)
            curr_stg_eff--;
        else
            curr_stg_eff = 0;

        int32_t next_stg_eff = next_stage_effect;
        if (next_stg_eff >= 1 && next_stg_eff <= 12)
            next_stg_eff--;
        else
            next_stg_eff = 0;

        x_pv_game_stage_effect& eff = effect[curr_stg_eff];
        x_pv_game_stage_change_effect& chg_eff = change_effect[curr_stg_eff][next_stg_eff];

        switch (stage_effect_transition_state) {
        case 0: {
            if (v6) {
                if (set_change_effect_frame(bar_beat->get_bar_current_frame())) {
                    chg_eff.bars_left = chg_eff.bar_count;
                    stage_effect_transition_state = 1;
                    stop_stage_effect_auth_3d(curr_stage_effect);
                }
                else {
                    stop_stage_effect_auth_3d(curr_stage_effect);
                    stop_stage_effect_glitter(curr_stage_effect);

                    set_stage_effect_auth_3d_frame(next_stage_effect, -1.0f);
                    set_stage_effect_glitter_frame(next_stage_effect, -1.0f);
                    curr_stage_effect = next_stage_effect;
                    next_stage_effect = 0;
                    stage_effect_transition_state = 0;
                }
            }
            else if (bar_beat->counter > 0) {
                std::vector<x_pv_game_stage_effect_auth_3d>& auth_3d = eff.auth_3d;
                for (x_pv_game_stage_effect_auth_3d& i : auth_3d) {
                    int32_t& id = i.id;
                    auth_3d_data_set_repeat(&id, false);
                }
            }
        } break;
        case 1: {
            if (v6) {
                chg_eff.bars_left -= 2;
                v6 = chg_eff.bars_left <= 0;
            }

            if (v6) {
                stop_stage_change_effect();
                stop_stage_effect_glitter(curr_stage_effect);

                set_stage_effect_auth_3d_frame(next_stage_effect, -1.0f);
                set_stage_effect_glitter_frame(next_stage_effect, -1.0f);
                curr_stage_effect = next_stage_effect;
                next_stage_effect = 0;
                stage_effect_transition_state = 0;
            }
            else {
                float_t delta_time_bar_beat = bar_beat->delta_time / (bar_beat->next_bar_time - bar_beat->curr_bar_time);

                std::vector<x_pv_game_stage_effect_glitter>& glitter = eff.glitter;
                for (x_pv_game_stage_effect_glitter& i : glitter) {
                    if (fabsf(i.fade_time) <= 0.000001f) {
                        Glitter::glt_particle_manager->SetSceneEffectExtColor(i.scene_counter, 0,
                            hash_murmurhash_empty, -1.0f, -1.0f, -1.0f, 0.0f);
                    }
                    else if (i.fade_time > 0.001f) {
                        i.fade_time_left -= delta_time_bar_beat * 4.0f;
                        if (i.fade_time_left < 0.0f)
                            i.fade_time_left = 0.0f;
                        Glitter::glt_particle_manager->SetSceneEffectExtColor(i.scene_counter, 0,
                            hash_murmurhash_empty, -1.0f, -1.0f, -1.0f, i.fade_time_left / i.fade_time);
                    }
                }
            }
        } break;
        }
    }
    else if (next_stage_effect)
        return;

    for (int32_t i = 8; i <= 9; i++) {
        if (~flags & (1 << i))
            continue;

        int32_t stage_effect;
        if (i > 0)
            stage_effect = i - 1;
        else
            stage_effect = 0;

        x_pv_game_stage_effect& eff = effect[stage_effect];
        std::vector<x_pv_game_stage_effect_auth_3d>& auth_3d = eff.auth_3d;
        std::vector<x_pv_game_stage_effect_glitter>& glitter = eff.glitter;

        bool stop = false;
        if (auth_3d.size())
            stop = auth_3d_data_get_ended(&auth_3d[eff.main_auth_3d_index].id);

        if (!stop) {
            for (x_pv_game_stage_effect_glitter& j : glitter)
                if (Glitter::glt_particle_manager->CheckSceneEnded(j.scene_counter)) {
                    stop = true;
                    break;
                }
        }

        if (stop) {
            stop_stage_effect_auth_3d(i);
            stop_stage_effect_glitter(i);
            flags &= ~(1 << i);
        }
    }
}

void x_pv_game_stage::load(int32_t stage_id, FrameRateControl* frame_rate_control, bool a4) {
    if (!stage_id)
        return;

    flags &= ~0x02;
    if (a4)
        flags |= 0x02;

    field_8 = 1;
    this->stage_id = stage_id;

    stage_data.load(stage_id, frame_rate_control);

    char buf[0x100];
    sprintf_s(buf, sizeof(buf), "stgpv%03d_param.pvsr", stage_id);
    stage_resource_file_handler.read_file(&data_list[DATA_X], "root+/pv_stage_rsrc/", buf);
    state = 10;
}

void x_pv_game_stage::load_change_effect(int32_t curr_stage_effect, int32_t next_stage_effect) {
    curr_stage_effect--;
    next_stage_effect--;

    if (curr_stage_effect < 0 || next_stage_effect < 0)
        return;

    size_t stage_effect_count = stage_resource->stage_effect.size();
    if (curr_stage_effect >= stage_effect_count || next_stage_effect >= stage_effect_count)
        return;

    pvsr_stage_change_effect& stg_chg_eff = stage_resource->
        stage_change_effect[curr_stage_effect][next_stage_effect];
    x_pv_game_stage_change_effect& chg_eff = change_effect[curr_stage_effect][next_stage_effect];

    if (!stg_chg_eff.enable) {
        chg_eff.enable = false;
        return;
    }

    chg_eff.enable = true;
    chg_eff.bars_left = 0;
    chg_eff.bar_count = stg_chg_eff.bar_count > -1 ? stg_chg_eff.bar_count : 2;

    size_t auth_3d_count = stg_chg_eff.auth_3d.size();
    size_t glitter_count = stg_chg_eff.glitter.size();

    chg_eff.auth_3d.resize(auth_3d_count);
    chg_eff.glitter.resize(glitter_count);

    data_struct* x_data = &data_list[DATA_X];

    for (size_t i = 0; i < auth_3d_count; i++) {
        pvsr_auth_3d& stg_chg_eff_auth_3d = stg_chg_eff.auth_3d[i];
        x_pv_game_stage_effect_auth_3d& chg_eff_auth_3d = chg_eff.auth_3d[i];

        chg_eff_auth_3d.reset();

        int32_t id = auth_3d_data_load_hash(stg_chg_eff_auth_3d.name.hash, x_data, &obj_db, &tex_db);
        if (!auth_3d_data_check_id_not_empty(&id)) {
            chg_eff_auth_3d.id = -1;
            continue;
        }

        auth_3d_data_read_file_modern(&id);
        auth_3d_data_set_enable(&id, false);
        auth_3d_data_set_repeat(&id, false);
        auth_3d_data_set_paused(&id, false);
        auth_3d_data_set_visibility(&id, false);
        auth_3d_data_set_frame_rate(&id, &bpm_frame_rate_control);

        chg_eff_auth_3d.id = id;
        chg_eff_auth_3d.field_1 = true;
        chg_eff_auth_3d.repeat = !!(stg_chg_eff_auth_3d.flags & PVSR_AUTH_3D_REPEAT);
    }
    
    for (size_t i = 0; i < glitter_count; i++) {
        pvsr_glitter& stg_chg_eff_glt = stg_chg_eff.glitter[i];
        x_pv_game_stage_effect_glitter& chg_eff_glt = chg_eff.glitter[i];

        chg_eff_glt.reset();
        chg_eff_glt.name = stg_chg_eff_glt.name;
        chg_eff_glt.force_disp = !!(stg_chg_eff_glt.flags & PVSR_GLITTER_FORCE_DISP);
    }
}

void x_pv_game_stage::reset() {
    flags = 0x01;
    stage_id = 0;
    field_8 = 0;
    state = 0;
    if (stage_resource) {
        delete stage_resource;
        stage_resource = 0;
    }
    bpm_frame_rate_control.Reset();
    stage_auth_3d.clear();
    stage_auth_3d.shrink_to_fit();
    stage_glitter.clear();
    stage_glitter.shrink_to_fit();
    curr_stage_effect = 0;
    next_stage_effect = 0;
    stage_effect_transition_state = 0;

    for (int32_t i = 0; i < X_PV_GAME_STAGE_EFFECT_COUNT; i++)
        effect[i].reset();

    for (int32_t i = 0; i < X_PV_GAME_STAGE_EFFECT_COUNT; i++)
        for (int32_t j = 0; j < X_PV_GAME_STAGE_EFFECT_COUNT; j++)
            change_effect[i][j].reset();

    obj_db.object_set.clear();
    obj_db.object_set.shrink_to_fit();
    tex_db.texture.clear();
    tex_db.texture.shrink_to_fit();
}

void x_pv_game_stage::reset_stage_effect() {
    stop_stage_effect(true);

    for (int32_t i = 8; i <= 9; i++) {
        if (~flags & (1 << i))
            continue;

        stop_stage_effect_auth_3d(i);
        stop_stage_effect_glitter(i);
    }

    for (uint32_t& i : stage_glitter)
        Glitter::glt_particle_manager->FreeScene(i);
}

bool x_pv_game_stage::set_change_effect_frame(float_t frame) {
    if (!curr_stage_effect || !next_stage_effect)
        return false;

    int32_t curr_stg_eff = curr_stage_effect;
    if (curr_stg_eff >= 1 && curr_stg_eff <= 12)
        curr_stg_eff--;
    else
        curr_stg_eff = 0;

    int32_t next_sta_eff = next_stage_effect;
    if (next_sta_eff >= 1 && next_sta_eff <= 12)
        next_sta_eff--;
    else
        next_sta_eff = 0;

    x_pv_game_stage_change_effect& chg_eff = change_effect[curr_stg_eff][next_sta_eff];
    if (!chg_eff.enable)
        return false;

    for (x_pv_game_stage_effect_auth_3d& i : chg_eff.auth_3d) {
        int32_t& id = i.id;
        auth_3d_data_set_camera_root_update(&id, false);
        auth_3d_data_set_enable(&id, true);
        auth_3d_data_set_repeat(&id, i.repeat);
        auth_3d_data_set_req_frame(&id, frame);
        auth_3d_data_set_max_frame(&id, auth_3d_data_get_last_frame(&id) - 1.0f);
        auth_3d_data_set_paused(&id, false);
        auth_3d_data_set_visibility(&id, !!(flags & 0x01));
        auth_3d_data_set_frame_rate(&id, &bpm_frame_rate_control);
    }

    for (x_pv_game_stage_effect_glitter& i : chg_eff.glitter) {
        if (i.name.hash == hash_murmurhash_empty)
            continue;

        i.scene_counter = Glitter::glt_particle_manager->LoadSceneEffect(i.name.hash, i.name.c_str());
        if (!i.scene_counter)
            continue;

        Glitter::glt_particle_manager->SetSceneFrameRate(i.scene_counter, &bpm_frame_rate_control);
        Glitter::glt_particle_manager->SetSceneEffectReqFrame(i.scene_counter, frame);
    }
    return true;
}

void x_pv_game_stage::set_stage_effect(int32_t stage_effect) {
    if (stage_effect < 1 || stage_effect > 12)
        return;

    if (stage_effect >= 8 && stage_effect <= 9) {
        if (flags & (1 << stage_effect)) {
            stop_stage_effect_auth_3d(stage_effect);
            stop_stage_effect_glitter(stage_effect);
        }

        set_stage_effect_auth_3d_frame(stage_effect, 0.0f);
        set_stage_effect_glitter_frame(stage_effect, 0.0f);
        flags |= 1 << stage_effect;
        return;
    }

    if (curr_stage_effect == stage_effect && !next_stage_effect || next_stage_effect)
        return;
    else if (curr_stage_effect) {
        if (~flags & 0x10) {
            next_stage_effect = stage_effect;
            stage_effect_transition_state = 0;
            return;
        }
        else if (curr_stage_effect >= 1 && curr_stage_effect <= 12) {
            stop_stage_effect_auth_3d(curr_stage_effect);
            stop_stage_effect_glitter(curr_stage_effect);
        }
    }

    if (stage_data.obj_hash.size())
        stage_data.set_stage(stage_data.obj_hash.front());
    curr_stage_effect = stage_effect;
    set_stage_effect_auth_3d_frame(stage_effect, -1.0f);
    set_stage_effect_glitter_frame(stage_effect, -1.0f);
}

void x_pv_game_stage::set_stage_effect_auth_3d_frame(int32_t stage_effect, float_t frame) {
    if (stage_effect < 1 || stage_effect > 12)
        return;

    if (frame < 0.0f)
        frame = bpm_frame_rate_control.bar_beat->get_bar_current_frame();

    std::vector<x_pv_game_stage_effect_auth_3d>& auth_3d = effect[stage_effect - 1ULL].auth_3d;
    for (x_pv_game_stage_effect_auth_3d& i : auth_3d) {
        int32_t& id = i.id;
        auth_3d_data_set_camera_root_update(&id, false);
        auth_3d_data_set_enable(&id, true);
        auth_3d_data_set_repeat(&id, true);
        if (i.repeat && auth_3d_data_get_last_frame(&id) <= frame) {
            int32_t frame_offset = (int32_t)auth_3d_data_get_frame_offset(&id);
            int32_t last_frame = (int32_t)auth_3d_data_get_last_frame(&id);
            int32_t _frame = frame_offset + (int32_t)(frame - (float_t)frame_offset) % last_frame;

            frame = (float_t)_frame + (frame - (float_t)(int32_t)frame);
        }
        auth_3d_data_set_req_frame(&id, frame);
        auth_3d_data_set_max_frame(&id, -1.0f);
        auth_3d_data_set_paused(&id, false);
        auth_3d_data_set_visibility(&id, true);
        auth_3d_data_set_frame_rate(&id, &bpm_frame_rate_control);
    }
}

void x_pv_game_stage::set_stage_effect_glitter_frame(int32_t stage_effect, float_t frame) {
    if (stage_effect < 1 || stage_effect > 12)
        return;

    if (frame < 0.0f)
        frame = bpm_frame_rate_control.bar_beat->get_bar_current_frame();

    std::vector<x_pv_game_stage_effect_glitter>& glitter = effect[stage_effect - 1ULL].glitter;
    for (x_pv_game_stage_effect_glitter& i : glitter) {
        i.scene_counter = Glitter::glt_particle_manager->LoadSceneEffect(i.name.hash, i.name.c_str());
        if (!i.scene_counter)
            continue;

        Glitter::glt_particle_manager->SetSceneFrameRate(i.scene_counter, &bpm_frame_rate_control);

        int32_t life_time = 0;
        Glitter::glt_particle_manager->GetSceneFrameLifeTime(i.scene_counter, &life_time);
        if (life_time <= 0)
            continue;

        if (stage_effect < 8 || stage_effect > 9) {
            float_t _life_time = (float_t)life_time;
            while (frame >= _life_time)
                frame -= _life_time;
            Glitter::glt_particle_manager->SetSceneEffectReqFrame(i.scene_counter, frame);
        }
        else if (frame < (float_t)life_time)
            Glitter::glt_particle_manager->SetSceneEffectReqFrame(i.scene_counter, frame);
        else
            Glitter::glt_particle_manager->FreeSceneEffect(i.scene_counter, false);

        i.fade_time_left = i.fade_time;
    }
}

void x_pv_game_stage::stop_stage_change_effect() {
    if (!next_stage_effect)
        return;

    int32_t curr_stg_eff = curr_stage_effect;
    if (curr_stg_eff >= 1 && curr_stg_eff <= 12)
        curr_stg_eff--;
    else
        curr_stg_eff = 0;

    int32_t next_stg_eff = next_stage_effect;
    if (next_stg_eff >= 1 && next_stg_eff <= 12)
        next_stg_eff--;
    else
        next_stg_eff = 0;

    x_pv_game_stage_change_effect& chg_eff = change_effect[curr_stg_eff][next_stg_eff];

    for (x_pv_game_stage_effect_auth_3d& i : chg_eff.auth_3d) {
        int32_t& id = i.id;
        auth_3d_data_set_visibility(&id, false);
        auth_3d_data_set_enable(&id, false);
    }

    for (x_pv_game_stage_effect_glitter& i : chg_eff.glitter)
        if (i.scene_counter) {
            Glitter::glt_particle_manager->FreeSceneEffect(i.scene_counter, false);
            i.scene_counter = 0;
        }
}

void x_pv_game_stage::stop_stage_effect(bool reset_stage) {
    if (curr_stage_effect < 1 || curr_stage_effect > 12)
        return;

    if (next_stage_effect) {
        switch (stage_effect_transition_state) {
        case 0:
            stop_stage_effect_auth_3d(curr_stage_effect);
            break;
        case 1:
            stop_stage_change_effect();
            break;
        }
        stop_stage_effect_glitter(curr_stage_effect);
    }
    else {
        stop_stage_effect_auth_3d(curr_stage_effect);
        stop_stage_effect_glitter(curr_stage_effect);
    }

    if (reset_stage)
        stage_data.set_default_stage();

    curr_stage_effect = 0;
    next_stage_effect = 0;
}

void x_pv_game_stage::stop_stage_effect_auth_3d(int32_t stage_effect) {
    if (stage_effect < 1 || stage_effect > 12)
        return;

    std::vector<x_pv_game_stage_effect_auth_3d>& auth_3d = effect[stage_effect - 1ULL].auth_3d;
    for (x_pv_game_stage_effect_auth_3d& i : auth_3d) {
        int32_t& id = i.id;
        auth_3d_data_set_visibility(&id, false);
        auth_3d_data_set_enable(&id, false);
    }
}

void x_pv_game_stage::stop_stage_effect_glitter(int32_t stage_effect) {
    if (stage_effect < 1 || stage_effect > 12)
        return;

    std::vector<x_pv_game_stage_effect_glitter>& glitter = effect[stage_effect - 1ULL].glitter;
    for (x_pv_game_stage_effect_glitter& i : glitter)
        if (i.scene_counter) {
            Glitter::glt_particle_manager->FreeSceneEffect(i.scene_counter, false);
            i.scene_counter = 0;
        }
}

void x_pv_game_stage::unload() {
    if (!stage_resource || !stage_id)
        return;

    for (x_pv_game_stage_effect& i : effect) {
        for (x_pv_game_stage_effect_auth_3d& j : i.auth_3d) {
            auth_3d_data_unload_id(j.id, rctx_ptr);
            j.id = -1;
        }

        for (x_pv_game_stage_effect_glitter& j : i.glitter)
            if (j.scene_counter) {
                Glitter::glt_particle_manager->FreeSceneEffect(j.scene_counter, false);
                j.scene_counter = 0;
            }
    }

    if (stage_effect_transition_state == 1)
        stop_stage_change_effect();

    for (int32_t i = 0; i < X_PV_GAME_STAGE_EFFECT_COUNT; i++)
        for (int32_t j = 0; j < X_PV_GAME_STAGE_EFFECT_COUNT; j++) {
            x_pv_game_stage_change_effect& chg_eff = change_effect[i][j];
            for (x_pv_game_stage_effect_auth_3d& k : chg_eff.auth_3d) {
                auth_3d_data_unload_id(k.id, rctx_ptr);
                k.id = -1;
            }

            for (x_pv_game_stage_effect_glitter& k : chg_eff.glitter)
                if (k.scene_counter) {
                    Glitter::glt_particle_manager->FreeSceneEffect(k.scene_counter, false);
                    k.scene_counter = 0;
                }

            chg_eff.enable = false;
            chg_eff.auth_3d.clear();
            chg_eff.auth_3d.shrink_to_fit();
            chg_eff.glitter.clear();
            chg_eff.glitter.shrink_to_fit();
            chg_eff.bars_left = 0;
            chg_eff.bar_count = 0;
        }

    for (uint32_t& i : stage_glitter) {
        Glitter::EffectGroup* eff_group = Glitter::glt_particle_manager->GetEffectGroup(i);
        if (eff_group)
            eff_group->FreeModel();

        Glitter::glt_particle_manager->UnloadEffectGroup(i);
    }
    
    for (string_hash& i : stage_auth_3d)
        auth_3d_data_unload_category(i.hash);

    delete stage_resource;
    stage_resource = 0;
    stage_id = 0;

    state = 30;
}

x_pv_game::x_pv_game() : state(), pv_count(),  pv_index(), state_old(), frame(), frame_float(),
time(), rob_chara_ids(), play(), success(), chara_id(), pv_end(), playdata(), scene_rot_y(),
branch_mode(), pause(), step_frame(), pv_id(), stage_id(), charas(), modules() {
    light_auth_3d_id = -1;
    target_anim_fps = 60.0f;
    anim_frame_speed = 1.0f;
    scene_rot_mat = mat4_identity;
    for (::chara_index& i : charas)
        i = CHARA_MAX;
    for (int32_t& i : modules)
        i = 0;
}

x_pv_game::~x_pv_game() {

}

bool x_pv_game::Init() {
    task_rob_manager_append_task();
    return true;
}

#if BAKE_PNG || BAKE_VIDEO
static bool img_write = false;
#endif

#if BAKE_VIDEO
FILE* pipe;
#endif

bool x_pv_game::Ctrl() {
    if (state_old == 20)
        ctrl((float_t)((frame_float + get_delta_frame()) * (1.0 / 60.0)), get_delta_frame() * (float_t)(1.0 / 60.0));
    else
        ctrl(0.0f, 0.0f);

    switch (state_old) {
    case 0:
        return false;
    case 1: {
        pv_index = 0;
        pv_count = 1;

        pv_data[pv_index].pv_id = pv_id;

        char buf[0x100];
        for (int32_t i = 0; i < pv_count; i++) {
            x_pv_game_data& pv_data = this->pv_data[i];

            sprintf_s(buf, sizeof(buf), "pv%03d.pvpp", pv_data.pv_id);
            pv_data.play_param_file_handler.read_file(&data_list[DATA_X], "root+/pv/", buf);
            pv_data.stage = &stage_data;
        }

        state_old = 2;
    } break;
    case 2: {
        bool wait_load = false;
        for (int32_t i = 0; i < pv_count; i++)
            if (pv_data[i].play_param_file_handler.check_not_ready())
                wait_load |= true;

        if (wait_load)
            break;

        for (int32_t i = 0; i < pv_count; i++) {
            x_pv_game_data& pv_data = this->pv_data[i];

            const void* pvpp_data = pv_data.play_param_file_handler.get_data();
            size_t pvpp_size = pv_data.play_param_file_handler.get_size();

            pv_data.play_param = new pvpp;
            pv_data.play_param->read(pvpp_data, pvpp_size);

            pv_data.load(pv_id, &field_71994, charas);

            int32_t chara_index = 0;
            for (pvpp_chara& i : pv_data.play_param->chara) {
                if (i.motion.size() && rob_chara_ids[chara_index] == -1) {
                    rob_chara_pv_data pv_data;
                    pv_data.chara_size_index = chara_init_data_get_chara_size_index(charas[chara_index]);
                    int32_t chara_id = rob_chara_array_init_chara_index(
                        charas[chara_index], &pv_data, modules[chara_index], true);
                    if (chara_id >= 0 && chara_id < ROB_CHARA_COUNT)
                        rob_chara_ids[chara_index] = chara_id;
                }

                if (++chara_index >= ROB_CHARA_COUNT)
                    break;
            }
        }

        stage_data.load(stage_id, &field_71994, false);

        state_old = 3;
    } break;
    case 3: {
        bool wait_load = false;
        for (int32_t i = 0; i < ROB_CHARA_COUNT; i++)
            if (rob_chara_ids[i] != -1)
                wait_load |= !task_rob_manager_check_chara_loaded(rob_chara_ids[i]);

        if (wait_load)
            break;

        data_struct* x_data = &data_list[DATA_X];

        light_param_data_storage_data_set_pv_id(pv_data[pv_index].pv_id);

        auth_3d_data_load_category(light_category.c_str());

        state_old = 7;
    } break;
    case 7: {
        bool wait_load = false;

        if (!auth_3d_data_check_category_loaded(light_category.c_str()))
            wait_load |= true;

        if (wait_load)
            break;

        data_struct* x_data = &data_list[DATA_X];

        light_auth_3d_id = -1;
        {
            data_struct* aft_data = &data_list[DATA_AFT];
            auth_3d_database* auth_3d_db = &aft_data->data_ft.auth_3d_db;

            char buf[0x200];
            sprintf_s(buf, sizeof(buf), "STGPV%03d_EFF_LT_000", pv_data[pv_index].pv_id);
            uint32_t light_auth_3d_hash = hash_utf8_murmurhash(buf);
            for (auth_3d_database_uid& i : auth_3d_db->uid)
                if (hash_string_murmurhash(i.name) == light_auth_3d_hash) {
                    light_auth_3d_id = auth_3d_data_load_uid(
                        (int32_t)(&i - auth_3d_db->uid.data()), auth_3d_db);
                    if (!auth_3d_data_check_id_not_empty(&light_auth_3d_id)) {
                        light_auth_3d_id = -1;
                        break;
                    }

                    auth_3d_data_read_file(&light_auth_3d_id, auth_3d_db);
                    auth_3d_data_set_enable(&light_auth_3d_id, false);
                    auth_3d_data_set_visibility(&light_auth_3d_id, false);
                    break;
                }
        }

        state_old = 8;
    } break;
    case 8: {
        bool wait_load = false;

        if (auth_3d_data_check_id_not_empty(&light_auth_3d_id)
            && !auth_3d_data_check_id_loaded(&light_auth_3d_id))
            wait_load |= true;

        if (wait_load)
            break;

        x_pv_game_dsc_data& dsc_data = pv_data[pv_index].dsc_data;

        app::TaskWork::AppendTask(&pv_param_task::post_process_task, "PV POST PROCESS TASK");
        {
            data_struct* x_data = &data_list[DATA_X];

            dsc dscouth;
            dsc dsc_scene;
            dsc dsc_system;
            dsc dsc_easy;

            char path_buf[0x200];
            char file_buf[0x200];

            farc dsc_common_farc;
            sprintf_s(path_buf, sizeof(path_buf), "root+/pv_script/pv%03d/", pv_data[pv_index].pv_id);
            sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_common.farc", pv_data[pv_index].pv_id);
            x_data->load_file(&dsc_common_farc, path_buf, file_buf, farc::load_file);

            sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_mouth.dsc", pv_data[pv_index].pv_id);
            farc_file* dscouth_ff = dsc_common_farc.read_file(file_buf);
            if (dscouth_ff)
                dscouth.parse(dscouth_ff->data, dscouth_ff->size, DSC_X);

            sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_scene.dsc", pv_data[pv_index].pv_id);
            farc_file* dsc_scene_ff = dsc_common_farc.read_file(file_buf);
            if (dsc_scene_ff)
                dsc_scene.parse(dsc_scene_ff->data, dsc_scene_ff->size, DSC_X);

            sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_system.dsc", pv_data[pv_index].pv_id);
            farc_file* dsc_system_ff = dsc_common_farc.read_file(file_buf);
            if (dsc_system_ff)
                dsc_system.parse(dsc_system_ff->data, dsc_system_ff->size, DSC_X);

            sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_easy.dsc", pv_data[pv_index].pv_id);
            dsc_easy.type = DSC_X;
            x_data->load_file(&dsc_easy, path_buf, file_buf, dsc::load_file);

            dsc_data.dsc.merge(4, &dscouth, &dsc_scene, &dsc_system, &dsc_easy);

            struct miku_state {
                bool disp;
                int32_t disp_time;
                int32_t set_motion_time;
                uint32_t set_motion_data_offset;
            } state[6];

            for (miku_state& i : state) {
                i.disp = true;
                i.disp_time = -1;
                i.set_motion_time = -1;
                i.set_motion_data_offset = 0;
            }

            std::vector<miku_state> state_vec;

            x_pv_bar_beat_data* bar_beat_data = 0;
            int32_t beat_counter = 0;

            int32_t time = -1;
            int32_t frame = -1;
            for (::dsc_data& i : dsc_data.dsc.data) {
                if (i.func == DSC_X_END)
                    break;

                uint32_t* data = dsc_data.dsc.get_func_data(&i);
                switch (i.func) {
                case DSC_X_TIME: {
                    for (miku_state& i : state)
                        if (state[chara_id].disp && i.disp_time == time && i.set_motion_time != time)
                            state_vec.push_back(i);
                    time = (int32_t)data[0];
                    frame = (int32_t)roundf((float_t)time * (float_t)(60.0 / 100000.0));
                } break;
                case DSC_X_MIKU_DISP: {
                    int32_t chara_id = (int32_t)data[0];
                    state[chara_id].disp = (int32_t)data[1] == 1;
                    if (state[chara_id].disp)
                        state[chara_id].disp_time = time;
                } break;
                case DSC_X_SET_MOTION: {
                    int32_t chara_id = (int32_t)data[0];
                    state[chara_id].set_motion_time = time;
                    state[chara_id].set_motion_data_offset = i.data_offset;
                } break;
                }
            }

            if (state_vec.size()) {
                int32_t time = -1;
                for (miku_state& i : state_vec) {
                    if (i.disp_time != time) {
                        uint32_t* data = dsc_data.dsc.add_func(dsc_x_get_func_name(DSC_X_TIME),
                            DSC_X_TIME, dsc_x_get_func_length(DSC_X_TIME));
                        data[0] = i.disp_time;
                        time = i.disp_time;
                    }

                    uint32_t* new_data = dsc_data.dsc.add_func(dsc_x_get_func_name(DSC_X_SET_MOTION),
                        DSC_X_SET_MOTION, dsc_x_get_func_length(DSC_X_SET_MOTION));
                    uint32_t* data = dsc_data.dsc.data_buffer.data() + i.set_motion_data_offset;
                    memcpy(new_data, data, sizeof(uint32_t) * dsc_x_get_func_length(DSC_X_SET_MOTION));
                }
                dsc_data.dsc.rebuild();
            }

            /*sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_easy.dsc", pv_data.pv_id);
            void* data = 0;
            size_t size = 0;
            dsc.unparse(&data, &size);
            stream s;
            s.open(file_buf, "wb");
            s.write(data, size);
            free(data);*/
        }

        pv_expression_array_reset();

        for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
            playdata[i].reset();
            if (rob_chara_ids[i] == -1)
                continue;

            rob_chara* rob_chr = rob_chara_array_get(rob_chara_ids[i]);
            if (rob_chr) {
                playdata[i].rob_chr = rob_chr;
                rob_chr->frame_speed = anim_frame_speed;
                rob_chr->data.motion.step_data.step = anim_frame_speed;
                pv_expression_array_reset_data(i, rob_chr, anim_frame_speed);
            }
            pv_game_dsc_data_find_playdata_set_motion(this, i);
            pv_game_dsc_data_find_playdata_item_anim(this, i);
        }

        pv_game_dsc_data_find_set_motion(this);

        dsc_data.dsc_data_ptr = dsc_data.dsc.data.data();
        dsc_data.dsc_data_ptr_end = dsc_data.dsc_data_ptr + dsc_data.dsc.data.size();

        state_old = 9;
    } break;
    case 9: {
        if (skin_param_manager_array_check_task_ready())
            break;

        bool wait_load = false;

        for (int32_t i = 0; i < pv_count; i++)
            if (!pv_data[i].state || pv_data[i].state == 10) {
                pv_data[i].state = 10;
                pv_data[i].field_1C |= 0x08;
                wait_load |= true;
            }

        if (wait_load)
            break;

        for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
            if (rob_chara_ids[i] == -1)
                continue;

            int32_t chara_id = rob_chara_ids[i];
            rob_chara* rob_chr = rob_chara_array_get(chara_id);
            if (!rob_chr)
                continue;

            pv_osage_manager_array_reset(chara_id);
            pv_osage_manager_array_set_pv_set_motion(chara_id, set_motion[chara_id]);
            pv_osage_manager_array_set_pv_id(chara_id, pv_data[pv_index].pv_id, true);
        }

        state = 10;
        state_old = 10;
    } break;
    case 10: {
        if (pv_osage_manager_array_get_disp()/* || osage_play_data_manager_check_task_not_ready()*/)
            break;

        bool wait_load = false;

        for (int32_t i = 0; i < pv_count; i++)
            if (pv_data[i].state != 30)
                wait_load |= true;

        if (wait_load)
            break;

        state_old = 19;
    } break;
    case 19: {
        x_pv_game_change_field(this, 1, -1, -1);

        Glitter::glt_particle_manager->SetPause(false);
        extern float_t frame_speed;
        frame_speed = 1.0f;

        pause = false;
        step_frame = false;

        frame_float = 0.0;
        this->frame = (int32_t)frame_float;
        this->time = (int64_t)round(frame_float * (100000.0 / 60.0) * 10000.0);

        while (pv_data[pv_index].dsc_data.dsc_data_ptr != pv_data[pv_index].dsc_data.dsc_data_ptr_end
            && x_pv_game_dsc_process(this, this->time))
            pv_data[pv_index].dsc_data.dsc_data_ptr++;

        auth_3d_data_set_enable(&light_auth_3d_id, true);
        auth_3d_data_set_camera_root_update(&light_auth_3d_id, false);
        auth_3d_data_set_paused(&light_auth_3d_id, false);
        auth_3d_data_set_repeat(&light_auth_3d_id, false);
        auth_3d_data_set_visibility(&light_auth_3d_id, true);
        auth_3d_data_set_req_frame(&light_auth_3d_id, 0.0f);

#if BAKE_VIDEO
        char buf[0x400];
        sprintf_s(buf, sizeof(buf), "ffmpeg -y -f rawvideo -pix_fmt rgb24 -s %dx%d"
            " -r 60 -i - -c:v h264_nvenc -profile:v high -qp 22 -i_qfactor 1.00 -b_qfactor 1.00"
            " -bf 4 -me_range 24 -color_range 2 -colorspace bt709 -pix_fmt yuv420p"
            " H:\\C\\Videos\\ReDIVA_pv%03d.264", 3840, 2160, pv_data[pv_index].pv_id);
        pipe = _popen(buf, "wb");
#endif

        pause = true;

        state_old = 20;
    } break;
    case 20: {
        float_t delta_frame = get_delta_frame();
        frame_float += delta_frame;
        frame = (int32_t)frame_float;
        time = (int64_t)round(frame_float * (100000.0 / 60.0) * 10000.0);

        while (pv_data[pv_index].dsc_data.dsc_data_ptr != pv_data[pv_index].dsc_data.dsc_data_ptr_end
            && x_pv_game_dsc_process(this, time))
            pv_data[pv_index].dsc_data.dsc_data_ptr++;

        //x_pv_game_map_auth_3d_to_mot(this, delta_frame != 0.0f && frame > 0);
#if BAKE_PNG || BAKE_VIDEO
        img_write = true;
#endif

        if (!play || pv_end)
            state_old = 21;
    } break;
    case 21: {
#if BAKE_VIDEO
        fflush(pipe);
        _pclose(pipe);
#endif
        unload();
        state_old = 22;
    } break;
    case 22: {
        if (state)
            break;

        Glitter::glt_particle_manager->FreeScenes();
        SetDest();
    } break;
    }
    return false;
}

#if DOF_BAKE
static void a3da_key_rev(a3da_key& k, std::vector<float_t>& values_src) {
    std::vector<kft3> value;
    int32_t type = interpolate_chs_reverse_sequence(values_src, value);

    k = {};
    switch (type) {
    case 0:
    default:
        k.type = A3DA_KEY_NONE;
        return;
    case 1:
        k.value = value[0].value;
        k.type = A3DA_KEY_STATIC;
        break;
    case 2:
        break;
    }

    k.type = A3DA_KEY_HERMITE;
    k.keys = value;
    k.max_frame = (float_t)(values_src.size() + 1);
}
#endif

bool x_pv_game::Dest() {
#if DOF_BAKE
    {
        data_struct* x_data = &data_list[DATA_X];

        char buf[0x200];
        sprintf_s(buf, sizeof(buf), "CAMPV%03d", pv_data.pv_id);

        farc* f = new farc;
        x_data->load_file(f, "root+/auth_3d/", hash_utf8_murmurhash(buf), ".farc", farc::load_file);

        if ((pv_data.pv_id % 100) >= 25 && (pv_data.pv_id % 100) <= 30 && stage_data.stage_id >= 25 && stage_data.stage_id <= 30)
            sprintf_s(buf, sizeof(buf), "CAMPV%03d_100.a3da", pv_data.pv_id);
        else
            sprintf_s(buf, sizeof(buf), "CAMPV%03d_BASE.a3da", pv_data.pv_id);

        a3da a;
        farc_file* ff = f->read_file(buf);
        if (ff)
            a.read(ff->data, ff->size);
        delete f;

        if ((pv_data.pv_id % 100) >= 25 && (pv_data.pv_id % 100) <= 30 && stage_data.stage_id >= 25 && stage_data.stage_id <= 30)
            sprintf_s(buf, sizeof(buf), "DOF\\auth_3d\\CAMPV%03d_100", pv_data.pv_id);
        else
            sprintf_s(buf, sizeof(buf), "DOF\\auth_3d\\CAMPV%03d_BASE", pv_data.pv_id);

        a.ready = true;
        a.compressed = true;
        a.format = A3DA_FORMAT_AFT;
        a.dof = {};
        a.dof.has_dof = true;

        a3da_key_rev(a.dof.model_transform.translation.x, dof_cam_data.position_x);
        a3da_key_rev(a.dof.model_transform.translation.y, dof_cam_data.position_y);
        a3da_key_rev(a.dof.model_transform.translation.z, dof_cam_data.position_z);
        a.dof.model_transform.translation.x.raw_data = true;
        a.dof.model_transform.translation.y.raw_data = true;
        a.dof.model_transform.translation.z.raw_data = true;
        a3da_key_rev(a.dof.model_transform.scale.x, dof_cam_data.focus_range);
        a3da_key_rev(a.dof.model_transform.rotation.x, dof_cam_data.fuzzing_range);
        a3da_key_rev(a.dof.model_transform.rotation.y, dof_cam_data.ratio);

        a3da_key& rot_z = a.dof.model_transform.rotation.z;
        if (dof_cam_data.enable_frame.size() > 1) {
            rot_z.type = A3DA_KEY_HOLD;
            rot_z.keys.reserve(dof_cam_data.enable_frame.size());
            for (std::pair<int32_t, bool>& i : dof_cam_data.enable_frame)
                rot_z.keys.push_back({ (float_t)i.first, i.second ? 1.00051f : 0.0f });
            rot_z.max_frame = a.play_control.size;
        }
        else if (dof_cam_data.enable_frame.size()) {
            rot_z.type = A3DA_KEY_STATIC;
            rot_z.value = dof_cam_data.enable_frame[0].second ? 1.00051f : 0.0f;
        }
        else
            rot_z.type = A3DA_KEY_NONE;

        a.write(buf);
    }
#endif

    Unload();
    task_rob_manager_free_task();

    light_param_data_storage_data_reset();
    rctx_ptr->post_process.tone_map->set_saturate_coeff(1.0f);
    rctx_ptr->post_process.tone_map->set_scene_fade(vec4_null);
    rctx_ptr->post_process.tone_map->set_scene_fade_blend_func(0);
    rctx_ptr->post_process.dof->data.pv.enable = false;
    rctx_ptr->object_data.object_culling = true;
    rctx_ptr->draw_pass.shadow_ptr->range = 1.0f;

    Glitter::glt_particle_manager->SetPause(false);
    extern float_t frame_speed;
    frame_speed = 1.0f;

    extern bool close;
    //close = true;
    return true;
}

void x_pv_game::Disp() {
    if (state_old != 20)
        return;

}

#if BAKE_PNG || BAKE_VIDEO
static int32_t frame_prev = -1;
#endif

void x_pv_game::Basic() {
    if (state_old != 20)
        return;

#if DOF_BAKE
    if (dof_cam_data.frame != frame) {
        camera* cam = rctx_ptr->camera;
        post_process_dof* dof = rctx_ptr->post_process.dof;

        dof_pv pv;
        dof->get_dof_pv(&pv);

        vec3 interest;
        vec3 view_point;
        cam->get_interest(interest);
        cam->get_view_point(view_point);

        bool enable = false;
        if (pv.enable && pv.f2.ratio > 0.0f) {
            if (pv.f2.focus > 0.0f) {
                vec3 direction;
                vec3_sub(interest, view_point, direction);
                vec3_normalize(direction, direction);
                vec3_mult_scalar(direction, pv.f2.focus, direction);
                vec3_add(direction, view_point, interest);
            }
            else
                interest = view_point;
            enable = true;
        }

        dof_cam_data.position_x.push_back(interest.x);
        dof_cam_data.position_y.push_back(interest.y);
        dof_cam_data.position_z.push_back(interest.z);
        dof_cam_data.focus.push_back(pv.f2.focus);
        dof_cam_data.focus_range.push_back(pv.f2.focus_range);
        dof_cam_data.fuzzing_range.push_back(pv.f2.fuzzing_range);
        dof_cam_data.ratio.push_back(pv.f2.ratio);
        if (dof_cam_data.enable != enable) {
            dof_cam_data.enable_frame.push_back({ frame, enable });
            dof_cam_data.enable = enable;
        }
        dof_cam_data.frame = frame;
    }
#endif

    if (step_frame)
        if (pause)
            pause = false;
        else {
            pause = true;
            step_frame = false;
        }

    Glitter::glt_particle_manager->SetPause(pause);
    extern float_t frame_speed;
    frame_speed = pause ? 0.0f : 1.0f;

#if BAKE_VIDEO
    if (img_write && frame_prev != frame) {
        texture* rend_texture = rctx_ptr->post_process.rend_texture.color_texture;
        size_t width = rend_texture->width;
        size_t height = rend_texture->height;

        std::vector<uint8_t> temp_pixels;
        temp_pixels.resize(width * height * 3);
        gl_state_bind_texture_2d(rend_texture->tex);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, temp_pixels.data());
        gl_state_bind_texture_2d(0);

        std::vector<uint8_t> pixels;
        pixels.resize(width * height * 3);

        uint8_t* src = (uint8_t*)temp_pixels.data();
        uint8_t* dst = (uint8_t*)pixels.data();
        for (size_t y = 0; y < height; y++) {
            uint8_t* src1 = &src[y * width * 3];
            uint8_t* dst1 = &dst[(height - y - 1) * width * 3];
            memcpy(dst1, src1, width * 3);
        }

        temp_pixels.clear();
        temp_pixels.shrink_to_fit();

        fwrite(pixels.data(), 1, width * height * 3, pipe);
        fflush(pipe);

        frame_prev = frame;
        img_write = false;
    }
#endif

#if BAKE_PNG
    if (img_write && frame_prev != frame) {
        texture* rend_texture = rctx_ptr->post_process.rend_texture.color_texture;
        uint32_t width = rend_texture->width;
        uint32_t height = rend_texture->height;

        std::vector<uint8_t> temp_pixels;
        temp_pixels.resize(width * height * 4 * sizeof(uint8_t));
        gl_state_bind_texture_2d(rend_texture->tex);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, temp_pixels.data());
        gl_state_bind_texture_2d(0);

        std::vector<uint8_t> pixels;
        pixels.resize(width * height * 4 * sizeof(uint8_t));

        uint8_t* src = (uint8_t*)temp_pixels.data();
        uint8_t* dst = (uint8_t*)pixels.data();
        for (size_t y = 0; y < height; y++) {
            uint8_t* src1 = &src[y * width * 4];
            uint8_t* dst1 = &dst[(height - y - 1) * width * 4];
            for (size_t x = 0; x < width; x++) {
                *dst1++ = *src1++;
                *dst1++ = *src1++;
                *dst1++ = *src1++;
                *dst1++ = *src1++;
                //*dst1++ = reverse_endianness_uint16_t(*src1++);
                //*dst1++ = reverse_endianness_uint16_t(*src1++);
                //*dst1++ = reverse_endianness_uint16_t(*src1++);
                //*dst1++ = reverse_endianness_uint16_t(*src1++);
            }
        }

        temp_pixels.clear();
        temp_pixels.shrink_to_fit();

        std::vector<uint8_t> png;
        uint32_t error = lodepng::encode(png, pixels, (uint32_t)width, (uint32_t)height, LCT_RGBA, 8);
        if (!error) {
            char buf[0x200];
            sprintf_s(buf, sizeof(buf), "E:\\Rinku\\X\\pv%03d", pv_data[pv_index].pv_id);
            CreateDirectoryA(buf, 0);

            sprintf_s(buf, sizeof(buf), "E:\\Rinku\\X\\pv%03d\\%05d.png", pv_data[pv_index].pv_id, frame);
            lodepng::save_file(png, buf);
        }
        frame_prev = frame;
        img_write = false;
    }
#endif
}

void x_pv_game::Window() {
    if (state_old != 20)
        return;

    if (Input::IsKeyTapped(GLFW_KEY_K, GLFW_MOD_CONTROL))
        state_old = 21;
    else if (Input::IsKeyTapped(GLFW_KEY_K))
        pause ^= true;

    if (Input::IsKeyTapped(GLFW_KEY_L)) {
        pause = true;
        step_frame = true;
    }

    if (!pause) {
        Basic();
        return;
    }

    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    float_t w = 240.0f;
    float_t h = 86.0f;

    extern int32_t width;
    ImGui::SetNextWindowPos({ (float_t)width - w, 0.0f }, ImGuiCond_Always);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    bool collapsed = !ImGui::Begin("X PV GAME", 0, window_flags);
    if (collapsed) {
        ImGui::End();
        return;
    }

    w = ImGui::GetContentRegionAvailWidth();
    if (ImGui::BeginTable("buttons", 2)) {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, w * 0.5f);

        ImGui::TableNextColumn();
        w = ImGui::GetContentRegionAvailWidth();
        if (ImGui::ButtonEnterKeyPressed(pause || (!pause && step_frame) ? "Play (K)" : "Pause (K)", { w, 0.0f }))
            pause ^= true;

        ImGui::TableNextColumn();
        w = ImGui::GetContentRegionAvailWidth();
        if (ImGui::ButtonEnterKeyPressed("Step Frame (L)", { w, 0.0f })) {
            pause = true;
            step_frame = true;
        }

        ImGui::TableNextColumn();
        w = ImGui::GetContentRegionAvailWidth();
        if (ImGui::ButtonEnterKeyPressed("Stop (Ctrl+K)", { w, 0.0f }))
            state_old = 21;

        ImGui::TableNextColumn();
        w = ImGui::GetContentRegionAvailWidth();
        char buf[0x200];
        sprintf_s(buf, sizeof(buf), "%d", frame);
        ImGui::PushStyleColor(ImGuiCol_Button, { 0.0f, 0.0f, 0.0f, 0.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.0f, 0.0f, 0.0f, 0.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.0f, 0.0f, 0.0f, 0.0f });
        ImGui::ButtonExEnterKeyPressed(buf, { w, 0.0f }, ImGuiButtonFlags_DontClosePopups
            | ImGuiButtonFlags_NoNavFocus | ImGuiButtonFlags_NoHoveredOnFocus);
        ImGui::PopStyleColor(3);
        ImGui::EndTable();
    }

    extern bool input_locked;
    input_locked |= ImGui::IsWindowFocused();
    ImGui::End();

    Basic();
}

void x_pv_game::Load(int32_t pv_id, int32_t stage_id, ::chara_index charas[6], int32_t modules[6]) {
    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    this->pv_id = pv_id;
    this->stage_id = stage_id;
    memmove(this->charas, charas, sizeof(this->charas));
    memmove(this->modules, modules, sizeof(this->modules));

    data_struct* x_data = &data_list[DATA_X];

    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "PV%03d", pv_id);
    uint32_t mot_set = aft_mot_db->get_motion_set_id(buf);
    motion_set_load_motion(mot_set, 0, aft_mot_db);
    motion_set_load_mothead(mot_set, 0, aft_mot_db);

    pv_param::post_process_data_load_files(pv_id);

    {
        dof d;
        sprintf_s(buf, sizeof(buf), "pv%03d", pv_id);
        x_data->load_file(&d, "root+/post_process_table/", hash_utf8_murmurhash(buf), ".dft", dof::load_file);
        pv_param::post_process_data_set_dof(d);
    }

    sprintf_s(buf, sizeof(buf), "EFFSTGPV%03d", 800 + stage_id);
    light_category = buf;

    int32_t chara_index = 0;

    play = true;
    success = true;
    chara_id = 0;
    target_anim_fps = get_target_anim_fps();
    anim_frame_speed = get_anim_frame_speed();
    pv_end = false;
    for (x_pv_play_data& i : playdata)
        i.reset();
    scene_rot_y = 0.0f;
    scene_rot_mat = mat4_identity;
    branch_mode = 0;

    pause = false;
    step_frame = false;

    for (int32_t& i : rob_chara_ids)
        i = -1;

    state = 10;
    state_old = 1;
    this->frame = 0;
    frame_float = 0.0;
    time = 0;

    for (std::vector<pv_data_set_motion>& i : set_motion) {
        i.clear();
        i.shrink_to_fit();
    }

    //pv_auth_3d_chara_count.clear();

    Glitter::glt_particle_manager->draw_all = false;
}

mot_key_set_type mot_fit_keys_into_curve(std::vector<float_t>& values_src,
    std::vector<uint16_t>& frames, std::vector<float_t>& values) {
    std::vector<kft3> value;
    int32_t type = interpolate_chs_reverse_sequence(values_src, value);

    frames.resize(0);
    values.resize(0);

    switch (type) {
    case 0:
    default:
        return MOT_KEY_SET_NONE;
    case 1:
        values.push_back(value[0].value);
        return MOT_KEY_SET_STATIC;
    case 2:
        break;
    }

    bool tangent = false;
    for (kft3& i : value)
        if (i.tangent1 != 0.0f || i.tangent2 != 0.0f) {
            tangent = true;
            break;
        }

    if (!tangent) {
        frames.reserve(value.size() + value.size() / 2);
        values.reserve((value.size() + value.size() / 2));

        for (kft3& i : value) {
            frames.push_back((uint16_t)i.frame);
            values.push_back(i.value);
            if (i.value == 0.0f) {
                frames.push_back((uint16_t)i.frame);
                values.push_back(i.value);
            }
        }
        return MOT_KEY_SET_HERMITE;
    }
    else {
        frames.reserve(value.size() + value.size() / 2);
        values.reserve((value.size() + value.size() / 2) * 2);

        for (kft3& i : value) {
            frames.push_back((uint16_t)i.frame);
            values.push_back(i.value);
            values.push_back(i.tangent1);

            if (i.value == 0.0f) {
                frames.push_back((uint16_t)i.frame);
                values.push_back(i.value);
                values.push_back(i.tangent1);
            }

            if (i.tangent1 != i.tangent2) {
                frames.push_back((uint16_t)i.frame);
                values.push_back(i.value);
                values.push_back(i.tangent2);

                if (i.value == 0.0f) {
                    frames.push_back((uint16_t)i.frame);
                    values.push_back(i.value);
                    values.push_back(i.tangent2);
                }
            }
        }
        return MOT_KEY_SET_HERMITE_TANGENT;
    }
}

/*bool mot_write_motion(void* data, const char* path, const char* file, uint32_t hash) {
    x_pv_game* xpvgm = (x_pv_game*)data;

    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "PV%03d", xpvgm->pv_data[xpvgm->pv_index].pv_id);
    motion_set_info* set_info = aft_mot_db->get_motion_set_by_name(buf);
    if (!set_info)
        return true;

    std::string mot_file = "mot_" + set_info->name + ".bin";

    mot_set* mot_set = new ::mot_set;
    {
        farc f;
        farc::load_file(&f, path, file, hash);

        farc_file* ff = f.read_file(mot_file.c_str());
        if (!ff) {
            delete mot_set;
            return true;
        }

        mot_set->unpack_file(ff->data, ff->size, false);
    }

    if (!mot_set->ready) {
        delete mot_set;
        return true;
    }

    for (auto& i : xpvgm->effchrpv_auth_3d_rob_mot_ids) {
        char buf[0x200];
        sprintf_s(buf, sizeof(buf), "PV826_OST_P%d_00", i.first + 1);

        int32_t motion_id = aft_mot_db->get_motion_id(buf);

        size_t motion_index = -1;
        for (motion_info& j : set_info->motion)
            if (j.id == motion_id) {
                motion_index = &j - set_info->motion.data();
                break;
            }

        mot_data* mot_data = &mot_set->vec[motion_index];

        uint16_t key_set_count = mot_data->key_set_count - 1;
        if (!key_set_count)
            continue;

        const char* name = bone_database_skeleton_type_to_string(BONE_DATABASE_SKELETON_COMMON);
        std::string* bone_names = aft_mot_db->bone_name.data();
        std::vector<bone_database_bone>* bones = 0;
        if (!aft_bone_data->get_skeleton_bones(name, &bones))
            continue;

        x_pv_game_a3da_to_mot& a2m = i.second;
        const mot_bone_info* bone_info = mot_data->bone_info.data();
        for (size_t key_set_offset = 0, i = 0; key_set_offset < key_set_count; i++) {
            motion_bone_index bone_index = (motion_bone_index)aft_bone_data->get_skeleton_bone_index(
                name, bone_names[bone_info[i].index].c_str());
            if (bone_index == -1) {
                i++;
                bone_index = (motion_bone_index)aft_bone_data->get_skeleton_bone_index(
                    name, bone_names[bone_info[i].index].c_str());
                if (bone_index == -1)
                    break;
            }

            bone_database_bone* bone = &(*bones)[bone_index];

            auto elem = a2m.bone_keys.find(bone_index);
            if (elem != a2m.bone_keys.end()) {
                x_pv_game_a3da_to_mot_keys& keys = elem->second;

                mot_key_set_data& key_set_data_x = mot_data->key_set[key_set_offset];
                key_set_data_x.frames.clear();
                key_set_data_x.values.clear();
                key_set_data_x.type = mot_fit_keys_into_curve(keys.x,
                    key_set_data_x.frames, key_set_data_x.values);
                key_set_data_x.keys_count = (uint16_t)key_set_data_x.frames.size();
                key_set_data_x.data_type = MOT_KEY_SET_DATA_F32;

                mot_key_set_data& key_set_data_y = mot_data->key_set[key_set_offset + 1];
                key_set_data_y.frames.clear();
                key_set_data_y.values.clear();
                key_set_data_y.type = mot_fit_keys_into_curve(keys.y,
                    key_set_data_y.frames, key_set_data_y.values);
                key_set_data_y.keys_count = (uint16_t)key_set_data_y.frames.size();
                key_set_data_y.data_type = MOT_KEY_SET_DATA_F32;

                mot_key_set_data& key_set_data_z = mot_data->key_set[key_set_offset + 2];
                key_set_data_z.frames.clear();
                key_set_data_z.values.clear();
                key_set_data_z.type = mot_fit_keys_into_curve(keys.z,
                    key_set_data_z.frames, key_set_data_z.values);
                key_set_data_z.keys_count = (uint16_t)key_set_data_z.frames.size();
                key_set_data_z.data_type = MOT_KEY_SET_DATA_F32;
            }

            elem = a2m.sec_bone_keys.find(bone_index);
            if (elem != a2m.sec_bone_keys.end()) {
                x_pv_game_a3da_to_mot_keys& keys = elem->second;

                mot_key_set_data& key_set_data_x = mot_data->key_set[key_set_offset + 3];
                key_set_data_x.frames.clear();
                key_set_data_x.values.clear();
                key_set_data_x.type = mot_fit_keys_into_curve(keys.x,
                    key_set_data_x.frames, key_set_data_x.values);
                key_set_data_x.keys_count = (uint16_t)key_set_data_x.frames.size();
                key_set_data_x.data_type = MOT_KEY_SET_DATA_F32;

                mot_key_set_data& key_set_data_y = mot_data->key_set[key_set_offset + 4];
                key_set_data_y.frames.clear();
                key_set_data_y.values.clear();
                key_set_data_y.type = mot_fit_keys_into_curve(keys.y,
                    key_set_data_y.frames, key_set_data_y.values);
                key_set_data_y.keys_count = (uint16_t)key_set_data_y.frames.size();
                key_set_data_y.data_type = MOT_KEY_SET_DATA_F32;

                mot_key_set_data& key_set_data_z = mot_data->key_set[key_set_offset + 5];
                key_set_data_z.frames.clear();
                key_set_data_z.values.clear();
                key_set_data_z.type = mot_fit_keys_into_curve(keys.z,
                    key_set_data_z.frames, key_set_data_z.values);
                key_set_data_z.keys_count = (uint16_t)key_set_data_z.frames.size();
                key_set_data_z.data_type = MOT_KEY_SET_DATA_F32;
            }

            if (bone_index == MOTION_BONE_KL_AGO_WJ) {
                mot_key_set_data& key_set_data_x = mot_data->key_set[key_set_offset];
                key_set_data_x.frames.clear();
                key_set_data_x.values.clear();
                key_set_data_x.values.push_back(0.0491406508f);
                key_set_data_x.type = MOT_KEY_SET_STATIC;
                key_set_data_x.keys_count = 1;
                key_set_data_x.data_type = MOT_KEY_SET_DATA_F32;

                mot_key_set_data& key_set_data_y = mot_data->key_set[key_set_offset + 1];
                key_set_data_y.frames.clear();
                key_set_data_y.values.clear();
                key_set_data_y.type = MOT_KEY_SET_NONE;
                key_set_data_y.keys_count = 0;
                key_set_data_y.data_type = MOT_KEY_SET_DATA_F32;

                mot_key_set_data& key_set_data_z = mot_data->key_set[key_set_offset + 2];
                key_set_data_z.frames.clear();
                key_set_data_z.values.clear();
                key_set_data_z.type = MOT_KEY_SET_NONE;
                key_set_data_z.keys_count = 0;
                key_set_data_z.data_type = MOT_KEY_SET_DATA_F32;
            }
            else if (bone_index == MOTION_BONE_N_KUBI_WJ_EX) {
                mot_key_set_data& key_set_data_x = mot_data->key_set[key_set_offset];
                key_set_data_x.frames.clear();
                key_set_data_x.values.clear();
                key_set_data_x.type = MOT_KEY_SET_NONE;
                key_set_data_x.keys_count = 1;
                key_set_data_x.data_type = MOT_KEY_SET_DATA_F32;

                mot_key_set_data& key_set_data_y = mot_data->key_set[key_set_offset + 1];
                key_set_data_y.frames.clear();
                key_set_data_y.values.clear();
                key_set_data_y.values.push_back(0.0331281610f);
                key_set_data_y.type = MOT_KEY_SET_STATIC;
                key_set_data_y.keys_count = 0;
                key_set_data_y.data_type = MOT_KEY_SET_DATA_F32;

                mot_key_set_data& key_set_data_z = mot_data->key_set[key_set_offset + 2];
                key_set_data_z.frames.clear();
                key_set_data_z.values.clear();
                key_set_data_z.type = MOT_KEY_SET_NONE;
                key_set_data_z.keys_count = 0;
                key_set_data_z.data_type = MOT_KEY_SET_DATA_F32;
            }

            if (bone->type >= BONE_DATABASE_BONE_POSITION_ROTATION)
                key_set_offset += 6;
            else
                key_set_offset += 3;
        }
    }

    {
        farc f;

        f.add_file(mot_file.c_str());
        farc_file* ff = &f.files.back();
        mot_set->pack_file(&ff->data, &ff->size);

        std::string mot_farc = "mot_" + set_info->name;
        f.write(mot_farc.c_str(), FARC_COMPRESS_FArC, false);
    }

    delete mot_set;
    return true;
}*/

void x_pv_game::Unload() {
    /*{
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        char buf[0x200];
        sprintf_s(buf, sizeof(buf), "PV%03d", pv_data[pv_index].pv_id);
        motion_set_info* set_info = aft_mot_db->get_motion_set_by_name(buf);
        if (set_info) {
            std::string farc_file = "mot_" + set_info->name + ".farc";
            aft_data->load_file(this, "rom/rob/", farc_file.c_str(), mot_write_motion);
        }
    }*/

    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++)
        pv_expression_array_reset_motion(i);

    pv_param::post_process_data_clear_data();

    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "PV%03d", pv_data[pv_index].pv_id);
    uint32_t mot_set = aft_mot_db->get_motion_set_id(buf);
    motion_set_unload_motion(mot_set);
    motion_set_unload_mothead(mot_set);

    state_old = 0;
    frame = 0;
    frame_float = 0.0;
    time = 0;

    for (int32_t& i : rob_chara_ids)
        if (i != -1) {
            rob_chara_array_free_chara_id(i);
            i = -1;
        }

    auth_3d_data_unload_id(light_auth_3d_id, rctx_ptr);

    light_auth_3d_id = -1;

    auth_3d_data_unload_category(light_category.c_str());

    light_category.clear();
    light_category.shrink_to_fit();

    /*for (uint32_t& i : objset_load)
        object_storage_unload_set(i);*/

    /*objset_load.clear();
    objset_load.shrink_to_fit();*/

    for (int32_t i = 0; i < pv_count; i++)
        pv_data[i].reset();
    stage_data.reset();

    for (std::vector<pv_data_set_motion>& i : set_motion) {
        i.clear();
        i.shrink_to_fit();
    }

    pv_data[pv_index].bar_beat.reset();

    pv_data[pv_index].stage_effects.clear();
    pv_data[pv_index].stage_effects.shrink_to_fit();
    pv_data[pv_index].stage_effect_index = 0;

    //pv_auth_3d_chara_count.clear();

    play = true;
    success = true;
    chara_id = 0;
    target_anim_fps = get_target_anim_fps();
    anim_frame_speed = get_anim_frame_speed();
    pv_end = false;
    for (x_pv_play_data& i : playdata)
        i.reset();
    scene_rot_y = 0.0f;
    scene_rot_mat = mat4_identity;
    branch_mode = 0;

    pv_id = 0;
    stage_id = 0;
    for (::chara_index& i : charas)
        i = CHARA_MAX;
    for (int32_t& i : modules)
        i = 0;

    Glitter::glt_particle_manager->draw_all = true;
    pv_param_task::post_process_task.SetDest();
}

void x_pv_game::ctrl(float_t curr_time, float_t delta_time) {
    for (int32_t i = 0; i < pv_count; i++)
        pv_data[i].ctrl(curr_time, delta_time);

    switch (state) {
    case 10:
        if (stage_data.state == 20)
            state = 20;
        break;
    case 30: {
        bool v38 = true;
        x_pv_game_data* pv_data = this->pv_data;
        for (int32_t i = 0; i < pv_count; i++, pv_data++)
            if (pv_data->state == 10 || pv_data->state == 20) {
                v38 = false;
                break;
            }

        if (!v38)
            break;

        stop_current_pv();
        this->pv_data[pv_index].unload_if_state_is_0();
        pv_index++;

        state = 20;
    } break;
    case 40: {
        bool v19 = true;
        x_pv_game_data* pv_data = this->pv_data;
        for (int32_t i = 0; i < pv_count; i++, pv_data++)
            if (pv_data->state == 10 || pv_data->state == 20) {
                v19 = false;
                break;
            }

        if (!v19)
            break;

        stop_current_pv();

        pv_data = this->pv_data;
        for (int32_t i = 0; i < pv_count; i++, pv_data++) {
            if (!pv_data->state) {
                rctx_ptr->camera->reset();
                pv_data->field_1C &= ~0xD1;
                pv_data->state = 0;
                pv_data->stage_effect_index = 0;
                pv_data->next_stage_effect_bar = 0;
                pv_data->dsc_data.reset();
            }

            if (i > 0)
                pv_data->unload_if_state_is_0();
        }


        if (pv_index) {
            pv_index = 0;
            if (!pv_data->state)
                pv_data->state = 20;
            else if (pv_data->state == 10)
                field_1C |= 0x08;
        }
        state = 20;
    }
    case 50: {
        bool v16 = false;
        for (int32_t i = 0; i < pv_count; i++)
            if (pv_data[i].state)
                v16 = true;

        if (!v16 && stage_data.state != 20)
            state = 0;
    } break;
    }

    stage_data.ctrl(delta_time);
}

void x_pv_game::stop_current_pv() {
    if (pv_count <= 0)
        return;

    pv_data[pv_index].stop();
    field_7198C.Reset();
    field_71994.Reset();
}

void x_pv_game::unload() {
    for (int32_t i = 0; i < pv_count; i++)
        pv_data[i].unload();
    stage_data.unload();
    state = 50;
}

XPVGameSelector::XPVGameSelector() : charas(), modules(), start(), exit() {
    pv_id = 0;
    stage_id = 0;

    for (chara_index& i : charas)
        i = CHARA_MIKU;
    
    for (int32_t& i : modules)
        i = 0;
}

XPVGameSelector::~XPVGameSelector() {

}

bool XPVGameSelector::Init() {
    pv_id = 0;
    stage_id = 0;

    for (chara_index& i : charas)
        i = CHARA_MIKU;

    for (int32_t& i : modules)
        i = 0;

    start = false;
    exit = false;
    return true;
}

bool XPVGameSelector::Ctrl() {
    return false;
}

bool XPVGameSelector::Dest() {
    return true;
}

void XPVGameSelector::Window() {
    static const char* pv_names[] = {
        "801. Strangers",
        "802. Ai Dee",
        "803. Streaming Heart",
        "804. Babylon",
        "805. The Lost One's Weeping",
        "806. Slow Motion",
        "807. Tale of the Deep-sea Lily",
        "808. Love Trial",
        "809. Love Song",
        "810. The First Sound",
        "811. LOL - lots of laugh -",
        "812. Patchwork Staccato",
        "813. Even a Kunoichi Needs Love",
        "814. Calc.",
        "815. A Single Red Leaf",
        "816. Holy Lance Explosion Boy",
        "817. Urotander, Underhanded Rangers",
        "818. Humorous Dream of Mrs. Pumpkin",
        "819. Solitary Envy",
        "820. Raspberry ＊ Monster",
        "821. Brain Revolution Girl",
        "822. Amazing Dolce",
        "823. Name of the Sin",
        "824. Satisfaction",
        "825. Cute Medley - Idol Sounds",
        "826. Beginning Medley - Primary Colors",
        "827. Cool Medley - Cyber Rock Jam",
        "828. Elegant Medley - Glossy Mixture",
        "829. Quirky Medley - Giga-Remix",
        "830. Ending Medley - Ultimate Exquisite Rampage",
        "831. Sharing The World",
        "832. Hand in Hand",
    };

    static const char* stage_names[] = {
        "001. Nighttime Curb Stage",
        "002. Digital Concert Stage",
        "003. Secret Ruins Stage",
        "004. Utopia Stage",
        "005. Classroom Concert Stage",
        "006. Capsule Room Stage",
        "007. Deep Sea Stage",
        "008. Verdict Stage",
        "009. Sky Garden Stage",
        "010. Musical Dawn Stage",
        "011. Sweet Dreams Stage",
        "012. Heart's Playroom Stage",
        "013. Ninja Village Stage",
        "014. Park of Promises Stage",
        "015. Autumn Sakura Stage",
        "016. Classy Lounge Stage",
        "017. Secret Base Stage",
        "018. Halloween Party Stage",
        "019. Twilight Shrine Stage",
        "020. Raspberry Stage",
        "021. Cabaret Stage",
        "022. Witch's House Stage",
        "023. Lilies & Shadows Stage",
        "024. Techno Club Stage",
        "025. Cute Concert Hall",
        "026. First Concert Hall",
        "027. Cool Concert Hall",
        "028. Elegant Concert Hall",
        "029. Quirky Concert Hall",
        "030. Ultimate Concert Hall",
        "031. Highrise Stage",
        "032. Mirai Concert Stage",
    };

    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    extern int32_t height;

    float_t w = 400.0f;
    float_t h = (float_t)height;
    h = min(h, 432.0f);

    extern int32_t width;
    ImGui::SetNextWindowPos({ (float_t)width - w, 0.0f }, ImGuiCond_Always);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    window_focus = false;
    bool open = true;
    if (!ImGui::Begin("X PV Game Selector", &open, window_flags)) {
        ImGui::End();
        return;
    }
    else if (!open) {
        start = false;
        exit = true;
        ImGui::End();
        return;
    }

    pv_id -= 801;
    pv_id = max(pv_id, 0);
    ImGui::ColumnComboBox("PV", pv_names, 32, &pv_id, 0, false, &window_focus);
    pv_id += 801;

    stage_id--;
    stage_id = max(stage_id, 0);
    ImGui::ColumnComboBox("Stage", stage_names, 32, &stage_id, 0, false, &window_focus);
    stage_id++;

    char buf[0x100];
    char buf1[0x100];
    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
        chara_index chara_old = charas[i];

        sprintf_s(buf, sizeof(buf), "Chara %dP", i + 1);
        ImGui::ColumnComboBox(buf, chara_full_names, CHARA_MAX,
            (int32_t*)&charas[i], 0, false, &window_focus);

        if (chara_old != charas[i])
            modules[i] = 0;
    }

    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
        item_table* itm_tbl = item_table_array_get_table(charas[i]);
        sprintf_s(buf, sizeof(buf), "Module %dP", i + 1);
        sprintf_s(buf1, sizeof(buf1), "%d", modules[i]);

        ImGui::StartPropertyColumn(buf);
        if (ImGui::BeginCombo("", buf1, 0)) {
            for (const auto& j : itm_tbl->cos) {
                if (j.first == 499)
                    continue;

                ImGui::PushID(&j);
                sprintf_s(buf1, sizeof(buf1), "%d", j.first);
                if (ImGui::Selectable(buf1, modules[i] == j.first)
                    || ImGui::ItemKeyPressed(GLFW_KEY_ENTER, true)
                    || (ImGui::IsItemFocused() && modules[i] != j.first))
                    modules[i] = j.first;
                ImGui::PopID();

                if (modules[i] == j.first)
                    ImGui::SetItemDefaultFocus();
            }

            window_focus |= true;
            ImGui::EndCombo();
        }
        ImGui::EndPropertyColumn();
    }

    if (ImGui::Button("Start")) {
        start = true;
        exit = true;
    }

    ImGui::End();
}

extern void x_pv_game_init() {
    x_pv_game_ptr = new x_pv_game;
}

extern void x_pv_game_free() {
    if (x_pv_game_ptr) {
        delete x_pv_game_ptr;
        x_pv_game_ptr = 0;
    }
}

#if DOF_BAKE
dof_cam::dof_cam() {
    frame = -999999;
    enable = false;
}

dof_cam::~dof_cam() {

}
#endif

static float_t dsc_time_to_frame(int64_t time) {
    return (float_t)time / 1000000000.0f * 60.0f;
}

static void x_pv_game_chara_item_alpha_callback(void* data, int32_t chara_id, int32_t type, float_t alpha) {
    if (!data)
        return;

    x_pv_game* xpvgm = (x_pv_game*)data;

    x_pv_game_data& pv_data = xpvgm->pv_data[xpvgm->pv_index];

    if (chara_id < 0 || chara_id >= pv_data.play_param->chara.size())
        return;

    draw_task_flags draw_task_flags;
    switch (type) {
    case 0:
    default:
        draw_task_flags = DRAW_TASK_ALPHA_ORDER_1;
        break;
    case 1:
        draw_task_flags = DRAW_TASK_ALPHA_ORDER_2;
        break;
    case 2:
        draw_task_flags = DRAW_TASK_ALPHA_ORDER_3;
        break;
    }

    for (x_pv_game_song_effect& i : pv_data.effect.song_effect) {
        if (i.chara_id != chara_id)
            continue;

        for (x_pv_game_song_effect_auth_3d& j : i.auth_3d)
            auth_3d_data_set_draw_task_flags_alpha(&j.id, draw_task_flags, alpha);

        for (x_pv_game_song_effect_glitter& j : i.glitter)
            Glitter::glt_particle_manager->SetSceneEffectExtColor(j.scene_counter,
                false, hash_murmurhash_empty, -1.0f, -1.0f, -1.0f, alpha);
    }
}

static void sub_140122B60(x_pv_game* a1, int32_t chara_id, int32_t motion_index, int64_t disp_time) {
    if (chara_id < 0 || chara_id > ROB_CHARA_COUNT || motion_index < 0)
        return;

    x_pv_play_data* playdata = &a1->playdata[chara_id];
    int64_t dsc_time = a1->pv_data[a1->pv_index].dsc_data.time;
    for (x_dsc_set_motion& i : playdata->motion_data.set_motion) {
        int64_t time = (int64_t)i.time * 10000;
        if (time > dsc_time && i.motion_index == motion_index
            && (!a1->branch_mode || a1->branch_mode == i.pv_branch_mode)) {
            rob_chara_bone_data* rob_bone_data = playdata->rob_chr->bone_data;
            rob_bone_data->motion_loaded.front()->mot_play_data.frame_data.field_14 =
                roundf(dsc_time_to_frame(time - disp_time)) - 1.0f;
            break;
        }
    }
}

static void x_pv_game_change_field(x_pv_game* xpvgm, int32_t field, int64_t dsc_time, int64_t curr_time) {
    light_param_data_storage_data_set_pv_cut(field);
}

static bool x_pv_game_dsc_process(x_pv_game* a1, int64_t curr_time) {
    x_pv_game_data& pv_data = a1->pv_data[a1->pv_index];

    dsc_x_func func = (dsc_x_func)pv_data.dsc_data.dsc_data_ptr->func;
    uint32_t* data = pv_data.dsc_data.dsc.get_func_data(
        pv_data.dsc_data.dsc_data_ptr);
    if (a1->branch_mode) {
        bool v19;
        if (a1->success)
            v19 = a1->branch_mode == 2;
        else
            v19 = a1->branch_mode == 1;
        if (!v19) {
            if (func < 0 || func > DSC_X_VR_LIVE_CHARA_VOICE) {
                a1->play = false;
                return false;
            }
            else if ((data[0] < 0 || data[0] > 1)
                && func != DSC_X_PV_END && func != DSC_X_PV_BRANCH_MODE)
                return true;
        }
    }

    switch (func) {
    case DSC_X_END: {
        a1->play = false;
        a1->state_old = 21;
        return false;
    } break;
    case DSC_X_TIME: {
        pv_data.dsc_data.time = (int64_t)(int32_t)data[0] * 10000;
        if (pv_data.dsc_data.time > curr_time)
            return false;
    } break;
    case DSC_X_MIKU_MOVE: {
        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        vec3 trans;
        trans.x = (float_t)(int32_t)data[1] * 0.001f;
        trans.y = (float_t)(int32_t)data[2] * 0.001f;
        trans.z = (float_t)(int32_t)data[3] * 0.001f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        mat4_mult_vec3_trans(&a1->scene_rot_mat,
            &trans, &rob_chr->data.miku_rot.position);
        rob_chr->set_osage_reset();
    } break;
    case DSC_X_MIKU_ROT: {
        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        float_t rot_y = (float_t)(int32_t)data[0] * 0.001f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        int16_t rot_y_int16 = (int32_t)((rot_y + a1->scene_rot_y) * 32768.0f * (float_t)(1.0 / 180.0));
        rob_chr->data.miku_rot.rot_y_int16 = rot_y_int16;
        rob_chr->set_osage_reset();
    } break;
    case DSC_X_MIKU_DISP: {
        data_struct* aft_data = &data_list[DATA_AFT];
        bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        int32_t disp = (int32_t)data[1];

        playdata->disp = disp == 1;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        if (disp == 1) {
            rob_chr->set_visibility(true);
            /*if (rob_chara::check_for_ageageagain_module(chara_index, module_index)) {
                sub_1405430F0(chara_id, 1);
                sub_1405430F0(chara_id, 2);
            }*/

            //pv_game::set_data_itmpv_visibility(a1->pv_game, a1->chara_id, true);
            for (x_pv_play_data_set_motion& i : playdata->set_motion) {
                bool v45 = rob_chr->set_motion_id(i.motion_id, i.frame,
                    i.duration, i.field_10, 0, i.blend_type, aft_bone_data, aft_mot_db);
                rob_chr->set_motion_reset_data(i.motion_id, i.dsc_frame);
                rob_chr->bone_data->disable_eye_motion = i.disable_eye_motion;
                rob_chr->data.motion.step_data.step = i.frame_speed;
                if (v45)
                   pv_expression_array_set_motion(pv_data.exp_file.hash, a1->chara_id, i.motion_id);
                //if (!a1->pv_game->data.pv->disable_calc_motfrm_limit)
                    sub_140122B60(a1, a1->chara_id, i.motion_index, i.dsc_time);
            }
            playdata->set_motion.clear();
        }
        else {
            rob_chr->set_visibility(false);
            //pv_game::set_data_itmpv_visibility(a1->pv_game, a1->chara_id, false);
        }
    } break;
    case DSC_X_MIKU_SHADOW: {
        a1->chara_id = data[0];
    } break;
    case DSC_X_TARGET: {

    } break;
    case DSC_X_SET_MOTION: {
        data_struct* aft_data = &data_list[DATA_AFT];
        bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        int32_t motion_index = (int32_t)data[1];
        int32_t duration_int = (int32_t)data[2];
        int32_t frame_speed_int = (int32_t)data[3];

        float_t duration;
        if (duration_int != -1){
            duration = (float_t)duration_int * 0.001f * 60.0f;
            if (duration < 0.0f)
                duration = 0.0f;
        }
        else
            duration = 0.0f;

        float_t frame_speed;
        if (frame_speed_int != -1) {
            frame_speed = (float_t)frame_speed_int * 0.001f;
            if (frame_speed < 0.0f)
                frame_speed = 0.0f;
        }
        else
            frame_speed = 1.0f;

        float_t frame = 0.0f;
        float_t dsc_frame = 0.0f;

        bool v11 = false;
        x_pv_play_data_motion* v56 = 0;
        for (x_pv_play_data_motion& i : playdata->motion)
            if (i.motion_index == motion_index) {
                v56 = &i;
                break;
            }

        if (v56 && v56->enable && (motion_index == v56->motion_index || !v56->motion_index)) {
            if (pv_data.dsc_data.time != v56->time) {
                frame = dsc_time_to_frame(curr_time - v56->time);
                dsc_frame = roundf(dsc_time_to_frame(pv_data.dsc_data.time - v56->time));
                if (frame < dsc_frame)
                    frame = dsc_frame;
                v11 = curr_time > v56->time;
            }
            else
                frame = 0.0f;
        }

        int32_t motion_id = -1;
        int32_t chara_id = 0;
        for (pvpp_chara& i : pv_data.play_param->chara) {
            if (chara_id++ != a1->chara_id)
                continue;

            int32_t mot_idx = 1;
            for (string_hash& j : i.motion) {
                if (mot_idx++ != motion_index)
                    continue;

                motion_id = aft_mot_db->get_motion_id(j.c_str());
                break;
            }
            break;
        }

        /*pv_db_pv_difficulty* v64 = 0i64;
        pv_db_pv* v65 = a1->pv_game->data.pv;
        pv_db_pv_difficulty* v66 = v65->difficulty[sub_14013C8C0()->difficulty].begin;
        pv_db_pv* v67 = a1->pv_game->data.pv;
        if (v66 == v67->difficulty[sub_14013C8C0()->difficulty].end)
            return 1;
        do {
            if (v66->edition == sub_14013C8C0()->edition)
                v64 = v66;
            ++v66;
            pv_db_pv* v68 = a1->pv_game->data.pv;
        } while (v66 != v68->difficulty[sub_14013C8C0()->difficulty].end);
        if (!v64)
            return 1;

        int32_t motion_id = vector_pv_db_pv_motion_get_element_by_index_or_null(&v64->motion[a1->chara_id], v52)->id;
        if (pv_game_data_get()->data.pv) {
            pv_db_pv_motion* v76 = vector_pv_db_pv_motion_get_element_by_index_or_null(&v64->motion[a1->chara_id], v52);
            motion_id = sub_1404EFA20(pv_game_data_get()->data.pv, rob_chr->chara_id, rob_chr->chara_index, v76);
        }*/

        if (motion_index) {
            if (motion_id == -1)
                motion_id = rob_chr->get_rob_cmn_mottbl_motion_id(0);
        }
        else if (motion_id == -1) {
            motion_id = rob_chr->get_rob_cmn_mottbl_motion_id(5);
            if (motion_id == -1)
                motion_id = rob_chr->get_rob_cmn_mottbl_motion_id(0);
        }

        duration /= a1->anim_frame_speed;
        if (v11)
            duration = 0.0f;

        x_pv_play_data_set_motion set_motion;
        set_motion.frame_speed = frame_speed * a1->anim_frame_speed;
        set_motion.motion_id = motion_id;
        set_motion.frame = frame;
        set_motion.duration = duration;
        set_motion.field_10 = v11;
        set_motion.blend_type = MOTION_BLEND_CROSS;
        set_motion.disable_eye_motion = true;
        set_motion.motion_index = motion_index;
        set_motion.dsc_time = v56 ? v56->time : pv_data.dsc_data.time;
        set_motion.dsc_frame = dsc_frame;

        //a1->field_2C560[a1->chara_id] = true;
        //a1->field_2C568[a1->chara_id] = set_motion;
        if (playdata->disp) {
            bool v84 = rob_chr->set_motion_id(motion_id, frame, duration,
                v11, false, MOTION_BLEND_CROSS, aft_bone_data, aft_mot_db);
            rob_chr->set_motion_reset_data(motion_id, dsc_frame);
            rob_chr->set_motion_skin_param(motion_id, dsc_frame);
            rob_chr->bone_data->disable_eye_motion = true;
            rob_chr->data.motion.step_data.step = set_motion.frame_speed;
            if (v84)
                pv_expression_array_set_motion(pv_data.exp_file.hash, a1->chara_id, motion_id);
            //if (!a1->pv_game->data.pv->disable_calc_motfrm_limit)
                sub_140122B60(a1, a1->chara_id, motion_index, v56 ? v56->time : 0);
        }
        else {
            playdata->set_motion.clear();
            playdata->set_motion.push_back(set_motion);
            rob_chr->data.motion.step_data.step = set_motion.frame_speed;
        }
    } break;
    case DSC_X_SET_PLAYDATA: {
        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        int32_t motion_index = (int32_t)data[1];
        if (motion_index < 0) {
            playdata->motion.clear();
            break;
        }

        x_pv_play_data_motion* motion = 0;
        for (x_pv_play_data_motion& i : playdata->motion)
            if (i.motion_index == motion_index) {
                motion = &i;
                break;
            }

        if (motion) {
            motion->enable = true;
            motion->motion_index = motion_index;
            motion->time = pv_data.dsc_data.time;
        }
        else {
            x_pv_play_data_motion motion;
            motion.enable = true;
            motion.motion_index = motion_index;
            motion.time = pv_data.dsc_data.time;
            playdata->motion.push_back(motion);
        }
    } break;
    case DSC_X_EFFECT: {

    } break;
    case DSC_X_FADEIN_FIELD: {

    } break;
    case DSC_X_EFFECT_OFF: {

    } break;
    case DSC_X_SET_CAMERA: {

    } break;
    case DSC_X_DATA_CAMERA: {

    } break;
    case DSC_X_CHANGE_FIELD: {
        int32_t field = (int32_t)data[0];
        if (field > 0)
            x_pv_game_change_field(a1, field, pv_data.dsc_data.time, curr_time);
        else
            x_pv_game_reset_field(a1);
    } break;
    case DSC_X_HIDE_FIELD: {

    } break;
    case DSC_X_MOVE_FIELD: {

    } break;
    case DSC_X_FADEOUT_FIELD: {

    } break;
    case DSC_X_EYE_ANIM: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        int32_t v115 = (int32_t)data[1];
        int32_t duration_int = (int32_t)data[2];

        float_t duration;
        if (duration_int != -1)
            duration = (float_t)duration_int * 0.001f * 60.0f;
        else
            duration = 6.0f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        duration /= a1->anim_frame_speed;

        float_t offset = 0.0f;
        //if (a1->pv_game->data.pv->is_old_pv)
        //    offset = 1.0f;

        rob_chr->set_eyelid_mottbl_motion_from_face(v115, duration, -1.0f, offset, aft_mot_db);
    } break;
    case DSC_X_MOUTH_ANIM: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        int32_t mouth_anim_id = (int32_t)data[2];
        int32_t duration_int = (int32_t)data[3];
        int32_t value_int = (int32_t)data[4];

        float_t duration;
        if (duration_int != -1) {
            duration = (float_t)duration_int * 0.001f * 60.0f;
            if (duration < 0.0f)
                duration = 0.0f;
        }
        else
            duration = 6.0f;

        float_t value;
        if (value_int != -1) {
            value = (float_t)value_int * 0.001f;
            if (value < 0.0f || value > 1.0f)
                value = 1.0f;
        }
        else
            value = 1.0f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        switch (mouth_anim_id) {
            case  4: mouth_anim_id = 34; break;
            case  5: mouth_anim_id = 35; break;
            case  6: mouth_anim_id = 36; break;
            case  7: mouth_anim_id = 37; break;
            case  8: mouth_anim_id = 38; break;
            case  9: mouth_anim_id = 39; break;
            case 10: mouth_anim_id =  4; break;
            case 11: mouth_anim_id =  5; break;
            case 12: mouth_anim_id =  6; break;
            case 13: mouth_anim_id =  7; break;
            case 14: mouth_anim_id =  8; break;
            case 15: mouth_anim_id =  9; break;
            case 16: mouth_anim_id = 10; break;
            case 17: mouth_anim_id = 11; break;
            case 18: mouth_anim_id = 12; break;
            case 19: mouth_anim_id = 13; break;
            case 20: mouth_anim_id = 14; break;
            case 21: mouth_anim_id = 15; break;
            case 22: mouth_anim_id = 16; break;
            case 23: mouth_anim_id = 17; break;
            case 24: mouth_anim_id = 18; break;
            case 25: mouth_anim_id = 19; break;
            case 26: mouth_anim_id = 20; break;
            case 27: mouth_anim_id = 21; break;
            case 28: mouth_anim_id = 22; break;
            case 29: mouth_anim_id = 40; break;
            case 30: mouth_anim_id = 41; break;
            case 31: mouth_anim_id = 42; break;
            default: printf(""); break;
        }

        int32_t mottbl_index = mouth_anim_id_to_mottbl_index(mouth_anim_id);
        duration /= a1->anim_frame_speed;

        //if (a1->pv_game->data.field_2D090 && mottbl_index != 144)
        //    value = 0.0f;

        float_t offset = 0.0f;
        //if (a1->pv_game->data.pv->is_old_pv)
        //    offset = 1.0f;

        rob_chr->set_mouth_mottbl_motion(0, mottbl_index, value,
            0, duration, 0.0f, 1.0f, -1, offset, aft_mot_db);
    } break;
    case DSC_X_HAND_ANIM: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        int32_t hand_index = (int32_t)data[1];
        int32_t hand_anim_id = (int32_t)data[2];
        int32_t duration_int = (int32_t)data[3];
        int32_t value_int = (int32_t)data[4];

        float_t duration;
        if (duration_int != -1) {
            duration = (float_t)duration_int * 0.001f * 60.0f;
            if (duration < 0.0f)
                duration = 0.0f;
        }
        else
            duration = 0.0f;

        float_t value;
        if (value_int != -1) {
            value = (float_t)value_int * 0.001f;
            if (value < 0.0f || value > 1.0f)
                value = 1.0f;
        }
        else
            value = 1.0f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        int32_t mottbl_index = hand_anim_id_to_mottbl_index(hand_anim_id);
        duration /= a1->anim_frame_speed;

        float_t offset = 0.0f;
        //if (a1->pv_game->data.pv->is_old_pv)
        //    offset = 1.0f;

        switch (hand_index) {
        case 0:
            rob_chr->set_hand_l_mottbl_motion(0, mottbl_index, value,
                0, duration, 0.0f, 1.0f, -1, offset, aft_mot_db);
            break;
        case 1:
            rob_chr->set_hand_r_mottbl_motion(0, mottbl_index, value,
                0, duration, 0.0f, 1.0f, -1, offset, aft_mot_db);
            break;
        }
    } break;
    case DSC_X_LOOK_ANIM: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        int32_t look_anim_id = (int32_t)data[1];
        int32_t duration_int = (int32_t)data[2];
        int32_t value_int = (int32_t)data[3];

        float_t duration;
        if (duration_int != -1) {
            duration = (float_t)duration_int * 0.001f * 60.0f;
            if (duration < 0.0f)
                duration = 0.0f;
        }
        else
            duration = 6.0f;

        float_t value;
        if (value_int != -1) {
            value = (float_t)value_int * 0.001f;
            if (value < 0.0f || value > 1.0f)
                value = 1.0f;
        }
        else
            value = 1.0f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        int32_t mottbl_index = look_anim_id_to_mottbl_index(look_anim_id);
        duration /= a1->anim_frame_speed;

        float_t offset = 0.0f;
        //if (a1->pv_game->data.pv->is_old_pv)
        //    offset = 1.0f;

        rob_chr->set_eyes_mottbl_motion(0, mottbl_index, value,
            mottbl_index == 224 ? 1 : 0, duration, 0.0f, 1.0f, -1, offset, aft_mot_db);
        return 1;
    } break;
    case DSC_X_EXPRESSION: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        int32_t expression_id = (int32_t)data[1];
        int32_t duration_int = (int32_t)data[2];
        int32_t value_int = (int32_t)data[3];

        float_t duration;
        if (duration_int != -1) {
            duration = (float_t)duration_int * 0.001f * 60.0f;
            if (duration < 0.0f)
                duration = 0.0f;
        }
        else
            duration = 0.0f;

        float_t value;
        if (value_int != -1) {
            value = (float_t)value_int * 0.001f;
            if (value < 0.0f || value > 1.0f)
                value = 1.0f;
        }
        else
            value = 1.0f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        int32_t mottbl_index = expression_id_to_mottbl_index(expression_id);
        duration /= a1->anim_frame_speed;

        bool v168 = true;
        //if (a1->has_perf_id && (a1->pv_game->data.pv->edit - 1) <= 1)
        //    v168 = false;

        float_t offset = 0.0f;
        //if (a1->pv_game->data.pv->is_old_pv)
        //    offset = 1.0f;

        rob_chr->set_face_mottbl_motion(0, mottbl_index, value, mottbl_index >= 214
            && mottbl_index <= 223 ? 1 : 0, duration, 0.0f, 1.0f, -1, offset, v168, aft_mot_db);

    } break;
    case DSC_X_LOOK_CAMERA: {

    } break;
    case DSC_X_LYRIC: {

    } break;
    case DSC_X_MUSIC_PLAY: {

    } break;
    case DSC_X_MODE_SELECT: {

    } break;
    case DSC_X_EDIT_MOTION: {

    } break;
    case DSC_X_BAR_TIME_SET: {

    } break;
    case DSC_X_SHADOWHEIGHT: {
        a1->chara_id = data[0];
    } break;
    case DSC_X_EDIT_FACE: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        int32_t expression_id = (int32_t)data[0];

        float_t duration = 0.0f;
        //if (func == DSC_EDIT_EXPRESSION)
        //    duration = (float_t)(int32_t)data[1] * 0.001f * 60.0f;

        int32_t v234 = -1;
        //if (!a1->has_perf_id)
        //    v234 = data[1];

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        int32_t mottbl_index = expression_id_to_mottbl_index(expression_id);
        duration /= a1->anim_frame_speed;

        bool v237 = true;
        //if (a1->has_perf_id)
        //    v237 = (a1->pv_game->data.pv->edit - 1) > 1;

        float_t offset = 0.0f;
        //if (a1->pv_game->data.pv->is_old_pv)
        //    offset = 1.0f;

        rob_chr->set_face_mottbl_motion(0, mottbl_index, 1.0f, mottbl_index >= 214
            && mottbl_index <= 223 ? 1 : 0, duration, 0.0f, 1.0f, -1, offset, v237, aft_mot_db);

        /*if (!a1->has_perf_id) {
            int32_t mottbl_index = mouth_anim_id_to_mottbl_index(v234);

            float_t value = 1.0f;
            //if (a1->pv_game->data.field_2D090 && mottbl_index != 144)
            //    value = 0.0f;

            rob_chr->set_mouth_mottbl_motion(0, mottbl_index, value, 0,
            a1->target_anim_fps * 0.1f, 0.0f, 1.0f, -1, offset, aft_mot_db);
        }*/
    } break;
    case DSC_X_DUMMY: {

    } break;
    case DSC_X_PV_END: {
        a1->pv_end = true;
        break;
    } break;
    case DSC_X_SHADOWPOS: {
        a1->chara_id = (int32_t)data[0];
    } break;
    case DSC_X_EDIT_LYRIC: {

    } break;
    case DSC_X_EDIT_TARGET: {

    } break;
    case DSC_X_EDIT_MOUTH: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        int32_t mouth_anim_id = (int32_t)data[1];
        float_t duration = 0.1f;
        //if (func == DSC_EDIT_MOUTH_ANIM)
        //    duration = (float_t)(int32_t)data[2] * 0.001f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        int32_t mottbl_index = mouth_anim_id_to_mottbl_index(mouth_anim_id);
        duration *= a1->target_anim_fps;

        float_t value = 1.0f;
        //if (a1->pv_game->data.field_2D090 && mottbl_index != 144)
        //    value = 0.0f;

        float_t offset = 0.0f;
        //if (a1->pv_game->data.pv->is_old_pv)
        //    offset = 1.0f;

        rob_chr->set_mouth_mottbl_motion(0, mottbl_index, value,
            0, duration, 0.0f, 1.0f, -1, offset, aft_mot_db);
    } break;
    case DSC_X_SET_CHARA: {
        a1->chara_id = (int32_t)data[0];
    } break;
    case DSC_X_EDIT_MOVE: {

    } break;
    case DSC_X_EDIT_SHADOW: {

    } break;
    case DSC_X_EDIT_EYELID: {

    } break;
    case DSC_X_EDIT_EYE: {

    } break;
    case DSC_X_EDIT_ITEM: {

    } break;
    case DSC_X_EDIT_EFFECT: {

    } break;
    case DSC_X_EDIT_DISP: {

    } break;
    case DSC_X_EDIT_HAND_ANIM: {

    } break;
    case DSC_X_AIM: {
        a1->chara_id = (int32_t)data[0];
    } break;
    case DSC_X_HAND_ITEM: {

    } break;
    case DSC_X_EDIT_BLUSH: {

    } break;
    case DSC_X_NEAR_CLIP: {

    } break;
    case DSC_X_CLOTH_WET: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        float_t value = (float_t)(int32_t)data[1] * 0.001f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        rob_chr->item_equip->wet = clamp(value, 0.0f, 1.0f);
    } break;
    case DSC_X_LIGHT_ROT: {

    } break;
    case DSC_X_SCENE_FADE: {

    } break;
    case DSC_X_TONE_TRANS: {
        vec3 start;
        vec3 end;
        start.x = (float_t)(int32_t)data[0] * 0.001f;
        start.y = (float_t)(int32_t)data[1] * 0.001f;
        start.z = (float_t)(int32_t)data[2] * 0.001f;
        end.x = (float_t)(int32_t)data[3] * 0.001f;
        end.y = (float_t)(int32_t)data[4] * 0.001f;
        end.z = (float_t)(int32_t)data[5] * 0.001f;
        rctx_ptr->post_process.tone_map->set_tone_trans(start, end);
    } break;
    case DSC_X_SATURATE: {
        float_t value = (float_t)(int32_t)data[0] * 0.001f;
        rctx_ptr->post_process.tone_map->set_saturate_coeff(value);
    } break;
    case DSC_X_FADE_MODE: {
        int32_t value = data[0];
        rctx_ptr->post_process.tone_map->set_scene_fade_blend_func(value);
    } break;
    case DSC_X_AUTO_BLINK: {

    } break;
    case DSC_X_PARTS_DISP: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        item_id id = (item_id)data[1];
        int32_t disp = (int32_t)data[2];

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        rob_chr->set_parts_disp(id, disp == 1);
    } break;
    case DSC_X_TARGET_FLYING_TIME: {

    } break;
    case DSC_X_CHARA_SIZE: {
        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        int32_t chara_size = (int32_t)data[1];

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            return 1;

        int32_t chara_size_index;
        if (chara_size == 0)
            chara_size_index = chara_init_data_get_chara_size_index(rob_chr->chara_index);
        else if (chara_size == 1) {
            chara_index chara_index = CHARA_MIKU;
            if (a1->chara_id < pv_data.play_param->chara.size())
                chara_index = (::chara_index)a1->pv_data[a1->pv_index]
                .play_param->chara[a1->chara_id].chara_effect.base_chara;
            else if (pv_data.pv_id == 826 && false)
                chara_index = rob_chara_array_get(a1->chara_id)->chara_index;

            //chara_index chara_index = pv_db_pv::get_performer_chara(a1->pv_game->data.pv, a1->chara_id);
            chara_size_index = chara_init_data_get_chara_size_index(chara_index);
        }
        else if (chara_size == 2)
            chara_size_index = 1;
        else if (chara_size == 3)
            chara_size_index = rob_chr->pv_data.chara_size_index;
        else {
            rob_chr->set_chara_size((float_t)chara_size / 1000.0f/*a1->field_2C54C*/);
            break;
        }

        if (chara_size_index < 0 || chara_size_index > 4)
            break;

        rob_chr->set_chara_size(chara_size_table_get_value(chara_size_index));
        rob_chr->set_chara_pos_adjust_y(chara_pos_adjust_y_table_get_value(chara_size_index));
    } break;
    case DSC_X_CHARA_HEIGHT_ADJUST: {
        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        int32_t height_adjust = (int32_t)data[1];

        rob_chara* rob_chr = playdata->rob_chr;
        if (rob_chr)
            rob_chr->set_chara_height_adjust(height_adjust != 0);
    } break;
    case DSC_X_ITEM_ANIM: {
        printf("");
    } break;
    case DSC_X_CHARA_POS_ADJUST: {
        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        vec3 pos;
        pos.x = (float_t)(int32_t)data[2] * 0.001f;
        pos.y = (float_t)(int32_t)data[3] * 0.001f;
        pos.z = (float_t)(int32_t)data[4] * 0.001f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        mat4_mult_vec3_trans(&a1->scene_rot_mat, &pos, &pos);
        rob_chr->set_chara_pos_adjust(&pos);

        /*if (rob_chara_check_for_ageageagain_module(rob_chr->chara_index, rob_chr->module_index)) {
            sub_1405430F0(rob_chr->chara_id, 1);
            sub_1405430F0(rob_chr->chara_id, 2);
        }*/
    } break;
    case DSC_X_SCENE_ROT: {
        float_t scene_rot_y = (float_t)(int32_t)data[0] * 0.001f;
        a1->scene_rot_y = scene_rot_y;

        mat4_rotate_y(scene_rot_y * DEG_TO_RAD_FLOAT, &a1->scene_rot_mat);

    } break;
    case DSC_X_EDIT_MOT_SMOOTH_LEN: {

    } break;
    case DSC_X_PV_BRANCH_MODE: {
        int32_t branch_mode = (int32_t)data[0];
        if (branch_mode >= 0 && branch_mode <= 2)
            a1->branch_mode = branch_mode;
    } break;
    case DSC_X_DATA_CAMERA_START: {

    } break;
    case DSC_X_MOVIE_PLAY: {

    } break;
    case DSC_X_MOVIE_DISP: {

    } break;
    case DSC_X_WIND: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        float_t wind_strength_kami = (float_t)(int32_t)data[1] * 0.001f;
        float_t wind_strength_outer = (float_t)(int32_t)data[2] * 0.001f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        rob_chr->item_equip->wind_strength = wind_strength_outer;
    } break;
    case DSC_X_OSAGE_STEP: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        float_t osage_step_kami = (float_t)(int32_t)data[1] * 0.001f;
        float_t osage_step_outer = (float_t)(int32_t)data[2] * 0.001f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        rob_chr->set_osage_step(osage_step_outer);
    } break;
    case DSC_X_OSAGE_MV_CCL: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        a1->chara_id = (int32_t)data[0];
        x_pv_play_data* playdata = &a1->playdata[a1->chara_id];

        float_t osage_mv_ccl_kami = (float_t)(int32_t)data[1] * 0.001f;
        float_t osage_mv_ccl_outer = (float_t)(int32_t)data[2] * 0.001f;

        rob_chara* rob_chr = playdata->rob_chr;
        if (!rob_chr)
            break;

        rob_chr->set_osage_move_cancel(1, osage_mv_ccl_kami);
        rob_chr->set_osage_move_cancel(2, osage_mv_ccl_outer);
    } break;
    case DSC_X_CHARA_COLOR: {

    } break;
    case DSC_X_SE_EFFECT: {

    } break;
    case DSC_X_CHARA_SHADOW_QUALITY: {

    } break;
    case DSC_X_STAGE_SHADOW_QUALITY: {

    } break;
    case DSC_X_COMMON_LIGHT: {

    } break;
    case DSC_X_TONE_MAP: {

    } break;
    case DSC_X_IBL_COLOR: {

    } break;
    case DSC_X_REFLECTION: {

    } break;
    case DSC_X_CHROMATIC_ABERRATION: {

    } break;
    case DSC_X_STAGE_SHADOW: {

    } break;
    case DSC_X_REFLECTION_QUALITY: {

    } break;
    case DSC_X_PV_END_FADEOUT: {

    } break;
    case DSC_X_CREDIT_TITLE: {

    } break;
    case DSC_X_BAR_POINT: {

    } break;
    case DSC_X_BEAT_POINT: {

    } break;
    case DSC_X_RESERVE1: {

    } break;
    case DSC_X_PV_AUTH_LIGHT_PRIORITY: {

    } break;
    case DSC_X_PV_CHARA_LIGHT: {

    } break;
    case DSC_X_PV_STAGE_LIGHT: {

    } break;
    case DSC_X_TARGET_EFFECT: {

    } break;
    case DSC_X_FOG: {

    } break;
    case DSC_X_BLOOM: {
        int32_t id = (int32_t)data[0];
        float_t duration = (float_t)(int32_t)data[1];

        pv_param::bloom& bloom_data = pv_param::post_process_data_get_bloom_data(id);

        pv_param_task::PostProcessCtrlBloom& bloom
            = pv_param_task::post_process_task.bloom;
        bloom.frame = 0.0f;
        bloom.duration = duration;
        bloom.data.data = bloom_data;
    } break;
    case DSC_X_COLOR_CORRECTION: {
        int32_t enable = (int32_t)data[0];
        int32_t id = (int32_t)data[1];
        float_t duration = (float_t)(int32_t)data[2];

        if (enable == 1) {
            pv_param::color_correction& cc_data
                = pv_param::post_process_data_get_color_correction_data(id);

            pv_param_task::PostProcessCtrlCC& cc
                = pv_param_task::post_process_task.cc;
            cc.frame = 0.0f;
            cc.duration = duration;
            cc.data.data = cc_data;
        }
    } break;
    case DSC_X_DOF: {
        int32_t enable = (int32_t)data[0];
        int32_t id = (int32_t)data[1];
        float_t duration = (float_t)(int32_t)data[2];

        rctx_ptr->post_process.dof->data.pv.enable = enable == 1;
        if (enable == 1) {
            pv_param::dof& dof_data = pv_param::post_process_data_get_dof_data(id);

            pv_param_task::PostProcessCtrlDof& dof
                = pv_param_task::post_process_task.dof;
            dof.frame = 0.0f;
            dof.duration = duration;
            dof.data.data = dof_data;
        }
    } break;
    case DSC_X_CHARA_ALPHA: {
        a1->chara_id = (int32_t)data[0];

        float_t alpha = (float_t)(int32_t)data[1] * 0.001f;
        float_t duration = (float_t)(int32_t)data[2];
        int32_t type = (int32_t)data[3];

        if (a1->chara_id >= 0 && a1->chara_id < ROB_CHARA_COUNT) {
            pv_param_task::PostProcessCtrlCharaAlpha& chara_alpha
                = pv_param_task::post_process_task.chara_alpha;
            pv_param::chara_alpha& chara_alpha_data = chara_alpha.data.data[a1->chara_id];
            chara_alpha_data.type = type;
            chara_alpha_data.frame = 0.0f;
            chara_alpha_data.alpha = alpha;
            chara_alpha_data.duration = duration * 2.0f;
        }

    } break;
    case DSC_X_AUTO_CAPTURE_BEGIN: {

    } break;
    case DSC_X_MANUAL_CAPTURE: {

    } break;
    case DSC_X_TOON_EDGE: {

    } break;
    case DSC_X_SHIMMER: {

    } break;
    case DSC_X_ITEM_ALPHA: {
        a1->chara_id = (int32_t)data[0];

        float_t alpha = (float_t)(int32_t)data[1] * 0.001f;
        float_t duration = (float_t)(int32_t)data[2];
        int32_t type = (int32_t)data[3];

        if (a1->chara_id >= 0 && a1->chara_id < ROB_CHARA_COUNT) {
            pv_param_task::PostProcessCtrlCharaItemAlpha& chara_item_alpha
                = pv_param_task::post_process_task.chara_item_alpha;

            pv_param::chara_alpha& chara_item_alpha_data = chara_item_alpha.data.data[a1->chara_id];
            chara_item_alpha_data.type = type;
            chara_item_alpha_data.frame = 0.0f;
            chara_item_alpha_data.alpha = alpha;
            chara_item_alpha_data.duration = duration * 2.0f;

            chara_item_alpha.callback[a1->chara_id] = x_pv_game_chara_item_alpha_callback;
            chara_item_alpha.callback_data[a1->chara_id] = a1;
        }
    } break;
    case DSC_X_MOVIE_CUT: {

    } break;
    case DSC_X_EDIT_CAMERA_BOX: {

    } break;
    case DSC_X_EDIT_STAGE_PARAM: {

    } break;
    case DSC_X_EDIT_CHANGE_FIELD: {

    } break;
    case DSC_X_MIKUDAYO_ADJUST: {

    } break;
    case DSC_X_LYRIC_2: {

    } break;
    case DSC_X_LYRIC_READ: {

    } break;
    case DSC_X_LYRIC_READ_2: {

    } break;
    case DSC_X_ANNOTATION: {

    } break;
    case DSC_X_STAGE_EFFECT: {
        int32_t stage_effect = (int32_t)data[0];

        if (stage_effect >= 8 && stage_effect <= 9)
            a1->stage_data.set_stage_effect(stage_effect);
    } break;
    case DSC_X_SONG_EFFECT: {
        bool enable = data[0] ? true : false;
        int32_t index = (int32_t)data[1];
        int32_t unk2 = (int32_t)data[2];

        if (unk2 && (~a1->field_1C & 0x10))
            break;

        if (enable) {
            pv_data.effect.set_song_effect(index, pv_data.dsc_data.time);
            //if (a2->field_18)
            //    pv_data.effect.set_song_effect_time(index/*, a2->field_20*/, true);
            //else
                pv_data.effect.set_song_effect_time(index, a1->time, false);
        }
        else
            pv_data.effect.stop_song_effect(index, true);
    } break;
    case DSC_X_SONG_EFFECT_ATTACH: {
        int32_t index = (int32_t)data[0];
        int32_t chara_id = (int32_t)data[1];
        int32_t unk2 = (int32_t)data[2];

        pv_data.effect.set_chara_id(index, chara_id, !!unk2);
    } break;
    case DSC_X_LIGHT_AUTH: {

    } break;
    case DSC_X_FADE: {

    } break;
    case DSC_X_SET_STAGE_EFFECT_ENV: {

    } break;
    case DSC_X_RESERVE2: {

    } break;
    case DSC_X_COMMON_EFFECT_AET_FRONT: {

    } break;
    case DSC_X_COMMON_EFFECT_AET_FRONT_LOW: {

    } break;
    case DSC_X_COMMON_EFFECT_PARTICLE: {

    } break;
    case DSC_X_SONG_EFFECT_ALPHA_SORT: {

    } break;
    case DSC_X_LOOK_CAMERA_FACE_LIMIT: {

    } break;
    case DSC_X_ITEM_LIGHT: {

    } break;
    case DSC_X_CHARA_EFFECT: {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        int32_t chara_id = (int32_t)data[0];
        bool enable = data[1] ? true : false;
        int32_t index = (int32_t)data[2];

        if (enable) {
            pv_data.chara_effect.set_chara_effect(chara_id, index, pv_data.dsc_data.time);
            //if (a2->field_18)
            //    pv_data.chara_effect.set_chara_effect_time(chara_id, index/*, a2->field_20*/);
            //else
                pv_data.chara_effect.set_chara_effect_time(chara_id, index, a1->time);
        }
        else
            pv_data.chara_effect.stop_chara_effect(chara_id, index);
    } break;
    case DSC_X_MARKER: {

    } break;
    case DSC_X_CHARA_EFFECT_CHARA_LIGHT: {

    } break;
    case DSC_X_ENABLE_COMMON_LIGHT_TO_CHARA: {

    } break;
    case DSC_X_ENABLE_FXAA: {

    } break;
    case DSC_X_ENABLE_TEMPORAL_AA: {

    } break;
    case DSC_X_ENABLE_REFLECTION: {

    } break;
    case DSC_X_BANK_BRANCH: {

    } break;
    case DSC_X_BANK_END: {

    } break;
    case DSC_X_NULL1: {

    } break;
    case DSC_X_NULL2: {

    } break;
    case DSC_X_NULL3: {

    } break;
    case DSC_X_NULL4: {

    } break;
    case DSC_X_NULL5: {

    } break;
    case DSC_X_NULL6: {

    } break;
    case DSC_X_NULL7: {

    } break;
    case DSC_X_NULL8: {

    } break;
    case DSC_X_VR_LIVE_MOVIE: {

    } break;
    case DSC_X_VR_CHEER: {

    } break;
    case DSC_X_VR_CHARA_PSMOVE: {

    } break;
    case DSC_X_VR_MOVE_PATH: {

    } break;
    case DSC_X_VR_SET_BASE: {

    } break;
    case DSC_X_VR_TECH_DEMO_EFFECT: {

    } break;
    case DSC_X_VR_TRANSFORM: {

    } break;
    case DSC_X_GAZE: {

    } break;
    case DSC_X_TECH_DEMO_GESUTRE: {

    } break;
    case DSC_X_VR_CHEMICAL_LIGHT_COLOR: {

    } break;
    case DSC_X_VR_LIVE_MOB: {

    } break;
    case DSC_X_VR_LIVE_HAIR_OSAGE: {

    } break;
    case DSC_X_VR_LIVE_LOOK_CAMERA: {

    } break;
    case DSC_X_VR_LIVE_CHEER: {

    } break;
    case DSC_X_VR_LIVE_GESTURE: {

    } break;
    case DSC_X_VR_LIVE_CLONE: {

    } break;
    case DSC_X_VR_LOOP_EFFECT: {

    } break;
    case DSC_X_VR_LIVE_ONESHOT_EFFECT: {

    } break;
    case DSC_X_VR_LIVE_PRESENT: {

    } break;
    case DSC_X_VR_LIVE_TRANSFORM: {

    } break;
    case DSC_X_VR_LIVE_FLY: {

    } break;
    case DSC_X_VR_LIVE_CHARA_VOICE: {

    } break;
    }
    return true;
}

/*static void set_bone_key_set_data(bone_data* bone_data,
    std::map<motion_bone_index, x_pv_game_a3da_to_mot_keys>& bone_keys,
    std::map<motion_bone_index, x_pv_game_a3da_to_mot_keys>& second_bone_keys, bool add_keys,
    motion_bone_index motion_bone_index, mot_key_set* key_set, vec3* data, int32_t count = 1) {
    if (add_keys) {
        auto elem = bone_keys.find(motion_bone_index);
        if (elem == bone_keys.end())
            elem = bone_keys.insert({ motion_bone_index, {} }).first;

        x_pv_game_a3da_to_mot_keys& keys = elem->second;
        keys.x.push_back(data[0].x);
        keys.y.push_back(data[0].y);
        keys.z.push_back(data[0].z);

        if (count == 2) {
            auto elem = second_bone_keys.find(motion_bone_index);
            if (elem == second_bone_keys.end())
                elem = second_bone_keys.insert({ motion_bone_index, {} }).first;

            x_pv_game_a3da_to_mot_keys& keys = elem->second;
            keys.x.push_back(data[1].x);
            keys.y.push_back(data[1].y);
            keys.z.push_back(data[1].z);
        }
    }

    bone_data += motion_bone_index;
    key_set += bone_data->key_set_offset;
    while (count > 0) {
        if (key_set[0].type == MOT_KEY_SET_STATIC && key_set[0].values)
            *(float_t*)&key_set[0].values[0] = data->x;
        if (key_set[1].type == MOT_KEY_SET_STATIC && key_set[1].values)
            *(float_t*)&key_set[1].values[0] = data->y;
        if (key_set[2].type == MOT_KEY_SET_STATIC && key_set[2].values)
            *(float_t*)&key_set[2].values[0] = data->z;
        key_set += 3;
        data++;
        count--;
    }
}

static void x_pv_game_map_auth_3d_to_mot(x_pv_game* xpvgm, bool add_keys) {
    for (auto& i : xpvgm->effchrpv_auth_3d_rob_mot_ids) {
        rob_chara* rob_chr = rob_chara_array_get(xpvgm->rob_chara_ids[i.first]);
        x_pv_game_a3da_to_mot& a2m = i.second;
        auth_3d* auth = auth_3d_data_get_auth_3d(a2m.auth_3d_id);
        auth_3d_object_hrc* oh = &auth->object_hrc[0];


        if (rob_chr->data.motion.motion_id == 0xC3) {
            data_struct* aft_data = &data_list[DATA_AFT];
            bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
            motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

            char buf[0x200];
            sprintf_s(buf, sizeof(buf), "PV826_OST_P%d_00", i.first + 1);
            int32_t motion_id = aft_mot_db->get_motion_id(buf);

            rob_chr->set_motion_id(motion_id, 0.0f,
                0.0f, false, false, MOTION_BLEND_CROSS, aft_bone_data, aft_mot_db);
            rob_chr->set_motion_reset_data(motion_id, 0.0f);
            rob_chr->set_motion_skin_param(motion_id, 0.0f);
        }

        rob_chr->set_visibility(oh->node[0].model_transform.visible);

        motion_blend_mot* mot = rob_chr->bone_data->motion_loaded.front();
        ::bone_data* bone_data = mot->bone_data.bones.data();
        mot_key_set* key_set = mot->mot_key_data.mot.key_sets;

        vec3 data[2];
        mat4 mat;
        data[0].x = oh->node[a2m.gblctr].model_transform.translation_value.x;
        data[0].y = oh->node[a2m.n_hara].model_transform.translation_value.y;
        data[0].z = oh->node[a2m.gblctr].model_transform.translation_value.z;
        data[1] = vec3_null;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_N_HARA_CP, key_set, data, 2);

        data[0] = oh->node[a2m.n_hara_y].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KG_HARA_Y, key_set, data);

        data[0] = oh->node[a2m.j_hara_wj].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_HARA_XZ, key_set, data);

        data[0] = oh->node[a2m.n_kosi].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_N_HARA, key_set, data);

        mat = oh->node[a2m.j_mune_wj].model_transform.mat;
        data[0] = { 0.0f, 0.945f, 0.0f };
        mat4_mult_vec3(&mat, &data[0], &data[1]);
        mat4_get_translation(&mat, &data[0]);
        vec3_add(data[0], data[1], data[0]);
        data[1] = vec3_null;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_CL_MUNE, key_set, data, 2);

        data[0] = oh->node[a2m.j_mune_b_wj].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_MUNE_B_WJ, key_set, data);

        data[0] = oh->node[a2m.j_kubi_wj].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_KUBI, key_set, data);

        data[0] = oh->node[a2m.n_kao].model_transform.rotation_value;
        data[0].z -= (float_t)M_PI;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_N_KAO, key_set, data);

        mat = oh->node[a2m.j_kao_wj].model_transform.mat;
        data[0] = { 0.0f, 0.40f, 0.0f };
        mat4_mult_vec3(&mat, &data[0], &data[1]);
        mat4_get_translation(&mat, &data[0]);
        vec3_add(data[0], data[1], data[0]);
        data[1] = vec3_null;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_CL_KAO, key_set, data, 2);

        data[0] = oh->node[a2m.j_eye_r_wj].model_transform.rotation_value;
        data[0].x += (float_t)(M_PI / 2.0);
        data[0].y = -data[0].z;
        data[0].z = 0.0f;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_EYE_R, key_set, data);

        data[0] = oh->node[a2m.j_eye_l_wj].model_transform.rotation_value;
        data[0].x += (float_t)(M_PI / 2.0);
        data[0].y = -data[0].z;
        data[0].z = 0.0f;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_EYE_L, key_set, data);

        data[0] = oh->node[a2m.n_waki_l].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_N_WAKI_L, key_set, data);

        data[1] = oh->node[a2m.j_waki_l_wj].model_transform.rotation_value;
        data[0].x = data[1].x;
        data[0].y = data[1].z;
        data[0].z = -data[1].y;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_WAKI_L_WJ, key_set, data);

        {
            vec3 pos_j_kata_l_wj;
            vec3 pos_j_ude_l_wj;
            vec3 pos_j_te_l_wj;
            mat4_get_translation(&oh->node[a2m.j_kata_l_wj].model_transform.mat, &pos_j_kata_l_wj);
            mat4_get_translation(&oh->node[a2m.j_ude_l_wj].model_transform.mat, &pos_j_ude_l_wj);
            mat4_get_translation(&oh->node[a2m.j_te_l_wj].model_transform.mat, &pos_j_te_l_wj);

            vec3 pos_middle;
            vec3_lerp_scalar(pos_j_kata_l_wj, pos_j_te_l_wj, pos_middle, 0.5f);

            vec3 tl_up_kata_dir;
            vec3_sub(pos_j_ude_l_wj, pos_middle, tl_up_kata_dir);

            float_t pos_middle_dist;
            vec3_length(tl_up_kata_dir, pos_middle_dist);
            if (pos_middle_dist == 0.0f) {
                mat = oh->node[a2m.j_kata_l_wj].model_transform.mat;
                data[0] = { 0.0f, 0.3f, 0.0f };
                mat4_mult_vec3(&mat, &data[0], &tl_up_kata_dir);
            }
            else
                vec3_mult_scalar(tl_up_kata_dir, 0.3f / pos_middle_dist, tl_up_kata_dir);

            vec3 tl_up_kata_pos;
            vec3_add(tl_up_kata_dir, pos_j_ude_l_wj, tl_up_kata_pos);

            mat = oh->node[a2m.j_mune_b_wj].model_transform.mat;
            mat4_mult_vec3_inv_trans(&mat, &tl_up_kata_pos, &data[0]);
            set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
                MOTION_BONE_TL_UP_KATA_L, key_set, data);
        }

        mat4_get_translation(&oh->node[a2m.j_te_l_wj].model_transform.mat, &data[0]);
        data[1].x = -(float_t)(M_PI / 2.0);
        data[1].y = -(float_t)(M_PI / 2.0);
        data[1].z = -(float_t)(M_PI / 2.0);
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_C_KATA_L, key_set, data, 2);

        data[0] = oh->node[a2m.j_te_l_wj].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_TE_L_WJ, key_set, data);

        data[0] = oh->node[a2m.n_waki_r].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_N_WAKI_R, key_set, data);

        data[1] = oh->node[a2m.j_waki_l_wj].model_transform.rotation_value;
        data[0].x = data[1].x;
        data[0].y = data[1].z;
        data[0].z = data[1].y;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_WAKI_R_WJ, key_set, data);

        {
            vec3 pos_j_kata_r_wj;
            vec3 pos_j_ude_r_wj;
            vec3 pos_j_te_r_wj;
            mat4_get_translation(&oh->node[a2m.j_kata_r_wj].model_transform.mat, &pos_j_kata_r_wj);
            mat4_get_translation(&oh->node[a2m.j_ude_r_wj].model_transform.mat, &pos_j_ude_r_wj);
            mat4_get_translation(&oh->node[a2m.j_te_r_wj].model_transform.mat, &pos_j_te_r_wj);

            vec3 pos_middle;
            vec3_lerp_scalar(pos_j_kata_r_wj, pos_j_te_r_wj, pos_middle, 0.5f);

            vec3 tl_up_kata_dir;
            vec3_sub(pos_j_ude_r_wj, pos_middle, tl_up_kata_dir);

            float_t pos_middle_dist;
            vec3_length(tl_up_kata_dir, pos_middle_dist);
            if (pos_middle_dist == 0.0f) {
                mat = oh->node[a2m.j_kata_r_wj].model_transform.mat;
                data[0] = { 0.0f, 0.3f, 0.0f };
                mat4_mult_vec3(&mat, &data[0], &tl_up_kata_dir);
            }
            else
                vec3_mult_scalar(tl_up_kata_dir, 0.3f / pos_middle_dist, tl_up_kata_dir);

            vec3 tl_up_kata_pos;
            vec3_add(tl_up_kata_dir, pos_j_ude_r_wj, tl_up_kata_pos);

            mat = oh->node[a2m.j_mune_b_wj].model_transform.mat;
            mat4_mult_vec3_inv_trans(&mat, &tl_up_kata_pos, &data[0]);
            set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
                MOTION_BONE_TL_UP_KATA_R, key_set, data);
        }

        mat4_get_translation(&oh->node[a2m.j_te_r_wj].model_transform.mat, &data[0]);
        data[1].x = -(float_t)(M_PI / 2.0);
        data[1].y = (float_t)(M_PI / 2.0);
        data[1].z = (float_t)(M_PI / 2.0);
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_C_KATA_R, key_set, data, 2);

        data[0] = oh->node[a2m.j_te_r_wj].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_TE_R_WJ, key_set, data);

        mat4_get_translation(&oh->node[a2m.j_asi_l_wj].model_transform.mat, &data[0]);
        data[1] = vec3_null;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_CL_MOMO_L, key_set, data, 2);

        mat4_get_translation(&oh->node[a2m.j_asi_r_wj].model_transform.mat, &data[0]);
        data[1] = vec3_null;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_CL_MOMO_R, key_set, data, 2);

        data[0] = { 0.0491406508f, 0.0f, 0.0f };
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_AGO_WJ, key_set, data);

        data[0] = { 0.0f, 0.0331281610f, 0.0f };
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_N_KUBI_WJ_EX, key_set, data);
    }
}*/

static void x_pv_game_reset_field(x_pv_game* xpvgm) {
    task_stage_modern_info v14;
    task_stage_modern_set_stage(&v14);
    Glitter::glt_particle_manager->FreeScenes();
}

static void pv_game_dsc_data_find_playdata_item_anim(x_pv_game* xpvgm, int32_t chara_id) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return;

    x_pv_play_data* playdata = &xpvgm->playdata[chara_id]; 
    playdata->motion_data.set_item.clear();

    int32_t pv_branch_mode = 0;
    int32_t time = -1;
    int32_t prev_time = -1;

    x_pv_game_dsc_data& dsc_data = xpvgm->pv_data[xpvgm->pv_index].dsc_data;
    ::dsc_data* i = dsc_data.dsc.data.data();
    ::dsc_data* i_end = dsc_data.dsc.data.data() + dsc_data.dsc.data.size();
    while (true) {
        i = dsc_data.find(DSC_X_ITEM_ANIM, &time, &pv_branch_mode, i, i_end);
        if (!i)
            break;

        if (time < 0) {
            time = prev_time;
            if (prev_time < 0)
                break;
        }

        uint32_t* data = dsc_data.dsc.get_func_data(i);
        if (chara_id == data[0] && data[2] == 2) {
            x_dsc_set_item item;
            item.item_index = data[1];
            item.time = time;
            item.pv_branch_mode = pv_branch_mode;
            playdata->motion_data.set_item.push_back(item);
        }
        prev_time = time;
        i++;
    }
}

static void pv_game_dsc_data_find_playdata_set_motion(x_pv_game* xpvgm, int32_t chara_id) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return;

    x_pv_play_data* playdata = &xpvgm->playdata[chara_id];
    playdata->motion_data.set_motion.clear();

    int32_t pv_branch_mode = 0;
    int32_t time = -1;
    int32_t prev_time = -1;

    x_pv_game_dsc_data& dsc_data = xpvgm->pv_data[xpvgm->pv_index].dsc_data;
    ::dsc_data* i = dsc_data.dsc.data.data();
    ::dsc_data* i_end = dsc_data.dsc.data.data() + dsc_data.dsc.data.size();
    while (true) {
        i = dsc_data.find(DSC_X_SET_MOTION, &time, &pv_branch_mode, i, i_end);
        if (!i)
            break;

        if (time < 0) {
            time = prev_time;
            if (prev_time < 0)
                break;
        }

        uint32_t* data = dsc_data.dsc.get_func_data(i);
        if (chara_id == data[0]) {
            x_dsc_set_motion motion;
            motion.motion_index = data[1];
            motion.time = time;
            motion.pv_branch_mode = pv_branch_mode;
            playdata->motion_data.set_motion.push_back(motion);
        }
        prev_time = time;
        i++;
    }
}

static void pv_game_dsc_data_find_set_motion(x_pv_game* xpvgm) {
    for (std::vector<pv_data_set_motion>& i : xpvgm->set_motion)
        i.clear();

    std::vector<x_dsc_set_motion> v98[6];

    int32_t pv_branch_mode = 0;
    int32_t time = -1;
    int32_t prev_time = -1;

    x_pv_game_dsc_data& dsc_data = xpvgm->pv_data[xpvgm->pv_index].dsc_data;
    ::dsc_data* i = dsc_data.dsc.data.data();
    ::dsc_data* i_end = dsc_data.dsc.data.data() + dsc_data.dsc.data.size();
    while (true) {
        i = dsc_data.find(DSC_X_SET_PLAYDATA, &time, &pv_branch_mode, i, i_end);
        if (!i)
            break;

        if (time < 0)
            time = prev_time;

        uint32_t* data = dsc_data.dsc.get_func_data(i);

        int32_t chara_id = (int32_t)data[0];
        int32_t motion_index = (int32_t)data[1];

        v98[chara_id].push_back({ time, motion_index, pv_branch_mode });

        prev_time = time;
        i++;
    }

    pv_branch_mode = 0;
    time = -1;
    prev_time = -1;

    i = dsc_data.dsc.data.data();
    i_end = dsc_data.dsc.data.data() + dsc_data.dsc.data.size();
    while (true) {
        i = dsc_data.find(DSC_X_CHANGE_FIELD, &time, &pv_branch_mode, i, i_end);
        if (!i)
            break;

        if (time < 0)
            time = prev_time;

        prev_time = time;
        i++;
    }

    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    pvpp* play_param = xpvgm->pv_data[xpvgm->pv_index].play_param;

    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
        x_pv_play_data& playdata = xpvgm->playdata[i];
        std::vector<pv_data_set_motion>& set_motion = xpvgm->set_motion[i];

        for (x_dsc_set_motion& j : playdata.motion_data.set_motion) {
            int32_t time = -1;
            int32_t v56 = 0;

            x_dsc_set_motion* i_begin = v98[i].data() + v98[i].size();
            x_dsc_set_motion* i_end = v98[i].data();
            for (x_dsc_set_motion* i = i_begin; i != i_end; ) {
                i--;

                if (i->motion_index == j.motion_index && i->time <= j.time) {
                    time = i->time;
                    break;
                }
            }

            if (time < 0)
                time = j.time;

            float_t frame = dsc_time_to_frame(((int64_t)j.time - time) * 10000);

            float_t v61 = frame;
            if (frame >= 0.0f)
                v61 = frame + 0.5f;
            else
                v61 = frame - 0.5f;

            int32_t frame_int = (int32_t)v61;
            if (v61 != -0.0f && (float_t)frame_int != v61)
                v61 = (float_t)(v61 < 0.0f ? frame_int - 1 : frame_int);

            int32_t motion_id = -1;
            if (i >= 0 && i < play_param->chara.size()) {
                pvpp_chara& chara = play_param->chara[i];
                if (j.motion_index > 0 && j.motion_index <= chara.motion.size()) {
                    string_hash& motion = chara.motion[(size_t)j.motion_index - 1];
                    motion_id = aft_mot_db->get_motion_id(motion.c_str());
                }
            }

            if (motion_id != -1)
                set_motion.push_back({ motion_id, { v61, -1 } });
        }
    }
}
#endif
