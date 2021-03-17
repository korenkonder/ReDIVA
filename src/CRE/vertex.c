/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "vertex.h"
#include "../KKdLib/half_t.h"
#include "../KKdLib/vec.h"
#include "../KKdLib/vector.h"

typedef struct vertex_data {
    vec3 pos;
    vec2 uv[2];
    vec4 color;
    vec3 normal;
    vec3 tangent;
    uint16_t bone_index[4];
    float_t bone_weight[4];
} vertex_data;

vector(vertex_data)

vertex* vertex_init() {
    vertex* v = force_malloc(sizeof(vertex));
    return v;
}

void vertex_load(vertex* v, vertex_update* upd) {
    if (!v || !upd)
        return;

    free(v->vert);
    free(v->ind);
    v->vert_count = 0;
    v->ind_count = 0;
    v->type = VERTEX_NONE;

    if (!upd->data || !upd->length)
        return;

    float_t* data = upd->data;
    size_t length = upd->length;
    bool uv = upd->uv;
    bool uv2 = upd->uv2;
    bool color = upd->color;
    bool normal = upd->normal;
    bool bones = upd->bones;
    size_t len = 3LL + (uv ? (2LL + (uv2 ? 2 : 0)) : 0) + (color ? 4 : 0) + (normal ? 3 : 0) + (bones ? 8 : 0);
    length /= len;
    vertex_data* verts = force_malloc_s(sizeof(vertex_data), length);
    if (bones) {
        for (size_t i = 0; i < length; i++) {
            vertex_data* v = &verts[i];
            v->pos.x = *data++;
            v->pos.y = *data++;
            v->pos.z = *data++;
            if (uv) {
                v->uv[0].x = *data++;
                v->uv[0].y = *data++;

                if (uv2) {
                    v->uv[1].x = *data++;
                    v->uv[1].y = *data++;
                }
            }

            if (color) {
                v->color.x = *data++;
                v->color.y = *data++;
                v->color.z = *data++;
                v->color.w = *data++;
            }
            else
                v->color = vec4_identity;

            if (normal) {
                v->normal.x = *data++;
                v->normal.y = *data++;
                v->normal.z = *data++;
                vec3_normalize(v->normal, v->normal);
            }

            for (int32_t j = 0; j < 4; j++)
                v->bone_index[j] = (uint16_t)*data++;

            for (int32_t j = 0; j < 4; j++)
                v->bone_weight[j] = *data++;
        }
    }
    else {
        for (size_t i = 0; i < length; i++) {
            vertex_data* v = &verts[i];
            v->pos.x = *data++;
            v->pos.y = *data++;
            v->pos.z = *data++;
            if (uv) {
                v->uv[0].x = *data++;
                v->uv[0].y = *data++;

                if (uv2) {
                    v->uv[1].x = *data++;
                    v->uv[1].y = *data++;
                }
            }

            if (color) {
                v->color.x = *data++;
                v->color.y = *data++;
                v->color.z = *data++;
                v->color.w = *data++;
            }
            else
                v->color = vec4_identity;

            if (normal) {
                v->normal.x = *data++;
                v->normal.y = *data++;
                v->normal.z = *data++;
                vec3_normalize(v->normal, v->normal);
            }
        }
    }

    v->ind_count = length;
    v->ind = force_malloc_s(sizeof(uint32_t), v->ind_count);

    vector_vertex_data vert_list;
    vert_list.begin = vert_list.end = vert_list.capacity_end = 0;
    for (size_t i = 0, j = 0; i < length; i++) {
        bool found = false;
        size_t count = vert_list.end - vert_list.begin;
        for (j = 0; j < count; i++)
            if (!memcmp(&vert_list.begin[j], &verts[i], sizeof(vertex_data))) {
                found = true;
                break;
            }

        if (!found)
            vector_vertex_data_push_back(&vert_list, &verts[i]);
        v->ind[i] = (uint32_t)j;
    }

    vector_size_t r;
    vec3 edge0, edge1, t;
    vec2 uv0, uv1;
    r.begin = r.end = r.capacity_end = 0;
    for (vertex_data* i = vert_list.begin; i != vert_list.end; i++) {
        vec3 p = i->pos;
        vec3 n = i->normal;
        vec3 pos, normal;
        for (size_t j = 0; j < length; j++) {
            pos = verts[j].pos;
            normal = verts[j].normal;
            if (p.x == pos.x && p.y == pos.y && p.z == pos.z
                && n.x == normal.x && n.y == normal.y && n.z == normal.z)
                vector_size_t_push_back(&r, &j);
        }

        if (r.end - r.begin < 1) {
            vector_size_t_clear(&r);
            continue;
        }

        t = vec3_null;
        for (size_t* j = r.begin; j != r.end; j++) {
            vec3 t0, t1;
            vertex_data* v1 = &verts[*j / 3 * 3 + 0];
            vertex_data* v2 = &verts[*j / 3 * 3 + 1];
            vertex_data* v3 = &verts[*j / 3 * 3 + 2];
            vec2_sub(v2->uv[0], v1->uv[0], uv0);
            vec2_sub(v3->uv[0], v1->uv[0], uv1);
            if (uv0.x * uv1.y - uv1.x * uv0.y != 0) {
                vec3_sub(v2->pos, v1->pos, edge0);
                vec3_sub(v3->pos, v1->pos, edge1);
                vec3_mult_scalar(edge0, uv1.y, t0);
                vec3_mult_scalar(edge1, uv0.y, t1);
                vec3_sub(t0, t1, t1);
                vec3_mult_scalar(t1, 1.0f / (uv0.x * uv1.y - uv1.x * uv0.y), t1);
                vec3_normalize(t1, t1);
                vec3_add(t, t1, t);
            }
        }
        vector_size_t_clear(&r);
        vec3_mult_scalar(t, 1.0f / (float_t)(r.end - r.begin), t);
        vec3_normalize(t, i->tangent);
    }
    free(r.begin);

    v->vert_count = vert_list.end - vert_list.begin;
    if (bones) {
        v->type = VERTEX_BONED;
        v->vert = force_malloc_s(56, v->vert_count);
        uint64_t vert_ptr = (uint64_t)v->vert;
        for (size_t i = 0; i < v->vert_count; i++) {
            vertex_data* vert = &vert_list.begin[i];
            *(float_t*)(vert_ptr + 0) = vert->pos.x;
            *(float_t*)(vert_ptr + 4) = vert->pos.y;
            *(float_t*)(vert_ptr + 8) = vert->pos.z;
            *(half_t*)(vert_ptr + 12) = float_to_half(vert->uv[0].x);
            *(half_t*)(vert_ptr + 14) = float_to_half(vert->uv[0].y);
            *(half_t*)(vert_ptr + 16) = float_to_half(vert->uv[1].x);
            *(half_t*)(vert_ptr + 18) = float_to_half(vert->uv[1].y);
            *(half_t*)(vert_ptr + 20) = float_to_half(vert->color.x);
            *(half_t*)(vert_ptr + 22) = float_to_half(vert->color.y);
            *(half_t*)(vert_ptr + 24) = float_to_half(vert->color.z);
            *(half_t*)(vert_ptr + 26) = float_to_half(vert->color.w);
            *(half_t*)(vert_ptr + 28) = float_to_half(vert->normal.x);
            *(half_t*)(vert_ptr + 30) = float_to_half(vert->normal.y);
            *(half_t*)(vert_ptr + 32) = float_to_half(vert->normal.z);
            *(half_t*)(vert_ptr + 34) = float_to_half(vert->tangent.x);
            *(half_t*)(vert_ptr + 36) = float_to_half(vert->tangent.y);
            *(half_t*)(vert_ptr + 38) = float_to_half(vert->tangent.z);
            *(uint16_t*)(vert_ptr + 40) = vert->bone_index[0];
            *(uint16_t*)(vert_ptr + 42) = vert->bone_index[1];
            *(uint16_t*)(vert_ptr + 44) = vert->bone_index[2];
            *(uint16_t*)(vert_ptr + 46) = vert->bone_index[3];
            *(half_t*)(vert_ptr + 48) = float_to_half(vert->bone_weight[0]);
            *(half_t*)(vert_ptr + 50) = float_to_half(vert->bone_weight[1]);
            *(half_t*)(vert_ptr + 52) = float_to_half(vert->bone_weight[2]);
            *(half_t*)(vert_ptr + 54) = float_to_half(vert->bone_weight[3]);
            vert_ptr += 56;
        }
    }
    else {
        v->type = VERTEX_SIMPLE;
        v->vert = force_malloc_s(40, v->vert_count);
        uint64_t vert_ptr = (uint64_t)v->vert;
        for (size_t i = 0; i < v->vert_count; i++) {
            vertex_data* vert = &vert_list.begin[i];
            *(float_t*)(vert_ptr + 0) = vert->pos.x;
            *(float_t*)(vert_ptr + 4) = vert->pos.y;
            *(float_t*)(vert_ptr + 8) = vert->pos.z;
            *(half_t*)(vert_ptr + 12) = float_to_half(vert->uv[0].x);
            *(half_t*)(vert_ptr + 14) = float_to_half(vert->uv[0].y);
            *(half_t*)(vert_ptr + 16) = float_to_half(vert->uv[1].x);
            *(half_t*)(vert_ptr + 18) = float_to_half(vert->uv[1].y);
            *(half_t*)(vert_ptr + 20) = float_to_half(vert->color.x);
            *(half_t*)(vert_ptr + 22) = float_to_half(vert->color.y);
            *(half_t*)(vert_ptr + 24) = float_to_half(vert->color.z);
            *(half_t*)(vert_ptr + 26) = float_to_half(vert->color.w);
            *(half_t*)(vert_ptr + 28) = float_to_half(vert->normal.x);
            *(half_t*)(vert_ptr + 30) = float_to_half(vert->normal.y);
            *(half_t*)(vert_ptr + 32) = float_to_half(vert->normal.z);
            *(half_t*)(vert_ptr + 34) = float_to_half(vert->tangent.x);
            *(half_t*)(vert_ptr + 36) = float_to_half(vert->tangent.y);
            *(half_t*)(vert_ptr + 38) = float_to_half(vert->tangent.z);
            vert_ptr += 40;
        }
    }

    free(vert_list.begin);
    free(verts);
}

void vertex_dispose(vertex* v) {
    if (!v)
        return;

    free(v->vert);
    free(v->ind);
    free(v);
}

vector_func(vertex_data)
