/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "rob.hpp"
#include "../../KKdLib/key_val.hpp"
#include "../../KKdLib/str_utils.hpp"
#include "../draw_task.hpp"

struct osage_setting {
    std::map<std::pair<std::string, std::string>, osage_setting_osg_cat> cat;
    std::map<object_info, std::string> obj;

    osage_setting();
    ~osage_setting();

    void parse(key_val* kv);
    rob_osage_parts parse_parts_string(std::string& s);
};

struct exp_func_op1 {
    const char* name;
    float_t(*func)(float_t v1);
};

struct exp_func_op2 {
    const char* name;
    float_t(*func)(float_t v1, float_t v2);
};

struct exp_func_op3 {
    const char* name;
    float_t(*func)(float_t v1, float_t v2, float_t v3);
};

static float_t ex_expression_block_stack_get_value(ex_expression_block_stack* stack);

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
static const exp_func_op1* exp_func_op1_find_func(std::string& name, const exp_func_op1* array);
static const exp_func_op2* exp_func_op2_find_func(std::string& name, const exp_func_op2* array);
static const exp_func_op3* exp_func_op3_find_func(std::string& name, const exp_func_op3* array);
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

static void closest_pt_segment_segment(vec3& vec, const vec3& p0, const vec3& p1, const OsageCollision::Work* cls);

static const osage_setting_osg_cat* osage_setting_data_get_cat_value(
    object_info* obj_info, const char* root_node);

static void sub_140218560(RobCloth* rob_cls, float_t step, bool a3);
static void sub_1402187D0(RobCloth* rob_cls, bool a2);
static void sub_1402196D0(RobCloth* rob_cls);
static void sub_140219940(RobCloth* rob_cls);
static void sub_140219D10(RobCloth* rob_cls);
static float_t sub_14021A290(vec3& a1, vec3& a2, vec3& a3, vec2& a4, vec2& a5, vec2& a6,
    vec3& tangent, vec3& binormal, vec3& normal);
static float_t sub_14021A5E0(vec3& a1, vec3& a2, float_t a3, float_t a4, vec3& tangent, vec3& binormal, vec3& normal);
static void sub_14021A890(CLOTHNode* a1, CLOTHNode* a2, CLOTHNode* a3, CLOTHNode* a4, CLOTHNode* a5);
static void sub_14021AA60(RobCloth* rob_cls, float_t step, bool a3);
static void sub_14021D480(RobCloth* rob_cls);
static void sub_14021DC60(RobCloth* rob_cls, float_t step);
static void sub_14021D840(RobCloth* rob_cls);
static void sub_14047C800(RobOsage* rob_osg, mat4* mat, vec3* parent_scale,
    float_t step, bool disable_wind, bool ring_coli, bool has_children_node);
static void sub_14047E1C0(RobOsage* rob_osg, vec3* scale);
static void sub_14047EE90(RobOsage* rob_osg, mat4* mat);
static void sub_14047F110(RobOsage* rob_osg, mat4* mat, vec3* parent_scale, bool init_rot);
static void sub_1404803B0(RobOsage* rob_osg, mat4* mat, vec3* parent_scale, bool has_children_node);
static void sub_140482180(RobOsageNode* node, float_t a2);
static void sub_140482300(vec3* a1, vec3* a2, vec3* a3, float_t osage_gravity_const, float_t weight);
static void sub_140482490(RobOsageNode* node, float_t step, float_t a3);
static void sub_140482F30(vec3* pos1, vec3* pos2, float_t length);
static void sub_14047D8C0(RobOsage* rob_osg, mat4* mat, vec3* parent_scale, float_t step, bool a5);
static void sub_14047C750(RobOsage* rob_osg, mat4* mat, vec3* parent_scale, float_t step);
static void sub_14047C770(RobOsage* rob_osg, mat4* mat, vec3* parent_scale, float_t step, bool a5);
static void sub_14047D620(RobOsage* rob_osg, float_t step);
static void sub_14047ECA0(RobOsage* rob_osg, float_t step);
static void sub_14047F990(RobOsage* rob_osg, mat4* a2, vec3* parent_scale, bool a4);
static void sub_140480260(RobOsage* rob_osg, mat4* mat, vec3* parent_scale, float_t step, bool v5);
static void sub_140482100(struc_476* a1, opd_blend_data* a2, struc_477* a3);
static void sub_140482DF0(struc_477* dst, struc_477* src0, struc_477* src1, float_t blend);
static bool sub_140482FF0(mat4& mat, vec3& trans, skin_param_hinge* hinge, vec3* rot, int32_t& yz_order);
static void sub_14053CE30(RobOsageNodeDataNormalRef* normal_ref, mat4* a2);
static bool sub_14053D1B0(vec3* l_trans, vec3* r_trans,
    vec3* u_trans, vec3* d_trans, vec3* a5, vec3* a6, vec3* a7);

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

osage_setting osage_setting_data;
size_t qword_140FBDF88 = 0;
int32_t rob_cloth_update_vertices_flags = 0x03;
bool rob_cloth_update_normals_select = false;

ExNodeBlock::ExNodeBlock() : bone_node_ptr(), type(), name(), parent_bone_node(),
parent_name(), parent_node(), item_equip_object(), field_58(), field_59(), has_children_node() {

}

ExNodeBlock::~ExNodeBlock() {

}

void ExNodeBlock::Field_10() {
    field_59 = false;
}

void ExNodeBlock::Reset() {
    bone_node_ptr = 0;
}

void ExNodeBlock::Field_58() {
    field_59 = false;
}

void ExNodeBlock::InitData(bone_node* bone_node, ExNodeType type,
    const char* name, rob_chara_item_equip_object* itm_eq_obj) {
    bone_node_ptr = bone_node;
    this->type = type;
    if (name)
        this->name = name;
    else
        this->name = "(null)";
    parent_bone_node = 0;
    item_equip_object = itm_eq_obj;
}

ExNullBlock::ExNullBlock() {
    Init();
}

ExNullBlock::~ExNullBlock() {

}

void ExNullBlock::Init() {
    bone_node_ptr = 0;
    type = EX_NONE;
    name = 0;
    parent_bone_node = 0;
    item_equip_object = 0;
    cns_data = 0;
}

void ExNullBlock::Field_10() {
    field_59 = false;
}

void ExNullBlock::Field_18(int32_t stage, bool a3) {

}

void ExNullBlock::Field_20() {
    if (!bone_node_ptr)
        return;

    mat4 mat;
    mat4 ex_data_mat = *parent_bone_node->ex_data_mat;
    bone_node_ptr->exp_data.mat_set(parent_bone_node->exp_data.parent_scale, ex_data_mat, mat);
    *bone_node_ptr->ex_data_mat = ex_data_mat;
    *bone_node_ptr->mat = mat;
}

void ExNullBlock::SetOsagePlayData() {
    Field_20();
}

void ExNullBlock::Disp(mat4* mat, render_context* rctx) {

}

void ExNullBlock::Field_40() {

}

void ExNullBlock::Field_48() {
    Field_20();
}

void ExNullBlock::Field_50() {

}

void ExNullBlock::InitData(rob_chara_item_equip_object* itm_eq_obj,
    obj_skin_block_constraint* cns_data, const char* cns_data_name, bone_database* bone_data) {
    bone_node* node = itm_eq_obj->get_bone_node(cns_data_name, bone_data);
    type = EX_NONE;
    bone_node_ptr = node;
    this->cns_data = cns_data;
    name = node->name;
    item_equip_object = itm_eq_obj;
}

RobOsageNodeDataNormalRef::RobOsageNodeDataNormalRef() : field_0(), n(), u(), d(), l(), r() {

}

void RobOsageNodeDataNormalRef::GetMat() {
    if (!field_0)
        return;

    vec3 n_trans;
    vec3 u_trans;
    vec3 d_trans;
    vec3 l_trans;
    vec3 r_trans;
    mat4_get_translation(&n->mat, &n_trans);
    mat4_get_translation(&u->mat, &u_trans);
    mat4_get_translation(&d->mat, &d_trans);
    mat4_get_translation(&l->mat, &l_trans);
    mat4_get_translation(&r->mat, &r_trans);

    vec3 v21;
    vec3 v22;
    vec3 v23;
    if (sub_14053D1B0(&l_trans, &r_trans, &u_trans, &d_trans, &v22, &v21, &v23)) {
        mat4 mat;
        mat.row0.x = v23.x;
        mat.row0.y = v21.x;
        mat.row0.z = v22.x;
        mat.row0.w = 0.0f;
        mat.row1.x = v23.y;
        mat.row1.y = v21.y;
        mat.row1.z = v22.y;
        mat.row1.w = 0.0f;
        mat.row2.x = v23.z;
        mat.row2.y = v21.z;
        mat.row2.z = v22.z;
        mat.row2.w = 0.0f;
        vec3_dot(n_trans, v23, mat.row3.x);
        vec3_dot(n_trans, v21, mat.row3.y);
        vec3_dot(n_trans, v22, mat.row3.z);
        vec3_negate(*(vec3*)&mat.row3, *(vec3*)&mat.row3);
        mat.row3.w = 1.0f;
        mat4_mult(&n->mat, &mat, &mat);
    }
}

void skin_param_hinge::limit() {
    ymin = max_def(ymin, -179.0f) * DEG_TO_RAD_FLOAT;
    ymax = min_def(ymax, 179.0f) * DEG_TO_RAD_FLOAT;
    zmin = max_def(zmin, -179.0f) * DEG_TO_RAD_FLOAT;
    zmax = min_def(zmax, 179.0f) * DEG_TO_RAD_FLOAT;
}

skin_param_osage_node::skin_param_osage_node() : coli_r(), inertial_cancel() {
    weight = 1.0f;
    hinge.limit();
}

RobOsageNodeResetData::RobOsageNodeResetData() : length() {

}

RobOsageNodeData::RobOsageNodeData() : force(), normal_ref() {

}

RobOsageNodeData::~RobOsageNodeData() {

}

void RobOsageNodeData::Reset() {
    force = 0.0f;
    boc.clear();
    normal_ref.field_0 = 0;
    normal_ref.n = 0;
    normal_ref.u = 0;
    normal_ref.d = 0;
    normal_ref.l = 0;
    normal_ref.r = 0;
    normal_ref.mat = mat4_identity;
    skp_osg_node.weight = 1.0f;
    skp_osg_node.inertial_cancel = 0.0f;
    skp_osg_node.coli_r = 0.0f;
    skp_osg_node.hinge = skin_param_hinge();
    skp_osg_node.hinge.limit();
}

struc_477::struc_477() : length() {

}

struc_476::struc_476() {

}

RobOsageNode::RobOsageNode() : length(), trans(), trans_orig(), trans_diff(), field_28(),
child_length(), bone_node_ptr(), bone_node_mat(), sibling_node(), max_distance(),
field_94(), reset_data(), field_C8(), external_force(), mat(), field_1B0() {
    field_CC = 1.0f;
    force = 1.0f;
    data_ptr = &data;
    opd_data.resize(3);
}

RobOsageNode::~RobOsageNode() {

}

void RobOsageNode::Reset() {
    length = 0.0f;
    trans = 0.0f;
    trans_orig = 0.0f;
    trans_diff = 0.0f;
    field_28 = 0.0f;
    child_length = 0.0f;
    bone_node_ptr = 0;
    bone_node_mat = 0;
    sibling_node = 0;
    max_distance = 0.0f;
    field_94 = 0.0f;
    reset_data.trans = 0.0f;
    reset_data.trans_diff = 0.0f;
    reset_data.rotation = 0.0f;
    reset_data.length = 0.0f;
    field_C8 = 0.0f;
    field_CC = 1.0f;
    external_force = 0.0f;
    force = 1.0f;
    data.Reset();
    data_ptr = &data;
    opd_data.clear();
    opd_data.resize(3);
    mat = mat4_null;
}

SkinParam::CollisionParam::CollisionParam() : type(), node_idx(), pos() {
    radius = 0.2f;
}

SkinParam::CollisionParam::~CollisionParam() {

}

skin_param_osage_root_normal_ref::skin_param_osage_root_normal_ref() {

}

skin_param_osage_root_normal_ref::~skin_param_osage_root_normal_ref() {

}

skin_param_osage_root_boc::skin_param_osage_root_boc() : ed_node(), st_node() {

}

skin_param_osage_root_boc::~skin_param_osage_root_boc() {

}

skin_param_osage_root::skin_param_osage_root() : field_0(), force(), force_gain(),
rot_y(), rot_z(), init_rot_y(), init_rot_z(), coli_r(), yz_order(), coli_type(), stiffness() {
    air_res = 0.5f;
    hinge_y = 90.0f;
    hinge_z = 90.0f;
    name ="NO-PARAM";
    friction = 1.0f;
    wind_afc = 0.5f;
    move_cancel = -0.01f;
    coli.resize(13);
}

skin_param_osage_root::~skin_param_osage_root() {

}

skin_param::skin_param() : friction(), wind_afc(), air_res(), rot(), init_rot(),
coli_type(), stiffness(), move_cancel(), coli_r(), force(), force_gain(), colli_tgt_osg() {
    reset();
}

skin_param::~skin_param() {

}

OsageCollision::Work::Work() : type(), radius(), pos(),
vec_center(), vec_center_length(), vec_center_length_squared(), friction() {

}

OsageCollision::Work::~Work() {

}
// 0x140484FE0
int32_t OsageCollision::Work::osage_capsule_cls(vec3& p0, vec3& p1, const float_t& cls_r) {
    return OsageCollision::osage_capsule_cls(p0, p1, cls_r, this);
}

// 0x140485180
int32_t OsageCollision::Work::osage_cls(vec3& p, const float_t& cls_r) {
    return OsageCollision::osage_cls(p, cls_r, this, 0);
}

void OsageCollision::Work::update_cls_work(OsageCollision::Work* cls,
    SkinParam::CollisionParam* cls_param, mat4* transform) {
    cls->type = SkinParam::CollisionTypeEnd;
    if (!cls_param || !transform)
        return;

    for (; cls_param->type; cls++, cls_param++) {
        cls->type = cls_param->type;
        cls->radius = cls_param->radius;
        mat4_mult_vec3_trans(&transform[cls_param->node_idx[0]],
            &cls_param->pos[0], &cls->pos[0]);

        switch (cls->type) {
        case SkinParam::CollisionTypeBall:
            mat4_mult_vec3(&transform[cls_param->node_idx[1]],
                &cls_param->pos[1], &cls->pos[1]);
            break;
        case SkinParam::CollisionTypeCapsulle:
        case SkinParam::CollisionTypeEllipse:
            mat4_mult_vec3_trans(&transform[cls_param->node_idx[1]],
                &cls_param->pos[1], &cls->pos[1]);
            cls->vec_center = cls->pos[1] - cls->pos[0];
            cls->vec_center_length_squared = vec3::length_squared(cls->vec_center);
            cls->vec_center_length = sqrtf(cls->vec_center_length_squared);
            if (cls->vec_center_length < 0.01f)
                cls->type = SkinParam::CollisionTypeBall;
            break;
        }
    }
    cls->type = SkinParam::CollisionTypeEnd;
}

void OsageCollision::Work::update_cls_work(OsageCollision::Work* cls,
    std::vector<SkinParam::CollisionParam>& cls_list, mat4* mats) {
    if (cls_list.size()) {
        cls->type = SkinParam::CollisionTypeEnd;
        cls->radius = 0.0f;
        cls->pos[0] = 0.0f;
        cls->pos[1] = 0.0f;
        cls->vec_center = 0.0f;
        cls->vec_center_length = 0.0f;
        cls->vec_center_length_squared = 0.0f;
        cls->friction = 1.0f;
    }
    else
        update_cls_work(cls, cls_list.data(), mats);
}


OsageCollision::OsageCollision() {

}

OsageCollision::~OsageCollision() {

}

// 0x140483B30
int32_t OsageCollision::cls_aabb_oidashi(vec3& vec, const vec3& p, const OsageCollision::Work* cls, const float_t r) {
    vec3 v29;
    vec3 v30;
    for (int32_t i = 0; i < 3; i++) {
        float_t v13 = ((float_t*)&p)[i];
        float_t v14 = ((float_t*)&cls->pos[0])[i] - v13;
        ((float_t*)&v30)[i] = v14;
        if (v14 > r)
            return 0;

        float_t v15 = v13 - (((float_t*)&cls->pos[0])[i] + ((float_t*)&cls->pos[1])[i]);
        ((float_t*)&v29)[i] = v15;
        if (v15 > r)
            return 0;
    }

    vec3 v28 = 0.0f;
    vec3 v32 = 0.0f;
    vec3 v27 = 1.0f;

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

    if (v16 > r * r)
        return 0;

    if (v16 > 0.0f)
        vec = v27 * v32 * (1.0f / v16) * (r - sqrtf(v16));
    else {
        float_t v25 = ((float_t*)&v28)[0];
        int32_t v26 = 0;
        for (int32_t i = 0; i < 3; i++) {
            if (v25 < ((float_t*)&v28)[i]) {
                v25 = ((float_t*)&v28)[i];
                v26 = i;
            }
        }
        ((float_t*)&vec)[v26] -= ((float_t*)&v27)[v26] * (v25 - r);
    }
    return 1;
}

// 0x140483DE0
int32_t OsageCollision::cls_ball_oidashi(vec3& vec, const vec3& p, const vec3& center, const float_t r) {
    float_t length = vec3::distance_squared(p, center);
    if (length > 0.000001f && length < r * r) {
        vec = (p - center) * (r / sqrtf(length) - 1.0f);
        return 1;
    }
    return 0;
}

// 0x140484540
int32_t OsageCollision::cls_capsule_oidashi(vec3& vec, const vec3& p, const  OsageCollision::Work* cls, const float_t r) {
    vec3 v11 = p - cls->pos[0];

    float_t v17 = vec3::dot(v11, cls->vec_center);
    if (v17 < 0.0f)
        return OsageCollision::cls_ball_oidashi(vec, p, cls->pos[0], r);

    float_t v19 = cls->vec_center_length_squared;
    if (fabsf(v19) <= 0.000001f)
        return OsageCollision::cls_ball_oidashi(vec, p, cls->pos[0], r);
    if (v17 > v19)
        return OsageCollision::cls_ball_oidashi(vec, p, cls->pos[1], r);

    float_t v20 = vec3::length_squared(v11);
    float_t v22 = fabsf(v20 - v17 * v17 / v19);
    if (v22 <= 0.000001f || v22 >= r * r)
        return 0;

    vec = (v11 - cls->vec_center * (v17 / v19)) * (r / sqrtf(v22) - 1.0f);
    return 1;
}

// 0x140483EA0
int32_t OsageCollision::cls_ellipse_oidashi(vec3& vec, const vec3& p, const OsageCollision::Work* cls, const float_t r) {
    if (cls->vec_center_length <= 0.000001f)
        return OsageCollision::cls_ball_oidashi(vec, p, cls->pos[0], r);

    float_t v61 = cls->vec_center_length * 0.5f;
    float_t v13 = sqrtf(r * r + v61 * v61);

    vec3 v54 = p - cls->pos[0];
    float_t v20 = vec3::length(v54);

    vec3 v57 = p - cls->pos[1];
    float_t v21 = vec3::length(v57);

    if (v20 <= 0.000001f || v21 <= 0.000001f)
        return 0;

    if (v20 != 0.0f)
        v54 *= 1.0f / v20;

    if (v21 != 0.0f)
        v57 *= 1.0f / v21;

    float_t v22 = v20 + v21;
    if (v21 + v20 >= v13 * 2.0f)
        return 0;

    float_t v25 = 1.0f / cls->vec_center_length;

    vec3 v63 = p - (cls->pos[0] + cls->pos[1]) * 0.5f;

    float_t v58 = vec3::length(v63);
    if (v58 != 0.0f)
        v63 *= 1.0f / v58;

    float_t v36 = vec3::dot(v63, cls->vec_center) * v25;
    float_t v37 = v36 * v36;
    v37 = min_def(v37, 1.0f);
    v22 *= 0.5f;
    float_t v38 = v22 * v22 - v61;
    if (v38 < 0.0f)
        return 0;

    float_t v39 = sqrtf(v38);
    if (v39 <= 0.000001f)
        return 0;

    float_t v42 = sqrtf(1.0f - v37);
    v22 = (v13 / v22 - 1.0f) * v36 * v58;
    v42 = (r / v39 - 1.0f) * v42 * v58;
    float_t v43 = sqrtf(v42 * v42 + v22 * v22);

    vec = vec3::normalize(v54 + v57) * v43;
    return 1;
}

