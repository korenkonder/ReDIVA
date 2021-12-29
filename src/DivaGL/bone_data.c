/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "bone_data.h"
#include "../KKdLib/interpolation.h"

typedef enum HeapCMallocType {
  HeapCMallocSystem = 0x00,
  HeapCMallocTemp   = 0x01,
  HeapCMallocMode   = 0x02,
  HeapCMallocEvent  = 0x03,
  HeapCMallocDebug  = 0x04,
  HeapCMallocMax    = 0x05,
} HeapCMallocType;

typedef struct skeleton_rotation_offset {
    bool x;
    bool y;
    bool z;
    vec3 rotation;
} skeleton_rotation_offset;

typedef struct struc_403 {
    int32_t field_0;
    int32_t field_4;
    int32_t field_8;
} struc_403;

typedef struct struc_404 {
    int32_t field_0;
    int32_t field_4;
} struc_404;

static aft_bone_database_struct** bone_database_ptr = (void*)0x00000000140FBC1C0;
static struc_403** rob_cmn_mottbl_data_ptr = (void*)0x00000001411E8258;
static void* (*HeapCMallocAllocate)(HeapCMallocType type, size_t size, char* name) = (void*)0x00000001403F2A00;
static void(*HeapCMallocFree)(HeapCMallocType type, void* data) = (void*)0x00000001403F2960;

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

static void bone_node_expression_data_reset_scale(bone_node_expression_data* data) {
    data->scale = vec3_identity;
    data->data_scale = vec3_identity;
}

static void bone_node_data_reset_position_rotation(bone_node_expression_data* data) {
    data->position = vec3_null;
    data->rotation = vec3_null;
    data->scale = vec3_identity;
    data->data_scale = vec3_identity;
}

static bone_data* bone_data_init(bone_data* bone) {
    bone->type = 0;
    bone->has_parent = 0;
    bone->motion_bone_index = 0;
    bone->mirror = 0;
    bone->parent = 0;
    bone->flags = 0;
    bone->key_set_offset = 0;
    bone->key_set_count = 0;
    bone->frame = 0;

    for (int32_t i = 0; i < 2; i++)
        bone->base_translation[i] = vec3_null;

    bone->rotation = vec3_null;
    bone->ik_target = vec3_null;
    bone->trans = vec3_null;
    for (int32_t i = 0; i < 2; i++)
        bone->rot_mat[i] = mat4u_null;

    for (int32_t i = 0; i < 2; i++)
        bone->trans_dup[i] = vec3_null;

    for (int32_t i = 0; i < 6; i++)
        bone->rot_mat_dup[i] = mat4u_null;

    bone->pole_target_mat = 0;
    bone->parent_mat = 0;
    bone->node = 0;
    bone->field_2E0 = 0.0f;
    bone->eyes_xrot_adjust_neg = 1.0f;
    bone->eyes_xrot_adjust_pos = 1.0f;
    return bone;
}

static void aft_bone_database_bones_calculate_count(aft_bone_database_bone* bones, size_t* object_bone_count,
    size_t* motion_bone_count, size_t* total_bone_count, size_t* ik_bone_count, size_t* chain_pos) {
    size_t _object_bone_count = 0;
    size_t _motion_bone_count = 0;
    size_t _total_bone_count = 0;
    size_t _ik_bone_count = 0;
    size_t _chain_pos = 0;
    for (aft_bone_database_bone* i = bones; i->type != AFT_BONE_DATABASE_BONE_END; i++)
        switch (i->type) {
        case AFT_BONE_DATABASE_BONE_ROTATION:
        case AFT_BONE_DATABASE_BONE_TYPE_1:
        case AFT_BONE_DATABASE_BONE_POSITION:
            _object_bone_count++;
            _motion_bone_count++;
            _total_bone_count++;
            _chain_pos++;
            break;
        case AFT_BONE_DATABASE_BONE_POSITION_ROTATION:
            _object_bone_count++;
            _motion_bone_count++;
            _total_bone_count++;
            _ik_bone_count++;
            _chain_pos++;
            break;
        case AFT_BONE_DATABASE_BONE_HEAD_IK_ROTATION:
            _object_bone_count++;
            _motion_bone_count++;
            _total_bone_count += 3;
            _ik_bone_count++;
            _chain_pos += 2;
            break;
        case AFT_BONE_DATABASE_BONE_ARM_IK_ROTATION:
        case AFT_BONE_DATABASE_BONE_LEGS_IK_ROTATION:
            _object_bone_count += 3;
            _motion_bone_count++;
            _total_bone_count += 4;
            _ik_bone_count++;
            _chain_pos += 3;
            break;
        }
    *object_bone_count = _object_bone_count;
    *motion_bone_count = _motion_bone_count;
    *total_bone_count = _total_bone_count;
    *ik_bone_count = _ik_bone_count;
    *chain_pos = _chain_pos;
}

static aft_bone_database_skeleton* aft_bone_database_get_skeleton_by_type(aft_bone_database_skeleton_type type) {
    if (type == AFT_BONE_DATABASE_SKELETON_NONE || !*bone_database_ptr
        || (*bone_database_ptr)->signature != 0x9102720)
        return 0;

    return (*bone_database_ptr)->skeletons[type];
}

static aft_bone_database_bone* aft_bone_database_get_bones_by_type(aft_bone_database_skeleton_type type) {
    aft_bone_database_skeleton* skel = aft_bone_database_get_skeleton_by_type(type);
    if (!skel)
        return 0;

    return skel->bones;
}

static uint16_t* aft_bone_database_get_skeleton_parent_indices_by_type(aft_bone_database_skeleton_type type) {
    aft_bone_database_skeleton* skel = aft_bone_database_get_skeleton_by_type(type);
    if (!skel)
        return 0;

    return skel->parent_indices;
}

static vec3* aft_bone_database_get_skeleton_positions_by_type(aft_bone_database_skeleton_type type) {
    aft_bone_database_skeleton* skel = aft_bone_database_get_skeleton_by_type(type);
    if (!skel)
        return 0;

    return skel->positions;
}

static float_t aft_bone_database_get_skeleton_heel_height(aft_bone_database_skeleton_type a1) {
    aft_bone_database_skeleton* skel = aft_bone_database_get_skeleton_by_type(a1);
    if (skel)
        return skel->heel_height[0];
    else
        return 0.0f;
}

