/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/database/bone.h"
#include "../KKdLib/database/motion.h"
#include "../KKdLib/mat.h"
#include "../KKdLib/mot.h"
#include "../KKdLib/vec.h"
#include "draw_object.h"
#include "object.h"
#include "static_var.h"

typedef enum chara_index {
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
} chara_index;

typedef enum ex_expression_block_stack_type {
    EX_EXPRESSION_BLOCK_STACK_NUMBER          = 0x00,
    EX_EXPRESSION_BLOCK_STACK_VARIABLE        = 0x01,
    EX_EXPRESSION_BLOCK_STACK_VARIABLE_RADIAN = 0x02,
    EX_EXPRESSION_BLOCK_STACK_OP1             = 0x03,
    EX_EXPRESSION_BLOCK_STACK_OP2             = 0x04,
    EX_EXPRESSION_BLOCK_STACK_OP3             = 0x05,
} ex_expression_block_stack_type;

typedef enum ex_node_type {
    EX_NONE       = 0x00,
    EX_OSAGE      = 0x01,
    EX_EXPRESSION = 0x02,
    EX_CONSTRAINT = 0x03,
    EX_CLOTH      = 0x04,
} ex_node_type;

typedef enum eyes_base_adjust_type {
    EYES_BASE_ADJUST_DIRECTION = 0,
    EYES_BASE_ADJUST_CLEARANCE = 1,
    EYES_BASE_ADJUST_OFF       = 2,
} eyes_base_adjust_type;

typedef enum item_id {
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
} item_id;

typedef enum item_sub_id {
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
} item_sub_id;

typedef enum motion_bone_index {
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
} motion_bone_index;

typedef struct bone_node bone_node;
typedef struct rob_chara_data_bone_data rob_chara_data_bone_data;

typedef struct bone_node_expression_data {
    vec3 position;
    vec3 rotation;
    vec3 scale;
    vec3 data_scale;
} bone_node_expression_data;

struct bone_node {
    char* name;
    mat4* mat;
    bone_node* parent;
    bone_node_expression_data exp_data;
    mat4* ex_data_mat;
};

vector(bone_node)

typedef struct struc_314 {
    uint32_t* field_0;
    size_t field_8;
} struc_314;

typedef struct mot_key_set {
    mot_key_set_type type;
    int32_t keys_count;
    int32_t current_key;
    int32_t last_key;
    uint16_t* frames;
    float_t* values;
} mot_key_set;

vector(mot_key_set)

typedef union request_data_object_data {
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
    };
    int32_t array[25];
} request_data_object_data;

typedef struct eyes_adjust {
    bool xrot_adjust;
    eyes_base_adjust_type base_adjust;
    float_t neg;
    float_t pos;
} eyes_adjust;

typedef struct struc_313 {
    vector_uint32_t bitfield;
    size_t motion_bone_count;
} struc_313;

typedef struct mot {
    uint16_t key_set_count;
    uint16_t frame_count;
    uint16_t field_4;
    mot_key_set* key_sets;
} mot;

typedef struct struc_369 {
    int32_t field_0;
    float_t field_4;
} struc_369;

typedef struct mot_parent {
    bool key_sets_ready;
    size_t key_set_count;
    vector_mot_key_set key_set;
    mot mot;
    vector_float_t key_set_data;
    mot_data* mot_data;
    bone_database_skeleton_type skeleton_type;
    int32_t skeleton_select;
    int32_t motion_id;
    float_t frame;
    struc_369 field_68;
} mot_parent;

typedef struct bone_data {
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
    vec3 trans_dup[2];
    mat4u rot_mat_dup[6];
    mat4* pole_target_mat;
    mat4* parent_mat;
    bone_node* node;
    float_t ik_segment_length[2];
    float_t ik_2nd_segment_length[2];
    float_t field_2E0;
    float_t eyes_xrot_adjust_neg;
    float_t eyes_xrot_adjust_pos;
} bone_data;

vector(bone_data)

typedef struct bone_data_parent {
    rob_chara_data_bone_data* rob_bone_data;
    size_t motion_bone_count;
    size_t ik_bone_count;
    size_t chain_pos;
    vector_bone_data bones;
    vector_uint16_t bone_indices;
    vec3 global_trans;
    vec3 global_rotation;
    uint32_t bone_key_set_count;
    uint32_t global_key_set_count;
    float_t rotation_y;
} bone_data_parent;