// 0x140484450
int32_t OsageCollision::cls_line2ball_oidashi(vec3& vec,
    const vec3& p0, const vec3& p1, const vec3& q, const float_t r) {
    vec3 _vec;
    OsageCollision::get_nearest_line2point(_vec, p0, p1, q);
    return OsageCollision::cls_ball_oidashi(vec, _vec, q, r);
}

// 0x1404844A0
int32_t OsageCollision::cls_line2capsule_oidashi(vec3& vec,
    const vec3& p0, const vec3& p1, const OsageCollision::Work* cls, const float_t r) {
    vec3 _vec;
    closest_pt_segment_segment(_vec, p0, p1, cls);
    return OsageCollision::cls_capsule_oidashi(vec, _vec, cls, r);
}

// 0x1404844F0
int32_t OsageCollision::cls_line2ellipse_oidashi(vec3& vec,
    const vec3& p0, const vec3& p1, const OsageCollision::Work* cls, const float_t r) {
    vec3 _vec;
    closest_pt_segment_segment(_vec, p0, p1, cls);
    return OsageCollision::cls_ellipse_oidashi(vec, _vec, cls, r);
}

// 0x140484780
int32_t OsageCollision::cls_plane_oidashi(vec3& vec, const vec3& p, const vec3& p1, const vec3& p2, const float_t r) {
    float_t v5 = vec3::dot(p, p2) - vec3::dot(p1, p2) - r;
    if (v5 >= 0.0f)
        return 0;

    vec = p2 * -v5;
    return 1;
}

// 0x140484E10
void OsageCollision::get_nearest_line2point(vec3& nearest, const vec3& p0, const vec3& p1, const vec3& q) {
    vec3 v6 = p1 - p0;
    vec3 v7 = q - p0;

    float_t v8 = vec3::dot(v7, v6);
    if (v8 < 0.0f) {
        nearest = p0;
        return;
    }

    float_t v9 = vec3::length_squared(v6);
    if (v9 <= 0.000001f)
        nearest = p0;
    else if (v8 <= v9)
        nearest = p0 + v6 * (v8 / v9);
    else
        nearest = p1;
}

// 0x140485000
int32_t OsageCollision::osage_capsule_cls(vec3& p0, vec3& p1, const float_t& cls_r, const OsageCollision::Work* cls) {
    if (!cls)
        return 0;

    int32_t v8 = 0;
    while (cls->type) {
        int32_t type = cls->type;
        vec3 vec = 0.0f;
        switch (cls->type) {
        case SkinParam::CollisionTypeBall:
            v8 += OsageCollision::cls_line2ball_oidashi(vec, p0, p1, cls->pos[0], cls_r + cls->radius);
            break;
        case SkinParam::CollisionTypeCapsulle:
            v8 += OsageCollision::cls_line2capsule_oidashi(vec, p0, p1, cls, cls_r + cls->radius);
            break;
        case SkinParam::CollisionTypeEllipse:
            v8 += OsageCollision::cls_line2ellipse_oidashi(vec, p0, p1, cls, cls_r + cls->radius);
            break;
        }

        if (v8 > 0) {
            p0 += vec;
            p1 += vec;
        }
        cls++;
    }
    return v8;
}

// 0x140485220
int32_t OsageCollision::osage_cls(vec3& p, const float_t& cls_r, const OsageCollision::Work* cls, float_t* fric) {
    if (!cls)
        return 0;

    int32_t v8 = 0;
    while (cls->type != SkinParam::CollisionTypeEnd) {
        int32_t v11 = 0;
        vec3 vec = 0.0f;
        switch (cls->type) {
        case SkinParam::CollisionTypeBall:
            v11 = OsageCollision::cls_ball_oidashi(vec, p, cls->pos[0], cls->radius + cls_r);
            break;
        case SkinParam::CollisionTypeCapsulle:
            v11 = OsageCollision::cls_capsule_oidashi(vec, p, cls, cls->radius + cls_r);
            break;
        case SkinParam::CollisionTypePlane:
            v11 = OsageCollision::cls_plane_oidashi(vec, p, cls->pos[0], cls->pos[1], cls_r);
            break;
        case SkinParam::CollisionTypeEllipse:
            v11 = OsageCollision::cls_ellipse_oidashi(vec, p, cls, cls->radius + cls_r);
            break;
        case SkinParam::CollisionTypeAABB:
            v11 = OsageCollision::cls_aabb_oidashi(vec, p, cls, cls_r);
            break;
        }

        if (fric && v11 > 0) {
            if (*fric > cls->friction)
                *fric = cls->friction;
        }
        cls++;
        v8 += v11;
        p += vec;
    }
    return v8;
}

// 0x1404851C0
int32_t OsageCollision::osage_cls_work_list(vec3& p, const float_t& cls_r, const OsageCollision& coli, float_t* fric) {
    if (coli.work_list.size() && coli.work_list.back().type != SkinParam::CollisionTypeEnd)
        return OsageCollision::osage_cls(p, cls_r, coli.work_list.data(), fric);
    return 0;
}

osage_ring_data::osage_ring_data() : ring_rectangle_x(), ring_rectangle_y(),
ring_rectangle_width(), ring_rectangle_height(), field_18() {
    ring_height = -1000.0f;
    ring_out_height = -1000.0f;
}

osage_ring_data::~osage_ring_data() {

}

CLOTHNode::CLOTHNode() : flags(), trans(), trans_orig(), field_1C(), trans_diff(), normal(),
tangent(), binormal(), tangent_sign(), texcoord(), field_64(), dist_top(), dist_bottom(),
dist_right(), dist_left(), field_80(), field_88(), reset_data(), field_B4(), field_D0() {

}

CLOTHNode::~CLOTHNode() {

}

CLOTH::CLOTH() : field_8(), root_count(), nodes_count(), wind_direction(),
field_44(), set_external_force(), external_force(), skin_param_ptr(), mats() {
    Reset();
}

CLOTH::~CLOTH() {

}

void CLOTH::Init() {
    field_58.clear();
    if (nodes_count <= 1)
        return;

    struc_341 v69 = {};
    size_t root_count = this->root_count;
    for (size_t i = 1; i < nodes_count; i++) {
        for (size_t j = 0; j < root_count; j++) {
            size_t v8 = (i - 1) * root_count + j;
            v69.field_0 = v8;
            size_t v7 = i * root_count + j;
            v69.field_8 = v7;
            vec3_distance(nodes.data()[v7].trans, nodes.data()[v8].trans, v69.length);
            field_58.push_back(v69);
            if (j < root_count - 1) {
                v69.field_0 = v7;
                size_t v32 = i * root_count + j + 1;
                v69.field_8 = v32;
                vec3_distance(nodes.data()[v32].trans, nodes.data()[v7].trans, v69.length);
                field_58.push_back(v69);
            }
        }

        if (field_8 & 0x04) {
            v69.field_0 = i * root_count;
            v69.field_8 = (i + 1) * root_count - 1;
            field_58.push_back(v69);
        }
    }
}

void CLOTH::SetSkinParamColiR(float_t coli_r) {
    skin_param_ptr->coli_r = coli_r;
}

void CLOTH::SetSkinParamFriction(float_t friction) {
    skin_param_ptr->friction = friction;
}

void CLOTH::SetSkinParamWindAfc(float_t wind_afc) {
    skin_param_ptr->wind_afc = wind_afc;
}

void CLOTH::SetWindDirection(vec3& wind_direction) {
    this->wind_direction = wind_direction;
}

void CLOTH::Field_30(float_t a2) {
    field_44 = a2;
}

void CLOTH::SetSkinParamHinge(float_t hinge_y, float_t hinge_z) {
    skin_param_hinge& hinge = skin_param_ptr->hinge;
    hinge.ymax = hinge_y;
    hinge.zmax = hinge_z;
    hinge.ymin = -hinge_y;
    hinge.zmin = -hinge_z;
    hinge.limit();
}

CLOTHNode* CLOTH::GetNodes() {
    return nodes.data();
}

void CLOTH::Reset() {
    field_8 &= ~1;
    root_count = 0;
    nodes_count = 0;
    nodes.clear();
    wind_direction = 0.0f;
    field_44 = 0.0f;
    set_external_force = false;
    external_force = 0.0f;
    skin_param.reset();
    skin_param_ptr = &skin_param;
}

void CLOTH::ResetData() {
    Reset();
}

RobClothSubMeshArray::RobClothSubMeshArray() {
    arr[0] = {};
    arr[1] = {};
    arr[2] = {};
    arr[3] = {};
}

RobCloth::RobCloth() : itm_eq_obj(), cls_root(), cls_data(), move_cancel(), osage_reset(),
mesh(), submesh(), vertex_buffer(), index_buffer(), reset_data_list() {
    ResetData();
}

RobCloth::~RobCloth() {
    ResetData();
}

void RobCloth::ResetData() {
    Reset();
    mats = 0;
    itm_eq_obj = 0;
    cls_root = 0;
    cls_data = 0;
    move_cancel = 0.0f;
    osage_reset = false;
    mesh[0] = {};
    mesh[1] = {};
    vertex_buffer[0].unload();
    vertex_buffer[1].unload();
    index_buffer[0].buffer = 0;
    index_buffer[1].buffer = 0;
    motion_reset_data.clear();
    reset_data_list = 0;
}

void RobCloth::AddMotionResetData(int32_t motion_id, float_t frame) {
    int32_t frame_int = (int32_t)roundf(frame * 1000.0f);

    auto elem = motion_reset_data.find({ motion_id, frame_int });
    if (elem != motion_reset_data.end())
        motion_reset_data.erase(elem);

    std::list<RobOsageNodeResetData> reset_data_list;
    CLOTHNode* i_begin = nodes.data() + root_count;
    CLOTHNode* i_end = nodes.data() + nodes.size();
    for (CLOTHNode* i = i_begin; i != i_end; i++)
        reset_data_list.push_back(i->reset_data);

    motion_reset_data.insert({ { motion_id, frame_int }, reset_data_list });
}

void RobCloth::ColiSet(mat4* mats) {
    if (skin_param_ptr->coli.size())
        OsageCollision::Work::update_cls_work(coli, skin_param_ptr->coli.data(), mats);
    OsageCollision::Work::update_cls_work(coli_ring, ring.skp_root_coli, mats);
}

void RobCloth::Disp(mat4* mat, render_context* rctx) {
    obj* obj = object_storage_get_obj(itm_eq_obj->obj_info);
    if (!obj)
        return;

    std::vector<GLuint>* tex = object_storage_get_obj_set_textures(itm_eq_obj->obj_info.set_id);

    vec3 center;
    vec3_add(nodes.data()[0].trans, nodes.data()[root_count * nodes_count - 1].trans, center);
    vec3_mult_scalar(center, 0.5f, center);

    ::obj o = *obj;
    o.num_mesh = index_buffer[1].buffer != 0 ? 2 : 1;
    o.mesh_array = mesh;
    o.bounding_sphere.center = center;
    o.bounding_sphere.radius *= 2.0f;

    for (uint32_t i = 0; i < o.num_mesh; i++) {
        obj_mesh& mesh = this->mesh[i];
        mesh.bounding_sphere.center = center;
        for (uint32_t j = 0; j < mesh.num_submesh; j++) {
            obj_sub_mesh& submesh = mesh.submesh_array[j];
            submesh.bounding_sphere.center = center;
        }
    }

    rctx->object_data.set_texture_pattern((int32_t)itm_eq_obj->texture_pattern.size(), itm_eq_obj->texture_pattern.data());
    draw_task_add_draw_object_by_obj(rctx, mat, &o, tex, vertex_buffer, index_buffer, 0, itm_eq_obj->alpha);
    rctx->object_data.set_texture_pattern();
}

void RobCloth::InitData(size_t root_count, size_t nodes_count, obj_skin_block_cloth_root* root,
    obj_skin_block_cloth_node* nodes, mat4* mats, int32_t a7,
    rob_chara_item_equip_object* itm_eq_obj, bone_database* bone_data) {
    this->itm_eq_obj = itm_eq_obj;
    obj* obj = object_storage_get_obj(itm_eq_obj->obj_info);
    if (!obj)
        return;

    obj_mesh_index_buffer* index_buffer = object_storage_get_obj_mesh_index_buffer(itm_eq_obj->obj_info);
    uint32_t mesh_name_index = obj->get_obj_mesh_index(cls_data->mesh_name);
    uint32_t backface_mesh_name_index = obj->get_obj_mesh_index(cls_data->backface_mesh_name);

    mesh[0] = obj->mesh_array[mesh_name_index];
    if (cls_data->backface_mesh_name && backface_mesh_name_index != -1)
        mesh[1] = obj->mesh_array[backface_mesh_name_index];
    else
        mesh[1] = {};

    for (int32_t i = 0; i < 2; i++) {
        RobClothSubMeshArray& submesh = this->submesh[i];
        for (uint32_t j = 0; j < mesh[i].num_submesh; j++) {
            obj_sub_mesh& v27 = submesh.arr[j];
            v27 = mesh[i].submesh_array[j];
            mesh[i].submesh_array[j].attrib.m.cloth = 1;
            v27.attrib.m.cloth = 0;
            v27.bounding_sphere.radius = 1000.0f;
            v27.axis_aligned_bounding_box.center = 0.0f;
            v27.axis_aligned_bounding_box.size = { 1000.0f, 1000.0f, 1000.0f };
        }
        mesh[i].submesh_array = submesh.arr;
        mesh[i].bounding_sphere.radius = 1000.0f;
    }

    this->index_buffer[0] = index_buffer[mesh_name_index];
    if (!vertex_buffer[0].load(mesh[0], true))
        return;

    this->index_buffer[1] = {};
    if (cls_data->backface_mesh_name && backface_mesh_name_index != -1)
        this->index_buffer[1] = index_buffer[backface_mesh_name_index];

    if (cls_data->backface_mesh_name && !vertex_buffer[1].load(mesh[1], true))
        return;

    field_8 = (((field_8 ^ (4 * a7)) & 0x04) ^ field_8) | 0x03;

    this->root_count = root_count;
    this->nodes_count = nodes_count;
    this->cls_root = root;
    this->mats = mats;

    this->root.clear();
    this->root.reserve(root_count);

    for (size_t i = 0; i < root_count; i++) {
        this->root.push_back({});
        RobClothRoot& v34 = this->root.back();
        v34.trans = root[i].trans;
        v34.normal = root[i].normal;
        for (int32_t j = 0; j < 4; j++) {
            obj_skin_block_cloth_root_bone_weight& bone_weight = root[i].bone_weights[j];
            v34.node[j] = 0;
            v34.node_mat[j] = 0;
            v34.mat[j] = &mats[bone_weight.matrix_index];
            if (bone_weight.bone_name) {
                v34.node[j] = itm_eq_obj->get_bone_node(bone_weight.bone_name, bone_data);
                if (v34.node[j])
                    v34.node_mat[j] = v34.node[j]->mat;
            }
            v34.weight[j] = bone_weight.weight;
        }
    }

    this->nodes.clear();
    this->nodes.reserve(root_count);

    for (size_t i = 0; i < root_count; i++) {
        this->nodes.push_back({});
        CLOTHNode& v55 = this->nodes.back();
        v55.trans = root[i].trans;
        v55.trans_orig = root[i].trans;
    }

    uint16_t* mesh_index_array = cls_data->mesh_index_array;

    obj_mesh* mesh = object_storage_get_obj_mesh(itm_eq_obj->obj_info, cls_data->mesh_name);
    obj_vertex_format vertex_format = (obj_vertex_format)0;
    obj_vertex_data* vertex_array = 0;
    if (mesh) {
        vertex_format = mesh->vertex_format;
        vertex_array = mesh->vertex_array;
    }

    if (mesh && vertex_format
        & (OBJ_VERTEX_TEXCOORD0 | OBJ_VERTEX_TANGENT | OBJ_VERTEX_NORMAL | OBJ_VERTEX_POSITION)) {
        for (size_t i = 0; i < root_count; ++i)
            this->root.data()[i].tangent = vertex_array[mesh_index_array[i]].tangent;
    }

    for (size_t i = 1; i < nodes_count; i++)
        for (size_t j = 0; j < root_count; j++) {
            this->nodes.push_back({});
            obj_skin_block_cloth_node& node = nodes[(i - 1) * root_count + j];
            CLOTHNode& v72 = this->nodes.back();
            v72.trans = node.trans;
            v72.trans_orig = node.trans;
            v72.normal = { 0.0f, 0.0f, 1.0f };
            v72.tangent = { 1.0f, 0.0f, 0.0f };
            v72.trans_diff = node.trans_diff;
            v72.field_64 = node.trans_diff;
            vec3_normalize(v72.field_64, v72.field_64);
            v72.dist_top = node.dist_top;
            v72.dist_bottom = node.dist_bottom;
            v72.dist_right = node.dist_right;
            v72.dist_left = node.dist_left;
            v72.reset_data = {};

            uint32_t flags = 0x00;
            if (i)
                flags |= 0x01;
            if (i != nodes_count - 1)
                flags |= 0x02;
            if (j)
                flags |= 0x04;
            if (j != root_count - 1)
                flags |= 0x08;
            v72.flags = flags;

            if (mesh && vertex_format & OBJ_VERTEX_TEXCOORD0)
                v72.texcoord = vertex_array[mesh_index_array[i * root_count + j]].texcoord0;
        }

    Init();
}

void RobCloth::InitDataParent(obj_skin_block_cloth* cls_data,
    rob_chara_item_equip_object* itm_eq_obj, bone_database* bone_data) {
    ResetData();
    this->cls_data = cls_data;
    InitData(cls_data->num_root, cls_data->num_node, cls_data->root_array,
        cls_data->node_array, cls_data->mat_array, cls_data->field_14, itm_eq_obj, bone_data);
}

float_t* RobCloth::LoadOpdData(size_t node_index, float_t* opd_data, size_t opd_count) {
    CLOTHNode* i_begin = nodes.data() + root_count;
    CLOTHNode* i_end = nodes.data() + nodes.size();
    for (CLOTHNode* i = i_begin; i != i_end; i++) {
        if (node_index >= i->opd_data.size())
            i->opd_data.resize(node_index + 1);

        opd_vec3_data* opd = &i->opd_data[node_index];
        opd->x = opd_data;
        opd_data += opd_count;
        opd->y = opd_data;
        opd_data += opd_count;
        opd->z = opd_data;
        opd_data += opd_count;
    }
    return opd_data;
}

void RobCloth::LoadSkinParam(void* kv, const char* name, bone_database* bone_data) {
    key_val* _kv = (key_val*)kv;
    skin_param_osage_root root;
    skin_param_osage_root_parse(_kv, name, root, bone_data);
    SetSkinParamOsageRoot(root);
}

