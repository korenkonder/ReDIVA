/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "mot.h"
#include "f2_struct.h"
#include "half_t.h"
#include "hash.h"
#include "io_json.h"
#include "io_msgpack.h"
#include "io_path.h"
#include "io_stream.h"
#include "msgpack.h"
#include "str_utils.h"

const char* mot_bone_name_ac[MOT_BONE_AC_MAX] = {
    "n_hara_cp",
    "kg_hara_y",
    "kl_hara_xz",
    "kl_hara_etc",
    "n_hara",
    "cl_mune",
    "j_mune_wj",
    "e_mune_cp",
    "n_mune_b",
    "kl_mune_b_wj",
    "kl_kubi",
    "n_kao",
    "cl_kao",
    "j_kao_wj",
    "e_kao_cp",
    "face_root",
    "kl_eye_l_wj",
    "kl_eye_r_wj",
    "kl_mabu_d_l_wj",
    "kl_mabu_d_r_wj",
    "kl_mabu_l_wj",
    "kl_mabu_r_wj",
    "kl_mabu_u_l_wj",
    "kl_mabu_u_r_wj",
    "n_ago",
    "tl_ago",
    "kl_ago_wj",
    "n_ago_b",
    "tl_ago_wj",
    "tl_ha_wj",
    "n_hoho_b_l",
    "tl_hoho_b_l_wj",
    "n_hoho_b_r",
    "tl_hoho_b_r_wj",
    "n_hoho_c_l",
    "tl_hoho_c_l_wj",
    "n_hoho_c_r",
    "tl_hoho_c_r_wj",
    "n_hoho_l",
    "tl_hoho_l_wj",
    "n_hoho_r",
    "tl_hoho_r_wj",
    "n_kuti_d",
    "tl_kuti_d_wj",
    "n_kuti_d_l",
    "tl_kuti_d_l_wj",
    "n_kuti_d_r",
    "tl_kuti_d_r_wj",
    "n_kuti_l",
    "tl_kuti_l_wj",
    "n_kuti_r",
    "tl_kuti_r_wj",
    "n_kuti_u",
    "tl_kuti_u_wj",
    "n_kuti_u_l",
    "tl_kuti_u_l_wj",
    "n_kuti_u_r",
    "tl_kuti_u_r_wj",
    "n_mabu_l_d",
    "tl_mabu_l_d_wj",
    "n_mabu_l_d_l",
    "tl_mabu_l_d_l_wj",
    "n_mabu_l_d_r",
    "tl_mabu_l_d_r_wj",
    "n_mabu_l_u",
    "tl_mabu_l_u_wj",
    "n_mabu_l_u_l",
    "tl_mabu_l_u_l_wj",
    "n_mabu_l_u_r",
    "tl_mabu_l_u_r_wj",
    "n_mabu_r_d",
    "tl_mabu_r_d_wj",
    "n_mabu_r_d_l",
    "tl_mabu_r_d_l_wj",
    "n_mabu_r_d_r",
    "tl_mabu_r_d_r_wj",
    "n_mabu_r_u",
    "tl_mabu_r_u_wj",
    "n_mabu_r_u_l",
    "tl_mabu_r_u_l_wj",
    "n_mabu_r_u_r",
    "tl_mabu_r_u_r_wj",
    "n_mayu_b_l",
    "tl_mayu_b_l_wj",
    "n_mayu_b_r",
    "tl_mayu_b_r_wj",
    "n_mayu_c_l",
    "tl_mayu_c_l_wj",
    "n_mayu_c_r",
    "tl_mayu_c_r_wj",
    "n_mayu_l",
    "tl_mayu_l_wj",
    "n_mayu_r",
    "tl_mayu_r_wj",
    "n_kubi_wj_ex",
    "n_waki_l",
    "kl_waki_l_wj",
    "tl_up_kata_l",
    "c_kata_l",
    "j_kata_l_wj_cu",
    "j_ude_l_wj",
    "e_ude_l_cp",
    "kl_te_l_wj",
    "nl_hito_l_wj",
    "nl_hito_b_l_wj",
    "nl_hito_c_l_wj",
    "nl_ko_l_wj",
    "nl_ko_b_l_wj",
    "nl_ko_c_l_wj",
    "nl_kusu_l_wj",
    "nl_kusu_b_l_wj",
    "nl_kusu_c_l_wj",
    "nl_naka_l_wj",
    "nl_naka_b_l_wj",
    "nl_naka_c_l_wj",
    "nl_oya_l_wj",
    "nl_oya_b_l_wj",
    "nl_oya_c_l_wj",
    "n_ste_l_wj_ex",
    "n_sude_l_wj_ex",
    "n_sude_b_l_wj_ex",
    "n_hiji_l_wj_ex",
    "n_skata_l_wj_cd_ex",
    "n_skata_b_l_wj_cd_cu_ex",
    "n_skata_c_l_wj_cd_cu_ex",
    "n_waki_r",
    "kl_waki_r_wj",
    "tl_up_kata_r",
    "c_kata_r",
    "j_kata_r_wj_cu",
    "j_ude_r_wj",
    "e_ude_r_cp",
    "kl_te_r_wj",
    "nl_hito_r_wj",
    "nl_hito_b_r_wj",
    "nl_hito_c_r_wj",
    "nl_ko_r_wj",
    "nl_ko_b_r_wj",
    "nl_ko_c_r_wj",
    "nl_kusu_r_wj",
    "nl_kusu_b_r_wj",
    "nl_kusu_c_r_wj",
    "nl_naka_r_wj",
    "nl_naka_b_r_wj",
    "nl_naka_c_r_wj",
    "nl_oya_r_wj",
    "nl_oya_b_r_wj",
    "nl_oya_c_r_wj",
    "n_ste_r_wj_ex",
    "n_sude_r_wj_ex",
    "n_sude_b_r_wj_ex",
    "n_hiji_r_wj_ex",
    "n_skata_r_wj_cd_ex",
    "n_skata_b_r_wj_cd_cu_ex",
    "n_skata_c_r_wj_cd_cu_ex",
    "kl_kosi_y",
    "kl_kosi_xz",
    "kl_kosi_etc_wj",
    "cl_momo_l",
    "j_momo_l_wj",
    "j_sune_l_wj",
    "e_sune_l_cp",
    "kl_asi_l_wj_co",
    "kl_toe_l_wj",
    "n_hiza_l_wj_ex",
    "cl_momo_r",
    "j_momo_r_wj",
    "j_sune_r_wj",
    "e_sune_r_cp",
    "kl_asi_r_wj_co",
    "kl_toe_r_wj",
    "n_hiza_r_wj_ex",
    "n_momo_l_cd_ex",
    "n_momo_b_l_wj_ex",
    "n_momo_c_l_wj_ex",
    "n_momo_r_cd_ex",
    "n_momo_b_r_wj_ex",
    "n_momo_c_r_wj_ex",
    "n_hara_cd_ex",
    "n_hara_b_wj_ex",
    "n_hara_c_wj_ex",
};

