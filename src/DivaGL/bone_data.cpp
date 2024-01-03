/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "bone_data.hpp"

ExNodeBlock_vtbl* ExClothBlock_vftable = (ExNodeBlock_vtbl*)0x0000000140A82FF0;
ExNodeBlock_vtbl* ExOsageBlock_vftable = (ExNodeBlock_vtbl*)0x0000000140A82F88;
ExNodeBlock_vtbl* ExNodeBlock_vftable = (ExNodeBlock_vtbl*)0x0000000140A82DC8;

static void* (*operator_new)(size_t) = (void* (*)(size_t))0x000000014084530C;
static void(*operator_delete)(void*) = (void(*)(void*))0x0000000140845378;

static const float_t get_osage_gravity_const() {
    return 0.00299444468691945f;
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

        v14 = v13 - (((float_t*)&cls->pos[0])[i] + ((float_t*)&cls->pos[1])[i]);
        ((float_t*)&v29)[i] = v14;
        if (v14 > r)
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
            if (((float_t*)&v28)[i] > v25) {
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
    if (fabsf(v19) <= 0.000001)
        return OsageCollision::cls_ball_oidashi(vec, p, cls->pos[0], r);
    else if (v17 > v19)
        return OsageCollision::cls_ball_oidashi(vec, p, cls->pos[1], r);

    float_t v20 = vec3::length_squared(v11);
    float_t v22 = fabsf(v20 - v17 * v17 / v19);
    if (v22 > 0.000001f && v22 < r * r) {
        vec = (v11 - cls->vec_center * (v17 / v19)) * (r / sqrtf(v22) - 1.0f);
        return 1;
    }
    return 0;
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

    if (v20 != 0.0f)
        v54 *= 1.0f / v20;

    if (v21 != 0.0f)
        v57 *= 1.0f / v21;

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
        if (v25 != 0.0)
            v56 *= 1.0f / v25;

        float_t v30 = 1.0f / cls->vec_center_length;
        vec3 v61 = cls->vec_center * v30;
        float_t v34 = vec3::dot(v61, v56);
        float_t v35 = vec3::dot(v61 * v34 - v56, cls->pos[0] - p0) / (v34 * v34 - 1.0f);
        if (v35 < 0.0f)
            vec = p0;
        else if (v35 <= v25)
            vec = p0 + v56 * v35;
        else
            vec = p1;
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
    float_t v5 = vec3::dot(p, p2) - vec3::dot(p1, p2) - r;
    if (v5 < 0.0f) {
        vec = p2 * -v5;
        return 1;
    }
    return 0;
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

// 0x140484FE0
int32_t OsageCollision::osage_capsule_cls(const OsageCollision::Work* cls, vec3& p0, vec3& p1, const float_t& cls_r) {
    return OsageCollision::osage_capsule_cls(p0, p1, cls_r, cls);
}

// 0x140485000
int32_t OsageCollision::osage_capsule_cls(vec3& p0, vec3& p1, const float_t& cls_r, const OsageCollision::Work* cls) {
    if (!cls)
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

        if (fric && v11 > 0 && *fric > cls->friction)
            *fric = cls->friction;

        p += vec;
        v8 += v11;
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

static void skin_param_hinge_limit(skin_param_hinge* hinge) {
    hinge->ymin = max_def(hinge->ymin, -179.0f) * DEG_TO_RAD_FLOAT;
    hinge->ymax = min_def(hinge->ymax, 179.0f) * DEG_TO_RAD_FLOAT;
    hinge->zmin = max_def(hinge->zmin, -179.0f) * DEG_TO_RAD_FLOAT;
    hinge->zmax = min_def(hinge->zmax, 179.0f) * DEG_TO_RAD_FLOAT;
}

static void RobOsage__node_data_init(RobOsageNodeData *node_data) {
    node_data->force = 0.0f;
    node_data->boc.clear();
    node_data->skp_osg_node.coli_r = 0.0f;
    node_data->skp_osg_node.weight = 1.0f;
    node_data->skp_osg_node.inertial_cancel = 0.0f;
    node_data->skp_osg_node.hinge = { -90.0f, 90.0f, -90.0f, 90.0f };
    skin_param_hinge_limit(&node_data->skp_osg_node.hinge);
    node_data->normal_ref.field_0 = 0;
    node_data->normal_ref.n = 0;
    node_data->normal_ref.u = 0;
    node_data->normal_ref.d = 0;
    node_data->normal_ref.l = 0;
    node_data->normal_ref.r = 0;
    node_data->normal_ref.mat = mat4_identity;
}

static void RobOsageNode__Reset(RobOsageNode* node) {
    void (*sub_14021DCC0)(prj::vector<opd_vec3_data> *vec, size_t size)
        = (void (*)(prj::vector<opd_vec3_data>*, size_t))0x000000014021DCC0;
    node->length = 0.0f;
    node->trans = 0.0f;
    node->trans_orig = 0.0f;
    node->trans_diff = 0.0f;
    node->field_28 = 0.0f;
    node->child_length = 0.0f;
    node->bone_node_ptr = 0;
    node->bone_node_mat = 0;
    node->sibling_node = 0;
    node->max_distance = 0.0f;
    node->field_94 = 0.0f;
    node->reset_data.trans = 0.0f;
    node->reset_data.trans_diff = 0.0f;
    node->reset_data.rotation = 0.0f;
    node->reset_data.length = 0.0f;
    node->field_C8 = 0.0f;
    node->field_CC = 1.0f;
    node->external_force = 0.0f;
    node->force = 1.0f;
    RobOsage__node_data_init(&node->data);
    node->data_ptr = &node->data;
    node->opd_data.clear();
    node->opd_data.resize(3);
    node->mat = mat4_null;
}

static void skin_param_init(skin_param* skp) {

}

static void RobOsage__Reset(RobOsage* rob_osg) {
    rob_osg->nodes.clear();
    RobOsageNode__Reset(&rob_osg->node);
    rob_osg->wind_direction = 0.0f;
    rob_osg->field_1EB4 = 0.0f;
    rob_osg->yz_order = 0;
    rob_osg->field_2A0 = true;
    rob_osg->motion_reset_data.clear();
    rob_osg->move_cancel = 0.0f;
    rob_osg->field_1F0C = false;
    rob_osg->osage_reset = false;
    rob_osg->prev_osage_reset = false;
    rob_osg->ring.ring_height = -1000.0f;
    rob_osg->ring.ring_rectangle_x = 0.0f;
    rob_osg->ring.ring_rectangle_y = 0.0f;
    rob_osg->ring.ring_rectangle_width = 0.0f;
    rob_osg->ring.ring_out_height = -1000.0f;
    rob_osg->ring.init = false;
    rob_osg->ring.ring_rectangle_height = 0.0f;
    rob_osg->ring.coli.work_list.clear();
    rob_osg->ring.skp_root_coli.clear();
    rob_osg->disable_collision = false;
    skin_param_init(&rob_osg->skin_param);
    rob_osg->skin_param_ptr = &rob_osg->skin_param;
    rob_osg->osage_setting.parts = ROB_OSAGE_PARTS_NONE;
    rob_osg->osage_setting.exf = 0;
    rob_osg->reset_data_list = 0;
    rob_osg->field_2A4 = 0.0f;
    rob_osg->field_2A1 = 0;
    rob_osg->set_external_force = false;
    rob_osg->external_force = 0.0f;
    rob_osg->parent_mat_ptr = 0;
    rob_osg->parent_mat = mat4_null;
}

void ExNodeBlock__Field_10(ExNodeBlock* node) {
    node->field_59 = false;
}

void ExOsageBlock__Init(ExOsageBlock* osg) {
    osg->base.__vftable->Reset(&osg->base);
}

// 0x14047EE90
static void RobOsage__ApplyResetData(RobOsage* rob_osg, mat4* mat) {
    if (rob_osg->reset_data_list) {
        std::list<RobOsageNodeResetData>::iterator reset_data = rob_osg->reset_data_list->begin();
        RobOsageNode* i_begin = rob_osg->nodes.data() + 1;
        RobOsageNode* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
        for (RobOsageNode* i = i_begin; i != i_end; i++)
            i->reset_data = *reset_data++;
        rob_osg->reset_data_list = 0;
    }

    mat4 temp;
    mat4_transpose(mat, &temp);
    RobOsageNode* i_begin = rob_osg->nodes.data() + 1;
    RobOsageNode* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++) {
        mat4_transform_vector(&temp, &i->reset_data.trans_diff, &i->trans_diff);
        mat4_transform_point(&temp, &i->reset_data.trans, &i->trans);
    }
    rob_osg->parent_mat = *rob_osg->parent_mat_ptr;
}

static void sub_14047F110(RobOsage* rob_osg, mat4* mat, vec3* parent_scale, bool init_rot) {
    mat4_transpose(mat, mat);
    vec3 position = rob_osg->exp_data.position * *parent_scale;
    mat4_mul_translate(mat, &position, mat);
    mat4_mul_rotate_zyx(mat, &rob_osg->exp_data.rotation, mat);

    skin_param* skin_param = rob_osg->skin_param_ptr;
    vec3 rot = skin_param->rot;
    if (init_rot)
        rot = skin_param->init_rot + rot;
    mat4_mul_rotate_zyx(mat, &rot, mat);
    mat4_transpose(mat, mat);
}

static bool sub_140482FF0(mat4* mat, vec3* trans, skin_param_hinge* hinge, vec3* rot, int32_t* yz_order) {
    bool clipped = false;
    float_t z_rot;
    float_t y_rot;
    mat4_transpose(mat, mat);
    if (*yz_order == 1) {
        y_rot = atan2f(-trans->z, trans->x);
        z_rot = atan2f(trans->y, sqrtf(trans->x * trans->x + trans->z * trans->z));
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
        mat4_mul_rotate_y(mat, y_rot, mat);
        mat4_mul_rotate_z(mat, z_rot, mat);
    }
    else {
        z_rot = atan2f(trans->y, trans->x);
        y_rot = atan2f(-trans->z, sqrtf(trans->x * trans->x + trans->y * trans->y));
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
        mat4_mul_rotate_z(mat, z_rot, mat);
        mat4_mul_rotate_y(mat, y_rot, mat);
    }
    mat4_transpose(mat, mat);

    if (rot) {
        rot->y = y_rot;
        rot->z = z_rot;
    }
    return clipped;
}

static void sub_1404803B0(RobOsage* rob_osg, mat4* root_matrix, vec3* parent_scale, bool has_children_node) {
    mat4 v47 = *root_matrix;
    vec3 v45 = rob_osg->exp_data.position * *parent_scale;
    mat4_transpose(&v47, &v47);
    mat4_transform_point(&v47, &v45, &rob_osg->nodes.data()[0].trans);

    if (rob_osg->osage_reset && !rob_osg->prev_osage_reset) {
        rob_osg->prev_osage_reset = true;
        RobOsage__ApplyResetData(rob_osg, root_matrix);
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
                mat4 parent_mat;
                vec3 v44;
                mat4_transpose(&rob_osg->parent_mat, &parent_mat);
                mat4_inverse_transform_point(&parent_mat, &i->trans, &v44);
                mat4_transpose(rob_osg->parent_mat_ptr, &parent_mat);
                mat4_transform_point(&parent_mat, &v44, &v44);
                i->trans += (v44 - i->trans) * move_cancel;
            }
        }
    }

    if (!has_children_node)
        return;

    mat4_transpose(&v47, &v47);
    sub_14047F110(rob_osg, &v47, parent_scale, false);
    *rob_osg->nodes.data()[0].bone_node_mat = v47;
    mat4_transpose(&v47, &v47);

    RobOsageNode* v29 = rob_osg->nodes.data();
    RobOsageNode* v30_begin = rob_osg->nodes.data() + 1;
    RobOsageNode* v30_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (RobOsageNode* v30 = v30_begin; v30 != v30_end; v29++, v30++) {
        vec3 v45;
        mat4_inverse_transform_point(&v47, &v30->trans, &v45);
        mat4_transpose(&v47, &v47);
        bool v32 = sub_140482FF0(&v47, &v45, &v30->data_ptr->skp_osg_node.hinge,
            &v30->reset_data.rotation, &rob_osg->yz_order);
        *v30->bone_node_ptr->ex_data_mat = v47;
        mat4_transpose(&v47, &v47);

        float_t v34 = vec3::distance(v30->trans, v29->trans);
        float_t v35 = parent_scale->x * v30->length;
        bool v36;
        if (v34 >= fabsf(v35)) {
            v35 = v34;
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
        mat4_transpose(&v48, &v48);
        mat4_mul_translate(&v48, parent_scale->x * rob_osg->node.length, 0.0f, 0.0f, &v48);
        mat4_transpose(&v48, &v48);
        *rob_osg->node.bone_node_ptr->ex_data_mat = v48;
    }
}

static void RobOsage__set_wind_direction(RobOsage* osg_block_data, vec3* wind_direction) {
    if (wind_direction)
        osg_block_data->wind_direction = *wind_direction;
    else
        osg_block_data->wind_direction = 0.0f;
}

static void ExOsageBlock__SetWindDirection(ExOsageBlock* osg) {
    vec3* (*wind_task_struct_get_wind_direction)() = (vec3 * (*)())0x000000014053D560;

    vec3 wind_direction = *wind_task_struct_get_wind_direction();
    wind_direction *= osg->base.item_equip_object->item_equip->wind_strength;
    RobOsage__set_wind_direction(&osg->rob, &wind_direction);
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

static void sub_140482F30(vec3* pos1, vec3* pos2, float_t length) {
    vec3 diff = *pos1 - *pos2;
    float_t dist = vec3::length_squared(diff);
    if (dist > length * length)
        *pos1 = *pos2 + diff * (length / sqrtf(dist));
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

static void sub_140482180(RobOsageNode* node, float_t a2) {
    float_t v2 = node->data_ptr->skp_osg_node.coli_r + a2;
    if (v2 <= node->trans.y)
        return;

    node->trans.y = v2;
    node->trans = node[-1].trans + vec3::normalize(node->trans - node[-1].trans) * node->length;
    node->trans_diff = 0.0f;
    node->field_C8 += 1.0f;
}

static void sub_14047C800(RobOsage* rob_osg, mat4* root_matrix, vec3* parent_scale,
    float_t step, bool disable_external_force, bool ring_coli, bool has_children_node) {
    if (!rob_osg->nodes.size())
        return;

    const float_t osage_gravity_const = get_osage_gravity_const();
    sub_1404803B0(rob_osg, root_matrix, parent_scale, false);

    RobOsageNode* v17 = &rob_osg->nodes.data()[0];
    v17->trans_orig = v17->trans;
    vec3 v113 = rob_osg->exp_data.position * *parent_scale;

    mat4 v130 = *root_matrix;
    mat4_transpose(&v130, &v130);
    mat4_transform_point(&v130, &v113, &v17->trans);
    v17->trans_diff = v17->trans - v17->trans_orig;

    mat4_transpose(&v130, &v130);
    sub_14047F110(rob_osg, &v130, parent_scale, false);
    *rob_osg->nodes.data()[0].bone_node_mat = v130;
    *rob_osg->nodes.data()[0].bone_node_ptr->ex_data_mat = v130;
    mat4_transpose(&v130, &v130);

    v113 = { 1.0f, 0.0f, 0.0f };
    vec3 v128;
    mat4_transform_vector(&v130, &v113, &v128);

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

            vec3 v126 = v55->trans + v55->trans_diff + v55->field_28;
            sub_140482F30(&v126, &v111, v25 * v55->length);

            vec3 v117 = (v128 - v126) * rob_osg->skin_param_ptr->stiffness;
            float_t weight = v55->data_ptr->skp_osg_node.weight;
            float_t v74 = 1.0f / (weight - (weight - 1.0f) * v55->data_ptr->skp_osg_node.inertial_cancel);
            v55->field_28 += v117 * v74;

            v126 = v55->trans + v55->trans_diff + v55->field_28;

            vec3 v129;
            mat4_inverse_transform_point(&v131, &v126, &v129);

            mat4_transpose(&v131, &v131);
            sub_140482FF0(&v131, &v129, 0, 0, &rob_osg->yz_order);
            mat4_transpose(&v131, &v131);

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
            mat4_inverse_transform_point(&v130, &v99->trans, &v129);
            mat4_transpose(&v130, &v130);
            bool v102 = sub_140482FF0(&v130, &v129,
                &v99->data_ptr->skp_osg_node.hinge,
                &v99->reset_data.rotation, &rob_osg->yz_order);
            *v99->bone_node_ptr->ex_data_mat = v130;
            mat4_transpose(&v130, &v130);

            float_t v104 = vec3::distance_squared(v99->trans, v100->trans);
            float_t v105 = v25 * v99->length;

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
            mat4_transpose(&v131, &v131);
            mat4_mul_translate(&v131, v25 * rob_osg->node.length, 0.0f, 0.0f, &v131);
            mat4_transpose(&v131, &v131);
            *rob_osg->node.bone_node_ptr->ex_data_mat = v131;
        }
    }
    rob_osg->field_2A0 = false;
}

static void RobOsage__coli_set(RobOsage* rob_osg, const mat4* transform) {
    if (rob_osg->skin_param_ptr->coli.size())
        OsageCollision::Work::update_cls_work(rob_osg->coli, rob_osg->skin_param_ptr->coli.data(), transform);
    OsageCollision::Work::update_cls_work(rob_osg->coli_ring, rob_osg->ring.skp_root_coli, transform);
}

static void sub_14047ECA0(RobOsage* rob_osg, float_t step) {
    if (step <= 0.0f)
        return;

    OsageCollision::Work* coli = rob_osg->coli;
    OsageCollision::Work* coli_ring = rob_osg->coli_ring;
    OsageCollision& vec_coli = rob_osg->ring.coli;

    RobOsageNode* i_begin = rob_osg->nodes.data() + 1;
    RobOsageNode* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    if (rob_osg->disable_collision)
        for (RobOsageNode* i = i_begin; i != i_end; i++) {
            RobOsageNodeData* data = i->data_ptr;
            i->field_C8 += (float_t)OsageCollision::osage_cls_work_list(i->trans,
                data->skp_osg_node.coli_r, vec_coli, &i->field_CC);
        }
    else
        for (RobOsageNode* i = i_begin; i != i_end; i++) {
            RobOsageNodeData* data = i->data_ptr;
            i->field_C8 += (float_t)OsageCollision::osage_cls_work_list(i->trans,
                data->skp_osg_node.coli_r, vec_coli, &i->field_CC);
            for (RobOsageNode*& j : data->boc) {
                j->field_C8 += (float_t)OsageCollision::osage_cls(coli_ring, j->trans, data->skp_osg_node.coli_r);
                j->field_C8 += (float_t)OsageCollision::osage_cls(coli, j->trans, data->skp_osg_node.coli_r);
            }

            i->field_C8 += (float_t)OsageCollision::osage_cls(coli_ring, i->trans, data->skp_osg_node.coli_r);
            i->field_C8 += (float_t)OsageCollision::osage_cls(coli, i->trans, data->skp_osg_node.coli_r);
        }
    }

static void sub_14047D620(RobOsage* rob_osg, float_t step) {
    if (step < 0.0f && rob_osg->disable_collision)
        return;

    prj::vector<RobOsageNode>& nodes = rob_osg->nodes;
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
                OsageCollision::osage_capsule_cls(coli_ring, i->trans, j->trans, data->skp_osg_node.coli_r)
                + OsageCollision::osage_capsule_cls(coli, i->trans, j->trans, data->skp_osg_node.coli_r));
            i->field_C8 += v17;
            j->field_C8 += v17;
        }
        if (coli_type && (coli_type != 1 || i != i_begin)) {
            float_t v20 = (float_t)(
                OsageCollision::osage_capsule_cls(coli_ring, i[0].trans, i[-1].trans, data->skp_osg_node.coli_r)
                + OsageCollision::osage_capsule_cls(coli, i[0].trans, i[-1].trans, data->skp_osg_node.coli_r));
            i[0].field_C8 += v20;
            i[-1].field_C8 += v20;
        }
    }
    nodes.data()[0].trans = v7;
}

