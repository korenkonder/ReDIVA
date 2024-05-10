/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include <list>
#include <map>
#include <new>
#include <vector>
#include "object.hpp"
#include "data.hpp"
#include "gl_state.hpp"
#include "render_context.hpp"
#include "shader_ft.hpp"
#include "../KKdLib/io/file_stream.hpp"
#include "../KKdLib/io/json.hpp"
#include "../KKdLib/io/path.hpp"
#include "../KKdLib/dds.hpp"
#include "../KKdLib/hash.hpp"
#include "../KKdLib/msgpack.hpp"
#include "../KKdLib/str_utils.hpp"

static GLuint create_index_buffer(size_t size, const void* data);
static GLuint create_vertex_buffer(size_t size, const void* data, bool dynamic = false);
static void free_index_buffer(GLuint buffer);
static void free_vertex_buffer(GLuint buffer);

static uint32_t remove_degenerate_triangle_indices(
    uint32_t* dst_index_array, const uint32_t num_index, uint32_t* src_index_array);

static void ObjsetInfo_calc_axis_aligned_bounding_box(ObjsetInfo* info);
static void ObjsetInfo_get_shader_index_texture_index(ObjsetInfo* info);
static bool ObjsetInfo_index_buffer_load(ObjsetInfo* info);
static void ObjsetInfo_index_buffer_free(ObjsetInfo* info);
static bool ObjsetInfo_load_textures(ObjsetInfo* info, const void* data, bool big_endian);
static bool ObjsetInfo_load_textures_modern(ObjsetInfo* info,
    const void* data, size_t size, const char* file, texture_database* tex_db);
static bool ObjsetInfo_vertex_buffer_load(ObjsetInfo* info);
static void ObjsetInfo_vertex_buffer_free(ObjsetInfo* info);
static uint32_t obj_vertex_format_get_vertex_size(obj_vertex_format format);
static uint32_t obj_vertex_format_get_vertex_size_comp1(obj_vertex_format format);
static uint32_t obj_vertex_format_get_vertex_size_comp2(obj_vertex_format format);

std::map<uint32_t, ObjsetInfo> object_storage_data;
std::map<uint32_t, ObjsetInfo> object_storage_data_modern;

obj_mesh_index_buffer::obj_mesh_index_buffer() : buffer() {

}

bool obj_mesh_index_buffer::load(obj_mesh& mesh) {
    size_t num_index = 0;
    for (uint32_t i = 0; i < mesh.num_submesh; i++)
        num_index += mesh.submesh_array[i].num_index;

    if (!num_index) {
        buffer = 0;
        return true;
    }

    uint16_t* indices = force_malloc<uint16_t>(num_index);
    obj_mesh_index_buffer::fill_data(indices, mesh);

    bool ret = load_data(num_index * sizeof(uint16_t), indices);
    free_def(indices);
    return ret;
}

bool obj_mesh_index_buffer::load_data(size_t size, const void* data) {
    if (!size)
        return false;

    buffer = create_index_buffer(size, data);
    return true;
}

void obj_mesh_index_buffer::unload() {
    free_index_buffer(buffer);
    buffer = 0;
}

void* obj_mesh_index_buffer::fill_data(void* data, obj_mesh& mesh) {
    uint16_t* indices = (uint16_t*)data;
    for (uint32_t i = 0; i < mesh.num_submesh; i++) {
        obj_sub_mesh& sub_mesh = mesh.submesh_array[i];

        if (sub_mesh.primitive_type == OBJ_PRIMITIVE_TRIANGLE_STRIP && !(sub_mesh.attrib.w & 0x80)){
            uint32_t* index_array = force_malloc<uint32_t>(sub_mesh.num_index);
            sub_mesh.num_index = remove_degenerate_triangle_indices(
                index_array, sub_mesh.num_index, sub_mesh.index_array);
            memmove(sub_mesh.index_array, index_array, sizeof(uint32_t) * sub_mesh.num_index);
            free_def(index_array);
        }

        uint32_t num_index = sub_mesh.num_index;
        uint32_t* index = sub_mesh.index_array;
        for (uint32_t j = num_index; j; j--, index++)
            *indices++ = (uint16_t)*index;
    }

    indices = (uint16_t*)data;
    for (uint32_t i = 0, offset = 0; i < mesh.num_submesh; i++) {
        obj_sub_mesh& sub_mesh = mesh.submesh_array[i];

        sub_mesh.first_index = 0;
        sub_mesh.last_index = 0;
        sub_mesh.index_offset = 0;
        if (sub_mesh.index_format != OBJ_INDEX_U16)
            continue;

        uint16_t first_index = 0xFFFF;
        uint16_t last_index = 0;
        for (uint32_t j = sub_mesh.num_index; j; j--) {
            uint16_t index = *indices++;
            if (index == 0xFFFF)
                continue;

            if (first_index > index)
                first_index = index;
            if (last_index < index)
                last_index = index;
        }

        sub_mesh.first_index = first_index;
        sub_mesh.last_index = last_index;
        sub_mesh.index_offset = (int32_t)(offset * sizeof(uint16_t));
        offset += sub_mesh.num_index;
    }
    return (void*)indices;
}

