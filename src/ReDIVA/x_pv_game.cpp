/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#if defined(ReDIVA_DEV)
#include "x_pv_game.hpp"
#include "../CRE/light_param/light.hpp"
#include "../CRE/light_param.hpp"
#include "../CRE/rob/rob.hpp"
#include "../CRE/rob/motion.hpp"
#include "../CRE/rob/skin_param.hpp"
#include "../CRE/data.hpp"
#include "../CRE/effect.hpp"
#include "../CRE/item_table.hpp"
#include "../CRE/object.hpp"
#include "../CRE/pv_expression.hpp"
#include "../CRE/pv_param.hpp"
#include "../CRE/random.hpp"
#include "../CRE/shader_ft.hpp"
#include "../CRE/sound.hpp"
#include "../CRE/sprite.hpp"
#include "../KKdLib/io/file_stream.hpp"
#include "../KKdLib/prj/algorithm.hpp"
#include "../KKdLib/farc.hpp"
#include "../KKdLib/interpolation.hpp"
#include "../KKdLib/sort.hpp"
#include "../KKdLib/str_utils.hpp"
#include <meshoptimizer/meshoptimizer.h>
#if BAKE_PNG
#include <lodepng/lodepng.h>
#endif
#if BAKE_VIDEO
#include <glad/glad_wgl.h>
#include <d3d11.h>
#include "../CRE/shader_dev.hpp"
#include "nvenc/nvenc_encoder.hpp"
#endif
#if BAKE_PV826
#include "../KKdLib/waitable_timer.hpp"
#endif
#include "information/dw_console.hpp"
#include "imgui_helper.hpp"
#include "input.hpp"
#include "print_work.hpp"

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
    DSC_X_MAX,
};

#if BAKE_DOF
struct dof_cam {
    std::vector<float_t> position_x;
    std::vector<float_t> position_y;
    std::vector<float_t> position_z;
    std::vector<float_t> focus;
    std::vector<float_t> focus_range;
    std::vector<float_t> fuzzing_range;
    std::vector<float_t> ratio;
    prj::vector_pair<int32_t, bool> enable_frame;
    int32_t frame;
    bool enable;

    dof_cam();
    ~dof_cam();

    void reset();
};
#endif

static int32_t aet_index_table[] = { 0, 1, 2, 6, 5 };

#if BAKE_DOF
dof_cam dof_cam_data;
#endif
x_pv_game* x_pv_game_ptr;
#if BAKE_X_PACK
XPVGameBaker* x_pv_game_baker_ptr;
#else
XPVGameSelector* x_pv_game_selector_ptr;
#endif

#if !BAKE_FAST
x_pv_game_music* x_pv_game_music_ptr;
#endif

extern render_context* rctx_ptr;

static vec4 bright_scale_get(int32_t index, float_t value);

static float_t dsc_time_to_frame(int64_t time);
static vec3 x_pv_game_dof_callback(void* data, int32_t chara_id);
static void x_pv_game_item_alpha_callback(void* data, int32_t chara_id, int32_t type, float_t alpha);
static void x_pv_game_change_field(x_pv_game* xpvgm, int32_t field, int64_t dsc_time, int64_t curr_time);
#if BAKE_PV826
static void x_pv_game_map_auth_3d_to_mot(x_pv_game* xpvgm, bool add_keys);
#endif
static void x_pv_game_reset_field(x_pv_game* xpvgm);

#if BAKE_X_PACK
static void x_pv_game_update_object_set(ObjsetInfo* info);
static bool x_pv_game_write_auth_3d(farc* f, auth_3d* auth,
    auth_3d_database_file* auth_3d_db, const char* category);
static void x_pv_game_write_dsc(const dsc& d, int32_t pv_id);
static void x_pv_game_write_glitter(Glitter::EffectGroup* eff_group, const auth_3d_database* x_pack_auth_3d_db,
    const object_database* obj_db, const object_database* x_pack_obj_db);
static void x_pv_game_write_object_set(ObjsetInfo* info,
    object_database_file* x_pack_obj_db, const texture_database* tex_db,
    const texture_database* x_pack_tex_db_base, texture_database_file* x_pack_tex_db);
static void x_pv_game_write_play_param(pvpp* play_param,
    int32_t pv_id, const auth_3d_database* x_pack_auth_3d_db);
static void x_pv_game_write_stage_resource(pvsr* stage_resource,
    int32_t stage_id, const auth_3d_database* x_pack_auth_3d_db);
#endif

static void print_dsc_command(dsc& dsc, dsc_data* dsc_data_ptr, int64_t time);
static void print_dsc_command(dsc& dsc, dsc_data* dsc_data_ptr, int64_t* time);

#if BAKE_FAST
static void replace_pv832(char* str);
static void replace_pv832(std::string& str);
#endif

#if BAKE_X_PACK
static bool get_replace_auth_name(char* name, size_t name_size, auth_3d* auth);
static void replace_names(char* str);
static void replace_names(std::string& str);
static object_info replace_object_info(object_info info,
    const object_database* src_obj_db, const object_database* dst_obj_db);
static uint32_t replace_texture_id(uint32_t id,
    const texture_database* src_tex_db, const texture_database* dst_tex_db);
#endif

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

float_t x_pv_bar_beat::get_next_bar_time(float_t time) {
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

    if (d == data.data() + data.size()) {
        float_t v11 = 0.0f;
        float_t v12 = divisor;
        if (data.size())
            v11 = data.back().bar_time;

        float_t bar_time = (float_t)((int32_t)((time - v11) / v12) + 1) * v12 + v11;
        if (bar_time < time + 0.00001f)
            bar_time += v12;
        return bar_time;
    }
    else
        return d->bar_time;
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
    curr_bar_time = 0.0f;
    next_bar_time = get_next_bar_time(0.0f);
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

    while (curr_time >= next_bar_time) {
        curr_bar_time = next_bar_time;
        bar++;
        counter++;
        next_bar_time = get_next_bar_time(next_bar_time);
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

float_t BPMFrameRateControl::get_delta_frame() {
    if (bar_beat)
        return bar_beat->get_delta_frame();
    return 0.0f;
}

void BPMFrameRateControl::reset() {
    bar_beat = 0;
    set_frame_speed(1.0f);
}

XPVFrameRateControl::XPVFrameRateControl() {

}

XPVFrameRateControl::~XPVFrameRateControl() {

}

float_t XPVFrameRateControl::get_delta_frame() {
    return ::get_delta_frame() * frame_speed;
}

void XPVFrameRateControl::reset() {
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
    start_pos = 0.0f;
    end_pos = 0.0f;
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

x_pv_play_data_motion* x_pv_play_data::get_motion(int32_t motion_index) {
    for (x_pv_play_data_motion& i : motion)
        if (i.motion_index == motion_index)
            return &i;
    return 0;
}

void x_pv_play_data::reset() {
    if (rob_chr) {
        data_struct* aft_data = &data_list[DATA_AFT];
        bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        int32_t mottbl_index = hand_anim_id_to_mottbl_index(2);

        rob_chr->reset_data(&rob_chr->pv_data, aft_bone_data, aft_mot_db);
        rob_chr->set_hand_l_mottbl_motion(0, mottbl_index, 1.0f, 0, 0.0f, 0.0f, 1.0f, -1, 0.0f, aft_mot_db);
        rob_chr->set_hand_r_mottbl_motion(0, mottbl_index, 1.0f, 0, 0.0f, 0.0f, 1.0f, -1, 0.0f, aft_mot_db);
        rob_chr->set_visibility(false);
    }

    motion.clear();
    motion.shrink_to_fit();
    rob_chr = 0;
    set_motion.clear();
    disp = true;
    motion_data.reset();
}

#if BAKE_PV826
x_pv_game_a3da_to_mot_keys::x_pv_game_a3da_to_mot_keys() {

}

x_pv_game_a3da_to_mot_keys::~x_pv_game_a3da_to_mot_keys() {

}

x_pv_game_a3da_to_mot::x_pv_game_a3da_to_mot(auth_3d_id id) {
    this->id = id;
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
}
#endif

x_pv_game_song_effect_auth_3d::x_pv_game_song_effect_auth_3d() {

}

x_pv_game_song_effect_auth_3d::x_pv_game_song_effect_auth_3d(auth_3d_id id) {
    this->id = id;
}

void x_pv_game_song_effect_auth_3d::reset() {
    id = {};
}

x_pv_game_song_effect_glitter::x_pv_game_song_effect_glitter() : scene_counter(), field_8(), field_9() {
    reset();
}

x_pv_game_song_effect_glitter::~x_pv_game_song_effect_glitter() {

}

void x_pv_game_song_effect_glitter::reset() {
    name.clear();
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
        if (!auth_3d_data_check_category_loaded(category.hash_murmurhash))
            break;

        auth_3d_id id = auth_3d_id(file.hash_murmurhash, &data_list[DATA_X], 0, 0);
        if (id.check_not_empty()) {
            id.read_file_modern();
            id.set_enable(false);
            id.set_repeat(false);
            id.set_paused(false);
            id.set_frame_rate(frame_rate_control);
        }

        this->id = id;
        rctx_ptr->camera->reset();
        state = 11;
    } break;
    case 11: {
        if (id.check_not_empty() && id.check_loaded())
            state = 20;
    } break;
    case 20: {
        auth_3d_id& id = this->id;
        id.set_enable(true);
        id.set_repeat(false);
        id.set_paused(false);
        id.set_camera_root_update(true);
        id.set_req_frame(curr_time * 60.0f);
    } break;
    }
}

void x_pv_game_camera::load(int32_t pv_id, int32_t stage_id, FrameRateControl* frame_rate_control) {
    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "CAMPV%03d", pv_id);
    category.assign(buf);

    if ((pv_id % 100) >= 25 && (pv_id % 100) <= 30 && stage_id >= 25 && stage_id <= 30)
        sprintf_s(buf, sizeof(buf), "CAMPV%03d_100", pv_id);
    else
        sprintf_s(buf, sizeof(buf), "CAMPV%03d_BASE", pv_id);
    file.assign(buf);

    this->frame_rate_control = frame_rate_control;
}

void x_pv_game_camera::load_data() {
    if (state || !category.str.size())
        return;

    auth_3d_data_load_category(&data_list[DATA_X], category.c_str(), category.hash_murmurhash);
    state = 10;
}

void x_pv_game_camera::reset() {
    state = 0;
    category.clear();
    file.clear();
    id = {};
}

void x_pv_game_camera::stop() {
    if (id.check_not_empty())
        id.set_enable(false);
}

void x_pv_game_camera::unload() {
    unload_data();
}

void x_pv_game_camera::unload_data() {
    if (state != 20)
        return;

    id.unload(rctx_ptr);
    auth_3d_data_unload_category(category.hash_murmurhash);
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
            if (!auth_3d_data_check_category_loaded(i.hash_murmurhash))
                wait_load |= true;

        for (string_hash& i : pv_obj_set)
            if (object_storage_get_objset_info(i.hash_murmurhash)
                && object_storage_load_obj_set_check_not_read(i.hash_murmurhash, obj_db, tex_db))
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
                auth_3d_id id = auth_3d_id(j.hash_murmurhash, x_data, obj_db, tex_db);
                if (!id.check_not_empty())
                    continue;

                id.read_file_modern();
                id.set_enable(false);
                id.set_repeat(false);
                id.set_paused(false);
                id.set_visibility(false);
                id.set_frame_rate(frame_rate_control);
                song_effect.auth_3d.push_back(id);
            }
        }
        state = 11;
    } break;
    case 11: {
        bool wait_load = false;

        for (x_pv_game_song_effect& i : song_effect)
            for (x_pv_game_song_effect_auth_3d& j : i.auth_3d)
                if (j.id.check_not_empty() && !j.id.check_loaded())
                    wait_load |= true;

        for (string_hash& i : pv_glitter)
            if (!Glitter::glt_particle_manager->CheckNoFileReaders(i.hash_murmurhash))
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
                v132.name.assign(j.name);
                v132.scene_counter = 0;
                v132.field_8 = !!(j.unk2 & 0x01);
                for (bool& i : v132.field_9)
                    i = true;

                for (int32_t k = 0; k < chara_count && k < ROB_CHARA_COUNT; k++) {
                    bool v121 = false;
                    if (k < play_param->chara.size()) {
                        pvpp_chara& chara = play_param->chara[k];
                        for (pvpp_glitter& l : chara.glitter)
                            if (j.name.hash_murmurhash == l.name.hash_murmurhash) {
                                v121 = true;
                                break;
                            }
                    }
                    v132.field_9[k] = v121;
                }
                song_effect.glitter.push_back(v132);
            }
        }

        for (string_hash& i : pv_glitter)
            Glitter::glt_particle_manager->SetSceneName(i.hash_murmurhash, i.c_str());

        state = 12;
        break;
    }
    case 12: {
        state = 20;
        break;
    }
    case 20: {
        for (x_pv_game_song_effect& i : song_effect)
            if (i.enable)
                for (x_pv_game_song_effect_auth_3d& j : i.auth_3d)
                    j.id.set_chara_id(i.chara_id);
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

        char buf[0x200];
        size_t len = sprintf_s(buf, sizeof(buf), "A3D_EFFPV%03d", pv_id);
        pv_auth_3d.push_back(std::string(buf, len));

        len = sprintf_s(buf, sizeof(buf), "effpv%03d", 800 + (pv_id % 100));
        pv_obj_set.push_back(std::string(buf, len));
        break;
    }
}

void x_pv_game_effect::load_data(int32_t pv_id, object_database* obj_db, texture_database* tex_db) {
    if (state)
        return;

    data_struct* x_data = &data_list[DATA_X];

    for (string_hash& i : pv_obj_set)
        object_storage_load_set_hash(x_data, i.hash_murmurhash);

    for (string_hash& i : pv_auth_3d)
        auth_3d_data_load_category(x_data, i.c_str(), i.hash_murmurhash);

    size_t effect_count = play_param->effect.size();
    for (size_t i = 0; i < effect_count; i++) {
        pvpp_effect& eff = play_param->effect[i];
        if (!eff.glitter.size())
            continue;

        char buf[0x200];
        size_t len = sprintf_s(buf, sizeof(buf), "eff_pv%03d_main", pv_id);
        uint32_t hash = (uint32_t)Glitter::glt_particle_manager->LoadFile(Glitter::X,
            x_data, buf, 0, -1.0f, true, obj_db, tex_db);
        if (hash != hash_murmurhash_empty)
            pv_glitter.push_back(std::string(buf, len));
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

void x_pv_game_effect::set_chara_id(int32_t index, int32_t chara_id, bool chara_item) {
    if (index < 0 || index >= song_effect.size())
        return;

    x_pv_game_song_effect& song_effect = this->song_effect[index];

    for (x_pv_game_song_effect_auth_3d& i : song_effect.auth_3d) {
        i.id.set_chara_id(chara_id);
        i.id.set_chara_item(chara_item);
    }

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
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT || chara_id >= chara_count)
        chara_id = -1;
    song_effect.chara_id = chara_id;

    for (x_pv_game_song_effect_auth_3d& i : song_effect.auth_3d) {
        i.id.set_enable(true);
        i.id.set_repeat(false);
        i.id.set_req_frame(0.0f);
        i.id.set_visibility(true);
    }

#if !BAKE_VIDEO_ALPHA
    for (x_pv_game_song_effect_glitter& i : song_effect.glitter) {
        i.scene_counter = Glitter::glt_particle_manager->LoadSceneEffect(i.name.hash_murmurhash, i.name.c_str());
        if (!i.scene_counter)
            continue;

        Glitter::glt_particle_manager->SetSceneFrameRate(i.scene_counter, 0);
    }
#endif

    song_effect.enable = true;
    song_effect.time = time;

    set_song_effect_time_inner(index, time, false);

    for (x_pv_game_song_effect_auth_3d& i : song_effect.auth_3d)
        i.id.set_chara_id(chara_id);
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
            if (key[temp = length / 2] > time)
                length = temp;
            else {
                key += temp + 1;
                length -= temp + 1;
            }

        if (key != change_fields->data() + change_fields->size())
            max_frame = (float_t)((double_t)(*key - song_effect.time) * 0.000000001) * 60.0f - 1.0f;
    }

    for (x_pv_game_song_effect_auth_3d& i : song_effect.auth_3d) {
        i.id.set_req_frame(req_frame);
        i.id.set_paused(false);
        i.id.set_max_frame(max_frame);
    }

    if (glitter)
        for (x_pv_game_song_effect_glitter& i : song_effect.glitter) {
            if (!i.scene_counter)
                continue;

            float_t frame = Glitter::glt_particle_manager->GetSceneFrameLifeTime(i.scene_counter, 0);
            Glitter::glt_particle_manager->SetSceneEffectReqFrame(i.scene_counter, req_frame - frame);
        }
}

void x_pv_game_effect::set_time(int64_t time, bool glitter) {
    size_t size = song_effect.size();
    for (size_t i = 0; i < size; i++)
        if (song_effect[i].enable)
            set_song_effect_time_inner((int32_t)i, time, glitter);
}

void x_pv_game_effect::stop() {
    size_t song_effect_count = song_effect.size();
    for (size_t i = 0; i < song_effect_count; i++)
        stop_song_effect((int32_t)i, false);

    for (string_hash& i : pv_glitter)
        Glitter::glt_particle_manager->FreeScene(i.hash_murmurhash);
}