static void rob_chara_data_bone_data_reserve(rob_chara_data_bone_data* rob_bone_data) {
    if (rob_bone_data->mats.begin)
        HeapCMallocFree(HeapCMallocSystem, rob_bone_data->mats.begin);

    size_t mats_size = rob_bone_data->object_bone_count;
    rob_bone_data->mats.begin = HeapCMallocAllocate(HeapCMallocSystem,
        mats_size * sizeof(mat4), "ALLOCsys");
    rob_bone_data->mats.end = rob_bone_data->mats.begin + mats_size;
    rob_bone_data->mats.capacity_end = rob_bone_data->mats.begin + mats_size;

    for (mat4* i = rob_bone_data->mats.begin; i != rob_bone_data->mats.end; i++)
        *i = mat4_identity;

    if (rob_bone_data->mats2.begin)
        HeapCMallocFree(HeapCMallocSystem, rob_bone_data->mats2.begin);

    size_t mats2_size = rob_bone_data->total_bone_count - rob_bone_data->object_bone_count;
    rob_bone_data->mats2.begin = HeapCMallocAllocate(HeapCMallocSystem,
        mats2_size * sizeof(mat4), "ALLOCsys");
    rob_bone_data->mats2.end = rob_bone_data->mats2.begin + mats2_size;
    rob_bone_data->mats2.capacity_end = rob_bone_data->mats2.begin + mats2_size;

    for (mat4* i = rob_bone_data->mats2.begin; i != rob_bone_data->mats2.end; i++)
        *i = mat4_identity;

    if (rob_bone_data->nodes.begin)
        HeapCMallocFree(HeapCMallocSystem, rob_bone_data->nodes.begin);

    size_t nodes_size = rob_bone_data->total_bone_count;
    rob_bone_data->nodes.begin = HeapCMallocAllocate(HeapCMallocSystem,
        nodes_size * sizeof(bone_node), "ALLOCsys");
    rob_bone_data->nodes.end = rob_bone_data->nodes.begin + nodes_size;
    rob_bone_data->nodes.capacity_end = rob_bone_data->nodes.begin + nodes_size;

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

static void rob_chara_data_bone_data_calculate_bones(rob_chara_data_bone_data* rob_bone_data,
    aft_bone_database_bone* bones) {
    aft_bone_database_bones_calculate_count(bones, &rob_bone_data->object_bone_count,
        &rob_bone_data->motion_bone_count, &rob_bone_data->total_bone_count,
        &rob_bone_data->ik_bone_count, &rob_bone_data->chain_pos);
}

static void rob_chara_data_bone_data_set_mats(rob_chara_data_bone_data* rob_bone_data,
    aft_bone_database_bone* a2) {
    size_t chain_pos = 0;
    size_t ik_bone_count = 0;
    size_t total_bone_count = 0;

    mat4* mats = rob_bone_data->mats.begin;
    mat4* mats2 = rob_bone_data->mats2.begin;
    bone_node* nodes = rob_bone_data->nodes.begin;
    for (aft_bone_database_bone* i = a2; i->type != AFT_BONE_DATABASE_BONE_END; i++)
        switch (i->type) {
        case AFT_BONE_DATABASE_BONE_ROTATION:
        case AFT_BONE_DATABASE_BONE_TYPE_1:
        case AFT_BONE_DATABASE_BONE_POSITION:
            (nodes++)->mat = mats++;

            chain_pos++;
            total_bone_count++;
            break;
        case AFT_BONE_DATABASE_BONE_POSITION_ROTATION:
            (nodes++)->mat = mats++;

            chain_pos++;
            ik_bone_count++;
            total_bone_count++;
            break;
        case AFT_BONE_DATABASE_BONE_HEAD_IK_ROTATION:
            (nodes++)->mat = mats2++;
            (nodes++)->mat = mats++;
            (nodes++)->mat = mats2++;

            chain_pos += 2;
            ik_bone_count++;
            total_bone_count += 3;
            break;
        case AFT_BONE_DATABASE_BONE_ARM_IK_ROTATION:
        case AFT_BONE_DATABASE_BONE_LEGS_IK_ROTATION:
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

    if (total_bone_count != rob_bone_data->total_bone_count)
        printf("Node mismatch\n");
    if (mats - rob_bone_data->mats.begin != rob_bone_data->object_bone_count)
        printf("Matrix mismatch\n");
    if (mats2 - rob_bone_data->mats2.begin != rob_bone_data->total_bone_count - rob_bone_data->object_bone_count)
        printf("Matrix2 mismatch\n");
    if (ik_bone_count != rob_bone_data->ik_bone_count)
        printf("LeafPos mismatch\n");
    if (chain_pos != rob_bone_data->chain_pos)
        printf("ChainPos mismatch\n");
}

static void rob_chara_data_bone_data_set_parent_mats(rob_chara_data_bone_data* rob_bone_data,
    uint16_t* parent_indices) {
    if (rob_bone_data->nodes.end - rob_bone_data->nodes.begin < 1)
        return;

    parent_indices++;
    for (bone_node* i = &rob_bone_data->nodes.begin[1]; i != rob_bone_data->nodes.end; i++)
        i->parent = &rob_bone_data->nodes.begin[*parent_indices++];
}

static void bone_data_parent_load_rob_chara_data(bone_data_parent* a1) {
    rob_chara_data_bone_data* rob_bone_data = a1->rob_bone_data;
    if (!rob_bone_data)
        return;

    a1->motion_bone_count = rob_bone_data->motion_bone_count;
    a1->ik_bone_count = rob_bone_data->ik_bone_count;
    a1->chain_pos = rob_bone_data->chain_pos;

    if (a1->bones.begin)
        HeapCMallocFree(HeapCMallocSystem, a1->bones.begin);

    size_t bones_size = rob_bone_data->motion_bone_count;
    a1->bones.begin = HeapCMallocAllocate(HeapCMallocSystem,
        bones_size * sizeof(bone_data), "ALLOCsys");
    a1->bones.end = a1->bones.begin + bones_size;
    a1->bones.capacity_end = a1->bones.begin + bones_size;

    for (bone_data* i = a1->bones.begin; i != a1->bones.end; i++)
        bone_data_init(i);

    a1->global_key_set_count = 6;
    a1->field_8 = true;
    a1->bone_key_set_count = (uint32_t)((a1->motion_bone_count + a1->ik_bone_count) * 3);
}

static void bone_data_parent_load_bone_database(bone_data_parent* bone,
    aft_bone_database_bone* bones, vec3* common_translation, vec3* translation) {
    rob_chara_data_bone_data* rob_bone_data = bone->rob_bone_data;
    size_t chain_pos = 0;
    size_t total_bone_count = 0;
    size_t ik_bone_count = 0;

    aft_bone_database_bone* v15 = bones;
    bone_data* v16 = bone->bones.begin;
    for (size_t i = 0; v15->type != AFT_BONE_DATABASE_BONE_END; i++, v15++, v16++) {
        v16->motion_bone_index = (uint32_t)i;
        v16->type = v15->type;
        v16->mirror = v15->mirror;
        v16->parent = v15->parent;
        v16->flags = v15->flags;
        if (v15->type >= AFT_BONE_DATABASE_BONE_POSITION_ROTATION)
            v16->key_set_count = 6;
        else
            v16->key_set_count = 3;
        v16->base_translation[0] = *common_translation++;
        v16->base_translation[1] = *translation++;
        v16->node = &rob_bone_data->nodes.begin[total_bone_count];
        v16->has_parent = v15->has_parent;
        if (v15->has_parent)
            v16->parent_mat = &rob_bone_data->mats.begin[v15->parent];
        else
            v16->parent_mat = 0;

        v16->pole_target_mat = 0;
        switch (v15->type) {
        case AFT_BONE_DATABASE_BONE_ROTATION:
        case AFT_BONE_DATABASE_BONE_TYPE_1:
        case AFT_BONE_DATABASE_BONE_POSITION:
            chain_pos++;
            total_bone_count++;
            break;
        case AFT_BONE_DATABASE_BONE_POSITION_ROTATION:
            chain_pos++;
            ik_bone_count++;
            total_bone_count++;
            break;
        case AFT_BONE_DATABASE_BONE_HEAD_IK_ROTATION:
            v16->ik_segment_length[0] = (common_translation++)->x;
            v16->ik_segment_length[1] = (translation++)->x;

            chain_pos += 2;
            ik_bone_count++;
            total_bone_count += 3;
            break;
        case AFT_BONE_DATABASE_BONE_ARM_IK_ROTATION:
        case AFT_BONE_DATABASE_BONE_LEGS_IK_ROTATION:
            v16->ik_segment_length[0] = (common_translation++)->x;
            v16->ik_segment_length[1] = (translation++)->x;
            v16->ik_2nd_segment_length[0] = (common_translation++)->x;
            v16->ik_2nd_segment_length[1] = (translation++)->x;

            mat4* pole_target = 0;
            if (v15->pole_target)
                pole_target = &rob_bone_data->mats.begin[v15->pole_target];
            v16->pole_target_mat = pole_target;

            chain_pos += 3;
            ik_bone_count++;
            total_bone_count += 4;
            break;
        }
    }

    if (total_bone_count != rob_bone_data->total_bone_count)
        printf("Node mismatch\n");
    if (ik_bone_count != bone->ik_bone_count)
        printf("LeafPos mismatch\n");
    if (chain_pos != bone->chain_pos)
        printf("ChainPos mismatch\n");
}

static void bone_data_parent_data_init(bone_data_parent* bone,
    rob_chara_data_bone_data* rob_bone_data) {
    bone->rob_bone_data = rob_bone_data;
    aft_bone_database_bone* common_bone_data = aft_bone_database_get_bones_by_type(rob_bone_data->base_skeleton_type);
    vec3* common_translation = aft_bone_database_get_skeleton_positions_by_type(rob_bone_data->base_skeleton_type);
    vec3* translation = aft_bone_database_get_skeleton_positions_by_type(rob_bone_data->skeleton_type);
    bone_data_parent_load_rob_chara_data(bone);
    bone_data_parent_load_bone_database(bone, common_bone_data, common_translation, translation);
}

static void mot_parent_get_key_set_count(mot_parent* a1, size_t motion_bone_count, size_t ik_bone_count) {
    a1->key_set_count = (motion_bone_count + ik_bone_count) * 3 + 16;
}

static void mot_parent_reserve_key_sets(mot_parent* a1) {
    if (a1->key_set.begin)
        HeapCMallocFree(HeapCMallocSystem, a1->key_set.begin);

    size_t key_set_count = a1->key_set_count;
    a1->key_set.begin = HeapCMallocAllocate(HeapCMallocSystem,
        key_set_count * sizeof(mot_key_set), "ALLOCsys");
    a1->key_set.end = a1->key_set.begin + key_set_count;
    a1->key_set.capacity_end = a1->key_set.begin + key_set_count;

    for (mot_key_set* i = a1->key_set.begin; i != a1->key_set.end; i++) {
        i->type = 0;
        i->keys_count = 0;
        i->current_key = 0;
        i->last_key = 0;
        i->frames = 0;
        i->values = 0;
    }

    if (a1->key_set_data.begin)
        HeapCMallocFree(HeapCMallocSystem, a1->key_set_data.begin);

    a1->key_set_data.begin = HeapCMallocAllocate(HeapCMallocSystem,
        key_set_count * sizeof(float_t), "ALLOCsys");
    a1->key_set_data.end = a1->key_set_data.begin + key_set_count;
    a1->key_set_data.capacity_end = a1->key_set_data.begin + key_set_count;

    for (float_t* i = a1->key_set_data.begin; i != a1->key_set_data.end; i++)
        *i = 0.0f;

    a1->mot.key_sets = a1->key_set.begin;
    a1->key_sets_ready = true;
}

static void mot_parent_init_key_sets(mot_parent* a1, aft_bone_database_skeleton_type type,
    size_t motion_bone_count, size_t ik_bone_count) {
    mot_parent_get_key_set_count(a1, motion_bone_count, ik_bone_count);
    mot_parent_reserve_key_sets(a1);
    a1->skeleton_type = type;
}

static void sub_14040F650(struc_313* a1, size_t a2) {
    uint32_t* v5 = a1->bitfield.end;
    uint32_t* v6 = a1->bitfield.begin;
    size_t v7 = (a2 + 31) >> 5;
    if ((ssize_t)v7 < v5 - v6) {
        uint32_t* v8 = &v6[v7];
        if (v8 == v6)
            a1->bitfield.end = v6;
        else if (v8 != v5) {
            memmove(&v6[v7], v5, 0);
            a1->bitfield.end = v8;
        }
    }
    a1->motion_bone_count = a2;

    a2 &= 0x1F;
    if (a2)
        a1->bitfield.begin[v7 - 1] &= (1 << a2) - 1;
}

static struc_314* sub_1404122C0(struc_313* a1, struc_314* a2, struc_314* a3, struc_314* a4) {
    uint32_t* v4 = a1->bitfield.begin;
    size_t v7 = a1->motion_bone_count;

    struc_314 v8;
    v8.field_0 = a1->bitfield.begin;
    v8.field_8 = 0;
    if (v7) {
        size_t v12 = (a3->field_0 - v4) * 32 + a3->field_8;
        v8.field_0 = &v4[v12 >> 5];
        v8.field_8 = v12 & 0x1F;
    }

    struc_314 v16;
    v16.field_0 = a1->bitfield.begin;
    v16.field_8 = 0;
    if (v7) {
        size_t v17 = (a4->field_0 - v4) * 32 + a4->field_8;
        v16.field_0 = &v4[v17 >> 5];
        v16.field_8 = v17 & 0x1F;
    }

    if (v8.field_0 != v16.field_0 || v8.field_8 != v16.field_8) {
        size_t v19 = 0;
        if (v7) {
            v4 += v7 >> 5;
            v19 = a1->motion_bone_count & 0x1F;
        }

        struc_314 v20 = v16;
        struc_314 v22 = v8;
        while (v20.field_0 != v4 || v20.field_8 != v19) {
            if (*v20.field_0 & (1 << v20.field_8) )
                *v22.field_0 |= 1 << v22.field_8;
            else
                *v22.field_0 &= ~(1 << v22.field_8);

            if (v22.field_8 >= 0x1F) {
                v22.field_8 = 0;
                v22.field_0++;
            }
            else
                v22.field_8++;

            if (v20.field_8 >= 0x1F) {
                v20.field_8 = 0;
                v20.field_0++;
            }
            else
                v20.field_8++;
        }
        sub_14040F650(a1, (v22.field_0 - a1->bitfield.begin) * 32 + v22.field_8);
    }

    size_t v18 = (v8.field_0 - v4) * 32 + v8.field_8;
    a2->field_0 = &a1->bitfield.begin[v18 >> 5];
    a2->field_8 = v18 & 0x1F;
    return a2;
}

static void sub_1404119A0(struc_313* a1) {
    uint32_t* v1 = a1->bitfield.begin;
    size_t v2 = a1->motion_bone_count;
    struc_314 v3;
    v3.field_0 = &v1[v2 >> 5];
    v3.field_8 = v2 & 0x1F;
    struc_314 v4;
    v4.field_0 = v1;
    v4.field_8 = 0;
    struc_314 v5;
    sub_1404122C0(a1, &v5, &v4, &v3);
}

static uint32_t* sub_14040BDB0(uint32_t* a1, uint32_t* a2, uint32_t* a3) {
    for (; a1 != a2; a1++, a3++)
        *a3 = *a1;
    return a3;
}

static void sub_14040BD00(uint32_t* a1, size_t a2, uint32_t* a3) {
    for (; a2; a1++, a2--)
        *a1 = *a3;
}

static void sub_14040E690(struc_313* a1, uint32_t** a2, uint32_t* a3, size_t a4, uint32_t* a5) {
    size_t v8 = a3 - a1->bitfield.begin;
    if (!a4) {
        *a2 = &a1->bitfield.begin[v8];
        return;
    }

    uint32_t* v9 = a1->bitfield.end;
    if (a1->bitfield.capacity_end - v9 >= (ssize_t)a4) {
        if (v9 - a3 >= (ssize_t)a4) {
            uint32_t v20 = *a5;
            size_t v21 = a4;
            uint32_t* v22 = &v9[-(ssize_t)a4];
            a1->bitfield.end = sub_14040BDB0(v22, v9, v9);
            memmove(&v9[-(v22 - a3)], a3, 4 * (v22 - a3));
            for (uint32_t* i = a3; i != &a3[v21]; i++)
                *i = v20;
        }
        else {
            uint32_t v17 = *a5;
            uint32_t v26 = *a5;
            sub_14040BDB0(a3, v9, &a3[a4]);
            sub_14040BD00(a1->bitfield.end, a4 - (a1->bitfield.end - a3), &v26);
            a1->bitfield.end += a4;
            uint32_t* v18 = &a1->bitfield.end[-(ssize_t)a4];
            for (uint32_t* i = a3; i != v18; i++)
                *i = v17;
        }
    }
    else {
        uint32_t* v7 = a1->bitfield.begin;
        size_t v10 = v9 - v7;
        size_t v11 = a1->bitfield.capacity_end - v7;
        size_t v12;
        if (UINT64_MAX / sizeof(uint32_t) - (v11 >> 1) >= v11)
            v12 = (v11 >> 1) + v11;
        else
            v12 = 0;
        if (v12 < v10 + a4)
            v12 = v10 + a4;
        uint32_t* v14 = HeapCMallocAllocate(HeapCMallocSystem, sizeof(uint32_t) * v12, "ALLOCsys");
        size_t v15 = a3 - a1->bitfield.begin;
        sub_14040BD00(&v14[v15], a4, a5);
        sub_14040BDB0(a1->bitfield.begin, a3, v14);
        sub_14040BDB0(a3, a1->bitfield.end, &v14[v15 + a4]);
        size_t v16 = a1->bitfield.end - a1->bitfield.begin + a4;
        if (a1->bitfield.begin)
            HeapCMallocFree(HeapCMallocSystem, a1->bitfield.begin);
        a1->bitfield.capacity_end = &v14[v12];
        a1->bitfield.end = &v14[v16];
        a1->bitfield.begin = v14;
    }
    *a2 = &a1->bitfield.begin[v8];
}

static void sub_140413F30(struc_313* a1, size_t a2, uint32_t* a3) {
    uint32_t* v3 = a1->bitfield.end;
    uint32_t* v5 = a1->bitfield.begin;
    size_t v6 = v3 - v5;
    if (v6 > a2) {
        uint32_t* v7 = &v5[a2];
        if (v7 == v5)
            a1->bitfield.end = v5;
        else if (v7 != v3)
            a1->bitfield.end = v7;
    }
    else if (v6 < a2) {
        uint32_t* a2a;
        sub_14040E690(a1, &a2a, v3, a2 - v6, a3);
    }
}

static size_t sub_14040EB30(struc_313* a1, struc_314* a2, size_t a3) {
    size_t v5 = (a2->field_0 - a1->bitfield.begin) * 32 + a2->field_8;
    if (!a3)
        return v5;

    size_t v6 = a1->motion_bone_count;
    uint32_t v21 = 0;
    sub_140413F30(a1, (v6 + a3 + 31) >> 5, &v21);
    v6 = a1->motion_bone_count;
    if (!v6) {
        a1->motion_bone_count = a3;
        return v5;
    }

    size_t v11 = a3 + v6;
    uint32_t* v9 = a1->bitfield.begin;
    struc_314 v14;
    v14.field_0 = &v9[v6 >> 5];
    v14.field_8 = v6 & 0x1F;
    a1->motion_bone_count = v11;
    struc_314 v16;
    v16.field_0 = &v9[v11 >> 5];
    v16.field_8 = v11 & 0x1F;
    struc_314 v12;
    v12.field_0 = &v9[v5 >> 5];
    v12.field_8 = v5 & 0x1F;
    while (v12.field_0 != v14.field_0 || v12.field_8 != v14.field_8) {
        if (v14.field_8)
            v14.field_8--;
        else {
            v14.field_8 = 31;
            v14.field_0--;
        }

        if (v16.field_8)
            v16.field_8--;
        else {
            v16.field_8 = 31;
            v16.field_0--;
        }

        if (*v14.field_0 & (1 << v14.field_8))
            *v16.field_0 |= 1 << v16.field_8;
        else
            *v16.field_0 &= ~(1 << v16.field_8);
    }
    return v5;
}

static struc_314* sub_14040E980(struc_313* a1, struc_314* a2, struc_314* a3, size_t a4, bool* a5) {
    struc_314 v20 = *a3;
    size_t v8 = sub_14040EB30(a1, &v20, a4);
    uint32_t* v9 = a1->bitfield.begin;

    size_t v10 = v8 + a4;
    struc_314 v12;
    v12.field_0 = &v9[v10 >> 5];
    v12.field_8 = v10 & 0x1F;
    v20.field_0 = &v9[v8 >> 5];
    v20.field_8 = v8 & 0x1F;
    while (v20.field_0 != v12.field_0 || v20.field_8 != v12.field_8) {
        if (*a5)
            *v20.field_0 |= (1 << v20.field_8);
        else
            *v20.field_0 &= ~(1 << v20.field_8);

        if (v20.field_8 >= 0x1F) {
            v20.field_8 = 0;
            v20.field_0++;
        }
        else
            v20.field_8++;
    }

    uint32_t* v17 = a1->bitfield.begin;
    a2->field_0 = &v17[v8 >> 5];
    a2->field_8 = v8 & 0x1F;
    return a2;
}

static void sub_140414550(struc_313* a1, size_t a2, char a3) {
    struc_314 a3a;
    struc_314 a4;
    struc_314 a2a;

    size_t v3 = a1->motion_bone_count;
    if (a2 <= v3) {
        if (a2 < v3) {
            uint32_t* v6 = a1->bitfield.begin;
            a4.field_0 = &v6[v3 >> 5];
            a4.field_8 = v3 & 0x1F;
            a3a.field_0 = &v6[a2 >> 5];
            a3a.field_8 = a2 & 0x1F;
            sub_1404122C0(a1, &a2a, &a3a, &a4);
        }
    }
    else {
        uint32_t*  v5 = a1->bitfield.begin;
        a3a.field_0 = &v5[v3 >> 5];
        a3a.field_8 = v3 & 0x1F;
        a4 = a3a;
        sub_14040E980(a1, &a3a, &a4, a2 - v3, &a3);
    }
}

static void sub_1404146F0(struc_240* a1) {
    struc_314 v1;
    v1.field_0 = a1->field_8.bitfield.begin;
    v1.field_8 = 0;
    motion_bone_index v2 = MOTION_BONE_N_HARA_CP;
    size_t v5 = a1->field_8.motion_bone_count;
    uint32_t* v6 = a1->field_8.bitfield.begin;
    while (true) {
        struc_314 v7;
        v7.field_0 = &v6[v5 >> 5];
        v7.field_8 = v5 & 0x1F;
        if (v1.field_0 == v7.field_0 && v1.field_8 == v7.field_8)
            break;

        if (a1->bone_check_func(v2++))
            *v1.field_0 |= 1 << v1.field_8;
        else
            *v1.field_0 &= ~(1 << v1.field_8);

        if (v1.field_8 >= 0x1F) {
            v1.field_8 = 0;
            v1.field_0++;
        }
        else
            v1.field_8++;
    }
}

static void sub_140413350(struc_240* a1, bool(*bone_check_func)(motion_bone_index), size_t motion_bone_count) {
    a1->bone_check_func = bone_check_func;
    a1->motion_bone_count = motion_bone_count;
    sub_1404119A0(&a1->field_8);
    sub_140414550(&a1->field_8, motion_bone_count, 0);
    sub_1404146F0(a1);
}

static void motion_blend_mot_load_bone_data(motion_blend_mot* a1, rob_chara_data_bone_data* rob_bone_data,
    bool(*bone_check_func)(motion_bone_index)) {
    bone_data_parent_data_init(&a1->bone_data, rob_bone_data);
    mot_parent_init_key_sets(
        &a1->mot_data,
        a1->bone_data.rob_bone_data->base_skeleton_type,
        a1->bone_data.motion_bone_count,
        a1->bone_data.ik_bone_count);
    sub_140413350(&a1->field_0, bone_check_func, a1->bone_data.motion_bone_count);
}

static void sub_140414900(struc_308* a1, mat4* mat) {
    if (mat) {
        mat4_to_mat4u(mat, &a1->mat);
        a1->field_8C = true;
    }
    else {
        a1->mat = mat4u_identity;
        a1->field_8C = false;
    }
}

static void sub_1401EA670(bone_data* a1, vec3* a2) {
    mat4 mat;
    mat4_transpose(a1->node[0].mat, &mat);

    vec3 v15;
    mat4_mult_vec3_inv(&mat, a2, &v15);

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

    mat4_mult(&rot_mat, &mat, &mat);
    mat4_transpose(&mat, a1->node[0].mat);
}

static void sub_1401EAC20(bone_data* a1, vec3* a2, float_t a3) {
    float_t len;
    vec3 v8;

    mat4 mat;
    mat4_transpose(a1->node[0].mat, &mat);
    mat4_mult_vec3_inv(&mat, a2, &v8);
    v8.x = 0.0f;
    vec3_length(v8, len);
    if (len <= 0.000001f)
        return;

    vec3_mult_scalar(v8, 1.0f / len, v8);
    float_t angle = atan2f(v8.z, v8.y);
    mat4_rotate_x_mult(&mat, angle * a3, &mat);
    mat4_transpose(&mat, a1->node[0].mat);
}

static void sub_1401EA970(bone_data* a1, vec3* a2) {
    vec3 v9  = *a2;

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

    mat4 mat;
    mat4_transpose(a1->node[0].mat, &mat);
    mat4_mult(&rot_mat, &mat, &mat);
    mat4_transpose(&mat, a1->node[0].mat);
}

static bool sub_1401E9D60(bone_data* a1, bone_data* a2) {
    bone_node* v5;
    vec3 v30;
    mat4 mat;

    switch (a1->motion_bone_index) {
    case MOTION_BONE_N_SKATA_L_WJ_CD_EX:
        v5 = a2[MOTION_BONE_C_KATA_L].node;
        mat4_transpose(v5[2].mat, &mat);
        mat4_get_translation(&mat, &v30);
        sub_1401EA670(a1, &v30);
        break;
    case MOTION_BONE_N_SKATA_R_WJ_CD_EX:
        v5 = a2[MOTION_BONE_C_KATA_R].node;
        mat4_transpose(v5[2].mat, &mat);
        mat4_get_translation(&mat, &v30);
        sub_1401EA670(a1, &v30);
        break;
    case MOTION_BONE_N_SKATA_B_L_WJ_CD_CU_EX:
        v5 = a2[MOTION_BONE_N_UP_KATA_L_EX].node;
        mat4_transpose(v5[0].mat, &mat);
        mat4_get_translation(&mat, &v30);
        sub_1401EAC20(a1, &v30, 0.333f);
        break;
    case MOTION_BONE_N_SKATA_B_R_WJ_CD_CU_EX:
        v5 = a2[MOTION_BONE_N_UP_KATA_R_EX].node;
        mat4_transpose(v5[0].mat, &mat);
        mat4_get_translation(&mat, &v30);
        sub_1401EAC20(a1, &v30, 0.333f);
        break;
    case MOTION_BONE_N_SKATA_C_L_WJ_CD_CU_EX:
        v5 = a2[MOTION_BONE_N_UP_KATA_L_EX].node;
        mat4_transpose(v5[0].mat, &mat);
        mat4_get_translation(&mat, &v30);
        sub_1401EAC20(a1, &v30, 0.5f);
        break;
    case MOTION_BONE_N_SKATA_C_R_WJ_CD_CU_EX:
        v5 = a2[MOTION_BONE_N_UP_KATA_R_EX].node;
        mat4_transpose(v5[0].mat, &mat);
        mat4_get_translation(&mat, &v30);
        sub_1401EAC20(a1, &v30, 0.5f);
        break;
    case MOTION_BONE_N_MOMO_A_L_WJ_CD_EX:
        v5 = a2[MOTION_BONE_CL_MOMO_L].node;
        mat4_transpose(v5[2].mat, &mat);
        mat4_get_translation(&mat, &v30);
        mat4_transpose(a1->node[0].mat, &mat);
        mat4_mult_vec3_inv(&mat, &v30, &v30);
        vec3_negate(v30, v30);
        sub_1401EA970(a1, &v30);
        break;
    case MOTION_BONE_N_MOMO_A_R_WJ_CD_EX:
        v5 = a2[MOTION_BONE_CL_MOMO_R].node;
        mat4_transpose(v5[2].mat, &mat);
        mat4_get_translation(&mat, &v30);
        mat4_transpose(a1->node[0].mat, &mat);
        mat4_mult_vec3_inv(&mat, &v30, &v30);
        vec3_negate(v30, v30);
        sub_1401EA970(a1, &v30);
        break;
    case MOTION_BONE_N_HARA_CD_EX:
        v5 = a2[MOTION_BONE_KL_MUNE_B_WJ].node;
        mat4_transpose(v5[0].mat, &mat);
        mat4_get_translation(&mat, &v30);
        mat4_transpose(a1->node[0].mat, &mat);
        mat4_mult_vec3_inv(&mat, &v30, &v30);
        sub_1401EA970(a1, &v30);
        break;
    default:
        return false;
    }
    return true;
}

static float_t sub_1401E9D10(float_t a1) {
    bool neg;
    if (a1 < 0.0f) {
        a1 = -a1;
        neg = true;
    }
    else
        neg = false;

    a1 = fmodf(a1 + (float_t)M_PI, (float_t)(M_PI * 2.0)) - (float_t)M_PI;
    if (neg)
        return -a1;
    else
        return a1;
}

static bool sub_1401EA070(bone_data* a1, bone_node_expression_data* a2, bone_data* a3) {
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
        mat4u_to_mat4_transpose(&a3[MOTION_BONE_CL_KAO].rot_mat[0], &dst);
        mat4u_to_mat4_transpose(&a3[MOTION_BONE_CL_KAO].rot_mat[1], &mat);
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
        a2->rotation.x = sub_1401E9D10(v11->exp_data.rotation.x) * (float_t)(1.0 / 3.0);
        break;
    case MOTION_BONE_N_SUDE_R_WJ_EX:
    case MOTION_BONE_N_SUDE_B_R_WJ_EX:
        v11 = &a3[MOTION_BONE_KL_TE_R_WJ].node[0];
        a2->rotation.x = sub_1401E9D10(v11->exp_data.rotation.x) * (float_t)(1.0 / 3.0);
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
        a2->rotation.y = sub_1401E9D10(v11->exp_data.rotation.y
            + v11->exp_data.rotation.x) * (float_t)(1.0 / 3.0);
        break;
    case MOTION_BONE_N_MOMO_B_R_WJ_EX:
    case MOTION_BONE_N_MOMO_C_R_WJ_EX:
        v11 = &a3[MOTION_BONE_CL_MOMO_R].node[0];
        a2->rotation.y = sub_1401E9D10(v11->exp_data.rotation.y
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

void bone_data_mult_ik(bone_data* a1, int32_t skeleton_select) {
    if (a1->type < AFT_BONE_DATABASE_BONE_HEAD_IK_ROTATION)
        return;

    mat4 mat;
    mat4_transpose(a1->node[0].mat, &mat);

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
        mat4 pole_target_mat;
        mat4_transpose(a1->pole_target_mat, &pole_target_mat);
        vec3 pole_target;
        mat4_get_translation(&pole_target_mat, &pole_target);
        mat4_mult_vec3_inv(&mat, &pole_target, &pole_target);
        float_t pole_target_length;
        vec2_length(*(vec2*)&pole_target.y, pole_target_length);
        if (pole_target_length > 0.000001f) {
            vec3_mult_scalar(pole_target, 1.0f / pole_target_length, pole_target);
            float_t cos_val = pole_target.y;
            float_t sin_val = pole_target.z;
            mat4_rotate_x_mult_sin_cos(&mat, sin_val, cos_val, &mat);
            mat4_rotate_x_mult_sin_cos(&rot_mat, sin_val, cos_val, &rot_mat);
        }
    }

    if (a1->type == AFT_BONE_DATABASE_BONE_HEAD_IK_ROTATION) {
        mat4_to_mat4u_transpose(&rot_mat, &a1->rot_mat[1]);
        mat4_transpose(&mat, a1->node[1].mat);
        mat4_translate_mult(&mat, a1->ik_segment_length[skeleton_select], 0.0f, 0.0f, &mat);
        mat4_transpose(&mat, a1->node[2].mat);
        return;
    }

    float_t v20 = a1->ik_2nd_segment_length[skeleton_select];
    float_t v21 = a1->ik_segment_length[skeleton_select];
    float_t v22;
    float_t v23;
    float_t v24;
    float_t v25;
    if (fabsf(v8) > 0.000001f) {
        float_t v26 = a1->field_2E0;
        if (v26 > 0.0001f) {
            float_t v27 = (v21 + v20) * v26;
            if (v10 > v27) {
                v10 = v27;
                v8 = v27 * v27;
            }
        }
        float_t v28 = (v8 - v20 * v20) / v21;
        v23 = (v28 + v21) / (2.0f * v10);
        v22 = (v28 - v21) / (2.0f * v20);

        v23 = clamp(v23, -1.0f, 1.0f);
        v22 = clamp(v22, -1.0f, 1.0f);

        v24 = sqrtf(1.0f - v23 * v23);
        v25 = sqrtf(1.0f - v22 * v22);
        if (a1->type == AFT_BONE_DATABASE_BONE_LEGS_IK_ROTATION)
            v24 = -v24;
        else
            v25 = -v25;
    }
    else {
        v22 = -1.0f;
        v23 = 1.0f;
        v24 = 0.0f;
        v25 = 0.0f;
    }

    mat4_rotate_z_mult_sin_cos(&mat, v24, v23, &mat);
    mat4_transpose(&mat, a1->node[1].mat);
    mat4_rotate_z_mult_sin_cos(&rot_mat, v24, v23, &rot_mat);
    mat4_to_mat4u_transpose(&rot_mat, &a1->rot_mat[1]);
    mat4_translate_mult(&mat, v21, 0.0f, 0.0f, &mat);
    mat4_rotate_z_mult_sin_cos(&mat, v25, v22, &mat);
    mat4_transpose(&mat, a1->node[2].mat);
    mat4_rotate_z_sin_cos(v25, v22, &rot_mat);
    mat4_to_mat4u_transpose(&rot_mat, &a1->rot_mat[2]);
    mat4_translate_mult(&mat, v20, 0.0f, 0.0f, &mat);
    mat4_transpose(&mat, a1->node[3].mat);
}

static void bone_data_mult_0(bone_data* a1, int32_t skeleton_select) {
    if (a1->flags != 0)
        return;

    mat4 mat;
    if (a1->has_parent)
        mat4_transpose(a1->parent_mat, &mat);
    else
        mat = mat4_identity;
    
    if (a1->type == AFT_BONE_DATABASE_BONE_POSITION) {
        mat4_translate_mult(&mat, a1->trans.x, a1->trans.y, a1->trans.z, &mat);
        a1->rot_mat[0] = mat4u_identity;
    }
    else if (a1->type == AFT_BONE_DATABASE_BONE_TYPE_1) {
        mat4_mult_vec3_inv(&mat, &a1->trans, &a1->trans);
        mat4_translate_mult(&mat, a1->trans.x, a1->trans.y, a1->trans.z, &mat);
        a1->rot_mat[0] = mat4u_identity;
    }
    else {
        mat4 rot_mat;
        if (a1->type == AFT_BONE_DATABASE_BONE_POSITION_ROTATION) {
            mat4u_to_mat4_transpose(&a1->rot_mat[0], &rot_mat);
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
        mat4_to_mat4u_transpose(&rot_mat, &a1->rot_mat[0]);
    }

    mat4_transpose(&mat, a1->node[0].mat);

    bone_data_mult_ik(a1, skeleton_select);
}

void bone_data_mult_1(bone_data* a1, mat4* parent_mat, bone_data* a3, bool a4) {
    mat4 mat;
    mat4 rot_mat;
    if (a1->has_parent)
        mat4_transpose(a1->parent_mat,  &mat);
    else
        mat4_transpose(parent_mat, &mat);

    if (a1->type != AFT_BONE_DATABASE_BONE_TYPE_1 && a1->type != AFT_BONE_DATABASE_BONE_POSITION) {
        if (a1->type != AFT_BONE_DATABASE_BONE_POSITION_ROTATION)
            a1->trans = a1->base_translation[1];

        mat4_translate_mult(&mat, a1->trans.x, a1->trans.y, a1->trans.z, &mat);
        if (a4) {
            a1->node[0].exp_data.rotation = vec3_null;
            if (!a1->flags) {
                mat4u_to_mat4_transpose(&a1->rot_mat[0], &rot_mat);
                mat4_get_rotation(&rot_mat, &a1->node[0].exp_data.rotation);
            }
            else if (sub_1401EA070(a1, &a1->node[0].exp_data, a3)) {
                vec3 rotation = a1->node[0].exp_data.rotation;
                mat4_rotate(rotation.x, rotation.y, rotation.z, &rot_mat);
                mat4_to_mat4u_transpose(&rot_mat, &a1->rot_mat[0]);
            }
            else {
                mat4_transpose(&mat, a1->node[0].mat);

                if (sub_1401E9D60(a1, a3)) {
                    mat4 dst;
                    mat4_invrot_normalized(&mat, &rot_mat);
                    mat4_transpose(a1->node[0].mat, &dst);
                    mat4_mult(&dst, &rot_mat, &rot_mat);
                    mat4_clear_trans(&rot_mat, &rot_mat);
                    mat4_get_rotation(&rot_mat, &a1->node[0].exp_data.rotation);
                    mat4_to_mat4u_transpose(&rot_mat, &a1->rot_mat[0]);
                }
                else
                    a1->rot_mat[0] = mat4u_identity;
            }
        }

        mat4u_to_mat4_transpose(&a1->rot_mat[0], &rot_mat);
        mat4_mult(&rot_mat, &mat, &mat);

    }
    else {
        mat4_translate_mult(&mat, a1->trans.x, a1->trans.y, a1->trans.z, &mat);
        if (a4)
            a1->node[0].exp_data.rotation = vec3_null;
    }

    mat4_transpose(&mat, a1->node[0].mat);
    if (a4) {
        a1->node[0].exp_data.position = a1->trans;
        bone_node_expression_data_reset_scale(&a1->node[0].exp_data);
    }

    if (a1->type < AFT_BONE_DATABASE_BONE_HEAD_IK_ROTATION)
        return;

    mat4u_to_mat4_transpose(&a1->rot_mat[1], &rot_mat);
    mat4_mult(&rot_mat, &mat, &mat);
    mat4_transpose(&mat, a1->node[1].mat);
    mat4_translate_mult(&mat, a1->ik_segment_length[1], 0.0f, 0.0f, &mat);

    if (a1->type == AFT_BONE_DATABASE_BONE_HEAD_IK_ROTATION) {
        mat4_transpose(&mat, a1->node[2].mat);
        if (!a4)
            return;

        a1->node[1].exp_data.position = vec3_null;
        mat4u_to_mat4_transpose(&a1->rot_mat[1], &rot_mat);
        mat4_get_rotation(&rot_mat, &a1->node[1].exp_data.rotation);
        bone_node_expression_data_reset_scale(&a1->node[1].exp_data);
        bone_node_expression_data_set_position_rotation(&a1->node[2].exp_data,
            a1->ik_segment_length[1], 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    }
    else {
        mat4u_to_mat4_transpose(&a1->rot_mat[2], &rot_mat);
        mat4_mult(&rot_mat, &mat, &mat);
        mat4_transpose(&mat, a1->node[2].mat);
        mat4_translate_mult(&mat, a1->ik_2nd_segment_length[1], 0.0f, 0.0f, &mat);
        mat4_transpose(&mat, a1->node[3].mat);
        if (!a4)
            return;

        a1->node[1].exp_data.position = vec3_null;
        mat4u_to_mat4_transpose(&a1->rot_mat[1], &rot_mat);
        mat4_get_rotation(&rot_mat, &a1->node[1].exp_data.rotation);
        bone_node_expression_data_reset_scale(&a1->node[1].exp_data);
        a1->node[2].exp_data.position.x = a1->ik_segment_length[1];
        *(vec2*)&a1->node[2].exp_data.position.y = vec2_null;
        mat4u_to_mat4_transpose(&a1->rot_mat[2], &rot_mat);
        mat4_get_rotation(&rot_mat, &a1->node[2].exp_data.rotation);
        bone_node_expression_data_reset_scale(&a1->node[2].exp_data);
        bone_node_expression_data_set_position_rotation(&a1->node[3].exp_data,
            a1->ik_2nd_segment_length[1], 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    }
}

void bone_data_mult_2(rob_chara_data_bone_data* rob_bone_data, mat4* mat) {
    for (bone_node* i = rob_bone_data->nodes.begin; i != rob_bone_data->nodes.end; i++)
        mat4_mult(i->mat, mat, i->mat);
    sub_140414900(&rob_bone_data->motion_loaded.head->next->value->field_4F8, mat);
}

static void sub_140413EB0(struc_308* a1) {
    a1->field_8C = false;
    a1->field_4C = a1->mat;
    a1->mat = mat4u_identity;
}

static void sub_1404117F0(motion_blend_mot* a1) {
    sub_140413EB0(&a1->field_4F8);
    int32_t skeleton_select = a1->mot_data.skeleton_select;
    for (bone_data* i = a1->bone_data.bones.begin; i != a1->bone_data.bones.end; i++)
        bone_data_mult_0(i, skeleton_select);
}

static void sub_14040FBF0(motion_blend_mot* a1, float_t a2) {
    bone_data* b_n_hara_cp = &a1->bone_data.bones.begin[MOTION_BONE_N_HARA_CP];
    bone_data* b_kg_hara_y = &a1->bone_data.bones.begin[MOTION_BONE_KG_HARA_Y];
    bone_data* b_kl_hara_xz = &a1->bone_data.bones.begin[MOTION_BONE_KL_HARA_XZ];
    bone_data* b_kl_hara_etc = &a1->bone_data.bones.begin[MOTION_BONE_KL_HARA_ETC];
    a1->field_4F8.field_90 = vec3_null;

    mat4 rot_mat;
    mat4 mat;
    mat4u_to_mat4_transpose(&b_n_hara_cp->rot_mat[0], &rot_mat);
    mat4u_to_mat4_transpose(&b_kg_hara_y->rot_mat[0], &mat);
    mat4_mult(&mat, &rot_mat, &rot_mat);
    mat4u_to_mat4_transpose(&b_kl_hara_xz->rot_mat[0], &mat);
    mat4_mult(&mat, &rot_mat, &rot_mat);
    mat4u_to_mat4_transpose(&b_kl_hara_etc->rot_mat[0], &mat);
    mat4_mult(&mat, &rot_mat, &rot_mat);
    mat4_to_mat4u_transpose(&rot_mat, &b_n_hara_cp->rot_mat[0]);
    b_kg_hara_y->rot_mat[0] = mat4u_identity;
    b_kl_hara_xz->rot_mat[0] = mat4u_identity;
    b_kl_hara_etc->rot_mat[0] = mat4u_identity;

    float_t v8 = a1->field_4F8.field_C0;
    float_t v9 = a1->field_4F8.field_C4;
    vec3 v10 = a1->field_4F8.field_C8;
    a1->field_4F8.field_A8 = b_n_hara_cp->trans;
    if (!a1->mot_data.skeleton_select) {
        if (a2 != v9) {
            b_n_hara_cp->trans.x = (b_n_hara_cp->trans.x - v10.x) * v9 + v10.x;
            b_n_hara_cp->trans.z = (b_n_hara_cp->trans.z - v10.z) * v9 + v10.z;
        }
        b_n_hara_cp->trans.y = ((b_n_hara_cp->trans.y - v10.y) * v8) + v10.y;
    }
    else {
        if (a2 != v9) {
            v9 /= a2;
            b_n_hara_cp->trans.x = (b_n_hara_cp->trans.x - v10.x) * v9 + v10.x;
            b_n_hara_cp->trans.z = (b_n_hara_cp->trans.z - v10.z) * v9 + v10.z;
        }

        if (a2 != v8) {
            v8 /= a2;
            b_n_hara_cp->trans.y = (b_n_hara_cp->trans.y - v10.y) * v8 + v10.y;
        }
    }
}

static bool sub_1404136B0(motion_blend_mot* a1) {
    motion_blend* v1 = a1->field_5D0;
    if (v1)
        return v1->__vftable->field_30(v1);
    else
        return false;
}

static bool sub_140410250(struc_313* a1, size_t a2) {
    return (a1->bitfield.begin[a2 >> 5] & (1 << (a2 & 0x1F))) != 0;
}

static void sub_140410A40(motion_blend_mot* a1, vector_bone_data* a2, vector_bone_data* a3) {
    motion_blend* v1 = a1->field_5D0;
    if (!v1 || !v1->field_9)
        return;

    v1->__vftable->field_20(v1, a2, a3);
    for (bone_data* i = a2->begin; i != a2->end; i++) {
        if (!sub_140410250(&a1->field_0.field_8, i->motion_bone_index))
            continue;

        bone_data* v8 = 0;
        if (a3)
            v8 = &a3->begin[i->motion_bone_index];
        v1->__vftable->field_28(v1, &a2->begin[i->motion_bone_index], v8);
    }
}

static void sub_140410B70(motion_blend_mot* a1, vector_bone_data* a2) {
    motion_blend* v1 = a1->field_5D0;
    if (!v1 || !v1->field_9)
        return;

    v1->__vftable->field_20(v1, &a1->bone_data.bones, a2);
    for (bone_data* i = a1->bone_data.bones.begin; i != a1->bone_data.bones.end; i++) {
        if (!sub_140410250(&a1->field_0.field_8, i->motion_bone_index))
            continue;

        bone_data* v6 = 0;
        if (a2)
            v6 = &a2->begin[i->motion_bone_index];
        v1->__vftable->field_28(a1->field_5D0, &a1->bone_data.bones.begin[i->motion_bone_index], v6);
    }
}

static motion_blend_type sub_1404125C0(motion_blend_mot* a1) {
    motion_blend* v1 = a1->field_5D0;
    if (v1 == &a1->cross.base)
        return MOTION_BLEND_CROSS;
    if (v1 == &a1->freeze.base)
        return MOTION_BLEND_FREEZE;
    if (v1 == &a1->combine.base.base)
        return MOTION_BLEND_COMBINE;
    return MOTION_BLEND;
}

static void sub_140410CB0(mot_blend* a1, vector_bone_data* a2) {
    motion_blend* v1 = &a1->field_F8;
    if (!v1->field_9)
        return;

    for (bone_data* i = a2->begin; i != a2->end; i++)
        if (sub_140410250(&a1->field_0.field_8, i->motion_bone_index))
            v1->__vftable->field_28(&a1->field_F8, &a2->begin[i->motion_bone_index], 0);
}

static void sub_1404182B0(rob_chara_data_bone_data* rob_bone_data) {
    if (!rob_bone_data->motion_loaded.size)
        return;

    list_ptr_motion_blend_mot_node* head = rob_bone_data->motion_loaded.head;
    list_ptr_motion_blend_mot_node* v2 = head;
    while (v2 != head->next) {
        bool v2a = v2 != head;
        v2 = v2->prev;
        if (sub_1404136B0(v2->value))
            if (v2a)
                sub_140410A40(v2->value, &v2->prev->value->bone_data.bones,
                    &v2->value->bone_data.bones);
    }

    list_ptr_motion_blend_mot_node* v5 = head;
    while (v5 != head->next) {
        bool v5a = v5 != head;
        v5 = v5->prev;
        if (!sub_1404136B0(v5->value))
            if (v5a)
                sub_140410B70(v5->value, &v5->next->value->bone_data.bones);
            else if (sub_1404125C0(v5->value) == MOTION_BLEND_FREEZE)
                sub_140410B70(v5->value, 0);
    }

    motion_blend_mot* v3 = head->next->value;
    sub_140410CB0(&rob_bone_data->face, &v3->bone_data.bones);
    sub_140410CB0(&rob_bone_data->hand_l, &v3->bone_data.bones);
    sub_140410CB0(&rob_bone_data->hand_r, &v3->bone_data.bones);
    sub_140410CB0(&rob_bone_data->mouth, &v3->bone_data.bones);
    sub_140410CB0(&rob_bone_data->eye, &v3->bone_data.bones);
    sub_140410CB0(&rob_bone_data->eyelid, &v3->bone_data.bones);

    bone_data* b_n_hara_cp = &v3->bone_data.bones.begin[MOTION_BONE_N_HARA_CP];
    v3->field_4F8.field_9C = b_n_hara_cp->trans;
    if (v3->field_4F8.field_0 & 2) {
        v3->field_4F8.field_90 = b_n_hara_cp->trans;
        b_n_hara_cp->trans = vec3_null;
    }
}

static void sub_14041B9F0(rob_chara_data_bone_data* rob_bone_data) {
    list_ptr_motion_blend_mot_node* v1 = rob_bone_data->motion_loaded.head;
    list_ptr_motion_blend_mot_node* v3 = v1->next;
    while (v3 != v1) {
        sub_1404146F0(&v3->value->field_0);
        v3 = v3->next;
    }
    sub_1404146F0(&rob_bone_data->face.field_0);
    sub_1404146F0(&rob_bone_data->eyelid.field_0);
}

static void motion_blend_mot_mult_mat(motion_blend_mot* a1, mat4* a2) {
    sub_140414900(&a1->field_4F8, a2);

    mat4 mat;
    mat4u_to_mat4(&a1->field_4F8.mat, &mat);
    bone_data* v3 = a1->bone_data.bones.begin;
    for (bone_data* v4 = a1->bone_data.bones.begin; v4 != a1->bone_data.bones.end; v4++)
        bone_data_mult_1(v4, &mat, v3, true);
}

void rob_chara_data_bone_data_update(rob_chara_data_bone_data* rob_bone_data, mat4* a2) {
    if (!rob_bone_data->motion_loaded.size)
        return;

    list_ptr_motion_blend_mot_node* v4 = rob_bone_data->motion_loaded.head;
    list_ptr_motion_blend_mot_node* v5 = v4->next;
    while (v5 != v4) {
        sub_1404117F0(v5->value);
        sub_14040FBF0(v5->value, rob_bone_data->ik_scale.ratio0);
        v5 = v5->next;
    }
    sub_1404182B0(rob_bone_data);
    sub_14041B9F0(rob_bone_data);
    motion_blend_mot_mult_mat(rob_bone_data->motion_loaded.head->next->value, a2);
}

static mot_blend* mot_blend_init(mot_blend* a1) {
    memset(a1, 0, sizeof(mot_blend));
    a1->field_38.frame = -1.0f;
    a1->field_38.motion_id = -1;
    a1->field_A8.field_8 = 1.0f;
    a1->field_A8.field_4 = 1.0f;
    a1->field_A8.field_10 = -1.0f;
    a1->field_A8.field_14 = -1.0f;
    a1->field_A8.field_18 = -1;
    a1->field_A8.field_1C = -1;
    a1->field_A8.field_24 = -1.0f;
    a1->field_A8.field_2C = -1;
    a1->field_F8.field_14 = 1.0f;
    return a1;
}

static void rob_chara_data_bone_data_eyes_xrot_adjust(rob_chara_data_bone_data* rob_bone_data,
    struc_241* a2, eyes_adjust* a3) {
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

static void rob_chara_data_bone_data_init_skeleton(rob_chara_data_bone_data* rob_bone_data,
    aft_bone_database_skeleton_type base_skeleton_type,
    aft_bone_database_skeleton_type skeleton_type) {
    if (rob_bone_data->base_skeleton_type == base_skeleton_type
        && rob_bone_data->skeleton_type == skeleton_type)
        return;

    aft_bone_database_bone* bones = aft_bone_database_get_bones_by_type(base_skeleton_type);
    rob_chara_data_bone_data_calculate_bones(rob_bone_data, bones);
    rob_chara_data_bone_data_reserve(rob_bone_data);
    rob_chara_data_bone_data_set_mats(rob_bone_data, bones);
    uint16_t* parent_indices = aft_bone_database_get_skeleton_parent_indices_by_type(base_skeleton_type);
    rob_chara_data_bone_data_set_parent_mats(rob_bone_data, parent_indices);
    rob_bone_data->base_skeleton_type = base_skeleton_type;
    rob_bone_data->skeleton_type = skeleton_type;
}

static bool motion_blend_mot_check(motion_bone_index bone_index) {
    return true;
}

static bool mot_blend_face_check(motion_bone_index bone_index) {
    return bone_index >= MOTION_BONE_FACE_ROOT && bone_index <= MOTION_BONE_TL_TOOTH_UPPER_WJ;
}

static bool mot_blend_hand_l_check(motion_bone_index bone_index) {
    return bone_index >= MOTION_BONE_N_HITO_L_EX && bone_index <= MOTION_BONE_NL_OYA_C_L_WJ;
}

static bool mot_blend_hand_r_check(motion_bone_index bone_index) {
    return bone_index >= MOTION_BONE_N_HITO_R_EX && bone_index <= MOTION_BONE_NL_OYA_C_R_WJ;
}

static bool mot_blend_mouth_check(motion_bone_index bone_index) {
    return (bone_index >= MOTION_BONE_N_AGO && bone_index <= MOTION_BONE_TL_TOOTH_UNDER_WJ)
        || (bone_index >= MOTION_BONE_N_KUTI_D && bone_index <= MOTION_BONE_TL_KUTI_U_R_WJ)
        || (bone_index >= MOTION_BONE_N_TOOTH_UPPER && bone_index <= MOTION_BONE_TL_TOOTH_UPPER_WJ);
}

static bool mot_blend_eye_check(motion_bone_index bone_index) {
    return bone_index >= MOTION_BONE_N_EYE_L && bone_index <= MOTION_BONE_KL_HIGHLIGHT_R_WJ;
}

static bool mot_blend_eyelid_check(motion_bone_index bone_index) {
    return (bone_index >= MOTION_BONE_N_EYE_L && bone_index <= MOTION_BONE_KL_HIGHLIGHT_R_WJ)
        || (bone_index >= MOTION_BONE_N_EYELID_L_A && bone_index <= MOTION_BONE_TL_EYELID_R_B_WJ)
        || (bone_index >= MOTION_BONE_N_MABU_L_D_A && bone_index <= MOTION_BONE_TL_MABU_R_U_C_WJ);
}

static void mot_parent_get_key_set_count_by_bone_database_bones(mot_parent* a1, aft_bone_database_bone* bones) {
    size_t object_bone_count;
    size_t motion_bone_count;
    size_t total_bone_count;
    size_t ik_bone_count;
    size_t chain_pos;
    aft_bone_database_bones_calculate_count(bones, &object_bone_count,
        &motion_bone_count, &total_bone_count, &ik_bone_count, &chain_pos);
    mot_parent_get_key_set_count(a1, motion_bone_count, ik_bone_count);
}

static void mot_parent_reserve_key_sets_by_skeleton_type(mot_parent* a1,
    aft_bone_database_skeleton_type type) {
    mot_parent_get_key_set_count_by_bone_database_bones(a1, aft_bone_database_get_bones_by_type(type));
    mot_parent_reserve_key_sets(a1);
    a1->skeleton_type = type;
}

static void mot_blend_reserve_key_sets(mot_blend* blend, aft_bone_database_skeleton_type type,
    bool(*a3)(motion_bone_index), size_t motion_bone_count) {
    mot_parent_reserve_key_sets_by_skeleton_type(&blend->field_38, type);
    sub_140413350(&blend->field_0, a3, motion_bone_count);
}

static void rob_chara_data_bone_data_init_data(rob_chara_data_bone_data* rob_bone_data,
    aft_bone_database_skeleton_type base_type,
    aft_bone_database_skeleton_type type) {
    rob_chara_data_bone_data_init_skeleton(rob_bone_data, base_type, type);
    for (motion_blend_mot** i = rob_bone_data->motions.begin; i != rob_bone_data->motions.end; i++)
        motion_blend_mot_load_bone_data(*i, rob_bone_data, motion_blend_mot_check);
    size_t motion_bone_count = rob_bone_data->motion_bone_count;
    mot_blend_reserve_key_sets(&rob_bone_data->face, base_type, mot_blend_face_check, motion_bone_count);
    mot_blend_reserve_key_sets(&rob_bone_data->hand_l, base_type, mot_blend_hand_l_check, motion_bone_count);
    mot_blend_reserve_key_sets(&rob_bone_data->hand_r, base_type, mot_blend_hand_r_check, motion_bone_count);
    mot_blend_reserve_key_sets(&rob_bone_data->mouth, base_type, mot_blend_mouth_check, motion_bone_count);
    mot_blend_reserve_key_sets(&rob_bone_data->eye, base_type, mot_blend_eye_check, motion_bone_count);
    mot_blend_reserve_key_sets(&rob_bone_data->eyelid, base_type, mot_blend_eyelid_check, motion_bone_count);
}

static void bone_data_parent_reset(bone_data_parent* a1) {
    a1->rob_bone_data = 0;
    a1->field_8 = false;
    a1->field_9 = false;
    a1->motion_bone_count = 0;
    a1->global_trans = vec3_null;
    a1->global_rotation = vec3_null;
    a1->ik_bone_count = 0;
    a1->chain_pos = 0;
    a1->bone_key_set_count = 0;
    a1->global_key_set_count = 0;
    a1->field_78 = 0;
    a1->rotation_y = 0;
    a1->bones.end = a1->bones.begin;
    a1->bone_indices.end = a1->bone_indices.begin;
}

static void sub_140413470(struc_308* a1) {
    a1->field_0 = 0;
    a1->field_8 = 0;
    a1->mat = mat4u_identity;
    a1->field_4C = mat4u_identity;
    a1->field_8C = false;
    a1->eyes_adjust = 0;
    a1->field_B8 = 0;
    a1->field_90 = vec3_identity;
    a1->field_9C = vec3_identity;
    a1->field_A8 = vec3_identity;
    a1->field_BC = 0;
    a1->field_BD = 0;
    a1->field_C0 = 1.0f;
    a1->field_C4 = 1.0f;
    a1->field_C8 = vec3_null;
}

static void motion_blend_mot_reset(motion_blend_mot* a1) {
    bone_data_parent_reset(&a1->bone_data);
    a1->mot_data.key_sets_ready = 0;
    a1->mot_data.skeleton_type = AFT_BONE_DATABASE_SKELETON_NONE;
    a1->mot_data.frame = -1.0f;
    a1->mot_data.key_set_count = 0;
    a1->mot_data.mot_data = 0;
    a1->mot_data.skeleton_select = 0;
    a1->mot_data.field_68.field_0 = 0;
    a1->mot_data.field_68.field_4 = 0.0f;
    a1->mot_data.key_set.end = a1->mot_data.key_set.begin;
    a1->mot_data.key_set_data.end = a1->mot_data.key_set_data.begin;
    a1->mot_data.motion_id = -1;
    a1->field_4A8.field_0.frame = 0.0f;
    a1->field_4A8.field_0.field_8 = 1.0f;
    a1->field_4A8.field_0.field_4 = 1.0f;
    a1->field_4A8.field_0.field_C = 0.0f;
    a1->field_4A8.field_0.field_10 = -1.0f;
    a1->field_4A8.field_0.field_14 = -1.0f;
    a1->field_4A8.field_0.field_18 = -1;
    a1->field_4A8.field_0.field_1C = -1;
    a1->field_4A8.field_0.field_20 = 0.0f;
    a1->field_4A8.field_0.field_24 = -1.0f;
    a1->field_4A8.field_0.field_28 = 0;
    a1->field_4A8.field_0.field_2C = -1;
    a1->field_4A8.field_30 = 0;
    a1->field_4A8.field_34 = 0;
    a1->field_4A8.field_38 = 0.0f;
    a1->field_4A8.field_40 = 0;
    a1->field_4A8.ex_data_mat = 0;
    sub_140413470(&a1->field_4F8);
    a1->field_0.bone_check_func = 0;
    sub_1404119A0(&a1->field_0.field_8);
    a1->field_0.motion_bone_count = 0;
    a1->cross.base.__vftable->field_8(&a1->cross.base);
    a1->freeze.base.__vftable->field_8(&a1->freeze.base);
    a1->combine.base.base.__vftable->field_8(&a1->combine.base.base);
    a1->field_5D0 = 0;
}

static void mot_blend_reset(mot_blend* a1) {
    a1->field_38.key_sets_ready = 0;
    a1->field_38.skeleton_type = AFT_BONE_DATABASE_SKELETON_NONE;
    a1->field_38.frame = -1.0f;
    a1->field_38.key_set_count = 0;
    a1->field_38.mot_data = 0;
    a1->field_38.skeleton_select = 0;
    a1->field_38.field_68.field_0 = 0;
    a1->field_38.field_68.field_4 = 0.0f;
    a1->field_38.key_set.end = a1->field_38.key_set.begin;
    a1->field_38.key_set_data.end = a1->field_38.key_set_data.begin;
    a1->field_38.motion_id = -1;
    a1->field_A8.frame = 0.0f;
    a1->field_A8.field_8 = 1.0f;
    a1->field_A8.field_4 = 1.0f;
    a1->field_A8.field_C = 0.0f;
    a1->field_A8.field_10 = -1.0f;
    a1->field_A8.field_14 = -1.0f;
    a1->field_A8.field_18 = -1;
    a1->field_A8.field_1C = -1;
    a1->field_A8.field_20 = 0.0f;
    a1->field_A8.field_24 = -1.0f;
    a1->field_A8.field_28 = 0;
    a1->field_A8.field_2C = -1;
    a1->field_D8 = 0;
    a1->field_DC = 0;
    a1->field_E0 = 0;
    a1->field_E8 = 0;
    a1->field_F0 = 0;
    a1->field_F8.__vftable->field_8(&a1->field_F8);
    a1->field_0.bone_check_func = 0;
    sub_1404119A0(&a1->field_0.field_8);
    a1->field_0.motion_bone_count = 0;
    a1->field_30 = 0;
}

static void sub_1404198D0(struc_258* a1) {
    a1->field_0 = 0;
    a1->field_8 = mat4u_identity;
    a1->ex_data_mat.field_0 = 0.0f;
    a1->ex_data_mat.field_4 = 0.0f;
    a1->ex_data_mat.field_8 = 0.0f;
    a1->ex_data_mat.field_C = 0.0f;
    a1->ex_data_mat.field_10 = 0.0f;
    a1->ex_data_mat.field_14 = 0.0f;
    a1->ex_data_mat.field_18 = 0.0f;
    a1->ex_data_mat.field_1C = 0.0f;
    a1->ex_data_mat.field_20 = 0.0f;
    a1->ex_data_mat.field_24 = 0.0f;
    a1->ex_data_mat.field_28 = 0.0f;
    a1->ex_data_mat.field_2C = 1.0f;
    a1->ex_data_mat.field_30 = 1.0f;
    a1->ex_data_mat.field_34 = -3.8f;
    a1->ex_data_mat.field_38 = 6.0f;
    a1->ex_data_mat.field_3C = -3.8f;
    a1->ex_data_mat.field_40 = 6.0f;
    a1->field_8C = 0;
    a1->field_8D = 0;
    a1->field_8E = 0;
    a1->field_8F = 0;
    a1->field_90 = 0;
    a1->field_91 = 0;
    a1->field_94 = 0.0f;
    a1->field_98 = 1.0f;
    a1->field_9C = 1.0f;
    a1->field_A0 = 0;
    a1->field_A4 = 0;
    a1->field_A8 = 0;
    a1->field_AC = 0;
    a1->eyes_adjust = 0;
    a1->field_B8 = 0;
    a1->field_B0 = 0;
    a1->field_BC = 0;
    a1->field_C0 = vec3_null;
    a1->field_CC = mat4u_identity;
    a1->field_10C = mat4u_identity;
    a1->field_14C = 0;
    a1->field_150 = 0;
    a1->field_158 = 0.0f;
    a1->field_15C.field_0 = 0;
    a1->field_15C.field_4 = 0;
    a1->field_15C.field_8 = 0;
    a1->field_15C.field_C = 0;
    a1->field_15C.field_10 = 0;
    a1->field_15C.field_14 = 0;
    a1->field_15C.field_18 = 0;
    a1->field_15C.field_1C = 0;
    a1->field_15C.field_20 = 0;
    a1->field_15C.field_24 = 0;
    a1->field_184 = 0;
    a1->field_188 = 0;
    a1->field_18C = 0;
    a1->field_190 = 0;
    a1->field_194 = 0;
    a1->field_195 = 0;
    a1->field_198 = 0;
    a1->field_19C = 0;
    a1->field_1A0 = 0;
    a1->field_1A4 = 0;
    a1->field_1A8 = 0;
    a1->field_1AC = 0;
    a1->field_1B0 = 0;
    a1->field_1B4 = 0;
    a1->field_1B8 = 0;
    a1->field_1C8 = 0;
    a1->field_1BC = vec2_identity;
    a1->field_1C4 = 0;
}

static void sub_1403FAEF0(struc_312* a1) {
    a1->field_0.field_0 = 0;
    a1->field_0.field_4 = 0;
    a1->field_0.field_8 = 0;
    a1->field_0.field_C = 0;
    a1->field_0.field_10 = 0;
    a1->field_0.field_14 = 0;
    a1->field_0.field_18 = 0;
    a1->field_0.field_1C = 0;
    a1->field_0.field_20 = 0;
    a1->field_0.field_24 = 0;
    a1->field_0.field_28 = 0;
    a1->field_2C.field_0 = 0;
    a1->field_2C.field_4 = 0;
    a1->field_2C.field_8 = 0;
    a1->field_2C.field_C = 0;
    a1->field_2C.field_10 = 0;
    a1->field_2C.field_14 = 0;
    a1->field_2C.field_18 = 0;
    a1->field_2C.field_1C = 0;
    a1->field_2C.field_20 = 0;
    a1->field_2C.field_24 = 0;
    a1->field_2C.field_28 = 0;
    a1->field_58 = 0;
    a1->field_8C = 1.0f;
    a1->field_90 = 0;
    a1->field_94 = 0;
    a1->field_98 = 1.0f;
    a1->field_5C = 0;
    a1->field_68 = 0;
    a1->field_74 = 0;
    a1->field_80 = 0;
    a1->field_60 = 0;
    a1->field_6C = 0;
    a1->field_78 = 0;
    a1->field_84 = 0;
    a1->field_64 = 0;
    a1->field_70 = 0;
    a1->field_7C = 0;
    a1->field_88 = 0;
}

static void rob_chara_data_bone_data_reset(rob_chara_data_bone_data* rob_bone_data) {
    void(*rob_chara_data_bone_data_motion_blend_mot_list_free)(rob_chara_data_bone_data * rob_bone_data,
        size_t a2) = (void*)0x0000000140418E80;
    void(*rob_chara_data_bone_data_motion_blend_mot_list_init)(rob_chara_data_bone_data * rob_bone_data)
        = (void*)0x000000014041A9E0;
    void(*rob_chara_data_bone_data_motion_blend_mot_free)(rob_chara_data_bone_data * rob_bone_data)
        = (void*)0x0000000140419000;
    void(*rob_chara_data_bone_data_motion_blend_mot_init)(rob_chara_data_bone_data * rob_bone_data)
        = (void*)0x000000014041A1A0;

    rob_bone_data->field_0 = 0;
    rob_bone_data->field_1 = 0;
    rob_bone_data->object_bone_count = 0;
    rob_bone_data->motion_bone_count = 0;
    rob_bone_data->total_bone_count = 0;
    rob_bone_data->ik_bone_count = 0;
    rob_bone_data->chain_pos = 0;
    rob_bone_data->mats.end = rob_bone_data->mats.begin;
    rob_bone_data->mats2.end = rob_bone_data->mats2.begin;
    rob_bone_data->nodes.end = rob_bone_data->nodes.begin;
    rob_bone_data->base_skeleton_type = AFT_BONE_DATABASE_SKELETON_NONE;
    rob_bone_data->skeleton_type = AFT_BONE_DATABASE_SKELETON_NONE;

    if (rob_bone_data->motion_loaded.size
        && rob_bone_data->motions.end - rob_bone_data->motions.begin == 3) {
        rob_chara_data_bone_data_motion_blend_mot_list_free(rob_bone_data, 0);
        for (motion_blend_mot** i = rob_bone_data->motions.begin; i != rob_bone_data->motions.end; i++)
            motion_blend_mot_reset(*i);
        rob_chara_data_bone_data_motion_blend_mot_list_init(rob_bone_data);
    }
    else {
        rob_chara_data_bone_data_motion_blend_mot_free(rob_bone_data);
        rob_chara_data_bone_data_motion_blend_mot_init(rob_bone_data);
    }

    mot_blend_reset(&rob_bone_data->face);
    mot_blend_reset(&rob_bone_data->hand_l);
    mot_blend_reset(&rob_bone_data->hand_r);
    mot_blend_reset(&rob_bone_data->mouth);
    mot_blend_reset(&rob_bone_data->eye);
    mot_blend_reset(&rob_bone_data->eyelid);
    rob_bone_data->field_758 = 0;
    rob_bone_data->ik_scale.ratio0 = 1.0f;
    rob_bone_data->ik_scale.ratio1 = 1.0f;
    rob_bone_data->ik_scale.ratio2 = 1.0f;
    rob_bone_data->ik_scale.ratio3 = 1.0f;
    rob_bone_data->field_76C = vec3_null;
    rob_bone_data->field_778 = vec3_null;
    sub_1404198D0(&rob_bone_data->field_788);
    sub_1403FAEF0(&rob_bone_data->field_958);
}

static skeleton_rotation_offset* skeleton_rotation_offset_array_get(aft_bone_database_skeleton_type type) {
    skeleton_rotation_offset** skeleton_rotation_offset_array_ptr = (void*)0x0000000140A01560;
    if (type >= AFT_BONE_DATABASE_SKELETON_COMMON && type <= AFT_BONE_DATABASE_SKELETON_TETO)
        return skeleton_rotation_offset_array_ptr[type];
    else
        return 0;
}

static vec3* bone_data_set_key_data(bone_data* data, vec3* keyframe_data,
    aft_bone_database_skeleton_type skeleton_type, bool get_data, bool reverse_x) {
    aft_bone_database_bone_type type = data->type;
    if (type == AFT_BONE_DATABASE_BONE_POSITION_ROTATION) {
        if (get_data) {
            data->trans = *keyframe_data;
            if (reverse_x)
                data->trans.x = -data->trans.x;
        }
        keyframe_data++;
    }
    else if (type >= AFT_BONE_DATABASE_BONE_HEAD_IK_ROTATION
        && type <= AFT_BONE_DATABASE_BONE_LEGS_IK_ROTATION) {
        if (get_data) {
            data->ik_target = *keyframe_data;
            if (reverse_x)
                data->ik_target.x = -data->ik_target.x;
        }
        keyframe_data++;
    }

    if (get_data) {
        if (type == AFT_BONE_DATABASE_BONE_TYPE_1 || type == AFT_BONE_DATABASE_BONE_POSITION) {
            data->trans = *keyframe_data;
            if (reverse_x)
                data->trans.x = -data->trans.x;
        }
        else if (!data->flags) {
            data->rotation = *keyframe_data;
            if (reverse_x) {
                skeleton_rotation_offset* rot_off = skeleton_rotation_offset_array_get(skeleton_type);
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

static void mot_parent_interpolate(mot_parent* a1, float_t frame,
    uint32_t key_set_offset, uint32_t key_set_count) {
    mot_key_frame_interpolate(&a1->mot, frame,
        &a1->key_set_data.begin[key_set_offset],
        &a1->key_set.begin[key_set_offset], key_set_count, &a1->field_68);
}

static bool motion_blend_mot_interpolate_get_reverse(int32_t* a1) {
    return (*a1 & 1) && (*a1 & 8) == 0 || !(*a1 & 1) && (*a1 & 8) != 0;
}

static void sub_140415430(motion_blend_mot* a1) {
    float_t rotation_y = a1->bone_data.rotation_y;
    vec3 global_trans = a1->bone_data.global_trans;
    vec3 global_rotation = a1->bone_data.global_rotation;
    mat4 mat;
    mat4_rotate_y(rotation_y, &mat);
    mat4_translate_mult(&mat, global_trans.x, global_trans.y, global_trans.z, &mat);
    mat4_rotate_mult(&mat, global_rotation.x, global_rotation.y, global_rotation.z, &mat);
    for (bone_data* i = a1->bone_data.bones.begin; i != a1->bone_data.bones.end; i++)
        switch (i->type) {
        case AFT_BONE_DATABASE_BONE_TYPE_1:
            mat4_mult_vec3_trans(&mat, &i->trans, &i->trans);
            break;
        case AFT_BONE_DATABASE_BONE_POSITION_ROTATION: {
            mat4 rot_mat;
            mat4_clear_trans(&mat, &rot_mat);
            mat4_to_mat4u_transpose(&rot_mat, &i->rot_mat[0]);
            mat4_mult_vec3_trans(&mat, &i->trans, &i->trans);
        } break;
        case AFT_BONE_DATABASE_BONE_HEAD_IK_ROTATION:
        case AFT_BONE_DATABASE_BONE_ARM_IK_ROTATION:
        case AFT_BONE_DATABASE_BONE_LEGS_IK_ROTATION:
            mat4_mult_vec3_trans(&mat, &i->ik_target, &i->ik_target);
            break;
        }
}

static void motion_blend_mot_interpolate(motion_blend_mot* a1) {
    vec3* keyframe_data = (vec3*)a1->mot_data.key_set_data.begin;
    bool reverse = motion_blend_mot_interpolate_get_reverse(&a1->field_4F8.field_0);
    float_t frame = a1->field_4A8.field_0.frame;
    
    aft_bone_database_skeleton_type skeleton_type = a1->bone_data.rob_bone_data->base_skeleton_type;
    bone_data* bones_data = a1->bone_data.bones.begin;
    for (uint16_t* i = a1->bone_data.bone_indices.begin; i != a1->bone_data.bone_indices.end; i++) {
        bone_data* data = &bones_data[*i];
        bool get_data = sub_140410250(&a1->field_0.field_8, data->motion_bone_index);
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

    sub_140415430(a1);
}

static void mot_blend_interpolate(mot_blend* a1, vector_bone_data* bones,
    vector_uint16_t* bone_indices, aft_bone_database_skeleton_type skeleton_type) {
    if (!a1->field_38.key_sets_ready || !a1->field_38.mot_data || a1->field_30)
        return;

    float_t frame = a1->field_A8.frame;
    vec3* keyframe_data = (vec3*)a1->field_38.key_set_data.begin;
    bone_data* bones_data = bones->begin;
    for (uint16_t* i = bone_indices->begin; i != bone_indices->end; i++) {
        bone_data* data = &bones_data[*i];
        bool get_data = sub_140410250(&a1->field_0.field_8, data->motion_bone_index);
        if (get_data) {
            mot_parent_interpolate(&a1->field_38, frame, data->key_set_offset, data->key_set_count);
            data->frame = frame;
        }
        keyframe_data = bone_data_set_key_data(data, keyframe_data, skeleton_type, get_data, 0);
    }
}

static void sub_1401EB1D0(bone_data* a1, int32_t a2) {
    if (!a1->flags)
        return;

    if (a1->type >= AFT_BONE_DATABASE_BONE_TYPE_1 && a1->type <= AFT_BONE_DATABASE_BONE_POSITION_ROTATION)
        a1->trans_dup[a2] = a1->trans;

    switch (a1->type) {
    case AFT_BONE_DATABASE_BONE_ARM_IK_ROTATION:
    case AFT_BONE_DATABASE_BONE_LEGS_IK_ROTATION:
        a1->rot_mat_dup[a2 + 4] = a1->rot_mat[2];
    case AFT_BONE_DATABASE_BONE_HEAD_IK_ROTATION:
        a1->rot_mat_dup[a2 + 2] = a1->rot_mat[1];
    case AFT_BONE_DATABASE_BONE_ROTATION:
    case AFT_BONE_DATABASE_BONE_TYPE_1:
    case AFT_BONE_DATABASE_BONE_POSITION:
    case AFT_BONE_DATABASE_BONE_POSITION_ROTATION:
    default:
        a1->rot_mat_dup[a2] = a1->rot_mat[0];
        break;
    }
}

static void sub_14041B4E0(struc_313* a1) {
    uint32_t* bitfield = a1->bitfield.begin;
    for (motion_bone_index i = MOTION_BONE_FACE_ROOT;
        i <= MOTION_BONE_TL_TOOTH_UPPER_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));
}

static void sub_14041B580(struc_313* a1) {
    uint32_t* bitfield = a1->bitfield.begin;
    for (motion_bone_index i = MOTION_BONE_FACE_ROOT;
        i <= MOTION_BONE_TL_TOOTH_UPPER_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));

    for (motion_bone_index i = MOTION_BONE_N_EYE_L;
        i <= MOTION_BONE_KL_HIGHLIGHT_R_WJ; i++)
        bitfield[i >> 5] |= 1 << (i & 0x1F);
}

static void sub_14041B800(struc_313* a1) {
    uint32_t* bitfield = a1->bitfield.begin;
    for (motion_bone_index i = MOTION_BONE_N_KUTI_D;
        i <= MOTION_BONE_TL_KUTI_U_R_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));

    for (motion_bone_index i = MOTION_BONE_N_AGO;
        i <= MOTION_BONE_TL_TOOTH_UNDER_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));

    for (motion_bone_index i = MOTION_BONE_N_TOOTH_UPPER;
        i <= MOTION_BONE_TL_TOOTH_UPPER_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));
}

static void sub_14041B440(struc_313* a1) {
    uint32_t* bitfield = a1->bitfield.begin;
    for (motion_bone_index i = MOTION_BONE_N_EYE_L;
        i <= MOTION_BONE_KL_HIGHLIGHT_R_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));
}

static void sub_14041B1C0(struc_313* a1) {
    uint32_t* bitfield = a1->bitfield.begin;
    for (motion_bone_index i = MOTION_BONE_N_MABU_L_D_A;
        i <= MOTION_BONE_TL_MABU_R_U_C_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));

    for (motion_bone_index i = MOTION_BONE_N_EYELID_L_A;
        i <= MOTION_BONE_TL_EYELID_R_B_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));
}

static void sub_14041B6B0(struc_313* a1) {
    uint32_t* bitfield = a1->bitfield.begin;
    for (motion_bone_index i = MOTION_BONE_N_HITO_L_EX;
        i <= MOTION_BONE_NL_OYA_C_L_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));
}

static void sub_14041B750(struc_313* a1) {
    uint32_t* bitfield = a1->bitfield.begin;
    for (motion_bone_index i = MOTION_BONE_N_HITO_R_EX;
        i <= MOTION_BONE_NL_OYA_C_R_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));
}

static void sub_14041B300(struc_313* a1) {
    uint32_t* bitfield = a1->bitfield.begin;
    for (motion_bone_index i = MOTION_BONE_N_MABU_L_D_A;
        i <= MOTION_BONE_TL_MABU_R_U_C_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));

    for (motion_bone_index i = MOTION_BONE_N_EYELID_L_A;
        i <= MOTION_BONE_TL_EYELID_R_B_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));
}