#if SHARED_OBJECT_BUFFER
obj_mesh_vertex_buffer::obj_mesh_vertex_buffer() : count(), buffers(), size(), offset(), index() {
#else
obj_mesh_vertex_buffer::obj_mesh_vertex_buffer() : count(), buffers(), size(), index() {
#endif

}

void obj_mesh_vertex_buffer::cycle_index() {
    if (++index >= count)
        index = 0;
}

GLuint obj_mesh_vertex_buffer::get_buffer() {
    if (index < count)
        return buffers[index];
    return 0;
}

size_t obj_mesh_vertex_buffer::get_offset() {
#if SHARED_OBJECT_BUFFER
    if (buffers[0])
        return offset;
#endif
    return 0;
}

GLsizeiptr obj_mesh_vertex_buffer::get_size() {
    if (buffers[0])
        return size;
    return 0;
}

bool obj_mesh_vertex_buffer::load(obj_mesh& mesh, bool dynamic) {
    if (!mesh.num_vertex || !mesh.vertex_array)
        return false;

    uint32_t size_vertex;
    switch (mesh.attrib.m.compression) {
    case 0:
    default:
        size_vertex = obj_vertex_format_get_vertex_size(mesh.vertex_format);
        break;
    case 1:
        size_vertex = obj_vertex_format_get_vertex_size_comp1(mesh.vertex_format);
        break;
    case 2:
        size_vertex = obj_vertex_format_get_vertex_size_comp2(mesh.vertex_format);
        break;
    }

    void* vertex = force_malloc((size_t)size_vertex * mesh.num_vertex);
    obj_mesh_vertex_buffer::fill_data(vertex, mesh);
    mesh.size_vertex = size_vertex;

    bool ret = load_data((size_t)size_vertex * mesh.num_vertex,
        vertex, mesh.attrib.m.double_buffer ? 2 : 1, dynamic);
    free_def(vertex);
    return ret;
}

bool obj_mesh_vertex_buffer::load_data(size_t size, const void* data, int32_t count, bool dynamic) {
    if (!size || count > 3)
        return false;

    this->count = count;
    this->size = (GLsizeiptr)size;
#if SHARED_OBJECT_BUFFER
    offset = 0;
#endif

    for (int32_t i = 0; i < count; i++) {
        buffers[i] = create_vertex_buffer(size, data, dynamic);
        if (!buffers[i]) {
            unload();
            return false;
        }
    }
    return true;
}

void obj_mesh_vertex_buffer::unload() {
    for (int32_t i = 0; i < count; i++) {
        free_vertex_buffer(buffers[i]);
        buffers[i] = 0;
    }

    count = 0;
#if SHARED_OBJECT_BUFFER
    offset = 0;
#endif
    size = 0;
    index = 0;
}

void* obj_mesh_vertex_buffer::fill_data(void* data, obj_mesh& mesh) {
    obj_vertex_format vertex_format = mesh.vertex_format;
    obj_vertex_data* vtx = mesh.vertex_array;
    uint32_t num_vertex = mesh.num_vertex;
    size_t d = (size_t)data;
    switch (mesh.attrib.m.compression) {
    case 0:
    default:
        for (uint32_t i = num_vertex; i; i--, vtx++) {
            if (vertex_format & OBJ_VERTEX_POSITION) {
                *(vec3*)d = vtx->position;
                d += 12;
            }

            if (vertex_format & OBJ_VERTEX_NORMAL) {
                *(vec3*)d = vtx->normal;
                d += 12;
            }

            if (vertex_format & OBJ_VERTEX_TANGENT) {
                *(vec4*)d = vtx->tangent;
                d += 16;
            }

            if (vertex_format & OBJ_VERTEX_BINORMAL) {
                *(vec3*)d = vtx->binormal;
                d += 12;
            }

            if (vertex_format & OBJ_VERTEX_TEXCOORD0) {
                *(vec2*)d = vtx->texcoord0;
                d += 8;
            }

            if (vertex_format & OBJ_VERTEX_TEXCOORD1) {
                *(vec2*)d = vtx->texcoord1;
                d += 8;
            }

            if (vertex_format & OBJ_VERTEX_TEXCOORD2) {
                *(vec2*)d = vtx->texcoord2;
                d += 8;
            }

            if (vertex_format & OBJ_VERTEX_TEXCOORD3) {
                *(vec2*)d = vtx->texcoord3;
                d += 8;
            }

            if (vertex_format & OBJ_VERTEX_COLOR0) {
                *(vec4*)d = vtx->color0;
                d += 16;
            }

            if (vertex_format & OBJ_VERTEX_COLOR1) {
                *(vec4*)d = vtx->color1;
                d += 16;
            }

            if (vertex_format & OBJ_VERTEX_BONE_DATA) {
                *(vec4*)d = vtx->bone_weight;
                d += 16;
                *(vec4i16*)d = vtx->bone_index;
                d += 8;
            }

            if (vertex_format & OBJ_VERTEX_UNKNOWN) {
                *(vec4*)d = vtx->unknown;
                d += 16;
            }
        }
        break;
    case 1:
        for (uint32_t i = num_vertex; i; i--, vtx++) {
            if (vertex_format & OBJ_VERTEX_POSITION) {
                *(vec3*)d = vtx->position;
                d += 12;
            }

            if (vertex_format & OBJ_VERTEX_NORMAL) {
                vec3_to_vec3i16(vtx->normal * 32767.0f, *(vec3i16*)d);
                *(int16_t*)(d + 6) = 0;
                d += 8;
            }

            if (vertex_format & OBJ_VERTEX_TANGENT) {
                vec4_to_vec4i16(vtx->tangent * 32767.0f, *(vec4i16*)d);
                d += 8;
            }

            if (vertex_format & OBJ_VERTEX_TEXCOORD0) {
                vec2_to_vec2h(vtx->texcoord0, *(vec2h*)d);
                d += 4;
            }

            if (vertex_format & OBJ_VERTEX_TEXCOORD1) {
                vec2_to_vec2h(vtx->texcoord1, *(vec2h*)d);
                d += 4;
            }

            if (vertex_format & OBJ_VERTEX_TEXCOORD2) {
                vec2_to_vec2h(vtx->texcoord2, *(vec2h*)d);
                d += 4;
            }

            if (vertex_format & OBJ_VERTEX_TEXCOORD3) {
                vec2_to_vec2h(vtx->texcoord3, *(vec2h*)d);
                d += 4;
            }

            if (vertex_format & OBJ_VERTEX_COLOR0) {
                vec4_to_vec4h(vtx->color0, *(vec4h*)d);
                d += 8;
            }

            if (vertex_format & OBJ_VERTEX_BONE_DATA) {
                vec4_to_vec4u16(vtx->bone_weight * 65535.0f, *(vec4u16*)d);
                d += 8;

                *(vec4i16*)d = vtx->bone_index;
                d += 8;
            }
        }
        break;
    case 2:
        for (uint32_t i = num_vertex; i; i--, vtx++) {
            if (vertex_format & OBJ_VERTEX_POSITION) {
                *(vec3*)d = vtx->position;
                d += 12;
            }

            if (vertex_format & OBJ_VERTEX_NORMAL) {
                vec3i16 normal_int;
                vec3_to_vec3i16(vtx->normal * 511.0f, normal_int);
                *(uint32_t*)d = (((uint32_t)0 & 0x03) << 30)
                    | (((uint32_t)normal_int.z & 0x3FF) << 20)
                    | (((uint32_t)normal_int.y & 0x3FF) << 10)
                    | ((uint32_t)normal_int.x & 0x3FF);
                d += 4;
            }

            if (vertex_format & OBJ_VERTEX_TANGENT) {
                vec4i16 tangent_int;
                vec4_to_vec4i16(vtx->tangent * 511.0f, tangent_int);
                *(uint32_t*)d = (((uint32_t)clamp_def(tangent_int.w, -1, 1) & 0x03) << 30)
                    | (((uint32_t)tangent_int.z & 0x3FF) << 20)
                    | (((uint32_t)tangent_int.y & 0x3FF) << 10)
                    | ((uint32_t)tangent_int.x & 0x3FF);
                d += 4;
            }

            if (vertex_format & OBJ_VERTEX_TEXCOORD0) {
                vec2_to_vec2h(vtx->texcoord0, *(vec2h*)d);
                d += 4;
            }

            if (vertex_format & OBJ_VERTEX_TEXCOORD1) {
                vec2_to_vec2h(vtx->texcoord1, *(vec2h*)d);
                d += 4;
            }

            if (vertex_format & OBJ_VERTEX_TEXCOORD2) {
                vec2_to_vec2h(vtx->texcoord2, *(vec2h*)d);
                d += 4;
            }

            if (vertex_format & OBJ_VERTEX_TEXCOORD3) {
                vec2_to_vec2h(vtx->texcoord3, *(vec2h*)d);
                d += 4;
            }

            if (vertex_format & OBJ_VERTEX_COLOR0) {
                vec4_to_vec4h(vtx->color0, *(vec4h*)d);
                d += 8;
            }

            if (vertex_format & OBJ_VERTEX_BONE_DATA) {
                vec4i16 bone_weight_int;
                vec4_to_vec4i16(vtx->bone_weight * 1023.0f, bone_weight_int);
                *(uint32_t*)d = (((uint32_t)0 & 0x03) << 30)
                    | (((uint32_t)bone_weight_int.z & 0x3FF) << 20)
                    | (((uint32_t)bone_weight_int.y & 0x3FF) << 10)
                    | ((uint32_t)bone_weight_int.x & 0x3FF);
                d += 4;

                vec4i bone_index;
                vec4i16_to_vec4i(vtx->bone_index, bone_index);
                vec4i_to_vec4u8(bone_index, *(vec4u8*)d);
                d += 4;
            }
        }
        break;
    }
    return (void*)d;
}

#if SHARED_OBJECT_BUFFER
obj_index_buffer::obj_index_buffer() : mesh_num(), mesh_data(), buffer() {
#else
obj_index_buffer::obj_index_buffer() : mesh_num(), mesh_data() {
#endif

}

bool obj_index_buffer::load(obj* obj) {
    if (!obj)
        return false;

    mesh_num = obj->num_mesh;
    mesh_data = new obj_mesh_index_buffer[obj->num_mesh];
    if (!mesh_data)
        return false;

#if SHARED_OBJECT_BUFFER
    size_t buffer_size = 0;
    for (uint32_t i = 0; i < mesh_num; i++) {
        obj_mesh& mesh = obj->mesh_array[i];
        size_t num_index = 0;
        for (uint32_t i = 0; i < mesh.num_submesh; i++)
            num_index += mesh.submesh_array[i].num_index;
        buffer_size += num_index * sizeof(uint16_t);
    }

    void* index = force_malloc(buffer_size);
    if (!index) {
        buffer = 0;
        unload();
        return false;
    }

    void* data = index;
    for (uint32_t i = 0; i < mesh_num; i++) {
        uint32_t offset = (uint32_t)((size_t)data - (size_t)index);
        data = obj_mesh_index_buffer::fill_data(data, obj->mesh_array[i]);

        obj_mesh& mesh = obj->mesh_array[i];
        for (uint32_t j = 0; j < mesh.num_submesh; j++)
            mesh.submesh_array[j].index_offset += offset;
    }

    buffer = create_index_buffer(buffer_size, index);
    if (!buffer) {
        free_def(index);
        unload();
        return false;
    }

    free_def(index);

    for (uint32_t i = 0; i < mesh_num; i++)
        mesh_data[i].buffer = buffer;
#else
    for (uint32_t i = 0; i < mesh_num; i++)
        if (!mesh_data[i].load(obj->mesh_array[i]))
            return false;
#endif
    return true;
}

void obj_index_buffer::unload() {
    if (mesh_data) {
#if SHARED_OBJECT_BUFFER
        free_index_buffer(buffer);
        buffer = 0;
#else
        for (uint32_t i = 0; i < mesh_num; i++)
            mesh_data[i].unload();
#endif
        delete[] mesh_data;
    }
    mesh_data = 0;
    mesh_num = 0;
#if SHARED_OBJECT_BUFFER
    buffer = 0;
#endif
}

#if SHARED_OBJECT_BUFFER
obj_vertex_buffer::obj_vertex_buffer() : mesh_num(), mesh_data(), buffers() {
#else
obj_vertex_buffer::obj_vertex_buffer() : mesh_num(), mesh_data() {
#endif

}

bool obj_vertex_buffer::load(obj* obj) {
    if (!obj)
        return false;

    mesh_num = obj->num_mesh;
    mesh_data = new obj_mesh_vertex_buffer[obj->num_mesh];
    if (!mesh_data)
        return false;

#if SHARED_OBJECT_BUFFER
    size_t buffer_size = 0;
    bool double_buffer = false;
    for (uint32_t i = 0; i < mesh_num; i++) {
        obj_mesh& mesh = obj->mesh_array[i];
        if (!mesh.num_vertex || !mesh.vertex_array)
            continue;

        uint32_t size_vertex;
        switch (mesh.attrib.m.compression) {
        case 0:
        default:
            size_vertex = obj_vertex_format_get_vertex_size(mesh.vertex_format);
            break;
        case 1:
            size_vertex = obj_vertex_format_get_vertex_size_comp1(mesh.vertex_format);
            break;
        case 2:
            size_vertex = obj_vertex_format_get_vertex_size_comp2(mesh.vertex_format);
            break;
        }
        mesh.size_vertex = size_vertex;

        buffer_size += (size_t)size_vertex * mesh.num_vertex;
        double_buffer |= !!mesh.attrib.m.double_buffer;
    }

    int32_t count = double_buffer ? 2 : 1;

    void* vertex = force_malloc(buffer_size);
    if (!vertex) {
        for (int32_t i = 0; i < count; i++)
            buffers[i] = 0;
        unload();
        return false;
    }

    void* data = vertex;
    for (uint32_t i = 0; i < mesh_num; i++) {
        obj_mesh_vertex_buffer& mesh_buffer = mesh_data[i];
        mesh_buffer.offset = (size_t)data - (size_t)vertex;
        mesh_buffer.count = count;
        mesh_buffer.size = (GLsizeiptr)buffer_size;

        data = obj_mesh_vertex_buffer::fill_data(data, obj->mesh_array[i]);
    }

    for (int32_t i = 0; i < count; i++) {
        buffers[i] = create_vertex_buffer(buffer_size, vertex);
        if (!buffers[i]) {
            free_def(vertex);
            unload();
            return false;
        }
    }

    free_def(vertex);

    for (uint32_t i = 0; i < mesh_num; i++)
        memcpy(mesh_data[i].buffers, buffers, count * sizeof(GLuint));
#else
    for (uint32_t i = 0; i < mesh_num; i++)
        if (!mesh_data[i].load(obj->mesh_array[i]))
            return false;
#endif
    return true;
}

void obj_vertex_buffer::unload() {
    if (mesh_data) {
#if SHARED_OBJECT_BUFFER
        for (int32_t i = 0; i < mesh_data[0].count; i++)
            free_vertex_buffer(buffers[i]);
#else
        for (uint32_t i = 0; i < mesh_num; i++)
            mesh_data[i].unload();
#endif
        delete[] mesh_data;
    }
    mesh_data = 0;
    mesh_num = 0;
#if SHARED_OBJECT_BUFFER
    buffers[0] = 0;
#endif
}

ObjsetInfo::ObjsetInfo() : obj_loaded(), tex_loaded(), obj_set(),
tex_num(), tex_data(), set_id(), objvb_num(), objvb(),
objib_num(), objib(), load_count(), modern() {

}

ObjsetInfo::~ObjsetInfo() {
    if (tex_data) {
        texture_array_free(tex_data);
        tex_data = 0;
    }

    ObjsetInfo_index_buffer_free(this);
    ObjsetInfo_vertex_buffer_free(this);
    alloc_handler.reset();
    while (tex_file_handler.ptr && tex_file_handler.ptr->count)
        tex_file_handler.reset();
    while (obj_file_handler.ptr && obj_file_handler.ptr->count)
        obj_file_handler.reset();
    while (farc_file_handler.ptr && farc_file_handler.ptr->count)
        farc_file_handler.reset();
}

inline int32_t obj_material_texture_type_get_texcoord_index(
    obj_material_texture_type type, int32_t index) {
    switch (type) {
    case OBJ_MATERIAL_TEXTURE_COLOR:
    case OBJ_MATERIAL_TEXTURE_ENVIRONMENT_SPHERE: // XHD
        if (index < 2)
            return index;
    case OBJ_MATERIAL_TEXTURE_NORMAL:
    case OBJ_MATERIAL_TEXTURE_SPECULAR:
        return 0;
    case OBJ_MATERIAL_TEXTURE_TRANSLUCENCY:
    case OBJ_MATERIAL_TEXTURE_TRANSPARENCY:
        return 1;
    }
    return -1;
}

inline int32_t obj_material_texture_type_get_texture_index(
    obj_material_texture_type type, int32_t base_index) {
    switch (type) {
    case OBJ_MATERIAL_TEXTURE_COLOR:
    case OBJ_MATERIAL_TEXTURE_ENVIRONMENT_SPHERE: // XHD
        if (base_index < 2)
            return base_index;
    case OBJ_MATERIAL_TEXTURE_NORMAL:
        return 2;
    case OBJ_MATERIAL_TEXTURE_SPECULAR:
        return 3;
    case OBJ_MATERIAL_TEXTURE_TRANSLUCENCY:
        return 1;
    case OBJ_MATERIAL_TEXTURE_TRANSPARENCY:
        return 4;
    //case OBJ_MATERIAL_TEXTURE_ENVIRONMENT_SPHERE: // AFT
    case OBJ_MATERIAL_TEXTURE_ENVIRONMENT_CUBE:
        return 5;
    }
    return -1;
}

void obj_skin_set_matrix_buffer(const obj_skin* s, const mat4* matrices,
    const mat4* ex_data_matrices, mat4* matrix_buffer, const mat4* mat, const mat4* global_mat) {
    if (!s->num_bone)
        return;

    obj_skin_bone* bone = s->bone_array;
    if (mat)
        for (uint32_t i = 0; i < s->num_bone; i++, bone++, matrix_buffer++) {
            mat4 temp;
            if (bone->id & 0x8000)
                mat4_mul(mat, &ex_data_matrices[bone->id & 0x7FFF], &temp);
            else
                mat4_mul(mat, &matrices[bone->id], &temp);

            mat4_mul(&temp, global_mat, &temp);
            mat4_mul(&bone->inv_bind_pose_mat, &temp, matrix_buffer);
        }
    else
        for (uint32_t i = 0; i < s->num_bone; i++, bone++, matrix_buffer++) {
            mat4 temp;
            if (bone->id & 0x8000)
                temp = ex_data_matrices[bone->id & 0x7FFF];
            else
                temp = matrices[bone->id];

            mat4_mul(&temp, global_mat, &temp);
            mat4_mul(&bone->inv_bind_pose_mat, &temp, matrix_buffer);
        }
}

void object_material_msgpack_read(const char* path, const char* set_name,
    obj_set* obj_set, object_database* obj_db) {
    if (!path_check_directory_exists(path))
        return;

    char set_name_buf[0x80];
    for (const char* i = set_name; *i && *i != '.'; i++) {
        char c = *i;
        if (c >= 'a' && c <= 'z')
            c -= 0x20;
        set_name_buf[i - set_name] = c;
        set_name_buf[i - set_name + 1] = 0;
    }

    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "%s\\%s\\", path, set_name_buf);
    if (!path_check_directory_exists(buf))
        return;

    sprintf_s(buf, sizeof(buf), "%s\\%s\\config.json", path, set_name_buf);
    if (!path_check_file_exists(buf))
        return;

    msgpack msg;

    file_stream s;
    s.open(buf, "rb");
    io_json_read(s, &msg);
    s.close();

    if (msg.type != MSGPACK_ARRAY)
        return;

    msgpack_array* ptr = msg.data.arr;
    for (msgpack& i : *ptr) {
        msgpack& object = i;

        std::string name = object.read_string("name");
        uint32_t name_hash = hash_string_murmurhash(name);

        for (uint32_t i = 0; i < obj_set->obj_num; i++) {
            obj* obj = obj_set->obj_data[i];

            if (name_hash != hash_string_murmurhash(obj->name))
                continue;

            msgpack* materials = object.read_array("material");
            if (materials) {
                msgpack_array* ptr = materials->data.arr;
                for (msgpack& j : *ptr) {
                    msgpack& material = j;

                    std::string name = material.read_string("name");
                    uint32_t name_hash = hash_string_murmurhash(name);

                    for (size_t k = 0; k < obj->num_material; k++) {
                        obj_material& mat = obj->material_array[k].material;

                        if (name_hash != hash_string_murmurhash(mat.name))
                            continue;

                        msgpack* shader_compo = material.read("shader_compo");
                        if (shader_compo) {
                            mat.shader_compo.m.color = shader_compo->read_bool("color") ? 1 : 0;
                            mat.shader_compo.m.color_a = shader_compo->read_bool("color_a") ? 1 : 0;
                            mat.shader_compo.m.color_l1 = shader_compo->read_bool("color_l1") ? 1 : 0;
                            mat.shader_compo.m.color_l1_a = shader_compo->read_bool("color_l1_a") ? 1 : 0;
                            mat.shader_compo.m.color_l2 = shader_compo->read_bool("color_l2") ? 1 : 0;
                            mat.shader_compo.m.color_l2_a = shader_compo->read_bool("color_l2_a") ? 1 : 0;
                            mat.shader_compo.m.transparency = shader_compo->read_bool("transparency") ? 1 : 0;
                            mat.shader_compo.m.specular = shader_compo->read_bool("specular") ? 1 : 0;
                            mat.shader_compo.m.normal_01 = shader_compo->read_bool("normal_01") ? 1 : 0;
                            mat.shader_compo.m.normal_02 = shader_compo->read_bool("normal_02") ? 1 : 0;
                            mat.shader_compo.m.envmap = shader_compo->read_bool("envmap") ? 1 : 0;
                            mat.shader_compo.m.color_l3 = shader_compo->read_bool("color_l3") ? 1 : 0;
                            mat.shader_compo.m.color_l3_a = shader_compo->read_bool("color_l3_a") ? 1 : 0;
                            mat.shader_compo.m.translucency = shader_compo->read_bool("translucency") ? 1 : 0;
                            mat.shader_compo.m.flag_14 = shader_compo->read_bool("flag_14") ? 1 : 0;
                            mat.shader_compo.m.override_ibl = shader_compo->read_bool("override_ibl") ? 1 : 0;
                            mat.shader_compo.m.dummy = shader_compo->read_uint32_t("dummy");
                        }

                        msgpack* _shader_name = material.read("shader_name");
                        if (_shader_name) {
                            std::string shader_name = _shader_name->read_string();
                            size_t name_length = min_def(sizeof(mat.shader.name) - 1, shader_name.size());
                            memcpy_s(mat.shader.name, sizeof(mat.shader.name) - 1, shader_name.c_str(), name_length);
                            mat.shader.name[name_length] = 0;
                        }

                        msgpack* shader_info = material.read("shader_info");
                        if (shader_info) {
                            mat.shader_info.m.vtx_trans_type = (obj_material_vertex_translation_type)
                                shader_info->read_uint32_t("vtx_trans_type");
                            mat.shader_info.m.col_src = (obj_material_color_source_type)shader_info->read_uint32_t("col_src");
                            mat.shader_info.m.is_lgt_diffuse = shader_info->read_bool("is_lgt_diffuse") ? 1 : 0;
                            mat.shader_info.m.is_lgt_specular = shader_info->read_bool("is_lgt_specular") ? 1 : 0;
                            mat.shader_info.m.is_lgt_per_pixel = shader_info->read_bool("is_lgt_per_pixel") ? 1 : 0;
                            mat.shader_info.m.is_lgt_double = shader_info->read_bool("is_lgt_double") ? 1 : 0;
                            mat.shader_info.m.bump_map_type = (obj_material_bump_map_type)
                                shader_info->read_uint32_t("bump_map_type");
                            mat.shader_info.m.fresnel_type = shader_info->read_uint32_t("fresnel_type");
                            mat.shader_info.m.line_light = shader_info->read_uint32_t("line_light");
                            mat.shader_info.m.recieve_shadow = shader_info->read_bool("recieve_shadow") ? 1 : 0;
                            mat.shader_info.m.cast_shadow = shader_info->read_bool("cast_shadow") ? 1 : 0;
                            mat.shader_info.m.specular_quality = (obj_material_specular_quality)
                                shader_info->read_uint32_t("specular_quality");
                            mat.shader_info.m.aniso_direction = (obj_material_aniso_direction)
                                shader_info->read_uint32_t("aniso_direction");
                            mat.shader_info.m.dummy = shader_info->read_uint32_t("dummy");
                        }

                        int32_t num_of_textures = 0;
                        msgpack* texdata = material.read("texdata");
                        if (texdata) {
                            for (obj_material_texture_data& l : mat.texdata) {
                                sprintf_s(buf, sizeof(buf), "%d", (int32_t)(&l - mat.texdata));
                                msgpack* tex = texdata->read(buf);

                                l.tex_index = -1;
                                if (!tex)
                                    continue;

                                msgpack* attrib = tex->read("attrib");
                                if (attrib) {
                                    l.attrib.m.repeat_u = attrib->read_bool("repeat_u") ? 1 : 0;
                                    l.attrib.m.repeat_v = attrib->read_bool("repeat_v") ? 1 : 0;
                                    l.attrib.m.mirror_u = attrib->read_bool("mirror_u") ? 1 : 0;
                                    l.attrib.m.mirror_v = attrib->read_bool("mirror_v") ? 1 : 0;
                                    l.attrib.m.ignore_alpha = attrib->read_bool("ignore_alpha") ? 1 : 0;
                                    l.attrib.m.blend = attrib->read_uint32_t("blend");
                                    l.attrib.m.alpha_blend = attrib->read_uint32_t("alpha_blend");
                                    l.attrib.m.border = attrib->read_bool("border") ? 1 : 0;
                                    l.attrib.m.clamp2edge = attrib->read_bool("clamp2edge") ? 1 : 0;
                                    l.attrib.m.filter = attrib->read_uint32_t("filter");
                                    l.attrib.m.mipmap = attrib->read_uint32_t("mipmap");
                                    l.attrib.m.mipmap_bias = attrib->read_uint32_t("mipmap_bias");
                                    l.attrib.m.flag_29 = attrib->read_bool("flag_29") ? 1 : 0;
                                    l.attrib.m.anisotropic_filter = attrib->read_uint32_t("anisotropic_filter");
                                }

                                msgpack* _tex_name = tex->read("tex_name");
                                if (_tex_name) {
                                    std::string tex_name = _tex_name->read_string();
                                    l.tex_index = hash_string_murmurhash(tex_name);
                                }

                                msgpack* shader_info = tex->read("shader_info");
                                if (shader_info) {
                                    l.shader_info.m.tex_type = (obj_material_texture_type)
                                        shader_info->read_uint32_t("tex_type");
                                    l.shader_info.m.uv_idx = shader_info->read_uint32_t("uv_idx");
                                    l.shader_info.m.texcoord_trans = (obj_material_texture_coordinate_translation_type)
                                        shader_info->read_uint32_t("texcoord_trans");
                                    l.shader_info.m.dummy = shader_info->read_uint32_t("dummy");
                                }

                                msgpack* _ex_shader = material.read("ex_shader");
                                if (_ex_shader) {
                                    std::string shader_name = _ex_shader->read_string();
                                    size_t name_length = min_def(sizeof(l.ex_shader) - 1, shader_name.size());
                                    memcpy_s(l.ex_shader, sizeof(l.ex_shader) - 1, shader_name.c_str(), name_length);
                                    l.ex_shader[name_length] = 0;
                                }

                                msgpack* weight = tex->read("weight");
                                if (weight)
                                    l.weight = weight->read_float_t();

                                msgpack* tex_coord_mat = tex->read_array("tex_coord_mat");
                                if (tex_coord_mat) {
                                    msgpack_array* tex_coord_mat_ptr = tex_coord_mat->data.arr;

                                    {
                                        msgpack& row0 = tex_coord_mat_ptr->data()[0];
                                        msgpack_array* row0_ptr = row0.data.arr;
                                        l.tex_coord_mat.row0.x = row0_ptr->data()[0].read_float_t();
                                        l.tex_coord_mat.row0.y = row0_ptr->data()[1].read_float_t();
                                        l.tex_coord_mat.row0.z = row0_ptr->data()[2].read_float_t();
                                        l.tex_coord_mat.row0.w = row0_ptr->data()[3].read_float_t();
                                    }

                                    {
                                        msgpack& row1 = tex_coord_mat_ptr->data()[1];
                                        msgpack_array* row1_ptr = row1.data.arr;
                                        l.tex_coord_mat.row1.x = row1_ptr->data()[0].read_float_t();
                                        l.tex_coord_mat.row1.y = row1_ptr->data()[1].read_float_t();
                                        l.tex_coord_mat.row1.z = row1_ptr->data()[2].read_float_t();
                                        l.tex_coord_mat.row1.w = row1_ptr->data()[3].read_float_t();
                                    }

                                    {
                                        msgpack& row2 = tex_coord_mat_ptr->data()[2];
                                        msgpack_array* row2_ptr = row2.data.arr;
                                        l.tex_coord_mat.row2.x = row2_ptr->data()[0].read_float_t();
                                        l.tex_coord_mat.row2.y = row2_ptr->data()[1].read_float_t();
                                        l.tex_coord_mat.row2.z = row2_ptr->data()[2].read_float_t();
                                        l.tex_coord_mat.row2.w = row2_ptr->data()[3].read_float_t();
                                    }

                                    {
                                        msgpack& row3 = tex_coord_mat_ptr->data()[3];
                                        msgpack_array* row3_ptr = row3.data.arr;
                                        l.tex_coord_mat.row3.x = row3_ptr->data()[0].read_float_t();
                                        l.tex_coord_mat.row3.y = row3_ptr->data()[1].read_float_t();
                                        l.tex_coord_mat.row3.z = row3_ptr->data()[2].read_float_t();
                                        l.tex_coord_mat.row3.w = row3_ptr->data()[3].read_float_t();
                                    }
                                }

                                msgpack* reserved = tex->read_array("reserved");
                                if (reserved) {
                                    msgpack_array* ptr = reserved->data.arr;
                                    for (int32_t m = 0; m < 8; m++)
                                        l.reserved[m] = ptr->data()[m].read_uint32_t();
                                }
                                num_of_textures++;
                            }
                        }

                        obj->material_array[k].num_of_textures = num_of_textures;

                        msgpack* attrib = material.read("attrib");
                        if (attrib) {
                            mat.attrib.m.alpha_texture = attrib->read_bool("alpha_texture") ? 1 : 0;
                            mat.attrib.m.alpha_material = attrib->read_bool("alpha_material") ? 1 : 0;
                            mat.attrib.m.punch_through = attrib->read_bool("punch_through") ? 1 : 0;
                            mat.attrib.m.double_sided = attrib->read_bool("double_sided") ? 1 : 0;
                            mat.attrib.m.normal_dir_light = attrib->read_bool("normal_dir_light") ? 1 : 0;
                            mat.attrib.m.src_blend_factor = (obj_material_blend_factor)
                                attrib->read_uint32_t("src_blend_factor");
                            mat.attrib.m.dst_blend_factor = (obj_material_blend_factor)
                                attrib->read_uint32_t("dst_blend_factor");
                            mat.attrib.m.blend_operation = attrib->read_uint32_t("blend_operation");
                            mat.attrib.m.zbias = attrib->read_uint32_t("zbias");
                            mat.attrib.m.no_fog = attrib->read_bool("no_fog") ? 1 : 0;
                            mat.attrib.m.translucent_priority = attrib->read_uint32_t("translucent_priority");
                            mat.attrib.m.has_fog_height = attrib->read_bool("has_fog_height") ? 1 : 0;
                            mat.attrib.m.flag_28 = attrib->read_bool("flag_28") ? 1 : 0;
                            mat.attrib.m.fog_height = attrib->read_bool("fog_height") ? 1 : 0;
                            mat.attrib.m.flag_30 = attrib->read_bool("flag_30") ? 1 : 0;
                            mat.attrib.m.flag_31 = attrib->read_bool("flag_31") ? 1 : 0;
                        }

                        msgpack* color = material.read("color");
                        if (color) {
                            msgpack* diffuse = color->read("diffuse");
                            if (diffuse) {
                                mat.color.diffuse.x = diffuse->read_float_t("r");
                                mat.color.diffuse.y = diffuse->read_float_t("g");
                                mat.color.diffuse.z = diffuse->read_float_t("b");
                                mat.color.diffuse.w = diffuse->read_float_t("a");
                            }

                            msgpack* ambient = color->read("ambient");
                            if (ambient) {
                                mat.color.ambient.x = ambient->read_float_t("r");
                                mat.color.ambient.y = ambient->read_float_t("g");
                                mat.color.ambient.z = ambient->read_float_t("b");
                                mat.color.ambient.w = ambient->read_float_t("a");
                            }

                            msgpack* specular = color->read("specular");
                            if (specular) {
                                mat.color.specular.x = specular->read_float_t("r");
                                mat.color.specular.y = specular->read_float_t("g");
                                mat.color.specular.z = specular->read_float_t("b");
                                mat.color.specular.w = specular->read_float_t("a");
                            }

                            msgpack* emission = color->read("emission");
                            if (emission) {
                                mat.color.emission.x = emission->read_float_t("r");
                                mat.color.emission.y = emission->read_float_t("g");
                                mat.color.emission.z = emission->read_float_t("b");
                                mat.color.emission.w = emission->read_float_t("a");
                            }

                            msgpack* shininess = color->read("shininess");
                            if (shininess)
                                mat.color.shininess = shininess->read_float_t();

                            msgpack* intensity = color->read("intensity");
                            if (intensity)
                                mat.color.intensity = intensity->read_float_t();
                        }

                        msgpack* center = material.read("center");
                        if (center) {
                            mat.center.x = center->read_float_t("x");
                            mat.center.y = center->read_float_t("y");
                            mat.center.z = center->read_float_t("z");
                        }

                        msgpack* radius = material.read("radius");
                        if (radius)
                            mat.radius = radius->read_float_t();

                        msgpack* bump_depth = material.read("bump_depth");
                        if (bump_depth)
                            mat.radius = bump_depth->read_float_t();

                        msgpack* reserved = material.read_array("reserved");
                        if (reserved) {
                            msgpack_array* ptr = reserved->data.arr;
                            for (int32_t m = 0; m < 15; m++)
                                mat.reserved[m] = ptr->data()[m].read_uint32_t();
                        }
                        break;
                    }
                }
            }

            msgpack* meshes = object.read_array("mesh");
            if (meshes) {
                msgpack_array* ptr = meshes->data.arr;
                for (msgpack& j : *ptr) {
                    msgpack& _mesh = j;

                    std::string name = _mesh.read_string("name");
                    uint32_t name_hash = hash_string_murmurhash(name);

                    for (size_t k = 0; k < obj->num_mesh; k++) {
                        obj_mesh& mesh = obj->mesh_array[k];

                        if (name_hash != hash_string_murmurhash(mesh.name))
                            continue;

                        msgpack* color_mult = _mesh.read("color_mult");
                        if (color_mult) {
                            vec3 _color_mult = 1.0f;

                            msgpack* rgb = color_mult->read("rgb");
                            if (rgb)
                                _color_mult = rgb->read_float_t();

                            msgpack* r = color_mult->read("r");
                            if (r)
                                _color_mult.x = r->read_float_t();

                            msgpack* g = color_mult->read("g");
                            if (g)
                                _color_mult.y = g->read_float_t();

                            msgpack* b = color_mult->read("b");
                            if (b)
                                _color_mult.z = b->read_float_t();

                            obj_vertex_data* vtx = mesh.vertex_array;
                            uint32_t num_vertex = mesh.num_vertex;
                            for (uint32_t i = num_vertex; i; i--, vtx++)
                                *(vec3*)&vtx->color0 = _color_mult;
                        }

                        msgpack* fix_alpha = _mesh.read("fix_alpha");
                        if (fix_alpha && fix_alpha->read_bool()) {
                            obj_vertex_data* vtx = mesh.vertex_array;
                            uint32_t num_vertex = mesh.num_vertex;
                            for (uint32_t i = num_vertex; i; i--, vtx++)
                                vtx->color0.w = 1.0f;
                        }

                        msgpack* invert_negative_color = _mesh.read("invert_negative_color");
                        if (invert_negative_color && invert_negative_color->read_bool()) {
                            obj_vertex_data* vtx = mesh.vertex_array;
                            uint32_t num_vertex = mesh.num_vertex;
                            for (uint32_t i = num_vertex; i; i--, vtx++)
                                if (vtx->color0.x < 0.0f)
                                    *(vec3*)&vtx->color0.x = -*(vec3*)&vtx->color0.x;
                        }

                        msgpack* replace_normals = _mesh.read("replace_normals");
                        if (replace_normals) {
                            vec3 normal = vec3(0.0f, 1.0f, 0.0f);

                            msgpack* x = replace_normals->read("x");
                            if (x)
                                normal.x = x->read_float_t();

                            msgpack* y = replace_normals->read("y");
                            if (y)
                                normal.y = y->read_float_t();

                            msgpack* z = replace_normals->read("z");
                            if (z)
                                normal.z = z->read_float_t();

                            obj_vertex_data* vtx = mesh.vertex_array;
                            uint32_t num_vertex = mesh.num_vertex;
                            for (uint32_t i = num_vertex; i; i--, vtx++)
                                vtx->normal = normal;
                        }

                        msgpack* sub_meshes = _mesh.read_array("sub_mesh");
                        if (!sub_meshes)
                            continue;

                        msgpack_array* ptr = sub_meshes->data.arr;
                        for (size_t l = 0; l < mesh.num_submesh; l++) {
                            obj_sub_mesh& sub_mesh = mesh.submesh_array[l];
                            msgpack& _sub_mesh = ptr->data()[l];

                            msgpack* attrib = _sub_mesh.read("attrib");
                            if (attrib) {
                                msgpack* recieve_shadow = attrib->read("recieve_shadow");
                                msgpack* cast_shadow = attrib->read("cast_shadow");
                                msgpack* transparent = attrib->read("transparent");
                                if (recieve_shadow)
                                    sub_mesh.attrib.m.recieve_shadow = recieve_shadow->read_bool();
                                if (cast_shadow)
                                    sub_mesh.attrib.m.cast_shadow = cast_shadow->read_bool();
                                if (transparent)
                                    sub_mesh.attrib.m.transparent = transparent->read_bool();
                            }
                        }
                        break;
                    }
                }
            }
            break;
        }
    }
}

void object_material_msgpack_read(const char* path, const char* set_name,
    txp_set* txp_set, texture_database* tex_db, ObjsetInfo* info) {
    if (!tex_db || !path_check_directory_exists(path))
        return;

    char set_name_buf[0x80];
    for (const char* i = set_name; *i && *i != '.'; i++) {
        char c = *i;
        if (c >= 'a' && c <= 'z')
            c -= 0x20;
        set_name_buf[i - set_name] = c;
        set_name_buf[i - set_name + 1] = 0;
    }

    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "%s\\%s\\", path, set_name_buf);
    if (!path_check_directory_exists(buf))
        return;

    sprintf_s(buf, sizeof(buf), "%s\\%s\\config_tex.json", path, set_name_buf);
    if (!path_check_file_exists(buf))
        return;

    msgpack msg;

    file_stream s;
    s.open(buf, "rb");
    io_json_read(s, &msg);
    s.close();

    if (msg.type != MSGPACK_MAP)
        return;

    obj_set* set = info->obj_set;

    msgpack* add = msg.read_array("Add");
    if (add) {
        std::vector<uint32_t> ids;
        msgpack_array* ptr = add->data.arr;
        for (msgpack& i : *ptr) {
            std::string name = i.read_string();
            if (!name.size())
                continue;

            sprintf_s(buf, sizeof(buf), "%s\\%s\\%s.dds", path, set_name_buf, name.c_str());
            if (!path_check_file_exists(buf))
                continue;

            dds d;
            sprintf_s(buf, sizeof(buf), "%s\\%s\\%s", path, set_name_buf, name.c_str());
            d.read(buf);
            if (!d.width || !d.height || !d.mipmaps_count || d.data.size() < 1)
                continue;

            uint32_t id = hash_string_murmurhash(name);
            ids.push_back(id);

            tex_db->texture.emplace_back();
            texture_info* info = &tex_db->texture.back();
            info->name.assign(name);
            info->name_hash = hash_string_murmurhash(info->name);
            info->id = id;

            txp_set->textures.push_back({});
            txp* tex = &txp_set->textures.back();
            tex->array_size = d.has_cube_map ? 6 : 1;
            tex->has_cube_map = d.has_cube_map;
            tex->mipmaps_count = d.mipmaps_count;

            tex->mipmaps.reserve((tex->has_cube_map ? 6LL : 1LL) * tex->mipmaps_count);
            int32_t index = 0;
            do
                for (uint32_t i = 0; i < tex->mipmaps_count; i++) {
                    txp_mipmap tex_mip;
                    tex_mip.width = max_def(d.width >> i, 1u);
                    tex_mip.height = max_def(d.height >> i, 1u);
                    tex_mip.format = d.format;

                    uint32_t size = tex_mip.get_size();
                    tex_mip.size = size;
                    tex_mip.data.resize(size);
                    memcpy(tex_mip.data.data(), d.data[index], size);
                    tex->mipmaps.push_back(tex_mip);
                    index++;
                }
            while (index / tex->mipmaps_count < tex->array_size);
        }

        tex_db->update();

        size_t tex_id_num = txp_set->textures.size();
        if (set->tex_id_num != tex_id_num) {
            uint32_t* tex_id_data = info->alloc_handler->allocate<uint32_t>(tex_id_num);
            memmove(tex_id_data, set->tex_id_data, sizeof(uint32_t) * set->tex_id_num);
            memmove(&tex_id_data[set->tex_id_num], ids.data(),
                sizeof(uint32_t) * (tex_id_num - set->tex_id_num));
            set->tex_id_data = tex_id_data;
            set->tex_id_num = (uint32_t)tex_id_num;
            info->tex_num = (uint32_t)tex_id_num;
        }
    }

    msgpack* replace = msg.read_array("Replace");
    if (replace) {
        msgpack_array* ptr = replace->data.arr;
        for (msgpack& i : *ptr) {
            std::string name = i.read_string();
            if (!name.size())
                continue;

            sprintf_s(buf, sizeof(buf), "%s\\%s\\%s.dds", path, set_name_buf, name.c_str());
            if (!path_check_file_exists(buf))
                continue;

            dds d;
            sprintf_s(buf, sizeof(buf), "%s\\%s\\%s", path, set_name_buf, name.c_str());
            d.read(buf);
            if (!d.width || !d.height || !d.mipmaps_count || d.data.size() < 1)
                continue;

            uint32_t id = hash_string_murmurhash(name);

            uint32_t* tex_id_data = set->tex_id_data;
            uint32_t tex_id_num = set->tex_id_num;

            txp* tex = 0;
            for (uint32_t i = 0; i < tex_id_num; i++)
                if (id == tex_id_data[i]) {
                    tex = &txp_set->textures[i];
                    break;
                }

            if (!tex)
                continue;

            tex->array_size = d.has_cube_map ? 6 : 1;
            tex->has_cube_map = d.has_cube_map;
            tex->mipmaps_count = d.mipmaps_count;

            tex->mipmaps.clear();
            tex->mipmaps.reserve((tex->has_cube_map ? 6LL : 1LL) * tex->mipmaps_count);
            int32_t index = 0;
            do
                for (uint32_t i = 0; i < tex->mipmaps_count; i++) {
                    txp_mipmap tex_mip;
                    tex_mip.width = max_def(d.width >> i, 1u);
                    tex_mip.height = max_def(d.height >> i, 1u);
                    tex_mip.format = d.format;

                    uint32_t size = tex_mip.get_size();
                    tex_mip.size = size;
                    tex_mip.data.resize(size);
                    memcpy(tex_mip.data.data(), d.data[index], size);
                    tex->mipmaps.push_back(tex_mip);
                    index++;
                }
            while (index / tex->mipmaps_count < tex->array_size);
        }
    }
}

void object_material_msgpack_write(const char* path, const char* set_name, uint32_t set_id,
    obj_set* obj_set, txp_set* txp_set, object_database* obj_db, texture_database* tex_db) {
    if (!path_check_directory_exists(path) && !path_create_directory(path))
        return;

    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "%s\\%s\\", path, set_name);
    if (!path_check_directory_exists(buf) && !path_create_directory(buf))
        return;

    msgpack_array objects(obj_set->obj_num);
    for (uint32_t i = 0; i < obj_set->obj_num; i++) {
        obj* obj = obj_set->obj_data[i];

        msgpack& object = objects.data()[i];
        object = msgpack(msgpack_map());
        object.append("name", obj->name);

        msgpack* materials = object.append("material", msgpack_array());
        if (materials) {
            msgpack_array* ptr = materials->data.arr;
            ptr->resize(obj->num_material);
            for (size_t j = 0; j < obj->num_material; j++) {
                obj_material& mat = obj->material_array[j].material;
                msgpack& material = ptr->data()[j];
                material = msgpack_map();

                msgpack* shader_compo = material.append("shader_compo", msgpack_map());
                if (shader_compo) {
                    shader_compo->append("color", (bool)mat.shader_compo.m.color);
                    shader_compo->append("color_a", (bool)mat.shader_compo.m.color_a);
                    shader_compo->append("color_l1", (bool)mat.shader_compo.m.color_l1);
                    shader_compo->append("color_l1_a", (bool)mat.shader_compo.m.color_l1_a);
                    shader_compo->append("color_l2", (bool)mat.shader_compo.m.color_l2);
                    shader_compo->append("color_l2_a", (bool)mat.shader_compo.m.color_l2_a);
                    shader_compo->append("transparency", (bool)mat.shader_compo.m.transparency);
                    shader_compo->append("specular", (bool)mat.shader_compo.m.specular);
                    shader_compo->append("normal_01", (bool)mat.shader_compo.m.normal_01);
                    shader_compo->append("normal_02", (bool)mat.shader_compo.m.normal_02);
                    shader_compo->append("envmap", (bool)mat.shader_compo.m.envmap);
                    shader_compo->append("color_l3", (bool)mat.shader_compo.m.color_l3);
                    shader_compo->append("color_l3_a", (bool)mat.shader_compo.m.color_l3_a);
                    shader_compo->append("translucency", (bool)mat.shader_compo.m.translucency);
                    shader_compo->append("flag_14", (bool)mat.shader_compo.m.flag_14);
                    shader_compo->append("override_ibl", (bool)mat.shader_compo.m.override_ibl);
                    shader_compo->append("dummy", mat.shader_compo.m.dummy);
                }

                material.append("shader_name", mat.shader.name);

                msgpack* shader_info = material.append("shader_info", msgpack_map());
                if (shader_info) {
                    shader_info->append("vtx_trans_type", mat.shader_info.m.vtx_trans_type);
                    shader_info->append("col_src", mat.shader_info.m.col_src);
                    shader_info->append("is_lgt_diffuse", (bool)mat.shader_info.m.is_lgt_diffuse);
                    shader_info->append("is_lgt_specular", (bool)mat.shader_info.m.is_lgt_specular);
                    shader_info->append("is_lgt_per_pixel", (bool)mat.shader_info.m.is_lgt_per_pixel);
                    shader_info->append("is_lgt_double", (bool)mat.shader_info.m.is_lgt_double);
                    shader_info->append("bump_map_type", mat.shader_info.m.bump_map_type);
                    shader_info->append("fresnel_type", mat.shader_info.m.fresnel_type);
                    shader_info->append("line_light", mat.shader_info.m.line_light);
                    shader_info->append("recieve_shadow", (bool)mat.shader_info.m.recieve_shadow);
                    shader_info->append("cast_shadow", (bool)mat.shader_info.m.cast_shadow);
                    shader_info->append("specular_quality", mat.shader_info.m.specular_quality);
                    shader_info->append("aniso_direction", mat.shader_info.m.aniso_direction);
                    shader_info->append("dummy", mat.shader_info.m.dummy);
                }

                msgpack* texdata = material.append("texdata", msgpack_map());
                if (texdata) {
                    for (obj_material_texture_data& k : mat.texdata) {
                        if (!k.tex_index || k.tex_index == -1
                            || k.tex_index == hash_murmurhash_empty || k.tex_index == hash_murmurhash_null)
                            continue;

                        sprintf_s(buf, sizeof(buf), "%d", (int32_t)(&k - mat.texdata));
                        msgpack& tex = *texdata->append(buf, msgpack_map());

                        msgpack* attrib = tex.append("attrib", msgpack_map());
                        if (attrib) {
                            attrib->append("repeat_u", (bool)k.attrib.m.repeat_u);
                            attrib->append("repeat_v", (bool)k.attrib.m.repeat_v);
                            attrib->append("mirror_u", (bool)k.attrib.m.mirror_u);
                            attrib->append("mirror_v", (bool)k.attrib.m.mirror_v);
                            attrib->append("ignore_alpha", (bool)k.attrib.m.ignore_alpha);
                            attrib->append("blend", k.attrib.m.blend);
                            attrib->append("alpha_blend", k.attrib.m.alpha_blend);
                            attrib->append("border", (bool)k.attrib.m.border);
                            attrib->append("clamp2edge", (bool)k.attrib.m.clamp2edge);
                            attrib->append("filter", k.attrib.m.filter);
                            attrib->append("mipmap", k.attrib.m.mipmap);
                            attrib->append("mipmap_bias", k.attrib.m.mipmap_bias);
                            attrib->append("flag_29", (bool)k.attrib.m.flag_29);
                            attrib->append("anisotropic_filter", k.attrib.m.anisotropic_filter);
                        }

                        tex.append("tex_name", tex_db->get_texture_name(k.tex_index));

                        msgpack* shader_info = tex.append("shader_info", msgpack_map());
                        if (shader_info) {
                            shader_info->append("tex_type", k.shader_info.m.tex_type);
                            shader_info->append("uv_idx", k.shader_info.m.uv_idx);
                            shader_info->append("texcoord_trans", k.shader_info.m.texcoord_trans);
                            shader_info->append("dummy", k.shader_info.m.dummy);
                        }

                        tex.append("ex_shader", k.ex_shader);
                        tex.append("weight", k.weight);

                        msgpack* tex_coord_mat = tex.append("tex_coord_mat", msgpack_array());
                        if (tex_coord_mat) {
                            msgpack_array* tex_coord_mat_ptr = tex_coord_mat->data.arr;
                            tex_coord_mat_ptr->resize(4);

                            {
                                msgpack& row0 = tex_coord_mat_ptr->data()[0];
                                row0 = msgpack_array();
                                msgpack_array* row0_ptr = row0.data.arr;
                                row0_ptr->resize(4);
                                row0_ptr->data()[0] = k.tex_coord_mat.row0.x;
                                row0_ptr->data()[1] = k.tex_coord_mat.row0.y;
                                row0_ptr->data()[2] = k.tex_coord_mat.row0.z;
                                row0_ptr->data()[3] = k.tex_coord_mat.row0.w;
                            }

                            {
                                msgpack& row1 = tex_coord_mat_ptr->data()[1];
                                row1 = msgpack_array();
                                msgpack_array* row1_ptr = row1.data.arr;
                                row1_ptr->resize(4);
                                row1_ptr->data()[0] = k.tex_coord_mat.row1.x;
                                row1_ptr->data()[1] = k.tex_coord_mat.row1.y;
                                row1_ptr->data()[2] = k.tex_coord_mat.row1.z;
                                row1_ptr->data()[3] = k.tex_coord_mat.row1.w;
                            }

                            {
                                msgpack& row2 = tex_coord_mat_ptr->data()[2];
                                row2 = msgpack_array();
                                msgpack_array* row2_ptr = row2.data.arr;
                                row2_ptr->resize(4);
                                row2_ptr->data()[0] = k.tex_coord_mat.row2.x;
                                row2_ptr->data()[1] = k.tex_coord_mat.row2.y;
                                row2_ptr->data()[2] = k.tex_coord_mat.row2.z;
                                row2_ptr->data()[3] = k.tex_coord_mat.row2.w;
                            }

                            {
                                msgpack& row3 = tex_coord_mat_ptr->data()[3];
                                row3 = msgpack_array();
                                msgpack_array* row3_ptr = row3.data.arr;
                                row3_ptr->resize(4);
                                row3_ptr->data()[0] = k.tex_coord_mat.row3.x;
                                row3_ptr->data()[1] = k.tex_coord_mat.row3.y;
                                row3_ptr->data()[2] = k.tex_coord_mat.row3.z;
                                row3_ptr->data()[3] = k.tex_coord_mat.row3.w;
                            }
                        }

                        msgpack* reserved = tex.append("reserved", msgpack_array());
                        if (reserved) {
                            msgpack_array* ptr = reserved->data.arr;
                            ptr->resize(8);
                            for (int32_t l = 0; l < 8; l++)
                                ptr->data()[l] = k.reserved[l];
                        }
                    }
                }

                msgpack* attrib = material.append("attrib", msgpack_map());
                if (attrib) {
                    attrib->append("alpha_texture", (bool)mat.attrib.m.alpha_texture);
                    attrib->append("alpha_material", (bool)mat.attrib.m.alpha_material);
                    attrib->append("punch_through", (bool)mat.attrib.m.punch_through);
                    attrib->append("double_sided", (bool)mat.attrib.m.double_sided);
                    attrib->append("normal_dir_light", (bool)mat.attrib.m.normal_dir_light);
                    attrib->append("src_blend_factor", mat.attrib.m.src_blend_factor);
                    attrib->append("dst_blend_factor", mat.attrib.m.dst_blend_factor);
                    attrib->append("blend_operation", mat.attrib.m.blend_operation);
                    attrib->append("zbias", mat.attrib.m.zbias);
                    attrib->append("no_fog", (bool)mat.attrib.m.no_fog);
                    attrib->append("translucent_priority", mat.attrib.m.translucent_priority);
                    attrib->append("has_fog_height", (bool)mat.attrib.m.has_fog_height);
                    attrib->append("flag_28", (bool)mat.attrib.m.flag_28);
                    attrib->append("fog_height", (bool)mat.attrib.m.fog_height);
                    attrib->append("flag_30", (bool)mat.attrib.m.flag_30);
                    attrib->append("flag_31", (bool)mat.attrib.m.flag_31);
                }

                msgpack* color = material.append("color", msgpack_map());
                if (color) {
                    msgpack* diffuse = color->append("diffuse", msgpack_map());
                    if (diffuse) {
                        diffuse->append("r", mat.color.diffuse.x);
                        diffuse->append("g", mat.color.diffuse.y);
                        diffuse->append("b", mat.color.diffuse.z);
                        diffuse->append("a", mat.color.diffuse.w);
                    }

                    msgpack* ambient = color->append("ambient", msgpack_map());
                    if (ambient) {
                        ambient->append("r", mat.color.ambient.x);
                        ambient->append("g", mat.color.ambient.y);
                        ambient->append("b", mat.color.ambient.z);
                        ambient->append("a", mat.color.ambient.w);
                    }

                    msgpack* specular = color->append("specular", msgpack_map());
                    if (specular) {
                        specular->append("r", mat.color.specular.x);
                        specular->append("g", mat.color.specular.y);
                        specular->append("b", mat.color.specular.z);
                        specular->append("a", mat.color.specular.w);
                    }

                    msgpack* emission = color->append("emission", msgpack_map());
                    if (emission) {
                        emission->append("r", mat.color.emission.x);
                        emission->append("g", mat.color.emission.y);
                        emission->append("b", mat.color.emission.z);
                        emission->append("a", mat.color.emission.w);
                    }

                    color->append("shininess", mat.color.shininess);
                    color->append("intensity", mat.color.intensity);

                }

                msgpack* center = material.append("center", msgpack_map());
                if (center) {
                    center->append("x", mat.center.x);
                    center->append("y", mat.center.y);
                    center->append("z", mat.center.z);
                }

                material.append("radius", mat.radius);
                material.append("name", mat.name);
                material.append("bump_depth", mat.bump_depth);

                msgpack* reserved = material.append("reserved", msgpack_array());
                if (reserved) {
                    msgpack_array* ptr = reserved->data.arr;
                    ptr->resize(15);
                    for (int32_t k = 0; k < 15; k++)
                        ptr->data()[k] = mat.reserved[k];
                }
            }
        }

        msgpack* meshes = object.append("mesh", msgpack_array());
        if (meshes) {
            msgpack_array* ptr = meshes->data.arr;
            ptr->resize(obj->num_mesh);
            for (size_t j = 0; j < obj->num_mesh; j++) {
                obj_mesh& mesh = obj->mesh_array[j];
                msgpack& _mesh = ptr->data()[j];
                _mesh = msgpack_map();

                _mesh.append("name", mesh.name);

                msgpack* sub_meshes = _mesh.append("sub_mesh", msgpack_array());
                if (sub_meshes) {
                    msgpack_array* ptr = sub_meshes->data.arr;
                    ptr->resize(mesh.num_submesh);
                    for (size_t k = 0; k < mesh.num_submesh; k++) {
                        obj_sub_mesh& sub_mesh = mesh.submesh_array[k];
                        msgpack& _sub_mesh = ptr->data()[k];
                        _sub_mesh = msgpack_map();

                        {
                            msgpack attrib = msgpack_map();
                            attrib.append("recieve_shadow", (bool)sub_mesh.attrib.m.recieve_shadow);
                            attrib.append("cast_shadow", (bool)sub_mesh.attrib.m.cast_shadow);
                            _sub_mesh.append("attrib", attrib);
                        }
                    }
                }
            }
        }
    }

    msgpack msg = objects;

    sprintf_s(buf, sizeof(buf), "%s\\%s\\config.json", path, set_name);

    file_stream s;
    s.open(buf, "wb");
    io_json_write(s, &msg);
    s.close();

    for (uint32_t i = 0; i < obj_set->tex_id_num; i++) {
        const char* texture_name = tex_db->get_texture_name(obj_set->tex_id_data[i]);

        txp& tex = txp_set->textures[i];

        txp_format format = tex.mipmaps[0].format;
        uint32_t width = tex.mipmaps[0].width;
        uint32_t height = tex.mipmaps[0].height;

        dds d;
        d.format = format;
        d.width = width;
        d.height = height;
        d.mipmaps_count = tex.mipmaps_count;
        d.has_cube_map = tex.has_cube_map;
        d.data.reserve((tex.has_cube_map ? 6LL : 1LL) * tex.mipmaps_count);
        uint32_t index = 0;
        do
            for (uint32_t j = 0; j < tex.mipmaps_count; j++) {
                uint32_t size = txp::get_size(format, max_def(width >> j, 1u), max_def(height >> j, 1u));
                void* data = force_malloc(size);
                memcpy(data, tex.mipmaps[index].data.data(), size);
                d.data.push_back(data);
                index++;
            }
        while (index / tex.mipmaps_count < tex.array_size);
        sprintf_s(buf, sizeof(buf), "%s\\%s\\%s", path, set_name, texture_name);
        d.write(buf);
    }
}