void x_pv_game_effect::stop_song_effect(int32_t index, bool free_glitter) {
    if ((state && state != 20) || index < 0 || index >= song_effect.size() || !song_effect[index].enable)
        return;

    x_pv_game_song_effect& song_effect = this->song_effect[index];

    for (x_pv_game_song_effect_auth_3d& i : song_effect.auth_3d) {
        auth_3d_id& id = i.id;
        id.set_enable(false);
        id.set_req_frame(0.0f);
        id.set_visibility(false);
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
    unload_data();
}

void x_pv_game_effect::unload_data() {
    if (state && state != 20)
        return;

    size_t song_effect_count = song_effect.size();
    for (size_t i = 0; i < song_effect_count; i++)
        stop_song_effect((int32_t)i, false);

    for (string_hash& i : pv_glitter)
        Glitter::glt_particle_manager->FreeScene(i.hash_murmurhash);

    for (x_pv_game_song_effect& i : song_effect)
        for (x_pv_game_song_effect_auth_3d& j : i.auth_3d) {
            j.id.unload(rctx_ptr);
            j.id = {};
        }

    for (string_hash& i : pv_auth_3d)
        auth_3d_data_unload_category(i.hash_murmurhash);

    for (string_hash& i : pv_obj_set)
        object_storage_unload_set(i.hash_murmurhash);

    for (string_hash& i : pv_glitter)
        Glitter::glt_particle_manager->UnloadEffectGroup(i.hash_murmurhash);

    song_effect.clear();
    song_effect.resize(song_effect_count);
    state = 0;
}

x_pv_game_chara_effect_auth_3d::x_pv_game_chara_effect_auth_3d()
    : field_0(), src_chara(), dst_chara(), objhrc_index(), node_mat() {
    id = {};
    time = -1;
    reset();
}

x_pv_game_chara_effect_auth_3d::~x_pv_game_chara_effect_auth_3d() {

}

void x_pv_game_chara_effect_auth_3d::reset() {
    field_0 = false;
    src_chara = CHARA_MAX;
    dst_chara = CHARA_MAX;
    file.clear();
    category.clear();
    object_set.clear();
    id = {};
    time = -1;
    node_name.clear();
    objhrc_index = -1;
    node_mat = 0;
}

x_pv_game_chara_effect::x_pv_game_chara_effect() : state(), change_fields(), frame_rate_control() {
    reset();
}

x_pv_game_chara_effect::~x_pv_game_chara_effect() {

}

#if BAKE_PV826
void x_pv_game_chara_effect::ctrl(object_database* obj_db, texture_database* tex_db,
    int32_t pv_id, std::map<uint32_t, auth_3d_id>* effchrpv_auth_3d_mot_ids) {
#else
void x_pv_game_chara_effect::ctrl(object_database* obj_db, texture_database* tex_db) {
#endif
    switch (state) {
    case 20: {
        bool wait_load = false;

        for (std::vector<x_pv_game_chara_effect_auth_3d>& i : auth_3d)
            for (x_pv_game_chara_effect_auth_3d j : i) {
                if (!j.category.str.size())
                    continue;

                if (!auth_3d_data_check_category_loaded(j.category.hash_murmurhash))
                    wait_load |= true;

                if (object_storage_get_objset_info(j.object_set.hash_murmurhash)
                    && object_storage_load_obj_set_check_not_read(j.object_set.hash_murmurhash, obj_db, tex_db))
                    wait_load |= true;
            }

        if (wait_load)
            break;

        data_struct* x_data = &data_list[DATA_X];

        for (std::vector<x_pv_game_chara_effect_auth_3d>& i : auth_3d)
            for (x_pv_game_chara_effect_auth_3d& j : i) {
                if (!j.category.str.size())
                    continue;

                auth_3d_id id = auth_3d_id(j.file.hash_murmurhash, x_data, obj_db, tex_db);
                if (!id.check_not_empty()) {
                    j.id = {};
                    continue;
                }

                id.read_file_modern();
                id.set_enable(false);
                id.set_repeat(false);
                id.set_paused(true);
                id.set_visibility(false);
                id.set_frame_rate(frame_rate_control);

                if (j.field_0)
                    id.set_src_dst_chara(j.src_chara, j.dst_chara);

                j.id = id;

#if BAKE_PV826
                if (pv_id == 826 && effchrpv_auth_3d_mot_ids)
                    effchrpv_auth_3d_mot_ids->insert({ j.file.hash_murmurhash, id });
#endif
            }

        state = 21;
    } break;
    case 21: {
        bool wait_load = false;

        for (std::vector<x_pv_game_chara_effect_auth_3d>& i : auth_3d)
            for (x_pv_game_chara_effect_auth_3d j : i) {
                if (!j.category.str.size())
                    continue;

                if (j.id.check_not_empty() && !j.id.check_loaded())
                    wait_load |= true;
            }

        if (!wait_load)
            state = 30;
    } break;
    }
}

vec3 x_pv_game_chara_effect::get_node_translation(int32_t chara_id,
    int32_t chara_effect, int32_t objhrc_index, const char* node_name) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return 0.0f;

    rob_chara* rob_chr = rob_chara_array_get(chara_id);
    if (!rob_chr)
        return 0.0f;

    std::vector<x_pv_game_chara_effect_auth_3d>& chr_eff_auth = this->auth_3d[rob_chr->chara_id];
    if (chara_effect < 0 || chara_effect >= chr_eff_auth.size())
        return 0.0f;

    x_pv_game_chara_effect_auth_3d& chr_eff = chr_eff_auth[chara_effect];
    if (chr_eff.id.is_null())
        return 0.0f;

    ::auth_3d* auth = chr_eff.id.get_auth_3d();
    if (!auth || objhrc_index < 0 || objhrc_index >= auth->object_hrc.size())
        return 0.0f;

    if (!chr_eff.node_mat || chr_eff.objhrc_index != objhrc_index
        || chr_eff.node_name.compare(node_name)) {
        mat4* mat = 0;
        uint32_t node_name_hash = hash_utf8_murmurhash(node_name);
        for (auth_3d_object_node& i : auth->object_hrc[objhrc_index].node)
            if (hash_string_murmurhash(i.name) == node_name_hash) {
                mat = i.mat;
                break;
            }

        if (!mat)
            return 0.0f;

        chr_eff.node_mat = mat;
        chr_eff.node_name.assign(node_name);
        chr_eff.objhrc_index = objhrc_index;
    }

    vec3 trans;
    mat4_get_translation(chr_eff.node_mat, &trans);
    return trans;
}

#if BAKE_PV826
void x_pv_game_chara_effect::load(int32_t pv_id, pvpp* play_param,
    FrameRateControl* frame_rate_control, chara_index charas[6],
    std::unordered_map<std::string, string_hash>* effchrpv_auth_3d_mot_names) {
#else
void x_pv_game_chara_effect::load(int32_t pv_id, pvpp* play_param,
    FrameRateControl* frame_rate_control, chara_index charas[6]) {
#endif
    if (state)
        return;

    this->play_param = play_param;
    this->frame_rate_control = frame_rate_control;

    size_t chara_count = play_param->chara.size();
    for (int32_t i = 0; i < chara_count && i < ROB_CHARA_COUNT; i++) {
        pvpp_chara& chara = play_param->chara[i];
        if (!chara.chara_effect_init)
            continue;

        pvpp_chara_effect& chara_effect = chara.chara_effect;

        chara_index src_chara = (chara_index)chara_effect.base_chara;
        chara_index dst_chara = charas[i];
        chara_index dst_chara_mei = dst_chara == CHARA_SAKINE ? CHARA_MEIKO : dst_chara;

        /*if (dst_chara == CHARA_EXTRA)
            dst_chara = src_chara;*/

        const char* src_chara_str = chara_index_get_auth_3d_name(src_chara);
        const char* dst_chara_str = chara_index_get_auth_3d_name(dst_chara);
        const char* dst_chara_mei_str = chara_index_get_auth_3d_name(dst_chara_mei);

#if BAKE_X_PACK
        if (pv_id == 814) {
        repeat:
            dst_chara_mei = dst_chara == CHARA_SAKINE ? CHARA_MEIKO : dst_chara;
            dst_chara_str = chara_index_get_auth_3d_name(dst_chara);
            dst_chara_mei_str = chara_index_get_auth_3d_name(dst_chara_mei);
        }
#endif
        for (pvpp_chara_effect_auth_3d& j : chara_effect.auth_3d) {
            std::string file(j.auth_3d.str);
            std::string object_set;
#if BAKE_PV826
            if (pv_id != 826) {
#endif
                if (j.has_object_set)
                    object_set.assign(j.object_set.str);
                else
                    object_set.assign(file);

                if (src_chara != dst_chara) {
                    if (src_chara_str) {
                        size_t pos = object_set.find(src_chara_str);
                        if (pos != -1)
                            object_set.replace(pos, 3, dst_chara_str);
                        else if (dst_chara_mei_str) {
                            size_t pos = object_set.find(dst_chara_mei_str);
                            if (pos != -1)
                                object_set.replace(pos, 3, src_chara_str);
                        }
                    }

                    if (!j.u00)
                        if (src_chara_str) {
                            size_t pos = file.find(src_chara_str);
                            if (pos != -1)
                                file.replace(pos, 3, dst_chara_str);
                            else if (dst_chara_mei_str) {
                                size_t pos = file.find(dst_chara_mei_str);
                                if (pos != -1)
                                    file.replace(pos, 3, src_chara_str);
                            }
                        }
                }
#if BAKE_PV826
            }
            else {
                if (!j.auth_3d.str.find("EFFCHRPV826"))
                    continue;

                file.assign("EFFCHRPV826MIK001");
                object_set.assign("EFFCHRPV826MIK001");
            }
#endif

            std::string category;
            if (!file.find("EFFCHRPV")) {
                size_t pos = file.find("_");
                if (pos != -1)
                    category.assign(file.substr(0, pos));
                else
                    category.assign(file);
                category.insert(0, "A3D_");
            }
            else
                category.assign(file);

            x_pv_game_chara_effect_auth_3d auth_3d;
            auth_3d.field_0 = j.u00;
            auth_3d.src_chara = src_chara;
            auth_3d.dst_chara = dst_chara;
            auth_3d.file = file;
            auth_3d.category = category;
            auth_3d.object_set = object_set;
            this->auth_3d[i].push_back(auth_3d);

#if BAKE_PV826
            if (pv_id == 826 && effchrpv_auth_3d_mot_names)
                effchrpv_auth_3d_mot_names->insert({ file, category });
#endif
        }
#if BAKE_X_PACK
        if (pv_id == 814) {
            dst_chara = (chara_index)((int32_t)(dst_chara + 1));
            if (dst_chara != CHARA_MAX)
                goto repeat;
        }
#endif
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

            auth_3d_data_load_category(x_data, j.category.c_str(), j.category.hash_murmurhash);
            object_storage_load_set_hash(x_data, j.object_set.hash_murmurhash);
        }

    state = 20;
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

    auth_3d_id& id = auth_3d[index].id;

    if (!id.check_not_empty())
        return;

    if (id.get_enable())
        id.set_req_frame((float_t)((double_t)(time - auth_3d[index].time) * 0.000000001) * 60.0f);
    else {
        id.set_enable(true);
        id.set_paused(false);
        id.set_repeat(false);
        id.set_req_frame(0.0f);
        id.set_visibility(true);
        auth_3d[index].time = time;
    }

    float_t max_frame = -1.0f;
    if (change_fields) {
        int64_t* key = change_fields->data();
        size_t length = change_fields->size();
        size_t temp;
        while (length > 0)
            if (key[temp = length / 2] > time)
                length = temp;
            else {
                key += temp + 1;
                length -= temp + 1;
            }

        if (key != change_fields->data() + change_fields->size())
            max_frame = (float_t)((double_t)(*key - auth_3d[index].time) * 0.000000001) * 60.0f - 1.0f;
    }

    id.set_max_frame(max_frame);
}

void x_pv_game_chara_effect::set_chara_effect_time(int32_t chara_id, int32_t index, int64_t time) {
    if (!state || chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return;

    for (x_pv_game_chara_effect_auth_3d& i : auth_3d[chara_id]) {
        if (!i.id.check_not_empty() || !i.id.get_enable())
            continue;

        i.id.set_req_frame((float_t)((double_t)(time - i.time) * 0.000000001) * 60.0f);
        i.id.set_paused(false);
    }
}

void x_pv_game_chara_effect::set_time(int64_t time) {
    if (!state)
        return;

    for (auto& i : auth_3d)
        for (x_pv_game_chara_effect_auth_3d& j : i) {
            if (!j.id.check_not_empty() || !j.id.get_enable())
                continue;

            j.id.set_req_frame((float_t)((double_t)(time - j.time) * 0.000000001) * 60.0f);
            j.id.set_paused(false);
        }
}

void x_pv_game_chara_effect::stop() {
    if (!state)
        return;

    for (std::vector<x_pv_game_chara_effect_auth_3d>& i : auth_3d)
        for (x_pv_game_chara_effect_auth_3d& j : i)
            if (j.id.check_not_empty())
                j.id.set_enable(false);
}

void x_pv_game_chara_effect::stop_chara_effect(int32_t chara_id, int32_t index) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return;

    for (x_pv_game_chara_effect_auth_3d& i : auth_3d[chara_id])
        if (i.id.check_not_empty())
            i.id.set_enable(false);
}

void x_pv_game_chara_effect::unload() {
    if (!state)
        return;

    unload_data();

    for (std::vector<x_pv_game_chara_effect_auth_3d>& i : auth_3d) {
        i.clear();
        i.shrink_to_fit();
    }

    frame_rate_control = 0;
    play_param = 0;
    state = 0;
}

void x_pv_game_chara_effect::unload_data() {
    if (!state)
        return;

    if (state == 10 || state == 30) {
        for (std::vector<x_pv_game_chara_effect_auth_3d>& i : auth_3d)
            for (x_pv_game_chara_effect_auth_3d& j : i) {
                if (!j.category.str.size())
                    continue;

                j.id.unload(rctx_ptr);
                auth_3d_data_unload_category(j.category.hash_murmurhash);
                object_storage_unload_set(j.object_set.hash_murmurhash);
            }

        state = 10;
    }
}

x_pv_game_music_args::x_pv_game_music_args() : type(), start(), field_2C() {

}

x_pv_game_music_args::~x_pv_game_music_args() {

}

x_pv_game_music_ogg::x_pv_game_music_ogg() {
    playback = ogg_playback_data_get(0);
}

x_pv_game_music_ogg::~x_pv_game_music_ogg() {
    if (playback) {
        playback->Reset();
        playback = 0;
    }
}

#if !BAKE_FAST
x_pv_game_music::x_pv_game_music() : flags(), pause(), channel_pair_volume(), fade_in(),
fade_out(), no_fade(), no_fade_remain(), fade_out_time_req(), fade_out_action_req(),
type(), start(), end(), play_on_end(), fade_in_time(), field_9C(), loaded(), ogg() {
    volume = 100;
    master_volume = 100;
    for (int32_t& i : channel_pair_volume)
        i = 100;
    fade_out_time = 3.0f;
}

x_pv_game_music::~x_pv_game_music() {

}

bool x_pv_game_music::check_args(int32_t type, std::string&& file_path, float_t start) {
    return args.type == type && !args.file_path.compare(file_path) && args.start == start;
}

void x_pv_game_music::ctrl(float_t delta_time) {
    if (no_fade && !pause) {
        no_fade_remain -= delta_time;
        if (no_fade_remain <= 0.0f) {
            fade_out.enable = true;
            fade_out.start = get_volume(0);
            fade_out.value = 0;
            fade_out.time = fade_out_time_req;
            fade_out.remain = fade_out_time_req;
            fade_out.action = fade_out_action_req;

            no_fade = false;
            no_fade_remain = 0.0f;
            fade_out_time_req = 0.0f;
            fade_out_action_req = X_PV_GAME_MUSIC_ACTION_NONE;
        }
    }

    if (fade_in.enable && !pause) {
        fade_in.remain -= delta_time;
        if (fade_in.remain > 0.0f) {
            int32_t value = fade_in.value;
            if (fade_in.time > 0.0)
                value = (int32_t)((float_t)value - (float_t)(value
                    - fade_in.start) * (fade_in.remain / fade_in.time));
            set_volume_map(0, value);
        }
        else
            fade_in_end();
    }

    if (fade_out.enable && !pause) {
        fade_out.remain -= delta_time;
        if (fade_out.remain > 0.0f) {
            int32_t value = fade_out.value;
            if (fade_out.time > 0.0f)
                value = (int32_t)(value - ((float_t)(value
                    - fade_out.start) * (fade_out.remain / fade_out.time)));
            set_volume_map(0, value);
        }
        else
            fade_out_end();
    }

    if (flags & X_PV_GAME_MUSIC_OGG) {
        OggPlayback* playback = ogg->playback;
        if (playback->GetFileState() == OGG_FILE_HANDLER_FILE_STATE_STOPPED)
            playback->Stop();
        if (playback->GetFileState() == OGG_FILE_HANDLER_FILE_STATE_STOPPED)
            play_or_stop();
        OggPlayback::SetChannelPairVolumePan(playback);
    }
}

void x_pv_game_music::exclude_flags(x_pv_game_music_flags flags) {
    if ((this->flags & X_PV_GAME_MUSIC_OGG) && (flags & X_PV_GAME_MUSIC_OGG))
        ogg_free();
    enum_and(this->flags, ~flags);
}

void x_pv_game_music::fade_in_end() {
    if (!fade_in.enable)
        return;

    fade_in.enable = false;
    fade_in.remain = 0.0f;
    set_volume_map(0, fade_in.value);

    switch (fade_in.action) {
    case X_PV_GAME_MUSIC_ACTION_STOP:
        stop();
        break;
    case X_PV_GAME_MUSIC_ACTION_PAUSE:
        set_pause(1);
        break;
    case X_PV_GAME_MUSIC_ACTION_PLAY:
        play();
        break;
    }
}

void x_pv_game_music::fade_out_end() {
    if (!fade_out.enable)
        return;

    fade_out.enable = false;
    fade_out.remain = 0.0f;
    set_volume_map(0, fade_out.value);

    switch (fade_out.action) {
    case X_PV_GAME_MUSIC_ACTION_STOP:
        stop();
        break;
    case X_PV_GAME_MUSIC_ACTION_PAUSE:
        set_pause(1);
        break;
    case X_PV_GAME_MUSIC_ACTION_PLAY:
        play();
        break;
    }
}

void x_pv_game_music::file_load(int32_t type, std::string&& file_path, bool play_on_end,
    float_t start, float_t end, float_t fade_in_time, float_t fade_out_time, bool a9) {
    this->type = type;
    this->file_path.assign(file_path);
    this->start = start;
    this->end = end;
    this->play_on_end = play_on_end;
    this->fade_out_time = fade_out_time;
    this->fade_in_time = fade_in_time;
    this->field_9C = a9;

    if (fade_in_time > 0.0f) {
        fade_in.start = 0;
        fade_in.value = 100;
        fade_in.time = fade_in_time;
        fade_in.remain = fade_in_time;
        fade_in.action = X_PV_GAME_MUSIC_ACTION_NONE;
        fade_in.enable = true;
    }

    if (end <= 0.0f) {
        fade_out.start = get_volume(0);
        fade_out.value = 0;
        fade_out.time = 0.0f;
        fade_out.remain = 0.0;
        fade_out.action = play_on_end
            ? X_PV_GAME_MUSIC_ACTION_PLAY : X_PV_GAME_MUSIC_ACTION_STOP;
    }
    else {
        no_fade = true;
        no_fade_remain = end + fade_in_time;
        fade_out_time_req = fade_out_time;
        fade_out_action_req = play_on_end
            ? X_PV_GAME_MUSIC_ACTION_PLAY : X_PV_GAME_MUSIC_ACTION_STOP;
    }
}

int32_t x_pv_game_music::get_master_volume(int32_t index) {
    if (!index)
        return master_volume;
    return 0;
}

int32_t x_pv_game_music::get_volume(int32_t index) {
    if (!index)
        return volume;
    return 0;
}

int32_t x_pv_game_music::include_flags(x_pv_game_music_flags flags) {
    if (!((this->flags & X_PV_GAME_MUSIC_OGG) || !(flags & X_PV_GAME_MUSIC_OGG)))
        ogg_init();
    enum_or(this->flags, flags);
    return 0;
}

int32_t x_pv_game_music::load(int32_t type, std::string&& file_path, bool wait_load, float_t time, bool a6) {
    if (type == 4 && set_ogg_args(std::string(file_path), time, wait_load) < 0)
        return -6;

    reset();
    reset_args();
    set_args(type, std::string(file_path), time, a6);
    pause = true;
    return 0;
}

void x_pv_game_music::ogg_free() {
    if (ogg) {
        delete ogg;
        ogg = 0;
    }
}

int32_t x_pv_game_music::ogg_init() {
    if (ogg)
        return 0;

    ogg = new x_pv_game_music_ogg;
    return ogg->playback ? 0 : -1;
}

int32_t x_pv_game_music::ogg_load(std::string&& file_path, float_t start) {
    if (!(flags & X_PV_GAME_MUSIC_OGG))
        return -2;

    start = max_def(start, 0.0f);

    OggPlayback* playback = ogg->playback;

    OggFileHandlerFileState file_state = playback->GetFileState();
    OggFileHandlerPauseState pause_state = playback->GetPauseState();

    if (!check_args(4, std::string(file_path), start)
        || file_state != OGG_FILE_HANDLER_FILE_STATE_PLAYING
        || pause_state != OGG_FILE_HANDLER_PAUSE_STATE_PAUSE) {
        playback->Stop();
        playback->SetLoadTimeSeek(start);
        playback->SetPath(file_path);
    }

    playback->SetPauseState(OGG_FILE_HANDLER_PAUSE_STATE_PLAY);
    ogg->file_path.assign(file_path);
    loaded = true;
    return 0;
}

int32_t x_pv_game_music::ogg_reset() {
    sound_stream_array_reset();
    include_flags(X_PV_GAME_MUSIC_OGG);
    return 0;
}

int32_t x_pv_game_music::ogg_stop() {
    if (!(flags & X_PV_GAME_MUSIC_OGG))
        return -2;

    ogg->playback->Stop();
    ogg->file_path.clear();
    loaded = false;
    return 0;
}

int32_t x_pv_game_music::play() {
    return play(type, std::string(file_path), play_on_end, start, end, fade_in_time, fade_out_time, false);
}

int32_t x_pv_game_music::play(int32_t type, std::string&& file_path, bool play_on_end,
    float_t start, float_t end, float_t fade_in_time, float_t fade_out_time, bool a9) {
    set_volume_map(0, 100);
    if (type == 4 && ogg_load(std::string(file_path), start) < 0)
        return -7;

    reset();
    reset_args();
    file_load(type, std::string(file_path), play_on_end, start, end, fade_in_time, fade_out_time, a9);
    pause = false;
    return 0;
}

void x_pv_game_music::play_fade_in(int32_t type, std::string&& file_path, float_t start,
    float_t end, bool play_on_end, bool a7, float_t fade_out_time, bool a10) {
    play(type, std::string(file_path), play_on_end, start, end, 0.0f, fade_out_time, a10);
}

int32_t x_pv_game_music::play_or_stop() {
    if (play_on_end)
        return play();
    else
        return stop();
}

void x_pv_game_music::reset() {
    fade_in = {};
    fade_out = {};
    no_fade = false;
    no_fade_remain = 0.0f;
    fade_out_time_req = 0.0f;
    fade_out_action_req = X_PV_GAME_MUSIC_ACTION_NONE;
    type = 0;
    file_path.clear();
    start = 0.0f;
    end = 0.0f;
    play_on_end = false;
    fade_in_time = 0.0f;
    fade_out_time = 3.0f;
    field_9C = false;
}

void x_pv_game_music::reset_args() {
    args.type = 0;
    args.file_path.clear();
    args.start = 0.0f;
    args.field_2C = false;
}

void x_pv_game_music::set_args(int32_t type, std::string&& file_path, float_t start, bool a5) {
    args.type = type;
    args.file_path.assign(file_path);
    args.start = start;
    args.field_2C = a5;
}

int32_t x_pv_game_music::set_channel_pair_volume(int32_t channel_pair, int32_t value) {
    if (!(flags & X_PV_GAME_MUSIC_OGG))
        return -2;

    ogg->playback->SetChannelPairVolume(channel_pair, ratio_to_db((float_t)value * 0.01f));
    return 0;
}

int32_t x_pv_game_music::set_channel_pair_volume_map(int32_t channel_pair, int32_t value) {
    if (channel_pair >= 0 && channel_pair <= 3) {
        set_channel_pair_volume(channel_pair, value);
        if (!channel_pair)
            set_channel_pair_volume_map(2, value);
        else if (channel_pair == 1)
            set_channel_pair_volume_map(3, value);
        channel_pair_volume[channel_pair] = value;
        return 0;
    }
    return -1;
}

int32_t x_pv_game_music::set_fade_out(float_t time, bool stop) {
    reset();
    if (time == 0.0f)
        x_pv_game_music::stop();
    else {
        fade_out.start = get_volume(0);
        fade_out.value = 0;
        fade_out.time = time;
        fade_out.remain = time;
        fade_out.enable = true;
        fade_out.action = stop ? X_PV_GAME_MUSIC_ACTION_STOP
            : X_PV_GAME_MUSIC_ACTION_PAUSE;
    }
    return 0;
}

int32_t x_pv_game_music::set_master_volume(int32_t value) {
    if (!(flags & X_PV_GAME_MUSIC_OGG))
        return -2;

    ogg->playback->SetMasterVolume(ratio_to_db(
        (float_t)(value * get_master_volume(0) / 100) * 0.01f));
    return 0;
}

int32_t x_pv_game_music::set_ogg_args(std::string&& file_path, float_t start, bool wait_load) {
    if (!(flags & X_PV_GAME_MUSIC_OGG))
        return -2;

    start = max_def(start, 0.0f);

    OggPlayback* playback = ogg->playback;

    OggFileHandlerFileState file_state = playback->GetFileState();
    OggFileHandlerPauseState pause_state = playback->GetPauseState();

    if (!check_args(4, std::string(file_path), start)
        || file_state != OGG_FILE_HANDLER_FILE_STATE_PLAYING
        || pause_state != OGG_FILE_HANDLER_PAUSE_STATE_PAUSE) {
        playback->Stop();
        playback->SetLoadTimeSeek(start);
        playback->SetPath(file_path);
        playback->SetPauseState(OGG_FILE_HANDLER_PAUSE_STATE_PAUSE);
        ogg->file_path.assign(file_path);

        if (wait_load) {
            OggFileHandlerFileState file_state = playback->GetFileState();
            while (file_state != OGG_FILE_HANDLER_FILE_STATE_PLAYING
                && file_state != OGG_FILE_HANDLER_FILE_STATE_STOPPED
                && file_state != OGG_FILE_HANDLER_FILE_STATE_MAX)
                file_state = playback->GetFileState();
        }
    }
    return 0;
}

int32_t x_pv_game_music::set_ogg_pause_state(uint8_t pause_state) {
    if (!(flags & X_PV_GAME_MUSIC_OGG))
        return -2;

    OggPlayback* playback = ogg->playback;
    if (playback->GetFileState() == OGG_FILE_HANDLER_FILE_STATE_PLAYING)
        playback->SetPauseState((OggFileHandlerPauseState)pause_state);
    return 0;
}

int32_t x_pv_game_music::set_pause(int32_t pause) {
    if (flags & X_PV_GAME_MUSIC_OGG)
        set_ogg_pause_state(pause != 0);
    this->pause = pause == 1;
    return 0;
}

void x_pv_game_music::set_volume_map(int32_t index, int32_t value) {
    if (!index) {
        if (flags & X_PV_GAME_MUSIC_OGG)
            set_master_volume(value);
        volume = value;
    }
}

int32_t x_pv_game_music::stop() {
    if (flags & X_PV_GAME_MUSIC_OGG)
        ogg_stop();
    reset();
    reset_args();
    pause = false;
    return 0;
}

void x_pv_game_music::stop_reset_flags() {
    stop();
    exclude_flags(X_PV_GAME_MUSIC_ALL);
}
#endif

aet_obj_data_comp::aet_obj_data_comp() : aet_id(), layer_name() {
    frame = -1.0f;
}

aet_obj_data_comp::~aet_obj_data_comp() {

}

aet_layout_data* aet_obj_data_comp::find_layout(const char* name) {
    return comp.Find(name);
}

bool aet_obj_data_comp::init_comp(uint32_t aet_id, const std::string&& layer_name,
    float_t frame, const aet_database* aet_db, const sprite_database* spr_db) {
    if (aet_id == hash_murmurhash_empty || aet_id == -1)
        return false;

    if (comp.data.size() && this->aet_id == aet_id && !strncmp(this->layer_name,
        layer_name.c_str(), layer_name.size()) && fabsf(this->frame - frame) <= 0.000001f)
        return true;

    comp.Clear();
    this->aet_id = aet_id;

    size_t len = min_def(sizeof(this->layer_name) - 1, layer_name.size());
    strncpy_s(this->layer_name, sizeof(this->layer_name), layer_name.c_str(), len);
    this->layer_name[len] = 0;
    this->frame = frame;

    aet_manager_init_aet_layout(&comp, aet_id, layer_name.c_str(),
        (AetFlags)0, RESOLUTION_MODE_HD, 0, frame, aet_db, spr_db);
    return true;
}

aet_obj_data::aet_obj_data() : comp(), loop(), hidden(), field_2A(), frame_rate_control() {
    hash = hash_murmurhash_empty;

    reset();
}

aet_obj_data::~aet_obj_data() {
    reset();
}

bool aet_obj_data::check_disp() {
    return aet_manager_get_obj_end(id);
}

aet_layout_data* aet_obj_data::find_aet_layout(const char* name,
    const aet_database* aet_db, const sprite_database* spr_db) {
    if (!name)
        return 0;

    if (!comp)
        comp = new aet_obj_data_comp;

    if (comp && comp->init_comp(hash, layer_name.c_str(), aet_manager_get_obj_frame(id), aet_db, spr_db))
        return comp->find_layout(name);

    return 0;
}

uint32_t aet_obj_data::init(AetArgs& args, const aet_database* aet_db) {
    reset();

    hash = args.id.id;
    layer_name.assign(args.layer_name);
    id = aet_manager_init_aet_object(args, aet_db);
    if (!id)
        return 0;

    aet_manager_set_obj_frame_rate_control(id, frame_rate_control);
#if BAKE_VIDEO_ALPHA
    aet_manager_set_obj_visible(id, false);
#else
    aet_manager_set_obj_visible(id, !hidden);
#endif
    field_2A = true;
    loop = !!(args.flags & AET_LOOP);
    return id;
}

void aet_obj_data::reset() {
    field_2A = false;

    if (id) {
        aet_manager_free_aet_object(id);
        id = 0;
    }

    hash = hash_murmurhash_empty;
    layer_name.clear();
    layer_name.shrink_to_fit();

    if (comp) {
        delete comp;
        comp = 0;
    }
}

x_pv_aet_disp_data::x_pv_aet_disp_data() : disp(), opacity() {

}

x_pv_aet_disp_string::x_pv_aet_disp_string() {

}

x_pv_aet_disp_string::~x_pv_aet_disp_string() {

}

void x_pv_aet_disp_string::ctrl(const char* name, aet_obj_data* aet, const aet_database* aet_db, const sprite_database* spr_db) {
    if (!aet) {
        data.disp = false;
        return;
    }

    aet_layout_data* layout = aet->find_aet_layout(name, aet_db, spr_db);
    if (!layout) {
        data.disp = false;
        return;
    }

    data.disp = true;
    data.rect.pos = *(vec2*)&layout->position;
    data.rect.size.x = layout->height * layout->mat.row1.y * layout->mat.row0.x;
    data.rect.size.y = layout->height * layout->mat.row1.y;
    data.opacity = clamp_def((uint8_t)(int32_t)(layout->opacity * 255.0f), 0x00, 0xFF);
}

static const char* sub_8133DDF8(int32_t id) {
    return 0;
}

const char* x_pv_str_array_pv::get_song_name(int32_t index) {
    if (!index)
        return song_name;

    int32_t id = 20000010 + 1000 * pv_id + 4 * (index - 1);

    const char* str = sub_8133DDF8(id);
    if (str)
        return str;
    return "";
}

const char* x_pv_str_array_pv::get_song_line_1(int32_t index) {
    int32_t id = 20000002 + 1000 * pv_id;

    if (index)
        id = 20000011 + 1000 * pv_id + 4 * (index - 1);

    const char* str = sub_8133DDF8(id);
    if (str)
        return str;
    return "";
}

const char* x_pv_str_array_pv::get_song_line_2(int32_t index) {
    int32_t id = 20000003 + 1000 * pv_id;

    if (index)
        id = 20000012 + 1000 * pv_id + 4 * (index - 1);

    const char* str = sub_8133DDF8(id);
    if (str)
        return str;
    return "";
}

const char* x_pv_str_array_pv::get_song_line_3(int32_t index) {
    int32_t id = 20000004 + 1000 * pv_id;
    if (index)
        id = 20000013 + 1000 * pv_id + 4 * (index - 1);

    const char* str = sub_8133DDF8(id);
    if (str)
        return str;
    return "";
}

x_pv_game_title::x_pv_game_title() : state(), str_array(), txt_layer_index() {
    aet_set_id = hash_murmurhash_empty;
    spr_set_id = hash_murmurhash_empty;
    aet_id = hash_murmurhash_empty;
}

x_pv_game_title::~x_pv_game_title() {
    unload();
}

void x_pv_game_title::ctrl() {
    switch (state) {
    case 2:
        if (!aet_manager_load_file_modern(aet_set_id, &aet_db)
            && !sprite_manager_load_file_modern(spr_set_id, &spr_db)) {
            spr_set* set = sprite_manager_get_set(spr_set_id, &spr_db);
            if (set) {
                SpriteData* sprdata = set->sprdata;
                for (uint32_t i = set->num_of_sprite; i; i--, sprdata++)
                    sprdata->resolution_mode = RESOLUTION_MODE_HD;
            }
            state = 3;
        }
        break;
    case 4:
        if (tit_layer.check_disp() && txt_layer.check_disp())
            state = 3;
        else if (txt_layer.id)
            ctrl_txt_data();
        break;
    }
}

void x_pv_game_title::ctrl_txt_data() {

    const char* comps[4] = {
        "p_txt01_c",
        "p_txt02_c",
        "p_txt03_c",
        "p_txt04_c",
    };

    const char** str = comps;
    for (x_pv_aet_disp_string& i : txt_data)
        i.ctrl(*str++, &txt_layer, &aet_db, &spr_db);
}

void x_pv_game_title::disp() {
    if (state != 4)
        return;

    font_info font(16);

    PrintWork print_work;
    print_work.SetFont(&font);
    print_work.prio = spr::SPR_PRIO_06;

    for (x_pv_aet_disp_string& i : txt_data) {
        if (!i.data.disp || !i.str.size())
            continue;

        font.set_glyph_size(i.data.rect.size.x, i.data.rect.size.y);
        print_work.color.a = i.data.opacity;
        print_work.line_origin_loc = i.data.rect.pos;
        print_work.text_current_loc = print_work.line_origin_loc;

        print_work.printf((app::text_flags)(app::TEXT_FLAG_ALIGN_FLAG_LOCATE_V_CENTER
            | app::TEXT_FLAG_ALIGN_FLAG_H_CENTER
            | app::TEXT_FLAG_FONT), i.str.c_str());
    }
}

bool x_pv_game_title::get_set_ids(int8_t cloud_index, uint32_t& aet_set_id, uint32_t& spr_set_id) {
    switch (cloud_index) {
    case 1:
    default:
        aet_set_id = hash_utf8_murmurhash("AET_PV_TIT01");
        spr_set_id = hash_utf8_murmurhash("SPR_PV_TIT01");
        break;
    case 2:
        aet_set_id = hash_utf8_murmurhash("AET_PV_TIT02");
        spr_set_id = hash_utf8_murmurhash("SPR_PV_TIT02");
        break;
    case 3:
        aet_set_id = hash_utf8_murmurhash("AET_PV_TIT03");
        spr_set_id = hash_utf8_murmurhash("SPR_PV_TIT03");
        break;
    case 4:
        aet_set_id = hash_utf8_murmurhash("AET_PV_TIT04");
        spr_set_id = hash_utf8_murmurhash("SPR_PV_TIT04");
        break;
    case 5:
        aet_set_id = hash_utf8_murmurhash("AET_PV_TIT05");
        spr_set_id = hash_utf8_murmurhash("SPR_PV_TIT05");
        break;
    }
    return 1;
}

void x_pv_game_title::load(int32_t pv_id, FrameRateControl* frame_rate_control) {
    if (state)
        return;

    str_array.pv_id = pv_id;

    int32_t cloud_index = 1;
    switch (pv_id) {
    case 809:
    case 810:
    case 823:
    case 824:
    case 825:
    case 830:
        cloud_index = 1; // Classic
        break;
    case 807:
    case 808:
    case 811:
    case 812:
    case 813:
    case 826:
        cloud_index = 2; // Cute
        break;
    case 801:
    case 803:
    case 805:
    case 814:
    case 819:
    case 827:
        cloud_index = 3; // Cool
        break;
    case 802:
    case 815:
    case 816:
    case 820:
    case 822:
    case 828:
        cloud_index = 4; // Elegant
        break;
    case 804:
    case 806:
    case 817:
    case 818:
    case 821:
    case 829:
        cloud_index = 5; // Chaos
        break;
    case 831:
    case 832:
        cloud_index = 6; // DLC
        break;
    }

    get_set_ids(cloud_index, aet_set_id, spr_set_id);

    switch (cloud_index) {
    case 1:
    default:
        aet_id = hash_utf8_murmurhash("AET_PV_TIT01_MAIN");
        break;
    case 2:
        aet_id = hash_utf8_murmurhash("AET_PV_TIT02_MAIN");
        break;
    case 3:
        aet_id = hash_utf8_murmurhash("AET_PV_TIT03_MAIN");
        break;
    case 4:
        aet_id = hash_utf8_murmurhash("AET_PV_TIT04_MAIN");
        break;
    case 5:
        aet_id = hash_utf8_murmurhash("AET_PV_TIT05_MAIN");
        break;
    }

    tit_layer.frame_rate_control = frame_rate_control;
    aet_manager_set_obj_frame_rate_control(tit_layer.id, frame_rate_control);

    txt_layer.frame_rate_control = frame_rate_control;
    aet_manager_set_obj_frame_rate_control(txt_layer.id, frame_rate_control);

    state = 1;
}

void x_pv_game_title::load_data() {
    if (!state || state != 1)
        return;

    sprite_manager_read_file_modern(spr_set_id, &data_list[DATA_X], &spr_db);
    aet_manager_read_file_modern(aet_set_id, &data_list[DATA_X], &aet_db);
    state = 2;
}

void x_pv_game_title::reset() {
    tit_layer.reset();
    txt_layer.reset();
    txt_data[0].data.disp = false;
    txt_data[1].data.disp = false;
    txt_data[2].data.disp = false;
    txt_data[3].data.disp = false;
    if (state == 4)
        state = 3;
}

void x_pv_game_title::show(int32_t index) {
    if (!state)
        return;

    txt_data[0].str.assign(str_array.get_song_name(index));
    txt_data[1].str.assign(str_array.get_song_line_1(index));
    txt_data[2].str.assign(str_array.get_song_line_2(index));
    txt_data[3].str.assign(str_array.get_song_line_3(index));

    txt_layer_index = 0;

    int32_t txt_layer_index = 3;
    while (!txt_data[txt_layer_index].str.size())
        if (--txt_layer_index < 2)
            break;

    txt_layer_index = max_def(txt_layer_index + 1, 2);
    this->txt_layer_index = txt_layer_index;

    if (state != 3 || aet_id == hash_murmurhash_empty)
        return;

    AetArgs args;
    args.id.id = aet_id;
    args.layer_name = "tit_01";
    args.prio = spr::SPR_PRIO_05;
    args.spr_db = &spr_db;

    tit_layer.init(args, &aet_db);

    switch (txt_layer_index) {
    case 0:
    case 1:
    case 2:
    default:
        args.layer_name = "txt01";
        break;
    case 3:
        args.layer_name = "txt02";
        break;
    case 4:
        args.layer_name = "txt03";
        break;
    }

    txt_layer.init(args, &aet_db);

    ctrl_txt_data();

    state = 4;
}

void x_pv_game_title::unload() {
    if (state >= 3)
        unload_data();

    aet_set_id = hash_murmurhash_empty;
    spr_set_id = hash_murmurhash_empty;
    aet_id = hash_murmurhash_empty;
    state = 0;
}

void x_pv_game_title::unload_data() {
    if (state && state != 2 && state != 1) {
        reset();
        sprite_manager_unload_set_modern(spr_set_id, &spr_db);
        aet_manager_unload_set_modern(aet_set_id, &aet_db);
        state = 1;
    }
}

x_pv_game_pv_data::x_pv_game_pv_data() : pv_game(), dsc_data_ptr(), dsc_data_ptr_end(),
play(), chara_id(), dsc_time(), pv_end(), playdata(), scene_rot_y(), branch_mode() {
    target_anim_fps = 60.0f;
    anim_frame_speed = 1.0f;
    scene_rot_mat = mat4_identity;

    for (x_pv_play_data& i : playdata)
        i.reset();
}

x_pv_game_pv_data::~x_pv_game_pv_data() {

}

void x_pv_game_pv_data::ctrl(bool a2, float_t curr_time, float_t delta_time) {
#pragma warning(suppress: 26451)
    int64_t _curr_time = 1000 * (int64_t)(curr_time * 1000000.0f);

    dsc_data* prev_dsc_data_ptr = dsc_data_ptr;

    float_t dsc_time_offset = 0.0f;
    while (dsc_data_ptr != dsc_data_ptr_end) {
        bool music_play = false;
        bool next = dsc_ctrl(delta_time, _curr_time, &dsc_time_offset);

        if (prev_dsc_data_ptr != dsc_data_ptr)
            print_dsc_command(dsc, prev_dsc_data_ptr, dsc_time);

        prev_dsc_data_ptr = dsc_data_ptr;

        if (!next)
            break;
    }
}

bool x_pv_game_pv_data::dsc_ctrl(float_t delta_time, int64_t curr_time,
    float_t* dsc_time_offset) {
    dsc_x_func func = (dsc_x_func)dsc_data_ptr->func;
    int32_t* data = dsc.get_func_data(dsc_data_ptr);

    if (branch_mode) {
        bool v19;
        if (pv_game->success)
            v19 = branch_mode == 2;
        else
            v19 = branch_mode == 1;
        if (!v19) {
            if (func < 0 || func >= DSC_X_MAX) {
                play = false;
                return false;
            }
            else if (func != DSC_X_TIME
                && func != DSC_X_PV_END && func != DSC_X_PV_BRANCH_MODE) {
                dsc_data_ptr++;
                return true;
            }
        }
    }

    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    x_pv_play_data* playdata = &this->playdata[chara_id];
    rob_chara* rob_chr = playdata->rob_chr;

    switch (func) {
    case DSC_X_END: {
        play = false;
        pv_game->state_old = 21;
        return false;
    } break;
    case DSC_X_TIME: {
        dsc_time = (int64_t)data[0] * 10000;
        if (dsc_time > curr_time)
            return false;
#if BAKE_X_PACK
        pv_end = true;
#endif
    } break;
    case DSC_X_MIKU_MOVE: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        vec3 pos;
        pos.x = (float_t)data[1] * 0.001f;
        pos.y = (float_t)data[2] * 0.001f;
        pos.z = (float_t)data[3] * 0.001f;

        if (!rob_chr)
            break;

        mat4_transform_point(&scene_rot_mat, &pos, &pos);
        rob_chr->set_data_miku_rot_position(pos);
        rob_chr->set_osage_reset();
    } break;
    case DSC_X_MIKU_ROT: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        float_t rot_y = (float_t)data[0] * 0.001f;

        if (!rob_chr)
            break;

        int16_t rot_y_int16 = (int32_t)((rot_y + scene_rot_y) * 32768.0f * (float_t)(1.0 / 180.0));
        rob_chr->data.miku_rot.rot_y_int16 = rot_y_int16;
        rob_chr->set_osage_reset();
    } break;
    case DSC_X_MIKU_DISP: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t disp = data[1];

        playdata->disp = disp == 1;

        if (!rob_chr)
            break;

        if (disp == 1) {
            rob_chr->set_visibility(true);
            if (rob_chr->check_for_ageageagain_module()) {
                rob_chara_age_age_array_set_skip(rob_chr->chara_id, 1);
                rob_chara_age_age_array_set_skip(rob_chr->chara_id, 2);
            }

            for (x_pv_play_data_set_motion& i : playdata->set_motion) {
                bool set = rob_chr->set_motion_id(i.motion_id, i.frame, i.blend_duration,
                    i.blend, false, i.blend_type, aft_bone_data, aft_mot_db);
                rob_chr->set_motion_reset_data(i.motion_id, i.dsc_frame);
                rob_chr->bone_data->disable_eye_motion = i.disable_eye_motion;
                rob_chr->data.motion.step_data.step = i.frame_speed;
                if (set)
                    pv_expression_array_set_motion(pv_game->get_data()
                        .exp_file.hash_murmurhash, chara_id, i.motion_id);
                set_motion_max_frame(chara_id, i.motion_index, i.dsc_time);
            }
            playdata->set_motion.clear();
        }
        else
            rob_chr->set_visibility(false);
    } break;
    case DSC_X_MIKU_SHADOW: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;
    } break;
    case DSC_X_TARGET: {

    } break;
    case DSC_X_SET_MOTION: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        if (!rob_chr)
            break;

        int32_t motion_index = data[1];
        int32_t blend_duration_int = data[2];
        int32_t frame_speed_int = data[3];

        float_t blend_duration;
        if (blend_duration_int != -1) {
            blend_duration = (float_t)blend_duration_int * 0.001f * 60.0f;
            if (blend_duration < 0.0f)
                blend_duration = 0.0f;
        }
        else
            blend_duration = 0.0f;

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

        bool blend = false;
        x_pv_play_data_motion* motion = playdata->get_motion(motion_index);

        if (motion && motion->enable && (motion_index == motion->motion_index || !motion->motion_index)) {
            if (dsc_time != motion->time) {
                frame = dsc_time_to_frame(curr_time - motion->time);
                dsc_frame = prj::roundf(dsc_time_to_frame(dsc_time - motion->time));
                if (frame < dsc_frame)
                    frame = dsc_frame;
                blend = curr_time > motion->time;
            }
            else
                frame = 0.0f;
        }

        uint32_t motion_id = -1;
        int32_t chara_id = 0;
        for (pvpp_chara& i : pv_game->get_data().play_param->chara) {
            if (chara_id++ != this->chara_id)
                continue;

            int32_t mot_idx = 1;
            for (string_hash& j : i.motion) {
                if (mot_idx++ != motion_index)
                    continue;

                std::string name(j.str);
                size_t pos = name.find("PV832");
                if (pos != -1)
                    name.replace(pos, 5, "PV800", 5);
                motion_id = aft_mot_db->get_motion_id(name.c_str());
                break;
            }
            break;
        }

        if (motion_index) {
            if (motion_id == -1)
                motion_id = rob_chr->get_rob_cmn_mottbl_motion_id(0);
        }
        else if (motion_id == -1) {
            motion_id = rob_chr->get_rob_cmn_mottbl_motion_id(5);
            if (motion_id == -1)
                motion_id = rob_chr->get_rob_cmn_mottbl_motion_id(0);
        }

        if (blend)
            blend_duration = 0.0f;
        else
            blend_duration /= anim_frame_speed;

        x_pv_play_data_set_motion set_motion;
        set_motion.frame_speed = frame_speed * anim_frame_speed;
        set_motion.motion_id = motion_id;
        set_motion.frame = frame;
        set_motion.blend_duration = blend_duration;
        set_motion.blend = blend;
        set_motion.blend_type = MOTION_BLEND_CROSS;
        set_motion.disable_eye_motion = true;
        set_motion.motion_index = motion_index;
        set_motion.dsc_time = motion ? motion->time : dsc_time;
        set_motion.dsc_frame = dsc_frame;

        if (playdata->disp) {
            bool set = rob_chr->set_motion_id(motion_id, frame, blend_duration,
                blend, false, MOTION_BLEND_CROSS, aft_bone_data, aft_mot_db);
            rob_chr->set_motion_reset_data(motion_id, dsc_frame);
            rob_chr->set_motion_skin_param(motion_id, dsc_frame);
            rob_chr->bone_data->disable_eye_motion = true;
            rob_chr->data.motion.step_data.step = set_motion.frame_speed;
            if (set)
                pv_expression_array_set_motion(pv_game->get_data()
                    .exp_file.hash_murmurhash, this->chara_id, motion_id);
            set_motion_max_frame(this->chara_id, motion_index, motion ? motion->time : 0);
        }
        else {
            playdata->set_motion.clear();
            playdata->set_motion.push_back(set_motion);
            rob_chr->data.motion.step_data.step = set_motion.frame_speed;
        }
    } break;
    case DSC_X_SET_PLAYDATA: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t motion_index = data[1];
        if (motion_index >= 0) {
            x_pv_play_data_motion* motion = playdata->get_motion(motion_index);
            if (motion) {
                motion->enable = true;
                motion->motion_index = motion_index;
                motion->time = dsc_time;
            }
            else
                playdata->motion.push_back({ true, motion_index, dsc_time });
        }
        else
            playdata->motion.clear();
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
        int32_t field = data[0];
        if (field > 0)
            x_pv_game_change_field(pv_game, field, dsc_time, curr_time);
        else
            x_pv_game_reset_field(pv_game);

        pv_game->get_data().effect.set_time(dsc_time, false);
        pv_game->get_data().chara_effect.set_time(dsc_time);
    } break;
    case DSC_X_HIDE_FIELD: {

    } break;
    case DSC_X_MOVE_FIELD: {

    } break;
    case DSC_X_FADEOUT_FIELD: {

    } break;
    case DSC_X_EYE_ANIM: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t v115 = data[1];
        int32_t blend_duration_int = data[2];

        float_t blend_duration;
        if (blend_duration_int != -1)
            blend_duration = (float_t)blend_duration_int * 0.001f * 60.0f;
        else
            blend_duration = 6.0f;

        if (!rob_chr)
            break;

        blend_duration /= anim_frame_speed;

        rob_chr->set_eyelid_mottbl_motion_from_face(v115, blend_duration, -1.0f, 0.0f, aft_mot_db);
    } break;
    case DSC_X_MOUTH_ANIM: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t mouth_anim_id = data[2];
        int32_t blend_duration_int = data[3];
        int32_t value_int = data[4];

        float_t blend_duration;
        if (blend_duration_int != -1) {
            blend_duration = (float_t)blend_duration_int * 0.001f * 60.0f;
            if (blend_duration < 0.0f)
                blend_duration = 0.0f;
        }
        else
            blend_duration = 6.0f;

        float_t value;
        if (value_int != -1) {
            value = (float_t)value_int * 0.001f;
            if (value < 0.0f || value > 1.0f)
                value = 1.0f;
        }
        else
            value = 1.0f;

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
        }

        int32_t mottbl_index = mouth_anim_id_to_mottbl_index(mouth_anim_id);
        blend_duration /= anim_frame_speed;

        rob_chr->set_mouth_mottbl_motion(0, mottbl_index, value,
            0, blend_duration, 0.0f, 1.0f, -1, 0.0f, aft_mot_db);
    } break;
    case DSC_X_HAND_ANIM: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t hand_index = data[1];
        int32_t hand_anim_id = data[2];
        int32_t blend_duration_int = data[3];
        int32_t value_int = data[4];

        float_t blend_duration;
        if (blend_duration_int != -1) {
            blend_duration = (float_t)blend_duration_int * 0.001f * 60.0f;
            if (blend_duration < 0.0f)
                blend_duration = 0.0f;
        }
        else
            blend_duration = 0.0f;

        float_t value;
        if (value_int != -1) {
            value = (float_t)value_int * 0.001f;
            if (value < 0.0f || value > 1.0f)
                value = 1.0f;
        }
        else
            value = 1.0f;

        if (!rob_chr)
            break;

        int32_t mottbl_index = hand_anim_id_to_mottbl_index(hand_anim_id);
        blend_duration /= anim_frame_speed;

        switch (hand_index) {
        case 0:
            rob_chr->set_hand_l_mottbl_motion(0, mottbl_index, value,
                0, blend_duration, 0.0f, 1.0f, -1, 0.0f, aft_mot_db);
            break;
        case 1:
            rob_chr->set_hand_r_mottbl_motion(0, mottbl_index, value,
                0, blend_duration, 0.0f, 1.0f, -1, 0.0f, aft_mot_db);
            break;
        }
    } break;
    case DSC_X_LOOK_ANIM: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t look_anim_id = data[1];
        int32_t blend_duration_int = data[2];
        int32_t value_int = data[3];

        float_t blend_duration;
        if (blend_duration_int != -1) {
            blend_duration = (float_t)blend_duration_int * 0.001f * 60.0f;
            if (blend_duration < 0.0f)
                blend_duration = 0.0f;
        }
        else
            blend_duration = 6.0f;

        float_t value;
        if (value_int != -1) {
            value = (float_t)value_int * 0.001f;
            if (value < 0.0f || value > 1.0f)
                value = 1.0f;
        }
        else
            value = 1.0f;

        if (!rob_chr)
            break;

        int32_t mottbl_index = look_anim_id_to_mottbl_index(look_anim_id);
        blend_duration /= anim_frame_speed;

        rob_chr->set_eyes_mottbl_motion(0, mottbl_index, value,
            mottbl_index == 224 ? 1 : 0, blend_duration, 0.0f, 1.0f, -1, 0.0f, aft_mot_db);
    } break;
    case DSC_X_EXPRESSION: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t expression_id = data[1];
        int32_t blend_duration_int = data[2];
        int32_t value_int = data[3];

        float_t blend_duration;
        if (blend_duration_int != -1) {
            blend_duration = (float_t)blend_duration_int * 0.001f * 60.0f;
            if (blend_duration < 0.0f)
                blend_duration = 0.0f;
        }
        else
            blend_duration = 0.0f;

        float_t value;
        if (value_int != -1) {
            value = (float_t)value_int * 0.001f;
            if (value < 0.0f || value > 1.0f)
                value = 1.0f;
        }
        else
            value = 1.0f;

        if (!rob_chr)
            break;

        int32_t mottbl_index = expression_id_to_mottbl_index(expression_id);
        blend_duration /= anim_frame_speed;

        rob_chr->set_face_mottbl_motion(0, mottbl_index, value, mottbl_index >= 214
            && mottbl_index <= 223 ? 1 : 0, blend_duration, 0.0f, 1.0f, -1, 0.0f, true, aft_mot_db);

    } break;
    case DSC_X_LOOK_CAMERA: {

    } break;
    case DSC_X_LYRIC: {

    } break;
    case DSC_X_MUSIC_PLAY: {
#if !BAKE_FAST
        char buf[0x200];
        sprintf_s(buf, sizeof(buf), "rom/sound/song/pv_%03d.ogg",
            pv_game->get_data().pv_id == 832 ? 800 : pv_game->get_data().pv_id);
        x_pv_game_music_ptr->play(4, buf, false, 0.0f, 0.0f, 0.0f, 3.0f, false);
        x_pv_game_music_ptr->set_channel_pair_volume_map(1, 100);
        x_pv_game_music_ptr->set_channel_pair_volume_map(0, 100);
#endif
    } break;
    case DSC_X_MODE_SELECT: {

    } break;
    case DSC_X_EDIT_MOTION: {

    } break;
    case DSC_X_BAR_TIME_SET: {

    } break;
    case DSC_X_SHADOWHEIGHT: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;
    } break;
    case DSC_X_EDIT_FACE: {
        int32_t expression_id = data[0];

        if (!rob_chr)
            break;

        int32_t mottbl_index = expression_id_to_mottbl_index(expression_id);

        rob_chr->set_face_mottbl_motion(0, mottbl_index, 1.0f, mottbl_index >= 214
            && mottbl_index <= 223 ? 1 : 0, 0.0f, 0.0f, 1.0f, -1, 0.0f, true, aft_mot_db);
    } break;
    case DSC_X_DUMMY: {

    } break;
    case DSC_X_PV_END: {
        dsc_data_ptr++;
        pv_end = true;
        break;
    } break;
    case DSC_X_SHADOWPOS: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;
    } break;
    case DSC_X_EDIT_LYRIC: {

    } break;
    case DSC_X_EDIT_TARGET: {

    } break;
    case DSC_X_EDIT_MOUTH: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t mouth_anim_id = data[1];

        if (!rob_chr)
            break;

        int32_t mottbl_index = mouth_anim_id_to_mottbl_index(mouth_anim_id);

        rob_chr->set_mouth_mottbl_motion(0, mottbl_index, 1.0f,
            0, 0.1f, 0.0f, 1.0f, -1, 0.0f, aft_mot_db);
    } break;
    case DSC_X_SET_CHARA: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;
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
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;
    } break;
    case DSC_X_HAND_ITEM: {

    } break;
    case DSC_X_EDIT_BLUSH: {

    } break;
    case DSC_X_NEAR_CLIP: {

    } break;
    case DSC_X_CLOTH_WET: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        float_t wet = (float_t)data[1] * 0.001f;

        wet = clamp_def(wet, 0.0f, 1.0f);

        if (rob_chr)
            rob_chr->item_equip->wet = wet;
    } break;
    case DSC_X_LIGHT_ROT: {

    } break;
    case DSC_X_SCENE_FADE: {

    } break;
    case DSC_X_TONE_TRANS: {
        vec3 start;
        start.x = (float_t)data[0] * 0.001f;
        start.y = (float_t)data[1] * 0.001f;
        start.z = (float_t)data[2] * 0.001f;

        vec3 end;
        end.x = (float_t)data[3] * 0.001f;
        end.y = (float_t)data[4] * 0.001f;
        end.z = (float_t)data[5] * 0.001f;

        rctx_ptr->render.set_tone_trans(start, end, 1);
    } break;
    case DSC_X_SATURATE: {
        float_t saturate_coeff = (float_t)data[0] * 0.001f;

        rctx_ptr->render.set_saturate_coeff(saturate_coeff, 1, false);
    } break;
    case DSC_X_FADE_MODE: {
        int32_t blend_func = data[0];

        rctx_ptr->render.set_scene_fade_blend_func(blend_func, 1);
    } break;
    case DSC_X_AUTO_BLINK: {

    } break;
    case DSC_X_PARTS_DISP: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        item_id id = (item_id)data[1];
        int32_t disp = data[2];

        if (!rob_chr)
            break;

        rob_chr->set_parts_disp(id, disp == 1);
    } break;
    case DSC_X_TARGET_FLYING_TIME: {

    } break;
    case DSC_X_CHARA_SIZE: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t chara_size = data[1];

        if (!rob_chr)
            break;

        int32_t chara_size_index;
        if (chara_size == 0)
            chara_size_index = chara_init_data_get_chara_size_index(rob_chr->chara_index);
        else if (chara_size == 1) {
            ::chara_index chara_index = CHARA_MIKU;
            if (chara_id < pv_game->get_data().play_param->chara.size())
                chara_index = (::chara_index)pv_game->get_data()
                    .play_param->chara[chara_id].chara_effect.base_chara;
            else if (pv_game->get_data().pv_id == 826 && false)
                chara_index = rob_chara_array_get(chara_id)->chara_index;

            chara_size_index = chara_init_data_get_chara_size_index(chara_index);
        }
        else if (chara_size == 2)
            chara_size_index = 1;
        else if (chara_size == 3)
            chara_size_index = rob_chr->pv_data.chara_size_index;
        else {
            rob_chr->set_chara_size((float_t)chara_size / 1000.0f);
            break;
        }

        if (chara_size_index < 0 || chara_size_index > 4)
            break;

        rob_chr->set_chara_size(chara_size_table_get_value(chara_size_index));
        rob_chr->set_chara_pos_adjust_y(chara_pos_adjust_y_table_get_value(chara_size_index));
    } break;
    case DSC_X_CHARA_HEIGHT_ADJUST: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        int32_t height_adjust = data[1];

        if (rob_chr)
            rob_chr->set_chara_height_adjust(height_adjust != 0);
    } break;
    case DSC_X_ITEM_ANIM: {
        printf_debug("");
    } break;
    case DSC_X_CHARA_POS_ADJUST: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        vec3 pos;
        pos.x = (float_t)data[2] * 0.001f;
        pos.y = (float_t)data[3] * 0.001f;
        pos.z = (float_t)data[4] * 0.001f;

        if (!rob_chr)
            break;

        mat4_transform_point(&scene_rot_mat, &pos, &pos);
        rob_chr->set_chara_pos_adjust(pos);

        if (rob_chr->check_for_ageageagain_module()) {
            rob_chara_age_age_array_set_skip(rob_chr->chara_id, 1);
            rob_chara_age_age_array_set_skip(rob_chr->chara_id, 2);
        }
    } break;
    case DSC_X_SCENE_ROT: {
        float_t scene_rot = (float_t)data[0] * 0.001f;

        scene_rot_y = scene_rot;
        mat4_rotate_y(scene_rot * DEG_TO_RAD_FLOAT, &scene_rot_mat);

    } break;
    case DSC_X_EDIT_MOT_SMOOTH_LEN: {

    } break;
    case DSC_X_PV_BRANCH_MODE: {
        int32_t branch_mode = data[0];

        if (branch_mode >= 0 && branch_mode <= 2)
            this->branch_mode = branch_mode;
    } break;
    case DSC_X_DATA_CAMERA_START: {

    } break;
    case DSC_X_MOVIE_PLAY: {

    } break;
    case DSC_X_MOVIE_DISP: {

    } break;
    case DSC_X_WIND: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        float_t wind_strength_kami = (float_t)data[1] * 0.001f;
        float_t wind_strength_outer = (float_t)data[2] * 0.001f;

        if (!rob_chr)
            break;

        rob_chr->set_wind_strength(wind_strength_outer);
    } break;
    case DSC_X_OSAGE_STEP: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        float_t osage_step_kami = (float_t)data[1] * 0.001f;
        float_t osage_step_outer = (float_t)data[2] * 0.001f;

        if (!rob_chr)
            break;

        rob_chr->set_osage_step(osage_step_outer);
    } break;
    case DSC_X_OSAGE_MV_CCL: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        float_t osage_mv_ccl_kami = (float_t)data[1] * 0.001f;
        float_t osage_mv_ccl_outer = (float_t)data[2] * 0.001f;

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
        int32_t index = data[0];

        if (index)
            pv_game->get_data().title.show(index - 1);
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
        int32_t id = data[0];
        float_t duration = (float_t)data[1];

        set_pv_param_post_process_bloom_data(true, id, duration);
    } break;
    case DSC_X_COLOR_CORRECTION: {
        int32_t enable = data[0];
        int32_t id = data[1];
        float_t duration = (float_t)data[2];

        set_pv_param_post_process_color_correction_data(enable == 1, id, duration);
    } break;
    case DSC_X_DOF: {
        int32_t enable = data[0];
        int32_t id = data[1];
        float_t duration = (float_t)data[2];

        set_pv_param_post_process_dof_data(enable == 1, id, duration);
    } break;
    case DSC_X_CHARA_ALPHA: {
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        float_t alpha = (float_t)data[1] * 0.001f;
        float_t duration = (float_t)data[2];
        int32_t type = data[3];

        if (chara_id >= 0 && chara_id < ROB_CHARA_COUNT)
            pv_param_task::post_process_task_set_chara_alpha(
                chara_id, type, alpha, duration * 2.0f);
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
        chara_id = data[0];
        playdata = &this->playdata[chara_id];
        rob_chr = playdata->rob_chr;

        float_t alpha = (float_t)data[1] * 0.001f;
        float_t duration = (float_t)data[2];
        int32_t type = data[3];

        if (chara_id >= 0 && chara_id < ROB_CHARA_COUNT) {
            pv_param_task::post_process_task_set_chara_item_alpha(
                chara_id, type, alpha, duration * 2.0f,
                x_pv_game_item_alpha_callback, this->pv_game);
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
        int32_t stage_effect = data[0];

        if (stage_effect >= 8 && stage_effect <= 9)
            pv_game->stage_data.set_stage_effect(stage_effect);
    } break;
    case DSC_X_SONG_EFFECT: {
        bool enable = data[0] ? true : false;
        int32_t index = data[1];
        int32_t unk2 = data[2];

        if (unk2 && !(pv_game->get_data().field_1C & 0x10))
            break;

        x_pv_game_effect& effect = pv_game->get_data().effect;
        if (enable) {
            effect.set_song_effect(index, dsc_time);
            //if (a2->field_18)
            //    effect.set_song_effect_time(index/*, a2->field_20*/, true);
            //else
                effect.set_song_effect_time(index, pv_game->time, false);
        }
        else
            effect.stop_song_effect(index, true);
    } break;
    case DSC_X_SONG_EFFECT_ATTACH: {
        int32_t index = data[0];
        int32_t chara_id = data[1];
        int32_t chara_item = data[2];

        pv_game->get_data().effect.set_chara_id(index, chara_id, !!chara_item);
    } break;
    case DSC_X_LIGHT_AUTH: {

    } break;
    case DSC_X_FADE: {

    } break;
    case DSC_X_SET_STAGE_EFFECT_ENV: {
        int32_t env_index = data[0];
        int32_t trans = data[1];
        pv_game->stage_data.set_env(env_index, (float_t)trans * (float_t)(1.0f / 60.0f), 0.0f);
        printf_debug("Time: %8d; Frame %5d; Env: %2d; Trans: %3d\n",
            (int32_t)(curr_time / 10000), pv_game->frame, env_index, trans);
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
        int32_t chara_id = data[0];
        bool enable = data[1] ? true : false;
        int32_t index = data[2];

        x_pv_game_chara_effect& chara_effect = pv_game->get_data().chara_effect;
        if (enable) {
            chara_effect.set_chara_effect(chara_id, index, dsc_time);
            //if (a2->field_18)
            //    chara_effect.set_chara_effect_time(chara_id, index/*, a2->field_20*/);
            //else
                chara_effect.set_chara_effect_time(chara_id, index, pv_game->time);
        }
        else
            chara_effect.stop_chara_effect(chara_id, index);
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

    dsc_data_ptr++;
    return true;
}

dsc_data* x_pv_game_pv_data::find(int32_t func_name, int32_t* time,
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
            int32_t* data = dsc.get_func_data(i);
            _time = data[0];
        }
        else if (i->func == DSC_X_PV_END)
            break;
        else if (i->func == DSC_X_PV_BRANCH_MODE) {
            if (pv_branch_mode) {
                int32_t* data = dsc.get_func_data(i);
                *pv_branch_mode = data[0];
            }
        }
    return 0;
}

