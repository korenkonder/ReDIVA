/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "pv_db.hpp"
#include "../KKdLib/hash.hpp"
#include "../KKdLib/str_utils.hpp"
#include "hand_item.hpp"
#include "mdata_manager.hpp"
#include "render_context.hpp"

extern render_context* rctx_ptr;

pv_db::TaskPvDB* task_pv_db;

pv_db_pv_lyric::pv_db_pv_lyric() : index() {

}

pv_db_pv_lyric::~pv_db_pv_lyric() {

}

pv_db_pv_performer::pv_db_pv_performer() {
    type = PV_PERFORMER_VOCAL;
    chara = (chara_index)-1;
    costume = -1;
    pv_costume[0] = -1;
    pv_costume[1] = -1;
    pv_costume[2] = -1;
    pv_costume[3] = -1;
    pv_costume[4] = -1;
    fixed = false;
    exclude = -1;
    size = PV_PERFORMER_NORMAL;
    pseudo_same_id = -1;
    item[0] = -1;
    item[1] = -1;
    item[2] = -1;
    item[3] = -1;
}

void pv_db_pv_performer::set_pv_costume(int32_t pv_costume, int32_t difficulty) {
    if (difficulty == -1) {
        for (int32_t i = 0; i < PV_DIFFICULTY_MAX; i++)
            if (this->pv_costume[i] == -1)
                this->pv_costume[i] = pv_costume;
    }
    else if (difficulty >= 0 && difficulty < PV_DIFFICULTY_MAX)
        this->pv_costume[difficulty] = pv_costume;
}

pv_db_pv_attribute::pv_db_pv_attribute() : type() {
    reset();
}

pv_db_pv_attribute::~pv_db_pv_attribute() {

}

void pv_db_pv_attribute::reset() {
    type = PV_ATTRIBUTE_ORIGINAL;
    bitset.reset();
}

pv_db_pv_motion::pv_db_pv_motion() : index(), id() {

}

pv_db_pv_motion::~pv_db_pv_motion() {

}

pv_db_pv_field_aet::pv_db_pv_field_aet() {

}

pv_db_pv_field_aet::~pv_db_pv_field_aet() {

}

void pv_db_pv_field_aet::reset() {
    for (std::vector<std::string>& i : name) {
        i.clear();
        i.shrink_to_fit();
    }

    for (std::vector<std::string>& i : id) {
        i.clear();
        i.shrink_to_fit();
    }

    for (std::vector<int32_t>& i : frame) {
        i.clear();
        i.shrink_to_fit();
    }
}

pv_db_pv_field::pv_db_pv_field() : index(), stage_index(), ex_stage_index(),
light_frame(), light_frame_set(), stage_flag(), npr_type(), cam_blur(), sdw_off() {
    reset();
}

pv_db_pv_field::~pv_db_pv_field() {

}

void pv_db_pv_field::reset() {
    index = -1;
    stage_index = -1;
    ex_stage_index = -1;
    auth_3d_list.clear();
    auth_3d_list.shrink_to_fit();
    ex_auth_3d_list.clear();
    ex_auth_3d_list.shrink_to_fit();
    auth_3d_frame_list.clear();
    auth_3d_frame_list.shrink_to_fit();
    light.clear();
    light.shrink_to_fit();
    light_frame = -3;
    light_frame_set = false;
    aet.reset();
    spr_set_back.clear();
    spr_set_back.shrink_to_fit();
    stage_flag = true;
    npr_type = 0;
    cam_blur = 0;
    sdw_off = false;
    play_eff_list.clear();
    play_eff_list.shrink_to_fit();
    stop_eff_list.clear();
    stop_eff_list.shrink_to_fit();
    effect_rs_list.clear();
    effect_rs_list.shrink_to_fit();
    effect_emision_rs_list.clear();
    effect_emision_rs_list.shrink_to_fit();
}

pv_db_pv_field_parent::pv_db_pv_field_parent() : ex_stage_set() {

}

pv_db_pv_field_parent::~pv_db_pv_field_parent() {

}

void pv_db_pv_field_parent::reset() {
    data.clear();
    data.shrink_to_fit();
    ex_stage_set = false;
}

pv_db_pv_item::pv_db_pv_item() : index() {

}

pv_db_pv_item::~pv_db_pv_item() {

}

pv_db_pv_hand_item::pv_db_pv_hand_item() : index(), id() {

}

pv_db_pv_hand_item::~pv_db_pv_hand_item() {

}

pv_db_pv_edit_effect::pv_db_pv_edit_effect() : index(), low_field() {
    reset();
}

pv_db_pv_edit_effect::~pv_db_pv_edit_effect() {

}

void pv_db_pv_edit_effect::reset() {
    index = -1;
    name.clear();
    name.shrink_to_fit();
    low_field = false;
}

pv_db_pv_title_image::pv_db_pv_title_image() : time(), end_time() {
    reset();
}

pv_db_pv_title_image::~pv_db_pv_title_image() {

}

void pv_db_pv_title_image::reset() {
    time = -1.0f;
    end_time = -1.0f;
    aet_name.clear();
    aet_name.shrink_to_fit();
}

pv_db_pv_songinfo::pv_db_pv_songinfo() {
    reset();
}

pv_db_pv_songinfo::~pv_db_pv_songinfo() {

}

void pv_db_pv_songinfo::reset() {
    music.clear();
    music.shrink_to_fit();
    lyrics.clear();
    lyrics.shrink_to_fit();
    arranger.clear();
    arranger.shrink_to_fit();
    manipulator.clear();
    manipulator.shrink_to_fit();
    pv_editor.clear();
    pv_editor.shrink_to_fit();
    guitar_player.clear();
    guitar_player.shrink_to_fit();

    for (std::pair<std::string, std::string>& i : ex_info) {
        i.first.clear();
        i.first.shrink_to_fit();
        i.second.clear();
        i.second.shrink_to_fit();
    }
}

pv_db_pv_movie::pv_db_pv_movie() : index() {

}

pv_db_pv_movie::~pv_db_pv_movie() {

}

pv_db_pv_difficulty::pv_db_pv_difficulty() : difficulty(), edition(), level(), level_sort_index(), movie_surface(),
version(), script_format(), high_speed_rate(), hidden_timing(), sudden_timing(), edit_chara_scale() {

}

pv_db_pv_difficulty::~pv_db_pv_difficulty() {

}

const pv_db_pv_motion& pv_db_pv_difficulty::get_motion_or_default(int32_t chara_id, int32_t index) const {
    static const pv_db_pv_motion pv_db_pv_motion_null;

    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return pv_db_pv_motion_null;

    for (const pv_db_pv_motion& i : motion[chara_id])
        if (i.index == index)
            return i;

    return pv_db_pv_motion_null;
}

void pv_db_pv_difficulty::reset() {
    difficulty = PV_DIFFICULTY_MAX;
    edition = -1;
    attribute.reset();
    script_file_name.clear();
    script_file_name.shrink_to_fit();
    level = PV_LV_00_0;
    level_sort_index = 0;
    se_name.clear();
    se_name.shrink_to_fit();
    pvbranch_success_se_name.clear();
    pvbranch_success_se_name.shrink_to_fit();
    slide_name.clear();
    slide_name.shrink_to_fit();
    chainslide_first_name.clear();
    chainslide_first_name.shrink_to_fit();
    chainslide_sub_name.clear();
    chainslide_sub_name.shrink_to_fit();
    chainslide_success_name.clear();
    chainslide_success_name.shrink_to_fit();
    chainslide_failure_name.clear();
    chainslide_failure_name.shrink_to_fit();
    slidertouch_name = "slide_windchime";

    for (std::vector<pv_db_pv_motion>& i : motion) {
        i.clear();
        i.shrink_to_fit();
    }

    field.reset();
    pv_item.clear();
    pv_item.shrink_to_fit();
    hand_item.clear();
    hand_item.shrink_to_fit();

    for (auto& i : field_228) {
        i.clear();
        i.shrink_to_fit();
    }

    edit_effect.clear();
    edit_effect.shrink_to_fit();
    title_image.reset();
    songinfo.reset();
    movie_list.clear();
    movie_list.shrink_to_fit();
    movie_surface = PV_MOVIE_SURFACE_BACK;
    effect_se_file_name.clear();
    effect_se_file_name.shrink_to_fit();
    effect_se_name_list.clear();
    effect_se_name_list.shrink_to_fit();
    version = 0;
    script_format = 0;
    high_speed_rate = 1;
    hidden_timing = 0.3f;
    sudden_timing = 0.6f;
    edit_chara_scale = false;
}

pv_db_pv_ex_song_ex_auth::pv_db_pv_ex_song_ex_auth() {

}

pv_db_pv_ex_song_ex_auth::~pv_db_pv_ex_song_ex_auth() {

}

pv_db_pv_ex_song::pv_db_pv_ex_song() : chara() {
    reset();
}

pv_db_pv_ex_song::~pv_db_pv_ex_song() {

}

