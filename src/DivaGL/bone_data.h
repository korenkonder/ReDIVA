/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"
#include "../KKdLib/mat.h"
#include "../KKdLib/string.h"
#include "../KKdLib/vec.h"
#include "../KKdLib/vector.h"
#include "shader_table.h"
#include <glad/glad.h>

typedef enum shadow_type_enum {
    SHADOW_CHARA = 0x00,
    SHADOW_STAGE = 0x01,
} shadow_type_enum;

typedef enum aft_obj_vertex_attrib_type {
    AFT_OBJ_VERTEX_ATTRIB_POSITION    = 0x0001,
    AFT_OBJ_VERTEX_ATTRIB_NORMAL      = 0x0002,
    AFT_OBJ_VERTEX_ATTRIB_TANGENT     = 0x0004,
    AFT_OBJ_VERTEX_ATTRIB_BINORMAL    = 0x0008,
    AFT_OBJ_VERTEX_ATTRIB_TEXCOORD0   = 0x0010,
    AFT_OBJ_VERTEX_ATTRIB_TEXCOORD1   = 0x0020,
    AFT_OBJ_VERTEX_ATTRIB_TEXCOORD2   = 0x0040,
    AFT_OBJ_VERTEX_ATTRIB_TEXCOORD3   = 0x0080,
    AFT_OBJ_VERTEX_ATTRIB_COLOR0      = 0x0100,
    AFT_OBJ_VERTEX_ATTRIB_COLOR1      = 0x0200,
    AFT_OBJ_VERTEX_ATTRIB_BONE_WEIGHT = 0x0400,
    AFT_OBJ_VERTEX_ATTRIB_BONE_INDEX  = 0x0800,
    AFT_OBJ_VERTEX_ATTRIB_UNK_1000    = 0x1000,
} aft_obj_vertex_attrib_type;

typedef enum aft_obj_mesh_flags {
    AFT_OBJ_MESH_1                     = 0x01,
    AFT_OBJ_MESH_FACE_CAMERA_POSITION  = 0x02,
    AFT_OBJ_MESH_TRANSLUCENT_NO_SHADOW = 0x04,
    AFT_OBJ_MESH_FACE_CAMERA_VIEW      = 0x08,
} aft_obj_mesh_flags;

typedef enum aft_obj_material_flags {
    AFT_OBJ_MATERIAL_COLOR          = 0x0001,
    AFT_OBJ_MATERIAL_COLOR_ALPHA    = 0x0002,
    AFT_OBJ_MATERIAL_COLOR_L1       = 0x0004,
    AFT_OBJ_MATERIAL_COLOR_L1_ALPHA = 0x0008,
    AFT_OBJ_MATERIAL_COLOR_L2       = 0x0010,
    AFT_OBJ_MATERIAL_COLOR_L2_ALPHA = 0x0020,
    AFT_OBJ_MATERIAL_TRANSPARENCY   = 0x0040,
    AFT_OBJ_MATERIAL_SPECULAR       = 0x0080,
    AFT_OBJ_MATERIAL_NORMAL         = 0x0100,
    AFT_OBJ_MATERIAL_NORMALALT      = 0x0200,
    AFT_OBJ_MATERIAL_ENVIRONMENT    = 0x0400,
    AFT_OBJ_MATERIAL_COLOR_L3       = 0x0800,
    AFT_OBJ_MATERIAL_COLOR_L3_ALPHA = 0x1000,
    AFT_OBJ_MATERIAL_TRANSLUCENCY   = 0x2000,
    AFT_OBJ_MATERIAL_FLAG_14        = 0x4000,
    AFT_OBJ_MATERIAL_OVERRIDE_IBL   = 0x8000,
} aft_obj_material_flags;

typedef enum aft_obj_material_texture_sampler_flags {
    AFT_OBJ_MATERIAL_TEXTURE_SAMPLER_REPEAT_U      = 0x00001,
    AFT_OBJ_MATERIAL_TEXTURE_SAMPLER_REPEAT_V      = 0x00002,
    AFT_OBJ_MATERIAL_TEXTURE_SAMPLER_MIRROR_U      = 0x00004,
    AFT_OBJ_MATERIAL_TEXTURE_SAMPLER_MIRROR_V      = 0x00008,
    AFT_OBJ_MATERIAL_TEXTURE_SAMPLER_IGNORE_ALPHA  = 0x00010,
    AFT_OBJ_MATERIAL_TEXTURE_SAMPLER_BORDER        = 0x08000,
    AFT_OBJ_MATERIAL_TEXTURE_SAMPLER_CLAMP_TO_EDGE = 0x10000,
} aft_obj_material_texture_sampler_flags;

typedef enum aft_obj_primitive_type {
    AFT_OBJ_PRIMITIVE_POINTS         = 0x00,
    AFT_OBJ_PRIMITIVE_LINES          = 0x01,
    AFT_OBJ_PRIMITIVE_LINE_STRIP     = 0x02,
    AFT_OBJ_PRIMITIVE_LINE_LOOP      = 0x03,
    AFT_OBJ_PRIMITIVE_TRIANGLES      = 0x04,
    AFT_OBJ_PRIMITIVE_TRIANGLE_STRIP = 0x05,
    AFT_OBJ_PRIMITIVE_TRIANGLE_FAN   = 0x06,
    AFT_OBJ_PRIMITIVE_QUADS          = 0x07,
    AFT_OBJ_PRIMITIVE_QUAD_STRIP     = 0x08,
    AFT_OBJ_PRIMITIVE_POLYGON        = 0x09,
} aft_obj_primitive_type;

typedef enum aft_obj_index_format {
    AFT_OBJ_INDEX_U8  = 0x00,
    AFT_OBJ_INDEX_U16 = 0x01,
    AFT_OBJ_INDEX_U32 = 0x02,
} aft_obj_index_format;

typedef enum aft_obj_sub_mesh_flags {
    AFT_OBJ_SUB_MESH_RECIEVE_SHADOW = 0x01,
    AFT_OBJ_SUB_MESH_CAST_SHADOW    = 0x02,
    AFT_OBJ_SUB_MESH_TRANSPARENT    = 0x04,
    AFT_OBJ_SUB_MESH_FLAG_8         = 0x08,
    AFT_OBJ_SUB_MESH_FLAG_10        = 0x10,
    AFT_OBJ_SUB_MESH_FLAG_20        = 0x20,
    AFT_OBJ_SUB_MESH_FLAG_40        = 0x40,
    AFT_OBJ_SUB_MESH_FLAG_80        = 0x80,
} aft_obj_sub_mesh_flags;

typedef enum aft_obj_skin_block_constraint_type {
    AFT_OBJ_SKIN_BLOCK_CONSTRAINT_NONE        = 0x00,
    AFT_OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION = 0x01,
    AFT_OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION   = 0x02,
    AFT_OBJ_SKIN_BLOCK_CONSTRAINT_POSITION    = 0x03,
    AFT_OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE    = 0x04,
} aft_obj_skin_block_constraint_type;

typedef enum aft_obj_database_string_type {
    AFT_OBJ_DATABASE_STRING_OBJECT_FILE_NAME  = 0x00,
    AFT_OBJ_DATABASE_STRING_TEXTURE_FILE_NAME = 0x01,
    AFT_OBJ_DATABASE_STRING_ARCHIVE_FILE_NAME = 0x02,
} aft_obj_database_string_type;

typedef enum aft_obj_material_texture_type {
    AFT_OBJ_MATERIAL_TEXTURE_NONE               = 0x0,
    AFT_OBJ_MATERIAL_TEXTURE_COLOR              = 0x01,
    AFT_OBJ_MATERIAL_TEXTURE_NORMAL             = 0x02,
    AFT_OBJ_MATERIAL_TEXTURE_SPECULAR           = 0x03,
    AFT_OBJ_MATERIAL_TEXTURE_HEIGHT             = 0x04,
    AFT_OBJ_MATERIAL_TEXTURE_REFLECTION         = 0x05,
    AFT_OBJ_MATERIAL_TEXTURE_TRANSLUCENCY       = 0x06,
    AFT_OBJ_MATERIAL_TEXTURE_TRANSPARENCY       = 0x07,
    AFT_OBJ_MATERIAL_TEXTURE_ENVIRONMENT_SPHERE = 0x08,
    AFT_OBJ_MATERIAL_TEXTURE_ENVIRONMENT_CUBE   = 0x09,
} aft_obj_material_texture_type;

typedef enum aft_obj_material_shader_vertex_tranlsation_type {
    AFT_OBJ_MATERIAL_SHADER_VERTEX_TRANLSATION_DEFAULT  = 0x00,
    AFT_OBJ_MATERIAL_SHADER_VERTEX_TRANLSATION_ENVELOPE = 0x01,
    AFT_OBJ_MATERIAL_SHADER_VERTEX_TRANLSATION_MORPHING = 0x02,
} aft_obj_material_shader_vertex_tranlsation_type;

typedef enum aft_obj_material_shader_color_source_type {
    AFT_OBJ_MATERIAL_SHADER_COLOR_SOURCE_MATERIAL_COLOR = 0x00,
    AFT_OBJ_MATERIAL_SHADER_COLOR_SOURCE_VERTEX_COLOR   = 0x01,
    AFT_OBJ_MATERIAL_SHADER_COLOR_SOURCE_VERTEX_MORPH   = 0x02,
} aft_obj_material_shader_color_source_type;