void x_pv_game_pv_data::find_bar_beat(x_pv_bar_beat& bar_beat) {
    x_pv_bar_beat_data* bar_beat_data = 0;
    int32_t beat_counter = 0;

    int32_t time = -1;
    for (dsc_data& i : dsc.data) {
        if (i.func == DSC_X_END)
            break;

        int32_t* data = dsc.get_func_data(&i);
        switch (i.func) {
        case DSC_X_TIME: {
            time = data[0];
        } break;
        case DSC_X_BAR_POINT: {
            bar_beat.data.push_back({});
            bar_beat_data = &bar_beat.data.back();
            *bar_beat_data = {};
            bar_beat_data->bar = data[0];
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

void x_pv_game_pv_data::find_change_fields(std::vector<int64_t>& change_fields) {
    int32_t pv_branch_mode = 0;
    int32_t time = -1;
    int32_t prev_time = -1;

    dsc_data* i = dsc.data.data();
    dsc_data* i_end = dsc.data.data() + dsc.data.size();
    while (i = find(DSC_X_CHANGE_FIELD, &time, &pv_branch_mode, i, i_end)) {
        if (time < 0) {
            time = prev_time;
            if (prev_time < 0)
                break;
        }

        change_fields.push_back((int64_t)time * 10000);
        prev_time = time;
        i++;
    }
}

void x_pv_game_pv_data::find_playdata_item_anim(int32_t chara_id) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return;

    x_pv_play_data* playdata = &this->playdata[chara_id];
    playdata->motion_data.set_item.clear();

    int32_t pv_branch_mode = 0;
    int32_t time = -1;
    int32_t prev_time = -1;

    dsc_data* i = dsc.data.data();
    dsc_data* i_end = dsc.data.data() + dsc.data.size();
    while (true) {
        i = find(DSC_X_ITEM_ANIM, &time, &pv_branch_mode, i, i_end);
        if (!i)
            break;

        if (time < 0) {
            time = prev_time;
            if (prev_time < 0)
                break;
        }

        int32_t* data = dsc.get_func_data(i);
        if (chara_id == data[0] && data[2] == 2)
            playdata->motion_data.set_item.push_back({ time, data[1], pv_branch_mode });
        prev_time = time;
        i++;
    }
}

void x_pv_game_pv_data::find_playdata_set_motion(int32_t chara_id) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return;

    x_pv_play_data* playdata = &this->playdata[chara_id];
    playdata->motion_data.set_motion.clear();

    int32_t pv_branch_mode = 0;
    int32_t time = -1;
    int32_t prev_time = -1;

    dsc_data* i = dsc.data.data();
    dsc_data* i_end = dsc.data.data() + dsc.data.size();
    while (true) {
        i = find(DSC_X_SET_MOTION, &time, &pv_branch_mode, i, i_end);
        if (!i)
            break;

        if (time < 0) {
            time = prev_time;
            if (prev_time < 0)
                break;
        }

        int32_t* data = dsc.get_func_data(i);
        if (chara_id == data[0])
            playdata->motion_data.set_motion.push_back({ time, data[1], pv_branch_mode });
        prev_time = time;
        i++;
    }
}

int64_t x_pv_game_pv_data::find_pv_end() {
    int32_t pv_branch_mode = 0;
    int32_t pv_end_time = -1;
    int32_t end_time = -1;

    dsc_data* i = dsc.data.data();
    dsc_data* i_end = dsc.data.data() + dsc.data.size();
    find(DSC_X_PV_END, &pv_end_time, &pv_branch_mode, i, i_end);

    pv_branch_mode = 0;

    i = dsc.data.data();
    i_end = dsc.data.data() + dsc.data.size();
    find(DSC_X_END, &end_time, &pv_branch_mode, i, i_end);
    return max_def(pv_end_time, end_time);
}

void x_pv_game_pv_data::find_set_motion() {
    for (std::vector<pv_data_set_motion>& i : set_motion)
        i.clear();

    std::vector<x_pv_play_data_event> v98[6];

    int32_t pv_branch_mode = 0;
    int32_t time = -1;
    int32_t prev_time = -1;

    dsc_data* i = dsc.data.data();
    dsc_data* i_end = dsc.data.data() + dsc.data.size();
    while (true) {
        i = find(DSC_X_SET_PLAYDATA, &time, &pv_branch_mode, i, i_end);
        if (!i)
            break;

        if (time < 0)
            time = prev_time;

        int32_t* data = dsc.get_func_data(i);

        int32_t chara_id = data[0];
        int32_t motion_index = data[1];

        v98[chara_id].push_back({ time, motion_index, pv_branch_mode });

        prev_time = time;
        i++;
    }

    pv_branch_mode = 0;
    time = -1;
    prev_time = -1;

    i = dsc.data.data();
    i_end = dsc.data.data() + dsc.data.size();
    while (true) {
        i = find(DSC_X_CHANGE_FIELD, &time, &pv_branch_mode, i, i_end);
        if (!i)
            break;

        if (time < 0)
            time = prev_time;

        prev_time = time;
        i++;
    }

    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    pvpp* play_param = pv_game->get_data().play_param;

    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
        x_pv_play_data& playdata = this->playdata[i];
        std::vector<pv_data_set_motion>& set_motion = this->set_motion[i];

        for (x_pv_play_data_event& j : playdata.motion_data.set_motion) {
            int32_t time = -1;
            int32_t v56 = 0;

            x_pv_play_data_event* i_begin = v98[i].data() + v98[i].size();
            x_pv_play_data_event* i_end = v98[i].data();
            for (x_pv_play_data_event* i = i_begin; i != i_end; ) {
                i--;

                if (i->index == j.index && i->time <= j.time) {
                    time = i->time;
                    break;
                }
            }

            if (time < 0)
                time = j.time;

            float_t frame = prj::roundf(dsc_time_to_frame(((int64_t)j.time - time) * 10000));

            uint32_t motion_id = -1;
            if (i >= 0 && i < play_param->chara.size()) {
                pvpp_chara& chara = play_param->chara[i];
                if (j.index > 0 && j.index <= chara.motion.size()) {
                    string_hash& motion = chara.motion[(size_t)j.index - 1];

                    std::string name(motion.str);
                    size_t pos = name.find("PV832");
                    if (pos != -1)
                        name.replace(pos, 5, "PV800", 5);
                    motion_id = aft_mot_db->get_motion_id(name.c_str());
                }
            }

            if (motion_id != -1)
                set_motion.push_back({ motion_id, { frame, -1 } });
        }
    }

    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
        if (i < 0 || i >= play_param->chara.size())
            continue;

        rob_chara* rob_chr = rob_chara_array_get(i);
        int32_t pv_id = pv_game->get_data().pv_id;

        std::vector<pv_data_set_motion>& set_motion = this->set_motion[i];
        std::vector<osage_init_data> vec;
        vec.reserve(set_motion.size());
        for (pv_data_set_motion& j : set_motion) {
            osage_init_data osage_init;
            osage_init.rob_chr = rob_chr;
            osage_init.pv_id = pv_id;
            osage_init.motion_id = j.motion_id;
            osage_init.frame = (int32_t)j.frame_stage_index.first;
            vec.push_back(osage_init);
        }
        skin_param_manager_add_task(i, vec);
    }
}

void x_pv_game_pv_data::find_stage_effects(prj::vector_pair<int64_t, int32_t>& stage_effects) {
    int32_t pv_branch_mode = 0;
    int32_t time = -1;
    int32_t prev_time = -1;

    dsc_data* i = dsc.data.data();
    dsc_data* i_end = dsc.data.data() + dsc.data.size();
    while (i = find(DSC_X_STAGE_EFFECT, &time, &pv_branch_mode, i, i_end)) {
        if (time < 0) {
            time = prev_time;
            if (prev_time < 0)
                break;
        }

        int32_t* data = dsc.get_func_data(i);
        int32_t stage_effect = data[0];

        if (stage_effect < 8 || stage_effect > 9)
            stage_effects.push_back((int64_t)time * 10000, stage_effect);
        prev_time = time;
        i++;
    }
}

void x_pv_game_pv_data::init(class x_pv_game* pv_game, bool music_play) {
    this->pv_game = pv_game;
    target_anim_fps = 60.0f;
    anim_frame_speed = 1.0f;
    scene_rot_mat = mat4_identity;

    target_anim_fps = get_target_anim_fps();
    anim_frame_speed = get_anim_frame_speed();

    pv_expression_array_reset();

    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
        if (pv_game->rob_chara_ids[i] == -1)
            continue;

        x_pv_play_data& playdata = this->playdata[i];
        playdata.reset();
        playdata.set_motion.clear();

        if (pv_game->rob_chara_ids[i] != -1)
            playdata.rob_chr = rob_chara_array_get(pv_game->rob_chara_ids[i]);

        if (playdata.rob_chr) {
            playdata.rob_chr->frame_speed = anim_frame_speed;
            playdata.rob_chr->data.motion.step_data.step = anim_frame_speed;
            pv_expression_array_set(i, playdata.rob_chr, anim_frame_speed);
        }

        find_playdata_set_motion(i);
        find_playdata_item_anim(i);
    }

    find_set_motion();

    pv_end = false;
    scene_rot_y = 0.0f;
    scene_rot_mat = mat4_identity;

    play = true;

    find_change_fields(pv_game->get_data().change_fields);

    Shadow* shad = shadow_ptr_get();
    if (shad) {
        shad->blur_filter_enable[0] = true;
        shad->blur_filter_enable[1] = true;
    }

    chara_id = 0;
}

void x_pv_game_pv_data::reset() {
    dsc.type = DSC_NONE;
    dsc.signature = 0;
    dsc.id = 0;
    dsc.data.clear();
    dsc.data.shrink_to_fit();
    dsc.data_buffer.clear();
    dsc.data_buffer.shrink_to_fit();
    dsc_data_ptr = 0;
    dsc_data_ptr_end = 0;
    dsc_time = -1;
}

bool x_pv_game_pv_data::set_pv_param_post_process_bloom_data(bool set, int32_t id, float_t duration) {
    pv_param::bloom& bloom = pv_param::post_process_data_get_bloom_data(id);
    if (set)
        pv_param_task::post_process_task_set_bloom_data(bloom, duration);
    return true;
}

bool x_pv_game_pv_data::set_pv_param_post_process_color_correction_data(bool set, int32_t id, float_t duration) {
    pv_param::color_correction& color_correction = pv_param::post_process_data_get_color_correction_data(id);
    if (set)
        pv_param_task::post_process_task_set_color_correction_data(color_correction, duration);
    return true;
}

bool x_pv_game_pv_data::set_pv_param_post_process_dof_data(bool set, int32_t id, float_t duration) {
    rctx_ptr->render.set_dof_enable(set);
    rctx_ptr->render.set_dof_update(set);

    pv_param::dof& dof = pv_param::post_process_data_get_dof_data(id);
    if (set)
        pv_param_task::post_process_task_set_dof_data(dof, duration,
            x_pv_game_dof_callback, this->pv_game);
    return true;
}

void x_pv_game_pv_data::set_motion_max_frame(int32_t chara_id, int32_t motion_index, int64_t time) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT || motion_index < 0)
        return;

    x_pv_play_data& playdata = this->playdata[chara_id];

    std::vector<x_pv_play_data_event>& set_motion = playdata.motion_data.set_motion;
    for (x_pv_play_data_event& i : set_motion)
        if (10000LL * i.time > dsc_time && i.index == motion_index
            && (!branch_mode || branch_mode == i.pv_branch_mode)) {
            playdata.rob_chr->bone_data->set_motion_max_frame(
                prj::roundf(dsc_time_to_frame(10000LL * i.time - time)) - 1.0f);
            break;
        }
}

void x_pv_game_pv_data::stop() {
    if (dsc.data.size()) {
        dsc_data_ptr = dsc.data.data();
        dsc_data_ptr_end = dsc_data_ptr + dsc.data.size();
    }
    else {
        dsc_data_ptr = 0;
        dsc_data_ptr_end = 0;
    }
    dsc_time = -1;
}

void x_pv_game_pv_data::unload() {
    unload_data();
}

void x_pv_game_pv_data::unload_data() {
    dsc.type = DSC_NONE;
    dsc.signature = 0;
    dsc.id = 0;
    dsc.data.clear();
    dsc.data.shrink_to_fit();
    dsc.data_buffer.clear();
    dsc.data_buffer.shrink_to_fit();
    dsc_data_ptr = 0;
    dsc_data_ptr_end = 0;
    dsc_time = -1;
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

#if BAKE_PV826
void x_pv_game_data::ctrl(float_t curr_time, float_t delta_time,
    std::map<uint32_t, auth_3d_id>* effchrpv_auth_3d_mot_ids) {
#else
void x_pv_game_data::ctrl(float_t curr_time, float_t delta_time) {
#endif
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
        effect.load_data(pv_id, &obj_db, &tex_db);
        chara_effect.load_data();
        title.load_data();
        pv_expression_file_load(&data_list[DATA_X], "root+/pv_expression/", exp_file.hash_murmurhash);
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

        wait_load |= pv_expression_file_check_not_ready(exp_file.hash_murmurhash);
        wait_load |= camera.state && camera.state != 20;
        wait_load |= effect.state && effect.state != 20;
        wait_load |= chara_effect.state && chara_effect.state != 10 && chara_effect.state != 30;

        if (wait_load)
            break;

        state = 23;
    } break;
    case 23: {
        pv_data.find_bar_beat(bar_beat);
        bar_beat.reset_time();
        stage->bpm_frame_rate_control.bar_beat = &bar_beat;
        state = 24;
    } break;
    case 24: {
        pv_data.find_stage_effects(stage_effects);

        int64_t pv_end_time = pv_data.find_pv_end();
        if (pv_end_time >= 0)
            this->pv_end_time = (float_t)((double_t)pv_end_time * 0.000000001);
        state = 25;
    } break;
    case 25: {
        pv_data.find_change_fields(change_fields);
        effect.change_fields = &change_fields;
        chara_effect.change_fields = &change_fields;
        state = 30;
    } break;
    case 30: {
        ctrl_stage_effect_index();
        bar_beat.set_time(this->curr_time, this->delta_time);
        pv_data.ctrl(true, curr_time, delta_time);
        frame++;
    } break;
    case 40: {
        //if (!sub_81254B9E())
            state = 0;
    } break;
    }

    camera.ctrl(this->curr_time);
    effect.ctrl(&obj_db, &tex_db);
#if BAKE_PV826
    chara_effect.ctrl(&obj_db, &tex_db, pv_id, effchrpv_auth_3d_mot_ids);
#else
    chara_effect.ctrl(&obj_db, &tex_db);
#endif
    title.ctrl();
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

    int32_t v14;
    if (stage->check_stage_effect_has_change_effect(curr_stage_effect->second, next_stage_effect->second))
        v14 = bar / 2 - 2;
    else
        v14 = bar / 2 - 1;
    next_stage_effect_bar = 2 * max_def(v14, 0) + 1;
}

void x_pv_game_data::disp() {
    title.disp();
}