void pv_db_pv_ex_song::reset() {
    for (chara_index& i : chara)
        i = (chara_index)-1;
    file.clear();
    file.shrink_to_fit();
    name.clear();
    name.shrink_to_fit();
    ex_auth.clear();
    ex_auth.shrink_to_fit();
}

pv_db_pv_ex_song_parent::pv_db_pv_ex_song_parent() : chara_count() {
    reset();
}

pv_db_pv_ex_song_parent::~pv_db_pv_ex_song_parent() {

}

void pv_db_pv_ex_song_parent::reset() {
    chara_count = 0;
    data.clear();
    data.shrink_to_fit();
}

pv_db_pv_mdata::pv_db_pv_mdata() : flag() {

}

pv_db_pv_mdata::~pv_db_pv_mdata() {
    reset();
}

void pv_db_pv_mdata::reset() {
    flag = false;
    dir.clear();
    dir.shrink_to_fit();
}

pv_db_pv_osage_init::pv_db_pv_osage_init() : frame(), stage() {

}

pv_db_pv_osage_init::~pv_db_pv_osage_init() {

}

pv_db_pv_stage_param::pv_db_pv_stage_param() : stage(), mhd_id() {

}

pv_db_pv_stage_param::~pv_db_pv_stage_param() {

}

pv_db_pv_disp2d::pv_db_pv_disp2d() : target_shadow_type(), title_start_2d_field(), title_end_2d_field(),
title_start_2d_low_field(), title_end_2d_low_field(), title_start_3d_field(), title_end_3d_field() {
    reset();
}

pv_db_pv_disp2d::~pv_db_pv_disp2d() {

}

void pv_db_pv_disp2d::reset() {
    set_name.clear();
    set_name.shrink_to_fit();
    target_shadow_type = -1;
    title_start_2d_field = -1;
    title_end_2d_field = -1;
    title_start_2d_low_field = -1;
    title_end_2d_low_field = -1;
    title_start_3d_field = -1;
    title_end_3d_field = -1;
    title_2d_layer.clear();
    title_2d_layer.shrink_to_fit();
}

pv_db_pv_chreff_data::pv_db_pv_chreff_data() : type() {

}

pv_db_pv_chreff_data::~pv_db_pv_chreff_data() {

}

pv_db_pv_chreff::pv_db_pv_chreff() : id() {

}

pv_db_pv_chreff::~pv_db_pv_chreff() {

}

pv_db_pv_chrcam::pv_db_pv_chrcam() : id() {

}

pv_db_pv_chrcam::~pv_db_pv_chrcam() {

}

pv_db_pv_chrmot::pv_db_pv_chrmot() : id() {

}

pv_db_pv_chrmot::~pv_db_pv_chrmot() {

}

pv_db_pv_another_song::pv_db_pv_another_song() {

}

pv_db_pv_another_song::~pv_db_pv_another_song() {

}

pv_db_pv_frame_texture::pv_db_pv_frame_texture() : type() {

}

pv_db_pv_frame_texture::~pv_db_pv_frame_texture() {
    reset();
}

void pv_db_pv_frame_texture::reset() {
    data.clear();
    data.shrink_to_fit();
    type = PV_FRAME_TEXTURE_PRE_PP;
}

pv_db_pv_auth_replace_by_module::pv_db_pv_auth_replace_by_module() : id(), module_id() {

}

pv_db_pv_auth_replace_by_module::~pv_db_pv_auth_replace_by_module() {

}

pv_db_pv::pv_db_pv() : id(), date(), remix_parent(), sabi(), edit(), disable_calc_motfrm_limit(),
use_osage_play_data(), eyes_xrot_adjust(), is_old_pv(), eyes_base_adjust_type(), pre_play_script() {
    reset();
}

pv_db_pv::~pv_db_pv() {

}

int32_t pv_db_pv::get_chrmot_motion_id(int32_t chara_id,
    chara_index chara_index, const pv_db_pv_motion& motion) const {
    if (!chrmot.size())
        return motion.id;

    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    for (const pv_db_pv_chrmot& i : chrmot)
        if (i.id == chara_id && chara_index_get_from_chara_name(i.chara.c_str())
            == chara_index && !i.org_name.compare(motion.name)) {
            uint32_t motion_id = aft_mot_db->get_motion_id(i.name.c_str());
            if (motion_id != -1)
                return motion_id;
            break;
        }

    return motion.id;
}

const pv_db_pv_difficulty* pv_db_pv::get_difficulty(
    pv_difficulty difficulty, pv_attribute_type attribute_type) const {
    if (attribute_type == PV_ATTRIBUTE_END)
        return 0;

    const std::vector<pv_db_pv_difficulty>& diff = this->difficulty[difficulty];
    for (const pv_db_pv_difficulty& i : diff)
        if (i.attribute.type == attribute_type)
            return &i;
    return 0;
}

const pv_db_pv_difficulty* pv_db_pv::get_difficulty(pv_difficulty difficulty, int32_t edition) const {
    const std::vector<pv_db_pv_difficulty>& diff = this->difficulty[difficulty];
    for (const pv_db_pv_difficulty& i : diff)
        if (i.edition == edition)
            return &i;
    return 0;
}

chara_index pv_db_pv::get_performer_chara(int32_t performer) const {
    if (performer < 0 || performer >= ROB_CHARA_COUNT || performer >= this->performer.size())
        return CHARA_MIKU;

    chara_index chara = this->performer[performer].chara;
    pv_performer_type type = this->performer[performer].type;
    if (type >= PV_PERFORMER_PSEUDO_DEFAULT && type <= PV_PERFORMER_PSEUDO_MYCHARA) {
        performer = get_performer_pseudo_same_id(performer);
        if (performer >= 0)
            chara = this->performer[performer].chara;
    }

    if (chara < 0 || chara >= CHARA_MAX)
        chara = CHARA_MIKU;

    return chara;
}

int32_t pv_db_pv::get_performer_costume(int32_t performer) const {
    if (performer < 0 || performer >= ROB_CHARA_COUNT || performer >= this->performer.size())
        return -1;

    int32_t costume = this->performer[performer].costume;
    pv_performer_type type = this->performer[performer].type;
    if (type >= PV_PERFORMER_PSEUDO_DEFAULT && type <= PV_PERFORMER_PSEUDO_MYCHARA) {
        performer = pv_db_pv::get_performer_pseudo_same_id(performer);
        if (performer >= 0) {
            const pv_db_pv_performer* perf = &this->performer[performer];
            costume = pv_db_pv::get_pseudo_costume(type, perf->chara, perf->costume);
        }
    }
    return costume;
}

size_t pv_db_pv::get_performer_count() const {
    if (performer.size())
        return performer.size();
    return 1;
}

int32_t pv_db_pv::get_performer_exclude(int32_t performer) const {
    if (performer < 0 || performer >= ROB_CHARA_COUNT || performer >= this->performer.size())
        return -1;

    return this->performer[performer].exclude;
}

bool pv_db_pv::get_performer_fixed(int32_t performer) const {
    if (performer < 0 || performer >= ROB_CHARA_COUNT || performer >= this->performer.size())
        return false;

    return this->performer[performer].fixed;
}

size_t pv_db_pv::get_performer_fixed_count() const {
    size_t count = 0;
    for (const pv_db_pv_performer& i : performer)
        if (i.fixed)
            count++;
    return count;
}

int32_t pv_db_pv::get_performer_item(int32_t performer, pv_performer_item item) const {
    if (performer < 0 || performer >= ROB_CHARA_COUNT || performer >= this->performer.size())
        return -1;

    if (item >= 0 && item < PV_PERFORMER_ITEM_MAX)
        return this->performer[performer].item[item];
    return -1;
}

bool pv_db_pv::get_performer_pseudo_fixed(chara_index chara_index, int32_t performer, bool a4, bool a5) const {
    if (performer < 0 || performer >= ROB_CHARA_COUNT
        || chara_index < 0 || chara_index >= CHARA_MAX || performer >= this->performer.size())
        return false;

    ::chara_index chara = this->performer[performer].chara;
    pv_performer_type type = this->performer[performer].type;
    bool v11 = (!a5 ? !a4 : false) || get_performer_fixed(performer);
    if (type >= PV_PERFORMER_PSEUDO_DEFAULT && type <= PV_PERFORMER_PSEUDO_MYCHARA)
        return true;
    else if (chara < 0 || chara >= CHARA_MAX)
        return false;
    return v11;
}

int32_t pv_db_pv::get_performer_pseudo_same_id(int32_t performer) const {
    if (performer < 0 || performer >= ROB_CHARA_COUNT || performer >= this->performer.size())
        return -1;

    pv_performer_type type = this->performer[performer].type;
    if (type < PV_PERFORMER_PSEUDO_DEFAULT || type > PV_PERFORMER_PSEUDO_MYCHARA)
        return -1;

    if (type == PV_PERFORMER_PSEUDO_SAME) {
        int32_t perf = this->performer[performer].pseudo_same_id;
        if (perf != -1) {
            if (perf >= 0 && perf < this->performer.size() && performer > perf)
                return perf;
            return -1;
        }
    }

    while (--performer >= 0) {
        if (this->performer[performer].type == PV_PERFORMER_VOCAL)
            return performer;
    }
    return -1;
}

