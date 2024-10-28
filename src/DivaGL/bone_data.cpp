/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "bone_data.hpp"

ExNodeBlock_vtbl* ExNodeBlock_vftable = (ExNodeBlock_vtbl*)0x0000000140A82DC8;
ExNodeBlock_vtbl* ExOsageBlock_vftable = (ExNodeBlock_vtbl*)0x0000000140A82F88;
ExNodeBlock_vtbl* ExClothBlock_vftable = (ExNodeBlock_vtbl*)0x0000000140A82FF0;

void (*origExNodeBlock__CtrlBegin)(ExNodeBlock* node);

void (*origExOsageBlock__Init)(ExOsageBlock* osg);
void (*origExOsageBlock__CtrlStep)(ExOsageBlock* osg, int32_t stage, bool disable_external_force);
void (*origExOsageBlock__CtrlMain)(ExOsageBlock* osg);
void (*origExOsageBlock__CtrlOsagePlayData)(ExOsageBlock* osg);
void (*origExOsageBlock__Disp)(ExOsageBlock* osg);
void (*origExOsageBlock__Reset)(ExOsageBlock* osg);
void (*origExOsageBlock__Field_40)(ExOsageBlock* osg);
void (*origExOsageBlock__CtrlInitBegin)(ExOsageBlock* osg);
void (*origExOsageBlock__CtrlInitMain)(ExOsageBlock* osg);
void (*origExOsageBlock__CtrlEnd)(ExOsageBlock* osg);

static void* (*operator_new)(size_t) = (void* (*)(size_t))0x000000014084530C;
static void(*operator_delete)(void*) = (void(*)(void*))0x0000000140845378;

