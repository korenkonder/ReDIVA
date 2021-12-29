/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "rob.h"
#include "data.h"
#include "draw_task.h"
#include "../KKdLib/str_utils.h"

vector_func(bone_data)
vector_func(bone_node)
vector_ptr_func(ex_constraint_block)
vector_ptr_func(ex_cloth_block)
vector_ptr_func(ex_expression_block)
vector_ptr_func(ex_osage_block)
vector_ptr_func(ex_node_block)
vector_func(ex_data_name_bone_index)
vector_func(mot_key_set)
vector_ptr_func(motion_blend_mot)
vector_func(osage_coli)
vector_func(pair_int32_t_ptr_rob_osage_node)
vector_func(pair_name_ex_osage_block)
vector_func(rob_osage_node)
vector_ptr_func(rob_osage_node)
vector_func(skin_param_osage_root_coli)
vector_func(struc_331)

typedef struct motion_storage {
    uint32_t set_id;
    int32_t count;
    mot_set set;
} motion_storage;

typedef struct exp_func_op1 {
    char* name;
    float_t(*func)(float_t v1);
} exp_func_op1;

typedef struct exp_func_op2 {
    char* name;
    float_t(*func)(float_t v1, float_t v2);
} exp_func_op2;

typedef struct exp_func_op3 {
    char* name;
    float_t(*func)(float_t v1, float_t v2, float_t v3);
} exp_func_op3;

vector(motion_storage)

static bone_data* bone_data_init(bone_data* bone);
static float_t bone_data_limit_angle(float_t angle);
static void bone_data_mult_0(bone_data* a1, int32_t skeleton_select);
static void bone_data_mult_1(bone_data* a1, mat4* parent_mat, bone_data* a3, bool solve_ik);
static bool bone_data_mult_1_ik(bone_data* a1, bone_data* a2);
static void bone_data_mult_1_ik_hands(bone_data* a1, vec3* a2);
static void bone_data_mult_1_ik_hands_2(bone_data* a1, vec3* a2, float_t a3);
static void bone_data_mult_1_ik_legs(bone_data* a1, vec3* a2);
static bool bone_data_mult_1_exp_data(bone_data* a1, bone_node_expression_data* a2, bone_data* a3);
static void bone_data_mult_ik(bone_data* a1, int32_t skeleton_select);

static float_t* bone_node_get_exp_data_component(bone_node* a1,
    size_t index, ex_expression_block_stack_type* type);

static void bone_data_parent_data_init(bone_data_parent* bone,
    rob_chara_data_bone_data* rob_bone_data, bone_database* bone_data);
static void bone_data_parent_load_bone_database(bone_data_parent* bone,
    vector_bone_database_bone* bones, vec3* common_translation, vec3* translation);
static void bone_data_parent_load_rob_chara_data(bone_data_parent* bone);
static void bone_data_parent_reset(bone_data_parent* bone);
static void bone_data_parent_free(bone_data_parent* bone);

static void bone_node_expression_data_mat_set(bone_node_expression_data* data,
    vec3* data_scale, mat4* ex_data_mat, mat4* mat);
static void bone_node_expression_data_reset_position_rotation(bone_node_expression_data* data);
static void bone_node_expression_data_reset_scale(bone_node_expression_data* data);
static void bone_node_expression_data_set_position_rotation(bone_node_expression_data* data,
    float_t position_x, float_t position_y, float_t position_z,
    float_t rotation_x, float_t rotation_y, float_t rotation_z);
static void bone_node_expression_data_set_position_rotation_vec3(bone_node_expression_data* data,
    vec3* position, vec3* rotation);

static ex_cloth_block* ex_cloth_block_init();
static void ex_cloth_block_draw(ex_cloth_block* cls);
static void ex_cloth_block_reset(ex_cloth_block* cls);
static void ex_cloth_block_update(ex_cloth_block* cls);
static void ex_cloth_block_dispose(ex_cloth_block* cls);
static ex_constraint_block* ex_constraint_block_init();
static void ex_constraint_block_init_data(ex_constraint_block* cns, rob_chara_data_item_equip_object* itm_eq_obj,
    object_skin_block_constraint* cns_data, char* cns_data_name, bone_database* bone_data);
static void ex_constraint_block_data_set(ex_constraint_block* cns);
static void ex_constraint_block_draw(ex_constraint_block* cns);
static void ex_constraint_block_set_default_data(ex_constraint_block* cns);
static void ex_constraint_block_time_step(ex_constraint_block* cns);
static void ex_constraint_block_update(ex_constraint_block* cns);
static void ex_constraint_block_dispose(ex_constraint_block* cns);
static ex_expression_block* ex_expression_block_init();
static void ex_expression_block_init_data(ex_expression_block* exp,
    rob_chara_data_item_equip_object* itm_eq_obj, object_skin_block_expression* exp_data,
    char* exp_data_name, object_info a4, chara_index chara_index, bone_database* bone_data);
static void ex_expression_block_data_set(ex_expression_block* exp);
static void ex_expression_block_draw(ex_expression_block* exp);
static void ex_expression_block_set_default_data(ex_expression_block* exp);
static void ex_expression_block_time_step(ex_expression_block* exp);
static void ex_expression_block_update(ex_expression_block* exp);
static void ex_expression_block_dispose(ex_expression_block* exp);
static float_t ex_expression_block_stack_get_value(ex_expression_block_stack* stack);
static void ex_node_block_init_data(ex_node_block* node, bone_node* bone_node,
    ex_node_type type, char* name, rob_chara_data_item_equip_object* itm_eq_obj);
static void ex_node_block_reset(ex_node_block* node);
static void ex_node_block_set_default_data(ex_node_block* node);
static void ex_node_block_free(ex_node_block* node);
static ex_osage_block* ex_osage_block_init();
static void ex_osage_block_draw(ex_osage_block* osg);
static void ex_osage_block_init_data(ex_osage_block* osg,
    rob_chara_data_item_equip_object* itm_eq_obj, object_skin_block_osage* osg_data,
    char* osg_data_name, object_skin_osage_node* osg_nodes, bone_node* bone_nodes,
    bone_node* ex_data_bone_nodes, object_skin* skin);
static void sub_1405F3E10(ex_osage_block* osg, object_skin_block_osage* osg_data,
    object_skin_osage_node* osg_nodes, vector_pair_int32_t_ptr_rob_osage_node* a4,
    vector_pair_name_ex_osage_block* a5);
static void ex_osage_block_reset(ex_osage_block* osg);
static void ex_osage_block_set_wind_direction(ex_osage_block* osg);
static void ex_osage_block_update(ex_osage_block* osg);
static void ex_osage_block_dispose(ex_osage_block* osg);

static float_t exp_abs(float_t v1);
static float_t exp_acos(float_t v1);
static float_t exp_add(float_t v1, float_t v2);
static float_t exp_and(float_t v1, float_t v2);
static float_t exp_asin(float_t v1);
static float_t exp_atan(float_t v1);
static float_t exp_av(float_t v1);
static float_t exp_ceil(float_t v1);
static float_t exp_cond(float_t v1, float_t v2, float_t v3);
static float_t exp_cos(float_t v1);
static float_t exp_div(float_t v1, float_t v2);
static float_t exp_eq(float_t v1, float_t v2);
static float_t exp_exp(float_t v1);
static float_t exp_floor(float_t v1);
static float_t exp_fmod(float_t v1, float_t v2);
static const exp_func_op1* exp_func_op1_find_func(string* name, const exp_func_op1* array);
static const exp_func_op2* exp_func_op2_find_func(string* name, const exp_func_op2* array);
static const exp_func_op3* exp_func_op3_find_func(string* name, const exp_func_op3* array);
static float_t exp_ge(float_t v1, float_t v2);
static float_t exp_gt(float_t v1, float_t v2);
static float_t exp_le(float_t v1, float_t v2);
static float_t exp_log(float_t v1);
static float_t exp_lt(float_t v1, float_t v2);
static float_t exp_max(float_t v1, float_t v2);
static float_t exp_min(float_t v1, float_t v2);
static float_t exp_mul(float_t v1, float_t v2);
static float_t exp_ne(float_t v1, float_t v2);
static float_t exp_neg(float_t v1);
static float_t exp_or(float_t v1, float_t v2);
static float_t exp_pow(float_t v1, float_t v2);
static float_t exp_rand(float_t v1, float_t v2, float_t v3);
static float_t exp_rand_0_1(float_t v1);
static float_t exp_round(float_t v1);
static float_t exp_sin(float_t v1);
static float_t exp_sqrt(float_t v1);
static float_t exp_sub(float_t v1, float_t v2);
static float_t exp_tan(float_t v1);

static float_t get_osage_gravity_const();

static void mot_key_frame_interpolate(mot* a1, float_t frame, float_t* value,
    mot_key_set* a4, uint32_t key_set_count, struc_369* a6);
static uint32_t mot_load_last_key_calc(uint16_t keys_count);
static bool mot_data_load_file(mot* a1, mot_data* a2);
static void mot_parent_get_key_set_count_by_bone_database_bones(mot_parent* a1,
    vector_bone_database_bone* a2);
static void mot_parent_get_key_set_count(mot_parent* a1, size_t motion_bone_count, size_t ik_bone_count);
static void mot_parent_init_key_sets(mot_parent* a1, bone_database_skeleton_type type,
    size_t motion_bone_count, size_t ik_bone_count);
static void mot_parent_interpolate(mot_parent* a1, float_t frame,
    uint32_t key_set_offset, uint32_t key_set_count);
static mot_data* mot_parent_load_file(mot_parent* a1, int32_t motion_id, motion_database* mot_db);
static void mot_parent_reserve_key_sets(mot_parent* a1);
static motion_blend_mot* motion_blend_mot_init();
static void motion_blend_mot_interpolate(motion_blend_mot* a1);
static void motion_blend_mot_load_bone_data(motion_blend_mot* a1,
    rob_chara_data_bone_data* a2, bone_database* bone_data);
static bool motion_blend_mot_interpolate_get_reverse(int32_t* a1);
static void motion_blend_mot_load_file(motion_blend_mot* a1, int32_t motion_id,
    bone_database* a3, motion_database* mot_db);
static void motion_blend_mot_mult_mat(motion_blend_mot* a1, mat4* mat);
static void motion_blend_mot_reset(motion_blend_mot* mot_blend_mot);
static void motion_blend_mot_dispose(motion_blend_mot* mot_blend_mot);

static void osage_coli_set(osage_coli* osg_coli,
    skin_param_osage_root_coli* skp_root_coli, mat4* mats);
static void osage_coli_ring_set(osage_coli* osg_coli,
    vector_skin_param_osage_root_coli* vec_skp_root_coli, mat4* mats);

static rob_chara_data_bone_data* rob_chara_data_bone_data_init();
static void rob_chara_data_bone_data_calculate_bones(rob_chara_data_bone_data* rob_bone_data,
    vector_bone_database_bone* bones);
static void rob_chara_data_bone_data_eyes_xrot_adjust(rob_chara_data_bone_data* rob_bone_data,
    const struc_241* a2, eyes_adjust* a3);
static float_t rob_chara_data_bone_data_get_frame(rob_chara_data_bone_data* rob_bone_data);
static float_t rob_chara_data_bone_data_get_frame_count(rob_chara_data_bone_data* rob_bone_data);
static void rob_chara_data_bone_data_get_ik_scale(
    rob_chara_data_bone_data* rob_bone_data, bone_database* bone_data);
static mat4* rob_chara_data_bone_data_get_mat(rob_chara_data_bone_data* rob_bone_data, size_t index);
static bone_node* rob_chara_data_bone_data_get_node(rob_chara_data_bone_data* rob_bone_data, size_t index);
static void rob_chara_data_bone_data_ik_scale_calculate(
    rob_chara_data_bone_data_ik_scale* ik_scale, vector_bone_data* vec_bone_data,
    bone_database_skeleton_type base_skeleton_type,
    bone_database_skeleton_type skeleton_type, bone_database* bone_db);
static void rob_chara_data_bone_data_init_data(rob_chara_data_bone_data* rob_bone_data,
    bone_database_skeleton_type base_skeleton_type,
    bone_database_skeleton_type skeleton_type, bone_database* bone_data);
static void rob_chara_data_bone_data_init_skeleton(rob_chara_data_bone_data* rob_bone_data,
    bone_database_skeleton_type base_skeleton_type,
    bone_database_skeleton_type skeleton_type, bone_database* bone_data);
static void rob_chara_data_bone_data_interpolate(rob_chara_data_bone_data* rob_bone_data);
static void rob_chara_data_bone_data_motion_blend_mot_free(rob_chara_data_bone_data* rob_bone_data);
static void rob_chara_data_bone_data_motion_blend_mot_init(rob_chara_data_bone_data* rob_bone_data);
static void rob_chara_data_bone_data_motion_blend_mot_list_free(rob_chara_data_bone_data* rob_bone_data,
    size_t last_index);
static void rob_chara_data_bone_data_motion_blend_mot_list_init(rob_chara_data_bone_data* rob_bone_data);
static void rob_chara_data_bone_data_motion_load(rob_chara_data_bone_data* rob_bone_data,
    int32_t motion_id, int32_t index, bone_database* bone_data, motion_database* mot_db);
static void rob_chara_data_bone_data_reserve(rob_chara_data_bone_data* rob_bone_data);
static void rob_chara_data_bone_data_set_frame(rob_chara_data_bone_data* rob_bone_data, float_t frame);
static void rob_chara_data_bone_data_reset(rob_chara_data_bone_data* rob_bone_data);
static void rob_chara_data_bone_data_set_mats(rob_chara_data_bone_data* rob_bone_data,
    vector_bone_database_bone* bones);
static void rob_chara_data_bone_data_set_parent_mats(rob_chara_data_bone_data* rob_bone_data,
    uint16_t* parent_indices);
static void rob_chara_data_bone_data_set_rotation_y(rob_chara_data_bone_data* rob_bone_data, float_t rotation_y);
static void rob_chara_data_bone_data_update(rob_chara_data_bone_data* rob_bone_data, mat4* mat);
static void rob_chara_data_bone_data_dispose(rob_chara_data_bone_data* rob_bone_data);

static rob_chara_data_item_equip* rob_chara_data_item_equip_init();
static void rob_chara_data_item_equip_draw(
    rob_chara_data_item_equip* rob_item_equip, int32_t chara_id, render_context* rctx);
static void rob_chara_data_item_equip_get_parent_bone_nodes(
    rob_chara_data_item_equip* rob_item_equip, bone_node* bone_nodes, bone_database* bone_data);
static void rob_chara_data_item_equip_load_object_info(
    rob_chara_data_item_equip* rob_item_equip, object_info object_info, item_id id, bone_database* bone_data);
static void rob_chara_data_item_equip_object_init(rob_chara_data_item_equip_object* itm_eq_obj);
static void rob_chara_data_item_equip_object_clear_ex_data(rob_chara_data_item_equip_object* itm_eq_obj);
static void rob_chara_data_item_equip_object_draw(
    rob_chara_data_item_equip_object* itm_eq_obj, mat4* mat, render_context* rctx);
static int32_t rob_chara_data_item_equip_object_get_bone_index(
    rob_chara_data_item_equip_object* itm_eq_obj, char* name, bone_database* bone_data);
static bone_node* rob_chara_data_item_equip_object_get_bone_node(
    rob_chara_data_item_equip_object* itm_eq_obj, int32_t bone_index);
static bone_node* rob_chara_data_item_equip_object_get_bone_node_by_name(
    rob_chara_data_item_equip_object* itm_eq_obj, char* name, bone_database* bone_data);
static void rob_chara_data_item_equip_object_get_parent_bone_nodes(
    rob_chara_data_item_equip_object* itm_eq_obj, bone_node* bone_nodes, bone_database* bone_data);
static void rob_chara_data_item_equip_object_init_data(rob_chara_data_item_equip_object* itm_eq_obj, size_t index);
static void rob_chara_data_item_equip_object_init_ex_data_bone_nodes(
    rob_chara_data_item_equip_object* itm_eq_obj, object_skin_ex_data* a2);
static void rob_chara_data_item_equip_object_load_ex_data(rob_chara_data_item_equip_object* itm_eq_obj,
    object_skin_ex_data* ex_data, bone_database* bone_data);
static void rob_chara_data_item_equip_object_load_object_info_ex_data(
    rob_chara_data_item_equip_object* itm_eq_obj, object_info object_info,
    bone_node* bone_nodes, bone_database* bone_data);
static void rob_chara_data_item_equip_object_reset(rob_chara_data_item_equip_object* itm_eq_obj);
static void rob_chara_data_item_equip_object_free(rob_chara_data_item_equip_object* itm_eq_obj);
static void rob_chara_data_item_equip_reset(rob_chara_data_item_equip* rob_item_equip);
static void rob_chara_data_item_equip_dispose(rob_chara_data_item_equip* rob_item_equip);

static bool rob_chara_data_check_for_ageageagain_module(chara_index chara_index, int32_t module_index);
static void rob_chara_data_load_default_motion(rob_chara_data* rob_chr_data,
    bone_database* bone_data, motion_database* mot_db);
static void rob_chara_data_load_default_motion_sub(rob_chara_data* rob_chr_data, int32_t skeleton_select,
    int32_t motion_id, bone_database* bone_data, motion_database* mot_db);
static void rob_chara_data_set_draw(rob_chara_data* rob_chr_data, item_id id, bool draw);

static void rob_osage_init(rob_osage* rob_osg);
static void rob_osage_init_data(rob_osage* rob_osg,
    object_skin_block_osage* osg_data, object_skin_osage_node* osg_nodes,
    bone_node* ex_data_bone_nodes, object_skin* skin);
static rob_osage_node* rob_osage_get_node(rob_osage* rob_osg, size_t index);
static void rob_osage_node_init(rob_osage_node* node);
static void rob_osage_node_free(rob_osage_node* node);
static void rob_osage_set_coli(rob_osage* rob_osg, mat4* mats);
static void rob_osage_free(rob_osage* rob_osg);

static void skin_param_init(skin_param* skp);

vector_func(motion_storage)

vector_motion_storage motion_storage_data;
rob_chara_data rob_chara_data_array[6];
extern wind* wind_ptr;

static const exp_func_op1 exp_func_op1_array[] = {
    { "neg"     , exp_neg      },
    { "sin"     , exp_sin      },
    { "cos"     , exp_cos      },
    { "tan"     , exp_tan      },
    { "abs"     , exp_abs      },
    { "sqrt"    , exp_sqrt     },
    { "av"      , exp_av       },
    { "floor"   , exp_floor    },
    { "ceil"    , exp_ceil     },
    { "round"   , exp_round    },
    { "asin"    , exp_asin     },
    { "acos"    , exp_acos     },
    { "atan"    , exp_atan     },
    { "log"     , exp_log      },
    { "exp"     , exp_exp      },
    { "rand_0_1", exp_rand_0_1 },
    { 0         , 0            },
};

static const exp_func_op2 exp_func_op2_array[] = {
    { "+"   , exp_add  },
    { "-"   , exp_sub  },
    { "*"   , exp_mul  },
    { "/"   , exp_div  },
    { "%"   , exp_fmod },
    { "=="  , exp_eq   },
    { ">"   , exp_gt   },
    { ">="  , exp_ge   },
    { "<"   , exp_lt   },
    { "<="  , exp_le   },
    { "!="  , exp_ne   },
    { "&&"  , exp_and  },
    { "||"  , exp_or   },
    { "min" , exp_min  },
    { "max" , exp_max  },
    { "fmod", exp_fmod },
    { "pow" , exp_pow  },
    { 0     , 0        },
};

static const exp_func_op3 exp_func_op3_array[] = {
    { "rand", exp_rand },
    { "cond", exp_cond },
    { 0     , 0        },
};

static const struc_218 stru_140A24B50[] = {
    { { 0.02f , -0.005f,  0.0f   }, 0.09f , 0xAB, 0 },
    { { 0.17f , -0.02f ,  0.03f  }, 0.09f , 0x05, 1 },
    { { 0.17f , -0.02f , -0.03f  }, 0.09f , 0x05, 1 },
    { { 0.0f  ,  0.02f ,  0.0f   }, 0.04f , 0x08, 0 },
    { { 0.005f,  0.0f  ,  0.08f  }, 0.12f , 0x0A, 1 },
    { { 0.03f ,  0.0f  ,  0.015f }, 0.055f, 0x8A, 1 },
    { { 0.15f ,  0.0f  ,  0.01f  }, 0.05f , 0x8A, 0 },
    { { 0.05f ,  0.0f  ,  0.01f  }, 0.05f , 0x8B, 0 },
    { { 0.17f ,  0.0f  ,  0.015f }, 0.06f , 0x8B, 0 },
    { { 0.08f ,  0.0f  ,  0.0f   }, 0.07f , 0x8C, 1 },
    { { 0.03f ,  0.0f  , -0.015f }, 0.055f, 0x67, 1 },
    { { 0.15f ,  0.0f  , -0.01f  }, 0.05f , 0x67, 0 },
    { { 0.05f ,  0.0f  , -0.01f  }, 0.05f , 0x68, 0 },
    { { 0.17f ,  0.0f  , -0.015f }, 0.06f , 0x68, 0 },
    { { 0.08f ,  0.0f  ,  0.0f   }, 0.07f , 0x69, 1 },
    { { 0.04f , -0.02f , -0.02f  }, 0.13f , 0xB3, 1 },
    { { 0.26f , -0.02f , -0.025f }, 0.09f , 0xB3, 0 },
    { { 0.08f ,  0.0f  , -0.02f  }, 0.09f , 0xB4, 0 },
    { { 0.28f ,  0.0f  , -0.02f  }, 0.09f , 0xB4, 0 },
    { { 0.03f , -0.02f , -0.01f  }, 0.07f , 0xB5, 0 },
    { { 0.02f , -0.02f , -0.02f  }, 0.035f, 0xB6, 0 },
    { { 0.04f , -0.02f ,  0.02f  }, 0.13f , 0xAD, 1 },
    { { 0.26f , -0.02f ,  0.025f }, 0.09f , 0xAD, 0 },
    { { 0.09f ,  0.0f  ,  0.02f  }, 0.09f , 0xAE, 0 },
    { { 0.28f ,  0.0f  ,  0.02f  }, 0.09f , 0xAE, 0 },
    { { 0.03f , -0.02f ,  0.01f  }, 0.07f , 0xAF, 0 },
    { { 0.02f , -0.02f ,  0.02f  }, 0.035f, 0xB0, 0 },
};

static const struc_218 stru_140A25090[] = {
    { { -0.00391f, -0.08831f,  0.0f     }, 0.09f , 0xAB, 0 },
    { {  0.17f   , -0.02f   ,  0.03f    }, 0.09f , 0x05, 1 },
    { {  0.17f   , -0.02f   , -0.03f    }, 0.09f , 0x05, 1 },
    { {  0.0f    ,  0.02f   ,  0.0f     }, 0.04f , 0x08, 0 },
    { {  0.05025f,  0.0f    , -0.00181f }, 0.12f , 0x0A, 1 },
    { {  0.03f   ,  0.0f    ,  0.015f   }, 0.055f, 0x8A, 1 },
    { {  0.15f   ,  0.0f    ,  0.01f    }, 0.05f , 0x8A, 0 },
    { {  0.05f   ,  0.0f    ,  0.01f    }, 0.05f , 0x8B, 0 },
    { {  0.17f   ,  0.0f    ,  0.015f   }, 0.06f , 0x8B, 0 },
    { {  0.08f   ,  0.0f    ,  0.0f     }, 0.07f , 0x8C, 1 },
    { {  0.03f   ,  0.0f    , -0.015f   }, 0.055f, 0x67, 1 },
    { {  0.15f   ,  0.0f    , -0.01f    }, 0.05f , 0x67, 0 },
    { {  0.05f   ,  0.0f    , -0.01f    }, 0.05f , 0x68, 0 },
    { {  0.17f   ,  0.0f    , -0.015f   }, 0.06f , 0x68, 0 },
    { {  0.08f   ,  0.0f    ,  0.0f     }, 0.07f , 0x69, 1 },
    { {  0.04f   , -0.02f   , -0.02f    }, 0.13f , 0xB3, 1 },
    { {  0.26f   , -0.02f   , -0.025f   }, 0.09f , 0xB3, 0 },
    { {  0.08f   ,  0.0f    , -0.02f    }, 0.09f , 0xB4, 0 },
    { {  0.28f   ,  0.0f    , -0.02f    }, 0.09f , 0xB4, 0 },
    { {  0.03f   , -0.02f   , -0.01f    }, 0.07f , 0xB5, 0 },
    { {  0.02f   , -0.02f   , -0.02f    }, 0.035f, 0xB6, 0 },
    { {  0.04f   , -0.02f   ,  0.02f    }, 0.13f , 0xAD, 1 },
    { {  0.26f   , -0.02f   ,  0.025f   }, 0.09f , 0xAD, 0 },
    { {  0.08f   ,  0.0f    ,  0.02f    }, 0.09f , 0xAE, 0 },
    { {  0.28f   ,  0.0f    ,  0.02f    }, 0.09f , 0xAE, 0 },
    { {  0.03f   , -0.02f   ,  0.01f    }, 0.07f , 0xAF, 0 },
    { {  0.02f   , -0.02f   ,  0.02f    }, 0.035f, 0xB0, 0 },
};

static const struc_218 stru_140A24E00[] = {
    { { 0.02f    , -0.005f,  0.0f   }, 0.09f , 0xAB, 0 },
    { { 0.165f   , -0.02f ,  0.015f }, 0.105f, 0x05, 1 },
    { { 0.165f   , -0.02f , -0.015f }, 0.105f, 0x05, 1 },
    { { 0.0f     ,  0.02f ,  0.0f   }, 0.04f , 0x08, 0 },
    { { 0.005f   ,  0.0f  ,  0.08f  }, 0.12f , 0x0A, 1 },
    { { 0.03f    ,  0.0f  ,  0.015f }, 0.055f, 0x8A, 1 },
    { { 0.15f    ,  0.0f  ,  0.01f  }, 0.05f , 0x8A, 0 },
    { { 0.05f    ,  0.0f  ,  0.01f  }, 0.05f , 0x8B, 0 },
    { { 0.17f    ,  0.0f  ,  0.015f }, 0.06f , 0x8B, 0 },
    { { 0.08f    ,  0.0f  ,  0.0f   }, 0.07f , 0x8C, 1 },
    { { 0.03f    ,  0.0f  , -0.015f }, 0.055f, 0x67, 1 },
    { { 0.15f    ,  0.0f  , -0.01f  }, 0.05f , 0x67, 0 },
    { { 0.05f    ,  0.0f  , -0.01f  }, 0.05f , 0x68, 0 },
    { { 0.17f    ,  0.0f  , -0.015f }, 0.06f , 0x68, 0 },
    { { 0.08f    ,  0.0f  ,  0.0f   }, 0.07f , 0x69, 1 },
    { { 0.04f    , -0.02f , -0.02f  }, 0.13f , 0xB3, 1 },
    { { 0.26f    , -0.02f , -0.025f }, 0.09f , 0xB3, 0 },
    { { 0.08f    ,  0.0f  , -0.02f  }, 0.09f , 0xB4, 0 },
    { { 0.28f    ,  0.0f  , -0.02f  }, 0.09f , 0xB4, 0 },
    { { 0.03f    , -0.02f , -0.01f  }, 0.07f , 0xB5, 0 },
    { { 0.02f    , -0.02f , -0.02f  }, 0.035f, 0xB6, 0 },
    { { 0.04f    , -0.02f ,  0.02f  }, 0.13f , 0xAD, 1 },
    { { 0.26f    , -0.02f ,  0.025f }, 0.09f , 0xAD, 0 },
    { { 0.08f    ,  0.0f  ,  0.02f  }, 0.09f , 0xAE, 0 },
    { { 0.28f    ,  0.0f  ,  0.02f  }, 0.09f , 0xAE, 0 },
    { { 0.03f    , -0.02f ,  0.01f  }, 0.07f , 0xAF, 0 },
    { { 0.02f    , -0.02f ,  0.02f  }, 0.035f, 0xB0, 0 },
};

static const struc_218 stru_140A25340[] = {
    { { -0.00391f, -0.08831f,  0.0f     }, 0.09f , 0xAB, 0 },
    { {  0.165f  , -0.02f   ,  0.015f   }, 0.105f, 0x05, 1 },
    { {  0.165f  , -0.02f   , -0.015f   }, 0.105f, 0x05, 1 },
    { {  0.0f    ,  0.02f   ,  0.0f     }, 0.04f , 0x08, 0 },
    { {  0.05025f,  0.0f    , -0.00181f }, 0.12f , 0x0A, 1 },
    { {  0.03f   ,  0.0f    ,  0.015f   }, 0.055f, 0x8A, 1 },
    { {  0.15f   ,  0.0f    ,  0.01f    }, 0.05f , 0x8A, 0 },
    { {  0.05f   ,  0.0f    ,  0.01f    }, 0.05f , 0x8B, 0 },
    { {  0.17f   ,  0.0f    ,  0.015f   }, 0.06f , 0x8B, 0 },
    { {  0.08f   ,  0.0f    ,  0.0f     }, 0.07f , 0x8C, 1 },
    { {  0.03f   ,  0.0f    , -0.015f   }, 0.055f, 0x67, 1 },
    { {  0.15f   ,  0.0f    , -0.01f    }, 0.05f , 0x67, 0 },
    { {  0.05f   ,  0.0f    , -0.01f    }, 0.05f , 0x68, 0 },
    { {  0.17f   ,  0.0f    , -0.015f   }, 0.06f , 0x68, 0 },
    { {  0.08f   ,  0.0f    ,  0.0f     }, 0.07f , 0x69, 1 },
    { {  0.04f   , -0.02f   , -0.02f    }, 0.13f , 0xB3, 1 },
    { {  0.26f   , -0.02f   , -0.025f   }, 0.09f , 0xB3, 0 },
    { {  0.08f   ,  0.0f    , -0.02f    }, 0.09f , 0xB4, 0 },
    { {  0.28f   ,  0.0f    , -0.02f    }, 0.09f , 0xB4, 0 },
    { {  0.03f   , -0.02f   , -0.01f    }, 0.07f , 0xB5, 0 },
    { {  0.02f   , -0.02f   , -0.02f    }, 0.035f, 0xB6, 0 },
    { {  0.04f   , -0.02f   ,  0.02f    }, 0.13f , 0xAD, 1 },
    { {  0.26f   , -0.02f   ,  0.025f   }, 0.09f , 0xAD, 0 },
    { {  0.08f   ,  0.0f    ,  0.02f    }, 0.09f , 0xAE, 0 },
    { {  0.28f   ,  0.0f    ,  0.02f    }, 0.09f , 0xAE, 0 },
    { {  0.03f   , -0.02f   ,  0.01f    }, 0.07f , 0xAF, 0 },
    { {  0.02f   , -0.02f   ,  0.02f    }, 0.035f, 0xB0, 0 },
};

