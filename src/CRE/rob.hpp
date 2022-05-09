/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/database/bone.hpp"
#include "../KKdLib/database/motion.hpp"
#include "../KKdLib/mat.h"
#include "../KKdLib/mot.hpp"
#include "../KKdLib/vec.h"
#include "draw_object.h"
#include "item_table.hpp"
#include "object.hpp"
#include "static_var.h"

enum ex_expression_block_stack_type {
    EX_EXPRESSION_BLOCK_STACK_NUMBER          = 0x00,
    EX_EXPRESSION_BLOCK_STACK_VARIABLE        = 0x01,
    EX_EXPRESSION_BLOCK_STACK_VARIABLE_RADIAN = 0x02,
    EX_EXPRESSION_BLOCK_STACK_OP1             = 0x03,
    EX_EXPRESSION_BLOCK_STACK_OP2             = 0x04,
    EX_EXPRESSION_BLOCK_STACK_OP3             = 0x05,
};

enum ex_node_type {
    EX_NONE       = 0x00,
    EX_OSAGE      = 0x01,
    EX_EXPRESSION = 0x02,
    EX_CONSTRAINT = 0x03,
    EX_CLOTH      = 0x04,
};

enum eyes_base_adjust_type {
    EYES_BASE_ADJUST_DIRECTION = 0,
    EYES_BASE_ADJUST_CLEARANCE = 1,
    EYES_BASE_ADJUST_OFF       = 2,
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
    MOTHEAD_DATA_TYPE_60                     = 0x3C,
    MOTHEAD_DATA_SET_EYELID_MOTION_FROM_FACE = 0x3D,
    MOTHEAD_DATA_ROB_PARTS_ADJUST            = 0x3E,
    MOTHEAD_DATA_TYPE_63                     = 0x3F,
    MOTHEAD_DATA_WIND_RESET                  = 0x40,
    MOTHEAD_DATA_OSAGE_RESET                 = 0x41,
    MOTHEAD_DATA_OSAGE_STEP                  = 0x42,
    MOTHEAD_DATA_TYPE_67                     = 0x43,
    MOTHEAD_DATA_TYPE_68                     = 0x44,
    MOTHEAD_DATA_TYPE_69                     = 0x45,
    MOTHEAD_DATA_TYPE_70                     = 0x46,
    MOTHEAD_DATA_OSAGE_MOVE_CANCEL           = 0x47,
    MOTHEAD_DATA_TYPE_72                     = 0x48,
    MOTHEAD_DATA_HAND_ADJUST                 = 0x49,
    MOTHEAD_DATA_TYPE_74                     = 0x4A,
    MOTHEAD_DATA_TYPE_75                     = 0x4B,
    MOTHEAD_DATA_TYPE_76                     = 0x4C,
    MOTHEAD_DATA_TYPE_77                     = 0x4D,
    MOTHEAD_DATA_TYPE_78                     = 0x4E,
    MOTHEAD_DATA_ROB_CHARA_COLI_RING         = 0x4F,
    MOTHEAD_DATA_ADJUST_GET_GLOBAL_TRANS     = 0x50,
    MOTHEAD_DATA_MAX                         = 0x51,
};

enum MotionBlendType {
    MOTION_BLEND_NONE    = -1,
    MOTION_BLEND         = 0x00,
    MOTION_BLEND_FREEZE  = 0x01,
    MOTION_BLEND_CROSS   = 0x02,
    MOTION_BLEND_COMBINE = 0x03,
};