inline void object_storage_init(const object_database* obj_db) {
    for (const object_set_info& i : obj_db->object_set) {
        ObjsetInfo info;
        info.set_id = i.id;
        info.name.assign(i.name);
        object_storage_data.insert({ i.id, info });
    }
    object_storage_data_modern.clear();
}

inline obj* object_storage_get_obj(object_info obj_info) {
    auto elem = object_storage_data.find(obj_info.set_id);
    if (elem != object_storage_data.end()) {
        obj_set* set = elem->second.obj_set;
        if (!set)
            return 0;

        auto elem_obj = elem->second.obj_id_data.find(obj_info.id);
        if (elem_obj != elem->second.obj_id_data.end())
            return set->obj_data[elem_obj->second];
        return 0;
    }

    auto elem_modern = object_storage_data_modern.find(obj_info.set_id);
    if (elem_modern != object_storage_data_modern.end()) {
        obj_set* set = elem_modern->second.obj_set;
        if (!set)
            return 0;

        auto elem_obj = elem_modern->second.obj_id_data.find(obj_info.id);
        if (elem_obj != elem_modern->second.obj_id_data.end())
            return set->obj_data[elem_obj->second];
        return 0;
    }
    return 0;
}

inline obj* object_storage_get_obj_by_index(uint32_t set_id, uint32_t index) {
    auto elem = object_storage_data.find(set_id);
    if (elem != object_storage_data.end()) {
        obj_set* set = elem->second.obj_set;
        if (!set)
            return 0;

        if (index < elem->second.obj_set->obj_num)
            return elem->second.obj_set->obj_data[index];
        return 0;
    }

    auto elem_modern = object_storage_data_modern.find(set_id);
    if (elem_modern != object_storage_data_modern.end()) {
        obj_set* set = elem_modern->second.obj_set;
        if (!set)
            return 0;

        if (index < elem_modern->second.obj_set->obj_num)
            return elem_modern->second.obj_set->obj_data[index];
        return 0;
    }
    return 0;
}