int32_t pv_db_pv::get_performer_pv_costume(int32_t performer, pv_difficulty difficulty) const {
    if (performer < 0 || performer >= ROB_CHARA_COUNT || performer >= this->performer.size())
        return -1;

    const pv_db_pv_performer* perf = &this->performer[performer];
    int32_t costume;
    if (difficulty < 0 || difficulty >= PV_DIFFICULTY_MAX)
        costume = perf->pv_costume[PV_DIFFICULTY_HARD];
    else
        costume = perf->pv_costume[difficulty];

    pv_performer_type type = perf->type;
    if (type >= PV_PERFORMER_PSEUDO_SWIM && type <= PV_PERFORMER_PSEUDO_SWIM_S) {
        performer = get_performer_pseudo_same_id(performer);
        if (performer >= 0) {
            const pv_db_pv_performer* perf = &this->performer[performer];
            if (difficulty < 0 || difficulty >= PV_DIFFICULTY_MAX)
                costume = perf->pv_costume[PV_DIFFICULTY_HARD];
            else
                costume = perf->pv_costume[difficulty];
            costume = pv_db_pv::get_pseudo_costume(perf->type, perf->chara, costume);
        }
    }
    return costume;
}

pv_performer_size pv_db_pv::get_performer_size(int32_t performer) const {
    if (performer < 0 || performer >= ROB_CHARA_COUNT || performer >= this->performer.size())
        return PV_PERFORMER_NORMAL;

    pv_performer_size size = this->performer[performer].size;
    pv_performer_type type = this->performer[performer].type;
    if (type >= PV_PERFORMER_PSEUDO_DEFAULT && type <= PV_PERFORMER_PSEUDO_MYCHARA) {
        performer = get_performer_pseudo_same_id(performer);
        if (performer >= 0)
            size = this->performer[performer].size;
    }
    return size;
}

pv_performer_type pv_db_pv::get_performer_type(int32_t performer) const {
    if (performer < 0 || performer >= ROB_CHARA_COUNT || performer >= this->performer.size())
        return PV_PERFORMER_VOCAL;

    return this->performer[performer].type;
}

bool pv_db_pv::is_performer_type_pseudo(int32_t performer) const {
    if (performer < 0 || performer >= ROB_CHARA_COUNT || performer >= this->performer.size())
        return false;

    pv_performer_type type = this->performer[performer].type;
    return type >= PV_PERFORMER_PSEUDO_DEFAULT && type <= PV_PERFORMER_PSEUDO_MYCHARA;
}

bool pv_db_pv::is_performer_type_vocal(int32_t performer) const {
    if (performer < 0 || performer >= ROB_CHARA_COUNT || performer >= this->performer.size())
        return false;

    return this->performer[performer].type == PV_PERFORMER_VOCAL;
}

void pv_db_pv::reset() {
    id = -1;
    date = -1;
    song_name.clear();
    song_name.shrink_to_fit();
    song_name_reading.clear();
    song_name_reading.shrink_to_fit();
    remix_parent = -1;
    bpm = 0;
    song_file_name.clear();
    song_file_name.shrink_to_fit();
    lyric.clear();
    lyric.shrink_to_fit();
    sabi = {};
    edit = 0;
    disable_calc_motfrm_limit = false;
    performer.clear();
    performer.shrink_to_fit();

    for (int32_t i = 0; i < PV_DIFFICULTY_MAX; i++) {
        difficulty[i].clear();
        difficulty[i].shrink_to_fit();
    }

    ex_song.reset();
    mdata.reset();
    osage_init.clear();
    osage_init.shrink_to_fit();
    stage_param.clear();
    stage_param.shrink_to_fit();
    disp2d.reset();
    use_osage_play_data = true;
    pv_expression_file_name.clear();
    pv_expression_file_name.shrink_to_fit();
    chreff.clear();
    chreff.shrink_to_fit();
    chrcam.clear();
    chrcam.shrink_to_fit();
    chrmot.clear();
    chrmot.shrink_to_fit();
    eyes_xrot_adjust = false;
    is_old_pv = false;
    eyes_base_adjust_type = EYES_BASE_ADJUST_NONE;
    eyes_rot_rate.clear();
    another_song.clear();
    another_song.shrink_to_fit();
    pre_play_script = false;

    for (int32_t i = 0; i < 6; i++)
        frame_texture[i].reset();

    auth_replace_by_module.clear();
    auth_replace_by_module.shrink_to_fit();
}

int32_t pv_db_pv::get_pseudo_costume(pv_performer_type type, chara_index chara_index, int32_t costume) {
    switch (type) {
    case PV_PERFORMER_PSEUDO_SAME:
        return costume;
    case PV_PERFORMER_PSEUDO_SWIM:
        return chara_init_data_get_swim_costume(chara_index);
    case PV_PERFORMER_PSEUDO_SWIM_S:
        return chara_init_data_get_swim_s_costume(chara_index);
    case PV_PERFORMER_PSEUDO_MYCHARA:
        return -1;
    default:
        return 0;
    }
}

namespace pv_db {
    static const char* aet_names[] = {
        "aet_front",
        "aet_front_low",
        "aet_front_3d_surf",
        "aet_back",
    };

    static const char* aet_list_names[] = {
        "aet_front_list",
        "aet_front_low_list",
        "aet_front_3d_surf_list",
        "aet_back_list",
    };

    static const char* aet_id_names[] = {
        "aet_id_front",
        "aet_id_front_low",
        "aet_id_front_3d_surf",
        "aet_id_back",
    };

    static const char* aet_id_list_names[] = {
        "aet_id_front_list",
        "aet_id_front_low_list",
        "aet_id_front_3d_surf_list",
        "aet_id_back_list",
    };

    static const char* aet_frame_names[] = {
        "aet_front_frame",
        "aet_front_low_frame",
        "aet_front_3d_surf_frame",
        "aet_back_frame",
    };

    static const char* aet_frame_list_names[] = {
        "aet_front_frame_list",
        "aet_front_low_frame_list",
        "aet_front_3d_surf_frame_list",
        "aet_back_frame_list",
    };

    static const char* difficulty_names[] = {
        "easy",
        "normal",
        "hard",
        "extreme",
        "encore",
    };

    static const char* eyes_base_adjust_type_names[] = {
        "DIRECTION",
        "CLEARANCE",
        "OFF",
    };

    static const char* frame_texture_names[] = {
        "frame_texture",
        "frame_texture_a",
        "frame_texture_b",
        "frame_texture_c",
        "frame_texture_d",
        "frame_texture_e",
    };

    static const char* frame_texture_type_names[] = {
        "frame_texture_type",
        "frame_texture_a_type",
        "frame_texture_b_type",
        "frame_texture_c_type",
        "frame_texture_d_type",
        "frame_texture_e_type",
    };

    static const char* performer_item_names[] = {
        "item_zujo",
        "item_face",
        "item_neck",
        "item_back",
    };

    static const char* pv_level_names[] = {
        "PV_LV_00_0",
        "PV_LV_00_5",
        "PV_LV_01_0",
        "PV_LV_01_5",
        "PV_LV_02_0",
        "PV_LV_02_5",
        "PV_LV_03_0",
        "PV_LV_03_5",
        "PV_LV_04_0",
        "PV_LV_04_5",
        "PV_LV_05_0",
        "PV_LV_05_5",
        "PV_LV_06_0",
        "PV_LV_06_5",
        "PV_LV_07_0",
        "PV_LV_07_5",
        "PV_LV_08_0",
        "PV_LV_08_5",
        "PV_LV_09_0",
        "PV_LV_09_5",
        "PV_LV_10_0",
    };

    TaskPvDB::TaskPvDB() : state(), reset(), field_99() {

    }

    TaskPvDB::~TaskPvDB() {

    }

    bool TaskPvDB::Init() {
        reset = false;
        field_99 = false;
        InitPerformerTypes();
        InitPerformerSizes();
        InitChrEffDataTypes();
        InitMovieSurfaces();
        return true;
    }

