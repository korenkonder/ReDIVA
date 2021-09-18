/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "obj.h"
#include "f2_struct.h"
#include "half_t.h"
#include "hash.h"
#include "io_path.h"
#include "io_stream.h"
#include "str_utils.h"

typedef enum obj_vertex_attrib_type {
    OBJ_VERTEX_ATTRIB_POSITION       = 0x00000001,
    OBJ_VERTEX_ATTRIB_NORMAL         = 0x00000002,
    OBJ_VERTEX_ATTRIB_TANGENT        = 0x00000004,
    OBJ_VERTEX_ATTRIB_BINORMAL       = 0x00000008,
    OBJ_VERTEX_ATTRIB_TEXCOORD0      = 0x00000010,
    OBJ_VERTEX_ATTRIB_TEXCOORD1      = 0x00000020,
    OBJ_VERTEX_ATTRIB_TEXCOORD2      = 0x00000040,
    OBJ_VERTEX_ATTRIB_TEXCOORD3      = 0x00000080,
    OBJ_VERTEX_ATTRIB_COLOR0         = 0x00000100,
    OBJ_VERTEX_ATTRIB_COLOR1         = 0x00000200,
    OBJ_VERTEX_ATTRIB_BONE_WEIGHT    = 0x00000400,
    OBJ_VERTEX_ATTRIB_BONE_INDEX     = 0x00000800,
    OBJ_VERTEX_ATTRIB_UNKNOWN        = 0x00001000,
    OBJ_VERTEX_ATTRIB_MODERN_STORAGE = 0x80000000,
} obj_vertex_attrib_type;

typedef struct obj_skin_block_header {
    ssize_t block_signature_offset;
    ssize_t block_offset;
} obj_skin_block_header;

typedef struct obj_skin_ex_data_header {
    int32_t osage_count;
    int32_t osage_nodes_count;
    ssize_t osage_nodes_offset;
    ssize_t osage_names_offset;
    ssize_t blocks_offset;
    int32_t strings_count;
    ssize_t strings_offset;
    ssize_t osage_sibling_infos_offset;
    int32_t cloth_count;
} obj_skin_ex_data_header;

typedef struct obj_skin_header {
    ssize_t bone_ids_offset;
    ssize_t bone_matrices_offset;
    ssize_t bone_names_offset;
    ssize_t ex_data_offset;
    int32_t bone_count;
    ssize_t bone_parent_ids_offset;
} obj_skin_header;

typedef struct obj_sub_mesh_header {
    int32_t bone_index_count;
    ssize_t bone_indices_offset;
    int32_t indices_count;
    ssize_t indices_offset;
} obj_sub_mesh_header;

typedef struct obj_mesh_header {
    int32_t sub_meshes_count;
    ssize_t sub_meshes_offset;
    obj_vertex_attrib_type attrib_type;
    int32_t vertex_size;
    int32_t vertex_count;
    ssize_t attrib_offsets[20];
    uint32_t attrib_flags;
} obj_mesh_header;

typedef struct obj_header {
    int32_t meshes_count;
    ssize_t meshes_offset;
    int32_t materials_count;
    ssize_t materials_offset;
} obj_header;

typedef struct obj_set_header {
    int32_t objects_count;
    int32_t max_object_id;
    ssize_t objects_offset;
    ssize_t object_skins_offset;
    ssize_t object_names_offset;
    ssize_t object_ids_offset;
    ssize_t texture_ids_offset;
    int32_t texture_ids_count;
} obj_set_header;

const char obj_material_texture_enrs_table_bin[] =
"\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x40\xBA\x44\xB0\x01\x10\x02\x18\x01\x10\x01\x10\x01\x10\x01"
"\x18\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01"
"\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01"
"\x10\x01\x50\x20\x01\x10\x01\x10\x01\x18\x01\x10\x01\x10\x01\x10"
"\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10"
"\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x50\x20\x01\x10\x01"
"\x10\x01\x18\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01"
"\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01"
"\x10\x01\x10\x01\x50\x20\x01\x10\x01\x10\x01\x18\x01\x10\x01\x10"
"\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10"
"\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x50\x20\x01"
"\x10\x01\x10\x01\x18\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01"
"\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01"
"\x10\x01\x10\x01\x10\x01\x50\x20\x01\x10\x01\x10\x01\x18\x01\x10"
"\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10"
"\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x50"
"\x20\x01\x10\x01\x10\x01\x18\x01\x10\x01\x10\x01\x10\x01\x10\x01"
"\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01"
"\x10\x01\x10\x01\x10\x01\x10\x01\x50\x20\x01\x10\x01\x10\x01\x18"
"\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10"
"\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10"
"\x01\x50\x20\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01"
"\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01"
"\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01\x10\x01"
"\x50\x40\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

static vector_enrs_entry obj_material_texture_enrs_table;
static bool obj_material_texture_enrs_table_initialized;

vector_func(obj_material_parent)
vector_func(obj_sub_mesh)
vector_func(obj_mesh)
vector_func(obj_skin_block_cloth_field_1C)
vector_func(obj_skin_block_cloth_field_20)
vector_func(obj_skin_motion_node)
vector_func(obj_skin_osage_node)
vector_func(obj_skin_block)
vector_func(obj_skin_bone)
vector_func(obj)

static void obj_material_texture_enrs_table_init();
static void obj_material_texture_enrs_table_free(void);
static void obj_classic_read_inner(obj_set* os, stream* s);
static void obj_classic_write_inner(obj_set* os, stream* s);
static void obj_classic_read_index(obj* o, stream* s,
    obj_sub_mesh* sub_mesh, int32_t indices_count);
static void obj_classic_write_index(obj* o, stream* s,
    obj_sub_mesh* sub_mesh, int32_t* indices_count);
static void obj_classic_read_model(obj* o, stream* s, ssize_t base_offset);
static void obj_classic_write_model(obj* o, stream* s, ssize_t base_offset);
static void obj_classic_read_skin(obj* o, stream* s, ssize_t base_offset);
static void obj_classic_write_skin(obj* o, stream* s, ssize_t base_offset);
static void obj_classic_read_skin_block_cloth(obj_skin_block_cloth* b,
    stream* s, string* str);
static void obj_classic_write_skin_block_cloth(obj_skin_block_cloth* b,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets,
    ssize_t* field_18_offset, ssize_t* field_1C_offset, ssize_t* field_20_offset,
    ssize_t* field_24_offset, ssize_t* field_28_offset);
static void obj_classic_read_skin_block_cloth_field_1C_sub(obj_skin_block_cloth_field_1C_sub* sub,
    stream* s, string* str);
static void obj_classic_write_skin_block_cloth_field_1C_sub(obj_skin_block_cloth_field_1C_sub* sub,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets);
static void obj_classic_read_skin_block_cloth_field_20_sub(obj_skin_block_cloth_field_20_sub* sub,
    stream* s, string* str);
static void obj_classic_write_skin_block_cloth_field_20_sub(obj_skin_block_cloth_field_20_sub* sub,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets);
static void obj_classic_read_skin_block_constraint(obj_skin_block_constraint* b,
    stream* s, string* str);
static void obj_classic_write_skin_block_constraint(obj_skin_block_constraint* b,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets, ssize_t* offsets);
static void obj_classic_read_skin_block_constraint_attach_point(obj_skin_block_constraint_attach_point* ap,
    stream* s, string* str);
static void obj_classic_write_skin_block_constraint_attach_point(obj_skin_block_constraint_attach_point* ap,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets);
static void obj_classic_read_skin_block_constraint_up_vector(obj_skin_block_constraint_up_vector* up,
    stream* s, string* str);
static void obj_classic_write_skin_block_constraint_up_vector(obj_skin_block_constraint_up_vector* up,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets);
static void obj_classic_read_skin_block_expression(obj_skin_block_expression* b,
    stream* s, string* str);
static void obj_classic_write_skin_block_expression(obj_skin_block_expression* b,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets);
static void obj_classic_read_skin_block_motion(obj_skin_block_motion* b,
    stream* s, string* str);
static void obj_classic_write_skin_block_motion(obj_skin_block_motion* b,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets,
    vector_string* string_set, ssize_t* bone_names_offset, ssize_t* bone_matrices_offset);
static void obj_classic_read_skin_block_node(obj_skin_block_node* b,
    stream* s, string* str);
static void obj_classic_write_skin_block_node(obj_skin_block_node* b,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets);
static void obj_classic_read_skin_block_osage(obj_skin_block_osage* b,
    stream* s, string* str);
static void obj_classic_write_skin_block_osage(obj_skin_block_osage* b,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets,
    vector_string* string_set, ssize_t* nodes_offset);
static void obj_classic_read_vertex(obj* o, stream* s, ssize_t* attrib_offsets, obj_mesh* mesh,
    ssize_t base_offset, int32_t vertex_count, obj_vertex_attrib_type attrib_type);
static void obj_classic_write_vertex(obj* o, stream* s, ssize_t* attrib_offsets, obj_mesh* mesh,
    ssize_t base_offset, int32_t* vertex_count, obj_vertex_attrib_type* attrib_type, int32_t* vertex_size);
static void obj_modern_read_inner(obj_set* os, stream* s);
static void obj_modern_write_inner(obj_set* os, stream* s);
static void obj_modern_read_index(obj* o, stream* s,
    obj_sub_mesh* sub_mesh, int32_t indices_count);
static void obj_modern_write_index(obj* o, stream* s, bool is_x,
    obj_sub_mesh* sub_mesh, int32_t* indices_count, f2_struct* ovtx);
static void obj_modern_read_model(obj* o, stream* s, ssize_t base_offset,
    uint32_t header_length, bool is_x, stream* s_oidx, stream* s_ovtx);
static void obj_modern_write_model(obj* o, stream* s,
    ssize_t base_offset, bool is_x, f2_struct* omdl);
static void obj_modern_read_skin(obj* o, stream* s, ssize_t base_offset,
    uint32_t header_length, bool is_x);
static void obj_modern_write_skin(obj* o, stream* s,
    ssize_t base_offset, bool is_x, f2_struct* oskn);
static void obj_modern_read_skin_block_cloth(obj_skin_block_cloth* b,
    stream* s, uint32_t header_length, string* str, bool is_x);
static void obj_modern_write_skin_block_cloth(obj_skin_block_cloth* b,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets, bool is_x,
    ssize_t* field_18_offset, ssize_t* field_1C_offset, ssize_t* field_20_offset,
    ssize_t* field_24_offset, ssize_t* field_28_offset);
static void obj_modern_read_skin_block_cloth_field_1C_sub(obj_skin_block_cloth_field_1C_sub* sub,
    stream* s, uint32_t header_length, string* str, bool is_x);
static void obj_modern_write_skin_block_cloth_field_1C_sub(obj_skin_block_cloth_field_1C_sub* sub,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets, bool is_x);
static void obj_modern_read_skin_block_cloth_field_20_sub(obj_skin_block_cloth_field_20_sub* sub,
    stream* s, uint32_t header_length, string* str, bool is_x);
static void obj_modern_write_skin_block_cloth_field_20_sub(obj_skin_block_cloth_field_20_sub* sub,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets, bool is_x);
static void obj_modern_read_skin_block_constraint(obj_skin_block_constraint* b,
    stream* s, uint32_t header_length, string* str, bool is_x);
static void obj_modern_write_skin_block_constraint(obj_skin_block_constraint* b,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets, bool is_x, ssize_t* offsets);
static void obj_modern_read_skin_block_constraint_attach_point(obj_skin_block_constraint_attach_point* ap,
    stream* s, uint32_t header_length, string* str, bool is_x);
static void obj_modern_write_skin_block_constraint_attach_point(obj_skin_block_constraint_attach_point* ap,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets, bool is_x);
static void obj_modern_read_skin_block_constraint_up_vector(obj_skin_block_constraint_up_vector* up,
    stream* s, uint32_t header_length, string* str, bool is_x);
static void obj_modern_write_skin_block_constraint_up_vector(obj_skin_block_constraint_up_vector* up,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets, bool is_x);
static void obj_modern_read_skin_block_expression(obj_skin_block_expression* b,
    stream* s, uint32_t header_length, string* str, bool is_x);
static void obj_modern_write_skin_block_expression(obj_skin_block_expression* b,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets, bool is_x);
static void obj_modern_read_skin_block_motion(obj_skin_block_motion* b,
    stream* s, uint32_t header_length, string* str, bool is_x);
static void obj_modern_write_skin_block_motion(obj_skin_block_motion* b,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets, bool is_x,
    vector_string* string_set, ssize_t* bone_names_offset, ssize_t* bone_matrices_offset);
static void obj_modern_read_skin_block_node(obj_skin_block_node* b,
    stream* s, uint32_t header_length, string* str, bool is_x);
static void obj_modern_write_skin_block_node(obj_skin_block_node* b,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets, bool is_x);
static void obj_modern_read_skin_block_osage(obj_skin_block_osage* b,
    stream* s, uint32_t header_length, string* str, bool is_x);
static void obj_modern_write_skin_block_osage(obj_skin_block_osage* b,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets, bool is_x,
    vector_string* string_set);
static void obj_modern_read_vertex(obj* o, stream* s, ssize_t* attrib_offsets, obj_mesh* mesh,
    const uint32_t attrib_flags, int32_t vertex_count, int32_t vertex_size);
static void obj_modern_write_vertex(obj* o, stream* s, bool is_x,
    ssize_t* attrib_offsets, obj_mesh* mesh, uint32_t* attrib_flags,
    int32_t* vertex_count, int32_t* vertex_size, f2_struct* ovtx);
static uint32_t obj_skin_strings_get_string_index(vector_string* vec, char* str);
static ssize_t obj_skin_strings_get_string_offset(vector_string* vec,
    vector_ssize_t* vec_off, char* str);
static void obj_skin_strings_push_back_check(vector_string* vec, char* str);

void obj_init(obj_set* os) {
    if (!os)
        return;

    memset(os, 0, sizeof(obj_set));
}

void obj_read(obj_set* os, char* path, bool modern) {
    if (!os || !path)
        return;

    if (!modern) {
        char* path_bin = str_utils_add(path, ".bin");
        if (path_check_file_exists(path_bin)) {
            stream s;
            io_open(&s, path_bin, "rb");
            if (s.io.stream)
                obj_classic_read_inner(os, &s);
            io_free(&s);
        }
        free(path_bin);
    }
    else {
        char* path_obj = str_utils_add(path, ".osd");
        if (path_check_file_exists(path_obj)) {
            stream s;
            io_open(&s, path_obj, "rb");
            if (s.io.stream)
                obj_modern_read_inner(os, &s);
            io_free(&s);
        }
        free(path_obj);
    }
}

void obj_wread(obj_set* os, wchar_t* path, bool modern) {
    if (!os || !path)
        return;

    if (!modern) {
        wchar_t* path_bin = str_utils_wadd(path, L".bin");
        if (path_wcheck_file_exists(path_bin)) {
            stream s;
            io_wopen(&s, path_bin, L"rb");
            if (s.io.stream)
                obj_classic_read_inner(os, &s);
            io_free(&s);
        }
        free(path_bin);
    }
    else {
        wchar_t* path_obj = str_utils_wadd(path, L".osd");
        if (path_wcheck_file_exists(path_obj)) {
            stream s;
            io_wopen(&s, path_obj, L"rb");
            if (s.io.stream)
                obj_modern_read_inner(os, &s);
            io_free(&s);
        }
        free(path_obj);
    }
}

void obj_mread(obj_set* os, void* data, size_t length, bool modern) {
    if (!os || !data)
        return;

    stream s;
    io_mopen(&s, data, length);
    if (!modern)
        obj_classic_read_inner(os, &s);
    else
        obj_modern_read_inner(os, &s);
    io_free(&s);
}

void obj_write(obj_set* os, char* path) {
    if (!os || !path || !os->ready)
        return;

    if (!os->modern) {
        char* path_bin = str_utils_add(path, ".bin");
        stream s;
        io_open(&s, path_bin, "wb");
        if (s.io.stream)
            obj_classic_write_inner(os, &s);
        io_free(&s);
        free(path_bin);
    }
    else {
        char* path_obj = str_utils_add(path, ".osd");
        stream s;
        io_open(&s, path_obj, "wb");
        if (s.io.stream)
            obj_modern_write_inner(os, &s);
        io_free(&s);
        free(path_obj);
    }
}

void obj_wwrite(obj_set* os, wchar_t* path) {
    if (!os || !path || !os->ready)
        return;

    if (!os->modern) {
        wchar_t* path_bin = str_utils_wadd(path, L".bin");
        stream s;
        io_wopen(&s, path_bin, L"wb");
        if (s.io.stream)
            obj_classic_write_inner(os, &s);
        io_free(&s);
        free(path_bin);
    }
    else {
        wchar_t* path_obj = str_utils_wadd(path, L".osd");
        stream s;
        io_wopen(&s, path_obj, L"wb");
        if (s.io.stream)
            obj_modern_write_inner(os, &s);
        io_free(&s);
        free(path_obj);
    }
}

void obj_mwrite(obj_set* os, void** data, size_t* length) {
    if (!os || !data)
        return;

    stream s;
    io_mopen(&s, 0, 0);
    if (!os->modern)
        obj_classic_write_inner(os, &s);
    else
        obj_modern_write_inner(os, &s);

    io_align_write(&s, 0x10);
    io_mcopy(&s, data, length);
    io_free(&s);
}

void obj_free(obj_set* os) {
    if (!os)
        return;

    for (obj* i = os->vec.begin; i != os->vec.end; i++) {
        for (obj_skin_bone* j = i->skin.bones.begin; j != i->skin.bones.end; j++)
            string_free(&j->name);
        vector_obj_skin_bone_free(&i->skin.bones);

        for (obj_skin_block* j = i->skin.ex_data.blocks.begin;
            j != i->skin.ex_data.blocks.end; j++)
            switch (j->type) {
            case OBJ_SKIN_BLOCK_CLOTH: {
                obj_skin_block_cloth* cloth = &j->cloth;
                string_free(&cloth->mesh_name);
                string_free(&cloth->backface_mesh_name);

                for (obj_skin_block_cloth_field_1C* k = cloth->field_1C.begin;
                    k != cloth->field_1C.end; k++) {
                    string_free(&k->sub_data_0.bone_name);
                    string_free(&k->sub_data_1.bone_name);
                    string_free(&k->sub_data_2.bone_name);
                    string_free(&k->sub_data_3.bone_name);
                }
                vector_obj_skin_block_cloth_field_1C_free(&cloth->field_1C);
                vector_obj_skin_block_cloth_field_20_free(&cloth->field_20);
                vector_uint16_t_free(&cloth->field_24);
                vector_uint16_t_free(&cloth->field_28);
            } break;
            case OBJ_SKIN_BLOCK_CONSTRAINT: {
                obj_skin_block_constraint* constraint = &j->constraint;
                string_free(&constraint->base.parent_name);
                string_free(&constraint->base.name);
                string_free(&constraint->source_node_name);
            } break;
            case OBJ_SKIN_BLOCK_EXPRESSION: {
                obj_skin_block_expression* expression = &j->expression;
                for (int32_t k = 0; k < 9; k++)
                    string_free(&expression->expressions[k]);
            } break;
            case OBJ_SKIN_BLOCK_MOTION: {
                obj_skin_block_motion* motion = &j->motion;
                for (obj_skin_motion_node* k = motion->nodes.begin; k != motion->nodes.end; k++)
                    string_free(&k->name);
                vector_obj_skin_motion_node_free(&motion->nodes);
            } break;
            case OBJ_SKIN_BLOCK_OSAGE: {
                obj_skin_block_osage* osage = &j->osage;
                string_free(&osage->base.parent_name);
                string_free(&osage->base.name);
                string_free(&osage->external_name);
                for (obj_skin_osage_node* k = osage->nodes.begin; k != osage->nodes.end; k++) {
                    string_free(&k->name);
                    string_free(&k->sibling_name);
                }
                vector_obj_skin_osage_node_free(&osage->nodes);
            } break;
            }
        vector_obj_skin_block_free(&i->skin.ex_data.blocks);

        for (obj_skin_osage_node* j = i->skin.ex_data.osage_nodes.begin;
            j != i->skin.ex_data.osage_nodes.end; j++) {
            string_free(&j->name);
            string_free(&j->sibling_name);
        }
        vector_obj_skin_osage_node_free(&i->skin.ex_data.osage_nodes);

        for (obj_mesh* j = i->meshes.begin; j != i->meshes.end; j++) {
            for (obj_sub_mesh* k = j->sub_meshes.begin; k != j->sub_meshes.end; k++) {
                free(k->bone_index);
                free(k->indices);
            }
            free(j->vertex);
            vector_obj_sub_mesh_free(&j->sub_meshes);
        }
        vector_obj_mesh_free(&i->meshes);
        vector_obj_material_parent_free(&i->materials);
        string_free(&i->name);
    }
    vector_obj_free(&os->vec);
    vector_uint32_t_free(&os->tex_ids);
}

static void obj_material_texture_enrs_table_init() {
    if (!obj_material_texture_enrs_table_initialized) {
        stream s;
        io_mopen(&s, (void*)&obj_material_texture_enrs_table_bin,
            sizeof(obj_material_texture_enrs_table_bin));
        enrs_read(&s, &obj_material_texture_enrs_table);
        io_free(&s);
        atexit(obj_material_texture_enrs_table_free);
        obj_material_texture_enrs_table_initialized = true;
    }
}

static void obj_material_texture_enrs_table_free(void) {
    enrs_free(&obj_material_texture_enrs_table);
}

static void obj_classic_read_inner(obj_set* os, stream* s) {
    uint32_t signature = io_read_uint32_t(s);
    if (signature != 0x5062500) {
        os->is_x = false;
        os->modern = false;
        os->ready = false;
        return;
    }

    obj_set_header osh;
    memset(&osh, 0, sizeof(obj_set_header));
    osh.objects_count = io_read_int32_t(s);
    osh.max_object_id = io_read_int32_t(s);
    osh.objects_offset = io_read_int32_t(s);
    osh.object_skins_offset = io_read_int32_t(s);
    osh.object_names_offset = io_read_int32_t(s);
    osh.object_ids_offset = io_read_int32_t(s);
    osh.texture_ids_offset = io_read_int32_t(s);
    osh.texture_ids_count = io_read_int32_t(s);

    int32_t count = osh.objects_count;
    os->vec = vector_empty(obj);
    vector_obj_reserve(&os->vec, osh.objects_count);

    int32_t* objects_offsets = 0;
    if (osh.objects_offset) {
        objects_offsets = force_malloc_s(ssize_t, count);
        io_set_position(s, osh.objects_offset, SEEK_SET);
        for (int32_t i = 0; i < count; i++)
            objects_offsets[i] = io_read_int32_t(s);
    }

    int32_t* object_skins_offsets = 0;
    if (osh.object_skins_offset) {
        object_skins_offsets = force_malloc_s(ssize_t, count);
        io_set_position(s, osh.object_skins_offset, SEEK_SET);
        for (int32_t i = 0; i < count; i++)
            object_skins_offsets[i] = io_read_int32_t(s);
    }

    int32_t* object_names_offsets = 0;
    if (osh.object_names_offset) {
        object_names_offsets = force_malloc_s(ssize_t, count);
        io_set_position(s, osh.object_names_offset, SEEK_SET);
        for (int32_t i = 0; i < count; i++)
            object_names_offsets[i] = io_read_int32_t(s);
    }

    if (osh.objects_offset) {
        for (int32_t i = 0; i < count; i++) {
            obj o;
            memset(&o, 0, sizeof(obj));

            if (objects_offsets[i])
                obj_classic_read_model(&o, s, objects_offsets[i]);

            if (osh.object_skins_offset && object_skins_offsets[i])
                obj_classic_read_skin(&o, s, object_skins_offsets[i]);

            if (osh.object_names_offset && object_names_offsets[i])
                io_read_string_null_terminated_offset(s, object_names_offsets[i], &o.name);

            vector_obj_push_back(&os->vec, &o);
        }

        io_set_position(s, osh.object_ids_offset, SEEK_SET);
        for (int32_t i = 0; i < count; i++)
            os->vec.begin[i].id = io_read_uint32_t(s);
    }

    if (osh.texture_ids_offset) {
        io_set_position(s, osh.texture_ids_offset, SEEK_SET);
        int32_t count = osh.texture_ids_count;
        os->tex_ids = vector_empty(uint32_t);
        vector_uint32_t_reserve(&os->tex_ids, count);
        for (int32_t i = 0; i < count; i++) {
            uint32_t tex_id = io_read_uint32_t(s);
            vector_uint32_t_push_back(&os->tex_ids, &tex_id);
        }
    }

    free(objects_offsets);
    free(object_skins_offsets);
    free(object_names_offsets);

    os->is_x = false;
    os->modern = false;
    os->ready = true;
}


static void obj_classic_write_inner(obj_set* os, stream* s) {
    io_set_position(s, 0x00, SEEK_SET);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_align_write(s, 0x10);

    obj_set_header osh;
    memset(&osh, 0, sizeof(obj_set_header));
    osh.objects_count = (int32_t)(os->vec.end - os->vec.begin);
    osh.max_object_id = -1;
    osh.texture_ids_count = (int32_t)(os->tex_ids.end - os->tex_ids.begin);

    int32_t count = osh.objects_count;

    osh.objects_offset = io_get_position(s);
    for (int32_t i = 0; i < count; i++)
        io_write_int32_t(s, 0);
    io_align_write(s, 0x10);

    int32_t* objects_offsets = force_malloc_s(int32_t, count);
    for (int32_t i = 0; i < count; i++) {
        objects_offsets[i] = (int32_t)io_get_position(s);
        obj_classic_write_model(&os->vec.begin[i], s, objects_offsets[i]);
    }
    io_align_write(s, 0x10);

    io_position_push(s, osh.objects_offset, SEEK_SET);
    for (int32_t i = 0; i < count; i++)
        io_write_int32_t(s, (int32_t)objects_offsets[i]);
    io_position_pop(s);
    free(objects_offsets);

    osh.object_ids_offset = io_get_position(s);
    for (int32_t i = 0; i < count; i++) {
        int32_t object_id = os->vec.begin[i].id;
        io_write_uint32_t(s, object_id);
        if (osh.max_object_id < object_id)
            osh.max_object_id = object_id;
    }
    io_align_write(s, 0x10);

    int32_t* object_names_offsets = force_malloc_s(int32_t, count);
    for (int32_t i = 0; i < count; i++) {
        object_names_offsets[i] = (int32_t)io_get_position(s);
        io_write_string_null_terminated(s, &os->vec.begin[i].name);
    }
    io_align_write(s, 0x10);

    osh.object_names_offset = io_get_position(s);
    for (int32_t i = 0; i < count; i++)
        io_write_int32_t(s, object_names_offsets[i]);
    io_align_write(s, 0x10);
    free(object_names_offsets);

    osh.texture_ids_offset = io_get_position(s);
    for (int32_t i = 0; i < osh.texture_ids_count; i++)
        io_write_uint32_t(s, os->tex_ids.begin[i]);
    io_align_write(s, 0x10);

    osh.object_skins_offset = io_get_position(s);
    for (int32_t i = 0; i < count; i++)
        io_write_int32_t(s, 0);
    io_align_write(s, 0x10);

    int32_t* object_skins_offsets = force_malloc_s(int32_t, count);
    for (int32_t i = 0; i < count; i++) {
        if (!os->vec.begin[i].skin_init) {
            object_skins_offsets[i] = 0;
            continue;
        }

        object_skins_offsets[i] = (int32_t)io_get_position(s);
        obj_classic_write_skin(&os->vec.begin[i], s, object_skins_offsets[i]);
    }
    io_align_write(s, 0x10);

    io_position_push(s, osh.object_skins_offset, SEEK_SET);
    for (int32_t i = 0; i < count; i++)
        io_write_int32_t(s, object_skins_offsets[i]);
    io_position_pop(s);
    free(object_skins_offsets);

    io_position_push(s, 0x00, SEEK_SET);
    io_write_uint32_t(s, 0x5062500);
    io_write_int32_t(s, osh.objects_count);
    io_write_uint32_t(s, osh.max_object_id);
    io_write_int32_t(s, (int32_t)osh.objects_offset);
    io_write_int32_t(s, (int32_t)osh.object_skins_offset);
    io_write_int32_t(s, (int32_t)osh.object_names_offset);
    io_write_int32_t(s, (int32_t)osh.object_ids_offset);
    io_write_int32_t(s, (int32_t)osh.texture_ids_offset);
    io_write_int32_t(s, osh.texture_ids_count);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_position_pop(s);
}

static void obj_classic_read_index(obj* o, stream* s,
    obj_sub_mesh* sub_mesh, int32_t indices_count) {
    uint32_t* indices = force_malloc_s(uint32_t, indices_count);
    switch (sub_mesh->index_format) {
    case OBJ_INDEX_U8:
        for (int32_t i = 0; i < indices_count; i++) {
            uint8_t idx = io_read_uint8_t(s);
            indices[i] = idx == 0xFF ? 0xFFFFFFFF : idx;
        }
        break;
    case OBJ_INDEX_U16:
        for (int32_t i = 0; i < indices_count; i++) {
            uint16_t idx = io_read_uint16_t(s);
            indices[i] = idx == 0xFFFF ? 0xFFFFFFFF : idx;
        }
        break;
    case OBJ_INDEX_U32:
        for (int32_t i = 0; i < indices_count; i++)
            indices[i] = io_read_uint32_t(s);
        break;
    }
    sub_mesh->indices = indices;
    sub_mesh->indices_count = indices_count;
}

static void obj_classic_write_index(obj* o, stream* s,
    obj_sub_mesh* sub_mesh, int32_t* indices_count) {
    uint32_t* indices = sub_mesh->indices;
    int32_t _indices_count = sub_mesh->indices_count;
    switch (sub_mesh->index_format) {
    case OBJ_INDEX_U8:
        for (int32_t i = 0; i < _indices_count; i++)
            io_write_uint8_t(s, (uint8_t)indices[i]);
        break;
    case OBJ_INDEX_U16:
        for (int32_t i = 0; i < _indices_count; i++)
            io_write_uint16_t(s, (uint16_t)indices[i]);
        break;
    case OBJ_INDEX_U32:
        for (int32_t i = 0; i < _indices_count; i++)
            io_write_uint32_t(s, indices[i]);
        break;
    }
    io_align_write(s, 0x04);
    *indices_count = _indices_count;
}

static void obj_classic_read_model(obj* o, stream* s, ssize_t base_offset) {
    const size_t mesh_size = 0xD8;
    const size_t sub_mesh_size = 0x5C;

    io_set_position(s, base_offset, SEEK_SET);

    obj_header oh;
    memset(&oh, 0, sizeof(obj_header));
    io_read(s, 0, 0x08);
    o->bounding_sphere.center.x = io_read_float_t(s);
    o->bounding_sphere.center.y = io_read_float_t(s);
    o->bounding_sphere.center.z = io_read_float_t(s);
    o->bounding_sphere.radius = io_read_float_t(s);
    oh.meshes_count = io_read_int32_t(s);
    oh.meshes_offset = io_read_int32_t(s);
    oh.materials_count = io_read_int32_t(s);
    oh.materials_offset = io_read_int32_t(s);
    io_read(s, 0, 0x28);

    if (oh.meshes_offset > 0) {
        o->meshes = vector_empty(obj_mesh);
        vector_obj_mesh_reserve(&o->meshes, oh.meshes_count);
        for (int32_t i = 0; i < oh.meshes_count; i++) {
            obj_mesh mesh;
            memset(&mesh, 0, sizeof(obj_mesh));

            io_set_position(s, base_offset + oh.meshes_offset + mesh_size * i, SEEK_SET);

            obj_mesh_header mh;
            memset(&mh, 0, sizeof(obj_mesh_header));
            io_read(s, 0, 0x04);
            mesh.bounding_sphere.center.x = io_read_float_t(s);
            mesh.bounding_sphere.center.y = io_read_float_t(s);
            mesh.bounding_sphere.center.z = io_read_float_t(s);
            mesh.bounding_sphere.radius = io_read_float_t(s);
            mh.sub_meshes_count = io_read_int32_t(s);
            mh.sub_meshes_offset = io_read_int32_t(s);
            mh.attrib_type = io_read_uint32_t(s);
            mh.vertex_size = io_read_int32_t(s);
            mh.vertex_count = io_read_int32_t(s);

            for (int32_t j = 0; j < 20; j++)
                mh.attrib_offsets[j] = io_read_int32_t(s);

            mesh.flags = io_read_int32_t(s);
            mh.attrib_flags = io_read_uint32_t(s);
            io_read(s, 0, 0x18);
            io_read(s, &mesh.name, sizeof(mesh.name));
            mesh.name[sizeof(mesh.name) - 1] = 0;

            if (mh.sub_meshes_offset) {
                mesh.sub_meshes = vector_empty(obj_sub_mesh);
                vector_obj_sub_mesh_reserve(&mesh.sub_meshes, mh.sub_meshes_count);
                for (int32_t j = 0; j < mh.sub_meshes_count; j++) {
                    obj_sub_mesh sub_mesh;
                    memset(&sub_mesh, 0, sizeof(obj_sub_mesh));

                    io_set_position(s, base_offset + mh.sub_meshes_offset + sub_mesh_size * j, SEEK_SET);

                    obj_sub_mesh_header smh;
                    memset(&smh, 0, sizeof(obj_sub_mesh_header));
                    io_read(s, 0, 0x04);
                    sub_mesh.bounding_sphere.center.x = io_read_float_t(s);
                    sub_mesh.bounding_sphere.center.y = io_read_float_t(s);
                    sub_mesh.bounding_sphere.center.z = io_read_float_t(s);
                    sub_mesh.bounding_sphere.radius = io_read_float_t(s);
                    sub_mesh.material_index = io_read_uint32_t(s);
                    io_read(s, &sub_mesh.texcoord_indices, 0x08);
                    smh.bone_index_count = io_read_int32_t(s);
                    smh.bone_indices_offset = io_read_int32_t(s);
                    sub_mesh.bones_per_vertex = io_read_uint32_t(s);
                    sub_mesh.primitive_type = io_read_uint32_t(s);
                    sub_mesh.index_format = io_read_uint32_t(s);
                    smh.indices_count = io_read_int32_t(s);
                    smh.indices_offset = io_read_int32_t(s);
                    sub_mesh.flags = io_read_uint32_t(s);
                    sub_mesh.bounding_box.center = sub_mesh.bounding_sphere.center;
                    sub_mesh.bounding_box.size.x = sub_mesh.bounding_sphere.radius * 2.0f;
                    sub_mesh.bounding_box.size.y = sub_mesh.bounding_sphere.radius * 2.0f;
                    sub_mesh.bounding_box.size.z = sub_mesh.bounding_sphere.radius * 2.0f;
                    io_read(s, 0, 0x18);
                    sub_mesh.index_offset = io_read_uint32_t(s);

                    if (sub_mesh.bones_per_vertex == 4 && smh.bone_indices_offset) {
                        uint16_t* bone_index = force_malloc_s(uint16_t, smh.bone_index_count);
                        io_set_position(s, base_offset + smh.bone_indices_offset, SEEK_SET);
                        for (int32_t k = 0; k < smh.bone_index_count; k++)
                            bone_index[k] = io_read_uint16_t(s);
                        sub_mesh.bone_index = bone_index;
                        sub_mesh.bone_index_count = smh.bone_index_count;
                    }

                    io_set_position(s, base_offset + smh.indices_offset, SEEK_SET);
                    obj_classic_read_index(o, s, &sub_mesh, smh.indices_count);

                    vector_obj_sub_mesh_push_back(&mesh.sub_meshes, &sub_mesh);
                }
            }

            obj_classic_read_vertex(o, s, mh.attrib_offsets, &mesh,
                base_offset, mh.vertex_count, mh.attrib_type);

            vector_obj_mesh_push_back(&o->meshes, &mesh);
        }
    }

    if (oh.materials_offset > 0) {
        io_set_position(s, base_offset + oh.materials_offset, SEEK_SET);
        o->materials = vector_empty(obj_material_parent);
        vector_obj_material_parent_reserve(&o->materials, oh.materials_count);
        for (int32_t i = 0; i < oh.materials_count; i++) {
            obj_material_parent m;
            io_read(s, &m, sizeof(obj_material_parent));
            vector_obj_material_parent_push_back(&o->materials, &m);
        }
    }
}