void RobCloth::ResetExtrenalForce() {
    set_external_force = false;
    external_force = 0.0f;
}

void RobCloth::SetForceAirRes(float_t force, float_t force_gain, float_t air_res) {
    skin_param_ptr->force = force;
    skin_param_ptr->force_gain = force_gain;
    skin_param_ptr->air_res = air_res;
}

void RobCloth::SetMotionResetData(int32_t motion_id, float_t frame) {
    osage_reset = true;
    auto elem = motion_reset_data.find({ motion_id, (int32_t)roundf(frame * 1000.0f) });
    if (elem != motion_reset_data.end() && elem->second.size() + root_count == nodes.size()) {
        this->reset_data_list = &elem->second;

        std::list<RobOsageNodeResetData>::iterator reset_data_list = this->reset_data_list->begin();
        CLOTHNode* i_begin = nodes.data() + root_count;
        CLOTHNode* i_end = nodes.data() + nodes.size();
        for (CLOTHNode* i = i_begin; i != i_end; i++)
            i->reset_data = *reset_data_list++;
    }
}

void RobCloth::SetOsagePlayData(std::vector<opd_blend_data>& opd_blend_data) {
    sub_140219940(this);

    ::opd_blend_data* i_begin = opd_blend_data.data() + opd_blend_data.size();
    ::opd_blend_data* i_end = opd_blend_data.data();
    for (::opd_blend_data* i = i_begin; i != i_end; ) {
        i--;

        float_t frame = i->frame;
        if (frame >= i->frame_count)
            frame = 0.0f;

        int32_t frame_int = (int32_t)frame;
        if (frame != -0.0f && (float_t)frame_int != frame)
            frame_int = (frame < 0.0f ? frame_int - 1 : frame_int);

        int32_t curr_key = frame_int;
        int32_t next_key = frame_int + 1;
        if ((float_t)next_key >= i->frame_count)
            next_key = 0;

        float_t blend = frame - (float_t)frame_int;
        float_t inv_blend = 1.0f - blend;

        for (size_t j = 0; j < root_count; j++) {
            CLOTHNode& root_node = nodes.data()[j];
            vec3 v26 = root_node.trans;
            mat4 mat = root.data()[j].field_98;
            mat4_translate_mult(&mat, &root_node.trans_orig, &mat);

            CLOTHNode* v29 = &nodes.data()[j + root_count];

            vec3 v78;
            mat4_mult_vec3(&mat, &v29->field_64, &v78);

            int32_t yz_order = 0;
            sub_140482FF0(mat, v78, 0, 0, yz_order);

            mat4 v32 = root.data()[j].field_D8;
            for (size_t k = 1; k < nodes_count; k++, v29 += root_count) {
                opd_vec3_data* opd = &v29->opd_data[i - i_end];
                float_t* opd_x = opd->x;
                float_t* opd_y = opd->y;
                float_t* opd_z = opd->z;

                vec3 v61;
                v61.x = opd_x[curr_key];
                v61.y = opd_y[curr_key];
                v61.z = opd_z[curr_key];

                vec3 v62;
                v62.x = opd_x[next_key];
                v62.y = opd_y[next_key];
                v62.z = opd_z[next_key];

                mat4_mult_vec3_trans(&v32, &v61, &v61);
                mat4_mult_vec3_trans(&v32, &v62, &v62);

                vec3 v57;
                vec3_mult_scalar(v61, blend, v57);

                vec3 v58;
                vec3_mult_scalar(v62, inv_blend, v58);
                vec3_add(v57, v58, v57);

                vec3 v85;
                mat4_mult_vec3_inv_trans(&mat, &v57, &v85);

                vec3 rotation = 0.0f;
                int32_t yz_order = 0;
                sub_140482FF0(mat, v85, 0, &rotation, yz_order);

                float_t v45;
                vec3_distance(v57, v26, v45);
                mat4_translate_mult(&mat, v45, 0.0f, 0.0f, &mat);

                struc_477 v83;
                v83.rotation = rotation;
                v83.length = v29->dist_top;
                sub_140482100(&v29->field_D0, i, &v83);

                v26 = v61;
            }
        }
    }

    for (size_t i = 0; i < root_count; i++) {
        CLOTHNode& root_node = nodes.data()[i];
        mat4 mat = root.data()[0].field_98;
        mat4_translate_mult(&mat, &root_node.trans_orig, &mat);

        CLOTHNode* v50 = &nodes.data()[i + root_count];

        vec3 v78;
        mat4_mult_vec3(&mat, &v50->field_64, &v78);
        int32_t yz_order = 0;
        sub_140482FF0(mat, v78, 0, 0, yz_order);

        for (size_t j = 1; j < nodes_count; j++, v50 += root_count) {
            mat4_rotate_z_mult(&mat, v50->field_D0.field_0.rotation.z, &mat);
            mat4_rotate_y_mult(&mat, v50->field_D0.field_0.rotation.y, &mat);
            mat4_translate_mult(&mat, v50->field_D0.field_0.length, 0.0f, 0.0f, &mat);
            mat4_get_translation(&mat, &v50->trans);
        }
    }
}

float_t* RobCloth::SetOsagePlayDataInit(float_t* opdi_data) {
    CLOTHNode* i_begin = nodes.data() + root_count;
    CLOTHNode* i_end = nodes.data() + nodes.size();
    for (CLOTHNode* i = i_begin; i != i_end; i++) {
        i->trans.x = *opdi_data++;
        i->trans.y = *opdi_data++;
        i->trans.z = *opdi_data++;
        i->trans_diff.x = *opdi_data++;
        i->trans_diff.y = *opdi_data++;
        i->trans_diff.z = *opdi_data++;
        i->field_1C = i->trans;
    }
    return opdi_data;
}

void RobCloth::SetSkinParamOsageRoot(skin_param_osage_root& skp_root) {
    SetForceAirRes(skp_root.force, skp_root.force_gain, skp_root.air_res);
    SetSkinParamFriction(skp_root.friction);
    SetSkinParamWindAfc(skp_root.wind_afc);
    SetSkinParamColiR(skp_root.coli_r);
    SetSkinParamHinge(skp_root.hinge_y, skp_root.hinge_z);
    skin_param_ptr->coli = skp_root.coli;
}

void RobCloth::UpdateDisp() {
    if (pv_osage_manager_array_get_disp())
        return;

    if (rob_cloth_update_vertices_flags & 0x01)
        UpdateNormals();

    if (rob_cloth_update_vertices_flags & 0x02) {
        obj_mesh* mesh = object_storage_get_obj_mesh(itm_eq_obj->obj_info, cls_data->mesh_name);
        obj_mesh* backface_mesh = object_storage_get_obj_mesh(itm_eq_obj->obj_info, cls_data->backface_mesh_name);
        if (cls_data->backface_mesh_name) {
            UpdateVertexBuffer(mesh, &vertex_buffer[0], nodes.data(),
                1.0f, cls_data->num_mesh_index, cls_data->mesh_index_array, false);
            UpdateVertexBuffer(backface_mesh, &vertex_buffer[1], nodes.data(),
                -1.0f, cls_data->num_backface_mesh_index, cls_data->backface_mesh_index_array, false);
        }
        else
            UpdateVertexBuffer(mesh, &vertex_buffer[0], nodes.data(),
                1.0, cls_data->num_mesh_index, cls_data->mesh_index_array, true);
    }
}

void RobCloth::UpdateNormals() {
    ssize_t root_count = this->root_count;
    CLOTHNode* node = &nodes.data()[root_count];
    if (rob_cloth_update_normals_select) {
        for (size_t i = 1; i < nodes_count - 1; i++) {
            sub_14021A890(node, node, node + 1, &node[-root_count], &node[root_count]);
            node++;
            for (ssize_t j = 1; j < root_count - 1; j++, node++)
                sub_14021A890(node, node - 1, node + 1, &node[-root_count], &node[root_count]);
            sub_14021A890(node, node - 1, node, &node[-root_count], &node[root_count]);
            node++;
        }

        sub_14021A890(node, node, node + 1, &node[-root_count], node);
        node++;

        for (ssize_t i = 1; i < root_count - 1; i++, node++)
            sub_14021A890(node, node - 1, node + 1, &node[-root_count], node);
        sub_14021A890(node, node - 1, node, &node[-root_count], node);
    }
    else
        for (size_t i = 1; i < nodes_count; i++, node++) {
            for (ssize_t j = 0; j < root_count - 1; j++, node++) {
                node[0].tangent_sign = sub_14021A290(
                    node[0].trans, node[-root_count].trans, node[1].trans,
                    node[0].texcoord, node[-root_count].texcoord, node[1].texcoord,
                    node[0].tangent, node[0].binormal, node[0].normal);
            }
            node[0].normal = node[-1].normal;
            node[0].tangent = node[-1].tangent;
            node[0].binormal = node[-1].binormal;
            node[0].tangent_sign = node[-1].tangent_sign;
        }
}

void RobCloth::UpdateVertexBuffer(obj_mesh* mesh, obj_mesh_vertex_buffer* vertex_buffer,
    CLOTHNode* nodes, float_t facing, int32_t indices_count, uint16_t* indices, bool double_sided) {
    if (!mesh || !vertex_buffer || (mesh->vertex_format
        & (OBJ_VERTEX_NORMAL | OBJ_VERTEX_POSITION)) != (OBJ_VERTEX_NORMAL | OBJ_VERTEX_POSITION))
        return;

    vertex_buffer->cycle_index();
    size_t data;
    if (GLAD_GL_VERSION_4_5) {
        data = (size_t)glMapNamedBuffer(vertex_buffer->get_buffer(), GL_WRITE_ONLY);
        if (!data) {
            glUnmapNamedBuffer(vertex_buffer->get_buffer());
            return;
        }
    }
    else {
        gl_state_bind_array_buffer(vertex_buffer->get_buffer());
        data = (size_t)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        if (!data) {
            gl_state_bind_array_buffer(0);
            return;
        }
    }

    if (double_sided)
        indices_count /= 2;

    bool tangent = !!(mesh->vertex_format & OBJ_VERTEX_TANGENT);

    if (!mesh->attrib.m.compressed) {
        if (tangent)
            for (int32_t i = 2; i; i--, facing = -1.0f) {
                for (int32_t j = indices_count; j; j--, indices++) {
                    CLOTHNode* node = &nodes[*indices];

                    *(vec3*)data = node->trans;
                    *(vec3*)(data + 0x0C) = node->normal * facing;
                    *(vec3*)(data + 0x18) = node->tangent;
                    *(float_t*)(data + 0x24) = node->tangent_sign;

                    data += mesh->size_vertex;
                }

                if (!double_sided)
                    break;
            }
        else
            for (int32_t i = 2; i; i--, facing = -1.0f) {
                for (int32_t j = indices_count; j; j--, indices++) {
                    CLOTHNode* node = &nodes[*indices];

                    *(vec3*)data = node->trans;
                    *(vec3*)(data + 0x0C) = node->normal * facing;

                    data += mesh->size_vertex;
                }

                if (!double_sided)
                    break;
            }
    }
    else {
        if (tangent)
            for (int32_t i = 2; i; i--, facing = -1.0f) {
                for (int32_t j = indices_count; j; j--, indices++) {
                    CLOTHNode* node = &nodes[*indices];

                    *(vec3*)data = node->trans;

                    vec3 normal = node->normal * (32727.0f * facing);
                    vec3_to_vec3i16(normal, *(vec3i16*)(data + 0x0C));
                    *(int16_t*)(data + 0x12) = 0;

                    vec4 tangent;
                    *(vec3*)&tangent = node->tangent;
                    tangent.w = node->tangent_sign;
                    tangent *= 32727.0f;
                    vec4_to_vec4i16(tangent, *(vec4i16*)(data + 0x14));

                    data += mesh->size_vertex;
                }

                if (!double_sided)
                    break;
            }
        else
            for (int32_t i = 2; i; i--, facing = -1.0f) {
                for (int32_t j = indices_count; j; j--, indices++) {
                    CLOTHNode* node = &nodes[*indices];

                    *(vec3*)data = node->trans;

                    vec3 normal = node->normal * (32727.0f * facing);
                    vec3_to_vec3i16(normal, *(vec3i16*)(data + 0x0C));
                    *(int16_t*)(data + 0x12) = 0;

                    data += mesh->size_vertex;
                }

                if (!double_sided)
                    break;
            }
    }

    if (GLAD_GL_VERSION_4_5)
        glUnmapNamedBuffer(vertex_buffer->get_buffer());
    else {
        glUnmapBuffer(GL_ARRAY_BUFFER);
        gl_state_bind_array_buffer(0);
    }
}

ExClothBlock::ExClothBlock() {
    Init();
}

ExClothBlock::~ExClothBlock() {

}

void ExClothBlock::Init() {
    rob.Reset();
    cls_data = 0;
    mats = 0;
    index = 0;
}

void ExClothBlock::Field_10() {
    field_59 = false;
}

void ExClothBlock::Field_18(int32_t stage, bool a3) {

}

void ExClothBlock::Field_20() {
    rob.ColiSet(mats);
    rob.SetWindDirection(task_wind->ptr->wind_direction);

    rob_chara_item_equip* rob_itm_equip = item_equip_object->item_equip;
    float_t step = get_delta_frame() * rob_itm_equip->step;
    if (rob_itm_equip->opd_blend_data.size() && rob_itm_equip->opd_blend_data[0].field_C)
        step = 1.0f;
    sub_140218560(&rob, step, false);
}

void ExClothBlock::SetOsagePlayData() {
    rob.SetOsagePlayData(item_equip_object->item_equip->opd_blend_data);
}

void ExClothBlock::Disp(mat4* mat, render_context* rctx) {
    rob.UpdateDisp();
    rob.Disp(mat, rctx);
}

void ExClothBlock::Reset() {
    rob.ResetData();
    bone_node_ptr = 0;
}

void ExClothBlock::Field_40() {

}

void ExClothBlock::Field_48() {
    rob.ColiSet(mats);
    rob.SetWindDirection(task_wind->ptr->wind_direction);
    sub_14021D480(&rob);
}

void ExClothBlock::Field_50() {
    rob.ColiSet(mats);
    rob.SetWindDirection(task_wind->ptr->wind_direction);
    sub_14021D840(&rob);
}

void ExClothBlock::AddMotionResetData(int32_t motion_id, float_t frame) {
    rob.AddMotionResetData(motion_id, frame);
}

void ExClothBlock::InitData(rob_chara_item_equip_object* itm_eq_obj,
    obj_skin_block_cloth* cls_data, skin_param_osage_root* skp_root, bone_database* bone_data) {
    type = EX_CLOTH;
    bone_node_ptr = 0;
    this->cls_data = cls_data;
    name = skp_root ? skp_root->name : 0;
    item_equip_object = itm_eq_obj;

    mats = itm_eq_obj->get_bone_node(0)->mat;
    rob.ResetData();
    rob.InitDataParent(cls_data, itm_eq_obj, bone_data);
    SetSkinParamOsageRoot(skp_root);
}

float_t* ExClothBlock::LoadOpdData(size_t node_index, float_t* opd_data, size_t opd_count) {
    return rob.LoadOpdData(node_index, opd_data, opd_count);
}

void ExClothBlock::SetMotionResetData(int32_t motion_id, float_t frame) {
    rob.SetMotionResetData(motion_id, frame);
}

float_t* ExClothBlock::SetOsagePlayDataInit(float_t* opdi_data) {
    return rob.SetOsagePlayDataInit(opdi_data);
}

void ExClothBlock::SetOsageReset() {
    rob.osage_reset = true;
}

void ExClothBlock::SetSkinParam(struc_571* skp) {
    rob.skin_param_ptr = &skp->skin_param;
}

void ExClothBlock::SetSkinParamOsageRoot(skin_param_osage_root* skp_root) {
    if (!skp_root)
        return;

    rob.SetSkinParamOsageRoot(*skp_root);

    vec3 wind_direction;
    vec3_mult_scalar(task_wind->ptr->wind_direction,
        item_equip_object->item_equip->wind_strength,
        wind_direction);
    rob.SetWindDirection(wind_direction);
}

struc_571::struc_571() : field_88() {

}

struc_571::~struc_571() {

}

osage_setting_osg_cat::osage_setting_osg_cat() : exf() {
    parts = ROB_OSAGE_PARTS_NONE;
}

RobOsage::RobOsage() : skin_param_ptr(), osage_setting(), field_2A0(), field_2A1(), field_2A4(),
wind_direction(), field_1EB4(), yz_order(), field_1EBC(), parent_mat_ptr(), parent_mat(), move_cancel(),
field_1F0C(), osage_reset(), field_1F0E(), field_1F0F(), set_external_force(), external_force(), reset_data_list() {

}

RobOsage::~RobOsage() {

}

void RobOsage::AddMotionResetData(int32_t motion_id, float_t frame) {
    int32_t frame_int = (int32_t)roundf(frame * 1000.0f);

    auto elem = motion_reset_data.find({ motion_id, frame_int });
    if (elem != motion_reset_data.end())
        motion_reset_data.erase(elem);

    std::list<RobOsageNodeResetData> reset_data_list;
    RobOsageNode* i_begin = nodes.data() + 1;
    RobOsageNode* i_end = nodes.data() + nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++)
        reset_data_list.push_back(i->reset_data);

    motion_reset_data.insert({ { motion_id, frame_int }, reset_data_list });
}

bool RobOsage::CheckPartsBits(rob_osage_parts_bit parts_bits) {
    if (osage_setting.parts != ROB_OSAGE_PARTS_NONE)
        return !!(parts_bits & (1 << osage_setting.parts));
    return false;
}

void RobOsage::ColiSet(mat4* mats) {
    if (skin_param_ptr->coli.size())
        OsageCollision::Work::update_cls_work(coli, skin_param_ptr->coli.data(), mats);
    OsageCollision::Work::update_cls_work(coli_ring, ring.skp_root_coli, mats);
}

RobOsageNode* RobOsage::GetNode(size_t index) {
    if (index < nodes.size())
        return &nodes.data()[index];
    else
        return &node;
}