enum motion_bone_index {
    MOTION_BONE_NONE                    = -1,
    MOTION_BONE_N_HARA_CP               = 0x00,
    MOTION_BONE_KG_HARA_Y               = 0x01,
    MOTION_BONE_KL_HARA_XZ              = 0x02,
    MOTION_BONE_KL_HARA_ETC             = 0x03,
    MOTION_BONE_N_HARA                  = 0x04,
    MOTION_BONE_CL_MUNE                 = 0x05,
    MOTION_BONE_N_MUNE_B                = 0x06,
    MOTION_BONE_KL_MUNE_B_WJ            = 0x07,
    MOTION_BONE_KL_KUBI                 = 0x08,
    MOTION_BONE_N_KAO                   = 0x09,
    MOTION_BONE_CL_KAO                  = 0x0A,
    MOTION_BONE_FACE_ROOT               = 0x0B,
    MOTION_BONE_N_AGO                   = 0x0C,
    MOTION_BONE_KL_AGO_WJ               = 0x0D,
    MOTION_BONE_N_TOOTH_UNDER           = 0x0E,
    MOTION_BONE_TL_TOOTH_UNDER_WJ       = 0x0F,
    MOTION_BONE_N_EYE_L                 = 0x10,
    MOTION_BONE_KL_EYE_L                = 0x11,
    MOTION_BONE_N_EYE_L_WJ_EX           = 0x12,
    MOTION_BONE_KL_HIGHLIGHT_L_WJ       = 0x13,
    MOTION_BONE_N_EYE_R                 = 0x14,
    MOTION_BONE_KL_EYE_R                = 0x15,
    MOTION_BONE_N_EYE_R_WJ_EX           = 0x16,
    MOTION_BONE_KL_HIGHLIGHT_R_WJ       = 0x17,
    MOTION_BONE_N_EYELID_L_A            = 0x18,
    MOTION_BONE_TL_EYELID_L_A_WJ        = 0x19,
    MOTION_BONE_N_EYELID_L_B            = 0x1A,
    MOTION_BONE_TL_EYELID_L_B_WJ        = 0x1B,
    MOTION_BONE_N_EYELID_R_A            = 0x1C,
    MOTION_BONE_TL_EYELID_R_A_WJ        = 0x1D,
    MOTION_BONE_N_EYELID_R_B            = 0x1E,
    MOTION_BONE_TL_EYELID_R_B_WJ        = 0x1F,
    MOTION_BONE_N_KUTI_D                = 0x20,
    MOTION_BONE_TL_KUTI_D_WJ            = 0x21,
    MOTION_BONE_N_KUTI_D_L              = 0x22,
    MOTION_BONE_TL_KUTI_D_L_WJ          = 0x23,
    MOTION_BONE_N_KUTI_D_R              = 0x24,
    MOTION_BONE_TL_KUTI_D_R_WJ          = 0x25,
    MOTION_BONE_N_KUTI_DS_L             = 0x26,
    MOTION_BONE_TL_KUTI_DS_L_WJ         = 0x27,
    MOTION_BONE_N_KUTI_DS_R             = 0x28,
    MOTION_BONE_TL_KUTI_DS_R_WJ         = 0x29,
    MOTION_BONE_N_KUTI_L                = 0x2A,
    MOTION_BONE_TL_KUTI_L_WJ            = 0x2B,
    MOTION_BONE_N_KUTI_M_L              = 0x2C,
    MOTION_BONE_TL_KUTI_M_L_WJ          = 0x2D,
    MOTION_BONE_N_KUTI_M_R              = 0x2E,
    MOTION_BONE_TL_KUTI_M_R_WJ          = 0x2F,
    MOTION_BONE_N_KUTI_R                = 0x30,
    MOTION_BONE_TL_KUTI_R_WJ            = 0x31,
    MOTION_BONE_N_KUTI_U                = 0x32,
    MOTION_BONE_TL_KUTI_U_WJ            = 0x33,
    MOTION_BONE_N_KUTI_U_L              = 0x34,
    MOTION_BONE_TL_KUTI_U_L_WJ          = 0x35,
    MOTION_BONE_N_KUTI_U_R              = 0x36,
    MOTION_BONE_TL_KUTI_U_R_WJ          = 0x37,
    MOTION_BONE_N_MABU_L_D_A            = 0x38,
    MOTION_BONE_TL_MABU_L_D_A_WJ        = 0x39,
    MOTION_BONE_N_MABU_L_D_B            = 0x3A,
    MOTION_BONE_TL_MABU_L_D_B_WJ        = 0x3B,
    MOTION_BONE_N_MABU_L_D_C            = 0x3C,
    MOTION_BONE_TL_MABU_L_D_C_WJ        = 0x3D,
    MOTION_BONE_N_MABU_L_U_A            = 0x3E,
    MOTION_BONE_TL_MABU_L_U_A_WJ        = 0x3F,
    MOTION_BONE_N_MABU_L_U_B            = 0x40,
    MOTION_BONE_TL_MABU_L_U_B_WJ        = 0x41,
    MOTION_BONE_N_EYELASHES_L           = 0x42,
    MOTION_BONE_TL_EYELASHES_L_WJ       = 0x43,
    MOTION_BONE_N_MABU_L_U_C            = 0x44,
    MOTION_BONE_TL_MABU_L_U_C_WJ        = 0x45,
    MOTION_BONE_N_MABU_R_D_A            = 0x46,
    MOTION_BONE_TL_MABU_R_D_A_WJ        = 0x47,
    MOTION_BONE_N_MABU_R_D_B            = 0x48,
    MOTION_BONE_TL_MABU_R_D_B_WJ        = 0x49,
    MOTION_BONE_N_MABU_R_D_C            = 0x4A,
    MOTION_BONE_TL_MABU_R_D_C_WJ        = 0x4B,
    MOTION_BONE_N_MABU_R_U_A            = 0x4C,
    MOTION_BONE_TL_MABU_R_U_A_WJ        = 0x4D,
    MOTION_BONE_N_MABU_R_U_B            = 0x4E,
    MOTION_BONE_TL_MABU_R_U_B_WJ        = 0x4F,
    MOTION_BONE_N_EYELASHES_R           = 0x50,
    MOTION_BONE_TL_EYELASHES_R_WJ       = 0x51,
    MOTION_BONE_N_MABU_R_U_C            = 0x52,
    MOTION_BONE_TL_MABU_R_U_C_WJ        = 0x53,
    MOTION_BONE_N_MAYU_L                = 0x54,
    MOTION_BONE_TL_MAYU_L_WJ            = 0x55,
    MOTION_BONE_N_MAYU_L_B              = 0x56,
    MOTION_BONE_TL_MAYU_L_B_WJ          = 0x57,
    MOTION_BONE_N_MAYU_L_C              = 0x58,
    MOTION_BONE_TL_MAYU_L_C_WJ          = 0x59,
    MOTION_BONE_N_MAYU_R                = 0x5A,
    MOTION_BONE_TL_MAYU_R_WJ            = 0x5B,
    MOTION_BONE_N_MAYU_R_B              = 0x5C,
    MOTION_BONE_TL_MAYU_R_B_WJ          = 0x5D,
    MOTION_BONE_N_MAYU_R_C              = 0x5E,
    MOTION_BONE_TL_MAYU_R_C_WJ          = 0x5F,
    MOTION_BONE_N_TOOTH_UPPER           = 0x60,
    MOTION_BONE_TL_TOOTH_UPPER_WJ       = 0x61,
    MOTION_BONE_N_KUBI_WJ_EX            = 0x62,
    MOTION_BONE_N_WAKI_L                = 0x63,
    MOTION_BONE_KL_WAKI_L_WJ            = 0x64,
    MOTION_BONE_TL_UP_KATA_L            = 0x65,
    MOTION_BONE_C_KATA_L                = 0x66,
    MOTION_BONE_KL_TE_L_WJ              = 0x67,
    MOTION_BONE_N_HITO_L_EX             = 0x68,
    MOTION_BONE_NL_HITO_L_WJ            = 0x69,
    MOTION_BONE_NL_HITO_B_L_WJ          = 0x6A,
    MOTION_BONE_NL_HITO_C_L_WJ          = 0x6B,
    MOTION_BONE_N_KO_L_EX               = 0x6C,
    MOTION_BONE_NL_KO_L_WJ              = 0x6D,
    MOTION_BONE_NL_KO_B_L_WJ            = 0x6E,
    MOTION_BONE_NL_KO_C_L_WJ            = 0x6F,
    MOTION_BONE_N_KUSU_L_EX             = 0x70,
    MOTION_BONE_NL_KUSU_L_WJ            = 0x71,
    MOTION_BONE_NL_KUSU_B_L_WJ          = 0x72,
    MOTION_BONE_NL_KUSU_C_L_WJ          = 0x73,
    MOTION_BONE_N_NAKA_L_EX             = 0x74,
    MOTION_BONE_NL_NAKA_L_WJ            = 0x75,
    MOTION_BONE_NL_NAKA_B_L_WJ          = 0x76,
    MOTION_BONE_NL_NAKA_C_L_WJ          = 0x77,
    MOTION_BONE_N_OYA_L_EX              = 0x78,
    MOTION_BONE_NL_OYA_L_WJ             = 0x79,
    MOTION_BONE_NL_OYA_B_L_WJ           = 0x7A,
    MOTION_BONE_NL_OYA_C_L_WJ           = 0x7B,
    MOTION_BONE_N_STE_L_WJ_EX           = 0x7C,
    MOTION_BONE_N_SUDE_L_WJ_EX          = 0x7D,
    MOTION_BONE_N_SUDE_B_L_WJ_EX        = 0x7E,
    MOTION_BONE_N_HIJI_L_WJ_EX          = 0x7F,
    MOTION_BONE_N_UP_KATA_L_EX          = 0x80,
    MOTION_BONE_N_SKATA_L_WJ_CD_EX      = 0x81,
    MOTION_BONE_N_SKATA_B_L_WJ_CD_CU_EX = 0x82,
    MOTION_BONE_N_SKATA_C_L_WJ_CD_CU_EX = 0x83,
    MOTION_BONE_N_WAKI_R                = 0x84,
    MOTION_BONE_KL_WAKI_R_WJ            = 0x85,
    MOTION_BONE_TL_UP_KATA_R            = 0x86,
    MOTION_BONE_C_KATA_R                = 0x87,
    MOTION_BONE_KL_TE_R_WJ              = 0x88,
    MOTION_BONE_N_HITO_R_EX             = 0x89,
    MOTION_BONE_NL_HITO_R_WJ            = 0x8A,
    MOTION_BONE_NL_HITO_B_R_WJ          = 0x8B,
    MOTION_BONE_NL_HITO_C_R_WJ          = 0x8C,
    MOTION_BONE_N_KO_R_EX               = 0x8D,
    MOTION_BONE_NL_KO_R_WJ              = 0x8E,
    MOTION_BONE_NL_KO_B_R_WJ            = 0x8F,
    MOTION_BONE_NL_KO_C_R_WJ            = 0x90,
    MOTION_BONE_N_KUSU_R_EX             = 0x91,
    MOTION_BONE_NL_KUSU_R_WJ            = 0x92,
    MOTION_BONE_NL_KUSU_B_R_WJ          = 0x93,
    MOTION_BONE_NL_KUSU_C_R_WJ          = 0x94,
    MOTION_BONE_N_NAKA_R_EX             = 0x95,
    MOTION_BONE_NL_NAKA_R_WJ            = 0x96,
    MOTION_BONE_NL_NAKA_B_R_WJ          = 0x97,
    MOTION_BONE_NL_NAKA_C_R_WJ          = 0x98,
    MOTION_BONE_N_OYA_R_EX              = 0x99,
    MOTION_BONE_NL_OYA_R_WJ             = 0x9A,
    MOTION_BONE_NL_OYA_B_R_WJ           = 0x9B,
    MOTION_BONE_NL_OYA_C_R_WJ           = 0x9C,
    MOTION_BONE_N_STE_R_WJ_EX           = 0x9D,
    MOTION_BONE_N_SUDE_R_WJ_EX          = 0x9E,
    MOTION_BONE_N_SUDE_B_R_WJ_EX        = 0x9F,
    MOTION_BONE_N_HIJI_R_WJ_EX          = 0xA0,
    MOTION_BONE_N_UP_KATA_R_EX          = 0xA1,
    MOTION_BONE_N_SKATA_R_WJ_CD_EX      = 0xA2,
    MOTION_BONE_N_SKATA_B_R_WJ_CD_CU_EX = 0xA3,
    MOTION_BONE_N_SKATA_C_R_WJ_CD_CU_EX = 0xA4,
    MOTION_BONE_KL_KOSI_Y               = 0xA5,
    MOTION_BONE_KL_KOSI_XZ              = 0xA6,
    MOTION_BONE_KL_KOSI_ETC_WJ          = 0xA7,
    MOTION_BONE_CL_MOMO_L               = 0xA8,
    MOTION_BONE_KL_ASI_L_WJ_CO          = 0xA9,
    MOTION_BONE_KL_TOE_L_WJ             = 0xAA,
    MOTION_BONE_N_HIZA_L_WJ_EX          = 0xAB,
    MOTION_BONE_CL_MOMO_R               = 0xAC,
    MOTION_BONE_KL_ASI_R_WJ_CO          = 0xAD,
    MOTION_BONE_KL_TOE_R_WJ             = 0xAE,
    MOTION_BONE_N_HIZA_R_WJ_EX          = 0xAF,
    MOTION_BONE_N_MOMO_A_L_WJ_CD_EX     = 0xB0,
    MOTION_BONE_N_MOMO_B_L_WJ_EX        = 0xB1,
    MOTION_BONE_N_MOMO_C_L_WJ_EX        = 0xB2,
    MOTION_BONE_N_MOMO_A_R_WJ_CD_EX     = 0xB3,
    MOTION_BONE_N_MOMO_B_R_WJ_EX        = 0xB4,
    MOTION_BONE_N_MOMO_C_R_WJ_EX        = 0xB5,
    MOTION_BONE_N_HARA_CD_EX            = 0xB6,
    MOTION_BONE_N_HARA_B_WJ_EX          = 0xB7,
    MOTION_BONE_N_HARA_C_WJ_EX          = 0xB8,
    MOTION_BONE_MAX                     = 0xB9,
};