static void obj_classic_write_model(obj* o, stream* s, ssize_t base_offset) {
    const size_t mesh_size = 0xD8;
    const size_t sub_mesh_size = 0x5C;

    io_write_uint32_t(s, 0);
    io_write_uint32_t(s, 0);
    io_write_float_t(s, 0);
    io_write_float_t(s, 0);
    io_write_float_t(s, 0);
    io_write_float_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write(s, 0, 0x28);
    io_align_write(s, 0x10);

    obj_header oh;
    memset(&oh, 0, sizeof(obj_header));
    oh.meshes_count = (int32_t)(o->meshes.end - o->meshes.begin);
    if (oh.meshes_count) {
        oh.meshes_offset = io_get_position(s) - base_offset;

        obj_mesh_header* mhs = force_malloc_s(obj_mesh_header, oh.meshes_count);
        for (int32_t i = 0; i < oh.meshes_count; i++) {
            obj_mesh* mesh = &o->meshes.begin[i];

            io_write(s, 0, 0x04);
            io_write_float_t(s, 0.0f);
            io_write_float_t(s, 0.0f);
            io_write_float_t(s, 0.0f);
            io_write_float_t(s, 0.0f);
            io_write_int32_t(s, 0);
            io_write_int32_t(s, 0);
            io_write_uint32_t(s, 0);
            io_write_int32_t(s, 0);
            io_write_int32_t(s, 0);

            for (int32_t j = 0; j < 20; j++)
                io_write_int32_t(s, 0);

            io_write_int32_t(s, 0);
            io_write_uint32_t(s, 0);
            io_write(s, 0, 0x18);
            io_write(s, 0, sizeof(mesh->name));
        }

        for (int32_t i = 0; i < oh.meshes_count; i++) {
            obj_mesh* mesh = &o->meshes.begin[i];
            obj_mesh_header* mh = &mhs[i];

            mh->sub_meshes_count = (int32_t)(mesh->sub_meshes.end - mesh->sub_meshes.begin);
            if (mh->sub_meshes_count) {

                mh->sub_meshes_offset = io_get_position(s) - base_offset;
                for (int32_t j = 0; j < mh->sub_meshes_count; j++) {
                    io_write(s, 0, 0x04);
                    io_write_float_t(s, 0.0f);
                    io_write_float_t(s, 0.0f);
                    io_write_float_t(s, 0.0f);
                    io_write_float_t(s, 0.0f);
                    io_write_uint32_t(s, 0);
                    io_write(s, 0, 0x08);
                    io_write_int32_t(s, 0);
                    io_write_int32_t(s, 0);
                    io_write_uint32_t(s, 0);
                    io_write_uint32_t(s, 0);
                    io_write_uint32_t(s, 0);
                    io_write_int32_t(s, 0);
                    io_write_int32_t(s, 0);
                    io_write_uint32_t(s, 0);
                    io_write(s, 0, 0x18);
                    io_write_uint32_t(s, 0);
                }
            }

            obj_classic_write_vertex(o, s, mh->attrib_offsets,
                mesh, base_offset, &mh->vertex_count, &mh->attrib_type, &mh->vertex_size);

            for (int32_t j = 0; j < mh->sub_meshes_count; j++) {
                obj_sub_mesh* sub_mesh = &mesh->sub_meshes.begin[j];

                obj_sub_mesh_header smh;
                memset(&smh, 0, sizeof(obj_sub_mesh_header));
                if (sub_mesh->bones_per_vertex == 4 && sub_mesh->bone_index_count) {
                    smh.bone_indices_offset = io_get_position(s) - base_offset;

                    smh.bone_index_count = sub_mesh->bone_index_count;
                    uint16_t* bone_index = sub_mesh->bone_index;
                    for (int32_t k = 0; k < smh.bone_index_count; k++)
                        io_write_uint16_t(s, bone_index[k]);
                    io_align_write(s, 0x04);
                }

                smh.indices_offset = io_get_position(s) - base_offset;
                obj_classic_write_index(o, s, sub_mesh, &smh.indices_count);

                io_position_push(s, base_offset + mh->sub_meshes_offset + sub_mesh_size * j, SEEK_SET);
                io_write(s, 0, 0x04);
                io_write_float_t(s, sub_mesh->bounding_sphere.center.x);
                io_write_float_t(s, sub_mesh->bounding_sphere.center.y);
                io_write_float_t(s, sub_mesh->bounding_sphere.center.z);
                io_write_float_t(s, sub_mesh->bounding_sphere.radius);
                io_write_uint32_t(s, sub_mesh->material_index);
                io_write(s, &sub_mesh->texcoord_indices, 0x08);
                io_write_int32_t(s, smh.bone_index_count);
                io_write_int32_t(s, (int32_t)smh.bone_indices_offset);
                io_write_uint32_t(s, sub_mesh->bones_per_vertex);
                io_write_uint32_t(s, sub_mesh->primitive_type);
                io_write_uint32_t(s, sub_mesh->index_format);
                io_write_int32_t(s, smh.indices_count);
                io_write_int32_t(s, (int32_t)smh.indices_offset);
                io_write_uint32_t(s, sub_mesh->flags);
                io_write(s, 0, 0x18);
                io_write_uint32_t(s, sub_mesh->index_offset);
                io_position_pop(s);
            }
        }

        io_position_push(s, base_offset + oh.meshes_offset, SEEK_SET);
        for (int32_t i = 0; i < oh.meshes_count; i++) {
            obj_mesh* mesh = &o->meshes.begin[i];
            obj_mesh_header* mh = &mhs[i];

            io_write(s, 0, 0x04);
            io_write_float_t(s, mesh->bounding_sphere.center.x);
            io_write_float_t(s, mesh->bounding_sphere.center.y);
            io_write_float_t(s, mesh->bounding_sphere.center.z);
            io_write_float_t(s, mesh->bounding_sphere.radius);
            io_write_int32_t(s, mh->sub_meshes_count);
            io_write_int32_t(s, (int32_t)mh->sub_meshes_offset);
            io_write_uint32_t(s, mh->attrib_type);
            io_write_int32_t(s, mh->vertex_size);
            io_write_int32_t(s, mh->vertex_count);

            for (int32_t j = 0; j < 20; j++)
                io_write_int32_t(s, (int32_t)mh->attrib_offsets[j]);

            io_write_int32_t(s, mesh->flags);
            io_write_uint32_t(s, mh->attrib_flags);
            io_write(s, 0, 0x18);
            io_write(s, &mesh->name, sizeof(mesh->name) - 1);
            io_write_char(s, '\0');
        }
        io_position_pop(s);
        free(mhs);
    }

    oh.materials_count = (int32_t)(o->materials.end - o->materials.begin);
    if (oh.materials_count) {
        oh.materials_offset = io_get_position(s) - base_offset;
        for (int32_t i = 0; i < oh.materials_count; i++)
            io_write(s, &o->materials.begin[i], sizeof(obj_material_parent));
    }
    io_align_write(s, 0x10);

    io_position_push(s, base_offset, SEEK_SET);
    io_write_uint32_t(s, 0x10000);
    io_write_uint32_t(s, 0x00);
    io_write_float_t(s, o->bounding_sphere.center.x);
    io_write_float_t(s, o->bounding_sphere.center.y);
    io_write_float_t(s, o->bounding_sphere.center.z);
    io_write_float_t(s, o->bounding_sphere.radius);
    io_write_int32_t(s, oh.meshes_count);
    io_write_int32_t(s, (int32_t)oh.meshes_offset);
    io_write_int32_t(s, oh.materials_count);
    io_write_int32_t(s, (int32_t)oh.materials_offset);
    io_position_pop(s);
}

static void obj_classic_read_skin(obj* o, stream* s, ssize_t base_offset) {
    obj_skin* sk = &o->skin;
    o->skin_init = true;
    io_set_position(s, base_offset, SEEK_SET);

    obj_skin_header sh;
    memset(&sh, 0, sizeof(obj_skin_header));
    sh.bone_ids_offset = io_read_int32_t(s);
    sh.bone_matrices_offset = io_read_int32_t(s);
    sh.bone_names_offset = io_read_int32_t(s);
    sh.ex_data_offset = io_read_int32_t(s);
    sh.bone_count = io_read_int32_t(s);
    sh.bone_parent_ids_offset = io_read_int32_t(s);
    io_read(s, 0, 0x0C);

    if (sh.bone_ids_offset) {
        sk->bones = vector_empty(obj_skin_bone);
        vector_obj_skin_bone_reserve(&sk->bones, sh.bone_count);

        int32_t* bone_names_offsets = 0;
        if (sh.bone_names_offset) {
            bone_names_offsets = force_malloc_s(int32_t, sh.bone_count);
            io_set_position(s, sh.bone_names_offset, SEEK_SET);
            for (int32_t i = 0; i < sh.bone_count; i++)
                bone_names_offsets[i] = io_read_int32_t(s);
        }


        if (sh.bone_ids_offset) {
            obj_skin_bone bone;
            memset(&bone, 0, sizeof(obj_skin_bone));

            io_set_position(s, sh.bone_ids_offset, SEEK_SET);
            for (int32_t i = 0; i < sh.bone_count; i++) {
                bone.id = io_read_uint32_t(s);
                vector_obj_skin_bone_push_back(&sk->bones, &bone);
            }

            if (sh.bone_matrices_offset) {
                io_set_position(s, sh.bone_matrices_offset, SEEK_SET);
                for (int32_t i = 0; i < sh.bone_count; i++) {
                    mat4 mat;
                    mat.row0.x = io_read_float_t(s);
                    mat.row0.y = io_read_float_t(s);
                    mat.row0.z = io_read_float_t(s);
                    mat.row0.w = io_read_float_t(s);
                    mat.row1.x = io_read_float_t(s);
                    mat.row1.y = io_read_float_t(s);
                    mat.row1.z = io_read_float_t(s);
                    mat.row1.w = io_read_float_t(s);
                    mat.row2.x = io_read_float_t(s);
                    mat.row2.y = io_read_float_t(s);
                    mat.row2.z = io_read_float_t(s);
                    mat.row2.w = io_read_float_t(s);
                    mat.row3.x = io_read_float_t(s);
                    mat.row3.y = io_read_float_t(s);
                    mat.row3.z = io_read_float_t(s);
                    mat.row3.w = io_read_float_t(s);
                    mat4_transpose(&mat, &sk->bones.begin[i].inv_bind_pose_mat);
                }
            }

            if (sh.bone_names_offset)
                for (int32_t i = 0; i < sh.bone_count; i++)
                    io_read_string_null_terminated_offset(s,
                        bone_names_offsets[i], &sk->bones.begin[i].name);

            if (sh.bone_parent_ids_offset) {
                io_set_position(s, sh.bone_parent_ids_offset, SEEK_SET);
                for (int32_t i = 0; i < sh.bone_count; i++)
                    sk->bones.begin[i].parent = io_read_uint32_t(s);
            }
        }
        free(bone_names_offsets);
    }

    if (sh.ex_data_offset) {
        obj_skin_ex_data* ex = &sk->ex_data;
        sk->ex_data_init = true;
        io_set_position(s, sh.ex_data_offset, SEEK_SET);

        obj_skin_ex_data_header exh;
        memset(&exh, 0, sizeof(obj_skin_ex_data_header));
        exh.osage_count = io_read_int32_t(s);
        exh.osage_nodes_count = io_read_int32_t(s);
        io_read(s, 0, 0x04);
        exh.osage_nodes_offset = io_read_int32_t(s);
        exh.osage_names_offset = io_read_int32_t(s);
        exh.blocks_offset = io_read_int32_t(s);
        exh.strings_count = io_read_int32_t(s);
        exh.strings_offset = io_read_int32_t(s);
        exh.osage_sibling_infos_offset = io_read_int32_t(s);
        exh.cloth_count = io_read_int32_t(s);
        io_read(s, 0, 0x1C);

        vector_string string_set = vector_empty(string);
        vector_string_reserve(&string_set, exh.strings_count);
        if (exh.strings_offset) {
            int32_t* strings_offsets = force_malloc_s(int32_t, exh.strings_count);
            io_set_position(s, exh.strings_offset, SEEK_SET);
            for (int32_t i = 0; i < exh.strings_count; i++)
                strings_offsets[i] = io_read_int32_t(s);

            for (int32_t i = 0; i < exh.strings_count; i++) {
                string str;
                io_read_string_null_terminated_offset(s, strings_offsets[i], &str);
                vector_string_push_back(&string_set, &str);
            }
            free(strings_offsets);
        }

        ex->osage_nodes = vector_empty(obj_skin_osage_node);
        vector_obj_skin_osage_node_reserve(&ex->osage_nodes, exh.osage_nodes_count);
        if (exh.osage_nodes_offset) {
            obj_skin_osage_node osage_node;
            memset(&osage_node, 0, sizeof(obj_skin_osage_node));

            io_set_position(s, exh.osage_nodes_offset, SEEK_SET);
            for (int32_t i = 0; i < exh.osage_nodes_count; i++) {
                int32_t name = io_read_uint32_t(s);
                if (name & 0x8000)
                    string_copy(&string_set.begin[name & 0x7FFF], &osage_node.name);
                else
                    memset(&osage_node.name, 0, sizeof(string));

                osage_node.length = io_read_float_t(s);
                io_read(s, 0, 0x04);
                vector_obj_skin_osage_node_push_back(&ex->osage_nodes, &osage_node);
            }
        }

        if (exh.blocks_offset) {
            int32_t blocks_count = 0;
            io_set_position(s, exh.blocks_offset, SEEK_SET);
            while (io_read_int32_t(s)) {
                io_read(s, 0, 0x04);
                blocks_count++;
            }

            obj_skin_block_header* bhs = force_malloc_s(obj_skin_block_header, blocks_count);
            io_set_position(s, exh.blocks_offset, SEEK_SET);
            for (int32_t i = 0; i < blocks_count; i++) {
                bhs[i].block_signature_offset = io_read_int32_t(s);
                bhs[i].block_offset = io_read_int32_t(s);
            }

            obj_skin_block block;
            memset(&block, 0, sizeof(obj_skin_block));

            ex->blocks = vector_empty(obj_skin_block);
            vector_obj_skin_block_reserve(&ex->blocks, blocks_count);
            for (int32_t i = 0; i < blocks_count; i++) {
                bool valid = false;

                char* block_signature = io_read_utf8_string_null_terminated_offset(s,
                    bhs[i].block_signature_offset);
                if (utf8_length(block_signature) == 3) {
                    valid = true;

                    uint32_t signature = load_reverse_endianness_uint32_t(block_signature);
                    switch (signature) {
                    case 'CLS\0':
                        block.type = OBJ_SKIN_BLOCK_CLOTH;
                        io_set_position(s, bhs[i].block_offset, SEEK_SET);
                        obj_classic_read_skin_block_cloth(&block.cloth,
                            s, string_set.begin);
                        break;
                    case 'CNS\0':
                        block.type = OBJ_SKIN_BLOCK_CONSTRAINT;
                        io_set_position(s, bhs[i].block_offset, SEEK_SET);
                        obj_classic_read_skin_block_constraint(&block.constraint,
                            s, string_set.begin);
                        break;
                    case 'EXP\0':
                        block.type = OBJ_SKIN_BLOCK_EXPRESSION;
                        io_set_position(s, bhs[i].block_offset, SEEK_SET);
                        obj_classic_read_skin_block_expression(&block.expression,
                            s, string_set.begin);
                        break;
                    case 'MOT\0':
                        block.type = OBJ_SKIN_BLOCK_MOTION;
                        io_set_position(s, bhs[i].block_offset, SEEK_SET);
                        obj_classic_read_skin_block_motion(&block.motion,
                            s, string_set.begin);
                        break;
                    case 'OSG\0':
                        block.type = OBJ_SKIN_BLOCK_OSAGE;
                        io_set_position(s, bhs[i].block_offset, SEEK_SET);
                        obj_classic_read_skin_block_osage(&block.osage,
                            s, string_set.begin);
                        break;
                    default:
                        valid = false;
                        break;
                    }
                }
                free(block_signature);

                if (valid)
                    vector_obj_skin_block_push_back(&ex->blocks, &block);
            }
            free(bhs);
        }

        io_set_position(s, exh.osage_sibling_infos_offset, SEEK_SET);
        while (true) {
            uint32_t bone_name_index = io_read_uint32_t(s);
            if (!bone_name_index)
                break;

            uint32_t sibling_name = io_read_uint32_t(s);
            float_t sibling_distance = io_read_float_t(s);

            if (!(bone_name_index & 0x8000))
                continue;

            char* bone_name = string_data(&string_set.begin[bone_name_index & 0x7FFF]);

            obj_skin_osage_node* osage_node = 0;
            for (obj_skin_osage_node* i = ex->osage_nodes.begin; i != ex->osage_nodes.end; i++)
                if (!strcmp(string_data(&i->name), bone_name)) {
                    string_free(&i->sibling_name);
                    if (sibling_name & 0x8000)
                        string_copy(&string_set.begin[sibling_name & 0x7FFF], &i->sibling_name);
                    else
                        memset(&i->sibling_name, 0, sizeof(string));

                    i->sibling_max_distance = sibling_distance;
                    break;
                }
        }

        for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++) {
            if (i->type != OBJ_SKIN_BLOCK_OSAGE)
                continue;

            obj_skin_block_osage* osage = &i->osage;
            if (!(osage->nodes.end - osage->nodes.begin))
                continue;

            int32_t count = osage->count;
            obj_skin_osage_node* donor_osage_node = &ex->osage_nodes.begin[osage->start_index];
            obj_skin_osage_node* osage_node = osage->nodes.begin;
            for (int32_t j = 0; j < count; j++) {
                string_free(&osage_node->name);
                string_free(&osage_node->sibling_name);
                string_copy(&donor_osage_node->name, &osage_node->name);
                osage_node->length = donor_osage_node->length;
                string_copy(&donor_osage_node->sibling_name, &osage_node->sibling_name);
                osage_node->sibling_max_distance = donor_osage_node->sibling_max_distance;

                donor_osage_node++;
                osage_node++;
            }
        }

        for (string* i = string_set.begin; i != string_set.end; i++)
            string_free(i);
        vector_string_free(&string_set);
    }
}

static void obj_classic_write_skin(obj* o, stream* s, ssize_t base_offset) {
    obj_skin* sk = &o->skin;

    obj_skin_header sh;
    memset(&sh, 0, sizeof(obj_skin_header));
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write_int32_t(s, 0);
    io_write(s, 0, 0x0C);
    io_align_write(s, 0x10);

    vector_string strings = vector_empty(string);
    vector_ssize_t string_offsets = vector_empty(ssize_t);
    vector_string string_set = vector_empty(string);
    sh.bone_count = (int32_t)(sk->bones.end - sk->bones.begin);
    if (sh.bone_count) {
        sh.bone_ids_offset = io_get_position(s);
        for (int32_t i = 0; i < sh.bone_count; i++)
            io_write_int32_t(s, sk->bones.begin[i].id);
        io_align_write(s, 0x10);

        sh.bone_parent_ids_offset = io_get_position(s);
        for (int32_t i = 0; i < sh.bone_count; i++)
            io_write_int32_t(s, sk->bones.begin[i].parent);
        io_align_write(s, 0x10);

        sh.bone_names_offset = io_get_position(s);
        for (int32_t i = 0; i < sh.bone_count; i++) {
            io_write_int32_t(s, 0);
            obj_skin_strings_push_back_check(&strings, string_data(&sk->bones.begin[i].name));
        }
        io_align_write(s, 0x10);

        sh.bone_matrices_offset = io_get_position(s);
        for (int32_t i = 0; i < sh.bone_count; i++) {
            mat4 mat;
            mat4_transpose(&sk->bones.begin[i].inv_bind_pose_mat, &mat);
            io_write_float_t(s, mat.row0.x);
            io_write_float_t(s, mat.row0.y);
            io_write_float_t(s, mat.row0.z);
            io_write_float_t(s, mat.row0.w);
            io_write_float_t(s, mat.row1.x);
            io_write_float_t(s, mat.row1.y);
            io_write_float_t(s, mat.row1.z);
            io_write_float_t(s, mat.row1.w);
            io_write_float_t(s, mat.row2.x);
            io_write_float_t(s, mat.row2.y);
            io_write_float_t(s, mat.row2.z);
            io_write_float_t(s, mat.row2.w);
            io_write_float_t(s, mat.row3.x);
            io_write_float_t(s, mat.row3.y);
            io_write_float_t(s, mat.row3.z);
            io_write_float_t(s, mat.row3.w);
        }
        io_align_write(s, 0x10);
    }

    obj_skin_block_header* bhs = 0;
    ssize_t osage_block_node = 0;
    ssize_t motion_block_node_mats = 0;
    ssize_t motion_block_node_name_offset = 0;
    ssize_t field_18 = 0;
    ssize_t field_1C = 0;
    ssize_t field_20 = 0;
    ssize_t field_24 = 0;
    ssize_t field_28 = 0;
    ssize_t blocks_count = 0;

    obj_skin_ex_data_header exh;
    memset(&exh, 0, sizeof(obj_skin_ex_data_header));
    if (sk->ex_data_init) {
        obj_skin_ex_data* ex = &sk->ex_data;

        sh.ex_data_offset = io_get_position(s);
        io_write_int32_t(s, 0);
        io_write_int32_t(s, 0);
        io_write(s, 0, 0x04);
        io_write_int32_t(s, 0);
        io_write_int32_t(s, 0);
        io_write_int32_t(s, 0);
        io_write_int32_t(s, 0);
        io_write_int32_t(s, 0);
        io_write_int32_t(s, 0);
        io_write_int32_t(s, 0);
        io_write(s, 0, 0x1C);
        io_align_write(s, 0x10);

        exh.osage_nodes_count = (int32_t)(ex->osage_nodes.end - ex->osage_nodes.begin);
        if (exh.osage_nodes_count) {
            exh.osage_nodes_offset = io_get_position(s);
            for (int32_t i = 0; i < exh.osage_nodes_count; i++) {
                io_write_int32_t(s, 0);
                io_write_int32_t(s, 0);
                io_write_int32_t(s, 0);
            }
            io_align_write(s, 0x10);

            exh.osage_sibling_infos_offset = io_get_position(s);
            for (obj_skin_osage_node* i = ex->osage_nodes.begin; i != ex->osage_nodes.end; i++) {
                if (!string_length(&i->sibling_name))
                    continue;

                io_write_int32_t(s, 0);
                io_write_int32_t(s, 0);
                io_write_int32_t(s, 0);
            }
            io_write_int32_t(s, 0);
            io_write_int32_t(s, 0);
            io_write_int32_t(s, 0);
            io_align_write(s, 0x10);
        }

        exh.cloth_count = 0;
        exh.osage_count = 0;
        for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
            if (i->type == OBJ_SKIN_BLOCK_CLOTH)
                exh.cloth_count++;
            else if (i->type == OBJ_SKIN_BLOCK_OSAGE)
                exh.osage_count++;

        if (exh.osage_count || exh.cloth_count) {
            exh.osage_names_offset = io_get_position(s);
            for (int32_t i = 0; i < exh.osage_count; i++)
                io_write_int32_t(s, 0);

            for (int32_t i = 0; i < exh.cloth_count; i++)
                io_write_int32_t(s, 0);
            io_align_write(s, 0x10);
        }

        blocks_count = ex->blocks.end - ex->blocks.begin;
        for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
            switch (i->type) {
            case OBJ_SKIN_BLOCK_CLOTH: {
                obj_skin_block_cloth* cls = &i->cloth;
                obj_skin_strings_push_back_check(&strings, string_data(&cls->mesh_name));
                obj_skin_strings_push_back_check(&strings, string_data(&cls->backface_mesh_name));
                for (obj_skin_block_cloth_field_1C* j = cls->field_1C.begin; j != cls->field_1C.end; j++) {
                    obj_skin_strings_push_back_check(&strings, string_data(&j->sub_data_0.bone_name));
                    obj_skin_strings_push_back_check(&strings, string_data(&j->sub_data_1.bone_name));
                    obj_skin_strings_push_back_check(&strings, string_data(&j->sub_data_2.bone_name));
                    obj_skin_strings_push_back_check(&strings, string_data(&j->sub_data_3.bone_name));
                }
                obj_skin_strings_push_back_check(&strings, "CLS");
            } break;
            case OBJ_SKIN_BLOCK_CONSTRAINT: {
                obj_skin_block_constraint* cns = &i->constraint;
                obj_skin_strings_push_back_check(&strings, string_data(&cns->base.parent_name));
                obj_skin_strings_push_back_check(&strings, string_data(&cns->base.name));
                obj_skin_strings_push_back_check(&strings, string_data(&cns->source_node_name));
                switch (cns->type) {
                case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
                    obj_skin_strings_push_back_check(&strings, "Direction");
                    obj_skin_strings_push_back_check(&strings, string_data(&cns->direction.up_vector.name));
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
                    obj_skin_strings_push_back_check(&strings, "Distance");
                    obj_skin_strings_push_back_check(&strings, string_data(&cns->distance.up_vector.name));
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
                    obj_skin_strings_push_back_check(&strings, "Orientation");
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION:
                    obj_skin_strings_push_back_check(&strings, "Position");
                    obj_skin_strings_push_back_check(&strings, string_data(&cns->position.up_vector.name));
                    break;
                }
                obj_skin_strings_push_back_check(&strings, "CNS");

                obj_skin_strings_push_back_check(&string_set, string_data(&cns->base.name));
            } break;
            case OBJ_SKIN_BLOCK_EXPRESSION: {
                obj_skin_block_expression* exp = &i->expression;
                for (int32_t j = 0; j < exp->expression_count; j++)
                    obj_skin_strings_push_back_check(&strings, string_data(&exp->expressions[j]));
                obj_skin_strings_push_back_check(&strings, string_data(&exp->base.parent_name));
                obj_skin_strings_push_back_check(&strings, string_data(&exp->base.name));
                obj_skin_strings_push_back_check(&strings, "EXP");

                obj_skin_strings_push_back_check(&string_set, string_data(&exp->base.name));
            } break;
            case OBJ_SKIN_BLOCK_MOTION: {
                obj_skin_block_motion* mot = &i->motion;
                obj_skin_strings_push_back_check(&strings, string_data(&mot->base.parent_name));
                obj_skin_strings_push_back_check(&strings, string_data(&mot->base.name));
                for (obj_skin_motion_node* j = mot->nodes.begin; j != mot->nodes.end; j++)
                    obj_skin_strings_push_back_check(&strings, string_data(&j->name));
                obj_skin_strings_push_back_check(&strings, "MOT");

                obj_skin_strings_push_back_check(&string_set, string_data(&mot->base.name));
                for (obj_skin_motion_node* j = mot->nodes.begin; j != mot->nodes.end; j++)
                    obj_skin_strings_push_back_check(&string_set, string_data(&j->name));
            } break;
            case OBJ_SKIN_BLOCK_OSAGE: {
                obj_skin_block_osage* osg = &i->osage;
                obj_skin_strings_push_back_check(&strings, string_data(&osg->base.parent_name));
                obj_skin_strings_push_back_check(&strings, string_data(&osg->base.name));
                obj_skin_strings_push_back_check(&strings, string_data(&osg->external_name));
                for (obj_skin_osage_node* j = osg->nodes.begin; j != osg->nodes.end; j++) {
                    obj_skin_strings_push_back_check(&strings, string_data(&j->name));
                    obj_skin_strings_push_back_check(&strings, string_data(&j->sibling_name));
                }
                obj_skin_strings_push_back_check(&strings, "OSG");

                obj_skin_strings_push_back_check(&string_set, string_data(&osg->external_name));
                for (obj_skin_osage_node* j = osg->nodes.begin; j != osg->nodes.end; j++) {
                    obj_skin_strings_push_back_check(&string_set, string_data(&j->name));
                    obj_skin_strings_push_back_check(&string_set, string_data(&j->sibling_name));
                }

                obj_skin_osage_node* donor_osage_node = &ex->osage_nodes.begin[osg->start_index];
                for (int32_t j = 0; j < osg->count; j++) {
                    obj_skin_strings_push_back_check(&string_set, string_data(&donor_osage_node->name));
                    obj_skin_strings_push_back_check(&string_set, string_data(&donor_osage_node->sibling_name));
                    donor_osage_node++;
                }
                obj_skin_strings_push_back_check(&string_set, string_data(&osg->base.name));
            } break;
            }

        exh.strings_count = (int32_t)(string_set.end - string_set.begin);
        exh.strings_offset = io_get_position(s);
        for (string* i = string_set.begin; i != string_set.end; i++)
            io_write_int32_t(s, 0);
        io_align_write(s, 0x10);

        if (blocks_count > 0) {
            exh.blocks_offset = io_get_position(s);
            for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++) {
                io_write_int32_t(s, 0);
                io_write_int32_t(s, 0);
            }
            io_write_int32_t(s, 0);
            io_write_int32_t(s, 0);
            io_align_write(s, 0x10);

            bhs = force_malloc_s(obj_skin_block_header, blocks_count);
            for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                switch (i->type) {
                case OBJ_SKIN_BLOCK_OSAGE:
                    bhs[i - ex->blocks.begin].block_offset = io_get_position(s);
                    io_write(s, 0, 0x28);
                    io_write(s, 0, 0x14);
                    io_write(s, 0, 0x14);
                    break;
                }
            io_align_write(s, 0x10);

            for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                switch (i->type) {
                case OBJ_SKIN_BLOCK_EXPRESSION:
                    bhs[i - ex->blocks.begin].block_offset = io_get_position(s);
                    io_write(s, 0, 0x28);
                    io_write(s, 0, 0x2C);
                    break;
                }
            io_align_write(s, 0x10);

            for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                switch (i->type) {
                case OBJ_SKIN_BLOCK_CLOTH:
                    bhs[i - ex->blocks.begin].block_offset = io_get_position(s);
                    io_write(s, 0, 0x34);
                    break;
                }
            io_align_write(s, 0x10);

            for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                switch (i->type) {
                case OBJ_SKIN_BLOCK_CONSTRAINT:
                    bhs[i - ex->blocks.begin].block_offset = io_get_position(s);
                    io_write(s, 0, 0x28);
                    io_write(s, 0, 0x10);
                    switch (i->constraint.type) {
                    case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
                        io_write(s, 0, 0x24);
                        io_write(s, 0, 0x18);
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
                        io_write(s, 0, 0x24);
                        io_write(s, 0, 0x04);
                        io_write(s, 0, 0x14);
                        io_write(s, 0, 0x14);
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
                        io_write(s, 0, 0x0C);
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION:
                        io_write(s, 0, 0x24);
                        io_write(s, 0, 0x14);
                        io_write(s, 0, 0x14);
                        break;
                    }
                    break;
                }
            io_align_write(s, 0x10);

            int32_t motion_block_count = 0;
            for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                switch (i->type) {
                case OBJ_SKIN_BLOCK_MOTION:
                    bhs[i - ex->blocks.begin].block_offset = io_get_position(s);
                    io_write(s, 0, 0x28);
                    io_write(s, 0, 0x10);
                    motion_block_count++;
                    break;
                }
            io_align_write(s, 0x10);

            if (exh.osage_count) {
                osage_block_node = io_get_position(s);
                for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                    if (i->type == OBJ_SKIN_BLOCK_OSAGE)
                        io_write(s, 0, (i->osage.nodes.end - i->osage.nodes.begin)
                            * (sizeof(uint32_t) + sizeof(float_t) * 4));
                io_align_write(s, 0x10);
            }

            if (motion_block_count) {
                motion_block_node_mats = io_get_position(s);
                for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                    if (i->type == OBJ_SKIN_BLOCK_MOTION)
                        io_write(s, 0, (i->motion.nodes.end - i->motion.nodes.begin) * sizeof(mat4));
                io_align_write(s, 0x10);

                motion_block_node_name_offset = io_get_position(s);
                for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                    if (i->type == OBJ_SKIN_BLOCK_MOTION)
                        io_write(s, 0, (i->motion.nodes.end - i->motion.nodes.begin) * sizeof(uint32_t));
                io_align_write(s, 0x10);
            }

            if (exh.cloth_count) {
                field_18 = io_get_position(s);
                for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                    if (i->type == OBJ_SKIN_BLOCK_CLOTH)
                        for (int32_t j = 0; j < 32; j++)
                            if (i->cloth.field_10 & (1 << j))
                                io_write(s, 0, sizeof(mat4));
                io_align_write(s, 0x10);

                field_1C = io_get_position(s);
                for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                    if (i->type == OBJ_SKIN_BLOCK_CLOTH)
                        io_write(s, 0, (10 * sizeof(int32_t) + 4 * (sizeof(int32_t) + 3 * sizeof(int32_t)))
                            * (i->cloth.field_1C.end - i->cloth.field_1C.begin));
                io_align_write(s, 0x10);

                field_20 = io_get_position(s);
                for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                    if (i->type == OBJ_SKIN_BLOCK_CLOTH)
                        io_write(s, 0, 10 * (11 * sizeof(int32_t))
                            * (i->cloth.field_20.end - i->cloth.field_20.begin));
                io_align_write(s, 0x10);

                field_24 = io_get_position(s);
                for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                    if (i->type == OBJ_SKIN_BLOCK_CLOTH)
                        io_write(s, 0, sizeof(uint16_t) + (i->cloth.field_24.end
                            - i->cloth.field_24.begin) * sizeof(uint16_t));
                io_align_write(s, 0x10);

                field_28 = io_get_position(s);
                for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                    if (i->type == OBJ_SKIN_BLOCK_CLOTH)
                        io_write(s, 0, sizeof(uint16_t) + (i->cloth.field_28.end
                            - i->cloth.field_28.begin) * sizeof(uint16_t));
                io_align_write(s, 0x10);
            }
        }
    }

    if (sh.bone_count || sk->ex_data_init) {
        for (string* i = strings.begin; i != &strings.end[-1]; i++) {
            char* i_str = string_data(i);
            for (string* j = i + 1; j != strings.end; j++)
                if (strcmp(i_str, string_data(j)) > 0) {
                    string temp = *i;
                    *i = *j;
                    *j = temp;
                    i_str = string_data(i);
                }
        }

        for (string* i = strings.begin; i != strings.end; i++) {
            ssize_t off = io_get_position(s);
            io_write(s, string_data(i), string_length(i) + 1);
            vector_ssize_t_push_back(&string_offsets, &off);
        }
    }
    io_align_write(s, 0x10);

    vector_string osage_names = vector_empty(string);
    if (sk->ex_data_init) {
        obj_skin_ex_data* ex = &sk->ex_data;

        size_t cls_offset = 0;
        size_t cns_offset = 0;
        size_t exp_offset = 0;
        size_t mot_offset = 0;
        size_t osg_offset = 0;
        for (string* i = strings.begin; i != strings.end; i++)
            if (!strcmp(string_data(i), "CLS"))
                cls_offset = string_offsets.begin[i - strings.begin];

        for (string* i = strings.begin; i != strings.end; i++)
            if (!strcmp(string_data(i), "CNS"))
                cns_offset = string_offsets.begin[i - strings.begin];

        for (string* i = strings.begin; i != strings.end; i++)
            if (!strcmp(string_data(i), "EXP"))
                exp_offset = string_offsets.begin[i - strings.begin];

        for (string* i = strings.begin; i != strings.end; i++)
            if (!strcmp(string_data(i), "MOT"))
                mot_offset = string_offsets.begin[i - strings.begin];

        for (string* i = strings.begin; i != strings.end; i++)
            if (!strcmp(string_data(i), "OSG"))
                osg_offset = string_offsets.begin[i - strings.begin];

        if (blocks_count > 0) {
            for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++) {
                if (i->type != OBJ_SKIN_BLOCK_CLOTH)
                    continue;

                io_position_push(s, bhs[i - ex->blocks.begin].block_offset, SEEK_SET);
                obj_classic_write_skin_block_cloth(&i->cloth,
                    s, &strings, &string_offsets, &field_18, &field_1C, &field_20, &field_24, &field_28);
                io_position_pop(s);
            }

            ssize_t constraint_type_name_offsets[4];
            constraint_type_name_offsets[0] = obj_skin_strings_get_string_offset(&strings,
                &string_offsets, "Direction");
            constraint_type_name_offsets[1] = obj_skin_strings_get_string_offset(&strings,
                &string_offsets, "Distance");
            constraint_type_name_offsets[2] = obj_skin_strings_get_string_offset(&strings,
                &string_offsets, "Orientation");
            constraint_type_name_offsets[3] = obj_skin_strings_get_string_offset(&strings,
                &string_offsets, "Position");

            for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++) {
                if (i->type != OBJ_SKIN_BLOCK_CONSTRAINT)
                    continue;

                io_position_push(s, bhs[i - ex->blocks.begin].block_offset, SEEK_SET);
                obj_classic_write_skin_block_constraint(&i->constraint,
                    s, &strings, &string_offsets, constraint_type_name_offsets);
                io_position_pop(s);
            }

            for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++) {
                if (i->type != OBJ_SKIN_BLOCK_EXPRESSION)
                    continue;

                io_position_push(s, bhs[i - ex->blocks.begin].block_offset, SEEK_SET);
                obj_classic_write_skin_block_expression(&i->expression,
                    s, &strings, &string_offsets);
                io_position_pop(s);
            }

            for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++) {
                if (i->type != OBJ_SKIN_BLOCK_MOTION)
                    continue;

                io_position_push(s, bhs[i - ex->blocks.begin].block_offset, SEEK_SET);
                obj_classic_write_skin_block_motion(&i->motion,
                    s, &strings, &string_offsets, &string_set,
                    &motion_block_node_name_offset, &motion_block_node_mats);
                io_position_pop(s);
            }

            for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++) {
                if (i->type != OBJ_SKIN_BLOCK_OSAGE)
                    continue;

                io_position_push(s, bhs[i - ex->blocks.begin].block_offset, SEEK_SET);
                obj_classic_write_skin_block_osage(&i->osage,
                    s, &strings, &string_offsets, &string_set, &osage_block_node);
                io_position_pop(s);
            }

            for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                if (i->type == OBJ_SKIN_BLOCK_OSAGE)
                    obj_skin_strings_push_back_check(&osage_names, string_data(&i->osage.external_name));
                else if (i->type == OBJ_SKIN_BLOCK_CLOTH)
                    obj_skin_strings_push_back_check(&osage_names, string_data(&i->cloth.mesh_name));

            io_position_push(s, exh.blocks_offset, SEEK_SET);
            for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                switch (i->type) {
                case OBJ_SKIN_BLOCK_CLOTH:
                    io_write_int32_t(s, (int32_t)cls_offset);
                    io_write_int32_t(s, (int32_t)bhs[i - ex->blocks.begin].block_offset);
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT:
                    io_write_int32_t(s, (int32_t)cns_offset);
                    io_write_int32_t(s, (int32_t)bhs[i - ex->blocks.begin].block_offset);
                    break;
                case OBJ_SKIN_BLOCK_EXPRESSION:
                    io_write_int32_t(s, (int32_t)exp_offset);
                    io_write_int32_t(s, (int32_t)bhs[i - ex->blocks.begin].block_offset);
                    break;
                case OBJ_SKIN_BLOCK_MOTION:
                    io_write_int32_t(s, (int32_t)mot_offset);
                    io_write_int32_t(s, (int32_t)bhs[i - ex->blocks.begin].block_offset);
                    break;
                case OBJ_SKIN_BLOCK_OSAGE:
                    io_write_int32_t(s, (int32_t)osg_offset);
                    io_write_int32_t(s, (int32_t)bhs[i - ex->blocks.begin].block_offset);
                    break;
                default:
                    io_write_int32_t(s, 0);
                    io_write_int32_t(s, 0);
                    break;
                }
            io_write_int32_t(s, 0);
            io_write_int32_t(s, 0);
            io_position_pop(s);
            free(bhs);
        }
    }

    if (sh.bone_count) {
        io_position_push(s, sh.bone_names_offset, SEEK_SET);
        for (int32_t i = 0; i < sh.bone_count; i++) {
            size_t str_offset = obj_skin_strings_get_string_offset(&strings,
                &string_offsets, string_data(&sk->bones.begin[i].name));
            io_write_int32_t(s, (int32_t)str_offset);
        }
        io_position_pop(s);
    }

    if (sk->ex_data_init) {
        obj_skin_ex_data* ex = &sk->ex_data;

        if (blocks_count > 0) {
            io_position_push(s, exh.strings_offset, SEEK_SET);
            for (string* i = string_set.begin; i != string_set.end; i++) {
                size_t str_offset = obj_skin_strings_get_string_offset(&strings,
                    &string_offsets, string_data(i));
                io_write_int32_t(s, (int32_t)str_offset);
            }
            io_position_pop(s);

            exh.osage_nodes_count = (int32_t)(ex->osage_nodes.end - ex->osage_nodes.begin);
            io_position_push(s, exh.osage_nodes_offset, SEEK_SET);
            for (obj_skin_osage_node* i = ex->osage_nodes.begin; i != ex->osage_nodes.end; i++) {
                uint32_t index = obj_skin_strings_get_string_index(&string_set, string_data(&i->name));
                io_write_uint32_t(s, index);
                io_write_float_t(s, i->length);
                io_write_uint32_t(s, index & 0x7FFF);
            }
            io_position_pop(s);

            io_position_push(s, exh.osage_sibling_infos_offset, SEEK_SET);
            for (obj_skin_osage_node* i = ex->osage_nodes.begin; i != ex->osage_nodes.end; i++) {
                if (!string_length(&i->sibling_name))
                    continue;

                uint32_t name = obj_skin_strings_get_string_index(&string_set, string_data(&i->name));
                uint32_t sibling_name = obj_skin_strings_get_string_index(&string_set, string_data(&i->sibling_name));
                io_write_uint32_t(s, name);
                io_write_uint32_t(s, sibling_name);
                io_write_float_t(s, i->sibling_max_distance);
            }
            io_write_int32_t(s, 0);
            io_write_int32_t(s, 0);
            io_write_int32_t(s, 0);
            io_position_pop(s);

            exh.osage_count = (int32_t)(osage_names.end - osage_names.begin);
            exh.osage_count -= exh.cloth_count;
            io_position_push(s, exh.osage_names_offset, SEEK_SET);
            for (string* i = osage_names.begin; i != osage_names.end; i++) {
                size_t str_offset = obj_skin_strings_get_string_offset(&strings,
                    &string_offsets, string_data(i));
                io_write_int32_t(s, (int32_t)str_offset);
            }
            io_position_pop(s);
        }

        for (string* i = osage_names.begin; i != osage_names.end; i++)
            string_free(i);
        vector_string_free(&osage_names);
    }

    for (string* i = strings.begin; i != strings.end; i++)
        string_free(i);
    vector_string_free(&strings);

    vector_ssize_t_free(&string_offsets);

    for (string* i = string_set.begin; i != string_set.end; i++)
        string_free(i);
    vector_string_free(&string_set);

    if (sk->ex_data_init) {
        io_position_push(s, sh.ex_data_offset, SEEK_SET);
        io_write_int32_t(s, exh.osage_count);
        io_write_int32_t(s, exh.osage_nodes_count);
        io_write(s, 0, 0x04);
        io_write_int32_t(s, (int32_t)exh.osage_nodes_offset);
        io_write_int32_t(s, (int32_t)exh.osage_names_offset);
        io_write_int32_t(s, (int32_t)exh.blocks_offset);
        io_write_int32_t(s, exh.strings_count);
        io_write_int32_t(s, (int32_t)exh.strings_offset);
        io_write_int32_t(s, (int32_t)exh.osage_sibling_infos_offset);
        io_write_int32_t(s, exh.cloth_count);
        io_write(s, 0, 0x1C);
        io_position_pop(s);
    }

    io_position_push(s, base_offset, SEEK_SET);
    io_write_int32_t(s, (int32_t)sh.bone_ids_offset);
    io_write_int32_t(s, (int32_t)sh.bone_matrices_offset);
    io_write_int32_t(s, (int32_t)sh.bone_names_offset);
    io_write_int32_t(s, (int32_t)sh.ex_data_offset);
    io_write_int32_t(s, sh.bone_count);
    io_write_int32_t(s, (int32_t)sh.bone_parent_ids_offset);
    io_write(s, 0, 0x0C);
    io_position_pop(s);
}