void RobOsage::InitData(obj_skin_block_osage* osg_data, obj_skin_osage_node* osg_nodes,
    bone_node* ex_data_bone_nodes, obj_skin* skin) {
    Reset();
    exp_data.set_position_rotation(osg_data->node.position, osg_data->node.rotation);
    nodes.clear();
    nodes.resize(osg_data->count + 1ULL);

    RobOsageNode* external = &nodes.data()[0];
    nodes.data()[0].Reset();
    external->child_length = osg_nodes->length;
    bone_node* external_bone_node = &ex_data_bone_nodes[osg_data->external_name_index & 0x7FFF];
    external->bone_node_ptr = external_bone_node;
    external->bone_node_mat = external_bone_node->mat;
    *external_bone_node->ex_data_mat = mat4_identity;
    external->sibling_node = 0;

    bone_node* parent_bone_node = external->bone_node_ptr;
    for (uint32_t i = 0; i < osg_data->count; i++) {
        obj_skin_osage_node* v14 = &osg_nodes[i];
        RobOsageNode* node = &nodes.data()[i + 1LL];
        node->Reset();
        node->length = v14[0].length;
        if (i != osg_data->count - 1)
            node->child_length = v14[1].length;
        bone_node* node_bone_node = &ex_data_bone_nodes[v14->name_index & 0x7FFF];
        node->bone_node_ptr = node_bone_node;
        node->bone_node_mat = node_bone_node->mat;
        node_bone_node->parent = parent_bone_node;
        parent_bone_node = node->bone_node_ptr;
    }

    node.Reset();
    node.length = osg_nodes[osg_data->count - 1].length;
    bone_node* v22 = &ex_data_bone_nodes[osg_data->name_index & 0x7FFF];
    node.bone_node_ptr = v22;
    node.bone_node_mat = v22->mat;
    v22->parent = parent_bone_node;
    *node.bone_node_ptr->ex_data_mat = mat4_identity;

    if (osg_data->count) {
        obj_skin_osage_node* node_array = osg_data->node_array;
        size_t v24 = 0;
        RobOsageNode* v26 = &nodes.data()[1];
        for (size_t i = 0; i < osg_data->count; i++) {
            vec3 v27 = 0.0f;
            if (node_array)
                v27 = node_array[i].rotation;

            mat4 mat;
            mat4_rotate(&v27, &mat);

            vec3 v49;
            v49 = 0.0f;
            v49.x = v26[i].length;
            mat4_mult_vec3(&mat, &v49, &v26[i].field_94);
        }
    }

    if (osg_data->count) {
        RobOsageNode* v26 = &nodes.data()[1];
        size_t v33 = 0;
        for (uint32_t i = 0; i < osg_data->count; i++) {
            v26[i].mat = mat4_null;

            obj_skin_bone* bone = skin->bone_array;
            for (uint32_t j = 0; j < skin->num_bone; j++, bone++)
                if (bone->id == osg_nodes->name_index) {
                    mat4_inverse_normalized(&bone->inv_bind_pose_mat, &v26[i].mat);
                    break;
                }

            osg_nodes++;
        }
    }

    RobOsageNode* v38 = &nodes.back();
    if (memcmp(&v38->mat, &mat4_null, sizeof(mat4)))
        mat4_translate_mult(&v38->mat, node.length, 0.0f, 0.0f, &node.mat);
}

float_t* RobOsage::LoadOpdData(size_t node_index, float_t* opd_data, size_t opd_count) {
    RobOsageNode* i_begin = nodes.data() + 1;
    RobOsageNode* i_end = nodes.data() + nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++) {
        if (node_index >= i->opd_data.size())
            i->opd_data.resize(node_index + 1);

        opd_vec3_data* opd = &i->opd_data[node_index];
        opd->x = opd_data;
        opd_data += opd_count;
        opd->y = opd_data;
        opd_data += opd_count;
        opd->z = opd_data;
        opd_data += opd_count;
    }
    return opd_data;
}

void RobOsage::LoadSkinParam(void* kv, const char* name,
    skin_param_osage_root& skp_root, object_info* obj_info, bone_database* bone_data) {
    key_val* _kv = (key_val*)kv;
    skin_param_ptr = &skin_param;
    for (RobOsageNode& i : nodes)
        i.data_ptr = &i.data;

    skin_param_osage_root_parse(_kv, name, skp_root, bone_data);
    SetSkinParamOsageRoot(skp_root);

    const osage_setting_osg_cat* osage_setting = osage_setting_data_get_cat_value(obj_info, name);
    if (osage_setting)
        this->osage_setting = *osage_setting;

    std::vector<skin_param_osage_node> vec;
    vec.resize(nodes.size() - 1);
    skin_param_osage_node_parse(_kv, name, &vec, skp_root);
    SetSkpOsgNodes(&vec);
}

void RobOsage::Reset() {
    nodes.clear();
    node.Reset();
    wind_direction = 0.0f;
    field_1EB4 = 0.0f;
    yz_order = 0;
    field_2A0 = true;
    motion_reset_data.clear();
    move_cancel = 0.0f;
    field_1F0C = false;
    osage_reset = false;
    field_1F0E = false;
    ring = osage_ring_data();
    field_1F0F = 0;
    skin_param.reset();
    skin_param_ptr = &skin_param;
    osage_setting = osage_setting_osg_cat();
    reset_data_list = 0;
    field_2A4 = 0.0f;
    field_2A1 = false;
    set_external_force = false;
    external_force = 0.0f;
    parent_mat_ptr = 0;
    parent_mat = mat4_null;
}

void RobOsage::ResetExtrenalForce() {
    set_external_force = false;
    external_force = 0.0f;
}

void RobOsage::SetAirRes(float_t air_res) {
    skin_param_ptr->air_res = air_res;
}

void RobOsage::SetColiR(float_t coli_r) {
    RobOsageNode* i_begin = nodes.data() + 1;
    RobOsageNode* i_end = nodes.data() + nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++)
        i->data_ptr->skp_osg_node.coli_r = coli_r;
}

void RobOsage::SetForce(float_t force, float_t force_gain) {
    skin_param_ptr->force = force;
    skin_param_ptr->force_gain = force_gain;
    RobOsageNode* i_begin = nodes.data() + 1;
    RobOsageNode* i_end = nodes.data() + nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++) {
        i->data_ptr->force = force;
        force = force * force_gain;
    }
}

void RobOsage::SetHinge(float_t hinge_y, float_t hinge_z) {
    hinge_y = min_def(hinge_y, 179.0f);
    hinge_z = min_def(hinge_z, 179.0f);
    hinge_y = hinge_y * DEG_TO_RAD_FLOAT;
    hinge_z = hinge_z * DEG_TO_RAD_FLOAT;
    RobOsageNode* i_begin = nodes.data() + 1;
    RobOsageNode* i_end = nodes.data() + nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++) {
        RobOsageNodeData* data = i->data_ptr;
        data->skp_osg_node.hinge.ymin = -hinge_y;
        data->skp_osg_node.hinge.ymax = hinge_y;
        data->skp_osg_node.hinge.zmin = -hinge_z;
        data->skp_osg_node.hinge.zmax = hinge_z;
    }
}

void RobOsage::SetInitRot(float_t init_rot_y, float_t init_rot_z) {
    skin_param_ptr->init_rot.y = init_rot_y * DEG_TO_RAD_FLOAT;
    skin_param_ptr->init_rot.z = init_rot_z * DEG_TO_RAD_FLOAT;
}

void RobOsage::SetMotionResetData(int32_t motion_id, float_t frame) {
    osage_reset = true;
    auto v6 = motion_reset_data.find({ motion_id, (int32_t)roundf(frame * 1000.0f) });
    if (v6 != motion_reset_data.end() && v6->second.size() + 1 == nodes.size())
        reset_data_list = &v6->second;
}

void RobOsage::SetNodesExternalForce(vec3* external_force, float_t strength) {
    vec3 v4;
    if (external_force)
        v4 = *external_force;
    else
        v4 = 0.0f;

    size_t exf = osage_setting.exf;
    size_t v8 = 0;
    if (exf >= 4) {
        float_t strength4 = strength * strength * strength * strength;
        v8 = ((exf - 4) / 4 + 1) * 4;
        for (size_t v10 = v8 / 4; v10; v10--)
            v4 *= strength4;
    }

    if (v8 < exf)
        for (size_t v12 = exf - v8; v12; v12--)
            v4 *= strength;

    RobOsageNode* i_begin = nodes.data() + 1;
    RobOsageNode* i_end = nodes.data() + nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++) {
        i->external_force = v4;
        v4 *= strength;
    }
}

void RobOsage::SetNodesForce(float_t force) {
    RobOsageNode* i_begin = nodes.data() + 1;
    RobOsageNode* i_end = nodes.data() + nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++)
        i->force = force;
}

void RobOsage::SetOsagePlayData(mat4* parent_mat,
    vec3& parent_scale, std::vector<opd_blend_data>& opd_blend_data) {
    if (!opd_blend_data.size())
        return;

    vec3 v63;
    vec3_mult(exp_data.position, parent_scale, v63);

    mat4 v85;
    mat4_mult_vec3_trans(&v85, &v63, &nodes.data()[0].trans);

    sub_14047F110(this, &v85, &parent_scale, false);
    *nodes.data()[0].bone_node_mat = v85;
    *nodes.data()[0].bone_node_ptr->ex_data_mat = v85;

    ::opd_blend_data* i_begin = opd_blend_data.data() + opd_blend_data.size();
    ::opd_blend_data* i_end = opd_blend_data.data();
    for (::opd_blend_data* i = i_begin; i != i_end; ) {
        i--;

        mat4 v87 = v85;
        vec3 v22 = nodes.data()[0].trans;
        vec3 v25 = v22;

        float_t frame = i->frame;
        if (frame >= i->frame_count)
            frame = 0.0f;

        int32_t frame_int = (int32_t)frame;
        if (frame != -0.0f && (float_t)frame_int != frame)
            frame_int = (frame < 0.0f ? frame_int - 1 : frame_int);

        int32_t curr_key = frame_int;
        int32_t next_key = frame_int + 1;
        if ((float_t)next_key >= i->frame_count)
            next_key = 0;

        float_t blend = frame - (float_t)frame_int;
        float_t inv_blend = 1.0f - blend;
        RobOsageNode* j_begin = nodes.data() + 1;
        RobOsageNode* j_end = nodes.data() + nodes.size();
        for (RobOsageNode* j = j_begin; j != j_end; j++) {
            opd_vec3_data* opd = &j->opd_data[i - i_end];
            float_t* opd_x = opd->x;
            float_t* opd_y = opd->y;
            float_t* opd_z = opd->z;

            vec3 v62;
            v62.x = opd_x[curr_key];
            v62.y = opd_y[curr_key];
            v62.z = opd_z[curr_key];

            vec3 v77;
            v77.x = opd_x[next_key];
            v77.y = opd_y[next_key];
            v77.z = opd_z[next_key];

            mat4_mult_vec3_trans(parent_mat, &v62, &v62);
            mat4_mult_vec3_trans(parent_mat, &v77, &v77);

            vec3 v82;
            vec3_mult_scalar(v77, blend, v82);

            vec3 v81;
            vec3_mult_scalar(v62, inv_blend, v81);
            vec3_add(v82, v81, v82);

            vec3 v86;
            mat4_mult_vec3_inv_trans(&v87, &v82, &v86);

            vec3 rotation = 0.0f;
            sub_140482FF0(v87, v86, 0, &rotation, yz_order);
            mat4_set_translation(&v87, &v82);

            float_t v50;
            vec3_distance(v77, v25, v50);

            float_t v49;
            vec3_distance(v62, v22, v49);

            struc_477 v72;
            v72.rotation = rotation;
            v72.length = v49 * inv_blend + v50 * blend;
            sub_140482100(&j->field_1B0, i, &v72);

            v22 = v62;
            v25 = v77;
        }
    }

    RobOsageNode* j_begin = nodes.data() + 1;
    RobOsageNode* j_end = nodes.data() + nodes.size();
    for (RobOsageNode* j = j_begin; j != j_end; j++) {
        float_t rot_y = j->field_1B0.field_0.rotation.y;
        float_t rot_z = j->field_1B0.field_0.rotation.z;
        rot_y = clamp_def(rot_y, (float_t)-M_PI, (float_t)M_PI);
        rot_z = clamp_def(rot_z, (float_t)-M_PI, (float_t)M_PI);
        mat4_rotate_z_mult(&v85, rot_z, &v85);
        mat4_rotate_y_mult(&v85, rot_y, &v85);
        j->bone_node_ptr->exp_data.parent_scale = parent_scale;
        *j->bone_node_ptr->ex_data_mat = v85;
        if (j->bone_node_mat) {
            mat4 mat = v85;
            mat4_scale_rot(&mat, &parent_scale, &mat);
            *j->bone_node_mat = mat;
        }
        mat4_translate_mult(&v85, j->field_1B0.field_0.length, 0.0f, 0.0f, &v85);
        j->trans_orig = j->trans;
        mat4_get_translation(&v85, &j->trans);
    }

    if (nodes.size() && node.bone_node_mat) {
        mat4 v87 = *nodes.back().bone_node_ptr->ex_data_mat;
        mat4_translate_mult(&v87, node.length * parent_scale.x, 0.0f, 0.0f, &v87);
        *node.bone_node_ptr->ex_data_mat = v87;

        mat4_scale_rot(&v87, &parent_scale, &v87);
        *node.bone_node_mat = v87;
        node.bone_node_ptr->exp_data.parent_scale = parent_scale;
    }
}

float_t* RobOsage::SetOsagePlayDataInit(float_t* opdi_data) {
    RobOsageNode* i_begin = nodes.data() + 1;
    RobOsageNode* i_end = nodes.data() + nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++) {
        i->trans.x = *opdi_data++;
        i->trans.y = *opdi_data++;
        i->trans.z = *opdi_data++;
        i->trans_diff.x = *opdi_data++;
        i->trans_diff.y = *opdi_data++;
        i->trans_diff.z = *opdi_data++;
        i->trans_orig = i->trans;
    }
    return opdi_data;
}

void RobOsage::SetRot(float_t rot_y, float_t rot_z) {
    skin_param_ptr->rot.y = rot_y * DEG_TO_RAD_FLOAT;
    skin_param_ptr->rot.z = rot_z * DEG_TO_RAD_FLOAT;
}

void RobOsage::SetSkinParamOsageRoot(skin_param_osage_root& skp_root) {
    skin_param_ptr->reset();
    SetForce(skp_root.force, skp_root.force_gain);
    SetAirRes(skp_root.air_res);
    SetRot(skp_root.rot_y, skp_root.rot_z);
    SetInitRot(skp_root.init_rot_y, skp_root.init_rot_z);
    SetYZOrder(skp_root.yz_order);
    skin_param_ptr->friction = skp_root.friction;
    skin_param_ptr->wind_afc = skp_root.wind_afc;
    SetColiR(skp_root.coli_r);
    SetHinge(skp_root.hinge_y, skp_root.hinge_z);
    skin_param_ptr->coli = skp_root.coli;
    skin_param_ptr->coli_type = skp_root.coli_type;
    skin_param_ptr->stiffness = skp_root.stiffness;
    skin_param_ptr->move_cancel = skp_root.move_cancel;
}

void RobOsage::SetSkpOsgNodes(std::vector<skin_param_osage_node>* skp_osg_nodes) {
    if (!skp_osg_nodes)
        return;

    if (nodes.size() - 1 != skp_osg_nodes->size())
        return;

    RobOsageNode* i = nodes.data() + 1;
    RobOsageNode* i_end = nodes.data() + nodes.size();
    skin_param_osage_node* j = skp_osg_nodes->data();
    skin_param_osage_node* j_end = skp_osg_nodes->data() + skp_osg_nodes->size();
    while (i != i_end && j != j_end) {
        i->data_ptr->skp_osg_node = *j++;
        i->data_ptr->skp_osg_node.hinge.limit();
        i++;
    }
}

void RobOsage::SetYZOrder(int32_t yz_order) {
    this->yz_order = yz_order;
}

ExOsageBlock::ExOsageBlock() : rob(), mats(), step(1.0f) {
    Reset();
}

ExOsageBlock::~ExOsageBlock() {
    Reset();
}

void ExOsageBlock::Init() {
    Reset();
}

void ExOsageBlock::Field_10() {
    field_59 = false;
}

void ExOsageBlock::Field_18(int32_t stage, bool a3) {
    rob_chara_item_equip* rob_itm_equip = item_equip_object->item_equip;
    float_t step = get_delta_frame() * rob_itm_equip->step;
    if (rob_itm_equip->opd_blend_data.size() && rob_itm_equip->opd_blend_data[0].field_C)
        step = 1.0f;

    mat4* parent_node_mat = parent_bone_node->ex_data_mat;
    vec3 parent_scale = parent_bone_node->exp_data.parent_scale;
    switch (stage) {
    case 0:
        sub_1404803B0(&rob, parent_node_mat, &parent_scale, has_children_node);
        break;
    case 1:
    case 2:
        if ((stage == 1 && field_58) || (stage == 2 && rob.field_2A0)) {
            SetWindDirection();
            sub_14047C800(&rob, parent_node_mat, &parent_scale, step, a3, true, has_children_node);
        }
        break;
    case 3:
        rob.ColiSet(mats);
        sub_14047ECA0(&rob, step);
        break;
    case 4:
        sub_14047D620(&rob, step);
        break;
    case 5: {
        sub_14047D8C0(&rob, parent_node_mat, &parent_scale, step, false);
        sub_140480260(&rob, parent_node_mat, &parent_scale, step, a3);
        field_59 = true;
    } break;
    }
}

void ExOsageBlock::Field_20() {
    field_1FF8 &= ~2;
    if (field_59) {
        field_59 = false;
        return;
    }

    rob_chara_item_equip* rob_itm_equip = item_equip_object->item_equip;
    float_t step = get_delta_frame() * rob_itm_equip->step;
    if (rob_itm_equip->opd_blend_data.size() && rob_itm_equip->opd_blend_data[0].field_C)
        step = 1.0f;

    vec3 parent_scale = parent_bone_node->exp_data.parent_scale;
    vec3 scale = parent_bone_node->exp_data.scale;

    mat4 mat = *parent_bone_node->ex_data_mat;
    if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f) {
        vec3_div(1.0f, scale, scale);
        mat4_scale_rot(&mat, &scale, &mat);
    }
    SetWindDirection();
    rob.ColiSet(mats);
    sub_14047C750(&rob, &mat, &parent_scale, step);
}

void ExOsageBlock::SetOsagePlayData() {
    rob.SetOsagePlayData(parent_bone_node->ex_data_mat,
        parent_bone_node->exp_data.parent_scale, item_equip_object->item_equip->opd_blend_data);
}

void ExOsageBlock::Disp(mat4* mat, render_context* rctx) {

}

void ExOsageBlock::Reset() {
    index = 0;
    rob.Reset();
    field_1FF8 &= ~3;
    mats = 0;
    step = 1.0f;
    ExNodeBlock::Reset();
}

void ExOsageBlock::Field_40() {

}

void ExOsageBlock::Field_48() {
    step = 4.0f;
    SetWindDirection();
    rob.ColiSet(mats);
    vec3 parent_scale = parent_bone_node->exp_data.parent_scale;
    sub_14047F990(&rob, parent_bone_node->ex_data_mat, &parent_scale, 0);
    field_1FF8 &= ~2;
}

void ExOsageBlock::Field_50() {
    if (field_59) {
        field_59 = false;
        return;
    }

    SetWindDirection();

    vec3 parent_scale = parent_bone_node->exp_data.parent_scale;
    rob.ColiSet(mats);
    sub_14047C770(&rob, parent_bone_node->ex_data_mat, &parent_scale, step, 1);
    float_t step = 0.5f * this->step;
    this->step = max_def(step, 1.0f);
}

void ExOsageBlock::Field_58() {
    vec3 parent_scale = parent_bone_node->exp_data.parent_scale;
    sub_14047E1C0(&rob, &parent_scale);
    field_59 = false;
}

void ExOsageBlock::AddMotionResetData(int32_t motion_id, float_t frame) {
    rob.AddMotionResetData(motion_id, frame);
}

void ExOsageBlock::InitData(rob_chara_item_equip_object* itm_eq_obj,
    obj_skin_block_osage* osg_data, const char* osg_data_name, obj_skin_osage_node* osg_nodes,
    bone_node* bone_nodes, bone_node* ex_data_bone_nodes, obj_skin* skin) {
    ExNodeBlock::InitData(&ex_data_bone_nodes[osg_data->external_name_index & 0x7FFF],
        EX_OSAGE, osg_data_name, itm_eq_obj);
    rob.InitData(osg_data, osg_nodes, ex_data_bone_nodes, skin);
    field_1FF8 &= ~2;
    mats = bone_nodes->mat;
}

float_t* ExOsageBlock::LoadOpdData(size_t node_index, float_t* opd_data, size_t opd_count) {
    return rob.LoadOpdData(node_index, opd_data, opd_count);
}

void ExOsageBlock::SetMotionResetData(int32_t motion_id, float_t frame) {
    rob.SetMotionResetData(motion_id, frame);
}