enum rob_bone_index {
    ROB_BONE_NONE                    = -1,
    ROB_BONE_N_HARA_CP               = 0x00,
    ROB_BONE_KG_HARA_Y               = 0x01,
    ROB_BONE_KL_HARA_XZ              = 0x02,
    ROB_BONE_KL_HARA_ETC             = 0x03,
    ROB_BONE_N_HARA                  = 0x04,
    ROB_BONE_CL_MUNE                 = 0x05,
    ROB_BONE_N_MUNE_B                = 0x06,
    ROB_BONE_KL_MUNE_B_WJ            = 0x07,
    ROB_BONE_KL_KUBI                 = 0x08,
    ROB_BONE_N_KAO                   = 0x09,
    ROB_BONE_CL_KAO                  = 0x0A,
    ROB_BONE_FACE_ROOT               = 0x0B,
    ROB_BONE_N_AGO                   = 0x0C,
    ROB_BONE_KL_AGO_WJ               = 0x0D,
    ROB_BONE_N_TOOTH_UNDER           = 0x0E,
    ROB_BONE_TL_TOOTH_UNDER_WJ       = 0x0F,
    ROB_BONE_N_EYE_L                 = 0x10,
    ROB_BONE_KL_EYE_L                = 0x11,
    ROB_BONE_N_EYE_L_WJ_EX           = 0x12,
    ROB_BONE_KL_HIGHLIGHT_L_WJ       = 0x13,
    ROB_BONE_N_EYE_R                 = 0x14,
    ROB_BONE_KL_EYE_R                = 0x15,
    ROB_BONE_N_EYE_R_WJ_EX           = 0x16,
    ROB_BONE_KL_HIGHLIGHT_R_WJ       = 0x17,
    ROB_BONE_N_EYELID_L_A            = 0x18,
    ROB_BONE_TL_EYELID_L_A_WJ        = 0x19,
    ROB_BONE_N_EYELID_L_B            = 0x1A,
    ROB_BONE_TL_EYELID_L_B_WJ        = 0x1B,
    ROB_BONE_N_EYELID_R_A            = 0x1C,
    ROB_BONE_TL_EYELID_R_A_WJ        = 0x1D,
    ROB_BONE_N_EYELID_R_B            = 0x1E,
    ROB_BONE_TL_EYELID_R_B_WJ        = 0x1F,
    ROB_BONE_N_KUTI_D                = 0x20,
    ROB_BONE_TL_KUTI_D_WJ            = 0x21,
    ROB_BONE_N_KUTI_D_L              = 0x22,
    ROB_BONE_TL_KUTI_D_L_WJ          = 0x23,
    ROB_BONE_N_KUTI_D_R              = 0x24,
    ROB_BONE_TL_KUTI_D_R_WJ          = 0x25,
    ROB_BONE_N_KUTI_DS_L             = 0x26,
    ROB_BONE_TL_KUTI_DS_L_WJ         = 0x27,
    ROB_BONE_N_KUTI_DS_R             = 0x28,
    ROB_BONE_TL_KUTI_DS_R_WJ         = 0x29,
    ROB_BONE_N_KUTI_L                = 0x2A,
    ROB_BONE_TL_KUTI_L_WJ            = 0x2B,
    ROB_BONE_N_KUTI_M_L              = 0x2C,
    ROB_BONE_TL_KUTI_M_L_WJ          = 0x2D,
    ROB_BONE_N_KUTI_M_R              = 0x2E,
    ROB_BONE_TL_KUTI_M_R_WJ          = 0x2F,
    ROB_BONE_N_KUTI_R                = 0x30,
    ROB_BONE_TL_KUTI_R_WJ            = 0x31,
    ROB_BONE_N_KUTI_U                = 0x32,
    ROB_BONE_TL_KUTI_U_WJ            = 0x33,
    ROB_BONE_N_KUTI_U_L              = 0x34,
    ROB_BONE_TL_KUTI_U_L_WJ          = 0x35,
    ROB_BONE_N_KUTI_U_R              = 0x36,
    ROB_BONE_TL_KUTI_U_R_WJ          = 0x37,
    ROB_BONE_N_MABU_L_D_A            = 0x38,
    ROB_BONE_TL_MABU_L_D_A_WJ        = 0x39,
    ROB_BONE_N_MABU_L_D_B            = 0x3A,
    ROB_BONE_TL_MABU_L_D_B_WJ        = 0x3B,
    ROB_BONE_N_MABU_L_D_C            = 0x3C,
    ROB_BONE_TL_MABU_L_D_C_WJ        = 0x3D,
    ROB_BONE_N_MABU_L_U_A            = 0x3E,
    ROB_BONE_TL_MABU_L_U_A_WJ        = 0x3F,
    ROB_BONE_N_MABU_L_U_B            = 0x40,
    ROB_BONE_TL_MABU_L_U_B_WJ        = 0x41,
    ROB_BONE_N_EYELASHES_L           = 0x42,
    ROB_BONE_TL_EYELASHES_L_WJ       = 0x43,
    ROB_BONE_N_MABU_L_U_C            = 0x44,
    ROB_BONE_TL_MABU_L_U_C_WJ        = 0x45,
    ROB_BONE_N_MABU_R_D_A            = 0x46,
    ROB_BONE_TL_MABU_R_D_A_WJ        = 0x47,
    ROB_BONE_N_MABU_R_D_B            = 0x48,
    ROB_BONE_TL_MABU_R_D_B_WJ        = 0x49,
    ROB_BONE_N_MABU_R_D_C            = 0x4A,
    ROB_BONE_TL_MABU_R_D_C_WJ        = 0x4B,
    ROB_BONE_N_MABU_R_U_A            = 0x4C,
    ROB_BONE_TL_MABU_R_U_A_WJ        = 0x4D,
    ROB_BONE_N_MABU_R_U_B            = 0x4E,
    ROB_BONE_TL_MABU_R_U_B_WJ        = 0x4F,
    ROB_BONE_N_EYELASHES_R           = 0x50,
    ROB_BONE_TL_EYELASHES_R_WJ       = 0x51,
    ROB_BONE_N_MABU_R_U_C            = 0x52,
    ROB_BONE_TL_MABU_R_U_C_WJ        = 0x53,
    ROB_BONE_N_MAYU_L                = 0x54,
    ROB_BONE_TL_MAYU_L_WJ            = 0x55,
    ROB_BONE_N_MAYU_L_B              = 0x56,
    ROB_BONE_TL_MAYU_L_B_WJ          = 0x57,
    ROB_BONE_N_MAYU_L_C              = 0x58,
    ROB_BONE_TL_MAYU_L_C_WJ          = 0x59,
    ROB_BONE_N_MAYU_R                = 0x5A,
    ROB_BONE_TL_MAYU_R_WJ            = 0x5B,
    ROB_BONE_N_MAYU_R_B              = 0x5C,
    ROB_BONE_TL_MAYU_R_B_WJ          = 0x5D,
    ROB_BONE_N_MAYU_R_C              = 0x5E,
    ROB_BONE_TL_MAYU_R_C_WJ          = 0x5F,
    ROB_BONE_N_TOOTH_UPPER           = 0x60,
    ROB_BONE_TL_TOOTH_UPPER_WJ       = 0x61,
    ROB_BONE_N_KUBI_WJ_EX            = 0x62,
    ROB_BONE_N_WAKI_L                = 0x63,
    ROB_BONE_KL_WAKI_L_WJ            = 0x64,
    ROB_BONE_TL_UP_KATA_L            = 0x65,
    ROB_BONE_C_KATA_L                = 0x66,
    ROB_BONE_KATA_L_WJ_CU            = 0x67,
    ROB_BONE_UDE_L_WJ                = 0x68,
    ROB_BONE_KL_TE_L_WJ              = 0x69,
    ROB_BONE_N_HITO_L_EX             = 0x6A,
    ROB_BONE_NL_HITO_L_WJ            = 0x6B,
    ROB_BONE_NL_HITO_B_L_WJ          = 0x6C,
    ROB_BONE_NL_HITO_C_L_WJ          = 0x6D,
    ROB_BONE_N_KO_L_EX               = 0x6E,
    ROB_BONE_NL_KO_L_WJ              = 0x6F,
    ROB_BONE_NL_KO_B_L_WJ            = 0x70,
    ROB_BONE_NL_KO_C_L_WJ            = 0x71,
    ROB_BONE_N_KUSU_L_EX             = 0x72,
    ROB_BONE_NL_KUSU_L_WJ            = 0x73,
    ROB_BONE_NL_KUSU_B_L_WJ          = 0x74,
    ROB_BONE_NL_KUSU_C_L_WJ          = 0x75,
    ROB_BONE_N_NAKA_L_EX             = 0x76,
    ROB_BONE_NL_NAKA_L_WJ            = 0x77,
    ROB_BONE_NL_NAKA_B_L_WJ          = 0x78,
    ROB_BONE_NL_NAKA_C_L_WJ          = 0x79,
    ROB_BONE_N_OYA_L_EX              = 0x7A,
    ROB_BONE_NL_OYA_L_WJ             = 0x7B,
    ROB_BONE_NL_OYA_B_L_WJ           = 0x7C,
    ROB_BONE_NL_OYA_C_L_WJ           = 0x7D,
    ROB_BONE_N_STE_L_WJ_EX           = 0x7E,
    ROB_BONE_N_SUDE_L_WJ_EX          = 0x7F,
    ROB_BONE_N_SUDE_B_L_WJ_EX        = 0x80,
    ROB_BONE_N_HIJI_L_WJ_EX          = 0x81,
    ROB_BONE_N_UP_KATA_L_EX          = 0x82,
    ROB_BONE_N_SKATA_L_WJ_CD_EX      = 0x83,
    ROB_BONE_N_SKATA_B_L_WJ_CD_CU_EX = 0x84,
    ROB_BONE_N_SKATA_C_L_WJ_CD_CU_EX = 0x85,
    ROB_BONE_N_WAKI_R                = 0x86,
    ROB_BONE_KL_WAKI_R_WJ            = 0x87,
    ROB_BONE_TL_UP_KATA_R            = 0x88,
    ROB_BONE_C_KATA_R                = 0x89,
    ROB_BONE_KATA_R_WJ_CU            = 0x8A,
    ROB_BONE_UDE_R_WJ                = 0x8B,
    ROB_BONE_KL_TE_R_WJ              = 0x8C,
    ROB_BONE_N_HITO_R_EX             = 0x8D,
    ROB_BONE_NL_HITO_R_WJ            = 0x8E,
    ROB_BONE_NL_HITO_B_R_WJ          = 0x8F,
    ROB_BONE_NL_HITO_C_R_WJ          = 0x90,
    ROB_BONE_N_KO_R_EX               = 0x91,
    ROB_BONE_NL_KO_R_WJ              = 0x92,
    ROB_BONE_NL_KO_B_R_WJ            = 0x93,
    ROB_BONE_NL_KO_C_R_WJ            = 0x94,
    ROB_BONE_N_KUSU_R_EX             = 0x95,
    ROB_BONE_NL_KUSU_R_WJ            = 0x96,
    ROB_BONE_NL_KUSU_B_R_WJ          = 0x97,
    ROB_BONE_NL_KUSU_C_R_WJ          = 0x98,
    ROB_BONE_N_NAKA_R_EX             = 0x99,
    ROB_BONE_NL_NAKA_R_WJ            = 0x9A,
    ROB_BONE_NL_NAKA_B_R_WJ          = 0x9B,
    ROB_BONE_NL_NAKA_C_R_WJ          = 0x9C,
    ROB_BONE_N_OYA_R_EX              = 0x9D,
    ROB_BONE_NL_OYA_R_WJ             = 0x9E,
    ROB_BONE_NL_OYA_B_R_WJ           = 0x9F,
    ROB_BONE_NL_OYA_C_R_WJ           = 0xA0,
    ROB_BONE_N_STE_R_WJ_EX           = 0xA1,
    ROB_BONE_N_SUDE_R_WJ_EX          = 0xA2,
    ROB_BONE_N_SUDE_B_R_WJ_EX        = 0xA3,
    ROB_BONE_N_HIJI_R_WJ_EX          = 0xA4,
    ROB_BONE_N_UP_KATA_R_EX          = 0xA5,
    ROB_BONE_N_SKATA_R_WJ_CD_EX      = 0xA6,
    ROB_BONE_N_SKATA_B_R_WJ_CD_CU_EX = 0xA7,
    ROB_BONE_N_SKATA_C_R_WJ_CD_CU_EX = 0xA8,
    ROB_BONE_KL_KOSI_Y               = 0xA9,
    ROB_BONE_KL_KOSI_XZ              = 0xAA,
    ROB_BONE_KL_KOSI_ETC_WJ          = 0xAB,
    ROB_BONE_CL_MOMO_L               = 0xAC,
    ROB_BONE_J_MOMO_L_WJ             = 0xAD,
    ROB_BONE_J_SUNE_L_WJ             = 0xAE,
    ROB_BONE_KL_ASI_L_WJ_CO          = 0xAF,
    ROB_BONE_KL_TOE_L_WJ             = 0xB0,
    ROB_BONE_N_HIZA_L_WJ_EX          = 0xB1,
    ROB_BONE_CL_MOMO_R               = 0xB2,
    ROB_BONE_J_MOMO_R_WJ             = 0xB3,
    ROB_BONE_J_SUNE_R_WJ             = 0xB4,
    ROB_BONE_KL_ASI_R_WJ_CO          = 0xB5,
    ROB_BONE_KL_TOE_R_WJ             = 0xB6,
    ROB_BONE_N_HIZA_R_WJ_EX          = 0xB7,
    ROB_BONE_N_MOMO_A_L_WJ_CD_EX     = 0xB8,
    ROB_BONE_N_MOMO_B_L_WJ_EX        = 0xB9,
    ROB_BONE_N_MOMO_C_L_WJ_EX        = 0xBA,
    ROB_BONE_N_MOMO_A_R_WJ_CD_EX     = 0xBB,
    ROB_BONE_N_MOMO_B_R_WJ_EX        = 0xBC,
    ROB_BONE_N_MOMO_C_R_WJ_EX        = 0xBD,
    ROB_BONE_N_HARA_CD_EX            = 0xBE,
    ROB_BONE_N_HARA_B_WJ_EX          = 0xBF,
    ROB_BONE_N_HARA_C_WJ_EX          = 0xC0,
    ROB_BONE_MAX                     = 0xC1,
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

enum skin_param_osage_root_coli_type {
    SKIN_PARAM_OSAGE_ROOT_COLI_TYPE_NONE     = 0x00,
    SKIN_PARAM_OSAGE_ROOT_COLI_TYPE_BALL     = 0x01,
    SKIN_PARAM_OSAGE_ROOT_COLI_TYPE_CYLINDER = 0x02,
    SKIN_PARAM_OSAGE_ROOT_COLI_TYPE_PLANE    = 0x03,
    SKIN_PARAM_OSAGE_ROOT_COLI_TYPE_ELLIPSE  = 0x04,
    SKIN_PARAM_OSAGE_ROOT_COLI_TYPE_5        = 0x05,
};

enum SubActExecType {
    SUB_ACTION_EXECUTE_NONE        = 0,
    SUB_ACTION_EXECUTE_CRY         = 1,
    SUB_ACTION_EXECUTE_SHAKE_HAND  = 2,
    SUB_ACTION_EXECUTE_EMBARRASSED = 3,
    SUB_ACTION_EXECUTE_ANGRY       = 4,
    SUB_ACTION_EXECUTE_LAUGH       = 5,
    SUB_ACTION_EXECUTE_COUNT_NUM   = 6,
};

enum SubActParamType  {
    SUB_ACTION_PARAM_NONE        = 0x0,
    SUB_ACTION_PARAM_CRY         = 0x1,
    SUB_ACTION_PARAM_SHAKE_HAND  = 0x2,
    SUB_ACTION_PARAM_EMBARRASSED = 0x3,
    SUB_ACTION_PARAM_ANGRY       = 0x4,
    SUB_ACTION_PARAM_LAUGH       = 0x5,
    SUB_ACTION_PARAM_COUNT_NUM   = 0x6,
};

struct rob_chara;
struct rob_chara_bone_data;

struct bone_node_expression_data {
    vec3 position;
    vec3 rotation;
    vec3 scale;
    vec3 parent_scale;