    bool TaskPvDB::Ctrl() {
        switch (state) {
        case 1: {
            if (reset) {
                paths.clear();
                pv_data.clear();
                reset = false;
            }

            if (paths.size()) {
                std::pair<std::string, std::string>& path = paths.front();
                file_handler.read_file(&data_list[DATA_AFT], path.first.c_str(), path.second.c_str());
                state = 2;
            }
            else
                state = 0;
        } break;
        case 2: {
            if (!file_handler.check_not_ready())
                state = 3;
        } break;
        case 3: {
            const void* data = file_handler.get_data();
            size_t size = file_handler.get_size();
            if (data && size) {
                key_val kv;
                kv.parse(data, size);

                for (int32_t i = 0; i < 1000; i++) {
                    pv_db_pv* pv = 0;
                    for (pv_db_pv& j : pv_data)
                        if (j.id == i) {
                            pv = &j;
                            break;
                        }

                    if (!pv) {
                        pv_db_pv pv;
                        if (ParsePvDb(&pv, kv, i)) {
                            ParsePvField(&pv, kv, i);
                            pv_data.push_back(pv);
                        }
                    }
                    else if (paths.front().second.find("field") != -1 || ParsePvDb(pv, kv, i))
                        ParsePvField(pv, kv, i);
                }
            }
            state = 4;
        } break;
        case 4: {
            file_handler.reset();
            paths.pop_front();
            state = 1;
        } break;
        default: {
            if (reset) {
                paths.clear();
                pv_data.clear();
                reset = false;
            }

            if (paths.size())
                state = 1;
        }
        }
        return false;
    }

    bool TaskPvDB::Dest() {
        return true;
    }

    void TaskPvDB::InitChrEffDataTypes() {
        chreff_data_types.insert({ "AUTH3D"    , PV_CHREFF_DATA_AUTH3D });
        chreff_data_types.insert({ "AUTH3D_OBJ", PV_CHREFF_DATA_AUTH3D_OBJ });
    }

    void TaskPvDB::InitMovieSurfaces() {
        movie_surfaces.insert({ "BACK" , PV_MOVIE_SURFACE_BACK });
        movie_surfaces.insert({ "FRONT", PV_MOVIE_SURFACE_FRONT });

    }

    void TaskPvDB::InitPerformerSizes() {
        performer_sizes.insert({ "NORMAL"    , PV_PERFORMER_NORMAL });
        performer_sizes.insert({ "PLAY_CHARA", PV_PERFORMER_PLAY_CHARA });
        performer_sizes.insert({ "PV_CHARA"  , PV_PERFORMER_PV_CHARA });
        performer_sizes.insert({ "SHORT"     , PV_PERFORMER_SHORT });
        performer_sizes.insert({ "TALL"      , PV_PERFORMER_TALL });
    }

    void TaskPvDB::InitPerformerTypes() {
        performer_types.insert({ "VOCAL",          PV_PERFORMER_VOCAL });
        performer_types.insert({ "PSEUDO_DEFAULT", PV_PERFORMER_PSEUDO_DEFAULT });
        performer_types.insert({ "PSEUDO_SAME",    PV_PERFORMER_PSEUDO_SAME });
        performer_types.insert({ "PSEUDO_SWIM",    PV_PERFORMER_PSEUDO_SWIM });
        performer_types.insert({ "PSEUDO_SWIM_S",  PV_PERFORMER_PSEUDO_SWIM_S });
        performer_types.insert({ "PSEUDO_MYCHARA", PV_PERFORMER_PSEUDO_MYCHARA });
        performer_types.insert({ "GUEST",          PV_PERFORMER_GUEST });
    }

