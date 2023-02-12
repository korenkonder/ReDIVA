/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../KKdLib/mat.hpp"
#include "../KKdLib/vec.hpp"
#include "string.hpp"
#include "vector.hpp"
#include "shader_table.hpp"
#include <glad/glad.h>

enum bone_database_bone_type {
    BONE_DATABASE_BONE_ROTATION          = 0x00,
    BONE_DATABASE_BONE_TYPE_1            = 0x01,
    BONE_DATABASE_BONE_POSITION          = 0x02,
    BONE_DATABASE_BONE_POSITION_ROTATION = 0x03,
    BONE_DATABASE_BONE_HEAD_IK_ROTATION  = 0x04,
    BONE_DATABASE_BONE_ARM_IK_ROTATION   = 0x05,
    BONE_DATABASE_BONE_LEGS_IK_ROTATION  = 0x06,
    BONE_DATABASE_BONE_END               = 0xFF,
};

enum bone_database_skeleton_type {
    BONE_DATABASE_SKELETON_COMMON = 0,
    BONE_DATABASE_SKELETON_MIKU   = 1,
    BONE_DATABASE_SKELETON_KAITO  = 2,
    BONE_DATABASE_SKELETON_LEN    = 3,
    BONE_DATABASE_SKELETON_LUKA   = 4,
    BONE_DATABASE_SKELETON_MEIKO  = 5,
    BONE_DATABASE_SKELETON_RIN    = 6,
    BONE_DATABASE_SKELETON_HAKU   = 7,
    BONE_DATABASE_SKELETON_NERU   = 8,
    BONE_DATABASE_SKELETON_SAKINE = 9,
    BONE_DATABASE_SKELETON_TETO   = 10,
    BONE_DATABASE_SKELETON_NONE   = -1,
};

enum chara_index {
    CHARA_MIKU   =  0,
    CHARA_RIN    =  1,
    CHARA_LEN    =  2,
    CHARA_LUKA   =  3,
    CHARA_NERU   =  4,
    CHARA_HAKU   =  5,
    CHARA_KAITO  =  6,
    CHARA_MEIKO  =  7,
    CHARA_SAKINE =  8,
    CHARA_TETO   =  9,
    CHARA_MAX    = 10,
};

enum draw_task_flags {
    DRAW_TASK_SHADOW                = 0x00000001,
    DRAW_TASK_2                     = 0x00000002,
    DRAW_TASK_4                     = 0x00000004,
    DRAW_TASK_SELF_SHADOW           = 0x00000008,
    DRAW_TASK_10                    = 0x00000010,
    DRAW_TASK_20                    = 0x00000020,
    DRAW_TASK_40                    = 0x00000040,
    DRAW_TASK_SHADOW_OBJECT         = 0x00000080,
    DRAW_TASK_CHARA_REFLECT         = 0x00000100,
    DRAW_TASK_REFLECT               = 0x00000200,
    DRAW_TASK_REFRACT               = 0x00000400,
    DRAW_TASK_800                   = 0x00000800,
    DRAW_TASK_TRANSLUCENT_NO_SHADOW = 0x00001000,
    DRAW_TASK_SSS                   = 0x00002000,
    DRAW_TASK_4000                  = 0x00004000,
    DRAW_TASK_8000                  = 0x00008000,
    DRAW_TASK_ALPHA_ORDER_1         = 0x00010000,
    DRAW_TASK_ALPHA_ORDER_2         = 0x00020000,
    DRAW_TASK_ALPHA_ORDER_3         = 0x00040000,
    DRAW_TASK_80000                 = 0x00080000,
    DRAW_TASK_100000                = 0x00100000,
    DRAW_TASK_200000                = 0x00200000,
    DRAW_TASK_400000                = 0x00400000,
    DRAW_TASK_800000                = 0x00800000,
    DRAW_TASK_PREPROCESS            = 0x01000000,
    DRAW_TASK_2000000               = 0x02000000,
    DRAW_TASK_4000000               = 0x04000000,
    DRAW_TASK_8000000               = 0x08000000,
    DRAW_TASK_10000000              = 0x10000000,
    DRAW_TASK_20000000              = 0x20000000,
    DRAW_TASK_40000000              = 0x40000000,
    DRAW_TASK_NO_TRANSLUCENCY       = 0x80000000,
};

enum ex_expression_block_stack_type {
    EX_EXPRESSION_BLOCK_STACK_NUMBER          = 0x00,
    EX_EXPRESSION_BLOCK_STACK_VARIABLE        = 0x01,
    EX_EXPRESSION_BLOCK_STACK_VARIABLE_RADIAN = 0x02,
    EX_EXPRESSION_BLOCK_STACK_OP1             = 0x03,
    EX_EXPRESSION_BLOCK_STACK_OP2             = 0x04,
    EX_EXPRESSION_BLOCK_STACK_OP3             = 0x05,
};

enum ExNodeType {
    EX_NONE       = 0x00,
    EX_OSAGE      = 0x01,
    EX_EXPRESSION = 0x02,
    EX_CONSTRAINT = 0x03,
    EX_CLOTH      = 0x04,
};

enum eyes_base_adjust_type {
    EYES_BASE_ADJUST_NONE      = -1,
    EYES_BASE_ADJUST_DIRECTION = 0x00,
    EYES_BASE_ADJUST_CLEARANCE = 0x01,
    EYES_BASE_ADJUST_OFF       = 0x02,
    EYES_BASE_ADJUST_MAX       = 0x03,
};

enum item_id {
    ITEM_NONE        = -1,
    ITEM_BODY        = 0x00,
    ITEM_ATAMA       = 0x01,
    ITEM_KATA_R      = 0x02,
    ITEM_MUNE        = 0x03,
    ITEM_KATA_L      = 0x04,
    ITEM_UDE_R       = 0x05,
    ITEM_SENAKA      = 0x06,
    ITEM_UDE_L       = 0x07,
    ITEM_HARA        = 0x08,
    ITEM_KOSI        = 0x09,
    ITEM_TE_R        = 0x0A,
    ITEM_TE_L        = 0x0B,
    ITEM_MOMO        = 0x0C,
    ITEM_SUNE        = 0x0D,
    ITEM_ASI         = 0x0E,
    ITEM_KAMI        = 0x0F,
    ITEM_OUTER       = 0x10,
    ITEM_PANTS       = 0x11,
    ITEM_ZUJO        = 0x12,
    ITEM_MEGANE      = 0x13,
    ITEM_KUBI        = 0x14,
    ITEM_JOHA_USHIRO = 0x15,
    ITEM_KUCHI       = 0x16,
    ITEM_ITEM09      = 0x17,
    ITEM_ITEM10      = 0x18,
    ITEM_ITEM11      = 0x19,
    ITEM_ITEM12      = 0x1A,
    ITEM_ITEM13      = 0x1B,
    ITEM_ITEM14      = 0x1C,
    ITEM_ITEM15      = 0x1D,
    ITEM_ITEM16      = 0x1E,
    ITEM_MAX         = 0x1F,
};

enum item_sub_id {
    ITEM_SUB_NONE        = -1,
    ITEM_SUB_ZUJO        = 0x00,
    ITEM_SUB_KAMI        = 0x01,
    ITEM_SUB_HITAI       = 0x02,
    ITEM_SUB_ME          = 0x03,
    ITEM_SUB_MEGANE      = 0x04,
    ITEM_SUB_MIMI        = 0x05,
    ITEM_SUB_KUCHI       = 0x06,
    ITEM_SUB_MAKI        = 0x07,
    ITEM_SUB_KUBI        = 0x08,
    ITEM_SUB_INNER       = 0x09,
    ITEM_SUB_OUTER       = 0x0A,
    ITEM_SUB_JOHA_MAE    = 0x0B,
    ITEM_SUB_JOHA_USHIRO = 0x0C,
    ITEM_SUB_HADA        = 0x0D,
    ITEM_SUB_KATA        = 0x0E,
    ITEM_SUB_U_UDE       = 0x0F,
    ITEM_SUB_L_UDE       = 0x10,
    ITEM_SUB_TE          = 0x11,
    ITEM_SUB_BELT        = 0x12,
    ITEM_SUB_COSI        = 0x13,
    ITEM_SUB_PANTS       = 0x14,
    ITEM_SUB_ASI         = 0x15,
    ITEM_SUB_SUNE        = 0x16,
    ITEM_SUB_KUTSU       = 0x17,
    ITEM_SUB_HEAD        = 0x18,
    ITEM_SUB_MAX         = 0x19,
};