    bone_node_expression_data();
};

struct bone_node {
    const char* name;
    mat4* mat;
    bone_node* parent;
    bone_node_expression_data exp_data;
    mat4* ex_data_mat;

    bone_node();
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
    std::vector<mot_key_set> key_set;
    mot mot;
    std::vector<float_t> key_set_data;
    const mot_data* mot_data;
    bone_database_skeleton_type skeleton_type;
    int32_t skeleton_select;
    int32_t motion_id;
    float_t frame;
    struc_369 field_68;

    mot_key_data();
    ~mot_key_data();
};

struct bone_data {
    bone_database_bone_type type;
    int32_t has_parent;
    motion_bone_index motion_bone_index;
    int32_t mirror;
    int32_t parent;
    int32_t flags;
    int32_t key_set_offset;
    int32_t key_set_count;
    float_t frame;
    vec3 base_translation[2];
    vec3 rotation;
    vec3 ik_target;
    vec3 trans;
    mat4u rot_mat[3];
    vec3 trans_prev[2];
    mat4u rot_mat_prev[3][2];
    mat4* pole_target_mat;
    mat4* parent_mat;
    bone_node* node;
    float_t ik_segment_length[2];
    float_t ik_2nd_segment_length[2];
    float_t field_2E0;
    float_t eyes_xrot_adjust_neg;
    float_t eyes_xrot_adjust_pos;

    bone_data();
};

struct bone_data_parent {
    rob_chara_bone_data* rob_bone_data;
    size_t motion_bone_count;
    size_t ik_bone_count;
    size_t chain_pos;
    std::vector<bone_data> bones;
    std::vector<uint16_t> bone_indices;
    vec3 global_trans;
    vec3 global_rotation;
    uint32_t bone_key_set_count;
    uint32_t global_key_set_count;
    float_t rotation_y;

    bone_data_parent();
    ~bone_data_parent();
};

struct mot_play_frame_data {
    float_t frame;
    float_t step;
    float_t step_prev;
    float_t frame_count;
    float_t last_frame;
    float_t field_14;
    int32_t field_18;
    int32_t field_1C;
    float_t field_20;
    float_t field_24;
    bool field_28;
    int32_t field_2C;
};

struct mot_play_data {
    mot_play_frame_data frame_data;
    int32_t field_30;
    bool field_34;
    float_t field_38;
    float_t* field_40;
    float_t* field_48;
};

struct struc_308 {
    int32_t field_0;
    int32_t field_4;
    uint8_t field_8;
    mat4u mat;
    mat4u field_4C;
    bool field_8C;
    vec3 field_90;
    vec3 field_9C;
    vec3 field_A8;
    float_t eyes_adjust;
    float_t prev_eyes_adjust;
    uint8_t field_BC;
    uint8_t field_BD;
    float_t field_C0;
    float_t field_C4;
    vec3 field_C8;
};

struct struc_400 {
    bool field_0;
    bool field_1;
    bool field_2;
    bool field_3;
    bool field_4;
    float_t frame;
    float_t eyes_adjust;
    float_t prev_eyes_adjust;
};

class MotionBlend {
public:
    bool field_8;
    bool field_9;
    float_t duration;
    float_t frame;
    float_t step;
    float_t offset;
    float_t blend;

    MotionBlend();
    virtual ~MotionBlend();
    virtual void Reset();
    virtual void Field_10(float_t a2, float_t a3, int32_t a4) = 0;
    virtual void Field_18(struc_400*) = 0;
    virtual void Field_20(std::vector<bone_data>*, std::vector<bone_data>*) = 0;
    virtual void Blend(bone_data*, bone_data*) = 0;
    virtual bool Field_30();

    void SetDuration(float_t duration, float_t step, float_t offset);
};

class MotionBlendCross : public MotionBlend {
public:
    bool field_20;
    bool field_21;
    mat4u field_24;
    mat4u field_64;
    mat4u field_A4;
    mat4u field_E4;

