/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "rob.h"
#include "data.h"
#include "draw_task.h"
#include "../KKdLib/key_val.h"
#include "../KKdLib/str_utils.h"

vector_func(bone_data)
vector_func(bone_node)
vector_ptr_func(ex_constraint_block)
vector_ptr_func(ex_cloth_block)
vector_ptr_func(ex_expression_block)
vector_ptr_func(ex_osage_block)
vector_ptr_func(ex_node_block)
vector_func(ex_data_name_bone_index)
vector_func(item_sub_data_item_change)
vector_func(item_sub_data_texture_change)
vector_func(item_sub_data_texture_change_tex)
vector_func(mot_key_set)
vector_ptr_func(motion_blend_mot)
vector_func(osage_coli)
vector_func(pair_int32_t_object_info)
vector_func(pair_name_ex_osage_block)
vector_func(pair_object_info_item_id)
vector_func(pair_uint32_t_ptr_rob_osage_node)
vector_func(rob_osage_node)
vector_ptr_func(rob_osage_node)
vector_func(skin_param_osage_node)
vector_func(skin_param_osage_root_boc)
vector_func(skin_param_osage_root_coli)
vector_func(skin_param_osage_root_normal_ref)
vector_func(struc_215)
vector_func(struc_294)
vector_func(struc_331)
vector_func(struc_373)

#define SKP_TEXT_BUF_SIZE 0x400

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
    rob_chara_bone_data* rob_bone_data, bone_database* bone_data);
static void bone_data_parent_load_bone_database(bone_data_parent* bone,
    vector_bone_database_bone* bones, vec3* common_translation, vec3* translation);
static void bone_data_parent_load_rob_chara(bone_data_parent* bone);
static void bone_data_parent_reset(bone_data_parent* bone);
static void bone_data_parent_free(bone_data_parent* bone);

static void bone_node_expression_data_mat_set(bone_node_expression_data* data,
    vec3* parent_scale, mat4* ex_data_mat, mat4* mat);
static void bone_node_expression_data_reset_position_rotation(bone_node_expression_data* data);
static void bone_node_expression_data_reset_scale(bone_node_expression_data* data);
static void bone_node_expression_data_set_position_rotation(bone_node_expression_data* data,
    float_t position_x, float_t position_y, float_t position_z,
    float_t rotation_x, float_t rotation_y, float_t rotation_z);
static void bone_node_expression_data_set_position_rotation_vec3(bone_node_expression_data* data,
    vec3* position, vec3* rotation);

static ex_cloth_block* ex_cloth_block_init();
static void ex_cloth_block_draw(ex_cloth_block* cls);
static void ex_cloth_block_field_8(ex_cloth_block* cls);
static void ex_cloth_block_field_18(ex_cloth_block* cls, int32_t a2, bool a3);
static void ex_cloth_block_field_28(ex_cloth_block* cls);
static void ex_cloth_block_field_40(ex_cloth_block* cls);
static void ex_cloth_block_field_48(ex_cloth_block* cls);
static void ex_cloth_block_field_50(ex_cloth_block* cls);
static void ex_cloth_block_reset(ex_cloth_block* cls);
static void ex_cloth_block_update(ex_cloth_block* cls);
static void ex_cloth_block_dispose(ex_cloth_block* cls);

static ex_constraint_block* ex_constraint_block_init();
static void ex_constraint_block_init_data(ex_constraint_block* cns, rob_chara_item_equip_object* itm_eq_obj,
    object_skin_block_constraint* cns_data, char* cns_data_name, bone_database* bone_data);
static void ex_constraint_block_data_set(ex_constraint_block* cns);
static void ex_constraint_block_draw(ex_constraint_block* cns);
static void ex_constraint_block_field_8(ex_constraint_block* cns);
static void ex_constraint_block_field_10(ex_constraint_block* cns);
static void ex_constraint_block_field_18(ex_constraint_block* cns, int32_t a2, bool a3);
static void ex_constraint_block_field_28(ex_constraint_block* cns);
static void ex_constraint_block_field_40(ex_constraint_block* cns);
static void ex_constraint_block_field_48(ex_constraint_block* cns);
static void ex_constraint_block_field_50(ex_constraint_block* cns);
static void ex_constraint_block_time_step(ex_constraint_block* cns);
static void ex_constraint_block_update(ex_constraint_block* cns);
static void ex_constraint_block_dispose(ex_constraint_block* cns);

static ex_expression_block* ex_expression_block_init();
static void ex_expression_block_init_data(ex_expression_block* exp,
    rob_chara_item_equip_object* itm_eq_obj, object_skin_block_expression* exp_data,
    char* exp_data_name, object_info a4, chara_index chara_index, bone_database* bone_data);
static void ex_expression_block_data_set(ex_expression_block* exp);
static void ex_expression_block_draw(ex_expression_block* exp);
static void ex_expression_block_field_8(ex_expression_block* exp);
static void ex_expression_block_field_10(ex_expression_block* exp);
static void ex_expression_block_field_18(ex_expression_block* exp, int32_t a2, bool a3);
static void ex_expression_block_field_28(ex_expression_block* exp);
static void ex_expression_block_field_40(ex_expression_block* exp);
static void ex_expression_block_field_48(ex_expression_block* exp);
static void ex_expression_block_field_50(ex_expression_block* exp);
static void ex_expression_block_time_step(ex_expression_block* exp);
static void ex_expression_block_update(ex_expression_block* exp);
static void ex_expression_block_dispose(ex_expression_block* exp);
static float_t ex_expression_block_stack_get_value(ex_expression_block_stack* stack);

static void ex_node_block_field_10(ex_node_block* node);
static void ex_node_block_field_58(ex_node_block* node);
static void ex_node_block_init_data(ex_node_block* node, bone_node* bone_node,
    ex_node_type type, char* name, rob_chara_item_equip_object* itm_eq_obj);
static void ex_node_block_reset(ex_node_block* node);
static void ex_node_block_free(ex_node_block* node);

static ex_osage_block* ex_osage_block_init();
static void ex_osage_block_draw(ex_osage_block* osg);
static void ex_osage_block_field_8(ex_osage_block* osg);
static void ex_osage_block_field_18(ex_osage_block* osg, int32_t a2, bool a3);
static void ex_osage_block_field_28(ex_osage_block* osg);
static void ex_osage_block_field_40(ex_osage_block* osg);
static void ex_osage_block_field_48(ex_osage_block* osg);
static void ex_osage_block_field_50(ex_osage_block* osg);
static void ex_osage_block_field_58(ex_osage_block* osg);
static void ex_osage_block_init_data(ex_osage_block* osg,
    rob_chara_item_equip_object* itm_eq_obj, object_skin_block_osage* osg_data,
    char* osg_data_name, object_skin_osage_node* osg_nodes, bone_node* bone_nodes,
    bone_node* ex_data_bone_nodes, object_skin* skin);
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

static const float_t get_osage_gravity_const();

static void item_sub_data_texture_change_free(item_sub_data_texture_change* tex_chg);

static void item_sub_data_texture_change_tex_free(item_sub_data_texture_change_tex* tex_chg_tex);

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
    rob_chara_bone_data* a2, bone_database* bone_data);
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

static rob_chara_bone_data* rob_chara_bone_data_init();
static void rob_chara_bone_data_calculate_bones(rob_chara_bone_data* rob_bone_data,
    vector_bone_database_bone* bones);
static void rob_chara_bone_data_eyes_xrot_adjust(rob_chara_bone_data* rob_bone_data,
    const struc_241* a2, eyes_adjust* a3);
static float_t rob_chara_bone_data_get_frame(rob_chara_bone_data* rob_bone_data);
static float_t rob_chara_bone_data_get_frame_count(rob_chara_bone_data* rob_bone_data);
static void rob_chara_bone_data_get_ik_scale(
    rob_chara_bone_data* rob_bone_data, bone_database* bone_data);
static mat4* rob_chara_bone_data_get_mat(rob_chara_bone_data* rob_bone_data, size_t index);
static bone_node* rob_chara_bone_data_get_node(rob_chara_bone_data* rob_bone_data, size_t index);
static void rob_chara_bone_data_ik_scale_calculate(
    rob_chara_bone_data_ik_scale* ik_scale, vector_bone_data* vec_bone_data,
    bone_database_skeleton_type base_skeleton_type,
    bone_database_skeleton_type skeleton_type, bone_database* bone_db);
static void rob_chara_bone_data_init_data(rob_chara_bone_data* rob_bone_data,
    bone_database_skeleton_type base_skeleton_type,
    bone_database_skeleton_type skeleton_type, bone_database* bone_data);
static void rob_chara_bone_data_init_skeleton(rob_chara_bone_data* rob_bone_data,
    bone_database_skeleton_type base_skeleton_type,
    bone_database_skeleton_type skeleton_type, bone_database* bone_data);
static void rob_chara_bone_data_interpolate(rob_chara_bone_data* rob_bone_data);
static void rob_chara_bone_data_motion_blend_mot_free(rob_chara_bone_data* rob_bone_data);
static void rob_chara_bone_data_motion_blend_mot_init(rob_chara_bone_data* rob_bone_data);
static void rob_chara_bone_data_motion_blend_mot_list_free(rob_chara_bone_data* rob_bone_data,
    size_t last_index);
static void rob_chara_bone_data_motion_blend_mot_list_init(rob_chara_bone_data* rob_bone_data);
static void rob_chara_bone_data_motion_load(rob_chara_bone_data* rob_bone_data,
    int32_t motion_id, int32_t index, bone_database* bone_data, motion_database* mot_db);
static void rob_chara_bone_data_reserve(rob_chara_bone_data* rob_bone_data);
static void rob_chara_bone_data_set_frame(rob_chara_bone_data* rob_bone_data, float_t frame);
static void rob_chara_bone_data_reset(rob_chara_bone_data* rob_bone_data);
static void rob_chara_bone_data_set_mats(rob_chara_bone_data* rob_bone_data,
    vector_bone_database_bone* bones);
static void rob_chara_bone_data_set_parent_mats(rob_chara_bone_data* rob_bone_data,
    uint16_t* parent_indices);
static void rob_chara_bone_data_set_rotation_y(rob_chara_bone_data* rob_bone_data, float_t rotation_y);
static void rob_chara_bone_data_update(rob_chara_bone_data* rob_bone_data, mat4* mat);
static void rob_chara_bone_data_dispose(rob_chara_bone_data* rob_bone_data);

static rob_chara_item_equip* rob_chara_item_equip_init();
static void rob_chara_item_equip_draw(
    rob_chara_item_equip* rob_item_equip, int32_t chara_id, render_context* rctx);
static void rob_chara_item_equip_get_parent_bone_nodes(
    rob_chara_item_equip* rob_item_equip, bone_node* bone_nodes, bone_database* bone_data);
static void rob_chara_item_equip_load_object_info(
    rob_chara_item_equip* rob_item_equip, object_info object_info,
    item_id id, bone_database* bone_data, void* data, object_database* obj_db);
static void rob_chara_item_equip_object_init(rob_chara_item_equip_object* itm_eq_obj);
static void rob_chara_item_equip_object_clear_ex_data(rob_chara_item_equip_object* itm_eq_obj);
static void rob_chara_item_equip_object_draw(
    rob_chara_item_equip_object* itm_eq_obj, mat4* mat, render_context* rctx);
static int32_t rob_chara_item_equip_object_get_bone_index(
    rob_chara_item_equip_object* itm_eq_obj, char* name, bone_database* bone_data);
static bone_node* rob_chara_item_equip_object_get_bone_node(
    rob_chara_item_equip_object* itm_eq_obj, int32_t bone_index);
static bone_node* rob_chara_item_equip_object_get_bone_node_by_name(
    rob_chara_item_equip_object* itm_eq_obj, char* name, bone_database* bone_data);
static void rob_chara_item_equip_object_get_parent_bone_nodes(
    rob_chara_item_equip_object* itm_eq_obj, bone_node* bone_nodes, bone_database* bone_data);
static void rob_chara_item_equip_object_init_data(rob_chara_item_equip_object* itm_eq_obj, size_t index);
static void rob_chara_item_equip_object_init_ex_data_bone_nodes(
    rob_chara_item_equip_object* itm_eq_obj, object_skin_ex_data* a2);
static void rob_chara_item_equip_object_load_ex_data(rob_chara_item_equip_object* itm_eq_obj,
    object_skin_ex_data* ex_data, bone_database* bone_data, void* data, object_database* obj_db);
static void rob_chara_item_equip_object_load_object_info_ex_data(
    rob_chara_item_equip_object* itm_eq_obj, object_info object_info,
    bone_node* bone_nodes, bone_database* bone_data, void* data, object_database* obj_db);
static void rob_chara_item_equip_object_reset(rob_chara_item_equip_object* itm_eq_obj);
static bool rob_chara_item_equip_object_set_boc(rob_chara_item_equip_object* itm_eq_obj,
    skin_param_osage_root* skp_root, ex_osage_block* osg);
static void rob_chara_item_equip_object_set_collision_target_osage(
    rob_chara_item_equip_object* itm_eq_obj, skin_param_osage_root* skp_root, skin_param* skp);
static void rob_chara_item_equip_object_skp_load(
    rob_chara_item_equip_object* itm_eq_obj, key_val* kv, bone_database* bone_data);
static void rob_chara_item_equip_object_skp_load_file(
    rob_chara_item_equip_object* itm_eq_obj, void* data, bone_database* bone_data, object_database* obj_db);
static void rob_chara_item_equip_object_free(rob_chara_item_equip_object* itm_eq_obj);
static void rob_chara_item_equip_reset(rob_chara_item_equip* rob_item_equip);
static void rob_chara_item_equip_reset_init_data(rob_chara_item_equip* rob_item_equip,
    bone_node* bone_nodes);
static void rob_chara_item_equip_set_item_equip_range(rob_chara_item_equip* rob_item_equip, bool value);
static void rob_chara_item_equip_set_shadow_type(rob_chara_item_equip* rob_item_equip, int32_t chara_id);
static void rob_chara_item_equip_dispose(rob_chara_item_equip* rob_item_equip);

static void rob_chara_item_sub_data_init(rob_chara_item_sub_data* rob_item_sub_data);
static bool rob_chara_item_sub_data_check_for_npr_flag(rob_chara_item_sub_data* item_sub_data);
static void rob_chara_item_sub_data_item_change_clear(
    rob_chara_item_sub_data* rob_item_sub_data);
static void rob_chara_item_sub_data_reload_items(rob_chara_item_sub_data* item_sub_data,
    int32_t chara_id, bone_database* bone_data, void* data, object_database* obj_db);
static void rob_chara_item_sub_data_texture_change_clear(
    rob_chara_item_sub_data* rob_item_sub_data);
static void rob_chara_item_sub_data_texture_pattern_clear(
    rob_chara_item_sub_data* rob_item_sub_data);
static void rob_chara_item_sub_data_free(
    rob_chara_item_sub_data* rob_item_sub_data);

static void rob_chara_data_init(rob_chara_data* rob_mot_blend);
static void rob_chara_data_free(rob_chara_data* rob_mot_blend);

static bool rob_chara_check_for_ageageagain_module(chara_index chara_index, int32_t module_index);
static void rob_chara_load_default_motion(rob_chara* rob_chr,
    bone_database* bone_data, motion_database* mot_db);
static void rob_chara_load_default_motion_sub(rob_chara* rob_chr, int32_t skeleton_select,
    int32_t motion_id, bone_database* bone_data, motion_database* mot_db);
static void rob_chara_set_draw(rob_chara* rob_chr, item_id id, bool draw);

static void rob_osage_init(rob_osage* rob_osg);
static void rob_osage_init_data(rob_osage* rob_osg,
    object_skin_block_osage* osg_data, object_skin_osage_node* osg_nodes,
    bone_node* ex_data_bone_nodes, object_skin* skin);
static rob_osage_node* rob_osage_get_node(rob_osage* rob_osg, size_t index);
static void rob_osage_load_skin_param(rob_osage* rob_osg, key_val* kv, char* name,
    skin_param_osage_root* skp_root, object_info* obj_info, bone_database* bone_data);
static void rob_osage_node_init(rob_osage_node* node);
static void rob_osage_node_free(rob_osage_node* node);
static void rob_osage_set_air_res(rob_osage* rob_osg, float_t air_res);
static void rob_osage_coli_set(rob_osage* rob_osg, mat4* mats);
static void rob_osage_set_coli_r(rob_osage* rob_osg, float_t coli_r);
static void rob_osage_set_force(rob_osage* rob_osg, float_t force, float_t force_gain);
static void rob_osage_set_hinge(rob_osage* rob_osg, float_t hinge_y, float_t hinge_z);
static void rob_osage_set_init_rot(rob_osage* rob_osg, float_t init_rot_y, float_t init_rot_z);
static void rob_osage_set_rot(rob_osage* rob_osg, float_t rotation_y, float_t rotation_z);
static void rob_osage_set_skin_param_osage_root(rob_osage* rob_osg, skin_param_osage_root* skp_root);
static void rob_osage_set_skp_osg_nodes(rob_osage* rob_osg,
    vector_skin_param_osage_node* skp_osg_nodes);
static void rob_osage_free(rob_osage* rob_osg);

static void rob_sub_action_init(rob_sub_action* sub_act);
static void rob_sub_action_init_sub(rob_sub_action* sub_act);
static void rob_sub_action_free(rob_sub_action* sub_act);

static void skin_param_init(skin_param* skp);

static bool skin_param_file_read(void* data, char* path, char* file, uint32_t hash);

static void skin_param_osage_node_parse(key_val* kv, char* name,
    vector_skin_param_osage_node* a3, skin_param_osage_root* skp_root);

static void skin_param_osage_root_init(skin_param_osage_root* root);
static void skin_param_osage_root_boc_free(skin_param_osage_root_boc* boc);
static void skin_param_osage_root_normal_ref_free(skin_param_osage_root_normal_ref* ref);
static void skin_param_osage_root_parse(key_val* kv, char* name,
    skin_param_osage_root* skp_root, bone_database* bone_data);
static void skin_param_osage_root_free(skin_param_osage_root* root);

vector_func(motion_storage)

vector_motion_storage motion_storage_data;
rob_chara rob_chara_array[ROB_CHARA_COUNT];
rob_chara_pv_data rob_chara_pv_data_array[ROB_CHARA_COUNT];
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

static const ex_node_block_vtbl ex_cloth_block_vftable = {
    (void*)ex_cloth_block_dispose,
    (void*)ex_cloth_block_field_8,
    (void*)ex_node_block_field_10,
    (void*)ex_cloth_block_field_18,
    (void*)ex_cloth_block_update,
    (void*)ex_cloth_block_field_28,
    (void*)ex_cloth_block_draw,
    (void*)ex_cloth_block_reset,
    (void*)ex_cloth_block_field_40,
    (void*)ex_cloth_block_field_48,
    (void*)ex_cloth_block_field_50,
    (void*)ex_node_block_field_58,
};

static const ex_node_block_vtbl ex_constraint_block_vftable = {
    (void*)ex_constraint_block_dispose,
    (void*)ex_constraint_block_field_8,
    (void*)ex_constraint_block_field_10,
    (void*)ex_constraint_block_field_18,
    (void*)ex_constraint_block_update,
    (void*)ex_constraint_block_field_28,
    (void*)ex_constraint_block_draw,
    (void*)ex_node_block_reset,
    (void*)ex_constraint_block_field_40,
    (void*)ex_constraint_block_field_48,
    (void*)ex_constraint_block_field_50,
    (void*)ex_node_block_field_58,
};

static const ex_node_block_vtbl ex_expression_block_vftable = {
    (void*)ex_expression_block_dispose,
    (void*)ex_expression_block_field_8,
    (void*)ex_expression_block_field_10,
    (void*)ex_expression_block_field_18,
    (void*)ex_expression_block_update,
    (void*)ex_expression_block_field_28,
    (void*)ex_expression_block_draw,
    (void*)ex_node_block_reset,
    (void*)ex_expression_block_field_40,
    (void*)ex_expression_block_field_48,
    (void*)ex_expression_block_field_50,
    (void*)ex_node_block_field_58,
};

static const ex_node_block_vtbl ex_osage_block_vftable = {
    (void*)ex_osage_block_dispose,
    (void*)ex_osage_block_field_8,
    (void*)ex_node_block_field_10,
    (void*)ex_osage_block_field_18,
    (void*)ex_osage_block_update,
    (void*)ex_osage_block_field_28,
    (void*)ex_osage_block_draw,
    (void*)ex_osage_block_reset,
    (void*)ex_osage_block_field_40,
    (void*)ex_osage_block_field_48,
    (void*)ex_osage_block_field_50,
    (void*)ex_osage_block_field_58,
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
        "rom/rob/", string_data(&file), mot_load_file);
    if (msf.ready && vector_length(msf.vec) > 0) {
        motion_storage_insert_motion_set(&msf.vec.begin[0], set);
        memset(&msf.vec.begin[0], 0, sizeof(mot_set));
    }
    mot_free(&msf);

    string_free(&file);
}

void motion_set_unload(uint32_t set) {
    motion_storage_delete_motion_set(set);
}

void rob_chara_init(rob_chara* rob_chr) {
    rob_chr->bone_data = rob_chara_bone_data_init();
    rob_chr->item_equip = rob_chara_item_equip_init();
    rob_chara_item_sub_data_init(&rob_chr->item_sub_data);
    rob_chara_data_init(&rob_chr->data);
    rob_chara_data_init(&rob_chr->data_prev);
    rob_chara_pv_data_init(&rob_chr->pv_data);
}

static void sub_1405EE878(ex_node_block* node) {
    
}

static void sub_1405F4BB0(rob_chara_item_equip_object* rob_item_equip, bool a2) {
    ex_node_block** begin = rob_item_equip->node_blocks.begin;
    ex_node_block** end = rob_item_equip->node_blocks.end;
    for (int32_t i = 0; i < 6; i++)
        for (ex_node_block** j = begin; j != end; j++)
            (*j)->vftable->field_18(*j, i, a2);
}

static void sub_1405F4820(rob_chara_item_equip_object* itm_eq_obj, int32_t a2) {
    if (itm_eq_obj->node_blocks.begin == itm_eq_obj->node_blocks.end)
        return;

    for (ex_node_block** i = itm_eq_obj->node_blocks.begin;
        i != itm_eq_obj->node_blocks.end; i++)
        (*i)->vftable->field_48(*i);

    for (int32_t v3 = a2; v3; v3--) {
        if (itm_eq_obj->field_1B8)
            sub_1405F4BB0(itm_eq_obj, true);

        for (ex_node_block** i = itm_eq_obj->node_blocks.begin;
            i != itm_eq_obj->node_blocks.end; i++)
            (*i)->vftable->field_50(*i);
    }
}

static void sub_1405F9640(rob_chara_item_equip_object* itm_eq_obj) {
    if (itm_eq_obj->osage_blocks.begin == itm_eq_obj->osage_blocks.end
        && itm_eq_obj->cloth_blocks.begin == itm_eq_obj->cloth_blocks.end)
        return;

    rob_chara_item_equip* rob_itm_equip = itm_eq_obj->item_equip;
    size_t index = 0;
    for (struc_373* i = rob_itm_equip->field_940.begin;
        i != rob_itm_equip->field_940.end; i++, index++) {
        /*size_t rob_chr = 0;
        int32_t child_osg_node = 0;
        sub_140489480(itm_eq_obj->obj_info, chr_data->motion_id, &rob_chr, &child_osg_node);
        if (!rob_chr)
            break;

        for (ex_osage_block** j = itm_eq_obj->osage_blocks.begin;
            j != itm_eq_obj->osage_blocks.end; j++)
            rob_chr = sub_1405F9A40(*j, index, rob_chr, child_osg_node);

        for (ex_cloth_block** k = itm_eq_obj->cloth_blocks.begin;
            k != itm_eq_obj->cloth_blocks.end; k++)
            rob_chr = sub_1405F9A30(*k, index, rob_chr, child_osg_node);*/
    }
}