static void sub_140413EA0(struc_240* a1, void(*a2)(struc_313*)) {
    a2(&a1->field_8);
}

static void sub_14041AD90(rob_chara_data_bone_data* a1) {
    if (a1->mouth.field_38.key_sets_ready && a1->mouth.field_38.mot_data && !a1->mouth.field_30)
        sub_140413EA0(&a1->face.field_0, sub_14041B800);
    if (a1->eyelid.field_38.key_sets_ready && a1->eyelid.field_38.mot_data && !a1->eyelid.field_30)
        sub_140413EA0(&a1->face.field_0, sub_14041B1C0);
    else if (a1->eye.field_38.key_sets_ready && a1->eye.field_38.mot_data && !a1->eye.field_30 || a1->field_758)
        sub_140413EA0(&a1->face.field_0, sub_14041B440);
}

static void sub_14041AD50(rob_chara_data_bone_data* a1) {
    if (a1->eye.field_38.key_sets_ready && a1->eye.field_38.mot_data && !a1->eye.field_30 || a1->field_758)
        sub_140413EA0(&a1->eyelid.field_0, sub_14041B440);
}

static void sub_140412F20(mot_blend* a1, vector_bone_data* a2) {
    uint32_t* bitfield = a1->field_0.field_8.bitfield.begin;
    for (bone_data* i = a2->begin; i != a2->end; ++i) {
        motion_bone_index v5 = i->motion_bone_index;
        if (bitfield[v5 >> 5] & (1 << (v5 & 0x1F)))
            sub_1401EB1D0(i, 0);
    }
}

