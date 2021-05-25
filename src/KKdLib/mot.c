/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "mot.h"
#include "f2_struct.h"
#include "io_stream.h"
#include "msgpack.h"
#include "io_json.h"
#include "io_msgpack.h"
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

mot* mot_init() {
    mot* m = force_malloc(sizeof(mot));
    return m;
}

void mot_read_mot(mot* m, char* path) {
    if (!m || !path)
        return;

    wchar_t* path_buf = char_string_to_wchar_t_string(path);
    mot_wread_mot(m, path_buf);
    free(path_buf);
}

void mot_wread_mot(mot* m, wchar_t* path) {
    if (!m || !path)
        return;

    memset(m, 0, sizeof(mot));
    wchar_t* path_mot = str_utils_wadd(path, L".mot");
    stream* s = io_wopen(path_mot, L"rb");
    if (s->io.stream) {
        size_t count = io_read_uint64_t(s);
        /*size_t data_length =*/ io_read_uint64_t(s);
        /*size_t data_offset =*/ io_read_uint64_t(s);
        m->length = count;
        m->data = force_malloc_s(mot_struct, count);
        io_read(s, m->data, sizeof(mot_struct) * count);
        m->ready = true;
    }
    io_dispose(s);
    free(path_mot);
}

void mot_write_mot(mot* m, char* path) {
    if (!m || !path || !m->ready)
        return;

    wchar_t* path_buf = char_string_to_wchar_t_string(path);
    mot_wwrite_mot(m, path_buf);
    free(path_buf);
}

void mot_wwrite_mot(mot* m, wchar_t* path) {
    if (!m || !path || !m->ready)
        return;

    wchar_t* path_mot = str_utils_wadd(path, L".mot");
    stream* s = io_wopen(path_mot, L"wb");
    if (s->io.stream) {
        size_t count = m->length;
        io_write_uint64_t(s, count);
        io_write_uint64_t(s, count * 0x20);
        io_write_uint64_t(s, 0x18);
        io_write(s, m->data, sizeof(mot_struct) * count);
    }
    io_dispose(s);
    free(path_mot);
}

void mot_read_mp(mot* m, char* path, bool json) {
    if (!m || !path)
        return;

    wchar_t* path_buf = char_string_to_wchar_t_string(path);
    mot_wread_mp(m, path_buf, json);
    free(path_buf);
}

void mot_wread_mp(mot* m, wchar_t* path, bool json) {
    if (!m || !path)
        return;

    memset(m, 0, sizeof(mot));
    wchar_t* path_mp = str_utils_wadd(path, json ? L".json" : L".mp");
    stream* s = io_wopen(path_mp, L"rb");
    msgpack* msg;
    if (json)
        msg = io_json_read(s);
    else
        msg = io_msgpack_read(s);

    msgpack* mot = msgpack_read(msg, L"AddParam");
    if (mot && mot->type == MSGPACK_ARRAY) {
        msgpack_array* ptr = SELECT_MSGPACK(msgpack_array, mot);
        m->length = ptr->length;
        m->data = force_malloc_s(mot_struct, m->length);
        for (size_t i = 0; i < m->length; i++) {
            if (ptr->data[i].type != MSGPACK_MAP)
                continue;

            msgpack* _m = &ptr->data[i];
            mot_struct* mots = &m->data[i];
            mots->time = msgpack_read_float_t(_m, L"Time");
            mots->flags = msgpack_read_int32_t(_m, L"Flags");
            mots->frame = msgpack_read_int32_t(_m, L"Frame");
            mots->pv_branch = msgpack_read_int32_t(_m, L"PVBranch");
            mots->id = msgpack_read_int32_t(_m, L"ID");
            mots->value = msgpack_read_int32_t(_m, L"Value");
        }
        m->ready = true;
    }
    msgpack_dispose(msg);
    io_dispose(s);
    free(path_mp);
}

void mot_write_mp(mot* m, char* path, bool json) {
    if (!m || !path || !m->ready)
        return;

    wchar_t* path_buf = char_string_to_wchar_t_string(path);
    mot_wwrite_mp(m, path_buf, json);
    free(path_buf);
}

void mot_wwrite_mp(mot* m, wchar_t* path, bool json) {
    if (!m || !path || !m->ready)
        return;

    wchar_t* path_mp = str_utils_wadd(path, json ? L".json" : L".mp");
    stream* s = io_wopen(path_mp, L"wb");
    if (s->io.stream) {
        msgpack* msg = msgpack_init_map(0);
        msgpack* mot = msgpack_init_array(L"AddParam", m->length);
        msgpack_array* ptr = SELECT_MSGPACK(msgpack_array, mot);
        for (size_t i = 0; i < m->length; i++) {
            msgpack* _m = &ptr->data[i];
            mot_struct* mots = &m->data[i];
            msgpack_set_map_empty(_m, 0);
            msgpack_append_float_t(_m, L"Time", mots->time);
            msgpack_append_int32_t(_m, L"Flags", mots->flags);
            msgpack_append_int32_t(_m, L"Frame", mots->frame);
            if (mots->pv_branch)
                msgpack_append_int32_t(_m, L"PVBranch", mots->pv_branch);
            msgpack_append_int32_t(_m, L"ID", mots->id);
            msgpack_append_int32_t(_m, L"Value", mots->value);
        }
        msgpack_append(msg, mot);
        msgpack_dispose(mot);
        if (json)
            io_json_write(s, msg);
        else
            io_msgpack_write(s, msg);
        msgpack_dispose(msg);
    }
    io_dispose(s);
    free(path_mp);
}

void mot_dispose(mot* m) {
    if (!m)
        return;

    free(m->data);
    free(m);
}