    MotionBlendCross();
    virtual ~MotionBlendCross() override;
    virtual void Reset() override;
    virtual void Field_10(float_t a2, float_t a3, int32_t a4) override;
    virtual void Field_18(struc_400*) override;
    virtual void Field_20(std::vector<bone_data>*, std::vector<bone_data>*) override;
    virtual void Blend(bone_data*, bone_data*) override;
};

class MotionBlendCombine : public MotionBlendCross {
public:
    MotionBlendCombine();
    virtual ~MotionBlendCombine() override;
    virtual void Field_18(struc_400*) override;
    virtual bool Field_30() override;
};

class MotionBlendFreeze : public MotionBlend {
public:
    bool field_20;
    bool field_21;
    int32_t field_24;
    float_t field_28;
    float_t field_2C;
    int32_t field_30;
    mat4u field_34;
    mat4u field_74;
    mat4u field_B4;
    mat4u field_F4;

    MotionBlendFreeze();
    virtual ~MotionBlendFreeze() override;
    virtual void Reset() override;
    virtual void Field_10(float_t a2, float_t a3, int32_t a4) override;
    virtual void Field_18(struc_400*) override;
    virtual void Field_20(std::vector<bone_data>*, std::vector<bone_data>*) override;
    virtual void Blend(bone_data*, bone_data*) override;
};

class PartialMotionBlendFreeze : public MotionBlendFreeze {
public:
    PartialMotionBlendFreeze();
    virtual ~PartialMotionBlendFreeze() override;
    virtual void Reset() override;
    virtual void Field_10(float_t a2, float_t a3, int32_t a4) override;
    virtual void Field_18(struc_400*) override;
    virtual void Field_20(std::vector<bone_data>*, std::vector<bone_data>*) override;
    virtual void Blend(bone_data*, bone_data*) override;
};

struct struc_313 {
    std::vector<uint32_t> bitfield;
    size_t motion_bone_count;

    struc_313();
    ~struc_313();
};

struct struc_240 {
    bool(*bone_check_func)(motion_bone_index bone_index);
    struc_313 field_8;
    size_t motion_bone_count;

    struc_240();
    ~struc_240();
};

struct motion_blend_mot {
    struc_240 field_0;
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
    virtual ~motion_blend_mot();
};

struct rob_chara_bone_data_ik_scale {
    float_t ratio0;
    float_t ratio1;
    float_t ratio2;
    float_t ratio3;

    rob_chara_bone_data_ik_scale();
};

struct mot_blend {
    struc_240 field_0;
    bool field_30;
    mot_key_data mot_key_data;
    mot_play_data mot_play_data;
    PartialMotionBlendFreeze blend;

    mot_blend();
    ~mot_blend();
};

struct struc_241 {
    float_t field_0;
    float_t field_4;
    float_t field_8;
    float_t field_C;
    float_t field_10;
    float_t field_14;
    float_t field_18;
    float_t field_1C;
    float_t field_20;
    float_t field_24;
    float_t field_28;
    float_t field_2C;
    float_t field_30;
    float_t field_34;
    float_t field_38;
    float_t field_3C;
    float_t field_40;
};

struct struc_243 {
    float_t field_0;
    float_t field_4;
    float_t field_8;
    float_t field_C;
    int32_t field_10;
    int32_t field_14;
    int32_t field_18;
    int32_t field_1C;
    float_t field_20;
    float_t field_24;
};

struct struc_258 {
    std::vector<bone_data>* field_0;
    mat4u field_8;
    struc_241 field_48;
    bool field_8C;
    bool field_8D;
    bool field_8E;
    bool field_8F;
    bool field_90;
    bool field_91;
    float_t field_94;
    float_t field_98;
    float_t field_9C;
    float_t field_A0;
    float_t field_A4;
    float_t field_A8;
    float_t field_AC;
    float_t field_B0;
    vec3 field_B4;
    vec3 field_C0;
    mat4u field_CC;
    mat4u field_10C;
    bool field_14C;
    float_t field_150;
    float_t field_154;
    float_t field_158;
    struc_243 field_15C;
    float_t field_184;
    float_t field_188;
    float_t field_18C;
    bool field_190;
    bool field_191;
    bool field_192;
    bool field_193;
    bool field_194;
    bool field_195;
    float_t field_198;
    float_t field_19C;
    float_t field_1A0;
    int32_t field_1A4;
    int32_t field_1A8;
    float_t field_1AC;
    float_t field_1B0;
    int32_t field_1B4;
    int32_t field_1B8;
    vec2 field_1BC;
    bool field_1C4;
    int32_t field_1C8;
    int32_t field_1CC;
};

struct struc_242 {
    float_t field_0;
    float_t field_4;
    float_t field_8;
    float_t field_C;
    float_t field_10;
    float_t field_14;
    float_t field_18;
    int32_t field_1C;
    float_t field_20;
    float_t field_24;
    float_t field_28;
};

struct struc_312 {
    struc_242 field_0;
    struc_242 field_2C;
    bool field_58;
    bool field_59;
    vec3 field_5C;
    vec3 field_68;
    vec3 field_74;
    vec3 field_80;
    float_t field_8C;
    std::vector<bone_data>* field_90;
    float_t field_98;
};

struct rob_chara_bone_data {
    bool field_0;
    bool field_1;
    size_t object_bone_count;
    size_t total_bone_count;
    size_t motion_bone_count;
    size_t ik_bone_count;
    size_t chain_pos;
    std::vector<mat4> mats;
    std::vector<mat4> mats2;
    std::vector<bone_node> nodes;
    bone_database_skeleton_type base_skeleton_type;
    bone_database_skeleton_type skeleton_type;
    std::vector<motion_blend_mot*> motions;
    std::list<size_t> motion_indices;
    std::list<size_t> motion_loaded_indices;
    std::list<motion_blend_mot*> motion_loaded;
    mot_blend face;
    mot_blend hand_l;
    mot_blend hand_r;
    mot_blend mouth;
    mot_blend eyes;
    mot_blend eyelid;
    bool field_758;
    rob_chara_bone_data_ik_scale ik_scale;
    vec3 field_76C;
    vec3 field_778;
    int32_t field_784;
    struc_258 field_788;
    struc_312 field_958;

    rob_chara_bone_data();
    virtual ~rob_chara_bone_data();
};

union rob_chara_pv_data_customize_items {
    struct {
        int32_t head;
        int32_t face;
        int32_t chest;
        int32_t back;
    };
    int32_t arr[4];
};

struct rob_chara_pv_data {
    rob_chara_type type;
    bool field_4;
    bool field_5;
    bool field_6;
    vec3 field_8;
    int16_t rot_y_int16;
    int16_t field_16;
    struc_242 field_18;
    struc_242 field_44;
    int32_t field_70;
    int32_t field_74[10];
    int32_t chara_size_index;
    bool height_adjust;
    rob_chara_pv_data_customize_items customize_items;
    eyes_adjust eyes_adjust;

    rob_chara_pv_data();
    virtual ~rob_chara_pv_data();
};

struct struc_218 {
    vec3 bone_offset;
    float_t scale;
    int32_t bone_index;
    int32_t field_14;
};

struct struc_344 {
    int32_t chara_size_index;
    int32_t field_4;
    int32_t field_8;
    int32_t field_C;
};

struct chara_init_data {
    int32_t object_set;
    bone_database_skeleton_type skeleton_type;
    object_info field_7E0[15];
    uint32_t motion_set;
    const struc_218* field_828;
    const struc_218* field_830;
    int32_t field_840;
    int32_t field_844;
    struc_344 field_848;
    object_info head_objects[9];
    object_info face_objects[15];
};

struct rob_chara_item_equip_object;

class ExNodeBlock {
public:
    bone_node* bone_node_ptr;
    ex_node_type type;
    const char* name;
    bone_node* parent_bone_node;
    std::string parent_name;
    ExNodeBlock* parent_node;
    rob_chara_item_equip_object* item_equip_object;
    bool field_58;
    bool field_59;
    bool has_children_node;

    ExNodeBlock();
    virtual ~ExNodeBlock();
    virtual void Init() = 0;
    virtual void Field_10() = 0;
    virtual void Field_18(int32_t stage, bool a3) = 0;
    virtual void Field_20() = 0;
    virtual void SetOsagePlayData() = 0;
    virtual void Disp() = 0;
    virtual void Reset();
    virtual void Field_40() = 0;
    virtual void Field_48() = 0;
    virtual void Field_50() = 0;
    virtual void Field_58();

    void InitData(bone_node* bone_node, ex_node_type type,
        const char* name, rob_chara_item_equip_object* itm_eq_obj);
};

class ExNullBlock : public ExNodeBlock {
public:
    obj_skin_block_constraint* cns_data;

    ExNullBlock();
    virtual ~ExNullBlock() override;
    virtual void Init() override;
    virtual void Field_10();
    virtual void Field_18(int32_t stage, bool a3) override;
    virtual void Field_20() override;
    virtual void SetOsagePlayData() override;
    virtual void Disp() override;
    virtual void Reset();
    virtual void Field_40() override;
    virtual void Field_48() override;
    virtual void Field_50() override;
    virtual void Field_58();

    void InitData(rob_chara_item_equip_object* itm_eq_obj, obj_skin_block_constraint* cns_data,
        const char* cns_data_name, bone_database* bone_data);
};

struct struc_571;

struct ExClothBlock : public ExNodeBlock {
public:
    //rob_cloth rob;
    obj_skin_block_cloth* cls_data;
    mat4* field_2428;
    size_t index;

