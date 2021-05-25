/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "default.h"

typedef enum mot_bone_index_ac {
    MOT_BONE_AC_INVALID = -1,
    MOT_BONE_AC_N_HARA_CP = 0,
    MOT_BONE_AC_KG_HARA_Y,
    MOT_BONE_AC_KL_HARA_XZ,
    MOT_BONE_AC_KL_HARA_ETC,
    MOT_BONE_AC_N_HARA,
    MOT_BONE_AC_CL_MUNE,
    MOT_BONE_AC_J_MUNE_WJ,
    MOT_BONE_AC_E_MUNE_CP,
    MOT_BONE_AC_N_MUNE_B,
    MOT_BONE_AC_KL_MUNE_B_WJ,
    MOT_BONE_AC_KL_KUBI,
    MOT_BONE_AC_N_KAO,
    MOT_BONE_AC_CL_KAO,
    MOT_BONE_AC_J_KAO_WJ,
    MOT_BONE_AC_E_KAO_CP,
    MOT_BONE_AC_FACE_ROOT,
    MOT_BONE_AC_KL_EYE_L_WJ,
    MOT_BONE_AC_KL_EYE_R_WJ,
    MOT_BONE_AC_KL_MABU_D_L_WJ,
    MOT_BONE_AC_KL_MABU_D_R_WJ,
    MOT_BONE_AC_KL_MABU_L_WJ,
    MOT_BONE_AC_KL_MABU_R_WJ,
    MOT_BONE_AC_KL_MABU_U_L_WJ,
    MOT_BONE_AC_KL_MABU_U_R_WJ,
    MOT_BONE_AC_N_AGO,
    MOT_BONE_AC_TL_AGO,
    MOT_BONE_AC_KL_AGO_WJ,
    MOT_BONE_AC_N_AGO_B,
    MOT_BONE_AC_TL_AGO_WJ,
    MOT_BONE_AC_TL_HA_WJ,
    MOT_BONE_AC_N_HOHO_B_L,
    MOT_BONE_AC_TL_HOHO_B_L_WJ,
    MOT_BONE_AC_N_HOHO_B_R,
    MOT_BONE_AC_TL_HOHO_B_R_WJ,
    MOT_BONE_AC_N_HOHO_C_L,
    MOT_BONE_AC_TL_HOHO_C_L_WJ,
    MOT_BONE_AC_N_HOHO_C_R,
    MOT_BONE_AC_TL_HOHO_C_R_WJ,
    MOT_BONE_AC_N_HOHO_L,
    MOT_BONE_AC_TL_HOHO_L_WJ,
    MOT_BONE_AC_N_HOHO_R,
    MOT_BONE_AC_TL_HOHO_R_WJ,
    MOT_BONE_AC_N_KUTI_D,
    MOT_BONE_AC_TL_KUTI_D_WJ,
    MOT_BONE_AC_N_KUTI_D_L,
    MOT_BONE_AC_TL_KUTI_D_L_WJ,
    MOT_BONE_AC_N_KUTI_D_R,
    MOT_BONE_AC_TL_KUTI_D_R_WJ,
    MOT_BONE_AC_N_KUTI_L,
    MOT_BONE_AC_TL_KUTI_L_WJ,
    MOT_BONE_AC_N_KUTI_R,
    MOT_BONE_AC_TL_KUTI_R_WJ,
    MOT_BONE_AC_N_KUTI_U,
    MOT_BONE_AC_TL_KUTI_U_WJ,
    MOT_BONE_AC_N_KUTI_U_L,
    MOT_BONE_AC_TL_KUTI_U_L_WJ,
    MOT_BONE_AC_N_KUTI_U_R,
    MOT_BONE_AC_TL_KUTI_U_R_WJ,
    MOT_BONE_AC_N_MABU_L_D,
    MOT_BONE_AC_TL_MABU_L_D_WJ,
    MOT_BONE_AC_N_MABU_L_D_L,
    MOT_BONE_AC_TL_MABU_L_D_L_WJ,
    MOT_BONE_AC_N_MABU_L_D_R,
    MOT_BONE_AC_TL_MABU_L_D_R_WJ,
    MOT_BONE_AC_N_MABU_L_U,
    MOT_BONE_AC_TL_MABU_L_U_WJ,
    MOT_BONE_AC_N_MABU_L_U_L,
    MOT_BONE_AC_TL_MABU_L_U_L_WJ,
    MOT_BONE_AC_N_MABU_L_U_R,
    MOT_BONE_AC_TL_MABU_L_U_R_WJ,
    MOT_BONE_AC_N_MABU_R_D,
    MOT_BONE_AC_TL_MABU_R_D_WJ,
    MOT_BONE_AC_N_MABU_R_D_L,
    MOT_BONE_AC_TL_MABU_R_D_L_WJ,
    MOT_BONE_AC_N_MABU_R_D_R,
    MOT_BONE_AC_TL_MABU_R_D_R_WJ,
    MOT_BONE_AC_N_MABU_R_U,
    MOT_BONE_AC_TL_MABU_R_U_WJ,
    MOT_BONE_AC_N_MABU_R_U_L,
    MOT_BONE_AC_TL_MABU_R_U_L_WJ,
    MOT_BONE_AC_N_MABU_R_U_R,
    MOT_BONE_AC_TL_MABU_R_U_R_WJ,
    MOT_BONE_AC_N_MAYU_B_L,
    MOT_BONE_AC_TL_MAYU_B_L_WJ,
    MOT_BONE_AC_N_MAYU_B_R,
    MOT_BONE_AC_TL_MAYU_B_R_WJ,
    MOT_BONE_AC_N_MAYU_C_L,
    MOT_BONE_AC_TL_MAYU_C_L_WJ,
    MOT_BONE_AC_N_MAYU_C_R,
    MOT_BONE_AC_TL_MAYU_C_R_WJ,
    MOT_BONE_AC_N_MAYU_L,
    MOT_BONE_AC_TL_MAYU_L_WJ,
    MOT_BONE_AC_N_MAYU_R,
    MOT_BONE_AC_TL_MAYU_R_WJ,
    MOT_BONE_AC_N_KUBI_WJ_EX,
    MOT_BONE_AC_N_WAKI_L,
    MOT_BONE_AC_KL_WAKI_L_WJ,
    MOT_BONE_AC_TL_UP_KATA_L,
    MOT_BONE_AC_C_KATA_L,
    MOT_BONE_AC_J_KATA_L_WJ_CU,
    MOT_BONE_AC_J_UDE_L_WJ,
    MOT_BONE_AC_E_UDE_L_CP,
    MOT_BONE_AC_KL_TE_L_WJ,
    MOT_BONE_AC_NL_HITO_L_WJ,
    MOT_BONE_AC_NL_HITO_B_L_WJ,
    MOT_BONE_AC_NL_HITO_C_L_WJ,
    MOT_BONE_AC_NL_KO_L_WJ,
    MOT_BONE_AC_NL_KO_B_L_WJ,
    MOT_BONE_AC_NL_KO_C_L_WJ,
    MOT_BONE_AC_NL_KUSU_L_WJ,
    MOT_BONE_AC_NL_KUSU_B_L_WJ,
    MOT_BONE_AC_NL_KUSU_C_L_WJ,
    MOT_BONE_AC_NL_NAKA_L_WJ,
    MOT_BONE_AC_NL_NAKA_B_L_WJ,
    MOT_BONE_AC_NL_NAKA_C_L_WJ,
    MOT_BONE_AC_NL_OYA_L_WJ,
    MOT_BONE_AC_NL_OYA_B_L_WJ,
    MOT_BONE_AC_NL_OYA_C_L_WJ,
    MOT_BONE_AC_N_STE_L_WJ_EX,
    MOT_BONE_AC_N_SUDE_L_WJ_EX,
    MOT_BONE_AC_N_SUDE_B_L_WJ_EX,
    MOT_BONE_AC_N_HIJI_L_WJ_EX,
    MOT_BONE_AC_N_SKATA_L_WJ_CD_EX,
    MOT_BONE_AC_N_SKATA_B_L_WJ_CD_CU_EX,
    MOT_BONE_AC_N_SKATA_C_L_WJ_CD_CU_EX,
    MOT_BONE_AC_N_WAKI_R,
    MOT_BONE_AC_KL_WAKI_R_WJ,
    MOT_BONE_AC_TL_UP_KATA_R,
    MOT_BONE_AC_C_KATA_R,
    MOT_BONE_AC_J_KATA_R_WJ_CU,
    MOT_BONE_AC_J_UDE_R_WJ,
    MOT_BONE_AC_E_UDE_R_CP,
    MOT_BONE_AC_KL_TE_R_WJ,
    MOT_BONE_AC_NL_HITO_R_WJ,
    MOT_BONE_AC_NL_HITO_B_R_WJ,
    MOT_BONE_AC_NL_HITO_C_R_WJ,
    MOT_BONE_AC_NL_KO_R_WJ,
    MOT_BONE_AC_NL_KO_B_R_WJ,
    MOT_BONE_AC_NL_KO_C_R_WJ,
    MOT_BONE_AC_NL_KUSU_R_WJ,
    MOT_BONE_AC_NL_KUSU_B_R_WJ,
    MOT_BONE_AC_NL_KUSU_C_R_WJ,
    MOT_BONE_AC_NL_NAKA_R_WJ,
    MOT_BONE_AC_NL_NAKA_B_R_WJ,
    MOT_BONE_AC_NL_NAKA_C_R_WJ,
    MOT_BONE_AC_NL_OYA_R_WJ,
    MOT_BONE_AC_NL_OYA_B_R_WJ,
    MOT_BONE_AC_NL_OYA_C_R_WJ,
    MOT_BONE_AC_N_STE_R_WJ_EX,
    MOT_BONE_AC_N_SUDE_R_WJ_EX,
    MOT_BONE_AC_N_SUDE_B_R_WJ_EX,
    MOT_BONE_AC_N_HIJI_R_WJ_EX,
    MOT_BONE_AC_N_SKATA_R_WJ_CD_EX,
    MOT_BONE_AC_N_SKATA_B_R_WJ_CD_CU_EX,
    MOT_BONE_AC_N_SKATA_C_R_WJ_CD_CU_EX,
    MOT_BONE_AC_KL_KOSI_Y,
    MOT_BONE_AC_KL_KOSI_XZ,
    MOT_BONE_AC_KL_KOSI_ETC_WJ,
    MOT_BONE_AC_CL_MOMO_L,
    MOT_BONE_AC_J_MOMO_L_WJ,
    MOT_BONE_AC_J_SUNE_L_WJ,
    MOT_BONE_AC_E_SUNE_L_CP,
    MOT_BONE_AC_KL_ASI_L_WJ_CO,
    MOT_BONE_AC_KL_TOE_L_WJ,
    MOT_BONE_AC_N_HIZA_L_WJ_EX,
    MOT_BONE_AC_CL_MOMO_R,
    MOT_BONE_AC_J_MOMO_R_WJ,
    MOT_BONE_AC_J_SUNE_R_WJ,
    MOT_BONE_AC_E_SUNE_R_CP,
    MOT_BONE_AC_KL_ASI_R_WJ_CO,
    MOT_BONE_AC_KL_TOE_R_WJ,
    MOT_BONE_AC_N_HIZA_R_WJ_EX,
    MOT_BONE_AC_N_MOMO_L_CD_EX,
    MOT_BONE_AC_N_MOMO_B_L_WJ_EX,
    MOT_BONE_AC_N_MOMO_C_L_WJ_EX,
    MOT_BONE_AC_N_MOMO_R_CD_EX,
    MOT_BONE_AC_N_MOMO_B_R_WJ_EX,
    MOT_BONE_AC_N_MOMO_C_R_WJ_EX,
    MOT_BONE_AC_N_HARA_CD_EX,
    MOT_BONE_AC_N_HARA_B_WJ_EX,
    MOT_BONE_AC_N_HARA_C_WJ_EX,
    MOT_BONE_AC_MAX = 181,
} mot_bone_index_ac;