#if BAKE_PV826
void x_pv_game_data::load(int32_t pv_id, FrameRateControl* frame_rate_control, chara_index charas[6],
    std::unordered_map<std::string, string_hash>* effchrpv_auth_3d_mot_names) {
#else
void x_pv_game_data::load(int32_t pv_id, FrameRateControl* frame_rate_control, chara_index charas[6]) {
#endif
    if (state)
        return;

    data_struct* x_data = &data_list[DATA_X];

    title.load(pv_id, frame_rate_control);
    effect.load(pv_id, play_param, frame_rate_control);
#if BAKE_PV826
    chara_effect.load(pv_id, play_param, frame_rate_control, charas, effchrpv_auth_3d_mot_names);
#else
    chara_effect.load(pv_id, play_param, frame_rate_control, charas);
#endif

    char buf[0x200];
    size_t len = sprintf_s(buf, sizeof(buf), "exp_PV%03d", pv_id);
    exp_file = std::string(buf, len);

    camera.load(pv_id, stage->stage_id, frame_rate_control);

#if !BAKE_FAST
    sprintf_s(buf, sizeof(buf), "rom/sound/song/pv_%03d.ogg", pv_id == 832 ? 800 : pv_id);
    x_pv_game_music_ptr->ogg_reset();
    x_pv_game_music_ptr->load(4, buf, true, 0.0f, false);
#endif

    state = 10;
}

void x_pv_game_data::reset() {
#if !BAKE_FAST
    x_pv_game_music_ptr->stop();
    x_pv_game_music_ptr->set_volume_map(0, 0);
#endif

    pv_id = 0;
    play_param_file_handler.reset();
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
    camera.reset();
    effect.reset();
    chara_effect.reset();
    pv_data.reset();
    exp_file.clear();
    //dof = {};
    stage = 0;
    obj_db.clear();
    tex_db.clear();
}

void x_pv_game_data::stop() {
    camera.stop();
    stage->reset_stage_effect();
    stage->reset_stage_env();
    stage->curr_stage_effect = 0;
    stage->next_stage_effect = 0;
    stage->stage_effect_transition_state = 0;
    effect.stop();
    chara_effect.stop();
    pv_data.stop();
    bar_beat.reset_time();
    stage_effect_index = 0;
    next_stage_effect_bar = 0;
    field_1C &= ~0xC0;
}

void x_pv_game_data::unload() {
    stop();

    camera.unload();
    title.unload();
    effect.unload();
    chara_effect.unload();
    //dof = {};
    pv_data.unload();
    pv_expression_file_unload(exp_file.hash_murmurhash);

    obj_db.clear();
    tex_db.clear();

#if !BAKE_FAST
    x_pv_game_music_ptr->stop();
    x_pv_game_music_ptr->set_channel_pair_volume_map(0, 100);
    x_pv_game_music_ptr->set_channel_pair_volume_map(1, 100);
    x_pv_game_music_ptr->exclude_flags(X_PV_GAME_MUSIC_OGG);
#endif
    state = 40;
}

void x_pv_game_data::unload_data() {
    if (state)
        return;

    stop();

    camera.unload_data();
    title.unload_data();
    effect.unload_data();
    chara_effect.unload_data();
    //dof = {};
    pv_data.unload_data();
    pv_expression_file_unload(exp_file.hash_murmurhash);

    obj_db.clear();
    tex_db.clear();

#if !BAKE_FAST
    x_pv_game_music_ptr->stop();
    x_pv_game_music_ptr->set_channel_pair_volume_map(0, 100);
    x_pv_game_music_ptr->set_channel_pair_volume_map(1, 100);
    x_pv_game_music_ptr->exclude_flags(X_PV_GAME_MUSIC_OGG);
#endif
}

PVStageFrameRateControl::PVStageFrameRateControl() {
    delta_frame = 1.0f;
}

PVStageFrameRateControl::~PVStageFrameRateControl() {

}

float_t PVStageFrameRateControl::get_delta_frame() {
    return delta_frame;
}

x_pv_game_stage_env_data::x_pv_game_stage_env_data() {

}

x_pv_game_stage_env_aet::x_pv_game_stage_env_aet() : state(), prev(), next() {
    duration = 1.0f;
}

x_pv_game_stage_env::x_pv_game_stage_env() : flags(),
state(), stage_resource(), trans_duration(), trans_remain() {
    env_index = -1;
    aet_gam_stgpv_id_hash = hash_murmurhash_empty;
    spr_gam_stgpv_id_hash = hash_murmurhash_empty;
    aet_gam_stgpv_id_main_hash = hash_murmurhash_empty;

    for (auto& i : data)
        for (auto& j : i.data)
            for (auto& k : j)
                k.frame_rate_control = &frame_rate_control;
}

x_pv_game_stage_env::~x_pv_game_stage_env() {

}

void x_pv_game_stage_env::ctrl(float_t delta_time) {
    frame_rate_control.delta_frame = delta_time * 60.0f;

    switch (state) {
    case 10:
        state = 11;
    case 11:
        if ((aet_gam_stgpv_id_hash == hash_murmurhash_empty
            || !aet_manager_load_file_modern(aet_gam_stgpv_id_hash, &aet_db))
            && (spr_gam_stgpv_id_hash == hash_murmurhash_empty
                || !sprite_manager_load_file_modern(spr_gam_stgpv_id_hash, &spr_db))) {
            spr_set* set = sprite_manager_get_set(spr_gam_stgpv_id_hash, &spr_db);
            if (set) {
                SpriteData* sprdata = set->sprdata;
                for (uint32_t i = set->num_of_sprite; i; i--, sprdata++)
                    sprdata->resolution_mode = RESOLUTION_MODE_HD;
            }
            state = 20;
        }
        break;
    case 20:
        if (env_index != -1 && !sub_810EE198(delta_time))
            sub_810EE03E();
        break;
    }
}

pvsr_auth_2d* x_pv_game_stage_env::get_aet(int32_t env_index, int32_t type, int32_t index) {
    if (env_index < 0 || env_index >= stage_resource->stage_effect_env.size())
        return 0;

    pvsr_stage_effect_env* env = &stage_resource->stage_effect_env[env_index];

    switch (type)  {
    case 0:
    default:
        if (index < env->aet_front.size())
            return &env->aet_front[index];
        break;
    case 1:
        if (index < env->aet_front_low.size())
            return &env->aet_front_low[index];
        break;
    case 2:
        if (index < env->aet_back.size())
            return &env->aet_back[index];
        break;
    case 3:
        if (index < env->unk03.size())
            return &env->unk03[index];
        break;
    case 4:
        if (index < env->unk04.size())
            return &env->unk04[index];
        break;
    }
    return 0;
}

void x_pv_game_stage_env::load(int32_t stage_id, pvsr* stage_resource) {
    if (this->stage_resource || !stage_resource || stage_resource->stage_effect_env.size() > 64)
        return;

    this->stage_resource = stage_resource;

    bool aet = false;
    for (pvsr_stage_effect_env& i : stage_resource->stage_effect_env)
        if (i.aet_front.size() || i.aet_front_low.size()
            || i.aet_back.size() || i.unk03.size() || i.unk04.size()) {
            aet = true;
            break;
        }

    if (aet) {
        char buf[0x40];
        sprintf_s(buf, sizeof(buf), "AET_GAM_STGPV%03d", stage_id);
        aet_gam_stgpv_id_hash = hash_utf8_murmurhash(buf);

        sprintf_s(buf, sizeof(buf), "SPR_GAM_STGPV%03d", stage_id);
        spr_gam_stgpv_id_hash = hash_utf8_murmurhash(buf);

        aet_manager_read_file_modern(aet_gam_stgpv_id_hash, &data_list[DATA_X], &aet_db);
        sprite_manager_read_file_modern(spr_gam_stgpv_id_hash, &data_list[DATA_X], &spr_db);

        sprintf_s(buf, sizeof(buf), "AET_GAM_STGPV%03d_MAIN", stage_id);
        aet_gam_stgpv_id_main_hash = hash_utf8_murmurhash(buf);
    }

    state = 10;
}

void x_pv_game_stage_env::reset() {
    flags = 0;
    state = 0;
    stage_resource = 0;
    env_index = -1;
    aet_gam_stgpv_id_hash = hash_murmurhash_empty;
    spr_gam_stgpv_id_hash = hash_murmurhash_empty;
    aet_gam_stgpv_id_main_hash = hash_murmurhash_empty;
    frame_rate_control.delta_frame = 1.0f;

    aet_db.clear();
    spr_db.clear();
}

void x_pv_game_stage_env::reset_env() {
    if (env_index == -1)
        return;

    for (auto& i : data[env_index].data)
        for (auto& j : i)
            j.reset();

    trans_duration = 0.0f;
    trans_remain = 0.0f;
    aet = {};
    env_index = -1;
}

static const float_t env_aet_opacity = 0.65f;

#pragma warning(push)
#pragma warning(disable: 6385)
void x_pv_game_stage_env::set(int32_t env_index, float_t end_time, float_t start_time) {
    if (!stage_resource || env_index < 0 || env_index >= 64
        || env_index >= stage_resource->stage_effect_env.size() || this->env_index == env_index)
        return;

    float_t duration = max_def(end_time - start_time, 0.0f);
    if (trans_duration > 0.0f) {
        if (fabsf(end_time) > 0.000001f)
            return;

        trans_duration = 0.0f;
        trans_remain = 0.0f;
        aet = {};
    }

    if (this->env_index == -1 || fabsf(duration) <= 0.000001f) {
        float_t frame = start_time * 60.0f;
        if (this->env_index != -1)
            for (auto& i : data[this->env_index].data)
                for (auto& j : i)
                    j.reset();

        if (!(flags & 0x04))
            for (int32_t type = 0; type < 5; type++)
                for (int32_t index = 0; index < 8; index++) {
                    pvsr_auth_2d* aet = get_aet(env_index, type, index);
                    if (!aet)
                        break;

                    aet_obj_data& aet_obj = data[env_index].data[type][index];

                    AetArgs args;
                    args.id.id = aet_gam_stgpv_id_main_hash;
                    args.layer_name = aet->name.c_str();
                    args.flags = AET_PLAY_ONCE;
                    switch (type) {
                    case 0:
                        args.index = 0;
                        args.prio = spr::SPR_PRIO_01;
                        break;
                    case 1:
                        args.index = 0;
                        args.prio = spr::SPR_PRIO_00;
                        break;
                    case 2:
                        args.index = 2;
                        args.prio = spr::SPR_PRIO_00;
                        break;
                    case 3:
                        args.index = 1;
                        args.prio = spr::SPR_PRIO_00;
                        break;
                    case 4:
                        args.index = 1;
                        args.prio = spr::SPR_PRIO_01;
                        break;
                    }
                    args.start_marker = 0;
                    args.end_marker = 0;
                    //args.color = bright_scale_get(aet_index_table[type], aet->bright_scale);
                    args.color = vec4(1.0f, 1.0f, 1.0f, env_aet_opacity);
                    args.spr_db = &spr_db;
                    aet_obj.init(args, &aet_db);

                    if (aet_obj.id)
                        aet_manager_set_obj_frame(aet_obj.id, frame);
                }
    }
    else {
        trans_duration = duration;
        trans_remain = duration;

        bool has_prev = false;
        for (int32_t type = 0; type < 5; type++)
            for (int32_t index = 0; index < 8; index++)
                if (data[this->env_index].data[type][index].id) {
                    aet.prev[type][index] = &data[this->env_index].data[type][index];
                    has_prev = true;
                }
                else
                    break;

        for (int32_t type = 0; type < 5; type++)
            for (int32_t index = 0; index < 8; index++)
                if (get_aet(env_index, type, index))
                    aet.next[type][index] = &data[env_index].data[type][index];
                else
                    break;

        if (has_prev) {
            aet.state = 1;
            aet.duration = duration * 0.5f;
        }
        else {
            aet.state = 2;
            aet.duration = duration;
        }
    }

    this->env_index = env_index;
}

void x_pv_game_stage_env::unload() {
    if (!state)
        return;

    if (aet_gam_stgpv_id_hash != hash_murmurhash_empty) {
        aet_manager_unload_set_modern(aet_gam_stgpv_id_hash, &aet_db);
        aet_gam_stgpv_id_hash = hash_murmurhash_empty;
    }

    if (spr_gam_stgpv_id_hash != hash_murmurhash_empty) {
        sprite_manager_unload_set_modern(spr_gam_stgpv_id_hash, &spr_db);
        spr_gam_stgpv_id_hash = hash_murmurhash_empty;
    }

    aet_gam_stgpv_id_main_hash = hash_murmurhash_empty;

    state = 0;
    stage_resource = 0;

    aet_db.clear();
    spr_db.clear();
}

void x_pv_game_stage_env::sub_810EE03E() {
    if (env_index < 0 || env_index >= 64)
        return;

    for (int32_t type = 0; type < 5; type++)
        for (int32_t index = 0; index < 8; index++) {
            aet_obj_data& aet_obj = data[env_index].data[type][index];
            if (!aet_obj.id || !aet_obj.check_disp())
                continue;

            AetArgs args;
            if (stage_resource && env_index >= 0 && env_index < 64) {
                pvsr_auth_2d* aet = get_aet(env_index, type, index);
                if (aet) {
                    args.id.id = aet_gam_stgpv_id_main_hash;
                    args.layer_name = aet->name.c_str();
                    args.flags = AET_PLAY_ONCE;
                    switch (type) {
                    case 0:
                        args.index = 0;
                        args.prio = spr::SPR_PRIO_01;
                        break;
                    case 1:
                        args.index = 0;
                        args.prio = spr::SPR_PRIO_00;
                        break;
                    case 2:
                        args.index = 2;
                        args.prio = spr::SPR_PRIO_00;
                        break;
                    case 3:
                        args.index = 1;
                        args.prio = spr::SPR_PRIO_00;
                        break;
                    case 4:
                        args.index = 1;
                        args.prio = spr::SPR_PRIO_01;
                        break;
                    }
                    args.start_marker = 0;
                    args.end_marker = 0;
                    //args.color = bright_scale_get(aet_index_table[type], aet->bright_scale);
                    args.color = vec4(1.0f, 1.0f, 1.0f, env_aet_opacity);
                }
            }
            args.spr_db = &spr_db;
            aet_obj.init(args, &aet_db);
        }
}

bool x_pv_game_stage_env::sub_810EE198(float_t delta_time) {
    if (trans_remain <= 0.0f)
        return false;

    trans_remain = max_def(trans_remain - delta_time, 0.0f);

    float_t t = 1.0f - trans_remain / trans_duration;
    switch (aet.state) {
    case 1: {
        float_t alpha = 1.0f - t * 2.0f;
        if (alpha > 0.0f) {
            for (int32_t type = 0; type < 5; type++)
                for (int32_t index = 0; index < 8; index++) {
                    aet_obj_data* aet_obj = aet.prev[type][index];
                    if (!aet_obj)
                        break;

                    aet_manager_set_obj_alpha(aet_obj->id, alpha * env_aet_opacity);
                }
            break;
        }

        for (int32_t type = 0; type < 5; type++)
            for (int32_t index = 0; index < 8; index++) {
                aet_obj_data* aet_obj = aet.prev[type][index];
                if (!aet_obj)
                    break;

                aet_obj->reset();
            }

        aet.state = 2;
    }
    case 2: {
        for (int32_t type = 0; type < 5; type++)
            for (int32_t index = 0; index < 8; index++) {
                aet_obj_data* aet_obj = aet.next[type][index];
                if (!aet_obj)
                    break;

                AetArgs args;
                if (stage_resource && env_index >= 0 && env_index < 64) {
                    pvsr_auth_2d* aet = get_aet(env_index, type, index);
                    if (aet) {
                        args.id.id = aet_gam_stgpv_id_main_hash;
                        args.layer_name = aet->name.c_str();
                        args.flags = AET_PLAY_ONCE;
                        switch (type) {
                        case 0:
                            args.index = 0;
                            args.prio = spr::SPR_PRIO_01;
                            break;
                        case 1:
                            args.index = 0;
                            args.prio = spr::SPR_PRIO_00;
                            break;
                        case 2:
                            args.index = 2;
                            args.prio = spr::SPR_PRIO_00;
                            break;
                        case 3:
                            args.index = 1;
                            args.prio = spr::SPR_PRIO_00;
                            break;
                        case 4:
                            args.index = 1;
                            args.prio = spr::SPR_PRIO_01;
                            break;
                        }
                        args.start_marker = 0;
                        args.end_marker = 0;
                        //args.color = bright_scale_get(aet_index_table[type], aet->bright_scale);
                        args.color = vec4(1.0f, 1.0f, 1.0f, env_aet_opacity);
                    }
                }
                args.spr_db = &spr_db;
                aet_obj->init(args, &aet_db);

                aet_manager_set_obj_alpha(aet_obj->id, 0.0f);
            }

        aet.state = 3;
    } break;
    case 3: {
        float_t alpha = 1.0f - trans_remain / aet.duration;
        for (int32_t type = 0; type < 5; type++)
            for (int32_t index = 0; index < 8; index++) {
                aet_obj_data* aet_obj = aet.next[type][index];
                if (!aet_obj)
                    break;

                aet_manager_set_obj_alpha(aet_obj->id, alpha * env_aet_opacity);
            }
    } break;
    }

    if (trans_remain <= 0.0f) {
        trans_duration = 0.0f;
        trans_remain = 0.0f;
        aet = {};
    }
    return false;
}
#pragma warning(pop)

x_pv_game_stage_effect_auth_3d::x_pv_game_stage_effect_auth_3d() : repeat(), field_1(), id() {
    reset();
}

void x_pv_game_stage_effect_auth_3d::reset() {
    repeat = false;
    field_1 = true;
    id = {};
}

x_pv_game_stage_effect_glitter::x_pv_game_stage_effect_glitter() :
    scene_counter(), fade_time(), fade_time_left(), force_disp() {
    reset();
}

void x_pv_game_stage_effect_glitter::reset() {
    name.clear();
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
            if (object_storage_get_objset_info(i)
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

    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "pv_stgpv%03d.stg", stage_id);
    file_handler.read_file(&data_list[DATA_X], "root+/stage/", buf);

    task_stage_modern_load_task("X_PV_STAGE");

    size_t len = sprintf_s(buf, sizeof(buf), "STGPV%03d.stg", stage_id);
    obj_hash.push_back(hash_murmurhash(buf, len));

    flags |= 0x02;
}

#if BAKE_X_PACK
void x_pv_game_stage_data::load_objects(int32_t stage_id, object_database* obj_db, texture_database* tex_db) {
#else
void x_pv_game_stage_data::load_objects(object_database* obj_db, texture_database* tex_db) {
#endif
    if (!(flags & 0x02) || file_handler.check_not_ready() || state && state != 2)
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

        std::string objhrc(i.name);
        objhrc.append("HRC");
        obj_hash.push_back(i.hash);
#if BAKE_X_PACK
        if (stage_id != 29)
            objhrc_hash.push_back(hash_string_murmurhash(objhrc));
#else
        objhrc_hash.push_back(hash_string_murmurhash(objhrc));
#endif
    }

    data_struct* x_data = &data_list[DATA_X];

    task_stage_modern_set_data(x_data, obj_db, tex_db, &stg_db);
    task_stage_modern_set_stage_hashes(obj_hash, stage_data);
    effect_manager_set_data(x_data, obj_db, tex_db, &stg_db);

    for (uint32_t& i : objhrc_hash)
        object_storage_load_set_hash(x_data, i);

    state = 1;
    flags |= 0x01;
}

void x_pv_game_stage_data::reset() {
    flags = 0x04;
    state = 0;
    file_handler.reset();
    stg_db.clear();
    frame_rate_control = 0;
    obj_hash.clear();
    obj_hash.shrink_to_fit();
    stage_info.clear();
    stage_info.shrink_to_fit();
    objhrc_hash.clear();
    objhrc_hash.shrink_to_fit();
}

void x_pv_game_stage_data::set_default_stage() {
    if (!(flags & 0x02) || state && state != 2)
        return;

    set_stage(hash_utf8_murmurhash("STGAETBACK"));
}

void x_pv_game_stage_data::set_stage(uint32_t hash) {
    if (!(flags & 0x02) || state && state != 2)
        return;

    if (flags & 0x01) {
        task_stage_modern_get_loaded_stage_infos(stage_info);
        flags &= ~0x01;
    }

    task_stage_modern_info stg_info;
    for (task_stage_modern_info& i : stage_info)
        if (i.check() && i.get_stage_hash() == hash) {
            stg_info = i;
            break;
        }

    stg_info.set_stage();
#if BAKE_VIDEO_ALPHA
    task_stage_modern_current_set_ground(false);
    task_stage_modern_current_set_sky(false);
    task_stage_modern_current_set_stage_display(false, false);
#else
    if (stg_info.check())
        stg_info.set_stage_display(!!(flags & 0x04), true);
    else
        task_stage_modern_current_set_stage_display(false, true);
#endif
}

void x_pv_game_stage_data::unload() {
    for (uint32_t& i : objhrc_hash)
        object_storage_unload_set(i);

    task_stage_modern_unload_task();

    flags &= ~0x02;
    state = 0;
    file_handler.reset();
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

#if BAKE_X_PACK
        if (stage_data.flags & 0x02)
            stage_data.load_objects(stage_id, &obj_db, &tex_db);
#else
        if (stage_data.flags & 0x02)
            stage_data.load_objects(&obj_db, &tex_db);
#endif

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
            char buf[0x200];
            size_t len = sprintf_s(buf, sizeof(buf), "EFFSTGPV%03d", stage_id);
            stage_auth_3d.push_back(std::string(buf, len));
        }

        data_struct* x_data = &data_list[DATA_X];

        for (pvsr_effect& i : sr->effect) {
            uint32_t hash = (uint32_t)Glitter::glt_particle_manager->LoadFile(Glitter::X,
                x_data, i.name.c_str(), 0, i.emission, true, &obj_db, &tex_db);
            if (hash != hash_murmurhash_empty)
                stage_glitter.push_back(hash);
        }

        for (string_hash& i : stage_auth_3d)
            auth_3d_data_load_category(x_data, i.c_str(), i.hash_murmurhash);

        env.load(stage_id, stage_resource);
        state = 11;
    } break;
    case 11: {
        if (stage_data.check_not_loaded())
            break;

        bool wait_load = false;

        for (string_hash& i : stage_auth_3d)
            if (!auth_3d_data_check_category_loaded(i.hash_murmurhash))
                wait_load |= true;

        for (uint32_t& i : stage_glitter)
            if (!Glitter::glt_particle_manager->CheckNoFileReaders(i))
                wait_load |= true;

        if (wait_load)
            break;

        data_struct* x_data = &data_list[DATA_X];

        for (pvsr_effect& i : stage_resource->effect)
            Glitter::glt_particle_manager->SetSceneName(i.name.hash_murmurhash, i.name.c_str());

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

                auto elem = auth_3d_ids.find(stg_eff_auth_3d.name.hash_murmurhash);
                if (elem == auth_3d_ids.end()) {
                    auth_3d_id id = auth_3d_id(stg_eff_auth_3d
                        .name.hash_murmurhash, x_data, &obj_db, &tex_db);
                    if (!id.check_not_empty()) {
                        eff_auth_3d.id = {};
                        continue;
                    }

                    id.read_file_modern();
                    id.set_enable(false);
                    id.set_repeat(true);
                    id.set_paused(false);
                    id.set_visibility(false);
                    id.set_frame_rate(&bpm_frame_rate_control);
                    eff_auth_3d.id = id;

                    auth_3d_ids.insert({ stg_eff_auth_3d.name.hash_murmurhash, id });
                }
                else
                    eff_auth_3d.id = elem->second;

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
                eff_glt.name.assign(stg_eff_glt.name);
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
        if (env.state && env.state != 20)
            break;

        bool wait_load = false;
        for (int32_t i = 0; i < X_PV_GAME_STAGE_EFFECT_COUNT; i++) {
            x_pv_game_stage_effect& eff = effect[i];
            for (x_pv_game_stage_effect_auth_3d& j : eff.auth_3d)
                if (j.id.check_not_empty() && !j.id.check_loaded())
                    wait_load |= true;
        }

        for (int32_t i = 0; i < X_PV_GAME_STAGE_EFFECT_COUNT; i++)
            for (int32_t j = 0; j < X_PV_GAME_STAGE_EFFECT_COUNT; j++) {
                x_pv_game_stage_change_effect& chg_eff = change_effect[i][j];
                for (x_pv_game_stage_effect_auth_3d& k : chg_eff.auth_3d)
                    if (k.id.check_not_empty() && !k.id.check_loaded())
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
        if (!task_stage_modern_check_task_ready()) {
            stage_data.stg_db.clear();
            state = 0;
        }
        break;
    }

    stage_data.ctrl(&obj_db, &tex_db);
    env.ctrl(delta_time);
}

