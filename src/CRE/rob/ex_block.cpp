/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "rob.hpp"
#include "../../KKdLib/key_val.hpp"
#include "../../KKdLib/str_utils.hpp"
#include "../app_system_detail.hpp"
#include "../data.hpp"
#include "../gl_state.hpp"
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

static void apply_gravity(vec3& vec, const vec3& p0, const vec3& p1,
    const float_t osage_gravity_const, const float_t weight);
static float_t calculate_tbn(const vec3& pos_a, const vec3& pos_b,
    const float_t uv_a_y, const float_t uv_b_y,
    vec3& tangent, vec3& binormal, vec3& normal);
static float_t calculate_tbn(const vec3& pos_a, const vec3& pos_b, const vec3& pos_c,
    const vec2& texcoord_a, const vec2& texcoord_b, const vec2& texcoord_c,
    vec3& tangent, vec3& binormal, vec3& normal);
static void closest_pt_segment_segment(vec3& vec, const vec3& p0, const vec3& q0, const OsageCollision::Work* cls);
static bool rotate_matrix_to_direction(mat4& mat, const vec3& direction,
    const skin_param_hinge* hinge, vec3* rot, const int32_t& yz_order);
static void segment_limit_distance(vec3& p0, const vec3& p1, const float_t max_distance);

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

static const char* expression_component_names[] = {
    "posx",
    "posy",
    "posz",
    "rotx",
    "roty",
    "rotz",
    "scalex",
    "scaley",
    "scalez",
};

size_t rob_cloth_iterations_count = 0;
int32_t rob_cloth_update_vertices_flags = 0x03;
bool rob_cloth_update_normals_select = false;
static bool rob_osage_enable_sibling_node = true;

ExNodeBlock::ExNodeBlock() : bone_node_ptr(), type(), name(), parent_bone_node(),
parent_name(), parent_node(), item_equip_object(), is_parent(), done(), has_children_node() {

}

ExNodeBlock::~ExNodeBlock() {

}

void ExNodeBlock::CtrlBegin() {
    done = false;
}

void ExNodeBlock::Reset() {
    bone_node_ptr = 0;
}