static void sub_14047D8C0(RobOsage* rob_osg, mat4* root_matrix, vec3* parent_scale, float_t step, bool a5) {
    if (!rob_osg->osage_reset && step <= 0.0f)
        return;

    OsageCollision::Work* coli = rob_osg->coli;
    OsageCollision::Work* coli_ring = rob_osg->coli_ring;
    OsageCollision& vec_coli = rob_osg->ring.coli;

    float_t v9 = 0.0f;
    if (step > 0.0f)
        v9 = 1.0f / step;

    mat4 v64;
    if (a5)
        v64 = *rob_osg->nodes.data()[0].bone_node_mat;
    else {
        v64 = *root_matrix;

        vec3 trans = rob_osg->exp_data.position * *parent_scale;
        mat4_transpose(&v64, &v64);
        mat4_transform_point(&v64, &trans, &rob_osg->nodes.data()[0].trans);
        mat4_transpose(&v64, &v64);
        sub_14047F110(rob_osg, &v64, parent_scale, false);
    }
    mat4_transpose(&v64, &v64);

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
        prj::vector<RobOsageNode>* v24 = rob_osg->skin_param_ptr->colli_tgt_osg;
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
        float_t v37 = (1.0f - rob_osg->field_1EB4) * rob_osg->skin_param_ptr->friction;
        if (a5) {
            sub_140482490(v35, step, parent_scale->x);
            v35->field_C8 = (float_t)OsageCollision::osage_cls_work_list(v35->trans,
                skp_osg_node->coli_r, vec_coli, &v35->field_CC);
            if (!rob_osg->disable_collision) {
                v35->field_C8 += (float_t)OsageCollision::osage_cls(coli_ring, v35->trans, skp_osg_node->coli_r);
                v35->field_C8 += (float_t)OsageCollision::osage_cls(coli, v35->trans, skp_osg_node->coli_r);
            }
            sub_140482180(v35, v60);
        }
        else
            sub_140482F30(&v35[0].trans, &v35[-1].trans, v35[0].length * parent_scale->x);

        vec3 v63;
        mat4_inverse_transform_point(&v64, &v35->trans, &v63);
        mat4_transpose(&v64, &v64);
        bool v40 = sub_140482FF0(&v64, &v63, &skp_osg_node->hinge,
            &v35->reset_data.rotation, &rob_osg->yz_order);
        v35->bone_node_ptr->exp_data.parent_scale = *parent_scale;
        *v35->bone_node_ptr->ex_data_mat = v64;
        mat4_transpose(&v64, &v64);

        if (v35->bone_node_mat) {
            mat4* v41 = v35->bone_node_mat;
            mat4_scale_rot(&v64, parent_scale, v41);
            mat4_transpose(v41, v41);
        }

        float_t v42 = v35->length * parent_scale->x;
        float_t v44 = vec3::distance_squared(v35[0].trans, v35[-1].trans);
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

        if (v35->field_C8 > 0.0f) {
            if (v37 > v35->field_CC)
                v37 = v35->field_CC;
            v35->trans_diff *= v37;
        }

        float_t v55 = vec3::length_squared(v35->trans_diff);
        if (v55 > v23 * v23)
            v35->trans_diff *= v23 / sqrtf(v55);

        mat4 v65;
        mat4_transpose(root_matrix, &v65);
        mat4_inverse_transform_point(&v65, &v35->trans, &v35->reset_data.trans);
        mat4_inverse_transform_vector(&v65, &v35->trans_diff, &v35->reset_data.trans_diff);
    }

    if (rob_osg->nodes.size() && rob_osg->node.bone_node_mat) {
        mat4 v65 = *rob_osg->nodes.back().bone_node_ptr->ex_data_mat;
        mat4_transpose(&v65, &v65);
        mat4_mul_translate(&v65, rob_osg->node.length * parent_scale->x, 0.0f, 0.0f, &v65);
        mat4_transpose(&v65, &v65);
        *rob_osg->node.bone_node_ptr->ex_data_mat = v65;
        mat4_transpose(&v65, &v65);
        mat4_scale_rot(&v65, parent_scale, &v65);
        mat4_transpose(&v65, &v65);
        *rob_osg->node.bone_node_mat = v65;
        rob_osg->node.bone_node_ptr->exp_data.parent_scale = *parent_scale;
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

static void sub_140480260(RobOsage* rob_osg, mat4* root_matrix,
    vec3* parent_scale, float_t step, bool disable_external_force) {
    if (!disable_external_force) {
        RobOsage__set_nodes_external_force(rob_osg, 0, 1.0f);
        RobOsage__set_nodes_force(rob_osg, 1.0f);
        rob_osg->set_external_force = false;
        rob_osg->external_force = 0.0f;
    }

    RobOsageNode* i_begin = rob_osg->nodes.data() + 1;
    RobOsageNode* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++) {
        i->field_C8 = 0.0f;
        i->field_CC = 1.0f;
    }

    rob_osg->field_2A0 = true;
    rob_osg->field_2A1 = false;
    rob_osg->field_2A4 = -1.0f;
    rob_osg->field_1F0C = false;
    rob_osg->osage_reset = false;
    rob_osg->prev_osage_reset = false;
    rob_osg->parent_mat = *rob_osg->parent_mat_ptr;
}