void x_pv_game_stage::ctrl_inner() {
    if (next_stage_effect && curr_stage_effect) {
        x_pv_bar_beat* bar_beat = bpm_frame_rate_control.bar_beat;
        if (!bar_beat)
            return;

        bool change_stage_effect = bar_beat && fabsf(bar_beat->delta_time) > 0.000001f
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
            if (change_stage_effect) {
                if (set_change_effect_frame_part_1()) {
                    chg_eff.bars_left = chg_eff.bar_count;
                    stage_effect_transition_state = 1;
                    stop_stage_effect_auth_3d(curr_stage_effect);

                    set_change_effect_frame_part_2(bar_beat->get_bar_current_frame());
                }
                else {
                    stop_stage_effect_auth_3d(curr_stage_effect);
                    stop_stage_effect_glitter(curr_stage_effect);

                    set_stage_effect_auth_3d_frame(next_stage_effect, -1.0f);
#if !BAKE_VIDEO_ALPHA
                    set_stage_effect_glitter_frame(next_stage_effect, -1.0f);
#endif
                    curr_stage_effect = next_stage_effect;
                    next_stage_effect = 0;
                    stage_effect_transition_state = 0;
                }
            }
            else if (bar_beat->counter > 0) {
                std::vector<x_pv_game_stage_effect_auth_3d>& auth_3d = eff.auth_3d;
                for (x_pv_game_stage_effect_auth_3d& i : auth_3d) {
                    auth_3d_id& id = i.id;
                    if (i.field_1)
                        id.set_repeat(false);
                }
            }
        } break;
        case 1: {
            if (change_stage_effect) {
                chg_eff.bars_left -= 2;
                change_stage_effect = chg_eff.bars_left <= 0;
            }

            if (change_stage_effect) {
                stop_stage_change_effect();
                stop_stage_effect_glitter(curr_stage_effect);

                set_stage_effect_auth_3d_frame(next_stage_effect, -1.0f);
#if !BAKE_VIDEO_ALPHA
                set_stage_effect_glitter_frame(next_stage_effect, -1.0f);
#endif
                curr_stage_effect = next_stage_effect;
                next_stage_effect = 0;
                stage_effect_transition_state = 0;
            }
            else {
                float_t delta_time_bar_beat = bar_beat->delta_time / (bar_beat->next_bar_time - bar_beat->curr_bar_time);

                std::vector<x_pv_game_stage_effect_glitter>& glitter = eff.glitter;
                for (x_pv_game_stage_effect_glitter& i : glitter) {
                    if (fabsf(i.fade_time) <= 0.000001f)
                        Glitter::glt_particle_manager->SetSceneEffectExtColor(i.scene_counter, 0,
                            hash_murmurhash_empty, -1.0f, -1.0f, -1.0f, 0.0f);
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
        if (!(flags & (1 << i)))
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
            stop = auth_3d[eff.main_auth_3d_index].id.get_ended();

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

    char buf[0x200];
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

        auto elem = auth_3d_ids.find(stg_chg_eff_auth_3d.name.hash_murmurhash);
        if (elem == auth_3d_ids.end()) {
            auth_3d_id id = auth_3d_id(stg_chg_eff_auth_3d
                .name.hash_murmurhash, x_data, &obj_db, &tex_db);
            if (!id.check_not_empty()) {
                chg_eff_auth_3d.id = {};
                continue;
            }

            id.read_file_modern();
            id.set_enable(false);
            id.set_repeat(false);
            id.set_paused(false);
            id.set_visibility(false);
            id.set_frame_rate(&bpm_frame_rate_control);
            chg_eff_auth_3d.id = id;

            auth_3d_ids.insert({ stg_chg_eff_auth_3d.name.hash_murmurhash, id });
        }
        else
            chg_eff_auth_3d.id = elem->second;

        chg_eff_auth_3d.field_1 = true;
        chg_eff_auth_3d.repeat = !!(stg_chg_eff_auth_3d.flags & PVSR_AUTH_3D_REPEAT);
    }

    for (size_t i = 0; i < glitter_count; i++) {
        pvsr_glitter& stg_chg_eff_glt = stg_chg_eff.glitter[i];
        x_pv_game_stage_effect_glitter& chg_eff_glt = chg_eff.glitter[i];

        chg_eff_glt.reset();
        chg_eff_glt.name.assign(stg_chg_eff_glt.name);
        chg_eff_glt.force_disp = !!(stg_chg_eff_glt.flags & PVSR_GLITTER_FORCE_DISP);
    }
}

void x_pv_game_stage::reset() {
    flags = 0x01;
    stage_id = 0;
    field_8 = 0;
    state = 0;
    stage_resource_file_handler.reset();
    if (stage_resource) {
        delete stage_resource;
        stage_resource = 0;
    }
    bpm_frame_rate_control.reset();
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

    obj_db.clear();
    tex_db.clear();
    auth_3d_ids.clear();
}

void x_pv_game_stage::reset_stage_effect() {
    stop_stage_effect(true);

    for (int32_t i = 8; i <= 9; i++) {
        if (!(flags & (1 << i)))
            continue;

        stop_stage_effect_auth_3d(i);
        stop_stage_effect_glitter(i);
    }

    for (uint32_t& i : stage_glitter)
        Glitter::glt_particle_manager->FreeScene(i);
}

void x_pv_game_stage::reset_stage_env() {
    env.reset_env();
}

bool x_pv_game_stage::set_change_effect_frame_part_1() {
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
    return true;
}

void x_pv_game_stage::set_change_effect_frame_part_2(float_t frame) {
    if (!curr_stage_effect || !next_stage_effect)
        return;

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
        return;

    for (x_pv_game_stage_effect_auth_3d& i : chg_eff.auth_3d) {
        auth_3d_id& id = i.id;
        id.set_camera_root_update(false);
#if BAKE_VIDEO_ALPHA
        id.set_enable(false);
#else
        id.set_enable(true);
#endif
        id.set_repeat(i.repeat);
        id.set_req_frame(frame);
        id.set_max_frame(id.get_last_frame() - 1.0f);
        id.set_paused(false);
        id.set_visibility(!!(flags & 0x01));
        id.set_frame_rate(&bpm_frame_rate_control);
    }

#if !BAKE_VIDEO_ALPHA
    for (x_pv_game_stage_effect_glitter& i : chg_eff.glitter) {
        if (i.name.hash_murmurhash == hash_murmurhash_empty)
            continue;

        i.scene_counter = Glitter::glt_particle_manager->LoadSceneEffect(
            i.name.hash_murmurhash, i.name.c_str(), true, 0x01);
        if (!i.scene_counter)
            continue;

        Glitter::glt_particle_manager->SetSceneFrameRate(i.scene_counter, &bpm_frame_rate_control);
        Glitter::glt_particle_manager->SetSceneEffectReqFrame(i.scene_counter, frame);
    }
#endif
}

void x_pv_game_stage::set_env(int32_t env_index, float_t end_time, float_t start_time) {
    env.set(env_index, end_time, start_time);
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
#if !BAKE_VIDEO_ALPHA
        set_stage_effect_glitter_frame(stage_effect, 0.0f);
#endif
        flags |= 1 << stage_effect;
        return;
    }

    if (curr_stage_effect == stage_effect && !next_stage_effect || next_stage_effect)
        return;
    else if (curr_stage_effect) {
        if (!(flags & 0x10)) {
            next_stage_effect = stage_effect;
            stage_effect_transition_state = 0;

            if (((bpm_frame_rate_control.bar_beat->bar - 1) % 2)
                && curr_stage_effect >= 1 && curr_stage_effect <= 12) {
                std::vector<x_pv_game_stage_effect_auth_3d>& auth_3d = effect[curr_stage_effect - 1ULL].auth_3d;
                for (x_pv_game_stage_effect_auth_3d& i : auth_3d) {
                    auth_3d_id& id = i.id;
                    if (i.field_1)
                        id.set_repeat(false);
                }
            }
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
#if !BAKE_VIDEO_ALPHA
    set_stage_effect_glitter_frame(stage_effect, -1.0f);
#endif
}

void x_pv_game_stage::set_stage_effect_auth_3d_frame(int32_t stage_effect, float_t frame) {
    if (stage_effect < 1 || stage_effect > 12)
        return;

    if (frame < 0.0f)
        frame = bpm_frame_rate_control.bar_beat->get_bar_current_frame();

    std::vector<x_pv_game_stage_effect_auth_3d>& auth_3d = effect[stage_effect - 1ULL].auth_3d;
    for (x_pv_game_stage_effect_auth_3d& i : auth_3d) {
        auth_3d_id& id = i.id;
        id.set_camera_root_update(false);
#if BAKE_VIDEO_ALPHA
        id.set_enable(false);
#else
        id.set_enable(true);
#endif
        id.set_repeat(true);
        float_t last_frame = id.get_last_frame();
        if (i.repeat && last_frame <= frame) {
            int32_t frame_offset = (int32_t)id.get_frame_offset();
            int32_t _frame = frame_offset + (int32_t)(frame - (float_t)frame_offset) % (int32_t)last_frame;

            frame = (float_t)_frame + (frame - prj::floorf(frame));
        }
        id.set_req_frame(frame);
        id.set_max_frame(-1.0f);
        id.set_paused(false);
        id.set_visibility(true);
        id.set_frame_rate(&bpm_frame_rate_control);
    }
}

void x_pv_game_stage::set_stage_effect_glitter_frame(int32_t stage_effect, float_t frame) {
    if (stage_effect < 1 || stage_effect > 12)
        return;

    if (frame < 0.0f)
        frame = bpm_frame_rate_control.bar_beat->get_bar_current_frame();

    std::vector<x_pv_game_stage_effect_glitter>& glitter = effect[stage_effect - 1ULL].glitter;
    for (x_pv_game_stage_effect_glitter& i : glitter) {
        i.scene_counter = Glitter::glt_particle_manager->LoadSceneEffect(
            i.name.hash_murmurhash, i.name.c_str(), true, 0x01);
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
        auth_3d_id& id = i.id;
        id.set_visibility(false);
        id.set_enable(false);
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
        auth_3d_id& id = i.id;
        id.set_visibility(false);
        id.set_enable(false);
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

    for (std::pair<uint32_t, auth_3d_id> i : auth_3d_ids)
        i.second.unload(rctx_ptr);

    auth_3d_ids.clear();

    for (int32_t i = 0; i < X_PV_GAME_STAGE_EFFECT_COUNT; i++) {
        x_pv_game_stage_effect& eff = effect[i];
        for (x_pv_game_stage_effect_glitter& j : eff.glitter)
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

    for (uint32_t& i : stage_glitter)
        Glitter::glt_particle_manager->UnloadEffectGroup(i);

    stage_glitter.clear();

    for (string_hash& i : stage_auth_3d)
        auth_3d_data_unload_category(i.hash_murmurhash);

    stage_auth_3d.clear();

    env.unload();

    delete stage_resource;
    stage_resource = 0;

    stage_data.unload();

    stage_id = 0;

    state = 30;
}

x_pv_game::x_pv_game() : state(), pv_count(),  pv_index(), state_old(), frame(), frame_float(), time(),
rob_chara_ids(), success(), task_effect_init(), pause(), step_frame(), pv_id(), stage_id(), charas(), modules() {
    light_auth_3d_id = {};
    for (chara_index& i : charas)
        i = CHARA_MAX;
    for (int32_t& i : modules)
        i = 0;
}

x_pv_game::~x_pv_game() {

}

bool x_pv_game::init() {
    task_rob_manager_add_task();
    return true;
}

#if BAKE_PNG || BAKE_VIDEO
static bool img_write = false;
#endif

#if BAKE_VIDEO
extern ID3D11Device* d3d_device;
extern ID3D11DeviceContext* d3d_device_context;
extern HANDLE d3d_gl_handle;

const int32_t d3d_in_flight_num = 3;
bool d3d_tex_write[d3d_in_flight_num];
int32_t d3d_curr_tex = 0;
int32_t d3d_tex_in_queue = 0;

ID3D11Texture2D* d3d_texture[d3d_in_flight_num];

GLuint d3d_query[d3d_in_flight_num];
GLuint d3d_gl_fbo[d3d_in_flight_num];
GLuint d3d_gl_rbo[d3d_in_flight_num];
HANDLE d3d_gl_rbo_handle[d3d_in_flight_num];

#if BAKE_VIDEO_ALPHA
bool d3d_alpha_tex_write[d3d_in_flight_num];
int32_t d3d_curr_alpha_tex = 0;
int32_t d3d_alpha_tex_in_queue = 0;

ID3D11Texture2D* d3d_alpha_texture[d3d_in_flight_num];

GLuint d3d_alpha_query[d3d_in_flight_num];
GLuint d3d_gl_alpha_fbo[d3d_in_flight_num];
GLuint d3d_gl_alpha_rbo[d3d_in_flight_num];
HANDLE d3d_gl_alpha_rbo_handle[d3d_in_flight_num];
#endif

waitable_timer d3d_timer;

const size_t nvenc_src_pixel_size = 8;
const size_t nvenc_dst_pixel_size = 4;

std::vector<uint8_t> nvenc_temp_pixels;

nvenc_encoder* nvenc_enc;
file_stream* nvenc_stream;

#if BAKE_VIDEO_ALPHA
nvenc_encoder* nvenc_alpha_enc;
file_stream* nvenc_alpha_stream;
#endif
#endif

#if BAKE_DOF
static void a3da_key_rev(a3da_key& k, std::vector<float_t>& values_src) {
    std::vector<kft3> values;
    int32_t type = interpolate_chs_reverse_sequence(values_src, values);

    k = {};
    switch (type) {
    case 0:
    default:
        k.type = A3DA_KEY_NONE;
        return;
    case 1:
        k.value = values[0].value;
        k.type = A3DA_KEY_STATIC;
        return;
    case 2:
        k.type = A3DA_KEY_LINEAR;
        break;
    case 3:
        k.type = A3DA_KEY_HERMITE;
        break;
    }

    k.keys.assign(values.begin(), values.end());
    k.max_frame = (float_t)(values_src.size() + 1);
}
#endif

#if BAKE_PNG || BAKE_VIDEO
static int32_t frame_prev = -1;
#endif

bool x_pv_game::ctrl() {
#if BAKE_VIDEO
    auto write_frame = [&](int32_t idx) {
        int32_t res = 0;
        glGetQueryObjectiv(d3d_query[idx], GL_QUERY_RESULT_AVAILABLE, &res);
        if (res) {
            nvenc_enc->write_frame(d3d_texture[idx], nvenc_stream);
            nvenc_stream->flush();
            d3d_tex_in_queue--;
            d3d_tex_write[idx] = false;
        }
    };

#if BAKE_VIDEO_ALPHA
    auto write_alpha_frame = [&](int32_t idx) {
        int32_t res = 0;
        glGetQueryObjectiv(d3d_alpha_query[idx], GL_QUERY_RESULT_AVAILABLE, &res);
        if (res) {
            nvenc_alpha_enc->write_frame(d3d_alpha_texture[idx], nvenc_alpha_stream);
            nvenc_alpha_stream->flush();
            d3d_alpha_tex_in_queue--;
            d3d_alpha_tex_write[idx] = false;
        }
    };
#endif

    if (GLAD_WGL_NV_DX_interop2 && d3d_tex_in_queue) {
        if (frame_prev == x_pv_game_ptr->frame)
            for (int32_t i = 0; i < d3d_in_flight_num && !d3d_tex_write[d3d_curr_tex]; i++)
                d3d_curr_tex = (d3d_curr_tex + 1) % d3d_in_flight_num;

        if (d3d_tex_write[d3d_curr_tex])
            write_frame(d3d_curr_tex);
    }

#if BAKE_VIDEO_ALPHA
    if (GLAD_WGL_NV_DX_interop2 && d3d_alpha_tex_in_queue) {
        if (frame_prev == x_pv_game_ptr->frame)
            for (int32_t i = 0; i < d3d_in_flight_num && !d3d_alpha_tex_write[d3d_curr_alpha_tex]; i++)
                d3d_curr_alpha_tex = (d3d_curr_alpha_tex + 1) % d3d_in_flight_num;

        if (d3d_alpha_tex_write[d3d_curr_alpha_tex])
            write_alpha_frame(d3d_curr_alpha_tex);
    }
#endif
#endif

#if BAKE_PNG || BAKE_VIDEO
    if (img_write && frame_prev != x_pv_game_ptr->frame) {
        texture* tex = rctx_ptr->screen_buffer.color_texture;
        int32_t width = tex->width;
        int32_t height = tex->height;

#if BAKE_VIDEO
        if (GLAD_WGL_NV_DX_interop2) {
            int32_t idx = (d3d_curr_tex + 1) % d3d_in_flight_num;
            d3d_curr_tex = idx;
            int32_t next_idx = (idx + 2) % d3d_in_flight_num;

            while (d3d_tex_write[next_idx]) {
                d3d_timer.sleep_float(1.0);
                write_frame(next_idx);
            }

            wglDXLockObjectsNV(d3d_gl_handle, 1, &d3d_gl_rbo_handle[next_idx]);

            gl_state_bind_framebuffer(d3d_gl_fbo[next_idx]);
            gl_state_active_bind_texture_2d(0, tex->tex);
            glViewport(0, 0, width, height);
            uniform_value[U_REDUCE] = 0;
            shaders_ft.set(SHADER_FT_REDUCE);
            glBeginQuery(GL_SAMPLES_PASSED, d3d_query[next_idx]);
            rctx_ptr->render.draw_quad(width, height,
                1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
            glEndQuery(GL_SAMPLES_PASSED);
            gl_state_bind_texture_2d(0);
            gl_state_bind_framebuffer(0);

            wglDXUnlockObjectsNV(d3d_gl_handle, 1, &d3d_gl_rbo_handle[next_idx]);

            d3d_tex_in_queue++;
            d3d_tex_write[next_idx] = true;

#if BAKE_VIDEO_ALPHA
            int32_t alpha_idx = (d3d_curr_alpha_tex + 1) % d3d_in_flight_num;
            d3d_curr_alpha_tex = alpha_idx;
            int32_t next_alpha_idx = (alpha_idx + 2) % d3d_in_flight_num;

            while (d3d_alpha_tex_write[next_alpha_idx]) {
                d3d_timer.sleep_float(1.0);
                write_alpha_frame(next_alpha_idx);
            }

            wglDXLockObjectsNV(d3d_gl_handle, 1, &d3d_gl_alpha_rbo_handle[next_alpha_idx]);

            gl_state_bind_framebuffer(d3d_gl_alpha_fbo[next_alpha_idx]);
            gl_state_active_bind_texture_2d(0, tex->tex);

            GLint swizzle_aaa1[] = { GL_ALPHA, GL_ALPHA, GL_ALPHA, GL_ONE };
            glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle_aaa1);

            glViewport(0, 0, width, height);
            uniform_value[U_REDUCE] = 0;
            shaders_ft.set(SHADER_FT_REDUCE);
            glBeginQuery(GL_SAMPLES_PASSED, d3d_alpha_query[next_alpha_idx]);
            rctx_ptr->render.draw_quad(width, height,
                1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
            glEndQuery(GL_SAMPLES_PASSED);

            GLint swizzle_rgba[] = { GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA };
            glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle_rgba);

            gl_state_bind_texture_2d(0);
            gl_state_bind_framebuffer(0);

            wglDXUnlockObjectsNV(d3d_gl_handle, 1, &d3d_gl_alpha_rbo_handle[next_alpha_idx]);

            d3d_alpha_tex_in_queue++;
            d3d_alpha_tex_write[next_alpha_idx] = true;
            #endif
        }
        else {
#if BAKE_VIDEO_ALPHA
            D3D11_MAPPED_SUBRESOURCE mapped_res = {};
            HRESULT result = d3d_device_context->Map(d3d_texture[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_res);
            D3D11_MAPPED_SUBRESOURCE mapped_alpha_res = {};
            HRESULT result_alpha = d3d_device_context->Map(d3d_alpha_texture[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_alpha_res);
            if (SUCCEEDED(result) && SUCCEEDED(result_alpha)) {
                gl_state_bind_texture_2d(tex->tex);
                glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_HALF_FLOAT, nvenc_temp_pixels.data());
                gl_state_bind_texture_2d(0);

                uint8_t* src = (uint8_t*)nvenc_temp_pixels.data();
                uint8_t* dst = (uint8_t*)mapped_res.pData;
                uint8_t* dst_alpha = (uint8_t*)mapped_alpha_res.pData;

                __m128 rgb_f32;
                vec4i rgb_u10;

                extern bool f16c;
                if (f16c)
                    for (size_t y = 0, i = height; i; y++, i--) {
                        uint8_t* src1 = &src[y * width * nvenc_src_pixel_size];
                        uint8_t* dst1 = &dst[(height - y - 1) * width * nvenc_dst_pixel_size];
                        uint8_t* dst1_alpha = &dst_alpha[(height - y - 1) * width * nvenc_dst_pixel_size];

                        for (size_t x = 0, j = width; j; x++, j--, src1 += nvenc_src_pixel_size,
                            dst1 += nvenc_dst_pixel_size, dst1_alpha += nvenc_dst_pixel_size) {
                            rgb_f32 = _mm_cvtph_ps(_mm_loadl_epi64((__m128i*)src1));
                            rgb_f32 = _mm_min_ps(_mm_max_ps(rgb_f32, vec4::load_xmm(0.0f)), vec4::load_xmm(1.0f));
                            rgb_f32 = _mm_mul_ps(rgb_f32, vec4::load_xmm((float_t)((1 << 10) - 1)));
                            rgb_u10 = vec4i::store_xmm(_mm_cvtps_epi32(rgb_f32));
                            *(uint32_t*)dst1 = (uint32_t)rgb_u10.x | ((uint32_t)rgb_u10.y << 10)
                                | ((uint32_t)rgb_u10.z << 20) | (0x03 << 30);
                            *(uint32_t*)dst1_alpha = (uint32_t)rgb_u10.w | ((uint32_t)rgb_u10.w << 10)
                                | ((uint32_t)rgb_u10.w << 20) | (0x03 << 30);
                        }
                    }
                else
                    for (size_t y = 0, i = height; i; y++, i--) {
                        uint8_t* src1 = &src[y * width * nvenc_src_pixel_size];
                        uint8_t* dst1 = &dst[(height - y - 1) * width * nvenc_dst_pixel_size];
                        uint8_t* dst1_alpha = &dst_alpha[(height - y - 1) * width * nvenc_dst_pixel_size];

                        for (size_t x = 0, j = width; j; x++, j--, src1 += nvenc_src_pixel_size,
                            dst1 += nvenc_dst_pixel_size, dst1_alpha += nvenc_dst_pixel_size) {
                            rgb_f32.m128_f32[0] = half_to_float_convert(((half_t*)src1)[0]);
                            rgb_f32.m128_f32[1] = half_to_float_convert(((half_t*)src1)[1]);
                            rgb_f32.m128_f32[2] = half_to_float_convert(((half_t*)src1)[2]);
                            rgb_f32.m128_f32[3] = half_to_float_convert(((half_t*)src1)[3]);
                            rgb_f32 = _mm_min_ps(_mm_max_ps(rgb_f32, vec4::load_xmm(0.0f)), vec4::load_xmm(1.0f));
                            rgb_f32 = _mm_mul_ps(rgb_f32, vec4::load_xmm((float_t)((1 << 10) - 1)));
                            rgb_u10 = vec4i::store_xmm(_mm_cvtps_epi32(rgb_f32));
                            *(uint32_t*)dst1 = (uint32_t)rgb_u10.x | ((uint32_t)rgb_u10.y << 10)
                                | ((uint32_t)rgb_u10.z << 20) | (0x03 << 30);
                            *(uint32_t*)dst1_alpha = (uint32_t)rgb_u10.w | ((uint32_t)rgb_u10.w << 10)
                                | ((uint32_t)rgb_u10.w << 20) | (0x03 << 30);
                        }
                    }

                d3d_device_context->Unmap(d3d_texture[0], 0);
                d3d_device_context->Unmap(d3d_alpha_texture[0], 0);
                nvenc_enc->write_frame(d3d_texture[0], nvenc_stream);
                nvenc_alpha_enc->write_frame(d3d_alpha_texture[0], nvenc_alpha_stream);
                nvenc_stream->flush();
                nvenc_alpha_stream->flush();
            }
            else {
                if (SUCCEEDED(result))
                    d3d_device_context->Unmap(d3d_texture[0], 0);
                if (SUCCEEDED(result_alpha))
                    d3d_device_context->Unmap(d3d_alpha_texture[0], 0);
            }
#else
            D3D11_MAPPED_SUBRESOURCE mapped_res = {};
            if (SUCCEEDED(d3d_device_context->Map(d3d_texture[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_res))) {
                gl_state_bind_texture_2d(tex->tex);
                glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_HALF_FLOAT, nvenc_temp_pixels.data());
                gl_state_bind_texture_2d(0);

                uint8_t* src = (uint8_t*)nvenc_temp_pixels.data();
                uint8_t* dst = (uint8_t*)mapped_res.pData;

                __m128 rgb_f32;
                vec4i rgb_u10;

                extern bool f16c;
                if (f16c)
                    for (size_t y = 0, i = height; i; y++, i--) {
                        uint8_t* src1 = &src[y * width * nvenc_src_pixel_size];
                        uint8_t* dst1 = &dst[(height - y - 1) * width * nvenc_dst_pixel_size];

                        for (size_t x = 0, j = width; j; x++, j--, src1 += nvenc_src_pixel_size, dst1 += nvenc_dst_pixel_size) {
                            rgb_f32 = _mm_cvtph_ps(_mm_loadl_epi64((__m128i*)src1));
                            rgb_f32 = _mm_min_ps(_mm_max_ps(rgb_f32, vec4::load_xmm(0.0f)), vec4::load_xmm(1.0f));
                            rgb_f32 = _mm_mul_ps(rgb_f32, vec4::load_xmm((float_t)((1 << 10) - 1)));
                            rgb_u10 = vec4i::store_xmm(_mm_cvtps_epi32(rgb_f32));
                            *(uint32_t*)dst1 = (uint32_t)rgb_u10.x | ((uint32_t)rgb_u10.y << 10)
                                | ((uint32_t)rgb_u10.z << 20) | (0x03 << 30);
                        }
                    }
                else
                    for (size_t y = 0, i = height; i; y++, i--) {
                        uint8_t* src1 = &src[y * width * nvenc_src_pixel_size];
                        uint8_t* dst1 = &dst[(height - y - 1) * width * nvenc_dst_pixel_size];

                        for (size_t x = 0, j = width; j; x++, j--, src1 += nvenc_src_pixel_size, dst1 += nvenc_dst_pixel_size) {
                            rgb_f32.m128_f32[0] = half_to_float_convert(((half_t*)src1)[0]);
                            rgb_f32.m128_f32[1] = half_to_float_convert(((half_t*)src1)[1]);
                            rgb_f32.m128_f32[2] = half_to_float_convert(((half_t*)src1)[2]);
                            rgb_f32.m128_f32[3] = half_to_float_convert(((half_t*)src1)[3]);
                            rgb_f32 = _mm_min_ps(_mm_max_ps(rgb_f32, vec4::load_xmm(0.0f)), vec4::load_xmm(1.0f));
                            rgb_f32 = _mm_mul_ps(rgb_f32, vec4::load_xmm((float_t)((1 << 10) - 1)));
                            rgb_u10 = vec4i::store_xmm(_mm_cvtps_epi32(rgb_f32));
                            *(uint32_t*)dst1 = (uint32_t)rgb_u10.x | ((uint32_t)rgb_u10.y << 10)
                                | ((uint32_t)rgb_u10.z << 20) | (0x03 << 30);
                        }
                    }

                d3d_device_context->Unmap(d3d_texture[0], 0);
                nvenc_enc->write_frame(d3d_texture[0], nvenc_stream);
                nvenc_stream->flush();
            }
#endif
        }
#elif BAKE_PNG
        std::vector<uint8_t> temp_pixels;
        temp_pixels.resize((size_t)width * (size_t)height * 4 * sizeof(uint8_t));
        gl_state_bind_texture_2d(tex->tex);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, temp_pixels.data());
        gl_state_bind_texture_2d(0);

        std::vector<uint8_t> pixels;
        pixels.resize((size_t)width * (size_t)height * 4 * sizeof(uint8_t));

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
            sprintf_s(buf, sizeof(buf), "G:\\ReDIVA\\Photos\\pv%03d",
                x_pv_game_ptr->get_data().pv_id);
            CreateDirectoryA(buf, 0);

            sprintf_s(buf, sizeof(buf), "G:\\ReDIVA\\Photos\\pv%03d\\%05d.png",
                x_pv_game_ptr->get_data().pv_id, x_pv_game_ptr->frame);
            lodepng::save_file(png, buf);
        }
#endif

        frame_prev = x_pv_game_ptr->frame;
        img_write = false;
    }
#endif

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

        get_data().pv_id = pv_id;

        char buf[0x200];
        for (int32_t i = 0; i < pv_count; i++) {
            x_pv_game_data& pv_data = this->data[i];

            sprintf_s(buf, sizeof(buf), "pv%03d.pvpp", pv_data.pv_id);
            pv_data.play_param_file_handler.read_file(&data_list[DATA_X], "root+/pv/", buf);
            pv_data.stage = &stage_data;
        }

        state_old = 2;
    } break;
    case 2: {
        bool wait_load = false;
        for (int32_t i = 0; i < pv_count; i++)
            if (data[i].play_param_file_handler.check_not_ready())
                wait_load |= true;

        if (wait_load)
            break;

        stage_data.load(stage_id, &field_71994, false);

        for (int32_t i = 0; i < pv_count; i++) {
            x_pv_game_data& pv_data = this->data[i];

            const void* pvpp_data = pv_data.play_param_file_handler.get_data();
            size_t pvpp_size = pv_data.play_param_file_handler.get_size();

            pv_data.play_param = new pvpp;
            pv_data.play_param->read(pvpp_data, pvpp_size);

#if BAKE_PV826
            pv_data.load(pv_id, &field_71994, charas,
                pv_id == 826 ? &effchrpv_auth_3d_mot_names : 0);
#else
            pv_data.load(pv_id, &field_71994, charas);
#endif

            int32_t chara_index = 0;
            for (pvpp_chara& i : pv_data.play_param->chara) {
                if (i.motion.size() && rob_chara_ids[chara_index] == -1) {
                    rob_chara_pv_data pv_data;
                    pv_data.chara_size_index = chara_init_data_get_chara_size_index(charas[chara_index]);
                    rob_sleeve_handler_data_get_sleeve_data(
                        charas[chara_index], modules[chara_index] + 1, pv_data.sleeve_l, pv_data.sleeve_r);
                    int32_t chara_id = rob_chara_array_init_chara_index(
                        charas[chara_index], pv_data, modules[chara_index], true);
                    if (chara_id >= 0 && chara_id < ROB_CHARA_COUNT)
                        rob_chara_ids[chara_index] = chara_id;
                }

                if (++chara_index >= ROB_CHARA_COUNT)
                    break;
            }

            if (pv_data.play_param->chara.size() < 2)
                rctx_ptr->render.update_res(false, 1);
            else
                rctx_ptr->render.update_res(false, 2);

#if BAKE_PV826
            if (pv_id == 826) {
                for (int32_t i = (int32_t)pv_data.play_param->chara.size(); i < ROB_CHARA_COUNT; i++) {
                    rob_chara_pv_data pv_data;
                    pv_data.chara_size_index = chara_init_data_get_chara_size_index(charas[i]);
                    int32_t chara_id = rob_chara_array_init_chara_index(charas[i], pv_data, modules[i], true);
                    if (chara_id >= 0 && chara_id < ROB_CHARA_COUNT)
                        rob_chara_ids[i] = chara_id;
                    effchrpv_rob_mot_ids.push_back(chara_id);
                }

                char buf[0x100];
                pv_data.play_param->chara.resize(6);
                pvpp_chara* chara = pv_data.play_param->chara.data();
                for (int32_t i = 1; i < 6; i++) {
                    switch (i) {
                    case 1:
                        chara[i].chara_effect.base_chara = PVPP_CHARA_RIN;
                        chara[i].chara_effect_init = true;
                        break;
                    case 2:
                        chara[i].chara_effect.base_chara = PVPP_CHARA_LEN;
                        chara[i].chara_effect_init = true;
                        break;
                    case 3:
                        chara[i].chara_effect.base_chara = PVPP_CHARA_LUKA;
                        chara[i].chara_effect_init = true;
                        break;
                    case 4:
                        chara[i].chara_effect.base_chara = PVPP_CHARA_MEIKO;
                        chara[i].chara_effect_init = true;
                        break;
                    case 5:
                        chara[i].chara_effect.base_chara = PVPP_CHARA_KAITO;
                        chara[i].chara_effect_init = true;
                        break;
                    }
                    sprintf_s(buf, sizeof(buf), "PV%03d_%s_P%d_00", 826, "OST", i + 1);

                    chara[i].motion.push_back(buf);
                }
            }
#endif
        }

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

        light_param_data_storage_data_set_pv_id(get_data().pv_id == 832 ? 800 : get_data().pv_id);

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

        light_auth_3d_id = {};
        {
            data_struct* aft_data = &data_list[DATA_AFT];
            auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;

            char buf[0x200];
            sprintf_s(buf, sizeof(buf), "STGPV%03d_EFF_LT_000",
                get_data().pv_id == 832 ? 800 : get_data().pv_id);
            uint32_t light_auth_3d_hash = hash_utf8_murmurhash(buf);
            int32_t uid = aft_auth_3d_db->get_uid(buf);
            if (uid != -1) {
                light_auth_3d_id = auth_3d_id(uid, aft_auth_3d_db);
                if (light_auth_3d_id.check_not_empty()) {
                    light_auth_3d_id.read_file(aft_auth_3d_db);
                    light_auth_3d_id.set_enable(false);
                    light_auth_3d_id.set_visibility(false);
                }
                else
                    light_auth_3d_id = {};
            }
        }

        state_old = 8;
    } break;
    case 8: {
        bool wait_load = false;

        if (light_auth_3d_id.check_not_empty() && !light_auth_3d_id.check_loaded())
            wait_load |= true;

        if (wait_load)
            break;

        x_pv_game_pv_data& pv_data = get_data().pv_data;

        pv_param_task::post_process_task_add_task();
        {
            int32_t pv_id = get_data().pv_id;

            data_struct* x_data = &data_list[DATA_X];

            dsc dsc_mouth;
            dsc dsc_scene;
            dsc dsc_system;
            dsc dsc_easy;

            char path_buf[0x200];
            char file_buf[0x200];

            farc dsc_common_farc;
            sprintf_s(path_buf, sizeof(path_buf), "root+/pv_script/pv%03d/", pv_id);
            sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_common.farc", pv_id);
            x_data->load_file(&dsc_common_farc, path_buf, file_buf, farc::load_file);

            sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_mouth.dsc", pv_id);
            farc_file* dsc_mouth_ff = dsc_common_farc.read_file(file_buf);
            if (dsc_mouth_ff)
                dsc_mouth.parse(dsc_mouth_ff->data, dsc_mouth_ff->size, DSC_X);

            sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_scene.dsc", pv_id);
            farc_file* dsc_scene_ff = dsc_common_farc.read_file(file_buf);
            if (dsc_scene_ff)
                dsc_scene.parse(dsc_scene_ff->data, dsc_scene_ff->size, DSC_X);

            sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_system.dsc", pv_id);
            farc_file* dsc_system_ff = dsc_common_farc.read_file(file_buf);
            if (dsc_system_ff)
                dsc_system.parse(dsc_system_ff->data, dsc_system_ff->size, DSC_X);

            sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_easy.dsc", pv_id);
            dsc_easy.type = DSC_X;
            x_data->load_file(&dsc_easy, path_buf, file_buf, dsc::load_file);

#if BAKE_PV826
            if (pv_id == 826) {
                dsc dsc_hand_keys[5];

                char file_buf[0x200];
                for (int32_t i = 2; i <= 6; i++) {
                    file_stream s;
                    sprintf_s(file_buf, sizeof(file_buf), "pv826\\pv_826_hand_%d.txt", i);
                    s.open(file_buf, "rb");
                    size_t length = s.length;
                    uint8_t* data = force_malloc<uint8_t>(length);
                    s.read(data, length);
                    s.close();

                    dsc& d = dsc_hand_keys[i - 2];
                    d.parse_text(data, length, DSC_X);
                    d.signature = 0x13120420;
                    free(data);

                    {
                        int32_t hand_l_time = -1;
                        int32_t hand_l_id = -1;
                        int32_t hand_l_duration = -1;
                        dsc_data* hand_l_data = 0;
                        int32_t time = -1;
                        for (dsc_data& j : d.data) {
                            if (j.func == DSC_X_END)
                                break;

                            int32_t* data = d.get_func_data(&j);
                            switch (j.func) {
                            case DSC_X_TIME:
                                time = data[0];
                                break;
                            case DSC_X_HAND_ANIM: {
                                if (data[1] == 0) {
                                    if (hand_l_time > -1 && hand_l_duration > time - hand_l_time)
                                        data[3] = (time - hand_l_time) / 100;

                                    hand_l_time = time;
                                    hand_l_id = data[2];
                                    hand_l_duration = data[3] * 100;
                                    hand_l_data = &j;
                                }
                            } break;
                            }
                        }
                    }

                    {
                        int32_t hand_r_time = -1;
                        int32_t hand_r_id = -1;
                        int32_t hand_r_duration = -1;
                        dsc_data* hand_r_data = 0;
                        int32_t time = -1;
                        for (dsc_data& j : d.data) {
                            if (j.func == DSC_X_END)
                                break;

                            int32_t* data = d.get_func_data(&j);
                            switch (j.func) {
                            case DSC_X_TIME:
                                time = data[0];
                                break;
                            case DSC_X_HAND_ANIM: {
                                if (data[1] == 1) {
                                    if (hand_r_time > -1 && hand_r_duration > time - hand_r_time)
                                        data[3] = (time - hand_r_time) / 100;

                                    hand_r_time = time;
                                    hand_r_id = data[2];
                                    hand_r_duration = data[3] * 100;
                                    hand_r_data = &j;
                                }
                            } break;
                            }
                        }
                    }
                }

                pv_data.dsc.merge(9, &dsc_mouth, &dsc_scene, &dsc_system, &dsc_easy,
                    &dsc_hand_keys[0], &dsc_hand_keys[1], &dsc_hand_keys[2], &dsc_hand_keys[3], dsc_hand_keys[4]);
            }
            else
#endif
                pv_data.dsc.merge(4, &dsc_mouth, &dsc_scene, &dsc_system, &dsc_easy);

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

            prj::vector_pair<int32_t, uint32_t> miku_disp;
            prj::vector_pair<int32_t, uint32_t> set_motion;
            prj::vector_pair<int32_t, uint32_t> set_playdata;

            int32_t time = -1;
            int32_t frame = -1;
            bool hand_anim_set[ROB_CHARA_COUNT] = {};
            int32_t hand_anim_set_time[ROB_CHARA_COUNT] = {};
            for (dsc_data& i : pv_data.dsc.data) {
                if (i.func == DSC_X_END)
                    break;

                int32_t* data = pv_data.dsc.get_func_data(&i);
                switch (i.func) {
                case DSC_X_TIME: {
                    for (miku_state& i : state)
                        if (state[get_data().pv_data.chara_id].disp
                            && i.disp_time == time && i.set_motion_time != time)
                            state_vec.push_back(i);
                    time = data[0];
                    frame = (int32_t)prj::roundf((float_t)time * (float_t)(60.0 / 100000.0));
                } break;
                case DSC_X_MIKU_DISP: {
                    int32_t chara_id = data[0];
                    state[chara_id].disp = data[1] == 1;
                    if (state[chara_id].disp)
                        state[chara_id].disp_time = time;
                    miku_disp.push_back(time, i.data_offset);

                    if (!hand_anim_set[chara_id]) {
                        hand_anim_set_time[chara_id] = time;
                        hand_anim_set[chara_id] = true;
                    }
                } break;
                case DSC_X_SET_MOTION: {
                    int32_t chara_id = data[0];
                    state[chara_id].set_motion_time = time;
                    state[chara_id].set_motion_data_offset = i.data_offset;
                    set_motion.push_back(time, i.data_offset);
                } break;
                case DSC_X_SET_PLAYDATA: {
                    set_playdata.push_back(time, i.data_offset);
                } break;
                }
            }

            for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
                if (!hand_anim_set[i])
                    continue;

                int32_t* data = pv_data.dsc.add_func(dsc_x_get_func_name(DSC_X_TIME),
                    DSC_X_TIME, dsc_x_get_func_length(DSC_X_TIME));
                data[0] = hand_anim_set_time[i];

                int32_t* new_hand_anim_l_data = pv_data.dsc.add_func(dsc_x_get_func_name(DSC_X_HAND_ANIM),
                    DSC_X_HAND_ANIM, dsc_x_get_func_length(DSC_X_HAND_ANIM));
                new_hand_anim_l_data[0] = i;
                new_hand_anim_l_data[1] = 0;
                new_hand_anim_l_data[2] = 9;
                new_hand_anim_l_data[3] = -1;
                new_hand_anim_l_data[4] = -1;

                int32_t* new_hand_anim_r_data = pv_data.dsc.add_func(dsc_x_get_func_name(DSC_X_HAND_ANIM),
                    DSC_X_HAND_ANIM, dsc_x_get_func_length(DSC_X_HAND_ANIM));
                new_hand_anim_r_data[0] = i;
                new_hand_anim_r_data[1] = 1;
                new_hand_anim_r_data[2] = 9;
                new_hand_anim_r_data[3] = -1;
                new_hand_anim_r_data[4] = -1;
            }

            if (state_vec.size()) {
                int32_t time = -1;
                for (miku_state& i : state_vec) {
                    if (i.disp_time != time) {
                        int32_t* data = pv_data.dsc.add_func(dsc_x_get_func_name(DSC_X_TIME),
                            DSC_X_TIME, dsc_x_get_func_length(DSC_X_TIME));
                        data[0] = i.disp_time;
                        time = i.disp_time;
                    }

                    int32_t* new_data = pv_data.dsc.add_func(dsc_x_get_func_name(DSC_X_SET_MOTION),
                        DSC_X_SET_MOTION, dsc_x_get_func_length(DSC_X_SET_MOTION));
                    int32_t* data = pv_data.dsc.data_buffer.data() + i.set_motion_data_offset;
                    memcpy(new_data, data, sizeof(int32_t) * dsc_x_get_func_length(DSC_X_SET_MOTION));
                }

                pv_data.dsc.rebuild();
            }

#if BAKE_PV826
            if (pv_id == 826 && (miku_disp.size() || set_motion.size() || set_playdata.size())) {
                time = -1;
                for (std::pair<int32_t, uint32_t>& i : miku_disp) {
                    if (i.first != time) {
                        int32_t* data = pv_data.dsc.add_func(dsc_x_get_func_name(DSC_X_TIME),
                            DSC_X_TIME, dsc_x_get_func_length(DSC_X_TIME));
                        data[0] = i.first;
                        time = i.first;
                    }

                    for (int32_t j = 1; j < 6; j++) {
                        int32_t* new_data = pv_data.dsc.add_func(dsc_x_get_func_name(DSC_X_MIKU_DISP),
                            DSC_X_MIKU_DISP, dsc_x_get_func_length(DSC_X_MIKU_DISP));
                        int32_t* data = pv_data.dsc.data_buffer.data() + i.second;
                        memcpy(new_data, data, sizeof(int32_t) * dsc_x_get_func_length(DSC_X_MIKU_DISP));
                        new_data[0] = j;
                    }
                }

                time = -1;
                for (std::pair<int32_t, uint32_t>& i : set_motion) {
                    if (i.first != time) {
                        int32_t* data = pv_data.dsc.add_func(dsc_x_get_func_name(DSC_X_TIME),
                            DSC_X_TIME, dsc_x_get_func_length(DSC_X_TIME));
                        data[0] = i.first;
                        time = i.first;
                    }

                    for (int32_t j = 1; j < 6; j++) {
                        int32_t* new_data = pv_data.dsc.add_func(dsc_x_get_func_name(DSC_X_SET_MOTION),
                            DSC_X_SET_MOTION, dsc_x_get_func_length(DSC_X_SET_MOTION));
                        int32_t* data = pv_data.dsc.data_buffer.data() + i.second;
                        memcpy(new_data, data, sizeof(int32_t) * dsc_x_get_func_length(DSC_X_SET_MOTION));
                        new_data[0] = j;
                    }
                }

                time = -1;
                for (std::pair<int32_t, uint32_t>& i : set_playdata) {
                    if (i.first != time) {
                        int32_t* data = pv_data.dsc.add_func(dsc_x_get_func_name(DSC_X_TIME),
                            DSC_X_TIME, dsc_x_get_func_length(DSC_X_TIME));
                        data[0] = i.first;
                        time = i.first;
                    }

                    for (int32_t j = 1; j < 6; j++) {
                        int32_t* new_data = pv_data.dsc.add_func(dsc_x_get_func_name(DSC_X_SET_PLAYDATA),
                            DSC_X_SET_PLAYDATA, dsc_x_get_func_length(DSC_X_SET_PLAYDATA));
                        int32_t* data = pv_data.dsc.data_buffer.data() + i.second;
                        memcpy(new_data, data, sizeof(int32_t) * dsc_x_get_func_length(DSC_X_SET_PLAYDATA));
                        new_data[0] = j;
                    }
                }

                pv_data.dsc.rebuild();
            }
#endif

            /*sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_easy.dsc", pv_data.pv_id);
            void* data = 0;
            size_t size = 0;
            dsc.unparse(&data, &size);
            stream s;
            s.open(file_buf, "wb");
            s.write(data, size);
            free_def(data);*/
        }

        get_data().pv_data.init(this, true);

        Shadow* shad = shadow_ptr_get();
        if (shad) {
            shad->blur_filter_enable[0] = true;
            shad->blur_filter_enable[1] = true;
        }

        state_old = 9;
    } break;
    case 9: {
        if (skin_param_manager_array_check_task_ready())
            break;

        bool wait_load = false;

        for (int32_t i = 0; i < pv_count; i++)
            if (!data[i].state || data[i].state == 10) {
                data[i].state = 10;
                data[i].field_1C |= 0x08;
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
            pv_osage_manager_array_set_pv_set_motion(chara_id, get_data().pv_data.set_motion[chara_id]);
            pv_osage_manager_array_set_pv_id(chara_id, get_data().pv_id == 832 ? 800 : get_data().pv_id, true);
        }

        state = 10;
        state_old = 10;
    } break;
    case 10: {
        if (pv_osage_manager_array_get_disp() || osage_play_data_manager_check_task_ready())
            break;

        bool wait_load = false;

        for (int32_t i = 0; i < pv_count; i++)
            if (data[i].state != 30)
                wait_load |= true;

        if (wait_load)
            break;

#if BAKE_X_PACK
        state_old = 18;
    } break;
    case 18: {
        x_pv_game_data& pv_data = this->get_data();

        XPVGameBaker* baker = x_pv_game_baker_ptr;

        x_pv_game_write_dsc(pv_data.pv_data.dsc, pv_data.pv_id);

        std::vector<auth_3d*> chara_effect_auth_3ds;
        std::vector<auth_3d*> song_effect_auth_3ds;
        std::vector<auth_3d*> stage_data_effect_auth_3ds;
        std::vector<auth_3d*> stage_data_change_effect_auth_3ds;

        auto add_auth_3d = [&](std::vector<auth_3d*>& auth_3ds, auth_3d_id id) {
            if (!id.check_not_empty() || !id.check_loaded())
                return;

            auth_3d* auth = id.get_auth_3d();
            if (auth && (auth->object.size() || auth->object_hrc.size()))
                auth_3ds.push_back(auth);
        };

        for (auto& i : pv_data.chara_effect.auth_3d)
            for (x_pv_game_chara_effect_auth_3d& j : i)
                add_auth_3d(chara_effect_auth_3ds, j.id);

        for (x_pv_game_song_effect& i : pv_data.effect.song_effect)
            for (x_pv_game_song_effect_auth_3d& j : i.auth_3d)
                add_auth_3d(song_effect_auth_3ds, j.id);

        for (int32_t i = 0; i < X_PV_GAME_STAGE_EFFECT_COUNT; i++) {
            x_pv_game_stage_effect& eff = stage_data.effect[i];
            for (x_pv_game_stage_effect_auth_3d& j : eff.auth_3d)
                add_auth_3d(stage_data_effect_auth_3ds, j.id);
        }

        for (int32_t i = 0; i < X_PV_GAME_STAGE_EFFECT_COUNT; i++)
            for (int32_t j = 0; j < X_PV_GAME_STAGE_EFFECT_COUNT; j++) {
                x_pv_game_stage_change_effect& chg_eff = stage_data.change_effect[i][j];
                for (x_pv_game_stage_effect_auth_3d& k : chg_eff.auth_3d)
                    add_auth_3d(stage_data_change_effect_auth_3ds, k.id);
            }

        prj::sort_unique(chara_effect_auth_3ds);
        prj::sort_unique(song_effect_auth_3ds);
        prj::sort_unique(stage_data_effect_auth_3ds);
        prj::sort_unique(stage_data_change_effect_auth_3ds);

        for (auth_3d*& i : stage_data_effect_auth_3ds) {
            auto j_begin = stage_data_change_effect_auth_3ds.begin();
            auto j_end = stage_data_change_effect_auth_3ds.end();
            for (auto j = j_begin; j != j_end;) {
                if (*j != i)
                    j++;
                else {
                    j = stage_data_change_effect_auth_3ds.erase(j);
                    j_end = stage_data_change_effect_auth_3ds.end();
                }
            }
        }

        std::vector<uint32_t> pv_object_set_ids;
        std::vector<uint32_t> stage_object_set_ids;
        std::vector<Glitter::EffectGroup*> pv_glitter_eff_groups;
        std::vector<Glitter::EffectGroup*> stage_glitter_eff_groups;

        {
            char name[0x40];
            sprintf_s(name, sizeof(name), "STGPV%03d", stage_data.stage_id);
            if (stage_data.obj_db.get_object_set_id(name) != -1)
                stage_object_set_ids.push_back(hash_utf8_murmurhash(name));

            sprintf_s(name, sizeof(name), "STGPV%03dHRC", stage_data.stage_id);
            if (stage_data.obj_db.get_object_set_id(name) != -1)
                stage_object_set_ids.push_back(hash_utf8_murmurhash(name));
        }

        auto add_auth_3d_object_set_id = [&](auth_3d* auth, std::vector<uint32_t>& object_set_ids) {
            object_set_ids.reserve(auth->object.size() + auth->object_hrc.size());

            for (auth_3d_object& i : auth->object)
                object_set_ids.push_back(i.object_info.set_id);

            for (auth_3d_object_hrc& i : auth->object_hrc)
                object_set_ids.push_back(i.object_info.set_id);
        };

        auto add_glitter_object_set_id = [&](uint32_t eff_group_hash,
            std::vector<Glitter::EffectGroup*>& glitter_eff_groups, std::vector<uint32_t>& object_set_ids) {
            Glitter::EffectGroup* eff_group = Glitter::glt_particle_manager->GetEffectGroup(eff_group_hash);
            if (eff_group) {
                glitter_eff_groups.push_back(eff_group);

                object_set_ids.reserve(eff_group->meshes.size());

                for (Glitter::Mesh& i : eff_group->meshes)
                    object_set_ids.push_back(i.object_set_hash);
            }
        };

        for (auth_3d*& i : chara_effect_auth_3ds)
            add_auth_3d_object_set_id(i, pv_object_set_ids);

        for (auth_3d*& i : song_effect_auth_3ds)
            add_auth_3d_object_set_id(i, pv_object_set_ids);

        for (auth_3d*& i : stage_data_effect_auth_3ds)
            add_auth_3d_object_set_id(i, stage_object_set_ids);

        for (auth_3d*& i : stage_data_change_effect_auth_3ds)
            add_auth_3d_object_set_id(i, stage_object_set_ids);

        for (string_hash& i : pv_data.effect.pv_glitter)
            add_glitter_object_set_id(i.hash_murmurhash, pv_glitter_eff_groups, pv_object_set_ids);

        for (x_pv_game_song_effect& i : pv_data.effect.song_effect)
            for (x_pv_game_song_effect_glitter& j : i.glitter)
                add_glitter_object_set_id(j.name.hash_murmurhash, pv_glitter_eff_groups, pv_object_set_ids);

        for (uint32_t& i : stage_data.stage_glitter)
            add_glitter_object_set_id(i, stage_glitter_eff_groups, stage_object_set_ids);

        prj::sort_unique(pv_object_set_ids);
        prj::sort_unique(stage_object_set_ids);

        std::vector<uint32_t> object_set_ids;
        object_set_ids.insert(object_set_ids.end(), pv_object_set_ids.begin(), pv_object_set_ids.end());
        object_set_ids.insert(object_set_ids.end(), stage_object_set_ids.begin(), stage_object_set_ids.end());

        prj::sort_unique(object_set_ids);

        for (uint32_t& i : object_set_ids) {
            ObjsetInfo* info = object_storage_get_objset_info(i);
            if (!info)
                continue;

            obj_set* set = info->obj_set;

            prj::shared_ptr<prj::stack_allocator>& alloc = info->alloc_handler;

            uint32_t obj_num = set->obj_num;
            obj** obj_data = set->obj_data;

            obj_vertex_buffer* objvb = info->objvb;
            for (uint32_t j = 0; j < obj_num; j++)
                objvb[j].unload();

            obj_index_buffer* objib = info->objib;
            for (uint32_t j = 0; j < obj_num; j++)
                objib[j].unload();

            for (uint32_t j = 0; j < obj_num; j++, obj_data++) {
                obj* obj = *obj_data;
                obj_mesh* mesh_array = obj->mesh_array;
                uint32_t num_mesh = obj->num_mesh;
                for (uint32_t k = 0; k < num_mesh; k++) {
                    obj_mesh* mesh = &mesh_array[k];

                    obj_sub_mesh* submesh_array = mesh->submesh_array;
                    uint32_t num_submesh = mesh->num_submesh;
                    for (uint32_t l = 0; l < num_submesh; l++) {
                        obj_sub_mesh* submesh = &submesh_array[l];

                        if (submesh->primitive_type != OBJ_PRIMITIVE_TRIANGLES || !submesh->num_index)
                            continue;

                        submesh->primitive_type = OBJ_PRIMITIVE_TRIANGLE_STRIP;

                        uint32_t* index_array = submesh->index_array;
                        submesh->index_array = alloc->allocate<uint32_t>(
                            meshopt_stripifyBound(submesh->num_index));
                        submesh->num_index = (uint32_t)meshopt_stripify(submesh->index_array,
                            index_array, submesh->num_index, mesh->num_vertex, 0xFFFFFFFF);

                        submesh->first_index = 0;
                        submesh->last_index = 0;
                        submesh->index_offset = 0;
                    }
                }
            }

            obj_data = set->obj_data;

            for (uint32_t j = 0; j < obj_num; j++)
                objvb[j].load(obj_data[j]);

            for (uint32_t j = 0; j < obj_num; j++)
                objib[j].load(obj_data[j]);
        }

        object_set_ids.clear();

        for (uint32_t& i : pv_object_set_ids) {
            ObjsetInfo* info = object_storage_get_objset_info(i);
            if (!info)
                continue;

            x_pv_game_update_object_set(info);
            x_pv_game_write_object_set(info, &baker->obj_db,
                &pv_data.tex_db, &baker->tex_db_base, &baker->tex_db);
        }

        for (uint32_t& i : stage_object_set_ids) {
            ObjsetInfo* info = object_storage_get_objset_info(i);
            if (!info)
                continue;

            x_pv_game_update_object_set(info);
            x_pv_game_write_object_set(info, &baker->obj_db,
                &stage_data.tex_db, &baker->tex_db_base, &baker->tex_db);
        }

        auth_3d_database_file& x_pack_auth_3d_db_file = baker->auth_3d_db;

        {
            auth_3d* auth = get_data().camera.id.get_auth_3d();

            char category[0x40];
            sprintf_s(category, sizeof(category), "CAMPV%03d", pv_data.pv_id);
            replace_names(category);
            
            char name[0x40];
            sprintf_s(name, sizeof(name), "CAMPV%03d_BASE", pv_data.pv_id);
            replace_names(name);

            x_pack_auth_3d_db_file.category.push_back(category);

            x_pack_auth_3d_db_file.uid.push_back({});
            auth_3d_database_uid_file& uid = x_pack_auth_3d_db_file.uid.back();
            uid.flags = (auth_3d_database_uid_flags)(AUTH_3D_DATABASE_UID_SIZE | AUTH_3D_DATABASE_UID_ORG_UID);
            uid.category.assign(category);
            uid.org_uid = x_pack_auth_3d_db_file.uid_max++;
            uid.size = auth->play_control.size;
            uid.value.assign("A ");
            uid.value.append(name);
        }

        if (song_effect_auth_3ds.size()) {
            char name[0x40];
            sprintf_s(name, sizeof(name), "EFFPV%03d", pv_data.pv_id);
            replace_names(name);

            x_pack_auth_3d_db_file.category.push_back(name);

            char path[MAX_PATH];
            strcpy_s(path, sizeof(path), "patch\\!temp\\auth_3d\\");
            strcat_s(path, sizeof(path), name);

            farc* effpv_farc = new farc;
            for (auth_3d*& i : song_effect_auth_3ds)
                x_pv_game_write_auth_3d(effpv_farc, i, &x_pack_auth_3d_db_file, name);

            effpv_farc->write(path, FARC_FArC, FARC_NONE, false);
            delete effpv_farc;
        }

        for (auth_3d*& i : chara_effect_auth_3ds)
            x_pv_game_write_auth_3d(0, i, &x_pack_auth_3d_db_file, 0);

        {
            char name[0x40];
            sprintf_s(name, sizeof(name), "EFFSTGPV%03d", stage_data.stage_id);
            replace_names(name);

            x_pack_auth_3d_db_file.category.push_back(name);

            char path[MAX_PATH];
            strcpy_s(path, sizeof(path), "patch\\!temp\\auth_3d\\");
            strcat_s(path, sizeof(path), name);

            farc* effstgpv_farc = new farc;
            for (stage_data_modern& i : stage_data.stage_data.stg_db.stage_modern)
                for (uint32_t& j : i.auth_3d_ids) {
                    auth_3d* auth = auth_3d_data_get_auth_3d(j);
                    if (auth)
                        x_pv_game_write_auth_3d(effstgpv_farc, auth, &x_pack_auth_3d_db_file, name);
                }

            for (auth_3d*& i : stage_data_effect_auth_3ds)
                x_pv_game_write_auth_3d(effstgpv_farc, i, &x_pack_auth_3d_db_file, name);

            for (auth_3d*& i : stage_data_change_effect_auth_3ds)
                x_pv_game_write_auth_3d(effstgpv_farc, i, &x_pack_auth_3d_db_file, name);

            x_pv_game_write_auth_3d(effstgpv_farc,
                light_auth_3d_id.get_auth_3d(), &x_pack_auth_3d_db_file, name);

            if (effstgpv_farc->files.size())
                effstgpv_farc->write(path, FARC_FArC, FARC_NONE, false);
            delete effstgpv_farc;
        }

        auth_3d_database x_pack_auth_3d_db;
        x_pack_auth_3d_db.add(&baker->auth_3d_db_base, false);
        x_pack_auth_3d_db.add(&baker->auth_3d_db, true);

        object_database x_pack_obj_db;
        x_pack_obj_db.add(&baker->obj_db);
        
        texture_database x_pack_tex_db;
        x_pack_tex_db.add(&baker->tex_db);

        for (Glitter::EffectGroup*& i : pv_glitter_eff_groups)
            x_pv_game_write_glitter(i, &x_pack_auth_3d_db, &pv_data.obj_db, &x_pack_obj_db);

        for (Glitter::EffectGroup*& i : stage_glitter_eff_groups)
            x_pv_game_write_glitter(i, &x_pack_auth_3d_db, &stage_data.obj_db, &x_pack_obj_db);

        x_pv_game_write_play_param(pv_data.play_param, pv_data.pv_id, &x_pack_auth_3d_db);
        x_pv_game_write_stage_resource(stage_data.stage_resource, stage_data.stage_id, &x_pack_auth_3d_db);

        stage_database_file& x_pack_stg_db = baker->stage_data;
        object_database& x_stage_obj_db = stage_data.obj_db;
        texture_database& x_stage_tex_db = stage_data.tex_db;
        stage_database& x_stg_db = stage_data.stage_data.stg_db;

        int32_t stage_max_id = -1;
        for (stage_data_file& i : baker->stage_data.stage_data)
            stage_max_id = max_def(stage_max_id, i.id);

        for (const stage_data_modern& i : x_stg_db.stage_modern) {
            x_pack_stg_db.stage_data.push_back({});
            stage_data_file& stage_data = x_pack_stg_db.stage_data.back();
            stage_data.id = ++stage_max_id;
            stage_data.name.assign(i.name);
            replace_names(stage_data.name);
            stage_data.auth_3d_name.assign("EFF");
            stage_data.auth_3d_name.append(stage_data.name);
            stage_data.object_set_id = x_pack_obj_db.get_object_set_id(stage_data.name.c_str());
            stage_data.object_ground = replace_object_info(i.object_ground, &x_stage_obj_db, &x_pack_obj_db);
            stage_data.object_ring = object_info();
            stage_data.object_sky = replace_object_info(i.object_sky, &x_stage_obj_db, &x_pack_obj_db);
            stage_data.object_shadow = replace_object_info(i.object_shadow, &x_stage_obj_db, &x_pack_obj_db);
            stage_data.object_reflect = replace_object_info(i.object_reflect, &x_stage_obj_db, &x_pack_obj_db);
            stage_data.object_refract = replace_object_info(i.object_refract, &x_stage_obj_db, &x_pack_obj_db);
            stage_data.lens_flare_texture = -1;
            stage_data.lens_shaft_texture = -1;
            stage_data.lens_ghost_texture = -1;
            stage_data.lens_shaft_inv_scale = 1.0f;
            stage_data.unknown = 0;

            stage_data.render_texture = replace_texture_id(i.render_texture, &x_stage_tex_db, &x_pack_tex_db);
            if (stage_data.render_texture == -1) {
                char name[0x40];
                sprintf_s(name, sizeof(name), "F_DIVA_STGPV%03d_AM_FB05", this->stage_data.stage_id);
                replace_names(name);
                stage_data.render_texture = x_pack_tex_db.get_texture_id(name);
            }

            stage_data.movie_texture = replace_texture_id(i.movie_texture, &x_stage_tex_db, &x_pack_tex_db);
            if (stage_data.movie_texture == -1) {
                char name[0x40];
                sprintf_s(name, sizeof(name), "F_DIVA_STGPV%03d_AM_FB04", this->stage_data.stage_id);
                replace_names(name);
                stage_data.movie_texture = x_pack_tex_db.get_texture_id(name);
            }

            stage_data.collision_file_path.assign("rom/STGTST_COLI.000.bin");
            stage_data.reflect_type = STAGE_DATA_REFLECT_DISABLE;
            stage_data.flags = (stage_data_flags)0;
            stage_data.ring_rectangle_x = i.ring_rectangle_x;
            stage_data.ring_rectangle_y = i.ring_rectangle_y;
            stage_data.ring_rectangle_width = i.ring_rectangle_width;
            stage_data.ring_rectangle_height = i.ring_rectangle_height;
            stage_data.ring_height = i.ring_height;
            stage_data.ring_out_height = i.ring_out_height;

            stage_data.auth_3d_ids.reserve(i.auth_3d_ids.size());
            for (const uint32_t& j : i.auth_3d_ids) {
                auth_3d* auth = auth_3d_data_get_auth_3d(j);
                if (!auth)
                    continue;

                char name[0x200];
                if (get_replace_auth_name(name, sizeof(name), auth)) {
                    int32_t uid = x_pack_auth_3d_db.get_uid(name);
                    if (uid != -1)
                        stage_data.auth_3d_ids.push_back(uid);
                }
            }
        }

#endif
        state_old = 19;
    } break;
    case 19: {
        x_pv_game_change_field(this, 1, -1, -1);

        Glitter::glt_particle_manager->SetPause(false);
        get_data().bar_beat.reset_time();

        extern float_t frame_speed;
        frame_speed = 1.0f;

        pause = false;
        step_frame = false;

        frame_float = 0.0;
        frame = (int32_t)frame_float;
        time = (int64_t)round(frame_float * (100000.0 / 60.0) * 10000.0);

        x_pv_game_pv_data& pv_data = get_data().pv_data;

        pv_data.dsc_data_ptr = pv_data.dsc.data.data();
        pv_data.dsc_data_ptr_end = pv_data.dsc_data_ptr + pv_data.dsc.data.size();

        pv_data.ctrl(false, 0.0f, 0.0f);

        light_auth_3d_id.set_enable(true);
        light_auth_3d_id.set_camera_root_update(false);
        light_auth_3d_id.set_paused(false);
        light_auth_3d_id.set_repeat(false);
        light_auth_3d_id.set_visibility(true);
        light_auth_3d_id.set_req_frame(0.0f);

#if BAKE_PV826
        if (pv_id == 826) {
            data_struct* aft_data = &data_list[DATA_AFT];
            bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
            motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

            for (int32_t& i : effchrpv_rob_mot_ids) {
                char buf[0x100];
                sprintf_s(buf, sizeof(buf), "PV826_OST_P%d_00", i + 1);
                uint32_t motion_id = aft_mot_db->get_motion_id(buf);
                rob_chara* rob_chr = rob_chara_array_get(rob_chara_ids[i]);
                rob_chr->set_motion_id(motion_id, 0.0f,
                    0.0f, false, false, MOTION_BLEND_CROSS, aft_bone_data, aft_mot_db);
                rob_chr->set_motion_reset_data(motion_id, 0.0f);
                rob_chr->set_motion_skin_param(motion_id, 0.0f);

                sprintf_s(buf, sizeof(buf), "A3D_EFFCHRPV%03dMIK%03d", pv_id, i);
                uint32_t hash = hash_utf8_murmurhash(buf);
                for (auto j : effchrpv_auth_3d_mot_names)
                    if (j.second.hash_murmurhash == hash) {
                        uint32_t hash = hash_string_murmurhash(j.first);
                        effchrpv_auth_3d_rob_mot_ids.insert({ i, { effchrpv_auth_3d_mot_ids[hash] } });
                        break;
                    }
            }

            for (auto& i : effchrpv_auth_3d_rob_mot_ids) {
                x_pv_game_a3da_to_mot& a2m = i.second;
                auth_3d* auth = a2m.id.get_auth_3d();
                a2m.get_bone_indices(&auth->object_hrc[0]);
            }

            for (auto& i : effchrpv_auth_3d_mot_ids) {
                auth_3d_id& id = i.second;
                id.set_repeat(false);
                id.set_camera_root_update(false);
                id.set_enable(true);
                id.set_paused(false);
                id.set_visibility(false);
                id.set_req_frame(0.0f);
            }
        }
#endif

#if BAKE_PNG || BAKE_VIDEO
        extern bool disable_cursor;
        disable_cursor = true;

        const int32_t width = BAKE_BASE_WIDTH * BAKE_RES_SCALE;
        const int32_t height = BAKE_BASE_HEIGHT * BAKE_RES_SCALE;

#if BAKE_VIDEO
        if (GLAD_WGL_NV_DX_interop2) {
            glGenRenderbuffers(d3d_in_flight_num, d3d_gl_rbo);
            glGenFramebuffers(d3d_in_flight_num, d3d_gl_fbo);
            glGenQueries(d3d_in_flight_num, d3d_query);

#if BAKE_VIDEO_ALPHA
            glGenRenderbuffers(d3d_in_flight_num, d3d_gl_alpha_rbo);
            glGenFramebuffers(d3d_in_flight_num, d3d_gl_alpha_fbo);
            glGenQueries(d3d_in_flight_num, d3d_alpha_query);
#endif

            D3D11_TEXTURE2D_DESC tex_desc = { };
            tex_desc.Width = width;
            tex_desc.Height = height;
            tex_desc.MipLevels = 1;
            tex_desc.ArraySize = 1;
            tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            tex_desc.SampleDesc.Count = 1;
            tex_desc.SampleDesc.Quality = 0;
            tex_desc.Usage = D3D11_USAGE_DEFAULT;
            tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
            tex_desc.CPUAccessFlags = 0;
            tex_desc.MiscFlags = 0;

            for (int32_t i = 0; i < d3d_in_flight_num; i++)
                d3d_device->CreateTexture2D(&tex_desc, 0, &d3d_texture[i]);

            for (int32_t i = 0; i < d3d_in_flight_num; i++)
                d3d_gl_rbo_handle[i] = wglDXRegisterObjectNV(d3d_gl_handle, d3d_texture[i],
                    d3d_gl_rbo[i], GL_RENDERBUFFER, WGL_ACCESS_READ_WRITE_NV);

            for (int32_t i = 0; i < d3d_in_flight_num; i++) {
                wglDXLockObjectsNV(d3d_gl_handle, 1, &d3d_gl_rbo_handle[i]);
                gl_state_bind_framebuffer(d3d_gl_fbo[i]);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, d3d_gl_rbo[i]);
                gl_state_bind_framebuffer(0);
                wglDXUnlockObjectsNV(d3d_gl_handle, 1, &d3d_gl_rbo_handle[i]);
            }

#if BAKE_VIDEO_ALPHA
            for (int32_t i = 0; i < d3d_in_flight_num; i++)
                d3d_device->CreateTexture2D(&tex_desc, 0, &d3d_alpha_texture[i]);

            for (int32_t i = 0; i < d3d_in_flight_num; i++)
                d3d_gl_alpha_rbo_handle[i] = wglDXRegisterObjectNV(d3d_gl_handle, d3d_alpha_texture[i],
                    d3d_gl_alpha_rbo[i], GL_RENDERBUFFER, WGL_ACCESS_READ_WRITE_NV);

            for (int32_t i = 0; i < d3d_in_flight_num; i++) {
                wglDXLockObjectsNV(d3d_gl_handle, 1, &d3d_gl_alpha_rbo_handle[i]);
                gl_state_bind_framebuffer(d3d_gl_alpha_fbo[i]);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, d3d_gl_alpha_rbo[i]);
                gl_state_bind_framebuffer(0);
                wglDXUnlockObjectsNV(d3d_gl_handle, 1, &d3d_gl_alpha_rbo_handle[i]);
            }
#endif
        }
        else {
            D3D11_TEXTURE2D_DESC tex_desc = { };
            tex_desc.Width = width;
            tex_desc.Height = height;
            tex_desc.MipLevels = 1;
            tex_desc.ArraySize = 1;
            tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            tex_desc.SampleDesc.Count = 1;
            tex_desc.SampleDesc.Quality = 0;
            tex_desc.Usage = D3D11_USAGE_DYNAMIC;
            tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            tex_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            tex_desc.MiscFlags = 0;

            d3d_device->CreateTexture2D(&tex_desc, 0, &d3d_texture[0]);

            nvenc_temp_pixels.resize((size_t)width* (size_t)height* nvenc_src_pixel_size);
        }

#if BAKE_VIDEO_ALPHA
        nvenc_enc = new nvenc_encoder(width, height, d3d_device);
        nvenc_alpha_enc = new nvenc_encoder(width, height, d3d_device);

        char buf[0x100];
        sprintf_s(buf, sizeof(buf), "G:\\ReDIVA\\Videos\\ReDIVA_pv%03d_color.265", get_data().pv_id);

        nvenc_stream = new file_stream();
        nvenc_stream->open(buf, "wb");

        sprintf_s(buf, sizeof(buf), "G:\\ReDIVA\\Videos\\ReDIVA_pv%03d_alpha.265", get_data().pv_id);

        nvenc_alpha_stream = new file_stream();
        nvenc_alpha_stream->open(buf, "wb");
#else
        nvenc_enc = new nvenc_encoder(width, height, d3d_device);

        char buf[0x100];
        sprintf_s(buf, sizeof(buf), "G:\\ReDIVA\\Videos\\ReDIVA_pv%03d.265", get_data().pv_id);

        nvenc_stream = new file_stream();
        nvenc_stream->open(buf, "wb");
#endif
#endif
#endif

#if BAKE_PV826
        x_pv_game_map_auth_3d_to_mot(this, true);
#endif

#if !BAKE_FAST
        pause = true;
#endif

        state_old = 20;
        //sound_work_release_se("load01_2", false);
    } break;
    case 20: {
        float_t delta_frame = get_delta_frame();
        frame_float += delta_frame;
        frame = (int32_t)frame_float;
        time = (int64_t)round(frame_float * (100000.0 / 60.0) * 10000.0);

#if !BAKE_FAST
        x_pv_game_music_ptr->set_pause(pause ? 1 : 0);
#endif

#if BAKE_PV826
        x_pv_game_map_auth_3d_to_mot(this, delta_frame != 0.0f);
#endif

#if BAKE_PNG || BAKE_VIDEO
        img_write = true;
#endif

        if (!get_data().pv_data.play || get_data().pv_data.pv_end)
            state_old = 21;
    } break;
    case 21: {
#if BAKE_PNG || BAKE_VIDEO
        extern bool disable_cursor;
        disable_cursor = false;

#if BAKE_VIDEO
        if (GLAD_WGL_NV_DX_interop2) {
            bool wait = false;
            for (int32_t i = 0; i < d3d_in_flight_num; i++)
                if (d3d_tex_write[i])
                    wait |= true;

#if BAKE_VIDEO_ALPHA
            for (int32_t i = 0; i < d3d_in_flight_num; i++)
                if (d3d_alpha_tex_write[i])
                    wait |= true;
#endif

            if (wait)
                break;
        }

#if BAKE_VIDEO_ALPHA
        nvenc_alpha_stream->close();
        delete nvenc_alpha_stream;
        nvenc_alpha_stream = 0;
#endif

        nvenc_stream->close();
        delete nvenc_stream;
        nvenc_stream = 0;

#if BAKE_VIDEO_ALPHA
        delete nvenc_alpha_enc;
        nvenc_alpha_enc = 0;
#endif

        delete nvenc_enc;
        nvenc_enc = 0;

        if (GLAD_WGL_NV_DX_interop2) {
#if BAKE_VIDEO_ALPHA
            for (int32_t i = 0; i < d3d_in_flight_num; i++) {
                wglDXUnregisterObjectNV(d3d_gl_handle, d3d_gl_alpha_rbo_handle[i]);
                d3d_gl_alpha_rbo_handle[i] = 0;
            }

            for (int32_t i = 0; i < d3d_in_flight_num; i++) {
                d3d_alpha_texture[i]->Release();
                d3d_alpha_texture[i] = 0;
            }

            glDeleteQueries(d3d_in_flight_num, d3d_alpha_query);

            glDeleteFramebuffers(d3d_in_flight_num, d3d_gl_alpha_fbo);
            memset(d3d_gl_alpha_fbo, 0, sizeof(d3d_gl_alpha_fbo));

            glDeleteRenderbuffers(d3d_in_flight_num, d3d_gl_alpha_rbo);
            memset(d3d_gl_alpha_rbo, 0, sizeof(d3d_gl_alpha_rbo));
#endif

            for (int32_t i = 0; i < d3d_in_flight_num; i++) {
                wglDXUnregisterObjectNV(d3d_gl_handle, d3d_gl_rbo_handle[i]);
                d3d_gl_rbo_handle[i] = 0;
            }

            for (int32_t i = 0; i < d3d_in_flight_num; i++) {
                d3d_texture[i]->Release();
                d3d_texture[i] = 0;
            }

            glDeleteQueries(d3d_in_flight_num, d3d_query);

            glDeleteFramebuffers(d3d_in_flight_num, d3d_gl_fbo);
            memset(d3d_gl_fbo, 0, sizeof(d3d_gl_fbo));

            glDeleteRenderbuffers(d3d_in_flight_num, d3d_gl_rbo);
            memset(d3d_gl_rbo, 0, sizeof(d3d_gl_rbo));
        }
        else {
            nvenc_temp_pixels.clear();
            nvenc_temp_pixels.shrink_to_fit();

            d3d_texture[0]->Release();
            d3d_texture[0] = 0;
        }
#endif
#endif

        for (int32_t i = 0; i < ROB_CHARA_COUNT; i++)
            pv_expression_array_reset_motion(i);

#if BAKE_DOF
        {
            x_pv_game_data& pv_data = this->get_data();

            data_struct* x_data = &data_list[DATA_X];

            char buf[0x200];
            sprintf_s(buf, sizeof(buf), "CAMPV%03d", pv_data.pv_id);

            farc* f = new farc;
            x_data->load_file(f, "root+/auth_3d/", hash_utf8_murmurhash(buf), ".farc", farc::load_file);

            if ((pv_data.pv_id % 100) >= 25 && (pv_data.pv_id % 100) <= 30
                && stage_data.stage_id >= 25 && stage_data.stage_id <= 30)
                sprintf_s(buf, sizeof(buf), "CAMPV%03d_100.a3da", pv_data.pv_id);
            else
                sprintf_s(buf, sizeof(buf), "CAMPV%03d_BASE.a3da", pv_data.pv_id);

            a3da a;
            farc_file* ff = f->read_file(buf);
            if (ff)
                a.read(ff->data, ff->size);
            delete f;

            if ((pv_data.pv_id % 100) >= 25 && (pv_data.pv_id % 100) <= 30
                && stage_data.stage_id >= 25 && stage_data.stage_id <= 30)
                sprintf_s(buf, sizeof(buf), "DOF\\auth_3d\\CAMPV%03d_100", pv_data.pv_id);
            else
                sprintf_s(buf, sizeof(buf), "DOF\\auth_3d\\CAMPV%03d_BASE", pv_data.pv_id);

            replace_pv832(buf);
            replace_pv832(a._file_name);

            a.ready = true;
            a.compressed = true;
            a.format = A3DA_FORMAT_AFT_X_PACK;
            a.dof = {};
            a.dof.has_dof = true;

            std::thread position_x(a3da_key_rev,
                std::ref(a.dof.model_transform.translation.x), std::ref(dof_cam_data.position_x));
            std::thread position_y(a3da_key_rev,
                std::ref(a.dof.model_transform.translation.y), std::ref(dof_cam_data.position_y));
            std::thread position_z(a3da_key_rev,
                std::ref(a.dof.model_transform.translation.z), std::ref(dof_cam_data.position_z));
            std::thread focus_range(a3da_key_rev,
                std::ref(a.dof.model_transform.scale.x), std::ref(dof_cam_data.focus_range));
            //std::thread focus(a3da_key_rev,
            //    std::ref(a.dof.model_transform.scale.y), std::ref(dof_cam_data.focus));
            std::thread fuzzing_range(a3da_key_rev,
                std::ref(a.dof.model_transform.rotation.x), std::ref(dof_cam_data.fuzzing_range));
            std::thread ratio(a3da_key_rev,
                std::ref(a.dof.model_transform.rotation.y), std::ref(dof_cam_data.ratio));
            SetThreadDescription((HANDLE)position_x.native_handle(), L"DoF A3DA Bake Thread: Position X");
            SetThreadDescription((HANDLE)position_y.native_handle(), L"DoF A3DA Bake Thread: Position Y");
            SetThreadDescription((HANDLE)position_z.native_handle(), L"DoF A3DA Bake Thread: Position Z");
            SetThreadDescription((HANDLE)focus_range.native_handle(), L"DoF A3DA Bake Thread: Focus Range");
            //SetThreadDescription((HANDLE)focus.native_handle(), L"DoF A3DA Bake Thread: Focus");
            SetThreadDescription((HANDLE)fuzzing_range.native_handle(), L"DoF A3DA Bake Thread: Fuzzing Range");
            SetThreadDescription((HANDLE)ratio.native_handle(), L"DoF A3DA Bake Thread: Ratio");

            if (position_x.joinable())
                position_x.join();

            if (position_y.joinable())
                position_y.join();

            if (position_z.joinable())
                position_z.join();

            if (focus_range.joinable())
                focus_range.join();
            
            //if (focus.joinable())
            //    focus.join();

            if (fuzzing_range.joinable())
                fuzzing_range.join();

            if (ratio.joinable())
                ratio.join();

            a.dof.model_transform.translation.x.raw_data = true;
            a.dof.model_transform.translation.y.raw_data = true;
            a.dof.model_transform.translation.z.raw_data = true;

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

            a.dof.model_transform.visibility.type = A3DA_KEY_STATIC;
            a.dof.model_transform.visibility.value = 1.0f;

            a.write(buf);

            dof_cam_data.reset();
        }
#endif

        for (int32_t i = 0; i < pv_count; i++)
            data[i].unload();
        stage_data.unload();
        state = 50;

        state_old = 22;
    } break;
    case 22: {
        if (state)
            break;

        Glitter::glt_particle_manager->FreeScenes();

        del();
    } break;
    }
    return false;
}

