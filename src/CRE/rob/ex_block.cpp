/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "rob.hpp"
#include "../../KKdLib/key_val.hpp"
#include "../../KKdLib/str_utils.hpp"
#include "skin_param.hpp"

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

static void sub_140218560(RobCloth* rob_cls, float_t step, bool a3);
static void sub_1402187D0(RobCloth* rob_cls, bool a2);
static void sub_140219940(RobCloth* rob_cls);
static void sub_140219D10(RobCloth* rob_cls);
static float_t sub_14021A290(const vec3& trans_a, const vec3& trans_b, const vec3& trans_c,
    const vec2& texcoord_a, const vec2& texcoord_b, const vec2& texcoord_c,
    vec3& tangent, vec3& binormal, vec3& normal);
static float_t sub_14021A5E0(const vec3& pos_a, const vec3& pos_b,
    const float_t uv_a_y, const float_t uv_b_y,
    vec3& tangent, vec3& binormal, vec3& normal);
static void sub_14021A890(CLOTHNode* a1, CLOTHNode* a2, CLOTHNode* a3, CLOTHNode* a4, CLOTHNode* a5);
static void sub_14021AA60(RobCloth* rob_cls, float_t step, bool a3);
static void sub_14021D480(RobCloth* rob_cls);
static void sub_14021DC60(RobCloth* rob_cls, float_t step);
static void sub_14021D840(RobCloth* rob_cls);
static void sub_14047C800(RobOsage* rob_osg, const mat4* root_matrix,
    const vec3* parent_scale, float_t step, bool disable_external_force,
    bool ring_coli, bool has_children_node);
static void sub_14047E1C0(RobOsage* rob_osg, vec3* scale);
static void sub_14047F110(RobOsage* rob_osg, mat4* mat, const vec3* parent_scale, bool init_rot);
static void sub_1404803B0(RobOsage* rob_osg, const mat4* root_matrix,
    const vec3* parent_scale, bool has_children_node);
static void sub_140482180(RobOsageNode* node, float_t ring_y);
static void sub_140482300(vec3* a1, vec3* a2, vec3* a3, float_t osage_gravity_const, float_t weight);
static void sub_140482490(RobOsageNode* node, float_t step, float_t a3);
static void sub_140482F30(vec3* pos1, vec3* pos2, float_t length);
static void sub_14047D8C0(RobOsage* rob_osg, const mat4* root_matrix,
    const vec3* parent_scale, float_t step, bool a5);
static void sub_14047C750(RobOsage* rob_osg, const mat4* root_matrix,
    const vec3* parent_scale, float_t step);
static void sub_14047C770(RobOsage* rob_osg, const mat4* root_matrix,
    const vec3* parent_scale, float_t step, bool disable_external_force);
static void sub_14047D620(RobOsage* rob_osg, float_t step);
static void sub_14047ECA0(RobOsage* rob_osg, float_t step);
static void sub_14047F990(RobOsage* rob_osg, const mat4* root_matrix,
    const vec3* parent_scale, bool a4);
static void sub_140480260(RobOsage* rob_osg, const mat4* root_matrix,
    const vec3* parent_scale, float_t step, bool disable_external_force);
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
    this->name = name ? name : "(null)";
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