void ExOsageBlock__Field_18(ExOsageBlock* osg, int32_t stage, bool disable_external_force) {
    float_t(*get_delta_frame)() = (float_t(*)())0x0000000140192D50;

    rob_chara_item_equip* rob_item_equip = osg->base.item_equip_object->item_equip;
    float_t step = get_delta_frame() * rob_item_equip->step;
    if (rob_item_equip->opd_blend_data.size() && rob_item_equip->opd_blend_data.front().field_C)
        step = 1.0f;

    bone_node* parent_node = osg->base.parent_bone_node;
    mat4* root_matrix = parent_node->ex_data_mat;
    vec3 parent_scale = parent_node->exp_data.parent_scale;
    switch (stage) {
    case 0:
        sub_1404803B0(&osg->rob, root_matrix, &parent_scale, osg->base.has_children_node);
        break;
    case 1:
    case 2:
        if ((stage == 1 && osg->base.field_58) || (stage == 2 && osg->rob.field_2A0)) {
            ExOsageBlock__SetWindDirection(osg);
            sub_14047C800(&osg->rob, root_matrix,
                &parent_scale, step, disable_external_force, true, osg->base.has_children_node);
        }
        break;
    case 3:
        RobOsage__coli_set(&osg->rob, osg->mat);
        sub_14047ECA0(&osg->rob, step);
        break;
    case 4:
        sub_14047D620(&osg->rob, step);
        break;
    case 5:
        sub_14047D8C0(&osg->rob, root_matrix, &parent_scale, step, false);
        sub_140480260(&osg->rob, root_matrix, &parent_scale, step, disable_external_force);
        osg->base.field_59 = true;
        break;
    }
}