static const float_t get_osage_gravity_const() {
    return 0.00299444468691945f;
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

// 0x140482DF0
void opd_node_data::lerp(opd_node_data& dst, const opd_node_data& src0, const opd_node_data& src1, float_t blend) {
    dst.length = lerp_def(src0.length, src1.length, blend);
    dst.rotation = vec3::lerp(src0.rotation, src1.rotation, blend);
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

inline float_t osage_ring_data::get_floor_height(const vec3& pos, const float_t coli_r) {
    if (pos.x < rect_x - coli_r
        || pos.z < rect_y - coli_r
        || pos.x > rect_x + rect_width
        || pos.z > rect_y + rect_height)
        return out_height + coli_r;
    else
        return ring_height + coli_r;
}

// 0x140485450
void OsageCollision::Work::update_cls_work(OsageCollision::Work* cls,
    SkinParam::CollisionParam* cls_param, const mat4* transform) {
    cls->type = SkinParam::CollisionTypeEnd;
    if (!cls_param || !transform)
        return;

    for (; cls_param->type != SkinParam::CollisionTypeEnd; cls++, cls_param++) {
        cls->type = cls_param->type;
        cls->radius = cls_param->radius;
        mat4 mat = transform[cls_param->node_idx[0]];
        mat4_transpose(&mat, &mat);
        mat4_transform_point(&mat, &cls_param->pos[0], &cls->pos[0]);
        switch (cls->type) {
        case SkinParam::CollisionTypePlane:
            mat4_transform_vector(&mat, &cls_param->pos[1], &cls->pos[1]);
            break;
        case SkinParam::CollisionTypeCapsule:
        case SkinParam::CollisionTypeEllipse:
            mat = transform[cls_param->node_idx[1]];
            mat4_transpose(&mat, &mat);
            mat4_transform_point(&mat, &cls_param->pos[1], &cls->pos[1]);
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
    prj::vector<SkinParam::CollisionParam>& cls_list, const mat4* transform) {
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
    if (!cls)
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
    if (coli.work_list.size() && coli.work_list.back().type != SkinParam::CollisionTypeEnd)
        return OsageCollision::osage_cls(p, cls_r, coli.work_list.data(), fric);
    return 0;
}

static void RobOsage__node_data_init(RobOsageNodeData *node_data) {
    node_data->force = 0.0f;
    node_data->boc.clear();
    node_data->skp_osg_node.coli_r = 0.0f;
    node_data->skp_osg_node.weight = 1.0f;
    node_data->skp_osg_node.inertial_cancel = 0.0f;
    node_data->skp_osg_node.hinge = skin_param_hinge();
    node_data->skp_osg_node.hinge.limit();
    node_data->normal_ref.set = false;
    node_data->normal_ref.n = 0;
    node_data->normal_ref.u = 0;
    node_data->normal_ref.d = 0;
    node_data->normal_ref.l = 0;
    node_data->normal_ref.r = 0;
    node_data->normal_ref.mat = mat4_identity;
}

static void RobOsageNode__Reset(RobOsageNode* node) {
    node->length = 0.0f;
    node->pos = 0.0f;
    node->fixed_pos = 0.0f;
    node->delta_pos = 0.0f;
    node->vel = 0.0f;
    node->child_length = 0.0f;
    node->bone_node_ptr = 0;
    node->bone_node_mat = 0;
    node->sibling_node = 0;
    node->max_distance = 0.0f;
    node->rel_pos = 0.0f;
    node->reset_data.pos = 0.0f;
    node->reset_data.delta_pos = 0.0f;
    node->reset_data.rotation = 0.0f;
    node->reset_data.length = 0.0f;
    node->hit = 0.0f;
    node->friction = 1.0f;
    node->external_force = 0.0f;
    node->force = 1.0f;
    RobOsage__node_data_init(&node->data);
    node->data_ptr = &node->data;
    node->opd_data.clear();
    node->opd_data.resize(3);
    node->mat = mat4_null;
}

static void skin_param_reset(skin_param* skp) {
    skp->coli.clear();
    skp->friction = 1.0f;
    skp->wind_afc = 0.0f;
    skp->air_res = 1.0f;
    skp->rot = 0.0f;
    skp->init_rot = 0.0f;
    skp->coli_type = SkinParam::RootCollisionTypeEnd;
    skp->stiffness = 0.0f;
    skp->move_cancel = -0.01f;
    skp->coli_r = 0.0f;
    skp->hinge = skin_param_hinge();
    skp->hinge.limit();
    skp->force = 0.0f;
    skp->force_gain = 0.0f;
    skp->colli_tgt_osg = 0;
}

static void RobOsage__Reset(RobOsage* rob_osg) {
    rob_osg->nodes.clear();
    RobOsageNode__Reset(&rob_osg->end_node);
    rob_osg->wind_direction = 0.0f;
    rob_osg->inertia = 0.0f;
    rob_osg->yz_order = 0;
    rob_osg->apply_physics = true;
    rob_osg->motion_reset_data.clear();
    rob_osg->move_cancel = 0.0f;
    rob_osg->move_cancelled = false;
    rob_osg->osage_reset = false;
    rob_osg->osage_reset_done = false;
    rob_osg->ring.ring_height = -1000.0f;
    rob_osg->ring.rect_x = 0.0f;
    rob_osg->ring.rect_y = 0.0f;
    rob_osg->ring.rect_width = 0.0f;
    rob_osg->ring.out_height = -1000.0f;
    rob_osg->ring.init = false;
    rob_osg->ring.rect_height = 0.0f;
    rob_osg->ring.coli_object.work_list.clear();
    rob_osg->ring.skp_root_coli.clear();
    rob_osg->disable_collision = false;
    skin_param_reset(&rob_osg->skin_param);
    rob_osg->skin_param_ptr = &rob_osg->skin_param;
    rob_osg->osage_setting.parts = ROB_OSAGE_PARTS_NONE;
    rob_osg->osage_setting.exf = 0;
    rob_osg->reset_data_list = 0;
    rob_osg->field_2A4 = 0.0f;
    rob_osg->field_2A1 = false;
    rob_osg->set_external_force = false;
    rob_osg->external_force = 0.0f;
    rob_osg->root_matrix_ptr = 0;
    rob_osg->root_matrix_prev = mat4_null;
}

void ExNodeBlock__CtrlBegin(ExNodeBlock* node) {
    node->done = false;
}

void ExOsageBlock__Init(ExOsageBlock* osg) {
    osg->base.__vftable->Reset(&osg->base);
}

// 0x14047EE90
static void RobOsage__ApplyResetData(RobOsage* rob_osg, const mat4& mat) {
    if (rob_osg->reset_data_list) {
        auto reset_data = rob_osg->reset_data_list->begin();
        RobOsageNode* i_begin = rob_osg->nodes.data() + 1;
        RobOsageNode* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
        for (RobOsageNode* i = i_begin; i != i_end; i++)
            i->reset_data = *reset_data++;
        rob_osg->reset_data_list = 0;
    }

    mat4 temp;
    mat4_transpose(&mat, &temp);
    RobOsageNode* i_begin = rob_osg->nodes.data() + 1;
    RobOsageNode* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++) {
        mat4_transform_vector(&temp, &i->reset_data.delta_pos, &i->delta_pos);
        mat4_transform_point(&temp, &i->reset_data.pos, &i->pos);
    }
    rob_osg->root_matrix_prev = *rob_osg->root_matrix_ptr;
}

// 0x14047F110
static void RobOsage__RotateMat(RobOsage* rob_osg, mat4& mat, const vec3& parent_scale, bool init_rot = false) {
    mat4_transpose(&mat, &mat);
    const vec3 position = rob_osg->exp_data.position * parent_scale;
    mat4_mul_translate(&mat, &position, &mat);
    mat4_mul_rotate_zyx(&mat, &rob_osg->exp_data.rotation, &mat);

    const skin_param* skin_param = rob_osg->skin_param_ptr;
    const vec3 rot = init_rot ? skin_param->init_rot + skin_param->rot : skin_param->rot;
    mat4_mul_rotate_zyx(&mat, &rot, &mat);
    mat4_transpose(&mat, &mat);
}

// 0x140482FF0
static bool rotate_matrix_to_direction(mat4& mat, const vec3& direction,
    const skin_param_hinge* hinge, vec3* rot, const int32_t& yz_order) {
    bool rot_clamped = false;
    float_t z_rot;
    float_t y_rot;
    mat4_transpose(&mat, &mat);
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
    mat4_transpose(&mat, &mat);

    if (rot) {
        rot->y = y_rot;
        rot->z = z_rot;
    }
    return rot_clamped;
}

// 0x1404803B0
static void RobOsage__BeginCalc(RobOsage* rob_osg, const mat4& root_matrix,
    const vec3& parent_scale, bool has_children_node) {
    mat4 mat = root_matrix;
    const vec3 pos = rob_osg->exp_data.position * parent_scale;
    mat4_transpose(&mat, &mat);
    mat4_transform_point(&mat, &pos, &rob_osg->nodes.data()[0].pos);

    if (rob_osg->osage_reset && !rob_osg->osage_reset_done) {
        rob_osg->osage_reset_done = true;
        RobOsage__ApplyResetData(rob_osg, root_matrix);
    }

    if (!rob_osg->move_cancelled) {
        rob_osg->move_cancelled = true;

        float_t move_cancel = rob_osg->skin_param_ptr->move_cancel;
        if (rob_osg->move_cancel == 1.0f || move_cancel < 0.0f)
            move_cancel = rob_osg->move_cancel;

        if (move_cancel > 0.0f) {
            RobOsageNode* i_begin = rob_osg->nodes.data() + 1;
            RobOsageNode* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
            for (RobOsageNode* i = i_begin; i != i_end; i++) {
                mat4 mat;
                vec3 pos;
                mat4_transpose(&rob_osg->root_matrix_prev, &mat);
                mat4_inverse_transform_point(&mat, &i->pos, &pos);
                mat4_transpose(rob_osg->root_matrix_ptr, &mat);
                mat4_transform_point(&mat, &pos, &pos);
                i->pos += (pos - i->pos) * move_cancel;
            }
        }
    }

    if (!has_children_node)
        return;

    mat4_transpose(&mat, &mat);
    RobOsage__RotateMat(rob_osg, mat, parent_scale);
    *rob_osg->nodes.data()[0].bone_node_mat = mat;
    mat4_transpose(&mat, &mat);

    RobOsageNode* v30_begin = rob_osg->nodes.data() + 1;
    RobOsageNode* v30_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (RobOsageNode* v30 = v30_begin; v30 != v30_end; v30++) {
        vec3 direction;
        mat4_inverse_transform_point(&mat, &v30->pos, &direction);

        mat4_transpose(&mat, &mat);
        bool rot_clamped = rotate_matrix_to_direction(mat, direction, &v30->data_ptr->skp_osg_node.hinge,
            &v30->reset_data.rotation, rob_osg->yz_order);
        *v30->bone_node_ptr->ex_data_mat = mat;
        mat4_transpose(&mat, &mat);

        v30->TranslateMat(mat, rot_clamped, parent_scale.x);
    }

    if (rob_osg->nodes.size() && rob_osg->end_node.bone_node_mat) {
        mat4 mat = *rob_osg->nodes.back().bone_node_ptr->ex_data_mat;
        mat4_transpose(&mat, &mat);
        mat4_mul_translate(&mat, parent_scale.x * rob_osg->end_node.length, 0.0f, 0.0f, &mat);
        mat4_transpose(&mat, &mat);
        *rob_osg->end_node.bone_node_ptr->ex_data_mat = mat;
    }
}

static void RobOsage__SetWindDirection(RobOsage* osg_block_data, vec3* wind_direction) {
    if (wind_direction)
        osg_block_data->wind_direction = *wind_direction;
    else
        osg_block_data->wind_direction = 0.0f;
}

static void ExOsageBlock__SetWindDirection(ExOsageBlock* osg) {
    vec3* (*task_wind_get_wind_direction)() = (vec3 * (*)())0x000000014053D560;

    vec3 wind_direction = *task_wind_get_wind_direction();
    wind_direction *= osg->base.item_equip_object->item_equip->wind_strength;
    RobOsage__SetWindDirection(&osg->rob, &wind_direction);
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

// 0x140482F30
static void segment_limit_distance(vec3& p0, const vec3& p1, const float_t max_distance) {
    const vec3 d = p0 - p1;
    float_t dist = vec3::length_squared(d);
    if (dist > max_distance * max_distance)
        p0 = p1 + d * (max_distance / sqrtf(dist));
}

// 0x140482490
static void RobOsageNode__CheckNodeDistance(RobOsageNode* node, const float_t& step, const float_t& parent_scale) {
    if (step != 1.0f) {
        vec3 d = node->pos - node->fixed_pos;

        float_t dist = vec3::length(d);
        if (dist != 0.0f)
            d *= 1.0f / dist;
        node->pos = node->fixed_pos + d * (step * dist);
    }

    segment_limit_distance(node->pos, node->GetPrevNode().pos, node->length * parent_scale);
    if (node->sibling_node)
        segment_limit_distance(node->pos, node->sibling_node->pos, node->max_distance);
}

// 0x140482180
void RobOsageNode__CheckFloorCollision(RobOsageNode* node, const float_t& floor_height) {
    float_t pos_y = node->data_ptr->skp_osg_node.coli_r + floor_height;
    if (pos_y <= node->pos.y)
        return;

    node->pos.y = pos_y;
    node->pos = node->GetPrevNode().pos + vec3::normalize(node->pos - node->GetPrevNode().pos) * node->length;
    node->delta_pos = 0.0f;
    node->hit += 1.0f;
}

// 0x14047C800
static void RobOsage__ApplyPhysics(RobOsage* rob_osg, const mat4& root_matrix, const vec3& parent_scale,
    float_t step, bool disable_external_force, bool ring_coli, bool has_children_node) {
    if (!rob_osg->nodes.size())
        return;

    const float_t osage_gravity_const = get_osage_gravity_const();
    RobOsage__BeginCalc(rob_osg, root_matrix, parent_scale, false);

    RobOsageNode* node = &rob_osg->nodes.data()[0];
    node->fixed_pos = node->pos;
    const vec3 v113 = rob_osg->exp_data.position * parent_scale;

    mat4 v130 = root_matrix;
    mat4_transpose(&v130, &v130);
    mat4_transform_point(&v130, &v113, &node->pos);
    node->delta_pos = node->pos - node->fixed_pos;

    mat4_transpose(&v130, &v130);
    RobOsage__RotateMat(rob_osg, v130, parent_scale);
    *rob_osg->nodes.data()[0].bone_node_mat = v130;
    *rob_osg->nodes.data()[0].bone_node_ptr->ex_data_mat = v130;
    mat4_transpose(&v130, &v130);

    vec3 direction = { 1.0f, 0.0f, 0.0f };
    mat4_transform_vector(&v130, &direction, &direction);

    if (!rob_osg->osage_reset && step <= 0.0f)
        return;

    const bool stiffness = rob_osg->skin_param_ptr->stiffness > 0.0f;

    RobOsageNode* v26_begin = rob_osg->nodes.data() + 1;
    RobOsageNode* v26_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (RobOsageNode* v26 = v26_begin; v26 != v26_end; v26++) {
        float_t weight = v26->data_ptr->skp_osg_node.weight;
        vec3 force;
        if (!rob_osg->set_external_force) {
            apply_gravity(force, v26->pos, v26->GetPrevNode().pos, osage_gravity_const, weight);

            if (v26 != v26_end - 1) {
                vec3 _force;
                apply_gravity(_force, v26->pos, v26->GetNextNode().pos, osage_gravity_const, weight);
                force = (force + _force) * 0.5f;
            }
        }
        else
            force = rob_osg->external_force * (1.0f / weight);

        const vec3 _direction = direction * (v26->data_ptr->force * v26->force);
        const float_t fric = (1.0f - rob_osg->inertia) * (1.0f - rob_osg->skin_param_ptr->air_res);

        vec3 vel = force + _direction - v26->delta_pos * fric + v26->external_force * weight;

        if (!disable_external_force)
            vel += rob_osg->wind_direction * rob_osg->skin_param_ptr->wind_afc;

        if (stiffness)
            vel -= (v26->delta_pos - v26->GetPrevNode().delta_pos) * (1.0f - rob_osg->skin_param_ptr->air_res);

        v26->vel = vel * (1.0f / (weight - (weight - 1.0f) * v26->data_ptr->skp_osg_node.inertial_cancel));
    }

    if (stiffness) {
        mat4 v131 = v130;
        vec3 v111;
        mat4_get_translation(&v131, &v111);

        RobOsageNode* v55_begin = rob_osg->nodes.data() + 1;
        RobOsageNode* v55_end = rob_osg->nodes.data() + rob_osg->nodes.size();
        for (RobOsageNode* v55 = v55_begin; v55 != v55_end; v55++) {
            vec3 v128;
            mat4_transform_point(&v131, &v55->rel_pos, &v128);

            const vec3 delta_pos = v55->delta_pos + v55->vel;
            vec3 v126 = v55->pos + delta_pos;
            segment_limit_distance(v126, v111, v55->length * parent_scale.x);

            vec3 v117 = (v128 - v126) * rob_osg->skin_param_ptr->stiffness;
            const float_t weight = v55->data_ptr->skp_osg_node.weight;
            v117 *= 1.0f / (weight - (weight - 1.0f) * v55->data_ptr->skp_osg_node.inertial_cancel);
            v55->vel += v117;

            v126 = v55->pos + delta_pos + v117;

            vec3 direction;
            mat4_inverse_transform_point(&v131, &v126, &direction);

            mat4_transpose(&v131, &v131);
            rotate_matrix_to_direction(v131, direction, 0, 0, rob_osg->yz_order);
            mat4_transpose(&v131, &v131);

            mat4_mul_translate(&v131, vec3::distance(v111, v126), 0.0f, 0.0f, &v131);

            v111 = v126;
        }
    }

    RobOsageNode* v82_begin = rob_osg->nodes.data() + 1;
    RobOsageNode* v82_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (RobOsageNode* v82 = v82_begin; v82 != v82_end; v82++) {
        v82->fixed_pos = v82->pos;
        v82->delta_pos += v82->vel;
        v82->pos += v82->delta_pos;
    }

    if (rob_osg->nodes.size() > 1) {
        RobOsageNode* v90_begin = rob_osg->nodes.data() + rob_osg->nodes.size() - 2;
        RobOsageNode* v90_end = rob_osg->nodes.data();
        for (RobOsageNode* v90 = v90_begin; v90 != v90_end; v90--)
            segment_limit_distance(v90->pos, v90->GetNextNode().pos, v90->child_length * parent_scale.x);
    }

    if (ring_coli) {
        RobOsageNode* node = &rob_osg->nodes.data()[0];
        const float_t floor_height = rob_osg->ring.get_floor_height(
            node->pos, node->data_ptr->skp_osg_node.coli_r);

        RobOsageNode* v98_begin = rob_osg->nodes.data() + 1;
        RobOsageNode* v98_end = rob_osg->nodes.data() + rob_osg->nodes.size();
        for (RobOsageNode* v98 = v98_begin; v98 != v98_end; v98++) {
            RobOsageNode__CheckNodeDistance(v98, step, parent_scale.x);
            RobOsageNode__CheckFloorCollision(v98, floor_height);
        }
    }

    if (has_children_node) {
        RobOsageNode* v99_begin = rob_osg->nodes.data() + 1;
        RobOsageNode* v99_end = rob_osg->nodes.data() + rob_osg->nodes.size();
        for (RobOsageNode* v99 = v99_begin; v99 != v99_end; v99++) {
            vec3 direction;
            mat4_inverse_transform_point(&v130, &v99->pos, &direction);

            mat4_transpose(&v130, &v130);
            bool rot_clamped = rotate_matrix_to_direction(v130, direction,
                &v99->data_ptr->skp_osg_node.hinge,
                &v99->reset_data.rotation, rob_osg->yz_order);
            *v99->bone_node_ptr->ex_data_mat = v130;
            mat4_transpose(&v130, &v130);

            v99->TranslateMat(v130, rot_clamped, parent_scale.x);
        }

        if (rob_osg->nodes.size() && rob_osg->end_node.bone_node_mat) {
            mat4 v131 = *rob_osg->nodes.back().bone_node_ptr->ex_data_mat;
            mat4_transpose(&v131, &v131);
            mat4_mul_translate(&v131, rob_osg->end_node.length * parent_scale.x, 0.0f, 0.0f, &v131);
            mat4_transpose(&v131, &v131);
            *rob_osg->end_node.bone_node_ptr->ex_data_mat = v131;
        }
    }
    rob_osg->apply_physics = false;
}

static void RobOsage__ColiSet(RobOsage* rob_osg, const mat4* transform) {
    if (rob_osg->skin_param_ptr->coli.size())
        OsageCollision::Work::update_cls_work(rob_osg->coli_chara, rob_osg->skin_param_ptr->coli.data(), transform);
    OsageCollision::Work::update_cls_work(rob_osg->coli_ring, rob_osg->ring.skp_root_coli, transform);
}

// 0x14047ECA0
static void RobOsage__CollideNodes(RobOsage* rob_osg, float_t step) {
    if (step <= 0.0f)
        return;

    const OsageCollision::Work* coli_chara = rob_osg->coli_chara;
    const OsageCollision::Work* coli_ring = rob_osg->coli_ring;
    const OsageCollision& coli_object = rob_osg->ring.coli_object;

    RobOsageNode* i_begin = rob_osg->nodes.data() + 1;
    RobOsageNode* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();

    for (RobOsageNode* i = i_begin; i != i_end; i++) {
        RobOsageNodeData* data = i->data_ptr;
        i->hit += (float_t)OsageCollision::osage_cls_work_list(i->pos,
            data->skp_osg_node.coli_r, coli_object, &i->friction);
        if (rob_osg->disable_collision)
            continue;

        for (RobOsageNode*& j : data->boc) {
            j->hit += (float_t)OsageCollision::osage_cls(coli_ring, j->pos, data->skp_osg_node.coli_r);
            j->hit += (float_t)OsageCollision::osage_cls(coli_chara, j->pos, data->skp_osg_node.coli_r);
        }

        i->hit += (float_t)OsageCollision::osage_cls(coli_ring, i->pos, data->skp_osg_node.coli_r);
        i->hit += (float_t)OsageCollision::osage_cls(coli_chara, i->pos, data->skp_osg_node.coli_r);
    }
}

static void RobOsage__ApplyBocRootColi(RobOsage* rob_osg, float_t step) {
    if (step < 0.0f && rob_osg->disable_collision)
        return;

    prj::vector<RobOsageNode>& nodes = rob_osg->nodes;
    const vec3 pos = nodes.data()[0].pos;
    const OsageCollision::Work* coli_chara = rob_osg->coli_chara;
    const OsageCollision::Work* coli_ring = rob_osg->coli_ring;
    const SkinParam::RootCollisionType coli_type = rob_osg->skin_param_ptr->coli_type;

    RobOsageNode* i_begin = nodes.data() + 1;
    RobOsageNode* i_end = nodes.data() + nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++) {
        RobOsageNodeData* data = i->data_ptr;
        for (RobOsageNode*& j : data->boc) {
            float_t hit = (float_t)(
                OsageCollision::osage_capsule_cls(coli_ring, i->pos, j->pos, data->skp_osg_node.coli_r)
                + OsageCollision::osage_capsule_cls(coli_chara, i->pos, j->pos, data->skp_osg_node.coli_r));
            i->hit += hit;
            j->hit += hit;
        }

        if (coli_type != SkinParam::RootCollisionTypeEnd
            && (coli_type != SkinParam::RootCollisionTypeCapsule || i != i_begin)) {
            float_t hit = (float_t)(
                OsageCollision::osage_capsule_cls(coli_ring, i->pos, i->GetPrevNode().pos, data->skp_osg_node.coli_r)
                + OsageCollision::osage_capsule_cls(coli_chara, i->pos, i->GetPrevNode().pos, data->skp_osg_node.coli_r));
            i->hit += hit;
            i->GetPrevNode().hit += hit;
        }
    }
    nodes.data()[0].pos = pos;
}

static void RobOsage__CollideNodesTargetOsage(RobOsage* rob_osg, const mat4& root_matrix,
    const vec3& parent_scale, float_t step, bool collide_nodes) {
    if (!rob_osg->osage_reset && step <= 0.0f)
        return;

    const OsageCollision::Work* coli_chara = rob_osg->coli_chara;
    const OsageCollision::Work* coli_ring = rob_osg->coli_ring;
    const OsageCollision& coli_object = rob_osg->ring.coli_object;

    const float_t inv_step = step > 0.0f ? 1.0f / step : 0.0f;

    mat4 v64;
    if (collide_nodes)
        v64 = *rob_osg->nodes.data()[0].bone_node_mat;
    else {
        v64 = root_matrix;

        const vec3 trans = rob_osg->exp_data.position * parent_scale;
        mat4_transpose(&v64, &v64);
        mat4_transform_point(&v64, &trans, &rob_osg->nodes.data()[0].pos);
        mat4_transpose(&v64, &v64);
        RobOsage__RotateMat(rob_osg, v64, parent_scale);
    }
    mat4_transpose(&v64, &v64);

    float_t floor_height = -1000.0f;
    if (collide_nodes) {
        RobOsageNode* node = &rob_osg->nodes.data()[0];
        floor_height = rob_osg->ring.get_floor_height(
            node->pos, node->data_ptr->skp_osg_node.coli_r);
    }

    const float_t v23 = step < 1.0f ? 0.2f / (2.0f - step) : 0.2f;

    if (rob_osg->skin_param_ptr->colli_tgt_osg) {
        prj::vector<RobOsageNode>* colli_tgt_osg = rob_osg->skin_param_ptr->colli_tgt_osg;
        RobOsageNode* i_begin = rob_osg->nodes.data() + 1;
        RobOsageNode* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
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

    RobOsageNode* i_begin = rob_osg->nodes.data() + 1;
    RobOsageNode* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++) {
        const float_t fric = (1.0f - rob_osg->inertia) * rob_osg->skin_param_ptr->friction;
        if (collide_nodes) {
            RobOsageNode__CheckNodeDistance(i, step, parent_scale.x);
            i->hit = (float_t)OsageCollision::osage_cls_work_list(i->pos,
                i->data_ptr->skp_osg_node.coli_r, coli_object, &i->friction);
            if (!rob_osg->disable_collision) {
                i->hit += (float_t)OsageCollision::osage_cls(coli_ring,
                    i->pos, i->data_ptr->skp_osg_node.coli_r);
                i->hit += (float_t)OsageCollision::osage_cls(coli_chara,
                    i->pos, i->data_ptr->skp_osg_node.coli_r);
            }
            RobOsageNode__CheckFloorCollision(i, floor_height);
        }
        else
            segment_limit_distance(i->pos, i->GetPrevNode().pos, i->length * parent_scale.x);

        vec3 direction;
        mat4_inverse_transform_point(&v64, &i->pos, &direction);
        mat4_transpose(&v64, &v64);

        bool rot_clamped = rotate_matrix_to_direction(v64, direction,
            &i->data_ptr->skp_osg_node.hinge,
            &i->reset_data.rotation, rob_osg->yz_order);
        i->bone_node_ptr->exp_data.parent_scale = parent_scale;
        *i->bone_node_ptr->ex_data_mat = v64;
        mat4_transpose(&v64, &v64);

        if (i->bone_node_mat) {
            mat4* v41 = i->bone_node_mat;
            mat4_scale_rot(&v64, &parent_scale, v41);
            mat4_transpose(v41, v41);
        }

        i->reset_data.length = i->TranslateMat(v64, rot_clamped, parent_scale.x);

        i->delta_pos = (i->pos - i->fixed_pos) * inv_step;

        if (i->hit > 0.0f)
            i->delta_pos *= min_def(fric, i->friction);

        const float_t v55 = vec3::length_squared(i->delta_pos);
        if (v55 > v23 * v23)
            i->delta_pos *= v23 / sqrtf(v55);

        mat4 v65;
        mat4_transpose(&root_matrix, &v65);
        mat4_inverse_transform_point(&v65, &i->pos, &i->reset_data.pos);
        mat4_inverse_transform_vector(&v65, &i->delta_pos, &i->reset_data.delta_pos);
    }

    if (rob_osg->nodes.size() && rob_osg->end_node.bone_node_mat) {
        mat4 mat = *rob_osg->nodes.back().bone_node_ptr->ex_data_mat;
        mat4_transpose(&mat, &mat);
        mat4_mul_translate(&mat, rob_osg->end_node.length * parent_scale.x, 0.0f, 0.0f, &mat);
        mat4_transpose(&mat, &mat);
        *rob_osg->end_node.bone_node_ptr->ex_data_mat = mat;
        mat4_transpose(&mat, &mat);
        mat4_scale_rot(&mat, &parent_scale, &mat);
        mat4_transpose(&mat, &mat);
        *rob_osg->end_node.bone_node_mat = mat;
        rob_osg->end_node.bone_node_ptr->exp_data.parent_scale = parent_scale;
    }
}

// 0x140480F40
static void RobOsage__set_nodes_external_force(RobOsage* rob_osg, vec3* external_force, float_t strength) {
    vec3 v4;
    if (external_force)
        v4 = *external_force;
    else
        v4 = 0.0f;

    size_t exf = rob_osg->osage_setting.exf;
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

    RobOsageNode* i_begin = rob_osg->nodes.data() + 1;
    RobOsageNode* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++) {
        i->external_force = v4;
        v4 *= strength;
    }
}

// 0x140481540
static void RobOsage__set_nodes_force(RobOsage* rob_osg, float_t force) {
    RobOsageNode* i_begin = rob_osg->nodes.data() + 1;
    RobOsageNode* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++)
        i->force = force;
}

// 0x140480260
static void RobOsage__EndCalc(RobOsage* rob_osg, const mat4& root_matrix,
    const vec3& parent_scale, float_t step, bool disable_external_force) {
    if (!disable_external_force) {
        RobOsage__set_nodes_external_force(rob_osg, 0, 1.0f);
        RobOsage__set_nodes_force(rob_osg, 1.0f);
        rob_osg->set_external_force = false;
        rob_osg->external_force = 0.0f;
    }

    RobOsageNode* i_begin = rob_osg->nodes.data() + 1;
    RobOsageNode* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++) {
        i->hit = 0.0f;
        i->friction = 1.0f;
    }

    rob_osg->apply_physics = true;
    rob_osg->field_2A1 = false;
    rob_osg->field_2A4 = -1.0f;
    rob_osg->move_cancelled = false;
    rob_osg->osage_reset = false;
    rob_osg->osage_reset_done = false;
    rob_osg->root_matrix_prev = *rob_osg->root_matrix_ptr;
}