static mot_data* mot_parent_load_file(mot_parent* a1, int32_t motion_id) {
    mot_data* (*sub_140403C90)(int32_t a1) = (void*)0x0000000140403C90;
    bool(*mot_load_file)(mot * a1, mot_key_set_file * a2) = (void*)0x000000014036F7B0;

    mot_data* v4;
    if (a1->motion_id == motion_id)
        v4 = a1->mot_data;
    else {
        v4 = sub_140403C90(motion_id);
        a1->mot_data = v4;
        if (v4) {
            a1->motion_id = motion_id;
            a1->skeleton_select = mot_load_file(&a1->mot, v4->data_file);
        }
        else {
            a1->skeleton_select = 0;
            a1->motion_id = -1;
        }
    }
    a1->frame = -1.0f;
    a1->field_68.field_0 = 0;
    a1->field_68.field_4 = 0.0f;
    return v4;
}

static void sub_140414ED0(mot_blend* a1, int32_t motion_id) {
    mot_parent_load_file(&a1->field_38, motion_id);
    a1->field_F8.__vftable->field_8(&a1->field_F8);
}

static void sub_14041BE50(rob_chara_data_bone_data* rob_bone_data, int32_t motion_id) {
    sub_14041AD90(rob_bone_data);
    mot_blend* v3 = &rob_bone_data->face;
    sub_140412F20(v3, &rob_bone_data->motion_loaded.head->next->value->bone_data.bones);
    sub_1404146F0(&v3->field_0);
    sub_140414ED0(v3, motion_id);
}