    bool TaskPvDB::ParsePvDb(pv_db_pv* pv, key_val& kv, int32_t pv_id) {
        if (!pv)
            return false;

        char buf[0x100];
        sprintf_s(buf, sizeof(buf), "pv_%03d", pv_id);

        if (!kv.has_key(buf) || !kv.open_scope(buf))
            return false;

        size_t base_scope_index = kv.curr_scope - kv.scope.data();

        std::string song_name;
        if (!kv.read("song_name", song_name)) {
            kv.close_scope();
            return false;
        }

        std::string song_name_reading;
        if (!kv.read("song_name_reading", song_name_reading)) {
            kv.close_scope();
            return false;
        }

        int32_t bpm;
        if (!kv.read("bpm", bpm)) {
            kv.close_scope();
            return false;
        }

        std::string song_file_name;
        if (!kv.read("song_file_name", song_file_name)) {
            kv.close_scope();
            return false;
        }

        int32_t date;
        if (!kv.read("date", date) || pv->date >= date) {
            kv.close_scope();
            return false;
        }

        bool unsupported_script_format = false;
        for (int32_t i = 0; i < PV_DIFFICULTY_MAX; i++) {
            if (!kv.open_scope_fmt("difficulty.%s.0", difficulty_names[i]))
                continue;

            if (!kv.has_key("script_file_name")) {
                kv.close_scope();
                continue;
            }

            int32_t script_format;
            if (kv.read("script_format", script_format) && script_format > 0x15122517)
                unsupported_script_format = true;

            kv.close_scope();
        }

        if (unsupported_script_format) {
            kv.close_scope();
            return false;
        }

        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;
        stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

        pv->reset();

        pv->id = pv_id;
        pv->song_name.assign(song_name);
        pv->song_name_reading.assign(song_name_reading);
        pv->bpm = bpm;
        pv->song_file_name.assign(song_file_name);
        pv->date = date;

        int32_t remix_parent;
        if (kv.read("remix_parent", remix_parent))
            pv->remix_parent = remix_parent;

        int32_t mdata_flag;
        if (kv.read("mdata_flag", mdata_flag))
            pv->mdata.flag = mdata_flag > 0;

        int32_t mdata_dir;
        if (kv.read("mdata.dir", mdata_dir))
            pv->mdata.dir = mdata_dir;

        if (kv.open_scope("lyric")) {
            for (int32_t i = 0; i < 1000; i++) {
                if (!kv.open_scope_fmt("%03zu", i))
                    continue;

                pv_db_pv_lyric lyric;
                if (kv.read(lyric.data)) {
                    lyric.index = i;
                    pv->lyric.push_back(lyric);
                }
                kv.close_scope();
            }
            kv.close_scope();
        }

        if (kv.open_scope("sabi")) {
            float_t start_time;
            float_t play_time;
            if (kv.read("start_time", start_time) && kv.read("play_time", play_time)) {
                pv->sabi.start_time = start_time;
                pv->sabi.play_time = play_time;
            }
            kv.close_scope();
        }

        int32_t edit;
        if (kv.read("edit", edit))
            pv->edit = edit;

        if (kv.has_key("disable_calc_motfrm_limit"))
            pv->disable_calc_motfrm_limit = true;

        int32_t count;
        if (kv.read("performer", "num", count)) {
            pv->performer.resize(count);
            for (int32_t i = 0; i < count; i++) {
                if (!kv.open_scope_fmt("%zu", i))
                    continue;

                pv_db_pv_performer& performer = pv->performer[i];

                std::string type;
                if (kv.read("type", type)) {
                    auto elem = performer_types.find(type);
                    if (elem != performer_types.end())
                        performer.type = elem->second;
                }

                const char* chara;
                if (kv.read("chara", chara)) {
                    chara_index chara_index = chara_index_get_from_chara_name(chara);
                    if (chara_index != CHARA_MAX)
                        performer.chara = chara_index;
                }

                int32_t costume;
                if (kv.read("costume", costume))
                    performer.costume = costume - 1;

                int32_t pv_costume;
                if (kv.read("pv_costume", pv_costume)) {
                    pv_costume--;
                    for (int32_t j = 0; j < PV_DIFFICULTY_MAX; j++)
                        if (performer.pv_costume[j] == -1)
                            performer.pv_costume[j] = pv_costume;
                }

                if (kv.has_key("fixed"))
                    performer.fixed = true;

                int32_t pseudo_same_id;
                if (kv.read("pseudo_same_id", pseudo_same_id))
                    performer.pseudo_same_id = pseudo_same_id;

                int32_t exclude;
                if (kv.read("exclude", exclude))
                    performer.exclude = exclude;

                std::string size;
                if (kv.read("size", size)) {
                    auto elem = performer_sizes.find(size);
                    if (elem != performer_sizes.end())
                        performer.size = elem->second;
                }

                for (int32_t j = 0; j < PV_PERFORMER_ITEM_MAX; j++) {
                    int32_t item;
                    if (kv.read(performer_item_names[j], item))
                        performer.item[j] = item;
                }

                kv.close_scope();
            }
            kv.close_scope();
        }

        if (kv.read("chrcam", "length", count)) {
            pv->chrcam.resize(count);
            for (int32_t i = 0; i < count; i++) {
                if (!kv.open_scope_fmt("%zu", i))
                    continue;

                pv_db_pv_chrcam& chrcam = pv->chrcam[i];

                int32_t id;
                if (kv.read("id", id))
                    chrcam.id = id;

                std::string chara;
                if (kv.read("chara", chara))
                    chrcam.chara.assign(chara);

                std::string org_name;
                if (kv.read("org_name", org_name))
                    chrcam.org_name.assign(org_name);

                std::string name;
                if (kv.read("name", name))
                    chrcam.name.assign(name);

                kv.close_scope();
            }
            kv.close_scope();
        }

        if (kv.read("chrmot", "length", count)) {
            pv->chrmot.resize(count);
            for (int32_t i = 0; i < count; i++) {
                if (!kv.open_scope_fmt("%zu", i))
                    continue;

                pv_db_pv_chrmot& chrmot = pv->chrmot[i];

                int32_t id;
                if (kv.read("id", id))
                    chrmot.id = id;

                std::string chara;
                if (kv.read("chara", chara))
                    chrmot.chara.assign(chara);

                std::string org_name;
                if (kv.read("org_name", org_name))
                    chrmot.org_name.assign(org_name);

                std::string name;
                if (kv.read("name", name))
                    chrmot.name.assign(name);

                kv.close_scope();
            }
            kv.close_scope();
        }

        if (kv.read("chreff", "length", count)) {
            pv->chreff.resize(count);
            for (int32_t i = 0; i < count; i++) {
                if (!kv.open_scope_fmt("%zu", i))
                    continue;

                pv_db_pv_chreff& chreff = pv->chreff[i];

                int32_t id;
                if (kv.read("id", id))
                    chreff.id = id;

                std::string name;
                if (kv.read("name", name))
                    chreff.name.assign(name);

                int32_t count;
                if (kv.read("data", "length", count)) {
                    chreff.data.resize(count);
                    for (int32_t j = 0; j < count; j++) {
                        if (!kv.open_scope_fmt("%zu", j))
                            continue;

                        pv_db_pv_chreff_data& data = chreff.data[j];

                        std::string type;
                        if (kv.read("type", type)) {
                            auto elem = chreff_data_types.find(type);
                            if (elem != chreff_data_types.end())
                                data.type = elem->second;
                        }

                        std::string name;
                        if (kv.read("name", name))
                            chreff.name.assign(name);

                        kv.close_scope();
                    }
                    kv.close_scope();
                }

                kv.close_scope();
            }
            kv.close_scope();
        }

        if (kv.has_key("eyes_xrot_adjust"))
            pv->eyes_xrot_adjust = true;

        if (kv.has_key("is_old_pv"))
            pv->is_old_pv = true;

        pv->eyes_base_adjust_type = EYES_BASE_ADJUST_NONE;

        const char* eyes_base_adjust_type;
        if (kv.read("eyes_base_adjust_type", eyes_base_adjust_type))
            for (int32_t i = EYES_BASE_ADJUST_NONE; i < EYES_BASE_ADJUST_MAX; i++)
                if (!strcmp(eyes_base_adjust_type, eyes_base_adjust_type_names[i])) {
                    pv->eyes_base_adjust_type = (::eyes_base_adjust_type)i;
                    break;
                }

        if (kv.read("eyes_rot_rate", "length", count)) {
            for (int32_t i = 0; i < count; i++) {
                if (!kv.open_scope_fmt("%zu", i))
                    continue;

                const char* chr;
                if (kv.read("chr", chr)) {
                    chara_index chara_index = chara_index_get_from_chara_name(chr);
                    float_t xp_rate;
                    float_t xn_rate;
                    if (chara_index != CHARA_MAX && kv.read("xp_rate", xp_rate) && kv.read("xn_rate", xn_rate)) {
                        pv_db_pv_eyes_rot_rate eyes_rot_rate;
                        eyes_rot_rate.xp_rate = xp_rate;
                        eyes_rot_rate.xn_rate = xn_rate;
                        pv->eyes_rot_rate.insert_or_assign(chara_index, eyes_rot_rate);
                    }
                }

                kv.close_scope();
            }
            kv.close_scope();
        }

        int32_t pv_costumes[ROB_CHARA_COUNT][PV_DIFFICULTY_MAX];
        for (int32_t i = 0; i < ROB_CHARA_COUNT; i++)
            for (int32_t j = 0; j < PV_DIFFICULTY_MAX; j++)
                pv_costumes[i][j] = -1;

        if (kv.open_scope("difficulty")) {
            const char* difficulty_attribute_original;
            const char* difficulty_attribute_extra;
            const char* difficulty_attribute_slide;
            kv.read("attribute.original", difficulty_attribute_original);
            kv.read("attribute.extra", difficulty_attribute_extra);
            kv.read("attribute.slide", difficulty_attribute_slide);

            for (int32_t i = 0; i < PV_DIFFICULTY_MAX; i++) {
                std::vector<pv_db_pv_difficulty>& diff = pv->difficulty[i];

                char buf[0x200];
                sprintf_s(buf, sizeof(buf), "%s", difficulty_names[i]);

                int32_t count;
                if (!kv.read(buf, "length", count))
                    continue;

                const char* attribute_original;
                const char* attribute_extra;
                const char* attribute_slide;
                kv.read("attribute.original", attribute_original);
                kv.read("attribute.extra", attribute_extra);
                kv.read("attribute.slide", attribute_slide);

                diff.reserve(count);
                for (int32_t j = 0; j < count; j++) {
                    if (!kv.open_scope_fmt("%d", j))
                        continue;

                    pv_db_pv_difficulty d;

                    d.difficulty = (pv_difficulty)i;

                    int32_t edition;
                    if (kv.read("edition", edition))
                        d.edition = edition;

                    const char* _attribute_original;
                    if (!kv.read("attribute.original", _attribute_original)) {
                        if (attribute_original)
                            _attribute_original = attribute_original;
                        else if (difficulty_attribute_original)
                            _attribute_original = difficulty_attribute_original;
                    }

                    if (_attribute_original) {
                        d.attribute.type = PV_ATTRIBUTE_ORIGINAL;
                        d.attribute.bitset.set(0, atoi(_attribute_original) > 0);
                    }

                    const char* _attribute_extra;
                    if (!kv.read("attribute.extra", _attribute_extra)) {
                        if (attribute_extra)
                            _attribute_extra = attribute_extra;
                        else if (difficulty_attribute_extra)
                            _attribute_extra = difficulty_attribute_extra;
                    }

                    if (_attribute_extra) {
                        d.attribute.type = PV_ATTRIBUTE_EXTRA;
                        d.attribute.bitset.set(1, atoi(_attribute_extra) > 0);
                    }

                    const char* _attribute_slide;
                    if (!kv.read("attribute.slide", _attribute_slide)) {
                        if (attribute_slide)
                            _attribute_slide = attribute_slide;
                        else if (difficulty_attribute_slide)
                            _attribute_slide = difficulty_attribute_slide;
                    }

                    if (_attribute_slide)
                        d.attribute.bitset.set(2, atoi(_attribute_slide) > 0);

                    std::string script_file_name;
                    if (!kv.read("script_file_name", script_file_name)
                        || !aft_data->check_file_exists(script_file_name.c_str())) {
                        kv.close_scope();
                        continue;
                    }

                    d.script_file_name.assign(script_file_name);

                    const char* level;
                    if (kv.read("level", level))
                        for (int32_t k = PV_LV_00_0; k < PV_LV_MAX; k++)
                            if (!strcmp(level, pv_level_names[k])) {
                                d.level = (pv_level)k;
                                break;
                            }

                    int32_t level_sort_index;
                    if (kv.read("level_sort_index", level_sort_index))
                        d.level_sort_index = level_sort_index;

                    key_val_scope diff_scope = *kv.curr_scope;
                    for (int32_t k = 0; k < 2; k++) {
                        *kv.curr_scope = k ? diff_scope : kv.scope[base_scope_index];

                        std::string se_name;
                        if (kv.read("se_name", se_name))
                            d.se_name.assign(se_name);

                        std::string pvbranch_success_se_name;
                        if (kv.read("pvbranch_success_se_name", pvbranch_success_se_name))
                            d.pvbranch_success_se_name.assign(pvbranch_success_se_name);

                        std::string slide_name;
                        if (kv.read("slide_name", slide_name))
                            d.slide_name.assign(slide_name);

                        std::string chainslide_first_name;
                        if (kv.read("chainslide_first_name", chainslide_first_name))
                            d.chainslide_first_name.assign(chainslide_first_name);

                        std::string chainslide_sub_name;
                        if (kv.read("chainslide_sub_name", chainslide_sub_name))
                            d.chainslide_sub_name.assign(chainslide_sub_name);

                        std::string chainslide_success_name;
                        if (kv.read("chainslide_success_name", chainslide_success_name))
                            d.chainslide_success_name.assign(chainslide_success_name);

                        std::string chainslide_failure_name;
                        if (kv.read("chainslide_failure_name", chainslide_failure_name))
                            d.chainslide_failure_name.assign(chainslide_failure_name);

                        std::string slidertouch_name;
                        if (kv.read("slidertouch_name", slidertouch_name))
                            d.slidertouch_name.assign(slidertouch_name);

                        for (size_t l = 0; l < ROB_CHARA_COUNT; l++) {
                            if (l)
                                sprintf_s(buf, sizeof(buf), "motion%zuP", l + 1);

                            if (!kv.open_scope_fmt(l ? buf : "motion"))
                                continue;

                            for (size_t m = 0; m < 100; m++) {
                                if (!kv.open_scope_fmt("%02zu", m))
                                    continue;

                                const char* motion_name;
                                if (kv.read(motion_name)) {
                                    pv_db_pv_motion motion;
                                    motion.index = (int32_t)m;
                                    motion.id = aft_mot_db->get_motion_id(motion_name);
                                    motion.name.assign(motion_name);
                                    d.motion[l].push_back(motion);
                                }

                                kv.close_scope();
                            }
                            kv.close_scope();
                        }

                        if (kv.open_scope("pv_item")) {
                            for (size_t k = 0; k < 10; k++) {
                                if (!kv.open_scope_fmt("%02zu", k))
                                    continue;

                                std::string name;
                                if (!kv.read(name))
                                    continue;

                                pv_db_pv_item item;
                                item.index = (int32_t)k;
                                item.name.assign(name);
                                d.pv_item.push_back(item);
                                kv.close_scope();
                            }
                            kv.close_scope();
                        }

                        if (kv.open_scope("hand_item")) {
                            for (size_t k = 0; k < 20; k++) {
                                if (!kv.open_scope_fmt("%02zu", k))
                                    continue;

                                std::string name;
                                if (!kv.read(name))
                                    continue;

                                pv_db_pv_hand_item hand_item;
                                hand_item.index = (int32_t)k;
                                hand_item.id = hand_item_handler_data_get_hand_item_uid(name.c_str());
                                hand_item.name.assign(name);
                                d.hand_item.push_back(hand_item);
                                kv.close_scope();
                            }
                            kv.close_scope();
                        }

                        if (kv.open_scope("edit_effect")) {
                            for (size_t k = 0; k < 0x200; k++) {
                                if (!kv.open_scope_fmt("%02zu", k))
                                    continue;

                                std::string name;
                                if (!kv.read(name))
                                    continue;

                                pv_db_pv_edit_effect edit_effect;
                                edit_effect.index = atoi(name.c_str());
                                edit_effect.name.assign(name);
                                d.edit_effect.push_back(edit_effect);
                                kv.close_scope();
                            }
                            kv.close_scope();
                        }

                        if (kv.open_scope("edit_effect_low_field")) {
                            for (size_t k = 0; k < 0x200; k++) {
                                if (!kv.open_scope_fmt("%02zu", k))
                                    continue;

                                int32_t index;
                                if (!kv.read(index))
                                    continue;

                                if (index < d.edit_effect.size())
                                    d.edit_effect[index].low_field = true;
                                kv.close_scope();
                            }
                            kv.close_scope();
                        }

                        if (kv.open_scope("title_image")) {
                            pv_db_pv_title_image& title_image = d.title_image;

                            float_t time;
                            if (kv.read("time", time))
                                title_image.time = time;

                            float_t end_time;
                            if (kv.read("end_time", end_time))
                                title_image.end_time = end_time;

                            std::string aet_name;
                            if (kv.read("aet_name", aet_name))
                                title_image.aet_name.assign(aet_name);

                            kv.close_scope();
                        }

                        if (kv.open_scope("songinfo")) {
                            pv_db_pv_songinfo& songinfo = d.songinfo;

                            std::string music;
                            if (kv.read("music", music))
                                songinfo.music.assign(music);

                            std::string lyrics;
                            if (kv.read("lyrics", lyrics))
                                songinfo.lyrics.assign(lyrics);

                            std::string arranger;
                            if (kv.read("arranger", arranger))
                                songinfo.arranger.assign(arranger);

                            std::string manipulator;
                            if (kv.read("manipulator", manipulator))
                                songinfo.manipulator.assign(manipulator);

                            std::string pv_editor;
                            if (kv.read("pv_editor", pv_editor))
                                songinfo.pv_editor.assign(pv_editor);

                            std::string guitar_player;
                            if (kv.read("guitar_player", guitar_player))
                                songinfo.guitar_player.assign(guitar_player);

                            if (kv.open_scope("ex_info")) {
                                std::pair<std::string, std::string>* ex_info = songinfo.ex_info;

                                std::string key;
                                std::string val;
                                if (kv.read("0.key", key) && kv.read("0.val", val)) {
                                    ex_info[0].first.assign(key);
                                    ex_info[0].second.assign(val);
                                }

                                if (kv.read("1.key", key) && kv.read("1.val", val)) {
                                    ex_info[1].first.assign(key);
                                    ex_info[1].second.assign(val);
                                }

                                if (kv.read("2.key", key) && kv.read("2.val", val)) {
                                    ex_info[2].first.assign(key);
                                    ex_info[2].second.assign(val);
                                }

                                if (kv.read("3.key", key) && kv.read("3.val", val)) {
                                    ex_info[3].first.assign(key);
                                    ex_info[3].second.assign(val);
                                }

                                kv.close_scope();
                            }

                            kv.close_scope();
                        }

                        int32_t count;
                        if (!k && kv.read("movie_list", "length", count)) {
                            d.movie_list.reserve(count);
                            for (int32_t l = 0; l < count; l++) {
                                if (!kv.open_scope_fmt(l))
                                    continue;

                                pv_db_pv_movie movie;
                                if (kv.read("name", movie.name)) {
                                    movie.index = l;
                                    d.movie_list.push_back(movie);
                                }
                                kv.close_scope();
                            }
                            kv.close_scope();
                        }

                        std::string movie_file_name;
                        if (kv.read("movie_file_name", movie_file_name)) {
                            pv_db_pv_movie movie;
                            movie.index = 0;
                            movie.name.assign(movie_file_name);
                            d.movie_list.push_back(movie);
                        }

                        std::string movie_surface;
                        if (kv.read("movie_surface", movie_surface)) {
                            auto elem = movie_surfaces.find(movie_surface);
                            if (elem != movie_surfaces.end())
                                d.movie_surface = elem->second;
                        }

                        std::string effect_se_file_name;
                        if (kv.read("effect_se_file_name", effect_se_file_name))
                            d.effect_se_file_name.assign(effect_se_file_name);

                        if (!k && kv.read("effect_se_name_list", "length", count)) {
                            d.effect_se_name_list.reserve(count);
                            for (int32_t l = 0; l < count; l++) {
                                if (!kv.open_scope_fmt(l))
                                    continue;

                                std::string effect_se_name;
                                if (kv.read(effect_se_name))
                                    d.effect_se_name_list.push_back(effect_se_name);
                                kv.close_scope();
                            }
                            kv.close_scope();
                        }

                        if (k) {
                            int32_t version;
                            if (kv.read("version", version))
                                d.version = version;

                            int32_t script_format;
                            if (kv.read("version", script_format))
                                d.script_format = script_format;
                        }

                        int32_t high_speed_rate;
                        if (kv.read("high_speed_rate", high_speed_rate))
                            d.high_speed_rate = high_speed_rate;

                        float_t hidden_timing;
                        if (kv.read("hidden_timing", hidden_timing))
                            d.hidden_timing = hidden_timing;

                        float_t sudden_timing;
                        if (kv.read("sudden_timing", sudden_timing))
                            d.sudden_timing = sudden_timing;

                        int32_t edit_chara_scale;
                        if (kv.read("edit_chara_scale", edit_chara_scale))
                            d.edit_chara_scale = edit_chara_scale == 1;

                        for (int32_t l = 0; l < ROB_CHARA_COUNT; l++) {
                            if (!kv.open_scope_fmt("performer.%d.pv_costume", l))
                                continue;

                            int32_t pv_costume;
                            if (kv.read(pv_costume))
                                pv_costumes[l][d.difficulty] = pv_costume - 1;
                            kv.close_scope();
                        }
                    }

                    diff.push_back(d);

                    kv.close_scope();
                }
                kv.close_scope();
            }

            kv.close_scope();
        }

        if (kv.read("ex_song", "length", count)) {
            int32_t chara_count = -1;
            if (count <= 0)
                goto ex_song_reset;

            pv->ex_song.data.reserve(count);
            for (int32_t i = 0; i < count; i++) {
                if (!kv.open_scope_fmt(i))
                    continue;

                const char* chara;
                std::string file;
                if (!kv.read("chara", chara) || !kv.read("file", file)) {
                    kv.close_scope();
                    continue;
                }

                std::string name;
                kv.read("name", name);

                pv_db_pv_ex_song ex_song;
                ex_song.chara[0] = chara_index_get_from_chara_name(chara);

                int32_t _chara_count = 1;
                bool v533 = false;

                for (size_t j = 1; j < ROB_CHARA_COUNT; j++) {
                    sprintf_s(buf, sizeof(buf), "chara%zuP", j + 1);

                    const char* chara;
                    if (kv.read(buf, chara)) {
                        ex_song.chara[j] = chara_index_get_from_chara_name(chara);
                        if (v533)
                            goto ex_song_reset;
                    }
                    else
                        v533 = true;
                }

                ex_song.file.assign(file);
                ex_song.name.assign(name);

                int32_t count;
                if (kv.read("ex_auth", "length", count)) {
                    ex_song.ex_auth.reserve(count);
                    for (int32_t j = 0; j < count; j++) {
                        if (!kv.open_scope_fmt(j))
                            continue;

                        std::string org_name;
                        std::string name;
                        if (kv.read("org_name", org_name)
                            && kv.read("name", name)) {
                            pv_db_pv_ex_song_ex_auth ex_auth;
                            ex_auth.org_name.assign(org_name);
                            ex_auth.name.assign(name);
                            ex_song.ex_auth.push_back(ex_auth);
                        }
                        kv.close_scope();
                    }
                    kv.close_scope();
                }

                pv->ex_song.data.push_back(ex_song);
                if (chara_count < 0)
                    chara_count = _chara_count;
                else if (chara_count != _chara_count)
                    goto ex_song_reset;

                kv.close_scope();
            }

            if (chara_count >= 0)
                pv->ex_song.chara_count = chara_count;
            else
            ex_song_reset:
                pv->ex_song.reset();

            kv.close_scope();
        }

        if (kv.read("osage_init", "length", count)) {
            pv->osage_init.reserve(count);
            for (int32_t i = 0; i < count; i++) {
                if (!kv.open_scope_fmt(i))
                    continue;

                std::string motion;
                int32_t frame;
                std::string stage;
                if (kv.read("motion", motion)
                    && kv.read("frame", frame)
                    && kv.read("stage", stage)) {
                    pv_db_pv_osage_init osage_init;
                    osage_init.motion.assign(motion);
                    osage_init.frame = frame;
                    osage_init.stage = aft_stage_data->get_stage_index(stage.c_str());
                    pv->osage_init.push_back(osage_init);
                }

                kv.close_scope();
            }
            kv.close_scope();
        }

        if (kv.read("stage_param", "length", count)) {
            pv->stage_param.reserve(count);
            for (int32_t i = 0; i < count; i++) {
                if (!kv.open_scope_fmt(i))
                    continue;

                pv_db_pv_stage_param stage_param;
                std::string stage;
                if (kv.read("stage", stage))
                    stage_param.stage = aft_stage_data->get_stage_index(stage.c_str());

                int32_t mhd_id;
                if (kv.read("mhd_id", mhd_id))
                    stage_param.mhd_id = mhd_id;

                if (stage_param.stage == -1 && stage_param.mhd_id < 0) {
                    kv.close_scope();
                    break;
                }

                std::string collision_file;
                if (kv.read("collision_file", collision_file))
                    stage_param.collision_file.assign(collision_file);

                std::string wind_file;
                if (kv.read("collision_file", wind_file))
                    stage_param.wind_file.assign(wind_file);

                pv->stage_param.push_back(stage_param);

                kv.close_scope();
            }
            kv.close_scope();
        }

        if (kv.open_scope("disp2d")) {
            pv_db_pv_disp2d& disp2d = pv->disp2d;

            std::string set_name;
            if (kv.read("set_name", set_name))
                disp2d.set_name.assign(set_name);

            int32_t target_shadow_type;
            if (kv.read("target_shadow_type", target_shadow_type))
                disp2d.target_shadow_type = target_shadow_type > 0;

            int32_t title_start_2d_field;
            int32_t title_end_2d_field;
            if (kv.read("title_start_2d_field", title_start_2d_field)
                && kv.read("title_end_2d_field", title_end_2d_field)) {
                disp2d.title_start_2d_field = title_start_2d_field;
                disp2d.title_end_2d_field = title_end_2d_field;
            }

            int32_t title_start_2d_low_field;
            int32_t title_end_2d_low_field;
            if (kv.read("title_start_2d_low_field", title_start_2d_low_field)
                && kv.read("title_end_2d_low_field", title_end_2d_low_field)) {
                disp2d.title_start_2d_low_field = title_start_2d_low_field;
                disp2d.title_end_2d_low_field = title_end_2d_low_field;
            }

            int32_t title_start_3d_field;
            int32_t title_end_3d_field;
            if (kv.read("title_start_3d_field", title_start_3d_field)
                && kv.read("title_end_3d_field", title_end_3d_field)) {
                disp2d.title_start_3d_field = title_start_3d_field;
                disp2d.title_end_3d_field = title_end_3d_field;
            }

            std::string title_2d_layer;
            if (kv.read("title_2d_layer", title_2d_layer))
                disp2d.title_2d_layer.assign(title_2d_layer);

            kv.close_scope();
        }

        std::string use_osage_play_data;
        if (kv.read("use_osage_play_data", use_osage_play_data))
            pv->use_osage_play_data = !use_osage_play_data.compare("false");

        std::string pv_expression_file_name;
        if (kv.read("pv_expression.file_name", pv_expression_file_name))
            pv->pv_expression_file_name.assign(pv_expression_file_name);

        if (kv.read("another_song", "length", count)) {
            pv->another_song.reserve(count);
            for (int32_t i = 0; i < count; i++) {
                if (!kv.open_scope_fmt(i))
                    continue;

                pv_db_pv_another_song another_song;

                std::string name;
                if (kv.read("name", name))
                    another_song.name.assign(name);

                std::string song_file_name;
                if (kv.read("song_file_name", song_file_name))
                    another_song.song_file_name.assign(song_file_name);

                pv->another_song.push_back(another_song);

                kv.close_scope();
            }
            kv.close_scope();
        }

        if (kv.has_key("pre_play_script"))
            pv->pre_play_script = true;

        for (int32_t i = 0; i < 6; i++) {
            pv_db_pv_frame_texture& frame_texture = pv->frame_texture[i];

            std::string data;
            if (kv.read(frame_texture_names[i], data))
                frame_texture.data.assign(data);

            const char* type;
            if (kv.read(frame_texture_type_names[i], type)) {
                if (!strcmp(type, "PRE_PP"))
                    frame_texture.type = PV_FRAME_TEXTURE_PRE_PP;
                else if (!strcmp(type, "POST_PP"))
                    frame_texture.type = PV_FRAME_TEXTURE_POST_PP;
                else if (!strcmp(type, "FB"))
                    frame_texture.type = PV_FRAME_TEXTURE_FB;
                else
                    frame_texture.type = PV_FRAME_TEXTURE_POST_PP;
            }
            else
                frame_texture.type = PV_FRAME_TEXTURE_POST_PP;
        }

        for (int32_t i = 0; i < ROB_CHARA_COUNT; i++)
            for (int32_t j = 0; j < PV_DIFFICULTY_MAX; j++) {
                int32_t pv_costume = pv_costumes[i][j];
                if (pv_costume == -1)
                    continue;
                else if (i >= pv->performer.size())
                    break;

                pv->performer[i].set_pv_costume(pv_costume, j);
            }

        kv.close_scope();
        return true;
    }