typedef enum aft_obj_material_shader_bump_map_type {
    AFT_OBJ_MATERIAL_SHADER_BUMP_MAP_NONE = 0x00,
    AFT_OBJ_MATERIAL_SHADER_BUMP_MAP_DOT  = 0x01,
    AFT_OBJ_MATERIAL_SHADER_BUMP_MAP_ENV  = 0x02,
} aft_obj_material_shader_bump_map_type;

typedef enum aft_obj_material_shader_specular_quality {
    AFT_OBJ_MATERIAL_SHADER_SPECULAR_QUALITY_LOW  = 0x00,
    AFT_OBJ_MATERIAL_SHADER_SPECULAR_QUALITY_HIGH = 0x01,
} aft_obj_material_shader_specular_quality;

typedef enum aft_obj_material_shader_aniso_direction {
    AFT_OBJ_MATERIAL_SHADER_ANISO_DIRECTION_NORMAL = 0x00,
    AFT_OBJ_MATERIAL_SHADER_ANISO_DIRECTION_U      = 0x01,
    AFT_OBJ_MATERIAL_SHADER_ANISO_DIRECTION_V      = 0x02,
    AFT_OBJ_MATERIAL_SHADER_ANISO_DIRECTION_RADIAL = 0x03,
} aft_obj_material_shader_aniso_direction;

typedef enum aft_obj_material_shader_blend_factor {
    AFT_OBJ_MATERIAL_SHADER_BLEND_FACTOR_ZERO              = 0x0,
    AFT_OBJ_MATERIAL_SHADER_BLEND_FACTOR_ONE               = 0x1,
    AFT_OBJ_MATERIAL_SHADER_BLEND_FACTOR_SRC_COLOR         = 0x2,
    AFT_OBJ_MATERIAL_SHADER_BLEND_FACTOR_INVERSE_SRC_COLOR = 0x3,
    AFT_OBJ_MATERIAL_SHADER_BLEND_FACTOR_SRC_ALPHA         = 0x4,
    AFT_OBJ_MATERIAL_SHADER_BLEND_FACTOR_INVERSE_SRC_ALPHA = 0x5,
    AFT_OBJ_MATERIAL_SHADER_BLEND_FACTOR_DST_ALPHA         = 0x6,
    AFT_OBJ_MATERIAL_SHADER_BLEND_FACTOR_INVERSE_DST_ALPHA = 0x7,
    AFT_OBJ_MATERIAL_SHADER_BLEND_FACTOR_DST_COLOR         = 0x8,
    AFT_OBJ_MATERIAL_SHADER_BLEND_FACTOR_INVERSE_DST_COLOR = 0x9,
} aft_obj_material_shader_blend_factor;

typedef enum aft_obj_material_specular_quality {
    AFT_OBJ_MATERIAL_SPECULAR_QUALITY_LOW  = 0x00,
    AFT_OBJ_MATERIAL_SPECULAR_QUALITY_HIGH = 0x01,
} aft_obj_material_specular_quality;

typedef enum aft_obj_material_vertex_translation_type {
    AFT_OBJ_MATERIAL_VERTEX_TRANSLATION_DEFAULT  = 0x00,
    AFT_OBJ_MATERIAL_VERTEX_TRANSLATION_ENVELOPE = 0x01,
    AFT_OBJ_MATERIAL_VERTEX_TRANSLATION_MORPHING = 0x02,
} aft_obj_material_vertex_translation_type;

typedef enum aft_obj_material_shader_lighting_type {
    AFT_OBJ_MATERIAL_SHADER_LIGHTING_LAMBERT  = 0x00,
    AFT_OBJ_MATERIAL_SHADER_LIGHTING_CONSTANT = 0x01,
    AFT_OBJ_MATERIAL_SHADER_LIGHTING_PHONG    = 0x02,
} aft_obj_material_shader_lighting_type;

typedef enum aft_bone_database_bone_type {
    AFT_BONE_DATABASE_BONE_ROTATION          = 0x00,
    AFT_BONE_DATABASE_BONE_TYPE_1            = 0x01,
    AFT_BONE_DATABASE_BONE_POSITION          = 0x02,
    AFT_BONE_DATABASE_BONE_POSITION_ROTATION = 0x03,
    AFT_BONE_DATABASE_BONE_HEAD_IK_ROTATION  = 0x04,
    AFT_BONE_DATABASE_BONE_ARM_IK_ROTATION   = 0x05,
    AFT_BONE_DATABASE_BONE_LEGS_IK_ROTATION  = 0x06,
    AFT_BONE_DATABASE_BONE_END               = 0xFF,
} aft_bone_database_bone_type;

typedef enum aft_bone_database_skeleton_type {
    AFT_BONE_DATABASE_SKELETON_COMMON = 0,
    AFT_BONE_DATABASE_SKELETON_MIKU   = 1,
    AFT_BONE_DATABASE_SKELETON_KAITO  = 2,
    AFT_BONE_DATABASE_SKELETON_LEN    = 3,
    AFT_BONE_DATABASE_SKELETON_LUKA   = 4,
    AFT_BONE_DATABASE_SKELETON_MEIKO  = 5,
    AFT_BONE_DATABASE_SKELETON_RIN    = 6,
    AFT_BONE_DATABASE_SKELETON_HAKU   = 7,
    AFT_BONE_DATABASE_SKELETON_NERU   = 8,
    AFT_BONE_DATABASE_SKELETON_SAKINE = 9,
    AFT_BONE_DATABASE_SKELETON_TETO   = 10,
    AFT_BONE_DATABASE_SKELETON_NONE   = -1,
} aft_bone_database_skeleton_type;

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

typedef enum draw_task_flags {
    DRAW_TASK_SHADOW          = 0x00000001,
    DRAW_TASK_2               = 0x00000002,
    DRAW_TASK_4               = 0x00000004,
    DRAW_TASK_SELF_SHADOW     = 0x00000008,
    DRAW_TASK_10              = 0x00000010,
    DRAW_TASK_20              = 0x00000020,
    DRAW_TASK_40              = 0x00000040,
    DRAW_TASK_SHADOW_OBJECT   = 0x00000080,
    DRAW_TASK_CHARA_REFLECT   = 0x00000100,
    DRAW_TASK_REFLECT         = 0x00000200,
    DRAW_TASK_REFRACT         = 0x00000400,
    DRAW_TASK_800             = 0x00000800,
    DRAW_TASK_1000            = 0x00001000,
    DRAW_TASK_SSS             = 0x00002000,
    DRAW_TASK_4000            = 0x00004000,
    DRAW_TASK_8000            = 0x00008000,
    DRAW_TASK_10000           = 0x00010000,
    DRAW_TASK_20000           = 0x00020000,
    DRAW_TASK_40000           = 0x00040000,
    DRAW_TASK_80000           = 0x00080000,
    DRAW_TASK_100000          = 0x00100000,
    DRAW_TASK_200000          = 0x00200000,
    DRAW_TASK_400000          = 0x00400000,
    DRAW_TASK_800000          = 0x00800000,
    DRAW_TASK_RIPPLE          = 0x01000000,
    DRAW_TASK_2000000         = 0x02000000,
    DRAW_TASK_4000000         = 0x04000000,
    DRAW_TASK_8000000         = 0x08000000,
    DRAW_TASK_10000000        = 0x10000000,
    DRAW_TASK_20000000        = 0x20000000,
    DRAW_TASK_40000000        = 0x40000000,
    DRAW_TASK_NO_TRANSLUCENCY = 0x80000000,
} draw_task_flags;

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

typedef enum mot_key_set_type {
    MOT_KEY_SET_NONE            = 0x00,
    MOT_KEY_SET_STATIC          = 0x01,
    MOT_KEY_SET_HERMITE         = 0x02,
    MOT_KEY_SET_HERMITE_TANGENT = 0x03,
} mot_key_set_type;