enum mot_key_set_type {
    MOT_KEY_SET_NONE            = 0x00,
    MOT_KEY_SET_STATIC          = 0x01,
    MOT_KEY_SET_HERMITE         = 0x02,
    MOT_KEY_SET_HERMITE_TANGENT = 0x03,
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
    MOTHEAD_DATA_ROB_HAND_ADJUST             = 0x49,
    MOTHEAD_DATA_TYPE_74                     = 0x4A,
    MOTHEAD_DATA_ROB_ADJUST_GLOBAL           = 0x4B,
    MOTHEAD_DATA_ROB_ARM_ADJUST              = 0x4C,
    MOTHEAD_DATA_DISABLE_EYE_MOTION          = 0x4D,
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

enum obj_index_format : uint32_t {
    OBJ_INDEX_U8  = 0x00,
    OBJ_INDEX_U16 = 0x01,
    OBJ_INDEX_U32 = 0x02,
};

enum obj_material_aniso_direction : uint32_t {
    OBJ_MATERIAL_ANISO_DIRECTION_NORMAL = 0,
    OBJ_MATERIAL_ANISO_DIRECTION_U      = 1,
    OBJ_MATERIAL_ANISO_DIRECTION_V      = 2,
    OBJ_MATERIAL_ANISO_DIRECTION_RADIAL = 3,
};

enum obj_material_blend_factor : uint32_t {
    OBJ_MATERIAL_BLEND_ZERO              = 0,
    OBJ_MATERIAL_BLEND_ONE               = 1,
    OBJ_MATERIAL_BLEND_SRC_COLOR         = 2,
    OBJ_MATERIAL_BLEND_INVERSE_SRC_COLOR = 3,
    OBJ_MATERIAL_BLEND_SRC_ALPHA         = 4,
    OBJ_MATERIAL_BLEND_INVERSE_SRC_ALPHA = 5,
    OBJ_MATERIAL_BLEND_DST_ALPHA         = 6,
    OBJ_MATERIAL_BLEND_INVERSE_DST_ALPHA = 7,
    OBJ_MATERIAL_BLEND_DST_COLOR         = 8,
    OBJ_MATERIAL_BLEND_INVERSE_DST_COLOR = 9,
    OBJ_MATERIAL_BLEND_ALPHA_SATURATE = 10,
};

enum obj_material_bump_map_type : uint32_t {
    OBJ_MATERIAL_BUMP_MAP_NONE = 0,
    OBJ_MATERIAL_BUMP_MAP_DOT  = 1,
    OBJ_MATERIAL_BUMP_MAP_ENV  = 2,
};

enum obj_material_color_source_type : uint32_t {
    OBJ_MATERIAL_COLOR_SOURCE_MATERIAL_COLOR = 0,
    OBJ_MATERIAL_COLOR_SOURCE_VERTEX_COLOR   = 1,
    OBJ_MATERIAL_COLOR_SOURCE_VERTEX_MORPH   = 2,
};

enum obj_material_shader_lighting_type : uint32_t {
    OBJ_MATERIAL_SHADER_LIGHTING_LAMBERT  = 0x00,
    OBJ_MATERIAL_SHADER_LIGHTING_CONSTANT = 0x01,
    OBJ_MATERIAL_SHADER_LIGHTING_PHONG    = 0x02,
};

enum obj_material_specular_quality : uint32_t {
    OBJ_MATERIAL_SPECULAR_QUALITY_LOW  = 0,
    OBJ_MATERIAL_SPECULAR_QUALITY_HIGH = 1,
};

enum obj_material_texture_type : uint32_t {
    OBJ_MATERIAL_TEXTURE_NONE               = 0x00,
    OBJ_MATERIAL_TEXTURE_COLOR              = 0x01,
    OBJ_MATERIAL_TEXTURE_NORMAL             = 0x02,
    OBJ_MATERIAL_TEXTURE_SPECULAR           = 0x03,
    OBJ_MATERIAL_TEXTURE_HEIGHT             = 0x04,
    OBJ_MATERIAL_TEXTURE_REFLECTION         = 0x05,
    OBJ_MATERIAL_TEXTURE_TRANSLUCENCY       = 0x06,
    OBJ_MATERIAL_TEXTURE_TRANSPARENCY       = 0x07,
    OBJ_MATERIAL_TEXTURE_ENVIRONMENT_SPHERE = 0x08,
    OBJ_MATERIAL_TEXTURE_ENVIRONMENT_CUBE   = 0x09,
};

enum obj_material_texture_coordinate_translation_type : uint32_t {
    OBJ_MATERIAL_TEXTURE_COORDINATE_TRANSLATION_NONE   = 0x00,
    OBJ_MATERIAL_TEXTURE_COORDINATE_TRANSLATION_UV     = 0x01,
    OBJ_MATERIAL_TEXTURE_COORDINATE_TRANSLATION_SPHERE = 0x02,
    OBJ_MATERIAL_TEXTURE_COORDINATE_TRANSLATION_CUBE   = 0x03
};

enum obj_material_vertex_translation_type : uint32_t {
    OBJ_MATERIAL_VERTEX_TRANSLATION_DEFAULT  = 0,
    OBJ_MATERIAL_VERTEX_TRANSLATION_ENVELOPE = 1,
    OBJ_MATERIAL_VERTEX_TRANSLATION_MORPHING = 2,
};

enum obj_primitive_type : uint32_t {
    OBJ_PRIMITIVE_POINTS         = 0x00,
    OBJ_PRIMITIVE_LINES          = 0x01,
    OBJ_PRIMITIVE_LINE_STRIP     = 0x02,
    OBJ_PRIMITIVE_LINE_LOOP      = 0x03,
    OBJ_PRIMITIVE_TRIANGLES      = 0x04,
    OBJ_PRIMITIVE_TRIANGLE_STRIP = 0x05,
    OBJ_PRIMITIVE_TRIANGLE_FAN   = 0x06,
    OBJ_PRIMITIVE_QUADS          = 0x07,
    OBJ_PRIMITIVE_QUAD_STRIP     = 0x08,
    OBJ_PRIMITIVE_POLYGON        = 0x09,
};

enum obj_skin_block_constraint_type : uint32_t {
    OBJ_SKIN_BLOCK_CONSTRAINT_NONE = 0,
    OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION,
    OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION,
    OBJ_SKIN_BLOCK_CONSTRAINT_POSITION,
    OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE,
};

enum obj_skin_block_constraint_coupling : uint32_t {
    OBJ_SKIN_BLOCK_CONSTRAINT_COUPLING_RIGID = 0x01,
    OBJ_SKIN_BLOCK_CONSTRAINT_COUPLING_SOFT  = 0x03,
};

enum obj_skin_block_type : uint32_t {
    OBJ_SKIN_BLOCK_NONE = 0,
    OBJ_SKIN_BLOCK_CLOTH,
    OBJ_SKIN_BLOCK_CONSTRAINT,
    OBJ_SKIN_BLOCK_EXPRESSION,
    OBJ_SKIN_BLOCK_ITEM,
    OBJ_SKIN_BLOCK_MOTION,
    OBJ_SKIN_BLOCK_OSAGE,
};

enum obj_skin_skin_param_coli_type {
    OBJ_SKIN_SKIN_PARAM_COLI_TYPE_NONE     = 0x00,
    OBJ_SKIN_SKIN_PARAM_COLI_TYPE_BALL     = 0x01,
    OBJ_SKIN_SKIN_PARAM_COLI_TYPE_CYLINDER = 0x02,
    OBJ_SKIN_SKIN_PARAM_COLI_TYPE_PLANE    = 0x03,
    OBJ_SKIN_SKIN_PARAM_COLI_TYPE_ELLIPSE  = 0x04,
};

enum obj_vertex_format : uint32_t {
    OBJ_VERTEX_NONE        = 0x0000,
    OBJ_VERTEX_POSITION    = 0x0001,
    OBJ_VERTEX_NORMAL      = 0x0002,
    OBJ_VERTEX_TANGENT     = 0x0004,
    OBJ_VERTEX_BINORMAL    = 0x0008,
    OBJ_VERTEX_TEXCOORD0   = 0x0010,
    OBJ_VERTEX_TEXCOORD1   = 0x0020,
    OBJ_VERTEX_TEXCOORD2   = 0x0040,
    OBJ_VERTEX_TEXCOORD3   = 0x0080,
    OBJ_VERTEX_COLOR0      = 0x0100,
    OBJ_VERTEX_COLOR1      = 0x0200,
    OBJ_VERTEX_BONE_WEIGHT = 0x0400,
    OBJ_VERTEX_BONE_INDEX  = 0x0800,
    OBJ_VERTEX_UNK_1000    = 0x1000,
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

enum shadow_type_enum {
    SHADOW_CHARA = 0x00,
    SHADOW_STAGE = 0x01,
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

enum texture_flags {
    TEXTURE_BLOCK_COMPRESSION = 0x1,
};

#define list(t) \
struct list_##t##_node; \
\
struct list_##t##_node {  \
    list_##t##_node* next; \
    list_##t##_node* prev; \
    t value; \
}; \
 \
struct list_##t { \
    list_##t##_node* head; \
    size_t size; \
};

#define list_ptr(t) \
struct list_ptr_##t##_node; \
\
struct list_ptr_##t##_node {  \
    list_ptr_##t##_node* next; \
    list_ptr_##t##_node* prev; \
    t* value; \
}; \
 \
struct list_ptr_##t { \
    list_ptr_##t##_node* head; \
    size_t size; \
};

#define tree_def(t) \
struct tree_##t##_node; \
\
struct tree_##t##_node { \
    tree_##t##_node* left; \
    tree_##t##_node* parent; \
    tree_##t##_node* right; \
    uint8_t color; \
    uint8_t is_null; \
    t value; \
}; \
 \
struct tree_##t { \
    tree_##t##_node* head; \
    size_t size; \
};

#define tree_ptr(t) \
struct tree_ptr_##t##_node; \
\
struct tree_ptr_##t##_node { \
    tree_ptr_##t##_node* left; \
    tree_ptr_##t##_node* parent; \
    tree_ptr_##t##_node* right; \
    uint8_t color; \
    uint8_t is_null; \
    t* value; \
}; \
 \
struct tree_ptr_##t { \
    tree_ptr_##t##_node* head; \
    size_t size; \
};

#define tree_pair(t1, t2) \
struct tree_pair_##t1##_##t2##_node; \
\
struct tree_pair_##t1##_##t2##_node { \
    tree_pair_##t1##_##t2##_node* left; \
    tree_pair_##t1##_##t2##_node* parent; \
    tree_pair_##t1##_##t2##_node* right; \
    uint8_t color; \
    uint8_t is_null; \
    t1 key; \
    t2 value; \
}; \
 \
struct tree_pair_##t1##_##t2 { \
    tree_pair_##t1##_##t2##_node* head; \
    size_t size; \
};

#define tree_pair_ptr(t1, t2) \
struct tree_pair_##t1##_ptr_##t2##_node; \
\
struct tree_pair_##t1##_ptr_##t2##_node { \
    tree_pair_##t1##_ptr_##t2##_node* left; \
    tree_pair_##t1##_ptr_##t2##_node* parent; \
    tree_pair_##t1##_ptr_##t2##_node* right; \
    uint8_t color; \
    uint8_t is_null; \
    t1 key; \
    t2* value; \
}; \
 \
struct tree_pair_##t1##_ptr_##t2 { \
    tree_pair_##t1##_ptr_##t2##_node* head; \
    size_t size; \
};

struct bone_database_bone {
    uint8_t type;
    bool has_parent;
    uint8_t parent;
    uint8_t pole_target;
    uint8_t mirror;
    uint8_t flags;
    const char* name;
};

struct bone_database_bone_file {
    uint8_t type;
    bool has_parent;
    uint8_t parent;
    uint8_t pole_target;
    uint8_t mirror;
    uint8_t flags;
    uint32_t name_offset;
};