    bool TaskPvDB::ParsePvField(pv_db_pv* pv, key_val& kv, int32_t pv_id) {
        if (!pv)
            return false;

        char buf[0x100];
        sprintf_s(buf, sizeof(buf), "pv_%03d", pv_id);

        if (!kv.has_key(buf) || !kv.open_scope(buf))
            return false;

        data_struct* aft_data = &data_list[DATA_AFT];
        stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

        for (int32_t i = 0; i < PV_DIFFICULTY_MAX; i++) {
            std::vector<pv_db_pv_difficulty>& diff = pv->difficulty[i];
            for (pv_db_pv_difficulty& j : diff) {
                if (!j.script_file_name.size())
                    continue;

                char buf[0x200];
                sprintf_s(buf, sizeof(buf), "difficulty.%s.%d.field", difficulty_names[i], j.edition);

                int32_t count;
                if (!kv.read(buf, "length", count) && !kv.read("field", "length", count))
                    continue;

                j.field.data.clear();
                j.field.ex_stage_set = false;

                j.field.data.reserve(count);
                for (int32_t k = 0; k <= count; k++) {
                    if (!kv.open_scope_fmt("%02d", k)) {
                        j.field.data.push_back({});
                        continue;
                    }

                    pv_db_pv_field field;

                    const char* stage;
                    if (kv.read("stage", stage))
                        field.stage_index = aft_stage_data->get_stage_index(stage);

                    const char* ex_stage;
                    if (kv.read("ex_stage", ex_stage)) {
                        field.ex_stage_index = aft_stage_data->get_stage_index(ex_stage);
                        if (field.ex_stage_index != -1)
                            j.field.ex_stage_set = true;
                    }

                    std::string auth_3d;
                    if (kv.read("auth_3d", auth_3d))
                        field.auth_3d_list.push_back(auth_3d);

                    int32_t count;
                    if (kv.read("auth_3d_list", "length", count)) {
                        field.auth_3d_list.reserve(count);
                        for (int32_t l = 0; l < count; l++) {
                            if (!kv.open_scope_fmt(l))
                                continue;

                            std::string auth_3d;
                            if (kv.read(auth_3d))
                                field.auth_3d_list.push_back(auth_3d);
                            kv.close_scope();
                        }
                        kv.close_scope();
                    }

                    std::string ex_auth_3d;
                    if (kv.read("ex_auth_3d", ex_auth_3d))
                        field.ex_auth_3d_list.push_back(ex_auth_3d);

                    count;
                    if (kv.read("ex_auth_3d_list", "length", count)) {
                        field.ex_auth_3d_list.reserve(count);
                        for (int32_t l = 0; l < count; l++) {
                            if (!kv.open_scope_fmt(l))
                                continue;

                            std::string ex_auth_3d;
                            if (kv.read(ex_auth_3d))
                                field.ex_auth_3d_list.push_back(ex_auth_3d);
                            kv.close_scope();
                        }
                        kv.close_scope();
                    }

                    int32_t auth_3d_frame;
                    if (kv.read("auth_3d_frame", auth_3d_frame))
                        field.auth_3d_frame_list.push_back(auth_3d_frame);

                    count;
                    if (kv.read("auth_3d_frame_list", "length", count)) {
                        field.auth_3d_frame_list.reserve(count);
                        for (int32_t l = 0; l < count; l++) {
                            if (!kv.open_scope_fmt(l))
                                continue;

                            int32_t auth_3d_frame;
                            if (kv.read(auth_3d_frame))
                                field.auth_3d_frame_list.push_back(auth_3d_frame);
                            kv.close_scope();
                        }
                        kv.close_scope();
                    }

                    kv.read("light", field.light);
                    if (kv.read("light_frame", field.light_frame))
                        field.light_frame_set = true;

                    for (int32_t l = 0; l < PV_AET_MAX; l++) {
                        std::string aet;
                        if (kv.read(aet_names[l], aet))
                            field.aet.name[l].push_back(aet);

                        int32_t count;
                        if (kv.read(aet_list_names[l], "length", count)) {
                            field.aet.name[l].reserve(count);
                            for (int32_t m = 0; m < count; m++) {
                                if (!kv.open_scope_fmt(m))
                                    continue;

                                std::string aet;
                                if (kv.read(aet))
                                    field.aet.name[l].push_back(aet);
                                kv.close_scope();
                            }
                            kv.close_scope();
                        }
                    }

                    for (int32_t l = 0; l < PV_AET_MAX; l++) {
                        std::string aet_id;
                        if (kv.read(aet_id_names[l], aet_id))
                            field.aet.id[l].push_back(aet_id);

                        int32_t count;
                        if (kv.read(aet_id_list_names[l], "length", count)) {
                            field.aet.id[l].reserve(count);
                            for (int32_t m = 0; m < count; m++) {
                                if (!kv.open_scope_fmt(m))
                                    continue;

                                std::string aet_id;
                                if (kv.read(aet_id))
                                    field.aet.id[l].push_back(aet_id);
                                kv.close_scope();
                            }
                            kv.close_scope();
                        }
                    }

                    for (int32_t l = 0; l < PV_AET_MAX; l++) {
                        int32_t aet_frame;
                        if (kv.read(aet_id_names[l], aet_frame))
                            field.aet.frame[l].push_back(aet_frame);

                        int32_t count;
                        if (kv.read(aet_id_list_names[l], "length", count)) {
                            field.aet.frame[l].reserve(count);
                            for (int32_t m = 0; m < count; m++) {
                                if (!kv.open_scope_fmt(m))
                                    continue;

                                int32_t aet_frame;
                                if (kv.read(aet_frame))
                                    field.aet.frame[l].push_back(aet_frame);
                                kv.close_scope();
                            }
                            kv.close_scope();
                        }
                    }

                    bool spr_set_back;
                    if (kv.read("spr_set_back", spr_set_back))
                        field.spr_set_back = spr_set_back;

                    bool stage_flag;
                    if (kv.read("stage_flag", stage_flag))
                        field.stage_flag = stage_flag;

                    bool npr_type;
                    if (kv.read("npr_type", npr_type))
                        field.npr_type = npr_type;

                    bool cam_blur;
                    if (kv.read("cam_blur", cam_blur))
                        field.cam_blur = cam_blur;

                    bool sdw_off;
                    if (kv.read("sdw_off", sdw_off))
                        field.sdw_off = sdw_off;

                    if (kv.read("play_eff_list", "length", count)) {
                        field.play_eff_list.reserve(count);
                        for (int32_t l = 0; l < count; l++) {
                            if (!kv.open_scope_fmt(l))
                                continue;

                            std::string play_eff;
                            if (kv.read(play_eff))
                                field.play_eff_list.push_back(play_eff);
                            kv.close_scope();
                        }
                        kv.close_scope();
                    }

                    if (kv.read("stop_eff_list", "length", count)) {
                        field.stop_eff_list.reserve(count);
                        for (int32_t l = 0; l < count; l++) {
                            if (!kv.open_scope_fmt(l))
                                continue;

                            std::string stop_eff;
                            if (kv.read(stop_eff))
                                field.stop_eff_list.push_back(stop_eff);
                            kv.close_scope();
                        }
                        kv.close_scope();
                    }

                    if (kv.read("effect_rs_list", "length", count)) {
                        field.effect_rs_list.reserve(count);
                        for (int32_t l = 0; l < count; l++) {
                            if (!kv.open_scope_fmt(l))
                                continue;

                            std::string effect_rs;
                            if (kv.read(effect_rs))
                                field.effect_rs_list.push_back(effect_rs);
                            kv.close_scope();
                        }
                        kv.close_scope();
                    }

                    if (kv.read("effect_emision_rs_list", "length", count)) {
                        field.effect_rs_list.reserve(count);
                        for (int32_t l = 0; l < count; l++) {
                            if (!kv.open_scope_fmt(l))
                                continue;

                            float_t effect_emision_rs;
                            if (kv.read(effect_emision_rs))
                                field.effect_emision_rs_list.push_back(effect_emision_rs);
                            kv.close_scope();
                        }
                        kv.close_scope();
                    }

                    j.field.data.push_back(field);

                    kv.close_scope();
                }
                kv.close_scope();
            }
        }
        kv.close_scope();
        return false;
    }
}