static void obj_classic_read_skin_block_cloth(obj_skin_block_cloth* b,
    stream* s, string* str) {
    int32_t mesh_name_offset = io_read_int32_t(s);
    int32_t backface_mesh_name_offset = io_read_int32_t(s);
    b->field_08 = io_read_int32_t(s);
    int32_t count = io_read_int32_t(s);
    b->field_10 = io_read_int32_t(s);
    b->field_14 = io_read_int32_t(s);
    int32_t field_18_offset = io_read_int32_t(s);
    int32_t field_1C_offset = io_read_int32_t(s);
    int32_t field_20_offset = io_read_int32_t(s);
    int32_t field_24_offset = io_read_int32_t(s);
    int32_t field_28_offset = io_read_int32_t(s);
    b->field_2C = io_read_uint32_t(s);
    b->field_30 = io_read_uint32_t(s);

    io_read_string_null_terminated_offset(s, mesh_name_offset, &b->mesh_name);
    io_read_string_null_terminated_offset(s, backface_mesh_name_offset, &b->backface_mesh_name);

    if (field_18_offset) {
        io_position_push(s, field_18_offset, SEEK_SET);
        for (int32_t i = 0; i < 32; i++) {
            if (!(b->field_10 & (1 << i)))
                continue;

            mat4 mat;
            mat.row0.x = io_read_float_t(s);
            mat.row0.y = io_read_float_t(s);
            mat.row0.z = io_read_float_t(s);
            mat.row0.w = io_read_float_t(s);
            mat.row1.x = io_read_float_t(s);
            mat.row1.y = io_read_float_t(s);
            mat.row1.z = io_read_float_t(s);
            mat.row1.w = io_read_float_t(s);
            mat.row2.x = io_read_float_t(s);
            mat.row2.y = io_read_float_t(s);
            mat.row2.z = io_read_float_t(s);
            mat.row2.w = io_read_float_t(s);
            mat.row3.x = io_read_float_t(s);
            mat.row3.y = io_read_float_t(s);
            mat.row3.z = io_read_float_t(s);
            mat.row3.w = io_read_float_t(s);
            mat4_transpose(&mat, &b->field_18[i]);
        }
        io_position_pop(s);
    }

    if (field_1C_offset) {
        io_position_push(s, field_1C_offset, SEEK_SET);
        b->field_1C = vector_empty(obj_skin_block_cloth_field_1C);
        vector_obj_skin_block_cloth_field_1C_reserve(&b->field_1C, count);
        for (int32_t i = 0; i < count; i++) {
            obj_skin_block_cloth_field_1C f;
            f.field_00 = io_read_float_t(s);
            f.field_04 = io_read_float_t(s);
            f.field_08 = io_read_float_t(s);
            f.field_0C = io_read_float_t(s);
            f.field_10 = io_read_float_t(s);
            f.field_14 = io_read_float_t(s);
            f.field_18 = io_read_float_t(s);
            f.field_1C = io_read_uint32_t(s);
            f.field_20 = io_read_uint32_t(s);
            f.field_24 = io_read_uint32_t(s);
            obj_classic_read_skin_block_cloth_field_1C_sub(&f.sub_data_0, s, str);
            obj_classic_read_skin_block_cloth_field_1C_sub(&f.sub_data_1, s, str);
            obj_classic_read_skin_block_cloth_field_1C_sub(&f.sub_data_2, s, str);
            obj_classic_read_skin_block_cloth_field_1C_sub(&f.sub_data_3, s, str);
            vector_obj_skin_block_cloth_field_1C_push_back(&b->field_1C, &f);
        }
        io_position_pop(s);
    }

    if (field_20_offset) {
        io_position_push(s, field_20_offset, SEEK_SET);
        b->field_20 = vector_empty(obj_skin_block_cloth_field_20);
        vector_obj_skin_block_cloth_field_20_reserve(&b->field_20, count);
        for (int32_t i = 0; i < count; i++) {
            obj_skin_block_cloth_field_20 f;
            obj_classic_read_skin_block_cloth_field_20_sub(&f.sub_data_0, s, str);
            obj_classic_read_skin_block_cloth_field_20_sub(&f.sub_data_1, s, str);
            obj_classic_read_skin_block_cloth_field_20_sub(&f.sub_data_2, s, str);
            obj_classic_read_skin_block_cloth_field_20_sub(&f.sub_data_3, s, str);
            obj_classic_read_skin_block_cloth_field_20_sub(&f.sub_data_4, s, str);
            obj_classic_read_skin_block_cloth_field_20_sub(&f.sub_data_5, s, str);
            obj_classic_read_skin_block_cloth_field_20_sub(&f.sub_data_6, s, str);
            obj_classic_read_skin_block_cloth_field_20_sub(&f.sub_data_7, s, str);
            obj_classic_read_skin_block_cloth_field_20_sub(&f.sub_data_8, s, str);
            obj_classic_read_skin_block_cloth_field_20_sub(&f.sub_data_9, s, str);
            vector_obj_skin_block_cloth_field_20_push_back(&b->field_20, &f);
        }
        io_position_pop(s);
    }

    if (field_24_offset) {
        io_position_push(s, field_24_offset, SEEK_SET);
        b->field_24 = vector_empty(uint16_t);
        count = io_read_uint16_t_stream_reverse_endianness(s);
        vector_uint16_t_reserve(&b->field_24, count);
        for (int32_t i = 0; i < count; i++) {
            uint16_t u = io_read_uint16_t(s);
            vector_uint16_t_push_back(&b->field_24, &u);
        }
        io_position_pop(s);
    }

    if (field_28_offset) {
        io_position_push(s, field_28_offset, SEEK_SET);
        b->field_28 = vector_empty(uint16_t);
        count = io_read_uint16_t_stream_reverse_endianness(s);
        vector_uint16_t_reserve(&b->field_28, count);
        for (int32_t i = 0; i < count; i++) {
            uint16_t u = io_read_uint16_t(s);
            vector_uint16_t_push_back(&b->field_28, &u);
        }
        io_position_pop(s);
    }
}

static void obj_classic_write_skin_block_cloth(obj_skin_block_cloth* b,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets,
    ssize_t* field_18_offset, ssize_t* field_1C_offset, ssize_t* field_20_offset,
    ssize_t* field_24_offset, ssize_t* field_28_offset) {
    int32_t count = (int32_t)(b->field_1C.end - b->field_1C.begin);
    ssize_t mesh_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, string_data(&b->mesh_name));
    ssize_t backface_mesh_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, string_data(&b->backface_mesh_name));

    io_write_int32_t(s, (int32_t)mesh_name_offset);
    io_write_int32_t(s, (int32_t)backface_mesh_name_offset);
    io_write_int32_t(s, b->field_08);
    io_write_int32_t(s, count);
    io_write_int32_t(s, b->field_10);
    io_write_int32_t(s, b->field_14);
    io_write_int32_t(s, (int32_t)*field_18_offset);
    io_write_int32_t(s, (int32_t)*field_1C_offset);
    io_write_int32_t(s, (int32_t)*field_20_offset);
    io_write_int32_t(s, (int32_t)*field_24_offset);
    io_write_int32_t(s, (int32_t)*field_28_offset);
    io_write_uint32_t(s, b->field_2C);
    io_write_uint32_t(s, b->field_30);

    if (b->field_10) {
        io_position_push(s, *field_18_offset, SEEK_SET);
        for (int32_t i = 0; i < 32; i++) {
            if (!(b->field_10 & (1 << i)))
                continue;

            mat4 mat;
            mat4_transpose(&b->field_18[i], &mat);
            io_write_float_t(s, mat.row0.x);
            io_write_float_t(s, mat.row0.y);
            io_write_float_t(s, mat.row0.z);
            io_write_float_t(s, mat.row0.w);
            io_write_float_t(s, mat.row1.x);
            io_write_float_t(s, mat.row1.y);
            io_write_float_t(s, mat.row1.z);
            io_write_float_t(s, mat.row1.w);
            io_write_float_t(s, mat.row2.x);
            io_write_float_t(s, mat.row2.y);
            io_write_float_t(s, mat.row2.z);
            io_write_float_t(s, mat.row2.w);
            io_write_float_t(s, mat.row3.x);
            io_write_float_t(s, mat.row3.y);
            io_write_float_t(s, mat.row3.z);
            io_write_float_t(s, mat.row3.w);
            *field_18_offset += sizeof(mat4);
        }
        io_position_pop(s);
    }

    if (b->field_1C.end - b->field_1C.begin) {
        io_position_push(s, *field_1C_offset, SEEK_SET);
        for (obj_skin_block_cloth_field_1C* i = b->field_1C.begin; i != b->field_1C.end; i++) {
            io_write_float_t(s, i->field_00);
            io_write_float_t(s, i->field_04);
            io_write_float_t(s, i->field_08);
            io_write_float_t(s, i->field_0C);
            io_write_float_t(s, i->field_10);
            io_write_float_t(s, i->field_14);
            io_write_float_t(s, i->field_18);
            io_write_uint32_t(s, i->field_1C);
            io_write_uint32_t(s, i->field_20);
            io_write_uint32_t(s, i->field_24);
            obj_classic_write_skin_block_cloth_field_1C_sub(&i->sub_data_0, s, strings, string_offsets);
            obj_classic_write_skin_block_cloth_field_1C_sub(&i->sub_data_1, s, strings, string_offsets);
            obj_classic_write_skin_block_cloth_field_1C_sub(&i->sub_data_2, s, strings, string_offsets);
            obj_classic_write_skin_block_cloth_field_1C_sub(&i->sub_data_3, s, strings, string_offsets);
        }
        io_position_pop(s);
        *field_1C_offset += (10 * sizeof(int32_t)
            + 4 * (sizeof(int32_t) + 3 * sizeof(int32_t))) * count;
    }

    if (b->field_20.end - b->field_20.begin) {
        io_position_push(s, *field_20_offset, SEEK_SET);
        for (obj_skin_block_cloth_field_20* i = b->field_20.begin; i != b->field_20.end; i++) {
            obj_classic_write_skin_block_cloth_field_20_sub(&i->sub_data_0, s, strings, string_offsets);
            obj_classic_write_skin_block_cloth_field_20_sub(&i->sub_data_1, s, strings, string_offsets);
            obj_classic_write_skin_block_cloth_field_20_sub(&i->sub_data_2, s, strings, string_offsets);
            obj_classic_write_skin_block_cloth_field_20_sub(&i->sub_data_3, s, strings, string_offsets);
            obj_classic_write_skin_block_cloth_field_20_sub(&i->sub_data_4, s, strings, string_offsets);
            obj_classic_write_skin_block_cloth_field_20_sub(&i->sub_data_5, s, strings, string_offsets);
            obj_classic_write_skin_block_cloth_field_20_sub(&i->sub_data_6, s, strings, string_offsets);
            obj_classic_write_skin_block_cloth_field_20_sub(&i->sub_data_7, s, strings, string_offsets);
            obj_classic_write_skin_block_cloth_field_20_sub(&i->sub_data_8, s, strings, string_offsets);
            obj_classic_write_skin_block_cloth_field_20_sub(&i->sub_data_9, s, strings, string_offsets);
        }
        io_position_pop(s);
        *field_20_offset += 10 * (11 * sizeof(int32_t)) * count;
    }

    if (b->field_24.end - b->field_24.begin) {
        io_position_push(s, *field_24_offset, SEEK_SET);
        io_write_uint16_t(s, (uint16_t)(b->field_24.end - b->field_24.begin));
        for (uint16_t* i = b->field_24.begin; i != b->field_24.end; i++)
            io_write_uint16_t(s, *i);
        io_position_pop(s);
        *field_24_offset += sizeof(uint16_t) + (b->field_24.end - b->field_24.begin) * sizeof(uint16_t);
    }

    if (b->field_28.end - b->field_28.begin) {
        io_position_push(s, *field_28_offset, SEEK_SET);
        io_write_uint16_t(s, (uint16_t)(b->field_28.end - b->field_28.begin));
        for (uint16_t* i = b->field_28.begin; i != b->field_28.end; i++)
            io_write_uint16_t(s, *i);
        io_position_pop(s);
        *field_28_offset += sizeof(uint16_t) + (b->field_28.end - b->field_28.begin) * sizeof(uint16_t);
    }
}

static void obj_classic_read_skin_block_cloth_field_1C_sub(obj_skin_block_cloth_field_1C_sub* sub,
    stream* s, string* str) {
    int32_t bone_name_offset = io_read_int32_t(s);
    io_read_string_null_terminated_offset(s, bone_name_offset, &sub->bone_name);
    sub->weight = io_read_float_t(s);
    sub->matrix_index = io_read_uint32_t(s);
    sub->field_0C = io_read_uint32_t(s);
}

static void obj_classic_write_skin_block_cloth_field_1C_sub(obj_skin_block_cloth_field_1C_sub* sub,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets) {
    ssize_t bone_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, string_data(&sub->bone_name));
    io_write_int32_t(s, (int32_t)bone_name_offset);
    io_write_float_t(s, sub->weight);
    io_write_uint32_t(s, sub->matrix_index);
    io_write_uint32_t(s, sub->field_0C);
}

static void obj_classic_read_skin_block_cloth_field_20_sub(obj_skin_block_cloth_field_20_sub* sub,
    stream* s, string* str) {
    sub->field_00 = io_read_uint32_t(s);
    sub->field_04 = io_read_float_t(s);
    sub->field_08 = io_read_float_t(s);
    sub->field_0C = io_read_float_t(s);
    sub->field_10 = io_read_float_t(s);
    sub->field_14 = io_read_float_t(s);
    sub->field_18 = io_read_float_t(s);
    sub->field_1C = io_read_float_t(s);
    sub->field_20 = io_read_float_t(s);
    sub->field_24 = io_read_float_t(s);
    sub->field_28 = io_read_float_t(s);
}

static void obj_classic_write_skin_block_cloth_field_20_sub(obj_skin_block_cloth_field_20_sub* sub,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets) {
    io_write_uint32_t(s, sub->field_00);
    io_write_float_t(s, sub->field_04);
    io_write_float_t(s, sub->field_08);
    io_write_float_t(s, sub->field_0C);
    io_write_float_t(s, sub->field_10);
    io_write_float_t(s, sub->field_14);
    io_write_float_t(s, sub->field_18);
    io_write_float_t(s, sub->field_1C);
    io_write_float_t(s, sub->field_20);
    io_write_float_t(s, sub->field_24);
    io_write_float_t(s, sub->field_28);
}

static void obj_classic_read_skin_block_constraint(obj_skin_block_constraint* b,
    stream* s, string* str) {
    obj_classic_read_skin_block_node(&b->base, s, str);

    int32_t type_offset = io_read_int32_t(s);
    char* type = io_read_utf8_string_null_terminated_offset(s, type_offset);

    int32_t name = io_read_int32_t(s);
    io_read_string_null_terminated_offset(s, name, &b->base.name);

    b->coupling = io_read_int32_t(s);

    int32_t source_node_name = io_read_int32_t(s);
    io_read_string_null_terminated_offset(s, source_node_name, &b->source_node_name);

    if (!strcmp(type, "Direction")) {
        b->type = OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION;
        obj_classic_read_skin_block_constraint_up_vector(&b->direction.up_vector,
            s, str);
        b->direction.align_axis.x = io_read_float_t(s);
        b->direction.align_axis.y = io_read_float_t(s);
        b->direction.align_axis.z = io_read_float_t(s);
        b->direction.target_offset.x = io_read_float_t(s);
        b->direction.target_offset.y = io_read_float_t(s);
        b->direction.target_offset.z = io_read_float_t(s);
    }
    else if (!strcmp(type, "Distance")) {
        b->type = OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE;
        obj_classic_read_skin_block_constraint_up_vector(&b->distance.up_vector,
            s, str);
        b->distance.distance = io_read_float_t(s);
        obj_classic_read_skin_block_constraint_attach_point(&b->distance.constrained_object,
            s, str);
        obj_classic_read_skin_block_constraint_attach_point(&b->distance.constraining_object,
            s, str);
    }
    else if (!strcmp(type, "Orientation")) {
        b->type = OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION;
        b->orientation.offset.x = io_read_float_t(s);
        b->orientation.offset.y = io_read_float_t(s);
        b->orientation.offset.z = io_read_float_t(s);
    }
    else if (!strcmp(type, "Position")) {
        b->type = OBJ_SKIN_BLOCK_CONSTRAINT_POSITION;
        obj_classic_read_skin_block_constraint_up_vector(&b->position.up_vector,
            s, str);
        obj_classic_read_skin_block_constraint_attach_point(&b->position.constrained_object,
            s, str);
        obj_classic_read_skin_block_constraint_attach_point(&b->position.constraining_object,
            s, str);
    }
    else
        b->type = OBJ_SKIN_BLOCK_CONSTRAINT_NONE;
    free(type);
}

static void obj_classic_write_skin_block_constraint(obj_skin_block_constraint* b,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets, ssize_t* offsets) {
    obj_classic_write_skin_block_node(&b->base, s, strings, string_offsets);

    ssize_t type_offset = 0;
    switch (b->type) {
    case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
        type_offset = offsets[0];
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
        type_offset = offsets[1];
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
        type_offset = offsets[2];
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION:
        type_offset = offsets[3];
        break;
    }
    io_write_int32_t(s, (int32_t)type_offset);

    ssize_t name = obj_skin_strings_get_string_offset(strings,
        string_offsets, string_data(&b->base.name));
    io_write_int32_t(s, (int32_t)name);

    io_write_int32_t(s, b->coupling);

    ssize_t source_node_name = obj_skin_strings_get_string_offset(strings,
        string_offsets, string_data(&b->source_node_name));
    io_write_int32_t(s, (int32_t)source_node_name);

    switch (b->type) {
    case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
        obj_classic_write_skin_block_constraint_up_vector(&b->direction.up_vector,
            s, strings, string_offsets);
        io_write_float_t(s, b->direction.align_axis.x);
        io_write_float_t(s, b->direction.align_axis.y);
        io_write_float_t(s, b->direction.align_axis.z);
        io_write_float_t(s, b->direction.target_offset.x);
        io_write_float_t(s, b->direction.target_offset.y);
        io_write_float_t(s, b->direction.target_offset.z);
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
        obj_classic_write_skin_block_constraint_up_vector(&b->distance.up_vector,
            s, strings, string_offsets);
        io_write_float_t(s, b->distance.distance);
        obj_classic_write_skin_block_constraint_attach_point(&b->distance.constrained_object,
            s, strings, string_offsets);
        obj_classic_write_skin_block_constraint_attach_point(&b->distance.constraining_object,
            s, strings, string_offsets);
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
        io_write_float_t(s, b->orientation.offset.x);
        io_write_float_t(s, b->orientation.offset.y);
        io_write_float_t(s, b->orientation.offset.z);
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION:
        obj_classic_write_skin_block_constraint_up_vector(&b->position.up_vector,
            s, strings, string_offsets);
        obj_classic_write_skin_block_constraint_attach_point(&b->position.constrained_object,
            s, strings, string_offsets);
        obj_classic_write_skin_block_constraint_attach_point(&b->position.constraining_object,
            s, strings, string_offsets);
        break;
    }
}

static void obj_classic_read_skin_block_constraint_attach_point(obj_skin_block_constraint_attach_point* ap,
    stream* s, string* str) {
    ap->affected_by_orientation = io_read_int32_t(s) != 0;
    ap->affected_by_scaling = io_read_int32_t(s) != 0;
    ap->offset.x = io_read_float_t(s);
    ap->offset.y = io_read_float_t(s);
    ap->offset.z = io_read_float_t(s);
}

static void obj_classic_write_skin_block_constraint_attach_point(obj_skin_block_constraint_attach_point* ap,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets) {
    io_write_int32_t(s, ap->affected_by_orientation ? 1 : 0);
    io_write_int32_t(s, ap->affected_by_scaling ? 1 : 0);
    io_write_float_t(s, ap->offset.x);
    io_write_float_t(s, ap->offset.y);
    io_write_float_t(s, ap->offset.z);
}

static void obj_classic_read_skin_block_constraint_up_vector(obj_skin_block_constraint_up_vector* up,
    stream* s, string* str) {
    up->active = io_read_int32_t(s) != 0;
    up->roll = io_read_float_t(s);
    up->affected_axis.x = io_read_float_t(s);
    up->affected_axis.y = io_read_float_t(s);
    up->affected_axis.z = io_read_float_t(s);
    up->point_at.x = io_read_float_t(s);
    up->point_at.y = io_read_float_t(s);
    up->point_at.z = io_read_float_t(s);

    int32_t name = io_read_int32_t(s);
    io_read_string_null_terminated_offset(s, name, &up->name);
}

static void obj_classic_write_skin_block_constraint_up_vector(obj_skin_block_constraint_up_vector* up,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets) {
    io_write_int32_t(s, up->active ? 1 : 0);
    io_write_float_t(s, up->roll);
    io_write_float_t(s, up->affected_axis.x);
    io_write_float_t(s, up->affected_axis.y);
    io_write_float_t(s, up->affected_axis.z);
    io_write_float_t(s, up->point_at.x);
    io_write_float_t(s, up->point_at.y);
    io_write_float_t(s, up->point_at.z);

    ssize_t name = obj_skin_strings_get_string_offset(strings,
        string_offsets, string_data(&up->name));
    io_write_int32_t(s, (int32_t)name);
}

static void obj_classic_read_skin_block_expression(obj_skin_block_expression* b,
    stream* s, string* str) {
    obj_classic_read_skin_block_node(&b->base, s, str);

    int32_t name = io_read_int32_t(s);
    io_read_string_null_terminated_offset(s, name, &b->base.name);

    b->expression_count = io_read_int32_t(s);
    b->expression_count = min(b->expression_count, 9);
    for (int32_t i = 0; i < b->expression_count; i++) {
        int32_t expression = io_read_int32_t(s);
        if (expression)
            io_read_string_null_terminated_offset(s, expression, &b->expressions[i]);
    }

    for (int32_t i = b->expression_count; i < 9; i++)
        memset(&b->expressions[i], 0, sizeof(string));
}

static void obj_classic_write_skin_block_expression(obj_skin_block_expression* b,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets) {
    obj_classic_write_skin_block_node(&b->base, s, strings, string_offsets);

    ssize_t name = obj_skin_strings_get_string_offset(strings,
        string_offsets, string_data(&b->base.name));
    io_write_int32_t(s, (int32_t)name);

    io_write_int32_t(s, min(b->expression_count, 9));
    for (int32_t i = 0; i < b->expression_count && i < 9; i++) {
        ssize_t expression = obj_skin_strings_get_string_offset(strings,
            string_offsets, string_data(&b->expressions[i]));
        io_write_int32_t(s, (int32_t)expression);
    }

    for (int32_t i = b->expression_count; i < 9; i++)
        io_write_int32_t(s, 0);
}

static void obj_classic_read_skin_block_motion(obj_skin_block_motion* b,
    stream* s, string* str) {
    obj_classic_read_skin_block_node(&b->base, s, str);

    int32_t name_offset = io_read_int32_t(s);
    int32_t count = io_read_int32_t(s);
    int32_t bone_names_offset = io_read_int32_t(s);
    int32_t bone_matrices_offset = io_read_int32_t(s);

    io_read_string_null_terminated_offset(s, name_offset, &b->base.name);

    obj_skin_motion_node node;
    memset(&node, 0, sizeof(obj_skin_motion_node));

    b->nodes = vector_empty(obj_skin_motion_node);
    vector_obj_skin_motion_node_reserve(&b->nodes, count);

    if (bone_names_offset) {
        io_position_push(s, bone_names_offset, SEEK_SET);
        for (int32_t i = 0; i < count; i++) {
            uint32_t name = io_read_uint32_t(s);
            if (name & 0x8000)
                string_copy(&str[name & 0x7FFF], &node.name);
            else
                memset(&node.name, 0, sizeof(string));
            vector_obj_skin_motion_node_push_back(&b->nodes, &node);
        }
        io_position_pop(s);
    }

    if (bone_matrices_offset) {
        io_position_push(s, bone_matrices_offset, SEEK_SET);
        for (int32_t i = 0; i < count; i++) {
            mat4 mat;
            mat.row0.x = io_read_float_t(s);
            mat.row0.y = io_read_float_t(s);
            mat.row0.z = io_read_float_t(s);
            mat.row0.w = io_read_float_t(s);
            mat.row1.x = io_read_float_t(s);
            mat.row1.y = io_read_float_t(s);
            mat.row1.z = io_read_float_t(s);
            mat.row1.w = io_read_float_t(s);
            mat.row2.x = io_read_float_t(s);
            mat.row2.y = io_read_float_t(s);
            mat.row2.z = io_read_float_t(s);
            mat.row2.w = io_read_float_t(s);
            mat.row3.x = io_read_float_t(s);
            mat.row3.y = io_read_float_t(s);
            mat.row3.z = io_read_float_t(s);
            mat.row3.w = io_read_float_t(s);
            mat4_transpose(&mat, &b->nodes.begin[i].transformation);
        }
        io_position_pop(s);
    }
}

static void obj_classic_write_skin_block_motion(obj_skin_block_motion* b,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets,
    vector_string* string_set, ssize_t* bone_names_offset, ssize_t* bone_matrices_offset) {
    obj_classic_write_skin_block_node(&b->base, s, strings, string_offsets);

    ssize_t name = obj_skin_strings_get_string_offset(strings,
        string_offsets, string_data(&b->base.name));
    io_write_int32_t(s, (int32_t)name);
    io_write_int32_t(s, (int32_t)(b->nodes.end - b->nodes.begin));
    io_write_int32_t(s, (int32_t)*bone_names_offset);
    io_write_int32_t(s, (int32_t)*bone_matrices_offset);

    obj_skin_motion_node node;
    memset(&node, 0, sizeof(obj_skin_motion_node));

    if (b->nodes.end - b->nodes.begin) {
        io_position_push(s, *bone_names_offset, SEEK_SET);
        for (obj_skin_motion_node* i = b->nodes.begin; i != b->nodes.end; i++)
            io_write_uint32_t(s, obj_skin_strings_get_string_index(string_set, string_data(&i->name)));
        io_position_pop(s);
        *bone_names_offset += (b->nodes.end - b->nodes.begin) * sizeof(uint32_t);

        io_position_push(s, *bone_matrices_offset, SEEK_SET);
        for (obj_skin_motion_node* i = b->nodes.begin; i != b->nodes.end; i++) {
            mat4 mat;
            mat4_transpose(&i->transformation, &mat);
            io_write_float_t(s, mat.row0.x);
            io_write_float_t(s, mat.row0.y);
            io_write_float_t(s, mat.row0.z);
            io_write_float_t(s, mat.row0.w);
            io_write_float_t(s, mat.row1.x);
            io_write_float_t(s, mat.row1.y);
            io_write_float_t(s, mat.row1.z);
            io_write_float_t(s, mat.row1.w);
            io_write_float_t(s, mat.row2.x);
            io_write_float_t(s, mat.row2.y);
            io_write_float_t(s, mat.row2.z);
            io_write_float_t(s, mat.row2.w);
            io_write_float_t(s, mat.row3.x);
            io_write_float_t(s, mat.row3.y);
            io_write_float_t(s, mat.row3.z);
            io_write_float_t(s, mat.row3.w);
        }
        io_position_pop(s);
        *bone_matrices_offset += (b->nodes.end - b->nodes.begin) * sizeof(mat4);
    }
}

static void obj_classic_read_skin_block_node(obj_skin_block_node* b,
    stream* s, string* str) {
    int32_t parent_name = io_read_int32_t(s);
    io_read_string_null_terminated_offset(s, parent_name, &b->parent_name);

    b->position.x = io_read_float_t(s);
    b->position.y = io_read_float_t(s);
    b->position.z = io_read_float_t(s);
    b->rotation.x = io_read_float_t(s);
    b->rotation.y = io_read_float_t(s);
    b->rotation.z = io_read_float_t(s);
    b->scale.x = io_read_float_t(s);
    b->scale.y = io_read_float_t(s);
    b->scale.z = io_read_float_t(s);
}