void ExOsageBlock__CtrlStep(ExOsageBlock* osg, int32_t stage, bool disable_external_force) {
    float_t(*get_delta_frame)() = (float_t(*)())0x0000000140192D50;

    rob_chara_item_equip* rob_item_equip = osg->base.item_equip_object->item_equip;
    float_t step = get_delta_frame() * rob_item_equip->step;
    if (rob_item_equip->opd_blend_data.size() && rob_item_equip->opd_blend_data.front().use_blend)
        step = 1.0f;

    bone_node* parent_node = osg->base.parent_bone_node;
    const mat4& root_matrix = *parent_node->ex_data_mat;
    const vec3 parent_scale = parent_node->exp_data.parent_scale;
    switch (stage) {
    case 0:
        RobOsage__BeginCalc(&osg->rob, root_matrix, parent_scale, osg->base.has_children_node);
        break;
    case 1:
    case 2:
        if ((stage == 1 && osg->base.is_parent) || (stage == 2 && osg->rob.apply_physics)) {
            ExOsageBlock__SetWindDirection(osg);
            RobOsage__ApplyPhysics(&osg->rob, root_matrix,
                parent_scale, step, disable_external_force, true, osg->base.has_children_node);
        }
        break;
    case 3:
        RobOsage__ColiSet(&osg->rob, osg->mats);
        RobOsage__CollideNodes(&osg->rob, step);
        break;
    case 4:
        RobOsage__ApplyBocRootColi(&osg->rob, step);
        break;
    case 5:
        RobOsage__CollideNodesTargetOsage(&osg->rob, root_matrix, parent_scale, step, false);
        RobOsage__EndCalc(&osg->rob, root_matrix, parent_scale, step, disable_external_force);
        osg->base.done = true;
        break;
    }
}

