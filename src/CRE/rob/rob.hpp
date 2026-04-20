/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../KKdLib/database/bone.hpp"
#include "../../KKdLib/database/motion.hpp"
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

enum BONE_BLK {
    BLK_DUMMY                   = -1,
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
    BONE_ID_DUMMY                   = -1,
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

enum Expr_type {
    Expr_constant = 0,
    Expr_variable,
    Expr_variable_rad,
    Expr_func1,
    Expr_func2,
    Expr_func3,
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

enum eyes_base_adjust_type {
    EYES_BASE_ADJUST_NONE      = -1,
    EYES_BASE_ADJUST_DIRECTION = 0x00,
    EYES_BASE_ADJUST_CLEARANCE = 0x01,
    EYES_BASE_ADJUST_OFF       = 0x02,
    EYES_BASE_ADJUST_MAX       = 0x03,
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

enum mothead_data_type {
    MOTHEAD_DATA_TYPE_0                      = 0x00,
    MOTHEAD_DATA_TYPE_1                      = 0x01,
    MOTHEAD_DATA_TYPE_2                      = 0x02,
    MOTHEAD_DATA_TYPE_3                      = 0x03,
    MOTHEAD_DATA_TYPE_4                      = 0x04,
    MOTHEAD_DATA_TYPE_5                      = 0x05,
    MOTHEAD_DATA_TYPE_6                      = 0x06,
    MOTHEAD_DATA_TYPE_7                      = 0x07,
    MOTHEAD_DATA_TYPE_8                      = 0x08,
    MOTHEAD_DATA_TYPE_9                      = 0x09,
    MOTHEAD_DATA_TYPE_10                     = 0x0A,
    MOTHEAD_DATA_TYPE_11                     = 0x0B,
    MOTHEAD_DATA_TYPE_12                     = 0x0C,
    MOTHEAD_DATA_TYPE_13                     = 0x0D,
    MOTHEAD_DATA_TYPE_14                     = 0x0E,
    MOTHEAD_DATA_TYPE_15                     = 0x0F,
    MOTHEAD_DATA_TYPE_16                     = 0x10,
    MOTHEAD_DATA_TYPE_17                     = 0x11,
    MOTHEAD_DATA_TYPE_18                     = 0x12,
    MOTHEAD_DATA_TYPE_19                     = 0x13,
    MOTHEAD_DATA_TYPE_20                     = 0x14,
    MOTHEAD_DATA_TYPE_21                     = 0x15,
    MOTHEAD_DATA_TYPE_22                     = 0x16,
    MOTHEAD_DATA_TYPE_23                     = 0x17,
    MOTHEAD_DATA_TYPE_24                     = 0x18,
    MOTHEAD_DATA_TYPE_25                     = 0x19,
    MOTHEAD_DATA_TYPE_26                     = 0x1A,
    MOTHEAD_DATA_TYPE_27                     = 0x1B,
    MOTHEAD_DATA_TYPE_28                     = 0x1C,
    MOTHEAD_DATA_TYPE_29                     = 0x1D,
    MOTHEAD_DATA_TYPE_30                     = 0x1E,
    MOTHEAD_DATA_TYPE_31                     = 0x1F,
    MOTHEAD_DATA_TYPE_32                     = 0x20,
    MOTHEAD_DATA_TYPE_33                     = 0x21,
    MOTHEAD_DATA_TYPE_34                     = 0x22,
    MOTHEAD_DATA_TYPE_35                     = 0x23,
    MOTHEAD_DATA_TYPE_36                     = 0x24,
    MOTHEAD_DATA_TYPE_37                     = 0x25,
    MOTHEAD_DATA_TYPE_38                     = 0x26,
    MOTHEAD_DATA_TYPE_39                     = 0x27,
    MOTHEAD_DATA_TYPE_40                     = 0x28,
    MOTHEAD_DATA_TYPE_41                     = 0x29,
    MOTHEAD_DATA_TYPE_42                     = 0x2A,
    MOTHEAD_DATA_TYPE_43                     = 0x2B,
    MOTHEAD_DATA_TYPE_44                     = 0x2C,
    MOTHEAD_DATA_TYPE_45                     = 0x2D,
    MOTHEAD_DATA_TYPE_46                     = 0x2E,
    MOTHEAD_DATA_TYPE_47                     = 0x2F,
    MOTHEAD_DATA_TYPE_48                     = 0x30,
    MOTHEAD_DATA_TYPE_49                     = 0x31,
    MOTHEAD_DATA_SET_FACE_MOTION_ID          = 0x32,
    MOTHEAD_DATA_TYPE_51                     = 0x33,
    MOTHEAD_DATA_TYPE_52                     = 0x34,
    MOTHEAD_DATA_SET_FACE_MOTTBL_MOTION      = 0x35,
    MOTHEAD_DATA_SET_HAND_R_MOTTBL_MOTION    = 0x36,
    MOTHEAD_DATA_SET_HAND_L_MOTTBL_MOTION    = 0x37,
    MOTHEAD_DATA_SET_MOUTH_MOTTBL_MOTION     = 0x38,
    MOTHEAD_DATA_SET_EYES_MOTTBL_MOTION      = 0x39,
    MOTHEAD_DATA_SET_EYELID_MOTTBL_MOTION    = 0x3A,
    MOTHEAD_DATA_SET_ROB_CHARA_HEAD_OBJECT   = 0x3B,
    MOTHEAD_DATA_SET_LOOK_CAMERA             = 0x3C,
    MOTHEAD_DATA_SET_EYELID_MOTION_FROM_FACE = 0x3D,
    MOTHEAD_DATA_ROB_PARTS_ADJUST            = 0x3E,
    MOTHEAD_DATA_TYPE_63                     = 0x3F,
    MOTHEAD_DATA_OSAGE_RESET                 = 0x40,
    MOTHEAD_DATA_MOTION_SKIN_PARAM           = 0x41,
    MOTHEAD_DATA_OSAGE_STEP                  = 0x42,
    MOTHEAD_DATA_SLEEVE_ADJUST               = 0x43,
    MOTHEAD_DATA_TYPE_68                     = 0x44,
    MOTHEAD_DATA_MOTION_MAX_FRAME            = 0x45,
    MOTHEAD_DATA_CAMERA_MAX_FRAME            = 0x46,
    MOTHEAD_DATA_OSAGE_MOVE_CANCEL           = 0x47,
    MOTHEAD_DATA_TYPE_72                     = 0x48,
    MOTHEAD_DATA_ROB_HAND_ADJUST             = 0x49,
    MOTHEAD_DATA_DISABLE_COLLISION           = 0x4A,
    MOTHEAD_DATA_ROB_ADJUST_GLOBAL           = 0x4B,
    MOTHEAD_DATA_ROB_ARM_ADJUST              = 0x4C,
    MOTHEAD_DATA_DISABLE_EYE_MOTION          = 0x4D,
    MOTHEAD_DATA_TYPE_78                     = 0x4E,
    MOTHEAD_DATA_ROB_CHARA_COLI_RING         = 0x4F,
    MOTHEAD_DATA_ADJUST_GET_GLOBAL_POS       = 0x50,
    MOTHEAD_DATA_MAX                         = 0x51,
};

enum MotionBlendType {
    MOTION_BLEND_NONE    = -1,
    MOTION_BLEND         = 0x00,
    MOTION_BLEND_FREEZE  = 0x01,
    MOTION_BLEND_CROSS   = 0x02,
    MOTION_BLEND_COMBINE = 0x03,
};

enum rob_chara_type {
    ROB_CHARA_TYPE_NONE = -1,
    ROB_CHARA_TYPE_0    = 0x00,
    ROB_CHARA_TYPE_1    = 0x01,
    ROB_CHARA_TYPE_2    = 0x02,
    ROB_CHARA_TYPE_3    = 0x03,
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
    ROB_OSAGE_PARTS_NONE        = -1,
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

struct rob_chara;
struct rob_chara_bone_data;

struct RobTransform {
    vec3 pos;
    vec3 rot;
    vec3 scale;
    vec3 hsc;