item_id pv_performer_item_to_item_id(pv_performer_item item) {
    static const item_id pv_performer_item_id_to_item_id_table[] = {
         ITEM_ZUJO, ITEM_MEGANE, ITEM_KUBI, ITEM_JOHA_USHIRO,
    };

    if (item >= 0 && item < PV_PERFORMER_ITEM_MAX)
        return pv_performer_item_id_to_item_id_table[item];
    return ITEM_NONE;
}

item_sub_id pv_performer_item_to_item_sub_id(pv_performer_item item) {
    static const item_sub_id pv_performer_item_id_to_item_sub_id_table[] = {
         ITEM_SUB_ZUJO, ITEM_SUB_MEGANE, ITEM_SUB_KUBI, ITEM_SUB_L_UDE,
    };

    if (item >= 0 && item < PV_PERFORMER_ITEM_MAX)
        return pv_performer_item_id_to_item_sub_id_table[item];
    return ITEM_SUB_NONE;
}

void task_pv_db_init() {
    task_pv_db = new pv_db::TaskPvDB;
}

void task_pv_db_add_paths() {
    pv_db::TaskPvDB* task_pv_db = task_pv_db_get();
    for (const std::string& i : mdata_manager_get()->GetPrefixes()) {
        std::string pv_db_file;
        pv_db_file.assign(i);
        pv_db_file.append("pv_db.txt");
        if (data_list[DATA_AFT].check_file_exists("rom/", pv_db_file.c_str()))
            task_pv_db->paths.push_back({ "rom/", pv_db_file });

        std::string pv_field_file;
        pv_field_file.assign(i);
        pv_field_file.append("pv_field.txt");
        if (data_list[DATA_AFT].check_file_exists("rom/", pv_field_file.c_str()))
            task_pv_db->paths.push_back({ "rom/", pv_field_file });
    }
}