bool x_pv_game::dest() {
    if (!unload())
        return false;

    task_rob_manager_del_task();

    Glitter::glt_particle_manager->SetPause(false);
    extern float_t frame_speed;
    frame_speed = 1.0f;

    extern bool close;
    //close = true;
    return true;
}

void x_pv_game::disp() {
    if (state_old != 20)
        return;

    if (pv_index >= 0 && pv_index < pv_count)
        get_data().disp();
}

void x_pv_game::basic() {
    if (state_old != 20 && state_old != 21)
        return;

#if BAKE_DOF
    if (dof_cam_data.frame != frame) {
        camera* cam = rctx_ptr->camera;

        bool enable = rctx_ptr->render.get_dof_enable();

        float_t focus;
        float_t focus_range;
        float_t fuzzing_range;
        float_t ratio;
        rctx_ptr->render.get_dof_data(focus, focus_range, fuzzing_range, ratio);

        vec3 interest;
        vec3 view_point;
        cam->get_interest(interest);
        cam->get_view_point(view_point);

        interest = view_point + vec3::normalize(interest - view_point) * focus;

        dof_cam_data.position_x.push_back(interest.x);
        dof_cam_data.position_y.push_back(interest.y);
        dof_cam_data.position_z.push_back(interest.z);
        dof_cam_data.focus.push_back(focus);
        dof_cam_data.focus_range.push_back(focus_range);
        dof_cam_data.fuzzing_range.push_back(fuzzing_range);
        dof_cam_data.ratio.push_back(ratio);
        if (dof_cam_data.enable != enable) {
            dof_cam_data.enable_frame.push_back(frame, enable);
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

#if BAKE_PV826
    for (auto& i : effchrpv_auth_3d_rob_mot_ids) {
        rob_chara* rob_chr = rob_chara_array_get(rob_chara_ids[i.first]);
        x_pv_game_a3da_to_mot& a2m = i.second;

        rob_chara_bone_data* rob_bone_data = rob_chr->bone_data;

        bone_database_skeleton_type skeleton_type = rob_bone_data->base_skeleton_type;
        motion_blend_mot* mot = rob_bone_data->motion_loaded.front();
        std::vector<bone_data>* bones = &mot->bone_data.bones;
        std::vector<uint16_t>* bone_indices = &mot->bone_data.bone_indices;

        bone_data* bones_data = bones->data();
        for (uint16_t& i : *bone_indices) {
            bone_data* data = &bones_data[i];
            ::motion_bone_index motion_bone_index = (::motion_bone_index)data->motion_bone_index;
            if (!(motion_bone_index >= MOTION_BONE_N_HITO_L_EX
                && motion_bone_index <= MOTION_BONE_NL_OYA_C_L_WJ
                || motion_bone_index >= MOTION_BONE_N_HITO_R_EX
                && motion_bone_index <= MOTION_BONE_NL_OYA_C_R_WJ))
                continue;

            auto elem = a2m.bone_keys.find(motion_bone_index);
            if (elem == a2m.bone_keys.end())
                elem = a2m.bone_keys.insert({ motion_bone_index, {} }).first;

            vec3 rotation;
            mat4_get_rotation(&data->rot_mat[0], &rotation);
            if (elem->second.x.size() == frame)
                elem->second.x.push_back(rotation.x);
            if (elem->second.y.size() == frame)
                elem->second.y.push_back(rotation.y);
            if (elem->second.z.size() == frame)
                elem->second.z.push_back(rotation.z);
        }
    }
#endif
}

void x_pv_game::window() {
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
        basic();
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
        sprintf_s(buf, sizeof(buf), "%d/%d", frame, (int32_t)(time / 10000));
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

    basic();
}

void x_pv_game::load(int32_t pv_id, int32_t stage_id, chara_index charas[6], int32_t modules[6]) {
    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    this->pv_id = pv_id;
    this->stage_id = stage_id;
    memmove(this->charas, charas, sizeof(this->charas));
    memmove(this->modules, modules, sizeof(this->modules));

    data_struct* x_data = &data_list[DATA_X];

    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "PV%03d", pv_id == 832 ? 800 : pv_id);
    uint32_t mot_set = aft_mot_db->get_motion_set_id(buf);
    motion_set_load_motion(mot_set, "", aft_mot_db);
    motion_set_load_mothead(mot_set, "", aft_mot_db);

    pv_param::light_data_load_files(pv_id, "");
    pv_param::post_process_data_load_files(pv_id, "");

    {
        dof d;
        sprintf_s(buf, sizeof(buf), "pv%03d", pv_id);
        x_data->load_file(&d, "root+/post_process_table/", hash_utf8_murmurhash(buf), ".dft", dof::load_file);
        pv_param::post_process_data_set_dof(d);
    }

    sprintf_s(buf, sizeof(buf), "EFFSTGPV%03d", 800 + stage_id);
    light_category = buf;

    int32_t chara_index = 0;

    success = true;
    task_effect_init = false;

    pause = false;
    step_frame = false;

    for (int32_t& i : rob_chara_ids)
        i = -1;

    state = 10;
    state_old = 1;
    this->frame = 0;
    frame_float = 0.0;
    time = 0;

    Glitter::counter.Reset();
    Glitter::glt_particle_manager->draw_all = false;
    //sound_work_play_se(1, "load01_2", 1.0f);
}

#if BAKE_PV826
static void fix_rotation(std::vector<float_t>& vec) {
    if (vec.size() < 2)
        return;

    const float_t half_pi = (float_t)(M_PI / 2.0);

    int32_t curr_rot = 0;
    float_t rot_fix = 0.0f;
    float_t rot_prev = vec.data()[0];
    float_t* i_begin = vec.data() + 1;
    float_t* i_end = vec.data() + vec.size();
    for (float_t* i = i_begin; i != i_end; i++) {
        float_t rot = *i;
        if (rot < -half_pi && rot_prev > half_pi && fabsf(rot - rot_prev) > half_pi) {
            curr_rot++;
            rot_fix = (float_t)(M_PI * 2.0 * (double_t)curr_rot);
        }
        else if (rot > half_pi && rot_prev < -half_pi && fabsf(rot - rot_prev) > half_pi) {
            curr_rot--;
            rot_fix = (float_t)(M_PI * 2.0 * (double_t)curr_rot);
        }

        if (curr_rot)
            *i = rot + rot_fix;
        rot_prev = rot;
    }
}

struct mot_data_bake {
    int32_t performer;
    x_pv_game_a3da_to_mot* data;
    lock<uint32_t> state;

    mot_data_bake() : performer(), data() {

    }
};

const int32_t bake_pv826_threads_count = 4;

const motion_set_info* bake_pv826_set_info;
std::thread* bake_pv826_thread;
mot_data_bake* bake_pv826_mot_data;
int32_t bake_pv826_performer;
std::mutex* bake_pv826_alloc_mutex;
prj::shared_ptr<prj::stack_allocator>* bake_pv826_alloc;
::mot_set* bake_pv826_mot_set;

static mot_key_set_type mot_write_motion_fit_keys_into_curve(std::vector<float_t>& values_src,
    prj::shared_ptr<prj::stack_allocator> alloc, uint16_t*& frames, float_t*& values, size_t& keys_count) {
    std::vector<uint16_t> _frames;
    std::vector<float_t> _values;
    mot_key_set_type type = mot_set::fit_keys_into_curve(values_src, _frames, _values);
    switch (type) {
    case MOT_KEY_SET_NONE:
        keys_count = 0;
        frames = 0;
        values = 0;
        break;
    case MOT_KEY_SET_STATIC:
        keys_count = 1;
        {
            std::unique_lock<std::mutex> u_lock(*bake_pv826_alloc_mutex);
            frames = 0;
            values = (*bake_pv826_alloc)->allocate<float_t>();
        }
        memcpy(values, _values.data(), sizeof(float_t));
        break;
    case MOT_KEY_SET_HERMITE:
        keys_count = _frames.size();
        {
            std::unique_lock<std::mutex> u_lock(*bake_pv826_alloc_mutex);
            frames = (*bake_pv826_alloc)->allocate<uint16_t>(_frames.data(), keys_count);
            values = (*bake_pv826_alloc)->allocate<float_t>(keys_count);
        }
        memcpy(frames, _frames.data(), sizeof(uint16_t) * keys_count);
        memcpy(values, _values.data(), sizeof(float_t) * keys_count);
        break;
    case MOT_KEY_SET_HERMITE_TANGENT:
        keys_count = _frames.size();
        {
            std::unique_lock<std::mutex> u_lock(*bake_pv826_alloc_mutex);
            frames = (*bake_pv826_alloc)->allocate<uint16_t>(keys_count);
            values = (*bake_pv826_alloc)->allocate<float_t>(keys_count * 2);
        }
        memcpy(frames, _frames.data(), sizeof(uint16_t) * keys_count);
        memcpy(values, _values.data(), sizeof(float_t) * keys_count * 2);
        break;
    }
    return type;
}

static void mot_write_motion(mot_data_bake* bake) {
    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "PV826_OST_P%d_00", bake->performer);

    uint32_t motion_id = aft_mot_db->get_motion_id(buf);

    size_t motion_index = -1;
    const motion_set_info* set_info = bake_pv826_set_info;
    for (const motion_info& j : set_info->motion)
        if (j.id == motion_id) {
            motion_index = &j - set_info->motion.data();
            break;
        }

    mot_data* mot_data = &bake_pv826_mot_set->mot_data[motion_index];

    uint16_t key_set_count = mot_data->key_set_count - 1;
    if (!key_set_count)
        return;

    const char* name = bone_database_skeleton_type_to_string(BONE_DATABASE_SKELETON_COMMON);
    std::string* bone_names = aft_mot_db->bone_name.data();
    const std::vector<bone_database_bone>* bones = aft_bone_data->get_skeleton_bones(name);
    if (!bones)
        return;

    prj::shared_ptr<prj::stack_allocator>& alloc = *bake_pv826_alloc;
    x_pv_game_a3da_to_mot& a2m = *bake->data;
    const mot_bone_info* bone_info = mot_data->bone_info_array;
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

        const bone_database_bone* bone = &(*bones)[bone_index];

        auto elem = a2m.bone_keys.find(bone_index);
        if (elem != a2m.bone_keys.end()) {
            x_pv_game_a3da_to_mot_keys& keys = elem->second;

            if (bone->type == BONE_DATABASE_BONE_ROTATION) {
                fix_rotation(keys.x);
                fix_rotation(keys.y);
                fix_rotation(keys.z);
            }

            mot_key_set_data& key_set_data_x = mot_data->key_set_array[key_set_offset];
            key_set_data_x.frames = 0;
            key_set_data_x.values = 0;
            size_t keys_x_count = 0;
            key_set_data_x.type = mot_write_motion_fit_keys_into_curve(keys.x, alloc,
                key_set_data_x.frames, key_set_data_x.values, keys_x_count);
            key_set_data_x.keys_count = (uint16_t)keys_x_count;
            key_set_data_x.data_type = MOT_KEY_SET_DATA_F32;

            mot_key_set_data& key_set_data_y = mot_data->key_set_array[key_set_offset + 1];
            key_set_data_y.frames = 0;
            key_set_data_y.values = 0;
            size_t keys_y_count = 0;
            key_set_data_y.type = mot_write_motion_fit_keys_into_curve(keys.y, alloc,
                key_set_data_y.frames, key_set_data_y.values, keys_y_count);
            key_set_data_y.keys_count = (uint16_t)keys_y_count;
            key_set_data_y.data_type = MOT_KEY_SET_DATA_F32;

            mot_key_set_data& key_set_data_z = mot_data->key_set_array[key_set_offset + 2];
            key_set_data_z.frames = 0;
            key_set_data_z.values = 0;
            size_t keys_z_count = 0;
            key_set_data_z.type = mot_write_motion_fit_keys_into_curve(keys.z, alloc,
                key_set_data_z.frames, key_set_data_z.values, keys_z_count);
            key_set_data_z.keys_count = (uint16_t)keys_z_count;
            key_set_data_z.data_type = MOT_KEY_SET_DATA_F32;
        }

        elem = a2m.sec_bone_keys.find(bone_index);
        if (elem != a2m.sec_bone_keys.end()) {
            x_pv_game_a3da_to_mot_keys& keys = elem->second;

            fix_rotation(keys.x);
            fix_rotation(keys.y);
            fix_rotation(keys.z);

            mot_key_set_data& key_set_data_x = mot_data->key_set_array[key_set_offset + 3];
            key_set_data_x.frames = 0;
            key_set_data_x.values = 0;
            size_t keys_x_count = 0;
            key_set_data_x.type = mot_write_motion_fit_keys_into_curve(keys.x, alloc,
                key_set_data_x.frames, key_set_data_x.values, keys_x_count);
            key_set_data_x.keys_count = (uint16_t)keys_x_count;
            key_set_data_x.data_type = MOT_KEY_SET_DATA_F32;

            mot_key_set_data& key_set_data_y = mot_data->key_set_array[key_set_offset + 4];
            key_set_data_y.frames = 0;
            key_set_data_y.values = 0;
            size_t keys_y_count = 0;
            key_set_data_y.type = mot_write_motion_fit_keys_into_curve(keys.y, alloc,
                key_set_data_y.frames, key_set_data_y.values, keys_x_count);
            key_set_data_y.keys_count = (uint16_t)keys_y_count;
            key_set_data_y.data_type = MOT_KEY_SET_DATA_F32;

            mot_key_set_data& key_set_data_z = mot_data->key_set_array[key_set_offset + 5];
            key_set_data_z.frames = 0;
            key_set_data_z.values = 0;
            size_t keys_z_count = 0;
            key_set_data_z.type = mot_write_motion_fit_keys_into_curve(keys.z, alloc,
                key_set_data_z.frames, key_set_data_z.values, keys_z_count);
            key_set_data_z.keys_count = (uint16_t)keys_z_count;
            key_set_data_z.data_type = MOT_KEY_SET_DATA_F32;
        }

        if (bone_index == MOTION_BONE_KL_AGO_WJ) {
            mot_key_set_data& key_set_data_x = mot_data->key_set_array[key_set_offset];
            key_set_data_x.frames = 0;
            {
                std::unique_lock<std::mutex> u_lock(*bake_pv826_alloc_mutex);
                key_set_data_x.values = alloc->allocate<float_t>();
                key_set_data_x.values[0] = 0.0491406508f;
            }
            key_set_data_x.type = MOT_KEY_SET_STATIC;
            key_set_data_x.keys_count = 1;
            key_set_data_x.data_type = MOT_KEY_SET_DATA_F32;

            mot_key_set_data& key_set_data_y = mot_data->key_set_array[key_set_offset + 1];
            key_set_data_y.frames = 0;
            key_set_data_y.values = 0;
            key_set_data_y.type = MOT_KEY_SET_NONE;
            key_set_data_y.keys_count = 0;
            key_set_data_y.data_type = MOT_KEY_SET_DATA_F32;

            mot_key_set_data& key_set_data_z = mot_data->key_set_array[key_set_offset + 2];
            key_set_data_z.frames = 0;
            key_set_data_z.values = 0;
            key_set_data_z.type = MOT_KEY_SET_NONE;
            key_set_data_z.keys_count = 0;
            key_set_data_z.data_type = MOT_KEY_SET_DATA_F32;
        }
        else if (bone_index == MOTION_BONE_N_KUBI_WJ_EX) {
            mot_key_set_data& key_set_data_x = mot_data->key_set_array[key_set_offset];
            key_set_data_x.frames = 0;
            key_set_data_x.values = 0;
            key_set_data_x.type = MOT_KEY_SET_NONE;
            key_set_data_x.keys_count = 1;
            key_set_data_x.data_type = MOT_KEY_SET_DATA_F32;

            mot_key_set_data& key_set_data_y = mot_data->key_set_array[key_set_offset + 1];
            key_set_data_y.frames = 0;
            {
                std::unique_lock<std::mutex> u_lock(*bake_pv826_alloc_mutex);
                key_set_data_y.values = alloc->allocate<float_t>();
                key_set_data_y.values[0] = 0.0331281610f;
            }
            key_set_data_y.type = MOT_KEY_SET_STATIC;
            key_set_data_y.keys_count = 0;
            key_set_data_y.data_type = MOT_KEY_SET_DATA_F32;

            mot_key_set_data& key_set_data_z = mot_data->key_set_array[key_set_offset + 2];
            key_set_data_z.frames = 0;
            key_set_data_z.values = 0;
            key_set_data_z.type = MOT_KEY_SET_NONE;
            key_set_data_z.keys_count = 0;
            key_set_data_z.data_type = MOT_KEY_SET_DATA_F32;
        }

        if (bone->type >= BONE_DATABASE_BONE_POSITION_ROTATION)
            key_set_offset += 6;
        else
            key_set_offset += 3;
    }

    bake->state.set(0);
}

static bool mot_write_motion_set(void* data, const char* path, const char* file, uint32_t hash) {
    x_pv_game* xpvgm = (x_pv_game*)data;

    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "PV%03d", xpvgm->pv_id);
    bake_pv826_set_info = aft_mot_db->get_motion_set_by_name(buf);
    if (!bake_pv826_set_info)
        return true;

    bake_pv826_alloc = new prj::shared_ptr<prj::stack_allocator>(new prj::stack_allocator);
    bake_pv826_alloc_mutex = 0;
    bake_pv826_thread = 0;
    bake_pv826_mot_data = 0;
    bake_pv826_performer = 1;

    bake_pv826_mot_set = (*bake_pv826_alloc)->allocate<::mot_set>();
    {
        std::string mot_file;
        mot_file.append("mot_");
        mot_file.append(bake_pv826_set_info->name);
        mot_file.append(".bin");

        farc f;
        farc::load_file(&f, path, file, hash);

        farc_file* ff = f.read_file(mot_file.c_str());
        if (!ff) {
            delete bake_pv826_alloc;
            bake_pv826_alloc = 0;
            bake_pv826_mot_set = 0;
            return true;
        }

        bake_pv826_mot_set->unpack_file(*bake_pv826_alloc, ff->data, ff->size, false);
    }

    if (!bake_pv826_mot_set->ready) {
        delete bake_pv826_alloc;
        bake_pv826_alloc = 0;
        bake_pv826_mot_set = 0;
        return true;
    }

    bake_pv826_alloc_mutex = new std::mutex;
    bake_pv826_thread = new std::thread[bake_pv826_threads_count];
    bake_pv826_mot_data = new mot_data_bake[bake_pv826_threads_count];
    return true;
}