// 0x14047C770
static void RobOsage__CtrlInitMain(RobOsage* rob_osg, const mat4& root_matrix,
    const vec3& parent_scale, float_t step, bool disable_external_force) {
    RobOsage__ApplyPhysics(rob_osg, root_matrix, parent_scale, step, disable_external_force, false, false);
    RobOsage__CollideNodesTargetOsage(rob_osg, root_matrix, parent_scale, step, true);
    RobOsage__EndCalc(rob_osg, root_matrix, parent_scale, step, disable_external_force);
}

// 0x14047C750
static void RobOsage__CtrlMain(RobOsage* rob_osg, const mat4& root_matrix, const vec3& parent_scale, float_t step) {
    RobOsage__CtrlInitMain(rob_osg, root_matrix, parent_scale, step, false);
}

void ExOsageBlock__CtrlMain(ExOsageBlock* osg) {
    osg->field_1FF8 &= ~2;
    if (osg->base.done) {
        osg->base.done = false;
        return;
    }

    float_t(*get_delta_frame)() = (float_t(*)())0x0000000140192D50;

    rob_chara_item_equip* rob_item_equip = osg->base.item_equip_object->item_equip;
    float_t step = get_delta_frame() * rob_item_equip->step;
    if (rob_item_equip->opd_blend_data.size() && rob_item_equip->opd_blend_data.front().use_blend)
        step = 1.0f;

    bone_node* parent_node = osg->base.parent_bone_node;
    const vec3 parent_scale = parent_node->exp_data.parent_scale;
    vec3 scale = parent_node->exp_data.scale;

    mat4 mat = *parent_node->ex_data_mat;
    if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f) {
        scale = 1.0f / scale;
        mat4_transpose(&mat, &mat);
        mat4_scale_rot(&mat, &scale, &mat);
        mat4_transpose(&mat, &mat);
    }
    ExOsageBlock__SetWindDirection(osg);

    RobOsage__ColiSet(&osg->rob, osg->mats);
    RobOsage__CtrlMain(&osg->rob, mat, parent_scale, step);
}