    RobTransform();

    void CalcMatrixHS(const vec3& hsc, mat4& mat, mat4& dsp_mat);

    void init(float_t px, float_t py, float_t pz,
        float_t rx, float_t ry, float_t rz);
    void init(const vec3& p, const vec3& r);
    void reset();
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
    void set_name_mat_no_scale_mat(const char* name, mat4* mat, mat4* no_scale_mat);
};

struct struc_314 {
    uint32_t* field_0;
    size_t field_8;
};

struct mot_key_set {
    mot_key_set_type type;
    int32_t keys_count;
    int32_t current_key;
    int32_t last_key;
    const uint16_t* frames;
    const float_t* values;
};

struct eyes_adjust {
    bool xrot_adjust;
    eyes_base_adjust_type base_adjust;
    float_t neg;
    float_t pos;

    eyes_adjust();
};

struct mot {
    uint16_t key_set_count;
    uint16_t frame_count;
    uint16_t field_4;
    mot_key_set* key_sets;
};

struct struc_369 {
    int32_t field_0;
    float_t field_4;
};

struct mot_key_data {
    bool key_sets_ready;
    size_t key_set_count;
    prj::sys_vector<mot_key_set> key_set;
    mot mot;
    prj::sys_vector<float_t> fc_value;
    const mot_data* mot_data;
    BONE_KIND skeleton_type;
    int32_t skeleton_select;
    uint32_t motion_id;
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

    void copy_rot_trans(const RobBlock& other);
    bool check_expression_id_not_null();
    bool get_constraint_ik(const RobBlock* bones);
    bool get_ex_rotation(RobTransform& transform, const RobBlock* bones);
    void get_mat(int32_t skeleton_select);
    void get_mat_ik(int32_t skeleton_select);
    void mult_mat(const mat4& parent_mat, const RobBlock* bones, bool solve_ik);
    void orient_x(const vec3& target);
    void orient_x_cns(const vec3& target, float_t weight);
    void orient_y(vec3 y_axis);
    const vec3* set_global_leaf_sub(const vec3* val, BONE_KIND kind, bool get_data, bool flip_x);
    void store_curr_rot_trans(int32_t skeleton_select);

    static float_t limit_angle(float_t angle);
    static void orient_to_target(mat4& mat, vec3 target, vec3 source);
};

struct bone_data_parent {
    rob_chara_bone_data* rob_bone_data;
    size_t motion_bone_count;
    size_t leaf_pos;
    size_t chain_pos;
    prj::sys_vector<RobBlock> bones;
    prj::sys_vector<uint16_t> bone_indices;
    vec3 gblctr_pos;
    vec3 gblctr_rot;
    uint32_t bone_key_set_count;
    uint32_t global_key_set_count;
    float_t rot_y;

    bone_data_parent();
    ~bone_data_parent();

    void ik_init(const BODYTYPE* bt, const CHAINPOSRADIUS* motion_chain_pos, const CHAINPOSRADIUS* disp_chain_pos);
};

struct mot_play_frame_data {
    float_t frame;
    float_t step;
    float_t step_prev;
    float_t frame_count;
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

struct struc_308 {
    int32_t field_0;
    int32_t field_4;
    uint8_t field_8;
    mat4 mat;
    mat4 field_4C;
    bool field_8C;
    vec3 field_90;
    vec3 field_9C;
    vec3 field_A8;
    float_t rot_y;
    float_t prev_rot_y;
    uint8_t field_BC;
    uint8_t field_BD;
    float_t field_C0;
    float_t field_C4;
    vec3 field_C8;

    struc_308();
};

struct struc_400 {
    bool field_0;
    bool field_1;
    bool field_2;
    bool field_3;
    bool field_4;
    float_t frame;
    float_t rot_y;
    float_t prev_rot_y;
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

typedef bool(* PFNMOTIONBONECHECKFUNC)(BONE_BLK bone_index);

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
    struc_308 field_4F8;
    int32_t field_5CC;
    MotionBlend* blend;

    motion_blend_mot();
    ~motion_blend_mot();

    void apply_global_transform();
    void copy_rot_trans();
    void copy_rot_trans(const prj::sys_vector<RobBlock>& bones);
    bool get_blend_enable();
    void get_n_hara_cp_position(vec3& value);
    MotionBlendType get_type();
    void init(rob_chara_bone_data* rob_bone_data,
        PFNMOTIONBONECHECKFUNC check_func, const bone_database* bone_data);
    void interpolate();
    void load_file(uint32_t motion_id, MotionBlendType blend_type, float_t blend,
        const bone_database* bone_data, const motion_database* mot_db);
    void mult_mat(const mat4* mat);
    void reset();
    void set_arm_length(BONE_BLK block_id, float_t value);
    void set_blend(MotionBlendType blend_type, float_t blend);
    void set_blend_duration(float_t duration, float_t step, float_t offset);
    void set_step(float_t step);
    void store_curr_rot_trans(int32_t skeleton_select);

    static bool interpolate_get_flip(struc_308& a1);
};

struct rob_chara_bone_data_ik_scale {
    float_t ratio0;
    float_t ratio1;
    float_t ratio2;
    float_t ratio3;

    rob_chara_bone_data_ik_scale();
};

struct partial_motion_blend_mot {
    motion_blend_mot_enabled_bones enabled_bones;
    bool disable;
    mot_key_data mot_key_data;
    mot_play_data mot_play_data;
    PartialMotionBlendFreeze blend;

    partial_motion_blend_mot();
    ~partial_motion_blend_mot();

    void init(BONE_KIND type,
        PFNMOTIONBONECHECKFUNC bone_check_func, size_t motion_bone_count, const bone_database* bone_data);
    void interpolate(prj::sys_vector<RobBlock>& bones,
        const prj::sys_vector<uint16_t>* bone_indices, BONE_KIND skeleton_type);
    void load_motion(uint32_t motion_id, const motion_database* mot_db);
    void reset();
    void set_blend_duration(float_t duration, float_t step, float_t offset);
    void set_frame(float_t frame);
    void set_step(float_t step);
    void store_curr_rot_trans(prj::sys_vector<RobBlock>& bones);
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

struct struc_936 {
    vec3 toe_l;
    vec3 toe_r;

    struc_936();

    void reset();
};

struct rob_chara_look_anim {
    prj::sys_vector<RobBlock>* bones;
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
    prj::sys_vector<RobBlock>* bones;
    float_t step;

    rob_chara_sleeve_adjust();

    void reset();
};

struct rob_chara_bone_data {
    bool field_0;
    bool field_1;
    size_t object_bone_count;
    size_t node_count;
    size_t motion_bone_count;
    size_t leaf_pos;
    size_t chain_pos;
    std::vector<mat4> mats;
    std::vector<mat4> mats2;
    std::vector<RobNode> nodes;
    BONE_KIND base_skeleton_type;
    BONE_KIND skeleton_type;
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
    rob_chara_bone_data_ik_scale ik_scale;
    struc_936 field_76C;
    rob_chara_look_anim look_anim;
    rob_chara_sleeve_adjust sleeve_adjust;

    rob_chara_bone_data();
    ~rob_chara_bone_data();