static void mot_write_motion_set(x_pv_game* xpvgm) {
    if (!bake_pv826_set_info)
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    {
        std::string mot_file;
        mot_file.append("mot_");
        mot_file.append(bake_pv826_set_info->name);
        mot_file.append(".bin");

        farc f;

        f.add_file(mot_file.c_str());
        farc_file* ff = &f.files.back();
        bake_pv826_mot_set->pack_file(&ff->data, &ff->size);
        ff->compressed = true;

        std::string mot_farc;
        mot_farc.append("pv826\\mot_");
        mot_farc.append(bake_pv826_set_info->name);
        f.write(mot_farc.c_str(), FARC_FArC, FARC_NONE, false);
    }

    delete[] bake_pv826_thread;
    delete[] bake_pv826_mot_data;
    delete bake_pv826_alloc_mutex;

    delete bake_pv826_alloc;

    bake_pv826_thread = 0;
    bake_pv826_mot_data = 0;
    bake_pv826_alloc_mutex = 0;
    bake_pv826_mot_set = 0;
    bake_pv826_alloc = 0;
    bake_pv826_set_info = 0;
}
#endif

bool x_pv_game::unload() {
    pv_osage_manager_array_set_not_reset_true();
    if (pv_osage_manager_array_get_disp())
        return false;

#if BAKE_PV826
    if (get_data().pv_id == 826) {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        char buf[0x200];
        sprintf_s(buf, sizeof(buf), "PV%03d", get_data().pv_id);
        const motion_set_info* set_info = aft_mot_db->get_motion_set_by_name(buf);
        if (set_info) {
            std::string farc_file = "mot_" + set_info->name + ".farc";
            aft_data->load_file(this, "rom/rob/", farc_file.c_str(), mot_write_motion_set);
        }

        if (bake_pv826_set_info) {
            waitable_timer timer;
            while (true) {
                int32_t free_thread_count = 0;
                for (int32_t i = 0; i < bake_pv826_threads_count; i++)
                    if (!bake_pv826_mot_data[i].state.get())
                        free_thread_count++;

                if (free_thread_count && bake_pv826_performer < 6)
                    for (auto& i : effchrpv_auth_3d_rob_mot_ids) {
                        if (!free_thread_count)
                            break;
                        else if (bake_pv826_performer != i.first)
                            continue;

                        std::thread* thread = 0;
                        int32_t thread_index = -1;
                        for (int32_t j = 0; j < bake_pv826_threads_count; j++)
                            if (!bake_pv826_mot_data[j].state.get()) {
                                thread = &bake_pv826_thread[j];
                                thread_index = j;
                                break;
                            }

                        if (!thread)
                            break;

                        mot_data_bake* bake = &bake_pv826_mot_data[thread_index];
                        bake->performer = bake_pv826_performer + 1;
                        bake->data = &i.second;
                        bake->state.set(1);

                        if (thread->joinable())
                            thread->join();

                        *thread = std::thread(mot_write_motion, bake);

                        wchar_t buf[0x80];
                        swprintf_s(buf, sizeof(buf) / sizeof(wchar_t),
                            L"X PV GAME BAKE PV826 P%d", bake->performer);
                        SetThreadDescription((HANDLE)thread->native_handle(), buf);

                        bake_pv826_performer++;
                        free_thread_count--;
                    }

                if (free_thread_count == 2)
                    break;

                timer.sleep_float(1.0);
            }

            for (int32_t i = 0; i < bake_pv826_threads_count; i++)
                bake_pv826_thread[i].join();

            mot_write_motion_set(this);
        }
    }
#endif

    for (int32_t& i : rob_chara_ids)
        skin_param_manager_reset(i);

    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    pv_param::light_data_clear_data();
    pv_param::post_process_data_clear_data();

    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "PV%03d", get_data().pv_id == 832 ? 800 : get_data().pv_id);
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

    light_auth_3d_id.unload(rctx_ptr);

    light_auth_3d_id = {};

    auth_3d_data_unload_category(light_category.c_str());

    light_category.clear();
    light_category.shrink_to_fit();

    for (int32_t i = 0; i < pv_count; i++)
        data[i].reset();
    stage_data.reset();

    get_data().bar_beat.reset();

    get_data().stage_effects.clear();
    get_data().stage_effects.shrink_to_fit();
    get_data().stage_effect_index = 0;

    success = true;
    task_effect_init = false;

    pv_id = 0;
    stage_id = 0;
    for (chara_index& i : charas)
        i = CHARA_MAX;
    for (int32_t& i : modules)
        i = 0;

    Glitter::glt_particle_manager->draw_all = true;

    osage_play_data_manager_reset();

    skin_param_storage_reset();

    light_param_data_storage_data_reset();

    rndr::Render& rend = rctx_ptr->render;
    rend.reset_saturate_coeff(0, true);
    rend.set_dof_enable(false);
    rend.set_dof_update(false);
    rend.reset_scene_fade(0);

    rend.set_taa(1);
    rend.update_res(0, -1);

    rctx_ptr->disp_manager->object_culling = true;
    shadow_ptr_get()->shadow_range_factor = 1.0f;
    rctx_ptr->render_manager->set_effect_texture(0);

    sound_work_reset_all_se();

    pv_param_task::post_process_task_del_task();
    return true;
}

void x_pv_game::ctrl(float_t curr_time, float_t delta_time) {
#if !BAKE_FAST
    x_pv_game_music_ptr->ctrl(delta_time);
#endif

#if BAKE_PV826
    for (int32_t i = 0; i < pv_count; i++)
        data[i].ctrl(curr_time, delta_time,
            data[i].pv_id == 826 ? &effchrpv_auth_3d_mot_ids : 0);
#else
    for (int32_t i = 0; i < pv_count; i++)
        data[i].ctrl(curr_time, delta_time);
#endif

    switch (state) {
    case 10:
        if (stage_data.state == 20)
            state = 20;
        break;
    case 30: {
        bool v38 = true;
        for (int32_t i = 0; i < pv_count; i++)
            if (data[i].state == 10 || data[i].state == 20) {
                v38 = false;
                break;
            }

        if (!v38)
            break;

        stop_current_pv();
        this->data[pv_index].unload_data();
        pv_index++;

        state = 20;
    } break;
    case 40: {
        bool v19 = true;
        for (int32_t i = 0; i < pv_count; i++)
            if (data[i].state == 10 || data[i].state == 20) {
                v19 = false;
                break;
            }

        if (!v19)
            break;

        stop_current_pv();

        x_pv_game_data* data = this->data;
        for (int32_t i = 0; i < pv_count; i++, data++) {
            if (!data->state) {
                rctx_ptr->camera->reset();
                data->field_1C &= ~0xD1;
                data->state = 0;
                data->stage_effect_index = 0;
                data->next_stage_effect_bar = 0;
                data->pv_data.reset();
            }

            if (i > 0)
                data->unload_data();
        }

        if (pv_index) {
            pv_index = 0;
            if (!data->state)
                data->state = 20;
            else if (data->state == 10)
                data->field_1C |= 0x08;
        }
        state = 20;
    }
    case 50: {
        bool v16 = false;
        for (int32_t i = 0; i < pv_count; i++)
            if (data[i].state)
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

    data[pv_index].stop();
    field_7198C.reset();
    field_71994.reset();
}

#if BAKE_X_PACK
XPVGameBaker::XPVGameBaker() : charas(), modules(), start(), exit(), next() {
    pv_id = 800;
    stage_id = 0;
    chara_index = CHARA_MIKU;

    for (::chara_index& i : charas)
        i = CHARA_MIKU;

    for (int32_t& i : modules)
        i = 0;

    next = true;
}

XPVGameBaker::~XPVGameBaker() {

}

bool XPVGameBaker::init() {
    data_struct* aft_data = &data_list[DATA_AFT];

    aet_db.read("diva\\AFT_mod\\mdata\\MPF2\\rom\\2d\\mdata_aet_db", false);
    auth_3d_db_base.read("diva\\AFT\\rom\\auth_3d\\auth_3d_db");
    auth_3d_db.read("diva\\AFT_mod\\mdata\\MPF2\\rom\\auth_3d\\mdata_auth_3d_db");
    obj_db.read("diva\\AFT_mod\\mdata\\MPF2\\rom\\objset\\mdata_obj_db", false);
    stage_data.read("diva\\AFT_mod\\mdata\\MPF2\\rom\\mdata_stage_data", false);
    {
        texture_database_file tex_db_base;
        tex_db_base.read("diva\\AFT\\rom\\objset\\tex_db", false);
        this->tex_db_base.add(&tex_db_base);
    }
    tex_db.read("diva\\AFT_mod\\mdata\\MPF2\\rom\\objset\\mdata_tex_db", false);
    return true;
}

bool XPVGameBaker::ctrl() {
    if (!next)
        return false;

    next = false;

    switch (pv_id) {
    case 832:
        exit = true;
        return true;
    }

    pv_id++;
    stage_id++;
    start = true;
    return false;
}

bool XPVGameBaker::dest() {
    aet_db.write("patch\\!temp\\2d\\mdata_aet_db");
    auth_3d_db.write("patch\\!temp\\auth_3d\\mdata_auth_3d_db");
    obj_db.write("patch\\!temp\\objset\\mdata_obj_db");
    stage_data.write("patch\\!temp\\mdata_stage_data");
    tex_db.write("patch\\!temp\\objset\\mdata_tex_db");
    return true;
}
#else
XPVGameSelector::XPVGameSelector() : charas(), modules(), start(), exit() {
    pv_id = 823;
    stage_id = 23;

    for (chara_index& i : charas)
        i = CHARA_MIKU;

    for (int32_t& i : modules)
        i = 0;

    pv_id = 816;
    stage_id = 16;

    charas[0] = CHARA_LEN;

    modules[0] = 0;

    pv_id = 811;
    stage_id = 11;

    charas[0] = CHARA_MIKU;

    modules[0] = 0;

#if BAKE_PV826
    pv_id = 826;
    stage_id = 26;

    charas[0] = CHARA_MIKU;
    charas[1] = CHARA_RIN;
    charas[2] = CHARA_LEN;
    charas[3] = CHARA_LUKA;
    charas[4] = CHARA_KAITO;
    charas[5] = CHARA_MEIKO;

    modules[0] = 168;
    modules[1] = 46;
    modules[2] = 39;
    modules[3] = 41;
    modules[4] = 40;
    modules[5] = 31;
#endif

    const prj::vector_pair_combine<int32_t, module>& modules = module_table_handler_data_get_modules();
    for (const auto& i : modules)
        modules_data[i.second.chara].push_back(&i.second);

    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++)
        for (const auto& j : modules_data[charas[i]])
            if (this->modules[i] == j->cos) {
                module_names[i].assign(j->name);
                break;
            }
}

XPVGameSelector::~XPVGameSelector() {

}

bool XPVGameSelector::init() {
    start = false;
    exit = false;
    return true;
}

bool XPVGameSelector::ctrl() {
    return false;
}

bool XPVGameSelector::dest() {
    return true;
}

void XPVGameSelector::window() {
    static const char* pv_names[] = {
        u8"801. Strangers",
        u8"802. Ai Dee",
        u8"803. Streaming Heart",
        u8"804. Babylon",
        u8"805. The Lost One's Weeping",
        u8"806. Slow Motion",
        u8"807. Tale of the Deep-sea Lily",
        u8"808. Love Trial",
        u8"809. Love Song",
        u8"810. The First Sound",
        u8"811. LOL - lots of laugh -",
        u8"812. Patchwork Staccato",
        u8"813. Even a Kunoichi Needs Love",
        u8"814. Calc.",
        u8"815. A Single Red Leaf",
        u8"816. Holy Lance Explosion Boy",
        u8"817. Urotander, Underhanded Rangers",
        u8"818. Humorous Dream of Mrs. Pumpkin",
        u8"819. Solitary Envy",
        u8"820. Raspberry ＊ Monster",
        u8"821. Brain Revolution Girl",
        u8"822. Amazing Dolce",
        u8"823. Name of the Sin",
        u8"824. Satisfaction",
        u8"825. Cute Medley - Idol Sounds",
        u8"826. Beginning Medley - Primary Colors",
        u8"827. Cool Medley - Cyber Rock Jam",
        u8"828. Elegant Medley - Glossy Mixture",
        u8"829. Quirky Medley - Giga-Remix",
        u8"830. Ending Medley - Ultimate Exquisite Rampage",
        u8"831. Sharing The World",
        u8"832. Hand in Hand",
    };

    static const char* stage_names[] = {
        u8"001. Nighttime Curb Stage",
        u8"002. Digital Concert Stage",
        u8"003. Secret Ruins Stage",
        u8"004. Utopia Stage",
        u8"005. Classroom Concert Stage",
        u8"006. Capsule Room Stage",
        u8"007. Deep Sea Stage",
        u8"008. Verdict Stage",
        u8"009. Sky Garden Stage",
        u8"010. Musical Dawn Stage",
        u8"011. Sweet Dreams Stage",
        u8"012. Heart's Playroom Stage",
        u8"013. Ninja Village Stage",
        u8"014. Park of Promises Stage",
        u8"015. Autumn Sakura Stage",
        u8"016. Classy Lounge Stage",
        u8"017. Secret Base Stage",
        u8"018. Halloween Party Stage",
        u8"019. Twilight Shrine Stage",
        u8"020. Raspberry Stage",
        u8"021. Cabaret Stage",
        u8"022. Witch's House Stage",
        u8"023. Lilies & Shadows Stage",
        u8"024. Techno Club Stage",
        u8"025. Cute Concert Hall",
        u8"026. First Concert Hall",
        u8"027. Cool Concert Hall",
        u8"028. Elegant Concert Hall",
        u8"029. Quirky Concert Hall",
        u8"030. Ultimate Concert Hall",
        u8"031. Highrise Stage",
        u8"032. Mirai Concert Stage",
    };

    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    extern int32_t height;
    extern int32_t width;

    float_t w = 400.0f;
    float_t h = (float_t)height;
    h = min_def(h, 438.0f);

    ImGui::SetNextWindowPos({ (float_t)width - w, 0.0f }, ImGuiCond_Always);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoResize;

    focus = false;
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
    pv_id = max_def(pv_id, 0);
    ImGui::ColumnComboBox("PV", pv_names, 32, &pv_id, 0, false, &focus);
    pv_id += 801;

    stage_id--;
    stage_id = max_def(stage_id, 0);
    ImGui::ColumnComboBox("Stage", stage_names, 32, &stage_id, 0, false, &focus);
    stage_id++;

    char buf[0x200];
    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
        chara_index chara_old = charas[i];

        sprintf_s(buf, sizeof(buf), "Chara %dP", i + 1);
        ImGui::ColumnComboBox(buf, chara_full_names, CHARA_MAX,
            (int32_t*)&charas[i], 0, false, &focus);

        if (chara_old != charas[i]) {
            modules[i] = 0;
            module_names[i].clear();

            for (const auto& j : modules_data[charas[i]])
                if (modules[i] == j->cos) {
                    module_names[i].assign(j->name);
                    break;
                }
        }
    }

    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
        sprintf_s(buf, sizeof(buf), "Module %dP", i + 1);

        ImGui::StartPropertyColumn(buf);
        extern ImFont* imgui_font_arial;
        if (imgui_font_arial)
            ImGui::PushFont(imgui_font_arial);
        if (ImGui::BeginCombo("", module_names[i].c_str(), 0)) {
            for (const auto& j : modules_data[charas[i]]) {
                if (j->cos == 499)
                    continue;

                ImGui::PushID(&j);
                if (ImGui::Selectable(j->name.c_str(), modules[i] == j->cos)
                    || ImGui::ItemKeyPressed(ImGuiKey_Enter)
                    || (ImGui::IsItemFocused() && modules[i] != j->cos)) {
                    modules[i] = j->cos;
                    module_names[i].assign(j->name);
                }
                ImGui::PopID();

                if (modules[i] == j->cos)
                    ImGui::SetItemDefaultFocus();
            }

            focus |= true;
            ImGui::EndCombo();
        }
        if (imgui_font_arial)
            ImGui::PopFont();
        ImGui::EndPropertyColumn();
    }

    if (ImGui::Button("Start")) {
        start = true;
        exit = true;
    }

    ImGui::End();
}
#endif

bool x_pv_game_init() {
    x_pv_game_ptr = new x_pv_game;
#if !BAKE_FAST
    x_pv_game_music_ptr = new x_pv_game_music;
#endif
    return true;
}

x_pv_game* x_pv_game_get() {
    return x_pv_game_ptr;
}

bool x_pv_game_free() {
    if (x_pv_game_ptr) {
        switch (x_pv_game_ptr->state_old) {
        case 0:
            break;
        case 21:
        case 22:
            return false;
        default:
            x_pv_game_ptr->state_old = 21;
            return false;
        }

        if (app::TaskWork::has_task(x_pv_game_ptr))
            return false;

        delete x_pv_game_ptr;
        x_pv_game_ptr = 0;

#if !BAKE_FAST
        x_pv_game_music_ptr->stop_reset_flags();

        delete x_pv_game_music_ptr;
        x_pv_game_music_ptr = 0;
#endif

    }
    return true;
}

#if BAKE_X_PACK
bool x_pv_game_baker_init() {
    if (!x_pv_game_baker_ptr)
        x_pv_game_baker_ptr = new XPVGameBaker;
    return true;
}

XPVGameBaker* x_pv_game_baker_get() {
    return x_pv_game_baker_ptr;
}

bool x_pv_game_baker_free() {
    if (x_pv_game_baker_ptr) {
        delete x_pv_game_baker_ptr;
        x_pv_game_baker_ptr = 0;
    }
    return true;
}
#else
bool x_pv_game_selector_init() {
    if (!x_pv_game_selector_ptr)
        x_pv_game_selector_ptr = new XPVGameSelector;
    return true;
}

XPVGameSelector* x_pv_game_selector_get() {
    return x_pv_game_selector_ptr;
}

bool x_pv_game_selector_free() {
    if (x_pv_game_selector_ptr) {
        delete x_pv_game_selector_ptr;
        x_pv_game_selector_ptr = 0;
    }
    return true;
}
#endif

#if BAKE_DOF
dof_cam::dof_cam() {
    frame = -999999;
    enable = false;
}

dof_cam::~dof_cam() {

}

void dof_cam::reset() {
    position_x.clear();
    position_y.clear();
    position_z.clear();
    focus.clear();
    focus_range.clear();
    fuzzing_range.clear();
    ratio.clear();
    enable_frame.clear();
    frame = -999999;
    enable = false;
}
#endif

static vec4 bright_scale_get(int32_t index, float_t value) {
    static const float_t bright_scale_table[] = {
        1.0f, 1.176f, 0.85f, 1.0f,
        1.0f, 1.176f, 1.176f, 1.176f
    };

    value = bright_scale_table[index] * value;
    return { value, value, value, 1.0f };
}

static float_t dsc_time_to_frame(int64_t time) {
    return prj::roundf((float_t)time / 1000000000.0f * 60.0f);
}

static vec3 x_pv_game_dof_callback(void* data, int32_t chara_id) {
    x_pv_game_data& pv_data = ((x_pv_game*)data)->get_data();
    if (chara_id >= pv_data.play_param->chara.size())
        return pv_data.chara_effect.get_node_translation(0,
            (int32_t)(chara_id - pv_data.play_param->chara.size()), 0, "j_kao_wj");

    if (chara_id < 0 && chara_id >= ROB_CHARA_COUNT)
        return 0.0f;

    rob_chara* rob_chr = rob_chara_array_get(chara_id);
    if (!rob_chr)
        return 0.0f;

    vec3 trans;
    mat4_get_translation(rob_chr->get_bone_data_mat(MOTION_BONE_CL_KAO), &trans);
    return trans;
}

static void x_pv_game_item_alpha_callback(void* data, int32_t chara_id, int32_t type, float_t alpha) {
    if (!data)
        return;

    x_pv_game_data& pv_data = ((x_pv_game*)data)->get_data();

    if (chara_id < 0 || chara_id >= pv_data.play_param->chara.size())
        return;

    mdl::ObjFlags flags;
    switch (type) {
    case 0:
    default:
        flags = mdl::OBJ_ALPHA_ORDER_1;
        break;
    case 1:
        flags = mdl::OBJ_ALPHA_ORDER_2;
        break;
    case 2:
        flags = mdl::OBJ_ALPHA_ORDER_3;
        break;
    }

    for (x_pv_game_song_effect& i : pv_data.effect.song_effect) {
        if (i.chara_id != chara_id)
            continue;

        for (x_pv_game_song_effect_auth_3d& j : i.auth_3d)
            j.id.set_alpha_obj_flags(alpha, flags);

        for (x_pv_game_song_effect_glitter& j : i.glitter)
            Glitter::glt_particle_manager->SetSceneEffectExtColor(j.scene_counter,
                false, hash_murmurhash_empty, -1.0f, -1.0f, -1.0f, alpha);
    }
}

static void x_pv_game_change_field(x_pv_game* xpvgm, int32_t field, int64_t dsc_time, int64_t curr_time) {
    if (!xpvgm->task_effect_init && (dsc_time > -1 || curr_time > -1)) {
        rand_state_array_4_set_seed_1393939();
        effect_manager_reset();
        xpvgm->task_effect_init = true;
    }

    light_param_data_storage_data_set_pv_cut(field);
}

#if BAKE_PV826
static void set_bone_key_set_data(bone_data* bone_data,
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
        auth_3d* auth = a2m.id.get_auth_3d();
        auth_3d_object_hrc* oh = &auth->object_hrc[0];

        float_t auth_frame = auth->frame;
        bool auth_frame_changed = auth->frame_changed;
        bool auth_paused = auth->paused;
        auth->frame = (float_t)xpvgm->frame_float;
        auth->frame_changed = false;
        auth->paused = true;
        auth->ctrl(rctx_ptr);

        rob_chr->set_visibility(oh->node[0].model_transform.visible);

        motion_blend_mot* mot = rob_chr->bone_data->motion_loaded.front();
        ::bone_data* bone_data = mot->bone_data.bones.data();
        mot_key_set* key_set = mot->mot_key_data.mot.key_sets;

        vec3 data[2];
        data[0] = oh->node[a2m.gblctr].model_transform.translation_value;
        data[0].y = oh->node[a2m.n_hara].model_transform.translation_value.y;
        data[1] = 0.0f;
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

        data[0] = { 0.0f, 0.945f, 0.0f };
        mat4_transform_vector(&oh->node[a2m.j_mune_wj].model_transform.mat, &data[0], &data[1]);
        mat4_get_translation(&oh->node[a2m.j_mune_wj].model_transform.mat, &data[0]);
        data[0] += data[1];
        data[1] = 0.0f;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_CL_MUNE, key_set, data, 2);

        data[0] = oh->node[a2m.j_mune_b_wj].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_MUNE_B_WJ, key_set, data);

        data[0] = oh->node[a2m.j_kubi_wj].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_KUBI, key_set, data);

        data[0] = { (float_t)(M_PI / 2.0), 0.0f, -(float_t)M_PI };
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_N_KAO, key_set, data);

        data[0] = { 0.0f, 0.40f, 0.0f };
        mat4_transform_vector(&oh->node[a2m.j_kao_wj].model_transform.mat, &data[0], &data[1]);
        mat4_get_translation(&oh->node[a2m.j_kao_wj].model_transform.mat, &data[0]);
        data[0] += data[1];
        data[1] = oh->node[a2m.j_kao_wj].model_transform.rotation_value;
        data[1].x = -data[1].x;
        data[1].z = -data[1].z;
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

        data[0] = oh->node[a2m.j_waki_l_wj].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_WAKI_L_WJ, key_set, data);

        {
            vec3 pos_j_kata_l_wj;
            vec3 pos_j_ude_l_wj;
            vec3 pos_j_te_l_wj;
            mat4_get_translation(&oh->node[a2m.j_kata_l_wj].model_transform.mat, &pos_j_kata_l_wj);
            mat4_get_translation(&oh->node[a2m.j_ude_l_wj].model_transform.mat, &pos_j_ude_l_wj);
            mat4_get_translation(&oh->node[a2m.j_te_l_wj].model_transform.mat, &pos_j_te_l_wj);

            vec3 tl_up_kata_dir = pos_j_ude_l_wj - vec3::lerp(pos_j_kata_l_wj, pos_j_te_l_wj, 0.5f);
            float_t pos_middle_dist = vec3::length(tl_up_kata_dir);
            if (pos_middle_dist == 0.0f) {
                mat4& mat = oh->node[a2m.j_kata_l_wj].model_transform.mat;
                data[0] = { 0.0f, 0.3f, 0.0f };
                mat4_transform_vector(&mat, &data[0], &tl_up_kata_dir);
            }
            else
                tl_up_kata_dir *= 0.3f / pos_middle_dist;

            vec3 tl_up_kata_pos = tl_up_kata_dir + pos_j_ude_l_wj;
            mat4& mat = oh->node[a2m.j_mune_b_wj].model_transform.mat;
            mat4_inverse_transform_point(&mat, &tl_up_kata_pos, &data[0]);
            set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
                MOTION_BONE_TL_UP_KATA_L, key_set, data);
        }

        mat4_get_translation(&oh->node[a2m.j_te_l_wj].model_transform.mat, &data[0]);
        data[1] = { -(float_t)(M_PI / 2.0), -(float_t)(M_PI / 2.0), -(float_t)(M_PI / 2.0) };
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_C_KATA_L, key_set, data, 2);

        data[0] = oh->node[a2m.j_te_l_wj].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_TE_L_WJ, key_set, data);

        data[0] = oh->node[a2m.n_waki_r].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_N_WAKI_R, key_set, data);

        data[0] = oh->node[a2m.j_waki_l_wj].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_WAKI_R_WJ, key_set, data);

        {
            vec3 pos_j_kata_r_wj;
            vec3 pos_j_ude_r_wj;
            vec3 pos_j_te_r_wj;
            mat4_get_translation(&oh->node[a2m.j_kata_r_wj].model_transform.mat, &pos_j_kata_r_wj);
            mat4_get_translation(&oh->node[a2m.j_ude_r_wj].model_transform.mat, &pos_j_ude_r_wj);
            mat4_get_translation(&oh->node[a2m.j_te_r_wj].model_transform.mat, &pos_j_te_r_wj);

            vec3 tl_up_kata_dir = pos_j_ude_r_wj - vec3::lerp(pos_j_kata_r_wj, pos_j_te_r_wj, 0.5f);
            float_t pos_middle_dist = vec3::length(tl_up_kata_dir);
            if (pos_middle_dist == 0.0f) {
                mat4& mat = oh->node[a2m.j_kata_r_wj].model_transform.mat;
                data[0] = { 0.0f, 0.3f, 0.0f };
                mat4_transform_vector(&mat, &data[0], &tl_up_kata_dir);
            }
            else
                tl_up_kata_dir *= 0.3f / pos_middle_dist;

            vec3 tl_up_kata_pos = tl_up_kata_dir + pos_j_ude_r_wj;
            mat4& mat = oh->node[a2m.j_mune_b_wj].model_transform.mat;
            mat4_inverse_transform_point(&mat, &tl_up_kata_pos, &data[0]);
            set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
                MOTION_BONE_TL_UP_KATA_R, key_set, data);
        }

        mat4_get_translation(&oh->node[a2m.j_te_r_wj].model_transform.mat, &data[0]);
        data[1] = { -(float_t)(M_PI / 2.0), (float_t)(M_PI / 2.0), (float_t)(M_PI / 2.0) };
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_C_KATA_R, key_set, data, 2);

        data[0] = oh->node[a2m.j_te_r_wj].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_TE_R_WJ, key_set, data);

        mat4_get_translation(&oh->node[a2m.j_asi_l_wj].model_transform.mat, &data[0]);
        data[0].y -= 0.033f;
        if (data[0].y < 0.103f)
            data[0].y = 0.103f;
        data[1] = oh->node[a2m.j_momo_l_wj].model_transform.rotation_value;
        data[1].z -= (float_t)(M_PI / 2.0);
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_CL_MOMO_L, key_set, data, 2);

        mat4_get_translation(&oh->node[a2m.j_asi_r_wj].model_transform.mat, &data[0]);
        data[0].y -= 0.033f;
        if (data[0].y < 0.103f)
            data[0].y = 0.103f;
        data[1] = oh->node[a2m.j_momo_r_wj].model_transform.rotation_value;
        data[1].z -= (float_t)(M_PI / 2.0);
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_CL_MOMO_R, key_set, data, 2);

        data[0] = oh->node[a2m.j_asi_l_wj].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_ASI_L_WJ_CO, key_set, data);

        data[0] = oh->node[a2m.j_asi_r_wj].model_transform.rotation_value;
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_ASI_R_WJ_CO, key_set, data);

        data[0] = { 0.0491406508f, 0.0f, 0.0f };
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_KL_AGO_WJ, key_set, data);

        data[0] = { 0.0f, 0.0331281610f, 0.0f };
        set_bone_key_set_data(bone_data, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            MOTION_BONE_N_KUBI_WJ_EX, key_set, data);

        auth->frame = auth_frame;
        auth->frame_changed = auth_frame_changed;
        auth->paused = auth_paused;
    }
}
#endif

static void x_pv_game_reset_field(x_pv_game* xpvgm) {

}

struct x_pv_game_auth_3d_hrc_obj_sub_mesh {
    std::vector<uint32_t> indices;

    x_pv_game_auth_3d_hrc_obj_sub_mesh() {

    }

    ~x_pv_game_auth_3d_hrc_obj_sub_mesh() {

    }
};

struct x_pv_game_auth_3d_hrc_obj_mesh {
    std::vector<x_pv_game_auth_3d_hrc_obj_sub_mesh> sub_meshes;
    std::vector<uint32_t> vertex_indices;
    std::vector<obj_vertex_data> vertices;

    x_pv_game_auth_3d_hrc_obj_mesh() {

    }

    ~x_pv_game_auth_3d_hrc_obj_mesh() {

    }
};

struct x_pv_game_auth_3d_hrc_obj_bone {
    uint32_t src_id;
    std::string dst_name;
    int32_t index;
    std::vector<x_pv_game_auth_3d_hrc_obj_mesh> meshes;

    x_pv_game_auth_3d_hrc_obj_bone() {
        src_id = -1;
        index = -1;
    }

    ~x_pv_game_auth_3d_hrc_obj_bone() {

    }
};

static int x_pv_game_auth_3d_hrc_obj_bone_compare_func(void const* src1, void const* src2) {
    x_pv_game_auth_3d_hrc_obj_bone* bone1 = (x_pv_game_auth_3d_hrc_obj_bone*)src1;
    x_pv_game_auth_3d_hrc_obj_bone* bone2 = (x_pv_game_auth_3d_hrc_obj_bone*)src2;
    return bone1->dst_name.compare(bone2->dst_name);
}

static void auth_3d_key_rev(auth_3d_key& k, std::vector<float_t>& values_src) {
    std::vector<kft3> values;
    int32_t type = interpolate_chs_reverse_sequence(values_src, values);

    k = {};
    switch (type) {
    case A3DA_KEY_NONE:
        k.type = AUTH_3D_KEY_NONE;
        k.value = 0.0f;
        return;
    case A3DA_KEY_STATIC:
        k.type = values[0].value != 0.0f ? AUTH_3D_KEY_STATIC : AUTH_3D_KEY_NONE;
        k.value = values[0].value;
        return;
    case A3DA_KEY_LINEAR:
        k.type = AUTH_3D_KEY_LINEAR;
        break;
    case A3DA_KEY_HERMITE:
    default:
        k.type = AUTH_3D_KEY_HERMITE;
        break;
    case A3DA_KEY_HOLD:
        k.type = AUTH_3D_KEY_HOLD;
        break;
    }

    k.max_frame = (float_t)(values_src.size() + 1);
    k.frame_delta = k.max_frame;
    k.value_delta = 0.0f;

    size_t length = values.size();
    if (length > 1) {
        k.keys_vec.assign(values.begin(), values.end());
        k.length = length;
        k.keys = k.keys_vec.data();

        kft3* first_key = &k.keys[0];
        kft3* last_key = &k.keys[length - 1];
        if (first_key->frame < last_key->frame
            && last_key->frame > 0.0f && k.max_frame > first_key->frame) {
            k.ep_type_pre = AUTH_3D_EP_NONE;
            k.ep_type_post = AUTH_3D_EP_NONE;
            k.frame_delta = last_key->frame - first_key->frame;
            k.value_delta = last_key->value - first_key->value;
        }
    }
    else if (length == 1) {
        float_t value = values.front().value;
        k.type = value != 0.0f ? AUTH_3D_KEY_STATIC : AUTH_3D_KEY_NONE;
        k.value = value;
    }
    else {
        k.type = AUTH_3D_KEY_NONE;
        k.value = 0.0f;
    }
}

#if BAKE_X_PACK
static void x_pv_game_update_object_set(ObjsetInfo* info) {
    prj::shared_ptr<prj::stack_allocator> old_alloc = info->alloc_handler;

    prj::shared_ptr<prj::stack_allocator>& alloc = info->alloc_handler;
    alloc = prj::shared_ptr<prj::stack_allocator>(new prj::stack_allocator);

    obj_set* set = alloc->allocate<obj_set>();
    set->move_data(info->obj_set, alloc);
    info->obj_set = set;
}

static bool x_pv_game_write_auth_3d(farc* f, auth_3d* auth,
    auth_3d_database_file* auth_3d_db, const char* category) {
    char name[0x200];
    if (!get_replace_auth_name(name, sizeof(name), auth))
        return false;

    a3da a;
    float_t max_frame = auth->max_frame;
    auth->max_frame = auth->play_control.size;
    auth->store(&a);
    auth->max_frame = max_frame;
    a._converter_version.assign("20111019");
    a._file_name.assign(name);
    a._file_name.append(".a3da");
    a._property_version.assign("20110526");
    a.ready = true;
    a.compressed = true;
    a.format = A3DA_FORMAT_AFT_X_PACK;

    for (a3da_m_object_hrc& i : a.m_object_hrc) {
        for (a3da_object_instance& j : i.instance) {
            replace_names(j.name);
            replace_names(j.uid_name);
        }

        replace_names(i.name);
    }

    for (std::string& i : a.m_object_hrc_list)
        replace_names(i);

    for (a3da_object& i : a.object) {
        replace_names(i.name);
        replace_names(i.parent_name);
        replace_names(i.uid_name);

        for (a3da_object_texture_pattern& j : i.texture_pattern)
            replace_names(j.name);

        for (a3da_object_texture_transform& j : i.texture_transform)
            replace_names(j.name);
    }

    for (a3da_object_hrc& i : a.object_hrc) {
        replace_names(i.name);
        replace_names(i.parent_name);
        replace_names(i.uid_name);
    }
    
    for (std::string& i : a.object_hrc_list)
        replace_names(i);


    for (std::string& i : a.object_list)
        replace_names(i);

    bool own_farc = !f;
    
    if (own_farc)
        f = new farc;

    farc_file* ff = f->add_file(name);
    ff->name.append(".a3da");
    a.write(&ff->data, &ff->size);

    if (own_farc) {
        char path[MAX_PATH];
        strcpy_s(path, sizeof(path), "patch\\!temp\\auth_3d\\");
        strcat_s(path, sizeof(path), name);

        f->write(path, FARC_FArC, FARC_NONE, false);
        delete f;

        auth_3d_db->category.push_back(name);
    }

    auth_3d_db->uid.push_back({});
    auth_3d_database_uid_file& uid = auth_3d_db->uid.back();
    uid.flags = (auth_3d_database_uid_flags)(AUTH_3D_DATABASE_UID_SIZE | AUTH_3D_DATABASE_UID_ORG_UID);
    uid.category.assign(own_farc ? name : category);
    uid.org_uid = auth_3d_db->uid_max++;
    uid.size = auth->play_control.size;
    uid.value.assign("A ");
    uid.value.append(name);
    return true;
}