float_t* ExOsageBlock::SetOsagePlayDataInit(float_t* opdi_data) {
    return rob.SetOsagePlayDataInit(opdi_data);
}

void ExOsageBlock::SetOsageReset() {
    rob.osage_reset = true;
}

void ExOsageBlock::SetSkinParam(struc_571* skp) {
    if (skp->nodes_data.size() == rob.nodes.size() - 1) {
        size_t node_index = 0;
        RobOsageNode* i_begin = rob.nodes.data() + 1;
        RobOsageNode* i_end = rob.nodes.data() + rob.nodes.size();
        for (RobOsageNode* i = i_begin; i != i_end; i++)
            i->data_ptr = &skp->nodes_data[node_index++];
    }
    else {
        rob.skin_param_ptr = &rob.skin_param;
        for (RobOsageNode& i : rob.nodes)
            i.data_ptr = &i.data;
    }
}

void ExOsageBlock::SetWindDirection() {
    vec3_mult_scalar(task_wind->ptr->wind_direction,
        item_equip_object->item_equip->wind_strength,
        rob.wind_direction);
}

void ExOsageBlock::sub_1405F3E10(obj_skin_block_osage* osg_data,
    obj_skin_osage_node* osg_nodes, std::vector<std::pair<uint32_t, RobOsageNode*>>* a4,
    std::map<const char*, ExNodeBlock*>* a5) {
    RobOsageNode* node = rob.GetNode(0);
    a4->push_back({ osg_data->external_name_index, node });

    for (uint32_t i = 0; i < osg_data->count; i++) {
        node = rob.GetNode(i);
        a4->push_back({ osg_nodes[i].name_index, node });

        if (node->bone_node_ptr && node->bone_node_ptr->name)
            a5->insert({ node->bone_node_ptr->name, this });
    }

    node = rob.GetNode(osg_data->count + 1ULL);
    a4->push_back({ osg_data->name_index, node });

    if (node->bone_node_ptr && node->bone_node_ptr->name)
        a5->insert({ node->bone_node_ptr->name, this });
}

ExConstraintBlock::ExConstraintBlock() : constraint_type(),
source_node_bone_node(), direction_up_vector_bone_node(), cns_data() {
    Init();
}

ExConstraintBlock::~ExConstraintBlock() {

}

void ExConstraintBlock::Init() {
    constraint_type = OBJ_SKIN_BLOCK_CONSTRAINT_NONE;
    source_node_bone_node = 0;
    direction_up_vector_bone_node = 0;
    cns_data = 0;
    field_80 = 0;
}

void ExConstraintBlock::Field_10() {
    if (bone_node_ptr) {
        bone_node_expression_data* node_exp_data = &bone_node_ptr->exp_data;
        node_exp_data->position = cns_data->node.position;
        node_exp_data->rotation = cns_data->node.rotation;
        node_exp_data->scale = cns_data->node.scale;
    }
    field_59 = false;
}

void ExConstraintBlock::Field_18(int32_t stage, bool a3) {
    if (field_59)
        return;

    if (stage == 0) {
        if (field_58)
            Field_20();
    }
    else if (stage == 2) {
        if (has_children_node)
            DataSet();
    }
    else if (stage == 5)
        Field_20();
}

void ExConstraintBlock::Field_20() {
    if (!parent_bone_node)
        return;

    if (field_59) {
        field_59 = false;
        return;
    }

    Calc();
    DataSet();
    field_59 = true;
}

void ExConstraintBlock::SetOsagePlayData() {
    Field_20();
}

void ExConstraintBlock::Disp(mat4* mat, render_context* rctx) {

}

void ExConstraintBlock::Field_40() {

}

void ExConstraintBlock::Field_48() {
    Field_20();
}

void ExConstraintBlock::Field_50() {
    Field_20();
}

