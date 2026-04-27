/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../KKdLib/database/bone.hpp"
#include "../../KKdLib/database/motion.hpp"
#include "../../KKdLib/prj/bit_array.hpp"
#include "../../KKdLib/prj/vector_pair.hpp"
#include "../../KKdLib/mat.hpp"
#include "../../KKdLib/mot.hpp"
#include "../../KKdLib/rectangle.hpp"
#include "../../KKdLib/vec.hpp"
#include "../prj/memory_manager.hpp"
#include "../item_table.hpp"
#include "../render_context.hpp"
#include "../object.hpp"
#include "../static_var.hpp"

enum ACT_NAME {
    ROB_ACT_NONE = 0,
    ROB_ACT_MOTION,
    ROB_ACT_KAMAE,
    ROB_ACT_YARARE,
    ROB_ACT_GUARD,
    ROB_ACT_NAGE,
    ROB_ACT_NAGERARE,
    ROB_ACT_JUMP,
    ROB_ACT_DOWN,
    ROB_ACT_RISE,
    ROB_ACT_UKEMI,
    ROB_ACT_GUARD_POSE,
    ROB_ACT_RINGOUT,
    ROB_ACT_GACHA,
    ROB_ACT_WALK,
    ROB_ACT_RUN,
    ROB_ACT_DASH,
    ROB_ACT_SY_DASH,
    ROB_ACT_YOKE,
    ROB_ACT_ROPEWORK,
    ROB_ACT_MAX,
};

enum AtkPoint {
    AP_NO_ATTACK = 0,
    AP_HIGH,
    AP_MIDDLE,
    AP_LOW,
    AP_AIR,
    AP_VERY_LOW,
    AP_LITTLE_LOW,
    AP_LITTLE_HIGH,
    AP_NAGEMISS_HIGH,
    AP_NAGEMISS_LOW,
    AP_NAGEMISS_DOWN,
    AP_NAGEMISS_HI_AIR,
    AP_DOWN_ATTACK,
    AP_AIR_LOW,
    AP_AIR_DOWN_ATTACK,
    AP_MAX,
};

enum AtkUnit {
    AU_NO_ATTACK = 0,
    AU_KATA_R,
    AU_KATA_L,
    AU_UDE_R,
    AU_UDE_L,
    AU_TE_R,
    AU_TE_L,
    AU_HIZA_R,
    AU_HIZA_L,
    AU_ASI_R,
    AU_ASI_L,
    AU_FOOT_R,
    AU_FOOT_L,
    AU_RYOU_TE,
    AU_ASI_ALL_R,
    AU_ASI_ALL_L,
    AU_SENAKA,
    AU_KAO,
    AU_KOSI,
    AU_MUNE_KAO,
    AU_BODY,
    AU_RYOU_ASI,
    AU_ELBO_R,
    AU_ELBO_L,
    AU_TE_ALL_R,
    AU_TE_ALL_L,
    AU_ASI_ALL,
    AU_RYOU_HIZA,
    AU_TE_ASI_R,
    AU_TE_ASI_L,
    AU_KATA_MUNE_R,
    AU_KATA_MUNE_L,
    AU_SHIRI,
    AU_KAO_KATA,
    AU_RYOU_ASI_HIZA,
    AU_RYOU_UDE,
    AU_RYOU_TE_ALL,
    AU_ZENSHIN,
    AU_TETSUZAN,
    AU_TETSUZAN_M,
    AU_ELBO_HIZA_R,
    AU_ELBO_HIZA_L,
    AU_RYOU_TE_KOSI,
    AU_BUKI_R,
    AU_BUKI_L,
    AU_KATA_HIZA_R,
    AU_KATA_HIZA_L,
    AU_KATA_R_HIZA_L,
    AU_KATA_L_HIZA_R,
    AU_RYOU_HIJI,
    AU_RYOU_ASI_TE_R,
    AU_RYOU_ASI_TE_L,
    AU_ELBO_MUNE_R,
    AU_ELBO_MUNE_L,
    AU_TE_R_ASI_L,
    AU_TE_L_ASI_R,
    AU_BODY_TE_R,
    AU_BODY_TE_L,
    AU_BODY_ASI_R,
    AU_BODY_ASI_L,
    AU_BODY_TE_ASI_R,
    AU_BODY_TE_ASI_L,
    AU_RYOU_TE_KAO,
    AU_MAX,
};

enum BONE_BLK {
    BLK_DUMMY = -1,

    BLK_N_HARA_CP               = 0x00,
    BLK_KG_HARA_Y               = 0x01,
    BLK_KL_HARA_XZ              = 0x02,
    BLK_KL_HARA_ETC             = 0x03,
    BLK_N_HARA                  = 0x04,
    BLK_CL_MUNE                 = 0x05,
    BLK_N_MUNE_B                = 0x06,
    BLK_KL_MUNE_B_WJ            = 0x07,
    BLK_KL_KUBI                 = 0x08,
    BLK_N_KAO                   = 0x09,
    BLK_CL_KAO                  = 0x0A,
    BLK_FACE_ROOT               = 0x0B,
    BLK_N_AGO                   = 0x0C,
    BLK_KL_AGO_WJ               = 0x0D,
    BLK_N_TOOTH_UNDER           = 0x0E,
    BLK_TL_TOOTH_UNDER_WJ       = 0x0F,
    BLK_N_EYE_L                 = 0x10,
    BLK_KL_EYE_L                = 0x11,
    BLK_N_EYE_L_WJ_EX           = 0x12,
    BLK_KL_HIGHLIGHT_L_WJ       = 0x13,
    BLK_N_EYE_R                 = 0x14,
    BLK_KL_EYE_R                = 0x15,
    BLK_N_EYE_R_WJ_EX           = 0x16,
    BLK_KL_HIGHLIGHT_R_WJ       = 0x17,
    BLK_N_EYELID_L_A            = 0x18,
    BLK_TL_EYELID_L_A_WJ        = 0x19,
    BLK_N_EYELID_L_B            = 0x1A,
    BLK_TL_EYELID_L_B_WJ        = 0x1B,
    BLK_N_EYELID_R_A            = 0x1C,
    BLK_TL_EYELID_R_A_WJ        = 0x1D,
    BLK_N_EYELID_R_B            = 0x1E,
    BLK_TL_EYELID_R_B_WJ        = 0x1F,
    BLK_N_KUTI_D                = 0x20,
    BLK_TL_KUTI_D_WJ            = 0x21,
    BLK_N_KUTI_D_L              = 0x22,
    BLK_TL_KUTI_D_L_WJ          = 0x23,
    BLK_N_KUTI_D_R              = 0x24,
    BLK_TL_KUTI_D_R_WJ          = 0x25,
    BLK_N_KUTI_DS_L             = 0x26,
    BLK_TL_KUTI_DS_L_WJ         = 0x27,
    BLK_N_KUTI_DS_R             = 0x28,
    BLK_TL_KUTI_DS_R_WJ         = 0x29,
    BLK_N_KUTI_L                = 0x2A,
    BLK_TL_KUTI_L_WJ            = 0x2B,
    BLK_N_KUTI_M_L              = 0x2C,
    BLK_TL_KUTI_M_L_WJ          = 0x2D,
    BLK_N_KUTI_M_R              = 0x2E,
    BLK_TL_KUTI_M_R_WJ          = 0x2F,
    BLK_N_KUTI_R                = 0x30,
    BLK_TL_KUTI_R_WJ            = 0x31,
    BLK_N_KUTI_U                = 0x32,
    BLK_TL_KUTI_U_WJ            = 0x33,
    BLK_N_KUTI_U_L              = 0x34,
    BLK_TL_KUTI_U_L_WJ          = 0x35,
    BLK_N_KUTI_U_R              = 0x36,
    BLK_TL_KUTI_U_R_WJ          = 0x37,
    BLK_N_MABU_L_D_A            = 0x38,
    BLK_TL_MABU_L_D_A_WJ        = 0x39,
    BLK_N_MABU_L_D_B            = 0x3A,
    BLK_TL_MABU_L_D_B_WJ        = 0x3B,
    BLK_N_MABU_L_D_C            = 0x3C,
    BLK_TL_MABU_L_D_C_WJ        = 0x3D,
    BLK_N_MABU_L_U_A            = 0x3E,
    BLK_TL_MABU_L_U_A_WJ        = 0x3F,
    BLK_N_MABU_L_U_B            = 0x40,
    BLK_TL_MABU_L_U_B_WJ        = 0x41,
    BLK_N_EYELASHES_L           = 0x42,
    BLK_TL_EYELASHES_L_WJ       = 0x43,
    BLK_N_MABU_L_U_C            = 0x44,
    BLK_TL_MABU_L_U_C_WJ        = 0x45,
    BLK_N_MABU_R_D_A            = 0x46,
    BLK_TL_MABU_R_D_A_WJ        = 0x47,
    BLK_N_MABU_R_D_B            = 0x48,
    BLK_TL_MABU_R_D_B_WJ        = 0x49,
    BLK_N_MABU_R_D_C            = 0x4A,
    BLK_TL_MABU_R_D_C_WJ        = 0x4B,
    BLK_N_MABU_R_U_A            = 0x4C,
    BLK_TL_MABU_R_U_A_WJ        = 0x4D,
    BLK_N_MABU_R_U_B            = 0x4E,
    BLK_TL_MABU_R_U_B_WJ        = 0x4F,
    BLK_N_EYELASHES_R           = 0x50,
    BLK_TL_EYELASHES_R_WJ       = 0x51,
    BLK_N_MABU_R_U_C            = 0x52,
    BLK_TL_MABU_R_U_C_WJ        = 0x53,
    BLK_N_MAYU_L                = 0x54,
    BLK_TL_MAYU_L_WJ            = 0x55,
    BLK_N_MAYU_L_B              = 0x56,
    BLK_TL_MAYU_L_B_WJ          = 0x57,
    BLK_N_MAYU_L_C              = 0x58,
    BLK_TL_MAYU_L_C_WJ          = 0x59,
    BLK_N_MAYU_R                = 0x5A,
    BLK_TL_MAYU_R_WJ            = 0x5B,
    BLK_N_MAYU_R_B              = 0x5C,
    BLK_TL_MAYU_R_B_WJ          = 0x5D,
    BLK_N_MAYU_R_C              = 0x5E,
    BLK_TL_MAYU_R_C_WJ          = 0x5F,
    BLK_N_TOOTH_UPPER           = 0x60,
    BLK_TL_TOOTH_UPPER_WJ       = 0x61,
    BLK_N_KUBI_WJ_EX            = 0x62,
    BLK_N_WAKI_L                = 0x63,
    BLK_KL_WAKI_L_WJ            = 0x64,
    BLK_TL_UP_KATA_L            = 0x65,
    BLK_C_KATA_L                = 0x66,
    BLK_KL_TE_L_WJ              = 0x67,
    BLK_N_HITO_L_EX             = 0x68,
    BLK_NL_HITO_L_WJ            = 0x69,
    BLK_NL_HITO_B_L_WJ          = 0x6A,
    BLK_NL_HITO_C_L_WJ          = 0x6B,
    BLK_N_KO_L_EX               = 0x6C,
    BLK_NL_KO_L_WJ              = 0x6D,
    BLK_NL_KO_B_L_WJ            = 0x6E,
    BLK_NL_KO_C_L_WJ            = 0x6F,
    BLK_N_KUSU_L_EX             = 0x70,
    BLK_NL_KUSU_L_WJ            = 0x71,
    BLK_NL_KUSU_B_L_WJ          = 0x72,
    BLK_NL_KUSU_C_L_WJ          = 0x73,
    BLK_N_NAKA_L_EX             = 0x74,
    BLK_NL_NAKA_L_WJ            = 0x75,
    BLK_NL_NAKA_B_L_WJ          = 0x76,
    BLK_NL_NAKA_C_L_WJ          = 0x77,
    BLK_N_OYA_L_EX              = 0x78,
    BLK_NL_OYA_L_WJ             = 0x79,
    BLK_NL_OYA_B_L_WJ           = 0x7A,
    BLK_NL_OYA_C_L_WJ           = 0x7B,
    BLK_N_STE_L_WJ_EX           = 0x7C,
    BLK_N_SUDE_L_WJ_EX          = 0x7D,
    BLK_N_SUDE_B_L_WJ_EX        = 0x7E,
    BLK_N_HIJI_L_WJ_EX          = 0x7F,
    BLK_N_UP_KATA_L_EX          = 0x80,
    BLK_N_SKATA_L_WJ_CD_EX      = 0x81,
    BLK_N_SKATA_B_L_WJ_CD_CU_EX = 0x82,
    BLK_N_SKATA_C_L_WJ_CD_CU_EX = 0x83,
    BLK_N_WAKI_R                = 0x84,
    BLK_KL_WAKI_R_WJ            = 0x85,
    BLK_TL_UP_KATA_R            = 0x86,
    BLK_C_KATA_R                = 0x87,
    BLK_KL_TE_R_WJ              = 0x88,
    BLK_N_HITO_R_EX             = 0x89,
    BLK_NL_HITO_R_WJ            = 0x8A,
    BLK_NL_HITO_B_R_WJ          = 0x8B,
    BLK_NL_HITO_C_R_WJ          = 0x8C,
    BLK_N_KO_R_EX               = 0x8D,
    BLK_NL_KO_R_WJ              = 0x8E,
    BLK_NL_KO_B_R_WJ            = 0x8F,
    BLK_NL_KO_C_R_WJ            = 0x90,
    BLK_N_KUSU_R_EX             = 0x91,
    BLK_NL_KUSU_R_WJ            = 0x92,
    BLK_NL_KUSU_B_R_WJ          = 0x93,
    BLK_NL_KUSU_C_R_WJ          = 0x94,
    BLK_N_NAKA_R_EX             = 0x95,
    BLK_NL_NAKA_R_WJ            = 0x96,
    BLK_NL_NAKA_B_R_WJ          = 0x97,
    BLK_NL_NAKA_C_R_WJ          = 0x98,
    BLK_N_OYA_R_EX              = 0x99,
    BLK_NL_OYA_R_WJ             = 0x9A,
    BLK_NL_OYA_B_R_WJ           = 0x9B,
    BLK_NL_OYA_C_R_WJ           = 0x9C,
    BLK_N_STE_R_WJ_EX           = 0x9D,
    BLK_N_SUDE_R_WJ_EX          = 0x9E,
    BLK_N_SUDE_B_R_WJ_EX        = 0x9F,
    BLK_N_HIJI_R_WJ_EX          = 0xA0,
    BLK_N_UP_KATA_R_EX          = 0xA1,
    BLK_N_SKATA_R_WJ_CD_EX      = 0xA2,
    BLK_N_SKATA_B_R_WJ_CD_CU_EX = 0xA3,
    BLK_N_SKATA_C_R_WJ_CD_CU_EX = 0xA4,
    BLK_KL_KOSI_Y               = 0xA5,
    BLK_KL_KOSI_XZ              = 0xA6,
    BLK_KL_KOSI_ETC_WJ          = 0xA7,
    BLK_CL_MOMO_L               = 0xA8,
    BLK_KL_ASI_L_WJ_CO          = 0xA9,
    BLK_KL_TOE_L_WJ             = 0xAA,
    BLK_N_HIZA_L_WJ_EX          = 0xAB,
    BLK_CL_MOMO_R               = 0xAC,
    BLK_KL_ASI_R_WJ_CO          = 0xAD,
    BLK_KL_TOE_R_WJ             = 0xAE,
    BLK_N_HIZA_R_WJ_EX          = 0xAF,
    BLK_N_MOMO_A_L_WJ_CD_EX     = 0xB0,
    BLK_N_MOMO_B_L_WJ_EX        = 0xB1,
    BLK_N_MOMO_C_L_WJ_EX        = 0xB2,
    BLK_N_MOMO_A_R_WJ_CD_EX     = 0xB3,
    BLK_N_MOMO_B_R_WJ_EX        = 0xB4,
    BLK_N_MOMO_C_R_WJ_EX        = 0xB5,
    BLK_N_HARA_CD_EX            = 0xB6,
    BLK_N_HARA_B_WJ_EX          = 0xB7,
    BLK_N_HARA_C_WJ_EX          = 0xB8,
    BLK_MAX                     = 0xB9,
};

enum BONE_ID {
    BONE_ID_DUMMY = -1,

    BONE_ID_N_HARA_CP               = 0x00,
    BONE_ID_KG_HARA_Y               = 0x01,
    BONE_ID_KL_HARA_XZ              = 0x02,
    BONE_ID_KL_HARA_ETC             = 0x03,
    BONE_ID_N_HARA                  = 0x04,
    BONE_ID_CL_MUNE                 = 0x05,
    BONE_ID_N_MUNE_B                = 0x06,
    BONE_ID_KL_MUNE_B_WJ            = 0x07,
    BONE_ID_KL_KUBI                 = 0x08,
    BONE_ID_N_KAO                   = 0x09,
    BONE_ID_CL_KAO                  = 0x0A,
    BONE_ID_FACE_ROOT               = 0x0B,
    BONE_ID_N_AGO                   = 0x0C,
    BONE_ID_KL_AGO_WJ               = 0x0D,
    BONE_ID_N_TOOTH_UNDER           = 0x0E,
    BONE_ID_TL_TOOTH_UNDER_WJ       = 0x0F,
    BONE_ID_N_EYE_L                 = 0x10,
    BONE_ID_KL_EYE_L                = 0x11,
    BONE_ID_N_EYE_L_WJ_EX           = 0x12,
    BONE_ID_KL_HIGHLIGHT_L_WJ       = 0x13,
    BONE_ID_N_EYE_R                 = 0x14,
    BONE_ID_KL_EYE_R                = 0x15,
    BONE_ID_N_EYE_R_WJ_EX           = 0x16,
    BONE_ID_KL_HIGHLIGHT_R_WJ       = 0x17,
    BONE_ID_N_EYELID_L_A            = 0x18,
    BONE_ID_TL_EYELID_L_A_WJ        = 0x19,
    BONE_ID_N_EYELID_L_B            = 0x1A,
    BONE_ID_TL_EYELID_L_B_WJ        = 0x1B,
    BONE_ID_N_EYELID_R_A            = 0x1C,
    BONE_ID_TL_EYELID_R_A_WJ        = 0x1D,
    BONE_ID_N_EYELID_R_B            = 0x1E,
    BONE_ID_TL_EYELID_R_B_WJ        = 0x1F,
    BONE_ID_N_KUTI_D                = 0x20,
    BONE_ID_TL_KUTI_D_WJ            = 0x21,
    BONE_ID_N_KUTI_D_L              = 0x22,
    BONE_ID_TL_KUTI_D_L_WJ          = 0x23,
    BONE_ID_N_KUTI_D_R              = 0x24,
    BONE_ID_TL_KUTI_D_R_WJ          = 0x25,
    BONE_ID_N_KUTI_DS_L             = 0x26,
    BONE_ID_TL_KUTI_DS_L_WJ         = 0x27,
    BONE_ID_N_KUTI_DS_R             = 0x28,
    BONE_ID_TL_KUTI_DS_R_WJ         = 0x29,
    BONE_ID_N_KUTI_L                = 0x2A,
    BONE_ID_TL_KUTI_L_WJ            = 0x2B,
    BONE_ID_N_KUTI_M_L              = 0x2C,
    BONE_ID_TL_KUTI_M_L_WJ          = 0x2D,
    BONE_ID_N_KUTI_M_R              = 0x2E,
    BONE_ID_TL_KUTI_M_R_WJ          = 0x2F,
    BONE_ID_N_KUTI_R                = 0x30,
    BONE_ID_TL_KUTI_R_WJ            = 0x31,
    BONE_ID_N_KUTI_U                = 0x32,
    BONE_ID_TL_KUTI_U_WJ            = 0x33,
    BONE_ID_N_KUTI_U_L              = 0x34,
    BONE_ID_TL_KUTI_U_L_WJ          = 0x35,
    BONE_ID_N_KUTI_U_R              = 0x36,
    BONE_ID_TL_KUTI_U_R_WJ          = 0x37,
    BONE_ID_N_MABU_L_D_A            = 0x38,
    BONE_ID_TL_MABU_L_D_A_WJ        = 0x39,
    BONE_ID_N_MABU_L_D_B            = 0x3A,
    BONE_ID_TL_MABU_L_D_B_WJ        = 0x3B,
    BONE_ID_N_MABU_L_D_C            = 0x3C,
    BONE_ID_TL_MABU_L_D_C_WJ        = 0x3D,
    BONE_ID_N_MABU_L_U_A            = 0x3E,
    BONE_ID_TL_MABU_L_U_A_WJ        = 0x3F,
    BONE_ID_N_MABU_L_U_B            = 0x40,
    BONE_ID_TL_MABU_L_U_B_WJ        = 0x41,
    BONE_ID_N_EYELASHES_L           = 0x42,
    BONE_ID_TL_EYELASHES_L_WJ       = 0x43,
    BONE_ID_N_MABU_L_U_C            = 0x44,
    BONE_ID_TL_MABU_L_U_C_WJ        = 0x45,
    BONE_ID_N_MABU_R_D_A            = 0x46,
    BONE_ID_TL_MABU_R_D_A_WJ        = 0x47,
    BONE_ID_N_MABU_R_D_B            = 0x48,
    BONE_ID_TL_MABU_R_D_B_WJ        = 0x49,
    BONE_ID_N_MABU_R_D_C            = 0x4A,
    BONE_ID_TL_MABU_R_D_C_WJ        = 0x4B,
    BONE_ID_N_MABU_R_U_A            = 0x4C,
    BONE_ID_TL_MABU_R_U_A_WJ        = 0x4D,
    BONE_ID_N_MABU_R_U_B            = 0x4E,
    BONE_ID_TL_MABU_R_U_B_WJ        = 0x4F,
    BONE_ID_N_EYELASHES_R           = 0x50,
    BONE_ID_TL_EYELASHES_R_WJ       = 0x51,
    BONE_ID_N_MABU_R_U_C            = 0x52,
    BONE_ID_TL_MABU_R_U_C_WJ        = 0x53,
    BONE_ID_N_MAYU_L                = 0x54,
    BONE_ID_TL_MAYU_L_WJ            = 0x55,
    BONE_ID_N_MAYU_L_B              = 0x56,
    BONE_ID_TL_MAYU_L_B_WJ          = 0x57,
    BONE_ID_N_MAYU_L_C              = 0x58,
    BONE_ID_TL_MAYU_L_C_WJ          = 0x59,
    BONE_ID_N_MAYU_R                = 0x5A,
    BONE_ID_TL_MAYU_R_WJ            = 0x5B,
    BONE_ID_N_MAYU_R_B              = 0x5C,
    BONE_ID_TL_MAYU_R_B_WJ          = 0x5D,
    BONE_ID_N_MAYU_R_C              = 0x5E,
    BONE_ID_TL_MAYU_R_C_WJ          = 0x5F,
    BONE_ID_N_TOOTH_UPPER           = 0x60,
    BONE_ID_TL_TOOTH_UPPER_WJ       = 0x61,
    BONE_ID_N_KUBI_WJ_EX            = 0x62,
    BONE_ID_N_WAKI_L                = 0x63,
    BONE_ID_KL_WAKI_L_WJ            = 0x64,
    BONE_ID_TL_UP_KATA_L            = 0x65,
    BONE_ID_C_KATA_L                = 0x66,
    BONE_ID_KATA_L_WJ_CU            = 0x67,
    BONE_ID_UDE_L_WJ                = 0x68,
    BONE_ID_KL_TE_L_WJ              = 0x69,
    BONE_ID_N_HITO_L_EX             = 0x6A,
    BONE_ID_NL_HITO_L_WJ            = 0x6B,
    BONE_ID_NL_HITO_B_L_WJ          = 0x6C,
    BONE_ID_NL_HITO_C_L_WJ          = 0x6D,
    BONE_ID_N_KO_L_EX               = 0x6E,
    BONE_ID_NL_KO_L_WJ              = 0x6F,
    BONE_ID_NL_KO_B_L_WJ            = 0x70,
    BONE_ID_NL_KO_C_L_WJ            = 0x71,
    BONE_ID_N_KUSU_L_EX             = 0x72,
    BONE_ID_NL_KUSU_L_WJ            = 0x73,
    BONE_ID_NL_KUSU_B_L_WJ          = 0x74,
    BONE_ID_NL_KUSU_C_L_WJ          = 0x75,
    BONE_ID_N_NAKA_L_EX             = 0x76,
    BONE_ID_NL_NAKA_L_WJ            = 0x77,
    BONE_ID_NL_NAKA_B_L_WJ          = 0x78,
    BONE_ID_NL_NAKA_C_L_WJ          = 0x79,
    BONE_ID_N_OYA_L_EX              = 0x7A,
    BONE_ID_NL_OYA_L_WJ             = 0x7B,
    BONE_ID_NL_OYA_B_L_WJ           = 0x7C,
    BONE_ID_NL_OYA_C_L_WJ           = 0x7D,
    BONE_ID_N_STE_L_WJ_EX           = 0x7E,
    BONE_ID_N_SUDE_L_WJ_EX          = 0x7F,
    BONE_ID_N_SUDE_B_L_WJ_EX        = 0x80,
    BONE_ID_N_HIJI_L_WJ_EX          = 0x81,
    BONE_ID_N_UP_KATA_L_EX          = 0x82,
    BONE_ID_N_SKATA_L_WJ_CD_EX      = 0x83,
    BONE_ID_N_SKATA_B_L_WJ_CD_CU_EX = 0x84,
    BONE_ID_N_SKATA_C_L_WJ_CD_CU_EX = 0x85,
    BONE_ID_N_WAKI_R                = 0x86,
    BONE_ID_KL_WAKI_R_WJ            = 0x87,
    BONE_ID_TL_UP_KATA_R            = 0x88,
    BONE_ID_C_KATA_R                = 0x89,
    BONE_ID_KATA_R_WJ_CU            = 0x8A,
    BONE_ID_UDE_R_WJ                = 0x8B,
    BONE_ID_KL_TE_R_WJ              = 0x8C,
    BONE_ID_N_HITO_R_EX             = 0x8D,
    BONE_ID_NL_HITO_R_WJ            = 0x8E,
    BONE_ID_NL_HITO_B_R_WJ          = 0x8F,
    BONE_ID_NL_HITO_C_R_WJ          = 0x90,
    BONE_ID_N_KO_R_EX               = 0x91,
    BONE_ID_NL_KO_R_WJ              = 0x92,
    BONE_ID_NL_KO_B_R_WJ            = 0x93,
    BONE_ID_NL_KO_C_R_WJ            = 0x94,
    BONE_ID_N_KUSU_R_EX             = 0x95,
    BONE_ID_NL_KUSU_R_WJ            = 0x96,
    BONE_ID_NL_KUSU_B_R_WJ          = 0x97,
    BONE_ID_NL_KUSU_C_R_WJ          = 0x98,
    BONE_ID_N_NAKA_R_EX             = 0x99,
    BONE_ID_NL_NAKA_R_WJ            = 0x9A,
    BONE_ID_NL_NAKA_B_R_WJ          = 0x9B,
    BONE_ID_NL_NAKA_C_R_WJ          = 0x9C,
    BONE_ID_N_OYA_R_EX              = 0x9D,
    BONE_ID_NL_OYA_R_WJ             = 0x9E,
    BONE_ID_NL_OYA_B_R_WJ           = 0x9F,
    BONE_ID_NL_OYA_C_R_WJ           = 0xA0,
    BONE_ID_N_STE_R_WJ_EX           = 0xA1,
    BONE_ID_N_SUDE_R_WJ_EX          = 0xA2,
    BONE_ID_N_SUDE_B_R_WJ_EX        = 0xA3,
    BONE_ID_N_HIJI_R_WJ_EX          = 0xA4,
    BONE_ID_N_UP_KATA_R_EX          = 0xA5,
    BONE_ID_N_SKATA_R_WJ_CD_EX      = 0xA6,
    BONE_ID_N_SKATA_B_R_WJ_CD_CU_EX = 0xA7,
    BONE_ID_N_SKATA_C_R_WJ_CD_CU_EX = 0xA8,
    BONE_ID_KL_KOSI_Y               = 0xA9,
    BONE_ID_KL_KOSI_XZ              = 0xAA,
    BONE_ID_KL_KOSI_ETC_WJ          = 0xAB,
    BONE_ID_CL_MOMO_L               = 0xAC,
    BONE_ID_J_MOMO_L_WJ             = 0xAD,
    BONE_ID_J_SUNE_L_WJ             = 0xAE,
    BONE_ID_KL_ASI_L_WJ_CO          = 0xAF,
    BONE_ID_KL_TOE_L_WJ             = 0xB0,
    BONE_ID_N_HIZA_L_WJ_EX          = 0xB1,
    BONE_ID_CL_MOMO_R               = 0xB2,
    BONE_ID_J_MOMO_R_WJ             = 0xB3,
    BONE_ID_J_SUNE_R_WJ             = 0xB4,
    BONE_ID_KL_ASI_R_WJ_CO          = 0xB5,
    BONE_ID_KL_TOE_R_WJ             = 0xB6,
    BONE_ID_N_HIZA_R_WJ_EX          = 0xB7,
    BONE_ID_N_MOMO_A_L_WJ_CD_EX     = 0xB8,
    BONE_ID_N_MOMO_B_L_WJ_EX        = 0xB9,
    BONE_ID_N_MOMO_C_L_WJ_EX        = 0xBA,
    BONE_ID_N_MOMO_A_R_WJ_CD_EX     = 0xBB,
    BONE_ID_N_MOMO_B_R_WJ_EX        = 0xBC,
    BONE_ID_N_MOMO_C_R_WJ_EX        = 0xBD,
    BONE_ID_N_HARA_CD_EX            = 0xBE,
    BONE_ID_N_HARA_B_WJ_EX          = 0xBF,
    BONE_ID_N_HARA_C_WJ_EX          = 0xC0,
    BONE_ID_MAX                     = 0xC1,
};