typedef enum mothead_data_type {
    MOTHEAD_DATA_TYPE_0  = 0x00,
    MOTHEAD_DATA_TYPE_1  = 0x01,
    MOTHEAD_DATA_TYPE_2  = 0x02,
    MOTHEAD_DATA_TYPE_3  = 0x03,
    MOTHEAD_DATA_TYPE_4  = 0x04,
    MOTHEAD_DATA_TYPE_5  = 0x05,
    MOTHEAD_DATA_TYPE_6  = 0x06,
    MOTHEAD_DATA_TYPE_7  = 0x07,
    MOTHEAD_DATA_TYPE_8  = 0x08,
    MOTHEAD_DATA_TYPE_9  = 0x09,
    MOTHEAD_DATA_TYPE_10 = 0x0A,
    MOTHEAD_DATA_TYPE_11 = 0x0B,
    MOTHEAD_DATA_TYPE_12 = 0x0C,
    MOTHEAD_DATA_TYPE_13 = 0x0D,
    MOTHEAD_DATA_TYPE_14 = 0x0E,
    MOTHEAD_DATA_TYPE_15 = 0x0F,
    MOTHEAD_DATA_TYPE_16 = 0x10,
    MOTHEAD_DATA_TYPE_17 = 0x11,
    MOTHEAD_DATA_TYPE_18 = 0x12,
    MOTHEAD_DATA_TYPE_19 = 0x13,
    MOTHEAD_DATA_TYPE_20 = 0x14,
    MOTHEAD_DATA_TYPE_21 = 0x15,
    MOTHEAD_DATA_TYPE_22 = 0x16,
    MOTHEAD_DATA_TYPE_23 = 0x17,
    MOTHEAD_DATA_TYPE_24 = 0x18,
    MOTHEAD_DATA_TYPE_25 = 0x19,
    MOTHEAD_DATA_TYPE_26 = 0x1A,
    MOTHEAD_DATA_TYPE_27 = 0x1B,
    MOTHEAD_DATA_TYPE_28 = 0x1C,
    MOTHEAD_DATA_TYPE_29 = 0x1D,
    MOTHEAD_DATA_TYPE_30 = 0x1E,
    MOTHEAD_DATA_TYPE_31 = 0x1F,
    MOTHEAD_DATA_TYPE_32 = 0x20,
    MOTHEAD_DATA_TYPE_33 = 0x21,
    MOTHEAD_DATA_TYPE_34 = 0x22,
    MOTHEAD_DATA_TYPE_35 = 0x23,
    MOTHEAD_DATA_TYPE_36 = 0x24,
    MOTHEAD_DATA_TYPE_37 = 0x25,
    MOTHEAD_DATA_TYPE_38 = 0x26,
    MOTHEAD_DATA_TYPE_39 = 0x27,
    MOTHEAD_DATA_TYPE_40 = 0x28,
    MOTHEAD_DATA_TYPE_41 = 0x29,
    MOTHEAD_DATA_TYPE_42 = 0x2A,
    MOTHEAD_DATA_TYPE_43 = 0x2B,
    MOTHEAD_DATA_TYPE_44 = 0x2C,
    MOTHEAD_DATA_TYPE_45 = 0x2D,
    MOTHEAD_DATA_TYPE_46 = 0x2E,
    MOTHEAD_DATA_TYPE_47 = 0x2F,
    MOTHEAD_DATA_TYPE_48 = 0x30,
    MOTHEAD_DATA_TYPE_49 = 0x31,
    MOTHEAD_DATA_TYPE_50 = 0x32,
    MOTHEAD_DATA_TYPE_51 = 0x33,
    MOTHEAD_DATA_TYPE_52 = 0x34,
    MOTHEAD_DATA_TYPE_53 = 0x35,
    MOTHEAD_DATA_TYPE_54 = 0x36,
    MOTHEAD_DATA_TYPE_55 = 0x37,
    MOTHEAD_DATA_TYPE_56 = 0x38,
    MOTHEAD_DATA_TYPE_57 = 0x39,
    MOTHEAD_DATA_TYPE_58 = 0x3A,
    MOTHEAD_DATA_TYPE_59 = 0x3B,
    MOTHEAD_DATA_TYPE_60 = 0x3C,
    MOTHEAD_DATA_TYPE_61 = 0x3D,
    MOTHEAD_DATA_TYPE_62 = 0x3E,
    MOTHEAD_DATA_TYPE_63 = 0x3F,
    MOTHEAD_DATA_TYPE_64 = 0x40,
    MOTHEAD_DATA_TYPE_65 = 0x41,
    MOTHEAD_DATA_TYPE_66 = 0x42,
    MOTHEAD_DATA_TYPE_67 = 0x43,
    MOTHEAD_DATA_TYPE_68 = 0x44,
    MOTHEAD_DATA_TYPE_69 = 0x45,
    MOTHEAD_DATA_TYPE_70 = 0x46,
    MOTHEAD_DATA_TYPE_71 = 0x47,
    MOTHEAD_DATA_TYPE_72 = 0x48,
    MOTHEAD_DATA_TYPE_73 = 0x49,
    MOTHEAD_DATA_TYPE_74 = 0x4A,
    MOTHEAD_DATA_TYPE_75 = 0x4B,
    MOTHEAD_DATA_TYPE_76 = 0x4C,
    MOTHEAD_DATA_TYPE_77 = 0x4D,
    MOTHEAD_DATA_TYPE_78 = 0x4E,
    MOTHEAD_DATA_TYPE_79 = 0x4F,
    MOTHEAD_DATA_TYPE_80 = 0x50,
} mothead_data_type;

typedef enum motion_blend_type {
    MOTION_BLEND_NONE    = -1,
    MOTION_BLEND         = 0x00,
    MOTION_BLEND_FREEZE  = 0x01,
    MOTION_BLEND_CROSS   = 0x02,
    MOTION_BLEND_COMBINE = 0x03,
} motion_blend_type;

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

typedef enum rob_sub_action_execute_type {
    ROB_SUB_ACTION_EXECUTE_NONE        = 0,
    ROB_SUB_ACTION_EXECUTE_CRY         = 1,
    ROB_SUB_ACTION_EXECUTE_SHAKE_HAND  = 2,
    ROB_SUB_ACTION_EXECUTE_EMBARRASSED = 3,
    ROB_SUB_ACTION_EXECUTE_ANGRY       = 4,
    ROB_SUB_ACTION_EXECUTE_LAUGH       = 5,
    ROB_SUB_ACTION_EXECUTE_COUNT_NUM   = 6,
} rob_sub_action_execute_type;

typedef enum rob_sub_action_param_type {
    ROB_SUB_ACTION_PARAM_NONE        = 0,
    ROB_SUB_ACTION_PARAM_CRY         = 1,
    ROB_SUB_ACTION_PARAM_SHAKE_HAND  = 2,
    ROB_SUB_ACTION_PARAM_EMBARRASSED = 3,
    ROB_SUB_ACTION_PARAM_ANGRY       = 4,
    ROB_SUB_ACTION_PARAM_LAUGH       = 5,
    ROB_SUB_ACTION_PARAM_COUNT_NUM   = 6,
} rob_sub_action_param_type;

typedef enum texture_flags {
    TEXTURE_BLOCK_COMPRESSION = 0x1,
} texture_flags;

#define list(t) \
typedef struct list_##t##_node list_##t##_node; \
\
struct list_##t##_node {  \
    list_##t##_node* next; \
    list_##t##_node* prev; \
    t* value; \
}; \
 \
typedef struct list_##t { \
    list_##t##_node* head; \
    size_t size; \
} list_##t;

#define list_ptr(t) \
typedef struct list_ptr_##t##_node list_ptr_##t##_node; \
\
struct list_ptr_##t##_node {  \
    list_ptr_##t##_node* next; \
    list_ptr_##t##_node* prev; \
    t* value; \
}; \
 \
typedef struct list_ptr_##t { \
    list_ptr_##t##_node* head; \
    size_t size; \
} list_ptr_##t;

#define tree_def(t) \
typedef struct tree_##t##_node tree_##t##_node; \
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
typedef struct tree_##t { \
    tree_##t##_node* head; \
    size_t size; \
} tree_##t;

#define tree_ptr(t) \
typedef struct tree_ptr_##t##_node tree_ptr_##t##_node; \
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
typedef struct tree_ptr_##t { \
    tree_ptr_##t##_node* head; \
    size_t size; \
} tree_ptr_##t;

#define tree_pair(t1, t2) \
typedef struct tree_pair_##t1##_##t2##_node tree_pair_##t1##_##t2##_node; \
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
typedef struct tree_pair_##t1##_##t2 { \
    tree_pair_##t1##_##t2##_node* head; \
    size_t size; \
} tree_pair_##t1##_##t2;

#define tree_pair_ptr(t1, t2) \
typedef struct tree_pair_##t1##_ptr_##t2##_node tree_pair_##t1##_ptr_##t2##_node; \
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
typedef struct tree_pair_##t1##_ptr_##t2 { \
    tree_pair_##t1##_ptr_##t2##_node* head; \
    size_t size; \
} tree_pair_##t1##_ptr_##t2;

typedef struct aft_bone_database_bone {
    uint8_t type;
    bool has_parent;
    uint8_t parent;
    uint8_t pole_target;
    uint8_t mirror;
    uint8_t flags;
    char* name;
} aft_bone_database_bone;

typedef struct aft_bone_database_bone_file {
    uint8_t type;
    bool has_parent;
    uint8_t parent;
    uint8_t pole_target;
    uint8_t mirror;
    uint8_t flags;
    uint32_t name_offset;
} aft_bone_database_bone_file;

typedef struct aft_bone_database_skeleton {
    aft_bone_database_bone* bones;
    int32_t posittion_count;
    int32_t field_C;
    vec3* positions;
    float_t* heel_height;
    uint32_t object_bone_name_count;
    int32_t field_24;
    char** object_bone_names;
    int32_t motion_bone_name_count;
    int32_t field_34;
    char** motion_bone_names;
    uint16_t* parent_indices;
    int32_t field_48;
    int32_t field_4C;
    int32_t field_50;
    int32_t field_54;
    int32_t field_58;
    int32_t field_5C;
} aft_bone_database_skeleton;

typedef struct aft_bone_database_struct {
    uint32_t signature;
    uint32_t skeleton_count;
    aft_bone_database_skeleton** skeletons;
    char** skeleton_names;
} aft_bone_database_struct;