const char* mot_bone_name_f[MOT_BONE_F_MAX] = {
    "n_hara_cp",
    "kg_hara_y",
    "kl_hara_xz",
    "kl_hara_etc",
    "n_hara",
    "cl_mune",
    "j_mune_wj",
    "e_mune_cp",
    "n_mune_b",
    "kl_mune_b_wj",
    "kl_kubi",
    "n_kao",
    "cl_kao",
    "j_kao_wj",
    "e_kao_cp",
    "face_root",
    "n_ago",
    "kl_ago_wj",
    "n_tooth_under",
    "tl_tooth_under_wj",
    "n_eye_l",
    "kl_eye_l",
    "n_eye_l_wj_ex",
    "kl_highlight_l_wj",
    "n_eye_r",
    "kl_eye_r",
    "n_eye_r_wj_ex",
    "kl_highlight_r_wj",
    "n_eyelid_l_a",
    "tl_eyelid_l_a_wj",
    "n_eyelid_l_b",
    "tl_eyelid_l_b_wj",
    "n_eyelid_r_a",
    "tl_eyelid_r_a_wj",
    "n_eyelid_r_b",
    "tl_eyelid_r_b_wj",
    "n_kuti_d",
    "tl_kuti_d_wj",
    "n_kuti_d_l",
    "tl_kuti_d_l_wj",
    "n_kuti_d_r",
    "tl_kuti_d_r_wj",
    "n_kuti_l",
    "tl_kuti_l_wj",
    "n_kuti_r",
    "tl_kuti_r_wj",
    "n_kuti_u",
    "tl_kuti_u_wj",
    "n_kuti_u_l",
    "tl_kuti_u_l_wj",
    "n_kuti_u_r",
    "tl_kuti_u_r_wj",
    "n_mabu_l_d_a",
    "tl_mabu_l_d_a_wj",
    "n_mabu_l_d_b",
    "tl_mabu_l_d_b_wj",
    "n_mabu_l_d_c",
    "tl_mabu_l_d_c_wj",
    "n_mabu_l_u_a",
    "tl_mabu_l_u_a_wj",
    "n_mabu_l_u_b",
    "tl_mabu_l_u_b_wj",
    "n_eyelashes_l",
    "tl_eyelashes_l_wj",
    "n_mabu_l_u_c",
    "tl_mabu_l_u_c_wj",
    "n_mabu_r_d_a",
    "tl_mabu_r_d_a_wj",
    "n_mabu_r_d_b",
    "tl_mabu_r_d_b_wj",
    "n_mabu_r_d_c",
    "tl_mabu_r_d_c_wj",
    "n_mabu_r_u_a",
    "tl_mabu_r_u_a_wj",
    "n_mabu_r_u_b",
    "tl_mabu_r_u_b_wj",
    "n_eyelashes_r",
    "tl_eyelashes_r_wj",
    "n_mabu_r_u_c",
    "tl_mabu_r_u_c_wj",
    "n_mayu_l",
    "tl_mayu_l_wj",
    "n_mayu_l_b",
    "tl_mayu_l_b_wj",
    "n_mayu_l_c",
    "tl_mayu_l_c_wj",
    "n_mayu_r",
    "tl_mayu_r_wj",
    "n_mayu_r_b",
    "tl_mayu_r_b_wj",
    "n_mayu_r_c",
    "tl_mayu_r_c_wj",
    "n_tooth_upper",
    "tl_tooth_upper_wj",
    "n_kubi_wj_ex",
    "n_waki_l",
    "kl_waki_l_wj",
    "tl_up_kata_l",
    "c_kata_l",
    "j_kata_l_wj_cu",
    "j_ude_l_wj",
    "e_ude_l_cp",
    "kl_te_l_wj",
    "n_hito_l_ex",
    "nl_hito_l_wj",
    "nl_hito_b_l_wj",
    "nl_hito_c_l_wj",
    "n_ko_l_ex",
    "nl_ko_l_wj",
    "nl_ko_b_l_wj",
    "nl_ko_c_l_wj",
    "n_kusu_l_ex",
    "nl_kusu_l_wj",
    "nl_kusu_b_l_wj",
    "nl_kusu_c_l_wj",
    "n_naka_l_ex",
    "nl_naka_l_wj",
    "nl_naka_b_l_wj",
    "nl_naka_c_l_wj",
    "n_oya_l_ex",
    "nl_oya_l_wj",
    "nl_oya_b_l_wj",
    "nl_oya_c_l_wj",
    "n_ste_l_wj_ex",
    "n_sude_l_wj_ex",
    "n_sude_b_l_wj_ex",
    "n_hiji_l_wj_ex",
    "n_up_kata_l_ex",
    "n_skata_l_wj_cd_ex",
    "n_skata_b_l_wj_cd_cu_ex",
    "n_skata_c_l_wj_cd_cu_ex",
    "n_waki_r",
    "kl_waki_r_wj",
    "tl_up_kata_r",
    "c_kata_r",
    "j_kata_r_wj_cu",
    "j_ude_r_wj",
    "e_ude_r_cp",
    "kl_te_r_wj",
    "n_hito_r_ex",
    "nl_hito_r_wj",
    "nl_hito_b_r_wj",
    "nl_hito_c_r_wj",
    "n_ko_r_ex",
    "nl_ko_r_wj",
    "nl_ko_b_r_wj",
    "nl_ko_c_r_wj",
    "n_kusu_r_ex",
    "nl_kusu_r_wj",
    "nl_kusu_b_r_wj",
    "nl_kusu_c_r_wj",
    "n_naka_r_ex",
    "nl_naka_r_wj",
    "nl_naka_b_r_wj",
    "nl_naka_c_r_wj",
    "n_oya_r_ex",
    "nl_oya_r_wj",
    "nl_oya_b_r_wj",
    "nl_oya_c_r_wj",
    "n_ste_r_wj_ex",
    "n_sude_r_wj_ex",
    "n_sude_b_r_wj_ex",
    "n_hiji_r_wj_ex",
    "n_up_kata_r_ex",
    "n_skata_r_wj_cd_ex",
    "n_skata_b_r_wj_cd_cu_ex",
    "n_skata_c_r_wj_cd_cu_ex",
    "kl_kosi_y",
    "kl_kosi_xz",
    "kl_kosi_etc_wj",
    "cl_momo_l",
    "j_momo_l_wj",
    "j_sune_l_wj",
    "e_sune_l_cp",
    "kl_asi_l_wj_co",
    "kl_toe_l_wj",
    "n_hiza_l_wj_ex",
    "cl_momo_r",
    "j_momo_r_wj",
    "j_sune_r_wj",
    "e_sune_r_cp",
    "kl_asi_r_wj_co",
    "kl_toe_r_wj",
    "n_hiza_r_wj_ex",
    "n_momo_a_l_wj_cd_ex",
    "n_momo_b_l_wj_ex",
    "n_momo_c_l_wj_ex",
    "n_momo_a_r_wj_cd_ex",
    "n_momo_b_r_wj_ex",
    "n_momo_c_r_wj_ex",
    "n_hara_cd_ex",
    "n_hara_b_wj_ex",
    "n_hara_c_wj_ex",
};

const char* mot_bone_name_ft[MOT_BONE_FT_MAX] = {
    "n_hara_cp",
    "kg_hara_y",
    "kl_hara_xz",
    "kl_hara_etc",
    "n_hara",
    "cl_mune",
    "j_mune_wj",
    "e_mune_cp",
    "n_mune_b",
    "kl_mune_b_wj",
    "kl_kubi",
    "n_kao",
    "cl_kao",
    "j_kao_wj",
    "e_kao_cp",
    "face_root",
    "n_ago",
    "kl_ago_wj",
    "n_tooth_under",
    "tl_tooth_under_wj",
    "n_eye_l",
    "kl_eye_l",
    "n_eye_l_wj_ex",
    "kl_highlight_l_wj",
    "n_eye_r",
    "kl_eye_r",
    "n_eye_r_wj_ex",
    "kl_highlight_r_wj",
    "n_eyelid_l_a",
    "tl_eyelid_l_a_wj",
    "n_eyelid_l_b",
    "tl_eyelid_l_b_wj",
    "n_eyelid_r_a",
    "tl_eyelid_r_a_wj",
    "n_eyelid_r_b",
    "tl_eyelid_r_b_wj",
    "n_kuti_d",
    "tl_kuti_d_wj",
    "n_kuti_d_l",
    "tl_kuti_d_l_wj",
    "n_kuti_d_r",
    "tl_kuti_d_r_wj",
    "n_kuti_ds_l",
    "tl_kuti_ds_l_wj",
    "n_kuti_ds_r",
    "tl_kuti_ds_r_wj",
    "n_kuti_l",
    "tl_kuti_l_wj",
    "n_kuti_m_l",
    "tl_kuti_m_l_wj",
    "n_kuti_m_r",
    "tl_kuti_m_r_wj",
    "n_kuti_r",
    "tl_kuti_r_wj",
    "n_kuti_u",
    "tl_kuti_u_wj",
    "n_kuti_u_l",
    "tl_kuti_u_l_wj",
    "n_kuti_u_r",
    "tl_kuti_u_r_wj",
    "n_mabu_l_d_a",
    "tl_mabu_l_d_a_wj",
    "n_mabu_l_d_b",
    "tl_mabu_l_d_b_wj",
    "n_mabu_l_d_c",
    "tl_mabu_l_d_c_wj",
    "n_mabu_l_u_a",
    "tl_mabu_l_u_a_wj",
    "n_mabu_l_u_b",
    "tl_mabu_l_u_b_wj",
    "n_eyelashes_l",
    "tl_eyelashes_l_wj",
    "n_mabu_l_u_c",
    "tl_mabu_l_u_c_wj",
    "n_mabu_r_d_a",
    "tl_mabu_r_d_a_wj",
    "n_mabu_r_d_b",
    "tl_mabu_r_d_b_wj",
    "n_mabu_r_d_c",
    "tl_mabu_r_d_c_wj",
    "n_mabu_r_u_a",
    "tl_mabu_r_u_a_wj",
    "n_mabu_r_u_b",
    "tl_mabu_r_u_b_wj",
    "n_eyelashes_r",
    "tl_eyelashes_r_wj",
    "n_mabu_r_u_c",
    "tl_mabu_r_u_c_wj",
    "n_mayu_l",
    "tl_mayu_l_wj",
    "n_mayu_l_b",
    "tl_mayu_l_b_wj",
    "n_mayu_l_c",
    "tl_mayu_l_c_wj",
    "n_mayu_r",
    "tl_mayu_r_wj",
    "n_mayu_r_b",
    "tl_mayu_r_b_wj",
    "n_mayu_r_c",
    "tl_mayu_r_c_wj",
    "n_tooth_upper",
    "tl_tooth_upper_wj",
    "n_kubi_wj_ex",
    "n_waki_l",
    "kl_waki_l_wj",
    "tl_up_kata_l",
    "c_kata_l",
    "j_kata_l_wj_cu",
    "j_ude_l_wj",
    "e_ude_l_cp",
    "kl_te_l_wj",
    "n_hito_l_ex",
    "nl_hito_l_wj",
    "nl_hito_b_l_wj",
    "nl_hito_c_l_wj",
    "n_ko_l_ex",
    "nl_ko_l_wj",
    "nl_ko_b_l_wj",
    "nl_ko_c_l_wj",
    "n_kusu_l_ex",
    "nl_kusu_l_wj",
    "nl_kusu_b_l_wj",
    "nl_kusu_c_l_wj",
    "n_naka_l_ex",
    "nl_naka_l_wj",
    "nl_naka_b_l_wj",
    "nl_naka_c_l_wj",
    "n_oya_l_ex",
    "nl_oya_l_wj",
    "nl_oya_b_l_wj",
    "nl_oya_c_l_wj",
    "n_ste_l_wj_ex",
    "n_sude_l_wj_ex",
    "n_sude_b_l_wj_ex",
    "n_hiji_l_wj_ex",
    "n_up_kata_l_ex",
    "n_skata_l_wj_cd_ex",
    "n_skata_b_l_wj_cd_cu_ex",
    "n_skata_c_l_wj_cd_cu_ex",
    "n_waki_r",
    "kl_waki_r_wj",
    "tl_up_kata_r",
    "c_kata_r",
    "j_kata_r_wj_cu",
    "j_ude_r_wj",
    "e_ude_r_cp",
    "kl_te_r_wj",
    "n_hito_r_ex",
    "nl_hito_r_wj",
    "nl_hito_b_r_wj",
    "nl_hito_c_r_wj",
    "n_ko_r_ex",
    "nl_ko_r_wj",
    "nl_ko_b_r_wj",
    "nl_ko_c_r_wj",
    "n_kusu_r_ex",
    "nl_kusu_r_wj",
    "nl_kusu_b_r_wj",
    "nl_kusu_c_r_wj",
    "n_naka_r_ex",
    "nl_naka_r_wj",
    "nl_naka_b_r_wj",
    "nl_naka_c_r_wj",
    "n_oya_r_ex",
    "nl_oya_r_wj",
    "nl_oya_b_r_wj",
    "nl_oya_c_r_wj",
    "n_ste_r_wj_ex",
    "n_sude_r_wj_ex",
    "n_sude_b_r_wj_ex",
    "n_hiji_r_wj_ex",
    "n_up_kata_r_ex",
    "n_skata_r_wj_cd_ex",
    "n_skata_b_r_wj_cd_cu_ex",
    "n_skata_c_r_wj_cd_cu_ex",
    "kl_kosi_y",
    "kl_kosi_xz",
    "kl_kosi_etc_wj",
    "cl_momo_l",
    "j_momo_l_wj",
    "j_sune_l_wj",
    "e_sune_l_cp",
    "kl_asi_l_wj_co",
    "kl_toe_l_wj",
    "n_hiza_l_wj_ex",
    "cl_momo_r",
    "j_momo_r_wj",
    "j_sune_r_wj",
    "e_sune_r_cp",
    "kl_asi_r_wj_co",
    "kl_toe_r_wj",
    "n_hiza_r_wj_ex",
    "n_momo_a_l_wj_cd_ex",
    "n_momo_b_l_wj_ex",
    "n_momo_c_l_wj_ex",
    "n_momo_a_r_wj_cd_ex",
    "n_momo_b_r_wj_ex",
    "n_momo_c_r_wj_ex",
    "n_hara_cd_ex",
    "n_hara_b_wj_ex",
    "n_hara_c_wj_ex",
};

