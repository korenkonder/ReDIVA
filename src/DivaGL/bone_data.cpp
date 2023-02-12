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

static void skin_param_hinge_limit(skin_param_hinge* hinge) {
    hinge->ymin = max_def(hinge->ymin, -179.0f) * DEG_TO_RAD_FLOAT;
    hinge->ymax = min_def(hinge->ymax, 179.0f) * DEG_TO_RAD_FLOAT;
    hinge->zmin = max_def(hinge->zmin, -179.0f) * DEG_TO_RAD_FLOAT;
    hinge->zmax = min_def(hinge->zmax, 179.0f) * DEG_TO_RAD_FLOAT;
}

static void rob_osage_node_data_init(rob_osage_node_data *node_data) {
    node_data->force = 0.0f;
    node_data->boc.end = node_data->boc.begin;
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

static void rob_osage_node_init(rob_osage_node* node) {
    void (*sub_14021DCC0)(vector_old_opd_vec3_data * vec, size_t size)
        = (void (*)(vector_old_opd_vec3_data*, size_t))0x000000014021DCC0;
    node->length = 0.0f;
    node->trans = 0.0f;
    node->trans_orig = 0.0f;
    node->trans_diff = 0.0f;
    node->field_28 = 0.0f;
    node->child_length = 0.0f;
    node->bone_node_ptr = 0;
    node->bone_node_mat = 0;
    node->sibling_node = 0;
    node->distance = 0.0f;
    node->field_94 = 0.0f;
    node->reset_data.trans = 0.0f;
    node->reset_data.trans_diff = 0.0f;
    node->reset_data.rotation = 0.0f;
    node->reset_data.length = 0.0f;
    node->field_C8 = 0.0f;
    node->field_CC = 1.0f;
    node->external_force = 0.0f;
    node->force = 1.0f;
    rob_osage_node_data_init(&node->data);
    node->data_ptr = &node->data;
    node->opd_data.end = node->opd_data.begin;
    sub_14021DCC0(&node->opd_data, 3);
    node->mat = mat4_null;
}

static void skin_param_init(skin_param* skp) {

}

static void rob_osage_init(rob_osage* rob_osg) {
    void (*sub_140216750)(tree_motion_reset_data * a1, tree_motion_reset_data_node * a2)
        = (void (*)(tree_motion_reset_data*, tree_motion_reset_data_node*))0x0000000140216750;
    void (*vector_old_rob_osage_node_clear)(vector_old_rob_osage_node * vec,
        rob_osage_node * begin, rob_osage_node * end)
        = (void (*)(vector_old_rob_osage_node*, rob_osage_node*, rob_osage_node*))0x00000001404817E0;

    vector_old_rob_osage_node_clear(&rob_osg->nodes, rob_osg->nodes.begin, rob_osg->nodes.end);
    rob_osg->nodes.end = rob_osg->nodes.begin;
    rob_osage_node_init(&rob_osg->node);
    rob_osg->wind_direction = 0.0f;
    rob_osg->field_1EB4 = 0.0f;
    rob_osg->yz_order = 0;
    rob_osg->field_2A0 = true;
    sub_140216750(&rob_osg->motion_reset_data, rob_osg->motion_reset_data.head->parent);
    rob_osg->motion_reset_data.head->parent = rob_osg->motion_reset_data.head;
    rob_osg->motion_reset_data.head->left = rob_osg->motion_reset_data.head;
    rob_osg->motion_reset_data.head->right = rob_osg->motion_reset_data.head;
    rob_osg->motion_reset_data.size = 0;
    rob_osg->move_cancel = 0.0f;
    rob_osg->field_1F0C = false;
    rob_osg->osage_reset = false;
    rob_osg->field_1F0E = false;
    rob_osg->ring.ring_height = -1000.0f;
    rob_osg->ring.ring_rectangle_x = 0.0f;
    rob_osg->ring.ring_rectangle_y = 0.0f;
    rob_osg->ring.ring_rectangle_width = 0.0f;
    rob_osg->ring.ring_out_height = -1000.0f;
    rob_osg->ring.field_18 = 0;
    rob_osg->ring.ring_rectangle_height = 0.0f;
    rob_osg->ring.coli.end = rob_osg->ring.coli.begin;
    rob_osg->ring.skp_root_coli.end = rob_osg->ring.skp_root_coli.begin;
    rob_osg->field_1F0F = false;
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

static void rob_osage_node_free(rob_osage_node* node) {
    if (node->opd_data.begin) {
        operator_delete(node->opd_data.begin);
        node->opd_data.begin = 0;
        node->opd_data.end = 0;
        node->opd_data.capacity_end = 0;
    }

    if (node->data.boc.begin) {
        operator_delete(node->data.boc.begin);
        node->data.boc.begin = 0;
        node->data.boc.end = 0;
        node->data.boc.capacity_end = 0;
    }
}

static void rob_osage_free(rob_osage* rob_osg) {
    void (*sub_14021BEF0)(tree_motion_reset_data * a1, tree_motion_reset_data_node * *a2,
        tree_motion_reset_data_node * a3, tree_motion_reset_data_node * a4)
        = (void (*)(tree_motion_reset_data*, tree_motion_reset_data_node**,
            tree_motion_reset_data_node*, tree_motion_reset_data_node*))0x000000014021BEF0;
    void (*vector_old_rob_osage_node_clear)(vector_old_rob_osage_node * vec,
        rob_osage_node * begin, rob_osage_node * end)
        = (void (*)(vector_old_rob_osage_node*, rob_osage_node*, rob_osage_node*))0x00000001404817E0;

    rob_osage_init(rob_osg);

    tree_motion_reset_data_node* v6;
    sub_14021BEF0(&rob_osg->motion_reset_data, &v6,
        rob_osg->motion_reset_data.head->left, rob_osg->motion_reset_data.head);
    operator_delete(rob_osg->motion_reset_data.head);

    if (rob_osg->ring.skp_root_coli.begin) {
        operator_delete(rob_osg->ring.skp_root_coli.begin);
        rob_osg->ring.skp_root_coli.begin = 0;
        rob_osg->ring.skp_root_coli.end = 0;
        rob_osg->ring.skp_root_coli.capacity_end = 0;
    }

    if (rob_osg->ring.coli.begin) {
        operator_delete(rob_osg->ring.coli.begin);
        rob_osg->ring.coli.begin = 0;
        rob_osg->ring.coli.end = 0;
        rob_osg->ring.coli.capacity_end = 0;
    }

    if (rob_osg->skin_param.coli.begin) {
        operator_delete(rob_osg->skin_param.coli.begin);
        rob_osg->skin_param.coli.begin = 0;
        rob_osg->skin_param.coli.end = 0;
        rob_osg->skin_param.coli.capacity_end = 0;
    }

    rob_osage_node_free(&rob_osg->node);

    if (rob_osg->nodes.begin) {
        vector_old_rob_osage_node_clear(&rob_osg->nodes, rob_osg->nodes.begin, rob_osg->nodes.end);
        operator_delete(rob_osg->nodes.begin);
        rob_osg->nodes.begin = 0;
        rob_osg->nodes.end = 0;
        rob_osg->nodes.capacity_end = 0;
    }
}

void ExNodeBlock__Field_10(ExNodeBlock* node) {
    node->field_59 = false;
}

ExOsageBlock* ExOsageBlock__Dispose(ExOsageBlock* osg, bool dispose) {
    osg->base.__vftable = ExOsageBlock_vftable;
    ExOsageBlock__Reset(osg);
    rob_osage_free(&osg->rob);
    osg->base.__vftable = ExNodeBlock_vftable;
    if (osg->base.parent_name.capacity > 0x0F)
        operator_delete(osg->base.parent_name.ptr);
    osg->base.parent_name = string_empty;
    if (dispose)
        operator_delete(osg);
    return osg;
}

void ExOsageBlock__Init(ExOsageBlock* osg) {
    osg->base.__vftable->Reset(&osg->base);
}

static void sub_14047EE90(rob_osage* rob_osg, mat4* mat) {
    if (rob_osg->reset_data_list) {
        list_RobOsageNodeResetData_node* v5 = rob_osg->reset_data_list->head->next;
        for (rob_osage_node* v6 = rob_osg->nodes.begin + 1; v6 != rob_osg->nodes.end; v6++) {
            v6->reset_data = v5->value;
            v5 = v5->next;
        }
        rob_osg->reset_data_list = 0;
    }

    mat4 temp;
    mat4_transpose(mat, &temp);
    for (rob_osage_node* v9 = rob_osg->nodes.begin + 1; v9 != rob_osg->nodes.end; v9++) {
        mat4_mult_vec3(&temp, &v9->reset_data.trans_diff, &v9->trans_diff);
        mat4_mult_vec3_trans(&temp, &v9->reset_data.trans, &v9->trans);
    }
    rob_osg->parent_mat = *rob_osg->parent_mat_ptr;
}

static void sub_14047F110(rob_osage* rob_osg, mat4* mat, vec3* parent_scale, bool init_rot) {
    mat4_transpose(mat, mat);
    vec3 position = rob_osg->exp_data.position * *parent_scale;
    mat4_translate_mult(mat, &position, mat);
    mat4_rotate_mult(mat, &rob_osg->exp_data.rotation, mat);

    skin_param* skin_param = rob_osg->skin_param_ptr;
    vec3 rot = skin_param->rot;
    if (init_rot)
        rot = skin_param->init_rot + rot;
    mat4_rotate_mult(mat, &rot, mat);
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
        mat4_rotate_y_mult(mat, y_rot, mat);
        mat4_rotate_z_mult(mat, z_rot, mat);
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
        mat4_rotate_z_mult(mat, z_rot, mat);
        mat4_rotate_y_mult(mat, y_rot, mat);
    }
    mat4_transpose(mat, mat);

    if (rot) {
        rot->y = y_rot;
        rot->z = z_rot;
    }
    return clipped;
}

static void sub_1404803B0(rob_osage* rob_osg, mat4* mat, vec3* parent_scale, bool a4) {
    mat4 v47 = *mat;
    vec3 v45;
    vec3_mult(rob_osg->exp_data.position, *parent_scale, v45);
    mat4_transpose(&v47, &v47);
    mat4_mult_vec3_trans(&v47, &v45, &rob_osg->nodes.begin[0].trans);
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

        if (move_cancel > 0.0f)
            for (rob_osage_node* v19 = rob_osg->nodes.begin + 1; v19 != rob_osg->nodes.end; v19++) {
                mat4 parent_mat;
                vec3 v46;
                mat4_transpose(&rob_osg->parent_mat, &parent_mat);
                mat4_mult_vec3_inv_trans(&parent_mat, &v19->trans, &v46);
                mat4_transpose(rob_osg->parent_mat_ptr, &parent_mat);
                mat4_mult_vec3_trans(&parent_mat, &v46, &v46);
                vec3_sub(v46, v19->trans, v46);
                vec3_mult_scalar(v46, move_cancel, v46);
                vec3_add(v19->trans, v46, v19->trans);
            }
    }

    if (a4) {
        mat4_transpose(&v47, &v47);
        sub_14047F110(rob_osg, &v47, parent_scale, 0);
        *rob_osg->nodes.begin[0].bone_node_mat = v47;
        mat4_transpose(&v47, &v47);
        for (rob_osage_node* v29 = rob_osg->nodes.begin, *v30 = rob_osg->nodes.begin + 1;
            v30 != rob_osg->nodes.end; v29++, v30++) {
            vec3 v46;
            mat4_mult_vec3_inv_trans(&v47, &v30->trans, &v46);
            mat4_transpose(&v47, &v47);
            bool v32 = sub_140482FF0(&v47, &v46, &v30->data_ptr->skp_osg_node.hinge,
                &v30->reset_data.rotation, &rob_osg->yz_order);
            *v30->bone_node_ptr->ex_data_mat = v47;
            mat4_transpose(&v47, &v47);
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

        if (vector_old_length(rob_osg->nodes) && rob_osg->node.bone_node_mat) {
            mat4 v48 = *rob_osg->nodes.end[-1].bone_node_ptr->ex_data_mat;
            mat4_transpose(&v48, &v48);
            mat4_translate_mult(&v48, parent_scale->x * rob_osg->node.length, 0.0f, 0.0f, &v48);
            mat4_transpose(&v48, &v48);
            *rob_osg->node.bone_node_ptr->ex_data_mat = v48;
        }
    }
}

static void rob_osage_set_wind_direction(rob_osage* osg_block_data, vec3* wind_direction) {
    if (wind_direction)
        osg_block_data->wind_direction = *wind_direction;
    else
        osg_block_data->wind_direction = 0.0f;
}

static void ExOsageBlock__SetWindDirection(ExOsageBlock* osg) {
    vec3* (*wind_task_struct_get_wind_direction)() = (vec3 * (*)())0x000000014053D560;

    vec3 wind_direction = *wind_task_struct_get_wind_direction();
    rob_chara_item_equip_object* item_equip_object = osg->base.item_equip_object;
    vec3_mult_scalar(wind_direction, item_equip_object->item_equip->wind_strength, wind_direction);
    rob_osage_set_wind_direction(&osg->rob, &wind_direction);
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

    sub_140482F30(&node[0].trans, &node[-1].trans, node->length * a3);
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
    node->trans_diff = 0.0f;
    node->field_C8 += 1.0f;
}

static void sub_14047C800(rob_osage* rob_osg, mat4* mat,
    vec3* parent_scale, float_t step, bool a5, bool a6, bool a7) {
    if (!vector_old_length(rob_osg->nodes))
        return;

    const float_t osage_gravity_const = get_osage_gravity_const();
    sub_1404803B0(rob_osg, mat, parent_scale, 0);

    rob_osage_node* v17 = &rob_osg->nodes.begin[0];
    v17->trans_orig = v17->trans;
    vec3 v113;
    vec3_mult(rob_osg->exp_data.position, *parent_scale, v113);

    mat4 v130 = *mat;
    mat4_transpose(&v130, &v130);
    mat4_mult_vec3_trans(&v130, &v113, &v17->trans);
    vec3_sub(v17->trans, v17->trans_orig, v17->trans_diff);
    mat4_transpose(&v130, &v130);
    sub_14047F110(rob_osg, &v130, parent_scale, 0);
    *rob_osg->nodes.begin[0].bone_node_mat = v130;
    *rob_osg->nodes.begin[0].bone_node_ptr->ex_data_mat = v130;
    mat4_transpose(&v130, &v130);

    v113 = { 1.0f, 0.0f, 0.0f };
    vec3 v128;
    mat4_mult_vec3(&v130, &v113, &v128);

    float_t v25 = parent_scale->x;
    if (!rob_osg->osage_reset && step <= 0.0f)
        return;

    bool stiffness = rob_osg->skin_param_ptr->stiffness > 0.0f;

    rob_osage_node* v12 = rob_osg->nodes.end;
    rob_osage_node* v30 = rob_osg->nodes.begin;
    for (rob_osage_node* v26 = rob_osg->nodes.begin + 1; v26 != v12; v26++, v30++) {
        rob_osage_node_data* v31 = v26->data_ptr;
        float_t weight = v31->skp_osg_node.weight;
        vec3 v111;
        if (!rob_osg->set_external_force) {
            sub_140482300(&v111, &v26->trans, &v30->trans, osage_gravity_const, weight);

            if (v26 != &v12[-1]) {
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

            mat4_transpose(&v131, &v131);
            sub_140482FF0(&v131, &v129, 0, 0, &rob_osg->yz_order);
            mat4_transpose(&v131, &v131);

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
        sub_140482F30(&v90[0].trans, &v90[1].trans, v25 * v90->child_length);

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
            mat4_transpose(&v130, &v130);
            bool v102 = sub_140482FF0(&v130, &v129,
                &v99->data_ptr->skp_osg_node.hinge,
                &v99->reset_data.rotation, &rob_osg->yz_order);
            *v99->bone_node_ptr->ex_data_mat = v130;
            mat4_transpose(&v130, &v130);

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

        if (vector_old_length(rob_osg->nodes) && rob_osg->node.bone_node_mat) {
            mat4 v131 = *rob_osg->nodes.end[-1].bone_node_ptr->ex_data_mat;
            mat4_transpose(&v131, &v131);
            mat4_translate_mult(&v131, v25 * rob_osg->node.length, 0.0f, 0.0f, &v131);
            mat4_transpose(&v131, &v131);
            *rob_osg->node.bone_node_ptr->ex_data_mat = v131;
        }
    }
    rob_osg->field_2A0 = false;
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

static int32_t sub_140484450(vec3* a1, vec3* a2, vec3* a3, vec3* a4, float_t* radius) {
    vec3 v8;
    sub_140484E10(&v8, a2, a3, a4);
    return sub_140483DE0(a1, &v8, a4, *radius);
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

static int32_t sub_140484540(vec3* a1, vec3* a2, osage_coli* coli, float_t radius) {

    vec3 v11;
    vec3_sub(*a2, coli->bone0_pos, v11);
    float_t v17;
    vec3_dot(v11, coli->bone_pos_diff, v17);
    if (v17 < 0.0f)
        return sub_140483DE0(a1, a2, &coli->bone0_pos, radius);

    float_t v19 = coli->bone_pos_diff_length_squared;
    if (fabs(coli->bone_pos_diff_length_squared) <= 0.000001)
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

static int32_t sub_1404844A0(vec3* a1, vec3* a2, vec3* a3, osage_coli* a4, float_t radius) {
    vec3 v8;
    sub_140484850(&v8, a2, a3, a4);
    return sub_140484540(a1, &v8, a4, radius);
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
        vec3 v21 = 0.0f;
        switch (a4->type) {
        case 1: {
            vec3 v22 = a4->bone0_pos;
            float_t v23 = a3->coli_r + a4->radius;
            v8 += sub_140484450(&v21, a1, a2, &v22, &v23);
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

static void osage_coli_set(osage_coli* osg_coli, skin_param_osage_root_coli* skp_root_coli, mat4* mats) {
    osg_coli->type = 0;
    if (!skp_root_coli || !mats)
        return;

    for (; skp_root_coli->type; osg_coli++, skp_root_coli++) {
        osg_coli->type = skp_root_coli->type;
        osg_coli->radius = skp_root_coli->radius;
        mat4 mat = mats[skp_root_coli->bone0_index];
        mat4_transpose(&mat, &mat);
        mat4_mult_vec3_trans(&mat, &skp_root_coli->bone0_pos, &osg_coli->bone0_pos);
        if (skp_root_coli->type == 2 || skp_root_coli->type == 4) {
            mat = mats[skp_root_coli->bone1_index];
            mat4_transpose(&mat, &mat);
            mat4_mult_vec3_trans(&mat, &skp_root_coli->bone1_pos, &osg_coli->bone1_pos);
            vec3_sub(osg_coli->bone1_pos, osg_coli->bone0_pos, osg_coli->bone_pos_diff);
            vec3_length_squared(osg_coli->bone_pos_diff, osg_coli->bone_pos_diff_length_squared);
            osg_coli->bone_pos_diff_length = sqrtf(osg_coli->bone_pos_diff_length_squared);
            if (osg_coli->bone_pos_diff_length < 0.01f)
                osg_coli->type = 1;
        }
        else if (skp_root_coli->type == 3)
            mat4_mult_vec3(&mat, &skp_root_coli->bone1_pos, &osg_coli->bone1_pos);
    }
    osg_coli->type = 0;
}

static void osage_coli_ring_set(osage_coli* osg_coli, vector_old_skin_param_osage_root_coli* vec_skp_root_coli, mat4* mats) {
    if (vec_skp_root_coli->begin == vec_skp_root_coli->end) {
        osg_coli->type = 0;
        osg_coli->radius = 0.0f;
        osg_coli->bone0_pos = 0.0f;
        osg_coli->bone1_pos = 0.0f;
        osg_coli->bone_pos_diff = 0.0f;
        osg_coli->bone_pos_diff_length = 0.0f;
        osg_coli->bone_pos_diff_length_squared = 0.0f;
        osg_coli->field_34 = 1.0f;
    }
    else
        osage_coli_set(osg_coli, vec_skp_root_coli->begin, mats);
}

static void rob_osage_coli_set(rob_osage* rob_osg, mat4* mats) {
    if (vector_old_length(rob_osg->skin_param_ptr->coli))
        osage_coli_set(rob_osg->coli, rob_osg->skin_param_ptr->coli.begin, mats);
    osage_coli_ring_set(rob_osg->coli_ring, &rob_osg->ring.skp_root_coli, mats);
}

static int32_t sub_140484780(vec3* a1, vec3* a2, vec3* a3, vec3* a4, float_t a5) {
    float_t v5;
    float_t v6;
    vec3_dot(*a2, *a4, v5);
    vec3_dot(*a3, *a4, v6);
    v5 -= v6;
    v5 -= a5;
    if (v5 >= 0.0f)
        return 0;
    vec3_mult_scalar(*a4, -v5, *a1);
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

static int32_t sub_140485220(vec3* a1, skin_param_osage_node* a2, osage_coli* coli, float_t* a4){
    if (!coli)
        return 0;

    int32_t v8 = 0;
    for (osage_coli* v9 = coli; v9->type; v9++) {
        int32_t v11 = 0;
        vec3 v31 = 0.0f;
        switch (v9->type) {
        case 1:
            v11 = sub_140483DE0(&v31, a1, &v9->bone0_pos, v9->radius + a2->coli_r);
            break;
        case 2:
            v11 = sub_140484540(&v31, a1, v9, v9->radius + a2->coli_r);
            break;
        case 3:
            v11 = sub_140484780(&v31, a1, &v9->bone0_pos, &v9->bone1_pos, a2->coli_r);
            break;
        case 4:
            v11 = sub_140483EA0(&v31, a1, v9, v9->radius + a2->coli_r);
            break;
        case 5:
            v11 = sub_140483B30(&v31, a1, v9, a2->coli_r);
            break;
        }

        if (a4 && v11 > 0 && v9->field_34 < *a4)
            *a4 = v9->field_34;

        vec3_add(*a1, v31, *a1);
        v8 += v11;
    }
    return v8;
}

static int32_t sub_1404851C0(vec3* a1, skin_param_osage_node* a2, vector_old_osage_coli* a3, float_t* a4) {
    if (a3->begin == a3->end || a3->end[-1].type)
        return 0;

    return sub_140485220(a1, a2, a3->begin, a4);
}

static int32_t sub_140485180(osage_coli* a1, vec3* a2, skin_param_osage_node* a3) {
    return sub_140485220(a2, a3, a1, 0);
}

static void sub_14047ECA0(rob_osage* rob_osg, float_t step) {
    if (step <= 0.0f)
        return;

    osage_coli* coli = rob_osg->coli;
    osage_coli* coli_ring = rob_osg->coli_ring;
    vector_old_osage_coli* ring_coli = &rob_osg->ring.coli;
    bool v11 = rob_osg->field_1F0F;
    for (rob_osage_node* i = rob_osg->nodes.begin + 1; i != rob_osg->nodes.end; i++) {
        rob_osage_node_data* data = i->data_ptr;
        i->field_C8 += (float_t)sub_1404851C0(&i->trans, &data->skp_osg_node, ring_coli, &i->field_CC);
        if (v11)
            continue;

        for (rob_osage_node** j = data->boc.begin; j != data->boc.end; j++) {
            rob_osage_node* k = *j;
            k->field_C8 += (float_t)sub_140485180(coli_ring, &k->trans, &data->skp_osg_node);
            k->field_C8 += (float_t)sub_140485180(coli, &k->trans, &data->skp_osg_node);
        }

        i->field_C8 += (float_t)sub_140485180(coli_ring, &i->trans, &data->skp_osg_node);
        i->field_C8 += (float_t)sub_140485180(coli, &i->trans, &data->skp_osg_node);
    }
}

static void sub_14047D620(rob_osage* rob_osg, float_t step) {
    if (step < 0.0f && rob_osg->field_1F0F)
        return;

    vector_old_rob_osage_node* nodes = &rob_osg->nodes;
    vec3 v7 = nodes->begin[0].trans;
    osage_coli* coli = rob_osg->coli;
    osage_coli* coli_ring = rob_osg->coli_ring;
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

        int32_t v18 = rob_osg->skin_param_ptr->coli_type;
        if (v18 && (v18 != 1 || v10 != nodes->begin + 1)) {
            float_t v20 = (float_t)(
                sub_140485000(&v10[0].trans, &v10[-1].trans, &v11->skp_osg_node, coli)
                + sub_140485000(&v10[0].trans, &v10[-1].trans, &v11->skp_osg_node, coli_ring));
            v10[0].field_C8 += v20;
            v10[-1].field_C8 += v20;
        }
    }
    nodes->begin[0].trans = v7;
}

static void sub_14047D8C0(rob_osage* rob_osg, mat4* mat, vec3* parent_scale, float_t step, bool a5) {
    if (!rob_osg->osage_reset && step <= 0.0f)
        return;

    osage_coli* coli = rob_osg->coli;
    osage_coli* coli_ring = rob_osg->coli_ring;
    vector_old_osage_coli* vec_ring_coli = &rob_osg->ring.coli;

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
        mat4_transpose(&v64, &v64);
        mat4_mult_vec3_trans(&v64, &trans, &rob_osg->nodes.begin[0].trans);
        mat4_transpose(&v64, &v64);
        sub_14047F110(rob_osg, &v64, parent_scale, 0);
    }
    mat4_transpose(&v64, &v64);

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

    float_t v23 = 0.2f;
    if (step < 1.0f)
        v23 = 0.2f / (2.0f - step);

    if (rob_osg->skin_param_ptr->colli_tgt_osg) {
        vector_old_rob_osage_node* v24 = rob_osg->skin_param_ptr->colli_tgt_osg;
        rob_osage_node* v26 = v24->end;
        rob_osage_node* v27 = rob_osg->nodes.begin + 1;
        for (rob_osage_node* v25 = rob_osg->nodes.begin + 1;
            v25 != rob_osg->nodes.end; v25++, v27++) {
            vec3 v62 = 0.0f;
            for (rob_osage_node* v30 = v24->begin + 1; v30 != v26; v30++)
                sub_140483DE0(&v62, &v27->trans, &v30->trans,
                    v27->data_ptr->skp_osg_node.coli_r + v30->data_ptr->skp_osg_node.coli_r);
            vec3_add(v27->trans, v62, v27->trans);
        }
    }

    for (rob_osage_node* v35 = rob_osg->nodes.begin + 1; v35 != rob_osg->nodes.end; v35++) {
        skin_param_osage_node* skp_osg_node = &v35->data_ptr->skp_osg_node;
        float_t v37 = (1.0f - rob_osg->field_1EB4) * rob_osg->skin_param_ptr->friction;
        if (a5) {
            sub_140482490(v35, step, parent_scale->x);
            v35->field_C8 = (float_t)sub_1404851C0(&v35->trans,
                skp_osg_node, vec_ring_coli, &v35->field_CC);
            if (!rob_osg->field_1F0F) {
                v35->field_C8 += (float_t)sub_140485180(coli_ring, &v35->trans, skp_osg_node);
                v35->field_C8 += (float_t)sub_140485180(coli, &v35->trans, skp_osg_node);
            }
            sub_140482180(v35, v60);
        }
        else
            sub_140482F30(&v35[0].trans, &v35[-1].trans, v35[0].length * parent_scale->x);

        vec3 v63;
        mat4_mult_vec3_inv_trans(&v64, &v35->trans, &v63);
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
            if (v35->field_CC < v37)
                v37 = v35->field_CC;
            vec3_mult_scalar(v35->trans_diff, v37, v35->trans_diff);
        }

        float_t v55;
        vec3_length_squared(v35->trans_diff, v55);
        if (v55 > v23 * v23)
            vec3_mult_scalar(v35->trans_diff, v23 / sqrtf(v55), v35->trans_diff);

        mat4 v65;
        mat4_transpose(mat, &v65);
        mat4_mult_vec3_inv_trans(&v65, &v35->trans, &v35->reset_data.trans);
        mat4_mult_vec3_inv(&v65, &v35->trans_diff, &v35->reset_data.trans_diff);
    }

    if (vector_old_length(rob_osg->nodes) && rob_osg->node.bone_node_mat) {
        mat4 v65 = *rob_osg->nodes.end[-1].bone_node_ptr->ex_data_mat;
        mat4_transpose(&v65, &v65);
        mat4_translate_mult(&v65, rob_osg->node.length * parent_scale->x, 0.0f, 0.0f, &v65);
        mat4_transpose(&v65, &v65);
        *rob_osg->node.bone_node_ptr->ex_data_mat = v65;
        mat4_transpose(&v65, &v65);
        mat4_scale_rot(&v65, parent_scale, &v65);
        mat4_transpose(&v65, &v65);
        *rob_osg->node.bone_node_mat = v65;
        rob_osg->node.bone_node_ptr->exp_data.parent_scale = *parent_scale;
    }
}

static void sub_140480F40(rob_osage* rob_osg, vec3* external_force, float_t a3) {
    vec3 v4;
    if (external_force)
        v4 = *external_force;
    else
        v4 = 0.0f;

    ssize_t v7 = rob_osg->osage_setting.parts;
    ssize_t v8 = 0;
    if (v7 >= 4) {
        float_t v9 = a3 * a3 * a3 * a3;
        ssize_t v10 = ((v7 - 4ULL) >> 2) + 1;
        v8 = 4 * v10;
        for (; v10; v10--)
            vec3_mult_scalar(v4, v9, v4);
    }

    if (v8 < v7)
        for (size_t v12 = v7 - v8; v12; v12--)
            vec3_mult_scalar(v4, a3, v4);

    for (rob_osage_node* i = rob_osg->nodes.begin + 1; i != rob_osg->nodes.end; i++) {
        i->external_force = v4;
        vec3_mult_scalar(v4, a3, v4);
    }
}

static void rob_osage_set_nodes_force(rob_osage* rob_osg, float_t a2) {
    for (rob_osage_node* i = rob_osg->nodes.begin + 1; i != rob_osg->nodes.end; i++)
        i->force = a2;
}

static void sub_140480260(rob_osage* rob_osg, mat4* mat, vec3* parent_scale, float_t step, bool v5) {
    if (!v5) {
        sub_140480F40(rob_osg, 0, 1.0f);
        rob_osage_set_nodes_force(rob_osg, 1.0f);
        rob_osg->set_external_force = false;
        rob_osg->external_force = 0.0f;
    }

    for (rob_osage_node* v6 = rob_osg->nodes.begin; v6 != rob_osg->nodes.end; v6++) {
        v6->field_C8 = 0.0f;
        v6->field_CC = 1.0f;
    }

    rob_osg->field_2A0 = true;
    rob_osg->field_2A1 = false;
    rob_osg->field_2A4 = -1.0f;
    rob_osg->field_1F0C = false;
    rob_osg->osage_reset = false;
    rob_osg->field_1F0E = false;
    rob_osg->parent_mat = *rob_osg->parent_mat_ptr;
}

void ExOsageBlock__Field_18(ExOsageBlock* osg, int32_t a2, bool a3) {
    float_t(*get_delta_frame)() = (float_t(*)())0x0000000140192D50;

    rob_chara_item_equip* rob_item_equip = osg->base.item_equip_object->item_equip;
    float_t step = get_delta_frame() * rob_item_equip->step;
    if (vector_old_length(rob_item_equip->opd_blend_data)
        && rob_item_equip->opd_blend_data.begin[0].field_C)
        step = 1.0f;

    bone_node* parent_node = osg->base.parent_bone_node;
    mat4* parent_node_mat = parent_node->ex_data_mat;
    vec3 parent_scale = parent_node->exp_data.parent_scale;
    switch (a2) {
    case 0:
        sub_1404803B0(&osg->rob, parent_node_mat, &parent_scale, osg->base.field_5A);
        break;
    case 1:
    case 2:
        if ((a2 == 1 && osg->base.field_58) || (a2 == 2 && osg->rob.field_2A0)) {
            ExOsageBlock__SetWindDirection(osg);
            sub_14047C800(&osg->rob, parent_node_mat, &parent_scale, step, a3, true, osg->base.field_5A);
        }
        break;
    case 3:
        rob_osage_coli_set(&osg->rob, osg->mat);
        sub_14047ECA0(&osg->rob, step);
        break;
    case 4:
        sub_14047D620(&osg->rob, step);
        break;
    case 5:
        sub_14047D8C0(&osg->rob, parent_node_mat, &parent_scale, step, false);
        sub_140480260(&osg->rob, parent_node_mat, &parent_scale, step, a3);
        osg->base.field_59 = true;
        break;
    }
}

static void sub_14047C770(rob_osage* rob_osg, mat4* mat, vec3* parent_scale, float_t step, bool a5) {
    sub_14047C800(rob_osg, mat, parent_scale, step, a5, false, false);
    sub_14047D8C0(rob_osg, mat, parent_scale, step, true);
    sub_140480260(rob_osg, mat, parent_scale, step, a5);
}

static void sub_14047C750(rob_osage* rob_osg, mat4* mat, vec3* parent_scale, float_t step) {
    sub_14047C770(rob_osg, mat, parent_scale, step, 0);
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
    if (vector_old_length(rob_item_equip->opd_blend_data)
        && rob_item_equip->opd_blend_data.begin[0].field_C)
        step = 1.0f;

    bone_node* parent_node = osg->base.parent_bone_node;
    vec3 parent_scale = parent_node->exp_data.parent_scale;
    vec3 scale = parent_node->exp_data.scale;

    mat4 mat = *parent_node->ex_data_mat;
    if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f) {
        vec3_div(1.0f, scale, scale);
        mat4_transpose(&mat, &mat);
        mat4_scale_rot(&mat, &scale, &mat);
        mat4_transpose(&mat, &mat);
    }
    ExOsageBlock__SetWindDirection(osg);

    rob_osage_coli_set(&osg->rob, osg->mat);
    sub_14047C750(&osg->rob, &mat, &parent_scale, step);
}

void sub_140482DF0(struc_477* dst, struc_477* src0, struc_477* src1, float_t blend) {
    dst->length = lerp_def(src0->length, src1->length, blend);
    vec3_lerp_scalar(src0->angle, src1->angle, dst->angle, blend);
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

void sub_14047E240(rob_osage* rob_osg, mat4* a2, vec3* parent_scale, vector_old_opd_blend_data* a4) {
    if (!vector_old_length(*a4))
        return;

    vec3 v63;
    vec3_mult(rob_osg->exp_data.position, *parent_scale, v63);

    mat4 v85;
    mat4_transpose(a2, &v85);
    mat4_mult_vec3_trans(&v85, &v63, &rob_osg->nodes.begin[0].trans);
    mat4_transpose(&v85, &v85);

    sub_14047F110(rob_osg, &v85, parent_scale, 0);
    *rob_osg->nodes.begin[0].bone_node_mat = v85;
    *rob_osg->nodes.begin[0].bone_node_ptr->ex_data_mat = v85;
    mat4_transpose(&v85, &v85);

    mat4_transpose(a2, a2);
    size_t v17 = vector_old_length(*a4) - 1;
    for (opd_blend_data* i = a4->end; i != a4->begin; v17--) {
        i--;
        mat4 v87 = v85;
        float_t frame = i->frame;
        vec3 v22 = rob_osg->nodes.begin[0].trans;
        vec3 v25 = v22;
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
        for (rob_osage_node* j = rob_osg->nodes.begin + 1; j != rob_osg->nodes.end; j++) {
            opd_vec3_data* opd = &j->opd_data.begin[v17];
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

            vec3 v84;
            mat4_mult_vec3_trans(a2, &v62, &v84);

            vec3 v83;
            mat4_mult_vec3_trans(a2, &v77, &v83);

            vec3 v82;
            vec3_mult_scalar(v83, blend, v82);

            vec3 v81;
            vec3_mult_scalar(v84, inv_blend, v81);
            vec3_add(v82, v81, v82);

            vec3 v86;
            mat4_mult_vec3_inv_trans(&v87, &v82, &v86);
            mat4_transpose(&v87, &v87);

            vec3 v64 = 0.0f;
            sub_140482FF0(&v87, &v86, 0, &v64, &rob_osg->yz_order);
            mat4_transpose(&v87, &v87);
            mat4_set_translation(&v87, &v82);

            float_t v50;
            vec3_distance(v83, v25, v50);

            float_t v49;
            vec3_distance(v84, v22, v49);

            struc_477 v72;
            v72.angle = v64;
            v72.length = v49 * inv_blend + v50 * blend;
            sub_140482100(&j->field_1B0, i, &v72);

            v22 = v84;
            v25 = v83;
        }
    }
    mat4_transpose(a2, a2);

    for (rob_osage_node* i = rob_osg->nodes.begin + 1; i != rob_osg->nodes.end; i++) {
        float_t v54 = i->field_1B0.field_0.angle.y;
        float_t v55 = i->field_1B0.field_0.angle.z;
        v54 = clamp_def(v54, (float_t)-M_PI, (float_t)M_PI);
        v55 = clamp_def(v55, (float_t)-M_PI, (float_t)M_PI);
        mat4_rotate_z_mult(&v85, v55, &v85);
        mat4_rotate_y_mult(&v85, v54, &v85);
        mat4_transpose(&v85, &v85);
        i->bone_node_ptr->exp_data.parent_scale = *parent_scale;
        *i->bone_node_ptr->ex_data_mat = v85;
        mat4_transpose(&v85, &v85);
        if (i->bone_node_mat) {
            mat4 mat = v85;
            mat4_scale_rot(&mat, parent_scale, &mat);
            mat4_transpose(&mat, i->bone_node_mat);
        }
        mat4_translate_mult(&v85, i->field_1B0.field_0.length, 0.0f, 0.0f, &v85);
        i->trans_orig = i->trans;
        mat4_get_translation(&v85, &i->trans);
    }

    if (vector_old_length(rob_osg->nodes) && rob_osg->node.bone_node_mat) {
        mat4 v87 = *rob_osg->nodes.end[-1].bone_node_ptr->ex_data_mat;
        mat4_transpose(&v87, &v87);
        mat4_translate_mult(&v87, rob_osg->node.length * parent_scale->x, 0.0f, 0.0f, &v87);
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
    sub_14047E240(&osg->rob, parent_node->ex_data_mat,
        &parent_scale, &rob_itm_equip->opd_blend_data);
}

void ExOsageBlock__Disp(ExOsageBlock* osg) {

}

void ExOsageBlock__Reset(ExOsageBlock* osg) {
    osg->index = 0;
    rob_osage_init(&osg->rob);
    osg->field_1FF8 &= ~3;
    osg->mat = 0;
    osg->step = 1.0f;
    osg->base.bone_node_ptr = 0;
}

void ExOsageBlock__Field_40(ExOsageBlock* osg) {

}

void ExOsageBlock__Field_48(ExOsageBlock* osg) {
    void (*sub_14047F990) (rob_osage * a1, mat4 * a2, vec3 * a3, bool a4)
        = (void (*) (rob_osage*, mat4*, vec3*, bool))0x000000014047F990;

    osg->step = 4.0f;
    ExOsageBlock__SetWindDirection(osg);
    rob_osage_coli_set(&osg->rob, osg->mat);
    bone_node* parent_node = osg->base.parent_bone_node;
    vec3 parent_scale = parent_node->exp_data.parent_scale;
    vec3_mult_scalar(parent_scale, 0.5f, parent_scale);
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
    rob_osage_coli_set(&osg->rob, osg->mat);
    sub_14047C770(&osg->rob, parent_node->ex_data_mat, &parent_scale, osg->step, 1);
    float_t step = 0.5f * osg->step;
    osg->step = max_def(step, 1.0f);
}

bool sub_14053D1B0(vec3* l_trans, vec3* r_trans,
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

void sub_14053CE30(rob_osage_node_data_normal_ref* normal_ref, mat4* a2) {
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
        mat4_mult(&v33, &v34, a2);
    }
}

void sub_14047E1C0(rob_osage* rob_osg, vec3* parent_scale) {
    for (rob_osage_node* i = rob_osg->nodes.begin + 1; i != rob_osg->nodes.end; ++i) {
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