typedef struct struc_346 {
    float_t frame;
    float_t step;
    float_t field_8;
    float_t frame_count;
    float_t last_frame;
    float_t field_14;
    int32_t field_18;
    int32_t field_1C;
    float_t field_20;
    float_t field_24;
    bool field_28;
    int32_t field_2C;
} struc_346;

typedef struct struc_370 {
    struc_346 field_0;
    int32_t field_30;
    bool field_34;
    float_t field_38;
    float_t* field_40;
    float_t* field_48;
} struc_370;

typedef struct struc_308 {
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
    float_t field_B8;
    uint8_t field_BC;
    uint8_t field_BD;
    float_t field_C0;
    float_t field_C4;
    vec3 field_C8;
} struc_308;

typedef struct motion_blend_mot {
    bone_data_parent bone_data;
    mot_parent mot_data;
    struc_370 field_4A8;
    struc_308 field_4F8;
} motion_blend_mot;

vector_ptr(motion_blend_mot)

typedef struct rob_chara_data_bone_data_ik_scale {
    float_t ratio0;
    float_t ratio1;
    float_t ratio2;
    float_t ratio3;
} rob_chara_data_bone_data_ik_scale;

struct rob_chara_data_bone_data {
    size_t object_bone_count;
    size_t total_bone_count;
    size_t motion_bone_count;
    size_t ik_bone_count;
    size_t chain_pos;
    vector_mat4 mats;
    vector_mat4 mats2;
    vector_bone_node nodes;
    bone_database_skeleton_type base_skeleton_type;
    bone_database_skeleton_type skeleton_type;
    vector_ptr_motion_blend_mot motions;
    vector_size_t motion_indices;
    vector_size_t motion_loaded_indices;
    vector_ptr_motion_blend_mot motion_loaded;
    rob_chara_data_bone_data_ik_scale ik_scale;
};

typedef struct struc_242 {
    int32_t field_0;
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
} struc_242;

typedef struct rob_chara_pv_data {
    int32_t index;
    bool field_4;
    bool field_5;
    bool field_6;
    vec3 field_8;
    int16_t field_14;
    int16_t field_16;
    struc_242 field_18;
    struc_242 field_44;
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
    bool field_A0;
    int32_t shadow_type[4];
    eyes_adjust eyes_adjust;
} rob_chara_pv_data;

typedef struct struc_218 {
    vec3 bone_offset;
    float_t scale;
    int32_t bone_index;
    int32_t field_14;
} struc_218;

typedef struct struc_344 {
    int32_t field_0;
    int32_t field_4;
    int32_t field_8;
    int32_t field_C;
} struc_344;

typedef struct chara_init_data {
    int32_t object_set;
    bone_database_skeleton_type skeleton_type;
    object_info base_face;
    int32_t motion_set;
    const struc_218* field_828;
    const struc_218* field_830;
    int32_t field_840;
    int32_t field_844;
    struc_344 field_848;
    object_info head_objects[9];
    object_info face_objects[15];
} chara_init_data;

typedef struct struc_241 {
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
} struc_241;

typedef struct ex_node_block ex_node_block;
typedef struct rob_chara_data_item_equip_object rob_chara_data_item_equip_object;

typedef struct ex_node_block_vtbl {
    void(*dispose)(ex_node_block*);
    //void(*field_8)(ex_node_block*);
    void(*set_default_data)(ex_node_block*);
    //void(*field_18)(ex_node_block*, int32_t, int32_t);
    void(*update)(ex_node_block*);
    //void(*field_28)(ex_node_block*);
    void(*draw)(ex_node_block*);
    void(*reset)(ex_node_block*);
    //void(*field_40)(ex_node_block*);
    //void(*field_48)(ex_node_block*);
    //void(*field_50)(ex_node_block*);
    //void(*field_58)(ex_node_block*);
} ex_node_block_vtbl;

struct ex_node_block {
    const ex_node_block_vtbl* vftable;
    bone_node* bone_node;
    ex_node_type type;
    char* name;
    bone_node* parent_bone_node;
    string parent_name;
    ex_node_block* parent_node;
    rob_chara_data_item_equip_object* item_equip_object;
    bool field_58;
    bool set_data;
    bool field_5A;
};