    bool check_look_anim_head_rotation();
    bool check_look_anim_ext_head_rotation();
    float_t get_frame();
    float_t get_frame_count();
    vec3* get_look_anim_target_view_point();
    bool get_look_anim_ext_head_rotation();
    bool get_look_anim_head_rotation();
    bool get_look_anim_update_view_point();
    mat4* get_mats_mat(size_t index);
    bool get_motion_has_looped();
    void interpolate();
    void load_eyes_motion(uint32_t motion_id, const motion_database* mot_db);
    void load_face_motion(uint32_t motion_id, const motion_database* mot_db);
    void load_eyelid_motion(uint32_t motion_id, const motion_database* mot_db);
    void load_hand_l_motion(uint32_t motion_id, const motion_database* mot_db);
    void load_hand_r_motion(uint32_t motion_id, const motion_database* mot_db);
    void load_mouth_motion(uint32_t motion_id, const motion_database* mot_db);
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
    void set_motion_frame(float_t frame, float_t step, float_t frame_count);
    void set_motion_loop(float_t loop_begin, int32_t loop_count, float_t loop_end);
    void set_motion_loop_state(mot_play_frame_data_loop_state value);
    void set_motion_max_frame(float_t value);
    void set_motion_playback_state(mot_play_frame_data_playback_state value);
    void set_mouth_blend_duration(float_t duration, float_t step, float_t offset);
    void set_mouth_frame(float_t frame);
    void set_mouth_step(float_t step);
    void update(const mat4* mat);
};

union rob_chara_pv_data_item {
    struct {
        int32_t head;
        int32_t face;
        int32_t chest;
        int32_t back;
    };
    int32_t arr[4];
};

struct RobAngle {
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

inline RobAngle operator-(const RobAngle& left, const RobAngle& right) {
    return (int16_t)(left.value - right.value);
}

inline RobAngle operator-(const int32_t& left, const RobAngle& right) {
    return (int16_t)(left - right.value);
}

inline RobAngle operator-(const RobAngle& left, const int32_t& right) {
    return (int16_t)(left.value - right);
}

struct rob_chara_pv_data {
    rob_chara_type type;
    bool field_4;
    bool field_5;
    bool field_6;
    vec3 field_8;
    RobAngle yang;
    int16_t field_16;
    rob_sleeve_data sleeve_l;
    rob_sleeve_data sleeve_r;
    int32_t field_70;
    uint32_t motion_face_ids[10];
    int32_t chara_size_index;
    bool height_adjust;
    rob_chara_pv_data_item item;
    eyes_adjust eyes_adjust;

    rob_chara_pv_data();

    void reset();
};

struct RobCollisionData {
    vec3 trans;
    float_t scale;
    BONE_ID bone;
    int32_t field_14;
};

struct struc_344 {
    int32_t chara_size_index;
    int32_t field_4;
    int32_t swim_costume;
    int32_t swim_s_costume;
};

struct RobKamae {
    const char* name;
    int32_t field_8;
    int32_t field_C;
    int32_t field_10;
    int32_t field_14;
    int32_t field_18;
};

struct RobData {
    int32_t object_set;
    BONE_KIND skeleton_type;
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

struct rob_chara_item_equip_object;

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
    const rob_chara_item_equip_object* skin_disp;
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
        const char* name, const rob_chara_item_equip_object* skin_disp);
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

    void set_data(const rob_chara_item_equip_object* skin_disp, const obj_skin_ex_node_constraint* data,
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
    uint32_t motion_id;
    float_t frame;
    float_t frame_count;
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
    const rob_chara_item_equip_object* skin_disp;
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

    void AddMotionResetData(uint32_t motion_id, float_t frame);
    void ApplyResetData();
    void CtrlOsagePlayData(const std::vector<opd_blend_data>& opd_blend_data);
    const float_t* LoadOpdData(size_t node_index, const float_t* opd_data, size_t opd_count);
    void LoadSkinParam(void* kv, const char* name, const bone_database* bone_data);
    void SetMotionResetData(uint32_t motion_id, float_t frame);
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
        const rob_chara_item_equip_object* skin, const bone_database* bone_data);
    void set_data(const obj_skin_ex_node_cloth* cldata,
        const rob_chara_item_equip_object* skin, const bone_database* bone_data);
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

    void AddMotionResetData(uint32_t motion_id, float_t frame);
    const float_t* LoadOpdData(size_t node_index, const float_t* opd_data, size_t opd_count);
    void SetMotionResetData(uint32_t motion_id, float_t frame);
    const float_t* SetOsagePlayDataInit(const float_t* opdi_data);
    void SetOsageReset();
    void SetRing(const osage_ring_data& ring);
    void SetSkinParam(skin_param_file_data* skp);
    void SetSkinParamOsageRoot(const skin_param_osage_root* skp_root);