inline ObjsetInfo* object_storage_get_objset_info(uint32_t set_id) {
    auto elem = object_storage_data.find(set_id);
    if (elem != object_storage_data.end())
        return &elem->second;

    auto elem_modern = object_storage_data_modern.find(set_id);
    if (elem_modern != object_storage_data_modern.end())
        return &elem_modern->second;
    return 0;
}

inline obj_mesh* object_storage_get_obj_mesh(object_info obj_info, const char* mesh_name) {
    auto elem = object_storage_data.find(obj_info.set_id);
    if (elem != object_storage_data.end()) {
        obj_set* set = elem->second.obj_set;
        if (!set)
            return 0;

        auto elem_obj = elem->second.obj_id_data.find(obj_info.id);
        if (elem_obj != elem->second.obj_id_data.end())
            return set->obj_data[elem_obj->second]->get_obj_mesh(mesh_name);
        return 0;
    }

    auto elem_modern = object_storage_data_modern.find(obj_info.set_id);
    if (elem_modern != object_storage_data_modern.end()) {
        obj_set* set = elem_modern->second.obj_set;
        if (!set)
            return 0;

        auto elem_obj = elem_modern->second.obj_id_data.find(obj_info.id);
        if (elem_obj != elem_modern->second.obj_id_data.end())
            return set->obj_data[elem_obj->second]->get_obj_mesh(mesh_name);
        return 0;
    }
    return 0;
}