enum BONE_NODE {
    BONE_NODE_N_HARA_CP               = 0x00,
    BONE_NODE_KG_HARA_Y               = 0x01,
    BONE_NODE_KL_HARA_XZ              = 0x02,
    BONE_NODE_KL_HARA_ETC             = 0x03,
    BONE_NODE_N_HARA                  = 0x04,
    BONE_NODE_CL_MUNE                 = 0x05,
    BONE_NODE_J_MUNE_WJ               = 0x06,
    BONE_NODE_E_MUNE_CP               = 0x07,
    BONE_NODE_N_MUNE_B                = 0x08,
    BONE_NODE_KL_MUNE_B_WJ            = 0x09,
    BONE_NODE_KL_KUBI                 = 0x0A,
    BONE_NODE_N_KAO                   = 0x0B,
    BONE_NODE_CL_KAO                  = 0x0C,
    BONE_NODE_J_KAO_WJ                = 0x0D,
    BONE_NODE_E_KAO_CP                = 0x0E,
    BONE_NODE_FACE_ROOT               = 0x0F,
    BONE_NODE_N_AGO                   = 0x10,
    BONE_NODE_KL_AGO_WJ               = 0x11,
    BONE_NODE_N_TOOTH_UNDER           = 0x12,
    BONE_NODE_TL_TOOTH_UNDER_WJ       = 0x13,
    BONE_NODE_N_EYE_L                 = 0x14,
    BONE_NODE_KL_EYE_L                = 0x15,
    BONE_NODE_N_EYE_L_WJ_EX           = 0x16,
    BONE_NODE_KL_HIGHLIGHT_L_WJ       = 0x17,
    BONE_NODE_N_EYE_R                 = 0x18,
    BONE_NODE_KL_EYE_R                = 0x19,
    BONE_NODE_N_EYE_R_WJ_EX           = 0x1A,
    BONE_NODE_KL_HIGHLIGHT_R_WJ       = 0x1B,
    BONE_NODE_N_EYELID_L_A            = 0x1C,
    BONE_NODE_TL_EYELID_L_A_WJ        = 0x1D,
    BONE_NODE_N_EYELID_L_B            = 0x1E,
    BONE_NODE_TL_EYELID_L_B_WJ        = 0x1F,
    BONE_NODE_N_EYELID_R_A            = 0x20,
    BONE_NODE_TL_EYELID_R_A_WJ        = 0x21,
    BONE_NODE_N_EYELID_R_B            = 0x22,
    BONE_NODE_TL_EYELID_R_B_WJ        = 0x23,
    BONE_NODE_N_KUTI_D                = 0x24,
    BONE_NODE_TL_KUTI_D_WJ            = 0x25,
    BONE_NODE_N_KUTI_D_L              = 0x26,
    BONE_NODE_TL_KUTI_D_L_WJ          = 0x27,
    BONE_NODE_N_KUTI_D_R              = 0x28,
    BONE_NODE_TL_KUTI_D_R_WJ          = 0x29,
    BONE_NODE_N_KUTI_DS_L             = 0x2A,
    BONE_NODE_TL_KUTI_DS_L_WJ         = 0x2B,
    BONE_NODE_N_KUTI_DS_R             = 0x2C,
    BONE_NODE_TL_KUTI_DS_R_WJ         = 0x2D,
    BONE_NODE_N_KUTI_L                = 0x2E,
    BONE_NODE_TL_KUTI_L_WJ            = 0x2F,
    BONE_NODE_N_KUTI_M_L              = 0x30,
    BONE_NODE_TL_KUTI_M_L_WJ          = 0x31,
    BONE_NODE_N_KUTI_M_R              = 0x32,
    BONE_NODE_TL_KUTI_M_R_WJ          = 0x33,
    BONE_NODE_N_KUTI_R                = 0x34,
    BONE_NODE_TL_KUTI_R_WJ            = 0x35,
    BONE_NODE_N_KUTI_U                = 0x36,
    BONE_NODE_TL_KUTI_U_WJ            = 0x37,
    BONE_NODE_N_KUTI_U_L              = 0x38,
    BONE_NODE_TL_KUTI_U_L_WJ          = 0x39,
    BONE_NODE_N_KUTI_U_R              = 0x3A,
    BONE_NODE_TL_KUTI_U_R_WJ          = 0x3B,
    BONE_NODE_N_MABU_L_D_A            = 0x3C,
    BONE_NODE_TL_MABU_L_D_A_WJ        = 0x3D,
    BONE_NODE_N_MABU_L_D_B            = 0x3E,
    BONE_NODE_TL_MABU_L_D_B_WJ        = 0x3F,
    BONE_NODE_N_MABU_L_D_C            = 0x40,
    BONE_NODE_TL_MABU_L_D_C_WJ        = 0x41,
    BONE_NODE_N_MABU_L_U_A            = 0x42,
    BONE_NODE_TL_MABU_L_U_A_WJ        = 0x43,
    BONE_NODE_N_MABU_L_U_B            = 0x44,
    BONE_NODE_TL_MABU_L_U_B_WJ        = 0x45,
    BONE_NODE_N_EYELASHES_L           = 0x46,
    BONE_NODE_TL_EYELASHES_L_WJ       = 0x47,
    BONE_NODE_N_MABU_L_U_C            = 0x48,
    BONE_NODE_TL_MABU_L_U_C_WJ        = 0x49,
    BONE_NODE_N_MABU_R_D_A            = 0x4A,
    BONE_NODE_TL_MABU_R_D_A_WJ        = 0x4B,
    BONE_NODE_N_MABU_R_D_B            = 0x4C,
    BONE_NODE_TL_MABU_R_D_B_WJ        = 0x4D,
    BONE_NODE_N_MABU_R_D_C            = 0x4E,
    BONE_NODE_TL_MABU_R_D_C_WJ        = 0x4F,
    BONE_NODE_N_MABU_R_U_A            = 0x50,
    BONE_NODE_TL_MABU_R_U_A_WJ        = 0x51,
    BONE_NODE_N_MABU_R_U_B            = 0x52,
    BONE_NODE_TL_MABU_R_U_B_WJ        = 0x53,
    BONE_NODE_N_EYELASHES_R           = 0x54,
    BONE_NODE_TL_EYELASHES_R_WJ       = 0x55,
    BONE_NODE_N_MABU_R_U_C            = 0x56,
    BONE_NODE_TL_MABU_R_U_C_WJ        = 0x57,
    BONE_NODE_N_MAYU_L                = 0x58,
    BONE_NODE_TL_MAYU_L_WJ            = 0x59,
    BONE_NODE_N_MAYU_L_B              = 0x5A,
    BONE_NODE_TL_MAYU_L_B_WJ          = 0x5B,
    BONE_NODE_N_MAYU_L_C              = 0x5C,
    BONE_NODE_TL_MAYU_L_C_WJ          = 0x5D,
    BONE_NODE_N_MAYU_R                = 0x5E,
    BONE_NODE_TL_MAYU_R_WJ            = 0x5F,
    BONE_NODE_N_MAYU_R_B              = 0x60,
    BONE_NODE_TL_MAYU_R_B_WJ          = 0x61,
    BONE_NODE_N_MAYU_R_C              = 0x62,
    BONE_NODE_TL_MAYU_R_C_WJ          = 0x63,
    BONE_NODE_N_TOOTH_UPPER           = 0x64,
    BONE_NODE_TL_TOOTH_UPPER_WJ       = 0x65,
    BONE_NODE_N_KUBI_WJ_EX            = 0x66,
    BONE_NODE_N_WAKI_L                = 0x67,
    BONE_NODE_KL_WAKI_L_WJ            = 0x68,
    BONE_NODE_TL_UP_KATA_L            = 0x69,
    BONE_NODE_C_KATA_L                = 0x6A,
    BONE_NODE_J_KATA_L_WJ_CU          = 0x6B,
    BONE_NODE_J_UDE_L_WJ              = 0x6C,
    BONE_NODE_E_UDE_L_CP              = 0x6D,
    BONE_NODE_KL_TE_L_WJ              = 0x6E,
    BONE_NODE_N_HITO_L_EX             = 0x6F,
    BONE_NODE_NL_HITO_L_WJ            = 0x70,
    BONE_NODE_NL_HITO_B_L_WJ          = 0x71,
    BONE_NODE_NL_HITO_C_L_WJ          = 0x72,
    BONE_NODE_N_KO_L_EX               = 0x73,
    BONE_NODE_NL_KO_L_WJ              = 0x74,
    BONE_NODE_NL_KO_B_L_WJ            = 0x75,
    BONE_NODE_NL_KO_C_L_WJ            = 0x76,
    BONE_NODE_N_KUSU_L_EX             = 0x77,
    BONE_NODE_NL_KUSU_L_WJ            = 0x78,
    BONE_NODE_NL_KUSU_B_L_WJ          = 0x79,
    BONE_NODE_NL_KUSU_C_L_WJ          = 0x7A,
    BONE_NODE_N_NAKA_L_EX             = 0x7B,
    BONE_NODE_NL_NAKA_L_WJ            = 0x7C,
    BONE_NODE_NL_NAKA_B_L_WJ          = 0x7D,
    BONE_NODE_NL_NAKA_C_L_WJ          = 0x7E,
    BONE_NODE_N_OYA_L_EX              = 0x7F,
    BONE_NODE_NL_OYA_L_WJ             = 0x80,
    BONE_NODE_NL_OYA_B_L_WJ           = 0x81,
    BONE_NODE_NL_OYA_C_L_WJ           = 0x82,
    BONE_NODE_N_STE_L_WJ_EX           = 0x83,
    BONE_NODE_N_SUDE_L_WJ_EX          = 0x84,
    BONE_NODE_N_SUDE_B_L_WJ_EX        = 0x85,
    BONE_NODE_N_HIJI_L_WJ_EX          = 0x86,
    BONE_NODE_N_UP_KATA_L_EX          = 0x87,
    BONE_NODE_N_SKATA_L_WJ_CD_EX      = 0x88,
    BONE_NODE_N_SKATA_B_L_WJ_CD_CU_EX = 0x89,
    BONE_NODE_N_SKATA_C_L_WJ_CD_CU_EX = 0x8A,
    BONE_NODE_N_WAKI_R                = 0x8B,
    BONE_NODE_KL_WAKI_R_WJ            = 0x8C,
    BONE_NODE_TL_UP_KATA_R            = 0x8D,
    BONE_NODE_C_KATA_R                = 0x8E,
    BONE_NODE_J_KATA_R_WJ_CU          = 0x8F,
    BONE_NODE_J_UDE_R_WJ              = 0x90,
    BONE_NODE_E_UDE_R_CP              = 0x91,
    BONE_NODE_KL_TE_R_WJ              = 0x92,
    BONE_NODE_N_HITO_R_EX             = 0x93,
    BONE_NODE_NL_HITO_R_WJ            = 0x94,
    BONE_NODE_NL_HITO_B_R_WJ          = 0x95,
    BONE_NODE_NL_HITO_C_R_WJ          = 0x96,
    BONE_NODE_N_KO_R_EX               = 0x97,
    BONE_NODE_NL_KO_R_WJ              = 0x98,
    BONE_NODE_NL_KO_B_R_WJ            = 0x99,
    BONE_NODE_NL_KO_C_R_WJ            = 0x9A,
    BONE_NODE_N_KUSU_R_EX             = 0x9B,
    BONE_NODE_NL_KUSU_R_WJ            = 0x9C,
    BONE_NODE_NL_KUSU_B_R_WJ          = 0x9D,
    BONE_NODE_NL_KUSU_C_R_WJ          = 0x9E,
    BONE_NODE_N_NAKA_R_EX             = 0x9F,
    BONE_NODE_NL_NAKA_R_WJ            = 0xA0,
    BONE_NODE_NL_NAKA_B_R_WJ          = 0xA1,
    BONE_NODE_NL_NAKA_C_R_WJ          = 0xA2,
    BONE_NODE_N_OYA_R_EX              = 0xA3,
    BONE_NODE_NL_OYA_R_WJ             = 0xA4,
    BONE_NODE_NL_OYA_B_R_WJ           = 0xA5,
    BONE_NODE_NL_OYA_C_R_WJ           = 0xA6,
    BONE_NODE_N_STE_R_WJ_EX           = 0xA7,
    BONE_NODE_N_SUDE_R_WJ_EX          = 0xA8,
    BONE_NODE_N_SUDE_B_R_WJ_EX        = 0xA9,
    BONE_NODE_N_HIJI_R_WJ_EX          = 0xAA,
    BONE_NODE_N_UP_KATA_R_EX          = 0xAB,
    BONE_NODE_N_SKATA_R_WJ_CD_EX      = 0xAC,
    BONE_NODE_N_SKATA_B_R_WJ_CD_CU_EX = 0xAD,
    BONE_NODE_N_SKATA_C_R_WJ_CD_CU_EX = 0xAE,
    BONE_NODE_KL_KOSI_Y               = 0xAF,
    BONE_NODE_KL_KOSI_XZ              = 0xB0,
    BONE_NODE_KL_KOSI_ETC_WJ          = 0xB1,
    BONE_NODE_CL_MOMO_L               = 0xB2,
    BONE_NODE_J_MOMO_L_WJ             = 0xB3,
    BONE_NODE_J_SUNE_L_WJ             = 0xB4,
    BONE_NODE_E_SUNE_L_CP             = 0xB5,
    BONE_NODE_KL_ASI_L_WJ_CO          = 0xB6,
    BONE_NODE_KL_TOE_L_WJ             = 0xB7,
    BONE_NODE_N_HIZA_L_WJ_EX          = 0xB8,
    BONE_NODE_CL_MOMO_R               = 0xB9,
    BONE_NODE_J_MOMO_R_WJ             = 0xBA,
    BONE_NODE_J_SUNE_R_WJ             = 0xBB,
    BONE_NODE_E_SUNE_R_CP             = 0xBC,
    BONE_NODE_KL_ASI_R_WJ_CO          = 0xBD,
    BONE_NODE_KL_TOE_R_WJ             = 0xBE,
    BONE_NODE_N_HIZA_R_WJ_EX          = 0xBF,
    BONE_NODE_N_MOMO_A_L_WJ_CD_EX     = 0xC0,
    BONE_NODE_N_MOMO_B_L_WJ_EX        = 0xC1,
    BONE_NODE_N_MOMO_C_L_WJ_EX        = 0xC2,
    BONE_NODE_N_MOMO_A_R_WJ_CD_EX     = 0xC3,
    BONE_NODE_N_MOMO_B_R_WJ_EX        = 0xC4,
    BONE_NODE_N_MOMO_C_R_WJ_EX        = 0xC5,
    BONE_NODE_N_HARA_CD_EX            = 0xC6,
    BONE_NODE_N_HARA_B_WJ_EX          = 0xC7,
    BONE_NODE_N_HARA_C_WJ_EX          = 0xC8,
    BONE_NODE_MAX                     = 0xC9,
};

enum CCmdYkType {
    CCMD_YK_TYPE_NON = 0,
    CCMD_YK_TYPE_R,
    CCMD_YK_TYPE_L,
};

enum Expr_type {
    Expr_constant = 0,
    Expr_variable,
    Expr_variable_rad,
    Expr_func1,
    Expr_func2,
    Expr_func3,
};

enum eyes_base_adjust_type {
    EYES_BASE_ADJUST_NONE = -1,

    EYES_BASE_ADJUST_DIRECTION = 0x00,
    EYES_BASE_ADJUST_CLEARANCE = 0x01,
    EYES_BASE_ADJUST_OFF       = 0x02,
    EYES_BASE_ADJUST_MAX       = 0x03,
};

enum FcurveKeyKind {
    FCURVE_KEY_KIND_STATIC_0 = 0,
    FCURVE_KEY_KIND_STATIC_DATA,
    FCURVE_KEY_KIND_HERMITE,
    FCURVE_KEY_KIND_HERMITE_TANGENT,
    FCURVE_KEY_KIND_MAX,
};

enum GuardKind {
    GUARD_KIND_NONE = 0,
    GUARD_KIND_TACHI,
    GUARD_KIND_SYAGAMI,
    GUARD_KIND_ALL,
};

enum HyoutanStat {
    HYOUTAN_STAT_NORMAL = 0,
    HYOUTAN_STAT_RIGHT_HAND,
    HYOUTAN_STAT_LEFT_HAND,
    HYOUTAN_STAT_MAX,
};

enum MhAirKind {
    AK_CENTER_MOVE = 0,
    AK_KIND_MAX,
};

enum MhAtkKind {
    ATK_DOWN_ATTACK_OK = 0,
    ATK_HIT_OK,
    ATK_GUARD,
    ATK_REVISE,
    ATK_YOROKENAI,
    ATK_YOKE_HIT,
    ATK_LR_YARARE,
    ATK_KIND_MAX,
};

enum MhEfcFlag {
    ETF_RIGHT = 0,
    ETF_LEFT,
    ETF_DOWN,
    ETF_TA_DOWN,
    ETF_COUNTER_DOWN,
    ETF_YOROKE_DOWN,
    ETF_NO_DOWN,
    ETF_NO_GUARD,
    ETF_GUARD_HALF,
    ETF_GUARD_HAZUSHI_LOW,
    ETF_GUARD_HAZUSHI_HIGH,
    ETF_FOLLOW_HIT_YOROKE,
    ETF_GUARD_YOROKE_HIGH,
    ETF_GUARD_YOROKE_LOW,
    ETF_YOROKE_HIGH,
    ETF_YOROKE_LOW,
    ETF_WALL_BREAK,
    ETF_COUNTER_YOROKE,
    ETF_YOROKE_NO_DOWN,
    ETF_URA_DOWN,
    ETF_COUNTER_URA_DOWN,
    ETF_FOLLOW_HIT_URA_DOWN,
    ETF_URA_NO_DOWN,
    ETF_FOLLOW_HIT_DOWN,
    ETF_S_COUNTER_DOWN,
    ETF_S_COUNTER_YOROKE,
    ETF_M_COUNTER_DOWN,
    ETF_M_COUNTER_YOROKE,
    ETF_L_COUNTER_DOWN,
    ETF_L_COUNTER_YOROKE,
    ETF_YARARE_REV,
    ETF_YOKE_GUARD,
    ETF_SIDE_DOWN,
    ETF_COUNTER_SIDE_DOWN,
    ETF_FOLLOW_SIDE_DOWN,
    ETF_STRAIGHT,
    ETF_REVERSE,
    ETF_WALL_FIX,
    ETF_NOT_WALL_FIX,
    ETF_SIDE_YOROKE,
    ETF_URA_YOROKE,
    ETF_SIDE_YOROKE_LOW,
    ETF_URA_YOROKE_LOW,
    ETF_ITAI_DOWN,
    ETF_NO_UKEMI,
    ETF_STUN,
    ETF_COUNTER_STUN,
    ETF_FOLLOW_HIT_STUN,
    ETF_URA_STUN,
    ETF_COUNTER_URA_STUN,
    ETF_FOLLOW_HIT_URA_STUN,
    ETF_SIDE_STUN,
    ETF_COUNTER_SIDE_STUN,
    ETF_FOLLOW_HIT_SIDE_STUN,
    ETF_STUN_DOWN,
    ETF_FOLLOW_HIT_SY_YA,
    ETF_NO_CMBXANG_OFFSET,
    ETF_BD_COUNTER_DOWN,
    ETF_BD_COUNTER_YOROKE,
    ETF_NO_COUNTER_DAMAGE,
    ETF_NO_HIT_AIR,
    ETF_FLAG_MAX,
};

enum MhEfcSpeedType {
    EST_NORMAL = 0,
    EST_EXTRA,
    EST_UP_PUNCH,
    EST_UP_PUNCH2,
    EST_DOWN_ATTACK,
    EST_NERICHAGI,
    EST_FUTTOBI,
    EST_LOW_ATTACK,
    EST_HAMMER,
    EST_JUMP_PUNCH,
    EST_CHAIN_PULL,
    EST_SPIN,
    EST_RENKAN,
    EST_NORMAL2,
    EST_MAX,
};

enum MhEnDrinkOffType {
    MH_EDRK_ATK_ALWAYS = 0,
    MH_EDRK_ATK_HIT,
    MH_EDRK_ATK_GUARD,
    MH_EDRK_DRINK_OFF_MAX,
};

enum MhGuardFlag {
    GK_BUTTON = 0,
    GK_NONE_DC,
    GK_NONE_DC_SELECT,
    GK_NONE_DC_ALL,
    GK_NONE_DC_TA,
    GK_NONE_DC_SY,
    GK_DC,
    GK_DC_SELECT,
    GK_DC_ALL,
    GK_DC_TA,
    GK_DC_SY,
    GK_COMA_A,
    GK_SIDE,
    GK_FLAG_MAX,
};

enum MhGuardType {
    GK_TYPE_ALL = 0,
    GK_TYPE_TA,
    GK_TYPE_SY,
    GK_TYPE_SEL,
    GK_TYPE_MAX,
};

enum MhKabeDmgType {
    MH_KBD_NORMAL = 0,
    MH_KBD_YARARE,
    MH_KBD_YOROKE,
    MH_KBD_MAX,
};

enum MhKaeshiAttrib {
    KT_ATTRIB_NONE = 0x0,
    KT_ATTRIB_TA_PUNCH,
    KT_ATTRIB_SY_PUNCH,
    KT_ATTRIB_HIJI,
    KT_ATTRIB_HIZA,
};

enum MhKaeshiFlag {
    KT_PUNCH_KAESHI = 0,
    KT_LOW_PUNCH_KAESHI,
    KT_KICK_KAESHI,
    KT_MID_KICK_KAESHI,
    KT_LOW_KICK_KAESHI,
    KT_HIJI_KAESHI,
    KT_HIZA_KAESHI,
    KT_SPIN_K_KAESHI,
    KT_LOW_SPIN_K_KAESHI,
    KT_SUMMER_KAESHI,
    KT_DOWN_PUNCH_KAESHI,
    KT_DOWN_KICK_KAESHI,
    KT_RYOUTE_KAESHI,
    KT_KATA_KAESHI,
    KT_TETSUZAN_KAESHI,
    KT_HEAD_KAESHI,
    KT_TE_TORI,
    KT_LOW_PUNCH_TORI,
    KT_ASI_TORI,
    KT_MID_ASI_TORI,
    KT_LOW_ASI_TORI,
    KT_HIJI_TORI,
    KT_HIZA_TORI,
    KT_RYOUASI_TORI,
    KT_HIGH,
    KT_MIDDLE,
    KT_LOW,
    KT_BEHIND,
    KT_TACHI_NAGE,
    KT_SY_GUARD,
    KT_GUARD_SHIFT,
    KT_LEFT,
    KT_FULL_KAESHI,
    KT_DOWN_DISABLE,
    KT_RESIST_DISABLE,
    KT_DISABLE,
    KT_TYPE_MAX,
};

