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

ex_node_block_vtbl* ex_osage_block_vftable = (ex_node_block_vtbl*)0x0000000140A82F88;
ex_node_block_vtbl* ex_node_block_vftable = (ex_node_block_vtbl*)0x0000000140A82DC8;

static const aft_object_info aft_object_info_null = { (uint16_t)-1, (uint16_t)-1 };

static aft_bone_database_struct** bone_database_ptr = (aft_bone_database_struct**)0x00000000140FBC1C0;
static struc_403** rob_cmn_mottbl_data_ptr = (struc_403**)0x00000001411E8258;
static void* (*HeapCMallocAllocate)(HeapCMallocType type, size_t size, const char* name)
    = (void* (*)(HeapCMallocType, size_t, const char*))0x00000001403F2A00;
static void(*HeapCMallocFree)(HeapCMallocType type, void* data)
    = (void(*)(HeapCMallocType, void*))0x00000001403F2960;
static void(*operator_new)(void*) = (void(*)(void*))0x0000000140845378;
static void(*operator_delete)(void*) = (void(*)(void*))0x0000000140845378;

static const float_t get_osage_gravity_const() {
    return 0.00299444468691945f;
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

static void bone_node_expression_data_reset_scale(bone_node_expression_data* data) {
    data->scale = vec3_identity;
    data->parent_scale = vec3_identity;
}

static void bone_node_data_reset_position_rotation(bone_node_expression_data* data) {
    data->position = vec3_null;
    data->rotation = vec3_null;
    data->scale = vec3_identity;
    data->parent_scale = vec3_identity;
}

static bone_data* bone_data_init(bone_data* bone) {
    bone->type = (aft_bone_database_bone_type)0;
    bone->has_parent = 0;
    bone->motion_bone_index = (motion_bone_index)0;
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

static void rob_chara_bone_data_reserve(rob_chara_bone_data* rob_bone_data) {
    if (rob_bone_data->mats.begin)
        HeapCMallocFree(HeapCMallocSystem, rob_bone_data->mats.begin);

    size_t mats_size = rob_bone_data->object_bone_count;
    rob_bone_data->mats.begin = (mat4*)HeapCMallocAllocate(HeapCMallocSystem,
        mats_size * sizeof(mat4), "ALLOCsys");
    rob_bone_data->mats.end = rob_bone_data->mats.begin + mats_size;
    rob_bone_data->mats.capacity_end = rob_bone_data->mats.begin + mats_size;

    for (mat4* i = rob_bone_data->mats.begin; i != rob_bone_data->mats.end; i++)
        *i = mat4_identity;

    if (rob_bone_data->mats2.begin)
        HeapCMallocFree(HeapCMallocSystem, rob_bone_data->mats2.begin);

    size_t mats2_size = rob_bone_data->total_bone_count - rob_bone_data->object_bone_count;
    rob_bone_data->mats2.begin = (mat4*)HeapCMallocAllocate(HeapCMallocSystem,
        mats2_size * sizeof(mat4), "ALLOCsys");
    rob_bone_data->mats2.end = rob_bone_data->mats2.begin + mats2_size;
    rob_bone_data->mats2.capacity_end = rob_bone_data->mats2.begin + mats2_size;

    for (mat4* i = rob_bone_data->mats2.begin; i != rob_bone_data->mats2.end; i++)
        *i = mat4_identity;

    if (rob_bone_data->nodes.begin)
        HeapCMallocFree(HeapCMallocSystem, rob_bone_data->nodes.begin);

    size_t nodes_size = rob_bone_data->total_bone_count;
    rob_bone_data->nodes.begin = (bone_node*)HeapCMallocAllocate(HeapCMallocSystem,
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
        i->exp_data.parent_scale = vec3_identity;
        i->ex_data_mat = 0;
    }
}

static void rob_chara_bone_data_calculate_bones(rob_chara_bone_data* rob_bone_data,
    aft_bone_database_bone* bones) {
    aft_bone_database_bones_calculate_count(bones, &rob_bone_data->object_bone_count,
        &rob_bone_data->motion_bone_count, &rob_bone_data->total_bone_count,
        &rob_bone_data->ik_bone_count, &rob_bone_data->chain_pos);
}

static void rob_chara_bone_data_set_mats(rob_chara_bone_data* rob_bone_data,
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

static void rob_chara_bone_data_set_parent_mats(rob_chara_bone_data* rob_bone_data,
    uint16_t* parent_indices) {
    if (vector_old_length(rob_bone_data->nodes) < 1)
        return;

    parent_indices++;
    for (bone_node* i = &rob_bone_data->nodes.begin[1]; i != rob_bone_data->nodes.end; i++)
        i->parent = &rob_bone_data->nodes.begin[*parent_indices++];
}

static void bone_data_parent_load_rob_chara(bone_data_parent* a1) {
    rob_chara_bone_data* rob_bone_data = a1->rob_bone_data;
    if (!rob_bone_data)
        return;

    a1->motion_bone_count = rob_bone_data->motion_bone_count;
    a1->ik_bone_count = rob_bone_data->ik_bone_count;
    a1->chain_pos = rob_bone_data->chain_pos;

    if (a1->bones.begin)
        HeapCMallocFree(HeapCMallocSystem, a1->bones.begin);

    size_t bones_size = rob_bone_data->motion_bone_count;
    a1->bones.begin = (bone_data*)HeapCMallocAllocate(HeapCMallocSystem,
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
    rob_chara_bone_data* rob_bone_data = bone->rob_bone_data;
    size_t chain_pos = 0;
    size_t total_bone_count = 0;
    size_t ik_bone_count = 0;

    aft_bone_database_bone* v15 = bones;
    bone_data* v16 = bone->bones.begin;
    for (size_t i = 0; v15->type != AFT_BONE_DATABASE_BONE_END; i++, v15++, v16++) {
        v16->motion_bone_index = (motion_bone_index)i;
        v16->type = (aft_bone_database_bone_type)v15->type;
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
    rob_chara_bone_data* rob_bone_data) {
    bone->rob_bone_data = rob_bone_data;
    aft_bone_database_bone* common_bone_data = aft_bone_database_get_bones_by_type(rob_bone_data->base_skeleton_type);
    vec3* common_translation = aft_bone_database_get_skeleton_positions_by_type(rob_bone_data->base_skeleton_type);
    vec3* translation = aft_bone_database_get_skeleton_positions_by_type(rob_bone_data->skeleton_type);
    bone_data_parent_load_rob_chara(bone);
    bone_data_parent_load_bone_database(bone, common_bone_data, common_translation, translation);
}

static void mot_key_data_get_key_set_count(mot_key_data* a1, size_t motion_bone_count, size_t ik_bone_count) {
    a1->key_set_count = (motion_bone_count + ik_bone_count) * 3 + 16;
}

static void mot_key_data_reserve_key_sets(mot_key_data* a1) {
    if (a1->key_set.begin)
        HeapCMallocFree(HeapCMallocSystem, a1->key_set.begin);

    size_t key_set_count = a1->key_set_count;
    a1->key_set.begin = (mot_key_set*)HeapCMallocAllocate(HeapCMallocSystem,
        key_set_count * sizeof(mot_key_set), "ALLOCsys");
    a1->key_set.end = a1->key_set.begin + key_set_count;
    a1->key_set.capacity_end = a1->key_set.begin + key_set_count;

    for (mot_key_set* i = a1->key_set.begin; i != a1->key_set.end; i++) {
        i->type = (mot_key_set_type)0;
        i->keys_count = 0;
        i->current_key = 0;
        i->last_key = 0;
        i->frames = 0;
        i->values = 0;
    }

    if (a1->key_set_data.begin)
        HeapCMallocFree(HeapCMallocSystem, a1->key_set_data.begin);

    a1->key_set_data.begin = (float_t*)HeapCMallocAllocate(HeapCMallocSystem,
        key_set_count * sizeof(float_t), "ALLOCsys");
    a1->key_set_data.end = a1->key_set_data.begin + key_set_count;
    a1->key_set_data.capacity_end = a1->key_set_data.begin + key_set_count;

    for (float_t* i = a1->key_set_data.begin; i != a1->key_set_data.end; i++)
        *i = 0.0f;

    a1->mot.key_sets = a1->key_set.begin;
    a1->key_sets_ready = true;
}

static void mot_key_data_init_key_sets(mot_key_data* a1, aft_bone_database_skeleton_type type,
    size_t motion_bone_count, size_t ik_bone_count) {
    mot_key_data_get_key_set_count(a1, motion_bone_count, ik_bone_count);
    mot_key_data_reserve_key_sets(a1);
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
        uint32_t* v14 = (uint32_t*)HeapCMallocAllocate(HeapCMallocSystem,
            sizeof(uint32_t) * v12, "ALLOCsys");
        size_t v15 = a3 - a1->bitfield.begin;
        sub_14040BD00(&v14[v15], a4, a5);
        sub_14040BDB0(a1->bitfield.begin, a3, v14);
        sub_14040BDB0(a3, a1->bitfield.end, &v14[v15 + a4]);
        size_t v16 = vector_old_length(a1->bitfield) + a4;
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

static void sub_140414550(struc_313* a1, size_t a2, bool a3) {
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
    int32_t v2 = MOTION_BONE_N_HARA_CP;
    size_t v5 = a1->field_8.motion_bone_count;
    uint32_t* v6 = a1->field_8.bitfield.begin;
    while (true) {
        struc_314 v7;
        v7.field_0 = &v6[v5 >> 5];
        v7.field_8 = v5 & 0x1F;
        if (v1.field_0 == v7.field_0 && v1.field_8 == v7.field_8)
            break;

        if (a1->bone_check_func((motion_bone_index)v2++))
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

static void motion_blend_mot_load_bone_data(motion_blend_mot* a1, rob_chara_bone_data* rob_bone_data,
    bool(*bone_check_func)(motion_bone_index)) {
    bone_data_parent_data_init(&a1->bone_data, rob_bone_data);
    mot_key_data_init_key_sets(
        &a1->mot_key_data,
        a1->bone_data.rob_bone_data->base_skeleton_type,
        a1->bone_data.motion_bone_count,
        a1->bone_data.ik_bone_count);
    sub_140413350(&a1->field_0, bone_check_func, a1->bone_data.motion_bone_count);
}

static void sub_140414900(struc_308* a1, mat4* mat) {
    if (mat) {
        a1->mat = *mat;
        a1->field_8C = true;
    }
    else {
        a1->mat = mat4u_identity;
        a1->field_8C = false;
    }
}

static void bone_data_mult_1_ik_hands(bone_data* a1, vec3* a2) {
    mat4 mat;
    mat4_transpose(a1->node[0].mat, &mat);

    vec3 v15;
    mat4_mult_vec3_inv_trans(&mat, a2, &v15);

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

static void bone_data_mult_1_ik_hands_2(bone_data* a1, vec3* a2, float_t a3) {
    float_t len;
    vec3 v8;

    mat4 mat;
    mat4_transpose(a1->node[0].mat, &mat);
    mat4_mult_vec3_inv_trans(&mat, a2, &v8);
    v8.x = 0.0f;
    vec3_length(v8, len);
    if (len <= 0.000001f)
        return;

    vec3_mult_scalar(v8, 1.0f / len, v8);
    float_t angle = atan2f(v8.z, v8.y);
    mat4_rotate_x_mult(&mat, angle * a3, &mat);
    mat4_transpose(&mat, a1->node[0].mat);
}

static void bone_data_mult_1_ik_legs(bone_data* a1, vec3* a2) {
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

static bool bone_data_mult_1_ik(bone_data* a1, bone_data* a2) {
    bone_node* v5;
    vec3 v30;
    mat4 mat;

    switch (a1->motion_bone_index) {
    case MOTION_BONE_N_SKATA_L_WJ_CD_EX:
        v5 = a2[MOTION_BONE_C_KATA_L].node;
        mat4_transpose(v5[2].mat, &mat);
        mat4_get_translation(&mat, &v30);
        bone_data_mult_1_ik_hands(a1, &v30);
        break;
    case MOTION_BONE_N_SKATA_R_WJ_CD_EX:
        v5 = a2[MOTION_BONE_C_KATA_R].node;
        mat4_transpose(v5[2].mat, &mat);
        mat4_get_translation(&mat, &v30);
        bone_data_mult_1_ik_hands(a1, &v30);
        break;
    case MOTION_BONE_N_SKATA_B_L_WJ_CD_CU_EX:
        v5 = a2[MOTION_BONE_N_UP_KATA_L_EX].node;
        mat4_transpose(v5[0].mat, &mat);
        mat4_get_translation(&mat, &v30);
        bone_data_mult_1_ik_hands_2(a1, &v30, 0.333f);
        break;
    case MOTION_BONE_N_SKATA_B_R_WJ_CD_CU_EX:
        v5 = a2[MOTION_BONE_N_UP_KATA_R_EX].node;
        mat4_transpose(v5[0].mat, &mat);
        mat4_get_translation(&mat, &v30);
        bone_data_mult_1_ik_hands_2(a1, &v30, 0.333f);
        break;
    case MOTION_BONE_N_SKATA_C_L_WJ_CD_CU_EX:
        v5 = a2[MOTION_BONE_N_UP_KATA_L_EX].node;
        mat4_transpose(v5[0].mat, &mat);
        mat4_get_translation(&mat, &v30);
        bone_data_mult_1_ik_hands_2(a1, &v30, 0.5f);
        break;
    case MOTION_BONE_N_SKATA_C_R_WJ_CD_CU_EX:
        v5 = a2[MOTION_BONE_N_UP_KATA_R_EX].node;
        mat4_transpose(v5[0].mat, &mat);
        mat4_get_translation(&mat, &v30);
        bone_data_mult_1_ik_hands_2(a1, &v30, 0.5f);
        break;
    case MOTION_BONE_N_MOMO_A_L_WJ_CD_EX:
        v5 = a2[MOTION_BONE_CL_MOMO_L].node;
        mat4_transpose(v5[2].mat, &mat);
        mat4_get_translation(&mat, &v30);
        mat4_transpose(a1->node[0].mat, &mat);
        mat4_mult_vec3_inv_trans(&mat, &v30, &v30);
        vec3_negate(v30, v30);
        bone_data_mult_1_ik_legs(a1, &v30);
        break;
    case MOTION_BONE_N_MOMO_A_R_WJ_CD_EX:
        v5 = a2[MOTION_BONE_CL_MOMO_R].node;
        mat4_transpose(v5[2].mat, &mat);
        mat4_get_translation(&mat, &v30);
        mat4_transpose(a1->node[0].mat, &mat);
        mat4_mult_vec3_inv_trans(&mat, &v30, &v30);
        vec3_negate(v30, v30);
        bone_data_mult_1_ik_legs(a1, &v30);
        break;
    case MOTION_BONE_N_HARA_CD_EX:
        v5 = a2[MOTION_BONE_KL_MUNE_B_WJ].node;
        mat4_transpose(v5[0].mat, &mat);
        mat4_get_translation(&mat, &v30);
        mat4_transpose(a1->node[0].mat, &mat);
        mat4_mult_vec3_inv_trans(&mat, &v30, &v30);
        bone_data_mult_1_ik_legs(a1, &v30);
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
        mat4_transpose(&a3[MOTION_BONE_CL_KAO].rot_mat[0], &dst);
        mat4_transpose(&a3[MOTION_BONE_CL_KAO].rot_mat[1], &mat);
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
        exp_data->rotation.x = sub_1401E9D10(v11->exp_data.rotation.x) * (float_t)(1.0 / 3.0);
        break;
    case MOTION_BONE_N_SUDE_R_WJ_EX:
    case MOTION_BONE_N_SUDE_B_R_WJ_EX:
        v11 = &a3[MOTION_BONE_KL_TE_R_WJ].node[0];
        exp_data->rotation.x = sub_1401E9D10(v11->exp_data.rotation.x) * (float_t)(1.0 / 3.0);
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
        exp_data->rotation.y = sub_1401E9D10(v11->exp_data.rotation.y
            + v11->exp_data.rotation.x) * (float_t)(1.0 / 3.0);
        break;
    case MOTION_BONE_N_MOMO_B_R_WJ_EX:
    case MOTION_BONE_N_MOMO_C_R_WJ_EX:
        v11 = &a3[MOTION_BONE_CL_MOMO_R].node[0];
        exp_data->rotation.y = sub_1401E9D10(v11->exp_data.rotation.y
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

void bone_data_mult_ik(bone_data* a1, int32_t skeleton_select) {
    if (a1->type < AFT_BONE_DATABASE_BONE_HEAD_IK_ROTATION)
        return;

    mat4 mat;
    mat4_transpose(a1->node[0].mat, &mat);

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
        mat4 pole_target_mat;
        mat4_transpose(a1->pole_target_mat, &pole_target_mat);
        vec3 pole_target;
        mat4_get_translation(&pole_target_mat, &pole_target);
        mat4_mult_vec3_inv_trans(&mat, &pole_target, &pole_target);
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
        mat4_transpose(&rot_mat, &a1->rot_mat[1]);
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
    if (v8 > 0.000001f) {
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
    mat4_transpose(&rot_mat, &a1->rot_mat[1]);
    mat4_translate_mult(&mat, v21, 0.0f, 0.0f, &mat);
    mat4_rotate_z_mult_sin_cos(&mat, v25, v22, &mat);
    mat4_transpose(&mat, a1->node[2].mat);
    mat4_rotate_z_sin_cos(v25, v22, &rot_mat);
    mat4_transpose(&rot_mat, &a1->rot_mat[2]);
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
        mat4_mult_vec3_inv_trans(&mat, &a1->trans, &a1->trans);
        mat4_translate_mult(&mat, a1->trans.x, a1->trans.y, a1->trans.z, &mat);
        a1->rot_mat[0] = mat4u_identity;
    }
    else {
        mat4 rot_mat;
        if (a1->type == AFT_BONE_DATABASE_BONE_POSITION_ROTATION) {
            mat4_transpose(&a1->rot_mat[0], &rot_mat);
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
        mat4_transpose(&rot_mat, &a1->rot_mat[0]);
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
                mat4_transpose(&a1->rot_mat[0], &rot_mat);
                mat4_get_rotation(&rot_mat, &a1->node[0].exp_data.rotation);
            }
            else if (bone_data_mult_1_exp_data(a1, &a1->node[0].exp_data, a3)) {
                vec3 rotation = a1->node[0].exp_data.rotation;
                mat4_rotate(rotation.x, rotation.y, rotation.z, &rot_mat);
                mat4_transpose(&rot_mat, &a1->rot_mat[0]);
            }
            else {
                mat4_transpose(&mat, a1->node[0].mat);

                if (bone_data_mult_1_ik(a1, a3)) {
                    mat4 dst;
                    mat4_invrot_normalized(&mat, &rot_mat);
                    mat4_transpose(a1->node[0].mat, &dst);
                    mat4_mult(&dst, &rot_mat, &rot_mat);
                    mat4_clear_trans(&rot_mat, &rot_mat);
                    mat4_get_rotation(&rot_mat, &a1->node[0].exp_data.rotation);
                    mat4_transpose(&rot_mat, &a1->rot_mat[0]);
                }
                else
                    a1->rot_mat[0] = mat4u_identity;
            }
        }

        mat4_transpose(&a1->rot_mat[0], &rot_mat);
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

    mat4_transpose(&a1->rot_mat[1], &rot_mat);
    mat4_mult(&rot_mat, &mat, &mat);
    mat4_transpose(&mat, a1->node[1].mat);
    mat4_translate_mult(&mat, a1->ik_segment_length[1], 0.0f, 0.0f, &mat);

    if (a1->type == AFT_BONE_DATABASE_BONE_HEAD_IK_ROTATION) {
        mat4_transpose(&mat, a1->node[2].mat);
        if (!a4)
            return;

        a1->node[1].exp_data.position = vec3_null;
        mat4_transpose(&a1->rot_mat[1], &rot_mat);
        mat4_get_rotation(&rot_mat, &a1->node[1].exp_data.rotation);
        bone_node_expression_data_reset_scale(&a1->node[1].exp_data);
        bone_node_expression_data_set_position_rotation(&a1->node[2].exp_data,
            a1->ik_segment_length[1], 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    }
    else {
        mat4_transpose(&a1->rot_mat[2], &rot_mat);
        mat4_mult(&rot_mat, &mat, &mat);
        mat4_transpose(&mat, a1->node[2].mat);
        mat4_translate_mult(&mat, a1->ik_2nd_segment_length[1], 0.0f, 0.0f, &mat);
        mat4_transpose(&mat, a1->node[3].mat);
        if (!a4)
            return;

        a1->node[1].exp_data.position = vec3_null;
        mat4_transpose(&a1->rot_mat[1], &rot_mat);
        mat4_get_rotation(&rot_mat, &a1->node[1].exp_data.rotation);
        bone_node_expression_data_reset_scale(&a1->node[1].exp_data);
        a1->node[2].exp_data.position.x = a1->ik_segment_length[1];
        *(vec2*)&a1->node[2].exp_data.position.y = vec2_null;
        mat4_transpose(&a1->rot_mat[2], &rot_mat);
        mat4_get_rotation(&rot_mat, &a1->node[2].exp_data.rotation);
        bone_node_expression_data_reset_scale(&a1->node[2].exp_data);
        bone_node_expression_data_set_position_rotation(&a1->node[3].exp_data,
            a1->ik_2nd_segment_length[1], 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    }
}

void bone_data_mult_2(rob_chara_bone_data* rob_bone_data, mat4* mat) {
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
    int32_t skeleton_select = a1->mot_key_data.skeleton_select;
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
    mat4_transpose(&b_n_hara_cp->rot_mat[0], &rot_mat);
    mat4_transpose(&b_kg_hara_y->rot_mat[0], &mat);
    mat4_mult(&mat, &rot_mat, &rot_mat);
    mat4_transpose(&b_kl_hara_xz->rot_mat[0], &mat);
    mat4_mult(&mat, &rot_mat, &rot_mat);
    mat4_transpose(&b_kl_hara_etc->rot_mat[0], &mat);
    mat4_mult(&mat, &rot_mat, &rot_mat);
    mat4_transpose(&rot_mat, &b_n_hara_cp->rot_mat[0]);
    b_kg_hara_y->rot_mat[0] = mat4u_identity;
    b_kl_hara_xz->rot_mat[0] = mat4u_identity;
    b_kl_hara_etc->rot_mat[0] = mat4u_identity;

    float_t v8 = a1->field_4F8.field_C0;
    float_t v9 = a1->field_4F8.field_C4;
    vec3 v10 = a1->field_4F8.field_C8;
    a1->field_4F8.field_A8 = b_n_hara_cp->trans;
    if (!a1->mot_key_data.skeleton_select) {
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
    motion_blend* v1 = a1->blend;
    if (v1)
        return v1->__vftable->field_30(v1);
    else
        return false;
}

static bool sub_140410250(struc_313* a1, size_t a2) {
    return (a1->bitfield.begin[a2 >> 5] & (1 << (a2 & 0x1F))) != 0;
}

static void sub_140410A40(motion_blend_mot* a1, vector_old_bone_data* a2, vector_old_bone_data* a3) {
    motion_blend* v1 = a1->blend;
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

static void sub_140410B70(motion_blend_mot* a1, vector_old_bone_data* a2) {
    motion_blend* v1 = a1->blend;
    if (!v1 || !v1->field_9)
        return;

    v1->__vftable->field_20(v1, &a1->bone_data.bones, a2);
    for (bone_data* i = a1->bone_data.bones.begin; i != a1->bone_data.bones.end; i++) {
        if (!sub_140410250(&a1->field_0.field_8, i->motion_bone_index))
            continue;

        bone_data* v6 = 0;
        if (a2)
            v6 = &a2->begin[i->motion_bone_index];
        v1->__vftable->field_28(a1->blend, &a1->bone_data.bones.begin[i->motion_bone_index], v6);
    }
}

static motion_blend_type sub_1404125C0(motion_blend_mot* a1) {
    motion_blend* v1 = a1->blend;
    if (v1 == &a1->cross.base)
        return MOTION_BLEND_CROSS;
    if (v1 == &a1->freeze.base)
        return MOTION_BLEND_FREEZE;
    if (v1 == &a1->combine.base.base)
        return MOTION_BLEND_COMBINE;
    return MOTION_BLEND;
}

static void sub_140410CB0(mot_blend* a1, vector_old_bone_data* a2) {
    motion_blend* v1 = &a1->blend.base;
    if (!v1->field_9)
        return;

    for (bone_data* i = a2->begin; i != a2->end; i++)
        if (sub_140410250(&a1->field_0.field_8, i->motion_bone_index))
            v1->__vftable->field_28(&a1->blend.base, &a2->begin[i->motion_bone_index], 0);
}

static void sub_1404182B0(rob_chara_bone_data* rob_bone_data) {
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

static void sub_14041B9F0(rob_chara_bone_data* rob_bone_data) {
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

    mat4 mat = a1->field_4F8.mat;
    bone_data* v3 = a1->bone_data.bones.begin;
    for (bone_data* v4 = a1->bone_data.bones.begin; v4 != a1->bone_data.bones.end; v4++)
        bone_data_mult_1(v4, &mat, v3, true);
}

void rob_chara_bone_data_update(rob_chara_bone_data* rob_bone_data, mat4* a2) {
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
    a1->mot_key_data.frame = -1.0f;
    a1->mot_key_data.motion_id = -1;
    a1->mot_play_data.frame_data.field_8 = 1.0f;
    a1->mot_play_data.frame_data.field_4 = 1.0f;
    a1->mot_play_data.frame_data.field_10 = -1.0f;
    a1->mot_play_data.frame_data.field_14 = -1.0f;
    a1->mot_play_data.frame_data.field_18 = -1;
    a1->mot_play_data.frame_data.field_1C = -1;
    a1->mot_play_data.frame_data.field_24 = -1.0f;
    a1->mot_play_data.frame_data.field_2C = -1;
    a1->blend.base.field_14 = 1.0f;
    return a1;
}

static void rob_chara_bone_data_eyes_xrot_adjust(rob_chara_bone_data* rob_bone_data,
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

static void rob_chara_bone_data_init_skeleton(rob_chara_bone_data* rob_bone_data,
    aft_bone_database_skeleton_type base_skeleton_type,
    aft_bone_database_skeleton_type skeleton_type) {
    if (rob_bone_data->base_skeleton_type == base_skeleton_type
        && rob_bone_data->skeleton_type == skeleton_type)
        return;

    aft_bone_database_bone* bones = aft_bone_database_get_bones_by_type(base_skeleton_type);
    rob_chara_bone_data_calculate_bones(rob_bone_data, bones);
    rob_chara_bone_data_reserve(rob_bone_data);
    rob_chara_bone_data_set_mats(rob_bone_data, bones);
    uint16_t* parent_indices = aft_bone_database_get_skeleton_parent_indices_by_type(base_skeleton_type);
    rob_chara_bone_data_set_parent_mats(rob_bone_data, parent_indices);
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

static void mot_key_data_get_key_set_count_by_bone_database_bones(mot_key_data* a1, aft_bone_database_bone* bones) {
    size_t object_bone_count;
    size_t motion_bone_count;
    size_t total_bone_count;
    size_t ik_bone_count;
    size_t chain_pos;
    aft_bone_database_bones_calculate_count(bones, &object_bone_count,
        &motion_bone_count, &total_bone_count, &ik_bone_count, &chain_pos);
    mot_key_data_get_key_set_count(a1, motion_bone_count, ik_bone_count);
}

static void mot_key_data_reserve_key_sets_by_skeleton_type(mot_key_data* a1,
    aft_bone_database_skeleton_type type) {
    mot_key_data_get_key_set_count_by_bone_database_bones(a1, aft_bone_database_get_bones_by_type(type));
    mot_key_data_reserve_key_sets(a1);
    a1->skeleton_type = type;
}

static void mot_blend_reserve_key_sets(mot_blend* blend, aft_bone_database_skeleton_type type,
    bool(*a3)(motion_bone_index), size_t motion_bone_count) {
    mot_key_data_reserve_key_sets_by_skeleton_type(&blend->mot_key_data, type);
    sub_140413350(&blend->field_0, a3, motion_bone_count);
}

static void rob_chara_bone_data_init_data(rob_chara_bone_data* rob_bone_data,
    aft_bone_database_skeleton_type base_type,
    aft_bone_database_skeleton_type type) {
    rob_chara_bone_data_init_skeleton(rob_bone_data, base_type, type);
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

static void sub_140548350(struc_525* a1) {
    a1->field_0 = 0;
    a1->field_4 = 0;
}

static int32_t* sub_140507140(int32_t* a1) {
    *a1 = 0;
    return a1;
}

static void sub_140506C20(struc_523* a1) {
    a1->field_0 = 0;
    a1->field_1 = 0;
    a1->field_4 = 0;
    a1->field_8 = 0;
    a1->field_9 = 0;
    a1->field_C = 0;
    a1->field_10 = 0;
    a1->field_1C = 0;
    a1->field_14 = 0;
    a1->field_18 = 0;
    a1->field_20 = 0;
    a1->field_24 = 0;
    a1->field_28 = 0;
    a1->field_2C = 0;
    a1->field_30 = 0;
    a1->field_34 = 0;
    a1->field_38 = 0;
    a1->field_3C = 0;
    a1->field_40 = 0;
    a1->field_44 = 0;
    a1->field_48 = 0;
    a1->field_4C = 0;
    a1->field_50 = 0;

    list_ptr_void_node* v2 = a1->field_58.head;
    list_ptr_void_node* v3 = v2->next;
    v2->next = v2;
    a1->field_58.head->prev = a1->field_58.head;
    a1->field_58.size = 0;
    while (v3 != a1->field_58.head) {
        list_ptr_void_node* v4 = v3->next;
        operator_delete(v3);
        v3 = v4;
    }
    a1->field_68 = 0;
    a1->field_70 = 0;
    a1->field_78 = 0;
    a1->field_80 = 0;
}

static void sub_1405071C0(struc_526* a1) {
    a1->field_0 = 0;
    a1->field_4 = 0;
}

static void sub_140505C90(struc_264* a1) {
    a1->field_0 = 0;
    a1->field_4.field_0 = 0;
    sub_140548350(&a1->field_4.field_4);
    a1->field_4.field_C = -1;
    a1->field_4.field_10 = 0;
    a1->field_18.field_0 = 0;
    sub_140548350(&a1->field_18.field_4);
    a1->field_18.field_C = -1;
    a1->field_18.field_10 = 0;
    a1->field_2C.field_0 = 0;
    sub_140548350(&a1->field_2C.field_4);
    a1->field_2C.field_C = -1;
    a1->field_2C.field_10 = 0;
    a1->field_40.field_0 = 0;
    sub_140548350(&a1->field_40.field_4);
    a1->field_40.field_C = -1;
    a1->field_40.field_10 = 0;
    a1->field_54.field_0 = 0;
    sub_140548350(&a1->field_54.field_4);
    a1->field_54.field_C = -1;
    a1->field_54.field_10 = 0;
    a1->field_68 = 0;
    a1->field_6C = 0;
    a1->field_70 = 0;
    a1->field_74 = 0;
    a1->field_78 = 0x7FFF;
    a1->field_7C = 0;
    a1->field_80 = 0;
    a1->field_84 = 0;
    a1->field_88 = 0;
    a1->field_8C = 0;
    a1->field_90 = 0;
    a1->field_94 = 0;
    a1->field_98 = 0;
    a1->field_9C = 0;
    a1->field_A0 = 0;
    a1->field_A4 = 0;
    a1->field_A5 = 0;
    a1->field_A8 = 0;
    sub_140507140(&a1->field_AC);
    a1->field_B0 = 0;
    a1->eyes_adjust = 0;
    sub_140506C20(&a1->field_B8);
    a1->field_140 = 0;
    a1->field_144 = 0;
    a1->field_145 = 0;
    a1->field_146 = 0;
    a1->field_147 = 0;
    a1->field_148 = 0;
    a1->field_14C = 0;
    a1->field_150 = 0;
    a1->field_152 = 0;
    a1->field_154 = 0;
    a1->field_158 = 0;
    a1->field_15C = 0;
    a1->field_160 = 0;
    a1->field_164 = 0;
    a1->field_168 = -1;
    a1->field_16C = 0;
    a1->field_170 = 0;
    a1->field_174 = 0;
    a1->field_178 = 0;
    a1->field_17C = 0;
    a1->field_180 = 0;
    a1->field_184 = 0;
    a1->field_188 = 0;
    a1->field_18C = 999.0f;
    a1->field_190 = 0;
    a1->field_194 = 0;
    a1->field_198 = 0;
    a1->field_19C = 0;
    a1->field_1A0 = 0;
    a1->field_1A4 = 0;
    a1->field_1A8 = 0;
    a1->field_1B0 = 0;
    a1->field_1B4 = 0;
    a1->field_1B8 = 0;
    a1->field_1BC = -1;
    sub_1405071C0(&a1->field_1C0);
    a1->field_1C8 = 0;
    a1->field_1C9 = 0;
    a1->field_1CA = 0;
    a1->field_1CC = 0;
    a1->field_1D0 = 0;
    a1->field_1D4 = 0;
}

static void rob_sub_action_init_sub(rob_sub_action* sub_act) {
    sub_act->data.field_0 = 0;
    sub_act->data.field_8 = 0;
    sub_act->data.field_10 = 0;
    sub_act->data.field_18 = 0;
    sub_act->data.cry.base.__vftable->field_8(&sub_act->data.cry.base);
    sub_act->data.shake_hand.base.__vftable->field_8(&sub_act->data.shake_hand.base);
    sub_act->data.embarrassed.base.__vftable->field_8(&sub_act->data.embarrassed.base);
    sub_act->data.angry.base.__vftable->field_8(&sub_act->data.angry.base);
    sub_act->data.laugh.base.__vftable->field_8(&sub_act->data.laugh.base);
}

static int32_t* sub_140507180(int32_t* a1) {
    *a1 = 0;
    return a1;
}

static void sub_140506440(rob_chara_adjust* a1) {
    a1->motion_id = -1;
    a1->prev_motion_id = -1;
    a1->frame_data.frame = 0.0f;
    a1->frame_data.prev_frame = 0.0f;
    a1->frame_data.last_set_frame = 0.0f;
    a1->step_data.frame = 0.0f;
    a1->step_data.field_4 = 0.0f;
    a1->step_data.field_8 = -1.0f;
    a1->step = -1.0f;
    a1->field_24 = 0;
    a1->field_28 = 0;
    a1->field_2C = 0;
    sub_140507180(&a1->field_30);
    a1->field_34 = 0;

    int32_t* v2 = a1->field_38;
    int32_t* v3 = a1->field_B8;
    for (int32_t i = 0; i < 32; i++) {
        *v2++ = -1;
        *v3++ = 0;
    }

    a1->field_138 = 1.0f;
    a1->field_13C = 1.0f;
    a1->field_140 = 0;
    a1->field_144 = 0;
    a1->field_150.field_0.base.__vftable->field_8(&a1->field_150.field_0.base);
    a1->field_150.field_48.base.__vftable->field_8(&a1->field_150.field_48.base);
    a1->field_150.field_90.base.__vftable->field_8(&a1->field_150.field_90.base);
    a1->field_150.field_D8.base.__vftable->field_8(&a1->field_150.field_D8.base);
    a1->field_150.field_120.base.__vftable->field_8(&a1->field_150.field_120.base);
    a1->field_150.field_168.base.__vftable->field_8(&a1->field_150.field_168.base);
    a1->field_150.field_1B0 = aft_object_info_null;
    a1->field_150.field_1B4 = aft_object_info_null;
    a1->field_150.field_1B8 = aft_object_info_null;
    a1->field_150.field_1BC = aft_object_info_null;
    a1->field_150.field_1C0 = 0;
    a1->field_150.field_1C4 = 0.0f;
    a1->field_318.base.__vftable->field_8(&a1->field_318);
    a1->field_360.base.__vftable->field_8(&a1->field_360);
    a1->field_3A8 = aft_object_info_null;
    a1->field_3B0.field_0.base.__vftable->field_8(&a1->field_3B0.field_0.base);
    a1->field_3B0.field_48.base.__vftable->field_8(&a1->field_3B0.field_48.base);
    a1->field_3B0.field_90.base.__vftable->field_8(&a1->field_3B0.field_90.base);
    a1->field_3B0.field_D8.base.__vftable->field_8(&a1->field_3B0.field_D8.base);
    a1->field_3B0.field_120.base.__vftable->field_8(&a1->field_3B0.field_120.base);
    a1->field_3B0.field_168.base.__vftable->field_8(&a1->field_3B0.field_168.base);
    a1->field_3B0.field_1B0 = aft_object_info_null;
    a1->field_3B0.field_1B4 = aft_object_info_null;
    a1->field_3B0.field_1B8 = aft_object_info_null;
    a1->field_3B0.field_1BC = aft_object_info_null;
    a1->field_3B0.field_1C0 = 0;

    rob_chara_data_parts_adjust* parts_adjust = a1->parts_adjust;
    rob_chara_data_parts_adjust* parts_adjust_prev = a1->parts_adjust_prev;
    for (int32_t i = 0; i < 13; i++) {
        parts_adjust->enable = false;
        parts_adjust->frame = 0.0f;
        parts_adjust->transition_frame = 0.0f;
        parts_adjust->curr_external_force = vec3_null;
        parts_adjust->curr_force = 1.0f;
        parts_adjust->curr_strength = 1.0f;
        parts_adjust->motion_id = -1;
        parts_adjust->set_frame = 0;
        parts_adjust->force_duration = 0.0f;
        parts_adjust->type = 6;
        parts_adjust->cycle_type = 0;
        parts_adjust->ignore_gravity = false;
        parts_adjust->external_force_min = vec3_null;
        parts_adjust->external_force_strength = vec3_null;
        parts_adjust->external_force = vec3_null;
        parts_adjust->cycle = 1.0f;
        parts_adjust->phase = 0.0f;
        parts_adjust->force = 1.0f;
        parts_adjust->strength = 1.0f;
        parts_adjust->strength_transition = 0.0f;
        parts_adjust_prev->enable = false;
        parts_adjust_prev->frame = 0.0f;
        parts_adjust_prev->transition_frame = 0.0f;
        parts_adjust_prev->curr_external_force = vec3_null;
        parts_adjust_prev->curr_force = 1.0f;
        parts_adjust_prev->curr_strength = 1.0f;
        parts_adjust_prev->motion_id = -1;
        parts_adjust_prev->set_frame = 0;
        parts_adjust_prev->force_duration = 0.0f;
        parts_adjust_prev->type = 6;
        parts_adjust_prev->cycle_type = 0;
        parts_adjust_prev->ignore_gravity = false;
        parts_adjust_prev->external_force_min = vec3_null;
        parts_adjust_prev->external_force_strength = vec3_null;
        parts_adjust_prev->external_force = vec3_null;
        parts_adjust_prev->cycle = 1.0f;
        parts_adjust_prev->phase = 0.0f;
        parts_adjust_prev->force = 1.0f;
        parts_adjust_prev->strength = 1.0f;
        parts_adjust_prev->strength_transition = 0.0f;
        parts_adjust++;
        parts_adjust_prev++;
    }

    rob_chara_data_parts_adjust* v7 = a1->field_10D8;
    struc_222* v8 = a1->field_12C8;
    rob_chara_data_hand_adjust* hand_adjust = a1->hand_adjust;
    rob_chara_data_hand_adjust* hand_adjust_prev = a1->hand_adjust_prev;
    for (int32_t i = 0; i < 2; i++) {
        v7->enable = false;
        v7->frame = 0.0f;
        v7->transition_frame = 0.0f;
        v7->curr_external_force = vec3_null;
        v7->curr_force = 1.0f;
        v7->curr_strength = 1.0f;
        v7->motion_id = -1;
        v7->set_frame = 0;
        v7->force_duration = 0.0f;
        v7->type = 6;
        v7->cycle_type = 0;
        v7->ignore_gravity = false;
        v7->external_force_min = vec3_null;
        v7->external_force_strength = vec3_null;
        v7->external_force = vec3_null;
        v7->cycle = 1.0f;
        v7->phase = 0.0f;
        v7->force = 1.0f;
        v7->strength = 1.0f;
        v7->strength_transition = 0.0f;
        v8->field_0 = 0;
        v8->field_4 = 0.0f;
        v8->field_8 = 0.0f;
        v8->field_C = 0.0f;
        v8->duration = 0.0f;
        v8->frame = 0.0f;
        hand_adjust->field_0 = 0;
        hand_adjust->field_2 = 0;
        hand_adjust->field_4 = -1;
        hand_adjust->field_8 = 1.0f;
        hand_adjust->field_C = 1.0f;
        hand_adjust->field_10 = 0.0f;
        hand_adjust->field_14 = 0.0f;
        hand_adjust->field_18 = 1.0f;
        hand_adjust->field_1C = 1.0f;
        hand_adjust->field_20 = 0;
        hand_adjust->field_21 = 0;
        hand_adjust->field_22 = 0;
        hand_adjust->field_23 = 0;
        hand_adjust->field_24 = vec3_null;
        hand_adjust->field_30 = vec3_null;
        hand_adjust->field_3C = 0;
        hand_adjust_prev->field_0 = 0;
        hand_adjust_prev->field_2 = 0;
        hand_adjust_prev->field_4 = -1;
        hand_adjust_prev->field_8 = 1.0f;
        hand_adjust_prev->field_C = 1.0f;
        hand_adjust_prev->field_10 = 0.0f;
        hand_adjust_prev->field_14 = 0.0f;
        hand_adjust_prev->field_18 = 1.0f;
        hand_adjust_prev->field_1C = 1.0f;
        hand_adjust_prev->field_20 = 0;
        hand_adjust_prev->field_21 = 0;
        hand_adjust_prev->field_22 = 0;
        hand_adjust_prev->field_23 = 0;
        hand_adjust_prev->field_24 = vec3_null;
        hand_adjust_prev->field_30 = vec3_null;
        hand_adjust_prev->field_3C = 0;
        v7++;
        v8++;
        hand_adjust++;
        hand_adjust_prev++;
    }
}

static struc_228* sub_14053A660(struc_228* a1) {
    a1->field_0 = 0;
    a1->field_4 = 0;
    a1->field_8 = 0;
    a1->field_C = 0;
    return a1;
}

static void sub_140539750(struc_223* a1) {
    void(*sub_140537110)(struc_223 * a1)
        = (void(*)(struc_223*))0x0000000140537110;
    void(*sub_1405335C0)(struc_223 * a1)
        = (void(*)(struc_223*))0x00000001405335C0;

    sub_14053A660(&a1->field_20);
    sub_14053A660(&a1->field_10);
    sub_140537110(a1);
    sub_1405335C0(a1);
    a1->field_7A0 = 0;
    a1->field_7A8 = -1;
}

static void rob_chara_data_reset(rob_chara_data* a1) {
    void(*rob_chara_data_miku_rot_reset)(rob_chara_data_miku_rot * a1)
        = (void(*)(rob_chara_data_miku_rot*))0x0000000140506A20;
    void(*rob_chara_data_adjuct_reset)(rob_chara_adjust_data * a1)
        = (void(*)(rob_chara_adjust_data*))0x0000000140506AE0;
    void(*sub_140505FB0)(struc_209 * a1)
        = (void(*)(struc_209*))0x0000000140505FB0;

    a1->field_0 = 0;
    sub_140505C90(&a1->field_8);
    rob_sub_action_init_sub(&a1->rob_sub_action);
    sub_140506440(&a1->adjust);
    sub_140539750(&a1->field_1588);
    rob_chara_data_miku_rot_reset(&a1->miku_rot);
    rob_chara_data_adjuct_reset(&a1->adjust_data);
    sub_140505FB0(&a1->field_1E68);
    a1->field_3D90 = 0.0f;
    a1->field_3D94 = 0;
    a1->field_3D98 = 0;
    a1->field_3D9A = 0;
    a1->field_3D9C = 0;
    a1->field_3D9D = 0;
    a1->field_3DA0 = 0;
    a1->field_3DA4 = 0;
    a1->field_3DA8 = 0;
    a1->field_3DB0 = 0;
    a1->field_3DB8 = 0;
    a1->field_3DBC = 0;
    a1->field_3DC0 = 0;
    a1->field_3DC4 = 0;
    a1->field_3DC8 = 0;
    a1->field_3DCC = 0;
    a1->field_3DD0 = 0;
    a1->field_3DD8 = 0;
    a1->field_3DD4 = 0.0f;
    a1->field_3DDC = 0.0f;
    a1->field_3DE0 = 0;
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
    a1->eyes_adjust = 0.0f;
    a1->prev_eyes_adjust = 0.0f;
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
    a1->mot_key_data.key_sets_ready = 0;
    a1->mot_key_data.skeleton_type = AFT_BONE_DATABASE_SKELETON_NONE;
    a1->mot_key_data.frame = -1.0f;
    a1->mot_key_data.key_set_count = 0;
    a1->mot_key_data.mot_data = 0;
    a1->mot_key_data.skeleton_select = 0;
    a1->mot_key_data.field_68.field_0 = 0;
    a1->mot_key_data.field_68.field_4 = 0.0f;
    a1->mot_key_data.key_set.end = a1->mot_key_data.key_set.begin;
    a1->mot_key_data.key_set_data.end = a1->mot_key_data.key_set_data.begin;
    a1->mot_key_data.motion_id = -1;
    a1->mot_play_data.frame_data.frame = 0.0f;
    a1->mot_play_data.frame_data.field_8 = 1.0f;
    a1->mot_play_data.frame_data.field_4 = 1.0f;
    a1->mot_play_data.frame_data.field_C = 0.0f;
    a1->mot_play_data.frame_data.field_10 = -1.0f;
    a1->mot_play_data.frame_data.field_14 = -1.0f;
    a1->mot_play_data.frame_data.field_18 = -1;
    a1->mot_play_data.frame_data.field_1C = -1;
    a1->mot_play_data.frame_data.field_20 = 0.0f;
    a1->mot_play_data.frame_data.field_24 = -1.0f;
    a1->mot_play_data.frame_data.field_28 = 0;
    a1->mot_play_data.frame_data.field_2C = -1;
    a1->mot_play_data.field_30 = 0;
    a1->mot_play_data.field_34 = 0;
    a1->mot_play_data.field_38 = 0.0f;
    a1->mot_play_data.field_40 = 0;
    a1->mot_play_data.field_48 = 0;
    sub_140413470(&a1->field_4F8);
    a1->field_0.bone_check_func = 0;
    sub_1404119A0(&a1->field_0.field_8);
    a1->field_0.motion_bone_count = 0;
    a1->cross.base.__vftable->field_8(&a1->cross.base);
    a1->freeze.base.__vftable->field_8(&a1->freeze.base);
    a1->combine.base.base.__vftable->field_8(&a1->combine.base.base);
    a1->blend = 0;
}

static void mot_blend_reset(mot_blend* a1) {
    a1->mot_key_data.key_sets_ready = 0;
    a1->mot_key_data.skeleton_type = AFT_BONE_DATABASE_SKELETON_NONE;
    a1->mot_key_data.frame = -1.0f;
    a1->mot_key_data.key_set_count = 0;
    a1->mot_key_data.mot_data = 0;
    a1->mot_key_data.skeleton_select = 0;
    a1->mot_key_data.field_68.field_0 = 0;
    a1->mot_key_data.field_68.field_4 = 0.0f;
    a1->mot_key_data.key_set.end = a1->mot_key_data.key_set.begin;
    a1->mot_key_data.key_set_data.end = a1->mot_key_data.key_set_data.begin;
    a1->mot_key_data.motion_id = -1;
    a1->mot_play_data.frame_data.frame = 0.0f;
    a1->mot_play_data.frame_data.field_8 = 1.0f;
    a1->mot_play_data.frame_data.field_4 = 1.0f;
    a1->mot_play_data.frame_data.field_C = 0.0f;
    a1->mot_play_data.frame_data.field_10 = -1.0f;
    a1->mot_play_data.frame_data.field_14 = -1.0f;
    a1->mot_play_data.frame_data.field_18 = -1;
    a1->mot_play_data.frame_data.field_1C = -1;
    a1->mot_play_data.frame_data.field_20 = 0.0f;
    a1->mot_play_data.frame_data.field_24 = -1.0f;
    a1->mot_play_data.frame_data.field_28 = 0;
    a1->mot_play_data.frame_data.field_2C = -1;
    a1->mot_play_data.field_30 = 0;
    a1->mot_play_data.field_34 = 0;
    a1->mot_play_data.field_38 = 0;
    a1->mot_play_data.field_40 = 0;
    a1->mot_play_data.field_48 = 0;
    a1->blend.base.__vftable->field_8(&a1->blend.base);
    a1->field_0.bone_check_func = 0;
    sub_1404119A0(&a1->field_0.field_8);
    a1->field_0.motion_bone_count = 0;
    a1->field_30 = 0;
}

static void sub_1404198D0(struc_258* a1) {
    a1->field_0 = 0;
    a1->field_8 = mat4u_identity;
    a1->field_48.field_0 = 0.0f;
    a1->field_48.field_4 = 0.0f;
    a1->field_48.field_8 = 0.0f;
    a1->field_48.field_C = 0.0f;
    a1->field_48.field_10 = 0.0f;
    a1->field_48.field_14 = 0.0f;
    a1->field_48.field_18 = 0.0f;
    a1->field_48.field_1C = 0.0f;
    a1->field_48.field_20 = 0.0f;
    a1->field_48.field_24 = 0.0f;
    a1->field_48.field_28 = 0.0f;
    a1->field_48.field_2C = 1.0f;
    a1->field_48.field_30 = 1.0f;
    a1->field_48.field_34 = -3.8f;
    a1->field_48.field_38 = 6.0f;
    a1->field_48.field_3C = -3.8f;
    a1->field_48.field_40 = 6.0f;
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

static void rob_chara_bone_data_reset(rob_chara_bone_data* rob_bone_data) {
    void(*rob_chara_bone_data_motion_blend_mot_list_free)(rob_chara_bone_data * rob_bone_data, size_t a2)
        = (void(*)(rob_chara_bone_data*, size_t))0x0000000140418E80;
    void(*rob_chara_bone_data_motion_blend_mot_list_init)(rob_chara_bone_data * rob_bone_data)
        = (void(*)(rob_chara_bone_data*))0x000000014041A9E0;
    void(*rob_chara_bone_data_motion_blend_mot_free)(rob_chara_bone_data * rob_bone_data)
        = (void(*)(rob_chara_bone_data*))0x0000000140419000;
    void(*rob_chara_bone_data_motion_blend_mot_init)(rob_chara_bone_data * rob_bone_data)
        = (void(*)(rob_chara_bone_data*))0x000000014041A1A0;

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

    if (rob_bone_data->motion_loaded.size && vector_old_length(rob_bone_data->motions) == 3) {
        rob_chara_bone_data_motion_blend_mot_list_free(rob_bone_data, 0);
        for (motion_blend_mot** i = rob_bone_data->motions.begin; i != rob_bone_data->motions.end; i++)
            motion_blend_mot_reset(*i);
        rob_chara_bone_data_motion_blend_mot_list_init(rob_bone_data);
    }
    else {
        rob_chara_bone_data_motion_blend_mot_free(rob_bone_data);
        rob_chara_bone_data_motion_blend_mot_init(rob_bone_data);
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
    skeleton_rotation_offset** skeleton_rotation_offset_array_ptr
        = (skeleton_rotation_offset**)0x0000000140A01560;
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

        key_index--;
        if (type == MOT_KEY_SET_HERMITE)
            *value = values[key_index];
        else
            *value = values[2 * key_index];
        a4->current_key = (int32_t)key_index;
    }
}

static void mot_key_data_interpolate(mot_key_data* a1, float_t frame,
    uint32_t key_set_offset, uint32_t key_set_count) {
    mot_key_frame_interpolate(&a1->mot, frame,
        &a1->key_set_data.begin[key_set_offset],
        &a1->key_set.begin[key_set_offset], key_set_count, &a1->field_68);
}

static bool motion_blend_mot_interpolate_get_reverse(int32_t* a1) {
    return *a1 & 0x01 && ~*a1 & 0x08 || ~*a1 & 0x01 && *a1 & 0x08;
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
            mat4_transpose(&rot_mat, &i->rot_mat[0]);
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
    vec3* keyframe_data = (vec3*)a1->mot_key_data.key_set_data.begin;
    bool reverse = motion_blend_mot_interpolate_get_reverse(&a1->field_4F8.field_0);
    float_t frame = a1->mot_play_data.frame_data.frame;

    aft_bone_database_skeleton_type skeleton_type = a1->bone_data.rob_bone_data->base_skeleton_type;
    bone_data* bones_data = a1->bone_data.bones.begin;
    for (uint16_t* i = a1->bone_data.bone_indices.begin; i != a1->bone_data.bone_indices.end; i++) {
        bone_data* data = &bones_data[*i];
        bool get_data = sub_140410250(&a1->field_0.field_8, data->motion_bone_index);
        if (reverse && data->mirror != 255)
            data = &bones_data[data->mirror];

        if (get_data && frame != data->frame) {
            mot_key_data_interpolate(&a1->mot_key_data, frame, data->key_set_offset, data->key_set_count);
            data->frame = frame;
        }

        keyframe_data = bone_data_set_key_data(data, keyframe_data, skeleton_type, get_data, reverse);
    }

    uint32_t bone_key_set_count = a1->bone_data.bone_key_set_count;
    if (frame != a1->mot_key_data.frame) {
        mot_key_data_interpolate(&a1->mot_key_data, frame, bone_key_set_count, a1->bone_data.global_key_set_count);
        a1->mot_key_data.frame = frame;
    }

    keyframe_data = (vec3*)&a1->mot_key_data.key_set_data.begin[bone_key_set_count];
    vec3 global_trans = keyframe_data[0];
    vec3 global_rotation = keyframe_data[1];
    if (reverse)
        vec3_negate(global_trans, global_trans);
    a1->bone_data.global_trans = global_trans;
    a1->bone_data.global_rotation = global_rotation;

    sub_140415430(a1);
}

static void mot_blend_interpolate(mot_blend* a1, vector_old_bone_data* bones,
    vector_old_uint16_t* bone_indices, aft_bone_database_skeleton_type skeleton_type) {
    if (!a1->mot_key_data.key_sets_ready || !a1->mot_key_data.mot_data || a1->field_30)
        return;

    float_t frame = a1->mot_play_data.frame_data.frame;
    vec3* keyframe_data = (vec3*)a1->mot_key_data.key_set_data.begin;
    bone_data* bones_data = bones->begin;
    for (uint16_t* i = bone_indices->begin; i != bone_indices->end; i++) {
        bone_data* data = &bones_data[*i];
        bool get_data = sub_140410250(&a1->field_0.field_8, data->motion_bone_index);
        if (get_data) {
            mot_key_data_interpolate(&a1->mot_key_data, frame, data->key_set_offset, data->key_set_count);
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
    for (uint32_t i = MOTION_BONE_FACE_ROOT;
        i <= MOTION_BONE_TL_TOOTH_UPPER_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));
}

static void sub_14041B580(struc_313* a1) {
    uint32_t* bitfield = a1->bitfield.begin;
    for (uint32_t i = MOTION_BONE_FACE_ROOT;
        i <= MOTION_BONE_TL_TOOTH_UPPER_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));

    for (uint32_t i = MOTION_BONE_N_EYE_L;
        i <= MOTION_BONE_KL_HIGHLIGHT_R_WJ; i++)
        bitfield[i >> 5] |= 1 << (i & 0x1F);
}

static void sub_14041B800(struc_313* a1) {
    uint32_t* bitfield = a1->bitfield.begin;
    for (uint32_t i = MOTION_BONE_N_KUTI_D;
        i <= MOTION_BONE_TL_KUTI_U_R_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));

    for (uint32_t i = MOTION_BONE_N_AGO;
        i <= MOTION_BONE_TL_TOOTH_UNDER_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));

    for (uint32_t i = MOTION_BONE_N_TOOTH_UPPER;
        i <= MOTION_BONE_TL_TOOTH_UPPER_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));
}

static void sub_14041B440(struc_313* a1) {
    uint32_t* bitfield = a1->bitfield.begin;
    for (uint32_t i = MOTION_BONE_N_EYE_L;
        i <= MOTION_BONE_KL_HIGHLIGHT_R_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));
}

static void sub_14041B1C0(struc_313* a1) {
    uint32_t* bitfield = a1->bitfield.begin;
    for (uint32_t i = MOTION_BONE_N_MABU_L_D_A;
        i <= MOTION_BONE_TL_MABU_R_U_C_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));

    for (uint32_t i = MOTION_BONE_N_EYELID_L_A;
        i <= MOTION_BONE_TL_EYELID_R_B_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));
}

static void sub_14041B6B0(struc_313* a1) {
    uint32_t* bitfield = a1->bitfield.begin;
    for (uint32_t i = MOTION_BONE_N_HITO_L_EX;
        i <= MOTION_BONE_NL_OYA_C_L_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));
}

static void sub_14041B750(struc_313* a1) {
    uint32_t* bitfield = a1->bitfield.begin;
    for (uint32_t i = MOTION_BONE_N_HITO_R_EX;
        i <= MOTION_BONE_NL_OYA_C_R_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));
}

static void sub_14041B300(struc_313* a1) {
    uint32_t* bitfield = a1->bitfield.begin;
    for (uint32_t i = MOTION_BONE_N_MABU_L_D_A;
        i <= MOTION_BONE_TL_MABU_R_U_C_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));

    for (uint32_t i = MOTION_BONE_N_EYELID_L_A;
        i <= MOTION_BONE_TL_EYELID_R_B_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));
}

static void sub_140413EA0(struc_240* a1, void(*a2)(struc_313*)) {
    a2(&a1->field_8);
}

static void sub_14041AD90(rob_chara_bone_data* a1) {
    if (a1->mouth.mot_key_data.key_sets_ready && a1->mouth.mot_key_data.mot_data && !a1->mouth.field_30)
        sub_140413EA0(&a1->face.field_0, sub_14041B800);
    if (a1->eyelid.mot_key_data.key_sets_ready && a1->eyelid.mot_key_data.mot_data && !a1->eyelid.field_30)
        sub_140413EA0(&a1->face.field_0, sub_14041B1C0);
    else if (a1->eye.mot_key_data.key_sets_ready && a1->eye.mot_key_data.mot_data && !a1->eye.field_30 || a1->field_758)
        sub_140413EA0(&a1->face.field_0, sub_14041B440);
}

static void sub_14041AD50(rob_chara_bone_data* a1) {
    if (a1->eye.mot_key_data.key_sets_ready && a1->eye.mot_key_data.mot_data && !a1->eye.field_30 || a1->field_758)
        sub_140413EA0(&a1->eyelid.field_0, sub_14041B440);
}

static void sub_140412F20(mot_blend* a1, vector_old_bone_data* a2) {
    uint32_t* bitfield = a1->field_0.field_8.bitfield.begin;
    for (bone_data* i = a2->begin; i != a2->end; ++i) {
        motion_bone_index v5 = i->motion_bone_index;
        if (bitfield[v5 >> 5] & (1 << (v5 & 0x1F)))
            sub_1401EB1D0(i, 0);
    }
}

static mot_data* mot_key_data_load_file(mot_key_data* a1, int32_t motion_id) {
    mot_data* (*sub_140403C90)(int32_t a1)
        = (mot_data * (*)(int32_t))0x0000000140403C90;
    bool(*mot_load_file)(mot * a1, mot_key_set_file * a2)
        = (bool(*)(mot*, mot_key_set_file*))0x000000014036F7B0;

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
    mot_key_data_load_file(&a1->mot_key_data, motion_id);
    a1->blend.base.__vftable->field_8(&a1->blend.base);
}

static void sub_14041BE50(rob_chara_bone_data* rob_bone_data, int32_t motion_id) {
    sub_14041AD90(rob_bone_data);
    mot_blend* v3 = &rob_bone_data->face;
    sub_140412F20(v3, &rob_bone_data->motion_loaded.head->next->value->bone_data.bones);
    sub_1404146F0(&v3->field_0);
    sub_140414ED0(v3, motion_id);
}

static void sub_14041ABA0(rob_chara_bone_data* a1) {
    list_ptr_motion_blend_mot_node* v1 = a1->motion_loaded.head;
    list_ptr_motion_blend_mot_node* v3 = v1->next;
    while (v3 != v1) {
        if (a1->face.mot_key_data.key_sets_ready
            && a1->face.mot_key_data.mot_data && !a1->face.field_30) {
            if (!a1->field_758 || a1->eye.mot_key_data.key_sets_ready
                && a1->eye.mot_key_data.mot_data && !a1->eye.field_30)
                sub_140413EA0(&v3->value->field_0, sub_14041B4E0);
            else
                sub_140413EA0(&v3->value->field_0, sub_14041B580);
        }
        else {
            if (a1->mouth.mot_key_data.key_sets_ready
                && a1->mouth.mot_key_data.mot_data && !a1->mouth.field_30)
                sub_140413EA0(&v3->value->field_0, sub_14041B800);
            if (a1->eyelid.mot_key_data.key_sets_ready
                && a1->eyelid.mot_key_data.mot_data && !a1->eyelid.field_30) {
                if (!a1->field_758
                    || a1->eye.mot_key_data.key_sets_ready
                    && a1->eye.mot_key_data.mot_data && !a1->eye.field_30)
                    sub_140413EA0(&v3->value->field_0, sub_14041B1C0);
                else
                    sub_140413EA0(&v3->value->field_0, sub_14041B300);
            }
            else if (a1->eye.mot_key_data.key_sets_ready
                && a1->eye.mot_key_data.mot_data && !a1->eye.field_30)
                sub_140413EA0(&v3->value->field_0, sub_14041B440);
        }

        if (a1->hand_l.mot_key_data.key_sets_ready
            && a1->hand_l.mot_key_data.mot_data && !a1->hand_l.field_30)
            sub_140413EA0(&v3->value->field_0, sub_14041B6B0);
        if (a1->hand_r.mot_key_data.key_sets_ready
            && a1->hand_r.mot_key_data.mot_data && !a1->hand_r.field_30)
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
    motion_blend* v1 = a1->blend;
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
    v7.field_1 = sub_1404137A0(&a1->field_4F8);
    v7.frame = a1->mot_key_data.frame;
    v7.eyes_adjust = a1->field_4F8.eyes_adjust;
    v7.prev_eyes_adjust = a1->field_4F8.prev_eyes_adjust;
    v1->__vftable->field_18(v1, &v7);
}

static bool sub_140410A20(motion_blend_mot* a1) {
    motion_blend* v1 = a1->blend;
    return !v1 || !v1->field_9;
}

static void sub_140415B30(mot_blend* a1) {
    struc_400 v3;
    v3.field_0 = false;
    v3.field_1 = false;
    v3.field_2 = false;
    v3.field_3 = false;
    v3.field_4 = false;
    v3.frame = 0.0f;
    v3.eyes_adjust = 0.0f;
    v3.prev_eyes_adjust = 0.0f;
    a1->blend.base.__vftable->field_18(&a1->blend.base, &v3);
}

static void sub_14041DAC0(rob_chara_bone_data* a1) {
    void(*rob_chara_bone_data_motion_blend_mot_list_free)(rob_chara_bone_data * rob_bone_data, size_t a2)
        = (void(*)(rob_chara_bone_data*, size_t))0x0000000140418E80;

    size_t v2 = 0;
    bool v3 = 0;
    list_ptr_motion_blend_mot_node* v1 = a1->motion_loaded.head;
    list_ptr_motion_blend_mot_node* v4 = v1->next;
    while (v4 != v1) {
        if (sub_1404136B0(v4->value))
            sub_140415A10(v4->value);
        else {
            if (v3) {
                rob_chara_bone_data_motion_blend_mot_list_free(a1, v2);
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

void rob_chara_bone_data_interpolate(rob_chara_bone_data* rob_bone_data) {
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
    vector_old_bone_data* bones = &v5->bone_data.bones;
    vector_old_uint16_t* bone_indices = &v5->bone_data.bone_indices;
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
        if (~a1->field_0.field_8.bitfield.begin[v5 >> 5] & (1 << (v5 & 0x1F)))
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

static void sub_140412BB0(motion_blend_mot* a1, vector_old_bone_data* a2) {
    for (bone_data* i = a1->bone_data.bones.begin; i != a1->bone_data.bones.end; i++)
        sub_1401EAD00(i, &a2->begin[i->motion_bone_index]);
}

static void sub_1403FBF10(motion_blend* a1, float_t a2) {
    a1->blend = clamp(a2, 0.0f, 1.0f);
    a1->field_8 = 1;
    a1->field_9 = 1;
}

static void sub_1404148C0(motion_blend_mot* a1, float_t a2) {
    motion_blend* v2 = a1->blend;
    if (v2)
        sub_1403FBF10(v2, a2);
}

static void sub_1404147F0(motion_blend_mot* a1, motion_blend_type type, float_t blend) {
    if (type == MOTION_BLEND_FREEZE) {
        a1->freeze.base.__vftable->field_8(&a1->freeze.base);
        a1->blend = &a1->freeze.base;
        sub_140412E10(a1, 0);
    }
    else if (type == MOTION_BLEND_CROSS) {
        a1->cross.base.__vftable->field_8(&a1->cross.base);
        a1->blend = &a1->cross.base;
    }
    else if (type == MOTION_BLEND_COMBINE) {
        a1->combine.base.base.__vftable->field_8(&a1->combine.base.base);
        a1->blend = &a1->combine.base.base;
        sub_1404148C0(a1, blend);
    }
    else
        a1->blend = 0;
}

static int32_t sub_1401F0E70(aft_bone_database_skeleton_type type, char* name) {
    aft_bone_database_struct* bone_database
        = *(aft_bone_database_struct**)0x0000000140FBC1C0;
    int32_t(*sub_1401F0EA0)(char* a1, char* a2)
        = (int32_t(*)(char*, char*))0x00000001401F0EA0;

    if (!bone_database || type == AFT_BONE_DATABASE_SKELETON_NONE)
        return -1;
    else
        return sub_1401F0EA0(bone_database->skeleton_names[type], name);
}

static void bone_data_parent_load_bone_indices_from_mot(bone_data_parent* a1, mot_data* a2) {
    if (!a2)
        return;

    char** (*sub_140403B00)()
        = (char** (*)())0x0000000140403B00;
    void (*sub_14040F210)(vector_old_uint16_t * a1, size_t a2)
        = (void (*)(vector_old_uint16_t*, size_t))0x000000014040F210;

    char** v4 = sub_140403B00();
    vector_old_uint16_t* v5 = &a1->bone_indices;
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

static void motion_blend_mot_load_file(motion_blend_mot* a1,
    int32_t motion_id, motion_blend_type a3, float_t blend) {
    sub_1404147F0(a1, a3, blend);
    mot_data* v5 = mot_key_data_load_file(&a1->mot_key_data, motion_id);
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
        rob_chara_bone_data* rob_bone_data = v6->rob_bone_data;
        for (mat4* i = rob_bone_data->mats.begin; i != rob_bone_data->mats.end; i++)
            *i = mat4_identity;

        for (mat4* i = rob_bone_data->mats2.begin; i != rob_bone_data->mats2.end; i++)
            *i = mat4_identity;

        a1->bone_data.bone_indices.end = a1->bone_data.bone_indices.begin;
    }
    sub_140413C30(&a1->field_4F8);
}

static void sub_14041BA60(rob_chara_bone_data* rob_bone_data) {
    motion_blend_mot* v1 = rob_bone_data->motion_loaded.head->next->value;
    if (v1)
        sub_1404146F0(&v1->field_0);
}

static void sub_14041AE40(rob_chara_bone_data* a1) {
    list_ptr_motion_blend_mot_node* v1 = a1->motion_loaded.head;
    struc_240* v3 = &v1->next->value->field_0;
    if (!v3)
        return;

    if (a1->face.mot_key_data.key_sets_ready
        && a1->face.mot_key_data.mot_data && !a1->face.field_30) {
        if (!a1->field_758 || a1->eye.mot_key_data.key_sets_ready
            && a1->eye.mot_key_data.mot_data && !a1->eye.field_30)
            sub_140413EA0(v3, sub_14041B4E0);
        else
            sub_140413EA0(v3, sub_14041B580);
    }
    else {
        if (a1->mouth.mot_key_data.key_sets_ready
            && a1->mouth.mot_key_data.mot_data && !a1->mouth.field_30)
            sub_140413EA0(&v1->next->value->field_0, sub_14041B800);
        else if (a1->eyelid.mot_key_data.key_sets_ready
            && a1->eyelid.mot_key_data.mot_data && !a1->eyelid.field_30) {
            if (!a1->field_758 || a1->eye.mot_key_data.key_sets_ready
                && a1->eye.mot_key_data.mot_data && !a1->eye.field_30)
                sub_140413EA0(v3, sub_14041B1C0);
            else
                sub_140413EA0(v3, sub_14041B300);
        }
        else if (a1->eye.mot_key_data.key_sets_ready
            && a1->eye.mot_key_data.mot_data && !a1->eye.field_30)
            sub_140413EA0(v3, sub_14041B440);
    }

    if (a1->hand_l.mot_key_data.key_sets_ready
        && a1->hand_l.mot_key_data.mot_data && !a1->hand_l.field_30)
        sub_140413EA0(v3, sub_14041B6B0);
    if (a1->hand_r.mot_key_data.key_sets_ready
        && a1->hand_r.mot_key_data.mot_data && !a1->hand_r.field_30)
        sub_140413EA0(v3, sub_14041B750);
}

void rob_chara_bone_data_motion_load(rob_chara_bone_data* rob_bone_data,
    int32_t motion_id, motion_blend_type motion_blend_type) {
    void(*rob_chara_bone_data_motion_blend_mot_list_init)(rob_chara_bone_data * rob_bone_data)
        = (void(*)(rob_chara_bone_data*))0x000000014041A9E0;
    void(*rob_chara_bone_data_motion_blend_mot_list_free)(rob_chara_bone_data * rob_bone_data, size_t a2)
        = (void(*)(rob_chara_bone_data*, size_t))0x0000000140418E80;
    list_ptr_motion_blend_mot_node* (*sub_140415D30)(list_ptr_motion_blend_mot * a1,
        list_ptr_motion_blend_mot_node * a2, list_ptr_motion_blend_mot_node * a3, motion_blend_mot * *a4)
        = (list_ptr_motion_blend_mot_node * (*)(list_ptr_motion_blend_mot*,
            list_ptr_motion_blend_mot_node*, list_ptr_motion_blend_mot_node*,
            motion_blend_mot**))0x0000000140415D30;
    list_size_t_node* (*sub_140415DB0)(list_size_t_node * *a1, list_size_t_node * a2,
        list_size_t_node * a3, size_t * a4) = (list_size_t_node * (*)(list_size_t_node**,
            list_size_t_node*, list_size_t_node*, size_t*))0x0000000140415DB0;
    motion_blend_mot* (*sub_140411A70)(motion_blend_mot * a1, rob_chara_bone_data * rob_bone_data)
        = (motion_blend_mot * (*)(motion_blend_mot*, rob_chara_bone_data*))0x0000000140411A70;

    if (!rob_bone_data->motion_loaded.size)
        return;
    if (motion_blend_type == 1) {
        rob_chara_bone_data_motion_blend_mot_list_free(rob_bone_data, 1);
        sub_14041AE40(rob_bone_data);
        list_ptr_motion_blend_mot_node*v15 = rob_bone_data->motion_loaded.head;
        motion_blend_mot_load_file(v15->next->value, motion_id, MOTION_BLEND_FREEZE, 1.0f);
        sub_14041BA60(rob_bone_data);
        return;
    }

    if (motion_blend_type != MOTION_BLEND_CROSS) {
        if (motion_blend_type == MOTION_BLEND_COMBINE)
            motion_blend_type = MOTION_BLEND;
        rob_chara_bone_data_motion_blend_mot_list_free(rob_bone_data, 1);
        motion_blend_mot_load_file(rob_bone_data->motion_loaded.head->next->value, motion_id, motion_blend_type, 1.0f);
        return;
    }

    rob_chara_bone_data_motion_blend_mot_list_free(rob_bone_data, 2);
    if (rob_bone_data->motion_indices.size) {
        motion_blend_mot* v6 = rob_bone_data->motion_loaded.head->next->value;
        rob_chara_bone_data_motion_blend_mot_list_init(rob_bone_data);
        sub_140412BB0(rob_bone_data->motion_loaded.head->next->value, &v6->bone_data.bones);
        motion_blend_mot_load_file(rob_bone_data->motion_loaded.head->next->value, motion_id, MOTION_BLEND_CROSS, 1.0f);
        return;
    }

    list_ptr_motion_blend_mot* v7 = &rob_bone_data->motion_loaded;
    motion_blend_mot* v16 = sub_140411A70(rob_bone_data->motion_loaded.head->next->value, rob_bone_data);
    if (!v16) {
        rob_chara_bone_data_motion_blend_mot_list_free(rob_bone_data, 1);
        sub_14041AE40(rob_bone_data);
        list_ptr_motion_blend_mot_node* v15 = v7->head;
        motion_blend_mot_load_file(v15->next->value, motion_id, MOTION_BLEND_FREEZE, 1.0f);
        sub_14041BA60(rob_bone_data);
        return;
    }

    list_ptr_motion_blend_mot_node* v8 = v7->head->next;
    motion_blend_mot* v9 = v8->value;
    list_ptr_motion_blend_mot_node* v10 = sub_140415D30(&rob_bone_data->motion_loaded, v8, v8->prev, &v16);
    rob_bone_data->motion_loaded.size++;
    v8->prev = v10;
    v10->prev->next = v10;
    size_t v17 = vector_old_length(rob_bone_data->motions);
    list_size_t_node* v12 = rob_bone_data->motion_loaded_indices.head->next;
    list_size_t_node* v13 = sub_140415DB0(&rob_bone_data->motion_loaded_indices.head, v12, v12->prev, &v17);
    rob_bone_data->motion_loaded_indices.size++;
    v12->prev = v13;
    v13->prev->next = v13;
    sub_140412BB0(v7->head->next->value, &v9->bone_data.bones);
    motion_blend_mot_load_file(v7->head->next->value, motion_id, MOTION_BLEND_CROSS, 1.0f);
}

static void sub_14041BFC0(rob_chara_bone_data* rob_bone_data, int32_t motion_id) {
    sub_140412F20(&rob_bone_data->hand_l, &rob_bone_data->motion_loaded.head->next->value->bone_data.bones);
    sub_140414ED0(&rob_bone_data->hand_l, motion_id);
}

static void sub_14041C260(rob_chara_bone_data* rob_bone_data, int32_t motion_id) {
    sub_140412F20(&rob_bone_data->hand_r, &rob_bone_data->motion_loaded.head->next->value->bone_data.bones);
    sub_140414ED0(&rob_bone_data->hand_r, motion_id);
}

static void sub_14041D200(rob_chara_bone_data* rob_bone_data, int32_t motion_id) {
    sub_140412F20(&rob_bone_data->mouth, &rob_bone_data->motion_loaded.head->next->value->bone_data.bones);
    sub_140414ED0(&rob_bone_data->mouth, motion_id);
}

static void sub_14041BDB0(rob_chara_bone_data* rob_bone_data, int32_t motion_id) {
    sub_140412F20(&rob_bone_data->eye, &rob_bone_data->motion_loaded.head->next->value->bone_data.bones);
    sub_140414ED0(&rob_bone_data->eye, motion_id);
}

static void sub_14041BD20(rob_chara_bone_data* rob_bone_data, int32_t motion_id) {
    sub_14041AD50(rob_bone_data);
    mot_blend* v3 = &rob_bone_data->eyelid;
    sub_140412F20(v3, &rob_bone_data->motion_loaded.head->next->value->bone_data.bones);
    sub_1404146F0(&v3->field_0);
    sub_140414ED0(v3, motion_id);
}

static void sub_140414BC0(mot_play_frame_data* a1, float_t frame) {
    float_t v2 = a1->field_14;
    a1->frame = frame;
    a1->field_28 = 0;
    if (v2 >= 0.0f && (a1->field_18 == 2 && frame <= v2) || frame >= v2)
        a1->field_14 = -1.0;
}

static void rob_chara_bone_data_set_frame(rob_chara_bone_data* a1, float_t frame) {
    sub_140414BC0(&a1->motion_loaded.head->next->value->mot_play_data.frame_data, frame);
}

static void sub_14041C680(rob_chara_bone_data* a1, char a2) {
    struc_308* v2 = &a1->motion_loaded.head->next->value->field_4F8;
    if (a2)
        v2->field_0 |= 2;
    else
        v2->field_0 &= ~2;
}

static void sub_14041C9D0(rob_chara_bone_data* a1, char a2) {
    struc_308* v2 = &a1->motion_loaded.head->next->value->field_4F8;
    v2->field_8 = (uint8_t)(v2->field_0 & 1);
    if (a2)
        v2->field_0 |= 1;
    else
        v2->field_0 &= ~1;
}

static void sub_14041D2D0(rob_chara_bone_data* a1, char a2) {
    struc_308* v2 = &a1->motion_loaded.head->next->value->field_4F8;
    if (a2)
        v2->field_0 |= 4;
    else
        v2->field_0 &= ~4;
}

static void sub_14041BC40(rob_chara_bone_data* a1, char a2) {
    struc_308* v2 = &a1->motion_loaded.head->next->value->field_4F8;
    if (a2)
        v2->field_0 |= 8;
    else
        v2->field_0 &= ~8;
}

static void sub_140414F00(struc_308* a1, float_t a2) {
    a1->prev_eyes_adjust = a1->eyes_adjust;
    a1->eyes_adjust = a2;
}

static void sub_14041D270(rob_chara_bone_data* a1, float_t a2) {
    sub_140414F00(&a1->motion_loaded.head->next->value->field_4F8, a2);
}

static void sub_14041D2A0(rob_chara_bone_data* a1, float_t a2) {
    a1->motion_loaded.head->next->value->field_4F8.prev_eyes_adjust = a2;
}

static void rob_chara_bone_data_set_rotation_y(rob_chara_bone_data* rob_bone_data, float_t rotation_y) {
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
    motion_blend* v4 = a1->blend;
    if (v4)
        sub_1403FBED0(v4, a2, a3, a4);
}

static void sub_14041BF80(rob_chara_bone_data* a1, float_t a2, float_t a3, float_t a4) {
    sub_140414C60(a1->motion_loaded.head->next->value, a2, a3, a4);
}

static void sub_14041D310(rob_chara_bone_data* a1) {
    motion_blend* v1 = a1->motion_loaded.head->next->value->blend;
    if (v1)
        v1->__vftable->field_10(v1);
}

static void rob_chara_load_default_motion_sub(rob_chara* a1, int32_t a2, int32_t motion_id) {
    sub_14041BE50(a1->bone_data, -1);
    sub_14041BFC0(a1->bone_data, -1);
    sub_14041C260(a1->bone_data, -1);
    sub_14041D200(a1->bone_data, -1);
    sub_14041BDB0(a1->bone_data, -1);
    sub_14041BD20(a1->bone_data, -1);
    rob_chara_bone_data_motion_load(a1->bone_data, motion_id, MOTION_BLEND_FREEZE);
    rob_chara_bone_data_set_frame(a1->bone_data, 0.0f);
    sub_14041C680(a1->bone_data, 0);
    sub_14041C9D0(a1->bone_data, 0);
    sub_14041D2D0(a1->bone_data, 0);
    sub_14041BC40(a1->bone_data, 0);
    sub_14041D270(a1->bone_data, 0.0f);
    sub_14041D2A0(a1->bone_data, 0.0f);
    rob_chara_bone_data_set_rotation_y(a1->bone_data, 0.0f);
    sub_14041BF80(a1->bone_data, 0.0f, 1.0f, 1.0f);
    sub_14041D310(a1->bone_data);
    rob_chara_bone_data_interpolate(a1->bone_data);
    rob_chara_bone_data_update(a1->bone_data, 0);
    sub_140412E10(a1->bone_data->motion_loaded.head->next->value, a2);
}

int32_t rob_cmn_mottbl_get_motion_index(rob_chara* a1, int32_t a2) {
    struc_403* rob_cmn_mottbl_data = *rob_cmn_mottbl_data_ptr;

    if (a2 == 224)
        return a1->data.adjust.motion_id;
    if (a2 >= 214 && a2 <= 223)
        return ((uint32_t*)&a1->data_prev.field_1E68.field_1C88)[a2];
    if (a2 >= 226 && a2 <= 235)
        return ((uint32_t*)&a1->data_prev.field_1E68.field_1C78)[a2 + 1];

    size_t v7 = a1->data.field_8.field_1A4;
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

static void rob_chara_load_default_motion(rob_chara* a1) {
    rob_chara_load_default_motion_sub(a1, 1, rob_cmn_mottbl_get_motion_index(a1, 0));
}

static bone_node* rob_chara_bone_data_get_node(rob_chara_bone_data* rob_bone_data, size_t index) {
    if ((ssize_t)index < vector_old_length(rob_bone_data->nodes))
        return &rob_bone_data->nodes.begin[index];
    return 0;
}

static char* ex_node_block_get_parent_name(ex_node_block* a1) {
    return string_data(&a1->parent_name);
}

static bone_node* rob_chara_item_equip_object_get_bone_node(
    rob_chara_item_equip_object* a1, int32_t a2) {
    bool ex_data = a2 & 0x8000 ? true : false;
    a2 &= 0x7FFF;
    if (!ex_data)
        return &a1->bone_nodes[a2];
    else if (a2 < vector_old_length(a1->ex_data_bone_nodes))
        return &a1->ex_data_bone_nodes.begin[a2];
    return 0;
}

static bone_node* rob_chara_item_equip_object_get_bone_node_by_name(
    rob_chara_item_equip_object* a1, char* a2) {
    int32_t(*rob_chara_item_equip_object_get_bone_index)(rob_chara_item_equip_object * a1, char* a2)
        = (int32_t(*)(rob_chara_item_equip_object * , char* ))0x00000001405F3400;
    return rob_chara_item_equip_object_get_bone_node(a1,
        rob_chara_item_equip_object_get_bone_index(a1, a2));
}

static void rob_chara_item_equip_object_get_parent_bone_nodes(
    rob_chara_item_equip_object* a1, bone_node* a2) {
    a1->bone_nodes = a2;
    a1->mat = a2->mat;
    for (ex_node_block** i = a1->node_blocks.begin; i != a1->node_blocks.end; i++)
        (*i)->parent_bone_node = rob_chara_item_equip_object_get_bone_node_by_name(
            a1, ex_node_block_get_parent_name(*i));
}

static void rob_chara_item_equip_get_parent_bone_nodes(rob_chara_item_equip* a1, bone_node* a2) {
    a1->bone_nodes = a2;
    a1->matrices = a2->mat;
    for (int32_t v2 = a1->first_item_equip_object; v2 < a1->max_item_equip_object; v2++)
        rob_chara_item_equip_object_get_parent_bone_nodes(&a1->item_equip_object[v2], a1->bone_nodes);
}

static bool rob_chara_check_for_ageageagain_module(chara_index chara_index, int32_t module_index) {
    return chara_index == CHARA_MIKU && module_index == 148;
}

static void sub_140517060(rob_chara* a1, bool a2) {
    void(*sub_140543780)(int32_t a1, int32_t a2, char a3)
        = (void(*)(int32_t, int32_t, char))0x0000000140543780;

    a1->data.field_0 &= ~1;
    a1->data.field_3 &= ~1;
    a1->data.field_0 |= a2 & 1;
    a1->data.field_3 |= a2 & 1;
    if (rob_chara_check_for_ageageagain_module(a1->chara_index, a1->module_index)) {
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

static float_t chara_pos_adjust_y_table_get_value(uint32_t a1) {
    static const float_t chara_pos_adjust_y_table[] = {
        0.071732f, 0.0f, -0.03859f, 0.0f, 0.0f
    };

    if (a1 > 4)
        return 0.0f;
    else
        return chara_pos_adjust_y_table[a1];
}

static void sub_14041C5C0(rob_chara_bone_data* a1, struc_243* a2) {
    a1->field_788.field_15C = *a2;
}

static void sub_14041D4E0(rob_chara_bone_data* a1, struc_242* a2) {
    a1->field_958.field_0 = *a2;
}

static void sub_14041D5C0(rob_chara_bone_data* a1, struc_242* a2) {
    a1->field_958.field_2C = *a2;
}

static void sub_14041D560(rob_chara_bone_data* a1, char a2, char a3) {
    a1->field_958.field_58 = a2;
    a1->field_958.field_59 = a3;
}

static void rob_chara_bone_data_ik_scale_calculate(
    rob_chara_bone_data_ik_scale* a1, vector_old_bone_data* a2,
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

static void rob_chara_bone_data_get_ik_scale(rob_chara_bone_data* a1) {
    if (!a1->motion_loaded.size)
        return;

    motion_blend_mot* v2 = a1->motion_loaded.head->next->value;
    rob_chara_bone_data_ik_scale_calculate(&a1->ik_scale, &v2->bone_data.bones, a1->base_skeleton_type, a1->skeleton_type);
    float_t ratio0 = a1->ik_scale.ratio0;
    v2->field_4F8.field_C0 = ratio0;
    v2->field_4F8.field_C4 = ratio0;
    v2->field_4F8.field_C8 = vec3_null;
}

static float_t rob_chara_bone_data_get_ik_scale_ratio0(rob_chara_bone_data* a1) {
    return a1->ik_scale.ratio0;
}

static void sub_1405167A0(rob_chara* a1) {
    a1->field_C = 1;
}

static void sub_140513C60(rob_chara_item_equip* a1, int32_t a2, bool a3) {
    if (a3)
        a1->field_18[a2] &= ~4;
    else
        a1->field_18[a2] |= 4;
}

static aft_object_info sub_140525C00(rob_chara_item_sub_data* a1, int32_t a2) {
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
        return aft_object_info_null;
    else
        return *(aft_object_info*)&v6->value;
}

static aft_object_info sub_1405104C0(rob_chara* a1, int32_t a2) {
    if (a2 > 8)
        return aft_object_info_null;

    aft_object_info v3 = sub_140525C00(&a1->item_sub_data, a2);
    if (v3.id == -1 && v3.set_id == -1)
        return a1->chara_init_data->head_objects[a2];
    return v3;
}

static void sub_1405139E0(rob_chara_item_equip* a1, uint32_t a2, bool disp) {
    a1->item_equip_object[a2].disp = disp;
}

static void sub_140517120(rob_chara* a1, uint32_t a2, bool disp) {
    if (a2 < 31)
        sub_1405139E0(a1->item_equip, a2, disp);
    else if (a2 == 31) {
        void(*sub_140543780)(int32_t a1, int32_t a2, char a3)
            = (void(*)(int32_t, int32_t, char))0x0000000140543780;
        void(*sub_1405430F0)(int32_t a1, int32_t a2)
            = (void(*)(int32_t, int32_t))0x00000001405430F0;

        for (int32_t i = 1; i < 31; i++) {
            sub_1405139E0(a1->item_equip, i, disp);
            if (i == 1 && rob_chara_check_for_ageageagain_module(
                a1->chara_index, a1->module_index)) {
                sub_140543780(a1->chara_id, 1, disp);
                sub_140543780(a1->chara_id, 2, disp);
                sub_1405430F0(a1->chara_id, 1);
                sub_1405430F0(a1->chara_id, 2);
            }
        }
    }
}

void rob_chara_reset_data(rob_chara* a1, rob_chara_pv_data* a2) {
    float_t(*sub_140228BD0)(vec3 * a1, float_t a2)
        = (float_t(*)(vec3*, float_t))0x0000000140228BD0;
    void(*rob_chara_data_reset)(rob_chara_data * a1)
        = (void(*)(rob_chara_data*))0x0000000140505EA0;
    struc_243* (*sub_140510550)(int32_t a1) = (struc_243 * (*)(int32_t))0x0000000140510550;
    struc_241* (*sub_1405106E0)(int32_t a1) = (struc_241 * (*)(int32_t))0x00000001405106E0;
    void(*rob_chara_load_motion)(rob_chara * a1, int32_t motion_id, bool a3, float_t a4, int32_t a5)
        = (void(*)(rob_chara*, int32_t, bool, float_t, int32_t))0x00000001405552A0;

    int32_t v2 = a1->data.field_8.field_1CC;
    rob_chara_bone_data_reset(a1->bone_data);
    rob_chara_bone_data_init_data(a1->bone_data,
        AFT_BONE_DATABASE_SKELETON_COMMON, a1->chara_init_data->skeleton_type);
    rob_chara_data_reset(&a1->data);
    rob_chara_data_reset(&a1->data_prev);
    rob_chara_item_equip_get_parent_bone_nodes(a1->item_equip,
        rob_chara_bone_data_get_node(a1->bone_data, 0));
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

    rob_chara_data* v3 = &a1->data;
    v3->miku_rot.field_0 = a2->field_14;
    v3->miku_rot.field_6 = a2->field_14;
    v3->miku_rot.field_18 = v8;
    v3->miku_rot.field_24 = v8;
    v3->miku_rot.field_48 = v8;
    v3->adjust_data.scale = chara_size_table_get_value(a1->pv_data.chara_size_index);
    v3->adjust_data.height_adjust = a1->pv_data.height_adjust;
    v3->adjust_data.pos_adjust_y = chara_pos_adjust_y_table_get_value(a1->pv_data.chara_size_index);
    v3->field_8.field_1A8 = a1->chara_init_data->field_838[v3->field_8.field_1A4];
    sub_14041C5C0(a1->bone_data, sub_140510550(a1->chara_index));
    rob_chara_bone_data_eyes_xrot_adjust(a1->bone_data,
        sub_1405106E0(a1->chara_index), &a2->eyes_adjust);
    sub_14041D4E0(a1->bone_data, &a2->field_18);
    sub_14041D5C0(a1->bone_data, &a2->field_44);
    sub_14041D560(a1->bone_data, 1, 0);
    rob_chara_bone_data_get_ik_scale(a1->bone_data);
    float_t ratio0 = rob_chara_bone_data_get_ik_scale_ratio0(a1->bone_data);
    v3->adjust.field_138 = ratio0;
    v3->adjust.field_13C = ratio0;
    v3->adjust.field_140 = 0;
    v3->adjust.field_144 = 0;
    v3->adjust.field_148 = 0;
    rob_chara_load_default_motion(a1);
    v3->field_8.field_0 = 0;
    v3->field_8.field_4.field_0 = 1;
    v3->field_8.field_4.field_C = rob_cmn_mottbl_get_motion_index(a1, 0);
    v3->field_8.field_4.field_10 = a2->field_5;
    rob_chara_load_motion(a1, v3->field_8.field_4.field_C, v3->field_8.field_4.field_10, 0.0, 0);
    if (!a2->field_6)
        v3->field_8.field_1CC = v2;
    sub_1405167A0(a1);
    a1->item_equip->field_DC = 0;
    sub_140513C60(a1->item_equip, 1, false);
    sub_140513C60(a1->item_equip, 10, false);
    sub_140513C60(a1->item_equip, 11, false);
    aft_object_info v17 = sub_1405104C0(a1, 0);
    v3->adjust.field_150.field_1B0 = v17;
    v3->adjust.field_3B0.field_1B0 = v17;
    sub_140517120(a1, 0x1F, true);
    a1->field_20 = 0;
}

static void skin_param_hinge_limit(skin_param_hinge* hinge) {
    hinge->ymin = max(hinge->ymin, -179.0f) * DEG_TO_RAD_FLOAT;
    hinge->ymax = min(hinge->ymax, 179.0f) * DEG_TO_RAD_FLOAT;
    hinge->zmin = max(hinge->zmin, -179.0f) * DEG_TO_RAD_FLOAT;
    hinge->zmax = min(hinge->zmax, 179.0f) * DEG_TO_RAD_FLOAT;
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
    node_data->normal_ref.mat = mat4u_identity;
}

static void rob_osage_node_init(rob_osage_node* node) {
    void (*sub_14021DCC0)(vector_old_opd_vec3_data * vec, size_t size)
        = (void (*)(vector_old_opd_vec3_data*, size_t))0x000000014021DCC0;
    node->length = 0.0f;
    node->trans = vec3_null;
    node->trans_orig = vec3_null;
    node->trans_diff = vec3_null;
    node->field_28 = vec3_null;
    node->child_length = 0.0f;
    node->bone_node_ptr = 0;
    node->bone_node_mat = 0;
    node->sibling_node = 0;
    node->distance = 0.0f;
    node->field_94 = vec3_null;
    node->reset_data.trans = vec3_null;
    node->reset_data.trans_diff = vec3_null;
    node->reset_data.rotation = vec3_null;
    node->reset_data.length = 0.0f;
    node->field_C8 = 0.0f;
    node->field_CC = 1.0f;
    node->external_force = vec3_null;
    node->force = 1.0f;
    rob_osage_node_data_init(&node->data);
    node->data_ptr = &node->data;
    node->opd_data.end = node->opd_data.begin;
    sub_14021DCC0(&node->opd_data, 3);
    node->mat = mat4u_null;
}

static void skin_param_init(skin_param* skp) {

}

static void rob_osage_init(rob_osage* rob_osg) {
    void (*sub_140216750)(tree_pair_pair_int32_t_int32_t_list_rob_osage_node_reset_data * a1,
        tree_pair_pair_int32_t_int32_t_list_rob_osage_node_reset_data_node * a2)
        = (void (*)(tree_pair_pair_int32_t_int32_t_list_rob_osage_node_reset_data*,
            tree_pair_pair_int32_t_int32_t_list_rob_osage_node_reset_data_node*))0x0000000140216750;
    void (*vector_old_rob_osage_node_clear)(vector_old_rob_osage_node * vec,
        rob_osage_node * begin, rob_osage_node * end)
        = (void (*)(vector_old_rob_osage_node*, rob_osage_node*, rob_osage_node*))0x00000001404817E0;

    vector_old_rob_osage_node_clear(&rob_osg->nodes, rob_osg->nodes.begin, rob_osg->nodes.end);
    rob_osg->nodes.end = rob_osg->nodes.begin;
    rob_osage_node_init(&rob_osg->node);
    rob_osg->wind_direction = vec3_null;
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
    rob_osg->osage_setting.parts = -1;
    rob_osg->osage_setting.exf = 0;
    rob_osg->reset_data_list = 0;
    rob_osg->field_2A4 = 0.0f;
    rob_osg->field_2A1 = 0;
    rob_osg->set_external_force = false;
    rob_osg->external_force = vec3_null;
    rob_osg->parent_mat_ptr = 0;
    rob_osg->parent_mat = mat4u_null;
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
    void (*sub_14021BEF0)(tree_pair_pair_int32_t_int32_t_list_rob_osage_node_reset_data * a1,
        tree_pair_pair_int32_t_int32_t_list_rob_osage_node_reset_data_node * *a2,
        tree_pair_pair_int32_t_int32_t_list_rob_osage_node_reset_data_node * a3,
        tree_pair_pair_int32_t_int32_t_list_rob_osage_node_reset_data_node * a4)
        = (void (*)(tree_pair_pair_int32_t_int32_t_list_rob_osage_node_reset_data*,
            tree_pair_pair_int32_t_int32_t_list_rob_osage_node_reset_data_node**,
            tree_pair_pair_int32_t_int32_t_list_rob_osage_node_reset_data_node*,
            tree_pair_pair_int32_t_int32_t_list_rob_osage_node_reset_data_node*))0x000000014021BEF0;
    void (*vector_old_rob_osage_node_clear)(vector_old_rob_osage_node * vec,
        rob_osage_node * begin, rob_osage_node * end)
        = (void (*)(vector_old_rob_osage_node*,
            rob_osage_node*, rob_osage_node*))0x00000001404817E0;

    rob_osage_init(rob_osg);

    tree_pair_pair_int32_t_int32_t_list_rob_osage_node_reset_data_node* v6;
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

void ex_node_block_field_10(ex_node_block* node) {
    node->field_59 = false;
}

void* ex_osage_block_dispose(ex_osage_block* osg, bool dispose) {
    osg->base.__vftable = ex_osage_block_vftable;
    ex_osage_block_reset(osg);
    rob_osage_free(&osg->rob);
    osg->base.__vftable = ex_node_block_vftable;
    if (osg->base.parent_name.capacity > 0x0F)
        operator_delete(osg->base.parent_name.ptr);
    osg->base.parent_name = string_empty;
    if (dispose)
        operator_delete(osg);
    return osg;
}

void ex_osage_block_init(ex_osage_block* osg) {
    osg->base.__vftable->reset(&osg->base);
}

static void sub_14047EE90(rob_osage* rob_osg, mat4* mat) {
    if (rob_osg->reset_data_list) {
        list_rob_osage_node_reset_data_node* v5 = rob_osg->reset_data_list->head->next;
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
        osg_block_data->wind_direction = vec3_null;
}

static void ex_osage_block_set_wind_direction(ex_osage_block* osg) {
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
    node->trans_diff = vec3_null;
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

static int32_t sub_140485220(vec3* a1, skin_param_osage_node* a2, osage_coli* coli, float_t* a4){
    if (!coli)
        return 0;

    int32_t v8 = 0;
    for (osage_coli* v9 = coli; v9->type; v9++) {
        int32_t v11 = 0;
        vec3 v31 = vec3_null;
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
            mat4_scale_rot(&v64, parent_scale->x, parent_scale->y, parent_scale->z, v41);
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
        mat4_scale_rot(&v65, parent_scale->x, parent_scale->y, parent_scale->z, &v65);
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
        v4 = vec3_null;

    size_t v7 = rob_osg->osage_setting.parts;
    size_t v8 = 0;
    if (v7 >= 4) {
        float_t v9 = a3 * a3 * a3 * a3;
        size_t v10 = ((v7 - 4ULL) >> 2) + 1;
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
        rob_osg->external_force = vec3_null;
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

void ex_osage_block_field_18(ex_osage_block* osg, int32_t a2, bool a3) {
    float_t(*get_delta_frame)() = (float_t(*)())0x0000000140192D50;

    rob_chara_item_equip* rob_item_equip = osg->base.item_equip_object->item_equip;
    float_t step = get_delta_frame() * rob_item_equip->step;
    if (vector_old_length(rob_item_equip->field_940)
        && rob_item_equip->field_940.begin[0].field_C)
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
            ex_osage_block_set_wind_direction(osg);
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

void ex_osage_block_field_20(ex_osage_block* osg) {
    osg->field_1FF8 &= ~2;
    if (osg->base.field_59) {
        osg->base.field_59 = false;
        return;
    }

    float_t(*get_delta_frame)() = (float_t(*)())0x0000000140192D50;

    rob_chara_item_equip* rob_item_equip = osg->base.item_equip_object->item_equip;
    float_t step = get_delta_frame() * rob_item_equip->step;
    if (vector_old_length(rob_item_equip->field_940)
        && rob_item_equip->field_940.begin[0].field_C)
        step = 1.0f;

    bone_node* parent_node = osg->base.parent_bone_node;
    vec3 parent_scale = parent_node->exp_data.parent_scale;
    vec3 scale = parent_node->exp_data.scale;

    mat4 mat = *parent_node->ex_data_mat;
    if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f) {
        vec3_div(vec3_identity, scale, scale);
        mat4_transpose(&mat, &mat);
        mat4_scale_rot(&mat, scale.x, scale.y, scale.z, &mat);
        mat4_transpose(&mat, &mat);
    }
    ex_osage_block_set_wind_direction(osg);

    rob_osage_coli_set(&osg->rob, osg->mat);
    sub_14047C750(&osg->rob, &mat, &parent_scale, step);
}

void sub_140482DF0(struc_477* dst, struc_477* src0, struc_477* src1, float_t blend) {
    dst->length = lerp(src0->length, src1->length, blend);
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

void sub_14047E240(rob_osage* rob_osg, mat4* a2, vec3* a3, vector_old_opd_blend_data* a4) {
    if (!vector_old_length(*a4))
        return;

    vec3 v63;
    vec3_mult(rob_osg->exp_data.position, *a3, v63);

    mat4 v85;
    mat4_transpose(a2, &v85);
    mat4_mult_vec3_trans(&v85, &v63, &rob_osg->nodes.begin[0].trans);
    mat4_transpose(&v85, &v85);

    sub_14047F110(rob_osg, &v85, a3, 0);
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

            vec3 v64 = vec3_null;
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
        v54 = clamp(v54, (float_t)-M_PI, (float_t)M_PI);
        v55 = clamp(v55, (float_t)-M_PI, (float_t)M_PI);
        mat4_rotate_z_mult(&v85, v55, &v85);
        mat4_rotate_y_mult(&v85, v54, &v85);
        mat4_transpose(&v85, &v85);
        i->bone_node_ptr->exp_data.parent_scale = *a3;
        *i->bone_node_ptr->ex_data_mat = v85;
        mat4_transpose(&v85, &v85);
        if (i->bone_node_mat) {
            mat4 mat = v85;
            mat4_scale_rot(&mat, a3->x, a3->y, a3->z, &mat);
            mat4_transpose(&mat, i->bone_node_mat);
        }
        mat4_translate_mult(&v85, i->field_1B0.field_0.length, 0.0f, 0.0f, &v85);
        i->trans_orig = i->trans;
        mat4_get_translation(&v85, &i->trans);
    }

    if (vector_old_length(rob_osg->nodes) && rob_osg->node.bone_node_mat) {
        mat4 v87 = *rob_osg->nodes.end[-1].bone_node_ptr->ex_data_mat;
        mat4_transpose(&v87, &v87);
        mat4_translate_mult(&v87, rob_osg->node.length * a3->x, 0.0f, 0.0f, &v87);
        mat4_transpose(&v87, &v87);
        *rob_osg->node.bone_node_ptr->ex_data_mat = v87;

        mat4_transpose(&v87, &v87);
        mat4_scale_rot(&v87, a3->x, a3->y, a3->z, &v87);
        mat4_transpose(&v87, &v87);
        *rob_osg->node.bone_node_mat = v87;
        rob_osg->node.bone_node_ptr->exp_data.parent_scale = *a3;
    }
}

void ex_osage_block_set_osage_play_data(ex_osage_block* osg) {
    rob_chara_item_equip* rob_itm_equip = osg->base.item_equip_object->item_equip;
    bone_node* parent_node = osg->base.parent_bone_node;
    vec3 parent_scale = parent_node->exp_data.parent_scale;
    sub_14047E240(&osg->rob, parent_node->ex_data_mat, &parent_scale, &rob_itm_equip->field_940);
}

void ex_osage_block_disp(ex_osage_block* osg) {

}

void ex_osage_block_reset(ex_osage_block* osg) {
    osg->index = 0;
    rob_osage_init(&osg->rob);
    osg->field_1FF8 &= ~3;
    osg->mat = 0;
    osg->step = 1.0f;
    osg->base.bone_node_ptr = 0;
}

void ex_osage_block_field_40(ex_osage_block* osg) {

}

void ex_osage_block_field_48(ex_osage_block* osg) {
    void (*sub_14047F990) (rob_osage * a1, mat4 * a2, vec3 * a3, bool a4)
        = (void (*) (rob_osage*, mat4*, vec3*, bool))0x000000014047F990;

    osg->step = 4.0f;
    ex_osage_block_set_wind_direction(osg);
    rob_osage_coli_set(&osg->rob, osg->mat);
    bone_node* parent_node = osg->base.parent_bone_node;
    vec3 parent_scale = parent_node->exp_data.parent_scale;
    vec3_mult_scalar(parent_scale, 0.5f, parent_scale);
    sub_14047F990(&osg->rob, parent_node->ex_data_mat, &parent_scale, false);
    osg->field_1FF8 &= ~2;
}

void ex_osage_block_field_50(ex_osage_block* osg) {
    if (osg->base.field_59) {
        osg->base.field_59 = 0;
        return;
    }

    void (*_sub_14047C770) (rob_osage * rob_osg, mat4 * mat,
        vec3 * parent_scale, float_t step, bool a5)
        = (void(*) (rob_osage*, mat4*, vec3*, float_t, bool))0x000000014047C770;

    ex_osage_block_set_wind_direction(osg);

    bone_node* parent_node = osg->base.parent_bone_node;
    vec3 parent_scale = parent_node->exp_data.parent_scale;
    rob_osage_coli_set(&osg->rob, osg->mat);
    sub_14047C770(&osg->rob, parent_node->ex_data_mat, &parent_scale, osg->step, 1);
    float_t step = 0.5f * osg->step;
    osg->step = max(step, 1.0f);
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

void sub_14047E1C0(rob_osage* rob_osg, vec3* a2) {
    for (rob_osage_node* i = rob_osg->nodes.begin + 1; i != rob_osg->nodes.end; ++i) {
        if (!i->data_ptr->normal_ref.field_0)
            continue;

        sub_14053CE30(&i->data_ptr->normal_ref, i->bone_node_mat);

        mat4 mat;
        mat4_transpose(i->bone_node_mat, &mat);
        mat4_scale_rot(&mat, a2->x, a2->y, a2->z, &mat);
        mat4_transpose(&mat, i->bone_node_mat);
    }
}

void ex_osage_block_field_58(ex_osage_block* osg) {
    void (*_sub_14047E1C0) (rob_osage * a1, vec3 * a2)
        = (void (*) (rob_osage*, vec3*))0x000000014047E1C0;

    bone_node* parent_node = osg->base.parent_bone_node;
    vec3 parent_scale = parent_node->exp_data.parent_scale;
    sub_14047E1C0(&osg->rob, &parent_scale);
}