inline obj_mesh* object_storage_get_obj_mesh_by_index(object_info obj_info, uint32_t index) {
    auto elem = object_storage_data.find(obj_info.set_id);
    if (elem != object_storage_data.end()) {
        obj_set* set = elem->second.obj_set;
        if (!set)
            return 0;

        auto elem_obj = elem->second.obj_id_data.find(obj_info.id);
        if (elem_obj != elem->second.obj_id_data.end()) {
            obj* obj = set->obj_data[elem_obj->second];
            if (index >= 0 && index < obj->num_mesh)
                return &obj->mesh_array[index];
        }
        return 0;
    }

    auto elem_modern = object_storage_data_modern.find(obj_info.set_id);
    if (elem_modern != object_storage_data_modern.end()) {
        obj_set* set = elem_modern->second.obj_set;
        if (!set)
            return 0;

        auto elem_obj = elem_modern->second.obj_id_data.find(obj_info.id);
        if (elem_obj != elem_modern->second.obj_id_data.end()) {
            obj* obj = set->obj_data[elem_obj->second];
            if (index >= 0 && index < obj->num_mesh)
                return &obj->mesh_array[index];
        }
        return 0;
    }
    return 0;
}

inline obj_mesh* object_storage_get_obj_mesh_by_object_hash(uint32_t hash, const char* mesh_name) {
    for (auto& i : object_storage_data) {
        obj_set* set = i.second.obj_set;
        if (!set)
            continue;

        for (uint32_t j = 0; j < set->obj_num; j++)
            if (set->obj_data[j]->hash == hash)
                return set->obj_data[j]->get_obj_mesh(mesh_name);
    }

    for (auto& i : object_storage_data_modern) {
        obj_set* set = i.second.obj_set;
        if (!set)
            continue;

        for (uint32_t j = 0; j < set->obj_num; j++)
            if (set->obj_data[j]->hash == hash)
                return set->obj_data[j]->get_obj_mesh(mesh_name);
    }
    return 0;
}