enum MhMotKind {
    MK_CHANGE = 0,
    MK_BEGIN,
    MK_FOLLOW,
    MK_NO_TRANS,
    MK_NO_MOVE,
    MK_RUN,
    MK_AIR,
    MK_JUMP,
    MK_TURN,
    MK_YARARE,
    MK_DOWN,
    MK_DOWN_POSE,
    MK_KAMAE,
    MK_SYAGAMI,
    MK_ATTACK,
    MK_GUARD,
    MK_UKEMI,
    MK_RISE,
    MK_GACHA,
    MK_NAGE,
    MK_NAGERARE,
    MK_DTURN,
    MK_BACK,
    MK_YOROKE,
    MK_RIDE_ON,
    MK_YOKE,
    MK_AGURA,
    MK_SAKADACHI,
    MK_FALL,
    MK_TSUKAMI,
    MK_TSUKAMARE,
    MK_MUTEKI,
    MK_HAJIKI_HIGH,
    MK_HAJIKI_LOW,
    MK_NERI,
    MK_OIDASANAI,
    MK_CHARGE_WHILE,
    MK_NO_DEAD,
    MK_KAESERU,
    MK_DOWN_HIT_ENABLE,
    MK_AIR_DOWN,
    MK_TUNNELS,
    MK_TUNNEL0,
    MK_NAGERARERU,
    MK_POSE_CANCEL_OK,
    MK_NO_FREEZE,
    MK_GUARD_NAGERENAI_DC,
    MK_NAGE_CHECK_NORMAL,
    MK_NAGE_CHECK_KUZURE,
    MK_SABAKI,
    MK_GACHA_DISP,
    MK_KABE_HIT_NG,
    MK_WALL_CHECK_RINGOUT,
    MK_FALL_RINGOUT_OK,
    MK_NO_RINGOUT,
    MK_RINGOUT_CHAIN,
    MK_NAGE_WALL_BREAK,
    MK_RECALC_OIDASHI_VEC,
    MK_BACK_SPEED_OFF,
    MK_CUT_WALL_SPEED_OFF,
    MK_YOKE_R,
    MK_YOKE_L,
    MK_YOKERENAI,
    MK_DOWN_ATTACK_OK,
    MK_WALK,
    MK_SYAGAMI_WALK,
    MK_DASH,
    MK_SYAGAMI_DASH,
    MK_DOWN_NAGE,
    MK_MOVE_FRONT,
    MK_MOVE_BACK,
    MK_SABAKI_MOTION,
    MK_COMBO_NAGE,
    MK_COMBO_NAGERARE,
    MK_CHARGE_OFF,
    MK_CHARGE,
    MK_ATEMIKAMAE,
    MK_ATEMIWAZA,
    MK_NAGENUKE_OK,
    MK_NAGENUKE,
    MK_SYAGAMI_NAGE,
    MK_NAGE_TSUKAMI,
    MK_YOKE_ATTACK,
    MK_JYOUDAN_KAWASHI,
    MK_TOKUJYOU_KAWASHI,
    MK_NAGE_FOLLOW_NAGERERU,
    MK_RESIST,
    MK_ATTACK_FOLLOW,
    MK_ATTACK_BEGIN,
    MK_FORCE_COUNTER_STRONG,
    MK_FORCE_COUNTER_NORMAL,
    MK_Y_MOVE,
    MK_Y_TRANS,
    MK_DMIRROR,
    MK_URA_SAKA,
    MK_SPD0,
    MK_FUMI,
    MK_KAO_MUKE,
    MK_KAO_MUKENAI,
    MK_KAO_MUKERUNA,
    MK_WALL_THRUST_EMY,
    MK_NEXT_TRANS,
    MK_NEXT_Y_TRANS,
    MK_BOUND_DOWN,
    MK_HEAVY_BOUND_DOWN,
    MK_NO_BOUND_DOWN,
    MK_REVERSE_ATK,
    MK_PL_SYAGAMI,
    MK_NOT_PL_SYAGAMI,
    MK_LOW_KAWASHI,
    MK_REVERSE_HIT_OK,
    MK_WALL_REDUCE_DAMAGE,
    MK_WALL_DOWN2,
    MK_REVERSE_ATK_FRONT,
    MK_DOWN_LIKE,
    MK_REDUCE_DAMAGE,
    MK_IMM_YOKERARE_DISABLE,
    MK_OFF_MOVE_L,
    MK_OFF_MOVE_R,
    MK_NO_CMBXANG_OFFSET,
    MK_COLLI_WALL_DISABLE,
    MK_NO_AIR,
    MK_ITAI,
    MK_ASI_LEAF_CTRL,
    MK_CATCH_SABAKI_DISABLE,
    MK_STUN,
    MK_ATTACK_BEGIN2,
    MK_KIND_MAX,
};

enum MhNextType {
    MH_NEXT_NONE = -1,

    MH_NEXT_MOTION = 0,
    MH_NEXT_REPEAT,
    MH_NEXT_KAMAE,
    MH_NEXT_MAKE,
    MH_NEXT_NAGE,
    MH_NEXT_DOWN_POSE,
    MH_NEXT_LOCK,
    MH_NEXT_MAX,
};

enum MhResistDownChkType {
    RESIST_NO_CHK_DOWN = 0,
    RESIST_CHK_DOWN,
};

enum MhReviseFlag {
    MH_REVISE_NO = 0,
    MH_REVISE_FORCE,
    MH_REVISE_RISE_START,
    MH_REVISE_MOT_SLOW,
    MH_REVISE_DASH,
    MH_REVISE_EMY_NO,
    MH_REVISE_GUARD_EMY_NO,
    MH_REVISE_ATK_FOLLOW_EMY_NO,
    MH_REVISE_NORM_EMY_NO,
    MH_REVISE_WALK_EMY_NO,
    MH_REVISE_UPDATE_EMY_ATK,
    MH_REVISE_FORCE_SLOW2,
    MH_REVISE_FORCE_FAST2,
    MH_REVISE_GUARD_SLOW,
    MH_REVISE_GUARD_EMY_ON,
    MH_REVISE_DOWN_ATK,
    MH_REVISE_FORCE_SLOW,
    MH_REVISE_MAX,
};

enum MhSmoothFlag {
    SMF_SAME_FSMOOTH_OFF = 0,
    SMF_NORMAL_RSMOOTH,
    SMF_FLAG_MAX,
};

enum MhYarareType {
    E_PUNCH = 0,
    E_KICK,
    E_STR,
    E_MAWASHI,
    E_MID_PUNCH,
    E_MID_STR,
    E_MID_MAWASHI,
    E_SY_PUNCH,
    E_SY_KICK,
    E_SY_STR,
    E_SY_MAWASHI,
    E_YOKO,
    E_YOKO2,
    E_TACKLE,
    E_KUZURE1_COUNTER,
    E_KUZURE1,
    E_KUZURE2_COUNTER,
    E_KUZURE2,
    E_URATEN,
    E_UP_PUNCH,
    E_DOWN_ATTACK,
    E_DOWN_ATTACK2,
    E_DOWN_ATTACK3,
    E_JUMP_PUNCH,
    E_SYAGAMASE,
    E_MOUKO,
    E_MOUKO2,
    E_TRN_PUNCH,
    E_BETA,
    E_NIKIKYAKU,
    E_HAZUSHI,
    E_MID_SLEEPER,
    E_MDLK,
    E_DASH_HAMMER,
    E_BETA_TATAKI,
    E_KASANE,
    E_LOWATK,
    E_HEAVY_KICK,
    E_KICK3,
    E_PUNCH3,
    E_SYAGAMASE2,
    E_WOL_BODY,
    E_KETAGURI,
    E_HOZAN,
    E_METUKI,
    E_HEAVY,
    E_UP_PUNCH_EV,
    E_MDLK_EV,
    E_MDLK_EV2,
    E_HIZADOWN,
    E_DASH_HAMMER_EV,
    E_UP_PUNCH_EV2,
    E_KUZURE2_HEAVY,
    E_SIDE_ATTACK,
    E_HEAVY_KICK_FS,
    E_NIKIKYAKU_FAST,
    E_NIKIKYAKU_FAST2,
    E_HEAVY_BOUND,
    E_JUMP_PUNCH_BOUND,
    E_BETA_TATAKI_BOUND,
    E_KUZURE2_UPPER,
    E_HEAVY_BOUND2,
    E_UP_PUNCH_URATEN,
    E_UP_PUNCH_EV3,
    E_UP_PUNCH2,
    E_MOUKO2_KUZURE,
    E_MID_STR_KUZURE,
    E_MID_PUNCH_KUZURE,
    E_PUNCH3_KUZURE,
    E_MDLK_KUZURE,
    E_MDLK_EV_KUZURE,
    E_KUZURE1_KUZURE,
    E_WOL_BODY_KUZURE,
    E_KICK3_KUZURE,
    E_GOROGORO,
    E_UEFUTTOBI,
    E_ITAI,
    E_KUZURE2_LR,
    E_HVYGD_UP_PUNCH,
    E_SY_STR_BECHA,
    E_MOUKO_SHORT,
    E_MAKE_90DEG,
    E_MOUKO_FUTTOBI,
    E_MOUKO_FUTTOBI_KUZURE,
    E_MOUKO_FUTTOBI2,
    E_UP_PUNCH_UPPER,
    E_UP_PUNCH_YORODOWN,
    E_SY_MAWASHI_MAKE_90DEG,
    E_LIGHT_BOUND,
    E_MOUKO_KUZURE,
    E_BETA_LONG,
    E_HEAVY_KICK_EV,
    E_HIZADOWN_AIR,
    E_SYAGAMASE_BECHA,
    E_MDLK_EV_KUZURE2,
    E_MAWASHI_ROT_S,
    E_MAWASHI_ROT_L,
    E_JKD,
    E_MDLK_HARD,
    E_BECHA_BOUND,
    E_LEG_HARD,
    E_FS_TEST_1,
    E_HIJI,
    E_PUSH,
    E_FUMARE,
    E_HEAVY_FUMARE,
    E_ASHIBARAI,
    E_FS_TEST_7,
    E_MAX,
};

enum MOT_BSTEP {
    BSTEP_BEGIN = 0,
    BSTEP_MAIN,
    BSTEP_FOLLOW1,
    BSTEP_FOLLOW2,
    BSTEP_MAX,
};

enum MotAdjustType {
    MA_TYPE_NONE = 0,
    MA_TYPE_COMMON,
    MA_TYPE_SCALE,
    MA_TYPE_BODY,
    MA_TYPE_ARM,
    MA_TYPE_HEIGHT,
    MA_TYPE_EMY_SCALE,
    MA_TYPE_EMY_BODY,
    MA_TYPE_EMY_ARM,
    MA_TYPE_EMY_HEIGHT,
    MA_TYPE_DIRECT,
};

enum MotLeafCtrlCharaFlag {
    LC_CHARA_NONE = -1,

    LC_CHARA_OK = 0,
    LC_CHARA_NG,
    LC_CHARA_MAX,
};

enum MotLeafCtrlLimit {
    LCLIMIT_XYZ = 0,
    LCLIMIT_X,
    LCLIMIT_Y,
    LCLIMIT_Z,
    LCLIMIT_XY,
    LCLIMIT_XZ,
    LCLIMIT_YZ,
    LCLIMIT_MAX,
};

enum MotLeafCtrlMode {
    LEAF_CTRL_NONE = -1,

    LEAF_CTRL_ON = 0,
    LEAF_CTRL_OFF,
    LEAF_CTRL_MAX,
};

enum MotLeafCtrlPart {
    LCPART_TE_R = 0,
    LCPART_TE_L,
    LCPART_ASI_R,
    LCPART_ASI_L,
    LCPART_MAX,
};

enum MotLeafCtrlTagId {
    LCTAG_ABS = 0,
    LCTAG_TE_R,
    LCTAG_TE_L,
    LCTAG_ASI_R,
    LCTAG_ASI_L,
    LCTAG_SUNE_R,
    LCTAG_SUNE_L,
    LCTAG_UDE_R,
    LCTAG_UDE_L,
    LCTAG_KAO,
    LCTAG_ADJ_EMY,
    LCTAG_ADJ_EMY_BODY,
    LCTAG_ADJ_EMY_HEIGHT,
    LCTAG_ADJ_REV,
    LCTAG_ADJ_BODY_REV,
    LCTAG_ADJ_HEIGHT_REV,
    LCTAG_OFS,
    LCTAG_OFS_TE_R,
    LCTAG_OFS_TE_L,
    LCTAG_OFS_ASI_R,
    LCTAG_OFS_ASI_L,
    LCTAG_OFS_SUNE_R,
    LCTAG_OFS_SUNE_L,
    LCTAG_OFS_UDE_R,
    LCTAG_OFS_UDE_L,
    LCTAG_OFS_KAO,
    LCTAG_OFS_BODY,
    LCTAG_MAX,
};

enum mot_play_frame_data_loop_state : uint32_t {
    MOT_PLAY_FRAME_DATA_LOOP_NONE       = (uint32_t)-1,
    MOT_PLAY_FRAME_DATA_LOOP_ONCE       = 0x00,
    MOT_PLAY_FRAME_DATA_LOOP_CONTINUOUS = 0x01,
    MOT_PLAY_FRAME_DATA_LOOP_RESET      = 0x02,
    MOT_PLAY_FRAME_DATA_LOOP_REVERSE    = 0x03,
    MOT_PLAY_FRAME_DATA_LOOP_MAX        = 0x04,
};

enum mot_play_frame_data_playback_state : uint32_t {
    MOT_PLAY_FRAME_DATA_PLAYBACK_NONE     = (uint32_t)-1,
    MOT_PLAY_FRAME_DATA_PLAYBACK_STOP     = 0x00,
    MOT_PLAY_FRAME_DATA_PLAYBACK_FORWARD  = 0x01,
    MOT_PLAY_FRAME_DATA_PLAYBACK_BACKWARD = 0x02,
    MOT_PLAY_FRAME_DATA_PLAYBACK_EXTERNAL = 0x03,
    MOT_PLAY_FRAME_DATA_PLAYBACK_MAX      = 0x04,
};

enum MhdNumber {
    MHD_NUM_0 = 0,
    MHD_NUM_1,
    MHD_NUM_2,
    MHD_NUM_3,
    MHD_NUM_4,
    MHD_NUM_5,
    MHD_NUM_6,
    MHD_NUM_7,
    MHD_NUM_8,
    MHD_NUM_9,
    MHD_NUM_10,
    MHD_NUM_11,
    MHD_NUM_12,
    MHD_NUM_13,
    MHD_NUM_14,
    MHD_NUM_15,
    MHD_NUM_16,
    MHD_NUM_17,
    MHD_NUM_18,
    MHD_NUM_19,
    MHD_NUM_20,
    MHD_NUM_21,
    MHD_NUM_22,
    MHD_NUM_23,
    MHD_NUM_24,
    MHD_NUM_25,
    MHD_NUM_26,
    MHD_NUM_27,
    MHD_NUM_28,
    MHD_NUM_29,
    MHD_NUM_30,
    MHD_NUM_31,
    MHD_NUM_32,
    MHD_NUM_33,
    MHD_NUM_34,
    MHD_NUM_35,
    MHD_NUM_36,
    MHD_NUM_37,
    MHD_NUM_38,
    MHD_NUM_39,
    MHD_NUM_40,
    MHD_SHIFT,
    MHD_NUM_42,
    MHD_NUM_43,
    MHD_NUM_44,
    MHD_NUM_45,
    MHD_NUM_46,
    MHD_NUM_47,
    MHD_NUM_48,
    MHD_NUM_49,
    MHD_NUM_50,
    MHD_NUM_51,
    MHD_NUM_52,
    MHD_NUM_53,
    MHD_NUM_54,
    MHD_NUM_55,
    MHD_NUM_56,
    MHD_NUM_57,
    MHD_NUM_58,
    MHD_NUM_59,
    MHD_NUM_60,
    MHD_NUM_61,
    MHD_NUM_62,
    MHD_NUM_63,
    MHD_NUM_64,
    MHD_NUM_65,
    MHD_NUM_66,
    MHD_NUM_67,
    MHD_NUM_68,
    MHD_MAX,
};

enum MhpNumber {
    MHP_NUM_0 = 0,
    MHP_NUM_1,
    MHP_NUM_2,
    MHP_NUM_3,
    MHP_NUM_4,
    MHP_NUM_5,
    MHP_NUM_6,
    MHP_NUM_7,
    MHP_NUM_8,
    MHP_NUM_9,
    MHP_NUM_10,
    MHP_NUM_11,
    MHP_NUM_12,
    MHP_NUM_13,
    MHP_NUM_14,
    MHP_NUM_15,
    MHP_NUM_16,
    MHP_NUM_17,
    MHP_NUM_18,
    MHP_NUM_19,
    MHP_NUM_20,
    MHP_NUM_21,
    MHP_NUM_22,
    MHP_NUM_23,
    MHP_NUM_24,
    MHP_NUM_25,
    MHP_NUM_26,
    MHP_NUM_27,
    MHP_NUM_28,
    MHP_NUM_29,
    MHP_NUM_30,
    MHP_NUM_31,
    MHP_NUM_32,
    MHP_NUM_33,
    MHP_NUM_34,
    MHP_NUM_35,
    MHP_NUM_36,
    MHP_NUM_37,
    MHP_NUM_38,
    MHP_NUM_39,
    MHP_NUM_40,
    MHP_NUM_41,
    MHP_NUM_42,
    MHP_NUM_43,
    MHP_NUM_44,
    MHP_NUM_45,
    MHP_NUM_46,
    MHP_NUM_47,
    MHP_NUM_48,
    MHP_NUM_49,
    MHP_SET_FACE_MOTION_ID,
    MHP_NUM_51,
    MHP_NUM_52,
    MHP_SET_FACE_MOTTBL_MOTION,
    MHP_SET_HAND_R_MOTTBL_MOTION,
    MHP_SET_HAND_L_MOTTBL_MOTION,
    MHP_SET_MOUTH_MOTTBL_MOTION,
    MHP_SET_EYES_MOTTBL_MOTION,
    MHP_SET_EYELID_MOTTBL_MOTION,
    MHP_SET_ROB_CHARA_HEAD_OBJECT,
    MHP_SET_LOOK_CAMERA,
    MHP_SET_EYELID_MOTION_FROM_FACE,
    MHP_ROB_PARTS_ADJUST,
    MHP_NUM_63,
    MHP_OSAGE_RESET,
    MHP_MOTION_SKIN_PARAM,
    MHP_OSAGE_STEP,
    MHP_SLEEVE_ADJUST,
    MHP_NUM_68,
    MHP_MOTION_MAX_FRAME,
    MHP_CAMERA_MAX_FRAME,
    MHP_OSAGE_MOVE_CANCEL,
    MHP_NUM_72,
    MHP_ROB_HAND_ADJUST,
    MHP_DISABLE_COLLISION,
    MHP_ROB_ADJUST_GLOBAL,
    MHP_ROB_ARM_ADJUST,
    MHP_DISABLE_EYE_MOTION,
    MHP_NUM_78,
    MHP_ROB_CHARA_COLI_RING,
    MHP_ADJUST_GET_GLOBAL_POS,
    MHP_MAX,
};

enum MotionBlendType {
    MOTION_BLEND_NONE = -1,

    MOTION_BLEND         = 0x00,
    MOTION_BLEND_FREEZE  = 0x01,
    MOTION_BLEND_CROSS   = 0x02,
    MOTION_BLEND_COMBINE = 0x03,
};

// Pure assumption
enum MOTTABLE_TYPE {
    MTP_NONE = 0,
    MTP_1,
    MTP_2,
    MTP_3,
    MTP_4,
    MTP_5,
    MTP_FACE_NULL,
    MTP_FACE_RESET,
    MTP_FACE_RESET_CL,
    MTP_FACE_RESET_OLD,
    MTP_FACE_RESET_OLD_CL,
    MTP_FACE_SAD,
    MTP_FACE_SAD_CL,
    MTP_FACE_SAD_OLD,
    MTP_FACE_SAD_OLD_CL,
    MTP_FACE_LAUGH,
    MTP_FACE_LAUGH_CL,
    MTP_FACE_LAUGH_OLD,
    MTP_FACE_LAUGH_OLD_CL,
    MTP_FACE_SURPRISE,
    MTP_FACE_SURPRISE_CL,
    MTP_FACE_SURPRISE_OLD,
    MTP_FACE_SURPRISE_OLD_CL,
    MTP_FACE_WINK_OLD,
    MTP_FACE_WINK_OLD_CL,
    MTP_FACE_ADMIRATION,
    MTP_FACE_ADMIRATION_CL,
    MTP_FACE_ADMIRATION_OLD,
    MTP_FACE_ADMIRATION_OLD_CL,
    MTP_FACE_SMILE,
    MTP_FACE_SMILE_CL,
    MTP_FACE_SMILE_OLD,
    MTP_FACE_SMILE_OLD_CL,
    MTP_FACE_SETTLED,
    MTP_FACE_SETTLED_CL,
    MTP_FACE_SETTLED_OLD,
    MTP_FACE_SETTLED_OLD_CL,
    MTP_FACE_DAZZLING,
    MTP_FACE_DAZZLING_CL,
    MTP_FACE_DAZZLING_OLD,
    MTP_FACE_DAZZLING_OLD_CL,
    MTP_FACE_LASCIVIOUS,
    MTP_FACE_LASCIVIOUS_CL,
    MTP_FACE_LASCIVIOUS_OLD,
    MTP_FACE_LASCIVIOUS_OLD_CL,
    MTP_FACE_STRONG,
    MTP_FACE_STRONG_CL,
    MTP_FACE_STRONG_OLD,
    MTP_FACE_STRONG_OLD_CL,
    MTP_FACE_CLARIFYING,
    MTP_FACE_CLARIFYING_CL,
    MTP_FACE_CLARIFYING_OLD,
    MTP_FACE_CLARIFYING_OLD_CL,
    MTP_FACE_GENTLE,
    MTP_FACE_GENTLE_CL,
    MTP_FACE_GENTLE_OLD,
    MTP_FACE_GENTLE_OLD_CL,
    MTP_FACE_CRY,
    MTP_FACE_CRY_CL,
    MTP_FACE_CRY_OLD,
    MTP_FACE_CRY_OLD_CL,
    MTP_FACE_CLOSE,
    MTP_FACE_CLOSE_CL,
    MTP_FACE_CLOSE_OLD,
    MTP_FACE_CLOSE_OLD_CL,
    MTP_FACE_NAGASI,
    MTP_FACE_NAGASI_CL,
    MTP_FACE_NAGASI_OLD,
    MTP_FACE_NAGASI_OLD_CL,
    MTP_FACE_KIRI,
    MTP_FACE_KIRI_CL,
    MTP_FACE_KIRI_OLD,
    MTP_FACE_KIRI_OLD_CL,
    MTP_FACE_UTURO,
    MTP_FACE_UTURO_CL,
    MTP_FACE_UTURO_OLD,
    MTP_FACE_UTURO_OLD_CL,
    MTP_FACE_OMOU,
    MTP_FACE_OMOU_CL,
    MTP_FACE_OMOU_OLD,
    MTP_FACE_OMOU_OLD_CL,
    MTP_FACE_SETUNA,
    MTP_FACE_SETUNA_CL,
    MTP_FACE_SETUNA_OLD,
    MTP_FACE_SETUNA_OLD_CL,
    MTP_FACE_GENKI,
    MTP_FACE_GENKI_CL,
    MTP_FACE_GENKI_OLD,
    MTP_FACE_GENKI_OLD_CL,
    MTP_FACE_YARU,
    MTP_FACE_YARU_CL,
    MTP_FACE_YARU_OLD,
    MTP_FACE_YARU_OLD_CL,
    MTP_FACE_COOL,
    MTP_FACE_COOL_CL,
    MTP_FACE_KOMARIWARAI,
    MTP_FACE_KOMARIWARAI_CL,
    MTP_FACE_KUMON,
    MTP_FACE_KUMON_CL,
    MTP_FACE_KUTSUU,
    MTP_FACE_KUTSUU_CL,
    MTP_FACE_NAKI,
    MTP_FACE_NAKI_CL,
    MTP_FACE_NAYAMI,
    MTP_FACE_NAYAMI_CL,
    MTP_FACE_SUPSERIOUS,
    MTP_FACE_SUPSERIOUS_CL,
    MTP_FACE_TSUYOKIWARAI,
    MTP_FACE_TSUYOKIWARAI_CL,
    MTP_FACE_WINK_L,
    MTP_FACE_WINK_L_CL,
    MTP_FACE_WINK_R,
    MTP_FACE_WINK_R_CL,
    MTP_FACE_WINKG_L,
    MTP_FACE_WINKG_L_CL,
    MTP_FACE_WINKG_R,
    MTP_FACE_WINKG_R_CL,
    MTP_FACE_RESET1,
    MTP_FACE_RESET1_CL,
    MTP_FACE_RESET2,
    MTP_FACE_RESET2_CL,
    MTP_FACE_RESET3,
    MTP_FACE_RESET3_CL,
    MTP_FACE_RESET4,
    MTP_FACE_RESET4_CL,
    MTP_FACE_RESET5,
    MTP_FACE_RESET5_CL,
    MTP_FACE_WINK_FT_OLD,
    MTP_FACE_WINK_FT_OLD_CL,
    MTP_FACE_NEW_IKARI_OLD,
    MTP_FACE_NEW_IKARI_OLD_CL,
    MTP_KUCHI_NULL,
    MTP_KUCHI_RESET,
    MTP_KUCHI_RESET_OLD,
    MTP_KUCHI_A,
    MTP_KUCHI_A_OLD,
    MTP_KUCHI_I,
    MTP_KUCHI_I_OLD,
    MTP_KUCHI_U,
    MTP_KUCHI_U_OLD,
    MTP_KUCHI_E,
    MTP_KUCHI_E_OLD,
    MTP_KUCHI_O,
    MTP_KUCHI_O_OLD,
    MTP_KUCHI_HE,
    MTP_KUCHI_HE_OLD,
    MTP_KUCHI_SURPRISE,
    MTP_KUCHI_SURPRISE_OLD,
    MTP_KUCHI_SMILE,
    MTP_KUCHI_SMILE_OLD,
    MTP_KUCHI_NIYA,
    MTP_KUCHI_NIYA_OLD,
    MTP_KUCHI_CHU,
    MTP_KUCHI_CHU_OLD,
    MTP_KUCHI_E_DOWN,
    MTP_KUCHI_HAMISE,
    MTP_KUCHI_HAMISE_DOWN,
    MTP_KUCHI_HE_S,
    MTP_KUCHI_HERAHERA,
    MTP_KUCHI_MOGUMOGU,
    MTP_KUCHI_NEKO,
    MTP_KUCHI_SAKEBI,
    MTP_KUCHI_SAKEBI_L,
    MTP_KUCHI_SMILE_L,
    MTP_KUCHI_NEUTRAL,
    MTP_EYES_NULL,
    MTP_EYES_RESET,
    MTP_EYES_RESET_OLD,
    MTP_EYES_UP,
    MTP_EYES_UP_OLD,
    MTP_EYES_DOWN,
    MTP_EYES_DOWN_OLD,
    MTP_EYES_LEFT,
    MTP_EYES_LEFT_OLD,
    MTP_EYES_RIGHT,
    MTP_EYES_RIGHT_OLD,
    MTP_EYES_UP_LEFT,
    MTP_EYES_UP_LEFT_OLD,
    MTP_EYES_UP_RIGHT,
    MTP_EYES_UP_RIGHT_OLD,
    MTP_EYES_DOWN_LEFT,
    MTP_EYES_DOWN_LEFT_OLD,
    MTP_EYES_DOWN_RIGHT,
    MTP_EYES_DOWN_RIGHT_OLD,
    MTP_EYES_MOVE_U_D,
    MTP_EYES_MOVE_U_D_OLD,
    MTP_EYES_MOVE_L_R,
    MTP_EYES_MOVE_L_R_OLD,
    MTP_EYES_MOVE_UL_DR,
    MTP_EYES_MOVE_UL_DR_OLD,
    MTP_EYES_MOVE_UR_DL,
    MTP_EYES_MOVE_UR_DL_OLD,
    MTP_HAND_NULL,
    MTP_HAND_RESET,
    MTP_HAND_NORMAL,
    MTP_HAND_OPEN,
    MTP_HAND_CLOSE,
    MTP_HAND_PEACE,
    MTP_HAND_GOOD,
    MTP_HAND_ONE,
    MTP_HAND_THREE,
    MTP_HAND_NEGI,
    MTP_HAND_SIZEN,
    MTP_HAND_PICK,
    MTP_HAND_MIC,
    MTP_HAND_FAN,
    MTP_HAND_BOTTLE,
    MTP_HAND_PHONE,
    MTP_HAND_HOLD,
    MTP_HAND_FLASHLIGHT,
    MTP_HAND_MIC_BLK,
    MTP_HAND_MIC_SLV,
    MTP_HAND_CUPICE,
    MTP_HAND_ICEBAR,
    MTP_FACE_MOT_SLOT_1,
    MTP_FACE_MOT_SLOT_2,
    MTP_FACE_MOT_SLOT_3,
    MTP_FACE_MOT_SLOT_4,
    MTP_FACE_MOT_SLOT_5,
    MTP_FACE_MOT_SLOT_6,
    MTP_FACE_MOT_SLOT_7,
    MTP_FACE_MOT_SLOT_8,
    MTP_FACE_MOT_SLOT_9,
    MTP_FACE_MOT_SLOT_10,
    MTP_EYES_BASE_MOT,
    MTP_225,
    MTP_226,
    MTP_227,
    MTP_228,
    MTP_229,
    MTP_230,
    MTP_231,
    MTP_232,
    MTP_233,
    MTP_234,
    MTP_235,
    MTP_FACE_EYEBROW_UP_RIGHT,
    MTP_FACE_EYEBROW_UP_RIGHT_CL,
    MTP_FACE_EYEBROW_UP_LEFT,
    MTP_FACE_EYEBROW_UP_LEFT_CL,
    MTP_FACE_KOMARIEGAO,
    MTP_FACE_KOMARIEGAO_CL,
    MTP_FACE_KONWAKU,
    MTP_FACE_KONWAKU_CL,
    MTP_KUCHI_PSP_A,
    MTP_KUCHI_PSP_E,
    MTP_KUCHI_PSP_O,
    MTP_KUCHI_PSP_SURPRISE,
    MTP_KUCHI_PSP_NIYA,
    MTP_KUCHI_PSP_NIYARI,
    MTP_KUCHI_HAMISE_E,
    MTP_KUCHI_SANKAKU,
    MTP_KUCHI_SHIKAKU,
    MTP_MAX,
};