static void obj_classic_write_skin_block_node(obj_skin_block_node* b,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets) {
    ssize_t parent_name = obj_skin_strings_get_string_offset(strings,
        string_offsets, string_data(&b->parent_name));
    io_write_int32_t(s, (int32_t)parent_name);

    io_write_float_t(s, b->position.x);
    io_write_float_t(s, b->position.y);
    io_write_float_t(s, b->position.z);
    io_write_float_t(s, b->rotation.x);
    io_write_float_t(s, b->rotation.y);
    io_write_float_t(s, b->rotation.z);
    io_write_float_t(s, b->scale.x);
    io_write_float_t(s, b->scale.y);
    io_write_float_t(s, b->scale.z);
}

static void obj_classic_read_skin_block_osage(obj_skin_block_osage* b,
    stream* s, string* str) {
    obj_classic_read_skin_block_node(&b->base, s, str);

    b->start_index = io_read_int32_t(s);
    b->count = io_read_int32_t(s);
    uint32_t external_name = io_read_uint32_t(s);
    if (external_name & 0x8000)
        string_copy(&str[external_name & 0x7FFF], &b->external_name);
    else
        memset(&b->external_name, 0, sizeof(string));

    uint32_t name = io_read_uint32_t(s);
    if (name & 0x8000)
        string_copy(&str[name & 0x7FFF], &b->base.name);
    else
        memset(&b->base.name, 0, sizeof(string));

    int32_t offset = io_read_int32_t(s);
    io_read(s, 0, 0x14);

    b->nodes = vector_empty(obj_skin_osage_node);
    if (offset) {
        vector_obj_skin_osage_node_reserve(&b->nodes, b->count);
        io_position_push(s, offset, SEEK_SET);
        if (io_read_uint32_t(s)) {
            io_set_position(s, offset, SEEK_SET);
            obj_skin_osage_node osage_node;
            memset(&osage_node, 0, sizeof(obj_skin_osage_node));

            for (int32_t i = 0; i < b->count; i++) {
                uint32_t name = io_read_uint32_t(s);
                if (name & 0x8000)
                    string_copy(&str[name & 0x7FFF], &osage_node.name);
                else
                    memset(&osage_node.name, 0, sizeof(string));

                osage_node.length = io_read_float_t(s);
                osage_node.rotation.x = io_read_float_t(s);
                osage_node.rotation.y = io_read_float_t(s);
                osage_node.rotation.z = io_read_float_t(s);
                vector_obj_skin_osage_node_push_back(&b->nodes, &osage_node);
            }
        }
    }
    io_position_pop(s);
}

static void obj_classic_write_skin_block_osage(obj_skin_block_osage* b,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets,
    vector_string* string_set, ssize_t* nodes_offset) {
    obj_classic_write_skin_block_node(&b->base, s, strings, string_offsets);

    io_write_int32_t(s, b->start_index);
    io_write_int32_t(s, b->count);
    io_write_uint32_t(s, obj_skin_strings_get_string_index(string_set, string_data(&b->external_name)));
    io_write_uint32_t(s, obj_skin_strings_get_string_index(string_set, string_data(&b->base.name)));

    io_write_int32_t(s, (int32_t)*nodes_offset);
    io_write(s, 0, 0x14);

    if (b->nodes.end - b->nodes.begin) {
        io_position_push(s, *nodes_offset, SEEK_SET);
        for (obj_skin_osage_node* i = b->nodes.begin; i != b->nodes.end; i++) {
            io_write_uint32_t(s, obj_skin_strings_get_string_index(string_set, string_data(&i->name)));
            io_write_float_t(s, i->length);
            io_write_float_t(s, i->rotation.x);
            io_write_float_t(s, i->rotation.y);
            io_write_float_t(s, i->rotation.z);
        }
        io_position_pop(s);
        *nodes_offset += (b->nodes.end - b->nodes.begin) * (sizeof(uint32_t) + sizeof(float_t) * 4);
    }
}

static void obj_classic_read_vertex(obj* o, stream* s, ssize_t* attrib_offsets, obj_mesh* mesh,
    ssize_t base_offset, int32_t vertex_count, obj_vertex_attrib_type attrib_type) {

    obj_vertex_flags vertex_flags = OBJ_VERTEX_NONE;
    if (attrib_type & OBJ_VERTEX_ATTRIB_POSITION)
        vertex_flags |= OBJ_VERTEX_POSITION;

    if (attrib_type & OBJ_VERTEX_ATTRIB_NORMAL)
        vertex_flags |= OBJ_VERTEX_NORMAL;

    if (attrib_type & OBJ_VERTEX_ATTRIB_TANGENT)
        vertex_flags |= OBJ_VERTEX_TANGENT;

    if (attrib_type & OBJ_VERTEX_ATTRIB_BINORMAL)
        vertex_flags |= OBJ_VERTEX_BINORMAL;

    if (attrib_type & OBJ_VERTEX_ATTRIB_TEXCOORD0)
        vertex_flags |= OBJ_VERTEX_TEXCOORD0;

    if (attrib_type & OBJ_VERTEX_ATTRIB_TEXCOORD1)
        vertex_flags |= OBJ_VERTEX_TEXCOORD1;

    if (attrib_type & OBJ_VERTEX_ATTRIB_TEXCOORD2)
        vertex_flags |= OBJ_VERTEX_TEXCOORD2;

    if (attrib_type & OBJ_VERTEX_ATTRIB_TEXCOORD3)
        vertex_flags |= OBJ_VERTEX_TEXCOORD3;

    if (attrib_type & OBJ_VERTEX_ATTRIB_COLOR0)
        vertex_flags |= OBJ_VERTEX_COLOR0;

    if (attrib_type & OBJ_VERTEX_ATTRIB_COLOR1)
        vertex_flags |= OBJ_VERTEX_COLOR1;

    if ((attrib_type & (OBJ_VERTEX_ATTRIB_BONE_WEIGHT | OBJ_VERTEX_ATTRIB_BONE_INDEX))
        == (OBJ_VERTEX_ATTRIB_BONE_WEIGHT | OBJ_VERTEX_ATTRIB_BONE_INDEX))
        vertex_flags |= OBJ_VERTEX_BONE_DATA;
    else
        attrib_type &= ~(OBJ_VERTEX_ATTRIB_BONE_WEIGHT | OBJ_VERTEX_ATTRIB_BONE_INDEX);

    if (attrib_type & OBJ_VERTEX_ATTRIB_UNKNOWN)
        vertex_flags |= OBJ_VERTEX_UNKNOWN;

    obj_vertex_data* vtx = force_malloc_s(obj_vertex_data, vertex_count);
    for (int32_t i = 0; i < 20; i++) {
        obj_vertex_attrib_type attribute = (obj_vertex_attrib_type)(1 << i);
        if (~attrib_type & attribute)
            continue;

        io_set_position(s, base_offset + attrib_offsets[i], SEEK_SET);
        switch (attribute) {
        case OBJ_VERTEX_ATTRIB_POSITION:
            for (int32_t j = 0; j < vertex_count; j++) {
                vtx[j].position.x = io_read_float_t(s);
                vtx[j].position.y = io_read_float_t(s);
                vtx[j].position.z = io_read_float_t(s);
            }
            break;
        case OBJ_VERTEX_ATTRIB_NORMAL:
            for (int32_t j = 0; j < vertex_count; j++) {
                vtx[j].normal.x = io_read_float_t(s);
                vtx[j].normal.y = io_read_float_t(s);
                vtx[j].normal.z = io_read_float_t(s);
            }
            break;
        case OBJ_VERTEX_ATTRIB_TANGENT:
            for (int32_t j = 0; j < vertex_count; j++) {
                vtx[j].tangent.x = io_read_float_t(s);
                vtx[j].tangent.y = io_read_float_t(s);
                vtx[j].tangent.z = io_read_float_t(s);
                vtx[j].tangent.w = io_read_float_t(s);
            }
            break;
        case OBJ_VERTEX_ATTRIB_BINORMAL:
            for (int32_t j = 0; j < vertex_count; j++) {
                vtx[j].binormal.x = io_read_float_t(s);
                vtx[j].binormal.y = io_read_float_t(s);
                vtx[j].binormal.z = io_read_float_t(s);
            }
            break;
        case OBJ_VERTEX_ATTRIB_TEXCOORD0:
            for (int32_t j = 0; j < vertex_count; j++) {
                vtx[j].texcoord0.x = io_read_float_t(s);
                vtx[j].texcoord0.y = io_read_float_t(s);
            }
            break;
        case OBJ_VERTEX_ATTRIB_TEXCOORD1:
            for (int32_t j = 0; j < vertex_count; j++) {
                vtx[j].texcoord1.x = io_read_float_t(s);
                vtx[j].texcoord1.y = io_read_float_t(s);
            }
            break;
        case OBJ_VERTEX_ATTRIB_TEXCOORD2:
            for (int32_t j = 0; j < vertex_count; j++) {
                vtx[j].texcoord2.x = io_read_float_t(s);
                vtx[j].texcoord2.y = io_read_float_t(s);
            }
            break;
        case OBJ_VERTEX_ATTRIB_TEXCOORD3:
            for (int32_t j = 0; j < vertex_count; j++) {
                vtx[j].texcoord3.x = io_read_float_t(s);
                vtx[j].texcoord3.y = io_read_float_t(s);
            }
            break;
        case OBJ_VERTEX_ATTRIB_COLOR0:
            for (int32_t j = 0; j < vertex_count; j++) {
                vtx[j].color0.x = io_read_float_t(s);
                vtx[j].color0.y = io_read_float_t(s);
                vtx[j].color0.z = io_read_float_t(s);
                vtx[j].color0.w = io_read_float_t(s);
            }
            break;
        case OBJ_VERTEX_ATTRIB_COLOR1:
            for (int32_t j = 0; j < vertex_count; j++) {
                vtx[j].color1.x = io_read_float_t(s);
                vtx[j].color1.y = io_read_float_t(s);
                vtx[j].color1.z = io_read_float_t(s);
                vtx[j].color1.w = io_read_float_t(s);
            }
            break;
        case OBJ_VERTEX_ATTRIB_BONE_WEIGHT:
            for (int32_t j = 0; j < vertex_count; j++) {
                vtx[j].bone_weight.x = io_read_float_t(s);
                vtx[j].bone_weight.y = io_read_float_t(s);
                vtx[j].bone_weight.z = io_read_float_t(s);
                vtx[j].bone_weight.w = io_read_float_t(s);
            }
            break;
        case OBJ_VERTEX_ATTRIB_BONE_INDEX:
            for (int32_t j = 0; j < vertex_count; j++) {
                vec4i bone_index;
                bone_index.x = (int32_t)io_read_float_t(s);
                bone_index.y = (int32_t)io_read_float_t(s);
                bone_index.z = (int32_t)io_read_float_t(s);
                bone_index.w = (int32_t)io_read_float_t(s);
                if (bone_index.x == 0xFF)
                    bone_index.x = -1;
                if (bone_index.y == 0xFF)
                    bone_index.y = -1;
                if (bone_index.z == 0xFF)
                    bone_index.z = -1;
                if (bone_index.w == 0xFF)
                    bone_index.w = -1;
                vtx[j].bone_index = bone_index;
            }
            break;
        case OBJ_VERTEX_ATTRIB_UNKNOWN:
            for (int32_t j = 0; j < vertex_count; j++) {
                vtx[j].unknown.x = io_read_float_t(s);
                vtx[j].unknown.y = io_read_float_t(s);
                vtx[j].unknown.z = io_read_float_t(s);
                vtx[j].unknown.w = io_read_float_t(s);
            }
            break;
        }
    }
    mesh->vertex = vtx;
    mesh->vertex_count = vertex_count;
    mesh->vertex_flags = vertex_flags;
}

static void obj_classic_write_vertex(obj* o, stream* s, ssize_t* attrib_offsets, obj_mesh* mesh,
    ssize_t base_offset, int32_t* vertex_count, obj_vertex_attrib_type* attrib_type, int32_t* vertex_size) {
    obj_vertex_data* vtx = mesh->vertex;
    int32_t _vertex_count = mesh->vertex_count;
    obj_vertex_flags vertex_flags = mesh->vertex_flags;

    obj_vertex_attrib_type _attrib_type = 0;
    int32_t _vertex_size = 0;
    if (vertex_flags & OBJ_VERTEX_POSITION) {
        _attrib_type |= OBJ_VERTEX_ATTRIB_POSITION;
        _vertex_size += 0x0C;
    }

    if (vertex_flags & OBJ_VERTEX_NORMAL) {
        _attrib_type |= OBJ_VERTEX_ATTRIB_NORMAL;
        _vertex_size += 0x0C;
    }

    if (vertex_flags & OBJ_VERTEX_TANGENT) {
        _attrib_type |= OBJ_VERTEX_ATTRIB_TANGENT;
        _vertex_size += 0x10;
    }

    if (vertex_flags & OBJ_VERTEX_BINORMAL) {
        _attrib_type |= OBJ_VERTEX_ATTRIB_BINORMAL;
        _vertex_size += 0x0C;
    }

    if (vertex_flags & OBJ_VERTEX_TEXCOORD0) {
        _attrib_type |= OBJ_VERTEX_ATTRIB_TEXCOORD0;
        _vertex_size += 0x08;
    }

    if (vertex_flags & OBJ_VERTEX_TEXCOORD1) {
        _attrib_type |= OBJ_VERTEX_ATTRIB_TEXCOORD1;
        _vertex_size += 0x08;
    }

    if (vertex_flags & OBJ_VERTEX_TEXCOORD2) {
        _attrib_type |= OBJ_VERTEX_ATTRIB_TEXCOORD2;
        _vertex_size += 0x08;
    }

    if (vertex_flags & OBJ_VERTEX_TEXCOORD3) {
        _attrib_type |= OBJ_VERTEX_ATTRIB_TEXCOORD3;
        _vertex_size += 0x08;
    }

    if (vertex_flags & OBJ_VERTEX_COLOR0) {
        _attrib_type |= OBJ_VERTEX_ATTRIB_COLOR0;
        _vertex_size += 0x10;
    }

    if (vertex_flags & OBJ_VERTEX_COLOR1) {
        _attrib_type |= OBJ_VERTEX_ATTRIB_COLOR1;
        _vertex_size += 0x10;
    }

    if (vertex_flags & OBJ_VERTEX_BONE_DATA) {
        _attrib_type |= OBJ_VERTEX_ATTRIB_BONE_WEIGHT | OBJ_VERTEX_ATTRIB_BONE_INDEX;
        _vertex_size += 0x20;
    }

    if (vertex_flags & OBJ_VERTEX_UNKNOWN) {
        _attrib_type |= OBJ_VERTEX_ATTRIB_UNKNOWN;
        _vertex_size += 0x10;
    }
    *vertex_count = _vertex_count;
    *attrib_type = _attrib_type;
    *vertex_size = _vertex_size;

    if (_attrib_type & OBJ_VERTEX_ATTRIB_POSITION) {
        attrib_offsets[0] = io_get_position(s) - base_offset;
        for (int32_t i = 0; i < _vertex_count; i++) {
            io_write_float_t(s, vtx[i].position.x);
            io_write_float_t(s, vtx[i].position.y);
            io_write_float_t(s, vtx[i].position.z);
        }
    }

    if (_attrib_type & OBJ_VERTEX_ATTRIB_NORMAL) {
        attrib_offsets[1] = io_get_position(s) - base_offset;
        for (int32_t i = 0; i < _vertex_count; i++) {
            io_write_float_t(s, vtx[i].normal.x);
            io_write_float_t(s, vtx[i].normal.y);
            io_write_float_t(s, vtx[i].normal.z);
        }
    }

    if (_attrib_type & OBJ_VERTEX_ATTRIB_TANGENT) {
        attrib_offsets[2] = io_get_position(s) - base_offset;
        for (int32_t i = 0; i < _vertex_count; i++) {
            io_write_float_t(s, vtx[i].tangent.x);
            io_write_float_t(s, vtx[i].tangent.y);
            io_write_float_t(s, vtx[i].tangent.z);
            io_write_float_t(s, vtx[i].tangent.w);
        }
    }

    if (_attrib_type & OBJ_VERTEX_ATTRIB_BINORMAL) {
        attrib_offsets[3] = io_get_position(s) - base_offset;
        for (int32_t i = 0; i < _vertex_count; i++) {
            io_write_float_t(s, vtx[i].binormal.x);
            io_write_float_t(s, vtx[i].binormal.y);
            io_write_float_t(s, vtx[i].binormal.z);
        }
    }

    if (_attrib_type & OBJ_VERTEX_ATTRIB_TEXCOORD0) {
        attrib_offsets[4] = io_get_position(s) - base_offset;
        for (int32_t i = 0; i < _vertex_count; i++) {
            io_write_float_t(s, vtx[i].texcoord0.x);
            io_write_float_t(s, vtx[i].texcoord0.y);
        }
    }

    if (_attrib_type & OBJ_VERTEX_ATTRIB_BONE_WEIGHT) {
        attrib_offsets[10] = io_get_position(s) - base_offset;
        for (int32_t i = 0; i < _vertex_count; i++) {
            io_write_float_t(s, vtx[i].bone_weight.x);
            io_write_float_t(s, vtx[i].bone_weight.y);
            io_write_float_t(s, vtx[i].bone_weight.z);
            io_write_float_t(s, vtx[i].bone_weight.w);
        }
    }

    if (_attrib_type & OBJ_VERTEX_ATTRIB_BONE_INDEX) {
        attrib_offsets[11] = io_get_position(s) - base_offset;
        for (int32_t i = 0; i < _vertex_count; i++) {
            vec4 bone_index;
            vec4i_to_vec4(vtx[i].bone_index, bone_index);
            io_write_float_t(s, bone_index.x);
            io_write_float_t(s, bone_index.y);
            io_write_float_t(s, bone_index.z);
            io_write_float_t(s, bone_index.w);
        }
    }

    if (_attrib_type & OBJ_VERTEX_ATTRIB_TEXCOORD1) {
        attrib_offsets[5] = io_get_position(s) - base_offset;
        for (int32_t i = 0; i < _vertex_count; i++) {
            io_write_float_t(s, vtx[i].texcoord1.x);
            io_write_float_t(s, vtx[i].texcoord1.y);
        }
    }

    if (_attrib_type & OBJ_VERTEX_ATTRIB_TEXCOORD2) {
        attrib_offsets[6] = io_get_position(s) - base_offset;
        for (int32_t i = 0; i < _vertex_count; i++) {
            io_write_float_t(s, vtx[i].texcoord2.x);
            io_write_float_t(s, vtx[i].texcoord2.y);
        }
    }

    if (_attrib_type & OBJ_VERTEX_ATTRIB_TEXCOORD3) {
        attrib_offsets[7] = io_get_position(s) - base_offset;
        for (int32_t i = 0; i < _vertex_count; i++) {
            io_write_float_t(s, vtx[i].texcoord3.x);
            io_write_float_t(s, vtx[i].texcoord3.y);
        }
    }

    if (_attrib_type & OBJ_VERTEX_ATTRIB_COLOR0) {
        attrib_offsets[8] = io_get_position(s) - base_offset;
        for (int32_t i = 0; i < _vertex_count; i++) {
            io_write_float_t(s, vtx[i].color0.x);
            io_write_float_t(s, vtx[i].color0.y);
            io_write_float_t(s, vtx[i].color0.z);
            io_write_float_t(s, vtx[i].color0.w);
        }
    }

    if (_attrib_type & OBJ_VERTEX_ATTRIB_COLOR1) {
        attrib_offsets[9] = io_get_position(s) - base_offset;
        for (int32_t i = 0; i < _vertex_count; i++) {
            io_write_float_t(s, vtx[i].color1.x);
            io_write_float_t(s, vtx[i].color1.y);
            io_write_float_t(s, vtx[i].color1.z);
            io_write_float_t(s, vtx[i].color1.w);
        }
    }

    if (_attrib_type & OBJ_VERTEX_ATTRIB_UNKNOWN) {
        attrib_offsets[12] = io_get_position(s) - base_offset;
        for (int32_t i = 0; i < _vertex_count; i++) {
            io_write_float_t(s, vtx[i].unknown.x);
            io_write_float_t(s, vtx[i].unknown.y);
            io_write_float_t(s, vtx[i].unknown.z);
            io_write_float_t(s, vtx[i].unknown.w);
        }
    }
}

static void obj_modern_read_inner(obj_set* os, stream* s) {
    f2_struct st;
    f2_struct_sread(&st, s);
    if (st.header.signature == reverse_endianness_uint32_t('MOSD') && st.data) {
        stream s_mosd;
        io_mopen(&s_mosd, st.data, st.length);
        s_mosd.is_big_endian = st.header.use_big_endian;

        uint32_t signature = io_read_uint32_t_stream_reverse_endianness(&s_mosd);
        if (signature != 0x5062501) {
            os->is_x = false;
            os->modern = false;
            os->ready = false;
            return;
        }

        bool is_x = true;
        io_set_position(&s_mosd, 0x00C, SEEK_SET);
        is_x &= io_read_uint32_t_stream_reverse_endianness(&s_mosd) == 0;
        io_set_position(&s_mosd, 0x014, SEEK_SET);
        is_x &= io_read_uint32_t_stream_reverse_endianness(&s_mosd) == 0;
        io_set_position(&s_mosd, 0x01C, SEEK_SET);
        is_x &= io_read_uint32_t_stream_reverse_endianness(&s_mosd) == 0;
        io_set_position(&s_mosd, 0x024, SEEK_SET);
        is_x &= io_read_uint32_t_stream_reverse_endianness(&s_mosd) == 0;
        io_set_position(&s_mosd, 0x02C, SEEK_SET);
        is_x &= io_read_uint32_t_stream_reverse_endianness(&s_mosd) == 0;

        io_set_position(&s_mosd, 0x04, SEEK_SET);
        obj_set_header osh;
        memset(&osh, 0, sizeof(obj_set_header));
        if (!is_x) {
            osh.objects_count = io_read_int32_t_stream_reverse_endianness(&s_mosd);
            osh.max_object_id = io_read_int32_t_stream_reverse_endianness(&s_mosd);
            osh.objects_offset = io_read_offset_f2(&s_mosd, st.header.length);
            osh.object_skins_offset = io_read_offset_f2(&s_mosd, st.header.length);
            osh.object_names_offset = io_read_offset_f2(&s_mosd, st.header.length);
            osh.object_ids_offset = io_read_offset_f2(&s_mosd, st.header.length);
            osh.texture_ids_offset = io_read_offset_f2(&s_mosd, st.header.length);
            osh.texture_ids_count = io_read_int32_t_stream_reverse_endianness(&s_mosd);
        }
        else {
            osh.objects_count = io_read_int32_t_stream_reverse_endianness(&s_mosd);
            osh.max_object_id = io_read_int32_t_stream_reverse_endianness(&s_mosd);
            osh.objects_offset = io_read_offset_x(&s_mosd);
            osh.object_skins_offset = io_read_offset_x(&s_mosd);
            osh.object_names_offset = io_read_offset_x(&s_mosd);
            osh.object_ids_offset = io_read_offset_x(&s_mosd);
            osh.texture_ids_offset = io_read_offset_x(&s_mosd);
            osh.texture_ids_count = io_read_int32_t_stream_reverse_endianness(&s_mosd);
        }

        int32_t count = osh.objects_count;
        os->vec = vector_empty(obj);
        vector_obj_reserve(&os->vec, osh.objects_count);

        ssize_t* objects_offsets = 0;
        if (osh.objects_offset) {
            objects_offsets = force_malloc_s(ssize_t, count);
            io_set_position(&s_mosd, osh.objects_offset, SEEK_SET);
            if (!is_x)
                for (int32_t i = 0; i < count; i++)
                    objects_offsets[i] = io_read_offset_f2(&s_mosd, st.header.length);
            else
                for (int32_t i = 0; i < count; i++)
                    objects_offsets[i] = io_read_offset_x(&s_mosd);
        }

        ssize_t* object_skins_offsets = 0;
        if (osh.object_skins_offset) {
            object_skins_offsets = force_malloc_s(ssize_t, count);
            io_set_position(&s_mosd, osh.object_skins_offset, SEEK_SET);
            if (!is_x)
                for (int32_t i = 0; i < count; i++)
                    object_skins_offsets[i] = io_read_offset_f2(&s_mosd, st.header.length);
            else
                for (int32_t i = 0; i < count; i++)
                    object_skins_offsets[i] = io_read_offset_x(&s_mosd);
        }

        ssize_t* object_names_offsets = 0;
        if (osh.object_names_offset) {
            object_names_offsets = force_malloc_s(ssize_t, count);
            io_set_position(&s_mosd, osh.object_names_offset, SEEK_SET);
            if (!is_x)
                for (int32_t i = 0; i < count; i++)
                    object_names_offsets[i] = io_read_offset_f2(&s_mosd, st.header.length);
            else
                for (int32_t i = 0; i < count; i++)
                    object_names_offsets[i] = io_read_offset_x(&s_mosd);
        }

        if (osh.objects_offset)
            for (int32_t i = 0; i < count; i++) {
                obj o;
                memset(&o, 0, sizeof(obj));

                if (osh.object_names_offset && object_names_offsets[i])
                    io_read_string_null_terminated_offset(&s_mosd,
                        object_names_offsets[i], &o.name);

                vector_obj_push_back(&os->vec, &o);
            }


        if (osh.object_ids_offset) {
            io_set_position(&s_mosd, osh.object_ids_offset, SEEK_SET);
            for (int32_t i = 0; i < count; i++)
                os->vec.begin[i].id = io_read_uint32_t_stream_reverse_endianness(&s_mosd);
        }

        free(objects_offsets);
        free(object_skins_offsets);
        free(object_names_offsets);

        if (osh.texture_ids_offset) {
            io_set_position(&s_mosd, osh.texture_ids_offset, SEEK_SET);
            int32_t count = osh.texture_ids_count;
            os->tex_ids = vector_empty(uint32_t);
            vector_uint32_t_reserve(&os->tex_ids, count);
            for (int32_t i = 0; i < count; i++) {
                uint32_t tex_id = io_read_uint32_t_stream_reverse_endianness(&s_mosd);
                vector_uint32_t_push_back(&os->tex_ids, &tex_id);
            }
        }

        int32_t omdl_index = 0;
        for (f2_struct* i = st.sub_structs.begin; i != st.sub_structs.end; i++) {
            if (i->header.signature != reverse_endianness_uint32_t('OMDL'))
                continue;

            f2_struct* oskn = 0;
            f2_struct* oidx = 0;
            f2_struct* ovtx = 0;
            for (f2_struct* j = i->sub_structs.begin; j != i->sub_structs.end; j++)
                if (!oskn && j->header.signature == reverse_endianness_uint32_t('OSKN'))
                    oskn = j;
                else if (!oidx && j->header.signature == reverse_endianness_uint32_t('OIDX'))
                    oidx = j;
                else if (!ovtx && j->header.signature == reverse_endianness_uint32_t('OVTX'))
                    ovtx = j;

            stream s_oskn;
            stream s_oidx;
            stream s_ovtx;
            stream* s_oskn_ptr = 0;
            stream* s_oidx_ptr = 0;
            stream* s_ovtx_ptr = 0;
            if (oskn) {
                io_mopen(&s_oskn, oskn->data, oskn->length);
                s_oskn.is_big_endian = oskn->header.use_big_endian;
                s_oskn_ptr = &s_oskn;
            }

            if (oidx) {
                io_mopen(&s_oidx, oidx->data, oidx->length);
                s_oidx.is_big_endian = oidx->header.use_big_endian;
                s_oidx_ptr = &s_oidx;
            }

            if (ovtx) {
                io_mopen(&s_ovtx, ovtx->data, ovtx->length);
                s_ovtx.is_big_endian = ovtx->header.use_big_endian;
                s_ovtx_ptr = &s_ovtx;
            }

            obj* o = &os->vec.begin[omdl_index];
            stream s_omdl;
            io_mopen(&s_omdl, i->data, i->length);
            s_omdl.is_big_endian = i->header.use_big_endian;
            obj_modern_read_model(o, &s_omdl, 0, i->header.length, is_x, s_oidx_ptr, s_ovtx_ptr);
            io_free(&s_omdl);

            if (s_oskn_ptr)
                obj_modern_read_skin(o, s_oskn_ptr, 0, oskn->header.length, is_x);

            if (s_oskn_ptr)
                io_free(&s_oskn);
            if (s_oidx_ptr)
                io_free(&s_oidx);
            if (s_ovtx_ptr)
                io_free(&s_ovtx);
            omdl_index++;
        }

        io_free(&s_mosd);

        os->is_x = is_x;
        os->modern = true;
        os->ready = true;
    }
    f2_struct_free(&st);
}

static void obj_modern_write_inner(obj_set* os, stream* s) {
    stream s_mosd;
    io_mopen(&s_mosd, 0, 0);
    uint32_t off;
    vector_enrs_entry e = vector_empty(enrs_entry);
    enrs_entry ee;
    vector_size_t pof = vector_empty(size_t);

    bool is_x = os->is_x;

    obj_set_header osh;
    memset(&osh, 0, sizeof(obj_set_header));
    osh.objects_count = (int32_t)(os->vec.end - os->vec.begin);
    osh.max_object_id = -1;
    osh.texture_ids_count = (int32_t)(os->tex_ids.end - os->tex_ids.begin);

    int32_t count = osh.objects_count;

    if (!is_x) {
        ee = (enrs_entry){ 0, 1, 44, 1, vector_empty(enrs_sub_entry) };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 9, ENRS_DWORD });
        vector_enrs_entry_push_back(&e, &ee);
        off = 44;
    }
    else {
        ee = (enrs_entry){ 0, 3, 72, 1, vector_empty(enrs_sub_entry) };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 3, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 4, 5, ENRS_QWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
        vector_enrs_entry_push_back(&e, &ee);
        off = 72;
        off = align_val(off, 0x10);
    }

    if (!is_x)
        off += (uint32_t)(count * 4LL);
    else
        off += (uint32_t)(count * 8LL);
    off = align_val(off, 0x10);

    if (!is_x)
        off += (uint32_t)(count * 4LL);
    else
        off += (uint32_t)(count * 8LL);
    off = align_val(off, 0x10);

    if (!is_x) {
        ee = (enrs_entry){ off, 1, 4, count, vector_empty(enrs_sub_entry) };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
        vector_enrs_entry_push_back(&e, &ee);
        off = (uint32_t)(count * 4LL);
    }
    else {
        ee = (enrs_entry){ off, 1, 8, count, vector_empty(enrs_sub_entry) };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_QWORD });
        vector_enrs_entry_push_back(&e, &ee);
        off = (uint32_t)(count * 8LL);
    }
    off = align_val(off, 0x10);

    ee = (enrs_entry){ off, 1, 4, count, vector_empty(enrs_sub_entry) };
    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
    vector_enrs_entry_push_back(&e, &ee);
    off = (uint32_t)(count * 4LL);
    off = align_val(off, 0x10);

    ee = (enrs_entry){ off, 1, 4, osh.texture_ids_count, vector_empty(enrs_sub_entry) };
    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
    vector_enrs_entry_push_back(&e, &ee);
    off = (uint32_t)(osh.texture_ids_count * 4LL);
    off = align_val(off, 0x10);

    io_set_position(&s_mosd, 0x00, SEEK_SET);
    if (!is_x) {
        io_write_int32_t(&s_mosd, 0);
        io_write_int32_t(&s_mosd, 0);
        io_write_int32_t(&s_mosd, 0);
        io_write_offset_f2_pof_add(&s_mosd, 0, 0x20, &pof);
        io_write_offset_f2_pof_add(&s_mosd, 0, 0x20, &pof);
        io_write_offset_f2_pof_add(&s_mosd, 0, 0x20, &pof);
        io_write_offset_f2_pof_add(&s_mosd, 0, 0x20, &pof);
        io_write_offset_f2_pof_add(&s_mosd, 0, 0x20, &pof);
        io_write_int32_t(&s_mosd, 0);
        io_write_int32_t(&s_mosd, 0);
        io_write_int32_t(&s_mosd, 0);
    }
    else {
        io_write_int32_t(&s_mosd, 0);
        io_write_int32_t(&s_mosd, 0);
        io_write_int32_t(&s_mosd, 0);
        io_write_offset_x_pof_add(&s_mosd, 0, &pof);
        io_write_offset_x_pof_add(&s_mosd, 0, &pof);
        io_write_offset_x_pof_add(&s_mosd, 0, &pof);
        io_write_offset_x_pof_add(&s_mosd, 0, &pof);
        io_write_offset_x_pof_add(&s_mosd, 0, &pof);
        io_write_int32_t(&s_mosd, 0);
        io_write_int32_t(&s_mosd, 0);
        io_write_int32_t(&s_mosd, 0);
        io_align_write(&s_mosd, 0x10);
    }

    osh.objects_offset = io_get_position(&s_mosd);
    if (!is_x)
        for (int32_t i = 0; i < count; i++)
            io_write_offset_f2(&s_mosd, 0, 0x20);
    else
        for (int32_t i = 0; i < count; i++)
            io_write_offset_x(&s_mosd, 0);
    io_align_write(&s_mosd, 0x10);

    osh.object_skins_offset = io_get_position(&s_mosd);
    if (!is_x)
        for (int32_t i = 0; i < count; i++)
            io_write_offset_f2(&s_mosd, 0, 0x20);
    else
        for (int32_t i = 0; i < count; i++)
            io_write_offset_x(&s_mosd, 0);
    io_align_write(&s_mosd, 0x10);

    osh.object_names_offset = io_get_position(&s_mosd);
    if (!is_x)
        for (int32_t i = 0; i < count; i++)
            io_write_offset_f2_pof_add(&s_mosd, 0, 0x20, &pof);
    else
        for (int32_t i = 0; i < count; i++)
            io_write_offset_x_pof_add(&s_mosd, 0, &pof);
    io_align_write(&s_mosd, 0x10);

    osh.object_ids_offset = io_get_position(&s_mosd);
    for (int32_t i = 0; i < count; i++)
        io_write_uint32_t(&s_mosd, os->vec.begin[i].id);
    io_align_write(&s_mosd, 0x10);

    osh.texture_ids_count = (int32_t)(os->tex_ids.end - os->tex_ids.begin);
    osh.texture_ids_offset = io_get_position(&s_mosd);
    for (int32_t i = 0; i < osh.texture_ids_count; i++)
        io_write_uint32_t(&s_mosd, os->tex_ids.begin[i]);
    io_align_write(&s_mosd, 0x10);

    ssize_t* object_names_offsets = force_malloc_s(ssize_t, count);
    for (int32_t i = 0; i < count; i++) {
        object_names_offsets[i] = (int32_t)io_get_position(&s_mosd);
        io_write_string_null_terminated(&s_mosd, &os->vec.begin[i].name);
    }
    io_align_write(&s_mosd, 0x10);

    io_position_push(&s_mosd, osh.object_names_offset, SEEK_SET);
    if (!is_x)
        for (int32_t i = 0; i < count; i++)
            io_write_offset_f2(&s_mosd, object_names_offsets[i], 0x20);
    else
        for (int32_t i = 0; i < count; i++)
            io_write_offset_x(&s_mosd, object_names_offsets[i]);
    io_position_pop(&s_mosd);
    free(object_names_offsets);

    io_position_push(&s_mosd, 0x00, SEEK_SET);
    if (!is_x) {
        io_write_uint32_t(&s_mosd, 0x5062501);
        io_write_int32_t(&s_mosd, osh.objects_count);
        io_write_int32_t(&s_mosd, -1);
        io_write_offset_f2(&s_mosd, osh.objects_offset, 0x20);
        io_write_offset_f2(&s_mosd, osh.object_skins_offset, 0x20);
        io_write_offset_f2(&s_mosd, osh.object_names_offset, 0x20);
        io_write_offset_f2(&s_mosd, osh.object_ids_offset, 0x20);
        io_write_offset_f2(&s_mosd, osh.texture_ids_offset, 0x20);
        io_write_int32_t(&s_mosd, osh.texture_ids_count);
        io_write_int32_t(&s_mosd, 0);
        io_write_int32_t(&s_mosd, 0);
    }
    else {
        io_write_uint32_t(&s_mosd, 0x5062501);
        io_write_int32_t(&s_mosd, osh.objects_count);
        io_write_int32_t(&s_mosd, -1);
        io_write_offset_x(&s_mosd, osh.objects_offset);
        io_write_offset_x(&s_mosd, osh.object_skins_offset);
        io_write_offset_x(&s_mosd, osh.object_names_offset);
        io_write_offset_x(&s_mosd, osh.object_ids_offset);
        io_write_offset_x(&s_mosd, osh.texture_ids_offset);
        io_write_int32_t(&s_mosd, osh.texture_ids_count);
        io_write_int32_t(&s_mosd, 0);
        io_write_int32_t(&s_mosd, 0);
    }
    io_position_pop(&s_mosd);

    f2_struct st;
    memset(&st, 0, sizeof(f2_struct));
    for (int32_t i = 0; i < count; i++) {
        obj* o = &os->vec.begin[i];

        f2_struct omdl;
        memset(&omdl, 0, sizeof(f2_struct));

        stream s_omdl;
        io_mopen(&s_omdl, 0, 0);

        if (o->skin_init) {
            f2_struct oskn;
            memset(&oskn, 0, sizeof(f2_struct));

            stream s_oskn;
            io_mopen(&s_oskn, 0, 0);

            obj_modern_write_skin(o, &s_oskn, 0, is_x, &oskn);

            io_align_write(&s_oskn, 0x10);
            io_mcopy(&s_oskn, &oskn.data, &oskn.length);
            io_free(&s_oskn);

            oskn.header.signature = reverse_endianness_uint32_t('OSKN');
            oskn.header.length = 0x20;
            oskn.header.use_big_endian = false;
            oskn.header.use_section_size = true;

            vector_f2_struct_push_back(&omdl.sub_structs, &oskn);
        }

        obj_modern_write_model(o, &s_omdl, 0, is_x, &omdl);

        io_align_write(&s_omdl, 0x10);
        io_mcopy(&s_omdl, &omdl.data, &omdl.length);
        io_free(&s_omdl);

        omdl.header.signature = reverse_endianness_uint32_t('OMDL');
        omdl.header.length = 0x20;
        omdl.header.use_big_endian = false;
        omdl.header.use_section_size = true;
        vector_f2_struct_push_back(&st.sub_structs, &omdl);
    }

    io_align_write(&s_mosd, 0x10);
    io_mcopy(&s_mosd, &st.data, &st.length);
    io_free(&s_mosd);

    st.enrs = e;
    st.pof = pof;

    st.header.signature = reverse_endianness_uint32_t('MOSD');
    st.header.length = 0x20;
    st.header.use_big_endian = false;
    st.header.use_section_size = true;

    f2_struct_swrite(&st, s, true, os->is_x);
    f2_struct_free(&st);
}