    void reset_ex_force();
    void set_data(const rob_chara_item_equip_object* skin_disp, const obj_skin_ex_node_cloth* cls_data,
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

    void AddMotionResetData(uint32_t motion_id, float_t frame);
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
    void SetMotionResetData(uint32_t motion_id, float_t frame);
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

    void AddMotionResetData(uint32_t motion_id, float_t frame);
    const float_t* LoadOpdData(size_t node_index, const float_t* opd_data, size_t opd_count);
    void SetDisableCollision(const bool& value);
    void SetMotionResetData(uint32_t motion_id, float_t frame);
    const float_t* SetOsagePlayDataInit(const float_t* opdi_data);
    void SetOsageReset();
    void SetRing(const osage_ring_data& ring);
    void SetSkinParam(skin_param_file_data* skp);
    void SetWindDirection();

    void get_node_list(
        const obj_skin_ex_node_osage* osg_data, const obj_skin_osage_joint* joint,
        prj::vector_pair<uint32_t, RobJointNode*>& joint_node_list,
        std::map<std::string, ExNodeBlock*>& ex_node_list);
    void reset_ex_force();
    void set_data(const rob_chara_item_equip_object* skin_disp, const obj_skin_ex_node_osage* root,
        const char* name, const obj_skin_osage_joint* joint, const RobNode* mot_node,
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

    void set_data(const rob_chara_item_equip_object* skin_disp, const obj_skin_ex_node_constraint* data,
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

    void set_data(const rob_chara_item_equip_object* skin_disp, const obj_skin_ex_node_expression* data,
        const char* node_name, object_info objuid, size_t index, const bone_database* bone_data);
};

struct rob_chara_item_equip;

struct rob_chara_item_equip_object {
    size_t index;
    const mat4* motion_matrix;
    object_info obj_uid;
    object_info obj_uid_sub;
    std::vector<texture_pattern_struct> texchg_vec;
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
    const rob_chara_item_equip* rob_disp;

    rob_chara_item_equip_object();
    ~rob_chara_item_equip_object();

    void add_motion_reset_data(uint32_t motion_id, float_t frame, int32_t iterations);
    void check_no_opd(std::vector<opd_blend_data>& opd_blend_data);
    void clear_ex_data();
    void disp(const mat4& mat, render_context* rctx);
    const RobNode* get_node(int32_t index) const;
    const RobNode* get_node(const char* name, const bone_database* bone_data) const;
    int32_t get_node_index(const char* name, const bone_database* bone_data) const;
    const mat4* get_ex_data_bone_node_mat(const char* name);
    RobJointNode* get_normal_ref_osage_node(const std::string& str, size_t* index);
    void set_motion_node(const RobNode* mot_node, const bone_database* bone_data);
    void init_members(size_t index = 0xDEADBEEF);
    void pos_reset(int32_t init_cnt);
    void reset_ex_force();
    void reset_nodes_ex_force(rob_osage_parts_bit parts_bits);
    void set(object_info objuid, const RobNode* mot_node,
        bool osage_reset, const bone_database* bone_data, void* data, const object_database* obj_db);
    void set_alpha_obj_flags(float_t alpha, int32_t flags);
    bool set_boc(const skin_param_osage_root& skp_root, ExOsageBlock* osg);
    void set_collision_target_osage(const skin_param_osage_root& skp_root, skin_param* skp);
    void set_disable_collision(rob_osage_parts_bit parts_bits, bool disable);
    void set_ex_node(obj_skin_ex_data* ex_data);
    void set_ex_node_block(obj_skin_ex_data* ex_data,
        const bone_database* bone_data, void* data, const object_database* obj_db);
    void set_motion_reset_data(uint32_t motion_id, float_t frame);
    void set_motion_skin_param(int8_t chara_id, uint32_t motion_id, int32_t frame);
    void set_null_blocks_expression_data(const vec3& position, const vec3& rotation, const vec3& scale);
    void set_osage_play_data_init(const float_t* opdi_data);
    void set_osage_reset();
    void set_osage_move_cancel(const float_t& mv_ccl);
    void set_texture_pattern(texture_pattern_struct* tex_pat, size_t count);
    void skp_load(void* kv, const bone_database* bone_data);
    void skp_load(const skin_param_osage_root& skp_root, std::vector<skin_param_osage_node>& vec,
        skin_param_file_data* skp_file_data, const bone_database* bone_data);
    bool skp_load_boc(const skin_param_osage_root& skp_root, std::vector<RobJointNodeData>* node_data);
    void skp_load_file(void* data, const bone_database* bone_data, const object_database* obj_db);
    bool skp_load_normal_ref(const skin_param_osage_root& skp_root, std::vector<RobJointNodeData>* node_data);
};

struct rob_chara_item_equip {
    const RobNode* motion_node;
    const mat4* motion_matrix;
    rob_chara_item_equip_object* skin_disp;
    int32_t parts_attr[RPK_MAX];
    bool one_skin;
    int32_t disp_begin;
    int32_t disp_max;
    uint32_t shadow_flag;
    SHADOW_GROUP shadow_group;
    vec3 position;
    std::vector<texture_pattern_struct> hyoutan_texchg_list;
    object_info hyoutan_obj;
    int32_t hyoutan_rpk;
    bool disable_update;
    int32_t field_DC;
    vec4 skin_color;
    float_t wet;
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

    rob_chara_item_equip();
    ~rob_chara_item_equip();

    void add_motion_reset_data(uint32_t motion_id, float_t frame, int32_t iterations);
    void disp(int32_t chara_id, render_context* rctx);
    rob_chara_item_equip_object* get_skin_disp(ROB_PARTS_KIND rpk);
    object_info get_object_info(ROB_PARTS_KIND rpk);
    void set_motion_node(const RobNode* mot_node, const bone_database* bone_data);
    const mat4* get_ex_data_bone_node_mat(ROB_PARTS_KIND rpk, const char* name);
    void load_body_parts_object_info(ROB_PARTS_KIND rpk, object_info obj_info,
        const bone_database* bone_data, void* data, const object_database* obj_db);
    void load_outfit_object_info(ROB_PARTS_KIND rpk, object_info obj_info,
        bool osage_reset, const bone_database* bone_data, void* data, const object_database* obj_db);
    void pos_reset(uint8_t init_cnt);
    void reset();
    void reset_ex_force();
    void reset_init_data(RobNode* mot_node);
    void reset_nodes_ex_force(rob_osage_parts parts);
    void set_alpha_obj_flags(float_t alpha, mdl::ObjFlags flags);
    void set_disable_collision(rob_osage_parts parts, bool disable);
    void set_disp(ROB_PARTS_KIND rpk, bool value);
    void set_item(object_info obj_uid, ROB_PARTS_KIND rpk, bool osage_reset,
        const bone_database* bone_data, void* data, const object_database* obj_db);
    void set_motion_reset_data(uint32_t motion_id, float_t frame);
    void set_motion_skin_param(int8_t chara_id, uint32_t motion_id, int32_t frame);
    void set_null_blocks_expression_data(ROB_PARTS_KIND rpk,
        const vec3& position, const vec3& rotation, const vec3& scale);
    void set_object_texture_pattern(ROB_PARTS_KIND rpk, texture_pattern_struct* tex_pat, size_t count);
    void set_one_skin(bool value);
    void set_opd_blend_data(std::list<motion_blend_mot*>* a2);
    void set_osage_play_data_init(ROB_PARTS_KIND rpk, const float_t* opdi_data);
    void set_osage_reset();
    void set_osage_step(float_t value);
    void set_osage_move_cancel(uint8_t id, const float_t& mv_ccl);
    void set_shadow_group(int32_t chara_id);
    void set_texture_pattern(texture_pattern_struct* tex_pat, size_t count);
    void skp_load(ROB_PARTS_KIND rpk, const skin_param_osage_root& skp_root, std::vector<skin_param_osage_node>& vec,
        skin_param_file_data* skp_file_data, const bone_database* bone_data);
};

struct item_cos_texture_change_tex {
    texture* org;
    texture* chg;
    bool changed;

    item_cos_texture_change_tex();
    ~item_cos_texture_change_tex();
};

struct rob_chara_item_cos_data {
    CHARA_NUM curr_chara_num;
    CHARA_NUM chara_num;
    item_cos_data curr_cos;
    item_cos_data cos;
    std::map<int32_t, std::vector<item_cos_texture_change_tex>> texture_change;
    std::map<int32_t, std::vector<uint32_t>> item_change;
    std::map<object_info, ROB_PARTS_KIND> field_F0;
    std::map<int32_t, ROB_PARTS_KIND> field_100;
    std::vector<texture_pattern_struct> texture_pattern[31];
    std::map<int32_t, object_info> head_replace;

    rob_chara_item_cos_data();
    ~rob_chara_item_cos_data();

    bool check_for_npr_flag();
    const item_cos_data* get_cos();
    object_info get_head_object_replace(int32_t head_object_id);
    float_t get_max_face_depth();
    void reload_items(int32_t chara_id,
        const bone_database* bone_data, void* data, const object_database* obj_db);
    void set_chara_num(CHARA_NUM chara_num);
    void set_chara_num_item(CHARA_NUM chara_num, int32_t item_no);
    void set_chara_num_item_nos(CHARA_NUM chara_num, const int32_t* items);
    void set_chara_num_item_zero(CHARA_NUM chara_num, int32_t item_no);
    void set_item(int32_t item_no);
    void set_item_array(const rob_chara_pv_data_item& item);
    void set_item_no(item_sub_id sub_id, int32_t item_no);
    void set_item_nos(const int32_t* item_nos);
    void set_item_zero(int32_t item_no);
    void set_texture_pattern(rob_chara_item_equip* rob_disp,
        ROB_PARTS_KIND rpk, bool tex_pat_for_all);
    void set_texture_pattern(rob_chara_item_equip* rob_disp,
        uint32_t item_no, ROB_PARTS_KIND rpk, bool tex_pat_for_all);
    void set_texture_pattern(rob_chara_item_equip* rob_disp,
        const std::vector<uint32_t>& item_nos, ROB_PARTS_KIND rpk, bool tex_pat_for_all);
    void texture_change_clear();
    void texture_pattern_clear();
};

struct struc_525 {
    int32_t field_0;
    int32_t field_4;
};

struct struc_524 {
    int32_t field_0;
    struc_525 field_4;
    uint32_t motion_id;
    int8_t field_10;
};

struct struc_523 {
    int8_t field_0;
    int8_t field_1;
    int32_t field_4;
    int16_t field_8;
    int32_t field_C;
    vec3 field_10;
    int32_t field_1C;
    int32_t field_20;
    int32_t field_24;
    int32_t field_28;
    int32_t field_2C;
    int32_t field_30;
    int32_t field_34;
    int32_t field_38;
    int32_t field_3C;
    int32_t field_40;
    int32_t field_44;
    int32_t field_48;
    int32_t field_4C;
    int8_t field_50;
    int32_t field_54;
    std::list<void*> field_58;
    int64_t field_68;
    int64_t field_70;
    int32_t field_78;
    float_t field_7C;
    float_t field_80;

    struc_523();
    ~struc_523();
};

struct struc_526 {
    int32_t field_0;
    int32_t field_4;
};

struct struc_264 {
    int8_t field_0;
    struc_524 field_4;
    struc_524 field_18;
    struc_524 field_2C;
    struc_524 field_40;
    struc_524 field_54;
    int32_t field_68;
    int32_t field_6C;
    int32_t field_70;
    float_t field_74;
    int16_t field_78;
    int32_t field_7C;
    int32_t field_80;
    int32_t field_84;
    int32_t field_88;
    int32_t field_8C;
    int32_t field_90;
    int32_t field_94;
    int8_t field_98;
    int32_t field_9C;
    int32_t field_A0;
    int8_t field_A4;
    int8_t field_A5;
    int32_t field_A8;
    int32_t field_AC;
    int32_t field_B0;
    bool field_B4;
    struc_523 field_B8;
    int32_t field_140;
    int8_t field_144;
    int8_t field_145;
    int16_t field_146;
    void* field_148;
    int8_t field_150;
    int16_t field_152;
    int32_t field_154;
    int32_t field_158;
    int32_t field_15C;
    int32_t field_160;
    int32_t field_164;
    int32_t field_168;
    int32_t field_16C;
    int32_t field_170;
    int32_t field_174;
    int32_t field_178;
    int32_t field_17C;
    int8_t field_180;
    int32_t field_184;
    int32_t field_188;
    float_t field_18C;
    int8_t field_190;
    int32_t field_194;
    int32_t field_198;
    int32_t field_19C;
    int32_t field_1A0;
    uint8_t kamae_type;
    const RobKamae* kamae_data;
    int32_t field_1B0;
    int32_t field_1B4;
    int32_t field_1B8;
    int32_t field_1BC;
    struc_526 field_1C0;
    int8_t field_1C8;
    int8_t field_1C9;
    int8_t field_1CA;
    int32_t field_1CC;
    int32_t field_1D0;
    int32_t field_1D4;

    struc_264();
    ~struc_264();
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

    virtual void Field_8() = 0;
    virtual void Field_10(SubActParam* param) = 0;
    virtual void Field_18(rob_chara* rob_chr) = 0;
    virtual void Field_20(rob_chara* rob_chr) = 0;
};

class SubActExecAngry : public SubActExec {
public:
    SubActExecAngry();
    virtual ~SubActExecAngry() override;

    virtual void Field_8() override;
    virtual void Field_10(SubActParam* param) override;
    virtual void Field_18(rob_chara* rob_chr) override;
    virtual void Field_20(rob_chara* rob_chr) override;
};

class SubActExecCountNum : public SubActExec {
public:
    int32_t field_10;
    int32_t field_14;
    int32_t field_18;

    SubActExecCountNum();
    virtual ~SubActExecCountNum() override;

    virtual void Field_8() override;
    virtual void Field_10(SubActParam* param) override;
    virtual void Field_18(rob_chara* rob_chr) override;
    virtual void Field_20(rob_chara* rob_chr) override;
};

class SubActExecCry : public SubActExec {
public:
    SubActExecCry();
    virtual ~SubActExecCry() override;

    virtual void Field_8() override;
    virtual void Field_10(SubActParam* param) override;
    virtual void Field_18(rob_chara* rob_chr) override;
    virtual void Field_20(rob_chara* rob_chr) override;
};

class SubActExecEmbarrassed : public SubActExec {
public:
    int8_t field_10;
    SubActExecEmbarrassed();
    virtual ~SubActExecEmbarrassed() override;

    virtual void Field_8() override;
    virtual void Field_10(SubActParam* param) override;
    virtual void Field_18(rob_chara* rob_chr) override;
    virtual void Field_20(rob_chara* rob_chr) override;
};

class SubActExecLaugh : public SubActExec {
public:
    SubActExecLaugh();
    virtual ~SubActExecLaugh() override;

    virtual void Field_8() override;
    virtual void Field_10(SubActParam* param) override;
    virtual void Field_18(rob_chara* rob_chr) override;
    virtual void Field_20(rob_chara* rob_chr) override;
};

class SubActExecShakeHand : public SubActExec {
public:
    int32_t field_10;
    int32_t field_14;
    int8_t field_18;

    SubActExecShakeHand();
    virtual ~SubActExecShakeHand() override;

    virtual void Field_8() override;
    virtual void Field_10(SubActParam* param) override;
    virtual void Field_18(rob_chara* rob_chr) override;
    virtual void Field_20(rob_chara* rob_chr) override;
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
        int8_t rf_31 : 1;
    } bit;
};

struct RobSubAction {
    struct Data {
        SubActExec* field_0;
        SubActExec* field_8;
        SubActParam* param;
        SubActExec* field_18;
        SubActExecCry cry;
        SubActExecShakeHand shake_hand;
        SubActExecEmbarrassed embarrassed;
        SubActExecAngry angry;
        SubActExecLaugh laugh;
        SubActExecCountNum count_num;

        Data();
        ~Data();
    } data;

    RobSubAction();
    ~RobSubAction();

    void Reset();
    void SetSubActParam(SubActParam* value);
};

struct rob_chara_motion_frame_data {
    float_t frame;
    float_t prev_frame;
    float_t last_set_frame;
};

struct rob_chara_motion_step_data {
    float_t frame;
    float_t field_4;
    float_t step;
};

class RobPartialMotion {
public:
    struct Data {
        uint32_t motion_id;
        int32_t mottbl_index;
        rob_partial_motion_playback_state playback_state;
        float_t frame;
        float_t step;
        float_t frame_count;
        float_t blend_duration;
        float_t blend_step;
        float_t blend_offset;
        float_t play_duration;
        rob_chara_motion_frame_data* frame_data;
        rob_chara_motion_step_data* step_data;
        rob_partial_motion_loop_state loop_state;

        inline Data() : motion_id(-1), mottbl_index(), playback_state(ROB_PARTIAL_MOTION_PLAYBACK_NONE), frame(),
            step(), frame_count(), blend_duration(), blend_step(), blend_offset(), play_duration(),
            frame_data(), step_data(), loop_state(ROB_PARTIAL_MOTION_LOOP_NONE) {
            Reset();
        }

        inline ~Data() {

        }

        inline void Reset() {
            motion_id = -1;
            mottbl_index = 6;
            playback_state = ROB_PARTIAL_MOTION_PLAYBACK_NONE;
            frame = 0.0f;
            step = 1.0f;
            frame_count = 0.0f;
            blend_duration = 0.0f;
            blend_step = 1.0f;
            blend_offset = 1.0f;
            play_duration = 0.0f;
            frame_data = 0;
            step_data = 0;
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
    uint32_t motion_id;
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
    int32_t iterations;

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

struct rob_chara_motion {
    uint32_t motion_id;
    uint32_t prev_motion_id;
    rob_chara_motion_frame_data frame_data;
    rob_chara_motion_step_data step_data;
    float_t step;
    int32_t field_24;
    uint8_t field_28;
    uint8_t field_29;
    uint8_t field_2A;
    int32_t loop_index;
    uint32_t field_30;
    int16_t field_34;
    int16_t field_36;
    int32_t field_38[32];
    int32_t field_B8[32];
    float_t field_138;
    float_t field_13C;
    vec3 field_140;
    int32_t field_14C;
    struc_405<int32_t> field_150;
    float_t field_314;
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

    rob_chara_motion();
    ~rob_chara_motion();

    void reset();
};

struct struc_228 {
    int32_t field_0;
    int32_t field_4;
    uint32_t field_8;
    int32_t field_C;
};

struct struc_227 {
    int32_t field_0;
    float_t field_4;
    float_t field_8;
};

struct mothead_data {
    mothead_data_type type;
    int32_t frame;
    const void* data;
};

struct mothead_mot_data {
    int32_t type;
    const void* data;
};

struct mothead_mot {
    struc_228 field_0;
    int16_t field_10;
    int16_t field_12;
    mothead_mot_data* mot_data;
    mothead_data* data;
    int64_t* field_28;

    mothead_mot();
};

struct mothead {
    uint32_t mot_set_id;
    uint32_t first_mot_id;
    uint32_t last_mot_id;
    mothead_mot** mots;

    mothead();
};

struct mothead_file {
    uint32_t mot_set_id;
    uint32_t first_mot_id;
    uint32_t last_mot_id;
    uint32_t mot_offsets_offset;
};

struct mothead_modern_file {
    uint32_t mot_set_id;
    int32_t mot_count;
    int64_t hashes_offset;
    int64_t mot_offsets_offset;
};

struct mothead_mot_file {
    int32_t field_0;
    int32_t field_4;
    int32_t field_8;
    int32_t field_C;
    int16_t field_10;
    int16_t field_12;
    uint32_t mot_data_offset;
    uint32_t data_offset;
    uint32_t field_1C;
};

struct struc_377 {
    const mothead_data* current;
    const mothead_data* data;
    bool is_x; // X
};

struct struc_226 {
    int8_t field_0[27];
};

struct struc_225 {
    float_t field_0[27];
};

struct struc_224 {
    float_t field_0[27];
};

struct struc_306 {
    int16_t field_0;
    float_t frame;
    float_t field_8;
    int16_t field_C;
    int16_t field_E;
    vec3 field_10;
    vec3 field_1C;
    vec3 field_28;
    int32_t field_34;
    int32_t field_38;
    int32_t field_3C;
    int32_t field_40;
    int32_t field_44;
    int32_t field_48;

    struc_306();
    struc_306(int16_t field_0, float_t frame, float_t field_8);
};

struct struc_652 {
    uint32_t motion_id;
    float_t frame_count;
    float_t frame;
    int16_t field_C;
    struc_228 field_10;
    struc_228 field_20;
    struc_228 field_30;
    struc_228 field_40;
    int16_t field_50;
    int16_t field_52;
    int16_t field_54;
    int32_t field_58;
    int32_t field_5C;
    int32_t field_60;
    int32_t field_64;
    int32_t field_68;
    int32_t loop_count;
    float_t loop_begin;
    float_t loop_end;
    float_t field_78;
    float_t field_7C;
    float_t field_80;
    int8_t field_84;
    int32_t field_88;
    int32_t field_8C;
    int32_t field_90;
    int16_t field_94;
    int16_t field_96;
    int16_t field_98;
    int32_t field_9C;
    int16_t field_A0;
    int32_t field_A4;
    int64_t field_A8;
    struc_227 field_B0[26];
    int32_t field_1E8;
    float_t field_1EC;
    float_t field_1F0;
    float_t field_1F4;
    float_t field_1F8;
    float_t field_1FC;
    float_t field_200;
    int32_t field_204;
    int32_t field_208;
    int32_t field_20C;
    int64_t field_210;
    float_t field_218;
    float_t field_21C;
    int16_t field_220;
    std::list<void*> field_228;
    int16_t field_238;
    float_t field_23C;
    int32_t field_240;
    int16_t field_244;
    const mothead_mot_data* field_248;
    int64_t field_250;
    float_t field_258;
    int32_t field_25C;
    int64_t field_260;
    int64_t field_268;
    int32_t field_270;
    int16_t field_274;
    int16_t field_276;
    int32_t field_278;
    int32_t field_27C;
    int32_t field_280;
    int16_t field_284;
    int64_t field_288;
    int32_t field_290;
    int32_t field_294;
    int32_t field_298;
    float_t field_29C;
    int8_t field_2A0;
    float_t field_2A4;
    float_t field_2A8;
    float_t field_2AC;
    int64_t field_2B0;
    int16_t field_2B8;
    int32_t field_2BC;
    float_t field_2C0;
    float_t field_2C4;
    int32_t field_2C8;
    int32_t field_2CC;
    int64_t field_2D0;
    int64_t field_2D8;
    int64_t field_2E0;
    int16_t field_2E8;
    int32_t field_2EC;
    int32_t field_2F0;
    int32_t field_2F4;
    int32_t field_2F8;
    int32_t field_2FC;
    int8_t field_300;
    int32_t field_304;
    int32_t field_308;
    float_t field_30C;
    int32_t field_310;
    int16_t field_314;
    vec3 field_318;
    float_t field_324;
    float_t field_328;
    int32_t iterations;

    struc_652();
    ~struc_652();

    void reset();

    void sub_140536DD0();
};

struct struc_651 {
    struc_377 field_0;
    int32_t field_10;
    int32_t field_14;
    int32_t field_18;
    float_t field_1C;
    vec3 field_20;
    struc_226 field_2C[3];
    struc_225 field_80[3];
    struc_224 field_1C4[3];
    int64_t field_308;
    float_t arm_adjust_next_value;  // X
    float_t arm_adjust_prev_value;  // X
    int32_t arm_adjust_start_frame; // X
    float_t arm_adjust_duration;    // X
    float_t field_310;
    float_t field_314;
    int8_t field_318;
    int32_t field_31C;
    float_t field_320;
    float_t field_324;
    float_t field_328;
    float_t field_32C;
    float_t field_330;
    float_t field_334;
    int8_t field_338;
    struc_306 field_33C[4];

    struc_651();

    void reset();
};

struct struc_223 {
    struc_652 field_0;
    struc_651 field_330;
    const int64_t* field_7A0;
    uint32_t motion_set_id;

    struc_223();
    ~struc_223();

    void reset();
};

struct rob_chara_data_miku_rot {
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

    rob_chara_data_miku_rot();

    void reset();
};

struct rob_chara_adjust_data {
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

    rob_chara_adjust_data();

    void reset();
};

struct struc_195 {
    vec3 prev_pos;
    vec3 pos;
    float_t scale;
    float_t field_1C;
    float_t field_20;
    float_t field_24;

    struc_195();
};

struct pos_scale {
    vec2 pos;
    float_t scale;

    pos_scale();

    float_t get_screen_pos_scale(const mat4& mat,
        const vec3& pos, float_t scale = 0.0f, bool apply_offset = false);
};

struct struc_267 {
    float_t field_0;
    int16_t field_4;
    int16_t field_6;
    float_t field_8;
    int16_t field_C;
};

struct struc_209 {
    int16_t field_0;
    int32_t field_4;
    int32_t field_8;
    int32_t field_C;
    int32_t field_10;
    int32_t field_14;
    int32_t field_18;
    int32_t field_1C;
    int32_t field_20;
    int32_t field_24;
    int32_t field_28;
    int32_t field_2C;
    int32_t field_30;
    int32_t field_34;
    int32_t field_38;
    int32_t field_3C;
    int32_t field_40;
    int32_t field_44;
    int32_t field_48;
    int32_t field_4C;
    int32_t field_50;
    int32_t field_54;
    int32_t field_58;
    int32_t field_5C;
    int32_t field_60;
    float_t field_64;
    int32_t field_68;
    int32_t field_6C;
    int32_t field_70;
    int32_t field_74;
    mat4 field_78[27];
    mat4 field_738[27];
    struc_195 field_DF8[27];
    struc_195 field_1230[27];
    struc_195 field_1668[27];
    pos_scale field_1AA0[27];
    float_t field_1BE4[27];
    prj::vector_pair<int64_t, float_t> field_1C50;
    int64_t field_1C68;
    int64_t field_1C70;
    int64_t field_1C78;
    int64_t field_1C80;
    int32_t field_1C88[27];
    int32_t field_1CF4[27];
    int32_t field_1D60[27];
    int32_t field_1DCC;
    int32_t field_1DD0;
    int32_t field_1DD4;
    int32_t field_1DD8;
    int32_t field_1DDC;
    float_t field_1DE0;
    float_t field_1DE4;
    int32_t field_1DE8;
    int32_t field_1DEC;
    float_t field_1DF0;
    float_t field_1DF4;
    int32_t field_1DF8;
    int32_t field_1DFC;
    int32_t field_1E00;
    int32_t field_1E04;
    int32_t field_1E08;
    int32_t field_1E0C;
    int32_t field_1E10;
    int32_t field_1E14;
    int32_t field_1E18;
    int32_t field_1E1C;
    int32_t field_1E20;
    int32_t field_1E24;
    int32_t field_1E28;
    int32_t field_1E2C;
    int32_t field_1E30;
    int32_t field_1E34;
    int32_t field_1E38;
    int32_t field_1E3C;
    int32_t field_1E40;
    int32_t field_1E44;
    int32_t field_1E48;
    int32_t field_1E4C;
    int32_t field_1E50;
    int32_t field_1E54;
    int32_t field_1E58;
    int32_t field_1E5C;
    int32_t field_1E60;
    int32_t field_1E64;
    int32_t field_1E68;
    int32_t field_1E6C;
    int32_t field_1E70;
    int32_t field_1E74;
    int32_t field_1E78;
    int32_t field_1E7C;
    int32_t field_1E80;
    int32_t field_1E84;
    int32_t field_1E88;
    int32_t field_1E8C;
    int32_t field_1E90;
    int32_t field_1E94;
    int32_t field_1E98;
    int32_t field_1E9C;
    int32_t field_1EA0;
    int32_t field_1EA4;
    int32_t field_1EA8;
    int32_t field_1EAC;
    int32_t field_1EB0;
    struc_267 field_1EB4[4];
    int32_t field_1EF4[4];
    int8_t field_1F04;
    int32_t field_1F08;
    int32_t field_1F0C;
    int32_t field_1F10;
    int32_t field_1F14;
    int32_t field_1F18;
    int32_t field_1F1C;
    int8_t field_1F20;
    int8_t field_1F21;
    int8_t field_1F22;
    int8_t field_1F23;
    int8_t field_1F24;
    int8_t field_1F25;
    int8_t field_1F26;

    struc_209();
    ~struc_209();
};

struct rob_chara_data {
    RobFlag flag;
    struc_264 action;
    RobSubAction rob_sub_action;
    rob_chara_motion motion;
    struc_223 motdata;
    rob_chara_data_miku_rot position;
    rob_chara_adjust_data adjust_data;
    struc_209 field_1E68;
    float_t field_3D90;
    int32_t field_3D94;
    int16_t field_3D98;
    int16_t field_3D9A;
    int32_t field_3D9C;
    int32_t field_3DA0;
    uint8_t field_3DA4;
    int64_t field_3DA8;
    int64_t field_3DB0;
    int32_t field_3DB8;
    int32_t field_3DBC;
    int32_t field_3DC0;
    int32_t field_3DC4;
    int32_t field_3DC8;
    int32_t field_3DCC;
    int32_t field_3DD0;
    float_t field_3DD4;
    int32_t field_3DD8;
    float_t field_3DDC;
    uint8_t field_3DE0;
    float_t arm_adjust_scale; // X

    rob_chara_data();
    ~rob_chara_data();

    void reset();
};

struct struc_307 {
    int64_t field_0;
    int32_t field_8;
    int32_t field_C;
    int64_t field_10;
    int16_t field_18;
    int16_t field_1A;
    int16_t field_1C;
    int16_t field_1E;
    int64_t field_20;
    rob_chara_bone_data* field_28;
};

struct rob_chara {
    int8_t chara_id;
    rob_chara_type type;
    bool field_C;
    bool field_D;
    CHARA_NUM chara_num;
    int32_t cos_id;
    float_t frame_speed;
    rob_chara* field_20;
    rob_chara_bone_data* bone_data;
    rob_chara_item_equip* rob_disp;
    rob_chara_item_cos_data item_cos_data;
    rob_chara_data data;
    rob_chara_data data_prev;
    const RobData* rob_data;
    rob_chara_pv_data pv_data;

    rob_chara();
    ~rob_chara();

    void adjust_ctrl();
    void arm_adjust_ctrl();
    void autoblink_disable();
    void autoblink_enable();
    bool check_for_ageageagain_module();
    mat4* get_bone_data_mat(size_t index);
    object_info get_rob_data_face_object(int32_t index);
    float_t get_frame();
    float_t get_frame_count();
    float_t get_max_face_depth();
    uint32_t get_rob_cmn_mottbl_motion_id(int32_t id);
    float_t get_pos_scale(int32_t bone, vec3& pos);
    void load_body_parts_object_info(ROB_PARTS_KIND rpk, object_info obj_info,
        const bone_database* bone_data, void* data, const object_database* obj_db);
    void load_motion(uint32_t motion_id, bool a3, float_t frame,
        MotionBlendType blend_type, const bone_database* bone_data, const motion_database* mot_db);
    void load_outfit_object_info(ROB_PARTS_KIND rpk, object_info obj_info,
        bool osage_reset, const bone_database* bone_data, void* data, const object_database* obj_db);
    void pos_reset();
    void rob_info_ctrl();
    void rob_motion_modifier_ctrl();
    void reset_data(const rob_chara_pv_data* pv_data,
        const bone_database* bone_data, const motion_database* mot_db);
    void reset_osage();
    void set_bone_data_frame(float_t frame);
    void set_chara_color(bool value);
    void set_chara_height_adjust(bool value);
    void set_chara_pos_adjust(const vec3& value);
    void set_chara_pos_adjust_y(float_t value);
    void set_chara_size(float_t value);
    void set_data_adjust_mat(rob_chara_adjust_data* rob_chr_adj, bool pos_adjust = true);
    void set_base_position_pos(const vec3& value);
    void set_base_position_yang(const RobAngle& value);
    void set_disable_collision(rob_osage_parts parts, bool disable);
    void set_eyelid_mottbl_motion(int32_t type, int32_t mottbl_index,
        float_t value, rob_partial_motion_playback_state playback_state, float_t blend_duration,
        float_t play_duration, float_t step, rob_partial_motion_loop_state loop_state,
        float_t blend_offset, const motion_database* mot_db);
    void set_eyelid_mottbl_motion_from_face(int32_t a2,
        float_t blend_duration, float_t value, float_t blend_offset, const motion_database* mot_db);
    void set_eyes_mottbl_motion(int32_t type, int32_t mottbl_index,
        float_t value, rob_partial_motion_playback_state playback_state, float_t blend_duration,
        float_t play_duration, float_t step, rob_partial_motion_loop_state loop_state,
        float_t blend_offset, const motion_database* mot_db);
    void set_face_mottbl_motion(int32_t type, int32_t mottbl_index,
        float_t value, rob_partial_motion_playback_state playback_state, float_t blend_duration, float_t play_duration,
        float_t step, rob_partial_motion_loop_state loop_state,
        float_t blend_offset, bool a11, const motion_database* mot_db);
    void set_face_object(object_info obj_info, int32_t a3);
    void set_face_object_index(int32_t index);
    void set_frame(float_t frame);
    void set_hand_item(int32_t uid, float_t blend_duration);
    void set_hand_item_l(int32_t uid);
    void set_hand_item_r(int32_t uid);
    void set_hand_l_mottbl_motion(int32_t type, int32_t mottbl_index,
        float_t value, rob_partial_motion_playback_state playback_state, float_t blend_duration,
        float_t play_duration, float_t step, rob_partial_motion_loop_state loop_state,
        float_t blend_offset, const motion_database* mot_db);
    void set_hand_r_mottbl_motion(int32_t type, int32_t mottbl_index,
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
    bool set_motion_id(uint32_t motion_id, float_t frame,
        float_t blend_duration, bool blend, bool set_motion_reset_data,
        MotionBlendType blend_type, const bone_database* bone_data, const motion_database* mot_db);
    void set_motion_loop(float_t loop_begin, float_t loop_end, int32_t loop_count);
    void set_motion_reset_data(uint32_t motion_id, float_t frame);
    void set_motion_skin_param(uint32_t motion_id, float_t frame);
    void set_motion_step(float_t value);
    void set_mouth_mottbl_motion(int32_t type, int32_t mottbl_index,
        float_t value, rob_partial_motion_playback_state playback_state, float_t blend_duration,
        float_t play_duration, float_t step, rob_partial_motion_loop_state loop_state,
        float_t blend_offset, const motion_database* mot_db);
    void set_osage_move_cancel(uint8_t id, float_t mv_ccl);
    void set_osage_reset();
    void set_osage_step(float_t value);
    void set_parts_disp(ROB_PARTS_KIND rpk, bool disp);
    void set_right_hand_scale(float_t value);
    void set_shadow_cast(bool value);
    void set_stage_data_ring(const int32_t& stage_index);
    void set_step_motion_step(float_t value);
    void set_use_opd(bool value);
    void set_visibility(bool value);
    void set_wind_strength(float_t value);

    inline bool is_visible() {
        return !!data.flag.bit.disp;
    }

    void sub_1405070E0(const bone_database* bone_data, const motion_database* mot_db);
    void sub_140509D30();
    void sub_1405163C0(int32_t index, mat4& mat);
    void sub_140551000();
};

struct pv_data_set_motion {
    uint32_t motion_id;
    std::pair<float_t, int32_t> frame_stage_index;

    pv_data_set_motion();
    pv_data_set_motion(uint32_t motion_id);
    pv_data_set_motion(uint32_t motion_id, std::pair<float_t, int32_t> frame_stage_index);
};

struct osage_init_data {
    rob_chara* rob_chr;
    int32_t pv_id;
    uint32_t motion_id;
    std::string dir;
    int32_t frame;

    osage_init_data();
    osage_init_data(rob_chara* rob_chr, uint32_t motion_id = -1);
    osage_init_data(rob_chara* rob_chr, int32_t pv_id,
        uint32_t motion_id, const std::string& dir = "", int32_t frame = -1);
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

extern const RobData* get_rob_data(CHARA_NUM cn);
extern int32_t rob_data_get_chara_size_index(CHARA_NUM cn);
extern int32_t rob_data_get_swim_costume(CHARA_NUM cn);
extern int32_t rob_data_get_swim_s_costume(CHARA_NUM cn);

extern float_t chara_pos_adjust_y_table_get_value(uint32_t index);
extern float_t chara_size_table_get_value(uint32_t index);

extern bool check_cos_id_is_501(int32_t cos_id);

extern uint32_t get_common_rob_mot(CHARA_NUM cn, int32_t a2, int32_t id);

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

extern void opd_make_manager_add_task(const OpdMakeManagerArgs& args);
extern bool opd_make_manager_check_task_ready();
extern bool opd_make_manager_del_task();
extern OpdMakeManagerData* opd_make_manager_get_data();

extern void opd_make_start();
extern void opd_make_start_get_motion_ids(std::vector<int32_t>& motion_ids);
extern void opd_make_stop();

extern bool osage_play_data_manager_add_task();
extern void osage_play_data_manager_append_chara_motion_id(rob_chara* rob_chr, uint32_t motion_id);
extern void osage_play_data_manager_append_chara_motion_ids(
    rob_chara* rob_chr, const std::vector<uint32_t>& motion_ids);
extern bool osage_play_data_manager_check_task_ready();
extern void osage_play_data_manager_get_opd_file_data(object_info obj_info,
    uint32_t motion_id, const float_t*& data, uint32_t& count);
extern void osage_play_data_manager_reset();

extern void osage_play_database_load();

extern void pv_osage_manager_array_reset(int32_t chara_id);

extern void rob_init();
extern void rob_free();

extern void rob_chara_age_age_array_ctrl(int32_t chara_id, int32_t part_id, mat4& mat);
extern void rob_chara_age_age_array_disp(render_context* rctx,
    int32_t chara_id, bool reflect, bool chara_color);
extern void rob_chara_age_age_array_load(int32_t chara_id, int32_t part_id);
extern void rob_chara_age_age_array_reset(int32_t chara_id);
extern void rob_chara_age_age_array_set_alpha(int32_t chara_id, int32_t part_id, float_t alpha);
extern void rob_chara_age_age_array_set_disp(int32_t chara_id, int32_t part_id, bool value);
extern void rob_chara_age_age_array_set_move_cancel(int32_t chara_id, int32_t part_id, float_t value);
extern void rob_chara_age_age_array_set_npr(int32_t chara_id, int32_t part_id, bool value);
extern void rob_chara_age_age_array_set_params(int32_t chara_id, int32_t part_id,
    int32_t npr, int32_t speed, int32_t skip, int32_t disp);
extern void rob_chara_age_age_array_set_speed(int32_t chara_id, int32_t part_id, float_t value);
extern void rob_chara_age_age_array_set_skip(int32_t chara_id, int32_t part_id);
extern void rob_chara_age_age_array_set_step(int32_t chara_id, int32_t part_id, float_t step);
extern void rob_chara_age_age_array_set_step_full(int32_t chara_id, int32_t part_id);

extern bool rob_chara_array_check_visibility(int32_t chara_id);
extern rob_chara* rob_chara_array_get(int32_t chara_id);
extern rob_chara_bone_data* rob_chara_array_get_bone_data(int32_t chara_id);
extern float_t rob_chara_array_get_data_adjust_scale(int32_t chara_id);
extern rob_chara_item_cos_data* rob_chara_array_get_item_cos_data(int32_t chara_id);
extern rob_chara_item_equip* rob_chara_array_get_rob_disp(int32_t chara_id);
extern int32_t rob_chara_array_init_chara_num(CHARA_NUM chara_num,
    const rob_chara_pv_data& pv_data, int32_t cos_id, bool can_set_default);
extern void rob_chara_array_free_chara_id(int32_t chara_id);
extern void rob_chara_array_reset_pv_data(int32_t chara_id);
extern void rob_chara_array_reset_bone_data_item_equip(int32_t chara_id);
extern void rob_chara_array_set_alpha_obj_flags(int32_t chara_id, float_t alpha, mdl::ObjFlags flags);
extern void rob_chara_array_set_visibility(int32_t chara_id, bool value);

extern bool rob_chara_check_for_ageageagain_module(CHARA_NUM chara_num, int32_t cos_id);

extern bool rob_chara_pv_data_array_check_chara_id(int32_t chara_id);

extern void rob_sleeve_handler_data_get_sleeve_data(
    CHARA_NUM chara_num, int32_t cos, rob_sleeve_data& l, rob_sleeve_data& r);
extern bool rob_sleeve_handler_data_load();
extern void rob_sleeve_handler_data_read();

extern bool pv_osage_manager_array_get_disp();
extern bool pv_osage_manager_array_get_disp(int32_t chara_id);
extern void pv_osage_manager_array_set_motion_ids(
    int32_t chara_id, const std::vector<uint32_t>& motion_ids);
extern void pv_osage_manager_array_set_not_reset_true();
extern void pv_osage_manager_array_set_pv_id(int32_t chara_id, int32_t pv_id, bool reset);
extern void pv_osage_manager_array_set_pv_set_motion(
    int32_t chara_id, const std::vector<pv_data_set_motion>& set_motion);

extern bool task_rob_load_add_task();
extern bool task_rob_load_append_free_req_data(CHARA_NUM chara_num);
extern bool task_rob_load_append_free_req_data_obj(CHARA_NUM chara_num, const item_cos_data* cos);
extern bool task_rob_load_append_load_req_data(CHARA_NUM chara_num);
extern bool task_rob_load_append_load_req_data_obj(CHARA_NUM chara_num, const item_cos_data* cos);
extern bool task_rob_load_check_load_req_data();
extern bool task_rob_load_del_task();

extern bool task_rob_manager_add_task();
extern bool task_rob_manager_check_chara_loaded(int32_t chara_id);
extern bool task_rob_manager_check_task_ready();
extern void task_rob_manager_free_all_chara();
extern bool task_rob_manager_get_free_chara_list_empty();
extern bool task_rob_manager_get_wait(int32_t chara_id);
extern bool task_rob_manager_hide_task();
extern bool task_rob_manager_run_task();
extern bool task_rob_manager_del_task();

extern int32_t expression_id_to_mottbl_index(int32_t expression_id);
extern int32_t hand_anim_id_to_mottbl_index(int32_t hand_anim_id);
extern int32_t look_anim_id_to_mottbl_index(int32_t look_anim_id);
extern int32_t mouth_anim_id_to_mottbl_index(int32_t mouth_anim_id);

inline bool rob_chara::check_for_ageageagain_module() {
    return rob_chara_check_for_ageageagain_module(chara_num, cos_id);
}