inline obj_mesh* object_storage_get_obj_mesh_by_object_hash_index(uint32_t hash, uint32_t index) {
    for (auto& i : object_storage_data) {
        obj_set* set = i.second.obj_set;
        if (!set)
            continue;

        for (uint32_t j = 0; j < set->obj_num; j++) {
            if (set->obj_data[j]->hash != hash)
                continue;

            obj* obj = set->obj_data[j];
            if (index >= 0 && index < obj->num_mesh)
                return &obj->mesh_array[index];
            return 0;
        }
    }

    for (auto& i : object_storage_data_modern) {
        obj_set* set = i.second.obj_set;
        if (!set)
            continue;

        for (uint32_t j = 0; j < set->obj_num; j++) {
            if (set->obj_data[j]->hash != hash)
                continue;

            obj* obj = set->obj_data[j];
            if (index >= 0 && index < obj->num_mesh)
                return &obj->mesh_array[index];
            return 0;
        }
    }
    return 0;
}

inline uint32_t object_storage_get_obj_mesh_index(object_info obj_info, const char* mesh_name) {
    auto elem = object_storage_data.find(obj_info.set_id);
    if (elem != object_storage_data.end()) {
        obj_set* set = elem->second.obj_set;
        if (!set)
            return -1;

        auto elem_obj = elem->second.obj_id_data.find(obj_info.id);
        if (elem_obj != elem->second.obj_id_data.end())
            return set->obj_data[elem_obj->second]->get_obj_mesh_index(mesh_name);
        return 0;
    }

    auto elem_modern = object_storage_data_modern.find(obj_info.set_id);
    if (elem_modern != object_storage_data_modern.end()) {
        obj_set* set = elem_modern->second.obj_set;
        if (!set)
            return -1;

        auto elem_obj = elem_modern->second.obj_id_data.find(obj_info.id);
        if (elem_obj != elem_modern->second.obj_id_data.end())
            return set->obj_data[elem_obj->second]->get_obj_mesh_index(mesh_name);
        return 0;
    }
    return -1;
}

inline uint32_t object_storage_get_obj_mesh_index_by_hash(uint32_t hash, const char* mesh_name) {
    for (auto& i : object_storage_data) {
        obj_set* set = i.second.obj_set;
        if (!set)
            continue;

        for (uint32_t j = 0; j < set->obj_num; j++)
            if (set->obj_data[j]->hash == hash)
                return set->obj_data[j]->get_obj_mesh_index(mesh_name);
    }

    for (auto& i : object_storage_data_modern) {
        obj_set* set = i.second.obj_set;
        if (!set)
            continue;

        for (uint32_t j = 0; j < set->obj_num; j++)
            if (set->obj_data[j]->hash == hash)
                return set->obj_data[j]->get_obj_mesh_index(mesh_name);
    }
    return -1;
}

inline const char* object_storage_get_obj_name(object_info obj_info) {
    obj* obj = object_storage_get_obj(obj_info);
    if (obj)
        return obj->name;
    return 0;
}

inline obj_set* object_storage_get_obj_set(uint32_t set_id) {
    auto elem = object_storage_data.find(set_id);
    if (elem != object_storage_data.end())
        return elem->second.obj_set;

    auto elem_modern = object_storage_data_modern.find(set_id);
    if (elem_modern != object_storage_data_modern.end())
        return elem_modern->second.obj_set;
    return 0;
}

inline size_t object_storage_get_obj_set_count() {
    return object_storage_data.size() + object_storage_data_modern.size();
}

inline int32_t object_storage_get_obj_storage_load_count(uint32_t set_id) {
    auto elem = object_storage_data.find(set_id);
    if (elem != object_storage_data.end())
        return elem->second.load_count;

    auto elem_modern = object_storage_data_modern.find(set_id);
    if (elem_modern != object_storage_data_modern.end())
        return elem_modern->second.load_count;
    return 0;
}

inline obj_skin* object_storage_get_obj_skin(object_info obj_info) {
    auto elem = object_storage_data.find(obj_info.set_id);
    if (elem != object_storage_data.end()) {
        obj_set* set = elem->second.obj_set;
        if (!set)
            return 0;

        auto elem_obj = elem->second.obj_id_data.find(obj_info.id);
        if (elem_obj != elem->second.obj_id_data.end())
            return set->obj_data[elem_obj->second]->skin;
        return 0;
    }

    auto elem_modern = object_storage_data_modern.find(obj_info.set_id);
    if (elem_modern != object_storage_data_modern.end()) {
        obj_set* set = elem_modern->second.obj_set;
        if (!set)
            return 0;

        auto elem_obj = elem_modern->second.obj_id_data.find(obj_info.id);
        if (elem_obj != elem_modern->second.obj_id_data.end())
            return set->obj_data[elem_obj->second]->skin;
        return 0;
    }
    return 0;
}