enum ReviseType {
    REVISE_NORMAL = 0x0,
    REVISE_FAST,
    REVISE_SLOW,
    REVISE_GUARD,
    REVISE_RISE,
    REVISE_YARARE,
    REVISE_STRISE,
    REVISE_SLOW2,
    REVISE_ATTACK_FOLLOW,
    REVISE_SLOW_GUARD,
    REVISE_GUARD2,
    REVISE_RISE_SLOW,
    REVISE_TYPE_MAX,
};

// Pure assumption
enum ROB_COLLI_ID {
    ROB_COLLI_ID_DUMMY = -1,

    ROB_COLLI_ID_KOSHI = 0,
    ROB_COLLI_ID_MUNE_L,
    ROB_COLLI_ID_MUNE_R,
    ROB_COLLI_ID_KUBI,
    ROB_COLLI_ID_KAO,
    ROB_COLLI_ID_KATA_R1,
    ROB_COLLI_ID_KATA_R2,
    ROB_COLLI_ID_UDE_R1,
    ROB_COLLI_ID_UDE_R2,
    ROB_COLLI_ID_TE_R,
    ROB_COLLI_ID_KATA_L1,
    ROB_COLLI_ID_KATA_L2,
    ROB_COLLI_ID_UDE_L1,
    ROB_COLLI_ID_UDE_L2,
    ROB_COLLI_ID_TE_L,
    ROB_COLLI_ID_MOMO_R1,
    ROB_COLLI_ID_MOMO_R2,
    ROB_COLLI_ID_SUNE_R1,
    ROB_COLLI_ID_SUNE_R2,
    ROB_COLLI_ID_ASI_R,
    ROB_COLLI_ID_TOE_R,
    ROB_COLLI_ID_MOMO_L1,
    ROB_COLLI_ID_MOMO_L2,
    ROB_COLLI_ID_SUNE_L1,
    ROB_COLLI_ID_SUNE_L2,
    ROB_COLLI_ID_ASI_L,
    ROB_COLLI_ID_TOE_L,
    ROB_COLLI_ID_MAX,
};

enum ROB_ID {
    ROB_ID_1P = 0,
    ROB_ID_2P,
    ROB_ID_3P,
    ROB_ID_4P,
    ROB_ID_5P,
    ROB_ID_6P,
    ROB_ID_MAX,

    ROB_ID_NULL = -1,
};

enum rob_chara_data_hand_adjust_type : uint16_t {
    ROB_CHARA_DATA_HAND_ADJUST_NONE           = (uint16_t)-1,
    ROB_CHARA_DATA_HAND_ADJUST_NORMAL         = 0x00,
    ROB_CHARA_DATA_HAND_ADJUST_SHORT          = 0x01,
    ROB_CHARA_DATA_HAND_ADJUST_TALL           = 0x02,
    ROB_CHARA_DATA_HAND_ADJUST_MIN            = 0x03,
    ROB_CHARA_DATA_HAND_ADJUST_MAX            = 0x04,
    ROB_CHARA_DATA_HAND_ADJUST_OPPOSITE_CHARA = 0x05,
    ROB_CHARA_DATA_HAND_ADJUST_CUSTOM         = 0x06,
    ROB_CHARA_DATA_HAND_ADJUST_1P             = 0x07,
    ROB_CHARA_DATA_HAND_ADJUST_2P             = 0x08,
    ROB_CHARA_DATA_HAND_ADJUST_3P             = 0x09,
    ROB_CHARA_DATA_HAND_ADJUST_4P             = 0x0A,
    ROB_CHARA_DATA_HAND_ADJUST_TYPE_11        = 0x0B, // X
    ROB_CHARA_DATA_HAND_ADJUST_TYPE_12        = 0x0C,
    ROB_CHARA_DATA_HAND_ADJUST_TYPE_13        = 0x0D,
    ROB_CHARA_DATA_HAND_ADJUST_TYPE_14        = 0x0E,
    ROB_CHARA_DATA_HAND_ADJUST_ITEM           = 0x0F,
};

enum rob_osage_parts {
    ROB_OSAGE_PARTS_NONE = -1,

    ROB_OSAGE_PARTS_LEFT        = 0x00,
    ROB_OSAGE_PARTS_RIGHT       = 0x01,
    ROB_OSAGE_PARTS_CENTER      = 0x02,
    ROB_OSAGE_PARTS_LONG_C      = 0x03,
    ROB_OSAGE_PARTS_SHORT_L     = 0x04,
    ROB_OSAGE_PARTS_SHORT_R     = 0x05,
    ROB_OSAGE_PARTS_APPEND_L    = 0x06,
    ROB_OSAGE_PARTS_APPEND_R    = 0x07,
    ROB_OSAGE_PARTS_MUFFLER     = 0x08,
    ROB_OSAGE_PARTS_WHITE_ONE_L = 0x09,
    ROB_OSAGE_PARTS_PONY        = 0x0A,
    ROB_OSAGE_PARTS_ANGEL_L     = 0x0B,
    ROB_OSAGE_PARTS_ANGEL_R     = 0x0C,
    ROB_OSAGE_PARTS_MAX         = 0x0D,
};

enum rob_osage_parts_bit {
    ROB_OSAGE_PARTS_LEFT_BIT        = 0x0001,
    ROB_OSAGE_PARTS_RIGHT_BIT       = 0x0002,
    ROB_OSAGE_PARTS_CENTER_BIT      = 0x0004,
    ROB_OSAGE_PARTS_LONG_C_BIT      = 0x0008,
    ROB_OSAGE_PARTS_SHORT_L_BIT     = 0x0010,
    ROB_OSAGE_PARTS_SHORT_R_BIT     = 0x0020,
    ROB_OSAGE_PARTS_APPEND_L_BIT    = 0x0040,
    ROB_OSAGE_PARTS_APPEND_R_BIT    = 0x0080,
    ROB_OSAGE_PARTS_MUFFLER_BIT     = 0x0100,
    ROB_OSAGE_PARTS_WHITE_ONE_L_BIT = 0x0200,
    ROB_OSAGE_PARTS_PONY_BIT        = 0x0400,
    ROB_OSAGE_PARTS_ANGEL_L_BIT     = 0x0800,
    ROB_OSAGE_PARTS_ANGEL_R_BIT     = 0x1000,
};

enum rob_partial_motion_loop_state : uint32_t {
    ROB_PARTIAL_MOTION_LOOP_NONE       = (uint32_t)-1,
    ROB_PARTIAL_MOTION_LOOP_ONCE       = 0x00,
    ROB_PARTIAL_MOTION_LOOP_CONTINUOUS = 0x01,
    ROB_PARTIAL_MOTION_LOOP_RESET      = 0x02,
    ROB_PARTIAL_MOTION_LOOP_REVERSE    = 0x03,
    ROB_PARTIAL_MOTION_LOOP_MAX        = 0x04,
};

enum rob_partial_motion_playback_state : uint32_t {
    ROB_PARTIAL_MOTION_PLAYBACK_NONE         = (uint32_t)-1,
    ROB_PARTIAL_MOTION_PLAYBACK_STOP         = 0x00,
    ROB_PARTIAL_MOTION_PLAYBACK_CHARA_MOTION = 0x01,
    ROB_PARTIAL_MOTION_PLAYBACK_FORWARD      = 0x02,
    ROB_PARTIAL_MOTION_PLAYBACK_DURATION     = 0x03,
    ROB_PARTIAL_MOTION_PLAYBACK_BACKWARD     = 0x04,
    ROB_PARTIAL_MOTION_PLAYBACK_MAX          = 0x05,
};

enum RobType {
    ROB_TYPE_NULL = -1,

    ROB_TYPE_PLAYER = 0x00,
    ROB_TYPE_CPU_ENEMY = 0x01,
    ROB_TYPE_AUTH = 0x02,
    ROB_TYPE_DATA_TEST = 0x03,
    ROB_TYPE_MAX = 0x04,
};

enum TargetType {
    ROB_TARGET_NONE = 0,
    ROB_TARGET_WALL,
    ROB_TARGET_ENEMY,
    ROB_TARGET_RINGOUT,
};

enum ExNodeType {
    EX_NODE_TYPE_NULL       = 0x00,
    EX_NODE_TYPE_OSAGE      = 0x01,
    EX_NODE_TYPE_EXPRESSION = 0x02,
    EX_NODE_TYPE_CONSTRAINT = 0x03,
    EX_NODE_TYPE_CLOTH      = 0x04,
    EX_NODE_TYPE_MAX        = 0x05,
};

enum SubActExecType {
    SUB_ACTION_EXECUTE_NONE        = 0x00,
    SUB_ACTION_EXECUTE_CRY         = 0x01,
    SUB_ACTION_EXECUTE_SHAKE_HAND  = 0x02,
    SUB_ACTION_EXECUTE_EMBARRASSED = 0x03,
    SUB_ACTION_EXECUTE_ANGRY       = 0x04,
    SUB_ACTION_EXECUTE_LAUGH       = 0x05,
    SUB_ACTION_EXECUTE_COUNT_NUM   = 0x06,
};

enum SubActParamType {
    SUB_ACTION_PARAM_NONE        = 0x00,
    SUB_ACTION_PARAM_CRY         = 0x01,
    SUB_ACTION_PARAM_SHAKE_HAND  = 0x02,
    SUB_ACTION_PARAM_EMBARRASSED = 0x03,
    SUB_ACTION_PARAM_ANGRY       = 0x04,
    SUB_ACTION_PARAM_LAUGH       = 0x05,
    SUB_ACTION_PARAM_COUNT_NUM   = 0x06,
};

namespace SkinParam {
    enum CollisionType {
        CollisionTypeEnd     = 0x00,
        CollisionTypeBall    = 0x01,
        CollisionTypeCapsule = 0x02,
        CollisionTypePlane   = 0x03,
        CollisionTypeEllipse = 0x04,
        CollisionTypeAABB    = 0x05,
        CollisionTypeMax     = 0x06,
    };

    enum RootCollisionType {
        RootCollisionTypeEnd             = 0x00,
        RootCollisionTypeCapsule         = 0x01,
        RootCollisionTypeCapsuleWithRoot = 0x02,
        RootCollisionTypeMax             = 0x03,
    };
}

class rob_chara;
class rob_chara_bone_data;

struct RobTransform {
    vec3 pos;
    vec3 rot;
    vec3 scale;
    vec3 hsc;

    RobTransform();

    void CalcMatrixHS(const vec3& hsc, mat4& mat, mat4& dsp_mat);

    void init();
    void init(const vec3& p, const vec3& r);
    void init(float_t px, float_t py, float_t pz,
        float_t rx, float_t ry, float_t rz);
    void reset_scale();
};

struct RobNode {
    const char* name;
    mat4* mat_ptr;
    const RobNode* parent;
    RobTransform transform;
    mat4* no_scale_mat;

    RobNode();

    float_t* get_transform_component(size_t index, Expr_type& type);
    const float_t* get_transform_component(size_t index, Expr_type& type) const;
    void init(const char* in_name, mat4* in_mat, mat4* in_no_scale_mat);
};

struct struc_314 {
    uint32_t* field_0;
    size_t field_8;
};

struct FcurveKey {
    FcurveKeyKind kind;
    int32_t sum;
    int32_t cache_idx;
    int32_t last_idx;
    const uint16_t* num;
    const float_t* val;
};

struct Fcurve {
    uint16_t fc_max;
    uint16_t frame_max;
    uint16_t key_num_type;
    FcurveKey* fck_ptr;

    bool fcurve_init(const void* data);
    void fcurve_init_u16(const void* data);
    void interpolate(float_t frame, float_t* value,
        FcurveKey* fck, uint32_t in_fc_max, const struct struc_369* a6);
    void set_fck_ptr(FcurveKey* ptr);
};

struct struc_369 {
    int32_t field_0;
    float_t field_4;
};

struct mot_key_data {
    bool key_sets_ready;
    size_t key_set_count;
    prj::sys_vector<FcurveKey> key_set;
    Fcurve mot;
    prj::sys_vector<float_t> fc_value;
    const mot_data* mot_data;
    BONE_KIND kind;
    int32_t motion_body_type;
    uint32_t motnum;
    float_t frame;
    struc_369 field_68;

    mot_key_data();
    ~mot_key_data();

    void interpolate(float_t frame, uint32_t key_set_offset, uint32_t key_set_count);
    void reset();
};

struct RobBlock {
    IK_TYPE ik_type;
    IH_TYPE inherit_type;
    BONE_BLK block_id;
    BONE_BLK flip_block_id;
    BONE_BLK inherit_mat_id;
    uint32_t expression_id;
    int32_t key_set_offset;
    int32_t key_set_count;
    float_t frame;
    vec3 chain_pos[2];
    vec3 chain_ang;
    vec3 leaf_pos[2];
    mat4 chain_rot[3];
    vec3 smooth_pos[2];
    mat4 smooth_rot[3][2];
    const mat4* up_vector_mat_ptr;
    const mat4* inherit_mat_ptr;
    RobNode* node;
    float_t len[2][2];
    float_t arm_length;
    float_t eyes_xrot_adjust_neg;
    float_t eyes_xrot_adjust_pos;

    RobBlock();
    ~RobBlock();

    bool calc_constraint(const RobBlock* block_top);
    void copy_rot_trans(const RobBlock& other);
    bool check_expression_id_not_null();
    void exp_set_dir(const vec3& glo);
    void exp_set_dir_zx(vec3 vy);
    void exp_set_rot(const vec3& glo, float_t keisuu);
    bool get_ex_rotation(RobTransform& transform, const RobBlock* block_top);
    void get_mat(int32_t target);
    void solve_ik(int32_t target);
    void recalc_fk_block(const mat4& cur_mat, const RobBlock* block_top, bool rot);
    const vec3* set_global_leaf_sub(const vec3* val, BONE_KIND kind, bool get_data, bool flip_x);
    void get_smooth_target(int32_t target);

    static float_t limit_angle(float_t angle);
};

struct bone_data_parent {
    rob_chara_bone_data* rob_bone_data;
    size_t block_max;
    size_t leaf_pos_max;
    size_t chain_pos_max;
    prj::sys_vector<RobBlock> block_vec;
    prj::sys_vector<uint16_t> bone_indices;
    vec3 gblctr_pos;
    vec3 gblctr_rot;
    uint32_t bone_key_set_count;
    uint32_t global_key_set_count;
    float_t yrot;

    bone_data_parent();
    ~bone_data_parent();

    void ik_init(const std::vector<BODYTYPE>* body_type_table,
        const CHAINPOSRADIUS* joint_table, const CHAINPOSRADIUS* disp_joint_table);
};

struct mot_play_frame_data {
    float_t frame;
    float_t step;
    float_t step_prev;
    float_t frame_max;
    float_t last_frame;
    float_t max_frame;
    mot_play_frame_data_playback_state playback_state;
    mot_play_frame_data_loop_state loop_state;
    float_t loop_begin;
    float_t loop_end;
    bool looped;
    int32_t loop_count;

    void reset();
    void set_frame(float_t frame);
};

struct mot_play_data {
    mot_play_frame_data frame_data;
    int32_t loop_index;
    bool loop_frames_enabled;
    float_t loop_frames;
    float_t* ext_frame;
    float_t* ext_step;

    void reset();
};

struct MotionSmooth {
    int32_t field_0;
    int32_t field_4;
    uint8_t field_8;
    mat4 base_mtx;
    mat4 base_mtx_bef;
    bool base_mtx_set;
    vec3 field_90;
    vec3 field_9C;
    vec3 field_A8;
    float_t move_yang;
    float_t move_yang_bef;
    bool root_ypos;
    bool root_xzpos;
    float_t field_C0;
    float_t field_C4;
    vec3 field_C8;

    MotionSmooth();

    void reset();
};

struct struc_400 {
    bool field_0;
    bool field_1;
    bool field_2;
    bool field_3;
    bool field_4;
    float_t frame;
    float_t move_yang;
    float_t move_yang_bef;
};

class MotionBlend {
public:
    bool field_8;
    bool enable;
    float_t duration;
    float_t frame;
    float_t step;
    float_t offset;
    float_t blend;

    MotionBlend();
    virtual ~MotionBlend();

    virtual void Reset();
    virtual void Field_10(float_t, float_t, int32_t) = 0;
    virtual void Step(struc_400*) = 0;
    virtual void Field_20(prj::sys_vector<RobBlock>* bones_curr, prj::sys_vector<RobBlock>* bones_prev) = 0;
    virtual void Blend(RobBlock* curr, RobBlock* prev) = 0;
    virtual bool Field_30();

    void SetDuration(float_t duration, float_t step, float_t offset);
};

class MotionBlendCross : public MotionBlend {
public:
    bool trans_xz;
    bool trans_y;
    mat4 rot_y_mat;
    mat4 field_64;
    mat4 field_A4;
    mat4 field_E4;

    MotionBlendCross();
    virtual ~MotionBlendCross() override;

    virtual void Reset() override;
    virtual void Field_10(float_t, float_t, int32_t) override;
    virtual void Step(struc_400*) override;
    virtual void Field_20(prj::sys_vector<RobBlock>* bones_curr, prj::sys_vector<RobBlock>* bones_prev) override;
    virtual void Blend(RobBlock* curr, RobBlock* prev) override;
    virtual bool Field_30() override;
};

class MotionBlendCombine : public MotionBlendCross {
public:
    MotionBlendCombine();
    virtual ~MotionBlendCombine() override;

    virtual void Step(struc_400*) override;
    virtual bool Field_30() override;
};

class MotionBlendFreeze : public MotionBlend {
public:
    bool trans_xz;
    bool trans_y;
    int32_t field_24;
    float_t field_28;
    float_t field_2C;
    int32_t field_30;
    mat4 rot_y_mat;
    mat4 field_74;
    mat4 field_B4;
    mat4 field_F4;

    MotionBlendFreeze();
    virtual ~MotionBlendFreeze() override;

    virtual void Reset() override;
    virtual void Field_10(float_t, float_t, int32_t) override;
    virtual void Step(struc_400*) override;
    virtual void Field_20(prj::sys_vector<RobBlock>* bones_curr, prj::sys_vector<RobBlock>* bones_prev) override;
    virtual void Blend(RobBlock* curr, RobBlock* prev) override;
};

class PartialMotionBlendFreeze : public MotionBlendFreeze {
public:
    PartialMotionBlendFreeze();
    virtual ~PartialMotionBlendFreeze() override;

    virtual void Reset() override;
    virtual void Field_10(float_t, float_t, int32_t) override;
    virtual void Step(struc_400*) override;
    virtual void Field_20(prj::sys_vector<RobBlock>* bones_curr, prj::sys_vector<RobBlock>* bones_prev) override;
    virtual void Blend(RobBlock* curr, RobBlock* prev) override;
};

typedef bool(* PFNMOTIONBONECHECKFUNC)(BONE_BLK blk);

struct motion_blend_mot_enabled_bones {
    PFNMOTIONBONECHECKFUNC func;
    prj::sys_vector<bool> arr;
    size_t count;

    motion_blend_mot_enabled_bones();
    ~motion_blend_mot_enabled_bones();

    void check();
    void init(PFNMOTIONBONECHECKFUNC func, size_t count);
    void reset();
    void set(void(*func)(prj::sys_vector<bool>&));
};

struct motion_blend_mot {
    motion_blend_mot_enabled_bones enabled_bones;
    MotionBlendCross cross;
    MotionBlendFreeze freeze;
    MotionBlendCombine combine;
    bone_data_parent bone_data;
    mot_key_data mot_key_data;
    mot_play_data mot_play_data;
    MotionSmooth smooth;
    int32_t field_5CC;
    MotionBlend* blend;

    motion_blend_mot();
    ~motion_blend_mot();

    void apply_global_transform();
    void copy_rot_trans();
    void copy_rot_trans(const prj::sys_vector<RobBlock>& block_vec);
    bool get_blend_enable();
    void get_n_hara_cp_position(vec3& value);
    MotionBlendType get_type();
    void init(rob_chara_bone_data* rob_bone_data,
        PFNMOTIONBONECHECKFUNC check_func, const bone_database* bone_data);
    void interpolate();
    void load_file(uint32_t motnum, MotionBlendType blend_type, float_t blend,
        const bone_database* bone_data, const motion_database* mot_db);
    void mult_mat(const mat4* mat);
    void reset();
    void set_arm_length(BONE_BLK blk, float_t value);
    void set_blend(MotionBlendType blend_type, float_t blend);
    void set_blend_duration(float_t duration, float_t step, float_t offset);
    void set_step(float_t step);
    void get_smooth_target(int32_t motion_body_type);

    static bool interpolate_get_flip(MotionSmooth& a1);
};

struct rob_chara_bone_data_adjust_scale {
    float_t base;
    float_t body;
    float_t arm;
    float_t height;

    rob_chara_bone_data_adjust_scale();
};

struct partial_motion_blend_mot {
    motion_blend_mot_enabled_bones enabled_bones;
    bool disable;
    mot_key_data mot_key_data;
    mot_play_data mot_play_data;
    PartialMotionBlendFreeze blend;

    partial_motion_blend_mot();
    ~partial_motion_blend_mot();

    void init(BONE_KIND type, PFNMOTIONBONECHECKFUNC bone_check_func,
        size_t block_max, const bone_database* bone_data);
    void interpolate(prj::sys_vector<RobBlock>& block_vec,
        const prj::sys_vector<uint16_t>* bone_indices, BONE_KIND kind);
    void load_file(uint32_t motnum, const motion_database* mot_db);
    void reset();
    void set_blend_duration(float_t duration, float_t step, float_t offset);
    void set_frame(float_t frame);
    void set_step(float_t step);
    void get_smooth_target(prj::sys_vector<RobBlock>& block_vec);
};

struct rob_chara_look_anim_eye_param_limits {
    float_t xrot_neg;
    float_t xrot_pos;
    float_t yrot_neg;
    float_t yrot_pos;
};

struct rob_chara_look_anim_eye_param {
    rob_chara_look_anim_eye_param_limits ac;
    rob_chara_look_anim_eye_param_limits ft;
    vec3 pos;
    float_t xrot_adjust_neg;
    float_t xrot_adjust_pos;
    float_t xrot_adjust_dir_neg;
    float_t xrot_adjust_dir_pos;
    float_t xrot_adjust_clear_neg;
    float_t xrot_adjust_clear_pos;

    rob_chara_look_anim_eye_param();
    rob_chara_look_anim_eye_param(rob_chara_look_anim_eye_param_limits ac,
        rob_chara_look_anim_eye_param_limits ft,
        vec3 pos, float_t xrot_adjust_neg, float_t xrot_adjust_pos,
        float_t xrot_adjust_dir_neg, float_t xrot_adjust_dir_pos,
        float_t xrot_adjust_clear_neg, float_t xrot_adjust_clear_pos);

    void reset();
};

struct struc_243 {
    float_t min;
    float_t max;
    float_t field_8;
    float_t field_C;
    float_t field_10;
    float_t field_14;
    float_t field_18;
    float_t field_1C;
    float_t field_20;
    float_t field_24;
};

struct struc_823 {
    float_t field_0;
    float_t field_4;
    float_t field_8;
    float_t field_C;
};

struct Motion {
    struct AshiOidashi {
        float_t y_bef;
        float_t y_old;
        float_t scale;

        AshiOidashi();

        void reset();
    };

    struct AshiOidashiColle {
        AshiOidashi l;
        AshiOidashi r;

        AshiOidashiColle();