vector_func(mot)

mot_set* mot_init() {
    mot_set* ms = force_malloc(sizeof(mot_set));
    return ms;
}

void mot_read_mot(mot_set* ms, char* path) {
    if (!ms || !path)
        return;

    wchar_t* path_buf = char_string_to_wchar_t_string(path);
    mot_wread_mot(ms, path_buf);
    free(path_buf);
}

void mot_wread_mot(mot_set* ms, wchar_t* path) {
    if (!ms || !path)
        return;

    vector_mot_free(&ms->vec);
    memset(ms, 0, sizeof(mot_set));

    wchar_t* path_bin = str_utils_wadd(path, L".bin");
    wchar_t* path_mot = str_utils_wadd(path, L".mot");
    if (path_wcheck_file_exists(path_bin)) {
        stream* s = io_wopen(path_bin, L"rb");
        if (s->io.stream) {
            typedef struct mot_header {
                uint32_t key_set_count_offset;
                uint32_t key_set_types_offset;
                uint32_t key_set_offset;
                uint32_t bone_info_offset;
            } mot_header;

            size_t count = 0;
            while (io_read_uint64_t(s) != 0) {
                io_read_uint64_t(s);
                count++;
            }

            if (count == 0)
                goto MotBinEnd;

            io_set_position(s, 0x00, SEEK_SET);
            vector_mot_append(&ms->vec, count);
            mot_header* mh = force_malloc_s(mot_header, count);
            for (size_t i = 0; i < count; i++) {
                mh[i].key_set_count_offset = io_read_uint32_t(s);
                mh[i].key_set_types_offset = io_read_uint32_t(s);
                mh[i].key_set_offset = io_read_uint32_t(s);
                mh[i].bone_info_offset = io_read_uint32_t(s);
            }

            for (size_t i = 0; i < count; i++) {
                mot m;
                memset(&m, 0, sizeof(mot));

                io_set_position(s, mh[i].bone_info_offset, SEEK_SET);
                m.bone_info_count = 0;
                io_read_uint16_t(s);
                do
                    m.bone_info_count++;
                while (io_read_uint16_t(s) != 0 && io_get_position(s) < s->length);

                io_set_position(s, mh[i].bone_info_offset, SEEK_SET);
                m.bone_info = force_malloc_s(mot_bone_info, m.bone_info_count);
                for (size_t j = 0; j < m.bone_info_count; j++)
                    m.bone_info[j].index = io_read_uint16_t(s);

                io_set_position(s, mh[i].key_set_count_offset, SEEK_SET);
                m.info = io_read_uint16_t(s);
                m.frame_count = io_read_uint16_t(s);

                m.key_set = force_malloc_s(mot_key_set, m.key_set_count);
                io_set_position(s, mh[i].key_set_types_offset, SEEK_SET);
                for (int32_t j = 0, b = 0; j < m.key_set_count; j++) {
                    if (j % 8 == 0)
                        b = io_read_uint16_t(s);

                    m.key_set[j].type = (b >> (j % 8 * 2)) & 0x03;
                }

                io_set_position(s, mh[i].key_set_offset, SEEK_SET);
                for (int32_t j = 0; j < m.key_set_count; j++) {
                    mot_key_set* mks = &m.key_set[j];
                    if (mks->type == MOT_KEY_SET_NONE) {
                        mks->keys_count = 0;
                        mks->keys = 0;
                    }
                    else if (mks->type == MOT_KEY_SET_STATIC) {
                        mks->keys_count = 1;
                        mks->keys = force_malloc_s(kft2, mks->keys_count);
                        mks->keys[0].frame = 0.0f;
                        mks->keys[0].value = io_read_float_t(s);
                    }
                    else if (mks->type == MOT_KEY_SET_HERMITE
                        || mks->type == MOT_KEY_SET_HERMITE_TANGENT) {
                        mks->keys_count = io_read_uint16_t(s);
                        mks->keys = force_malloc_s(kft2, mks->keys_count);
                        for (uint32_t k = 0; k < mks->keys_count; k++)
                            mks->keys[k].frame = io_read_uint16_t(s);
                        io_align_read(s, 0x04);

                        if (mks->type == MOT_KEY_SET_HERMITE_TANGENT)
                            for (uint32_t k = 0; k < mks->keys_count; k++) {
                                mks->keys[k].value = io_read_float_t(s);
                                mks->keys[k].tangent = io_read_float_t(s);
                            }
                        else
                            for (uint32_t k = 0; k < mks->keys_count; k++) {
                                mks->keys[k].value = io_read_float_t(s);
                                mks->keys[k].tangent = 0.0f;
                            }
                    }
                }
                vector_mot_push_back(&ms->vec, &m);
            }
            free(mh);

        MotBinEnd:
            ms->is_x = false;
            ms->modern = false;
            ms->ready = true;
        }
        io_dispose(s);
    }
    else if (path_wcheck_file_exists(path_mot)) {
        f2_struct st;
        f2_struct_wread(&st, path_mot);
        if (st.data) {
            typedef struct mot_header {
                uint32_t hash;
                int64_t name_offset;
                int64_t key_set_count_offset;
                int64_t key_set_types_offset;
                int64_t key_set_offset;
                int64_t bone_info_offset;
                int64_t bone_hash_offset;
                int32_t bone_info_count;
            } mot_header;

            stream* s = io_open_memory(st.data, st.length);
            s->is_big_endian = st.header.use_big_endian;

            io_set_position(s, 0x0C, SEEK_SET);
            bool is_x = io_read_uint32_t_stream_reverse_endianness(s) == 0;

            io_set_position(s, 0x00, SEEK_SET);
            vector_mot_append(&ms->vec, 1);
            mot_header mh;
            memset(&mh, 0, sizeof(mot_header));
            if (!is_x) {
                mh.hash = (uint32_t)io_read_uint64_t_stream_reverse_endianness(s);
                mh.name_offset = io_read_int32_t_stream_reverse_endianness(s);
                mh.key_set_count_offset = io_read_int32_t_stream_reverse_endianness(s);
                mh.key_set_types_offset = io_read_int32_t_stream_reverse_endianness(s);
                mh.key_set_offset = io_read_int32_t_stream_reverse_endianness(s);
                mh.bone_info_offset = io_read_int32_t_stream_reverse_endianness(s);
                mh.bone_hash_offset = io_read_int32_t_stream_reverse_endianness(s);
                mh.bone_info_count = io_read_int32_t_stream_reverse_endianness(s);


                mh.name_offset -= 0x40;
                mh.key_set_count_offset -= 0x40;
                mh.key_set_types_offset -= 0x40;
                mh.key_set_offset -= 0x40;
                mh.bone_info_offset -= 0x40;
                mh.bone_hash_offset -= 0x40;
            }
            else {
                mh.hash = (uint32_t)io_read_uint64_t_stream_reverse_endianness(s);
                mh.name_offset = io_read_int64_t_stream_reverse_endianness(s);
                mh.key_set_count_offset = io_read_int64_t_stream_reverse_endianness(s);
                mh.key_set_types_offset = io_read_int64_t_stream_reverse_endianness(s);
                mh.key_set_offset = io_read_int64_t_stream_reverse_endianness(s);
                mh.bone_info_offset = io_read_int64_t_stream_reverse_endianness(s);
                mh.bone_hash_offset = io_read_int64_t_stream_reverse_endianness(s);
                mh.bone_info_count = io_read_int32_t_stream_reverse_endianness(s);
            }

            mot m;
            memset(&m, 0, sizeof(mot));
            m.div_frames = io_read_uint16_t_stream_reverse_endianness(s);;
            m.div_count = io_read_uint8_t(s);;

            io_read_char_buffer_string_null_terminated_offset(s, mh.name_offset, false, &m.name);

            io_set_position(s, mh.bone_info_offset, SEEK_SET);
            m.bone_info_count = mh.bone_info_count;
            m.bone_info = force_malloc_s(mot_bone_info, m.bone_info_count);
            if (!is_x)
                for (size_t j = 0; j < mh.bone_info_count; j++)
                    io_read_char_buffer_string_null_terminated_offset(s,
                        io_read_int32_t_stream_reverse_endianness(s) - 0x40LL, true, &m.bone_info[j].name);
            else
                for (size_t j = 0; j < mh.bone_info_count; j++)
                    io_read_char_buffer_string_null_terminated_offset(s,
                        io_read_int64_t_stream_reverse_endianness(s), true, &m.bone_info[j].name);

            io_set_position(s, mh.bone_hash_offset, SEEK_SET);
            for (size_t j = 0; j < mh.bone_info_count; j++)
                io_read_uint64_t_stream_reverse_endianness(s);

            io_set_position(s, mh.key_set_count_offset, SEEK_SET);
            m.info = io_read_uint16_t_stream_reverse_endianness(s);
            m.frame_count = io_read_uint16_t_stream_reverse_endianness(s);

            m.key_set = force_malloc_s(mot_key_set, m.key_set_count);
            io_set_position(s, mh.key_set_types_offset, SEEK_SET);
            for (int32_t j = 0, b = 0; j < m.key_set_count; j++) {
                if (j % 8 == 0)
                    b = io_read_uint16_t_stream_reverse_endianness(s);

                m.key_set[j].type = (b >> (j % 8 * 2)) & 0x03;
            }

            io_set_position(s, mh.key_set_offset, SEEK_SET);
            for (int32_t j = 0; j < m.key_set_count; j++) {
                mot_key_set* mks = &m.key_set[j];
                if (mks->type == MOT_KEY_SET_NONE) {
                    mks->keys_count = 0;
                    mks->keys = 0;
                }
                else if (mks->type == MOT_KEY_SET_STATIC) {
                    mks->keys_count = 1;
                    mks->keys = force_malloc_s(kft2, mks->keys_count);
                    mks->keys[0].frame = 0.0f;
                    mks->keys[0].value = io_read_float_t_stream_reverse_endianness(s);
                }
                else if (mks->type == MOT_KEY_SET_HERMITE
                    || mks->type == MOT_KEY_SET_HERMITE_TANGENT) {
                    mks->keys_count = io_read_uint16_t_stream_reverse_endianness(s);
                    mks->data_type = io_read_uint16_t_stream_reverse_endianness(s);
                    mks->keys = force_malloc_s(kft2, mks->keys_count);
                    if (mks->type == MOT_KEY_SET_HERMITE_TANGENT)
                        for (uint32_t k = 0; k < mks->keys_count; k++)
                            mks->keys[k].tangent = io_read_float_t_stream_reverse_endianness(s);
                    else
                        for (uint32_t k = 0; k < mks->keys_count; k++)
                            mks->keys[k].tangent = 0.0f;

                    if (mks->data_type == MOT_KEY_SET_DATA_F16)
                        for (uint32_t k = 0; k < mks->keys_count; k++)
                            mks->keys[k].value = half_to_float(io_read_half_t_stream_reverse_endianness(s));
                    else
                        for (uint32_t k = 0; k < mks->keys_count; k++)
                            mks->keys[k].value = io_read_float_t_stream_reverse_endianness(s);
                    io_align_read(s, 0x04);

                    for (uint32_t k = 0; k < mks->keys_count; k++)
                        mks->keys[k].frame = (float_t)io_read_uint16_t_stream_reverse_endianness(s);
                    io_align_read(s, 0x04);
                }
            }

            m.murmurhash = st.header.murmurhash;
            vector_mot_push_back(&ms->vec, &m);

            ms->is_x = is_x;
            ms->modern = true;
            ms->ready = true;
            io_dispose(s);
        }
        f2_struct_free(&st);
    }
    free(path_bin);
    free(path_mot);
}