inline obj_index_buffer* object_storage_get_obj_index_buffers(uint32_t set_id) {
    ObjsetInfo* info = object_storage_get_objset_info(set_id);
    if (info && info->objib)
        return info->objib;
    return 0;
}

inline obj_mesh_index_buffer* object_storage_get_obj_mesh_index_buffer(object_info obj_info) {
    ObjsetInfo* info = object_storage_get_objset_info(obj_info.set_id);
    if (info && info->obj_set && info->objib) {
        auto elem = info->obj_id_data.find(obj_info.id);
        if (elem != info->obj_id_data.end())
            return info->objib[elem->second].mesh_data;
    }
    return 0;
}

inline obj_vertex_buffer* object_storage_get_obj_vertex_buffers(uint32_t set_id) {
    ObjsetInfo* info = object_storage_get_objset_info(set_id);
    if (info && info->objvb)
        return info->objvb;
    return 0;
}

inline obj_mesh_vertex_buffer* object_storage_get_obj_mesh_vertex_buffer(object_info obj_info) {
    ObjsetInfo* info = object_storage_get_objset_info(obj_info.set_id);
    if (info && info->obj_set && info->objvb) {
        auto elem = info->obj_id_data.find(obj_info.id);
        if (elem != info->obj_id_data.end())
            return info->objvb[elem->second].mesh_data;
    }
    return 0;
}

GLuint object_storage_get_obj_set_texture(uint32_t set, uint32_t tex_id) {
    std::vector<GLuint>* textures = object_storage_get_obj_set_textures(set);
    if (!textures)
        return 0;

    ObjsetInfo* info = object_storage_get_objset_info(set);
    if (!info)
        return 0;

    auto elem = info->tex_id_data.find(tex_id);
    if (elem != info->tex_id_data.end())
        return (*textures)[elem->second];
    return 0;
}

inline std::vector<GLuint>* object_storage_get_obj_set_textures(int32_t set) {
    ObjsetInfo* info = object_storage_get_objset_info(set);
    if (info)
        return &info->gentex;
    return 0;
}

int32_t object_storage_load_set(void* data, const object_database* obj_db, const char* name) {
    const object_set_info* set_info = obj_db->get_object_set_info(name);
    if (!set_info)
        return 1;

    ObjsetInfo* info = object_storage_get_objset_info(set_info->id);
    if (!info)
        return 1;

    if (info->load_count > 0) {
        info->load_count++;
        return 1;
    }

    const std::string& archive_file_name = set_info->archive_file_name;
    const std::string& object_file_name = set_info->object_file_name;
    const std::string& texture_file_name = set_info->texture_file_name;
    if (!object_file_name.size() || !texture_file_name.size())
        return 1;

    if (archive_file_name.size()) {
        info->obj_file_handler.read_file(data, "rom/objset/",
            archive_file_name.c_str(), object_file_name.c_str(), false);
        info->tex_file_handler.read_file(data, "rom/objset/",
            archive_file_name.c_str(), texture_file_name.c_str(), false);
    }
    else {
        info->obj_file_handler.read_file(data, "rom/objset/", object_file_name.c_str());
        info->tex_file_handler.read_file(data, "rom/objset/", texture_file_name.c_str());
    }

    info->load_count = 1;
    info->obj_loaded = false;
    info->tex_loaded = false;
    return 0;
}

int32_t object_storage_load_set(void* data, const object_database* obj_db, uint32_t set_id) {
    const object_set_info* set_info = obj_db->get_object_set_info(set_id);
    if (!set_info)
        return 1;

    ObjsetInfo* info = object_storage_get_objset_info(set_id);
    if (!info)
        return 1;

    if (info->load_count > 0) {
        info->load_count++;
        return 1;
    }

    info->modern = false;

    const std::string& archive_file_name = set_info->archive_file_name;
    const std::string& object_file_name = set_info->object_file_name;
    const std::string& texture_file_name = set_info->texture_file_name;
    if (!object_file_name.size() || !texture_file_name.size())
        return 1;

    if (archive_file_name.size()) {
        info->obj_file_handler.read_file(data, "rom/objset/",
            archive_file_name.c_str(), object_file_name.c_str(), false);
        info->tex_file_handler.read_file(data, "rom/objset/",
            archive_file_name.c_str(), texture_file_name.c_str(), false);
    }
    else {
        info->obj_file_handler.read_file(data, "rom/objset/", object_file_name.c_str());
        info->tex_file_handler.read_file(data, "rom/objset/", texture_file_name.c_str());
    }

    info->load_count = 1;
    info->obj_loaded = false;
    info->tex_loaded = false;
    return 0;
}

int32_t object_storage_load_set_hash(void* data, uint32_t hash) {
    if (!hash || hash == hash_murmurhash_empty)
        return 1;

    std::string file;
    if (!((data_struct*)data)->get_file("root+/objset/", hash, ".farc", file))
        return 1;

    ObjsetInfo* info = object_storage_get_objset_info(hash);
    if (!info) {
        info = &object_storage_data_modern.insert({ hash, {} }).first->second;
        info->set_id = hash;
    }

    if (info->load_count > 0) {
        info->load_count++;
        return 1;
    }

    info->modern = true;

    info->farc_file_handler.read_file(data, "root+/objset/", file.c_str());

    info->load_count = 1;
    info->obj_loaded = false;
    info->tex_loaded = false;
    return 0;
}

bool object_storage_load_obj_set_check_not_read(uint32_t set_id,
    object_database* obj_db, texture_database* tex_db) {
    ObjsetInfo* info = object_storage_get_objset_info(set_id);
    if (!info)
        return true;

    if (!info->modern) {
        if (!info->obj_loaded && !info->obj_file_handler.check_not_ready()) {
            const void* data = info->obj_file_handler.get_data();
            size_t size = info->obj_file_handler.get_size();
            if (!data || !size)
                return false;

            prj::shared_ptr<prj::stack_allocator>& alloc = info->alloc_handler;
            alloc = prj::shared_ptr<prj::stack_allocator>(new prj::stack_allocator);

            obj_set* set = alloc->allocate<obj_set>();
            info->obj_set = set;
            set->unpack_file(alloc, data, size, false);
            if (!set->ready)
                return false;

            info->obj_file_handler.reset();
            info->obj_id_data.reserve(set->obj_num);
            for (uint32_t i = 0; i < set->obj_num; i++)
                info->obj_id_data.push_back(set->obj_data[i]->id, i);
            info->obj_id_data.sort();

            if (!ObjsetInfo_vertex_buffer_load(info)
                || !ObjsetInfo_index_buffer_load(info))
                return false;

            ObjsetInfo_calc_axis_aligned_bounding_box(info);
            info->obj_loaded = true;
        }

        if (!info->obj_loaded)
            return true;

        if (!info->tex_loaded && !info->tex_file_handler.check_not_ready()) {
            if (!info->tex_file_handler.get_data())
                return false;
            else if (ObjsetInfo_load_textures(info,
                info->tex_file_handler.get_data(), false))
                return false;

            ObjsetInfo_get_shader_index_texture_index(info);
            info->tex_file_handler.reset();
            info->tex_loaded = true;
        }
    }
    else if (!info->obj_loaded && !info->farc_file_handler.check_not_ready()) {
        const void* data = info->farc_file_handler.get_data();
        size_t size = info->farc_file_handler.get_size();
        if (!data || !size)
            return false;

        farc f;
        f.read(data, size, true);

        std::string& file = info->farc_file_handler.ptr->file;

        size_t file_len = file.size();
        if (file_len >= 0x100 - 4)
            return false;

        const char* t = strrchr(file.c_str(), '.');
        if (t)
            file_len = t - file.c_str();

        char buf[0x100];
        memcpy_s(buf, sizeof(buf), file.c_str(), file_len);
        char* ext = buf + file_len;
        size_t ext_len = sizeof(buf) - file_len;

        memcpy_s(ext, ext_len, ".osd", 5);
        farc_file* osd = f.read_file(buf);
        if (!osd)
            return false;

        memcpy_s(ext, ext_len, ".txd", 5);
        farc_file* txd = f.read_file(buf);
        if (!txd)
            return false;

        memcpy_s(ext, ext_len, ".osi", 5);
        farc_file* osi = f.read_file(buf);
        if (!osi)
            return false;

        memcpy_s(ext, ext_len, ".txi", 5);
        farc_file* txi = f.read_file(buf);
        if (!txi)
            return false;

        object_database_file obj_db_file;
        obj_db_file.read(osi->data, osi->size, true);

        texture_database_file tex_db_file;
        tex_db_file.read(txi->data, txi->size, true);

        object_set_info_file* set_info_file = 0;
        if (obj_db_file.ready)
            for (object_set_info_file& m : obj_db_file.object_set)
                if (m.id == info->set_id) {
                    set_info_file = &m;
                    break;
                }

        if (!set_info_file)
            return false;

        if (obj_db_file.ready) {
            if (obj_db)
                obj_db->add(&obj_db_file);
        }

        if (tex_db_file.ready) {
            if (tex_db)
                tex_db->add(&tex_db_file);
        }

        info->name.assign(set_info_file->name);

        prj::shared_ptr<prj::stack_allocator>& alloc = info->alloc_handler;
        alloc = prj::shared_ptr<prj::stack_allocator>(new prj::stack_allocator);

        obj_set* set = alloc->allocate<obj_set>();
        info->obj_set = set;
        set->unpack_file(alloc, osd->data, osd->size, true);
        if (!set->ready)
            return false;

        object_material_msgpack_read("patch\\AFT\\objset", file.c_str(), set, obj_db);

        info->obj_file_handler.reset();
        info->obj_id_data.reserve(set->obj_num);
        for (uint32_t i = 0; i < set->obj_num; i++)
            info->obj_id_data.push_back(set->obj_data[i]->id, i);
        info->obj_id_data.sort();

        if (!ObjsetInfo_vertex_buffer_load(info)
            || !ObjsetInfo_index_buffer_load(info))
            return false;

        ObjsetInfo_calc_axis_aligned_bounding_box(info);
        info->obj_loaded = true;

        if (ObjsetInfo_load_textures_modern(info, txd->data, txd->size, file.c_str(), tex_db))
            return false;

        ObjsetInfo_get_shader_index_texture_index(info);
        info->tex_loaded = true;
        info->farc_file_handler.reset();
    }

    if (info->obj_loaded && info->tex_loaded)
        return false;
    return true;
}

inline void object_storage_unload_set(const object_database* obj_db, const char* name) {
    const object_set_info* set_info = obj_db->get_object_set_info(name);
    if (!set_info)
        return;

    ObjsetInfo* info = object_storage_get_objset_info(set_info->id);
    if (!info || info->load_count <= 0)
        return;

    if (--info->load_count > 0)
        return;

    info->obj_id_data.clear();
    info->tex_id_data.clear();
    info->gentex.clear();

    texture_array_free(info->tex_data);
    info->tex_data = 0;
    info->tex_num = 0;

    ObjsetInfo_index_buffer_free(info);
    ObjsetInfo_vertex_buffer_free(info);
    info->load_count = 0;
    info->tex_loaded = false;
    info->obj_loaded = false;
    info->alloc_handler.reset();
    info->obj_set = 0;
    info->tex_file_handler.reset();
    info->obj_file_handler.reset();
    info->farc_file_handler.reset();
}

inline void object_storage_unload_set(uint32_t set_id) {
    ObjsetInfo* info = object_storage_get_objset_info(set_id);
    if (!info || info->load_count <= 0)
        return;

    if (--info->load_count > 0)
        return;

    info->obj_id_data.clear();
    info->tex_id_data.clear();
    info->gentex.clear();

    texture_array_free(info->tex_data);
    info->tex_data = 0;
    info->tex_num = 0;

    ObjsetInfo_index_buffer_free(info);
    ObjsetInfo_vertex_buffer_free(info);
    info->load_count = 0;
    info->tex_loaded = false;
    info->obj_loaded = false;
    info->alloc_handler.reset();
    info->obj_set = 0;
    info->tex_file_handler.reset();
    info->obj_file_handler.reset();
    info->farc_file_handler.reset();
    if (info->modern)
        object_storage_data_modern.erase(set_id);
}

inline void object_storage_free() {
    object_storage_data.clear();
    object_storage_data_modern.clear();
}

static GLuint create_index_buffer(size_t size, const void* data) {
    GLuint buffer = 0;
    glGenBuffers(1, &buffer);
    gl_state_bind_element_array_buffer(buffer);
    if (GLAD_GL_VERSION_4_4)
        glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)size, data, 0);
    else
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)size, data, GL_STATIC_DRAW);
    gl_state_bind_element_array_buffer(0);

    if (glGetError()) {
        glDeleteBuffers(1, &buffer);
        return 0;
    }
    return buffer;
}