struct bone_database_skeleton {
    bone_database_bone* bones;
    int32_t posittion_count;
    int32_t field_C;
    vec3* positions;
    float_t* heel_height;
    uint32_t object_bone_name_count;
    int32_t field_24;
    const char** object_bone_names;
    int32_t motion_bone_name_count;
    int32_t field_34;
    const char** motion_bone_names;
    uint16_t* parent_indices;
    int32_t field_48;
    int32_t field_4C;
    int32_t field_50;
    int32_t field_54;
    int32_t field_58;
    int32_t field_5C;
};

struct bone_database_struct {
    uint32_t signature;
    uint32_t skeleton_count;
    bone_database_skeleton** skeletons;
    const char** skeleton_names;
};

struct rob_chara;
struct rob_chara_bone_data;
struct rob_chara_item_equip;
struct bone_node;

struct bone_node_expression_data {
    vec3 position;
    vec3 rotation;
    vec3 scale;
    vec3 parent_scale;
};

struct bone_node {
    const char* name;
    mat4* mat;
    bone_node* parent;
    bone_node_expression_data exp_data;
    mat4* ex_data_mat;
};

vector_old(bone_node)

struct struc_314 {
    uint32_t* field_0;
    size_t field_8;
};

struct mot_key_set {
    mot_key_set_type type;
    int32_t keys_count;
    int32_t current_key;
    int32_t last_key;
    uint16_t* frames;
    float_t* values;
};

vector_old(mot_key_set)

union item_cos_data {
    struct {
        int32_t zujo;
        int32_t kami;
        int32_t hitai;
        int32_t me;
        int32_t megane;
        int32_t mimi;
        int32_t kuchi;
        int32_t maki;
        int32_t kubi;
        int32_t inner;
        int32_t outer;
        int32_t joha_mae;
        int32_t joha_ushiro;
        int32_t hada;
        int32_t kata;
        int32_t u_ude;
        int32_t l_ude;
        int32_t te;
        int32_t belt;
        int32_t cosi;
        int32_t pants;
        int32_t asi;
        int32_t sune;
        int32_t kutsu;
        int32_t head;
    } data;
    int32_t arr[25];
};

struct texture;

struct item_cos_texture_change_tex {
    texture* org;
    texture* chg;
    bool changed;
};

vector_old(item_cos_texture_change_tex)

struct item_cos_texture_change {
    uint32_t item_no;
    vector_old_item_cos_texture_change_tex tex;
};

struct item_cos_item_change {
    item_id id;
    vector_old_uint32_t item_nos;
};

struct object_info {
    uint16_t id;
    uint16_t set_id;
};

struct texture_pattern_struct {
    int32_t src;
    int32_t dst;
};

tree_def(item_cos_texture_change)
tree_def(item_cos_item_change)
tree_pair(object_info, item_id)
tree_pair(int32_t, int32_t)
vector_old(texture_pattern_struct)
tree_pair(int32_t, object_info)

list_ptr(void)

struct rob_chara_item_cos_data {
    ::chara_index chara_index;
    ::chara_index chara_index_2nd;
    item_cos_data field_8;
    item_cos_data field_6C;
    tree_item_cos_texture_change texture_change;
    tree_item_cos_item_change item_change;
    tree_pair_object_info_item_id field_F0;
    tree_pair_int32_t_int32_t field_100;
    vector_old_texture_pattern_struct texture_pattern[31];
    tree_pair_int32_t_object_info head_replace;
};

struct struc_525 {
    int32_t field_0;
    int32_t field_4;
};

struct struc_524 {
    int32_t field_0;
    struc_525 field_4;
    int32_t motion_id;
    uint8_t field_10;
};

struct struc_523 {
    uint8_t field_0;
    uint8_t field_1;
    int32_t field_4;
    int16_t field_8;
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
    uint8_t field_50;
    int32_t field_54;
    list_ptr_void field_58;
    int64_t field_68;
    int64_t field_70;
    int32_t field_78;
    float_t field_7C;
    float_t field_80;
};

struct struc_526 {
    int32_t field_0;
    int32_t field_4;
};

struct struc_264 {
    uint8_t field_0;
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
    uint8_t field_98;
    int32_t field_9C;
    int32_t field_A0;
    uint8_t field_A4;
    uint8_t field_A5;
    int32_t field_A8;
    int32_t field_AC;
    int32_t field_B0;
    uint8_t field_B4;
    struc_523 field_B8;
    int32_t field_140;
    uint8_t field_144;
    uint8_t field_145;
    uint8_t field_146;
    uint8_t field_147;
    int32_t field_148;
    int32_t field_14C;
    uint8_t field_150;
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
    uint8_t field_180;
    int32_t field_184;
    int32_t field_188;
    float_t field_18C;
    uint8_t field_190;
    int32_t field_194;
    int32_t field_198;
    int32_t field_19C;
    int32_t field_1A0;
    uint8_t field_1A4;
    uint8_t** field_1A8;
    int32_t field_1B0;
    int32_t field_1B4;
    int32_t field_1B8;
    int32_t field_1BC;
    struc_526 field_1C0;
    uint8_t field_1C8;
    uint8_t field_1C9;
    uint8_t field_1CA;
    int32_t field_1CC;
    int32_t field_1D0;
    int32_t field_1D4;
};

struct SubActExec;
struct SubActParam;

struct SubActExec_vtbl {
    SubActExec* (* Dispose)(SubActExec*, bool);
    void(* Field_8)(SubActExec*);
    void(* Field_10)(SubActExec*, SubActParam*);
    void(* Field_18)(SubActExec*, rob_chara*);
    void(* Field_20)(SubActExec*, rob_chara*);
};

struct SubActExec {
    SubActExec_vtbl* __vftable;
    SubActExecType type;
    int32_t field_C;
};

struct SubActExecCry {
    SubActExec base;
};

struct SubActExecShakeHand {
    SubActExec base;
    int64_t field_10;
    uint8_t field_18;
};

struct SubActExecEmbarrassed {
    SubActExec base;
    uint8_t field_10;
};

struct SubActExecAngry {
    SubActExec base;
};

struct SubActExecLaugh {
    SubActExec base;
};

struct SubActExecCountNum {
    SubActExec base;
    int64_t field_10;
    int32_t field_18;
};

struct SubActParam_vtbl {
    void* (*Dispose)(void*, bool);
};

struct SubActParam {
    SubActParam_vtbl* __vftable;
    SubActParamType type;
};

struct RobSubAction;

struct RobSubAction_vtbl {
    void(*dispose)(RobSubAction*, bool);
};

struct RobSubAction {
    RobSubAction_vtbl* __vftable;

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
    } data;
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

struct RobPartialMotion;

struct RobPartialMotion_vtbl {
    RobPartialMotion* (*dispose)(RobPartialMotion*, bool);
    void* (*Reset)(RobPartialMotion*);
};

struct RobPartialMotion {
    RobPartialMotion_vtbl* __vftable;

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
    } data;
};

struct RobFaceMotion {
    RobPartialMotion base;
};

struct RobHandMotion {
    RobPartialMotion base;
};

struct RobMouthMotion {
    RobPartialMotion base;
};

struct RobEyesMotion {
    RobPartialMotion base;
};

struct RobEyelidMotion {
    RobPartialMotion base;
};

struct rob_chara_data_adjust {
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
    vec3 external_force;
    vec3 external_force_cycle_strength;
    vec3 external_force_cycle;
    float_t cycle;
    float_t phase;
    float_t force;
    float_t strength;
    float_t strength_transition;
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
    vec3 field_30;
    float_t arm_length;
    int32_t field_40;
};

struct rob_chara_data_arm_adjust {
    bool enable;
    float_t value;
    float_t prev_value;
    float_t next_value;
    float_t duration;
    float_t frame;
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
};