void mot_write_mot(mot_set* ms, char* path) {
    if (!ms || !path || !ms->ready)
        return;

    wchar_t* path_buf = char_string_to_wchar_t_string(path);
    mot_wwrite_mot(ms, path_buf);
    free(path_buf);
}

void mot_wwrite_mot(mot_set* ms, wchar_t* path) {
    if (!ms || !path || !ms->ready)
        return;

    if (!ms->modern) {
        wchar_t* path_bin = str_utils_wadd(path, L".bin");
        stream* s = io_wopen(path_bin, L"wb");
        if (s->io.stream) {
            typedef struct mot_header {
                uint32_t key_set_count_offset;
                uint32_t key_set_types_offset;
                uint32_t key_set_offset;
                uint32_t bone_info_offset;
            } mot_header;

            size_t count = ms->vec.end - ms->vec.begin;
            io_set_position(s, count * 0x10 + 0x10, SEEK_SET);
            mot_header* mh = force_malloc_s(mot_header, count);
            for (size_t i = 0; i < count; i++) {
                mot* m = &ms->vec.begin[i];

                mh[i].key_set_count_offset = (uint32_t)io_get_position(s);
                io_write_uint16_t(s, m->info);
                io_write_uint16_t(s, m->frame_count);

                mh[i].key_set_types_offset = (uint32_t)io_get_position(s);
                uint16_t key_set_type_buf = 0;
                for (int32_t j = 0; j < m->key_set_count; j++) {
                    key_set_type_buf |= ((uint16_t)m->key_set[j].type & 0x03) << (j % 8 * 2);

                    if (j % 8 == 7) {
                        io_write_uint16_t(s, key_set_type_buf);
                        key_set_type_buf = 0;
                    }
                }

                if (m->key_set_count % 8 != 0)
                    io_write_uint16_t(s, key_set_type_buf);
                io_align_write(s, 0x04);

                mh[i].key_set_offset = (uint32_t)io_get_position(s);
                for (int32_t j = 0; j < m->key_set_count; j++) {
                    mot_key_set* mks = &m->key_set[j];
                    if (mks->type == MOT_KEY_SET_STATIC)
                        io_write_float_t(s, mks->keys[0].value);
                    else if (mks->type == MOT_KEY_SET_HERMITE
                        || mks->type == MOT_KEY_SET_HERMITE_TANGENT) {
                        io_write_uint16_t(s, (uint16_t)mks->keys_count);
                        for (size_t k = 0; k < mks->keys_count; k++)
                            io_write_uint16_t(s, (uint16_t)mks->keys[k].frame);

                        if (io_get_position(s) % 4 != 0)
                            io_write_uint16_t(s, 0x00);

                        if (mks->type == MOT_KEY_SET_HERMITE_TANGENT)
                            for (size_t k = 0; k < mks->keys_count; k++) {
                                io_write_float_t(s, mks->keys[k].value);
                                io_write_float_t(s, mks->keys[k].tangent);
                            }
                        else
                            for (size_t k = 0; k < mks->keys_count; k++)
                                io_write_float_t(s, mks->keys[k].value);
                    }
                }
                io_align_write(s, 0x04);

                mh[i].bone_info_offset = (uint32_t)io_get_position(s);
                if (m->bone_info_count)
                    for (int32_t j = 0; j < m->bone_info_count; j++)
                        io_write_uint16_t(s, m->bone_info[j].index);
                else
                    io_write_uint16_t(s, 0x00);
                io_write_uint16_t(s, 0x00);
            }
            io_align_write(s, 0x04);

            io_set_position(s, 0x00, SEEK_SET);
            for (size_t i = 0; i < count; i++) {
                io_write_uint32_t(s, mh[i].key_set_count_offset);
                io_write_uint32_t(s, mh[i].key_set_types_offset);
                io_write_uint32_t(s, mh[i].key_set_offset);
                io_write_uint32_t(s, mh[i].bone_info_offset);
            }
            free(mh);
        }
        io_dispose(s);
        free(path_bin);
    }
    else {
        wchar_t* path_mot = str_utils_wadd(path, L".mot");
        stream* s = io_open_memory(0, 0);
        uint32_t o;
        vector_enrs_entry e = { 0, 0, 0 };
        enrs_entry ee;
        vector_size_t pof = { 0, 0, 0 };
        size_t pof_offset = !ms->is_x ? 0x40 : 0x00;
        uint32_t murmurhash = 0;
        if (ms->vec.end - ms->vec.begin > 0) {
            typedef struct mot_header {
                uint32_t hash;
                int64_t name_offset;
                int64_t key_set_count_offset;
                int64_t key_set_types_offset;
                int64_t key_set_offset;
                int64_t bone_info_offset;
                int64_t bone_hash_offset;
                int32_t bone_info_count;
            } mot_header;

            mot_header mh;
            memset(&mh, 0, sizeof(mot_header));
            mot* m = ms->vec.begin;

            if (!ms->is_x) {
                ee = (enrs_entry){ 0, 3, 48, 1, { 0, 0, 0 } };
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_TYPE_QWORD });
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 7, ENRS_TYPE_DWORD });
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_TYPE_WORD });
                vector_enrs_entry_push_back(&e, &ee);
                o = 48;
            }
            else {
                ee = (enrs_entry){ 0, 3, 64, 1, { 0, 0, 0 } };
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 7, ENRS_TYPE_QWORD });
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_TYPE_DWORD });
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_TYPE_WORD });
                vector_enrs_entry_push_back(&e, &ee);
                o = 64;
            };

            ee = (enrs_entry){ o, 1, 4, 1, { 0, 0, 0 } };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_TYPE_WORD });
            vector_enrs_entry_push_back(&e, &ee);
            o = 4;

            ee = (enrs_entry){ o, 1,  (m->key_set_count + 3) / 4, 1, { 0, 0, 0 } };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, (m->key_set_count + 7) / 8, ENRS_TYPE_WORD });
            vector_enrs_entry_push_back(&e, &ee);
            o = (m->key_set_count + 3) / 4;
            o = align_val(o, 4);

            for (int32_t j = 0; j < m->key_set_count; j++) {
                mot_key_set* mks = &m->key_set[j];
                if (mks->type == MOT_KEY_SET_STATIC) {
                    ee = (enrs_entry){ o, 1, 4, 1, { 0, 0, 0 } };
                    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_TYPE_DWORD });
                    vector_enrs_entry_push_back(&e, &ee);
                    o = 4;
                }
                else if (mks->type == MOT_KEY_SET_HERMITE
                    || mks->type == MOT_KEY_SET_HERMITE_TANGENT) {
                    if (mks->type == MOT_KEY_SET_HERMITE_TANGENT)
                        ee = (enrs_entry){ o, 1, 4, 1, { 0, 0, 0 } };
                    else
                        ee = (enrs_entry){ o, 1, 3, 1, { 0, 0, 0 } };

                    o = 4;
                    if (mks->type == MOT_KEY_SET_HERMITE_TANGENT)
                        o += mks->keys_count * 4;

                    if (mks->data_type == MOT_KEY_SET_DATA_F16)
                        o += align_val(mks->keys_count * 2, 4);
                    else
                        o += mks->keys_count * 4;
                    o += align_val(mks->keys_count * 2, 4);
                    o = align_val(o, 4);
                    ee.size = o;

                    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_TYPE_WORD });
                    if (mks->type == MOT_KEY_SET_HERMITE_TANGENT)
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, mks->keys_count, ENRS_TYPE_DWORD });
                    if (mks->data_type == MOT_KEY_SET_DATA_F16) {
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, mks->keys_count, ENRS_TYPE_WORD });
                        vector_enrs_sub_entry_push_back(&ee.sub,
                            &(enrs_sub_entry){ mks->keys_count % 2 == 1 ? 2 : 0, mks->keys_count, ENRS_TYPE_WORD });
                    }
                    else {
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, mks->keys_count, ENRS_TYPE_DWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, mks->keys_count, ENRS_TYPE_WORD });
                    }
                    vector_enrs_entry_push_back(&e, &ee);
                }
            }
            o = align_val(o, 16);

            if (!ms->is_x) {
                ee = (enrs_entry){ o, 1, m->bone_info_count * 4, 1, { 0, 0, 0 } };
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, m->bone_info_count, ENRS_TYPE_DWORD });
                vector_enrs_entry_push_back(&e, &ee);
                o = m->bone_info_count * 4;
            }
            else {
                ee = (enrs_entry){ o, 1, m->bone_info_count * 8, 1, { 0, 0, 0 } };
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, m->bone_info_count, ENRS_TYPE_QWORD });
                vector_enrs_entry_push_back(&e, &ee);
                o = m->bone_info_count * 8;
            }
            o = align_val(o, 16);

            ee = (enrs_entry){ o, 1, m->bone_info_count * 8, 1, { 0, 0, 0 } };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, m->bone_info_count, ENRS_TYPE_QWORD });
            vector_enrs_entry_push_back(&e, &ee);
            o = m->bone_info_count * 8;

            if (!ms->is_x) {
                io_write_uint64_t(s, 0);
                pof_add(s, &pof, pof_offset);
                io_write_int32_t(s, 0);
                pof_add(s, &pof, pof_offset);
                io_write_int32_t(s, 0);
                pof_add(s, &pof, pof_offset);
                io_write_int32_t(s, 0);
                pof_add(s, &pof, pof_offset);
                io_write_int32_t(s, 0);
                pof_add(s, &pof, pof_offset);
                io_write_int32_t(s, 0);
                pof_add(s, &pof, pof_offset);
                io_write_int32_t(s, 0);
                io_write_int32_t(s, 0);
            }
            else {
                io_write_uint64_t(s, 0);
                pof_add(s, &pof, pof_offset);
                io_write_int64_t(s, 0);
                pof_add(s, &pof, pof_offset);
                io_write_int64_t(s, 0);
                pof_add(s, &pof, pof_offset);
                io_write_int64_t(s, 0);
                pof_add(s, &pof, pof_offset);
                io_write_int64_t(s, 0);
                pof_add(s, &pof, pof_offset);
                io_write_int64_t(s, 0);
                pof_add(s, &pof, pof_offset);
                io_write_int64_t(s, 0);
                io_write_int32_t(s, 0);
            }
            io_write_uint16_t(s, 0);
            io_write_uint8_t(s, 0);
            io_align_write(s, 0x10);

            mh.hash = hash_char_murmurhash(string_access(&m->name), 0, false);

            mh.key_set_count_offset = io_get_position(s);
            io_write_uint16_t(s, m->info);
            io_write_uint16_t(s, m->frame_count);

            mh.key_set_types_offset = io_get_position(s);
            uint16_t key_set_type_buf = 0;
            for (int32_t j = 0; j < m->key_set_count; j++) {
                key_set_type_buf |= ((uint16_t)m->key_set[j].type & 0x03) << (j % 8 * 2);

                if (j % 8 == 7) {
                    io_write_uint16_t(s, key_set_type_buf);
                    key_set_type_buf = 0;
                }
            }

            if (m->key_set_count % 8 != 0)
                io_write_uint16_t(s, key_set_type_buf);
            io_align_write(s, 0x04);

            mh.key_set_offset = io_get_position(s);
            for (int32_t j = 0; j < m->key_set_count; j++) {
                mot_key_set* mks = &m->key_set[j];
                if (mks->type == MOT_KEY_SET_STATIC)
                    io_write_float_t(s, mks->keys[0].value);
                else if (mks->type == MOT_KEY_SET_HERMITE
                    || mks->type == MOT_KEY_SET_HERMITE_TANGENT) {
                    io_write_uint16_t(s, (uint16_t)mks->keys_count);
                    io_write_uint16_t(s, (uint16_t)mks->data_type);
                    if (mks->type == MOT_KEY_SET_HERMITE_TANGENT)
                        for (uint32_t k = 0; k < mks->keys_count; k++)
                            io_write_float_t(s, mks->keys[k].tangent);

                    if (mks->data_type == MOT_KEY_SET_DATA_F16)
                        for (uint32_t k = 0; k < mks->keys_count; k++)
                            io_write_half_t(s, float_to_half(mks->keys[k].value));
                    else
                        for (uint32_t k = 0; k < mks->keys_count; k++)
                            io_write_float_t(s, mks->keys[k].value);
                    io_align_read(s, 0x04);

                    for (uint32_t k = 0; k < mks->keys_count; k++)
                        io_write_uint16_t(s, (uint16_t)mks->keys[k].frame);
                    io_align_read(s, 0x04);
                }
            }
            io_align_write(s, 0x10);

            size_t* bone_info_offsets = force_malloc_s(size_t, m->bone_info_count);
            mh.bone_info_offset = io_get_position(s);
            if (!ms->is_x)
                for (int32_t j = 0; j < m->bone_info_count; j++) {
                    pof_add(s, &pof, pof_offset);
                    io_write_uint32_t(s, 0);
                }
            else
                for (int32_t j = 0; j < m->bone_info_count; j++) {
                    pof_add(s, &pof, pof_offset);
                    io_write_uint64_t(s, 0);
                }
            io_align_write(s, 0x10);

            mh.bone_hash_offset = io_get_position(s);
            for (int32_t j = 0; j < m->bone_info_count; j++)
                io_write_uint64_t(s, hash_char_murmurhash(
                    string_access(&m->bone_info[j].name), 0, false));
            io_align_write(s, 0x10);

            mh.name_offset = io_get_position(s);
            io_write_char_string_null_terminated(s, string_access(&m->name));

            for (int32_t j = 0; j < m->bone_info_count; j++) {
                bone_info_offsets[j] = io_get_position(s);
                io_write_char_string_null_terminated(s, string_access(&m->bone_info[j].name));
            }
            io_align_write(s, 0x10);

            io_set_position(s, mh.bone_info_offset, SEEK_SET);
            mh.bone_info_count = m->bone_info_count;
            if (!ms->is_x)
                for (int32_t j = 0; j < m->bone_info_count; j++)
                    io_write_int32_t(s, (int32_t)(bone_info_offsets[j] + 0x40));
            else
                for (int32_t j = 0; j < m->bone_info_count; j++)
                    io_write_int64_t(s, bone_info_offsets[j]);
            free(bone_info_offsets);

            io_set_position(s, 0x00, SEEK_SET);
            if (!ms->is_x) {
                io_write_uint64_t(s, (uint64_t)mh.hash);
                io_write_int32_t(s, (int32_t)(mh.name_offset + 0x40));
                io_write_int32_t(s, (int32_t)(mh.key_set_count_offset + 0x40));
                io_write_int32_t(s, (int32_t)(mh.key_set_types_offset + 0x40));
                io_write_int32_t(s, (int32_t)(mh.key_set_offset + 0x40));
                io_write_int32_t(s, (int32_t)(mh.bone_info_offset + 0x40));
                io_write_int32_t(s, (int32_t)(mh.bone_hash_offset + 0x40));
                io_write_int32_t(s, mh.bone_info_count);
            }
            else {
                io_write_uint64_t(s, (uint64_t)mh.hash);
                io_write_int64_t(s, mh.name_offset);
                io_write_int64_t(s, mh.key_set_count_offset);
                io_write_int64_t(s, mh.key_set_types_offset);
                io_write_int64_t(s, mh.key_set_offset);
                io_write_int64_t(s, mh.bone_info_offset);
                io_write_int64_t(s, mh.bone_hash_offset);
                io_write_int32_t(s, mh.bone_info_count);
            }

            if (m->div_frames > 0) {
                io_write_uint16_t(s, m->div_frames);
                io_write_uint8_t(s, m->div_count);
            }
            else {
                io_write_uint16_t(s, 0);
                io_write_uint8_t(s, 0);
            }

            murmurhash = m->murmurhash;
        }

        f2_struct st;
        memset(&st, 0, sizeof(f2_struct));
        st.length = s->io.data.vec.end - s->io.data.vec.begin;
        st.data = force_malloc(st.length);
        memcpy(st.data, s->io.data.vec.begin, st.length);
        io_dispose(s);

        st.enrs = e;
        st.pof = pof;

        st.header.signature = reverse_endianness_uint32_t('MOTC');
        st.header.length = 0x40;
        st.header.use_big_endian = false;
        st.header.use_section_size = true;
        st.header.murmurhash = murmurhash;
        st.header.inner_signature = 0xFF010008;

        f2_struct_wwrite(&st, path_mot, true, ms->is_x);
        f2_struct_free(&st);
        free(path_mot);
    }
}