// 0x14047E240
void RobOsage__ctrl_osage_play_data(RobOsage* rob_osg, mat4* root_matrix,
    const vec3& parent_scale, prj::vector<opd_blend_data>* opd_blend_data) {
    if (!opd_blend_data->size())
        return;

    const vec3 v63 = rob_osg->exp_data.position * parent_scale;

    mat4 v85;
    mat4_transpose(root_matrix, &v85);
    mat4_transform_point(&v85, &v63, &rob_osg->nodes.data()[0].pos);
    mat4_transpose(&v85, &v85);

    RobOsage__RotateMat(rob_osg, v85, parent_scale, false);
    *rob_osg->nodes.data()[0].bone_node_mat = v85;
    *rob_osg->nodes.data()[0].bone_node_ptr->ex_data_mat = v85;
    mat4_transpose(&v85, &v85);

    mat4_transpose(root_matrix, root_matrix);
    ::opd_blend_data* i_begin = opd_blend_data->data() + opd_blend_data->size();
    ::opd_blend_data* i_end = opd_blend_data->data();
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
        vec3 parent_curr_pos = rob_osg->nodes.data()[0].pos;
        vec3 parent_next_pos = rob_osg->nodes.data()[0].pos;

        RobOsageNode* j_begin = rob_osg->nodes.data() + 1;
        RobOsageNode* j_end = rob_osg->nodes.data() + rob_osg->nodes.size();
        for (RobOsageNode* j = j_begin; j != j_end; j++) {
            opd_vec3_data* opd = &j->opd_data[i - i_end];
            const float_t* opd_x = opd->x;
            const float_t* opd_y = opd->y;
            const float_t* opd_z = opd->z;

            vec3 curr_pos;
            curr_pos.x = opd_x[curr_key];
            curr_pos.y = opd_y[curr_key];
            curr_pos.z = opd_z[curr_key];

            vec3 next_pos;
            next_pos.x = opd_x[next_key];
            next_pos.y = opd_y[next_key];
            next_pos.z = opd_z[next_key];

            mat4_transform_point(root_matrix, &curr_pos, &curr_pos);
            mat4_transform_point(root_matrix, &next_pos, &next_pos);

            vec3 _trans = curr_pos * inv_blend + next_pos * blend;

            vec3 direction;
            mat4_inverse_transform_point(&v87, &_trans, &direction);
            mat4_transpose(&v87, &v87);

            vec3 rotation = 0.0f;
            rotate_matrix_to_direction(v87, direction, 0, &rotation, rob_osg->yz_order);
            mat4_transpose(&v87, &v87);
            mat4_set_translation(&v87, &_trans);

            float_t length = vec3::distance(curr_pos, parent_curr_pos) * inv_blend
                + vec3::distance(next_pos, parent_next_pos) * blend;
            j->opd_node_data.set_data(i, { length, rotation });

            parent_curr_pos = curr_pos;
            parent_next_pos = next_pos;
        }
    }
    mat4_transpose(root_matrix, root_matrix);

    RobOsageNode* j_begin = rob_osg->nodes.data() + 1;
    RobOsageNode* j_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (RobOsageNode* j = j_begin; j != j_end; j++) {
        float_t v54 = j->opd_node_data.curr.rotation.y;
        float_t v55 = j->opd_node_data.curr.rotation.z;
        v54 = clamp_def(v54, (float_t)-M_PI, (float_t)M_PI);
        v55 = clamp_def(v55, (float_t)-M_PI, (float_t)M_PI);
        mat4_mul_rotate_z(&v85, v55, &v85);
        mat4_mul_rotate_y(&v85, v54, &v85);
        mat4_transpose(&v85, &v85);
        j->bone_node_ptr->exp_data.parent_scale = parent_scale;
        *j->bone_node_ptr->ex_data_mat = v85;
        mat4_transpose(&v85, &v85);
        if (j->bone_node_mat) {
            mat4 mat = v85;
            mat4_scale_rot(&mat, &parent_scale, &mat);
            mat4_transpose(&mat, j->bone_node_mat);
        }
        mat4_mul_translate(&v85, j->opd_node_data.curr.length, 0.0f, 0.0f, &v85);
        j->fixed_pos = j->pos;
        mat4_get_translation(&v85, &j->pos);
    }

    if (rob_osg->nodes.size() && rob_osg->end_node.bone_node_mat) {
        mat4 v87 = *rob_osg->nodes.back().bone_node_ptr->ex_data_mat;
        mat4_transpose(&v87, &v87);
        mat4_mul_translate(&v87, rob_osg->end_node.length * parent_scale.x, 0.0f, 0.0f, &v87);
        mat4_transpose(&v87, &v87);
        *rob_osg->end_node.bone_node_ptr->ex_data_mat = v87;

        mat4_transpose(&v87, &v87);
        mat4_scale_rot(&v87, &parent_scale, &v87);
        mat4_transpose(&v87, &v87);
        *rob_osg->end_node.bone_node_mat = v87;
        rob_osg->end_node.bone_node_ptr->exp_data.parent_scale = parent_scale;
    }
}