typedef enum mot_bone_index_f {
    MOT_BONE_F_INVALID = -1,
    MOT_BONE_F_N_HARA_CP = 0,
    MOT_BONE_F_KG_HARA_Y,
    MOT_BONE_F_KL_HARA_XZ,
    MOT_BONE_F_KL_HARA_ETC,
    MOT_BONE_F_N_HARA,
    MOT_BONE_F_CL_MUNE,
    MOT_BONE_F_J_MUNE_WJ,
    MOT_BONE_F_E_MUNE_CP,
    MOT_BONE_F_N_MUNE_B,
    MOT_BONE_F_KL_MUNE_B_WJ,
    MOT_BONE_F_KL_KUBI,
    MOT_BONE_F_N_KAO,
    MOT_BONE_F_CL_KAO,
    MOT_BONE_F_J_KAO_WJ,
    MOT_BONE_F_E_KAO_CP,
    MOT_BONE_F_FACE_ROOT,
    MOT_BONE_F_N_AGO,
    MOT_BONE_F_KL_AGO_WJ,
    MOT_BONE_F_N_TOOTH_UNDER,
    MOT_BONE_F_TL_TOOTH_UNDER_WJ,
    MOT_BONE_F_N_EYE_L,
    MOT_BONE_F_KL_EYE_L,
    MOT_BONE_F_N_EYE_L_WJ_EX,
    MOT_BONE_F_KL_HIGHLIGHT_L_WJ,
    MOT_BONE_F_N_EYE_R,
    MOT_BONE_F_KL_EYE_R,
    MOT_BONE_F_N_EYE_R_WJ_EX,
    MOT_BONE_F_KL_HIGHLIGHT_R_WJ,
    MOT_BONE_F_N_EYELID_L_A,
    MOT_BONE_F_TL_EYELID_L_A_WJ,
    MOT_BONE_F_N_EYELID_L_B,
    MOT_BONE_F_TL_EYELID_L_B_WJ,
    MOT_BONE_F_N_EYELID_R_A,
    MOT_BONE_F_TL_EYELID_R_A_WJ,
    MOT_BONE_F_N_EYELID_R_B,
    MOT_BONE_F_TL_EYELID_R_B_WJ,
    MOT_BONE_F_N_KUTI_D,
    MOT_BONE_F_TL_KUTI_D_WJ,
    MOT_BONE_F_N_KUTI_D_L,
    MOT_BONE_F_TL_KUTI_D_L_WJ,
    MOT_BONE_F_N_KUTI_D_R,
    MOT_BONE_F_TL_KUTI_D_R_WJ,
    MOT_BONE_F_N_KUTI_L,
    MOT_BONE_F_TL_KUTI_L_WJ,
    MOT_BONE_F_N_KUTI_R,
    MOT_BONE_F_TL_KUTI_R_WJ,
    MOT_BONE_F_N_KUTI_U,
    MOT_BONE_F_TL_KUTI_U_WJ,
    MOT_BONE_F_N_KUTI_U_L,
    MOT_BONE_F_TL_KUTI_U_L_WJ,
    MOT_BONE_F_N_KUTI_U_R,
    MOT_BONE_F_TL_KUTI_U_R_WJ,
    MOT_BONE_F_N_MABU_L_D_A,
    MOT_BONE_F_TL_MABU_L_D_A_WJ,
    MOT_BONE_F_N_MABU_L_D_B,
    MOT_BONE_F_TL_MABU_L_D_B_WJ,
    MOT_BONE_F_N_MABU_L_D_C,
    MOT_BONE_F_TL_MABU_L_D_C_WJ,
    MOT_BONE_F_N_MABU_L_U_A,
    MOT_BONE_F_TL_MABU_L_U_A_WJ,
    MOT_BONE_F_N_MABU_L_U_B,
    MOT_BONE_F_TL_MABU_L_U_B_WJ,
    MOT_BONE_F_N_EYELASHES_L,
    MOT_BONE_F_TL_EYELASHES_L_WJ,
    MOT_BONE_F_N_MABU_L_U_C,
    MOT_BONE_F_TL_MABU_L_U_C_WJ,
    MOT_BONE_F_N_MABU_R_D_A,
    MOT_BONE_F_TL_MABU_R_D_A_WJ,
    MOT_BONE_F_N_MABU_R_D_B,
    MOT_BONE_F_TL_MABU_R_D_B_WJ,
    MOT_BONE_F_N_MABU_R_D_C,
    MOT_BONE_F_TL_MABU_R_D_C_WJ,
    MOT_BONE_F_N_MABU_R_U_A,
    MOT_BONE_F_TL_MABU_R_U_A_WJ,
    MOT_BONE_F_N_MABU_R_U_B,
    MOT_BONE_F_TL_MABU_R_U_B_WJ,
    MOT_BONE_F_N_EYELASHES_R,
    MOT_BONE_F_TL_EYELASHES_R_WJ,
    MOT_BONE_F_N_MABU_R_U_C,
    MOT_BONE_F_TL_MABU_R_U_C_WJ,
    MOT_BONE_F_N_MAYU_L,
    MOT_BONE_F_TL_MAYU_L_WJ,
    MOT_BONE_F_N_MAYU_L_B,
    MOT_BONE_F_TL_MAYU_L_B_WJ,
    MOT_BONE_F_N_MAYU_L_C,
    MOT_BONE_F_TL_MAYU_L_C_WJ,
    MOT_BONE_F_N_MAYU_R,
    MOT_BONE_F_TL_MAYU_R_WJ,
    MOT_BONE_F_N_MAYU_R_B,
    MOT_BONE_F_TL_MAYU_R_B_WJ,
    MOT_BONE_F_N_MAYU_R_C,
    MOT_BONE_F_TL_MAYU_R_C_WJ,
    MOT_BONE_F_N_TOOTH_UPPER,
    MOT_BONE_F_TL_TOOTH_UPPER_WJ,
    MOT_BONE_F_N_KUBI_WJ_EX,
    MOT_BONE_F_N_WAKI_L,
    MOT_BONE_F_KL_WAKI_L_WJ,
    MOT_BONE_F_TL_UP_KATA_L,
    MOT_BONE_F_C_KATA_L,
    MOT_BONE_F_J_KATA_L_WJ_CU,
    MOT_BONE_F_J_UDE_L_WJ,
    MOT_BONE_F_E_UDE_L_CP,
    MOT_BONE_F_KL_TE_L_WJ,
    MOT_BONE_F_N_HITO_L_EX,
    MOT_BONE_F_NL_HITO_L_WJ,
    MOT_BONE_F_NL_HITO_B_L_WJ,
    MOT_BONE_F_NL_HITO_C_L_WJ,
    MOT_BONE_F_N_KO_L_EX,
    MOT_BONE_F_NL_KO_L_WJ,
    MOT_BONE_F_NL_KO_B_L_WJ,
    MOT_BONE_F_NL_KO_C_L_WJ,
    MOT_BONE_F_N_KUSU_L_EX,
    MOT_BONE_F_NL_KUSU_L_WJ,
    MOT_BONE_F_NL_KUSU_B_L_WJ,
    MOT_BONE_F_NL_KUSU_C_L_WJ,
    MOT_BONE_F_N_NAKA_L_EX,
    MOT_BONE_F_NL_NAKA_L_WJ,
    MOT_BONE_F_NL_NAKA_B_L_WJ,
    MOT_BONE_F_NL_NAKA_C_L_WJ,
    MOT_BONE_F_N_OYA_L_EX,
    MOT_BONE_F_NL_OYA_L_WJ,
    MOT_BONE_F_NL_OYA_B_L_WJ,
    MOT_BONE_F_NL_OYA_C_L_WJ,
    MOT_BONE_F_N_STE_L_WJ_EX,
    MOT_BONE_F_N_SUDE_L_WJ_EX,
    MOT_BONE_F_N_SUDE_B_L_WJ_EX,
    MOT_BONE_F_N_HIJI_L_WJ_EX,
    MOT_BONE_F_N_UP_KATA_L_EX,
    MOT_BONE_F_N_SKATA_L_WJ_CD_EX,
    MOT_BONE_F_N_SKATA_B_L_WJ_CD_CU_EX,
    MOT_BONE_F_N_SKATA_C_L_WJ_CD_CU_EX,
    MOT_BONE_F_N_WAKI_R,
    MOT_BONE_F_KL_WAKI_R_WJ,
    MOT_BONE_F_TL_UP_KATA_R,
    MOT_BONE_F_C_KATA_R,
    MOT_BONE_F_J_KATA_R_WJ_CU,
    MOT_BONE_F_J_UDE_R_WJ,
    MOT_BONE_F_E_UDE_R_CP,
    MOT_BONE_F_KL_TE_R_WJ,
    MOT_BONE_F_N_HITO_R_EX,
    MOT_BONE_F_NL_HITO_R_WJ,
    MOT_BONE_F_NL_HITO_B_R_WJ,
    MOT_BONE_F_NL_HITO_C_R_WJ,
    MOT_BONE_F_N_KO_R_EX,
    MOT_BONE_F_NL_KO_R_WJ,
    MOT_BONE_F_NL_KO_B_R_WJ,
    MOT_BONE_F_NL_KO_C_R_WJ,
    MOT_BONE_F_N_KUSU_R_EX,
    MOT_BONE_F_NL_KUSU_R_WJ,
    MOT_BONE_F_NL_KUSU_B_R_WJ,
    MOT_BONE_F_NL_KUSU_C_R_WJ,
    MOT_BONE_F_N_NAKA_R_EX,
    MOT_BONE_F_NL_NAKA_R_WJ,
    MOT_BONE_F_NL_NAKA_B_R_WJ,
    MOT_BONE_F_NL_NAKA_C_R_WJ,
    MOT_BONE_F_N_OYA_R_EX,
    MOT_BONE_F_NL_OYA_R_WJ,
    MOT_BONE_F_NL_OYA_B_R_WJ,
    MOT_BONE_F_NL_OYA_C_R_WJ,
    MOT_BONE_F_N_STE_R_WJ_EX,
    MOT_BONE_F_N_SUDE_R_WJ_EX,
    MOT_BONE_F_N_SUDE_B_R_WJ_EX,
    MOT_BONE_F_N_HIJI_R_WJ_EX,
    MOT_BONE_F_N_UP_KATA_R_EX,
    MOT_BONE_F_N_SKATA_R_WJ_CD_EX,
    MOT_BONE_F_N_SKATA_B_R_WJ_CD_CU_EX,
    MOT_BONE_F_N_SKATA_C_R_WJ_CD_CU_EX,
    MOT_BONE_F_KL_KOSI_Y,
    MOT_BONE_F_KL_KOSI_XZ,
    MOT_BONE_F_KL_KOSI_ETC_WJ,
    MOT_BONE_F_CL_MOMO_L,
    MOT_BONE_F_J_MOMO_L_WJ,
    MOT_BONE_F_J_SUNE_L_WJ,
    MOT_BONE_F_E_SUNE_L_CP,
    MOT_BONE_F_KL_ASI_L_WJ_CO,
    MOT_BONE_F_KL_TOE_L_WJ,
    MOT_BONE_F_N_HIZA_L_WJ_EX,
    MOT_BONE_F_CL_MOMO_R,
    MOT_BONE_F_J_MOMO_R_WJ,
    MOT_BONE_F_J_SUNE_R_WJ,
    MOT_BONE_F_E_SUNE_R_CP,
    MOT_BONE_F_KL_ASI_R_WJ_CO,
    MOT_BONE_F_KL_TOE_R_WJ,
    MOT_BONE_F_N_HIZA_R_WJ_EX,
    MOT_BONE_F_N_MOMO_A_L_WJ_CD_EX,
    MOT_BONE_F_N_MOMO_B_L_WJ_EX,
    MOT_BONE_F_N_MOMO_C_L_WJ_EX,
    MOT_BONE_F_N_MOMO_A_R_WJ_CD_EX,
    MOT_BONE_F_N_MOMO_B_R_WJ_EX,
    MOT_BONE_F_N_MOMO_C_R_WJ_EX,
    MOT_BONE_F_N_HARA_CD_EX,
    MOT_BONE_F_N_HARA_B_WJ_EX,
    MOT_BONE_F_N_HARA_C_WJ_EX,
    MOT_BONE_F_MAX = 193,
} mot_bone_index_f;

