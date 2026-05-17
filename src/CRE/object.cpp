/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "object.hpp"
#include "../KKdLib/io/file_stream.hpp"
#include "../KKdLib/io/json.hpp"
#include "../KKdLib/io/path.hpp"
#include "../KKdLib/dds.hpp"
#include "../KKdLib/farc.hpp"
#include "../KKdLib/hash.hpp"
#include "../KKdLib/msgpack.hpp"
#include "../KKdLib/str_utils.hpp"
#include "prj/memory_manager.hpp"
#include "data.hpp"
#include "ogl_buffer_object.hpp"
#include "render_context.hpp"
#include "shader_ft.hpp"
#include <list>
#include <map>
#include <new>
#include <unordered_map>
#include <vector>

static int32_t align_val_32(int32_t value); // Added

static void calc_aabb(obj_axis_aligned_bounding_box& aabb, obj_mesh& mesh, obj_sub_mesh& sub_mesh);

#if SHARED_OBJECT_BUFFER
static void calc_index_buffer(obj_sub_mesh& sub_mesh, int32_t& size); // Added

static void calc_mesh_index_buffer(obj_mesh& mesh, int32_t& size); // Added
static int32_t calc_mesh_vertex_buffer(obj_mesh& mesh); // Added

static void calc_object_index_buffer(obj* obj, int32_t& size); // Added
static void calc_object_vertex_buffer(obj* obj, int32_t& size); // Added
#endif

static void convert_objdata_tex_index(ObjsetInfo* info);
static void convert_objdata_tex_index_internal(ObjsetInfo* info, obj* obj);

static void create_mesh_aabb(obj_mesh& mesh);

static void create_object_aabb(obj* obj);
static bool create_object_index_buffer(ObjIB* objib, obj* obj);
static bool create_object_vertex_buffer(ObjVB* objvb, obj* obj);
#if SHARED_OBJECT_BUFFER
inline bool create_object_vertex_buffer(ObjVB* objvb, obj* obj, GLuint in_vb, uint32_t& offset); // Added
#endif

static void create_objset_aabb(ObjsetInfo* info);

static void fill_index_buffer(void* buf, obj_sub_mesh& sub_mesh, int32_t& offset); // Added

static void fill_mesh_index_buffer(void* buf, obj_mesh& mesh, int32_t& offset); // Added
static int32_t fill_mesh_vertex_buffer(void* buf, obj_mesh& mesh); // Added

#if SHARED_OBJECT_BUFFER
static void fill_object_index_buffer(void* buf, obj* obj, int32_t& offset); // Added
static void fill_object_vertex_buffer(void* buf, obj* obj, int32_t& offset); // Added
#endif

static void free_objdata_indirect_table(ObjsetInfo* info);

static void free_object_index_buffer(ObjIB* objib);
static void free_object_vertex_buffer(ObjVB* objvb);

static void free_objset_texture(ObjsetInfo* info);

static bool load_objset_texture(ObjsetInfo* info, const void* data, bool big_endian = false);
static bool load_objset_texture_modern(ObjsetInfo* info,
    const void* data, size_t size, const char* file, texture_database* tex_db);

static void load_objset_vertex_array(ObjsetInfo* info); // Added

static void make_objdata_indirect_table(ObjsetInfo* info);

static int32_t rewrite_to_restart_index(uint32_t* new_indices,
    const int32_t num_indices, const uint32_t* indices);

std::map<uint32_t, ObjsetInfo> objset_info_storage_data;
std::map<uint32_t, ObjsetInfo> objset_info_storage_data_modern;

ObjsetInfo::ObjsetInfo() : obj_ready(), tex_ready(), obj_set(), tex_num(), textures(),
id(), objvb_num(), objvb(), objib_num(), objib(), req_cnt(), modern() {
#if SHARED_OBJECT_BUFFER
    ib = 0;
    vb = 0;
#endif
}

