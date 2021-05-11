/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "vertex.h"
#include "../KKdLib/half_t.h"
#include "../KKdLib/hash.h"
#include "../KKdLib/vector.h"

#pragma pack(push, 1)
typedef struct vertex_pre_data {
    vec3 pos;
    vec2 uv;
    vec2 uv2;
    vec4 color;
    vec3 normal;
    vec3 tangent;
    vec4i bone_index;
    vec4 bone_weight;
} vertex_pre_data;
#pragma pack(pop)

vector(vertex_pre_data)

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

    bool use_bone_data = false;
    bool translucent = false;
    float_t* data = upd->data;
    size_t length = upd->length;
    bool uv = upd->uv;
    bool uv2 = upd->uv2;
    bool color = upd->color;
    bool normal = upd->normal;
    bool bones = upd->bones;
    size_t len = 3LL + (uv ? (2LL + (uv2 ? 2 : 0)) : 0) + (color ? 4 : 0) + (normal ? 3 : 0) + (bones ? 8 : 0);
    length /= len;
    vertex_pre_data* verts = force_malloc_s(vertex_pre_data, length);
    if (bones) {
        for (size_t i = 0; i < length; i++) {
            vertex_pre_data* v = &verts[i];
            v->pos.x = *data++;
            v->pos.y = *data++;
            v->pos.z = *data++;
            if (uv) {
                v->uv.x = *data++;
                v->uv.y = *data++;

                if (uv2) {
                    v->uv2.x = *data++;
                    v->uv2.y = *data++;
                }
                else
                    v->uv2 = vec2_null;
            }
            else {
                v->uv = vec2_null;
                v->uv2 = vec2_null;
            }

            if (color) {
                v->color.x = *data++;
                v->color.y = *data++;
                v->color.z = *data++;
                v->color.w = *data++;
                if (v->color.w < 1.0f)
                    translucent = true;
            }
            else
                v->color = vec4_identity;

            if (normal) {
                v->normal.x = *data++;
                v->normal.y = *data++;
                v->normal.z = *data++;
                vec3_normalize(v->normal, v->normal);
            }
            else
                v->normal = vec3_null;

            v->tangent = vec3_null;

            v->bone_index.x = (int32_t)*data++;
            v->bone_index.y = (int32_t)*data++;
            v->bone_index.z = (int32_t)*data++;
            v->bone_index.w = (int32_t)*data++;

            vec4 bone_weight;
            bone_weight.x = *data++;
            bone_weight.y = *data++;
            bone_weight.z = *data++;
            bone_weight.w = *data++;
            vec4_normalize(bone_weight, bone_weight);

            float_t bone_weight_length;
            vec4_length_squared(bone_weight, bone_weight_length);
            if (bone_weight_length != 0.0f)
                use_bone_data = true;

            v->bone_weight = bone_weight;
        }
    }
    else {
        for (size_t i = 0; i < length; i++) {
            vertex_pre_data* v = &verts[i];
            v->pos.x = *data++;
            v->pos.y = *data++;
            v->pos.z = *data++;
            if (uv) {
                v->uv.x = *data++;
                v->uv.y = *data++;

                if (uv2) {
                    v->uv2.x = *data++;
                    v->uv2.y = *data++;
                }
                else
                    v->uv2 = vec2_null;
            }
            else {
                v->uv = vec2_null;
                v->uv2 = vec2_null;
            }

            if (color) {
                v->color.x = *data++;
                v->color.y = *data++;
                v->color.z = *data++;
                v->color.w = *data++;
                if (v->color.w < 1.0f)
                    translucent = true;
            }
            else
                v->color = vec4_identity;

            if (normal) {
                v->normal.x = *data++;
                v->normal.y = *data++;
                v->normal.z = *data++;
                vec3_normalize(v->normal, v->normal);
            }
            else
                v->normal = vec3_null;

            v->tangent = vec3_null;
            v->bone_index = vec4i_null;
            v->bone_weight = vec4_null;
        }
    }

    v->translucent = translucent;
    v->ind_count = length;
    v->ind = force_malloc_s(uint32_t, v->ind_count);

    vector_vertex_pre_data vert_list = { 0, 0, 0 };
    vector_vertex_pre_data_append(&vert_list, length);
    for (size_t i = 0; i < length; i++) {
        vertex_pre_data* vert = &verts[i];

        bool found = false;
        vertex_pre_data* j = vert_list.begin;
        uint32_t k = 0;
        for (; j != vert_list.end; j++, k++)
            if (!memcmp(j, vert, sizeof(vertex_pre_data))) {
                found = true;
                break;
            }

        if (!found)
            vector_vertex_pre_data_push_back(&vert_list, vert);
        v->ind[i] = k;
    }

    if (normal) {
        vec3 edge0, edge1, t;
        vec2 uv0, uv1;
        vector_size_t r = { 0, 0, 0 };
        vector_size_t_append(&r, length);
        for (vertex_pre_data* i = vert_list.begin; i != vert_list.end; i++) {
            vec3 p = i->pos;
            vec3 n = i->normal;
            for (size_t j = 0; j < length; j++) {
                vec3 pos = verts[j].pos;
                vec3 normal = verts[j].normal;
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
                vertex_pre_data* v1 = &verts[*j / 3 * 3 + 0];
                vertex_pre_data* v2 = &verts[*j / 3 * 3 + 1];
                vertex_pre_data* v3 = &verts[*j / 3 * 3 + 2];
                vec2_sub(v2->uv, v1->uv, uv0);
                vec2_sub(v3->uv, v1->uv, uv1);
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
            vec3_mult_scalar(t, 1.0f / (float_t)(r.end - r.begin), t);
            vec3_normalize(t, i->tangent);
            vector_size_t_clear(&r);
        }
        vector_size_t_free(&r);
    }
    free(verts);

    v->vert_count = vert_list.end - vert_list.begin;
    if (use_bone_data)
        v->type = VERTEX_BONED;
    else
        v->type = VERTEX_SIMPLE;

    v->vert = force_malloc_s(vertex_data, v->vert_count);
    if (!v->vert) {
        vector_vertex_pre_data_free(&vert_list);
        vertex_bounding_box bound_box;
        bound_box.min = vec3_null;
        bound_box.max = vec3_null;
        v->bound_box = bound_box;
        return;
    }

    vec3 bound_box_min = (vec3){ FLT_MAX, FLT_MAX, FLT_MAX };
    vec3 bound_box_max = (vec3){ -FLT_MAX, -FLT_MAX, -FLT_MAX };

    vertex_data* vert_ptr = v->vert;
    for (vertex_pre_data* i = vert_list.begin; i != vert_list.end; i++) {
        vec3 pos;
        vec2 uv;
        vec2 uv2;
        vec4 color;
        vec3 normal;
        vec3 tangent;
        vec4i bone_index;
        vec4 bone_weight;

        pos = i->pos;
        uv = i->uv;
        uv2 = i->uv2;
        color = i->color;
        normal = i->normal;
        tangent = i->tangent;
        bone_index = i->bone_index;
        bone_weight = i->bone_weight;

        vec3_min(bound_box_min, pos, bound_box_min);
        vec3_max(bound_box_max, pos, bound_box_max);

        vec3_mult_min_max_scalar(normal, -32768.0f, 32767.0f, normal);
        vec3_mult_min_max_scalar(tangent, -32768.0f, 32767.0f, tangent);
        vec4_mult_scalar(bone_weight, 255.0f, bone_weight);

        vert_ptr->pos = pos;
        vert_ptr->uv.x = float_to_half(uv.x);
        vert_ptr->uv.y = float_to_half(uv.y);
        vert_ptr->uv2.x = float_to_half(uv2.x);
        vert_ptr->uv2.y = float_to_half(uv2.y);
        vert_ptr->color.r = float_to_half(color.x);
        vert_ptr->color.g = float_to_half(color.y);
        vert_ptr->color.b = float_to_half(color.z);
        vert_ptr->color.a = float_to_half(color.w);
        vert_ptr->normal.x = (int16_t)normal.x;
        vert_ptr->normal.y = (int16_t)normal.y;
        vert_ptr->normal.z = (int16_t)normal.z;
        vert_ptr->tangent.x = (int16_t)tangent.x;
        vert_ptr->tangent.y = (int16_t)tangent.y;
        vert_ptr->tangent.z = (int16_t)tangent.z;
        vert_ptr->bone_index.x = (uint16_t)bone_index.x;
        vert_ptr->bone_index.y = (uint16_t)bone_index.y;
        vert_ptr->bone_index.z = (uint16_t)bone_index.z;
        vert_ptr->bone_index.w = (uint16_t)bone_index.w;
        vert_ptr->bone_weight.x = (uint8_t)bone_weight.x;
        vert_ptr->bone_weight.y = (uint8_t)bone_weight.y;
        vert_ptr->bone_weight.z = (uint8_t)bone_weight.z;
        vert_ptr->bone_weight.w = (uint8_t)bone_weight.w;
        vert_ptr++;
    }
    vector_vertex_pre_data_free(&vert_list);
    v->bound_box.min = bound_box_min;
    v->bound_box.max = bound_box_max;
}

void vertex_dispose(vertex* v) {
    if (!v)
        return;

    free(v->vert);
    free(v->ind);
    free(v);
}

vector_func(vertex_pre_data)