void ExConstraintBlock::Calc() {
    bone_node* node = bone_node_ptr;
    if (!node)
        return;

    vec3 pos;
    vec3_mult(parent_bone_node->exp_data.parent_scale, node->exp_data.position, pos);

    mat4 mat;
    mat4_translate_mult(parent_bone_node->ex_data_mat, &pos, &mat);

    switch (constraint_type) {
    case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION: {
        obj_skin_block_constraint_orientation* orientation = cns_data->orientation;

        vec3 trans;
        mat4_get_translation(&mat, &trans);

        mat3 rot;
        mat3_from_mat4(source_node_bone_node->mat, &rot);
        mat3_normalize_rotation(&rot, &rot);
        mat3_rotate_mult(&rot, &orientation->offset, &rot);
        mat4_from_mat3(&rot, node->mat);
        mat4_set_translation(node->mat, &trans);
    } break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION: {
        obj_skin_block_constraint_direction* direction = cns_data->direction;

        vec3 align_axis = direction->align_axis;
        vec3 target_offset = direction->target_offset;
        mat4_mult_vec3_trans(source_node_bone_node->mat, &target_offset, &target_offset);
        mat4_mult_vec3_inv_trans(&mat, &target_offset, &target_offset);
        float_t target_offset_length;
        vec3_length_squared(target_offset, target_offset_length);
        if (target_offset_length <= 0.000001f)
            break;

        mat4 v59;
        sub_1401EB410(&v59, &align_axis, &target_offset);
        if (direction_up_vector_bone_node) {
            vec3 affected_axis = direction->up_vector.affected_axis;
            mat4 v56;
            mat4_mult(&v59, &mat, &v56);
            mat4_mult_vec3(&v56, &affected_axis, &affected_axis);
            mat4* v20 = direction_up_vector_bone_node->mat;

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
    case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION: {
        obj_skin_block_constraint_position* position = cns_data->position;

        vec3 constraining_offset = position->constraining_object.offset;
        vec3 constrained_offset = position->constrained_object.offset;
        if (position->constraining_object.affected_by_orientation)
            mat4_mult_vec3(source_node_bone_node->mat, &constraining_offset, &constraining_offset);

        vec3 source_node_trans;
        mat4_get_translation(source_node_bone_node->mat, &source_node_trans);
        vec3_add(constraining_offset, source_node_trans, source_node_trans);
        mat4_set_translation(&mat, &source_node_trans);
        if (direction_up_vector_bone_node) {
            vec3 up_vector_trans;
            mat4_get_translation(direction_up_vector_bone_node->mat, &up_vector_trans);
            mat4_mult_vec3_inv_trans(&mat, &up_vector_trans, &up_vector_trans);

            mat4 v26;
            sub_1401EB410(&v26, &position->up_vector.affected_axis, &up_vector_trans);
            mat4_mult(&v26, &mat, &mat);
        }
        if (position->constrained_object.affected_by_orientation)
            mat4_mult_vec3(&mat, &constrained_offset, &constrained_offset);

        mat4 constrained_offset_mat;
        mat4_translate(&constrained_offset, &constrained_offset_mat);
        mat4_mult(&mat, &constrained_offset_mat, node->mat);
    } break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
    default:
        *node->mat = mat;
        break;
    }
}

void ExConstraintBlock::DataSet() {
    if (!bone_node_ptr)
        return;

    bone_node_expression_data* exp_data = &bone_node_ptr->exp_data;
    vec3 parent_scale = parent_bone_node->exp_data.parent_scale;

    mat4 mat;
    mat4_inverse_normalized(parent_bone_node->ex_data_mat, &mat);
    mat4_mult(bone_node_ptr->mat, &mat, &mat);
    mat4_get_rotation(&mat, &bone_node_ptr->exp_data.rotation);
    mat4_get_translation(&mat, &exp_data->position);
    if (fabsf(parent_scale.x) > 0.000001f)
        exp_data->position.x /= parent_scale.x;
    if (fabsf(parent_scale.y) > 0.000001f)
        exp_data->position.y /= parent_scale.z;
    if (fabsf(parent_scale.z) > 0.000001f)
        exp_data->position.z /= parent_scale.z;
    *bone_node_ptr->ex_data_mat = *bone_node_ptr->mat;
    mat4_scale_rot(bone_node_ptr->mat, &parent_scale, bone_node_ptr->mat);
    vec3_mult(exp_data->scale, parent_scale, exp_data->parent_scale);
}

void ExConstraintBlock::InitData(rob_chara_item_equip_object* itm_eq_obj,
    obj_skin_block_constraint* cns_data, const char* cns_data_name, bone_database* bone_data) {
    bone_node* node = itm_eq_obj->get_bone_node(cns_data_name, bone_data);
    type = EX_CONSTRAINT;
    bone_node_ptr = node;
    this->cns_data = cns_data;
    name = node->name;
    item_equip_object = itm_eq_obj;

    source_node_bone_node = itm_eq_obj->get_bone_node(cns_data->source_node_name, bone_data);

    obj_skin_block_constraint_type type = cns_data->type;
    const char* up_vector_name;
    if (type == OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION) {
        constraint_type = OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION;
        up_vector_name = cns_data->direction->up_vector.name;
    }
    else if (type == OBJ_SKIN_BLOCK_CONSTRAINT_POSITION) {
        constraint_type = OBJ_SKIN_BLOCK_CONSTRAINT_POSITION;
        up_vector_name = cns_data->position->up_vector.name;
    }
    else if (type == OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE) {
        constraint_type = OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE;
        up_vector_name = cns_data->distance->up_vector.name;
    }
    else if (type == OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION) {
        constraint_type = OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION;
        return;
    }
    else {
        constraint_type = OBJ_SKIN_BLOCK_CONSTRAINT_NONE;
        return;
    }

    if (up_vector_name)
        direction_up_vector_bone_node = itm_eq_obj->get_bone_node(up_vector_name, bone_data);
}

void ExConstraintBlock::sub_1405F10D0(mat4* mat, vec3* a2, float_t a3, float_t a4) {
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
    mat->row3 = { 0.0f, 0.0f, 0.0f, 1.0f };
}

void ExConstraintBlock::sub_1401EB410(mat4* mat, vec3* a2, vec3* target_offset) {
    vec3 v3;
    vec3 v4;
    vec3_normalize(*a2, v3);
    vec3_normalize(*target_offset, v4);
    vec3 v13;
    vec3_cross(v4, v3, v13);

    float_t v18;
    vec3_dot(v3, v4, v18);
    v18 = clamp_def(v18, -1.0f, 1.0f);
    float_t v19 = 1.0f - v18;

    float_t v20 = 1.0f - v18 * v18;
    v20 = sqrtf(clamp_def(v20, 0.0f, 1.0f));

    sub_1405F10D0(mat, &v13, v18, v20);
}

ExExpressionBlock::ExExpressionBlock() : values(), types(), expressions(),
stack_data(), exp_data(), field_3D20(), field_3D28(), frame(), step(true) {

}

ExExpressionBlock::~ExExpressionBlock() {

}

void ExExpressionBlock::Init() {
    frame = 0.0f;
}

void ExExpressionBlock::Field_10() {
    bone_node_expression_data* node_exp_data = &bone_node_ptr->exp_data;
    node_exp_data->position = exp_data->node.position;
    node_exp_data->rotation = exp_data->node.rotation;
    node_exp_data->scale = exp_data->node.scale;
    field_59 = false;
}

void ExExpressionBlock::Field_18(int32_t stage, bool a3) {
    if (field_59)
        return;

    if (stage == 0) {
        if (field_58)
            Field_20();
    }
    else if (stage == 2) {
        if (has_children_node)
            DataSet();
    }
    else if (stage == 5)
        Field_20();
}

void ExExpressionBlock::Field_20() {
    if (!parent_bone_node)
        return;

    if (field_59) {
        field_59 = false;
        return;
    }

    Calc();
    DataSet();
    field_59 = true;
}

void ExExpressionBlock::SetOsagePlayData() {
    Field_20();
}

void ExExpressionBlock::Disp(mat4* mat, render_context* rctx) {

}

void ExExpressionBlock::Field_40() {

}

void ExExpressionBlock::Field_48() {
    Field_20();
}

void ExExpressionBlock::Field_50() {
    Field_20();
}

void ExExpressionBlock::Calc() {
    float_t delta_frame = get_delta_frame();
    if (step)
        delta_frame *= item_equip_object->item_equip->step;
    float_t frame = this->frame + delta_frame;
    this->frame = frame >= 65535.0f ? frame - 65535.0f : frame;

    if (field_3D28) {
        field_3D28(&bone_node_ptr->exp_data);
        return;
    }

    for (int32_t i = 0; i < 9; i++) {
        if (!expressions[i])
            break;

        float_t value = ex_expression_block_stack_get_value(expressions[i]);
        if (types[i] == EX_EXPRESSION_BLOCK_STACK_VARIABLE_RADIAN)
            value *= DEG_TO_RAD_FLOAT;
        *values[i] = value;
    }
}

void ExExpressionBlock::DataSet() {
    bone_node_expression_data* data = &bone_node_ptr->exp_data;
    vec3 parent_scale = parent_bone_node->exp_data.parent_scale;
    mat4 ex_data_mat = *parent_bone_node->ex_data_mat;
    mat4 mat = mat4_identity;
    data->mat_set(parent_scale, ex_data_mat, mat);
    *bone_node_ptr->mat = mat;
    *bone_node_ptr->ex_data_mat = ex_data_mat;
}

void ExExpressionBlock::InitData(rob_chara_item_equip_object* itm_eq_obj,
    obj_skin_block_expression* exp_data, const char* exp_data_name, object_info a4,
    size_t index, bone_database* bone_data) {
    ex_expression_block_stack* stack_buf[28];
    ex_expression_block_stack** stack_buf_val = stack_buf;

    bone_node* node = itm_eq_obj->get_bone_node(exp_data_name, bone_data);
    type = EX_EXPRESSION;
    bone_node_ptr = node;
    this->exp_data = exp_data;
    name = node->name;
    item_equip_object = itm_eq_obj;

    node->exp_data.position = exp_data->node.position;
    node->exp_data.rotation = exp_data->node.rotation;
    node->exp_data.scale = exp_data->node.scale;
    field_3D28 = 0;

    ex_expression_block_stack* stack_val = stack_data;
    for (int32_t i = 0; i < 9; i++) {
        values[i] = 0;
        expressions[i] = 0;
    }

    for (int32_t i = 0; i < 9; i++) {
        const char* expression = exp_data->expression_array[i];
        if (!expression || str_utils_compare_length(expression, utf8_length(expression), "= ", 2))
            break;

        expression += 2;

        int32_t index = 0;
        expression = str_utils_get_next_int32_t(expression, index, ' ');
        values[i] = bone_node_ptr->get_exp_data_component(index, types[i]);

        while (expression) {
            std::string value_type;
            expression = str_utils_get_next_string(expression, value_type, ' ');
            if (!value_type.size() || !memcmp(value_type.c_str(), "error",
                min_def(value_type.size(), 5)) && value_type.size() == 5)
                break;

            if (value_type[0] == 'n') {
                stack_val->type = EX_EXPRESSION_BLOCK_STACK_NUMBER;
                expression = str_utils_get_next_float_t(expression, stack_val->number.value, ' ');
                *stack_buf_val++ = stack_val;
            }
            else if (value_type[0] == 'v') {
                std::string func_str;
                expression = str_utils_get_next_string(expression, func_str, ' ');
                stack_val->type = EX_EXPRESSION_BLOCK_STACK_VARIABLE;
                int32_t v40 = func_str[0] - '0';
                if (v40 < 9) {
                    bone_node* v42 = itm_eq_obj->get_bone_node(func_str.c_str() + 2, bone_data);
                    if (v42)
                        stack_val->var.value = v42->get_exp_data_component(v40, stack_val->type);
                    else {
                        stack_val->type = EX_EXPRESSION_BLOCK_STACK_NUMBER;
                        stack_val->number.value = 0.0f;
                    }
                }
                else
                    stack_val->var.value = &frame;
                *stack_buf_val++ = stack_val;
            }
            else if (value_type[0] == 'f') {
                std::string func_str;
                expression = str_utils_get_next_string(expression, func_str, ' ');
                stack_val->type = EX_EXPRESSION_BLOCK_STACK_OP1;
                stack_val->op1.v1 = stack_buf_val[-1];
                stack_val->op1.func = exp_func_op1_find_func(func_str, exp_func_op1_array)->func;
                stack_buf_val[-1] = stack_val;
            }
            else if (value_type[0] == 'g') {
                std::string func_str;
                expression = str_utils_get_next_string(expression, func_str, ' ');
                stack_val->type = EX_EXPRESSION_BLOCK_STACK_OP2;
                stack_val->op2.v1 = stack_buf_val[-2];
                stack_val->op2.v2 = stack_buf_val[-1];
                stack_val->op2.func = exp_func_op2_find_func(func_str, exp_func_op2_array)->func;
                stack_buf_val[-2] = stack_val;
                stack_buf_val--;
            }
            else if (value_type[0] == 'h') {
                std::string func_str;
                expression = str_utils_get_next_string(expression, func_str, ' ');
                stack_val->type = EX_EXPRESSION_BLOCK_STACK_OP3;
                stack_val->op3.v1 = stack_buf_val[-3];
                stack_val->op3.v2 = stack_buf_val[-2];
                stack_val->op3.v3 = stack_buf_val[-1];
                stack_val->op3.func = exp_func_op3_find_func(func_str, exp_func_op3_array)->func;
                stack_buf_val[-3] = stack_val;
                stack_buf_val -= 2;
            }
            expressions[i] = stack_buf_val[-1];
            stack_val++;
        }
    }

    step = !((ssize_t)index - ITEM_TE_R <= 1);
}

void osage_setting_data_init() {
    osage_setting_data.cat = {};
    osage_setting_data.obj = {};
}

bool osage_setting_data_load_file(void* data, const char* path, const char* file, uint32_t hash) {
    key_val kv;
    if (key_val::load_file(&kv, path, file, hash)) {
        osage_setting_data.parse(&kv);
        return true;
    }
    return false;
}

bool osage_setting_data_obj_has_key(object_info key) {
    return osage_setting_data.obj.find(key) != osage_setting_data.obj.end();
}

void osage_setting_data_free() {
    osage_setting_data.cat.clear();
    osage_setting_data.obj.clear();
}

void skin_param_osage_node_parse(void* kv, const char* name,
    std::vector<skin_param_osage_node>* vec, skin_param_osage_root& skp_root) {
    key_val* _kv = (key_val*)kv;
    if (!_kv->open_scope(name))
        return;

    if (!_kv->open_scope("node")) {
        _kv->close_scope();
        return;
    }

    int32_t count;
    if (_kv->read("length", count)) {
        size_t c = vec->size();
        if (c < count)
            vec->resize(count);
    }
    else
        vec->clear();

    int32_t j = 0;
    for (auto i = vec->begin(); i != vec->end(); i++, j++) {
        if (!_kv->open_scope_fmt(j))
            continue;

        *i = skin_param_osage_node();

        float_t coli_r = 0.0f;
        if (_kv->read("coli_r", coli_r))
            i->coli_r = coli_r;

        float_t weight = 0.0f;
        if (_kv->read("weight", weight))
            i->weight = weight;

        float_t inertial_cancel = 0.0f;
        if (_kv->read("inertial_cancel", inertial_cancel))
            i->inertial_cancel = inertial_cancel;

        i->hinge.ymin = -skp_root.hinge_y;
        i->hinge.ymax = skp_root.hinge_y;
        i->hinge.zmin = -skp_root.hinge_z;
        i->hinge.zmax = skp_root.hinge_z;

        float_t hinge_ymax = 0.0f;
        if (_kv->read("hinge_ymax", hinge_ymax))
            i->hinge.ymax = hinge_ymax;

        float_t hinge_ymin = 0.0f;
        if (_kv->read("hinge_ymin", hinge_ymin))
            i->hinge.ymin = hinge_ymin;

        float_t hinge_zmax = 0.0f;
        if (_kv->read("hinge_zmax", hinge_zmax))
            i->hinge.zmax = hinge_zmax;

        float_t hinge_zmin = 0.0f;
        if (_kv->read("hinge_zmin", hinge_zmin))
            i->hinge.zmin = hinge_zmin;
        _kv->close_scope();
    }

    _kv->close_scope();
    _kv->close_scope();
}

void skin_param_osage_root_parse(void* kv, const char* name,
    skin_param_osage_root& skp_root, bone_database* bone_data) {
    key_val* _kv = (key_val*)kv;
    if (!_kv->open_scope(name))
        return;

    int32_t node_length = 0;
    _kv->read("node.length", node_length);

    if (!_kv->open_scope("root")) {
        _kv->close_scope();
        return;
    }

    float_t force = 0.0f;
    float_t force_gain = 0.0f;
    if (_kv->read("force", force)
        && _kv->read("force_gain", force_gain)) {
        skp_root.force = force;
        skp_root.force_gain = force_gain;
    }

    float_t air_res = 0.0f;
    if (_kv->read("air_res", air_res))
        skp_root.air_res = min_def(air_res, 0.9f);

    float_t rot_y = 0.0f;
    float_t rot_z = 0.0f;
    if (_kv->read("rot_y", rot_y)
        && _kv->read("rot_z", rot_z)) {
        skp_root.rot_y = rot_y;
        skp_root.rot_z = rot_z;
    }

    float_t friction = 0.0f;
    if (_kv->read("friction", friction))
        skp_root.friction = friction;

    float_t wind_afc = 0;
    if (_kv->read("wind_afc", wind_afc)) {
        skp_root.wind_afc = wind_afc;
        if (!str_utils_compare_length(name, utf8_length(name), "c_opa", 5))
            skp_root.wind_afc = 0.0f;
    }

    int32_t coli_type = 0;
    if (_kv->read("coli_type", coli_type))
        skp_root.coli_type = coli_type;

    float_t init_rot_y = 0.0f;
    float_t init_rot_z = 0.0f;
    if (_kv->read("init_rot_y", init_rot_y)
        && _kv->read("init_rot_z", init_rot_z)) {
        skp_root.init_rot_y = init_rot_y;
        skp_root.init_rot_z = init_rot_z;
    }

    float_t hinge_y = 0.0f;
    float_t hinge_z = 0.0f;
    if (_kv->read("hinge_y", hinge_y)
        && _kv->read("hinge_z", hinge_z)) {
        skp_root.hinge_y = hinge_y;
        skp_root.hinge_z = hinge_z;
    }

    float_t coli_r = 0.0f;
    if (_kv->read("coli_r", coli_r))
        skp_root.coli_r = coli_r;

    float_t stiffness = 0.0f;
    if (_kv->read("stiffness", stiffness))
        skp_root.stiffness = stiffness;

    float_t move_cancel = 0.0f;
    if (_kv->read("move_cancel", move_cancel))
        skp_root.move_cancel = move_cancel;

    int32_t count;
    if (_kv->read("coli", "length", count)) {
        std::vector<SkinParam::CollisionParam>& vc = skp_root.coli;
        for (int32_t i = 0; i < count; i++) {
            if (!_kv->open_scope_fmt(i))
                continue;

            SkinParam::CollisionParam* c = &vc[i];

            int32_t type = 0;
            if (!_kv->read("type", type)) {
                _kv->close_scope();
                break;
            }
            c->type = (SkinParam::CollisionType)type;

            float_t radius = 0;
            if (!_kv->read("radius", radius)) {
                _kv->close_scope();
                break;
            }
            c->radius = radius;

            const char* bone0_name;
            if (!_kv->read("bone.0.name", bone0_name)) {
                _kv->close_scope();
                break;
            }

            c->node_idx[0] = bone_data->get_skeleton_object_bone_index(
                bone_database_skeleton_type_to_string(BONE_DATABASE_SKELETON_COMMON), bone0_name);

            vec3 bone0_pos = 0.0f;
            if (!_kv->read("bone.0.posx", bone0_pos.x)
                || !_kv->read("bone.0.posy", bone0_pos.y)
                || !_kv->read("bone.0.posz", bone0_pos.z)) {
                _kv->close_scope();
                break;
            }

            c->pos[0] = bone0_pos;

            const char* bone1_name;
            if (_kv->read("bone.1.name", bone1_name)) {
                c->node_idx[1] = bone_data->get_skeleton_object_bone_index(
                    bone_database_skeleton_type_to_string(BONE_DATABASE_SKELETON_COMMON), bone1_name);

                vec3 bone1_pos = 0.0f;
                if (!_kv->read("bone.1.posx", bone1_pos.x)
                    || !_kv->read("bone.1.posy", bone1_pos.y)
                    || !_kv->read("bone.1.posz", bone1_pos.z)) {
                    _kv->close_scope();
                    break;
                }

                c->pos[1] = bone1_pos;
            }
            _kv->close_scope();
        }
        _kv->close_scope();
    }

    if (_kv->read("boc", "length", count)) {
        std::vector<skin_param_osage_root_boc>* vb = &skp_root.boc;

        vb->resize(count);
        for (int32_t i = 0; i < count; i++) {
            if (!_kv->open_scope_fmt(i))
                continue;

            int32_t st_node = 0;
            std::string ed_root;
            int32_t ed_node = 0;
            if (_kv->read("st_node", st_node)
                && st_node < node_length
                && _kv->read("ed_root", ed_root)
                && _kv->read("ed_node", ed_node)
                && ed_node < node_length) {
                skin_param_osage_root_boc b;
                b.st_node = st_node;
                b.ed_root.assign(ed_root);
                b.ed_node = ed_node;
                vb->push_back(b);
            }
            _kv->close_scope();
        }
        _kv->close_scope();
    }

    std::string colli_tgt_osg;
    if (_kv->read("colli_tgt_osg", colli_tgt_osg))
        skp_root.colli_tgt_osg.assign(colli_tgt_osg);

    if (_kv->read("normal_ref", "length", count)) {
        std::vector<skin_param_osage_root_normal_ref>* vnr = &skp_root.normal_ref;

        vnr->resize(count);
        for (int32_t i = 0; i < count; i++) {
            if (!_kv->open_scope_fmt(i))
                continue;

            std::string n;
            if (_kv->read("N", n)) {
                skin_param_osage_root_normal_ref nr;
                nr.n.assign(n);
                _kv->read("U", nr.u);
                _kv->read("D", nr.d);
                _kv->read("L", nr.l);
                _kv->read("R", nr.r);
                vnr->push_back(nr);
            }
            _kv->close_scope();
        }
        _kv->close_scope();
    }

    _kv->close_scope();
    _kv->close_scope();
}

osage_setting::osage_setting() {

}

osage_setting::~osage_setting() {

}

void osage_setting::parse(key_val* kv) {
    int32_t count;
    if (kv->read("cat", "length", count)) {
        for (int32_t i = 0; i < count; i++) {
            if (!kv->open_scope_fmt(i))
                continue;

            std::string name;
            kv->read("name", name);

            int32_t count;
            if (kv->read("osg", "length", count)) {
                for (int32_t j = 0; j < count; j++) {
                    if (!kv->open_scope_fmt(j))
                        continue;

                    int32_t exf = 0;
                    kv->read("exf", exf);

                    std::string parts;
                    kv->read("parts", parts);

                    std::string root;
                    kv->read("root", root);

                    std::pair<std::string, std::string> key = { name, root };
                    auto elem = cat.find(key);
                    if (elem == cat.end())
                        elem = cat.insert({ key, {} }).first;

                    elem->second.exf = exf;
                    elem->second.parts = parse_parts_string(parts);
                    kv->close_scope();
                }
                kv->close_scope();
            }
            kv->close_scope();
        }
        kv->close_scope();
    }

    data_struct* aft_data = &data_list[DATA_AFT];
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    if (kv->read("obj", "length", count)) {
        for (int32_t i = 0; i < count; i++) {
            if (!kv->open_scope_fmt(i))
                continue;

            std::string cat;
            kv->read("cat", cat);

            std::string name;
            kv->read("name", name);

            obj.insert_or_assign(aft_obj_db->get_object_info(name.c_str()), cat);
            kv->close_scope();
        }
        kv->close_scope();
    }
}

rob_osage_parts osage_setting::parse_parts_string(std::string& s) {
    static const char* parts_string_array[] = {
        "LEFT", "RIGHT", "CENTER", "LONG_C", "SHORT_L", "SHORT_R",
        "APPEND_L", "APPEND_R", "MUFFLER", "WHITE_ONE_L", "PONY", "ANGEL_L", "ANGEL_R",
    };

    for (const char*& i : parts_string_array)
        if (!s.compare(i))
            return (rob_osage_parts)(&i - parts_string_array);
    return ROB_OSAGE_PARTS_NONE;
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

static const exp_func_op1* exp_func_op1_find_func(std::string& name, const exp_func_op1* array) {
    const char* name_str = name.c_str();
    while (array->name) {
        if (!str_utils_compare(name_str, array->name))
            return array;
        array++;
    }
    return 0;
}

static const exp_func_op2* exp_func_op2_find_func(std::string& name, const exp_func_op2* array) {
    const char* name_str = name.c_str();
    while (array->name) {
        if (!str_utils_compare(name_str, array->name))
            return array;
        array++;
    }
    return 0;
}

static const exp_func_op3* exp_func_op3_find_func(std::string& name, const exp_func_op3* array) {
    const char* name_str = name.c_str();
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
    return max_def(v1, v2);
}

static float_t exp_min(float_t v1, float_t v2) {
    return min_def(v1, v2);
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
    return v1 != 0.0f || v2 != 0.0f ? 1.0f : 0.0f;
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

// 0x140484850
static void closest_pt_segment_segment(vec3& vec, const vec3& p0, const vec3& p1, const OsageCollision::Work* cls) {
    vec3 v56 = p1 - p0;
    vec3 v60 = vec3::cross(v56, cls->vec_center);
    float_t v46 = vec3::length_squared(v60);
    if (v46 <= 0.000001f) {
        vec3 v56;
        vec3 v57;
        vec3 v58;
        vec3 v59;
        OsageCollision::get_nearest_line2point(v59, cls->pos[0], cls->pos[1], p0);
        OsageCollision::get_nearest_line2point(v58, cls->pos[0], cls->pos[1], p1);
        OsageCollision::get_nearest_line2point(v57, p0, p1, cls->pos[0]);
        OsageCollision::get_nearest_line2point(v56, p0, p1, cls->pos[1]);

        float_t v48 = vec3::distance_squared(p1, cls->pos[0]);
        float_t v51 = vec3::distance_squared(cls->pos[0], v57);
        float_t v52 = vec3::distance_squared(cls->pos[1], v56);
        float_t v55 = vec3::distance_squared(p0, v59);

        vec = p0;

        if (v55 > v48) {
            vec = p1;
            v55 = v48;
        }

        if (v55 > v51) {
            vec = v57;
            v55 = v51;
        }

        if (v55 > v52)
            vec = v56;
    }
    else {
        float_t v25 = vec3::length(v56);
        if (v25 != 0.0f)
            v56 *= 1.0f / v25;

        float_t v30 = 1.0f / cls->vec_center_length;
        vec3 v61 = cls->vec_center * v30;
        float_t v34 = vec3::dot(v61, v56);
        float_t v35 = vec3::dot(v61 * v34 - v56, cls->pos[0] - p0)
            / (v34 * v34 - 1.0f);
        if (v35 < 0.0f)
            vec = p0;
        else if (v35 <= v25)
            vec = p0 + v56 * v35;
        else
            vec = p1;

    }
}

static const osage_setting_osg_cat* osage_setting_data_get_cat_value(
    object_info* obj_info, const char* root_node) {
    auto elem_obj = osage_setting_data.obj.find(*obj_info);
    if (elem_obj == osage_setting_data.obj.end())
        return 0;

    std::pair<std::string, std::string> key = { elem_obj->second, root_node };
    auto elem_cat = osage_setting_data.cat.find(key);
    if (elem_cat == osage_setting_data.cat.end())
        return 0;

    return &elem_cat->second;
}

static void sub_140218560(RobCloth* rob_cls, float_t step, bool a3) {
    sub_140219940(rob_cls);
    if (rob_cls->osage_reset) {
        sub_1402196D0(rob_cls);
        rob_cls->osage_reset = false;
    }

    if (rob_cls->move_cancel > 0.0f) {
        size_t root_count = rob_cls->root_count;
        size_t nodes_count = rob_cls->nodes_count;

        float_t move_cancel = rob_cls->move_cancel;
        for (size_t i = 0; i < root_count; i++) {
            mat4 mat = rob_cls->root.data()[i].field_D8;
            CLOTHNode* node = &rob_cls->nodes.data()[i + root_count];
            for (size_t j = 1; j < nodes_count; j++, node += root_count) {
                vec3 v19;
                mat4_mult_vec3_trans(&mat, &node->reset_data.trans, &v19);
                node->trans += (v19 - node->trans) * move_cancel;
            }
        }
    }

    if (step > 0.0f) {
        sub_1402187D0(rob_cls, a3);
        sub_140219D10(rob_cls);
        sub_14021AA60(rob_cls, step, false);

        if (!a3) {
            rob_cls->set_external_force = false;
            rob_cls->external_force = 0.0f;
        }
    }
}

static void sub_1402187D0(RobCloth* rob_cls, bool a2) {
    float_t v7 = (1.0f - rob_cls->field_44) * (1.0f - rob_cls->skin_param_ptr->air_res);

    float_t osage_gravity = get_osage_gravity_const();

    vec3 external_force;
    vec3_mult_scalar(rob_cls->wind_direction, rob_cls->skin_param_ptr->wind_afc, external_force);
    if (rob_cls->set_external_force) {
        vec3_add(external_force, rob_cls->external_force, external_force);
        osage_gravity = 0.0f;
    }

    size_t root_count = rob_cls->root_count;
    size_t nodes_count = rob_cls->nodes_count;

    float_t force = rob_cls->skin_param_ptr->force;
    CLOTHNode* node = &rob_cls->nodes.data()[root_count];
    for (size_t i = 1; i < nodes_count; i++) {
        RobClothRoot* root = rob_cls->root.data();
        for (size_t j = 0; j < root_count; j++, root++, node++) {
            mat4 mat = root->field_98;

            vec3 v37;
            mat4_mult_vec3(&mat, &node->field_64, &v37);

            float_t v25;
            if (!a2)
                v25 = v7;
            else if (node->trans_diff.y >= 0.0f)
                v25 = 1.0f;
            else
                v25 = 0.0f;

            vec3_mult_scalar(v37, force, v37);

            vec3 v38;
            vec3_mult_scalar(node->trans_diff, v25, v38);

            vec3_sub(v37, v38, v37);
            vec3_add(v37, external_force, v37);
            v37.y -= osage_gravity;
            vec3_add(node->trans_diff, v37, node->trans_diff);

            node->field_1C = node->trans;
            vec3_add(node->trans, node->trans_diff, node->trans);
        }
        force *= rob_cls->skin_param_ptr->force_gain;
    }
}

static void sub_1402196D0(RobCloth* rob_cls) {
    size_t root_count = rob_cls->root_count;
    size_t nodes_count = rob_cls->nodes_count;

    if (rob_cls->reset_data_list) {
        std::list<RobOsageNodeResetData>::iterator reset_data_list = rob_cls->reset_data_list->begin();
        CLOTHNode* i_begin = rob_cls->nodes.data() + root_count;
        CLOTHNode* i_end = rob_cls->nodes.data() + rob_cls->nodes.size();
        for (CLOTHNode* i = i_begin; i != i_end; i++)
            i->reset_data = *reset_data_list++;
        rob_cls->reset_data_list = 0;
    }


    RobClothRoot* root = rob_cls->root.data();
    for (size_t i = 0; i < root_count; i++, root++) {
        CLOTHNode* node = &rob_cls->nodes.data()[i + root_count];
        for (size_t j = 1; j < nodes_count; j++, node += root_count) {
            mat4_mult_vec3_trans(&root->field_D8, &node->reset_data.trans, &node->trans);
            mat4_mult_vec3(&root->field_D8, &node->reset_data.trans_diff, &node->trans_diff);
        }
    }
}

static void sub_140219940(RobCloth* rob_cls) {
    size_t root_count = rob_cls->root_count;

    for (size_t i = 0; i < root_count; i++) {
        RobClothRoot& root = rob_cls->root.data()[i];
        CLOTHNode& root_node = rob_cls->nodes.data()[i];

        mat4 m = mat4_null;
        for (int32_t j = 0; j < 4; j++) {
            if (!root.mat[j] || !root.node_mat[j])
                continue;

            mat4 mat;
            mat4_mult(root.mat[j], root.node_mat[j], &mat);

            float_t weight = root.weight[j];
            mat4_scale_mult(&mat, weight, weight, weight, weight, &mat);
            mat4_add(&m, &mat, &m);
        }
        root.field_98 = m;

        mat4_mult_vec3_trans(&m, &root_node.trans_orig, &root_node.trans);
        mat4_mult_vec3(&m, &root.normal, &root_node.normal);
        mat4_mult_vec3(&m, (vec3*)&root.tangent, &root_node.tangent);
        root_node.tangent_sign = root.tangent.w;
        root_node.field_1C = root_node.trans;

        mat4_translate_mult(&m, &root_node.trans_orig, &m);
        root.field_D8 = m;
        mat4_inverse(&m, &m);
        root.field_118 = m;
    }
}

static void sub_140219D10(RobCloth* rob_cls) {
    CLOTHNode* node = rob_cls->nodes.data();
    ssize_t root_count = rob_cls->root_count;
    size_t nodes_count = rob_cls->nodes_count;

    for (size_t i = nodes_count - 2; i; i--) {
        CLOTHNode* v7 = &node[root_count * i];
        for (ssize_t j = 0; j < root_count; j++, v7++)
            sub_140482F30(&v7->trans, &v7[root_count].trans, v7->dist_bottom);
    }

    if (nodes_count <= 1)
        return;

    vec3* v11 = (vec3*)operator new(sizeof(vec3) * root_count);
    vec3* v12 = (vec3*)operator new(sizeof(vec3) * root_count);
    memset(v11, 0, sizeof(vec3) * root_count);
    memset(v12, 0, sizeof(vec3) * root_count);

    vec3* v13 = &v11[root_count - 2];
    vec3* v14 = v12 + 1;
    CLOTHNode* v15 = &node[root_count];
    CLOTHNode* v16 = &node[2 * root_count - 1];
    for (size_t i = nodes_count - 1; i; i--) {
        if (root_count) {
            vec3* v17 = v12;
            vec3* v17a = v11;
            CLOTHNode* v18 = v15;
            for (ssize_t j = root_count; j > 0; j--, v17++, v17a++, v18++) {
                *v17 = v18->trans;
                *v17a = v18->trans;
            }
        }

        if (root_count - 2 >= 0) {
            vec3* v20 = v13;
            CLOTHNode* v22 = v16 - 1;
            for (ssize_t j = root_count - 1; j > 0; j--, v20--, v22--)
                sub_140482F30(v20, v20 + 1, v22->dist_left);
            v14 = v12 + 1;
        }

        if (rob_cls->field_8 & 0x04)
            sub_140482F30(&v11[root_count - 1], v11, v16->dist_right);

        if (root_count > 1) {
            vec3* v23 = v14;
            CLOTHNode* v24 = v15 + 1;
            for (ssize_t j = root_count - 1; j > 0; j--, v23++, v24++)
                sub_140482F30(v23, v23 - 1, v24->dist_right);
            v14 = v12 + 1;
        }

        if (rob_cls->field_8 & 0x04)
            sub_140482F30(v12, &v12[root_count - 1], v15->dist_left);

        if (root_count > 0) {
            vec3* v27 = v11;
            vec3* v28 = v12;
            CLOTHNode* v36 = v15;
            for (ssize_t j = root_count; j > 0; j--, v27++, v28++, v36++) {
                vec3 v44;
                vec3_add(*v27, *v28, v44);
                vec3_mult_scalar(v44, 0.5f, v36->trans);
            }
        }
        v16 += root_count;
        v15 += root_count;
    }

    operator delete(v11);
    operator delete(v12);
}

static float_t sub_14021A290(vec3& a1, vec3& a2, vec3& a3, vec2& a4, vec2& a5, vec2& a6,
    vec3& tangent, vec3& binormal, vec3& normal) {
    vec3 v11;
    vec3_sub(a3, a1, v11);

    vec3 v15;
    vec3_sub(a2, a1, v15);

    vec3_cross(v15, v11, normal);

    float_t v12;
    vec3_length(normal, v12);
    if (v12 <= 0.000001)
        return 1.0f;

    vec3_normalize(normal, normal);

    vec2 v37;
    vec2 v38;
    vec2_sub(a5, a4, v37);
    vec2_sub(a6, a4, v38);

    float_t v19 = 1.0f / (v38.y * v37.x - v37.y * v38.x);

    vec3_mult_scalar(v15, v38.y, v15);
    vec3_mult_scalar(v11, v37.y, v11);

    vec3 v20;
    vec3_sub(v15, v11, v20);
    vec3_mult_scalar(v20, v19, v20);
    vec3_normalize(v20, v20);

    vec3_cross(normal, v20, binormal);
    vec3_cross(normal, binormal, tangent);

    mat3 mat;
    mat.row0 = tangent;
    mat.row1 = binormal;
    mat.row2 = normal;
    mat3_transpose(&mat, &mat);
    return mat3_determinant(&mat);
}

static float_t sub_14021A5E0(vec3& a1, vec3& a2, float_t a3, float_t a4, vec3& tangent, vec3& binormal, vec3& normal) {
    vec3_cross(a1, a2, normal);
    vec3_normalize(normal, normal);

    vec3 v16;
    vec3 v17;
    vec3_mult_scalar(a1, a4, v16);
    vec3_mult_scalar(a2, a3, v17);

    vec3 v18;
    vec3_sub(v16, v17, v18);

    vec3_normalize(v18, v18);

    vec3_cross(normal, v18, binormal);
    vec3_cross(normal, binormal, tangent);

    mat3 mat;
    mat.row0 = tangent;
    mat.row1 = binormal;
    mat.row2 = normal;
    mat3_transpose(&mat, &mat);
    return mat3_determinant(&mat);
}

static void sub_14021A890(CLOTHNode* a1, CLOTHNode* a2, CLOTHNode* a3, CLOTHNode* a4, CLOTHNode* a5) {
    vec3 v18;
    vec3_sub(a4->trans, a5->trans, v18);

    vec3 v17;
    vec3_sub(a3->trans, a2->trans, v17);

    float_t v5;
    float_t v7;
    vec3_length(v18, v5);
    vec3_length(v17, v7);
    if (v5 <= 0.000001f || v7 <= 0.000001f)
        return;

    vec2 v19;
    vec2_sub(a4->texcoord, a5->texcoord, v19);

    vec2 v20;
    vec2_sub(a3->texcoord, a2->texcoord, v20);

    float_t v14 = v19.x * v20.y - v19.y * v20.x;
    if (fabsf(v14) > 0.000001f) {
        v14 = 1.0f / v14;
        a1->tangent_sign = sub_14021A5E0(v18, v17, v19.y * v14, v20.y * v14,
            a1->tangent, a1->binormal, a1->normal);
    }
}

void sub_14021AA60(RobCloth* rob_cls, float_t step, bool a3) {
    ssize_t root_count = rob_cls->root_count;
    size_t nodes_count = rob_cls->nodes_count;

    float_t v10 = 1.0f / step;

    RobClothRoot* root = rob_cls->root.data();
    CLOTHNode* node = &rob_cls->nodes.data()[root_count];
    float_t coli_r = rob_cls->skin_param_ptr->coli_r;
    float_t ring_height;
    if (node->trans.x < rob_cls->ring.ring_rectangle_x - coli_r
        || node->trans.z < rob_cls->ring.ring_rectangle_y - coli_r
        || node->trans.x > rob_cls->ring.ring_rectangle_x + rob_cls->ring.ring_rectangle_width
        || node->trans.z > rob_cls->ring.ring_rectangle_y + rob_cls->ring.ring_rectangle_height)
        ring_height = rob_cls->ring.ring_out_height;
    else
        ring_height = rob_cls->ring.ring_height;

    float_t v19 = ring_height + coli_r;

    for (size_t i = 1; i < nodes_count; i++) {
        for (ssize_t j = 0; j < root_count; j++, node++) {
            float_t fric = (1.0f - rob_cls->field_44) * rob_cls->skin_param_ptr->friction;
            if (step != 1.0f) {
                vec3 v60;
                vec3_sub(node->trans, node->field_1C, v60);

                float_t v30;
                vec3_length(v60, v30);
                if (v30 * step > 0.0f && v30 != 0.0f)
                    vec3_mult_scalar(v60, 1.0f / v30, v60);

                vec3_mult_scalar(v60, v30 * step, v60);
                vec3_add(node->field_1C, v60, node->trans);
            }
            sub_140482F30(&node[0].trans, &node[-root_count].trans, node[0].dist_top);

            int32_t v39 = OsageCollision::osage_cls_work_list(node->trans,
                rob_cls->skin_param_ptr->coli_r, rob_cls->ring.coli, &fric);
            v39 += rob_cls->coli_ring->osage_cls(node->trans, rob_cls->skin_param_ptr->coli_r);
            v39 += rob_cls->coli->osage_cls(node->trans, rob_cls->skin_param_ptr->coli_r);

            if (v19 > node->trans.y && v19 < 1001.0) {
                node->trans.y = v19;
                node->trans_diff = 0.0f;
            }

            mat4 mat = root->field_98;
            mat4_set_translation(&mat, &node[-root_count].trans);

            int32_t yz_order = 1;
            sub_140482FF0(mat, node->field_64, 0, 0, yz_order);

            vec3 v74;
            mat4_mult_vec3_inv_trans(&mat, &node->trans, &v74);

            sub_140482FF0(mat, v74, &rob_cls->skin_param_ptr->hinge,
                &node->reset_data.rotation, yz_order);
            mat4_translate_mult(&mat, node->dist_top, 0.0f, 0.0f, &mat);
            mat4_get_translation(&mat, &node->trans);

            if (v39)
                vec3_mult_scalar(node->trans_diff, fric, node->trans_diff);

            vec3 v44;
            vec3_sub(node->trans, node->field_1C, v44);
            vec3_mult_scalar(v44, v10, node->trans_diff);

            if (!a3) {
                mat4& v49 = rob_cls->root.data()[j].field_118;
                mat4_mult_vec3_trans(&v49, &node->trans, &node->reset_data.trans);
                mat4_mult_vec3(&v49, &node->trans_diff, &node->reset_data.trans_diff);
            }
        }
    }
}

static void sub_14021D480(RobCloth* rob_cls) {
    sub_140219940(rob_cls);

    const float_t osage_gravity_const = get_osage_gravity_const();

    ssize_t root_count = rob_cls->root_count;
    size_t nodes_count = rob_cls->nodes_count;

    CLOTHNode* node = &rob_cls->nodes.data()[root_count];
    float_t coli_r = rob_cls->skin_param_ptr->coli_r;
    float_t ring_height;
    if (node->trans.x < rob_cls->ring.ring_rectangle_x - coli_r
        || node->trans.z < rob_cls->ring.ring_rectangle_y - coli_r
        || node->trans.x > rob_cls->ring.ring_rectangle_x + rob_cls->ring.ring_rectangle_width
        || node->trans.z > rob_cls->ring.ring_rectangle_y + rob_cls->ring.ring_rectangle_height)
        ring_height = rob_cls->ring.ring_out_height;
    else
        ring_height = rob_cls->ring.ring_height;

    float_t v10 = ring_height + coli_r;

    for (size_t i = 1; i < nodes_count; i++) {
        RobClothRoot* root = rob_cls->root.data();
        for (ssize_t j = 0; j < root_count; j++, root++, node++) {
            mat4 mat = root->field_98;

            vec3 v38;
            mat4_mult_vec3(&mat, &node->field_64, &v38);
            v38.y -= osage_gravity_const;

            vec3_normalize(v38, v38);
            vec3_mult_scalar(v38, node->dist_top, v38);
            vec3_add(node[-root_count].trans, v38, node[0].trans);

            OsageCollision::osage_cls_work_list(node->trans,
                rob_cls->skin_param_ptr->coli_r, rob_cls->ring.coli);
            rob_cls->coli_ring->osage_cls(node->trans, rob_cls->skin_param_ptr->coli_r);
            rob_cls->coli->osage_cls(node->trans, rob_cls->skin_param_ptr->coli_r);

            if (v10 > node->trans.y && v10 < 1001.0f)
                node->trans.y = v10;

            node->trans_diff = 0.0f;
            node->field_1C = node->trans;
        }
    }

    for (size_t i = 0; i < qword_140FBDF88; ++i)
        sub_14021DC60(rob_cls, 1.0f);

    node = &rob_cls->nodes.data()[root_count];
    for (size_t i = 1; i < nodes_count; i++)
        for (ssize_t j = 0; j < root_count; j++, node++)
            node->trans_diff = 0.0f;
}

static void sub_14021DC60(RobCloth* rob_cls, float_t step) {
    if (step <= 0.0f)
        return;

    sub_140219940(rob_cls);
    sub_1402187D0(rob_cls, true);
    sub_140219D10(rob_cls);
    sub_14021AA60(rob_cls, step, true);
}

static void sub_14021D840(RobCloth* rob_cls) {
    sub_140218560(rob_cls, 1.0f, true);
}

static void sub_14047C800(RobOsage* rob_osg, mat4* mat, vec3* parent_scale,
    float_t step, bool disable_wind, bool ring_coli, bool has_children_node) {
    if (!rob_osg->nodes.size())
        return;

    const float_t osage_gravity_const = get_osage_gravity_const();
    sub_1404803B0(rob_osg, mat, parent_scale, false);

    RobOsageNode* v17 = &rob_osg->nodes.data()[0];
    v17->trans_orig = v17->trans;
    vec3 v113;
    vec3_mult(rob_osg->exp_data.position, *parent_scale, v113);

    mat4 v130 = *mat;
    mat4_mult_vec3_trans(&v130, &v113, &v17->trans);
    vec3_sub(v17->trans, v17->trans_orig, v17->trans_diff);
    sub_14047F110(rob_osg, &v130, parent_scale, false);
    *rob_osg->nodes.data()[0].bone_node_mat = v130;
    *rob_osg->nodes.data()[0].bone_node_ptr->ex_data_mat = v130;

    v113 = { 1.0f, 0.0f, 0.0f };
    vec3 v128;
    mat4_mult_vec3(&v130, &v113, &v128);

    float_t v25 = parent_scale->x;
    if (!rob_osg->osage_reset && step <= 0.0f)
        return;

    bool stiffness = rob_osg->skin_param_ptr->stiffness > 0.0f;

    RobOsageNode* v30 = rob_osg->nodes.data();
    RobOsageNode* v26_begin = rob_osg->nodes.data() + 1;
    RobOsageNode* v26_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (RobOsageNode* v26 = v26_begin; v26 != v26_end; v26++, v30++) {
        RobOsageNodeData* v31 = v26->data_ptr;
        float_t weight = v31->skp_osg_node.weight;
        vec3 v111;
        if (!rob_osg->set_external_force) {
            sub_140482300(&v111, &v26->trans, &v30->trans, osage_gravity_const, weight);

            if (v26 != v26_end - 1) {
                vec3 v112;
                sub_140482300(&v112, &v26->trans, &v26[1].trans, osage_gravity_const, weight);
                vec3_add(v111, v112, v111);
                vec3_mult_scalar(v111, 0.5f, v111);
            }
        }
        else
            vec3_mult_scalar(rob_osg->external_force, 1.0f / weight, v111);

        vec3 v127;
        vec3_mult_scalar(v128, v31->force * v26->force, v127);
        float_t v41 = (1.0f - rob_osg->field_1EB4) * (1.0f - rob_osg->skin_param_ptr->air_res);

        vec3 v126;
        vec3_add(v111, v127, v126);
        vec3_mult_scalar(v26->trans_diff, v41, v111);
        vec3_sub(v126, v111, v126);
        vec3_mult_scalar(v26->external_force, weight, v111);
        vec3_add(v126, v111, v126);

        if (!disable_wind) {
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

        RobOsageNode* v55_begin = rob_osg->nodes.data() + 1;
        RobOsageNode* v55_end = rob_osg->nodes.data() + rob_osg->nodes.size();
        for (RobOsageNode* v55 = v55_begin; v55 != v55_end; v55++) {
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

            sub_140482FF0(v131, v129, 0, 0, rob_osg->yz_order);

            float_t v58;
            vec3_distance(v111, v126, v58);
            mat4_translate_mult(&v131, v58, 0.0f, 0.0f, &v131);

            v111 = v126;
        }
    }

    RobOsageNode* v82_begin = rob_osg->nodes.data() + 1;
    RobOsageNode* v82_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (RobOsageNode* v82 = v82_begin; v82 != v82_end; v82++) {
        v82->trans_orig = v82->trans;
        vec3_add(v82->field_28, v82->trans_diff, v82->trans_diff);
        vec3_add(v82->trans_diff, v82->trans, v82->trans);
    }

    if (rob_osg->nodes.size() > 1) {
        RobOsageNode* v90_begin = rob_osg->nodes.data() + rob_osg->nodes.size() - 2;
        RobOsageNode* v90_end = rob_osg->nodes.data();
        for (RobOsageNode* v90 = v90_begin; v90 != v90_end; v90--)
            sub_140482F30(&v90[0].trans, &v90[1].trans, v25 * v90->child_length);
    }

    if (ring_coli) {
        RobOsageNode* v91 = &rob_osg->nodes.data()[0];
        float_t coli_r = v91->data_ptr->skp_osg_node.coli_r;
        float_t ring_height;
        if (v91->trans.x < rob_osg->ring.ring_rectangle_x - coli_r
            || v91->trans.z < rob_osg->ring.ring_rectangle_y - coli_r
            || v91->trans.x > rob_osg->ring.ring_rectangle_x + rob_osg->ring.ring_rectangle_width
            || v91->trans.z > rob_osg->ring.ring_rectangle_y + rob_osg->ring.ring_rectangle_height)
            ring_height = rob_osg->ring.ring_out_height;
        else
            ring_height = rob_osg->ring.ring_height;

        float_t v96 = ring_height + coli_r;
        RobOsageNode* v98_begin = rob_osg->nodes.data() + 1;
        RobOsageNode* v98_end = rob_osg->nodes.data() + rob_osg->nodes.size();
        for (RobOsageNode* v98 = v98_begin; v98 != v98_end; v98++) {
            sub_140482490(v98, step, v25);
            sub_140482180(v98, v96);
        }
    }

    if (has_children_node) {
        RobOsageNode* v100 = rob_osg->nodes.data();
        RobOsageNode* v99_begin = rob_osg->nodes.data() + 1;
        RobOsageNode* v99_end = rob_osg->nodes.data() + rob_osg->nodes.size();
        for (RobOsageNode* v99 = v99_begin; v99 != v99_end; v99++, v100++) {
            vec3 v129;
            mat4_mult_vec3_inv_trans(&v130, &v99->trans, &v129);
            bool v102 = sub_140482FF0(v130, v129,
                &v99->data_ptr->skp_osg_node.hinge,
                &v99->reset_data.rotation, rob_osg->yz_order);
            *v99->bone_node_ptr->ex_data_mat = v130;

            float_t v104;
            vec3_distance_squared(v99->trans, v100->trans, v104);
            float_t v105 = v25 * v99->length;

            bool v106;
            if (v104 >= v105 * v105) {
                v105 = sqrtf(v104);
                v106 = false;
            }
            else
                v106 = true;

            mat4_translate_mult(&v130, v105, 0.0f, 0.0f, &v130);
            if (v102 || v106)
                mat4_get_translation(&v130, &v99->trans);
        }

        if (rob_osg->nodes.size() && rob_osg->node.bone_node_mat) {
            mat4 v131 = *rob_osg->nodes.back().bone_node_ptr->ex_data_mat;
            mat4_translate_mult(&v131, v25 * rob_osg->node.length, 0.0f, 0.0f, &v131);
            *rob_osg->node.bone_node_ptr->ex_data_mat = v131;
        }
    }
    rob_osg->field_2A0 = false;
}

static void sub_14047E1C0(RobOsage* rob_osg, vec3* scale) {
    RobOsageNode* i_begin = rob_osg->nodes.data() + 1;
    RobOsageNode* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++)
        if (i->data_ptr->normal_ref.field_0) {
            sub_14053CE30(&i->data_ptr->normal_ref, i->bone_node_mat);
            mat4_scale_rot(i->bone_node_mat, scale, i->bone_node_mat);
        }
}

static void sub_14047EE90(RobOsage* rob_osg, mat4* mat) {
    if (rob_osg->reset_data_list) {
        std::list<RobOsageNodeResetData>::iterator reset_data_list = rob_osg->reset_data_list->begin();
        RobOsageNode* i_begin = rob_osg->nodes.data() + 1;
        RobOsageNode* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
        for (RobOsageNode* i = i_begin; i != i_end; i++)
            i->reset_data = *reset_data_list++;
        rob_osg->reset_data_list = 0;
    }

    RobOsageNode* i_begin = rob_osg->nodes.data() + 1;
    RobOsageNode* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++) {
        mat4_mult_vec3(mat, &i->reset_data.trans_diff, &i->trans_diff);
        mat4_mult_vec3_trans(mat, &i->reset_data.trans, &i->trans);
    }
    rob_osg->parent_mat = *rob_osg->parent_mat_ptr;
}

static void sub_14047F110(RobOsage* rob_osg, mat4* mat, vec3* parent_scale, bool init_rot) {
    vec3 position = rob_osg->exp_data.position * *parent_scale;
    mat4_translate_mult(mat, &position, mat);
    mat4_rotate_mult(mat, &rob_osg->exp_data.rotation, mat);

    skin_param* skin_param = rob_osg->skin_param_ptr;
    vec3 rot = skin_param->rot;
    if (init_rot)
        rot = skin_param->init_rot + rot;
    mat4_rotate_mult(mat, &rot, mat);
}

static void sub_1404803B0(RobOsage* rob_osg, mat4* mat, vec3* parent_scale, bool has_children_node) {
    mat4 v47 = *mat;
    vec3 v45;
    vec3_mult(rob_osg->exp_data.position, *parent_scale, v45);
    mat4_mult_vec3_trans(&v47, &v45, &rob_osg->nodes.data()[0].trans);
    if (rob_osg->osage_reset && !rob_osg->field_1F0E) {
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

        if (move_cancel > 0.0f) {
            RobOsageNode* i_begin = rob_osg->nodes.data() + 1;
            RobOsageNode* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
            for (RobOsageNode* i = i_begin; i != i_end; i++) {
                vec3 v46;
                mat4_mult_vec3_inv_trans(&rob_osg->parent_mat, &i->trans, &v46);
                mat4_mult_vec3_trans(rob_osg->parent_mat_ptr, &v46, &v46);
                i->trans += (v46 - i->trans) * move_cancel;
            }
        }
    }

    if (has_children_node) {
        sub_14047F110(rob_osg, &v47, parent_scale, false);
        *rob_osg->nodes.data()[0].bone_node_mat = v47;
        RobOsageNode* v29 = rob_osg->nodes.data();
        RobOsageNode* v30_begin = rob_osg->nodes.data() + 1;
        RobOsageNode* v30_end = rob_osg->nodes.data() + rob_osg->nodes.size();
        for (RobOsageNode* v30 = v30_begin; v30 != v30_end; v29++, v30++) {
            vec3 v46;
            mat4_mult_vec3_inv_trans(&v47, &v30->trans, &v46);
            bool v32 = sub_140482FF0(v47, v46, &v30->data_ptr->skp_osg_node.hinge,
                &v30->reset_data.rotation, rob_osg->yz_order);
            *v30->bone_node_ptr->ex_data_mat = v47;
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

        if (rob_osg->nodes.size() && rob_osg->node.bone_node_mat) {
            mat4 v48 = *rob_osg->nodes.back().bone_node_ptr->ex_data_mat;
            mat4_translate_mult(&v48, parent_scale->x * rob_osg->node.length, 0.0f, 0.0f, &v48);
            *rob_osg->node.bone_node_ptr->ex_data_mat = v48;
        }
    }
}

static void sub_140482180(RobOsageNode* node, float_t a2) {
    float_t v2 = node->data_ptr->skp_osg_node.coli_r + a2;
    if (v2 <= node->trans.y)
        return;

    node->trans.y = v2;
    vec3 v3;
    vec3_sub(node->trans, node[-1].trans, v3);
    vec3_normalize(v3, v3);
    vec3_mult_scalar(v3, node->length, v3);
    vec3_add(node[-1].trans, v3, node->trans);
    node->trans_diff = 0.0f;
    node->field_C8 += 1.0f;
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

static void sub_140482490(RobOsageNode* node, float_t step, float_t a3) {
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

    sub_140482F30(&node[0].trans, &node[-1].trans, node->length * a3);
    if (node->sibling_node)
        sub_140482F30(&node->trans, &node->sibling_node->trans, node->max_distance);
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

static void sub_14047D8C0(RobOsage* rob_osg, mat4* mat, vec3* parent_scale, float_t step, bool a5) {
    if (!rob_osg->osage_reset && step <= 0.0f)
        return;

    OsageCollision::Work* coli = rob_osg->coli;
    OsageCollision::Work* coli_ring = rob_osg->coli_ring;
    OsageCollision& vec_ring_coli = rob_osg->ring.coli;

    float_t v9 = 0.0f;
    if (step > 0.0f)
        v9 = 1.0f / step;

    mat4 v64;
    if (a5)
        v64 = *rob_osg->nodes.data()[0].bone_node_mat;
    else {
        v64 = *mat;

        vec3 trans;
        vec3_mult(rob_osg->exp_data.position, *parent_scale, trans);
        mat4_mult_vec3_trans(&v64, &trans, &rob_osg->nodes.data()[0].trans);
        sub_14047F110(rob_osg, &v64, parent_scale, false);
    }

    float_t v60 = -1000.0f;
    if (a5) {
        RobOsageNode* v16 = &rob_osg->nodes.data()[0];
        float_t coli_r = v16->data_ptr->skp_osg_node.coli_r;
        float_t ring_height;
        if (v16->trans.x < rob_osg->ring.ring_rectangle_x - coli_r
            || v16->trans.z < rob_osg->ring.ring_rectangle_y - coli_r
            || v16->trans.x > rob_osg->ring.ring_rectangle_x + rob_osg->ring.ring_rectangle_width
            || v16->trans.z > rob_osg->ring.ring_rectangle_y + rob_osg->ring.ring_rectangle_height)
            ring_height = rob_osg->ring.ring_out_height;
        else
            ring_height = rob_osg->ring.ring_height;

        v60 = ring_height + coli_r;
    }

    float_t v23 = 0.2f;
    if (step < 1.0f)
        v23 = 0.2f / (2.0f - step);

    if (rob_osg->skin_param_ptr->colli_tgt_osg) {
        std::vector<RobOsageNode>* v24 = rob_osg->skin_param_ptr->colli_tgt_osg;
        RobOsageNode* v27_begin = rob_osg->nodes.data() + 1;
        RobOsageNode* v27_end = rob_osg->nodes.data() + rob_osg->nodes.size();
        for (RobOsageNode* v27 = v27_begin; v27 != v27_end; v27++) {
            vec3 v62 = 0.0f;

            RobOsageNode* v30_begin = v24->data() + 1;
            RobOsageNode* v30_end = v24->data() + v24->size();
            for (RobOsageNode* v30 = v30_begin; v30 != v30_end; v30++)
                OsageCollision::cls_ball_oidashi(v62, v27->trans, v30->trans,
                    v27->data_ptr->skp_osg_node.coli_r + v30->data_ptr->skp_osg_node.coli_r);
            vec3_add(v27->trans, v62, v27->trans);
        }
    }

    RobOsageNode* v35_begin = rob_osg->nodes.data() + 1;
    RobOsageNode* v35_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (RobOsageNode* v35 = v35_begin; v35 != v35_end; v35++) {
        skin_param_osage_node* skp_osg_node = &v35->data_ptr->skp_osg_node;
        float_t v37 = (1.0f - rob_osg->field_1EB4) * rob_osg->skin_param_ptr->friction;
        if (a5) {
            sub_140482490(&v35[0], step, parent_scale->x);
            v35->field_C8 = (float_t)OsageCollision::osage_cls_work_list(v35->trans,
                skp_osg_node->coli_r, vec_ring_coli, &v35->field_CC);
            if (!rob_osg->field_1F0F) {
                v35->field_C8 += (float_t)coli_ring->osage_cls(v35->trans, skp_osg_node->coli_r);
                v35->field_C8 += (float_t)coli->osage_cls(v35->trans, skp_osg_node->coli_r);
            }
            sub_140482180(&v35[0], v60);
        }
        else
            sub_140482F30(&v35[0].trans, &v35[-1].trans, v35[0].length * parent_scale->x);

        vec3 v63;
        mat4_mult_vec3_inv_trans(&v64, &v35->trans, &v63);
        bool v40 = sub_140482FF0(v64, v63, &skp_osg_node->hinge,
            &v35->reset_data.rotation, rob_osg->yz_order);
        v35->bone_node_ptr->exp_data.parent_scale = *parent_scale;
        *v35->bone_node_ptr->ex_data_mat = v64;

        if (v35->bone_node_mat) {
            mat4* v41 = v35->bone_node_mat;
            mat4_scale_rot(&v64, parent_scale, v41);
        }

        float_t v42 = v35->length * parent_scale->x;
        float_t v44;
        bool v45;
        vec3_distance_squared(v35[0].trans, v35[-1].trans, v44);
        if (v44 >= v42 * v42) {
            v42 = sqrtf(v44);
            v45 = false;
        }
        else
            v45 = true;

        mat4_translate_mult(&v64, v42, 0.0f, 0.0f, &v64);
        v35->reset_data.length = v42;
        if (v40 || v45)
            mat4_get_translation(&v64, &v35->trans);

        vec3 v62;
        vec3_sub(v35->trans, v35->trans_orig, v62);
        vec3_mult_scalar(v62, v9, v35->trans_diff);

        if (v35->field_C8 > 0.0f) {
            if (v37 > v35->field_CC)
                v37 = v35->field_CC;
            vec3_mult_scalar(v35->trans_diff, v37, v35->trans_diff);
        }

        float_t v55;
        vec3_length_squared(v35->trans_diff, v55);
        if (v55 > v23 * v23)
            vec3_mult_scalar(v35->trans_diff, v23 / sqrtf(v55), v35->trans_diff);

        mat4_mult_vec3_inv_trans(mat, &v35->trans, &v35->reset_data.trans);
        mat4_mult_vec3_inv(mat, &v35->trans_diff, &v35->reset_data.trans_diff);
    }

    if (rob_osg->nodes.size() && rob_osg->node.bone_node_mat) {
        mat4 v65 = *rob_osg->nodes.back().bone_node_ptr->ex_data_mat;
        mat4_translate_mult(&v65, rob_osg->node.length * parent_scale->x, 0.0f, 0.0f, &v65);
        *rob_osg->node.bone_node_ptr->ex_data_mat = v65;
        mat4_scale_rot(&v65, parent_scale, &v65);
        *rob_osg->node.bone_node_mat = v65;
        rob_osg->node.bone_node_ptr->exp_data.parent_scale = *parent_scale;
    }
}

static void sub_14047C750(RobOsage* rob_osg, mat4* mat, vec3* parent_scale, float_t step) {
    sub_14047C770(rob_osg, mat, parent_scale, step, false);
}

static void sub_14047C770(RobOsage* rob_osg, mat4* mat, vec3* parent_scale, float_t step, bool a5) {
    sub_14047C800(rob_osg, mat, parent_scale, step, a5, false, false);
    sub_14047D8C0(rob_osg, mat, parent_scale, step, true);
    sub_140480260(rob_osg, mat, parent_scale, step, a5);
}

static void sub_14047D620(RobOsage* rob_osg, float_t step) {
    if (step < 0.0f && rob_osg->field_1F0F)
        return;

    std::vector<RobOsageNode>& nodes = rob_osg->nodes;
    vec3 v7 = nodes.data()[0].trans;
    OsageCollision::Work* coli = rob_osg->coli;
    OsageCollision::Work* coli_ring = rob_osg->coli_ring;
    int32_t coli_type = rob_osg->skin_param_ptr->coli_type;
    RobOsageNode* i_begin = nodes.data() + 1;
    RobOsageNode* i_end = nodes.data() + nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++) {
        RobOsageNodeData* data = i->data_ptr;
        for (RobOsageNode*& j : data->boc) {
            float_t v17 = (float_t)(
                coli->osage_capsule_cls(i->trans, j->trans, data->skp_osg_node.coli_r)
                + coli_ring->osage_capsule_cls(i->trans, j->trans, data->skp_osg_node.coli_r));
            i->field_C8 += v17;
            j->field_C8 += v17;
        }

        if (coli_type && (coli_type != 1 || i != i_begin)) {
            float_t v20 = (float_t)(
                coli->osage_capsule_cls(i[0].trans, i[-1].trans, data->skp_osg_node.coli_r)
                + coli_ring->osage_capsule_cls(i[0].trans, i[-1].trans, data->skp_osg_node.coli_r));
            i[0].field_C8 += v20;
            i[-1].field_C8 += v20;
        }
    }
    nodes.data()[0].trans = v7;
}

static void sub_14047ECA0(RobOsage* rob_osg, float_t step) {
    if (step <= 0.0f)
        return;

    OsageCollision& vec_coli = rob_osg->ring.coli;
    OsageCollision::Work* coli_ring = rob_osg->coli_ring;
    OsageCollision::Work* coli = rob_osg->coli;
    bool v11 = rob_osg->field_1F0F;
    RobOsageNode* i_begin = rob_osg->nodes.data() + 1;
    RobOsageNode* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++) {
        RobOsageNodeData* data = i->data_ptr;
        i->field_C8 += (float_t)OsageCollision::osage_cls_work_list(i->trans,
            data->skp_osg_node.coli_r, vec_coli, &i->field_CC);
        if (v11)
            continue;

        for (RobOsageNode*& j : data->boc) {
            j->field_C8 += (float_t)coli_ring->osage_cls(j->trans, data->skp_osg_node.coli_r);
            j->field_C8 += (float_t)coli->osage_cls(j->trans, data->skp_osg_node.coli_r);
        }

        i->field_C8 += (float_t)coli_ring->osage_cls(i->trans, data->skp_osg_node.coli_r);
        i->field_C8 += (float_t)coli->osage_cls(i->trans, data->skp_osg_node.coli_r);
    }
}

static void sub_14047F990(RobOsage* rob_osg, mat4* a2, vec3* parent_scale, bool a4) {
    if (!rob_osg->nodes.size())
        return;

    vec3 v76;
    vec3_mult(rob_osg->exp_data.position, *parent_scale, v76);
    mat4_mult_vec3_trans(a2, &v76, &v76);
    vec3 v74 = v76;
    RobOsageNode* v12 = &rob_osg->nodes.data()[0];
    v12->trans = v76;
    v12->trans_orig = v76;
    v12->trans_diff = 0.0f;

    float_t ring_height;
    RobOsageNode* v14 = &rob_osg->nodes.data()[0];
    float_t coli_r = v14->data_ptr->skp_osg_node.coli_r;
    if (v14->trans.x < rob_osg->ring.ring_rectangle_x - coli_r
        || v14->trans.z < rob_osg->ring.ring_rectangle_y - coli_r
        || v14->trans.x > rob_osg->ring.ring_rectangle_x + rob_osg->ring.ring_rectangle_width
        || v14->trans.z > rob_osg->ring.ring_rectangle_y + rob_osg->ring.ring_rectangle_height)
        ring_height = rob_osg->ring.ring_out_height;
    else
        ring_height = rob_osg->ring.ring_height;

    mat4 v78 = *a2;
    sub_14047F110(rob_osg, &v78, parent_scale, true);
    vec3 v60 = { 1.0f, 0.0f, 0.0f };
    mat4_mult_vec3(&v78, &v60, &v60);

    OsageCollision::Work* coli_ring = rob_osg->coli_ring;
    OsageCollision::Work* coli = rob_osg->coli;
    float_t v25 = ring_height + coli_r;
    float_t v16 = parent_scale->x;
    RobOsageNode* i = rob_osg->nodes.data();
    RobOsageNode* j_begin = rob_osg->nodes.data() + 1;
    RobOsageNode* j_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (RobOsageNode* j = j_begin; j != j_end; i++, j++) {
        vec3_normalize(v60, v60);
        vec3_mult_scalar(v60, v16 * j->length, v60);
        vec3_add(i->trans, v60, v74);
        if (a4 && j->sibling_node)
            sub_140482F30(&v74, &j->sibling_node->trans, j->max_distance);

        skin_param_osage_node* v38 = &j->data_ptr->skp_osg_node;
        coli_ring->osage_cls(v74, v38->coli_r);
        coli->osage_cls(v74, v38->coli_r);

        float_t v39 = v25 + v38->coli_r;
        if (v74.y < v39 && v39 < 1001.0f) {
            v74.y = v39;
            vec3_sub(v74, i->trans, v74);
            vec3_normalize(v74, v74);
            vec3_mult_scalar(v74, v16 * j->length, v74);
            vec3_add(v74, i->trans, v74);
        }
        j->trans = v74;
        j->trans_diff = 0.0f;
        vec3 v77;
        mat4_mult_vec3_inv_trans(&v78, &j->trans, &v77);
        sub_140482FF0(v78, v77, &j->data_ptr->skp_osg_node.hinge,
            &j->reset_data.rotation, rob_osg->yz_order);
        j->bone_node_ptr->exp_data.parent_scale = *parent_scale;
        *j->bone_node_ptr->ex_data_mat = v78;
        if (j->bone_node_mat)
            mat4_scale_rot(&v78, parent_scale, j->bone_node_mat);

        float_t v55;
        vec3_distance(j->trans, i->trans, v55);
        float_t v56 = v16 * j->length;
        if (v55 >= fabsf(v56))
            v56 = v55;
        mat4_translate_mult(&v78, v56, 0.0f, 0.0f, &v78);
        mat4_get_translation(&v78, &j->trans);
        vec3_sub(j->trans, i->trans, v60);
    }

    if (rob_osg->nodes.size() && rob_osg->node.bone_node_mat) {
        mat4 v79 = *rob_osg->nodes.back().bone_node_ptr->ex_data_mat;
        mat4_translate_mult(&v79, v16 * rob_osg->node.length, 0.0f, 0.0f, &v79);
        *rob_osg->node.bone_node_ptr->ex_data_mat = v79;
        mat4_scale_rot(&v79, parent_scale, &v79);
        *rob_osg->node.bone_node_mat = v79;
        rob_osg->node.bone_node_ptr->exp_data.parent_scale = *parent_scale;
    }
}

static void sub_140480260(RobOsage* rob_osg, mat4* mat, vec3* parent_scale, float_t step, bool v5) {
    if (!v5) {
        rob_osg->SetNodesExternalForce(0, 1.0f);
        rob_osg->SetNodesForce(1.0f);
        rob_osg->set_external_force = false;
        rob_osg->external_force = 0.0f;
    }

    rob_osg->field_2A0 = true;
    rob_osg->field_2A1 = false;
    rob_osg->field_2A4 = -1.0f;
    rob_osg->field_1F0C = false;
    rob_osg->osage_reset = false;
    rob_osg->field_1F0E = false;

    for (RobOsageNode& i : rob_osg->nodes) {
        i.field_C8 = 0.0f;
        i.field_CC = 1.0f;
    }

    rob_osg->parent_mat = *rob_osg->parent_mat_ptr;
}

static void sub_140482100(struc_476* a1, opd_blend_data* a2, struc_477* a3) {
    if (!a2->field_C)
        a1->field_0 = *a3;
    else if (a2->type == MOTION_BLEND_FREEZE) {
        if (a2->blend == 0.0f)
            a1->field_10 = a1->field_0;
        sub_140482DF0(&a1->field_0, &a1->field_10, a3, a2->blend);
    }
    else if (a2->type == MOTION_BLEND_CROSS)
        sub_140482DF0(&a1->field_0, &a1->field_0, a3, a2->blend);
}

static void sub_140482DF0(struc_477* dst, struc_477* src0, struc_477* src1, float_t blend) {
    dst->length = lerp_def(src0->length, src1->length, blend);
    vec3_lerp_scalar(src0->rotation, src1->rotation, dst->rotation, blend);
}

static bool sub_140482FF0(mat4& mat, vec3& trans, skin_param_hinge* hinge, vec3* rot, int32_t& yz_order) {
    bool clipped = false;
    float_t z_rot;
    float_t y_rot;
    if (yz_order == 1) {
        y_rot = atan2f(-trans.z, trans.x);
        z_rot = atan2f(trans.y, sqrtf(trans.x * trans.x + trans.z * trans.z));
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
        mat4_rotate_y_mult(&mat, y_rot, &mat);
        mat4_rotate_z_mult(&mat, z_rot, &mat);
    }
    else {
        z_rot = atan2f(trans.y, trans.x);
        y_rot = atan2f(-trans.z, sqrtf(trans.x * trans.x + trans.y * trans.y));
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
        mat4_rotate_z_mult(&mat, z_rot, &mat);
        mat4_rotate_y_mult(&mat, y_rot, &mat);
    }

    if (rot) {
        rot->y = y_rot;
        rot->z = z_rot;
    }
    return clipped;
}

static void sub_14053CE30(RobOsageNodeDataNormalRef* normal_ref, mat4* a2) {
    if (!normal_ref->field_0)
        return;

    vec3 n_trans;
    vec3 u_trans;
    vec3 d_trans;
    vec3 l_trans;
    vec3 r_trans;
    mat4_get_translation(normal_ref->n->bone_node_ptr->ex_data_mat, &n_trans);
    mat4_get_translation(normal_ref->u->bone_node_ptr->ex_data_mat, &u_trans);
    mat4_get_translation(normal_ref->d->bone_node_ptr->ex_data_mat, &d_trans);
    mat4_get_translation(normal_ref->l->bone_node_ptr->ex_data_mat, &l_trans);
    mat4_get_translation(normal_ref->r->bone_node_ptr->ex_data_mat, &r_trans);

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

        mat4 v33 = normal_ref->mat;
        mat4_mult(&v33, &v34, a2);
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