    ExClothBlock();
    virtual ~ExClothBlock() override;
    virtual void Init() override;
    virtual void Field_10();
    virtual void Field_18(int32_t stage, bool a3) override;
    virtual void Field_20() override;
    virtual void SetOsagePlayData() override;
    virtual void Disp() override;
    virtual void Reset() override;
    virtual void Field_40() override;
    virtual void Field_48() override;
    virtual void Field_50() override;
    virtual void Field_58();

    void AddMotionResetData(int32_t motion_id, float_t frame);
    float_t* LoadOpdData(size_t node_index, float_t* opd_data, size_t opd_count);
    void SetMotionResetData(int32_t motion_id, float_t frame);
    void SetOsageReset();
    void SetSkinParam(struc_571* skp);
};

struct opd_vec3_data {
    float_t* x;
    float_t* y;
    float_t* z;
};

struct rob_osage_node;

struct rob_osage_node_data_normal_ref {
    bool field_0;
    rob_osage_node* n;
    rob_osage_node* u;
    rob_osage_node* d;
    rob_osage_node* l;
    rob_osage_node* r;
    mat4u mat;
};

struct skin_param_hinge {
    float_t ymin;
    float_t ymax;
    float_t zmin;
    float_t zmax;

    skin_param_hinge();
    ~skin_param_hinge();

    void limit();
};

struct skin_param_osage_node {
    float_t coli_r;
    float_t weight;
    float_t inertial_cancel;
    skin_param_hinge hinge;

    skin_param_osage_node();
    ~skin_param_osage_node();
};

struct rob_osage_node_data {
    float_t force;
    std::vector<rob_osage_node*> boc;
    rob_osage_node_data_normal_ref normal_ref;
    skin_param_osage_node skp_osg_node;

    rob_osage_node_data();
    ~rob_osage_node_data();

    void reset();
};

struct struc_477 {
    float_t length;
    vec3 angle;
};

struct struc_476 {
    struc_477 field_0;
    struc_477 field_10;
};

struct rob_osage_node_reset_data {
    vec3 trans;
    vec3 trans_diff;
    vec3 rotation;
    float_t length;
};

struct rob_osage_node {
    float_t length;
    vec3 trans;
    vec3 trans_orig;
    vec3 trans_diff;
    vec3 field_28;
    float_t child_length;
    bone_node* bone_node_ptr;
    mat4* bone_node_mat;
    mat4u mat;
    rob_osage_node* sibling_node;
    float_t max_distance;
    vec3 field_94;
    rob_osage_node_reset_data reset_data;
    float_t field_C8;
    float_t field_CC;
    vec3 external_force;
    float_t force;
    rob_osage_node_data* data_ptr;
    rob_osage_node_data data;
    std::vector<opd_vec3_data> opd_data;
    struc_476 field_1B0;

    rob_osage_node();
    ~rob_osage_node();

    void reset();
};

struct skin_param_osage_root_coli {
    skin_param_osage_root_coli_type type;
    int32_t bone0_index;
    int32_t bone1_index;
    float_t radius;
    vec3 bone0_pos;
    vec3 bone1_pos;

    skin_param_osage_root_coli();
    ~skin_param_osage_root_coli();
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
    std::vector<skin_param_osage_root_coli> coli;
    float_t coli_r;
    float_t friction;
    float_t wind_afc;
    int32_t yz_order;
    std::vector<skin_param_osage_root_boc> boc;
    int32_t coli_type;
    float_t stiffness;
    float_t move_cancel;
    std::string colli_tgt_osg;
    std::vector<skin_param_osage_root_normal_ref> normal_ref;

    skin_param_osage_root();
    ~skin_param_osage_root();
};

struct skin_param {
    std::vector<skin_param_osage_root_coli> coli;
    float_t friction;
    float_t wind_afc;
    float_t air_res;
    vec3 rot;
    vec3 init_rot;
    int32_t coli_type;
    float_t stiffness;
    float_t move_cancel;
    float_t coli_r;
    skin_param_hinge hinge;
    float_t force;
    float_t force_gain;
    std::vector<rob_osage_node>* colli_tgt_osg;

    skin_param();
    ~skin_param();

    void reset();
};

struct struc_571 {
    skin_param skin_param;
    std::vector<rob_osage_node_data> nodes_data;
    bool field_88;

    struc_571();
    ~struc_571();
};

struct osage_coli {
    skin_param_osage_root_coli_type type;
    float_t radius;
    vec3 bone0_pos;
    vec3 bone1_pos;
    vec3 bone_pos_diff;
    float_t bone_pos_diff_length;
    float_t bone_pos_diff_length_squared;
    float_t field_34;

    osage_coli();
    ~osage_coli();
};

struct osage_ring_data {
    float_t ring_rectangle_x;
    float_t ring_rectangle_y;
    float_t ring_rectangle_width;
    float_t ring_rectangle_height;
    float_t ring_height;
    float_t ring_out_height;
    bool field_18;
    std::vector<osage_coli> coli;
    std::vector<skin_param_osage_root_coli> skp_root_coli;

    osage_ring_data();
    ~osage_ring_data();
};

struct osage_setting_osg_cat {
    rob_osage_parts parts;
    size_t exf;

    osage_setting_osg_cat();
};

struct rob_osage {
    skin_param* skin_param_ptr;
    bone_node_expression_data exp_data;
    std::vector<rob_osage_node> nodes;
    rob_osage_node node;
    skin_param skin_param;
    osage_setting_osg_cat osage_setting;
    bool field_2A0;
    bool field_2A1;
    float_t field_2A4;
    osage_coli coli[64];
    osage_coli coli_ring[64];
    vec3 wind_direction;
    float_t field_1EB4;
    int32_t yz_order;
    int32_t field_1EBC;
    mat4* parent_mat_ptr;
    mat4u parent_mat;
    float_t move_cancel;
    bool field_1F0C;
    bool osage_reset;
    bool field_1F0E;
    bool field_1F0F;
    osage_ring_data ring;
    std::map<std::pair<int32_t, int32_t>, std::list<rob_osage_node_reset_data>> motion_reset_data;
    std::list<rob_osage_node_reset_data>* reset_data_list;
    bool set_external_force;
    vec3 external_force;

    rob_osage();
    ~rob_osage();
};

class ExOsageBlock : public ExNodeBlock {
public:
    size_t index;
    rob_osage rob;
    mat4* mats;
    int32_t field_1FF8;
    float_t step;

    ExOsageBlock();
    virtual ~ExOsageBlock() override;
    virtual void Init() override;
    virtual void Field_10();
    virtual void Field_18(int32_t stage, bool a3) override;
    virtual void Field_20() override;
    virtual void SetOsagePlayData() override;
    virtual void Disp() override;
    virtual void Reset() override;
    virtual void Field_40() override;
    virtual void Field_48() override;
    virtual void Field_50() override;
    virtual void Field_58() override;

    void AddMotionResetData(int32_t motion_id, float_t frame);
    void InitData(rob_chara_item_equip_object* itm_eq_obj, obj_skin_block_osage* osg_data,
        const char* osg_data_name, obj_skin_osage_node* osg_nodes, bone_node* bone_nodes,
        bone_node* ex_data_bone_nodes, obj_skin* skin);
    float_t* LoadOpdData(size_t node_index, float_t* opd_data, size_t opd_count);
    void SetMotionResetData(int32_t motion_id, float_t frame);
    void SetOsageReset();
    void SetSkinParam(struc_571* skp);
    void SetWindDirection();
    void sub_1405F3E10(obj_skin_block_osage* osg_data, obj_skin_osage_node* osg_nodes,
        std::vector<std::pair<uint32_t, rob_osage_node*>>* a4,
        std::map<const char*, ExNodeBlock*>* a5);
};

class ExConstraintBlock : public ExNodeBlock {
public:
    obj_skin_block_constraint_type constraint_type;
    bone_node* source_node_bone_node;
    bone_node* direction_up_vector_bone_node;
    obj_skin_block_constraint* cns_data;
    int64_t field_80;

    ExConstraintBlock();
    virtual ~ExConstraintBlock() override;
    virtual void Init() override;
    virtual void Field_10() override;
    virtual void Field_18(int32_t stage, bool a3) override;
    virtual void Field_20() override;
    virtual void SetOsagePlayData() override;
    virtual void Disp() override;
    virtual void Reset();
    virtual void Field_40() override;
    virtual void Field_48() override;
    virtual void Field_50() override;
    virtual void Field_58();

    void Calc();
    void DataSet();
    void InitData(rob_chara_item_equip_object* itm_eq_obj, obj_skin_block_constraint* cns_data,
        const char* cns_data_name, bone_database* bone_data);