static void obj_modern_read_index(obj* o, stream* s,
    obj_sub_mesh* sub_mesh, int32_t indices_count) {
    uint32_t* indices = force_malloc_s(uint32_t, indices_count);
    switch (sub_mesh->index_format) {
    case OBJ_INDEX_U8:
        for (int32_t i = 0; i < indices_count; i++) {
            uint8_t idx = io_read_uint8_t(s);
            indices[i] = idx == 0xFF ? 0xFFFFFFFF : idx;
        }
        break;
    case OBJ_INDEX_U16:
        for (int32_t i = 0; i < indices_count; i++) {
            uint16_t idx = io_read_uint16_t_stream_reverse_endianness(s);
            indices[i] = idx == 0xFFFF ? 0xFFFFFFFF : idx;
        }
        break;
    case OBJ_INDEX_U32:
        for (int32_t i = 0; i < indices_count; i++)
            indices[i] = io_read_uint32_t_stream_reverse_endianness(s);
        break;
    }
    sub_mesh->indices = indices;
    sub_mesh->indices_count = indices_count;
}

static void obj_modern_write_index(obj* o, stream* s, bool is_x,
    obj_sub_mesh* sub_mesh, int32_t* indices_count, f2_struct* oidx) {
    int32_t size = 1;
    enrs_type type = ENRS_INVALID;
    switch (sub_mesh->index_format) {
    case OBJ_INDEX_U16:
        size = 2;
        type = ENRS_WORD;
        break;
    case OBJ_INDEX_U32:
        size = 4;
        type = ENRS_DWORD;
        break;
    }

    uint32_t off = 0;
    vector_enrs_entry* e = &oidx->enrs;
    enrs_entry ee;
    bool add_enrs = true;
    if (e->end - e->begin > 0) {
        off = (uint32_t)((size_t)e->end[-1].size * e->end[-1].repeat_count);
        if (e->end[-1].count && e->end[-1].sub.begin[0].type == type) {
            e->end[-1].repeat_count += sub_mesh->indices_count;
            add_enrs = false;
        }
    }

    if (add_enrs)
        if (type != ENRS_INVALID) {
            ee = (enrs_entry){ off, 1, size, sub_mesh->indices_count, vector_empty(enrs_sub_entry) };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, type });
            vector_enrs_entry_push_back(e, &ee);
        }
        else {
            ee = (enrs_entry){ off, 0, size, sub_mesh->indices_count, vector_empty(enrs_sub_entry) };
            vector_enrs_entry_push_back(e, &ee);
        }

    uint32_t* indices = sub_mesh->indices;
    int32_t _indices_count = sub_mesh->indices_count;
    switch (sub_mesh->index_format) {
    case OBJ_INDEX_U8:
        for (int32_t i = 0; i < _indices_count; i++)
            io_write_uint8_t(s, (uint8_t)indices[i]);
        break;
    case OBJ_INDEX_U16:
        for (int32_t i = 0; i < _indices_count; i++)
            io_write_uint16_t(s, (uint16_t)indices[i]);
        break;
    case OBJ_INDEX_U32:
        for (int32_t i = 0; i < _indices_count; i++)
            io_write_uint32_t(s, indices[i]);
        break;
    }
    io_align_write(s, 0x04);
    *indices_count = _indices_count;
}

static void obj_modern_read_model(obj* o, stream* s, ssize_t base_offset,
    uint32_t header_length, bool is_x, stream* s_oidx, stream* s_ovtx) {
    const size_t mesh_size = is_x ? 0x130 : 0xD8;
    const size_t sub_mesh_size = is_x ? 0x80 : 0x70;

    io_set_position(s, base_offset, SEEK_SET);

    obj_header oh;
    memset(&oh, 0, sizeof(obj_header));
    if (!is_x) {
        io_read(s, 0, 0x08);
        o->bounding_sphere.center.x = io_read_float_t_stream_reverse_endianness(s);
        o->bounding_sphere.center.y = io_read_float_t_stream_reverse_endianness(s);
        o->bounding_sphere.center.z = io_read_float_t_stream_reverse_endianness(s);
        o->bounding_sphere.radius = io_read_float_t_stream_reverse_endianness(s);
        oh.meshes_count = io_read_int32_t_stream_reverse_endianness(s);
        oh.meshes_offset = io_read_offset_f2(s, header_length);
        oh.materials_count = io_read_int32_t_stream_reverse_endianness(s);
        oh.materials_offset = io_read_offset_f2(s, header_length);
    }
    else {
        io_read(s, 0, 0x08);
        oh.meshes_count = io_read_int32_t_stream_reverse_endianness(s);
        oh.materials_count = io_read_int32_t_stream_reverse_endianness(s);
        o->bounding_sphere.center.x = io_read_float_t_stream_reverse_endianness(s);
        o->bounding_sphere.center.y = io_read_float_t_stream_reverse_endianness(s);
        o->bounding_sphere.center.z = io_read_float_t_stream_reverse_endianness(s);
        o->bounding_sphere.radius = io_read_float_t_stream_reverse_endianness(s);
        oh.meshes_offset = io_read_offset_x(s);
        oh.materials_offset = io_read_offset_x(s);
        io_read(s, 0, 0x10);
        o->flags = io_read_uint8_t(s);
        io_read(s, 0, 0x07);
    }
    io_read(s, 0, 0x28);

    if (oh.meshes_offset > 0) {
        o->meshes = vector_empty(obj_mesh);
        vector_obj_mesh_reserve(&o->meshes, oh.meshes_count);
        for (int32_t i = 0; i < oh.meshes_count; i++) {
            obj_mesh mesh;
            memset(&mesh, 0, sizeof(obj_mesh));

            io_set_position(s, base_offset + oh.meshes_offset + mesh_size * i, SEEK_SET);

            obj_mesh_header mh;
            memset(&mh, 0, sizeof(obj_mesh_header));
            io_read(s, 0, 0x04);
            mesh.bounding_sphere.center.x = io_read_float_t_stream_reverse_endianness(s);
            mesh.bounding_sphere.center.y = io_read_float_t_stream_reverse_endianness(s);
            mesh.bounding_sphere.center.z = io_read_float_t_stream_reverse_endianness(s);
            mesh.bounding_sphere.radius = io_read_float_t_stream_reverse_endianness(s);
            mh.sub_meshes_count = io_read_int32_t_stream_reverse_endianness(s);
            mh.sub_meshes_offset = io_read_offset(s, 0, is_x);
            if (mh.sub_meshes_offset && !is_x)
                mh.sub_meshes_offset -= header_length;
            mh.attrib_type = io_read_uint32_t_stream_reverse_endianness(s);
            mh.vertex_size = io_read_int32_t_stream_reverse_endianness(s);
            mh.vertex_count = io_read_int32_t_stream_reverse_endianness(s);

            if (!is_x)
                for (int32_t j = 0; j < 20; j++)
                    mh.attrib_offsets[j] = io_read_offset_f2(s, 0);
            else
                for (int32_t j = 0; j < 20; j++)
                    mh.attrib_offsets[j] = io_read_offset_x(s);

            mesh.flags = io_read_int32_t_stream_reverse_endianness(s);
            mh.attrib_flags = io_read_uint32_t_stream_reverse_endianness(s);
            io_read(s, 0, 0x18);
            io_read(s, &mesh.name, sizeof(mesh.name));
            mesh.name[sizeof(mesh.name) - 1] = 0;

            if (mh.sub_meshes_offset) {
                mesh.sub_meshes = vector_empty(obj_sub_mesh);
                vector_obj_sub_mesh_reserve(&mesh.sub_meshes, mh.sub_meshes_count);
                for (int32_t j = 0; j < mh.sub_meshes_count; j++) {
                    obj_sub_mesh sub_mesh;
                    memset(&sub_mesh, 0, sizeof(obj_sub_mesh));

                    io_set_position(s, base_offset + mh.sub_meshes_offset + sub_mesh_size * j, SEEK_SET);

                    obj_sub_mesh_header smh;
                    memset(&smh, 0, sizeof(obj_sub_mesh_header));
                    io_read(s, 0, 0x04);
                    sub_mesh.bounding_sphere.center.x = io_read_float_t_stream_reverse_endianness(s);
                    sub_mesh.bounding_sphere.center.y = io_read_float_t_stream_reverse_endianness(s);
                    sub_mesh.bounding_sphere.center.z = io_read_float_t_stream_reverse_endianness(s);
                    sub_mesh.bounding_sphere.radius = io_read_float_t_stream_reverse_endianness(s);
                    sub_mesh.material_index = io_read_uint32_t_stream_reverse_endianness(s);
                    io_read(s, &sub_mesh.texcoord_indices, 0x08);
                    smh.bone_index_count = io_read_int32_t_stream_reverse_endianness(s);
                    smh.bone_indices_offset = io_read_offset(s, header_length, is_x);
                    sub_mesh.bones_per_vertex = io_read_uint32_t_stream_reverse_endianness(s);
                    sub_mesh.primitive_type = io_read_uint32_t_stream_reverse_endianness(s);
                    sub_mesh.index_format = io_read_uint32_t_stream_reverse_endianness(s);
                    smh.indices_count = io_read_int32_t_stream_reverse_endianness(s);
                    smh.indices_offset = io_read_offset(s, 0, is_x);
                    sub_mesh.flags = io_read_uint32_t_stream_reverse_endianness(s);
                    io_read(s, 0, 0x10);
                    sub_mesh.bounding_box.center.x = io_read_float_t_stream_reverse_endianness(s);
                    sub_mesh.bounding_box.center.y = io_read_float_t_stream_reverse_endianness(s);
                    sub_mesh.bounding_box.center.z = io_read_float_t_stream_reverse_endianness(s);
                    sub_mesh.bounding_box.size.x = io_read_float_t_stream_reverse_endianness(s);
                    sub_mesh.bounding_box.size.y = io_read_float_t_stream_reverse_endianness(s);
                    sub_mesh.bounding_box.size.z = io_read_float_t_stream_reverse_endianness(s);
                    io_read(s, 0, 0x04);
                    sub_mesh.index_offset = io_read_uint32_t_stream_reverse_endianness(s);

                    if (is_x)
                        io_read(s, 0, 0x04);

                    if (sub_mesh.bones_per_vertex == 4 && smh.bone_indices_offset) {
                        uint16_t* bone_index = force_malloc_s(uint16_t, smh.bone_index_count);
                        io_set_position(s, base_offset + smh.bone_indices_offset, SEEK_SET);
                        for (int32_t k = 0; k < smh.bone_index_count; k++)
                            bone_index[k] = io_read_uint16_t_stream_reverse_endianness(s);
                        sub_mesh.bone_index = bone_index;
                        sub_mesh.bone_index_count = smh.bone_index_count;
                    }

                    io_set_position(s_oidx, smh.indices_offset, SEEK_SET);
                    obj_modern_read_index(o, s_oidx, &sub_mesh, smh.indices_count);

                    vector_obj_sub_mesh_push_back(&mesh.sub_meshes, &sub_mesh);
                }
            }

            obj_modern_read_vertex(o, s_ovtx, mh.attrib_offsets, &mesh,
                mh.attrib_flags, mh.vertex_count, mh.vertex_size);
            vector_obj_mesh_push_back(&o->meshes, &mesh);
        }
    }

    if (oh.materials_offset > 0) {
        obj_material_texture_enrs_table_init();

        io_set_position(s, base_offset + oh.materials_offset, SEEK_SET);
        o->materials = vector_empty(obj_material_parent);
        vector_obj_material_parent_reserve(&o->materials, oh.materials_count);
        for (int32_t i = 0; i < oh.materials_count; i++) {
            obj_material_parent m;
            io_read(s, &m, sizeof(obj_material_parent));
            if (s->is_big_endian)
                enrs_apply(&obj_material_texture_enrs_table, &m);
            vector_obj_material_parent_push_back(&o->materials, &m);
        }
    }
}

static void obj_modern_write_model(obj* o, stream* s,
    ssize_t base_offset, bool is_x, f2_struct* omdl) {
    const size_t mesh_size = is_x ? 0x130 : 0xD8;
    const size_t sub_mesh_size = is_x ? 0x80 : 0x70;

    f2_struct oidx;
    f2_struct ovtx;
    memset(&oidx, 0, sizeof(f2_struct));
    memset(&ovtx, 0, sizeof(f2_struct));

    stream s_oidx;
    stream s_ovtx;
    io_mopen(&s_oidx, 0, 0);
    io_mopen(&s_ovtx, 0, 0);

    uint32_t off;
    vector_enrs_entry e = vector_empty(enrs_entry);
    enrs_entry ee;
    vector_size_t pof = vector_empty(size_t);

    obj_header oh;
    memset(&oh, 0, sizeof(obj_header));

    oh.meshes_count = (int32_t)(o->meshes.end - o->meshes.begin);
    oh.materials_count = (int32_t)(o->materials.end - o->materials.begin);

    if (!is_x) {
        ee = (enrs_entry){ 0, 1, 80, 1, vector_empty(enrs_sub_entry) };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 10, ENRS_DWORD });
        vector_enrs_entry_push_back(&e, &ee);
        off = 80;
    }
    else {
        ee = (enrs_entry){ 0, 2, 112, 1, vector_empty(enrs_sub_entry) };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 8, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_QWORD });
        vector_enrs_entry_push_back(&e, &ee);
        off = 112;
    }

    if (!is_x) {
        ee = (enrs_entry){ off, 1, 216, oh.meshes_count, vector_empty(enrs_sub_entry) };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 32, ENRS_DWORD });
        vector_enrs_entry_push_back(&e, &ee);
        off = (uint32_t)(oh.meshes_count * 216LL);
    }
    else {
        ee = (enrs_entry){ off, 5, 304, oh.meshes_count, vector_empty(enrs_sub_entry) };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 6, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_QWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 3, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 4, 20, ENRS_QWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_DWORD });
        vector_enrs_entry_push_back(&e, &ee);
        off = (uint32_t)(oh.meshes_count * 304LL);
    }

    uint32_t total_sub_meshes = 0;
    for (int32_t i = 0; i < oh.meshes_count; i++) {
        obj_mesh* mesh = &o->meshes.begin[i];
        total_sub_meshes += (uint32_t)(mesh->sub_meshes.end - mesh->sub_meshes.begin);
    }

    if (!is_x) {
        ee = (enrs_entry){ off, 17, 112, total_sub_meshes, vector_empty(enrs_sub_entry) };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 6, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 8, 1, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 16, 1, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_WORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_WORD });
        vector_enrs_entry_push_back(&e, &ee);
        off = (uint32_t)(oh.meshes_count * 112LL);
    }
    else {
        ee = (enrs_entry){ off, 17, 128, total_sub_meshes, vector_empty(enrs_sub_entry) };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 6, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 8, 1, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 4, 1, ENRS_QWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_QWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 16, 1, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_WORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_WORD });
        vector_enrs_entry_push_back(&e, &ee);
        off = (uint32_t)(oh.meshes_count * 128LL);
    }

    int32_t total_bone_index_count = 0;
    for (obj_mesh* i = o->meshes.begin; i != o->meshes.end; i++)
        for (obj_sub_mesh* j = i->sub_meshes.begin; j != i->sub_meshes.end; j++)
            if (j->bones_per_vertex == 4)
                total_bone_index_count += j->bone_index_count;

    ee = (enrs_entry){ off, 1, 2, total_bone_index_count, vector_empty(enrs_sub_entry) };
    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_WORD });
    vector_enrs_entry_push_back(&e, &ee);
    off = (uint32_t)(2 * (size_t)total_bone_index_count);
    off = align_val(off, is_x ? 0x10 : 0x04);

    if (oh.materials_count) {
        obj_material_texture_enrs_table_init();

        enrs_entry* mte = &obj_material_texture_enrs_table.begin[0];
        ee = (enrs_entry){ off, 186, 1200, oh.materials_count, };
        ee.offset = off;
        ee.count = mte->count;
        ee.size = mte->size;
        ee.repeat_count = oh.materials_count;
        ee.sub = vector_empty(enrs_sub_entry);
        vector_enrs_sub_entry_insert_range(&ee.sub, 0, mte->sub.begin, mte->sub.end);
        vector_enrs_entry_push_back(&e, &ee);
        off = (uint32_t)(oh.materials_count * oh.materials_count);
    }

    if (!is_x) {
        io_write_uint32_t(s, 0);
        io_write_uint32_t(s, 0);
        io_write_float_t(s, 0);
        io_write_float_t(s, 0);
        io_write_float_t(s, 0);
        io_write_float_t(s, 0);
        io_write_int32_t(s, 0);
        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
        io_write_int32_t(s, 0);
        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
        io_write(s, 0, 0x28);
    }
    else {
        io_write_uint32_t(s, 0);
        io_write_uint32_t(s, 0);
        io_write_int32_t(s, 0);
        io_write_int32_t(s, 0);
        io_write_float_t(s, 0);
        io_write_float_t(s, 0);
        io_write_float_t(s, 0);
        io_write_float_t(s, 0);
        io_write_offset_x_pof_add(s, 0, &pof);
        io_write_offset_x_pof_add(s, 0, &pof);
        io_write(s, 0, 0x10);
        io_write_uint8_t(s, 0);
        io_write(s, 0, 0x07);
        io_write(s, 0, 0x28);
    }
    io_align_write(s, 0x10);

    if (oh.meshes_count) {
        oh.meshes_offset = io_get_position(s) - base_offset;

        obj_mesh_header* mhs = force_malloc_s(obj_mesh_header, oh.meshes_count);
        obj_sub_mesh_header** smhss = force_malloc_s(obj_sub_mesh_header*, oh.meshes_count);
        for (int32_t i = 0; i < oh.meshes_count; i++) {
            obj_mesh* mesh = &o->meshes.begin[i];

            io_write(s, 0, 0x04);
            io_write_float_t(s, 0.0f);
            io_write_float_t(s, 0.0f);
            io_write_float_t(s, 0.0f);
            io_write_float_t(s, 0.0f);
            io_write_int32_t(s, 0);
            io_write_offset_pof_add(s, 0, 0x20, is_x, &pof);
            io_write_uint32_t(s, 0);
            io_write_int32_t(s, 0);
            io_write_int32_t(s, 0);

            if (!is_x)
                for (int32_t j = 0; j < 20; j++)
                    io_write_offset_f2(s, 0, 0);
            else
                for (int32_t j = 0; j < 20; j++)
                    io_write_offset_x(s, 0);

            io_write_int32_t(s, 0);
            io_write_uint32_t(s, 0);
            io_write(s, 0, 0x18);
            io_write(s, 0, sizeof(mesh->name));
        }

        for (int32_t i = 0; i < oh.meshes_count; i++) {
            obj_mesh* mesh = &o->meshes.begin[i];
            obj_mesh_header* mh = &mhs[i];

            mh->sub_meshes_count = (int32_t)(mesh->sub_meshes.end - mesh->sub_meshes.begin);
            mh->attrib_type = OBJ_VERTEX_ATTRIB_MODERN_STORAGE;
            if (mh->sub_meshes_count) {
                mh->sub_meshes_offset = io_get_position(s) - base_offset;
                for (int32_t j = 0; j < mh->sub_meshes_count; j++) {
                    io_write(s, 0, 0x04);
                    io_write_float_t(s, 0.0f);
                    io_write_float_t(s, 0.0f);
                    io_write_float_t(s, 0.0f);
                    io_write_float_t(s, 0.0f);
                    io_write_uint32_t(s, 0);
                    io_write(s, 0, 0x08);
                    io_write_int32_t(s, 0);
                    io_write_offset_pof_add(s, 0, 0x20, is_x, &pof);
                    io_write_uint32_t(s, 0);
                    io_write_uint32_t(s, 0);
                    io_write_uint32_t(s, 0);
                    io_write_int32_t(s, 0);
                    io_write_offset(s, 0, 0, is_x);
                    io_write_uint32_t(s, 0);
                    io_write(s, 0, 0x10);
                    io_write_float_t(s, 0);
                    io_write_float_t(s, 0);
                    io_write_float_t(s, 0);
                    io_write_float_t(s, 0);
                    io_write_float_t(s, 0);
                    io_write_float_t(s, 0);
                    io_write(s, 0, 0x04);
                    io_write_uint32_t(s, 0);

                    if (is_x)
                        io_write(s, 0, 0x04);
                }
            }

            obj_modern_write_vertex(o, &s_ovtx, is_x, mh->attrib_offsets, mesh,
                &mh->attrib_flags, &mh->vertex_count, &mh->vertex_size, &ovtx);
        }

        for (int32_t i = 0; i < oh.meshes_count; i++) {
            obj_mesh* mesh = &o->meshes.begin[i];
            obj_mesh_header* mh = &mhs[i];
            obj_sub_mesh_header* smhs = 0;

            if (mh->sub_meshes_count) {
                smhs = force_malloc_s(obj_sub_mesh_header, mh->sub_meshes_count);
                smhss[i] = smhs;
                for (int32_t j = 0; j < mh->sub_meshes_count; j++) {
                    obj_sub_mesh* sub_mesh = &mesh->sub_meshes.begin[j];
                    obj_sub_mesh_header* smh = &smhs[j];

                    if (sub_mesh->bones_per_vertex == 4 && sub_mesh->bone_index_count) {
                        smh->bone_indices_offset = io_get_position(s) - base_offset;

                        smh->bone_index_count = sub_mesh->bone_index_count;
                        uint16_t* bone_index = sub_mesh->bone_index;
                        for (int32_t k = 0; k < smh->bone_index_count; k++)
                            io_write_uint16_t(s, bone_index[k]);
                    }

                    smh->indices_offset = io_get_position(&s_oidx);
                    obj_modern_write_index(o, &s_oidx, is_x, sub_mesh, &smh->indices_count, &oidx);
                }
            }
        }
        io_align_write(s, is_x ? 0x10 : 0x04);

        for (int32_t i = 0; i < oh.meshes_count; i++) {
            obj_mesh* mesh = &o->meshes.begin[i];
            obj_mesh_header* mh = &mhs[i];
            obj_sub_mesh_header* smhs = smhss[i];

            if (mh->sub_meshes_count) {
                io_position_push(s, base_offset + mh->sub_meshes_offset, SEEK_SET);
                for (int32_t j = 0; j < mh->sub_meshes_count; j++) {
                    obj_sub_mesh* sub_mesh = &mesh->sub_meshes.begin[j];
                    obj_sub_mesh_header* smh = &smhs[j];

                    io_write(s, 0, 0x04);
                    io_write_float_t(s, sub_mesh->bounding_sphere.center.x);
                    io_write_float_t(s, sub_mesh->bounding_sphere.center.y);
                    io_write_float_t(s, sub_mesh->bounding_sphere.center.z);
                    io_write_float_t(s, sub_mesh->bounding_sphere.radius);
                    io_write_uint32_t(s, sub_mesh->material_index);
                    io_write(s, &sub_mesh->texcoord_indices, 0x08);
                    io_write_int32_t(s, smh->bone_index_count);
                    io_write_offset(s, smh->bone_indices_offset, 0x20, is_x);
                    io_write_uint32_t(s, sub_mesh->bones_per_vertex);
                    io_write_uint32_t(s, sub_mesh->primitive_type);
                    io_write_uint32_t(s, sub_mesh->index_format);
                    io_write_int32_t(s, smh->indices_count);
                    io_write_offset(s, smh->indices_offset, 0, is_x);
                    io_write_uint32_t(s, sub_mesh->flags);
                    io_write(s, 0, 0x10);
                    io_write_float_t(s, sub_mesh->bounding_box.center.x);
                    io_write_float_t(s, sub_mesh->bounding_box.center.y);
                    io_write_float_t(s, sub_mesh->bounding_box.center.z);
                    io_write_float_t(s, sub_mesh->bounding_box.size.x);
                    io_write_float_t(s, sub_mesh->bounding_box.size.y);
                    io_write_float_t(s, sub_mesh->bounding_box.size.z);

                    uint32_t index_max = 0;
                    for (int32_t k = 0; k < sub_mesh->indices_count; k++)
                        if (index_max < sub_mesh->indices[k])
                            index_max = sub_mesh->indices[k];

                    switch (sub_mesh->index_format) {
                    case OBJ_INDEX_U8:
                        io_write_uint32_t(s, index_max << 24);
                        break;
                    case OBJ_INDEX_U16:
                        io_write_uint32_t(s, index_max << 16);
                        break;
                    case OBJ_INDEX_U32:
                        io_write_uint32_t(s, index_max);
                        break;
                    }
                    io_write_uint32_t(s, sub_mesh->index_offset);

                    if (is_x)
                        io_write(s, 0, 0x04);
                }
                io_position_pop(s);
            }
            free(smhs);
        }

        io_position_push(s, base_offset + oh.meshes_offset, SEEK_SET);
        for (int32_t i = 0; i < oh.meshes_count; i++) {
            obj_mesh* mesh = &o->meshes.begin[i];
            obj_mesh_header* mh = &mhs[i];

            io_write(s, 0, 0x04);
            io_write_float_t(s, mesh->bounding_sphere.center.x);
            io_write_float_t(s, mesh->bounding_sphere.center.y);
            io_write_float_t(s, mesh->bounding_sphere.center.z);
            io_write_float_t(s, mesh->bounding_sphere.radius);
            io_write_int32_t(s, mh->sub_meshes_count);
            if (mh->sub_meshes_offset && !is_x)
                mh->sub_meshes_offset += 0x20;
            io_write_offset(s, mh->sub_meshes_offset, 0, is_x);
            io_write_uint32_t(s, mh->attrib_type);
            io_write_int32_t(s, mh->vertex_size);
            io_write_int32_t(s, mh->vertex_count);

            if (!is_x)
                for (int32_t j = 0; j < 20; j++)
                    io_write_offset_f2(s, mh->attrib_offsets[j], 0);
            else
                for (int32_t j = 0; j < 20; j++)
                    io_write_offset_x(s, mh->attrib_offsets[j]);

            io_write_int32_t(s, mesh->flags);
            io_write_uint32_t(s, mh->attrib_flags);
            io_write(s, 0, 0x18);
            io_write(s, &mesh->name, sizeof(mesh->name) - 1);
            io_write_char(s, '\0');
        }
        io_position_pop(s);
        free(mhs);
        free(smhss);
    }

    if (oh.materials_count) {
        oh.materials_offset = io_get_position(s) - base_offset;
        for (int32_t i = 0; i < oh.materials_count; i++)
            io_write(s, &o->materials.begin[i], sizeof(obj_material_parent));
    }
    io_align_write(s, 0x10);

    io_position_push(s, base_offset, SEEK_SET);
    if (!is_x) {
        io_write_uint32_t(s, 0x10000);
        io_write_uint32_t(s, 0x00);
        io_write_float_t(s, o->bounding_sphere.center.x);
        io_write_float_t(s, o->bounding_sphere.center.y);
        io_write_float_t(s, o->bounding_sphere.center.z);
        io_write_float_t(s, o->bounding_sphere.radius);
        io_write_int32_t(s, oh.meshes_count);
        io_write_offset_f2(s, oh.meshes_offset, 0x20);
        io_write_int32_t(s, oh.materials_count);
        io_write_offset_f2(s, oh.materials_offset, 0x20);
    }
    else {
        io_write_uint32_t(s, 0x10000);
        io_write_uint32_t(s, 0x00);
        io_write_int32_t(s, oh.meshes_count);
        io_write_int32_t(s, oh.materials_count);
        io_write_float_t(s, o->bounding_sphere.center.x);
        io_write_float_t(s, o->bounding_sphere.center.y);
        io_write_float_t(s, o->bounding_sphere.center.z);
        io_write_float_t(s, o->bounding_sphere.radius);
        io_write_offset_x(s, oh.meshes_offset);
        io_write_offset_x(s, oh.materials_offset);
        io_write(s, 0, 0x10);
        io_write_uint8_t(s, o->flags);
        io_write(s, 0, 0x07);
    }
    io_position_pop(s);

    omdl->enrs = e;
    omdl->pof = pof;

    io_align_write(&s_oidx, 0x10);
    io_mcopy(&s_oidx, &oidx.data, &oidx.length);
    io_free(&s_oidx);

    oidx.header.signature = reverse_endianness_uint32_t('OIDX');
    oidx.header.length = 0x20;
    oidx.header.use_big_endian = false;
    oidx.header.use_section_size = true;
    vector_f2_struct_push_back(&omdl->sub_structs, &oidx);

    io_align_write(&s_ovtx, 0x10);
    io_mcopy(&s_ovtx, &ovtx.data, &ovtx.length);
    io_free(&s_ovtx);

    ovtx.header.signature = reverse_endianness_uint32_t('OVTX');
    ovtx.header.length = 0x20;
    ovtx.header.use_big_endian = false;
    ovtx.header.use_section_size = true;
    vector_f2_struct_push_back(&omdl->sub_structs, &ovtx);
}