typedef struct ex_cloth_block {
    ex_node_block base;
    //rob_cloth rob;
    object_skin_block_cloth* cloth_block;
    mat4* field_2428;
    size_t index;
} ex_cloth_block;

typedef struct struc_331 {
    int64_t field_0;
    int64_t field_8;
    int64_t field_10;
} struc_331;

vector(struc_331)

typedef struct rob_osage_node rob_osage_node;

typedef struct rob_osage_node_data_normal_ref {
    bool field_0;
    rob_osage_node* n;
    rob_osage_node* u;
    rob_osage_node* d;
    rob_osage_node* l;
    rob_osage_node* r;
    mat4u mat;
} rob_osage_node_data_normal_ref;

typedef struct skin_param_hinge {
    float_t ymin;
    float_t ymax;
    float_t zmin;
    float_t zmax;
} skin_param_hinge;

typedef struct skin_param_osage_node {
    float_t coli_r;
    float_t weight;
    float_t inertial_cancel;
    skin_param_hinge hinge;
} skin_param_osage_node;

vector_ptr(rob_osage_node)

typedef struct rob_osage_node_data {
    float_t force;
    vector_ptr_rob_osage_node boc;
    rob_osage_node_data_normal_ref normal_ref;
    skin_param_osage_node skp_osg_node;
} rob_osage_node_data;

struct rob_osage_node {
    float_t length;
    vec3 trans;
    vec3 field_10;
    vec3 field_1C;
    vec3 field_28;
    float_t child_length;
    bone_node* bone_node;
    mat4* bone_node_mat;
    mat4u mat;
    rob_osage_node* sibling_node;
    float_t distance;
    vec3 field_94;
    vec3 field_A0;
    vec3 field_AC;
    vec3 field_B8;
    float_t field_C4;
    float_t field_C8;
    float_t field_CC;
    vec3 field_D0;
    float_t field_DC;
    rob_osage_node_data* data_ptr;
    rob_osage_node_data data;
    int32_t field_194;
    vector_struc_331 field_198;
    int32_t field_1B0;
    vec3 field_1B4;
    vec3 field_1C0;
    float_t field_1CC;
};

vector(rob_osage_node)

typedef struct pair_int32_t_ptr_rob_osage_node {
    int32_t key;
    rob_osage_node* value;
} pair_int32_t_ptr_rob_osage_node;

vector(pair_int32_t_ptr_rob_osage_node)

typedef struct skin_param_osage_root_coli {
    int32_t type;
    int32_t bone0_index;
    int32_t bone1_index;
    float_t radius;
    vec3 bone0_pos;
    vec3 bone1_pos;
} skin_param_osage_root_coli;

vector(skin_param_osage_root_coli)

typedef struct skin_param {
    vector_skin_param_osage_root_coli coli;
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
    vector_rob_osage_node* colli_tgt_osg;
} skin_param;

typedef struct osage_coli {
    int32_t type;
    float_t radius;
    vec3 bone0_pos;
    vec3 bone1_pos;
    vec3 bone_pos_diff;
    float_t bone_pos_diff_length;
    float_t bone_pos_diff_length_squared;
    float_t field_34;
} osage_coli;

vector(osage_coli)

typedef struct osage_ring_data {
    float_t ring_rectangle_x;
    float_t ring_rectangle_y;
    float_t ring_rectangle_width;
    float_t ring_rectangle_height;
    float_t ring_height;
    float_t ring_out_height;
    bool field_18;
    vector_osage_coli coli;
    vector_skin_param_osage_root_coli skp_root_coli;
} osage_ring_data;

typedef struct rob_osage {
    skin_param* skin_param_ptr;
    bone_node_expression_data exp_data;
    vector_rob_osage_node nodes;
    rob_osage_node node;
    skin_param skin_param;
    int32_t field_290;
    int64_t field_298;
    bool field_2A0;
    bool field_2A1;
    float_t field_2A4;
    osage_coli coli[64];
    osage_coli coli_ring[64];
    vec3 wind_direction;
    float_t field_1EB4;
    int32_t field_1EB8;
    int32_t field_1EBC;
    mat4* mats;
    mat4u mat;
    float_t field_1F08;
    bool field_1F0C;
    bool field_1F0D;
    bool field_1F0E;
    bool field_1F0F;
    osage_ring_data ring;
    //tree_295 field_1F60;
    int64_t field_1F70;
    bool field_1F78;
    vec3 field_1F7C;
} rob_osage;