void ExNullBlock::Field_18(int32_t stage, bool disable_external_force) {

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

void ExNullBlock::Disp(const mat4* mat, render_context* rctx) {

}

void ExNullBlock::Field_40() {

}

void ExNullBlock::Field_48() {
    Field_20();
}

void ExNullBlock::Field_50() {

}

void ExNullBlock::InitData(rob_chara_item_equip_object* itm_eq_obj,
    obj_skin_block_constraint* cns_data, const char* cns_data_name, const bone_database* bone_data) {
    bone_node* node = itm_eq_obj->get_bone_node(cns_data_name, bone_data);
    type = EX_NONE;
    bone_node_ptr = node;
    this->cns_data = cns_data;
    name = node->name;
    item_equip_object = itm_eq_obj;
}

RobOsageNodeDataNormalRef::RobOsageNodeDataNormalRef() : set(), n(), u(), d(), l(), r() {

}

bool RobOsageNodeDataNormalRef::Check() {
    set = false;
    if (!n)
        return set;

    if (u && !memcmp(&u->mat, &mat4_null, sizeof(mat4)))
        u = 0;

    if (d && !memcmp(&d->mat, &mat4_null, sizeof(mat4)))
        d = 0;

    if (l && !memcmp(&l->mat, &mat4_null, sizeof(mat4)))
        l = 0;

    if (r && !memcmp(&r->mat, &mat4_null, sizeof(mat4)))
        r = 0;

    if ((u || d) && (l || r)) {
        if (!u)
            u = n;
        if (!d)
            d = n;
        if (!l)
            l = n;
        if (!r)
            r = n;
        set = true;
    }
    return set;
}

void RobOsageNodeDataNormalRef::GetMat() {
    if (!Check())
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
        mat.row3.x = -vec3::dot(n_trans, v23);
        mat.row3.y = -vec3::dot(n_trans, v21);
        mat.row3.z = -vec3::dot(n_trans, v22);
        mat.row3.w = 1.0f;
        mat4_mul(&n->mat, &mat, &mat);
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
    normal_ref.set = false;
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

void RobOsageNodeData::SetForce(const skin_param_osage_root& skp_root,
    skin_param_osage_node* skp_osg_node, size_t index) {
    this->skp_osg_node = *skp_osg_node;
    this->skp_osg_node.hinge.limit();

    float_t force = skp_root.force;
    float_t force_gain = skp_root.force_gain;
    for (; index >= 4; index -= 4)
        force = force * force_gain * force_gain * force_gain * force_gain;
    for (; index; index--)
        force *= force_gain;
    this->force = force;
}

opd_node_data::opd_node_data() : length() {

}

opd_node_data::opd_node_data(float_t length, vec3 rotation) {
    this->length = length;
    this->rotation = rotation;
}

void opd_node_data::lerp(opd_node_data& dst, const opd_node_data& src0, const opd_node_data& src1, float_t blend) {
    dst.length = lerp_def(src0.length, src1.length, blend);
    dst.rotation = vec3::lerp(src0.rotation, src1.rotation, blend);
}

opd_node_data_pair::opd_node_data_pair() {

}

void opd_node_data_pair::set_data(opd_blend_data* blend_data, opd_node_data&& node_data) {
    if (!blend_data->use_blend)
        curr = node_data;
    else if (blend_data->type == MOTION_BLEND_FREEZE) {
        if (blend_data->blend == 0.0f)
            prev = curr;
        opd_node_data::lerp(curr, prev, node_data, blend_data->blend);
    }
    else if (blend_data->type == MOTION_BLEND_CROSS)
        opd_node_data::lerp(curr, curr, node_data, blend_data->blend);
}

RobOsageNode::RobOsageNode() : length(), trans(), trans_orig(), trans_diff(),
field_28(), child_length(), bone_node_ptr(), bone_node_mat(), sibling_node(),
max_distance(), field_94(), reset_data(), field_C8(), external_force(), mat() {
    friction = 1.0f;
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
    friction = 1.0f;
    external_force = 0.0f;
    force = 1.0f;
    data.Reset();
    data_ptr = &data;
    opd_data.clear();
    opd_data.resize(3);
    mat = mat4_null;
}

OsageCollision::Work::Work() : type(), radius(), pos(),
vec_center(), vec_center_length(), vec_center_length_squared(), friction() {

}

OsageCollision::Work::~Work() {

}

// 0x140485450
void OsageCollision::Work::update_cls_work(OsageCollision::Work* cls,
    SkinParam::CollisionParam* cls_param, const mat4* transform) {
    cls->type = SkinParam::CollisionTypeEnd;
    if (!cls_param || !transform)
        return;

    for (; cls_param->type; cls++, cls_param++) {
        cls->type = cls_param->type;
        cls->radius = cls_param->radius;
        mat4_transform_point(&transform[cls_param->node_idx[0]],
            &cls_param->pos[0], &cls->pos[0]);

        switch (cls->type) {
        case SkinParam::CollisionTypePlane:
            mat4_transform_vector(&transform[cls_param->node_idx[0]],
                &cls_param->pos[1], &cls->pos[1]);
            break;
        case SkinParam::CollisionTypeCapsule:
        case SkinParam::CollisionTypeEllipse:
            mat4_transform_point(&transform[cls_param->node_idx[1]],
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

// 0x140485410
void OsageCollision::Work::update_cls_work(OsageCollision::Work* cls,
    std::vector<SkinParam::CollisionParam>& cls_list, const mat4* transform) {
    if (!cls_list.size()) {
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
        update_cls_work(cls, cls_list.data(), transform);
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
int32_t OsageCollision::cls_capsule_oidashi(vec3& vec, const vec3& p, const OsageCollision::Work* cls, const float_t r) {
    vec3 v11 = p - cls->pos[0];

    float_t v17 = vec3::dot(v11, cls->vec_center);
    if (v17 < 0.0f)
        return OsageCollision::cls_ball_oidashi(vec, p, cls->pos[0], r);

    float_t v19 = cls->vec_center_length_squared;
    if (fabsf(v19) <= 0.000001f)
        return OsageCollision::cls_ball_oidashi(vec, p, cls->pos[0], r);
    else if (v17 > v19)
        return OsageCollision::cls_ball_oidashi(vec, p, cls->pos[1], r);

    float_t v20 = vec3::length_squared(v11);
    float_t v21 = v17 / v19;
    float_t v22 = fabsf(v20 - v21 * v17);
    if (v22 > 0.000001f && v22 < r * r) {
        vec = (v11 - cls->vec_center * v21) * (r / sqrtf(v22) - 1.0f);
        return 1;
    }
    return 0;
}

// 0x140483EA0
int32_t OsageCollision::cls_ellipse_oidashi(vec3& vec, const vec3& p, const OsageCollision::Work* cls, const float_t r) {
    const vec3& p1 = cls->pos[0];
    const vec3& q1 = cls->pos[1];

    if (fabsf(cls->vec_center_length) <= 0.000001f)
        return OsageCollision::cls_ball_oidashi(vec, p, p1, r);

    const float_t v61 = cls->vec_center_length * 0.5f;
    const float_t v13 = sqrtf(r * r + v61 * v61);

    vec3 d0 = p - p1;
    const float_t d0_len = vec3::length(d0);

    vec3 d1 = p - q1;
    const float_t d1_len = vec3::length(d1);

    if (fabsf(d0_len) <= 0.000001 || fabsf(d1_len) <= 0.000001f)
        return 0;

    const float_t d_len = (d0_len + d1_len) * 0.5f;
    if (d_len >= v13)
        return 0;

    vec3 v63 = p - (p1 + q1) * 0.5f;

    const float_t v58 = vec3::length(v63);
    if (v58 != 0.0f)
        v63 *= 1.0f / v58;

    const float_t v36 = vec3::dot(v63, cls->vec_center) * (1.0f / cls->vec_center_length);

    const float_t v38 = d_len * d_len - v61 * v61;
    if (v38 < 0.0f)
        return 0;

    const float_t v39 = sqrtf(v38);
    if (fabsf(v39) <= 0.000001f)
        return 0;

    const float_t v40 = sqrtf(1.0f - min_def(v36 * v36, 1.0f));
    const float_t v41 = (v13 / d_len - 1.0f) * (v36 * v58);
    const float_t v42 = (r / v39 - 1.0f) * (v40 * v58);
    const float_t v43 = sqrtf(v42 * v42 + v41 * v41);

    if (d0_len != 0.0f)
        d0 *= 1.0f / d0_len;

    if (d1_len != 0.0f)
        d1 *= 1.0f / d1_len;

    vec = vec3::normalize(d0 + d1) * v43;
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
int32_t OsageCollision::cls_plane_oidashi(vec3& vec,
    const vec3& p, const vec3& p1, const vec3& p2, const float_t r) {
    float_t v5 = vec3::dot(p, p2) - vec3::dot(p1, p2) - r;
    if (v5 < 0.0f) {
        vec = p2 * -v5;
        return 1;
    }
    return 0;
}

// 0x140484E10
void OsageCollision::get_nearest_line2point(vec3& nearest, const vec3& p0, const vec3& p1, const vec3& q) {
    vec3 p0p1 = p1 - p0;
    vec3 p0q = q - p0;

    float_t t = vec3::dot(p0q, p0p1);
    if (t < 0.0f) {
        nearest = p0;
        return;
    }

    float_t p0p1_len = vec3::length_squared(p0p1);
    if (p0p1_len <= 0.000001f)
        nearest = p0;
    else if (t <= p0p1_len)
        nearest = p0 + p0p1 * (t / p0p1_len);
    else
        nearest = p1;
}

// 0x140484FE0
int32_t OsageCollision::osage_capsule_cls(const OsageCollision::Work* cls, vec3& p0, vec3& p1, const float_t& cls_r) {
    return OsageCollision::osage_capsule_cls(p0, p1, cls_r, cls);
}

// 0x140485000
int32_t OsageCollision::osage_capsule_cls(vec3& p0, vec3& p1, const float_t& cls_r, const OsageCollision::Work* cls) {
    if (!cls || cls->type == SkinParam::CollisionTypeEnd)
        return 0;

    int32_t v8 = 0;
    while (cls->type != SkinParam::CollisionTypeEnd) {
        vec3 vec = 0.0f;
        switch (cls->type) {
        case SkinParam::CollisionTypeBall:
            v8 += OsageCollision::cls_line2ball_oidashi(vec, p0, p1, cls->pos[0], cls_r + cls->radius);
            break;
        case SkinParam::CollisionTypeCapsule:
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

// 0x140485180
int32_t OsageCollision::osage_cls(const OsageCollision::Work* cls, vec3& p, const float_t& cls_r) {
    return OsageCollision::osage_cls(p, cls_r, cls, 0);
}

// 0x140485220
int32_t OsageCollision::osage_cls(vec3& p, const float_t& cls_r, const OsageCollision::Work* cls, float_t* fric) {
    if (!cls || cls->type == SkinParam::CollisionTypeEnd)
        return 0;

    int32_t v8 = 0;
    while (cls->type != SkinParam::CollisionTypeEnd) {
        int32_t v11 = 0;
        vec3 vec = 0.0f;
        switch (cls->type) {
        case SkinParam::CollisionTypeBall:
            v11 = OsageCollision::cls_ball_oidashi(vec, p, cls->pos[0], cls->radius + cls_r);
            break;
        case SkinParam::CollisionTypeCapsule:
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

        if (fric && v11 > 0)
            *fric = max_def(*fric, cls->friction);

        v8 += v11;
        p += vec;
        cls++;
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
ring_rectangle_width(), ring_rectangle_height(), init() {
    ring_height = -1000.0f;
    ring_out_height = -1000.0f;
}

osage_ring_data::~osage_ring_data() {

}

CLOTHNode::CLOTHNode() : flags(), trans(), trans_orig(), field_1C(), trans_diff(), normal(),
tangent(), binormal(), tangent_sign(), texcoord(), field_64(), dist_top(), dist_bottom(),
dist_right(), dist_left(), field_80(), field_88(), reset_data(), field_B4() {

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
            size_t v7 = i * root_count + j;
            v69.field_0 = v8;
            v69.field_8 = v7;
            v69.length = vec3::distance(nodes.data()[v7].trans, nodes.data()[v8].trans);
            field_58.push_back(v69);
            if (j < root_count - 1) {
                size_t v32 = i * root_count + j + 1;
                v69.field_0 = v7;
                v69.field_8 = v32;
                v69.length = vec3::distance(nodes.data()[v32].trans, nodes.data()[v7].trans);
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

void CLOTH::SetSkinParamColiR(float_t value) {
    skin_param_ptr->coli_r = value;
}

void CLOTH::SetSkinParamFriction(float_t value) {
    skin_param_ptr->friction = value;
}

void CLOTH::SetSkinParamWindAfc(float_t value) {
    skin_param_ptr->wind_afc = value;
}

void CLOTH::SetWindDirection(vec3& value) {
    this->wind_direction = value;
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
    index_buffer[0] = {};
    index_buffer[1] = {};
    motion_reset_data.clear();
    reset_data_list = 0;
}

void RobCloth::AddMotionResetData(uint32_t motion_id, float_t frame) {
    int32_t frame_int = (int32_t)prj::roundf(frame * 1000.0f);

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

// 0x1402196D0
void RobCloth::ApplyResetData() {
    size_t root_count = this->root_count;
    size_t nodes_count = this->nodes_count;

    if (reset_data_list) {
        auto reset_data = this->reset_data_list->begin();
        CLOTHNode* i_begin = nodes.data() + root_count;
        CLOTHNode* i_end = nodes.data() + nodes.size();
        for (CLOTHNode* i = i_begin; i != i_end; i++)
            i->reset_data = *reset_data++;
        reset_data_list = 0;
    }

    RobClothRoot* root = this->root.data();
    for (size_t i = 0; i < root_count; i++, root++) {
        CLOTHNode* node = &nodes.data()[i + root_count];
        for (size_t j = 1; j < nodes_count; j++, node += root_count) {
            mat4_transform_point(&root->field_D8, &node->reset_data.trans, &node->trans);
            mat4_transform_vector(&root->field_D8, &node->reset_data.trans_diff, &node->trans_diff);
        }
    }
}

void RobCloth::ColiSet(const mat4* transform) {
    if (skin_param_ptr->coli.size())
        OsageCollision::Work::update_cls_work(coli, skin_param_ptr->coli.data(), transform);
    OsageCollision::Work::update_cls_work(coli_ring, ring.skp_root_coli, transform);
}

void RobCloth::Disp(const mat4* mat, render_context* rctx) {
    obj* obj = object_storage_get_obj(itm_eq_obj->obj_info);
    if (!obj)
        return;

    std::vector<GLuint>* tex = object_storage_get_obj_set_textures(itm_eq_obj->obj_info.set_id);

    vec3 center = (nodes.data()[0].trans + nodes.data()[root_count * nodes_count - 1].trans) * 0.5f;

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

    rctx->disp_manager->set_texture_pattern((int32_t)itm_eq_obj->texture_pattern.size(), itm_eq_obj->texture_pattern.data());
    rctx->disp_manager->entry_obj_by_obj(mat, &o, tex, vertex_buffer, index_buffer, 0, itm_eq_obj->alpha);
    rctx->disp_manager->set_texture_pattern();
}

void RobCloth::InitData(size_t root_count, size_t nodes_count, obj_skin_block_cloth_root* root,
    obj_skin_block_cloth_node* nodes, mat4* mats, int32_t a7,
    rob_chara_item_equip_object* itm_eq_obj, const bone_database* bone_data) {
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
            obj_sub_mesh& cloth_mesh = submesh.arr[j];
            cloth_mesh = mesh[i].submesh_array[j];
            cloth_mesh.attrib.m.cloth = 0;
            cloth_mesh.bounding_sphere.radius = 1000.0f;
            cloth_mesh.axis_aligned_bounding_box.center = 0.0f;
            cloth_mesh.axis_aligned_bounding_box.size = { 1000.0f, 1000.0f, 1000.0f };
            mesh[i].submesh_array[j].attrib.m.cloth = 1;
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

    std::vector<size_t> index_array(root_count * nodes_count, 0);
    uint16_t* mesh_index_array = cls_data->mesh_index_array;
    for (size_t i = 0; i < cls_data->num_mesh_index; i++)
        index_array.data()[mesh_index_array[i]] = i;

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
            this->root.data()[i].tangent = vertex_array[index_array.data()[i]].tangent;
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
            v72.field_64 = vec3::normalize(node.trans_diff);
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
                v72.texcoord = vertex_array[index_array.data()[i * root_count + j]].texcoord0;
        }

    Init();
}

void RobCloth::InitDataParent(obj_skin_block_cloth* cls_data,
    rob_chara_item_equip_object* itm_eq_obj, const bone_database* bone_data) {
    ResetData();
    this->cls_data = cls_data;
    InitData(cls_data->num_root, cls_data->num_node, cls_data->root_array,
        cls_data->node_array, cls_data->mat_array, cls_data->field_14, itm_eq_obj, bone_data);
}

const float_t* RobCloth::LoadOpdData(size_t node_index, const float_t* opd_data, size_t opd_count) {
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

void RobCloth::LoadSkinParam(void* kv, const char* name, const bone_database* bone_data) {
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

void RobCloth::SetMotionResetData(uint32_t motion_id, float_t frame) {
    osage_reset = true;
    auto elem = motion_reset_data.find({ motion_id, (int32_t)prj::roundf(frame * 1000.0f) });
    if (elem != motion_reset_data.end() && elem->second.size() + root_count == nodes.size()) {
        this->reset_data_list = &elem->second;

        auto reset_data_list = this->reset_data_list->begin();
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

        int32_t curr_key = (int32_t)(int64_t)prj::floorf(frame);
        int32_t next_key = curr_key + 1;
        if ((float_t)next_key >= i->frame_count)
            next_key = 0;

        float_t blend = frame - (float_t)(int64_t)frame;
        float_t inv_blend = 1.0f - blend;

        for (size_t j = 0; j < root_count; j++) {
            CLOTHNode& root_node = nodes.data()[j];
            vec3 v26 = root_node.trans;
            mat4 mat = root.data()[j].field_98;
            mat4_mul_translate(&mat, &root_node.trans_orig, &mat);

            CLOTHNode* v29 = &nodes.data()[j + root_count];

            vec3 v78;
            mat4_transform_vector(&mat, &v29->field_64, &v78);

            int32_t yz_order = 0;
            sub_140482FF0(mat, v78, 0, 0, yz_order);

            mat4 v32 = root.data()[j].field_D8;
            for (size_t k = 1; k < nodes_count; k++, v29 += root_count) {
                opd_vec3_data* opd = &v29->opd_data[i - i_end];
                const float_t* opd_x = opd->x;
                const float_t* opd_y = opd->y;
                const float_t* opd_z = opd->z;

                vec3 v61;
                v61.x = opd_x[curr_key];
                v61.y = opd_y[curr_key];
                v61.z = opd_z[curr_key];

                vec3 v62;
                v62.x = opd_x[next_key];
                v62.y = opd_y[next_key];
                v62.z = opd_z[next_key];

                mat4_transform_point(&v32, &v61, &v61);
                mat4_transform_point(&v32, &v62, &v62);

                vec3 v57 = v61 * inv_blend + v62 * blend;

                vec3 v85;
                mat4_inverse_transform_point(&mat, &v57, &v85);

                vec3 rotation = 0.0f;
                int32_t yz_order = 0;
                sub_140482FF0(mat, v85, 0, &rotation, yz_order);

                mat4_mul_translate(&mat, vec3::distance(v57, v26), 0.0f, 0.0f, &mat);

                v29->opd_node_data.set_data(i, { v29->dist_top, rotation });

                v26 = v61;
            }
        }
    }

    for (size_t i = 0; i < root_count; i++) {
        CLOTHNode& root_node = nodes.data()[i];
        mat4 mat = root.data()[0].field_98;
        mat4_mul_translate(&mat, &root_node.trans_orig, &mat);

        CLOTHNode* v50 = &nodes.data()[i + root_count];

        vec3 v78;
        mat4_transform_vector(&mat, &v50->field_64, &v78);
        int32_t yz_order = 0;
        sub_140482FF0(mat, v78, 0, 0, yz_order);

        for (size_t j = 1; j < nodes_count; j++, v50 += root_count) {
            mat4_mul_rotate_z(&mat, v50->opd_node_data.curr.rotation.z, &mat);
            mat4_mul_rotate_y(&mat, v50->opd_node_data.curr.rotation.y, &mat);
            mat4_mul_translate(&mat, v50->opd_node_data.curr.length, 0.0f, 0.0f, &mat);
            mat4_get_translation(&mat, &v50->trans);
        }
    }
}

const float_t* RobCloth::SetOsagePlayDataInit(const float_t* opdi_data) {
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

void RobCloth::SetSkinParamOsageRoot(const skin_param_osage_root& skp_root) {
    SetForceAirRes(skp_root.force, skp_root.force_gain, skp_root.air_res);
    SetSkinParamFriction(skp_root.friction);
    SetSkinParamWindAfc(skp_root.wind_afc);
    SetSkinParamColiR(skp_root.coli_r);
    SetSkinParamHinge(skp_root.hinge_y, skp_root.hinge_z);
    skin_param_ptr->coli.assign(skp_root.coli.begin(), skp_root.coli.end());
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
    GL::ArrayBuffer buffer = vertex_buffer->get_buffer();
    size_t data = (size_t)buffer.MapMemory();
    if (!data)
        return;

    if (double_sided)
        indices_count /= 2;

    bool tangent = !!(mesh->vertex_format & OBJ_VERTEX_TANGENT);

    switch (mesh->attrib.m.compression) {
    case 0:
    default:
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
        break;
    case 1:
        if (tangent)
            for (int32_t i = 2; i; i--, facing = -1.0f) {
                for (int32_t j = indices_count; j; j--, indices++) {
                    CLOTHNode* node = &nodes[*indices];

                    *(vec3*)data = node->trans;

                    vec3_to_vec3i16(node->normal * (32767.0f * facing), *(vec3i16*)(data + 0x0C));
                    *(int16_t*)(data + 0x12) = 0;

                    vec4 tangent;
                    *(vec3*)&tangent = node->tangent;
                    tangent.w = node->tangent_sign;
                    vec4_to_vec4i16(tangent * 32767.0f, *(vec4i16*)(data + 0x14));

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

                    vec3_to_vec3i16(node->normal * (32767.0f * facing), *(vec3i16*)(data + 0x0C));
                    *(int16_t*)(data + 0x12) = 0;

                    data += mesh->size_vertex;
                }

                if (!double_sided)
                    break;
            }
    case 2:
        if (tangent)
            for (int32_t i = 2; i; i--, facing = -1.0f) {
                for (int32_t j = indices_count; j; j--, indices++) {
                    CLOTHNode* node = &nodes[*indices];

                    *(vec3*)data = node->trans;

                    vec3i16 normal_int;
                    vec3_to_vec3i16(node->normal * 511.0f, normal_int);
                    *(uint32_t*)(data + 0x0C) = (((uint32_t)0 & 0x03) << 30)
                        | (((uint32_t)normal_int.z & 0x3FF) << 20)
                        | (((uint32_t)normal_int.y & 0x3FF) << 10)
                        | ((uint32_t)normal_int.x & 0x3FF);

                    vec4 tangent;
                    *(vec3*)&tangent = node->tangent;
                    tangent.w = node->tangent_sign;

                    vec4i16 tangent_int;
                    vec4_to_vec4i16(tangent * 511.0f, tangent_int);
                    *(uint32_t*)(data + 0x10) = (((uint32_t)clamp_def(tangent_int.w, -1, 1) & 0x03) << 30)
                        | (((uint32_t)tangent_int.z & 0x3FF) << 20)
                        | (((uint32_t)tangent_int.y & 0x3FF) << 10)
                        | ((uint32_t)tangent_int.x & 0x3FF);

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

                    vec3i16 normal_int;
                    vec3_to_vec3i16(node->normal * 511.0f, normal_int);
                    *(uint32_t*)(data + 0x0C) = (((uint32_t)0 & 0x03) << 30)
                        | (((uint32_t)normal_int.z & 0x3FF) << 20)
                        | (((uint32_t)normal_int.y & 0x3FF) << 10)
                        | ((uint32_t)normal_int.x & 0x3FF);

                    data += mesh->size_vertex;
                }

                if (!double_sided)
                    break;
            }
        break;
    }

    buffer.UnmapMemory();
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

void ExClothBlock::Field_18(int32_t stage, bool disable_external_force) {

}

void ExClothBlock::Field_20() {
    ColiSet();

    rob_chara_item_equip* rob_itm_equip = item_equip_object->item_equip;
    float_t step = get_delta_frame() * rob_itm_equip->osage_step;
    if (rob_itm_equip->opd_blend_data.size() && rob_itm_equip->opd_blend_data.front().use_blend)
        step = 1.0f;
    sub_140218560(&rob, step, false);
}

void ExClothBlock::SetOsagePlayData() {
    rob.SetOsagePlayData(item_equip_object->item_equip->opd_blend_data);
}

void ExClothBlock::Disp(const mat4* mat, render_context* rctx) {
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
    ColiSet();
    sub_14021D480(&rob);
}

void ExClothBlock::Field_50() {
    ColiSet();
    sub_14021D840(&rob);
}

void ExClothBlock::AddMotionResetData(uint32_t motion_id, float_t frame) {
    rob.AddMotionResetData(motion_id, frame);
}

void ExClothBlock::ColiSet() {
    rob.ColiSet(mats);
    rob.SetWindDirection(task_wind->ptr->wind_direction);
}

void ExClothBlock::InitData(rob_chara_item_equip_object* itm_eq_obj,
    obj_skin_block_cloth* cls_data, skin_param_osage_root* skp_root, const bone_database* bone_data) {
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

const float_t* ExClothBlock::LoadOpdData(size_t node_index, const float_t* opd_data, size_t opd_count) {
    return rob.LoadOpdData(node_index, opd_data, opd_count);
}

void ExClothBlock::SetMotionResetData(uint32_t motion_id, float_t frame) {
    rob.SetMotionResetData(motion_id, frame);
}

const float_t* ExClothBlock::SetOsagePlayDataInit(const float_t* opdi_data) {
    return rob.SetOsagePlayDataInit(opdi_data);
}

void ExClothBlock::SetOsageReset() {
    rob.osage_reset = true;
}

void ExClothBlock::SetSkinParam(skin_param_file_data* skp) {
    rob.skin_param_ptr = &skp->skin_param;
}

void ExClothBlock::SetSkinParamOsageRoot(skin_param_osage_root* skp_root) {
    if (!skp_root)
        return;

    rob.SetSkinParamOsageRoot(*skp_root);

    vec3 wind_direction = task_wind->ptr->wind_direction
        * item_equip_object->item_equip->wind_strength;
    rob.SetWindDirection(wind_direction);
}

RobOsage::RobOsage() : skin_param_ptr(), osage_setting(), field_2A0(), field_2A1(), field_2A4(), wind_direction(),
field_1EB4(), yz_order(), field_1EBC(), root_matrix_ptr(), root_matrix(), move_cancel(), field_1F0C(),
osage_reset(), prev_osage_reset(), disable_collision(), set_external_force(), external_force(), reset_data_list() {

}

RobOsage::~RobOsage() {

}

void RobOsage::AddMotionResetData(uint32_t motion_id, float_t frame) {
    int32_t frame_int = (int32_t)prj::roundf(frame * 1000.0f);

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

// 0x14047EE90
void RobOsage::ApplyResetData(const mat4* mat) {
    if (reset_data_list) {
        auto reset_data = reset_data_list->begin();
        RobOsageNode* i_begin = nodes.data() + 1;
        RobOsageNode* i_end = nodes.data() + nodes.size();
        for (RobOsageNode* i = i_begin; i != i_end; i++)
            i->reset_data = *reset_data++;
        reset_data_list = 0;
    }

    RobOsageNode* i_begin = nodes.data() + 1;
    RobOsageNode* i_end = nodes.data() + nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++) {
        mat4_transform_vector(mat, &i->reset_data.trans_diff, &i->trans_diff);
        mat4_transform_point(mat, &i->reset_data.trans, &i->trans);
    }
    root_matrix = *root_matrix_ptr;
}

bool RobOsage::CheckPartsBits(rob_osage_parts_bit parts_bits) {
    if (osage_setting.parts != ROB_OSAGE_PARTS_NONE)
        return !!(parts_bits & (1 << osage_setting.parts));
    return false;
}

void RobOsage::ColiSet(const mat4* transform) {
    if (skin_param_ptr->coli.size())
        OsageCollision::Work::update_cls_work(coli, skin_param_ptr->coli.data(), transform);
    OsageCollision::Work::update_cls_work(coli_ring, ring.skp_root_coli, transform);
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
        if (node_array)
            for (size_t i = 0; i < osg_data->count; i++) {
                mat3 mat;
                mat3_rotate_zyx(&node_array[i].rotation, &mat);

                vec3 v49 = { v26[i].length, 0.0f, 0.0f };
                mat3_transform_vector(&mat, &v49, &v26[i].field_94);
            }
        else
            for (size_t i = 0; i < osg_data->count; i++)
                v26[i].field_94 = { v26[i].length, 0.0f, 0.0f };
    }

    if (osg_data->count) {
        RobOsageNode* v26 = &nodes.data()[1];
        size_t v33 = 0;
        for (uint32_t i = 0; i < osg_data->count; i++) {
            v26[i].mat = mat4_null;

            obj_skin_bone* bone = skin->bone_array;
            for (uint32_t j = 0; j < skin->num_bone; j++, bone++)
                if (bone->id == osg_nodes->name_index) {
                    mat4_invert_fast(&bone->inv_bind_pose_mat, &v26[i].mat);
                    break;
                }

            osg_nodes++;
        }
    }

    RobOsageNode* v38 = &nodes.back();
    if (memcmp(&v38->mat, &mat4_null, sizeof(mat4)))
        mat4_mul_translate(&v38->mat, node.length, 0.0f, 0.0f, &node.mat);
}

const float_t* RobOsage::LoadOpdData(size_t node_index, const float_t* opd_data, size_t opd_count) {
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
    skin_param_osage_root& skp_root, object_info* obj_info, const bone_database* bone_data) {
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
    prev_osage_reset = false;
    ring = osage_ring_data();
    disable_collision = false;
    skin_param.reset();
    skin_param_ptr = &skin_param;
    osage_setting = osage_setting_osg_cat();
    reset_data_list = 0;
    field_2A4 = 0.0f;
    field_2A1 = false;
    set_external_force = false;
    external_force = 0.0f;
    root_matrix_ptr = 0;
    root_matrix = mat4_null;
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

void RobOsage::SetMotionResetData(uint32_t motion_id, float_t frame) {
    osage_reset = true;
    auto elem = motion_reset_data.find({ motion_id, (int32_t)prj::roundf(frame * 1000.0f) });
    if (elem != motion_reset_data.end() && elem->second.size() + 1 == nodes.size())
        reset_data_list = &elem->second;
}

// 0x140480F40
void RobOsage::SetNodesExternalForce(vec3* external_force, float_t strength) {
    if (!external_force) {
        RobOsageNode* i_begin = nodes.data() + 1;
        RobOsageNode* i_end = nodes.data() + nodes.size();
        for (RobOsageNode* i = i_begin; i != i_end; i++)
            i->external_force = 0.0f;
        return;
    }

    vec3 v4 = *external_force;
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

// 0x140481540
void RobOsage::SetNodesForce(float_t force) {
    RobOsageNode* i_begin = nodes.data() + 1;
    RobOsageNode* i_end = nodes.data() + nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++)
        i->force = force;
}

// 0x14047E240
void RobOsage::SetOsagePlayData(const mat4* root_matrix,
    const vec3& parent_scale, std::vector<opd_blend_data>& opd_blend_data) {
    if (!opd_blend_data.size())
        return;

    vec3 v63 = exp_data.position * parent_scale;

    mat4 v85 = *root_matrix;
    mat4_transform_point(&v85, &v63, &nodes.data()[0].trans);

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

        int32_t curr_key = (int32_t)(int64_t)prj::floorf(frame);
        int32_t next_key = curr_key + 1;
        if ((float_t)next_key >= i->frame_count)
            next_key = 0;

        float_t blend = frame - (float_t)(int64_t)frame;
        float_t inv_blend = 1.0f - blend;
        RobOsageNode* j_begin = nodes.data() + 1;
        RobOsageNode* j_end = nodes.data() + nodes.size();
        for (RobOsageNode* j = j_begin; j != j_end; j++) {
            opd_vec3_data* opd = &j->opd_data[i - i_end];
            const float_t* opd_x = opd->x;
            const float_t* opd_y = opd->y;
            const float_t* opd_z = opd->z;

            vec3 v62;
            v62.x = opd_x[curr_key];
            v62.y = opd_y[curr_key];
            v62.z = opd_z[curr_key];

            vec3 v77;
            v77.x = opd_x[next_key];
            v77.y = opd_y[next_key];
            v77.z = opd_z[next_key];

            mat4_transform_point(root_matrix, &v62, &v62);
            mat4_transform_point(root_matrix, &v77, &v77);

            vec3 v82 = v62 * inv_blend + v77 * blend;

            vec3 v86;
            mat4_inverse_transform_point(&v87, &v82, &v86);

            vec3 rotation = 0.0f;
            sub_140482FF0(v87, v86, 0, &rotation, yz_order);
            mat4_set_translation(&v87, &v82);

            float_t length = vec3::distance(v62, v22) * inv_blend + vec3::distance(v77, v25) * blend;
            j->opd_node_data.set_data(i, { length, rotation });

            v22 = v62;
            v25 = v77;
        }
    }

    RobOsageNode* j_begin = nodes.data() + 1;
    RobOsageNode* j_end = nodes.data() + nodes.size();
    for (RobOsageNode* j = j_begin; j != j_end; j++) {
        float_t rot_y = j->opd_node_data.curr.rotation.y;
        float_t rot_z = j->opd_node_data.curr.rotation.z;
        rot_y = clamp_def(rot_y, (float_t)-M_PI, (float_t)M_PI);
        rot_z = clamp_def(rot_z, (float_t)-M_PI, (float_t)M_PI);
        mat4_mul_rotate_z(&v85, rot_z, &v85);
        mat4_mul_rotate_y(&v85, rot_y, &v85);
        j->bone_node_ptr->exp_data.parent_scale = parent_scale;
        *j->bone_node_ptr->ex_data_mat = v85;

        if (j->bone_node_mat)
            mat4_scale_rot(&v85, &parent_scale, j->bone_node_mat);

        mat4_mul_translate(&v85, j->opd_node_data.curr.length, 0.0f, 0.0f, &v85);
        j->trans_orig = j->trans;
        mat4_get_translation(&v85, &j->trans);
    }

    if (nodes.size() && node.bone_node_mat) {
        mat4 v87 = *nodes.back().bone_node_ptr->ex_data_mat;
        mat4_mul_translate(&v87, node.length * parent_scale.x, 0.0f, 0.0f, &v87);
        *node.bone_node_ptr->ex_data_mat = v87;

        mat4_scale_rot(&v87, &parent_scale, &v87);
        *node.bone_node_mat = v87;
        node.bone_node_ptr->exp_data.parent_scale = parent_scale;
    }
}

const float_t* RobOsage::SetOsagePlayDataInit(const float_t* opdi_data) {
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

void RobOsage::SetSkinParamOsageRoot(const skin_param_osage_root& skp_root) {
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
    skin_param_ptr->coli.assign(skp_root.coli.begin(), skp_root.coli.end());
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

ExOsageBlock::ExOsageBlock() : rob(), mats(), field_1FF8() {
    step = 1.0f;
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

void ExOsageBlock::Field_18(int32_t stage, bool disable_external_force) {
    rob_chara_item_equip* rob_itm_equip = item_equip_object->item_equip;
    float_t step = get_delta_frame() * rob_itm_equip->osage_step;
    if (rob_itm_equip->opd_blend_data.size() && rob_itm_equip->opd_blend_data.front().use_blend)
        step = 1.0f;

    mat4* root_matrix = parent_bone_node->ex_data_mat;
    vec3 parent_scale = parent_bone_node->exp_data.parent_scale;
    switch (stage) {
    case 0:
        sub_1404803B0(&rob, root_matrix, &parent_scale, has_children_node);
        break;
    case 1:
    case 2:
        if ((stage == 1 && field_58) || (stage == 2 && rob.field_2A0)) {
            SetWindDirection();
            sub_14047C800(&rob, root_matrix, &parent_scale, step,
                disable_external_force, true, has_children_node);
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
        sub_14047D8C0(&rob, root_matrix, &parent_scale, step, false);
        sub_140480260(&rob, root_matrix, &parent_scale, step, disable_external_force);
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
    float_t step = get_delta_frame() * rob_itm_equip->osage_step;
    if (rob_itm_equip->opd_blend_data.size() && rob_itm_equip->opd_blend_data.front().use_blend)
        step = 1.0f;

    vec3 parent_scale = parent_bone_node->exp_data.parent_scale;
    vec3 scale = parent_bone_node->exp_data.scale;

    mat4 root_matrix = *parent_bone_node->ex_data_mat;
    if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f) {
        scale = 1.0f / scale;
        mat4_scale_rot(&root_matrix, &scale, &root_matrix);
    }
    SetWindDirection();
    rob.ColiSet(mats);
    sub_14047C750(&rob, &root_matrix, &parent_scale, step);
}

void ExOsageBlock::SetOsagePlayData() {
    rob.SetOsagePlayData(parent_bone_node->ex_data_mat,
        parent_bone_node->exp_data.parent_scale, item_equip_object->item_equip->opd_blend_data);
}

void ExOsageBlock::Disp(const mat4* mat, render_context* rctx) {

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
    sub_14047F990(&rob, parent_bone_node->ex_data_mat, &parent_scale, false);
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
    sub_14047C770(&rob, parent_bone_node->ex_data_mat, &parent_scale, step, true);
    float_t step = 0.5f * this->step;
    this->step = max_def(step, 1.0f);
}

void ExOsageBlock::Field_58() {
    vec3 parent_scale = parent_bone_node->exp_data.parent_scale;
    sub_14047E1C0(&rob, &parent_scale);
    field_59 = false;
}

void ExOsageBlock::AddMotionResetData(uint32_t motion_id, float_t frame) {
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

const float_t* ExOsageBlock::LoadOpdData(size_t node_index, const float_t* opd_data, size_t opd_count) {
    return rob.LoadOpdData(node_index, opd_data, opd_count);
}

void ExOsageBlock::SetDisableCollision(bool value) {
    rob.disable_collision = value;
}

void ExOsageBlock::SetMotionResetData(uint32_t motion_id, float_t frame) {
    rob.SetMotionResetData(motion_id, frame);
}

const float_t* ExOsageBlock::SetOsagePlayDataInit(const float_t* opdi_data) {
    return rob.SetOsagePlayDataInit(opdi_data);
}

void ExOsageBlock::SetOsageReset() {
    rob.osage_reset = true;
}

void ExOsageBlock::SetSkinParam(skin_param_file_data* skp) {
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
    rob.wind_direction = task_wind->ptr->wind_direction
        * item_equip_object->item_equip->wind_strength;
}

void ExOsageBlock::sub_1405F3E10(obj_skin_block_osage* osg_data,
    obj_skin_osage_node* osg_nodes, prj::vector_pair<uint32_t, RobOsageNode*>* a4,
    std::map<const char*, ExNodeBlock*>* a5) {
    RobOsageNode* node = rob.GetNode(0);
    a4->push_back(osg_data->external_name_index, node);

    for (uint32_t i = 0; i < osg_data->count; i++) {
        node = rob.GetNode(i + 1ULL);
        a4->push_back(osg_nodes[i].name_index, node);

        if (node->bone_node_ptr && node->bone_node_ptr->name)
            a5->insert({ node->bone_node_ptr->name, this });
    }

    node = rob.GetNode(osg_data->count + 1ULL);
    a4->push_back(osg_data->name_index, node);

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

void ExConstraintBlock::Field_18(int32_t stage, bool disable_external_force) {
    if (field_59)
        return;

    switch (stage) {
    case 0:
        if (field_58)
            Field_20();
        break;
    case 2:
        if (has_children_node)
            DataSet();
        break;
    case 5:
        Field_20();
        break;
    }
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

void ExConstraintBlock::Disp(const mat4* mat, render_context* rctx) {

}

void ExConstraintBlock::Field_40() {

}

void ExConstraintBlock::Field_48() {
    Field_20();
}

void ExConstraintBlock::Field_50() {
    Field_20();
}

static void sub_1401EB410(mat4& mat, vec3& in_v1, vec3& in_v2) {
    const vec3 v1 = vec3::normalize(in_v1);
    const vec3 v2 = vec3::normalize(in_v2);
    const vec3 axis = vec3::cross(v2, v1);

    float_t c = clamp_def(vec3::dot(v2, v1), -1.0f, 1.0f);
    float_t s = sqrtf(clamp_def(1.0f - c * c, 0.0f, 1.0f));
    mat4_set(&axis, -s, c, &mat);
}

void ExConstraintBlock::Calc() {
    bone_node* node = bone_node_ptr;
    if (!node)
        return;

    vec3 pos = parent_bone_node->exp_data.parent_scale * node->exp_data.position;

    mat4 mat;
    mat4_mul_translate(parent_bone_node->ex_data_mat, &pos, &mat);

    switch (constraint_type) {
    case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION: {
        obj_skin_block_constraint_orientation* orientation = cns_data->orientation;

        vec3 trans;
        mat4_get_translation(&mat, &trans);

        mat3 rot;
        mat4_to_mat3(source_node_bone_node->mat, &rot);
        mat3_normalize_rotation(&rot, &rot);
        mat3_mul_rotate_zyx(&rot, &orientation->offset, &rot);
        mat4_from_mat3(&rot, node->mat);
        mat4_set_translation(node->mat, &trans);
    } break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION: {
        obj_skin_block_constraint_direction* direction = cns_data->direction;

        vec3 align_axis = direction->align_axis;
        vec3 target_offset = direction->target_offset;
        mat4_transform_point(source_node_bone_node->mat, &target_offset, &target_offset);
        mat4_inverse_transform_point(&mat, &target_offset, &target_offset);
        float_t target_offset_length = vec3::length_squared(target_offset);
        if (target_offset_length <= 0.000001f)
            break;

        mat4 v59;
        sub_1401EB410(v59, align_axis, target_offset);
        if (direction_up_vector_bone_node) {
            vec3 affected_axis = direction->up_vector.affected_axis;
            mat4 v56;
            mat4_mul(&v59, &mat, &v56);
            mat4_transform_vector(&v56, &affected_axis, &affected_axis);
            mat4* v20 = direction_up_vector_bone_node->mat;

            vec3 v23;
            vec3 v24;
            mat4_get_translation(v20, &v23);
            mat4_get_translation(&v56, &v24);
            v23 = v23 - v24;

            vec3 v50;
            mat4_transform_vector(&mat, &target_offset, &v50);

            vec3 v25 = vec3::normalize(vec3::cross(v50, affected_axis));
            vec3 v29 = vec3::normalize(vec3::cross(v50, v23));

            vec3 v35 = vec3::cross(v25, v29);

            float_t v39 = vec3::dot(v29, v25);
            float_t v36 = vec3::dot(v35, v50);

            float_t v40 = vec3::length(v35);
            if (v36 >= 0.0f)
                v40 = -v40;

            mat4_set(&target_offset, -v40, v39, &v56);
            mat4_mul(&v59, &v56, &v59);
        }
        mat4_mul(&v59, &mat, node->mat);
    } break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION: {
        obj_skin_block_constraint_position* position = cns_data->position;

        vec3 constraining_offset = position->constraining_object.offset;
        vec3 constrained_offset = position->constrained_object.offset;
        if (position->constraining_object.affected_by_orientation)
            mat4_transform_vector(source_node_bone_node->mat, &constraining_offset, &constraining_offset);

        vec3 source_node_trans;
        mat4_get_translation(source_node_bone_node->mat, &source_node_trans);
        source_node_trans = constraining_offset + source_node_trans;
        mat4_set_translation(&mat, &source_node_trans);
        if (direction_up_vector_bone_node) {
            vec3 up_vector_trans;
            mat4_get_translation(direction_up_vector_bone_node->mat, &up_vector_trans);
            mat4_inverse_transform_point(&mat, &up_vector_trans, &up_vector_trans);

            mat4 v26;
            sub_1401EB410(v26, position->up_vector.affected_axis, up_vector_trans);
            mat4_mul(&v26, &mat, &mat);
        }
        if (position->constrained_object.affected_by_orientation)
            mat4_transform_vector(&mat, &constrained_offset, &constrained_offset);

        mat4 constrained_offset_mat;
        mat4_translate(&constrained_offset, &constrained_offset_mat);
        mat4_mul(&mat, &constrained_offset_mat, node->mat);
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
    mat4_invert_fast(parent_bone_node->ex_data_mat, &mat);
    mat4_mul(bone_node_ptr->mat, &mat, &mat);
    mat4_get_rotation(&mat, &exp_data->rotation);
    mat4_get_translation(&mat, &exp_data->position);
    if (fabsf(parent_scale.x) > 0.000001f)
        exp_data->position.x /= parent_scale.x;
    if (fabsf(parent_scale.y) > 0.000001f)
        exp_data->position.y /= parent_scale.z;
    if (fabsf(parent_scale.z) > 0.000001f)
        exp_data->position.z /= parent_scale.z;
    *bone_node_ptr->ex_data_mat = *bone_node_ptr->mat;
    mat4_scale_rot(bone_node_ptr->mat, &parent_scale, bone_node_ptr->mat);
    exp_data->parent_scale = exp_data->scale * parent_scale;
}

void ExConstraintBlock::InitData(rob_chara_item_equip_object* itm_eq_obj,
    obj_skin_block_constraint* cns_data, const char* cns_data_name, const bone_database* bone_data) {
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

void ExExpressionBlock::Field_18(int32_t stage, bool disable_external_force) {
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

void ExExpressionBlock::Disp(const mat4* mat, render_context* rctx) {

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
        delta_frame *= item_equip_object->item_equip->osage_step;
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
    size_t index, const bone_database* bone_data) {
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
    return prj::roundf(v1);
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
static void closest_pt_segment_segment(vec3& vec, const vec3& p0, const vec3& q0, const OsageCollision::Work* cls) {
    const vec3& p1 = cls->pos[0];
    const vec3& q1 = cls->pos[1];

    vec3 d0 = q0 - p0;
    vec3 d1 = cls->vec_center;
    if (vec3::length_squared(vec3::cross(d0, d1)) <= 0.000001f) {
        vec3 p0_proj;
        vec3 q0_proj;
        vec3 p1_proj;
        vec3 q1_proj;
        OsageCollision::get_nearest_line2point(p0_proj, p1, q1, p0);
        OsageCollision::get_nearest_line2point(q0_proj, p1, q1, q0);
        OsageCollision::get_nearest_line2point(p1_proj, p0, q0, p1);
        OsageCollision::get_nearest_line2point(q1_proj, p0, q0, q1);

        float_t p0_dist = vec3::distance_squared(p0, p0_proj);
        float_t q0_dist = vec3::distance_squared(q0, q0_proj);
        float_t p1_dist = vec3::distance_squared(p1, p1_proj);
        float_t q1_dist = vec3::distance_squared(q1, q1_proj);

        float_t dist = p0_dist;
        vec = p0;

        if (dist > q0_dist) {
            vec = q0;
            dist = q0_dist;
        }

        if (dist > p1_dist) {
            vec = p1_proj;
            dist = p1_dist;
        }

        if (dist > q1_dist) {
            vec = q1_proj;
            dist = q1_dist;
        }
    }
    else {
        float_t d0_len = vec3::length(d0);
        if (d0_len != 0.0f)
            d0 *= 1.0f / d0_len;

        float_t d1_len = cls->vec_center_length;
        if (d1_len != 0.0f)
            d1 *= 1.0f / d1_len;

        float_t b = vec3::dot(d1, d0);
        float_t t = vec3::dot(d1 * b - d0, p1 - p0) / (b * b - 1.0f);
        if (t < 0.0f)
            vec = p0;
        else if (t <= d0_len)
            vec = p0 + d0 * t;
        else
            vec = q0;
    }
}

static void sub_140218560(RobCloth* rob_cls, float_t step, bool a3) {
    sub_140219940(rob_cls);
    if (rob_cls->osage_reset) {
        rob_cls->ApplyResetData();
        rob_cls->osage_reset = false;
    }

    if (rob_cls->move_cancel > 0.0f) {
        size_t root_count = rob_cls->root_count;
        size_t nodes_count = rob_cls->nodes_count;

        float_t move_cancel = rob_cls->move_cancel;
        for (size_t i = 0; i < root_count; i++) {
            mat4& mat = rob_cls->root.data()[i].field_D8;
            CLOTHNode* node = &rob_cls->nodes.data()[i + root_count];
            for (size_t j = 1; j < nodes_count; j++, node += root_count) {
                vec3 trans;
                mat4_transform_point(&mat, &node->reset_data.trans, &trans);
                node->trans += (trans - node->trans) * move_cancel;
            }
        }
    }

    if (step > 0.0f && !get_pause()) {
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

    vec3 external_force = rob_cls->wind_direction * rob_cls->skin_param_ptr->wind_afc;
    if (rob_cls->set_external_force) {
        external_force += rob_cls->external_force;
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
            mat4_transform_vector(&mat, &node->field_64, &v37);

            float_t v25;
            if (!a2)
                v25 = v7;
            else if (node->trans_diff.y >= 0.0f)
                v25 = 1.0f;
            else
                v25 = 0.0f;

            v37 = v37 * force - node->trans_diff * v25 + external_force;
            v37.y -= osage_gravity;
            node->trans_diff += v37;

            node->field_1C = node->trans;
            node->trans += node->trans_diff;
        }
        force *= rob_cls->skin_param_ptr->force_gain;
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
            mat4_mul(root.mat[j], root.node_mat[j], &mat);

            float_t weight = root.weight[j];
            mat4_mul_scale(&mat, weight, weight, weight, weight, &mat);
            mat4_add(&m, &mat, &m);
        }
        root.field_98 = m;

        mat4_transform_point(&m, &root_node.trans_orig, &root_node.trans);
        mat4_transform_vector(&m, &root.normal, &root_node.normal);
        mat4_transform_vector(&m, (vec3*)&root.tangent, &root_node.tangent);
        root_node.tangent_sign = root.tangent.w;
        root_node.field_1C = root_node.trans;

        mat4_mul_translate(&m, &root_node.trans_orig, &m);
        root.field_D8 = m;
        mat4_invert(&m, &m);
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
            for (ssize_t j = root_count; j > 0; j--, v27++, v28++, v36++)
                v36->trans = (*v27 + *v28) * 0.5f;
        }
        v16 += root_count;
        v15 += root_count;
    }

    operator delete(v11);
    operator delete(v12);
}

static float_t sub_14021A290(const vec3& trans_a, const vec3& trans_b, const vec3& trans_c,
    const vec2& texcoord_a, const vec2& texcoord_b, const vec2& texcoord_c,
    vec3& tangent, vec3& binormal, vec3& normal) {
    vec3 pos_a = trans_b - trans_a;
    vec3 pos_b = trans_c - trans_a;

    normal = vec3::cross(pos_a, pos_b);

    float_t normal_length = vec3::length(normal);
    if (normal_length <= 0.000001f)
        return 1.0f;

    normal *= 1.0f / normal_length;

    vec2 uv_a = texcoord_b - texcoord_a;
    vec2 uv_b = texcoord_c - texcoord_a;

    float_t r = 1.0f / (uv_b.y * uv_a.x - uv_a.y * uv_b.x);

    vec3 _tangent = vec3::normalize((pos_a * uv_b.y - pos_b * uv_a.y) * r);
    binormal = vec3::cross(_tangent, normal);
    tangent = vec3::cross(normal, binormal);

    mat3 mat;
    mat.row0 = tangent;
    mat.row1 = binormal;
    mat.row2 = normal;
    mat3_transpose(&mat, &mat);
    return mat3_determinant(&mat);
}

static float_t sub_14021A5E0(const vec3& pos_a, const vec3& pos_b,
    const float_t uv_a_y, const float_t uv_b_y,
    vec3& tangent, vec3& binormal, vec3& normal) {
    normal = vec3::normalize(vec3::cross(pos_a, pos_b));

    vec3 _tangent = vec3::normalize(pos_a * uv_b_y - pos_b * uv_a_y);
    binormal = vec3::cross(_tangent, normal);
    tangent = vec3::cross(normal, binormal);

    mat3 mat;
    mat.row0 = tangent;
    mat.row1 = binormal;
    mat.row2 = normal;
    mat3_transpose(&mat, &mat);
    return mat3_determinant(&mat);
}

static void sub_14021A890(CLOTHNode* a1, CLOTHNode* a2, CLOTHNode* a3, CLOTHNode* a4, CLOTHNode* a5) {
    vec3 pos_a = a4->trans - a5->trans;
    vec3 pos_b = a3->trans - a2->trans;

    float_t pos_b_length = vec3::length(pos_a);
    float_t pos_a_length = vec3::length(pos_b);
    if (pos_b_length <= 0.000001f || pos_a_length <= 0.000001f)
        return;

    vec2 uv_a = a4->texcoord - a5->texcoord;
    vec2 uv_b = a3->texcoord - a2->texcoord;

    float_t r = uv_b.y * uv_a.x - uv_a.y * uv_b.x;
    if (fabsf(r) > 0.000001f) {
        r = 1.0f / r;
        a1->tangent_sign = sub_14021A5E0(pos_a, pos_b,
            uv_a.y * r, uv_b.y * r, a1->tangent, a1->binormal, a1->normal);
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
                vec3 v60 = node->trans - node->field_1C;

                float_t v30 = vec3::length(v60);
                if (v30 * step > 0.0f && v30 != 0.0f)
                    v60 *= 1.0f / v30;

                node->trans = node->field_1C + v60 * (v30 * step);
            }
            sub_140482F30(&node[0].trans, &node[-root_count].trans, node[0].dist_top);

            int32_t v39 = OsageCollision::osage_cls_work_list(node->trans,
                rob_cls->skin_param_ptr->coli_r, rob_cls->ring.coli, &fric);
            v39 += OsageCollision::osage_cls(rob_cls->coli_ring, node->trans, rob_cls->skin_param_ptr->coli_r);
            v39 += OsageCollision::osage_cls(rob_cls->coli, node->trans, rob_cls->skin_param_ptr->coli_r);

            if (v19 > node->trans.y && v19 < 1001.0) {
                node->trans.y = v19;
                node->trans_diff = 0.0f;
            }

            mat4 mat = root->field_98;
            mat4_set_translation(&mat, &node[-root_count].trans);

            int32_t yz_order = 1;
            sub_140482FF0(mat, node->field_64, 0, 0, yz_order);

            vec3 v74;
            mat4_inverse_transform_point(&mat, &node->trans, &v74);

            sub_140482FF0(mat, v74, &rob_cls->skin_param_ptr->hinge,
                &node->reset_data.rotation, yz_order);
            mat4_mul_translate(&mat, node->dist_top, 0.0f, 0.0f, &mat);
            mat4_get_translation(&mat, &node->trans);

            if (v39)
                node->trans_diff *= fric;

            node->trans_diff = (node->trans - node->field_1C) * v10;

            if (!a3) {
                mat4& v49 = rob_cls->root.data()[j].field_118;
                mat4_transform_point(&v49, &node->trans, &node->reset_data.trans);
                mat4_transform_vector(&v49, &node->trans_diff, &node->reset_data.trans_diff);
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
            mat4_transform_vector(&mat, &node->field_64, &v38);
            v38.y -= osage_gravity_const;

            node[0].trans = node[-root_count].trans + vec3::normalize(v38) * node->dist_top;

            OsageCollision::osage_cls_work_list(node->trans,
                rob_cls->skin_param_ptr->coli_r, rob_cls->ring.coli);
            OsageCollision::osage_cls(rob_cls->coli_ring, node->trans, rob_cls->skin_param_ptr->coli_r);
            OsageCollision::osage_cls(rob_cls->coli, node->trans, rob_cls->skin_param_ptr->coli_r);

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

static void sub_14047C800(RobOsage* rob_osg, const mat4* root_matrix,
    const vec3* parent_scale, float_t step, bool disable_external_force,
    bool ring_coli, bool has_children_node) {
    if (!rob_osg->nodes.size())
        return;

    const float_t osage_gravity_const = get_osage_gravity_const();
    sub_1404803B0(rob_osg, root_matrix, parent_scale, false);

    RobOsageNode* v17 = &rob_osg->nodes.data()[0];
    v17->trans_orig = v17->trans;
    vec3 v113 = rob_osg->exp_data.position * *parent_scale;

    mat4 v130 = *root_matrix;
    mat4_transform_point(&v130, &v113, &v17->trans);
    v17->trans_diff = v17->trans - v17->trans_orig;

    sub_14047F110(rob_osg, &v130, parent_scale, false);
    *rob_osg->nodes.data()[0].bone_node_mat = v130;
    *rob_osg->nodes.data()[0].bone_node_ptr->ex_data_mat = v130;

    v113 = { 1.0f, 0.0f, 0.0f };
    vec3 v128;
    mat4_transform_vector(&v130, &v113, &v128);

    float_t v25 = parent_scale->x;
    if (!rob_osg->osage_reset && (get_pause() || step <= 0.0f))
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
                v111 = (v111 + v112) * 0.5f;
            }
        }
        else
            v111 = rob_osg->external_force * (1.0f / weight);

        vec3 v127 = v128 * (v31->force * v26->force);
        float_t v41 = (1.0f - rob_osg->field_1EB4) * (1.0f - rob_osg->skin_param_ptr->air_res);

        vec3 v126 = v111 + v127 - v26->trans_diff * v41 + v26->external_force * weight;

        if (!disable_external_force)
            v126 += rob_osg->wind_direction * rob_osg->skin_param_ptr->wind_afc;

        if (stiffness)
            v126 -= (v26->trans_diff - v30->trans_diff) * (1.0f - rob_osg->skin_param_ptr->air_res);

        v26->field_28 = v126 * (1.0f / (weight - (weight - 1.0f) * v31->skp_osg_node.inertial_cancel));
    }

    if (stiffness) {
        mat4 v131 = v130;
        vec3 v111;
        mat4_get_translation(&v131, &v111);

        RobOsageNode* v55_begin = rob_osg->nodes.data() + 1;
        RobOsageNode* v55_end = rob_osg->nodes.data() + rob_osg->nodes.size();
        for (RobOsageNode* v55 = v55_begin; v55 != v55_end; v55++) {
            mat4_transform_point(&v131, &v55->field_94, &v128);

            vec3 v123 = v55->trans_diff + v55->field_28;
            vec3 v126 = v55->trans + v123;
            sub_140482F30(&v126, &v111, v25 * v55->length);

            vec3 v117 = (v128 - v126) * rob_osg->skin_param_ptr->stiffness;
            float_t weight = v55->data_ptr->skp_osg_node.weight;
            float_t v74 = 1.0f / (weight - (weight - 1.0f) * v55->data_ptr->skp_osg_node.inertial_cancel);
            v55->field_28 += v117 * v74;

            v126 = v55->trans + v117 + v123;

            vec3 v129;
            mat4_inverse_transform_point(&v131, &v126, &v129);

            sub_140482FF0(v131, v129, 0, 0, rob_osg->yz_order);

            mat4_mul_translate(&v131, vec3::distance(v111, v126), 0.0f, 0.0f, &v131);

            v111 = v126;
        }
    }

    RobOsageNode* v82_begin = rob_osg->nodes.data() + 1;
    RobOsageNode* v82_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (RobOsageNode* v82 = v82_begin; v82 != v82_end; v82++) {
        v82->trans_orig = v82->trans;
        v82->trans_diff += v82->field_28;
        v82->trans += v82->trans_diff;
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

        float_t ring_y = ring_height + coli_r;
        RobOsageNode* v98_begin = rob_osg->nodes.data() + 1;
        RobOsageNode* v98_end = rob_osg->nodes.data() + rob_osg->nodes.size();
        for (RobOsageNode* v98 = v98_begin; v98 != v98_end; v98++) {
            sub_140482490(v98, step, v25);
            sub_140482180(v98, ring_y);
        }
    }

    if (has_children_node) {
        RobOsageNode* v100 = rob_osg->nodes.data();
        RobOsageNode* v99_begin = rob_osg->nodes.data() + 1;
        RobOsageNode* v99_end = rob_osg->nodes.data() + rob_osg->nodes.size();
        for (RobOsageNode* v99 = v99_begin; v99 != v99_end; v99++, v100++) {
            vec3 v129;
            mat4_inverse_transform_point(&v130, &v99->trans, &v129);
            bool v102 = sub_140482FF0(v130, v129,
                &v99->data_ptr->skp_osg_node.hinge,
                &v99->reset_data.rotation, rob_osg->yz_order);
            *v99->bone_node_ptr->ex_data_mat = v130;

            float_t v104 = vec3::distance_squared(v99->trans, v100->trans);
            float_t v105 = v99->length * v25;

            bool v106;
            if (v104 >= v105 * v105) {
                v105 = sqrtf(v104);
                v106 = false;
            }
            else
                v106 = true;

            mat4_mul_translate(&v130, v105, 0.0f, 0.0f, &v130);
            if (v102 || v106)
                mat4_get_translation(&v130, &v99->trans);
        }

        if (rob_osg->nodes.size() && rob_osg->node.bone_node_mat) {
            mat4 v131 = *rob_osg->nodes.back().bone_node_ptr->ex_data_mat;
            mat4_mul_translate(&v131, v25 * rob_osg->node.length, 0.0f, 0.0f, &v131);
            *rob_osg->node.bone_node_ptr->ex_data_mat = v131;
        }
    }
    rob_osg->field_2A0 = false;
}

static void sub_14047E1C0(RobOsage* rob_osg, vec3* scale) {
    RobOsageNode* i_begin = rob_osg->nodes.data() + 1;
    RobOsageNode* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++)
        if (i->data_ptr->normal_ref.set) {
            sub_14053CE30(&i->data_ptr->normal_ref, i->bone_node_mat);
            mat4_scale_rot(i->bone_node_mat, scale, i->bone_node_mat);
        }
}

static void sub_14047F110(RobOsage* rob_osg, mat4* mat, const vec3* parent_scale, bool init_rot) {
    vec3 position = rob_osg->exp_data.position * *parent_scale;
    mat4_mul_translate(mat, &position, mat);
    mat4_mul_rotate_zyx(mat, &rob_osg->exp_data.rotation, mat);

    skin_param* skin_param = rob_osg->skin_param_ptr;
    vec3 rot = skin_param->rot;
    if (init_rot)
        rot = skin_param->init_rot + rot;
    mat4_mul_rotate_zyx(mat, &rot, mat);
}

static void sub_1404803B0(RobOsage* rob_osg, const mat4* root_matrix,
    const vec3* parent_scale, bool has_children_node) {
    mat4 v47 = *root_matrix;
    vec3 v45 = rob_osg->exp_data.position * *parent_scale;
    mat4_transform_point(&v47, &v45, &rob_osg->nodes.data()[0].trans);

    if (rob_osg->osage_reset && !rob_osg->prev_osage_reset) {
        rob_osg->prev_osage_reset = true;
        rob_osg->ApplyResetData(root_matrix);
    }

    if (!rob_osg->field_1F0C) {
        rob_osg->field_1F0C = true;

        float_t move_cancel = rob_osg->skin_param_ptr->move_cancel;
        if (rob_osg->move_cancel == 1.0f || move_cancel < 0.0f)
            move_cancel = rob_osg->move_cancel;

        if (move_cancel > 0.0f) {
            RobOsageNode* i_begin = rob_osg->nodes.data() + 1;
            RobOsageNode* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
            for (RobOsageNode* i = i_begin; i != i_end; i++) {
                vec3 v44;
                mat4_inverse_transform_point(&rob_osg->root_matrix, &i->trans, &v44);
                mat4_transform_point(rob_osg->root_matrix_ptr, &v44, &v44);
                i->trans += (v44 - i->trans) * move_cancel;
            }
        }
    }

    if (!has_children_node)
        return;

    sub_14047F110(rob_osg, &v47, parent_scale, false);
    *rob_osg->nodes.data()[0].bone_node_mat = v47;

    RobOsageNode* v29 = rob_osg->nodes.data();
    RobOsageNode* v30_begin = rob_osg->nodes.data() + 1;
    RobOsageNode* v30_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (RobOsageNode* v30 = v30_begin; v30 != v30_end; v29++, v30++) {
        vec3 v45;
        mat4_inverse_transform_point(&v47, &v30->trans, &v45);
        bool v32 = sub_140482FF0(v47, v45, &v30->data_ptr->skp_osg_node.hinge,
            &v30->reset_data.rotation, rob_osg->yz_order);
        *v30->bone_node_ptr->ex_data_mat = v47;

        float_t v34 = vec3::distance_squared(v30->trans, v29->trans);
        float_t v35 = v30->length * parent_scale->x;

        bool v36;
        if (v34 >= v35 * v35) {
            v35 = sqrtf(v34);
            v36 = false;
        }
        else
            v36 = true;

        mat4_mul_translate(&v47, v35, 0.0f, 0.0f, &v47);
        if (v32 || v36)
            mat4_get_translation(&v47, &v30->trans);
    }

    if (rob_osg->nodes.size() && rob_osg->node.bone_node_mat) {
        mat4 v48 = *rob_osg->nodes.back().bone_node_ptr->ex_data_mat;
        mat4_mul_translate(&v48, parent_scale->x * rob_osg->node.length, 0.0f, 0.0f, &v48);
        *rob_osg->node.bone_node_ptr->ex_data_mat = v48;
    }
}

static void sub_140482180(RobOsageNode* node, float_t ring_y) {
    float_t v2 = node->data_ptr->skp_osg_node.coli_r + ring_y;
    if (v2 <= node->trans.y)
        return;

    node->trans.y = v2;
    node->trans = node[-1].trans + vec3::normalize(node->trans - node[-1].trans) * node->length;
    node->trans_diff = 0.0f;
    node->field_C8 += 1.0f;
}

static void sub_140482300(vec3* a1, vec3* a2, vec3* a3, float_t osage_gravity_const, float_t weight) {
    weight *= osage_gravity_const;
    vec3 diff = *a2 - *a3;
    float_t dist = vec3::length(diff);
    if (dist <= fabsf(osage_gravity_const)) {
        diff.y = osage_gravity_const;
        dist = vec3::length(diff);
    }
    diff = -(diff * (weight * diff.y * (1.0f / (dist * dist))));
    diff.y -= weight;
    *a1 = diff;
}

static void sub_140482490(RobOsageNode* node, float_t step, float_t a3) {
    if (step != 1.0f) {
        vec3 v4 = node->trans - node->trans_orig;

        float_t v9 = vec3::length(v4);
        if (v9 != 0.0f)
            v4 *= 1.0f / v9;
        node->trans = node->trans_orig + v4 * (step * v9);
    }

    sub_140482F30(&node[0].trans, &node[-1].trans, node->length * a3);
    if (node->sibling_node)
        sub_140482F30(&node->trans, &node->sibling_node->trans, node->max_distance);
}

static void sub_140482F30(vec3* pos1, vec3* pos2, float_t length) {
    vec3 diff = *pos1 - *pos2;
    float_t dist = vec3::length_squared(diff);
    if (dist > length * length)
        *pos1 = *pos2 + diff * (length / sqrtf(dist));
}

static void sub_14047D8C0(RobOsage* rob_osg, const mat4* root_matrix,
    const vec3* parent_scale, float_t step, bool a5) {
    if (!rob_osg->osage_reset && (get_pause() || step <= 0.0f))
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
        v64 = *root_matrix;

        vec3 trans = rob_osg->exp_data.position * *parent_scale;
        mat4_transform_point(&v64, &trans, &rob_osg->nodes.data()[0].trans);
        sub_14047F110(rob_osg, &v64, parent_scale, false);
    }

    float_t ring_y = -1000.0f;
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

        ring_y = ring_height + coli_r;
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
            v27->trans += v62;
        }
    }

    RobOsageNode* v35_begin = rob_osg->nodes.data() + 1;
    RobOsageNode* v35_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (RobOsageNode* v35 = v35_begin; v35 != v35_end; v35++) {
        skin_param_osage_node* skp_osg_node = &v35->data_ptr->skp_osg_node;
        float_t fric = (1.0f - rob_osg->field_1EB4) * rob_osg->skin_param_ptr->friction;
        if (a5) {
            sub_140482490(v35, step, parent_scale->x);
            v35->field_C8 = (float_t)OsageCollision::osage_cls_work_list(v35->trans,
                skp_osg_node->coli_r, vec_ring_coli, &v35->friction);
            if (!rob_osg->disable_collision) {
                v35->field_C8 += (float_t)OsageCollision::osage_cls(coli_ring, v35->trans, skp_osg_node->coli_r);
                v35->field_C8 += (float_t)OsageCollision::osage_cls(coli, v35->trans, skp_osg_node->coli_r);
            }
            sub_140482180(v35, ring_y);
        }
        else
            sub_140482F30(&v35[0].trans, &v35[-1].trans, v35[0].length * parent_scale->x);

        vec3 v63;
        mat4_inverse_transform_point(&v64, &v35->trans, &v63);
        bool v40 = sub_140482FF0(v64, v63, &skp_osg_node->hinge,
            &v35->reset_data.rotation, rob_osg->yz_order);
        v35->bone_node_ptr->exp_data.parent_scale = *parent_scale;
        *v35->bone_node_ptr->ex_data_mat = v64;

        if (v35->bone_node_mat)
            mat4_scale_rot(&v64, parent_scale, v35->bone_node_mat);

        float_t v44 = vec3::distance_squared(v35[0].trans, v35[-1].trans);
        float_t v42 = v35->length * parent_scale->x;

        bool v45;
        if (v44 >= v42 * v42) {
            v42 = sqrtf(v44);
            v45 = false;
        }
        else
            v45 = true;

        mat4_mul_translate(&v64, v42, 0.0f, 0.0f, &v64);
        v35->reset_data.length = v42;
        if (v40 || v45)
            mat4_get_translation(&v64, &v35->trans);

        v35->trans_diff = (v35->trans - v35->trans_orig) * v9;

        if (v35->field_C8 > 0.0f)
            v35->trans_diff *= min_def(fric, v35->friction);

        float_t v55 = vec3::length_squared(v35->trans_diff);
        if (v55 > v23 * v23)
            v35->trans_diff *= v23 / sqrtf(v55);

        mat4_inverse_transform_point(root_matrix, &v35->trans, &v35->reset_data.trans);
        mat4_inverse_transform_vector(root_matrix, &v35->trans_diff, &v35->reset_data.trans_diff);
    }

    if (rob_osg->nodes.size() && rob_osg->node.bone_node_mat) {
        mat4 v65 = *rob_osg->nodes.back().bone_node_ptr->ex_data_mat;
        mat4_mul_translate(&v65, rob_osg->node.length * parent_scale->x, 0.0f, 0.0f, &v65);
        *rob_osg->node.bone_node_ptr->ex_data_mat = v65;
        mat4_scale_rot(&v65, parent_scale, &v65);
        *rob_osg->node.bone_node_mat = v65;
        rob_osg->node.bone_node_ptr->exp_data.parent_scale = *parent_scale;
    }
}

static void sub_14047C750(RobOsage* rob_osg, const mat4* root_matrix,
    const vec3* parent_scale, float_t step) {
    sub_14047C770(rob_osg, root_matrix, parent_scale, step, false);
}

static void sub_14047C770(RobOsage* rob_osg, const mat4* root_matrix,
    const vec3* parent_scale, float_t step, bool disable_external_force) {
    sub_14047C800(rob_osg, root_matrix, parent_scale, step, disable_external_force, false, false);
    sub_14047D8C0(rob_osg, root_matrix, parent_scale, step, true);
    sub_140480260(rob_osg, root_matrix, parent_scale, step, disable_external_force);
}

static void sub_14047D620(RobOsage* rob_osg, float_t step) {
    if (get_pause() || step <= 0.0f || rob_osg->disable_collision)
        return;

    std::vector<RobOsageNode>& nodes = rob_osg->nodes;
    vec3 v7 = nodes.data()[0].trans;
    OsageCollision::Work* coli = rob_osg->coli;
    OsageCollision::Work* coli_ring = rob_osg->coli_ring;
    SkinParam::RootCollisionType  coli_type = rob_osg->skin_param_ptr->coli_type;
    RobOsageNode* i_begin = nodes.data() + 1;
    RobOsageNode* i_end = nodes.data() + nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++) {
        RobOsageNodeData* data = i->data_ptr;
        for (RobOsageNode*& j : data->boc) {
            float_t v17 = (float_t)(
                OsageCollision::osage_capsule_cls(coli_ring, i->trans, j->trans, data->skp_osg_node.coli_r)
                + OsageCollision::osage_capsule_cls(coli, i->trans, j->trans, data->skp_osg_node.coli_r));
            i->field_C8 += v17;
            j->field_C8 += v17;
        }

        if (coli_type != SkinParam::RootCollisionTypeEnd
            && (coli_type != SkinParam::RootCollisionTypeBall || i != i_begin)) {
            float_t v20 = (float_t)(
                OsageCollision::osage_capsule_cls(coli_ring, i[0].trans, i[-1].trans, data->skp_osg_node.coli_r)
                + OsageCollision::osage_capsule_cls(coli, i[0].trans, i[-1].trans, data->skp_osg_node.coli_r));
            i[0].field_C8 += v20;
            i[-1].field_C8 += v20;
        }
    }
    nodes.data()[0].trans = v7;
}

static void sub_14047ECA0(RobOsage* rob_osg, float_t step) {
    if (get_pause() || step <= 0.0f)
        return;

    OsageCollision::Work* coli_ring = rob_osg->coli_ring;
    OsageCollision::Work* coli = rob_osg->coli;
    OsageCollision& vec_coli = rob_osg->ring.coli;

    RobOsageNode* i_begin = rob_osg->nodes.data() + 1;
    RobOsageNode* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    if (rob_osg->disable_collision)
        for (RobOsageNode* i = i_begin; i != i_end; i++) {
            RobOsageNodeData* data = i->data_ptr;
            i->field_C8 += (float_t)OsageCollision::osage_cls_work_list(i->trans,
                data->skp_osg_node.coli_r, vec_coli, &i->friction);
        }
    else
        for (RobOsageNode* i = i_begin; i != i_end; i++) {
            RobOsageNodeData* data = i->data_ptr;
            i->field_C8 += (float_t)OsageCollision::osage_cls_work_list(i->trans,
                data->skp_osg_node.coli_r, vec_coli, &i->friction);
            for (RobOsageNode*& j : data->boc) {
                j->field_C8 += (float_t)OsageCollision::osage_cls(coli_ring, j->trans, data->skp_osg_node.coli_r);
                j->field_C8 += (float_t)OsageCollision::osage_cls(coli, j->trans, data->skp_osg_node.coli_r);
            }

            i->field_C8 += (float_t)OsageCollision::osage_cls(coli_ring, i->trans, data->skp_osg_node.coli_r);
            i->field_C8 += (float_t)OsageCollision::osage_cls(coli, i->trans, data->skp_osg_node.coli_r);
        }
}

static void sub_14047F990(RobOsage* rob_osg, const mat4* root_matrix,
    const vec3* parent_scale, bool a4) {
    if (!rob_osg->nodes.size())
        return;

    vec3 v76 = rob_osg->exp_data.position * *parent_scale;
    mat4_transform_point(root_matrix, &v76, &v76);
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

    mat4 v78 = *root_matrix;
    sub_14047F110(rob_osg, &v78, parent_scale, true);
    vec3 v60 = { 1.0f, 0.0f, 0.0f };
    mat4_transform_vector(&v78, &v60, &v60);

    OsageCollision::Work* coli_ring = rob_osg->coli_ring;
    OsageCollision::Work* coli = rob_osg->coli;
    float_t v25 = ring_height + coli_r;
    float_t v16 = parent_scale->x;
    RobOsageNode* i = rob_osg->nodes.data();
    RobOsageNode* j_begin = rob_osg->nodes.data() + 1;
    RobOsageNode* j_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (RobOsageNode* j = j_begin; j != j_end; i++, j++) {
        vec3 v74 = i->trans + vec3::normalize(v60) * (v16 * j->length);
        if (a4 && j->sibling_node)
            sub_140482F30(&v74, &j->sibling_node->trans, j->max_distance);

        skin_param_osage_node* v38 = &j->data_ptr->skp_osg_node;
        OsageCollision::osage_cls(coli_ring, v74, v38->coli_r);
        OsageCollision::osage_cls(coli, v74, v38->coli_r);

        float_t v39 = v25 + v38->coli_r;
        if (v74.y < v39 && v39 < 1001.0f) {
            v74.y = v39;
            v74 = vec3::normalize(v74 - i->trans) * (v16 * j->length) + i->trans;
        }
        j->trans = v74;
        j->trans_diff = 0.0f;
        vec3 v77;
        mat4_inverse_transform_point(&v78, &j->trans, &v77);
        sub_140482FF0(v78, v77, &j->data_ptr->skp_osg_node.hinge,
            &j->reset_data.rotation, rob_osg->yz_order);
        j->bone_node_ptr->exp_data.parent_scale = *parent_scale;
        *j->bone_node_ptr->ex_data_mat = v78;

        if (j->bone_node_mat)
            mat4_scale_rot(&v78, parent_scale, j->bone_node_mat);

        float_t v55 = vec3::distance(j->trans, i->trans);
        float_t v56 = v16 * j->length;
        if (v55 >= fabsf(v56))
            v56 = v55;
        mat4_mul_translate(&v78, v56, 0.0f, 0.0f, &v78);
        mat4_get_translation(&v78, &j->trans);
        v60 = j->trans - i->trans;
    }

    if (rob_osg->nodes.size() && rob_osg->node.bone_node_mat) {
        mat4 v79 = *rob_osg->nodes.back().bone_node_ptr->ex_data_mat;
        mat4_mul_translate(&v79, v16 * rob_osg->node.length, 0.0f, 0.0f, &v79);
        *rob_osg->node.bone_node_ptr->ex_data_mat = v79;
        mat4_scale_rot(&v79, parent_scale, &v79);
        *rob_osg->node.bone_node_mat = v79;
        rob_osg->node.bone_node_ptr->exp_data.parent_scale = *parent_scale;
    }
}

static void sub_140480260(RobOsage* rob_osg, const mat4* root_matrix,
    const vec3* parent_scale, float_t step, bool disable_external_force) {
    if (!disable_external_force) {
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
    rob_osg->prev_osage_reset = false;

    for (RobOsageNode& i : rob_osg->nodes) {
        i.field_C8 = 0.0f;
        i.friction = 1.0f;
    }

    rob_osg->root_matrix = *rob_osg->root_matrix_ptr;
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
        mat4_mul_rotate_y(&mat, y_rot, &mat);
        mat4_mul_rotate_z(&mat, z_rot, &mat);
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
        mat4_mul_rotate_z(&mat, z_rot, &mat);
        mat4_mul_rotate_y(&mat, y_rot, &mat);
    }

    if (rot) {
        rot->y = y_rot;
        rot->z = z_rot;
    }
    return clipped;
}

static void sub_14053CE30(RobOsageNodeDataNormalRef* normal_ref, mat4* a2) {
    if (!normal_ref->set)
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
        mat4_mul(&v33, &v34, a2);
    }
}

static bool sub_14053D1B0(vec3* l_trans, vec3* r_trans,
    vec3* u_trans, vec3* d_trans, vec3* a5, vec3* a6, vec3* a7) {
    float_t length;
    *a5 = *d_trans - *u_trans;
    length = vec3::length_squared(*a5);
    if (length <= 0.000001f)
        return false;
    *a5 *= 1.0f / sqrtf(length);

    *a6 = vec3::cross(*r_trans - *l_trans, *a5);
    length = vec3::length_squared(*a6);
    if (length <= 0.000001f)
        return false;
    *a6 *= 1.0f / sqrtf(length);

    *a7 = vec3::normalize(vec3::cross(*a5, *a6));
    return true;
}