static void obj_modern_read_skin(obj* o, stream* s, ssize_t base_offset,
    uint32_t header_length, bool is_x) {
    obj_skin* sk = &o->skin;
    o->skin_init = true;
    io_set_position(s, base_offset, SEEK_SET);

    obj_skin_header sh;
    memset(&sh, 0, sizeof(obj_skin_header));
    if (!is_x) {
        sh.bone_ids_offset = io_read_offset_f2(s, header_length);
        sh.bone_matrices_offset = io_read_offset_f2(s, header_length);
        sh.bone_names_offset = io_read_offset_f2(s, header_length);
        sh.ex_data_offset = io_read_offset_f2(s, header_length);
        sh.bone_count = io_read_int32_t_stream_reverse_endianness(s);
        sh.bone_parent_ids_offset = io_read_offset_f2(s, header_length);
        io_read(s, 0, 0x0C);
    }
    else {
        sh.bone_ids_offset = io_read_offset_x(s);
        sh.bone_matrices_offset = io_read_offset_x(s);
        sh.bone_names_offset = io_read_offset_x(s);
        sh.ex_data_offset = io_read_offset_x(s);
        sh.bone_count = io_read_int32_t_stream_reverse_endianness(s);
        sh.bone_parent_ids_offset = io_read_offset_x(s);
        io_read(s, 0, 0x18);
    }

    if (sh.bone_ids_offset) {
        sk->bones = vector_empty(obj_skin_bone);
        vector_obj_skin_bone_reserve(&sk->bones, sh.bone_count);

        ssize_t* bone_names_offsets = 0;
        if (sh.bone_names_offset) {
            bone_names_offsets = force_malloc_s(ssize_t, sh.bone_count);
            io_set_position(s, sh.bone_names_offset, SEEK_SET);
            if (!is_x)
                for (int32_t i = 0; i < sh.bone_count; i++)
                    bone_names_offsets[i] = io_read_offset_f2(s, header_length);
            else
                for (int32_t i = 0; i < sh.bone_count; i++)
                    bone_names_offsets[i] = io_read_offset_x(s);
        }


        if (sh.bone_ids_offset) {
            obj_skin_bone bone;
            memset(&bone, 0, sizeof(obj_skin_bone));

            io_set_position(s, sh.bone_ids_offset, SEEK_SET);
            for (int32_t i = 0; i < sh.bone_count; i++) {
                bone.id = io_read_uint32_t_stream_reverse_endianness(s);
                vector_obj_skin_bone_push_back(&sk->bones, &bone);
            }

            if (sh.bone_matrices_offset) {
                io_set_position(s, sh.bone_matrices_offset, SEEK_SET);
                for (int32_t i = 0; i < sh.bone_count; i++) {
                    mat4 mat;
                    mat.row0.x = io_read_float_t_stream_reverse_endianness(s);
                    mat.row0.y = io_read_float_t_stream_reverse_endianness(s);
                    mat.row0.z = io_read_float_t_stream_reverse_endianness(s);
                    mat.row0.w = io_read_float_t_stream_reverse_endianness(s);
                    mat.row1.x = io_read_float_t_stream_reverse_endianness(s);
                    mat.row1.y = io_read_float_t_stream_reverse_endianness(s);
                    mat.row1.z = io_read_float_t_stream_reverse_endianness(s);
                    mat.row1.w = io_read_float_t_stream_reverse_endianness(s);
                    mat.row2.x = io_read_float_t_stream_reverse_endianness(s);
                    mat.row2.y = io_read_float_t_stream_reverse_endianness(s);
                    mat.row2.z = io_read_float_t_stream_reverse_endianness(s);
                    mat.row2.w = io_read_float_t_stream_reverse_endianness(s);
                    mat.row3.x = io_read_float_t_stream_reverse_endianness(s);
                    mat.row3.y = io_read_float_t_stream_reverse_endianness(s);
                    mat.row3.z = io_read_float_t_stream_reverse_endianness(s);
                    mat.row3.w = io_read_float_t_stream_reverse_endianness(s);
                    mat4_transpose(&mat, &sk->bones.begin[i].inv_bind_pose_mat);
                }
            }

            if (sh.bone_names_offset)
                for (int32_t i = 0; i < sh.bone_count; i++)
                    io_read_string_null_terminated_offset(s,
                        bone_names_offsets[i], &sk->bones.begin[i].name);

            if (sh.bone_parent_ids_offset) {
                io_set_position(s, sh.bone_parent_ids_offset, SEEK_SET);
                for (int32_t i = 0; i < sh.bone_count; i++)
                    sk->bones.begin[i].parent = io_read_uint32_t_stream_reverse_endianness(s);
            }
        }
        free(bone_names_offsets);
    }

    if (sh.ex_data_offset) {
        obj_skin_ex_data* ex = &sk->ex_data;
        sk->ex_data_init = true;
        io_set_position(s, sh.ex_data_offset, SEEK_SET);

        obj_skin_ex_data_header exh;
        memset(&exh, 0, sizeof(obj_skin_ex_data_header));
        if (!is_x) {
            exh.osage_count = io_read_int32_t_stream_reverse_endianness(s);
            exh.osage_nodes_count = io_read_int32_t_stream_reverse_endianness(s);
            io_read(s, 0, 0x04);
            exh.osage_nodes_offset = io_read_offset_f2(s, header_length);
            exh.osage_names_offset = io_read_offset_f2(s, header_length);
            exh.blocks_offset = io_read_offset_f2(s, header_length);
            exh.strings_count = io_read_int32_t_stream_reverse_endianness(s);
            exh.strings_offset = io_read_offset_f2(s, header_length);
            exh.osage_sibling_infos_offset = io_read_offset_f2(s, header_length);
            exh.cloth_count = io_read_int32_t_stream_reverse_endianness(s);
            io_read(s, 0, 0x1C);
        }
        else {
            exh.osage_count = io_read_int32_t_stream_reverse_endianness(s);
            exh.osage_nodes_count = io_read_int32_t_stream_reverse_endianness(s);
            io_read(s, 0, 0x08);
            exh.osage_nodes_offset = io_read_offset_x(s);
            exh.osage_names_offset = io_read_offset_x(s);
            exh.blocks_offset = io_read_offset_x(s);
            exh.strings_count = io_read_int32_t_stream_reverse_endianness(s);
            exh.strings_offset = io_read_offset_x(s);
            exh.osage_sibling_infos_offset = io_read_offset_x(s);
            exh.cloth_count = io_read_int32_t_stream_reverse_endianness(s);
            io_read(s, 0, 0x04);
            io_read(s, 0, 0x38);
        }

        vector_string string_set = vector_empty(string);
        vector_string_reserve(&string_set, exh.strings_count);
        if (exh.strings_offset) {
            ssize_t* strings_offsets = force_malloc_s(ssize_t, exh.strings_count);
            io_set_position(s, exh.strings_offset, SEEK_SET);
            if (!is_x)
                for (int32_t i = 0; i < exh.strings_count; i++)
                    strings_offsets[i] = io_read_offset_f2(s, header_length);
            else
                for (int32_t i = 0; i < exh.strings_count; i++)
                    strings_offsets[i] = io_read_offset_x(s);

            for (int32_t i = 0; i < exh.strings_count; i++) {
                string str;
                io_read_string_null_terminated_offset(s, strings_offsets[i], &str);
                vector_string_push_back(&string_set, &str);
            }
            free(strings_offsets);
        }

        ex->osage_nodes = vector_empty(obj_skin_osage_node);
        vector_obj_skin_osage_node_reserve(&ex->osage_nodes, exh.osage_nodes_count);
        if (exh.osage_nodes_offset) {
            obj_skin_osage_node osage_node;
            memset(&osage_node, 0, sizeof(obj_skin_osage_node));

            io_set_position(s, exh.osage_nodes_offset, SEEK_SET);
            for (int32_t i = 0; i < exh.osage_nodes_count; i++) {
                int32_t name = io_read_uint32_t_stream_reverse_endianness(s);
                if (name & 0x8000)
                    string_copy(&string_set.begin[name & 0x7FFF], &osage_node.name);
                else
                    memset(&osage_node.name, 0, sizeof(string));

                osage_node.length = io_read_float_t_stream_reverse_endianness(s);
                io_read(s, 0, 0x04);
                vector_obj_skin_osage_node_push_back(&ex->osage_nodes, &osage_node);
            }
        }

        if (exh.blocks_offset) {
            int32_t blocks_count = 0;
            io_set_position(s, exh.blocks_offset, SEEK_SET);
            if (!is_x)
                while (io_read_int32_t(s)) {
                    io_read(s, 0, 0x04);
                    blocks_count++;
                }
            else
                while (io_read_int64_t(s)) {
                    io_read(s, 0, 0x08);
                    blocks_count++;
                }

            obj_skin_block_header* bhs = force_malloc_s(obj_skin_block_header, blocks_count);
            io_set_position(s, exh.blocks_offset, SEEK_SET);
            if (!is_x)
                for (int32_t i = 0; i < blocks_count; i++) {
                    bhs[i].block_signature_offset = io_read_offset_f2(s, header_length);
                    bhs[i].block_offset = io_read_offset_f2(s, header_length);
                }
            else
                for (int32_t i = 0; i < blocks_count; i++) {
                    bhs[i].block_signature_offset = io_read_offset_x(s);
                    bhs[i].block_offset = io_read_offset_x(s);
                }

            obj_skin_block block;
            memset(&block, 0, sizeof(obj_skin_block));

            ex->blocks = vector_empty(obj_skin_block);
            vector_obj_skin_block_reserve(&ex->blocks, blocks_count);
            for (int32_t i = 0; i < blocks_count; i++) {
                bool valid = false;

                char* block_signature = io_read_utf8_string_null_terminated_offset(s,
                    bhs[i].block_signature_offset);
                if (utf8_length(block_signature) == 3) {
                    valid = true;

                    uint32_t signature = load_reverse_endianness_uint32_t(block_signature);
                    switch (signature) {
                    case 'CLS\0':
                        block.type = OBJ_SKIN_BLOCK_CLOTH;
                        io_set_position(s, bhs[i].block_offset, SEEK_SET);
                        obj_modern_read_skin_block_cloth(&block.cloth, s,
                            header_length, string_set.begin, is_x);
                        break;
                    case 'CNS\0':
                        block.type = OBJ_SKIN_BLOCK_CONSTRAINT;
                        io_set_position(s, bhs[i].block_offset, SEEK_SET);
                        obj_modern_read_skin_block_constraint(&block.constraint, s,
                            header_length, string_set.begin, is_x);
                        break;
                    case 'EXP\0':
                        block.type = OBJ_SKIN_BLOCK_EXPRESSION;
                        io_set_position(s, bhs[i].block_offset, SEEK_SET);
                        obj_modern_read_skin_block_expression(&block.expression, s,
                            header_length, string_set.begin, is_x);
                        break;
                    case 'MOT\0':
                        block.type = OBJ_SKIN_BLOCK_MOTION;
                        io_set_position(s, bhs[i].block_offset, SEEK_SET);
                        obj_modern_read_skin_block_motion(&block.motion, s,
                            header_length, string_set.begin, is_x);
                        break;
                    case 'OSG\0':
                        block.type = OBJ_SKIN_BLOCK_OSAGE;
                        io_set_position(s, bhs[i].block_offset, SEEK_SET);
                        obj_modern_read_skin_block_osage(&block.osage, s,
                            header_length, string_set.begin, is_x);
                        break;
                    default:
                        valid = false;
                        break;
                    }
                }
                free(block_signature);

                if (valid)
                    vector_obj_skin_block_push_back(&ex->blocks, &block);
            }
            free(bhs);
        }

        io_set_position(s, exh.osage_sibling_infos_offset, SEEK_SET);
        while (true) {
            uint32_t bone_name_index = io_read_uint32_t_stream_reverse_endianness(s);
            if (!bone_name_index)
                break;

            uint32_t sibling_name = io_read_uint32_t_stream_reverse_endianness(s);
            float_t sibling_distance = io_read_float_t_stream_reverse_endianness(s);

            if (!(bone_name_index & 0x8000))
                continue;

            char* bone_name = string_data(&string_set.begin[bone_name_index & 0x7FFF]);

            obj_skin_osage_node* osage_node = 0;
            for (obj_skin_osage_node* i = ex->osage_nodes.begin; i != ex->osage_nodes.end; i++)
                if (!strcmp(string_data(&i->name), bone_name)) {
                    string_free(&i->sibling_name);
                    if (sibling_name & 0x8000)
                        string_copy(&string_set.begin[sibling_name & 0x7FFF], &i->sibling_name);
                    else
                        memset(&i->sibling_name, 0, sizeof(string));

                    i->sibling_max_distance = sibling_distance;
                    break;
                }

        }

        for (string* i = string_set.begin; i != string_set.end; i++)
            string_free(i);
        vector_string_free(&string_set);
    }
}

static void obj_modern_write_skin(obj* o, stream* s,
    ssize_t base_offset,  bool is_x, f2_struct* oskn) {
    obj_skin* sk = &o->skin;

    uint32_t off;
    vector_enrs_entry e = vector_empty(enrs_entry);
    enrs_entry ee;
    vector_size_t pof = vector_empty(size_t);

    if (!is_x) {
        ee = (enrs_entry){ 0, 1, 48, 1, vector_empty(enrs_sub_entry) };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 9, ENRS_DWORD });
        vector_enrs_entry_push_back(&e, &ee);
        off = 48;
    }
    else {
        ee = (enrs_entry){ 0, 3, 72, 1, vector_empty(enrs_sub_entry) };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 4, ENRS_QWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 4, 4, ENRS_QWORD });
        vector_enrs_entry_push_back(&e, &ee);
        off = 72;
    }
    off = align_val(off, 0x10);

    obj_skin_header sh;
    memset(&sh, 0, sizeof(obj_skin_header));
    sh.bone_count = (int32_t)(sk->bones.end - sk->bones.begin);
    if (sh.bone_count) {
        if (sh.bone_count % 4) {
            ee = (enrs_entry){ off, 1, 4, sh.bone_count, vector_empty(enrs_sub_entry) };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
            vector_enrs_entry_push_back(&e, &ee);
            off = (uint32_t)(sh.bone_count * 4LL);
            off = align_val(off, 0x10);

            ee = (enrs_entry){ off, 1, 4, sh.bone_count, vector_empty(enrs_sub_entry) };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
            vector_enrs_entry_push_back(&e, &ee);
            off = (uint32_t)(sh.bone_count * 4LL);
        }
        else {
            ee = (enrs_entry){ off, 1, 4, sh.bone_count * 2, vector_empty(enrs_sub_entry) };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
            vector_enrs_entry_push_back(&e, &ee);
            off = (uint32_t)(sh.bone_count * 2 * 4LL);
        }
        off = align_val(off, 0x10);

        if (!is_x) {
            ee = (enrs_entry){ off, 1, 4, sh.bone_count, vector_empty(enrs_sub_entry) };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
            vector_enrs_entry_push_back(&e, &ee);
            off = (uint32_t)(sh.bone_count * 4LL);
        }
        else {
            ee = (enrs_entry){ off, 1, 8, sh.bone_count, vector_empty(enrs_sub_entry) };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_QWORD });
            vector_enrs_entry_push_back(&e, &ee);
            off = (uint32_t)(sh.bone_count * 8LL);
        }
        off = align_val(off, 0x10);

        ee = (enrs_entry){ off, 1, 64, sh.bone_count, vector_empty(enrs_sub_entry) };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 16, ENRS_DWORD });
        vector_enrs_entry_push_back(&e, &ee);
        off = (uint32_t)(sh.bone_count * 64LL);
        off = align_val(off, 0x10);
    }

    vector_string strings = vector_empty(string);
    vector_ssize_t string_offsets = vector_empty(ssize_t);
    vector_string string_set = vector_empty(string);

    obj_skin_ex_data_header exh;
    memset(&exh, 0, sizeof(obj_skin_ex_data_header));

    obj_skin_block_header* bhs = 0;
    ssize_t motion_block_node_mats = 0;
    ssize_t motion_block_node_name_offset = 0;
    ssize_t field_18 = 0;
    ssize_t field_1C = 0;
    ssize_t field_20 = 0;
    ssize_t field_24 = 0;
    ssize_t field_28 = 0;
    int32_t blocks_count = 0;
    int32_t expression_count = 0;
    int32_t motion_count = 0;
    int32_t motion_nodes_count = 0;

    if (sk->ex_data_init) {
        obj_skin_ex_data* ex = &sk->ex_data;

        for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
            switch (i->type) {
            case OBJ_SKIN_BLOCK_CLOTH: {
                obj_skin_block_cloth* cls = &i->cloth;
                obj_skin_strings_push_back_check(&strings, string_data(&cls->mesh_name));
                obj_skin_strings_push_back_check(&strings, string_data(&cls->backface_mesh_name));
                for (obj_skin_block_cloth_field_1C* j = cls->field_1C.begin; j != cls->field_1C.end; j++) {
                    obj_skin_strings_push_back_check(&strings, string_data(&j->sub_data_0.bone_name));
                    obj_skin_strings_push_back_check(&strings, string_data(&j->sub_data_1.bone_name));
                    obj_skin_strings_push_back_check(&strings, string_data(&j->sub_data_2.bone_name));
                    obj_skin_strings_push_back_check(&strings, string_data(&j->sub_data_3.bone_name));
                }
                obj_skin_strings_push_back_check(&strings, "CLS");
            } break;
            case OBJ_SKIN_BLOCK_CONSTRAINT: {
                obj_skin_block_constraint* cns = &i->constraint;
                obj_skin_strings_push_back_check(&strings, string_data(&cns->base.parent_name));
                obj_skin_strings_push_back_check(&strings, string_data(&cns->base.name));
                obj_skin_strings_push_back_check(&strings, string_data(&cns->source_node_name));
                switch (cns->type) {
                case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
                    obj_skin_strings_push_back_check(&strings, "Direction");
                    obj_skin_strings_push_back_check(&strings, string_data(&cns->direction.up_vector.name));
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
                    obj_skin_strings_push_back_check(&strings, "Distance");
                    obj_skin_strings_push_back_check(&strings, string_data(&cns->distance.up_vector.name));
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
                    obj_skin_strings_push_back_check(&strings, "Orientation");
                    break;
                case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION:
                    obj_skin_strings_push_back_check(&strings, "Position");
                    obj_skin_strings_push_back_check(&strings, string_data(&cns->position.up_vector.name));
                    break;
                }
                obj_skin_strings_push_back_check(&strings, "CNS");

                obj_skin_strings_push_back_check(&string_set, string_data(&cns->base.name));
            } break;
            case OBJ_SKIN_BLOCK_EXPRESSION: {
                obj_skin_block_expression* exp = &i->expression;
                for (int32_t j = 0; j < exp->expression_count; j++)
                    obj_skin_strings_push_back_check(&strings, string_data(&exp->expressions[j]));
                obj_skin_strings_push_back_check(&strings, string_data(&exp->base.parent_name));
                obj_skin_strings_push_back_check(&strings, string_data(&exp->base.name));
                obj_skin_strings_push_back_check(&strings, "EXP");

                obj_skin_strings_push_back_check(&string_set, string_data(&exp->base.name));
            } break;
            case OBJ_SKIN_BLOCK_MOTION: {
                obj_skin_block_motion* mot = &i->motion;
                obj_skin_strings_push_back_check(&strings, string_data(&mot->base.parent_name));
                obj_skin_strings_push_back_check(&strings, string_data(&mot->base.name));
                for (obj_skin_motion_node* j = mot->nodes.begin; j != mot->nodes.end; j++)
                    obj_skin_strings_push_back_check(&strings, string_data(&j->name));
                obj_skin_strings_push_back_check(&strings, "MOT");

                if (!is_x) {
                    obj_skin_strings_push_back_check(&string_set, string_data(&mot->base.name));
                }
                for (obj_skin_motion_node* j = mot->nodes.begin; j != mot->nodes.end; j++)
                    obj_skin_strings_push_back_check(&string_set, string_data(&j->name));
            } break;
            case OBJ_SKIN_BLOCK_OSAGE: {
                obj_skin_block_osage* osg = &i->osage;
                obj_skin_strings_push_back_check(&strings, string_data(&osg->base.parent_name));
                obj_skin_strings_push_back_check(&strings, string_data(&osg->base.name));
                obj_skin_strings_push_back_check(&strings, string_data(&osg->external_name));
                obj_skin_osage_node* osage_node = &ex->osage_nodes.begin[osg->start_index];
                for (int32_t j = 0; j < osg->count; j++) {
                    obj_skin_strings_push_back_check(&strings, string_data(&osage_node->name));
                    obj_skin_strings_push_back_check(&strings, string_data(&osage_node->sibling_name));
                    osage_node++;
                }
                obj_skin_strings_push_back_check(&strings, "OSG");

                obj_skin_strings_push_back_check(&string_set, string_data(&osg->external_name));
                osage_node = &ex->osage_nodes.begin[osg->start_index];
                for (int32_t j = 0; j < osg->count; j++) {
                    obj_skin_strings_push_back_check(&string_set, string_data(&osage_node->name));
                    obj_skin_strings_push_back_check(&string_set, string_data(&osage_node->sibling_name));
                    osage_node++;
                }
                obj_skin_strings_push_back_check(&string_set, string_data(&osg->base.name));
            } break;
            }

        exh.osage_nodes_count = (int32_t)(ex->osage_nodes.end - ex->osage_nodes.begin);

        exh.cloth_count = 0;
        exh.osage_count = 0;
        for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
            if (i->type == OBJ_SKIN_BLOCK_CLOTH)
                exh.cloth_count++;
            else if (i->type == OBJ_SKIN_BLOCK_EXPRESSION)
                expression_count++;
            else if (i->type == OBJ_SKIN_BLOCK_MOTION) {
                motion_count++;
                motion_nodes_count += (int32_t)(i->motion.nodes.end - i->motion.nodes.begin);
            }
            else if (i->type == OBJ_SKIN_BLOCK_OSAGE)
                exh.osage_count++;

        exh.strings_count = (int32_t)(string_set.end - string_set.begin);
    }

    if (sk->ex_data_init) {
        obj_skin_ex_data* ex = &sk->ex_data;

        if (!is_x) {
            ee = (enrs_entry){ off, 1, 80, 1, vector_empty(enrs_sub_entry) };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 10, ENRS_DWORD });
            vector_enrs_entry_push_back(&e, &ee);
            off = 80;
        }
        else {
            ee = (enrs_entry){ off, 5, 128, 1, vector_empty(enrs_sub_entry) };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_DWORD });
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 4, ENRS_QWORD });
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 4, 2, ENRS_QWORD });
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
            vector_enrs_entry_push_back(&e, &ee);
            off = 128;
        }
        off = align_val(off, 0x10);

        if (exh.osage_nodes_count) {
            ee = (enrs_entry){ off, 1, 12, exh.osage_nodes_count, vector_empty(enrs_sub_entry) };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_DWORD });
            vector_enrs_entry_push_back(&e, &ee);
            off = (uint32_t)(exh.osage_nodes_count * 12LL);
            off = align_val(off, 0x10);

            int32_t osage_sibling_infos_count = 1;
            for (obj_skin_osage_node* i = ex->osage_nodes.begin; i != ex->osage_nodes.end; i++)
                if (string_length(&i->sibling_name))
                    osage_sibling_infos_count++;

            ee = (enrs_entry){ off, 1, 12, osage_sibling_infos_count, vector_empty(enrs_sub_entry) };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 3, ENRS_DWORD });
            vector_enrs_entry_push_back(&e, &ee);
            off = (uint32_t)(osage_sibling_infos_count * 12LL);
            off = align_val(off, 0x10);
        }

        if (exh.osage_count || exh.cloth_count) {
            int32_t count = exh.osage_count + exh.cloth_count;
            if (!is_x) {
                ee = (enrs_entry){ off, 1, 4, count, vector_empty(enrs_sub_entry) };
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
                vector_enrs_entry_push_back(&e, &ee);
                off = (uint32_t)(count * 4LL);
            }
            else {
                ee = (enrs_entry){ off, 1, 8, count, vector_empty(enrs_sub_entry) };
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_QWORD });
                vector_enrs_entry_push_back(&e, &ee);
                off = (uint32_t)(count * 8LL);
            }
            off = align_val(off, 0x10);
        }

        if (!is_x) {
            ee = (enrs_entry){ off, 1, 4, exh.strings_count, vector_empty(enrs_sub_entry) };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
            vector_enrs_entry_push_back(&e, &ee);
            off = (uint32_t)(exh.strings_count * 4LL);
        }
        else {
            ee = (enrs_entry){ off, 1, 8, exh.strings_count, vector_empty(enrs_sub_entry) };
            vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_QWORD });
            vector_enrs_entry_push_back(&e, &ee);
            off = (uint32_t)(exh.strings_count * 8LL);
        }
        off = align_val(off, 0x10);

        blocks_count = (int32_t)(ex->blocks.end - ex->blocks.begin);
        if (blocks_count > 0) {
            if (!is_x) {
                ee = (enrs_entry){ off, 1, 8, blocks_count, vector_empty(enrs_sub_entry) };
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_DWORD });
                vector_enrs_entry_push_back(&e, &ee);
                off = (uint32_t)(blocks_count * 8LL);
            }
            else {
                ee = (enrs_entry){ off, 1, 16, blocks_count, vector_empty(enrs_sub_entry) };
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_QWORD });
                vector_enrs_entry_push_back(&e, &ee);
                off = (uint32_t)(blocks_count * 16LL);
            }
            off = align_val(off, 0x10);

            if (exh.osage_count) {
                if (!is_x) {
                    ee = (enrs_entry){ off, 2, 76, exh.osage_count, vector_empty(enrs_sub_entry) };
                    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 14, ENRS_DWORD });
                    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 4, 1, ENRS_DWORD });
                    vector_enrs_entry_push_back(&e, &ee);
                    off = (uint32_t)(exh.osage_count * 76LL);
                }
                else {
                    ee = (enrs_entry){ off, 4, 104, exh.osage_count, vector_empty(enrs_sub_entry) };
                    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_QWORD });
                    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 9, ENRS_DWORD });
                    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 4, 4, ENRS_DWORD });
                    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 8, 1, ENRS_QWORD });
                    vector_enrs_entry_push_back(&e, &ee);
                    off = (uint32_t)(exh.osage_count * 104LL);
                }
                off = align_val(off, 0x10);
            }

            if (expression_count) {
                if (!is_x) {
                    ee = (enrs_entry){ off, 1, 84, expression_count, vector_empty(enrs_sub_entry) };
                    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 19, ENRS_DWORD });
                    vector_enrs_entry_push_back(&e, &ee);
                    off = (uint32_t)(expression_count * 84LL);
                }
                else {
                    ee = (enrs_entry){ off, 5, 136, expression_count, vector_empty(enrs_sub_entry) };
                    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_QWORD });
                    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 9, ENRS_DWORD });
                    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 4, 1, ENRS_QWORD });
                    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
                    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 4, 9, ENRS_QWORD });
                    vector_enrs_entry_push_back(&e, &ee);
                    off = (uint32_t)(expression_count * 136LL);
                }
                off = align_val(off, 0x10);
            }

            if (exh.cloth_count) {
                if (!is_x) {
                    ee = (enrs_entry){ off, 1, 52, exh.cloth_count, vector_empty(enrs_sub_entry) };
                    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 13, ENRS_DWORD });
                    vector_enrs_entry_push_back(&e, &ee);
                    off = (uint32_t)(exh.cloth_count * 52LL);
                }
                else {
                    ee = (enrs_entry){ off, 4, 88, exh.cloth_count, vector_empty(enrs_sub_entry) };
                    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 2, ENRS_QWORD });
                    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 4, ENRS_DWORD });
                    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 6, ENRS_QWORD });
                    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
                    vector_enrs_entry_push_back(&e, &ee);
                    off = (uint32_t)(exh.cloth_count * 88LL);
                }
                off = align_val(off, 0x10);
            }

            int32_t constraint_count = 0;
            obj_skin_block_constraint_type cns_type = OBJ_SKIN_BLOCK_CONSTRAINT_NONE;
            for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++) {
                if (i->type != OBJ_SKIN_BLOCK_CONSTRAINT)
                    continue;

                if (cns_type == i->constraint.type) {
                    constraint_count++;
                    continue;
                }

                if (cns_type) {
                    ee.repeat_count = constraint_count;
                    vector_enrs_entry_push_back(&e, &ee);
                    off = ee.size * ee.repeat_count;
                }

                cns_type = i->constraint.type;
                if (!is_x)
                    switch (cns_type) {
                    case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
                        ee = (enrs_entry){ off, 8, 144, 0, vector_empty(enrs_sub_entry) };
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 29, ENRS_DWORD });
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
                        ee = (enrs_entry){ off, 8, 136, 0, vector_empty(enrs_sub_entry) };
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 34, ENRS_DWORD });
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
                        ee = (enrs_entry){ off, 6, 68, 0, vector_empty(enrs_sub_entry) };
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 17, ENRS_DWORD });
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION:
                        ee = (enrs_entry){ off, 8, 132, 0, vector_empty(enrs_sub_entry) };
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 33, ENRS_DWORD });
                        break;
                    }
                else
                    switch (cns_type) {
                    case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
                        ee = (enrs_entry){ off, 8, 144, 0, vector_empty(enrs_sub_entry) };
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_QWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 9, ENRS_DWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 4, 2, ENRS_QWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 4, 1, ENRS_QWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 8, ENRS_DWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_QWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 6, ENRS_DWORD });
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
                        ee = (enrs_entry){ off, 8, 168, 0, vector_empty(enrs_sub_entry) };
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_QWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 9, ENRS_DWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 4, 2, ENRS_QWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 4, 1, ENRS_QWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 8, ENRS_DWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_QWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 11, ENRS_DWORD });
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
                        ee = (enrs_entry){ off, 6, 96, 0, vector_empty(enrs_sub_entry) };
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_QWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 9, ENRS_DWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 4, 2, ENRS_QWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 4, 1, ENRS_QWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 3, ENRS_DWORD });
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION:
                        ee = (enrs_entry){ off, 8, 160, 0, vector_empty(enrs_sub_entry) };
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_QWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 9, ENRS_DWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 4, 2, ENRS_QWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 4, 1, ENRS_QWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 8, ENRS_DWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_QWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 10, ENRS_DWORD });
                        break;
                    }
                constraint_count = 1;
            }

            if (constraint_count) {
                ee.repeat_count = constraint_count;
                vector_enrs_entry_push_back(&e, &ee);
                off = ee.size * ee.repeat_count;
            }
            off = align_val(off, 0x10);

            if (!is_x) {
                ee = (enrs_entry){ off, 1, 56, motion_count, vector_empty(enrs_sub_entry) };
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 14, ENRS_DWORD });
                vector_enrs_entry_push_back(&e, &ee);
                off = (uint32_t)(motion_count * 56LL);
            }
            else {
                ee = (enrs_entry){ off, 5, 80, motion_count, vector_empty(enrs_sub_entry) };
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_QWORD });
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 9, ENRS_DWORD });
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 4, 1, ENRS_QWORD });
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 4, 2, ENRS_QWORD });
                vector_enrs_entry_push_back(&e, &ee);
                off = (uint32_t)(motion_count * 80LL);
            }
            off = align_val(off, 0x10);

            if (motion_count) {
                ee = (enrs_entry){ off, 1, 64, motion_nodes_count, vector_empty(enrs_sub_entry) };
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 16, ENRS_DWORD });
                vector_enrs_entry_push_back(&e, &ee);
                off = (uint32_t)(motion_nodes_count * 64LL);
                off = align_val(off, 0x10);

                ee = (enrs_entry){ off, 1, 4, motion_nodes_count, vector_empty(enrs_sub_entry) };
                vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_DWORD });
                vector_enrs_entry_push_back(&e, &ee);
                off = (uint32_t)(motion_nodes_count * 4LL);
                off = align_val(off, 0x10);
            }

            if (exh.cloth_count) {
                int32_t field_18_count = 0;
                int32_t field_1C_count = 0;
                int32_t field_20_count = 0;
                int32_t field_24_count = 0;
                int32_t field_28_count = 0;
                for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                    if (i->type == OBJ_SKIN_BLOCK_CLOTH) {
                        for (int32_t j = 0; j < 32; j++)
                            if (i->cloth.field_10 & (1 << j))
                                field_18_count++;

                        field_1C_count += (int32_t)(i->cloth.field_1C.end - i->cloth.field_1C.begin);
                        field_20_count += (int32_t)(i->cloth.field_20.end - i->cloth.field_20.begin);
                        field_24_count += (int32_t)(i->cloth.field_24.end - i->cloth.field_24.begin);
                        field_24_count++;
                        field_28_count += (int32_t)(i->cloth.field_28.end - i->cloth.field_28.begin);
                        field_28_count++;
                    }

                if (field_18_count) {
                    ee = (enrs_entry){ off, 1, 64, field_18_count, vector_empty(enrs_sub_entry) };
                    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 16, ENRS_DWORD });
                    vector_enrs_entry_push_back(&e, &ee);
                    off = (uint32_t)(field_18_count * 64LL);
                    off = align_val(off, 0x10);
                }

                if (field_1C_count) {
                    if (!is_x) {
                        ee = (enrs_entry){ off, 1, 104, field_1C_count, vector_empty(enrs_sub_entry) };
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 26, ENRS_DWORD });
                        vector_enrs_entry_push_back(&e, &ee);
                        off = (uint32_t)(field_1C_count * 104LL);
                    }
                    else {
                        ee = (enrs_entry){ off, 9, 136, field_1C_count, vector_empty(enrs_sub_entry) };
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 10, ENRS_DWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_QWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 3, ENRS_DWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 4, 1, ENRS_QWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 3, ENRS_DWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 4, 1, ENRS_QWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 3, ENRS_DWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 4, 1, ENRS_QWORD });
                        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 3, ENRS_DWORD });
                        vector_enrs_entry_push_back(&e, &ee);
                        off = (uint32_t)(field_1C_count * 136LL);
                    }
                    off = align_val(off, 0x10);
                }

                if (field_20_count) {
                    ee = (enrs_entry){ off, 1, 440, field_20_count, vector_empty(enrs_sub_entry) };
                    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 110, ENRS_DWORD });
                    vector_enrs_entry_push_back(&e, &ee);
                    off = (uint32_t)(field_20_count * 440LL);
                    off = align_val(off, 0x10);
                }

                if (field_24_count) {
                    ee = (enrs_entry){ off, 1, 2, field_24_count, vector_empty(enrs_sub_entry) };
                    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_WORD });
                    vector_enrs_entry_push_back(&e, &ee);
                    off = (uint32_t)(field_24_count * 2LL);
                    off = align_val(off, 0x10);
                }

                if (field_28_count) {
                    ee = (enrs_entry){ off, 1, 2, field_28_count, vector_empty(enrs_sub_entry) };
                    vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 1, ENRS_WORD });
                    vector_enrs_entry_push_back(&e, &ee);
                    off = (uint32_t)(field_28_count * 2LL);
                    off = align_val(off, 0x10);
                }
            }
        }
    }

    if (!is_x) {
        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
        io_write_int32_t(s, 0);
        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
        io_write(s, 0, 0x0C);
    }
    else {
        io_write_offset_x_pof_add(s, 0, &pof);
        io_write_offset_x_pof_add(s, 0, &pof);
        io_write_offset_x_pof_add(s, 0, &pof);
        io_write_offset_x_pof_add(s, 0, &pof);
        io_write_int32_t(s, 0);
        io_write_offset_x_pof_add(s, 0, &pof);
        io_write(s, 0, 0x18);
    }
    io_align_write(s, 0x10);

    if (sh.bone_count) {
        sh.bone_ids_offset = io_get_position(s);
        for (int32_t i = 0; i < sh.bone_count; i++)
            io_write_int32_t(s, sk->bones.begin[i].id);
        io_align_write(s, 0x10);

        sh.bone_parent_ids_offset = io_get_position(s);
        for (int32_t i = 0; i < sh.bone_count; i++)
            io_write_int32_t(s, sk->bones.begin[i].parent);
        io_align_write(s, 0x10);

        sh.bone_names_offset = io_get_position(s);
        if (!is_x)
            for (int32_t i = 0; i < sh.bone_count; i++) {
                io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                obj_skin_strings_push_back_check(&strings, string_data(&sk->bones.begin[i].name));
            }
        else
            for (int32_t i = 0; i < sh.bone_count; i++) {
                io_write_offset_x_pof_add(s, 0, &pof);
                obj_skin_strings_push_back_check(&strings, string_data(&sk->bones.begin[i].name));
            }
        io_align_write(s, 0x10);

        sh.bone_matrices_offset = io_get_position(s);
        for (int32_t i = 0; i < sh.bone_count; i++) {
            mat4 mat;
            mat4_transpose(&sk->bones.begin[i].inv_bind_pose_mat, &mat);
            io_write_float_t(s, mat.row0.x);
            io_write_float_t(s, mat.row0.y);
            io_write_float_t(s, mat.row0.z);
            io_write_float_t(s, mat.row0.w);
            io_write_float_t(s, mat.row1.x);
            io_write_float_t(s, mat.row1.y);
            io_write_float_t(s, mat.row1.z);
            io_write_float_t(s, mat.row1.w);
            io_write_float_t(s, mat.row2.x);
            io_write_float_t(s, mat.row2.y);
            io_write_float_t(s, mat.row2.z);
            io_write_float_t(s, mat.row2.w);
            io_write_float_t(s, mat.row3.x);
            io_write_float_t(s, mat.row3.y);
            io_write_float_t(s, mat.row3.z);
            io_write_float_t(s, mat.row3.w);
        }
        io_align_write(s, 0x10);
    }

    if (sk->ex_data_init) {
        obj_skin_ex_data* ex = &sk->ex_data;

        sh.ex_data_offset = io_get_position(s);
        if (!is_x) {
            io_write_int32_t(s, 0);
            io_write_int32_t(s, 0);
            io_write(s, 0, 0x04);
            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
            io_write_int32_t(s, 0);
            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
            io_write_int32_t(s, 0);
            io_write(s, 0, 0x1C);
        }
        else {
            io_write_int32_t(s, 0);
            io_write_int32_t(s, 0);
            io_write(s, 0, 0x08);
            io_write_offset_x_pof_add(s, 0, &pof);
            io_write_offset_x_pof_add(s, 0, &pof);
            io_write_offset_x_pof_add(s, 0, &pof);
            io_write_int32_t(s, 0);
            io_write_offset_x_pof_add(s, 0, &pof);
            io_write_offset_x_pof_add(s, 0, &pof);
            io_write_int32_t(s, 0);
            io_write(s, 0, 0x04);
            io_write(s, 0, 0x38);
        }
        io_align_write(s, 0x10);

        if (exh.osage_nodes_count) {
            exh.osage_nodes_offset = io_get_position(s);
            for (int32_t i = 0; i < exh.osage_nodes_count; i++) {
                io_write_int32_t(s, 0);
                io_write_int32_t(s, 0);
                io_write_int32_t(s, 0);
            }
            io_align_write(s, 0x10);

            exh.osage_sibling_infos_offset = io_get_position(s);
            for (obj_skin_osage_node* i = ex->osage_nodes.begin; i != ex->osage_nodes.end; i++) {
                if (!string_length(&i->sibling_name))
                    continue;

                io_write_int32_t(s, 0);
                io_write_int32_t(s, 0);
                io_write_int32_t(s, 0);
            }
            io_write_int32_t(s, 0);
            io_write_int32_t(s, 0);
            io_write_int32_t(s, 0);
            io_align_write(s, 0x10);
        }

        if (exh.osage_count || exh.cloth_count) {
            exh.osage_names_offset = io_get_position(s);
            if (!is_x) {
                for (int32_t i = 0; i < exh.osage_count; i++)
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);

                for (int32_t i = 0; i < exh.cloth_count; i++)
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
            }
            else {
                for (int32_t i = 0; i < exh.osage_count; i++)
                    io_write_offset_x_pof_add(s, 0, &pof);

                for (int32_t i = 0; i < exh.cloth_count; i++)
                    io_write_offset_x_pof_add(s, 0, &pof);
            }
            io_align_write(s, 0x10);
        }

        exh.strings_offset = io_get_position(s);
        if (!is_x)
            for (string* i = string_set.begin; i != string_set.end; i++)
                io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
        else
            for (string* i = string_set.begin; i != string_set.end; i++)
                io_write_offset_x_pof_add(s, 0, &pof);
        io_align_write(s, 0x10);

        if (blocks_count > 0) {
            exh.blocks_offset = io_get_position(s);
            if (!is_x) {
                for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++) {
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                    io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                }
                io_write_offset_f2(s, 0, 0x20);
                io_write_offset_f2(s, 0, 0x20);
            }
            else {
                for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++) {
                    io_write_offset_x_pof_add(s, 0, &pof);
                    io_write_offset_x_pof_add(s, 0, &pof);
                }
                io_write_offset_x(s, 0);
                io_write_offset_x(s, 0);
            }
            io_align_write(s, 0x10);

            bhs = force_malloc_s(obj_skin_block_header, blocks_count);
            if (!is_x)
                for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                    switch (i->type) {
                    case OBJ_SKIN_BLOCK_OSAGE:
                        bhs[i - ex->blocks.begin].block_offset = io_get_position(s);
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                        io_write(s, 0, 0x24);
                        io_write(s, 0, 0x24);
                        break;
                    }
            else
                for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                    switch (i->type) {
                    case OBJ_SKIN_BLOCK_OSAGE:
                        bhs[i - ex->blocks.begin].block_offset = io_get_position(s);
                        io_write_offset_x_pof_add(s, 0, &pof);
                        io_write(s, 0, 0x28);
                        if (string_length(&i->osage.motion_node_name)) {
                            io_write(s, 0, 0x18);
                            io_write_offset_x_pof_add(s, 0, &pof);
                            io_write(s, 0, 0x18);
                        }
                        else
                            io_write(s, 0, 0x38);
                        break;
                    }
            io_align_write(s, 0x10);

            if (!is_x)
                for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                    switch (i->type) {
                    case OBJ_SKIN_BLOCK_EXPRESSION:
                        bhs[i - ex->blocks.begin].block_offset = io_get_position(s);
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                        io_write(s, 0, 0x24);
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                        io_write(s, 0, 0x04);

                        for (int32_t j = 0; j < i->expression.expression_count && j < 9; j++)
                            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);

                        for (int32_t j = i->expression.expression_count; j < 9; j++)
                            io_write(s, 0, 0x04);
                        break;
                    }
            else
                for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                    switch (i->type) {
                    case OBJ_SKIN_BLOCK_EXPRESSION:
                        bhs[i - ex->blocks.begin].block_offset = io_get_position(s);
                        io_write_offset_x_pof_add(s, 0, &pof);
                        io_write(s, 0, 0x28);
                        io_write_offset_x_pof_add(s, 0, &pof);
                        io_write(s, 0, 0x08);

                        for (int32_t j = 0; j < i->expression.expression_count && j < 9; j++)
                            io_write_offset_x_pof_add(s, 0, &pof);

                        for (int32_t j = i->expression.expression_count; j < 9; j++)
                            io_write(s, 0, 0x08);
                        break;
                    }
            io_align_write(s, 0x10);

            if (!is_x)
                for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                    switch (i->type) {
                    case OBJ_SKIN_BLOCK_CLOTH:
                        bhs[i - ex->blocks.begin].block_offset = io_get_position(s);
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                        io_write(s, 0, 0x10);

                        if (i->cloth.field_10)
                            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                        else
                            io_write(s, 0, 0x04);

                        if (i->cloth.field_1C.end - i->cloth.field_1C.begin)
                            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                        else
                            io_write(s, 0, 0x04);

                        if (i->cloth.field_20.end - i->cloth.field_20.begin)
                            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                        else
                            io_write(s, 0, 0x04);

                        if (i->cloth.field_24.end - i->cloth.field_24.begin)
                            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                        else
                            io_write(s, 0, 0x04);

                        if (i->cloth.field_28.end - i->cloth.field_28.begin)
                            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                        else
                            io_write(s, 0, 0x04);

                        io_write(s, 0, 0x08);
                        break;
                    }
            else
                for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                    switch (i->type) {
                    case OBJ_SKIN_BLOCK_CLOTH:
                        bhs[i - ex->blocks.begin].block_offset = io_get_position(s);
                        io_write_offset_x_pof_add(s, 0, &pof);
                        io_write_offset_x_pof_add(s, 0, &pof);
                        io_write(s, 0, 0x10);

                        if (i->cloth.field_10)
                            io_write_offset_x_pof_add(s, 0, &pof);
                        else
                            io_write(s, 0, 0x08);

                        if (i->cloth.field_1C.end - i->cloth.field_1C.begin)
                            io_write_offset_x_pof_add(s, 0, &pof);
                        else
                            io_write(s, 0, 0x08);

                        if (i->cloth.field_20.end - i->cloth.field_20.begin)
                            io_write_offset_x_pof_add(s, 0, &pof);
                        else
                            io_write(s, 0, 0x08);

                        if (i->cloth.field_24.end - i->cloth.field_24.begin)
                            io_write_offset_x_pof_add(s, 0, &pof);
                        else
                            io_write(s, 0, 0x08);

                        if (i->cloth.field_28.end - i->cloth.field_28.begin)
                            io_write_offset_x_pof_add(s, 0, &pof);
                        else
                            io_write(s, 0, 0x08);

                        io_write(s, 0, 0x10);
                        break;
                    }
            io_align_write(s, 0x10);

            if (!is_x)
                for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                    switch (i->type) {
                    case OBJ_SKIN_BLOCK_CONSTRAINT:
                        bhs[i - ex->blocks.begin].block_offset = io_get_position(s);
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                        io_write(s, 0, 0x24);
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                        io_write(s, 0, 0x04);
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                        switch (i->constraint.type) {
                        case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
                            io_write(s, 0, 0x20);
                            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                            io_write(s, 0, 0x18);
                            break;
                        case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
                            io_write(s, 0, 0x20);
                            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                            io_write(s, 0, 0x04);
                            io_write(s, 0, 0x14);
                            io_write(s, 0, 0x14);
                            break;
                        case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
                            io_write(s, 0, 0x0C);
                            break;
                        case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION:
                            io_write(s, 0, 0x20);
                            io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                            io_write(s, 0, 0x14);
                            io_write(s, 0, 0x14);
                            break;
                        }
                        break;
                    }
            else
                for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                    switch (i->type) {
                    case OBJ_SKIN_BLOCK_CONSTRAINT:
                        bhs[i - ex->blocks.begin].block_offset = io_get_position(s);
                        io_write_offset_x_pof_add(s, 0, &pof);
                        io_write(s, 0, 0x28);
                        io_write_offset_x_pof_add(s, 0, &pof);
                        io_write_offset_x_pof_add(s, 0, &pof);
                        io_write(s, 0, 0x08);
                        io_write_offset_x_pof_add(s, 0, &pof);
                        switch (i->constraint.type) {
                        case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
                            io_write(s, 0, 0x20);
                            io_write_offset_x_pof_add(s, 0, &pof);
                            io_write(s, 0, 0x18);
                            break;
                        case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
                            io_write(s, 0, 0x20);
                            io_write_offset_x_pof_add(s, 0, &pof);
                            io_write(s, 0, 0x04);
                            io_write(s, 0, 0x14);
                            io_write(s, 0, 0x14);
                            io_write(s, 0, 0x04);
                            break;
                        case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
                            io_write(s, 0, 0x0C);
                            io_write(s, 0, 0x04);
                            break;
                        case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION:
                            io_write(s, 0, 0x20);
                            io_write_offset_x_pof_add(s, 0, &pof);
                            io_write(s, 0, 0x14);
                            io_write(s, 0, 0x14);
                            break;
                        }
                        break;
                    }
            io_align_write(s, 0x10);

            if (!is_x)
                for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                    switch (i->type) {
                    case OBJ_SKIN_BLOCK_MOTION:
                        bhs[i - ex->blocks.begin].block_offset = io_get_position(s);
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                        io_write(s, 0, 0x24);
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                        io_write(s, 0, 0x04);
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                        io_write_offset_f2_pof_add(s, 0, 0x20, &pof);
                        break;
                    }
            else
                for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                    switch (i->type) {
                    case OBJ_SKIN_BLOCK_MOTION:
                        bhs[i - ex->blocks.begin].block_offset = io_get_position(s);
                        io_write_offset_x_pof_add(s, 0, &pof);
                        io_write(s, 0, 0x28);
                        io_write_offset_x_pof_add(s, 0, &pof);
                        io_write(s, 0, 0x08);
                        io_write_offset_x_pof_add(s, 0, &pof);
                        io_write_offset_x_pof_add(s, 0, &pof);
                        break;
                    }
            io_align_write(s, 0x10);

            if (motion_count) {
                motion_block_node_mats = io_get_position(s);
                for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                    if (i->type == OBJ_SKIN_BLOCK_MOTION)
                        io_write(s, 0, (i->motion.nodes.end - i->motion.nodes.begin) * sizeof(mat4));
                io_align_write(s, 0x10);

                motion_block_node_name_offset = io_get_position(s);
                for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                    if (i->type == OBJ_SKIN_BLOCK_MOTION)
                        io_write(s, 0, (i->motion.nodes.end - i->motion.nodes.begin) * sizeof(uint32_t));
                io_align_write(s, 0x10);
            }

            if (exh.cloth_count) {
                field_18 = io_get_position(s);
                for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                    if (i->type == OBJ_SKIN_BLOCK_CLOTH)
                        for (int32_t j = 0; j < 32; j++)
                            if (i->cloth.field_10 & (1 << j))
                                io_write(s, 0, sizeof(mat4));
                io_align_write(s, 0x10);

                field_1C = io_get_position(s);
                if (!is_x) {
                    for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                        if (i->type == OBJ_SKIN_BLOCK_CLOTH)
                            io_write(s, 0, (10 * sizeof(int32_t)
                                + 4 * (sizeof(int32_t) + 3 * sizeof(int32_t)))
                                * (i->cloth.field_1C.end - i->cloth.field_1C.begin));
                }
                else {
                    for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                        if (i->type == OBJ_SKIN_BLOCK_CLOTH)
                            io_write(s, 0, (10 * sizeof(int32_t)
                                + 4 * (sizeof(int64_t) + 4 * sizeof(int32_t)))
                                * (i->cloth.field_1C.end - i->cloth.field_1C.begin));
                }
                io_align_write(s, 0x10);

                field_20 = io_get_position(s);
                for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                    if (i->type == OBJ_SKIN_BLOCK_CLOTH)
                        io_write(s, 0, 10 * (11 * sizeof(int32_t))
                            * (i->cloth.field_20.end - i->cloth.field_20.begin));
                io_align_write(s, 0x10);

                field_24 = io_get_position(s);
                for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                    if (i->type == OBJ_SKIN_BLOCK_CLOTH)
                        io_write(s, 0, sizeof(uint16_t) + (i->cloth.field_24.end
                            - i->cloth.field_24.begin) * sizeof(uint16_t));
                io_align_write(s, 0x10);

                field_28 = io_get_position(s);
                for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                    if (i->type == OBJ_SKIN_BLOCK_CLOTH)
                        io_write(s, 0, sizeof(uint16_t) + (i->cloth.field_28.end
                            - i->cloth.field_28.begin) * sizeof(uint16_t));
                io_align_write(s, 0x10);
            }
        }
    }

    if (sh.bone_count || sk->ex_data_init) {
        for (string* i = strings.begin; i != &strings.end[-1]; i++) {
            char* i_str = string_data(i);
            for (string* j = i + 1; j != strings.end; j++)
                if (strcmp(i_str, string_data(j)) > 0) {
                    string temp = *i;
                    *i = *j;
                    *j = temp;
                    i_str = string_data(i);
                }
        }

        for (string* i = strings.begin; i != strings.end; i++) {
            ssize_t off = io_get_position(s);
            io_write(s, string_data(i), string_length(i) + 1);
            vector_ssize_t_push_back(&string_offsets, &off);
        }
    }
    io_align_write(s, 0x10);

    vector_string osage_names = vector_empty(string);
    if (sk->ex_data_init) {
        obj_skin_ex_data* ex = &sk->ex_data;

        size_t cls_offset = 0;
        size_t cns_offset = 0;
        size_t exp_offset = 0;
        size_t mot_offset = 0;
        size_t osg_offset = 0;
        for (string* i = strings.begin; i != strings.end; i++)
            if (!strcmp(string_data(i), "CLS"))
                cls_offset = string_offsets.begin[i - strings.begin];

        for (string* i = strings.begin; i != strings.end; i++)
            if (!strcmp(string_data(i), "CNS"))
                cns_offset = string_offsets.begin[i - strings.begin];

        for (string* i = strings.begin; i != strings.end; i++)
            if (!strcmp(string_data(i), "EXP"))
                exp_offset = string_offsets.begin[i - strings.begin];

        for (string* i = strings.begin; i != strings.end; i++)
            if (!strcmp(string_data(i), "MOT"))
                mot_offset = string_offsets.begin[i - strings.begin];

        for (string* i = strings.begin; i != strings.end; i++)
            if (!strcmp(string_data(i), "OSG"))
                osg_offset = string_offsets.begin[i - strings.begin];

        if (blocks_count > 0) {
            for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++) {
                if (i->type != OBJ_SKIN_BLOCK_CLOTH)
                    continue;

                io_position_push(s, bhs[i - ex->blocks.begin].block_offset, SEEK_SET);
                obj_modern_write_skin_block_cloth(&i->cloth, s, &strings, &string_offsets,
                    is_x, &field_18, &field_1C, &field_20, &field_24, &field_28);
                io_position_pop(s);
            }

            ssize_t constraint_type_name_offsets[4];
            constraint_type_name_offsets[0] = obj_skin_strings_get_string_offset(&strings,
                &string_offsets, "Direction");
            constraint_type_name_offsets[1] = obj_skin_strings_get_string_offset(&strings,
                &string_offsets, "Distance");
            constraint_type_name_offsets[2] = obj_skin_strings_get_string_offset(&strings,
                &string_offsets, "Orientation");
            constraint_type_name_offsets[3] = obj_skin_strings_get_string_offset(&strings,
                &string_offsets, "Position");

            for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++) {
                if (i->type != OBJ_SKIN_BLOCK_CONSTRAINT)
                    continue;

                io_position_push(s, bhs[i - ex->blocks.begin].block_offset, SEEK_SET);
                obj_modern_write_skin_block_constraint(&i->constraint,
                    s, &strings, &string_offsets, is_x, constraint_type_name_offsets);
                io_position_pop(s);
            }

            for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++) {
                if (i->type != OBJ_SKIN_BLOCK_EXPRESSION)
                    continue;

                io_position_push(s, bhs[i - ex->blocks.begin].block_offset, SEEK_SET);
                obj_modern_write_skin_block_expression(&i->expression,
                    s, &strings, &string_offsets, is_x);
                io_position_pop(s);
            }

            for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++) {
                if (i->type != OBJ_SKIN_BLOCK_MOTION)
                    continue;

                io_position_push(s, bhs[i - ex->blocks.begin].block_offset, SEEK_SET);
                obj_modern_write_skin_block_motion(&i->motion,
                    s, &strings, &string_offsets, is_x, &string_set,
                    &motion_block_node_name_offset, &motion_block_node_mats);
                io_position_pop(s);
            }

            for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++) {
                if (i->type != OBJ_SKIN_BLOCK_OSAGE)
                    continue;

                io_position_push(s, bhs[i - ex->blocks.begin].block_offset, SEEK_SET);
                obj_modern_write_skin_block_osage(&i->osage,
                    s, &strings, &string_offsets, is_x, &string_set);
                io_position_pop(s);
            }

            for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                if (i->type == OBJ_SKIN_BLOCK_OSAGE)
                    obj_skin_strings_push_back_check(&osage_names, string_data(&i->osage.external_name));
                else if (i->type == OBJ_SKIN_BLOCK_CLOTH)
                    obj_skin_strings_push_back_check(&osage_names, string_data(&i->cloth.mesh_name));

            io_position_push(s, exh.blocks_offset, SEEK_SET);
            if (!is_x) {
                for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                switch (i->type) {
                    case OBJ_SKIN_BLOCK_CLOTH:
                        io_write_offset_f2(s, cls_offset, 0x20);
                        io_write_offset_f2(s, bhs[i - ex->blocks.begin].block_offset, 0x20);
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT:
                        io_write_offset_f2(s, cns_offset, 0x20);
                        io_write_offset_f2(s, bhs[i - ex->blocks.begin].block_offset, 0x20);
                        break;
                    case OBJ_SKIN_BLOCK_EXPRESSION:
                        io_write_offset_f2(s, exp_offset, 0x20);
                        io_write_offset_f2(s, bhs[i - ex->blocks.begin].block_offset, 0x20);
                        break;
                    case OBJ_SKIN_BLOCK_MOTION:
                        io_write_offset_f2(s, mot_offset, 0x20);
                        io_write_offset_f2(s, bhs[i - ex->blocks.begin].block_offset, 0x20);
                        break;
                    case OBJ_SKIN_BLOCK_OSAGE:
                        io_write_offset_f2(s, osg_offset, 0x20);
                        io_write_offset_f2(s, bhs[i - ex->blocks.begin].block_offset, 0x20);
                        break;
                    default:
                        io_write_offset_f2(s, 0, 0);
                        io_write_offset_f2(s, 0, 0);
                        break;
                    }
                io_write_offset_f2(s, 0, 0);
                io_write_offset_f2(s, 0, 0);
            }
            else {
                for (obj_skin_block* i = ex->blocks.begin; i != ex->blocks.end; i++)
                    switch (i->type) {
                    case OBJ_SKIN_BLOCK_CLOTH:
                        io_write_offset_x(s, cls_offset);
                        io_write_offset_x(s, bhs[i - ex->blocks.begin].block_offset);
                        break;
                    case OBJ_SKIN_BLOCK_CONSTRAINT:
                        io_write_offset_x(s, cns_offset);
                        io_write_offset_x(s, bhs[i - ex->blocks.begin].block_offset);
                        break;
                    case OBJ_SKIN_BLOCK_EXPRESSION:
                        io_write_offset_x(s, exp_offset);
                        io_write_offset_x(s, bhs[i - ex->blocks.begin].block_offset);
                        break;
                    case OBJ_SKIN_BLOCK_MOTION:
                        io_write_offset_x(s, mot_offset);
                        io_write_offset_x(s, bhs[i - ex->blocks.begin].block_offset);
                        break;
                    case OBJ_SKIN_BLOCK_OSAGE:
                        io_write_offset_x(s, osg_offset);
                        io_write_offset_x(s, bhs[i - ex->blocks.begin].block_offset);
                        break;
                    default:
                        io_write_offset_x(s, 0);
                        io_write_offset_x(s, 0);
                        break;
                    }
                io_write_offset_x(s, 0);
                io_write_offset_x(s, 0);
            }
            io_position_pop(s);
            free(bhs);
        }
    }

    if (sh.bone_count) {
        io_position_push(s, sh.bone_names_offset, SEEK_SET);
        if (!is_x)
            for (int32_t i = 0; i < sh.bone_count; i++) {
                size_t str_offset = obj_skin_strings_get_string_offset(&strings,
                    &string_offsets, string_data(&sk->bones.begin[i].name));
                io_write_offset_f2(s, str_offset, 0x20);
            }
        else
            for (int32_t i = 0; i < sh.bone_count; i++) {
                size_t str_offset = obj_skin_strings_get_string_offset(&strings,
                    &string_offsets, string_data(&sk->bones.begin[i].name));
                io_write_offset_x(s, str_offset);
            }
        io_position_pop(s);
    }

    if (sk->ex_data_init) {
        obj_skin_ex_data* ex = &sk->ex_data;

        if (blocks_count > 0) {
            io_position_push(s, exh.strings_offset, SEEK_SET);
            if (!is_x)
                for (string* i = string_set.begin; i != string_set.end; i++) {
                    size_t str_offset = obj_skin_strings_get_string_offset(&strings,
                        &string_offsets, string_data(i));
                    io_write_offset_f2(s, str_offset, 0x20);
                }
            else
                for (string* i = string_set.begin; i != string_set.end; i++) {
                    size_t str_offset = obj_skin_strings_get_string_offset(&strings,
                        &string_offsets, string_data(i));
                    io_write_offset_x(s, str_offset);
                }
            io_position_pop(s);

            exh.osage_nodes_count = (int32_t)(ex->osage_nodes.end - ex->osage_nodes.begin);
            io_position_push(s, exh.osage_nodes_offset, SEEK_SET);
            for (obj_skin_osage_node* i = ex->osage_nodes.begin; i != ex->osage_nodes.end; i++) {
                uint32_t index = obj_skin_strings_get_string_index(&string_set, string_data(&i->name));
                io_write_uint32_t(s, index);
                io_write_float_t(s, i->length);
                io_write_uint32_t(s, 0);
            }
            io_position_pop(s);

            io_position_push(s, exh.osage_sibling_infos_offset, SEEK_SET);
            for (obj_skin_osage_node* i = ex->osage_nodes.begin; i != ex->osage_nodes.end; i++) {
                if (!string_length(&i->sibling_name))
                    continue;

                uint32_t name = obj_skin_strings_get_string_index(&string_set, string_data(&i->name));
                uint32_t sibling_name = obj_skin_strings_get_string_index(&string_set, string_data(&i->sibling_name));
                io_write_uint32_t(s, name);
                io_write_uint32_t(s, sibling_name);
                io_write_float_t(s, i->sibling_max_distance);
            }
            io_write_int32_t(s, 0);
            io_write_int32_t(s, 0);
            io_write_int32_t(s, 0);
            io_position_pop(s);

            exh.osage_count = (int32_t)(osage_names.end - osage_names.begin);
            exh.osage_count -= exh.cloth_count;
            io_position_push(s, exh.osage_names_offset, SEEK_SET);
            if (!is_x)
                for (string* i = osage_names.begin; i != osage_names.end; i++) {
                    size_t str_offset = obj_skin_strings_get_string_offset(&strings,
                        &string_offsets, string_data(i));
                    io_write_offset_f2(s, str_offset, 0x20);
                }
            else
                for (string* i = osage_names.begin; i != osage_names.end; i++) {
                    size_t str_offset = obj_skin_strings_get_string_offset(&strings,
                        &string_offsets, string_data(i));
                    io_write_offset_x(s, str_offset);
                }
            io_position_pop(s);
        }

        for (string* i = osage_names.begin; i != osage_names.end; i++)
            string_free(i);
        vector_string_free(&osage_names);
    }

    for (string* i = strings.begin; i != strings.end; i++)
        string_free(i);
    vector_string_free(&strings);

    vector_ssize_t_free(&string_offsets);

    for (string* i = string_set.begin; i != string_set.end; i++)
        string_free(i);
    vector_string_free(&string_set);

    if (sk->ex_data_init) {
        io_position_push(s, sh.ex_data_offset, SEEK_SET);
        if (!is_x) {
            io_write_int32_t(s, exh.osage_count);
            io_write_int32_t(s, exh.osage_nodes_count);
            io_write(s, 0, 0x04);
            io_write_offset_f2(s, exh.osage_nodes_offset, 0x20);
            io_write_offset_f2(s, exh.osage_names_offset, 0x20);
            io_write_offset_f2(s, exh.blocks_offset, 0x20);
            io_write_int32_t(s, exh.strings_count);
            io_write_offset_f2(s, exh.strings_offset, 0x20);
            io_write_offset_f2(s, exh.osage_sibling_infos_offset, 0x20);
            io_write_int32_t(s, exh.cloth_count);
            io_write(s, 0, 0x1C);
        }
        else {
            io_write_int32_t(s, exh.osage_count);
            io_write_int32_t(s, exh.osage_nodes_count);
            io_write(s, 0, 0x08);
            io_write_offset_x(s, exh.osage_nodes_offset);
            io_write_offset_x(s, exh.osage_names_offset);
            io_write_offset_x(s, exh.blocks_offset);
            io_write_int32_t(s, exh.strings_count);
            io_write_offset_x(s, exh.strings_offset);
            io_write_offset_x(s, exh.osage_sibling_infos_offset);
            io_write_int32_t(s, exh.cloth_count);
            io_write(s, 0, 0x04);
            io_write(s, 0, 0x38);
        }
        io_position_pop(s);
    }

    io_position_push(s, base_offset, SEEK_SET);
    if (!is_x) {
        io_write_offset_f2(s, sh.bone_ids_offset, 0x20);
        io_write_offset_f2(s, sh.bone_matrices_offset, 0x20);
        io_write_offset_f2(s, sh.bone_names_offset, 0x20);
        io_write_offset_f2(s, sh.ex_data_offset, 0x20);
        io_write_int32_t(s, sh.bone_count);
        io_write_offset_f2(s, sh.bone_parent_ids_offset, 0x20);
        io_write(s, 0, 0x0C);
    }
    else {
        io_write_offset_x(s, sh.bone_ids_offset);
        io_write_offset_x(s, sh.bone_matrices_offset);
        io_write_offset_x(s, sh.bone_names_offset);
        io_write_offset_x(s, sh.ex_data_offset);
        io_write_int32_t(s, sh.bone_count);
        io_write_offset_x(s, sh.bone_parent_ids_offset);
        io_write(s, 0, 0x18);
    }
    io_position_pop(s);

    oskn->enrs = e;
    oskn->pof = pof;
}