static void sub_14041ABA0(rob_chara_data_bone_data* a1) {
    list_ptr_motion_blend_mot_node* v1 = a1->motion_loaded.head;
    list_ptr_motion_blend_mot_node* v3 = v1->next;
    while (v3 != v1) {
        if (a1->face.field_38.key_sets_ready && a1->face.field_38.mot_data && !a1->face.field_30) {
            if (!a1->field_758 || a1->eye.field_38.key_sets_ready && a1->eye.field_38.mot_data && !a1->eye.field_30)
                sub_140413EA0(&v3->value->field_0, sub_14041B4E0);
            else
                sub_140413EA0(&v3->value->field_0, sub_14041B580);
        }
        else {
            if (a1->mouth.field_38.key_sets_ready && a1->mouth.field_38.mot_data && !a1->mouth.field_30)
                sub_140413EA0(&v3->value->field_0, sub_14041B800);
            if (a1->eyelid.field_38.key_sets_ready && a1->eyelid.field_38.mot_data && !a1->eyelid.field_30) {
                if (!a1->field_758
                    || a1->eye.field_38.key_sets_ready && a1->eye.field_38.mot_data && !a1->eye.field_30)
                    sub_140413EA0(&v3->value->field_0, sub_14041B1C0);
                else
                    sub_140413EA0(&v3->value->field_0, sub_14041B300);
            }
            else if (a1->eye.field_38.key_sets_ready && a1->eye.field_38.mot_data && !a1->eye.field_30)
                sub_140413EA0(&v3->value->field_0, sub_14041B440);
        }

        if (a1->hand_l.field_38.key_sets_ready && a1->hand_l.field_38.mot_data && !a1->hand_l.field_30)
            sub_140413EA0(&v3->value->field_0, sub_14041B6B0);
        if (a1->hand_r.field_38.key_sets_ready && a1->hand_r.field_38.mot_data && !a1->hand_r.field_30)
            sub_140413EA0(&v3->value->field_0, sub_14041B750);
        v3 = v3->next;
    }
    sub_14041AD90(a1);
    sub_14041AD50(a1);
}