void mot_read_mp(mot_set* ms, char* path, bool json) {
    if (!ms || !path)
        return;

    wchar_t* path_buf = char_string_to_wchar_t_string(path);
    mot_wread_mp(ms, path_buf, json);
    free(path_buf);
}

void mot_wread_mp(mot_set* ms, wchar_t* path, bool json) {
    if (!ms || !path)
        return;

    vector_mot_free(&ms->vec);
    memset(ms, 0, sizeof(mot_set));

    wchar_t* path_mp = str_utils_wadd(path, json ? L".json" : L".mp");
    if (!path_wcheck_file_exists(path_mp)) {
        free(path_mp);
        return;
    }

    stream* s = io_wopen(path_mp, L"rb");
    msgpack msg;
    if (json)
        io_json_read(s, &msg);
    else
        io_msgpack_read(s, &msg);

    msgpack* _mot;
    _mot = msgpack_read(&msg, L"MOT");
    if (_mot && _mot->type == MSGPACK_ARRAY) {
        msgpack_array* ptr = SELECT_MSGPACK_PTR(msgpack_array, _mot);
        size_t length = ptr->end - ptr->begin;
        vector_mot_append(&ms->vec, length);
        for (size_t i = 0; i < length; i++) {
            if (ptr->begin[i].type != MSGPACK_MAP)
                continue;

            msgpack* _m = &ptr->begin[i];
            mot m;
            memset(&m, 0, sizeof(mot));
            m.frame_count = msgpack_read_uint16_t(_m, L"FrameCount");
            m.high_bits = msgpack_read_uint16_t(_m, L"HighBits");

            msgpack* key_sets = msgpack_read(_m, L"KeySets");
            if (key_sets->type == MSGPACK_ARRAY) {
                msgpack_array* key_sets_ptr = SELECT_MSGPACK_PTR(msgpack_array, key_sets);
                m.key_set_count = (uint16_t)(key_sets_ptr->end - key_sets_ptr->begin);
                m.key_set = force_malloc_s(mot_key_set, m.key_set_count);
                for (size_t j = 0; j < m.key_set_count; j++) {
                    mot_key_set* mks = &m.key_set[j];
                    msgpack* key_set = &key_sets_ptr->begin[j];
                    if (key_set->type != MSGPACK_ARRAY)
                        goto BinKeyNull;

                    msgpack_array* key_set_ptr = SELECT_MSGPACK_PTR(msgpack_array, key_set);
                    if (key_set_ptr->end - key_set_ptr->begin != 2)
                        continue;

                    mks->type = msgpack_read_int32_t(&key_set_ptr->begin[0], 0);
                    msgpack* keys = &key_set_ptr->begin[1];
                    if (mks->type < MOT_KEY_SET_STATIC
                        || mks->type > MOT_KEY_SET_HERMITE_TANGENT
                        || keys->type != MSGPACK_ARRAY)
                        goto BinKeyNull;

                    msgpack_array* keys_ptr = SELECT_MSGPACK_PTR(msgpack_array, keys);
                    mks->keys_count = (uint32_t)(keys_ptr->end - keys_ptr->begin);
                    if (mks->keys_count < 1)
                        goto BinKeyNull;
                    else if (mks->type == MOT_KEY_SET_STATIC) {
                        mks->keys = force_malloc_s(kft2, 1);
                        mks->keys[0].frame = 0;
                        mks->keys[0].value = msgpack_read_float_t(&keys_ptr->begin[0], 0);
                        mks->keys[0].tangent = 0;
                    }
                    else {
                        mks->keys = force_malloc_s(kft2, mks->keys_count);
                        kft2* kft = mks->keys;
                        for (size_t k = 0; k < mks->keys_count; k++) {
                            msgpack* key = &keys_ptr->begin[k];
                            if (key->type != MSGPACK_ARRAY) {
                            BinKeyEnd:
                                if (k == 0) {
                                    free(mks->keys);
                                    goto BinKeyNull;
                                }
                                mks->keys_count = (uint32_t)k;
                                break;
                            }

                            msgpack_array* key_ptr = SELECT_MSGPACK_PTR(msgpack_array, key);
                            size_t count = key_ptr->end - key_ptr->begin;
                            if (count < 1 || count > 3)
                                goto BinKeyEnd;

                            kft[k].frame = msgpack_read_float_t(&key_ptr->begin[0], 0);
                            if (count > 1) {
                                kft[k].value = msgpack_read_float_t(&key_ptr->begin[1], 0);
                                if (count > 2)
                                    kft[k].tangent = msgpack_read_float_t(&key_ptr->begin[2], 0);
                                else
                                    kft[k].tangent = 0;
                            }
                            else {
                                kft[k].value = 0;
                                kft[k].tangent = 0;
                            }
                        }
                    }
                    continue;

                BinKeyNull:
                    mks->type = MOT_KEY_SET_NONE;
                    mks->keys_count = 0;
                    mks->keys = 0;
                }
            }

            msgpack* bone_info = msgpack_read(_m, L"BoneInfo");
            if (bone_info && bone_info->type == MSGPACK_ARRAY) {
                msgpack_array* bone_info_ptr = SELECT_MSGPACK_PTR(msgpack_array, bone_info);
                m.bone_info_count = (int32_t)(bone_info_ptr->end - bone_info_ptr->begin);
                if (m.bone_info_count > 0) {
                    m.bone_info = force_malloc_s(mot_bone_info, m.bone_info_count);
                    for (size_t j = 0; j < m.bone_info_count; j++)
                        m.bone_info[j].index = msgpack_read_int32_t(&bone_info_ptr->begin[j], 0);
                }
                else {
                    m.bone_info = 0;
                    m.bone_info_count = 0;
                }
            }
            else {
                m.bone_info = 0;
                m.bone_info_count = 0;
            }
            vector_mot_push_back(&ms->vec, &m);
        }
        ms->ready = true;
        ms->modern = false;
        ms->is_x = false;
    }

    _mot = msgpack_read(&msg, L"MOTModern");
    if (_mot && _mot->type == MSGPACK_MAP) {
        mot m;
        memset(&m, 0, sizeof(mot));
        msgpack_read_char_buffer_string(_mot, L"Name", &m.name);
        m.frame_count = msgpack_read_uint16_t(_mot, L"FrameCount");
        m.high_bits = msgpack_read_uint16_t(_mot, L"HighBits");
        msgpack* div = msgpack_read(_mot, L"Div");
        if (div) {
            m.div_frames = msgpack_read_uint16_t(div, L"Frames");
            m.div_count = msgpack_read_uint8_t(div, L"Count");
        }
        else {
            m.div_frames = 0;
            m.div_count = 0;
        }
        m.murmurhash = msgpack_read_uint32_t(_mot, L"MurmurHash");

        msgpack* _is_x = msgpack_read(_mot, L"X");
        bool is_x = false;
        if (_is_x && _is_x->type == MSGPACK_BOOL)
            is_x = msgpack_read(_is_x, 0);

        msgpack* key_sets = msgpack_read(_mot, L"KeySets");
        if (key_sets->type == MSGPACK_ARRAY) {
            msgpack_array* key_sets_ptr = SELECT_MSGPACK_PTR(msgpack_array, key_sets);
            m.key_set_count = (uint16_t)(key_sets_ptr->end - key_sets_ptr->begin);
            m.key_set = force_malloc_s(mot_key_set, m.key_set_count);
            for (size_t j = 0; j < m.key_set_count; j++) {
                mot_key_set* mks = &m.key_set[j];
                msgpack* key_set = &key_sets_ptr->begin[j];
                if (key_set->type != MSGPACK_ARRAY)
                    goto MotKeyNull;

                msgpack_array* key_set_ptr = SELECT_MSGPACK_PTR(msgpack_array, key_set);
                if (key_set_ptr->end - key_set_ptr->begin != 3)
                    continue;

                mks->type = msgpack_read_int32_t(&key_set_ptr->begin[0], 0);
                mks->data_type = msgpack_read_int32_t(&key_set_ptr->begin[1], 0);
                msgpack* keys = &key_set_ptr->begin[2];
                if (mks->type < MOT_KEY_SET_STATIC
                    || mks->type > MOT_KEY_SET_HERMITE_TANGENT
                    || keys->type != MSGPACK_ARRAY)
                    goto MotKeyNull;

                msgpack_array* keys_ptr = SELECT_MSGPACK_PTR(msgpack_array, keys);
                mks->keys_count = (uint32_t)(keys_ptr->end - keys_ptr->begin);
                if (mks->keys_count < 1)
                    goto MotKeyNull;
                else if (mks->type == MOT_KEY_SET_STATIC) {
                    mks->keys = force_malloc_s(kft2, 1);
                    mks->keys[0].frame = 0;
                    mks->keys[0].value = msgpack_read_float_t(&keys_ptr->begin[0], 0);
                    mks->keys[0].tangent = 0;
                }
                else {
                    mks->keys = force_malloc_s(kft2, mks->keys_count);
                    kft2* kft = mks->keys;
                    for (size_t k = 0; k < mks->keys_count; k++) {
                        msgpack* key = &keys_ptr->begin[k];
                        if (key->type != MSGPACK_ARRAY) {
                        MotKeyEnd:
                            if (k == 0) {
                                free(mks->keys);
                                goto MotKeyNull;
                            }
                            mks->keys_count = (uint32_t)k;
                            break;
                        }

                        msgpack_array* key_ptr = SELECT_MSGPACK_PTR(msgpack_array, key);
                        size_t count = key_ptr->end - key_ptr->begin;
                        if (count < 1 || count > 3)
                            goto MotKeyEnd;

                        kft[k].frame = msgpack_read_float_t(&key_ptr->begin[0], 0);
                        if (count > 1) {
                            kft[k].value = msgpack_read_float_t(&key_ptr->begin[1], 0);
                            if (count > 2)
                                kft[k].tangent = msgpack_read_float_t(&key_ptr->begin[2], 0);
                            else
                                kft[k].tangent = 0;
                        }
                        else {
                            kft[k].value = 0;
                            kft[k].tangent = 0;
                        }
                    }
                }
                continue;

            MotKeyNull:
                mks->type = MOT_KEY_SET_NONE;
                mks->keys_count = 0;
                mks->keys = 0;
            }
        }

        msgpack* bone_info = msgpack_read(_mot, L"BoneInfo");
        if (bone_info && bone_info->type == MSGPACK_ARRAY) {
            msgpack_array* bone_info_ptr = SELECT_MSGPACK_PTR(msgpack_array, bone_info);
            m.bone_info_count = (int32_t)(bone_info_ptr->end - bone_info_ptr->begin);
            if (m.bone_info_count > 0) {
                m.bone_info = force_malloc_s(mot_bone_info, m.bone_info_count);
                for (size_t j = 0; j < m.bone_info_count; j++)
                    msgpack_read_char_buffer_string(&bone_info_ptr->begin[j], 0, &m.bone_info[j].name);
            }
            else {
                m.bone_info = 0;
                m.bone_info_count = 0;
            }
        }
        else {
            m.bone_info = 0;
            m.bone_info_count = 0;
        }
        vector_mot_push_back(&ms->vec, &m);
        ms->ready = true;
        ms->modern = true;
        ms->is_x = is_x;
    }
    msgpack_free(&msg);
    io_dispose(s);
    free(path_mp);
}