static void sub_14047C770(RobOsage* rob_osg, mat4* root_matrix,
    vec3* parent_scale, float_t step, bool disable_external_force) {
    sub_14047C800(rob_osg, root_matrix, parent_scale, step, disable_external_force, false, false);
    sub_14047D8C0(rob_osg, root_matrix, parent_scale, step, true);
    sub_140480260(rob_osg, root_matrix, parent_scale, step, disable_external_force);
}

static void sub_14047C750(RobOsage* rob_osg, mat4* root_matrix, vec3* parent_scale, float_t step) {
    sub_14047C770(rob_osg, root_matrix, parent_scale, step, false);
}

void ExOsageBlock__Field_20(ExOsageBlock* osg) {
    osg->field_1FF8 &= ~2;
    if (osg->base.field_59) {
        osg->base.field_59 = false;
        return;
    }

    float_t(*get_delta_frame)() = (float_t(*)())0x0000000140192D50;

    rob_chara_item_equip* rob_item_equip = osg->base.item_equip_object->item_equip;
    float_t step = get_delta_frame() * rob_item_equip->step;
    if (rob_item_equip->opd_blend_data.size() && rob_item_equip->opd_blend_data.front().field_C)
        step = 1.0f;

    bone_node* parent_node = osg->base.parent_bone_node;
    vec3 parent_scale = parent_node->exp_data.parent_scale;
    vec3 scale = parent_node->exp_data.scale;

    mat4 mat = *parent_node->ex_data_mat;
    if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f) {
        scale = 1.0f / scale;
        mat4_transpose(&mat, &mat);
        mat4_scale_rot(&mat, &scale, &mat);
        mat4_transpose(&mat, &mat);
    }
    ExOsageBlock__SetWindDirection(osg);

    RobOsage__coli_set(&osg->rob, osg->mat);
    sub_14047C750(&osg->rob, &mat, &parent_scale, step);
}