void ExOsageBlock__CtrlOsagePlayData(ExOsageBlock* osg) {
    rob_chara_item_equip* rob_itm_equip = osg->base.item_equip_object->item_equip;
    bone_node* parent_node = osg->base.parent_bone_node;
    const vec3 parent_scale = parent_node->exp_data.parent_scale;
    RobOsage__ctrl_osage_play_data(&osg->rob, parent_node->ex_data_mat,
        parent_scale, &rob_itm_equip->opd_blend_data);
}

void ExOsageBlock__Disp(ExOsageBlock* osg) {

}

void ExOsageBlock__Reset(ExOsageBlock* osg) {
    osg->index = 0;
    RobOsage__Reset(&osg->rob);
    osg->field_1FF8 &= ~3;
    osg->mats = 0;
    osg->step = 1.0f;
    osg->base.bone_node_ptr = 0;
}

void ExOsageBlock__Field_40(ExOsageBlock* osg) {

}

void ExOsageBlock__CtrlInitBegin(ExOsageBlock* osg) {
    void (*RobOsage__CtrlInitBegin) (RobOsage * This, mat4 * root_matrix, const vec3 & parent_scale, bool a4)
        = (void (*) (RobOsage*, mat4*, const vec3&, bool))0x000000014047F990;

    osg->step = 4.0f;
    ExOsageBlock__SetWindDirection(osg);
    RobOsage__ColiSet(&osg->rob, osg->mats);
    bone_node* parent_node = osg->base.parent_bone_node;
    const vec3 parent_scale = parent_node->exp_data.parent_scale * 0.5f;
    RobOsage__CtrlInitBegin(&osg->rob, parent_node->ex_data_mat, parent_scale, false);
    osg->field_1FF8 &= ~2;
}