typedef enum mot_bone_index_ft {
    MOT_BONE_FT_INVALID = -1,
    MOT_BONE_FT_N_HARA_CP = 0,
    MOT_BONE_FT_KG_HARA_Y,
    MOT_BONE_FT_KL_HARA_XZ,
    MOT_BONE_FT_KL_HARA_ETC,
    MOT_BONE_FT_N_HARA,
    MOT_BONE_FT_CL_MUNE,
    MOT_BONE_FT_J_MUNE_WJ,
    MOT_BONE_FT_E_MUNE_CP,
    MOT_BONE_FT_N_MUNE_B,
    MOT_BONE_FT_KL_MUNE_B_WJ,
    MOT_BONE_FT_KL_KUBI,
    MOT_BONE_FT_N_KAO,
    MOT_BONE_FT_CL_KAO,
    MOT_BONE_FT_J_KAO_WJ,
    MOT_BONE_FT_E_KAO_CP,
    MOT_BONE_FT_FACE_ROOT,
    MOT_BONE_FT_N_AGO,
    MOT_BONE_FT_KL_AGO_WJ,
    MOT_BONE_FT_N_TOOTH_UNDER,
    MOT_BONE_FT_TL_TOOTH_UNDER_WJ,
    MOT_BONE_FT_N_EYE_L,
    MOT_BONE_FT_KL_EYE_L,
    MOT_BONE_FT_N_EYE_L_WJ_EX,
    MOT_BONE_FT_KL_HIGHLIGHT_L_WJ,
    MOT_BONE_FT_N_EYE_R,
    MOT_BONE_FT_KL_EYE_R,
    MOT_BONE_FT_N_EYE_R_WJ_EX,
    MOT_BONE_FT_KL_HIGHLIGHT_R_WJ,
    MOT_BONE_FT_N_EYELID_L_A,
    MOT_BONE_FT_TL_EYELID_L_A_WJ,
    MOT_BONE_FT_N_EYELID_L_B,
    MOT_BONE_FT_TL_EYELID_L_B_WJ,
    MOT_BONE_FT_N_EYELID_R_A,
    MOT_BONE_FT_TL_EYELID_R_A_WJ,
    MOT_BONE_FT_N_EYELID_R_B,
    MOT_BONE_FT_TL_EYELID_R_B_WJ,
    MOT_BONE_FT_N_KUTI_D,
    MOT_BONE_FT_TL_KUTI_D_WJ,
    MOT_BONE_FT_N_KUTI_D_L,
    MOT_BONE_FT_TL_KUTI_D_L_WJ,
    MOT_BONE_FT_N_KUTI_D_R,
    MOT_BONE_FT_TL_KUTI_D_R_WJ,
    MOT_BONE_FT_N_KUTI_DS_L,
    MOT_BONE_FT_TL_KUTI_DS_L_WJ,
    MOT_BONE_FT_N_KUTI_DS_R,
    MOT_BONE_FT_TL_KUTI_DS_R_WJ,
    MOT_BONE_FT_N_KUTI_L,
    MOT_BONE_FT_TL_KUTI_L_WJ,
    MOT_BONE_FT_N_KUTI_M_L,
    MOT_BONE_FT_TL_KUTI_M_L_WJ,
    MOT_BONE_FT_N_KUTI_M_R,
    MOT_BONE_FT_TL_KUTI_M_R_WJ,
    MOT_BONE_FT_N_KUTI_R,
    MOT_BONE_FT_TL_KUTI_R_WJ,
    MOT_BONE_FT_N_KUTI_U,
    MOT_BONE_FT_TL_KUTI_U_WJ,
    MOT_BONE_FT_N_KUTI_U_L,
    MOT_BONE_FT_TL_KUTI_U_L_WJ,
    MOT_BONE_FT_N_KUTI_U_R,
    MOT_BONE_FT_TL_KUTI_U_R_WJ,
    MOT_BONE_FT_N_MABU_L_D_A,
    MOT_BONE_FT_TL_MABU_L_D_A_WJ,
    MOT_BONE_FT_N_MABU_L_D_B,
    MOT_BONE_FT_TL_MABU_L_D_B_WJ,
    MOT_BONE_FT_N_MABU_L_D_C,
    MOT_BONE_FT_TL_MABU_L_D_C_WJ,
    MOT_BONE_FT_N_MABU_L_U_A,
    MOT_BONE_FT_TL_MABU_L_U_A_WJ,
    MOT_BONE_FT_N_MABU_L_U_B,
    MOT_BONE_FT_TL_MABU_L_U_B_WJ,
    MOT_BONE_FT_N_EYELASHES_L,
    MOT_BONE_FT_TL_EYELASHES_L_WJ,
    MOT_BONE_FT_N_MABU_L_U_C,
    MOT_BONE_FT_TL_MABU_L_U_C_WJ,
    MOT_BONE_FT_N_MABU_R_D_A,
    MOT_BONE_FT_TL_MABU_R_D_A_WJ,
    MOT_BONE_FT_N_MABU_R_D_B,
    MOT_BONE_FT_TL_MABU_R_D_B_WJ,
    MOT_BONE_FT_N_MABU_R_D_C,
    MOT_BONE_FT_TL_MABU_R_D_C_WJ,
    MOT_BONE_FT_N_MABU_R_U_A,
    MOT_BONE_FT_TL_MABU_R_U_A_WJ,
    MOT_BONE_FT_N_MABU_R_U_B,
    MOT_BONE_FT_TL_MABU_R_U_B_WJ,
    MOT_BONE_FT_N_EYELASHES_R,
    MOT_BONE_FT_TL_EYELASHES_R_WJ,
    MOT_BONE_FT_N_MABU_R_U_C,
    MOT_BONE_FT_TL_MABU_R_U_C_WJ,
    MOT_BONE_FT_N_MAYU_L,
    MOT_BONE_FT_TL_MAYU_L_WJ,
    MOT_BONE_FT_N_MAYU_L_B,
    MOT_BONE_FT_TL_MAYU_L_B_WJ,
    MOT_BONE_FT_N_MAYU_L_C,
    MOT_BONE_FT_TL_MAYU_L_C_WJ,
    MOT_BONE_FT_N_MAYU_R,
    MOT_BONE_FT_TL_MAYU_R_WJ,
    MOT_BONE_FT_N_MAYU_R_B,
    MOT_BONE_FT_TL_MAYU_R_B_WJ,
    MOT_BONE_FT_N_MAYU_R_C,
    MOT_BONE_FT_TL_MAYU_R_C_WJ,
    MOT_BONE_FT_N_TOOTH_UPPER,
    MOT_BONE_FT_TL_TOOTH_UPPER_WJ,
    MOT_BONE_FT_N_KUBI_WJ_EX,
    MOT_BONE_FT_N_WAKI_L,
    MOT_BONE_FT_KL_WAKI_L_WJ,
    MOT_BONE_FT_TL_UP_KATA_L,
    MOT_BONE_FT_C_KATA_L,
    MOT_BONE_FT_J_KATA_L_WJ_CU,
    MOT_BONE_FT_J_UDE_L_WJ,
    MOT_BONE_FT_E_UDE_L_CP,
    MOT_BONE_FT_KL_TE_L_WJ,
    MOT_BONE_FT_N_HITO_L_EX,
    MOT_BONE_FT_NL_HITO_L_WJ,
    MOT_BONE_FT_NL_HITO_B_L_WJ,
    MOT_BONE_FT_NL_HITO_C_L_WJ,
    MOT_BONE_FT_N_KO_L_EX,
    MOT_BONE_FT_NL_KO_L_WJ,
    MOT_BONE_FT_NL_KO_B_L_WJ,
    MOT_BONE_FT_NL_KO_C_L_WJ,
    MOT_BONE_FT_N_KUSU_L_EX,
    MOT_BONE_FT_NL_KUSU_L_WJ,
    MOT_BONE_FT_NL_KUSU_B_L_WJ,
    MOT_BONE_FT_NL_KUSU_C_L_WJ,
    MOT_BONE_FT_N_NAKA_L_EX,
    MOT_BONE_FT_NL_NAKA_L_WJ,
    MOT_BONE_FT_NL_NAKA_B_L_WJ,
    MOT_BONE_FT_NL_NAKA_C_L_WJ,
    MOT_BONE_FT_N_OYA_L_EX,
    MOT_BONE_FT_NL_OYA_L_WJ,
    MOT_BONE_FT_NL_OYA_B_L_WJ,
    MOT_BONE_FT_NL_OYA_C_L_WJ,
    MOT_BONE_FT_N_STE_L_WJ_EX,
    MOT_BONE_FT_N_SUDE_L_WJ_EX,
    MOT_BONE_FT_N_SUDE_B_L_WJ_EX,
    MOT_BONE_FT_N_HIJI_L_WJ_EX,
    MOT_BONE_FT_N_UP_KATA_L_EX,
    MOT_BONE_FT_N_SKATA_L_WJ_CD_EX,
    MOT_BONE_FT_N_SKATA_B_L_WJ_CD_CU_EX,
    MOT_BONE_FT_N_SKATA_C_L_WJ_CD_CU_EX,
    MOT_BONE_FT_N_WAKI_R,
    MOT_BONE_FT_KL_WAKI_R_WJ,
    MOT_BONE_FT_TL_UP_KATA_R,
    MOT_BONE_FT_C_KATA_R,
    MOT_BONE_FT_J_KATA_R_WJ_CU,
    MOT_BONE_FT_J_UDE_R_WJ,
    MOT_BONE_FT_E_UDE_R_CP,
    MOT_BONE_FT_KL_TE_R_WJ,
    MOT_BONE_FT_N_HITO_R_EX,
    MOT_BONE_FT_NL_HITO_R_WJ,
    MOT_BONE_FT_NL_HITO_B_R_WJ,
    MOT_BONE_FT_NL_HITO_C_R_WJ,
    MOT_BONE_FT_N_KO_R_EX,
    MOT_BONE_FT_NL_KO_R_WJ,
    MOT_BONE_FT_NL_KO_B_R_WJ,
    MOT_BONE_FT_NL_KO_C_R_WJ,
    MOT_BONE_FT_N_KUSU_R_EX,
    MOT_BONE_FT_NL_KUSU_R_WJ,
    MOT_BONE_FT_NL_KUSU_B_R_WJ,
    MOT_BONE_FT_NL_KUSU_C_R_WJ,
    MOT_BONE_FT_N_NAKA_R_EX,
    MOT_BONE_FT_NL_NAKA_R_WJ,
    MOT_BONE_FT_NL_NAKA_B_R_WJ,
    MOT_BONE_FT_NL_NAKA_C_R_WJ,
    MOT_BONE_FT_N_OYA_R_EX,
    MOT_BONE_FT_NL_OYA_R_WJ,
    MOT_BONE_FT_NL_OYA_B_R_WJ,
    MOT_BONE_FT_NL_OYA_C_R_WJ,
    MOT_BONE_FT_N_STE_R_WJ_EX,
    MOT_BONE_FT_N_SUDE_R_WJ_EX,
    MOT_BONE_FT_N_SUDE_B_R_WJ_EX,
    MOT_BONE_FT_N_HIJI_R_WJ_EX,
    MOT_BONE_FT_N_UP_KATA_R_EX,
    MOT_BONE_FT_N_SKATA_R_WJ_CD_EX,
    MOT_BONE_FT_N_SKATA_B_R_WJ_CD_CU_EX,
    MOT_BONE_FT_N_SKATA_C_R_WJ_CD_CU_EX,
    MOT_BONE_FT_KL_KOSI_Y,
    MOT_BONE_FT_KL_KOSI_XZ,
    MOT_BONE_FT_KL_KOSI_ETC_WJ,
    MOT_BONE_FT_CL_MOMO_L,
    MOT_BONE_FT_J_MOMO_L_WJ,
    MOT_BONE_FT_J_SUNE_L_WJ,
    MOT_BONE_FT_E_SUNE_L_CP,
    MOT_BONE_FT_KL_ASI_L_WJ_CO,
    MOT_BONE_FT_KL_TOE_L_WJ,
    MOT_BONE_FT_N_HIZA_L_WJ_EX,
    MOT_BONE_FT_CL_MOMO_R,
    MOT_BONE_FT_J_MOMO_R_WJ,
    MOT_BONE_FT_J_SUNE_R_WJ,
    MOT_BONE_FT_E_SUNE_R_CP,
    MOT_BONE_FT_KL_ASI_R_WJ_CO,
    MOT_BONE_FT_KL_TOE_R_WJ,
    MOT_BONE_FT_N_HIZA_R_WJ_EX,
    MOT_BONE_FT_N_MOMO_A_L_WJ_CD_EX,
    MOT_BONE_FT_N_MOMO_B_L_WJ_EX,
    MOT_BONE_FT_N_MOMO_C_L_WJ_EX,
    MOT_BONE_FT_N_MOMO_A_R_WJ_CD_EX,
    MOT_BONE_FT_N_MOMO_B_R_WJ_EX,
    MOT_BONE_FT_N_MOMO_C_R_WJ_EX,
    MOT_BONE_FT_N_HARA_CD_EX,
    MOT_BONE_FT_N_HARA_B_WJ_EX,
    MOT_BONE_FT_N_HARA_C_WJ_EX,
    MOT_BONE_FT_MAX = 201,
} mot_bone_index_ft;

typedef struct mot_struct {
    float_t time;
    int32_t flags;
    int32_t frame;
    int32_t padding_0C;
    int32_t pv_branch;
    int32_t id;
    int32_t value;
    int32_t padding_1C;
} mot_struct;

typedef struct mot {
    bool ready;
    size_t length;
    mot_struct* data;
} mot;

extern const char* mot_bone_name_ac[MOT_BONE_AC_MAX];
extern const char* mot_bone_name_f[MOT_BONE_F_MAX];
extern const char* mot_bone_name_ft[MOT_BONE_FT_MAX];

extern mot* mot_init();
extern void mot_read_mot(mot* m, char* path);
extern void mot_wread_mot(mot* m, wchar_t* path);
extern void mot_write_mot(mot* m, char* path);
extern void mot_wwrite_mot(mot* m, wchar_t* path);
extern void mot_read_mp(mot* m, char* path, bool json);
extern void mot_wread_mp(mot* m, wchar_t* path, bool json);
extern void mot_write_mp(mot* m, char* path, bool json);
extern void mot_wwrite_mp(mot* m, wchar_t* path, bool json);
extern void mot_dispose(mot* m);