void sub_140482DF0(struc_477* dst, struc_477* src0, struc_477* src1, float_t blend) {
    dst->length = lerp_def(src0->length, src1->length, blend);
    dst->angle = vec3::lerp(src0->angle, src1->angle, blend);
}

void sub_140482100(struc_476* a1, opd_blend_data* a2, struc_477* a3) {
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

// 0x14047E240
void RobOsage__set_osage_play_data(RobOsage* rob_osg, mat4* parent_mat,
    vec3* parent_scale, prj::vector<opd_blend_data>* opd_blend_data) {
    if (!opd_blend_data->size())
        return;

    vec3 v63 = rob_osg->exp_data.position * *parent_scale;

    mat4 v85;
    mat4_transpose(parent_mat, &v85);
    mat4_transform_point(&v85, &v63, &rob_osg->nodes.data()[0].trans);
    mat4_transpose(&v85, &v85);

    sub_14047F110(rob_osg, &v85, parent_scale, false);
    *rob_osg->nodes.data()[0].bone_node_mat = v85;
    *rob_osg->nodes.data()[0].bone_node_ptr->ex_data_mat = v85;
    mat4_transpose(&v85, &v85);

    mat4_transpose(parent_mat, parent_mat);
    ::opd_blend_data* i_begin = opd_blend_data->data() + opd_blend_data->size();
    ::opd_blend_data* i_end = opd_blend_data->data();
    for (::opd_blend_data* i = i_begin; i != i_end; ) {
        i--;

        mat4 v87 = v85;
        vec3 v22 = rob_osg->nodes.data()[0].trans;
        vec3 v25 = v22;

        float_t frame = i->frame;
        if (frame >= i->frame_count)
            frame = 0.0f;

        frame = prj::floorf(frame);

        int32_t curr_key = (int32_t)(int64_t)frame;
        int32_t next_key = curr_key + 1;
        if ((float_t)next_key >= i->frame_count)
            next_key = 0;

        float_t blend = frame - (float_t)(int64_t)frame;
        float_t inv_blend = 1.0f - blend;
        RobOsageNode* j_begin = rob_osg->nodes.data() + 1;
        RobOsageNode* j_end = rob_osg->nodes.data() + rob_osg->nodes.size();
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

            mat4_transform_point(parent_mat, &v62, &v62);
            mat4_transform_point(parent_mat, &v77, &v77);

            vec3 v82 = v62 * inv_blend + v77 * blend;

            vec3 v86;
            mat4_inverse_transform_point(&v87, &v82, &v86);
            mat4_transpose(&v87, &v87);

            vec3 v64 = 0.0f;
            sub_140482FF0(&v87, &v86, 0, &v64, &rob_osg->yz_order);
            mat4_transpose(&v87, &v87);
            mat4_set_translation(&v87, &v82);

            struc_477 v72;
            v72.angle = v64;
            v72.length = vec3::distance(v62, v22) * inv_blend + vec3::distance(v77, v25) * blend;
            sub_140482100(&j->field_1B0, i, &v72);

            v22 = v62;
            v25 = v77;
        }
    }
    mat4_transpose(parent_mat, parent_mat);

    RobOsageNode* j_begin = rob_osg->nodes.data() + 1;
    RobOsageNode* j_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (RobOsageNode* j = j_begin; j != j_end; j++) {
        float_t v54 = j->field_1B0.field_0.angle.y;
        float_t v55 = j->field_1B0.field_0.angle.z;
        v54 = clamp_def(v54, (float_t)-M_PI, (float_t)M_PI);
        v55 = clamp_def(v55, (float_t)-M_PI, (float_t)M_PI);
        mat4_mul_rotate_z(&v85, v55, &v85);
        mat4_mul_rotate_y(&v85, v54, &v85);
        mat4_transpose(&v85, &v85);
        j->bone_node_ptr->exp_data.parent_scale = *parent_scale;
        *j->bone_node_ptr->ex_data_mat = v85;
        mat4_transpose(&v85, &v85);
        if (j->bone_node_mat) {
            mat4 mat = v85;
            mat4_scale_rot(&mat, parent_scale, &mat);
            mat4_transpose(&mat, j->bone_node_mat);
        }
        mat4_mul_translate(&v85, j->field_1B0.field_0.length, 0.0f, 0.0f, &v85);
        j->trans_orig = j->trans;
        mat4_get_translation(&v85, &j->trans);
    }

    if (rob_osg->nodes.size() && rob_osg->node.bone_node_mat) {
        mat4 v87 = *rob_osg->nodes.back().bone_node_ptr->ex_data_mat;
        mat4_transpose(&v87, &v87);
        mat4_mul_translate(&v87, rob_osg->node.length * parent_scale->x, 0.0f, 0.0f, &v87);
        mat4_transpose(&v87, &v87);
        *rob_osg->node.bone_node_ptr->ex_data_mat = v87;

        mat4_transpose(&v87, &v87);
        mat4_scale_rot(&v87, parent_scale, &v87);
        mat4_transpose(&v87, &v87);
        *rob_osg->node.bone_node_mat = v87;
        rob_osg->node.bone_node_ptr->exp_data.parent_scale = *parent_scale;
    }
}

