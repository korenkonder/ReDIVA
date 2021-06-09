/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "vertex.h"
#include "../KKdLib/half_t.h"
#include "../KKdLib/hash.h"
#include "../KKdLib/vector.h"
#include "../CRE/static_var.h"
#define GLEW_STATIC
#include <GLEW/glew.h>

typedef struct vertex_pre_data {
    vec3 pos;
    vec2 texcoord0;
    vec2 texcoord1;
    vec2 texcoord2;
    vec2 texcoord3;
    vec4 color;
    vec3 normal;
    vec3 tangent;
    vec4u16 bone_index;
    vec4 bone_weight;
} vertex_pre_data;

vector(vertex_pre_data)

void vertex_init(vertex* v) {
    glGenBuffers(1, &v->vbo);
    glGenBuffers(1, &v->ebo);
}

void vertex_load(vertex* v, vertex_data* upd) {
    if (!v || !upd)
        return;

    free(v->vert);
    free(v->ind);
    v->vert_count = 0;
    v->ind_count = 0;
    v->type = VERTEX_NONE;

    if (!upd->position || !upd->length)
        return;

    bool use_bone_data = false;
    bool translucent = false;
    size_t length = upd->length;
    vec3* position = upd->position;
    vec2* texcoord0 = upd->texcoord0;
    vec2* texcoord1 = upd->texcoord1;
    vec2* texcoord2 = upd->texcoord2;
    vec2* texcoord3 = upd->texcoord3;
    vec4* color = upd->color;
    vec3* normal = upd->normal;
    vec4i* bone_index = upd->bone_index;
    vec4* bone_weight = upd->bone_weight;

    vertex_pre_data* verts = force_malloc_s(vertex_pre_data, length);
    for (size_t i = 0; i < length; i++) {
        vertex_pre_data* v = &verts[i];
        v->pos = *position++;
    }

    if (texcoord0)
        for (size_t i = 0; i < length; i++) {
            vertex_pre_data* v = &verts[i];
            v->texcoord0 = *texcoord0++;
        }
    else
        for (size_t i = 0; i < length; i++) {
            vertex_pre_data* v = &verts[i];
            v->texcoord0 = vec2_null;
        }

    if (texcoord1)
        for (size_t i = 0; i < length; i++) {
            vertex_pre_data* v = &verts[i];
            v->texcoord1 = *texcoord1++;
        }
    else
        for (size_t i = 0; i < length; i++) {
            vertex_pre_data* v = &verts[i];
            v->texcoord1 = vec2_null;
        }

    if (texcoord2)
        for (size_t i = 0; i < length; i++) {
            vertex_pre_data* v = &verts[i];
            v->texcoord2 = *texcoord2++;
        }
    else
        for (size_t i = 0; i < length; i++) {
            vertex_pre_data* v = &verts[i];
            v->texcoord2 = vec2_null;
        }

    if (texcoord3)
        for (size_t i = 0; i < length; i++) {
            vertex_pre_data* v = &verts[i];
            v->texcoord3 = *texcoord3++;
        }
    else
        for (size_t i = 0; i < length; i++) {
            vertex_pre_data* v = &verts[i];
            v->texcoord3 = vec2_null;
        }

    if (color)
        for (size_t i = 0; i < length; i++) {
            vertex_pre_data* v = &verts[i];
            v->color = *color++;
        }
    else
        for (size_t i = 0; i < length; i++) {
            vertex_pre_data* v = &verts[i];
            v->color = vec4_identity;
        }

    if (normal)
        for (size_t i = 0; i < length; i++) {
            vertex_pre_data* v = &verts[i];
            v->normal = *normal++;
        }
    else
        for (size_t i = 0; i < length; i++) {
            vertex_pre_data* v = &verts[i];
            v->normal = vec3_null;
        }

    if (bone_index && bone_weight)
        for (size_t i = 0; i < length; i++) {
            vertex_pre_data* v = &verts[i];
            if (bone_index->x < 0x0000 || bone_index->x > 0xFFFF)
                v->bone_index.x = 0xFFFF;
            else
                v->bone_index.x = bone_index->x;

            if (bone_index->y < 0x0000 || bone_index->y > 0xFFFF)
                v->bone_index.y = 0xFFFF;
            else
                v->bone_index.y = bone_index->y;

            if (bone_index->z < 0x0000 || bone_index->z > 0xFFFF)
                v->bone_index.z = 0xFFFF;
            else
                v->bone_index.z = bone_index->z;

            if (bone_index->w < 0x0000 || bone_index->w > 0xFFFF)
                v->bone_index.w = 0xFFFF;
            else
                v->bone_index.w = bone_index->w;

            float_t bone_weight_length;
            vec4_normalize(*bone_weight++, v->bone_weight);
            vec4_length_squared(v->bone_weight, bone_weight_length);
            if (bone_weight_length != 0.0f)
                use_bone_data = true;
        }
    else
        for (size_t i = 0; i < length; i++) {
            vertex_pre_data* v = &verts[i];
            v->bone_index = (vec4u16){ 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF };
            v->bone_weight = vec4_null;
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
                vec2_sub(v2->texcoord0, v1->texcoord0, uv0);
                vec2_sub(v3->texcoord0, v1->texcoord0, uv1);
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

    v->vert = force_malloc_s(vertex_struct, v->vert_count);
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

    vertex_struct* vert_ptr = v->vert;
    for (vertex_pre_data* i = vert_list.begin; i != vert_list.end; i++) {
        vec3 pos;
        vec2 texcoord0;
        vec2 texcoord1;
        vec2 texcoord2;
        vec2 texcoord3;
        vec4 color;
        vec3 normal;
        vec3 tangent;
        vec4u16 bone_index;
        vec4 bone_weight;

        pos = i->pos;
        texcoord0 = i->texcoord0;
        texcoord1 = i->texcoord1;
        texcoord2 = i->texcoord2;
        texcoord3 = i->texcoord3;
        color = i->color;
        normal = i->normal;
        tangent = i->tangent;
        bone_index = i->bone_index;
        bone_weight = i->bone_weight;

        vec3_min(bound_box_min, pos, bound_box_min);
        vec3_max(bound_box_max, pos, bound_box_max);

        vec3_mult_min_max_scalar(normal, -32767.0f, 32767.0f, normal);
        vec3_mult_min_max_scalar(tangent, -32767.0f, 32767.0f, tangent);
        vec4_mult_scalar(bone_weight, 255.0f, bone_weight);

        vert_ptr->pos = pos;
        vec2_to_vec2h(texcoord0, vert_ptr->texcoord[0]);
        vec2_to_vec2h(texcoord1, vert_ptr->texcoord[1]);
        vec2_to_vec2h(texcoord2, vert_ptr->texcoord[2]);
        vec2_to_vec2h(texcoord3, vert_ptr->texcoord[3]);
        vec4_to_vec4h(color, vert_ptr->color);
        vec3_to_vec3i16(normal, vert_ptr->normal);
        vec3_to_vec3i16(tangent, vert_ptr->tangent);
        vert_ptr->bone_index = bone_index;
        vec4_to_vec4u8(bone_weight, vert_ptr->bone_weight);
        vert_ptr++;
    }
    vector_vertex_pre_data_free(&vert_list);
    v->bound_box.min = bound_box_min;
    v->bound_box.max = bound_box_max;

    bind_array_buffer(v->vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(sizeof(vertex_data) * v->vert_count),
        (void*)v->vert, GL_DYNAMIC_DRAW);
    bind_array_buffer(0);

    bind_element_array_buffer(v->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(sizeof(uint32_t) * v->ind_count),
        v->ind, GL_DYNAMIC_DRAW);
    bind_element_array_buffer(0);
}

void vertex_free(vertex* v) {
    if (!v)
        return;

    if (v->vbo)
        glDeleteBuffers(1, &v->vbo);
    if (v->ebo)
        glDeleteBuffers(1, &v->ebo);

    free(v->vert);
    free(v->ind);
    v->vert_count = 0;
    v->ind_count = 0;
    v->type = VERTEX_NONE;
}

vector_func(vertex_pre_data)