typedef struct rob_chara rob_chara;
typedef struct rob_chara_bone_data rob_chara_bone_data;
typedef struct rob_chara_item_equip rob_chara_item_equip;
typedef struct bone_node bone_node;

typedef struct bone_node_expression_data {
    vec3 position;
    vec3 rotation;
    vec3 scale;
    vec3 parent_scale;
} bone_node_expression_data;

typedef struct bone_node {
    char* name;
    mat4* mat;
    bone_node* parent;
    bone_node_expression_data exp_data;
    mat4* ex_data_mat;
} bone_node;

vector_old(bone_node)

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

vector_old(mot_key_set)

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

typedef struct struc_285 {
    int64_t field_0;
    int64_t field_8;
    int64_t field_10;
    int64_t field_18;
} struc_285;

list(struc_285)

typedef struct struc_289 {
    int32_t field_0;
    int32_t field_4;
    int64_t field_8;
    int64_t field_10;
    int64_t field_18;
} struc_289;

list(struc_289)

typedef struct struc_298 {
    int64_t field_0;
    int64_t field_8;
    int64_t field_10;
} struc_298;

typedef struct struc_293 {
    int32_t key;
    void* value;
} struc_293;

list(struc_293)

tree_pair(int32_t, int32_t)

list_ptr(void)

typedef struct rob_chara_item_sub_data {
    chara_index chara_index;
    int32_t field_4;
    request_data_object_data field_8;
    request_data_object_data field_6C;
    list_struc_285 field_D0;
    list_struc_289 field_E0;
    list_struc_293 field_F0;
    list_ptr_void field_100;
    struc_298 field_110[31];
    tree_pair_int32_t_int32_t field_3F8;
} rob_chara_item_sub_data;

typedef struct struc_525 {
    int32_t field_0;
    int32_t field_4;
} struc_525;

typedef struct struc_524 {
    int32_t field_0;
    struc_525 field_4;
    int32_t field_C;
    uint8_t field_10;
} struc_524;

typedef struct struc_523 {
    uint8_t field_0;
    uint8_t field_1;
    int32_t field_4;
    uint8_t field_8;
    uint8_t field_9;
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
    int64_t field_78;
    int32_t field_80;
} struc_523;

typedef struct struc_526 {
    int32_t field_0;
    int32_t field_4;
} struc_526;

typedef struct struc_264 {
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
    uint8_t eyes_adjust;
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
} struc_264;

typedef struct rob_sub_action rob_sub_action;

typedef struct rob_sub_action_vtbl {
    void(*dispose)(rob_sub_action*, bool);
} rob_sub_action_vtbl;

typedef struct rob_sub_action_execute rob_sub_action_execute;

typedef struct rob_sub_action_execute_vftable {
    void(*dispose)(rob_sub_action_execute*);
    void(*field_8)(rob_sub_action_execute*);
    void(*field_10)(rob_sub_action_execute*);
    void(*field_18)(rob_sub_action_execute*, rob_chara*);
    void(*field_20)(rob_sub_action_execute*, rob_chara*);
} rob_sub_action_execute_vftable;

struct rob_sub_action_execute {
    rob_sub_action_execute_vftable* __vftable;
    rob_sub_action_execute_type type;
    int32_t field_C;
};

typedef struct rob_sub_action_execute_cry {
    rob_sub_action_execute base;
} rob_sub_action_execute_cry;

typedef struct rob_sub_action_execute_shake_hand {
    rob_sub_action_execute base;
    int64_t field_10;
    uint8_t field_18;
} rob_sub_action_execute_shake_hand;

typedef struct rob_sub_action_execute_embarrassed {
    rob_sub_action_execute base;
    uint8_t field_10;
} rob_sub_action_execute_embarrassed;

typedef struct rob_sub_action_execute_angry {
    rob_sub_action_execute base;
} rob_sub_action_execute_angry;

typedef struct rob_sub_action_execute_laugh {
    rob_sub_action_execute base;
} rob_sub_action_execute_laugh;

typedef struct rob_sub_action_execute_count_num {
    rob_sub_action_execute base;
    int64_t field_10;
    int32_t field_18;
} rob_sub_action_execute_count_num;

typedef struct rob_sub_action_param {
    rob_sub_action_param_type type;
} rob_sub_action_param;

typedef struct rob_sub_action_data {
    rob_sub_action_execute* field_0;
    rob_sub_action_execute* field_8;
    rob_sub_action_param* field_10;
    rob_sub_action_execute* field_18;
    rob_sub_action_execute_cry cry;
    rob_sub_action_execute_shake_hand shake_hand;
    rob_sub_action_execute_embarrassed embarrassed;
    rob_sub_action_execute_angry angry;
    rob_sub_action_execute_laugh laugh;
    rob_sub_action_execute_count_num count_num;
} rob_sub_action_data;

struct rob_sub_action {
    rob_sub_action_vtbl* __vftable;
    rob_sub_action_data data;
};

typedef struct struc_389 {
    float_t frame;
    float_t prev_frame;
    float_t last_frame;
} struc_389;

typedef struct struc_406 {
    float_t frame;
    float_t field_4;
    float_t field_8;
} struc_406;

typedef struct rob_partial_motion rob_partial_motion;

typedef struct rob_partial_motion_vtbl {
    void(*dispose)(rob_partial_motion*, bool);
    void(*field_8)(void*);
} rob_partial_motion_vtbl;

struct rob_partial_motion {
    rob_partial_motion_vtbl* __vftable;
    int32_t field_8;
    int32_t field_C;
    int32_t field_10;
    float_t field_14;
    float_t field_18;
    float_t field_1C;
    float_t field_20;
    float_t field_24;
    float_t field_28;
    float_t field_2C;
    struc_389* field_30;
    struc_389* field_38;
    int32_t field_40;
    int32_t field_44;
};

typedef struct rob_face_motion {
    rob_partial_motion base;
} rob_face_motion;

typedef struct rob_hand_motion {
    rob_partial_motion base;
} rob_hand_motion;

typedef struct rob_mouth_motion {
    rob_partial_motion base;
} rob_mouth_motion;

typedef struct rob_eyes_motion {
    rob_partial_motion base;
} rob_eyes_motion;

typedef struct rob_eyelid_motion {
    rob_partial_motion base;
} rob_eyelid_motion;

typedef struct struc_269 {
    uint8_t field_0;
    float_t field_4;
    float_t field_8;
    vec3 field_C;
    float_t force;
    float_t field_1C;
    int32_t field_20;
    int32_t field_24;
    float_t field_28;
    int32_t field_2C;
    int32_t field_30;
    uint8_t field_34;
    vec3 field_38;
    vec3 field_44;
    vec3 field_50;
    float_t field_5C;
    float_t field_60;
    float_t field_64;
    float_t field_68;
    float_t field_6C;
} struc_269;

typedef struct struc_229 {
    uint8_t field_0;
    uint8_t field_1;
    int16_t field_2;
    int16_t field_4;
    float_t field_8;
    float_t field_C;
    float_t field_10;
    float_t field_14;
    float_t field_18;
    float_t field_1C;
    uint8_t field_20;
    uint8_t field_21;
    uint8_t field_22;
    uint8_t field_23;
    vec3 field_24;
    vec3 field_30;
    int32_t field_3C;
    int32_t field_40;
} struc_229;

typedef struct struc_222 {
    uint8_t field_0;
    float_t field_4;
    float_t field_8;
    float_t field_C;
    float_t field_10;
    float_t field_14;
} struc_222;

typedef struct aft_object_info {
    uint16_t id;
    uint16_t set_id;
} aft_object_info;

typedef struct struc_405 {
    rob_face_motion field_0;
    rob_hand_motion field_48;
    rob_hand_motion field_90;
    rob_mouth_motion field_D8;
    rob_eyes_motion field_120;
    rob_eyelid_motion field_168;
    aft_object_info field_1B0;
    aft_object_info field_1B4;
    aft_object_info field_1B8;
    aft_object_info field_1BC;
    int32_t field_1C0;
    float_t field_1C4;
} struc_405;

typedef struct struc_268 {
    int32_t motion_id;
    int32_t prev_motion_id;
    struc_389 field_8;
    struc_406 field_14;
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
    rob_hand_motion field_318;
    rob_hand_motion field_360;
    aft_object_info field_3A8;
    aft_object_info field_3AC;
    struc_405 field_3B0;
    struc_269 field_578[13];
    struc_269 field_B28[13];
    struc_269 field_10D8[2];
    struc_229 field_11B8[2];
    struc_229 field_1240[2];
    struc_222 field_12C8[2];
} struc_268;

typedef struct struc_228 {
    int32_t field_0;
    int32_t field_4;
    uint32_t field_8;
    int32_t field_C;
} struc_228;

typedef struct struc_227 {
    int32_t field_0;
    float_t field_4;
    float_t field_8;
} struc_227;

typedef struct mothead_data {
    mothead_data_type type;
    int32_t frame;
    void* data;
} mothead_data;

typedef struct struc_377 {
    mothead_data* field_0;
    mothead_data* field_8;
} struc_377;

typedef struct struc_226 {
    uint8_t field_0[27];
} struc_226;

typedef struct struc_225 {
    float_t field_0[27];
} struc_225;