struct rob_chara_motion {
    int32_t motion_id;
    int32_t prev_motion_id;
    struc_389 frame_data;
    struc_406 step_data;
    float_t step;
    int32_t field_24;
    uint8_t field_28;
    uint8_t field_29;
    uint8_t field_2A;
    int32_t field_2C;
    int32_t field_30;
    int32_t field_34;
    int32_t field_38[32];
    int32_t field_B8[32];
    float_t field_138;
    float_t field_13C;
    int32_t field_140;
    int32_t field_144;
    int32_t field_148;
    int32_t field_14C;
    struc_405 field_150;
    RobHandMotion hand_l;
    RobHandMotion hand_r;
    object_info hand_l_object;
    object_info hand_r_object;
    struc_405 field_3B0;
    rob_chara_data_adjust parts_adjust[13];
    rob_chara_data_adjust parts_adjust_prev[13];
    rob_chara_data_adjust adjust_global;
    rob_chara_data_adjust adjust_global_prev;
    rob_chara_data_hand_adjust hand_adjust[2];
    rob_chara_data_hand_adjust hand_adjust_prev[2];
    rob_chara_data_arm_adjust arm_adjust[2];
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

struct struc_652 {
    int32_t motion_id;
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
    list_ptr_void field_228;
    int16_t field_238;
    float_t field_23C;
    int32_t field_240;
    int16_t field_244;
    const mothead_data2* field_248;
    int64_t field_250;
    float_t field_258;
    int32_t field_25C;
    struc_526 field_260;
    struc_526 field_268;
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
    struc_526 field_2F4;
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
};

struct struc_377 {
    mothead_data* field_0;
    mothead_data* field_8;
};

struct struc_226 {
    uint8_t field_0[27];
};

struct struc_225 {
    float_t field_0[27];
};

struct struc_224 {
    int32_t field_0[27];
};

struct struc_306 {
    int16_t field_0;
    float_t field_4;
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
    int64_t *field_7A0;
    int32_t motion_set_id;
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
    vec3 field_60;
    mat4 field_6C;
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
    mat4 mat;
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

struct struc_215 {
    int64_t field_0;
    float_t field_8;
};

vector_old(struc_215)

struct struc_267 {
    float_t field_0;
    int16_t field_4;
    int16_t field_6;
    float_t field_8;
    int16_t field_C;
    int16_t field_E;
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
    struc_210 field_1AA0[27];
    float_t field_1BE4[27];
    vector_old_struc_215 field_1C50;
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
    int32_t field_1DE0;
    int32_t field_1DE4;
    int32_t field_1DE8;
    int32_t field_1DEC;
    int32_t field_1DF0;
    int32_t field_1DF4;
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
    uint8_t field_1F04;
    int32_t field_1F08;
    int32_t field_1F0C;
    int32_t field_1F10;
    int32_t field_1F14;
    int32_t field_1F18;
    int32_t field_1F1C;
    int32_t field_1F20;
    uint8_t field_1F24;
    uint8_t field_1F25;
    uint8_t field_1F26;
};

struct rob_chara_data {
    uint8_t field_0;
    uint8_t field_1;
    uint8_t field_2;
    uint8_t field_3;
    int32_t field_4;
    struc_264 field_8;
    RobSubAction rob_sub_action;
    rob_chara_motion motion;
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
};

struct struc_242 {
    float_t field_0;
    float_t field_4;
    float_t field_8;
    float_t field_C;
    float_t field_10;
    float_t field_14;
    float_t field_18;
    vec3 field_1C;
    float_t field_28;
};

struct eyes_adjust {
    bool xrot_adjust;
    eyes_base_adjust_type base_adjust;
    float_t neg;
    float_t pos;
};

struct rob_chara_pv_data_customize_items {
    int32_t head;
    int32_t face;
    int32_t chest;
    int32_t back;
};

struct rob_chara_pv_data {
    rob_chara_type type;
    int8_t field_4;
    int8_t field_5;
    int8_t field_6;
    vec3 field_8;
    int16_t rot_y_int16;
    int16_t field_16;
    struc_242 field_18;
    struc_242 field_44;
    int32_t field_70;
    int32_t motion_face_ids[10];
    int32_t chara_size_index;
    int8_t height_adjust;
    union {
        rob_chara_pv_data_customize_items data;
        int32_t arr[4];
    } customize_items;
    eyes_adjust eyes_adjust;
};

struct touch_area;

struct touch_area_vftable {
    touch_area(*dispose)(touch_area*, bool);
    void(*field_8)(touch_area*);
    void(*field_10)(touch_area*);
    void(*field_18)(touch_area*);
    void(*field_20)(touch_area*);
    void(*field_28)(touch_area*);
    void(*field_30)(touch_area*);
};

struct touch_area {
    touch_area_vftable* __vftable;
    int32_t field_8;
    int32_t field_C;
    float_t field_10;
    float_t field_14;
    float_t field_18;
    float_t field_1C;
    float_t field_20;
    int32_t field_24;
};

struct rob_touch_area {
    touch_area area;
    int64_t field_28;
    uint8_t field_30[3];
    int16_t field_34;
};

vector_old(rob_touch_area)

struct rob_touch;

struct rob_touch_vftable {
    rob_touch* (*dispose)(rob_touch*, bool);
};

struct rob_touch {
    rob_touch_vftable __vftable;
    uint8_t field_8;
    vector_old_rob_touch_area area;
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

struct struc_218 {
    vec3 bone_offset;
    float_t scale;
    rob_bone_index bone_index;
    int32_t ik;
};

struct struc_344 {
    int32_t chara_size_index;
    int32_t field_4;
    int32_t field_8;
    int32_t field_C;
};

struct chara_init_data {
    int32_t field_0;
    int32_t object_set;
    bone_database_skeleton_type skeleton_type;
    object_info field_C;
    object_info field_10;
    object_info field_14;
    object_info field_18;
    object_info field_1C;
    object_info field_20;
    object_info field_24;
    object_info field_28;
    object_info field_2C;
    object_info field_30;
    object_info field_34;
    object_info field_38;
    object_info field_3C;
    object_info field_40;
    object_info field_44;
    int32_t field_48;
    int32_t field_4C;
    int32_t field_50;
    int32_t field_54;
    int32_t field_58;
    int32_t field_5C;
    int32_t field_60;
    int32_t field_64;
    int32_t field_68;
    int32_t field_6C;
    int32_t field_70;
    int32_t field_74;
    int32_t field_78;
    int32_t field_7C;
    int32_t field_80;
    int32_t field_84;
    int32_t field_88;
    int32_t field_8C;
    int32_t field_90;
    int32_t field_94;
    int32_t field_98;
    int32_t field_9C;
    int32_t field_A0;
    int32_t field_A4;
    int32_t field_A8;
    int32_t field_AC;
    int32_t field_B0;
    int32_t field_B4;
    int32_t field_B8;
    int32_t field_BC;
    int32_t field_C0;
    int32_t field_C4;
    int32_t field_C8;
    int32_t field_CC;
    int32_t field_D0;
    int32_t field_D4;
    int32_t field_D8;
    int32_t field_DC;
    int32_t field_E0;
    int32_t field_E4;
    int32_t field_E8;
    int32_t field_EC;
    int32_t field_F0;
    int32_t field_F4;
    int32_t field_F8;
    int32_t field_FC;
    int32_t field_100;
    int32_t field_104;
    int32_t field_108;
    int32_t field_10C;
    int32_t field_110;
    int32_t field_114;
    int32_t field_118;
    int32_t field_11C;
    int32_t field_120;
    int32_t field_124;
    int32_t field_128;
    int32_t field_12C;
    int32_t field_130;
    int32_t field_134;
    int32_t field_138;
    int32_t field_13C;
    int32_t field_140;
    int32_t field_144;
    int32_t field_148;
    int32_t field_14C;
    int32_t field_150;
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
    int32_t field_180;
    int32_t field_184;
    int32_t field_188;
    int32_t field_18C;
    int32_t field_190;
    int32_t field_194;
    int32_t field_198;
    int32_t field_19C;
    int32_t field_1A0;
    int32_t field_1A4;
    int32_t field_1A8;
    int32_t field_1AC;
    int32_t field_1B0;
    int32_t field_1B4;
    int32_t field_1B8;
    int32_t field_1BC;
    int32_t field_1C0;
    int32_t field_1C4;
    int32_t field_1C8;
    int32_t field_1CC;
    int32_t field_1D0;
    int32_t field_1D4;
    int32_t field_1D8;
    int32_t field_1DC;
    int32_t field_1E0;
    int32_t field_1E4;
    int32_t field_1E8;
    int32_t field_1EC;
    int32_t field_1F0;
    int32_t field_1F4;
    int32_t field_1F8;
    int32_t field_1FC;
    int32_t field_200;
    int32_t field_204;
    int32_t field_208;
    int32_t field_20C;
    int32_t field_210;
    int32_t field_214;
    int32_t field_218;
    int32_t field_21C;
    int32_t field_220;
    int32_t field_224;
    int32_t field_228;
    int32_t field_22C;
    int32_t field_230;
    int32_t field_234;
    int32_t field_238;
    int32_t field_23C;
    int32_t field_240;
    int32_t field_244;
    int32_t field_248;
    int32_t field_24C;
    int32_t field_250;
    int32_t field_254;
    int32_t field_258;
    int32_t field_25C;
    int32_t field_260;
    int32_t field_264;
    int32_t field_268;
    int32_t field_26C;
    int32_t field_270;
    int32_t field_274;
    int32_t field_278;
    int32_t field_27C;
    int32_t field_280;
    int32_t field_284;
    int32_t field_288;
    int32_t field_28C;
    int32_t field_290;
    int32_t field_294;
    int32_t field_298;
    int32_t field_29C;
    int32_t field_2A0;
    int32_t field_2A4;
    int32_t field_2A8;
    int32_t field_2AC;
    int32_t field_2B0;
    int32_t field_2B4;
    int32_t field_2B8;
    int32_t field_2BC;
    int32_t field_2C0;
    int32_t field_2C4;
    int32_t field_2C8;
    int32_t field_2CC;
    int32_t field_2D0;
    int32_t field_2D4;
    int32_t field_2D8;
    int32_t field_2DC;
    int32_t field_2E0;
    int32_t field_2E4;
    int32_t field_2E8;
    int32_t field_2EC;
    int32_t field_2F0;
    int32_t field_2F4;
    int32_t field_2F8;
    int32_t field_2FC;
    int32_t field_300;
    int32_t field_304;
    int32_t field_308;
    int32_t field_30C;
    int32_t field_310;
    int32_t field_314;
    int32_t field_318;
    int32_t field_31C;
    int32_t field_320;
    int32_t field_324;
    int32_t field_328;
    int32_t field_32C;
    int32_t field_330;
    int32_t field_334;
    int32_t field_338;
    int32_t field_33C;
    int32_t field_340;
    int32_t field_344;
    int32_t field_348;
    int32_t field_34C;
    int32_t field_350;
    int32_t field_354;
    int32_t field_358;
    int32_t field_35C;
    int32_t field_360;
    int32_t field_364;
    int32_t field_368;
    int32_t field_36C;
    int32_t field_370;
    int32_t field_374;
    int32_t field_378;
    int32_t field_37C;
    int32_t field_380;
    int32_t field_384;
    int32_t field_388;
    int32_t field_38C;
    int32_t field_390;
    int32_t field_394;
    int32_t field_398;
    int32_t field_39C;
    int32_t field_3A0;
    int32_t field_3A4;
    int32_t field_3A8;
    int32_t field_3AC;
    int32_t field_3B0;
    int32_t field_3B4;
    int32_t field_3B8;
    int32_t field_3BC;
    int32_t field_3C0;
    int32_t field_3C4;
    int32_t field_3C8;
    int32_t field_3CC;
    int32_t field_3D0;
    int32_t field_3D4;
    int32_t field_3D8;
    int32_t field_3DC;
    int32_t field_3E0;
    int32_t field_3E4;
    int32_t field_3E8;
    int32_t field_3EC;
    int32_t field_3F0;
    int32_t field_3F4;
    int32_t field_3F8;
    int32_t field_3FC;
    int32_t field_400;
    int32_t field_404;
    int32_t field_408;
    int32_t field_40C;
    int32_t field_410;
    int32_t field_414;
    int32_t field_418;
    int32_t field_41C;
    int32_t field_420;
    int32_t field_424;
    int32_t field_428;
    int32_t field_42C;
    int32_t field_430;
    int32_t field_434;
    int32_t field_438;
    int32_t field_43C;
    int32_t field_440;
    int32_t field_444;
    int32_t field_448;
    int32_t field_44C;
    int32_t field_450;
    int32_t field_454;
    int32_t field_458;
    int32_t field_45C;
    int32_t field_460;
    int32_t field_464;
    int32_t field_468;
    int32_t field_46C;
    int32_t field_470;
    int32_t field_474;
    int32_t field_478;
    int32_t field_47C;
    int32_t field_480;
    int32_t field_484;
    int32_t field_488;
    int32_t field_48C;
    int32_t field_490;
    int32_t field_494;
    int32_t field_498;
    int32_t field_49C;
    int32_t field_4A0;
    int32_t field_4A4;
    int32_t field_4A8;
    int32_t field_4AC;
    int32_t field_4B0;
    int32_t field_4B4;
    int32_t field_4B8;
    int32_t field_4BC;
    int32_t field_4C0;
    int32_t field_4C4;
    int32_t field_4C8;
    int32_t field_4CC;
    int32_t field_4D0;
    int32_t field_4D4;
    int32_t field_4D8;
    int32_t field_4DC;
    int32_t field_4E0;
    int32_t field_4E4;
    int32_t field_4E8;
    int32_t field_4EC;
    int32_t field_4F0;
    int32_t field_4F4;
    int32_t field_4F8;
    int32_t field_4FC;
    int32_t field_500;
    int32_t field_504;
    int32_t field_508;
    int32_t field_50C;
    int32_t field_510;
    int32_t field_514;
    int32_t field_518;
    int32_t field_51C;
    int32_t field_520;
    int32_t field_524;
    int32_t field_528;
    int32_t field_52C;
    int32_t field_530;
    int32_t field_534;
    int32_t field_538;
    int32_t field_53C;
    int32_t field_540;
    int32_t field_544;
    int32_t field_548;
    int32_t field_54C;
    int32_t field_550;
    int32_t field_554;
    int32_t field_558;
    int32_t field_55C;
    int32_t field_560;
    int32_t field_564;
    int32_t field_568;
    int32_t field_56C;
    int32_t field_570;
    int32_t field_574;
    int32_t field_578;
    int32_t field_57C;
    int32_t field_580;
    int32_t field_584;
    int32_t field_588;
    int32_t field_58C;
    int32_t field_590;
    int32_t field_594;
    int32_t field_598;
    int32_t field_59C;
    int32_t field_5A0;
    int32_t field_5A4;
    int32_t field_5A8;
    int32_t field_5AC;
    int32_t field_5B0;
    int32_t field_5B4;
    int32_t field_5B8;
    int32_t field_5BC;
    int32_t field_5C0;
    int32_t field_5C4;
    int32_t field_5C8;
    int32_t field_5CC;
    int32_t field_5D0;
    int32_t field_5D4;
    int32_t field_5D8;
    int32_t field_5DC;
    int32_t field_5E0;
    int32_t field_5E4;
    int32_t field_5E8;
    int32_t field_5EC;
    int32_t field_5F0;
    int32_t field_5F4;
    int32_t field_5F8;
    int32_t field_5FC;
    int32_t field_600;
    int32_t field_604;
    int32_t field_608;
    int32_t field_60C;
    int32_t field_610;
    int32_t field_614;
    int32_t field_618;
    int32_t field_61C;
    int32_t field_620;
    int32_t field_624;
    int32_t field_628;
    int32_t field_62C;
    int32_t field_630;
    int32_t field_634;
    int32_t field_638;
    int32_t field_63C;
    int32_t field_640;
    int32_t field_644;
    int32_t field_648;
    int32_t field_64C;
    int32_t field_650;
    int32_t field_654;
    int32_t field_658;
    int32_t field_65C;
    int32_t field_660;
    int32_t field_664;
    int32_t field_668;
    int32_t field_66C;
    int32_t field_670;
    int32_t field_674;
    int32_t field_678;
    int32_t field_67C;
    int32_t field_680;
    int32_t field_684;
    int32_t field_688;
    int32_t field_68C;
    int32_t field_690;
    int32_t field_694;
    int32_t field_698;
    int32_t field_69C;
    int32_t field_6A0;
    int32_t field_6A4;
    int32_t field_6A8;
    int32_t field_6AC;
    int32_t field_6B0;
    int32_t field_6B4;
    int32_t field_6B8;
    int32_t field_6BC;
    int32_t field_6C0;
    int32_t field_6C4;
    int32_t field_6C8;
    int32_t field_6CC;
    int32_t field_6D0;
    int32_t field_6D4;
    int32_t field_6D8;
    int32_t field_6DC;
    int32_t field_6E0;
    int32_t field_6E4;
    int32_t field_6E8;
    int32_t field_6EC;
    int32_t field_6F0;
    int32_t field_6F4;
    int32_t field_6F8;
    int32_t field_6FC;
    int32_t field_700;
    int32_t field_704;
    int32_t field_708;
    int32_t field_70C;
    int32_t field_710;
    int32_t field_714;
    int32_t field_718;
    int32_t field_71C;
    int32_t field_720;
    int32_t field_724;
    int32_t field_728;
    int32_t field_72C;
    int32_t field_730;
    int32_t field_734;
    int32_t field_738;
    int32_t field_73C;
    int32_t field_740;
    int32_t field_744;
    int32_t field_748;
    int32_t field_74C;
    int32_t field_750;
    int32_t field_754;
    int32_t field_758;
    int32_t field_75C;
    int32_t field_760;
    int32_t field_764;
    int32_t field_768;
    int32_t field_76C;
    int32_t field_770;
    int32_t field_774;
    int32_t field_778;
    int32_t field_77C;
    int32_t field_780;
    int32_t field_784;
    int32_t field_788;
    int32_t field_78C;
    int32_t field_790;
    int32_t field_794;
    int32_t field_798;
    int32_t field_79C;
    int32_t field_7A0;
    int32_t field_7A4;
    int32_t field_7A8;
    int32_t field_7AC;
    int32_t field_7B0;
    int32_t field_7B4;
    int32_t field_7B8;
    int32_t field_7BC;
    int32_t field_7C0;
    int32_t field_7C4;
    int32_t field_7C8;
    int32_t field_7CC;
    int32_t field_7D0;
    int32_t field_7D4;
    int32_t field_7D8;
    int32_t field_7DC;
    object_info field_7E0;
    object_info field_7E4;
    object_info field_7E8;
    object_info field_7EC;
    object_info field_7F0;
    object_info field_7F4;
    object_info field_7F8;
    object_info field_7FC;
    object_info field_800;
    object_info field_804;
    object_info field_808;
    object_info field_80C;
    object_info field_810;
    object_info field_814;
    object_info field_818;
    uint32_t motion_set;
    int32_t field_820;
    int32_t field_824;
    struc_218* field_828;
    struc_218* field_830;
    uint8_t*** field_838;
    int32_t field_840;
    int32_t field_844;
    struc_344* field_848;
    object_info* head_objects;
    void* field_858;
    void* field_860;
    object_info* face_objects;
};

struct rob_detail;

struct rob_detail_vtbl {
    rob_detail* (*dispose)(rob_detail*, bool);
};

struct rob_detail {
    rob_detail_vtbl* __vftable;
    rob_chara* field_8;
    rob_chara_data* field_10;
};

struct rob_chara {
    uint8_t chara_id;
    uint8_t field_1;
    uint8_t field_2;
    uint8_t field_3;
    int32_t field_4;
    int16_t field_8;
    int16_t field_A;
    uint8_t field_C;
    uint8_t field_D;
    chara_index chara_index;
    int32_t module_index;
    int32_t field_18;
    float_t field_1C;
    struc_307* field_20;
    rob_chara_bone_data* bone_data;
    rob_chara_item_equip* item_equip;
    rob_chara_item_cos_data item_cos_data;
    rob_chara_data data;
    rob_chara_data data_prev;
    chara_init_data* chara_init_data;
    rob_detail* rob_detail;
    rob_chara_pv_data pv_data;
    int32_t field_80E4;
    rob_touch rob_touch;
};

struct struc_313 {
    vector_old_uint32_t bitfield;
    size_t motion_bone_count;
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

struct mot_key_set_file {
    uint16_t info;
    uint16_t frame_count;
};

struct mot_data {
    mot_key_set_file* data_file;
    uint16_t* key_set_types;
    void* key_sets;
    uint16_t* bone_info;
};

struct mot_key_data {
    bool key_sets_ready;
    size_t key_set_count;
    vector_old_mot_key_set key_set;
    mot mot;
    vector_old_float_t key_set_data;
    mot_data* mot_data;
    bone_database_skeleton_type skeleton_type;
    int32_t skeleton_select;
    int32_t motion_id;
    float_t frame;
    struc_369 field_68;
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
    mat4 rot_mat[3];
    vec3 trans_prev[2];
    mat4 rot_mat_prev[3][2];
    mat4* pole_target_mat;
    mat4* parent_mat;
    bone_node* node;
    float_t ik_segment_length[2];
    float_t ik_2nd_segment_length[2];
    float_t arm_length;
    float_t eyes_xrot_adjust_neg;
    float_t eyes_xrot_adjust_pos;
};

vector_old(bone_data)

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

struct MotionBlend;

struct /*VFT*/ MotionBlend_vtbl {
    void* (*Dispose)(MotionBlend*, bool);
    void (*Reset)(MotionBlend*);
    void (*Field10)(MotionBlend*, float_t a2, float_t a3, int32_t a4);
    void (*Step)(MotionBlend*, struc_400*);
    void (*Field20)(MotionBlend*, vector_old_bone_data*, vector_old_bone_data*);
    void (*Blend)(MotionBlend*, bone_data*, bone_data*);
    bool (*Field30)(MotionBlend*);
};

struct MotionBlend {
    MotionBlend_vtbl* __vftable;
    bool enable;
    bool rot_y;
    float_t duration;
    float_t frame;
    float_t step;
    float_t offset;
    float_t blend;
};

struct PartialMotionBlendFreeze {
    MotionBlend base;
};

struct MotionBlendCross {
    MotionBlend base;
    bool field_20;
    bool field_21;
    mat4 field_24;
    mat4 field_64;
    mat4 field_A4;
    mat4 field_E4;
};

struct MotionBlendFreeze {
    MotionBlend base;
    uint8_t field_20;
    uint8_t field_21;
    int32_t field_24;
    float_t field_28;
    float_t field_2C;
    int32_t field_30;
    mat4 field_34;
    mat4 field_74;
    mat4 rot_y;
    mat4 field_F4;
};

struct MotionBlendCombine {
    MotionBlendCross base;
};

struct struc_240 {
    bool(*bone_check_func)(motion_bone_index bone_index);
    struc_313 field_8;
    size_t motion_bone_count;
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

struct mot_blend {
    struc_240 field_0;
    uint8_t field_30;
    mot_key_data mot_key_data;
    mot_play_data mot_play_data;
    PartialMotionBlendFreeze blend;
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
    vector_old_bone_data* field_0;
    mat4 field_8;
    struc_241 field_48;
    uint8_t field_8C;
    uint8_t field_8D;
    uint8_t field_8E;
    uint8_t field_8F;
    uint8_t field_90;
    uint8_t field_91;
    float_t field_94;
    float_t field_98;
    float_t field_9C;
    float_t field_A0;
    float_t field_A4;
    int32_t field_A8;
    int32_t field_AC;
    float_t field_B0;
    vec3 field_B4;
    vec3 field_C0;
    mat4 field_CC;
    mat4 field_10C;
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
    uint8_t field_1C4;
    int32_t field_1C8;
    int32_t field_1CC;
};

struct struc_312 {
    struc_242 field_0;
    struc_242 field_2C;
    uint8_t field_58;
    uint8_t field_59;
    vec3 field_5C;
    vec3 field_68;
    vec3 field_74;
    vec3 field_80;
    float_t field_8C;
    vector_old_bone_data* bones;
    float_t step;
};

struct bone_data_parent {
    rob_chara_bone_data* rob_bone_data;
    bool field_8;
    bool field_9;
    size_t motion_bone_count;
    size_t ik_bone_count;
    size_t chain_pos;
    vector_old_bone_data bones;
    vector_old_uint16_t bone_indices;
    vec3 global_trans;
    vec3 global_rotation;
    uint32_t bone_key_set_count;
    uint32_t global_key_set_count;
    float_t field_78;
    float_t rot_y;
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
};

vector_old_ptr(motion_blend_mot)
vector_old(mat4)
list_ptr(motion_blend_mot)
list(size_t)

struct rob_chara_bone_data_ik_scale {
    float_t ratio0;
    float_t ratio1;
    float_t ratio2;
    float_t ratio3;
};

struct rob_chara_bone_data {
    uint8_t field_0;
    uint8_t field_1;
    size_t object_bone_count;
    size_t total_bone_count;
    size_t motion_bone_count;
    size_t ik_bone_count;
    size_t chain_pos;
    vector_old_mat4 mats;
    vector_old_mat4 mats2;
    vector_old_bone_node nodes;
    bone_database_skeleton_type base_skeleton_type;
    bone_database_skeleton_type skeleton_type;
    vector_old_ptr_motion_blend_mot motions;
    list_size_t motion_indices;
    list_size_t motion_loaded_indices;
    list_ptr_motion_blend_mot motion_loaded;
    mot_blend face;
    mot_blend hand_l;
    mot_blend hand_r;
    mot_blend mouth;
    mot_blend eye;
    mot_blend eyelid;
    bool disable_eye_motion;
    rob_chara_bone_data_ik_scale ik_scale;
    vec3 field_76C;
    vec3 field_778;
    int32_t field_784;
    struc_258 field_788;
    struc_312 field_958;
};

struct opd_blend_data {
    int32_t motion_id;
    float_t frame;
    float_t frame_count;
    bool field_C;
    MotionBlendType type;
    float_t blend;
};

vector_old(opd_blend_data)

struct obj_skin_block_node {
    const char* parent_name;
    vec3 position;
    vec3 rotation;
    vec3 scale;
};

struct obj_skin_block_osage_node {
    uint32_t name_index;
    float_t length;
    vec3 rotation;
};

struct obj_skin_block_osage {
    obj_skin_block_node node;
    int32_t start_index;
    int32_t count;
    int32_t external_name_index;
    int32_t name_index;
    obj_skin_block_osage_node* nodes;
    int32_t field_48;
    int32_t field_4C;
    int32_t field_50;
    int32_t field_54;
};

struct obj_skin_block_expression {
    obj_skin_block_node node;
    const char* name;
    uint32_t expression_count;
    const char* expressions[9];
};

struct obj_skin_block_cloth_root_bone_weight {
    const char* bone_name;
    float_t weight;
    uint32_t matrix_index;
    uint32_t reserved;
};

struct obj_skin_block_cloth_root {
    vec3 trans;
    vec3 normal;
    float_t field_18;
    int32_t field_1C;
    int32_t field_20;
    int32_t field_24;
    obj_skin_block_cloth_root_bone_weight field_28[4];
};


struct obj_skin_block_cloth_node {
    uint32_t flags;
    vec3 trans;
    vec3 trans_prev;
    float_t dist_top;
    float_t dist_bottom;
    float_t dist_right;
    float_t dist_left;
};

struct skin_param_osage_root_coli {
    int32_t type;
    int32_t bone0_index;
    int32_t bone1_index;
    float_t radius;
    vec3 bone0_pos;
    vec3 bone1_pos;
};

vector_old(skin_param_osage_root_coli)

struct skin_param_osage_root_boc {
    int32_t ed_node;
    string ed_root;
    int32_t st_node;
};

vector_old(skin_param_osage_root_boc)

struct skin_param_osage_root_normal_ref {
    string n;
    string u;
    string d;
    string l;
    string r;
};

vector_old(skin_param_osage_root_normal_ref)

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
    vector_old_skin_param_osage_root_coli coli;
    float_t coli_r;
    float_t friction;
    float_t wind_afc;
    int32_t yz_order;
    vector_old_skin_param_osage_root_boc boc;
    int32_t coli_type;
    float_t stiffness;
    float_t move_cancel;
    string colli_tgt_osg;
    vector_old_skin_param_osage_root_normal_ref normal_ref;
};

struct obj_skin_block_cloth {
    const char* mesh_name;
    const char* backface_mesh_name;
    int32_t field_8;
    uint32_t root_count;
    uint32_t nodes_count;
    int32_t field_14;
    mat4* mats;
    obj_skin_block_cloth_root* root;
    obj_skin_block_cloth_node* nodes;
    uint16_t* mesh_indices;
    uint16_t* backface_mesh_indices;
    skin_param_osage_root* skp_root;
    uint32_t reserved;
};

struct obj_skin_block_constraint_orientation {
    vec3 offset;
};

struct obj_skin_block_constraint_up_vector {
    bool active;
    float_t roll;
    vec3 affected_axis;
    vec3 point_at;
    const char* name;
};

struct obj_skin_block_constraint_direction {
    obj_skin_block_constraint_up_vector up_vector;
    vec3 align_axis;
    vec3 target_offset;
};

struct obj_skin_block_constraint_attach_point {
    int32_t affected_by_orientation;
    int32_t affected_by_scaling;
    vec3 offset;
};

struct obj_skin_block_constraint_position {
    obj_skin_block_constraint_up_vector up_vector;
    obj_skin_block_constraint_attach_point constrained_object;
    obj_skin_block_constraint_attach_point constraining_object;
};

struct obj_skin_block_constraint_distance {
    obj_skin_block_constraint_up_vector up_vector;
    float_t distance;
    obj_skin_block_constraint_attach_point constrained_object;
    obj_skin_block_constraint_attach_point constraining_object;
};

struct obj_skin_block_constraint {
    obj_skin_block_node node;
    const char* type;
    const char* name;
    int32_t coupling;
    const char* source_node_name;
    union {
        obj_skin_block_constraint_orientation orientation;
        obj_skin_block_constraint_direction direction;
        obj_skin_block_constraint_position position;
        obj_skin_block_constraint_distance distance;
    };
};

union obj_skin_block_union {
    obj_skin_block_node base;
    obj_skin_block_cloth cloth;
    obj_skin_block_constraint constraint;
    obj_skin_block_expression expression;
    obj_skin_block_osage osage;
};

struct obj_skin_block {
    const char* signature;
    obj_skin_block_union* block;
};

struct rob_chara_item_equip_object;
struct ExNodeBlock;

struct ExNodeBlock_vtbl {
    void* (*Dispose)(ExNodeBlock*, bool dispose);
    void(*Field8)(ExNodeBlock*);
    void(*Field10)(ExNodeBlock*);
    void(*Field18)(ExNodeBlock*, int32_t, bool);
    void(*Field20)(ExNodeBlock*);
    void(*SetOsagePlayData)(ExNodeBlock*);
    void(*Disp)(ExNodeBlock*);
    void(*Reset)(ExNodeBlock*);
    void(*Field40)(ExNodeBlock*);
    void(*Field48)(ExNodeBlock*);
    void(*Field50)(ExNodeBlock*);
    void(*Field58)(ExNodeBlock*);
};

struct ExNodeBlock {
    ExNodeBlock_vtbl* __vftable;
    bone_node* bone_node_ptr;
    ExNodeType type;
    const char* name;
    bone_node* parent_bone_node;
    string parent_name;
    ExNodeBlock* parent_node;
    rob_chara_item_equip_object* item_equip_object;
    bool field_58;
    bool field_59;
    bool field_5A;
};

struct ExNullBlock {
    ExNodeBlock base;
    obj_skin_block_node* node_data;
};

struct rob_osage_node;

struct rob_osage_node_data_normal_ref {
    bool field_0;
    int field_4;
    rob_osage_node* n;
    rob_osage_node* u;
    rob_osage_node* d;
    rob_osage_node* l;
    rob_osage_node* r;
    mat4 mat;
};

struct skin_param_hinge {
    float_t ymin;
    float_t ymax;
    float_t zmin;
    float_t zmax;
};

struct skin_param_osage_node {
    float_t coli_r;
    float_t weight;
    float_t inertial_cancel;
    skin_param_hinge hinge;
};

vector_old_ptr(rob_osage_node)

struct rob_osage_node_data {
    float_t force;
    vector_old_ptr_rob_osage_node boc;
    rob_osage_node_data_normal_ref normal_ref;
    skin_param_osage_node skp_osg_node;
};

struct opd_vec3_data {
    float_t* x;
    float_t* y;
    float_t* z;
};

vector_old(opd_vec3_data)

struct struc_477 {
    float_t length;
    vec3 angle;
};

struct struc_476 {
    struc_477 field_0;
    struc_477 field_10;
};

struct RobOsageNodeResetData {
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
    mat4 mat;
    rob_osage_node* sibling_node;
    float_t distance;
    vec3 field_94;
    RobOsageNodeResetData reset_data;
    float_t field_C8;
    float_t field_CC;
    vec3 external_force;
    float_t force;
    rob_osage_node_data* data_ptr;
    rob_osage_node_data data;
    vector_old_opd_vec3_data opd_data;
    struc_476 field_1B0;
};

vector_old(rob_osage_node)

struct skin_param {
    vector_old_skin_param_osage_root_coli coli;
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
    vector_old_rob_osage_node* colli_tgt_osg;
};

struct osage_coli {
    int32_t type;
    float_t radius;
    vec3 bone0_pos;
    vec3 bone1_pos;
    vec3 bone_pos_diff;
    float_t bone_pos_diff_length;
    float_t bone_pos_diff_length_squared;
    float_t field_34;
};

vector_old(osage_coli)

struct osage_ring_data {
    float_t ring_rectangle_x;
    float_t ring_rectangle_y;
    float_t ring_rectangle_width;
    float_t ring_rectangle_height;
    float_t ring_height;
    float_t ring_out_height;
    bool field_18;
    vector_old_osage_coli coli;
    vector_old_skin_param_osage_root_coli skp_root_coli;
};

struct pair_int32_t_int32_t {
    int32_t key;
    int32_t value;
};

list(RobOsageNodeResetData)

struct pair_pair_int32_t_int32_t_list_RobOsageNodeResetData {
    pair_int32_t_int32_t key;
    list_RobOsageNodeResetData value;
};

typedef pair_pair_int32_t_int32_t_list_RobOsageNodeResetData motion_reset_data;

tree_def(motion_reset_data)

struct osage_setting_osg_cat {
    rob_osage_parts parts;
    size_t exf;
};

struct rob_osage {
    skin_param* skin_param_ptr;
    bone_node_expression_data exp_data;
    vector_old_rob_osage_node nodes;
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
    mat4 parent_mat;
    float_t move_cancel;
    bool field_1F0C;
    bool osage_reset;
    bool field_1F0E;
    bool field_1F0F;
    osage_ring_data ring;
    tree_motion_reset_data motion_reset_data;
    list_RobOsageNodeResetData* reset_data_list;
    bool set_external_force;
    vec3 external_force;
};

struct ExOsageBlock {
    ExNodeBlock base;
    size_t index;
    rob_osage rob;
    mat4* mat;
    int32_t field_1FF8;
    float_t step;
};

struct ExConstraintBlock {
    ExNodeBlock base;
    obj_skin_block_constraint_type type;
    bone_node* source_node_bone_node;
    bone_node* direction_up_vector_bone_node;
    obj_skin_block_constraint* cns_data;
    int64_t field_80;
};

struct struc_323 {
    int32_t field_0;
    int64_t field_8;
    int64_t field_10;
    int64_t field_18;
    int64_t field_20;
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

struct ExExpressionBlock {
    ExNodeBlock base;
    float_t* values[9];
    ex_expression_block_stack_type types[9];
    ex_expression_block_stack* expressions[9];
    ex_expression_block_stack stack_data[384];
    obj_skin_block_expression* exp_data;
    bool field_3D20;
    void(*field_3D28)(bone_node_expression_data*);
    float_t frame;
    bool init;
};

struct struc_342 {
    int32_t field_0;
    vec3 field_4;
    int64_t field_10;
    int64_t field_18;
    int64_t field_20;
    int64_t field_28;
    int32_t field_30;
    vec3 field_34;
    vec3 field_40;
    vec3 field_4C;
    int64_t field_58;
    int64_t field_60;
    int64_t field_68;
    int64_t field_70;
    int64_t field_78;
    int64_t field_80;
    int64_t field_88;
    int64_t field_90;
    int64_t field_98;
    int64_t field_A0;
    int64_t field_A8;
    int64_t field_B0;
    int64_t field_B8;
    int64_t field_C0;
    int64_t field_C8;
    int64_t field_D0;
    int64_t field_D8;
    int64_t field_E0;
    int64_t field_E8;
};

struct CLOTHNode {
    uint32_t flags;
    vec3 trans;
    vec3 trans_orig;
    vec3 field_1C;
    vec3 trans_diff;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    float_t tangent_sign;
    vec2 texcoord;
    vec3 field_64;
    float_t dist_top;
    float_t dist_bottom;
    float_t dist_right;
    float_t dist_left;
    __int64 field_80;
    int field_88;
    RobOsageNodeResetData reset_data;
    int field_B4;
    vector_old_opd_vec3_data opd_data;
    struc_476 field_D0;
};

struct CLOTH;

struct CLOTH_vtbl {
    CLOTH* (*Init)(CLOTH*);
    CLOTH* (*Dispose)(CLOTH*, bool dispose);
    void(*SetSkinParamColiR)(CLOTH*, float_t coli_r);
    void(*SetSkinParamFriction)(CLOTH*, float_t friction);
    void(*SetSkinParamWindAfc)(CLOTH*, float_t wind_afc);
    void(*SetWindDirection)(CLOTH*, vec3* wind_direction);
    void(*Field_30)(CLOTH*, float_t a2);
    void(*SetSkinParamHinge)(CLOTH*, float_t hinge_y, float_t hinge_z);
    CLOTHNode* (*GetNodes)(CLOTH*);
    void(*Reset)(CLOTH*);
    void(*Field_50)(CLOTH*);
    void(*ResetData)(CLOTH*);
};

vector_old(CLOTHNode)

struct struc_341 {
    size_t field_0;
    size_t field_8;
    float_t field_10;
    int32_t field_14;
};

vector_old(struc_341)

struct CLOTH {
    CLOTH_vtbl* __vftable;
    int32_t field_8;
    size_t root_count;
    size_t nodes_count;
    vector_old_CLOTHNode nodes;
    vec3 wind_direction;
    float_t field_44;
    bool set_external_force;
    vec3 external_force;
    vector_old_struc_341 field_58;
    skin_param* skin_param_ptr;
    skin_param skin_param;
    osage_coli coli[64];
    osage_coli coli_ring[64];
    osage_ring_data ring;
    mat4* mats;
};

struct obj_bounding_sphere {
    vec3 center;
    float_t radius;
};

struct obj_sub_mesh_attrib_member {
    uint32_t recieve_shadow : 1;
    uint32_t cast_shadow : 1;
    uint32_t transparent : 1;
    uint32_t CLOTH : 1;
    uint32_t dummy : 28;
};

union obj_sub_mesh_attrib {
    obj_sub_mesh_attrib_member m;
    uint32_t w;
};

struct obj_axis_aligned_bounding_box {
    vec3 center;
    vec3 size;
};

struct obj_sub_mesh {
    uint32_t flags;
    obj_bounding_sphere bounding_sphere;
    uint32_t material_index;
    uint8_t uv_index[8];
    uint32_t bone_indices_count;
    uint16_t* bone_indices;
    uint32_t bones_per_vertex;
    obj_primitive_type primitive_type;
    obj_index_format index_format;
    uint32_t indices_count;
    uint16_t* indices;
    obj_sub_mesh_attrib attrib;
    uint32_t reserved[4];
    obj_axis_aligned_bounding_box* axis_aligned_bounding_box;
    uint16_t first_index;
    uint16_t last_index;
    uint32_t indices_offset;
};

struct obj_mesh_vertex_array {
    vec3* position;
    vec3* normal;
    vec4* tangent;
    vec3* binormal;
    vec2* texcoord0;
    vec2* texcoord1;
    vec2* texcoord2;
    vec2* texcoord3;
    vec4* color0;
    vec4* color1;
    vec4* bone_weight;
    vec4* bone_index;
    void* field_60;
    void* field_68;
    void* field_70;
    void* field_78;
    void* field_80;
    void* field_88;
    void* field_90;
    void* field_98;
};

union obj_mesh_vertex_array_union {
    obj_mesh_vertex_array data;
    void* ptr[20];
};

struct obj_mesh_attrib_member {
    uint32_t double_buffer : 1;
    uint32_t billboard_y_axis : 1;
    uint32_t translucent_no_shadow : 1;
    uint32_t billboard : 1;
    uint32_t dummy : 28;
};

union obj_mesh_attrib {
    obj_mesh_attrib_member m;
    uint32_t w;
};

struct obj_mesh {
    uint32_t flags;
    obj_bounding_sphere bounding_sphere;
    uint32_t num_submesh;
    obj_sub_mesh* submesh_array;
    obj_vertex_format vertex_format;
    uint32_t size_vertex;
    uint32_t num_vertex;
    obj_mesh_vertex_array_union vertex;
    obj_mesh_attrib attrib;
    uint32_t vertex_flags;
    uint32_t reserved[6];
    char name[64];
};

struct obj_shader_compo_member {
    uint32_t color : 1;
    uint32_t color_a : 1;
    uint32_t color_l1 : 1;
    uint32_t color_l1_a : 1;
    uint32_t color_l2 : 1;
    uint32_t color_l2_a : 1;
    uint32_t transparency : 1;
    uint32_t specular : 1;
    uint32_t normal_01 : 1;
    uint32_t normal_02 : 1;
    uint32_t envmap : 1;
    uint32_t color_l3 : 1;
    uint32_t color_l3_a : 1;
    uint32_t translucency : 1;
    uint32_t flag_14 : 1;
    uint32_t override_ibl : 1;
    uint32_t dummy : 16;
};

union obj_shader_compo {
    obj_shader_compo_member m;
    uint32_t w;
};

struct obj_material_shader_attrib_member {
    obj_material_vertex_translation_type vtx_trans_type : 2;
    obj_material_color_source_type col_src : 2;
    uint32_t is_lgt_diffuse : 1;
    uint32_t is_lgt_specular : 1;
    uint32_t is_lgt_per_pixel : 1;
    uint32_t is_lgt_double : 1;
    obj_material_bump_map_type bump_map_type : 2;
    uint32_t fresnel_type : 4;
    uint32_t line_light : 4;
    uint32_t recieve_shadow : 1;
    uint32_t cast_shadow : 1;
    obj_material_specular_quality specular_quality : 1;
    obj_material_aniso_direction aniso_direction : 2;
    uint32_t dummy : 9;
};

union obj_material_shader_attrib {
    obj_material_shader_attrib_member m;
    uint32_t w;
};

struct obj_texture_attrib_member {
    uint32_t repeat_u : 1;
    uint32_t repeat_v : 1;
    uint32_t mirror_u : 1;
    uint32_t mirror_v : 1;
    uint32_t ignore_alpha : 1;
    uint32_t blend : 5;
    uint32_t alpha_blend : 5;
    uint32_t border : 1;
    uint32_t clamp2edge : 1;
    uint32_t filter : 3;
    uint32_t mipmap : 2;
    uint32_t mipmap_bias : 7;
    uint32_t flag_29 : 1;
    uint32_t anisotropic_filter : 2;
};

union obj_texture_attrib {
    obj_texture_attrib_member m;
    uint32_t w;
};

struct obj_texture_shader_attrib_member {
    obj_material_texture_type tex_type : 4;
    int32_t uv_idx : 4;
    obj_material_texture_coordinate_translation_type texcoord_trans : 3;
    uint32_t dummy : 21;
};

union obj_texture_shader_attrib {
    obj_texture_shader_attrib_member m;
    uint32_t w;
};

struct obj_material_texture_data {
    obj_texture_attrib attrib;
    uint32_t tex_index;
    obj_texture_shader_attrib shader_info;
    char ex_shader[8];
    float_t weight;
    mat4 tex_coord_mat;
    union {
        uint32_t reserved[8];
        int32_t texture_index;
    };
};

struct obj_material_attrib_member {
    uint32_t alpha_texture : 1;
    uint32_t alpha_material : 1;
    uint32_t punch_through : 1;
    uint32_t double_sided : 1;
    uint32_t normal_dir_light : 1;
    obj_material_blend_factor src_blend_factor : 4;
    obj_material_blend_factor dst_blend_factor : 4;
    uint32_t blend_operation : 3;
    uint32_t zbias : 4;
    uint32_t no_fog : 1;
    uint32_t translucent_priority : 6;
    uint32_t has_fog_height : 1;
    uint32_t flag_28 : 1;
    uint32_t fog_height : 1;
    uint32_t flag_30 : 1;
    uint32_t flag_31 : 1;
};

union obj_material_attrib {
    obj_material_attrib_member m;
    uint32_t w;
};

struct obj_material_color {
    vec4 diffuse;
    vec4 ambient;
    vec4 specular;
    vec4 emission;
    float_t shininess;
    float_t intensity;
};

struct obj_material {
    obj_shader_compo shader_compo;
    union {
        char name[8];
        int32_t index;
    } shader;
    obj_material_shader_attrib shader_info;
    obj_material_texture_data texdata[8];
    obj_material_attrib attrib;
    obj_material_color color;
    vec3 center;
    float_t radius;
    char name[64];
    float_t bump_depth;
    uint32_t reserved[15];
};

struct obj_material_data {
    uint32_t num_of_textures;
    obj_material material;
};

struct obj {
    uint32_t version;
    uint32_t flags;
    obj_bounding_sphere bounding_sphere;
    uint32_t num_mesh;
    obj_mesh* mesh_array;
    uint32_t num_material;
    obj_material_data* material_array;
    uint32_t reserved[10];
};

struct obj_mesh_vertex_buffer {
    int32_t count;
    GLuint buffers[3];
    int32_t index;
    GLenum enum_data;
};

struct obj_vertex_buffer {
    int32_t meshes_count;
    obj_mesh_vertex_buffer* meshes;
};

typedef GLuint obj_mesh_index_buffer;

struct obj_index_buffer {
    int32_t meshes_count;
    obj_mesh_index_buffer* meshes;
};

struct obj_skin_osage_node {
    int32_t name_index;
    float_t length;
    int32_t index;
};

struct obj_skin_osage_sibling_info {
    int32_t name_index;
    int32_t sibling_name_index;
    float_t distance;
};

struct obj_skin_ex_data {
    int32_t osage_name_count;
    int32_t osage_node_count;
    const char** field_8;
    obj_skin_osage_node* osage_nodes;
    const char** osage_names;
    obj_skin_block* blocks;
    int32_t bone_names_count;
    const char** bone_names;
    obj_skin_osage_sibling_info* osage_sibling_infos;
    uint32_t cloth_count;
    uint32_t reserved[7];
};

struct obj_skin {
    uint32_t* bone_ids;
    mat4* bone_matrices;
    const char** bone_names;
    obj_skin_ex_data* ex_data;
    uint32_t bone_count;
    int32_t bone_parent_ids_offset;
    int32_t field_28;
    int32_t field_2C;
    int32_t field_30;
};

struct obj_set_info {
    int32_t id;
    string name;
    string object_file_name;
    string texture_file_name;
    string archive_file_name;
};

struct pair_uint32_t_obj_set_info {
    uint32_t id;
    obj_set_info data;
};

vector_old(pair_uint32_t_obj_set_info)

struct obj_database {
    vector_old_pair_uint32_t_obj_set_info field_0;
    size_t field_18;
    vector_old_pair_uint32_t_obj_set_info field_20;
    bool field_38;
};

struct obj_set {
    uint32_t signature;
    uint32_t objects_count;
    uint32_t max_object_id;
    obj** objects;
    obj_skin** object_skins;
    const char** object_names;
    uint32_t* object_ids;
    uint32_t* texture_ids;
    uint32_t texture_ids_count;
    uint32_t field_3C;
    uint32_t field_40;
};

struct obj_bounding_box {
    vec3 center;
    vec3 size;
};

struct texture {
    int32_t init_count;
    int32_t id;
    texture_flags flags;
    int16_t width;
    int16_t height;
    GLuint texture;
    GLenum target;
    GLenum internal_format;
    int32_t max_mipmap_level;
    int32_t size;
};

struct RobClothSubMeshArray {
    obj_sub_mesh arr[4];
};

struct struc_295 {
    int32_t field_0;
    int32_t field_4;
    int64_t field_8;
    int64_t field_10;
};

vector_old(struc_295)

struct RobClothRoot {
    vec3 trans;
    vec3 normal;
    vec4 tangent;
    bone_node* node[4];
    mat4* node_mat[4];
    mat4* mat[4];
    float_t weight[4];
    mat4 field_98;
    mat4 field_D8;
    mat4 field_118;
};

vector_old(RobClothRoot)

struct RobCloth {
    CLOTH base;
    vector_old_RobClothRoot root;
    rob_chara_item_equip_object* itm_eq_obj;
    obj_skin_block_cloth_root* cls_root;
    obj_skin_block_cloth* cls_data;
    float_t move_cancel;
    bool osage_reset;
    obj_mesh mesh[2];
    RobClothSubMeshArray submesh[2];
    obj_axis_aligned_bounding_box axis_aligned_bounding_box;
    obj_mesh_vertex_buffer vertex_buffer[2];
    GLuint index_buffer[2];
    tree_motion_reset_data motion_reset_data;
    list_RobOsageNodeResetData* reset_data_list;
};

struct ExClothBlock {
    ExNodeBlock base;
    RobCloth rob;
    obj_skin_block_cloth* cls_data;
    mat4* mats;
    size_t index;
};

vector_old_ptr(ExNodeBlock)
vector_old_ptr(ExNullBlock)
vector_old_ptr(ExOsageBlock)
vector_old_ptr(ExConstraintBlock)
vector_old_ptr(ExExpressionBlock)
vector_old_ptr(ExClothBlock)

struct ex_data_name_bone_index {
    const char* name;
    int32_t bone_index;
};

vector_old(ex_data_name_bone_index)

struct texture_data_struct {
    int32_t field_0;
    vec3 texture_color_coefficients;
    vec3 texture_color_offset;
    vec3 texture_specular_coefficients;
    vec3 texture_specular_offset;
};

struct rob_chara_item_equip_object {
    size_t index;
    mat4* mats;
    object_info object_info;
    int32_t field_14;
    vector_old_texture_pattern_struct texture_pattern;
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
    vector_old_ptr_ExNodeBlock node_blocks;
    vector_old_bone_node ex_data_bone_nodes;
    vector_old_mat4 ex_data_matrices;
    vector_old_mat4 field_108;
    vector_old_ex_data_name_bone_index ex_bones;
    int64_t field_138;
    vector_old_ptr_ExNullBlock null_blocks;
    vector_old_ptr_ExOsageBlock osage_blocks;
    vector_old_ptr_ExConstraintBlock constraint_blocks;
    vector_old_ptr_ExExpressionBlock expression_blocks;
    vector_old_ptr_ExClothBlock cloth_blocks;
    int8_t field_1B8;
    size_t frame_count;
    bool use_opd;
    obj_skin_ex_data* skin_ex_data;
    obj_skin* skin;
    rob_chara_item_equip* item_equip;
};

struct rob_chara_item_equip {
    bone_node* bone_nodes;
    mat4* matrices;
    rob_chara_item_equip_object* item_equip_object;
    int32_t field_18[31];
    bool item_equip_range;
    int32_t first_item_equip_object;
    int32_t max_item_equip_object;
    int32_t field_A0;
    shadow_type_enum shadow_type;
    vec3 position;
    int32_t field_B4;
    vector_old_texture_pattern_struct texture_pattern;
    object_info field_D0;
    item_id field_D4;
    bool disable_update;
    int32_t field_DC;
    vec4 texture_color_coefficients;
    float_t wet;
    float_t wind_strength;
    bool chara_color;
    bool npr_flag;
    mat4 mat;
    mat4 field_13C[30];
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
    vector_old_opd_blend_data opd_blend_data;
    bool parts_short;
    bool parts_append;
    bool parts_white_one_l;
};

extern void ExNodeBlock__Field_10(ExNodeBlock* node);

extern ExOsageBlock* ExOsageBlock__Dispose(ExOsageBlock* osg, bool dispose);
extern void ExOsageBlock__Init(ExOsageBlock* osg);
extern void ExOsageBlock__Field_18(ExOsageBlock* osg, int32_t a2, bool a3);
extern void ExOsageBlock__Field_20(ExOsageBlock* osg);
extern void ExOsageBlock__SetOsagePlayData(ExOsageBlock* osg);
extern void ExOsageBlock__Disp(ExOsageBlock* osg);
extern void ExOsageBlock__Reset(ExOsageBlock* osg);
extern void ExOsageBlock__Field_40(ExOsageBlock* osg);
extern void ExOsageBlock__Field_48(ExOsageBlock* osg);
extern void ExOsageBlock__Field_50(ExOsageBlock* osg);
extern void ExOsageBlock__Field_58(ExOsageBlock* osg);