void ExOsageBlock__CtrlInitMain(ExOsageBlock* osg) {
    if (osg->base.done) {
        osg->base.done = 0;
        return;
    }

    ExOsageBlock__SetWindDirection(osg);

    bone_node* parent_node = osg->base.parent_bone_node;
    const vec3 parent_scale = parent_node->exp_data.parent_scale;
    RobOsage__ColiSet(&osg->rob, osg->mats);
    RobOsage__CtrlInitMain(&osg->rob, *parent_node->ex_data_mat, parent_scale, osg->step, true);
    float_t step = 0.5f * osg->step;
    osg->step = max_def(step, 1.0f);
}

// 0x14053D1B0
static bool RobOsageNodeDataNormalRef__GetAxes(const vec3& l_trans, const vec3& r_trans,
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

// 0x14053CE30
static void RobOsageNodeDataNormalRef__GetMat(RobOsageNodeDataNormalRef* normal_ref, mat4* mat) {
    if (!normal_ref->set)
        return;

    mat4 _mat;
    vec3 n_trans;
    vec3 u_trans;
    vec3 d_trans;
    vec3 l_trans;
    vec3 r_trans;
    mat4_transpose(normal_ref->n->bone_node_ptr->ex_data_mat, &_mat);
    mat4_get_translation(&_mat, &n_trans);
    mat4_transpose(normal_ref->u->bone_node_ptr->ex_data_mat, &_mat);
    mat4_get_translation(&_mat, &u_trans);
    mat4_transpose(normal_ref->d->bone_node_ptr->ex_data_mat, &_mat);
    mat4_get_translation(&_mat, &d_trans);
    mat4_transpose(normal_ref->l->bone_node_ptr->ex_data_mat, &_mat);
    mat4_get_translation(&_mat, &l_trans);
    mat4_transpose(normal_ref->r->bone_node_ptr->ex_data_mat, &_mat);
    mat4_get_translation(&_mat, &r_trans);

    vec3 z_axis;
    vec3 y_axis;
    vec3 x_axis;
    if (RobOsageNodeDataNormalRef__GetAxes(
        l_trans, r_trans, u_trans, d_trans, z_axis, y_axis, x_axis)) {
        mat4 temp = mat4(
            x_axis.x, x_axis.y, x_axis.z, 0.0f,
            y_axis.x, y_axis.y, y_axis.z, 0.0f,
            z_axis.x, z_axis.y, z_axis.z, 0.0f,
            n_trans.x, n_trans.y, n_trans.z, 1.0f);

        mat4 _mat = normal_ref->mat;
        mat4_transpose(&_mat, &_mat);
        mat4_mul(&_mat, &temp, mat);
        mat4_transpose(mat, mat);
    }
}

// 0x14047E1C0
static void RobOsage__CtrlEnd(RobOsage* rob_osg, const vec3& parent_scale) {
    RobOsageNode* i_begin = rob_osg->nodes.data() + 1;
    RobOsageNode* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++) {
        if (!i->data_ptr->normal_ref.set)
            continue;

        RobOsageNodeDataNormalRef__GetMat(&i->data_ptr->normal_ref, i->bone_node_mat);

        mat4 mat;
        mat4_transpose(i->bone_node_mat, &mat);
        mat4_scale_rot(&mat, &parent_scale, &mat);
        mat4_transpose(&mat, i->bone_node_mat);
    }
}

void ExOsageBlock__CtrlEnd(ExOsageBlock* osg) {
    bone_node* parent_node = osg->base.parent_bone_node;
    const vec3 parent_scale = parent_node->exp_data.parent_scale;
    RobOsage__CtrlEnd(&osg->rob, parent_scale);
}