        void calc(prj::sys_vector<RobBlock>& block_vec, const mat4& cur_mat, float_t step,
            BONE_KIND kind, const BONE_BLK* c_momo_l_ik_blk, const BONE_BLK* c_momo_r_ik_blk);
        void calc_sub(RobBlock& bl_momo, const RobBlock& bl_toe,
            float_t hh, Motion::AshiOidashi& ashi, float_t step);
        void reset();
    };
};

struct rob_chara_look_anim {
    prj::sys_vector<RobBlock>* block_vec;
    mat4 mat;
    rob_chara_look_anim_eye_param param;
    bool update_view_point;
    bool init_head_rotation;
    bool head_rotation;
    bool init_eyes_rotation;
    bool eyes_rotation;
    bool disable;
    float_t head_rot_strength;
    float_t eyes_rot_strength;
    float_t eyes_rot_step;
    float_t duration;
    float_t eyes_rot_frame;
    float_t step;
    float_t head_rot_frame;
    float_t field_B0;
    vec3 target_view_point;
    vec3 view_point;
    mat4 left_eye_mat;
    mat4 right_eye_mat;
    bool ext_head_rotation;
    float_t ext_head_rot_strength;
    float_t ext_head_rot_y_angle;
    float_t ext_head_rot_x_angle;
    struc_243 field_15C;
    vec3 field_184;
    bool field_190;
    bool field_191;
    bool field_192;
    bool field_193;
    bool field_194;
    bool field_195;
    float_t head_rot_blend;
    struc_823 field_19C;
    struc_823 field_1AC;
    float_t eyes_xrot_adjust_neg;
    float_t eyes_xrot_adjust_pos;
    bool ft;
    int32_t type;

    rob_chara_look_anim();

    void reset();
    void set(bool update_view_point, bool rotation_enable, float_t head_rot_strength,
        float_t eyes_rot_strength, float_t duration, float_t eyes_rot_step, float_t a8, bool ft);
    void set_eyes_xrot_adjust(float_t neg, float_t pos);
    void set_target_view_point(const vec3& value);
};

struct rob_sleeve_data {
    float_t radius;
    float_t cyofs;
    float_t czofs;
    float_t ymin;
    float_t ymax;
    float_t zmin;
    float_t zmax;
    float_t mune_xofs;
    float_t mune_yofs;
    float_t mune_zofs;
    float_t mune_rad;
};

struct rob_chara_sleeve_adjust {
    rob_sleeve_data sleeve_l;
    rob_sleeve_data sleeve_r;
    bool enable1;
    bool enable2;
    vec3 field_5C;
    vec3 field_68;
    vec3 field_74;
    vec3 field_80;
    float_t radius;
    prj::sys_vector<RobBlock>* block_vec;
    float_t step;

    rob_chara_sleeve_adjust();

    void reset();
};

class rob_chara_bone_data {
public:
    bool field_0;
    bool field_1;
    size_t mat_max;
    size_t node_max;
    size_t block_max;
    size_t leaf_pos_max;
    size_t chain_pos_max;
    std::vector<mat4> mat_vec;
    std::vector<mat4> mat2_vec;
    std::vector<RobNode> node_vec;
    BONE_KIND kind;
    BONE_KIND disp_kind;
    std::vector<motion_blend_mot*> motions;
    std::list<size_t> motion_indices;
    std::list<size_t> motion_loaded_indices;
    std::list<motion_blend_mot*> motion_loaded;
    partial_motion_blend_mot face;
    partial_motion_blend_mot hand_l;
    partial_motion_blend_mot hand_r;
    partial_motion_blend_mot mouth;
    partial_motion_blend_mot eyes;
    partial_motion_blend_mot eyelid;
    bool disable_eye_motion;
    rob_chara_bone_data_adjust_scale adjust_scale;
    Motion::AshiOidashiColle ashi_oidashi;
    rob_chara_look_anim look_anim;
    rob_chara_sleeve_adjust sleeve_adjust;

    rob_chara_bone_data();
    ~rob_chara_bone_data();

    bool check_look_anim_head_rotation();
    bool check_look_anim_ext_head_rotation();
    float_t get_frame() const;
    float_t get_frame_max() const;
    vec3* get_look_anim_target_view_point();
    bool get_look_anim_ext_head_rotation();
    bool get_look_anim_head_rotation();
    bool get_look_anim_update_view_point();
    mat4* get_mats_mat(size_t index);
    bool get_motion_has_looped();
    void interpolate();
    void load_eyes_motion(uint32_t motnum, const motion_database* mot_db);
    void load_face_motion(uint32_t motnum, const motion_database* mot_db);
    void load_eyelid_motion(uint32_t motnum, const motion_database* mot_db);
    void load_hand_l_motion(uint32_t motnum, const motion_database* mot_db);
    void load_hand_r_motion(uint32_t motnum, const motion_database* mot_db);
    void load_mouth_motion(uint32_t motnum, const motion_database* mot_db);
    void motion_step();
    void reset();
    void set_disable_eye_motion(bool value);
    void set_eyelid_blend_duration(float_t duration, float_t step, float_t offset);
    void set_eyelid_frame(float_t frame);
    void set_eyelid_step(float_t step);
    void set_eyes_blend_duration(float_t duration, float_t step, float_t offset);
    void set_eyes_frame(float_t frame);
    void set_eyes_step(float_t step);
    void set_face_blend_duration(float_t duration, float_t step, float_t offset);
    void set_face_frame(float_t frame);
    void set_face_step(float_t step);
    void set_frame(float_t frame);
    void set_hand_l_blend_duration(float_t duration, float_t step, float_t offset);
    void set_hand_l_frame(float_t frame);
    void set_hand_l_step(float_t step);
    void set_hand_r_blend_duration(float_t duration, float_t step, float_t offset);
    void set_hand_r_frame(float_t frame);
    void set_hand_r_step(float_t step);
    void set_look_anim(bool update_view_point, bool rotation_enable, float_t head_rot_strength,
        float_t eyes_rot_strength, float_t duration, float_t eyes_rot_step, float_t a8, bool ft);
    void set_look_anim_target_view_point(const vec3& value);
    void set_mats_identity();
    void set_motion_blend_duration(float_t duration, float_t step, float_t offset);
    void set_motion_frame(float_t frame, float_t step, float_t frame_max);
    void set_motion_loop(float_t loop_begin, int32_t loop_count, float_t loop_end);
    void set_motion_loop_state(mot_play_frame_data_loop_state value);
    void set_motion_max_frame(float_t value);
    void set_motion_playback_state(mot_play_frame_data_playback_state value);
    void set_mouth_blend_duration(float_t duration, float_t step, float_t offset);
    void set_mouth_frame(float_t frame);
    void set_mouth_step(float_t step);
    void update(const mat4* mat);
};

class RobAngle {
public:
    int16_t value;

    inline RobAngle( ) : value() {}
    inline RobAngle(int16_t value) : value(value) {}

    inline float_t get_deg() {
        return (float_t)((double_t)value * 180.0 * (1.0 / 32768.0));
    }

    inline float_t get_rad() {
        return (float_t)((double_t)value * M_PI * (1.0 / 32768.0));
    }

    inline void set_deg(float_t in_deg) {
        value = (int16_t)(int32_t)(in_deg * 32768.0f * (float_t)(1.0 / 180.0));
    }

    inline void set_rad(float_t in_rad) {
        value = (int16_t)(int32_t)((double_t)(in_rad * 32768.0f) * (1.0 / M_PI));
    }
};

inline RobAngle operator+(const RobAngle& left, const RobAngle& right) {
    return (int16_t)(left.value + right.value);
}

inline RobAngle operator+(const int32_t& left, const RobAngle& right) {
    return (int16_t)(left + right.value);
}

inline RobAngle operator+(const RobAngle& left, const int32_t& right) {
    return (int16_t)(left.value + right);
}

inline void operator+=(RobAngle& left, const RobAngle& right) {
    left = (int16_t)(left.value + right.value);
}

inline void operator+=(RobAngle& left, const int32_t right) {
    left = (int16_t)(left.value + right);
}

inline RobAngle operator-(const RobAngle& left, const RobAngle& right) {
    return (int16_t)(left.value - right.value);
}

inline RobAngle operator-(const int32_t& left, const RobAngle& right) {
    return (int16_t)(left - right.value);
}

inline RobAngle operator-(const RobAngle& left, const int32_t& right) {
    return (int16_t)(left.value - right);
}

inline void operator-=(RobAngle& left, const RobAngle& right) {
    left = (int16_t)(left.value - right.value);
}

inline void operator-=(RobAngle& left, const int32_t right) {
    left = (int16_t)(left.value - right);
}

inline RobAngle operator-(const RobAngle& left) {
    return -left.value;
}

struct eyes_adjust {
    bool xrot_adjust;
    eyes_base_adjust_type base_adjust;
    float_t neg;
    float_t pos;

    eyes_adjust();
};

struct RobItemEquipInit {
    uint32_t item_no[4];
};

struct RobInit {
    RobType rob_type;
    bool disp;
    bool mirror;
    bool drank_reset;
    vec3 pos;
    RobAngle yang;
    int16_t energy;
    rob_sleeve_data sleeve_l;
    rob_sleeve_data sleeve_r;
    int32_t field_70;
    uint32_t face_mot_slot[10];
    int32_t chara_size_index;
    bool height_adjust;
    RobItemEquipInit item;
    eyes_adjust eyes_adjust;

    RobInit();

    void init();
};

struct RobCollisionData {
    vec3 trans;
    float_t scale;
    BONE_ID bone;
    int32_t field_14;
};

struct RobKamae {
    const char* name;
    int32_t field_8;
    int32_t field_C;
    int32_t field_10;
    int32_t field_14;
    int32_t field_18;
};

struct struc_344 {
    int32_t chara_size_index;
    int32_t field_4;
    int32_t swim_costume;
    int32_t swim_s_costume;
};

struct RobData {
    int32_t objset;
    BONE_KIND bone;
    object_info body_obj_uid[15];
    uint32_t motfile;
    uint32_t motfile_auth;
    const RobCollisionData* colli_data;
    const RobCollisionData* push_colli_data;
    const RobKamae** kamae_tbl;
    int32_t field_840;
    int32_t field_844;
    struc_344 field_848;
    object_info head_objects[9];
    object_info face_objects[15];
};

class RobSkinDisp;

struct RobSkinOfs {
    bool flag;
    RobTransform transform;

    RobSkinOfs();
};

class ExNodeBlock {
public:
    RobNode* dst_node;
    ExNodeType type;
    const char* name;
    const RobNode* parent;
    std::string parent_name;
    ExNodeBlock* parent_node;
    const RobSkinDisp* skin_disp;
    bool is_parent;
    bool done;
    bool has_children_node;

    ExNodeBlock();
    virtual ~ExNodeBlock();

    virtual void init() = 0;
    virtual void CtrlBegin() = 0;
    virtual void CtrlStep(int32_t stage, bool disable_ex_force) = 0;
    virtual void ctrl() = 0;
    virtual void CtrlOsagePlayData() = 0;
    virtual void disp(const mat4& mat, render_context* rctx) = 0;
    virtual void dest();
    virtual void disp_debug() = 0;
    virtual void pos_init() = 0;
    virtual void pos_init_cont() = 0;
    virtual void CtrlEnd();

    void init_members();
    void set_data(RobNode* node, ExNodeType type,
        const char* name, const RobSkinDisp* skin_disp);
    void set_name(const char* name);
};

class ExNullBlock : public ExNodeBlock {
public:
    const obj_skin_ex_node_constraint* data;

    ExNullBlock();
    virtual ~ExNullBlock() override;

    virtual void init() override;
    virtual void CtrlBegin() override;
    virtual void CtrlStep(int32_t stage, bool disable_ex_force) override;
    virtual void ctrl() override;
    virtual void CtrlOsagePlayData() override;
    virtual void disp(const mat4& mat, render_context* rctx) override;
    virtual void disp_debug() override;
    virtual void pos_init() override;
    virtual void pos_init_cont() override;

    void set_data(const RobSkinDisp* skin_disp, const obj_skin_ex_node_constraint* data,
        const char* name, const bone_database* bone_data);
};

struct RobJointNode;

struct RobJointNodeDataNormalRef {
    bool set;
    RobJointNode* n;
    RobJointNode* u;
    RobJointNode* d;
    RobJointNode* l;
    RobJointNode* r;
    mat4 mat;

    RobJointNodeDataNormalRef();

    bool Check();
    void GetMat(mat4* mat);
    void Load();

    static bool GetAxes(const vec3& l_trans, const vec3& r_trans,
        const vec3& u_trans, const vec3& d_trans, vec3& z_axis, vec3& y_axis, vec3& x_axis);
};

struct skin_param_hinge {
    float_t ymin;
    float_t ymax;
    float_t zmin;
    float_t zmax;

    inline skin_param_hinge() {
        ymin = -90.0f;
        ymax = 90.0f;
        zmin = -90.0f;
        zmax = 90.0f;
    }

    bool clamp(float_t& y, float_t& z) const;
    void limit();
};

struct skin_param_osage_node {
    float_t coli_r;
    float_t weight;
    float_t inertial_cancel;
    skin_param_hinge hinge;

    skin_param_osage_node();
};

struct RobJointNodeResetData {
    vec3 pos;
    vec3 vec;
    vec3 rotation;
    float_t length;

    RobJointNodeResetData();
};

struct skin_param_osage_root;

struct RobJointNodeData {
    float_t force;
    std::vector<RobJointNode*> boc;
    RobJointNodeDataNormalRef normal_ref;
    skin_param_osage_node skp_osg_node;

    RobJointNodeData();
    ~RobJointNodeData();

    void SetForce(const skin_param_osage_root& skp_root,
        skin_param_osage_node* skp_osg_node, size_t index);

    void reset();
};

struct opd_blend_data {
    uint32_t motnum;
    float_t frame;
    float_t frame_max;
    bool use_blend;
#if OPD_PLAY_GEN
    bool no_loop; // Added
#endif
    MotionBlendType type;
    float_t blend;
};

struct opd_vec3_data {
    const float_t* x;
    const float_t* y;
    const float_t* z;
};

struct opd_node_data {
    float_t length;
    vec3 rotation;

    opd_node_data();
    opd_node_data(float_t length, vec3 rotation);

    static void lerp(opd_node_data& dst, const opd_node_data& src0, const opd_node_data& src1, float_t blend);
};

struct opd_node_data_pair {
    opd_node_data curr;
    opd_node_data prev;

    opd_node_data_pair();

    void set_data(const opd_blend_data* blend_data, const opd_node_data& node_data);
};

struct RobJointNode {
    float_t length_back;
    vec3 pos;
    vec3 old_pos;
    vec3 vec;
    vec3 vel;
    float_t length_next;
    RobNode* dst_node;
    mat4* dst_node_mat;
    mat4 mat;
    RobJointNode* distance;
    float_t length_dist;
    vec3 rel_pos;
    RobJointNodeResetData reset_data;
    float_t hit;
    float_t friction;
    vec3 ex_force;
    float_t force;
    RobJointNodeData* data_ptr;
    RobJointNodeData data;
    std::vector<opd_vec3_data> opd_data;
    opd_node_data_pair opd_node_data;

    RobJointNode();
    ~RobJointNode();

    void CheckFloorCollision(const float_t& floor_height);
    void CheckNodeDistance(const float_t& step, const float_t& hsc);

    void reset();

    inline RobJointNode& GetNextNode() {
        return *(this + 1);
    }

    inline const RobJointNode& GetNextNode() const {
        return *(this + 1);
    }

    inline RobJointNode& GetPrevNode() {
        return *(this - 1);
    }

    inline const RobJointNode& GetPrevNode() const {
        return *(this - 1);
    }

    inline float_t TranslateMat(mat4& mat, const bool rot_clamped, const float_t hsc_x) {
        const float_t dist = vec3::distance_squared(pos, GetPrevNode().pos);
        const float_t len = length_back * hsc_x;

        float_t length;
        bool length_clamped;
        if (dist >= len * len) {
            length = sqrtf(dist);
            length_clamped = false;
        }
        else {
            length = len;
            length_clamped = true;
        }

        mat4_mul_translate_x(&mat, length, &mat);
        if (rot_clamped || length_clamped)
            mat4_get_translation(&mat, &pos);
        return length;
    }
};

namespace SkinParam {
    struct CollisionParam {
        CollisionType type;
        int32_t node_idx[2];
        float_t radius;
        vec3 pos[2];

        CollisionParam();
    };
};

struct skin_param_osage_root_normal_ref {
    std::string n;
    std::string u;
    std::string d;
    std::string l;
    std::string r;

    skin_param_osage_root_normal_ref();
    ~skin_param_osage_root_normal_ref();
};

struct skin_param_osage_root_boc {
    int32_t ed_node;
    std::string ed_root;
    int32_t st_node;

    skin_param_osage_root_boc();
    ~skin_param_osage_root_boc();
};

struct skin_param_osage_root {
    int32_t field_0;
    float_t force;
    float_t force_gain;
    float_t air_res;
    float_t rot_y;
    float_t rot_z;
    float_t init_rot_y;
    float_t init_rot_z;
    float_t hinge_y;
    float_t hinge_z;
    const char* name;
    std::vector<SkinParam::CollisionParam> coli;
    float_t coli_r;
    float_t friction;
    float_t wind_afc;
    ROTTYPE rottype;
    std::vector<skin_param_osage_root_boc> boc;
    SkinParam::RootCollisionType coli_type;
    float_t stiffness;
    float_t move_cancel;
    std::string colli_tgt_osg;
    std::vector<skin_param_osage_root_normal_ref> normal_ref;

    skin_param_osage_root();
    ~skin_param_osage_root();
};

struct skin_param {
    std::vector<SkinParam::CollisionParam> coli;
    float_t friction;
    float_t wind_afc;
    float_t air_res;
    vec3 rot;
    vec3 init_rot;
    SkinParam::RootCollisionType coli_type;
    float_t stiffness;
    float_t move_cancel;
    float_t coli_r;
    skin_param_hinge hinge;
    float_t force;
    float_t force_gain;
    std::vector<RobJointNode>* colli_tgt_osg;

    skin_param();
    ~skin_param();

    void set_skin_param_osage_root(const skin_param_osage_root& skp_root);

    inline void reset() {
        coli.clear();
        friction = 1.0f;
        wind_afc = 0.0f;
        air_res = 1.0f;
        rot = 0.0f;
        init_rot = 0.0f;
        coli_type = SkinParam::RootCollisionTypeEnd;
        stiffness = 0.0f;
        move_cancel = -0.01f;
        coli_r = 0.0f;
        hinge = skin_param_hinge();
        hinge.limit();
        force = 0.0f;
        force_gain = 0.0f;
        colli_tgt_osg = 0;
    }
};

struct OsageCollision {
    struct Work {
        SkinParam::CollisionType type;
        float_t radius;
        vec3 pos[2];
        vec3 vec_center;
        float_t vec_center_length;
        float_t vec_center_length_squared;
        float_t friction;

        Work();

        static void update_cls_work(OsageCollision::Work* work,
            const SkinParam::CollisionParam* cls, const mat4* motmat);
        static void update_cls_work(OsageCollision::Work* work,
            const std::vector<SkinParam::CollisionParam>& cls_list, const mat4* motmat);
    };

    std::vector<Work> work_list;

    OsageCollision();
    ~OsageCollision();

    static int32_t cls_aabb_oidashi(vec3& vec, const vec3& p, const OsageCollision::Work* cls, const float_t r);
    static int32_t cls_ball_oidashi(vec3& vec, const vec3& p, const vec3& center, const float_t r);
    static int32_t cls_capsule_oidashi(vec3& vec, const vec3& p, const OsageCollision::Work* coli, const float_t r);
    static int32_t cls_ellipse_oidashi(vec3& vec, const vec3& p, const OsageCollision::Work* cls, const float_t r);
    static int32_t cls_line2ball_oidashi(vec3& vec,
        const vec3& p0, const vec3& p1, const vec3& q, const float_t r);
    static int32_t cls_line2capsule_oidashi(vec3& vec,
        const vec3& p0, const vec3& p1, const OsageCollision::Work* cls, const float_t r);
    static int32_t cls_line2ellipse_oidashi(vec3& vec,
        const vec3& p0, const vec3& p1, const OsageCollision::Work* cls, const float_t r);
    static int32_t cls_plane_oidashi(vec3& vec, const vec3& p, const vec3& p1, const vec3& p2, const float_t r);
    static void get_nearest_line2point(vec3& nearest, const vec3& p0, const vec3& p1, const vec3& q);
    static int32_t osage_capsule_cls(const OsageCollision::Work* cls, vec3& p0, vec3& p1, const float_t& cls_r);
    static int32_t osage_capsule_cls(vec3& p0, vec3& p1, const float_t& cls_r, const OsageCollision::Work* cls);
    static int32_t osage_cls(const OsageCollision::Work* cls, vec3& p, const float_t& cls_r);
    static int32_t osage_cls(vec3& p, const float_t& cls_r, const OsageCollision::Work* cls, float_t* fric = 0);
    static int32_t osage_cls_work_list(vec3& p, const float_t& cls_r, const OsageCollision& coli, float_t* fric = 0);
};

struct osage_ring_data {
    float_t rect_x;
    float_t rect_y;
    float_t rect_width;
    float_t rect_height;
    float_t ring_height;
    float_t out_height;
    bool init;
    OsageCollision coli_object;
    std::vector<SkinParam::CollisionParam> skp_root_coli;

    osage_ring_data();
    ~osage_ring_data();

    float_t get_floor_height(const vec3& pos, const float_t coli_r) const;
    void reset();

    static void parse(const std::string& path, osage_ring_data& ring);

    osage_ring_data& operator=(const osage_ring_data& ring);
};

struct skin_param_file_data;

struct CLOTH_VERTEX {
    uint32_t flag;
    vec3 pos;
    vec3 org_pos;
    vec3 old_pos;
    vec3 vec;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    float_t m;
    vec2 uv;
    vec3 localvec;
    float_t length_up;
    float_t length_down;
    float_t length_left;
    float_t length_right;
    vec3 F;
    RobJointNodeResetData reset_data;
    std::vector<opd_vec3_data> opd_data;
    opd_node_data_pair opd_node_data;

    CLOTH_VERTEX();
    ~CLOTH_VERTEX();
};

struct CLOTH_FLAG {
    uint32_t exec : 1;
    uint32_t disp : 1;
    uint32_t ring : 1;
};

struct CLOTH_SPRING {
    size_t index0;
    size_t index1;
    float_t length;
};

struct CLOTH {
    CLOTH_FLAG flag;
    size_t width;
    size_t height;
    std::vector<CLOTH_VERTEX> vtxarg;
    vec3 wind_dir;
    float_t wet;
    bool use_ex_force;
    vec3 ex_force;
    std::vector<CLOTH_SPRING> spring;
    skin_param* skin_param_ptr;
    skin_param skin_param;
    OsageCollision::Work coli_chara[64];
    OsageCollision::Work coli_ring[64];
    osage_ring_data ring;

    CLOTH();
    virtual void set_spring();
    virtual ~CLOTH();

    virtual void set_coli_r(float_t value);
    virtual void set_friction(float_t value);
    virtual void set_wind_affect(float_t value);
    virtual void set_wind_dir(const vec3& value);
    virtual void set_wetness(float_t value);
    virtual void set_angle_limit(float_t y, float_t z);
    virtual CLOTH_VERTEX* get_vertex();
    virtual void init();
    virtual void disp_debug();
    virtual void dest();
};

struct CLOTH_WEIGHTED_ROOT {
    vec3 pos;
    vec3 normal;
    vec4 tangent;
    const RobNode* node[4];
    const mat4* node_mat[4];
    const mat4* bone_mat[4];
    float_t weight[4];
    mat4 mat;
    mat4 mat_pos;
    mat4 inv_mat_pos;
};

struct RobCloth : public CLOTH {
    const mat4* local_mat;
    std::vector<CLOTH_WEIGHTED_ROOT> root;
    const RobSkinDisp* skin_disp;
    const obj_skin_ex_node_cloth_root* root_data;
    const obj_skin_ex_node_cloth* data;
    float_t move_cancel;
    bool osage_reset;
    obj_mesh mesh[2];
    obj_sub_mesh submesh[2][4];
    obj_mesh_vertex_buffer vb[2];
    obj_mesh_index_buffer ib[2];
    std::map<std::pair<int32_t, int32_t>, std::list<RobJointNodeResetData>> motion_reset_data;
    std::list<RobJointNodeResetData>* reset_data_list;

    RobCloth();
    virtual ~RobCloth() override;

    virtual void disp_debug() override;
    virtual void dest() override;

    void AddMotionResetData(uint32_t motnum, float_t frame);
    void ApplyResetData();
    void CtrlOsagePlayData(const std::vector<opd_blend_data>& opd_blend_data);
    const float_t* LoadOpdData(size_t node_index, const float_t* opd_data, size_t opd_count);
    void LoadSkinParam(void* kv, const char* name, const bone_database* bone_data);
    void SetMotionResetData(uint32_t motnum, float_t frame);
    const float_t* SetOsagePlayDataInit(const float_t* opdi_data);
    void SetOsageReset();
    void SetRing(const osage_ring_data& ring);
    void SetSkinParam(skin_param_file_data* skp);
    void SetSkinParamOsageRoot(const skin_param_osage_root& skp_root);

    void calc(const float_t dt);
    void calc_force(float_t time, bool init_flag);
    void calc_normal();
    void calc_root();
    void calc_stretch();
    void calc_velocity(const float_t dt, bool a3);
    void ctrl(const float_t dt, bool init_flag);
    void disp(const mat4& mat, render_context* rctx);
    void make_osage_coli(const mat4* motmat);
    void modify_obj();
    void pos_init();
    void pos_init_cont();
    void reset_ex_force();
    void set_data(size_t w, size_t h, const obj_skin_ex_node_cloth_root* rt_data,
        const obj_skin_ex_node_cloth_point* move, const mat4* lcl_mat, uint32_t ring_flag,
        const RobSkinDisp* skin, const bone_database* bone_data);
    void set_data(const obj_skin_ex_node_cloth* cldata,
        const RobSkinDisp* skin, const bone_database* bone_data);
    void set_ex_force(const vec3& f);
    void set_move_cancel(const float_t& mv_ccl);
    void set_param(float_t force, float_t force_gain, float_t air_res);
};

class ExClothBlock : public ExNodeBlock {
public:
    RobCloth cloth_work;
    const obj_skin_ex_node_cloth* data;
    const mat4* motion_matrix;
    size_t block_idx;

    ExClothBlock();
    virtual ~ExClothBlock() override;

    virtual void init() override;
    virtual void CtrlBegin() override;
    virtual void CtrlStep(int32_t stage, bool disable_ex_force) override;
    virtual void ctrl() override;
    virtual void CtrlOsagePlayData() override;
    virtual void disp(const mat4& mat, render_context* rctx) override;
    virtual void dest() override;
    virtual void disp_debug() override;
    virtual void pos_init() override;
    virtual void pos_init_cont() override;

