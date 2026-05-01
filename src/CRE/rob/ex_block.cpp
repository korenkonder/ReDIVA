/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "rob.hpp"
#include "../../KKdLib/key_val.hpp"
#include "../../KKdLib/str_utils.hpp"
#include "../app_system_detail.hpp"
#include "../data.hpp"
#include "../debug_print.hpp"
#include "../gl_state.hpp"
#include "skin_param.hpp"

struct ExpFuncUnaryTbl {
    const char* name;
    float_t(*func)(float_t v1);
};

struct ExpFuncBinaryTbl {
    const char* name;
    float_t(*func)(float_t v1, float_t v2);
};

struct ExpFuncTernaryTbl {
    const char* name;
    float_t(*func)(float_t v1, float_t v2, float_t v3);
};

static const ExpFuncUnaryTbl* ExpFuncUnaryTblFindFunc(std::string& name, const ExpFuncUnaryTbl* array);
static const ExpFuncBinaryTbl* ExpFuncBinaryTblFindFunc(std::string& name, const ExpFuncBinaryTbl* array);
static const ExpFuncTernaryTbl* ExpFuncTernaryTblFindFunc(std::string& name, const ExpFuncTernaryTbl* array);

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
    const float_t gravity, const float_t weight);
static float_t calc_tangent_basis(const vec3& v1, const vec3& v2, const vec3& pos_c,
    const vec2& uv0, const vec2& uv1, const vec2& uv2, vec3& tangent, vec3& binormal, vec3& normal);
static float_t calc_tangent_basis(const vec3& v1, const vec3& v2,
    const float_t t1, const float_t t2, vec3& tangent, vec3& binormal, vec3& normal);
static void calc_tangent_basis_sub(CLOTH_VERTEX* vtx, const CLOTH_VERTEX* vtxL, const CLOTH_VERTEX* vtxR,
    const CLOTH_VERTEX* vtxU, const CLOTH_VERTEX* vtxD);
static void closest_pt_segment_segment(vec3& vec, const vec3& p0, const vec3& q0, const OsageCollision::Work* cls);
static void length_limit(vec3& p, const vec3& r, const float_t rr);
static bool make_axis_matrix(mat4& mat, const vec3& axis,
    const skin_param_hinge* hinge, vec3* rot, const ROTTYPE& rottype);
static void make_direction_matrix(mat4& mat, const vec3& v0, const vec3& v1);
static void modify_cloth_object(obj_mesh* mesh, obj_mesh_vertex_buffer* vb,
    CLOTH_VERTEX* vtxarg, float_t sgn, int32_t num_idx, uint16_t* idxtbl, bool do_ura);