static void rob_chara_item_equip_object_calc(rob_chara_item_equip_object* itm_eq_obj) {
    if (itm_eq_obj->field_B0 > 0) {
        sub_1405F4820(itm_eq_obj, itm_eq_obj->field_B0);
        itm_eq_obj->field_B0 = 0;
    }

    for (ex_node_block** i = itm_eq_obj->node_blocks.begin;
        i != itm_eq_obj->node_blocks.end; ++i)
        (*i)->vftable->field_10(*i);

    if (itm_eq_obj->field_1C8) {
        itm_eq_obj->field_1C8 = 0;
        sub_1405F9640(itm_eq_obj);
        for (ex_node_block** j = itm_eq_obj->node_blocks.begin;
            j != itm_eq_obj->node_blocks.end; ++j)
            (*j)->vftable->draw(*j);
    }
    else {
        if (itm_eq_obj->field_1B8)
            sub_1405F4BB0(itm_eq_obj, 0);

        for (ex_node_block** k = itm_eq_obj->node_blocks.begin; k != itm_eq_obj->node_blocks.end; ++k)
            (*k)->vftable->update(*k);
    }
    for (ex_node_block** l = itm_eq_obj->node_blocks.begin; l != itm_eq_obj->node_blocks.end; ++l)
        (*l)->vftable->field_58(*l);
}

static void rob_chara_item_equip_calc(rob_chara_item_equip* rob_item_equip) {
    if (!rob_item_equip->field_D8)
        for (int32_t i = rob_item_equip->first_item_equip_object; i < rob_item_equip->max_item_equip_object; i++)
            rob_chara_item_equip_object_calc(&rob_item_equip->item_equip_object[i]);
}

static float_t rob_chara_get_trans_scale(rob_chara* rob_chr, int32_t bone, vec3* trans) {
    if (bone > 26)
        return 0.0f;
    *trans = rob_chr->data.field_1E68.field_DF8[bone].trans;
    return rob_chr->data.field_1E68.field_DF8[bone].scale;
}

mat4* rob_chara_bone_data_get_mats_mat(rob_chara_bone_data* rob_bone_data, ssize_t index) {
    if (index < vector_length(rob_bone_data->mats))
        return &rob_bone_data->mats.begin[index];
    return 0;
}

static void sub_140509D30(rob_chara* rob_chr) {
    struc_195* v39 = rob_chr->data.field_1E68.field_DF8;
    struc_195* v40 = rob_chr->data.field_1E68.field_1230;
    struc_195* v41 = rob_chr->data.field_1E68.field_1668;
    mat4u* v42 = rob_chr->data.field_1E68.field_78;
    mat4u* v43 = rob_chr->data.field_1E68.field_738;

    const struc_218* v3 = rob_chr->chara_init_data->field_828;
    const struc_218* v6 = rob_chr->chara_init_data->field_830;
    mat4 mat;
    for (int32_t i = 0; i < 27; i++)  {
        mat = *rob_chara_bone_data_get_mats_mat(rob_chr->bone_data, v3->bone_index);
        mat4_translate_mult(&mat, v3->bone_offset.x, v3->bone_offset.y, v3->bone_offset.z, &mat);
        mat4_to_mat4u(&mat, v42);

        mat = *rob_chara_bone_data_get_mats_mat(rob_chr->bone_data, v6->bone_index);
        mat4_translate_mult(&mat, v3->bone_offset.x, v3->bone_offset.y, v3->bone_offset.z, &mat);
        mat4_to_mat4u(&mat, v43);
        float_t v14 = rob_chr->data.adjust_data.scale;

        vec3 v23;
        vec3_mult_scalar(*(vec3*)&v42->row3, v14, v23);
        vec3_add(v23, rob_chr->data.adjust_data.trans, v23);
        *(vec3*)&v42->row3 = v23;

        vec3 v29;
        vec3_mult_scalar(*(vec3*)&v43->row3, v14, v23);
        vec3_add(v23, rob_chr->data.adjust_data.trans, v29);
        *(vec3*)&v43->row3 = v29;

        float_t v20 = v3->scale * v14;
        v39->scale = v20;
        v39->field_24 = v20;
        v39->prev_trans = v39->trans;
        v39->trans = v23;

        float_t v19 = v6->scale * v14;
        v40->scale = v19;
        v40->field_24 = v19;
        v40->prev_trans = v40->trans;
        v40->trans = v29;

        v41->scale = v19;
        v41->field_24 = v19;
        v41->prev_trans = v41->trans;
        v41->trans = v29;

        v39++;
        v40++;
        v41++;
        v42++;
        v43++;
        v3++;
        v6++;
    }
}

void rob_chara_calc(rob_chara* rob_chr) {
    if (rob_chara_bone_data_get_frame(rob_chr->bone_data) < 0.0)
        rob_chara_bone_data_set_frame(rob_chr->bone_data, 0.0);

    float_t frame = rob_chara_bone_data_get_frame(rob_chr->bone_data);
    float_t frame_count = rob_chara_bone_data_get_frame_count(rob_chr->bone_data);
    if (frame > frame_count)
        rob_chara_bone_data_set_frame(rob_chr->bone_data, frame_count);

    rob_chara_get_trans_scale(rob_chr, 0, &rob_chr->item_equip->position);

    rob_chara_item_equip_calc(rob_chr->item_equip);

    /*if (rob_chara_check_for_ageageagain_module(rob_chr->chara_index, rob_chr->module_index)) {
        sub_140543520(rob_chr->chara_id, 1, rob_chr->item_equip->osage_phys_step);
        sub_14054FC90(rob_chr, 1u, "j_tail_l_006_wj");
        sub_140543520(rob_chr->chara_id, 2, rob_chr->item_equip->osage_phys_step);
        sub_14054FC90(rob_chr, 2u, "j_tail_r_006_wj");
    }*/
}

void rob_chara_draw(rob_chara* rob_chr, render_context* rctx) {
    rob_chr->item_equip->texture_color_coeff = vec4u_identity;
    rob_chr->item_equip->mat = mat4u_identity;
    rob_chara_item_equip_draw(rob_chr->item_equip, rob_chr->chara_id, rctx);
}

mat4* rob_chara_get_bone_data_mat(rob_chara* chara, size_t index) {
    return rob_chara_bone_data_get_mat(chara->bone_data, index);
}

float_t rob_chara_get_frame(rob_chara* rob_chr) {
    return rob_chara_bone_data_get_frame(rob_chr->bone_data);
}

float_t rob_chara_get_frame_count(rob_chara* rob_chr) {
    return rob_chara_bone_data_get_frame_count(rob_chr->bone_data);
}

void rob_chara_load_motion(rob_chara* rob_chr, int32_t motion_id,
    int32_t index, bone_database* bone_data, motion_database* mot_db) {
    rob_chara_bone_data_motion_load(rob_chr->bone_data, motion_id, index, bone_data, mot_db);
}

void rob_chara_reload_items(rob_chara* rob_chr, item_sub_data* sub_data,
    bone_database* bone_data, void* data, object_database* obj_db) {
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

    rob_chara_item_equip* rob_item_equip = rob_chr->item_equip;
    object_info base_face = chara_init_data_array[rob_chr->chara_index].base_face;
    rob_chara_item_equip_load_object_info(rob_item_equip,
        base_face, ITEM_ATAMA, bone_data, data, obj_db);

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
        rob_chara_item_equip_load_object_info(rob_item_equip,
            obj_info, id, bone_data, data, obj_db);
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
            rob_chara_item_equip_load_object_info(rob_item_equip,
                obj_info, ITEM_ITEM09, bone_data, data, obj_db);
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
        rob_chara_item_equip_load_object_info(rob_item_equip,
            obj_info, id, bone_data, data, obj_db);
    }
}

void rob_chara_reset(rob_chara* rob_chr, bone_database* bone_data, void* data, object_database* obj_db) {
    bone_node* v3 = rob_chara_bone_data_get_node(rob_chr->bone_data, 0);
    rob_chara_item_equip_reset_init_data(rob_chr->item_equip, v3);
    rob_chara_item_equip_set_item_equip_range(rob_chr->item_equip, rob_chr->module_index == 501);
    rob_chara_item_equip_load_object_info(rob_chr->item_equip,
        object_info_null, ITEM_BODY, bone_data, data, obj_db);
    rob_chara_item_equip_load_object_info(rob_chr->item_equip,
        rob_chr->chara_init_data->base_face, ITEM_ATAMA, bone_data, data, obj_db);
    rob_chara_item_equip_set_shadow_type(rob_chr->item_equip, rob_chr->chara_id);
    rob_chr->item_equip->field_A0 = 5;
    rob_chara_item_sub_data_reload_items(&rob_chr->item_sub_data,
        rob_chr->chara_id, bone_data, data, obj_db);
    if (rob_chara_item_sub_data_check_for_npr_flag(&rob_chr->item_sub_data))
        rob_chr->item_equip->npr_flag = true;

    /*if (rob_chara_check_for_ageageagain_module(rob_chrchara_index, rob_chr->module_index)) {
        sub_140542E30(rob_chr->chara_id, 1);
        sub_140542E30(rob_chr->chara_id, 2);
    }*/
}

static float_t chara_size_table_get_value(uint32_t a1) {
    static const float_t chara_size_table[] = {
        1.07f, 1.0f, 0.96f, 0.987f, 1.025f
    };

    if (a1 > 4)
        return 1.0f;
    else
        return chara_size_table[a1];
}

static float_t chara_pos_adjust_y_table_get_value(uint32_t a1) {
    static const float_t chara_pos_adjust_y_table[] = {
        0.071732f, 0.0f, -0.03859f, 0.0f, 0.0f
    };

    if (a1 > 4)
        return 0.0f;
    else
        return chara_pos_adjust_y_table[a1];
}

void rob_chara_reset_data(rob_chara* rob_chr, rob_chara_pv_data* pv_data,
    bone_database* bone_data, motion_database* mot_db) {
    rob_chara_bone_data_reset(rob_chr->bone_data);
    rob_chara_bone_data_init_data(rob_chr->bone_data,
        BONE_DATABASE_SKELETON_COMMON, rob_chr->chara_init_data->skeleton_type, bone_data);
    rob_chara_item_equip_get_parent_bone_nodes(rob_chr->item_equip,
        rob_chara_bone_data_get_node(rob_chr->bone_data, 0), bone_data);
    rob_chara_data* chr_data = &rob_chr->data;
    chr_data->field_1D38.field_0 = pv_data->field_14;
    chr_data->field_1D38.field_6 = pv_data->field_14;
    chr_data->adjust_data.scale = chara_size_table_get_value(rob_chr->pv_data.chara_size_index);
    chr_data->adjust_data.height_adjust = rob_chr->pv_data.height_adjust;
    chr_data->adjust_data.pos_adjust_y = chara_pos_adjust_y_table_get_value(rob_chr->pv_data.chara_size_index);
    rob_chara_bone_data_eyes_xrot_adjust(rob_chr->bone_data, chara_some_data_array, &pv_data->eyes_adjust);
    rob_chara_bone_data_get_ik_scale(rob_chr->bone_data, bone_data);
    rob_chara_load_default_motion(rob_chr, bone_data, mot_db);
    int32_t motion_id = 195; //rob_cmn_mottbl_get_motion_index(rob_chr, 0);
    rob_chara_load_motion(rob_chr, motion_id, 0, bone_data, mot_db);
    rob_chara_set_draw(rob_chr, ITEM_MAX, true);
}

void rob_chara_set_frame(rob_chara* rob_chr, float_t frame) {
    rob_chara_bone_data_set_frame(rob_chr->bone_data, frame);
    rob_chara_bone_data_interpolate(rob_chr->bone_data);
    rob_chara_bone_data_update(rob_chr->bone_data, 0);
    sub_140509D30(rob_chr);
}

void rob_chara_set_pv_data(rob_chara* rob_chr, int8_t chara_id,
    chara_index chara_index, uint32_t module_index, rob_chara_pv_data* pv_data) {
    rob_chr->chara_id = chara_id;
    rob_chr->chara_index = chara_index;
    rob_chr->module_index = module_index;
    rob_chr->pv_data = *pv_data;
    rob_chr->chara_init_data = chara_init_data_get(chara_index);
}

void rob_chara_set_visibility(rob_chara* rob_chr, bool value) {
    rob_chr->data.field_0 &= ~0x01;
    rob_chr->data.field_3 &= ~0x01;
    rob_chr->data.field_0 |= value & 0x01;
    rob_chr->data.field_3 |= value & 0x01;
    /*if (rob_chara_check_for_ageageagain_module(rob_chr->chara_index, rob_chr->module_index)) {
        sub_140543780(rob_chr->chara_id, 1, value);
        sub_140543780(rob_chr->chara_id, 2, value);
    }*/
}

void rob_chara_free(rob_chara* rob_chr) {
    if (rob_chr->bone_data)
        rob_chara_bone_data_dispose(rob_chr->bone_data);

    if (rob_chr->item_equip)
        rob_chara_item_equip_dispose(rob_chr->item_equip);

    rob_chara_item_sub_data_free(&rob_chr->item_sub_data);
    rob_chara_data_free(&rob_chr->data);
    rob_chara_data_free(&rob_chr->data_prev);
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
        mat4_mult_vec3_inv_trans(&mat, &a1->trans, &a1->trans);
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
        mat4_mult_vec3_inv_trans(a1->node[0].mat, &v30, &v30);
        vec3_negate(v30, v30);
        bone_data_mult_1_ik_legs(a1, &v30);
        break;
    case MOTION_BONE_N_MOMO_A_R_WJ_CD_EX:
        v5 = a2[MOTION_BONE_CL_MOMO_R].node;
        mat4_get_translation(v5[2].mat, &v30);
        mat4_mult_vec3_inv_trans(a1->node[0].mat, &v30, &v30);
        vec3_negate(v30, v30);
        bone_data_mult_1_ik_legs(a1, &v30);
        break;
    case MOTION_BONE_N_HARA_CD_EX:
        v5 = a2[MOTION_BONE_KL_MUNE_B_WJ].node;
        mat4_get_translation(v5[0].mat, &v30);
        mat4_mult_vec3_inv_trans(a1->node[0].mat, &v30, &v30);
        bone_data_mult_1_ik_legs(a1, &v30);
        break;
    default:
        return false;
    }
    return true;
}