void ExNodeBlock::CtrlEnd() {
    done = false;
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

void ExNullBlock::CtrlBegin() {
    done = false;
}

void ExNullBlock::CtrlStep(int32_t stage, bool disable_external_force) {

}

void ExNullBlock::CtrlMain() {
    if (!bone_node_ptr)
        return;

    mat4 mat;
    mat4 ex_data_mat = *parent_bone_node->ex_data_mat;
    bone_node_ptr->exp_data.mat_set(parent_bone_node->exp_data.parent_scale, ex_data_mat, mat);
    *bone_node_ptr->ex_data_mat = ex_data_mat;
    *bone_node_ptr->mat = mat;
}

void ExNullBlock::CtrlOsagePlayData() {
    CtrlMain();
}

void ExNullBlock::Disp(const mat4& mat, render_context* rctx) {

}

void ExNullBlock::Field_40() {

}

void ExNullBlock::CtrlInitBegin() {
    CtrlMain();
}

void ExNullBlock::CtrlInitMain() {

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

// 0x14053CE30
void RobOsageNodeDataNormalRef::GetMat(mat4* mat) {
    if (!set)
        return;

    vec3 n_trans;
    vec3 u_trans;
    vec3 d_trans;
    vec3 l_trans;
    vec3 r_trans;
    mat4_get_translation(n->bone_node_ptr->ex_data_mat, &n_trans);
    mat4_get_translation(u->bone_node_ptr->ex_data_mat, &u_trans);
    mat4_get_translation(d->bone_node_ptr->ex_data_mat, &d_trans);
    mat4_get_translation(l->bone_node_ptr->ex_data_mat, &l_trans);
    mat4_get_translation(r->bone_node_ptr->ex_data_mat, &r_trans);

    vec3 z_axis;
    vec3 y_axis;
    vec3 x_axis;
    if (RobOsageNodeDataNormalRef::GetAxes(
        l_trans, r_trans, u_trans, d_trans, z_axis, y_axis, x_axis)) {
        mat4 temp = mat4(
            x_axis.x, x_axis.y, x_axis.z, 0.0f,
            y_axis.x, y_axis.y, y_axis.z, 0.0f,
            z_axis.x, z_axis.y, z_axis.z, 0.0f,
            n_trans.x, n_trans.y, n_trans.z, 1.0f);
        mat4_mul(&this->mat, &temp, mat);
    }
}

// 0x14053CAC0
void RobOsageNodeDataNormalRef::Load() {
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

    vec3 z_axis;
    vec3 y_axis;
    vec3 x_axis;
    if (RobOsageNodeDataNormalRef::GetAxes(
        l_trans, r_trans, u_trans, d_trans, z_axis, y_axis, x_axis)) {
        mat4 temp = mat4(
            x_axis.x, y_axis.x, z_axis.x, 0.0f,
            x_axis.y, y_axis.y, z_axis.y, 0.0f,
            x_axis.z, y_axis.z, z_axis.z, 0.0f,
            -vec3::dot(n_trans, x_axis),
            -vec3::dot(n_trans, y_axis),
            -vec3::dot(n_trans, z_axis), 1.0f);
        mat4_mul(&n->mat, &temp, &mat);
    }
}

// 0x14053D1B0
bool RobOsageNodeDataNormalRef::GetAxes(const vec3& l_trans, const vec3& r_trans,
    const vec3& u_trans, const vec3& d_trans, vec3& z_axis, vec3& y_axis, vec3& x_axis) {
    z_axis = d_trans - u_trans;
    if (fabsf(vec3::length_squared(z_axis)) <= 0.000001f)
        return false;

    z_axis = vec3::normalize(z_axis);
    y_axis = vec3::cross(r_trans - l_trans, z_axis);
    if (fabsf(vec3::length_squared(y_axis)) <= 0.000001f)
        return false;

    y_axis = vec3::normalize(y_axis);
    x_axis = vec3::normalize(vec3::cross(z_axis, y_axis));
    return true;
}

inline bool skin_param_hinge::clamp(float_t& y, float_t& z) const {
    bool clamped = false;
    if (y > ymax) {
        y = ymax;
        clamped = true;
    }

    if (y < ymin) {
        y = ymin;
        clamped = true;
    }

    if (z > zmax) {
        z = zmax;
        clamped = true;
    }

    if (z < zmin) {
        z = zmin;
        clamped = true;
    }
    return clamped;
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

// 0x140482DF0
void opd_node_data::lerp(opd_node_data& dst, const opd_node_data& src0, const opd_node_data& src1, float_t blend) {
    dst.length = lerp_def(src0.length, src1.length, blend);
    dst.rotation = vec3::lerp(src0.rotation, src1.rotation, blend);
}

opd_node_data_pair::opd_node_data_pair() {

}

// 0x140482100
void opd_node_data_pair::set_data(opd_blend_data* blend_data, const opd_node_data& node_data) {
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

RobOsageNode::RobOsageNode() : length(), child_length(), bone_node_ptr(),
bone_node_mat(), sibling_node(), max_distance(), hit() {
    friction = 1.0f;
    force = 1.0f;
    data_ptr = &data;
    opd_data.resize(3);
}

RobOsageNode::~RobOsageNode() {

}

// 0x140482180
void RobOsageNode::CheckFloorCollision(const float_t& floor_height) {
    float_t pos_y = data_ptr->skp_osg_node.coli_r + floor_height;
    if (pos_y <= pos.y)
        return;

    pos.y = pos_y;
    pos = GetPrevNode().pos + vec3::normalize(pos - GetPrevNode().pos) * length;
    delta_pos = 0.0f;
    hit += 1.0f;
}

// 0x140482490
void RobOsageNode::CheckNodeDistance(const float_t& step, const float_t& parent_scale) {
    if (step != 1.0f) {
        vec3 d = pos - fixed_pos;

        float_t dist = vec3::length(d);
        if (dist != 0.0f)
            d *= 1.0f / dist;
        pos = fixed_pos + d * (step * dist);
    }

    segment_limit_distance(pos, GetPrevNode().pos, length * parent_scale);

    if (rob_osage_enable_sibling_node) {
        if (sibling_node)
            segment_limit_distance(pos, sibling_node->pos, max_distance);
    }
}

void RobOsageNode::Reset() {
    length = 0.0f;
    pos = 0.0f;
    fixed_pos = 0.0f;
    delta_pos = 0.0f;
    vel = 0.0f;
    child_length = 0.0f;
    bone_node_ptr = 0;
    bone_node_mat = 0;
    sibling_node = 0;
    max_distance = 0.0f;
    rel_pos = 0.0f;
    reset_data.pos = 0.0f;
    reset_data.delta_pos = 0.0f;
    reset_data.rotation = 0.0f;
    reset_data.length = 0.0f;
    hit = 0.0f;
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
    const float_t length = vec3::distance_squared(p, center);
    if (length > 0.000001f && length < r * r) {
        vec = (p - center) * (r / sqrtf(length) - 1.0f);
        return 1;
    }
    return 0;
}

// 0x140484540
int32_t OsageCollision::cls_capsule_oidashi(vec3& vec, const vec3& p, const OsageCollision::Work* cls, const float_t r) {
    const vec3 v11 = p - cls->pos[0];

    const float_t v17 = vec3::dot(v11, cls->vec_center);
    if (v17 < 0.0f)
        return OsageCollision::cls_ball_oidashi(vec, p, cls->pos[0], r);

    const float_t v19 = cls->vec_center_length_squared;
    if (fabsf(v19) <= 0.000001f)
        return OsageCollision::cls_ball_oidashi(vec, p, cls->pos[0], r);
    else if (v17 > v19)
        return OsageCollision::cls_ball_oidashi(vec, p, cls->pos[1], r);

    const float_t v20 = vec3::length_squared(v11);
    const float_t v21 = v17 / v19;
    const float_t v22 = fabsf(v20 - v21 * v17);
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
    const float_t d = vec3::dot(p, p2) - vec3::dot(p1, p2) - r;
    if (d < 0.0f) {
        vec = p2 * -d;
        return 1;
    }
    return 0;
}

// 0x140484E10
void OsageCollision::get_nearest_line2point(vec3& nearest, const vec3& p0, const vec3& p1, const vec3& q) {
    const vec3 p0p1 = p1 - p0;
    const vec3 p0q = q - p0;

    const float_t t = vec3::dot(p0q, p0p1);
    if (t < 0.0f) {
        nearest = p0;
        return;
    }

    const float_t p0p1_len = vec3::length_squared(p0p1);
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

    int32_t hit = 0;
    while (cls->type != SkinParam::CollisionTypeEnd) {
        vec3 vec = 0.0f;
        switch (cls->type) {
        case SkinParam::CollisionTypeBall:
            hit += OsageCollision::cls_line2ball_oidashi(vec, p0, p1, cls->pos[0], cls_r + cls->radius);
            break;
        case SkinParam::CollisionTypeCapsule:
            hit += OsageCollision::cls_line2capsule_oidashi(vec, p0, p1, cls, cls_r + cls->radius);
            break;
        case SkinParam::CollisionTypeEllipse:
            hit += OsageCollision::cls_line2ellipse_oidashi(vec, p0, p1, cls, cls_r + cls->radius);
            break;
        }

        if (hit > 0) {
            p0 += vec;
            p1 += vec;
        }
        cls++;
    }
    return hit;
}

// 0x140485180
int32_t OsageCollision::osage_cls(const OsageCollision::Work* cls, vec3& p, const float_t& cls_r) {
    return OsageCollision::osage_cls(p, cls_r, cls, 0);
}

// 0x140485220
int32_t OsageCollision::osage_cls(vec3& p, const float_t& cls_r, const OsageCollision::Work* cls, float_t* fric) {
    if (!cls || cls->type == SkinParam::CollisionTypeEnd)
        return 0;

    int32_t hit = 0;
    while (cls->type != SkinParam::CollisionTypeEnd) {
        int32_t _hit = 0;
        vec3 vec = 0.0f;
        switch (cls->type) {
        case SkinParam::CollisionTypeBall:
            _hit = OsageCollision::cls_ball_oidashi(vec, p, cls->pos[0], cls->radius + cls_r);
            break;
        case SkinParam::CollisionTypeCapsule:
            _hit = OsageCollision::cls_capsule_oidashi(vec, p, cls, cls->radius + cls_r);
            break;
        case SkinParam::CollisionTypePlane:
            _hit = OsageCollision::cls_plane_oidashi(vec, p, cls->pos[0], cls->pos[1], cls_r);
            break;
        case SkinParam::CollisionTypeEllipse:
            _hit = OsageCollision::cls_ellipse_oidashi(vec, p, cls, cls->radius + cls_r);
            break;
        case SkinParam::CollisionTypeAABB:
            _hit = OsageCollision::cls_aabb_oidashi(vec, p, cls, cls_r);
            break;
        }

        if (fric && _hit > 0)
            *fric = max_def(*fric, cls->friction);

        hit += _hit;
        p += vec;
        cls++;
    }
    return hit;
}

// 0x1404851C0
int32_t OsageCollision::osage_cls_work_list(vec3& p, const float_t& cls_r, const OsageCollision& coli, float_t* fric) {
    if (coli.work_list.size() && coli.work_list.back().type == SkinParam::CollisionTypeEnd)
        return OsageCollision::osage_cls(p, cls_r, coli.work_list.data(), fric);
    return 0;
}

osage_ring_data::osage_ring_data() : rect_x(), rect_y(),
rect_width(), rect_height(), ring_height(), out_height(), init() {
    reset();
}

osage_ring_data::~osage_ring_data() {

}

void osage_ring_data::reset() {
    rect_x = 0.0f;
    rect_y = 0.0f;
    rect_width = 0.0f;
    rect_height = 0.0f;
    ring_height = -1000.0f;
    out_height = -1000.0f;
    init = false;
    coli_object.work_list.clear();
    skp_root_coli.clear();
}

inline float_t osage_ring_data::get_floor_height(const vec3& pos, const float_t coli_r) const {
    const float_t x = rect_x - coli_r;
    const float_t y = rect_y - coli_r;
    if (pos.x < x || pos.z < y
        || pos.x > x + rect_width + coli_r
        || pos.z > y + rect_height + coli_r)
        return out_height + coli_r;
    else
        return ring_height + coli_r;
}

void osage_ring_data::parse(const std::string& path, osage_ring_data& ring) {
    ring.reset();

    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;

    key_val kv;
    if (!aft_data->load_file(&kv, path.c_str(), key_val::load_file))
        return;

    int32_t count;
    if (kv.read("object", "length", count)) {
        for (int32_t i = 0; i < count; i++) {
            if (!kv.open_scope_fmt(i))
                continue;

            int32_t type;
            if (!kv.read("type", type))
                break;

            OsageCollision::Work cls;
            cls.type = (SkinParam::CollisionType)type;

            float_t radius;
            if (!kv.read("radius", radius))
                break;

            cls.radius = radius;

            if (kv.open_scope("pos")) {
                for (int32_t j = 0; j < 2; j++) {
                    if (!kv.open_scope_fmt(j))
                        continue;

                    float_t x;
                    if (kv.read("x", x))
                        cls.pos[j].x = x;

                    float_t y;
                    if (kv.read("y", y))
                        cls.pos[j].y = y;

                    float_t z;
                    if (kv.read("z", z))
                        cls.pos[j].z = z;

                    kv.close_scope();
                }
                kv.close_scope();
            }

            float_t friction;
            if (kv.read("friction", friction))
                cls.friction = friction;

            int32_t node_idx0 = -1;
            int32_t node_idx1 = -1;

            const char* bone0_name;
            const char* bone1_name;
            if (kv.read("bone.0.name", bone0_name)) {
                node_idx0 = aft_bone_data->get_skeleton_object_bone_index(
                    bone_database_skeleton_type_to_string(BONE_DATABASE_SKELETON_COMMON), bone0_name);
                if (node_idx0 >= 0 && kv.read("bone.1.name", bone1_name))
                    node_idx1 = aft_bone_data->get_skeleton_object_bone_index(
                        bone_database_skeleton_type_to_string(BONE_DATABASE_SKELETON_COMMON), bone1_name);
            }

            if (node_idx0 >= 0) {
                SkinParam::CollisionParam cls_param;
                cls_param.type = cls.type;
                cls_param.node_idx[0] = node_idx0;
                cls_param.node_idx[1] = node_idx1;
                cls_param.radius = cls.radius;
                cls_param.pos[0] = cls.pos[0];
                cls_param.pos[1] = cls.pos[1];
                ring.skp_root_coli.push_back(cls_param);
            }
            else {
                if (cls.type == SkinParam::CollisionTypeCapsule || cls.type == SkinParam::CollisionTypeEllipse) {
                    cls.vec_center = cls.pos[1] - cls.pos[0];
                    cls.vec_center_length_squared = vec3::length_squared(cls.vec_center);
                    cls.vec_center_length = sqrtf(cls.vec_center_length_squared);
                }
                ring.coli_object.work_list.push_back(cls);
            }

            kv.close_scope();
        }
        kv.close_scope();
    }

    ring.coli_object.work_list.push_back({});
    ring.skp_root_coli.push_back({});

    kv.open_scope("ring");

    float_t rect_x;
    float_t rect_y;
    float_t rect_width;
    float_t rect_height;
    float_t ring_height;
    float_t out_height;
    if (kv.read("rect.x", rect_x) && kv.read("rect.y", rect_y)
        && kv.read("rect.width", rect_width) && kv.read("rect.height", rect_height)
        && kv.read("ring_height", ring_height) && kv.read("out_height", out_height)) {
        ring.init = true;
        ring.rect_x = rect_x;
        ring.rect_y = rect_y;
        ring.rect_width = rect_width;
        ring.rect_height = rect_height;
        ring.ring_height = ring_height;
        ring.out_height = out_height;
    }
    kv.close_scope();
}

osage_ring_data& osage_ring_data::operator=(const osage_ring_data& ring) {
    rect_x = ring.rect_x;
    rect_y = ring.rect_y;
    rect_width = ring.rect_width;
    rect_height = ring.rect_height;
    ring_height = ring.ring_height;
    out_height = ring.out_height;
    init = ring.init;
    coli_object.work_list.assign(ring.coli_object.work_list.begin(), ring.coli_object.work_list.end());
    skp_root_coli.assign(ring.skp_root_coli.begin(), ring.skp_root_coli.end());
    return *this;
}

CLOTHNode::CLOTHNode() : flags(), tangent_sign(), dist_top(),
dist_bottom(), dist_right(), dist_left(), reset_data() {

}

CLOTHNode::~CLOTHNode() {

}

// 0x14021A890
void CLOTHNode::CalculateTBN(const CLOTHNode* right, const CLOTHNode* left,
    const CLOTHNode* top, const CLOTHNode* bottom) {
    const vec3 pos_a = top->pos - bottom->pos;
    const vec3 pos_b = left->pos - right->pos;

    const float_t pos_b_length = vec3::length(pos_a);
    const float_t pos_a_length = vec3::length(pos_b);
    if (pos_b_length <= 0.000001f || pos_a_length <= 0.000001f)
        return;

    const vec2 uv_a = top->texcoord - bottom->texcoord;
    const vec2 uv_b = left->texcoord - right->texcoord;

    float_t r = uv_b.y * uv_a.x - uv_a.y * uv_b.x;
    if (fabsf(r) > 0.000001f) {
        r = 1.0f / r;
        tangent_sign = calculate_tbn(pos_a, pos_b,
            uv_a.y * r, uv_b.y * r, tangent, binormal, normal);
    }
}

CLOTH::CLOTH() : flags(), root_count(), nodes_count(), wind_direction(),
inertia(), set_external_force(), external_force(), skin_param_ptr(), mats() {
    Reset();
}

CLOTH::~CLOTH() {

}

void CLOTH::Init() {
    lines.clear();
    if (nodes_count <= 1)
        return;

    CLOTHLine line = {};
    const size_t root_count = this->root_count;
    for (size_t i = 1; i < nodes_count; i++) {
        for (size_t j = 0; j < root_count; j++) {
            size_t top = (i - 1) * root_count + j;
            size_t center = i * root_count + j;
            line.idx[0] = top;
            line.idx[1] = center;
            line.length = vec3::distance(nodes.data()[top].pos, nodes.data()[center].pos);
            lines.push_back(line);

            if (j < root_count - 1) {
                size_t left = i * root_count + j + 1;
                line.idx[0] = center;
                line.idx[1] = left;
                line.length = vec3::distance(nodes.data()[center].pos, nodes.data()[left].pos);
                lines.push_back(line);
            }
        }

        if (flags & 0x04) {
            line.idx[0] = i * root_count;
            line.idx[1] = (i + 1) * root_count - 1;
            lines.push_back(line);
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

void CLOTH::SetWindDirection(const vec3& value) {
    this->wind_direction = value;
}

void CLOTH::SetInertia(float_t value) {
    inertia = value;
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
    flags &= ~0x01;
    root_count = 0;
    nodes_count = 0;
    nodes.clear();
    wind_direction = 0.0f;
    inertia = 0.0f;
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

// 0x1402187D0
void RobCloth::ApplyPhysics(bool ignore_friction) {
    float_t fric = (1.0f - inertia) * (1.0f - skin_param_ptr->air_res);

    float_t osage_gravity = get_osage_gravity_const();

    vec3 external_force = wind_direction * skin_param_ptr->wind_afc;
    if (set_external_force) {
        external_force += external_force;
        osage_gravity = 0.0f;
    }

    const size_t root_count = this->root_count;
    const size_t nodes_count = this->nodes_count;

    float_t force = skin_param_ptr->force;
    CLOTHNode* node = &nodes.data()[root_count];
    for (size_t i = 1; i < nodes_count; i++) {
        RobClothRoot* root = this->root.data();
        for (size_t j = 0; j < root_count; j++, root++, node++) {
            mat4 mat = root->mat;

            vec3 direction;
            mat4_transform_vector(&mat, &node->direction, &direction);

            const float_t _fric = ignore_friction ? (node->delta_pos.y >= 0.0f ? 1.0f : 0.0f) : fric;
            vec3 vel = direction * force - node->delta_pos * _fric + external_force;
            vel.y -= osage_gravity;
            node->delta_pos += vel;

            node->prev_pos = node->pos;
            node->pos += node->delta_pos;
        }
        force *= skin_param_ptr->force_gain;
    }
}

// 0x1402196D0
void RobCloth::ApplyResetData() {
    const size_t root_count = this->root_count;
    const size_t nodes_count = this->nodes_count;

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
            mat4_transform_point(&root->mat_pos, &node->reset_data.pos, &node->pos);
            mat4_transform_vector(&root->mat_pos, &node->reset_data.delta_pos, &node->delta_pos);
        }
    }
}

void RobCloth::ColiSet(const mat4* transform) {
    if (skin_param_ptr->coli.size())
        OsageCollision::Work::update_cls_work(coli_chara, skin_param_ptr->coli.data(), transform);
    OsageCollision::Work::update_cls_work(coli_ring, ring.skp_root_coli, transform);
}

// 0x14021AA60
void RobCloth::CollideNodes(const float_t step, bool a3) {
    const ssize_t root_count = this->root_count;
    const size_t nodes_count = this->nodes_count;

    const float_t inv_step = 1.0f / step;

    const RobClothRoot* root = this->root.data();
    CLOTHNode* node = &nodes.data()[root_count];
    const float_t floor_height = ring.get_floor_height(node->pos, skin_param_ptr->coli_r);

    for (size_t i = 1; i < nodes_count; i++) {
        for (ssize_t j = 0; j < root_count; j++, node++) {
            float_t fric = (1.0f - inertia) * skin_param_ptr->friction;
            if (step != 1.0f) {
                vec3 delta_pos = node->pos - node->prev_pos;

                float_t trans_length = vec3::length(delta_pos);
                if (trans_length * step > 0.0f && trans_length != 0.0f)
                    delta_pos *= 1.0f / trans_length;

                node->pos = node->prev_pos + delta_pos * (trans_length * step);
            }
            segment_limit_distance(node[0].pos, node[-root_count].pos, node[0].dist_top);

            int32_t hit = OsageCollision::osage_cls_work_list(node->pos,
                skin_param_ptr->coli_r, ring.coli_object, &fric);
            hit += OsageCollision::osage_cls(coli_ring, node->pos, skin_param_ptr->coli_r);
            hit += OsageCollision::osage_cls(coli_chara, node->pos, skin_param_ptr->coli_r);

            if (floor_height > node->pos.y && floor_height < 1001.0f) {
                node->pos.y = floor_height;
                node->delta_pos = 0.0f;
            }

            mat4 mat = root->mat;
            mat4_set_translation(&mat, &node[-root_count].pos);

            int32_t yz_order = 1;
            rotate_matrix_to_direction(mat, node->direction, 0, 0, yz_order);

            vec3 direction;
            mat4_inverse_transform_point(&mat, &node->pos, &direction);

            rotate_matrix_to_direction(mat, direction, &skin_param_ptr->hinge,
                &node->reset_data.rotation, yz_order);
            mat4_mul_translate(&mat, node->dist_top, 0.0f, 0.0f, &mat);
            mat4_get_translation(&mat, &node->pos);

            if (hit)
                node->delta_pos *= fric;

            node->delta_pos = (node->pos - node->prev_pos) * inv_step;

            if (!a3) {
                const mat4& inv_mat_pos = this->root.data()[j].inv_mat_pos;
                mat4_transform_point(&inv_mat_pos, &node->pos, &node->reset_data.pos);
                mat4_transform_vector(&inv_mat_pos, &node->delta_pos, &node->reset_data.delta_pos);
            }
        }
    }
}

// 0x14021D480
void RobCloth::CtrlInitBegin() {
    GetRootData();

    const float_t osage_gravity_const = get_osage_gravity_const();

    const ssize_t root_count = this->root_count;
    const size_t nodes_count = this->nodes_count;

    CLOTHNode* node = &nodes.data()[root_count];
    const float_t floor_height = ring.get_floor_height(node->pos, skin_param_ptr->coli_r);

    for (size_t i = 1; i < nodes_count; i++) {
        RobClothRoot* root = this->root.data();
        for (ssize_t j = 0; j < root_count; j++, root++, node++) {
            mat4 mat = root->mat;

            vec3 v38;
            mat4_transform_vector(&mat, &node->direction, &v38);
            v38.y -= osage_gravity_const;

            node[0].pos = node[-root_count].pos + vec3::normalize(v38) * node->dist_top;

            OsageCollision::osage_cls_work_list(node->pos, skin_param_ptr->coli_r, ring.coli_object);
            OsageCollision::osage_cls(coli_ring, node->pos, skin_param_ptr->coli_r);
            OsageCollision::osage_cls(coli_chara, node->pos, skin_param_ptr->coli_r);

            if (floor_height > node->pos.y && floor_height < 1001.0f)
                node->pos.y = floor_height;

            node->delta_pos = 0.0f;
            node->prev_pos = node->pos;
        }
    }

    for (size_t i = 0; i < rob_cloth_iterations_count; ++i)
        CtrlStep(1.0f);

    node = &nodes.data()[root_count];
    for (size_t i = 1; i < nodes_count; i++)
        for (ssize_t j = 0; j < root_count; j++, node++)
            node->delta_pos = 0.0f;
}

// 0x14021D840
void RobCloth::CtrlInitMain() {
    CtrlMain(1.0f, true);
}

// 0x140218560
void RobCloth::CtrlMain(const float_t step, bool ignore_friction) {
    GetRootData();

    if (osage_reset) {
        ApplyResetData();
        osage_reset = false;
    }

    if (move_cancel > 0.0f) {
        const size_t root_count = this->root_count;
        const size_t nodes_count = this->nodes_count;

        float_t move_cancel = this->move_cancel;
        for (size_t i = 0; i < root_count; i++) {
            const mat4& mat_pos = root.data()[i].mat_pos;
            CLOTHNode* node = &nodes.data()[i + root_count];
            for (size_t j = 1; j < nodes_count; j++, node += root_count) {
                vec3 pos;
                mat4_transform_point(&mat_pos, &node->reset_data.pos, &pos);
                node->pos += (pos - node->pos) * move_cancel;
            }
        }
    }

    if (step > 0.0f && !get_pause()) {
        ApplyPhysics(ignore_friction);
        NodesLimitDistance();
        CollideNodes(step, false);

        if (!ignore_friction) {
            set_external_force = false;
            external_force = 0.0f;
        }
    }
}

// 0x140218E40
void RobCloth::CtrlOsagePlayData(std::vector<opd_blend_data>& opd_blend_data) {
    GetRootData();

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

            vec3 parent_trans = root_node.pos;
            mat4 mat = root.data()[j].mat;
            mat4_mul_translate(&mat, &root_node.fixed_pos, &mat);

            CLOTHNode* v29 = &nodes.data()[j + root_count];

            vec3 direction;
            mat4_transform_vector(&mat, &v29->direction, &direction);

            int32_t yz_order = 0;
            rotate_matrix_to_direction(mat, direction, 0, 0, yz_order);

            const mat4& mat_pos = root.data()[j].mat_pos;
            for (size_t k = 1; k < nodes_count; k++, v29 += root_count) {
                opd_vec3_data* opd = &v29->opd_data[i - i_end];
                const float_t* opd_x = opd->x;
                const float_t* opd_y = opd->y;
                const float_t* opd_z = opd->z;

                vec3 curr_trans;
                curr_trans.x = opd_x[curr_key];
                curr_trans.y = opd_y[curr_key];
                curr_trans.z = opd_z[curr_key];

                vec3 next_trans;
                next_trans.x = opd_x[next_key];
                next_trans.y = opd_y[next_key];
                next_trans.z = opd_z[next_key];

                mat4_transform_point(&mat_pos, &curr_trans, &curr_trans);
                mat4_transform_point(&mat_pos, &next_trans, &next_trans);

                vec3 _trans = curr_trans * inv_blend + next_trans * blend;

                vec3 direction;
                mat4_inverse_transform_point(&mat, &_trans, &direction);

                vec3 rotation = 0.0f;
                int32_t yz_order = 0;
                rotate_matrix_to_direction(mat, direction, 0, &rotation, yz_order);

                mat4_mul_translate(&mat, vec3::distance(_trans, parent_trans), 0.0f, 0.0f, &mat);

                v29->opd_node_data.set_data(i, { v29->dist_top, rotation });

                parent_trans = _trans;
            }
        }
    }

    for (size_t i = 0; i < root_count; i++) {
        CLOTHNode& root_node = nodes.data()[i];
        mat4 mat = root.data()[i].mat;
        mat4_mul_translate(&mat, &root_node.fixed_pos, &mat);

        CLOTHNode* v50 = &nodes.data()[i + root_count];

        vec3 direction;
        mat4_transform_vector(&mat, &v50->direction, &direction);

        int32_t yz_order = 0;
        rotate_matrix_to_direction(mat, direction, 0, 0, yz_order);

        for (size_t j = 1; j < nodes_count; j++, v50 += root_count) {
            mat4_mul_rotate_z(&mat, v50->opd_node_data.curr.rotation.z, &mat);
            mat4_mul_rotate_y(&mat, v50->opd_node_data.curr.rotation.y, &mat);
            mat4_mul_translate(&mat, v50->opd_node_data.curr.length, 0.0f, 0.0f, &mat);
            mat4_get_translation(&mat, &v50->pos);
        }
    }
}

// 0x14021DC60
void RobCloth::CtrlStep(const float_t step) {
    if (step <= 0.0f)
        return;

    GetRootData();
    ApplyPhysics(true);
    NodesLimitDistance();
    CollideNodes(step, true);
}

void RobCloth::Disp(const mat4& mat, render_context* rctx) {
    obj* obj = objset_info_storage_get_obj(itm_eq_obj->obj_info);
    if (!obj)
        return;

    std::vector<GLuint>* textures = objset_info_storage_get_obj_set_gentex(itm_eq_obj->obj_info.set_id);

    vec3 center = (nodes.data()[0].pos + nodes.data()[root_count * nodes_count - 1].pos) * 0.5f;

    ::obj o = *obj;
    o.num_mesh = index_buffer[1].buffer != 0 ? 2 : 1;
    o.mesh_array = mesh;
    o.bounding_sphere.center = center;
    o.bounding_sphere.radius *= 2.0f;

    for (int32_t i = 0; i < o.num_mesh; i++) {
        obj_mesh& mesh = this->mesh[i];
        mesh.bounding_sphere.center = center;
        for (int32_t j = 0; j < mesh.num_submesh; j++) {
            obj_sub_mesh& submesh = mesh.submesh_array[j];
            submesh.bounding_sphere.center = center;
        }
    }

    rctx->disp_manager->set_texture_pattern((int32_t)itm_eq_obj->texture_pattern.size(), itm_eq_obj->texture_pattern.data());
    rctx->disp_manager->entry_obj_by_obj(mat, &o, textures, vertex_buffer, index_buffer, 0, itm_eq_obj->alpha);
    rctx->disp_manager->set_texture_pattern();
}

// 0x140219940
void RobCloth::GetRootData() {
    for (size_t i = 0; i < root_count; i++) {
        RobClothRoot& root = this->root.data()[i];
        CLOTHNode& root_node = nodes.data()[i];

        mat4 m = mat4_null;
        for (int32_t j = 0; j < 4; j++) {
            if (!root.bone_mat[j] || !root.node_mat[j])
                continue;

            mat4 mat;
            mat4_mul(root.bone_mat[j], root.node_mat[j], &mat);

            float_t weight = root.weight[j];
            mat4_mul_scale(&mat, weight, weight, weight, weight, &mat);
            mat4_add(&m, &mat, &m);
        }
        root.mat = m;

        mat4_transform_point(&m, &root_node.fixed_pos, &root_node.pos);
        mat4_transform_vector(&m, &root.normal, &root_node.normal);
        mat4_transform_vector(&m, (vec3*)&root.tangent, &root_node.tangent);
        root_node.tangent_sign = root.tangent.w;
        root_node.prev_pos = root_node.pos;

        mat4_mul_translate(&m, &root_node.fixed_pos, &m);
        root.mat_pos = m;
        mat4_invert(&m, &m);
        root.inv_mat_pos = m;
    }
}

void RobCloth::InitData(size_t root_count, size_t nodes_count, obj_skin_block_cloth_root* root,
    obj_skin_block_cloth_node* nodes, mat4* mats, int32_t loop,
    rob_chara_item_equip_object* itm_eq_obj, const bone_database* bone_data) {
    this->itm_eq_obj = itm_eq_obj;
    obj* obj = objset_info_storage_get_obj(itm_eq_obj->obj_info);
    if (!obj)
        return;

    obj_mesh_index_buffer* index_buffer = objset_info_storage_get_obj_mesh_index_buffer(itm_eq_obj->obj_info);
    uint32_t mesh_name_index = obj->get_obj_mesh_index(cls_data->mesh_name);
    uint32_t backface_mesh_name_index = obj->get_obj_mesh_index(cls_data->backface_mesh_name);

    mesh[0] = obj->mesh_array[mesh_name_index];
    if (cls_data->backface_mesh_name && backface_mesh_name_index != -1)
        mesh[1] = obj->mesh_array[backface_mesh_name_index];
    else
        mesh[1] = {};

    for (int32_t i = 0; i < 2; i++) {
        RobClothSubMeshArray& submesh = this->submesh[i];
        for (int32_t j = 0; j < mesh[i].num_submesh; j++) {
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
    if (!vertex_buffer[0].load(mesh[0], GL::BUFFER_USAGE_STREAM))
        return;

    this->index_buffer[1] = {};
    if (cls_data->backface_mesh_name && backface_mesh_name_index != -1)
        this->index_buffer[1] = index_buffer[backface_mesh_name_index];

    if (cls_data->backface_mesh_name && !vertex_buffer[1].load(mesh[1], GL::BUFFER_USAGE_STREAM))
        return;

    flags = (((flags ^ (0x04 * loop)) & 0x04) ^ flags) | 0x03;

    this->root_count = root_count;
    this->nodes_count = nodes_count;
    this->cls_root = root;
    this->mats = mats;

    this->root.clear();
    this->root.reserve(root_count);

    for (size_t i = 0; i < root_count; i++) {
        this->root.push_back({});
        RobClothRoot& cls_root = this->root.back();
        cls_root.pos = root[i].pos;
        cls_root.normal = root[i].normal;
        for (int32_t j = 0; j < 4; j++) {
            obj_skin_block_cloth_root_bone_weight& bone_weight = root[i].bone_weights[j];
            cls_root.node[j] = 0;
            cls_root.node_mat[j] = 0;
            cls_root.bone_mat[j] = &mats[bone_weight.matrix_index];
            if (bone_weight.bone_name) {
                cls_root.node[j] = itm_eq_obj->get_bone_node(bone_weight.bone_name, bone_data);
                if (cls_root.node[j])
                    cls_root.node_mat[j] = cls_root.node[j]->mat;
            }
            cls_root.weight[j] = bone_weight.weight;
        }
    }

    this->nodes.clear();
    this->nodes.reserve(root_count);

    for (size_t i = 0; i < root_count; i++) {
        this->nodes.push_back({});
        CLOTHNode& v55 = this->nodes.back();
        v55.pos = root[i].pos;
        v55.fixed_pos = root[i].pos;
    }

    std::vector<size_t> index_array(root_count * nodes_count, 0);
    uint16_t* mesh_index_array = cls_data->mesh_index_array;
    for (size_t i = 0; i < cls_data->num_mesh_index; i++)
        index_array.data()[mesh_index_array[i]] = i;

    obj_mesh* mesh = objset_info_storage_get_obj_mesh(itm_eq_obj->obj_info, cls_data->mesh_name);
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
            v72.pos = node.pos;
            v72.fixed_pos = node.pos;
            v72.normal = { 0.0f, 0.0f, 1.0f };
            v72.tangent = { 1.0f, 0.0f, 0.0f };
            v72.delta_pos = node.delta_pos;
            v72.direction = vec3::normalize(node.delta_pos);
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
        cls_data->node_array, cls_data->mat_array, cls_data->loop, itm_eq_obj, bone_data);

    obj* obj = objset_info_storage_get_obj(itm_eq_obj->obj_info);
    if (!obj)
        return;

    for (int32_t i = 0; i < 2; i++) {
        if (!mesh[i].num_vertex || !mesh[i].vertex_array || !index_buffer[i].buffer)
            continue;

        obj_mesh_vertex_buffer* obj_vert_buf = &vertex_buffer[i];
        obj_mesh_index_buffer* obj_index_buf = &index_buffer[i];

        obj_mesh* mesh = &this->mesh[i];
        for (int32_t j = 0; j < mesh->num_submesh; j++) {
            obj_material_data* material = &obj->material_array[mesh->submesh_array[j].material_index];
            for (int32_t k = 0; k < (mesh->attrib.m.double_buffer ? 2 : 1); k++) {
                extern render_context* rctx_ptr;
                rctx_ptr->disp_manager->add_vertex_array(mesh, &mesh->submesh_array[j], material,
                    obj_vert_buf->get_buffer(), obj_vert_buf->get_offset(), obj_index_buf->buffer, 0, 0);

                obj_vert_buf->cycle_index();
            }
        }
    }
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

// 0x140219D10
void RobCloth::NodesLimitDistance() {
    CLOTHNode* node = nodes.data();
    const ssize_t root_count = this->root_count;
    const size_t nodes_count = this->nodes_count;

    for (size_t i = nodes_count - 2; i; i--) {
        CLOTHNode* v7 = &node[root_count * i];
        for (ssize_t j = 0; j < root_count; j++, v7++)
            segment_limit_distance(v7->pos, v7[root_count].pos, v7->dist_bottom);
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
                *v17 = v18->pos;
                *v17a = v18->pos;
            }
        }

        if (root_count - 2 >= 0) {
            vec3* v20 = v13;
            CLOTHNode* v22 = v16 - 1;
            for (ssize_t j = root_count - 1; j > 0; j--, v20--, v22--)
                segment_limit_distance(v20[0], v20[1], v22->dist_left);
            v14 = v12 + 1;
        }

        if (flags & 0x04)
            segment_limit_distance(v11[root_count - 1], v11[0], v16->dist_right);

        if (root_count > 1) {
            vec3* v23 = v14;
            CLOTHNode* v24 = v15 + 1;
            for (ssize_t j = root_count - 1; j > 0; j--, v23++, v24++)
                segment_limit_distance(v23[0], v23[-1], v24->dist_right);
            v14 = v12 + 1;
        }

        if (flags & 0x04)
            segment_limit_distance(v12[0], v12[root_count - 1], v15->dist_left);

        if (root_count > 0) {
            vec3* v27 = v11;
            vec3* v28 = v12;
            CLOTHNode* v36 = v15;
            for (ssize_t j = root_count; j > 0; j--, v27++, v28++, v36++)
                v36->pos = (*v27 + *v28) * 0.5f;
        }
        v16 += root_count;
        v15 += root_count;
    }

    operator delete(v11);
    operator delete(v12);
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

// 0x14021EF50
void RobCloth::SetMoveCancel(const float_t& value) {
    move_cancel = value;
}

const float_t* RobCloth::SetOsagePlayDataInit(const float_t* opdi_data) {
    CLOTHNode* i_begin = nodes.data() + root_count;
    CLOTHNode* i_end = nodes.data() + nodes.size();
    for (CLOTHNode* i = i_begin; i != i_end; i++) {
        i->pos.x = *opdi_data++;
        i->pos.y = *opdi_data++;
        i->pos.z = *opdi_data++;
        i->delta_pos.x = *opdi_data++;
        i->delta_pos.y = *opdi_data++;
        i->delta_pos.z = *opdi_data++;
        i->prev_pos = i->pos;
    }
    return opdi_data;
}

void RobCloth::SetOsageReset() {
    osage_reset = true;
}

void RobCloth::SetRing(const osage_ring_data& ring) {
    this->ring = ring;
}

void RobCloth::SetSkinParam(skin_param_file_data* skp) {
    skin_param_ptr = &skp->skin_param;
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
        obj_mesh* mesh = objset_info_storage_get_obj_mesh(itm_eq_obj->obj_info, cls_data->mesh_name);
        obj_mesh* backface_mesh = objset_info_storage_get_obj_mesh(itm_eq_obj->obj_info, cls_data->backface_mesh_name);
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
    const ssize_t root_count = this->root_count;
    CLOTHNode* node = &nodes.data()[root_count];
    if (rob_cloth_update_normals_select) {
        for (size_t i = 1; i < nodes_count - 1; i++) {
            node->CalculateTBN(node, node + 1, &node[-root_count], &node[root_count]);
            node++;
            for (ssize_t j = 1; j < root_count - 1; j++, node++)
                node->CalculateTBN(node - 1, node + 1, &node[-root_count], &node[root_count]);
            node->CalculateTBN(node - 1, node, &node[-root_count], &node[root_count]);
            node++;
        }

        node->CalculateTBN(node, node + 1, &node[-root_count], node);
        node++;

        for (ssize_t i = 1; i < root_count - 1; i++, node++)
            node->CalculateTBN(node - 1, node + 1, &node[-root_count], node);
        node->CalculateTBN(node - 1, node, &node[-root_count], node);
    }
    else
        for (size_t i = 1; i < nodes_count; i++, node++) {
            for (ssize_t j = 0; j < root_count - 1; j++, node++) {
                node[0].tangent_sign = calculate_tbn(
                    node[0].pos, node[-root_count].pos, node[1].pos,
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
    size_t data = (size_t)buffer.MapMemory(gl_state);
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

                    *(vec3*)data = node->pos;
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

                    *(vec3*)data = node->pos;
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

                    *(vec3*)data = node->pos;

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

                    *(vec3*)data = node->pos;

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

                    *(vec3*)data = node->pos;

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

                    *(vec3*)data = node->pos;

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

    buffer.UnmapMemory(gl_state);
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

void ExClothBlock::CtrlBegin() {
    done = false;
}

void ExClothBlock::CtrlStep(int32_t stage, bool disable_external_force) {

}

void ExClothBlock::CtrlMain() {
    ColiSet();

    rob_chara_item_equip* rob_itm_equip = item_equip_object->item_equip;
    float_t step = get_delta_frame() * rob_itm_equip->osage_step;
    if (rob_itm_equip->opd_blend_data.size() && rob_itm_equip->opd_blend_data.front().use_blend)
        step = 1.0f;
    rob.CtrlMain(step, false);
}

void ExClothBlock::CtrlOsagePlayData() {
    rob.CtrlOsagePlayData(item_equip_object->item_equip->opd_blend_data);
}

void ExClothBlock::Disp(const mat4& mat, render_context* rctx) {
    rob.UpdateDisp();
    rob.Disp(mat, rctx);
}

void ExClothBlock::Reset() {
    rob.ResetData();
    bone_node_ptr = 0;
}

void ExClothBlock::Field_40() {

}

void ExClothBlock::CtrlInitBegin() {
    ColiSet();
    rob.CtrlInitBegin();
}

void ExClothBlock::CtrlInitMain() {
    ColiSet();
    rob.CtrlInitMain();
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

// 0x1405F9280
void ExClothBlock::SetMoveCancel(const float_t& value) {
    rob.SetMoveCancel(value);
}

const float_t* ExClothBlock::SetOsagePlayDataInit(const float_t* opdi_data) {
    return rob.SetOsagePlayDataInit(opdi_data);
}

void ExClothBlock::SetOsageReset() {
    rob.SetOsageReset();
}

void ExClothBlock::SetRing(const osage_ring_data& ring) {
    rob.SetRing(ring);
}

void ExClothBlock::SetSkinParam(skin_param_file_data* skp) {
    rob.SetSkinParam(skp);
}

void ExClothBlock::SetSkinParamOsageRoot(const skin_param_osage_root* skp_root) {
    if (!skp_root)
        return;

    rob.SetSkinParamOsageRoot(*skp_root);

    vec3 wind_direction = task_wind->ptr->wind_direction
        * item_equip_object->item_equip->wind_strength;
    rob.SetWindDirection(wind_direction);
}

RobOsage::RobOsage() : skin_param_ptr(), apply_physics(), field_2A1(), field_2A4(),
inertia(), yz_order(), root_matrix_ptr(), move_cancel(), move_cancelled(), osage_reset(),
osage_reset_done(), disable_collision(), set_external_force(), reset_data_list() {

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

// 0x14047D620
void RobOsage::ApplyBocRootColi(const float_t step) {
    if (get_pause() || step <= 0.0f || disable_collision)
        return;

    const vec3 pos = nodes.data()[0].pos;
    const OsageCollision::Work* coli_chara = this->coli_chara;
    const OsageCollision::Work* coli_ring = this->coli_ring;
    const SkinParam::RootCollisionType coli_type = skin_param_ptr->coli_type;

    RobOsageNode* i_begin = nodes.data() + 1;
    RobOsageNode* i_end = nodes.data() + nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++) {
        skin_param_osage_node* skp_osg_node = &i->data_ptr->skp_osg_node;
        for (RobOsageNode*& j : i->data_ptr->boc) {
            float_t hit = (float_t)(
                OsageCollision::osage_capsule_cls(coli_ring, i->pos, j->pos, skp_osg_node->coli_r)
                + OsageCollision::osage_capsule_cls(coli_chara, i->pos, j->pos, skp_osg_node->coli_r));
            i->hit += hit;
            j->hit += hit;
        }

        if (coli_type != SkinParam::RootCollisionTypeEnd
            && (coli_type != SkinParam::RootCollisionTypeCapsule || i != i_begin)) {
            float_t hit = (float_t)(
                OsageCollision::osage_capsule_cls(coli_ring, i->pos, i->GetPrevNode().pos, skp_osg_node->coli_r)
                + OsageCollision::osage_capsule_cls(coli_chara, i->pos, i->GetPrevNode().pos, skp_osg_node->coli_r));
            i->hit += hit;
            i->GetPrevNode().hit += hit;
        }
    }
    nodes.data()[0].pos = pos;
}

// 0x14047EE90
void RobOsage::ApplyResetData(const mat4& mat) {
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
        mat4_transform_vector(&mat, &i->reset_data.delta_pos, &i->delta_pos);
        mat4_transform_point(&mat, &i->reset_data.pos, &i->pos);
    }
    root_matrix_prev = *root_matrix_ptr;
}

// 0x1404803B0
void RobOsage::BeginCalc(const mat4& root_matrix, const vec3& parent_scale, bool has_children_node) {
    mat4 mat = root_matrix;
    const vec3 pos = exp_data.position * parent_scale;
    mat4_transform_point(&mat, &pos, &nodes.data()[0].pos);

    if (osage_reset && !osage_reset_done) {
        osage_reset_done = true;
        ApplyResetData(root_matrix);
    }

    if (!move_cancelled) {
        move_cancelled = true;

        float_t move_cancel = skin_param_ptr->move_cancel;
        if (move_cancel == 1.0f || move_cancel < 0.0f)
            move_cancel = this->move_cancel;

        if (move_cancel > 0.0f) {
            RobOsageNode* i_begin = nodes.data() + 1;
            RobOsageNode* i_end = nodes.data() + nodes.size();
            for (RobOsageNode* i = i_begin; i != i_end; i++) {
                vec3 pos;
                mat4_inverse_transform_point(&root_matrix_prev, &i->pos, &pos);
                mat4_transform_point(root_matrix_ptr, &pos, &pos);
                i->pos += (pos - i->pos) * move_cancel;
            }
        }
    }

    if (!has_children_node)
        return;

    RotateMat(mat, parent_scale);
    *nodes.data()[0].bone_node_mat = mat;

    RobOsageNode* v30_begin = nodes.data() + 1;
    RobOsageNode* v30_end = nodes.data() + nodes.size();
    for (RobOsageNode* v30 = v30_begin; v30 != v30_end; v30++) {
        vec3 direction;
        mat4_inverse_transform_point(&mat, &v30->pos, &direction);

        bool rot_clamped = rotate_matrix_to_direction(mat, direction,
            &v30->data_ptr->skp_osg_node.hinge,
            &v30->reset_data.rotation, yz_order);
        *v30->bone_node_ptr->ex_data_mat = mat;

        v30->TranslateMat(mat, rot_clamped, parent_scale.x);
    }

    if (nodes.size() && end_node.bone_node_mat) {
        mat4 mat = *nodes.back().bone_node_ptr->ex_data_mat;
        mat4_mul_translate_x(&mat, end_node.length * parent_scale.x, &mat);
        *end_node.bone_node_ptr->ex_data_mat = mat;
    }
}

bool RobOsage::CheckPartsBits(const rob_osage_parts_bit& parts_bits) {
    if (osage_setting.parts != ROB_OSAGE_PARTS_NONE)
        return !!(parts_bits & (1 << osage_setting.parts));
    return false;
}

void RobOsage::ColiSet(const mat4* transform) {
    if (skin_param_ptr->coli.size())
        OsageCollision::Work::update_cls_work(coli_chara, skin_param_ptr->coli.data(), transform);
    OsageCollision::Work::update_cls_work(coli_ring, ring.skp_root_coli, transform);
}

// 0x14047ECA0
void RobOsage::CollideNodes(const float_t step) {
    if (get_pause() || step <= 0.0f)
        return;

    const OsageCollision::Work* coli_chara = this->coli_chara;
    const OsageCollision::Work* coli_ring = this->coli_ring;
    const OsageCollision& coli_object = ring.coli_object;

    RobOsageNode* i_begin = nodes.data() + 1;
    RobOsageNode* i_end = nodes.data() + nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++) {
        skin_param_osage_node* skp_osg_node = &i->data_ptr->skp_osg_node;
        i->hit += (float_t)OsageCollision::osage_cls_work_list(i->pos,
            skp_osg_node->coli_r, coli_object, &i->friction);
        if (disable_collision)
            continue;

        for (RobOsageNode*& j : i->data_ptr->boc) {
            j->hit += (float_t)OsageCollision::osage_cls(coli_ring, j->pos, skp_osg_node->coli_r);
            j->hit += (float_t)OsageCollision::osage_cls(coli_chara, j->pos, skp_osg_node->coli_r);
        }

        i->hit += (float_t)OsageCollision::osage_cls(coli_ring, i->pos, skp_osg_node->coli_r);
        i->hit += (float_t)OsageCollision::osage_cls(coli_chara, i->pos, skp_osg_node->coli_r);
    }
}

// 0x14047D8C0
void RobOsage::CollideNodesTargetOsage(const mat4& root_matrix,
    const vec3& parent_scale, const float_t step, bool collide_nodes) {
    if (!osage_reset && (get_pause() || step <= 0.0f))
        return;

    const OsageCollision::Work* coli_chara = this->coli_chara;
    const OsageCollision::Work* coli_ring = this->coli_ring;
    const OsageCollision& coli_object = ring.coli_object;

    const float_t inv_step = step > 0.0f ? 1.0f / step : 0.0f;

    mat4 v64;
    if (collide_nodes)
        v64 = *nodes.data()[0].bone_node_mat;
    else {
        v64 = root_matrix;

        const vec3 pos = exp_data.position * parent_scale;
        mat4_transform_point(&v64, &pos, &nodes.data()[0].pos);
        RotateMat(v64, parent_scale);
    }

    float_t floor_height = -1000.0f;
    if (collide_nodes) {
        RobOsageNode* node = &nodes.data()[0];
        floor_height = ring.get_floor_height(
            node->pos, node->data_ptr->skp_osg_node.coli_r);
    }

    const float_t v23 = step < 1.0f ? 0.2f / (2.0f - step) : 0.2f;

    if (skin_param_ptr->colli_tgt_osg) {
        std::vector<RobOsageNode>* colli_tgt_osg = skin_param_ptr->colli_tgt_osg;
        RobOsageNode* i_begin = nodes.data() + 1;
        RobOsageNode* i_end = nodes.data() + nodes.size();
        for (RobOsageNode* i = i_begin; i != i_end; i++) {
            vec3 v62 = 0.0f;

            RobOsageNode* j_begin = colli_tgt_osg->data() + 1;
            RobOsageNode* j_end = colli_tgt_osg->data() + colli_tgt_osg->size();
            for (RobOsageNode* j = j_begin; j != j_end; j++)
                OsageCollision::cls_ball_oidashi(v62, i->pos, j->pos,
                    i->data_ptr->skp_osg_node.coli_r + j->data_ptr->skp_osg_node.coli_r);
            i->pos += v62;
        }
    }

    RobOsageNode* i_begin = nodes.data() + 1;
    RobOsageNode* i_end = nodes.data() + nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++) {
        const float_t fric = (1.0f - inertia) * skin_param_ptr->friction;
        if (collide_nodes) {
            i->CheckNodeDistance(step, parent_scale.x);
            i->hit += (float_t)OsageCollision::osage_cls_work_list(i->pos,
                i->data_ptr->skp_osg_node.coli_r, coli_object, &i->friction);
            if (!disable_collision) {
                i->hit += (float_t)OsageCollision::osage_cls(coli_ring,
                    i->pos, i->data_ptr->skp_osg_node.coli_r);
                i->hit += (float_t)OsageCollision::osage_cls(coli_chara,
                    i->pos, i->data_ptr->skp_osg_node.coli_r);
            }
            i->CheckFloorCollision(floor_height);
        }
        else
            segment_limit_distance(i->pos, i->GetPrevNode().pos, i->length * parent_scale.x);

        vec3 direction;
        mat4_inverse_transform_point(&v64, &i->pos, &direction);

        bool rot_clamped = rotate_matrix_to_direction(v64, direction,
            &i->data_ptr->skp_osg_node.hinge,
            &i->reset_data.rotation, yz_order);
        i->bone_node_ptr->exp_data.parent_scale = parent_scale;
        *i->bone_node_ptr->ex_data_mat = v64;

        if (i->bone_node_mat)
            mat4_scale_rot(&v64, &parent_scale, i->bone_node_mat);

        i->reset_data.length = i->TranslateMat(v64, rot_clamped, parent_scale.x);

        i->delta_pos = (i->pos - i->fixed_pos) * inv_step;

        if (i->hit > 0.0f)
            i->delta_pos *= min_def(fric, i->friction);

        const float_t v55 = vec3::length_squared(i->delta_pos);
        if (v55 > v23 * v23)
            i->delta_pos *= v23 / sqrtf(v55);

        mat4_inverse_transform_point(&root_matrix, &i->pos, &i->reset_data.pos);
        mat4_inverse_transform_vector(&root_matrix, &i->delta_pos, &i->reset_data.delta_pos);
    }

    if (nodes.size() && end_node.bone_node_mat) {
        mat4 mat = *nodes.back().bone_node_ptr->ex_data_mat;
        mat4_mul_translate_x(&mat, end_node.length * parent_scale.x, &mat);
        *end_node.bone_node_ptr->ex_data_mat = mat;

        mat4_scale_rot(&mat, &parent_scale, &mat);
        *end_node.bone_node_mat = mat;
        end_node.bone_node_ptr->exp_data.parent_scale = parent_scale;
    }
}

// 0x14047E1C0
void RobOsage::CtrlEnd(const vec3& parent_scale) {
    RobOsageNode* i_begin = nodes.data() + 1;
    RobOsageNode* i_end = nodes.data() + nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++)
        if (i->data_ptr->normal_ref.set) {
            i->data_ptr->normal_ref.GetMat(i->bone_node_mat);
            mat4_scale_rot(i->bone_node_mat, &parent_scale, i->bone_node_mat);
        }
}

// 0x14047F990
void RobOsage::CtrlInitBegin(const mat4& root_matrix, const vec3& parent_scale, bool sibling_node) {
    if (!nodes.size())
        return;

    vec3 root_pos = exp_data.position * parent_scale;
    mat4_transform_point(&root_matrix, &root_pos, &root_pos);
    RobOsageNode* v12 = &nodes.data()[0];
    v12->pos = root_pos;
    v12->fixed_pos = root_pos;
    v12->delta_pos = 0.0f;

    RobOsageNode* node = &nodes.data()[0];
    const float_t floor_height = ring.get_floor_height(
        node->pos, node->data_ptr->skp_osg_node.coli_r);

    mat4 v78 = root_matrix;
    RotateMat(v78, parent_scale, true);

    vec3 v60 = { 1.0f, 0.0f, 0.0f };
    mat4_transform_vector(&v78, &v60, &v60);

    RobOsageNode* i_begin = this->nodes.data() + 1;
    RobOsageNode* i_end = this->nodes.data() + this->nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++) {
        vec3 v74 = i->GetPrevNode().pos + vec3::normalize(v60) * (i->length * parent_scale.x);
        if (sibling_node && i->sibling_node)
            segment_limit_distance(v74, i->sibling_node->pos, i->max_distance);

        skin_param_osage_node* v38 = &i->data_ptr->skp_osg_node;
        OsageCollision::osage_cls(coli_ring, v74, v38->coli_r);
        OsageCollision::osage_cls(coli_chara, v74, v38->coli_r);

        const float_t v39 = floor_height + v38->coli_r;
        if (v74.y < v39 && v39 < 1001.0f) {
            v74.y = v39;
            v74 = vec3::normalize(v74 - i->GetPrevNode().pos) * (i->length * parent_scale.x) + i->GetPrevNode().pos;
        }
        i->pos = v74;
        i->delta_pos = 0.0f;

        vec3 direction;
        mat4_inverse_transform_point(&v78, &i->pos, &direction);

        rotate_matrix_to_direction(v78, direction, &i->data_ptr->skp_osg_node.hinge,
            &i->reset_data.rotation, yz_order);
        i->bone_node_ptr->exp_data.parent_scale = parent_scale;
        *i->bone_node_ptr->ex_data_mat = v78;

        if (i->bone_node_mat)
            mat4_scale_rot(&v78, &parent_scale, i->bone_node_mat);

        float_t v55 = vec3::distance(i->pos, i->GetPrevNode().pos);
        float_t v56 = i->length * parent_scale.x;
        if (v55 >= fabsf(v56))
            v56 = v55;
        mat4_mul_translate(&v78, v56, 0.0f, 0.0f, &v78);
        mat4_get_translation(&v78, &i->pos);
        v60 = i->pos - i->GetPrevNode().pos;
    }

    if (nodes.size() && end_node.bone_node_mat) {
        mat4 mat = *nodes.back().bone_node_ptr->ex_data_mat;
        mat4_mul_translate_x(&mat, end_node.length * parent_scale.x, &mat);
        *end_node.bone_node_ptr->ex_data_mat = mat;

        mat4_scale_rot(&mat, &parent_scale, &mat);
        *end_node.bone_node_mat = mat;
        end_node.bone_node_ptr->exp_data.parent_scale = parent_scale;
    }
}

// 0x14047C770
void RobOsage::CtrlInitMain(const mat4& root_matrix, const vec3& parent_scale,
    const float_t step, bool disable_external_force) {
    ApplyPhysics(root_matrix, parent_scale, step, disable_external_force, false, false);
    CollideNodesTargetOsage(root_matrix, parent_scale, step, true);
    EndCalc(root_matrix, parent_scale, step, disable_external_force);
}

// 0x14047C750
void RobOsage::CtrlMain(const mat4& root_matrix, const vec3& parent_scale, const float_t step) {
    CtrlInitMain(root_matrix, parent_scale, step, false);
}

// 0x14047E240
void RobOsage::CtrlOsagePlayData(const mat4& root_matrix,
    const vec3& parent_scale, std::vector<opd_blend_data>& opd_blend_data) {
    if (!opd_blend_data.size())
        return;

    const vec3 v63 = exp_data.position * parent_scale;

    mat4 v85 = root_matrix;
    mat4_transform_point(&v85, &v63, &nodes.data()[0].pos);

    RotateMat(v85, parent_scale);
    *nodes.data()[0].bone_node_mat = v85;
    *nodes.data()[0].bone_node_ptr->ex_data_mat = v85;

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

        mat4 v87 = v85;
        vec3 parent_curr_trans = nodes.data()[0].pos;
        vec3 parent_next_trans = nodes.data()[0].pos;

        RobOsageNode* j_begin = nodes.data() + 1;
        RobOsageNode* j_end = nodes.data() + nodes.size();
        for (RobOsageNode* j = j_begin; j != j_end; j++) {
            opd_vec3_data* opd = &j->opd_data[i - i_end];
            const float_t* opd_x = opd->x;
            const float_t* opd_y = opd->y;
            const float_t* opd_z = opd->z;

            vec3 curr_trans;
            curr_trans.x = opd_x[curr_key];
            curr_trans.y = opd_y[curr_key];
            curr_trans.z = opd_z[curr_key];

            vec3 next_trans;
            next_trans.x = opd_x[next_key];
            next_trans.y = opd_y[next_key];
            next_trans.z = opd_z[next_key];

            mat4_transform_point(&root_matrix, &curr_trans, &curr_trans);
            mat4_transform_point(&root_matrix, &next_trans, &next_trans);

            vec3 _trans = curr_trans * inv_blend + next_trans * blend;

            vec3 direction;
            mat4_inverse_transform_point(&v87, &_trans, &direction);

            vec3 rotation = 0.0f;
            rotate_matrix_to_direction(v87, direction, 0, &rotation, yz_order);
            mat4_set_translation(&v87, &_trans);

            float_t length = vec3::distance(curr_trans, parent_curr_trans) * inv_blend
                + vec3::distance(next_trans, parent_next_trans) * blend;
            j->opd_node_data.set_data(i, { length, rotation });

            parent_curr_trans = curr_trans;
            parent_next_trans = next_trans;
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
        j->fixed_pos = j->pos;
        mat4_get_translation(&v85, &j->pos);
    }

    if (nodes.size() && end_node.bone_node_mat) {
        mat4 mat = *nodes.back().bone_node_ptr->ex_data_mat;
        mat4_mul_translate_x(&mat, end_node.length * parent_scale.x, &mat);
        *end_node.bone_node_ptr->ex_data_mat = mat;

        mat4_scale_rot(&mat, &parent_scale, &mat);
        *end_node.bone_node_mat = mat;
        end_node.bone_node_ptr->exp_data.parent_scale = parent_scale;
    }
}

// 0x140480260
void RobOsage::EndCalc(const mat4& root_matrix, const vec3& parent_scale,
    const float_t step, bool disable_external_force) {
    if (!disable_external_force) {
        SetNodesExternalForce(0, 1.0f);
        SetNodesForce(1.0f);
        set_external_force = false;
        external_force = 0.0f;
    }

    apply_physics = true;
    field_2A1 = false;
    field_2A4 = -1.0f;
    move_cancelled = false;
    osage_reset = false;
    osage_reset_done = false;

    for (RobOsageNode& i : nodes) {
        i.hit = 0.0f;
        i.friction = 1.0f;
    }

    root_matrix_prev = *root_matrix_ptr;
}

RobOsageNode* RobOsage::GetNode(size_t index) {
    if (index < nodes.size())
        return &nodes.data()[index];
    else
        return &end_node;
}

void RobOsage::InitData(obj_skin_block_osage* osg_data, obj_skin_osage_node* osg_nodes,
    bone_node* ex_data_bone_nodes, obj_skin* skin) {
    Reset();
    exp_data.set_position_rotation(osg_data->node.position, osg_data->node.rotation);
    nodes.clear();
    nodes.resize(osg_data->count + 1ULL);

    RobOsageNode* node = nodes.data();
    node->Reset();
    node->child_length = osg_nodes->length;
    bone_node* node_ptr = &ex_data_bone_nodes[osg_data->name_index & 0x7FFF];
    node->bone_node_ptr = node_ptr;
    node->bone_node_mat = node_ptr->mat;
    *node_ptr->ex_data_mat = mat4_identity;
    node->sibling_node = 0;

    bone_node* parent_node_ptr = node->bone_node_ptr;
    for (int32_t i = 0; i < osg_data->count; i++) {
        obj_skin_osage_node* osg_node = &osg_nodes[i];
        RobOsageNode* node = &nodes.data()[i + 1LL];
        node->Reset();
        node->length = osg_node[0].length;
        if (i != osg_data->count - 1)
            node->child_length = osg_node[1].length;
        bone_node* node_ptr = &ex_data_bone_nodes[osg_node->name_index & 0x7FFF];
        node->bone_node_ptr = node_ptr;
        node->bone_node_mat = node_ptr->mat;
        node_ptr->parent = parent_node_ptr;
        parent_node_ptr = node->bone_node_ptr;
    }

    end_node.Reset();
    end_node.length = osg_nodes[osg_data->count - 1].length;
    bone_node* end_node_ptr = &ex_data_bone_nodes[osg_data->end_name_index & 0x7FFF];
    end_node.bone_node_ptr = end_node_ptr;
    end_node.bone_node_mat = end_node_ptr->mat;
    end_node_ptr->parent = parent_node_ptr;
    *end_node.bone_node_ptr->ex_data_mat = mat4_identity;

    if (osg_data->count) {
        obj_skin_osage_node* node_array = osg_data->node_array;
        size_t v24 = 0;
        RobOsageNode* node = &nodes.data()[1];
        if (node_array)
            for (size_t i = 0; i < osg_data->count; i++, node++) {
                mat3 mat;
                mat3_rotate_zyx(&node_array[i].rotation, &mat);

                vec3 v49 = { node->length, 0.0f, 0.0f };
                mat3_transform_vector(&mat, &v49, &node->rel_pos);
            }
        else
            for (size_t i = 0; i < osg_data->count; i++, node++)
                node->rel_pos = { node->length, 0.0f, 0.0f };
    }

    if (osg_data->count) {
        RobOsageNode* node = &nodes.data()[1];
        size_t v33 = 0;
        for (int32_t i = 0; i < osg_data->count; i++, node++) {
            node->mat = mat4_null;

            obj_skin_bone* bone = skin->bone_array;
            for (int32_t j = 0; j < skin->num_bone; j++, bone++)
                if (bone->id == osg_nodes->name_index) {
                    mat4_invert_fast(&bone->inv_bind_pose_mat, &node->mat);
                    break;
                }

            osg_nodes++;
        }
    }

    RobOsageNode* last_node = &nodes.back();
    if (memcmp(&last_node->mat, &mat4_null, sizeof(mat4)))
        mat4_mul_translate(&last_node->mat, end_node.length, 0.0f, 0.0f, &end_node.mat);
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
    end_node.Reset();
    wind_direction = 0.0f;
    inertia = 0.0f;
    yz_order = 0;
    apply_physics = true;
    motion_reset_data.clear();
    move_cancel = 0.0f;
    move_cancelled = false;
    osage_reset = false;
    osage_reset_done = false;
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
    root_matrix_prev = mat4_null;
}

void RobOsage::ResetBoc() {
    RobOsageNode* i_begin = nodes.data() + 1;
    RobOsageNode* i_end = nodes.data() + nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++)
        i->data_ptr->boc.clear();
}

void RobOsage::ResetExtrenalForce() {
    set_external_force = false;
    external_force = 0.0f;
}

// 0x14047F110
void RobOsage::RotateMat(mat4& mat, const vec3& parent_scale, bool init_rot) {
    const vec3 position = exp_data.position * parent_scale;
    mat4_mul_translate(&mat, &position, &mat);
    mat4_mul_rotate_zyx(&mat, &exp_data.rotation, &mat);

    const vec3 rot = init_rot ? skin_param_ptr->init_rot + skin_param_ptr->rot : skin_param_ptr->rot;
    mat4_mul_rotate_zyx(&mat, &rot, &mat);
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

// 0x140481090
void RobOsage::SetDisableCollision(const bool& value) {
    disable_collision = value;
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
        skin_param_osage_node* skp_osg_node = &i->data_ptr->skp_osg_node;
        skp_osg_node->hinge.ymin = -hinge_y;
        skp_osg_node->hinge.ymax = hinge_y;
        skp_osg_node->hinge.zmin = -hinge_z;
        skp_osg_node->hinge.zmax = hinge_z;
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

// 0x1404810D0
void RobOsage::SetMoveCancel(const float_t& value) {
    move_cancel = value;
}

// 0x140480F40
void RobOsage::SetNodesExternalForce(const vec3* external_force, const float_t& strength) {
    if (!external_force) {
        RobOsageNode* i_begin = nodes.data() + 1;
        RobOsageNode* i_end = nodes.data() + nodes.size();
        for (RobOsageNode* i = i_begin; i != i_end; i++)
            i->external_force = 0.0f;
        return;
    }

    vec3 _external_force = *external_force;
    for (size_t i = osage_setting.exf; i; i--)
        _external_force *= strength;

    RobOsageNode* i_begin = nodes.data() + 1;
    RobOsageNode* i_end = nodes.data() + nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++) {
        i->external_force = _external_force;
        _external_force *= strength;
    }
}

// 0x140481540
void RobOsage::SetNodesForce(const float_t& force) {
    RobOsageNode* i_begin = nodes.data() + 1;
    RobOsageNode* i_end = nodes.data() + nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++)
        i->force = force;
}

const float_t* RobOsage::SetOsagePlayDataInit(const float_t* opdi_data) {
    RobOsageNode* i_begin = nodes.data() + 1;
    RobOsageNode* i_end = nodes.data() + nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++) {
        i->pos.x = *opdi_data++;
        i->pos.y = *opdi_data++;
        i->pos.z = *opdi_data++;
        i->delta_pos.x = *opdi_data++;
        i->delta_pos.y = *opdi_data++;
        i->delta_pos.z = *opdi_data++;
        i->fixed_pos = i->pos;
    }
    return opdi_data;
}

// 0x1405F9A90, but it's combined with ExOsageBlock::SetOsageReset
void RobOsage::SetOsageReset() {
    osage_reset = true;
}

void RobOsage::SetRing(const osage_ring_data& ring) {
    this->ring = ring;
}

void RobOsage::SetRot(float_t rot_y, float_t rot_z) {
    skin_param_ptr->rot.y = rot_y * DEG_TO_RAD_FLOAT;
    skin_param_ptr->rot.z = rot_z * DEG_TO_RAD_FLOAT;
}

void RobOsage::SetSkinParam(skin_param_file_data* skp) {
    if (skp->nodes_data.size() == nodes.size() - 1) {
        skin_param_ptr = &skp->skin_param;

        size_t node_index = 0;
        RobOsageNode* i_begin = nodes.data() + 1;
        RobOsageNode* i_end = nodes.data() + nodes.size();
        for (RobOsageNode* i = i_begin; i != i_end; i++)
            i->data_ptr = &skp->nodes_data[node_index++];
    }
    else {
        skin_param_ptr = &skin_param;
        for (RobOsageNode& i : nodes)
            i.data_ptr = &i.data;
    }
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

void RobOsage::SetWindDirection(const vec3* value) {
    if (value)
        wind_direction = *value;
    else
        wind_direction = 0.0f;
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

void ExOsageBlock::CtrlBegin() {
    done = false;
}

void ExOsageBlock::CtrlStep(int32_t stage, bool disable_external_force) {
    rob_chara_item_equip* rob_itm_equip = item_equip_object->item_equip;
    float_t step = get_delta_frame() * rob_itm_equip->osage_step;
    if (rob_itm_equip->opd_blend_data.size() && rob_itm_equip->opd_blend_data.front().use_blend)
        step = 1.0f;

    const mat4& root_matrix = *parent_bone_node->ex_data_mat;
    const vec3 parent_scale = parent_bone_node->exp_data.parent_scale;
    switch (stage) {
    case 0:
        rob.BeginCalc(root_matrix, parent_scale, has_children_node);
        break;
    case 1:
    case 2:
        if ((stage == 1 && is_parent) || (stage == 2 && rob.apply_physics)) {
            SetWindDirection();
            rob.ApplyPhysics(root_matrix, parent_scale, step,
                disable_external_force, true, has_children_node);
        }
        break;
    case 3:
        rob.ColiSet(mats);
        rob.CollideNodes(step);
        break;
    case 4:
        rob.ApplyBocRootColi(step);
        break;
    case 5: {
        rob.CollideNodesTargetOsage(root_matrix, parent_scale, step, false);
        rob.EndCalc(root_matrix, parent_scale, step, disable_external_force);
        done = true;
    } break;
    }
}

void ExOsageBlock::CtrlMain() {
    field_1FF8 &= ~2;
    if (done) {
        done = false;
        return;
    }

    rob_chara_item_equip* rob_itm_equip = item_equip_object->item_equip;
    float_t step = get_delta_frame() * rob_itm_equip->osage_step;
    if (rob_itm_equip->opd_blend_data.size() && rob_itm_equip->opd_blend_data.front().use_blend)
        step = 1.0f;

    const vec3 parent_scale = parent_bone_node->exp_data.parent_scale;
    vec3 scale = parent_bone_node->exp_data.scale;

    mat4 root_matrix = *parent_bone_node->ex_data_mat;
    if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f) {
        scale = 1.0f / scale;
        mat4_scale_rot(&root_matrix, &scale, &root_matrix);
    }
    SetWindDirection();

    rob.ColiSet(mats);
    rob.CtrlMain(root_matrix, parent_scale, step);
}

void ExOsageBlock::CtrlOsagePlayData() {
    rob.CtrlOsagePlayData(*parent_bone_node->ex_data_mat,
        parent_bone_node->exp_data.parent_scale, item_equip_object->item_equip->opd_blend_data);
}

void ExOsageBlock::Disp(const mat4& mat, render_context* rctx) {

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

void ExOsageBlock::CtrlInitBegin() {
    step = 4.0f;
    SetWindDirection();
    rob.ColiSet(mats);
    const vec3 parent_scale = parent_bone_node->exp_data.parent_scale;
    rob.CtrlInitBegin(*parent_bone_node->ex_data_mat, parent_scale, false);
    field_1FF8 &= ~2;
}

void ExOsageBlock::CtrlInitMain() {
    if (done) {
        done = false;
        return;
    }

    SetWindDirection();

    const vec3 parent_scale = parent_bone_node->exp_data.parent_scale;
    rob.ColiSet(mats);
    rob.CtrlInitMain(*parent_bone_node->ex_data_mat, parent_scale, step, true);
    step = max_def(step * 0.5f, 1.0f);
}

void ExOsageBlock::CtrlEnd() {
    const vec3 parent_scale = parent_bone_node->exp_data.parent_scale;
    rob.CtrlEnd(parent_scale);
    done = false;
}

void ExOsageBlock::AddMotionResetData(uint32_t motion_id, float_t frame) {
    rob.AddMotionResetData(motion_id, frame);
}

void ExOsageBlock::InitData(rob_chara_item_equip_object* itm_eq_obj,
    obj_skin_block_osage* osg_data, const char* osg_data_name, obj_skin_osage_node* osg_nodes,
    bone_node* bone_nodes, bone_node* ex_data_bone_nodes, obj_skin* skin) {
    ExNodeBlock::InitData(&ex_data_bone_nodes[osg_data->name_index & 0x7FFF],
        EX_OSAGE, osg_data_name, itm_eq_obj);
    rob.InitData(osg_data, osg_nodes, ex_data_bone_nodes, skin);
    field_1FF8 &= ~2;
    mats = bone_nodes->mat;
}

const float_t* ExOsageBlock::LoadOpdData(size_t node_index, const float_t* opd_data, size_t opd_count) {
    return rob.LoadOpdData(node_index, opd_data, opd_count);
}

// 0x1405F9510
void ExOsageBlock::SetDisableCollision(const bool& value) {
    rob.SetDisableCollision(value);
}

void ExOsageBlock::SetMotionResetData(uint32_t motion_id, float_t frame) {
    rob.SetMotionResetData(motion_id, frame);
}

const float_t* ExOsageBlock::SetOsagePlayDataInit(const float_t* opdi_data) {
    return rob.SetOsagePlayDataInit(opdi_data);
}

// 0x1405F9290
void ExOsageBlock::SetMoveCancel(const float_t& value) {
    rob.SetMoveCancel(value);
}

// 0x1405F9A90, but it's combined with RobOsage::SetOsageReset
void ExOsageBlock::SetOsageReset() {
    rob.SetOsageReset();
}

void ExOsageBlock::SetRing(const osage_ring_data& ring) {
    rob.SetRing(ring);
}

void ExOsageBlock::SetSkinParam(skin_param_file_data* skp) {
    rob.SetSkinParam(skp);
}

void ExOsageBlock::SetWindDirection() {
    vec3 wind_direction = task_wind->ptr->wind_direction
        * item_equip_object->item_equip->wind_strength;
    rob.SetWindDirection(&wind_direction);
}

void ExOsageBlock::sub_1405F3E10(obj_skin_block_osage* osg_data,
    obj_skin_osage_node* osg_nodes, prj::vector_pair<uint32_t, RobOsageNode*>* a4,
    std::map<const char*, ExNodeBlock*>* a5) {
    RobOsageNode* node = rob.GetNode(0);
    a4->push_back(osg_data->name_index, node);

    for (int32_t i = 0; i < osg_data->count; i++) {
        node = rob.GetNode(i + 1ULL);
        a4->push_back(osg_nodes[i].name_index, node);

        if (node->bone_node_ptr && node->bone_node_ptr->name)
            a5->insert({ node->bone_node_ptr->name, this });
    }

    node = rob.GetNode(osg_data->count + 1ULL);
    a4->push_back(osg_data->end_name_index, node);

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

void ExConstraintBlock::CtrlBegin() {
    if (bone_node_ptr) {
        bone_node_expression_data* node_exp_data = &bone_node_ptr->exp_data;
        node_exp_data->position = cns_data->node.position;
        node_exp_data->rotation = cns_data->node.rotation;
        node_exp_data->scale = cns_data->node.scale;
    }
    done = false;
}

void ExConstraintBlock::CtrlStep(int32_t stage, bool disable_external_force) {
    if (done)
        return;

    switch (stage) {
    case 0:
        if (is_parent)
            CtrlMain();
        break;
    case 2:
        if (has_children_node)
            DataSet();
        break;
    case 5:
        CtrlMain();
        break;
    }
}

void ExConstraintBlock::CtrlMain() {
    if (!parent_bone_node)
        return;

    if (done) {
        done = false;
        return;
    }

    Calc();
    DataSet();
    done = true;
}

void ExConstraintBlock::CtrlOsagePlayData() {
    CtrlMain();
}

void ExConstraintBlock::Disp(const mat4& mat, render_context* rctx) {

}

void ExConstraintBlock::Field_40() {

}

void ExConstraintBlock::CtrlInitBegin() {
    CtrlMain();
}

void ExConstraintBlock::CtrlInitMain() {
    CtrlMain();
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
    case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
        CalcConstraintOrientation(mat);
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
        CalcConstraintDirection(mat);
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION:
        CalcConstraintPosition(mat);
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
        CalcConstraintDistance(mat);
        break;
    default:
        *node->mat = mat;
        break;
    }
}

inline void ExConstraintBlock::CalcConstraintDirection(mat4 mat) {
    obj_skin_block_constraint_direction* direction = cns_data->direction;

    vec3 align_axis = direction->align_axis;
    vec3 target_offset = direction->target_offset;
    mat4_transform_point(source_node_bone_node->mat, &target_offset, &target_offset);
    mat4_inverse_transform_point(&mat, &target_offset, &target_offset);
    float_t target_offset_length = vec3::length_squared(target_offset);
    if (target_offset_length <= 0.000001f)
        return;

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
    mat4_mul(&v59, &mat, bone_node_ptr->mat);
}

void ExConstraintBlock::CalcConstraintDistance(mat4 mat) {
    *bone_node_ptr->mat = mat;
}

void ExConstraintBlock::CalcConstraintOrientation(mat4 mat) {
    obj_skin_block_constraint_orientation* orientation = cns_data->orientation;

    mat3 rot;
    mat4_to_mat3(source_node_bone_node->mat, &rot);
    mat3_normalize_rotation(&rot, &rot);
    mat3_mul_rotate_zyx(&rot, &orientation->offset, &rot);
    mat4_replace_rotation(&mat, &rot, bone_node_ptr->mat);
}

void ExConstraintBlock::CalcConstraintPosition(mat4 mat) {
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
    mat4_mul(&mat, &constrained_offset_mat, bone_node_ptr->mat);
}

void ExConstraintBlock::DataSet() {
    if (!bone_node_ptr)
        return;

    bone_node_expression_data* exp_data = &bone_node_ptr->exp_data;
    const vec3 parent_scale = parent_bone_node->exp_data.parent_scale;

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

void ExExpressionBlock::CtrlBegin() {
    bone_node_expression_data* node_exp_data = &bone_node_ptr->exp_data;
    node_exp_data->position = exp_data->node.position;
    node_exp_data->rotation = exp_data->node.rotation;
    node_exp_data->scale = exp_data->node.scale;
    done = false;
}

void ExExpressionBlock::CtrlStep(int32_t stage, bool disable_external_force) {
    if (done)
        return;

    switch (stage) {
    case 0:
        if (is_parent)
            CtrlMain();
        break;
    case 2:
        if (has_children_node)
            DataSet();
        break;
    case 5:
        CtrlMain();
        break;
    }
}

void ExExpressionBlock::CtrlMain() {
    if (!parent_bone_node)
        return;

    if (done) {
        done = false;
        return;
    }

    Calc();
    DataSet();
    done = true;
}

void ExExpressionBlock::CtrlOsagePlayData() {
    CtrlMain();
}

void ExExpressionBlock::Disp(const mat4& mat, render_context* rctx) {

}

void ExExpressionBlock::Field_40() {

}

void ExExpressionBlock::CtrlInitBegin() {
    CtrlMain();
}

void ExExpressionBlock::CtrlInitMain() {
    CtrlMain();
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
    const vec3 parent_scale = parent_bone_node->exp_data.parent_scale;
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
                min_def(value_type.size(), 5)) && value_type.size() == 5) {
                printf_debug_info("Expression error: %s.", this->name);
                if (index >= 0 && index < 9)
                    printf_debug_info("%s\n", expression_component_names[index]);
                else
                    printf_debug_info("\n");
                break;
            }

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

// 0x140482300
static void apply_gravity(vec3& vec, const vec3& p0, const vec3& p1,
    const float_t gravity, const float_t weight) {
    vec3 diff = p0 - p1;
    float_t dist = vec3::length(diff);
    if (dist <= fabsf(gravity)) {
        diff.y = gravity;
        dist = vec3::length(diff);
    }
    diff *= 1.0f / dist;
    vec = vec3(0.0f, -(gravity * weight), 0.0f) - (diff * diff.y * (gravity * weight));
}

// 0x14021A5E0
static float_t calculate_tbn(const vec3& pos_a, const vec3& pos_b,
    const float_t uv_a_y, const float_t uv_b_y,
    vec3& tangent, vec3& binormal, vec3& normal) {
    normal = vec3::normalize(vec3::cross(pos_a, pos_b));

    vec3 _tangent = vec3::normalize(pos_a * uv_b_y - pos_b * uv_a_y);
    binormal = vec3::cross(_tangent, normal);
    tangent = vec3::cross(normal, binormal);

    mat3 mat = mat3(tangent, binormal, normal);
    mat3_transpose(&mat, &mat);
    return mat3_determinant(&mat);
}

// 0x14021A290
static float_t calculate_tbn(const vec3& trans_a, const vec3& trans_b, const vec3& trans_c,
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

    mat3 mat = mat3(tangent, binormal, normal);
    mat3_transpose(&mat, &mat);
    return mat3_determinant(&mat);
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

        const float_t p0_dist = vec3::distance_squared(p0, p0_proj);
        const float_t q0_dist = vec3::distance_squared(q0, q0_proj);
        const float_t p1_dist = vec3::distance_squared(p1, p1_proj);
        const float_t q1_dist = vec3::distance_squared(q1, q1_proj);

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
        const float_t d0_len = vec3::length(d0);
        if (d0_len != 0.0f)
            d0 *= 1.0f / d0_len;

        const float_t d1_len = cls->vec_center_length;
        if (d1_len != 0.0f)
            d1 *= 1.0f / d1_len;

        const float_t b = vec3::dot(d1, d0);
        const float_t t = vec3::dot(d1 * b - d0, p1 - p0) / (b * b - 1.0f);
        if (t < 0.0f)
            vec = p0;
        else if (t <= d0_len)
            vec = p0 + d0 * t;
        else
            vec = q0;
    }
}

// 0x140482FF0
static bool rotate_matrix_to_direction(mat4& mat, const vec3& direction,
    const skin_param_hinge* hinge, vec3* rot, const int32_t& yz_order) {
    bool rot_clamped = false;
    float_t z_rot;
    float_t y_rot;
    if (yz_order == 1) {
        y_rot = atan2f(-direction.z, direction.x);
        z_rot = atan2f(direction.y, sqrtf(direction.x * direction.x + direction.z * direction.z));
        if (hinge)
            rot_clamped = hinge->clamp(y_rot, z_rot);
        mat4_mul_rotate_y(&mat, y_rot, &mat);
        mat4_mul_rotate_z(&mat, z_rot, &mat);
    }
    else {
        z_rot = atan2f(direction.y, direction.x);
        y_rot = atan2f(-direction.z, sqrtf(direction.x * direction.x + direction.y * direction.y));
        if (hinge)
            rot_clamped = hinge->clamp(y_rot, z_rot);
        mat4_mul_rotate_z(&mat, z_rot, &mat);
        mat4_mul_rotate_y(&mat, y_rot, &mat);
    }

    if (rot) {
        rot->y = y_rot;
        rot->z = z_rot;
    }
    return rot_clamped;
}

// 0x140482F30
static void segment_limit_distance(vec3& p0, const vec3& p1, float_t max_distance) {
    const vec3 d = p0 - p1;
    const float_t dist = vec3::length_squared(d);
    if (dist > max_distance * max_distance)
        p0 = p1 + d * (max_distance / sqrtf(dist));
}

// 0x14047C800
void RobOsage::ApplyPhysics(const mat4& root_matrix, const vec3& parent_scale,
    const float_t step, bool disable_external_force, bool ring_coli, bool has_children_node) {
    if (!nodes.size())
        return;

    const float_t osage_gravity_const = get_osage_gravity_const();
    BeginCalc(root_matrix, parent_scale, false);

    RobOsageNode* node = &nodes.data()[0];
    node->fixed_pos = node->pos;
    const vec3 v113 = exp_data.position * parent_scale;

    mat4 v130 = root_matrix;
    mat4_transform_point(&v130, &v113, &node->pos);
    node->delta_pos = node->pos - node->fixed_pos;

    RotateMat(v130, parent_scale);
    *nodes.data()[0].bone_node_mat = v130;
    *nodes.data()[0].bone_node_ptr->ex_data_mat = v130;

    vec3 direction = { 1.0f, 0.0f, 0.0f };
    mat4_transform_vector(&v130, &direction, &direction);

    if (!osage_reset && (get_pause() || step <= 0.0f))
        return;

    const bool stiffness = skin_param_ptr->stiffness > 0.0f;

    RobOsageNode* v26_begin = nodes.data() + 1;
    RobOsageNode* v26_end = nodes.data() + nodes.size();
    for (RobOsageNode* v26 = v26_begin; v26 != v26_end; v26++) {
        float_t weight = v26->data_ptr->skp_osg_node.weight;
        vec3 force;
        if (!set_external_force) {
            apply_gravity(force, v26->pos, v26->GetPrevNode().pos, osage_gravity_const, weight);

            if (v26 != v26_end - 1) {
                vec3 _force;
                apply_gravity(_force, v26->pos, v26->GetNextNode().pos, osage_gravity_const, weight);
                force = (force + _force) * 0.5f;
            }
        }
        else
            force = external_force * (1.0f / weight);

        const vec3 _direction = direction * (v26->data_ptr->force * v26->force);
        const float_t fric = (1.0f - inertia) * (1.0f - skin_param_ptr->air_res);

        vec3 vel = force + _direction - v26->delta_pos * fric + v26->external_force * weight;

        if (!disable_external_force)
            vel += wind_direction * skin_param_ptr->wind_afc;

        if (stiffness)
            vel -= (v26->delta_pos - v26->GetPrevNode().delta_pos) * (1.0f - skin_param_ptr->air_res);

        v26->vel = vel * (1.0f / (weight - (weight - 1.0f) * v26->data_ptr->skp_osg_node.inertial_cancel));
    }

    if (stiffness) {
        mat4 v131 = v130;
        vec3 v111;
        mat4_get_translation(&v131, &v111);

        RobOsageNode* v55_begin = nodes.data() + 1;
        RobOsageNode* v55_end = nodes.data() + nodes.size();
        for (RobOsageNode* v55 = v55_begin; v55 != v55_end; v55++) {
            vec3 v128;
            mat4_transform_point(&v131, &v55->rel_pos, &v128);

            const vec3 delta_pos = v55->delta_pos + v55->vel;
            vec3 v126 = v55->pos + delta_pos;
            segment_limit_distance(v126, v111, v55->length * parent_scale.x);

            vec3 v117 = (v128 - v126) * skin_param_ptr->stiffness;
            const float_t weight = v55->data_ptr->skp_osg_node.weight;
            v117 *= 1.0f / (weight - (weight - 1.0f) * v55->data_ptr->skp_osg_node.inertial_cancel);
            v55->vel += v117;

            v126 = v55->pos + delta_pos + v117;

            vec3 direction;
            mat4_inverse_transform_point(&v131, &v126, &direction);

            rotate_matrix_to_direction(v131, direction, 0, 0, yz_order);

            mat4_mul_translate(&v131, vec3::distance(v111, v126), 0.0f, 0.0f, &v131);

            v111 = v126;
        }
    }

    RobOsageNode* v82_begin = nodes.data() + 1;
    RobOsageNode* v82_end = nodes.data() + nodes.size();
    for (RobOsageNode* v82 = v82_begin; v82 != v82_end; v82++) {
        v82->fixed_pos = v82->pos;
        v82->delta_pos += v82->vel;
        v82->pos += v82->delta_pos;
    }

    if (nodes.size() > 1) {
        RobOsageNode* v90_begin = nodes.data() + nodes.size() - 2;
        RobOsageNode* v90_end = nodes.data();
        for (RobOsageNode* v90 = v90_begin; v90 != v90_end; v90--)
            segment_limit_distance(v90[0].pos, v90[1].pos, v90->child_length * parent_scale.x);
    }

    if (ring_coli) {
        RobOsageNode* node = &nodes.data()[0];
        const float_t floor_height = ring.get_floor_height(
            node->pos, node->data_ptr->skp_osg_node.coli_r);

        RobOsageNode* v98_begin = nodes.data() + 1;
        RobOsageNode* v98_end = nodes.data() + nodes.size();
        for (RobOsageNode* v98 = v98_begin; v98 != v98_end; v98++) {
            v98->CheckNodeDistance(step, parent_scale.x);
            v98->CheckFloorCollision(floor_height);
        }
    }

    if (has_children_node) {
        RobOsageNode* v99_begin = nodes.data() + 1;
        RobOsageNode* v99_end = nodes.data() + nodes.size();
        for (RobOsageNode* v99 = v99_begin; v99 != v99_end; v99++) {
            vec3 direction;
            mat4_inverse_transform_point(&v130, &v99->pos, &direction);

            bool rot_clamped = rotate_matrix_to_direction(v130, direction,
                &v99->data_ptr->skp_osg_node.hinge,
                &v99->reset_data.rotation, yz_order);
            *v99->bone_node_ptr->ex_data_mat = v130;

            v99->TranslateMat(v130, rot_clamped, parent_scale.x);
        }

        if (nodes.size() && end_node.bone_node_mat) {
            mat4 mat = *nodes.back().bone_node_ptr->ex_data_mat;
            mat4_mul_translate_x(&mat, end_node.length * parent_scale.x, &mat);
            *end_node.bone_node_ptr->ex_data_mat = mat;
        }
    }
    apply_physics = false;
}