typedef struct ex_osage_block {
    ex_node_block base;
    size_t index;
    rob_osage rob;
    mat4* mat;
    int32_t field_1FF8;
    float_t osage_phys_step;
} ex_osage_block;

typedef struct pair_name_ex_osage_block {
    char* name;
    ex_osage_block* block;
} pair_name_ex_osage_block;

vector(pair_name_ex_osage_block)

typedef struct ex_constraint_block {
    ex_node_block base;
    object_skin_block_constraint_type type;
    bone_node* source_node_bone_node;
    bone_node* direction_up_vector_bone_node;
    object_skin_block_constraint* cns_data;
    int64_t field_80;
} ex_constraint_block;

typedef struct ex_expression_block_stack ex_expression_block_stack;

typedef struct ex_expression_block_stack_number {
    float_t value;
} ex_expression_block_stack_number;

typedef struct ex_expression_block_stack_variable {
    float_t* value;
} ex_expression_block_stack_variable;

typedef struct ex_expression_block_stack_variable_radian {
    float_t* value;
} ex_expression_block_stack_variable_radian;

typedef struct ex_expression_block_stack_op1 {
    float_t(*func)(float_t v1);
    ex_expression_block_stack* v1;
} ex_expression_block_stack_op1;

typedef struct ex_expression_block_stack_op2 {
    float_t(*func)(float_t v1, float_t v2);
    ex_expression_block_stack* v1;
    ex_expression_block_stack* v2;
} ex_expression_block_stack_op2;

typedef struct ex_expression_block_stack_op3 {
    float_t(*func)(float_t v1, float_t v2, float_t v3);
    ex_expression_block_stack* v1;
    ex_expression_block_stack* v2;
    ex_expression_block_stack* v3;
} ex_expression_block_stack_op3;

struct ex_expression_block_stack {
    ex_expression_block_stack_type type;
    union ex_expression_block_stack_union {
        ex_expression_block_stack_number number;
        ex_expression_block_stack_variable var;
        ex_expression_block_stack_variable_radian var_rad;
        ex_expression_block_stack_op1 op1;
        ex_expression_block_stack_op2 op2;
        ex_expression_block_stack_op3 op3;
    };
};

typedef struct ex_expression_block {
    ex_node_block base;
    float_t* values[9];
    ex_expression_block_stack_type types[9];
    ex_expression_block_stack* expressions[9];
    ex_expression_block_stack stack_data[384];
    object_skin_block_expression* exp_data;
    bool field_3D20;
    void(*field_3D28)(bone_node_expression_data*);
    float_t frame;
    bool osage_phys_step;
} ex_expression_block;

vector_ptr(ex_constraint_block)
vector_ptr(ex_cloth_block)
vector_ptr(ex_expression_block)
vector_ptr(ex_osage_block)
vector_ptr(ex_node_block)

typedef struct ex_data_name_bone_index {
    char* name;
    int32_t bone_index;
} ex_data_name_bone_index;

vector(ex_data_name_bone_index)

typedef struct rob_chara_data_item_equip rob_chara_data_item_equip;

struct rob_chara_data_item_equip_object {
    size_t index;
    mat4* mat;
    object_info object_info;
    int32_t field_14;
    vector_texture_pattern_struct texture_pattern;
    texture_data_struct texture_data;
    bool field_64;
    bone_node_expression_data exp_data;
    float_t alpha;
    draw_task_flags draw_task_flags;
    bool draw;
    int32_t field_A4;
    mat4* field_A8;
    int32_t field_B0;
    bone_node* bone_nodes;
    vector_ptr_ex_node_block node_blocks;
    vector_bone_node ex_data_bone_nodes;
    vector_mat4 field_F0;
    vector_mat4 field_108;
    vector_ex_data_name_bone_index ex_bones;
    int64_t field_138;
    vector_ptr_ex_osage_block osage_blocks;
    vector_ptr_ex_constraint_block constraint_blocks;
    vector_ptr_ex_expression_block expression_blocks;
    vector_ptr_ex_cloth_block cloth_blocks;
    bool field_1B8;
    int64_t field_1C0;
    bool field_1C8;
    object_skin_ex_data* skin_ex_data;
    object_skin* skin;
    rob_chara_data_item_equip* item_equip;
};