    void AddMotionResetData(const uint32_t& motnum, const float_t& frame);
    const float_t* LoadOpdData(size_t node_index, const float_t* opd_data, size_t opd_count);
    void SetMotionResetData(const uint32_t& motnum, const float_t& frame);
    const float_t* SetOsagePlayDataInit(const float_t* opdi_data);
    void SetOsageReset();
    void SetRing(const osage_ring_data& ring);
    void SetSkinParam(skin_param_file_data* skp);
    void SetSkinParamOsageRoot(const skin_param_osage_root* skp_root);

    void reset_ex_force();
    void set_data(const RobSkinDisp* skin_disp, const obj_skin_ex_node_cloth* cls_data,
        const skin_param_osage_root* skp_root, const bone_database* bone_data);
    void set_ex_force(const vec3& f);
    void set_move_cancel(const float_t& mv_ccl);
    void set_param();
};

struct skin_param_file_data {
    skin_param skin_param;
    std::vector<RobJointNodeData> nodes_data;
    bool depends_on_others;

    skin_param_file_data();
    ~skin_param_file_data();
};

struct osage_setting_osg_cat {
    rob_osage_parts parts;
    size_t exf;

    osage_setting_osg_cat();
};

struct RobOsage {
    skin_param* skin_param_ptr;
    RobTransform transform;
    std::vector<RobJointNode> joint_node_vec;
    RobJointNode effector;
    skin_param skin_param;
    osage_setting_osg_cat osage_setting;
    bool apply_physics;
    bool field_2A1;
    float_t field_2A4;
    OsageCollision::Work coli_chara[64];
    OsageCollision::Work coli_ring[64];
    vec3 wind_dir;
    float_t wet;
    ROTTYPE rottype;
    mat4* root_matrix_ptr;
    mat4 root_matrix_prev;
    float_t move_cancel;
    bool move_cancelled;
    bool osage_reset;
    bool osage_reset_done;
    bool disable_collision;
    osage_ring_data ring;
    std::map<std::pair<int32_t, int32_t>, std::list<RobJointNodeResetData>> motion_reset_data;
    std::list<RobJointNodeResetData>* reset_data_list;
    bool use_ex_force;
    vec3 ex_force;

    RobOsage();
    ~RobOsage();

    void AddMotionResetData(const uint32_t& motnum, const float_t& frame);
    void ApplyBocRootColi(const float_t step);
    void ApplyPhysics(const mat4& root_matrix, const vec3& hsc,
        const float_t step, bool disable_ex_force, bool ring_coli, bool has_children_node);
    void ApplyResetData(const mat4& mat);
    void BeginCalc(const mat4& root_matrix, const vec3& hsc, bool has_children_node);
    bool CheckPartsBits(const rob_osage_parts_bit& parts_bits);
    void CollideNodes(const float_t step);
    void CollideNodesTargetOsage(const mat4& root_matrix,
        const vec3& hsc, const float_t step, bool collide_nodes);
    void CtrlEnd(const vec3& hsc);
    void CtrlOsagePlayData(const mat4& root_matrix,
        const vec3& hsc, const std::vector<opd_blend_data>& opd_blend_data);
    void EndCalc(const mat4& root_matrix, const vec3& hsc,
        const float_t step, bool disable_ex_force);
    const float_t* LoadOpdData(size_t node_index, const float_t* opd_data, size_t opd_count);
    void LoadSkinParam(void* kv, const char* name,
        skin_param_osage_root& skp_root, const object_info& obj_info, const bone_database* bone_data);
    void RotateMat(mat4& mat, const vec3& hsc, bool init_rot = false);
    void SetDisableCollision(const bool& value);
    void SetMotionResetData(const uint32_t& motnum, const float_t& frame);
    void SetNodesExternalForce(const vec3* ex_force, const float_t& gain);
    void SetNodesForce(const float_t& force);
    const float_t* SetOsagePlayDataInit(const float_t* opdi_data);
    void SetOsageReset();
    void SetRing(const osage_ring_data& ring);
    void SetSkinParam(skin_param_file_data* skp);
    void SetSkinParamOsageRoot(const skin_param_osage_root& skp_root);
    void SetSkpOsgNodes(const std::vector<skin_param_osage_node>* skp_osg_nodes);
    void SetWindDirection(const vec3* value);

    void ctrl(const mat4& root_matrix, const vec3& hsc, const float_t step);
    void dest();
    void dest_boc();
    RobJointNode* get_joint_node(size_t index);
    void init(const obj_skin_ex_node_osage* osg_data, const obj_skin_osage_joint* joint,
        RobNode* ex_node, const obj_skin* skin);
    void make_osage_coli(const mat4* motmat);
    void pos_init(const mat4& root_matrix, const vec3& hsc, bool dist_flag);
    void pos_init_cont(const mat4& root_matrix, const vec3& hsc,
        const float_t step, bool disable_ex_force);
    void reset_ex_force();
    void set_air_res(float_t air);
    void set_angle(float_t angle_y, float_t angle_z);
    void set_coli_r(float_t coli_r);
    void set_ex_force(const vec3& f);
    void set_force(float_t force, float_t gain);
    void set_init_angle(float_t angle_y, float_t angle_z);
    void set_limit_angle(float_t angle_y, float_t angle_z);
    void set_move_cancel(const float_t& mv_ccl);
    void set_rot_type(ROTTYPE rot_type);
};

struct ExOsageBlockFlag {
    uint32_t alive : 1;
    uint32_t pos_init : 1;
};

class ExOsageBlock : public ExNodeBlock {
public:
    size_t block_idx;
    RobOsage osage_work;
    const mat4* motion_matrix;
    ExOsageBlockFlag flag;
    float_t init_coma_step;

    ExOsageBlock();
    virtual ~ExOsageBlock() override;

    virtual void init() override;
    virtual void CtrlBegin() override;
    virtual void CtrlStep(int32_t stage, bool disable_ex_force) override;
    virtual void ctrl() override;
    virtual void CtrlOsagePlayData() override;
    virtual void disp(const mat4& mat, render_context* rctx) override;
    virtual void dest() override;
    virtual void disp_debug() override;
    virtual void pos_init() override;
    virtual void pos_init_cont() override;
    virtual void CtrlEnd() override;

    void AddMotionResetData(const uint32_t& motnum, const float_t& frame);
    const float_t* LoadOpdData(size_t node_index, const float_t* opd_data, size_t opd_count);
    void SetDisableCollision(const bool& value);
    void SetMotionResetData(const uint32_t& motnum, const float_t& frame);
    const float_t* SetOsagePlayDataInit(const float_t* opdi_data);
    void SetOsageReset();
    void SetRing(const osage_ring_data& ring);
    void SetSkinParam(skin_param_file_data* skp);
    void SetWindDirection();

    void make_joint_map(const obj_skin_ex_node_osage* root,
        const obj_skin_osage_joint* joint, prj::vector_pair<uint32_t, RobJointNode*>& joint_map,
        std::map<std::string, ExNodeBlock*>& node_name_map);
    void reset_ex_force();
    void set_data(const RobSkinDisp* skin_disp, const obj_skin_ex_node_osage* root,
        const obj_skin_osage_joint* joint, const RobNode* mot_node,
        RobNode* ex_node, const obj_skin* skin);
    void set_ex_force(const vec3& f);
    void set_move_cancel(const float_t& mv_ccl);
};

class ExConstraintBlock : public ExNodeBlock {
public:
    obj_skin_ex_node_constraint_type cns_type;
    const RobNode* src_node;
    const RobNode* upvector_node;
    const obj_skin_ex_node_constraint* data;
    uint32_t dst_node_id;
    uint32_t src_node_id;

    ExConstraintBlock();
    virtual ~ExConstraintBlock() override;

    virtual void init() override;
    virtual void CtrlBegin() override;
    virtual void CtrlStep(int32_t stage, bool disable_ex_force) override;
    virtual void ctrl() override;
    virtual void CtrlOsagePlayData() override;
    virtual void disp(const mat4& mat, render_context* rctx) override;
    virtual void disp_debug() override;
    virtual void pos_init() override;
    virtual void pos_init_cont() override;

    void Calc();
    void CalcConstraintDirection(mat4 mat);
    void CalcConstraintDistance(mat4 mat);
    void CalcConstraintOrientation(mat4 mat);
    void CalcConstraintPosition(mat4 mat);
    void CalcMatrixHS();

    void set_data(const RobSkinDisp* skin_disp, const obj_skin_ex_node_constraint* data,
        const char* name, const bone_database* bone_data);
};

union Expr_value {
    float_t constant;
    const float* variable;
    float_t(*func1)(float_t);
    float_t(*func2)(float_t, float_t);
    float_t(*func3)(float_t, float_t, float_t);
};

struct Expr_node {
    Expr_type type;
    Expr_value data;
    Expr_node* operand[3];

    float_t eval();
};

class ExExpressionBlock : public ExNodeBlock {
public:
    float_t* result[9];
    Expr_type result_type[9];
    Expr_node* expr_node[9];
    Expr_node expr_work[384];
    const obj_skin_ex_node_expression* data;
    bool field_3D20;
    void(*hard_coded_func)(RobTransform*);
    float_t timer;
    bool step;

    ExExpressionBlock();
    virtual ~ExExpressionBlock() override;

    virtual void init() override;
    virtual void CtrlBegin() override;
    virtual void CtrlStep(int32_t stage, bool disable_ex_force) override;
    virtual void ctrl() override;
    virtual void CtrlOsagePlayData() override;
    virtual void disp(const mat4& mat, render_context* rctx) override;
    virtual void disp_debug() override;
    virtual void pos_init() override;
    virtual void pos_init_cont() override;

    void Calc();
    void CalcMatrixHS();

    void set_data(const RobSkinDisp* skin_disp, const obj_skin_ex_node_expression* data,
        const char* node_name, object_info objuid, size_t index, const bone_database* bone_data);
};

class RobDisp;

class RobSkinDisp {
public:
    size_t index;
    const mat4* motion_matrix;
    object_info obj_uid;
    object_info obj_uid_sub;
    std::vector<TexChange> texchg_vec;
    RobSkinCol skn_col;
    RobSkinOfs skn_ofs;
    float_t alpha;
    mdl::ObjFlags obj_flags;
    bool can_disp;
    uint32_t bone_kind;
    mat4* mat;
    int32_t init_cnt;
    const RobNode* motion_node;
    std::vector<ExNodeBlock*> ex_node_block;
    std::vector<RobNode> ex_node;
    std::vector<mat4> matrix;
    std::vector<mat4> no_scale_matrix;
    prj::vector_pair<const char*, uint32_t> node_name_map;
    int64_t field_138;
    std::vector<ExNullBlock*> null_blk;
    std::vector<ExOsageBlock*> osage_blk;
    std::vector<ExConstraintBlock*> constraint;
    std::vector<ExExpressionBlock*> expression;
    std::vector<ExClothBlock*> cloth;
    bool osage_depends_on_others;
    size_t osage_nodes_count;
    bool use_opd;
    const obj_skin_ex_data* skin_ex_data;
    const obj_skin* skin;
    const RobDisp* rob_disp;

    RobSkinDisp();
    ~RobSkinDisp();

    void add_motion_reset_data(const uint32_t& motnum, const float_t& frame, int32_t init_cnt);
    void check_no_opd(std::vector<opd_blend_data>& opd_blend_data);
    void dest_ex_node();
    void disp(const mat4& mat, render_context* rctx);
    const RobNode* get_node(int32_t index) const;
    const RobNode* get_node(const char* name, const bone_database* bone_data) const;
    int32_t get_node_index(const char* name, const bone_database* bone_data) const;
    const mat4* get_ex_data_bone_node_mat(const char* name);
    RobJointNode* get_normal_ref_osage_node(const std::string& str, size_t* index);
    void init_members(size_t index = 0xDEADBEEF);
    void pos_reset(int32_t init_cnt);
    void reset_ex_force();
    void reset_nodes_ex_force(rob_osage_parts_bit parts_bits);
    void set(object_info objuid, const RobNode* mot_node,
        bool osage_reset, const bone_database* bone_data, void* data, const object_database* obj_db);
    void set_alpha_obj_flags(float_t alpha, int32_t flags);
    bool set_boc(const skin_param_osage_root& skp_root, ExOsageBlock* osg);
    void set_col_color(const vec3& blend, const vec3& ofs);
    void set_col_specular(const vec3& blend, const vec3& ofs);
    void set_col_type(int32_t type);
    void set_collision_target_osage(const skin_param_osage_root& skp_root, skin_param* skp);
    void set_disable_collision(rob_osage_parts_bit parts_bits, bool disable);
    void set_ex_node(obj_skin_ex_data* ex_data);
    void set_ex_node_block(obj_skin_ex_data* ex_data,
        const bone_database* bone_data, void* data, const object_database* obj_db);
    void set_motion_node(const RobNode* mot_node, const bone_database* bone_data);
    void set_motion_reset_data(const uint32_t& motnum, const float_t& frame);
    void set_motion_skin_param(int8_t rob_id, uint32_t motnum, int32_t frame);
    void set_ofs(const vec3& pos, const vec3& rot, const vec3& scale);
    void set_osage_play_data_init(const float_t* opdi_data);
    void set_osage_reset();
    void set_osage_move_cancel(const float_t& mv_ccl);
    void set_tex_change(size_t num, const TexChange* texchg);
    void skp_load(void* kv, const bone_database* bone_data);
    void skp_load(const skin_param_osage_root& skp_root, std::vector<skin_param_osage_node>& vec,
        skin_param_file_data* skp_file_data, const bone_database* bone_data);
    bool skp_load_boc(const skin_param_osage_root& skp_root, std::vector<RobJointNodeData>* node_data);
    void skp_load_file(void* data, const bone_database* bone_data, const object_database* obj_db);
    bool skp_load_normal_ref(const skin_param_osage_root& skp_root, std::vector<RobJointNodeData>* node_data);
};

class RobDisp {
public:
    const RobNode* motion_node;
    const mat4* motion_matrix;
    RobSkinDisp* skin_disp;
    int32_t parts_attr[RPK_MAX];
    bool one_skin;
    int32_t disp_begin;
    int32_t disp_max;
    uint32_t shadow_flag;
    SHADOW_GROUP shadow_group;
    vec3 position;
    std::vector<TexChange> hyoutan_texchg_list;
    object_info hyoutan_obj;
    int32_t hyoutan_rpk;
    bool disable_update;
    HyoutanStat hyoutan_status;
    vec4 skin_color;
    float_t wet_cloth;
    float_t wind_strength;
    bool chara_color;
    bool npr_flag;
    mat4 mat;
    mat4 parts_matrix[RPK_MAX];
    std::vector<void*> shadow_obj_list;
    std::vector<void*> shadow_vb_list;
    obj_skin* shadow_skin;
    float_t osage_step;
    bool use_opd;
    std::vector<opd_blend_data> opd_blend_data;
    bool parts_short;
    bool parts_append;
    bool parts_white_one_l;

    RobDisp();
    ~RobDisp();

    void add_motion_reset_data(const uint32_t& motnum, const float_t& frame, int32_t init_cnt);
    void disp(int32_t rob_id, render_context* rctx);
    object_info get_objid(ROB_PARTS_KIND rpk) const;
    RobSkinDisp* get_skin_work(ROB_PARTS_KIND rpk); // Added
    const RobSkinDisp* get_skin_work(ROB_PARTS_KIND rpk) const;
    const mat4* get_ex_data_bone_node_mat(ROB_PARTS_KIND rpk, const char* name);
    ROB_PARTS_KIND get_free_item(ROB_PARTS_KIND rpk) const;
    void pos_reset(uint8_t init_cnt);
    void reset();
    void reset_ex_force();
    void reset_init_data(RobNode* mot_node);
    void reset_nodes_ex_force(rob_osage_parts parts);
    void set_alpha_obj_flags(float_t alpha, mdl::ObjFlags flags);
    void set_base(ROB_PARTS_KIND rpk, object_info obj_uid, bool osage_reset,
        const bone_database* bone_data, void* data, const object_database* obj_db);
    void set_body(object_info obj_uid,
        const bone_database* bone_data, void* data, const object_database* obj_db);
    void set_col_color(ROB_PARTS_KIND rpk, const vec3& blend, const vec3& ofs);
    void set_col_specular(ROB_PARTS_KIND rpk, const vec3& blend, const vec3& ofs);
    void set_col_type(ROB_PARTS_KIND rpk, int32_t type);
    void set_disable_collision(rob_osage_parts parts, bool disable);
    void set_disp_flag(ROB_PARTS_KIND rpk, bool flag);
    void set_hyoutan(ROB_PARTS_KIND rpk, object_info obj_uid, object_info obj_uid_sub,
        const bone_database* bone_data, void* data, const object_database* obj_db);
    void set_hyoutan_tex_change(const TexChange* texchg, int32_t tex_num);
    void set_item(ROB_PARTS_KIND rpk, object_info obj_info,
        const bone_database* bone_data, void* data, const object_database* obj_db);
    void set_merge(ROB_PARTS_KIND rpk, bool flag);
    void set_motion_node(const RobNode* mot_node, const bone_database* bone_data);
    void set_motion_reset_data(const uint32_t& motnum, const float_t& frame);
    void set_motion_skin_param(int8_t chara_id, uint32_t motnum, int32_t frame);
    void set_ofs(ROB_PARTS_KIND rpk, const vec3& trans, const vec3& rot, const vec3& scale);
    void set_one_skin(bool value);
    void set_opd_blend_data(std::list<motion_blend_mot*>* a2);
    void set_osage_play_data_init(ROB_PARTS_KIND rpk, const float_t* opdi_data);
    void set_osage_reset();
    void set_osage_step(float_t value);
    void set_osage_move_cancel(uint8_t id, const float_t& mv_ccl);
    void set_reflect(ROB_PARTS_KIND rpk, bool flag);
    void set_shadow(ROB_PARTS_KIND rpk, bool flag);
    void set_shadow_group(int32_t rob_id);
    void set_skin(object_info obj_uid, ROB_PARTS_KIND kind, bool osage_reset,
        const bone_database* bone_data, void* data, const object_database* obj_db);
    void set_tex_change(ROB_PARTS_KIND rpk, const TexChange* texchg, int32_t tex_num);
    void skp_load(ROB_PARTS_KIND rpk, const skin_param_osage_root& skp_root, std::vector<skin_param_osage_node>& vec,
        skin_param_file_data* skp_file_data, const bone_database* bone_data);
};

struct RobItemTXHD {
    texture* src;
    texture* dst;
    bool dst_copy;

    RobItemTXHD();
    ~RobItemTXHD();
};

struct RobItemHavePart {
    std::vector<uint32_t> item_no;

    RobItemHavePart() = default;
    ~RobItemHavePart() = default;
};

struct RobItemHave {
    RobItemHavePart part[ROB_ITEM_EQUIP_ID_MAX];

    RobItemHave() = default;
    ~RobItemHave() = default;
};

struct RobItemHaveSub {
    RobItemHavePart part[ROB_ITEM_EQUIP_SUB_ID_MAX];

    RobItemHaveSub() = default;
    ~RobItemHaveSub() = default;
};

typedef prj::vector_pair_combine<std::string, RobItemEquip> RobItemDbgSet;

class RobItem {
private:
    CHARA_NUM m_cn;
    CHARA_NUM m_cn_load;
    RobItemEquip m_equip;
    RobItemEquip m_equip_load;
    std::map<uint32_t, std::vector<RobItemTXHD>> m_txhd_map;
    std::map<ROB_PARTS_KIND, std::vector<uint32_t>> m_nude_attr_map;
    std::map<object_info, ROB_PARTS_KIND> m_rpk_map;
    std::map<int32_t, ROB_PARTS_KIND> m_rpk_sp_map;
    std::vector<TexChange> m_texchg_list[RPK_MAX];
    std::map<int32_t, object_info> m_head_map;

private:
    void equip_phase0(RobItemEquip* item_set);
    void equip_phase1(RobDisp* rdp,
        const bone_database* bone_data, void* data, const object_database* obj_db);
    void equip_phase2(RobDisp* rdp, RobItemEquip* item_set,
        const bone_database* bone_data, void* data, const object_database* obj_db);
    void equip_phase3(RobDisp* rdp, RobItemEquip* item_set,
        const bone_database* bone_data, void* data, const object_database* obj_db);
    void equip_phase4(RobDisp* rdp, RobItemEquip* item_set,
        const bone_database* bone_data, void* data, const object_database* obj_db);
    void equip_phase5(RobDisp* rdp, RobItemEquip* item_set);
    void equip_phase6(RobDisp* rdp, RobItemEquip* item_set);
    void equip_phase6(RobDisp* rdp, RobItemEquip* item_set, uint32_t item_no);
    void equip_phase7();
    ROB_PARTS_KIND get_rpk_item(ROB_ITEM_EQUIP_SUB_ID id) const;
    void hide_rpk_sp(RobDisp* rdp, int32_t rpk_sp,
        const bone_database* bone_data, void* data, const object_database* obj_db);
    void make_chg_tex(uint32_t item_no, const RobItemTable* tbl);
    void free_copy_texture_all();
    void init_nude_attr_map();
    void make_nude_attr_map(uint32_t item_no, const RobItemTable* tbl);
    void set_obj_phase2(RobDisp* rdp, uint32_t item_no, const RobItemTable* tbl, ROB_ITEM_EQUIP_SUB_ID id,
        const bone_database* bone_data, void* data, const object_database* obj_db);
    void set_obj_phase3(RobDisp* rdp, uint32_t item_no, const RobItemTable* tbl,
        const bone_database* bone_data, void* data, const object_database* obj_db);
    void set_texture(RobDisp* rdp,
        const std::vector<uint32_t>& item_nos, ROB_PARTS_KIND rpk, bool is_hyoutan = false);
    void set_texture(RobDisp* rdp, uint32_t item_no, ROB_PARTS_KIND rpk, bool is_hyoutan = false);
    void reset_texture(RobDisp* rdp, ROB_PARTS_KIND rpk, bool is_hyoutan = false);
    void clear_texchg_list();
    void disp_obj_internal(object_info obj_uid, mat4& mat, uint32_t item_no, render_context* rctx);
    //static bool s_check_equip_exclusion(CHARA_NUM cn, RobItemEquip*);
    //static bool s_check_equip_sub(CHARA_NUM cn, int32_t, const RobItemHeader*, RobItemEquip*, const PlayerInformation*);
    //static bool s_check_equip_sub_phase0(uint32_t, const RobItemHeader*);
    //static bool s_check_equip_sub_phase1(CHARA_NUM cn, uint32_t);
    //static bool s_check_equip_sub_phase2(CHARA_NUM cn, uint32_t, int32_t);
    //static bool s_check_equip_sub_phase3(CHARA_NUM cn, uint32_t, const pd::PlayerBitfield<2000>&);
    //static bool s_check_equip_sub_phase4(CHARA_NUM cn, uint32_t, int32_t);
    //static bool s_check_equip_sub_phase5(CHARA_NUM cn, int32_t, uint32_t);
    //static bool s_check_equip_chara(CHARA_NUM, int32_t, RobItemEquip*);
    //static bool s_check_equip_point(CHARA_NUM, const RobItemHeader*, const RobItemEquip*);
    static bool s_repair_equip(CHARA_NUM, uint32_t, int32_t, RobItemEquip* item_set);

    static int32_t s_have_dbg_ref;
    static RobItemHave s_have_dbg[];
    static int32_t s_have_sub_dbg_ref;
    static RobItemHaveSub s_have_sub_dbg[];

public:
    RobItem();
    ~RobItem();
    void set_chara_num(CHARA_NUM cn);
    void req_obj(uint32_t item_no, void* data, const object_database* obj_db);
    bool wait_obj(uint32_t item_no);
    void free_obj(uint32_t item_no);
    void req_obj_all(void* data, const object_database* obj_db);
    bool wait_obj_all();
    void free_obj_all();
    void free_obj_diff();

private:
    void regist_item(ROB_ITEM_EQUIP_SUB_ID id, uint32_t item_no);

public:
    void regist_item_one(uint32_t item_no);
    void regist_item_all(const RobItemEquip* item_set);
    void delete_item(uint32_t item_no);
    void equip(int32_t rc, const bone_database* bone_data,
        void* data, const object_database* obj_db);
    ROB_ITEM_EQUIP_ID get_equip_id(uint32_t item_no) const;
    ROB_ITEM_EQUIP_SUB_ID get_equip_sub_id(uint32_t item_no) const;
    uint32_t check_exclusive_item(uint32_t item_no) const;
    RobItemEquip* get_equip();
    const RobItemEquip* get_equip() const;
    bool is_equipped_item(uint32_t item_no) const;
    //const char* get_name(uint32_t item_no) const;
    //const char* get_name_asc(uint32_t item_no) const;
    void init_disp_obj(uint32_t item_no);
    void disp_obj(uint32_t item_no, render_context* rctx, const mat4& mat);
    void get_item_texchange_list(uint32_t item_no, std::vector<TexChange>& tex_chg_list);
    RobItemEquip* get_equip_load();
    const RobItemEquip* get_equip_load() const;

    void regist_item_all(const RobItemEquipInit* item_set);
    void free_copy_texture(uint32_t item_no);
    void free_copy_texture(std::map<uint32_t, std::vector<RobItemTXHD>>::iterator elem);

    bool check_for_npr_flag() const;
    object_info get_head_object_replace(int32_t head_object_id) const;
    float_t get_face_depth() const;