static bool sub_140413630(struc_308* a1) {
    return (a1->field_4 & 2) != 0;
}

static bool sub_140413790(struc_308* a1) {
    return (a1->field_0 & 2) != 0;
}

static bool sub_1404137A0(struc_308* a1) {
    return (a1->field_0 & 0x10) != 0;
}

static void sub_140415A10(motion_blend_mot* a1) {
    motion_blend* v1 = a1->field_5D0;
    if (!v1)
        return;

    struc_400 v7;
    v7.field_0 = 0;
    v7.field_2 = 0;
    v7.field_3 = 0;
    v7.field_4 = 0;
    if (!a1->field_4F8.field_BD)
        v7.field_0 = !sub_140413630(&a1->field_4F8);
    if (!a1->field_4F8.field_BC)
        v7.field_2 = !sub_140413790(&a1->field_4F8);

    v7.field_1 = sub_1404137A0(&a1->field_4F8) ? 1 : 0;
    v7.field_8 = a1->mot_data.frame;
    v7.field_C = a1->field_4F8.eyes_adjust;
    v7.field_10 = a1->field_4F8.field_B8;
    v1->__vftable->field_18(v1, &v7.field_0);
}

static bool sub_140410A20(motion_blend_mot* a1) {
    motion_blend* v1 = a1->field_5D0;
    return !v1 || !v1->field_9;
}

static void sub_140415B30(mot_blend* a1) {
    struc_400 v3; // [rsp+20h] [rbp-28h] BYREF

    motion_blend* v1 = &a1->field_F8;
    v3.field_0 = 0;
    v3.field_1;
    v3.field_2;
    v3.field_3;
    v3.field_4 = 0;
    v3.field_8 = 0.0f;
    v3.field_C = 0.0f;
    v3.field_10 = 0.0f;
    a1->field_F8.__vftable->field_18(&a1->field_F8, &v3.field_0);
}

static void sub_14041DAC0(rob_chara_data_bone_data* a1) {
    void(*rob_bone_data_motion_blend_mot_list_free)(rob_chara_data_bone_data * rob_bone_data,
        size_t a2) = (void*)0x0000000140418E80;

    size_t v2 = 0;
    bool v3 = 0;
    list_ptr_motion_blend_mot_node* v1 = a1->motion_loaded.head;
    list_ptr_motion_blend_mot_node* v4 = v1->next;
    while (v4 != v1) {
        if (sub_1404136B0(v4->value))
            sub_140415A10(v4->value);
        else {
            if (v3) {
                rob_bone_data_motion_blend_mot_list_free(a1, v2);
                break;
            }
            sub_140415A10(v4->value);
            v3 = sub_140410A20(v4->value);
        }
        v4 = v4->next;
        v2++;
    }

    sub_140415B30(&a1->face);
    sub_140415B30(&a1->hand_l);
    sub_140415B30(&a1->hand_r);
    sub_140415B30(&a1->mouth);
    sub_140415B30(&a1->eye);
    sub_140415B30(&a1->eyelid);
}

void rob_chara_data_bone_data_interpolate(rob_chara_data_bone_data* rob_bone_data) {
    if (!rob_bone_data->motion_loaded.size)
        return;

    sub_14041ABA0(rob_bone_data);
    sub_14041DAC0(rob_bone_data);

    list_ptr_motion_blend_mot_node* v2 = rob_bone_data->motion_loaded.head;
    list_ptr_motion_blend_mot_node* v3 = v2->next;
    while (v3 != v2) {
        motion_blend_mot_interpolate(v3->value);
        v3 = v3->next;
    }

    aft_bone_database_skeleton_type skeleton_type = rob_bone_data->base_skeleton_type;
    motion_blend_mot* v5 = rob_bone_data->motion_loaded.head->next->value;
    vector_bone_data* bones = &v5->bone_data.bones;
    vector_uint16_t* bone_indices = &v5->bone_data.bone_indices;
    mot_blend_interpolate(&rob_bone_data->face, bones, bone_indices, skeleton_type);
    mot_blend_interpolate(&rob_bone_data->hand_l, bones, bone_indices, skeleton_type);
    mot_blend_interpolate(&rob_bone_data->hand_r, bones, bone_indices, skeleton_type);
    mot_blend_interpolate(&rob_bone_data->mouth, bones, bone_indices, skeleton_type);
    mot_blend_interpolate(&rob_bone_data->eye, bones, bone_indices, skeleton_type);
    mot_blend_interpolate(&rob_bone_data->eyelid, bones, bone_indices, skeleton_type);
}

static void sub_140412E10(motion_blend_mot* a1, int32_t a2) {
    for (bone_data* i = a1->bone_data.bones.begin; i != a1->bone_data.bones.end; i++) {
        motion_bone_index v5 = i->motion_bone_index;
        if (!(a1->field_0.field_8.bitfield.begin[v5 >> 5] & (1 << (v5 & 0x1F))))
            continue;

        sub_1401EB1D0(i, a2);
        if (i->type == AFT_BONE_DATABASE_BONE_POSITION_ROTATION && a1->field_4F8.field_0 & 2)
            vec3_add(i->trans_dup[a2], a1->field_4F8.field_90, i->trans_dup[a2]);
    }
}

static void sub_1401EAD00(bone_data* a1, bone_data* a2) {
    if (!a1->flags)
        return;

    if (a1->type >= AFT_BONE_DATABASE_BONE_TYPE_1 && a1->type <= AFT_BONE_DATABASE_BONE_POSITION_ROTATION) {
        a1->trans = a2->trans;
        a1->trans_dup[0] = a2->trans_dup[0];
    }

    switch (a1->type) {
    case AFT_BONE_DATABASE_BONE_ARM_IK_ROTATION:
    case AFT_BONE_DATABASE_BONE_LEGS_IK_ROTATION:
        a1->rot_mat[2] = a1->rot_mat[2];
        a1->rot_mat_dup[4] = a1->rot_mat_dup[4];
    case AFT_BONE_DATABASE_BONE_HEAD_IK_ROTATION:
        a1->rot_mat[1] = a1->rot_mat[1];
        a1->rot_mat_dup[2] = a1->rot_mat_dup[2];
    case AFT_BONE_DATABASE_BONE_ROTATION:
    case AFT_BONE_DATABASE_BONE_TYPE_1:
    case AFT_BONE_DATABASE_BONE_POSITION:
    case AFT_BONE_DATABASE_BONE_POSITION_ROTATION:
    default:
        a1->rot_mat[0] = a1->rot_mat[0];
        a1->rot_mat_dup[0] = a1->rot_mat_dup[0];
        break;
    }
}