static const chara_init_data chara_init_data_array[] = {
    {
        0x0000, BONE_DATABASE_SKELETON_MIKU,
        { 0x00F7, 0x0000 }, 1,
        stru_140A24B50, stru_140A25090,
        1, 0, { 0x01, 0x01, 0x1E, 0x1D },
        {
            { 0x00F7, 0x0000 },
            { 0x011D, 0x0000 },
            { 0x011E, 0x0000 },
            { 0x011F, 0x0000 },
            { 0x0120, 0x0000 },
            { 0x0121, 0x0000 },
            { 0x0122, 0x0000 },
            { 0x020F, 0x0000 },
            { 0x0251, 0x0000 },
        },
        {
            { -1, -1 },
            { 0x0124, 0x0000 },
            { 0x0123, 0x0000 },
            { 0x0125, 0x0000 },
            { 0x0126, 0x0000 },
            { 0x0127, 0x0000 },
            { 0x0128, 0x0000 },
            { 0x0259, 0x0000 },
            { 0x0254, 0x0000 },
            { 0x0252, 0x0000 },
            { 0x0253, 0x0000 },
            { 0x0255, 0x0000 },
            { 0x0256, 0x0000 },
            { 0x0258, 0x0000 },
            { 0x0257, 0x0000 },
        },
    },
    {
        0x0107, BONE_DATABASE_SKELETON_RIN, 
        { 0x004A, 0x0107 }, 9,
        stru_140A24B50, stru_140A25090,
        1, 0, { 0x02, 0x02, 0x01, 0x01 },
        {
            { 0x004A, 0x0107 },
            { 0x004E, 0x0107 },
            { 0x004F, 0x0107 },
            { 0x0050, 0x0107 },
            { 0x0051, 0x0107 },
            { 0x0052, 0x0107 },
            { 0x0053, 0x0107 },
            { 0x00A6, 0x0107 },
            { 0x00C3, 0x0107 },
        },
        {
            { -1, -1 },
            { 0x0055, 0x0107 },
            { 0x0054, 0x0107 },
            { 0x0056, 0x0107 },
            { 0x0057, 0x0107 },
            { 0x0058, 0x0107 },
            { 0x0059, 0x0107 },
            { 0x00CB, 0x0107 },
            { 0x00C6, 0x0107 },
            { 0x00C4, 0x0107 },
            { 0x00C5, 0x0107 },
            { 0x00C7, 0x0107 },
            { 0x00C8, 0x0107 },
            { 0x00CA, 0x0107 },
            { 0x00C9, 0x0107 },
        },
    },
    {
        0x0105, BONE_DATABASE_SKELETON_LEN,
        { 0x0040, 0x0105 }, 5,
        stru_140A24B50, stru_140A25090,
        0, 0, { 0x02, 0x03, 0x01, 0x01 },
        {
            { 0x0040, 0x0105 },
            { 0x0041, 0x0105 },
            { 0x0042, 0x0105 },
            { 0x0043, 0x0105 },
            { 0x0044, 0x0105 },
            { 0x0045, 0x0105 },
            { 0x0046, 0x0105 },
            { 0x007F, 0x0105 },
            { 0x0098, 0x0105 },
        },
        {
            { -1, -1 },
            { 0x0048, 0x0105 },
            { 0x0047, 0x0105 },
            { 0x0049, 0x0105 },
            { 0x004A, 0x0105 },
            { 0x004B, 0x0105 },
            { 0x004C, 0x0105 },
            { 0x00A0, 0x0105 },
            { 0x009B, 0x0105 },
            { 0x0099, 0x0105 },
            { 0x009A, 0x0105 },
            { 0x009C, 0x0105 },
            { 0x009D, 0x0105 },
            { 0x009F, 0x0105 },
            { 0x009E, 0x0105 },
        },
    },
    {
        0x0106, BONE_DATABASE_SKELETON_LUKA,
        { 0x004C, 0x0106 }, 6,
        stru_140A24E00, stru_140A25340,
        0, 0, { 0x01, 0x04, 0x01, 0x01 },
        {
            { 0x004C, 0x0106 },
            { 0x004D, 0x0106 },
            { 0x004E, 0x0106 },
            { 0x004F, 0x0106 },
            { 0x0050, 0x0106 },
            { 0x0051, 0x0106 },
            { 0x0052, 0x0106 },
            { 0x008C, 0x0106 },
            { 0x00A1, 0x0106 },
        },
        {
            { -1, -1 },
            { 0x0054, 0x0106 },
            { 0x0053, 0x0106 },
            { 0x0055, 0x0106 },
            { 0x0056, 0x0106 },
            { 0x0057, 0x0106 },
            { 0x0058, 0x0106 },
            { 0x00A9, 0x0106 },
            { 0x00A4, 0x0106 },
            { 0x00A2, 0x0106 },
            { 0x00A3, 0x0106 },
            { 0x00A5, 0x0106 },
            { 0x00A6, 0x0106 },
            { 0x00A8, 0x0106 },
            { 0x00A7, 0x0106 },
        },
    },
    {
        0x0108, BONE_DATABASE_SKELETON_NERU,
        { 0x0019, 0x0108 }, 8,
        stru_140A24B50, stru_140A25090,
        0, 0, { 0x02, 0x02, 0x02, 0x02 },
        {
            { 0x0019, 0x0108 },
            { 0x001A, 0x0108 },
            { 0x001B, 0x0108 },
            { 0x001C, 0x0108 },
            { 0x001D, 0x0108 },
            { 0x001E, 0x0108 },
            { 0x001F, 0x0108 },
            { 0x002A, 0x0108 },
            { 0x002B, 0x0108 },
        },
        {
            { -1, -1 },
            { 0x0021, 0x0108 },
            { 0x0020, 0x0108 },
            { 0x0022, 0x0108 },
            { 0x0023, 0x0108 },
            { 0x0024, 0x0108 },
            { 0x0025, 0x0108 },
            { 0x0033, 0x0108 },
            { 0x002E, 0x0108 },
            { 0x002C, 0x0108 },
            { 0x002D, 0x0108 },
            { 0x002F, 0x0108 },
            { 0x0030, 0x0108 },
            { 0x0032, 0x0108 },
            { 0x0031, 0x0108 },
        },
    },
    {
        0x0109, BONE_DATABASE_SKELETON_HAKU,
        { 0x001F, 0x0109 }, 3,
        stru_140A24E00, stru_140A25340,
        1, 0, { 0x01, 0x01, 0x02, 0x02 },
        {
            { 0x001F, 0x0109 },
            { 0x0020, 0x0109 },
            { 0x0021, 0x0109 },
            { 0x0022, 0x0109 },
            { 0x0023, 0x0109 },
            { 0x0024, 0x0109 },
            { 0x0025, 0x0109 },
            { 0x0032, 0x0109 },
            { 0x0033, 0x0109 },
        },
        {
            { -1, -1 },
            { 0x0027, 0x0109 },
            { 0x0026, 0x0109 },
            { 0x0028, 0x0109 },
            { 0x0029, 0x0109 },
            { 0x002A, 0x0109 },
            { 0x002B, 0x0109 },
            { 0x003B, 0x0109 },
            { 0x0036, 0x0109 },
            { 0x0034, 0x0109 },
            { 0x0035, 0x0109 },
            { 0x0037, 0x0109 },
            { 0x0038, 0x0109 },
            { 0x003A, 0x0109 },
            { 0x0039, 0x0109 },
        },
    },
    {
        0x010D, BONE_DATABASE_SKELETON_KAITO,
        { 0x003C, 0x010D}, 4,
        stru_140A24B50, stru_140A25090,
        0, 0, { 0x00, 0x00, 0x01, 0x01 },
        {
            { 0x003C, 0x010D },
            { 0x0050, 0x010D },
            { 0x003D, 0x010D },
            { 0x003E, 0x010D },
            { 0x003F, 0x010D },
            { 0x0040, 0x010D },
            { 0x0041, 0x010D },
            { 0x006D, 0x010D },
            { 0x0077, 0x010D },
        },
        {
            { -1, -1 },
            { 0x0044, 0x010D },
            { 0x0043, 0x010D },
            { 0x0045, 0x010D },
            { 0x0046, 0x010D },
            { 0x0047, 0x010D },
            { 0x0048, 0x010D },
            { 0x007F, 0x010D },
            { 0x007A, 0x010D },
            { 0x0078, 0x010D },
            { 0x0079, 0x010D },
            { 0x007B, 0x010D },
            { 0x007C, 0x010D },
            { 0x007E, 0x010D },
            { 0x007D, 0x010D },
        },
    },
    {
        0x010E, BONE_DATABASE_SKELETON_MEIKO,
        { 0x0040, 0x010E }, 7,
        stru_140A24E00, stru_140A25340,
        1, 0, { 0x00, 0x00, 0x01, 0x01 },
        {
            { 0x0040, 0x010E },
            { 0x0041, 0x010E },
            { 0x0042, 0x010E },
            { 0x0043, 0x010E },
            { 0x0044, 0x010E },
            { 0x0045, 0x010E },
            { 0x0046, 0x010E },
            { 0x007B, 0x010E },
            { 0x0085, 0x010E },
        },
        {
            { -1, -1 },
            { 0x0048, 0x010E },
            { 0x0047, 0x010E },
            { 0x0049, 0x010E },
            { 0x004A, 0x010E },
            { 0x004B, 0x010E },
            { 0x004C, 0x010E },
            { 0x008D, 0x010E },
            { 0x0088, 0x010E },
            { 0x0086, 0x010E },
            { 0x0087, 0x010E },
            { 0x0089, 0x010E },
            { 0x008A, 0x010E },
            { 0x008C, 0x010E },
            { 0x008B, 0x010E },
        },
    },
    {
        0x010F, BONE_DATABASE_SKELETON_SAKINE,
        { 0x001A, 0x010F }, 10,
        stru_140A24E00, stru_140A25340,
        1, 0, { 0x01, 0x01, 0x02, 0x02 },
        {
            { 0x001A, 0x010F },
            { 0x001B, 0x010F },
            { 0x001C, 0x010F },
            { 0x001D, 0x010F },
            { 0x001E, 0x010F },
            { 0x001F, 0x010F },
            { 0x0020, 0x010F },
            { 0x0031, 0x010F },
            { 0x0033, 0x010F },
        },
        {
            { -1, -1 },
            { 0x0022, 0x010F },
            { 0x0021, 0x010F },
            { 0x0023, 0x010F },
            { 0x0024, 0x010F },
            { 0x0025, 0x010F },
            { 0x0026, 0x010F },
            { 0x003B, 0x010F },
            { 0x0036, 0x010F },
            { 0x0034, 0x010F },
            { 0x0035, 0x010F },
            { 0x0037, 0x010F },
            { 0x0038, 0x010F },
            { 0x003A, 0x010F },
            { 0x0039, 0x010F },
        },
    },
    {
        0x063D, BONE_DATABASE_SKELETON_TETO,
        { 0x0000, 0x063D }, 467,
        stru_140A24B50, stru_140A25090,
        1, 0, { 0x01, 0x01, 0x02, 0x02 },
        {
            { 0x0000, 0x063D },
            { 0x0001, 0x063D },
            { 0x0002, 0x063D },
            { 0x0003, 0x063D },
            { 0x0004, 0x063D },
            { 0x0005, 0x063D },
            { 0x0006, 0x063D },
            { 0x0012, 0x063D },
            { 0x0013, 0x063D },
        },
        {
            { -1, -1 },
            { 0x0008, 0x063D },
            { 0x0007, 0x063D },
            { 0x0009, 0x063D },
            { 0x000A, 0x063D },
            { 0x000B, 0x063D },
            { 0x000C, 0x063D },
            { 0x001B, 0x063D },
            { 0x0016, 0x063D },
            { 0x0014, 0x063D },
            { 0x0015, 0x063D },
            { 0x0017, 0x063D },
            { 0x0018, 0x063D },
            { 0x001A, 0x063D },
            { 0x0019, 0x063D },
        },
    }
};

static const struc_241 chara_some_data_array[] = {
    {
        -0.0610865242779255f, 0.104719758033752f, -0.0802851468324661f, 0.113446399569511f,
        -0.0436332300305367f, 0.0610865242779255f, -0.0802851468324661f, 0.113446399569511f,
        0.0148999998345971f, -0.0212999992072582f, 0.0f, 1.0f,
        1.0f, -3.8f, 6.0f, -3.8f,
        6.0f,
    },
    {
        -0.0610865242779255f, 0.104719758033752f, -0.0802851468324661f, 0.113446399569511f,
        -0.0750491619110107f, 0.104719758033752f, -0.0733038261532783f, 0.125663697719574f,
        0.033500000834465f, -0.0111999996006489f, 0.0f, 1.0f,
        1.0f, -5.5f, 10.0f, -5.5f,
        6.5f,
    },
    {
        -0.0610865242779255f, 0.104719758033752f, -0.0802851468324661f, 0.113446399569511f,
        -0.0837758034467697f, 0.0855211317539215f, -0.104719758033752f, 0.120427720248699f,
        0.0166999995708466f, -0.00540000014007092f, 0.0f, 1.0f,
        1.0f, -6.5f, 6.0f, -6.0f,
        2.0f,
    },
    {
        -0.0610865242779255f, 0.104719758033752f, -0.0802851468324661f, 0.113446399569511f,
        -0.0349065847694874f, 0.0698131695389748f, -0.0872664600610733f, 0.113446399569511f,
        0.011400000192225f, 0.00810000021010637f, 0.0f, 1.0f,
        1.0f, -3.5f, 6.0f, -3.5f,
        6.0f,
    },
    {
        -0.0610865242779255f, 0.104719758033752f, -0.0802851468324661f, 0.113446399569511f,
        -0.0349065847694874f, 0.0733038261532783f, -0.0907571166753769f, 0.13962633907795f,
        0.0151000004261732f, -0.0439999997615814f, 0.0f, 1.0f,
        1.0f, -2.6f, 6.0f, -2.6f,
        6.0f,
    },
    {
        -0.0610865242779255f, 0.104719758033752f, -0.0802851468324661f, 0.113446399569511f,
        -0.0523598790168762f, 0.122173048555851f, -0.0872664600610733f, 0.130899697542191f,
        0.0221999995410442f, -0.000199999994947575f, 0.0f, 1.0f,
        1.0f, -4.0f, 7.5f, -3.5f,
        6.0f,
    },
    {
        -0.0610865242779255f, 0.104719758033752f, -0.0802851468324661f, 0.113446399569511f,
        -0.0733038261532783f, 0.101229101419449f, -0.0785398185253143f, 0.113446399569511f,
        0.025000000372529f, -0.0230999998748302f, 0.0f, 1.0f,
        1.0f, -4.0f, 6.0f, -3.8f,
        3.5f,
    },
    {
        -0.0610865242779255f, 0.104719758033752f, -0.0802851468324661f, 0.113446399569511f,
        -0.0663225129246712f, 0.0593411959707737f, -0.0785398185253143f, 0.113446399569511f,
        0.0118000004440546f, -0.00510000018402934f, 0.0f, 1.0f,
        1.0f, -5.6f, 6.0f, -5.6f,
        5.5f,
    },
    {
        -0.0610865242779255f, 0.104719758033752f, -0.0802851468324661f, 0.113446399569511f,
        -0.0645771846175194f, 0.0663225129246712f, -0.0698131695389748f, 0.113446399569511f,
        0.0229000002145767f, -0.0148999998345971f, 0.0f, 1.0f,
        1.0f, -5.0f, 4.0f, -5.0f,
        2.5f,
    },
    {
        -0.0610865242779255f, 0.104719758033752f, -0.0802851468324661f, 0.113446399569511f,
        -0.0698131695389748f, 0.0698131695389748f, -0.113446399569511f, 0.122173048555851f,
        0.0137000000104308f, -0.00800000037997961f, 0.0f, 1.0f,
        1.0f, -5.5f, 6.0f, -5.0f,
        4.0f,
    },
};