static void obj_modern_read_skin_block_cloth(obj_skin_block_cloth* b,
    stream* s, uint32_t header_length, string* str, bool is_x) {
    ssize_t mesh_name_offset = io_read_offset(s, header_length, is_x);
    io_read_string_null_terminated_offset(s, mesh_name_offset, &b->mesh_name);

    ssize_t backface_mesh_name_offset = io_read_offset(s, header_length, is_x);
    io_read_string_null_terminated_offset(s, backface_mesh_name_offset, &b->backface_mesh_name);

    b->field_08 = io_read_int32_t_stream_reverse_endianness(s);
    int32_t count = io_read_int32_t_stream_reverse_endianness(s);
    b->field_10 = io_read_int32_t_stream_reverse_endianness(s);
    b->field_14 = io_read_int32_t_stream_reverse_endianness(s);
    ssize_t field_18_offset = io_read_offset(s, header_length, is_x);
    ssize_t field_1C_offset = io_read_offset(s, header_length, is_x);

    b->field_1C = vector_empty(obj_skin_block_cloth_field_1C);
    b->field_20 = vector_empty(obj_skin_block_cloth_field_20);
    b->field_24 = vector_empty(uint16_t);
    b->field_28 = vector_empty(uint16_t);
    ssize_t field_20_offset = io_read_offset(s, header_length, is_x);
    ssize_t field_24_offset = io_read_offset(s, header_length, is_x);
    ssize_t field_28_offset = io_read_offset(s, header_length, is_x);
    b->field_2C = io_read_uint32_t_stream_reverse_endianness(s);
    b->field_30 = io_read_uint32_t_stream_reverse_endianness(s);

    if (field_18_offset) {
        io_position_push(s, field_18_offset, SEEK_SET);
        for (int32_t i = 0; i < 32; i++) {
            if (!(b->field_10 & (1 << i)))
                continue;

            mat4 mat;
            mat.row0.x = io_read_float_t_stream_reverse_endianness(s);
            mat.row0.y = io_read_float_t_stream_reverse_endianness(s);
            mat.row0.z = io_read_float_t_stream_reverse_endianness(s);
            mat.row0.w = io_read_float_t_stream_reverse_endianness(s);
            mat.row1.x = io_read_float_t_stream_reverse_endianness(s);
            mat.row1.y = io_read_float_t_stream_reverse_endianness(s);
            mat.row1.z = io_read_float_t_stream_reverse_endianness(s);
            mat.row1.w = io_read_float_t_stream_reverse_endianness(s);
            mat.row2.x = io_read_float_t_stream_reverse_endianness(s);
            mat.row2.y = io_read_float_t_stream_reverse_endianness(s);
            mat.row2.z = io_read_float_t_stream_reverse_endianness(s);
            mat.row2.w = io_read_float_t_stream_reverse_endianness(s);
            mat.row3.x = io_read_float_t_stream_reverse_endianness(s);
            mat.row3.y = io_read_float_t_stream_reverse_endianness(s);
            mat.row3.z = io_read_float_t_stream_reverse_endianness(s);
            mat.row3.w = io_read_float_t_stream_reverse_endianness(s);
            mat4_transpose(&mat, &b->field_18[i]);
        }
        io_position_pop(s);
    }

    if (field_1C_offset) {
        io_position_push(s, field_1C_offset, SEEK_SET);
        vector_obj_skin_block_cloth_field_1C_reserve(&b->field_1C, count);
        for (int32_t i = 0; i < count; i++) {
            obj_skin_block_cloth_field_1C f;
            f.field_00 = io_read_float_t_stream_reverse_endianness(s);
            f.field_04 = io_read_float_t_stream_reverse_endianness(s);
            f.field_08 = io_read_float_t_stream_reverse_endianness(s);
            f.field_0C = io_read_float_t_stream_reverse_endianness(s);
            f.field_10 = io_read_float_t_stream_reverse_endianness(s);
            f.field_14 = io_read_float_t_stream_reverse_endianness(s);
            f.field_18 = io_read_float_t_stream_reverse_endianness(s);
            f.field_1C = io_read_uint32_t_stream_reverse_endianness(s);
            f.field_20 = io_read_uint32_t_stream_reverse_endianness(s);
            f.field_24 = io_read_uint32_t_stream_reverse_endianness(s);
            obj_modern_read_skin_block_cloth_field_1C_sub(&f.sub_data_0, s, header_length, str, is_x);
            obj_modern_read_skin_block_cloth_field_1C_sub(&f.sub_data_1, s, header_length, str, is_x);
            obj_modern_read_skin_block_cloth_field_1C_sub(&f.sub_data_2, s, header_length, str, is_x);
            obj_modern_read_skin_block_cloth_field_1C_sub(&f.sub_data_3, s, header_length, str, is_x);
            vector_obj_skin_block_cloth_field_1C_push_back(&b->field_1C, &f);
        }
        io_position_pop(s);
    }

    if (field_20_offset) {
        io_position_push(s, field_20_offset, SEEK_SET);
        vector_obj_skin_block_cloth_field_20_reserve(&b->field_20, count);
        for (int32_t i = 0; i < count; i++) {
            obj_skin_block_cloth_field_20 f;
            obj_modern_read_skin_block_cloth_field_20_sub(&f.sub_data_0, s, header_length, str, is_x);
            obj_modern_read_skin_block_cloth_field_20_sub(&f.sub_data_1, s, header_length, str, is_x);
            obj_modern_read_skin_block_cloth_field_20_sub(&f.sub_data_2, s, header_length, str, is_x);
            obj_modern_read_skin_block_cloth_field_20_sub(&f.sub_data_3, s, header_length, str, is_x);
            obj_modern_read_skin_block_cloth_field_20_sub(&f.sub_data_4, s, header_length, str, is_x);
            obj_modern_read_skin_block_cloth_field_20_sub(&f.sub_data_5, s, header_length, str, is_x);
            obj_modern_read_skin_block_cloth_field_20_sub(&f.sub_data_6, s, header_length, str, is_x);
            obj_modern_read_skin_block_cloth_field_20_sub(&f.sub_data_7, s, header_length, str, is_x);
            obj_modern_read_skin_block_cloth_field_20_sub(&f.sub_data_8, s, header_length, str, is_x);
            obj_modern_read_skin_block_cloth_field_20_sub(&f.sub_data_9, s, header_length, str, is_x);
            vector_obj_skin_block_cloth_field_20_push_back(&b->field_20, &f);
        }
        io_position_pop(s);
    }

    if (field_24_offset) {
        io_position_push(s, field_24_offset, SEEK_SET);
        count = io_read_uint16_t_stream_reverse_endianness(s);
        vector_uint16_t_reserve(&b->field_24, count);
        for (int32_t i = 0; i < count; i++) {
            uint16_t u = io_read_uint16_t_stream_reverse_endianness(s);
            vector_uint16_t_push_back(&b->field_24, &u);
        }
        io_position_pop(s);
    }

    if (field_28_offset) {
        io_position_push(s, field_28_offset, SEEK_SET);
        count = io_read_uint16_t_stream_reverse_endianness(s);
        vector_uint16_t_reserve(&b->field_28, count);
        for (int32_t i = 0; i < count; i++) {
            uint16_t u = io_read_uint16_t_stream_reverse_endianness(s);
            vector_uint16_t_push_back(&b->field_28, &u);
        }
        io_position_pop(s);
    }

}

static void obj_modern_write_skin_block_cloth(obj_skin_block_cloth* b,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets, bool is_x,
    ssize_t* field_18_offset, ssize_t* field_1C_offset, ssize_t* field_20_offset,
    ssize_t* field_24_offset, ssize_t* field_28_offset) {
    int32_t count = (int32_t)(b->field_1C.end - b->field_1C.begin);
    ssize_t mesh_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, string_data(&b->mesh_name));
    ssize_t backface_mesh_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, string_data(&b->backface_mesh_name));

    if (!is_x) {
        io_write_offset_f2(s, mesh_name_offset, 0x20);
        io_write_offset_f2(s, backface_mesh_name_offset, 0x20);
        io_write_int32_t(s, b->field_08);
        io_write_int32_t(s, count);
        io_write_int32_t(s, b->field_10);
        io_write_int32_t(s, b->field_14);
        io_write_offset_f2(s, b->field_10 ? *field_18_offset : 0, 0x20);
        io_write_offset_f2(s, b->field_1C.end - b->field_1C.begin ? *field_1C_offset : 0, 0x20);
        io_write_offset_f2(s, b->field_20.end - b->field_20.begin ? *field_20_offset : 0, 0x20);
        io_write_offset_f2(s, b->field_24.end - b->field_24.begin ? *field_24_offset : 0, 0x20);
        io_write_offset_f2(s, b->field_28.end - b->field_28.begin ? *field_28_offset : 0, 0x20);
        io_write_uint32_t(s, b->field_2C);
        io_write_uint32_t(s, b->field_30);
    }
    else {
        io_write_offset_x(s, mesh_name_offset);
        io_write_offset_x(s, backface_mesh_name_offset);
        io_write_int32_t(s, b->field_08);
        io_write_int32_t(s, count);
        io_write_int32_t(s, b->field_10);
        io_write_int32_t(s, b->field_14);
        io_write_offset_x(s, b->field_10 ? *field_18_offset : 0);
        io_write_offset_x(s, b->field_1C.end - b->field_1C.begin ? *field_1C_offset : 0);
        io_write_offset_x(s, b->field_20.end - b->field_20.begin ? *field_20_offset : 0);
        io_write_offset_x(s, b->field_24.end - b->field_24.begin ? *field_24_offset : 0);
        io_write_offset_x(s, b->field_28.end - b->field_28.begin ? *field_28_offset : 0);
        io_write_uint32_t(s, b->field_2C);
        io_write_uint32_t(s, b->field_30);
    }

    if (b->field_10) {
        io_position_push(s, *field_18_offset, SEEK_SET);
        for (int32_t i = 0; i < 32; i++) {
            if (!(b->field_10 & (1 << i)))
                continue;

            mat4 mat;
            mat4_transpose(&b->field_18[i], &mat);
            io_write_float_t(s, mat.row0.x);
            io_write_float_t(s, mat.row0.y);
            io_write_float_t(s, mat.row0.z);
            io_write_float_t(s, mat.row0.w);
            io_write_float_t(s, mat.row1.x);
            io_write_float_t(s, mat.row1.y);
            io_write_float_t(s, mat.row1.z);
            io_write_float_t(s, mat.row1.w);
            io_write_float_t(s, mat.row2.x);
            io_write_float_t(s, mat.row2.y);
            io_write_float_t(s, mat.row2.z);
            io_write_float_t(s, mat.row2.w);
            io_write_float_t(s, mat.row3.x);
            io_write_float_t(s, mat.row3.y);
            io_write_float_t(s, mat.row3.z);
            io_write_float_t(s, mat.row3.w);
            *field_18_offset += sizeof(mat4);
        }
        io_position_pop(s);
    }

    if (b->field_1C.end - b->field_1C.begin) {
        io_position_push(s, *field_1C_offset, SEEK_SET);
        for (obj_skin_block_cloth_field_1C* i = b->field_1C.begin; i != b->field_1C.end; i++) {
            io_write_float_t(s, i->field_00);
            io_write_float_t(s, i->field_04);
            io_write_float_t(s, i->field_08);
            io_write_float_t(s, i->field_0C);
            io_write_float_t(s, i->field_10);
            io_write_float_t(s, i->field_14);
            io_write_float_t(s, i->field_18);
            io_write_uint32_t(s, i->field_1C);
            io_write_uint32_t(s, i->field_20);
            io_write_uint32_t(s, i->field_24);
            obj_modern_write_skin_block_cloth_field_1C_sub(&i->sub_data_0,
                s, strings, string_offsets, is_x);
            obj_modern_write_skin_block_cloth_field_1C_sub(&i->sub_data_1,
                s, strings, string_offsets, is_x);
            obj_modern_write_skin_block_cloth_field_1C_sub(&i->sub_data_2,
                s, strings, string_offsets, is_x);
            obj_modern_write_skin_block_cloth_field_1C_sub(&i->sub_data_3,
                s, strings, string_offsets, is_x);
        }
        io_position_pop(s);
        if (!is_x)
            *field_1C_offset += (10 * sizeof(int32_t)
                + 4 * (sizeof(int64_t) + 4 * sizeof(int32_t))) * count;
        else
            *field_1C_offset += (10 * sizeof(int32_t)
                + 4 * (sizeof(int32_t) + 3 * sizeof(int32_t))) * count;
    }

    if (b->field_20.end - b->field_20.begin) {
        io_position_push(s, *field_20_offset, SEEK_SET);
        for (obj_skin_block_cloth_field_20* i = b->field_20.begin; i != b->field_20.end; i++) {
            obj_modern_write_skin_block_cloth_field_20_sub(&i->sub_data_0,
                s, strings, string_offsets, is_x);
            obj_modern_write_skin_block_cloth_field_20_sub(&i->sub_data_1,
                s, strings, string_offsets, is_x);
            obj_modern_write_skin_block_cloth_field_20_sub(&i->sub_data_2,
                s, strings, string_offsets, is_x);
            obj_modern_write_skin_block_cloth_field_20_sub(&i->sub_data_3,
                s, strings, string_offsets, is_x);
            obj_modern_write_skin_block_cloth_field_20_sub(&i->sub_data_4,
                s, strings, string_offsets, is_x);
            obj_modern_write_skin_block_cloth_field_20_sub(&i->sub_data_5,
                s, strings, string_offsets, is_x);
            obj_modern_write_skin_block_cloth_field_20_sub(&i->sub_data_6,
                s, strings, string_offsets, is_x);
            obj_modern_write_skin_block_cloth_field_20_sub(&i->sub_data_7,
                s, strings, string_offsets, is_x);
            obj_modern_write_skin_block_cloth_field_20_sub(&i->sub_data_8,
                s, strings, string_offsets, is_x);
            obj_modern_write_skin_block_cloth_field_20_sub(&i->sub_data_9,
                s, strings, string_offsets, is_x);
        }
        io_position_pop(s);
        *field_20_offset += 10 * (11 * sizeof(int32_t)) * count;
    }

    if (b->field_24.end - b->field_24.begin) {
        io_position_push(s, *field_24_offset, SEEK_SET);
        io_write_uint16_t(s, (uint16_t)(b->field_24.end - b->field_24.begin));
        for (uint16_t* i = b->field_24.begin; i != b->field_24.end; i++)
            io_write_uint16_t(s, *i);
        io_position_pop(s);
        *field_24_offset += sizeof(uint16_t) + (b->field_24.end - b->field_24.begin) * sizeof(uint16_t);
    }

    if (b->field_28.end - b->field_28.begin) {
        io_position_push(s, *field_28_offset, SEEK_SET);
        io_write_uint16_t(s, (uint16_t)(b->field_28.end - b->field_28.begin));
        for (uint16_t* i = b->field_28.begin; i != b->field_28.end; i++)
            io_write_uint16_t(s, *i);
        io_position_pop(s);
        *field_28_offset += sizeof(uint16_t) + (b->field_28.end - b->field_28.begin) * sizeof(uint16_t);
    }
}