    static void sub_1405F10D0(mat4* mat, vec3* a2, float_t a3, float_t a4);
    static void sub_1401EB410(mat4* mat, vec3* a2, vec3* target_offset);
};

struct ex_expression_block_stack;

struct ex_expression_block_stack_number {
    float_t value;
};

struct ex_expression_block_stack_variable {
    float_t* value;
};

struct ex_expression_block_stack_variable_radian {
    float_t* value;
};

struct ex_expression_block_stack_op1 {
    float_t(*func)(float_t v1);
    ex_expression_block_stack* v1;
};

struct ex_expression_block_stack_op2 {
    float_t(*func)(float_t v1, float_t v2);
    ex_expression_block_stack* v1;
    ex_expression_block_stack* v2;
};

struct ex_expression_block_stack_op3 {
    float_t(*func)(float_t v1, float_t v2, float_t v3);
    ex_expression_block_stack* v1;
    ex_expression_block_stack* v2;
    ex_expression_block_stack* v3;
};

struct ex_expression_block_stack {
    ex_expression_block_stack_type type;
    union {
        ex_expression_block_stack_number number;
        ex_expression_block_stack_variable var;
        ex_expression_block_stack_variable_radian var_rad;
        ex_expression_block_stack_op1 op1;
        ex_expression_block_stack_op2 op2;
        ex_expression_block_stack_op3 op3;
    };
};

class ExExpressionBlock : public ExNodeBlock {
public:
    float_t* values[9];
    ex_expression_block_stack_type types[9];
    ex_expression_block_stack* expressions[9];
    ex_expression_block_stack stack_data[384];
    obj_skin_block_expression* exp_data;
    bool field_3D20;
    void(*field_3D28)(bone_node_expression_data*);
    float_t frame;
    bool step;

    ExExpressionBlock();
    virtual ~ExExpressionBlock() override;
    virtual void Init() override;
    virtual void Field_10() override;
    virtual void Field_18(int32_t stage, bool a3) override;
    virtual void Field_20() override;
    virtual void SetOsagePlayData() override;
    virtual void Disp() override;
    virtual void Reset();
    virtual void Field_40() override;
    virtual void Field_48() override;
    virtual void Field_50() override;
    virtual void Field_58();

    void Calc();
    void DataSet();
    void InitData(rob_chara_item_equip_object* itm_eq_obj, obj_skin_block_expression* exp_data,
        const char* exp_data_name, object_info a4, size_t index, bone_database* bone_data);
};

struct ex_data_name_bone_index {
    const char* name;
    int32_t bone_index;
};

struct rob_chara_item_equip;

struct rob_chara_item_equip_object {
    size_t index;
    mat4* mats;
    object_info obj_info;
    int32_t field_14;
    std::vector<texture_pattern_struct> texture_pattern;
    texture_data_struct texture_data;
    bool null_blocks_data_set;
    bone_node_expression_data exp_data;
    float_t alpha;
    draw_task_flags draw_task_flags;
    bool disp;
    int32_t field_A4;
    mat4* mat;
    int32_t osage_iterations;
    bone_node* bone_nodes;
    std::vector<ExNodeBlock*> node_blocks;
    std::vector<bone_node> ex_data_bone_nodes;
    std::vector<mat4> ex_data_mats;
    std::vector<mat4> ex_data_block_mats;
    std::vector<ex_data_name_bone_index> ex_bones;
    int64_t field_138;
    std::vector<ExNullBlock*> null_blocks;
    std::vector<ExOsageBlock*> osage_blocks;
    std::vector<ExConstraintBlock*> constraint_blocks;
    std::vector<ExExpressionBlock*> expression_blocks;
    std::vector<ExClothBlock*> cloth_blocks;
    bool field_1B8;
    int64_t field_1C0;
    bool use_opd;
    obj_skin_ex_data* skin_ex_data;
    obj_skin* skin;
    rob_chara_item_equip* item_equip;

    rob_chara_item_equip_object();
    virtual ~rob_chara_item_equip_object();
};

struct opd_blend_data {
    int32_t motion_id;
    float_t frame;
    float_t frame_count;
    bool field_C;
    MotionBlendType type;
    float_t blend;
};

struct rob_chara_item_equip {
    bone_node* bone_nodes;
    mat4* matrices;
    rob_chara_item_equip_object* item_equip_object;
    int32_t field_18[31];
    bool item_equip_range;
    item_id first_item_equip_object;
    item_id max_item_equip_object;
    int32_t field_A0;
    shadow_type_enum shadow_type;
    vec3 position;
    int32_t eyes_adjust;
    std::vector<texture_pattern_struct> texture_pattern;
    object_info field_D0;
    int32_t field_D4;
    bool field_D8;
    int32_t field_DC;
    vec4u texture_color_coeff;
    float_t wet;
    float_t wind_strength;
    bool chara_color;
    bool npr_flag;
    mat4u mat;
    mat4u field_13C[30];
    int32_t field_8BC;
    int32_t field_8C0;
    int32_t field_8C4;
    int32_t field_8C8;
    int32_t field_8CC;
    int32_t field_8D0;
    int32_t field_8D4;
    int32_t field_8D8;
    int32_t field_8DC;
    int32_t field_8E0;
    int32_t field_8E4;
    int32_t field_8E8;
    int32_t field_8EC;
    int32_t field_8F0;
    int32_t field_8F4;
    int32_t field_8F8;
    int32_t field_8FC;
    int64_t field_900;
    int64_t field_908;
    int64_t field_910;
    int64_t field_918;
    int64_t field_920;
    int64_t field_928;
    int64_t field_930;
    float_t step;
    bool use_opd;
    std::vector<opd_blend_data> opd_blend_data;
    bool parts_short;
    bool parts_append;
    bool parts_white_one_l;

    rob_chara_item_equip();
    virtual ~rob_chara_item_equip();
};

struct item_cos_texture_change_tex {
    texture* org;
    texture* chg;
    bool changed;

    item_cos_texture_change_tex();
    ~item_cos_texture_change_tex();
};

struct rob_chara_item_cos_data {
    ::chara_index chara_index;
    ::chara_index chara_index_2nd;
    item_cos_data cos;
    item_cos_data cos_2nd;
    std::map<int32_t, std::vector<item_cos_texture_change_tex>> texture_change;
    std::map<int32_t, std::vector<uint32_t>> item_change;
    std::map<object_info, item_id> field_F0;
    std::map<int32_t, int32_t > field_100;
    std::vector<texture_pattern_struct> texture_pattern[31];
    std::map<int32_t, object_info> head_replace;

    rob_chara_item_cos_data();
    ~rob_chara_item_cos_data();
};

struct struc_525 {
    int32_t field_0;
    int32_t field_4;
};

struct struc_524 {
    int32_t field_0;
    struc_525 field_4;
    int32_t motion_id;
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
    bool eyes_adjust;
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
    uint8_t field_1A4;
    char** field_1A8;
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

struct RobSubAction {
    struct Data {
        SubActExec* field_0;
        SubActExec* field_8;
        SubActParam* field_10;
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
};

struct struc_389 {
    float_t frame;
    float_t prev_frame;
    float_t last_set_frame;
};

struct struc_406 {
    float_t frame;
    float_t field_4;
    float_t step;
};

class RobPartialMotion {
public:
    struct Data {
        int32_t motion_id;
        int32_t mottbl_index;
        int32_t state;
        float_t frame;
        float_t play_frame_step;
        float_t frame_count;
        float_t duration;
        float_t step;
        float_t offset;
        float_t field_24;
        struc_389* frame_data;
        struc_406* step_data;
        int32_t field_38;

        Data();
        ~Data();
    } data;

    RobPartialMotion();
    virtual ~RobPartialMotion();
    virtual void Reset() = 0;
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

struct struc_405 {
    RobFaceMotion face;
    RobHandMotion hand_l;
    RobHandMotion hand_r;
    RobMouthMotion mouth;
    RobEyesMotion eye;
    RobEyelidMotion eyelid;
    object_info head_object;
    object_info hand_l_object;
    object_info hand_r_object;
    object_info face_object;
    int32_t field_1C0;
    float_t time;

    struc_405();
    ~struc_405();

    void Reset();
};

struct rob_chara_data_parts_adjust {
    bool enable;
    float_t frame;
    float_t transition_frame;
    vec3 curr_external_force;
    float_t curr_force;
    float_t curr_strength;
    int32_t motion_id;
    float_t set_frame;
    float_t force_duration;
    int32_t type;
    int32_t cycle_type;
    bool ignore_gravity;
    vec3 external_force_min;
    vec3 external_force_strength;
    vec3 external_force;
    float_t cycle;
    float_t phase;
    float_t force;
    float_t strength;
    float_t strength_transition;

    void reset();
};

struct rob_chara_data_hand_adjust {
    bool enable;
    int16_t scale_select;
    rob_chara_data_hand_adjust_type type;
    float_t current_scale;
    float_t scale;
    float_t transition_time;
    float_t current_time;
    float_t rotation_blend;
    float_t offset_blend;
    bool enable_scale;
    bool disable_x;
    bool disable_y;
    bool disable_z;
    vec3 offset;
    vec3 field_30;
    float_t field_3C;
    int32_t field_40;

    void reset();
};

struct struc_222 {
    bool field_0;
    float_t field_4;
    float_t field_8;
    float_t field_C;
    float_t duration;
    float_t frame;
};

struct rob_chara_adjust {
    int32_t motion_id;
    int32_t prev_motion_id;
    struc_389 frame_data;
    struc_406 step_data;
    float_t step;
    int32_t field_24;
    int8_t field_28;
    int8_t field_29;
    int8_t field_2A;
    int32_t field_2C;
    uint32_t field_30;
    int16_t field_34;
    int16_t field_36;
    int32_t field_38[32];
    int32_t field_B8[32];
    float_t field_138;
    float_t field_13C;
    vec3 field_140;
    int32_t field_14C;
    struc_405 field_150;
    float_t field_314;
    RobHandMotion hand_l;
    RobHandMotion hand_r;
    object_info hand_l_object;
    object_info hand_r_object;
    struc_405 field_3B0;
    rob_chara_data_parts_adjust parts_adjust[13];
    rob_chara_data_parts_adjust parts_adjust_prev[13];
    rob_chara_data_parts_adjust field_10D8[2];
    rob_chara_data_hand_adjust hand_adjust[2];
    rob_chara_data_hand_adjust hand_adjust_prev[2];
    struc_222 field_12C8[2];