    static const char* s_get_equip_sub_id_str(ROB_ITEM_EQUIP_SUB_ID id);
    //static bool s_check_equip(CHARA_NUM cn, int32_t, RobItemEquip*, const void*);
    //static void s_get_objset(CHARA_NUM cn, uint32_t, int32_t*, size_t);
    static const char* s_get_name(const CHARA_NUM& cn, uint32_t item_no);
    static ROB_ITEM_EQUIP_ID s_get_equip_id(CHARA_NUM cn, uint32_t item_no);
    static ROB_ITEM_EQUIP_SUB_ID s_get_equip_sub_id(CHARA_NUM cn, uint32_t item_no);
    static void s_get_equip_sub_group(int32_t id, std::vector<int32_t>* sub_group);
    static bool s_is_replace_part(CHARA_NUM cn, uint32_t item_no, ROB_ITEM_EQUIP_SUB_ID id);
    static bool s_equip_ok(CHARA_NUM cn, uint32_t item_no, ROB_ITEM_EQUIP_SUB_ID id);
    //static bool s_have_costume(CHARA_NUM cn, int32_t, const void*);
    static uint32_t s_get_exclusion(CHARA_NUM cn, uint32_t item_no);
    //static uint32_t s_get_set_item_no(CHARA_NUM cn, uint32_t item_no);
    static uint32_t s_get_point(CHARA_NUM cn, uint32_t item_no);
    static void s_get_offset_list(CHARA_NUM cn, uint32_t item_no, std::vector<RobItemDataOfs>& ofs_list, bool clear);
    static ROB_ITEM_TYPE s_get_type(CHARA_NUM cn, uint32_t item_no);
    static bool s_is_texorg(CHARA_NUM cn, uint32_t item_no);
    static bool s_is_objorg(CHARA_NUM cn, uint32_t item_no);
    //static void s_get_ng_item();
    //static void s_get_ng_item_name();
    //static void s_get_ng_item_point();
    //static bool s_check_ng_item(CHARA_NUM cn, uint32_t item_no);
    static std::string s_check_ng_item_name(CHARA_NUM cn, uint32_t item_no);
    static int32_t s_check_ng_item_point(CHARA_NUM cn, uint32_t item_no);
    static uint32_t get_dbgset_num(CHARA_NUM cn);
    static const RobItemDbgSet* get_dbgset(CHARA_NUM cn);
    static void init_have_dbg();
    static void dest_have_dbg();
    static void init_have_sub_dbg();
    static void dest_have_sub_dbg();
    static RobItemHave* get_have_dbg(CHARA_NUM cn);
    static RobItemHaveSub* get_have_sub_dbg(CHARA_NUM cn);

    static void s_req_obj(CHARA_NUM cn, uint32_t item_no, void* data, const object_database* obj_db);
    static bool s_wait_obj(CHARA_NUM cn, uint32_t item_no);
    static void s_free_obj(CHARA_NUM cn, uint32_t item_no);
    static void s_req_obj_all(CHARA_NUM cn, const RobItemEquip* item_set, void* data, const object_database* obj_db);
    static bool s_wait_obj_all(CHARA_NUM cn, const RobItemEquip* item_set);
    static void s_free_obj_all(CHARA_NUM cn, const RobItemEquip* item_set);

    static void s_regist_item_one(CHARA_NUM cn, uint32_t item_no, RobItem* rob_item);
    static void s_regist_item_all(CHARA_NUM cn, const RobItemEquip* item_set, RobItem* rob_item);
    static void s_delete_item(CHARA_NUM cn, uint32_t item_no, RobItem* rob_item);
};

union RobFlag {
    uint32_t u32;
    struct {
        int8_t disp : 1;
        int8_t fix_hara : 1;
        int8_t ringout : 1;
        int8_t bound_wall : 1;
        int8_t compel : 1;
        int8_t pl_syagami : 1;
        int8_t land : 1;
        int8_t old_fix_hara : 1;
        int8_t invincible : 1;
        int8_t yokerare : 1;
        int8_t yokerare_mot : 1;
        int8_t wall_hit_ok : 1;
        int8_t own_down_pose : 1;
        int8_t time_up : 1;
        int8_t kabe_yoroke : 1;
        int8_t jump_rise : 1;
        int8_t round_loser : 1;
        int8_t wall_fix_ok : 1;
        int8_t wall_fix_ng : 1;
        int8_t not_normal : 1;
        int8_t old_not_normal : 1;
        int8_t mk_change_off : 1;
        int8_t wall_moved_mot : 1;
        int8_t dmy_yokerare : 1;
        int8_t yoketa : 1;
        int8_t yokerare_mot2 : 1;
        int8_t mk_yoke_off : 1;
        int8_t set_damage : 1;
        int8_t rf_28 : 1;
        int8_t rf_29 : 1;
        int8_t rf_30 : 1;
        int8_t no_ctrl : 1;
    } bit;
};

struct RobJump {
    float_t frame;
    float_t zvec;
    float_t next_land_frame;
    float_t land_time;

    RobJump();

    void init();
};

union RobAttackResult {
    uint32_t u32;
    struct {
        int8_t hit : 1;
        int8_t down : 1;
        int8_t counter : 1;
        int8_t l_counter : 1;
        int8_t m_counter : 1;
        int8_t mf_5 : 1;
        int8_t air : 1;
        int8_t air_counter : 1;
        int8_t finish : 1;
        int8_t finish_air : 1;
        int8_t guard : 1;
        int8_t guard_hazushi : 1;
        int8_t guard_half : 1;
        int8_t yoroke : 1;
        int8_t down_attack : 1;
        int8_t land_attack : 1;
        int8_t mf_16 : 1;
        int8_t hajiki : 1;
        int8_t follow_hit : 1;
        int8_t normal_hit : 1;
        int8_t ura_hit : 1;
        int8_t side_counter : 1;
        int8_t side_l_counter : 1;
        int8_t side_m_counter : 1;
        int8_t yoroke_l_counter : 1;
        int8_t side_hit : 1;
        int8_t yoroke_counter : 1;
        int8_t yoroke_follow : 1;
        int8_t stun_begin : 1;
        int8_t stun_hit : 1;
        int8_t syagami_hit : 1;
        int8_t bd_counter : 1;
    } bit;
};

union RobAttackFlag {
    uint32_t u32;
    struct {
        int8_t hit_ng : 1;
        int8_t both_hit : 1;
        int8_t hit_yokerare : 1;
        int8_t old_hit_yokerare : 1;
    } bit;
};

struct RobAttack {
    int16_t hit_timer;
    RobAttackFlag flag;
    RobAttackResult ar_flag;
    RobAttackResult ar_mask;
    RobAttackResult old_ar_flag;
    RobAttackResult old_ar_mask;
    RobAttackResult ar_flag_c;
    RobAttackResult ar_mask_c;
    bool first_hit_result;
    RobAttackResult cmb_first_ar_mask;
    int32_t cmb_node_cnt;
    bool next_attack_wait;
    int8_t sideturn_stop_counter;

    RobAttack();

    void init();
};

struct ActParam {
    uint32_t flag;
    uint32_t type;

    ActParam();

    void init();
};

struct RobActType {
    ACT_NAME name;
    ActParam act_param;
    uint32_t motnum;
    bool mirror;

    RobActType();

    void setup();
};

struct RobGuard {
    GuardKind guard_kind;
    prj::BitArray<5> guard_input;

    RobGuard();

    void init();
};

struct RobYarare {
    int16_t get_damage;
    float_t get_power;
    RobAngle yarare_yang;
    RobAttackResult hit_ar_mask;
    vec3 efc_spd;
    vec3 get_spd;
    vec3 ringout_spd;
    int16_t combo_count;
    int16_t combo_damage;
    int16_t down_combo_count;
    int16_t down_combo_damage;
    int16_t down_combo_count2;
    int16_t down_combo_damage2;
    int16_t down_combo_count3;
    int16_t down_combo_count4;
    int16_t nage_combo_count;
    int16_t nage_combo_damage;
    int16_t air_combo_count;
    int16_t air_combo_damage;
    uint32_t combo_flag;
    bool combo_xang_offs;
    std::list<void*> field_58;
    int16_t max_nowall_combo_count;
    int16_t max_nowall_combo_damage;
    int16_t max_wall_combo_count;
    int16_t max_wall_combo_damage;
    int16_t max_current_combo_count;
    int16_t max_current_combo_damage;
    int16_t mot_get_damage;
    int16_t real_get_damage;
    int16_t orig_get_damage;
    float_t gs_resist_timer;
    float_t gs_resist_step;

    RobYarare();
    ~RobYarare();

    void init();
};

struct RobNage {
    float_t disable_timer;
    int8_t nagerare_cnt;
    int16_t kaeshi_damage;
    rob_chara* enemy;
    bool right_side;
    RobAngle tgt_yang;
    vec3 tgt_pos;
    float_t tgt_timer;

    RobNage();

    void init();
};

struct RobRise {
    uint32_t motnum;
    float_t hurry;
    float_t down_timer;

    RobRise();

    void init();
};

union RobUkemiKind {
    uint32_t u32;
    struct {
        int8_t ukemi_ok : 1;
        int8_t ukemi_wait : 1;
        int8_t ukemi_wait_just : 1;
        int8_t ukemi_dir : 1;
        int8_t ukemi_now : 1;
        int8_t ukemi_air : 1;
        int8_t ukemi_select : 1;
        int8_t ukemi_land : 1;
        int8_t ukemi_delay : 1;
        int8_t ukemi_dir2 : 1;
    } bit;
};

struct RobUkemi{
    RobUkemiKind ukemi_kind;
    float_t land_elapsed;

    RobUkemi();

    void init();
};

struct RobOffensiveMove {
    float_t offmv_timer;
    bool revise_grd_emy_no;
    vec3 target_pos;
    union {
        uint32_t u32;
        struct {
            int8_t en_from_right : 1;
        } bit;
    } target_flag;

    RobOffensiveMove();

    void init();
};

struct RobSideTurn {
    float_t side_stop_counter;
    float_t side_guard_disable;
    float_t side_turn_timer;
    bool right_side_turn;

    RobSideTurn();

    void init();
};

struct RobAction {
    int8_t step;
    RobActType action;
    RobActType old_action;
    RobActType command;
    RobActType bak_command;
    RobActType sys_command;
    RobJump jump;
    RobAttack attack;
    RobGuard guard;
    int32_t field_B0;
    bool field_B4;
    RobYarare yarare;
    RobNage nage;
    RobRise rise;
    RobUkemi ukemi;
    RobOffensiveMove off_move;
    RobSideTurn side_turn;
    uint8_t kamae_type;
    const RobKamae* kamae_data;
    float_t down_counter;
    float_t down_counter_ex;
    float_t old_down_counter;
    uint32_t mot_backup;
    prj::BitArray<36> kaeshi_flag;
    int16_t gacha_count;
    bool gacha;
    int32_t drank_count;
    int32_t drank_add_req;
    float_t wall_yoroke_timer;

    RobAction();
    ~RobAction();

    void init();
};

class SubActParam {
public:
    SubActParamType type;

    SubActParam(SubActParamType type);
    virtual ~SubActParam();
};

class SubActParamAngry : public SubActParam {
public:
    SubActParamAngry();
    virtual ~SubActParamAngry();
};

class SubActParamCountNum : public SubActParam {
public:
    int32_t field_10;

    SubActParamCountNum();
    virtual ~SubActParamCountNum();
};

class SubActParamCry : public SubActParam {
public:
    SubActParamCry();
    virtual ~SubActParamCry();
};

class SubActParamEmbarrassed : public SubActParam {
public:
    SubActParamEmbarrassed();
    virtual ~SubActParamEmbarrassed();
};

class SubActParamLaugh : public SubActParam {
public:
    SubActParamLaugh();
    virtual ~SubActParamLaugh();
};

class SubActParamShakeHand : public SubActParam {
public:
    int32_t field_10;

    SubActParamShakeHand();
    virtual ~SubActParamShakeHand();
};

class SubActExec {
public:
    SubActExecType type;
    int32_t field_C;

    SubActExec(SubActExecType type);
    virtual ~SubActExec();

    virtual void init() = 0;
    virtual void set(const SubActParam* param) = 0;
    virtual void Field_18(rob_chara* impl) = 0;
    virtual void Field_20(rob_chara* impl) = 0;
};

class SubActExecAngry : public SubActExec {
public:
    SubActExecAngry();
    virtual ~SubActExecAngry() override;

    virtual void init() override;
    virtual void set(const SubActParam* param) override;
    virtual void Field_18(rob_chara* impl) override;
    virtual void Field_20(rob_chara* impl) override;
};

class SubActExecCountNum : public SubActExec {
public:
    int32_t field_10;
    int32_t field_14;
    int32_t field_18;

    SubActExecCountNum();
    virtual ~SubActExecCountNum() override;

    virtual void init() override;
    virtual void set(const SubActParam* param) override;
    virtual void Field_18(rob_chara* impl) override;
    virtual void Field_20(rob_chara* impl) override;
};

class SubActExecCry : public SubActExec {
public:
    SubActExecCry();
    virtual ~SubActExecCry() override;

    virtual void init() override;
    virtual void set(const SubActParam* param) override;
    virtual void Field_18(rob_chara* impl) override;
    virtual void Field_20(rob_chara* impl) override;
};

class SubActExecEmbarrassed : public SubActExec {
public:
    int8_t field_10;
    SubActExecEmbarrassed();
    virtual ~SubActExecEmbarrassed() override;

    virtual void init() override;
    virtual void set(const SubActParam* param) override;
    virtual void Field_18(rob_chara* impl) override;
    virtual void Field_20(rob_chara* impl) override;
};

class SubActExecLaugh : public SubActExec {
public:
    SubActExecLaugh();
    virtual ~SubActExecLaugh() override;

    virtual void init() override;
    virtual void set(const SubActParam* param) override;
    virtual void Field_18(rob_chara* impl) override;
    virtual void Field_20(rob_chara* impl) override;
};

class SubActExecShakeHand : public SubActExec {
public:
    int32_t field_10;
    int32_t field_14;
    int8_t field_18;

    SubActExecShakeHand();
    virtual ~SubActExecShakeHand() override;

    virtual void init() override;
    virtual void set(const SubActParam* param) override;
    virtual void Field_18(rob_chara* impl) override;
    virtual void Field_20(rob_chara* impl) override;
};

struct RobSubAction {
    SubActExec* field_8;
    SubActExec* field_10;
    SubActParam* param;
    SubActExec* field_20;
    SubActExecCry cry;
    SubActExecShakeHand shake_hand;
    SubActExecEmbarrassed embarrassed;
    SubActExecAngry angry;
    SubActExecLaugh laugh;
    SubActExecCountNum count_num;

    RobSubAction();
    virtual ~RobSubAction();

    void dest();
    void init();
    void set(SubActParam* value);
};

struct RobMotionFrame {
    float_t f;
    float_t old_f;
    float_t req_f;
};

struct RobMotionStep {
    float_t f;
    float_t old_f;
    float_t req_f;
};

union RobMotFlag {
    uint32_t u32;
    struct {
        int8_t mot_end : 1;
        int8_t next_null : 1;
        int8_t next_mirror : 1;
        int8_t mirror : 1;
        int8_t change : 1;
        int8_t frame_change : 1;
        int8_t frame_ctrl : 1;
        int8_t ext_frame_req : 1;
        int8_t suspend : 1;
        int8_t resume : 1;
        int8_t frame_stop : 1;
        int8_t mf_11 : 1;
        int8_t mf_12 : 1;
        int8_t mf_13 : 1;
        int8_t mf_14 : 1;
        int8_t mf_15 : 1;
        int8_t mf_16 : 1;
        int8_t mf_17 : 1;
        int8_t mf_18 : 1;
        int8_t mf_19 : 1;
        int8_t mf_20 : 1;
        int8_t mf_21 : 1;
        int8_t mf_22 : 1;
        int8_t mf_23 : 1;
        int8_t mf_24 : 1;
        int8_t mf_25 : 1;
        int8_t mf_26 : 1;
        int8_t mf_27 : 1;
        int8_t mf_28 : 1;
        int8_t mf_29 : 1;
        int8_t mf_30 : 1;
        int8_t mf_31 : 1;
    } bit;
};

class RobPartialMotion {
public:
    struct Data {
        uint32_t motnum;
        MOTTABLE_TYPE mottbl_type;
        rob_partial_motion_playback_state playback_state;
        float_t frame;
        float_t step;
        float_t frame_max;
        float_t blend_duration;
        float_t blend_step;
        float_t blend_offset;
        float_t play_duration;
        const RobMotionFrame* frame_ptr;
        const RobMotionStep* step_ptr;
        rob_partial_motion_loop_state loop_state;

        inline Data() : motnum(-1), mottbl_type(), playback_state(ROB_PARTIAL_MOTION_PLAYBACK_NONE), frame(),
            step(), frame_max(), blend_duration(), blend_step(), blend_offset(), play_duration(),
            frame_ptr(), step_ptr(), loop_state(ROB_PARTIAL_MOTION_LOOP_NONE) {
            Reset();
        }

        inline ~Data() {

        }

        inline void Reset() {
            motnum = -1;
            mottbl_type = MTP_FACE_NULL;
            playback_state = ROB_PARTIAL_MOTION_PLAYBACK_NONE;
            frame = 0.0f;
            step = 1.0f;
            frame_max = 0.0f;
            blend_duration = 0.0f;
            blend_step = 1.0f;
            blend_offset = 1.0f;
            play_duration = 0.0f;
            frame_ptr = 0;
            step_ptr = 0;
            loop_state = ROB_PARTIAL_MOTION_LOOP_NONE;
        }
    } data;

    RobPartialMotion();
    virtual ~RobPartialMotion();

    virtual void Reset() = 0;

    bool CheckEnded();
    bool CheckPlaybackStateBackward();
    bool CheckPlaybackStateCharaMotion();
    bool CheckPlaybackStateDuration();
    bool CheckPlaybackStateForward();
    bool CheckPlaybackStateStop();
    bool CheckPlaybackStateValid();
    void GetFrameStep();
    void Step();
};

class RobFaceMotion : public RobPartialMotion {
public:
    RobFaceMotion();
    virtual ~RobFaceMotion() override;

    virtual void Reset() override;
};

class RobHandMotion : public RobPartialMotion {
public:
    RobHandMotion();
    virtual ~RobHandMotion() override;

    virtual void Reset() override;
};

class RobMouthMotion : public RobPartialMotion {
public:
    RobMouthMotion();
    virtual ~RobMouthMotion() override;

    virtual void Reset() override;
};

class RobEyesMotion : public RobPartialMotion {
public:
    RobEyesMotion();
    virtual ~RobEyesMotion() override;

    virtual void Reset() override;
};

class RobEyelidMotion : public RobPartialMotion {
public:
    RobEyelidMotion();
    virtual ~RobEyelidMotion() override;

    virtual void Reset() override;
};

template <typename T>
struct struc_405 {
    RobFaceMotion face;
    RobHandMotion hand_l;
    RobHandMotion hand_r;
    RobMouthMotion mouth;
    RobEyesMotion eyes;
    RobEyelidMotion eyelid;
    object_info head_object;
    object_info hand_l_object;
    object_info hand_r_object;
    object_info face_object;
    T field_1C0;
    float_t time;

    struc_405() : field_1C0(), time() {

    }

    ~struc_405() {

    }

    void Reset() {
        face.Reset();
        hand_l.Reset();
        hand_r.Reset();
        mouth.Reset();
        eyes.Reset();
        eyelid.Reset();
        head_object = object_info();
        hand_l_object = object_info();
        hand_r_object = object_info();
        face_object = object_info();
        field_1C0 = {};
        time = 0.0f;
    }
};

struct rob_chara_data_adjust {
    bool enable;
    float_t frame;
    float_t transition_frame;
    vec3 curr_ex_force;
    float_t curr_force;
    float_t curr_strength;
    uint32_t motnum;
    float_t set_frame;
    float_t transition_duration;
    int32_t type;
    int32_t cycle_type;
    bool ignore_gravity;
    vec3 ex_force;
    vec3 ex_force_cycle_strength;
    vec3 ex_force_cycle;
    float_t cycle;
    float_t phase;
    float_t force;
    float_t strength;
    float_t strength_transition;

    rob_chara_data_adjust();

    void reset();
};

struct rob_chara_data_hand_adjust {
    bool enable;
    int16_t scale_select;
    rob_chara_data_hand_adjust_type type;
    float_t current_scale;
    float_t scale;
    float_t duration;
    float_t current_time;
    float_t rotation_blend;
    float_t scale_blend;
    bool enable_scale;
    bool disable_x;
    bool disable_y;
    bool disable_z;
    vec3 offset;
    vec3 target;
    float_t arm_length;
    int32_t init_cnt;

    rob_chara_data_hand_adjust();

    void reset();
};

struct rob_chara_data_arm_adjust {
    bool enable;
    float_t value;
    float_t prev_value;
    float_t next_value;
    float_t duration;
    float_t frame;

    void reset();
};

struct RobMotion {
    uint32_t num;
    uint32_t old_num;
    RobMotionFrame frame;
    RobMotionStep step;
    float_t osage_step;
    MOT_BSTEP basic_step;
    RobMotFlag flag;
    int32_t repeat_count;
    prj::BitArray<23> shift_req;
    int16_t same_mot_idx;
    int16_t same_count;
    int32_t field_38[32];
    int32_t field_B8[32];
    float_t mot_adjust_scale;
    float_t mot_xz_adjust_scale;
    vec3 mot_adjust_base_pos;
    uint32_t face_motnum;
    struc_405<int32_t> field_150;
    RobHandMotion hand_l;
    RobHandMotion hand_r;
    object_info hand_l_object;
    object_info hand_r_object;
    struc_405<float_t> field_3B0;
    rob_chara_data_adjust parts_adjust[ROB_OSAGE_PARTS_MAX];
    rob_chara_data_adjust parts_adjust_prev[ROB_OSAGE_PARTS_MAX];
    rob_chara_data_adjust adjust_global;
    rob_chara_data_adjust adjust_global_prev;
    rob_chara_data_hand_adjust hand_adjust[2];
    rob_chara_data_hand_adjust hand_adjust_prev[2];
    rob_chara_data_arm_adjust arm_adjust[2];

    RobMotion();
    ~RobMotion();

    void reset();
};

struct CtrlDMot {
    CtrlDMot();

    uint32_t mot_id;
    uint32_t mot_flag;
};

class RobMotId : public CtrlDMot {
public:
    RobMotId();

    void clear();
    void set(uint32_t in_mot_id, uint32_t in_mot_flag);
};

struct RobMotDAs {
    float_t count;
    float_t follow2;
    float_t nagerenai;

    RobMotDAs();

    void clear();
};

struct MhdKind {
    uint32_t mhk_kind[4];
};

struct MhdList {
    int32_t type;
    void* data;
};

struct MhpList {
    int32_t type;
    int32_t frame;
    void* data;
};

struct MhcList {
    void* data;
};

struct MhdData {
    MhdKind mot_kind;
    uint16_t start_style;
    uint16_t end_style;
    MhdList* mh_list;
    MhpList* pp_list;
    MhcList* cm_list;

    MhdData();
};

struct MhdFileHeader {
    uint32_t file_id;
    uint32_t min_mot_id;
    uint32_t max_mot_id;
    MhdData** data_list;

    MhdFileHeader();
};

struct ES_DATA {
    int16_t est_land_xang;
    int16_t est_down_xang;
    int16_t est_down_air_xang;
    int16_t est_counter_down_xang;
    int16_t est_yoroke_down_xang;
    float_t est_power;
    float_t est_guard_power;
    float_t est_land_power;
    float_t est_down_power;
    float_t est_down_air_power;
    float_t est_counter_down_power;
    float_t est_yoroke_down_power;
    float_t est_finish_mul_yspd;
    float_t est_finish_mul_xzspd;
};

struct MhdAttackSound {
    int8_t mhats_efc_sound_type;
    int32_t mhats_efc_sound_code;
    int16_t mhats_efc_sound_cond;
};

struct MhdNagenuke {
    int16_t mhnu_type;
    uint32_t mhnu_lever_dir;
};

struct RobTarget {
    TargetType target_flag;
    float_t target_end;
    vec3 target_pos;

    RobTarget();
};

struct MotLeafCtrl {
    int16_t mode;
    float_t start_frame;
    float_t end_frame;
    int16_t target_id;
    int16_t limit;
    vec3 ofs;
    vec3 min;
    vec3 max;
    vec3 field_34;
    vec3 field_40;

    MotLeafCtrl();
    MotLeafCtrl(int16_t mode, float_t start_frame, float_t end_frame);
};

struct RobMotData {
    uint32_t mot;
    float_t frame_max;
    float_t frame;
    int16_t ex_damage_all;
    prj::BitArray<MK_KIND_MAX> motkind_fix;
    prj::BitArray<MK_KIND_MAX> motkind;
    prj::BitArray<MK_KIND_MAX> old_motkind_fix;
    prj::BitArray<MK_KIND_MAX> old_motkind;
    uint16_t start_style;
    uint16_t end_style;
    uint16_t end_style_flag;
    MhNextType next_type;
    RobMotId next;
    RobMotId next_end;
    int32_t next_limit;
    float_t loop_begin;
    float_t loop_end;
    float_t main_mot_frame;
    float_t follow1_frame;
    float_t follow2_frame;
    bool attack_flag;
    prj::BitArray<7> attack_kind;
    AtkUnit attack_unit;
    AtkPoint attack_point;
    int16_t original_damage;
    int16_t attack_damage;
    int16_t dist_min_damage;
    prj::BitArray<6> dc_type;
    RobAngle efc_yang;
    MhYarareType yarare_type;
    prj::BitArray<ETF_FLAG_MAX> efc_flag;
    MhEfcSpeedType efc_spd;
    float_t attack_ball_multi;
    float_t down_attack_multi;
    RobMotDAs field_B8[25];
    prj::BitArray<AK_KIND_MAX> air_kind;
    float_t jump_frame;
    float_t stop_frame;
    float_t land_frame;
    float_t ev_land_frame;
    float_t land_ypos;
    float_t gravity;
    RobMotId nagerare_mot;
    RobMotId nagerare_mot_mirror;
    float_t nage_ringout_frame;
    float_t nage_cancel_frame;
    float_t nage_kabe_follow2;
    int16_t nage_damage;
    std::list<MhdNagenuke*> nage_nuke_list;
    int16_t tag_jumph;
    float_t tag_xofs;
    float_t tag_zofs;
    int16_t shift_num;
    MhdList* shift_adr;
    float_t ukemi_start_ofs_frame;
    float_t ukemi_end_frame;
    float_t ukemi_exec_frame;
    uint32_t ukemi_flag;
    prj::BitArray<KT_TYPE_MAX> kaesare_type;
    prj::BitArray<KT_TYPE_MAX> kaeseru_type;
    MhKaeshiAttrib kaesare_attrib;
    RobAngle mov_yang;
    RobAngle old_mov_yang;
    prj::BitArray<3> gacha_flag;
    RobMotId gacha_mot;
    int16_t gacha_times;
    float_t gacha_frame;
    MhGuardType guard_type;
    prj::BitArray<GK_FLAG_MAX> guard_flag;
    float_t guard_frame;
    uint32_t frame_ctrl_type;
    float_t frame_ctrl_frame;
    int8_t frame_ctrl_tag_type;
    float_t frame_ctrl_tag_add;
    float_t frame_ctrl_min;
    float_t frame_ctrl_max;
    ES_DATA* est_adr;
    RobAngle dturn_yang;
    prj::BitArray<2> smooth_flag;
    float_t smooth_f_length;
    float_t smooth_r_length;
    float_t ride_on_ypos;
    std::vector<MhdAttackSound*> atk_snd_list;
    int16_t kabe_damage;
    MhKabeDmgType kabe_damage_type;
    prj::BitArray<21> nagereru_char;
    prj::BitArray<KT_TYPE_MAX> resist_type;
    int16_t resist_low_limit;
    int16_t resist_max_limit;
    int16_t resist_percent;
    MhResistDownChkType resist_down_chk;
    float_t resist_slow_frame;
    float_t resist_slow_step;
    MhEnDrinkOffType en_drink_off_type;
    int16_t en_drink_off_dec;
    float_t trans_xofs;
    float_t trans_yofs;
    float_t trans_zofs;
    float_t jump_zspd;
    float_t wall_haritsuki_frame;
    int32_t init_cnt;
    MhpList* play_prog;
    MhpList* play_prog_org;
    bool play_prog_x; // X
    prj::BitArray<17> revise_flag;
    uint32_t rob_cam_flag;
    RobTarget target;
    bool colliball_flag[3][ROB_COLLI_ID_MAX];
    float_t colliball_ratio[3][ROB_COLLI_ID_MAX];
    float_t colliball_timer[3][ROB_COLLI_ID_MAX];
    prj::BitArray<41> touch_nage_mask;
    float_t arm_adjust_next_value;  // X
    float_t arm_adjust_prev_value;  // X
    int32_t arm_adjust_start_frame; // X
    float_t arm_adjust_duration;    // X
    float_t mot_adjust_start;
    float_t mot_adjust_end;
    uint8_t mot_adjust_flag;
    MotAdjustType mot_adjust_type;
    float_t mot_adjust_scale;
    float_t pre_adjust_scale;
    float_t mot_adjust_base_ypos;
    float_t pre_adjust_base_ypos;
    float_t atk2_start_frame;
    float_t atk2_end_frame;
    bool atk2_set_flag;
    MotLeafCtrl leaf_ctrl[LCPART_MAX];
    MhcList* mh_command;
    int32_t mh_did;