static const skeleton_rotation_offset skeleton_rotation_offset_array[] = {
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 0, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 0, 0, 0, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, (float_t)(-M_PI * 2.0), 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 0, 0, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { (float_t)M_PI, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 1, { (float_t)-M_PI, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 0, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { (float_t)M_PI, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 1, { (float_t)-M_PI, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 0, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
};

static const ex_node_block_vtbl ex_constraint_block_vftable = {
    (void*)ex_constraint_block_dispose,
    //(void*)sub_1405F35F0,
    (void*)ex_constraint_block_set_default_data,
    //(void*)sub_1405F4960,
    (void*)ex_constraint_block_update,
    //(void*)sub_1405F2440,
    (void*)ex_constraint_block_draw,
    (void*)ex_node_block_reset,
    //(void*)sub_1405F2820,
    //(void*)sub_1405F4610,
    //(void*)sub_1405F4700,
    //(void*)sub_1405F48E0,
};

static const ex_node_block_vtbl ex_expression_block_vftable = {
    (void*)ex_expression_block_dispose,
    //(void*)sub_1405F3610,
    (void*)ex_expression_block_set_default_data,
    //(void*)sub_1405F49A0,
    (void*)ex_expression_block_update,
    //(void*)sub_1405F2450,
    (void*)ex_expression_block_draw,
    (void*)ex_node_block_reset,
    //(void*)sub_1405F2830,
    //(void*)sub_1405F4620,
    //(void*)sub_1405F4710,
    //(void*)sub_1405F48E0,
};

static const ex_node_block_vtbl ex_osage_block_vftable = {
    (void*)ex_osage_block_dispose,
    //(void*)sub_1405F3640,
    (void*)ex_node_block_set_default_data,
    //(void*)sub_1405F49F0,
    (void*)ex_osage_block_update,
    //(void*)sub_1405F2470,
    (void*)ex_osage_block_draw,
    (void*)ex_osage_block_reset,
    //(void*)sub_1405F2860,
    //(void*)sub_1405F4640,
    //(void*)sub_1405F4730,
    //(void*)sub_1405F48F0,
};

static const ex_node_block_vtbl ex_cloth_block_vftable = {
    (void*)ex_cloth_block_dispose,
    //(void*)sub_1405F35B0,
    (void*)ex_node_block_set_default_data,
    //(void*)sub_1405F4950,
    (void*)ex_cloth_block_update,
    //(void*)sub_1405F2420,
    (void*)ex_cloth_block_draw,
    (void*)ex_cloth_block_reset,
    //(void*)sub_1405F2810,
    //(void*)sub_1405F45F0,
    //(void*)sub_1405F46E0,
    //(void*)sub_1405F48E0,
};

chara_init_data* chara_init_data_get(chara_index chara_index) {
    if (chara_index >= CHARA_MIKU && chara_index <= CHARA_TETO)
        return (chara_init_data*)&chara_init_data_array[chara_index];
    return 0;
}

void motion_set_load_motion(uint32_t set, string* motion_name, motion_database* mot_db) {
    if (motion_storage_get_motion_set(set)) {
        motion_storage_append_mot_set(set);
        return;
    }

    motion_set_info* set_info = motion_database_get_motion_set_by_id(mot_db, set);
    if (!set_info)
        return;

    if (!motion_name)
        motion_name = &set_info->name;

    string file;
    string_init_length(&file, "mot_", 4);
    string_add_length(&file, string_data(motion_name), motion_name->length);
    string_add_length(&file, ".farc", 5);

    mot_set_farc msf;
    mot_init(&msf);
    data_struct_load_file(&data_list[DATA_AFT], &msf,
        "rom\\rob\\", string_data(&file), mot_load_file);
    if (msf.ready && msf.vec.end - msf.vec.begin > 0) {
        motion_storage_insert_motion_set(&msf.vec.begin[0], set);
        memset(&msf.vec.begin[0], 0, sizeof(mot_set));
    }
    mot_free(&msf);

    string_free(&file);
}

void motion_set_unload(uint32_t set) {
    motion_storage_delete_motion_set(set);
}

void rob_chara_data_init(rob_chara_data* rob_chr_data) {
    rob_chr_data->bone_data = rob_chara_data_bone_data_init();
    rob_chr_data->item_equip = rob_chara_data_item_equip_init();
    rob_chara_pv_data_init(&rob_chr_data->pv_data);
}

void rob_chara_data_calc(rob_chara_data* rob_chr_data) {
    if (rob_chara_data_bone_data_get_frame(rob_chr_data->bone_data) < 0.0)
        rob_chara_data_bone_data_set_frame(rob_chr_data->bone_data, 0.0);

    float_t frame = rob_chara_data_bone_data_get_frame(rob_chr_data->bone_data);
    float_t frame_count = rob_chara_data_bone_data_get_frame_count(rob_chr_data->bone_data);
    if (frame > frame_count)
        rob_chara_data_bone_data_set_frame(rob_chr_data->bone_data, frame_count);

    mat4_get_translation(&rob_chr_data->bone_data->mats.begin[MOTION_BONE_KL_KOSI_ETC_WJ],
        &rob_chr_data->item_equip->position);
    //rob_chara_data_get_trans_scale(rob_chr_data, 0, &rob_chr_data->item_equip->position);

    if (!rob_chr_data->item_equip->field_D8) {
        rob_chara_data_item_equip* rob_item_equip = rob_chr_data->item_equip;
        for (int32_t i = rob_item_equip->first_item_equip_object;
            i < rob_item_equip->max_item_equip_object; i++) {
            rob_chara_data_item_equip_object* itm_eq_obj = &rob_item_equip->item_equip_object[i];

            if (itm_eq_obj->field_B0 > 0)
                itm_eq_obj->field_B0 = 0;

            for (ex_node_block** i = itm_eq_obj->node_blocks.begin;
                i != itm_eq_obj->node_blocks.end; i++)
                (*i)->vftable->set_default_data(*i);

            if (itm_eq_obj->field_1C8)
                itm_eq_obj->field_1C8 = 0;
            else {
                for (ex_node_block** i = itm_eq_obj->node_blocks.begin;
                    i != itm_eq_obj->node_blocks.end; i++)
                    (*i)->vftable->update(*i);
            }
        }
    }

    /*if (rob_chara_data_check_for_ageageagain_module(rob_chr_data->chara_index, rob_chr_data->module_index)) {
        sub_140543520(rob_chr_data->chara_id, 1, rob_chr_data->item_equip->osage_phys_step);
        sub_14054FC90(rob_chr_data, 1u, "j_tail_l_006_wj");
        sub_140543520(rob_chr_data->chara_id, 2, rob_chr_data->item_equip->osage_phys_step);
        sub_14054FC90(rob_chr_data, 2u, "j_tail_r_006_wj");
    }*/
}

void rob_chara_data_draw(rob_chara_data* rob_chr_data, render_context* rctx) {
    rob_chr_data->item_equip->texture_color_coeff = vec4u_identity;
    rob_chr_data->item_equip->mat = mat4u_identity;
    rob_chara_data_item_equip_draw(rob_chr_data->item_equip, rob_chr_data->chara_id, rctx);
}

mat4* rob_chara_data_get_bone_data_mat(rob_chara_data* chara, size_t index) {
    return rob_chara_data_bone_data_get_mat(chara->bone_data, index);
}

float_t rob_chara_data_get_frame(rob_chara_data* rob_chr_data) {
    return rob_chara_data_bone_data_get_frame(rob_chr_data->bone_data);
}

float_t rob_chara_data_get_frame_count(rob_chara_data* rob_chr_data) {
    return rob_chara_data_bone_data_get_frame_count(rob_chr_data->bone_data);
}

void rob_chara_data_load_motion(rob_chara_data* rob_chr_data, int32_t motion_id,
    int32_t index, bone_database* bone_data, motion_database* mot_db) {
    rob_chara_data_bone_data_motion_load(rob_chr_data->bone_data, motion_id, index, bone_data, mot_db);
}

void rob_chara_data_reload_items(rob_chara_data* rob_chr_data,
    item_sub_data* sub_data, bone_database* bone_data) {
    static const item_sub_id array0[] = {
        ITEM_SUB_ZUJO,
        ITEM_SUB_KAMI,
        ITEM_SUB_HITAI,
        ITEM_SUB_ME,
        ITEM_SUB_MEGANE,
        ITEM_SUB_MIMI,
        ITEM_SUB_KUCHI,
        ITEM_SUB_MAKI,
        ITEM_SUB_KUBI,
        ITEM_SUB_INNER,
        ITEM_SUB_JOHA_MAE,
        ITEM_SUB_JOHA_USHIRO,
        ITEM_SUB_HADA,
        ITEM_SUB_U_UDE,
        ITEM_SUB_L_UDE,
        ITEM_SUB_TE,
        ITEM_SUB_BELT,
        ITEM_SUB_ASI,
        ITEM_SUB_SUNE,
        ITEM_SUB_HEAD,
        ITEM_SUB_MAX,
    };

    static const item_sub_id array1[] = {
        ITEM_SUB_KATA,
        ITEM_SUB_PANTS,
        ITEM_SUB_MAX,
    };

    static const item_sub_id array2[] = {
        ITEM_SUB_COSI,
        ITEM_SUB_OUTER,
        ITEM_SUB_MAX,
    };

    static const item_sub_id array3[] = {
        ITEM_SUB_KUTSU,
        ITEM_SUB_MAX
    };

    static const item_id array4[] = {
        ITEM_ZUJO,
        ITEM_KAMI,
        ITEM_NONE,
        ITEM_NONE,
        ITEM_MEGANE,
        ITEM_NONE,
        ITEM_KUCHI,
        ITEM_NONE,
        ITEM_KUBI,
        ITEM_NONE,
        ITEM_OUTER,
        ITEM_NONE,
        ITEM_NONE,
        ITEM_NONE,
        ITEM_NONE,
        ITEM_NONE,
        ITEM_JOHA_USHIRO,
        ITEM_NONE,
        ITEM_NONE,
        ITEM_PANTS,
        ITEM_NONE,
        ITEM_NONE,
        ITEM_NONE,
        ITEM_NONE,
        ITEM_NONE,
    };

    rob_chara_data_item_equip* rob_item_equip = rob_chr_data->item_equip;
    object_info base_face = chara_init_data_array[rob_chr_data->chara_index].base_face;
    rob_chara_data_item_equip_load_object_info(rob_item_equip, base_face, ITEM_ATAMA, bone_data);

    for (int32_t i = 0; array0[i] != ITEM_SUB_MAX; i++) {
        item_sub_id sub_id = array0[i];
        item_id id = array4[sub_id];

        object_info obj_info = object_info_null;
        int32_t no = sub_data->data[sub_id];
        if (sub_id == ITEM_SUB_HEAD) {
            if (no == 981)
                obj_info = (object_info){ 912, 3797 };
        }
        else if (sub_id == ITEM_SUB_KAMI) {
            if (id == ITEM_NONE)
                continue;

            if (no == 500)
                obj_info = (object_info){ 84, 6 };
            else if (no == 681)
                obj_info = (object_info){ 913, 3796 };
        }
        rob_chara_data_item_equip_load_object_info(rob_item_equip, obj_info, id, bone_data);
    }

    for (int32_t i = 0; array1[i] != ITEM_SUB_MAX; i++) {
        item_sub_id sub_id = array1[i];

        object_info obj_info = object_info_null;
        int32_t no = sub_data->data[sub_id];
        if (sub_id == ITEM_SUB_KATA) {
            if (no == 301)
                obj_info = (object_info){ 306, 953 };
            else if (no == 181)
                obj_info = (object_info){ 915, 3794 };
            rob_chara_data_item_equip_load_object_info(rob_item_equip, obj_info, ITEM_ITEM09, bone_data);
        }
    }

    for (int32_t i = 0; array2[i] != ITEM_SUB_MAX; i++) {
        item_sub_id sub_id = array2[i];
        item_id id = array4[sub_id];
        if (id == ITEM_NONE)
            continue;

        object_info obj_info = object_info_null;
        int32_t no = sub_data->data[sub_id];
        if (sub_id == ITEM_SUB_OUTER)
            if (no == 1)
                obj_info = (object_info){ 7, 4 };
            else if (no == 481)
                obj_info = (object_info){ 914, 3795 };
        rob_chara_data_item_equip_load_object_info(rob_item_equip, obj_info, id, bone_data);
    }
}

void rob_chara_data_reset(rob_chara_data* rob_chr_data, rob_chara_pv_data* pv_data,
    bone_database* bone_data, motion_database* mot_db) {
    rob_chara_data_bone_data_reset(rob_chr_data->bone_data);
    rob_chara_data_bone_data_init_data(rob_chr_data->bone_data,
        BONE_DATABASE_SKELETON_COMMON, rob_chr_data->chara_init_data->skeleton_type, bone_data);
    rob_chara_data_item_equip_get_parent_bone_nodes(rob_chr_data->item_equip,
        rob_chara_data_bone_data_get_node(rob_chr_data->bone_data, 0), bone_data);
    rob_chara_data_bone_data_eyes_xrot_adjust(rob_chr_data->bone_data, chara_some_data_array, &pv_data->eyes_adjust);
    rob_chara_data_bone_data_get_ik_scale(rob_chr_data->bone_data, bone_data);
    rob_chara_data_load_default_motion(rob_chr_data, bone_data, mot_db);
    int32_t motion_id = 195; //rob_cmn_mottbl_get_motion_index(rob_chr_data, 0);
    rob_chara_data_load_motion(rob_chr_data, motion_id, 0, bone_data, mot_db);
    rob_chara_data_set_draw(rob_chr_data, ITEM_MAX, true);
}

void rob_chara_data_set(rob_chara_data* rob_chr_data, int8_t chara_id,
    chara_index chara_index, uint32_t module_index, rob_chara_pv_data* pv_data) {
    rob_chr_data->chara_id = chara_id;
    rob_chr_data->chara_index = chara_index;
    rob_chr_data->module_index = module_index;
    rob_chr_data->pv_data = *pv_data;
    rob_chr_data->chara_init_data = chara_init_data_get(chara_index);
}

void rob_chara_data_set_frame(rob_chara_data* rob_chr_data, float_t frame) {
    rob_chara_data_bone_data_set_frame(rob_chr_data->bone_data, frame);
    rob_chara_data_bone_data_interpolate(rob_chr_data->bone_data);
    rob_chara_data_bone_data_update(rob_chr_data->bone_data, 0);
}

void rob_chara_data_free(rob_chara_data* rob_chr_data) {
    if (rob_chr_data->bone_data)
        rob_chara_data_bone_data_dispose(rob_chr_data->bone_data);

    if (rob_chr_data->item_equip)
        rob_chara_data_item_equip_dispose(rob_chr_data->item_equip);
}

static bone_data* bone_data_init(bone_data* bone) {
    memset(bone, 0, sizeof(bone_data));
    bone->eyes_xrot_adjust_neg = 1.0f;
    bone->eyes_xrot_adjust_pos = 1.0f;
    return bone;
}

static float_t bone_data_limit_angle(float_t angle) {
    bool neg;
    if (angle < 0.0f) {
        angle = -angle;
        neg = true;
    }
    else
        neg = false;

    angle = fmodf(angle + (float_t)M_PI, (float_t)(M_PI * 2.0)) - (float_t)M_PI;
    if (neg)
        return -angle;
    else
        return angle;
}

static void bone_data_mult_0(bone_data* a1, int32_t skeleton_select) {
    if (a1->flags != 0)
        return;

    mat4 mat;
    if (a1->has_parent)
        mat = *a1->parent_mat;
    else
        mat = mat4_identity;

    if (a1->type == BONE_DATABASE_BONE_POSITION) {
        mat4_translate_mult(&mat, a1->trans.x, a1->trans.y, a1->trans.z, &mat);
        a1->rot_mat[0] = mat4u_identity;
    }
    else if (a1->type == BONE_DATABASE_BONE_TYPE_1) {
        mat4_mult_vec3_inv(&mat, &a1->trans, &a1->trans);
        mat4_translate_mult(&mat, a1->trans.x, a1->trans.y, a1->trans.z, &mat);
        a1->rot_mat[0] = mat4u_identity;
    }
    else {
        mat4 rot_mat;
        if (a1->type == BONE_DATABASE_BONE_POSITION_ROTATION) {
            mat4u_to_mat4(&a1->rot_mat[0], &rot_mat);
            mat4_rotate_z_mult(&rot_mat, a1->rotation.z, &rot_mat);
        }
        else {
            a1->trans = a1->base_translation[skeleton_select];
            mat4_rotate_z(a1->rotation.z, &rot_mat);
        }

        mat4_rotate_y_mult(&rot_mat, a1->rotation.y, &rot_mat);

        if (a1->motion_bone_index == MOTION_BONE_KL_EYE_L
            || a1->motion_bone_index == MOTION_BONE_KL_EYE_R) {
            if (a1->rotation.x > 0.0)
                a1->rotation.x *= a1->eyes_xrot_adjust_pos;
            else if (a1->rotation.x < 0.0)
                a1->rotation.x *= a1->eyes_xrot_adjust_neg;
        }

        mat4_rotate_x_mult(&rot_mat, a1->rotation.x, &rot_mat);
        mat4_translate_mult(&mat, a1->trans.x, a1->trans.y, a1->trans.z, &mat);
        mat4_mult(&rot_mat, &mat, &mat);
        mat4_to_mat4u(&rot_mat, &a1->rot_mat[0]);
    }

    *a1->node[0].mat = mat;

    bone_data_mult_ik(a1, skeleton_select);
}

static void bone_data_mult_1(bone_data* a1, mat4* parent_mat, bone_data* a3, bool solve_ik) {
    mat4 mat;
    mat4 rot_mat;
    if (a1->has_parent)
        mat = *a1->parent_mat;
    else
        mat = *parent_mat;

    if (a1->type != BONE_DATABASE_BONE_TYPE_1 && a1->type != BONE_DATABASE_BONE_POSITION) {
        if (a1->type != BONE_DATABASE_BONE_POSITION_ROTATION)
            a1->trans = a1->base_translation[1];

        mat4_translate_mult(&mat, a1->trans.x, a1->trans.y, a1->trans.z, &mat);
        if (solve_ik) {
            a1->node[0].exp_data.rotation = vec3_null;
            if (!a1->flags) {
                mat4u_to_mat4(&a1->rot_mat[0], &rot_mat);
                mat4_get_rotation(&rot_mat, &a1->node[0].exp_data.rotation);
            }
            else if (bone_data_mult_1_exp_data(a1, &a1->node[0].exp_data, a3)) {
                vec3 rotation = a1->node[0].exp_data.rotation;
                mat4_rotate(rotation.x, rotation.y, rotation.z, &rot_mat);
                mat4_to_mat4u(&rot_mat, &a1->rot_mat[0]);
            }
            else {
                *a1->node[0].mat = mat;

                if (bone_data_mult_1_ik(a1, a3)) {
                    mat4_invrot_normalized(&mat, &rot_mat);
                    mat4_mult(a1->node[0].mat, &rot_mat, &rot_mat);
                    mat4_clear_trans(&rot_mat, &rot_mat);
                    mat4_get_rotation(&rot_mat, &a1->node[0].exp_data.rotation);
                    mat4_to_mat4u(&rot_mat, &a1->rot_mat[0]);
                }
                else
                    a1->rot_mat[0] = mat4u_identity;
            }
        }

        mat4u_to_mat4(&a1->rot_mat[0], &rot_mat);
        mat4_mult(&rot_mat, &mat, &mat);
    }
    else {
        mat4_translate_mult(&mat, a1->trans.x, a1->trans.y, a1->trans.z, &mat);
        if (solve_ik)
            a1->node[0].exp_data.rotation = vec3_null;
    }

    *a1->node[0].mat = mat;
    if (solve_ik) {
        a1->node[0].exp_data.position = a1->trans;
        bone_node_expression_data_reset_scale(&a1->node[0].exp_data);
    }

    if (a1->type < BONE_DATABASE_BONE_HEAD_IK_ROTATION)
        return;

    mat4u_to_mat4(&a1->rot_mat[1], &rot_mat);
    mat4_mult(&rot_mat, &mat, &mat);
    *a1->node[1].mat = mat;
    mat4_translate_mult(&mat, a1->ik_segment_length[1], 0.0f, 0.0f, &mat);

    if (a1->type == BONE_DATABASE_BONE_HEAD_IK_ROTATION) {
        *a1->node[2].mat = mat;
        if (!solve_ik)
            return;

        a1->node[1].exp_data.position = vec3_null;
        mat4u_to_mat4(&a1->rot_mat[1], &rot_mat);
        mat4_get_rotation(&rot_mat, &a1->node[1].exp_data.rotation);
        bone_node_expression_data_reset_scale(&a1->node[1].exp_data);
        bone_node_expression_data_set_position_rotation(&a1->node[2].exp_data,
            a1->ik_segment_length[1], 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    }
    else {
        mat4u_to_mat4(&a1->rot_mat[2], &rot_mat);
        mat4_mult(&rot_mat, &mat, &mat);
        *a1->node[2].mat = mat;
        mat4_translate_mult(&mat, a1->ik_2nd_segment_length[1], 0.0f, 0.0f, &mat);
        *a1->node[3].mat = mat;
        if (!solve_ik)
            return;

        a1->node[1].exp_data.position = vec3_null;
        mat4u_to_mat4(&a1->rot_mat[1], &rot_mat);
        mat4_get_rotation(&rot_mat, &a1->node[1].exp_data.rotation);
        bone_node_expression_data_reset_scale(&a1->node[1].exp_data);
        a1->node[2].exp_data.position.x = a1->ik_segment_length[1];
        *(vec2*)&a1->node[2].exp_data.position.y = vec2_null;
        mat4u_to_mat4(&a1->rot_mat[2], &rot_mat);
        mat4_get_rotation(&rot_mat, &a1->node[2].exp_data.rotation);
        bone_node_expression_data_reset_scale(&a1->node[2].exp_data);
        bone_node_expression_data_set_position_rotation(&a1->node[3].exp_data,
            a1->ik_2nd_segment_length[1], 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    }
}

static bool bone_data_mult_1_ik(bone_data* a1, bone_data* a2) {
    bone_node* v5;
    vec3 v30;

    switch (a1->motion_bone_index) {
    case MOTION_BONE_N_SKATA_L_WJ_CD_EX:
        v5 = a2[MOTION_BONE_C_KATA_L].node;
        mat4_get_translation(v5[2].mat, &v30);
        bone_data_mult_1_ik_hands(a1, &v30);
        break;
    case MOTION_BONE_N_SKATA_R_WJ_CD_EX:
        v5 = a2[MOTION_BONE_C_KATA_R].node;
        mat4_get_translation(v5[2].mat, &v30);
        bone_data_mult_1_ik_hands(a1, &v30);
        break;
    case MOTION_BONE_N_SKATA_B_L_WJ_CD_CU_EX:
        v5 = a2[MOTION_BONE_N_UP_KATA_L_EX].node;
        mat4_get_translation(v5[0].mat, &v30);
        bone_data_mult_1_ik_hands_2(a1, &v30, 0.333f);
        break;
    case MOTION_BONE_N_SKATA_B_R_WJ_CD_CU_EX:
        v5 = a2[MOTION_BONE_N_UP_KATA_R_EX].node;
        mat4_get_translation(v5[0].mat, &v30);
        bone_data_mult_1_ik_hands_2(a1, &v30, 0.333f);
        break;
    case MOTION_BONE_N_SKATA_C_L_WJ_CD_CU_EX:
        v5 = a2[MOTION_BONE_N_UP_KATA_L_EX].node;
        mat4_get_translation(v5[0].mat, &v30);
        bone_data_mult_1_ik_hands_2(a1, &v30, 0.5f);
        break;
    case MOTION_BONE_N_SKATA_C_R_WJ_CD_CU_EX:
        v5 = a2[MOTION_BONE_N_UP_KATA_R_EX].node;
        mat4_get_translation(v5[0].mat, &v30);
        bone_data_mult_1_ik_hands_2(a1, &v30, 0.5f);
        break;
    case MOTION_BONE_N_MOMO_A_L_WJ_CD_EX:
        v5 = a2[MOTION_BONE_CL_MOMO_L].node;
        mat4_get_translation(v5[2].mat, &v30);
        mat4_mult_vec3_inv(a1->node[0].mat, &v30, &v30);
        vec3_negate(v30, v30);
        bone_data_mult_1_ik_legs(a1, &v30);
        break;
    case MOTION_BONE_N_MOMO_A_R_WJ_CD_EX:
        v5 = a2[MOTION_BONE_CL_MOMO_R].node;
        mat4_get_translation(v5[2].mat, &v30);
        mat4_mult_vec3_inv(a1->node[0].mat, &v30, &v30);
        vec3_negate(v30, v30);
        bone_data_mult_1_ik_legs(a1, &v30);
        break;
    case MOTION_BONE_N_HARA_CD_EX:
        v5 = a2[MOTION_BONE_KL_MUNE_B_WJ].node;
        mat4_get_translation(v5[0].mat, &v30);
        mat4_mult_vec3_inv(a1->node[0].mat, &v30, &v30);
        bone_data_mult_1_ik_legs(a1, &v30);
        break;
    default:
        return false;
    }
    return true;
}

static void bone_data_mult_1_ik_hands(bone_data* a1, vec3* a2) {
    vec3 v15;
    mat4_mult_vec3_inv(a1->node[0].mat, a2, &v15);

    float_t len;
    vec3_length_squared(v15, len);
    if (len <= 0.000001f)
        return;

    len = sqrtf(len);
    if (len != 0.0f)
        vec3_mult_scalar(v15, 1.0f / len, v15);

    vec3 v17;
    v17.x = -v15.z * v15.x - v15.z;
    v17.y = -v15.y * v15.z;
    v17.z = v15.x * v15.x + v15.y * v15.y + v15.x;
    vec3_length_squared(v17, len);
    if (len <= 0.000001f)
        return;

    len = sqrtf(len);
    if (len != 0.0f)
        vec3_mult_scalar(v17, 1.0f / len, v17);

    vec3 v16;
    vec3_cross(v17, v15, v16);

    mat4 rot_mat = mat4_identity;
    *(vec3*)&rot_mat.row0 = v15;
    *(vec3*)&rot_mat.row1 = v16;
    *(vec3*)&rot_mat.row2 = v17;

    mat4_mult(&rot_mat, a1->node[0].mat, a1->node[0].mat);
}

static void bone_data_mult_1_ik_hands_2(bone_data* a1, vec3* a2, float_t a3) {
    float_t len;
    vec3 v8;

    mat4_mult_vec3_inv(a1->node[0].mat, a2, &v8);
    v8.x = 0.0f;
    vec3_length(v8, len);
    if (len <= 0.000001f)
        return;

    vec3_mult_scalar(v8, 1.0f / len, v8);
    float_t angle = atan2f(v8.z, v8.y);
    mat4_rotate_x_mult(a1->node[0].mat, angle * a3, a1->node[0].mat);
}

static void bone_data_mult_1_ik_legs(bone_data* a1, vec3* a2) {
    vec3 v9 = *a2;

    float_t len;
    vec3_length_squared(v9, len);
    if (len <= 0.000001f)
        return;

    len = sqrtf(len);
    if (len != 0.0f)
        vec3_mult_scalar(v9, 1.0f / len, v9);

    vec3 v8;
    v8.x = -v9.z * v9.x;
    v8.y = -v9.z * v9.y - v9.z;
    v8.z = v9.y * v9.y - v9.x * -v9.x + v9.y;

    vec3_length_squared(v8, len);
    if (len <= 0.000001f)
        return;

    len = sqrtf(len);
    if (len != 0.0f)
        vec3_mult_scalar(v8, 1.0f / len, v8);

    vec3 v10;
    vec3_cross(v9, v8, v10);

    mat4 rot_mat = mat4_identity;
    *(vec3*)&rot_mat.row0 = v10;
    *(vec3*)&rot_mat.row1 = v9;
    *(vec3*)&rot_mat.row2 = v8;

    mat4_mult(&rot_mat, a1->node[0].mat, a1->node[0].mat);
}

static bool bone_data_mult_1_exp_data(bone_data* a1, bone_node_expression_data* a2, bone_data* a3) {
    float_t v10;
    bone_node* v11;
    vec3 v15;
    mat4 dst;
    mat4 mat;

    bool ret = true;
    switch (a1->motion_bone_index) {
    case MOTION_BONE_N_EYE_L_WJ_EX:
        a2->rotation.x = -a3[MOTION_BONE_KL_EYE_L].node[0].exp_data.rotation.x;
        a2->rotation.y = a3[MOTION_BONE_KL_EYE_L].node[0].exp_data.rotation.y * (float_t)(-1.0 / 2.0);
        break;
    case MOTION_BONE_N_EYE_R_WJ_EX:
        a2->rotation.x = -a3[MOTION_BONE_KL_EYE_R].node[0].exp_data.rotation.x;
        a2->rotation.y = a3[MOTION_BONE_KL_EYE_R].node[0].exp_data.rotation.y * (float_t)(-1.0 / 2.0);
        break;
    case MOTION_BONE_N_KUBI_WJ_EX:
        mat4u_to_mat4(&a3[MOTION_BONE_CL_KAO].rot_mat[0], &dst);
        mat4u_to_mat4(&a3[MOTION_BONE_CL_KAO].rot_mat[1], &mat);
        mat4_mult(&mat, &dst, &dst);
        mat4_get_rotation(&dst, &v15);
        v10 = v15.z;
        if (v10 < (float_t)-M_PI_2)
            v10 += (float_t)(M_PI * 2.0);
        a2->rotation.y = (v10 - (float_t)M_PI_2) * 0.2f;
        break;
    case MOTION_BONE_N_HITO_L_EX:
    case MOTION_BONE_N_HITO_R_EX:
        a2->rotation.z = (float_t)(-9.0 * DEG_TO_RAD);
        break;
    case MOTION_BONE_N_KO_L_EX:
        a2->rotation.x = (float_t)(-8.0 * DEG_TO_RAD);
        a2->rotation.z = (float_t)(16.0 * DEG_TO_RAD);
        break;
    case MOTION_BONE_N_KUSU_L_EX:
    case MOTION_BONE_N_KUSU_R_EX:
        a2->rotation.z = (float_t)(9.0 * DEG_TO_RAD);
        break;
    case MOTION_BONE_N_NAKA_L_EX:
    case MOTION_BONE_N_NAKA_R_EX:
        a2->rotation.z = (float_t)(-1.0 * DEG_TO_RAD);
        break;
    case MOTION_BONE_N_OYA_L_EX:
        a2->rotation.x = (float_t)(75.0 * DEG_TO_RAD);
        a2->rotation.y = (float_t)(12.0 * DEG_TO_RAD);
        a2->rotation.z = (float_t)(-24.0 * DEG_TO_RAD);
        break;
    case MOTION_BONE_N_STE_L_WJ_EX:
        a2->rotation.x = a3[MOTION_BONE_KL_TE_L_WJ].node[0].exp_data.rotation.x;
        break;
    case MOTION_BONE_N_SUDE_L_WJ_EX:
    case MOTION_BONE_N_SUDE_B_L_WJ_EX:
        v11 = &a3[MOTION_BONE_KL_TE_L_WJ].node[0];
        a2->rotation.x = bone_data_limit_angle(v11->exp_data.rotation.x) * (float_t)(1.0 / 3.0);
        break;
    case MOTION_BONE_N_SUDE_R_WJ_EX:
    case MOTION_BONE_N_SUDE_B_R_WJ_EX:
        v11 = &a3[MOTION_BONE_KL_TE_R_WJ].node[0];
        a2->rotation.x = bone_data_limit_angle(v11->exp_data.rotation.x) * (float_t)(1.0 / 3.0);
        break;
    case MOTION_BONE_N_HIJI_L_WJ_EX:
        a2->rotation.z = a3[MOTION_BONE_C_KATA_L].node[2].exp_data.rotation.z * (float_t)(1.0 / 2.0);
        break;
    case MOTION_BONE_N_UP_KATA_L_EX:
    case MOTION_BONE_N_UP_KATA_R_EX:
        break;
    case MOTION_BONE_N_KO_R_EX:
        a2->rotation.x = (float_t)(8.0 * DEG_TO_RAD);
        a2->rotation.z = (float_t)(16.0 * DEG_TO_RAD);
        break;
    case MOTION_BONE_N_OYA_R_EX:
        a2->rotation.x = (float_t)(-75.0 * DEG_TO_RAD);
        a2->rotation.y = (float_t)(-12.0 * DEG_TO_RAD);
        a2->rotation.z = (float_t)(-24.0 * DEG_TO_RAD);
        break;
    case MOTION_BONE_N_STE_R_WJ_EX:
        a2->rotation.x = a3[MOTION_BONE_KL_TE_R_WJ].node[0].exp_data.rotation.x;
        break;
    case MOTION_BONE_N_HIJI_R_WJ_EX:
        a2->rotation.z = a3[MOTION_BONE_C_KATA_R].node[2].exp_data.rotation.z * (float_t)(1.0 / 2.0);
        break;
    case MOTION_BONE_N_HIZA_L_WJ_EX:
        a2->rotation.z = a3[MOTION_BONE_CL_MOMO_L].node[2].exp_data.rotation.z * (float_t)(1.0 / 2.0);
        break;
    case MOTION_BONE_N_HIZA_R_WJ_EX:
        a2->rotation.z = a3[MOTION_BONE_CL_MOMO_R].node[2].exp_data.rotation.z * (float_t)(1.0 / 2.0);
        break;
    case MOTION_BONE_N_MOMO_B_L_WJ_EX:
    case MOTION_BONE_N_MOMO_C_L_WJ_EX:
        v11 = &a3[MOTION_BONE_CL_MOMO_L].node[0];
        a2->rotation.y = bone_data_limit_angle(v11->exp_data.rotation.y
            + v11->exp_data.rotation.x) * (float_t)(1.0 / 3.0);
        break;
    case MOTION_BONE_N_MOMO_B_R_WJ_EX:
    case MOTION_BONE_N_MOMO_C_R_WJ_EX:
        v11 = &a3[MOTION_BONE_CL_MOMO_R].node[0];
        a2->rotation.y = bone_data_limit_angle(v11->exp_data.rotation.y
            + v11->exp_data.rotation.x) * (float_t)(1.0 / 3.0);
        break;
    case MOTION_BONE_N_HARA_B_WJ_EX:
        a2->rotation.y = a3[MOTION_BONE_CL_MUNE].node[0].exp_data.rotation.y * (float_t)(1.0 / 3.0)
            + a3[MOTION_BONE_KL_KOSI_Y].node[0].exp_data.rotation.y * (float_t)(2.0 / 3.0);
        break;
    case MOTION_BONE_N_HARA_C_WJ_EX:
        a2->rotation.y = a3[MOTION_BONE_CL_MUNE].node[0].exp_data.rotation.y * (float_t)(2.0 / 3.0)
            + a3[MOTION_BONE_KL_KOSI_Y].node[0].exp_data.rotation.y * (float_t)(1.0 / 3.0);
        break;
    default:
        ret = false;
        break;
    }
    return ret;
}

static void bone_data_mult_ik(bone_data* a1, int32_t skeleton_select) {
    if (a1->type < BONE_DATABASE_BONE_HEAD_IK_ROTATION)
        return;

    mat4 mat = *a1->node[0].mat;

    vec3 v30;
    mat4_mult_vec3_inv(&mat, &a1->ik_target, &v30);
    float_t v6;
    float_t v8;
    vec2_length_squared(*(vec2*)&v30, v6);
    vec3_length_squared(v30, v8);
    float_t v9 = sqrtf(v6);
    float_t v10 = sqrtf(v8);
    mat4 rot_mat;
    if (v9 > 0.000001f && v8 > 0.000001f) {
        mat4_rotate_z_sin_cos(v30.y / v9, v30.x / v9, &rot_mat);
        mat4_rotate_y_mult_sin_cos(&rot_mat, -v30.z / v10, v9 / v10, &rot_mat);
        mat4_mult(&rot_mat, &mat, &mat);
    }
    else
        rot_mat = mat4_identity;

    if (a1->pole_target_mat) {
        vec3 pole_target;
        mat4_get_translation(a1->pole_target_mat, &pole_target);
        mat4_mult_vec3_inv(&mat, &pole_target, &pole_target);
        float_t pole_target_length;
        vec2_length(*(vec2*)&pole_target.y, pole_target_length);
        if (pole_target_length > 0.000001f) {
            vec3_mult_scalar(pole_target, 1.0f / pole_target_length, pole_target);
            float_t rot_cos = pole_target.y;
            float_t rot_sin = pole_target.z;
            mat4_rotate_x_mult_sin_cos(&mat, rot_sin, rot_cos, &mat);
            mat4_rotate_x_mult_sin_cos(&rot_mat, rot_sin, rot_cos, &rot_mat);
        }
    }

    if (a1->type == BONE_DATABASE_BONE_HEAD_IK_ROTATION) {
        mat4_to_mat4u(&rot_mat, &a1->rot_mat[1]);
        *a1->node[1].mat = mat;
        mat4_translate_mult(&mat, a1->ik_segment_length[skeleton_select], 0.0f, 0.0f, &mat);
        *a1->node[2].mat = mat;
        return;
    }

    float_t ik_segment_length = a1->ik_segment_length[skeleton_select];
    float_t ik_2nd_segment_length = a1->ik_2nd_segment_length[skeleton_select];
    float_t rot_sin;
    float_t rot_cos;
    float_t rot_2nd_sin;
    float_t rot_2nd_cos;
    if (fabsf(v8) > 0.000001f) {
        float_t v26 = a1->field_2E0;
        if (v26 > 0.0001f) {
            float_t v27 = (ik_segment_length + ik_2nd_segment_length) * v26;
            if (v10 > v27) {
                v10 = v27;
                v8 = v27 * v27;
            }
        }
        float_t v28 = (v8 - ik_2nd_segment_length * ik_2nd_segment_length) / ik_segment_length;
        rot_cos = (v28 + ik_segment_length) / (2.0f * v10);
        rot_2nd_cos = (v28 - ik_segment_length) / (2.0f * ik_2nd_segment_length);

        rot_cos = clamp(rot_cos, -1.0f, 1.0f);
        rot_2nd_cos = clamp(rot_2nd_cos, -1.0f, 1.0f);

        rot_sin = sqrtf(1.0f - rot_cos * rot_cos);
        rot_2nd_sin = sqrtf(1.0f - rot_2nd_cos * rot_2nd_cos);
        if (a1->type == BONE_DATABASE_BONE_LEGS_IK_ROTATION)
            rot_sin = -rot_sin;
        else
            rot_2nd_sin = -rot_2nd_sin;

        float_t arot_sin = asinf(rot_sin) * RAD_TO_DEG_FLOAT;
        float_t arot_cos = acosf(rot_cos) * RAD_TO_DEG_FLOAT;
        float_t arot_2nd_sin = asinf(rot_2nd_sin) * RAD_TO_DEG_FLOAT;
        float_t arot_2nd_cos = acosf(rot_2nd_cos) * RAD_TO_DEG_FLOAT;
    }
    else {
        rot_sin = 0.0f;
        rot_cos = 1.0f;
        rot_2nd_sin = 0.0f;
        rot_2nd_cos = -1.0f;
    }

    mat4_rotate_z_mult_sin_cos(&mat, rot_sin, rot_cos, &mat);
    *a1->node[1].mat = mat;
    mat4_rotate_z_mult_sin_cos(&rot_mat, rot_sin, rot_cos, &rot_mat);
    mat4_to_mat4u(&rot_mat, &a1->rot_mat[1]);
    mat4_translate_mult(&mat, ik_segment_length, 0.0f, 0.0f, &mat);
    mat4_rotate_z_mult_sin_cos(&mat, rot_2nd_sin, rot_2nd_cos, &mat);
    *a1->node[2].mat = mat;
    mat4_rotate_z_sin_cos(rot_2nd_sin, rot_2nd_cos, &rot_mat);
    mat4_to_mat4u(&rot_mat, &a1->rot_mat[2]);
    mat4_translate_mult(&mat, ik_2nd_segment_length, 0.0f, 0.0f, &mat);
    *a1->node[3].mat = mat;
}

static vec3* bone_data_set_key_data(bone_data* data, vec3* keyframe_data,
    bone_database_skeleton_type skeleton_type, bool get_data, bool reverse_x) {
    bone_database_bone_type type = data->type;
    if (type == BONE_DATABASE_BONE_POSITION_ROTATION) {
        if (get_data) {
            data->trans = *keyframe_data;
            if (reverse_x)
                data->trans.x = -data->trans.x;
        }
        keyframe_data++;
    }
    else if (type >= BONE_DATABASE_BONE_HEAD_IK_ROTATION
        && type <= BONE_DATABASE_BONE_LEGS_IK_ROTATION) {
        if (get_data) {
            data->ik_target = *keyframe_data;
            if (reverse_x)
                data->ik_target.x = -data->ik_target.x;
        }
        keyframe_data++;
    }

    if (get_data) {
        if (type == BONE_DATABASE_BONE_TYPE_1 || type == BONE_DATABASE_BONE_POSITION) {
            data->trans = *keyframe_data;
            if (reverse_x)
                data->trans.x = -data->trans.x;
        }
        else if (!data->flags) {
            data->rotation = *keyframe_data;
            if (reverse_x) {
                const skeleton_rotation_offset* rot_off = skeleton_rotation_offset_array;
                size_t index = data->motion_bone_index;
                if (rot_off[index].x)
                    data->rotation.x = rot_off[index].rotation.x - data->rotation.x;
                if (rot_off[index].y)
                    data->rotation.y = rot_off[index].rotation.y - data->rotation.y;
                if (rot_off[index].z)
                    data->rotation.z = rot_off[index].rotation.z - data->rotation.z;
            }
        }
    }
    keyframe_data++;
    return keyframe_data;
}

static void bone_data_parent_data_init(bone_data_parent* bone,
    rob_chara_data_bone_data* rob_bone_data, bone_database* bone_data) {
    char* base_name = bone_database_skeleton_type_to_string(rob_bone_data->base_skeleton_type);
    char* name = bone_database_skeleton_type_to_string(rob_bone_data->skeleton_type);
    vector_bone_database_bone* common_bones = 0;
    vector_vec3* common_translation = 0;
    vector_vec3* translation = 0;
    if (!bone_database_get_skeleton_bones(bone_data, base_name, &common_bones)
        || !bone_database_get_skeleton_positions(bone_data, base_name, &common_translation)
        || !bone_database_get_skeleton_positions(bone_data, name, &translation))
        return;

    bone->rob_bone_data = rob_bone_data;
    bone_data_parent_load_rob_chara_data(bone);
    bone_data_parent_load_bone_database(bone, common_bones, common_translation->begin, translation->begin);
}

static void bone_data_parent_load_bone_database(bone_data_parent* bone,
    vector_bone_database_bone* bones, vec3* common_translation, vec3* translation) {
    rob_chara_data_bone_data* rob_bone_data = bone->rob_bone_data;
    size_t chain_pos = 0;
    size_t total_bone_count = 0;
    size_t ik_bone_count = 0;

    bone_data* v16 = bone->bones.begin;
    for (bone_database_bone* i = bones->begin; i != bones->end; i++, v16++) {
        v16->motion_bone_index = (motion_bone_index)(i - bones->begin);
        v16->type = i->type;
        v16->mirror = i->mirror;
        v16->parent = i->parent;
        v16->flags = i->flags;
        if (i->type >= BONE_DATABASE_BONE_POSITION_ROTATION)
            v16->key_set_count = 6;
        else
            v16->key_set_count = 3;
        v16->base_translation[0] = *common_translation++;
        v16->base_translation[1] = *translation++;
        v16->node = &rob_bone_data->nodes.begin[total_bone_count];
        v16->has_parent = i->has_parent;
        if (i->has_parent)
            v16->parent_mat = &rob_bone_data->mats.begin[i->parent];
        else
            v16->parent_mat = 0;

        v16->pole_target_mat = 0;
        switch (i->type) {
        case BONE_DATABASE_BONE_ROTATION:
        case BONE_DATABASE_BONE_TYPE_1:
        case BONE_DATABASE_BONE_POSITION:
            chain_pos++;
            total_bone_count++;
            break;
        case BONE_DATABASE_BONE_POSITION_ROTATION:
            chain_pos++;
            ik_bone_count++;
            total_bone_count++;
            break;
        case BONE_DATABASE_BONE_HEAD_IK_ROTATION:
            v16->ik_segment_length[0] = (common_translation++)->x;
            v16->ik_segment_length[1] = (translation++)->x;

            chain_pos += 2;
            ik_bone_count++;
            total_bone_count += 3;
            break;
        case BONE_DATABASE_BONE_ARM_IK_ROTATION:
        case BONE_DATABASE_BONE_LEGS_IK_ROTATION:
            v16->ik_segment_length[0] = (common_translation++)->x;
            v16->ik_segment_length[1] = (translation++)->x;
            v16->ik_2nd_segment_length[0] = (common_translation++)->x;
            v16->ik_2nd_segment_length[1] = (translation++)->x;

            mat4* pole_target = 0;
            if (i->pole_target)
                pole_target = &rob_bone_data->mats.begin[i->pole_target];
            v16->pole_target_mat = pole_target;

            chain_pos += 3;
            ik_bone_count++;
            total_bone_count += 4;
            break;
        }
    }
}

static void bone_data_parent_load_bone_indices_from_mot(bone_data_parent* a1,
    mot_data* a2, bone_database* a3, motion_database* mot_db) {
    if (!a2)
        return;

    vector_uint16_t* bone_indices = &a1->bone_indices;
    vector_uint16_t_clear(bone_indices, 0);

    uint16_t key_set_count = (a2->info & 0x3FFF) - 1;
    if (!key_set_count)
        return;

    bone_database_skeleton_type skeleton_type = a1->rob_bone_data->base_skeleton_type;
    char* skeleton_type_string = bone_database_skeleton_type_to_string(skeleton_type);
    string* bone_names = mot_db->bone_name.begin;

    mot_bone_info* bone_info = a2->bone_info;
    for (size_t key_set_offset= 0, i = 0; key_set_offset< key_set_count; i++) {
        int32_t bone_index = bone_database_get_skeleton_bone_index(a3,
            skeleton_type_string, string_data(&bone_names[bone_info[i].index]));
        if (bone_index == -1) {
            bone_index = bone_database_get_skeleton_bone_index(a3,
                skeleton_type_string, string_data(&bone_names[bone_info[++i].index]));
            if (bone_index == -1)
                break;
        }
        *vector_uint16_t_reserve_back(bone_indices) = (uint16_t)bone_index;

        bone_data* v15 = &a1->bones.begin[bone_index];
        v15->key_set_offset = (int32_t)key_set_offset;
        v15->frame = -1.0f;
        if (v15->type >= BONE_DATABASE_BONE_POSITION_ROTATION)
            key_set_offset+= 6;
        else
            key_set_offset+= 3;
    }
}

static void bone_data_parent_load_rob_chara_data(bone_data_parent* bone) {
    rob_chara_data_bone_data* rob_bone_data = bone->rob_bone_data;
    if (!rob_bone_data)
        return;

    bone->motion_bone_count = rob_bone_data->motion_bone_count;
    bone->ik_bone_count = rob_bone_data->ik_bone_count;
    bone->chain_pos = rob_bone_data->chain_pos;

    size_t bones_size = rob_bone_data->motion_bone_count;
    vector_bone_data_reserve(&bone->bones, bones_size);
    bone->bones.end += bones_size;

    for (bone_data* i = bone->bones.begin; i != bone->bones.end; i++)
        bone_data_init(i);

    bone->global_key_set_count = 6;
    bone->bone_key_set_count = (uint32_t)((bone->motion_bone_count + bone->ik_bone_count) * 3);
}

static float_t* bone_node_get_exp_data_component(bone_node* a1,
    size_t index, ex_expression_block_stack_type* type) {
    switch (index) {
    case 0:
        *type = EX_EXPRESSION_BLOCK_STACK_VARIABLE;
        return &a1->exp_data.position.x;
    case 1:
        *type = EX_EXPRESSION_BLOCK_STACK_VARIABLE;
        return &a1->exp_data.position.y;
    case 2:
        *type = EX_EXPRESSION_BLOCK_STACK_VARIABLE;
        return &a1->exp_data.position.z;
    case 3:
        *type = EX_EXPRESSION_BLOCK_STACK_VARIABLE_RADIAN;
        return &a1->exp_data.rotation.x;
    case 4:
        *type = EX_EXPRESSION_BLOCK_STACK_VARIABLE_RADIAN;
        return &a1->exp_data.rotation.y;
    case 5:
        *type = EX_EXPRESSION_BLOCK_STACK_VARIABLE_RADIAN;
        return &a1->exp_data.rotation.z;
    case 6:
        *type = EX_EXPRESSION_BLOCK_STACK_VARIABLE;
        return &a1->exp_data.scale.x;
    case 7:
        *type = EX_EXPRESSION_BLOCK_STACK_VARIABLE;
        return &a1->exp_data.scale.y;
    case 8:
        *type = EX_EXPRESSION_BLOCK_STACK_VARIABLE;
        return &a1->exp_data.scale.z;
    default:
        return 0;
    }
}

static void bone_data_parent_reset(bone_data_parent* bone) {
    bone->rob_bone_data = 0;
    bone->motion_bone_count = 0;
    bone->global_trans = vec3_null;
    bone->global_rotation = vec3_null;
    bone->ik_bone_count = 0;
    bone->chain_pos = 0;
    bone->bone_key_set_count = 0;
    bone->global_key_set_count = 0;
    bone->rotation_y = 0;
    bone->bones.end = bone->bones.begin;
    bone->bone_indices.end = bone->bone_indices.begin;
}

static void bone_data_parent_free(bone_data_parent* bone) {
    vector_uint16_t_free(&bone->bone_indices, 0);
    vector_bone_data_free(&bone->bones, 0);
}

static void bone_node_expression_data_mat_set(bone_node_expression_data* data,
    vec3* data_scale, mat4* ex_data_mat, mat4* mat) {
    vec3 position;
    vec3_mult(data->position, *data_scale, position);
    mat4_translate_mult( ex_data_mat, position.x, position.y, position.z, ex_data_mat);
    vec3 rotation = data->rotation;
    mat4_rotate_mult(ex_data_mat, rotation.x, rotation.y, rotation.z, ex_data_mat);
    vec3_mult(data->scale, *data_scale, data->data_scale);
    *mat = *ex_data_mat;
    mat4_scale_rot(mat, data->data_scale.x, data->data_scale.y, data->data_scale.z, mat);
}

static void bone_node_expression_data_reset_position_rotation(bone_node_expression_data* data) {
    data->position = vec3_null;
    data->rotation = vec3_null;
    data->scale = vec3_identity;
    data->data_scale = vec3_identity;
}

static void bone_node_expression_data_reset_scale(bone_node_expression_data* data) {
    data->scale = vec3_identity;
    data->data_scale = vec3_identity;
}

static void bone_node_expression_data_set_position_rotation(bone_node_expression_data* data,
    float_t position_x, float_t position_y, float_t position_z,
    float_t rotation_x, float_t rotation_y, float_t rotation_z) {
    data->position.x = position_x;
    data->position.y = position_y;
    data->position.z = position_z;
    data->rotation.x = rotation_x;
    data->rotation.z = rotation_z;
    data->rotation.y = rotation_y;
    data->scale = vec3_identity;
    data->data_scale = vec3_identity;
}

static void bone_node_expression_data_set_position_rotation_vec3(bone_node_expression_data* data,
    vec3* position, vec3* rotation) {
    data->position = *position;
    data->rotation = *rotation;
    data->scale = vec3_identity;
    data->data_scale = vec3_identity;
}

static ex_cloth_block* ex_cloth_block_init() {
    ex_cloth_block* cls = force_malloc(sizeof(ex_cloth_block));
    cls->base.vftable = &ex_cloth_block_vftable;
    cls->base.parent_name = string_empty;
    return cls;
}

static void ex_cloth_block_draw(ex_cloth_block* cls) {

}

static void ex_cloth_block_reset(ex_cloth_block* cls) {

}

static void ex_cloth_block_update(ex_cloth_block* cls) {

}

static void ex_cloth_block_dispose(ex_cloth_block* cls) {
    ex_node_block_free(&cls->base);
    free(cls);
}

static ex_constraint_block* ex_constraint_block_init() {
    ex_constraint_block* cns = force_malloc(sizeof(ex_constraint_block));
    cns->base.vftable = &ex_constraint_block_vftable;
    cns->base.parent_name = string_empty;
    return cns;
}

static void ex_constraint_block_init_data(ex_constraint_block* cns, rob_chara_data_item_equip_object* itm_eq_obj,
    object_skin_block_constraint* cns_data, char* cns_data_name, bone_database* bone_data) {
    bone_node* node = rob_chara_data_item_equip_object_get_bone_node_by_name(
        itm_eq_obj, cns_data_name, bone_data);
    ex_node_block_init_data(&cns->base, node, EX_CONSTRAINT, node->name, itm_eq_obj);
    cns->cns_data = cns_data;

    cns->source_node_bone_node = rob_chara_data_item_equip_object_get_bone_node_by_name(
        itm_eq_obj, string_data(&cns_data->source_node_name), bone_data);

    object_skin_block_constraint_type type = cns_data->type;
    char* up_vector_name;
    if (type == OBJECT_SKIN_BLOCK_CONSTRAINT_DIRECTION) {
        cns->type = OBJECT_SKIN_BLOCK_CONSTRAINT_DIRECTION;
        up_vector_name = string_data(&cns_data->direction.up_vector.name);
    }
    else if (type == OBJECT_SKIN_BLOCK_CONSTRAINT_POSITION) {
        cns->type = OBJECT_SKIN_BLOCK_CONSTRAINT_POSITION;
        up_vector_name = string_data(&cns_data->position.up_vector.name);
    }
    else if (type == OBJECT_SKIN_BLOCK_CONSTRAINT_DISTANCE) {
        cns->type = OBJECT_SKIN_BLOCK_CONSTRAINT_DISTANCE;
        up_vector_name = string_data(&cns_data->distance.up_vector.name);
    }
    else {
        cns->type = OBJECT_SKIN_BLOCK_CONSTRAINT_NONE;
        if (type == OBJECT_SKIN_BLOCK_CONSTRAINT_ORIENTATION)
            cns->type = OBJECT_SKIN_BLOCK_CONSTRAINT_ORIENTATION;
        return;
    }

    if (up_vector_name)
        cns->direction_up_vector_bone_node = rob_chara_data_item_equip_object_get_bone_node_by_name(
            itm_eq_obj, up_vector_name, bone_data);
}

static void ex_constraint_block_data_set(ex_constraint_block* cns) {
    if (!cns->base.bone_node)
        return;

    bone_node* node = cns->base.bone_node;
    bone_node_expression_data* exp_data = &node->exp_data;
    bone_node* parent_node = cns->base.parent_bone_node;
    vec3 data_scale = parent_node->exp_data.data_scale;

    mat4 mat;
    mat4_inverse_normalized(parent_node->ex_data_mat, &mat);
    mat4_mult(node->mat, &mat, &mat);
    mat4_get_rotation(&mat, &node->exp_data.rotation);
    mat4_get_translation(&mat, &exp_data->position);
    if (fabsf(data_scale.x) > 0.000001f)
        exp_data->position.x /= data_scale.x;
    if (fabsf(data_scale.y) > 0.000001f)
        exp_data->position.y /= data_scale.z;
    if (fabsf(data_scale.z) > 0.000001f)
        exp_data->position.z /= data_scale.z;
    *node->ex_data_mat = *node->mat;
    mat4_scale_rot(node->mat, data_scale.x, data_scale.y, data_scale.z, node->mat);
    vec3_mult(exp_data->scale, data_scale, exp_data->data_scale);
}

static void ex_constraint_block_draw(ex_constraint_block* cns) {

}

static void ex_constraint_block_set_default_data(ex_constraint_block* cns) {
    if (cns->base.bone_node) {
        bone_node_expression_data* exp_data = &cns->base.bone_node->exp_data;
        object_skin_block_constraint* cns_data = cns->cns_data;
        exp_data->position = cns_data->base.position;
        exp_data->rotation = cns_data->base.rotation;
        exp_data->scale = cns_data->base.scale;
    }
    cns->base.set_data = false;
}

static void ex_constraint_block_time_step(ex_constraint_block* cns) {
}

static void ex_constraint_block_update(ex_constraint_block* cns) {
    if (!cns->base.parent_bone_node)
        return;

    if (cns->base.set_data)
        cns->base.set_data = false;
    else {
        ex_constraint_block_time_step(cns);
        ex_constraint_block_data_set(cns);
        cns->base.set_data = true;
    }
}

static void ex_constraint_block_dispose(ex_constraint_block* cns) {
    ex_node_block_free(&cns->base);
    free(cns);
}

static ex_expression_block* ex_expression_block_init() {
    ex_expression_block* exp = force_malloc(sizeof(ex_expression_block));
    exp->base.vftable = &ex_expression_block_vftable;
    exp->base.parent_name = string_empty;
    return exp;
}

static void ex_expression_block_init_data(ex_expression_block* exp,
    rob_chara_data_item_equip_object* itm_eq_obj, object_skin_block_expression* exp_data,
    char* exp_data_name, object_info a4, chara_index chara_index, bone_database* bone_data) {
    ex_expression_block_stack* stack_buf[28];
    ex_expression_block_stack** stack_buf_val = stack_buf;

    bone_node* node = rob_chara_data_item_equip_object_get_bone_node_by_name(
        itm_eq_obj, exp_data_name, bone_data);
    ex_node_block_init_data(&exp->base, node, EX_EXPRESSION, node->name, itm_eq_obj);

    exp->exp_data = exp_data;
    node->exp_data.position = exp_data->base.position;
    node->exp_data.rotation = exp_data->base.rotation;
    node->exp_data.scale = exp_data->base.scale;
    exp->field_3D28 = 0;

    ex_expression_block_stack* stack_val = exp->stack_data;
    for (int32_t i = 0; i < 9; i++) {
        exp->values[i] = 0;
        exp->expressions[i] = 0;
    }

    for (int32_t i = 0; i < 9; i++) {
        char* expression = string_data(&exp_data->expressions[i]);
        if (!expression || str_utils_compare_length(expression, exp_data->expressions[i].length, "= ", 2))
            break;

        expression += 2;

        int32_t index = 0;
        expression = str_utils_get_next_int32_t(expression, &index, ' ');
        exp->values[i] = bone_node_get_exp_data_component(exp->base.bone_node, index, &exp->types[i]);

        while (expression) {
            string v73;
            expression = str_utils_get_next_string(expression, &v73, ' ');
            if (!v73.length || !memcmp(string_data(&v73), "error", min(v73.length, 5)) && v73.length == 5) {
                string_free(&v73);
                break;
            }

            if (*string_data(&v73) == 'n') {
                stack_val->type = EX_EXPRESSION_BLOCK_STACK_NUMBER;
                expression = str_utils_get_next_float_t(expression, &stack_val->number.value, ' ');
                *stack_buf_val++ = stack_val;
            }
            else if (*string_data(&v73) == 'v') {
                string func_str;
                expression = str_utils_get_next_string(expression, &func_str, ' ');
                stack_val->type = EX_EXPRESSION_BLOCK_STACK_VARIABLE;
                int32_t v40 = *string_data(&func_str) - '0';
                if (v40 < 9) {
                    bone_node* v42 = rob_chara_data_item_equip_object_get_bone_node_by_name(itm_eq_obj,
                        string_data(&func_str) + 2, bone_data);
                    if (v42)
                        stack_val->var.value = bone_node_get_exp_data_component(v42, v40, &stack_val->type);
                    else {
                        stack_val->type = EX_EXPRESSION_BLOCK_STACK_NUMBER;
                        stack_val->number.value = 0.0f;
                    }
                }
                else
                    stack_val->var.value = &exp->frame;
                *stack_buf_val++ = stack_val;
                string_free(&func_str);
            }
            else if (*string_data(&v73) == 'f') {
                string func_str;
                expression = str_utils_get_next_string(expression, &func_str, ' ');
                stack_val->type = EX_EXPRESSION_BLOCK_STACK_OP1;
                stack_val->op1.v1 = stack_buf_val[-1];
                stack_val->op1.func = exp_func_op1_find_func(&func_str, exp_func_op1_array)->func;
                stack_buf_val[-1] = stack_val;
                string_free(&func_str);
            }
            else if (*string_data(&v73) == 'g') {
                string func_str;
                expression = str_utils_get_next_string(expression, &func_str, ' ');
                stack_val->type = EX_EXPRESSION_BLOCK_STACK_OP2;
                stack_val->op2.v1 = stack_buf_val[-2];
                stack_val->op2.v2 = stack_buf_val[-1];
                stack_val->op2.func = exp_func_op2_find_func(&func_str, exp_func_op2_array)->func;
                stack_buf_val[-2] = stack_val;
                stack_buf_val--;
                string_free(&func_str);
            }
            else if (*string_data(&v73) == 'h') {
                string func_str;
                expression = str_utils_get_next_string(expression, &func_str, ' ');
                stack_val->type = EX_EXPRESSION_BLOCK_STACK_OP3;
                stack_val->op3.v1 = stack_buf_val[-3];
                stack_val->op3.v2 = stack_buf_val[-2];
                stack_val->op3.v3 = stack_buf_val[-1];
                stack_val->op3.func = exp_func_op3_find_func(&func_str, exp_func_op3_array)->func;
                stack_buf_val[-3] = stack_val;
                stack_buf_val -= 2;
                string_free(&func_str);
            }
            string_free(&v73);
            exp->expressions[i] = stack_buf_val[-1];
            stack_val++;
        }
    }

    if (chara_index >= CHARA_MIKU && chara_index <= CHARA_TETO)
        exp->osage_phys_step = true;
}

static void ex_expression_block_data_set(ex_expression_block* exp) {
    bone_node* node = exp->base.bone_node;
    bone_node* parent_node = exp->base.parent_bone_node;
    bone_node_expression_data* data = &node->exp_data;
    vec3 data_scale = exp->base.parent_bone_node->exp_data.data_scale;
    mat4 ex_data_mat = *parent_node->ex_data_mat;
    mat4 mat = mat4_identity;
    bone_node_expression_data_mat_set(data, &data_scale, &ex_data_mat, &mat);
    *node->mat = mat;
    *node->ex_data_mat = ex_data_mat;
}

static void ex_expression_block_draw(ex_expression_block* exp) {

}

static void ex_expression_block_set_default_data(ex_expression_block* exp) {
    bone_node_expression_data* exp_data = &exp->base.bone_node->exp_data;
    object_skin_block_expression* exp_exp_data = exp->exp_data;
    exp_data->position = exp_exp_data->base.position;
    exp_data->rotation = exp_exp_data->base.rotation;
    exp_data->scale = exp_exp_data->base.scale;
    exp->base.set_data = false;
}

static void ex_expression_block_time_step(ex_expression_block* exp) {
    float_t delta_frame = get_delta_frame();
    if (exp->osage_phys_step)
        delta_frame *= exp->base.item_equip_object->item_equip->osage_phys_step;
    float_t frame = exp->frame + delta_frame;
    exp->frame = frame >= 65535.0f ? frame - 65535.0f : frame;

    if (exp->field_3D28)
        exp->field_3D28(&exp->base.bone_node->exp_data);
    else
        for (int32_t i = 0; i < 9; i++) {
            if (!exp->expressions[i])
                break;

            *exp->values[i] = ex_expression_block_stack_get_value(exp->expressions[i]);
            if (exp->types[i] == EX_EXPRESSION_BLOCK_STACK_VARIABLE_RADIAN)
                *exp->values[i] *= DEG_TO_RAD_FLOAT;
        }
}

static void ex_expression_block_update(ex_expression_block* exp) {
    if (!exp->base.parent_bone_node)
        return;

    if (exp->base.set_data)
        exp->base.set_data = false;
    else {
        ex_expression_block_time_step(exp);
        ex_expression_block_data_set(exp);
        exp->base.set_data = true;
    }
}

static void ex_expression_block_dispose(ex_expression_block* exp) {
    ex_node_block_free(&exp->base);
    free(exp);
}

static float_t ex_expression_block_stack_get_value(ex_expression_block_stack* stack) {
    float_t value = 0.0f;
    switch (stack->type) {
    case EX_EXPRESSION_BLOCK_STACK_NUMBER:
        value = stack->number.value;
        break;
    case EX_EXPRESSION_BLOCK_STACK_VARIABLE:
        value = *stack->var.value;
        break;
    case EX_EXPRESSION_BLOCK_STACK_VARIABLE_RADIAN:
        value = *stack->var_rad.value * RAD_TO_DEG_FLOAT;
        break;
    case EX_EXPRESSION_BLOCK_STACK_OP1: {
        float_t v1 = ex_expression_block_stack_get_value(stack->op1.v1);
        if (stack->op1.func)
            value = stack->op1.func(v1);
    } break;
    case EX_EXPRESSION_BLOCK_STACK_OP2: {
        float_t v1 = ex_expression_block_stack_get_value(stack->op2.v1);
        float_t v2 = ex_expression_block_stack_get_value(stack->op2.v2);
        if (stack->op2.func)
            value = stack->op2.func(v1, v2);
    } break;
    case EX_EXPRESSION_BLOCK_STACK_OP3: {
        float_t v1 = ex_expression_block_stack_get_value(stack->op1.v1);
        float_t v2 = ex_expression_block_stack_get_value(stack->op2.v2);
        float_t v3 = ex_expression_block_stack_get_value(stack->op3.v3);
        if (stack->op3.func)
            value = stack->op3.func(v1, v2, v3);
    } break;
    }
    return value;
}

static void ex_node_block_init_data(ex_node_block* node, bone_node* bone_node,
    ex_node_type type, char* name, rob_chara_data_item_equip_object* itm_eq_obj) {
    node->bone_node = bone_node;
    node->type = type;
    if (name)
        node->name = name;
    else
        node->name = "(null)";
    node->parent_bone_node = 0;
    node->item_equip_object = itm_eq_obj;
}

static void ex_node_block_reset(ex_node_block* node) {
    node->bone_node = 0;
}

static void ex_node_block_set_default_data(ex_node_block* node) {
    node->set_data = false;
}

static void ex_node_block_free(ex_node_block* node) {
    string_free(&node->parent_name);
}

static ex_osage_block* ex_osage_block_init() {
    ex_osage_block* osg = force_malloc(sizeof(ex_osage_block));
    osg->base.vftable = &ex_osage_block_vftable;
    osg->base.parent_name = string_empty;
    return osg;
}

static void rob_osage_init(rob_osage* rob_osg) {
    vector_rob_osage_node_clear(&rob_osg->nodes, rob_osage_node_free);
    rob_osg->nodes.end = rob_osg->nodes.begin;
    rob_osage_node_init(&rob_osg->node);
    rob_osg->wind_direction = vec3_null;
    rob_osg->wind_direction.z = 0;
    rob_osg->field_1EB4 = 0.0f;
    rob_osg->field_1EB8 = 0;
    rob_osg->field_2A0 = true;
    /*tree_295* v2 = &rob_osg->field_1F60;
    sub_140216750(&rob_osg->field_1F60, rob_osg->field_1F60.head->parent);
    v2->head->parent = v2->head;
    v2->head->left = v2->head;
    v2->head->right = v2->head;
    rob_osg->field_1F60.size = 0;*/
    rob_osg->field_1F08 = 0.0;
    rob_osg->field_1F0C = false;
    rob_osg->field_1F0D = false;
    rob_osg->field_1F0E = false;
    rob_osg->ring.ring_rectangle_x = 0.0f;
    rob_osg->ring.ring_rectangle_y = 0.0f;
    rob_osg->ring.ring_rectangle_width = 0.0f;
    rob_osg->ring.ring_rectangle_height = 0.0f;
    rob_osg->ring.ring_height = -1000.0f;
    rob_osg->ring.ring_out_height = -1000.0f;
    rob_osg->ring.field_18 = 0;
    rob_osg->ring.coli.end = rob_osg->ring.coli.begin;
    rob_osg->ring.skp_root_coli.end = rob_osg->ring.skp_root_coli.begin;
    rob_osg->field_1F0F = 0;
    skin_param_init(&rob_osg->skin_param);
    rob_osg->skin_param_ptr = &rob_osg->skin_param;
    rob_osg->field_290 = -1;
    rob_osg->field_298 = 0;
    rob_osg->field_1F70 = 0;
    rob_osg->field_2A4 = 0.0f;
    rob_osg->field_2A1 = false;
    rob_osg->field_1F78 = 0;
    rob_osg->field_1F7C = vec3_null;
    rob_osg->mats = 0;
    rob_osg->mat = mat4u_null;
}

static void rob_osage_init_data(rob_osage* rob_osg,
    object_skin_block_osage* osg_data, object_skin_osage_node* osg_nodes,
    bone_node* ex_data_bone_nodes, object_skin* skin) {
    rob_osage_init(rob_osg);
    bone_node_expression_data_set_position_rotation_vec3(
        &rob_osg->exp_data,
        &osg_data->base.position,
        &osg_data->base.rotation);
    vector_rob_osage_node* nodes = &rob_osg->nodes;
    vector_rob_osage_node_clear(&rob_osg->nodes, 0);
    vector_rob_osage_node_reserve(&rob_osg->nodes, osg_data->count + 1ULL);
    rob_osg->nodes.end += osg_data->count + 1ULL;

    rob_osage_node* external = nodes->begin;
    rob_osage_node_init(nodes->begin);
    external->child_length = osg_nodes->length;
    bone_node* external_bone_node = &ex_data_bone_nodes[osg_data->external_name_index & 0x7FFF];
    external->bone_node = external_bone_node;
    external->bone_node_mat = external_bone_node->mat;
    *external_bone_node->ex_data_mat = mat4_identity;
    external->sibling_node = 0;

    bone_node* parent_bone_node = external->bone_node;
    for (int32_t i = 0; i < osg_data->count; i++) {
        object_skin_osage_node* v14 = &osg_nodes[i];
        rob_osage_node* node = &nodes->begin[i + 1];
        rob_osage_node_init(node);
        node->length = v14[0].length;
        if (i != osg_data->count - 1)
            node->child_length = v14[1].length;
        bone_node* node_bone_node = &ex_data_bone_nodes[v14->name_index & 0x7FFF];
        node->bone_node = node_bone_node;
        node->bone_node_mat = node_bone_node->mat;
        node_bone_node->parent = parent_bone_node;
        parent_bone_node = node->bone_node;
    }

    rob_osage_node_init(&rob_osg->node);
    rob_osg->node.length = osg_nodes[osg_data->count - 1].length;
    bone_node* v22 = &ex_data_bone_nodes[osg_data->name_index & 0x7FFF];
    rob_osg->node.bone_node = v22;
    rob_osg->node.bone_node_mat = v22->mat;
    v22->parent = parent_bone_node;
    *rob_osg->node.bone_node->ex_data_mat = mat4_identity;

    object_skin_osage_node* v23 = osg_data->nodes;
    for (int32_t i = 0; i < osg_data->count; i++) {
        rob_osage_node* v26 = &nodes->begin[i + 1];
        vec3 v27 = vec3_null;
        if (v23)
            v27 = v23[i].rotation;

        mat4 mat;
        mat4_rotate(v27.x, v27.y, v27.z, &mat);
        v26->field_94 = vec3_null;
        v26->field_94.x = v26->length;
        mat4_mult_vec3(&mat, &v26->field_94, &v26->field_94);

        v26->mat = mat4u_null;
        object_skin_bone* v36 = skin->bones;
        for (int32_t j = 0; j < skin->bones_count; j++)
            if (v36[j].id == osg_nodes->name_index) {
                mat4 mat;
                mat4u_to_mat4(&v36[j].inv_bind_pose_mat, &mat);
                mat4_inverse_normalized(&mat, &mat);
                mat4_to_mat4u(&mat, &v26->mat);
            }
    }

    rob_osage_node* v38 = &rob_osg->nodes.end[-1];
    if (!memcmp(&v38->mat, &mat4_null, sizeof(mat4))) {
        mat4 mat;
        mat4u_to_mat4(&v38->mat, &mat);
        mat4_translate_mult(&mat, rob_osg->node.length, 0.0f, 0.0f, &mat);
        mat4_to_mat4u(&mat, &rob_osg->node.mat);
    }
}

static void skin_param_hinge_limit(skin_param_hinge* hinge) {
    hinge->ymin = max(hinge->ymin, -179.0f) * DEG_TO_RAD_FLOAT;
    hinge->ymax = min(hinge->ymax, 179.0f) * DEG_TO_RAD_FLOAT;
    hinge->zmin = max(hinge->zmin, -179.0f) * DEG_TO_RAD_FLOAT;
    hinge->zmax = min(hinge->zmax, 179.0f) * DEG_TO_RAD_FLOAT;
}

static void rob_osage_node_data_init(rob_osage_node_data* node_data) {
    node_data->force = 0.0f;
    node_data->boc.end = node_data->boc.begin;
    node_data->skp_osg_node.coli_r = 0.0f;
    node_data->skp_osg_node.weight = 1.0f;
    node_data->skp_osg_node.inertial_cancel = 0.0f;
    node_data->skp_osg_node.hinge = (skin_param_hinge){ -90.0f, 90.0f, -90.0f, 90.0f };
    skin_param_hinge_limit(&node_data->skp_osg_node.hinge);
    node_data->normal_ref.field_0 = 0;
    node_data->normal_ref.n = 0;
    node_data->normal_ref.u = 0;
    node_data->normal_ref.d = 0;
    node_data->normal_ref.l = 0;
    node_data->normal_ref.r = 0;
    node_data->normal_ref.mat = mat4u_identity;
}

static void ex_osage_block_draw(ex_osage_block* osg) {

}

static void ex_osage_block_init_data(ex_osage_block* osg,
    rob_chara_data_item_equip_object* itm_eq_obj, object_skin_block_osage* osg_data,
    char* osg_data_name, object_skin_osage_node* osg_nodes, bone_node* bone_nodes,
    bone_node* ex_data_bone_nodes, object_skin* skin) {
    ex_node_block_init_data(&osg->base,
        &ex_data_bone_nodes[osg_data->external_name_index & 0x7FFF], EX_OSAGE, osg_data_name, itm_eq_obj);
    rob_osage_init_data(&osg->rob, osg_data, osg_nodes, ex_data_bone_nodes, skin);
    osg->field_1FF8 &= ~2;
    osg->mat = bone_nodes->mat;
}

static void sub_1405F3E10(ex_osage_block* osg, object_skin_block_osage* osg_data,
    object_skin_osage_node* osg_nodes, vector_pair_int32_t_ptr_rob_osage_node* a4,
    vector_pair_name_ex_osage_block* a5) {
    rob_osage* v5 = &osg->rob;

    rob_osage_node* v9 = rob_osage_get_node(&osg->rob, 0);
    pair_int32_t_ptr_rob_osage_node* v38
        = vector_pair_int32_t_ptr_rob_osage_node_reserve_back(a4);
    v38->key = osg_data->external_name_index;
    v38->value = v9;

    for (size_t i = 0; i < osg_data->count; i++) {
        v9 = rob_osage_get_node(v5, i);
        v38 = vector_pair_int32_t_ptr_rob_osage_node_reserve_back(a4);
        v38->key = osg_nodes[i].name_index;
        v38->value = v9;

        if (v9->bone_node && v9->bone_node->name) {
            pair_name_ex_osage_block* v39 = vector_pair_name_ex_osage_block_reserve_back(a5);
            v39->name = v9->bone_node->name;
            v39->block = osg;
        }
    }

    v9 = rob_osage_get_node(v5, osg_data->count + 1ULL);
    v38 = vector_pair_int32_t_ptr_rob_osage_node_reserve_back(a4);
    v38->key = osg_data->name_index;
    v38->value = v9;

    if (v9->bone_node && v9->bone_node->name) {
        pair_name_ex_osage_block* v39 = vector_pair_name_ex_osage_block_reserve_back(a5);
        v39->name = v9->bone_node->name;
        v39->block = osg;
    }
}

static void ex_osage_block_reset(ex_osage_block* osg) {
    osg->index = 0;
    rob_osage_init(&osg->rob);
    osg->field_1FF8 &= ~3;
    osg->mat = 0;
    osg->osage_phys_step = 1.0f;
    ex_node_block_reset(&osg->base);
}

static void ex_osage_block_set_wind_direction(ex_osage_block* osg) {
    vec3_mult_scalar(wind_ptr->wind_direction,
        osg->base.item_equip_object->item_equip->wind_strength,
        osg->rob.wind_direction);
}

static void sub_14047F110(rob_osage* rob_osg, mat4* mat, vec3* data_scale, bool init_rot) {
    vec3 position;
    vec3_mult(rob_osg->exp_data.position, *data_scale, position);
    mat4_translate_mult(mat, position.x, position.y, position.z, mat);

    vec3 rotation = rob_osg->exp_data.rotation;
    mat4_rotate_mult(mat, rotation.x, rotation.y, rotation.z, mat);

    skin_param* skin_param = rob_osg->skin_param_ptr;
    vec3 rot = rob_osg->skin_param_ptr->rot;
    if (init_rot)
        vec3_add(rot, skin_param->init_rot, rot);
    mat4_rotate_mult(mat, rot.x, rot.y, rot.z, mat);
}

static bool sub_140482FF0(mat4* a1, vec3* a2, skin_param_hinge* hinge, vec3* a4, int32_t* a5) {
    bool clipped = false;
    float_t y;
    float_t z;
    if (*a5 == 1) {
        y = atan2f(-a2->z, a2->x);
        z = atan2f(a2->y, sqrtf(a2->x * a2->x + a2->z * a2->z));
        if (hinge) {
            clipped = false;
            if (y > hinge->ymax) {
                y = hinge->ymax;
                clipped = true;
            }

            if (y < hinge->ymin) {
                y = hinge->ymin;
                clipped = true;
            }

            if (z > hinge->zmax) {
                z = hinge->zmax;
                clipped = true;
            }

            if (z < hinge->zmin) {
                z = hinge->zmin;
                clipped = true;
            }
        }
        mat4_rotate_y_mult(a1, y, a1);
        mat4_rotate_z_mult(a1, z, a1);
    }
    else {
        z = atan2f(a2->y, a2->x);
        y = atan2f(-a2->z, sqrtf(a2->x * a2->x + a2->y * a2->y));
        if (hinge) {
            clipped = false;
            if (y > hinge->ymax) {
                y = hinge->ymax;
                clipped = true;
            }

            if (y < hinge->ymin) {
                y = hinge->ymin;
                clipped = true;
            }

            if (z > hinge->zmax) {
                z = hinge->zmax;
                clipped = true;
            }

            if (z < hinge->zmin) {
                z = hinge->zmin;
                clipped = true;
            }
        }
        mat4_rotate_z_mult(a1, z, a1);
        mat4_rotate_y_mult(a1, y, a1);
    }

    if (a4) {
        a4->y = y;
        a4->z = z;
    }
    return clipped;
}

static void sub_14047EE90(rob_osage* rob_osg, mat4* mat) {
    __int64*** v2 = (__int64***)rob_osg->field_1F70;
    if (v2) {
        __int64* v5 = **v2;
        rob_osage_node* v6 = rob_osg->nodes.begin + 1;
        if (v6 != rob_osg->nodes.end) {
            rob_osage_node* v7 = rob_osg->nodes.begin;
            do {
                v7->length = *((float_t*)v5 + 4);
                v7->trans = *(vec3*)((char*)v5 + 20);
                v7->field_10 = *(vec3*)(v5 + 4);
                v7->field_1C = *(vec3*)((char*)v5 + 44);
                ++v6;
                ++v7;
                v5 = (__int64*)*v5;
            } while (v6 != rob_osg->nodes.end);
        }
        rob_osg->field_1F70 = 0i64;
    }

    rob_osage_node* v8 = rob_osg->nodes.begin + 1;
    if (v8 != rob_osg->nodes.end) {
        rob_osage_node* v9 = &rob_osg->nodes.begin[1];
        do {
            mat4_mult_vec3(mat, &v9->field_AC, &v9->field_1C);
            mat4_mult_vec3_trans(mat, &v9->field_A0, &v9->trans);
            ++v8;
            ++v9;
        } while (v8 != rob_osg->nodes.end);
    }
    mat4_to_mat4u(rob_osg->mats, &rob_osg->mat);
}

static void sub_1404803B0(rob_osage* rob_osg, mat4* mat, vec3* data_scale, bool a4) {
    rob_osage_node* v6; // rdi
    rob_osage_node* v7; // rbx
    float_t v9; // xmm2_4
    skin_param* v15; // rax
    float_t v16; // xmm0_4
    float_t v17; // xmm7_4
    bool v18; // cc
    rob_osage_node* v19; // rbx
    rob_osage_node* v20; // rsi
    rob_osage_node* v29; // rbx
    rob_osage_node* v30; // rsi
    int* v31; // r14
    unsigned __int8 v32; // r13
    float_t v33; // xmm0_4
    float_t v34; // xmm2
    float_t v35; // xmm1_4
    bool v36; // r15
    rob_osage_node* v39; // rax
    vec3 src; // [rsp+40h] [rbp-C0h] BYREF
    vec3 v45; // [rsp+68h] [rbp-98h] BYREF
    vec3 dst; // [rsp+78h] [rbp-88h] BYREF
    mat4 v47; // [rsp+90h] [rbp-70h] BYREF

    v6 = rob_osg->nodes.end;
    v7 = rob_osg->nodes.begin;
    v9 = rob_osg->exp_data.position.y * data_scale->y;
    v47 = *mat;
    vec3_mult(rob_osg->exp_data.position, *data_scale, src);
    mat4_mult_vec3_trans(&v47, &src, &dst);
    v7->trans.x = dst.x;
    v7->trans.y = dst.y;
    v7->trans.z = dst.z;
    if (rob_osg->field_1F0D && !rob_osg->field_1F0E) {
        rob_osg->field_1F0E = true;
        sub_14047EE90(rob_osg, mat);
    }

    if (!rob_osg->field_1F0C) {
        v15 = rob_osg->skin_param_ptr;
        v16 = rob_osg->field_1F08;
        rob_osg->field_1F0C = true;
        v17 = v15->move_cancel;
        if (v16 == 1.0)
            v17 = v16;
        v18 = v17 <= 0.0;
        if (v17 < 0.0) {
            v17 = v16;
            v18 = v16 <= 0.0;
        }

        if (!v18) {
            v19 = rob_osg->nodes.begin + 1;
            if (v19 != v6) {
                v20 = rob_osg->nodes.begin + 1;
                do {
                    mat4 mat;
                    mat4u_to_mat4(&rob_osg->mat, &mat);
                    mat4_mult_vec3_inv(&mat, &v20->trans, &dst);
                    mat4_mult_vec3_trans(&rob_osg->mats[0], &dst, &v45);
                    vec3_sub(v45, v20->trans, v45);
                    vec3_mult_scalar(v45, v17, v45);
                    vec3_add(v20->trans, v45, v20->trans);
                    ++v19;
                    ++v20;
                } while (v19 != v6);
            }
        }
    }

    if (a4) {
        sub_14047F110(rob_osg, &v47, data_scale, false);
        *rob_osg->nodes.begin->bone_node_mat = v47;
        v29 = rob_osg->nodes.begin + 1;
        if (v29 != v6) {
            v30 = rob_osg->nodes.begin;
            v31 = &rob_osg->field_1EB8;
            do {
                mat4_mult_vec3_inv(&v47, &v30[1].trans, &v45);
                v32 = sub_140482FF0(&v47, &v45, &v30[1].data_ptr->skp_osg_node.hinge, &v30[1].field_B8, v31);
                *v30[1].bone_node->ex_data_mat = v47;
                v33 = v30[1].trans.y - v30->trans.y;
                v34 = v30[1].trans.x;
                v34 = (v34 - v30->trans.x) * (v34 - v30->trans.x)
                    + (v33 * v33)
                    + ((v30[1].trans.z - v30->trans.z)
                        * (v30[1].trans.z - v30->trans.z));
                v35 = data_scale->x * v29->length;
                if (v34 >= (v35 * v35)) {
                    v35 = sqrtf(v34);
                    v36 = 0;
                }
                else {
                    v36 = 1;
                }
                mat4_translate_mult(&v47, v35, 0.0, 0.0, &v47);
                if (v32 || v36)
                    mat4_get_translation(&v47, &v30[1].trans);
                ++v29;
                ++v30;
            } while (v29 != v6);
            rob_osg = rob_osg;
        }
        v39 = rob_osg->nodes.end;
        if (rob_osg->nodes.begin != v39 && rob_osg->node.bone_node_mat)
            mat4_translate_mult(v39[-1].bone_node->ex_data_mat,
                data_scale->x * rob_osg->node.length, 0.0f, 0.0f,
                rob_osg->node.bone_node->ex_data_mat);
    }
}

static void sub_140482300(vec3* a1, vec3* a2, vec3* a3, float_t osage_gravity_const, float_t weight) {
    float_t v5; // xmm3_4
    float_t v6; // xmm2
    float_t v7; // xmm6_4
    float_t v8; // xmm0_4
    float_t v9; // xmm5_4
    float_t v10; // xmm1_4
    float_t v11; // xmm2
    float_t v12; // [rsp+4h] [rbp-3Ch]
    float_t v13; // [rsp+4h] [rbp-3Ch]

    v5 = osage_gravity_const;
    v7 = osage_gravity_const * weight;
    v8 = a2->x - a3->x;
    v12 = a2->y - a3->y;
    v9 = a2->z - a3->z;
    v6 = v12 * v12 + v8 * v8 + v9 * v9;
    v10 = sqrtf(v6);
    if (v10 > fabsf(osage_gravity_const))
        v5 = v12;
    else {
        v11 = v8 * v8 + osage_gravity_const * osage_gravity_const + v9 * v9;
        v10 = sqrtf(v11);
    }
    v10 = 1.0f / v10;
    v13 = v5 * v10;
    a1->x = -v8 * v10 * v13 * v7;
    a1->y = -v7 - v13 * v13 * v7;
    a1->z = -v9 * v10 * v13 * v7;
}

static void sub_140482F30(vec3* a1, vec3* a2, float_t a3) {
    vec3 v5;
    vec3_sub(*a1, *a2, v5);

    float_t length;
    vec3_length_squared(v5, length);
    if (length > (a3 * a3)) {
        vec3_mult_scalar(v5, a3 / sqrtf(length), v5);
        vec3_add(*a2, v5, *a1);
    }
}

static void sub_140482490(rob_osage_node* node, float_t* osage_phys_step, float_t* a3) {
    if (*osage_phys_step != 1.0) {
        vec3 v4;
        vec3_sub(node->trans, node->field_10, v4);

        float_t length;
        vec3_length(v4, length);
        if (length != 0.0)
            vec3_mult_scalar(v4, 1.0f / length, v4);

        vec3_mult_scalar(v4, *osage_phys_step * length, v4);
        vec3_add(node->field_10, v4, node->trans);
    }
    sub_140482F30(&node->trans, &node[-1].trans, node->length * *a3);

    if (node->sibling_node)
        sub_140482F30(&node->trans, &node->sibling_node->trans, node->distance);

}

static void sub_140482180(rob_osage_node* node, float_t* a2) {
    float_t v2 = node->data_ptr->skp_osg_node.coli_r + *a2;
    if (v2 <= node->trans.y)
        return;

    node->trans.y = v2;

    vec3 v3;
    vec3_sub(node->trans, node[-1].trans, v3);

    float_t length;
    vec3_length_squared(v3, length);
    if (length > 0.000001f)
        vec3_mult_scalar(v3, 1.0f / sqrtf(length), v3);
    vec3_mult_scalar(v3, node->length, v3);
    vec3_add(node[-1].trans, v3, node->trans);
    node->field_1C = vec3_null;
    node->field_C8 += 1.0f;
}

static void sub_14047C800(rob_osage* rob_osg, mat4* mat, vec3* data_scale, float_t osage_phys_step, bool a5, bool a6, bool a7) {
    rob_osage_node* v7; // rax
    rob_osage_node* v12; // rdi
    float_t osage_gravity_const; // xmm0_4
    rob_osage_node* v17; // rcx
    vec4 v18; // xmm1
    vec4 v19; // xmm2
    rob_osage_node* v20; // rbx
    float_t v21; // xmm0_4
    rob_osage_node* v22; // rax
    float_t v25; // xmm9_4
    rob_osage_node* v26; // rbx
    bool v27; // r13
    float_t v28; // xmm12_4
    rob_osage_node* v30; // rsi
    rob_osage_node_data* v31; // r15
    float_t v32; // xmm8_4
    float_t v33; // xmm5_4
    float_t v34; // xmm6_4
    float_t v35; // xmm7_4
    float_t v36; // xmm2_4
    float_t v38; // xmm2_4
    skin_param* v39; // rax
    float_t v40; // xmm4_4
    float_t v41; // xmm3_4
    float_t v42; // xmm6_4
    float_t v43; // xmm7_4
    float_t v44; // xmm5_4
    float_t v45; // xmm0_4
    float_t v46; // xmm1_4
    float_t v47; // xmm3_4
    float_t v48; // xmm1_4
    float_t v49; // xmm2_4
    float_t v50; // xmm0_4
    float_t v51; // xmm1_4
    vec4 v52; // xmm2
    vec4 v53; // xmm3
    vec4 v54; // xmm4
    rob_osage_node* v55; // rbx
    vec4 v56; // xmm5
    float_t v57; // xmm12_4
    float_t v58; // er12
    vec2 v59; // r13
    rob_osage_node* v60; // rsi
    float_t v61; // eax
    float_t v62; // eax
    float_t v63; // eax
    float_t v64; // xmm3_4
    float_t v65; // xmm4_4
    float_t v68; // xmm8_4
    rob_osage_node_data* v70; // rcx
    float_t v71; // xmm0_4
    float_t v72; // xmm1_4
    float_t v73; // xmm0_4
    float_t v74; // xmm2_4
    rob_osage_node* v82; // rax
    rob_osage_node* v83; // rdx
    rob_osage_node* i; // rbx
    rob_osage_node* v91; // rbx
    float_t v92; // xmm4_4
    float_t v93; // xmm2_4
    float_t v94; // xmm1_4
    float_t v95; // xmm0_4
    float_t v96; // xmm3_4
    float_t v97; // xmm0_4
    rob_osage_node* v98; // rbx
    rob_osage_node* v99; // rbx
    rob_osage_node* v100; // rsi
    int* v101; // r14
    unsigned __int8 v102; // r13
    float_t v103; // xmm0_4
    float_t v104; // xmm2
    float_t v105; // xmm1_4
    char v106; // r12
    rob_osage_node* v109; // rax
    vec3 a1a; // [rsp+30h] [rbp-D0h] BYREF
    vec3 v112; // [rsp+40h] [rbp-C0h] BYREF
    vec3 src; // [rsp+50h] [rbp-B0h] BYREF
    vec2 v114; // [rsp+60h] [rbp-A0h]
    float_t v115; // [rsp+68h] [rbp-98h]
    vec3 v116; // [rsp+70h] [rbp-90h]
    vec3 v117; // [rsp+80h] [rbp-80h]
    float_t a3a; // [rsp+90h] [rbp-70h] BYREF
    vec3 v119; // [rsp+98h] [rbp-68h] BYREF
    vec3 v120; // [rsp+B0h] [rbp-50h]
    vec3 v126; // [rsp+E8h] [rbp-18h] BYREF
    vec3 dst; // [rsp+100h] [rbp+0h] BYREF
    vec3 v128; // [rsp+110h] [rbp+10h] BYREF
    vec3 a2a; // [rsp+120h] [rbp+20h] BYREF
    mat4 v130; // [rsp+130h] [rbp+30h] BYREF
    mat4 v131; // [rsp+170h] [rbp+70h] BYREF
    mat4 v132; // [rsp+1B0h] [rbp+B0h] BYREF

    v7 = rob_osg->nodes.end;
    if (rob_osg->nodes.begin == v7)
        return;

    sub_1404803B0(rob_osg, mat, data_scale, 0);
    v12 = rob_osg->nodes.end;
    osage_gravity_const = get_osage_gravity_const();
    v17 = rob_osg->nodes.begin;
    v130 = *mat;
    v18.x = rob_osg->exp_data.position.x;
    v17->field_10 = v17->trans;
    v20 = rob_osg->nodes.begin;
    src.x = rob_osg->exp_data.position.x * data_scale->x;
    src.y = rob_osg->exp_data.position.y * data_scale->y;
    src.z = rob_osg->exp_data.position.z * data_scale->z;
    mat4_mult_vec3_trans(&v130, &src, &dst);
    v18.x = dst.y;
    v20->trans.x = dst.x;
    v21 = dst.z;
    v20->trans.y = v18.x;
    v20->trans.z = v21;
    v22 = rob_osg->nodes.begin;
    v18.x = v22->trans.y - v22->field_10.y;
    v19.x = v22->trans.z - v22->field_10.z;
    v22->field_1C.x = v22->trans.x - v22->field_10.x;
    v22->field_1C.y = v18.x;
    v22->field_1C.z = v19.x;
    sub_14047F110(rob_osg, &v130, data_scale, 0);
    *rob_osg->nodes.begin->bone_node_mat = v130;
    *rob_osg->nodes.begin->bone_node->ex_data_mat = v130;
    src = (vec3){ 1.0f, 0.0f, 0.0f };
    mat4_mult_vec3(&v130, &src, &v128);
    v25 = data_scale->x;
    a3a = data_scale->x;
    if (!rob_osg->field_1F0D && osage_phys_step <= 0.0f)
        return;

    v26 = rob_osg->nodes.begin + 1;
    if (v26 != v12) {
        v27 = a5;
        v28 = 0.5f;
        v30 = rob_osg->nodes.begin;
        do {
            v31 = v30[1].data_ptr;
            a1a = vec3_null;
            v32 = v31->skp_osg_node.weight;
            if (!rob_osg->field_1F78) {
                sub_140482300(
                    &a1a,
                    &v30[1].trans,
                    &v30->trans,
                    osage_gravity_const,
                    v31->skp_osg_node.weight);

                if (v26 == &v12[-1]) {
                    v35 = a1a.z;
                    v34 = a1a.y;
                    v33 = a1a.x;
                }
                else {
                    sub_140482300(&v112, &v30[1].trans, &v30[2].trans, osage_gravity_const, v32);
                    v33 = (a1a.x + v112.x) * v28;
                    v34 = (a1a.y + v112.y) * v28;
                    v35 = (a1a.z + v112.z) * v28;
                }
            }
            else {
                v33 = rob_osg->field_1F7C.x * (1.0f / v32);
                v34 = rob_osg->field_1F7C.y * (1.0f / v32);
                v35 = rob_osg->field_1F7C.z * (1.0f / v32);
            }
            v36 = v31->force;
            v38 = v30[1].field_DC;
            v39 = rob_osg->skin_param_ptr;
            v40 = 1.0f - rob_osg->skin_param_ptr->air_res;
            v41 = (1.0f - rob_osg->field_1EB4) * v40;
            v42 = ((v34 + (v128.y * v36 * v38)) - (v30[1].field_1C.y * v41)) + (v30[1].field_D0.y * v32);
            v43 = ((v35 + (v128.z * v36 * v38)) - (v30[1].field_1C.z * v41)) + (v30[1].field_D0.z * v32);
            v44 = ((v33 + (v128.x * v36 * v38)) - (v30[1].field_1C.x * v41)) + (v30[1].field_D0.x * v32);
            v126.y = v42;
            v126.z = v43;
            v126.x = v44;
            if (!v27) {
                v45 = v39->wind_afc;
                v44 = v44 + (rob_osg->wind_direction.x * v45);
                v42 = v42 + (rob_osg->wind_direction.y * v45);
                v43 = v43 + (rob_osg->wind_direction.z * v45);
                v126.x = v44;
                v126.y = v42;
                v126.z = v43;
            }

            if (v39->stiffness > 0.0) {
                v46 = v30[1].field_1C.z - v30->field_1C.z;
                v47 = v30[1].field_1C.x - v30->field_1C.x;
                v114.y = v30[1].field_1C.y - v30->field_1C.y;
                v115 = v46;
                v114.x = v47;
                *(vec2*)&v116.x = v114;
                v116.z = v46;
                v44 = v44 - (v40 * v47);
                v126.x = v44;
                v126.y = v42 - (v114.y * v40);
                v126.z = v43 - (v46 * v40);
            }
            v48 = v31->skp_osg_node.weight;
            v117 = v126;
            ++v26;
            ++v30;
            v49 = 1.0f / (v48 - (v48 - 1.0f) * v31->skp_osg_node.inertial_cancel);
            v50 = v126.y * v49;
            v51 = v126.z * v49;
            v30->field_28.x = v49 * v44;
            v30->field_28.y = v50;
            v30->field_28.z = v51;
        } while (v26 != v12);
    }

    if (rob_osg->skin_param_ptr->stiffness > 0.0) {
        v52 = v130.row0;
        v53 = v130.row1;
        v54 = v130.row2;
        v55 = rob_osg->nodes.begin + 1;
        v56 = v130.row3;
        v131 = v130;
        mat4_get_translation(&v130, &a1a);
        if (v55 != v12) {
            v58 = a1a.z;
            v59 = *(vec2*)&a1a.x;
            v60 = v55;
            while (1) {
                v61 = v60->field_94.x;
                v132.row0 = v52;
                v132.row1 = v53;
                v132.row2 = v54;
                v119.x = v61;
                v62 = v60->field_94.y;
                v132.row3 = v56;
                v119.y = v62;
                v63 = v60->field_94.z;
                v119.z = v63;
                mat4_mult_vec3_trans(&v132, &v119, &v128);
                v68 = v60->field_1C.x + v60->field_28.x;
                v64 = v60->field_1C.y + v60->field_28.y;
                v65 = v60->field_1C.z + v60->field_28.z;
                v126.x = v60->trans.x + v68;
                v126.y = v60->trans.y + v64;
                v126.z = v60->trans.z + v65;
                sub_140482F30(&v126, &a1a, v25 * v55->length);
                v70 = v60->data_ptr;
                v71 = rob_osg->skin_param_ptr->stiffness;
                v72 = v70->skp_osg_node.weight;
                vec3_sub(v128, v126, v117);
                vec3_mult_scalar(v117, v71, v117);
                v120 = v117;
                v73 = (v72 - 1.0f) * v70->skp_osg_node.inertial_cancel;
                v74 = 1.0f / (v72 - v73);
                v60->field_28.x += v117.x * v74;
                v60->field_28.y += v117.y * v74;
                v60->field_28.z += v117.z * v74;
                v116.x = v60->trans.x + v117.x * v74 + v68;
                v116.y = v60->trans.y + v117.y * v74 + v64;
                v116.z = v60->trans.z + v117.z * v74 + v65;
                v126 = v116;
                mat4_mult_vec3_inv(&v131, &v116, &a2a);
                sub_140482FF0(&v131, &a2a, 0, 0, &rob_osg->field_1EB8);
                *(vec2*)&src.x = v59;
                src.z = v58;
                mat4_translate_mult(
                    &v131,
                    sqrtf((v59.x - v126.x) * (v59.x - v126.x)
                        + (v59.y - v126.y) * (v59.y - v126.y)
                        + (v58 - v126.z) * (v58 - v126.z)),
                    0.0f,
                    0.0f, &v131);
                v59 = *(vec2*)&v126.x;
                v58 = v126.z;
                ++v55;
                ++v60;
                a1a = v126;
                if (v55 == v12)
                    break;
                v56 = v131.row3;
                v54 = v131.row2;
                v53 = v131.row1;
                v57 = a1a.x;
                v52 = v131.row0;
            }
        }
    }
    v82 = rob_osg->nodes.begin + 1;
    if (v82 != v12) {
        v83 = rob_osg->nodes.begin + 1;
        do {
            ++v82;
            ++v83;
            v83->field_10.x = v83->trans.x;
            v83->field_10.y = v83->trans.y;
            v83->field_10.z = v83->trans.z;
            vec3_add(v83->field_1C, v83->field_28, v83->field_1C);
            vec3_add(v83->trans, v83->field_1C, v83->trans);
        } while (v82 != v12);
    }
    for (i = v12 - 2; i != rob_osg->nodes.begin; --i)
        sub_140482F30(&i->trans, &i[1].trans, v25 * i->child_length);

    if (a6) {
        v91 = rob_osg->nodes.begin;
        v92 = v91->trans.x;
        v93 = v91->data_ptr->skp_osg_node.coli_r;
        v94 = rob_osg->ring.ring_rectangle_x - v93;
        v95 = rob_osg->ring.ring_rectangle_y - v93;
        if (v92 < v94
            || (v96 = v91->trans.z, v96 < v95)
            || v92 >(v94 + rob_osg->ring.ring_rectangle_width + v93)
            || v96 > (v95 + rob_osg->ring.ring_rectangle_height + v93))
            v97 = rob_osg->ring.ring_out_height;
        else
            v97 = rob_osg->ring.ring_height;

        v98 = v91 + 1;
        for (v126.x = v97 + v93; v98 != v12; ++v98) {
            sub_140482490(v98, &osage_phys_step, &a3a);
            sub_140482180(v98, &v126.x);
        }
    }

    if (a7) {
        v99 = rob_osg->nodes.begin + 1;
        if (v99 != v12) {
            v100 = rob_osg->nodes.begin;
            v101 = &rob_osg->field_1EB8;
            do {
                mat4_mult_vec3_inv(&v130, &v100[1].trans, &a2a);
                v102 = sub_140482FF0(
                    &v130,
                    &a2a,
                    &v100[1].data_ptr->skp_osg_node.hinge,
                    &v100[1].field_B8,
                    v101);
                *v100[1].bone_node->ex_data_mat = v130;
                v103 = v100[1].trans.y - v100->trans.y;
                v104 = (((v100[1].trans.x - v100->trans.x) * (v100[1].trans.x - v100->trans.x))
                    + (v103 * v103)) + ((v100[1].trans.z - v100->trans.z)
                        * (v100[1].trans.z - v100->trans.z));
                v105 = v25 * v99->length;
                if (v104 >= (v105 * v105)) {
                    v105 = sqrtf(v104);
                    v106 = 0;
                }
                else
                    v106 = 1;
                mat4_translate_mult(&v130, v105, 0.0f, 0.0f, &v130);
                if (v102 || v106)
                    mat4_get_translation(&v130, &v100[1].trans);
                ++v99;
                ++v100;
            } while (v99 != v12);
            rob_osg = rob_osg;
        }
        v109 = rob_osg->nodes.end;
        if (rob_osg->nodes.begin != v109 && rob_osg->node.bone_node_mat) {
            mat4_translate_mult(v109[-1].bone_node->ex_data_mat,
                v25 * rob_osg->node.length, 0.0f, 0.0f,
                rob_osg->node.bone_node->ex_data_mat);
        }
    }
    rob_osg->field_2A0 = false;
}

static bool sub_140483DE0(vec3* a1, vec3* a2, vec3* a3, float_t radius) {
    vec3 v5;
    float_t length;
    vec3_sub(*a2, *a3, v5);
    vec3_length_squared(v5, length);
    if (fabs(length) > 0.000001 && length < radius * radius) {
        vec3_mult_scalar(v5, radius / sqrtf(length) - 1.0f, *a1);
        return true;
    }
    return false;
}

static bool sub_140484540(vec3* a1, vec3* a2, osage_coli* a3, float_t radius) {
    vec3 v11;
    vec3_sub(*a2, a3->bone0_pos, v11);

    float_t v17;
    vec3_dot(v11, a3->bone_pos_diff, v17);
    if (v17 < 0.0f)
        return sub_140483DE0(a1, a2, &a3->bone0_pos, radius);
    else if (fabsf(a3->bone_pos_diff_length_squared) <= 0.000001f)
        return sub_140483DE0(a1, a2, &a3->bone0_pos, radius);
    else if (v17 > a3->bone_pos_diff_length_squared)
        return sub_140483DE0(a1, a2, &a3->bone1_pos, radius);

    float_t v20;
    vec3_length_squared(v11, v20);
    v20 += (v17 * v17) / a3->bone_pos_diff_length_squared;
    if (v20 <= 0.000001f || v20 >= radius * radius)
        return false;

    vec3 v29;
    vec3_mult_scalar(a3->bone_pos_diff, v17 / a3->bone_pos_diff_length_squared, v29);
    vec3_sub(v11, v29, v29);
    vec3_mult_scalar(v29, radius / sqrtf(v20) - 1.0f, *a1);
    return true;
}

static bool sub_140484780(vec3* a1, vec3* a2, vec3* a3, vec3* a4, float_t radius) {
    float v5; // xmm8_4
    float v6; // xmm8_4
    __int64 result; // rax
    float_t v8; // xmm1_4
    float_t v9; // xmm2_4

    float_t v3;
    vec3_dot(*a2, *a4, v3);
    float_t v4;
    vec3_dot(*a3, *a4, v4);
    v5 = v3 - v4 - radius;
    if (v5 >= 0.0f)
        return false;

    vec3_mult_scalar(*a4, -v5, *a1);
    return true;
}

static bool sub_140483EA0(vec3* a1, vec3* a2, osage_coli* a3, float_t radius) {
    float v4; // xmm14_4
    float v6; // xmm0_4
    float v7; // xmm10_4
    float v8; // xmm2_4
    float v9; // xmm3_4
    bool v11; // r9
    vec4 v12; // xmm5
    float v13; // xmm6_4
    vec4 v14; // xmm8
    vec4 v15; // xmm0
    float_t v16; // xmm1_4
    float v17; // er11
    float v18; // r10^4
    vec4 v19; // xmm1
    float v20; // xmm15_4
    vec4 v21; // xmm11
    vec4 v22; // xmm7
    float v23; // xmm2_4
    float v24; // xmm3_4
    float v25; // xmm0_4
    float_t v26; // xmm14_4
    float v27; // xmm3_4
    vec4 v28; // xmm10
    float v29; // xmm2_4
    vec4 v30; // xmm0
    float v31; // xmm2_4
    float v32; // xmm1_4
    float v33; // xmm0_4
    float v34; // xmm10_4
    float v35; // xmm1_4
    float v36; // xmm1_4
    float v37; // xmm3_4
    vec4 v38; // xmm0
    float v39; // xmm2_4
    vec4 v40; // xmm0
    float v41; // xmm6_4
    vec4 v42; // xmm1
    float v43; // xmm6_4
    float v44; // xmm8_4
    float v45; // xmm2_4
    float v46; // xmm3_4
    vec4 v47; // xmm0
    vec2 v48; // rax
    float v49; // edx
    float v50; // xmm1_4
    vec2 v51; // [rsp+20h] [rbp-E0h]
    vec2 v52; // [rsp+20h] [rbp-E0h]
    float v53; // [rsp+28h] [rbp-D8h]
    vec2 v54; // [rsp+30h] [rbp-D0h]
    vec2 v55; // [rsp+30h] [rbp-D0h]
    vec2 v56; // [rsp+40h] [rbp-C0h]
    vec2 v57; // [rsp+40h] [rbp-C0h]
    float v58; // [rsp+50h] [rbp-B0h]
    float v59; // [rsp+54h] [rbp-ACh]
    float v60; // [rsp+58h] [rbp-A8h]
    float v61; // [rsp+5Ch] [rbp-A4h]
    float v62; // [rsp+60h] [rbp-A0h]
    vec3 a3a; // [rsp+70h] [rbp-90h] BYREF

    v4 = a3->bone_pos_diff_length;
    v6 = fabs(a3->bone_pos_diff_length);
    v7 = a3->bone0_pos.x;
    v8 = a3->bone0_pos.y;
    v9 = a3->bone0_pos.z;
    a3a.x = v7;
    a3a.y = v8;
    a3a.z = v9;
    if (v6 <= 0.000001)
        return sub_140483DE0(a1, a2, &a3a, radius);
    v11 = 0;
    v14.x = a2->x;
    v12.x = a2->x - a3->bone1_pos.x;
    v61 = (v4 * 0.5) * (v4 * 0.5);
    v13 = fsqrt((radius * radius) + v61);
    v14.x = a2->x - v7;
    v15 = v14;
    v16 = a2->y - v8;
    v17 = a2->z - v9;
    v53 = a2->z - a3->bone1_pos.z;
    v15.x = (v14.x * v14.x) + (v16 * v16);
    v51.y = a2->y - a3->bone1_pos.y;
    v56 = (vec2)__PAIR64__(LODWORD(v16), LODWORD(v14.x));
    v18 = v16;
    v19 = v12;
    v54 = v56;
    v15.x = v15.x + (v17 * v17);
    v51.x = v12.x;
    v57 = v51;
    v20 = _mm_sqrt_ps((__m128)v15).m128_f32[0];
    v19.x = ((v12.x * v12.x) + (v51.y * v51.y)) + (v53 * v53);
    v21 = (vec4)_mm_sqrt_ps((__m128)v19);
    if (fabs(v20) <= 0.000001 || fabs(v21.x) <= 0.000001)
        return 0;
    v22 = v21;
    v22.x = v21.x + v20;
    if ((v21.x + v20) < (v13 * 2.0)) {
        v23 = a3->bone_pos_diff.y;
        v24 = a3->bone_pos_diff.z;
        v25 = 1.0 / v4;
        v26 = a3->bone1_pos.x + v7;
        v28 = (vec4)LODWORD(a2->x);
        v52.x = v26;
        v52.y = a3a.y + a3->bone1_pos.y;
        v62 = a3->bone_pos_diff.x * v25;
        *(vec2*)&a3a.x = v52;
        v60 = v23 * v25;
        v59 = v24 * v25;
        a3a.z = a3a.z + a3->bone1_pos.z;
        v27 = a2->z;
        v28.x = v28.x - (v26 * 0.5);
        v29 = a2->y;
        a3a.x = v28.x;
        v30 = v28;
        a3a.y = v29 - (v52.y * 0.5);
        v30.x = ((v28.x * v28.x) + (a3a.y * a3a.y))
            + ((v27 - (a3a.z * 0.5)) * (v27 - (a3a.z * 0.5)));
        v31 = v27 - (a3a.z * 0.5);
        v32 = _mm_sqrt_ps((__m128)v30).m128_f32[0];
        v58 = v32;
        if (v32 == 0.0) {
            v35 = a3a.y;
            v34 = a3a.x;
        }
        else {
            v33 = 1.0 / v32;
            v34 = v28.x * (1.0 / v32);
            v35 = a3a.y * (1.0 / v32);
            v31 = v31 * v33;
        }
        v36 = ((v35 * v60) + (v62 * v34)) + (v31 * v59);
        v37 = v36 * v36;
        if ((v36 * v36) >= 1.0)
            v37 = 1.0;
        v22.x = v22.x * 0.5;
        v38 = v22;
        v38.x = (v22.x * v22.x) - v61;
        if (v38.x < 0.0)
            return 0;
        v39 = _mm_sqrt_ps((__m128)v38).m128_f32[0];
        if (fabs(v39) <= 0.000001)
            return 0;
        v40 = (vec4)0x3F800000u;
        v40.x = 1.0 - v37;
        v41 = ((v13 / v22.x) - 1.0) * (v36 * v58);
        v42 = (vec4)_mm_sqrt_ps((__m128)v40);
        v42.x = (((v42.x * v58) * ((radius / v39) - 1.0))
            * ((v42.x * v58) * ((radius / v39) - 1.0)))
            + (v41 * v41);
        LODWORD(v43) = _mm_sqrt_ps((__m128)v42).m128_u32[0];
        if (v20 == 0.0) {
            v44 = v54.x;
        }
        else {
            v44 = v14.x * (1.0 / v20);
            v18 = v54.y * (1.0 / v20);
            v17 = v17 * (1.0 / v20);
        }
        v45 = v57.y;
        v46 = v53;
        if (v21.x == 0.0) {
            v12 = (vec4)LODWORD(v57.x);
        }
        else {
            v12.x = v12.x * (1.0 / v21.x);
            v45 = v57.y * (1.0 / v21.x);
            v46 = v53 * (1.0 / v21.x);
        }
        v12.x = v12.x + v44;
        v47 = v12;
        v48 = (vec2)__PAIR64__(v18 + v45, LODWORD(v12.x));
        v49 = v17 + v46;
        v47.x = ((v12.x * v12.x) + ((v18 + v45) * (v18 + v45)))
            + ((v17 + v46) * (v17 + v46));
        v50 = _mm_sqrt_ps((__m128)v47).m128_f32[0];
        if (v50 != 0.0) {
            v12.x = v12.x * (1.0 / v50);
            v55.x = v12.x;
            v55.y = (v18 + v45) * (1.0 / v50);
            v48 = v55;
            v49 = (v17 + v46) * (1.0 / v50);
        }
        *(vec2*)&a3a.x = v48;
        a3a.z = v49;
        v11 = 1;
        a1->x = v12.x * v43;
        a1->y = v48.y * v43;
        a1->z = v49 * v43;
    }
    return v11;
}

int32_t sub_140485220(vec3* a1, float_t* radius, osage_coli* a3, float_t* a4) {
    if (!a3)
        return 0;

    int32_t v8 = 0;
    while (a3->type) {
        int32_t v11 = 0;
        vec3 v31 = vec3_null;
        switch (a3->type) {
        case 1:
            v11 = sub_140483DE0(&v31, a1, &a3->bone0_pos, a3->radius + *radius);
            break;
        case 2:
            v11 = sub_140484540(&v31, a1, a3, a3->radius + *radius);
            break;
        case 3:
            v11 = sub_140484780(&v31, a1, &a3->bone0_pos, &a3->bone1_pos, *radius);
            break;
        case 4:
            v11 = sub_140483EA0(&v31, a1, a3, a3->radius + *radius);
            break;
        case 5:
            v11 = sub_140483B30(&v31, a1, a3, *radius);
            break;
        }

        if (a4 && v11 > 0) {
            if (a3->field_34 < *a4)
                *a4 = a3->field_34;
        }
        a3++;
        v8 += v11;
        vec3_add(*a1, v31, *a1);
    }
    return v8;
}

static int32_t sub_1404851C0(vec3* a1, float_t* a2, vector_osage_coli* a3, float_t* a4) {
    if (a3->begin == a3->end || a3->end[-1].type)
        return 0;
    else
        return sub_140485220(a1, a2, a3->begin, a4);
}

static void sub_14047D8C0(rob_osage* rob_osg, mat4* mat, vec3* data_scale, float_t osage_phys_step, bool a5) {
    float v9; // xmm10_4
    rob_osage_node* v16; // rax
    float v17; // xmm4_4
    float v18; // xmm2_4
    float v19; // xmm1_4
    float v20; // xmm0_4
    float v21; // xmm3_4
    float v22; // xmm0_4
    float v23; // xmm9_4
    vector_rob_osage_node* v24; // r15
    rob_osage_node* v25; // rdi
    rob_osage_node* v26; // rax
    rob_osage_node* v27; // rsi
    vec3 v28;
    rob_osage_node* v30; // rbx
    rob_osage_node* v35;
    float v37; // xmm6_4
    bool v39; // al
    mat4* v41; // rax
    float v42; // xmm7_4
    float_t v44; // xmm2
    bool v45; // di
    float_t v55; // xmm2
    rob_osage_node* v59; // rax
    float_t v60; // [rsp+30h] [rbp-D0h] BYREF
    vec3 src; // [rsp+40h] [rbp-C0h] BYREF
    vec3 dst; // [rsp+50h] [rbp-B0h] BYREF
    vec3 v63; // [rsp+60h] [rbp-A0h] BYREF
    mat4 v64; // [rsp+70h] [rbp-90h] BYREF

    if (!rob_osg->field_1F0D && osage_phys_step <= 0.0f)
        return;

    v9 = 0.0f;
    if (osage_phys_step > 0.0f)
        v9 = 1.0f / osage_phys_step;

    if (a5)
        v64 = *rob_osg->nodes.begin[0].bone_node_mat;
    else {
        vec3_mult(rob_osg->exp_data.position, *data_scale, src);
        v64 = *mat;
        mat4_mult_vec3_trans(&v64, &src, &rob_osg->nodes.begin[0].trans);
        sub_14047F110(rob_osg, &v64, data_scale, 0);
    }

    v60 = -1000.0f;
    if (a5) {
        v16 = &rob_osg->nodes.begin[0];
        v17 = v16->trans.x;
        v18 = v16->data_ptr->skp_osg_node.coli_r;
        v19 = rob_osg->ring.ring_rectangle_x - v18;
        v20 = rob_osg->ring.ring_rectangle_y - v18;
        v21 = v16->trans.z;
        if (v17 < v19
            || v21 < v20
            || v17 > (v19 + rob_osg->ring.ring_rectangle_width) + v18
            || v21 > (v20 + rob_osg->ring.ring_rectangle_height) + v18)
            v22 = rob_osg->ring.ring_out_height;
        else
            v22 = rob_osg->ring.ring_height;
        v60 = v22 + v18;
    }
    v23 = 0.2f;
    if (osage_phys_step < 1.0)
        v23 = 0.2f / (2.0f - osage_phys_step);
    v24 = rob_osg->skin_param_ptr->colli_tgt_osg;
    if (v24) {
        v25 = rob_osg->nodes.begin + 1;
        if (v25 != rob_osg->nodes.end) {
            v26 = v24->end;
            v27 = rob_osg->nodes.begin + 1;
            do {
                v28 = vec3_null;
                v30 = v24->begin + 1;
                dst = vec3_null;
                if (v30 != v26) {
                    do {
                        sub_140483DE0(
                            &dst,
                            &v27->trans,
                            &v30->trans,
                            v27->data_ptr->skp_osg_node.coli_r + v30->data_ptr->skp_osg_node.coli_r);
                        v26 = v24->end;
                        ++v30;
                    } while (v30 != v26);
                    v28 = dst;
                }
                vec3_add(v28, v27->trans, v27->trans);
                ++v25;
                ++v27;
            } while (v25 != rob_osg->nodes.end);
        }
    }
    v35 = rob_osg->nodes.begin + 1;
    if (v35 != rob_osg->nodes.end) {
        do {
            v37 = (1.0f - rob_osg->field_1EB4) * rob_osg->skin_param_ptr->friction;
            if (a5) {
                sub_140482490(v35, &osage_phys_step, &data_scale->x);
                v35->field_C8 += (float_t)sub_1404851C0(
                    &v35->trans,
                    &v35->data_ptr->skp_osg_node.coli_r,
                    &rob_osg->ring.coli,
                    &v35->field_CC);
                if (!rob_osg->field_1F0F) {
                    v35->field_C8 += (float_t)sub_140485180(
                        rob_osg->coli_ring,
                        &v35->trans,
                        &v35->data_ptr->skp_osg_node.coli_r);
                    v35->field_C8 += (float_t)sub_140485180(
                        rob_osg->coli,
                        &v35->trans,
                        &v35->data_ptr->skp_osg_node.coli_r);
                }
                sub_140482180(v35, &v60);
            }
            else
                sub_140482F30(&v35->trans, &v35[-1].trans, v35->length * data_scale->x);

            mat4_mult_vec3_inv(&v64, &v35->trans, &v63);
            v39 = sub_140482FF0(&v64, &v63, &v35->data_ptr->skp_osg_node.hinge, &v35->field_B8, &rob_osg->field_1EB8);
            v35->bone_node->exp_data.data_scale = *data_scale;
            *v35->bone_node->ex_data_mat = v64;
            v41 = v35->bone_node_mat;
            if (v41)
                mat4_scale_rot(&v64, data_scale->x, data_scale->y, data_scale->z, v41);
            v42 = v35->length * data_scale->x;
            vec3 v43;
            vec3_sub(v35->trans, v35[-1].trans, v43);
            vec3_length_squared(v43, v44);
            if (v44 >= (v42 * v42)) {
                v42 = sqrtf(v44);
                v45 = false;
            }
            else
                v45 = true;
            mat4_translate_mult(&v64, v42, 0.0, 0.0, &v64);
            v35->field_C4 = v42;
            if (v39 || v45)
                mat4_get_translation(&v64, &v35->trans);

            vec3_sub(v35->trans, v35->field_10, v35->field_1C);
            vec3_mult_scalar(v35->field_1C, v9, v35->field_1C);
            if (v35->field_C8 > 0.0) {
                if (v35->field_CC < v37)
                    v37 = v35->field_CC;
                vec3_mult_scalar(v35->field_1C, v37, v35->field_1C);
            }
            vec3_length_squared(v35->field_1C, v55);
            if (v55 > v23 * v23)
                vec3_mult_scalar(v35->field_1C, v23 / sqrtf(v55), v35->field_1C);

            mat4_mult_vec3_inv(mat, &v35->trans, &v35->field_A0);
            sub_1400B39E0(mat, &v35->field_1C, &v35->field_AC);
            ++v35;
        } while (v35 != rob_osg->nodes.end);
    }

    v59 = rob_osg->nodes.end;
    if (rob_osg->nodes.begin != v59 && rob_osg->node.bone_node_mat) {
        mat4 v65 = *v59[-1].bone_node->ex_data_mat;
        mat4_translate_mult(&v65, rob_osg->node.length * data_scale->x, 0.0, 0.0, &v65);
        *rob_osg->node.bone_node->ex_data_mat = v65;
        mat4_scale_rot(&v65, data_scale->x, data_scale->y, data_scale->z, &v65);
        *rob_osg->node.bone_node_mat = v65;
        rob_osg->node.bone_node->exp_data.data_scale = *data_scale;
    }
}

static void sub_14047C770(rob_osage* a1, mat4* a2, vec3* a3, float_t osage_phys_step, bool a5) {
    sub_14047C800(a1, a2, a3, osage_phys_step, a5, 0, 0);
    sub_14047D8C0(a1, a2, a3, osage_phys_step, true);
    sub_140480260(a1, a2, a3, osage_phys_step, a5);
}

static void ex_osage_block_update(ex_osage_block* osg) {
    osg->field_1FF8 &= ~2;
    if (osg->base.set_data) {
        osg->base.set_data = false;
        return;
    }

    rob_chara_data_item_equip* v2 = osg->base.item_equip_object->item_equip;
    float_t osage_phys_step = get_delta_frame() * v2->osage_phys_step;
    //if (v2->field_940.begin != v2->field_940.end && v4->field_C)
    //    osage_phys_step = 1.0f;
    bone_node* v6 = osg->base.parent_bone_node;
    vec3 data_scale = v6->exp_data.data_scale;
    vec3 scale = v6->exp_data.scale;

    mat4 mat;
    mat4* v7;
    if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f) {
        vec3_div(vec3_identity, scale, scale);
        mat4_scale_rot(v6->ex_data_mat, scale.x, scale.y, scale.z, &mat);
        v7 = &mat;
    }
    else
        v7 = v6->ex_data_mat;
    ex_osage_block_set_wind_direction(osg);
    rob_osage_set_coli(&osg->rob, osg->mat);
    sub_14047C770(&osg->rob, v7, &data_scale, osage_phys_step, false);
}

static void ex_osage_block_dispose(ex_osage_block* osg) {
    ex_osage_block_reset(osg);
    rob_osage_free(&osg->rob);
    ex_node_block_free(&osg->base);
    free(osg);
}

static float_t exp_abs(float_t v1) {
    return fabsf(v1);
}

static float_t exp_acos(float_t v1) {
    return acosf(v1);
}

static float_t exp_add(float_t v1, float_t v2) {
    return v1 + v2;
}

static float_t exp_and(float_t v1, float_t v2) {
    return v1 != 0.0f && v2 != 0.0f ? 1.0f : 0.0f;
}

static float_t exp_asin(float_t v1) {
    return asinf(v1);
}

static float_t exp_atan(float_t v1) {
    return atanf(v1);
}

static float_t exp_av(float_t v1) {
    return v1 * 0.1f;
}

static float_t exp_ceil(float_t v1) {
    return ceilf(v1);
}

static float_t exp_cond(float_t v1, float_t v2, float_t v3) {
    return v1 != 0.0f ? v2 : v3;
}

static float_t exp_cos(float_t v1) {
    return cosf(v1);
}

static float_t exp_div(float_t v1, float_t v2) {
    return v1 / v2;
}

static float_t exp_eq(float_t v1, float_t v2) {
    return v1 == v2 ? 1.0f : 0.0f;
}

static float_t exp_exp(float_t v1) {
    return expf(v1);
}

static float_t exp_floor(float_t v1) {
    return floorf(v1);
}

static float_t exp_fmod(float_t v1, float_t v2) {
    return fmodf(v1, v2);
}

static const exp_func_op1* exp_func_op1_find_func(string* name, const exp_func_op1* array) {
    char* name_str = string_data(name);
    while (array->name) {
        if (!str_utils_compare(name_str, array->name))
            return array;
        array++;
    }
    return 0;
}

static const exp_func_op2* exp_func_op2_find_func(string* name, const exp_func_op2* array) {
    char* name_str = string_data(name);
    while (array->name) {
        if (!str_utils_compare(name_str, array->name))
            return array;
        array++;
    }
    return 0;
}

static const exp_func_op3* exp_func_op3_find_func(string* name, const exp_func_op3* array) {
    char* name_str = string_data(name);
    while (array->name) {
        if (!str_utils_compare(name_str, array->name))
            return array;
        array++;
    }
    return 0;
}

static float_t exp_ge(float_t v1, float_t v2) {
    return v1 >= v2 ? 1.0f : 0.0f;
}

static float_t exp_gt(float_t v1, float_t v2) {
    return v1 > v2 ? 1.0f : 0.0f;
}

static float_t exp_le(float_t v1, float_t v2) {
    return v1 <= v2 ? 1.0f : 0.0f;
}

static float_t exp_log(float_t v1) {
    return logf(v1);
}

static float_t exp_lt(float_t v1, float_t v2) {
    return v1 < v2 ? 1.0f : 0.0f;
}

static float_t exp_max(float_t v1, float_t v2) {
    return max(v1, v2);
}

static float_t exp_min(float_t v1, float_t v2) {
    return min(v1, v2);
}

static float_t exp_mul(float_t v1, float_t v2) {
    return v1 * v2;
}

static float_t exp_ne(float_t v1, float_t v2) {
    return v1 != v2 ? 1.0f : 0.0f;
}

static float_t exp_neg(float_t v1) {
    return -v1;
}

static float_t exp_or(float_t v1, float_t v2) {
    return v1 != 0.0 || v2 != 0.0f ? 1.0f : 0.0f;
}

static float_t exp_pow(float_t v1, float_t v2) {
    return powf(v1, v2);
}

static float_t exp_rand(float_t v1, float_t v2, float_t v3) {
    return v1 != 0.0f ? v2 : v3;
}

static float_t exp_rand_0_1(float_t v1) {
    return 0.0f;
}

static float_t exp_round(float_t v1) {
    return roundf(v1);
}

static float_t exp_sin(float_t v1) {
    return sinf(v1);
}

static float_t exp_sqrt(float_t v1) {
    return sqrtf(v1);
}

static float_t exp_sub(float_t v1, float_t v2) {
    return v1 - v2;
}

static float_t exp_tan(float_t v1) {
    return tanf(v1);
}

static float_t get_osage_gravity_const() {
    return 0.00299444468691945f;
}

static void mot_key_frame_interpolate(mot* a1, float_t frame, float_t* value,
    mot_key_set* a4, uint32_t key_set_count, struc_369* a6) {
    if (a6->field_0 == 4) {
        int32_t frame_int = (int32_t)frame;
        if (frame != -0.0f && (float_t)frame_int != frame)
            frame = (float_t)(frame < 0.0f ? frame_int - 1 : frame_int);
    }

    if (!key_set_count)
        return;

    for (uint32_t i = key_set_count; i; i--, a4++, value++) {
        mot_key_set_type type = a4->type;
        float_t* values = a4->values;
        if (type == MOT_KEY_SET_STATIC) {
            *value = values[0];
            continue;
        }
        else if (type != MOT_KEY_SET_HERMITE && type != MOT_KEY_SET_HERMITE_TANGENT) {
            *value = 0.0f;
            continue;
        }

        int32_t current_key = a4->current_key;
        int32_t keys_count = a4->keys_count;
        uint16_t* frames = a4->frames;
        int32_t frame_int = (int32_t)frame;
        size_t key_index;
        if (current_key > keys_count
            || (frame_int > a4->last_key + frames[current_key])
            || current_key > 0 && frame_int < frames[current_key - 1]) {
            uint16_t* key = a4->frames;
            size_t length = keys_count;
            size_t temp;
            while (length > 0)
                if (frame_int < key[temp = length / 2])
                    length /= 2;
                else {
                    key += temp + 1;
                    length -= temp + 1;
                }
            key_index = key - frames;
        }
        else {
            uint16_t* key = &frames[current_key];
            for (uint16_t* v17 = &frames[keys_count]; key != v17; key++)
                if (frame_int < *key)
                    break;
            key_index = key - frames;
        }

        bool found = false;
        if (key_index < keys_count)
            for (; key_index < keys_count; key_index++)
                if ((float_t)frames[key_index] >= frame) {
                    found = true;
                    break;
                }

        if (found) {
            float_t next_frame = (float_t)frames[key_index];
            if (fabsf(next_frame - frame) > 0.000001f && key_index > 0) {
                float_t curr_frame = (float_t)frames[key_index - 1];
                float_t t = (frame - curr_frame) / (next_frame - curr_frame);
                if (type == MOT_KEY_SET_HERMITE) {
                    values += key_index - 1;
                    float_t p1 = values[0];
                    float_t p2 = values[1];
                    *value = (t * 2.0f - 3.0f) * (t * t) * (p1 - p2) + p1;
                }
                else {
                    values += key_index * 2 - 2;
                    float_t p1 = values[0];
                    float_t p2 = values[2];
                    float_t t1 = values[1];
                    float_t t2 = values[3];
                    *value = ((t - 1.0f) * t1 + t * t2) * (t - 1.0f) * (frame - curr_frame)
                        + (t * 2.0f - 3.0f) * (t * t) * (p1 - p2) + p1;
                }
                a4->current_key = (int32_t)key_index;
                continue;
            }
        }

        key_index--;
        if (type == MOT_KEY_SET_HERMITE)
            *value = values[key_index];
        else
            *value = values[2 * key_index];
        a4->current_key = (int32_t)key_index;
    }
}

static uint32_t mot_load_last_key_calc(uint16_t keys_count) {
    uint16_t v2 = keys_count >> 8;
    uint32_t v3 = 16;
    if (v2) {
        v3 = 8;
        keys_count = v2;
    }

    if (keys_count >> 4) {
        v3 -= 4;
        keys_count >>= 4;
    }

    if (keys_count >> 2) {
        v3 -= 2;
        keys_count >>= 2;
    }

    if (keys_count & ~1)
        keys_count = 2;
    return v3 - keys_count;
}

static bool mot_data_load_file(mot* a1, mot_data* a2) {
    a1->frame_count = 0;
    if (!a2)
        return 0;

    uint16_t info = a2->info;
    a1->frame_count = a2->frame_count;

    int32_t key_set_count = info & 0x3FFF;
    bool skeleton_select = (info & 0x4000) != 0;
    a1->key_set_count = key_set_count;
    a1->field_4 = a2->info & 0x8000;

    mot_key_set_data* key_set_file = a2->key_set;
    mot_key_set* key_set = a1->key_sets;

    for (int32_t i = 0; i < key_set_count; i++, key_set++, key_set_file++) {
        key_set->type = key_set_file->type;
        if (key_set->type == MOT_KEY_SET_STATIC)
            key_set->values = key_set_file->values;
        else if (key_set->type != MOT_KEY_SET_NONE) {
            uint16_t keys_count = key_set_file->keys_count;
            key_set->frames = key_set_file->frames;
            key_set->values = key_set_file->values;

            key_set->keys_count = keys_count;
            key_set->current_key = 0;

            uint16_t last_key = a1->frame_count;
            if (keys_count > 1) {
                if (keys_count > last_key)
                    keys_count = a1->frame_count;
                last_key = (16 - mot_load_last_key_calc(keys_count)) * a1->frame_count / keys_count;
            }
            key_set->last_key = last_key;
        }
    }
    return skeleton_select;
}

static void mot_parent_get_key_set_count_by_bone_database_bones(mot_parent* a1,
    vector_bone_database_bone* a2) {
    size_t object_bone_count;
    size_t motion_bone_count;
    size_t total_bone_count;
    size_t ik_bone_count;
    size_t chain_pos;
    bone_database_bones_calculate_count(a2, &object_bone_count,
        &motion_bone_count, &total_bone_count, &ik_bone_count, &chain_pos);
    mot_parent_get_key_set_count(a1, motion_bone_count, ik_bone_count);
}

static void mot_parent_get_key_set_count(mot_parent* a1, size_t motion_bone_count, size_t ik_bone_count) {
    a1->key_set_count = (motion_bone_count + ik_bone_count) * 3 + 16;
}

static void mot_parent_init_key_sets(mot_parent* a1, bone_database_skeleton_type type,
    size_t motion_bone_count, size_t ik_bone_count) {
    mot_parent_get_key_set_count(a1, motion_bone_count, ik_bone_count);
    mot_parent_reserve_key_sets(a1);
    a1->skeleton_type = type;
}

static void mot_parent_interpolate(mot_parent* a1, float_t frame,
    uint32_t key_set_offset, uint32_t key_set_count) {
    mot_key_frame_interpolate(&a1->mot, frame,
        &a1->key_set_data.begin[key_set_offset],
        &a1->key_set.begin[key_set_offset], key_set_count, &a1->field_68);
}

static mot_data* mot_parent_load_file(mot_parent* a1, int32_t motion_id, motion_database* mot_db) {
    if (a1->motion_id != motion_id) {
        mot_data* mot_data = motion_storage_get_mot_data(motion_id, mot_db);
        a1->mot_data = mot_data;
        if (mot_data) {
            a1->skeleton_select = mot_data_load_file(&a1->mot, mot_data);
            a1->motion_id = motion_id;
        }
        else {
            a1->skeleton_select = 0;
            a1->motion_id = -1;
        }
    }
    a1->frame = -1.0f;
    a1->field_68.field_0 = 0;
    a1->field_68.field_4 = 0.0f;
    return a1->mot_data;
}

static void mot_parent_reserve_key_sets(mot_parent* a1) {
    vector_mot_key_set_reserve(&a1->key_set, a1->key_set_count);
    a1->key_set.end += a1->key_set_count;
    memset(a1->key_set.begin, 0, sizeof(mot_key_set) * a1->key_set_count);

    vector_float_t_reserve(&a1->key_set_data, a1->key_set_count);
    a1->key_set_data.end += a1->key_set_count;
    memset(a1->key_set_data.begin, 0, sizeof(float_t) * a1->key_set_count);

    a1->mot.key_sets = a1->key_set.begin;
    a1->key_sets_ready = true;
}

static void mot_parent_free(mot_parent* a1) {
    vector_mot_key_set_free(&a1->key_set, 0);
    vector_float_t_free(&a1->key_set_data, 0);
}

static motion_blend_mot* motion_blend_mot_init() {
    motion_blend_mot* mot_blend_mot = force_malloc(sizeof(motion_blend_mot));
    bone_data_parent_reset(&mot_blend_mot->bone_data);
    mot_blend_mot->mot_data.key_set = vector_empty(mot_key_set);
    mot_blend_mot->mot_data.key_set_data = vector_empty(float_t);
    mot_blend_mot->mot_data.skeleton_type = BONE_DATABASE_SKELETON_NONE;
    mot_blend_mot->mot_data.frame = -1.0f;
    mot_blend_mot->mot_data.motion_id = -1;
    mot_blend_mot->field_4F8.field_C0 = 1.0f;
    mot_blend_mot->field_4F8.field_C4 = 1.0f;
    return mot_blend_mot;
}

static void motion_blend_mot_interpolate(motion_blend_mot* a1) {
    vec3* keyframe_data = (vec3*)a1->mot_data.key_set_data.begin;
    bool reverse = motion_blend_mot_interpolate_get_reverse(&a1->field_4F8.field_0);
    float_t frame = a1->field_4A8.field_0.frame;

    bone_database_skeleton_type skeleton_type = a1->bone_data.rob_bone_data->base_skeleton_type;
    bone_data* bones_data = a1->bone_data.bones.begin;
    for (uint16_t* i = a1->bone_data.bone_indices.begin; i != a1->bone_data.bone_indices.end; i++) {
        bone_data* data = &bones_data[*i];
        bool get_data = true;
        if (reverse && data->mirror != 255)
            data = &bones_data[data->mirror];

        if (get_data && frame != data->frame) {
            mot_parent_interpolate(&a1->mot_data, frame, data->key_set_offset, data->key_set_count);
            data->frame = frame;
        }

        keyframe_data = bone_data_set_key_data(data, keyframe_data, skeleton_type, get_data, reverse);
    }

    uint32_t v15 = a1->bone_data.bone_key_set_count;
    if (frame != a1->mot_data.frame) {
        mot_parent_interpolate(&a1->mot_data, frame, v15, a1->bone_data.global_key_set_count);
        a1->mot_data.frame = frame;
    }

    keyframe_data = (vec3*)&a1->mot_data.key_set_data.begin[v15];
    vec3 global_trans = keyframe_data[0];
    vec3 global_rotation = keyframe_data[1];
    if (reverse)
        vec3_negate(global_trans, global_trans);
    a1->bone_data.global_trans = global_trans;
    a1->bone_data.global_rotation = global_rotation;

    float_t rotation_y = a1->bone_data.rotation_y;
    mat4 mat;
    mat4_rotate_y(rotation_y, &mat);
    mat4_translate_mult(&mat, global_trans.x, global_trans.y, global_trans.z, &mat);
    mat4_rotate_mult(&mat, global_rotation.x, global_rotation.y, global_rotation.z, &mat);
    for (bone_data* i = a1->bone_data.bones.begin; i != a1->bone_data.bones.end; i++)
        switch (i->type) {
        case BONE_DATABASE_BONE_TYPE_1:
            mat4_mult_vec3_trans(&mat, &i->trans, &i->trans);
            break;
        case BONE_DATABASE_BONE_POSITION_ROTATION: {
            mat4 rot_mat;
            mat4_clear_trans(&mat, &rot_mat);
            mat4_to_mat4u(&rot_mat, &i->rot_mat[0]);
            mat4_mult_vec3_trans(&mat, &i->trans, &i->trans);
        } break;
        case BONE_DATABASE_BONE_HEAD_IK_ROTATION:
        case BONE_DATABASE_BONE_ARM_IK_ROTATION:
        case BONE_DATABASE_BONE_LEGS_IK_ROTATION:
            mat4_mult_vec3_trans(&mat, &i->ik_target, &i->ik_target);
            break;
        }
}

static bool motion_blend_mot_interpolate_get_reverse(int32_t* a1) {
    return (*a1 & 1) && (*a1 & 8) == 0 || !(*a1 & 1) && (*a1 & 8) != 0;
}

static void motion_blend_mot_load_bone_data(motion_blend_mot* a1,
    rob_chara_data_bone_data* a2, bone_database* bone_data) {
    bone_data_parent_data_init(&a1->bone_data, a2, bone_data);
    mot_parent_init_key_sets(
        &a1->mot_data,
        a1->bone_data.rob_bone_data->base_skeleton_type,
        a1->bone_data.motion_bone_count,
        a1->bone_data.ik_bone_count);
}

static void motion_blend_mot_load_file(motion_blend_mot* a1, int32_t motion_id,
    bone_database* type, motion_database* mot_db) {
    mot_data* v6 = mot_parent_load_file(&a1->mot_data, motion_id, mot_db);
    bone_data_parent* v7 = &a1->bone_data;
    if (type) {
        bone_data_parent_load_bone_indices_from_mot(v7, v6, type, mot_db);
        vector_uint16_t* bone_indices = &a1->bone_data.bone_indices;
        bone_data* bone = a1->bone_data.bones.begin;
        for (uint16_t* i = bone_indices->begin; i != bone_indices->end; i++)
            bone[*i].frame = -1.0f;
    }
    else {
        rob_chara_data_bone_data* rob_bone_data = v7->rob_bone_data;
        for (mat4* i = rob_bone_data->mats.begin; i != rob_bone_data->mats.end; i++)
            *i = mat4_identity;

        for (mat4* i = rob_bone_data->mats2.begin; i != rob_bone_data->mats2.end; i++)
            *i = mat4_identity;

        a1->bone_data.bone_indices.end = a1->bone_data.bone_indices.begin;
    }

    a1->field_4F8.field_8C = false;
    a1->field_4F8.field_4C = a1->field_4F8.mat;
    a1->field_4F8.mat = mat4u_identity;
}

static void motion_blend_mot_mult_mat(motion_blend_mot* a1, mat4* mat) {
    if (mat) {
        mat4_to_mat4u(mat, &a1->field_4F8.mat);
        a1->field_4F8.field_8C = true;
    }
    else {
        a1->field_4F8.mat = mat4u_identity;
        a1->field_4F8.field_8C = false;
    }

    mat4 m;
    mat4u_to_mat4(&a1->field_4F8.mat, &m);
    bone_data* v3 = a1->bone_data.bones.begin;
    for (bone_data* v4 = a1->bone_data.bones.begin; v4 != a1->bone_data.bones.end; v4++)
        bone_data_mult_1(v4, &m, v3, true);
}

static void motion_blend_mot_reset(motion_blend_mot* mot_blend_mot) {
    bone_data_parent_reset(&mot_blend_mot->bone_data);
    mot_blend_mot->mot_data.key_sets_ready = 0;
    mot_blend_mot->mot_data.skeleton_type = BONE_DATABASE_SKELETON_NONE;
    mot_blend_mot->mot_data.frame = -1.0f;
    mot_blend_mot->mot_data.key_set_count = 0;
    mot_blend_mot->mot_data.mot_data = 0;
    mot_blend_mot->mot_data.skeleton_select = 0;
    mot_blend_mot->mot_data.field_68.field_0 = 0;
    mot_blend_mot->mot_data.field_68.field_4 = 0.0f;
    mot_blend_mot->mot_data.key_set.end = mot_blend_mot->mot_data.key_set.begin;
    mot_blend_mot->mot_data.key_set_data.end = mot_blend_mot->mot_data.key_set_data.begin;
    mot_blend_mot->mot_data.motion_id = -1;
    mot_blend_mot->field_4A8.field_0.frame = 0.0f;
    mot_blend_mot->field_4A8.field_0.field_8 = 1.0f;
    mot_blend_mot->field_4A8.field_0.step = 1.0f;
    mot_blend_mot->field_4A8.field_0.frame_count = 0;
    mot_blend_mot->field_4A8.field_0.last_frame = -1.0f;
    mot_blend_mot->field_4A8.field_0.field_14 = -1.0f;
    mot_blend_mot->field_4A8.field_0.field_18 = -1;
    mot_blend_mot->field_4A8.field_0.field_1C = -1;
    mot_blend_mot->field_4A8.field_0.field_20 = 0;
    mot_blend_mot->field_4A8.field_0.field_24 = -1.0f;
    mot_blend_mot->field_4A8.field_0.field_28 = 0;
    mot_blend_mot->field_4A8.field_0.field_2C = -1;
    mot_blend_mot->field_4A8.field_30 = 0;
    mot_blend_mot->field_4A8.field_34 = 0;
    mot_blend_mot->field_4A8.field_38 = 0;
    mot_blend_mot->field_4A8.field_40 = 0;
    mot_blend_mot->field_4A8.field_48 = 0;
    mot_blend_mot->field_4F8.field_0 = 0;
    mot_blend_mot->field_4F8.field_8 = 0;
    mot_blend_mot->field_4F8.mat = mat4u_identity;
    mot_blend_mot->field_4F8.field_4C = mat4u_identity;
    mot_blend_mot->field_4F8.field_8C = false;
    mot_blend_mot->field_4F8.eyes_adjust = 0.0f;
    mot_blend_mot->field_4F8.field_B8 = 0.0f;
    mot_blend_mot->field_4F8.field_90 = vec3_identity;
    mot_blend_mot->field_4F8.field_9C = vec3_identity;
    mot_blend_mot->field_4F8.field_A8 = vec3_identity;
    mot_blend_mot->field_4F8.field_BC = 0;
    mot_blend_mot->field_4F8.field_BD = 0;
    mot_blend_mot->field_4F8.field_C0 = 1.0f;
    mot_blend_mot->field_4F8.field_C4 = 1.0f;
    mot_blend_mot->field_4F8.field_C8 = vec3_null;
}

static void motion_blend_mot_dispose(motion_blend_mot* mot_blend_mot) {
    mot_parent_free(&mot_blend_mot->mot_data);
    bone_data_parent_free(&mot_blend_mot->bone_data);
    free(mot_blend_mot);
}

static void osage_coli_set(osage_coli* osg_coli,
    skin_param_osage_root_coli* skp_root_coli, mat4* mats) {
    osage_coli* v4 = osg_coli;
    skin_param_osage_root_coli* v6 = skp_root_coli;
    osg_coli->type = 0;
    if (skp_root_coli && mats && skp_root_coli->type)
        while (v6->type) {
            v4->type = v6->type;
            v4->radius = v6->radius;

            mat4 mat = mats[v6->bone0_index];
            mat4_mult_vec3_trans(&mat, &v4->bone0_pos, &v4->bone0_pos);

            if (v6->type == 2 || v6->type == 4) {
                mat = mats[v6->bone1_index];
                mat4_mult_vec3_trans(&mat, &v6->bone1_pos, &v4->bone1_pos);
                vec3_sub(v4->bone1_pos, v4->bone0_pos, v4->bone_pos_diff);

                float_t length;
                vec3_length_squared(v4->bone_pos_diff, length);
                v4->bone_pos_diff_length_squared = length;
                length = sqrtf(length);
                v4->bone_pos_diff_length = length;
                if (length < 0.01f)
                    v4->type = 1;
            }
            else if (v6->type == 3)
                mat4_mult_vec3(&mat, &v4->bone1_pos, &v4->bone1_pos);

            v4++;
            v6++;
        }
    v4->type = 0;
}

static void osage_coli_ring_set(osage_coli* osg_coli,
    vector_skin_param_osage_root_coli* vec_skp_root_coli, mat4* mats) {
    if (vec_skp_root_coli->begin == vec_skp_root_coli->end) {
        osg_coli->type = 0;
        osg_coli->radius = 0.0f;
        osg_coli->bone0_pos = vec3_null;
        osg_coli->bone1_pos = vec3_null;
        osg_coli->bone_pos_diff = vec3_null;
        osg_coli->bone_pos_diff_length = 0.0f;
        osg_coli->bone_pos_diff_length_squared = 0.0f;
        osg_coli->field_34 = 1.0;
    }
    else
        osage_coli_set(osg_coli, vec_skp_root_coli->begin, mats);
}

static rob_chara_data_bone_data* rob_chara_data_bone_data_init() {
    rob_chara_data_bone_data* rob_bone_data = force_malloc(sizeof(rob_chara_data_bone_data));
    rob_bone_data->base_skeleton_type = BONE_DATABASE_SKELETON_NONE;
    rob_bone_data->skeleton_type = BONE_DATABASE_SKELETON_NONE;
    rob_bone_data->ik_scale.ratio0 = 1.0f;
    return rob_bone_data;
}

static void rob_chara_data_bone_data_calculate_bones(rob_chara_data_bone_data* rob_bone_data,
    vector_bone_database_bone* bones) {
    bone_database_bones_calculate_count(bones, &rob_bone_data->object_bone_count,
        &rob_bone_data->motion_bone_count, &rob_bone_data->total_bone_count,
        &rob_bone_data->ik_bone_count, &rob_bone_data->chain_pos);
}

static void rob_chara_data_bone_data_eyes_xrot_adjust(rob_chara_data_bone_data* rob_bone_data,
    const struc_241* a2, eyes_adjust* a3) {
    float_t v1_neg;
    float_t v1_pos;
    switch (a3->base_adjust) {
    case EYES_BASE_ADJUST_DIRECTION:
    default:
        v1_neg = a2->field_34 * -(float_t)(1.0 / 3.8);
        v1_pos = a2->field_38 * (float_t)(1.0 / 6.0);
        break;
    case EYES_BASE_ADJUST_CLEARANCE:
        v1_neg = a2->field_3C * -(float_t)(1.0 / 3.8);
        v1_pos = a2->field_40 * (float_t)(1.0 / 6.0);
        break;
    case EYES_BASE_ADJUST_OFF:
        v1_neg = 1.0f;
        v1_pos = 1.0f;
        break;
    }

    float_t v2_pos = 1.0f;
    float_t v2_neg = 1.0f;
    if (a3->xrot_adjust) {
        v2_neg = a2->field_2C;
        v2_pos = a2->field_30;
    }

    if (a3->neg >= 0.0f && a3->pos >= 0.0f) {
        v2_neg = a3->neg;
        v2_pos = a3->pos;
    }

    float_t eyes_xrot_adjust_neg = v1_neg * v2_neg;
    float_t eyes_xrot_adjust_pos = v1_pos * v2_pos;
    for (motion_blend_mot** i = rob_bone_data->motions.begin; i != rob_bone_data->motions.end; i++) {
        bone_data* data = (*i)->bone_data.bones.begin;
        data[MOTION_BONE_KL_EYE_L].eyes_xrot_adjust_neg = eyes_xrot_adjust_neg;
        data[MOTION_BONE_KL_EYE_L].eyes_xrot_adjust_pos = eyes_xrot_adjust_pos;
        data[MOTION_BONE_KL_EYE_R].eyes_xrot_adjust_neg = eyes_xrot_adjust_neg;
        data[MOTION_BONE_KL_EYE_R].eyes_xrot_adjust_pos = eyes_xrot_adjust_pos;
    }
}

static float_t rob_chara_data_bone_data_get_frame(rob_chara_data_bone_data* rob_bone_data) {
    return rob_bone_data->motion_loaded.begin[0]->field_4A8.field_0.frame;
}

static float_t rob_chara_data_bone_data_get_frame_count(rob_chara_data_bone_data* rob_bone_data) {
    return (float_t)rob_bone_data->motion_loaded.begin[0]->mot_data.mot.frame_count;
}

static void rob_chara_data_bone_data_get_ik_scale(
    rob_chara_data_bone_data* rob_bone_data, bone_database* bone_data) {
    if (rob_bone_data->motion_loaded.end - rob_bone_data->motion_loaded.begin < 0)
        return;

    motion_blend_mot* v2 = rob_bone_data->motion_loaded.begin[0];
    rob_chara_data_bone_data_ik_scale_calculate(&rob_bone_data->ik_scale, &v2->bone_data.bones,
        rob_bone_data->base_skeleton_type, rob_bone_data->skeleton_type, bone_data);
    float_t ratio0 = rob_bone_data->ik_scale.ratio0;
    v2->field_4F8.field_C0 = ratio0;
    v2->field_4F8.field_C4 = ratio0;
    v2->field_4F8.field_C8 = vec3_null;
}

static mat4* rob_chara_data_bone_data_get_mat(rob_chara_data_bone_data* rob_bone_data, size_t index) {
    bone_node* node = rob_chara_data_bone_data_get_node(rob_bone_data, index);
    if (node)
        return node->mat;
    return 0;
}

static bone_node* rob_chara_data_bone_data_get_node(rob_chara_data_bone_data* rob_bone_data, size_t index) {
    if ((ssize_t)index < rob_bone_data->nodes.end - rob_bone_data->nodes.begin)
        return &rob_bone_data->nodes.begin[index];
    return 0;
}

static void rob_chara_data_bone_data_ik_scale_calculate(
    rob_chara_data_bone_data_ik_scale* ik_scale, vector_bone_data* vec_bone_data,
    bone_database_skeleton_type base_skeleton_type,
    bone_database_skeleton_type skeleton_type, bone_database* bone_db) {
    char* base_name = bone_database_skeleton_type_to_string(base_skeleton_type);
    char* name = bone_database_skeleton_type_to_string(skeleton_type);
    float_t* base_heel_height = 0;
    float_t* heel_height = 0;
    if (!bone_database_get_skeleton_heel_height(bone_db, base_name, &base_heel_height)
        || !bone_database_get_skeleton_heel_height(bone_db, name, &heel_height))
        return;

    bone_data* b_cl_mune = &vec_bone_data->begin[MOTION_BONE_CL_MUNE];
    bone_data* b_kl_kubi = &vec_bone_data->begin[MOTION_BONE_KL_KUBI];
    bone_data* b_c_kata_l = &vec_bone_data->begin[MOTION_BONE_C_KATA_L];
    bone_data* b_cl_momo_l = &vec_bone_data->begin[MOTION_BONE_CL_MOMO_L];

    float_t base_height = fabsf(b_cl_momo_l->base_translation[0].y) + b_cl_momo_l->ik_segment_length[0]
        + b_cl_momo_l->ik_2nd_segment_length[0] + *base_heel_height;
    float_t height = fabsf(b_cl_momo_l->base_translation[1].y) + b_cl_momo_l->ik_segment_length[1]
        + b_cl_momo_l->ik_2nd_segment_length[1] + *heel_height;
    ik_scale->ratio0 = height / base_height;
    ik_scale->ratio1 = (fabsf(b_kl_kubi->base_translation[1].y) + b_cl_mune->ik_segment_length[1])
        / (fabsf(b_kl_kubi->base_translation[0].y) + b_cl_mune->ik_segment_length[0]);
    ik_scale->ratio2 = (b_c_kata_l->ik_segment_length[1] + b_c_kata_l->ik_2nd_segment_length[1])
        / (b_c_kata_l->ik_segment_length[0] + b_c_kata_l->ik_2nd_segment_length[0]);
    ik_scale->ratio3 = (fabsf(b_kl_kubi->base_translation[1].y) + b_cl_mune->ik_segment_length[1] + height)
        / (fabsf(b_kl_kubi->base_translation[0].y) + b_cl_mune->ik_segment_length[0] + base_height);
}

static void rob_chara_data_bone_data_init_data(rob_chara_data_bone_data* rob_bone_data,
    bone_database_skeleton_type base_skeleton_type,
    bone_database_skeleton_type skeleton_type, bone_database* bone_data) {
    rob_chara_data_bone_data_init_skeleton(rob_bone_data, base_skeleton_type, skeleton_type, bone_data);
    for (motion_blend_mot** i = rob_bone_data->motions.begin; i != rob_bone_data->motions.end; i++)
        motion_blend_mot_load_bone_data(*i, rob_bone_data, bone_data);
}

static void rob_chara_data_bone_data_init_skeleton(rob_chara_data_bone_data* rob_bone_data,
    bone_database_skeleton_type base_skeleton_type,
    bone_database_skeleton_type skeleton_type, bone_database* bone_data) {
    if (rob_bone_data->base_skeleton_type == base_skeleton_type
        && rob_bone_data->skeleton_type == skeleton_type)
        return;

    char* name = bone_database_skeleton_type_to_string(base_skeleton_type);
    vector_bone_database_bone* bones = 0;
    vector_uint16_t* parent_indices = 0;
    if (!bone_database_get_skeleton_bones(bone_data, name, &bones)
        || !bone_database_get_skeleton_parent_indices(bone_data, name, &parent_indices))
        return;

    rob_chara_data_bone_data_calculate_bones(rob_bone_data, bones);
    rob_chara_data_bone_data_reserve(rob_bone_data);
    rob_chara_data_bone_data_set_mats(rob_bone_data, bones);
    rob_chara_data_bone_data_set_parent_mats(rob_bone_data, parent_indices->begin);
    rob_bone_data->base_skeleton_type = base_skeleton_type;
    rob_bone_data->skeleton_type = skeleton_type;
}

static void rob_chara_data_bone_data_interpolate(rob_chara_data_bone_data* rob_bone_data) {
    if (rob_bone_data->motion_loaded.end - rob_bone_data->motion_loaded.begin < 0)
        return;

    for (motion_blend_mot** i = rob_bone_data->motion_loaded.begin;
        i != rob_bone_data->motion_loaded.end; i++)
        motion_blend_mot_interpolate(*i);
}

static void rob_chara_data_bone_data_motion_blend_mot_free(rob_chara_data_bone_data* rob_bone_data) {
    vector_size_t_clear(&rob_bone_data->motion_indices, 0);
    vector_ptr_motion_blend_mot_clear(&rob_bone_data->motions, motion_blend_mot_dispose);
}

static void rob_chara_data_bone_data_motion_blend_mot_init(rob_chara_data_bone_data* rob_bone_data) {
    for (int32_t i = 0; i < 3; i++) {
        *vector_ptr_motion_blend_mot_reserve_back(&rob_bone_data->motions) = motion_blend_mot_init();
        *vector_size_t_reserve_back(&rob_bone_data->motion_indices) = i;
    }
    rob_chara_data_bone_data_motion_blend_mot_list_init(rob_bone_data);
}

static void rob_chara_data_bone_data_motion_blend_mot_list_free(rob_chara_data_bone_data* rob_bone_data,
    size_t last_index) {
    for (motion_blend_mot** i = &rob_bone_data->motion_loaded.end[-1];
        i - rob_bone_data->motion_loaded.begin > (ssize_t)last_index; i--) {
        motion_blend_mot* type = rob_bone_data->motion_loaded.end[-1];
        if ((ssize_t)rob_bone_data->motion_loaded_indices.end[-1]
            >= rob_bone_data->motions.end - rob_bone_data->motions.begin && type)
            motion_blend_mot_dispose(type);
        rob_bone_data->motion_loaded.end[-1] = 0;
        vector_ptr_motion_blend_mot_pop_back(&rob_bone_data->motion_loaded, 0);
        vector_size_t_pop_back(&rob_bone_data->motion_loaded_indices, 0);
    }
}

static void rob_chara_data_bone_data_motion_blend_mot_list_init(rob_chara_data_bone_data* rob_bone_data) {
    size_t free_index = rob_bone_data->motion_indices.end[-1];
    vector_size_t_pop_back(&rob_bone_data->motion_indices, 0);
    vector_ptr_motion_blend_mot_push_back(&rob_bone_data->motion_loaded,
        &rob_bone_data->motions.begin[free_index]);
    vector_size_t_push_back(&rob_bone_data->motion_loaded_indices, &free_index);
}

static void rob_chara_data_bone_data_reserve(rob_chara_data_bone_data* rob_bone_data) {
    size_t mats_size = rob_bone_data->object_bone_count;
    vector_mat4_reserve(&rob_bone_data->mats, mats_size);
    rob_bone_data->mats.end += mats_size;

    size_t mats2_size = rob_bone_data->total_bone_count - rob_bone_data->object_bone_count;
    vector_mat4_reserve(&rob_bone_data->mats2, mats2_size);
    rob_bone_data->mats2.end += mats2_size;

    size_t nodes_size = rob_bone_data->total_bone_count;
    vector_bone_node_reserve(&rob_bone_data->nodes, nodes_size);
    rob_bone_data->nodes.end += nodes_size;

    for (bone_node* i = rob_bone_data->nodes.begin; i != rob_bone_data->nodes.end; i++) {
        i->name = 0;
        i->mat = 0;
        i->parent = 0;
        i->exp_data.position = vec3_null;
        i->exp_data.rotation = vec3_null;
        i->exp_data.scale = vec3_identity;
        i->exp_data.data_scale = vec3_identity;
        i->ex_data_mat = 0;
    }
}

static void rob_chara_data_bone_data_motion_load(rob_chara_data_bone_data* rob_bone_data,
    int32_t motion_id, int32_t index, bone_database* bone_data, motion_database* mot_db) {
    if (rob_bone_data->motion_loaded.end - rob_bone_data->motion_loaded.begin < 1)
        return;

    if (index == 1) {
        rob_chara_data_bone_data_motion_blend_mot_list_free(rob_bone_data, 1);
        motion_blend_mot_load_file(rob_bone_data->motion_loaded.begin[0], motion_id, bone_data, mot_db);
        return;
    }

    if (index != 2) {
        if (index == 3)
            index = 0;
        rob_chara_data_bone_data_motion_blend_mot_list_free(rob_bone_data, 1);
        motion_blend_mot_load_file(rob_bone_data->motion_loaded.begin[0], motion_id, bone_data, mot_db);
        return;
    }

    rob_chara_data_bone_data_motion_blend_mot_list_free(rob_bone_data, 2);
    if (rob_bone_data->motion_indices.end - rob_bone_data->motion_indices.begin > 0) {
        rob_chara_data_bone_data_motion_blend_mot_list_init(rob_bone_data);
        motion_blend_mot_load_file(rob_bone_data->motion_loaded.begin[0], motion_id, bone_data, mot_db);
        return;
    }

    motion_blend_mot* v16 = motion_blend_mot_init();
    motion_blend_mot_load_bone_data(v16, rob_bone_data, bone_data);
    vector_ptr_motion_blend_mot_push_back(&rob_bone_data->motion_loaded, &v16);
    *vector_size_t_reserve_back(&rob_bone_data->motion_loaded_indices)
        = (size_t)(rob_bone_data->motions.end - rob_bone_data->motions.begin);
    motion_blend_mot_load_file(rob_bone_data->motion_loaded.begin[0], motion_id, bone_data, mot_db);
}

static void rob_chara_data_bone_data_reset(rob_chara_data_bone_data* rob_bone_data) {
    rob_bone_data->object_bone_count = 0;
    rob_bone_data->motion_bone_count = 0;
    rob_bone_data->total_bone_count = 0;
    rob_bone_data->ik_bone_count = 0;
    rob_bone_data->chain_pos = 0;
    rob_bone_data->mats.end = rob_bone_data->mats.begin;
    rob_bone_data->mats2.end = rob_bone_data->mats2.begin;
    rob_bone_data->nodes.end = rob_bone_data->nodes.begin;
    rob_bone_data->base_skeleton_type = BONE_DATABASE_SKELETON_NONE;
    rob_bone_data->skeleton_type = BONE_DATABASE_SKELETON_NONE;

    if (rob_bone_data->motions.end - rob_bone_data->motions.begin == 3) {
        rob_chara_data_bone_data_motion_blend_mot_list_free(rob_bone_data, 0);
        for (motion_blend_mot** i = rob_bone_data->motions.begin; i != rob_bone_data->motions.end; i++)
            motion_blend_mot_reset(*i);
        rob_chara_data_bone_data_motion_blend_mot_list_init(rob_bone_data);
    }
    else {
        rob_chara_data_bone_data_motion_blend_mot_free(rob_bone_data);
        rob_chara_data_bone_data_motion_blend_mot_init(rob_bone_data);
    }
}

static void sub_140414BC0(struc_346* a1, float_t frame) {
    float_t v2 = a1->field_14;
    a1->frame = frame;
    a1->field_28 = false;
    if (v2 >= 0.0f && (a1->field_18 == 2 && frame <= v2) || frame >= v2)
        a1->field_14 = -1.0;
}

static void rob_chara_data_bone_data_set_frame(rob_chara_data_bone_data* rob_bone_data, float_t frame) {
    sub_140414BC0(&rob_bone_data->motion_loaded.begin[0]->field_4A8.field_0, frame);
}

static void rob_chara_data_bone_data_set_mats(rob_chara_data_bone_data* rob_bone_data,
    vector_bone_database_bone* bones) {
    size_t chain_pos = 0;
    size_t ik_bone_count = 0;
    size_t total_bone_count = 0;

    mat4* mats = rob_bone_data->mats.begin;
    mat4* mats2 = rob_bone_data->mats2.begin;
    bone_node* nodes = rob_bone_data->nodes.begin;
    for (bone_database_bone* i = bones->begin; i != bones->end; i++)
        switch (i->type) {
        case BONE_DATABASE_BONE_ROTATION:
        case BONE_DATABASE_BONE_TYPE_1:
        case BONE_DATABASE_BONE_POSITION:
            (nodes++)->mat = mats++;

            chain_pos++;
            total_bone_count++;
            break;
        case BONE_DATABASE_BONE_POSITION_ROTATION:
            (nodes++)->mat = mats++;

            chain_pos++;
            ik_bone_count++;
            total_bone_count++;
            break;
        case BONE_DATABASE_BONE_HEAD_IK_ROTATION:
            (nodes++)->mat = mats2++;
            (nodes++)->mat = mats++;
            (nodes++)->mat = mats2++;

            chain_pos += 2;
            ik_bone_count++;
            total_bone_count += 3;
            break;
        case BONE_DATABASE_BONE_ARM_IK_ROTATION:
        case BONE_DATABASE_BONE_LEGS_IK_ROTATION:
            (nodes++)->mat = mats++;
            (nodes++)->mat = mats++;
            (nodes++)->mat = mats++;
            (nodes++)->mat = mats2++;

            chain_pos += 3;
            ik_bone_count++;
            total_bone_count += 4;
            break;
        }

    for (bone_node* i = rob_bone_data->nodes.begin; i != rob_bone_data->nodes.end; i++)
        i->ex_data_mat = i->mat;
}

static void rob_chara_data_bone_data_set_parent_mats(rob_chara_data_bone_data* rob_bone_data,
    uint16_t* parent_indices) {
    if (rob_bone_data->nodes.end - rob_bone_data->nodes.begin < 1)
        return;

    parent_indices++;
    for (bone_node* i = &rob_bone_data->nodes.begin[1]; i != rob_bone_data->nodes.end; i++)
        i->parent = &rob_bone_data->nodes.begin[*parent_indices++];
}

static void rob_chara_data_bone_data_set_rotation_y(rob_chara_data_bone_data* rob_bone_data, float_t rotation_y) {
    rob_bone_data->motion_loaded.begin[0]->bone_data.rotation_y = rotation_y;
}

static void rob_chara_data_bone_data_update(rob_chara_data_bone_data* rob_bone_data, mat4* mat) {
    if (rob_bone_data->motion_loaded_indices.end - rob_bone_data->motion_loaded_indices.begin < 1)
        return;

    for (motion_blend_mot** i = rob_bone_data->motion_loaded.begin;
        i != rob_bone_data->motion_loaded.end; i++) {
        if (!*i)
            continue;

        motion_blend_mot* v5 = *i;
        v5->field_4F8.field_8C = false;
        v5->field_4F8.field_4C = v5->field_4F8.mat;
        v5->field_4F8.mat = mat4u_identity;
        int32_t skeleton_select = v5->mot_data.skeleton_select;

        for (bone_data* j = v5->bone_data.bones.begin; j != v5->bone_data.bones.end; j++)
            bone_data_mult_0(j, skeleton_select);

        float_t ratio = rob_bone_data->ik_scale.ratio0;
        bone_data* b_n_hara_cp = &v5->bone_data.bones.begin[MOTION_BONE_N_HARA_CP];
        bone_data* b_kg_hara_y = &v5->bone_data.bones.begin[MOTION_BONE_KG_HARA_Y];
        bone_data* b_kl_hara_xz = &v5->bone_data.bones.begin[MOTION_BONE_KL_HARA_XZ];
        bone_data* b_kl_hara_etc = &v5->bone_data.bones.begin[MOTION_BONE_KL_HARA_ETC];
        v5->field_4F8.field_90 = vec3_null;

        mat4 rot_mat;
        mat4 mat;
        mat4u_to_mat4(&b_n_hara_cp->rot_mat[0], &rot_mat);
        mat4u_to_mat4(&b_kg_hara_y->rot_mat[0], &mat);
        mat4_mult(&mat, &rot_mat, &rot_mat);
        mat4u_to_mat4(&b_kl_hara_xz->rot_mat[0], &mat);
        mat4_mult(&mat, &rot_mat, &rot_mat);
        mat4u_to_mat4(&b_kl_hara_etc->rot_mat[0], &mat);
        mat4_mult(&mat, &rot_mat, &rot_mat);
        mat4_to_mat4u(&rot_mat, &b_n_hara_cp->rot_mat[0]);
        b_kg_hara_y->rot_mat[0] = mat4u_identity;
        b_kl_hara_xz->rot_mat[0] = mat4u_identity;
        b_kl_hara_etc->rot_mat[0] = mat4u_identity;

        float_t v8 = v5->field_4F8.field_C0;
        float_t v9 = v5->field_4F8.field_C4;
        vec3 v10 = v5->field_4F8.field_C8;
        v5->field_4F8.field_A8 = b_n_hara_cp->trans;
        if (!v5->mot_data.skeleton_select) {
            if (ratio != v9) {
                b_n_hara_cp->trans.x = (b_n_hara_cp->trans.x - v10.x) * v9 + v10.x;
                b_n_hara_cp->trans.z = (b_n_hara_cp->trans.z - v10.z) * v9 + v10.z;
            }
            b_n_hara_cp->trans.y = ((b_n_hara_cp->trans.y - v10.y) * v8) + v10.y;
        }
        else {
            if (ratio != v9) {
                v9 /= ratio;
                b_n_hara_cp->trans.x = (b_n_hara_cp->trans.x - v10.x) * v9 + v10.x;
                b_n_hara_cp->trans.z = (b_n_hara_cp->trans.z - v10.z) * v9 + v10.z;
            }

            if (ratio != v8) {
                v8 /= ratio;
                b_n_hara_cp->trans.y = (b_n_hara_cp->trans.y - v10.y) * v8 + v10.y;
            }
        }
    }

    motion_blend_mot_mult_mat(rob_bone_data->motion_loaded.begin[0], mat);
}

static void rob_chara_data_bone_data_dispose(rob_chara_data_bone_data* rob_bone_data) {
    vector_mat4_free(&rob_bone_data->mats, 0);
    vector_mat4_free(&rob_bone_data->mats2, 0);
    vector_bone_node_free(&rob_bone_data->nodes, 0);
    vector_ptr_motion_blend_mot_free(&rob_bone_data->motions, motion_blend_mot_dispose);
    vector_size_t_free(&rob_bone_data->motion_indices, 0);
    vector_size_t_free(&rob_bone_data->motion_loaded_indices, 0);
    for (motion_blend_mot** i = rob_bone_data->motion_loaded.begin;
        i != rob_bone_data->motion_loaded.end; i++)
        *i = 0;
    vector_ptr_motion_blend_mot_free(&rob_bone_data->motion_loaded, 0);
    free(rob_bone_data);
}

static rob_chara_data_item_equip* rob_chara_data_item_equip_init() {
    rob_chara_data_item_equip* rob_item_equip = force_malloc(sizeof(rob_chara_data_item_equip));
    rob_chara_data_item_equip_object* itm_eq_obj = force_malloc_s(rob_chara_data_item_equip_object, 31);
    for (int32_t i = 0; i < 31; i++)
        rob_chara_data_item_equip_object_init(&itm_eq_obj[i]);
    rob_item_equip->item_equip_object = itm_eq_obj;
    rob_item_equip->wind_strength = 1.0f;
    rob_item_equip->chara_color = 1;
    for (int32_t i = 0; i < 31; i++)
        rob_item_equip->item_equip_object[i].item_equip = rob_item_equip;
    rob_chara_data_item_equip_reset(rob_item_equip);
    return rob_item_equip;
}

static float_t sub_140512F60(rob_chara_data_item_equip* rob_item_equip) {
    float_t v26; 
    float_t v27;
    float_t v28;

    mat4* mats = rob_item_equip->matrices;
    float_t v25 = 1.0f;
    float_t* v2 = &v27;
    float_t v3 = mats->row3.y;
    v26 = mats->row3.x;
    if (v3 >= 1.0f)
        v2 = &v25;
    v28 = mats->row3.z;
    float_t v4 = mats[175].row3.x;
    v27 = v3;
    float_t v5 = *v2;
    float_t v6 = mats[175].row3.y;
    v26 = v4;
    float_t* v7 = &v27;
    float_t v8 = mats[175].row3.z;
    v27 = v6;
    if (v5 <= v6)
        v7 = &v25;
    float_t v9 = mats[181].row3.y;
    v28 = v8;
    float_t v10 = mats[181].row3.x;
    v25 = v5;
    float_t v11 = *v7;
    v26 = v10;
    float_t* v12 = &v27;
    float_t v13 = mats[181].row3.z;
    v27 = v9;
    if (v11 <= v9)
        v12 = &v25;
    float_t v14 = mats[105].row3.y;
    v28 = v13;
    float_t v15 = mats[105].row3.x;
    v25 = v11;
    float_t v16 = *v12;
    v26 = v15;
    float_t*v17 = &v27;
    float_t v18 = mats[105].row3.z;
    v27 = v14;
    if (v16 <= v14)
        v17 = &v25;
    float_t v19 = mats[140].row3.y;
    v28 = v18;
    float_t v20 = mats[140].row3.x;
    v25 = v16;
    float_t v21 = *v17;
    v26 = v20;
    float_t* v22 = &v27;
    float_t v23 = mats[140].row3.z;
    v25 = v21;
    if (v21 <= v19)
        v22 = &v25;
    v27 = v19;
    v28 = v23;
    return *v22;
}

static void rob_chara_data_item_equip_draw(
    rob_chara_data_item_equip* rob_item_equip, int32_t chara_id, render_context* rctx) {
    draw_task_flags v2 = 0;
    if (rctx->chara_reflect)
        v2 = DRAW_TASK_CHARA_REFLECT;
    if (rctx->chara_refract)
        v2 |= DRAW_TASK_REFRACT;

    object_data* object_data = &rctx->object_data;
    shadow* shad = rctx->draw_pass.shadow_ptr;
    if (rob_item_equip->shadow_type != -1) {
        if (rob_item_equip->field_A0 & 4) {
            shadow_type_enum shadow_type = rob_item_equip->shadow_type;
            vec3 pos = rob_item_equip->position;
            pos.y -= 0.2f;
            vector_vec3_push_back(&shad->field_1D0[shadow_type], &pos);

            float_t v9;
            if (sub_140512F60(rob_item_equip) <= -0.2f)
                v9 = -0.5f;
            else
                v9 = 0.05f;
            rctx->draw_pass.shadow_ptr->field_1C0[shadow_type] = v9;
            object_data_set_shadow_type(object_data, shadow_type);
            v2 |= DRAW_TASK_SHADOW;
        }

        if (rob_item_equip->field_A0 & 1)
            v2 |= DRAW_TASK_4;
    }
    object_data_set_draw_task_flags(object_data, v2);
    object_data_set_chara_color(object_data, rob_item_equip->chara_color);

    vec4 v23;
    object_data_get_texture_color_coeff(object_data, &v23);

    vec4 texture_color_coeff;
    vec4u_to_vec4(rob_item_equip->texture_color_coeff, texture_color_coeff);
    object_data_set_texture_color_coeff(object_data, &texture_color_coeff);
    object_data_set_wet_param(object_data, rob_item_equip->wet);
    //sub_140502FB0(rob_item_equip->field_F9);
    //sub_140512C20(rob_item_equip);
    //sub_1405421D0(chara_id, rctx->chara_reflect, rob_item_equip->chara_color);

    mat4 mat;
    mat4u_to_mat4(&rob_item_equip->mat, &mat);
    if (rob_item_equip->item_equip_range)
        for (int32_t i = rob_item_equip->first_item_equip_object;
            i < rob_item_equip->max_item_equip_object; i++)
            rob_chara_data_item_equip_object_draw(&rob_item_equip->item_equip_object[i], &mat, rctx);
    else {
        for (int32_t i = 1; i < 31; i++) {
            draw_task_flags v18 = 0;
            if (rob_item_equip->field_18[i] == 0) {
                if (rctx->chara_reflect)
                    v18 = DRAW_TASK_CHARA_REFLECT;
                if (rctx->chara_refract)
                    v18 |= DRAW_TASK_REFRACT;
            }

            draw_task_flags v19 = DRAW_TASK_4 | DRAW_TASK_SHADOW;
            if (i == 8)
                v19 = 0;

            if (!(rob_item_equip->field_A0 & 4))
                v19 &= ~DRAW_TASK_SHADOW;

            object_data_set_draw_task_flags(object_data, (draw_task_flags)(v18 | v19 | DRAW_TASK_SSS));
            rob_chara_data_item_equip_object_draw(&rob_item_equip->item_equip_object[i], &mat, rctx);
        }
    }
    object_data_set_texture_color_coeff(object_data, &v23);
    object_data_set_wet_param(object_data, 0.0f);
    object_data_set_chara_color(object_data, 0);
    object_data_set_draw_task_flags(object_data, 0);
    object_data_set_shadow_type(object_data, SHADOW_CHARA);
}

static void rob_chara_data_item_equip_get_parent_bone_nodes(
    rob_chara_data_item_equip* rob_item_equip, bone_node* bone_nodes, bone_database* bone_data) {
    rob_item_equip->bone_nodes = bone_nodes;
    rob_item_equip->matrices = bone_nodes->mat;
    for (int32_t i = rob_item_equip->first_item_equip_object;
        i < rob_item_equip->max_item_equip_object; i++)
        rob_chara_data_item_equip_object_get_parent_bone_nodes(
            &rob_item_equip->item_equip_object[i], rob_item_equip->bone_nodes, bone_data);
}

static void rob_chara_data_item_equip_load_object_info(rob_chara_data_item_equip* rob_item_equip,
    object_info object_info, item_id id, bone_database* bone_data) {
    if (id < ITEM_BODY || id > ITEM_ITEM16 || !rob_item_equip->bone_nodes)
        return;

    rob_chara_data_item_equip_object_load_object_info_ex_data(
        &rob_item_equip->item_equip_object[id], object_info, rob_item_equip->bone_nodes, bone_data);
    rob_item_equip->item_equip_object[id].draw = true;
}

static void rob_chara_data_item_equip_object_init(rob_chara_data_item_equip_object* itm_eq_obj) {
    memset(itm_eq_obj, 0, sizeof(rob_chara_data_item_equip_object));
    bone_node_expression_data_reset_position_rotation(&itm_eq_obj->exp_data);
    rob_chara_data_item_equip_object_init_data(itm_eq_obj, 0x12345678);
}

static void rob_chara_data_item_equip_object_clear_ex_data(rob_chara_data_item_equip_object* itm_eq_obj) {
    for (ex_osage_block** i = itm_eq_obj->osage_blocks.begin;
        i != itm_eq_obj->osage_blocks.end; i++) {
        (*i)->base.vftable->reset(&(*i)->base);
        if (*i)
            (*i)->base.vftable->dispose(&(*i)->base);
    }
    itm_eq_obj->osage_blocks.end = itm_eq_obj->osage_blocks.begin;

    for (ex_constraint_block** i = itm_eq_obj->constraint_blocks.begin;
        i != itm_eq_obj->constraint_blocks.end; i++) {
        (*i)->base.vftable->reset(&(*i)->base);
        if (*i)
            (*i)->base.vftable->dispose(&(*i)->base);
    }
    itm_eq_obj->constraint_blocks.end = itm_eq_obj->constraint_blocks.begin;

    for (ex_expression_block** i = itm_eq_obj->expression_blocks.begin;
        i != itm_eq_obj->expression_blocks.end; i++) {
        (*i)->base.vftable->reset(&(*i)->base);
        if (*i)
            (*i)->base.vftable->dispose(&(*i)->base);
    }
    itm_eq_obj->expression_blocks.end = itm_eq_obj->expression_blocks.begin;

    for (ex_cloth_block** i = itm_eq_obj->cloth_blocks.begin;
        i != itm_eq_obj->cloth_blocks.end; i++) {
        (*i)->base.vftable->reset(&(*i)->base);
        if (*i)
            (*i)->base.vftable->dispose(&(*i)->base);
    }
    itm_eq_obj->cloth_blocks.end = itm_eq_obj->cloth_blocks.begin;

    itm_eq_obj->node_blocks.end = itm_eq_obj->node_blocks.begin;
}

static void rob_chara_data_item_equip_object_draw(
    rob_chara_data_item_equip_object* itm_eq_obj, mat4* mat, render_context* rctx) {
    if (itm_eq_obj->object_info.id == -1 && itm_eq_obj->object_info.set_id == -1)
        return;

    object_data* object_data = &rctx->object_data;
    draw_task_flags v2 = object_data_get_draw_task_flags(object_data);
    draw_task_flags v4;
    if (fabsf(itm_eq_obj->alpha - 1.0f) > 0.000001f)
        v4 = v2 | itm_eq_obj->draw_task_flags;
    else
        v4 = v2 & ~(DRAW_TASK_40000 | DRAW_TASK_20000 | DRAW_TASK_10000);
    object_data_set_draw_task_flags(object_data, v4);
    if (itm_eq_obj->draw) {
        draw_task_add_draw_object_by_object_info_object_skin(rctx,
            itm_eq_obj->object_info,
            &itm_eq_obj->texture_pattern,
            &itm_eq_obj->texture_data,
            itm_eq_obj->alpha,
            itm_eq_obj->mat,
            itm_eq_obj->field_F0.begin,
            itm_eq_obj->field_A8,
            mat);

        for (ex_node_block** i = itm_eq_obj->node_blocks.begin;
            i != itm_eq_obj->node_blocks.end; i++)
            (*i)->vftable->draw(*i);
    }
    object_data_set_draw_task_flags(object_data, v2);
}

static int32_t rob_chara_data_item_equip_object_get_bone_index(
    rob_chara_data_item_equip_object* itm_eq_obj, char* name, bone_database* bone_data) {
    int32_t bone_index = bone_database_get_skeleton_motion_bone_index(bone_data,
        bone_database_skeleton_type_to_string(BONE_DATABASE_SKELETON_COMMON), name);
    if (bone_index == -1)
        for (ex_data_name_bone_index* i = itm_eq_obj->ex_bones.begin; i != itm_eq_obj->ex_bones.end; i++)
            if (!str_utils_compare(name, i->name))
                return 0x8000 | i->bone_index;
    return bone_index;
}

static bone_node* rob_chara_data_item_equip_object_get_bone_node(
    rob_chara_data_item_equip_object* itm_eq_obj, int32_t bone_index) {
    if (!(bone_index & 0x8000))
        return &itm_eq_obj->bone_nodes[bone_index & 0x7FFF];
    else if ((bone_index & 0x7FFF) < (itm_eq_obj->ex_data_bone_nodes.end - itm_eq_obj->ex_data_bone_nodes.begin))
        return &itm_eq_obj->ex_data_bone_nodes.begin[bone_index & 0x7FFF];
    return 0;
}

static bone_node* rob_chara_data_item_equip_object_get_bone_node_by_name(
    rob_chara_data_item_equip_object* itm_eq_obj, char* name, bone_database* bone_data) {
    return rob_chara_data_item_equip_object_get_bone_node(itm_eq_obj,
        rob_chara_data_item_equip_object_get_bone_index(itm_eq_obj, name, bone_data));
}

static void rob_chara_data_item_equip_object_get_parent_bone_nodes(
    rob_chara_data_item_equip_object* itm_eq_obj, bone_node* bone_nodes, bone_database* bone_data) {

    itm_eq_obj->bone_nodes = bone_nodes;
    itm_eq_obj->mat = bone_nodes->mat;
    for (ex_node_block** i = itm_eq_obj->node_blocks.begin; i != itm_eq_obj->node_blocks.end; i++)
        (*i)->parent_bone_node = rob_chara_data_item_equip_object_get_bone_node_by_name(
            itm_eq_obj, string_data(&(*i)->parent_name), bone_data);
}

static void rob_chara_data_item_equip_object_init_data(rob_chara_data_item_equip_object* itm_eq_obj, size_t index) {
    itm_eq_obj->index = index;
    itm_eq_obj->object_info = object_info_null;
    itm_eq_obj->field_14 = -1;
    itm_eq_obj->mat = 0;
    itm_eq_obj->texture_pattern.end = itm_eq_obj->texture_pattern.begin;
    itm_eq_obj->texture_data.field_0 = -1;
    itm_eq_obj->texture_data.texture_color_coeff = vec3_identity;
    itm_eq_obj->texture_data.texture_color_offset = vec3_null;
    itm_eq_obj->texture_data.texture_specular_coeff = vec3_identity;
    itm_eq_obj->texture_data.texture_specular_offset = vec3_null;
    itm_eq_obj->alpha = 1.0f;
    itm_eq_obj->draw_task_flags = DRAW_TASK_10000;
    itm_eq_obj->field_64 = 0;
    itm_eq_obj->draw = true;
    itm_eq_obj->field_A4 = 0;
    itm_eq_obj->field_A8 = 0;
    itm_eq_obj->bone_nodes = 0;
    rob_chara_data_item_equip_object_clear_ex_data(itm_eq_obj);
    itm_eq_obj->ex_data_bone_nodes.end = itm_eq_obj->ex_data_bone_nodes.begin;
    itm_eq_obj->field_F0.end = itm_eq_obj->field_F0.begin;
    itm_eq_obj->field_108.end = itm_eq_obj->field_108.begin;
    for (ex_data_name_bone_index* i = itm_eq_obj->ex_bones.begin; i != itm_eq_obj->ex_bones.end; i++)
        i->name = 0;
    itm_eq_obj->ex_bones.end = itm_eq_obj->ex_bones.begin;
    itm_eq_obj->field_1B8 = 0;
    itm_eq_obj->field_1C8 = 0;
    itm_eq_obj->field_1C0 = 0;
}

static void rob_chara_data_item_equip_object_init_ex_data_bone_nodes(
    rob_chara_data_item_equip_object* itm_eq_obj, object_skin_ex_data* a2) {
    itm_eq_obj->ex_data_bone_nodes.end = itm_eq_obj->ex_data_bone_nodes.begin;
    itm_eq_obj->field_F0.end = itm_eq_obj->field_F0.begin;
    int32_t count = 40;
    if (a2->bone_names_count > 40)
        count = a2->bone_names_count;

    vector_bone_node_reserve(&itm_eq_obj->ex_data_bone_nodes, count);
    itm_eq_obj->ex_data_bone_nodes.end += count;
    vector_mat4_reserve(&itm_eq_obj->field_F0, count);
    itm_eq_obj->field_F0.end += count;
    vector_mat4_reserve(&itm_eq_obj->field_108, count);
    itm_eq_obj->field_108.end += count;

    bone_node* v4 = itm_eq_obj->ex_data_bone_nodes.begin;
    mat4* v5 = itm_eq_obj->field_F0.begin;
    mat4* v8 = itm_eq_obj->field_108.begin;
    for (int32_t v10 = 0; v10 < count; v10++)
        v4[v10].mat = &v5[v10];

    if (a2->bone_names) {
        vector_ex_data_name_bone_index* v14 = &itm_eq_obj->ex_bones;
        memset(v14->begin, 0, sizeof(ex_data_name_bone_index) * (v14->end - v14->begin));
        for (ex_data_name_bone_index* i = v14->begin; i != v14->end; ++i)
            i->name = 0;
        v14->end = v14->begin;

        for (int32_t i = 0; i < a2->bone_names_count; i++) {
            bone_node* v38 = &v4[i];
            v38->name = a2->bone_names[i];
            v38->mat = &v5[i];
            *v38->mat = mat4_identity;
            mat4_translate(0.0f, 0.0f, -1000.0f, v38->mat);
            bone_node_expression_data_reset_position_rotation(&v38->exp_data);
            v38->ex_data_mat = &v8[i];
            *v38->ex_data_mat = mat4_identity;

            ex_data_name_bone_index* v35 = vector_ex_data_name_bone_index_reserve_back(v14);
            v35->name = a2->bone_names[i];
            v35->bone_index = i;
        }
    }
}

static void rob_chara_data_item_equip_object_load_ex_data(rob_chara_data_item_equip_object* itm_eq_obj,
    object_skin_ex_data* ex_data, bone_database* bone_data) {
    if (!ex_data->blocks)
        return;

    vector_pair_int32_t_ptr_rob_osage_node a4
        = vector_empty(pair_int32_t_ptr_rob_osage_node);
    vector_pair_name_ex_osage_block a5 = vector_empty(pair_name_ex_osage_block);
    rob_chara_data_item_equip_object_clear_ex_data(itm_eq_obj);

    size_t constraint_count = 0;
    size_t expression_count = 0;
    size_t osage_count = 0;
    size_t cloth_count = 0;
    char** bone_names_ptr = ex_data->bone_names;
    object_skin_block* v4 = ex_data->blocks;
    for (int32_t i = 0; i < ex_data->blocks_count; i++, v4++) {
        ex_node_block* node;
        if (v4->type == OBJECT_SKIN_BLOCK_CLOTH) {
            if (cloth_count >= 0x08)
                continue;

            ex_cloth_block* cls = ex_cloth_block_init();
            node = &cls->base;
            vector_ptr_ex_cloth_block_push_back(&itm_eq_obj->cloth_blocks, &cls);
            cls->index = cloth_count + osage_count;
            cloth_count++;
            continue;
        }
        else if (v4->type == OBJECT_SKIN_BLOCK_CONSTRAINT) {
            if (constraint_count >= 0x40)
                continue;

            ex_constraint_block* cns = ex_constraint_block_init();
            node = &cns->base;
            vector_ptr_ex_constraint_block_push_back(&itm_eq_obj->constraint_blocks, &cns);
            ex_constraint_block_init_data(cns, itm_eq_obj, &v4->constraint,
                bone_names_ptr[v4->constraint.name_index & 0x7FFF], bone_data);
            constraint_count++;
        }
        else if (v4->type == OBJECT_SKIN_BLOCK_EXPRESSION) {
            if (expression_count >= 0x50)
                continue;

            ex_expression_block* exp = ex_expression_block_init();
            node = &exp->base;
            vector_ptr_ex_expression_block_push_back(&itm_eq_obj->expression_blocks, &exp);
            ex_expression_block_init_data(exp, itm_eq_obj, &v4->expression,
                bone_names_ptr[v4->expression.name_index & 0x7FFF],
                itm_eq_obj->object_info, itm_eq_obj->index, bone_data);
            expression_count++;
        }
        else if (v4->type == OBJECT_SKIN_BLOCK_OSAGE) {
            if (osage_count >= 0x100)
                continue;

            ex_osage_block* osg = ex_osage_block_init();
            node = &osg->base;
            vector_ptr_ex_osage_block_push_back(&itm_eq_obj->osage_blocks, &osg);
            ex_osage_block_init_data(osg, itm_eq_obj, &v4->osage,
                bone_names_ptr[v4->constraint.name_index & 0x7FFF],
                &itm_eq_obj->skin_ex_data->osage_nodes[v4->osage.start_index],
                itm_eq_obj->bone_nodes, itm_eq_obj->ex_data_bone_nodes.begin, itm_eq_obj->skin);
            sub_1405F3E10(osg, &v4->osage, &itm_eq_obj->skin_ex_data->osage_nodes[v4->osage.start_index], &a4, &a5);
            osg->index = osage_count;
            osage_count++;
        }
        else
            continue;

        object_skin_block_node* v54 = &v4->base;
        bone_node_expression_data exp_data;
        exp_data.position = v54->position;
        exp_data.rotation = v54->rotation;
        exp_data.scale = v54->scale;
        exp_data.data_scale = vec3_identity;
        string_copy(&v54->parent_name, &node->parent_name);

        bone_node* parent_bone_node = rob_chara_data_item_equip_object_get_bone_node_by_name(itm_eq_obj,
            string_data(&v54->parent_name), bone_data);
        node->parent_bone_node = parent_bone_node;
        if (node->bone_node) {
            node->bone_node->exp_data = exp_data;
            node->bone_node->parent = parent_bone_node;
        }
        vector_ptr_ex_node_block_push_back(&itm_eq_obj->node_blocks, &node);
    }

    for (ex_node_block** i = itm_eq_obj->node_blocks.begin; i != itm_eq_obj->node_blocks.end; i++) {
        ex_node_block* v86 = (*i)->parent_node;
        if (v86) {
            v86->field_5A = true;
            if (v86->type == EX_OSAGE || v86->type == EX_CLOTH || !v86->field_58)
                continue;
        }
        (*i)->field_58 = true;
    }
    vector_pair_int32_t_ptr_rob_osage_node_free(&a4, 0);
    vector_pair_name_ex_osage_block_free(&a5, 0);
}

static void rob_chara_data_item_equip_object_load_object_info_ex_data(
    rob_chara_data_item_equip_object* itm_eq_obj, object_info object_info,
    bone_node* bone_nodes, bone_database* bone_data) {
    itm_eq_obj->object_info = object_info;
    itm_eq_obj->bone_nodes = bone_nodes;
    itm_eq_obj->mat = bone_nodes->mat;
    itm_eq_obj->ex_data_bone_nodes.end = itm_eq_obj->ex_data_bone_nodes.begin;
    for (ex_data_name_bone_index* i = itm_eq_obj->ex_bones.begin;
        i != itm_eq_obj->ex_bones.end; i++)
        i->name = 0;

    itm_eq_obj->ex_bones.end = itm_eq_obj->ex_bones.begin;
    itm_eq_obj->field_F0.end = itm_eq_obj->field_F0.begin;
    rob_chara_data_item_equip_object_clear_ex_data(itm_eq_obj);
    object_skin* skin = object_storage_get_object_skin(itm_eq_obj->object_info);
    if (!skin || !skin->ex_data_init)
        return;

    itm_eq_obj->skin = skin;
    itm_eq_obj->skin_ex_data = &skin->ex_data;
    rob_chara_data_item_equip_object_init_ex_data_bone_nodes(itm_eq_obj, &skin->ex_data);
    rob_chara_data_item_equip_object_load_ex_data(itm_eq_obj, &skin->ex_data, bone_data);
}

static void rob_chara_data_item_equip_object_reset(rob_chara_data_item_equip_object* itm_eq_obj) {
    rob_chara_data_item_equip_object_init_data(itm_eq_obj, 0xDEADBEEF);
}

static void rob_chara_data_item_equip_object_free(rob_chara_data_item_equip_object* itm_eq_obj) {
    rob_chara_data_item_equip_object_init_data(itm_eq_obj, 0xDDDDDDDD);

    if (itm_eq_obj->cloth_blocks.begin)
        free(itm_eq_obj->cloth_blocks.begin);
    itm_eq_obj->cloth_blocks = vector_ptr_empty(ex_cloth_block);

    if (itm_eq_obj->expression_blocks.begin)
        free(itm_eq_obj->expression_blocks.begin);
    itm_eq_obj->expression_blocks = vector_ptr_empty(ex_expression_block);

    if (itm_eq_obj->constraint_blocks.begin)
        free(itm_eq_obj->constraint_blocks.begin);
    itm_eq_obj->constraint_blocks = vector_ptr_empty(ex_constraint_block);

    if (itm_eq_obj->osage_blocks.begin)
        free(itm_eq_obj->osage_blocks.begin);
    itm_eq_obj->osage_blocks = vector_ptr_empty(ex_osage_block);

    if (itm_eq_obj->ex_bones.begin)
        free(itm_eq_obj->ex_bones.begin);
    itm_eq_obj->ex_bones = vector_empty(ex_data_name_bone_index);

    if (itm_eq_obj->field_108.begin)
        free(itm_eq_obj->field_108.begin);
    itm_eq_obj->field_108 = vector_empty(mat4);

    if (itm_eq_obj->field_F0.begin)
        free(itm_eq_obj->field_F0.begin);
    itm_eq_obj->field_F0 = vector_empty(mat4);

    if (itm_eq_obj->ex_data_bone_nodes.begin)
        free(itm_eq_obj->ex_data_bone_nodes.begin);
    itm_eq_obj->ex_data_bone_nodes = vector_empty(bone_node);

    if (itm_eq_obj->node_blocks.begin)
        free(itm_eq_obj->node_blocks.begin);
    itm_eq_obj->node_blocks = vector_ptr_empty(ex_node_block);

    if (itm_eq_obj->texture_pattern.begin)
        free(itm_eq_obj->texture_pattern.begin);
    itm_eq_obj->texture_pattern = vector_empty(texture_pattern_struct);

}

static void rob_chara_data_item_equip_reset(rob_chara_data_item_equip* rob_item_equip) {
    rob_item_equip->bone_nodes = 0;
    rob_item_equip->matrices = 0;
    for (int32_t i = 0; i < 31; i++)
        rob_chara_data_item_equip_object_reset(&rob_item_equip->item_equip_object[i]);
    rob_item_equip->item_equip_range = false;
    rob_item_equip->shadow_type = -1;
    rob_item_equip->field_A0 = 5;
    rob_item_equip->texture_pattern.end = rob_item_equip->texture_pattern.begin;
    rob_item_equip->field_D0 = object_info_null;
    rob_item_equip->field_D4 = -1;
    rob_item_equip->field_D8 = false;
    rob_item_equip->field_958 = false;
    rob_item_equip->field_959 = false;
    rob_item_equip->field_95A = false;
    rob_item_equip->osage_phys_step = 0.0f;
    rob_item_equip->field_93C = false;
    rob_item_equip->first_item_equip_object = 1;
    rob_item_equip->max_item_equip_object = 30;
}

static void rob_chara_data_item_equip_dispose(rob_chara_data_item_equip* rob_item_equip) {
    rob_chara_data_item_equip_reset(rob_item_equip);
    rob_chara_data_item_equip_object* itm_eq_obj = rob_item_equip->item_equip_object;
    for (int32_t i = 0; i < 31; i++)
        rob_chara_data_item_equip_object_free(&itm_eq_obj[i]);
    free(itm_eq_obj);

    if (rob_item_equip->texture_pattern.begin)
        free(rob_item_equip->texture_pattern.begin);
    rob_item_equip->texture_pattern = vector_empty(texture_pattern_struct);
    free(rob_item_equip);
}

static bool rob_chara_data_check_for_ageageagain_module(chara_index chara_index, int32_t module_index) {
    return chara_index == CHARA_MIKU && module_index == 148;
}

static void rob_chara_data_load_default_motion(rob_chara_data* rob_chr_data,
    bone_database* bone_data, motion_database* mot_db) {
    int32_t motion_id = 195; //rob_cmn_mottbl_get_motion_index(rob_chr_data, 0);
    rob_chara_data_load_default_motion_sub(rob_chr_data, 1, motion_id, bone_data, mot_db);
}

static void sub_14041C680(rob_chara_data_bone_data* rob_bone_data, bool a2) {
    struc_308* v2 = &rob_bone_data->motion_loaded.begin[0]->field_4F8;
    if (a2)
        v2->field_0 |= 2;
    else
        v2->field_0 &= ~2;
}

static void sub_14041C9D0(rob_chara_data_bone_data* rob_bone_data, bool a2) {
    struc_308* v2 = &rob_bone_data->motion_loaded.begin[0]->field_4F8;
    v2->field_8 = (uint8_t)(v2->field_0 & 1);
    if (a2)
        v2->field_0 |= 1;
    else
        v2->field_0 &= ~1;
}

static void sub_14041D2D0(rob_chara_data_bone_data* rob_bone_data, bool a2) {
    struc_308* v2 = &rob_bone_data->motion_loaded.begin[0]->field_4F8;
    if (a2)
        v2->field_0 |= 4;
    else
        v2->field_0 &= ~4;
}

static void sub_14041BC40(rob_chara_data_bone_data* rob_bone_data, bool a2) {
    struc_308* v2 = &rob_bone_data->motion_loaded.begin[0]->field_4F8;
    if (a2)
        v2->field_0 |= 8;
    else
        v2->field_0 &= ~8;
}

static void sub_140414F00(struc_308* a1, float_t a2) {
    a1->field_B8 = a1->eyes_adjust;
    a1->eyes_adjust = a2;
}

static void sub_14041D270(rob_chara_data_bone_data* rob_bone_data, float_t a2) {
    sub_140414F00(&rob_bone_data->motion_loaded.begin[0]->field_4F8, a2);
}

static void sub_14041D2A0(rob_chara_data_bone_data* rob_bone_data, float_t a2) {
    rob_bone_data->motion_loaded.begin[0]->field_4F8.field_B8 = a2;
}

/*static void sub_1403FBED0(motion_blend* hinge, float_t v47, float_t hinge, float_t init_rot) {
    if (v47 < 0.0f) {
        hinge->field_8 = false;
        hinge->field_C = 0.0f;
        hinge->field_10 = 0.0f;
        hinge->field_14 = hinge;
        hinge->field_18 = init_rot;
    }
    else {
        hinge->field_8 = true;
        hinge->field_C = v47;
        hinge->field_10 = 0.0f;
        hinge->field_14 = hinge;
        hinge->field_18 = init_rot;
    }
}*/

/*static void sub_140414C60(motion_blend_mot* hinge, float_t v47, float_t hinge, float_t init_rot) {
    motion_blend* v4 = hinge->field_5D0;
    if (v4)
        sub_1403FBED0(v4, v47, hinge, init_rot);
}

static void sub_14041BF80(rob_chara_data_bone_data* rob_bone_data, float_t v47, float_t hinge, float_t init_rot) {
    sub_140414C60(rob_bone_data->motion_loaded.begin[0], v47, hinge, init_rot);
}

static void sub_14041D310(rob_chara_data_bone_data* rob_bone_data) {
    motion_blend* v1 = rob_bone_data->motion_loaded.begin[0]->field_5D0;
    if (v1)
        v1->vftable->field_10(v1);
}

static void sub_140419820(rob_chara_data_bone_data* rob_bone_data, int32_t skeleton_select) {
    sub_140412E10(rob_bone_data->motion_loaded.begin[0], skeleton_select);
}*/

static void rob_chara_data_load_default_motion_sub(rob_chara_data* rob_chr_data, int32_t skeleton_select,
    int32_t motion_id, bone_database* bone_data, motion_database* mot_db) {
    /*sub_14041BE50(rob_chr_data->bone_data, -1);
    sub_14041BFC0(rob_chr_data->bone_data, -1);
    sub_14041C260(rob_chr_data->bone_data, -1);
    sub_14041D200(rob_chr_data->bone_data, -1);
    sub_14041BDB0(rob_chr_data->bone_data, -1);
    sub_14041BD20(rob_chr_data->bone_data, -1);*/
    rob_chara_data_bone_data_motion_load(rob_chr_data->bone_data, motion_id, 1, bone_data, mot_db);
    rob_chara_data_bone_data_set_frame(rob_chr_data->bone_data, 0.0f);
    sub_14041C680(rob_chr_data->bone_data, 0);
    sub_14041C9D0(rob_chr_data->bone_data, 0);
    sub_14041D2D0(rob_chr_data->bone_data, 0);
    sub_14041BC40(rob_chr_data->bone_data, 0);
    sub_14041D270(rob_chr_data->bone_data, 0.0f);
    sub_14041D2A0(rob_chr_data->bone_data, 0.0f);
    rob_chara_data_bone_data_set_rotation_y(rob_chr_data->bone_data, 0.0f);
    //sub_14041BF80(rob_chr_data->bone_data, 0.0f, 1.0f, 1.0f);
    //sub_14041D310(rob_chr_data->bone_data);
    rob_chara_data_bone_data_interpolate(rob_chr_data->bone_data);
    rob_chara_data_bone_data_update(rob_chr_data->bone_data, 0);
    //sub_140419820(rob_chr_data->bone_data, skeleton_select);
}

static void rob_chara_data_set_draw(rob_chara_data* rob_chr_data, item_id id, bool draw) {
    if (id < ITEM_BODY)
        return;
    else if (id < ITEM_ITEM16)
        rob_chr_data->item_equip->item_equip_object[id].draw = draw;
    else if (id == ITEM_MAX)
        for (int32_t i = ITEM_ATAMA; i <= ITEM_ITEM16; i++) {
            rob_chr_data->item_equip->item_equip_object[i].draw = draw;
            /*if (i == ITEM_ATAMA && rob_chara_data_check_for_ageageagain_module(
                rob_chr_data->chara_index, rob_chr_data->module_index)) {
                sub_140543780(rob_chr_data->chara_id, 1, draw);
                sub_140543780(rob_chr_data->chara_id, 2, draw);
                sub_1405430F0(rob_chr_data->chara_id, 1);
                sub_1405430F0(rob_chr_data->chara_id, 2);
            }*/
        }
}

static rob_osage_node* rob_osage_get_node(rob_osage* rob_osg,
    size_t index) {
    if ((ssize_t)index < rob_osg->nodes.end - rob_osg->nodes.begin)
        return &rob_osg->nodes.begin[index];
    else
        return &rob_osg->node;
}

static void rob_osage_node_init(rob_osage_node* node) {
    node->length = 0.0f;
    node->trans = vec3_null;
    node->field_10 = vec3_null;
    node->field_1C = vec3_null;
    node->field_28 = vec3_null;
    node->child_length = 0.0f;
    node->bone_node = 0;
    node->bone_node_mat = 0;
    node->sibling_node = 0;
    node->distance = 0.0f;
    node->field_94 = vec3_null;
    node->field_A0 = vec3_null;
    node->field_AC = vec3_null;
    node->field_B8 = vec3_null;
    node->field_C4 = 0.0f;
    node->field_C8 = 0.0f;
    node->field_CC = 1.0f;
    node->field_D0 = vec3_null;
    node->field_DC = 1.0f;
    rob_osage_node_data_init(&node->data);
    node->data_ptr = &node->data;
    node->field_198.end = node->field_198.begin;
    vector_struc_331_reserve(&node->field_198, 3);
    node->field_198.end += 3;
    node->mat = mat4u_null;
}

static void rob_osage_node_free(rob_osage_node* node) {
    vector_struc_331_free(&node->field_198, 0);
    vector_ptr_rob_osage_node* boc = &node->data.boc;
    for (rob_osage_node** i = boc->begin; i != boc->end; i++)
        *i = 0;
    vector_ptr_rob_osage_node_free(&node->data.boc, 0);
}

static void rob_osage_set_coli(rob_osage* rob_osg, mat4* mats) {
    if (rob_osg->skin_param_ptr->coli.begin != rob_osg->skin_param_ptr->coli.end)
        osage_coli_set(rob_osg->coli, rob_osg->skin_param_ptr->coli.begin, mats);
    osage_coli_ring_set(rob_osg->coli_ring, &rob_osg->ring.skp_root_coli, mats);
}

static void rob_osage_free(rob_osage* rob_osg) {
    rob_osage_init(rob_osg);
    //sub_14021BEF0(&rob_osg->field_1F60, rob_osg->field_1F60.head->left, rob_osg->field_1F60.head);
    //j_free_2(rob_osg->field_1F60.head);
    vector_skin_param_osage_root_coli_free(&rob_osg->ring.skp_root_coli, 0);
    vector_osage_coli_free(&rob_osg->ring.coli, 0);
    vector_skin_param_osage_root_coli_free(&rob_osg->skin_param.coli, 0);
    rob_osage_node_free(&rob_osg->node);
    vector_rob_osage_node_free(&rob_osg->nodes, rob_osage_node_free);
}

static void skin_param_init(skin_param* skp) {
    skp->coli.end = skp->coli.begin;
    skp->friction = 1.0f;
    skp->wind_afc = 0.0f;
    skp->air_res = 1.0f;
    skp->rot = vec3_null;
    skp->init_rot = vec3_null;
    skp->coli_type = 0;
    skp->stiffness = 0.0f;
    skp->move_cancel = -0.01f;
    skp->coli_r = 0.0;
    skp->hinge = (skin_param_hinge){ -90.0f, 90.0f, -90.0f, 90.0f };
    skin_param_hinge_limit(&skp->hinge);
    skp->force = 0.0f;
    skp->force_gain = 0.0f;
    skp->colli_tgt_osg = 0;
}

void rob_chara_data_array_init() {
    for (int32_t i = 0; i < 6; i++)
        rob_chara_data_init(&rob_chara_data_array[i]);
}

rob_chara_data* rob_chara_data_array_get(int32_t chara_id) {
    if (rob_chara_pv_data_array_check_chara_id(chara_id))
        return &rob_chara_data_array[chara_id];
    return 0;
}

void rob_chara_data_array_free() {
    for (int32_t i = 0; i < 6; i++)
        rob_chara_data_free(&rob_chara_data_array[i]);
}

void rob_chara_pv_data_init(rob_chara_pv_data* pv_data) {
    memset(pv_data, 0, sizeof(rob_chara_pv_data));
    pv_data->index = 2;
    pv_data->field_4 = 1;
    pv_data->field_5 = 0;
    pv_data->field_6 = 0;
    pv_data->field_14 = 0x00;
    pv_data->field_16 = 0xC9;
    pv_data->field_74 = -1;
    pv_data->field_78 = -1;
    pv_data->field_7C = -1;
    pv_data->field_80 = -1;
    pv_data->field_84 = -1;
    pv_data->field_88 = -1;
    pv_data->field_8C = -1;
    pv_data->field_90 = -1;
    pv_data->field_94 = -1;
    pv_data->field_98 = -1;
    pv_data->field_9C = 1;
    pv_data->eyes_adjust.base_adjust = EYES_BASE_ADJUST_DIRECTION;
    pv_data->eyes_adjust.neg = -1.0f;
    pv_data->eyes_adjust.pos = -1.0f;
}

inline bool rob_chara_pv_data_array_check_chara_id(int32_t chara_id) {
    //return rob_chara_pv_data_array[chara_id].index != -1;
    return rob_chara_data_array[chara_id].chara_id != -1;
}

inline void motion_storage_init() {
    motion_storage_data = vector_empty(motion_storage);
}

inline void motion_storage_append_mot_set(uint32_t set_id) {
    for (motion_storage* i = motion_storage_data.begin;
        i != motion_storage_data.end; i++)
        if (i->set_id == set_id) {
            i->count++;
            return;
        }
}

inline void motion_storage_insert_motion_set(mot_set* set, uint32_t set_id) {
    for (motion_storage* i = motion_storage_data.begin;
        i != motion_storage_data.end; i++)
        if (i->set_id == set_id) {
            mot_set_free(&i->set);
            i->count++;
            i->set = *set;
            return;
        }

    motion_storage* mot_set_storage = vector_motion_storage_reserve_back(&motion_storage_data);
    mot_set_storage->set_id = set_id;
    mot_set_storage->count = 1;
    mot_set_storage->set = *set;
}

inline mot_set* motion_storage_get_motion_set(uint32_t set_id) {
    for (motion_storage* i = motion_storage_data.begin;
        i != motion_storage_data.end; i++)
        if (i->set_id == set_id)
            return &i->set;
    return 0;
}

inline void motion_storage_delete_motion_set(uint32_t set_id) {
    for (motion_storage* i = motion_storage_data.begin;
        i != motion_storage_data.end; i++)
        if (i->set_id == set_id) {
            i->count--;
            if (i->count > 0)
                break;

            mot_set_free(&i->set);

            vector_motion_storage_erase(&motion_storage_data,
                i - motion_storage_data.begin, 0);
            break;
        }
}

inline mot_data* motion_storage_get_mot_data(uint32_t motion_id, motion_database* mot_db) {
    uint32_t set_id = -1;
    size_t motion_index = -1;
    vector_motion_set_info* motion_set = &mot_db->motion_set;
    for (motion_set_info* i = motion_set->begin; i != motion_set->end; i++) {
        vector_motion_info* motion = &i->motion;
        for (motion_info* j = motion->begin; j != motion->end; j++)
            if (j->id == motion_id) {
                set_id = i->id;
                motion_index = j - motion->begin;

                break;
            }

        if (set_id != -1)
            break;
    }

    if (set_id == -1)
        return 0;

    for (motion_storage* i = motion_storage_data.begin;
        i != motion_storage_data.end; i++)
        if (i->set_id == set_id)
            return &i->set.vec.begin[motion_index];
    return 0;
}

inline void motion_storage_free() {
    for (motion_storage* i = motion_storage_data.begin;
        i != motion_storage_data.end; i++) {
        mot_set_free(&i->set);
    }
    vector_motion_storage_free(&motion_storage_data, 0);
}