static const ExpFuncUnaryTbl exp_func_unary_tbl[] = {
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

static const ExpFuncBinaryTbl exp_func_binary_tbl[] = {
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

static const ExpFuncTernaryTbl exp_func_ternary_tbl[] = {
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

static bool rob_osage_enable_distance = true;

bool calc_normal_flag = false;
size_t cloth_init_cnt = 0;
int32_t cloth_modify_flag = 0x03;
int32_t dbg_init_flag = 0;
int32_t dbg_osage_line_disp_flag = 0;

#if OPD_PLAY_GEN
extern std::atomic_bool opd_play_gen_run;
#endif

ExNodeBlock::ExNodeBlock() : dst_node(), type(), name(), parent(),
parent_name(), parent_node(), skin_disp(), is_parent(), done(), has_children_node() {

}

ExNodeBlock::~ExNodeBlock() {

}

void ExNodeBlock::CtrlBegin() {
    done = false;
}

void ExNodeBlock::dest() {
    dst_node = 0;
}

void ExNodeBlock::CtrlEnd() {
    done = false;
}

// 0x1405EEBF0
void ExNodeBlock::init_members() {
    dst_node = 0;
    type = EX_NODE_TYPE_NULL;
    name = 0;
    parent = 0;
    skin_disp = 0;
    is_parent = false;
}

// 0x1405EEB80
void ExNodeBlock::set_data(RobNode* node, ExNodeType type,
    const char* name, const RobSkinDisp* skin_disp) {
    init_members();

    dst_node = node;
    this->type = type;
    this->name = name ? name : "(null)";
    parent = 0;
    this->skin_disp = skin_disp;
}

// 0x1405F92B0
void ExNodeBlock::set_name(const char* name) {
    this->name = name;
}

ExNullBlock::ExNullBlock() {
    init();
}

ExNullBlock::~ExNullBlock() {

}

void ExNullBlock::init() {
    dst_node = 0;
    type = EX_NODE_TYPE_NULL;
    name = 0;
    parent = 0;
    skin_disp = 0;
    data = 0;
}

void ExNullBlock::CtrlBegin() {
    done = false;
}

void ExNullBlock::CtrlStep(int32_t stage, bool disable_ex_force) {

}

void ExNullBlock::ctrl() {
    if (!dst_node)
        return;

    mat4 dsp_mat;
    mat4 mat = *parent->no_scale_mat;
    dst_node->transform.CalcMatrixHS(parent->transform.hsc, mat, dsp_mat);
    *dst_node->no_scale_mat = mat;
    *dst_node->mat_ptr = dsp_mat;
}

void ExNullBlock::CtrlOsagePlayData() {
    ctrl();
}

void ExNullBlock::disp(const mat4& mat, render_context* rctx) {

}

void ExNullBlock::disp_debug() {

}

void ExNullBlock::pos_init() {
    ctrl();
}

void ExNullBlock::pos_init_cont() {

}

void ExNullBlock::set_data(const RobSkinDisp* skin_disp,
    const obj_skin_ex_node_constraint* data, const char* name, const bone_database* bone_data) {
    RobNode* node = (RobNode*)skin_disp->get_node(name, bone_data);
    type = EX_NODE_TYPE_NULL;
    dst_node = node;
    this->data = data;
    this->name = node->name;
    this->skin_disp = skin_disp;
}

RobJointNodeDataNormalRef::RobJointNodeDataNormalRef() : set(), n(), u(), d(), l(), r() {

}

bool RobJointNodeDataNormalRef::Check() {
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
void RobJointNodeDataNormalRef::GetMat(mat4* mat) {
    if (!set)
        return;

    vec3 n_trans;
    vec3 u_trans;
    vec3 d_trans;
    vec3 l_trans;
    vec3 r_trans;
    mat4_get_translation(n->dst_node->no_scale_mat, &n_trans);
    mat4_get_translation(u->dst_node->no_scale_mat, &u_trans);
    mat4_get_translation(d->dst_node->no_scale_mat, &d_trans);
    mat4_get_translation(l->dst_node->no_scale_mat, &l_trans);
    mat4_get_translation(r->dst_node->no_scale_mat, &r_trans);

    vec3 z_axis;
    vec3 y_axis;
    vec3 x_axis;
    if (RobJointNodeDataNormalRef::GetAxes(
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
void RobJointNodeDataNormalRef::Load() {
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
    if (RobJointNodeDataNormalRef::GetAxes(
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
bool RobJointNodeDataNormalRef::GetAxes(const vec3& l_trans, const vec3& r_trans,
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

RobJointNodeResetData::RobJointNodeResetData() : length() {

}

RobJointNodeData::RobJointNodeData() : force(), normal_ref() {

}

RobJointNodeData::~RobJointNodeData() {

}

void RobJointNodeData::SetForce(const skin_param_osage_root& skp_root,
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

// 0x140482D00
void RobJointNodeData::reset() {
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
void opd_node_data_pair::set_data(const opd_blend_data* blend_data, const opd_node_data& node_data) {
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

RobJointNode::RobJointNode() : length_back(), length_next(), dst_node(),
dst_node_mat(), distance(), length_dist(), hit() {
    friction = 1.0f;
    force = 1.0f;
    data_ptr = &data;
    opd_data.resize(3);
}

RobJointNode::~RobJointNode() {

}

// 0x140482180
void RobJointNode::CheckFloorCollision(const float_t& floor_height) {
    float_t pos_y = data_ptr->skp_osg_node.coli_r + floor_height;
    if (pos_y <= pos.y)
        return;

    pos.y = pos_y;
    pos = GetPrevNode().pos + vec3::normalize(pos - GetPrevNode().pos) * length_back;
    vec = 0.0f;
    hit += 1.0f;
}

// 0x140482490
void RobJointNode::CheckNodeDistance(const float_t& step, const float_t& hsc) {
    if (step != 1.0f) {
        vec3 d = pos - old_pos;

        float_t dist = vec3::length(d);
        if (dist != 0.0f)
            d *= 1.0f / dist;
        pos = old_pos + d * (step * dist);
    }

    length_limit(pos, GetPrevNode().pos, length_back * hsc);

    if (rob_osage_enable_distance) {
        if (distance)
            length_limit(pos, distance->pos, length_dist);
    }
}

// 0x140482B00
void RobJointNode::reset() {
    length_back = 0.0f;
    pos = 0.0f;
    old_pos = 0.0f;
    vec = 0.0f;
    vel = 0.0f;
    length_next = 0.0f;
    dst_node = 0;
    dst_node_mat = 0;
    distance = 0;
    length_dist = 0.0f;
    rel_pos = 0.0f;
    reset_data.pos = 0.0f;
    reset_data.vec = 0.0f;
    reset_data.rotation = 0.0f;
    reset_data.length = 0.0f;
    hit = 0.0f;
    friction = 1.0f;
    ex_force = 0.0f;
    force = 1.0f;
    data.reset();
    data_ptr = &data;
    opd_data.clear();
    opd_data.resize(3);
    mat = mat4_null;
}

OsageCollision::Work::Work() : type(), radius(), pos(),
vec_center(), vec_center_length(), vec_center_length_squared(), friction() {

}

// 0x140485450
void OsageCollision::Work::update_cls_work(OsageCollision::Work* work,
    const SkinParam::CollisionParam* cls, const mat4* motmat) {
    work->type = SkinParam::CollisionTypeEnd;
    if (!cls || !motmat)
        return;

    for (; cls->type; work++, cls++) {
        work->type = cls->type;
        work->radius = cls->radius;
        mat4_transform_point(&motmat[cls->node_idx[0]], &cls->pos[0], &work->pos[0]);

        switch (work->type) {
        case SkinParam::CollisionTypePlane:
            mat4_transform_vector(&motmat[cls->node_idx[0]], &cls->pos[1], &work->pos[1]);
            break;
        case SkinParam::CollisionTypeCapsule:
        case SkinParam::CollisionTypeEllipse:
            mat4_transform_point(&motmat[cls->node_idx[1]], &cls->pos[1], &work->pos[1]);
            work->vec_center = work->pos[1] - work->pos[0];
            work->vec_center_length_squared = vec3::length_squared(work->vec_center);
            work->vec_center_length = sqrtf(work->vec_center_length_squared);
            if (work->vec_center_length < 0.01f)
                work->type = SkinParam::CollisionTypeBall;
            break;
        }
    }
    work->type = SkinParam::CollisionTypeEnd;
}

// 0x140485410
void OsageCollision::Work::update_cls_work(OsageCollision::Work* work,
    const std::vector<SkinParam::CollisionParam>& cls_list, const mat4* motmat) {
    if (!cls_list.size()) {
        work->type = SkinParam::CollisionTypeEnd;
        work->radius = 0.0f;
        work->pos[0] = 0.0f;
        work->pos[1] = 0.0f;
        work->vec_center = 0.0f;
        work->vec_center_length = 0.0f;
        work->vec_center_length_squared = 0.0f;
        work->friction = 1.0f;
    }
    else
        update_cls_work(work, cls_list.data(), motmat);
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
                node_idx0 = aft_bone_data->get_bone_index(BONE_KIND_CMN, bone0_name);
                if (node_idx0 >= 0 && kv.read("bone.1.name", bone1_name))
                    node_idx1 = aft_bone_data->get_bone_index(BONE_KIND_CMN, bone1_name);
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

CLOTH_VERTEX::CLOTH_VERTEX() : flag(), m(), length_up(),
length_down(), length_left(), length_right(), reset_data() {

}

CLOTH_VERTEX::~CLOTH_VERTEX() {

}

CLOTH::CLOTH() : flag(), width(), height(), wind_dir(),
wet(), use_ex_force(), ex_force(), skin_param_ptr() {
    init();
}

CLOTH::~CLOTH() {

}

void CLOTH::set_spring() {
    spring.clear();
    if (height <= 1)
        return;

    CLOTH_SPRING line = {};
    const size_t width = this->width;
    for (size_t i = 1; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            size_t top = (i - 1) * width + j;
            size_t center = i * width + j;
            line.index0 = top;
            line.index1 = center;
            line.length = vec3::distance(vtxarg.data()[top].pos, vtxarg.data()[center].pos);
            spring.push_back(line);

            if (j < width - 1) {
                size_t left = i * width + j + 1;
                line.index0 = center;
                line.index1= left;
                line.length = vec3::distance(vtxarg.data()[center].pos, vtxarg.data()[left].pos);
                spring.push_back(line);
            }
        }

        if (flag.ring) {
            line.index0 = i * width;
            line.index1 = (i + 1) * width - 1;
            spring.push_back(line);
        }
    }
}

void CLOTH::set_coli_r(float_t value) {
    skin_param_ptr->coli_r = value;
}

void CLOTH::set_friction(float_t value) {
    skin_param_ptr->friction = value;
}

void CLOTH::set_wind_affect(float_t value) {
    skin_param_ptr->wind_afc = value;
}

void CLOTH::set_wind_dir(const vec3& value) {
    wind_dir = value;
}

void CLOTH::set_wetness(float_t value) {
    wet = value;
}

void CLOTH::set_angle_limit(float_t y, float_t z) {
    skin_param_hinge& hinge = skin_param_ptr->hinge;
    hinge.ymax = y;
    hinge.zmax = z;
    hinge.ymin = -y;
    hinge.zmin = -z;
    hinge.limit();
}

CLOTH_VERTEX* CLOTH::get_vertex() {
    return vtxarg.data();
}

void CLOTH::init() {
    flag.exec = 0;
    width = 0;
    height = 0;
    vtxarg.clear();
    wind_dir = 0.0f;
    wet = 0.0f;
    use_ex_force = false;
    ex_force = 0.0f;
    skin_param.reset();
    skin_param_ptr = &skin_param;
}

void CLOTH::disp_debug() {
    if (!dbg_osage_line_disp_flag)
        return;

    const color4u8 color_spring = color_red;

    const CLOTH_VERTEX* vtx = vtxarg.data();
    for (const auto& i : spring)
        dx_draw_line(vtx[i.index0].pos, vtx[i.index1].pos, color_spring);
}

void CLOTH::dest() {
    init();
}

RobCloth::RobCloth() : local_mat(), skin_disp(), root_data(), data(), move_cancel(), osage_reset(),
mesh(), submesh(), vb(), ib(), reset_data_list() {
    dest();
}

RobCloth::~RobCloth() {
    dest();
}

void RobCloth::disp_debug() {
    CLOTH::disp_debug();

    if (!dbg_osage_line_disp_flag)
        return;

    const color4u8 color_tangent = color_green;
    const color4u8 color_binormal = color_cyan;
    const color4u8 color_normal = color_white;

    const CLOTH_VERTEX* vtx = vtxarg.data();
    vtx += width;
    for (size_t i = 1; i < height; i++) {
        for (size_t j = 0; j < width; j++, vtx++) {
            dx_draw_line(vtx->pos, vtx->tangent * 0.05f + vtx->pos, color_tangent);
            dx_draw_line(vtx->pos, vtx->binormal * 0.05f + vtx->pos, color_binormal);
            dx_draw_line(vtx->pos, vtx->normal * 0.05f + vtx->pos, color_normal);
        }
    }
}

void RobCloth::dest() {
    vb[0].unload();
    vb[1].unload();
    init();
    local_mat = 0;
    skin_disp = 0;
    root_data = 0;
    data = 0;
    move_cancel = 0.0f;
    osage_reset = false;
    mesh[0] = {};
    mesh[1] = {};
    ib[0] = {};
    ib[1] = {};
    motion_reset_data.clear();
    reset_data_list = 0;
}

void RobCloth::AddMotionResetData(uint32_t motnum, float_t frame) {
    int32_t frame_int = (int32_t)prj::roundf(frame * 1000.0f);

    auto elem = motion_reset_data.find({ motnum, frame_int });
    if (elem != motion_reset_data.end())
        motion_reset_data.erase(elem);

    std::list<RobJointNodeResetData> reset_data_list;
    CLOTH_VERTEX* i_begin = vtxarg.data() + width;
    CLOTH_VERTEX* i_end = vtxarg.data() + vtxarg.size();
    for (CLOTH_VERTEX* i = i_begin; i != i_end; i++)
        reset_data_list.push_back(i->reset_data);

    motion_reset_data.insert({ { motnum, frame_int }, reset_data_list });
}

// 0x1402196D0
void RobCloth::ApplyResetData() {
    const size_t width = this->width;
    const size_t height = this->height;

    if (reset_data_list) {
        auto reset_data = this->reset_data_list->begin();
        CLOTH_VERTEX* i_begin = vtxarg.data() + width;
        CLOTH_VERTEX* i_end = vtxarg.data() + vtxarg.size();
        for (CLOTH_VERTEX* i = i_begin; i != i_end; i++)
            i->reset_data = *reset_data++;
        reset_data_list = 0;
    }

    CLOTH_WEIGHTED_ROOT* root = this->root.data();
    for (size_t i = 0; i < width; i++, root++) {
        CLOTH_VERTEX* vtx = &vtxarg.data()[i + width];
        for (size_t j = 1; j < height; j++, vtx += width) {
            mat4_transform_point(&root->mat_pos, &vtx->reset_data.pos, &vtx->pos);
            mat4_transform_vector(&root->mat_pos, &vtx->reset_data.vec, &vtx->vec);
        }
    }
}

// 0x140218E40
void RobCloth::CtrlOsagePlayData(const std::vector<opd_blend_data>& opd_blend_data) {
    calc_root();

    const ::opd_blend_data* i_begin = opd_blend_data.data() + opd_blend_data.size();
    const ::opd_blend_data* i_end = opd_blend_data.data();
    for (const ::opd_blend_data* i = i_begin; i != i_end; ) {
        i--;

        float_t frame = i->frame;
#if OPD_PLAY_GEN
        if (frame >= i->frame_max)
            frame = i->no_loop ? i->frame_max - 1.0f : 0.0f;
#else
        if (frame >= i->frame_max)
            frame = 0.0f;
#endif

        int32_t curr_key = (int32_t)(int64_t)prj::floorf(frame);
        int32_t next_key = curr_key + 1;
#if OPD_PLAY_GEN
        if ((float_t)next_key >= i->frame_max)
            next_key = i->no_loop ? (int32_t)i->frame_max - 1 : 0;
#else
        if ((float_t)next_key >= i->frame_max)
            next_key = 0;
#endif

        float_t blend = frame - (float_t)(int64_t)frame;
        float_t inv_blend = 1.0f - blend;

        for (size_t j = 0; j < width; j++) {
            CLOTH_VERTEX& root_node = vtxarg.data()[j];

            vec3 parent_trans = root_node.pos;
            mat4 mat = root.data()[j].mat;
            mat4_mul_translate(&mat, &root_node.org_pos, &mat);

            CLOTH_VERTEX* v29 = &vtxarg.data()[j + width];

            vec3 localvec;
            mat4_transform_vector(&mat, &v29->localvec, &localvec);

            ROTTYPE rottype = ROTTYPE_YZ;
            make_axis_matrix(mat, localvec, 0, 0, rottype);

            const mat4& mat_pos = root.data()[j].mat_pos;
            for (size_t k = 1; k < height; k++, v29 += width) {
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

                vec3 localvec;
                mat4_inverse_transform_point(&mat, &_trans, &localvec);

                vec3 rotation = 0.0f;
                ROTTYPE rottype = ROTTYPE_YZ;
                make_axis_matrix(mat, localvec, 0, &rotation, rottype);

                mat4_mul_translate(&mat, vec3::distance(_trans, parent_trans), 0.0f, 0.0f, &mat);

                v29->opd_node_data.set_data(i, { v29->length_up, rotation });

                parent_trans = _trans;
            }
        }
    }

    for (size_t i = 0; i < width; i++) {
        CLOTH_VERTEX& root_node = vtxarg.data()[i];
        mat4 mat = root.data()[i].mat;
        mat4_mul_translate(&mat, &root_node.org_pos, &mat);

        CLOTH_VERTEX* v50 = &vtxarg.data()[i + width];

        vec3 localvec;
        mat4_transform_vector(&mat, &v50->localvec, &localvec);

        ROTTYPE rottype = ROTTYPE_YZ;
        make_axis_matrix(mat, localvec, 0, 0, rottype);

        for (size_t j = 1; j < height; j++, v50 += width) {
            mat4_mul_rotate_z(&mat, v50->opd_node_data.curr.rotation.z, &mat);
            mat4_mul_rotate_y(&mat, v50->opd_node_data.curr.rotation.y, &mat);
            mat4_mul_translate(&mat, v50->opd_node_data.curr.length, 0.0f, 0.0f, &mat);
            mat4_get_translation(&mat, &v50->pos);
        }
    }
}

const float_t* RobCloth::LoadOpdData(size_t node_index, const float_t* opd_data, size_t opd_count) {
    CLOTH_VERTEX* i_begin = vtxarg.data() + width;
    CLOTH_VERTEX* i_end = vtxarg.data() + vtxarg.size();
    for (CLOTH_VERTEX* i = i_begin; i != i_end; i++) {
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

void RobCloth::SetMotionResetData(uint32_t motnum, float_t frame) {
    osage_reset = true;
    auto elem = motion_reset_data.find({ motnum, (int32_t)prj::roundf(frame * 1000.0f) });
    if (elem != motion_reset_data.end() && elem->second.size() + width == vtxarg.size()) {
        this->reset_data_list = &elem->second;

        auto reset_data_list = this->reset_data_list->begin();
        CLOTH_VERTEX* i_begin = vtxarg.data() + width;
        CLOTH_VERTEX* i_end = vtxarg.data() + vtxarg.size();
        for (CLOTH_VERTEX* i = i_begin; i != i_end; i++)
            i->reset_data = *reset_data_list++;
    }
}

const float_t* RobCloth::SetOsagePlayDataInit(const float_t* opdi_data) {
    CLOTH_VERTEX* i_begin = vtxarg.data() + width;
    CLOTH_VERTEX* i_end = vtxarg.data() + vtxarg.size();
    for (CLOTH_VERTEX* i = i_begin; i != i_end; i++) {
        i->pos.x = *opdi_data++;
        i->pos.y = *opdi_data++;
        i->pos.z = *opdi_data++;
        i->vec.x = *opdi_data++;
        i->vec.y = *opdi_data++;
        i->vec.z = *opdi_data++;
        i->old_pos = i->pos;
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
    set_param(skp_root.force, skp_root.force_gain, skp_root.air_res);
    set_friction(skp_root.friction);
    set_wind_affect(skp_root.wind_afc);
    set_coli_r(skp_root.coli_r);
    set_angle_limit(skp_root.hinge_y, skp_root.hinge_z);
    skin_param_ptr->coli.assign(skp_root.coli.begin(), skp_root.coli.end());
}

// 0x14021DC60
void RobCloth::calc(const float_t dt) {
    if (dt <= 0.0f)
        return;

    calc_root();
    calc_force(dt, true);
    calc_stretch();
    calc_velocity(dt, true);
}

// 0x1402187D0
void RobCloth::calc_force(float_t time, bool init_flag) {
    float_t fric = (1.0f - wet) * (1.0f - skin_param_ptr->air_res);

    float_t osage_gravity = get_gravity();

    vec3 ex_force = wind_dir * skin_param_ptr->wind_afc;
    if (use_ex_force) {
        ex_force += this->ex_force;
        osage_gravity = 0.0f;
    }

    const size_t width = this->width;
    const size_t height = this->height;

    float_t force = skin_param_ptr->force;
    CLOTH_VERTEX* vtx = &vtxarg.data()[width];
    for (size_t i = 1; i < height; i++) {
        CLOTH_WEIGHTED_ROOT* root = this->root.data();
        for (size_t j = 0; j < width; j++, root++, vtx++) {
            mat4 mat = root->mat;

            vec3 localvec;
            mat4_transform_vector(&mat, &vtx->localvec, &localvec);

            const float_t _fric = dbg_init_flag || init_flag ? (vtx->vec.y >= 0.0f ? 1.0f : 0.0f) : fric;
            vec3 vel = localvec * force - vtx->vec * _fric + ex_force;
            vel.y -= osage_gravity;
            vtx->vec += vel;

            vtx->old_pos = vtx->pos;
            vtx->pos += vtx->vec;
        }
        force *= skin_param_ptr->force_gain;
    }
}

// 0x140218B40
void RobCloth::calc_normal() {
    const ssize_t width = this->width;
    CLOTH_VERTEX* vtx = &vtxarg.data()[width];
    if (calc_normal_flag) {
        for (size_t i = 1; i < height - 1; i++) {
            calc_tangent_basis_sub(vtx, vtx, vtx + 1, &vtx[-width], &vtx[width]);
            vtx++;
            for (ssize_t j = 1; j < width - 1; j++, vtx++)
                calc_tangent_basis_sub(vtx, vtx - 1, vtx + 1, &vtx[-width], &vtx[width]);
            calc_tangent_basis_sub(vtx, vtx - 1, vtx, &vtx[-width], &vtx[width]);
            vtx++;
        }

        calc_tangent_basis_sub(vtx, vtx, vtx + 1, &vtx[-width], vtx);
        vtx++;

        for (ssize_t i = 1; i < width - 1; i++, vtx++)
            calc_tangent_basis_sub(vtx, vtx - 1, vtx + 1, &vtx[-width], vtx);
        calc_tangent_basis_sub(vtx, vtx - 1, vtx, &vtx[-width], vtx);
    }
    else
        for (size_t i = 1; i < height; i++, vtx++) {
            for (ssize_t j = 0; j < width - 1; j++, vtx++) {
                vtx[0].m = calc_tangent_basis(
                    vtx[0].pos, vtx[-width].pos, vtx[1].pos,
                    vtx[0].uv, vtx[-width].uv, vtx[1].uv,
                    vtx[0].tangent, vtx[0].binormal, vtx[0].normal);
            }
            vtx[0].normal = vtx[-1].normal;
            vtx[0].tangent = vtx[-1].tangent;
            vtx[0].binormal = vtx[-1].binormal;
            vtx[0].m = vtx[-1].m;
        }
}

// 0x140219940
void RobCloth::calc_root() {
    for (size_t i = 0; i < width; i++) {
        CLOTH_WEIGHTED_ROOT& root = this->root.data()[i];
        CLOTH_VERTEX& root_node = vtxarg.data()[i];

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

        mat4_transform_point(&m, &root_node.org_pos, &root_node.pos);
        mat4_transform_vector(&m, &root.normal, &root_node.normal);
        mat4_transform_vector(&m, (vec3*)&root.tangent, &root_node.tangent);
        root_node.m = root.tangent.w;
        root_node.old_pos = root_node.pos;

        mat4_mul_translate(&m, &root_node.org_pos, &m);
        root.mat_pos = m;
        mat4_invert(&m, &m);
        root.inv_mat_pos = m;
    }
}

// 0x140219D10
void RobCloth::calc_stretch() {
    CLOTH_VERTEX* vtx = vtxarg.data();
    const ssize_t width = this->width;
    const size_t height = this->height;

    for (size_t i = height - 2; i; i--) {
        CLOTH_VERTEX* v7 = &vtx[width * i];
        for (ssize_t j = 0; j < width; j++, v7++)
            length_limit(v7->pos, v7[width].pos, v7->length_down);
    }

    if (height <= 1)
        return;

    vec3* v11 = (vec3*)operator new(sizeof(vec3) * width);
    vec3* v12 = (vec3*)operator new(sizeof(vec3) * width);
    memset(v11, 0, sizeof(vec3) * width);
    memset(v12, 0, sizeof(vec3) * width);

    vec3* v13 = &v11[width - 2];
    vec3* v14 = v12 + 1;
    CLOTH_VERTEX* v15 = &vtx[width];
    CLOTH_VERTEX* v16 = &vtx[2 * width - 1];
    for (size_t i = height - 1; i; i--) {
        if (width) {
            vec3* v17 = v12;
            vec3* v17a = v11;
            CLOTH_VERTEX* v18 = v15;
            for (ssize_t j = width; j > 0; j--, v17++, v17a++, v18++) {
                *v17 = v18->pos;
                *v17a = v18->pos;
            }
        }

        if (width - 2 >= 0) {
            vec3* v20 = v13;
            CLOTH_VERTEX* v22 = v16 - 1;
            for (ssize_t j = width - 1; j > 0; j--, v20--, v22--)
                length_limit(v20[0], v20[1], v22->length_right);
            v14 = v12 + 1;
        }

        if (flag.ring)
            length_limit(v11[width - 1], v11[0], v16->length_left);

        if (width > 1) {
            vec3* v23 = v14;
            CLOTH_VERTEX* v24 = v15 + 1;
            for (ssize_t j = width - 1; j > 0; j--, v23++, v24++)
                length_limit(v23[0], v23[-1], v24->length_left);
            v14 = v12 + 1;
        }

        if (flag.ring)
            length_limit(v12[0], v12[width - 1], v15->length_right);

        if (width > 0) {
            vec3* v27 = v11;
            vec3* v28 = v12;
            CLOTH_VERTEX* v36 = v15;
            for (ssize_t j = width; j > 0; j--, v27++, v28++, v36++)
                v36->pos = (*v27 + *v28) * 0.5f;
        }
        v16 += width;
        v15 += width;
    }

    operator delete(v11);
    operator delete(v12);
}

// 0x14021AA60
void RobCloth::calc_velocity(const float_t dt, bool a3) {
    const ssize_t width = this->width;
    const size_t height = this->height;

    const float_t inv_dt = 1.0f / dt;

    const CLOTH_WEIGHTED_ROOT* root = this->root.data();
    CLOTH_VERTEX* vtx = &vtxarg.data()[width];
    const float_t floor_height = ring.get_floor_height(vtx->pos, skin_param_ptr->coli_r);

    for (size_t i = 1; i < height; i++) {
        for (ssize_t j = 0; j < width; j++, vtx++) {
            float_t fric = (1.0f - wet) * skin_param_ptr->friction;
            if (dt != 1.0f) {
                vec3 vec = vtx->pos - vtx->old_pos;

                float_t trans_length = vec3::length(vec);
                if (trans_length * dt > 0.0f && trans_length != 0.0f)
                    vec *= 1.0f / trans_length;

                vtx->pos = vtx->old_pos + vec * (trans_length * dt);
            }

            length_limit(vtx[0].pos, vtx[-width].pos, vtx[0].length_up);

            int32_t hit = OsageCollision::osage_cls_work_list(vtx->pos,
                skin_param_ptr->coli_r, ring.coli_object, &fric);
            hit += OsageCollision::osage_cls(coli_ring, vtx->pos, skin_param_ptr->coli_r);
            hit += OsageCollision::osage_cls(coli_chara, vtx->pos, skin_param_ptr->coli_r);

            if (floor_height > vtx->pos.y && floor_height < 1001.0f) {
                vtx->pos.y = floor_height;
                vtx->vec = 0.0f;
            }

            mat4 mat = root->mat;
            mat4_set_translation(&mat, &vtx[-width].pos);

            ROTTYPE rottype = ROTTYPE_ZY;
            make_axis_matrix(mat, vtx->localvec, 0, 0, rottype);

            vec3 localvec;
            mat4_inverse_transform_point(&mat, &vtx->pos, &localvec);

            make_axis_matrix(mat, localvec, &skin_param_ptr->hinge,
                &vtx->reset_data.rotation, rottype);
            mat4_mul_translate(&mat, vtx->length_up, 0.0f, 0.0f, &mat);
            mat4_get_translation(&mat, &vtx->pos);

            if (hit)
                vtx->vec *= fric;

            vtx->vec = (vtx->pos - vtx->old_pos) * inv_dt;

            if (!a3) {
                const mat4& inv_mat_pos = this->root.data()[j].inv_mat_pos;
                mat4_transform_point(&inv_mat_pos, &vtx->pos, &vtx->reset_data.pos);
                mat4_transform_vector(&inv_mat_pos, &vtx->vec, &vtx->reset_data.vec);
            }
        }
    }
}

// 0x140218560
void RobCloth::ctrl(const float_t dt, bool init_flag) {
    calc_root();

    if (osage_reset) {
        ApplyResetData();
        osage_reset = false;
    }

    if (move_cancel > 0.0f) {
        const size_t width = this->width;
        const size_t height = this->height;

        float_t move_cancel = this->move_cancel;
        for (size_t i = 0; i < width; i++) {
            const mat4& mat_pos = root.data()[i].mat_pos;
            CLOTH_VERTEX* vtx = &vtxarg.data()[i + width];
            for (size_t j = 1; j < height; j++, vtx += width) {
                vec3 pos;
                mat4_transform_point(&mat_pos, &vtx->reset_data.pos, &pos);
                vtx->pos += (pos - vtx->pos) * move_cancel;
            }
        }
    }

    if (dt > 0.0f && !get_pause()) {
        calc_force(dt, init_flag);
        calc_stretch();
        calc_velocity(dt, false);

        if (!init_flag) {
            use_ex_force = false;
            ex_force = 0.0f;
        }
    }
}

// 0x14021B930
void RobCloth::disp(const mat4& mat, render_context* rctx) {
    obj* obj = objset_info_storage_get_obj(skin_disp->obj_uid);
    if (!obj)
        return;

    std::vector<GLuint>* textures = objset_info_storage_get_obj_set_gentex(skin_disp->obj_uid.set_id);

    vec3 center = (vtxarg.data()[0].pos + vtxarg.data()[width * height - 1].pos) * 0.5f;

    ::obj o = *obj;
    o.num_mesh = ib[1].buffer != 0 ? 2 : 1;
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

    rctx->disp_manager->set_texture_pattern((int32_t)skin_disp->texchg_vec.size(), skin_disp->texchg_vec.data());
    rctx->disp_manager->entry_obj_by_obj(mat, &o, textures, vb, ib, 0, skin_disp->alpha);
    rctx->disp_manager->set_texture_pattern();
}

// 0x14021D330
void RobCloth::modify_obj() {
    if (pv_osage_manager_array_get_disp())
        return;

    if (cloth_modify_flag & 0x01)
        calc_normal();

    if (cloth_modify_flag & 0x02) {
        obj_mesh* omote_mesh = objset_info_storage_get_obj_mesh(skin_disp->obj_uid, data->omote_name);
        obj_mesh* ura_mesh = objset_info_storage_get_obj_mesh(skin_disp->obj_uid, data->ura_name);
        if (data->ura_name) {
            modify_cloth_object(omote_mesh, &vb[0], vtxarg.data(),
                1.0f, data->num_omote_index, data->omote_index_array, false);
            modify_cloth_object(ura_mesh, &vb[1], vtxarg.data(),
                -1.0f, data->num_ura_index, data->ura_index_array, false);
        }
        else
            modify_cloth_object(omote_mesh, &vb[0], vtxarg.data(),
                1.0, data->num_omote_index, data->omote_index_array, true);
    }
}

// 0x14021E460
void RobCloth::set_data(size_t w, size_t h, const obj_skin_ex_node_cloth_root * rt_data,
    const obj_skin_ex_node_cloth_point* move, const mat4* lcl_mat, uint32_t ring_flag,
    const RobSkinDisp* skin, const bone_database* bone_data) {
    skin_disp = skin;

    obj* obj = objset_info_storage_get_obj(skin_disp->obj_uid);
    if (!obj)
        return;

    obj_mesh_index_buffer* ib = objset_info_storage_get_obj_mesh_index_buffer(skin_disp->obj_uid);
    uint32_t omotea_mesh_index = obj->get_obj_mesh_index(data->omote_name);
    uint32_t ura_mesh_index = obj->get_obj_mesh_index(data->ura_name);

    mesh[0] = obj->mesh_array[omotea_mesh_index];
    if (data->ura_name && ura_mesh_index != -1)
        mesh[1] = obj->mesh_array[ura_mesh_index];
    else
        mesh[1] = {};

    for (int32_t i = 0; i < 2; i++) {
        for (int32_t j = 0; j < mesh[i].num_submesh; j++) {
            obj_sub_mesh& cloth_mesh = submesh[i][j];
            cloth_mesh = mesh[i].submesh_array[j];
            cloth_mesh.attrib.m.cloth = 0;
            cloth_mesh.bounding_sphere.radius = 1000.0f;
            cloth_mesh.axis_aligned_bounding_box.center = 0.0f;
            cloth_mesh.axis_aligned_bounding_box.size = { 1000.0f, 1000.0f, 1000.0f };
            mesh[i].submesh_array[j].attrib.m.cloth = 1;
        }
        mesh[i].submesh_array = submesh[i];
        mesh[i].bounding_sphere.radius = 1000.0f;
    }

    this->ib[0] = ib[omotea_mesh_index];
    if (!vb[0].load(mesh[0], GL::BUFFER_USAGE_STREAM))
        return;

    this->ib[1] = {};
    if (data->ura_name && ura_mesh_index != -1)
        this->ib[1] = ib[ura_mesh_index];

    if (data->ura_name && !vb[1].load(mesh[1], GL::BUFFER_USAGE_STREAM))
        return;

    flag.exec = 1;
    flag.disp = 1;
    flag.ring = ring_flag;

    this->width = w;
    this->height = h;
    this->root_data = rt_data;
    this->local_mat = lcl_mat;

    this->root.clear();
    this->root.reserve(w);

    for (size_t i = 0; i < w; i++) {
        this->root.push_back({});
        CLOTH_WEIGHTED_ROOT& root = this->root.back();
        root.pos = rt_data[i].pos;
        root.normal = rt_data[i].normal;
        for (int32_t j = 0; j < 4; j++) {
            const obj_skin_ex_node_cloth_weight& weight = rt_data[i].weight[j];
            root.node[j] = 0;
            root.node_mat[j] = 0;
            root.bone_mat[j] = &lcl_mat[weight.mat_idx];
            if (weight.bone_name) {
                root.node[j] = skin->get_node(weight.bone_name, bone_data);
                if (root.node[j])
                    root.node_mat[j] = root.node[j]->mat_ptr;
            }
            root.weight[j] = weight.weight;
        }
    }

    this->vtxarg.clear();
    this->vtxarg.reserve(w);

    for (size_t i = 0; i < w; i++) {
        this->vtxarg.push_back({});
        CLOTH_VERTEX& vtx = this->vtxarg.back();
        vtx.pos = root[i].pos;
        vtx.org_pos = root[i].pos;
    }

    std::vector<size_t> index_array(w * h, 0);
    uint16_t* omote_index_array = data->omote_index_array;
    for (size_t i = 0; i < data->num_omote_index; i++)
        index_array.data()[omote_index_array[i]] = i;

    obj_mesh* mesh = objset_info_storage_get_obj_mesh(skin_disp->obj_uid, data->omote_name);
    obj_vertex_format vertex_format = (obj_vertex_format)0;
    obj_vertex_data* vertex_array = 0;
    if (mesh) {
        vertex_format = mesh->vertex_format;
        vertex_array = mesh->vertex_array;
    }

    if (mesh && vertex_format
        & (OBJ_VERTEX_TEXCOORD0 | OBJ_VERTEX_TANGENT | OBJ_VERTEX_NORMAL | OBJ_VERTEX_POSITION)) {
        for (size_t i = 0; i < w; ++i)
            this->root.data()[i].tangent = vertex_array[index_array.data()[i]].tangent;
    }

    for (size_t i = 1; i < h; i++)
        for (size_t j = 0; j < w; j++) {
            this->vtxarg.push_back({});
            const obj_skin_ex_node_cloth_point& mv = move[(i - 1) * w + j];
            CLOTH_VERTEX& vtx = this->vtxarg.back();
            vtx.pos = mv.pos;
            vtx.org_pos = mv.pos;
            vtx.normal = { 0.0f, 0.0f, 1.0f };
            vtx.tangent = { 1.0f, 0.0f, 0.0f };
            vtx.vec = mv.vec;
            vtx.localvec = vec3::normalize(mv.vec);
            vtx.length_up = mv.length[0];
            vtx.length_down = mv.length[1];
            vtx.length_left = mv.length[2];
            vtx.length_right = mv.length[3];
            vtx.reset_data = {};

            uint32_t flag = 0x00;
            if (i)
                flag |= 0x01;
            if (i != h - 1)
                flag |= 0x02;
            if (j)
                flag |= 0x04;
            if (j != w - 1)
                flag |= 0x08;
            vtx.flag = flag;

            if (mesh && vertex_format & OBJ_VERTEX_TEXCOORD0)
                vtx.uv = vertex_array[index_array.data()[i * w + j]].texcoord0;
        }

    set_spring();
}

// 0x14021EEB0
void RobCloth::set_data(const obj_skin_ex_node_cloth* cldata,
    const RobSkinDisp* skin, const bone_database* bone_data) {
    dest();
    data = cldata;
    set_data(cldata->width, cldata->height, cldata->fix_point,
        cldata->move_point, cldata->mat_array, cldata->ring_flag, skin, bone_data);

    obj* obj = objset_info_storage_get_obj(skin->obj_uid);
    if (!obj)
        return;

    for (int32_t i = 0; i < 2; i++) {
        if (!mesh[i].num_vertex || !mesh[i].vertex_array || !ib[i].buffer)
            continue;

        obj_mesh_vertex_buffer* obj_vert_buf = &vb[i];
        obj_mesh_index_buffer* obj_index_buf = &ib[i];

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

// 0x14021EF20
void RobCloth::set_ex_force(const vec3& f) {
    use_ex_force = true;
    ex_force = f;
}

// 0x14021EF50
void RobCloth::set_move_cancel(const float_t& mv_ccl) {
    move_cancel = mv_ccl;
}

// 0x14021FD00
void RobCloth::make_osage_coli(const mat4* motmat) {
    if (skin_param_ptr->coli.size())
        OsageCollision::Work::update_cls_work(coli_chara, skin_param_ptr->coli.data(), motmat);
    OsageCollision::Work::update_cls_work(coli_ring, ring.skp_root_coli, motmat);
}

// 0x14021D480
void RobCloth::pos_init() {
    calc_root();

    const float_t gravity = get_gravity();

    const ssize_t width = this->width;
    const size_t height = this->height;

    CLOTH_VERTEX* vtx = &vtxarg.data()[width];
    const float_t floor_height = ring.get_floor_height(vtx->pos, skin_param_ptr->coli_r);

    for (size_t i = 1; i < height; i++) {
        CLOTH_WEIGHTED_ROOT* root = this->root.data();
        for (ssize_t j = 0; j < width; j++, root++, vtx++) {
            mat4 mat = root->mat;

            vec3 v38;
            mat4_transform_vector(&mat, &vtx->localvec, &v38);
            v38.y -= gravity;

            vtx[0].pos = vtx[-width].pos + vec3::normalize(v38) * vtx->length_up;

            OsageCollision::osage_cls_work_list(vtx->pos, skin_param_ptr->coli_r, ring.coli_object);
            OsageCollision::osage_cls(coli_ring, vtx->pos, skin_param_ptr->coli_r);
            OsageCollision::osage_cls(coli_chara, vtx->pos, skin_param_ptr->coli_r);

            if (floor_height > vtx->pos.y && floor_height < 1001.0f)
                vtx->pos.y = floor_height;

            vtx->vec = 0.0f;
            vtx->old_pos = vtx->pos;
        }
    }

    for (size_t i = 0; i < cloth_init_cnt; ++i)
        calc(1.0f);

    vtx = &vtxarg.data()[width];
    for (size_t i = 1; i < height; i++)
        for (ssize_t j = 0; j < width; j++, vtx++)
            vtx->vec = 0.0f;
}

// 0x14021D840
void RobCloth::pos_init_cont() {
    ctrl(1.0f, true);
}

void RobCloth::reset_ex_force() {
    use_ex_force = false;
    ex_force = 0.0f;
}

void RobCloth::set_param(float_t force, float_t force_gain, float_t air_res) {
    skin_param_ptr->force = force;
    skin_param_ptr->force_gain = force_gain;
    skin_param_ptr->air_res = air_res;
}

ExClothBlock::ExClothBlock() {
    init();
}

ExClothBlock::~ExClothBlock() {

}

void ExClothBlock::init() {
    cloth_work.init();
    data = 0;
    motion_matrix = 0;
    block_idx = 0;
}

void ExClothBlock::CtrlBegin() {
    done = false;
}

void ExClothBlock::CtrlStep(int32_t stage, bool disable_ex_force) {

}

void ExClothBlock::ctrl() {
    set_param();

    const RobDisp* rob_disp = skin_disp->rob_disp;
    float_t delta_frame = get_delta_frame();
#if OPD_PLAY_GEN
    if (opd_play_gen_run)
        delta_frame = 1.0f;
#endif
    float_t step = delta_frame * rob_disp->osage_step;
    if (rob_disp->opd_blend_data.size() && rob_disp->opd_blend_data.front().use_blend)
        step = 1.0f;
    cloth_work.ctrl(step, false);
}

void ExClothBlock::CtrlOsagePlayData() {
    cloth_work.CtrlOsagePlayData(skin_disp->rob_disp->opd_blend_data);
}

void ExClothBlock::disp(const mat4& mat, render_context* rctx) {
    cloth_work.modify_obj();
    cloth_work.disp(mat, rctx);
}

void ExClothBlock::dest() {
    cloth_work.dest();
    dst_node = 0;
}

void ExClothBlock::disp_debug() {

}

void ExClothBlock::pos_init() {
    set_param();
    cloth_work.pos_init();
}

void ExClothBlock::pos_init_cont() {
    set_param();
    cloth_work.pos_init_cont();
}

void ExClothBlock::AddMotionResetData(const uint32_t& motnum, const float_t& frame) {
    cloth_work.AddMotionResetData(motnum, frame);
}

const float_t* ExClothBlock::LoadOpdData(size_t node_index, const float_t* opd_data, size_t opd_count) {
    return cloth_work.LoadOpdData(node_index, opd_data, opd_count);
}

void ExClothBlock::SetMotionResetData(const uint32_t& motnum, const float_t& frame) {
    cloth_work.SetMotionResetData(motnum, frame);
}

const float_t* ExClothBlock::SetOsagePlayDataInit(const float_t* opdi_data) {
    return cloth_work.SetOsagePlayDataInit(opdi_data);
}

void ExClothBlock::SetOsageReset() {
    cloth_work.SetOsageReset();
}

void ExClothBlock::SetRing(const osage_ring_data& ring) {
    cloth_work.SetRing(ring);
}

void ExClothBlock::SetSkinParam(skin_param_file_data* skp) {
    cloth_work.SetSkinParam(skp);
}

void ExClothBlock::SetSkinParamOsageRoot(const skin_param_osage_root* skp_root) {
    if (!skp_root)
        return;

    cloth_work.SetSkinParamOsageRoot(*skp_root);

    vec3 wind_dir = task_wind->stage_wind.wind->val * skin_disp->rob_disp->wind_strength;
    cloth_work.set_wind_dir(wind_dir);
}

// 0x1405F5220
void ExClothBlock::reset_ex_force() {
    cloth_work.reset_ex_force();
}

void ExClothBlock::set_data(const RobSkinDisp* skin_disp,
    const obj_skin_ex_node_cloth* data, const skin_param_osage_root* skp_root, const bone_database* bone_data) {
    type = EX_NODE_TYPE_CLOTH;
    dst_node = 0;
    this->data = data;
    name = skp_root ? skp_root->name : 0;
    this->skin_disp = skin_disp;

    motion_matrix = skin_disp->get_node(0)->mat_ptr;
    cloth_work.dest();
    cloth_work.set_data(data, skin_disp, bone_data);
    SetSkinParamOsageRoot(skp_root);
}

// 0x1405F7EF0
void ExClothBlock::set_ex_force(const vec3& f) {
    cloth_work.set_ex_force(f);
}

// 0x1405F9280
void ExClothBlock::set_move_cancel(const float_t& mv_ccl) {
    cloth_work.set_move_cancel(mv_ccl);
}

void ExClothBlock::set_param() {
    cloth_work.make_osage_coli(motion_matrix);
    cloth_work.set_wind_dir(task_wind->stage_wind.wind->val);
}

RobOsage::RobOsage() : skin_param_ptr(), apply_physics(), field_2A1(), field_2A4(),
wet(), rottype(), root_matrix_ptr(), move_cancel(), move_cancelled(), osage_reset(),
osage_reset_done(), disable_collision(), use_ex_force(), reset_data_list() {

}

RobOsage::~RobOsage() {

}

void RobOsage::AddMotionResetData(const uint32_t& motnum, const float_t& frame) {
    int32_t frame_int = (int32_t)prj::roundf(frame * 1000.0f);

    auto elem = motion_reset_data.find({ motnum, frame_int });
    if (elem != motion_reset_data.end())
        motion_reset_data.erase(elem);

    std::list<RobJointNodeResetData> reset_data_list;
    RobJointNode* i_begin = joint_node_vec.data() + 1;
    RobJointNode* i_end = joint_node_vec.data() + joint_node_vec.size();
    for (RobJointNode* i = i_begin; i != i_end; i++)
        reset_data_list.push_back(i->reset_data);

    motion_reset_data.insert({ { motnum, frame_int }, reset_data_list });
}

// 0x14047D620
void RobOsage::ApplyBocRootColi(const float_t step) {
    if (get_pause() || step <= 0.0f || disable_collision)
        return;

    const vec3 pos = joint_node_vec.data()[0].pos;
    const OsageCollision::Work* coli_chara = this->coli_chara;
    const OsageCollision::Work* coli_ring = this->coli_ring;
    const SkinParam::RootCollisionType coli_type = skin_param_ptr->coli_type;

    RobJointNode* i_begin = joint_node_vec.data() + 1;
    RobJointNode* i_end = joint_node_vec.data() + joint_node_vec.size();
    for (RobJointNode* i = i_begin; i != i_end; i++) {
        skin_param_osage_node* skp_osg_node = &i->data_ptr->skp_osg_node;
        for (RobJointNode*& j : i->data_ptr->boc) {
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
    joint_node_vec.data()[0].pos = pos;
}

// 0x14047C800
void RobOsage::ApplyPhysics(const mat4& root_matrix, const vec3& hsc,
    const float_t step, bool disable_ex_force, bool ring_coli, bool has_children_node) {
    if (!joint_node_vec.size())
        return;

    const float_t gravity = get_gravity();
    BeginCalc(root_matrix, hsc, false);

    RobJointNode* node = &joint_node_vec.data()[0];
    node->old_pos = node->pos;
    const vec3 v113 = transform.pos * hsc;

    mat4 v130 = root_matrix;
    mat4_transform_point(&v130, &v113, &node->pos);
    node->vec = node->pos - node->old_pos;

    RotateMat(v130, hsc);
    *joint_node_vec.data()[0].dst_node_mat = v130;
    *joint_node_vec.data()[0].dst_node->no_scale_mat = v130;

    vec3 direction = { 1.0f, 0.0f, 0.0f };
    mat4_transform_vector(&v130, &direction, &direction);

    if (!osage_reset && (get_pause() || step <= 0.0f))
        return;

    const bool stiffness = skin_param_ptr->stiffness > 0.0f;

    RobJointNode* v26_begin = joint_node_vec.data() + 1;
    RobJointNode* v26_end = joint_node_vec.data() + joint_node_vec.size();
    for (RobJointNode* v26 = v26_begin; v26 != v26_end; v26++) {
        float_t weight = v26->data_ptr->skp_osg_node.weight;
        vec3 force;
        if (!use_ex_force) {
            apply_gravity(force, v26->pos, v26->GetPrevNode().pos, gravity, weight);

            if (v26 != v26_end - 1) {
                vec3 _force;
                apply_gravity(_force, v26->pos, v26->GetNextNode().pos, gravity, weight);
                force = (force + _force) * 0.5f;
            }
        }
        else
            force = ex_force * (1.0f / weight);

        const vec3 _direction = direction * (v26->data_ptr->force * v26->force);
        const float_t fric = (1.0f - wet) * (1.0f - skin_param_ptr->air_res);

        vec3 vel = force + _direction - v26->vec * fric + v26->ex_force * weight;

        if (!disable_ex_force)
            vel += wind_dir * skin_param_ptr->wind_afc;

        if (stiffness)
            vel -= (v26->vec - v26->GetPrevNode().vec) * (1.0f - skin_param_ptr->air_res);

        v26->vel = vel * (1.0f / (weight - (weight - 1.0f) * v26->data_ptr->skp_osg_node.inertial_cancel));
    }

    if (stiffness) {
        mat4 v131 = v130;
        vec3 v111;
        mat4_get_translation(&v131, &v111);

        RobJointNode* v55_begin = joint_node_vec.data() + 1;
        RobJointNode* v55_end = joint_node_vec.data() + joint_node_vec.size();
        for (RobJointNode* v55 = v55_begin; v55 != v55_end; v55++) {
            vec3 v128;
            mat4_transform_point(&v131, &v55->rel_pos, &v128);

            const vec3 vec = v55->vec + v55->vel;
            vec3 v126 = v55->pos + vec;
            length_limit(v126, v111, v55->length_back * hsc.x);

            vec3 v117 = (v128 - v126) * skin_param_ptr->stiffness;
            const float_t weight = v55->data_ptr->skp_osg_node.weight;
            v117 *= 1.0f / (weight - (weight - 1.0f) * v55->data_ptr->skp_osg_node.inertial_cancel);
            v55->vel += v117;

            v126 = v55->pos + vec + v117;

            vec3 direction;
            mat4_inverse_transform_point(&v131, &v126, &direction);

            make_axis_matrix(v131, direction, 0, 0, rottype);

            mat4_mul_translate(&v131, vec3::distance(v111, v126), 0.0f, 0.0f, &v131);

            v111 = v126;
        }
    }

    RobJointNode* v82_begin = joint_node_vec.data() + 1;
    RobJointNode* v82_end = joint_node_vec.data() + joint_node_vec.size();
    for (RobJointNode* v82 = v82_begin; v82 != v82_end; v82++) {
        v82->old_pos = v82->pos;
        v82->vec += v82->vel;
        v82->pos += v82->vec;
    }

    if (joint_node_vec.size() > 1) {
        RobJointNode* v90_begin = joint_node_vec.data() + joint_node_vec.size() - 2;
        RobJointNode* v90_end = joint_node_vec.data();
        for (RobJointNode* v90 = v90_begin; v90 != v90_end; v90--)
            length_limit(v90[0].pos, v90[1].pos, v90->length_next * hsc.x);
    }

    if (ring_coli) {
        RobJointNode* node = &joint_node_vec.data()[0];
        const float_t floor_height = ring.get_floor_height(
            node->pos, node->data_ptr->skp_osg_node.coli_r);

        RobJointNode* v98_begin = joint_node_vec.data() + 1;
        RobJointNode* v98_end = joint_node_vec.data() + joint_node_vec.size();
        for (RobJointNode* v98 = v98_begin; v98 != v98_end; v98++) {
            v98->CheckNodeDistance(step, hsc.x);
            v98->CheckFloorCollision(floor_height);
        }
    }

    if (has_children_node) {
        RobJointNode* v99_begin = joint_node_vec.data() + 1;
        RobJointNode* v99_end = joint_node_vec.data() + joint_node_vec.size();
        for (RobJointNode* v99 = v99_begin; v99 != v99_end; v99++) {
            vec3 direction;
            mat4_inverse_transform_point(&v130, &v99->pos, &direction);

            bool rot_clamped = make_axis_matrix(v130, direction,
                &v99->data_ptr->skp_osg_node.hinge,
                &v99->reset_data.rotation, rottype);
            *v99->dst_node->no_scale_mat = v130;

            v99->TranslateMat(v130, rot_clamped, hsc.x);
        }

        if (joint_node_vec.size() && effector.dst_node_mat) {
            mat4 mat = *joint_node_vec.back().dst_node->no_scale_mat;
            mat4_mul_translate_x(&mat, effector.length_back* hsc.x, &mat);
            *effector.dst_node->no_scale_mat = mat;
        }
    }
    apply_physics = false;
}

// 0x14047EE90
void RobOsage::ApplyResetData(const mat4& mat) {
    if (reset_data_list) {
        auto reset_data = reset_data_list->begin();
        RobJointNode* i_begin = joint_node_vec.data() + 1;
        RobJointNode* i_end = joint_node_vec.data() + joint_node_vec.size();
        for (RobJointNode* i = i_begin; i != i_end; i++)
            i->reset_data = *reset_data++;
        reset_data_list = 0;
    }

    RobJointNode* i_begin = joint_node_vec.data() + 1;
    RobJointNode* i_end = joint_node_vec.data() + joint_node_vec.size();
    for (RobJointNode* i = i_begin; i != i_end; i++) {
        mat4_transform_vector(&mat, &i->reset_data.vec, &i->vec);
        mat4_transform_point(&mat, &i->reset_data.pos, &i->pos);
    }
    root_matrix_prev = *root_matrix_ptr;
}

// 0x1404803B0
void RobOsage::BeginCalc(const mat4& root_matrix, const vec3& hsc, bool has_children_node) {
    mat4 mat = root_matrix;
    const vec3 pos = transform.pos * hsc;
    mat4_transform_point(&mat, &pos, &joint_node_vec.data()[0].pos);

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
            RobJointNode* i_begin = joint_node_vec.data() + 1;
            RobJointNode* i_end = joint_node_vec.data() + joint_node_vec.size();
            for (RobJointNode* i = i_begin; i != i_end; i++) {
                vec3 pos;
                mat4_inverse_transform_point(&root_matrix_prev, &i->pos, &pos);
                mat4_transform_point(root_matrix_ptr, &pos, &pos);
                i->pos += (pos - i->pos) * move_cancel;
            }
        }
    }

    if (!has_children_node)
        return;

    RotateMat(mat, hsc);
    *joint_node_vec.data()[0].dst_node_mat = mat;

    RobJointNode* v30_begin = joint_node_vec.data() + 1;
    RobJointNode* v30_end = joint_node_vec.data() + joint_node_vec.size();
    for (RobJointNode* v30 = v30_begin; v30 != v30_end; v30++) {
        vec3 direction;
        mat4_inverse_transform_point(&mat, &v30->pos, &direction);

        bool rot_clamped = make_axis_matrix(mat, direction,
            &v30->data_ptr->skp_osg_node.hinge,
            &v30->reset_data.rotation, rottype);
        *v30->dst_node->no_scale_mat = mat;

        v30->TranslateMat(mat, rot_clamped, hsc.x);
    }

    if (joint_node_vec.size() && effector.dst_node_mat) {
        mat4 mat = *joint_node_vec.back().dst_node->no_scale_mat;
        mat4_mul_translate_x(&mat, effector.length_back * hsc.x, &mat);
        *effector.dst_node->no_scale_mat = mat;
    }
}

bool RobOsage::CheckPartsBits(const rob_osage_parts_bit& parts_bits) {
    if (osage_setting.parts != ROB_OSAGE_PARTS_NONE)
        return !!(parts_bits & (1 << osage_setting.parts));
    return false;
}

// 0x14047ECA0
void RobOsage::CollideNodes(const float_t step) {
    if (get_pause() || step <= 0.0f)
        return;

    const OsageCollision::Work* coli_chara = this->coli_chara;
    const OsageCollision::Work* coli_ring = this->coli_ring;
    const OsageCollision& coli_object = ring.coli_object;

    RobJointNode* i_begin = joint_node_vec.data() + 1;
    RobJointNode* i_end = joint_node_vec.data() + joint_node_vec.size();
    for (RobJointNode* i = i_begin; i != i_end; i++) {
        skin_param_osage_node* skp_osg_node = &i->data_ptr->skp_osg_node;
        i->hit += (float_t)OsageCollision::osage_cls_work_list(i->pos,
            skp_osg_node->coli_r, coli_object, &i->friction);
        if (disable_collision)
            continue;

        for (RobJointNode*& j : i->data_ptr->boc) {
            j->hit += (float_t)OsageCollision::osage_cls(coli_ring, j->pos, skp_osg_node->coli_r);
            j->hit += (float_t)OsageCollision::osage_cls(coli_chara, j->pos, skp_osg_node->coli_r);
        }

        i->hit += (float_t)OsageCollision::osage_cls(coli_ring, i->pos, skp_osg_node->coli_r);
        i->hit += (float_t)OsageCollision::osage_cls(coli_chara, i->pos, skp_osg_node->coli_r);
    }
}

// 0x14047D8C0
void RobOsage::CollideNodesTargetOsage(const mat4& root_matrix,
    const vec3& hsc, const float_t step, bool collide_nodes) {
    if (!osage_reset && (get_pause() || step <= 0.0f))
        return;

    const OsageCollision::Work* coli_chara = this->coli_chara;
    const OsageCollision::Work* coli_ring = this->coli_ring;
    const OsageCollision& coli_object = ring.coli_object;

    const float_t inv_step = step > 0.0f ? 1.0f / step : 0.0f;

    mat4 v64;
    if (collide_nodes)
        v64 = *joint_node_vec.data()[0].dst_node_mat;
    else {
        v64 = root_matrix;

        const vec3 pos = transform.pos * hsc;
        mat4_transform_point(&v64, &pos, &joint_node_vec.data()[0].pos);
        RotateMat(v64, hsc);
    }

    float_t floor_height = -1000.0f;
    if (collide_nodes) {
        RobJointNode* node = &joint_node_vec.data()[0];
        floor_height = ring.get_floor_height(
            node->pos, node->data_ptr->skp_osg_node.coli_r);
    }

    const float_t v23 = step < 1.0f ? 0.2f / (2.0f - step) : 0.2f;

    if (skin_param_ptr->colli_tgt_osg) {
        std::vector<RobJointNode>* colli_tgt_osg = skin_param_ptr->colli_tgt_osg;
        RobJointNode* i_begin = joint_node_vec.data() + 1;
        RobJointNode* i_end = joint_node_vec.data() + joint_node_vec.size();
        for (RobJointNode* i = i_begin; i != i_end; i++) {
            vec3 v62 = 0.0f;

            RobJointNode* j_begin = colli_tgt_osg->data() + 1;
            RobJointNode* j_end = colli_tgt_osg->data() + colli_tgt_osg->size();
            for (RobJointNode* j = j_begin; j != j_end; j++)
                OsageCollision::cls_ball_oidashi(v62, i->pos, j->pos,
                    i->data_ptr->skp_osg_node.coli_r + j->data_ptr->skp_osg_node.coli_r);
            i->pos += v62;
        }
    }

    RobJointNode* i_begin = joint_node_vec.data() + 1;
    RobJointNode* i_end = joint_node_vec.data() + joint_node_vec.size();
    for (RobJointNode* i = i_begin; i != i_end; i++) {
        const float_t fric = (1.0f - wet) * skin_param_ptr->friction;
        if (collide_nodes) {
            i->CheckNodeDistance(step, hsc.x);
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
            length_limit(i->pos, i->GetPrevNode().pos, i->length_back * hsc.x);

        vec3 direction;
        mat4_inverse_transform_point(&v64, &i->pos, &direction);

        bool rot_clamped = make_axis_matrix(v64, direction,
            &i->data_ptr->skp_osg_node.hinge,
            &i->reset_data.rotation, rottype);
        i->dst_node->transform.hsc = hsc;
        *i->dst_node->no_scale_mat = v64;

        if (i->dst_node_mat)
            mat4_scale_rot(&v64, &hsc, i->dst_node_mat);

        i->reset_data.length = i->TranslateMat(v64, rot_clamped, hsc.x);

        i->vec = (i->pos - i->old_pos) * inv_step;

        if (i->hit > 0.0f)
            i->vec *= min_def(fric, i->friction);

        const float_t v55 = vec3::length_squared(i->vec);
        if (v55 > v23 * v23)
            i->vec *= v23 / sqrtf(v55);

        mat4_inverse_transform_point(&root_matrix, &i->pos, &i->reset_data.pos);
        mat4_inverse_transform_vector(&root_matrix, &i->vec, &i->reset_data.vec);
    }

    if (joint_node_vec.size() && effector.dst_node_mat) {
        mat4 mat = *joint_node_vec.back().dst_node->no_scale_mat;
        mat4_mul_translate_x(&mat, effector.length_back * hsc.x, &mat);
        *effector.dst_node->no_scale_mat = mat;

        mat4_scale_rot(&mat, &hsc, &mat);
        *effector.dst_node_mat = mat;
        effector.dst_node->transform.hsc = hsc;
    }
}

// 0x14047E1C0
void RobOsage::CtrlEnd(const vec3& hsc) {
    RobJointNode* i_begin = joint_node_vec.data() + 1;
    RobJointNode* i_end = joint_node_vec.data() + joint_node_vec.size();
    for (RobJointNode* i = i_begin; i != i_end; i++)
        if (i->data_ptr->normal_ref.set) {
            i->data_ptr->normal_ref.GetMat(i->dst_node_mat);
            mat4_scale_rot(i->dst_node_mat, &hsc, i->dst_node_mat);
        }
}

// 0x14047E240
void RobOsage::CtrlOsagePlayData(const mat4& root_matrix,
    const vec3& hsc, const std::vector<opd_blend_data>& opd_blend_data) {
    if (!opd_blend_data.size())
        return;

    const vec3 v63 = transform.pos * hsc;

    mat4 v85 = root_matrix;
    mat4_transform_point(&v85, &v63, &joint_node_vec.data()[0].pos);

    RotateMat(v85, hsc);
    *joint_node_vec.data()[0].dst_node_mat = v85;
    *joint_node_vec.data()[0].dst_node->no_scale_mat = v85;

    const ::opd_blend_data* i_begin = opd_blend_data.data() + opd_blend_data.size();
    const ::opd_blend_data* i_end = opd_blend_data.data();
    for (const ::opd_blend_data* i = i_begin; i != i_end; ) {
        i--;

        float_t frame = i->frame;
#if OPD_PLAY_GEN
        if (frame >= i->frame_max)
            frame = i->no_loop ? i->frame_max - 1.0f : 0.0f;
#else
        if (frame >= i->frame_max)
            frame = 0.0f;
#endif

        int32_t curr_key = (int32_t)(int64_t)prj::floorf(frame);
        int32_t next_key = curr_key + 1;
#if OPD_PLAY_GEN
        if ((float_t)next_key >= i->frame_max)
            next_key = i->no_loop ? (int32_t)i->frame_max - 1 : 0;
#else
        if ((float_t)next_key >= i->frame_max)
            next_key = 0;
#endif

        float_t blend = frame - (float_t)(int64_t)frame;
        float_t inv_blend = 1.0f - blend;

        mat4 v87 = v85;
        vec3 parent_curr_trans = joint_node_vec.data()[0].pos;
        vec3 parent_next_trans = joint_node_vec.data()[0].pos;

        RobJointNode* j_begin = joint_node_vec.data() + 1;
        RobJointNode* j_end = joint_node_vec.data() + joint_node_vec.size();
        for (RobJointNode* j = j_begin; j != j_end; j++) {
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
            make_axis_matrix(v87, direction, 0, &rotation, rottype);
            mat4_set_translation(&v87, &_trans);

            float_t length = vec3::distance(curr_trans, parent_curr_trans) * inv_blend
                + vec3::distance(next_trans, parent_next_trans) * blend;
            j->opd_node_data.set_data(i, { length, rotation });

            parent_curr_trans = curr_trans;
            parent_next_trans = next_trans;
        }
    }

    RobJointNode* j_begin = joint_node_vec.data() + 1;
    RobJointNode* j_end = joint_node_vec.data() + joint_node_vec.size();
    for (RobJointNode* j = j_begin; j != j_end; j++) {
        float_t rot_y = j->opd_node_data.curr.rotation.y;
        float_t rot_z = j->opd_node_data.curr.rotation.z;
        rot_y = clamp_def(rot_y, (float_t)-M_PI, (float_t)M_PI);
        rot_z = clamp_def(rot_z, (float_t)-M_PI, (float_t)M_PI);
        mat4_mul_rotate_z(&v85, rot_z, &v85);
        mat4_mul_rotate_y(&v85, rot_y, &v85);
        j->dst_node->transform.hsc = hsc;
        *j->dst_node->no_scale_mat = v85;

        if (j->dst_node_mat)
            mat4_scale_rot(&v85, &hsc, j->dst_node_mat);

        mat4_mul_translate(&v85, j->opd_node_data.curr.length, 0.0f, 0.0f, &v85);
        j->old_pos = j->pos;
        mat4_get_translation(&v85, &j->pos);
    }

    if (joint_node_vec.size() && effector.dst_node_mat) {
        mat4 mat = *joint_node_vec.back().dst_node->no_scale_mat;
        mat4_mul_translate_x(&mat, effector.length_back * hsc.x, &mat);
        *effector.dst_node->no_scale_mat = mat;

        mat4_scale_rot(&mat, &hsc, &mat);
        *effector.dst_node_mat = mat;
        effector.dst_node->transform.hsc = hsc;
    }
}

// 0x140480260
void RobOsage::EndCalc(const mat4& root_matrix, const vec3& hsc,
    const float_t step, bool disable_ex_force) {
    if (!disable_ex_force) {
        SetNodesExternalForce(0, 1.0f);
        SetNodesForce(1.0f);
        use_ex_force = false;
        ex_force = 0.0f;
    }

    apply_physics = true;
    field_2A1 = false;
    field_2A4 = -1.0f;
    move_cancelled = false;
    osage_reset = false;
    osage_reset_done = false;

    for (RobJointNode& i : joint_node_vec) {
        i.hit = 0.0f;
        i.friction = 1.0f;
    }

    root_matrix_prev = *root_matrix_ptr;
}

const float_t* RobOsage::LoadOpdData(size_t node_index, const float_t* opd_data, size_t opd_count) {
    RobJointNode* i_begin = joint_node_vec.data() + 1;
    RobJointNode* i_end = joint_node_vec.data() + joint_node_vec.size();
    for (RobJointNode* i = i_begin; i != i_end; i++) {
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
    skin_param_osage_root& skp_root, const object_info& obj_info, const bone_database* bone_data) {
    key_val* _kv = (key_val*)kv;
    skin_param_ptr = &skin_param;
    for (RobJointNode& i : joint_node_vec)
        i.data_ptr = &i.data;

    skin_param_osage_root_parse(_kv, name, skp_root, bone_data);
    SetSkinParamOsageRoot(skp_root);

    const osage_setting_osg_cat* osage_setting = osage_setting_data_get_cat_value(obj_info, name);
    if (osage_setting)
        this->osage_setting = *osage_setting;

    std::vector<skin_param_osage_node> vec;
    vec.resize(joint_node_vec.size() - 1);
    skin_param_osage_node_parse(_kv, name, &vec, skp_root);
    SetSkpOsgNodes(&vec);
}

// 0x14047F110
void RobOsage::RotateMat(mat4& mat, const vec3& hsc, bool init_rot) {
    const vec3 position = transform.pos * hsc;
    mat4_mul_translate(&mat, &position, &mat);
    mat4_mul_rotate_zyx(&mat, &transform.rot, &mat);

    const vec3 rot = init_rot ? skin_param_ptr->init_rot + skin_param_ptr->rot : skin_param_ptr->rot;
    mat4_mul_rotate_zyx(&mat, &rot, &mat);
}

// 0x140481090
void RobOsage::SetDisableCollision(const bool& value) {
    disable_collision = value;
}

void RobOsage::SetMotionResetData(const uint32_t& motnum, const float_t& frame) {
    osage_reset = true;
    auto elem = motion_reset_data.find({ motnum, (int32_t)prj::roundf(frame * 1000.0f) });
    if (elem != motion_reset_data.end() && elem->second.size() + 1 == joint_node_vec.size())
        reset_data_list = &elem->second;
}

// 0x140480F40
void RobOsage::SetNodesExternalForce(const vec3* ex_force, const float_t& gain) {
    if (!ex_force) {
        RobJointNode* i_begin = joint_node_vec.data() + 1;
        RobJointNode* i_end = joint_node_vec.data() + joint_node_vec.size();
        for (RobJointNode* i = i_begin; i != i_end; i++)
            i->ex_force = 0.0f;
        return;
    }

    vec3 _ex_force = *ex_force;
    for (size_t i = osage_setting.exf; i; i--)
        _ex_force *= gain;

    RobJointNode* i_begin = joint_node_vec.data() + 1;
    RobJointNode* i_end = joint_node_vec.data() + joint_node_vec.size();
    for (RobJointNode* i = i_begin; i != i_end; i++) {
        i->ex_force = _ex_force;
        _ex_force *= gain;
    }
}

// 0x140481540
void RobOsage::SetNodesForce(const float_t& force) {
    RobJointNode* i_begin = joint_node_vec.data() + 1;
    RobJointNode* i_end = joint_node_vec.data() + joint_node_vec.size();
    for (RobJointNode* i = i_begin; i != i_end; i++)
        i->force = force;
}

const float_t* RobOsage::SetOsagePlayDataInit(const float_t* opdi_data) {
    RobJointNode* i_begin = joint_node_vec.data() + 1;
    RobJointNode* i_end = joint_node_vec.data() + joint_node_vec.size();
    for (RobJointNode* i = i_begin; i != i_end; i++) {
        i->pos.x = *opdi_data++;
        i->pos.y = *opdi_data++;
        i->pos.z = *opdi_data++;
        i->vec.x = *opdi_data++;
        i->vec.y = *opdi_data++;
        i->vec.z = *opdi_data++;
        i->old_pos = i->pos;
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

void RobOsage::SetSkinParam(skin_param_file_data* skp) {
    if (skp->nodes_data.size() == joint_node_vec.size() - 1) {
        skin_param_ptr = &skp->skin_param;

        size_t node_index = 0;
        RobJointNode* i_begin = joint_node_vec.data() + 1;
        RobJointNode* i_end = joint_node_vec.data() + joint_node_vec.size();
        for (RobJointNode* i = i_begin; i != i_end; i++)
            i->data_ptr = &skp->nodes_data[node_index++];
    }
    else {
        skin_param_ptr = &skin_param;
        for (RobJointNode& i : joint_node_vec)
            i.data_ptr = &i.data;
    }
}

void RobOsage::SetSkinParamOsageRoot(const skin_param_osage_root& skp_root) {
    skin_param_ptr->reset();
    set_force(skp_root.force, skp_root.force_gain);
    set_air_res(skp_root.air_res);
    set_angle(skp_root.rot_y, skp_root.rot_z);
    set_init_angle(skp_root.init_rot_y, skp_root.init_rot_z);
    set_rot_type(skp_root.rottype);
    skin_param_ptr->friction = skp_root.friction;
    skin_param_ptr->wind_afc = skp_root.wind_afc;
    set_coli_r(skp_root.coli_r);
    set_limit_angle(skp_root.hinge_y, skp_root.hinge_z);
    skin_param_ptr->coli.assign(skp_root.coli.begin(), skp_root.coli.end());
    skin_param_ptr->coli_type = skp_root.coli_type;
    skin_param_ptr->stiffness = skp_root.stiffness;
    skin_param_ptr->move_cancel = skp_root.move_cancel;
}

void RobOsage::SetSkpOsgNodes(const std::vector<skin_param_osage_node>* skp_osg_nodes) {
    if (!skp_osg_nodes)
        return;

    if (joint_node_vec.size() - 1 != skp_osg_nodes->size())
        return;

    RobJointNode* i = joint_node_vec.data() + 1;
    RobJointNode* i_end = joint_node_vec.data() + joint_node_vec.size();
    const skin_param_osage_node* j = skp_osg_nodes->data();
    const skin_param_osage_node* j_end = skp_osg_nodes->data() + skp_osg_nodes->size();
    while (i != i_end && j != j_end) {
        i->data_ptr->skp_osg_node = *j++;
        i->data_ptr->skp_osg_node.hinge.limit();
        i++;
    }
}

void RobOsage::SetWindDirection(const vec3* value) {
    if (value)
        wind_dir = *value;
    else
        wind_dir = 0.0f;
}

// 0x14047C750
void RobOsage::ctrl(const mat4& root_matrix, const vec3& hsc, const float_t step) {
    pos_init_cont(root_matrix, hsc, step, false);
}

// 0x14047F200
void RobOsage::dest() {
    joint_node_vec.clear();
    effector.reset();
    wind_dir = 0.0f;
    wet = 0.0f;
    rottype = ROTTYPE_YZ;
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
    use_ex_force = false;
    ex_force = 0.0f;
    root_matrix_ptr = 0;
    root_matrix_prev = mat4_null;
}

// 0x1404808C0
void RobOsage::dest_boc() {
    RobJointNode* i_begin = joint_node_vec.data() + 1;
    RobJointNode* i_end = joint_node_vec.data() + joint_node_vec.size();
    for (RobJointNode* i = i_begin; i != i_end; i++)
        i->data_ptr->boc.clear();
}

// 0x140482AA0
RobJointNode* RobOsage::get_joint_node(size_t index) {
    if (index < joint_node_vec.size())
        return &joint_node_vec.data()[index];
    else
        return &effector;
}

// 0x14047F370
void RobOsage::init(const obj_skin_ex_node_osage* osg_data, const obj_skin_osage_joint* joint,
    RobNode* ex_node, const obj_skin* skin) {
    dest();

    transform.init(osg_data->transform.position, osg_data->transform.rotation);
    joint_node_vec.clear();
    joint_node_vec.resize(osg_data->nb_joint + 1ULL);

    RobJointNode* node = joint_node_vec.data();
    node->reset();
    node->length_next = joint->length;
    RobNode* root_node = &ex_node[osg_data->root_idx & 0x7FFF];
    node->dst_node = root_node;
    node->dst_node_mat = root_node->mat_ptr;
    *root_node->no_scale_mat = mat4_identity;
    node->distance = 0;

    RobNode* parent_node_ptr = node->dst_node;
    for (uint32_t i = 0; i < osg_data->nb_joint; i++) {
        const obj_skin_osage_joint* osg_node = &joint[i];
        RobJointNode* node = &joint_node_vec.data()[i + 1LL];
        node->reset();
        node->length_back = osg_node[0].length;
        if (i + 1 < osg_data->nb_joint)
            node->length_next = osg_node[1].length;
        RobNode* node_ptr = &ex_node[osg_node->nid & 0x7FFF];
        node->dst_node = node_ptr;
        node->dst_node_mat = node_ptr->mat_ptr;
        node_ptr->parent = parent_node_ptr;
        parent_node_ptr = node->dst_node;
    }

    effector.reset();
    effector.length_back = joint[osg_data->nb_joint - 1].length;
    RobNode* effector_node = &ex_node[osg_data->efc_idx & 0x7FFF];
    effector.dst_node = effector_node;
    effector.dst_node_mat = effector_node->mat_ptr;
    effector_node->parent = parent_node_ptr;
    *effector.dst_node->no_scale_mat = mat4_identity;

    if (osg_data->nb_joint) {
        obj_skin_osage_joint_rotation* joint_rotation = osg_data->joint_rotation_array;
        RobJointNode* node = &joint_node_vec.data()[1];
        if (joint_rotation)
            for (uint32_t i = 0; i < osg_data->nb_joint; i++, joint_rotation++, node++) {
                mat3 mat;
                mat3_rotate_zyx(&joint_rotation->rotation, &mat);

                const vec3 rel_pos = { node->length_back, 0.0f, 0.0f };
                mat3_transform_vector(&mat, &rel_pos, &node->rel_pos);
            }
        else
            for (uint32_t i = 0; i < osg_data->nb_joint; i++, node++)
                node->rel_pos = { node->length_back, 0.0f, 0.0f };
    }

    if (osg_data->nb_joint) {
        RobJointNode* node = &joint_node_vec.data()[1];
        size_t v33 = 0;
        for (uint32_t i = 0; i < osg_data->nb_joint; i++, node++) {
            node->mat = mat4_null;

            obj_skin_bone* bone = skin->bone_array;
            for (int32_t j = 0; j < skin->num_bone; j++, bone++)
                if (bone->id == joint->nid) {
                    mat4_invert_fast(&bone->inv_bind_pose_mat, &node->mat);
                    break;
                }

            joint++;
        }
    }

    RobJointNode* last_node = &joint_node_vec.back();
    if (memcmp(&last_node->mat, &mat4_null, sizeof(mat4)))
        mat4_mul_translate(&last_node->mat, effector.length_back, 0.0f, 0.0f, &effector.mat);
}

// 0x1404816B0
void RobOsage::make_osage_coli(const mat4* motmat) {
    if (skin_param_ptr->coli.size())
        OsageCollision::Work::update_cls_work(coli_chara, skin_param_ptr->coli.data(), motmat);
    OsageCollision::Work::update_cls_work(coli_ring, ring.skp_root_coli, motmat);
}

// 0x14047F990
void RobOsage::pos_init(const mat4& root_matrix, const vec3& hsc, bool dist_flag) {
    if (!joint_node_vec.size())
        return;

    vec3 root_pos = transform.pos * hsc;
    mat4_transform_point(&root_matrix, &root_pos, &root_pos);
    RobJointNode* v12 = &joint_node_vec.data()[0];
    v12->pos = root_pos;
    v12->old_pos = root_pos;
    v12->vec = 0.0f;

    RobJointNode* node = &joint_node_vec.data()[0];
    const float_t floor_height = ring.get_floor_height(
        node->pos, node->data_ptr->skp_osg_node.coli_r);

    mat4 v78 = root_matrix;
    RotateMat(v78, hsc, true);

    vec3 v60 = { 1.0f, 0.0f, 0.0f };
    mat4_transform_vector(&v78, &v60, &v60);

    RobJointNode* i_begin = this->joint_node_vec.data() + 1;
    RobJointNode* i_end = this->joint_node_vec.data() + this->joint_node_vec.size();
    for (RobJointNode* i = i_begin; i != i_end; i++) {
        vec3 v74 = i->GetPrevNode().pos + vec3::normalize(v60) * (i->length_back * hsc.x);
        if (dist_flag && i->distance)
            length_limit(v74, i->distance->pos, i->length_dist);

        skin_param_osage_node* v38 = &i->data_ptr->skp_osg_node;
        OsageCollision::osage_cls(coli_ring, v74, v38->coli_r);
        OsageCollision::osage_cls(coli_chara, v74, v38->coli_r);

        const float_t v39 = floor_height + v38->coli_r;
        if (v74.y < v39 && v39 < 1001.0f) {
            v74.y = v39;
            v74 = vec3::normalize(v74 - i->GetPrevNode().pos) * (i->length_back * hsc.x) + i->GetPrevNode().pos;
        }
        i->pos = v74;
        i->vec = 0.0f;

        vec3 direction;
        mat4_inverse_transform_point(&v78, &i->pos, &direction);

        make_axis_matrix(v78, direction, &i->data_ptr->skp_osg_node.hinge,
            &i->reset_data.rotation, rottype);
        i->dst_node->transform.hsc = hsc;
        *i->dst_node->no_scale_mat = v78;

        if (i->dst_node_mat)
            mat4_scale_rot(&v78, &hsc, i->dst_node_mat);

        float_t v55 = vec3::distance(i->pos, i->GetPrevNode().pos);
        float_t v56 = i->length_back * hsc.x;
        if (v55 >= fabsf(v56))
            v56 = v55;
        mat4_mul_translate(&v78, v56, 0.0f, 0.0f, &v78);
        mat4_get_translation(&v78, &i->pos);
        v60 = i->pos - i->GetPrevNode().pos;
    }

    if (joint_node_vec.size() && effector.dst_node_mat) {
        mat4 mat = *joint_node_vec.back().dst_node->no_scale_mat;
        mat4_mul_translate_x(&mat, effector.length_back * hsc.x, &mat);
        *effector.dst_node->no_scale_mat = mat;

        mat4_scale_rot(&mat, &hsc, &mat);
        *effector.dst_node_mat = mat;
        effector.dst_node->transform.hsc = hsc;
    }
}

// 0x14047C770
void RobOsage::pos_init_cont(const mat4& root_matrix, const vec3& hsc,
    const float_t step, bool disable_ex_force) {
    ApplyPhysics(root_matrix, hsc, step, disable_ex_force, false, false);
    CollideNodesTargetOsage(root_matrix, hsc, step, true);
    EndCalc(root_matrix, hsc, step, disable_ex_force);
}

// 0x1404808F0
void RobOsage::reset_ex_force() {
    use_ex_force = false;
    ex_force = 0.0f;
}

// 0x140480C60
void RobOsage::set_air_res(float_t air) {
    skin_param_ptr->air_res = air;
}

// 0x140480C70
void RobOsage::set_angle(float_t angle_y, float_t angle_z) {
    skin_param_ptr->rot.y = angle_y * DEG_TO_RAD_FLOAT;
    skin_param_ptr->rot.z = angle_z * DEG_TO_RAD_FLOAT;
}

// 0x140480D30
void RobOsage::set_coli_r(float_t coli_r) {
    RobJointNode* i_begin = joint_node_vec.data() + 1;
    RobJointNode* i_end = joint_node_vec.data() + joint_node_vec.size();
    for (RobJointNode* i = i_begin; i != i_end; i++)
        i->data_ptr->skp_osg_node.coli_r = coli_r;
}

// 0x140480F10
void RobOsage::set_ex_force(const vec3& f) {
    use_ex_force = true;
    ex_force = f;
}

// 0x140481040
void RobOsage::set_force(float_t force, float_t gain) {
    skin_param_ptr->force = force;
    skin_param_ptr->force_gain = gain;
    RobJointNode* i_begin = joint_node_vec.data() + 1;
    RobJointNode* i_end = joint_node_vec.data() + joint_node_vec.size();
    for (RobJointNode* i = i_begin; i != i_end; i++) {
        i->data_ptr->force = force;
        force = force * gain;
    }
}

// 0x1404810A0
void RobOsage::set_init_angle(float_t angle_y, float_t angle_z) {
    skin_param_ptr->init_rot.y = angle_y * DEG_TO_RAD_FLOAT;
    skin_param_ptr->init_rot.z = angle_z * DEG_TO_RAD_FLOAT;
}

// 0x140480CA0
void RobOsage::set_limit_angle(float_t angle_y, float_t angle_z) {
    angle_y = min_def(angle_y, 179.0f) * DEG_TO_RAD_FLOAT;
    angle_z = min_def(angle_z, 179.0f) * DEG_TO_RAD_FLOAT;
    RobJointNode* i_begin = joint_node_vec.data() + 1;
    RobJointNode* i_end = joint_node_vec.data() + joint_node_vec.size();
    for (RobJointNode* i = i_begin; i != i_end; i++) {
        skin_param_osage_node* skp_osg_node = &i->data_ptr->skp_osg_node;
        skp_osg_node->hinge.ymin = -angle_y;
        skp_osg_node->hinge.ymax = angle_y;
        skp_osg_node->hinge.zmin = -angle_z;
        skp_osg_node->hinge.zmax = angle_z;
    }
}

// 0x1404810D0
void RobOsage::set_move_cancel(const float_t& mv_ccl) {
    move_cancel = mv_ccl;
}

// 0x1404814C0
void RobOsage::set_rot_type(ROTTYPE rot_type) {
    rottype = rot_type;
}

ExOsageBlock::ExOsageBlock() : motion_matrix(), flag() {
    init_coma_step = 1.0f;
    dest();
}

ExOsageBlock::~ExOsageBlock() {
    dest();
}

void ExOsageBlock::init() {
    dest();
}

void ExOsageBlock::CtrlBegin() {
    done = false;
}

void ExOsageBlock::CtrlStep(int32_t stage, bool disable_ex_force) {
    const RobDisp* rob_disp = skin_disp->rob_disp;
    float_t delta_frame = get_delta_frame();
#if OPD_PLAY_GEN
    if (opd_play_gen_run)
        delta_frame = 1.0f;
#endif
    float_t step = delta_frame * rob_disp->osage_step;
    if (rob_disp->opd_blend_data.size() && rob_disp->opd_blend_data.front().use_blend)
        step = 1.0f;

    const mat4& root_matrix = *parent->no_scale_mat;
    const vec3 hsc = parent->transform.hsc;
    switch (stage) {
    case 0:
        osage_work.BeginCalc(root_matrix, hsc, has_children_node);
        break;
    case 1:
    case 2:
        if ((stage == 1 && is_parent) || (stage == 2 && osage_work.apply_physics)) {
            SetWindDirection();
            osage_work.ApplyPhysics(root_matrix, hsc, step,
                disable_ex_force, true, has_children_node);
        }
        break;
    case 3:
        osage_work.make_osage_coli(motion_matrix);
        osage_work.CollideNodes(step);
        break;
    case 4:
        osage_work.ApplyBocRootColi(step);
        break;
    case 5: {
        osage_work.CollideNodesTargetOsage(root_matrix, hsc, step, false);
        osage_work.EndCalc(root_matrix, hsc, step, disable_ex_force);
        done = true;
    } break;
    }
}

void ExOsageBlock::ctrl() {
    flag.pos_init = 0;
    if (done) {
        done = false;
        return;
    }

    const RobDisp* rob_disp = skin_disp->rob_disp;
    float_t delta_frame = get_delta_frame();
#if OPD_PLAY_GEN
    if (opd_play_gen_run)
        delta_frame = 1.0f;
#endif
    float_t step = delta_frame * rob_disp->osage_step;
    if (rob_disp->opd_blend_data.size() && rob_disp->opd_blend_data.front().use_blend)
        step = 1.0f;

    const vec3 hsc = parent->transform.hsc;
    vec3 scale = parent->transform.scale;

    mat4 root_matrix = *parent->no_scale_mat;
    if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f) {
        scale = 1.0f / scale;
        mat4_scale_rot(&root_matrix, &scale, &root_matrix);
    }
    SetWindDirection();

    osage_work.make_osage_coli(motion_matrix);
    osage_work.ctrl(root_matrix, hsc, step);
}

void ExOsageBlock::CtrlOsagePlayData() {
    osage_work.CtrlOsagePlayData(*parent->no_scale_mat,
        parent->transform.hsc, skin_disp->rob_disp->opd_blend_data);
}

void ExOsageBlock::disp(const mat4& mat, render_context* rctx) {

}

void ExOsageBlock::dest() {
    block_idx = 0;
    osage_work.dest();
    flag.alive = 0;
    flag.pos_init = 0;
    motion_matrix = 0;
    init_coma_step = 1.0f;
    ExNodeBlock::dest();
}

void ExOsageBlock::disp_debug() {

}

void ExOsageBlock::pos_init() {
    init_coma_step = 4.0f;
    SetWindDirection();
    osage_work.make_osage_coli(motion_matrix);
    const vec3 hsc = parent->transform.hsc;
    osage_work.pos_init(*parent->no_scale_mat, hsc, false);
    flag.pos_init = 0;
}

void ExOsageBlock::pos_init_cont() {
    if (done) {
        done = false;
        return;
    }

    SetWindDirection();

    const vec3 hsc = parent->transform.hsc;
    osage_work.make_osage_coli(motion_matrix);
    osage_work.pos_init_cont(*parent->no_scale_mat, hsc, init_coma_step, true);
    init_coma_step = max_def(init_coma_step * 0.5f, 1.0f);
}

void ExOsageBlock::CtrlEnd() {
    const vec3 hsc = parent->transform.hsc;
    osage_work.CtrlEnd(hsc);
    done = false;
}

void ExOsageBlock::AddMotionResetData(const uint32_t& motnum, const float_t& frame) {
    osage_work.AddMotionResetData(motnum, frame);
}

const float_t* ExOsageBlock::LoadOpdData(size_t node_index, const float_t* opd_data, size_t opd_count) {
    return osage_work.LoadOpdData(node_index, opd_data, opd_count);
}

// 0x1405F9510
void ExOsageBlock::SetDisableCollision(const bool& value) {
    osage_work.SetDisableCollision(value);
}

void ExOsageBlock::SetMotionResetData(const uint32_t& motnum, const float_t& frame) {
    osage_work.SetMotionResetData(motnum, frame);
}

const float_t* ExOsageBlock::SetOsagePlayDataInit(const float_t* opdi_data) {
    return osage_work.SetOsagePlayDataInit(opdi_data);
}

// 0x1405F9A90, but it's combined with RobOsage::SetOsageReset
void ExOsageBlock::SetOsageReset() {
    osage_work.SetOsageReset();
}

void ExOsageBlock::SetRing(const osage_ring_data& ring) {
    osage_work.SetRing(ring);
}

void ExOsageBlock::SetSkinParam(skin_param_file_data* skp) {
    osage_work.SetSkinParam(skp);
}

void ExOsageBlock::SetWindDirection() {
    vec3 wind_dir = task_wind->stage_wind.wind->val * skin_disp->rob_disp->wind_strength;
    osage_work.SetWindDirection(&wind_dir);
}

// 0x1405F3E10
void ExOsageBlock::make_joint_map(const obj_skin_ex_node_osage* root,
    const obj_skin_osage_joint* joint, prj::vector_pair<uint32_t, RobJointNode*>& joint_map,
    std::map<std::string, ExNodeBlock*>& node_name_map) {
    RobJointNode* root_node = osage_work.get_joint_node(0);
    joint_map.push_back(root->root_idx, root_node);

    for (uint32_t i = 0; i < root->nb_joint; i++) {
        RobJointNode* node = osage_work.get_joint_node(i + 1ULL);
        joint_map.push_back(joint[i].nid, node);

        if (node->dst_node && node->dst_node->name)
            node_name_map.insert({ node->dst_node->name, this });
    }

    RobJointNode* effector_node = osage_work.get_joint_node(root->nb_joint + 1ULL);
    joint_map.push_back(root->efc_idx, effector_node);

    if (effector_node->dst_node && effector_node->dst_node->name)
        node_name_map.insert({ effector_node->dst_node->name, this });
}

void ExOsageBlock::reset_ex_force() {
    osage_work.reset_ex_force();
}

// 0x1405F7E10
void ExOsageBlock::set_data(const RobSkinDisp* skp,
    const obj_skin_ex_node_osage* root, const obj_skin_osage_joint* joint,
    const RobNode* mot_node, RobNode* ex_node, const obj_skin* skin) {
    ExNodeBlock::set_data(&ex_node[root->root_idx & 0x7FFF], EX_NODE_TYPE_OSAGE, 0, skp);
    osage_work.init(root, joint, ex_node, skin);
    flag.pos_init = 0;
    motion_matrix = mot_node->mat_ptr;
}

void ExOsageBlock::set_ex_force(const vec3& f) {
    osage_work.set_ex_force(f);
}

// 0x1405F9290
void ExOsageBlock::set_move_cancel(const float_t& mv_ccl) {
    osage_work.set_move_cancel(mv_ccl);
}

ExConstraintBlock::ExConstraintBlock() : cns_type(), src_node(),
upvector_node(), data(), dst_node_id(), src_node_id() {
    init();
}

ExConstraintBlock::~ExConstraintBlock() {

}

void ExConstraintBlock::init() {
    cns_type = OBJ_SKIN_EX_NODE_CONSTRAINT_NONE;
    src_node = 0;
    upvector_node = 0;
    data = 0;
    dst_node_id = 0;
    src_node_id = 0;
}

void ExConstraintBlock::CtrlBegin() {
    if (dst_node) {
        RobTransform& transform = dst_node->transform;
        transform.pos = data->transform.position;
        transform.rot = data->transform.rotation;
        transform.scale = data->transform.scale;
    }
    done = false;
}

void ExConstraintBlock::CtrlStep(int32_t stage, bool disable_ex_force) {
    if (done)
        return;

    switch (stage) {
    case 0:
        if (is_parent)
            ctrl();
        break;
    case 2:
        if (has_children_node)
            CalcMatrixHS();
        break;
    case 5:
        ctrl();
        break;
    }
}

void ExConstraintBlock::ctrl() {
    if (!parent)
        return;

    if (done) {
        done = false;
        return;
    }

    Calc();
    CalcMatrixHS();
    done = true;
}

void ExConstraintBlock::CtrlOsagePlayData() {
    ctrl();
}

void ExConstraintBlock::disp(const mat4& mat, render_context* rctx) {

}

void ExConstraintBlock::disp_debug() {

}

void ExConstraintBlock::pos_init() {
    ctrl();
}

void ExConstraintBlock::pos_init_cont() {
    ctrl();
}

void ExConstraintBlock::Calc() {
    RobNode* node = dst_node;
    if (!node)
        return;

    vec3 pos = parent->transform.hsc * node->transform.pos;

    mat4 mat;
    mat4_mul_translate(parent->no_scale_mat, &pos, &mat);

    switch (cns_type) {
    case OBJ_SKIN_EX_NODE_CONSTRAINT_ORIENTATION:
        CalcConstraintOrientation(mat);
        break;
    case OBJ_SKIN_EX_NODE_CONSTRAINT_DIRECTION:
        CalcConstraintDirection(mat);
        break;
    case OBJ_SKIN_EX_NODE_CONSTRAINT_POSITION:
        CalcConstraintPosition(mat);
        break;
    case OBJ_SKIN_EX_NODE_CONSTRAINT_DISTANCE:
        CalcConstraintDistance(mat);
        break;
    default:
        *node->mat_ptr = mat;
        break;
    }
}

inline void ExConstraintBlock::CalcConstraintDirection(mat4 mat) {
    obj_skin_ex_node_constraint_direction* direction = data->direction;

    vec3 align_axis = direction->align_axis;
    vec3 target_offset = direction->target_offset;

    vec3 world_offset;
    mat4_transform_point(src_node->mat_ptr, &target_offset, &world_offset);

    vec3 v51;
    mat4_inverse_transform_point(&mat, &world_offset, &v51);
    float_t v51_length = vec3::length_squared(v51);
    if (v51_length <= 0.000001f)
        return;

    mat4 rot_mat;
    make_direction_matrix(rot_mat, align_axis, v51);

    if (upvector_node) {
        vec3 affected_axis = direction->up_vector.affected_axis;

        mat4 dir_mat;
        mat4_mul(&rot_mat, &mat, &dir_mat);

        vec3 dir_axis;
        mat4_transform_vector(&dir_mat, &affected_axis, &dir_axis);

        mat4* up_mat = upvector_node->mat_ptr;

        vec3 up_vector_trans;
        vec3 post_dir_trans;
        mat4_get_translation(up_mat, &up_vector_trans);
        mat4_get_translation(&dir_mat, &post_dir_trans);

        vec3 up_vector_off = up_vector_trans - post_dir_trans;

        vec3 v50;
        mat4_transform_vector(&mat, &v51, &v50);

        vec3 v25 = vec3::normalize(vec3::cross(v50, dir_axis));
        vec3 v29 = vec3::normalize(vec3::cross(v50, up_vector_off));

        vec3 v35 = vec3::cross(v25, v29);

        float_t v39 = vec3::dot(v25, v29);
        float_t v36 = vec3::dot(v35, v50);

        float_t v40 = vec3::length(v35);
        if (v36 >= 0.0f)
            v40 = -v40;

        mat4 up_corr_mat;
        mat4_set(&v51, -v40, v39, &up_corr_mat);
        mat4_mul(&rot_mat, &up_corr_mat, &rot_mat);
    }

    mat4_mul(&rot_mat, &mat, dst_node->mat_ptr);
}

void ExConstraintBlock::CalcConstraintDistance(mat4 mat) {
    *dst_node->mat_ptr = mat;
}

void ExConstraintBlock::CalcConstraintOrientation(mat4 mat) {
    obj_skin_ex_node_constraint_orientation* orientation = data->orientation;

    mat3 rot;
    mat4_to_mat3(src_node->mat_ptr, &rot);
    mat3_normalize_rotation(&rot, &rot);
    mat3_mul_rotate_zyx(&rot, &orientation->offset, &rot);
    mat4_replace_rotation(&mat, &rot, dst_node->mat_ptr);
}

void ExConstraintBlock::CalcConstraintPosition(mat4 mat) {
    obj_skin_ex_node_constraint_position* position = data->position;

    vec3 constraining_offset = position->constraining_object.offset;
    vec3 constrained_offset = position->constrained_object.offset;
    if (position->constraining_object.affected_by_orientation)
        mat4_transform_vector(src_node->mat_ptr, &constraining_offset, &constraining_offset);

    vec3 source_node_trans;
    mat4_get_translation(src_node->mat_ptr, &source_node_trans);
    source_node_trans = constraining_offset + source_node_trans;
    mat4_set_translation(&mat, &source_node_trans);

    if (upvector_node) {
        vec3 up_vector_trans;
        mat4_get_translation(upvector_node->mat_ptr, &up_vector_trans);
        mat4_inverse_transform_point(&mat, &up_vector_trans, &up_vector_trans);

        mat4 rot_mat;
        make_direction_matrix(rot_mat, position->up_vector.affected_axis, up_vector_trans);
        mat4_mul(&rot_mat, &mat, &mat);
    }

    if (position->constrained_object.affected_by_orientation)
        mat4_transform_vector(&mat, &constrained_offset, &constrained_offset);

    mat4 constrained_offset_mat;
    mat4_translate(&constrained_offset, &constrained_offset_mat);
    mat4_mul(&mat, &constrained_offset_mat, dst_node->mat_ptr);
}

void ExConstraintBlock::CalcMatrixHS() {
    if (!dst_node)
        return;

    RobTransform& transform = dst_node->transform;
    const vec3 hsc = parent->transform.hsc;

    mat4 mat;
    mat4_invert_fast(parent->no_scale_mat, &mat);
    mat4_mul(dst_node->mat_ptr, &mat, &mat);
    mat4_get_rotation_zyx(&mat, &transform.rot);
    mat4_get_translation(&mat, &transform.pos);
    if (fabsf(hsc.x) > 0.000001f)
        transform.pos.x /= hsc.x;
    if (fabsf(hsc.y) > 0.000001f)
        transform.pos.y /= hsc.z;
    if (fabsf(hsc.z) > 0.000001f)
        transform.pos.z /= hsc.z;
    *dst_node->no_scale_mat = *dst_node->mat_ptr;
    mat4_scale_rot(dst_node->mat_ptr, &hsc, dst_node->mat_ptr);
    transform.hsc = transform.scale * hsc;
}


void ExConstraintBlock::set_data(const RobSkinDisp* skin_disp,
    const obj_skin_ex_node_constraint* data, const char* name, const bone_database* bone_data) {
    RobNode* node = (RobNode*)skin_disp->get_node(name, bone_data);
    type = EX_NODE_TYPE_CONSTRAINT;
    dst_node = node;
    this->data = data;
    this->name = node->name;
    this->skin_disp = skin_disp;

    src_node = skin_disp->get_node(data->src_name, bone_data);

    obj_skin_ex_node_constraint_type type = data->type;
    const char* up_vector_name;
    if (type == OBJ_SKIN_EX_NODE_CONSTRAINT_DIRECTION) {
        cns_type = OBJ_SKIN_EX_NODE_CONSTRAINT_DIRECTION;
        up_vector_name = data->direction->up_vector.name;
    }
    else if (type == OBJ_SKIN_EX_NODE_CONSTRAINT_POSITION) {
        cns_type = OBJ_SKIN_EX_NODE_CONSTRAINT_POSITION;
        up_vector_name = data->position->up_vector.name;
    }
    else if (type == OBJ_SKIN_EX_NODE_CONSTRAINT_DISTANCE) {
        cns_type = OBJ_SKIN_EX_NODE_CONSTRAINT_DISTANCE;
        up_vector_name = data->distance->up_vector.name;
    }
    else if (type == OBJ_SKIN_EX_NODE_CONSTRAINT_ORIENTATION) {
        cns_type = OBJ_SKIN_EX_NODE_CONSTRAINT_ORIENTATION;
        return;
    }
    else {
        cns_type = OBJ_SKIN_EX_NODE_CONSTRAINT_NONE;
        return;
    }

    if (up_vector_name)
        upvector_node = skin_disp->get_node(up_vector_name, bone_data);
}

float_t Expr_node::eval() {
    float_t value = 0.0f;
    switch (type) {
    case Expr_constant:
        value = data.constant;
        break;
    case Expr_variable:
        value = *data.variable;
        break;
    case Expr_variable_rad:
        value = *data.variable * RAD_TO_DEG_FLOAT;
        break;
    case Expr_func1: {
        float_t v1 = operand[0]->eval();
        if (data.func1)
            value = data.func1(v1);
    } break;
    case Expr_func2: {
        float_t v1 = operand[0]->eval();
        float_t v2 = operand[1]->eval();
        if (data.func1)
            value = data.func2(v1, v2);
    } break;
    case Expr_func3: {
        float_t v1 = operand[0]->eval();
        float_t v2 = operand[1]->eval();
        float_t v3 = operand[2]->eval();
        if (data.func3)
            value = data.func3(v1, v2, v3);
    } break;
    }
    return value;
}

ExExpressionBlock::ExExpressionBlock() : result(), result_type(), expr_node(),
expr_work(), data(), field_3D20(), hard_coded_func(), timer(), step(true) {

}

ExExpressionBlock::~ExExpressionBlock() {

}

void ExExpressionBlock::init() {
    timer = 0.0f;
}

void ExExpressionBlock::CtrlBegin() {
    RobTransform& transform = dst_node->transform;
    transform.pos = data->transform.position;
    transform.rot = data->transform.rotation;
    transform.scale = data->transform.scale;
    done = false;
}

void ExExpressionBlock::CtrlStep(int32_t stage, bool disable_ex_force) {
    if (done)
        return;

    switch (stage) {
    case 0:
        if (is_parent)
            ctrl();
        break;
    case 2:
        if (has_children_node)
            CalcMatrixHS();
        break;
    case 5:
        ctrl();
        break;
    }
}

void ExExpressionBlock::ctrl() {
    if (!parent)
        return;

    if (done) {
        done = false;
        return;
    }

    Calc();
    CalcMatrixHS();
    done = true;
}

void ExExpressionBlock::CtrlOsagePlayData() {
    ctrl();
}

void ExExpressionBlock::disp(const mat4& mat, render_context* rctx) {

}

void ExExpressionBlock::disp_debug() {

}

void ExExpressionBlock::pos_init() {
    ctrl();
}

void ExExpressionBlock::pos_init_cont() {
    ctrl();
}

void ExExpressionBlock::Calc() {
    float_t delta_frame = get_delta_frame();
#if OPD_PLAY_GEN
    if (opd_play_gen_run)
        delta_frame = 1.0f;
#endif
    if (step)
        delta_frame *= skin_disp->rob_disp->osage_step;
    float_t timer = this->timer + delta_frame;
    this->timer = timer >= 65535.0f ? timer - 65535.0f : timer;

    if (hard_coded_func) {
        hard_coded_func(&dst_node->transform);
        return;
    }

    for (int32_t i = 0; i < 9; i++) {
        if (!expr_node[i])
            break;

        float_t value = expr_node[i]->eval();
        if (result_type[i] == Expr_variable_rad)
            value *= DEG_TO_RAD_FLOAT;
        *result[i] = value;
    }
}

void ExExpressionBlock::CalcMatrixHS() {
    RobTransform* data = &dst_node->transform;
    const vec3 hsc = parent->transform.hsc;
    mat4 mat = *parent->no_scale_mat;
    mat4 dsp_mat = mat4_identity;
    data->CalcMatrixHS(hsc, mat, dsp_mat);
    *dst_node->mat_ptr = dsp_mat;
    *dst_node->no_scale_mat = mat;
}

void ExExpressionBlock::set_data(const RobSkinDisp* skin_disp,
    const obj_skin_ex_node_expression* data, const char* node_name, object_info objuid,
    size_t index, const bone_database* bone_data) {
    Expr_node* stack_buf[28];
    Expr_node** stack_buf_val = stack_buf;

    RobNode* node = (RobNode*)skin_disp->get_node(node_name, bone_data);
    type = EX_NODE_TYPE_EXPRESSION;
    dst_node = node;
    this->data = data;
    this->name = node->name;
    this->skin_disp = skin_disp;

    node->transform.pos = data->transform.position;
    node->transform.rot = data->transform.rotation;
    node->transform.scale = data->transform.scale;
    hard_coded_func = 0;

    Expr_node* expr = expr_work;
    for (int32_t i = 0; i < 9; i++) {
        result[i] = 0;
        expr_node[i] = 0;
    }

    for (int32_t i = 0; i < 9; i++) {
        const char* script = data->script[i];
        if (!script || str_utils_compare_length(script, utf8_length(script), "= ", 2))
            break;

        script += 2;

        int32_t index = 0;
        script = str_utils_get_next_int32_t(script, index, ' ');
        result[i] = dst_node->get_transform_component(index, result_type[i]);

        while (script) {
            std::string value_type;
            script = str_utils_get_next_string(script, value_type, ' ');
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
                expr->type = Expr_constant;
                script = str_utils_get_next_float_t(script, expr->data.constant, ' ');
                *stack_buf_val++ = expr;
            }
            else if (value_type[0] == 'v') {
                std::string func_str;
                script = str_utils_get_next_string(script, func_str, ' ');
                expr->type = Expr_variable;
                int32_t index = func_str[0] - '0';
                if (index >= 0 && index < 9) {
                    const RobNode* node = skin_disp->get_node(func_str.c_str() + 2, bone_data);
                    if (node)
                        expr->data.variable = node->get_transform_component(index, expr->type);
                    else {
                        expr->type = Expr_constant;
                        expr->data.constant = 0.0f;
                    }
                }
                else
                    expr->data.variable = &timer;
                *stack_buf_val++ = expr;
            }
            else if (value_type[0] == 'f') {
                std::string func_str;
                script = str_utils_get_next_string(script, func_str, ' ');
                expr->type = Expr_func1;
                expr->operand[0] = stack_buf_val[-1];
                expr->data.func1 = ExpFuncUnaryTblFindFunc(func_str, exp_func_unary_tbl)->func;
                stack_buf_val[-1] = expr;
            }
            else if (value_type[0] == 'g') {
                std::string func_str;
                script = str_utils_get_next_string(script, func_str, ' ');
                expr->type = Expr_func2;
                expr->operand[0] = stack_buf_val[-2];
                expr->operand[1] = stack_buf_val[-1];
                expr->data.func2 = ExpFuncBinaryTblFindFunc(func_str, exp_func_binary_tbl)->func;
                stack_buf_val[-2] = expr;
                stack_buf_val--;
            }
            else if (value_type[0] == 'h') {
                std::string func_str;
                script = str_utils_get_next_string(script, func_str, ' ');
                expr->type = Expr_func3;
                expr->operand[0] = stack_buf_val[-3];
                expr->operand[1] = stack_buf_val[-2];
                expr->operand[2] = stack_buf_val[-1];
                expr->data.func3 = ExpFuncTernaryTblFindFunc(func_str, exp_func_ternary_tbl)->func;
                stack_buf_val[-3] = expr;
                stack_buf_val -= 2;
            }
            expr_node[i] = stack_buf_val[-1];
            expr++;
        }
    }

    step = !(index == RPK_TE_R || index == RPK_TE_L);
}

static const ExpFuncUnaryTbl* ExpFuncUnaryTblFindFunc(std::string& name, const ExpFuncUnaryTbl* array) {
    const char* name_str = name.c_str();
    while (array->name) {
        if (!str_utils_compare(name_str, array->name))
            return array;
        array++;
    }
    return 0;
}

static const ExpFuncBinaryTbl* ExpFuncBinaryTblFindFunc(std::string& name, const ExpFuncBinaryTbl* array) {
    const char* name_str = name.c_str();
    while (array->name) {
        if (!str_utils_compare(name_str, array->name))
            return array;
        array++;
    }
    return 0;
}

static const ExpFuncTernaryTbl* ExpFuncTernaryTblFindFunc(std::string& name, const ExpFuncTernaryTbl* array) {
    const char* name_str = name.c_str();
    while (array->name) {
        if (!str_utils_compare(name_str, array->name))
            return array;
        array++;
    }
    return 0;
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

// 0x14021A290
static float_t calc_tangent_basis(const vec3& p0, const vec3& p1, const vec3& p2,
    const vec2& uv0, const vec2& uv1, const vec2& uv2, vec3& tangent, vec3& binormal, vec3& normal) {
    vec3 v1 = p1 - p0;
    vec3 v2 = p2 - p0;

    normal = vec3::cross(v1, v2);

    float_t normal_length = vec3::length(normal);
    if (normal_length <= 0.000001f)
        return 1.0f;

    normal *= 1.0f / normal_length;

    vec2 t1 = uv1 - uv0;
    vec2 t2 = uv2 - uv0;

    float_t r = 1.0f / (t2.y * t1.x - t1.y * t2.x);

    vec3 _tangent = vec3::normalize((v1 * t2.y - v2 * t1.y) * r);
    binormal = vec3::cross(_tangent, normal);
    tangent = vec3::cross(normal, binormal);

    mat3 mat = mat3(tangent, binormal, normal);
    mat3_transpose(&mat, &mat);
    return mat3_determinant(&mat);
}

// 0x14021A5E0
static float_t calc_tangent_basis(const vec3& v1, const vec3& v2,
    const float_t t1, const float_t t2, vec3& tangent, vec3& binormal, vec3& normal) {
    normal = vec3::normalize(vec3::cross(v1, v2));

    vec3 _tangent = vec3::normalize(v1 * t2 - v2 * t1);
    binormal = vec3::cross(_tangent, normal);
    tangent = vec3::cross(normal, binormal);

    mat3 mat = mat3(tangent, binormal, normal);
    mat3_transpose(&mat, &mat);
    return mat3_determinant(&mat);
}

// 0x14021A890
static void calc_tangent_basis_sub(CLOTH_VERTEX* vtx, const CLOTH_VERTEX* vtxL, const CLOTH_VERTEX* vtxR,
    const CLOTH_VERTEX* vtxU, const CLOTH_VERTEX* vtxD) {
    const vec3 v1 = vtxU->pos - vtxD->pos;
    const vec3 v2 = vtxR->pos - vtxL->pos;

    const float_t v2_length = vec3::length(v1);
    const float_t v1_length = vec3::length(v2);
    if (v2_length <= 0.000001f || v1_length <= 0.000001f)
        return;

    const vec2 uv_a = vtxU->uv - vtxD->uv;
    const vec2 uv_b = vtxR->uv - vtxL->uv;

    float_t r = uv_b.y * uv_a.x - uv_a.y * uv_b.x;
    if (fabsf(r) > 0.000001f) {
        r = 1.0f / r;
        vtx->m = calc_tangent_basis(v1, v2, uv_a.y * r, uv_b.y * r, vtx->tangent, vtx->binormal, vtx->normal);
    }
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

// 0x140482F30
static void length_limit(vec3& p, const vec3& r, float_t rr) {
    const vec3 d = p - r;
    const float_t dist = vec3::length_squared(d);
    if (dist > rr * rr)
        p = r + d * (rr / sqrtf(dist));
}

// 0x140482FF0
static bool make_axis_matrix(mat4& mat, const vec3& axis,
    const skin_param_hinge* hinge, vec3* rot, const ROTTYPE& rottype) {
    bool rot_clamped = false;
    float_t z_rot;
    float_t y_rot;
    if (rottype == ROTTYPE_ZY) {
        y_rot = atan2f(-axis.z, axis.x);
        z_rot = atan2f(axis.y, sqrtf(axis.x * axis.x + axis.z * axis.z));
        if (hinge)
            rot_clamped = hinge->clamp(y_rot, z_rot);
        mat4_mul_rotate_y(&mat, y_rot, &mat);
        mat4_mul_rotate_z(&mat, z_rot, &mat);
    }
    else {
        z_rot = atan2f(axis.y, axis.x);
        y_rot = atan2f(-axis.z, sqrtf(axis.x * axis.x + axis.y * axis.y));
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

// 0x1401EB410
static void make_direction_matrix(mat4& mat, const vec3& v0, const vec3& v1) {
    vec3 v0_norm = vec3::normalize(v0);
    vec3 v1_norm = vec3::normalize(v1);

    vec3 axis = vec3::cross(v1_norm, v0_norm);

    float_t c = clamp_def(vec3::dot(v1_norm, v0_norm), -1.0f, 1.0f);
    float_t s = sqrtf(clamp_def(1.0f - c * c, 0.0f, 1.0f));
    mat4_set(&axis, -s, c, &mat);
}

static void modify_cloth_object(obj_mesh* mesh, obj_mesh_vertex_buffer* vb,
    CLOTH_VERTEX* vtxarg, float_t sgn, int32_t num_idx, uint16_t* idxtbl, bool do_ura) {
    if (!mesh || !vb || (mesh->vertex_format
        & (OBJ_VERTEX_NORMAL | OBJ_VERTEX_POSITION)) != (OBJ_VERTEX_NORMAL | OBJ_VERTEX_POSITION))
        return;

    vb->cycle_index();
    GL::ArrayBuffer buffer = vb->get_buffer();
    size_t data = (size_t)buffer.MapMemory(gl_state);
    if (!data)
        return;

    if (do_ura)
        num_idx /= 2;

    bool tangent = !!(mesh->vertex_format & OBJ_VERTEX_TANGENT);

    switch (mesh->attrib.m.compression) {
    case 0:
    default:
        if (tangent)
            for (int32_t i = 2; i; i--, sgn = -1.0f) {
                for (int32_t j = num_idx; j; j--, idxtbl++) {
                    CLOTH_VERTEX* vtx = &vtxarg[*idxtbl];

                    *(vec3*)data = vtx->pos;
                    *(vec3*)(data + 0x0C) = vtx->normal * sgn;
                    *(vec3*)(data + 0x18) = vtx->tangent;
                    *(float_t*)(data + 0x24) = vtx->m;

                    data += mesh->size_vertex;
                }

                if (!do_ura)
                    break;
            }
        else
            for (int32_t i = 2; i; i--, sgn = -1.0f) {
                for (int32_t j = num_idx; j; j--, idxtbl++) {
                    CLOTH_VERTEX* vtx = &vtxarg[*idxtbl];

                    *(vec3*)data = vtx->pos;
                    *(vec3*)(data + 0x0C) = vtx->normal * sgn;

                    data += mesh->size_vertex;
                }

                if (!do_ura)
                    break;
            }
        break;
    case 1:
        if (tangent)
            for (int32_t i = 2; i; i--, sgn = -1.0f) {
                for (int32_t j = num_idx; j; j--, idxtbl++) {
                    CLOTH_VERTEX* vtx = &vtxarg[*idxtbl];

                    *(vec3*)data = vtx->pos;

                    vec3_to_vec3i16(vtx->normal * (32767.0f * sgn), *(vec3i16*)(data + 0x0C));
                    *(int16_t*)(data + 0x12) = 0;

                    vec4 tangent;
                    *(vec3*)&tangent = vtx->tangent;
                    tangent.w = vtx->m;
                    vec4_to_vec4i16(tangent * 32767.0f, *(vec4i16*)(data + 0x14));

                    data += mesh->size_vertex;
                }

                if (!do_ura)
                    break;
            }
        else
            for (int32_t i = 2; i; i--, sgn = -1.0f) {
                for (int32_t j = num_idx; j; j--, idxtbl++) {
                    CLOTH_VERTEX* vtx = &vtxarg[*idxtbl];

                    *(vec3*)data = vtx->pos;

                    vec3_to_vec3i16(vtx->normal * (32767.0f * sgn), *(vec3i16*)(data + 0x0C));
                    *(int16_t*)(data + 0x12) = 0;

                    data += mesh->size_vertex;
                }

                if (!do_ura)
                    break;
            }
    case 2:
        if (tangent)
            for (int32_t i = 2; i; i--, sgn = -1.0f) {
                for (int32_t j = num_idx; j; j--, idxtbl++) {
                    CLOTH_VERTEX* vtx = &vtxarg[*idxtbl];

                    *(vec3*)data = vtx->pos;

                    vec3i16 normal_int;
                    vec3_to_vec3i16(vtx->normal * 511.0f, normal_int);
                    *(uint32_t*)(data + 0x0C) = (((uint32_t)0 & 0x03) << 30)
                        | (((uint32_t)normal_int.z & 0x3FF) << 20)
                        | (((uint32_t)normal_int.y & 0x3FF) << 10)
                        | ((uint32_t)normal_int.x & 0x3FF);

                    vec4 tangent;
                    *(vec3*)&tangent = vtx->tangent;
                    tangent.w = vtx->m;

                    vec4i16 tangent_int;
                    vec4_to_vec4i16(tangent * 511.0f, tangent_int);
                    *(uint32_t*)(data + 0x10) = (((uint32_t)clamp_def(tangent_int.w, -1, 1) & 0x03) << 30)
                        | (((uint32_t)tangent_int.z & 0x3FF) << 20)
                        | (((uint32_t)tangent_int.y & 0x3FF) << 10)
                        | ((uint32_t)tangent_int.x & 0x3FF);

                    data += mesh->size_vertex;
                }

                if (!do_ura)
                    break;
            }
        else
            for (int32_t i = 2; i; i--, sgn = -1.0f) {
                for (int32_t j = num_idx; j; j--, idxtbl++) {
                    CLOTH_VERTEX* vtx = &vtxarg[*idxtbl];

                    *(vec3*)data = vtx->pos;

                    vec3i16 normal_int;
                    vec3_to_vec3i16(vtx->normal * 511.0f, normal_int);
                    *(uint32_t*)(data + 0x0C) = (((uint32_t)0 & 0x03) << 30)
                        | (((uint32_t)normal_int.z & 0x3FF) << 20)
                        | (((uint32_t)normal_int.y & 0x3FF) << 10)
                        | ((uint32_t)normal_int.x & 0x3FF);

                    data += mesh->size_vertex;
                }

                if (!do_ura)
                    break;
            }
        break;
    }

    buffer.UnmapMemory(gl_state);
}