static void sub_140412BB0(motion_blend_mot* a1, vector_bone_data* a2) {
    for (bone_data* i = a1->bone_data.bones.begin; i != a1->bone_data.bones.end; i++)
        sub_1401EAD00(i, &a2->begin[i->motion_bone_index]);
}

static void sub_1403FBF10(motion_blend* a1, float_t a2) {
    a1->blend = clamp(a2, 0.0f, 1.0f);
    a1->field_8 = 1;
    a1->field_9 = 1;
}

static void sub_1404148C0(motion_blend_mot* a1, float_t a2) {
    motion_blend* v2 = a1->field_5D0;
    if (v2)
        sub_1403FBF10(v2, a2);
}

static void sub_1404147F0(motion_blend_mot* a1, motion_blend_type type, float_t blend) {
    if (type == MOTION_BLEND_FREEZE) {
        a1->freeze.base.__vftable->field_8(&a1->freeze.base);
        a1->field_5D0 = &a1->freeze.base;
        sub_140412E10(a1, 0);
    }
    else if (type == MOTION_BLEND_CROSS) {
        a1->cross.base.__vftable->field_8(&a1->cross.base);
        a1->field_5D0 = &a1->cross.base;
    }
    else if (type == MOTION_BLEND_COMBINE) {
        a1->combine.base.base.__vftable->field_8(&a1->combine.base.base);
        a1->field_5D0 = &a1->combine.base.base;
        sub_1404148C0(a1, blend);
    }
    else
        a1->field_5D0 = 0;
}

static int32_t sub_1401F0E70(aft_bone_database_skeleton_type type, char* name) {
    aft_bone_database_struct* bone_database = *(void**)0x0000000140FBC1C0;
    int32_t(*sub_1401F0EA0)(char* a1, char* a2) = (void*)0x00000001401F0EA0;

    if (!bone_database || type == AFT_BONE_DATABASE_SKELETON_NONE)
        return -1;
    else
        return sub_1401F0EA0(bone_database->skeleton_names[type], name);
}

static void bone_data_parent_load_bone_indices_from_mot(bone_data_parent* a1, mot_data* a2) {
    if (!a2)
        return;

    char** (*sub_140403B00)() = (void*)0x0000000140403B00;
    void (*sub_14040F210)(vector_uint16_t * a1, size_t a2) = (void*)0x000000014040F210;

    char** v4 = sub_140403B00();
    vector_uint16_t* v5 = &a1->bone_indices;
    uint16_t v6 = a2->data_file->info;
    a1->bone_indices.end = a1->bone_indices.begin;
    aft_bone_database_skeleton_type v8 = a1->rob_bone_data->base_skeleton_type;
    uint16_t v9 = (v6 & 0x3FFF) - 1;
    if (!v9)
        return;

    for (size_t v10 = 0, v11 = 0; v10 < v9; v11++) {
        int32_t v12 = sub_1401F0E70(v8, v4[a2->bone_info[v11]]);
        if (v12 == -1) {
            v12 = sub_1401F0E70(v8, v4[a2->bone_info[++v11]]);
            if (v12 == -1)
                break;
        }

        uint16_t* v14 = v5->end;
        bone_data* v15 = &a1->bones.begin[v12];
        uint16_t v21 = (uint16_t)v12;
        if (&v21 >= v14 || v5->begin > &v21) {
            if (v14 == v5->capacity_end)
                sub_14040F210(v5, 1);

            uint16_t* v18 = v5->end;
            if (v18)
                *v18 = v21;
        }
        else {
            size_t v16 = &v21 - v5->begin;
            if (v14 == v5->capacity_end)
                sub_14040F210(v5, 1);

            uint16_t* v17 = v5->end;
            if (v17)
                *v17 = v5->begin[v16];
        }
        v5->end++;

        v15->key_set_offset = (int32_t)v10;
        v15->frame = -1.0f;
        if (v15->type >= AFT_BONE_DATABASE_BONE_POSITION_ROTATION)
            v10 += 6;
        else
            v10 += 3;
    }
}

static void sub_140413C30(struc_308* a1) {
    a1->field_8C = false;
    a1->field_4C = a1->mat;
    a1->mat = mat4u_identity;
}

static void motion_blend_mot_load_file(motion_blend_mot* a1, int32_t motion_id, int32_t a3, float_t blend) {
    sub_1404147F0(a1, a3, blend);
    mot_data* v5 = mot_parent_load_file(&a1->mot_data, motion_id);
    bone_data_parent* v6 = &a1->bone_data;
    if (v5) {
        bone_data_parent_load_bone_indices_from_mot(v6, v5);
        uint16_t* v7 = a1->bone_data.bone_indices.begin;
        uint16_t* v8 = a1->bone_data.bone_indices.end;
        bone_data* v9 = a1->bone_data.bones.begin;
        for (; v7 != v8; v7++)
            v9[*v7].frame = -1.0f;
    }
    else {
        rob_chara_data_bone_data* rob_bone_data = v6->rob_bone_data;
        for (mat4* i = rob_bone_data->mats.begin; i != rob_bone_data->mats.end; i++)
            *i = mat4_identity;

        for (mat4* i = rob_bone_data->mats2.begin; i != rob_bone_data->mats2.end; i++)
            *i = mat4_identity;

        a1->bone_data.bone_indices.end = a1->bone_data.bone_indices.begin;
    }
    sub_140413C30(&a1->field_4F8);
}

static void sub_14041BA60(rob_chara_data_bone_data* rob_bone_data) {
    motion_blend_mot* v1 = rob_bone_data->motion_loaded.head->next->value;
    if (v1)
        sub_1404146F0(&v1->field_0);
}

static void sub_14041AE40(rob_chara_data_bone_data* a1) {
    list_ptr_motion_blend_mot_node* v1 = a1->motion_loaded.head;
    struc_240* v3 = &v1->next->value->field_0;
    if (!v3)
        return;

    if (a1->face.field_38.key_sets_ready && a1->face.field_38.mot_data && !a1->face.field_30) {
        if (!a1->field_758 || a1->eye.field_38.key_sets_ready && a1->eye.field_38.mot_data && !a1->eye.field_30)
            sub_140413EA0(v3, sub_14041B4E0);
        else
            sub_140413EA0(v3, sub_14041B580);
    }
    else {
        if (a1->mouth.field_38.key_sets_ready && a1->mouth.field_38.mot_data && !a1->mouth.field_30)
            sub_140413EA0(&v1->next->value->field_0, sub_14041B800);
        else if (a1->eyelid.field_38.key_sets_ready && a1->eyelid.field_38.mot_data && !a1->eyelid.field_30) {
            if (!a1->field_758 || a1->eye.field_38.key_sets_ready && a1->eye.field_38.mot_data && !a1->eye.field_30)
                sub_140413EA0(v3, sub_14041B1C0);
            else
                sub_140413EA0(v3, sub_14041B300);
        }
        else if (a1->eye.field_38.key_sets_ready && a1->eye.field_38.mot_data && !a1->eye.field_30)
            sub_140413EA0(v3, sub_14041B440);
    }

    if (a1->hand_l.field_38.key_sets_ready && a1->hand_l.field_38.mot_data && !a1->hand_l.field_30)
        sub_140413EA0(v3, sub_14041B6B0);
    if (a1->hand_r.field_38.key_sets_ready && a1->hand_r.field_38.mot_data && !a1->hand_r.field_30)
        sub_140413EA0(v3, sub_14041B750);
}

void rob_chara_data_bone_data_motion_load(rob_chara_data_bone_data* rob_bone_data, int32_t motion_id, int32_t index) {
    void(*rob_bone_data_motion_blend_mot_list_init)(rob_chara_data_bone_data * rob_bone_data)
        = (void*)0x000000014041A9E0;
    void(*rob_bone_data_motion_blend_mot_list_free)(rob_chara_data_bone_data * rob_bone_data,
        size_t a2) = (void*)0x0000000140418E80;
    list_ptr_motion_blend_mot_node* (*sub_140415D30)(list_ptr_motion_blend_mot * a1,
        list_ptr_motion_blend_mot_node * a2, list_ptr_motion_blend_mot_node * a3,
        motion_blend_mot * *a4) = (void*)0x0000000140415D30;
    list_size_t_node* (*sub_140415DB0)(list_size_t_node * *a1, list_size_t_node * a2,
        list_size_t_node * a3, size_t * a4) = (void*)0x0000000140415DB0;
    motion_blend_mot* (*sub_140411A70)(motion_blend_mot * a1,
        rob_chara_data_bone_data * rob_bone_data) = (void*)0x0000000140411A70;

    if (!rob_bone_data->motion_loaded.size)
        return;
    if (index == 1) {
        rob_bone_data_motion_blend_mot_list_free(rob_bone_data, 1);
        sub_14041AE40(rob_bone_data);
        list_ptr_motion_blend_mot_node*v15 = rob_bone_data->motion_loaded.head;
        motion_blend_mot_load_file(v15->next->value, motion_id, 1, 1.0f);
        sub_14041BA60(rob_bone_data);
        return;
    }

    if (index != 2) {
        if (index == 3)
            index = 0;
        rob_bone_data_motion_blend_mot_list_free(rob_bone_data, 1);
        motion_blend_mot_load_file(rob_bone_data->motion_loaded.head->next->value, motion_id, index, 1.0f);
        return;
    }

    rob_bone_data_motion_blend_mot_list_free(rob_bone_data, 2);
    if (rob_bone_data->motion_indices.size) {
        motion_blend_mot* v6 = rob_bone_data->motion_loaded.head->next->value;
        rob_bone_data_motion_blend_mot_list_init(rob_bone_data);
        sub_140412BB0(rob_bone_data->motion_loaded.head->next->value, &v6->bone_data.bones);
        motion_blend_mot_load_file(rob_bone_data->motion_loaded.head->next->value, motion_id, 2, 1.0f);
        return;
    }

    list_ptr_motion_blend_mot* v7 = &rob_bone_data->motion_loaded;
    motion_blend_mot* v16 = sub_140411A70(rob_bone_data->motion_loaded.head->next->value, rob_bone_data);
    if (!v16) {
        rob_bone_data_motion_blend_mot_list_free(rob_bone_data, 1);
        sub_14041AE40(rob_bone_data);
        list_ptr_motion_blend_mot_node* v15 = v7->head;
        motion_blend_mot_load_file(v15->next->value, motion_id, 1, 1.0f);
        sub_14041BA60(rob_bone_data);
        return;
    }

    list_ptr_motion_blend_mot_node* v8 = v7->head->next;
    motion_blend_mot* v9 = v8->value;
    list_ptr_motion_blend_mot_node* v10 = sub_140415D30(&rob_bone_data->motion_loaded, v8, v8->prev, &v16);
    rob_bone_data->motion_loaded.size++;
    v8->prev = v10;
    v10->prev->next = v10;
    size_t v17 = rob_bone_data->motions.end - rob_bone_data->motions.begin;
    list_size_t_node* v12 = rob_bone_data->motion_loaded_indices.head->next;
    list_size_t_node* v13 = sub_140415DB0(&rob_bone_data->motion_loaded_indices.head, v12, v12->prev, &v17);
    rob_bone_data->motion_loaded_indices.size++;
    v12->prev = v13;
    v13->prev->next = v13;
    sub_140412BB0(v7->head->next->value, &v9->bone_data.bones);
    motion_blend_mot_load_file(v7->head->next->value, motion_id, 2, 1.0f);
}

static void sub_14041BFC0(rob_chara_data_bone_data* rob_bone_data, int32_t motion_id) {
    sub_140412F20(&rob_bone_data->hand_l, &rob_bone_data->motion_loaded.head->next->value->bone_data.bones);
    sub_140414ED0(&rob_bone_data->hand_l, motion_id);
}

static void sub_14041C260(rob_chara_data_bone_data* rob_bone_data, int32_t motion_id) {
    sub_140412F20(&rob_bone_data->hand_r, &rob_bone_data->motion_loaded.head->next->value->bone_data.bones);
    sub_140414ED0(&rob_bone_data->hand_r, motion_id);
}

static void sub_14041D200(rob_chara_data_bone_data* rob_bone_data, int32_t motion_id) {
    sub_140412F20(&rob_bone_data->mouth, &rob_bone_data->motion_loaded.head->next->value->bone_data.bones);
    sub_140414ED0(&rob_bone_data->mouth, motion_id);
}

static void sub_14041BDB0(rob_chara_data_bone_data* rob_bone_data, int32_t motion_id) {
    sub_140412F20(&rob_bone_data->eye, &rob_bone_data->motion_loaded.head->next->value->bone_data.bones);
    sub_140414ED0(&rob_bone_data->eye, motion_id);
}

static void sub_14041BD20(rob_chara_data_bone_data* rob_bone_data, int32_t motion_id) {
    sub_14041AD50(rob_bone_data);
    mot_blend* v3 = &rob_bone_data->eyelid;
    sub_140412F20(v3, &rob_bone_data->motion_loaded.head->next->value->bone_data.bones);
    sub_1404146F0(&v3->field_0);
    sub_140414ED0(v3, motion_id);
}

static void sub_140414BC0(struc_346* a1, float_t frame) {
    float_t v2 = a1->field_14;
    a1->frame = frame;
    a1->field_28 = 0;
    if (v2 >= 0.0f && (a1->field_18 == 2 && frame <= v2) || frame >= v2)
        a1->field_14 = -1.0;
}

static void rob_chara_data_bone_data_set_frame(rob_chara_data_bone_data* a1, float_t frame) {
    sub_140414BC0(&a1->motion_loaded.head->next->value->field_4A8.field_0, frame);
}

static void sub_14041C680(rob_chara_data_bone_data* a1, char a2) {
    struc_308* v2 = &a1->motion_loaded.head->next->value->field_4F8;
    if (a2)
        v2->field_0 |= 2;
    else
        v2->field_0 &= ~2;
}

static void sub_14041C9D0(rob_chara_data_bone_data* a1, char a2) {
    struc_308* v2 = &a1->motion_loaded.head->next->value->field_4F8;
    v2->field_8 = (uint8_t)(v2->field_0 & 1);
    if (a2)
        v2->field_0 |= 1;
    else
        v2->field_0 &= ~1;
}

static void sub_14041D2D0(rob_chara_data_bone_data* a1, char a2) {
    struc_308* v2 = &a1->motion_loaded.head->next->value->field_4F8;
    if (a2)
        v2->field_0 |= 4;
    else
        v2->field_0 &= ~4;
}

static void sub_14041BC40(rob_chara_data_bone_data* a1, char a2) {
    struc_308* v2 = &a1->motion_loaded.head->next->value->field_4F8;
    if (a2)
        v2->field_0 |= 8;
    else
        v2->field_0 &= ~8;
}

static void sub_140414F00(struc_308* a1, float_t a2) {
    a1->field_B8 = a1->eyes_adjust;
    a1->eyes_adjust = a2;
}

static void sub_14041D270(rob_chara_data_bone_data* a1, float_t a2) {
    sub_140414F00(&a1->motion_loaded.head->next->value->field_4F8, a2);
}

static void sub_14041D2A0(rob_chara_data_bone_data* a1, float_t a2) {
    a1->motion_loaded.head->next->value->field_4F8.field_B8 = a2;
}

static void rob_chara_data_bone_data_set_rotation_y(rob_chara_data_bone_data* rob_bone_data, float_t rotation_y) {
    rob_bone_data->motion_loaded.head->next->value->bone_data.rotation_y = rotation_y;
}

static void sub_1403FBED0(motion_blend* a1, float_t a2, float_t a3, float_t a4) {
    if (a2 < 0.0f) {
        a1->field_8 = false;
        a1->field_C = 0.0f;
        a1->field_10 = 0.0f;
        a1->field_14 = a3;
        a1->field_18 = a4;
    }
    else {
        a1->field_8 = true;
        a1->field_C = a2;
        a1->field_10 = 0.0f;
        a1->field_14 = a3;
        a1->field_18 = a4;
    }
}

static void sub_140414C60(motion_blend_mot* a1, float_t a2, float_t a3, float_t a4) {
    motion_blend* v4 = a1->field_5D0;
    if (v4)
        sub_1403FBED0(v4, a2, a3, a4);
}

static void sub_14041BF80(rob_chara_data_bone_data* a1, float_t a2, float_t a3, float_t a4) {
    sub_140414C60(a1->motion_loaded.head->next->value, a2, a3, a4);
}

