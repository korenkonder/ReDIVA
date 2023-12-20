/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "disp_manager.hpp"
#include "../Glitter/glitter.hpp"
#include "../render_context.hpp"
#include "../shader_ft.hpp"
#include "draw_object.hpp"

extern render_context* rctx_ptr;

static const GLuint        POSITION_INDEX =  0;
static const GLuint     BONE_WEIGHT_INDEX =  1;
static const GLuint          NORMAL_INDEX =  2;
static const GLuint          COLOR0_INDEX =  3;
static const GLuint          COLOR1_INDEX =  4;
static const GLuint     MORPH_COLOR_INDEX =  5;
static const GLuint         TANGENT_INDEX =  6;
static const GLuint         UNKNOWN_INDEX =  7;
static const GLuint       TEXCOORD0_INDEX =  8;
static const GLuint       TEXCOORD1_INDEX =  9;
static const GLuint  MORPH_POSITION_INDEX = 10;
static const GLuint    MORPH_NORMAL_INDEX = 11;
static const GLuint   MORPH_TANGENT_INDEX = 12;
static const GLuint MORPH_TEXCOORD0_INDEX = 13;
static const GLuint MORPH_TEXCOORD1_INDEX = 14;
static const GLuint      BONE_INDEX_INDEX = 15;

static void object_data_get_vertex_attrib_buffer_bindings(const mdl::ObjSubMeshArgs* args,
    int32_t texcoord_array[2], GLuint vertex_attrib_buffer_binding[16]);

material_list_struct::material_list_struct() : blend_color(),
has_blend_color(), emission(), has_emission() {
    hash = (uint32_t)-1;
}

material_list_struct::material_list_struct(uint32_t hash, vec4& blend_color,
    vec4u8& has_blend_color, vec4& emission, vec4u8& has_emission) : hash(hash), blend_color(blend_color),
    has_blend_color(has_blend_color), emission(emission), has_emission(has_emission) {

}

texture_pattern_struct::texture_pattern_struct() {

}

texture_pattern_struct::texture_pattern_struct(texture_id src, texture_id dst) : src(src), dst(dst) {

}

texture_transform_struct::texture_transform_struct() {
    id = (uint32_t)-1;
    mat = mat4_identity;
}

texture_transform_struct::texture_transform_struct(uint32_t id, const mat4& mat) : id(id), mat(mat) {

}

morph_struct::morph_struct() : weight() {

}

texture_data_struct::texture_data_struct() : field_0() {

}

namespace mdl {
    ObjSubMeshArgs::ObjSubMeshArgs() : sub_mesh(), mesh(), material(), textures(), mat_count(),
        mats(), vertex_buffer(), index_buffer(), set_blend_color(), chara_color(), self_shadow(),
        shadow(), morph_vertex_buffer(), morph_weight(), texture_pattern_count(),
        texture_transform_count(), texture_transform_array(), instances_count(), instances_mat(), func() {

    }

    EtcObjTeapot::EtcObjTeapot() {
        size = 1.0f;
    }

    EtcObjGrid::EtcObjGrid() {
        w = 10;
        h = 10;
        ws = 20;
        hs = 20;
    }

    EtcObjCube::EtcObjCube() : wire() {

    }

    EtcObjSphere::EtcObjSphere() : wire() {
        radius = 1.0f;
        slices = 8;
        stacks = 8;
    }

    EtcObjPlane::EtcObjPlane() {
        w = 10;
        h = 10;
    }

    EtcObjCone::EtcObjCone() : wire() {
        base = 1.0f;
        height = 1.0f;
        slices = 8;
        stacks = 8;
    }

    EtcObjLine::EtcObjLine() {
        pos[0] = { 0.0f, 0.0f, 0.0f };
        pos[1] = { 0.0f, 0.0f, 1.0f };
    }

    EtcObjCross::EtcObjCross() {
        size = 0.1f;
    }

    EtcObjCapsule::EtcObjCapsule() : wire() {
        radius = 1.0f;
        slices = 8;
        stacks = 8;
        pos[0] = { 0.0f, 0.0f, 0.0f };
        pos[1] = { 0.0f, 0.0f, 1.0f };
    }

    EtcObjEllipse::EtcObjEllipse() : wire() {
        radius = 1.0f;
        slices = 8;
        stacks = 8;
        pos[0] = { 0.0f, 0.0f, 0.0f };
        pos[1] = { 0.0f, 0.0f, 1.0f };
    }

    EtcObjCylinder::EtcObjCylinder() : wire() {
        base = 1.0f;
        top = 1.0f;
        height = 1.0f;
        slices = 8;
        stacks = 8;
    }

    EtcObj::Data::Data() : capsule() {

    }

    EtcObj::EtcObj(EtcObjType type) : count(), offset() {
        this->type = type;
        color = 0xFFFFFFFF;
        //fog = false;
        constant = false;
        switch (type) {
        case ETC_OBJ_TEAPOT:
            data.teapot = {};
            break;
        case ETC_OBJ_GRID:
            data.grid = {};
            break;
        case ETC_OBJ_CUBE:
            data.cube = {};
            break;
        case ETC_OBJ_SPHERE:
            data.sphere = {};
            break;
        case ETC_OBJ_PLANE:
            data.plane = {};
            break;
        case ETC_OBJ_CONE:
            data.cone = {};
            break;
        case ETC_OBJ_LINE:
            data.line = {};
            break;
        case ETC_OBJ_CROSS:
            data.cross = {};
            break;
        case ETC_OBJ_CAPSULE: // Added
            data.capsule = {};
            break;
        case ETC_OBJ_ELLIPSE: // Added
            data.ellipse = {};
            break;
        case ETC_OBJ_CYLINDER: // Added
            data.cylinder = {};
            break;
        }
    }

    EtcObj::~EtcObj() {

    }

    UserArgs::UserArgs() : func(), data() {

    }

    ObjTranslucentArgs::ObjTranslucentArgs() : count(), sub_mesh() {

    }

    ObjData::Args::Args() : sub_mesh() {

    }

    ObjData::Args::~Args() {

    }

    ObjData::ObjData() : kind(), view_z(), radius() {

    }

    ObjData::~ObjData() {

    }

    void ObjData::init_etc(DispManager* disp_manager, const mat4* mat, mdl::EtcObj* etc) {
        kind = mdl::OBJ_KIND_ETC;
        this->mat = *mat;
        args.etc = *etc;

        disp_manager->add_vertex_array(&args.etc, this->mat);
    }

    void ObjData::init_sub_mesh(DispManager* disp_manager, const mat4* mat, float_t radius,
        const obj_sub_mesh* sub_mesh, const obj_mesh* mesh,
        const obj_material_data* material, const std::vector<GLuint>* textures,
        int32_t mat_count, const mat4* mats, /*GLuint vertex_buffer, GLuint index_buffer,*/
        obj_mesh_vertex_buffer* vertex_buffer, obj_mesh_index_buffer* index_buffer, const vec4* blend_color,
        const vec4* emission, /*GLuint morph_vertex_buffer,*/ obj_mesh_vertex_buffer* morph_vertex_buffer,
        int32_t instances_count, const mat4* instances_mat, void(*func)(const ObjSubMeshArgs*)) {
        kind = mdl::OBJ_KIND_NORMAL;
        this->mat = *mat;
        this->radius = radius;

        mdl::ObjSubMeshArgs* args = &this->args.sub_mesh;
        args->mesh = mesh;
        args->morph_weight = disp_manager->morph.weight;
        args->material = material;
        args->sub_mesh = sub_mesh;
        args->textures = textures;
        args->mat_count = mat_count;
        args->mats = mats;
        args->vertex_buffer = vertex_buffer;
        args->index_buffer = index_buffer;
        args->morph_vertex_buffer = morph_vertex_buffer;

        args->texture_pattern_count = disp_manager->texture_pattern_count;
        for (int32_t i = 0; i < disp_manager->texture_pattern_count && i < TEXTURE_PATTERN_COUNT; i++)
            args->texture_pattern_array[i] = disp_manager->texture_pattern_array[i];

        args->texture_transform_count = disp_manager->texture_transform_count;
        for (int32_t i = 0; i < disp_manager->texture_transform_count && i < TEXTURE_TRANSFORM_COUNT; i++)
            args->texture_transform_array[i] = disp_manager->texture_transform_array[i];

        if (blend_color && *blend_color != 1.0f) {
            args->set_blend_color = true;
            args->blend_color = *blend_color;
        }
        else {
            args->set_blend_color = false;
            args->blend_color = 1.0f;
        }

        args->emission = *emission;

        args->chara_color = disp_manager->chara_color;
        args->self_shadow = disp_manager->obj_flags & (mdl::OBJ_8 | mdl::OBJ_4) ? 1 : 0;
        args->shadow = disp_manager->shadow_type;
        args->texture_color_coefficients = disp_manager->texture_color_coefficients;
        args->texture_color_coefficients.w = disp_manager->wet_param;
        args->texture_color_offset = disp_manager->texture_color_offset;
        args->texture_specular_coefficients = disp_manager->texture_specular_coefficients;
        args->texture_specular_offset = disp_manager->texture_specular_offset;
        args->instances_count = instances_count;
        args->instances_mat = instances_mat;
        args->func = func;

        disp_manager->add_vertex_array(args);
    }

    void ObjData::init_translucent(const mat4* mat, ObjTranslucentArgs* translucent) {
        kind = mdl::OBJ_KIND_TRANSLUCENT;
        this->mat = *mat;
        args.translucent = *translucent;
    }

    void ObjData::init_user(const mat4* mat, UserArgsFunc func, void* data) {
        kind = OBJ_KIND_USER;
        this->mat = *mat;
        args.user.func = func;
        args.user.data = data;
    }

    DispManager::DispManager() : obj_flags(), shadow_type(), field_8(), field_C(),
        culling(), show_alpha_center(), show_mat_center(),  texture_pattern_count(),
        texture_pattern_array(), wet_param(), texture_transform_count(),
        texture_transform_array(), material_list_count(), material_list_array() {
        put_index = -1;
        object_culling = true;
        object_sort = true;
        chara_color = true;
        buff_offset = 0;
        buff_max = 0;
        buff_size = 0x300000;
        buff = force_malloc(0x300000);
        texture_color_coefficients = 1.0f;
        texture_color_offset = 0.0f;
        texture_specular_coefficients = 1.0f;
        texture_specular_offset = 0.0f;
    }

    DispManager::~DispManager() {
        for (DispManager::vertex_array& i : vertex_array_cache)
            glDeleteVertexArrays(1, &i.vertex_array);
        vertex_array_cache.clear();

        for (DispManager::etc_vertex_array& i : etc_vertex_array_cache) {
            glDeleteVertexArrays(1, &i.vertex_array);

            i.vertex_buffer.Destroy();
            i.index_buffer.Destroy();
        }
        etc_vertex_array_cache.clear();

        free_def(buff);
    }