    rob_chara_adjust();
    ~rob_chara_adjust();
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
    void* data;
};

struct mothead_data2 {
    int32_t type;
    void* data;
};

struct mothead_mot {
    struc_228 field_0;
    int16_t field_10;
    int16_t field_12;
    std::vector<mothead_data2> field_18;
    std::vector<mothead_data> data;
    std::vector<int64_t> field_28;

    mothead_mot();
    virtual ~mothead_mot();
};

struct mothead {
    int32_t mot_set_id;
    int32_t first_mot_id;
    int32_t last_mot_id;
    std::vector<mothead_mot*> mots;

    mothead();
    virtual ~mothead();
};

struct mothead_file {
    int32_t mot_set_id;
    int32_t first_mot_id;
    int32_t last_mot_id;
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
    uint32_t field_14;
    uint32_t field_18;
    uint32_t field_1C;
};

struct struc_377 {
    const mothead_data* field_0;
    const mothead_data* field_8;
};

struct struc_226 {
    int8_t field_0[27];
};

struct struc_225 {
    float field_0[27];
};

struct struc_224 {
    int32_t field_0[27];
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
};

struct struc_652 {
    uint32_t motion_id;
    float_t frame_count;
    float_t field_8;
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
    int32_t field_6C;
    float_t field_70;
    float_t field_74;
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
    const mothead_data2* field_248;
    int64_t field_250;
    float_t field_258;
    int32_t field_25C;
    int64_t field_260;
    int64_t field_268;
    int32_t field_270;
    uint16_t field_274;
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
};

struct struc_223 {
    struc_652 field_0;
    struc_651 field_330;
    const int64_t* field_7A0;
    uint32_t motion_set_id;

    struc_223();
    ~struc_223();
};

struct rob_chara_data_miku_rot {
    int16_t rot_y_int16;
    int16_t field_2;
    int16_t field_4;
    int16_t field_6;
    int32_t field_8;
    int32_t field_C;
    float_t field_10;
    int32_t field_14;
    vec3 position;
    vec3 field_24;
    vec3 field_30;
    vec3 field_3C;
    vec3 field_48;
    vec3 field_54;
    int32_t field_60;
    int32_t field_64;
    int32_t field_68;
    mat4u field_6C;
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
    bool get_global_trans;
    vec3 trans;
    mat4u mat;
    float_t left_hand_scale;
    float_t right_hand_scale;
    float_t left_hand_scale_default;
    float_t right_hand_scale_default;
};

struct struc_195 {
    vec3 prev_trans;
    vec3 trans;
    float_t scale;
    float_t field_1C;
    float_t field_20;
    float_t field_24;
};

struct struc_210 {
    vec2 field_0;
    float_t scale;
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
    mat4u field_78[27];
    mat4u field_738[27];
    struc_195 field_DF8[27];
    struc_195 field_1230[27];
    struc_195 field_1668[27];
    struc_210 field_1AA0[27];
    float_t field_1BE4[27];
    std::vector<std::pair<int64_t, float_t>> field_1C50;
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
    uint8_t field_0;
    uint8_t field_1;
    uint8_t field_2;
    uint8_t field_3;
    int32_t field_4;
    struc_264 field_8;
    RobSubAction rob_sub_action;
    rob_chara_adjust adjust;
    struc_223 field_1588;
    rob_chara_data_miku_rot miku_rot;
    rob_chara_adjust_data adjust_data;
    struc_209 field_1E68;
    float_t field_3D90;
    int32_t field_3D94;
    int16_t field_3D98;
    int16_t field_3D9A;
    uint8_t field_3D9C;
    uint8_t field_3D9D;
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

    rob_chara_data();
    ~rob_chara_data();
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
    chara_index chara_index;
    int32_t module_index;
    float_t frame_speed;
    void* field_20;
    rob_chara_bone_data* bone_data;
    rob_chara_item_equip* item_equip;
    rob_chara_item_cos_data item_cos_data;
    rob_chara_data data;
    rob_chara_data data_prev;
    const chara_init_data* chara_init_data;
    rob_chara_pv_data pv_data;

    rob_chara();
    virtual ~rob_chara();

    mat4* get_bone_data_mat(size_t index);
    float_t get_frame();
    float_t get_frame_count();
    int32_t get_rob_cmn_mottbl_motion_id(int32_t id);
    void load_motion(int32_t motion_id, bool a3, float_t frame,
        MotionBlendType blend_type, bone_database* bone_data, motion_database* mot_db);
    void reset_data(rob_chara_pv_data* pv_data,
        bone_database* bone_data, motion_database* mot_db);
    void set_bone_data_frame(float_t frame);
    void set_chara_height_adjust(bool value);
    void set_chara_pos_adjust(vec3* value);
    void set_chara_pos_adjust_y(float_t value);
    void set_chara_size(float_t value);
    void set_eyelid_mottbl_motion_from_face(int32_t a2,
        float_t duration, float_t value, float_t offset, motion_database* mot_db);
    void set_eyelid_mottbl_motion(int32_t type,
        int32_t mottbl_index, float_t value, int32_t state, float_t duration,
        float_t a7, float_t step, int32_t a9, float_t offset, motion_database* mot_db);
    void set_eyes_mottbl_motion(int32_t type,
        int32_t mottbl_index, float_t value, int32_t state, float_t duration,
        float_t a7, float_t step, int32_t a9, float_t offset, motion_database* mot_db);
    void set_face_mottbl_motion(int32_t type, int32_t mottbl_index,
        float_t value, int32_t state, float_t duration, float_t a7,
        float_t step, int32_t a9, float_t offset, bool a11, motion_database* mot_db);
    void set_frame(float_t frame);
    void set_hand_l_mottbl_motion(int32_t type, int32_t mottbl_index,
        float_t value, int32_t state, float_t duration, float_t a7,
        float_t step, int32_t a9, float_t offset, motion_database* mot_db);
    void set_hand_r_mottbl_motion(int32_t type, int32_t mottbl_index,
        float_t value, int32_t state, float_t duration, float_t a7,
        float_t step, int32_t a9, float_t offset, motion_database* mot_db);
    bool set_motion_id(int32_t motion_id, float_t frame,
        float_t duration, bool a5, bool set_motion_reset_data,
        MotionBlendType blend_type, bone_database* bone_data, motion_database* mot_db);
    void set_motion_reset_data(int32_t motion_id, float_t frame);
    void set_motion_skin_param(int32_t motion_id, float_t frame);
    void set_mouth_mottbl_motion(int32_t type, int32_t mottbl_index,
        float_t value, int32_t state, float_t duration, float_t a7,
        float_t step, int32_t a9, float_t offset, motion_database* mot_db);
    void set_osage_move_cancel(uint8_t id, float_t value);
    void set_osage_reset();
    void set_osage_step(float_t value);
    void set_parts_disp(item_id id, bool disp);
    void set_visibility(bool value);
};

struct skeleton_rotation_offset {
    bool x;
    bool y;
    bool z;
    vec3 rotation;
};

#define ROB_CHARA_COUNT 6

extern rob_chara* rob_chara_array;
extern rob_chara_pv_data* rob_chara_pv_data_array;

extern const chara_init_data* chara_init_data_get(chara_index chara_index);
extern int32_t chara_init_data_get_chara_size_index(chara_index chara_index);

extern float_t chara_pos_adjust_y_table_get_value(uint32_t a1);
extern float_t chara_size_table_get_value(uint32_t a1);

extern void motion_set_load_mothead(uint32_t set, std::string* mdata_dir, motion_database* mot_db);
extern void motion_set_load_motion(uint32_t set, std::string* mdata_dir, motion_database* mot_db);
extern void motion_set_unload_mothead(uint32_t set);
extern void motion_set_unload_motion(uint32_t set);

extern void pv_osage_manager_array_reset(int32_t chara_id);

extern void rob_init();
extern void rob_free();

extern mat4* rob_chara_bone_data_get_mats_mat(rob_chara_bone_data* rob_bone_data, size_t index);

extern rob_chara* rob_chara_array_get(int32_t chara_id);
extern int32_t rob_chara_array_init_chara_index(chara_index chara_index,
    rob_chara_pv_data* pv_data, int32_t module_index, bool can_set_default);
extern void rob_chara_array_free_chara_id(int32_t chara_id);

extern bool rob_chara_pv_data_array_check_chara_id(int32_t chara_id);

extern bool pv_osage_manager_array_ptr_get_disp();

extern void task_rob_manager_append_task();
extern bool task_rob_manager_check_chara_loaded(int32_t chara_id);

extern void motion_storage_init();
extern bool motion_storage_check_mot_file_not_ready(uint32_t set_id);
extern const mot_data* motion_storage_get_mot_data(uint32_t motion_id, motion_database* mot_db);
extern float_t motion_storage_get_mot_data_frame_count(uint32_t motion_id, motion_database* mot_db);
extern const mot_set* motion_storage_get_motion_set(uint32_t set_id);
extern void motion_storage_free();

extern void mothead_storage_init();
extern bool mothead_storage_check_mhd_file_not_ready(uint32_t set_id);
extern const mothead_mot* mothead_storage_get_mot_by_motion_id(uint32_t motion_id, motion_database* mot_db);
extern void mothead_storage_free();

extern void osage_setting_data_init();
extern bool osage_setting_data_load_file(void* data, const char* path, const char* file, uint32_t hash);
extern void osage_setting_data_free();
