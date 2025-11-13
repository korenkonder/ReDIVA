/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "disp_manager.hpp"
#include "../../KKdLib/database/stage.hpp"
#include "../Glitter/glitter.hpp"
#include "../config.hpp"
#include "../gl_state.hpp"
#include "../render_context.hpp"
#include "../shader_ft.hpp"
#include "draw_object.hpp"

extern render_context* rctx_ptr;

extern bool reflect_draw;
extern bool reflect_full;

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

static void sub_140436760(const cam_data& cam);

material_list_struct::material_list_struct() : blend_color(), has_blend_color(), emission(), has_emission() {
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
    static void gen_capsule_indices(std::vector<uint32_t>& indices,
        int32_t slices, int32_t stacks, float_t length, bool wire, size_t start);
    static void gen_capsule_vertices(etc_obj_vertex_data* data,
        int32_t slices, int32_t stacks, float_t length, float_t radius);

    static void gen_cylinder_indices(std::vector<uint32_t>& indices,
        int32_t slices, int32_t stacks, bool wire, size_t start);
    static void gen_cylinder_vertices(etc_obj_vertex_data* data,
        int32_t slices, int32_t stacks, float_t base, float_t height);

    static void gen_sphere_indices(std::vector<uint32_t>& indices,
        int32_t slices, int32_t stacks, bool wire, size_t start);
    static void gen_sphere_vertices(etc_obj_vertex_data* data,
        int32_t slices, int32_t stacks, float_t radius);

    static void set_default_vertex_attrib();

    ObjSubMeshArgs::ObjSubMeshArgs() : sub_mesh(), mesh(), material(), textures(), mat_count(), mats(),
        vertex_buffer(), vertex_buffer_offset(), index_buffer(), set_blend_color(), chara_color(), self_shadow(),
        shadow(), morph_vertex_buffer(), morph_vertex_buffer_offset(), morph_weight(), texture_pattern_count(),
        texture_transform_count(), instances_count(), instances_mat(), func(), func_data() {

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

    EtcObjCylinder::EtcObjCylinder() : wire() {
        base = 1.0f;
        height = 1.0f;
        slices = 8;
        stacks = 8;
    }

    EtcObj::Data::Data() : capsule() {

    }

    EtcObj::Data::Data(const EtcObjTeapot& other) : capsule() {
        teapot = other;
    }

    EtcObj::Data::Data(const EtcObjGrid& other) : capsule() {
        grid = other;
    }

    EtcObj::Data::Data(const EtcObjCube& other) : capsule() {
        cube = other;
    }

    EtcObj::Data::Data(const EtcObjSphere& other) : capsule() {
        sphere = other;
    }

    EtcObj::Data::Data(const EtcObjPlane& other) : capsule() {
        plane = other;
    }

    EtcObj::Data::Data(const EtcObjCone& other) : capsule() {
        cone = other;
    }

    EtcObj::Data::Data(const EtcObjLine& other) : capsule() {
        line = other;
    }

    EtcObj::Data::Data(const EtcObjCross& other) : capsule() {
        cross = other;
    }

    EtcObj::Data::Data(const EtcObjCapsule& other) : capsule() {
        capsule = other;
    }

    EtcObj::Data::Data(const EtcObjCylinder& other) : capsule() {
        cylinder = other;
    }

    EtcObj::EtcObj(EtcObjType type) {
        this->type = type;
        color = 0xFFFFFFFF;
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

    void ObjData::init_etc(DispManager* disp_manager,
        const mat4& mat, int32_t index, int32_t count, const EtcObj& etc) {
        kind = mdl::OBJ_KIND_ETC;
        this->mat = mat;
        args.etc.color = etc.color;
        args.etc.constant = etc.constant;
        args.etc.index = index;
        args.etc.count = count;

        if (etc.type == mdl::ETC_OBJ_CAPSULE) {
            const EtcObjCapsule& capsule = etc.data.capsule;

            vec3 origin = (capsule.pos[0] + capsule.pos[1]) * 0.5f;
            mat4_add_translate(&this->mat, &origin, &this->mat);

            const vec3 dir = vec3::normalize(capsule.pos[1] - capsule.pos[0]);
            const vec3 up = { 0.0f, 0.0f, 1.0f };
            vec3 axis;
            float_t angle;
            vec3::axis_angle_from_vectors(axis, angle, up, dir);

            mat4 m = mat4_identity;
            mat4_mul_rotation(&m, &axis, angle, &m);
            mat4_mul(&m, &this->mat, &this->mat);
        }
    }

    void ObjData::init_sub_mesh(DispManager* disp_manager, const mat4& mat, float_t radius, const obj_sub_mesh* sub_mesh,
        const obj_mesh* mesh, const obj_material_data* material, const std::vector<GLuint>* textures,
        int32_t mat_count, const mat4* mats, GLuint vertex_buffer, size_t vertex_buffer_offset,
        GLuint index_buffer, const vec4& blend_color, const vec4& emission, GLuint morph_vertex_buffer,
        size_t morph_vertex_buffer_offset,
        int32_t instances_count, const mat4* instances_mat, draw_func func, const ObjSubMeshArgs* func_data) {
        kind = mdl::OBJ_KIND_NORMAL;
        this->mat = mat;
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
        args->vertex_buffer_offset = vertex_buffer_offset;
        args->index_buffer = index_buffer;
        args->morph_vertex_buffer = morph_vertex_buffer;
        args->morph_vertex_buffer_offset = morph_vertex_buffer_offset;

        args->texture_pattern_count = disp_manager->texture_pattern_count;
        for (int32_t i = 0; i < disp_manager->texture_pattern_count && i < TEXTURE_PATTERN_COUNT; i++)
            args->texture_pattern_array[i] = disp_manager->texture_pattern_array[i];

        args->texture_transform_count = disp_manager->texture_transform_count;
        for (int32_t i = 0; i < disp_manager->texture_transform_count && i < TEXTURE_TRANSFORM_COUNT; i++)
            args->texture_transform_array[i] = disp_manager->texture_transform_array[i];

        if (blend_color != 1.0f) {
            args->set_blend_color = true;
            args->blend_color = blend_color;
        }
        else {
            args->set_blend_color = false;
            args->blend_color = 1.0f;
        }

        args->emission = emission;

        args->chara_color = disp_manager->chara_color;
        if (disp_manager->obj_flags & OBJ_4)
            args->self_shadow = 1;
        else if (disp_manager->obj_flags & mdl::OBJ_8)
            args->self_shadow = 2;
        else
            args->self_shadow = 0;
        args->shadow = disp_manager->shadow_type;
        args->texture_color_coefficients = disp_manager->texture_color_coefficients;
        args->texture_color_coefficients.w = disp_manager->wet_param;
        args->texture_color_offset = disp_manager->texture_color_offset;
        args->texture_specular_coefficients = disp_manager->texture_specular_coefficients;
        args->texture_specular_offset = disp_manager->texture_specular_offset;
        args->instances_count = instances_count;
        args->instances_mat = instances_mat;
        args->func = func;
        args->func_data = func_data;

        disp_manager->add_vertex_array(mesh, sub_mesh, material, vertex_buffer,
            vertex_buffer_offset, index_buffer, morph_vertex_buffer, morph_vertex_buffer_offset);
    }

    void ObjData::init_translucent(const mat4& mat, const  ObjTranslucentArgs& translucent) {
        kind = mdl::OBJ_KIND_TRANSLUCENT;
        this->mat = mat;
        args.translucent = translucent;
    }

    void ObjData::init_user(const mat4& mat, UserArgsFunc func, void* data) {
        kind = OBJ_KIND_USER;
        this->mat = mat;
        args.user.func = func;
        args.user.data = data;
    }

    EtcObjManager::EtcObjManager() : vao(), vbo_vertex_count(), ebo_index_count() {

    }

    EtcObjManager::~EtcObjManager() {
        if (vao) {
            glDeleteVertexArrays(1, &vao);
            vao = 0;
        }

        ebo.Destroy();
        vbo.Destroy();
    }

    bool EtcObjManager::add_capsule(int32_t& index, int32_t& count, const EtcObjCapsule& capsule) {
        if (capsule.slices < 2 || capsule.stacks < 2)
            return false;

        const float_t length = vec3::distance(capsule.pos[0], capsule.pos[1]);
        if (length < 0.00000f) {
            EtcObjSphere sphere = {};
            sphere.radius = capsule.radius;
            sphere.slices = capsule.slices;
            sphere.stacks = capsule.stacks;
            sphere.wire = capsule.wire;
            return add_sphere(index, count, sphere);
        }

        for (const auto& i : etc_obj_buffer) {
            if (i.first.first != ETC_OBJ_CAPSULE)
                continue;

            const EtcObjCapsule& other_capsule = i.second.capsule;
            if (other_capsule.slices == capsule.slices
                && ((other_capsule.stacks + 1)) >> 1 == ((capsule.stacks + 1) >> 1)
                && fabsf(other_capsule.radius - capsule.radius) < 0.00001f
                && fabsf(vec3::distance(other_capsule.pos[0], other_capsule.pos[1]) - length) < 0.00001f) {
                index = i.first.second;
                count = 1;
                return true;
            }
        }

        index = (int32_t)draw_param_buffer.size();
        count = 1;

        etc_obj_vertex_data* vtx_data = 0;
        const size_t stacks = (((size_t)capsule.stacks + 1) / 2) * 2;
        size_t vtx_count = (size_t)capsule.slices * stacks + 2;
        size_t start = add_data(vtx_data, vtx_count);

        gen_capsule_vertices(vtx_data, capsule.slices, capsule.stacks, length, capsule.radius);

        draw_param_buffer.push_back({});
        etc_obj_draw_param& draw_param = draw_param_buffer.back();

        draw_param.attrib.m.primitive = capsule.wire ? GL_LINES : GL_TRIANGLES;
        draw_param.start = (GLuint)start;

        size_t idx_offset = index_buffer.size();
        gen_capsule_indices(index_buffer, capsule.slices, capsule.stacks, length, capsule.wire, start);

        draw_param.end = (GLuint)(start + vtx_count - 1);
        draw_param.offset = (GLintptr)(idx_offset * sizeof(uint32_t));
        draw_param.count = (GLsizei)(index_buffer.size() - idx_offset);

        etc_obj_buffer.push_back({ ETC_OBJ_CAPSULE, index }, capsule);
        return true;
    }

    bool EtcObjManager::add_cone(int32_t& index, int32_t& count, const EtcObjCone& cone) {
        for (const auto& i : etc_obj_buffer) {
            if (i.first.first != ETC_OBJ_CONE)
                continue;

            const EtcObjCone& other_cone = i.second.cone;
            if (other_cone.slices == cone.slices
                && other_cone.stacks == cone.stacks
                && fabsf(other_cone.base - cone.base) < 0.00001f
                && fabsf(other_cone.height - cone.height) < 0.00001f
                && other_cone.wire == cone.wire) {
                index = i.first.second;
                count = cone.wire ? 2 : 1;
                return true;
            }
        }

        const float_t base = cone.base;
        const float_t height = cone.height;
        const int32_t slices = cone.slices;
        const int32_t stacks = cone.stacks;

        const float_t slice_step = (float_t)((M_PI * 2.0) / (double_t)slices);

        if (!cone.wire) {
            etc_obj_vertex_data* vtx_data = 0;
            int32_t _index = 0;
            if (add_data(_index, vtx_data, (size_t)slices * 3, GL_TRIANGLES)) {
                index = _index;
                count = 1;

                float_t slice_angle = (float_t)-1 * slice_step;
                float_t x = cosf(slice_angle);
                float_t y = sinf(slice_angle);

                const float_t cosn = base / sqrtf(height * height + base * base);
                const float_t sinn = 1.0f - cosn * cosn;

                etc_obj_vertex_data vtx2 = { vec3(x * base, y * base, 0.0f), vec3(x * sinn, y * sinn, cosn) };
                for (int32_t j = 0; j < slices; j++) {
                    float_t slice_angle0 = ((float_t)j - 0.5f) * slice_step;
                    float_t x0 = cosf(slice_angle0);
                    float_t y0 = sinf(slice_angle0);

                    vtx_data[0] = { vec3(0.0f, 0.0f, height), vec3(x0 * sinn, y0 * sinn, cosn) };
                    vtx_data[1] = vtx2;

                    float_t slice_angle2 = (float_t)j * slice_step;
                    float_t x2 = cosf(slice_angle2);
                    float_t y2 = sinf(slice_angle2);

                    vtx2 = { vec3(x2 * base, y2 * base, 0.0f), vec3(x2 * sinn, y2 * sinn, cosn) };
                    vtx_data[2] = vtx2;

                    vtx_data += 3;
                }

                etc_obj_buffer.push_back({ ETC_OBJ_CONE, index }, cone);
                return true;
            }
        }
        else {
            const float_t cosn = base / sqrtf(height * height + base * base);
            const float_t sinn = 1.0f - cosn * cosn;

            etc_obj_vertex_data* vtx_data = 0;
            int32_t _index = 0;
            if (add_data(_index, vtx_data, ((size_t)slices + 1) * stacks + 1, GL_LINE_STRIP)) {
                index = _index;
                count = 2;

                for (int32_t i = 0; i < stacks; i++) {
                    const float_t xy = (1.0f - (float_t)i * (1.0f / stacks)) * base;
                    const float_t z = (float_t)i * height * (1.0f / stacks);
                    for (int32_t j = 0; j <= slices; j++) {
                        float_t slice_angle = (float_t)j * slice_step;
                        float_t x = cosf(slice_angle);
                        float_t y = sinf(slice_angle);
                        *vtx_data++ = { vec3(x * xy, y * xy, z), vec3(x * sinn, y * sinn, cosn) };
                    }
                }
                *vtx_data++ = { vec3(0.0f, 0.0f, height), vec3(0.0f, 0.0f, 1.0f) };

                int32_t _index = 0;
                if (add_data(_index, vtx_data, ((size_t)slices - 1) * 2, GL_LINES)) {
                    for (int32_t j = 1; j < slices; j++) {
                        float_t slice_angle = (float_t)j * slice_step;
                        float_t x = cosf(slice_angle);
                        float_t y = sinf(slice_angle);
                        vtx_data[0] = { vec3(x * base, y * base, 0.0f), vec3(x * sinn, y * sinn, cosn) };
                        vtx_data[1] = { vec3(0.0f, 0.0f, height), vec3(x * sinn, y * sinn, cosn) };
                        vtx_data += 2;
                    }

                    etc_obj_buffer.push_back({ ETC_OBJ_CONE, index }, cone);
                    return true;
                }
            }
        }
        return false;
    }

    bool EtcObjManager::add_cross(int32_t& index, int32_t& count, const EtcObjCross& cross) {
        if (fabsf(cross.size) < 0.00001f)
            return false;

        for (const auto& i : etc_obj_buffer) {
            if (i.first.first != ETC_OBJ_CROSS)
                continue;

            const EtcObjCross& other_cross = i.second.cross;
            if (fabsf(other_cross.size - cross.size) < 0.00001f) {
                index = i.first.second;
                count = 1;
                return true;
            }
        }

        int32_t _index = 0;
        etc_obj_vertex_data* vtx_data = 0;
        if (add_data(_index, vtx_data, 6, GL_LINES)) {
            index = _index;
            count = 1;

            const float_t size = cross.size;

            vtx_data[0] = vec3(-size,  0.0f,  0.0f);
            vtx_data[1] = vec3( size,  0.0f,  0.0f);
            vtx_data[2] = vec3( 0.0f, -size,  0.0f);
            vtx_data[3] = vec3( 0.0f,  size,  0.0f);
            vtx_data[4] = vec3( 0.0f,  0.0f, -size);
            vtx_data[5] = vec3( 0.0f,  0.0f,  size);

            etc_obj_buffer.push_back({ ETC_OBJ_CROSS, index }, cross);
            return true;
        }
        return false;
    }

    bool EtcObjManager::add_cube(int32_t& index, int32_t& count, const EtcObjCube& cube) {
        if (fabsf(cube.size.x) < 0.00001f || fabsf(cube.size.y) < 0.00001f || fabsf(cube.size.z) < 0.00001f)
            return false;

        for (const auto& i : etc_obj_buffer) {
            if (i.first.first != ETC_OBJ_CUBE)
                continue;

            const EtcObjCube& other_cube = i.second.cube;
            if (vec3::dot(vec3::abs(other_cube.size - cube.size), 1.0f) < 0.00001f) {
                index = i.first.second;
                count = cube.wire ? 2 : 1;
                return true;
            }
        }

        const vec3 size = cube.size * 0.5f;

        const vec3 vtx_array[] = {
            { -size.x, -size.y, -size.z },
            { -size.x, -size.y,  size.z },
            {  size.x, -size.y,  size.z },
            {  size.x, -size.y, -size.z },
            { -size.x,  size.y, -size.z },
            { -size.x,  size.y,  size.z },
            {  size.x,  size.y,  size.z },
            {  size.x,  size.y, -size.z },
        };

        int32_t _index = 0;
        etc_obj_vertex_data* vtx_data = 0;
        if (!cube.wire) {
            if (add_data(_index, vtx_data, 36, GL_TRIANGLES)) {
                index = _index;
                count = 1;

                struct cube_face_data {
                    int32_t vtx_idx[4];
                    vec3 normal;
                };

                const cube_face_data face_array[] = {
                    { { 5, 1, 2, 6 }, vec3( 0.0f,  0.0f,  1.0f) },
                    { { 6, 2, 3, 7 }, vec3( 1.0f,  0.0f,  0.0f) },
                    { { 7, 3, 0, 4 }, vec3( 0.0f,  0.0f, -1.0f) },
                    { { 4, 0, 1, 5 }, vec3(-1.0f,  0.0f,  0.0f) },
                    { { 4, 5, 6, 7 }, vec3( 0.0f,  1.0f,  0.0f) },
                    { { 1, 0, 3, 2 }, vec3( 0.0f, -1.0f,  0.0f) },
                };

                const cube_face_data* face = face_array;

                for (int32_t i = 0; i < 6; i++) {
                    vtx_data[0].position = vtx_array[face[i].vtx_idx[0]];
                    vtx_data[0].normal = face[i].normal;
                    vtx_data[1].position = vtx_array[face[i].vtx_idx[1]];
                    vtx_data[1].normal = face[i].normal;
                    vtx_data[2].position = vtx_array[face[i].vtx_idx[3]];
                    vtx_data[2].normal = face[i].normal;
                    vtx_data[3].position = vtx_array[face[i].vtx_idx[3]];
                    vtx_data[3].normal = face[i].normal;
                    vtx_data[4].position = vtx_array[face[i].vtx_idx[1]];
                    vtx_data[4].normal = face[i].normal;
                    vtx_data[5].position = vtx_array[face[i].vtx_idx[2]];
                    vtx_data[5].normal = face[i].normal;
                    vtx_data += 6;
                }

                etc_obj_buffer.push_back({ ETC_OBJ_CUBE, index }, cube);
                return true;
            }
        }
        else {
            if (add_data(_index, vtx_data, 10, GL_LINE_STRIP)) {
                index = _index;
                count = 2;

                vtx_data[0] = vtx_array[0];
                vtx_data[1] = vtx_array[1];
                vtx_data[2] = vtx_array[5];
                vtx_data[3] = vtx_array[4];
                vtx_data[4] = vtx_array[0];
                vtx_data[5] = vtx_array[3];
                vtx_data[6] = vtx_array[2];
                vtx_data[7] = vtx_array[6];
                vtx_data[8] = vtx_array[7];
                vtx_data[9] = vtx_array[3];
                vtx_data += 10;

                if (add_data(_index, vtx_data, 6, GL_LINES)) {
                    vtx_data[0] = vtx_array[4];
                    vtx_data[1] = vtx_array[7];
                    vtx_data[2] = vtx_array[5];
                    vtx_data[3] = vtx_array[6];
                    vtx_data[4] = vtx_array[1];
                    vtx_data[5] = vtx_array[2];
                    vtx_data += 6;

                    etc_obj_buffer.push_back({ ETC_OBJ_CUBE, index }, cube);
                    return true;
                }
            }
        }
        return false;
    }

    bool EtcObjManager::add_cylinder(int32_t& index, int32_t& count, const EtcObjCylinder& cylinder) {
        if (cylinder.slices < 2 || cylinder.stacks < 0)
            return false;

        for (const auto& i : etc_obj_buffer) {
            if (i.first.first != ETC_OBJ_CYLINDER)
                continue;

            const EtcObjCylinder& other_cylinder = i.second.cylinder;
            if (other_cylinder.slices == cylinder.slices
                && other_cylinder.stacks == cylinder.stacks
                && fabsf(other_cylinder.base - cylinder.base) < 0.00001f
                && fabsf(other_cylinder.height - cylinder.height) < 0.00001f) {
                index = i.first.second;
                count = 1;
                return true;
            }
        }

        index = (int32_t)draw_param_buffer.size();
        count = 1;

        etc_obj_vertex_data* vtx_data = 0;
        size_t vtx_count = ((size_t)cylinder.slices * ((size_t)cylinder.stacks + 1) + 2);
        size_t start = add_data(vtx_data, vtx_count);

        gen_cylinder_vertices(vtx_data, cylinder.slices, cylinder.stacks, cylinder.base, cylinder.height);

        draw_param_buffer.push_back({});
        etc_obj_draw_param& draw_param = draw_param_buffer.back();

        draw_param.attrib.m.primitive = cylinder.wire ? GL_LINES : GL_TRIANGLES;
        draw_param.start = (GLuint)start;

        size_t idx_offset = index_buffer.size();
        gen_cylinder_indices(index_buffer, cylinder.slices, cylinder.stacks, cylinder.wire, start);

        draw_param.end = (GLuint)(start + vtx_count - 1);
        draw_param.offset = (GLintptr)(idx_offset * sizeof(uint32_t));
        draw_param.count = (GLsizei)(index_buffer.size() - idx_offset);

        etc_obj_buffer.push_back({ ETC_OBJ_CYLINDER, index }, cylinder);
        return true;
    }


    template <typename T>
    size_t EtcObjManager::add_data(T*& data, size_t num_vertex) {
        size_t size = vertex_buffer.size();
        size_t align_offset = (sizeof(T) - size % sizeof(T)) % sizeof(T);
        size_t vertex_array_size = sizeof(T) * num_vertex;

        vertex_buffer.resize(size + align_offset + vertex_array_size);
        T* vtx_data = (T*)(vertex_buffer.data() + size + align_offset);
        data = vtx_data;
        return (uint32_t)((size + align_offset) / sizeof(T));
    }

    template <typename T>
    bool EtcObjManager::add_data(int32_t& index, T*& data, size_t num_vertex, GLenum primitive) {
        size_t size = vertex_buffer.size();
        size_t align_offset = (sizeof(T) - size % sizeof(T)) % sizeof(T);
        size_t vertex_array_size = sizeof(T) * num_vertex;

        draw_param_buffer.push_back({});
        etc_obj_draw_param& draw_param = draw_param_buffer.back();
        draw_param.attrib.m.primitive = primitive;
        draw_param.first = (GLint)(uint32_t)((size + align_offset) / sizeof(T));
        draw_param.count = (GLsizei)num_vertex;

        index = (int32_t)(draw_param_buffer.size() - 1);

        if (vertex_buffer.size() < size + align_offset + vertex_array_size)
            vertex_buffer.resize(size + align_offset + vertex_array_size);

        T* vtx_data = (T*)(vertex_buffer.data() + size + align_offset);
        data = vtx_data;
        return true;
    }

    bool EtcObjManager::add_grid(int32_t& index, int32_t& count, const EtcObjGrid& grid) {
        if (grid.ws < 1 || grid.hs < 1)
            return false;

        for (const auto& i : etc_obj_buffer) {
            if (i.first.first != ETC_OBJ_GRID)
                continue;

            const EtcObjGrid& other_grid = i.second.grid;
            if (!memcmp(&other_grid, &grid, sizeof(EtcObjGrid))) {
                index = i.first.second;
                count = 1;
                return true;
            }
        }

        int32_t _index = 0;
        etc_obj_vertex_data* vtx_data = 0;
        if (add_data(_index, vtx_data, ((size_t)grid.ws + grid.hs) * 2, GL_LINES)) {
            index = _index;
            count = 2;

            const int32_t ws = grid.ws;
            const int32_t hs = grid.hs;
            const int32_t ws_half = grid.ws / 2;
            const int32_t hs_half = grid.hs / 2;

            const float_t width = (float_t)grid.w * 0.5f;
            const float_t height = (float_t)grid.h * 0.5f;
            const float_t ws_half_flt = (float_t)ws_half;
            const float_t hs_half_flt = (float_t)hs_half;

            for (int32_t i = 0; i <= ws; i++) {
                if (ws_half == i)
                    continue;

                const float_t w = ((float_t)i * (width / ws_half_flt)) - width;
                vtx_data[0] = vec3(w, 0.0f, -height);
                vtx_data[1] = vec3(w, 0.0f,  height);
                vtx_data += 2;
            }

            for (int32_t i = 0; i <= hs; i++) {
                if (hs_half == i)
                    continue;

                const float_t h = ((float_t)i * (height / hs_half_flt)) - height;
                vtx_data[0] = vec3(-width, 0.0f, h);
                vtx_data[1] = vec3( width, 0.0f, h);
                vtx_data += 2;
            }

            if (add_data(_index, vtx_data, 4, GL_LINES)) {
                vtx_data[0] = vec3(  0.0f, 0.0f, -height);
                vtx_data[1] = vec3(  0.0f, 0.0f,  height);
                vtx_data[2] = vec3(-width, 0.0f,    0.0f);
                vtx_data[3] = vec3( width, 0.0f,    0.0f);
                vtx_data += 4;

                etc_obj_buffer.push_back({ ETC_OBJ_GRID, index }, grid);
                return true;
            }
        }
        return false;
    }

    bool EtcObjManager::add_line(int32_t& index, int32_t& count, const EtcObjLine& line) {
        if (vec3::distance(line.pos[0], line.pos[1]) < 0.00001f)
            return false;

        for (const auto& i : etc_obj_buffer) {
            if (i.first.first != ETC_OBJ_LINE)
                continue;

            const EtcObjLine& other_line = i.second.line;
            if (other_line.pos[0] == line.pos[0]
                && other_line.pos[1] == line.pos[1]) {
                index = i.first.second;
                count = 1;
                return true;
            }
        }

        int32_t _index = 0;
        etc_obj_vertex_data* vtx_data = 0;
        if (add_data(_index, vtx_data, 2, GL_LINES)) {
            index = _index;
            count = 1;

            vtx_data[0] = line.pos[0];
            vtx_data[1] = line.pos[1];
            vtx_data += 2;

            etc_obj_buffer.push_back({ ETC_OBJ_LINE, index }, line);
            return true;
        }
        return false;
    }

    bool EtcObjManager::add_obj(int32_t& index, int32_t& count, const EtcObj& etc) {
        switch (etc.type) {
        case mdl::ETC_OBJ_GRID:
            return add_grid(index, count, etc.data.grid);
        case mdl::ETC_OBJ_CUBE:
            return add_cube(index, count, etc.data.cube);
        case mdl::ETC_OBJ_SPHERE:
            return add_sphere(index, count, etc.data.sphere);
        case mdl::ETC_OBJ_PLANE:
            return add_plane(index, count, etc.data.plane);
        case mdl::ETC_OBJ_CONE:
            return add_cone(index, count, etc.data.cone);
        case mdl::ETC_OBJ_LINE:
            return add_line(index, count, etc.data.line);
        case mdl::ETC_OBJ_CROSS:
            return add_cross(index, count, etc.data.cross);
        case mdl::ETC_OBJ_CAPSULE: // Added
            return add_capsule(index, count, etc.data.capsule);
        case mdl::ETC_OBJ_CYLINDER: // Added
            return add_cylinder(index, count, etc.data.cylinder);
        default:
            return false;
        }
    }

    bool EtcObjManager::add_plane(int32_t& index, int32_t& count, const EtcObjPlane& plane) {
        if (!plane.w || !plane.h)
            return false;

        for (const auto& i : etc_obj_buffer) {
            if (i.first.first != ETC_OBJ_PLANE)
                continue;

            const EtcObjPlane& other_plane = i.second.plane;
            if (other_plane.w == plane.w && other_plane.h == plane.h) {
                index = i.first.second;
                count = 1;
                return true;
            }
        }

        int32_t _index = 0;
        etc_obj_vertex_data* vtx_data = 0;
        if (add_data(_index, vtx_data, 4, GL_TRIANGLE_STRIP)) {
            index = _index;
            count = 1;

            const float_t width = (float_t)plane.w * 0.5f;
            const float_t height = (float_t)plane.h * 0.5f;

            vtx_data[0] = vec3( width, 0.0f,  height);
            vtx_data[1] = vec3( width, 0.0f, -height);
            vtx_data[2] = vec3(-width, 0.0f,  height);
            vtx_data[3] = vec3(-width, 0.0f, -height);
            vtx_data += 4;

            etc_obj_buffer.push_back({ ETC_OBJ_PLANE, index }, plane);
            return true;
        }
        return false;
    }

    bool EtcObjManager::add_sphere(int32_t& index, int32_t& count, const EtcObjSphere& sphere) {
        if (sphere.slices < 2 || sphere.stacks < 2)
            return false;

        for (const auto& i : etc_obj_buffer) {
            if (i.first.first != ETC_OBJ_SPHERE)
                continue;

            const EtcObjSphere& other_sphere = i.second.sphere;
            if (other_sphere.slices == sphere.slices
                && other_sphere.stacks == sphere.stacks
                && fabsf(other_sphere.radius - sphere.radius) < 0.00001f) {
                index = i.first.second;
                count = 1;
                return true;
            }
        }

        index = (int32_t)draw_param_buffer.size();
        count = 1;

        const size_t stacks = (((size_t)sphere.stacks + 1) / 2) * 2;
        const size_t slices = sphere.slices;

        etc_obj_vertex_data* vtx_data = 0;
        size_t vtx_count = ((size_t)slices * ((size_t)stacks - 1) + 2);
        size_t start = add_data(vtx_data, vtx_count);

        gen_sphere_vertices(vtx_data, sphere.slices, sphere.stacks, sphere.radius);

        draw_param_buffer.push_back({});
        etc_obj_draw_param& draw_param = draw_param_buffer.back();

        draw_param.attrib.m.primitive = sphere.wire ? GL_LINES : GL_TRIANGLES;
        draw_param.start = (GLuint)start;

        size_t idx_offset = index_buffer.size();
        gen_sphere_indices(index_buffer, sphere.slices, sphere.stacks, sphere.wire, start);

        draw_param.end = (GLuint)(start + vtx_count - 1);
        draw_param.offset = (GLintptr)(idx_offset * sizeof(uint32_t));
        draw_param.count = (GLsizei)(index_buffer.size() - idx_offset);

        etc_obj_buffer.push_back({ ETC_OBJ_SPHERE, index }, sphere);
        return true;
    }

    void EtcObjManager::clear() {
        draw_param_buffer.clear();
        vertex_buffer.clear();
        index_buffer.clear();
        etc_obj_buffer.clear();
    }

    void EtcObjManager::pre_draw() {
        update();
    }

    void EtcObjManager::post_draw() {
        clear();
    }

    void EtcObjManager::update() {
        static const GLsizei buffer_size = sizeof(etc_obj_vertex_data);

        if (vbo_vertex_count < vertex_buffer.size() / buffer_size) {
            if (!vbo_vertex_count)
                vbo_vertex_count = 256;

            while (vbo_vertex_count < vertex_buffer.size() / buffer_size)
                vbo_vertex_count *= 2;

            if (vao) {
                glDeleteVertexArrays(1, &vao);
                vao = 0;
            }

            vbo.Recreate(gl_state, buffer_size * vbo_vertex_count);

            glGenVertexArrays(1, &vao);
            gl_state.bind_vertex_array(vao, true);
            gl_state.bind_array_buffer(vbo, true);
            gl_state.bind_element_array_buffer(ebo, true);

            set_default_vertex_attrib();

            glEnableVertexAttribArray(POSITION_INDEX);
            glVertexAttribPointer(POSITION_INDEX, 3, GL_FLOAT, GL_FALSE, buffer_size,
                (void*)offsetof(etc_obj_vertex_data, position));
            glEnableVertexAttribArray(NORMAL_INDEX);
            glVertexAttribPointer(NORMAL_INDEX, 3, GL_FLOAT, GL_FALSE, buffer_size,
                (void*)offsetof(etc_obj_vertex_data, normal));

            gl_state.bind_array_buffer(0);
            gl_state.bind_vertex_array(0);
            gl_state.bind_element_array_buffer(0);
        }

        vbo.WriteMemory(gl_state, 0, vertex_buffer.size(), vertex_buffer.data());

        if (ebo_index_count < index_buffer.size()) {
            if (!ebo_index_count)
                ebo_index_count = 256;

            while (ebo_index_count < index_buffer.size())
                ebo_index_count *= 2;

            ebo.Recreate(gl_state, sizeof(uint32_t) * ebo_index_count);

            gl_state.bind_vertex_array(vao);
            gl_state.bind_element_array_buffer(ebo, true);

            gl_state.bind_vertex_array(0);
            gl_state.bind_element_array_buffer(0);
        }

        ebo.WriteMemory(gl_state, 0, sizeof(uint32_t) * index_buffer.size(), index_buffer.data());

        gl_state.bind_array_buffer(0);
        gl_state.bind_element_array_buffer(0);
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

        free_def(buff);
    }

    void DispManager::add_vertex_array(const obj_mesh* mesh, const obj_sub_mesh* sub_mesh,
        const obj_material_data* material, GLuint vertex_buffer, size_t vertex_buffer_offset,
        GLuint index_buffer, GLuint morph_vertex_buffer, size_t morph_vertex_buffer_offset) {
        if (!vertex_buffer || !index_buffer)
            return;

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

        uint32_t compression = mesh->attrib.m.compression;
        GLsizei size_vertex = (GLsizei)mesh->size_vertex;
        obj_vertex_format vertex_format = mesh->vertex_format;

        for (DispManager::vertex_array& i : vertex_array_cache)
            if (i.vertex_buffer == vertex_buffer
                && i.vertex_buffer_offset == vertex_buffer_offset
                && i.morph_vertex_buffer == morph_vertex_buffer
                && i.morph_vertex_buffer_offset == morph_vertex_buffer_offset
                && i.index_buffer == index_buffer && i.vertex_format == vertex_format
                && i.size_vertex == size_vertex && i.compression == compression
                && !memcmp(i.texcoord_array, texcoord_array, sizeof(texcoord_array)))
                return;

        vertex_array_cache.push_back({});
        DispManager::vertex_array* vertex_array = &vertex_array_cache.back();

        glGenVertexArrays(1, &vertex_array->vertex_array);

        vertex_array->vertex_buffer = vertex_buffer;
        vertex_array->vertex_buffer_offset = vertex_buffer_offset;
        vertex_array->morph_vertex_buffer = morph_vertex_buffer;
        vertex_array->morph_vertex_buffer_offset = morph_vertex_buffer_offset;
        vertex_array->index_buffer = index_buffer;
        vertex_array->vertex_format = vertex_format;
        vertex_array->size_vertex = size_vertex;
        vertex_array->compression = compression;
        memcpy(&vertex_array->texcoord_array, texcoord_array, sizeof(texcoord_array));

        gl_state.bind_vertex_array(vertex_array->vertex_array, true);
        set_default_vertex_attrib();

        gl_state.bind_array_buffer(vertex_buffer, true);
        if (index_buffer)
            gl_state.bind_element_array_buffer(index_buffer, true);

        size_t offset = vertex_buffer_offset;
        if (vertex_format & OBJ_VERTEX_POSITION) {
            glEnableVertexAttribArray(POSITION_INDEX);
            glVertexAttribPointer(POSITION_INDEX,
                3, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
            offset += 12;
        }

        if (vertex_format & OBJ_VERTEX_NORMAL) {
            glEnableVertexAttribArray(NORMAL_INDEX);
            switch (compression) {
            case 0:
            default:
                glVertexAttribPointer(NORMAL_INDEX,
                    3, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                offset += 12;
                break;
            case 1:
                glVertexAttribPointer(NORMAL_INDEX,
                    3, GL_SHORT, GL_TRUE, size_vertex, (void*)offset);
                offset += 8;
                break;
            case 2:
                glVertexAttribPointer(NORMAL_INDEX,
                    4, GL_INT_2_10_10_10_REV, GL_TRUE, size_vertex, (void*)offset);
                offset += 4;
                break;
            }
        }

        if (vertex_format & OBJ_VERTEX_TANGENT) {
            glEnableVertexAttribArray(TANGENT_INDEX);
            switch (compression) {
            case 0:
            default:
                glVertexAttribPointer(TANGENT_INDEX,
                    4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                offset += 16;
                break;
            case 1:
                glVertexAttribPointer(TANGENT_INDEX,
                    4, GL_SHORT, GL_TRUE, size_vertex, (void*)offset);
                offset += 8;
                break;
            case 2:
                glVertexAttribPointer(TANGENT_INDEX,
                    4, GL_INT_2_10_10_10_REV, GL_TRUE, size_vertex, (void*)offset);
                offset += 4;
                break;
            }
        }

        if (!compression && (vertex_format & OBJ_VERTEX_BINORMAL))
            offset += 12;

        for (int32_t i = 0; i < 2; i++) {
            int32_t texcoord_index = texcoord_array[i];
            if (texcoord_index < 0)
                continue;

            if (vertex_format & (OBJ_VERTEX_TEXCOORD0 << texcoord_index)) {
                glEnableVertexAttribArray(TEXCOORD0_INDEX + i);
                switch (compression) {
                case 0:
                default:
                    glVertexAttribPointer(TEXCOORD0_INDEX + i, 2, GL_FLOAT, GL_FALSE,
                        size_vertex, (void*)(offset + 8ULL * texcoord_index));
                    break;
                case 1:
                case 2:
                    glVertexAttribPointer(TEXCOORD0_INDEX + i, 2, GL_HALF_FLOAT, GL_FALSE,
                        size_vertex, (void*)(offset + 4ULL * texcoord_index));
                    break;
                }
            }
        }

        switch (compression) {
        case 0:
        default:
            if (vertex_format & OBJ_VERTEX_TEXCOORD0)
                offset += 8;
            if (vertex_format & OBJ_VERTEX_TEXCOORD1)
                offset += 8;
            if (vertex_format & OBJ_VERTEX_TEXCOORD2)
                offset += 8;
            if (vertex_format & OBJ_VERTEX_TEXCOORD3)
                offset += 8;
            break;
        case 1:
        case 2:
            if (vertex_format & OBJ_VERTEX_TEXCOORD0)
                offset += 4;
            if (vertex_format & OBJ_VERTEX_TEXCOORD1)
                offset += 4;
            if (vertex_format & OBJ_VERTEX_TEXCOORD2)
                offset += 4;
            if (vertex_format & OBJ_VERTEX_TEXCOORD3)
                offset += 4;
            break;
        }

        if (vertex_format & OBJ_VERTEX_COLOR0) {
            glEnableVertexAttribArray(COLOR0_INDEX);
            switch (compression) {
            case 0:
            default:
                glVertexAttribPointer(COLOR0_INDEX,
                    4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                offset += 16;
                break;
            case 1:
            case 2:
                glVertexAttribPointer(COLOR0_INDEX,
                    4, GL_HALF_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                offset += 8;
                break;
            }
        }

        if (!compression && (vertex_format & OBJ_VERTEX_COLOR1))
            offset += 16;

        if (vertex_format & OBJ_VERTEX_BONE_DATA) {
            glEnableVertexAttribArray(BONE_WEIGHT_INDEX);
            switch (compression) {
            case 0:
            default:
                glVertexAttribPointer(BONE_WEIGHT_INDEX,
                    4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                offset += 16;
                break;
            case 1:
                glVertexAttribPointer(BONE_WEIGHT_INDEX,
                    4, GL_UNSIGNED_SHORT, GL_TRUE, size_vertex, (void*)offset);
                offset += 8;
                break;
            case 2:
                glVertexAttribPointer(BONE_WEIGHT_INDEX,
                    4, GL_UNSIGNED_INT_2_10_10_10_REV, GL_TRUE, size_vertex, (void*)offset);
                offset += 4;
                break;
            }

            glEnableVertexAttribArray(BONE_INDEX_INDEX);
            switch (compression) {
            case 0:
            case 1:
            default:
                glVertexAttribIPointer(BONE_INDEX_INDEX,
                    4, GL_UNSIGNED_SHORT, size_vertex, (void*)offset);
                offset += 8;
                break;
            case 2:
                glVertexAttribIPointer(BONE_INDEX_INDEX,
                    4, GL_UNSIGNED_BYTE, size_vertex, (void*)offset);
                offset += 4;
                break;
            }
        }

        if (!compression && (vertex_format & OBJ_VERTEX_UNKNOWN)) {
            glEnableVertexAttribArray(UNKNOWN_INDEX);
            glVertexAttribPointer(UNKNOWN_INDEX,
                4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
            offset += 16;
        }

        if (morph_vertex_buffer) {
            gl_state.bind_array_buffer(morph_vertex_buffer, true);

            size_t offset = morph_vertex_buffer_offset;
            if (vertex_format & OBJ_VERTEX_POSITION) {
                glEnableVertexAttribArray(MORPH_POSITION_INDEX);
                glVertexAttribPointer(MORPH_POSITION_INDEX,
                    3, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                offset += 12;
            }

            if (vertex_format & OBJ_VERTEX_NORMAL) {
                glEnableVertexAttribArray(MORPH_NORMAL_INDEX);
                switch (compression) {
                case 0:
                default:
                    glVertexAttribPointer(MORPH_NORMAL_INDEX,
                        3, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                    offset += 12;
                    break;
                case 1:
                    glVertexAttribPointer(MORPH_NORMAL_INDEX,
                        3, GL_SHORT, GL_TRUE, size_vertex, (void*)offset);
                    offset += 8;
                    break;
                case 2:
                    glVertexAttribPointer(MORPH_NORMAL_INDEX,
                        3, GL_INT_2_10_10_10_REV, GL_TRUE, size_vertex, (void*)offset);
                    offset += 4;
                    break;
                }
            }

            if (vertex_format & OBJ_VERTEX_TANGENT) {
                glEnableVertexAttribArray(MORPH_TANGENT_INDEX);
                switch (compression) {
                case 0:
                default:
                    glVertexAttribPointer(MORPH_TANGENT_INDEX,
                        4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                    offset += 16;
                    break;
                case 1:
                    glVertexAttribPointer(MORPH_TANGENT_INDEX,
                        4, GL_SHORT, GL_TRUE, size_vertex, (void*)offset);
                    offset += 8;
                    break;
                case 2:
                    glVertexAttribPointer(MORPH_TANGENT_INDEX,
                        4, GL_INT_2_10_10_10_REV, GL_TRUE, size_vertex, (void*)offset);
                    offset += 4;
                    break;
                }
            }

            if (!compression && (vertex_format & OBJ_VERTEX_BINORMAL))
                offset += 12;

            if (vertex_format & OBJ_VERTEX_TEXCOORD0) {
                glEnableVertexAttribArray(MORPH_TEXCOORD0_INDEX);
                switch (compression) {
                case 0:
                default:
                    glVertexAttribPointer(MORPH_TEXCOORD0_INDEX,
                        2, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                    offset += 8;
                    break;
                case 1:
                case 2:
                    glVertexAttribPointer(MORPH_TEXCOORD0_INDEX,
                        2, GL_HALF_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                    offset += 4;
                    break;
                }
            }

            if (vertex_format & OBJ_VERTEX_TEXCOORD1) {
                glEnableVertexAttribArray(MORPH_TEXCOORD1_INDEX);
                switch (compression) {
                case 0:
                default:
                    glVertexAttribPointer(MORPH_TEXCOORD1_INDEX,
                        2, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                    offset += 8;
                    break;
                case 1:
                case 2:
                    glVertexAttribPointer(MORPH_TEXCOORD1_INDEX,
                        2, GL_HALF_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                    offset += 4;
                    break;
                }
            }

            switch (compression) {
            case 0:
            default:
                if (vertex_format & OBJ_VERTEX_TEXCOORD2)
                    offset += 8;
                if (vertex_format & OBJ_VERTEX_TEXCOORD3)
                    offset += 8;
                break;
            case 1:
            case 2:
                if (vertex_format & OBJ_VERTEX_TEXCOORD2)
                    offset += 4;
                if (vertex_format & OBJ_VERTEX_TEXCOORD3)
                    offset += 4;
                break;
            }

            if (vertex_format & OBJ_VERTEX_COLOR0) {
                glEnableVertexAttribArray(MORPH_COLOR_INDEX);
                switch (compression) {
                case 0:
                default:
                    glVertexAttribPointer(MORPH_COLOR_INDEX,
                        4, GL_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                    offset += 16;
                    break;
                case 1:
                case 2:
                    glVertexAttribPointer(MORPH_COLOR_INDEX,
                        4, GL_HALF_FLOAT, GL_FALSE, size_vertex, (void*)offset);
                    offset += 8;
                    break;
                }
            }

            switch (compression) {
            case 0:
            default:
                if (vertex_format & OBJ_VERTEX_COLOR1)
                    offset += 16;

                if (vertex_format & OBJ_VERTEX_BONE_DATA)
                    offset += 24;

                if (vertex_format & OBJ_VERTEX_UNKNOWN)
                    offset += 16;
                break;
            case 1:
                if (vertex_format & OBJ_VERTEX_BONE_DATA)
                    offset += 16;
                break;
            case 2:
                if (vertex_format & OBJ_VERTEX_BONE_DATA)
                    offset += 8;
                break;
            }
        }

        gl_state.bind_array_buffer(0);
        gl_state.bind_vertex_array(0);
        if (index_buffer)
            gl_state.bind_element_array_buffer(0);
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

    void DispManager::calc_obj_radius(const cam_data& cam, mdl::ObjType type) {
        std::vector<vec3> alpha_center;
        std::vector<vec3> mat_center;

        for (ObjData*& i : obj[type]) {
            vec3 center = 0.0f;
            bool v50 = false;
            switch (i->kind) {
            case OBJ_KIND_NORMAL: {
                mat4 mat = i->mat;
                if (i->args.sub_mesh.mesh->attrib.m.billboard)
                    model_mat_face_camera_view(cam, &mat, &mat);
                else if (i->args.sub_mesh.mesh->attrib.m.billboard_y_axis)
                    model_mat_face_camera_position(cam, &mat, &mat);

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

            mat4_transform_point(&cam.get_view_mat(), &center, &center);
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
                entry_obj_etc(mat, etc);
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
                entry_obj_etc(mat, etc);
            }
    }

    void DispManager::calc_obj_radius(const cam_data& cam, ObjTypeScreen type) {
        for (ObjData*& i : obj_screen[type]) {
            vec3 center = 0.0f;
            bool v50 = false;
            switch (i->kind) {
            case OBJ_KIND_NORMAL: {
                mat4 mat = i->mat;
                if (i->args.sub_mesh.mesh->attrib.m.billboard)
                    model_mat_face_camera_view(cam, &mat, &mat);
                else if (i->args.sub_mesh.mesh->attrib.m.billboard_y_axis)
                    model_mat_face_camera_position(cam, &mat, &mat);

                get_obj_center(mat, &i->args.sub_mesh, center);

                v50 = !!i->args.sub_mesh.material->material.attrib.m.flag_30;
                i->radius = i->args.sub_mesh.mesh->bounding_sphere.radius;
            } break;
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

            mat4_transform_point(&cam.get_view_mat(), &center, &center);
            i->view_z = center.z;
        }
    }

    void DispManager::calc_obj_radius(const cam_data& cam, ObjTypeReflect type) {
        for (ObjData*& i : obj_reflect[type]) {
            vec3 center = 0.0f;
            bool v50 = false;
            switch (i->kind) {
            case OBJ_KIND_NORMAL: {
                mat4 mat = i->mat;
                if (i->args.sub_mesh.mesh->attrib.m.billboard)
                    model_mat_face_camera_view(cam, &mat, &mat);
                else if (i->args.sub_mesh.mesh->attrib.m.billboard_y_axis)
                    model_mat_face_camera_position(cam, &mat, &mat);

                get_obj_center(mat, &i->args.sub_mesh, center);

                v50 = !!i->args.sub_mesh.material->material.attrib.m.flag_30;
                i->radius = i->args.sub_mesh.mesh->bounding_sphere.radius;
            } break;
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

            mat4_transform_point(&cam.get_view_mat(), &center, &center);
            i->view_z = center.z;
        }
    }

    void DispManager::draw(render_data_context& rend_data_ctx, ObjType type,
        const cam_data& cam, int32_t depth_mask, bool reflect_texture_mask, int32_t alpha) {
        if (type < 0 || type >= OBJ_TYPE_MAX || get_obj_count(type) < 1)
            return;

        int32_t alpha_test = 0;
        float_t min_alpha = 1.0f;
        float_t alpha_threshold = 0.0f;
        bool reflect = rend_data_ctx.shader_flags.arr[U_REFLECT] == 1;
        void(*func)(render_data_context & rend_data_ctx, const ObjSubMeshArgs * args,
            const cam_data & cam, const mat4 * mat) = draw_sub_mesh_default;

        for (int32_t i = 0; i < 5; i++)
            rend_data_ctx.state.active_bind_texture_2d(i, rctx_ptr->empty_texture_2d->glid);
        rend_data_ctx.state.active_bind_texture_cube_map(5, rctx_ptr->empty_texture_cube_map->glid);
        rend_data_ctx.state.active_texture(0);
        rend_data_ctx.state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        rend_data_ctx.reset_shader_flags();

        switch (type) {
        case OBJ_TYPE_TRANSLUCENT:
        case OBJ_TYPE_TRANSLUCENT_SORT_BY_RADIUS:
            if (depth_mask)
                func = draw_sub_mesh_no_mat;
            else
                rend_data_ctx.state.set_depth_mask(GL_FALSE);

            alpha_test = 1;
            min_alpha = 0.0f;
            alpha_threshold = 0.0f;
            break;
        case OBJ_TYPE_TRANSPARENT:
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
            func = draw_sub_mesh_no_mat;
            rend_data_ctx.state.set_color_mask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
            rctx_ptr->draw_state->rend_data[rend_data_ctx.index].shader_index = SHADER_FT_SIL;
            break;
        case OBJ_TYPE_TYPE_7:
            func = draw_sub_mesh_no_mat;
            rend_data_ctx.state.set_color_mask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
            rctx_ptr->draw_state->rend_data[rend_data_ctx.index].shader_index = SHADER_FT_SIL;

            alpha_test = 1;
            min_alpha = 0.0f;
            alpha_threshold = 0.99999994f;
            break;
        case OBJ_TYPE_REFLECT_CHARA_OPAQUE:
            rend_data_ctx.state.set_cull_face_mode(GL_FRONT);

            if (!reflect_draw) {
                if (reflect)
                    func = draw_sub_mesh_cheap;
                else if (rctx_ptr->render_manager->reflect_type == STAGE_DATA_REFLECT_REFLECT_MAP)
                    func = draw_sub_mesh_cheap_reflect_map;
            }
            break;
        case OBJ_TYPE_REFLECT_CHARA_TRANSLUCENT:
            rend_data_ctx.state.set_cull_face_mode(GL_FRONT);

            min_alpha = 0.0f;

            if (!reflect_draw) {
                if (reflect)
                    func = draw_sub_mesh_cheap;
                else if (rctx_ptr->render_manager->reflect_type == STAGE_DATA_REFLECT_REFLECT_MAP)
                    func = draw_sub_mesh_cheap_reflect_map;
            }
            break;
        case OBJ_TYPE_REFLECT_CHARA_TRANSPARENT:
            rend_data_ctx.state.set_cull_face_mode(GL_FRONT);

            alpha_test = 1;
            min_alpha = 0.1f;
            alpha_threshold = 0.5f;

            if (!reflect_draw) {
                if (reflect)
                    func = draw_sub_mesh_cheap;
                else if (rctx_ptr->render_manager->reflect_type == STAGE_DATA_REFLECT_REFLECT_MAP)
                    func = draw_sub_mesh_cheap_reflect_map;
            }
            break;
        case OBJ_TYPE_REFLECT_OPAQUE:
            alpha_test = 1;
            alpha_threshold = 0.5f;

            if (reflect_draw)
                rend_data_ctx.state.set_cull_face_mode(GL_FRONT);
            else if (!reflect_texture_mask)
                func = draw_sub_mesh_cheap_reflect_map;
            break;
        case OBJ_TYPE_REFLECT_TRANSLUCENT:
            rend_data_ctx.state.set_depth_mask(GL_FALSE);

            min_alpha = 0.0f;
            alpha_threshold = 0.0f;

            if (reflect_draw)
                rend_data_ctx.state.set_cull_face_mode(GL_FRONT);
            break;
        case OBJ_TYPE_REFLECT_TRANSPARENT:
            alpha_test = 1;
            min_alpha = 0.1f;
            alpha_threshold = 0.5f;

            if (reflect_draw)
                rend_data_ctx.state.set_cull_face_mode(GL_FRONT);
            break;
        case OBJ_TYPE_REFRACT_TRANSLUCENT:
            rend_data_ctx.state.set_depth_mask(GL_FALSE);

            min_alpha = 0.0f;
            alpha_threshold = 0.0f;
            break;
        case OBJ_TYPE_REFRACT_TRANSPARENT:
            alpha_test = 1;
            min_alpha = 0.1f;
            alpha_threshold = 0.5f;
            break;
        case OBJ_TYPE_SSS:
            func = draw_sub_mesh_sss;

            if (reflect_draw)
                rend_data_ctx.state.set_cull_face_mode(GL_FRONT);
            break;
        case OBJ_TYPE_OPAQUE_ALPHA_ORDER_POST_GLITTER:
        case OBJ_TYPE_OPAQUE_ALPHA_ORDER_POST_TRANSLUCENT:
        case OBJ_TYPE_OPAQUE_ALPHA_ORDER_POST_OPAQUE:
            if (reflect_draw)
                rend_data_ctx.state.set_cull_face_mode(GL_FRONT);
            break;
        case OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_POST_GLITTER:
        case OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_POST_TRANSLUCENT:
        case OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_POST_OPAQUE:
            alpha_test = 1;
            min_alpha = 0.1f;
            alpha_threshold = 0.5f;

            if (reflect_draw)
                rend_data_ctx.state.set_cull_face_mode(GL_FRONT);
            break;
        case OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_POST_GLITTER:
        case OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_POST_TRANSLUCENT:
        case OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_POST_OPAQUE:
            rend_data_ctx.state.set_depth_mask(GL_FALSE);

            alpha_test = 1;
            min_alpha = 0.0f;
            alpha_threshold = 0.0f;

            if (reflect_draw)
                rend_data_ctx.state.set_cull_face_mode(GL_FRONT);
            break;
        case OBJ_TYPE_USER:
            func = draw_sub_mesh_no_mat;
            break;
        }
        rend_data_ctx.set_batch_alpha_threshold(alpha_threshold);
        rend_data_ctx.set_batch_min_alpha(min_alpha);
        rend_data_ctx.shader_flags.arr[U_ALPHA_TEST] = alpha_test;

        if (alpha < 0)
            for (ObjData*& i : obj[type]) {
                switch (i->kind) {
                case OBJ_KIND_NORMAL: {
                    draw_sub_mesh(rend_data_ctx, &i->args.sub_mesh, &i->mat, func, cam);
                } break;
                case OBJ_KIND_ETC: {
                    draw_etc_obj(rend_data_ctx, &i->args.etc, &i->mat);
                } break;
                case OBJ_KIND_USER: {
                    i->args.user.func(rend_data_ctx, i->args.user.data, cam, &i->mat);
                } break;
                case OBJ_KIND_TRANSLUCENT: {
                    for (int32_t j = 0; j < i->args.translucent.count; j++)
                        draw_sub_mesh(rend_data_ctx, i->args.translucent.sub_mesh[j], &i->mat, func, cam);
                } break;
                }
            }
        else
            for (ObjData*& i : obj[type]) {
                switch (i->kind) {
                case OBJ_KIND_NORMAL: {
                    int32_t a = (int32_t)(i->args.sub_mesh.blend_color.w * 255.0f);
                    a = clamp_def(a, 0, 255);
                    if (a == alpha)
                        draw_sub_mesh(rend_data_ctx, &i->args.sub_mesh, &i->mat, func, cam);
                } break;
                case OBJ_KIND_TRANSLUCENT: {
                    for (int32_t j = 0; j < i->args.translucent.count; j++) {
                        ObjSubMeshArgs* args = i->args.translucent.sub_mesh[j];
                        int32_t a = (int32_t)(args->blend_color.w * 255.0f);
                        a = clamp_def(a, 0, 255);
                        if (a == alpha)
                            draw_sub_mesh(rend_data_ctx, args, &i->mat, func, cam);
                    }
                } break;
                }
            }

        switch (type) {
        case OBJ_TYPE_TRANSLUCENT:
        case OBJ_TYPE_TRANSLUCENT_SORT_BY_RADIUS:
            if (!depth_mask)
                rend_data_ctx.state.set_depth_mask(GL_TRUE);
            break;
        case OBJ_TYPE_TYPE_6:
        case OBJ_TYPE_TYPE_7:
            rctx_ptr->draw_state->rend_data[rend_data_ctx.index].shader_index = -1;
            rend_data_ctx.state.set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            break;
        case OBJ_TYPE_REFLECT_CHARA_OPAQUE:
        case OBJ_TYPE_REFLECT_CHARA_TRANSLUCENT:
        case OBJ_TYPE_REFLECT_CHARA_TRANSPARENT:
            rend_data_ctx.state.set_cull_face_mode(GL_BACK);
            break;
        case OBJ_TYPE_REFLECT_OPAQUE:
            if (reflect_draw)
                rend_data_ctx.state.set_cull_face_mode(GL_BACK);
            break;
        case OBJ_TYPE_REFLECT_TRANSLUCENT:
            if (reflect_draw)
                rend_data_ctx.state.set_cull_face_mode(GL_BACK);
            rend_data_ctx.state.set_depth_mask(GL_TRUE);
            break;
        case OBJ_TYPE_REFLECT_TRANSPARENT:
            if (reflect_draw)
                rend_data_ctx.state.set_cull_face_mode(GL_BACK);
            break;
        case OBJ_TYPE_REFRACT_TRANSLUCENT:
            if (reflect_draw)
                rend_data_ctx.state.set_cull_face_mode(GL_BACK);
            rend_data_ctx.state.set_depth_mask(GL_TRUE);
            break;
        case OBJ_TYPE_SSS:
            if (reflect_draw)
                rend_data_ctx.state.set_cull_face_mode(GL_BACK);
            break;
        case OBJ_TYPE_OPAQUE_ALPHA_ORDER_POST_GLITTER:
        case OBJ_TYPE_OPAQUE_ALPHA_ORDER_POST_TRANSLUCENT:
        case OBJ_TYPE_OPAQUE_ALPHA_ORDER_POST_OPAQUE:
            if (reflect_draw)
                rend_data_ctx.state.set_cull_face_mode(GL_BACK);
            break;
        case OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_POST_GLITTER:
        case OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_POST_TRANSLUCENT:
        case OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_POST_OPAQUE:
            if (reflect_draw)
                rend_data_ctx.state.set_cull_face_mode(GL_BACK);
            break;
        case OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_POST_GLITTER:
        case OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_POST_TRANSLUCENT:
        case OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_POST_OPAQUE:
            rend_data_ctx.state.set_depth_mask(GL_TRUE);

            if (reflect_draw)
                rend_data_ctx.state.set_cull_face_mode(GL_BACK);
            break;
        }

        rend_data_ctx.reset_shader_flags();
        rend_data_ctx.state.bind_vertex_array(0);
        shader::unbind(rend_data_ctx.state);
        rend_data_ctx.state.set_blend_func(GL_ONE, GL_ZERO);
        for (int32_t i = 0; i < 5; i++)
            rend_data_ctx.state.bind_sampler(i, 0);
    }

    void DispManager::draw(render_data_context& rend_data_ctx, mdl::ObjTypeScreen type,
        const cam_data& cam, int32_t depth_mask, bool reflect_texture_mask, int32_t alpha) {
        if (type < 0 || type >= OBJ_TYPE_SCREEN_MAX || get_obj_count(type) < 1)
            return;

        int32_t alpha_test = 0;
        float_t min_alpha = 1.0f;
        float_t alpha_threshold = 0.0f;
        void(*func)(render_data_context & rend_data_ctx, const ObjSubMeshArgs * args,
            const cam_data & cam, const mat4 * mat) = draw_sub_mesh_default;

        for (int32_t i = 0; i < 5; i++)
            rend_data_ctx.state.active_bind_texture_2d(i, rctx_ptr->empty_texture_2d->glid);
        rend_data_ctx.state.active_bind_texture_cube_map(5, rctx_ptr->empty_texture_cube_map->glid);
        rend_data_ctx.state.active_texture(0);
        rend_data_ctx.state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        rend_data_ctx.reset_shader_flags();

        switch (type) {
        case OBJ_TYPE_SCREEN_TRANSLUCENT:
            if (depth_mask)
                func = draw_sub_mesh_no_mat;
            else
                rend_data_ctx.state.set_depth_mask(GL_FALSE);

            alpha_test = 1;
            min_alpha = 0.0f;
            alpha_threshold = 0.0f;
            break;
        case OBJ_TYPE_SCREEN_TRANSPARENT:
            alpha_test = 1;
            min_alpha = 0.1f;
            alpha_threshold = 0.5f;
            break;
        case OBJ_TYPE_SCREEN_TRANSPARENT_ALPHA_ORDER_POST_TRANSLUCENT:
            alpha_test = 1;
            min_alpha = 0.1f;
            alpha_threshold = 0.5f;

            if (reflect_draw)
                rend_data_ctx.state.set_cull_face_mode(GL_FRONT);
            break;
        case OBJ_TYPE_SCREEN_TRANSLUCENT_ALPHA_ORDER_POST_TRANSLUCENT:
            rend_data_ctx.state.set_depth_mask(GL_FALSE);

            alpha_test = 1;
            min_alpha = 0.0f;
            alpha_threshold = 0.0f;

            if (reflect_draw)
                rend_data_ctx.state.set_cull_face_mode(GL_FRONT);
            break;
        }
        rend_data_ctx.set_batch_alpha_threshold(alpha_threshold);
        rend_data_ctx.set_batch_min_alpha(min_alpha);
        rend_data_ctx.shader_flags.arr[U_ALPHA_TEST] = alpha_test;

        if (alpha < 0)
            for (ObjData*& i : obj_screen[type]) {
                switch (i->kind) {
                case OBJ_KIND_NORMAL: {
                    draw_sub_mesh(rend_data_ctx, &i->args.sub_mesh, &i->mat, func, cam);
                } break;
                case OBJ_KIND_TRANSLUCENT: {
                    for (int32_t j = 0; j < i->args.translucent.count; j++)
                        draw_sub_mesh(rend_data_ctx, i->args.translucent.sub_mesh[j], &i->mat, func, cam);
                } break;
                }
            }
        else
            for (ObjData*& i : obj_screen[type]) {
                switch (i->kind) {
                case OBJ_KIND_NORMAL: {
                    int32_t a = (int32_t)(i->args.sub_mesh.blend_color.w * 255.0f);
                    a = clamp_def(a, 0, 255);
                    if (a == alpha)
                        draw_sub_mesh(rend_data_ctx, &i->args.sub_mesh, &i->mat, func, cam);
                } break;
                case OBJ_KIND_TRANSLUCENT: {
                    for (int32_t j = 0; j < i->args.translucent.count; j++) {
                        ObjSubMeshArgs* args = i->args.translucent.sub_mesh[j];
                        int32_t a = (int32_t)(args->blend_color.w * 255.0f);
                        a = clamp_def(a, 0, 255);
                        if (a == alpha)
                            draw_sub_mesh(rend_data_ctx, args, &i->mat, func, cam);
                    }
                } break;
                }
            }

        switch (type) {
        case OBJ_TYPE_SCREEN_TRANSLUCENT:
            if (!depth_mask)
                rend_data_ctx.state.set_depth_mask(GL_TRUE);
            break;
        case OBJ_TYPE_SCREEN_TRANSPARENT_ALPHA_ORDER_POST_TRANSLUCENT:
            if (reflect_draw)
                rend_data_ctx.state.set_cull_face_mode(GL_BACK);
            break;
        case OBJ_TYPE_SCREEN_TRANSLUCENT_ALPHA_ORDER_POST_TRANSLUCENT:
            rend_data_ctx.state.set_depth_mask(GL_TRUE);

            if (reflect_draw)
                rend_data_ctx.state.set_cull_face_mode(GL_BACK);
            break;
        }

        rend_data_ctx.reset_shader_flags();
        rend_data_ctx.state.bind_vertex_array(0);
        shader::unbind(rend_data_ctx.state);
        rend_data_ctx.state.set_blend_func(GL_ONE, GL_ZERO);
        for (int32_t i = 0; i < 5; i++)
            rend_data_ctx.state.bind_sampler(i, 0);
    }

    void DispManager::draw(render_data_context& rend_data_ctx, mdl::ObjTypeReflect type,
        const cam_data& cam, int32_t depth_mask, bool reflect_texture_mask) {
        if (type < 0 || type >= OBJ_TYPE_REFLECT_MAX || get_obj_count(type) < 1)
            return;

        int32_t alpha_test = 0;
        float_t min_alpha = 1.0f;
        float_t alpha_threshold = 0.0f;
        bool reflect = rend_data_ctx.shader_flags.arr[U_REFLECT] == 1;
        void(*func)(render_data_context & rend_data_ctx, const ObjSubMeshArgs * args,
            const cam_data & cam, const mat4 * mat) = draw_sub_mesh_default;

        for (int32_t i = 0; i < 5; i++)
            rend_data_ctx.state.active_bind_texture_2d(i, rctx_ptr->empty_texture_2d->glid);
        rend_data_ctx.state.active_bind_texture_cube_map(5, rctx_ptr->empty_texture_cube_map->glid);
        rend_data_ctx.state.active_texture(0);
        rend_data_ctx.state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        rend_data_ctx.reset_shader_flags();

        switch (type) {
        case OBJ_TYPE_REFLECT_TRANSLUCENT_SORT_BY_RADIUS:
            if (depth_mask)
                func = draw_sub_mesh_no_mat;
            else
                rend_data_ctx.state.set_depth_mask(GL_FALSE);

            alpha_test = 1;
            min_alpha = 0.0f;
            alpha_threshold = 0.0f;

            rend_data_ctx.state.set_cull_face_mode(GL_FRONT);
            break;
        }
        rend_data_ctx.set_batch_alpha_threshold(alpha_threshold);
        rend_data_ctx.set_batch_min_alpha(min_alpha);
        rend_data_ctx.shader_flags.arr[U_ALPHA_TEST] = alpha_test;

        for (ObjData*& i : obj_reflect[type]) {
            switch (i->kind) {
            case OBJ_KIND_NORMAL: {
                draw_sub_mesh(rend_data_ctx, &i->args.sub_mesh, &i->mat, func, cam);
            } break;
            case OBJ_KIND_TRANSLUCENT: {
                for (int32_t j = 0; j < i->args.translucent.count; j++)
                    draw_sub_mesh(rend_data_ctx, i->args.translucent.sub_mesh[j], &i->mat, func, cam);
            } break;
            }
        }

        switch (type) {
        case OBJ_TYPE_REFLECT_TRANSLUCENT_SORT_BY_RADIUS:
            if (!depth_mask)
                rend_data_ctx.state.set_cull_face_mode(GL_BACK);
            break;
        }

        rend_data_ctx.reset_shader_flags();
        rend_data_ctx.state.bind_vertex_array(0);
        shader::unbind(rend_data_ctx.state);
        rend_data_ctx.state.set_blend_func(GL_ONE, GL_ZERO);
        for (int32_t i = 0; i < 5; i++)
            rend_data_ctx.state.bind_sampler(i, 0);
    }

    void DispManager::entry_list(ObjType type, ObjData* data) {
        obj[type].push_back(data);
    }

    void DispManager::entry_list(ObjTypeScreen type, ObjData* data) {
        obj_screen[type].push_back(data);
    }

    void DispManager::entry_list(ObjTypeReflect type, ObjData* data) {
        obj_reflect[type].push_back(data);
    }

    static int32_t obj_bounding_sphere_check_visibility_default(const vec3& center, camera* cam, float_t radius) {
        vec3 _center;
        mat4_transform_point(&cam->view, &center, &_center);

        double_t min_depth = (double_t)_center.z - (double_t)radius;
        double_t max_depth = (double_t)_center.z + (double_t)radius;
        if (-cam->min_distance < min_depth || -cam->max_distance > max_depth)
            return 0;

        float_t v5 = vec3::dot(cam->field_1E4, _center);
        if (v5 < -radius)
            return 0;

        float_t v6 = vec3::dot(cam->field_1F0, _center);
        if (v6 < -radius)
            return 0;

        float_t v7 = vec3::dot(cam->field_1FC, _center);
        if (v7 < -radius)
            return 0;

        float_t v8 = vec3::dot(cam->field_208, _center);
        if (v8 < -radius)
            return 0;

        if (-cam->min_distance >= max_depth && -cam->max_distance <= min_depth
            && v5 >= radius && v6 >= radius && v7 >= radius && v8 >= radius)
            return 1;
        return 2;
    }

    static int32_t obj_bounding_sphere_check_visibility(const obj_bounding_sphere& sphere,
        CullingCheck* culling, camera* cam, const mat4& mat) {
        if (culling->func)
            return culling->func(&sphere, &cam->view);

        vec3 center;
        mat4_transform_point(&mat, &sphere.center, &center);
        return obj_bounding_sphere_check_visibility_default(center, cam, mat4_get_max_scale(&mat) * sphere.radius);
    }

    static int32_t obj_axis_aligned_bounding_box_check_visibility_default(
        const obj_axis_aligned_bounding_box* aabb, camera* cam, const mat4& mat) {
        vec3 points[8];
        points[0] = aabb->center + (aabb->size ^ vec3( 0.0f,  0.0f,  0.0f));
        points[1] = aabb->center + (aabb->size ^ vec3(-0.0f, -0.0f, -0.0f));
        points[2] = aabb->center + (aabb->size ^ vec3(-0.0f,  0.0f,  0.0f));
        points[3] = aabb->center + (aabb->size ^ vec3( 0.0f, -0.0f, -0.0f));
        points[4] = aabb->center + (aabb->size ^ vec3( 0.0f, -0.0f,  0.0f));
        points[5] = aabb->center + (aabb->size ^ vec3(-0.0f,  0.0f, -0.0f));
        points[6] = aabb->center + (aabb->size ^ vec3( 0.0f,  0.0f, -0.0f));
        points[7] = aabb->center + (aabb->size ^ vec3(-0.0f, -0.0f,  0.0f));

        mat4 view_mat;
        mat4_mul(&mat, &cam->view, &view_mat);
        for (int32_t i = 0; i < 8; i++)
            mat4_transform_point(&view_mat, &points[i], &points[i]);

        vec4 v2[6];
        *(vec3*)&v2[0] = { 0.0f, 0.0f, -1.0f };
        v2[0].w = -cam->min_distance;
        *(vec3*)&v2[1] = cam->field_1E4;
        v2[1].w = 0.0f;
        *(vec3*)&v2[2] = cam->field_1F0;
        v2[2].w = 0.0f;
        *(vec3*)&v2[3] = cam->field_1FC;
        v2[3].w = 0.0f;
        *(vec3*)&v2[4] = cam->field_208;
        v2[4].w = 0.0f;
        *(vec3*)&v2[5] = { 0.0f, 0.0f, 1.0f };
        v2[5].w = cam->max_distance;

        for (int32_t i = 0; i < 6; i++)
            for (int32_t j = 0; j < 8; j++) {
                float_t vtx_data = vec3::dot(*(vec3*)&v2[i], points[j]) + v2[i].w;
                if (vtx_data > 0.0f)
                    break;

                if (j == 7)
                    return 0;
            }
        return 1;
    }

    static int32_t obj_axis_aligned_bounding_box_check_visibility(
        const obj_axis_aligned_bounding_box* aabb,
        CullingCheck* culling, camera* cam, const mat4& mat) {
        if (culling->func)
            return 1;
        return obj_axis_aligned_bounding_box_check_visibility_default(aabb, cam, mat);
    }

    bool DispManager::entry_obj(const ::obj* obj, const mat4& mat, obj_mesh_vertex_buffer* obj_vert_buf,
        obj_mesh_index_buffer* obj_index_buf, const std::vector<GLuint>* textures, const vec4* blend_color,
        const mat4* bone_mat, const ::obj* obj_morph, obj_mesh_vertex_buffer* obj_morph_vert_buf,
        int32_t instances_count, const mat4* instances_mat,
        draw_func func, const ObjSubMeshArgs* func_data, bool enable_bone_mat) {
        if (!obj_vert_buf || !obj_index_buf) {
            printf_debug("mdl::DispManager::entry_obj: no vertex or index object buffer to draw;\n");
            printf_debug("    Object: %s\n", obj->name);
            return false;
        }

        ::camera* cam = rctx_ptr->camera;

        if (object_culling && !instances_count && !bone_mat && (!obj
            || !obj_bounding_sphere_check_visibility(
                obj->bounding_sphere, &culling, cam, mat))) {
            culling.culled.objects++;
            return false;
        }
        culling.passed.objects++;

        for (int32_t i = 0; i < obj->num_mesh; i++) {
            const obj_mesh* mesh = &obj->mesh_array[i];
            const obj_mesh* mesh_morph = 0;
            if (obj_vert_buf && obj_morph_vert_buf) {
                if (obj_vert_buf[i].get_size() != obj_morph_vert_buf[i].get_size())
                    continue;

                if (obj_morph && i < obj_morph->num_mesh)
                    mesh_morph = &obj_morph->mesh_array[i];
            }

            if (object_culling && !instances_count && !bone_mat
                && !obj_bounding_sphere_check_visibility(
                    mesh->bounding_sphere, &culling, cam, mat)
                && (!mesh_morph || !obj_bounding_sphere_check_visibility(
                    mesh_morph->bounding_sphere, &culling, cam, mat))) {
                culling.culled.meshes++;
                continue;
            }
            culling.passed.meshes++;

            ObjSubMeshArgs* translucent_priority[40];
            int32_t translucent_priority_count = 0;

            for (int32_t j = 0; j < mesh->num_submesh; j++) {
                const obj_sub_mesh* sub_mesh = &mesh->submesh_array[j];
                const obj_sub_mesh* sub_mesh_morph = 0;
                if (sub_mesh->attrib.m.cloth)
                    continue;

                if (object_culling && !instances_count && !bone_mat) {
                    int32_t v32 = obj_bounding_sphere_check_visibility(
                        sub_mesh->bounding_sphere, &culling, cam, mat);
                    if (v32 != 2 || (!mesh->attrib.m.billboard && !mesh->attrib.m.billboard_y_axis)) {
                        if (v32 == 2)
                            v32 = obj_axis_aligned_bounding_box_check_visibility(
                                &sub_mesh->axis_aligned_bounding_box, &culling, cam, mat);

                        if (!v32) {
                            if (!mesh_morph || j >= mesh_morph->num_submesh) {
                                culling.culled.sub_meshes++;
                                continue;
                            }

                            sub_mesh_morph = &mesh_morph->submesh_array[j];
                            if (!sub_mesh_morph) {
                                culling.culled.sub_meshes++;
                                continue;
                            }

                            int32_t v32 = obj_bounding_sphere_check_visibility(
                                sub_mesh_morph->bounding_sphere, &culling, cam, mat);
                            if (v32 == 2)
                                v32 = obj_axis_aligned_bounding_box_check_visibility(
                                    &sub_mesh_morph->axis_aligned_bounding_box, &culling, cam, mat);

                            if (!v32) {
                                culling.culled.sub_meshes++;
                                continue;
                            }
                        }
                    }
                }
                culling.passed.sub_meshes++;

                int32_t num_bone_index = sub_mesh->num_bone_index;
                const uint16_t* bone_index = sub_mesh->bone_index_array;

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

                obj_material_data* material = &obj->material_array[sub_mesh->material_index];
                ObjData* data = alloc_obj_data(OBJ_KIND_NORMAL);
                if (!data)
                    continue;

                GLuint morph_vertex_buffer = 0;
                size_t morph_vertex_buffer_offset = 0;
                if (obj_morph_vert_buf) {
                    morph_vertex_buffer = obj_morph_vert_buf[i].get_buffer();
                    morph_vertex_buffer_offset = obj_morph_vert_buf[i].get_offset();
                }

                GLuint index_buffer = 0;
                if (obj_index_buf)
                    index_buffer = obj_index_buf[i].buffer;

                GLuint vertex_buffer = 0;
                size_t vertex_buffer_offset = 0;
                if (obj_vert_buf) {
                    vertex_buffer = obj_vert_buf[i].get_buffer();
                    vertex_buffer_offset = obj_vert_buf[i].get_offset();
                }

                if (!vertex_buffer || !index_buffer || obj_morph_vert_buf && !morph_vertex_buffer) {
                    printf_debug("mdl::DispManager::entry_obj: no vertex or index mesh buffer to draw;\n");
                    printf_debug("    Object: %s; Mesh: %s; Sub Mesh: %d\n", obj->name, mesh->name, j);
                    continue;
                }

                const uint32_t material_hash = hash_utf8_murmurhash(material->material.name);

                const material_list_struct* mat_list = 0;
                for (int32_t k = 0; k < material_list_count; k++)
                    if (material_list_array[k].hash == material_hash) {
                        mat_list = &material_list_array[k];
                        break;
                    }

                vec4 _blend_color = 1.0f;
                vec4 _emission = { 0.0f, 0.0f, 0.0f, 1.0f };

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

                data->init_sub_mesh(this, mat, obj->bounding_sphere.radius, sub_mesh, mesh, material, textures,
                    num_bone_index, mats, vertex_buffer, vertex_buffer_offset, index_buffer, _blend_color, _emission,
                    morph_vertex_buffer, morph_vertex_buffer_offset, instances_count, instances_mat, func, func_data);

                if (obj_flags & mdl::OBJ_SHADOW_OBJECT) {
                    entry_list((ObjType)(OBJ_TYPE_SHADOW_OBJECT_CHARA
                        + shadow_type), data);
                    if (obj_flags & mdl::OBJ_USER)
                        entry_list(OBJ_TYPE_USER, data);
                    continue;
                }

                const obj_material_attrib_member attrib = material->material.attrib.m;
                if ((obj_flags & mdl::OBJ_ALPHA_ORDER) && data->args.sub_mesh.blend_color.w < 1.0f) {
                    if (!(obj_flags & mdl::OBJ_NO_TRANSLUCENCY)) {
                        bool translucent = false;
                        if (!attrib.flag_28 && (!attrib.punch_through
                            && (attrib.alpha_texture || attrib.alpha_material)
                            || sub_mesh->attrib.m.translucent))
                            translucent = true;

                        if (translucent) {
                            if (obj_flags & mdl::OBJ_ALPHA_ORDER_POST_GLITTER)
                                entry_list(OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_POST_GLITTER, data);
                            else if (obj_flags & mdl::OBJ_ALPHA_ORDER_POST_TRANSLUCENT)
                                entry_list(OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_POST_TRANSLUCENT, data);
                            else
                                entry_list(OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_POST_OPAQUE, data);
                        }
                        else {
                            if (!attrib.punch_through) {
                                if (obj_flags & mdl::OBJ_ALPHA_ORDER_POST_GLITTER)
                                    entry_list(OBJ_TYPE_OPAQUE_ALPHA_ORDER_POST_GLITTER, data);
                                else if (obj_flags & mdl::OBJ_ALPHA_ORDER_POST_TRANSLUCENT)
                                    entry_list(OBJ_TYPE_OPAQUE_ALPHA_ORDER_POST_TRANSLUCENT, data);
                                else
                                    entry_list(OBJ_TYPE_OPAQUE_ALPHA_ORDER_POST_OPAQUE, data);
                            }
                            else {
                                if (obj_flags & mdl::OBJ_ALPHA_ORDER_POST_GLITTER)
                                    entry_list(OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_POST_GLITTER, data);
                                else if (obj_flags & mdl::OBJ_ALPHA_ORDER_POST_TRANSLUCENT)
                                    entry_list(OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_POST_TRANSLUCENT, data);
                                else
                                    entry_list(OBJ_TYPE_TRANSPARENT_ALPHA_ORDER_POST_OPAQUE, data);
                            }

                            if (obj_flags & mdl::OBJ_SSS)
                                entry_list(OBJ_TYPE_SSS, data);
                        }
                    }
                }
                else if (!attrib.flag_28 && (data->args.sub_mesh.blend_color.w < 1.0f
                    || !attrib.punch_through && attrib.alpha_texture | attrib.alpha_material
                    || sub_mesh->attrib.m.translucent)) {
                    if (!(obj_flags & mdl::OBJ_NO_TRANSLUCENCY)) {
                        if (!attrib.translucent_priority)
                            if (mesh->attrib.m.translucent_sort_by_radius
                                || obj_flags & mdl::OBJ_TRANSLUCENT_SORT_BY_RADIUS) {
                                entry_list(OBJ_TYPE_TRANSLUCENT_SORT_BY_RADIUS, data);
                            }
                            else
                                entry_list(OBJ_TYPE_TRANSLUCENT, data);
                        else if (translucent_priority_count < 40)
                            translucent_priority[translucent_priority_count++] = &data->args.sub_mesh;
                    }
                }
                else {
                    if (obj_flags & mdl::OBJ_SHADOW)
                        entry_list((ObjType)(OBJ_TYPE_SHADOW_CHARA + shadow_type), data);

                    if (obj_flags & mdl::OBJ_SSS)
                        entry_list(OBJ_TYPE_SSS, data);

                    if (attrib.punch_through) {
                        if (!(obj_flags & mdl::OBJ_NO_TRANSLUCENCY))
                            entry_list(OBJ_TYPE_TRANSPARENT, data);

                        if (obj_flags & mdl::OBJ_CHARA_REFLECT)
                            entry_list(OBJ_TYPE_REFLECT_CHARA_OPAQUE, data);

                        if (obj_flags & mdl::OBJ_REFLECT)
                            entry_list(OBJ_TYPE_REFLECT_OPAQUE, data);

                        if (obj_flags & mdl::OBJ_REFRACT)
                            entry_list(OBJ_TYPE_REFRACT_TRANSPARENT, data);
                    }
                    else {
                        if (!(obj_flags & mdl::OBJ_NO_TRANSLUCENCY))
                            entry_list(OBJ_TYPE_OPAQUE, data);

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

            if (translucent_priority_count <= 0)
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
            if (data) {
                data->init_translucent(mat, translucent_args);
                if (obj_flags & mdl::OBJ_ALPHA_ORDER_POST_GLITTER)
                    entry_list(OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_POST_GLITTER, data);
                else if (obj_flags & mdl::OBJ_ALPHA_ORDER_POST_TRANSLUCENT)
                    entry_list(OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_POST_TRANSLUCENT, data);
                else if (obj_flags & mdl::OBJ_ALPHA_ORDER_POST_OPAQUE)
                    entry_list(OBJ_TYPE_TRANSLUCENT_ALPHA_ORDER_POST_OPAQUE, data);
                else
                    entry_list(OBJ_TYPE_TRANSLUCENT, data);
            }
        }
        return true;
    }

    bool DispManager::entry_obj_screen(const ::obj* obj, const mat4& mat, obj_mesh_vertex_buffer* obj_vert_buf,
        obj_mesh_index_buffer* obj_index_buf, const std::vector<GLuint>* textures, const vec4* blend_color) {
        if (!obj_vert_buf || !obj_index_buf)
            return false;

        culling.passed.objects++;

        for (int32_t i = 0; i < obj->num_mesh; i++) {
            const obj_mesh* mesh = &obj->mesh_array[i];

            culling.passed.meshes++;

            ObjSubMeshArgs* translucent_priority[40];
            int32_t translucent_priority_count = 0;

            for (int32_t j = 0; j < mesh->num_submesh; j++) {
                const obj_sub_mesh* sub_mesh = &mesh->submesh_array[j];
                if (sub_mesh->attrib.m.cloth)
                    continue;

                culling.passed.sub_meshes++;

                obj_material_data* material = &obj->material_array[sub_mesh->material_index];
                ObjData* data = alloc_obj_data(OBJ_KIND_NORMAL);
                if (!data)
                    continue;

                GLuint index_buffer = 0;
                if (obj_index_buf)
                    index_buffer = obj_index_buf[i].buffer;

                GLuint vertex_buffer = 0;
                size_t vertex_buffer_offset = 0;
                if (obj_vert_buf) {
                    vertex_buffer = obj_vert_buf[i].get_buffer();
                    vertex_buffer_offset = obj_vert_buf[i].get_offset();
                }

                if (!vertex_buffer || !index_buffer)
                    continue;

                vec4 _blend_color = 1.0f;
                if (blend_color)
                    _blend_color = *blend_color;
                vec4 _emission = material->material.color.emission;

                data->init_sub_mesh(this, mat, obj->bounding_sphere.radius, sub_mesh, mesh, material, textures,
                    0, 0, vertex_buffer, vertex_buffer_offset, index_buffer, _blend_color, _emission,
                    0, 0, 0, 0, 0, 0);

                const obj_material_attrib_member attrib = material->material.attrib.m;
                if ((obj_flags & mdl::OBJ_ALPHA_ORDER) && data->args.sub_mesh.blend_color.w < 1.0f) {
                    if (!(obj_flags & mdl::OBJ_NO_TRANSLUCENCY)) {
                        bool translucent = false;
                        if (!attrib.flag_28 && (!attrib.punch_through
                            && (attrib.alpha_texture || attrib.alpha_material)
                            || sub_mesh->attrib.m.translucent))
                            translucent = true;

                        if (translucent) {
                            if (obj_flags & mdl::OBJ_ALPHA_ORDER_POST_GLITTER);
                            else if (obj_flags & mdl::OBJ_ALPHA_ORDER_POST_TRANSLUCENT)
                                entry_list(OBJ_TYPE_SCREEN_TRANSLUCENT_ALPHA_ORDER_POST_TRANSLUCENT, data);
                            else;
                        }
                        else {
                            if (!attrib.punch_through) {
                                if (obj_flags & mdl::OBJ_ALPHA_ORDER_POST_GLITTER);
                                else if (obj_flags & mdl::OBJ_ALPHA_ORDER_POST_TRANSLUCENT)
                                    entry_list(OBJ_TYPE_SCREEN_OPAQUE_ALPHA_ORDER_POST_TRANSLUCENT, data);
                                else;
                            }
                            else {
                                if (obj_flags & mdl::OBJ_ALPHA_ORDER_POST_GLITTER);
                                else if (obj_flags & mdl::OBJ_ALPHA_ORDER_POST_TRANSLUCENT)
                                    entry_list(OBJ_TYPE_SCREEN_TRANSPARENT_ALPHA_ORDER_POST_TRANSLUCENT, data);
                                else;
                            }
                        }
                    }
                }
                else if (!attrib.flag_28 && (data->args.sub_mesh.blend_color.w < 1.0f
                    || !attrib.punch_through && attrib.alpha_texture | attrib.alpha_material
                    || sub_mesh->attrib.m.translucent)) {
                    if (!(obj_flags & mdl::OBJ_NO_TRANSLUCENCY)) {
                        if (!attrib.translucent_priority)
                            if (mesh->attrib.m.translucent_sort_by_radius
                                || obj_flags & mdl::OBJ_TRANSLUCENT_SORT_BY_RADIUS);
                            else
                                entry_list(OBJ_TYPE_SCREEN_TRANSLUCENT, data);
                        else if (translucent_priority_count < 40)
                            translucent_priority[translucent_priority_count++] = &data->args.sub_mesh;
                    }
                }
                else {
                    if (attrib.punch_through) {
                        if (!(obj_flags & mdl::OBJ_NO_TRANSLUCENCY))
                            entry_list(OBJ_TYPE_SCREEN_TRANSPARENT, data);
                    }
                    else {
                        if (!(obj_flags & mdl::OBJ_NO_TRANSLUCENCY))
                            entry_list(OBJ_TYPE_SCREEN_OPAQUE, data);
                    }
                    continue;
                }
            }

            if (translucent_priority_count <= 0)
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
            if (data) {
                data->init_translucent(mat, translucent_args);
                if (obj_flags & mdl::OBJ_ALPHA_ORDER_POST_GLITTER);
                else if (obj_flags & mdl::OBJ_ALPHA_ORDER_POST_TRANSLUCENT)
                    entry_list(OBJ_TYPE_SCREEN_TRANSLUCENT_ALPHA_ORDER_POST_TRANSLUCENT, data);
                else if (obj_flags & mdl::OBJ_ALPHA_ORDER_POST_OPAQUE);
                else
                    entry_list(OBJ_TYPE_SCREEN_TRANSLUCENT, data);
            }
        }

        return true;
    }

    void DispManager::entry_obj_by_obj(const mat4& mat,
        const ::obj* obj, const std::vector<GLuint>* textures, obj_mesh_vertex_buffer* obj_vert_buf,
        obj_mesh_index_buffer* obj_index_buf, const mat4* bone_mat, float_t alpha) {
        if (!obj)
            return;

        vec4 blend_color = { 1.0f, 1.0f, 1.0f, alpha };
        vec4* blend_color_ptr = alpha < 1.0f ? &blend_color : 0;

        entry_obj(obj, mat, obj_vert_buf, obj_index_buf, textures, blend_color_ptr,
            bone_mat, 0, 0, 0, 0, 0, 0, !!bone_mat);
    }

    bool DispManager::entry_obj_by_object_info(const mat4& mat, object_info obj_info) {
        return entry_obj_by_object_info(mat, obj_info, 0, 0, 0, 0, 0, 0, false);
    }

    bool DispManager::entry_obj_by_object_info(const mat4& mat, object_info obj_info, const mat4* bone_mat) {
        vec4 blend_color = 1.0f;
        return entry_obj_by_object_info(mat, obj_info, &blend_color, bone_mat, 0, 0, 0, 0, true);
    }

    bool DispManager::entry_obj_by_object_info(const mat4& mat, object_info obj_info, const vec4* blend_color,
        const mat4* bone_mat, int32_t instances_count, const mat4* instances_mat,
        void(*func)(const ObjSubMeshArgs*), const ObjSubMeshArgs* func_data, bool enable_bone_mat) {
        if (obj_info.id == -1 && obj_info.set_id == -1)
            return false;

        ::obj* obj = objset_info_storage_get_obj(obj_info);
        if (!obj)
            return false;

        std::vector<GLuint>* textures = objset_info_storage_get_obj_set_gentex(obj_info.set_id);
        obj_mesh_vertex_buffer* obj_vert_buffer = objset_info_storage_get_obj_mesh_vertex_buffer(obj_info);
        obj_mesh_index_buffer* obj_index_buffer = objset_info_storage_get_obj_mesh_index_buffer(obj_info);

        ::obj* obj_morph = 0;
        obj_mesh_vertex_buffer* obj_morph_vert_buffer = 0;
        if (morph.object.set_id != -1) {
            obj_morph = objset_info_storage_get_obj(morph.object);
            obj_morph_vert_buffer = objset_info_storage_get_obj_mesh_vertex_buffer(morph.object);
        }

        return entry_obj(obj, mat, obj_vert_buffer, obj_index_buffer,
            textures, blend_color, bone_mat, obj_morph, obj_morph_vert_buffer,
            instances_count, instances_mat, func, func_data, enable_bone_mat);
    }

    bool DispManager::entry_obj_by_object_info_screen(const mat4& mat, object_info obj_info, const vec4* blend_color) {
        if (obj_info.id == (uint16_t)-1 && obj_info.set_id == (uint16_t)-1)
            return false;

        ::obj* obj = objset_info_storage_get_obj(obj_info);
        if (!obj)
            return false;

        std::vector<GLuint>* textures = objset_info_storage_get_obj_set_gentex(obj_info.set_id);
        obj_mesh_vertex_buffer* obj_vert_buffer = objset_info_storage_get_obj_mesh_vertex_buffer(obj_info);
        obj_mesh_index_buffer* obj_index_buffer = objset_info_storage_get_obj_mesh_index_buffer(obj_info);

        return entry_obj_screen(obj, mat, obj_vert_buffer, obj_index_buffer, textures, blend_color);
    }

    bool DispManager::entry_obj_by_object_info(const mat4& mat,
        object_info obj_info, float_t alpha, const mat4* bone_mat) {
        vec4 blend_color = 1.0f;
        blend_color.w = alpha;
        return entry_obj_by_object_info(mat, obj_info, &blend_color, bone_mat, 0, 0, 0, 0, !!bone_mat);
    }

    bool DispManager::entry_obj_by_object_info(const mat4& mat, object_info obj_info,
        float_t r, float_t g, float_t b, float_t a, const mat4* bone_mat) {
        vec4 blend_color = { r, g, b, a };
        return entry_obj_by_object_info(mat, obj_info, &blend_color, bone_mat, 0, 0, 0, 0, !!bone_mat);
    }

    bool DispManager::entry_obj_by_object_info(const mat4& mat, object_info obj_info,
        const vec4* blend_color, const mat4* bone_mat) {
        return entry_obj_by_object_info(mat, obj_info, blend_color, 0, 0, 0, 0, 0, !!bone_mat);
    }

    bool DispManager::entry_obj_by_object_info_instanced(object_info obj_info,
        const std::vector<mat4>& instances, float_t alpha) {
        return entry_obj_by_object_info_instanced(obj_info, instances, 1.0f, 1.0f, 1.0f, alpha);
    }

    bool DispManager::entry_obj_by_object_info_instanced(object_info obj_info,
        const std::vector<mat4>& instances, float_t r, float_t g, float_t b, float_t a) {
        const vec4 blend_color = { r, g, b, a };
        if (!instances.size())
            return false;

        mat4* instances_mat = alloc_mat4_array((int32_t)instances.size());
        if (!instances_mat)
            return false;

        memmove(instances_mat, instances.data(), sizeof(mat4) * instances.size());
        return entry_obj_by_object_info(mat4_identity, obj_info, a >= 0.0f ? &blend_color : (vec4*)0,
            0, (int32_t)instances.size(), instances_mat, 0, 0, true);
    }

    void DispManager::entry_obj_by_object_info_object_skin(object_info obj_info,
        const std::vector<texture_pattern_struct>* texture_pattern,
        const texture_data_struct* texture_data, float_t alpha,
        const mat4* matrices, const mat4* ex_data_matrices, const mat4* mat, const mat4& global_mat) {
        obj_skin* skin = objset_info_storage_get_obj_skin(obj_info);
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

    void DispManager::entry_obj_etc(const mat4& mat, const EtcObj& etc) {
        int32_t first = 0;
        int32_t count = 0;
        if (!rctx_ptr->etc_obj_manager->add_obj(first, count, etc))
            return;

        ObjData* data = alloc_obj_data(OBJ_KIND_ETC);
        if (!data)
            return;

        data->init_etc(this, mat, first, count, etc);
        if (etc.color.a == 0xFF) {
            if (obj_flags & OBJ_SHADOW)
                entry_list((mdl::ObjType)(OBJ_TYPE_SHADOW_CHARA + shadow_type), data);
            entry_list(OBJ_TYPE_OPAQUE, data);
        }
        else
           entry_list(OBJ_TYPE_TRANSLUCENT, data);
    }

    void DispManager::entry_obj_etc_screen(const mat4& mat, const EtcObj& etc) {
        int32_t first = 0;
        int32_t count = 0;
        if (!rctx_ptr->etc_obj_manager->add_obj(first, count, etc))
            return;

        ObjData* data = alloc_obj_data(OBJ_KIND_ETC);
        if (!data)
            return;

        data->init_etc(this, mat, first, count, etc);
        if (etc.color.a == 0xFF)
            entry_list(OBJ_TYPE_SCREEN_OPAQUE, data);
        else
            entry_list(OBJ_TYPE_SCREEN_TRANSLUCENT, data);
    }

    void DispManager::entry_obj_user(const mat4& mat, UserArgsFunc func, void* data, ObjType type) {
        ObjData* _data = alloc_obj_data(OBJ_KIND_USER);
        if (_data) {
            _data->init_user(mat, func, data);
            entry_list(type, _data);
        }
    }

    GLuint DispManager::get_vertex_array(const obj_mesh* mesh, const obj_sub_mesh* sub_mesh,
        const obj_material_data* material, GLuint vertex_buffer, size_t vertex_buffer_offset,
        GLuint index_buffer, GLuint morph_vertex_buffer, size_t morph_vertex_buffer_offset) {
        if (!vertex_buffer || !index_buffer)
            return 0;

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

        uint32_t compression = mesh->attrib.m.compression;
        GLsizei size_vertex = (GLsizei)mesh->size_vertex;
        obj_vertex_format vertex_format = mesh->vertex_format;

        for (DispManager::vertex_array& i : vertex_array_cache)
            if (i.vertex_buffer == vertex_buffer
                && i.vertex_buffer_offset == vertex_buffer_offset
                && i.morph_vertex_buffer == morph_vertex_buffer
                && i.morph_vertex_buffer_offset == morph_vertex_buffer_offset
                && i.index_buffer == index_buffer && i.vertex_format == vertex_format
                && i.size_vertex == size_vertex && i.compression == compression
                && !memcmp(i.texcoord_array, texcoord_array, sizeof(texcoord_array)))
                return i.vertex_array;
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

    void DispManager::get_obj_center(const mat4& mat, const ObjSubMeshArgs* args, vec3& center) {
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

    int32_t DispManager::get_obj_count(ObjTypeScreen type) {
        return (int32_t)obj_screen[type].size();
    }

    int32_t DispManager::get_obj_count(ObjTypeReflect type) {
        return (int32_t)obj_reflect[type].size();
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

    void DispManager::obj_sort(render_data_context& rend_data_ctx,
        ObjType type, int32_t compare_func, const cam_data& cam, bool a3) {
        std::list<ObjData*>& list = obj[type];
        if (list.size() < 1)
            return;

        if (a3 && type == mdl::OBJ_TYPE_TRANSLUCENT)
            sub_140436760(cam);

        calc_obj_radius(cam, type);

        switch (compare_func) {
        case 0:
            list.sort([](const ObjData* left, const ObjData* right) {
                return left->view_z > right->view_z;
            });
            break;
        case 1:
            list.sort([](const ObjData* left, const ObjData* right) {
                return left->view_z < right->view_z;
            });
            break;
        case 2:
            list.sort([](const ObjData* left, const ObjData* right) {
                return left->radius > right->radius;
            });
            break;
        }
    }

    void DispManager::obj_sort(render_data_context& rend_data_ctx,
        ObjTypeScreen type, int32_t compare_func, const cam_data& cam) {
        std::list<ObjData*>& list = obj[type];
        if (list.size() < 1)
            return;

        calc_obj_radius(cam, type);

        switch (compare_func) {
        case 0:
            list.sort([](const ObjData* left, const ObjData* right) {
                return left->view_z > right->view_z;
                });
            break;
        case 1:
            list.sort([](const ObjData* left, const ObjData* right) {
                return left->view_z < right->view_z;
                });
            break;
        case 2:
            list.sort([](const ObjData* left, const ObjData* right) {
                return left->radius > right->radius;
                });
            break;
        }
    }

    void DispManager::obj_sort(render_data_context& rend_data_ctx,
        ObjTypeReflect type, int32_t compare_func, const cam_data& cam) {
        std::list<ObjData*>& list = obj[type];
        if (list.size() < 1)
            return;

        calc_obj_radius(cam, type);

        switch (compare_func) {
        case 0:
            list.sort([](const ObjData* left, const ObjData* right) {
                return left->view_z > right->view_z;
                });
            break;
        case 1:
            list.sort([](const ObjData* left, const ObjData* right) {
                return left->view_z < right->view_z;
                });
            break;
        case 2:
            list.sort([](const ObjData* left, const ObjData* right) {
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

        for (std::list<ObjData*>& i : obj_screen)
            i.clear();

        for (std::list<ObjData*>& i : obj_reflect)
            i.clear();

        buffer_reset();
    }

    void DispManager::remove_index_buffer(GLuint buffer) {
        auto i_begin = vertex_array_cache.begin();
        auto i_end = vertex_array_cache.end();
        for (auto i = i_begin; i != i_end;)
            if (i->index_buffer == buffer) {
                glDeleteVertexArrays(1, &i->vertex_array);
                i = vertex_array_cache.erase(i);
                i_end = vertex_array_cache.end();
            }
            else
                i++;
    }

    void DispManager::remove_vertex_buffer(GLuint buffer) {
        auto i_begin = vertex_array_cache.begin();
        auto i_end = vertex_array_cache.end();
        for (auto i = i_begin; i != i_end;)
            if (i->vertex_buffer == buffer || i->morph_vertex_buffer == buffer) {
                glDeleteVertexArrays(1, &i->vertex_array);
                i = vertex_array_cache.erase(i);
                i_end = vertex_array_cache.end();
            }
            else
                i++;
    }

    void DispManager::set_chara_color(bool value) {
        chara_color = value;
    }

    void DispManager::set_culling_func(bool(*func)(const obj_bounding_sphere*, const mat4*)) {
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

    static void gen_capsule_indices(std::vector<uint32_t>& indices,
        int32_t slices, int32_t stacks, float_t length, bool wire, size_t start) {
        if (slices < 2 || stacks < 2)
            return;

        stacks = ((stacks + 1) / 2) * 2;

        if (length < 0.00001f)
            gen_sphere_indices(indices, slices, stacks, wire, start);

        if (!wire) {
            // Top stack vertices
            {
                int32_t m1 = 0;
                int32_t m2 = 1;

                indices.reserve(3LL * slices);

                for (int32_t j = 0, k = 1; j < slices; j++) {
                    indices.push_back((uint32_t)(start + m1));
                    indices.push_back((uint32_t)(start + j + m2));
                    indices.push_back((uint32_t)(start + k + m2));

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
                    indices.push_back((uint32_t)(start + j + m1));
                    indices.push_back((uint32_t)(start + j + m2));
                    indices.push_back((uint32_t)(start + k + m1));

                    indices.push_back((uint32_t)(start + k + m1));
                    indices.push_back((uint32_t)(start + j + m2));
                    indices.push_back((uint32_t)(start + k + m2));

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
                    indices.push_back((uint32_t)(start + j + m1));
                    indices.push_back((uint32_t)(start + m2));
                    indices.push_back((uint32_t)(start + k + m1));

                    if (++k >= slices)
                        k = 0;
                }
            }
        }
        else {
            // Top stack wireframe
            {
                int32_t m1 = 0;
                int32_t m2 = 1;

                indices.reserve(2LL * slices);

                for (int32_t j = 0; j < slices; j++) {
                    indices.push_back((uint32_t)(start + m1));
                    indices.push_back((uint32_t)(start + j + m2));
                }
            }

            // Middle stacks wireframe
            for (int32_t i = 1; i < stacks; i++) {
                int32_t m1 = (i - 1) * slices + 1;
                int32_t m2 = m1 + slices;

                indices.reserve(2LL * slices);

                for (int32_t j = 0; j < slices; j++) {
                    indices.push_back((uint32_t)(start + j + m1));
                    indices.push_back((uint32_t)(start + j + m2));
                }
            }

            // Bottom stack wireframe
            {
                int32_t m1 = (stacks - 1) * slices + 1;
                int32_t m2 = m1 + slices;

                indices.reserve(2LL * slices);

                for (int32_t j = 0; j < slices; j++) {
                    indices.push_back((uint32_t)(start + j + m1));
                    indices.push_back((uint32_t)(start + m2));
                }
            }

            // Slices wireframe
            for (int32_t i = 1; i <= stacks; i++) {
                int32_t m = (i - 1) * slices + 1;

                indices.reserve(2LL * slices);

                for (int32_t j = 0, k = 1; j < slices; j++) {
                    indices.push_back((uint32_t)(start + j + m));
                    indices.push_back((uint32_t)(start + k + m));

                    if (++k >= slices)
                        k = 0;
                }
            }
        }
    }

    static void gen_capsule_vertices(etc_obj_vertex_data* data,
        int32_t slices, int32_t stacks, float_t length, float_t radius) {
        stacks = ((stacks + 1) / 2) * 2;

        if (length < 0.00001f)
            gen_sphere_vertices(data, slices, stacks, radius);

        length *= 0.5f;

        *data++ = { { 0.0f, 0.0f, radius + length }, { 0.0f, 0.0f, 1.0f } };

        double_t slice_step = (M_PI * 2.0) / (double_t)slices;
        double_t stack_step = M_PI / (double_t)stacks;

        for (int32_t i = 1; i <= stacks / 2; i++) {
            float_t stack_angle = (float_t)((M_PI / 2.0) - (double_t)i * stack_step);
            float_t xy = cosf(stack_angle);
            float_t z = sinf(stack_angle);
            z *= radius;

            for (int32_t j = 0; j < slices; j++) {
                float_t slice_angle = (float_t)((double_t)j * slice_step);

                float_t x = xy * cosf(slice_angle);
                float_t y = xy * sinf(slice_angle);

                *data++ = { { x * radius, y * radius, z + length }, { x, y, z } };
            }
        }

        for (int32_t i = stacks / 2; i < stacks; i++) {
            float_t stack_angle = (float_t)((M_PI / 2.0) - (double_t)i * stack_step);
            float_t xy = cosf(stack_angle);
            float_t z = sinf(stack_angle);
            z *= radius;

            for (int32_t j = 0; j < slices; j++) {
                float_t slice_angle = (float_t)((double_t)j * slice_step);

                float_t x = xy * cosf(slice_angle);
                float_t y = xy * sinf(slice_angle);

                *data++ = { { x * radius, y * radius, z - length }, { x, y, z } };
            }
        }

        *data++ = { { 0.0f, 0.0f, -radius - length }, { 0.0f, 0.0f, -1.0f } };
    }

    static void gen_cylinder_indices(std::vector<uint32_t>& indices,
        int32_t slices, int32_t stacks, bool wire, size_t start) {
        if (slices < 2 || stacks < 0)
            return;

        if (!wire) {
            // Top cap vertices
            {
                int32_t m1 = 0;
                int32_t m2 = 1;

                indices.reserve(3LL * slices);

                for (int32_t j = 0, k = 1; j < slices; j++) {
                    indices.push_back((uint32_t)(start + m1));
                    indices.push_back((uint32_t)(start + j + m2));
                    indices.push_back((uint32_t)(start + k + m2));

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
                    indices.push_back((uint32_t)(start + j + m1));
                    indices.push_back((uint32_t)(start + j + m2));
                    indices.push_back((uint32_t)(start + k + m1));

                    indices.push_back((uint32_t)(start + k + m1));
                    indices.push_back((uint32_t)(start + j + m2));
                    indices.push_back((uint32_t)(start + k + m2));

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
                    indices.push_back((uint32_t)(start + j + m1));
                    indices.push_back((uint32_t)(start + m2));
                    indices.push_back((uint32_t)(start + k + m1));

                    if (++k >= slices)
                        k = 0;
                }
            }
        }
        else {
            // Stacks wireframe
            for (int32_t i = 0; i < stacks; i++) {
                int32_t m1 = i * slices + 1;
                int32_t m2 = m1 + slices;

                indices.reserve(2LL * slices);

                for (int32_t j = 0; j < slices; j++) {
                    indices.push_back((uint32_t)(start + j + m1));
                    indices.push_back((uint32_t)(start + j + m2));
                }
            }

            // Slices wireframe
            for (int32_t i = 0; i <= stacks; i++) {
                int32_t m = i * slices + 1;

                indices.reserve(2LL * slices);

                for (int32_t j = 0, k = 1; j < slices; j++) {
                    indices.push_back((uint32_t)(start + j + m));
                    indices.push_back((uint32_t)(start + k + m));

                    if (++k >= slices)
                        k = 0;
                }
            }
        }
    }

    static void gen_cylinder_vertices(etc_obj_vertex_data* data,
        int32_t slices, int32_t stacks, float_t base, float_t height) {
        float_t half_height = height * 0.5f;

        *data++ = { { 0.0f, 0.0f, stacks ? half_height : 0.0f }, { 0.0f, 1.0f, 0.0f } };

        double_t slice_step = (M_PI * 2.0) / (double_t)slices;

        for (int32_t i = 0; i <= stacks; i++) {
            float_t z = stacks ? lerp_def(half_height, -half_height, (float_t)i / (float_t)stacks) : 0.0f;

            for (int32_t j = 0; j < slices; j++) {
                float_t slice_angle = (float_t)((double_t)j * slice_step);

                float_t x = cosf(slice_angle);
                float_t y = sinf(slice_angle);

                *data++ = { { x * base, y * base, z }, { x, y, 0.0f } };
            }
        }

        *data++ = { { 0.0f, 0.0f, stacks ? -half_height : 0.0f }, { 0.0f, -1.0f, 0.0f } };
    }

    static void gen_sphere_indices(std::vector<uint32_t>& indices,
        int32_t slices, int32_t stacks, bool wire, size_t start) {
        if (slices < 2 || stacks < 2)
            return;

        if (!wire) {
            // Top stack vertices
            {
                int32_t m1 = 0;
                int32_t m2 = 1;

                indices.reserve(3LL * slices);

                for (int32_t j = 0, k = 1; j < slices; j++) {
                    indices.push_back((uint32_t)(start + m1));
                    indices.push_back((uint32_t)(start + k + m2));
                    indices.push_back((uint32_t)(start + j + m2));

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
                    indices.push_back((uint32_t)(start + j + m1));
                    indices.push_back((uint32_t)(start + k + m1));
                    indices.push_back((uint32_t)(start + j + m2));

                    indices.push_back((uint32_t)(start + k + m1));
                    indices.push_back((uint32_t)(start + k + m2));
                    indices.push_back((uint32_t)(start + j + m2));

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
                    indices.push_back((uint32_t)(start + j + m1));
                    indices.push_back((uint32_t)(start + k + m1));
                    indices.push_back((uint32_t)(start + m2));

                    if (++k >= slices)
                        k = 0;
                }
            }
        }
        else {
            // Top stack wireframe
            {
                int32_t m1 = 0;
                int32_t m2 = 1;

                indices.reserve(2LL * slices);

                for (int32_t j = 0; j < slices; j++) {
                    indices.push_back((uint32_t)(start + m1));
                    indices.push_back((uint32_t)(start + j + m2));
                }
            }

            // Middle stacks wireframe
            for (int32_t i = 1; i < stacks - 1; i++) {
                int32_t m1 = (i - 1) * slices + 1;
                int32_t m2 = m1 + slices;

                indices.reserve(2LL * slices);

                for (int32_t j = 0; j < slices; j++) {
                    indices.push_back((uint32_t)(start + j + m1));
                    indices.push_back((uint32_t)(start + j + m2));
                }
            }

            // Bottom stack wireframe
            {
                int32_t m1 = (stacks - 2) * slices + 1;
                int32_t m2 = m1 + slices;

                indices.reserve(2LL * slices);

                for (int32_t j = 0; j < slices; j++) {
                    indices.push_back((uint32_t)(start + j + m1));
                    indices.push_back((uint32_t)(start + m2));
                }
            }

            // Slices wireframe
            for (int32_t i = 1; i < stacks; i++) {
                int32_t m = (i - 1) * slices + 1;

                indices.reserve(2LL * slices);

                for (int32_t j = 0, k = 1; j < slices; j++) {
                    indices.push_back((uint32_t)(start + j + m));
                    indices.push_back((uint32_t)(start + k + m));

                    if (++k >= slices)
                        k = 0;
                }
            }
        }
    }

    static void gen_sphere_vertices(etc_obj_vertex_data* data,
        int32_t slices, int32_t stacks, float_t radius) {
        *data++ = { { 0.0f, 0.0f, radius }, { 0.0f, 0.0f, -1.0f } };

        double_t slice_step = (M_PI * 2.0) / (double_t)slices;
        double_t stack_step = M_PI / (double_t)stacks;

        for (int32_t i = 1; i < stacks; i++) {
            float_t stack_angle = (float_t)((M_PI / 2.0) - (double_t)i * stack_step);
            float_t xy = cosf(stack_angle);
            float_t z = sinf(stack_angle);

            for (int32_t j = 0; j < slices; j++) {
                float_t slice_angle = -(float_t)((double_t)j * slice_step);

                float_t x = xy * cosf(slice_angle);
                float_t y = xy * sinf(slice_angle);

                *data++ = { { x * radius, y * radius, z * radius }, { x, y, z } };
            }
        }

        *data++ = { { 0.0f, 0.0f, -radius }, { 0.0f, 0.0f, -1.0f } };
    }

    inline static void set_default_vertex_attrib() {
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
}

static void sub_140436760(const cam_data& cam) {
    for (auto& i : rctx_ptr->disp_manager->obj[mdl::OBJ_TYPE_TRANSLUCENT]) {
        if (i->kind != mdl::OBJ_KIND_NORMAL)
            continue;

        const obj_mesh* mesh = i->args.sub_mesh.mesh;
        if (!mesh)
            continue;

        const obj_sub_mesh* sub_mesh = i->args.sub_mesh.sub_mesh;
        const obj_axis_aligned_bounding_box* aabb = &sub_mesh->axis_aligned_bounding_box;
        if (mesh->attrib.m.billboard_y_axis && aabb->size.y > 1.0f && aabb->size.y < 1.2f) {
            if (!strcmp(mesh->name, "STGD2NS064_EFF_WATESMOKE_A_BMZ_000"))
                i->view_z += sub_mesh->bounding_sphere.radius;
            continue;
        }

        if (i->args.sub_mesh.material->material.attrib.m.src_blend_factor == OBJ_MATERIAL_BLEND_ZERO
            && i->args.sub_mesh.material->material.attrib.m.dst_blend_factor == OBJ_MATERIAL_BLEND_ONE
            && aabb->size.y > 2.0f && aabb->size.y < 2.5f) {
            if (strcmp(mesh->name, "STGD2NS064_EFF_DISPLAY_MZ_000") && strcmp(mesh->name, "STGD2NS064_EFF_RAY_B_MZ_000"))
                continue;
        }
        else if (aabb->size.y > 3.0f && aabb->size.x > 3.6f && aabb->size.x < 3.7f) {
            if (strcmp(mesh->name, "STGD2NS064_EFF_WATERFALL_A_MZ_000"))
                continue;
        }
        else
            continue;

        mat4 mat;
        mat4_mul_translate(&i->mat, &aabb->center, &mat);
        mat4_mul(&mat, &cam.get_view_mat(), &mat);
        mat4_transpose(&mat, &mat);

        vec4 t;
        *(vec3*)&t = aabb->size;
        t.w = 1.0f;

        float_t view_z = vec4::dot(t ^ vec4( 0.0f,  0.0f,  0.0f, 0.0f ), mat.row2);
        if (view_z < vec4::dot(t ^ vec4( 0.0f,  0.0f, -0.0f, 0.0f ), mat.row2))
            view_z = vec4::dot(t ^ vec4( 0.0f,  0.0f, -0.0f, 0.0f ), mat.row2);
        if (view_z < vec4::dot(t ^ vec4( 0.0f, -0.0f,  0.0f, 0.0f ), mat.row2))
            view_z = -t.y;
        if (view_z < vec4::dot(t ^ vec4( 0.0f, -0.0f, -0.0f, 0.0f ), mat.row2))
            view_z = vec4::dot(t ^ vec4( 0.0f, -0.0f, -0.0f, 0.0f ), mat.row2);
        if (view_z < vec4::dot(t ^ vec4(-0.0f,  0.0f,  0.0f, 0.0f ), mat.row2))
            view_z = -t.x;
        if (view_z < vec4::dot(t ^ vec4(-0.0f,  0.0f, -0.0f, 0.0f ), mat.row2))
            view_z = vec4::dot(t ^ vec4(-0.0f,  0.0f, -0.0f, 0.0f ), mat.row2);
        if (view_z < vec4::dot(t ^ vec4(-0.0f, -0.0f,  0.0f, 0.0f ), mat.row2))
            view_z = vec4::dot(t ^ vec4(-0.0f, -0.0f,  0.0f, 0.0f ), mat.row2);
        if (view_z < vec4::dot(t ^ vec4(-0.0f, -0.0f, -0.0f, 0.0f ), mat.row2))
            view_z = vec4::dot(t ^ vec4(-0.0f, -0.0f, -0.0f, 0.0f ), mat.row2);
        i->view_z = view_z;
    }
}