static void sub_14041D310(rob_chara_data_bone_data* a1) {
    motion_blend* v1 = a1->motion_loaded.head->next->value->field_5D0;
    if (v1)
        v1->__vftable->field_10(v1);
}

static void rob_chara_data_load_default_motion_sub(rob_chara_data* a1, int32_t a2, int32_t motion_id) {
    sub_14041BE50(a1->bone_data, -1);
    sub_14041BFC0(a1->bone_data, -1);
    sub_14041C260(a1->bone_data, -1);
    sub_14041D200(a1->bone_data, -1);
    sub_14041BDB0(a1->bone_data, -1);
    sub_14041BD20(a1->bone_data, -1);
    rob_chara_data_bone_data_motion_load(a1->bone_data, motion_id, 1);
    rob_chara_data_bone_data_set_frame(a1->bone_data, 0.0f);
    sub_14041C680(a1->bone_data, 0);
    sub_14041C9D0(a1->bone_data, 0);
    sub_14041D2D0(a1->bone_data, 0);
    sub_14041BC40(a1->bone_data, 0);
    sub_14041D270(a1->bone_data, 0.0f);
    sub_14041D2A0(a1->bone_data, 0.0f);
    rob_chara_data_bone_data_set_rotation_y(a1->bone_data, 0.0f);
    sub_14041BF80(a1->bone_data, 0.0f, 1.0f, 1.0f);
    sub_14041D310(a1->bone_data);
    rob_chara_data_bone_data_interpolate(a1->bone_data);
    rob_chara_data_bone_data_update(a1->bone_data, 0);
    sub_140412E10(a1->bone_data->motion_loaded.head->next->value, a2);
}

int32_t rob_cmn_mottbl_get_motion_index(rob_chara_data* a1, int32_t a2) {
    struc_403* rob_cmn_mottbl_data = *rob_cmn_mottbl_data_ptr;

    if (a2 == 224)
        return a1->field_440.field_290.field_0;
    if (a2 >= 214 && a2 <= 223)
        return ((uint32_t*)&a1->field_4228.field_1E68.field_1C88)[a2];
    if (a2 >= 226 && a2 <= 235)
        return ((uint32_t*)&a1->field_4228.field_1E68.field_1C78)[a2 + 1];

    size_t v7 = a1->field_440.field_8.field_1A4;
    size_t v8 = a1->chara_index;
    if (!rob_cmn_mottbl_data
        || v8 < 0 || v8 >= rob_cmn_mottbl_data->field_0
        || a2 < 0 || a2 >= rob_cmn_mottbl_data->field_4)
        return -1;

    struc_404* v4 = (struc_404*)((size_t)rob_cmn_mottbl_data + rob_cmn_mottbl_data->field_8);
    if (v7 < v4[v8].field_4)
        return ((int32_t*)((size_t)rob_cmn_mottbl_data
            + ((uint32_t*)((size_t)rob_cmn_mottbl_data + v4[v8].field_0))[v7]))[a2];
    return -1;
}

static void rob_chara_data_load_default_motion(rob_chara_data* a1) {
    rob_chara_data_load_default_motion_sub(a1, 1, rob_cmn_mottbl_get_motion_index(a1, 0));
}

static bone_node* rob_chara_data_bone_data_get_node(rob_chara_data_bone_data* rob_bone_data, size_t index) {
    if ((ssize_t)index < rob_bone_data->nodes.end - rob_bone_data->nodes.begin)
        return &rob_bone_data->nodes.begin[index];
    return 0;
}

static char* ex_node_block_get_parent_name(ex_node_block* a1) {
    return string_data(&a1->parent_name);
}

static bone_node* rob_chara_data_item_equip_object_get_bone_node(
    rob_chara_data_item_equip_object* a1, int32_t a2) {
    bool ex_data = a2 & 0x8000 ? true : false;
    a2 &= 0x7FFF;
    if (!ex_data)
        return &a1->bone_nodes[a2];
    else if (a2 < a1->ex_data_bone_nodes.end - a1->ex_data_bone_nodes.begin)
        return &a1->ex_data_bone_nodes.begin[a2];
    return 0;
}

static bone_node* rob_chara_data_item_equip_object_get_bone_node_by_name(
    rob_chara_data_item_equip_object* a1, char* a2) {
    int32_t(*rob_chara_data_item_equip_object_get_bone_index)(
        rob_chara_data_item_equip_object * a1, char* a2) = (void*)0x00000001405F3400;
    return rob_chara_data_item_equip_object_get_bone_node(a1,
        rob_chara_data_item_equip_object_get_bone_index(a1, a2));
}

static void rob_chara_data_item_equip_object_get_parent_bone_nodes(
    rob_chara_data_item_equip_object* a1, bone_node* a2) {
    a1->bone_nodes = a2;
    a1->mat = a2->mat;
    for (ex_node_block** i = a1->node_blocks.begin; i != a1->node_blocks.end; i++)
        (*i)->parent_bone_node = rob_chara_data_item_equip_object_get_bone_node_by_name(
            a1, ex_node_block_get_parent_name(*i));
}

static void rob_chara_data_item_equip_get_parent_bone_nodes(rob_chara_data_item_equip* a1, bone_node* a2) {
    a1->bone_nodes = a2;
    a1->matrices = a2->mat;
    for (int32_t v2 = a1->first_item_equip_object; v2 < a1->max_item_equip_object; v2++)
        rob_chara_data_item_equip_object_get_parent_bone_nodes(&a1->item_equip_object[v2], a1->bone_nodes);
}

static bool rob_chara_data_check_for_ageageagain_module(chara_index chara_index, int32_t module_index) {
    return chara_index == CHARA_MIKU && module_index == 148;
}

static void sub_140517060(rob_chara_data* a1, bool a2) {
    void(*sub_140543780)(int32_t a1, int32_t a2, char a3) = (void*)0x0000000140543780;

    a1->field_440.field_0 &= ~1;
    a1->field_440.field_3 &= ~1;
    a1->field_440.field_0 |= a2 & 1;
    a1->field_440.field_3 |= a2 & 1;
    if (rob_chara_data_check_for_ageageagain_module(a1->chara_index, a1->module_index)) {
        sub_140543780(a1->chara_id, 1, a2);
        sub_140543780(a1->chara_id, 2, a2);
    }
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

static float_t sub_140510320(uint32_t a1) {
    static const float_t flt_1411ADAD0[] = {
        0.071732f, 0.0f, -0.03859f, 0.0f, 0.0f
    };

    if (a1 > 4)
        return 0.0f;
    else
        return flt_1411ADAD0[a1];
}

static void sub_14041C5C0(rob_chara_data_bone_data* a1, struc_243* a2) {
    a1->field_788.field_15C = *a2;
}

static void sub_14041D4E0(rob_chara_data_bone_data* a1, struc_242* a2) {
    a1->field_958.field_0 = *a2;
}

static void sub_14041D5C0(rob_chara_data_bone_data* a1, struc_242* a2) {
    a1->field_958.field_2C = *a2;
}

static void sub_14041D560(rob_chara_data_bone_data* a1, char a2, char a3) {
    a1->field_958.field_58 = a2;
    a1->field_958.field_59 = a3;
}

static void rob_chara_data_bone_data_ik_scale_calculate(
    rob_chara_data_bone_data_ik_scale* a1, vector_bone_data* a2,
    aft_bone_database_skeleton_type base_skeleton_type,
    aft_bone_database_skeleton_type skeleton_type) {
    bone_data* b_cl_mune = &a2->begin[MOTION_BONE_CL_MUNE];
    bone_data* b_kl_kubi = &a2->begin[MOTION_BONE_KL_KUBI];
    bone_data* b_c_kata_l = &a2->begin[MOTION_BONE_C_KATA_L];
    bone_data* b_cl_momo_l = &a2->begin[MOTION_BONE_CL_MOMO_L];

    float_t base_height = fabsf(b_cl_momo_l->base_translation[0].y) + b_cl_momo_l->ik_segment_length[0]
        + b_cl_momo_l->ik_2nd_segment_length[0] + aft_bone_database_get_skeleton_heel_height(base_skeleton_type);
    float_t height = fabsf(b_cl_momo_l->base_translation[1].y) + b_cl_momo_l->ik_segment_length[1]
        + b_cl_momo_l->ik_2nd_segment_length[1] + aft_bone_database_get_skeleton_heel_height(skeleton_type);
    a1->ratio0 = height / base_height;
    a1->ratio1 = (fabsf(b_kl_kubi->base_translation[1].y) + b_cl_mune->ik_segment_length[1])
        / (fabsf(b_kl_kubi->base_translation[0].y) + b_cl_mune->ik_segment_length[0]);
    a1->ratio2 = (b_c_kata_l->ik_segment_length[1] + b_c_kata_l->ik_2nd_segment_length[1])
        / (b_c_kata_l->ik_segment_length[0] + b_c_kata_l->ik_2nd_segment_length[0]);
    a1->ratio3 = (fabsf(b_kl_kubi->base_translation[1].y) + b_cl_mune->ik_segment_length[1] + height)
        / (fabsf(b_kl_kubi->base_translation[0].y) + b_cl_mune->ik_segment_length[0] + base_height);
}

static void rob_chara_data_bone_data_get_ik_scale(rob_chara_data_bone_data* a1) {
    if (!a1->motion_loaded.size)
        return;

    motion_blend_mot* v2 = a1->motion_loaded.head->next->value;
    rob_chara_data_bone_data_ik_scale_calculate(&a1->ik_scale, &v2->bone_data.bones, a1->base_skeleton_type, a1->skeleton_type);
    float_t ratio0 = a1->ik_scale.ratio0;
    v2->field_4F8.field_C0 = ratio0;
    v2->field_4F8.field_C4 = ratio0;
    v2->field_4F8.field_C8 = vec3_null;
}

static float_t rob_chara_data_bone_data_get_ik_scale_ratio0(rob_chara_data_bone_data* a1) {
    return a1->ik_scale.ratio0;
}

static void sub_1405167A0(rob_chara_data* a1) {
    a1->field_C = 1;
}

static void sub_140513C60(rob_chara_data_item_equip* a1, int32_t a2, bool a3) {
    if (a3)
        a1->field_18[a2] &= ~4;
    else
        a1->field_18[a2] |= 4;
}

static aft_object_info sub_140525C00(struc_288* a1, int32_t a2) {
    tree_pair_int32_t_int32_t_node* v2 = a1->field_3F8.head;
    tree_pair_int32_t_int32_t_node* v3 = v2;
    tree_pair_int32_t_int32_t_node* v4 = v2->parent;
    while (!v4->is_null)
        if (v4->key >= a2) {
            v3 = v4;
            v4 = v4->left;
        }
        else
            v4 = v4->right;

    tree_pair_int32_t_int32_t_node* v6;
    if (v3 == v2 || a2 < v3->key)
        v6 = v2;
    else
        v6 = v3;

    if (v6 == v2)
        return (aft_object_info) { -1, -1 };
    else
        return *(aft_object_info*)&v6->value;
}

static aft_object_info sub_1405104C0(rob_chara_data* a1, int32_t a2) {
    if (a2 > 8)
        return (aft_object_info) { -1, -1 };

    aft_object_info v3 = sub_140525C00(&a1->field_38, a2);
    if (v3.id == -1 && v3.set_id == -1)
        return a1->chara_init_data->head_objects[a2];
    return v3;
}

static void sub_1405139E0(rob_chara_data_item_equip* a1, uint32_t a2, bool draw) {
    a1->item_equip_object[a2].draw = draw;
}

static void sub_140517120(rob_chara_data* a1, uint32_t a2, bool draw) {
    if (a2 < 31)
        sub_1405139E0(a1->item_equip, a2, draw);
    else if (a2 == 31) {
        void(*sub_140543780)(int32_t a1, int32_t a2, char a3) = (void*)0x0000000140543780;
        void(*sub_1405430F0)(int32_t a1, int32_t a2) = (void*)0x00000001405430F0;

        for (int32_t i = 1; i < 31; i++) {
            sub_1405139E0(a1->item_equip, i, draw);
            if (i == 1 && rob_chara_data_check_for_ageageagain_module(
                a1->chara_index, a1->module_index)) {
                sub_140543780(a1->chara_id, 1, draw);
                sub_140543780(a1->chara_id, 2, draw);
                sub_1405430F0(a1->chara_id, 1);
                sub_1405430F0(a1->chara_id, 2);
            }
        }
    }
}

void rob_chara_data_reset(rob_chara_data* a1, rob_chara_pv_data* a2) {
    float_t(*sub_140228BD0)(vec3 * a1, float_t a2) = (void*)0x0000000140228BD0;
    void(*sub_140505EA0)(struc_263 * a1) = (void*)0x0000000140505EA0;
    struc_243* (*sub_140510550)(int32_t a1) = (void*)0x0000000140510550;
    struc_241* (*sub_1405106E0)(int32_t a1) = (void*)0x00000001405106E0;
    void(*sub_1405552A0)(rob_chara_data * a1, int32_t motion_id,
        bool a3, float_t a4, int32_t a5) = (void*)0x00000001405552A0;

    int32_t v2 = a1->field_440.field_8.field_1CC;
    rob_chara_data_bone_data_reset(a1->bone_data);
    rob_chara_data_bone_data_init_data(a1->bone_data,
        AFT_BONE_DATABASE_SKELETON_COMMON, a1->chara_init_data->skeleton_type);
    sub_140505EA0(&a1->field_440);
    sub_140505EA0(&a1->field_4228);
    rob_chara_data_item_equip_get_parent_bone_nodes(a1->item_equip,
        rob_chara_data_bone_data_get_node(a1->bone_data, 0));
    a1->field_4 = a2->index;
    sub_140517060(a1, a2->field_4);
    a1->field_A = a2->field_16;
    a1->field_1C = 1.0f;
    vec3 v8 = a2->field_8;
    if (a2->index != 2) {
        vec3 v19 = a2->field_8;
        v19.y += 1.0f;
        v8.y = sub_140228BD0(&v19, 0.0);
    }

    struc_263* v3 = &a1->field_440;
    v3->field_1D38.field_0 = a2->field_14;
    v3->field_1D38.field_6 = a2->field_14;
    v3->field_1D38.field_18 = v8;
    v3->field_1D38.field_24 = v8;
    v3->field_1D38.ex_data_mat = v8;
    v3->field_1DE4.scale = chara_size_table_get_value(a1->pv_data.field_9C);
    v3->field_1DE4.field_4 = a1->pv_data.field_A0;
    v3->field_1DE4.field_8 = sub_140510320(a1->pv_data.field_9C);
    v3->field_8.field_1A8 = a1->chara_init_data->field_838[v3->field_8.field_1A4];
    sub_14041C5C0(a1->bone_data, sub_140510550(a1->chara_index));
    rob_chara_data_bone_data_eyes_xrot_adjust(a1->bone_data,
        sub_1405106E0(a1->chara_index), &a2->eyes_adjust);
    sub_14041D4E0(a1->bone_data, &a2->field_18);
    sub_14041D5C0(a1->bone_data, &a2->field_44);
    sub_14041D560(a1->bone_data, 1, 0);
    rob_chara_data_bone_data_get_ik_scale(a1->bone_data);
    float_t ratio0 = rob_chara_data_bone_data_get_ik_scale_ratio0(a1->bone_data);
    v3->field_290.field_138 = ratio0;
    v3->field_290.field_13C = ratio0;
    v3->field_290.field_140 = 0;
    v3->field_290.field_144 = 0;
    v3->field_290.field_148 = 0;
    rob_chara_data_load_default_motion(a1);
    v3->field_8.field_4 = 1;
    v3->field_8.field_10 = rob_cmn_mottbl_get_motion_index(a1, 0);
    v3->field_8.field_0 = 0;
    v3->field_8.field_14 = a2->field_5;
    sub_1405552A0(a1, v3->field_8.field_10, v3->field_8.field_14, 0.0, 0);
    if (!a2->field_6)
        v3->field_8.field_1CC = v2;
    sub_1405167A0(a1);
    a1->item_equip->field_DC = 0;
    sub_140513C60(a1->item_equip, 1, false);
    sub_140513C60(a1->item_equip, 10, false);
    sub_140513C60(a1->item_equip, 11, false);
    aft_object_info v17 = sub_1405104C0(a1, 0);
    v3->field_290.field_300 = v17;
    v3->field_290.field_560 = v17;
    sub_140517120(a1, 0x1F, true);
    a1->field_20 = 0;
}