typedef struct struc_224 {
    int32_t field_0[27];
} struc_224;

typedef struct struc_306 {
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
} struc_306;

typedef struct struc_223 {
    int32_t field_0;
    float_t field_4;
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
    uint8_t field_84;
    int32_t field_88;
    int32_t field_8C;
    int32_t field_90;
    int16_t field_94;
    int32_t field_98;
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
    int32_t field_23C;
    int32_t field_240;
    int16_t field_244;
    int64_t field_248;
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
    uint8_t field_2A0;
    float_t field_2A4;
    float_t coli;
    float_t field_2AC;
    int64_t field_2B0;
    int16_t rotation_y;
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
    uint8_t field_300;
    int32_t field_304;
    int32_t field_308;
    float_t field_30C;
    int32_t field_310;
    int16_t field_314;
    float_t field_318;
    float_t field_31C;
    float_t field_320;
    float_t field_324;
    float_t field_328;
    int32_t field_32C;
    struc_377 field_330;
    int32_t field_340;
    int32_t field_344;
    int32_t field_348;
    float_t field_34C;
    vec3 field_350;
    struc_226 field_35C[3];
    struc_225 field_3B0[3];
    struc_224 field_4F4[3];
    int64_t field_638;
    float_t field_640;
    float_t field_644;
    uint8_t field_648;
    int32_t field_64C;
    float_t field_650;
    float_t field_654;
    float_t field_658;
    int32_t field_65C;
    float_t field_660;
    float_t field_664;
    uint8_t field_668;
    struc_306 field_66C[4];
    int64_t* field_7A0;
    int32_t field_7A8;
} struc_223;

typedef struct struc_399 {
    int16_t field_0;
    int16_t field_2;
    int16_t field_4;
    int16_t field_6;
    int32_t field_8;
    int32_t field_C;
    int32_t field_10;
    int32_t field_14;
    vec3 field_18;
    vec3 field_24;
    int32_t field_30;
    int32_t field_34;
    int32_t field_38;
    int32_t field_3C;
    int32_t field_40;
    int32_t field_44;
    vec3 field_48;
    vec3 field_54;
    int32_t field_60;
    int32_t field_64;
    int32_t field_68;
    mat4u field_6C;
} struc_399;

typedef struct rob_chara_adjust_data {
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
} rob_chara_adjust_data;

typedef struct struc_195 {
    vec3 rotation;
    vec3 trans;
    float_t scale;
    float_t field_1C;
    float_t field_20;
    float_t field_24;
} struc_195;

typedef struct struc_210 {
    vec2 field_0;
    float_t scale;
} struc_210;

typedef struct struc_215 {
    int64_t field_0;
    float_t field_8;
} struc_215;

vector_old(struc_215)

typedef struct struc_267 {
    float_t field_0;
    int16_t field_4;
    float_t field_8;
    int16_t field_C;
} struc_267;

typedef struct struc_209 {
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
    int32_t field_1EF4;
    int32_t field_1EF8;
    int32_t field_1EFC;
    int32_t field_1F00;
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
} struc_209;

typedef struct rob_chara_data {
    uint8_t field_0;
    uint8_t field_1;
    uint8_t field_2;
    uint8_t field_3;
    int32_t field_4;
    struc_264 field_8;
    rob_sub_action rob_sub_action;
    struc_268 field_290;
    struc_223 field_1588;
    struc_399 field_1D38;
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
} rob_chara_data;

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

typedef struct eyes_adjust {
    bool xrot_adjust;
    eyes_base_adjust_type base_adjust;
    float_t neg;
    float_t pos;
} eyes_adjust;

typedef struct rob_chara_pv_data {
    int32_t index;
    uint8_t field_4;
    uint8_t field_5;
    uint8_t field_6;
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
    int32_t chara_size_index;
    bool height_adjust;
    int32_t item_no[4];
    eyes_adjust eyes_adjust;
} rob_chara_pv_data;

typedef struct touch_area touch_area;

typedef struct touch_area_vftable {
    void(*dispose)(touch_area*);
    void(*field_8)(touch_area*);
    void(*field_10)(touch_area*);
    void(*field_18)(touch_area*);
    void(*field_20)(touch_area*);
    void(*field_28)(touch_area*);
    void(*field_30)(touch_area*);
} touch_area_vftable;

struct touch_area {
    touch_area_vftable* vftable;
    int32_t field_8;
    int32_t field_C;
    float_t field_10;
    float_t field_14;
    float_t field_18;
    float_t field_1C;
    float_t field_20;
    int32_t field_24;
};

typedef struct rob_touch_area {
    touch_area area;
    int64_t field_28;
    uint8_t field_30[3];
    int16_t field_34;
} rob_touch_area;

vector_old(rob_touch_area)

typedef struct rob_touch {
    void** dispose;
    uint8_t field_8;
    vector_old_rob_touch_area area;
} rob_touch;

typedef struct struc_307 {
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
} struc_307;

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
    int32_t field_0;
    int32_t object_set;
    aft_bone_database_skeleton_type skeleton_type;
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
    int32_t eyes_adjust;
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
    int32_t coli;
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
    int32_t field_7E0;
    int16_t base_face_object_id;
    int16_t base_face_object_set;
    int32_t field_7E8;
    int32_t field_7EC;
    int32_t field_7F0;
    int32_t field_7F4;
    int32_t field_7F8;
    int32_t field_7FC;
    int32_t field_800;
    int32_t field_804;
    int32_t field_808;
    int32_t field_80C;
    int32_t field_810;
    int32_t field_814;
    int32_t field_818;
    int32_t motion_set;
    int32_t field_820;
    int32_t field_824;
    struc_218* field_828;
    struc_218* field_830;
    uint8_t*** field_838;
    int32_t field_840;
    int32_t field_844;
    struc_344* field_848;
    aft_object_info* head_objects;
    void* field_858;
    void* field_860;
    aft_object_info* face_objects;
} chara_init_data;

typedef struct rob_detail {
    rob_chara* field_8;
    rob_chara_data* field_10;
} rob_detail;

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
    rob_chara_item_sub_data item_sub_data;
    rob_chara_data data;
    rob_chara_data data_prev;
    chara_init_data* chara_init_data;
    rob_detail* rob_detail;
    rob_chara_pv_data pv_data;
    int32_t field_80E4;
    rob_touch rob_touch;
};

typedef struct struc_313 {
    vector_old_uint32_t bitfield;
    size_t motion_bone_count;
} struc_313;

typedef struct mot {
    uint16_t key_set_count;
    uint16_t frame_count;
    uint16_t field_4;
    uint16_t field_6;
    mot_key_set* key_sets;
} mot;

typedef struct struc_369 {
    int32_t field_0;
    float_t field_4;
} struc_369;

typedef struct mot_key_set_file {
    uint16_t info;
    uint16_t frame_count;
} mot_key_set_file;

typedef struct mot_data {
    mot_key_set_file* data_file;
    uint16_t* key_set_types;
    void* key_sets;
    uint16_t* bone_info;
} mot_data;

typedef struct mot_parent {
    bool key_sets_ready;
    size_t key_set_count;
    vector_old_mot_key_set key_set;
    mot mot;
    vector_old_float_t key_set_data;
    mot_data* mot_data;
    aft_bone_database_skeleton_type skeleton_type;
    int32_t skeleton_select;
    int32_t motion_id;
    float_t frame;
    struc_369 field_68;
} mot_parent;