static void bone_data_mult_1_ik_hands(bone_data* a1, vec3* a2) {
    vec3 v15;
    mat4_mult_vec3_inv_trans(a1->node[0].mat, a2, &v15);

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

    mat4_mult_vec3_inv_trans(a1->node[0].mat, a2, &v8);
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

static bool bone_data_mult_1_exp_data(bone_data* a1, bone_node_expression_data* exp_data, bone_data* a3) {
    float_t v10;
    bone_node* v11;
    vec3 v15;
    mat4 dst;
    mat4 mat;

    bool ret = true;
    switch (a1->motion_bone_index) {
    case MOTION_BONE_N_EYE_L_WJ_EX:
        exp_data->rotation.x = -a3[MOTION_BONE_KL_EYE_L].node[0].exp_data.rotation.x;
        exp_data->rotation.y = a3[MOTION_BONE_KL_EYE_L].node[0].exp_data.rotation.y * (float_t)(-1.0 / 2.0);
        break;
    case MOTION_BONE_N_EYE_R_WJ_EX:
        exp_data->rotation.x = -a3[MOTION_BONE_KL_EYE_R].node[0].exp_data.rotation.x;
        exp_data->rotation.y = a3[MOTION_BONE_KL_EYE_R].node[0].exp_data.rotation.y * (float_t)(-1.0 / 2.0);
        break;
    case MOTION_BONE_N_KUBI_WJ_EX:
        mat4u_to_mat4(&a3[MOTION_BONE_CL_KAO].rot_mat[0], &dst);
        mat4u_to_mat4(&a3[MOTION_BONE_CL_KAO].rot_mat[1], &mat);
        mat4_mult(&mat, &dst, &dst);
        mat4_get_rotation(&dst, &v15);
        v10 = v15.z;
        if (v10 < (float_t)-M_PI_2)
            v10 += (float_t)(M_PI * 2.0);
        exp_data->rotation.y = (v10 - (float_t)M_PI_2) * 0.2f;
        break;
    case MOTION_BONE_N_HITO_L_EX:
    case MOTION_BONE_N_HITO_R_EX:
        exp_data->rotation.z = (float_t)(-9.0 * DEG_TO_RAD);
        break;
    case MOTION_BONE_N_KO_L_EX:
        exp_data->rotation.x = (float_t)(-8.0 * DEG_TO_RAD);
        exp_data->rotation.z = (float_t)(16.0 * DEG_TO_RAD);
        break;
    case MOTION_BONE_N_KUSU_L_EX:
    case MOTION_BONE_N_KUSU_R_EX:
        exp_data->rotation.z = (float_t)(9.0 * DEG_TO_RAD);
        break;
    case MOTION_BONE_N_NAKA_L_EX:
    case MOTION_BONE_N_NAKA_R_EX:
        exp_data->rotation.z = (float_t)(-1.0 * DEG_TO_RAD);
        break;
    case MOTION_BONE_N_OYA_L_EX:
        exp_data->rotation.x = (float_t)(75.0 * DEG_TO_RAD);
        exp_data->rotation.y = (float_t)(12.0 * DEG_TO_RAD);
        exp_data->rotation.z = (float_t)(-24.0 * DEG_TO_RAD);
        break;
    case MOTION_BONE_N_STE_L_WJ_EX:
        exp_data->rotation.x = a3[MOTION_BONE_KL_TE_L_WJ].node[0].exp_data.rotation.x;
        break;
    case MOTION_BONE_N_SUDE_L_WJ_EX:
    case MOTION_BONE_N_SUDE_B_L_WJ_EX:
        v11 = &a3[MOTION_BONE_KL_TE_L_WJ].node[0];
        exp_data->rotation.x = bone_data_limit_angle(v11->exp_data.rotation.x) * (float_t)(1.0 / 3.0);
        break;
    case MOTION_BONE_N_SUDE_R_WJ_EX:
    case MOTION_BONE_N_SUDE_B_R_WJ_EX:
        v11 = &a3[MOTION_BONE_KL_TE_R_WJ].node[0];
        exp_data->rotation.x = bone_data_limit_angle(v11->exp_data.rotation.x) * (float_t)(1.0 / 3.0);
        break;
    case MOTION_BONE_N_HIJI_L_WJ_EX:
        exp_data->rotation.z = a3[MOTION_BONE_C_KATA_L].node[2].exp_data.rotation.z * (float_t)(1.0 / 2.0);
        break;
    case MOTION_BONE_N_UP_KATA_L_EX:
    case MOTION_BONE_N_UP_KATA_R_EX:
        break;
    case MOTION_BONE_N_KO_R_EX:
        exp_data->rotation.x = (float_t)(8.0 * DEG_TO_RAD);
        exp_data->rotation.z = (float_t)(16.0 * DEG_TO_RAD);
        break;
    case MOTION_BONE_N_OYA_R_EX:
        exp_data->rotation.x = (float_t)(-75.0 * DEG_TO_RAD);
        exp_data->rotation.y = (float_t)(-12.0 * DEG_TO_RAD);
        exp_data->rotation.z = (float_t)(-24.0 * DEG_TO_RAD);
        break;
    case MOTION_BONE_N_STE_R_WJ_EX:
        exp_data->rotation.x = a3[MOTION_BONE_KL_TE_R_WJ].node[0].exp_data.rotation.x;
        break;
    case MOTION_BONE_N_HIJI_R_WJ_EX:
        exp_data->rotation.z = a3[MOTION_BONE_C_KATA_R].node[2].exp_data.rotation.z * (float_t)(1.0 / 2.0);
        break;
    case MOTION_BONE_N_HIZA_L_WJ_EX:
        exp_data->rotation.z = a3[MOTION_BONE_CL_MOMO_L].node[2].exp_data.rotation.z * (float_t)(1.0 / 2.0);
        break;
    case MOTION_BONE_N_HIZA_R_WJ_EX:
        exp_data->rotation.z = a3[MOTION_BONE_CL_MOMO_R].node[2].exp_data.rotation.z * (float_t)(1.0 / 2.0);
        break;
    case MOTION_BONE_N_MOMO_B_L_WJ_EX:
    case MOTION_BONE_N_MOMO_C_L_WJ_EX:
        v11 = &a3[MOTION_BONE_CL_MOMO_L].node[0];
        exp_data->rotation.y = bone_data_limit_angle(v11->exp_data.rotation.y
            + v11->exp_data.rotation.x) * (float_t)(1.0 / 3.0);
        break;
    case MOTION_BONE_N_MOMO_B_R_WJ_EX:
    case MOTION_BONE_N_MOMO_C_R_WJ_EX:
        v11 = &a3[MOTION_BONE_CL_MOMO_R].node[0];
        exp_data->rotation.y = bone_data_limit_angle(v11->exp_data.rotation.y
            + v11->exp_data.rotation.x) * (float_t)(1.0 / 3.0);
        break;
    case MOTION_BONE_N_HARA_B_WJ_EX:
        exp_data->rotation.y = a3[MOTION_BONE_CL_MUNE].node[0].exp_data.rotation.y * (float_t)(1.0 / 3.0)
            + a3[MOTION_BONE_KL_KOSI_Y].node[0].exp_data.rotation.y * (float_t)(2.0 / 3.0);
        break;
    case MOTION_BONE_N_HARA_C_WJ_EX:
        exp_data->rotation.y = a3[MOTION_BONE_CL_MUNE].node[0].exp_data.rotation.y * (float_t)(2.0 / 3.0)
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
    mat4_mult_vec3_inv_trans(&mat, &a1->ik_target, &v30);
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
        mat4_mult_vec3_inv_trans(&mat, &pole_target, &pole_target);
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
    if (v8 > 0.000001f) {
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
    rob_chara_bone_data* rob_bone_data, bone_database* bone_data) {
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
    bone_data_parent_load_rob_chara(bone);
    bone_data_parent_load_bone_database(bone, common_bones, common_translation->begin, translation->begin);
}

static void bone_data_parent_load_bone_database(bone_data_parent* bone,
    vector_bone_database_bone* bones, vec3* common_translation, vec3* translation) {
    rob_chara_bone_data* rob_bone_data = bone->rob_bone_data;
    size_t chain_pos = 0;
    size_t total_bone_count = 0;
    size_t ik_bone_count = 0;

    bone_data* bone_node = bone->bones.begin;
    for (bone_database_bone* i = bones->begin; i != bones->end; i++, bone_node++) {
        bone_node->motion_bone_index = (motion_bone_index)(i - bones->begin);
        bone_node->type = i->type;
        bone_node->mirror = i->mirror;
        bone_node->parent = i->parent;
        bone_node->flags = i->flags;
        if (i->type >= BONE_DATABASE_BONE_POSITION_ROTATION)
            bone_node->key_set_count = 6;
        else
            bone_node->key_set_count = 3;
        bone_node->base_translation[0] = *common_translation++;
        bone_node->base_translation[1] = *translation++;
        bone_node->node = &rob_bone_data->nodes.begin[total_bone_count];
        bone_node->has_parent = i->has_parent;
        if (i->has_parent)
            bone_node->parent_mat = &rob_bone_data->mats.begin[i->parent];
        else
            bone_node->parent_mat = 0;

        bone_node->pole_target_mat = 0;
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
            bone_node->ik_segment_length[0] = (common_translation++)->x;
            bone_node->ik_segment_length[1] = (translation++)->x;

            chain_pos += 2;
            ik_bone_count++;
            total_bone_count += 3;
            break;
        case BONE_DATABASE_BONE_ARM_IK_ROTATION:
        case BONE_DATABASE_BONE_LEGS_IK_ROTATION:
            bone_node->ik_segment_length[0] = (common_translation++)->x;
            bone_node->ik_segment_length[1] = (translation++)->x;
            bone_node->ik_2nd_segment_length[0] = (common_translation++)->x;
            bone_node->ik_2nd_segment_length[1] = (translation++)->x;

            mat4* pole_target = 0;
            if (i->pole_target)
                pole_target = &rob_bone_data->mats.begin[i->pole_target];
            bone_node->pole_target_mat = pole_target;

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

        bone_data* bone_node = &a1->bones.begin[bone_index];
        bone_node->key_set_offset = (int32_t)key_set_offset;
        bone_node->frame = -1.0f;
        if (bone_node->type >= BONE_DATABASE_BONE_POSITION_ROTATION)
            key_set_offset+= 6;
        else
            key_set_offset+= 3;
    }
}

static void bone_data_parent_load_rob_chara(bone_data_parent* bone) {
    rob_chara_bone_data* rob_bone_data = bone->rob_bone_data;
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
    vec3* parent_scale, mat4* ex_data_mat, mat4* mat) {
    vec3 position;
    vec3_mult(data->position, *parent_scale, position);
    mat4_translate_mult( ex_data_mat, position.x, position.y, position.z, ex_data_mat);
    vec3 rotation = data->rotation;
    mat4_rotate_mult(ex_data_mat, rotation.x, rotation.y, rotation.z, ex_data_mat);
    vec3_mult(data->scale, *parent_scale, data->parent_scale);
    *mat = *ex_data_mat;
    mat4_scale_rot(mat, data->parent_scale.x, data->parent_scale.y, data->parent_scale.z, mat);
}

static void bone_node_expression_data_reset_position_rotation(bone_node_expression_data* data) {
    data->position = vec3_null;
    data->rotation = vec3_null;
    data->scale = vec3_identity;
    data->parent_scale = vec3_identity;
}

static void bone_node_expression_data_reset_scale(bone_node_expression_data* data) {
    data->scale = vec3_identity;
    data->parent_scale = vec3_identity;
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
    data->parent_scale = vec3_identity;
}

static void bone_node_expression_data_set_position_rotation_vec3(bone_node_expression_data* data,
    vec3* position, vec3* rotation) {
    data->position = *position;
    data->rotation = *rotation;
    data->scale = vec3_identity;
    data->parent_scale = vec3_identity;
}

static ex_cloth_block* ex_cloth_block_init() {
    ex_cloth_block* cls = force_malloc(sizeof(ex_cloth_block));
    cls->base.vftable = &ex_cloth_block_vftable;
    cls->base.parent_name = string_empty;
    return cls;
}

static void ex_cloth_block_draw(ex_cloth_block* cls) {

}

static void ex_cloth_block_field_8(ex_cloth_block* cls) {
    //cls->rob.field_48(&cls->rob);
    cls->cls_data = 0;
    cls->field_2428 = 0;
    cls->index = 0;
}

static void ex_cloth_block_field_18(ex_cloth_block* cls, int32_t a2, bool a3) {

}

static void sub_1405F98A0(ex_cloth_block* cls) {
    //sub_14021FD00(&cls->rob, cls->field_2428);
    //cls->rob.__vftable->set_wind_direction(&cls->rob, wind_task_struct_get_wind_direction());
}

static void ex_cloth_block_field_28(ex_cloth_block* a1) {
    //sub_140218E40(&cls->rob, &cls->base.item_equip_object->item_equip->field_940);
}

static void ex_cloth_block_field_40(ex_cloth_block* cls) {

}

static void ex_cloth_block_field_48(ex_cloth_block* cls) {
    sub_1405F98A0(cls);
    //sub_14021D480(&cls->rob);
}

static void ex_cloth_block_field_50(ex_cloth_block* cls) {
    sub_1405F98A0(cls);
    //sub_14021D840(&cls->rob);
}

static void ex_cloth_block_reset(ex_cloth_block* cls) {

}

static void ex_cloth_block_update(ex_cloth_block* cls) {
    sub_1405F98A0(cls);
    rob_chara_item_equip* rob_item_equip = cls->base.item_equip_object->item_equip;
    float_t step = get_delta_frame() * rob_item_equip->step;
    if (rob_item_equip->field_940.begin != rob_item_equip->field_940.end
        && rob_item_equip->field_940.begin->field_C)
        step = 1.0f;
    //sub_140218560(&cls->rob, step, 0);
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

static void ex_constraint_block_init_data(ex_constraint_block* cns, rob_chara_item_equip_object* itm_eq_obj,
    object_skin_block_constraint* cns_data, char* cns_data_name, bone_database* bone_data) {
    bone_node* node = rob_chara_item_equip_object_get_bone_node_by_name(
        itm_eq_obj, cns_data_name, bone_data);
    ex_node_block_init_data(&cns->base, node, EX_CONSTRAINT, node->name, itm_eq_obj);
    cns->cns_data = cns_data;

    cns->source_node_bone_node = rob_chara_item_equip_object_get_bone_node_by_name(
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
        cns->direction_up_vector_bone_node = rob_chara_item_equip_object_get_bone_node_by_name(
            itm_eq_obj, up_vector_name, bone_data);
}

static void ex_constraint_block_data_set(ex_constraint_block* cns) {
    if (!cns->base.bone_node)
        return;

    bone_node* node = cns->base.bone_node;
    bone_node_expression_data* exp_data = &node->exp_data;
    bone_node* parent_node = cns->base.parent_bone_node;
    vec3 parent_scale = parent_node->exp_data.parent_scale;

    mat4 mat;
    mat4_inverse_normalized(parent_node->ex_data_mat, &mat);
    mat4_mult(node->mat, &mat, &mat);
    mat4_get_rotation(&mat, &node->exp_data.rotation);
    mat4_get_translation(&mat, &exp_data->position);
    if (fabsf(parent_scale.x) > 0.000001f)
        exp_data->position.x /= parent_scale.x;
    if (fabsf(parent_scale.y) > 0.000001f)
        exp_data->position.y /= parent_scale.z;
    if (fabsf(parent_scale.z) > 0.000001f)
        exp_data->position.z /= parent_scale.z;
    *node->ex_data_mat = *node->mat;
    mat4_scale_rot(node->mat, parent_scale.x, parent_scale.y, parent_scale.z, node->mat);
    vec3_mult(exp_data->scale, parent_scale, exp_data->parent_scale);
}

static void ex_constraint_block_draw(ex_constraint_block* cns) {

}

static void ex_constraint_block_field_8(ex_constraint_block* cns) {
    cns->type = OBJECT_SKIN_BLOCK_CONSTRAINT_NONE;
    cns->source_node_bone_node = 0;
    cns->direction_up_vector_bone_node = 0;
    cns->cns_data = 0;
    cns->field_80 = 0;
}

static void ex_constraint_block_field_10(ex_constraint_block* cns) {
    if (cns->base.bone_node) {
        bone_node_expression_data* exp_data = &cns->base.bone_node->exp_data;
        object_skin_block_constraint* cns_data = cns->cns_data;
        exp_data->position = cns_data->base.position;
        exp_data->rotation = cns_data->base.rotation;
        exp_data->scale = cns_data->base.scale;
    }
    cns->base.field_59 = false;
}

static void ex_constraint_block_field_18(ex_constraint_block* cns, int32_t a2, bool a3) {
    if (cns->base.field_59)
        return;

    if (a2 == 0) {
        if (cns->base.field_58)
            cns->base.vftable->update(&cns->base);
    }
    else if (a2 == 2) {
        if (cns->base.field_5A)
            ex_constraint_block_data_set(cns);
    }
    else if (a2 == 5)
        cns->base.vftable->update(&cns->base);
}

static void ex_constraint_block_field_28(ex_constraint_block* cns) {
    cns->base.vftable->update(&cns->base);
}

static void ex_constraint_block_field_40(ex_constraint_block* cns) {

}

static void ex_constraint_block_field_48(ex_constraint_block* cns) {
    cns->base.vftable->update(&cns->base);
}

static void ex_constraint_block_field_50(ex_constraint_block* cns) {
    cns->base.vftable->update(&cns->base);
}

static void sub_1405F10D0(mat4* mat, vec3* a2, float_t a3, float_t a4) {
    vec3 v5;
    vec3 v6;
    vec3 v7;
    vec3 v8;
    vec3 v9;
    vec3_normalize(*a2, v5);
    vec3_mult_scalar(v5, 1.0f - a3, v9);
    vec3_mult_scalar(v5, v9.x, v6);
    vec3_mult_scalar(v5, v9.y, v7);
    vec3_mult_scalar(v5, v9.z, v8);
    mat->row0.x = v6.x + a3;
    mat->row0.y = v6.y - v5.z * a4;
    mat->row0.z = v6.z + v5.y * a4;
    mat->row0.w = 0.0f;
    mat->row1.x = v7.x + v5.z * a4;
    mat->row1.y = v7.y + a3;
    mat->row1.z = v7.z - v5.x * a4;
    mat->row1.w = 0.0f;
    mat->row2.x = v8.x - v5.y * a4;
    mat->row2.y = v8.y + v5.x * a4;
    mat->row2.z = v8.z + a3;
    mat->row2.w = 0.0f;
    mat->row3 = (vec4){ 0.0f, 0.0f, 0.0f, 1.0f };
}

static void sub_1401EB410(mat4* mat, vec3* a2, vec3* target_offset) {
    vec3 v3;
    vec3 v4;
    vec3_normalize(*a2, v3);
    vec3_normalize(*target_offset, v4);
    vec3 v13;
    vec3_cross(v4, v3, v13);

    float_t v18;
    vec3_dot(v3, v4, v18);
    v18 = clamp(v18, -1.0f, 1.0f);
    float_t v19 = 1.0f - v18;

    float_t v20 = 1.0f - v18 * v18;
    v20 = sqrtf(clamp(v20, 0.0f, 1.0f));

    sub_1405F10D0(mat, &v13, v18, v20);
}

static void ex_constraint_block_time_step(ex_constraint_block* cns) {
    bone_node* node = cns->base.bone_node;
    if (!node)
        return;

    vec3 pos;
    bone_node* parent_node = cns->base.parent_bone_node;
    vec3_mult(parent_node->exp_data.parent_scale, node->exp_data.position, pos);

    mat4 mat;
    mat4_translate_mult(parent_node->ex_data_mat, pos.x, pos.y, pos.z, &mat);

    bone_node* source_node_bone_node = cns->source_node_bone_node;
    switch (cns->type) {
    case OBJECT_SKIN_BLOCK_CONSTRAINT_ORIENTATION: {
        object_skin_block_constraint* cns_data = cns->cns_data;
        vec3 trans;
        mat4_get_translation(&mat, &trans);

        mat3 rot;
        mat3_from_mat4(source_node_bone_node->mat, &rot);
        mat3_normalize_rotation(&rot, &rot);

        vec3 offset = cns_data->orientation.offset;
        mat3_rotate_mult(&rot, offset.x, offset.y, offset.z, &rot);

        mat4_from_mat3(&rot, node->mat);
        mat4_set_translation(node->mat, &trans);
    } break;
    case OBJECT_SKIN_BLOCK_CONSTRAINT_DIRECTION: {
        object_skin_block_constraint* cns_data = cns->cns_data;
        vec3 align_axis = cns_data->direction.align_axis;
        vec3 target_offset = cns_data->direction.target_offset;
        mat4_mult_vec3_trans(source_node_bone_node->mat, &target_offset, &target_offset);
        mat4_mult_vec3_inv_trans(&mat, &target_offset, &target_offset);
        float_t target_offset_length;
        vec3_length_squared(target_offset, target_offset_length);
        if (target_offset_length <= 0.000001f)
            break;

        mat4 v59;
        sub_1401EB410(&v59, &align_axis, &target_offset);
        if (cns->direction_up_vector_bone_node) {
            vec3 affected_axis = cns_data->direction.up_vector.affected_axis;
            mat4 v56;
            mat4_mult(&v59, &mat, &v56);
            mat4_mult_vec3(&v56, &affected_axis, &affected_axis);
            mat4* v20 = cns->direction_up_vector_bone_node->mat;

            vec3 v23;
            vec3 v24;
            mat4_get_translation(v20, &v23);
            mat4_get_translation(&v56, &v24);
            vec3_sub(v23, v24, v23);

            vec3 v50;
            mat4_mult_vec3(&mat, &target_offset, &v50);

            vec3 v25;
            vec3 v29;
            vec3_cross(v50, affected_axis, v25);
            vec3_cross(v50, v23, v29);
            vec3_normalize(v25, v25);
            vec3_normalize(v29, v29);

            vec3 v35;
            vec3_cross(v25, v29, v35);

            float_t v39;
            float_t v36;
            vec3_dot(v29, v25, v39);
            vec3_dot(v35, v50, v36);

            float_t v40;
            vec3_length(v35, v40);
            if (v36 >= 0.0f)
                v40 = -v40;

            sub_1405F10D0(&v56, &target_offset, v39, v40);
            mat4_mult(&v59, &v56, &v59);
        }
        mat4_mult(&v59, &mat, node->mat);
    } break;
    case OBJECT_SKIN_BLOCK_CONSTRAINT_POSITION: {
        object_skin_block_constraint* cns_data = cns->cns_data;
        vec3 constraining_offset = cns_data->position.constraining_object.offset;
        vec3 constrained_offset = cns_data->position.constrained_object.offset;
        if (cns_data->position.constraining_object.affected_by_orientation)
            mat4_mult_vec3(source_node_bone_node->mat, &constraining_offset, &constraining_offset);

        vec3 source_node_trans;
        mat4_get_translation(source_node_bone_node->mat, &source_node_trans);
        vec3_add(constraining_offset, source_node_trans, source_node_trans);
        mat4_set_translation(&mat, &source_node_trans);
        if (cns->direction_up_vector_bone_node) {
            vec3 up_vector_trans;
            mat4_get_translation(cns->direction_up_vector_bone_node->mat, &up_vector_trans);
            mat4_mult_vec3_inv_trans(&mat, &up_vector_trans, &up_vector_trans);

            mat4 v26;
            sub_1401EB410(&v26, &cns_data->position.up_vector.affected_axis, &up_vector_trans);
            mat4_mult(&v26, &mat, &mat);
        }
        if (cns_data->position.constrained_object.affected_by_orientation)
            mat4_mult_vec3(&mat, &constrained_offset, &constrained_offset);

        mat4 constrained_offset_mat;
        mat4_translate(constrained_offset.x, constrained_offset.y,
            constrained_offset.z, &constrained_offset_mat);
        mat4_mult(&mat, &constrained_offset_mat, node->mat);
    } break;
    case OBJECT_SKIN_BLOCK_CONSTRAINT_DISTANCE:
    default:
        *node->mat = mat;
        break;
    }
}

static void ex_constraint_block_update(ex_constraint_block* cns) {
    if (!cns->base.parent_bone_node)
        return;

    if (cns->base.field_59)
        cns->base.field_59 = false;
    else {
        ex_constraint_block_time_step(cns);
        ex_constraint_block_data_set(cns);
        cns->base.field_59 = true;
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
    rob_chara_item_equip_object* itm_eq_obj, object_skin_block_expression* exp_data,
    char* exp_data_name, object_info a4, chara_index chara_index, bone_database* bone_data) {
    ex_expression_block_stack* stack_buf[28];
    ex_expression_block_stack** stack_buf_val = stack_buf;

    bone_node* node = rob_chara_item_equip_object_get_bone_node_by_name(
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
                    bone_node* v42 = rob_chara_item_equip_object_get_bone_node_by_name(itm_eq_obj,
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

    exp->step = chara_index >= CHARA_MIKU && chara_index <= CHARA_TETO;
}

static void ex_expression_block_data_set(ex_expression_block* exp) {
    bone_node* node = exp->base.bone_node;
    bone_node* parent_node = exp->base.parent_bone_node;
    bone_node_expression_data* data = &node->exp_data;
    vec3 parent_scale = exp->base.parent_bone_node->exp_data.parent_scale;
    mat4 ex_data_mat = *parent_node->ex_data_mat;
    mat4 mat = mat4_identity;
    bone_node_expression_data_mat_set(data, &parent_scale, &ex_data_mat, &mat);
    *node->mat = mat;
    *node->ex_data_mat = ex_data_mat;
}

static void ex_expression_block_draw(ex_expression_block* exp) {

}

static void ex_expression_block_field_8(ex_expression_block* exp) {
    exp->frame = 0.0;
}

static void ex_expression_block_field_10(ex_expression_block* exp) {
    bone_node_expression_data* exp_data = &exp->base.bone_node->exp_data;
    object_skin_block_expression* exp_exp_data = exp->exp_data;
    exp_data->position = exp_exp_data->base.position;
    exp_data->rotation = exp_exp_data->base.rotation;
    exp_data->scale = exp_exp_data->base.scale;
    exp->base.field_59 = false;
}

static void ex_expression_block_field_18(ex_expression_block* exp, int32_t a2, bool a3) {
    if (exp->base.field_59)
        return;

    if (a2 == 0) {
        if (exp->base.field_58)
            exp->base.vftable->update(&exp->base);
    }
    else if (a2 == 2) {
        if (exp->base.field_5A)
            ex_expression_block_data_set(exp);
    }
    else if (a2 == 5)
        exp->base.vftable->update(&exp->base);
}

static void ex_expression_block_field_28(ex_expression_block* exp) {
    exp->base.vftable->update(&exp->base);
}

static void ex_expression_block_field_40(ex_expression_block* exp) {

}

static void ex_expression_block_field_48(ex_expression_block* exp) {
    exp->base.vftable->update(&exp->base);
}

static void ex_expression_block_field_50(ex_expression_block* exp) {
    exp->base.vftable->update(&exp->base);
}

static void ex_expression_block_time_step(ex_expression_block* exp) {
    float_t delta_frame = get_delta_frame();
    if (exp->step)
        delta_frame *= exp->base.item_equip_object->item_equip->step;
    float_t frame = exp->frame + delta_frame;
    exp->frame = frame >= 65535.0f ? frame - 65535.0f : frame;

    if (exp->field_3D28) {
        exp->field_3D28(&exp->base.bone_node->exp_data);
        return;
    }

    for (int32_t i = 0; i < 9; i++) {
        if (!exp->expressions[i])
            break;

        float_t value = ex_expression_block_stack_get_value(exp->expressions[i]);
        if (exp->types[i] == EX_EXPRESSION_BLOCK_STACK_VARIABLE_RADIAN)
            value *= DEG_TO_RAD_FLOAT;
        *exp->values[i] = value;
    }
}

static void ex_expression_block_update(ex_expression_block* exp) {
    if (!exp->base.parent_bone_node)
        return;

    if (exp->base.field_59) {
        exp->base.field_59 = false;
        return;
    }

    ex_expression_block_time_step(exp);
    ex_expression_block_data_set(exp);
    exp->base.field_59 = true;
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

static void ex_node_block_field_10(ex_node_block* node) {
    node->field_59 = false;
}

static void ex_node_block_field_58(ex_node_block* node) {
    node->field_59 = 0;
}

static void ex_node_block_init_data(ex_node_block* node, bone_node* bone_node,
    ex_node_type type, char* name, rob_chara_item_equip_object* itm_eq_obj) {
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

static void ex_node_block_free(ex_node_block* node) {
    string_free(&node->parent_name);
}

static void sub_14047EE90(rob_osage* rob_osg, mat4* mat) {
    if (rob_osg->field_1F70) {
        /*list_struc_479_node* v5 = rob_osg->field_1F70->head->next;
        for (rob_osage_node* v6 = rob_osg->nodes.begin + 1; v6 != rob_osg->nodes.end; v6++) {
            v6->field_A0 = v5->value.field_0;
            v6->field_AC = v5->value.field_C;
            v6->field_B8 = v5->value.field_18;
            v6->field_C4 = v5->value.field_24;
            v5 = v5->next;
        }*/
        rob_osg->field_1F70 = 0;
    }

    for (rob_osage_node* v9 = rob_osg->nodes.begin + 1; v9 != rob_osg->nodes.end; v9++) {
        mat4_mult_vec3(mat, &v9->field_A0.trans_diff, &v9->trans_diff);
        mat4_mult_vec3_trans(mat, &v9->field_A0.trans, &v9->trans);
    }
    mat4_to_mat4u(rob_osg->parent_mat_ptr, &rob_osg->parent_mat);
}

static void sub_14047F110(rob_osage* rob_osg, mat4* mat, vec3* parent_scale, bool init_rot) {
    vec3 position;
    vec3_mult(rob_osg->exp_data.position, *parent_scale, position);
    mat4_translate_mult(mat, position.x, position.y, position.z, mat);

    vec3 rotation = rob_osg->exp_data.rotation;
    mat4_rotate_mult(mat, rotation.x, rotation.y, rotation.z, mat);

    skin_param* skin_param = rob_osg->skin_param_ptr;
    vec3 rot = skin_param->rot;
    if (init_rot)
        vec3_add(rot, skin_param->init_rot, rot);
    mat4_rotate_mult(mat, rot.x, rot.y, rot.z, mat);
}

static bool sub_140482FF0(mat4* mat, vec3* a2, skin_param_hinge* hinge, vec3* a4, int32_t a5) {
    bool clipped = false;
    float_t z_rot;
    float_t y_rot;
    if (a5 == 1) {
        y_rot = atan2f(-a2->z, a2->x);
        z_rot = atan2f(a2->y, sqrtf(a2->x * a2->x + a2->z * a2->z));
        if (hinge) {
            if (y_rot > hinge->ymax) {
                y_rot = hinge->ymax;
                clipped = true;
            }

            if (y_rot < hinge->ymin) {
                y_rot = hinge->ymin;
                clipped = true;
            }

            if (z_rot > hinge->zmax) {
                z_rot = hinge->zmax;
                clipped = true;
            }

            if (z_rot < hinge->zmin) {
                z_rot = hinge->zmin;
                clipped = true;
            }
        }
        mat4_rotate_y_mult(mat, y_rot, mat);
        mat4_rotate_z_mult(mat, z_rot, mat);
    }
    else {
        z_rot = atan2f(a2->y, a2->x);
        y_rot = atan2f(-a2->z, sqrtf(a2->x * a2->x + a2->y * a2->y));
        if (hinge) {
            if (y_rot > hinge->ymax) {
                y_rot = hinge->ymax;
                clipped = true;
            }

            if (y_rot < hinge->ymin) {
                y_rot = hinge->ymin;
                clipped = true;
            }

            if (z_rot > hinge->zmax) {
                z_rot = hinge->zmax;
                clipped = true;
            }

            if (z_rot < hinge->zmin) {
                z_rot = hinge->zmin;
                clipped = true;
            }
        }
        mat4_rotate_z_mult(mat, z_rot, mat);
        mat4_rotate_y_mult(mat, y_rot, mat);
    }

    if (a4) {
        a4->y = y_rot;
        a4->z = z_rot;
    }
    return clipped;
}

static void sub_1404803B0(rob_osage* rob_osg, mat4* mat, vec3* parent_scale, bool a4) {
    mat4 v47 = *mat;
    vec3 v45;
    vec3_mult(rob_osg->exp_data.position, *parent_scale, v45);
    mat4_mult_vec3_trans(&v47, &v45, &rob_osg->nodes.begin[0].trans);
    if (rob_osg->wind_reset && !rob_osg->field_1F0E) {
        rob_osg->field_1F0E = true;
        sub_14047EE90(rob_osg, mat);
    }

    if (!rob_osg->field_1F0C) {
        rob_osg->field_1F0C = true;

        float_t move_cancel = rob_osg->skin_param_ptr->move_cancel;
        if (rob_osg->move_cancel == 1.0f)
            move_cancel = 1.0f;
        if (move_cancel < 0.0f)
            move_cancel = rob_osg->move_cancel;

        if (move_cancel > 0.0f)
            for (rob_osage_node* v19 = rob_osg->nodes.begin + 1; v19 != rob_osg->nodes.end; v19++) {
                mat4 parent_mat;
                vec3 v46;
                mat4_mult_vec3_inv_trans(&parent_mat, &v19->trans, &v46);
                mat4_mult_vec3_trans(&parent_mat, &v46, &v46);
                vec3_sub(v46, v19->trans, v46);
                vec3_mult_scalar(v46, move_cancel, v46);
                vec3_add(v19->trans, v46, v19->trans);
            }
    }

    if (a4) {
        sub_14047F110(rob_osg, &v47, parent_scale, 0);
        *rob_osg->nodes.begin->bone_node_mat = v47;
        for (rob_osage_node* v29 = rob_osg->nodes.begin, *v30 = rob_osg->nodes.begin + 1;
            v30 != rob_osg->nodes.end; v29++, v30++) {
            vec3 v46;
            mat4_mult_vec3_inv_trans(&v47, &v30->trans, &v46);
            bool v32 = sub_140482FF0(&v47, &v46, &v30->data_ptr->skp_osg_node.hinge,
                &v30->field_A0.rotation, rob_osg->field_1EB8);
            *v30->bone_node->ex_data_mat = v47;
            float_t v34;
            vec3_distance(v30->trans, v29->trans, v34);
            float_t v35 = parent_scale->x * v30->length;
            bool v36;
            if (v34 >= fabsf(v35)) {
                v35 = v34;
                v36 = false;
            }
            else
                v36 = true;
            mat4_translate_mult(&v47, v35, 0.0f, 0.0f, &v47);
            if (v32 || v36)
                mat4_get_translation(&v47, &v30->trans);
        }

        if (rob_osg->nodes.begin != rob_osg->nodes.end && rob_osg->node.bone_node_mat) {
            mat4 v48 = *rob_osg->nodes.end[-1].bone_node->ex_data_mat;
            mat4_translate_mult(&v48, parent_scale->x * rob_osg->node.length, 0.0f, 0.0f, &v48);
            *rob_osg->node.bone_node->ex_data_mat = v48;
        }
    }
}

static void sub_140482300(vec3* a1, vec3* a2, vec3* a3, float_t osage_gravity_const, float_t weight) {
    weight *= osage_gravity_const;
    vec3 diff;
    vec3_sub(*a2, *a3, diff);
    float_t dist;
    vec3_length(diff, dist);
    if (dist <= fabsf(osage_gravity_const)) {
        diff.y = osage_gravity_const;
        vec3_length(diff, dist);
    }
    vec3_mult_scalar(diff, weight * diff.y * (1.0f / (dist * dist)), diff);
    vec3_negate(diff, diff);
    diff.y -= weight;
    *a1 = diff;
}

static void sub_140482F30(vec3* pos1, vec3* pos2, float_t length) {
    vec3 diff;
    vec3_sub(*pos1, *pos2, diff);
    float_t dist;
    vec3_length_squared(diff, dist);
    if (dist > length * length) {
        vec3_mult_scalar(diff, length / sqrtf(dist), diff);
        vec3_add(*pos2, diff, *pos1);
    }
}

static void sub_140482490(rob_osage_node* node, float_t step, float_t a3) {
    if (step != 1.0f) {
        vec3 v4;
        vec3_sub(node->trans, node->trans_orig, v4);

        float_t v9;
        vec3_length(v4, v9);
        if (v9 != 0.0f)
            vec3_mult_scalar(v4, 1.0f / v9, v4);
        vec3_mult_scalar(v4, step * v9, v4);
        vec3_add(node->trans_orig, v4, node->trans);
    }

    sub_140482F30(&node->trans, &node[-1].trans, node->length * a3);
    if (node->sibling_node)
        sub_140482F30(&node->trans, &node->sibling_node->trans, node->distance);
}

static void sub_140482180(rob_osage_node* node, float_t a2) {
    float_t v2 = node->data_ptr->skp_osg_node.coli_r + a2;
    if (v2 <= node->trans.y)
        return;

    node->trans.y = v2;
    vec3 v3;
    vec3_sub(node->trans, node[-1].trans, v3);
    vec3_normalize(v3, v3);
    vec3_mult_scalar(v3, node->length, v3);
    vec3_add(node[-1].trans, v3, node->trans);
    node->trans_diff = vec3_null;
    node->field_C8 += 1.0f;
}

static void sub_14047C800(rob_osage* rob_osg, mat4* mat,
    vec3* parent_scale, float_t step, bool a5, bool a6, bool a7) {
    rob_osage_node* v7 = rob_osg->nodes.end;
    if (rob_osg->nodes.begin == v7)
        return;

    const float_t osage_gravity_const = get_osage_gravity_const();
    sub_1404803B0(rob_osg, mat, parent_scale, 0);

    rob_osage_node* v17 = &rob_osg->nodes.begin[0];
    v17->trans_orig = v17->trans;
    vec3 v113;
    vec3_mult(rob_osg->exp_data.position, *parent_scale, v113);

    mat4 v130 = *mat;
    mat4_mult_vec3_trans(&v130, &v113, &v17->trans);
    vec3_sub(v17->trans, v17->trans_orig, v17->trans_diff);
    sub_14047F110(rob_osg, &v130, parent_scale, 0);
    *rob_osg->nodes.begin->bone_node_mat = v130;
    *rob_osg->nodes.begin->bone_node->ex_data_mat = v130;

    v113 = (vec3){ 1.0f, 0.0f, 0.0f };
    vec3 v128;
    mat4_mult_vec3(&v130, &v113, &v128);

    float_t v25 = parent_scale->x;
    if (!rob_osg->wind_reset && step <= 0.0f)
        return;

    bool stiffness = rob_osg->skin_param_ptr->stiffness > 0.0f;

    rob_osage_node* v12 = rob_osg->nodes.end;
    rob_osage_node* v30 = rob_osg->nodes.begin;
    for (rob_osage_node* v26 = rob_osg->nodes.begin + 1; v26 != v12; v26++, v30++) {
        rob_osage_node_data* v31 = v26->data_ptr;
        float_t weight = v31->skp_osg_node.weight;
        vec3 v111;
        if (!rob_osg->field_1F78) {
            sub_140482300(&v111, &v26->trans, &v30->trans, osage_gravity_const, weight);

            if (v26 != &v12[-1]) {
                vec3 v112;
                sub_140482300(&v112, &v26->trans, &v26[1].trans, osage_gravity_const, weight);
                vec3_add(v111, v112, v111);
                vec3_mult_scalar(v111, 0.5f, v111);
            }
        }
        else
            vec3_mult_scalar(rob_osg->field_1F7C, 1.0f / weight, v111);

        vec3 v127;
        vec3_mult_scalar(v128, v31->force * v26->force, v127);
        float_t v41 = (1.0f - rob_osg->field_1EB4) * (1.0f - rob_osg->skin_param_ptr->air_res);

        vec3 v126;
        vec3_add(v111, v127, v126);
        vec3_mult_scalar(v26->trans_diff, v41, v111);
        vec3_sub(v126, v111, v126);
        vec3_mult_scalar(v26->field_D0, weight, v111);
        vec3_add(v126, v111, v126);

        if (!a5) {
            vec3_mult_scalar(rob_osg->wind_direction, rob_osg->skin_param_ptr->wind_afc, v111);
            vec3_add(v126, v111, v126);
        }

        if (stiffness) {
            vec3 v116;
            vec3_sub(v26->trans_diff, v30->trans_diff, v116);
            vec3_mult_scalar(v116, 1.0f - rob_osg->skin_param_ptr->air_res, v116);
            vec3_sub(v126, v116, v126);
        }

        float_t v49 = 1.0f / (weight - (weight - 1.0f) * v31->skp_osg_node.inertial_cancel);
        vec3_mult_scalar(v126, v49, v26->field_28);
    }

    if (stiffness) {
        mat4 v131 = v130;
        vec3 v111;
        mat4_get_translation(&v131, &v111);

        for (rob_osage_node* v55 = rob_osg->nodes.begin + 1; v55 != v12; v55++) {
            mat4_mult_vec3_trans(&v131, &v55->field_94, &v128);

            vec3 v123;
            vec3_add(v55->trans_diff, v55->field_28, v123);
            vec3 v126;
            vec3_add(v55->trans, v123, v126);
            sub_140482F30(&v126, &v111, v25 * v55->length);
            vec3 v117;
            vec3_sub(v128, v126, v117);
            vec3_mult_scalar(v117, rob_osg->skin_param_ptr->stiffness, v117);
            float_t weight = v55->data_ptr->skp_osg_node.weight;
            float_t v74 = 1.0f / (weight - (weight - 1.0f) * v55->data_ptr->skp_osg_node.inertial_cancel);
            vec3_mult_scalar(v117, v74, v117);
            vec3_add(v55->field_28, v117, v55->field_28);
            vec3_add(v55->trans, v117, v126);
            vec3_add(v126, v123, v126);

            vec3 v129;
            mat4_mult_vec3_inv_trans(&v131, &v126, &v129);

            sub_140482FF0(&v131, &v129, 0, 0, rob_osg->field_1EB8);

            float_t v58;
            vec3_distance(v111, v126, v58);
            mat4_translate_mult(&v131, v58, 0.0f, 0.0f, &v131);

            v111 = v126;
        }
    }

    for (rob_osage_node* v82 = rob_osg->nodes.begin + 1; v82 != v12; v82++) {
        v82->trans_orig = v82->trans;
        vec3_add(v82->field_28, v82->trans_diff, v82->trans_diff);
        vec3_add(v82->trans_diff, v82->trans, v82->trans);
    }

    for (rob_osage_node* v90 = v12 - 2; v90 != rob_osg->nodes.begin; v90--)
        sub_140482F30(&v90->trans, &v90[1].trans, v25 * v90->child_length);

    if (a6) {
        rob_osage_node* v91 = rob_osg->nodes.begin;
        float_t v93 = v91->data_ptr->skp_osg_node.coli_r;
        float_t v97;
        if (v91->trans.x < rob_osg->ring.ring_rectangle_x - v93
            || v91->trans.z < rob_osg->ring.ring_rectangle_y - v93
            || v91->trans.x > rob_osg->ring.ring_rectangle_x + rob_osg->ring.ring_rectangle_width
            || v91->trans.z > rob_osg->ring.ring_rectangle_y + rob_osg->ring.ring_rectangle_height)
            v97 = rob_osg->ring.ring_out_height;
        else
            v97 = rob_osg->ring.ring_height;

        float_t v96 = v97 + v93;
        for (rob_osage_node* v98 = v91 + 1; v98 != v12; v98++) {
            sub_140482490(v98, step, v25);
            sub_140482180(v98, v96);
        }
    }

    if (a7) {
        for (rob_osage_node* v99 = rob_osg->nodes.begin + 1, *v100 = rob_osg->nodes.begin;
            v99 != v12; v99++, v100++) {
            vec3 v129;
            mat4_mult_vec3_inv_trans(&v130, &v99->trans, &v129);
            bool v102 = sub_140482FF0(&v130, &v129,
                &v99->data_ptr->skp_osg_node.hinge,
                &v99->field_A0.rotation, rob_osg->field_1EB8);
            *v99->bone_node->ex_data_mat = v130;

            float_t v104;
            vec3_distance(v99->trans, v100->trans, v104);
            float_t v105 = v25 * v99->length;

            bool v106;
            if (v104 >= fabs(v105)) {
                v105 = sqrtf(v104);
                v106 = false;
            }
            else
                v106 = true;

            mat4_translate_mult(&v130, v105, 0.0f, 0.0f, &v130);
            if (v102 || v106)
                mat4_get_translation(&v130, &v99->trans);
        }

        if (rob_osg->nodes.begin != rob_osg->nodes.end && rob_osg->node.bone_node_mat) {
            mat4 v131 = *rob_osg->nodes.end[-1].bone_node->ex_data_mat;
            mat4_translate_mult(&v131, v25 * rob_osg->node.length, 0.0f, 0.0f, &v131);
            *rob_osg->node.bone_node->ex_data_mat = v131;
        }
    }
    rob_osg->field_2A0 = false;
}

static int32_t sub_140483DE0(vec3* a1, vec3* a2, vec3* a3, float_t radius) {
    vec3 v5;
    float_t length;
    vec3_sub(*a2, *a3, v5);
    vec3_length_squared(v5, length);
    if (length > 0.000001f && length < radius * radius) {
        vec3_mult_scalar(v5, radius / sqrtf(length) - 1.0f, *a1);
        return 1;
    }
    return 0;
}

static int32_t sub_140484540(vec3* a1, vec3* a2, osage_coli* coli, float_t radius) {
    vec3 v11;
    vec3_sub(*a2, coli->bone0_pos, v11);

    float_t v17;
    vec3_dot(v11, coli->bone_pos_diff, v17);
    if (v17 < 0.0f)
        return sub_140483DE0(a1, a2, &coli->bone0_pos, radius);

    float_t v19 = coli->bone_pos_diff_length_squared;
    if (fabsf(v19) <= 0.000001f)
        return sub_140483DE0(a1, a2, &coli->bone0_pos, radius);
    if (v17 > v19)
        return sub_140483DE0(a1, a2, &coli->bone1_pos, radius);

    float_t v20;
    vec3_length_squared(v11, v20);
    float_t v22 = fabsf(v20 - v17 * v17 / v19);
    if (v22 <= 0.000001f || v22 >= radius * radius)
        return 0;

    vec3 v29;
    vec3_mult_scalar(coli->bone_pos_diff, v17 / v19, v29);
    vec3_sub(v11, v29, v29);
    vec3_mult_scalar(v29, radius / sqrtf(v22) - 1.0f, *a1);
    return 1;
}

static int32_t sub_140484780(vec3* a1, vec3* a2, vec3* a3, vec3* a4, float_t radius) {
    float_t v3;
    vec3_dot(*a2, *a4, v3);
    float_t v4;
    vec3_dot(*a3, *a4, v4);
    float_t v5 = v3 - v4 - radius;
    if (v5 >= 0.0f)
        return 0;

    vec3_mult_scalar(*a4, -v5, *a1);
    return 1;
}

static int32_t sub_140483EA0(vec3* a1, vec3* a2, osage_coli* a3, float_t radius) {
    if (a3->bone_pos_diff_length <= 0.000001f)
        return sub_140483DE0(a1, a2, &a3->bone0_pos, radius);

    float_t v61 = a3->bone_pos_diff_length * 0.5f;
    float_t v13 = sqrtf(radius * radius + v61 * v61);

    float_t v20;
    vec3 v54;
    vec3_sub(*a2, a3->bone0_pos, v54);
    vec3_length(v54, v20);

    float_t v21;
    vec3 v57;
    vec3_sub(*a2, a3->bone1_pos, v57);
    vec3_length(v57, v21);

    if (v20 <= 0.000001f || v21 <= 0.000001f)
        return 0;

    float_t v22 = v20 + v21;
    if (v21 + v20 >= v13 * 2.0f)
        return 0;

    float_t v25 = 1.0f / a3->bone_pos_diff_length;

    vec3 v63;
    vec3_add(a3->bone0_pos, a3->bone1_pos, v63);
    vec3_mult_scalar(v63, 0.5f, v63);
    vec3_sub(*a2, v63, v63);

    float_t v58;
    vec3_length(v63, v58);
    if (v58 != 0.0f)
        vec3_mult_scalar(v63, 1.0f / v58, v63);

    float_t v36;
    vec3_dot(v63, a3->bone_pos_diff, v36);
    v36 *= v25;
    float_t v37 = v36 * v36;
    v37 = min(v37, 1.0f);
    v22 *= 0.5f;
    float_t v38 = v22 * v22 - v61;
    if (v38 < 0.0f)
        return 0;

    float_t v39 = sqrtf(v38);
    if (v39 <= 0.000001f)
        return 0;

    float_t v42 = sqrtf(1.0f - v37);
    v22 = (v13 / v22 - 1.0f) * v36 * v58;
    v42 = (radius / v39 - 1.0f) * v42 * v58;
    float_t v43 = sqrtf(v42 * v42 + v22 * v22);
    if (v20 != 0.0f)
        vec3_mult_scalar(v54, 1.0f / v20, v54);

    if (v21 != 0.0f)
        vec3_mult_scalar(v57, 1.0f / v21, v57);

    vec3 v48;
    vec3_add(v54, v57, v48);
    vec3_normalize(v48, v48);
    vec3_mult_scalar(v48, v43, *a1);
    return 1;
}

static int32_t sub_140483B30(vec3* a1, vec3* a2, osage_coli* a3, float_t radius) {
    vec3 bone_pos_diff;
    vec3_add(a3->bone0_pos, a3->bone1_pos, bone_pos_diff);

    vec3 v29;
    vec3 v30;
    for (int32_t i = 0; i < 3; i++) {
        float_t v13 = ((float_t*)a2)[i];
        float_t v14 = ((float_t*)&a3->bone0_pos)[i] - v13;
        ((float_t*)&v30)[i] = v14;
        if (v14 > radius)
            return 0;

        v14 = v13 - ((float_t*)&bone_pos_diff)[i];
        ((float_t*)&v29)[i] = v14;
        if (v14 > radius)
            return 0;
    }

    vec3 v28 = vec3_null;
    vec3 v32 = vec3_null;
    vec3 v27 = vec3_identity;

    float_t v16 = 0.0f;
    for (int32_t i = 0; i < 3; i++) {
        float_t v18 = ((float_t*)&v30)[i];
        float_t v19 = ((float_t*)&v29)[i];
        if (fabsf(v18) >= fabsf(v19)) {
            ((float_t*)&v28)[i] = v19;
            ((float_t*)&v27)[i] = 1.0f;
            v18 = v19;
        }
        else {
            ((float_t*)&v28)[i] = v18;
            ((float_t*)&v27)[i] = -1.0f;
        }

        if (v18 > 0.0f) {
            float_t v20 = v18 * v18;
            ((float_t*)&v32)[i] = v20;
            v16 += v20;
        }
    }

    if (v16 > radius * radius)
        return 0;

    if (v16 > 0.0f) {
        vec3_mult(v27, v32, v32);
        vec3_mult_scalar(v32, 1.0f / v16, v32);
        vec3_mult_scalar(v32, radius - sqrtf(v16), *a1);
    }
    else {
        float_t v25 = ((float_t*)&v28)[0];
        int32_t v26 = 0;
        for (int32_t i = 0; i < 3; i++) {
            if (((float_t*)&v28)[i] > v25) {
                v25 = ((float_t*)&v28)[i];
                v26 = i;
            }
        }
        ((float_t*)a1)[v26] -= ((float_t*)&v27)[v26] * (v25 - radius);
    }
    return 1;
}

static int32_t sub_140485220(vec3* a1, skin_param_osage_node* a2, osage_coli* a3, float_t* a4) {
    if (!a3)
        return 0;

    int32_t v8 = 0;
    while (a3->type) {
        int32_t v11 = 0;
        vec3 v31 = vec3_null;
        switch (a3->type) {
        case 1:
            v11 = sub_140483DE0(&v31, a1, &a3->bone0_pos, a3->radius + a2->coli_r);
            break;
        case 2:
            v11 = sub_140484540(&v31, a1, a3, a3->radius + a2->coli_r);
            break;
        case 3:
            v11 = sub_140484780(&v31, a1, &a3->bone0_pos, &a3->bone1_pos, a2->coli_r);
            break;
        case 4:
            v11 = sub_140483EA0(&v31, a1, a3, a3->radius + a2->coli_r);
            break;
        case 5:
            v11 = sub_140483B30(&v31, a1, a3, a2->coli_r);
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

static int32_t sub_1404851C0(vec3* a1, skin_param_osage_node* a2, vector_osage_coli* a3, float_t* a4) {
    if (a3->begin == a3->end || a3->end[-1].type)
        return 0;
    return sub_140485220(a1, a2, a3->begin, a4);
}

static void sub_14047D8C0(rob_osage* rob_osg, mat4* mat, vec3* parent_scale, float_t step, bool a5) {
    if (!rob_osg->wind_reset && step <= 0.0f)
        return;

    osage_coli* coli = rob_osg->coli;
    osage_coli* coli_ring = rob_osg->coli_ring;
    vector_osage_coli* vec_ring_coli = &rob_osg->ring.coli;

    float_t v9 = 0.0f;
    if (step > 0.0f)
        v9 = 1.0f / step;

    mat4 v64;
    if (a5)
        v64 = *rob_osg->nodes.begin[0].bone_node_mat;
    else {
        v64 = *mat;

        vec3 trans;
        vec3_mult(rob_osg->exp_data.position, *parent_scale, trans);
        mat4_mult_vec3_trans(&v64, &trans, &rob_osg->nodes.begin[0].trans);
        sub_14047F110(rob_osg, &v64, parent_scale, 0);
    }

    float_t v60 = -1000.0f;
    if (a5) {
        float_t v22;
        rob_osage_node* v16 = &rob_osg->nodes.begin[0];
        float_t v18 = v16->data_ptr->skp_osg_node.coli_r;
        if (v16->trans.x < rob_osg->ring.ring_rectangle_x - v18
            || v16->trans.z < rob_osg->ring.ring_rectangle_y - v18
            || v16->trans.x > rob_osg->ring.ring_rectangle_x + rob_osg->ring.ring_rectangle_width
            || v16->trans.z > rob_osg->ring.ring_rectangle_y + rob_osg->ring.ring_rectangle_height)
            v22 = rob_osg->ring.ring_out_height;
        else
            v22 = rob_osg->ring.ring_height;
        v60 = v22 + v18;
    }

    float_t  v23 = 0.2f;
    if (step < 1.0f)
        v23 = 0.2f / (2.0f - step);

    if (rob_osg->skin_param_ptr->colli_tgt_osg) {
        vector_rob_osage_node* v24 = rob_osg->skin_param_ptr->colli_tgt_osg;
        rob_osage_node* v26 = v24->end;
        rob_osage_node* v27 = rob_osg->nodes.begin;
        for (rob_osage_node* v25 = rob_osg->nodes.begin + 1;
            v25 != rob_osg->nodes.end; v25++, v27++) {
            vec3 v62 = vec3_null;
            for (rob_osage_node* v30 = v24->begin + 1; v30 != v26; v30++)
                sub_140483DE0(&v62, &v27->trans, &v30->trans,
                    v27->data_ptr->skp_osg_node.coli_r + v30->data_ptr->skp_osg_node.coli_r);
            vec3_add(v27->trans, v62, v27->trans);
        }
    }

    for (rob_osage_node* v35 = rob_osg->nodes.begin + 1; v35 != rob_osg->nodes.end; v35++) {
        skin_param_osage_node* skp_osg_node = &v35->data_ptr->skp_osg_node;
        float_t v37 = (1.0f - rob_osg->field_1EB4) * rob_osg->skin_param_ptr->friction;
        vec3* v38 = &v35->trans;
        if (a5) {
            sub_140482490(v35, step, parent_scale->x);
            v35->field_C8 = (float_t)sub_1404851C0(v38, skp_osg_node, vec_ring_coli, &v35->field_CC);
            if (!rob_osg->field_1F0F) {
                v35->field_C8 += (float_t)sub_140485220(v38, skp_osg_node, coli_ring, 0);
                v35->field_C8 += (float_t)sub_140485220(v38, skp_osg_node, coli, 0);
            }
            sub_140482180(v35, v60);
        }
        else
            sub_140482F30(v38, &v35[-1].trans, v35->length * parent_scale->x);

        vec3 v63;
        mat4_mult_vec3_inv_trans(&v64, v38, &v63);
        bool v40 = sub_140482FF0(&v64, &v63, &skp_osg_node->hinge,
            &v35->field_A0.rotation, rob_osg->field_1EB8);
        v35->bone_node->exp_data.parent_scale = *parent_scale;
        *v35->bone_node->ex_data_mat = v64;

        if (v35->bone_node_mat) {
            mat4* v41 = v35->bone_node_mat;
            mat4_scale_rot(&v64, parent_scale->x, parent_scale->y, parent_scale->z, v41);
        }

        float_t v42 = v35->length * parent_scale->x;
        float_t v44;
        bool v45;
        vec3_distance_squared(*v38, v35[-1].trans, v44);
        if (v44 >= v42 * v42) {
            v42 = sqrtf(v44);
            v45 = false;
        }
        else
            v45 = true;

        mat4_translate_mult(&v64, v42, 0.0f, 0.0f, &v64);
        v35->field_A0.field_24 = v42;
        if (v40 || v45)
            mat4_get_translation(&v64, v38);

        vec3 v62;
        vec3_sub(*v38, v35->trans_orig, v62);
        vec3_mult_scalar(v62, v9, v35->trans_diff);

        if (v35->field_C8 > 0.0f) {
            if (v35->field_CC < v37)
                v37 = v35->field_CC;
            vec3_mult_scalar(v35->trans_diff, v37, v35->trans_diff);
        }

        float_t v55;
        vec3_length_squared(v35->trans_diff, v55);
        if (v55 > v23 * v23)
            vec3_mult_scalar(v35->trans_diff, v23 / sqrtf(v55), v35->trans_diff);

        mat4 v65;
        mat4_mult_vec3_inv_trans(&v65, v38, &v35->field_A0.trans);
        mat4_mult_vec3_inv(&v65, &v35->trans_diff, &v35->field_A0.trans_diff);
    }

    if (rob_osg->nodes.begin != rob_osg->nodes.end && rob_osg->node.bone_node_mat) {
        mat4 v65 = *rob_osg->nodes.end[-1].bone_node->ex_data_mat;
        mat4_translate_mult(&v65, rob_osg->node.length * parent_scale->x, 0.0f, 0.0f, &v65);
        *rob_osg->node.bone_node->ex_data_mat = v65;
        mat4_scale_rot(&v65, parent_scale->x, parent_scale->y, parent_scale->z, &v65);
        *rob_osg->node.bone_node_mat = v65;
        rob_osg->node.bone_node->exp_data.parent_scale = *parent_scale;
    }
}

static void sub_140480F40(rob_osage* rob_osg, vec3* a2, float_t a3) {
    vec3 v4;
    if (a2)
        v4 = *a2;
    else
        v4 = vec3_null;

    size_t v7 = rob_osg->field_298;
    size_t v8 = 0;
    if (v7 >= 4) {
        float_t v9 = a3 * a3 * a3 * a3;
        v8 = ((v7 - 4) / 4 + 1) * 4;
        for (size_t v10 = v8 / 4; v10; v10--)
            vec3_mult_scalar(v4, v9, v4);
    }

    if (v8 < v7)
        for (size_t v12 = v7 - v8; v12; v12--)
            vec3_mult_scalar(v4, a3, v4);


    for (rob_osage_node* v13 = rob_osg->nodes.begin + 1;
        v13 != rob_osg->nodes.end; v13++) {
        v13->field_D0 = v4;
        vec3_mult_scalar(v4, a3, v4);
    }
}

static void rob_osage_set_nodes_force(rob_osage* rob_osg, float_t force) {
    for (rob_osage_node* i = rob_osg->nodes.begin + 1; i != rob_osg->nodes.end; i++)
        i->force = force;
}

static void sub_140480260(rob_osage* rob_osg, mat4* mat, vec3* parent_scale, float_t step, bool v5) {
    if (!v5) {
        sub_140480F40(rob_osg, 0, 1.0f);
        rob_osage_set_nodes_force(rob_osg, 1.0f);
        rob_osg->field_1F78 = 0;
        rob_osg->field_1F7C = vec3_null;
    }

    rob_osg->field_2A0 = true;
    rob_osg->field_2A1 = false;
    rob_osg->field_2A4 = -1.0f;
    rob_osg->field_1F0C = false;
    rob_osg->wind_reset = false;
    rob_osg->field_1F0E = false;

    for (rob_osage_node* i = rob_osg->nodes.begin; i != rob_osg->nodes.end; i++) {
        i->field_C8 = 0.0;
        i->field_CC = 1.0;
    }

    mat4_to_mat4u(rob_osg->parent_mat_ptr, &rob_osg->parent_mat);
}

static void sub_14047C770(rob_osage* rob_osg, mat4* mat, vec3* parent_scale, float_t step, bool a5) {
    sub_14047C800(rob_osg, mat, parent_scale, step, a5, false, false);
    sub_14047D8C0(rob_osg, mat, parent_scale, step, true);
    sub_140480260(rob_osg, mat, parent_scale, step, a5);
}

static void sub_14047C750(rob_osage* rob_osg, mat4* mat, vec3* parent_scale, float_t step) {
    sub_14047C770(rob_osg, mat, parent_scale, step, 0);
}

static void sub_14047ECA0(rob_osage* rob_osg, float_t step) {
    if (step <= 0.0f)
        return;

    vector_osage_coli* vec_coli = &rob_osg->ring.coli;
    osage_coli* coli_ring = rob_osg->coli_ring;
    osage_coli* coli = rob_osg->coli;
    bool v11 = rob_osg->field_1F0F;
    for (rob_osage_node* i = rob_osg->nodes.begin + 1; i != rob_osg->nodes.end; i++) {
        rob_osage_node_data* data = i->data_ptr;
        i->field_C8 += (float_t)sub_1404851C0(&i->trans, &data->skp_osg_node, vec_coli, &i->field_CC);
        if (v11)
            continue;

        for (rob_osage_node** j = data->boc.begin; j != data->boc.end; j++) {
            rob_osage_node* k = *j;
            k->field_C8 += (float_t)sub_140485220(&k->trans, &data->skp_osg_node, coli_ring, 0);
            k->field_C8 += (float_t)sub_140485220(&k->trans, &data->skp_osg_node, coli, 0);
        }

        i->field_C8 += (float_t)sub_140485220(&i->trans, &data->skp_osg_node, coli_ring, 0);
        i->field_C8 += (float_t)sub_140485220(&i->trans, &data->skp_osg_node, coli, 0);
    }
}

void sub_140484E10(vec3* a1, vec3* a2, vec3* a3, vec3* a4) {
    vec3 v6;
    vec3_sub(*a3, *a2, v6);

    vec3 v7;
    vec3_sub(*a4, *a2, v7);

    float_t v8;
    vec3_dot(v7, v6, v8);
    if (v8 < 0.0f) {
        *a1 = *a2;
        return;
    }

    float_t v9;
    vec3_length_squared(v6, v9);
    if (v9 <= 0.000001f)
        *a1 = *a2;
    else if (v8 <= v9) {
        float_t v10 = v8 / v9;
        vec3_mult_scalar(v6, v8 / v9, v6);
        vec3_add(*a2, v6, *a1);
    }
    else
        *a1 = *a3;
}

static int32_t sub_140484450(vec3* a1, vec3* a2, vec3* a3, vec3* a4, float_t radius) {
    vec3 v8;
    sub_140484E10(&v8, a2, a3, a4);
    return sub_140483DE0(a1, &v8, a4, radius);
}

static void sub_140484850(vec3* a1, vec3* a2, vec3* a3, osage_coli* coli) {
    vec3 v56;
    vec3_sub(*a3, *a2, v56);

    vec3 v60;
    vec3_cross(v56, coli->bone_pos_diff, v60);

    float_t v46;
    vec3_length_squared(v60, v46);
    if (v46 <= 0.000001f) {
        vec3 v56;
        vec3 v57;
        vec3 v58;
        vec3 v59;
        sub_140484E10(&v59, &coli->bone0_pos, &coli->bone1_pos, a2);
        sub_140484E10(&v58, &coli->bone0_pos, &coli->bone1_pos, a3);
        sub_140484E10(&v57, a2, a3, &coli->bone0_pos);
        sub_140484E10(&v56, a2, a3, &coli->bone1_pos);

        float_t v48;
        float_t v51;
        float_t v52;
        float_t v55;
        vec3_distance_squared(*a3, coli->bone0_pos, v48);
        vec3_distance_squared(coli->bone0_pos, v57, v51);
        vec3_distance_squared(coli->bone1_pos, v56, v52);
        vec3_distance_squared(*a2, v59, v55);
        *a1 = *a2;

        if (v55 > v48) {
            *a1 = *a3;
            v55 = v48;
        }

        if (v55 > v51) {
            *a1 = v57;
            v55 = v51;
        }

        if (v55 > v52)
            *a1 = v56;
    }
    else {
        float_t v25;
        vec3_length(v56, v25);
        if (v25 != 0.0)
            vec3_mult_scalar(v56, 1.0f / v25, v56);

        float_t v30 = 1.0f / coli->bone_pos_diff_length;
        vec3 v61;
        float_t v34;
        vec3_mult_scalar(coli->bone_pos_diff, v30, v61);
        vec3_dot(v61, v56, v34);
        vec3_mult_scalar(v61, v34, v61);
        vec3_sub(v61, v56, v61);
        vec3_sub(coli->bone0_pos, *a2, v60);
        float_t v35;
        vec3_dot(v61, v60, v35);
        v35 /= (v34 * v34 - 1.0f);
        if (v35 < 0.0f)
            *a1 = *a2;
        else if (v35 <= v25) {
            vec3_mult_scalar(v56, v35, v56);
            vec3_add(*a2, v56, *a1);
        }
        else
            *a1 = *a3;

    }
}

static int32_t sub_1404844A0(vec3* a1, vec3* a2, vec3* a3, osage_coli* a4, float_t radius) {
    vec3 v8;
    sub_140484850(&v8, a2, a3, a4);
    return sub_140484540(a1, &v8, a4, radius);
}

static int32_t sub_1404844F0(vec3* a1, vec3* a2, vec3* a3, osage_coli* a4, float_t radius) {
    vec3 v8;
    sub_140484850(&v8, a2, a3, a4);
    return sub_140483EA0(a1, &v8, a4, radius);
}

static int32_t sub_140485000(vec3* a1, vec3* a2, skin_param_osage_node* a3, osage_coli* a4) {
    if (!a4)
        return 0;

    int32_t v8 = 0;
    while (a4->type) {
        int32_t type = a4->type;
        vec3 v21 = vec3_null;
        switch (a4->type) {
        case 1: {
            v8 += sub_140484450(&v21, a1, a2, &a4->bone0_pos, a3->coli_r + a4->radius);
        } break;
        case 2:
            v8 += sub_1404844A0(&v21, a1, a2, a4, a3->coli_r + a4->radius);
            break;
        case 4:
            v8 += sub_1404844F0(&v21, a1, a2, a4, a3->coli_r + a4->radius);
            break;
        }

        if (v8 > 0) {
            vec3_add(*a1, v21, *a1);
            vec3_add(*a2, v21, *a2);
        }
        a4++;
    }
    return v8;
}

static void sub_14047D620(rob_osage* rob_osg, float_t step) {
    if (step < 0.0f && rob_osg->field_1F0F)
        return;

    vector_rob_osage_node* nodes = &rob_osg->nodes;
    vec3 v7 = nodes->begin[0].trans;
    osage_coli* coli = rob_osg->coli;
    osage_coli* coli_ring = rob_osg->coli_ring;
    int32_t coli_type = rob_osg->skin_param_ptr->coli_type;
    for (rob_osage_node* v10 = nodes->begin + 1; v10 != nodes->end; v10++) {
        rob_osage_node_data* v11 = v10->data_ptr;
        for (rob_osage_node** j = v11->boc.begin; j != v11->boc.end; j++) {
            rob_osage_node* v15 = *j;
            float_t v17 = (float_t)(
                sub_140485000(&v10->trans, &v15->trans, &v11->skp_osg_node, coli)
                + sub_140485000(&v10->trans, &v15->trans, &v11->skp_osg_node, coli_ring));
            v10->field_C8 += v17;
            v15->field_C8 += v17;
        }

        if (coli_type && (coli_type != 1 || v10 != nodes->begin + 1)) {
            float_t v20 = (float_t)(
                sub_140485000(&v10[0].trans, &v10[-1].trans, &v11->skp_osg_node, coli)
                + sub_140485000(&v10[0].trans, &v10[-1].trans, &v11->skp_osg_node, coli_ring));
            v10[0].field_C8 += v20;
            v10[-1].field_C8 += v20;
        }
    }
    nodes->begin[0].trans = v7;
}

static void sub_14047F990(rob_osage* a1, mat4* a2, vec3* a3, bool a4) {
    rob_osage_node* v4;
    v4 = a1->nodes.end;
    if (a1->nodes.begin == v4)
        return;

    vec3 v76;
    vec3_mult(a1->exp_data.position, *a3, v76);
    mat4_mult_vec3_trans(a2, &v76, &v76);
    vec3 v74 = v76;
    rob_osage_node* v12 = &a1->nodes.begin[0];
    v12->trans = v76;
    v12->trans_orig = v76;
    v12->trans_diff = vec3_null;

    float_t ring_height;
    rob_osage_node* v14 = &a1->nodes.begin[0];
    float_t coli_r = v14->data_ptr->skp_osg_node.coli_r;
    if (v14->trans.x < a1->ring.ring_rectangle_x - coli_r
        || v14->trans.z < a1->ring.ring_rectangle_y - coli_r
        || v14->trans.x > a1->ring.ring_rectangle_x + a1->ring.ring_rectangle_width
        || v14->trans.z > a1->ring.ring_rectangle_y + a1->ring.ring_rectangle_height)
        ring_height = a1->ring.ring_out_height;
    else
        ring_height = a1->ring.ring_height;

    mat4 v78 = *a2;
    sub_14047F110(a1, &v78, a3, true);
    vec3 v60 = (vec3){ 1.0f, 0.0f, 0.0f };
    mat4_mult_vec3(&v78, &v60, &v60);

    osage_coli* coli_ring = a1->coli_ring;
    osage_coli* coli = a1->coli;
    float_t v25 = ring_height + coli_r;
    float_t v16 = a3->x;
    for (rob_osage_node* i = a1->nodes.begin, *j = a1->nodes.begin + 1; j != a1->nodes.end; i++, j++) {
        vec3_normalize(v60, v60);
        vec3_mult_scalar(v60, v16 * j->length, v60);
        vec3_add(i->trans, v60, v74);
        if (a4 && j->sibling_node)
            sub_140482F30(&v74, &j->sibling_node->trans, j->distance);

        skin_param_osage_node* v38 = &j->data_ptr->skp_osg_node;
        sub_140485220(&v74, v38, coli_ring, 0);
        sub_140485220(&v74, v38, coli, 0);

        float_t v39 = v25 + v38->coli_r;
        if (v74.y < v39 && v39 < 1001.0f) {
            v74.y = v39;
            vec3_sub(v74, i->trans, v74);
            vec3_normalize(v74, v74);
            vec3_mult_scalar(v74, v16 * j->length, v74);
            vec3_add(v74, i->trans, v74);
        }
        j->trans = v74;
        j->trans_diff = vec3_null;
        vec3 v77;
        mat4_mult_vec3_inv_trans(&v78, &j->trans, &v77);
        sub_140482FF0(&v78, &v77, &j->data_ptr->skp_osg_node.hinge,
            &j->field_A0.rotation, a1->field_1EB8);
        j->bone_node->exp_data.parent_scale = *a3;
        *j->bone_node->ex_data_mat = v78;
        if (j->bone_node_mat)
            mat4_scale_rot(&v78, a3->x, a3->y, a3->z, j->bone_node_mat);

        float_t v55;
        vec3_distance(j->trans, i->trans, v55);
        float_t v56 = v16 * j->length;
        if (v55 >= fabsf(v56))
            v56 = v55;
        mat4_translate_mult(&v78, v56, 0.0f, 0.0f, &v78);
        mat4_get_translation(&v78, &j->trans);
        vec3_sub(j->trans, i->trans, v60);
    }

    if (a1->nodes.begin != a1->nodes.end && a1->node.bone_node_mat) {
        mat4 v79 = *a1->nodes.end[-1].bone_node->ex_data_mat;
        mat4_translate_mult(&v79, v16 * a1->node.length, 0.0f, 0.0f, &v79);
        *a1->node.bone_node->ex_data_mat = v79;
        mat4_scale_rot(&v79, a3->x, a3->y, a3->z, &v79);
        *a1->node.bone_node_mat = v79;
        a1->node.bone_node->exp_data.parent_scale = *a3;
    }
}

static bool sub_14053D1B0(vec3* l_trans, vec3* r_trans,
    vec3* u_trans, vec3* d_trans, vec3* a5, vec3* a6, vec3* a7) {
    float_t length;
    vec3_sub(*d_trans, *u_trans, *a5);
    vec3_length_squared(*a5, length);
    if (length <= 0.000001f)
        return false;
    vec3_mult_scalar(*a5, 1.0f / sqrtf(length), *a5);

    vec3 v9;
    vec3_sub(*r_trans, *l_trans, v9);
    vec3_cross(v9, *a5, *a6);
    vec3_length_squared(*a6, length);
    if (length <= 0.000001f)
        return false;
    vec3_mult_scalar(*a6, 1.0f / sqrtf(length), *a6);

    vec3_cross(*a5, *a6, *a7);
    vec3_normalize(*a7, *a7);
    return true;
}

static void sub_14053CE30(rob_osage_node_data_normal_ref* normal_ref, mat4* a2) {
    if (!normal_ref->field_0)
        return;

    vec3 n_trans;
    vec3 u_trans;
    vec3 d_trans;
    vec3 l_trans;
    vec3 r_trans;
    mat4_get_translation(normal_ref->n->bone_node->ex_data_mat, &n_trans);
    mat4_get_translation(normal_ref->u->bone_node->ex_data_mat, &u_trans);
    mat4_get_translation(normal_ref->d->bone_node->ex_data_mat, &d_trans);
    mat4_get_translation(normal_ref->l->bone_node->ex_data_mat, &l_trans);
    mat4_get_translation(normal_ref->r->bone_node->ex_data_mat, &r_trans);

    vec3 v27;
    vec3 v26;
    vec3 v28;
    if (sub_14053D1B0(&l_trans, &r_trans, &u_trans, &d_trans, &v27, &v26, &v28)) {
        mat4 v34;
        *(vec3*)&v34.row0 = v28;
        *(vec3*)&v34.row1 = v26;
        *(vec3*)&v34.row2 = v27;
        *(vec3*)&v34.row3 = n_trans;
        v34.row0.w = 0.0f;
        v34.row1.w = 0.0f;
        v34.row2.w = 0.0f;
        v34.row3.w = 1.0f;

        mat4 v33;
        mat4u_to_mat4(&normal_ref->mat, &v33);
        mat4_mult(&v33, &v34, a2);
    }
}

static void sub_14047E1C0(rob_osage* rob_osg, vec3* scale) {
    for (rob_osage_node* i = rob_osg->nodes.begin + 1; i != rob_osg->nodes.end; i++)
        if (i->data_ptr->normal_ref.field_0) {
            sub_14053CE30(&i->data_ptr->normal_ref, i->bone_node_mat);
            mat4_scale_rot(i->bone_node_mat, scale->x, scale->y, scale->z, i->bone_node_mat);
        }
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
    rob_osg->field_1EB4 = 0.0f;
    rob_osg->field_1EB8 = 0;
    rob_osg->field_2A0 = true;
    /*tree_295* cns_data = &rob_osg->field_1F60;
    sub_140216750(&rob_osg->field_1F60, rob_osg->field_1F60.head->parent);
    cns_data->head->parent = cns_data->head;
    cns_data->head->left = cns_data->head;
    cns_data->head->right = cns_data->head;
    rob_osg->field_1F60.size = 0;*/
    rob_osg->move_cancel = 0.0f;
    rob_osg->field_1F0C = false;
    rob_osg->wind_reset = false;
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
    rob_osg->parent_mat_ptr = 0;
    rob_osg->parent_mat = mat4u_null;
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

static void ex_osage_block_field_8(ex_osage_block* osg) {
    osg->base.vftable->reset(&osg->base);
}

static void ex_osage_block_field_18(ex_osage_block* osg, int32_t a2, bool a3) {
    rob_chara_item_equip* rob_item_equip = osg->base.item_equip_object->item_equip;
    float_t step = get_delta_frame() * rob_item_equip->step;
    if (rob_item_equip->field_940.begin != rob_item_equip->field_940.end
        && rob_item_equip->field_940.begin->field_C)
        step = 1.0f;

    bone_node* parent_node = osg->base.parent_bone_node;
    mat4* parent_node_mat = parent_node->ex_data_mat;
    vec3 parent_scale = parent_node->exp_data.parent_scale;
    switch (a2) {
    case 0: {
        sub_1404803B0(&osg->rob, parent_node_mat, &parent_scale, osg->base.field_5A);
    } break;
    case 1:
    case 2: {
        if ((a2 == 1 && osg->base.field_58) || (a2 == 2 && osg->rob.field_2A0)) {
            ex_osage_block_set_wind_direction(osg);
            sub_14047C800(&osg->rob, parent_node_mat, &parent_scale, step, a3, true, osg->base.field_5A);
        }
    } break;
    case 3: {
        rob_osage_coli_set(&osg->rob, osg->mat);
        sub_14047ECA0(&osg->rob, step);
    } break;
    case 4: {
        sub_14047D620(&osg->rob, step);
    } break;
    case 5: {
        sub_14047D8C0(&osg->rob, parent_node_mat, &parent_scale, step, false);
        sub_140480260(&osg->rob, parent_node_mat, &parent_scale, step, a3);
        osg->base.field_59 = true;
    } break;
    }
}

static void ex_osage_block_field_28(ex_osage_block* osg) {
    /*rob_chara_item_equip* rob_itm_equip = osg->base.item_equip_object->item_equip;
    bone_node* parent_node = osg->base.parent_bone_node;
    vec3 parent_scale = parent_node->exp_data.parent_scale;
    //sub_14047E240(&osg->rob, parent_node->ex_data_mat, &parent_scale, &rob_itm_equip->field_940);*/
}

static void ex_osage_block_field_40(ex_osage_block* osg) {

}

static void ex_osage_block_field_48(ex_osage_block* osg) {
    osg->step = 4.0f;
    ex_osage_block_set_wind_direction(osg);
    rob_osage_coli_set(&osg->rob, osg->mat);
    bone_node* parent_node = osg->base.parent_bone_node;
    vec3 parent_scale = parent_node->exp_data.parent_scale;
    sub_14047F990(&osg->rob, parent_node->ex_data_mat, &parent_scale, 0);
    osg->field_1FF8 &= ~2;
}

static void ex_osage_block_field_50(ex_osage_block* osg) {
    if (osg->base.field_59) {
        osg->base.field_59 = 0;
        return;
    }

    ex_osage_block_set_wind_direction(osg);

    bone_node* parent_node = osg->base.parent_bone_node;
    vec3 parent_scale = parent_node->exp_data.parent_scale;
    rob_osage_coli_set(&osg->rob, osg->mat);
    sub_14047C770(&osg->rob, parent_node->ex_data_mat, &parent_scale, osg->step, 1);
    float_t step = 0.5f * osg->step;
    osg->step = max(step, 1.0f);
}

static void ex_osage_block_field_58(ex_osage_block* osg) {
    bone_node* parent_node = osg->base.parent_bone_node;
    vec3 parent_scale = parent_node->exp_data.parent_scale;
    sub_14047E1C0(&osg->rob, &parent_scale);
}

static void ex_osage_block_init_data(ex_osage_block* osg,
    rob_chara_item_equip_object* itm_eq_obj, object_skin_block_osage* osg_data,
    char* osg_data_name, object_skin_osage_node* osg_nodes, bone_node* bone_nodes,
    bone_node* ex_data_bone_nodes, object_skin* skin) {
    ex_node_block_init_data(&osg->base,
        &ex_data_bone_nodes[osg_data->external_name_index & 0x7FFF], EX_OSAGE, osg_data_name, itm_eq_obj);
    rob_osage_init_data(&osg->rob, osg_data, osg_nodes, ex_data_bone_nodes, skin);
    osg->field_1FF8 &= ~2;
    osg->mat = bone_nodes->mat;
}

static void sub_1405F3E10(ex_osage_block* osg, object_skin_block_osage* osg_data,
    object_skin_osage_node* osg_nodes, vector_pair_uint32_t_ptr_rob_osage_node* a4,
    vector_pair_name_ex_osage_block* a5) {
    rob_osage* v5 = &osg->rob;

    rob_osage_node* v9 = rob_osage_get_node(&osg->rob, 0);
    pair_uint32_t_ptr_rob_osage_node* v38
        = vector_pair_uint32_t_ptr_rob_osage_node_reserve_back(a4);
    v38->key = osg_data->external_name_index;
    v38->value = v9;

    for (size_t i = 0; i < osg_data->count; i++) {
        v9 = rob_osage_get_node(v5, i);
        v38 = vector_pair_uint32_t_ptr_rob_osage_node_reserve_back(a4);
        v38->key = osg_nodes[i].name_index;
        v38->value = v9;

        if (v9->bone_node && v9->bone_node->name) {
            pair_name_ex_osage_block* v39 = vector_pair_name_ex_osage_block_reserve_back(a5);
            v39->name = v9->bone_node->name;
            v39->block = osg;
        }
    }

    v9 = rob_osage_get_node(v5, osg_data->count + 1ULL);
    v38 = vector_pair_uint32_t_ptr_rob_osage_node_reserve_back(a4);
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
    osg->step = 1.0f;
    ex_node_block_reset(&osg->base);
}

static void ex_osage_block_set_wind_direction(ex_osage_block* osg) {
    vec3_mult_scalar(wind_ptr->wind_direction,
        osg->base.item_equip_object->item_equip->wind_strength,
        osg->rob.wind_direction);
}

static void ex_osage_block_update(ex_osage_block* osg) {
    osg->field_1FF8 &= ~2;
    if (osg->base.field_59) {
        osg->base.field_59 = false;
        return;
    }

    rob_chara_item_equip* rob_item_equip = osg->base.item_equip_object->item_equip;
    float_t step = get_delta_frame() * rob_item_equip->step;
    if (rob_item_equip->field_940.begin != rob_item_equip->field_940.end
        && rob_item_equip->field_940.begin->field_C)
        step = 1.0f;

    bone_node* parent_node = osg->base.parent_bone_node;
    vec3 parent_scale = parent_node->exp_data.parent_scale;
    vec3 scale = parent_node->exp_data.scale;

    mat4 mat = *parent_node->ex_data_mat;
    if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f) {
        vec3_div(vec3_identity, scale, scale);
        mat4_scale_rot(&mat, scale.x, scale.y, scale.z, &mat);
    }
    ex_osage_block_set_wind_direction(osg);
    rob_osage_coli_set(&osg->rob, osg->mat);
    sub_14047C770(&osg->rob, &mat, &parent_scale, step, false);
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
    return acosf(v1) * RAD_TO_DEG_FLOAT;
}

static float_t exp_add(float_t v1, float_t v2) {
    return v1 + v2;
}

static float_t exp_and(float_t v1, float_t v2) {
    return v1 != 0.0f && v2 != 0.0f ? 1.0f : 0.0f;
}

static float_t exp_asin(float_t v1) {
    return asinf(v1) * RAD_TO_DEG_FLOAT;
}

static float_t exp_atan(float_t v1) {
    return atanf(v1) * RAD_TO_DEG_FLOAT;
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
    return cosf(fmodf(v1, 360.0f) * DEG_TO_RAD_FLOAT);
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
    return sinf(fmodf(v1, 360.0f) * DEG_TO_RAD_FLOAT);
}

static float_t exp_sqrt(float_t v1) {
    return sqrtf(v1);
}

static float_t exp_sub(float_t v1, float_t v2) {
    return v1 - v2;
}

static float_t exp_tan(float_t v1) {
    return tanf(fmodf(v1, 360.0f) * DEG_TO_RAD_FLOAT);
}

static const float_t get_osage_gravity_const() {
    return 0.00299444468691945f;
}

static void item_sub_data_item_change_free(item_sub_data_item_change* itm_chg) {
    vector_uint32_t_free(&itm_chg->item_ids, 0);
}

static void item_sub_data_texture_change_free(item_sub_data_texture_change* tex_chg) {
    vector_item_sub_data_texture_change_tex_free(&tex_chg->tex, item_sub_data_texture_change_tex_free);
}

static void item_sub_data_texture_change_tex_free(item_sub_data_texture_change_tex* tex_chg_tex) {
    if (tex_chg_tex->changed)
        texture_free(tex_chg_tex->chg);
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
        if (type == MOT_KEY_SET_NONE) {
            *value = 0.0f;
            continue;
        }
        else if (type == MOT_KEY_SET_STATIC) {
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

        if (key_index > 0)
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

    uint32_t bone_key_set_count = a1->bone_data.bone_key_set_count;
    if (frame != a1->mot_data.frame) {
        mot_parent_interpolate(&a1->mot_data, frame, bone_key_set_count, a1->bone_data.global_key_set_count);
        a1->mot_data.frame = frame;
    }

    keyframe_data = (vec3*)&a1->mot_data.key_set_data.begin[bone_key_set_count];
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
    rob_chara_bone_data* a2, bone_database* bone_data) {
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
        rob_chara_bone_data* rob_bone_data = v7->rob_bone_data;
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
    osage_coli* v5 = osg_coli;
    skin_param_osage_root_coli* v6 = skp_root_coli;
    osg_coli->type = 0;
    if (skp_root_coli && mats && skp_root_coli->type)
        while (v6->type) {
            v5->type = v6->type;
            v5->radius = v6->radius;

            mat4 mat = mats[v6->bone0_index];
            mat4_mult_vec3_trans(&mat, &v6->bone0_pos, &v5->bone0_pos);

            if (v6->type == 2 || v6->type == 4) {
                mat = mats[v6->bone1_index];
                mat4_mult_vec3_trans(&mat, &v6->bone1_pos, &v5->bone1_pos);
                vec3_sub(v5->bone1_pos, v5->bone0_pos, v5->bone_pos_diff);

                float_t length;
                vec3_length_squared(v5->bone_pos_diff, length);
                v5->bone_pos_diff_length_squared = length;
                length = sqrtf(length);
                v5->bone_pos_diff_length = length;
                if (length < 0.01f)
                    v5->type = 1;
            }
            else if (v6->type == 3)
                mat4_mult_vec3(&mat, &v6->bone1_pos, &v5->bone1_pos);

            v5++;
            v6++;
        }
    v5->type = 0;
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
        osg_coli->field_34 = 1.0f;
    }
    else
        osage_coli_set(osg_coli, vec_skp_root_coli->begin, mats);
}

static rob_chara_bone_data* rob_chara_bone_data_init() {
    rob_chara_bone_data* rob_bone_data = force_malloc(sizeof(rob_chara_bone_data));
    rob_bone_data->base_skeleton_type = BONE_DATABASE_SKELETON_NONE;
    rob_bone_data->skeleton_type = BONE_DATABASE_SKELETON_NONE;
    rob_bone_data->ik_scale.ratio0 = 1.0f;
    return rob_bone_data;
}

static void rob_chara_bone_data_calculate_bones(rob_chara_bone_data* rob_bone_data,
    vector_bone_database_bone* bones) {
    bone_database_bones_calculate_count(bones, &rob_bone_data->object_bone_count,
        &rob_bone_data->motion_bone_count, &rob_bone_data->total_bone_count,
        &rob_bone_data->ik_bone_count, &rob_bone_data->chain_pos);
}

static void rob_chara_bone_data_eyes_xrot_adjust(rob_chara_bone_data* rob_bone_data,
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

static float_t rob_chara_bone_data_get_frame(rob_chara_bone_data* rob_bone_data) {
    return rob_bone_data->motion_loaded.begin[0]->field_4A8.field_0.frame;
}

static float_t rob_chara_bone_data_get_frame_count(rob_chara_bone_data* rob_bone_data) {
    return (float_t)rob_bone_data->motion_loaded.begin[0]->mot_data.mot.frame_count;
}

static void rob_chara_bone_data_get_ik_scale(
    rob_chara_bone_data* rob_bone_data, bone_database* bone_data) {
    if (vector_length(rob_bone_data->motion_loaded) < 0)
        return;

    motion_blend_mot* v2 = rob_bone_data->motion_loaded.begin[0];
    rob_chara_bone_data_ik_scale_calculate(&rob_bone_data->ik_scale, &v2->bone_data.bones,
        rob_bone_data->base_skeleton_type, rob_bone_data->skeleton_type, bone_data);
    float_t ratio0 = rob_bone_data->ik_scale.ratio0;
    v2->field_4F8.field_C0 = ratio0;
    v2->field_4F8.field_C4 = ratio0;
    v2->field_4F8.field_C8 = vec3_null;
}

static mat4* rob_chara_bone_data_get_mat(rob_chara_bone_data* rob_bone_data, size_t index) {
    bone_node* node = rob_chara_bone_data_get_node(rob_bone_data, index);
    if (node)
        return node->mat;
    return 0;
}

static bone_node* rob_chara_bone_data_get_node(rob_chara_bone_data* rob_bone_data, size_t index) {
    if ((ssize_t)index < vector_length(rob_bone_data->nodes))
        return &rob_bone_data->nodes.begin[index];
    return 0;
}

static void rob_chara_bone_data_ik_scale_calculate(
    rob_chara_bone_data_ik_scale* ik_scale, vector_bone_data* vec_bone_data,
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

static void rob_chara_bone_data_init_data(rob_chara_bone_data* rob_bone_data,
    bone_database_skeleton_type base_skeleton_type,
    bone_database_skeleton_type skeleton_type, bone_database* bone_data) {
    rob_chara_bone_data_init_skeleton(rob_bone_data, base_skeleton_type, skeleton_type, bone_data);
    for (motion_blend_mot** i = rob_bone_data->motions.begin; i != rob_bone_data->motions.end; i++)
        motion_blend_mot_load_bone_data(*i, rob_bone_data, bone_data);
}

static void rob_chara_bone_data_init_skeleton(rob_chara_bone_data* rob_bone_data,
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

    rob_chara_bone_data_calculate_bones(rob_bone_data, bones);
    rob_chara_bone_data_reserve(rob_bone_data);
    rob_chara_bone_data_set_mats(rob_bone_data, bones);
    rob_chara_bone_data_set_parent_mats(rob_bone_data, parent_indices->begin);
    rob_bone_data->base_skeleton_type = base_skeleton_type;
    rob_bone_data->skeleton_type = skeleton_type;
}

static void rob_chara_bone_data_interpolate(rob_chara_bone_data* rob_bone_data) {
    if (vector_length(rob_bone_data->motion_loaded) < 0)
        return;

    for (motion_blend_mot** i = rob_bone_data->motion_loaded.begin;
        i != rob_bone_data->motion_loaded.end; i++)
        motion_blend_mot_interpolate(*i);
}

static void rob_chara_bone_data_motion_blend_mot_free(rob_chara_bone_data* rob_bone_data) {
    vector_size_t_clear(&rob_bone_data->motion_indices, 0);
    vector_ptr_motion_blend_mot_clear(&rob_bone_data->motions, motion_blend_mot_dispose);
}

static void rob_chara_bone_data_motion_blend_mot_init(rob_chara_bone_data* rob_bone_data) {
    for (int32_t i = 0; i < 3; i++) {
        *vector_ptr_motion_blend_mot_reserve_back(&rob_bone_data->motions) = motion_blend_mot_init();
        *vector_size_t_reserve_back(&rob_bone_data->motion_indices) = i;
    }
    rob_chara_bone_data_motion_blend_mot_list_init(rob_bone_data);
}

static void rob_chara_bone_data_motion_blend_mot_list_free(rob_chara_bone_data* rob_bone_data,
    size_t last_index) {
    for (motion_blend_mot** i = &rob_bone_data->motion_loaded.end[-1];
        i - rob_bone_data->motion_loaded.begin > (ssize_t)last_index; i--) {
        motion_blend_mot* type = rob_bone_data->motion_loaded.end[-1];
        if ((ssize_t)rob_bone_data->motion_loaded_indices.end[-1]
            >= vector_length(rob_bone_data->motions) && type)
            motion_blend_mot_dispose(type);
        rob_bone_data->motion_loaded.end[-1] = 0;
        vector_ptr_motion_blend_mot_pop_back(&rob_bone_data->motion_loaded, 0);
        vector_size_t_pop_back(&rob_bone_data->motion_loaded_indices, 0);
    }
}

static void rob_chara_bone_data_motion_blend_mot_list_init(rob_chara_bone_data* rob_bone_data) {
    size_t free_index = rob_bone_data->motion_indices.end[-1];
    vector_size_t_pop_back(&rob_bone_data->motion_indices, 0);
    vector_ptr_motion_blend_mot_push_back(&rob_bone_data->motion_loaded,
        &rob_bone_data->motions.begin[free_index]);
    vector_size_t_push_back(&rob_bone_data->motion_loaded_indices, &free_index);
}

static void rob_chara_bone_data_reserve(rob_chara_bone_data* rob_bone_data) {
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
        i->exp_data.parent_scale = vec3_identity;
        i->ex_data_mat = 0;
    }
}

static void rob_chara_bone_data_motion_load(rob_chara_bone_data* rob_bone_data,
    int32_t motion_id, int32_t index, bone_database* bone_data, motion_database* mot_db) {
    if (vector_length(rob_bone_data->motion_loaded) < 1)
        return;

    if (index == 1) {
        rob_chara_bone_data_motion_blend_mot_list_free(rob_bone_data, 1);
        motion_blend_mot_load_file(rob_bone_data->motion_loaded.begin[0], motion_id, bone_data, mot_db);
        return;
    }

    if (index != 2) {
        if (index == 3)
            index = 0;
        rob_chara_bone_data_motion_blend_mot_list_free(rob_bone_data, 1);
        motion_blend_mot_load_file(rob_bone_data->motion_loaded.begin[0], motion_id, bone_data, mot_db);
        return;
    }

    rob_chara_bone_data_motion_blend_mot_list_free(rob_bone_data, 2);
    if (vector_length(rob_bone_data->motion_indices) > 0) {
        rob_chara_bone_data_motion_blend_mot_list_init(rob_bone_data);
        motion_blend_mot_load_file(rob_bone_data->motion_loaded.begin[0], motion_id, bone_data, mot_db);
        return;
    }

    motion_blend_mot* v16 = motion_blend_mot_init();
    motion_blend_mot_load_bone_data(v16, rob_bone_data, bone_data);
    vector_ptr_motion_blend_mot_push_back(&rob_bone_data->motion_loaded, &v16);
    *vector_size_t_reserve_back(&rob_bone_data->motion_loaded_indices) = vector_length(rob_bone_data->motions);
    motion_blend_mot_load_file(rob_bone_data->motion_loaded.begin[0], motion_id, bone_data, mot_db);
}

static void rob_chara_bone_data_reset(rob_chara_bone_data* rob_bone_data) {
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

    if (vector_length(rob_bone_data->motions) == 3) {
        rob_chara_bone_data_motion_blend_mot_list_free(rob_bone_data, 0);
        for (motion_blend_mot** i = rob_bone_data->motions.begin; i != rob_bone_data->motions.end; i++)
            motion_blend_mot_reset(*i);
        rob_chara_bone_data_motion_blend_mot_list_init(rob_bone_data);
    }
    else {
        rob_chara_bone_data_motion_blend_mot_free(rob_bone_data);
        rob_chara_bone_data_motion_blend_mot_init(rob_bone_data);
    }
}

static void sub_140414BC0(struc_346* a1, float_t frame) {
    float_t v2 = a1->field_14;
    a1->frame = frame;
    a1->field_28 = false;
    if (v2 >= 0.0f && (a1->field_18 == 2 && frame <= v2) || frame >= v2)
        a1->field_14 = -1.0;
}

static void rob_chara_bone_data_set_frame(rob_chara_bone_data* rob_bone_data, float_t frame) {
    sub_140414BC0(&rob_bone_data->motion_loaded.begin[0]->field_4A8.field_0, frame);
}

static void rob_chara_bone_data_set_mats(rob_chara_bone_data* rob_bone_data,
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

static void rob_chara_bone_data_set_parent_mats(rob_chara_bone_data* rob_bone_data,
    uint16_t* parent_indices) {
    if (vector_length(rob_bone_data->nodes) < 1)
        return;

    parent_indices++;
    for (bone_node* i = &rob_bone_data->nodes.begin[1]; i != rob_bone_data->nodes.end; i++)
        i->parent = &rob_bone_data->nodes.begin[*parent_indices++];
}

static void rob_chara_bone_data_set_rotation_y(rob_chara_bone_data* rob_bone_data, float_t rotation_y) {
    rob_bone_data->motion_loaded.begin[0]->bone_data.rotation_y = rotation_y;
}

static void rob_chara_bone_data_update(rob_chara_bone_data* rob_bone_data, mat4* mat) {
    if (vector_length(rob_bone_data->motion_loaded_indices) < 1)
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

static void rob_chara_bone_data_dispose(rob_chara_bone_data* rob_bone_data) {
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

static rob_chara_item_equip* rob_chara_item_equip_init() {
    rob_chara_item_equip* rob_item_equip = force_malloc(sizeof(rob_chara_item_equip));
    rob_chara_item_equip_object* itm_eq_obj = force_malloc_s(rob_chara_item_equip_object, 31);
    for (int32_t i = 0; i < 31; i++)
        rob_chara_item_equip_object_init(&itm_eq_obj[i]);
    rob_item_equip->item_equip_object = itm_eq_obj;
    rob_item_equip->wind_strength = 1.0f;
    rob_item_equip->chara_color = 1;
    for (int32_t i = 0; i < 31; i++)
        rob_item_equip->item_equip_object[i].item_equip = rob_item_equip;
    rob_chara_item_equip_reset(rob_item_equip);
    return rob_item_equip;
}

static float_t sub_140512F60(rob_chara_item_equip* rob_item_equip) {
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

static void rob_chara_item_equip_draw(
    rob_chara_item_equip* rob_item_equip, int32_t chara_id, render_context* rctx) {
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
            rob_chara_item_equip_object_draw(&rob_item_equip->item_equip_object[i], &mat, rctx);
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
            rob_chara_item_equip_object_draw(&rob_item_equip->item_equip_object[i], &mat, rctx);
        }
    }
    object_data_set_texture_color_coeff(object_data, &v23);
    object_data_set_wet_param(object_data, 0.0f);
    object_data_set_chara_color(object_data, 0);
    object_data_set_draw_task_flags(object_data, 0);
    object_data_set_shadow_type(object_data, SHADOW_CHARA);
}

static void rob_chara_item_equip_get_parent_bone_nodes(
    rob_chara_item_equip* rob_item_equip, bone_node* bone_nodes, bone_database* bone_data) {
    rob_item_equip->bone_nodes = bone_nodes;
    rob_item_equip->matrices = bone_nodes->mat;
    for (int32_t i = rob_item_equip->first_item_equip_object;
        i < rob_item_equip->max_item_equip_object; i++)
        rob_chara_item_equip_object_get_parent_bone_nodes(
            &rob_item_equip->item_equip_object[i], rob_item_equip->bone_nodes, bone_data);
}

static void rob_chara_item_equip_load_object_info(rob_chara_item_equip* rob_item_equip,
    object_info object_info, item_id id, bone_database* bone_data, void* data, object_database* obj_db) {
    if (id < ITEM_BODY || id > ITEM_ITEM16 || !rob_item_equip->bone_nodes)
        return;

    rob_chara_item_equip_object_load_object_info_ex_data(
        &rob_item_equip->item_equip_object[id], object_info,
        rob_item_equip->bone_nodes, bone_data, data, obj_db);
    rob_item_equip->item_equip_object[id].draw = true;
}

static void rob_chara_item_equip_object_init(rob_chara_item_equip_object* itm_eq_obj) {
    memset(itm_eq_obj, 0, sizeof(rob_chara_item_equip_object));
    bone_node_expression_data_reset_position_rotation(&itm_eq_obj->exp_data);
    rob_chara_item_equip_object_init_data(itm_eq_obj, 0x12345678);
}

static void rob_chara_item_equip_object_clear_ex_data(rob_chara_item_equip_object* itm_eq_obj) {
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

static void rob_chara_item_equip_object_draw(
    rob_chara_item_equip_object* itm_eq_obj, mat4* mat, render_context* rctx) {
    if (itm_eq_obj->obj_info.id == -1 && itm_eq_obj->obj_info.set_id == -1)
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
            itm_eq_obj->obj_info,
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

static int32_t rob_chara_item_equip_object_get_bone_index(
    rob_chara_item_equip_object* itm_eq_obj, char* name, bone_database* bone_data) {
    int32_t bone_index = bone_database_get_skeleton_motion_bone_index(bone_data,
        bone_database_skeleton_type_to_string(BONE_DATABASE_SKELETON_COMMON), name);
    if (bone_index == -1)
        for (ex_data_name_bone_index* i = itm_eq_obj->ex_bones.begin; i != itm_eq_obj->ex_bones.end; i++)
            if (!str_utils_compare(name, i->name))
                return 0x8000 | i->bone_index;
    return bone_index;
}

static bone_node* rob_chara_item_equip_object_get_bone_node(
    rob_chara_item_equip_object* itm_eq_obj, int32_t bone_index) {
    if (!(bone_index & 0x8000))
        return &itm_eq_obj->bone_nodes[bone_index & 0x7FFF];
    else if ((bone_index & 0x7FFF) < vector_length(itm_eq_obj->ex_data_bone_nodes))
        return &itm_eq_obj->ex_data_bone_nodes.begin[bone_index & 0x7FFF];
    return 0;
}

static bone_node* rob_chara_item_equip_object_get_bone_node_by_name(
    rob_chara_item_equip_object* itm_eq_obj, char* name, bone_database* bone_data) {
    return rob_chara_item_equip_object_get_bone_node(itm_eq_obj,
        rob_chara_item_equip_object_get_bone_index(itm_eq_obj, name, bone_data));
}

static void rob_chara_item_equip_object_get_parent_bone_nodes(
    rob_chara_item_equip_object* itm_eq_obj, bone_node* bone_nodes, bone_database* bone_data) {

    itm_eq_obj->bone_nodes = bone_nodes;
    itm_eq_obj->mat = bone_nodes->mat;
    for (ex_node_block** i = itm_eq_obj->node_blocks.begin; i != itm_eq_obj->node_blocks.end; i++)
        (*i)->parent_bone_node = rob_chara_item_equip_object_get_bone_node_by_name(
            itm_eq_obj, string_data(&(*i)->parent_name), bone_data);
}

static void rob_chara_item_equip_object_init_data(rob_chara_item_equip_object* itm_eq_obj, size_t index) {
    itm_eq_obj->index = index;
    itm_eq_obj->obj_info = object_info_null;
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
    rob_chara_item_equip_object_clear_ex_data(itm_eq_obj);
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

static void rob_chara_item_equip_object_init_ex_data_bone_nodes(
    rob_chara_item_equip_object* itm_eq_obj, object_skin_ex_data* a2) {
    itm_eq_obj->ex_data_bone_nodes.end = itm_eq_obj->ex_data_bone_nodes.begin;
    itm_eq_obj->field_F0.end = itm_eq_obj->field_F0.begin;

    int32_t count = a2->bone_names_count;
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
        memset(v14->begin, 0, sizeof(ex_data_name_bone_index) * vector_length(*v14));
        for (ex_data_name_bone_index* i = v14->begin; i != v14->end; i++)
            i->name = 0;
        v14->end = v14->begin;

        for (int32_t i = 0; i < count; i++) {
            bone_node* v38 = &v4[i];
            v38->name = a2->bone_names[i];
            v38->mat = &v5[i];
            *v38->mat = mat4_identity;
            bone_node_expression_data_reset_position_rotation(&v38->exp_data);
            v38->ex_data_mat = &v8[i];
            *v38->ex_data_mat = mat4_identity;

            ex_data_name_bone_index* v35 = vector_ex_data_name_bone_index_reserve_back(v14);
            v35->name = a2->bone_names[i];
            v35->bone_index = i;
        }
    }
}

static void rob_chara_item_equip_object_load_ex_data(rob_chara_item_equip_object* itm_eq_obj,
    object_skin_ex_data* ex_data, bone_database* bone_data, void* data, object_database* obj_db) {
    if (!ex_data->blocks)
        return;

    vector_pair_uint32_t_ptr_rob_osage_node a4
        = vector_empty(pair_uint32_t_ptr_rob_osage_node);
    vector_pair_name_ex_osage_block a5 = vector_empty(pair_name_ex_osage_block);
    rob_chara_item_equip_object_clear_ex_data(itm_eq_obj);

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
                itm_eq_obj->obj_info, itm_eq_obj->index, bone_data);
            expression_count++;
        }
        else if (v4->type == OBJECT_SKIN_BLOCK_OSAGE) {
            if (osage_count >= 0x100)
                continue;

            ex_osage_block* osg = ex_osage_block_init();
            node = &osg->base;
            vector_ptr_ex_osage_block_push_back(&itm_eq_obj->osage_blocks, &osg);
            ex_osage_block_init_data(osg, itm_eq_obj, &v4->osage,
                bone_names_ptr[v4->osage.external_name_index & 0x7FFF],
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
        exp_data.parent_scale = vec3_identity;
        string_copy(&v54->parent_name, &node->parent_name);

        bone_node* parent_bone_node = rob_chara_item_equip_object_get_bone_node_by_name(itm_eq_obj,
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

    for (ex_osage_block** i = itm_eq_obj->osage_blocks.begin;
        i != itm_eq_obj->osage_blocks.end; i++) {
        ex_osage_block* osg = *i;
        ex_node_block* node = &osg->base;
        ex_node_block* parent_node = node->parent_node;
        bone_node* parent_bone_node = 0;
        if (parent_node && node->field_58) {
            while (!parent_node->field_58) {
                parent_node = parent_node->parent_node;
                if (!parent_node)
                    break;
                parent_bone_node = parent_node->parent_bone_node;
            }

        }
        else
            parent_bone_node = node->parent_bone_node;

        if (parent_bone_node && parent_bone_node->ex_data_mat) {
            osg->rob.parent_mat_ptr = parent_bone_node->ex_data_mat;
            mat4_to_mat4u(parent_bone_node->ex_data_mat, &osg->rob.parent_mat);
        }
    }

    if (ex_data->osage_sibling_infos) {
        pair_uint32_t_ptr_rob_osage_node* v6 = a4.begin;
        pair_uint32_t_ptr_rob_osage_node* v7 = a4.end;

        for (int32_t i = 0; i < ex_data->osage_sibling_infos_count; i++) {
            object_skin_osage_sibling_info* sibling_info = &ex_data->osage_sibling_infos[i];
            uint32_t name_index = sibling_info->name_index;
            uint32_t sibling_name_index = sibling_info->sibling_name_index;

            pair_uint32_t_ptr_rob_osage_node* node = v6;
            for (; node != v7; node++)
                if (node->key == name_index)
                    break;

            pair_uint32_t_ptr_rob_osage_node* sibling_node = v6;
            for (; sibling_node != v7; sibling_node++)
                if (sibling_node->key == sibling_name_index)
                    break;

            if (node != v7 && sibling_node != v7) {
                rob_osage_node* v102 = node->value;
                v102->sibling_node = sibling_node->value;
                v102->distance = sibling_info->distance;
            }
        }
    }

    if (osage_count || cloth_count)
        rob_chara_item_equip_object_skp_load_file(itm_eq_obj, data, bone_data, obj_db);

    size_t v103 = 0;
    for (ex_osage_block** i = itm_eq_obj->osage_blocks.begin; i != itm_eq_obj->osage_blocks.end; i++)
        v103 += vector_length((*i)->rob.nodes) - 1;
    //for (ex_cloth_block** index = itm_eq_obj->cloth_blocks.begin; index != itm_eq_obj->cloth_blocks.end; index++)
    //    v103 += vector_length((*index)->rob.base.field_20) - (*index)->rob.base.field_10;
    itm_eq_obj->field_1C0 = v103;

    vector_pair_uint32_t_ptr_rob_osage_node_free(&a4, 0);
    vector_pair_name_ex_osage_block_free(&a5, 0);
}

static void rob_chara_item_equip_object_load_object_info_ex_data(
    rob_chara_item_equip_object* itm_eq_obj, object_info object_info,
    bone_node* bone_nodes, bone_database* bone_data, void* data, object_database* obj_db) {
    itm_eq_obj->obj_info = object_info;
    itm_eq_obj->bone_nodes = bone_nodes;
    itm_eq_obj->mat = bone_nodes->mat;
    itm_eq_obj->ex_data_bone_nodes.end = itm_eq_obj->ex_data_bone_nodes.begin;
    for (ex_data_name_bone_index* i = itm_eq_obj->ex_bones.begin;
        i != itm_eq_obj->ex_bones.end; i++)
        i->name = 0;

    itm_eq_obj->ex_bones.end = itm_eq_obj->ex_bones.begin;
    itm_eq_obj->field_F0.end = itm_eq_obj->field_F0.begin;
    rob_chara_item_equip_object_clear_ex_data(itm_eq_obj);
    object_skin* skin = object_storage_get_object_skin(itm_eq_obj->obj_info);
    if (!skin || !skin->ex_data_init)
        return;

    itm_eq_obj->skin = skin;
    itm_eq_obj->skin_ex_data = &skin->ex_data;
    rob_chara_item_equip_object_init_ex_data_bone_nodes(itm_eq_obj, &skin->ex_data);
    rob_chara_item_equip_object_load_ex_data(itm_eq_obj, &skin->ex_data, bone_data, data, obj_db);
}

static void rob_chara_item_equip_object_reset(rob_chara_item_equip_object* itm_eq_obj) {
    rob_chara_item_equip_object_init_data(itm_eq_obj, 0xDEADBEEF);
}

static bool rob_chara_item_equip_object_set_boc(rob_chara_item_equip_object* itm_eq_obj,
    skin_param_osage_root* skp_root, ex_osage_block* osg) {
    rob_osage* rob_osg = &osg->rob;
    for (rob_osage_node* i = rob_osg->nodes.begin + 1; i != rob_osg->nodes.end; i++)
        i->data_ptr->boc.end = i->data_ptr->boc.begin;

    bool v6 = false;
    for (skin_param_osage_root_boc* i = skp_root->boc.begin; i != skp_root->boc.end; i++) {
        ex_osage_block** v8 = itm_eq_obj->osage_blocks.begin;
        ex_osage_block** v9 = itm_eq_obj->osage_blocks.end;
        if (v8 == v9)
            continue;

        while (v8 != v9) {
            ex_osage_block* v10 = *v8;
            if (str_utils_compare((*v8)->base.name, string_data(&i->ed_root))
                || i->ed_node + 1LL >= vector_length(rob_osg->nodes)
                || i->st_node + 1LL >= vector_length(v10->rob.nodes)) {
                v8++;
                continue;
            }

            rob_osage_node* v17 = rob_osage_get_node(rob_osg, i->ed_node + 1LL);
            rob_osage_node* v18 = rob_osage_get_node(&v10->rob, i->st_node + 1LL);
            vector_ptr_rob_osage_node_push_back(&v17->data_ptr->boc, &v18);
            v6 = true;
            break;
        }
    }
    return v6;
}

static void rob_chara_item_equip_object_set_collision_target_osage(
    rob_chara_item_equip_object* itm_eq_obj, skin_param_osage_root* skp_root, skin_param* skp) {
    if (!skp_root->colli_tgt_osg.length)
        return;

    char* colli_tgt_osg = string_data(&skp_root->colli_tgt_osg);
    for (ex_osage_block** i = itm_eq_obj->osage_blocks.begin; i != itm_eq_obj->osage_blocks.end; ++i)
        if (!str_utils_compare(colli_tgt_osg, (*i)->base.name)) {
            skp->colli_tgt_osg = &(*i)->rob.nodes;
            break;
        }
}

static void rob_chara_item_equip_object_skp_load(
    rob_chara_item_equip_object* itm_eq_obj, key_val* kv, bone_database* bone_data) {
    if (vector_length(kv->key_hash) < 1)
        return;

    itm_eq_obj->field_1B8 = 0;
    for (ex_osage_block** i = itm_eq_obj->osage_blocks.begin;
        i != itm_eq_obj->osage_blocks.end; i++) {
        ex_osage_block* osg = *i;
        skin_param_osage_root root;
        memset(&root, 0, sizeof(skin_param_osage_root));
        skin_param_osage_root_init(&root);
        rob_osage_load_skin_param(&osg->rob, kv, osg->base.name,
            &root, &itm_eq_obj->obj_info, bone_data);
        rob_chara_item_equip_object_set_collision_target_osage(itm_eq_obj, &root, osg->rob.skin_param_ptr);
        itm_eq_obj->field_1B8 |= rob_chara_item_equip_object_set_boc(itm_eq_obj, &root, *i);
        itm_eq_obj->field_1B8 |= root.coli_type != 0;
        //itm_eq_obj->field_1B8 |= sub_1405FAA30(itm_eq_obj, &root, 0);
        skin_param_osage_root_free(&root);
    }
    for (ex_cloth_block** i = itm_eq_obj->cloth_blocks.begin;
        i != itm_eq_obj->cloth_blocks.end; i++) {
        ex_cloth_block* cls = *i;
        skin_param_osage_root root;
        memset(&root, 0, sizeof(skin_param_osage_root));
        skin_param_osage_root_init(&root);
        skin_param_osage_root_parse(kv, cls->base.name, &root, bone_data);
        //rob_cloth_load_skin_param(&cls->rob, &root);
        skin_param_osage_root_free(&root);
    }
}

static void rob_chara_item_equip_object_skp_load_file(
    rob_chara_item_equip_object* itm_eq_obj, void* data,
    bone_database* bone_data, object_database* obj_db) {
    key_val* kv_ptr = 0;//sub_14060B4B0(itm_eq_obj->object_info);
    if (kv_ptr) {
        rob_chara_item_equip_object_skp_load(itm_eq_obj, kv_ptr, bone_data);
        return;
    }

    char buf[0x200];
    char* name = object_database_get_object_name(obj_db, itm_eq_obj->obj_info);
    sprintf_s(buf, sizeof(buf), "ext_skp_%s.txt", name);

    for (int32_t i = 0; buf[i]; i++)
        if (buf[i] > 0x40 && buf[i] < 0x5B)
            buf[i] += 0x20;

    key_val kv;
    data_struct_load_file(data, &kv, "rom/skin_param/", buf, skin_param_file_read);
    rob_chara_item_equip_object_skp_load(itm_eq_obj, &kv, bone_data);
    key_val_free(&kv);
}

static void rob_chara_item_equip_object_free(rob_chara_item_equip_object* itm_eq_obj) {
    rob_chara_item_equip_object_init_data(itm_eq_obj, 0xDDDDDDDD);

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

static void rob_chara_item_equip_reset(rob_chara_item_equip* rob_item_equip) {
    rob_item_equip->bone_nodes = 0;
    rob_item_equip->matrices = 0;
    for (int32_t i = 0; i < 31; i++)
        rob_chara_item_equip_object_reset(&rob_item_equip->item_equip_object[i]);
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
    rob_item_equip->step = 0.0f;
    rob_item_equip->field_93C = false;
    rob_item_equip->first_item_equip_object = 1;
    rob_item_equip->max_item_equip_object = 30;
}

static void rob_chara_item_equip_reset_init_data(rob_chara_item_equip* rob_item_equip,
    bone_node* bone_nodes) {
    rob_chara_item_equip_reset(rob_item_equip);
    rob_item_equip->bone_nodes = bone_nodes;
    rob_item_equip->matrices = bone_nodes->mat;

    mat4u* v7 = rob_item_equip->field_13C;
    int32_t* v8 = rob_item_equip->field_18;
    for (item_id v6 = ITEM_BODY; v6 < ITEM_MAX; v6++) {
        rob_chara_item_equip_object_init_data(&rob_item_equip->item_equip_object[v6], v6);
        *v8++ = 0;
        *v7++ = mat4u_identity;
    }

    rob_item_equip->step = 1.0f;
    rob_item_equip->wet = 0.0f;
    rob_item_equip->wind_strength = 1.0f;
    rob_item_equip->chara_color = true;
    rob_item_equip->npr_flag = false;
    rob_item_equip->mat = mat4u_identity;
}

static void rob_chara_item_equip_set_item_equip_range(rob_chara_item_equip* rob_item_equip, bool value) {
    rob_item_equip->item_equip_range = value;
    if (value) {
        rob_item_equip->first_item_equip_object = ITEM_BODY;
        rob_item_equip->max_item_equip_object = ITEM_ATAMA;
    }
    else {
        rob_item_equip->first_item_equip_object = ITEM_ATAMA;
        rob_item_equip->max_item_equip_object = ITEM_ITEM16;
    }
}

static void rob_chara_item_equip_set_shadow_type(rob_chara_item_equip* rob_item_equip, int32_t chara_id) {
    if (chara_id == 0)
        rob_item_equip->shadow_type = SHADOW_CHARA;
    else
        rob_item_equip->shadow_type = SHADOW_STAGE;
}

static void rob_chara_item_equip_dispose(rob_chara_item_equip* rob_item_equip) {
    rob_chara_item_equip_reset(rob_item_equip);
    rob_chara_item_equip_object* itm_eq_obj = rob_item_equip->item_equip_object;
    for (int32_t i = 0; i < 31; i++)
        rob_chara_item_equip_object_free(&itm_eq_obj[i]);
    free(itm_eq_obj);

    if (rob_item_equip->texture_pattern.begin)
        free(rob_item_equip->texture_pattern.begin);
    rob_item_equip->texture_pattern = vector_empty(texture_pattern_struct);
    free(rob_item_equip);
}

static void rob_chara_item_sub_data_init(rob_chara_item_sub_data* rob_item_sub_data) {
    rob_item_sub_data->texture_change = vector_empty(item_sub_data_texture_change);
    rob_item_sub_data->item_change = vector_empty(item_sub_data_item_change);
    rob_item_sub_data->field_F0 = vector_empty(pair_object_info_item_id);
    rob_item_sub_data->field_100 = vector_empty(struc_294);
    for (int32_t i = 0; i < 31; i++)
        rob_item_sub_data->texture_pattern[i] = vector_empty(texture_pattern_struct);
    rob_item_sub_data->field_3F8 = vector_empty(pair_int32_t_object_info);
    rob_item_sub_data->chara_index = 0;
    rob_item_sub_data->chara_index_2nd = 0;
    memset(&rob_item_sub_data->item, 0, sizeof(item_sub_data));
    memset(&rob_item_sub_data->item_2nd, 0, sizeof(item_sub_data));
    rob_chara_item_sub_data_texture_pattern_clear(rob_item_sub_data);
}

static bool rob_chara_item_sub_data_check_for_npr_flag(rob_chara_item_sub_data* item_sub_data) {
    int32_t* data = item_sub_data->item.data;
    for (item_sub_id i = ITEM_SUB_ZUJO; i < ITEM_SUB_MAX; i++, data++) {
        //v5 = item_table_get_item_data_by_chara_index(item_sub_data->chara_index, *data);
        //if (v5 && v5->npr_flag)
        //    return true;
    }
    return false;
}

static void rob_chara_item_sub_data_item_change_clear(
    rob_chara_item_sub_data* rob_item_sub_data) {
    vector_item_sub_data_item_change_clear(
        &rob_item_sub_data->item_change, item_sub_data_item_change_free);
}

static void rob_chara_item_sub_data_reload_items(rob_chara_item_sub_data* itm_sub_data,
    int32_t chara_id, bone_database* bone_data, void* data, object_database* obj_db) {
    item_sub_data sub_data;
    memset(&sub_data, 0, sizeof(item_sub_data));
    sub_data.kami = 500;
    sub_data.outer = 1;
    sub_data.kata = 301;
    //sub_data.kami = 681;
    //sub_data.outer = 181;
    //sub_data.kata = 481;
    //sub_data.head = 981;
    rob_chara_reload_items(&rob_chara_array[chara_id],
        &sub_data, bone_data, data, obj_db);

    /*rob_chara_item_equip* rob_item_equip = rob_chara_array[chara_id].item_equip;
    sub_140522990(itm_sub_data, item_sub_data->item);
    rob_chara_item_sub_data_load_chara_init_data(itm_sub_data, rob_item_equip);
    sub_140522C60(itm_sub_data, rob_item_equip, item_sub_data->item);
    sub_140522D00(itm_sub_data, rob_item_equip, item_sub_data->item);
    sub_140522D90(itm_sub_data, rob_item_equip, item_sub_data->item);
    sub_140522F90(itm_sub_data, rob_item_equip, item_sub_data->item);
    sub_1405231D0(itm_sub_data, rob_item_equip, item_sub_data->item);
    sub_1405234E0(itm_sub_data);*/
}

static void rob_chara_item_sub_data_texture_change_clear(
    rob_chara_item_sub_data* rob_item_sub_data) {
    vector_item_sub_data_texture_change_clear(
        &rob_item_sub_data->texture_change, item_sub_data_texture_change_free);
}

static void rob_chara_item_sub_data_texture_pattern_clear(
    rob_chara_item_sub_data* rob_item_sub_data) {
    for (int32_t i = 0; i < 31; i++)
        vector_texture_pattern_struct_clear(&rob_item_sub_data->texture_pattern[i], 0);
}

static void rob_chara_item_sub_data_free(
    rob_chara_item_sub_data* rob_item_sub_data) {
    rob_chara_item_sub_data_texture_change_clear(rob_item_sub_data);
    rob_chara_item_sub_data_item_change_clear(rob_item_sub_data);
    rob_chara_item_sub_data_texture_pattern_clear(rob_item_sub_data);
    vector_pair_object_info_item_id_free(&rob_item_sub_data->field_F0, 0);
    vector_struc_294_free(&rob_item_sub_data->field_100, 0);
    vector_pair_int32_t_object_info_free(&rob_item_sub_data->field_3F8, 0);
    for (int32_t i = 0; i < 31; i++)
        vector_texture_pattern_struct_free(&rob_item_sub_data->texture_pattern[i], 0);
    vector_item_sub_data_item_change_free(
        &rob_item_sub_data->item_change, item_sub_data_item_change_free);
    vector_item_sub_data_texture_change_free(
        &rob_item_sub_data->texture_change, item_sub_data_texture_change_free);
}

static void rob_chara_data_init(rob_chara_data* rob_mot_blend) {
    memset(rob_mot_blend, 0, sizeof(rob_chara_data));
    rob_sub_action_init(&rob_mot_blend->rob_sub_action);
    //sub_140514FC0(&rob_mot_blend->field_290);
    //sub_1405387F0(&rob_mot_blend->field_1588);
    //sub_140514BC0(&rob_mot_blend->field_1E68);
}

static void rob_chara_data_free(rob_chara_data* rob_mot_blend) {
    rob_sub_action_free(&rob_mot_blend->rob_sub_action);
    vector_ptr_void_free(&rob_mot_blend->field_8.field_110, 0);
}

static bool rob_chara_check_for_ageageagain_module(chara_index chara_index, int32_t module_index) {
    return chara_index == CHARA_MIKU && module_index == 148;
}

static void rob_chara_load_default_motion(rob_chara* rob_chr,
    bone_database* bone_data, motion_database* mot_db) {
    int32_t motion_id = 195; //rob_cmn_mottbl_get_motion_index(rob_chr, 0);
    rob_chara_load_default_motion_sub(rob_chr, 1, motion_id, bone_data, mot_db);
}

static void sub_14041C680(rob_chara_bone_data* rob_bone_data, bool a2) {
    struc_308* v2 = &rob_bone_data->motion_loaded.begin[0]->field_4F8;
    if (a2)
        v2->field_0 |= 2;
    else
        v2->field_0 &= ~2;
}

static void sub_14041C9D0(rob_chara_bone_data* rob_bone_data, bool a2) {
    struc_308* v2 = &rob_bone_data->motion_loaded.begin[0]->field_4F8;
    v2->field_8 = (uint8_t)(v2->field_0 & 1);
    if (a2)
        v2->field_0 |= 1;
    else
        v2->field_0 &= ~1;
}

static void sub_14041D2D0(rob_chara_bone_data* rob_bone_data, bool a2) {
    struc_308* v2 = &rob_bone_data->motion_loaded.begin[0]->field_4F8;
    if (a2)
        v2->field_0 |= 4;
    else
        v2->field_0 &= ~4;
}

static void sub_14041BC40(rob_chara_bone_data* rob_bone_data, bool a2) {
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

static void sub_14041D270(rob_chara_bone_data* rob_bone_data, float_t a2) {
    sub_140414F00(&rob_bone_data->motion_loaded.begin[0]->field_4F8, a2);
}

static void sub_14041D2A0(rob_chara_bone_data* rob_bone_data, float_t a2) {
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
    motion_blend* parent_node = hinge->field_5D0;
    if (parent_node)
        sub_1403FBED0(parent_node, v47, hinge, init_rot);
}

static void sub_14041BF80(rob_chara_bone_data* rob_bone_data, float_t v47, float_t hinge, float_t init_rot) {
    sub_140414C60(rob_bone_data->motion_loaded.begin[0], v47, hinge, init_rot);
}

static void sub_14041D310(rob_chara_bone_data* rob_bone_data) {
    motion_blend* node = rob_bone_data->motion_loaded.begin[0]->field_5D0;
    if (node)
        node->vftable->field_10(node);
}

static void sub_140419820(rob_chara_bone_data* rob_bone_data, int32_t skeleton_select) {
    sub_140412E10(rob_bone_data->motion_loaded.begin[0], skeleton_select);
}*/

static void rob_chara_load_default_motion_sub(rob_chara* rob_chr, int32_t skeleton_select,
    int32_t motion_id, bone_database* bone_data, motion_database* mot_db) {
    /*sub_14041BE50(rob_chr->bone_data, -1);
    sub_14041BFC0(rob_chr->bone_data, -1);
    sub_14041C260(rob_chr->bone_data, -1);
    sub_14041D200(rob_chr->bone_data, -1);
    sub_14041BDB0(rob_chr->bone_data, -1);
    sub_14041BD20(rob_chr->bone_data, -1);*/
    rob_chara_bone_data_motion_load(rob_chr->bone_data, motion_id, 1, bone_data, mot_db);
    rob_chara_bone_data_set_frame(rob_chr->bone_data, 0.0f);
    sub_14041C680(rob_chr->bone_data, 0);
    sub_14041C9D0(rob_chr->bone_data, 0);
    sub_14041D2D0(rob_chr->bone_data, 0);
    sub_14041BC40(rob_chr->bone_data, 0);
    sub_14041D270(rob_chr->bone_data, 0.0f);
    sub_14041D2A0(rob_chr->bone_data, 0.0f);
    rob_chara_bone_data_set_rotation_y(rob_chr->bone_data, 0.0f);
    //sub_14041BF80(rob_chr->bone_data, 0.0f, 1.0f, 1.0f);
    //sub_14041D310(rob_chr->bone_data);
    rob_chara_bone_data_interpolate(rob_chr->bone_data);
    rob_chara_bone_data_update(rob_chr->bone_data, 0);
    sub_140509D30(rob_chr);
    //sub_140419820(rob_chr->bone_data, skeleton_select);
}

static void rob_chara_set_draw(rob_chara* rob_chr, item_id id, bool draw) {
    if (id < ITEM_BODY)
        return;
    else if (id < ITEM_ITEM16)
        rob_chr->item_equip->item_equip_object[id].draw = draw;
    else if (id == ITEM_MAX)
        for (int32_t i = ITEM_ATAMA; i <= ITEM_ITEM16; i++) {
            rob_chr->item_equip->item_equip_object[i].draw = draw;
            /*if (index == ITEM_ATAMA && rob_chara_check_for_ageageagain_module(
                rob_chr->chara_index, rob_chr->module_index)) {
                sub_140543780(rob_chr->chara_id, 1, draw);
                sub_140543780(rob_chr->chara_id, 2, draw);
                sub_1405430F0(rob_chr->chara_id, 1);
                sub_1405430F0(rob_chr->chara_id, 2);
            }*/
        }
}

static rob_osage_node* rob_osage_get_node(rob_osage* rob_osg,
    size_t index) {
    if ((ssize_t)index < vector_length(rob_osg->nodes))
        return &rob_osg->nodes.begin[index];
    else
        return &rob_osg->node;
}

static void rob_osage_load_skin_param(rob_osage* rob_osg, key_val* kv, char* name,
    skin_param_osage_root* skp_root, object_info* obj_info, bone_database* bone_data) {
    rob_osg->skin_param_ptr = &rob_osg->skin_param;
    for (rob_osage_node* i = rob_osg->nodes.begin; i != rob_osg->nodes.end; i++)
        i->data_ptr = &i->data;

    skin_param_osage_root_parse(kv, name, skp_root, bone_data);
    rob_osage_set_skin_param_osage_root(rob_osg, skp_root);

    size_t v11 = 0;//sub_14060ADC0(obj_info, &Memory);;
    if (v11) {
        rob_osg->field_290 = *(uint32_t*)v11;
        rob_osg->field_298 = *(size_t*)(v11 + 8);
    }

    vector_skin_param_osage_node vec = vector_empty(skin_param_osage_node);
    size_t count = vector_length(rob_osg->nodes) - 1;
    vector_skin_param_osage_node_reserve(&vec, count);
    vec.end += count;
    for (skin_param_osage_node* i = vec.begin; i != vec.end; i++) {
        i->coli_r = 0.0f;
        i->weight = 1.0f;
        i->inertial_cancel = 0.0f;
        i->hinge.ymin = -90.0f;
        i->hinge.ymax = 90.0f;
        i->hinge.zmin = -90.0f;
        i->hinge.zmax = 90.0f;
    }
    skin_param_osage_node_parse(kv, name, &vec, skp_root);
    rob_osage_set_skp_osg_nodes(rob_osg, &vec);
    vector_skin_param_osage_node_free(&vec, 0);
}

static void rob_osage_node_init(rob_osage_node* node) {
    node->length = 0.0f;
    node->trans = vec3_null;
    node->trans_orig = vec3_null;
    node->trans_diff = vec3_null;
    node->field_28 = vec3_null;
    node->child_length = 0.0f;
    node->bone_node = 0;
    node->bone_node_mat = 0;
    node->sibling_node = 0;
    node->distance = 0.0f;
    node->field_94 = vec3_null;
    node->field_A0.trans = vec3_null;
    node->field_A0.trans_diff = vec3_null;
    node->field_A0.rotation = vec3_null;
    node->field_A0.field_24 = 0.0f;
    node->field_C8 = 0.0f;
    node->field_CC = 1.0f;
    node->field_D0 = vec3_null;
    node->force = 1.0f;
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

static void rob_osage_set_air_res(rob_osage* rob_osg, float_t air_res) {
    rob_osg->skin_param_ptr->air_res = air_res;
}

static void rob_osage_coli_set(rob_osage* rob_osg, mat4* mats) {
    if (rob_osg->skin_param_ptr->coli.begin != rob_osg->skin_param_ptr->coli.end)
        osage_coli_set(rob_osg->coli, rob_osg->skin_param_ptr->coli.begin, mats);
    osage_coli_ring_set(rob_osg->coli_ring, &rob_osg->ring.skp_root_coli, mats);
}

static void rob_osage_set_coli_r(rob_osage* rob_osg, float_t coli_r) {
    for (rob_osage_node* i = rob_osg->nodes.begin + 1; i != rob_osg->nodes.end; i++)
        i->data_ptr->skp_osg_node.coli_r = coli_r;
}

static void rob_osage_set_force(rob_osage* rob_osg, float_t force, float_t force_gain) {
    rob_osg->skin_param_ptr->force = force;
    rob_osg->skin_param_ptr->force_gain = force_gain;
    for (rob_osage_node* i = rob_osg->nodes.begin + 1; i != rob_osg->nodes.end; i++) {
        i->data_ptr->force = force;
        force = force * force_gain;
    }
}

static void rob_osage_set_hinge(rob_osage* rob_osg, float_t hinge_y, float_t hinge_z) {
    hinge_y = min(hinge_y, 179.0f);
    hinge_z = min(hinge_z, 179.0f);
    hinge_y = hinge_y * DEG_TO_RAD_FLOAT;
    hinge_z = hinge_z * DEG_TO_RAD_FLOAT;
    for (rob_osage_node* i = rob_osg->nodes.begin + 1; i != rob_osg->nodes.end; i++) {
        rob_osage_node_data* data = i->data_ptr;
        data->skp_osg_node.hinge.ymin = -hinge_y;
        data->skp_osg_node.hinge.ymax = hinge_y;
        data->skp_osg_node.hinge.zmin = -hinge_z;
        data->skp_osg_node.hinge.zmax = hinge_z;
    }
}

static void rob_osage_set_init_rot(rob_osage* rob_osg, float_t init_rot_y, float_t init_rot_z) {
    rob_osg->skin_param_ptr->init_rot.y = init_rot_y * DEG_TO_RAD_FLOAT;
    rob_osg->skin_param_ptr->init_rot.z = init_rot_z * DEG_TO_RAD_FLOAT;
}

static void rob_osage_set_rot(rob_osage* rob_osg, float_t rotation_y, float_t rotation_z) {
    rob_osg->skin_param_ptr->rot.y = rotation_y * DEG_TO_RAD_FLOAT;
    rob_osg->skin_param_ptr->rot.z = rotation_z * DEG_TO_RAD_FLOAT;
}

static void rob_osage_set_skin_param_osage_root(rob_osage* rob_osg, skin_param_osage_root* skp_root) {
    skin_param_init(rob_osg->skin_param_ptr);
    rob_osage_set_force(rob_osg, skp_root->force, skp_root->force_gain);
    rob_osage_set_air_res(rob_osg, skp_root->air_res);
    rob_osage_set_rot(rob_osg, skp_root->rot_y, skp_root->rot_z);
    rob_osage_set_init_rot(rob_osg, skp_root->init_rot_y, skp_root->init_rot_z);
    rob_osg->field_1EB8 = 0;//skp_root->field_54;
    rob_osg->skin_param_ptr->friction = skp_root->friction;
    rob_osg->skin_param_ptr->wind_afc = skp_root->wind_afc;
    rob_osage_set_coli_r(rob_osg, skp_root->coli_r);
    rob_osage_set_hinge(rob_osg, skp_root->hinge_y, skp_root->hinge_z);
    vector_skin_param_osage_root_coli_insert_range(&rob_osg->skin_param_ptr->coli,
        0, skp_root->coli.begin, skp_root->coli.end);
    rob_osg->skin_param_ptr->coli_type = skp_root->coli_type;
    rob_osg->skin_param_ptr->stiffness = skp_root->stiffness;
    rob_osg->skin_param_ptr->move_cancel = skp_root->move_cancel;
}

static void rob_osage_set_skp_osg_nodes(rob_osage* rob_osg,
    vector_skin_param_osage_node* skp_osg_nodes) {
    if (!skp_osg_nodes)
        return;

    if (vector_length(rob_osg->nodes) - 1 != vector_length(*skp_osg_nodes))
        return;

    rob_osage_node* i = rob_osg->nodes.begin + 1;
    skin_param_osage_node* j = skp_osg_nodes->begin;
    while (i != rob_osg->nodes.end && j != skp_osg_nodes->end) {
        i->data_ptr->skp_osg_node = *j++;
        skin_param_hinge_limit(&i->data_ptr->skp_osg_node.hinge);
        i++;
    }
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

static void rob_sub_action_init(rob_sub_action* sub_act) {
    sub_act->data.field_0 = 0;
    sub_act->data.field_8 = 0;
    sub_act->data.field_10 = 0;
    sub_act->data.field_18 = 0;
    //rob_sub_action_execute_cry_init(&sub_act->data.cry);
    //rob_sub_action_execute_shake_hand_init(&sub_act->data.shake_hand);
    //rob_sub_action_execute_embarrassed_init(&sub_act->data.embarrassed);
    //rob_sub_action_execute_angry_init(&sub_act->data.angry);
    //rob_sub_action_execute_laugh_init(&sub_act->data.laugh);
    //rob_sub_action_execute_count_num_init(&sub_act->data.count_num);
    rob_sub_action_init_sub(sub_act);
}

static void rob_sub_action_init_sub(rob_sub_action* sub_act) {
    sub_act->data.field_0 = 0;
    sub_act->data.field_8 = 0;
    sub_act->data.field_10 = 0;
    sub_act->data.field_18 = 0;
    //sub_act->data.cry.base.vftable->field_8(&sub_act->data.cry.base);
    //sub_act->data.shake_hand.base.vftable->field_8(&sub_act->data.shake_hand.base);
    //sub_act->data.embarrassed.base.vftable->field_8(&sub_act->data.embarrassed.base);
    //sub_act->data.angry.base.vftable->field_8(&sub_act->data.angry.base);
    //sub_act->data.laugh.base.vftable->field_8(&sub_act->data.laugh.base);
}

static void rob_sub_action_free(rob_sub_action* sub_act) {
    sub_act->data.field_0 = 0;
    sub_act->data.field_8 = 0;
    sub_act->data.field_10 = 0;
    sub_act->data.field_18 = 0;
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

static bool skin_param_file_read(void* data, char* path, char* file, uint32_t hash) {
    string s;
    string_init(&s, path);
    string_add(&s, file);

    key_val* kv = data;
    key_val_file_read(kv, string_data(&s));

    string_free(&s);
    return kv->buf ? true : false;
}

static void skin_param_osage_node_parse(key_val* kv, char* name,
    vector_skin_param_osage_node* vec, skin_param_osage_root* skp_root) {
    char buf[SKP_TEXT_BUF_SIZE];
    int32_t count;
    size_t len;
    size_t len1;
    size_t len2;
    size_t off;

    key_val lkv;
    if (!key_val_get_local_key_val(kv, name, &lkv))
        return;

    len = utf8_length(name);
    memcpy(buf, name, len);
    off = len;

    len1 = 5;
    memcpy(&buf[len], ".node", 5);
    off = len + len1;

    buf[off] = 0;
    key_val sub_local_key_val;
    if (key_val_get_local_key_val(&lkv, buf, &sub_local_key_val)) {
        if (key_val_read_int32_t(&lkv, buf, off, ".length", 8, &count)) {
            size_t c = vector_length(*vec);
            if (c < count) {
                vector_skin_param_osage_node_reserve(vec, count - c);
                vec->end += count - c;
            }
        }
        else
            vector_skin_param_osage_node_clear(vec, 0);

        int32_t j = 0;
        for (skin_param_osage_node* i = vec->begin; i != vec->end; i++, j++) {
            memset(i, 0, sizeof(skin_param_osage_node));
            len2 = sprintf_s(buf + len + len1,
                SKP_TEXT_BUF_SIZE - len - len1, ".%d", j);
            off = len + len1 + len2;

            i->coli_r = 0.0f;
            i->weight = 1.0f;
            i->inertial_cancel = 0.0f;
            i->hinge.ymin = -90.0f;
            i->hinge.ymax = 90.0f;
            i->hinge.zmin = -90.0f;
            i->hinge.zmax = 90.0f;

            float_t coli_r = 0;
            if (key_val_read_float_t(&sub_local_key_val,
                buf, off, ".coli_r", 8, &coli_r))
                i->coli_r = coli_r;

            float_t weight = 0;
            if (key_val_read_float_t(&sub_local_key_val,
                buf, off, ".weight", 8, &weight))
                i->weight = weight;

            float_t inertial_cancel = 0;
            if (key_val_read_float_t(&sub_local_key_val,
                buf, off, ".inertial_cancel", 17, &inertial_cancel))
                i->inertial_cancel = inertial_cancel;

            i->hinge.ymin = -skp_root->hinge_y;
            i->hinge.ymax = skp_root->hinge_y;
            i->hinge.zmin = -skp_root->hinge_z;
            i->hinge.zmax = skp_root->hinge_z;

            float_t hinge_ymax = 0.0f;
            if (key_val_read_float_t(&sub_local_key_val,
                buf, off, ".hinge_ymax", 17, &hinge_ymax))
                i->hinge.ymax = hinge_ymax;

            float_t hinge_ymin = 0.0f;
            if (key_val_read_float_t(&sub_local_key_val,
                buf, off, ".hinge_ymin", 17, &hinge_ymin))
                i->hinge.ymin = hinge_ymin;

            float_t hinge_zmax = 0.0f;
            if (key_val_read_float_t(&sub_local_key_val,
                buf, off, ".hinge_zmax", 17, &hinge_zmax))
                i->hinge.zmax = hinge_zmax;

            float_t hinge_zmin = 0.0f;
            if (key_val_read_float_t(&sub_local_key_val,
                buf, off, ".hinge_zmin", 17, &hinge_zmin))
                i->hinge.zmin = hinge_zmin;
        }
        key_val_free(&sub_local_key_val);
    }
    key_val_free(&lkv);
}

static void skin_param_osage_root_init(skin_param_osage_root* root) {
    root->field_0 = 0;
    root->force = 0.0f;
    root->force_gain = 0.0f;
    root->air_res = 0.5f;
    root->rot_y = 0.0f;
    root->rot_z = 0.0f;
    root->init_rot_y = 0.0f;
    root->init_rot_z = 0.0f;
    root->hinge_y = 90.0f;
    root->hinge_z = 90.0f;
    root->name = "NO-PARAM";

    vector_skin_param_osage_root_coli_reserve(&root->coli, 12);
    root->coli.end += 12;
    for (skin_param_osage_root_coli* i = root->coli.begin; i != root->coli.end; i++) {
        i->type = 0;
        i->bone0_index = 0;
        i->bone1_index = 0;
        i->radius = 0.2f;;
        i->bone0_pos = vec3_null;
        i->bone1_pos = vec3_null;
    }

    root->coli_r = 0.0f;
    root->friction = 1.0f;
    root->wind_afc = 0.5f;

    vector_skin_param_osage_root_boc_clear(&root->boc, skin_param_osage_root_boc_free);

    root->coli_type = 0;
    root->stiffness = 0.0f;
    root->move_cancel = -0.01f;
    string_init(&root->colli_tgt_osg, 0);

    vector_skin_param_osage_root_normal_ref_clear(
        &root->normal_ref, skin_param_osage_root_normal_ref_free);
}

static void skin_param_osage_root_boc_free(skin_param_osage_root_boc* boc) {
    string_free(&boc->ed_root);
}

static void skin_param_osage_root_normal_ref_free(skin_param_osage_root_normal_ref* ref) {
    string_free(&ref->r);
    string_free(&ref->l);
    string_free(&ref->d);
    string_free(&ref->u);
    string_free(&ref->n);
}

static void skin_param_osage_root_parse(key_val* kv, char* name,
    skin_param_osage_root* skp_root, bone_database* bone_data) {
    char buf[SKP_TEXT_BUF_SIZE];
    int32_t count;
    size_t len;
    size_t len1;
    size_t len2;
    size_t len3;
    size_t off;

    key_val lkv;
    if (!key_val_get_local_key_val(kv, name, &lkv))
        return;

    len = utf8_length(name);
    memcpy(buf, name, len);
    off = len;

    int32_t node_length = 0;
    key_val_read_int32_t(&lkv,
        buf, off, ".node.length", 13, &node_length);

    len1 = 5;
    memcpy(&buf[len], ".root", 5);
    off = len + len1;

    float_t force = 0.0f;
    float_t force_gain = 0.0f;
    if (key_val_read_float_t(&lkv,
        buf, off, ".force", 7, &force)
        && key_val_read_float_t(&lkv,
            buf, off, ".force_gain", 12, &force_gain)) {
        skp_root->force = force;
        skp_root->force_gain = force_gain;
    }

    float_t air_res = 0.0f;
    if (key_val_read_float_t(&lkv,
        buf, off, ".air_res", 9, &air_res))
        skp_root->air_res = min(air_res, 0.9f);

    float_t rot_y = 0.0f;
    float_t rot_z = 0.0f;
    if (key_val_read_float_t(&lkv,
        buf, off, ".rot_y", 7, &rot_y)
        && key_val_read_float_t(&lkv,
            buf, off, ".rot_z", 7, &rot_z)) {
        skp_root->rot_y = rot_y;
        skp_root->rot_z = rot_z;
    }

    float_t wind_afc = 0;
    if (key_val_read_float_t(&lkv,
        buf, off, ".wind_afc", 10, &wind_afc)) {
        skp_root->wind_afc = wind_afc;
        if (!str_utils_compare_length(name, utf8_length(name), "c_opa", 5))
            skp_root->wind_afc = 0.0f;
    }

    int32_t coli_type = 0;
    if (key_val_read_int32_t(&lkv,
        buf, off, ".coli_type", 11, &coli_type))
        skp_root->coli_type = coli_type;

    float_t init_rot_y = 0.0f;
    float_t init_rot_z = 0.0f;
    if (key_val_read_float_t(&lkv,
        buf, off, ".init_rot_y", 12, &init_rot_y)
        && key_val_read_float_t(&lkv,
            buf, off, ".init_rot_z", 12, &init_rot_z)) {
        skp_root->init_rot_y = init_rot_y;
        skp_root->init_rot_z = init_rot_z;
    }

    float_t hinge_y = 0.0f;
    float_t hinge_z = 0.0f;
    if (key_val_read_float_t(&lkv,
        buf, off, ".hinge_y", 9, &hinge_y)
        && key_val_read_float_t(&lkv,
            buf, off, ".hinge_z", 9, &hinge_z)) {
        skp_root->hinge_y = hinge_y;
        skp_root->hinge_z = hinge_z;
    }

    float_t coli_r = 0.0f;
    if (key_val_read_float_t(&lkv,
        buf, off, ".coli_r", 8, &coli_r))
        skp_root->coli_r = coli_r;

    float_t stiffness = 0.0f;
    if (key_val_read_float_t(&lkv,
        buf, off, ".stiffness", 11, &stiffness))
        skp_root->stiffness = stiffness;

    float_t move_cancel = 0.0f;
    if (key_val_read_float_t(&lkv,
        buf, off, ".move_cancel", 13, &move_cancel))
        skp_root->move_cancel = move_cancel;

    len2 = 5;
    memcpy(buf + len + len1, ".coli", 5);
    off = len + len1 + len2;

    buf[off] = 0;
    key_val sub_local_key_val;
    if (key_val_read_int32_t(&lkv, buf, off, ".length", 8, &count)
        && key_val_get_local_key_val(&lkv, buf, &sub_local_key_val)) {
        vector_skin_param_osage_root_coli* vc = &skp_root->coli;

        vector_skin_param_osage_root_coli_reserve(vc, count);
        vc->end += count;
        for (int32_t i = 0; i < count; i++) {
            skin_param_osage_root_coli* c = &vc->begin[i];
            memset(c, 0, sizeof(skin_param_osage_root_coli));
            len3 = sprintf_s(buf + len + len1 + len2,
                SKP_TEXT_BUF_SIZE - len - len1 - len2, ".%d", i);
            off = len + len1 + len2 + len3;

            int32_t type = 0;
            if (!key_val_read_int32_t(&sub_local_key_val,
                buf, off, ".type", 6, &type))
                break;
            c->type = type;

            float_t radius = 0;
            if (!key_val_read_float_t(&sub_local_key_val,
                buf, off, ".radius", 8, &radius))
                break;
            c->radius = radius;

            char* bone0_name = 0;
            if (!key_val_read_string_ptr(&sub_local_key_val,
                buf, off, ".bone.0.name", 13, &bone0_name))
                break;

            c->bone0_index = bone_database_get_skeleton_object_bone_index(bone_data,
                bone_database_skeleton_type_to_string(BONE_DATABASE_SKELETON_COMMON), bone0_name);

            vec3 bone0_pos = vec3_null;
            if (!key_val_read_float_t(&sub_local_key_val,
                buf, off, ".bone.0.posx", 13, &bone0_pos.x))
                break;

            if (!key_val_read_float_t(&sub_local_key_val,
                buf, off, ".bone.0.posy", 13, &bone0_pos.y))
                break;

            if (!key_val_read_float_t(&sub_local_key_val,
                buf, off, ".bone.0.posz", 13, &bone0_pos.z))
                break;

            c->bone0_pos = bone0_pos;

            char* bone1_name = 0;
            if (key_val_read_string_ptr(&sub_local_key_val,
                buf, off, ".bone.1.name", 13, &bone1_name)) {

                c->bone1_index = bone_database_get_skeleton_object_bone_index(bone_data,
                    bone_database_skeleton_type_to_string(BONE_DATABASE_SKELETON_COMMON), bone1_name);

                vec3 bone1_pos = vec3_null;
                if (!key_val_read_float_t(&sub_local_key_val,
                    buf, off, ".bone.1.posx", 13, &bone1_pos.x))
                    break;

                if (!key_val_read_float_t(&sub_local_key_val,
                    buf, off, ".bone.1.posy", 13, &bone1_pos.y))
                    break;

                if (!key_val_read_float_t(&sub_local_key_val,
                    buf, off, ".bone.1.posz", 13, &bone1_pos.z))
                    break;

                c->bone1_pos = bone1_pos;

            }
        }
        key_val_free(&sub_local_key_val);
    }

    len2 = 4;
    memcpy(buf + len + len1, ".boc", 4);
    off = len + len1 + len2;

    buf[off] = 0;
    if (key_val_read_int32_t(&lkv, buf, off, ".length", 8, &count)
        && key_val_get_local_key_val(&lkv, buf, &sub_local_key_val)) {
        vector_skin_param_osage_root_boc* vb = &skp_root->boc;

        vector_skin_param_osage_root_boc_reserve(vb, count);
        for (int32_t i = 0; i < count; i++) {
            len3 = sprintf_s(buf + len + len1 + len2,
                SKP_TEXT_BUF_SIZE - len - len1 - len2, ".%d", i);
            off = len + len1 + len2 + len3;

            int32_t st_node = 0;
            string ed_root = string_empty;
            int32_t ed_node = 0;
            if (key_val_read_int32_t(&sub_local_key_val,
                buf, off, ".st_node", 9, &st_node)
                && st_node < node_length
                && key_val_read_string(&sub_local_key_val,
                    buf, off, ".ed_root", 9, &ed_root)
                && key_val_read_int32_t(&sub_local_key_val,
                    buf, off, ".ed_node", 9, &ed_node)
                && ed_node < node_length) {
                skin_param_osage_root_boc* b = vector_skin_param_osage_root_boc_reserve_back(vb);
                memset(b, 0, sizeof(skin_param_osage_root_boc));
                b->st_node = st_node;
                b->ed_root = ed_root;
                b->ed_node = ed_node;
            }
            else
                string_free(&ed_root);
        }
        key_val_free(&sub_local_key_val);
    }

    off = len + len1;
    string colli_tgt_osg = string_empty;
    if (key_val_read_string(&lkv,
        buf, off, ".colli_tgt_osg", 15, &colli_tgt_osg))
        skp_root->colli_tgt_osg = colli_tgt_osg;

    len2 = 11;
    memcpy(buf + len + len1, ".normal_ref", 11);
    off = len + len1 + len2;

    buf[off] = 0;
    if (key_val_read_int32_t(&lkv, buf, off, ".length", 8, &count)
        && key_val_get_local_key_val(&lkv, buf, &sub_local_key_val)) {
        vector_skin_param_osage_root_normal_ref* vnr = &skp_root->normal_ref;

        vector_skin_param_osage_root_normal_ref_reserve(vnr, count);
        for (int32_t i = 0; i < count; i++) {
            len3 = sprintf_s(buf + len + len1 + len2,
                SKP_TEXT_BUF_SIZE - len - len1 - len2, ".%d", i);
            off = len + len1 + len2 + len3;

            string n = string_empty;
            if (key_val_read_string(&sub_local_key_val,
                buf, off, ".N", 3, &n)) {
                skin_param_osage_root_normal_ref* nr
                    = vector_skin_param_osage_root_normal_ref_reserve_back(vnr);
                memset(nr, 0, sizeof(skin_param_osage_root_normal_ref));

                nr->n = n;
                key_val_read_string(&sub_local_key_val,
                    buf, off, ".U", 3, &nr->u);
                key_val_read_string(&sub_local_key_val,
                    buf, off, ".D", 3, &nr->d);
                key_val_read_string(&sub_local_key_val,
                    buf, off, ".L", 3, &nr->l);
                key_val_read_string(&sub_local_key_val,
                    buf, off, ".R", 3, &nr->r);
            }
        }
        key_val_free(&sub_local_key_val);
    }

    key_val_free(&lkv);
}

static void skin_param_osage_root_free(skin_param_osage_root* root) {
    vector_skin_param_osage_root_normal_ref_free(
        &root->normal_ref, skin_param_osage_root_normal_ref_free);
    string_free(&root->colli_tgt_osg);
    vector_skin_param_osage_root_boc_free(&root->boc, skin_param_osage_root_boc_free);
    vector_skin_param_osage_root_coli_free(&root->coli, 0);
}

void rob_chara_array_init() {
    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++)
        rob_chara_init(&rob_chara_array[i]);
}

rob_chara* rob_chara_array_get(int32_t chara_id) {
    if (rob_chara_pv_data_array_check_chara_id(chara_id))
        return &rob_chara_array[chara_id];
    return 0;
}

int32_t rob_chara_array_set_pv_data(chara_index chara_index,
    rob_chara_pv_data* pv_data, uint32_t module_index, bool a4) {
    if (pv_data->field_0 > 3)
        return -1;

    rob_chara_pv_data* v9 = rob_chara_pv_data_array;
    int32_t chara_id = 0;
    while (v9->field_0 != -1) {
        chara_id++;
        v9++;
        if (chara_id >= ROB_CHARA_COUNT)
            return -1;
    }

    if (a4 && module_index > 498)
        module_index = 0;
    rob_chara_pv_data_array[chara_id] = *pv_data;
    rob_chara_set_pv_data(&rob_chara_array[chara_id], chara_id, chara_index, module_index, pv_data);
    return chara_id;
}

void rob_chara_array_free() {
    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++)
        rob_chara_free(&rob_chara_array[i]);
}

void rob_chara_pv_data_init(rob_chara_pv_data* pv_data) {
    memset(pv_data, 0, sizeof(rob_chara_pv_data));
    pv_data->field_0 = 2;
    pv_data->field_4 = 1;
    pv_data->field_5 = 0;
    pv_data->field_6 = 0;
    pv_data->field_14 = 0x00;
    pv_data->field_16 = 0xC9;
    pv_data->field_74[0] = -1;
    pv_data->field_74[1] = -1;
    pv_data->field_74[2] = -1;
    pv_data->field_74[3] = -1;
    pv_data->field_74[4] = -1;
    pv_data->field_74[5] = -1;
    pv_data->field_74[6] = -1;
    pv_data->field_74[7] = -1;
    pv_data->field_74[8] = -1;
    pv_data->field_74[9] = -1;
    pv_data->chara_size_index = 1;
    pv_data->height_adjust = false;
    pv_data->eyes_adjust.base_adjust = EYES_BASE_ADJUST_DIRECTION;
    pv_data->eyes_adjust.neg = -1.0f;
    pv_data->eyes_adjust.pos = -1.0f;
}

inline bool rob_chara_pv_data_array_check_chara_id(int32_t chara_id) {
    return rob_chara_pv_data_array[chara_id].field_0 != -1;
}

void rob_chara_pv_data_array_init() {
    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++)
        rob_chara_pv_data_init(&rob_chara_pv_data_array[i]);
    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++)
        rob_chara_pv_data_array[i].field_0 = -1;
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