void mot_write_mp(mot_set* ms, char* path, bool json) {
    if (!ms || !path || !ms->ready)
        return;

    wchar_t* path_buf = char_string_to_wchar_t_string(path);
    mot_wwrite_mp(ms, path_buf, json);
    free(path_buf);
}

void mot_wwrite_mp(mot_set* ms, wchar_t* path, bool json) {
    if (!ms || !path || !ms->ready)
        return;

    wchar_t* path_mp = str_utils_wadd(path, json ? L".json" : L".mp");
    stream* s = io_wopen(path_mp, L"wb");
    if (s->io.stream) {
        size_t length = ms->vec.end - ms->vec.begin;
        msgpack msg;
        msgpack_init_map(&msg, 0);
        if (!ms->modern) {
            msgpack _mot;
            msgpack_init_array(&_mot, L"MOT", length);
            msgpack_array* _mot_ptr = SELECT_MSGPACK(msgpack_array, _mot);
            for (size_t i = 0; i < length; i++) {
                mot* m = &ms->vec.begin[i];
                msgpack* _m = &_mot_ptr->begin[i];
                msgpack_set_map_empty(_m, 0);
                msgpack_append_uint16_t(_m, L"FrameCount", m->frame_count);
                msgpack_append_uint16_t(_m, L"HighBits", m->high_bits);

                msgpack key_sets;
                msgpack_init_array(&key_sets, L"KeySets", m->key_set_count);
                msgpack_array* key_sets_ptr = SELECT_MSGPACK(msgpack_array, key_sets);
                for (size_t j = 0; j < m->key_set_count; j++) {
                    mot_key_set* mks = &m->key_set[j];
                    if (mks->type == MOT_KEY_SET_NONE)
                        msgpack_set_null(&key_sets_ptr->begin[j], 0);
                    else if (mks->type == MOT_KEY_SET_STATIC) {
                        msgpack key_set;
                        msgpack_init_array(&key_set, 0, 2);
                        msgpack_array* key_set_ptr = SELECT_MSGPACK(msgpack_array, key_set);
                        msgpack_set_int32_t(&key_set_ptr->begin[0], 0, mks->type);

                        msgpack key;
                        msgpack_init_array(&key, 0, 1);
                        msgpack_array* key_ptr = SELECT_MSGPACK(msgpack_array, key);
                        msgpack_set_float_t(&key_ptr->begin[0], 0, mks->keys[0].value);
                        msgpack_set_array(&key_set_ptr->begin[1], 0, key_ptr);
                        msgpack_free(&key);

                        msgpack_set_array(&key_sets_ptr->begin[j], 0, key_set_ptr);
                        msgpack_free(&key_set);
                    }
                    else if (mks->type == MOT_KEY_SET_HERMITE || mks->type == MOT_KEY_SET_HERMITE_TANGENT) {
                        msgpack key_set;
                        msgpack_init_array(&key_set, 0, 2);
                        msgpack_array* key_set_ptr = SELECT_MSGPACK(msgpack_array, key_set);
                        msgpack_set_int32_t(&key_set_ptr->begin[0], 0, mks->type);

                        kft2* kft = mks->keys;
                        kf_type kf_check = mks->type == MOT_KEY_SET_HERMITE_TANGENT
                            ? KEY_FRAME_TYPE_2 : KEY_FRAME_TYPE_1;
                        msgpack keys;
                        msgpack_init_array(&keys, 0, mks->keys_count);
                        msgpack_array* keys_ptr = SELECT_MSGPACK(msgpack_array, keys);
                        for (size_t k = 0; k < mks->keys_count; k++) {
                            kft2 kf;
                            kf_type kt;
                            kft_check(&kft[k], kf_check, &kf, &kt);
                            switch (kt) {
                            case KEY_FRAME_TYPE_0: {
                                msgpack key;
                                msgpack_init_array(&key, 0, 1);
                                msgpack_array* key_ptr = SELECT_MSGPACK(msgpack_array, key);
                                msgpack_set_float_t(&key_ptr->begin[0], 0, kf.frame);
                                msgpack_set_array(&keys_ptr->begin[k], 0, key_ptr);
                                msgpack_free(&key);
                            } break;
                            case KEY_FRAME_TYPE_1: {
                                msgpack key;
                                msgpack_init_array(&key, 0, 2);
                                msgpack_array* key_ptr = SELECT_MSGPACK(msgpack_array, key);
                                msgpack_set_float_t(&key_ptr->begin[0], 0, kf.frame);
                                msgpack_set_float_t(&key_ptr->begin[1], 0, kf.value);
                                msgpack_set_array(&keys_ptr->begin[k], 0, key_ptr);
                                msgpack_free(&key);
                            } break;
                            case KEY_FRAME_TYPE_2: {
                                msgpack key;
                                msgpack_init_array(&key, 0, 3);
                                msgpack_array* key_ptr = SELECT_MSGPACK(msgpack_array, key);
                                msgpack_set_float_t(&key_ptr->begin[0], 0, kf.frame);
                                msgpack_set_float_t(&key_ptr->begin[1], 0, kf.value);
                                msgpack_set_float_t(&key_ptr->begin[2], 0, kf.tangent);
                                msgpack_set_array(&keys_ptr->begin[k], 0, key_ptr);
                                msgpack_free(&key);
                            } break;
                            }
                        }
                        msgpack_set_array(&key_set_ptr->begin[1], 0, keys_ptr);
                        msgpack_free(&keys);

                        msgpack_set_array(&key_sets_ptr->begin[j], 0, key_set_ptr);
                        msgpack_free(&key_set);
                    }
                }
                msgpack_append(_m, &key_sets);
                msgpack_free(&key_sets);

                mot_bone_info* mbi = m->bone_info;
                msgpack bone_info;
                msgpack_init_array(&bone_info, L"BoneInfo", m->bone_info_count);
                msgpack_array* bone_info_ptr = SELECT_MSGPACK(msgpack_array, bone_info);
                for (size_t j = 0; j < m->bone_info_count; j++)
                    msgpack_set_int32_t(&bone_info_ptr->begin[j], 0, mbi[j].index);
                msgpack_append(_m, &bone_info);
                msgpack_free(&bone_info);
            }
            msgpack_append(&msg, &_mot);
            msgpack_free(&_mot);
        }
        else {
            mot* m = &ms->vec.begin[0];
            msgpack _mot;
            msgpack_init_map(&_mot, L"MOTModern");
            msgpack_append_char_string(&_mot, L"Name", string_access(&m->name));
            msgpack_append_uint16_t(&_mot, L"FrameCount", m->frame_count);
            msgpack_append_uint16_t(&_mot, L"HighBits", m->high_bits);
            if (m->div_frames != 0) {
                msgpack div;
                msgpack_init_map(&div, L"Div");
                msgpack_append_uint16_t(&div, L"Frames", m->div_frames);
                msgpack_append_uint8_t(&div, L"Count", m->div_count);
                msgpack_append(&_mot, &div);
                msgpack_free(&div);

            }
            msgpack_append_uint32_t(&_mot, L"MurmurHash", m->murmurhash);

            if (ms->is_x)
                msgpack_append_bool(&_mot, L"X", ms->is_x);

            msgpack key_sets;
            msgpack_init_array(&key_sets, L"KeySets", m->key_set_count);
            msgpack_array* key_sets_ptr = SELECT_MSGPACK(msgpack_array, key_sets);
            for (size_t j = 0; j < m->key_set_count; j++) {
                mot_key_set* mks = &m->key_set[j];
                if (mks->type == MOT_KEY_SET_NONE)
                    msgpack_set_null(&key_sets_ptr->begin[j], 0);
                else if (mks->type == MOT_KEY_SET_STATIC) {
                    msgpack key_set;
                    msgpack_init_array(&key_set, 0, 3);
                    msgpack_array* key_set_ptr = SELECT_MSGPACK(msgpack_array, key_set);
                    msgpack_set_int32_t(&key_set_ptr->begin[0], 0, mks->type);
                    msgpack_set_int32_t(&key_set_ptr->begin[1], 0, 0);

                    msgpack key;
                    msgpack_init_array(&key, 0, 1);
                    msgpack_array* key_ptr = SELECT_MSGPACK(msgpack_array, key);
                    msgpack_set_float_t(&key_ptr->begin[0], 0, mks->keys[0].value);
                    msgpack_set_array(&key_set_ptr->begin[2], 0, key_ptr);
                    msgpack_free(&key);

                    msgpack_set_array(&key_sets_ptr->begin[j], 0, key_set_ptr);
                    msgpack_free(&key_set);
                }
                else if (mks->type == MOT_KEY_SET_HERMITE || mks->type == MOT_KEY_SET_HERMITE_TANGENT) {
                    msgpack key_set;
                    msgpack_init_array(&key_set, 0, 3);
                    msgpack_array* key_set_ptr = SELECT_MSGPACK(msgpack_array, key_set);
                    msgpack_set_int32_t(&key_set_ptr->begin[0], 0, mks->type);
                    msgpack_set_int32_t(&key_set_ptr->begin[1], 0, mks->data_type);

                    kft2* kft = mks->keys;
                    kf_type kf_check = mks->type == MOT_KEY_SET_HERMITE_TANGENT
                        ? KEY_FRAME_TYPE_2 : KEY_FRAME_TYPE_1;
                    msgpack keys;
                    msgpack_init_array(&keys, 0, mks->keys_count);
                    msgpack_array* keys_ptr = SELECT_MSGPACK(msgpack_array, keys);
                    for (size_t k = 0; k < mks->keys_count; k++) {
                        kft2 kf;
                        kf_type kt;
                        kft_check(&kft[k], kf_check, &kf, &kt);
                        switch (kt) {
                        case KEY_FRAME_TYPE_0: {
                            msgpack key;
                            msgpack_init_array(&key, 0, 1);
                            msgpack_array* key_ptr = SELECT_MSGPACK(msgpack_array, key);
                            msgpack_set_float_t(&key_ptr->begin[0], 0, kf.frame);
                            msgpack_set_array(&keys_ptr->begin[k], 0, key_ptr);
                            msgpack_free(&key);
                        } break;
                        case KEY_FRAME_TYPE_1: {
                            msgpack key;
                            msgpack_init_array(&key, 0, 2);
                            msgpack_array* key_ptr = SELECT_MSGPACK(msgpack_array, key);
                            msgpack_set_float_t(&key_ptr->begin[0], 0, kf.frame);
                            msgpack_set_float_t(&key_ptr->begin[1], 0, kf.value);
                            msgpack_set_array(&keys_ptr->begin[k], 0, key_ptr);
                            msgpack_free(&key);
                        } break;
                        case KEY_FRAME_TYPE_2: {
                            msgpack key;
                            msgpack_init_array(&key, 0, 3);
                            msgpack_array* key_ptr = SELECT_MSGPACK(msgpack_array, key);
                            msgpack_set_float_t(&key_ptr->begin[0], 0, kf.frame);
                            msgpack_set_float_t(&key_ptr->begin[1], 0, kf.value);
                            msgpack_set_float_t(&key_ptr->begin[2], 0, kf.tangent);
                            msgpack_set_array(&keys_ptr->begin[k], 0, key_ptr);
                            msgpack_free(&key);
                        } break;
                        }
                    }
                    msgpack_set_array(&key_set_ptr->begin[2], 0, keys_ptr);
                    msgpack_free(&keys);

                    msgpack_set_array(&key_sets_ptr->begin[j], 0, key_set_ptr);
                    msgpack_free(&key_set);
                }
            }
            msgpack_append(&_mot, &key_sets);
            msgpack_free(&key_sets);

            mot_bone_info* mbi = m->bone_info;
            msgpack bone_info;
            msgpack_init_array(&bone_info, L"BoneInfo", m->bone_info_count);
            msgpack_array* bone_info_ptr = SELECT_MSGPACK(msgpack_array, bone_info);
            for (size_t j = 0; j < m->bone_info_count; j++)
                msgpack_set_char_string(&bone_info_ptr->begin[j], 0, string_access(&mbi[j].name));
            msgpack_append(&_mot, &bone_info);
            msgpack_free(&bone_info);
            msgpack_append(&msg, &_mot);
            msgpack_free(&_mot);
        }
        if (json)
            io_json_write(s, &msg);
        else
            io_msgpack_write(s, &msg);
        msgpack_free(&msg);
    }
    io_dispose(s);
    free(path_mp);
}

void mot_dispose(mot_set* ms) {
    if (!ms)
        return;

    for (mot* i = ms->vec.begin; i != ms->vec.end; i++) {
        if (ms->modern)
            if (i->bone_info)
                for (size_t j = 0; j < i->bone_info_count; j++)
                    string_dispose(&i->bone_info[j].name);
        free(i->bone_info);

        if (i->key_set)
            for (size_t j = 0; j < i->key_set_count; j++)
                free(i->key_set[j].keys);
        free(i->key_set);
        string_dispose(&i->name);
    }
    vector_mot_free(&ms->vec);
    free(ms);
}