typedef struct bone_data {
    aft_bone_database_bone_type type;
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

vector_old(bone_data)

typedef struct motion_blend motion_blend;

typedef struct motion_blend_vtbl {
    void(*dispose)(motion_blend*);
    void(*field_8)(motion_blend*);
    void(*field_10)(motion_blend*);
    void(*field_18)(motion_blend*, uint8_t*);
    void(*field_20)(motion_blend*, vector_old_bone_data*, vector_old_bone_data*);
    void(*field_28)(motion_blend*, bone_data*, bone_data*);
    bool(*field_30)(motion_blend*);
} motion_blend_vtbl;

struct motion_blend {
    motion_blend_vtbl* __vftable;
    bool field_8;
    bool field_9;
    float_t field_C;
    float_t field_10;
    float_t field_14;
    float_t field_18;
    float_t blend;
};

typedef struct motion_blend_cross {
    motion_blend base;
    bool field_20;
    bool field_21;
    mat4u field_24;
    mat4u field_64;
    mat4u field_A4;
    mat4u field_E4;
} motion_blend_cross;

typedef struct motion_blend_freeze {
    motion_blend base;
    uint8_t field_20;
    uint8_t field_21;
    int32_t field_24;
    float_t field_28;
    float_t field_2C;
    int32_t field_30;
    mat4u field_34;
    mat4u field_74;
    mat4u eyes_adjust;
    mat4u field_F4;
} motion_blend_freeze;

typedef struct motion_blend_combine {
    motion_blend_cross base;
} motion_blend_combine;

typedef struct struc_400 {
    uint8_t field_0;
    char field_1;
    char field_2;
    char field_3;
    char field_4;
    float_t field_8;
    float_t field_C;
    float_t field_10;
} struc_400;

typedef struct struc_240 {
    bool(*bone_check_func)(motion_bone_index bone_index);
    struc_313 field_8;
    size_t motion_bone_count;
} struc_240;

typedef struct struc_346 {
    float_t frame;
    float_t field_4;
    float_t field_8;
    float_t field_C;
    float_t field_10;
    float_t field_14;
    int32_t field_18;
    int32_t field_1C;
    float_t field_20;
    float_t field_24;
    uint8_t field_28;
    int32_t field_2C;
} struc_346;

typedef struct mot_blend {
    struc_240 field_0;
    uint8_t field_30;
    mot_parent field_38;
    struc_346 field_A8;
    int32_t field_D8;
    uint8_t field_DC;
    int32_t field_E0;
    int32_t field_E4;
    int64_t field_E8;
    int64_t field_F0;
    motion_blend field_F8;
} mot_blend;

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

typedef struct struc_243 {
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
} struc_243;

typedef struct struc_258 {
    vector_old_bone_data* field_0;
    mat4u field_8;
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
    int32_t eyes_adjust;
    int32_t field_B8;
    int32_t field_BC;
    vec3 field_C0;
    mat4u field_CC;
    mat4u field_10C;
    uint8_t field_14C;
    float_t field_150;
    float_t field_154;
    float_t field_158;
    struc_243 field_15C;
    int32_t field_184;
    int32_t field_188;
    int32_t field_18C;
    int32_t field_190;
    uint8_t field_194;
    uint8_t field_195;
    int32_t field_198;
    int32_t field_19C;
    int32_t field_1A0;
    int32_t field_1A4;
    int32_t field_1A8;
    int32_t field_1AC;
    int32_t field_1B0;
    int32_t field_1B4;
    int32_t field_1B8;
    vec2 field_1BC;
    uint8_t field_1C4;
    int32_t field_1C8;
    int32_t field_1CC;
} struc_258;

typedef struct struc_312 {
    struc_242 field_0;
    struc_242 field_2C;
    uint8_t field_58;
    uint8_t field_59;
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
    float_t field_8C;
    int32_t field_90;
    int32_t field_94;
    float_t field_98;
    int32_t field_9C;
} struc_312;

typedef struct bone_data_parent {
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
    float_t rotation_y;
} bone_data_parent;

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
    struc_240 field_0;
    motion_blend_cross cross;
    motion_blend_freeze freeze;
    motion_blend_combine combine;
    bone_data_parent bone_data;
    mot_parent mot_data;
    struc_370 field_4A8;
    struc_308 field_4F8;
    int32_t field_5CC;
    motion_blend* field_5D0;
} motion_blend_mot;

vector_old_ptr(motion_blend_mot)
list_ptr(motion_blend_mot)
list(size_t)

typedef struct rob_chara_bone_data_ik_scale {
    float_t ratio0;
    float_t ratio1;
    float_t ratio2;
    float_t ratio3;
} rob_chara_bone_data_ik_scale;

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
    aft_bone_database_skeleton_type base_skeleton_type;
    aft_bone_database_skeleton_type skeleton_type;
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
    uint8_t field_758;
    rob_chara_bone_data_ik_scale ik_scale;
    vec3 field_76C;
    vec3 field_778;
    int32_t field_784;
    struc_258 field_788;
    struc_312 field_958;
};

typedef struct struc_373 {
    int32_t motion_id;
    float_t frame;
    float_t frame_count;
    uint8_t field_C;
    int32_t field_10;
    float_t blend;
} struc_373;

vector_old(struc_373)

typedef struct aft_obj_skin_block_node {
    char* parent_name;
    vec3 position;
    vec3 rotation;
    vec3 scale;
} aft_obj_skin_block_node;

typedef struct aft_obj_skin_block_osage_node {
    uint32_t name_index;
    float_t length;
    vec3 rotation;
} aft_obj_skin_block_osage_node;

typedef struct aft_obj_skin_block_osage {
    aft_obj_skin_block_node node;
    int32_t start_index;
    int32_t count;
    int32_t external_name_index;
    int32_t name_index;
    aft_obj_skin_block_osage_node* nodes;
    int32_t field_48;
    int32_t field_4C;
    int32_t field_50;
    int32_t field_54;
} aft_obj_skin_block_osage;

typedef struct aft_obj_skin_block_expression {
    aft_obj_skin_block_node node;
    char* name;
    uint32_t expression_count;
    char* expressions[9];
} aft_obj_skin_block_expression;

typedef struct struc_231 {
    uint8_t* bone_name;
    float_t weight;
    uint32_t matrix_index;
    uint32_t field_10;
    int32_t field_14;
} struc_231;

typedef struct struc_230 {
    vec3 field_0;
    vec3 field_C;
    float_t field_18;
    int32_t field_1C;
    int32_t field_20;
    int32_t field_24;
    struc_231 field_28[4];
} struc_230;

typedef struct struc_251 {
    uint32_t field_0;
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
} struc_251;

typedef struct aft_obj_skin_block_cloth {
    char* mesh_name;
    char* backface_mesh_name;
    uint32_t field_8;
    uint32_t count;
    uint32_t field_10;
    int32_t field_14;
    mat4u* field_18;
    struc_230* field_1C;
    struc_251* field_20;
    uint16_t* field_24;
    uint16_t* field_28;
    int64_t field_48;
    uint32_t field_30;
    uint32_t field_54;
} aft_obj_skin_block_cloth;

typedef struct aft_obj_skin_block_constraint_orientation {
    vec3 offset;
} aft_obj_skin_block_constraint_orientation;

typedef struct aft_obj_skin_block_constraint_up_vector_old {
    bool active;
    float_t roll;
    vec3 affected_axis;
    vec3 point_at;
    char* name;
} aft_obj_skin_block_constraint_up_vector_old;

typedef struct aft_obj_skin_block_constraint_direction {
    aft_obj_skin_block_constraint_up_vector_old up_vector_old;
    vec3 align_axis;
    vec3 target_offset;
} aft_obj_skin_block_constraint_direction;

typedef struct aft_obj_skin_block_constraint_attach_point {
    int32_t affected_by_orientation;
    int32_t affected_by_scaling;
    vec3 offset;
} aft_obj_skin_block_constraint_attach_point;

typedef struct aft_obj_skin_block_constraint_position {
    aft_obj_skin_block_constraint_up_vector_old up_vector_old;
    aft_obj_skin_block_constraint_attach_point constrained_object;
    aft_obj_skin_block_constraint_attach_point constraining_object;
} aft_obj_skin_block_constraint_position;

typedef struct aft_obj_skin_block_constraint_distance {
    aft_obj_skin_block_constraint_up_vector_old up_vector_old;
    float_t distance;
    aft_obj_skin_block_constraint_attach_point constrained_object;
    aft_obj_skin_block_constraint_attach_point constraining_object;
} aft_obj_skin_block_constraint_distance;

typedef struct aft_obj_skin_block_constraint {
    aft_obj_skin_block_node node;
    char* type;
    char* name;
    int32_t coupling;
    char* source_node_name;
    union {
        aft_obj_skin_block_constraint_orientation orientation;
        aft_obj_skin_block_constraint_direction direction;
        aft_obj_skin_block_constraint_position position;
        aft_obj_skin_block_constraint_distance distance;
    };
} aft_obj_skin_block_constraint;

typedef struct aft_obj_skin_block {
    char* signature;
    union {
        aft_obj_skin_block_node node;
        aft_obj_skin_block_osage osage;
        aft_obj_skin_block_expression expression;
        aft_obj_skin_block_cloth cloth;
        aft_obj_skin_block_constraint constraint;
    };
} aft_obj_skin_block;

typedef struct rob_chara_item_equip_object rob_chara_item_equip_object;
typedef struct ex_node_block ex_node_block;

typedef struct ex_node_block_vtbl {
    void* (*dispose)(ex_node_block*, bool dispose);
    void(*field_8)(ex_node_block*);
    void(*field_10)(ex_node_block*);
    void(*field_18)(ex_node_block*, int64_t, int64_t);
    void(*update)(ex_node_block*);
    void(*field_28)(ex_node_block*);
    void(*draw)(ex_node_block*);
    void(*reset)(ex_node_block*);
    void(*field_40)(ex_node_block*);
    void(*field_48)(ex_node_block*);
    void(*field_50)(ex_node_block*);
    void(*field_58)(ex_node_block*);
} ex_node_block_vtbl;

struct ex_node_block {
    ex_node_block_vtbl* __vftable;
    bone_node* bone_node_ptr;
    ex_node_type type;
    char* name;
    bone_node* parent_bone_node;
    string parent_name;
    ex_node_block* parent_node;
    rob_chara_item_equip_object* item_equip_object;
    bool field_58;
    bool field_59;
    bool field_5A;
};

typedef struct ex_null_block {
    ex_node_block base;
    aft_obj_skin_block_node* node_data;
} ex_null_block;

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

vector_old_ptr(rob_osage_node)

typedef struct rob_osage_node_data {
    float_t force;
    vector_old_ptr_rob_osage_node boc;
    rob_osage_node_data_normal_ref normal_ref;
    skin_param_osage_node skp_osg_node;
} rob_osage_node_data;

typedef struct struc_331 {
    int64_t field_0;
    int64_t field_8;
    int64_t field_10;
} struc_331;

vector_old(struc_331)