void ExOsageBlock__SetOsagePlayData(ExOsageBlock* osg) {
    rob_chara_item_equip* rob_itm_equip = osg->base.item_equip_object->item_equip;
    bone_node* parent_node = osg->base.parent_bone_node;
    vec3 parent_scale = parent_node->exp_data.parent_scale;
    RobOsage__set_osage_play_data(&osg->rob, parent_node->ex_data_mat,
        &parent_scale, &rob_itm_equip->opd_blend_data);
}

void ExOsageBlock__Disp(ExOsageBlock* osg) {

}

void ExOsageBlock__Reset(ExOsageBlock* osg) {
    osg->index = 0;
    RobOsage__Reset(&osg->rob);
    osg->field_1FF8 &= ~3;
    osg->mat = 0;
    osg->step = 1.0f;
    osg->base.bone_node_ptr = 0;
}

void ExOsageBlock__Field_40(ExOsageBlock* osg) {

}

void ExOsageBlock__Field_48(ExOsageBlock* osg) {
    void (*sub_14047F990) (RobOsage * a1, mat4 * a2, vec3 * a3, bool a4)
        = (void (*) (RobOsage*, mat4*, vec3*, bool))0x000000014047F990;

    osg->step = 4.0f;
    ExOsageBlock__SetWindDirection(osg);
    RobOsage__coli_set(&osg->rob, osg->mat);
    bone_node* parent_node = osg->base.parent_bone_node;
    vec3 parent_scale = parent_node->exp_data.parent_scale * 0.5f;
    sub_14047F990(&osg->rob, parent_node->ex_data_mat, &parent_scale, false);
    osg->field_1FF8 &= ~2;
}