bool task_pv_db_add_task() {
    return app::TaskWork::AddTask(task_pv_db_get(), "PV DB");
}

void task_pv_db_free_pv_data() {
    task_pv_db_get()->pv_data.clear();
}

bool task_pv_db_del_task() {
    return task_pv_db_get()->DelTask();
}

pv_db::TaskPvDB* task_pv_db_get() {
    return task_pv_db;
}

uint32_t task_pv_db_get_paths_count() {
    return (uint32_t)task_pv_db_get()->paths.size();
}

const pv_db_pv* task_pv_db_get_pv(int32_t pv_id) {
    std::list<pv_db_pv>& pv_data = task_pv_db_get()->pv_data;
    for (pv_db_pv& i : pv_data)
        if (i.id == pv_id)
            return &i;
    return 0;
}

const pv_db_pv_difficulty* task_pv_db_get_pv_difficulty(int32_t pv_id,
    pv_difficulty difficulty, pv_attribute_type attribute_type) {
    const pv_db_pv* pv = task_pv_db_get_pv(pv_id);
    if (pv)
        return pv->get_difficulty(difficulty, attribute_type);
    return 0;
}

const pv_db_pv_difficulty* task_pv_db_get_pv_difficulty(int32_t pv_id,
    pv_difficulty difficulty, int32_t edition) {
    const pv_db_pv* pv = task_pv_db_get_pv(pv_id);
    if (pv)
        return pv->get_difficulty(difficulty, edition);
    return 0;
}

bool task_pv_db_is_paths_empty() {
    return !task_pv_db_get()->paths.size();
}

void task_pv_db_free() {
    delete task_pv_db;
}