typedef struct struc_477 {
    float_t length;
    vec3 angle;
} struc_477;

typedef struct struc_476 {
    struc_477 field_0;
    struc_477 field_10;
} struc_476;

typedef struct struc_479 {
    vec3 trans;
    vec3 trans_diff;
    vec3 rotation;
    float_t field_24;
} struc_479;

struct rob_osage_node {
    float_t length;
    vec3 trans;
    vec3 trans_orig;
    vec3 trans_diff;
    vec3 field_28;
    float_t child_length;
    bone_node* bone_node;
    mat4* bone_node_mat;
    mat4u mat;
    rob_osage_node* sibling_node;
    float_t distance;
    vec3 field_94;
    struc_479 field_A0;
    float_t field_C8;
    float_t field_CC;
    vec3 field_D0;
    float_t force;
    rob_osage_node_data* data_ptr;
    rob_osage_node_data data;
    vector_old_struc_331 field_198;
    struc_476 field_1B0;
};

vector_old(rob_osage_node)

typedef struct skin_param_osage_root_coli {
    int32_t type;
    int32_t bone0_index;
    int32_t bone1_index;
    float_t radius;
    vec3 bone0_pos;
    vec3 bone1_pos;
} skin_param_osage_root_coli;

vector_old(skin_param_osage_root_coli)