void ExOsageBlock__Field_50(ExOsageBlock* osg) {
    if (osg->base.field_59) {
        osg->base.field_59 = 0;
        return;
    }

    ExOsageBlock__SetWindDirection(osg);

    bone_node* parent_node = osg->base.parent_bone_node;
    vec3 parent_scale = parent_node->exp_data.parent_scale;
    RobOsage__coli_set(&osg->rob, osg->mat);
    sub_14047C770(&osg->rob, parent_node->ex_data_mat, &parent_scale, osg->step, true);
    float_t step = 0.5f * osg->step;
    osg->step = max_def(step, 1.0f);
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

static void sub_14053CE30(RobOsageNodeDataNormalRef* normal_ref, mat4* a2) {
    if (!normal_ref->field_0)
        return;

    mat4 mat;
    vec3 n_trans;
    vec3 u_trans;
    vec3 d_trans;
    vec3 l_trans;
    vec3 r_trans;
    mat4_transpose(normal_ref->n->bone_node_ptr->ex_data_mat, &mat);
    mat4_get_translation(&mat, &n_trans);
    mat4_transpose(normal_ref->u->bone_node_ptr->ex_data_mat, &mat);
    mat4_get_translation(&mat, &u_trans);
    mat4_transpose(normal_ref->d->bone_node_ptr->ex_data_mat, &mat);
    mat4_get_translation(&mat, &d_trans);
    mat4_transpose(normal_ref->l->bone_node_ptr->ex_data_mat, &mat);
    mat4_get_translation(&mat, &l_trans);
    mat4_transpose(normal_ref->r->bone_node_ptr->ex_data_mat, &mat);
    mat4_get_translation(&mat, &r_trans);

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
        mat4_transpose(&v33, &v33);
        mat4_mul(&v33, &v34, a2);
        mat4_transpose(a2, a2);
    }
}

static void sub_14047E1C0(RobOsage* rob_osg, vec3* parent_scale) {
    RobOsageNode* i_begin = rob_osg->nodes.data() + 1;
    RobOsageNode* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++) {
        if (!i->data_ptr->normal_ref.field_0)
            continue;

        sub_14053CE30(&i->data_ptr->normal_ref, i->bone_node_mat);

        mat4 mat;
        mat4_transpose(i->bone_node_mat, &mat);
        mat4_scale_rot(&mat, parent_scale, &mat);
        mat4_transpose(&mat, i->bone_node_mat);
    }
}

void ExOsageBlock__Field_58(ExOsageBlock* osg) {
    bone_node* parent_node = osg->base.parent_bone_node;
    vec3 parent_scale = parent_node->exp_data.parent_scale;
    sub_14047E1C0(&osg->rob, &parent_scale);
}