static void obj_modern_read_skin_block_cloth_field_1C_sub(obj_skin_block_cloth_field_1C_sub* sub,
    stream* s, uint32_t header_length, string* str, bool is_x) {
    ssize_t bone_name_offset = io_read_offset(s, header_length, is_x);
    io_read_string_null_terminated_offset(s, bone_name_offset, &sub->bone_name);
    sub->weight = io_read_float_t_stream_reverse_endianness(s);
    sub->matrix_index = io_read_uint32_t_stream_reverse_endianness(s);
    sub->field_0C = io_read_uint32_t_stream_reverse_endianness(s);
    if (is_x)
        io_read(s, 0, 0x04);
}

static void obj_modern_write_skin_block_cloth_field_1C_sub(obj_skin_block_cloth_field_1C_sub* sub,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets, bool is_x) {
    ssize_t bone_name_offset = obj_skin_strings_get_string_offset(strings,
        string_offsets, string_data(&sub->bone_name));
    io_write_offset(s, bone_name_offset, 0x20, is_x);
    io_write_float_t(s, sub->weight);
    io_write_uint32_t(s, sub->matrix_index);
    io_write_uint32_t(s, sub->field_0C);
    if (is_x)
        io_write(s, 0, 0x04);
}

static void obj_modern_read_skin_block_cloth_field_20_sub(obj_skin_block_cloth_field_20_sub* sub,
    stream* s, uint32_t header_length, string* str, bool is_x) {
    sub->field_00 = io_read_uint32_t_stream_reverse_endianness(s);
    sub->field_04 = io_read_float_t_stream_reverse_endianness(s);
    sub->field_08 = io_read_float_t_stream_reverse_endianness(s);
    sub->field_0C = io_read_float_t_stream_reverse_endianness(s);
    sub->field_10 = io_read_float_t_stream_reverse_endianness(s);
    sub->field_14 = io_read_float_t_stream_reverse_endianness(s);
    sub->field_18 = io_read_float_t_stream_reverse_endianness(s);
    sub->field_1C = io_read_float_t_stream_reverse_endianness(s);
    sub->field_20 = io_read_float_t_stream_reverse_endianness(s);
    sub->field_24 = io_read_float_t_stream_reverse_endianness(s);
    sub->field_28 = io_read_float_t_stream_reverse_endianness(s);
}

static void obj_modern_write_skin_block_cloth_field_20_sub(obj_skin_block_cloth_field_20_sub* sub,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets, bool is_x) {
    io_write_uint32_t(s, sub->field_00);
    io_write_float_t(s, sub->field_04);
    io_write_float_t(s, sub->field_08);
    io_write_float_t(s, sub->field_0C);
    io_write_float_t(s, sub->field_10);
    io_write_float_t(s, sub->field_14);
    io_write_float_t(s, sub->field_18);
    io_write_float_t(s, sub->field_1C);
    io_write_float_t(s, sub->field_20);
    io_write_float_t(s, sub->field_24);
    io_write_float_t(s, sub->field_28);
}

static void obj_modern_read_skin_block_constraint(obj_skin_block_constraint* b,
    stream* s, uint32_t header_length, string* str, bool is_x) {
    obj_modern_read_skin_block_node(&b->base, s, header_length, str, is_x);

    ssize_t type_offset = io_read_offset(s, header_length, is_x);
    char* type = io_read_utf8_string_null_terminated_offset(s, type_offset);

    ssize_t name = io_read_offset(s, header_length, is_x);
    io_read_string_null_terminated_offset(s, name, &b->base.name);

    b->coupling = io_read_int32_t_stream_reverse_endianness(s);

    ssize_t source_node_name = io_read_offset(s, header_length, is_x);
    io_read_string_null_terminated_offset(s, source_node_name, &b->source_node_name);

    if (!strcmp(type, "Direction")) {
        b->type = OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION;
        obj_modern_read_skin_block_constraint_up_vector(&b->direction.up_vector,
            s, header_length, str, is_x);
        b->direction.align_axis.x = io_read_float_t_stream_reverse_endianness(s);
        b->direction.align_axis.y = io_read_float_t_stream_reverse_endianness(s);
        b->direction.align_axis.z = io_read_float_t_stream_reverse_endianness(s);
        b->direction.target_offset.x = io_read_float_t_stream_reverse_endianness(s);
        b->direction.target_offset.y = io_read_float_t_stream_reverse_endianness(s);
        b->direction.target_offset.z = io_read_float_t_stream_reverse_endianness(s);
    }
    else if (!strcmp(type, "Distance")) {
        b->type = OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE;
        obj_modern_read_skin_block_constraint_up_vector(&b->distance.up_vector,
            s, header_length, str, is_x);
        b->distance.distance = io_read_float_t_stream_reverse_endianness(s);
        obj_modern_read_skin_block_constraint_attach_point(&b->distance.constrained_object,
            s, header_length, str, is_x);
        obj_modern_read_skin_block_constraint_attach_point(&b->distance.constraining_object,
            s, header_length, str, is_x);
    }
    else if (!strcmp(type, "Orientation")) {
        b->type = OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION;
        b->orientation.offset.x = io_read_float_t_stream_reverse_endianness(s);
        b->orientation.offset.y = io_read_float_t_stream_reverse_endianness(s);
        b->orientation.offset.z = io_read_float_t_stream_reverse_endianness(s);
    }
    else if (!strcmp(type, "Position")) {
        b->type = OBJ_SKIN_BLOCK_CONSTRAINT_POSITION;
        obj_modern_read_skin_block_constraint_up_vector(&b->position.up_vector,
            s, header_length, str, is_x);
        obj_modern_read_skin_block_constraint_attach_point(&b->position.constrained_object,
            s, header_length, str, is_x);
        obj_modern_read_skin_block_constraint_attach_point(&b->position.constraining_object,
            s, header_length, str, is_x);
    }
    else
        b->type = OBJ_SKIN_BLOCK_CONSTRAINT_NONE;
    free(type);
}

static void obj_modern_write_skin_block_constraint(obj_skin_block_constraint* b,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets, bool is_x, ssize_t* offsets) {
    obj_modern_write_skin_block_node(&b->base, s, strings, string_offsets, is_x);

    ssize_t type_offset = 0;
    switch (b->type) {
    case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
        type_offset = offsets[0];
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
        type_offset = offsets[1];
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
        type_offset = offsets[2];
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION:
        type_offset = offsets[3];
        break;
    }
    io_write_offset(s, type_offset, 0x20, is_x);

    ssize_t name = obj_skin_strings_get_string_offset(strings,
        string_offsets, string_data(&b->base.name));
    io_write_offset(s, name, 0x20, is_x);

    io_write_int32_t(s, b->coupling);

    ssize_t source_node_name = obj_skin_strings_get_string_offset(strings,
        string_offsets, string_data(&b->source_node_name));
    io_write_offset(s, source_node_name, 0x20, is_x);

    switch (b->type) {
    case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION:
        obj_modern_write_skin_block_constraint_up_vector(&b->direction.up_vector,
            s, strings, string_offsets, is_x);
        io_write_float_t(s, b->direction.align_axis.x);
        io_write_float_t(s, b->direction.align_axis.y);
        io_write_float_t(s, b->direction.align_axis.z);
        io_write_float_t(s, b->direction.target_offset.x);
        io_write_float_t(s, b->direction.target_offset.y);
        io_write_float_t(s, b->direction.target_offset.z);
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
        obj_modern_write_skin_block_constraint_up_vector(&b->distance.up_vector,
            s, strings, string_offsets, is_x);
        io_write_float_t(s, b->distance.distance);
        obj_modern_write_skin_block_constraint_attach_point(&b->distance.constrained_object,
            s, strings, string_offsets, is_x);
        obj_modern_write_skin_block_constraint_attach_point(&b->distance.constraining_object,
            s, strings, string_offsets, is_x);
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION:
        io_write_float_t(s, b->orientation.offset.x);
        io_write_float_t(s, b->orientation.offset.y);
        io_write_float_t(s, b->orientation.offset.z);
        break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION:
        obj_modern_write_skin_block_constraint_up_vector(&b->position.up_vector,
            s, strings, string_offsets, is_x);
        obj_modern_write_skin_block_constraint_attach_point(&b->position.constrained_object,
            s, strings, string_offsets, is_x);
        obj_modern_write_skin_block_constraint_attach_point(&b->position.constraining_object,
            s, strings, string_offsets, is_x);
        break;
    }
}

static void obj_modern_read_skin_block_constraint_attach_point(obj_skin_block_constraint_attach_point* ap,
    stream* s, uint32_t header_length, string* str, bool is_x) {
    ap->affected_by_orientation = io_read_int32_t_stream_reverse_endianness(s) != 0;
    ap->affected_by_scaling = io_read_int32_t_stream_reverse_endianness(s) != 0;
    ap->offset.x = io_read_float_t_stream_reverse_endianness(s);
    ap->offset.y = io_read_float_t_stream_reverse_endianness(s);
    ap->offset.z = io_read_float_t_stream_reverse_endianness(s);
}

static void obj_modern_write_skin_block_constraint_attach_point(obj_skin_block_constraint_attach_point* ap,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets, bool is_x) {
    io_write_int32_t(s, ap->affected_by_orientation ? 1 : 0);
    io_write_int32_t(s, ap->affected_by_scaling ? 1 : 0);
    io_write_float_t(s, ap->offset.x);
    io_write_float_t(s, ap->offset.y);
    io_write_float_t(s, ap->offset.z);
}

static void obj_modern_read_skin_block_constraint_up_vector(obj_skin_block_constraint_up_vector* up,
    stream* s, uint32_t header_length, string* str, bool is_x) {
    up->active = io_read_int32_t_stream_reverse_endianness(s) != 0;
    up->roll = io_read_float_t_stream_reverse_endianness(s);
    up->affected_axis.x = io_read_float_t_stream_reverse_endianness(s);
    up->affected_axis.y = io_read_float_t_stream_reverse_endianness(s);
    up->affected_axis.z = io_read_float_t_stream_reverse_endianness(s);
    up->point_at.x = io_read_float_t_stream_reverse_endianness(s);
    up->point_at.y = io_read_float_t_stream_reverse_endianness(s);
    up->point_at.z = io_read_float_t_stream_reverse_endianness(s);

    ssize_t name = io_read_offset(s, header_length, is_x);
    io_read_string_null_terminated_offset(s, name, &up->name);
}

static void obj_modern_write_skin_block_constraint_up_vector(obj_skin_block_constraint_up_vector* up,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets, bool is_x) {
    io_write_int32_t(s, up->active ? 1 : 0);
    io_write_float_t(s, up->roll);
    io_write_float_t(s, up->affected_axis.x);
    io_write_float_t(s, up->affected_axis.y);
    io_write_float_t(s, up->affected_axis.z);
    io_write_float_t(s, up->point_at.x);
    io_write_float_t(s, up->point_at.y);
    io_write_float_t(s, up->point_at.z);

    ssize_t name = obj_skin_strings_get_string_offset(strings,
        string_offsets, string_data(&up->name));
    io_write_offset(s, name, 0x20, is_x);
}

static void obj_modern_read_skin_block_expression(obj_skin_block_expression* b,
    stream* s, uint32_t header_length, string* str, bool is_x) {
    obj_modern_read_skin_block_node(&b->base, s, header_length, str, is_x);

    ssize_t name = io_read_offset(s, header_length, is_x);
    io_read_string_null_terminated_offset(s, name, &b->base.name);

    b->expression_count = io_read_int32_t_stream_reverse_endianness(s);
    b->expression_count = min(b->expression_count, 10);
    if (!is_x)
        for (int32_t i = 0; i < b->expression_count; i++) {
            ssize_t expression = io_read_offset_f2(s, header_length);
            if (expression)
                io_read_string_null_terminated_offset(s, expression, &b->expressions[i]);
        }
    else
        for (int32_t i = 0; i < b->expression_count; i++) {
            ssize_t expression = io_read_offset_x(s);
            if (expression)
                io_read_string_null_terminated_offset(s, expression, &b->expressions[i]);
        }

    for (int32_t i = b->expression_count; i < 9; i++)
        memset(&b->expressions[i], 0, sizeof(string));
}

static void obj_modern_write_skin_block_expression(obj_skin_block_expression* b,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets, bool is_x) {
    obj_modern_write_skin_block_node(&b->base, s, strings, string_offsets, is_x);

    ssize_t name = obj_skin_strings_get_string_offset(strings,
        string_offsets, string_data(&b->base.name));
    io_write_offset(s, name, 0x20, is_x);

    io_write_int32_t(s, min(b->expression_count, 9));
    if (!is_x) {
        for (int32_t i = 0; i < b->expression_count && i < 9; i++) {
            ssize_t expression = obj_skin_strings_get_string_offset(strings,
                string_offsets, string_data(&b->expressions[i]));
            io_write_offset_f2(s, (int32_t)expression, 0x20);
        }

        for (int32_t i = b->expression_count; i < 9; i++)
            io_write_offset_f2(s, 0, 0x20);
    }
    else {
        for (int32_t i = 0; i < b->expression_count && i < 9; i++) {
            ssize_t expression = obj_skin_strings_get_string_offset(strings,
                string_offsets, string_data(&b->expressions[i]));
            io_write_offset_x(s, expression);
        }

        for (int32_t i = b->expression_count; i < 9; i++)
            io_write_offset_x(s, 0);
    }
}

static void obj_modern_read_skin_block_motion(obj_skin_block_motion* b,
    stream* s, uint32_t header_length, string* str, bool is_x) {
    obj_modern_read_skin_block_node(&b->base, s, header_length, str, is_x);

    ssize_t name_offset = io_read_offset(s, header_length, is_x);
    int32_t count = io_read_int32_t_stream_reverse_endianness(s);
    ssize_t bone_names_offset = io_read_offset(s, header_length, is_x);
    ssize_t bone_matrices_offset = io_read_offset(s, header_length, is_x);

    io_read_string_null_terminated_offset(s, name_offset, &b->base.name);


    b->nodes = vector_empty(obj_skin_motion_node);
    vector_obj_skin_motion_node_reserve(&b->nodes, count);
    if (bone_names_offset) {
        obj_skin_motion_node node;
        memset(&node, 0, sizeof(obj_skin_motion_node));
        io_position_push(s, bone_names_offset, SEEK_SET);
        for (int32_t i = 0; i < count; i++) {
            uint32_t name = io_read_uint32_t_stream_reverse_endianness(s);
            if (name & 0x8000)
                string_copy(&str[name & 0x7FFF], &node.name);
            else
                memset(&node.name, 0, sizeof(string));
            vector_obj_skin_motion_node_push_back(&b->nodes, &node);
        }
        io_position_pop(s);
    }

    if (bone_matrices_offset) {
        io_position_push(s, bone_matrices_offset, SEEK_SET);
        for (int32_t i = 0; i < count; i++) {
            mat4 mat;
            mat.row0.x = io_read_float_t_stream_reverse_endianness(s);
            mat.row0.y = io_read_float_t_stream_reverse_endianness(s);
            mat.row0.z = io_read_float_t_stream_reverse_endianness(s);
            mat.row0.w = io_read_float_t_stream_reverse_endianness(s);
            mat.row1.x = io_read_float_t_stream_reverse_endianness(s);
            mat.row1.y = io_read_float_t_stream_reverse_endianness(s);
            mat.row1.z = io_read_float_t_stream_reverse_endianness(s);
            mat.row1.w = io_read_float_t_stream_reverse_endianness(s);
            mat.row2.x = io_read_float_t_stream_reverse_endianness(s);
            mat.row2.y = io_read_float_t_stream_reverse_endianness(s);
            mat.row2.z = io_read_float_t_stream_reverse_endianness(s);
            mat.row2.w = io_read_float_t_stream_reverse_endianness(s);
            mat.row3.x = io_read_float_t_stream_reverse_endianness(s);
            mat.row3.y = io_read_float_t_stream_reverse_endianness(s);
            mat.row3.z = io_read_float_t_stream_reverse_endianness(s);
            mat.row3.w = io_read_float_t_stream_reverse_endianness(s);
            mat4_transpose(&mat, &b->nodes.begin[i].transformation);
        }
        io_position_pop(s);
    }
}

static void obj_modern_write_skin_block_motion(obj_skin_block_motion* b,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets, bool is_x,
    vector_string* string_set, ssize_t* bone_names_offset, ssize_t* bone_matrices_offset) {
    obj_modern_write_skin_block_node(&b->base, s, strings, string_offsets, is_x);

    ssize_t name = obj_skin_strings_get_string_offset(strings,
        string_offsets, string_data(&b->base.name));
    io_write_offset(s, name, 0x20, is_x);
    io_write_int32_t(s, (int32_t)(b->nodes.end - b->nodes.begin));
    io_write_offset(s, b->nodes.end - b->nodes.begin ? *bone_names_offset : 0, 0x20, is_x);
    io_write_offset(s, b->nodes.end - b->nodes.begin ? *bone_matrices_offset : 0, 0x20, is_x);

    obj_skin_motion_node node;
    memset(&node, 0, sizeof(obj_skin_motion_node));

    if (b->nodes.end - b->nodes.begin) {
        io_position_push(s, *bone_names_offset, SEEK_SET);
        for (obj_skin_motion_node* i = b->nodes.begin; i != b->nodes.end; i++)
            io_write_uint32_t(s, obj_skin_strings_get_string_index(string_set, string_data(&i->name)));
        io_position_pop(s);
        *bone_names_offset += (b->nodes.end - b->nodes.begin) * sizeof(uint32_t);

        io_position_push(s, *bone_matrices_offset, SEEK_SET);
        for (obj_skin_motion_node* i = b->nodes.begin; i != b->nodes.end; i++) {
            mat4 mat;
            mat4_transpose(&i->transformation, &mat);
            io_write_float_t(s, mat.row0.x);
            io_write_float_t(s, mat.row0.y);
            io_write_float_t(s, mat.row0.z);
            io_write_float_t(s, mat.row0.w);
            io_write_float_t(s, mat.row1.x);
            io_write_float_t(s, mat.row1.y);
            io_write_float_t(s, mat.row1.z);
            io_write_float_t(s, mat.row1.w);
            io_write_float_t(s, mat.row2.x);
            io_write_float_t(s, mat.row2.y);
            io_write_float_t(s, mat.row2.z);
            io_write_float_t(s, mat.row2.w);
            io_write_float_t(s, mat.row3.x);
            io_write_float_t(s, mat.row3.y);
            io_write_float_t(s, mat.row3.z);
            io_write_float_t(s, mat.row3.w);
        }
        io_position_pop(s);
        *bone_matrices_offset += (b->nodes.end - b->nodes.begin) * sizeof(mat4);
    }
}

static void obj_modern_read_skin_block_node(obj_skin_block_node* b,
    stream* s, uint32_t header_length, string* str, bool is_x) {
    ssize_t parent_name = io_read_offset(s, header_length, is_x);
    io_read_string_null_terminated_offset(s, parent_name, &b->parent_name);

    b->position.x = io_read_float_t_stream_reverse_endianness(s);
    b->position.y = io_read_float_t_stream_reverse_endianness(s);
    b->position.z = io_read_float_t_stream_reverse_endianness(s);
    b->rotation.x = io_read_float_t_stream_reverse_endianness(s);
    b->rotation.y = io_read_float_t_stream_reverse_endianness(s);
    b->rotation.z = io_read_float_t_stream_reverse_endianness(s);
    b->scale.x = io_read_float_t_stream_reverse_endianness(s);
    b->scale.y = io_read_float_t_stream_reverse_endianness(s);
    b->scale.z = io_read_float_t_stream_reverse_endianness(s);
    if (is_x)
        io_read(s, 0, 0x04);
}

static void obj_modern_write_skin_block_node(obj_skin_block_node* b,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets, bool is_x) {
    ssize_t parent_name = obj_skin_strings_get_string_offset(strings,
        string_offsets, string_data(&b->parent_name));
    io_write_offset(s, parent_name, 0x20, is_x);

    io_write_float_t(s, b->position.x);
    io_write_float_t(s, b->position.y);
    io_write_float_t(s, b->position.z);
    io_write_float_t(s, b->rotation.x);
    io_write_float_t(s, b->rotation.y);
    io_write_float_t(s, b->rotation.z);
    io_write_float_t(s, b->scale.x);
    io_write_float_t(s, b->scale.y);
    io_write_float_t(s, b->scale.z);
    if (is_x)
        io_write(s, 0, 0x04);
}

static void obj_modern_read_skin_block_osage(obj_skin_block_osage* b,
    stream* s, uint32_t header_length, string* str, bool is_x) {
    obj_modern_read_skin_block_node(&b->base, s, header_length, str, is_x);

    b->start_index = io_read_int32_t_stream_reverse_endianness(s);
    b->count = io_read_int32_t_stream_reverse_endianness(s);
    uint32_t external_name = io_read_uint32_t_stream_reverse_endianness(s);
    if (external_name & 0x8000)
        string_copy(&str[external_name & 0x7FFF], &b->external_name);
    else
        memset(&b->external_name, 0, sizeof(string));

    uint32_t name = io_read_uint32_t_stream_reverse_endianness(s);
    if (name & 0x8000)
        string_copy(&str[name & 0x7FFF], &b->base.name);
    else
        memset(&b->base.name, 0, sizeof(string));

    if (!is_x)
        io_read(s, 0, 0x14);
    else {
        io_read(s, 0, 0x08);
        ssize_t motion_node_name = io_read_offset_x(s);
        io_read_string_null_terminated_offset(s, motion_node_name, &b->motion_node_name);
        io_read(s, 0, 0x18);
    }
    b->nodes = vector_empty(obj_skin_osage_node);
}

static void obj_modern_write_skin_block_osage(obj_skin_block_osage* b,
    stream* s, vector_string* strings, vector_ssize_t* string_offsets, bool is_x,
    vector_string* string_set) {
    obj_modern_write_skin_block_node(&b->base, s, strings, string_offsets, is_x);

    io_write_int32_t(s, b->start_index);
    io_write_int32_t(s, b->count);
    io_write_uint32_t(s, obj_skin_strings_get_string_index(string_set, string_data(&b->external_name)));
    io_write_uint32_t(s, obj_skin_strings_get_string_index(string_set, string_data(&b->base.name)));

    if (!is_x)
        io_write(s, 0, 0x14);
    else {
        io_write(s, 0, 0x08);
        ssize_t motion_node_name = obj_skin_strings_get_string_offset(strings,
            string_offsets, string_data(&b->motion_node_name));
        io_write_offset_x(s, motion_node_name);
        io_write(s, 0, 0x18); 
    }
}

static void obj_modern_read_vertex(obj* o, stream* s, ssize_t* attrib_offsets, obj_mesh* mesh,
    const uint32_t attrib_flags, int32_t vertex_count, int32_t vertex_size) {
    obj_vertex_flags vertex_flags = OBJ_VERTEX_POSITION
        | OBJ_VERTEX_NORMAL
        | OBJ_VERTEX_TANGENT
        | OBJ_VERTEX_TEXCOORD0
        | OBJ_VERTEX_TEXCOORD1
        | OBJ_VERTEX_COLOR0;

    if (attrib_flags & 0x08)
        vertex_flags |= OBJ_VERTEX_TEXCOORD2 | OBJ_VERTEX_TEXCOORD3;

    if (attrib_flags & 0x04)
        vertex_flags |= OBJ_VERTEX_BONE_DATA;

    ssize_t vtx_offset = attrib_offsets[13];

    bool has_tangents = false;
    obj_vertex_data* vtx = force_malloc_s(obj_vertex_data, vertex_count);
    for (int32_t i = 0; i < vertex_count; i++) {
        io_set_position(s, vtx_offset + (ssize_t)vertex_size * i, SEEK_SET);

        vec3 position;
        position.x = io_read_float_t_stream_reverse_endianness(s);
        position.y = io_read_float_t_stream_reverse_endianness(s);
        position.z = io_read_float_t_stream_reverse_endianness(s);
        vtx[i].position = position;

        vec3 normal;
        normal.x = (float_t)io_read_int16_t_stream_reverse_endianness(s);
        normal.y = (float_t)io_read_int16_t_stream_reverse_endianness(s);
        normal.z = (float_t)io_read_int16_t_stream_reverse_endianness(s);
        io_read(s, 0, 0x02);
        vec3_div_min_max_scalar(normal, -32768.0f, 32767.0f, vtx[i].normal);

        vec4 tangent;
        tangent.x = (float_t)io_read_int16_t_stream_reverse_endianness(s);
        tangent.y = (float_t)io_read_int16_t_stream_reverse_endianness(s);
        tangent.z = (float_t)io_read_int16_t_stream_reverse_endianness(s);
        tangent.w = (float_t)io_read_int16_t_stream_reverse_endianness(s);
        vec4_div_min_max_scalar(tangent, -32768.0f, 32767.0f, vtx[i].tangent);

        vec2h texcoord0;
        texcoord0.x = io_read_half_t_stream_reverse_endianness(s);
        texcoord0.y = io_read_half_t_stream_reverse_endianness(s);
        vec2h_to_vec2(texcoord0, vtx[i].texcoord0);

        vec2h texcoord1;
        texcoord1.x = io_read_half_t_stream_reverse_endianness(s);
        texcoord1.y = io_read_half_t_stream_reverse_endianness(s);
        vec2h_to_vec2(texcoord1, vtx[i].texcoord1);

        if (attrib_flags & 0x08) {
            vec2h texcoord2;
            texcoord2.x = io_read_half_t_stream_reverse_endianness(s);
            texcoord2.y = io_read_half_t_stream_reverse_endianness(s);
            vec2h_to_vec2(texcoord2, vtx[i].texcoord2);

            vec2h texcoord3;
            texcoord3.x = io_read_half_t_stream_reverse_endianness(s);
            texcoord3.y = io_read_half_t_stream_reverse_endianness(s);
            vec2h_to_vec2(texcoord3, vtx[i].texcoord3);
        }

        vec4h color;
        color.x = io_read_half_t_stream_reverse_endianness(s);
        color.y = io_read_half_t_stream_reverse_endianness(s);
        color.z = io_read_half_t_stream_reverse_endianness(s);
        color.w = io_read_half_t_stream_reverse_endianness(s);
        vec4h_to_vec4(color, vtx[i].color0);

        if (attrib_flags & 0x04) {
            vec4 bone_weight;
            bone_weight.x = (float_t)io_read_int16_t_stream_reverse_endianness(s);
            bone_weight.y = (float_t)io_read_int16_t_stream_reverse_endianness(s);
            bone_weight.z = (float_t)io_read_int16_t_stream_reverse_endianness(s);
            bone_weight.w = (float_t)io_read_int16_t_stream_reverse_endianness(s);
            vec4_div_min_max_scalar(bone_weight, -32768.0f, 32767.0f, vtx[i].bone_weight);

            vec4i bone_index;
            bone_index.x = io_read_uint8_t(s);
            bone_index.y = io_read_uint8_t(s);
            bone_index.z = io_read_uint8_t(s);
            bone_index.w = io_read_uint8_t(s);
            if (bone_index.x == 0xFF)
                bone_index.x = -1;
            if (bone_index.y == 0xFF)
                bone_index.y = -1;
            if (bone_index.z == 0xFF)
                bone_index.z = -1;
            if (bone_index.w == 0xFF)
                bone_index.w = -1;
            vtx[i].bone_index = bone_index;
        }

        if (memcmp(&vtx[i].tangent, &vec4_null, sizeof(vec4)))
            has_tangents = true;
    }

    if (!has_tangents)
        vertex_flags &= ~OBJ_VERTEX_TANGENT;

    mesh->vertex = vtx;
    mesh->vertex_count = vertex_count;
    mesh->vertex_flags = vertex_flags;
}

static void obj_modern_write_vertex(obj* o, stream* s, bool is_x,
    ssize_t* attrib_offsets, obj_mesh* mesh, uint32_t* attrib_flags,
    int32_t* vertex_count, int32_t* vertex_size, f2_struct* ovtx) {

    obj_vertex_data* vtx = mesh->vertex;
    int32_t _vertex_count = mesh->vertex_count;
    obj_vertex_flags vertex_flags = mesh->vertex_flags;

    uint32_t _attrib_flags = 0;
    int32_t _vertex_size = 0x2C;
    int32_t enrs_se3_rc = 12;
    if (vertex_flags & OBJ_VERTEX_TEXCOORD2 | vertex_flags & OBJ_VERTEX_TEXCOORD3) {
        _attrib_flags |= 0x08;
        _vertex_size += 0x08;
        enrs_se3_rc += 4;
    }

    if (vertex_flags & OBJ_VERTEX_BONE_DATA) {
        _attrib_flags |= 0x04;
        _vertex_size += 0x0C;
        enrs_se3_rc += 4;
    }
    else
        _attrib_flags |= 0x02;

    uint32_t off = 0;
    vector_enrs_entry* e = &ovtx->enrs;
    enrs_entry ee;
    bool add_enrs = true;
    if (e->end - e->begin > 0) {
        off = (uint32_t)((size_t)e->end[-1].size * e->end[-1].repeat_count);
        if (e->end[-1].sub.begin[2].repeat_count == enrs_se3_rc) {
            e->end[-1].repeat_count += _vertex_count;
            add_enrs = false;
        }
    }

    if (add_enrs) {
        ee = (enrs_entry){ off, 3, _vertex_size, _vertex_count, vector_empty(enrs_sub_entry) };
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 3, ENRS_DWORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 0, 3, ENRS_WORD });
        vector_enrs_sub_entry_push_back(&ee.sub, &(enrs_sub_entry){ 2, enrs_se3_rc, ENRS_WORD });
        vector_enrs_entry_push_back(e, &ee);
    }

    bool has_tangents = false;
    attrib_offsets[13] = io_get_position(s);
    for (int32_t i = 0; i < _vertex_count; i++) {
        vec3 position = vtx[i].position;
        io_write_float_t(s, position.x);
        io_write_float_t(s, position.y);
        io_write_float_t(s, position.z);

        vec3 normal;
        vec3_mult_min_max_scalar(vtx[i].normal, -32768.0f, 32767.0f, normal);
        io_write_int16_t(s, (int16_t)roundf(normal.x));
        io_write_int16_t(s, (int16_t)roundf(normal.y));
        io_write_int16_t(s, (int16_t)roundf(normal.z));
        io_write(s, 0, 0x02);

        vec4 tangent;
        vec4_mult_min_max_scalar(vtx[i].tangent, -32768.0f, 32767.0f, tangent);
        io_write_int16_t(s, (int16_t)roundf(tangent.x));
        io_write_int16_t(s, (int16_t)roundf(tangent.y));
        io_write_int16_t(s, (int16_t)roundf(tangent.z));
        io_write_int16_t(s, (int16_t)roundf(tangent.w));

        vec2h texcoord0;
        vec2_to_vec2h(vtx[i].texcoord0, texcoord0);
        io_write_half_t(s, texcoord0.x);
        io_write_half_t(s, texcoord0.y);

        vec2h texcoord1;
        vec2_to_vec2h(vtx[i].texcoord1, texcoord1);
        io_write_half_t(s, texcoord1.x);
        io_write_half_t(s, texcoord1.y);

        if (_attrib_flags & 0x08) {
            vec2h texcoord2;
            vec2_to_vec2h(vtx[i].texcoord2, texcoord2);
            io_write_half_t(s, texcoord2.x);
            io_write_half_t(s, texcoord2.y);

            vec2h texcoord3;
            vec2_to_vec2h(vtx[i].texcoord3, texcoord3);
            io_write_half_t(s, texcoord3.x);
            io_write_half_t(s, texcoord3.y);
        }

        vec4h color;
        vec4_to_vec4h(vtx[i].color0, color);
        io_write_half_t(s, color.x);
        io_write_half_t(s, color.y);
        io_write_half_t(s, color.z);
        io_write_half_t(s, color.w);

        if (_attrib_flags & 0x04) {
            vec4 bone_weight;
            vec4_mult_min_max_scalar(vtx[i].bone_weight, -32768.0f, 32767.0f, bone_weight);
            io_write_int16_t(s, (int16_t)bone_weight.x);
            io_write_int16_t(s, (int16_t)bone_weight.y);
            io_write_int16_t(s, (int16_t)bone_weight.z);
            io_write_int16_t(s, (int16_t)bone_weight.w);

            vec4u8 bone_index;
            vec4_to_vec4u8(vtx[i].bone_index, bone_index)
            io_write_uint8_t(s, bone_index.x);
            io_write_uint8_t(s, bone_index.y);
            io_write_uint8_t(s, bone_index.z);
            io_write_uint8_t(s, bone_index.w);
        }
    }

    *attrib_flags = _attrib_flags;
    *vertex_count = _vertex_count;
    *vertex_size = _vertex_size;
}

inline static uint32_t obj_skin_strings_get_string_index(vector_string* vec, char* str) {
    if (!str)
        return 0;

    ssize_t str_index = 0;
    for (string* i = vec->begin; i != vec->end; i++)
        if (!strcmp(str, string_data(i)))
            str_index = i - vec->begin;
    return (uint32_t)(0x8000 | str_index);
}

inline static ssize_t obj_skin_strings_get_string_offset(vector_string* vec,
    vector_ssize_t* vec_off, char* str) {
    if (!str)
        return 0;

    ssize_t str_offset = 0;
    for (string* i = vec->begin; i != vec->end; i++)
        if (!strcmp(str, string_data(i)))
            str_offset = vec_off->begin[i - vec->begin];
    return str_offset;
}

inline static void obj_skin_strings_push_back_check(vector_string* vec, char* str) {
    if (!str)
        return;

    bool found = false;
    for (string* k = vec->begin; k != vec->end; k++)
        if (!strcmp(string_data(k), str)) {
            found = true;
            break;
        }

    if (!found) {
        string _s;
        string_init(&_s, str);
        vector_string_push_back(vec, &_s);
    }
}