struct rob_chara_data_item_equip {
    bone_node* bone_nodes;
    mat4* matrices;
    rob_chara_data_item_equip_object* item_equip_object;
    int32_t field_18[31];
    bool item_equip_range;
    int32_t first_item_equip_object;
    int32_t max_item_equip_object;
    int32_t field_A0;
    shadow_type_enum shadow_type;
    vec3 position;
    int32_t eyes_adjust;
    vector_texture_pattern_struct texture_pattern;
    object_info field_D0;
    int32_t field_D4;
    bool field_D8;
    int32_t field_DC;
    vec4u texture_color_coeff;
    float_t wet;
    float_t wind_strength;
    bool chara_color;
    bool field_F9;
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
    float_t osage_phys_step;
    bool field_93C;
    //vector_struc_373 field_940;
    bool field_958;
    bool field_959;
    bool field_95A;
};

typedef union item_sub_data {
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
    };
    int32_t data[25];
} item_sub_data;

typedef struct rob_chara_data {
    int8_t chara_id;
    chara_index chara_index;
    int32_t module_index;
    rob_chara_data_bone_data* bone_data;
    rob_chara_data_item_equip* item_equip;
    chara_init_data* chara_init_data;
    rob_chara_pv_data pv_data;
} rob_chara_data;

typedef struct skeleton_rotation_offset {
    bool x;
    bool y;
    bool z;
    vec3 rotation;
} skeleton_rotation_offset;

extern rob_chara_data rob_chara_data_array[];

extern chara_init_data* chara_init_data_get(chara_index chara_index);

extern void motion_set_load_motion(uint32_t set, string* motion_name, motion_database* mot_db);
extern void motion_set_unload(uint32_t set);

extern void rob_chara_data_init(rob_chara_data* rob_chr_data);
extern void rob_chara_data_calc(rob_chara_data* rob_chr_data);
extern void rob_chara_data_draw(rob_chara_data* rob_chr_data, render_context* rctx);
extern mat4* rob_chara_data_get_bone_data_mat(rob_chara_data* chara, size_t index);
extern float_t rob_chara_data_get_frame(rob_chara_data* rob_chr_data);
extern float_t rob_chara_data_get_frame_count(rob_chara_data* rob_chr_data);
extern void rob_chara_data_load_motion(rob_chara_data* rob_chr_data, int32_t motion_id,
    int32_t index, bone_database* bone_data, motion_database* mot_db);
extern void rob_chara_data_reload_items(rob_chara_data* rob_chr_data,
    item_sub_data* sub_data, bone_database* bone_data);
extern void rob_chara_data_reset(rob_chara_data* a1, rob_chara_pv_data* pv_data,
    bone_database* bone_data, motion_database* mot_db);
extern void rob_chara_data_set(rob_chara_data* rob_chr_data, int8_t chara_id,
    chara_index chara_index, uint32_t module_index, rob_chara_pv_data* pv_data);
extern void rob_chara_data_set_frame(rob_chara_data* rob_chr_data, float_t frame);
extern void rob_chara_data_free(rob_chara_data* rob_chr_data);

extern void rob_chara_data_array_init();
extern rob_chara_data* rob_chara_data_array_get(int32_t chara_id);
extern void rob_chara_data_array_free();

extern void rob_chara_pv_data_init(rob_chara_pv_data* pv_data);

extern bool rob_chara_pv_data_array_check_chara_id(int32_t chara_id);

extern void motion_storage_init();
extern void motion_storage_append_mot_set(uint32_t set_id);
extern void motion_storage_insert_motion_set(mot_set* set, uint32_t set_id);
extern mot_set* motion_storage_get_motion_set(uint32_t set_id);
extern void motion_storage_delete_motion_set(uint32_t set_id);
extern mot_data* motion_storage_get_mot_data(uint32_t motion_id, motion_database* mot_db);
extern void motion_storage_free();