    RobMotData();
    ~RobMotData();

    void clear();
    void mhd_pp_clear();
    void mhd_pp_exec(rob_chara* impl, float_t frame, const motion_database* mot_db);
    void mhd_smd_atk_clear();
    void mhd_smd_clear();
    void mhd_smd_exec(rob_chara* impl, MhdList* list);
};

struct RobPosition {
    RobAngle yang;
    RobAngle act_yang;
    RobAngle hara_xang;
    RobAngle hara_yang;
    RobAngle hara_zang;
    RobAngle revise_tag_yang;
    ReviseType revise_type;
    float_t ground_pos;
    float_t ground_last;
    vec3 pos;
    vec3 gpos;
    vec3 spd;
    vec3 adjust_spd;
    vec3 old_gpos;
    vec3 velocity;
    vec3 pos_camera_old;
    mat4 rob_mat;

    RobPosition();

    void reset();
};

struct RobAdjust {
    float_t scale;
    bool height_adjust;
    float_t pos_adjust_y;
    vec3 pos_adjust;
    vec3 offset;
    bool offset_x;
    bool offset_y;
    bool offset_z;
    bool get_global_pos;
    vec3 pos;
    mat4 mat;
    float_t left_hand_scale;
    float_t right_hand_scale;
    float_t left_hand_scale_default;
    float_t right_hand_scale_default;
    mat4 item_mat;      // X
    vec3 item_pos;      // X
    float_t item_scale; // X

    RobAdjust();

    void reset();
};

namespace prj {
    struct Sphere3f {
        vec3 c;
        float_t r;

        Sphere3f();
    };

    struct BallCollision {
        vec3 old_c;
        Sphere3f ball;
        float_t sink;
        int32_t colli_ball_mask;
        float_t org_rad;

        BallCollision();
    };
}

struct pos_scale {
    vec2 pos;
    float_t scale;

    pos_scale();

    float_t get_screen_pos_scale(const mat4& mat,
        const vec3& pos, float_t scale = 0.0f, bool apply_offset = false);
};

struct RobColliWall {
    float_t wdist;
    RobAngle wang;
    float_t wheight;
    RobAngle wyang;

    RobColliWall();

    void setup();
};

struct RobCollision {
    int8_t touch_count;
    int8_t wall_touch_cnt;
    uint32_t touch_mask;
    uint32_t attack_mask;
    uint32_t hit_mask;
    uint32_t damage_mask;
    uint32_t check_wall;
    uint32_t wall_hit_num;
    uint32_t wall_hit_mask;
    uint32_t handrail_hit_num;
    uint32_t handrail_hit_mask;
    uint32_t dummy_wall_hit_num;
    uint32_t dummy_wall_hit_mask;
    uint32_t dummy_ringout_num;
    uint32_t dummy_ringout_mask;
    uint32_t above_floor_num;
    uint32_t above_floor_mask;
    uint32_t floor_hit_num;
    uint32_t floor_hit_mask;
    uint32_t ringout_num;
    uint32_t ringout_mask;
    uint32_t ringout_air_num;
    uint32_t ringout_air_mask;
    float_t sink_floor;
    int32_t floor_idx;
    uint32_t fld_on;
    float_t sink_wall;
    int32_t wall_idx;
    vec3 wall_hit_pos;
    mat4 mat[ROB_COLLI_ID_MAX];
    mat4 push_mat[ROB_COLLI_ID_MAX];
    prj::BallCollision cb_hit[ROB_COLLI_ID_MAX];
    prj::BallCollision cb_rob[ROB_COLLI_ID_MAX];
    prj::BallCollision cb_stg[ROB_COLLI_ID_MAX];
    pos_scale field_1AA0[ROB_COLLI_ID_MAX];
    float_t field_1BE4[ROB_COLLI_ID_MAX];
    prj::vector_pair<int64_t, float_t> field_1C50;
    int64_t field_1C68;
    int64_t field_1C70;
    int64_t field_1C78;
    int64_t field_1C80;
    int32_t field_1C88[ROB_COLLI_ID_MAX];
    int32_t field_1CF4[ROB_COLLI_ID_MAX];
    int32_t field_1D60[ROB_COLLI_ID_MAX];
    int32_t field_1DCC;
    int32_t field_1DD0;
    int32_t field_1DD4;
    int32_t field_1DD8;
    float_t sink;
    float_t max_ypos;
    float_t min_ypos;
    int32_t max_idx;
    int32_t min_idx;
    float_t top_ypos;
    float_t bot_ypos;
    int32_t top_idx;
    int32_t bot_idx;
    vec3 moveF;
    vec3 moveR;
    vec3 moveRT;
    vec3 moveM;
    vec3 moveD;
    vec3 moveW[2];
    vec3 moveWd[2];
    vec3 moveWr[2];
    vec3 moveWT[2];
    vec3 moveT[2];
    RobColliWall wall_info[4];
    float_t ringout_dist[4];
    bool init_thrust_vec_req;
    vec3 thrust_vec;
    vec3 rep_hit_pos;
    bool req_wall_break;
    bool enable_dummy_collision;
    bool fall_ringout;
    bool req_kabe_yoroke;
    bool calc_rob_colli;
    bool colli_attack_hit;
    bool floor_ringout_disable;

    RobCollision();
    ~RobCollision();

    void init();
};

union RobEnInfoFlag {
    uint32_t u32;
    struct {
        int8_t en_from_behind : 1;
        int8_t en_from_right : 1;
        int8_t en_not_look_me : 1;
        int8_t en_down_attack : 1;
        int8_t en_down_attack2 : 1;
        int8_t en_down_nage : 1;
        int8_t en_front : 1;
        int8_t en_back : 1;
        int8_t en_right : 1;
        int8_t en_left : 1;
        int8_t en_at_front : 1;
        int8_t en_at_back : 1;
        int8_t en_at_right : 1;
        int8_t en_at_left : 1;
    } bit;
};

union RobInfoFlag {
    uint32_t u32;
    struct {
        int8_t disp_left : 1;
        int8_t calculated_disp_left : 1;
        int8_t death : 1;
        int8_t disp_left_cmd : 1;
        int8_t disp_left_cmd_base : 1;
    } bit;
};

struct RobInfo {
    float_t en_dist;
    float_t en_dist_3d;
    RobAngle en_dir_yang;
    RobAngle en_dir_yang2;
    RobAngle revise_en_dir_yang;
    RobEnInfoFlag en_flag;
    RobEnInfoFlag old_en_flag;
    RobInfoFlag flag;
    prj::BitArray<41> en_nage_flag;
    prj::BitArray<41> en_touch_nage_flag;
    float_t profit;
    float_t my_profit;

    RobInfo();

    void init();
};

struct RobRingOut {
    bool stop;
    bool down;
    bool stand;
    bool fall;
    bool air;
    bool air2down;
    bool set_fix_hara;
    bool replay_ringout;
    vec3 ringout_pos;
    vec3 ringout_spd;

    RobRingOut();

    void init();
};

struct RobSound {
    bool blow_flag;

    RobSound();

    void init();
};

class RobBase {
public:
    RobFlag flag;
    RobAction action;
    RobSubAction sub_action;
    RobMotion robmot;
    RobMotData motdata;
    RobPosition position;
    RobAdjust adjust;
    RobCollision collision;
    RobInfo robinfo;
    RobRingOut ringout;
    RobSound sound;
    float_t arm_adjust_scale; // X

    RobBase();
    ~RobBase();

    void init();
};

class rob_chara {
public:
    int8_t idnm;
    RobType type;
    int16_t energy;
    bool disp_pos_reset;
    bool disp_pos_reset_forbidden;
    CHARA_NUM chara_num;
    int32_t cos_id;
    float_t frame_speed;
    rob_chara* enemy;
    rob_chara_bone_data* bone_data;
    RobDisp* disp;
    RobItem item;
    RobBase rob_base;
    RobBase rob_base_old;
    const RobData* rob_data;
    RobInit rob_init;

    rob_chara();
    ~rob_chara();

    void adjust_ctrl();
    void arm_adjust_ctrl();
    void autoblink_disable();
    void autoblink_enable();
    void calc_mot_adjust_scale();
    RobAngle calc_mot_yang(bool compel_flag) const;
    bool check_disp_left() const;
    bool check_for_ageageagain_module();
    mat4* get_bone_data_mat(size_t index);
    uint32_t get_common_mot(MOTTABLE_TYPE mottbl_type) const;
    float_t get_frame() const;
    float_t get_frame_max() const;
    const vec3* get_gpos() const;
    object_info get_rob_data_face_object(int32_t index);
    float_t get_face_depth() const;
    float_t get_pos_scale(ROB_COLLI_ID colli_id, vec3& center);
    const RobInit* get_rob_init() const;
    void set_item(ROB_PARTS_KIND kind, object_info obj_uid,
        const bone_database* bone_data, void* data, const object_database* obj_db);
    void pos_reset();
    void rob_info_ctrl();
    void rob_motion_modifier_ctrl();
    bool replace_rob_motion(uint32_t motnum, float_t frame, float_t blend_duration,
        bool blend, bool set_motion_reset_data, MotionBlendType blend_type,
        const bone_database* bone_data, const motion_database* mot_db);
    void req_frame_change(float_t frame);
    void reset_rob(const RobInit& robinit,
        const bone_database* bone_data, const motion_database* mot_db);
    void reset_osage();
    void reset_osage_pos();
    void set_base(ROB_PARTS_KIND kind, object_info obj_uid, bool osage_reset,
        const bone_database* bone_data, void* data, const object_database* obj_db);
    void set_bone_data_frame(float_t frame);
    void set_chara(ROB_ID id, CHARA_NUM cn, int32_t cos_id, const RobInit& ri);
    void set_chara_color(bool value);
    void set_chara_height_adjust(bool value);
    void set_chara_pos_adjust(const vec3& value);
    void set_chara_pos_adjust_y(float_t value);
    void set_chara_size(float_t value);
    void set_data_adjust_mat(RobAdjust* rob_chr_adj, bool pos_adjust = true);
    void set_disable_collision(rob_osage_parts parts, bool disable);
    void set_disp_flag(bool flag);
    void set_eyelid_mottbl_motion(int32_t type, MOTTABLE_TYPE mottbl_type,
        float_t value, rob_partial_motion_playback_state playback_state, float_t blend_duration,
        float_t play_duration, float_t step, rob_partial_motion_loop_state loop_state,
        float_t blend_offset, const motion_database* mot_db);
    void set_eyelid_mottbl_motion_from_face(int32_t a2,
        float_t blend_duration, float_t value, float_t blend_offset, const motion_database* mot_db);
    void set_eyes_mottbl_motion(int32_t type, MOTTABLE_TYPE mottbl_type,
        float_t value, rob_partial_motion_playback_state playback_state, float_t blend_duration,
        float_t play_duration, float_t step, rob_partial_motion_loop_state loop_state,
        float_t blend_offset, const motion_database* mot_db);
    void set_face_mottbl_motion(int32_t type, MOTTABLE_TYPE mottbl_type,
        float_t value, rob_partial_motion_playback_state playback_state, float_t blend_duration, float_t play_duration,
        float_t step, rob_partial_motion_loop_state loop_state,
        float_t blend_offset, bool a11, const motion_database* mot_db);
    void set_face_object(object_info obj_info, int32_t a3);
    void set_face_object_index(int32_t index);
    void set_frame(float_t frame);
    void set_hand_item(int32_t uid, float_t blend_duration);
    void set_hand_item_l(int32_t uid);
    void set_hand_item_r(int32_t uid);
    void set_hand_l_mottbl_motion(int32_t type, MOTTABLE_TYPE mottbl_type,
        float_t value, rob_partial_motion_playback_state playback_state, float_t blend_duration,
        float_t play_duration, float_t step, rob_partial_motion_loop_state loop_state,
        float_t blend_offset, const motion_database* mot_db);
    void set_hand_r_mottbl_motion(int32_t type, MOTTABLE_TYPE mottbl_type,
        float_t value, rob_partial_motion_playback_state playback_state, float_t blend_duration,
        float_t play_duration, float_t step, rob_partial_motion_loop_state loop_state,
        float_t blend_offset, const motion_database* mot_db);
    void set_left_hand_scale(float_t value);
    void set_look_camera(bool update_view_point, bool rotation_enable, float_t head_rot_strength,
        float_t eyes_rot_strength, float_t blend_duration, float_t eyes_rot_step, float_t a8, bool ft);
    void set_look_camera_new(bool rotation_enable, float_t head_rot_strength,
        float_t eyes_rot_strength, float_t blend_duration, float_t eyes_rot_step, float_t a8);
    void set_look_camera_old(bool rotation_enable, float_t head_rot_strength,
        float_t eyes_rot_strength, float_t blend_duration, float_t eyes_rot_step, float_t a8);
    void set_motion_loop(float_t loop_begin, float_t loop_end, int32_t loop_count);
    void set_motion_reset_data(const uint32_t& motnum, const float_t& frame);
    void set_motion_skin_param(const uint32_t& motnum, const float_t& frame);
    void set_motion_step(float_t value);
    void set_mouth_mottbl_motion(int32_t type, MOTTABLE_TYPE mottbl_type,
        float_t value, rob_partial_motion_playback_state playback_state, float_t blend_duration,
        float_t play_duration, float_t step, rob_partial_motion_loop_state loop_state,
        float_t blend_offset, const motion_database* mot_db);
    void set_osage_move_cancel(uint8_t id, float_t mv_ccl);
    void set_osage_reset();
    void set_osage_step(float_t value);
    void set_parts_disp(ROB_PARTS_KIND kind, bool flag);
    void set_right_hand_scale(float_t value);
    void set_rob_motion(uint32_t motnum, bool mirror, float_t frame,
        MotionBlendType blend_type, const bone_database* bone_data, const motion_database* mot_db);
    void set_rob_motion_external(uint32_t motnum);
    void set_rob_pos(const vec3& pos);
    void set_rob_sys_command(ACT_NAME name, int32_t motion_id, const ActParam* param, bool mirror);
    void set_rob_yang(const RobAngle& ang);
    void set_shadow_cast(bool value);
    void set_stage_data_ring(const int32_t& stage_index);
    void set_step_motion_step(float_t value);
    void set_use_opd(bool value);
    void set_wind_strength(float_t value);

    inline bool get_disp_flag() {
        return !!rob_base.flag.bit.disp;
    }

    void sub_1405070E0(const bone_database* bone_data, const motion_database* mot_db);
    void sub_140509D30();
    void sub_1405163C0(int32_t index, mat4& mat);
    void sub_140551000();
};

class RobManagement {
public:
    static bool colli_check_on;
    static CHARA_NUM chara_num[];
    static int32_t instance_count;
    static rob_chara rob_impl[];
    static RobInit rob_init[];

    RobManagement();
    ~RobManagement();

    bool check_alive_rob(ROB_ID id);
    bool check_disp_left(ROB_ID id);
    bool check_ringout_only_rob(ROB_ID id);
    bool check_ringout_transit_rob(ROB_ID id);
    ROB_ID create_rob(CHARA_NUM cn, const RobInit& ri, int32_t cos_id, bool can_set_default);
    void dest_all();
    void dest_rob(ROB_ID id);
    float_t get_adjust_scale(ROB_ID id) const;
    CHARA_NUM get_chara(ROB_ID id) const;
    bool get_colli_check_on() const;
    bool get_disp_on(ROB_ID id) const;
    const vec3* get_gpos(ROB_ID id) const;
    rob_chara* get_rob(ROB_ID id);
    const RobData* get_rob_data(ROB_ID id) const;
    // RobDetail* get_rob_detail_if(ROB_ID id);
    rob_chara_bone_data* get_rob_motion_work(ROB_ID id);
    size_t get_rob_num() const;
    RobDisp* get_rob_robdisp_work(ROB_ID id);
    RobItem* get_rob_robitem_work(ROB_ID id);
    const RobInit* get_rob_init(ROB_ID id) const;
    bool is_init(ROB_ID id) const;
    void reset_osage_pos(ROB_ID id);
    void reset_rob(const RobInit& rob0, const RobInit& rob1,
        const bone_database* bone_data, const motion_database* mot_db);
    void reset_rob(ROB_ID id, const RobInit& ri,
        const bone_database* bone_data, const motion_database* mot_db);
    void set_colli_check_on(bool value);
    void set_disp_on(ROB_ID id, bool value);
    void set_mot_frame(ROB_ID id, float_t frame);
    void set_motion(ROB_ID id, int32_t motnum);
    void set_no_ctrl(ROB_ID id);
    void set_rob_pos(ROB_ID id, const vec3& pos);
    void set_rob_yang(ROB_ID id, const RobAngle& ang);

    static void init();
};

struct pv_data_set_motion {
    uint32_t motnum;
    std::pair<float_t, int32_t> frame_stage_index;

    pv_data_set_motion();
    pv_data_set_motion(uint32_t motnum);
    pv_data_set_motion(uint32_t motnum, std::pair<float_t, int32_t> frame_stage_index);
};

struct osage_init_data {
    rob_chara* rob_chr;
    int32_t pv_id;
    uint32_t motnum;
    std::string dir;
    int32_t frame;

    osage_init_data();
    osage_init_data(rob_chara* rob_chr, uint32_t motnum = -1);
    osage_init_data(rob_chara* rob_chr, int32_t pv_id,
        uint32_t motnum, const std::string& dir = "", int32_t frame = -1);
    ~osage_init_data();
};

struct OpdMakeManagerArgs {
    const std::vector<int32_t>* motion_ids;
    const std::vector<uint32_t>* modules;
    const std::vector<std::string>* objects;
    bool use_current_skp;
    bool use_opdi;
};

struct OpdMakeManagerData {
    struct Worker {
        uint32_t mode;
        uint32_t progress;
        CHARA_NUM chara;
        std::vector<uint32_t> items;

        Worker();
        ~Worker();
    };

    uint32_t mode;
    uint32_t left;
    uint32_t count;
    std::vector<Worker> workers;

    OpdMakeManagerData();
    ~OpdMakeManagerData();
};

struct OpdChecker {
    int32_t state;
    int64_t field_8;
    int64_t field_10;
    int64_t field_18;
    std::vector<std::string> objects;
    int32_t index;
    int32_t size;
    int32_t field_40;
    bool terminated;
    std::thread* thread;
    std::mutex state_mtx;
    std::mutex terminated_mtx;
    std::mutex index_mtx;

    OpdChecker();
    ~OpdChecker();

    bool CheckFileAdler32Checksum(const std::string& path);
    bool CheckFileVersion(const std::string& path, uint32_t version);
    bool CheckStateNot3();
    std::vector<std::string>& GetObjects();
    void GetIndexSize(int32_t& index, int32_t& size);
    int32_t GetState();
    bool GetTerminated();
    void LaunchThread();
    void SetIndexSize(int32_t index, int32_t size);
    void SetState(int32_t value);
    void SetTerminated();
    void TerminateThread();

    static void ThreadMain(OpdChecker* opd_checker);

    void sub_140471020();
};

extern RobManagement* get_rob_management();

extern const RobData* get_rob_data(CHARA_NUM cn);
extern int32_t rob_data_get_chara_size_index(CHARA_NUM cn);
extern int32_t rob_data_get_swim_costume(CHARA_NUM cn);
extern int32_t rob_data_get_swim_s_costume(CHARA_NUM cn);

extern float_t chara_pos_adjust_y_table_get_value(uint32_t index);
extern float_t chara_size_table_get_value(uint32_t index);

extern bool check_cos_id_is_501(int32_t cos_id);

const char* get_dev_ram_opdi_dir();

extern const float_t get_gravity();

extern const uint32_t* get_opd_motion_ids();
extern const uint32_t* get_opd_motion_set_ids();

extern const char* get_ram_osage_play_data_dir();
extern const char* get_ram_osage_play_data_tmp_dir();
extern const char* get_rom_osage_play_data_dir();
extern const char* get_rom_osage_play_data_opdi_dir();

extern OpdChecker* opd_checker_get();
extern bool opd_checker_check_state_not_3();
extern bool opd_checker_has_objects();
extern void opd_checker_launch_thread();
extern void opd_checker_terminate_thread();

extern void opd_make_manager_open(const OpdMakeManagerArgs& args);
extern bool opd_make_manager_check_alive();
extern bool opd_make_manager_close();
extern OpdMakeManagerData* opd_make_manager_get_data();

extern void opd_make_start();
extern void opd_make_start_get_motion_ids(std::vector<int32_t>& motion_ids);
extern void opd_make_stop();

extern bool osage_play_data_manager_open();
extern void osage_play_data_manager_append_chara_motion_id(rob_chara* rob_chr, uint32_t motnum);
extern void osage_play_data_manager_append_chara_motion_ids(
    rob_chara* rob_chr, const std::vector<uint32_t>& motion_ids);
extern bool osage_play_data_manager_check_alive();
extern void osage_play_data_manager_get_opd_file_data(object_info obj_info,
    uint32_t motnum, const float_t*& data, uint32_t& count);
extern void osage_play_data_manager_reset();

extern void osage_play_database_load();

extern void pv_osage_manager_array_reset(ROB_ID rob_id);

extern void rob_init();
extern void rob_free();

extern void rob_chara_age_age_array_ctrl(int32_t rob_id, int32_t part, mat4& mat);
extern void rob_chara_age_age_array_disp(render_context* rctx,
    int32_t rob_id, bool reflect, bool chara_color);
extern void rob_chara_age_age_array_load(int32_t rob_id, int32_t part);
extern void rob_chara_age_age_array_reset(int32_t rob_id);
extern void rob_chara_age_age_array_set_alpha(int32_t rob_id, int32_t part, float_t alpha);
extern void rob_chara_age_age_array_set_disp(int32_t rob_id, int32_t part, bool value);
extern void rob_chara_age_age_array_set_move_cancel(int32_t rob_id, int32_t part, float_t value);
extern void rob_chara_age_age_array_set_npr(int32_t rob_id, int32_t part, bool value);
extern void rob_chara_age_age_array_set_params(int32_t rob_id, int32_t part,
    int32_t npr, int32_t speed, int32_t skip, int32_t disp);
extern void rob_chara_age_age_array_set_speed(int32_t rob_id, int32_t part, float_t value);
extern void rob_chara_age_age_array_set_skip(int32_t rob_id, int32_t part);
extern void rob_chara_age_age_array_set_step(int32_t rob_id, int32_t part, float_t step);
extern void rob_chara_age_age_array_set_step_full(int32_t rob_id, int32_t part);

extern void rob_chara_array_reset_pv_data(ROB_ID id);
extern void rob_chara_array_reset_bone_data_item_equip(ROB_ID id);
extern void rob_chara_array_set_alpha_obj_flags(ROB_ID id, float_t alpha, mdl::ObjFlags flags);

extern bool rob_chara_check_for_ageageagain_module(CHARA_NUM cn, int32_t cos_id);

extern void rob_sleeve_handler_data_get_sleeve_data(
    CHARA_NUM cn, int32_t cos, rob_sleeve_data& l, rob_sleeve_data& r);
extern bool rob_sleeve_handler_data_load();
extern void rob_sleeve_handler_data_read();

extern bool pv_osage_manager_array_get_disp();
extern bool pv_osage_manager_array_get_disp(ROB_ID rob_id);
extern void pv_osage_manager_array_set_motion_ids(
    ROB_ID rob_id, const std::vector<uint32_t>& motion_ids);
extern void pv_osage_manager_array_set_not_reset_true();
extern void pv_osage_manager_array_set_pv_id(ROB_ID rob_id, int32_t pv_id, bool reset);
extern void pv_osage_manager_array_set_pv_set_motion(
    ROB_ID rob_id, const std::vector<pv_data_set_motion>& set_motion);

extern bool task_rob_load_open();
extern bool task_rob_load_append_free_req_data(CHARA_NUM cn);
extern bool task_rob_load_append_free_req_data_obj(CHARA_NUM cn, const RobItemEquip* equip);
extern bool task_rob_load_append_load_req_data(CHARA_NUM cn);
extern bool task_rob_load_append_load_req_data_obj(CHARA_NUM cn, const RobItemEquip* equip);
extern bool task_rob_load_check_load_req_data();
extern bool task_rob_load_close();

extern bool task_rob_manager_open();
extern bool task_rob_manager_check_chara_loaded(ROB_ID rob_id);
extern bool task_rob_manager_check_alive();
extern bool task_rob_manager_get_free_chara_list_empty();
extern bool task_rob_manager_get_wait(ROB_ID rob_id);
extern bool task_rob_manager_restart();
extern bool task_rob_manager_suspend();
extern bool task_rob_manager_close();

extern MOTTABLE_TYPE expression_id_to_mottbl_type(int32_t id);
extern MOTTABLE_TYPE hand_anim_id_to_mottbl_type(int32_t id);
extern MOTTABLE_TYPE look_anim_id_to_mottbl_type(int32_t id);
extern MOTTABLE_TYPE mouth_anim_id_to_mottbl_type(int32_t id);

inline bool rob_chara::check_for_ageageagain_module() {
    return rob_chara_check_for_ageageagain_module(chara_num, cos_id);
}