static void x_pv_game_write_dsc(const dsc& d, int32_t pv_id) {
    data_struct* aft_data = &data_list[DATA_AFT];

    dsc _d = d;
    {
        std::vector<dsc_data> data_temp;
        data_temp.reserve(_d.data.size());

        for (const dsc_data& i : _d.data)
            switch (i.func) {
            case DSC_X_TARGET:
            case DSC_X_TARGET_FLYING_TIME:
            case DSC_X_DOF:
                break;
            default:
                data_temp.push_back(i);
                break;
            }

        _d.data.assign(data_temp.begin(), data_temp.end());
        data_temp.clear();
        _d.rebuild();
    }

    for (const dsc_data& i : _d.data) {
        if (i.func != DSC_X_MOUTH_ANIM)
            continue;

        int32_t* data = _d.get_func_data(&i);

        int32_t& mouth_anim_id = data[2];
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
        }
    }

    _d.convert(DSC_FT);
    _d.signature = 0x14050921;

    char file_buf[0x200];

    const char* dsc_diffs[] = {
        "easy",
        "normal",
        "hard",
        "extreme",
        "extreme_1",
    };

    for (const char*& i : dsc_diffs) {
        dsc dsc_chart;
        sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_%s.dsc", pv_id == 832 ? 800 : pv_id, i);
        if (!aft_data->load_file(&dsc_chart, "rom/script/", file_buf, dsc::load_file))
            continue;

        {
            std::vector<dsc_data> data_temp;
            data_temp.reserve(dsc_chart.data.size());

            for (const dsc_data& i : dsc_chart.data)
                switch (i.func) {
                case DSC_X_TIME:
                case DSC_X_TARGET:
                case DSC_X_TARGET_FLYING_TIME:
                    data_temp.push_back(i);
                    break;
                }

            dsc_chart.data.assign(data_temp.begin(), data_temp.end());
            data_temp.clear();
            dsc_chart.rebuild();
        }

        dsc dsc_merge;
        dsc_merge.merge(2, &_d, &dsc_chart);

        void* dsc_data = 0;
        size_t dsc_length = 0;
        dsc_merge.unparse(&dsc_data, &dsc_length);

        char path[MAX_PATH];
        strcpy_s(path, sizeof(path), "patch\\!temp\\script\\");
        sprintf_s(file_buf, sizeof(file_buf), "pv_%03d_%s.dsc", pv_id == 832 ? 800 : pv_id, i);
        strcat_s(path, sizeof(path), file_buf);

        file_stream s_dsc;
        s_dsc.open(path, "wb");
        s_dsc.write(dsc_data, dsc_length);
        s_dsc.close();

        free_def(dsc_data);
    }

    _d = d;
    {
        std::vector<dsc_data> data_temp;
        data_temp.reserve(_d.data.size());

        for (const dsc_data& i : _d.data)
            switch (i.func) {
            case DSC_X_END:
            case DSC_X_TIME:
            case DSC_X_CHANGE_FIELD:
            case DSC_X_CREDIT_TITLE:
            case DSC_X_BAR_POINT:
            case DSC_X_BEAT_POINT:
            case DSC_X_STAGE_EFFECT:
            case DSC_X_SONG_EFFECT:
            case DSC_X_SONG_EFFECT_ATTACH:
            case DSC_X_SET_STAGE_EFFECT_ENV:
            case DSC_X_CHARA_EFFECT:
                data_temp.push_back(i);
                break;
            }

        _d.data.assign(data_temp.begin(), data_temp.end());
        data_temp.clear();
        _d.rebuild();
    }

    void* dsc_data = 0;
    size_t dsc_length = 0;
    _d.unparse(&dsc_data, &dsc_length);

    char path[MAX_PATH];
    strcpy_s(path, sizeof(path), "patch\\!temp\\pv_script\\");
    sprintf_s(file_buf, sizeof(file_buf), "pv_%03d.dsc", pv_id == 832 ? 800 : pv_id);
    strcat_s(path, sizeof(path), file_buf);

    file_stream s_dsc;
    s_dsc.open(path, "wb");
    s_dsc.write(dsc_data, dsc_length);
    s_dsc.close();
    free_def(dsc_data);
}

static void x_pv_game_write_glitter(Glitter::EffectGroup* eff_group, const auth_3d_database* x_pack_auth_3d_db,
    const object_database* obj_db, const object_database* x_pack_obj_db) {
    Glitter::EffectGroup temp_eff_group(Glitter::X);
    temp_eff_group.name.assign(eff_group->name);
    temp_eff_group.version = eff_group->version;

    temp_eff_group.effects.reserve(eff_group->effects.size());
    for (Glitter::Effect* i : eff_group->effects) {
        Glitter::Effect* eff = new Glitter::Effect(Glitter::X);
        *eff = *i;
        temp_eff_group.effects.push_back(eff);
    }

    temp_eff_group.emission = eff_group->emission;
    temp_eff_group.resources_count = eff_group->resources_count;
    temp_eff_group.resource_hashes.assign(
        eff_group->resource_hashes.begin(), eff_group->resource_hashes.end());
    temp_eff_group.resources_tex = eff_group->resources_tex;
    temp_eff_group.version = eff_group->version;

    char name[0x200];
    strcpy_s(name, sizeof(name), eff_group->name.c_str());

    replace_names(name);

    data_struct* x_data = &data_list[DATA_X];
    auto& hashes = x_data->glitter_list_murmurhash;
    for (Glitter::Effect*& i : temp_eff_group.effects) {
        if (!i)
            continue;

        Glitter::Effect* e = i;
        e->name.clear();
        if (e->data.name_hash == hash_murmurhash_empty)
            continue;

        auto elem = hashes.find(e->data.name_hash);
        if (elem == hashes.end()) {
            printf_debug("Couldn't find name for hash 0x%08X\n", e->data.name_hash);
            continue;
        }

        e->name.assign(elem->second);
        replace_names(e->name);

        if (!e->data.ext_anim_x
            || (e->data.ext_anim_x->flags & Glitter::EFFECT_EXT_ANIM_CHARA_ANIM))
            continue;

        Glitter::Effect::ExtAnimX* ext_anim = e->data.ext_anim_x;

        uint32_t file_name_hash = ext_anim->file_name_hash;
        uint32_t object_hash = ext_anim->object_hash;
        uint32_t instance_id = ext_anim->instance_id;
        for (int32_t& i : auth_3d_data->loaded_ids) {
            if (i < 0 || (i & 0x7FFF) >= AUTH_3D_DATA_COUNT)
                continue;

            auth_3d* auth = &auth_3d_data->data[i & 0x7FFF];
            if (auth->id != i)
                continue;

            bool ret = false;
            if (file_name_hash != hash_murmurhash_empty) {
                if (auth->hash != file_name_hash)
                    continue;

                ret = true;
            }

            std::string uid_name;
            if (instance_id < 0) {
                for (auth_3d_object& i : auth->object)
                    if (object_hash == i.object_hash) {
                        uid_name.assign(i.uid_name);
                        ret = false;
                        break;
                    }

                if (!uid_name.size())
                    for (auth_3d_object_hrc& i : auth->object_hrc)
                        if (object_hash == i.object_hash) {
                            uid_name.assign(i.uid_name);
                            ret = false;
                            break;
                        }
            }
            else {
                int32_t obj_instance = 0;
                for (auth_3d_object& i : auth->object)
                    if (object_hash == i.object_hash) {
                        if (obj_instance == instance_id) {
                            uid_name.assign(i.uid_name);
                            ret = false;
                            break;
                        }
                        obj_instance++;
                    }

                if (!uid_name.size()) {
                    int32_t obj_hrc_instance = 0;
                    for (auth_3d_object_hrc& i : auth->object_hrc)
                        if (object_hash == i.object_hash) {
                            if (obj_hrc_instance == instance_id) {
                                uid_name.assign(i.uid_name);
                                ret = false;
                                break;
                            }
                            obj_hrc_instance++;
                        }
                }
            }

            if (ret) {
            End:
                free(e->data.ext_anim_x);
                e->data.ext_anim_x = 0;
                break;
            }
            else if (!uid_name.size())
                continue;

            replace_names(uid_name);
            object_info info = x_pack_obj_db->get_object_info(uid_name.c_str());
            ext_anim->object_hash = ((uint32_t)(uint16_t)info.set_id << 16) | (uint32_t)(uint16_t)info.id;

            if (ext_anim->file_name_hash == hash_murmurhash_empty) {
                ext_anim->file_name_hash = -1;
                break;
            }

            char auth_name[0x200];
            if (!get_replace_auth_name(auth_name, sizeof(auth_name), auth))
                goto End;

            ext_anim->file_name_hash = x_pack_auth_3d_db->get_uid(auth_name);
            break;
        }
    }

    for (Glitter::Effect*& i : temp_eff_group.effects) {
        if (!i)
            continue;

        Glitter::Effect* eff = i;
        if (eff->data.name_hash == hash_murmurhash_empty)
            continue;

        for (Glitter::Emitter*& j : eff->emitters) {
            if (!j)
                continue;

            Glitter::Emitter* emit = j;

            for (Glitter::Particle*& k : emit->particles) {
                if (!k)
                    continue;

                Glitter::Particle* ptcl = k;
                if (ptcl->data.type != Glitter::PARTICLE_MESH)
                    continue;

                object_info info;
                info.set_id = (uint32_t)ptcl->data.mesh.object_set_name_hash;
                info.id = (uint32_t)ptcl->data.mesh.object_name_hash;
                info = replace_object_info(info, obj_db, x_pack_obj_db);
                ptcl->data.mesh.object_set_name_hash = info.set_id;
                ptcl->data.mesh.object_name_hash = info.id;
            }
        }
    }

    Glitter::FileWriter::Write(Glitter::X, &temp_eff_group, "patch\\!temp\\particle_x\\",
        name, (Glitter::FileWriterFlags)(Glitter::FILE_WRITER_COMPRESS
            | Glitter::FILE_WRITER_ENCRYPT | Glitter::FILE_WRITER_NO_LIST));
}

static void x_pv_game_write_object_set(ObjsetInfo* info,
    object_database_file* x_pack_obj_db, const texture_database* tex_db,
    const texture_database* x_pack_tex_db_base, texture_database_file* x_pack_tex_db) {
    char name[0x200];
    strcpy_s(name, sizeof(name), info->name.c_str());

    for (size_t j = 0; j < sizeof(name) && name[j]; j++) {
        char c = name[j];
        if (c >= 'A' && c <= 'Z')
            name[j] += 0x20;
    }

    replace_names(name);

    if (strstr(name, "effchrpv826"))
        return;

    int32_t object_set_max_id = -1;
    for (object_set_info_file& i : x_pack_obj_db->object_set)
        object_set_max_id = max_def(object_set_max_id, (int32_t)i.id);

    int32_t texture_max_id = -1;
    for (texture_info_file& i : x_pack_tex_db->texture)
        texture_max_id = max_def(texture_max_id, (int32_t)i.id);

    for (size_t j = 0; j < sizeof(name) && name[j]; j++) {
        char c = name[j];
        if (c >= 'a' && c <= 'z')
            name[j] -= 0x20;
    }

    x_pack_obj_db->object_set.push_back({});
    object_set_info_file& obj_set_info = x_pack_obj_db->object_set.back();
    obj_set_info.name.assign(name);
    obj_set_info.id = ++object_set_max_id;

    for (size_t j = 0; j < sizeof(name); j++) {
        char c = name[j];
        if (c >= 'A' && c <= 'Z')
            name[j] += 0x20;
    }

    obj_set_info.object_file_name.assign(name);
    obj_set_info.object_file_name.append("_obj.bin");
    obj_set_info.texture_file_name.assign(name);
    obj_set_info.texture_file_name.append("_tex.bin");
    obj_set_info.archive_file_name.assign(name);
    obj_set_info.archive_file_name.append(".farc");

    farc f;
    {
        char buf[0x100];
        strcpy_s(buf, sizeof(buf), name);
        strcat_s(buf, sizeof(buf), "_obj.bin");

        prj::shared_ptr<prj::stack_allocator> alloc(new prj::stack_allocator);
        obj_set* set = alloc->allocate<obj_set>();
        set->move_data(info->obj_set, alloc);

        uint32_t tex_id_num = set->tex_id_num;
        x_pack_tex_db->texture.reserve(tex_id_num);
        for (uint32_t i = 0; i < tex_id_num; i++) {
            uint32_t& id = set->tex_id_data[i];
            const char* tex_name = tex_db->get_texture_name(id);
            if (tex_name) {
                char name_buf[0x200];
                strcpy_s(name_buf, sizeof(name_buf), tex_name);
                replace_names(name_buf);

                bool found = false;
                uint32_t hash = hash_utf8_murmurhash(name_buf);
                for (texture_info_file& j : x_pack_tex_db->texture)
                    if (hash == hash_string_murmurhash(j.name)) {
                        id = j.id;
                        found = true;
                        break;
                    }

                if (!found) {
                    uint32_t tex_id = x_pack_tex_db_base->get_texture_id(name_buf);
                    if (tex_id != -1) {
                        id = tex_id;
                        found = true;
                    }
                }

                if (!found) {
                    x_pack_tex_db->texture.push_back({});
                    texture_info_file& tex_info = x_pack_tex_db->texture.back();
                    id = ++texture_max_id;
                    tex_info.name.assign(name_buf);
                    tex_info.id = id;
                }
            }
            else
                id = -1;
        }

        uint32_t obj_num = set->obj_num;
        obj_set_info.object.reserve(obj_num);
        for (uint32_t i = 0; i < obj_num; i++) {
            obj* obj = set->obj_data[i];

            replace_names((char*)obj->name);
            obj->id = i;

            obj_set_info.object.push_back({});
            obj_set_info.object.back().id = i;
            obj_set_info.object.back().name.assign(obj->name);

            uint32_t num_material = obj->num_material;
            for (uint32_t j = 0; j < num_material; j++) {
                obj_material& material = obj->material_array[j].material;

                for (obj_material_texture_data& k : material.texdata) {
                    k.texture_index = 0;
                    if (k.tex_index == -1)
                        continue;

                    const char* tex_name = tex_db->get_texture_name(k.tex_index);
                    if (tex_name) {
                        char name_buf[0x200];
                        strcpy_s(name_buf, sizeof(name_buf), tex_name);
                        replace_names(name_buf);

                        k.tex_index = -1;
                        for (texture_info_file& i : x_pack_tex_db->texture)
                            if (!i.name.compare(name_buf)) {
                                k.tex_index = i.id;
                                break;
                            }
                    }
                    else
                        k.tex_index = -1;

                }

                if (*(int32_t*)&material.shader.name[4] == 0xDEADFF) {
                    *(int32_t*)&material.shader.name[4] = 0;
                    const char* shader_name = shaders_ft.get_name_by_index(material.shader.index);
                    strcpy_s(material.shader.name, sizeof(material.shader.name), shader_name);
                }
            }
        }

        set->modern = false;

        farc_file* ff_obj = f.add_file(buf);
        set->pack_file(&ff_obj->data, &ff_obj->size);
        ff_obj->compressed = true;
    }

    {
        char buf[0x100];
        strcpy_s(buf, sizeof(buf), name);
        strcat_s(buf, sizeof(buf), "_tex.bin");

        uint32_t tex_num = info->tex_num;
        texture** tex_data = info->tex_data;

        txp_set txp;
        txp.textures.resize(tex_num);
        for (uint32_t j = 0; j < tex_num; j++)
            texture_txp_store(tex_data[j], &txp.textures[j]);

        farc_file* ff_tex = f.add_file(buf);
        txp.pack_file(&ff_tex->data, &ff_tex->size, false);
        ff_tex->compressed = true;
    }

    char buf[0x200];
    strcpy_s(buf, sizeof(buf), "patch\\!temp\\objset\\");
    strcat_s(buf, sizeof(buf), name);

    f.write(buf, FARC_FArC, FARC_NONE, false);
}

inline static int64_t x_pv_game_write_strings_get_string_offset(
    const prj::vector_pair<string_hash, int64_t>& vec, const std::string& str) {
    std::string _str(str);
    replace_names(_str);

    uint64_t hash_fnv1a64m = hash_string_fnv1a64m(_str);
    uint64_t hash_murmurhash = hash_string_murmurhash(_str);
    for (auto& i : vec)
        if (hash_fnv1a64m == i.first.hash_fnv1a64m && hash_murmurhash == i.first.hash_murmurhash)
            return i.second;
    return 0;
}

inline static bool x_pv_game_write_file_strings_push_back_check(stream& s,
    prj::vector_pair<string_hash, int64_t>& vec, const std::string& str) {
    std::string _str(str);
    replace_names(_str);

    uint64_t hash_fnv1a64m = hash_string_fnv1a64m(_str);
    uint64_t hash_murmurhash = hash_string_murmurhash(_str);
    for (auto& i : vec)
        if (hash_fnv1a64m == i.first.hash_fnv1a64m && hash_murmurhash == i.first.hash_murmurhash)
            return false;

    vec.push_back({ _str, s.get_position() });
    s.write_string_null_terminated(_str);
    return true;
}

static void x_pv_game_write_play_param(pvpp* play_param,
    int32_t pv_id, const auth_3d_database* x_pack_auth_3d_db) {
    char path[MAX_PATH];
    sprintf_s(path, sizeof(path), "patch\\!temp\\pv\\pv%03d.pvpp", pv_id == 832 ? 800 : pv_id);

    size_t chara_count = 0;
    size_t effect_count = play_param->effect.size();

    for (pvpp_chara& i : play_param->chara)
        if (i.auth_3d.size() || i.glitter.size()
            || i.chara_effect_init && i.chara_effect.auth_3d.size())
            chara_count++;

    int64_t chara_offset = 0;
    int64_t effect_offset = 0;

    file_stream s;
    s.open(path, "wb");
    s.write(0x20);

    prj::vector_pair<string_hash, int64_t> strings;

    for (pvpp_chara& i : play_param->chara) {
        strings.reserve(i.auth_3d.size() + i.glitter.size());
        for (string_hash& j : i.auth_3d)
            x_pv_game_write_file_strings_push_back_check(s, strings, j.str);

        for (pvpp_glitter& j : i.glitter)
            x_pv_game_write_file_strings_push_back_check(s, strings, j.name.str);

        if (i.chara_effect_init && i.chara_effect.auth_3d.size()) {
            pvpp_chara_effect& chara_effect = i.chara_effect;
            strings.reserve(chara_effect.auth_3d.size() * 2);
            for (pvpp_chara_effect_auth_3d& j : chara_effect.auth_3d) {
                x_pv_game_write_file_strings_push_back_check(s, strings, j.auth_3d.str);

                if (j.has_object_set)
                    x_pv_game_write_file_strings_push_back_check(s, strings, j.object_set.str);
            }
        }
    }

    for (pvpp_effect& i : play_param->effect) {
        strings.reserve(i.auth_3d.size() + i.glitter.size());
        for (string_hash& j : i.auth_3d)
            x_pv_game_write_file_strings_push_back_check(s, strings, j.str);

        for (pvpp_glitter& j : i.glitter)
            x_pv_game_write_file_strings_push_back_check(s, strings, j.name.str);
    }
    s.align_write(0x10);

    if (chara_count) {
        chara_offset = s.get_position();
        s.write(0x20 * chara_count);

        int64_t* auth_3d_offsets = force_malloc<int64_t>(chara_count);
        int64_t* glitter_offsets = force_malloc<int64_t>(chara_count);
        int64_t* chara_effect_offsets = force_malloc<int64_t>(chara_count);

        for (pvpp_chara& i : play_param->chara) {
            if (!i.auth_3d.size() && !i.glitter.size() && (!i.chara_effect_init
                || i.chara_effect_init && !i.chara_effect.auth_3d.size()))
                continue;

            if (i.auth_3d.size()) {
                *auth_3d_offsets++ = s.get_position();
                for (string_hash& j : i.auth_3d)
                    s.write_int64_t(x_pv_game_write_strings_get_string_offset(strings, j.str));
                s.align_write(0x08);
            }
            else
                *auth_3d_offsets++ = 0;

            if (i.glitter.size()) {
                *glitter_offsets++ = s.get_position();
                for (pvpp_glitter& j : i.glitter) {
                    s.write_int64_t(x_pv_game_write_strings_get_string_offset(strings, j.name.str));
                    s.write_uint8_t(j.unk2);
                    s.align_write(0x08);
                }
                s.align_write(0x08);
            }
            else
                *glitter_offsets++ = 0;

            if (i.chara_effect_init && i.chara_effect.auth_3d.size()) {
                *chara_effect_offsets = s.get_position();
                s.write(0x10);

                int64_t auth_3d_offset = s.get_position();

                pvpp_chara_effect& chara_effect = i.chara_effect;
                for (pvpp_chara_effect_auth_3d& j : chara_effect.auth_3d) {
                    s.write_int64_t(x_pv_game_write_strings_get_string_offset(strings, j.auth_3d.str));

                    if (j.has_object_set)
                        s.write_int64_t(x_pv_game_write_strings_get_string_offset(strings, j.object_set.str));
                    else
                        s.write_int64_t(0x00);

                    s.write_uint8_t(j.u00);
                    s.align_write(0x08);
                }
                s.align_write(0x08);

                s.position_push(*chara_effect_offsets++, SEEK_SET);
                s.write_int8_t(chara_effect.base_chara);
                s.write_int8_t(chara_effect.chara_id);
                s.write_int8_t((int8_t)chara_effect.auth_3d.size());
                s.align_write(0x08);
                s.write_int64_t(auth_3d_offset);
                s.position_pop();
            }
            else
                *chara_effect_offsets++ = 0;
        }
        auth_3d_offsets -= chara_count;
        glitter_offsets -= chara_count;
        chara_effect_offsets -= chara_count;

        s.position_push(chara_offset, SEEK_SET);
        for (pvpp_chara& i : play_param->chara) {
            if (!i.auth_3d.size() && !i.glitter.size() && (!i.chara_effect_init
                || i.chara_effect_init && !i.chara_effect.auth_3d.size()))
                continue;

            s.write_int8_t((int8_t)i.auth_3d.size());
            s.write_int8_t((int8_t)i.glitter.size());
            s.align_write(0x08);
            s.write_int64_t(*auth_3d_offsets++);
            s.write_int64_t(*glitter_offsets++);
            s.write_int64_t(*chara_effect_offsets++);
        }
        auth_3d_offsets -= chara_count;
        glitter_offsets -= chara_count;
        chara_effect_offsets -= chara_count;
        s.position_pop();

        free_def(auth_3d_offsets);
        free_def(glitter_offsets);
        free_def(chara_effect_offsets);
    }

    if (effect_count) {
        effect_offset = s.get_position();
        s.write(0x20 * effect_count);

        int64_t* auth_3d_offsets = force_malloc<int64_t>(effect_count);
        int64_t* glitter_offsets = force_malloc<int64_t>(effect_count);

        for (pvpp_effect& i : play_param->effect) {
            if (i.auth_3d.size()) {
                *auth_3d_offsets++ = s.get_position();
                for (string_hash& j : i.auth_3d)
                    s.write_int64_t(x_pv_game_write_strings_get_string_offset(strings, j.str));
                s.align_write(0x08);
            }
            else
                *auth_3d_offsets++ = 0;

            if (i.glitter.size()) {
                *glitter_offsets++ = s.get_position();
                for (pvpp_glitter& j : i.glitter) {
                    s.write_int64_t(x_pv_game_write_strings_get_string_offset(strings, j.name.str));
                    s.write_uint8_t(j.unk2 ? 0x01 : 0x00);
                    s.align_write(0x08);
                }
                s.align_write(0x08);
            }
            else
                *glitter_offsets++ = 0;
        }
        auth_3d_offsets -= effect_count;
        glitter_offsets -= effect_count;

        s.position_push(effect_offset, SEEK_SET);
        for (pvpp_effect& i : play_param->effect) {
            s.write_int8_t(i.chara_id);
            s.write_int8_t((int8_t)i.auth_3d.size());
            s.write_int8_t((int8_t)i.glitter.size());
            s.align_write(0x08);
            s.write_int64_t(*auth_3d_offsets++);
            s.write_int64_t(*glitter_offsets++);
        }
        auth_3d_offsets -= effect_count;
        glitter_offsets -= effect_count;
        s.position_pop();

        free_def(auth_3d_offsets);
        free_def(glitter_offsets);
    }
    s.align_write(0x10);

    s.position_push(0x00, SEEK_SET);
    s.write_uint32_t(reverse_endianness_uint32_t('pvpp'));
    s.write_int8_t((int8_t)chara_count);
    s.write_int8_t((int8_t)effect_count);
    s.align_write(0x08);
    s.write_int64_t(chara_offset);
    s.write_int64_t(effect_offset);
    s.position_pop();
    s.close();
}

static void x_pv_game_write_stage_resource(pvsr* stage_resource,
    int32_t stage_id, const auth_3d_database* x_pack_auth_3d_db) {
    struct x_pack_pvsr_effect {
        char name[0x40];
        float_t emission;
    };

    char path[MAX_PATH];
    sprintf_s(path, sizeof(path), "patch\\!temp\\pv_stage_rsrc\\stgpv%03d_param.pvsr",
        800 + (stage_id == 32 ? 00 : stage_id));

    size_t effect_count = stage_resource->effect.size();
    size_t stage_effect_count = stage_resource->stage_effect.size();
    size_t stage_effect_env_count = stage_resource->stage_effect_env.size();

    int64_t effect_offset = 0;
    int64_t stage_effect_offset = 0;
    int64_t stage_effect_env_offset = 0;
    int64_t stage_change_effect_offset = 0;

    file_stream s;
    s.open(path, "wb");
    s.write(0x30);

    prj::vector_pair<string_hash, int64_t> strings;

    for (pvsr_effect& i : stage_resource->effect)
        x_pv_game_write_file_strings_push_back_check(s, strings, i.name.str);

    for (pvsr_stage_effect& i : stage_resource->stage_effect) {
        for (pvsr_auth_3d& j : i.auth_3d)
            x_pv_game_write_file_strings_push_back_check(s, strings, j.name.str);

        for (pvsr_glitter& j : i.glitter)
            x_pv_game_write_file_strings_push_back_check(s, strings, j.name.str);
    }

    for (pvsr_stage_effect_env& i : stage_resource->stage_effect_env)
        for (pvsr_auth_2d& j : i.aet_front_low)
            x_pv_game_write_file_strings_push_back_check(s, strings, j.name.str);

    for (int32_t i = 0; i < PVSR_STAGE_EFFECT_COUNT; i++)
        for (int32_t j = 0; j < PVSR_STAGE_EFFECT_COUNT; j++) {
            pvsr_stage_change_effect& chg_eff = stage_resource->stage_change_effect[i][j];

            for (pvsr_auth_3d& k : chg_eff.auth_3d)
                x_pv_game_write_file_strings_push_back_check(s, strings, k.name.str);

            for (pvsr_glitter& k : chg_eff.glitter)
                x_pv_game_write_file_strings_push_back_check(s, strings, k.name.str);
        }
    s.align_write(0x10);

    if (effect_count) {
        effect_offset = s.get_position();

        for (pvsr_effect& i : stage_resource->effect) {
            s.write_int64_t(x_pv_game_write_strings_get_string_offset(strings, i.name.str));
            s.write_float_t(i.emission);
            s.align_write(0x08);
        }
        s.align_write(0x08);
    }

    if (stage_effect_count) {
        stage_effect_offset = s.get_position();
        s.write(0x18 * stage_effect_count);

        int64_t* auth_3d_offsets = force_malloc<int64_t>(stage_effect_count);
        int64_t* glitter_offsets = force_malloc<int64_t>(stage_effect_count);

        for (pvsr_stage_effect& i : stage_resource->stage_effect) {
            if (i.auth_3d.size()) {
                *auth_3d_offsets++ = s.get_position();
                for (pvsr_auth_3d& j : i.auth_3d) {
                    s.write_int64_t(x_pv_game_write_strings_get_string_offset(strings, j.name.str));
                    s.write_uint8_t(j.flags);
                    s.align_write(0x08);
                }
                s.align_write(0x08);
            }
            else
                *auth_3d_offsets++ = 0;

            if (i.glitter.size()) {
                *glitter_offsets++ = s.get_position();
                for (pvsr_glitter& j : i.glitter) {
                    s.write_int64_t(x_pv_game_write_strings_get_string_offset(strings, j.name.str));
                    s.write_int8_t(j.fade_time);
                    s.write_uint8_t(j.flags);
                    s.align_write(0x08);
                }
                s.align_write(0x08);
            }
            else
                *glitter_offsets++ = 0;
        }
        auth_3d_offsets -= stage_effect_count;
        glitter_offsets -= stage_effect_count;

        s.position_push(stage_effect_offset, SEEK_SET);
        for (pvsr_stage_effect& i : stage_resource->stage_effect) {
            s.write_int8_t((int8_t)i.auth_3d.size());
            s.write_int8_t((int8_t)i.glitter.size());
            s.align_write(0x08);
            s.write_int64_t(*auth_3d_offsets++);
            s.write_int64_t(*glitter_offsets++);
        }
        auth_3d_offsets -= stage_effect_count;
        glitter_offsets -= stage_effect_count;
        s.position_pop();

        free_def(auth_3d_offsets);
        free_def(glitter_offsets);
    }

    if (stage_effect_env_count) {
        stage_effect_env_offset = s.get_position();
        s.write(0x10 * stage_effect_env_count);

        int64_t* aet_offsets = force_malloc<int64_t>(stage_effect_env_count);

        for (pvsr_stage_effect_env& i : stage_resource->stage_effect_env) {
            if (i.aet_front_low.size()) {
                *aet_offsets++ = s.get_position();
                for (pvsr_auth_2d& j : i.aet_front_low)
                    s.write_int64_t(x_pv_game_write_strings_get_string_offset(strings, j.name.str));
                s.align_write(0x08);
            }
            else
                *aet_offsets++ = 0;
        }
        aet_offsets -= stage_effect_env_count;

        s.position_push(stage_effect_env_offset, SEEK_SET);
        for (pvsr_stage_effect_env& i : stage_resource->stage_effect_env) {
            s.write_int8_t((int8_t)i.aet_front_low.size());
            s.align_write(0x08);
            s.write_int64_t(*aet_offsets++);
        }
        aet_offsets -= stage_effect_env_count;
        s.position_pop();

        free_def(aet_offsets);
    }

    {
        stage_change_effect_offset = s.get_position();
        s.write(0x18 * PVSR_STAGE_EFFECT_COUNT * PVSR_STAGE_EFFECT_COUNT);

        int64_t* auth_3d_offsets = force_malloc<int64_t>(PVSR_STAGE_EFFECT_COUNT * PVSR_STAGE_EFFECT_COUNT);
        int64_t* glitter_offsets = force_malloc<int64_t>(PVSR_STAGE_EFFECT_COUNT * PVSR_STAGE_EFFECT_COUNT);

        for (int32_t i = 0; i < PVSR_STAGE_EFFECT_COUNT; i++)
            for (int32_t j = 0; j < PVSR_STAGE_EFFECT_COUNT; j++) {
                pvsr_stage_change_effect& chg_eff = stage_resource->stage_change_effect[i][j];

                if (chg_eff.auth_3d.size()) {
                    *auth_3d_offsets++ = s.get_position();
                    for (pvsr_auth_3d& k : chg_eff.auth_3d) {
                        s.write_int64_t(x_pv_game_write_strings_get_string_offset(strings, k.name.str));
                        s.write_uint8_t(k.flags);
                        s.align_write(0x08);
                    }
                    s.align_write(0x08);
                }
                else
                    *auth_3d_offsets++ = 0;

                if (chg_eff.glitter.size()) {
                    *glitter_offsets++ = s.get_position();
                    for (pvsr_glitter& k : chg_eff.glitter) {
                        s.write_int64_t(x_pv_game_write_strings_get_string_offset(strings, k.name.str));
                        s.write_int8_t(k.fade_time);
                        s.write_uint8_t(k.flags);
                        s.align_write(0x08);
                    }
                    s.align_write(0x08);
                }
                else
                    *glitter_offsets++ = 0;
            }
        auth_3d_offsets -= PVSR_STAGE_EFFECT_COUNT * PVSR_STAGE_EFFECT_COUNT;
        glitter_offsets -= PVSR_STAGE_EFFECT_COUNT * PVSR_STAGE_EFFECT_COUNT;

        s.position_push(stage_change_effect_offset, SEEK_SET);
        for (int32_t i = 0; i < PVSR_STAGE_EFFECT_COUNT; i++)
            for (int32_t j = 0; j < PVSR_STAGE_EFFECT_COUNT; j++) {
                pvsr_stage_change_effect& chg_eff = stage_resource->stage_change_effect[i][j];

                s.write_uint8_t(chg_eff.enable ? 0x01 : 0x00);
                s.write_int8_t(chg_eff.bar_count);
                s.write_int8_t((int8_t)chg_eff.auth_3d.size());
                s.write_int8_t((int8_t)chg_eff.glitter.size());
                s.align_write(0x08);
                s.write_int64_t(*auth_3d_offsets++);
                s.write_int64_t(*glitter_offsets++);
            }
        auth_3d_offsets -= PVSR_STAGE_EFFECT_COUNT * PVSR_STAGE_EFFECT_COUNT;
        glitter_offsets -= PVSR_STAGE_EFFECT_COUNT * PVSR_STAGE_EFFECT_COUNT;
        s.position_pop();

        free_def(auth_3d_offsets);
        free_def(glitter_offsets);
    }
    s.align_write(0x10);

    s.position_push(0x00, SEEK_SET);
    s.write_uint32_t(reverse_endianness_uint32_t('pvsr'));
    s.write_int8_t((int8_t)effect_count);
    s.write_int8_t((int8_t)stage_effect_count);
    s.write_int8_t((int8_t)stage_effect_env_count);
    s.align_write(0x08);
    s.write_int64_t(effect_offset);
    s.write_int64_t(stage_effect_offset);
    s.write_int64_t(stage_effect_env_offset);
    s.write_int64_t(stage_change_effect_offset);
    s.position_pop();
    s.close();
}
#endif

static void print_dsc_command(dsc& dsc, dsc_data* dsc_data_ptr, int64_t time) {
    print_dsc_command(dsc, dsc_data_ptr, &time);
}

static void print_dsc_command(dsc& dsc, dsc_data* dsc_data_ptr, int64_t* time) {
    if (dsc_data_ptr->func < 0 || dsc_data_ptr->func >= DSC_X_MAX) {
        dw_console_printf(DW_CONSOLE_PV_SCRIPT, "UNKNOWN command(%d)\n", dsc_data_ptr->func);
        return;
    }

    if (dsc_data_ptr->func == DSC_X_TIME) {
        if (time)
            return;
    }
    else {
        if (time)
#pragma warning(suppress: 26451)
            dw_console_printf(DW_CONSOLE_PV_SCRIPT, "%.3f:", (float_t)*time * 0.00001f);
    }

    dsc_get_func_length get_func_length = dsc.get_dsc_get_func_length();

    dw_console_printf(DW_CONSOLE_PV_SCRIPT, "%s(", dsc_data_ptr->name);

    int32_t* data = dsc.get_func_data(dsc_data_ptr);

    int32_t length = get_func_length(dsc_data_ptr->func);
    for (int32_t i = 0; i < length; i++)
        if (i)
            dw_console_printf(DW_CONSOLE_PV_SCRIPT, ", %d", ((int32_t*)data)[i]);
        else
            dw_console_printf(DW_CONSOLE_PV_SCRIPT, "%d", ((int32_t*)data)[i]);

    dw_console_printf(DW_CONSOLE_PV_SCRIPT, ")\n");
}

#if BAKE_FAST
static void replace_pv832(char* str) {
    char* pv;
    pv = str;
    while (true) {
        pv = strstr(pv, "PV832");
        if (!pv)
            break;

        memcpy(pv + 2, "800", 3);
        pv += 5;
    }

    pv = str;
    while (true) {
        pv = strstr(pv, "pv832");
        if (!pv)
            break;

        memcpy(pv + 2, "800", 3);
        pv += 5;
    }
}

inline static void replace_pv832(std::string& str) {
    replace_pv832((char*)str.data());
}
#endif

#if BAKE_X_PACK
static bool get_replace_auth_name(char* name, size_t name_size, auth_3d* auth) {
    if (!name)
        return false;

    strcpy_s(name, name_size, auth->file_name.c_str());

    const char* a3da_ext = ".a3da";

    char* ext = 0;
    char* temp = name;
    while (temp = strstr(temp, a3da_ext))
        ext = temp++;

    if (ext)
        *ext = 0;

    replace_names(name);

    return !strstr(name, "EFFCHRPV826");
}

static void replace_names(char* str) {
    char* stgpv;
    stgpv = str;
    while (true) {
        stgpv = strstr(stgpv, "STGPV0");
        if (!stgpv)
            break;

        stgpv[5] = '8';
        stgpv += 6;
    }

    stgpv = str;
    while (true) {
        stgpv = strstr(stgpv, "stgpv0");
        if (!stgpv)
            break;

        stgpv[5] = '8';
        stgpv += 6;
    }

    replace_pv832(str);
}

inline static void replace_names(std::string& str) {
    replace_names((char*)str.data());
}

static object_info replace_object_info(object_info info,
    const object_database* src_obj_db, const object_database* dst_obj_db) {
    const char* _name = src_obj_db->get_object_name(info);
    if (!_name)
        return object_info();

    std::string name(_name);
    replace_names(name);
    return dst_obj_db->get_object_info(name.c_str());
}

static uint32_t replace_texture_id(uint32_t id,
    const texture_database* src_tex_db, const texture_database* dst_tex_db) {
    const char* _name = src_tex_db->get_texture_name(id);
    if (!_name)
        return -1;

    std::string name(_name);
    replace_names(name);
    return dst_tex_db->get_texture_id(name.c_str());
}
#endif
#endif