ObjsetInfo::~ObjsetInfo() {
    free_objdata_indirect_table(this);
    free_objset_texture(this);
    free_objset_index_buffer(this);
    free_objset_vertex_buffer(this);

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

// 0x1405E9250
void obj_skin_set_matrix_buffer(const obj_skin* s, const mat4* matrices,
    const mat4* ex_data_matrices, mat4* matrix_buffer, const mat4* mat, const mat4& global_mat) {
    if (!s->num_bone)
        return;

    obj_skin_bone* bone = s->bone_array;
    if (mat)
        for (int32_t i = 0; i < s->num_bone; i++, bone++, matrix_buffer++) {
            mat4 temp;
            if (bone->id & 0x8000)
                mat4_mul(mat, &ex_data_matrices[bone->id & 0x7FFF], &temp);
            else
                mat4_mul(mat, &matrices[bone->id], &temp);

            mat4_mul(&temp, &global_mat, &temp);
            mat4_mul(&bone->inv_bind_pose_mat, &temp, matrix_buffer);
        }
    else
        for (int32_t i = 0; i < s->num_bone; i++, bone++, matrix_buffer++) {
            mat4 temp;
            if (bone->id & 0x8000)
                temp = ex_data_matrices[bone->id & 0x7FFF];
            else
                temp = matrices[bone->id];

            mat4_mul(&temp, &global_mat, &temp);
            mat4_mul(&bone->inv_bind_pose_mat, &temp, matrix_buffer);
        }
}

void object_material_msgpack_read(const char* path, const char* set_name,
    obj_set* obj_set) {
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

    if (msg.type != MSGPACK_ARRAY) {
        printf("Failed to load Object Config JSON!\nPath: %s\n", buf);
        return;
    }

    msgpack_array* ptr = msg.data.arr;
    for (msgpack& i : *ptr) {
        msgpack& object = i;

        std::string name = object.read_string("name");
        uint32_t name_hash = hash_string_murmurhash(name);

        for (int32_t i = 0; i < obj_set->obj_num; i++) {
            obj* obj = obj_set->obj_data[i];

            if (name_hash != hash_utf8_murmurhash(obj->name))
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

                        if (name_hash != hash_utf8_murmurhash(mat.name))
                            continue;

                        msgpack* shader_compo = material.read("shader_compo");
                        if (shader_compo) {
                            msgpack* color = shader_compo->read("color");
                            if (color)
                                mat.shader_compo.m.color = color->read_bool() ? 1 : 0;

                            msgpack* color_a = shader_compo->read("color_a");
                            if (color_a)
                                mat.shader_compo.m.color_a = color_a->read_bool() ? 1 : 0;

                            msgpack* color_l1 = shader_compo->read("color_l1");
                            if (color_l1)
                                mat.shader_compo.m.color_l1 = color_l1->read_bool() ? 1 : 0;

                            msgpack* color_l1_a = shader_compo->read("color_l1_a");
                            if (color_l1_a)
                                mat.shader_compo.m.color_l1_a = color_l1_a->read_bool() ? 1 : 0;

                            msgpack* color_l2 = shader_compo->read("color_l2");
                            if (color_l2)
                                mat.shader_compo.m.color_l2 = color_l2->read_bool() ? 1 : 0;

                            msgpack* color_l2_a = shader_compo->read("color_l2_a");
                            if (color_l2_a)
                                mat.shader_compo.m.color_l2_a = color_l2_a->read_bool() ? 1 : 0;

                            msgpack* transparency = shader_compo->read("transparency");
                            if (transparency)
                                mat.shader_compo.m.transparency = transparency->read_bool() ? 1 : 0;

                            msgpack* specular = shader_compo->read("specular");
                            if (specular)
                                mat.shader_compo.m.specular = specular->read_bool() ? 1 : 0;

                            msgpack* normal_01 = shader_compo->read("normal_01");
                            if (normal_01)
                                mat.shader_compo.m.normal_01 = normal_01->read_bool() ? 1 : 0;

                            msgpack* normal_02 = shader_compo->read("normal_02");
                            if (normal_02)
                                mat.shader_compo.m.normal_02 = normal_02->read_bool() ? 1 : 0;

                            msgpack* envmap = shader_compo->read("envmap");
                            if (envmap)
                                mat.shader_compo.m.envmap = envmap->read_bool() ? 1 : 0;

                            msgpack* color_l3 = shader_compo->read("color_l3");
                            if (color_l3)
                                mat.shader_compo.m.color_l3 = color_l3->read_bool() ? 1 : 0;

                            msgpack* color_l3_a = shader_compo->read("color_l3_a");
                            if (color_l3_a)
                                mat.shader_compo.m.color_l3_a = color_l3_a->read_bool() ? 1 : 0;

                            msgpack* translucency = shader_compo->read("translucency");
                            if (translucency)
                                mat.shader_compo.m.translucency = translucency->read_bool() ? 1 : 0;

                            msgpack* env_sphere = shader_compo->read({ "env_sphere", "flag_14" });
                            if (env_sphere)
                                mat.shader_compo.m.env_sphere = env_sphere->read_bool() ? 1 : 0;

                            msgpack* env_cube = shader_compo->read({ "env_cube", "override_ibl" });
                            if (env_cube)
                                mat.shader_compo.m.env_cube = env_cube->read_bool() ? 1 : 0;

                            msgpack* dummy = shader_compo->read("dummy");
                            if (dummy)
                                mat.shader_compo.m.dummy = dummy->read_uint32_t();
                        }

                        msgpack* _shader_name = material.read("shader_name");
                        if (_shader_name) {
                            std::string shader_name = _shader_name->read_string();
                            size_t name_length = min_def(sizeof(mat.shader.name) - 1, shader_name.size());
                            memcpy_s(mat.shader.name, sizeof(mat.shader.name) - 1, shader_name.c_str(), name_length);
                            memset(mat.shader.name + name_length, 0, sizeof(mat.shader.name) - name_length);
                        }

                        msgpack* shader_info = material.read("shader_info");
                        if (shader_info) {
                            msgpack* vtx_trans_type = shader_info->read("vtx_trans_type");
                            if (vtx_trans_type)
                                mat.shader_info.m.vtx_trans_type = (obj_material_vertex_translation_type)
                                vtx_trans_type->read_uint32_t();

                            msgpack* col_src = shader_info->read("col_src");
                            if (col_src)
                                mat.shader_info.m.col_src
                                = (obj_material_color_source_type)col_src->read_uint32_t("col_src");

                            msgpack* is_lgt_diffuse = shader_info->read("is_lgt_diffuse");
                            if (is_lgt_diffuse)
                                mat.shader_info.m.is_lgt_diffuse = is_lgt_diffuse->read_bool() ? 1 : 0;

                            msgpack* is_lgt_specular = shader_info->read("is_lgt_specular");
                            if (is_lgt_specular)
                                mat.shader_info.m.is_lgt_specular = is_lgt_specular->read_bool() ? 1 : 0;

                            msgpack* is_lgt_per_pixel = shader_info->read("is_lgt_per_pixel");
                            if (is_lgt_per_pixel)
                                mat.shader_info.m.is_lgt_per_pixel = is_lgt_per_pixel->read_bool() ? 1 : 0;

                            msgpack* is_lgt_double = shader_info->read("is_lgt_double");
                            if (is_lgt_double)
                                mat.shader_info.m.is_lgt_double = is_lgt_double->read_bool() ? 1 : 0;

                            msgpack* bump_map_type = shader_info->read("bump_map_type");
                            if (bump_map_type)
                                mat.shader_info.m.bump_map_type = (obj_material_bump_map_type)
                                bump_map_type->read_uint32_t();

                            msgpack* fresnel_type = shader_info->read("fresnel_type");
                            if (fresnel_type)
                                mat.shader_info.m.fresnel_type = fresnel_type->read_uint32_t();

                            msgpack* line_light = shader_info->read("line_light");
                            if (line_light)
                                mat.shader_info.m.line_light = line_light->read_uint32_t();

                            msgpack* receive_shadow = shader_info->read({ "receive_shadow", "recieve_shadow" });
                            if (receive_shadow)
                                mat.shader_info.m.receive_shadow = receive_shadow->read_bool() ? 1 : 0;

                            msgpack* cast_shadow = shader_info->read("cast_shadow");
                            if (cast_shadow)
                                mat.shader_info.m.cast_shadow = cast_shadow->read_bool() ? 1 : 0;

                            msgpack* specular_quality = shader_info->read("specular_quality");
                            if (specular_quality)
                                mat.shader_info.m.specular_quality = (obj_material_specular_quality)
                                specular_quality->read_uint32_t();

                            msgpack* aniso_direction = shader_info->read("aniso_direction");
                            if (aniso_direction)
                                mat.shader_info.m.aniso_direction = (obj_material_aniso_direction)
                                aniso_direction->read_uint32_t();

                            msgpack* dummy = shader_info->read("dummy");
                            if (dummy)
                                mat.shader_info.m.dummy = dummy->read_uint32_t();
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
                                    msgpack* repeat_u = attrib->read("repeat_u");
                                    if (repeat_u)
                                        l.attrib.m.repeat_u = repeat_u->read_bool() ? 1 : 0;

                                    msgpack* repeat_v = attrib->read("repeat_v");
                                    if (repeat_v)
                                        l.attrib.m.repeat_v = repeat_v->read_bool() ? 1 : 0;

                                    msgpack* mirror_u = attrib->read("mirror_u");
                                    if (mirror_u)
                                        l.attrib.m.mirror_u = mirror_u->read_bool() ? 1 : 0;

                                    msgpack* mirror_v = attrib->read("mirror_v");
                                    if (mirror_v)
                                        l.attrib.m.mirror_v = mirror_v->read_bool() ? 1 : 0;

                                    msgpack* ignore_alpha = attrib->read("ignore_alpha");
                                    if (ignore_alpha)
                                        l.attrib.m.ignore_alpha = ignore_alpha->read_bool() ? 1 : 0;

                                    msgpack* blend = attrib->read("blend");
                                    if (blend)
                                        l.attrib.m.blend = blend->read_uint32_t();

                                    msgpack* alpha_blend = attrib->read("alpha_blend");
                                    if (alpha_blend)
                                        l.attrib.m.alpha_blend = alpha_blend->read_uint32_t();

                                    msgpack* border = attrib->read("border");
                                    if (border)
                                        l.attrib.m.border = border->read_bool() ? 1 : 0;

                                    msgpack* clamp2edge = attrib->read("clamp2edge");
                                    if (clamp2edge)
                                        l.attrib.m.clamp2edge = clamp2edge->read_bool() ? 1 : 0;

                                    msgpack* filter = attrib->read("filter");
                                    if (filter)
                                        l.attrib.m.filter = filter->read_uint32_t();

                                    msgpack* mipmap = attrib->read("mipmap");
                                    if (mipmap)
                                        l.attrib.m.mipmap = mipmap->read_uint32_t();

                                    msgpack* mipmap_bias = attrib->read("mipmap_bias");
                                    if (mipmap_bias)
                                        l.attrib.m.mipmap_bias = mipmap_bias->read_uint32_t();

                                    msgpack* ignore = attrib->read({ "ignore", "flag_29" });
                                    if (ignore)
                                        l.attrib.m.ignore = ignore->read_bool() ? 1 : 0;

                                    msgpack* aniso = attrib->read({ "aniso", "anisotropic_filter" });
                                    if (aniso)
                                        l.attrib.m.aniso = aniso->read_uint32_t();
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
                                    memset(l.ex_shader + name_length, 0, sizeof(l.ex_shader) - name_length);
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

                            msgpack* alpha_tex = attrib->read({ "alpha_tex", "alpha_texture" });
                            if (alpha_tex)
                                mat.attrib.m.alpha_tex = alpha_tex->read_bool() ? 1 : 0;

                            msgpack* alpha_mat = attrib->read({ "alpha_mat", "alpha_material" });
                            if (alpha_mat)
                                mat.attrib.m.alpha_mat = alpha_mat->read_bool() ? 1 : 0;

                            msgpack* punch_through = attrib->read("punch_through");
                            if (punch_through)
                                mat.attrib.m.punch_through = punch_through->read_bool() ? 1 : 0;

                            msgpack* double_sided = attrib->read("double_sided");
                            if (double_sided)
                                mat.attrib.m.double_sided = double_sided->read_bool() ? 1 : 0;

                            msgpack* normal_dir_light = attrib->read("normal_dir_light");
                            if (normal_dir_light)
                                mat.attrib.m.normal_dir_light = normal_dir_light->read_bool() ? 1 : 0;

                            msgpack* src_blend_factor = attrib->read("src_blend_factor");
                            if (src_blend_factor)
                                mat.attrib.m.src_blend_factor = (obj_material_blend_factor)
                                src_blend_factor->read_uint32_t();

                            msgpack* dst_blend_factor = attrib->read("dst_blend_factor");
                            if (dst_blend_factor)
                                mat.attrib.m.dst_blend_factor = (obj_material_blend_factor)
                                dst_blend_factor->read_uint32_t();

                            msgpack* blend_operation = attrib->read("blend_operation");
                            if (blend_operation)
                                mat.attrib.m.blend_operation = blend_operation->read_uint32_t();

                            msgpack* zbias = attrib->read("zbias");
                            if (zbias)
                                mat.attrib.m.zbias = zbias->read_uint32_t();

                            msgpack* no_z_fog = attrib->read({ "no_z_fog", "no_fog" });
                            if (no_z_fog)
                                mat.attrib.m.no_z_fog = no_z_fog->read_bool() ? 1 : 0;

                            msgpack* alpha_prio = attrib->read({ "alpha_prio", "translucent_priority" });
                            if (alpha_prio)
                                mat.attrib.m.alpha_prio = alpha_prio->read_uint32_t();

                            msgpack* y_fog = attrib->read({ "y_fog", "has_fog_height" });
                            if (y_fog)
                                mat.attrib.m.y_fog = y_fog->read_bool() ? 1 : 0;

                            msgpack* ignore_alpha = attrib->read({ "ignore_alpha", "flag_28" });
                            if (ignore_alpha)
                                mat.attrib.m.ignore_alpha = ignore_alpha->read_bool() ? 1 : 0;

                            msgpack* y_fogmap = attrib->read({ "y_fogmap", "fog_height" });
                            if (y_fogmap)
                                mat.attrib.m.y_fogmap = y_fogmap->read_bool() ? 1 : 0;

                            msgpack* use_mat_center = attrib->read({ "use_mat_center", "flag_30" });
                            if (use_mat_center)
                                mat.attrib.m.use_mat_center = use_mat_center->read_bool() ? 1 : 0;

                            msgpack* dummy = attrib->read({ "dummy", "flag_31" });
                            if (dummy)
                                mat.attrib.m.dummy = dummy->read_bool() ? 1 : 0;
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
                            mat.bump_depth = bump_depth->read_float_t();

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

                        if (name_hash != hash_utf8_murmurhash(mesh.name))
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
                            int32_t num_vertex = mesh.num_vertex;
                            for (int32_t i = num_vertex; i > 0; i--, vtx++)
                                *(vec3*)&vtx->color0 = _color_mult;
                        }

                        msgpack* fix_alpha = _mesh.read("fix_alpha");
                        if (fix_alpha && fix_alpha->read_bool()) {
                            obj_vertex_data* vtx = mesh.vertex_array;
                            int32_t num_vertex = mesh.num_vertex;
                            for (int32_t i = num_vertex; i > 0; i--, vtx++)
                                vtx->color0.w = 1.0f;
                        }

                        msgpack* fix_normals = _mesh.read("fix_zero_normals");
                        if (fix_normals && fix_normals->read_bool()) {
                            std::unordered_map<uint32_t, vec3> normals;

                            obj_vertex_data* vtx = mesh.vertex_array;
                            for (size_t l = 0; l < mesh.num_submesh; l++) {
                                obj_sub_mesh& sub_mesh = mesh.submesh_array[l];
                                if (sub_mesh.primitive_type != OBJ_PRIMITIVE_TRIANGLES)
                                    continue;

                                uint32_t* index = sub_mesh.index_array;
                                int32_t num_index = sub_mesh.num_index;
                                for (int32_t l = 0; l < num_index; l++) {
                                    if (vtx[index[l]].normal != 0.0f)
                                        continue;

                                    const int32_t idx = l / 3 * 3;
                                    const uint32_t v[3] = { index[idx + 0], index[idx + 1], index[idx + 2] };

                                    bool found = false;
                                    for (int32_t m = 0; m < 3; m++) {
                                        if (vtx[v[m]].normal == 0.0f)
                                            continue;

                                        vec3 normal = vtx[v[m]].normal;
                                        if (vtx[v[0]].normal == 0.0f)
                                            normals[v[0]] += normal;
                                        if (vtx[v[1]].normal == 0.0f)
                                            normals[v[1]] += normal;
                                        if (vtx[v[2]].normal == 0.0f)
                                            normals[v[2]] += normal;

                                        found = true;
                                    }

                                    if (!found) {
                                        const vec3 p0 = vtx[v[0]].position;
                                        const vec3 p1 = vtx[v[1]].position;
                                        const vec3 p2 = vtx[v[2]].position;
                                        const vec3 normal = vec3::cross(p1 - p0, p2 - p0);
                                        normals[v[0]] += normal;
                                        normals[v[1]] += normal;
                                        normals[v[2]] += normal;
                                    }
                                }
                            }

                            for (auto& l : normals)
                                vtx[l.first].normal = vec3::normalize(l.second);
                        }

                        msgpack* invert_negative_color = _mesh.read("invert_negative_color");
                        if (invert_negative_color && invert_negative_color->read_bool()) {
                            obj_vertex_data* vtx = mesh.vertex_array;
                            int32_t num_vertex = mesh.num_vertex;
                            for (int32_t i = num_vertex; i > 0; i--, vtx++)
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
                            int32_t num_vertex = mesh.num_vertex;
                            for (int32_t i = num_vertex; i > 0; i--, vtx++)
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
                                msgpack* receive_shadow = attrib->read({ "receive_shadow", "recieve_shadow" });
                                if (receive_shadow)
                                    sub_mesh.attrib.m.receive_shadow = receive_shadow->read_bool();

                                msgpack* cast_shadow = attrib->read("cast_shadow");
                                if (cast_shadow)
                                    sub_mesh.attrib.m.cast_shadow = cast_shadow->read_bool();

                                msgpack* vertex_alpha = attrib->read({ "vertex_alpha", "translucent" });
                                if (vertex_alpha)
                                    sub_mesh.attrib.m.vertex_alpha = vertex_alpha->read_bool();

                                msgpack* hide = attrib->read("hide");
                                if (hide)
                                    sub_mesh.attrib.m.hide = hide->read_bool();

                                msgpack* poly_offset = attrib->read("poly_offset");
                                if (poly_offset)
                                    sub_mesh.attrib.m.poly_offset = poly_offset->read_uint32_t();

                                msgpack* use_restart_index = attrib->read("use_restart_index");
                                if (hide)
                                    sub_mesh.attrib.m.use_restart_index = hide->read_bool();
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

    if (msg.type != MSGPACK_MAP) {
        printf("Failed to load Texture Config JSON!\nPath: %s\n", buf);
        return;
    }

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
                for (int32_t i = 0; i < tex->mipmaps_count; i++) {
                    txp_mipmap tex_mip;
                    tex_mip.width = max_def(d.width >> i, 1);
                    tex_mip.height = max_def(d.height >> i, 1);
                    tex_mip.format = d.format;

                    int32_t size = tex_mip.get_size();
                    tex_mip.size = size;
                    tex_mip.data.resize(size);
                    memcpy(tex_mip.data.data(), d.data[index], size);
                    tex->mipmaps.push_back(tex_mip);
                    index++;
                }
            while (index / tex->mipmaps_count < tex->array_size);
        }

        tex_db->update();

        int32_t tex_id_num = (int32_t)txp_set->textures.size();
        if (set->tex_id_num != tex_id_num) {
            uint32_t* tex_id_data = info->alloc_handler->allocate<uint32_t>(tex_id_num);
            memmove(tex_id_data, set->tex_id_data, sizeof(uint32_t) * set->tex_id_num);
            memmove(&tex_id_data[set->tex_id_num], ids.data(),
                sizeof(uint32_t) * ((size_t)tex_id_num - set->tex_id_num));
            set->tex_id_data = tex_id_data;
            set->tex_id_num = tex_id_num;
            info->tex_num = tex_id_num;
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
            int32_t tex_id_num = set->tex_id_num;

            txp* tex = 0;
            for (int32_t i = 0; i < tex_id_num; i++)
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
                for (int32_t i = 0; i < tex->mipmaps_count; i++) {
                    txp_mipmap tex_mip;
                    tex_mip.width = max_def(d.width >> i, 1);
                    tex_mip.height = max_def(d.height >> i, 1);
                    tex_mip.format = d.format;

                    int32_t size = tex_mip.get_size();
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
    obj_set* obj_set, txp_set* txp_set, texture_database* tex_db) {
    if (!path_check_directory_exists(path) && !path_create_directory(path))
        return;

    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "%s\\%s\\", path, set_name);
    if (!path_check_directory_exists(buf) && !path_create_directory(buf))
        return;

    msgpack_array objects(obj_set->obj_num);
    for (int32_t i = 0; i < obj_set->obj_num; i++) {
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
                    shader_compo->append("env_sphere", (bool)mat.shader_compo.m.env_sphere);
                    shader_compo->append("env_cube", (bool)mat.shader_compo.m.env_cube);
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
                    shader_info->append("receive_shadow", (bool)mat.shader_info.m.receive_shadow);
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
                            attrib->append("ignore", (bool)k.attrib.m.ignore);
                            attrib->append("aniso", k.attrib.m.aniso);
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
                    attrib->append("alpha_tex", (bool)mat.attrib.m.alpha_tex);
                    attrib->append("alpha_mat", (bool)mat.attrib.m.alpha_mat);
                    attrib->append("punch_through", (bool)mat.attrib.m.punch_through);
                    attrib->append("double_sided", (bool)mat.attrib.m.double_sided);
                    attrib->append("normal_dir_light", (bool)mat.attrib.m.normal_dir_light);
                    attrib->append("src_blend_factor", mat.attrib.m.src_blend_factor);
                    attrib->append("dst_blend_factor", mat.attrib.m.dst_blend_factor);
                    attrib->append("blend_operation", mat.attrib.m.blend_operation);
                    attrib->append("zbias", mat.attrib.m.zbias);
                    attrib->append("no_z_fog", (bool)mat.attrib.m.no_z_fog);
                    attrib->append("alpha_prio", mat.attrib.m.alpha_prio);
                    attrib->append("y_fog", (bool)mat.attrib.m.y_fog);
                    attrib->append("ignore_alpha", (bool)mat.attrib.m.ignore_alpha);
                    attrib->append("y_fogmap", (bool)mat.attrib.m.y_fogmap);
                    attrib->append("use_mat_center", (bool)mat.attrib.m.use_mat_center);
                    attrib->append("dummy", (bool)mat.attrib.m.dummy);
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
                            attrib.append("receive_shadow", (bool)sub_mesh.attrib.m.receive_shadow);
                            attrib.append("cast_shadow", (bool)sub_mesh.attrib.m.cast_shadow);
                            attrib.append("vertex_alpha", (bool)sub_mesh.attrib.m.vertex_alpha);
                            attrib.append("hide", (bool)sub_mesh.attrib.m.hide);
                            attrib.append("poly_offset", sub_mesh.attrib.m.poly_offset);
                            attrib.append("use_restart_index", (bool)sub_mesh.attrib.m.use_restart_index);
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

    for (int32_t i = 0; i < obj_set->tex_id_num; i++) {
        const char* texture_name = tex_db->get_texture_name(obj_set->tex_id_data[i]);

        txp& tex = txp_set->textures[i];

        txp_format format = tex.mipmaps[0].format;
        int32_t width = tex.mipmaps[0].width;
        int32_t height = tex.mipmaps[0].height;

        dds d;
        d.format = format;
        d.width = width;
        d.height = height;
        d.mipmaps_count = tex.mipmaps_count;
        d.has_cube_map = tex.has_cube_map;
        d.data.reserve((tex.has_cube_map ? 6LL : 1LL) * tex.mipmaps_count);
        int32_t index = 0;
        do
            for (int32_t j = 0; j < tex.mipmaps_count; j++) {
                int32_t size = txp::get_size(format,
                    max_def(width >> j, 1), max_def(height >> j, 1));
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

inline void objset_info_storage_init(const object_database* obj_db) {
    for (const object_set_info& i : obj_db->object_set) {
        ObjsetInfo info;
        info.id = i.id;
        info.name.assign(i.name);
        objset_info_storage_data.insert({ i.id, info });
    }
    objset_info_storage_data_modern.clear();
}

inline void objset_info_storage_free() {
    objset_info_storage_data.clear();
    objset_info_storage_data_modern.clear();
}

// 0x140457910
inline bool check_objset_ready(uint32_t objset_index) {
    ObjsetInfo* info = get_objset_info(objset_index);
    if (info)
        return info->obj_ready && info->tex_ready;
    return false;
}

// 0x140458040
bool create_mesh_index_buffer(IndexBuffer& ibhn, obj_mesh& mesh) {
    uint32_t size = 0;
    for (int32_t i = 0; i < mesh.num_submesh; i++) {
        obj_sub_mesh& sub_mesh = mesh.submesh_array[i];
        if (sub_mesh.index_format == OBJ_INDEX_U16)
            size += align_val_32((int32_t)(sizeof(uint16_t) * sub_mesh.num_index));
    }

    if (!size) {
        ibhn.ib = 0;
        return true;
    }

    void* buf = force_malloc(size);
    int32_t offset = 0;
    fill_mesh_index_buffer(buf, mesh, offset);

    ibhn.create(offset, buf);
    free_def(buf);
    return true;
}

#if SHARED_OBJECT_BUFFER
// Added
inline void create_mesh_index_buffer(IndexBuffer& ibhn, obj_mesh& mesh, GLuint in_ib) {
    //ibhn.create(in_ib, size);
}
#endif

// 0x140458280
bool create_mesh_vertex_buffer(VertexBuffer& vbhn, obj_mesh& mesh, GL::BufferUsage usage) {
    if (!mesh.num_vertex || !mesh.vertex_array)
        return false;

    uint32_t size_vertex = get_vertex_size(mesh.vertex_format, mesh.attrib.m.compression);
    void* buf = prj::MemoryManager::alloc(prj::MemCTemp, (size_t)size_vertex * mesh.num_vertex, "TMP_VTXBUF");
    fill_mesh_vertex_buffer(buf, mesh);
    mesh.size_vertex = size_vertex;

    bool ret = vbhn.create((size_t)size_vertex * mesh.num_vertex,
        buf, mesh.attrib.m.soft_body ? 2 : 1, usage);
    prj::MemoryManager::free(prj::MemCTemp, buf);
    return ret;
}

#if SHARED_OBJECT_BUFFER
// Added
inline void create_mesh_vertex_buffer(VertexBuffer& vbhn,
    obj_mesh& mesh, GLuint in_vb, uint32_t& offset) {
    if (!mesh.num_vertex || !mesh.vertex_array)
        return;

    uint32_t size_vertex = get_vertex_size(mesh.vertex_format, mesh.attrib.m.compression);
    mesh.size_vertex = size_vertex;

    uint32_t size = size_vertex * mesh.num_vertex;
    vbhn.create(in_vb, size, offset);
    offset += align_val_32(size);
}
#endif

// 0x1404588F0
bool create_objset_index_buffer(ObjsetInfo* info) {
    obj_set* set = info->obj_set;
    info->objib_num = set->obj_num;
    info->objib = prj::MemoryManager::alloc<ObjIB>(prj::MemCSystem, set->obj_num, "OBJIB");
    if (!info->objib)
        return false;

#if SHARED_OBJECT_BUFFER
    for (int32_t i = 0; i < set->obj_num; i++) {
        obj* obj = set->obj_data[i];
        ObjIB& objib = info->objib[i];

        objib.num_ib = obj->num_mesh;

        IndexBuffer* ibhn_array = prj::MemoryManager::alloc<IndexBuffer>(prj::MemCSystem, obj->num_mesh, "MESHIB");
        if (!ibhn_array)
            return false;

        objib.ibhn_array = new (ibhn_array) IndexBuffer[obj->num_mesh];
    }

    int32_t size = 0;
    for (int32_t i = 0; i < set->obj_num; i++)
        calc_object_index_buffer(set->obj_data[i], size);

    void* buf = prj::MemoryManager::alloc(prj::MemCTemp, size, "TMP_IDXBUF");
    memset(buf, 0, size);
    if (!buf)
        return false;

    int32_t fill_offset = 0;
    for (int32_t i = 0; i < set->obj_num; i++)
        fill_object_index_buffer(buf, set->obj_data[i], fill_offset);

    GLuint ib = 0;
    if (fill_offset > 0) // size is bigger than filled buf
        ib = create_index_buffer(fill_offset, buf);
    info->ib = ib;

    if (buf)
        prj::MemoryManager::free(prj::MemCTemp, buf);

    for (int32_t i = 0; i < set->obj_num; i++) {
        obj* obj = set->obj_data[i];
        ObjIB& objib = info->objib[i];

        objib.num_ib = obj->num_mesh;
        for (int32_t j = 0; j < obj->num_mesh; j++)
#pragma warning(suppress: 6385)
            objib.ibhn_array[j].create(ib);
    }
#else
    for (int32_t i = 0; i < set->obj_num; i++)
        if (!create_object_index_buffer(&info->objib[i], set->obj_data[i]))
            return false;
#endif
    return true;
}

// 0x1404589B0
bool create_objset_vertex_buffer(ObjsetInfo* info) {
    obj_set* set = info->obj_set;
    info->objvb_num = set->obj_num;
    info->objvb = prj::MemoryManager::alloc<ObjVB>(prj::MemCSystem, set->obj_num, "OBJVB");
    if (!info->objvb)
        return false;

#if SHARED_OBJECT_BUFFER
    int32_t size = 0;
    for (int32_t i = 0; i < set->obj_num; i++)
        calc_object_vertex_buffer(set->obj_data[i], size);

    void* buf = prj::MemoryManager::alloc(prj::MemCTemp, size, "TMP_VTXBUF");
    memset(buf, 0, size);

    int32_t fill_offset = 0;
    for (int32_t i = 0; i < set->obj_num; i++)
        fill_object_vertex_buffer(buf, set->obj_data[i], fill_offset);

    GLuint vb = 0;
    if (size > 0)
        vb = create_vertex_buffer(size, buf);
    info->vb = vb;

    if (buf)
        prj::MemoryManager::free(prj::MemCTemp, buf);

    uint32_t offset = 0;
    for (int32_t i = 0; i < set->obj_num; i++)
        create_object_vertex_buffer(&info->objvb[i], set->obj_data[i], vb, offset);
#else
    for (int32_t i = 0; i < set->obj_num; i++)
        if (!info->objvb[i].load(set->obj_data[i]))
            return false;
#endif
    return true;
}

// 0x140459860
inline bool find_objdata_index(ObjsetInfoObject& info_object, object_info obj_id) {
    info_object.info = get_objset_info(obj_id.set_id);
    if (!info_object.info)
        return false;

    auto elem = info_object.info->objdb_map.find(obj_id.id);
    if (elem == info_object.info->objdb_map.end())
        return false;

    info_object.index = elem->second;
    return true;
}

// 0x140459B40
void free_objset_index_buffer(ObjsetInfo* info) {
    if (info->objib) {
        for (int32_t i = 0; i < info->objib_num; i++)
            free_object_index_buffer(&info->objib[i]);
        prj::MemoryManager::free(prj::MemCSystem, info->objib);
    }

    info->objib = 0;
    info->objib_num = 0;

#if SHARED_OBJECT_BUFFER
    if (info->ib)
        free_index_buffer(info->ib);
    info->ib = 0;
#endif
}

// 0x140459C70
void free_objset_vertex_buffer(ObjsetInfo* info) {
    if (info->objvb) {
        for (int32_t i = 0; i < info->objvb_num; i++)
            free_object_vertex_buffer(&info->objvb[i]);
        prj::MemoryManager::free(prj::MemCSystem, info->objvb);
    }

    info->objvb = 0;
    info->objvb_num = 0;

#if SHARED_OBJECT_BUFFER
    if (info->vb)
        free_vertex_buffer(info->vb);
    info->vb = 0;
#endif
}

// 0x140459EC0
inline obj_set* get_obj_data_header(uint32_t objset_index) {
    auto elem = objset_info_storage_data.find(objset_index);
    if (elem != objset_info_storage_data.end())
        return elem->second.obj_set;

    auto elem_modern = objset_info_storage_data_modern.find(objset_index);
    if (elem_modern != objset_info_storage_data_modern.end())
        return elem_modern->second.obj_set;
    return 0;
}

// 0x14045A120
inline obj_bounding_sphere* get_object_bsphere(object_info obj_id) {
    obj* obj = get_object_header(obj_id);
    if (obj)
        return &obj->bounding_sphere;
    return 0;
}

// 0x14045A140
inline obj* get_object_header(object_info obj_id) {
    ObjsetInfo* info = get_objset_info(obj_id.set_id);
    if (info) {
        obj_set* set = info->obj_set;
        if (set) {
            auto elem_obj = info->objdb_map.find(obj_id.id);
            if (elem_obj != info->objdb_map.end())
                return set->obj_data[elem_obj->second];
        }
    }
    return 0;
}

// 0x14045A250
inline IndexBuffer* get_object_index_buffer(object_info obj_id) {
    ObjsetInfo* info = get_objset_info(obj_id.set_id);
    if (info && info->obj_set && info->objib) {
        auto elem = info->objdb_map.find(obj_id.id);
        if (elem != info->objdb_map.end())
            return info->objib[elem->second].ibhn_array;
    }
    return 0;
}

// 0x14045A340
inline const char* get_object_name(object_info obj_id) {
    obj* obj = get_object_header(obj_id);
    if (obj)
        return obj->name;
    return 0;
}

// 0x14045A3C0
inline int32_t get_object_num(uint32_t objset_index) {
    obj_set* set = get_obj_data_header(objset_index);
    if (set)
        return set->obj_num;
    return 0;
}

// 0x14045A3E0
inline obj_skin* get_object_skin(object_info obj_id) {
    auto elem = objset_info_storage_data.find(obj_id.set_id);
    if (elem != objset_info_storage_data.end()) {
        obj_set* set = elem->second.obj_set;
        if (!set)
            return 0;

        auto elem_obj = elem->second.objdb_map.find(obj_id.id);
        if (elem_obj != elem->second.objdb_map.end())
            return set->obj_data[elem_obj->second]->skin;
        return 0;
    }

    auto elem_modern = objset_info_storage_data_modern.find(obj_id.set_id);
    if (elem_modern != objset_info_storage_data_modern.end()) {
        obj_set* set = elem_modern->second.obj_set;
        if (!set)
            return 0;

        auto elem_obj = elem_modern->second.objdb_map.find(obj_id.id);
        if (elem_obj != elem_modern->second.objdb_map.end())
            return set->obj_data[elem_obj->second]->skin;
        return 0;
    }
    return 0;
}

// 0x14045A460
inline obj_skin_ex_data* get_object_skin_osage_header(object_info obj_id) {
    obj_skin* skin = get_object_skin(obj_id);
    if (skin)
        return skin->ex_data;
    return 0;
}

// 0x14045A480
inline VertexBuffer* get_object_vertex_buffer(object_info obj_id) {
    ObjsetInfo* info = get_objset_info(obj_id.set_id);
    if (info && info->obj_set && info->objvb) {
        auto elem = info->objdb_map.find(obj_id.id);
        if (elem != info->objdb_map.end())
            return info->objvb[elem->second].vbhn_array;
    }
    return 0;
}

// 0x14045A750
inline uint32_t get_objnum_idx2uid(uint32_t objset_index, int32_t obj_index, const object_database* obj_db) {
    uint32_t set_id = obj_db->get_object_set_id(objset_index);
    if (set_id != -1) {
        ObjsetInfo* info = get_objset_info(set_id);
        if (info) {
            obj_set* set = info->obj_set;
            if (set && obj_index >= 0 && obj_index < set->obj_num)
                return set->obj_data[obj_index]->id;
        }
    }
    return -1;
}

// 0x14045A8F0
inline GLuint get_objset_gen_textures_id(uint32_t objset_index, uint32_t uid) {
    std::vector<GLuint>* gentex_vec = get_objset_gen_textures_vec(objset_index);
    if (!gentex_vec)
        return 0;

    ObjsetInfo* info = get_objset_info(objset_index);
    if (!info)
        return 0;

    auto elem = info->texidx_map.find(uid);
    if (elem != info->texidx_map.end())
        return (*gentex_vec)[elem->second];
    return 0;
}

// 0x14045A9E0
inline std::vector<GLuint>* get_objset_gen_textures_vec(uint32_t objset_index) {
    ObjsetInfo* info = get_objset_info(objset_index);
    if (info)
        return &info->gentex_vec;
    return 0;
}

// 0x14045AC00
inline ObjsetInfo* get_objset_info(uint32_t objset_index) {
    auto elem = objset_info_storage_data.find(objset_index);
    if (elem != objset_info_storage_data.end())
        return &elem->second;

    auto elem_modern = objset_info_storage_data_modern.find(objset_index);
    if (elem_modern != objset_info_storage_data_modern.end())
        return &elem_modern->second;
    return 0;
}

// 0x14045ADD0
inline size_t get_objset_num() {
    return objset_info_storage_data.size() + objset_info_storage_data_modern.size();
}

// 0x14045ADE0
inline int32_t get_objset_num_textures(uint32_t objset_index) {
    ObjsetInfo* info = get_objset_info(objset_index);
    if (info)
        return info->tex_num;
    return 0;
}

// 0x14045AE20
inline texture** get_objset_textures(uint32_t objset_index) {
    ObjsetInfo* info = get_objset_info(objset_index);
    if (info)
        return info->textures;
    return 0;
}

// 0x140459D90
inline obj_mesh* get_mesh(object_info obj_id, const char* mesh_name) {
    auto elem = objset_info_storage_data.find(obj_id.set_id);
    if (elem != objset_info_storage_data.end()) {
        obj_set* set = elem->second.obj_set;
        if (!set)
            return 0;

        auto elem_obj = elem->second.objdb_map.find(obj_id.id);
        if (elem_obj != elem->second.objdb_map.end())
            return set->obj_data[elem_obj->second]->get_obj_mesh(mesh_name);
        return 0;
    }

    auto elem_modern = objset_info_storage_data_modern.find(obj_id.set_id);
    if (elem_modern != objset_info_storage_data_modern.end()) {
        obj_set* set = elem_modern->second.obj_set;
        if (!set)
            return 0;

        auto elem_obj = elem_modern->second.objdb_map.find(obj_id.id);
        if (elem_obj != elem_modern->second.objdb_map.end())
            return set->obj_data[elem_obj->second]->get_obj_mesh(mesh_name);
        return 0;
    }
    return 0;
}

// 0x140459D40
inline obj_mesh* get_mesh(object_info obj_id, int32_t mesh_index) {
    auto elem = objset_info_storage_data.find(obj_id.set_id);
    if (elem != objset_info_storage_data.end()) {
        obj_set* set = elem->second.obj_set;
        if (!set)
            return 0;

        auto elem_obj = elem->second.objdb_map.find(obj_id.id);
        if (elem_obj != elem->second.objdb_map.end()) {
            obj* obj = set->obj_data[elem_obj->second];
            if (mesh_index >= 0 && mesh_index < obj->num_mesh)
                return &obj->mesh_array[mesh_index];
        }
        return 0;
    }

    auto elem_modern = objset_info_storage_data_modern.find(obj_id.set_id);
    if (elem_modern != objset_info_storage_data_modern.end()) {
        obj_set* set = elem_modern->second.obj_set;
        if (!set)
            return 0;

        auto elem_obj = elem_modern->second.objdb_map.find(obj_id.id);
        if (elem_obj != elem_modern->second.objdb_map.end()) {
            obj* obj = set->obj_data[elem_obj->second];
            if (mesh_index >= 0 && mesh_index < obj->num_mesh)
                return &obj->mesh_array[mesh_index];
        }
        return 0;
    }
    return 0;
}

// Added
inline obj_mesh* get_mesh_modern(uint32_t hash, const char* mesh_name) {
    for (auto& i : objset_info_storage_data) {
        obj_set* set = i.second.obj_set;
        if (!set)
            continue;

        for (int32_t j = 0; j < set->obj_num; j++)
            if (set->obj_data[j]->hash == hash)
                return set->obj_data[j]->get_obj_mesh(mesh_name);
    }

    for (auto& i : objset_info_storage_data_modern) {
        obj_set* set = i.second.obj_set;
        if (!set)
            continue;

        for (int32_t j = 0; j < set->obj_num; j++)
            if (set->obj_data[j]->hash == hash)
                return set->obj_data[j]->get_obj_mesh(mesh_name);
    }
    return 0;
}

// Added
inline obj_mesh* get_mesh_modern(uint32_t hash, int32_t mesh_index) {
    for (auto& i : objset_info_storage_data) {
        obj_set* set = i.second.obj_set;
        if (!set)
            continue;

        for (int32_t j = 0; j < set->obj_num; j++) {
            if (set->obj_data[j]->hash != hash)
                continue;

            obj* obj = set->obj_data[j];
            if (mesh_index >= 0 && mesh_index < obj->num_mesh)
                return &obj->mesh_array[mesh_index];
            return 0;
        }
    }

    for (auto& i : objset_info_storage_data_modern) {
        obj_set* set = i.second.obj_set;
        if (!set)
            continue;

        for (int32_t j = 0; j < set->obj_num; j++) {
            if (set->obj_data[j]->hash != hash)
                continue;

            obj* obj = set->obj_data[j];
            if (mesh_index >= 0 && mesh_index < obj->num_mesh)
                return &obj->mesh_array[mesh_index];
            return 0;
        }
    }
    return 0;
}

// 0x140459DE0
inline int32_t get_mesh_index(object_info obj_id, const char* mesh_name) {
    auto elem = objset_info_storage_data.find(obj_id.set_id);
    if (elem != objset_info_storage_data.end()) {
        obj_set* set = elem->second.obj_set;
        if (!set)
            return -1;

        auto elem_obj = elem->second.objdb_map.find(obj_id.id);
        if (elem_obj != elem->second.objdb_map.end())
            return set->obj_data[elem_obj->second]->get_obj_mesh_index(mesh_name);
        return 0;
    }

    auto elem_modern = objset_info_storage_data_modern.find(obj_id.set_id);
    if (elem_modern != objset_info_storage_data_modern.end()) {
        obj_set* set = elem_modern->second.obj_set;
        if (!set)
            return -1;

        auto elem_obj = elem_modern->second.objdb_map.find(obj_id.id);
        if (elem_obj != elem_modern->second.objdb_map.end())
            return set->obj_data[elem_obj->second]->get_obj_mesh_index(mesh_name);
        return 0;
    }
    return -1;
}

// Added
inline int32_t get_mesh_index_modern(uint32_t hash, const char* mesh_name) {
    for (auto& i : objset_info_storage_data) {
        obj_set* set = i.second.obj_set;
        if (!set)
            continue;

        for (int32_t j = 0; j < set->obj_num; j++)
            if (set->obj_data[j]->hash == hash)
                return set->obj_data[j]->get_obj_mesh_index(mesh_name);
    }

    for (auto& i : objset_info_storage_data_modern) {
        obj_set* set = i.second.obj_set;
        if (!set)
            continue;

        for (int32_t j = 0; j < set->obj_num; j++)
            if (set->obj_data[j]->hash == hash)
                return set->obj_data[j]->get_obj_mesh_index(mesh_name);
    }
    return -1;
}

// 0x140459E10 
inline VertexBuffer* get_mesh_vertex_buffer(object_info obj_id, const char* mesh_name) {
    int32_t mesh_index = get_mesh_index(obj_id, mesh_name);
    if (mesh_index == -1)
        return 0;

    ObjsetInfo* info = get_objset_info(obj_id.set_id);
    if (!info)
        return 0;

    auto elem_obj = info->objdb_map.find(obj_id.id);
    if (elem_obj != info->objdb_map.end())
        return &info->objvb[elem_obj->second].vbhn_array[mesh_index];
    return 0;
}

// 0x14045AFA0
inline uint32_t get_texnum_idx2uid(uint32_t objset_index, int32_t tex_index) {
    ObjsetInfo* info = get_objset_info(objset_index);
    if (!info)
        return -1;

    obj_set* set = info->obj_set;
    if (set && tex_index >= 0 && tex_index < set->tex_id_num)
        return set->tex_id_data[tex_index];
    return -1;
}

// 0x14045C6A0
int32_t request_objset(void* data, const object_database* obj_db, uint32_t objset_index) {
    const object_set_info* set_info = obj_db->get_object_set_info(objset_index);
    if (!set_info)
        return 1;

    ObjsetInfo* info = get_objset_info(objset_index);
    if (!info)
        return 1;

    if (info->req_cnt > 0) {
        info->req_cnt++;
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
            archive_file_name.c_str(), object_file_name.c_str(), prj::MemCSystem, false);
        info->tex_file_handler.read_file(data, "rom/objset/",
            archive_file_name.c_str(), texture_file_name.c_str(), prj::MemCSystem, false);
    }
    else {
        info->obj_file_handler.read_file(data,
            "rom/objset/", object_file_name.c_str(), prj::MemCSystem);
        info->tex_file_handler.read_file(data,
            "rom/objset/", texture_file_name.c_str(), prj::MemCSystem);
    }

    info->req_cnt = 1;
    info->obj_ready = false;
    info->tex_ready = false;
    return 0;
}

// Added
int32_t request_objset(void* data, const object_database* obj_db, const char* name) {
    const object_set_info* set_info = obj_db->get_object_set_info(name);
    if (!set_info)
        return 1;

    ObjsetInfo* info = get_objset_info(set_info->id);
    if (!info)
        return 1;

    if (info->req_cnt > 0) {
        info->req_cnt++;
        return 1;
    }

    const std::string& archive_file_name = set_info->archive_file_name;
    const std::string& object_file_name = set_info->object_file_name;
    const std::string& texture_file_name = set_info->texture_file_name;
    if (!object_file_name.size() || !texture_file_name.size())
        return 1;

    if (archive_file_name.size()) {
        info->obj_file_handler.read_file(data, "rom/objset/",
            archive_file_name.c_str(), object_file_name.c_str(), prj::MemCSystem, false);
        info->tex_file_handler.read_file(data, "rom/objset/",
            archive_file_name.c_str(), texture_file_name.c_str(), prj::MemCSystem, false);
    }
    else {
        info->obj_file_handler.read_file(data,
            "rom/objset/", object_file_name.c_str(), prj::MemCSystem);
        info->tex_file_handler.read_file(data,
            "rom/objset/", texture_file_name.c_str(), prj::MemCSystem);
    }

    info->req_cnt = 1;
    info->obj_ready = false;
    info->tex_ready = false;
    return 0;
}

// Added
int32_t request_objset_modern(void* data, uint32_t hash) {
    if (!hash || hash == hash_murmurhash_empty)
        return 1;

    std::string file;
    if (!((data_struct*)data)->get_file("root+/objset/", hash, ".farc", file))
        return 1;

    ObjsetInfo* info = get_objset_info(hash);
    if (!info) {
        info = &objset_info_storage_data_modern.insert({ hash, {} }).first->second;
        info->id = hash;
    }

    if (info->req_cnt > 0) {
        info->req_cnt++;
        return 1;
    }

    info->modern = true;

    info->farc_file_handler.read_file(data, "root+/objset/", file.c_str(), prj::MemCSystem);

    info->req_cnt = 1;
    info->obj_ready = false;
    info->tex_ready = false;
    return 0;
}

// 0x14045DA60
bool wait_objset(uint32_t objset_index, object_database* obj_db, texture_database* tex_db) {
    ObjsetInfo* info = get_objset_info(objset_index);
    if (!info)
        return true;

    if (!info->modern) {
        if (!info->obj_ready && !info->obj_file_handler.check_not_ready()) {
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
            make_objdata_indirect_table(info);

            if (!create_objset_vertex_buffer(info)
                || !create_objset_index_buffer(info))
                return false;

            create_objset_aabb(info);
            load_objset_vertex_array(info);
            info->obj_ready = true;
        }

        if (!info->obj_ready)
            return true;

        if (!info->tex_ready && !info->tex_file_handler.check_not_ready()) {
            if (!info->tex_file_handler.get_data()
                || load_objset_texture(info, info->tex_file_handler.get_data()))
                return false;

            convert_objdata_tex_index(info);
            info->tex_file_handler.reset();
            info->tex_ready = true;
        }
    }
    else if (!info->obj_ready && !info->farc_file_handler.check_not_ready()) {
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
                if (m.id == info->id) {
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

        object_material_msgpack_read("patch\\AFT\\objset", file.c_str(), set);

        info->obj_file_handler.reset();
        make_objdata_indirect_table(info);

        if (!create_objset_vertex_buffer(info)
            || !create_objset_index_buffer(info))
            return false;

        create_objset_aabb(info);
        load_objset_vertex_array(info);
        info->obj_ready = true;

        if (load_objset_texture_modern(info, txd->data, txd->size, file.c_str(), tex_db))
            return false;

        convert_objdata_tex_index(info);
        info->tex_ready = true;
        info->farc_file_handler.reset();
    }

    if (info->obj_ready && info->tex_ready)
        return false;
    return true;
}

// 0x1404599B0
inline void free_objset(uint32_t objset_index) {
    ObjsetInfo* info = get_objset_info(objset_index);
    if (!info || info->req_cnt <= 0)
        return;

    if (--info->req_cnt > 0)
        return;

    free_objdata_indirect_table(info);
    free_objset_texture(info);
    free_objset_index_buffer(info);
    free_objset_vertex_buffer(info);

    info->req_cnt = 0;
    info->tex_ready = false;
    info->obj_ready = false;
    info->alloc_handler.reset();
    info->obj_set = 0;
    info->tex_file_handler.reset();
    info->obj_file_handler.reset();
    info->farc_file_handler.reset();
    if (info->modern)
        objset_info_storage_data_modern.erase(objset_index);
}

// Added
inline void free_objset(const object_database* obj_db, const char* name) {
    const object_set_info* set_info = obj_db->get_object_set_info(name);
    if (!set_info)
        return;

    ObjsetInfo* info = get_objset_info(set_info->id);
    if (!info || info->req_cnt <= 0)
        return;

    if (--info->req_cnt > 0)
        return;

    free_objdata_indirect_table(info);
    free_objset_texture(info);
    free_objset_index_buffer(info);
    free_objset_vertex_buffer(info);

    info->req_cnt = 0;
    info->tex_ready = false;
    info->obj_ready = false;
    info->alloc_handler.reset();
    info->obj_set = 0;
    info->tex_file_handler.reset();
    info->obj_file_handler.reset();
    info->farc_file_handler.reset();
}

// Added
static int32_t align_val_32(int32_t value) {
    return value + (32 - value % 32) % 32;
}

// 0x140457260
static void calc_aabb(obj_axis_aligned_bounding_box& aabb, obj_mesh& mesh, obj_sub_mesh& sub_mesh) {
    vec3 _min = 9999999.0f;
    vec3 _max = -100000000.0f;

    uint32_t* index = sub_mesh.index_array;
    int32_t num_index = sub_mesh.num_index;
    obj_vertex_data* vertex_array = mesh.vertex_array;
    if (sub_mesh.index_format == OBJ_INDEX_U16)
        for (int32_t i = 0; i < num_index; i++, index++) {
            if (*index == 0xFFFFFFFF)
                continue;

            vec3 pos = vertex_array[*index].position;
            _min = vec3::min(_min, pos);
            _max = vec3::max(_max, pos);
        }
    else
        for (int32_t l = 0; l < num_index; l++, index++) {
            vec3 pos = vertex_array[*index].position;
            _min = vec3::min(_min, pos);
            _max = vec3::max(_max, pos);
        }

    vec3 center = (_max + _min) * 0.5f;
    vec3 size = _max - center;
    aabb.center = center;
    aabb.size = size;
}

#if SHARED_OBJECT_BUFFER
// Added
inline static void calc_index_buffer(obj_sub_mesh& sub_mesh, int32_t& size) {
    if (sub_mesh.index_format == OBJ_INDEX_U16) {
        int32_t _size = sizeof(uint16_t) * sub_mesh.num_index;
        size += align_val_32(_size);
    }
}

// Added
inline static void calc_mesh_index_buffer(obj_mesh& mesh, int32_t& size) {
    for (int32_t i = 0; i < mesh.num_submesh; i++)
        calc_index_buffer(mesh.submesh_array[i], size);
}

// Added
inline static int32_t calc_mesh_vertex_buffer(obj_mesh& mesh) {
    uint32_t vertex_size = get_vertex_size(mesh.vertex_format);
    return mesh.num_vertex * vertex_size;
}

// Added
inline static void calc_object_index_buffer(obj* obj, int32_t& size) {
    for (int32_t i = 0; i < obj->num_mesh; i++)
        calc_mesh_index_buffer(obj->mesh_array[i], size);
}

// Added
inline static void calc_object_vertex_buffer(obj* obj, int32_t& size) {
    for (int32_t i = 0; i < obj->num_mesh; i++) {
        int32_t _size = calc_mesh_vertex_buffer(obj->mesh_array[i]);
        size += align_val_32(_size);
    }
}
#endif

// 0x140457E40
static void convert_objdata_tex_index(ObjsetInfo* info) {
    obj_set* set = info->obj_set;
    for (int32_t i = 0; i < set->obj_num; i++)
        convert_objdata_tex_index_internal(info, set->obj_data[i]);
}

// 0x140457EA0
static void convert_objdata_tex_index_internal(ObjsetInfo* info, obj* obj) {
    int32_t num_material = obj->num_material;
    for (int32_t i = 0; i < num_material; i++) {
        obj_material_data& material_data = obj->material_array[i];
        obj_material& material = material_data.material;

        if (*(int32_t*)&material.shader.name[4] != 0xDEADFF) {
            material.shader.index = shaders_ft.get_index_by_name(material.shader.name);
            *(int32_t*)&material.shader.name[4] = 0xDEADFF;
        }

        for (obj_material_texture_data& j : material.texdata) {
            if (j.tex_index == -1)
                continue;

            obj_material_texture_data& texture = j;
            uint32_t tex_index = texture.tex_index;
            texture.tex_index = -1;
            texture.texture_index = 0;

            auto elem = info->texidx_map.find(tex_index);
            if (elem != info->texidx_map.end()) {
                texture.tex_index = tex_index;
                texture.texture_index = elem->second;
            }
        }
    }
}

// 0x140457FC0
static void create_mesh_aabb(obj_mesh& mesh) {
    for (int32_t i = 0; i < mesh.num_submesh; i++)
        calc_aabb(mesh.submesh_array[i].axis_aligned_bounding_box, mesh, mesh.submesh_array[i]);
}

// 0x140458690
static void create_object_aabb(obj* obj) {
    for (int32_t i = 0; i < obj->num_mesh; i++)
        create_mesh_aabb(obj->mesh_array[i]);
}

// 0x1404586E0
static bool create_object_index_buffer(ObjIB* objib, obj* obj) {
    if (!obj)
        return false;

    objib->num_ib = obj->num_mesh;
    objib->ibhn_array = prj::MemoryManager::alloc<IndexBuffer>(prj::MemCSystem, obj->num_mesh, "MESHIB");
    if (!objib->ibhn_array)
        return false;

    for (int32_t i = 0; i < objib->num_ib; i++)
        if (!create_mesh_index_buffer(objib->ibhn_array[i], obj->mesh_array[i]))
            return false;
    return true;
}

// 0x140458790
static bool create_object_vertex_buffer(ObjVB* objvb, obj* obj) {
    if (!obj)
        return false;

    objvb->num_vb = obj->num_mesh;
    objvb->vbhn_array = new VertexBuffer[obj->num_mesh];
    if (!objvb->vbhn_array)
        return false;

    for (int32_t i = 0; i < objvb->num_vb; i++)
        if (!create_mesh_vertex_buffer(objvb->vbhn_array[i], obj->mesh_array[i]))
            return false;
    return true;
}

#if SHARED_OBJECT_BUFFER
// Added
inline static bool create_object_vertex_buffer(ObjVB* objvb, obj* obj, GLuint in_vb, uint32_t& offset) {
    if (!obj)
        return false;

    objvb->num_vb = obj->num_mesh;
    objvb->vbhn_array = new VertexBuffer[obj->num_mesh];
    if (!objvb->vbhn_array)
        return false;

    for (int32_t i = 0; i < obj->num_mesh; i++)
        create_mesh_vertex_buffer(objvb->vbhn_array[i], obj->mesh_array[i], in_vb, offset);
    return true;
}
#endif

// 0x1404588A0
static void create_objset_aabb(ObjsetInfo* info) {
    obj_set* set = info->obj_set;
    for (int32_t i = 0; i < set->obj_num; i++)
        create_object_aabb(set->obj_data[i]);
}

// Added
inline static void fill_index_buffer(void* buf, obj_sub_mesh& sub_mesh, int32_t& offset) {
    if (sub_mesh.primitive_type == OBJ_PRIMITIVE_TRIANGLE_STRIP && !sub_mesh.attrib.m.use_restart_index) {
        uint32_t* index_array = prj::MemoryManager::alloc<uint32_t>(prj::MemCTemp, sub_mesh.num_index, "NEW INDICES");
        sub_mesh.num_index = rewrite_to_restart_index(
            index_array, sub_mesh.num_index, sub_mesh.index_array);
        memmove(sub_mesh.index_array, index_array, sizeof(uint32_t) * sub_mesh.num_index);
        prj::MemoryManager::free(prj::MemCTemp, index_array);
    }

    sub_mesh.min_index = 0;
    sub_mesh.max_index = 0;
    sub_mesh.index_offset = 0;
    if (sub_mesh.index_format != OBJ_INDEX_U16)
        return;

    const uint32_t* index = sub_mesh.index_array;
    uint16_t* indices = (uint16_t*)buf;
    for (int32_t j = sub_mesh.num_index; j > 0; j--, index++)
        *indices++ = (uint16_t)*index;

    uint16_t min_index = 0xFFFF;
    uint16_t max_index = 0;
    indices = (uint16_t*)buf;
    for (int32_t j = sub_mesh.num_index; j > 0; j--) {
        uint16_t index = *indices++;
        if (index == 0xFFFF)
            continue;

        if (min_index > index)
            min_index = index;
        if (max_index < index)
            max_index = index;
    }

    sub_mesh.min_index = min_index;
    sub_mesh.max_index = max_index;
    sub_mesh.index_offset = offset;

    int32_t size = sizeof(uint16_t) * sub_mesh.num_index;
    offset += align_val_32(size);
}

// Added
static void fill_mesh_index_buffer(void* buf, obj_mesh& mesh, int32_t& offset) {
    for (int32_t i = 0; i < mesh.num_submesh; i++)
        fill_index_buffer((uint8_t*)buf + offset, mesh.submesh_array[i], offset);
}

// Added
static int32_t fill_mesh_vertex_buffer(void* buf, obj_mesh& mesh) {
    uint32_t size_vertex = get_vertex_size(mesh.vertex_format, mesh.attrib.m.compression);
    obj_vertex_format vertex_format = mesh.vertex_format;
    obj_vertex_data* vtx = mesh.vertex_array;
    int32_t num_vertex = mesh.num_vertex;
    size_t d = (size_t)buf;
    switch (mesh.attrib.m.compression) {
    case 0:
    default:
        for (int32_t i = num_vertex; i > 0; i--, vtx++) {
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
        for (int32_t i = num_vertex; i > 0; i--, vtx++) {
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
        for (int32_t i = num_vertex; i > 0; i--, vtx++) {
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
    return size_vertex * num_vertex;
}

#if SHARED_OBJECT_BUFFER
// Added
inline static void fill_object_index_buffer(void* buf, obj* obj, int32_t& offset) {
    for (int32_t i = 0; i < obj->num_mesh; i++)
        fill_mesh_index_buffer(buf, obj->mesh_array[i], offset);
}

// Added
inline static void fill_object_vertex_buffer(void* buf, obj* obj, int32_t& offset) {
    for (int32_t i = 0; i < obj->num_mesh; i++) {
        int32_t size = fill_mesh_vertex_buffer((uint8_t*)buf + offset, obj->mesh_array[i]);
        offset += align_val_32(size);
    }
}
#endif

// 0x140459940
static void free_objdata_indirect_table(ObjsetInfo* info) {
    info->objdb_map.clear();
    info->texidx_map.clear();
    info->gentex_vec.clear();
}

// Inlined
inline static void free_object_index_buffer(ObjIB* objib) {
    if (objib->ibhn_array) {
#if SHARED_OBJECT_BUFFER
        for (int32_t i = 0; i < objib->num_ib; i++)
            objib->ibhn_array[i].destroy_shared();
#else
        for (int32_t i = 0; i < objib->num_ib; i++)
            objib->ibhn_array[i].destroy();
#endif
        prj::MemoryManager::free(prj::MemCSystem, objib->ibhn_array);
    }
    objib->ibhn_array = 0;
    objib->num_ib = 0;
}

// Inlined
inline static void free_object_vertex_buffer(ObjVB* objvb) {
    if (objvb->vbhn_array) {
#if SHARED_OBJECT_BUFFER
        for (int32_t i = 0; i < objvb->num_vb; i++)
            objvb->vbhn_array[i].destroy_shared();
#else
        for (int32_t i = 0; i < num_vb; i++)
            vbhn_array[i].destroy();
#endif
        delete[] objvb->vbhn_array;
    }
    objvb->vbhn_array = 0;
    objvb->num_vb = 0;
}

// 0x140459C40
static void free_objset_texture(ObjsetInfo* info) {
    texture_array_free(info->textures);
    info->textures = 0;
    info->tex_num = 0;
}

// 0x14045B4F0
static bool load_objset_texture(ObjsetInfo* info, const void* data, bool big_endian) {
    obj_set* set = info->obj_set;
    if (!set || !data)
        return true;
    else if (!set->tex_id_num)
        return false;

    {
        txp_set txp;
        txp.unpack_file(data, big_endian);
        info->tex_num = (int32_t)txp.textures.size();
        texture_txp_set_load(&txp, &info->textures, set->tex_id_data);
    }

    info->texidx_map.reserve(info->tex_num);
    info->gentex_vec.reserve(info->tex_num);
    uint32_t* tex_id_data = set->tex_id_data;
    int32_t tex_num = info->tex_num;
    texture** textures = info->textures;
    for (int32_t i = 0; i < tex_num; i++) {
        info->texidx_map.push_back(tex_id_data[i], i);
        info->gentex_vec.push_back(textures[i]->glid);
    }
    info->texidx_map.sort();
    return false;
}

// Added
static bool load_objset_texture_modern(ObjsetInfo* info,
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
        texture_txp_set_load(&txp, &info->textures, set->tex_id_data);
    }

    info->texidx_map.reserve(info->tex_num);
    info->gentex_vec.reserve(info->tex_num);
    uint32_t* tex_id_data = set->tex_id_data;
    int32_t tex_num = info->tex_num;
    texture** textures = info->textures;
    for (int32_t i = 0; i < tex_num; i++) {
        info->texidx_map.push_back(tex_id_data[i], i);
        info->gentex_vec.push_back(textures[i]->glid);
    }
    info->texidx_map.sort();
    return false;
}

// Added
static void load_objset_vertex_array(ObjsetInfo* info) {
    obj_set* set = info->obj_set;
    ObjVB* vbhn_array = info->objvb;
    ObjIB* ibhn_array = info->objib;
    for (int32_t i = 0; i < set->obj_num; i++) {
        obj* obj = set->obj_data[i];

#if SHARED_OBJECT_BUFFER
        bool soft_body = false;
        for (int32_t i = 0; i < obj->num_mesh; i++) {
            obj_mesh& mesh = obj->mesh_array[i];
            if (!mesh.num_vertex || !mesh.vertex_array)
                continue;

            soft_body |= !!mesh.attrib.m.soft_body;
        }

        int32_t num_flip = soft_body ? 2 : 1;
#endif

        for (int32_t j = 0; j < obj->num_mesh; j++) {
            obj_mesh* mesh = &obj->mesh_array[j];
            if (!mesh->num_vertex || !mesh->vertex_array)
                continue;

            for (int32_t k = 0; k < mesh->num_submesh; k++) {
                obj_sub_mesh* sub_mesh = &mesh->submesh_array[k];
                if (sub_mesh->attrib.m.hide)
                    continue;

                obj_material_data* material = &obj->material_array[sub_mesh->material_index];

#if !SHARED_OBJECT_BUFFER
                int32_t num_flip = mesh->attrib.m.soft_body ? 2 : 1;
#endif
                for (int32_t l = 0; l < num_flip; l++) {
                    GLuint vb = 0;
                    uint32_t vb_offset = 0;
                    if (vbhn_array && vbhn_array[i].vbhn_array) {
                        vb = vbhn_array[i].vbhn_array[j].get_glvb();
                        vb_offset = vbhn_array[i].vbhn_array[j].get_glvb_offset();
                    }

                    GLuint ib = 0;
                    if (ibhn_array && ibhn_array[i].ibhn_array)
                        ib = ibhn_array[i].ibhn_array[j].ib;

                    extern render_context* rctx_ptr;
                    rctx_ptr->disp_manager->add_vertex_array(mesh, sub_mesh, material,
                        vb, vb_offset, ib, 0, 0);

                    if (vbhn_array && vbhn_array[i].vbhn_array)
                        vbhn_array[i].vbhn_array[j].flip();
                }
            }
        }
    }
}

// 0x14045B8B0
static void make_objdata_indirect_table(ObjsetInfo* info) {
    obj_set* set = info->obj_set;
    if (!set)
        return;

    info->objdb_map.reserve(set->obj_num);
    for (int32_t i = 0; i < set->obj_num; i++)
        info->objdb_map.push_back(set->obj_data[i]->id, i);
    info->objdb_map.sort();
}

// 0x14045D3B0
static int32_t rewrite_to_restart_index(uint32_t* new_indices,
    const int32_t num_indices, const uint32_t* indices) {
    if (!num_indices)
        return 0;

    new_indices[0] = indices[0];

    int32_t src_index = 1;
    int32_t dst_index = 1;
    int32_t strip_length = 1;
    while (src_index < num_indices - 4)
        if (indices[src_index] != indices[src_index + 1]) {
            new_indices[dst_index++] = indices[src_index];
            strip_length++;
            src_index++;
        }
        else if (indices[src_index + 3] == indices[src_index + 4]) {
            new_indices[dst_index++] = indices[src_index];
            new_indices[dst_index++] = 0xFFFFFFFF;
            new_indices[dst_index++] = indices[src_index + 4];

            if (strip_length % 2) {
                new_indices[dst_index++] = indices[src_index + 4];
                strip_length = 0;
            }
            else
                strip_length = 1;
            src_index += 5;
        }
        else if (indices[src_index - 1] != indices[src_index + 2]
            || indices[src_index + 1] != indices[src_index + 4]) {
            new_indices[dst_index++] = indices[src_index];
            new_indices[dst_index++] = 0xFFFFFFFF;
            new_indices[dst_index++] = indices[src_index + 3];

            if (!(strip_length % 2)) {
                new_indices[dst_index++] = indices[src_index + 3];
                strip_length = 0;
            }
            else
                strip_length = 1;
            src_index += 4;
        }
        else {
            new_indices[dst_index++] = indices[src_index];
            strip_length++;
            src_index += 5;
        }

    if (src_index < num_indices) {
        indices += src_index;
        new_indices += dst_index;
        dst_index += num_indices - src_index;
        while (src_index++ < num_indices)
            *new_indices++ = *indices++;
    }
    return dst_index;
}