    void DispManager::add_vertex_array(ObjSubMeshArgs* args) {
        const obj_mesh* mesh = args->mesh;
        const obj_sub_mesh* sub_mesh = args->sub_mesh;
        const obj_material_data* material = args->material;

        //GLuint vertex_buffer = args->vertex_buffer;
        obj_mesh_vertex_buffer* vertex_buffer = args->vertex_buffer;
        //GLuint morph_vertex_buffer = args->morph_vertex_buffer;
        obj_mesh_vertex_buffer* morph_vertex_buffer = args->morph_vertex_buffer;
        //GLuint index_buffer = args->index_buffer;
        obj_mesh_index_buffer* index_buffer = args->index_buffer;

        int32_t texcoord_array[2] = { -1, -1 };
        int32_t color_tex_index = 0;
        for (const obj_material_texture_data& i : material->material.texdata) {
            if (i.tex_index == -1)
                continue;

            int32_t texcoord_index = obj_material_texture_type_get_texcoord_index(
                i.shader_info.m.tex_type, color_tex_index);
            if (texcoord_index < 0)
                continue;

            texcoord_array[texcoord_index] = sub_mesh->uv_index[&i - material->material.texdata];

            if (i.shader_info.m.tex_type == OBJ_MATERIAL_TEXTURE_COLOR)
                color_tex_index++;
        }

        GLuint vertex_attrib_buffer_binding[16] = {};
        object_data_get_vertex_attrib_buffer_bindings(args,
            texcoord_array, vertex_attrib_buffer_binding);

        bool compressed = mesh->attrib.m.compressed;
        GLsizei size_vertex = (GLsizei)mesh->size_vertex;
        obj_vertex_format vertex_format = mesh->vertex_format;

        DispManager::vertex_array* vertex_array = 0;
        for (DispManager::vertex_array& i : vertex_array_cache)
            if (i.alive_time >= 0 && i.vertex_buffer == vertex_buffer
                && i.morph_vertex_buffer == morph_vertex_buffer
                && i.index_buffer == index_buffer && i.vertex_format == vertex_format
                && i.size_vertex == size_vertex && i.compressed == compressed
                && !memcmp(i.vertex_attrib_buffer_binding,
                    vertex_attrib_buffer_binding, sizeof(vertex_attrib_buffer_binding))
                && !memcmp(i.texcoord_array, texcoord_array, sizeof(texcoord_array)))
                if (i.vertex_array) {
                    i.alive_time = 60;
                    return;
                }
                else {
                    vertex_array = &i;
                    break;
                }

        if (!vertex_array)
            for (DispManager::vertex_array& i : vertex_array_cache)
                if (i.alive_time <= 0) {
                    vertex_array = &i;
                    break;
                }

        if (!vertex_array) {
            vertex_array_cache.push_back({});
            vertex_array = &vertex_array_cache.back();
        }

        if (!vertex_array->vertex_array) {
            glGenVertexArrays(1, &vertex_array->vertex_array);

            gl_state_bind_vertex_array(vertex_array->vertex_array);
            glVertexAttrib4f(       POSITION_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f(    BONE_WEIGHT_INDEX, 0.0f, 0.0f, 0.0f, 0.0f);
            glVertexAttrib4f(         NORMAL_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f(         COLOR0_INDEX, 1.0f, 1.0f, 1.0f, 1.0f);
            glVertexAttrib4f(         COLOR1_INDEX, 1.0f, 1.0f, 1.0f, 1.0f);
            glVertexAttrib4f(    MORPH_COLOR_INDEX, 1.0f, 1.0f, 1.0f, 1.0f);
            glVertexAttrib4f(        TANGENT_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f(        UNKNOWN_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f(      TEXCOORD0_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f(      TEXCOORD1_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f( MORPH_POSITION_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f(   MORPH_NORMAL_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f(  MORPH_TANGENT_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f(MORPH_TEXCOORD0_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f(MORPH_TEXCOORD1_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f(     BONE_INDEX_INDEX, 0.0f, 0.0f, 0.0f, 0.0f);
        }

        vertex_array->vertex_buffer = vertex_buffer;
        vertex_array->morph_vertex_buffer = morph_vertex_buffer;
        vertex_array->index_buffer = index_buffer;
        vertex_array->alive_time = 60;
        vertex_array->vertex_format = vertex_format;
        vertex_array->size_vertex = size_vertex;
        vertex_array->compressed = compressed;
        memcpy(&vertex_array->texcoord_array, texcoord_array, sizeof(texcoord_array));
        memcpy(&vertex_array->vertex_attrib_buffer_binding,
            vertex_attrib_buffer_binding, sizeof(vertex_attrib_buffer_binding));

        gl_state_bind_vertex_array(vertex_array->vertex_array);
        //gl_state_bind_array_buffer(vertex_buffer, true);
        gl_state_bind_array_buffer(vertex_buffer->get_buffer(), true);
        if (index_buffer)
            //gl_state_bind_element_array_buffer(index_buffer, true);
            gl_state_bind_element_array_buffer(index_buffer->buffer, true);

#if SHARED_OBJECT_BUFFER
        //size_t offset = vertex_buffer_offset;
        size_t offset = vertex_buffer->get_offset();
#else
        size_t offset = 0;
#endif
        if (vertex_format & OBJ_VERTEX_POSITION) {
            if (!vertex_array->vertex_attrib_array[POSITION_INDEX]) {
                glEnableVertexAttribArray(POSITION_INDEX);
                vertex_array->vertex_attrib_array[POSITION_INDEX] = true;
            }

            glVertexAttribPointer(POSITION_INDEX,
                3, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
            offset += 12;
        }
        else if (vertex_array->vertex_attrib_array[POSITION_INDEX]) {
            glDisableVertexAttribArray(POSITION_INDEX);
            vertex_array->vertex_attrib_array[POSITION_INDEX] = false;
        }

        if (vertex_format & OBJ_VERTEX_NORMAL) {
            if (!vertex_array->vertex_attrib_array[NORMAL_INDEX]) {
                glEnableVertexAttribArray(NORMAL_INDEX);
                vertex_array->vertex_attrib_array[NORMAL_INDEX] = true;
            }

            if (!compressed) {
                glVertexAttribPointer(NORMAL_INDEX,
                    3, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                offset += 12;
            }
            else {
                glVertexAttribPointer(NORMAL_INDEX,
                    3, GL_SHORT, GL_TRUE, size_vertex, (void*)offset);
                offset += 8;
            }
        }
        else if (vertex_array->vertex_attrib_array[NORMAL_INDEX]) {
            glDisableVertexAttribArray(NORMAL_INDEX);
            vertex_array->vertex_attrib_array[NORMAL_INDEX] = false;
        }

        if (vertex_format & OBJ_VERTEX_TANGENT) {
            if (!vertex_array->vertex_attrib_array[TANGENT_INDEX]) {
                glEnableVertexAttribArray(TANGENT_INDEX);
                vertex_array->vertex_attrib_array[TANGENT_INDEX] = true;
            }

            if (!compressed) {
                glVertexAttribPointer(TANGENT_INDEX,
                    4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                offset += 16;
            }
            else {
                glVertexAttribPointer(TANGENT_INDEX,
                    4, GL_SHORT, GL_TRUE, size_vertex, (void*)offset);
                offset += 8;
            }
        }
        else if (vertex_array->vertex_attrib_array[TANGENT_INDEX]) {
            glDisableVertexAttribArray(TANGENT_INDEX);
            vertex_array->vertex_attrib_array[TANGENT_INDEX] = false;
        }

        if (!compressed && vertex_format & OBJ_VERTEX_BINORMAL)
            offset += 12;

        for (int32_t i = 0; i < 2; i++) {
            int32_t texcoord_index = texcoord_array[i];
            if (texcoord_index < 0) {
                if (vertex_array->vertex_attrib_array[TEXCOORD0_INDEX + i]) {
                    glDisableVertexAttribArray(TEXCOORD0_INDEX + i);
                    vertex_array->vertex_attrib_array[TEXCOORD0_INDEX + i] = false;
                }
                continue;
            }

            if (vertex_format & (OBJ_VERTEX_TEXCOORD0 << texcoord_index)) {
                if (!vertex_array->vertex_attrib_array[TEXCOORD0_INDEX + i]) {
                    glEnableVertexAttribArray(TEXCOORD0_INDEX + i);
                    vertex_array->vertex_attrib_array[TEXCOORD0_INDEX + i] = true;
                }

                if (!compressed)
                    glVertexAttribPointer(TEXCOORD0_INDEX + i, 2, GL_FLOAT, GL_FALSE,
                        size_vertex, (void*)(offset + 8ULL * texcoord_index));
                else
                    glVertexAttribPointer(TEXCOORD0_INDEX + i, 2, GL_HALF_FLOAT, GL_FALSE,
                        size_vertex, (void*)(offset + 4ULL * texcoord_index));
            }
        }

        if (!compressed) {
            if (vertex_format & OBJ_VERTEX_TEXCOORD0)
                offset += 8;
            if (vertex_format & OBJ_VERTEX_TEXCOORD1)
                offset += 8;
            if (vertex_format & OBJ_VERTEX_TEXCOORD2)
                offset += 8;
            if (vertex_format & OBJ_VERTEX_TEXCOORD3)
                offset += 8;
        }
        else {
            if (vertex_format & OBJ_VERTEX_TEXCOORD0)
                offset += 4;
            if (vertex_format & OBJ_VERTEX_TEXCOORD1)
                offset += 4;
            if (vertex_format & OBJ_VERTEX_TEXCOORD2)
                offset += 4;
            if (vertex_format & OBJ_VERTEX_TEXCOORD3)
                offset += 4;
        }

        if (vertex_format & OBJ_VERTEX_COLOR0) {
            if (!vertex_array->vertex_attrib_array[COLOR0_INDEX]) {
                glEnableVertexAttribArray(COLOR0_INDEX);
                vertex_array->vertex_attrib_array[COLOR0_INDEX] = true;
            }

            if (!compressed) {
                glVertexAttribPointer(COLOR0_INDEX,
                    4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                offset += 16;
            }
            else {
                glVertexAttribPointer(COLOR0_INDEX,
                    4, GL_HALF_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                offset += 8;
            }
        }
        else if (vertex_array->vertex_attrib_array[COLOR0_INDEX]) {
            glDisableVertexAttribArray(COLOR0_INDEX);
            vertex_array->vertex_attrib_array[COLOR0_INDEX] = false;
        }

        if (vertex_format & OBJ_VERTEX_COLOR1)
            offset += 16;

        if (vertex_format & OBJ_VERTEX_BONE_DATA) {
            if (!vertex_array->vertex_attrib_array[BONE_WEIGHT_INDEX]) {
                glEnableVertexAttribArray(BONE_WEIGHT_INDEX);
                vertex_array->vertex_attrib_array[BONE_WEIGHT_INDEX] = true;
            }

            if (!compressed) {
                glVertexAttribPointer(BONE_WEIGHT_INDEX,
                    4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                offset += 16;
            }
            else {
                glVertexAttribPointer(BONE_WEIGHT_INDEX,
                    4, GL_UNSIGNED_SHORT, GL_TRUE, size_vertex, (void*)offset);
                offset += 8;
            }

            if (!vertex_array->vertex_attrib_array[BONE_INDEX_INDEX]) {
                glEnableVertexAttribArray(BONE_INDEX_INDEX);
                vertex_array->vertex_attrib_array[BONE_INDEX_INDEX] = true;
            }

            glVertexAttribIPointer(BONE_INDEX_INDEX,
                4, GL_SHORT, size_vertex, (void*)offset);
            offset += 8;
        }
        else {
            if (vertex_array->vertex_attrib_array[BONE_WEIGHT_INDEX]) {
                glDisableVertexAttribArray(BONE_WEIGHT_INDEX);
                vertex_array->vertex_attrib_array[BONE_WEIGHT_INDEX] = false;
            }

            if (vertex_array->vertex_attrib_array[BONE_INDEX_INDEX]) {
                glDisableVertexAttribArray(BONE_INDEX_INDEX);
                vertex_array->vertex_attrib_array[BONE_INDEX_INDEX] = false;
            }
        }

        if (!compressed && vertex_format & OBJ_VERTEX_UNKNOWN) {
            if (!vertex_array->vertex_attrib_array[UNKNOWN_INDEX]) {
                glEnableVertexAttribArray(UNKNOWN_INDEX);
                vertex_array->vertex_attrib_array[UNKNOWN_INDEX] = true;
            }

            glVertexAttribPointer(UNKNOWN_INDEX,
                4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
            offset += 16;
        }
        else if (vertex_array->vertex_attrib_array[UNKNOWN_INDEX]) {
            glDisableVertexAttribArray(UNKNOWN_INDEX);
            vertex_array->vertex_attrib_array[UNKNOWN_INDEX] = false;
        }

        if (morph_vertex_buffer) {
            //gl_state_bind_array_buffer(morph_vertex_buffer, true);
            gl_state_bind_array_buffer(morph_vertex_buffer->get_buffer(), true);

#if SHARED_OBJECT_BUFFER
            //size_t offset = morph_vertex_buffer_offset;
            size_t offset = morph_vertex_buffer->get_offset();
#else
            size_t offset = 0;
#endif
            if (vertex_format & OBJ_VERTEX_POSITION) {
                if (!vertex_array->vertex_attrib_array[MORPH_POSITION_INDEX]) {
                    glEnableVertexAttribArray(MORPH_POSITION_INDEX);
                    vertex_array->vertex_attrib_array[MORPH_POSITION_INDEX] = true;
                }

                glVertexAttribPointer(MORPH_POSITION_INDEX,
                    3, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                offset += 12;
            }
            else if (vertex_array->vertex_attrib_array[MORPH_POSITION_INDEX]) {
                glDisableVertexAttribArray(MORPH_POSITION_INDEX);
                vertex_array->vertex_attrib_array[MORPH_POSITION_INDEX] = false;
            }

            if (vertex_format & OBJ_VERTEX_NORMAL) {
                if (!vertex_array->vertex_attrib_array[MORPH_NORMAL_INDEX]) {
                    glEnableVertexAttribArray(MORPH_NORMAL_INDEX);
                    vertex_array->vertex_attrib_array[MORPH_NORMAL_INDEX] = true;
                }

                if (!compressed) {
                    glVertexAttribPointer(MORPH_NORMAL_INDEX,
                        3, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                    offset += 12;
                }
                else {
                    glVertexAttribPointer(MORPH_NORMAL_INDEX,
                        3, GL_SHORT, GL_TRUE, size_vertex, (void*)offset);
                    offset += 8;
                }
            }
            else if (vertex_array->vertex_attrib_array[MORPH_NORMAL_INDEX]) {
                glDisableVertexAttribArray(MORPH_NORMAL_INDEX);
                vertex_array->vertex_attrib_array[MORPH_NORMAL_INDEX] = false;
            }

            if (vertex_format & OBJ_VERTEX_TANGENT) {
                if (!vertex_array->vertex_attrib_array[MORPH_TANGENT_INDEX]) {
                    glEnableVertexAttribArray(MORPH_TANGENT_INDEX);
                    vertex_array->vertex_attrib_array[MORPH_TANGENT_INDEX] = true;
                }

                if (!compressed) {
                    glVertexAttribPointer(MORPH_TANGENT_INDEX,
                        4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                    offset += 16;
                }
                else {
                    glVertexAttribPointer(MORPH_TANGENT_INDEX,
                        4, GL_SHORT, GL_TRUE, size_vertex, (void*)offset);
                    offset += 8;
                }
            }
            else if (vertex_array->vertex_attrib_array[MORPH_TANGENT_INDEX]) {
                glDisableVertexAttribArray(MORPH_TANGENT_INDEX);
                vertex_array->vertex_attrib_array[MORPH_TANGENT_INDEX] = false;
            }

            if (!compressed && vertex_format & OBJ_VERTEX_BINORMAL)
                offset += 12;

            if (vertex_format & OBJ_VERTEX_TEXCOORD0) {
                if (!vertex_array->vertex_attrib_array[MORPH_TEXCOORD0_INDEX]) {
                    glEnableVertexAttribArray(MORPH_TEXCOORD0_INDEX);
                    vertex_array->vertex_attrib_array[MORPH_TEXCOORD0_INDEX] = true;
                }

                if (!compressed) {
                    glVertexAttribPointer(MORPH_TEXCOORD0_INDEX,
                        2, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                    offset += 8;
                }
                else {
                    glVertexAttribPointer(MORPH_TEXCOORD0_INDEX,
                        2, GL_HALF_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                    offset += 4;
                }
            }
            else if (vertex_array->vertex_attrib_array[MORPH_TEXCOORD0_INDEX]) {
                glDisableVertexAttribArray(MORPH_TEXCOORD0_INDEX);
                vertex_array->vertex_attrib_array[MORPH_TEXCOORD0_INDEX] = false;
            }

            if (vertex_format & OBJ_VERTEX_TEXCOORD1) {
                if (!vertex_array->vertex_attrib_array[MORPH_TEXCOORD1_INDEX]) {
                    glEnableVertexAttribArray(MORPH_TEXCOORD1_INDEX);
                    vertex_array->vertex_attrib_array[MORPH_TEXCOORD1_INDEX] = true;
                }

                if (!compressed) {
                    glVertexAttribPointer(MORPH_TEXCOORD1_INDEX,
                        2, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                    offset += 8;
                }
                else {
                    glVertexAttribPointer(MORPH_TEXCOORD1_INDEX,
                        2, GL_HALF_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                    offset += 4;
                }
            }
            else if (vertex_array->vertex_attrib_array[MORPH_TEXCOORD1_INDEX]) {
                glDisableVertexAttribArray(MORPH_TEXCOORD1_INDEX);
                vertex_array->vertex_attrib_array[MORPH_TEXCOORD1_INDEX] = false;
            }

            if (!compressed) {
                if (vertex_format & OBJ_VERTEX_TEXCOORD2)
                    offset += 8;
                if (vertex_format & OBJ_VERTEX_TEXCOORD3)
                    offset += 8;
            }
            else {
                if (vertex_format & OBJ_VERTEX_TEXCOORD2)
                    offset += 4;
                if (vertex_format & OBJ_VERTEX_TEXCOORD3)
                    offset += 4;
            }

            if (vertex_format & OBJ_VERTEX_COLOR0) {
                uniform_value[U_MORPH_COLOR] = 1;

                if (!vertex_array->vertex_attrib_array[MORPH_COLOR_INDEX]) {
                    glEnableVertexAttribArray(MORPH_COLOR_INDEX);
                    vertex_array->vertex_attrib_array[MORPH_COLOR_INDEX] = true;
                }

                if (!compressed) {
                    glVertexAttribPointer(MORPH_COLOR_INDEX,
                        4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                    offset += 16;
                }
                else {
                    glVertexAttribPointer(MORPH_COLOR_INDEX,
                        4, GL_HALF_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                    offset += 8;
                }
            }
            else if (vertex_array->vertex_attrib_array[MORPH_COLOR_INDEX]) {
                glDisableVertexAttribArray(MORPH_COLOR_INDEX);
                vertex_array->vertex_attrib_array[MORPH_COLOR_INDEX] = false;
            }

            if (!compressed) {
                if (vertex_format & OBJ_VERTEX_COLOR1)
                    offset += 16;

                if (vertex_format & OBJ_VERTEX_BONE_DATA)
                    offset += 32;

                if (vertex_format & OBJ_VERTEX_UNKNOWN)
                    offset += 16;
            }
            else {
                if (vertex_format & OBJ_VERTEX_BONE_DATA)
                    offset += 16;
            }
        }
        else {
            if (vertex_array->vertex_attrib_array[MORPH_POSITION_INDEX]) {
                glDisableVertexAttribArray(MORPH_POSITION_INDEX);
                vertex_array->vertex_attrib_array[MORPH_POSITION_INDEX] = false;
            }

            if (vertex_array->vertex_attrib_array[MORPH_NORMAL_INDEX]) {
                glDisableVertexAttribArray(MORPH_NORMAL_INDEX);
                vertex_array->vertex_attrib_array[MORPH_NORMAL_INDEX] = false;
            }

            if (vertex_array->vertex_attrib_array[MORPH_TANGENT_INDEX]) {
                glDisableVertexAttribArray(MORPH_TANGENT_INDEX);
                vertex_array->vertex_attrib_array[MORPH_TANGENT_INDEX] = false;
            }

            if (vertex_array->vertex_attrib_array[MORPH_TEXCOORD0_INDEX]) {
                glDisableVertexAttribArray(MORPH_TEXCOORD0_INDEX);
                vertex_array->vertex_attrib_array[MORPH_TEXCOORD0_INDEX] = false;
            }

            if (vertex_array->vertex_attrib_array[MORPH_TEXCOORD1_INDEX]) {
                glDisableVertexAttribArray(MORPH_TEXCOORD1_INDEX);
                vertex_array->vertex_attrib_array[MORPH_TEXCOORD1_INDEX] = false;
            }

            if (vertex_array->vertex_attrib_array[MORPH_COLOR_INDEX]) {
                glDisableVertexAttribArray(MORPH_COLOR_INDEX);
                vertex_array->vertex_attrib_array[MORPH_COLOR_INDEX] = false;
            }
        }

        gl_state_bind_array_buffer(0);
        gl_state_bind_vertex_array(0);
        if (index_buffer)
            gl_state_bind_element_array_buffer(0);
    }

    static void gen_cube_vertices(std::vector<float_t>& data) {
        data.resize(sizeof(vec3) * 2 * 4 * 6);

        vec3* vtx = (vec3*)data.data();
        *vtx++ = { -1.0f, -1.0f, -1.0f };
        *vtx++ = {  0.0f,  0.0f, -1.0f };
        *vtx++ = {  1.0f,  1.0f, -1.0f };
        *vtx++ = {  0.0f,  0.0f, -1.0f };
        *vtx++ = {  1.0f, -1.0f, -1.0f };
        *vtx++ = {  0.0f,  0.0f, -1.0f };
        *vtx++ = { -1.0f,  1.0f, -1.0f };
        *vtx++ = {  0.0f,  0.0f, -1.0f };

        *vtx++ = { -1.0f, -1.0f,  1.0f };
        *vtx++ = {  0.0f,  0.0f,  1.0f };
        *vtx++ = {  1.0f, -1.0f,  1.0f };
        *vtx++ = {  0.0f,  0.0f,  1.0f };
        *vtx++ = {  1.0f,  1.0f,  1.0f };
        *vtx++ = {  0.0f,  0.0f,  1.0f };
        *vtx++ = { -1.0f,  1.0f,  1.0f };
        *vtx++ = {  0.0f,  0.0f,  1.0f };

        *vtx++ = { -1.0f,  1.0f,  1.0f };
        *vtx++ = { -1.0f,  0.0f,  0.0f };
        *vtx++ = { -1.0f,  1.0f, -1.0f };
        *vtx++ = { -1.0f,  0.0f,  0.0f };
        *vtx++ = { -1.0f, -1.0f, -1.0f };
        *vtx++ = { -1.0f,  0.0f,  0.0f };
        *vtx++ = { -1.0f, -1.0f,  1.0f };
        *vtx++ = { -1.0f,  0.0f,  0.0f };

        *vtx++ = {  1.0f,  1.0f, -1.0f };
        *vtx++ = {  1.0f,  0.0f,  0.0f };
        *vtx++ = {  1.0f,  1.0f,  1.0f };
        *vtx++ = {  1.0f,  0.0f,  0.0f };
        *vtx++ = {  1.0f, -1.0f, -1.0f };
        *vtx++ = {  1.0f,  0.0f,  0.0f };
        *vtx++ = {  1.0f, -1.0f,  1.0f };
        *vtx++ = {  1.0f,  0.0f,  0.0f };

        *vtx++ = { -1.0f, -1.0f, -1.0f };
        *vtx++ = {  0.0f, -1.0f,  0.0f };
        *vtx++ = {  1.0f, -1.0f, -1.0f };
        *vtx++ = {  0.0f, -1.0f,  0.0f };
        *vtx++ = {  1.0f, -1.0f,  1.0f };
        *vtx++ = {  0.0f, -1.0f,  0.0f };
        *vtx++ = { -1.0f, -1.0f,  1.0f };
        *vtx++ = {  0.0f, -1.0f,  0.0f };

        *vtx++ = { -1.0f,  1.0f, -1.0f };
        *vtx++ = {  0.0f,  1.0f,  0.0f };
        *vtx++ = {  1.0f,  1.0f,  1.0f };
        *vtx++ = {  0.0f,  1.0f,  0.0f };
        *vtx++ = {  1.0f,  1.0f, -1.0f };
        *vtx++ = {  0.0f,  1.0f,  0.0f };
        *vtx++ = { -1.0f,  1.0f,  1.0f };
        *vtx++ = {  0.0f,  1.0f,  0.0f };
    }

    static size_t gen_cube_indices(std::vector<uint32_t>& indices) {
        const uint32_t sides_indices[] = {
             0,  1,  2,  0,  3,  1,
             4,  5,  6,  6,  7,  4,
             8,  9, 10, 10, 11,  8,
            12, 13, 14, 15, 14, 13,
            16, 17, 18, 18, 19, 16,
            20, 21, 22, 20, 23, 21,
        };

        const uint32_t edges_indices[] = {
            0, 2, 1, 2, 0, 3, 1, 3,
            4, 5, 6, 5, 4, 7, 6, 7,
            0, 4, 1, 6, 2, 5, 3, 7,
        };

        indices.insert(indices.end(), sides_indices,
            sides_indices + sizeof(sides_indices) / sizeof(uint32_t));

        size_t wire_offset = indices.size();

        indices.insert(indices.end(), edges_indices,
            edges_indices + sizeof(edges_indices) / sizeof(uint32_t));

        return wire_offset;
    }

    static void gen_sphere_vertices(std::vector<float_t>& data,
        int32_t slices, int32_t stacks, float_t radius) {
        if (slices < 2 || stacks < 2)
            return;

        data.reserve(sizeof(vec3) * 2);

        data.push_back(0.0f);
        data.push_back(radius);
        data.push_back(0.0f);

        data.push_back(0.0f);
        data.push_back(1.0f);
        data.push_back(0.0f);

        double_t slice_step = (M_PI * 2.0) / (double_t)slices;
        double_t stack_step = M_PI / (double_t)stacks;

        for (int32_t i = 1; i < stacks; i++) {
            float_t stack_angle = (float_t)((M_PI / 2.0) - (double_t)i * stack_step);
            float_t xz = cosf(stack_angle);
            float_t y = sinf(stack_angle);

            data.reserve(sizeof(vec3) * 2 * slices);

            for (int32_t j = 0; j < slices; j++) {
                float_t slice_angle = (float_t)((double_t)j * slice_step);

                float_t x = xz * cosf(slice_angle);
                float_t z = xz * sinf(slice_angle);

                data.push_back(x * radius);
                data.push_back(y * radius);
                data.push_back(z * radius);

                data.push_back(x);
                data.push_back(y);
                data.push_back(z);
            }
        }

        data.reserve(sizeof(vec3) * 2);

        data.push_back(0.0f);
        data.push_back(-radius);
        data.push_back(0.0f);

        data.push_back(0.0f);
        data.push_back(-1.0f);
        data.push_back(0.0f);
    }

    static size_t gen_sphere_indices(std::vector<uint32_t>& indices,
        int32_t slices, int32_t stacks) {
        if (slices < 2 || stacks < 2)
            return 0;

        // Top stack vertices
        {
            int32_t m1 = 0;
            int32_t m2 = 1;

            indices.reserve(3LL * slices);

            for (int32_t j = 0, k = 1; j < slices; j++) {
                indices.push_back(m1);
                indices.push_back(k + m2);
                indices.push_back(j + m2);

                if (++k >= slices)
                    k = 0;
            }
        }

        // Middle stacks vertices
        for (int32_t i = 1; i < stacks - 1; i++) {
            int32_t m1 = (i - 1) * slices + 1;
            int32_t m2 = m1 + slices;

            indices.reserve(6LL * slices);

            for (int32_t j = 0, k = 1; j < slices; j++) {
                indices.push_back(j + m1);
                indices.push_back(k + m1);
                indices.push_back(j + m2);

                indices.push_back(k + m1);
                indices.push_back(k + m2);
                indices.push_back(j + m2);

                if (++k >= slices)
                    k = 0;
            }
        }

        // Bottom stack vertices
        {
            int32_t m1 = (stacks - 2) * slices + 1;
            int32_t m2 = m1 + slices;

            indices.reserve(3LL * slices);

            for (int32_t j = 0, k = 1; j < slices; j++) {
                indices.push_back(j + m1);
                indices.push_back(k + m1);
                indices.push_back(m2);

                if (++k >= slices)
                    k = 0;
            }
        }

        size_t wire_offset = indices.size();

        // Top stack wireframe
        {
            int32_t m1 = 0;
            int32_t m2 = 1;

            indices.reserve(2LL * slices);

            for (int32_t j = 0; j < slices; j++) {
                indices.push_back(m1);
                indices.push_back(j + m2);
            }
        }

        // Middle stacks wireframe
        for (int32_t i = 1; i < stacks - 1; i++) {
            int32_t m1 = (i - 1) * slices + 1;
            int32_t m2 = m1 + slices;

            indices.reserve(2LL * slices);

            for (int32_t j = 0; j < slices; j++) {
                indices.push_back(j + m1);
                indices.push_back(j + m2);
            }
        }

        // Bottom stack wireframe
        {
            int32_t m1 = (stacks - 2) * slices + 1;
            int32_t m2 = m1 + slices;

            indices.reserve(2LL * slices);

            for (int32_t j = 0; j < slices; j++) {
                indices.push_back(j + m1);
                indices.push_back(m2);
            }
        }

        // Slices wireframe
        for (int32_t i = 1; i < stacks; i++) {
            int32_t m = (i - 1) * slices + 1;

            indices.reserve(2LL * slices);

            for (int32_t j = 0, k = 1; j < slices; j++) {
                indices.push_back(j + m);
                indices.push_back(k + m);

                if (k++ >= slices)
                    k = 0;
            }
        }

        return wire_offset;
    }

    static void gen_capsule_vertices(std::vector<float_t>& data,
        int32_t slices, int32_t stacks, float_t length, float_t radius) {
        if (slices < 2 || stacks < 2)
            return;

        stacks = ((stacks + 1) >> 1) << 1;

        if (length < 0.00001f) {
            gen_sphere_vertices(data, slices, stacks, radius);
            return;
        }

        length *= 0.5f;

        data.reserve(sizeof(vec3) * 2);

        data.push_back(0.0f);
        data.push_back(radius + length);
        data.push_back(0.0f);

        data.push_back(0.0f);
        data.push_back(1.0f);
        data.push_back(0.0f);

        double_t slice_step = (M_PI * 2.0) / (double_t)slices;
        double_t stack_step = M_PI / (double_t)stacks;

        for (int32_t i = 1; i <= stacks / 2; i++) {
            float_t stack_angle = (float_t)((M_PI / 2.0) - (double_t)i * stack_step);
            float_t xz = cosf(stack_angle);
            float_t y = sinf(stack_angle);

            data.reserve(sizeof(vec3) * 2 * slices);

            for (int32_t j = 0; j < slices; j++) {
                float_t slice_angle = (float_t)((double_t)j * slice_step);

                float_t x = xz * cosf(slice_angle);
                float_t z = xz * sinf(slice_angle);

                data.push_back(x * radius);
                data.push_back(y + length);
                data.push_back(z * radius);

                data.push_back(x);
                data.push_back(y);
                data.push_back(z);
            }
        }

        for (int32_t i = stacks / 2; i < stacks; i++) {
            float_t stack_angle = (float_t)((M_PI / 2.0) - (double_t)i * stack_step);
            float_t xz = cosf(stack_angle);
            float_t y = sinf(stack_angle);

            data.reserve(sizeof(vec3) * 2 * slices);

            for (int32_t j = 0; j < slices; j++) {
                float_t slice_angle = (float_t)((double_t)j * slice_step);

                float_t x = xz * cosf(slice_angle);
                float_t z = xz * sinf(slice_angle);

                data.push_back(x * radius);
                data.push_back(y - length);
                data.push_back(z * radius);

                data.push_back(x);
                data.push_back(y);
                data.push_back(z);
            }
        }

        data.reserve(sizeof(vec3) * 2);

        data.push_back(0.0f);
        data.push_back(-radius - length);
        data.push_back(0.0f);

        data.push_back(0.0f);
        data.push_back(-1.0f);
        data.push_back(0.0f);
    }

    static size_t gen_capsule_indices(std::vector<uint32_t>& indices,
        int32_t slices, int32_t stacks, float_t length) {
        if (slices < 2 || stacks < 2)
            return 0;

        stacks = ((stacks + 1) >> 1) << 1;

        if (length < 0.00001f)
            return gen_sphere_indices(indices, slices, stacks);

        // Top stack vertices
        {
            int32_t m1 = 0;
            int32_t m2 = 1;

            indices.reserve(3LL * slices);

            for (int32_t j = 0, k = 1; j < slices; j++) {
                indices.push_back(m1);
                indices.push_back(k + m2);
                indices.push_back(j + m2);

                if (++k >= slices)
                    k = 0;
            }
        }

        // Middle stacks vertices
        for (int32_t i = 1; i < stacks; i++) {
            int32_t m1 = (i - 1) * slices + 1;
            int32_t m2 = m1 + slices;

            indices.reserve(6LL * slices);

            for (int32_t j = 0, k = 1; j < slices; j++) {
                indices.push_back(j + m1);
                indices.push_back(k + m1);
                indices.push_back(j + m2);

                indices.push_back(k + m1);
                indices.push_back(k + m2);
                indices.push_back(j + m2);

                if (++k >= slices)
                    k = 0;
            }
        }

        // Bottom stack vertices
        {
            int32_t m1 = (stacks - 1) * slices + 1;
            int32_t m2 = m1 + slices;

            indices.reserve(3LL * slices);

            for (int32_t j = 0, k = 1; j < slices; j++) {
                indices.push_back(j + m1);
                indices.push_back(k + m1);
                indices.push_back(m2);

                if (++k >= slices)
                    k = 0;
            }
        }

        size_t wire_offset = indices.size();

        // Top stack wireframe
        {
            int32_t m1 = 0;
            int32_t m2 = 1;

            indices.reserve(2LL * slices);

            for (int32_t j = 0; j < slices; j++) {
                indices.push_back(m1);
                indices.push_back(j + m2);
            }
        }

        // Middle stacks wireframe
        for (int32_t i = 1; i < stacks; i++) {
            int32_t m1 = (i - 1) * slices + 1;
            int32_t m2 = m1 + slices;

            indices.reserve(2LL * slices);

            for (int32_t j = 0; j < slices; j++) {
                indices.push_back(j + m1);
                indices.push_back(j + m2);
            }
        }

        // Bottom stack wireframe
        {
            int32_t m1 = (stacks - 1) * slices + 1;
            int32_t m2 = m1 + slices;

            indices.reserve(2LL * slices);

            for (int32_t j = 0; j < slices; j++) {
                indices.push_back(j + m1);
                indices.push_back(m2);
            }
        }

        // Slices wireframe
        for (int32_t i = 1; i <= stacks; i++) {
            int32_t m = (i - 1) * slices + 1;

            indices.reserve(2LL * slices);

            for (int32_t j = 0, k = 1; j < slices; j++) {
                indices.push_back(j + m);
                indices.push_back(k + m);

                if (k++ >= slices)
                    k = 0;
            }
        }

        return wire_offset;
    }

    static void gen_ellipse_vertices(std::vector<float_t>& data,
        int32_t slices, int32_t stacks, float_t length, float_t radius) {
        if (slices < 2 || stacks < 2)
            return;

        stacks = ((stacks + 1) >> 1) << 1;

        if (length < 0.00001f) {
            gen_sphere_vertices(data, slices, stacks, radius);
            return;
        }

        length *= 0.5f;

        data.reserve(sizeof(vec3) * 2);

        data.push_back(0.0f);
        data.push_back(radius + length);
        data.push_back(0.0f);

        data.push_back(0.0f);
        data.push_back(1.0f);
        data.push_back(0.0f);

        double_t slice_step = (M_PI * 2.0) / (double_t)slices;
        double_t stack_step = M_PI / (double_t)stacks;

        for (int32_t i = 1; i < stacks; i++) {
            float_t stack_angle = (float_t)((M_PI / 2.0) - (double_t)i * stack_step);
            float_t xz = cosf(stack_angle);
            float_t y = sinf(stack_angle);
            float_t y_n = y;
            y *= radius + length;

            data.reserve(sizeof(vec3) * 2 * slices);

            for (int32_t j = 0; j < slices; j++) {
                float_t slice_angle = (float_t)((double_t)j * slice_step);

                float_t x = xz * cosf(slice_angle);
                float_t z = xz * sinf(slice_angle);

                data.push_back(x * radius);
                data.push_back(y);
                data.push_back(z * radius);

                data.push_back(x);
                data.push_back(y_n);
                data.push_back(z);
            }
        }

        data.reserve(sizeof(vec3) * 2);

        data.push_back(0.0f);
        data.push_back(-radius - length);
        data.push_back(0.0f);

        data.push_back(0.0f);
        data.push_back(-1.0f);
        data.push_back(0.0f);
    }

    static size_t gen_ellipse_indices(std::vector<uint32_t>& indices,
        int32_t slices, int32_t stacks, float_t length) {
        stacks = ((stacks + 1) >> 1) << 1;

        return gen_sphere_indices(indices, slices, stacks);
    }

    static void gen_cylinder_vertices(std::vector<float_t>& data,
        int32_t slices, int32_t stacks, float_t base, float_t top, float_t height) {
        float_t half_height = height * 0.5f;

        if (slices < 2 || stacks < 0)
            return;

        data.reserve(sizeof(vec3) * 2);

        data.push_back(0.0f);
        data.push_back(half_height);
        data.push_back(0.0f);

        data.push_back(0.0f);
        data.push_back(1.0f);
        data.push_back(0.0f);

        double_t slice_step = (M_PI * 2.0) / (double_t)slices;
        double_t stack_step = M_PI / (double_t)stacks;

        for (int32_t i = 0; i <= stacks; i++) {
            float_t y = lerp_def(half_height, -half_height, (float_t)i / (float_t)stacks);
            float_t radius = lerp_def(top, base, (float_t)i / (float_t)stacks);

            data.reserve(sizeof(vec3) * 2 * slices);

            for (int32_t j = 0; j < slices; j++) {
                float_t slice_angle = (float_t)((double_t)j * slice_step);

                float_t x = cosf(slice_angle);
                float_t z = sinf(slice_angle);

                data.push_back(x * radius);
                data.push_back(y);
                data.push_back(z * radius);

                data.push_back(x);
                data.push_back(0.0f);
                data.push_back(z);
            }
        }

        data.reserve(sizeof(vec3) * 2);

        data.push_back(0.0f);
        data.push_back(-half_height);
        data.push_back(0.0f);

        data.push_back(0.0f);
        data.push_back(-1.0f);
        data.push_back(0.0f);
    }

    static size_t gen_cylinder_indices(std::vector<uint32_t>& indices,
        int32_t slices, int32_t stacks) {
        if (slices < 2 || stacks < 0)
            return 0;

        // Top cap vertices
        {
            int32_t m1 = 0;
            int32_t m2 = 1;

            indices.reserve(3LL * slices);

            for (int32_t j = 0, k = 1; j < slices; j++) {
                indices.push_back(m1);
                indices.push_back(k + m2);
                indices.push_back(j + m2);

                if (++k >= slices)
                    k = 0;
            }
        }

        // Stacks vertices
        for (int32_t i = 0; i < stacks; i++) {
            int32_t m1 = i * slices + 1;
            int32_t m2 = m1 + slices;

            indices.reserve(6LL * slices);

            for (int32_t j = 0, k = 1; j < slices; j++) {
                indices.push_back(j + m1);
                indices.push_back(k + m1);
                indices.push_back(j + m2);

                indices.push_back(k + m1);
                indices.push_back(k + m2);
                indices.push_back(j + m2);

                if (++k >= slices)
                    k = 0;
            }
        }

        // Bottom cap vertices
        {
            int32_t m1 = stacks * slices + 1;
            int32_t m2 = m1 + slices;

            indices.reserve(3LL * slices);

            for (int32_t j = 0, k = 1; j < slices; j++) {
                indices.push_back(j + m1);
                indices.push_back(k + m1);
                indices.push_back(m2);

                if (++k >= slices)
                    k = 0;
            }
        }

        size_t wire_offset = indices.size();

        // Stacks wireframe
        for (int32_t i = 0; i < stacks; i++) {
            int32_t m1 = i * slices + 1;
            int32_t m2 = m1 + slices;

            indices.reserve(2LL * slices);

            for (int32_t j = 0; j < slices; j++) {
                indices.push_back(j + m1);
                indices.push_back(j + m2);
            }
        }

        // Slices wireframe
        for (int32_t i = 0; i <= stacks; i++) {
            int32_t m = i * slices + 1;

            indices.reserve(2LL * slices);

            for (int32_t j = 0, k = 1; j < slices; j++) {
                indices.push_back(j + m);
                indices.push_back(k + m);

                if (k++ >= slices)
                    k = 0;
            }
        }

        return wire_offset;
    }

    void DispManager::add_vertex_array(EtcObj* etc, mat4& mat) {
        EtcObjType type = etc->type;
        switch (type) {
        case mdl::ETC_OBJ_TEAPOT:
        case mdl::ETC_OBJ_GRID:
        case mdl::ETC_OBJ_CUBE:
        case mdl::ETC_OBJ_SPHERE:
        case mdl::ETC_OBJ_PLANE:
        case mdl::ETC_OBJ_CONE:
        case mdl::ETC_OBJ_LINE:
        case mdl::ETC_OBJ_CROSS:
        case mdl::ETC_OBJ_CAPSULE: // Added
        case mdl::ETC_OBJ_ELLIPSE: // Added
        case mdl::ETC_OBJ_CYLINDER: // Added
            break;
        default:
            return;
        }

        bool indexed = false;
        bool wire = false;
        float_t length = 0.0f;

        switch (type) {
        case mdl::ETC_OBJ_TEAPOT: {
            EtcObjTeapot& teapot = etc->data.teapot;

            indexed = true;
        } break;
        case mdl::ETC_OBJ_GRID: {
            EtcObjGrid& grid = etc->data.grid;

        } break;
        case mdl::ETC_OBJ_CUBE: {
            EtcObjCube& cube = etc->data.cube;

            vec3 size = cube.size * 0.5f;
            mat4_scale_rot(&mat, &size, &mat);

            indexed = true;
            wire = cube.wire;
        } break;
        case mdl::ETC_OBJ_SPHERE: {
            EtcObjSphere& sphere = etc->data.sphere;

            indexed = true;
            wire = sphere.wire;
        } break;
        case mdl::ETC_OBJ_PLANE: {
            EtcObjPlane& plane = etc->data.plane;

            mat4_scale_rot(&mat, (float_t)plane.w * 0.5f, 1.0f, (float_t)plane.h * 0.5f, &mat);
        } break;
        case mdl::ETC_OBJ_CONE: {
            EtcObjCone& cone = etc->data.cone;

            indexed = true;
            wire = cone.wire;
        } break;
        case mdl::ETC_OBJ_LINE: {
            EtcObjLine& line = etc->data.line;

        } break;
        case mdl::ETC_OBJ_CROSS: {
            EtcObjCross& cross = etc->data.cross;

            mat4_scale_rot(&mat, cross.size, &mat);
        } break;
        case mdl::ETC_OBJ_CAPSULE: { // Added
            EtcObjCapsule& capsule = etc->data.capsule;

            vec3 origin = (capsule.pos[0] + capsule.pos[1]) * 0.5f;
            mat4_add_translate(&mat, &origin, &mat);

            vec3 dir = vec3::normalize(capsule.pos[1] - capsule.pos[0]);
            vec3 up = { 0.0f, 1.0f, 0.0f };
            vec3 axis;
            float_t angle;
            Glitter::axis_angle_from_vectors(&axis, &angle, &up, &dir);

            mat4 m = mat4_identity;
            mat4_mul_rotation(&m, &axis, angle, &m);
            mat4_mul(&m, &mat, &mat);

            indexed = true;
            wire = capsule.wire;
            length = vec3::distance(capsule.pos[0], capsule.pos[1]);
        } break;
        case mdl::ETC_OBJ_ELLIPSE: { // Added
            EtcObjEllipse& ellipse = etc->data.ellipse;

            vec3 origin = (ellipse.pos[0] + ellipse.pos[1]) * 0.5f;
            mat4_add_translate(&mat, &origin, &mat);

            vec3 dir = vec3::normalize(ellipse.pos[1] - ellipse.pos[0]);
            vec3 up = { 0.0f, 1.0f, 0.0f };
            vec3 axis;
            float_t angle;
            Glitter::axis_angle_from_vectors(&axis, &angle, &up, &dir);

            mat4 m = mat4_identity;
            mat4_mul_rotation(&m, &axis, angle, &m);
            mat4_mul(&m, &mat, &mat);

            indexed = true;
            wire = ellipse.wire;
            length = vec3::distance(ellipse.pos[0], ellipse.pos[1]);
        } break;
        case mdl::ETC_OBJ_CYLINDER: { // Added
            EtcObjCylinder& cylinder = etc->data.cylinder;

            indexed = true;
            wire = cylinder.wire;
        } break;
        }

        DispManager::etc_vertex_array* etc_vertex_array = 0;
        for (DispManager::etc_vertex_array& i : etc_vertex_array_cache) {
            if (i.alive_time <= 0 || i.type != type || i.vertex_buffer.IsNull())
                continue;

            if (type == mdl::ETC_OBJ_TEAPOT
                || type == mdl::ETC_OBJ_GRID
                && !memcmp(&i.data.grid, &etc->data.grid, sizeof(EtcObjGrid))
                || type == mdl::ETC_OBJ_CUBE
                || type == mdl::ETC_OBJ_SPHERE
                && i.data.sphere.slices == etc->data.sphere.slices
                && i.data.sphere.stacks == etc->data.sphere.stacks
                && fabsf(i.data.sphere.radius - etc->data.sphere.radius) < 0.00001f
                || type == mdl::ETC_OBJ_PLANE
                || type == mdl::ETC_OBJ_CONE
                && i.data.cone.slices == etc->data.cone.slices
                && i.data.cone.stacks == etc->data.cone.stacks
                && fabsf(i.data.cone.base - etc->data.cone.base) < 0.00001f
                && fabsf(i.data.cone.height - etc->data.cone.height) < 0.00001f
                || type == mdl::ETC_OBJ_LINE
                || type == mdl::ETC_OBJ_CROSS
                || type == mdl::ETC_OBJ_CAPSULE // Added
                && i.data.capsule.slices == etc->data.capsule.slices
                && ((i.data.capsule.stacks + 1)) >> 1 == ((etc->data.capsule.stacks + 1) >> 1)
                && fabsf(i.data.capsule.radius - etc->data.capsule.radius) < 0.00001f
                && fabsf(vec3::distance(i.data.capsule.pos[0], i.data.capsule.pos[1]) - length) < 0.00001f
                || type == mdl::ETC_OBJ_ELLIPSE // Added
                && i.data.ellipse.slices == etc->data.ellipse.slices
                && ((i.data.ellipse.stacks + 1)) >> 1 == ((etc->data.ellipse.stacks + 1) >> 1)
                && fabsf(i.data.ellipse.radius - etc->data.ellipse.radius) < 0.00001f
                && fabsf(vec3::distance(i.data.ellipse.pos[0], i.data.ellipse.pos[1]) - length) < 0.00001f
                || type == mdl::ETC_OBJ_CYLINDER // Added
                && i.data.cylinder.slices == etc->data.cylinder.slices
                && i.data.cylinder.stacks == etc->data.cylinder.stacks
                && fabsf(i.data.cylinder.base - etc->data.cylinder.base) < 0.00001f
                && fabsf(i.data.cylinder.top - etc->data.cylinder.top) < 0.00001f
                && fabsf(i.data.cylinder.height - etc->data.cylinder.height) < 0.00001f)
                if (i.vertex_array) {
                    i.alive_time = 2;
                    if (!wire) {
                        etc->count = i.count;
                        etc->offset = i.offset;
                    }
                    else {
                        etc->count = i.wire_count;
                        etc->offset = i.wire_offset;
                    }
                    return;
                }
                else {
                    etc_vertex_array = &i;
                    break;
                }
        }

        if (!etc_vertex_array)
            for (DispManager::etc_vertex_array& i : etc_vertex_array_cache)
                if (i.alive_time <= 0) {
                    etc_vertex_array = &i;
                    break;
                }

        if (!etc_vertex_array) {
            etc_vertex_array_cache.push_back({});
            etc_vertex_array = &etc_vertex_array_cache.back();
        }

        if (!etc_vertex_array->vertex_array) {
            glGenVertexArrays(1, &etc_vertex_array->vertex_array);

            gl_state_bind_vertex_array(etc_vertex_array->vertex_array);
            glVertexAttrib4f(       POSITION_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f(    BONE_WEIGHT_INDEX, 0.0f, 0.0f, 0.0f, 0.0f);
            glVertexAttrib4f(         NORMAL_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f(         COLOR0_INDEX, 1.0f, 1.0f, 1.0f, 1.0f);
            glVertexAttrib4f(         COLOR1_INDEX, 1.0f, 1.0f, 1.0f, 1.0f);
            glVertexAttrib4f(    MORPH_COLOR_INDEX, 1.0f, 1.0f, 1.0f, 1.0f);
            glVertexAttrib4f(        TANGENT_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f(        UNKNOWN_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f(      TEXCOORD0_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f(      TEXCOORD1_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f( MORPH_POSITION_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f(   MORPH_NORMAL_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f(  MORPH_TANGENT_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f(MORPH_TEXCOORD0_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f(MORPH_TEXCOORD1_INDEX, 0.0f, 0.0f, 0.0f, 1.0f);
            glVertexAttrib4f(     BONE_INDEX_INDEX, 0.0f, 0.0f, 0.0f, 0.0f);
        }

        etc_vertex_array->alive_time = 2;
        etc_vertex_array->data = etc->data;
        etc_vertex_array->type = type;

        std::vector<float_t> vtx_data;
        std::vector<uint32_t> vtx_indices;
        switch (type) {
        case mdl::ETC_OBJ_TEAPOT: {
            EtcObjTeapot& teapot = etc->data.teapot;

            etc_vertex_array->count = (GLsizei)vtx_indices.size();
        } break;
        case mdl::ETC_OBJ_GRID: {
            EtcObjGrid& grid = etc->data.grid;

            etc_vertex_array->count = (GLsizei)(vtx_data.size() / 6);
        } break;
        case mdl::ETC_OBJ_CUBE: {
            EtcObjCube& cube = etc->data.cube;

            gen_cube_vertices(vtx_data);
            size_t wire_offset = gen_cube_indices(vtx_indices);

            etc_vertex_array->offset = 0;
            etc_vertex_array->count = (GLsizei)wire_offset;
            etc_vertex_array->wire_offset = wire_offset * sizeof(uint32_t);
            etc_vertex_array->wire_count = (GLsizei)(vtx_indices.size() - wire_offset);
        } break;
        case mdl::ETC_OBJ_SPHERE: {
            EtcObjSphere& sphere = etc->data.sphere;

            gen_sphere_vertices(vtx_data, sphere.slices, sphere.stacks, sphere.radius);
            size_t wire_offset = gen_sphere_indices(vtx_indices, sphere.slices, sphere.stacks);

            etc_vertex_array->offset = 0;
            etc_vertex_array->count = (GLsizei)wire_offset;
            etc_vertex_array->wire_offset = wire_offset * sizeof(uint32_t);
            etc_vertex_array->wire_count = (GLsizei)(vtx_indices.size() - wire_offset);
        } break;
        case mdl::ETC_OBJ_PLANE: {
            EtcObjPlane& plane = etc->data.plane;

            etc_vertex_array->count = (GLsizei)(vtx_data.size() / 6);
        } break;
        case mdl::ETC_OBJ_CONE: {
            EtcObjCone& cone = etc->data.cone;

            etc_vertex_array->count = (GLsizei)vtx_indices.size();
        } break;
        case mdl::ETC_OBJ_LINE: {
            EtcObjLine& line = etc->data.line;

            etc_vertex_array->count = (GLsizei)(vtx_data.size() / 6);
        } break;
        case mdl::ETC_OBJ_CROSS: {
            EtcObjCross& cross = etc->data.cross;

            etc_vertex_array->count = (GLsizei)(vtx_data.size() / 6);
        } break;
        case mdl::ETC_OBJ_CAPSULE: { // Added
            EtcObjCapsule& capsule = etc->data.capsule;

            gen_capsule_vertices(vtx_data, capsule.slices, capsule.stacks, length, capsule.radius);
            size_t wire_offset = gen_capsule_indices(vtx_indices, capsule.slices, capsule.stacks, length);

            etc_vertex_array->offset = 0;
            etc_vertex_array->count = (GLsizei)wire_offset;
            etc_vertex_array->wire_offset = wire_offset * sizeof(uint32_t);
            etc_vertex_array->wire_count = (GLsizei)(vtx_indices.size() - wire_offset);
        } break;
        case mdl::ETC_OBJ_ELLIPSE: { // Added
            EtcObjEllipse& ellipse = etc->data.ellipse;

            gen_ellipse_vertices(vtx_data, ellipse.slices, ellipse.stacks, length, ellipse.radius);
            size_t wire_offset = gen_ellipse_indices(vtx_indices, ellipse.slices, ellipse.stacks, length);

            etc_vertex_array->offset = 0;
            etc_vertex_array->count = (GLsizei)wire_offset;
            etc_vertex_array->wire_offset = wire_offset * sizeof(uint32_t);
            etc_vertex_array->wire_count = (GLsizei)(vtx_indices.size() - wire_offset);
        } break;
        case mdl::ETC_OBJ_CYLINDER: { // Added
            EtcObjCylinder& cylinder = etc->data.cylinder;

            gen_cylinder_vertices(vtx_data, cylinder.slices, cylinder.stacks,
                cylinder.base, cylinder.top, cylinder.height);
            size_t wire_offset = gen_cylinder_indices(vtx_indices, cylinder.slices, cylinder.stacks);

            etc_vertex_array->offset = 0;
            etc_vertex_array->count = (GLsizei)wire_offset;
            etc_vertex_array->wire_offset = wire_offset * sizeof(uint32_t);
            etc_vertex_array->wire_count = (GLsizei)(vtx_indices.size() - wire_offset);
        } break;
        }

        if (!wire) {
            etc->count = etc_vertex_array->count;
            etc->offset = etc_vertex_array->offset;
        }
        else {
            etc->count = etc_vertex_array->wire_count;
            etc->offset = etc_vertex_array->wire_offset;
        }

        if (!etc_vertex_array->count)
            return;

        if (etc_vertex_array->max_vtx < vtx_data.size()) {
            etc_vertex_array->max_vtx = vtx_data.size();

            etc_vertex_array->vertex_buffer.Destroy();
        }

        if (etc_vertex_array->max_idx < vtx_indices.size()) {
            etc_vertex_array->max_idx = vtx_indices.size();

            etc_vertex_array->index_buffer.Destroy();
        }

        GLsizei size_vertex = sizeof(vec3) * 2;

        gl_state_bind_vertex_array(etc_vertex_array->vertex_array);

        if (etc_vertex_array->vertex_buffer.IsNull())
            etc_vertex_array->vertex_buffer.Create(sizeof(float_t) * vtx_data.size());

        etc_vertex_array->vertex_buffer.Bind(true);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float_t)
            * vtx_data.size(), vtx_data.data());

        if (indexed) {
            if (etc_vertex_array->index_buffer.IsNull())
                etc_vertex_array->index_buffer.Create(sizeof(uint32_t) * vtx_indices.size());

            etc_vertex_array->index_buffer.Bind(true);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint32_t)
                * vtx_indices.size(), vtx_indices.data());
        }

        glEnableVertexAttribArray(POSITION_INDEX);
        glVertexAttribPointer(POSITION_INDEX,
            3, GL_FLOAT, GL_FALSE, size_vertex, (void*)0);

        glEnableVertexAttribArray(NORMAL_INDEX);
        glVertexAttribPointer(NORMAL_INDEX,
            3, GL_FLOAT, GL_FALSE, size_vertex, (void*)sizeof(vec3));

        gl_state_bind_array_buffer(0);
        gl_state_bind_vertex_array(0);
        if (indexed)
            gl_state_bind_element_array_buffer(0);
    }

    void* DispManager::alloc_data(int32_t size) {
        if (!buff || buff_offset + size > buff_size)
            return 0;

        void* data = (void*)((size_t)buff + buff_offset);
        buff_offset += size;
        buff_max = max_def(buff_max, buff_offset);
        return data;
    }

    ObjData* DispManager::alloc_obj_data(ObjKind kind) {
        int32_t size = (int32_t)align_val(sizeof(ObjKind) + sizeof(mat4) + sizeof(float_t) * 2, 0x08);
        switch (kind) {
        case OBJ_KIND_NORMAL:
            return (ObjData*)alloc_data(size + sizeof(ObjSubMeshArgs));
        case OBJ_KIND_ETC:
            return (ObjData*)alloc_data(size + sizeof(EtcObj));
        case OBJ_KIND_USER:
            return (ObjData*)alloc_data(size + sizeof(UserArgs));
        case OBJ_KIND_TRANSLUCENT:
            return (ObjData*)alloc_data(size + sizeof(ObjTranslucentArgs));
        default:
            return 0;
        }
    }

    mat4* DispManager::alloc_mat4_array(int32_t count) {
        return (mat4*)alloc_data(sizeof(mat4) * count);
    }

    void DispManager::buffer_reset() {
        buff_offset = 0;
    }

    void DispManager::calc_obj_radius(const mat4* view, mdl::ObjType type) {
        std::vector<vec3> alpha_center;
        std::vector<vec3> mat_center;

        for (ObjData*& i : obj[type]) {
            vec3 center = 0.0f;
            bool v50 = false;
            switch (i->kind) {
            case OBJ_KIND_NORMAL: {
                mat4 mat = i->mat;
                if (i->args.sub_mesh.mesh->attrib.m.billboard)
                    model_mat_face_camera_view(view, &mat, &mat);
                else if (i->args.sub_mesh.mesh->attrib.m.billboard_y_axis)
                    model_mat_face_camera_position(view, &mat, &mat);

                get_obj_center(mat, &i->args.sub_mesh, center);

                v50 = !!i->args.sub_mesh.material->material.attrib.m.flag_30;
                i->radius = i->args.sub_mesh.mesh->bounding_sphere.radius;
            } break;
            case OBJ_KIND_ETC:
            case OBJ_KIND_USER:
                mat4_get_translation(&i->mat, &center);
                break;
            case OBJ_KIND_TRANSLUCENT:
                vec3 center_sum = 0.0f;
                for (int32_t j = 0; j < i->args.translucent.count; j++) {
                    vec3 _center = 0.0f;
                    get_obj_center(i->mat, i->args.translucent.sub_mesh[j], _center);
                    if (i->args.translucent.sub_mesh[j]->material->material.attrib.m.flag_30) {
                        v50 = true;
                        break;
                    }

                    center_sum += _center;
                }

                if (!v50)
                    center = center_sum * (1.0f / (float_t)i->args.translucent.count);
                break;
            }

            if (show_alpha_center && type == OBJ_TYPE_TRANSLUCENT)
                alpha_center.push_back(center);
            if (show_mat_center && type == OBJ_TYPE_TRANSLUCENT && v50)
                mat_center.push_back(center);

            mat4_transform_point(view, &center, &center);
            i->view_z = center.z;
        }

        if (show_alpha_center && type == OBJ_TYPE_TRANSLUCENT)
            for (vec3& i : alpha_center) {
                mdl::EtcObj etc(ETC_OBJ_SPHERE);
                etc.color = { 0xFF, 0x00, 0x00, 0xFF };
                etc.data.sphere.radius = 0.05f;
                etc.data.sphere.slices = 8;
                etc.data.sphere.stacks = 8;
                etc.data.sphere.wire = false;

                mat4 mat;
                mat4_translate(&i, &mat);
                mdl::DispManager::entry_obj_etc(&mat, &etc);
            }

        if (show_mat_center && type == OBJ_TYPE_TRANSLUCENT)
            for (vec3& i : alpha_center) {
                mdl::EtcObj etc(ETC_OBJ_SPHERE);
                etc.color = { 0x00, 0x00, 0xFF, 0xFF };
                etc.data.sphere.radius = 0.05f;
                etc.data.sphere.slices = 8;
                etc.data.sphere.stacks = 8;
                etc.data.sphere.wire = false;

                mat4 mat;
                mat4_translate(&i, &mat);
                mdl::DispManager::entry_obj_etc(&mat, &etc);
            }
    }

    void DispManager::check_vertex_arrays() {
        for (DispManager::vertex_array& i : vertex_array_cache)
            if (i.alive_time > 0 && --i.alive_time <= 0) {
                i.vertex_buffer = 0;
                i.morph_vertex_buffer = 0;
            }

        for (DispManager::etc_vertex_array& i : etc_vertex_array_cache)
            if (i.alive_time > 0 && --i.alive_time <= 0) {
                gl_state_bind_vertex_array(i.vertex_array);
                glDisableVertexAttribArray(POSITION_INDEX);
                glDisableVertexAttribArray(  NORMAL_INDEX);
                gl_state_bind_array_buffer(0, true);
                gl_state_bind_element_array_buffer(0, true);
                gl_state_bind_vertex_array(0);
            }
    }

    void DispManager::draw(ObjType type, int32_t depth_mask, bool a4) {
        if (get_obj_count(type) < 1)
            return;

        render_context* rctx = rctx_ptr;

        int32_t alpha_test = 0;
        float_t min_alpha = 1.0f;
        float_t alpha_threshold = 0.0f;
        bool reflect = uniform_value[U_REFLECT] == 1;
        void(*func)(render_context * rctx, const ObjSubMeshArgs * args) = draw_sub_mesh_default;

        for (int32_t i = 0; i < 5; i++)
            gl_state_active_bind_texture_2d(i, rctx->empty_texture_2d);
        gl_state_active_bind_texture_cube_map(5, rctx->empty_texture_cube_map);
        gl_state_active_texture(0);
        gl_state_set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        uniform_value_reset();
        gl_state_get();

        rctx->obj_scene_ubo.Bind(0);
        rctx->obj_batch_ubo.Bind(1);
        rctx->obj_skinning_ubo.Bind(3);

        switch (type) {
        case OBJ_TYPE_TRANSLUCENT:
        case OBJ_TYPE_TRANSLUCENT_NO_SHADOW:
        case OBJ_TYPE_TRANSLUCENT_LOCAL:
            if (depth_mask)
                func = draw_sub_mesh_translucent;
            else
                gl_state_set_depth_mask(GL_FALSE);
            alpha_test = 1;
            min_alpha = 0.0f;
            alpha_threshold = 0.0f;
            break;
        case OBJ_TYPE_TRANSPARENT:
        case OBJ_TYPE_TRANSPARENT_LOCAL:
            alpha_test = 1;
            min_alpha = 0.1f;
            alpha_threshold = 0.5f;
            break;
        case OBJ_TYPE_SHADOW_CHARA:
        case OBJ_TYPE_SHADOW_STAGE:
        case OBJ_TYPE_SHADOW_OBJECT_CHARA:
        case OBJ_TYPE_SHADOW_OBJECT_STAGE:
            func = draw_sub_mesh_shadow;
            break;
        case OBJ_TYPE_TYPE_6:
            func = draw_sub_mesh_translucent;
            gl_state_set_color_mask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
            rctx->draw_state->shader_index = SHADER_FT_SIL;
            break;
        case OBJ_TYPE_TYPE_7:
            func = draw_sub_mesh_translucent;
            gl_state_set_color_mask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
            rctx->draw_state->shader_index = SHADER_FT_SIL;
            alpha_test = 1;
            min_alpha = 0.0f;
            alpha_threshold = 0.99999994f;
            break;
        case OBJ_TYPE_REFLECT_CHARA_OPAQUE:
            gl_state_set_cull_face_mode(GL_FRONT);
            if (reflect)
                func = draw_sub_mesh_reflect;
            else if (rctx->render_manager->reflect_type == STAGE_DATA_REFLECT_REFLECT_MAP)
                func = draw_sub_mesh_reflect_reflect_map;
            break;
        case OBJ_TYPE_REFLECT_CHARA_TRANSLUCENT:
            gl_state_set_cull_face_mode(GL_FRONT);
            if (reflect)
                func = draw_sub_mesh_reflect;
            else if (rctx->render_manager->reflect_type == STAGE_DATA_REFLECT_REFLECT_MAP)
                func = draw_sub_mesh_reflect_reflect_map;
            min_alpha = 0.0f;
            break;
        case OBJ_TYPE_REFLECT_CHARA_TRANSPARENT:
            gl_state_set_cull_face_mode(GL_FRONT);
            if (reflect)
                func = draw_sub_mesh_reflect;
            else if (rctx->render_manager->reflect_type == STAGE_DATA_REFLECT_REFLECT_MAP)
                func = draw_sub_mesh_reflect_reflect_map;
            alpha_test = 1;
            min_alpha = 0.1f;
            alpha_threshold = 0.5f;
            break;
        case OBJ_TYPE_REFLECT_OPAQUE:
            alpha_test = 1;
            if (!a4)
                func = draw_sub_mesh_reflect_reflect_map;
            break;
        case OBJ_TYPE_REFLECT_TRANSLUCENT:
        case OBJ_TYPE_REFRACT_TRANSLUCENT:
            gl_state_set_depth_mask(GL_FALSE);
            min_alpha = 0.0f;
            alpha_threshold = 0.0f;
            break;
        case OBJ_TYPE_REFLECT_TRANSPARENT:
        case OBJ_TYPE_REFRACT_TRANSPARENT:
            alpha_test = 1;
            min_alpha = 0.1f;
            alpha_threshold = 0.0f;
            break;
        case OBJ_TYPE_SSS:
            func = draw_sub_mesh_sss;
            break;
        case OBJ_TYPE_USER:
            func = draw_sub_mesh_translucent;
            break;
        default:
            break;
        }
        rctx->obj_batch.g_max_alpha = { 0.0f, 0.0f, alpha_threshold, min_alpha };
        uniform_value[U_ALPHA_TEST] = alpha_test;

        for (ObjData*& i : obj[type]) {
            switch (i->kind) {
            case OBJ_KIND_NORMAL: {
                draw_sub_mesh(rctx, &i->args.sub_mesh, &i->mat, func);
            } break;
            case OBJ_KIND_ETC: {
                draw_object_model_mat_load(rctx, i->mat);
                draw_etc_obj(rctx, &i->args.etc);
            } break;
            case OBJ_KIND_USER: {
                i->args.user.func(i->args.user.data, &i->mat);
            } break;
            case OBJ_KIND_TRANSLUCENT: {
                for (int32_t j = 0; j < i->args.translucent.count; j++)
                    draw_sub_mesh(rctx, i->args.translucent.sub_mesh[j], &i->mat, func);
            } break;
            }
        }

        switch (type) {
        case OBJ_TYPE_TRANSLUCENT:
        case OBJ_TYPE_TRANSLUCENT_NO_SHADOW:
        case OBJ_TYPE_REFLECT_TRANSLUCENT:
        case OBJ_TYPE_REFRACT_TRANSLUCENT:
            if (!depth_mask)
                gl_state_set_depth_mask(GL_TRUE);
            break;
        case OBJ_TYPE_TYPE_6:
        case OBJ_TYPE_TYPE_7:
            rctx_ptr->draw_state->shader_index = -1;
            gl_state_set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            break;
        case OBJ_TYPE_REFLECT_CHARA_OPAQUE:
        case OBJ_TYPE_REFLECT_CHARA_TRANSLUCENT:
        case OBJ_TYPE_REFLECT_CHARA_TRANSPARENT:
            gl_state_set_cull_face_mode(GL_BACK);
            break;
        }

        uniform_value_reset();
        gl_state_bind_vertex_array(0);
        shader::unbind();
        gl_state_set_blend_func(GL_ONE, GL_ZERO);
        for (int32_t i = 0; i < 5; i++)
            gl_state_bind_sampler(i, 0);
    }

    void DispManager::draw_translucent(ObjType type, int32_t alpha) {
        if (get_obj_count(type) < 1)
            return;

        render_context* rctx = rctx_ptr;

        int32_t alpha_test = 0;
        float_t min_alpha = 1.0f;
        float_t alpha_threshold = 0.0f;
        bool reflect = uniform_value[U_REFLECT] == 1;
        void(*func)(render_context * rctx, const ObjSubMeshArgs * args) = draw_sub_mesh_default;

        for (int32_t i = 0; i < 5; i++)
            gl_state_active_bind_texture_2d(i, rctx->empty_texture_2d);
        gl_state_active_bind_texture_cube_map(5, rctx->empty_texture_cube_map);
        gl_state_active_texture(0);
        gl_state_set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        uniform_value_reset();
        gl_state_get();

        rctx->obj_scene_ubo.Bind(0);
        rctx->obj_batch_ubo.Bind(1);
        rctx->obj_skinning_ubo.Bind(3);

        switch (type) {
        case OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_1:
        case OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_2:
        case OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_3:
        case OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_2_LOCAL:
            gl_state_set_depth_mask(GL_FALSE);
            alpha_test = 1;
            min_alpha = 0.0f;
            alpha_threshold = 0.0f;
            break;
        case OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_1:
        case OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_2:
        case OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_3:
        case OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_2_LOCAL:
            alpha_test = 1;
            min_alpha = 0.1f;
            alpha_threshold = 0.5f;
            break;
        }
        rctx->obj_batch.g_max_alpha = { 0.0f, 0.0f, alpha_threshold, min_alpha };
        uniform_value[U_ALPHA_TEST] = alpha_test;

        for (ObjData*& i : obj[type]) {
            switch (i->kind) {
            case OBJ_KIND_NORMAL: {
                int32_t a = (int32_t)(i->args.sub_mesh.blend_color.w * 255.0f);
                a = clamp_def(a, 0, 255);
                if (a == alpha)
                    draw_sub_mesh(rctx, &i->args.sub_mesh, &i->mat, func);
            } break;
            case OBJ_KIND_TRANSLUCENT: {
                for (int32_t j = 0; j < i->args.translucent.count; j++) {
                    ObjSubMeshArgs* args = i->args.translucent.sub_mesh[j];
                    int32_t a = (int32_t)(args->blend_color.w * 255.0f);
                    a = clamp_def(a, 0, 255);
                    if (a == alpha)
                        draw_sub_mesh(rctx, args, &i->mat, func);
                }
            } break;
            }
        }

        switch (type) {
        case OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_1:
        case OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_2:
        case OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_3:
            gl_state_set_depth_mask(GL_TRUE);
            break;
        case OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_1:
        case OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_2:
        case OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_3:
            gl_state_set_cull_face_mode(GL_BACK);
            break;
        }

        uniform_value_reset();
        shader::unbind();
        gl_state_set_blend_func(GL_ONE, GL_ZERO);
    }

    /*void DispManager::draw_show_vector(ObjType type, int32_t show_vector) {
        if (get_obj_count(type) < 1)
            return;

        render_context* rctx = rctx_ptr;

        for (int32_t i = 0; i < 5; i++)
            gl_state_active_bind_texture_2d(i, rctx->empty_texture_2d);
        gl_state_active_bind_texture_cube_map(5, rctx->empty_texture_cube_map);
        gl_state_active_texture(0);
        gl_state_set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        uniform_value_reset();
        gl_state_get();

        for (ObjData*& i : obj[type]) {
            switch (i->type) {
            case OBJ_KIND_NORMAL: {
                draw_sub_mesh_show_vector(rctx, &i->args.sub_mesh,
                    &i->mat, show_vector);
            } break;
            case OBJ_KIND_TRANSLUCENT: {
                for (int32_t j = 0; j < i->args.translucent.count; j++)
                    draw_sub_mesh_show_vector(rctx, i->args.translucent.sub_mesh[j],
                        &i->mat, show_vector);
            } break;
            }
        }
        gl_state_set_blend_func(GL_ONE, GL_ZERO);
    }*/

    void DispManager::entry_list(ObjType type, ObjData* data) {
        obj[type].push_back(data);
    }

    static int32_t obj_axis_aligned_bounding_box_check_visibility(
        obj_axis_aligned_bounding_box* aabb, camera* cam, const mat4* mat) {
        vec3 points[8];
        points[0] = aabb->center + (aabb->size ^ vec3(0.0f, 0.0f, 0.0f));
        points[1] = aabb->center + (aabb->size ^ vec3(-0.0f, -0.0f, -0.0f));
        points[2] = aabb->center + (aabb->size ^ vec3(-0.0f, 0.0f, 0.0f));
        points[3] = aabb->center + (aabb->size ^ vec3(0.0f, -0.0f, -0.0f));
        points[4] = aabb->center + (aabb->size ^ vec3(0.0f, -0.0f, 0.0f));
        points[5] = aabb->center + (aabb->size ^ vec3(-0.0f, 0.0f, -0.0f));
        points[6] = aabb->center + (aabb->size ^ vec3(0.0f, 0.0f, -0.0f));
        points[7] = aabb->center + (aabb->size ^ vec3(-0.0f, -0.0f, 0.0f));

        mat4 view_mat;
        mat4_mul(mat, &cam->view, &view_mat);
        for (int32_t i = 0; i < 8; i++)
            mat4_transform_point(&view_mat, &points[i], &points[i]);

        vec4 v2[6];
        *(vec3*)&v2[0] = { 0.0f, 0.0f, -1.0f };
        v2[0].w = (float_t)-cam->min_distance;
        *(vec3*)&v2[1] = cam->field_1E4;
        v2[1].w = 0.0f;
        *(vec3*)&v2[2] = cam->field_1F0;
        v2[2].w = 0.0f;
        *(vec3*)&v2[3] = cam->field_1FC;
        v2[3].w = 0.0f;
        *(vec3*)&v2[4] = cam->field_208;
        v2[4].w = 0.0f;
        *(vec3*)&v2[5] = { 0.0f, 0.0f, 1.0f };
        v2[5].w = (float_t)cam->max_distance;

        for (int32_t i = 0; i < 6; i++)
            for (int32_t j = 0; j < 8; j++) {
                float_t v34 = vec3::dot(*(vec3*)&v2[i], points[j]) + v2[i].w;
                if (v34 > 0.0f)
                    break;

                if (j == 7)
                    return 0;
            }
        return 1;
    }

    static int32_t obj_bounding_sphere_check_visibility(const obj_bounding_sphere* sphere,
        CullingCheck* culling, camera* cam, const mat4* mat) {
        if (culling->func)
            return culling->func(sphere, &cam->view);

        vec3 center;
        mat4_transform_point(mat, &sphere->center, &center);
        mat4_transform_point(&cam->view, &center, &center);
        float_t radius = mat4_get_max_scale(mat) * sphere->radius;

        double_t min_depth = (double_t)center.z - (double_t)radius;
        double_t max_depth = (double_t)center.z + (double_t)radius;
        if (-cam->min_distance < min_depth || -cam->max_distance > max_depth)
            return 0;

        float_t v5 = vec3::dot(cam->field_1E4, center);
        if (v5 < -radius)
            return 0;

        float_t v6 = vec3::dot(cam->field_1F0, center);
        if (v6 < -radius)
            return 0;

        float_t v7 = vec3::dot(cam->field_1FC, center);
        if (v7 < -radius)
            return 0;

        float_t v8 = vec3::dot(cam->field_208, center);
        if (v8 < -radius)
            return 0;

        if (-cam->min_distance >= max_depth && -cam->max_distance <= min_depth
            && v5 >= radius && v6 >= radius && v7 >= radius && v8 >= radius)
            return 1;
        return 2;
    }

    bool DispManager::entry_obj(const ::obj* object, obj_mesh_vertex_buffer* obj_vertex_buf,
        obj_mesh_index_buffer* obj_index_buf, const mat4* mat,
        const std::vector<GLuint>* textures, const vec4* blend_color, const mat4* bone_mat,
        const ::obj* object_morph, obj_mesh_vertex_buffer* obj_morph_vertex_buf, int32_t instances_count,
        const mat4* instances_mat, void(*func)(const ObjSubMeshArgs*), bool enable_bone_mat, bool local) {
        if (!obj_vertex_buf || !obj_index_buf) {
            printf_debug("mdl::DispManager::entry_obj: no vertex or index object buffer to draw;\n");
            printf_debug("    Object: %s\n", object->name);
            return false;
        }

        ::camera* cam = rctx_ptr->camera;
        cam->update_data();

        if (!local && object_culling && !instances_count && !bone_mat && (!object
            || !obj_bounding_sphere_check_visibility(
                &object->bounding_sphere, &culling, cam, mat))) {
            culling.culled.objects++;
            return false;
        }
        culling.passed.objects++;

        for (uint32_t i = 0; i < object->num_mesh; i++) {
            obj_mesh* mesh = &object->mesh_array[i];
            obj_mesh* mesh_morph = 0;
            if (obj_vertex_buf && obj_morph_vertex_buf) {
                if (obj_vertex_buf[i].get_size() != obj_morph_vertex_buf[i].get_size())
                    continue;

                if (object_morph && i < object_morph->num_mesh)
                    mesh_morph = &object_morph->mesh_array[i];
            }

            if (!local && object_culling && !instances_count && !bone_mat
                && !obj_bounding_sphere_check_visibility(
                    &mesh->bounding_sphere, &culling, cam, mat)
                && (!mesh_morph || !obj_bounding_sphere_check_visibility(
                    &mesh_morph->bounding_sphere, &culling, cam, mat))) {
                culling.culled.meshes++;
                continue;
            }
            culling.passed.meshes++;

            ObjSubMeshArgs* translucent_priority[40];
            int32_t translucent_priority_count = 0;

            for (uint32_t j = 0; j < mesh->num_submesh; j++) {
                obj_sub_mesh* sub_mesh = &mesh->submesh_array[j];
                obj_sub_mesh* sub_mesh_morph = 0;
                if (sub_mesh->attrib.m.cloth)
                    continue;

                if (!local && object_culling && !instances_count && !bone_mat) {
                    int32_t v32 = obj_bounding_sphere_check_visibility(
                        &sub_mesh->bounding_sphere, &culling, cam, mat);
                    if (v32 != 2 || (!mesh->attrib.m.billboard && !mesh->attrib.m.billboard_y_axis)) {
                        if (v32 == 2) {
                            if (culling.func)
                                v32 = 1;
                            else
                                v32 = obj_axis_aligned_bounding_box_check_visibility(
                                    &sub_mesh->axis_aligned_bounding_box, cam, mat);
                        }

                        if (!v32) {
                            if (!mesh_morph || j >= mesh_morph->num_submesh) {
                                culling.culled.submesh_array++;
                                continue;
                            }

                            sub_mesh_morph = &mesh_morph->submesh_array[j];
                            if (!sub_mesh_morph) {
                                culling.culled.submesh_array++;
                                continue;
                            }

                            v32 = obj_bounding_sphere_check_visibility(
                                &sub_mesh_morph->bounding_sphere, &culling, cam, mat);
                            if (v32 == 2) {
                                if (culling.func)
                                    v32 = 1;
                                else
                                    v32 = obj_axis_aligned_bounding_box_check_visibility(
                                        &sub_mesh_morph->axis_aligned_bounding_box, cam, mat);
                            }

                            if (!v32) {
                                culling.culled.submesh_array++;
                                continue;
                            }
                        }
                    }
                }
                culling.passed.submesh_array++;

                int32_t num_bone_index = sub_mesh->num_bone_index;
                uint16_t* bone_index = sub_mesh->bone_index_array;

                mat4* mats;
                if (num_bone_index && enable_bone_mat) {
                    mats = alloc_mat4_array(num_bone_index);
                    if (bone_mat)
                        for (int32_t k = 0; k < num_bone_index; k++, bone_index++)
                            mats[k] = bone_mat[*bone_index];
                    else
                        for (int32_t k = 0; k < num_bone_index; k++)
                            mats[k] = mat4_identity;
                }
                else {
                    mats = 0;
                    num_bone_index = 0;
                }

                obj_material_data* material = &object->material_array[sub_mesh->material_index];
                ObjData* data = alloc_obj_data(OBJ_KIND_NORMAL);
                if (!data)
                    continue;

                //GLuint morph_vertex_buffer = 0;
                //if (obj_morph_vertex_buf)
                //    morph_vertex_buffer = obj_morph_vertex_buf[i].get_buffer();
                obj_mesh_vertex_buffer* morph_vertex_buffer
                    = obj_morph_vertex_buf ? &obj_morph_vertex_buf[i] : 0;

                //GLuint index_buffer = 0;
                //if (obj_index_buf)
                //    index_buffer = obj_index_buf[i].buffer;
                obj_mesh_index_buffer* index_buffer = &obj_index_buf[i];

                //GLuint vertex_buffer = 0;
                //if (obj_vertex_buf)
                //    vertex_buffer = obj_vertex_buf[i].get_buffer();
                obj_mesh_vertex_buffer* vertex_buffer = &obj_vertex_buf[i];

                if (!vertex_buffer || !index_buffer || obj_morph_vertex_buf && !morph_vertex_buffer) {
                    printf_debug("mdl::DispManager::entry_obj: no vertex or index mesh buffer to draw;\n");
                    printf_debug("    Object: %s; Mesh: %s; Sub Mesh: %d\n", object->name, mesh->name, j);
                    continue;
                }

                uint32_t material_hash = hash_utf8_murmurhash(material->material.name);

                material_list_struct* mat_list = 0;
                for (int32_t k = 0; k < material_list_count; k++)
                    if (material_list_array[k].hash == material_hash) {
                        mat_list = &material_list_array[k];
                        break;
                    }

                vec4 _blend_color = 1.0f;
                vec4 _emission = 0.0f;

                if (mat_list) {
                    bool has_blend_color = false;
                    if (mat_list->has_blend_color.x) {
                        _blend_color.x = mat_list->blend_color.x;
                        has_blend_color = true;
                    }

                    if (mat_list->has_blend_color.y) {
                        _blend_color.y = mat_list->blend_color.y;
                        has_blend_color = true;
                    }

                    if (mat_list->has_blend_color.z) {
                        _blend_color.z = mat_list->blend_color.z;
                        has_blend_color = true;
                    }

                    if (mat_list->has_blend_color.w) {
                        _blend_color.w = mat_list->blend_color.w;
                        has_blend_color = true;
                    }

                    if (blend_color) {
                        if (!has_blend_color)
                            _blend_color = *blend_color;
                        else
                            _blend_color *= *blend_color;
                    }

                    bool has_emission = false;
                    if (mat_list->has_emission.x) {
                        _emission.x = mat_list->emission.x;
                        has_emission = true;
                    }

                    if (mat_list->has_emission.y) {
                        _emission.y = mat_list->emission.y;
                        has_emission = true;
                    }

                    if (mat_list->has_emission.z) {
                        _emission.z = mat_list->emission.z;
                        has_emission = true;
                    }

                    if (mat_list->has_emission.w) {
                        _emission.w = mat_list->emission.w;
                        has_emission = true;
                    }

                    if (!has_emission)
                        _emission = material->material.color.emission;
                }
                else {
                    if (blend_color)
                        _blend_color = *blend_color;
                    _emission = material->material.color.emission;
                }

                data->init_sub_mesh(this, mat, object->bounding_sphere.radius, sub_mesh,
                    mesh, material, textures, num_bone_index, mats, vertex_buffer, index_buffer,
                    &_blend_color, &_emission, morph_vertex_buffer, instances_count, instances_mat, func);

                if (obj_flags & mdl::OBJ_SHADOW_OBJECT) {
                    entry_list((ObjType)(OBJ_TYPE_SHADOW_OBJECT_CHARA
                        + shadow_type), data);
                    if (obj_flags & mdl::OBJ_USER)
                        entry_list(OBJ_TYPE_USER, data);
                    continue;
                }

                obj_material_attrib_member attrib = material->material.attrib.m;
                if (obj_flags & (mdl::OBJ_ALPHA_ORDER_1 | mdl::OBJ_ALPHA_ORDER_2 | mdl::OBJ_ALPHA_ORDER_3)
                    && data->args.sub_mesh.blend_color.w < 1.0f) {
                    if (!(obj_flags & mdl::OBJ_NO_TRANSLUCENCY)) {
                        if (attrib.flag_28 || (attrib.punch_through
                            || !(attrib.alpha_texture | attrib.alpha_material))
                            && !sub_mesh->attrib.m.transparent) {
                            if (!attrib.punch_through) {
                                if (obj_flags & mdl::OBJ_ALPHA_ORDER_1)
                                    entry_list(OBJ_TYPE_OPAQUE_ALPHA_ORDER_1, data);
                                else if (obj_flags & mdl::OBJ_ALPHA_ORDER_2)
                                    entry_list(local ? OBJ_TYPE_OPAQUE_ALPHA_ORDER_2_LOCAL
                                        : OBJ_TYPE_OPAQUE_ALPHA_ORDER_2, data);
                                else
                                    entry_list(OBJ_TYPE_OPAQUE_ALPHA_ORDER_3, data);
                            }
                            else {
                                if (obj_flags & mdl::OBJ_ALPHA_ORDER_1)
                                    entry_list(OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_1, data);
                                else if (obj_flags & mdl::OBJ_ALPHA_ORDER_2)
                                    entry_list(local ? OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_2_LOCAL
                                        : OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_2, data);
                                else
                                    entry_list(OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_3, data);
                            }

                            if (obj_flags & mdl::OBJ_SSS)
                                entry_list(OBJ_TYPE_SSS, data);
                        }

                        if (obj_flags & mdl::OBJ_ALPHA_ORDER_1)
                            entry_list(OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_1, data);
                        else if (obj_flags & mdl::OBJ_ALPHA_ORDER_2)
                            entry_list(local ? OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_2_LOCAL
                                : OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_2, data);
                        else
                            entry_list(OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_3, data);
                    }
                }
                else {
                    if (attrib.flag_28 || data->args.sub_mesh.blend_color.w >= 1.0f
                        && (attrib.punch_through || !(attrib.alpha_texture | attrib.alpha_material))
                        && !sub_mesh->attrib.m.transparent) {
                        if (obj_flags & mdl::OBJ_SHADOW)
                            entry_list((ObjType)(OBJ_TYPE_SHADOW_CHARA + shadow_type), data);

                        if (obj_flags & mdl::OBJ_SSS)
                            entry_list(OBJ_TYPE_SSS, data);

                        if (attrib.punch_through) {
                            if (!(obj_flags & mdl::OBJ_NO_TRANSLUCENCY))
                                entry_list(local ? OBJ_TYPE_TRANSPARENT_LOCAL
                                    : OBJ_TYPE_TRANSPARENT, data);

                            if (obj_flags & mdl::OBJ_CHARA_REFLECT)
                                entry_list(OBJ_TYPE_REFLECT_CHARA_OPAQUE, data);

                            if (obj_flags & mdl::OBJ_REFLECT)
                                entry_list(OBJ_TYPE_REFLECT_OPAQUE, data);

                            if (obj_flags & mdl::OBJ_REFRACT)
                                entry_list(OBJ_TYPE_REFRACT_TRANSPARENT, data);
                        }
                        else {
                            if (!(obj_flags & mdl::OBJ_NO_TRANSLUCENCY))
                                entry_list(local ? OBJ_TYPE_OPAQUE_LOCAL
                                    : OBJ_TYPE_OPAQUE, data);

                            if (obj_flags & mdl::OBJ_20)
                                entry_list(OBJ_TYPE_TYPE_6, data);

                            if (obj_flags & mdl::OBJ_CHARA_REFLECT)
                                entry_list(OBJ_TYPE_REFLECT_CHARA_OPAQUE, data);

                            if (obj_flags & mdl::OBJ_REFLECT)
                                entry_list(OBJ_TYPE_REFLECT_OPAQUE, data);

                            if (obj_flags & mdl::OBJ_REFRACT)
                                entry_list(OBJ_TYPE_REFRACT_OPAQUE, data);
                        }

                        if (obj_flags & mdl::OBJ_USER)
                            entry_list(OBJ_TYPE_USER, data);
                        continue;
                    }
                    else if (!(obj_flags & mdl::OBJ_NO_TRANSLUCENCY)) {
                        if (!attrib.translucent_priority)
                            if (local)
                                entry_list(OBJ_TYPE_TRANSLUCENT_LOCAL, data);
                            else if (mesh->attrib.m.translucent_no_shadow
                                || obj_flags & mdl::OBJ_TRANSLUCENT_NO_SHADOW) {
                                entry_list(OBJ_TYPE_TRANSLUCENT_NO_SHADOW, data);
                            }
                            else
                                entry_list(OBJ_TYPE_TRANSLUCENT, data);
                        else if (translucent_priority_count < 40)
                            translucent_priority[translucent_priority_count++] = &data->args.sub_mesh;
                    }
                }

                if (obj_flags & mdl::OBJ_SHADOW)
                    entry_list((ObjType)(OBJ_TYPE_SHADOW_CHARA
                        + shadow_type), data);
                if (obj_flags & mdl::OBJ_40)
                    entry_list(OBJ_TYPE_TYPE_7, data);
                if (obj_flags & mdl::OBJ_CHARA_REFLECT)
                    entry_list(OBJ_TYPE_REFLECT_CHARA_OPAQUE, data);
                if (obj_flags & mdl::OBJ_REFLECT) {
                    if (rctx_ptr->render_manager->reflect_type != STAGE_DATA_REFLECT_REFLECT_MAP)
                        entry_list(OBJ_TYPE_REFLECT_OPAQUE, data);
                    else
                        entry_list(OBJ_TYPE_REFLECT_TRANSLUCENT, data);
                }
                if (obj_flags & mdl::OBJ_REFRACT)
                    entry_list(OBJ_TYPE_REFRACT_TRANSLUCENT, data);
                if (obj_flags & mdl::OBJ_USER)
                    entry_list(OBJ_TYPE_USER, data);
            }

            if (!translucent_priority_count)
                continue;

            ObjTranslucentArgs translucent_args;
            translucent_args.count = 0;
            for (int32_t j = 62; j; j--)
                for (int32_t k = 0; k < translucent_priority_count; k++) {
                    ObjSubMeshArgs* sub_mesh = translucent_priority[k];
                    if (sub_mesh->material->material.attrib.m.translucent_priority != j)
                        continue;

                    translucent_args.sub_mesh[translucent_args.count] = sub_mesh;
                    translucent_args.count++;
                }

            ObjData* data = alloc_obj_data(OBJ_KIND_TRANSLUCENT);
            if (!data)
                continue;

            data->init_translucent(mat, &translucent_args);
            if (obj_flags & mdl::OBJ_ALPHA_ORDER_1)
                entry_list(OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_1, data);
            else if (obj_flags & mdl::OBJ_ALPHA_ORDER_2)
                entry_list(local ? OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_2_LOCAL
                    : OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_2, data);
            else if (obj_flags & mdl::OBJ_ALPHA_ORDER_3)
                entry_list(OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_3, data);
            else
                entry_list(local ? OBJ_TYPE_TRANSLUCENT_LOCAL
                    : OBJ_TYPE_TRANSLUCENT, data);
        }
        return true;
    }

    void DispManager::entry_obj_by_obj(const mat4* mat,
        const ::obj* obj, const std::vector<GLuint>* textures, obj_mesh_vertex_buffer* obj_vert_buf,
        obj_mesh_index_buffer* obj_index_buf, const mat4* bone_mat, float_t alpha) {
        if (!obj)
            return;

        vec4 blend_color = { 1.0f, 1.0f, 1.0f, alpha };
        vec4* blend_color_ptr = alpha < 1.0f ? &blend_color : 0;

        entry_obj(obj, obj_vert_buf, obj_index_buf, mat, textures, blend_color_ptr,
            bone_mat, 0, 0, 0, 0, 0, !!bone_mat);
    }

    bool DispManager::entry_obj_by_object_info(const mat4* mat, object_info obj_info, const mat4* bone_mat) {
        vec4 blend_color = 1.0f;
        return entry_obj_by_object_info(mat, obj_info, &blend_color, bone_mat, 0, 0, 0, true);
    }

    bool DispManager::entry_obj_by_object_info(const mat4* mat, object_info obj_info,
        const vec4* blend_color, const mat4* bone_mat, int32_t instances_count,
        const mat4* instances_mat, void(*func)(const ObjSubMeshArgs*), bool enable_bone_mat, bool local) {
        if (obj_info.id == -1 && obj_info.set_id == -1)
            return false;

        ::obj* object = object_storage_get_obj(obj_info);
        if (!object)
            return false;

        std::vector<GLuint>* textures = object_storage_get_obj_set_textures(obj_info.set_id);
        obj_mesh_vertex_buffer* obj_vertex_buffer = object_storage_get_obj_mesh_vertex_buffer(obj_info);
        obj_mesh_index_buffer* obj_index_buffer = object_storage_get_obj_mesh_index_buffer(obj_info);

        ::obj* obj_morph = 0;
        obj_mesh_vertex_buffer* obj_morph_vertex_buffer = 0;
        if (morph.object.set_id != -1) {
            obj_morph = object_storage_get_obj(morph.object);
            obj_morph_vertex_buffer = object_storage_get_obj_mesh_vertex_buffer(morph.object);
        }

        return entry_obj(object, obj_vertex_buffer, obj_index_buffer,
            mat, textures, blend_color, bone_mat, obj_morph, obj_morph_vertex_buffer,
            instances_count, instances_mat, func, enable_bone_mat, local);
    }

    bool DispManager::entry_obj_by_object_info(const mat4* mat,
        object_info obj_info, float_t alpha, const mat4* bone_mat) {
        vec4 blend_color = 1.0f;
        blend_color.w = alpha;
        return entry_obj_by_object_info(mat, obj_info, &blend_color, bone_mat, 0, 0, 0, true);
    }

    bool DispManager::entry_obj_by_object_info(const mat4* mat, object_info obj_info,
        float_t r, float_t g, float_t b, float_t a, const mat4* bone_mat, bool local) {
        vec4 blend_color = { r, g, b, a };
        return entry_obj_by_object_info(mat, obj_info, &blend_color, bone_mat, 0, 0, 0, true, local);
    }

    bool DispManager::entry_obj_by_object_info(const mat4* mat, object_info obj_info,
        const vec4* blend_color, const mat4* bone_mat, bool local) {
        return entry_obj_by_object_info(mat, obj_info, blend_color, 0, 0, 0, 0, false, local);
    }

    bool mdl::DispManager::entry_obj_by_object_info_instanced(object_info obj_info,
        const std::vector<mat4>& instances, float_t alpha) {
        return entry_obj_by_object_info_instanced(obj_info, instances, 1.0, 1.0, 1.0, alpha);
    }

    bool mdl::DispManager::entry_obj_by_object_info_instanced(object_info obj_info,
        const std::vector<mat4>& instances, float_t r, float_t g, float_t b, float_t a) {
        const vec4 blend_color = { r, g, b, a };
        if (!instances.size())
            return false;

        mat4* instances_mat = alloc_mat4_array((int32_t)instances.size());
        if (!instances_mat)
            return false;

        memmove(instances_mat, instances.data(), sizeof(mat4) * instances.size());
        return entry_obj_by_object_info(&mat4_identity, obj_info, a >= 0.0f ? &blend_color : (vec4*)0,
            0, (int32_t)instances.size(), instances_mat, 0, true);
    }

    void DispManager::entry_obj_by_object_info_object_skin(object_info obj_info,
        const std::vector<texture_pattern_struct>* texture_pattern,
        const texture_data_struct* texture_data, float_t alpha,
        const mat4* matrices, const mat4* ex_data_matrices, const mat4* mat, const mat4* global_mat) {
        obj_skin* skin = object_storage_get_obj_skin(obj_info);
        if (!skin)
            return;

        obj_skin_set_matrix_buffer(skin, matrices, ex_data_matrices, rctx_ptr->matrix_buffer, mat, global_mat);

        vec4 texture_color_coefficients;
        vec4 texture_color_offset;
        vec4 texture_specular_coefficients;
        vec4 texture_specular_offset;
        if (texture_data && !texture_data->field_0) {
            vec4 value;
            get_texture_color_coeff(texture_color_coefficients);
            *(vec3*)&value = texture_data->texture_color_coefficients * *(vec3*)&texture_color_coefficients;
            value.w = 0.0f;
            set_texture_color_coefficients(value);

            get_texture_color_offset(texture_color_offset);
            *(vec3*)&value = texture_data->texture_color_offset;
            value.w = 0.0f;
            set_texture_color_offset(value);

            get_texture_specular_coeff(texture_specular_coefficients);
            *(vec3*)&value = texture_data->texture_specular_coefficients * *(vec3*)&texture_specular_coefficients;
            value.w = 0.0f;
            set_texture_specular_coefficients(value);

            get_texture_specular_offset(texture_specular_offset);
            *(vec3*)&value = texture_data->texture_specular_offset;
            value.w = 0.0f;
            set_texture_specular_offset(value);
        }

        size_t texture_pattern_count = texture_pattern ? texture_pattern->size() : 0;
        if (texture_pattern && texture_pattern_count)
            set_texture_pattern((int32_t)texture_pattern_count, texture_pattern->data());

        if (fabsf(alpha - 1.0f) > 0.000001f)
            entry_obj_by_object_info(global_mat, obj_info, alpha, rctx_ptr->matrix_buffer);
        else
            entry_obj_by_object_info(global_mat, obj_info, rctx_ptr->matrix_buffer);

        if (texture_pattern && texture_pattern_count)
            set_texture_pattern();

        if (texture_data && !texture_data->field_0) {
            set_texture_color_coefficients(texture_color_coefficients);
            set_texture_color_offset(texture_color_offset);
            set_texture_specular_coefficients(texture_specular_coefficients);
            set_texture_specular_offset(texture_specular_offset);
        }
    }

    void DispManager::entry_obj_etc(const mat4* mat, EtcObj* etc, bool local) {
        ObjData* data = alloc_obj_data(mdl::OBJ_KIND_ETC);
        if (!data)
            return;

        data->init_etc(this, mat, etc);
        if (etc->color.a == 0xFF) {
            if (!local && (obj_flags & OBJ_SHADOW))
                mdl::DispManager::entry_list((mdl::ObjType)(OBJ_TYPE_SHADOW_CHARA + shadow_type), data);
            mdl::DispManager::entry_list(local ? OBJ_TYPE_OPAQUE_LOCAL : OBJ_TYPE_OPAQUE, data);
        }
        else
            mdl::DispManager::entry_list(local ? OBJ_TYPE_TRANSLUCENT_LOCAL : OBJ_TYPE_TRANSLUCENT, data);
    }

    void DispManager::entry_obj_user(const mat4* mat, UserArgsFunc func, void* data, ObjType type) {
        ObjData* _data = alloc_obj_data(OBJ_KIND_USER);
        if (_data) {
            _data->init_user(mat, func, data);
            entry_list(type, _data);
        }
    }

    GLuint DispManager::get_vertex_array(const ObjSubMeshArgs* args) {
        const obj_mesh* mesh = args->mesh;
        const obj_sub_mesh* sub_mesh = args->sub_mesh;
        const obj_material_data* material = args->material;

        //GLuint vertex_buffer = args->vertex_buffer;
        obj_mesh_vertex_buffer* vertex_buffer = args->vertex_buffer;
        //GLuint morph_vertex_buffer = args->morph_vertex_buffer;
        obj_mesh_vertex_buffer* morph_vertex_buffer = args->morph_vertex_buffer;
        //GLuint index_buffer = args->index_buffer;
        obj_mesh_index_buffer* index_buffer = args->index_buffer;

        int32_t texcoord_array[2] = { -1, -1 };
        int32_t color_tex_index = 0;
        for (const obj_material_texture_data& i : material->material.texdata) {
            if (i.tex_index == -1)
                continue;

            int32_t texcoord_index = obj_material_texture_type_get_texcoord_index(
                i.shader_info.m.tex_type, color_tex_index);
            if (texcoord_index < 0)
                continue;

            texcoord_array[texcoord_index] = sub_mesh->uv_index[&i - material->material.texdata];

            if (i.shader_info.m.tex_type == OBJ_MATERIAL_TEXTURE_COLOR)
                color_tex_index++;
        }

        GLuint vertex_attrib_buffer_binding[16] = {};
        object_data_get_vertex_attrib_buffer_bindings(args,
            texcoord_array, vertex_attrib_buffer_binding);

        bool compressed = mesh->attrib.m.compressed;
        GLsizei size_vertex = (GLsizei)mesh->size_vertex;
        obj_vertex_format vertex_format = mesh->vertex_format;

        for (DispManager::vertex_array& i : vertex_array_cache)
            if (i.alive_time > 0 && i.vertex_buffer == vertex_buffer
                && i.morph_vertex_buffer == morph_vertex_buffer
                && i.index_buffer == index_buffer && i.vertex_format == vertex_format
                && i.size_vertex == size_vertex && i.compressed == compressed
                && !memcmp(i.vertex_attrib_buffer_binding,
                    vertex_attrib_buffer_binding, sizeof(vertex_attrib_buffer_binding))
                && !memcmp(i.texcoord_array, texcoord_array, sizeof(texcoord_array)))
                return i.vertex_array;
        return 0;
    }

    GLuint DispManager::get_vertex_array(const EtcObj* etc) {
        EtcObjType type = etc->type;
        float_t length = 0;
        switch (type) {
        case mdl::ETC_OBJ_TEAPOT:
        case mdl::ETC_OBJ_GRID:
        case mdl::ETC_OBJ_CUBE:
        case mdl::ETC_OBJ_SPHERE:
        case mdl::ETC_OBJ_PLANE:
        case mdl::ETC_OBJ_CONE:
        case mdl::ETC_OBJ_LINE:
        case mdl::ETC_OBJ_CROSS:
            break;
        case mdl::ETC_OBJ_CAPSULE: // Added
            length = vec3::distance(etc->data.capsule.pos[0], etc->data.capsule.pos[1]);
            break;
        case mdl::ETC_OBJ_ELLIPSE: // Added
            length = vec3::distance(etc->data.ellipse.pos[0], etc->data.ellipse.pos[1]);
            break;
        case mdl::ETC_OBJ_CYLINDER: // Added
            break;
        default:
            return 0;
        }

        for (DispManager::etc_vertex_array& i : etc_vertex_array_cache) {
            if (i.alive_time <= 0 || i.type != type)
                continue;

            switch (type) {
            case mdl::ETC_OBJ_TEAPOT:
                return i.vertex_array;
            case mdl::ETC_OBJ_GRID:
                if (!memcmp(&i.data.grid, &etc->data.grid, sizeof(EtcObjGrid)))
                    return i.vertex_array;
                break;
            case mdl::ETC_OBJ_CUBE:
                return i.vertex_array;
            case mdl::ETC_OBJ_SPHERE:
                if (i.data.sphere.slices == etc->data.sphere.slices
                    && i.data.sphere.stacks == etc->data.sphere.stacks
                    && fabsf(i.data.sphere.radius - etc->data.sphere.radius) < 0.00001f)
                    return i.vertex_array;
                break;
            case mdl::ETC_OBJ_PLANE:
                return i.vertex_array;
            case mdl::ETC_OBJ_CONE:
                if (i.data.cone.slices == etc->data.cone.slices
                    && i.data.cone.stacks == etc->data.cone.stacks
                    && fabsf(i.data.cone.base - etc->data.cone.base) < 0.00001f
                    && fabsf(i.data.cone.height - etc->data.cone.height) < 0.00001f)
                    return i.vertex_array;
                break;
            case mdl::ETC_OBJ_LINE:
                return i.vertex_array;
            case mdl::ETC_OBJ_CROSS:
                return i.vertex_array;
            case mdl::ETC_OBJ_CAPSULE: // Added
                if (i.data.capsule.slices == etc->data.capsule.slices
                    && ((i.data.capsule.stacks + 1)) >> 1 == ((etc->data.capsule.stacks + 1) >> 1)
                    && fabsf(i.data.capsule.radius - etc->data.capsule.radius) < 0.00001f
                    && fabsf(vec3::distance(i.data.capsule.pos[0], i.data.capsule.pos[1]) - length) < 0.00001f)
                    return i.vertex_array;
                break;
            case mdl::ETC_OBJ_ELLIPSE: // Added
                if (i.data.ellipse.slices == etc->data.ellipse.slices
                    && ((i.data.ellipse.stacks + 1)) >> 1 == ((etc->data.ellipse.stacks + 1) >> 1)
                    && fabsf(i.data.ellipse.radius - etc->data.ellipse.radius) < 0.00001f
                    && fabsf(vec3::distance(i.data.ellipse.pos[0], i.data.ellipse.pos[1]) - length) < 0.00001f)
                    return i.vertex_array;
                break;
            case mdl::ETC_OBJ_CYLINDER: // Added
                if (i.data.cylinder.slices == etc->data.cylinder.slices
                    && i.data.cylinder.stacks == etc->data.cylinder.stacks
                    && fabsf(i.data.cylinder.base - etc->data.cylinder.base) < 0.00001f
                    && fabsf(i.data.cylinder.top - etc->data.cylinder.top) < 0.00001f
                    && fabsf(i.data.cylinder.height - etc->data.cylinder.height) < 0.00001f)
                    return i.vertex_array;
                break;
            }
        }
        return 0;
    }

    bool DispManager::get_chara_color() {
        return chara_color;
    }

    void DispManager::get_material_list(int32_t& count, material_list_struct*& value) {
        count = material_list_count;

        for (int32_t i = 0; i < count; i++)
            value[i] = material_list_array[i];
    }

    void DispManager::get_morph(object_info& object, float_t& weight) {
        weight = morph.weight;
        object = morph.object;
    }

    void DispManager::get_obj_center(const mat4& mat, const mdl::ObjSubMeshArgs* args, vec3& center) {
        const vec3 bounding_sphere_center = args->sub_mesh->bounding_sphere.center;
        if (args->mat_count <= 0 || !args->sub_mesh->num_bone_index) {
            mat4_transform_point(&mat, &bounding_sphere_center, &center);
            return;
        }

        vec3 center_sum = 0.0f;
        int32_t num_bone_index = args->sub_mesh->num_bone_index;
        for (int32_t i = 0; i < num_bone_index; i++) {
            vec3 _center;
            mat4_transform_point(&args->mats[i], &bounding_sphere_center, &_center);
            center_sum += _center;
        }
        center = center_sum * (1.0f / (float_t)num_bone_index);
    }

    int32_t DispManager::get_obj_count(ObjType type) {
        return (int32_t)obj[type].size();
    }

    ObjFlags DispManager::get_obj_flags() {
        return obj_flags;
    }

    shadow_type_enum DispManager::get_shadow_type() {
        return shadow_type;
    }

    void DispManager::get_texture_color_coeff(vec4& value) {
        value = texture_color_coefficients;
    }

    void DispManager::get_texture_color_offset(vec4& value) {
        value = texture_color_offset;
    }

    void DispManager::get_texture_pattern(int32_t& count, texture_pattern_struct*& value) {
        count = texture_pattern_count;

        for (int32_t i = 0; i < count; i++)
            value[i] = texture_pattern_array[i];
    }

    void DispManager::get_texture_specular_coeff(vec4& value) {
        value = texture_specular_coefficients;
    }

    void DispManager::get_texture_specular_offset(vec4& value) {
        value = texture_specular_offset;
    }

    void DispManager::get_texture_transform(int32_t& count, texture_transform_struct*& value) {
        count = texture_transform_count;

        for (int32_t i = 0; i < count; i++)
            value[i] = texture_transform_array[i];
    }

    float_t DispManager::get_wet_param() {
        return wet_param;
    }

    void DispManager::obj_sort(const mat4* view, ObjType type, int32_t compare_func) {
        std::list<ObjData*>& list = obj[type];
        if (list.size() < 1)
            return;

        calc_obj_radius(view, type);

        switch (compare_func) {
        case 0:
            list.sort([](ObjData* left, ObjData* right) {
                return left->view_z > right->view_z;
            });
            break;
        case 1:
            list.sort([](ObjData* left, ObjData* right) {
                return left->view_z < right->view_z;
            });
            break;
        case 2:
            list.sort([](ObjData* left, ObjData* right) {
                return left->radius > right->radius;
            });
            break;
        }
    }

    void DispManager::refresh() {
        culling.passed_prev = culling.passed;
        culling.culled_prev = culling.culled;
        culling.passed = {};
        culling.culled = {};

        obj_flags = (::mdl::ObjFlags)0;
        field_8 = 0;
        field_C = 0;

        texture_pattern_count = 0;
        memset(texture_pattern_array, 0, sizeof(texture_pattern_array));
        texture_transform_count = 0;
        memset(texture_transform_array, 0, sizeof(texture_transform_array));
        texture_color_coefficients = 1.0f;
        texture_color_offset = 0.0f;
        texture_specular_coefficients = 1.0f;
        texture_specular_offset = 0.0f;

        for (std::list<ObjData*>& i : obj)
            i.clear();

        buffer_reset();
    }

    void DispManager::set_chara_color(bool value) {
        chara_color = value;
    }

    void DispManager::set_culling_finc(bool(*func)(const obj_bounding_sphere*, const mat4*)) {
        culling.func = func;
    }

    void DispManager::set_obj_flags(ObjFlags flags) {
        obj_flags = flags;
    }

    void DispManager::set_material_list(int32_t count, const material_list_struct* value) {
        if (count > MATERIAL_LIST_COUNT)
            return;

        material_list_count = count;

        if (count)
            for (int32_t i = 0; i < count; i++)
                material_list_array[i] = value[i];
        else
            for (int32_t i = 0; i < MATERIAL_LIST_COUNT; i++)
                material_list_array[i] = {};
    }

    void DispManager::set_morph(object_info object, float_t weight) {
        morph.weight = weight;
        morph.object = object;
    }

    void DispManager::set_shadow_type(shadow_type_enum type) {
        if (type == SHADOW_CHARA || type == SHADOW_STAGE)
            shadow_type = type;
    }

    void DispManager::set_texture_color_coefficients(const vec4& value) {
        texture_color_coefficients = value;
    }

    void DispManager::set_texture_color_offset(const vec4& value) {
        texture_color_offset = value;
    }

    void DispManager::set_texture_pattern(int32_t count, const texture_pattern_struct* value) {
        if (count > TEXTURE_PATTERN_COUNT)
            return;

        texture_pattern_count = count;

        if (count)
            for (int32_t i = 0; i < count; i++)
                texture_pattern_array[i] = value[i];
        else
            for (int32_t i = 0; i < TEXTURE_PATTERN_COUNT; i++)
                texture_pattern_array[i] = {};
    }

    void DispManager::set_texture_specular_coefficients(const vec4& value) {
        texture_specular_coefficients = value;
    }

    void DispManager::set_texture_specular_offset(const vec4& value) {
        texture_specular_offset = value;
    }

    void DispManager::set_texture_transform(int32_t count, const texture_transform_struct* value) {
        if (count > TEXTURE_TRANSFORM_COUNT)
            return;

        texture_transform_count = count;

        if (count)
            for (int32_t i = 0; i < count; i++)
                texture_transform_array[i] = value[i];
        else
            for (int32_t i = count; i < TEXTURE_TRANSFORM_COUNT; i++)
                texture_transform_array[i] = {};
    }

    void DispManager::set_wet_param(float_t value) {
        wet_param = value;
    }
}

static void object_data_get_vertex_attrib_buffer_bindings(const mdl::ObjSubMeshArgs* args,
    int32_t texcoord_array[2], GLuint vertex_attrib_buffer_binding[16]) {
    const obj_mesh* mesh = args->mesh;
    const obj_sub_mesh* sub_mesh = args->sub_mesh;

    //GLuint vertex_buffer = args->vertex_buffer;
    GLuint vertex_buffer = args->vertex_buffer
        ? args->vertex_buffer->get_buffer() : 0;
    //GLuint morph_vertex_buffer = args->morph_vertex_buffer;
    GLuint morph_vertex_buffer = args->morph_vertex_buffer
        ? args->morph_vertex_buffer->get_buffer() : 0;

    bool compressed = mesh->attrib.m.compressed;
    GLsizei size_vertex = (GLsizei)mesh->size_vertex;
    obj_vertex_format vertex_format = mesh->vertex_format;

    if (vertex_format & OBJ_VERTEX_POSITION)
        vertex_attrib_buffer_binding[POSITION_INDEX] = vertex_buffer;
    if (vertex_format & OBJ_VERTEX_NORMAL)
        vertex_attrib_buffer_binding[NORMAL_INDEX] = vertex_buffer;
    if (vertex_format & OBJ_VERTEX_TANGENT)
        vertex_attrib_buffer_binding[TANGENT_INDEX] = vertex_buffer;

    for (int32_t i = 0; i < 2; i++) {
        int32_t texcoord_index = texcoord_array[i];
        if (texcoord_index >= 0)
            if (vertex_format & (OBJ_VERTEX_TEXCOORD0 << texcoord_index))
                vertex_attrib_buffer_binding[TEXCOORD0_INDEX + i] = vertex_buffer;
    }

    if (vertex_format & OBJ_VERTEX_COLOR0)
        vertex_attrib_buffer_binding[COLOR0_INDEX] = vertex_buffer;

    if (vertex_format & OBJ_VERTEX_BONE_DATA) {
        vertex_attrib_buffer_binding[BONE_WEIGHT_INDEX] = vertex_buffer;
        vertex_attrib_buffer_binding[BONE_INDEX_INDEX] = vertex_buffer;
    }

    if (!compressed && vertex_format & OBJ_VERTEX_UNKNOWN)
        vertex_attrib_buffer_binding[UNKNOWN_INDEX] = vertex_buffer;

    if (args->morph_vertex_buffer) {
        if (vertex_format & OBJ_VERTEX_POSITION)
            vertex_attrib_buffer_binding[MORPH_POSITION_INDEX] = morph_vertex_buffer;
        if (vertex_format & OBJ_VERTEX_NORMAL)
            vertex_attrib_buffer_binding[MORPH_NORMAL_INDEX] = morph_vertex_buffer;
        if (vertex_format & OBJ_VERTEX_TANGENT)
            vertex_attrib_buffer_binding[MORPH_TANGENT_INDEX] = morph_vertex_buffer;
        if (vertex_format & OBJ_VERTEX_TEXCOORD0)
            vertex_attrib_buffer_binding[MORPH_TEXCOORD0_INDEX] = morph_vertex_buffer;
        if (vertex_format & OBJ_VERTEX_TEXCOORD1)
            vertex_attrib_buffer_binding[MORPH_TEXCOORD1_INDEX] = morph_vertex_buffer;
        if (vertex_format & OBJ_VERTEX_COLOR0)
            vertex_attrib_buffer_binding[MORPH_COLOR_INDEX] = morph_vertex_buffer;
    }
}