static GLuint create_vertex_buffer(size_t size, const void* data, bool dynamic) {
    GLuint buffer = 0;
    glGenBuffers(1, &buffer);
    gl_state_bind_array_buffer(buffer);
    if (GLAD_GL_VERSION_4_4)
        glBufferStorage(GL_ARRAY_BUFFER, (GLsizeiptr)size, data,
            dynamic ? GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT : 0);
    else
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)size, data,
            dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
    gl_state_bind_array_buffer(0);

    if (glGetError()) {
        glDeleteBuffers(1, &buffer);
        return 0;
    }
    return buffer;
}

static void free_index_buffer(GLuint buffer) {
    if (!buffer)
        return;

    extern render_context* rctx_ptr;
    rctx_ptr->disp_manager->check_index_buffer(buffer);
    glDeleteBuffers(1, &buffer);
    glGetError();
}

static void free_vertex_buffer(GLuint buffer) {
    if (!buffer)
        return;

    extern render_context* rctx_ptr;
    rctx_ptr->disp_manager->check_vertex_buffer(buffer);
    glDeleteBuffers(1, &buffer);
    glGetError();
}

static uint32_t remove_degenerate_triangle_indices(
    uint32_t* dst_index_array, const uint32_t num_index, uint32_t* src_index_array) {
    if (!num_index)
        return 0;

    dst_index_array[0] = src_index_array[0];

    uint32_t src_index = 1;
    uint32_t dst_index = 1;
    uint32_t strip_length = 1;
    while ((int32_t)src_index < (int32_t)num_index - 4)
        if (src_index_array[src_index] != src_index_array[src_index + 1]) {
            dst_index_array[dst_index++] = src_index_array[src_index];
            strip_length++;
            src_index++;
        }
        else if (src_index_array[src_index + 3] == src_index_array[src_index + 4]) {
            dst_index_array[dst_index++] = src_index_array[src_index];
            dst_index_array[dst_index++] = 0xFFFFFFFF;
            dst_index_array[dst_index++] = src_index_array[src_index + 4];

            if (strip_length % 2) {
                dst_index_array[dst_index++] = src_index_array[src_index + 4];
                strip_length = 0;
            }
            else
                strip_length = 1;
            src_index += 5;
        }
        else if (src_index_array[src_index - 1] != src_index_array[src_index + 2]
            || src_index_array[src_index + 1] != src_index_array[src_index + 4]) {
            dst_index_array[dst_index++] = src_index_array[src_index];
            dst_index_array[dst_index++] = 0xFFFFFFFF;
            dst_index_array[dst_index++] = src_index_array[src_index + 3];

            if (!(strip_length % 2)) {
                dst_index_array[dst_index++] = src_index_array[src_index + 3];
                strip_length = 0;
            }
            else
                strip_length = 1;
            src_index += 4;
        }
        else {
            dst_index_array[dst_index++] = src_index_array[src_index];
            strip_length++;
            src_index += 5;
        }

    if (src_index < (int32_t)num_index) {
        src_index_array += src_index;
        dst_index_array += dst_index;
        dst_index += num_index - src_index;
        while (src_index++ < (int32_t)num_index)
            *dst_index_array++ = *src_index_array++;
    }
    return dst_index;
}

static void ObjsetInfo_calc_axis_aligned_bounding_box(ObjsetInfo* info) {
    obj_set* set = info->obj_set;
    for (uint32_t i = 0; i < set->obj_num; i++) {
        obj* obj = set->obj_data[i];
        for (uint32_t j = 0; j < obj->num_mesh; j++) {
            obj_mesh& mesh = obj->mesh_array[j];
            for (uint32_t k = 0; k < mesh.num_submesh; k++) {
                vec3 _min =    9999999.0f;
                vec3 _max = -100000000.0f;

                obj_sub_mesh& sub_mesh = mesh.submesh_array[k];
                uint32_t* index = sub_mesh.index_array;
                uint32_t num_index = sub_mesh.num_index;
                obj_vertex_data* vertex_array = mesh.vertex_array;
                if (sub_mesh.index_format == OBJ_INDEX_U16)
                    for (uint32_t l = 0; l < num_index; l++, index++) {
                        if (*index == 0xFFFFFFFF)
                            continue;

                        vec3 pos = vertex_array[*index].position;
                        _min = vec3::min(_min, pos);
                        _max = vec3::max(_max, pos);
                    }
                else
                    for (uint32_t l = 0; l < num_index; l++, index++) {
                        vec3 pos = vertex_array[*index].position;
                        _min = vec3::min(_min, pos);
                        _max = vec3::max(_max, pos);
                    }

                vec3 center = (_max + _min) * 0.5f;
                vec3 size = _max - center;
                sub_mesh.axis_aligned_bounding_box.center = center;
                sub_mesh.axis_aligned_bounding_box.size = size;
            }
        }
    }
}

static void ObjsetInfo_get_shader_index_texture_index(ObjsetInfo* info) {
    obj_set* set = info->obj_set;
    for (uint32_t i = 0; i < set->obj_num; i++) {
        obj* obj = set->obj_data[i];
        uint32_t num_material = obj->num_material;
        for (uint32_t j = 0; j < num_material; j++) {
            obj_material_data& material_data = obj->material_array[j];
            obj_material& material = material_data.material;

            if (*(int32_t*)&material.shader.name[4] != 0xDEADFF) {
                material.shader.index = shaders_ft.get_index_by_name(material.shader.name);
                *(int32_t*)&material.shader.name[4] = 0xDEADFF;
            }

            for (obj_material_texture_data& k : material.texdata) {
                if (k.tex_index == -1)
                    continue;

                obj_material_texture_data& texture = k;
                uint32_t tex_index = texture.tex_index;
                texture.tex_index = -1;
                texture.texture_index = 0;

                std::pair<uint32_t, uint32_t>* tex_id_data = info->tex_id_data.data();
                uint32_t tex_id_num = (uint32_t)info->tex_id_data.size();
                for (uint32_t l = tex_id_num; l; l--, tex_id_data++)
                    if (tex_id_data->first == tex_index) {
                        texture.tex_index = tex_index;
                        texture.texture_index = tex_id_data->second;
                        break;
                    }
            }
        }
    }
}

static bool ObjsetInfo_index_buffer_load(ObjsetInfo* info) {
    obj_set* set = info->obj_set;
    info->objib_num = set->obj_num;
    info->objib = new obj_index_buffer[set->obj_num];
    if (!info->objib)
        return true;

    for (uint32_t i = 0; i < set->obj_num; i++)
        if (!info->objib[i].load(set->obj_data[i]))
            return false;
    return true;
}

static void ObjsetInfo_index_buffer_free(ObjsetInfo* info) {
    if (info->objib) {
        for (uint32_t i = 0; i < info->objib_num; i++)
            info->objib[i].unload();
        delete[] info->objib;
    }

    info->objib = 0;
    info->objib_num = 0;
}

static bool ObjsetInfo_load_textures(ObjsetInfo* info, const void* data, bool big_endian) {
    obj_set* set = info->obj_set;
    if (!set || !data)
        return true;
    else if (!set->tex_id_num)
        return false;

    {
        txp_set txp;
        txp.unpack_file(data, big_endian);
        info->tex_num = (int32_t)txp.textures.size();
        texture_txp_set_load(&txp, &info->tex_data, set->tex_id_data);
    }

    info->tex_id_data.reserve(info->tex_num);
    info->gentex.reserve(info->tex_num);
    uint32_t* tex_id_data = set->tex_id_data;
    uint32_t tex_num = info->tex_num;
    texture** tex_data = info->tex_data;
    for (uint32_t i = 0; i < tex_num; i++) {
        info->tex_id_data.push_back(tex_id_data[i], i);
        info->gentex.push_back(tex_data[i]->glid);
    }
    info->tex_id_data.sort();
    return false;
}

static bool ObjsetInfo_load_textures_modern(ObjsetInfo* info,
    const void* data, size_t size, const char* file, texture_database* tex_db) {
    obj_set* set = info->obj_set;
    if (!set || !data || !size)
        return true;
    else if (!set->tex_id_num)
        return false;

    {
        txp_set txp;
        txp.unpack_file_modern(data, size, 'MTXD');
        info->tex_num = (int32_t)txp.textures.size();
        object_material_msgpack_read("patch\\AFT\\objset", file, &txp, tex_db, info);
        texture_txp_set_load(&txp, &info->tex_data, set->tex_id_data);
    }

    info->tex_id_data.reserve(info->tex_num);
    info->gentex.reserve(info->tex_num);
    uint32_t* tex_id_data = set->tex_id_data;
    uint32_t tex_num = info->tex_num;
    texture** tex_data = info->tex_data;
    for (uint32_t i = 0; i < tex_num; i++) {
        info->tex_id_data.push_back(tex_id_data[i], i);
        info->gentex.push_back(tex_data[i]->glid);
    }
    info->tex_id_data.sort();
    return false;
}

static bool ObjsetInfo_vertex_buffer_load(ObjsetInfo* info) {
    obj_set* set = info->obj_set;
    info->objvb_num = set->obj_num;
    info->objvb = new obj_vertex_buffer[set->obj_num];
    if (!info->objvb)
        return true;

    for (uint32_t i = 0; i < set->obj_num; i++)
        if (!info->objvb[i].load(set->obj_data[i]))
            return false;
    return true;
}

static void ObjsetInfo_vertex_buffer_free(ObjsetInfo* info) {
    if (info->objvb) {
        for (uint32_t i = 0; i < info->objvb_num; i++)
            info->objvb[i].unload();
        delete[] info->objvb;
    }

    info->objvb = 0;
    info->objvb_num = 0;
}

inline static uint32_t obj_vertex_format_get_vertex_size(obj_vertex_format format) {
    uint32_t size = 0;
    if (format & OBJ_VERTEX_POSITION)
        size += 12;
    if (format & OBJ_VERTEX_NORMAL)
        size += 12;
    if (format & OBJ_VERTEX_TANGENT)
        size += 16;
    if (format & OBJ_VERTEX_BINORMAL)
        size += 12;
    if (format & OBJ_VERTEX_TEXCOORD0)
        size += 8;
    if (format & OBJ_VERTEX_TEXCOORD1)
        size += 8;
    if (format & OBJ_VERTEX_TEXCOORD2)
        size += 8;
    if (format & OBJ_VERTEX_TEXCOORD3)
        size += 8;
    if (format & OBJ_VERTEX_COLOR0)
        size += 16;
    if (format & OBJ_VERTEX_COLOR1)
        size += 16;
    if (format & OBJ_VERTEX_BONE_DATA)
        size += 24;
    if (format & OBJ_VERTEX_UNKNOWN)
        size += 16;
    return size;
}

inline static uint32_t obj_vertex_format_get_vertex_size_comp1(obj_vertex_format format) {
    uint32_t size = 0;
    if (format & OBJ_VERTEX_POSITION)
        size += 12;
    if (format & OBJ_VERTEX_NORMAL)
        size += 8;
    if (format & OBJ_VERTEX_TANGENT)
        size += 8;
    if (format & OBJ_VERTEX_TEXCOORD0)
        size += 4;
    if (format & OBJ_VERTEX_TEXCOORD1)
        size += 4;
    if (format & OBJ_VERTEX_TEXCOORD2)
        size += 4;
    if (format & OBJ_VERTEX_TEXCOORD3)
        size += 4;
    if (format & OBJ_VERTEX_COLOR0)
        size += 8;
    if (format & OBJ_VERTEX_BONE_DATA)
        size += 16;
    return size;
}

inline static uint32_t obj_vertex_format_get_vertex_size_comp2(obj_vertex_format format) {
    uint32_t size = 0;
    if (format & OBJ_VERTEX_POSITION)
        size += 12;
    if (format & OBJ_VERTEX_NORMAL)
        size += 4;
    if (format & OBJ_VERTEX_TANGENT)
        size += 4;
    if (format & OBJ_VERTEX_TEXCOORD0)
        size += 4;
    if (format & OBJ_VERTEX_TEXCOORD1)
        size += 4;
    if (format & OBJ_VERTEX_TEXCOORD2)
        size += 4;
    if (format & OBJ_VERTEX_TEXCOORD3)
        size += 4;
    if (format & OBJ_VERTEX_COLOR0)
        size += 8;
    if (format & OBJ_VERTEX_BONE_DATA)
        size += 8;
    return size;
}