typedef struct skin_param {
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

vector_old(osage_coli)

typedef struct osage_ring_data {
    float_t ring_rectangle_x;
    float_t ring_rectangle_y;
    float_t ring_rectangle_width;
    float_t ring_rectangle_height;
    float_t ring_height;
    float_t ring_out_height;
    bool field_18;
    vector_old_osage_coli coli;
    vector_old_skin_param_osage_root_coli skp_root_coli;
} osage_ring_data;

typedef struct struc_478 {
    int32_t field_0;
    int32_t field_4;
    int64_t field_8;
    int64_t field_10;
} struc_478;

tree_def(struc_478)

typedef struct list_struc_479_node list_struc_479_node;

struct list_struc_479_node {
    list_struc_479_node* next;
    list_struc_479_node* prev;
    struc_479 value;
};

typedef struct list_struc_479 {
    list_struc_479_node* head;
    size_t size;
} list_struc_479;

typedef struct rob_osage {
    skin_param* skin_param_ptr;
    bone_node_expression_data exp_data;
    vector_old_rob_osage_node nodes;
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
    mat4* parent_mat_ptr;
    mat4u parent_mat;
    float_t move_cancel;
    bool field_1F0C;
    bool wind_reset;
    bool field_1F0E;
    bool field_1F0F;
    osage_ring_data ring;
    tree_struc_478 field_1F60;
    list_struc_479* field_1F70;
    bool field_1F78;
    vec3 field_1F7C;
} rob_osage;

typedef struct ex_osage_block {
    ex_node_block base;
    size_t index;
    rob_osage rob;
    mat4* mat;
    int32_t field_1FF8;
    float_t step;
} ex_osage_block;

typedef struct ex_constraint_block {
    ex_node_block base;
    aft_obj_skin_block_constraint_type type;
    bone_node* source_node_bone_node;
    bone_node* direction_up_vector_old_bone_node;
    aft_obj_skin_block_constraint* cns_data;
    int64_t field_80;
} ex_constraint_block;

typedef struct struc_323 {
    int32_t field_0;
    int64_t field_8;
    int64_t field_10;
    int64_t field_18;
    int64_t field_20;
} struc_323;

typedef struct ex_expression_block_stack ex_expression_block_stack;

typedef struct ex_expression_block_stack_number {
    float_t value;
}ex_expression_block_stack_number;

typedef struct ex_expression_block_stack_variable {
    float_t* value;
}ex_expression_block_stack_variable;

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
    union {
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
    aft_obj_skin_block_expression* exp_data;
    bool field_3D20;
    void(*field_3D28)(bone_node_expression_data*);
    float_t frame;
    bool init;
} ex_expression_block;

typedef struct cloth cloth;

typedef struct cloth_vftable {
    void(*dispose)(void*);
    void(*field_8)(void*);
    void(*field_10)(void*);
    void(*field_18)(void*);
    void(*field_20)(void*);
    void(*field_28)(void*, int64_t);
    void(*field_30)(void*);
    void(*field_38)(void*);
    void(*field_40)(void*);
    void(*field_48)(void*);
    void(*field_50)(void*);
    void(*field_58)(void*);
} cloth_vftable;

typedef struct struc_342 {
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
} struc_342;

typedef struct struc_341 {
    int64_t field_0;
    int64_t field_8;
    int64_t field_10;
} struc_341;

vector_old(struc_341)

struct cloth {
    cloth_vftable* vftable;
    int32_t field_8;
    int64_t field_10;
    int64_t field_18;
    struc_342* field_20;
    int64_t field_28;
    int64_t field_30;
    vec3 field_38;
    float_t field_44;
    uint8_t field_48;
    vec3 field_4C;
    vector_old_struc_341 field_58;
    skin_param* field_70;
    skin_param field_78;
    osage_coli coli[64];
    osage_coli coli_ring[64];
    vec3 field_1CE8;
    vec3 field_1CF4;
    bool field_1D00;
    int64_t field_1D08;
    int64_t field_1D10;
    int64_t field_1D18;
    int64_t field_1D20;
    int64_t field_1D28;
    int64_t field_1D30;
    mat4u* field_1D38;
};

typedef struct aft_obj_bounding_sphere {
    vec3 center;
    float_t radius;
} aft_obj_bounding_sphere;

typedef struct aft_obj_axis_aligned_bounding_box {
    vec3 center;
    vec3 size;
} aft_obj_axis_aligned_bounding_box;

typedef struct aft_obj_sub_mesh {
    uint32_t field_0;
    aft_obj_bounding_sphere bounding_sphere;
    uint32_t material_index;
    uint8_t texcoord_indices[8];
    uint32_t bone_indices_count;
    uint16_t* bone_indices;
    uint32_t bones_per_vertex;
    aft_obj_primitive_type primitive_type;
    aft_obj_index_format index_format;
    uint32_t indices_count;
    uint16_t* indices;
    aft_obj_sub_mesh_flags flags;
    uint32_t field_4C;
    int32_t field_50;
    int32_t field_54;
    int32_t field_58;
    int32_t field_5C;
    aft_obj_axis_aligned_bounding_box* axis_aligned_bounding_box;
    uint16_t first_index;
    uint16_t last_index;
    uint32_t indices_offset;
} aft_obj_sub_mesh;

typedef union aft_obj_mesh_attrib_offsets {
    struct {
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
    void* ptr[20];
} aft_obj_mesh_attrib_offsets;

typedef struct aft_obj_mesh {
    uint32_t field_0;
    aft_obj_bounding_sphere bounding_sphere;
    uint32_t sub_meshes_count;
    aft_obj_sub_mesh* sub_meshes;
    aft_obj_vertex_attrib_type attrib_type;
    uint32_t vertex_size;
    uint32_t vertex_count;
    aft_obj_mesh_attrib_offsets attrib_offsets;
    aft_obj_mesh_flags flags;
    uint32_t attrib_flags;
    uint32_t field_D8;
    uint32_t field_DC;
    uint32_t field_E0;
    uint32_t field_E4;
    uint32_t field_E8;
    uint32_t field_EC;
    char name[64];
} aft_obj_mesh;

typedef union aft_obj_material_shader {
    char name[8];
    aft_shader_enum index;
} aft_obj_material_shader;

typedef struct aft_obj_material_shader_flags {
    uint32_t vertex_translation_type : 2;
    uint32_t color_source_type : 2;
    uint32_t lambert_shading : 1;
    uint32_t phong_shading : 1;
    uint32_t per_pixel_shading : 1;
    uint32_t double_shading : 1;
    uint32_t bump_map_type : 2;
    uint32_t fresnel : 4;
    uint32_t line_light : 4;
    uint32_t recieve_shadow : 1;
    uint32_t cast_shadow : 1;
    uint32_t specular_quality : 1;
    uint32_t aniso_direction : 3;
} aft_obj_material_shader_flags;

typedef struct aft_obj_material_texture {
    aft_obj_material_texture_sampler_flags sampler_flags;
    uint32_t texture_id;
    int32_t texture_flags;
    char shader_name[8];
    float_t weight;
    mat4u tex_coord_mat;
    float_t unk[8];
} aft_obj_material_texture;

typedef struct aft_obj_material {
    aft_obj_material_flags material_flags;
    aft_obj_material_shader shader;
    union {
        uint32_t shader_flags;
        aft_obj_material_shader_flags shader_flags_data;
    };
    aft_obj_material_texture textures[8];
    int32_t blend_flags;
    vec4u diffuse;
    vec4u ambient;
    vec4u specular;
    vec4u emission;
    float_t shininess;
    float_t intensity;
    aft_obj_bounding_sphere reserved_sphere;
    char name[64];
    float_t bump_depth;
    float_t unk[15];
} aft_obj_material;

typedef struct aft_obj_material_data {
    int32_t texture_count;
    aft_obj_material material;
} aft_obj_material_data;

typedef struct aft_obj {
    uint32_t field_0;
    uint32_t field_4;
    aft_obj_bounding_sphere bounding_sphere;
    uint32_t meshes_count;
    aft_obj_mesh* meshes;
    uint32_t materials_count;
    aft_obj_material_data* materials;
    uint32_t field_38;
    uint32_t field_3C;
    uint32_t field_40;
    uint32_t field_44;
    uint32_t field_48;
    uint32_t field_4C;
    uint32_t field_50;
    uint32_t field_54;
    uint32_t field_58;
    uint32_t field_5C;
} aft_obj;

typedef struct aft_obj_mesh_vertex_buffer
{
    int32_t count;
    GLuint buffers[3];
    int32_t index;
    GLenum enum_data;
} aft_obj_mesh_vertex_buffer;

typedef struct aft_obj_vertex_buffer {
    int32_t meshes_count;
    aft_obj_mesh_vertex_buffer* meshes;
} aft_obj_vertex_buffer;

typedef GLuint aft_obj_mesh_index_buffer;

typedef struct aft_obj_index_buffer {
    int32_t meshes_count;
    aft_obj_mesh_index_buffer* meshes;
} aft_obj_index_buffer;

typedef struct texture_pattern_struct {
    int32_t src;
    int32_t dst;
} texture_pattern_struct;

vector_old(texture_pattern_struct)

typedef struct aft_obj_skin_osage_node {
    int32_t name_index;
    float_t length;
    int32_t index;
} aft_obj_skin_osage_node;

typedef struct aft_obj_skin_ex_data {
    int32_t osage_name_count;
    int32_t osage_node_count;
    char** field_8;
    aft_obj_skin_osage_node* osage_nodes;
    char** osage_names;
    aft_obj_skin_block* blocks;
    int32_t field_28;
    int32_t field_2C;
    char** strings;
    int64_t osage_sibling_infos;
    int32_t field_40;
    int32_t field_44;
    int32_t field_48;
    int32_t field_4C;
    int32_t field_50;
    int32_t field_54;
    int32_t field_58;
    int32_t field_5C;
} aft_obj_skin_ex_data;

typedef struct aft_obj_skin {
    uint32_t* bone_ids;
    mat4u* bone_matrices;
    char** bone_names;
    aft_obj_skin_ex_data* ex_data;
    uint32_t bone_count;
    int32_t bone_parent_ids_offset;
    int32_t field_28;
    int32_t field_2C;
    int32_t field_30;
} aft_obj_skin;

typedef struct aft_obj_set_info {
    int32_t id;
    string name;
    string object_file_name;
    string texture_file_name;
    string archive_file_name;
} aft_obj_set_info;

typedef struct pair_uint32_t_obj_set_info {
    uint32_t id;
    aft_obj_set_info data;
} pair_uint32_t_obj_set_info;

vector_old(pair_uint32_t_obj_set_info)

typedef struct aft_obj_database {
    vector_old_pair_uint32_t_obj_set_info field_0;
    size_t field_18;
    vector_old_pair_uint32_t_obj_set_info field_20;
    bool field_38;
} aft_obj_database;

typedef struct aft_obj_set {
    uint32_t signature;
    uint32_t objects_count;
    uint32_t max_object_id;
    aft_obj** objects;
    aft_obj_skin** object_skins;
    char** object_names;
    uint32_t* object_ids;
    uint32_t* texture_ids;
    uint32_t texture_ids_count;
    uint32_t field_3C;
    uint32_t field_40;
} aft_obj_set;

typedef struct aft_obj_bounding_box {
    vec3 center;
    vec3 size;
} aft_obj_bounding_box;

typedef struct pair_int32_t_int32_t {
    int32_t key;
    int32_t value;
} pair_int32_t_int32_t;

typedef struct texture {
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
} texture;

typedef struct struc_332 {
    aft_obj_sub_mesh field_0;
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
    int64_t field_F0;
    int64_t field_F8;
    int64_t field_100;
    int64_t field_108;
    int64_t field_110;
    int64_t field_118;
    int64_t field_120;
    int64_t field_128;
    int64_t field_130;
    int64_t field_138;
    int64_t field_140;
    int64_t field_148;
    int64_t field_150;
    int64_t field_158;
    int64_t field_160;
    int64_t field_168;
    int64_t field_170;
    int64_t field_178;
    int64_t field_180;
    int64_t field_188;
    int64_t field_190;
    int64_t field_198;
    int64_t field_1A0;
    int64_t field_1A8;
    int64_t field_1B0;
    int64_t field_1B8;
} struc_332;

typedef struct struc_295 {
    int32_t field_0;
    int32_t field_4;
    int64_t field_8;
    int64_t field_10;
} struc_295;

vector_old(struc_295)

typedef struct rob_cloth {
    cloth base;
    int64_t field_1D40;
    int64_t field_1D48;
    int64_t field_1D50;
    rob_chara_item_equip_object* field_1D58;
    struc_230* field_1D60;
    aft_obj_skin_block_cloth* cls_data;
    float_t move_cancel;
    uint8_t field_1D74;
    aft_obj_mesh field_1D78[2];
    struc_332 field_1FD8[2];
    aft_obj_axis_aligned_bounding_box field_2358;
    aft_obj_mesh_vertex_buffer field_2370[2];
    GLuint mesh_buffer;
    int32_t field_23A4;
    vector_old_struc_295 field_23A8;
    int64_t field_23B8;
} rob_cloth;

typedef struct ex_cloth_block {
    ex_node_block base;
    rob_cloth rob;
    aft_obj_skin_block_cloth* cls_data;
    mat4* field_2428;
    int64_t field_2430;
} ex_cloth_block;

vector_old_ptr(ex_node_block)
vector_old_ptr(ex_null_block)
vector_old_ptr(ex_osage_block)
vector_old_ptr(ex_constraint_block)
vector_old_ptr(ex_expression_block)
vector_old_ptr(ex_cloth_block)

typedef struct ex_data_name_bone_index {
    char* name;
    int32_t bone_index;
} ex_data_name_bone_index;

vector_old(ex_data_name_bone_index)

typedef struct texture_data_struct {
    int32_t field_0;
    vec3 field_4;
    vec3 field_10;
    vec3 field_1C;
    vec3 field_28;
} texture_data_struct;

struct rob_chara_item_equip_object {
    size_t index;
    mat4* mat;
    aft_object_info object_info;
    int32_t field_14;
    vector_old_texture_pattern_struct field_18;
    texture_data_struct texture_data;
    char field_64;
    bone_node_expression_data exp_data;
    float_t alpha;
    draw_task_flags draw_task_flags;
    bool disp;
    int32_t field_A4;
    mat4* field_A8;
    int32_t field_B0;
    bone_node* bone_nodes;
    vector_old_ptr_ex_node_block node_blocks;
    vector_old_bone_node ex_data_bone_nodes;
    vector_old_mat4 ex_data_matrices;
    vector_old_mat4 field_108;
    vector_old_ex_data_name_bone_index ex_bones;
    int64_t field_138;
    vector_old_ptr_ex_null_block null_blocks;
    vector_old_ptr_ex_osage_block osage_blocks;
    vector_old_ptr_ex_constraint_block constraint_blocks;
    vector_old_ptr_ex_expression_block expression_blocks;
    vector_old_ptr_ex_cloth_block cloth_blocks;
    char field_1B8;
    int64_t field_1C0;
    char field_1C8;
    aft_obj_skin_ex_data* skin_ex_data;
    aft_obj_skin* skin;
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
    int32_t eyes_adjust;
    vector_old_texture_pattern_struct texture_pattern;
    aft_object_info field_D0;
    int32_t field_D4;
    uint8_t field_D8;
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
    uint8_t field_93C;
    vector_old_struc_373 field_940;
    uint8_t field_958;
    uint8_t field_959;
    uint8_t field_95A;
};

extern void bone_data_mult_ik(bone_data* a1, int32_t a2);
extern void bone_data_mult_1(bone_data* a1, mat4* parent_mat, bone_data* a3, bool solve_ik);
extern void bone_data_mult_2(rob_chara_bone_data* a1, mat4* a2);
extern void rob_chara_bone_data_motion_load(rob_chara_bone_data* rob_bone_data,
    int32_t motion_id, motion_blend_type blend_type);
extern void rob_chara_bone_data_interpolate(rob_chara_bone_data* a1);
extern void rob_chara_bone_data_update(rob_chara_bone_data* a1, mat4* a2);
extern void rob_chara_reset_data(rob_chara* a1, rob_chara_pv_data* a2);

extern void ex_node_block_field_10(ex_node_block* node);

extern void* ex_osage_block_dispose(ex_osage_block* osg, bool dispose);
extern void ex_osage_block_field_8(ex_osage_block* osg);
extern void ex_osage_block_field_18(ex_osage_block* osg, int32_t a2, bool a3);
extern void ex_osage_block_update(ex_osage_block* osg);
extern void ex_osage_block_field_28(ex_osage_block* osg);
extern void ex_osage_block_disp(ex_osage_block* osg);
extern void ex_osage_block_reset(ex_osage_block* osg);
extern void ex_osage_block_field_40(ex_osage_block* osg);
extern void ex_osage_block_field_48(ex_osage_block* osg);
extern void ex_osage_block_field_50(ex_osage_block* osg);
extern void ex_osage_block_field_58(ex_osage_block* osg);
