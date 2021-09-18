/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "vertex.h"
#include "../KKdLib/half_t.h"
#include "../KKdLib/hash.h"
#include "../KKdLib/vector.h"
#include "../CRE/static_var.h"
#include <glad/glad.h>

vector(obj_vertex_data)

static void vertex_data_load(vertex* v, vertex_data* data, obj_vertex_data* verts, size_t index);
static void vertex_data_store(vertex* v, obj_vertex_data* verts, size_t index);

void vertex_init(vertex* v) {
    v->vbo = 0;
    v->ebo = 0;
}

void vertex_load(vertex* v, vertex_data* data) {
    if (!v || !data)
        return;

    vertex_free(v);

    if (!data->vert || !data->count || !data->morph_count)
        return;

    v->type = VERTEX_SIMPLE;
    v->translucent = false;
    size_t count = data->count;
    size_t morph_count = data->morph_count;
    obj_vertex_data* verts = force_malloc_s(obj_vertex_data, count * morph_count);
    for (size_t i = 0; i < morph_count; i++) {
        vertex_data_load(v, data, verts, i);
        if (~data->flags & OBJ_VERTEX_NORMAL
            || data->flags & OBJ_VERTEX_TANGENT
            || data->primitive != VERTEX_PRIMITIVE_TRIANGLE)
            continue;

        size_t offset = count * i;
        size_t count_clamp = count / 3 * 3;
        for (size_t j = 0; j < count_clamp; j += 3) {
            obj_vertex_data* vert = &verts[j + offset];
            vec2 uv0, uv1;
            vec2_sub(vert[1].texcoord0, vert[0].texcoord0, uv0);
            vec2_sub(vert[2].texcoord0, vert[0].texcoord0, uv1);

            vec3 edge0;
            vec3 edge1;
            vec3_sub(vert[1].position, vert[0].position, edge0);
            vec3_sub(vert[2].position, vert[0].position, edge1);

            vec3 t0, t1, t;
            vec3_mult_scalar(edge0, uv1.y, t0);
            vec3_mult_scalar(edge1, uv0.y, t1);
            vec3_sub(t0, t1, t);
            vec3_normalize(t, t);

            vec3 b0, b1, b;
            vec3_mult_scalar(edge1, uv0.x, b0);
            vec3_mult_scalar(edge0, uv1.x, b1);
            vec3_sub(b0, b1, b);
            vec3_normalize(b, b);

            for (int32_t k = 0; k < 3; k++) {
                vec3 temp;
                float_t direction;
                vec3_cross(vert[k].normal, t, temp);
                vec3_normalize(temp, temp);
                vec3_dot(temp, b, direction);
                *(vec3*)&vert[k].tangent = t;
                if (direction >= 0.0f)
                    vert[k].tangent.w = 1.0f;
                else
                    vert[k].tangent.w = -1.0f;
            }
        }
    }
    v->ind_count = count;
    v->ind = force_malloc_s(uint32_t, v->ind_count);

    vector_obj_vertex_data vert_list = vector_empty(obj_vertex_data);
    vector_obj_vertex_data_reserve(&vert_list, count * morph_count);
    for (size_t i = 0; i < count; i++) {
        bool found = false;
        uint32_t l = 0;
        for (size_t j = 0; j < morph_count; j++) {
            l = 0;
            bool f = false;
            obj_vertex_data* vert = &verts[i + j * count];
            for (obj_vertex_data* k = vert_list.begin; k != vert_list.end; k += morph_count, l++)
                if (!memcmp(k, vert, sizeof(obj_vertex_data))) {
                    f = true;
                    break;
                }

            if (found && !f)
                found = false;
            else if (f)
                found = true;
        }

        if (!found)
            for (size_t j = 0; j < morph_count; j++)
                vector_obj_vertex_data_push_back(&vert_list, &verts[i + j * count]);
        v->ind[i] = l;
    }

    v->vert_count = (vert_list.end - vert_list.begin) / data->morph_count;
    v->vert = force_malloc_s(vertex_struct, v->vert_count * data->morph_count);
    if (!v->vert || !vert_list.begin) {
        vector_obj_vertex_data_free(&vert_list);
        v->type = VERTEX_NONE;
        v->translucent = false;
        free(v->vert);
        v->vert_count = 0;
        free(v->ind);
        v->ind_count = 0;
        v->bound_box.min = vec3_null;
        v->bound_box.max = vec3_null;
        v->morph_count = 0;
        v->vbo = 0;
        v->ebo = 0;
        return;
    }
    v->morph_count = morph_count;
    free(verts);

    v->bound_box.min = (vec3){  FLT_MAX,  FLT_MAX,  FLT_MAX };
    v->bound_box.max = (vec3){ -FLT_MAX, -FLT_MAX, -FLT_MAX };
    for (size_t i = 0; i < morph_count; i++)
        vertex_data_store(v, vert_list.begin, i);
    vector_obj_vertex_data_free(&vert_list);

    v->vbo = force_malloc_s(uint32_t, v->morph_count);
    glGenBuffers((GLsizei)v->morph_count, v->vbo);
    for (size_t i = 0; i < morph_count; i++) {
        vertex_struct* vert_ptr = v->vert + i * v->vert_count;

        glBindBuffer(GL_ARRAY_BUFFER, v->vbo[i]);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(sizeof(vertex_struct) * v->vert_count),
            (void*)vert_ptr, GL_DYNAMIC_DRAW);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &v->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, v->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(sizeof(uint32_t) * v->ind_count),
        v->ind, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void vertex_free(vertex* v) {
    if (!v)
        return;

    if (v->vbo) {
        glDeleteBuffers((GLsizei)v->morph_count, v->vbo);
        free(v->vbo);
    }

    if (v->ebo) {
        glDeleteBuffers(1, &v->ebo);
        v->ebo = 0;
    }

    free(v->vert);
    free(v->ind);
    v->vert_count = 0;
    v->ind_count = 0;
    v->type = VERTEX_NONE;
    v->translucent = false;
}

static void vertex_data_load(vertex* v, vertex_data* data, obj_vertex_data* verts, size_t index) {
    size_t count = data->count;
    size_t offset = index * count;
    obj_vertex_data* vert = data->vert;

    verts += offset;
    vert += offset;
    if (data->flags & OBJ_VERTEX_POSITION)
        for (size_t i = 0; i < count; i++)
            verts[i].position = vert[i].position;
    else
        for (size_t i = 0; i < count; i++)
            verts[i].position = vec3_null;

    if (data->flags & OBJ_VERTEX_NORMAL)
        for (size_t i = 0; i < count; i++) {
            vec3 normal = vert[i].normal;
            vec3_normalize(normal, normal);
            verts[i].normal = normal;
        }
    else
        for (size_t i = 0; i < count; i++)
            verts[i].normal = vec3_null;

    if (data->flags & OBJ_VERTEX_TANGENT)
        for (size_t i = 0; i < count; i++) {
            vec3 tangent = *(vec3*)&vert[i].tangent;
            vec3_normalize(tangent, tangent);
            *(vec3*)&verts[i].tangent = tangent;
            verts[i].tangent.w = vert[i].tangent.w;
        }
    else
        for (size_t i = 0; i < count; i++)
            verts[i].tangent = vec4_null;

    if (data->flags & OBJ_VERTEX_TEXCOORD0)
        for (size_t i = 0; i < count; i++)
            verts[i].texcoord0 = vert[i].texcoord0;
    else
        for (size_t i = 0; i < count; i++)
            verts[i].texcoord0 = vec2_null;

    if (data->flags & OBJ_VERTEX_TEXCOORD1)
        for (size_t i = 0; i < count; i++)
            verts[i].texcoord1 = vert[i].texcoord1;
    else
        for (size_t i = 0; i < count; i++)
            verts[i].texcoord1 = vec2_null;

    if (data->flags & OBJ_VERTEX_TEXCOORD2)
        for (size_t i = 0; i < count; i++)
            verts[i].texcoord2 = vert[i].texcoord2;
    else
        for (size_t i = 0; i < count; i++)
            verts[i].texcoord2 = vec2_null;

    if (data->flags & OBJ_VERTEX_TEXCOORD3)
        for (size_t i = 0; i < count; i++)
            verts[i].texcoord3 = vert[i].texcoord3;
    else
        for (size_t i = 0; i < count; i++)
            verts[i].texcoord3 = vec2_null;

    if (data->flags & OBJ_VERTEX_COLOR0)
        for (size_t i = 0; i < count; i++) {
            verts[i].color0 = vert[i].color0;
            if (verts[i].color0.w != 1.0f)
                v->translucent = true;
        }
    else
        for (size_t i = 0; i < count; i++)
            verts[i].color0 = vec4_identity;

    if (data->flags & OBJ_VERTEX_COLOR1)
        for (size_t i = 0; i < count; i++)
            verts[i].color1 = vert[i].color1;
    else
        for (size_t i = 0; i < count; i++)
            verts[i].color1 = vec4_identity;

    if (data->flags & OBJ_VERTEX_BONE_DATA)
        for (size_t i = 0; i < count; i++) {
            vec4i bone_index = vert[i].bone_index;
            verts[i].bone_index = bone_index;
            if (bone_index.x < 0x0000 || bone_index.x > 0xFFFF)
                verts[i].bone_index.x = 0xFFFF;
            if (bone_index.y < 0x0000 || bone_index.y > 0xFFFF)
                verts[i].bone_index.y = 0xFFFF;
            if (bone_index.z < 0x0000 || bone_index.z > 0xFFFF)
                verts[i].bone_index.z = 0xFFFF;
            if (bone_index.w < 0x0000 || bone_index.w > 0xFFFF)
                verts[i].bone_index.w = 0xFFFF;

            vec4 bone_weight;
            float_t bone_weight_length;
            vec4_normalize(vert[i].bone_weight, bone_weight);
            vec4_length_squared(bone_weight, bone_weight_length);
            verts[i].bone_weight = bone_weight;
            if (bone_weight_length != 0.0f)
                v->type = VERTEX_BONED;
        }
    else
        for (size_t i = 0; i < count; i++) {
            verts[i].bone_index = (vec4i){ 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF };
            verts[i].bone_weight = vec4_null;
        }
}

static void vertex_data_store(vertex* v, obj_vertex_data* verts, size_t index) {
    size_t count = v->vert_count;
    size_t morph_count = v->morph_count;
    vertex_struct* vert_ptr = v->vert;
    vert_ptr += index * count;
    for (size_t i = 0; i < v->vert_count; i++) {
        vec3 pos;
        vec2 texcoord0;
        vec2 texcoord1;
        vec2 texcoord2;
        vec2 texcoord3;
        vec4 color0;
        vec4 color1;
        vec3 normal;
        vec4 tangent;
        vec4i bone_index;
        vec4 bone_weight;

        obj_vertex_data* vert = &verts[i * v->morph_count + index];
        pos = vert->position;
        texcoord0 = vert->texcoord0;
        texcoord1 = vert->texcoord1;
        texcoord2 = vert->texcoord2;
        texcoord3 = vert->texcoord3;
        color0 = vert->color0;
        color1 = vert->color1;
        normal = vert->normal;
        tangent = vert->tangent;
        bone_index = vert->bone_index;
        bone_weight = vert->bone_weight;

        vec3_min(v->bound_box.min, pos, v->bound_box.min);
        vec3_max(v->bound_box.max, pos, v->bound_box.max);

        vec3_mult_scalar(normal, 32767.0f, normal);
        vec4_mult_scalar(tangent, 32767.0f, tangent);
        vec4_mult_scalar(bone_weight, 255.0f, bone_weight);

        vert_ptr->pos = pos;
        vec2_to_vec2h(texcoord0, vert_ptr->texcoord[0]);
        vec2_to_vec2h(texcoord1, vert_ptr->texcoord[1]);
        vec2_to_vec2h(texcoord2, vert_ptr->texcoord[2]);
        vec2_to_vec2h(texcoord3, vert_ptr->texcoord[3]);
        vec4_to_vec4h(color0, vert_ptr->color[0]);
        vec4_to_vec4h(color1, vert_ptr->color[1]);
        vec3_to_vec3i16(normal, *(vec3i16*)&vert_ptr->normal);
        vert_ptr->normal.w = 0;
        vec3_to_vec3i16(tangent, vert_ptr->tangent);
        vec4i_to_vec4u16(bone_index, vert_ptr->bone_index);
        vec4_to_vec4u8(bone_weight, vert_ptr->bone_weight);
        vert_ptr++;
    }
}

vector_func(obj_vertex_data)
