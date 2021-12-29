/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "a3da.h"
#include "f2/struct.h"
#include "io/path.h"
#include "io/stream.h"
#include "half_t.h"
#include "hash.h"
#include "key_val.h"
#include "str_utils.h"
#include <time.h>

typedef struct a3da_vec3_header {
    uint32_t x;
    uint32_t y;
    uint32_t z;
} a3da_vec3_header;

typedef struct a3da_model_transform_header {
    a3da_vec3_header scale;
    a3da_vec3_header rotation;
    a3da_vec3_header translation;
    uint32_t visibility;
} a3da_model_transform_header;

typedef struct a3dc_header {
    uint32_t binary_length;
    uint32_t binary_offset;
    uint32_t string_length;
    uint32_t string_offset;
} a3dc_header;

typedef struct a3dc_key_header {
    a3da_key_type type : 8;
    a3da_ep_type ep_type_pre : 4;
    a3da_ep_type ep_type_post : 4;
    uint32_t padding : 16;
    float_t value;
    float_t max;
    uint32_t length;
} a3dc_key_header;

vector_func(a3da_ambient)
vector_func(a3da_model_transform)
vector_func(a3da_camera_root)
vector_func(a3da_chara)
vector_func(a3da_curve)
vector_func(a3da_event)
vector_func(a3da_fog)
vector_func(a3da_light)
vector_func(a3da_m_object_hrc)
vector_func(a3da_material_list)
vector_func(a3da_object)
vector_func(a3da_object_hrc)
vector_func(a3da_object_instance)
vector_func(a3da_object_node)
vector_func(a3da_object_texture_pattern)
vector_func(a3da_object_texture_transform)
vector_func(a3da_point)

#define A3DA_TEXT_BUF_SIZE 0x400

static void a3da_read_inner(a3da* a, stream* s);
static void a3da_write_inner(a3da* a, stream* s);
static void a3da_read_text(a3da* a, void* data, size_t length);
static void a3da_write_text(a3da* a, void** data, size_t* length, bool a3dc);
static void a3da_read_data(a3da* a, void* data, size_t length);
static void a3da_write_data(a3da* a, void** data, size_t* length);
static void a3da_get_time_stamp(char* buf, size_t buf_size);
static bool key_val_read_a3da_key(key_val* kv, char* buf,
    size_t offset, char* str_add, size_t str_add_len, a3da_key* value);
static void key_val_write_a3da_key(stream* s, char* buf,
    size_t offset, char* str_add, size_t str_add_len, a3da_key* value);
static bool key_val_read_a3da_key_raw_data(key_val* kv, char* buf,
    size_t offset, a3da_key* value);
static void key_val_write_a3da_key_raw_data(stream* s, char* buf,
    size_t offset, a3da_key* value);
static bool key_val_read_a3da_model_transform(key_val* kv, char* buf,
    size_t offset, char* str_add, size_t str_add_len, a3da_model_transform* value);
static void key_val_write_a3da_model_transform(stream* s, char* buf,
    size_t offset, char* str_add, size_t str_add_len, a3da_model_transform* value, int32_t write_mask);
static bool key_val_read_a3da_rgba(key_val* kv, char* buf,
    size_t offset, char* str_add, size_t str_add_len, a3da_rgba* value);
static void key_val_write_a3da_rgba(stream* s, char* buf,
    size_t offset, char* str_add, size_t str_add_len, a3da_rgba* value);
static bool key_val_read_a3da_vec3(key_val* kv, char* buf,
    size_t offset, char* str_add, size_t str_add_len, a3da_vec3* value);
static void key_val_write_a3da_vec3(stream* s, char* buf,
    size_t offset, char* str_add, size_t str_add_len, a3da_vec3* value);
static void a3dc_read_a3da_key(void* data, size_t length, a3da_key* value);
static void a3dc_write_a3da_key(stream* s, a3da_key* value);
static void a3dc_read_a3da_key_f16(void* data, size_t length, a3da_key* value, a3da_compress_f16 f16);
static void a3dc_write_a3da_key_f16(stream* s, a3da_key* value, a3da_compress_f16 f16);
static void a3dc_read_a3da_model_transform(void* data, size_t length,
    a3da_model_transform* value, a3da_compress_f16 f16);
static void a3dc_write_a3da_model_transform(stream* s,
    a3da_model_transform* value, a3da_compress_f16 f16);
static void a3dc_write_a3da_model_transform_offset(stream* s,
    a3da_model_transform* value);
static void a3dc_write_a3da_model_transform_offset_data(stream* s,
    a3da_model_transform* value);
static void a3dc_read_a3da_rgba(void* data, size_t length, a3da_rgba* value);
static void a3dc_write_a3da_rgba(stream* s, a3da_rgba* value);
static void a3dc_read_a3da_vec3(void* data, size_t length, a3da_vec3* value);
static void a3dc_write_a3da_vec3(stream* s, a3da_vec3* value);
static void a3dc_read_a3da_vec3_f16(void* data, size_t length, a3da_vec3* value, a3da_compress_f16 f16);
static void a3dc_write_a3da_vec3_f16(stream* s, a3da_vec3* value, a3da_compress_f16 f16);

void a3da_init(a3da* a) {
    memset(a, 0, sizeof(a3da));
}

void a3da_read(a3da* a, char* path) {
    if (!a || !path)
        return;

    char* path_a3da = str_utils_add(path, ".a3da");
    if (path_check_file_exists(path_a3da)) {
        stream s;
        io_open(&s, path_a3da, "rb");
        if (s.io.stream)
            a3da_read_inner(a, &s);
        io_free(&s);
    }
    free(path_a3da);
}

void a3da_wread(a3da* a, wchar_t* path) {
    if (!a || !path)
        return;

    wchar_t* path_a3da = str_utils_wadd(path, L".a3da");
    if (path_wcheck_file_exists(path_a3da)) {
        stream s;
        io_wopen(&s, path_a3da, L"rb");
        if (s.io.stream)
            a3da_read_inner(a, &s);
        io_free(&s);
    }
    free(path_a3da);
}

void a3da_mread(a3da* a, void* data, size_t length) {
    if (!a || !data || !length)
        return;

    stream s;
    io_mopen(&s, data, length);
    a3da_read_inner(a, &s);
    io_free(&s);
}

void a3da_write(a3da* a, char* path) {
    if (!a || !path || !a->ready)
        return;

    char* path_a3da = str_utils_add(path, ".a3da");
    stream s;
    io_open(&s, path_a3da, "wb");
    if (s.io.stream)
        a3da_write_inner(a, &s);
    io_free(&s);
    free(path_a3da);
}

void a3da_wwrite(a3da* a, wchar_t* path) {
    if (!a || !path || !a->ready)
        return;

    wchar_t* path_a3da = str_utils_wadd(path, L".a3da");
    stream s;
    io_wopen(&s, path_a3da, L"wb");
    if (s.io.stream)
        a3da_write_inner(a, &s);
    io_free(&s);
    free(path_a3da);
}

void a3da_mwrite(a3da* a, void** data, size_t* length) {
    if (!a || !data || !a->ready)
        return;

    stream s;
    io_mopen(&s, 0, 0);
    a3da_write_inner(a, &s);
    io_mcopy(&s, data, length);
    io_free(&s);
}

bool a3da_load_file(void* data, char* path, char* file, uint32_t hash) {
    size_t file_len = utf8_length(file);

    char* t = strrchr(file, '.');
    if (t)
        file_len = t - file;

    string s;
    string_init(&s, path);
    string_add_length(&s, file, file_len);

    a3da* a = data;
    a3da_read(a, string_data(&s));

    string_free(&s);
    return a->ready;
}

void a3da_free(a3da* a) {
    if (!a)
        return;

    string_free(&a->_file_name);
    string_free(&a->_property_version);
    string_free(&a->_converter_version);

    vector_a3da_ambient_free(&a->ambient, a3da_ambient_free);
    vector_string_free(&a->auth_2d, string_free);
    a3da_camera_auxiliary_free(&a->camera_auxiliary);
    vector_a3da_camera_root_free(&a->camera_root, a3da_camera_root_free);
    vector_a3da_chara_free(&a->chara, a3da_chara_free);
    vector_a3da_curve_free(&a->curve, a3da_curve_free);
    a3da_dof_free(&a->dof);
    vector_a3da_event_free(&a->event, a3da_event_free);
    vector_a3da_fog_free(&a->fog, a3da_fog_free);
    vector_a3da_light_free(&a->light, a3da_light_free);
    vector_a3da_m_object_hrc_free(&a->m_object_hrc, a3da_m_object_hrc_free);
    vector_string_free(&a->m_object_hrc_list, string_free);
    vector_a3da_material_list_free(&a->material_list, a3da_material_list_free);
    vector_string_free(&a->motion, string_free);
    vector_a3da_object_free(&a->object, a3da_object_free);
    vector_a3da_object_hrc_free(&a->object_hrc, a3da_object_hrc_free);
    vector_string_free(&a->object_hrc_list, string_free);
    vector_string_free(&a->object_list, string_free);
    vector_a3da_point_free(&a->point, a3da_point_free);
    a3da_post_process_free(&a->post_process);
}

void a3da_key_free(a3da_key* k) {
    vector_kft3_free(&k->keys, 0);
}

void a3da_rgba_free(a3da_rgba* rgba) {
    if (rgba->flags & A3DA_RGBA_R)
        a3da_key_free(&rgba->r);
    if (rgba->flags & A3DA_RGBA_G)
        a3da_key_free(&rgba->g);
    if (rgba->flags & A3DA_RGBA_B)
        a3da_key_free(&rgba->b);
    if (rgba->flags & A3DA_RGBA_A)
        a3da_key_free(&rgba->a);
}

void a3da_vec3_free(a3da_vec3* vec) {
    a3da_key_free(&vec->x);
    a3da_key_free(&vec->y);
    a3da_key_free(&vec->z);
}

void a3da_model_transform_free(a3da_model_transform* mt) {
    a3da_vec3_free(&mt->rotation);
    a3da_vec3_free(&mt->scale);
    a3da_vec3_free(&mt->translation);
    a3da_key_free(&mt->visibility);
}

void a3da_ambient_free(a3da_ambient* a) {
    if (a->flags & A3DA_AMBIENT_LIGHT_DIFFUSE)
        a3da_rgba_free(&a->light_diffuse);
    string_free(&a->name);
    if (a->flags & A3DA_AMBIENT_RIM_LIGHT_DIFFUSE)
        a3da_rgba_free(&a->rim_light_diffuse);
}

void a3da_camera_auxiliary_free(a3da_camera_auxiliary* ca) {
    if (ca->flags & A3DA_CAMERA_AUXILIARY_AUTO_EXPOSURE)
        a3da_key_free(&ca->auto_exposure);
    if (ca->flags & A3DA_CAMERA_AUXILIARY_EXPOSURE)
        a3da_key_free(&ca->exposure);
    if (ca->flags & A3DA_CAMERA_AUXILIARY_EXPOSURE_RATE)
        a3da_key_free(&ca->exposure_rate);
    if (ca->flags & A3DA_CAMERA_AUXILIARY_GAMMA)
        a3da_key_free(&ca->gamma);
    if (ca->flags & A3DA_CAMERA_AUXILIARY_GAMMA_RATE)
        a3da_key_free(&ca->gamma_rate);
    if (ca->flags & A3DA_CAMERA_AUXILIARY_SATURATE)
        a3da_key_free(&ca->saturate);
}

void a3da_camera_root_free(a3da_camera_root* cr) {
    a3da_model_transform_free(&cr->interest);
    a3da_model_transform_free(&cr->model_transform);
    a3da_camera_root_view_point_free(&cr->view_point);
}

void a3da_camera_root_view_point_free(a3da_camera_root_view_point* crvp) {
    if (crvp->flags & A3DA_CAMERA_ROOT_VIEW_POINT_FOV)
        a3da_key_free(&crvp->fov);
    else
        a3da_key_free(&crvp->focal_length);
    a3da_model_transform_free(&crvp->model_transform);
    if (crvp->flags & A3DA_CAMERA_ROOT_VIEW_POINT_ROLL)
        a3da_key_free(&crvp->roll);
}

void a3da_chara_free(a3da_chara* c) {
    a3da_model_transform_free(&c->model_transform);
    string_free(&c->name);
}

void a3da_curve_free(a3da_curve* c) {
    a3da_key_free(&c->curve);
    string_free(&c->name);
}

void a3da_dof_free(a3da_dof* d) {
    a3da_model_transform_free(&d->model_transform);
}

void a3da_event_free(a3da_event* e) {
    string_free(&e->name);
    string_free(&e->param_1);
    string_free(&e->ref);
}

void a3da_fog_free(a3da_fog* f) {
    if (f->flags & A3DA_FOG_COLOR)
        a3da_rgba_free(&f->color);
    if (f->flags & A3DA_FOG_DENSITY)
        a3da_key_free(&f->density);
    if (f->flags & A3DA_FOG_END)
        a3da_key_free(&f->end);
    if (f->flags & A3DA_FOG_START)
        a3da_key_free(&f->start);
}

void a3da_light_free(a3da_light* l) {
    if (l->flags & A3DA_LIGHT_AMBIENT)
        a3da_rgba_free(&l->ambient);
    if (l->flags & A3DA_LIGHT_CONE_ANGLE)
        a3da_key_free(&l->cone_angle);
    if (l->flags & A3DA_LIGHT_CONSTANT)
        a3da_key_free(&l->constant);
    if (l->flags & A3DA_LIGHT_DIFFUSE)
        a3da_rgba_free(&l->diffuse);
    if (l->flags & A3DA_LIGHT_DROP_OFF)
        a3da_key_free(&l->drop_off);
    if (l->flags & A3DA_LIGHT_FAR)
        a3da_key_free(&l->_far);
    if (l->flags & A3DA_LIGHT_INTENSITY)
        a3da_key_free(&l->intensity);
    if (l->flags & A3DA_LIGHT_LINEAR)
        a3da_key_free(&l->linear);
    if (l->flags & A3DA_LIGHT_POSITION)
        a3da_model_transform_free(&l->position);
    if (l->flags & A3DA_LIGHT_QUADRATIC)
        a3da_key_free(&l->quadratic);
    if (l->flags & A3DA_LIGHT_SPECULAR)
        a3da_rgba_free(&l->specular);
    if (l->flags & A3DA_LIGHT_SPOT_DIRECTION)
        a3da_model_transform_free(&l->spot_direction);
    if (l->flags & A3DA_LIGHT_TONE_CURVE)
        a3da_rgba_free(&l->tone_curve);
    string_free(&l->type);
}

void a3da_m_object_hrc_free(a3da_m_object_hrc* moh) {
    vector_a3da_object_instance_free(&moh->instance,
        a3da_object_instance_free);
    a3da_model_transform_free(&moh->model_transform);
    string_free(&moh->name);
    vector_a3da_object_node_free(&moh->node, a3da_object_node_free);
}

void a3da_material_list_free(a3da_material_list* ml) {
    if (ml->flags & A3DA_MATERIAL_LIST_BLEND_COLOR)
        a3da_rgba_free(&ml->blend_color);
    if (ml->flags & A3DA_MATERIAL_LIST_GLOW_INTENSITY)
        a3da_key_free(&ml->glow_intensity);
    if (ml->flags & A3DA_MATERIAL_LIST_INCANDESCENCE)
        a3da_rgba_free(&ml->incandescence);
    string_free(&ml->name);
}

void a3da_object_free(a3da_object* o) {
    a3da_model_transform_free(&o->model_transform);
    string_free(&o->morph);
    string_free(&o->name);
    string_free(&o->parent_name);
    string_free(&o->parent_node);
    string_free(&o->pattern);
    vector_a3da_object_texture_pattern_free(&o->texture_pattern,
        a3da_object_texture_pattern_free);
    vector_a3da_object_texture_transform_free(&o->texture_transform,
        a3da_object_texture_transform_free);
    string_free(&o->uid_name);
}

void a3da_object_hrc_free(a3da_object_hrc* oh) {
    string_free(&oh->name);
    vector_a3da_object_node_free(&oh->node, a3da_object_node_free);
    string_free(&oh->parent_name);
    string_free(&oh->parent_node);
    string_free(&oh->uid_name);
}

void a3da_object_instance_free(a3da_object_instance* oi) {
    a3da_model_transform_free(&oi->model_transform);
    string_free(&oi->name);
    string_free(&oi->uid_name);
}

void a3da_object_node_free(a3da_object_node* on) {
    a3da_model_transform_free(&on->model_transform);
    string_free(&on->name);
}

void a3da_object_texture_pattern_free(a3da_object_texture_pattern* otp) {
    string_free(&otp->name);
    string_free(&otp->pattern);
}

void a3da_object_texture_transform_free(a3da_object_texture_transform* ott) {
    if (ott->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_COVERAGE_U)
        a3da_key_free(&ott->coverage_u);
    if (ott->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_COVERAGE_V)
        a3da_key_free(&ott->coverage_v);
    string_free(&ott->name);
    if (ott->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_OFFSET_U)
        a3da_key_free(&ott->offset_u);
    if (ott->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_OFFSET_V)
        a3da_key_free(&ott->offset_v);
    if (ott->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_REPEAT_U)
        a3da_key_free(&ott->repeat_u);
    if (ott->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_REPEAT_V)
        a3da_key_free(&ott->repeat_v);
    if (ott->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_ROTATE)
        a3da_key_free(&ott->rotate);
    if (ott->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_ROTATE_FRAME)
        a3da_key_free(&ott->rotate_frame);
    if (ott->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_TRANSLATE_FRAME_U)
        a3da_key_free(&ott->translate_frame_u);
    if (ott->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_TRANSLATE_FRAME_V)
        a3da_key_free(&ott->translate_frame_v);
}

void a3da_point_free(a3da_point* p) {
    a3da_model_transform_free(&p->model_transform);
    string_free(&p->name);
}

void a3da_post_process_free(a3da_post_process* pp) {
    if (pp->flags & A3DA_POST_PROCESS_INTENSITY)
        a3da_rgba_free(&pp->intensity);
    if (pp->flags & A3DA_POST_PROCESS_LENS_FLARE)
        a3da_key_free(&pp->lens_flare);
    if (pp->flags & A3DA_POST_PROCESS_LENS_GHOST)
        a3da_key_free(&pp->lens_ghost);
    if (pp->flags & A3DA_POST_PROCESS_LENS_SHAFT)
        a3da_key_free(&pp->lens_shaft);
    if (pp->flags & A3DA_POST_PROCESS_RADIUS)
        a3da_rgba_free(&pp->radius);
    if (pp->flags & A3DA_POST_PROCESS_SCENE_FADE)
        a3da_rgba_free(&pp->scene_fade);
}

static void a3da_read_inner(a3da* a, stream* s) {
    a3dc_header header;
    memset(&header, 0, sizeof(a3dc_header));

    a->format = A3DA_FORMAT_F;
    uint32_t signature = io_read_uint32_t(s);
    io_set_position(s, 0x00, SEEK_SET);

    stream _s;
    if (signature == reverse_endianness_int32_t('A3DA')) {
        f2_struct st;
        f2_struct_sread(&st, s);
        io_mopen(&_s, st.data, st.length);
        a->format = A3DA_FORMAT_F2;
        f2_struct_free(&st);
    }
    else {
        size_t length = s->length;
        void* data = force_malloc(length);
        io_read(s, data, length);
        io_mopen(&_s, data, length);
        free(data);
    }

    signature = io_read_uint32_t(&_s);
    if (signature != reverse_endianness_int32_t('#A3D'))
        goto End;

    signature = io_read_uint32_t(&_s);
    if ((signature & 0xFF) == 'A') {
        header.string_offset = 0x10;
        header.string_length = (int32_t)(_s.length - 0x10);
    }
    else if ((signature & 0xFF) == 'C') {
        io_set_position(&_s, 0x10, SEEK_SET);
        io_read_int32_t(&_s);
        io_read_int32_t(&_s);
        uint32_t sub_headers_offset = io_read_uint32_t_reverse_endianness(&_s, true);
        uint16_t sub_headers_count = io_read_uint16_t_reverse_endianness(&_s, true);
        uint16_t sub_headers_stride = io_read_uint16_t_reverse_endianness(&_s, true);

        if (sub_headers_count != 0x02)
            goto End;

        io_set_position(&_s, sub_headers_offset, SEEK_SET);
        if (io_read_int32_t(&_s) != 0x50)
            goto End;
        header.string_offset = io_read_uint32_t_reverse_endianness(&_s, true);
        header.string_length = io_read_uint32_t_reverse_endianness(&_s, true);

        io_set_position(&_s, (ssize_t)sub_headers_offset + sub_headers_stride, SEEK_SET);
        if (io_read_int32_t(&_s) != 0x4C42)
            goto End;
        header.binary_offset = io_read_uint32_t_reverse_endianness(&_s, true);
        header.binary_length = io_read_uint32_t_reverse_endianness(&_s, true);
    }
    else
        goto End;

    io_set_position(&_s, header.string_offset, SEEK_SET);
    void* a3da_data = force_malloc(header.string_length);
    io_read(&_s, a3da_data, header.string_length);
    a3da_read_text(a, a3da_data, header.string_length);
    free(a3da_data);

    if (signature == reverse_endianness_int32_t('C___')) {
        a->compressed = true;
        io_set_position(&_s, header.binary_offset, SEEK_SET);
        void* a3dc_data = force_malloc(header.binary_length);
        io_read(&_s, a3dc_data, header.binary_length);
        a3da_read_data(a, a3dc_data, header.binary_length);
        free(a3dc_data);
    }
    a->ready = true;

End:
    io_free(&_s);
}

static void a3da_write_inner(a3da* a, stream* s) {
    bool a3dc = a->compressed || a->format > A3DA_FORMAT_AFT;

    void* a3dc_data = 0;
    size_t a3dc_data_length = 0;
    if (a3dc)
        a3da_write_data(a, &a3dc_data, &a3dc_data_length);

    void* a3da_data = 0;
    size_t a3da_data_length = 0;
    a3da_write_text(a, &a3da_data, &a3da_data_length, a3dc);

    stream s_a3da;
    stream* _s = s;
    if (a->format > A3DA_FORMAT_AFT) {
        io_mopen(&s_a3da, 0, 0);
        _s = &s_a3da;
    }

    if (a3dc) {
        a3dc_header header;
        memset(&header, 0, sizeof(a3dc_header));
        header.string_offset = (uint32_t)0x40;
        header.string_length = (uint32_t)a3da_data_length;
        header.binary_offset = (uint32_t)(0x40 + align_val(a3da_data_length, 0x20));
        header.binary_length = (uint32_t)a3dc_data_length;

        io_write(_s, "#A3DC__________\n", 16);
        io_write_int32_t(_s, 0x2000);
        io_write_int32_t(_s, 0x00);
        io_write_uint32_t_reverse_endianness(_s, 0x20, true);
        io_write_uint16_t_reverse_endianness(_s, 0x02, true);
        io_write_uint16_t_reverse_endianness(_s, 0x10, true);
        io_write_char(_s, 'P');
        io_align_write(_s, 0x04);
        io_write_uint32_t_reverse_endianness(_s, header.string_offset, true);
        io_write_uint32_t_reverse_endianness(_s, header.string_length, true);
        io_write_uint32_t_reverse_endianness(_s, 0x01, true);
        io_write_char(_s, 'B');
        io_write_char(_s, 'L');
        io_align_write(_s, 0x04);
        io_write_uint32_t_reverse_endianness(_s, header.binary_offset, true);
        io_write_uint32_t_reverse_endianness(_s, header.binary_length, true);
        io_write_uint32_t_reverse_endianness(_s, 0x20, true);
        io_write(_s, a3da_data, a3da_data_length);
        io_align_write(_s, 0x20);
        io_write(_s, a3dc_data, a3dc_data_length);
        free(a3dc_data);
    }
    else
        io_write(_s, a3da_data, a3da_data_length);
    free(a3da_data);

    if (a->format > A3DA_FORMAT_AFT) {
        f2_struct st;
        memset(&st, 0, sizeof(f2_struct));

        io_align_write(&s_a3da, 0x10);
        io_mcopy(&s_a3da, &st.data, &st.length);
        io_free(&s_a3da);

        st.header.signature = reverse_endianness_uint32_t('A3DA');
        st.header.length = 0x40;
        st.header.use_big_endian = false;
        st.header.use_section_size = true;
        st.header.inner_signature = a->format == A3DA_FORMAT_XHD ? 0x00131010 : 0x01131010;

        f2_struct_swrite(&st, s, true, a->format == A3DA_FORMAT_X || a->format == A3DA_FORMAT_XHD);
        f2_struct_free(&st);
    }
}

static void a3da_read_text(a3da* a, void* data, size_t length) {
    char buf[A3DA_TEXT_BUF_SIZE];
    int32_t count;
    int32_t count1;
    size_t len;
    size_t len1;
    size_t len2;
    size_t len3;
    size_t off;

    key_val kv;
    key_val_init(&kv, data, length);
    key_val lkv;
    if (key_val_get_local_key_val(&kv, "_", &lkv)) {
        len = 1;
        memcpy(buf, "_", 1);
        off = len;

        key_val_read_int32_t(&lkv,
            buf, off, ".compress_f16", 14, (int32_t*)&a->_compress_f16);
        key_val_read_string(&lkv,
            buf, off, ".converter.version", 19, &a->_converter_version);
        key_val_read_string(&lkv,
            buf, off, ".file_name", 11, &a->_file_name);
        key_val_read_string(&lkv,
            buf, off, ".property.version", 19, &a->_property_version);
        key_val_free(&lkv);
    }

    memset(&a->camera_auxiliary, 0, sizeof(a3da_camera_auxiliary));
    if (key_val_get_local_key_val(&kv, "camera_auxiliary", &lkv)) {
        a3da_camera_auxiliary* ca = &a->camera_auxiliary;

        len = 16;
        memcpy(buf, "camera_auxiliary", 16);
        off = len;

        if (key_val_read_a3da_key(&lkv,
            buf, off, ".auto_exposure", 15, &ca->auto_exposure))
            ca->flags |= A3DA_CAMERA_AUXILIARY_AUTO_EXPOSURE;
        if (key_val_read_a3da_key(&lkv,
            buf, off, ".exposure", 10, &ca->exposure))
            ca->flags |= A3DA_CAMERA_AUXILIARY_EXPOSURE;
        if (key_val_read_a3da_key(&lkv,
            buf, off, ".exposure_rate", 15, &ca->exposure_rate))
            ca->flags |= A3DA_CAMERA_AUXILIARY_EXPOSURE_RATE;
        if (key_val_read_a3da_key(&lkv,
            buf, off, ".gamma", 7, &ca->gamma))
            ca->flags |= A3DA_CAMERA_AUXILIARY_GAMMA;
        if (key_val_read_a3da_key(&lkv,
            buf, off, ".gamma_rate", 12, &ca->gamma_rate))
            ca->flags |= A3DA_CAMERA_AUXILIARY_GAMMA_RATE;
        if (key_val_read_a3da_key(&lkv,
            buf, off, ".saturate", 10, &ca->saturate))
            ca->flags |= A3DA_CAMERA_AUXILIARY_SATURATE;

        if (ca->flags & (A3DA_CAMERA_AUXILIARY_EXPOSURE_RATE | A3DA_CAMERA_AUXILIARY_GAMMA_RATE))
            if (a->format < A3DA_FORMAT_F || a->format == A3DA_FORMAT_AFT)
                a->format = A3DA_FORMAT_F;
        key_val_free(&lkv);
    }

    memset(&a->play_control, 0, sizeof(a3da_play_control));
    if (key_val_get_local_key_val(&kv, "play_control", &lkv)) {
        a3da_play_control* pc = &a->play_control;

        len = 12;
        memcpy(buf, "play_control", 12);
        off = len;

        key_val_read_float_t(&lkv,
            buf, off, ".begin", 5, &pc->begin);
        if (key_val_read_int32_t(&lkv,
            buf, off, ".div", 5, &pc->div))
            pc->flags |= A3DA_PLAY_CONTROL_DIV;
        key_val_read_float_t(&lkv,
            buf, off, ".fps", 5, &pc->fps);
        if (key_val_read_float_t(&lkv,
            buf, off, ".offset", 8, &pc->offset))
            pc->flags |= A3DA_PLAY_CONTROL_OFFSET;
        key_val_read_float_t(&lkv,
            buf, off, ".size", 6, &pc->size);
        key_val_free(&lkv);
    }

    memset(&a->post_process, 0, sizeof(a3da_post_process));
    if (key_val_get_local_key_val(&kv, "post_process", &lkv)) {
        a3da_post_process* pp = &a->post_process;

        len = 12;
        memcpy(buf, "post_process", 12);
        off = len;

        if (key_val_read_a3da_rgba(&lkv,
            buf, off, ".Diffuse", 9, &pp->intensity))
            pp->flags |= A3DA_POST_PROCESS_INTENSITY;
        if (key_val_read_a3da_key(&lkv,
            buf, off, ".lens_flare", 12, &pp->lens_flare))
            pp->flags |= A3DA_POST_PROCESS_LENS_FLARE;
        if (key_val_read_a3da_key(&lkv,
            buf, off, ".lens_ghost", 12, &pp->lens_ghost))
            pp->flags |= A3DA_POST_PROCESS_LENS_GHOST;
        if (key_val_read_a3da_key(&lkv,
            buf, off, ".lens_shaft", 12, &pp->lens_shaft))
            pp->flags |= A3DA_POST_PROCESS_LENS_SHAFT;
        if (key_val_read_a3da_rgba(&lkv,
            buf, off, ".Ambient", 9, &pp->radius))
            pp->flags |= A3DA_POST_PROCESS_RADIUS;
        if (key_val_read_a3da_rgba(&lkv,
            buf, off, ".Specular", 10, &pp->scene_fade))
            pp->flags |= A3DA_POST_PROCESS_SCENE_FADE;
        key_val_free(&lkv);
    }

    memset(&a->dof, 0, sizeof(a3da_dof));
    if (key_val_get_local_key_val(&kv, "dof", &lkv)) {
        a3da_dof* d = &a->dof;

        len = 3;
        memcpy(buf, "dof", 3);
        off = len;

        d->has_dof = key_val_read_a3da_model_transform(&lkv,
            buf, off, "", 1, &d->model_transform);
        key_val_free(&lkv);
    }

    len = 7;
    memcpy(buf, "ambient", 7);
    off = len;

    a->ambient = vector_empty(a3da_ambient);
    if (key_val_read_int32_t(&kv, buf, off, ".length", 8, &count)
        && key_val_get_local_key_val(&kv, "ambient", &lkv)) {
        vector_a3da_ambient* va = &a->ambient;

        vector_a3da_ambient_reserve(va, count);
        va->end += count;
        for (int32_t i = 0; i < count; i++) {
            a3da_ambient* a = &va->begin[i];
            memset(a, 0, sizeof(a3da_ambient));
            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", i);
            off = len + len1;

            if (key_val_read_a3da_rgba(&lkv,
                buf, off, ".light.Diffuse", 15, &a->light_diffuse))
                a->flags |= A3DA_AMBIENT_LIGHT_DIFFUSE;
            key_val_read_string(&lkv,
                buf, off, ".name", 6, &a->name);
            if (key_val_read_a3da_rgba(&lkv,
                buf, off, ".rim.light.Diffuse", 19, &a->rim_light_diffuse))
                a->flags |= A3DA_AMBIENT_RIM_LIGHT_DIFFUSE;
        }
        key_val_free(&lkv);
    }

    len = 7;
    memcpy(buf, "auth_2d", 7);
    off = len;

    a->auth_2d = vector_empty(string);
    if (key_val_read_int32_t(&kv, buf, off, ".length", 8, &count)
        && key_val_get_local_key_val(&kv, "auth_2d", &lkv)) {
        vector_string* va2 = &a->auth_2d;

        vector_string_reserve(va2, count);
        va2->end += count;
        for (int32_t i = 0; i < count; i++) {
            string* a2 = &va2->begin[i];
            *a2 = string_empty;
            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", i);
            off = len + len1;

            key_val_read_string(&lkv,
                buf, off, ".name", 6, a2);
        }
        key_val_free(&lkv);
    }

    len = 11;
    memcpy(buf, "camera_root", 11);
    off = len;

    a->camera_root = vector_empty(a3da_camera_root);
    if (key_val_read_int32_t(&kv, buf, off, ".length", 8, &count)
        && key_val_get_local_key_val(&kv, "camera_root", &lkv)) {
        vector_a3da_camera_root* vcr = &a->camera_root;

        vector_a3da_camera_root_reserve(vcr, count);
        vcr->end += count;
        for (int32_t i = 0; i < count; i++) {
            a3da_camera_root* cr = &vcr->begin[i];
            memset(cr, 0, sizeof(a3da_camera_root));
            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", i);
            off = len + len1;

            key_val_read_a3da_model_transform(&lkv,
                buf, off, ".interest", 10, &cr->interest);
            key_val_read_a3da_model_transform(&lkv,
                buf, off, "", 1, &cr->model_transform);

            a3da_camera_root_view_point* vp = &cr->view_point;

            len2 = 11;
            memcpy(buf + len + len1, ".view_point", 11);
            off = len + len1 + len2;

            key_val_read_float_t(&lkv,
                buf, off, ".aspect", 8, &vp->aspect);

            memcpy(buf + len + len1 + len2, ".fov_is_horizontal", 19);
            if (key_val_has_key(&lkv, buf)) {
                key_val_read_a3da_key(&lkv,
                    buf, off, ".fov", 5, &vp->fov);
                key_val_read_bool(&lkv,
                    buf, off, ".fov_is_horizontal", 19, &vp->fov_is_horizontal);
                vp->flags |= A3DA_CAMERA_ROOT_VIEW_POINT_FOV;
            }
            else {
                key_val_read_float_t(&lkv,
                    buf, off, ".camera_aperture_h", 19, &vp->camera_aperture_h);
                key_val_read_float_t(&lkv,
                    buf, off, ".camera_aperture_w", 19, &vp->camera_aperture_w);
                key_val_read_a3da_key(&lkv,
                    buf, off, ".focal_length", 14, &vp->focal_length);
            }
            key_val_read_a3da_model_transform(&lkv,
                buf, off, "", 1, &vp->model_transform);
            if (key_val_read_a3da_key(&lkv,
                buf, off, ".roll", 6, &vp->roll))
                vp->flags |= A3DA_CAMERA_ROOT_VIEW_POINT_ROLL;
        }
        key_val_free(&lkv);
    }

    len = 5;
    memcpy(buf, "chara", 5);
    off = len;

    a->chara = vector_empty(a3da_chara);
    if (key_val_read_int32_t(&kv, buf, off, ".length", 8, &count)
        && key_val_get_local_key_val(&kv, "chara", &lkv)) {
        vector_a3da_chara* vc = &a->chara;

        vector_a3da_chara_reserve(vc, count);
        vc->end += count;
        for (int32_t i = 0; i < count; i++) {
            a3da_chara* c = &vc->begin[i];
            memset(c, 0, sizeof(a3da_chara));
            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", i);
            off = len + len1;

            key_val_read_a3da_model_transform(&lkv,
                buf, off, "", 1, &c->model_transform);
            key_val_read_string(&lkv,
                buf, off, ".name", 6, &c->name);
        }
    }

    len = 5;
    memcpy(buf, "curve", 5);
    off = len;

    a->curve = vector_empty(a3da_curve);
    if (key_val_read_int32_t(&kv, buf, off, ".length", 8, &count)
        && key_val_get_local_key_val(&kv, "curve", &lkv)) {
        vector_a3da_curve* vc = &a->curve;

        vector_a3da_curve_reserve(vc, count);
        vc->end += count;
        for (int32_t i = 0; i < count; i++) {
            a3da_curve* c = &vc->begin[i];
            memset(c, 0, sizeof(a3da_curve));
            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", i);
            off = len + len1;

            key_val_read_a3da_key(&lkv,
                buf, off, ".cv", 4, &c->curve);
            key_val_read_string(&lkv,
                buf, off, ".name", 6, &c->name);
        }
        key_val_free(&lkv);
    }

    len = 5;
    memcpy(buf, "event", 5);
    off = len;

    a->event = vector_empty(a3da_event);
    if (key_val_read_int32_t(&kv, buf, off, ".length", 8, &count)
        && key_val_get_local_key_val(&kv, "event", &lkv)) {
        vector_a3da_event* ve = &a->event;

        vector_a3da_event_reserve(ve, count);
        ve->end += count;
        for (int32_t i = 0; i < count; i++) {
            a3da_event* e = &ve->begin[i];
            memset(e, 0, sizeof(a3da_event));
            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", i);
            off = len + len1;

            key_val_read_float_t(&lkv,
                buf, off, ".begin", 7, &e->begin);
            key_val_read_float_t(&lkv,
                buf, off, ".clip_begin", 12, &e->clip_begin);
            key_val_read_float_t(&lkv,
                buf, off, ".clip_end", 10, &e->clip_end);
            key_val_read_float_t(&lkv,
                buf, off, ".end", 5, &e->end);
            key_val_read_string(&lkv,
                buf, off, ".name", 6, &e->name);
            key_val_read_string(&lkv,
                buf, off, ".param1", 8, &e->param_1);
            key_val_read_string(&lkv,
                buf, off, ".ref", 5, &e->ref);
            key_val_read_float_t(&lkv,
                buf, off, ".time_ref_scale", 16, &e->time_ref_scale);
            key_val_read_int32_t(&lkv,
                buf, off, ".type", 6, (int32_t*)&e->type);
        }
        key_val_free(&lkv);
    }

    len = 3;
    memcpy(buf, "fog", 3);
    off = len;

    a->fog = vector_empty(a3da_fog);
    if (key_val_read_int32_t(&kv, buf, off, ".length", 8, &count)
        && key_val_get_local_key_val(&kv, "fog", &lkv)) {
        vector_a3da_fog* vf = &a->fog;

        vector_a3da_fog_reserve(vf, count);
        vf->end += count;
        for (int32_t i = 0; i < count; i++) {
            a3da_fog* f = &vf->begin[i];
            memset(f, 0, sizeof(a3da_fog));
            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", i);
            off = len + len1;

            if (key_val_read_a3da_rgba(&lkv,
                buf, off, ".Diffuse", 9, &f->color))
                f->flags |= A3DA_FOG_COLOR;
            if (key_val_read_a3da_key(&lkv,
                buf, off, ".density", 9, &f->density))
                f->flags |= A3DA_FOG_DENSITY;
            if (key_val_read_a3da_key(&lkv,
                buf, off, ".end", 5, &f->end))
                f->flags |= A3DA_FOG_END;
            key_val_read_int32_t(&lkv,
                buf, off, ".id", 4, (int32_t*)&f->id);
            string name;
            if (key_val_read_string(&lkv,
                buf, off, ".name", 7, &name)) {
                if (str_utils_compare(string_data(&name), "Z"))
                    f->id = FOG_DEPTH;
                else if (str_utils_compare(string_data(&name), "Height"))
                    f->id = FOG_HEIGHT;
                string_free(&name);
            }
            if (key_val_read_a3da_key(&lkv,
                buf, off, ".start", 7, &f->start))
                f->flags |= A3DA_FOG_START;
        }
        key_val_free(&lkv);
    }

    len = 5;
    memcpy(buf, "light", 5);
    off = len;

    a->light = vector_empty(a3da_light);
    if (key_val_read_int32_t(&kv, buf, off, ".length", 8, &count)
        && key_val_get_local_key_val(&kv, "light", &lkv)) {
        vector_a3da_light* vl = &a->light;

        vector_a3da_light_reserve(vl, count);
        vl->end += count;
        for (int32_t i = 0; i < count; i++) {
            a3da_light* l = &vl->begin[i];
            memset(l, 0, sizeof(a3da_light));
            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", i);
            off = len + len1;

            if (key_val_read_a3da_rgba(&lkv,
                buf, off, ".Ambient", 9, &l->ambient))
                l->flags |= A3DA_LIGHT_AMBIENT;
            if (key_val_read_a3da_key(&lkv,
                buf, off, ".ConeAngle", 11, &l->cone_angle))
                l->flags |= A3DA_LIGHT_CONE_ANGLE;
            if (key_val_read_a3da_key(&lkv,
                buf, off, ".CONSTANT", 10, &l->constant))
                l->flags |= A3DA_LIGHT_CONSTANT;
            if (key_val_read_a3da_rgba(&lkv,
                buf, off, ".Diffuse", 9, &l->diffuse))
                l->flags |= A3DA_LIGHT_DIFFUSE;
            if (key_val_read_a3da_key(&lkv,
                buf, off, ".DropOff", 9, &l->drop_off))
                l->flags |= A3DA_LIGHT_DROP_OFF;
            if (key_val_read_a3da_key(&lkv,
                buf, off, ".FAR", 5, &l->_far))
                l->flags |= A3DA_LIGHT_FAR;
            key_val_read_int32_t(&lkv,
                buf, off, ".id", 4, (int32_t*)&l->id);
            if (key_val_read_a3da_key(&lkv,
                buf, off, ".Intensity", 11, &l->intensity))
                l->flags |= A3DA_LIGHT_INTENSITY;
            if (key_val_read_a3da_key(&lkv,
                buf, off, ".LINEAR", 8, &l->linear))
                l->flags |= A3DA_LIGHT_LINEAR;
            if (key_val_read_a3da_model_transform(&lkv,
                buf, off, ".position", 10, &l->position))
                l->flags |= A3DA_LIGHT_POSITION;
            if (key_val_read_a3da_key(&lkv,
                buf, off, ".QUADRATIC", 11, &l->quadratic))
                l->flags |= A3DA_LIGHT_QUADRATIC;
            if (key_val_read_a3da_rgba(&lkv,
                buf, off, ".Specular", 10, &l->specular))
                l->flags |= A3DA_LIGHT_SPECULAR;
            if (key_val_read_a3da_model_transform(&lkv,
                buf, off, ".spot_direction", 16, &l->spot_direction))
                l->flags |= A3DA_LIGHT_SPOT_DIRECTION;
            if (key_val_read_a3da_rgba(&lkv,
                buf, off, ".Incandescence", 15, &l->tone_curve))
                l->flags |= A3DA_LIGHT_TONE_CURVE;
            key_val_read_string(&lkv,
                buf, off, ".type", 6, &l->type);
        }
        key_val_free(&lkv);
    }

    len = 8;
    memcpy(buf, "m_objhrc", 8);
    off = len;

    a->m_object_hrc = vector_empty(a3da_m_object_hrc);
    if (key_val_read_int32_t(&kv, buf, off, ".length", 8, &count)
        && key_val_get_local_key_val(&kv, "m_objhrc", &lkv)) {
        vector_a3da_m_object_hrc* vmoh = &a->m_object_hrc;

        vector_a3da_m_object_hrc_reserve(vmoh, count);
        vmoh->end += count;
        for (int32_t i = 0; i < count; i++) {
            a3da_m_object_hrc* moh = &vmoh->begin[i];
            memset(moh, 0, sizeof(a3da_m_object_hrc));
            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", i);
            off = len + len1;

            len2 = 9;
            memcpy(buf + len + len1, ".instance", 9);
            off = len + len1 + len2;

            buf[off] = 0;
            key_val sub_local_key_val;
            if (key_val_read_int32_t(&lkv, buf, off, ".length", 8, &count1)
                && key_val_get_local_key_val(&lkv, buf, &sub_local_key_val) ) {
                vector_a3da_object_instance* voi = &moh->instance;

                vector_a3da_object_instance_reserve(voi, count1);
                voi->end += count1;
                for (int32_t j = 0; j < count1; j++) {
                    a3da_object_instance* oi = &voi->begin[j];
                    memset(oi, 0, sizeof(a3da_object_instance));
                    len3 = sprintf_s(buf + len + len1 + len2,
                        A3DA_TEXT_BUF_SIZE - len - len1 - len2, ".%d", j);
                    off = len + len1 + len2 + len3;

                    key_val_read_a3da_model_transform(&sub_local_key_val,
                        buf, off, "", 1, &oi->model_transform);
                    key_val_read_string(&sub_local_key_val,
                        buf, off, ".name", 6, &oi->name);
                    key_val_read_bool(&sub_local_key_val,
                        buf, off, ".shadow", 8, &oi->shadow);
                    key_val_read_string(&sub_local_key_val,
                        buf, off, ".uid_name", 10, &oi->uid_name);
                }
                key_val_free(&sub_local_key_val);
            }

            off = len + len1;
            key_val_read_a3da_model_transform(&lkv,
                buf, off, "", 1, &moh->model_transform);
            key_val_read_string(&lkv,
                buf, off, ".name", 6, &moh->name);

            len2 = 5;
            memcpy(buf + len + len1, ".node", 5);
            off = len + len1 + len2;

            buf[off] = 0;
            if (key_val_read_int32_t(&lkv, buf, off, ".length", 8, &count1)
                && key_val_get_local_key_val(&lkv, buf, &sub_local_key_val)) {
                vector_a3da_object_node* vmohn = &moh->node;

                vector_a3da_object_node_reserve(vmohn, count1);
                vmohn->end += count1;
                for (int32_t j = 0; j < count1; j++) {
                    a3da_object_node* mohn = &vmohn->begin[j];
                    memset(mohn, 0, sizeof(a3da_object_node));
                    len3 = sprintf_s(buf + len + len1 + len2,
                        A3DA_TEXT_BUF_SIZE - len - len1 - len2, ".%d", j);
                    off = len + len1 + len2 + len3;

                    if (key_val_read_vec3(&sub_local_key_val,
                        buf, off, ".joint_orient", 14, &mohn->joint_orient))
                        mohn->flags |= A3DA_OBJECT_NODE_JOINT_ORIENT;
                    key_val_read_a3da_model_transform(&sub_local_key_val,
                        buf, off, "", 1, &mohn->model_transform);
                    key_val_read_string(&sub_local_key_val,
                        buf, off, ".name", 6, &mohn->name);
                    key_val_read_int32_t(&sub_local_key_val,
                        buf, off, ".parent", 8, &mohn->parent);
                }
                key_val_free(&sub_local_key_val);
            }
        }
        key_val_free(&lkv);
    }

    len = 13;
    memcpy(buf, "m_objhrc_list", 13);
    off = len;

    a->m_object_hrc_list = vector_empty(string);
    if (key_val_read_int32_t(&kv, buf, off, ".length", 8, &count)
        && key_val_get_local_key_val(&kv, "m_objhrc_list", &lkv)) {
        vector_string* vmohl = &a->m_object_hrc_list;

        vector_string_reserve(vmohl, count);
        vmohl->end += count;
        for (int32_t i = 0; i < count; i++) {
            string* mohl = &vmohl->begin[i];
            *mohl = string_empty;
            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", i);
            off = len + len1;

            key_val_read_string(&lkv,
                buf, off, "", 1, mohl);
        }
        key_val_free(&lkv);
    }

    len = 13;
    memcpy(buf, "material_list", 13);
    off = len;

    a->material_list = vector_empty(a3da_material_list);
    if (key_val_read_int32_t(&kv, buf, off, ".length", 8, &count)
        && key_val_get_local_key_val(&kv, "material_list", &lkv)) {
        vector_a3da_material_list* vml = &a->material_list;

        vector_a3da_material_list_reserve(vml, count);
        vml->end += count;
        for (int32_t i = 0; i < count; i++) {
            a3da_material_list* ml = &vml->begin[i];
            memset(ml, 0, sizeof(a3da_material_list));
            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", i);
            off = len + len1;

            if (key_val_read_a3da_rgba(&lkv,
                buf, off, ".blend_color", 15, &ml->blend_color))
                ml->flags |= A3DA_MATERIAL_LIST_BLEND_COLOR;
            if (key_val_read_a3da_key(&lkv,
                buf, off, ".glow_intensity", 15, &ml->glow_intensity))
                ml->flags |= A3DA_MATERIAL_LIST_GLOW_INTENSITY;
            if (key_val_read_a3da_rgba(&lkv,
                buf, off, ".incandescence", 15, &ml->incandescence))
                ml->flags |= A3DA_MATERIAL_LIST_INCANDESCENCE;
            key_val_read_string(&lkv,
                buf, off, ".name", 6, &ml->name);
        }
        key_val_free(&lkv);
    }

    len = 6;
    memcpy(buf, "motion", 6);
    off = len;

    a->motion = vector_empty(string);
    if (key_val_read_int32_t(&kv, buf, off, ".length", 8, &count)
        && key_val_get_local_key_val(&kv, "motion", &lkv)) {
        vector_string* vm = &a->motion;

        vector_string_reserve(vm, count);
        vm->end += count;
        for (int32_t i = 0; i < count; i++) {
            string* m = &vm->begin[i];
            *m = string_empty;
            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", i);
            off = len + len1;

            key_val_read_string(&lkv,
                buf, off, "", 1, m);
        }
        key_val_free(&lkv);
    }

    len = 6;
    memcpy(buf, "object", 6);
    off = len;

    a->object = vector_empty(a3da_object);
    if (key_val_read_int32_t(&kv, buf, off, ".length", 8, &count)
        && key_val_get_local_key_val(&kv, "object", &lkv)) {
        vector_a3da_object* vo = &a->object;

        vector_a3da_object_reserve(vo, count);
        vo->end += count;
        for (int32_t i = 0; i < count; i++) {
            a3da_object* o = &vo->begin[i];
            memset(o, 0, sizeof(a3da_object));
            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", i);
            off = len + len1;

            key_val_read_a3da_model_transform(&lkv,
                buf, off, "", 1, &o->model_transform);
            if (key_val_read_string(&lkv,
                buf, off, ".morph", 7, &o->morph))
                key_val_read_float_t(&lkv,
                    buf, off, ".morph_offset", 14, &o->morph_offset);
            key_val_read_string(&lkv,
                buf, off, ".name", 6, &o->name);
            key_val_read_string(&lkv,
                buf, off, ".parent_name", 13, &o->parent_name);
            key_val_read_string(&lkv,
                buf, off, ".parent_node", 13, &o->parent_node);
            if (key_val_read_string(&lkv,
                buf, off, ".pat", 5, &o->pattern))
                key_val_read_float_t(&lkv,
                    buf, off, ".pat_offset", 12, &o->pattern_offset);

            len2 = 8;
            memcpy(buf + len + len1, ".tex_pat", 8);
            off = len + len1 + len2;

            buf[off] = 0;
            key_val sub_local_key_val;
            if (key_val_read_int32_t(&lkv, buf, off, ".length", 8, &count1)
                && key_val_get_local_key_val(&lkv, buf, &sub_local_key_val)) {
                vector_a3da_object_texture_pattern* votp = &o->texture_pattern;

                vector_a3da_object_texture_pattern_reserve(votp, count1);
                votp->end += count1;
                for (int32_t j = 0; j < count1; j++) {
                    a3da_object_texture_pattern* otp = &votp->begin[j];
                    memset(otp, 0, sizeof(a3da_object_texture_pattern));
                    len3 = sprintf_s(buf + len + len1 + len2,
                        A3DA_TEXT_BUF_SIZE - len - len1 - len2, ".%d", j);
                    off = len + len1 + len2 + len3;

                    key_val_read_string(&sub_local_key_val,
                        buf, off, ".name", 6, &otp->name);
                    if (key_val_read_string(&sub_local_key_val,
                        buf, off, ".pat", 5, &otp->pattern))
                        key_val_read_float_t(&sub_local_key_val,
                            buf, off, ".pat_offset", 12, &otp->pattern_offset);
                }
                key_val_free(&sub_local_key_val);
            }

            len2 = 14;
            memcpy(buf + len + len1, ".tex_transform", 14);
            off = len + len1 + len2;

            buf[off] = 0;
            if (key_val_read_int32_t(&lkv, buf, off, ".length", 8, &count1)
                && key_val_get_local_key_val(&lkv, buf, &sub_local_key_val)) {
                vector_a3da_object_texture_transform* vott = &o->texture_transform;

                vector_a3da_object_texture_transform_reserve(vott, count1);
                vott->end += count1;
                for (int32_t j = 0; j < count1; j++) {
                    a3da_object_texture_transform* ott = &vott->begin[j];
                    memset(ott, 0, sizeof(a3da_object_texture_transform));
                    len3 = sprintf_s(buf + len + len1 + len2,
                        A3DA_TEXT_BUF_SIZE - len - len1 - len2, ".%d", j);
                    off = len + len1 + len2 + len3;

                    if (key_val_read_a3da_key(&sub_local_key_val,
                        buf, off, ".coverageU", 11, &ott->coverage_u))
                        ott->flags |= A3DA_OBJECT_TEXTURE_TRANSFORM_COVERAGE_U;
                    if (key_val_read_a3da_key(&sub_local_key_val,
                        buf, off, ".coverageV", 11, &ott->coverage_v))
                        ott->flags |= A3DA_OBJECT_TEXTURE_TRANSFORM_COVERAGE_V;
                    key_val_read_string(&sub_local_key_val,
                        buf, off, ".name", 6, &ott->name);
                    if (key_val_read_a3da_key(&sub_local_key_val,
                        buf, off, ".offsetU", 9, &ott->offset_u))
                        ott->flags |= A3DA_OBJECT_TEXTURE_TRANSFORM_OFFSET_U;
                    if (key_val_read_a3da_key(&sub_local_key_val,
                        buf, off, ".offsetV", 9, &ott->offset_v))
                        ott->flags |= A3DA_OBJECT_TEXTURE_TRANSFORM_OFFSET_V;
                    if (key_val_read_a3da_key(&sub_local_key_val,
                        buf, off, ".repeatU", 9, &ott->repeat_u))
                        ott->flags |= A3DA_OBJECT_TEXTURE_TRANSFORM_REPEAT_U;
                    if (key_val_read_a3da_key(&sub_local_key_val,
                        buf, off, ".repeatV", 9, &ott->repeat_v))
                        ott->flags |= A3DA_OBJECT_TEXTURE_TRANSFORM_REPEAT_V;
                    if (key_val_read_a3da_key(&sub_local_key_val,
                        buf, off, ".rotate", 8, &ott->rotate))
                        ott->flags |= A3DA_OBJECT_TEXTURE_TRANSFORM_ROTATE;
                    if (key_val_read_a3da_key(&sub_local_key_val,
                        buf, off, ".rotateFrame", 13, &ott->rotate_frame))
                        ott->flags |= A3DA_OBJECT_TEXTURE_TRANSFORM_ROTATE_FRAME;
                    if (key_val_read_a3da_key(&sub_local_key_val,
                        buf, off, ".translateFrameU", 17, &ott->translate_frame_u))
                        ott->flags |= A3DA_OBJECT_TEXTURE_TRANSFORM_TRANSLATE_FRAME_U;
                    if (key_val_read_a3da_key(&sub_local_key_val,
                        buf, off, ".translateFrameV", 17, &ott->translate_frame_v))
                        ott->flags |= A3DA_OBJECT_TEXTURE_TRANSFORM_TRANSLATE_FRAME_V;
                }
                key_val_free(&sub_local_key_val);
            }

            off = len + len1;
            key_val_read_string(&lkv,
                buf, off, ".uid_name", 10, &o->uid_name);
        }
        key_val_free(&lkv);
    }

    len = 6;
    memcpy(buf, "objhrc", 6);
    off = len;

    a->object_hrc = vector_empty(a3da_object_hrc);
    if (key_val_read_int32_t(&kv, buf, off, ".length", 8, &count)
        && key_val_get_local_key_val(&kv, "objhrc", &lkv)) {
        vector_a3da_object_hrc* voh = &a->object_hrc;

        vector_a3da_object_hrc_reserve(voh, count);
        voh->end += count;
        for (int32_t i = 0; i < count; i++) {
            a3da_object_hrc* oh = &voh->begin[i];
            memset(oh, 0, sizeof(a3da_object_hrc));
            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", i);
            off = len + len1;

            key_val_read_string(&lkv,
                buf, off, ".name", 6, &oh->name);

            len2 = 5;
            memcpy(buf + len + len1, ".node", 5);
            off = len + len1 + len2;

            buf[off] = 0;
            key_val sub_local_key_val;
            if (key_val_read_int32_t(&lkv, buf, off, ".length", 8, &count1)
                && key_val_get_local_key_val(&lkv, buf, &sub_local_key_val)) {
                vector_a3da_object_node* vohn = &oh->node;

                vector_a3da_object_node_reserve(vohn, count1);
                vohn->end += count1;
                for (int32_t j = 0; j < count1; j++) {
                    a3da_object_node* ohn = &vohn->begin[j];
                    memset(ohn, 0, sizeof(a3da_object_node));
                    len3 = sprintf_s(buf + len + len1 + len2,
                        A3DA_TEXT_BUF_SIZE - len - len1 - len2, ".%d", j);
                    off = len + len1 + len2 + len3;

                    if (key_val_read_vec3(&sub_local_key_val,
                        buf, off, ".joint_orient", 14, &ohn->joint_orient))
                        ohn->flags |= A3DA_OBJECT_NODE_JOINT_ORIENT;
                    key_val_read_a3da_model_transform(&sub_local_key_val,
                        buf, off, "", 1, &ohn->model_transform);
                    key_val_read_string(&sub_local_key_val,
                        buf, off, ".name", 6, &ohn->name);
                    key_val_read_int32_t(&sub_local_key_val,
                        buf, off, ".parent", 8, &ohn->parent);
                }
                key_val_free(&sub_local_key_val);
            }

            off = len + len1;
            key_val_read_string(&lkv,
                buf, off, ".parent_name", 13, &oh->parent_name);
            key_val_read_string(&lkv,
                buf, off, ".parent_node", 13, &oh->parent_node);
            key_val_read_bool(&lkv,
                buf, off, ".shadow", 8, &oh->shadow);
            key_val_read_string(&lkv,
                buf, off, ".uid_name", 10, &oh->uid_name);
        }
        key_val_free(&lkv);
    }

    len = 11;
    memcpy(buf, "objhrc_list", 11);
    off = len;

    a->object_hrc_list = vector_empty(string);
    if (key_val_read_int32_t(&kv, buf, off, ".length", 8, &count)
        && key_val_get_local_key_val(&kv, "objhrc_list", &lkv)) {
        vector_string* vohl = &a->object_hrc_list;

        vector_string_reserve(vohl, count);
        vohl->end += count;
        for (int32_t i = 0; i < count; i++) {
            string* ohl = &vohl->begin[i];
            *ohl = string_empty;
            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", i);
            off = len + len1;

            key_val_read_string(&lkv,
                buf, off, "", 1, ohl);
        }
        key_val_free(&lkv);
    }

    len = 11;
    memcpy(buf, "object_list", 11);
    off = len;

    a->object_list = vector_empty(string);
    if (key_val_read_int32_t(&kv, buf, off, ".length", 8, &count)
        && key_val_get_local_key_val(&kv, "object_list", &lkv)) {
        vector_string* vol = &a->object_list;

        vector_string_reserve(vol, count);
        vol->end += count;
        for (int32_t i = 0; i < count; i++) {
            string* ol = &vol->begin[i];
            *ol = string_empty;
            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", i);
            off = len + len1;

            key_val_read_string(&lkv,
                buf, off, "", 1, ol);
        }
        key_val_free(&lkv);
    }

    len = 5;
    memcpy(buf, "point", 5);
    off = len;

    a->point = vector_empty(a3da_point);
    if (key_val_read_int32_t(&kv, buf, off, ".length", 8, &count)
        && key_val_get_local_key_val(&kv, "point", &lkv)) {
        vector_a3da_point* vp = &a->point;

        vector_a3da_point_reserve(vp, count);
        vp->end += count;
        for (int32_t i = 0; i < count; i++) {
            a3da_point* p = &vp->begin[i];
            memset(p, 0, sizeof(a3da_point));
            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", i);
            off = len + len1;

            key_val_read_a3da_model_transform(&lkv,
                buf, off, "", 1, &p->model_transform);
            key_val_read_string(&lkv,
                buf, off, ".name", 6, &p->name);
        }
        key_val_free(&lkv);
    }

    key_val_free(&kv);
}

static void a3da_write_text(a3da* a, void** data, size_t* length, bool a3dc) {
    char buf[A3DA_TEXT_BUF_SIZE];
    int32_t count;
    int32_t count1;
    size_t len;
    size_t len1;
    size_t len2;
    size_t len3;
    size_t off;

    char a3da_timestamp[0x100];
    a3da_get_time_stamp(a3da_timestamp, 0x100);

    stream s;
    io_mopen(&s, 0, 0);

    if (a3dc) {
        if (a->_compress_f16 != A3DA_COMPRESS_F32F32F32F32)
            io_write(&s, "#-compress_f16\n", 15);
    }
    else
        io_write(&s, "#A3DA__________\n", 16);

    io_write(&s, a3da_timestamp, utf8_length(a3da_timestamp));

    len = 1;
    memcpy(buf, "_", 1);
    off = len;

    if (a3dc && a->format > A3DA_FORMAT_AFT)
        key_val_write_int32_t(&s, buf, off, ".compress_f16", 14, a->_compress_f16);
    key_val_write_string(&s, buf, off, ".converter.version", 19, &a->_converter_version);
    key_val_write_string(&s, buf, off, ".file_name", 11, &a->_file_name);
    key_val_write_string(&s, buf, off, ".property.version", 18, &a->_property_version);

    vector_int32_t sort_index = vector_empty(int32_t);
    vector_int32_t sort_index1 = vector_empty(int32_t);
    if (a->ambient.end - a->ambient.begin && a->format == A3DA_FORMAT_MGF) {
        len = 7;
        memcpy(buf, "ambient", 7);
        off = len;

        vector_a3da_ambient* va = &a->ambient;
        count = (int32_t)(va->end - va->begin);
        key_val_get_lexicographic_order(&sort_index, count);
        for (int32_t i = 0; i < count; i++) {
            a3da_ambient* a = &va->begin[sort_index.begin[i]];

            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", sort_index.begin[i]);
            off = len + len1;

            if (a->flags & A3DA_AMBIENT_LIGHT_DIFFUSE)
                key_val_write_a3da_rgba(&s,
                    buf, off, ".light.Diffuse", 15, &a->light_diffuse);
            key_val_write_string(&s, buf, off, ".name", 6, &a->name);
            if (a->flags & A3DA_AMBIENT_RIM_LIGHT_DIFFUSE)
                key_val_write_a3da_rgba(&s,
                    buf, off, ".rimlight.Diffuse", 18, &a->rim_light_diffuse);
        }

        off = len;
        key_val_write_int32_t(&s, buf, off, ".length", 8, count);
    }

    if (a->auth_2d.end - a->auth_2d.begin) {
        len = 7;
        memcpy(buf, "auth_2d", 7);
        off = len;

        vector_string* va2 = &a->auth_2d;
        count = (int32_t)(va2->end - va2->begin);
        key_val_get_lexicographic_order(&sort_index, count);
        for (int32_t i = 0; i < count; i++) {
            string* a2 = &va2->begin[sort_index.begin[i]];

            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", sort_index.begin[i]);
            off = len + len1;

            key_val_write_string(&s, buf, off, ".name", 6, a2);
        }

        off = len;
        key_val_write_int32_t(&s, buf, off, ".length", 8, count);
    }

    if (a->camera_auxiliary.flags) {
        len = 16;
        memcpy(buf, "camera_auxiliary", 16);
        off = len;

        a3da_camera_auxiliary* ca = &a->camera_auxiliary;
        if (a->format == A3DA_FORMAT_F || a->format > A3DA_FORMAT_AFT) {
            if (ca->flags & A3DA_CAMERA_AUXILIARY_EXPOSURE_RATE)
                key_val_write_a3da_key(&s, buf, off, ".exposure_rate", 15, &ca->exposure_rate);
            if (ca->flags & A3DA_CAMERA_AUXILIARY_GAMMA_RATE)
                key_val_write_a3da_key(&s, buf, off, ".gamma_rate", 12, &ca->gamma_rate);
            if (ca->flags & A3DA_CAMERA_AUXILIARY_SATURATE)
                key_val_write_a3da_key(&s, buf, off, ".saturate", 10, &ca->saturate);
        }
        else {
            if (ca->flags & A3DA_CAMERA_AUXILIARY_AUTO_EXPOSURE)
                key_val_write_a3da_key(&s, buf, off, ".auto_exposure", 15, &ca->auto_exposure);
            if (ca->flags & A3DA_CAMERA_AUXILIARY_EXPOSURE)
                key_val_write_a3da_key(&s, buf, off, ".exposure", 10, &ca->exposure);
            if (ca->flags & A3DA_CAMERA_AUXILIARY_GAMMA)
                key_val_write_a3da_key(&s, buf, off, ".gamma", 7, &ca->gamma);
            if (ca->flags & A3DA_CAMERA_AUXILIARY_SATURATE)
                key_val_write_a3da_key(&s, buf, off, ".saturate", 10, &ca->saturate);
        }
    }

    if (a->camera_root.end - a->camera_root.begin) {
        len = 11;
        memcpy(buf, "camera_root", 11);
        off = len;

        vector_a3da_camera_root* vcr = &a->camera_root;
        count = (int32_t)(vcr->end - vcr->begin);
        key_val_get_lexicographic_order(&sort_index, count);
        for (int32_t i = 0; i < count; i++) {
            a3da_camera_root* cr = &vcr->begin[sort_index.begin[i]];

            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", sort_index.begin[i]);
            off = len + len1;

            key_val_write_a3da_model_transform(&s,
                buf, off, ".interest", 10, &cr->interest, 0x1F);
            key_val_write_a3da_model_transform(&s,
                buf, off, "", 1, &cr->model_transform, 0x1E);

            a3da_camera_root_view_point* vp = &cr->view_point;

            len2 = 11;
            memcpy(buf + len + len1, ".view_point", 11);
            off = len + len1 + len2;

            key_val_write_float_t(&s,
                buf, off, ".aspect", 8, vp->aspect);
            if (vp->flags & A3DA_CAMERA_ROOT_VIEW_POINT_FOV) {
                key_val_write_a3da_key(&s,
                    buf, off, ".fov", 5, &vp->fov);
                key_val_write_bool(&s,
                    buf, off, ".fov_is_horizontal", 19, vp->fov_is_horizontal);
            }
            else {
                key_val_write_float_t(&s,
                    buf, off, ".camera_aperture_h", 19, vp->camera_aperture_h);
                key_val_write_float_t(&s,
                    buf, off, ".camera_aperture_w", 19, vp->camera_aperture_w);
                key_val_write_a3da_key(&s,
                    buf, off, ".focal_length", 14, &vp->focal_length);
            }
            key_val_write_a3da_model_transform(&s,
                buf, off, "", 1, &vp->model_transform, 0x10);
            if (vp->flags & A3DA_CAMERA_ROOT_VIEW_POINT_ROLL)
                key_val_write_a3da_key(&s,
                    buf, off, ".roll", 6, &vp->roll);
            key_val_write_a3da_model_transform(&s,
                buf, off, "", 1, &vp->model_transform, 0x0F);

            off = len + len1;
            key_val_write_a3da_model_transform(&s,
                buf, off, "", 1, &cr->model_transform, 0x01);
        }

        off = len;
        key_val_write_int32_t(&s, buf, off, ".length", 8, count);
    }

    if (a->chara.end - a->chara.begin) {
        len = 5;
        memcpy(buf, "chara", 5);
        off = len;

        vector_a3da_chara* vc = &a->chara;
        count = (int32_t)(vc->end - vc->begin);
        key_val_get_lexicographic_order(&sort_index, count);
        for (int32_t i = 0; i < count; i++) {
            a3da_chara* c = &vc->begin[sort_index.begin[i]];

            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", sort_index.begin[i]);
            off = len + len1;

            key_val_write_string(&s, buf, off, ".name", 6, &c->name);
            key_val_write_a3da_model_transform(&s,
                buf, off, "", 1, &c->model_transform, 0x1F);
        }

        off = len;
        key_val_write_int32_t(&s, buf, off, ".length", 8, count);
    }

    if (a->curve.end - a->curve.begin) {
        len = 5;
        memcpy(buf, "curve", 5);
        off = len;

        vector_a3da_curve* vc = &a->curve;
        count = (int32_t)(vc->end - vc->begin);
        key_val_get_lexicographic_order(&sort_index, count);
        for (int32_t i = 0; i < count; i++) {
            a3da_curve* c = &vc->begin[sort_index.begin[i]];

            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", sort_index.begin[i]);
            off = len + len1;

            key_val_write_a3da_key(&s, buf, off, ".cv", 4, &c->curve);
            key_val_write_string(&s, buf, off, ".name", 6, &c->name);
        }

        off = len;
        key_val_write_int32_t(&s, buf, off, ".length", 8, count);
    }

    if (a->dof.has_dof && a->format == A3DA_FORMAT_AFT) {
        len = 3;
        memcpy(buf, "dof", 3);
        off = len;

        a3da_dof* d = &a->dof;

        key_val_write_string_ptr(&s, buf, off, ".name", 6, "DOF");
        key_val_write_a3da_model_transform(&s,
            buf, off, "", 1, &d->model_transform, 0x1F);
    }

    if (a->event.end - a->event.begin) {
        len = 5;
        memcpy(buf, "event", 5);
        off = len;

        vector_a3da_event* ve = &a->event;
        count = (int32_t)(ve->end - ve->begin);
        key_val_get_lexicographic_order(&sort_index, count);
        for (int32_t i = 0; i < count; i++) {
            a3da_event* e = &ve->begin[sort_index.begin[i]];

            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", sort_index.begin[i]);
            off = len + len1;

            key_val_write_float_t(&s, buf, off, ".begin", 7, e->begin);
            key_val_write_float_t(&s, buf, off, ".clip_begin", 12, e->clip_begin);
            key_val_write_float_t(&s, buf, off, ".clip_en", 9, e->clip_end);
            key_val_write_float_t(&s, buf, off, ".end", 5, e->end);
            key_val_write_string(&s, buf, off, ".name", 6, &e->name);
            key_val_write_string(&s, buf, off, ".param1", 8, &e->param_1);
            key_val_write_string(&s, buf, off, ".ref", 5, &e->ref);
            key_val_write_float_t(&s, buf, off, ".time_ref_scale", 16, e->time_ref_scale);
            key_val_write_int32_t(&s, buf, off, ".type", 6, e->type);
        }

        off = len;
        key_val_write_int32_t(&s, buf, off, ".length", 8, count);
    }

    if (a->fog.end - a->fog.begin) {
        len = 3;
        memcpy(buf, "fog", 3);
        off = len;

        vector_a3da_fog* vf = &a->fog;
        count = (int32_t)(vf->end - vf->begin);
        key_val_get_lexicographic_order(&sort_index, count);
        for (int32_t i = 0; i < count; i++) {
            a3da_fog* f = &vf->begin[sort_index.begin[i]];

            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", sort_index.begin[i]);
            off = len + len1;

            if (f->flags & A3DA_FOG_COLOR)
                key_val_write_a3da_rgba(&s, buf, off, ".Diffuse", 9, &f->color);
            if (f->flags & A3DA_FOG_DENSITY)
                key_val_write_a3da_key(&s, buf, off, ".density", 4, &f->density);
            if (f->flags & A3DA_FOG_END)
                key_val_write_a3da_key(&s, buf, off, ".end", 5, &f->end);
            key_val_write_int32_t(&s, buf, off, ".id", 4, f->id);
            if (f->flags & A3DA_FOG_START)
                key_val_write_a3da_key(&s, buf, off, ".start", 7, &f->start);
        }

        off = len;
        key_val_write_int32_t(&s, buf, off, ".length", 8, count);
    }

    if (a->light.end - a->light.begin) {
        len = 5;
        memcpy(buf, "light", 5);
        off = len;

        bool xhd = a->format == A3DA_FORMAT_XHD;

        vector_a3da_light* vl = &a->light;
        count = (int32_t)(vl->end - vl->begin);
        key_val_get_lexicographic_order(&sort_index, count);
        for (int32_t i = 0; i < count; i++) {
            a3da_light* l = &vl->begin[sort_index.begin[i]];

            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", sort_index.begin[i]);
            off = len + len1;

            if (l->flags & A3DA_LIGHT_AMBIENT)
                key_val_write_a3da_rgba(&s,
                    buf, off, ".Ambient", 9, &l->ambient);
            if (l->flags & A3DA_LIGHT_CONSTANT && xhd)
                key_val_write_a3da_key(&s,
                    buf, off, ".CONSTANT", 10, &l->constant);
            if (l->flags & A3DA_LIGHT_CONE_ANGLE && xhd)
                key_val_write_a3da_key(&s,
                    buf, off, ".ConeAngle", 11, &l->cone_angle);
            if (l->flags & A3DA_LIGHT_DIFFUSE)
                key_val_write_a3da_rgba(&s,
                    buf, off, ".Diffuse", 9, &l->diffuse);
            if (l->flags & A3DA_LIGHT_DROP_OFF && xhd)
                key_val_write_a3da_key(&s,
                    buf, off, ".DropOff", 9, &l->drop_off);
            if (l->flags & A3DA_LIGHT_FAR && xhd)
                key_val_write_a3da_key(&s,
                    buf, off, ".FAR", 5, &l->_far);
            if (l->flags & A3DA_LIGHT_TONE_CURVE)
                key_val_write_a3da_rgba(&s,
                    buf, off, ".Incandescence", 15, &l->tone_curve);
            if (l->flags & A3DA_LIGHT_INTENSITY && xhd)
                key_val_write_a3da_key(&s,
                    buf, off, ".Intensity", 9, &l->intensity);
            if (l->flags & A3DA_LIGHT_LINEAR && xhd)
                key_val_write_a3da_key(&s,
                    buf, off, ".LINEAR", 6, &l->linear);
            if (l->flags & A3DA_LIGHT_QUADRATIC && xhd)
                key_val_write_a3da_key(&s,
                    buf, off, ".QUADRATIC", 11, &l->quadratic);
            if (l->flags & A3DA_LIGHT_SPECULAR)
                key_val_write_a3da_rgba(&s,
                    buf, off, ".Specular", 10, &l->specular);
            key_val_write_int32_t(&s,
                buf, off, ".id", 4, l->id);
            char* name = "none";
            switch (l->id) {
            case LIGHT_CHARA:
                name = "Char";
                break;
            case LIGHT_STAGE:
                name = "Stage";
                break;
            case LIGHT_SUN:
                name = "Sun";
                break;
            case LIGHT_REFLECT:
                name = "Reflect";
                break;
            case LIGHT_CHARA_COLOR:
                name = "CharColor";
                break;
            case LIGHT_TONE_CURVE:
                name = "ToneCurve";
                break;
            }
            key_val_write_string_ptr(&s,
                buf, off, ".name", 6, name);
            if (l->flags & A3DA_LIGHT_POSITION)
                key_val_write_a3da_model_transform(&s,
                    buf, off, ".position", 9, &l->position, 0x1F);
            if (l->flags & A3DA_LIGHT_SPOT_DIRECTION)
                key_val_write_a3da_model_transform(&s,
                    buf, off, ".spot_direction", 9, &l->spot_direction, 0x1F);
            key_val_write_string(&s,
                buf, off, ".type", 6, &l->type);
        }

        off = len;
        key_val_write_int32_t(&s, buf, off, ".length", 8, count);
    }

    if (a->m_object_hrc.end - a->m_object_hrc.begin) {
        len = 8;
        memcpy(buf, "m_objhrc", 8);
        off = len;

        vector_a3da_m_object_hrc* vmoh = &a->m_object_hrc;
        count = (int32_t)(vmoh->end - vmoh->begin);
        key_val_get_lexicographic_order(&sort_index, count);
        for (int32_t i = 0; i < count; i++) {
            a3da_m_object_hrc* moh = &vmoh->begin[sort_index.begin[i]];

            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", sort_index.begin[i]);
            off = len + len1;

            len2 = 9;
            memcpy(buf + len + len1, ".instance", 9);
            off = len + len1 + len2;

            vector_a3da_object_instance* voi = &moh->instance;
            count1 = (int32_t)(voi->end - voi->begin);
            key_val_get_lexicographic_order(&sort_index1, count1);
            for (int32_t j = 0; j < count; j++) {
                a3da_object_instance* oi = &voi->begin[sort_index1.begin[i]];

                len3 = sprintf_s(buf + len + len1 + len2,
                    A3DA_TEXT_BUF_SIZE - len - len1 - len2, ".%d", sort_index.begin[j]);
                off = len + len1 + len2 + len3;

                key_val_write_a3da_model_transform(&s,
                    buf, off, "", 1, &oi->model_transform, 0x10);
                key_val_write_string(&s,
                    buf, off, ".name", 6, &oi->name);
                key_val_write_a3da_model_transform(&s,
                    buf, off, "", 1, &oi->model_transform, 0x0C);
                key_val_write_bool(&s,
                    buf, off, ".shadow", 6, &oi->shadow);
                key_val_write_a3da_model_transform(&s,
                    buf, off, "", 1, &oi->model_transform, 0x02);
                key_val_write_string(&s,
                    buf, off, ".uid_name", 10, &oi->uid_name);
                key_val_write_a3da_model_transform(&s,
                    buf, off, "", 1, &oi->model_transform, 0x01);
            }

            off = len + len1 + len2;
            key_val_write_int32_t(&s, buf, off, ".length", 8, count1);

            off = len + len1;
            key_val_write_a3da_model_transform(&s,
                buf, off, "", 1, &moh->model_transform, 0x10);
            key_val_write_string(&s,
                buf, off, ".name", 6, &moh->name);

            len2 = 5;
            memcpy(buf + len + len1, ".node", 5);
            off = len + len1 + len2;

            vector_a3da_object_node* vmohn = &moh->node;
            count1 = (int32_t)(vmohn->end - vmohn->begin);
            key_val_get_lexicographic_order(&sort_index1, count1);
            for (int32_t j = 0; j < count; j++) {
                a3da_object_node* mohn = &vmohn->begin[sort_index1.begin[i]];

                len3 = sprintf_s(buf + len + len1 + len2,
                    A3DA_TEXT_BUF_SIZE - len - len1 - len2, ".%d", sort_index.begin[j]);
                off = len + len1 + len2 + len3;

                key_val_write_a3da_model_transform(&s,
                    buf, off, "", 1, &mohn->model_transform, 0x10);
                if (mohn->flags & A3DA_OBJECT_NODE_JOINT_ORIENT)
                    key_val_write_vec3(&s,
                        buf, off, ".joint_orient", 14, &mohn->joint_orient);
                key_val_write_string(&s,
                    buf, off, ".name", 6, &mohn->name);
                key_val_write_int32_t(&s,
                    buf, off, ".parent", 8, mohn->parent);
                key_val_write_a3da_model_transform(&s,
                    buf, off, "", 1, &mohn->model_transform, 0x0F);
            }

            off = len + len1 + len2;
            key_val_write_int32_t(&s, buf, off, ".length", 8, count1);

            off = len + len1;
            key_val_write_a3da_model_transform(&s,
                buf, off, "", 1, &moh->model_transform, 0x0F);
        }

        off = len;
        key_val_write_int32_t(&s, buf, off, ".length", 8, count);
    }

    if (a->m_object_hrc_list.end - a->m_object_hrc_list.begin) {
        len = 13;
        memcpy(buf, "m_objhrc_list", 13);
        off = len;

        vector_string* vmohl = &a->m_object_hrc_list;
        count = (int32_t)(vmohl->end - vmohl->begin);
        key_val_get_lexicographic_order(&sort_index, count);
        for (int32_t i = 0; i < count; i++) {
            string* mohl = &vmohl->begin[sort_index.begin[i]];

            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", sort_index.begin[i]);
            off = len + len1;

            key_val_write_string(&s, buf, off, "", 1, mohl);
        }

        off = len;
        key_val_write_int32_t(&s, buf, off, ".length", 8, count);
    }

    if (a->material_list.end - a->material_list.begin
        && (a->format == A3DA_FORMAT_X || a->format == A3DA_FORMAT_XHD)) {
        len = 13;
        memcpy(buf, "material_list", 13);
        off = len;

        vector_a3da_material_list* vml = &a->material_list;
        count = (int32_t)(vml->end - vml->begin);
        key_val_get_lexicographic_order(&sort_index, count);
        for (int32_t i = 0; i < count; i++) {
            a3da_material_list* ml = &vml->begin[sort_index.begin[i]];

            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", sort_index.begin[i]);
            off = len + len1;

            if (ml->flags & A3DA_MATERIAL_LIST_BLEND_COLOR)
                key_val_write_a3da_rgba(&s, buf, off, ".blend_color", 13, &ml->blend_color);
            if (ml->flags & A3DA_MATERIAL_LIST_BLEND_COLOR)
                key_val_write_a3da_key(&s, buf, off, ".glow_intensity", 16, &ml->glow_intensity);
            key_val_write_uint32_t(&s, buf, off, ".hash_name", 11,
                hash_murmurhash(string_data(&ml->name), ml->name.length, 0, false, false));
            if (ml->flags & A3DA_MATERIAL_LIST_BLEND_COLOR)
                key_val_write_a3da_rgba(&s, buf, off, ".incandescence", 15, &ml->incandescence);
            key_val_write_string(&s, buf, off, ".name", 6, &ml->name);
        }

        off = len;
        key_val_write_int32_t(&s, buf, off, ".length", 8, count);
    }

    if (a->motion.end - a->motion.begin) {
        len = 6;
        memcpy(buf, "motion", 6);
        off = len;

        vector_string* vm = &a->motion;
        count = (int32_t)(vm->end - vm->begin);
        key_val_get_lexicographic_order(&sort_index, count);
        for (int32_t i = 0; i < count; i++) {
            string* m = &vm->begin[sort_index.begin[i]];

            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", sort_index.begin[i]);
            off = len + len1;

            key_val_write_string(&s, buf, off, "", 1, m);
        }

        off = len;
        key_val_write_int32_t(&s, buf, off, ".length", 8, count);
    }

    if (a->object.end - a->object.begin) {
        len = 6;
        memcpy(buf, "object", 6);
        off = len;

        vector_a3da_object* vo = &a->object;
        count = (int32_t)(vo->end - vo->begin);
        key_val_get_lexicographic_order(&sort_index, count);
        for (int32_t i = 0; i < count; i++) {
            a3da_object* o = &vo->begin[sort_index.begin[i]];

            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", sort_index.begin[i]);
            off = len + len1;

            key_val_write_a3da_model_transform(&s,
                buf, off, "", 1, &o->model_transform, 0x10);
            if (string_data(&o->morph)) {
                key_val_write_string(&s,
                    buf, off, ".morph", 7, &o->morph);
                key_val_write_float_t(&s,
                    buf, off, ".morph_offset", 14, o->morph_offset);
            }
            key_val_write_string(&s,
                buf, off, ".name", 6, &o->name);
            key_val_write_string(&s,
                buf, off, ".parent_name", 13, &o->parent_name);
            key_val_write_string(&s,
                buf, off, ".parent_node", 13, &o->parent_node);
            if (string_data(&o->pattern)) {
                key_val_write_string(&s,
                    buf, off, ".pat", 5, &o->pattern);
                key_val_write_float_t(&s,
                    buf, off, ".pat_offset", 12, o->pattern_offset);
            }
            key_val_write_a3da_model_transform(&s,
                buf, off, "", 1, &o->model_transform, 0x0C);

            len2 = 8;
            memcpy(buf + len + len1, ".tex_pat", 8);
            off = len + len1 + len2;

            vector_a3da_object_texture_pattern* votp = &o->texture_pattern;
            count1 = (int32_t)(votp->end - votp->begin);
            key_val_get_lexicographic_order(&sort_index1, count1);
            for (int32_t j = 0; j < count; j++) {
                a3da_object_texture_pattern* otp = &votp->begin[sort_index1.begin[i]];

                len3 = sprintf_s(buf + len + len1 + len2,
                    A3DA_TEXT_BUF_SIZE - len - len1 - len2, ".%d", sort_index.begin[j]);
                off = len + len1 + len2 + len3;

                key_val_write_string(&s,
                    buf, off, ".name", 6, &otp->name);
                if (string_data(&otp->pattern)) {
                    key_val_write_string(&s,
                        buf, off, ".pat", 5, &otp->pattern);
                    key_val_write_float_t(&s,
                        buf, off, ".pat_offset", 12, otp->pattern_offset);
                }
            }

            off = len + len1 + len2;
            key_val_write_int32_t(&s, buf, off, ".length", 8, count1);

            len2 = 14;
            memcpy(buf + len + len1, ".tex_transform", 14);
            off = len + len1 + len2;

            vector_a3da_object_texture_transform* vott = &o->texture_transform;
            count1 = (int32_t)(vott->end - vott->begin);
            key_val_get_lexicographic_order(&sort_index1, count1);
            for (int32_t j = 0; j < count; j++) {
                a3da_object_texture_transform* ott = &vott->begin[sort_index1.begin[i]];

                len3 = sprintf_s(buf + len + len1 + len2,
                    A3DA_TEXT_BUF_SIZE - len - len1 - len2, ".%d", sort_index.begin[j]);
                off = len + len1 + len2 + len3;

                if (ott->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_COVERAGE_U)
                    key_val_write_a3da_key(&s,
                        buf, off, ".coverageU", 11, &ott->coverage_u);
                if (ott->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_COVERAGE_V)
                    key_val_write_a3da_key(&s,
                        buf, off, ".coverageV", 11, &ott->coverage_v);
                key_val_write_string(&s,
                    buf, off, ".name", 6, &ott->name);
                if (ott->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_OFFSET_U)
                    key_val_write_a3da_key(&s,
                        buf, off, ".offsetU", 9, &ott->offset_u);
                if (ott->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_OFFSET_V)
                    key_val_write_a3da_key(&s,
                        buf, off, ".offsetV", 9, &ott->offset_v);
                if (ott->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_REPEAT_U)
                    key_val_write_a3da_key(&s,
                        buf, off, ".repeatU", 9, &ott->repeat_u);
                if (ott->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_REPEAT_V)
                    key_val_write_a3da_key(&s,
                        buf, off, ".repeatV", 9, &ott->repeat_v);
                if (ott->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_ROTATE)
                    key_val_write_a3da_key(&s,
                        buf, off, ".rotate", 8, &ott->rotate);
                if (ott->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_ROTATE_FRAME)
                    key_val_write_a3da_key(&s,
                        buf, off, ".rotateFrame", 13, &ott->rotate_frame);
                if (ott->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_TRANSLATE_FRAME_U)
                    key_val_write_a3da_key(&s,
                        buf, off, ".translateFrameU", 17, &ott->translate_frame_u);
                if (ott->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_TRANSLATE_FRAME_V)
                    key_val_write_a3da_key(&s,
                        buf, off, ".translateFrameV", 17, &ott->translate_frame_v);
            }

            off = len + len1 + len2;
            key_val_write_int32_t(&s, buf, off, ".length", 8, count1);

            off = len + len1;
            key_val_write_a3da_model_transform(&s,
                buf, off, "", 1, &o->model_transform, 0x02);
            key_val_write_string(&s,
                buf, off, ".uid_name", 10, &o->uid_name);
            key_val_write_a3da_model_transform(&s,
                buf, off, "", 1, &o->model_transform, 0x01);
        }

        off = len;
        key_val_write_int32_t(&s, buf, off, ".length", 8, count);
    }

    if (a->object_list.end - a->object_list.begin) {
        len = 11;
        memcpy(buf, "object_list", 11);
        off = len;

        vector_string* vol = &a->object_list;
        count = (int32_t)(vol->end - vol->begin);
        key_val_get_lexicographic_order(&sort_index, count);
        for (int32_t i = 0; i < count; i++) {
            string* ol = &vol->begin[sort_index.begin[i]];

            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", sort_index.begin[i]);
            off = len + len1;

            key_val_write_string(&s, buf, off, "", 1, ol);
        }

        off = len;
        key_val_write_int32_t(&s, buf, off, ".length", 8, count);
    }

    if (a->object_hrc.end - a->object_hrc.begin) {
        len = 6;
        memcpy(buf, "objhrc", 6);
        off = len;

        vector_a3da_object_hrc* voh = &a->object_hrc;
        count = (int32_t)(voh->end - voh->begin);
        key_val_get_lexicographic_order(&sort_index, count);
        for (int32_t i = 0; i < count; i++) {
            a3da_object_hrc* oh = &voh->begin[sort_index.begin[i]];

            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", sort_index.begin[i]);
            off = len + len1;

            key_val_write_string(&s,
                buf, off, ".name", 6, &oh->name);

            len2 = 5;
            memcpy(buf + len + len1, ".node", 5);
            off = len + len1 + len2;

            vector_a3da_object_node* vohn = &oh->node;
            count1 = (int32_t)(vohn->end - vohn->begin);
            key_val_get_lexicographic_order(&sort_index1, count1);
            for (int32_t j = 0; j < count; j++) {
                a3da_object_node* ohn = &vohn->begin[sort_index1.begin[i]];

                len3 = sprintf_s(buf + len + len1 + len2,
                    A3DA_TEXT_BUF_SIZE - len - len1 - len2, ".%d", sort_index.begin[j]);
                off = len + len1 + len2 + len3;

                key_val_write_a3da_model_transform(&s,
                    buf, off, "", 1, &ohn->model_transform, 0x10);
                if (ohn->flags & A3DA_OBJECT_NODE_JOINT_ORIENT)
                    key_val_write_vec3(&s,
                        buf, off, ".joint_orient", 14, &ohn->joint_orient);
                key_val_write_string(&s,
                    buf, off, ".name", 6, &ohn->name);
                key_val_write_int32_t(&s,
                    buf, off, ".parent", 8, ohn->parent);
                key_val_write_a3da_model_transform(&s,
                    buf, off, "", 1, &ohn->model_transform, 0x0F);
            }

            off = len + len1 + len2;
            key_val_write_int32_t(&s, buf, off, ".length", 8, count1);

            off = len + len1;
            key_val_write_string(&s,
                buf, off, ".parent_name", 13, &oh->parent_name);
            key_val_write_string(&s,
                buf, off, ".parent_node", 13, &oh->parent_node);
            key_val_write_bool(&s,
                buf, off, ".shadow", 6, &oh->shadow);
            key_val_write_string(&s,
                buf, off, ".uid_name", 10, &oh->uid_name);
        }

        off = len;
        key_val_write_int32_t(&s, buf, off, ".length", 8, count);
    }

    if (a->object_hrc_list.end - a->object_hrc_list.begin) {
        len = 11;
        memcpy(buf, "objhrc_list", 11);
        off = len;

        vector_string* vohl = &a->object_hrc_list;
        count = (int32_t)(vohl->end - vohl->begin);
        key_val_get_lexicographic_order(&sort_index, count);
        for (int32_t i = 0; i < count; i++) {
            string* ohl = &vohl->begin[sort_index.begin[i]];

            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", sort_index.begin[i]);
            off = len + len1;

            key_val_write_string(&s, buf, off, "", 1, ohl);
        }

        off = len;
        key_val_write_int32_t(&s, buf, off, ".length", 8, count);
    }

    {
        a3da_play_control* pc = &a->play_control;

        len = 12;
        memcpy(buf, "play_control", 12);
        off = len;

        key_val_write_float_t(&s,
            buf, off, ".begin", 6, pc->begin);
        if (pc->flags & A3DA_PLAY_CONTROL_DIV && a->format > A3DA_FORMAT_AFT)
            key_val_write_int32_t(&s,
                buf, off, ".div", 5, pc->div);
        key_val_write_float_t(&s,
            buf, off, ".fps", 5, pc->fps);
        if (pc->flags & A3DA_PLAY_CONTROL_OFFSET) {
            if (a->format > A3DA_FORMAT_AFT) {
                key_val_write_float_t(&s,
                    buf, off, ".offset", 8, pc->offset);
                key_val_write_float_t(&s,
                    buf, off, ".size", 6, pc->size);
            }
            else
                key_val_write_float_t(&s,
                    buf, off, ".size", 6, pc->size + pc->offset);
        }
        else
            key_val_write_float_t(&s,
                buf, off, ".size", 6, pc->size);
    }

    if (a->post_process.flags) {
        len = 12;
        memcpy(buf, "post_process", 12);
        off = len;

        a3da_post_process* pp = &a->post_process;
        if (a->post_process.flags & A3DA_POST_PROCESS_RADIUS)
            key_val_write_a3da_rgba(&s, buf, off, ".Ambient", 9, &pp->radius);
        if (a->post_process.flags & A3DA_POST_PROCESS_INTENSITY)
            key_val_write_a3da_rgba(&s, buf, off, ".Diffuse", 9, &pp->intensity);
        if (a->post_process.flags & A3DA_POST_PROCESS_SCENE_FADE)
            key_val_write_a3da_rgba(&s, buf, off, ".Specular", 10, &pp->scene_fade);
        if (a->post_process.flags & A3DA_POST_PROCESS_LENS_FLARE)
            key_val_write_a3da_key(&s, buf, off, ".lens_flare", 12, &pp->lens_flare);
        if (a->post_process.flags & A3DA_POST_PROCESS_LENS_GHOST)
            key_val_write_a3da_key(&s, buf, off, ".lens_ghost", 12, &pp->lens_ghost);
        if (a->post_process.flags & A3DA_POST_PROCESS_LENS_SHAFT)
            key_val_write_a3da_key(&s, buf, off, ".lens_shaft", 12, &pp->lens_shaft);
    }

    if (a->point.end - a->point.begin) {
        len = 5;
        memcpy(buf, "point", 5);
        off = len;

        vector_a3da_point* vp = &a->point;
        count = (int32_t)(vp->end - vp->begin);
        key_val_get_lexicographic_order(&sort_index, count);
        for (int32_t i = 0; i < count; i++) {
            a3da_point* p = &vp->begin[sort_index.begin[i]];

            len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", sort_index.begin[i]);
            off = len + len1;

            key_val_write_string(&s, buf, off, ".name", 6, &p->name);
            key_val_write_a3da_model_transform(&s,
                buf, off, "", 1, &p->model_transform, 0x1F);
        }

        off = len;
        key_val_write_int32_t(&s, buf, off, ".length", 8, count);
    }

    vector_int32_t_free(&sort_index1, 0);
    vector_int32_t_free(&sort_index, 0);

    io_mcopy(&s, data, length);
    io_free(&s);
}

static void a3da_read_data(a3da* a, void* data, size_t length) {
    for (a3da_ambient* i = a->ambient.begin; i != a->ambient.end; i++) {
        if (i->flags & A3DA_AMBIENT_LIGHT_DIFFUSE)
            a3dc_read_a3da_rgba(data, length, &i->light_diffuse);
        if (i->flags & A3DA_AMBIENT_RIM_LIGHT_DIFFUSE)
            a3dc_read_a3da_rgba(data, length, &i->rim_light_diffuse);
    }

    if (a->camera_auxiliary.flags) {
        a3da_camera_auxiliary* ca = &a->camera_auxiliary;

        if (ca->flags & A3DA_CAMERA_AUXILIARY_AUTO_EXPOSURE)
            a3dc_read_a3da_key(data, length, &ca->auto_exposure);
        if (ca->flags & A3DA_CAMERA_AUXILIARY_EXPOSURE)
            a3dc_read_a3da_key(data, length, &ca->exposure);
        if (ca->flags & A3DA_CAMERA_AUXILIARY_EXPOSURE_RATE)
            a3dc_read_a3da_key(data, length, &ca->exposure_rate);
        if (ca->flags & A3DA_CAMERA_AUXILIARY_GAMMA)
            a3dc_read_a3da_key(data, length, &ca->gamma);
        if (ca->flags & A3DA_CAMERA_AUXILIARY_GAMMA_RATE)
            a3dc_read_a3da_key(data, length, &ca->gamma_rate);
        if (ca->flags & A3DA_CAMERA_AUXILIARY_SATURATE)
            a3dc_read_a3da_key(data, length, &ca->saturate);
    }

    for (a3da_camera_root* i = a->camera_root.begin; i != a->camera_root.end; i++) {
        a3dc_read_a3da_model_transform(data, length, &i->interest, a->_compress_f16);
        a3dc_read_a3da_model_transform(data, length, &i->model_transform, a->_compress_f16);

        a3da_camera_root_view_point* vp = &i->view_point;

        if (vp->flags & A3DA_CAMERA_ROOT_VIEW_POINT_FOV)
            a3dc_read_a3da_key(data, length, &vp->fov);
        else
            a3dc_read_a3da_key(data, length, &vp->focal_length);
        a3dc_read_a3da_model_transform(data, length, &vp->model_transform, a->_compress_f16);
        if (vp->flags & A3DA_CAMERA_ROOT_VIEW_POINT_ROLL)
            a3dc_read_a3da_key(data, length, &vp->roll);
    }

    for (a3da_chara* i = a->chara.begin; i != a->chara.end; i++)
        a3dc_read_a3da_model_transform(data, length, &i->model_transform, a->_compress_f16);

    for (a3da_curve* i = a->curve.begin; i != a->curve.end; i++)
        a3dc_read_a3da_key(data, length, &i->curve);

    if (a->dof.has_dof) {
        a3da_dof* d = &a->dof;

        a3dc_read_a3da_model_transform(data, length, &d->model_transform, a->_compress_f16);
    }

    for (a3da_fog* i = a->fog.begin; i != a->fog.end; i++) {
        if (i->flags & A3DA_FOG_COLOR)
            a3dc_read_a3da_rgba(data, length, &i->color);
        if (i->flags & A3DA_FOG_DENSITY)
            a3dc_read_a3da_key(data, length, &i->density);
        if (i->flags & A3DA_FOG_END)
            a3dc_read_a3da_key(data, length, &i->end);
        if (i->flags & A3DA_FOG_START)
            a3dc_read_a3da_key(data, length, &i->start);
    }

    for (a3da_light* i = a->light.begin; i != a->light.end; i++) {
        if (i->flags & A3DA_LIGHT_AMBIENT)
            a3dc_read_a3da_rgba(data, length, &i->ambient);
        if (i->flags & A3DA_LIGHT_CONE_ANGLE)
            a3dc_read_a3da_key(data, length, &i->cone_angle);
        if (i->flags & A3DA_LIGHT_CONSTANT)
            a3dc_read_a3da_key(data, length, &i->constant);
        if (i->flags & A3DA_LIGHT_DIFFUSE)
            a3dc_read_a3da_rgba(data, length, &i->diffuse);
        if (i->flags & A3DA_LIGHT_DROP_OFF)
            a3dc_read_a3da_key(data, length, &i->drop_off);
        if (i->flags & A3DA_LIGHT_FAR)
            a3dc_read_a3da_key(data, length, &i->_far);
        if (i->flags & A3DA_LIGHT_INTENSITY)
            a3dc_read_a3da_key(data, length, &i->intensity);
        if (i->flags & A3DA_LIGHT_LINEAR)
            a3dc_read_a3da_key(data, length, &i->linear);
        if (i->flags & A3DA_LIGHT_POSITION)
            a3dc_read_a3da_model_transform(data, length, &i->position, a->_compress_f16);
        if (i->flags & A3DA_LIGHT_QUADRATIC)
            a3dc_read_a3da_key(data, length, &i->quadratic);
        if (i->flags & A3DA_LIGHT_SPECULAR)
            a3dc_read_a3da_rgba(data, length, &i->specular);
        if (i->flags & A3DA_LIGHT_SPOT_DIRECTION)
            a3dc_read_a3da_model_transform(data, length, &i->spot_direction, a->_compress_f16);
        if (i->flags & A3DA_LIGHT_TONE_CURVE)
            a3dc_read_a3da_rgba(data, length, &i->tone_curve);
    }

    for (a3da_m_object_hrc* i = a->m_object_hrc.begin; i != a->m_object_hrc.end; i++) {
        for (a3da_object_instance* j = i->instance.begin; j != i->instance.end; j++)
            a3dc_read_a3da_model_transform(data, length, &j->model_transform, a->_compress_f16);

        a3dc_read_a3da_model_transform(data, length, &i->model_transform, a->_compress_f16);

        for (a3da_object_node* j = i->node.begin; j != i->node.end; j++)
            a3dc_read_a3da_model_transform(data, length, &j->model_transform, a->_compress_f16);
    }

    for (a3da_material_list* i = a->material_list.begin; i != a->material_list.end; i++) {
        if (i->flags & A3DA_MATERIAL_LIST_BLEND_COLOR)
            a3dc_read_a3da_rgba(data, length, &i->blend_color);
        if (i->flags & A3DA_MATERIAL_LIST_GLOW_INTENSITY)
            a3dc_read_a3da_key(data, length, &i->glow_intensity);
        if (i->flags & A3DA_MATERIAL_LIST_INCANDESCENCE)
            a3dc_read_a3da_rgba(data, length, &i->incandescence);
    }

    for (a3da_object* i = a->object.begin; i != a->object.end; i++) {
        a3dc_read_a3da_model_transform(data, length, &i->model_transform, a->_compress_f16);

        for (a3da_object_texture_transform* j = i->texture_transform.begin;
            j != i->texture_transform.end; j++) {
            if (j->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_COVERAGE_U)
                a3dc_read_a3da_key(data, length, &j->coverage_u);
            if (j->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_COVERAGE_V)
                a3dc_read_a3da_key(data, length, &j->coverage_v);
            if (j->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_OFFSET_U)
                a3dc_read_a3da_key(data, length, &j->offset_u);
            if (j->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_OFFSET_V)
                a3dc_read_a3da_key(data, length, &j->offset_v);
            if (j->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_REPEAT_U)
                a3dc_read_a3da_key(data, length, &j->repeat_u);
            if (j->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_REPEAT_V)
                a3dc_read_a3da_key(data, length, &j->repeat_v);
            if (j->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_ROTATE)
                a3dc_read_a3da_key(data, length, &j->rotate);
            if (j->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_ROTATE_FRAME)
                a3dc_read_a3da_key(data, length, &j->rotate_frame);
            if (j->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_TRANSLATE_FRAME_U)
                a3dc_read_a3da_key(data, length, &j->translate_frame_u);
            if (j->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_TRANSLATE_FRAME_V)
                a3dc_read_a3da_key(data, length, &j->translate_frame_v);
        }
    }

    for (a3da_object_hrc* i = a->object_hrc.begin; i != a->object_hrc.end; i++)
        for (a3da_object_node* j = i->node.begin; j != i->node.end; j++)
            a3dc_read_a3da_model_transform(data, length, &j->model_transform, a->_compress_f16);

    for (a3da_point* i = a->point.begin; i != a->point.end; i++)
        a3dc_read_a3da_model_transform(data, length, &i->model_transform, a->_compress_f16);

    if (a->post_process.flags) {
        a3da_post_process* pp = &a->post_process;

        if (pp->flags & A3DA_POST_PROCESS_INTENSITY)
            a3dc_read_a3da_rgba(data, length, &pp->intensity);
        if (pp->flags & A3DA_POST_PROCESS_LENS_FLARE)
            a3dc_read_a3da_key(data, length, &pp->lens_flare);
        if (pp->flags & A3DA_POST_PROCESS_LENS_GHOST)
            a3dc_read_a3da_key(data, length, &pp->lens_ghost);
        if (pp->flags & A3DA_POST_PROCESS_LENS_SHAFT)
            a3dc_read_a3da_key(data, length, &pp->lens_shaft);
        if (pp->flags & A3DA_POST_PROCESS_RADIUS)
            a3dc_read_a3da_rgba(data, length, &pp->radius);
        if (pp->flags & A3DA_POST_PROCESS_SCENE_FADE)
            a3dc_read_a3da_rgba(data, length, &pp->scene_fade);
    }
}

static void a3da_write_data(a3da* a, void** data, size_t* length) {
    stream s;
    io_mopen(&s, 0, 0);

    for (a3da_camera_root* i = a->camera_root.begin; i != a->camera_root.end; i++) {
        a3da_camera_root_view_point* vp = &i->view_point;
        a3dc_write_a3da_model_transform_offset(&s, &i->model_transform);
        a3dc_write_a3da_model_transform_offset(&s, &vp->model_transform);
        a3dc_write_a3da_model_transform_offset(&s, &i->interest);
    }

    for (a3da_chara* i = a->chara.begin; i != a->chara.end; i++)
        a3dc_write_a3da_model_transform_offset(&s, &i->model_transform);

    if (a->dof.has_dof) {
        a3da_dof* d = &a->dof;

        a3dc_write_a3da_model_transform_offset(&s, &d->model_transform);
    }

    for (a3da_light* i = a->light.begin; i != a->light.end; i++) {
        if (i->flags & A3DA_LIGHT_POSITION)
            a3dc_write_a3da_model_transform_offset(&s, &i->position);
        if (i->flags & A3DA_LIGHT_SPOT_DIRECTION)
            a3dc_write_a3da_model_transform_offset(&s, &i->spot_direction);
    }

    for (a3da_m_object_hrc* i = a->m_object_hrc.begin; i != a->m_object_hrc.end; i++) {
        a3dc_write_a3da_model_transform_offset(&s, &i->model_transform);

        for (a3da_object_node* j = i->node.begin; j != i->node.end; j++)
            a3dc_write_a3da_model_transform_offset(&s, &j->model_transform);

        for (a3da_object_instance* j = i->instance.begin; j != i->instance.end; j++)
            a3dc_write_a3da_model_transform_offset(&s, &j->model_transform);
    }

    for (a3da_object* i = a->object.begin; i != a->object.end; i++)
        a3dc_write_a3da_model_transform_offset(&s, &i->model_transform);

    for (a3da_object_hrc* i = a->object_hrc.begin; i != a->object_hrc.end; i++)
        for (a3da_object_node* j = i->node.begin; j != i->node.end; j++)
            a3dc_write_a3da_model_transform_offset(&s, &j->model_transform);

    for (a3da_point* i = a->point.begin; i != a->point.end; i++)
        a3dc_write_a3da_model_transform_offset(&s, &i->model_transform);

    for (a3da_ambient* i = a->ambient.begin; i != a->ambient.end; i++) {
        if (i->flags & A3DA_AMBIENT_LIGHT_DIFFUSE)
            a3dc_write_a3da_rgba(&s, &i->light_diffuse);
        if (i->flags & A3DA_AMBIENT_RIM_LIGHT_DIFFUSE)
            a3dc_write_a3da_rgba(&s, &i->rim_light_diffuse);
    }

    if (a->camera_auxiliary.flags) {
        a3da_camera_auxiliary* ca = &a->camera_auxiliary;

        if (ca->flags & A3DA_CAMERA_AUXILIARY_AUTO_EXPOSURE)
            a3dc_write_a3da_key(&s, &ca->auto_exposure);
        if (ca->flags & A3DA_CAMERA_AUXILIARY_EXPOSURE)
            a3dc_write_a3da_key(&s, &ca->exposure);
        if (ca->flags & A3DA_CAMERA_AUXILIARY_EXPOSURE_RATE)
            a3dc_write_a3da_key(&s, &ca->exposure_rate);
        if (ca->flags & A3DA_CAMERA_AUXILIARY_GAMMA)
            a3dc_write_a3da_key(&s, &ca->gamma);
        if (ca->flags & A3DA_CAMERA_AUXILIARY_GAMMA_RATE)
            a3dc_write_a3da_key(&s, &ca->gamma_rate);
        if (ca->flags & A3DA_CAMERA_AUXILIARY_SATURATE)
            a3dc_write_a3da_key(&s, &ca->saturate);
    }

    for (a3da_camera_root* i = a->camera_root.begin; i != a->camera_root.end; i++) {
        a3dc_write_a3da_model_transform(&s, &i->model_transform, a->_compress_f16);

        a3da_camera_root_view_point* vp = &i->view_point;

        a3dc_write_a3da_model_transform(&s, &vp->model_transform, a->_compress_f16);
        if (vp->flags & A3DA_CAMERA_ROOT_VIEW_POINT_ROLL)
            a3dc_write_a3da_key(&s, &vp->roll);
        if (vp->flags & A3DA_CAMERA_ROOT_VIEW_POINT_FOV)
            a3dc_write_a3da_key(&s, &vp->fov);
        else
            a3dc_write_a3da_key(&s, &vp->focal_length);
        a3dc_write_a3da_model_transform(&s, &i->interest, a->_compress_f16);
    }

    for (a3da_chara* i = a->chara.begin; i != a->chara.end; i++)
        a3dc_write_a3da_model_transform(&s, &i->model_transform, a->_compress_f16);

    for (a3da_curve* i = a->curve.begin; i != a->curve.end; i++)
        a3dc_write_a3da_key(&s, &i->curve);

    if (a->dof.has_dof) {
        a3da_dof* d = &a->dof;

        a3dc_write_a3da_model_transform(&s, &d->model_transform, a->_compress_f16);
    }

    for (a3da_light* i = a->light.begin; i != a->light.end; i++) {
        if (i->flags & A3DA_LIGHT_POSITION)
            a3dc_write_a3da_model_transform(&s, &i->position, a->_compress_f16);
        if (i->flags & A3DA_LIGHT_SPOT_DIRECTION)
            a3dc_write_a3da_model_transform(&s, &i->spot_direction, a->_compress_f16);
        if (i->flags & A3DA_LIGHT_AMBIENT)
            a3dc_write_a3da_rgba(&s, &i->ambient);
        if (i->flags & A3DA_LIGHT_DIFFUSE)
            a3dc_write_a3da_rgba(&s, &i->diffuse);
        if (i->flags & A3DA_LIGHT_SPECULAR)
            a3dc_write_a3da_rgba(&s, &i->specular);
        if (i->flags & A3DA_LIGHT_TONE_CURVE)
            a3dc_write_a3da_rgba(&s, &i->tone_curve);
        if (a->format == A3DA_FORMAT_XHD) {
            if (i->flags & A3DA_LIGHT_INTENSITY)
                a3dc_write_a3da_key(&s, &i->intensity);
            if (i->flags & A3DA_LIGHT_FAR)
                a3dc_write_a3da_key(&s, &i->_far);
            if (i->flags & A3DA_LIGHT_CONSTANT)
                a3dc_write_a3da_key(&s, &i->constant);
            if (i->flags & A3DA_LIGHT_LINEAR)
                a3dc_write_a3da_key(&s, &i->linear);
            if (i->flags & A3DA_LIGHT_QUADRATIC)
                a3dc_write_a3da_key(&s, &i->quadratic);
            if (i->flags & A3DA_LIGHT_DROP_OFF)
                a3dc_write_a3da_key(&s, &i->drop_off);
            if (i->flags & A3DA_LIGHT_CONE_ANGLE)
                a3dc_write_a3da_key(&s, &i->cone_angle);
        }
    }

    for (a3da_fog* i = a->fog.begin; i != a->fog.end; i++) {
        if (i->flags & A3DA_FOG_DENSITY)
            a3dc_write_a3da_key(&s, &i->density);
        if (i->flags & A3DA_FOG_END)
            a3dc_write_a3da_key(&s, &i->end);
        if (i->flags & A3DA_FOG_START)
            a3dc_write_a3da_key(&s, &i->start);
        if (i->flags & A3DA_FOG_COLOR)
            a3dc_write_a3da_rgba(&s, &i->color);
    }

    for (a3da_m_object_hrc* i = a->m_object_hrc.begin; i != a->m_object_hrc.end; i++) {
        a3dc_write_a3da_model_transform(&s, &i->model_transform, a->_compress_f16);

        for (a3da_object_node* j = i->node.begin; j != i->node.end; j++)
            a3dc_write_a3da_model_transform(&s, &j->model_transform, a->_compress_f16);

        for (a3da_object_instance* j = i->instance.begin; j != i->instance.end; j++)
            a3dc_write_a3da_model_transform(&s, &j->model_transform, a->_compress_f16);
    }

    for (a3da_material_list* i = a->material_list.begin; i != a->material_list.end; i++) {
        if (i->flags & A3DA_MATERIAL_LIST_GLOW_INTENSITY)
            a3dc_write_a3da_key(&s, &i->glow_intensity);
        if (i->flags & A3DA_MATERIAL_LIST_BLEND_COLOR)
            a3dc_write_a3da_rgba(&s, &i->blend_color);
        if (i->flags & A3DA_MATERIAL_LIST_INCANDESCENCE)
            a3dc_write_a3da_rgba(&s, &i->incandescence);
    }

    for (a3da_object* i = a->object.begin; i != a->object.end; i++) {
        a3dc_write_a3da_model_transform(&s, &i->model_transform, a->_compress_f16);

        for (a3da_object_texture_transform* j = i->texture_transform.begin;
            j != i->texture_transform.end; j++) {
            if (j->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_COVERAGE_U)
                a3dc_write_a3da_key(&s, &j->coverage_u);
            if (j->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_COVERAGE_V)
                a3dc_write_a3da_key(&s, &j->coverage_v);
            if (j->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_REPEAT_U)
                a3dc_write_a3da_key(&s, &j->repeat_u);
            if (j->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_REPEAT_V)
                a3dc_write_a3da_key(&s, &j->repeat_v);
            if (j->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_OFFSET_U)
                a3dc_write_a3da_key(&s, &j->offset_u);
            if (j->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_OFFSET_V)
                a3dc_write_a3da_key(&s, &j->offset_v);
            if (j->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_ROTATE)
                a3dc_write_a3da_key(&s, &j->rotate);
            if (j->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_ROTATE_FRAME)
                a3dc_write_a3da_key(&s, &j->rotate_frame);
            if (j->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_TRANSLATE_FRAME_U)
                a3dc_write_a3da_key(&s, &j->translate_frame_u);
            if (j->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_TRANSLATE_FRAME_V)
                a3dc_write_a3da_key(&s, &j->translate_frame_v);
        }
    }

    for (a3da_object_hrc* i = a->object_hrc.begin; i != a->object_hrc.end; i++)
        for (a3da_object_node* j = i->node.begin; j != i->node.end; j++)
            a3dc_write_a3da_model_transform(&s, &j->model_transform, a->_compress_f16);

    for (a3da_point* i = a->point.begin; i != a->point.end; i++)
        a3dc_write_a3da_model_transform(&s, &i->model_transform, a->_compress_f16);

    if (a->post_process.flags) {
        a3da_post_process* pp = &a->post_process;

        if (pp->flags & A3DA_POST_PROCESS_LENS_FLARE)
            a3dc_write_a3da_key(&s, &pp->lens_flare);
        if (pp->flags & A3DA_POST_PROCESS_LENS_GHOST)
            a3dc_write_a3da_key(&s, &pp->lens_ghost);
        if (pp->flags & A3DA_POST_PROCESS_LENS_SHAFT)
            a3dc_write_a3da_key(&s, &pp->lens_shaft);
        if (pp->flags & A3DA_POST_PROCESS_INTENSITY)
            a3dc_write_a3da_rgba(&s, &pp->intensity);
        if (pp->flags & A3DA_POST_PROCESS_RADIUS)
            a3dc_write_a3da_rgba(&s, &pp->radius);
        if (pp->flags & A3DA_POST_PROCESS_SCENE_FADE)
            a3dc_write_a3da_rgba(&s, &pp->scene_fade);
    }

    for (a3da_camera_root* i = a->camera_root.begin; i != a->camera_root.end; i++) {
        a3da_camera_root_view_point* vp = &i->view_point;
        a3dc_write_a3da_model_transform_offset_data(&s, &i->model_transform);
        a3dc_write_a3da_model_transform_offset_data(&s, &vp->model_transform);
        a3dc_write_a3da_model_transform_offset_data(&s, &i->interest);
    }

    for (a3da_chara* i = a->chara.begin; i != a->chara.end; i++)
        a3dc_write_a3da_model_transform_offset_data(&s, &i->model_transform);

    if (a->dof.has_dof) {
        a3da_dof* d = &a->dof;

        a3dc_write_a3da_model_transform_offset_data(&s, &d->model_transform);
    }

    for (a3da_light* i = a->light.begin; i != a->light.end; i++) {
        if (i->flags & A3DA_LIGHT_POSITION)
            a3dc_write_a3da_model_transform_offset_data(&s, &i->position);
        if (i->flags & A3DA_LIGHT_SPOT_DIRECTION)
            a3dc_write_a3da_model_transform_offset_data(&s, &i->spot_direction);
    }

    for (a3da_m_object_hrc* i = a->m_object_hrc.begin; i != a->m_object_hrc.end; i++) {
        a3dc_write_a3da_model_transform_offset_data(&s, &i->model_transform);

        for (a3da_object_node* j = i->node.begin; j != i->node.end; j++)
            a3dc_write_a3da_model_transform_offset_data(&s, &j->model_transform);

        for (a3da_object_instance* j = i->instance.begin; j != i->instance.end; j++)
            a3dc_write_a3da_model_transform_offset_data(&s, &j->model_transform);
    }

    for (a3da_object* i = a->object.begin; i != a->object.end; i++)
        a3dc_write_a3da_model_transform_offset_data(&s, &i->model_transform);

    for (a3da_object_hrc* i = a->object_hrc.begin; i != a->object_hrc.end; i++)
        for (a3da_object_node* j = i->node.begin; j != i->node.end; j++)
            a3dc_write_a3da_model_transform_offset_data(&s, &j->model_transform);

    for (a3da_point* i = a->point.begin; i != a->point.end; i++)
        a3dc_write_a3da_model_transform_offset_data(&s, &i->model_transform);

    io_align_write(&s, 0x10);
    io_mcopy(&s, data, length);
    io_free(&s);
}

static void a3da_get_time_stamp(char* buf, size_t buf_size) {
    time_t time_now;
    struct tm tm;
    time(&time_now);
    gmtime_s(&tm, &time_now);

    const char* day_of_week[] = {
        "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat",
    };

    const char* month[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
    };

    sprintf_s(buf, buf_size, "#%s %s %02d %02d:%02d:%02d %04d\n",
        day_of_week[tm.tm_wday], month[tm.tm_mon],
        tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, 1900 + tm.tm_year);
}

static bool key_val_read_a3da_key(key_val* kv, char* buf,
    size_t offset, char* str_add, size_t str_add_len, a3da_key* value) {
    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    memset(value, 0, sizeof(a3da_key));
    key_val lkv;
    if (!key_val_get_local_key_val(kv, buf, &lkv))
        return false;
    else if (key_val_read_int32_t(&lkv, buf, offset, ".bin_offset", 12, &value->bin_offset)) {
        value->flags = A3DA_KEY_BIN_OFFSET;
        key_val_free(&lkv);
        return true;
    }
    else if (!key_val_read_int32_t(&lkv, buf, offset, ".type", 6, (int32_t*)&value->type)) {
        key_val_free(&lkv);
        return false;
    }

    if (value->type == A3DA_KEY_NONE) {
        key_val_free(&lkv);
        return true;
    }
    else if (value->type == A3DA_KEY_STATIC) {
        key_val_read_float_t(&lkv, buf, offset, ".value", 7, &value->value);
        key_val_free(&lkv);
        return true;
    }

    key_val_read_int32_t(&lkv, buf, offset,
        ".ep_type_post", 14, (int32_t*)&value->ep_type_post);
    key_val_read_int32_t(&lkv, buf, offset,
        ".ep_type_pre", 13, (int32_t*)&value->ep_type_pre);
    key_val_read_float_t(&lkv, buf, offset, ".max", 5, &value->max);

    if (key_val_read_a3da_key_raw_data(&lkv, buf, offset, value)) {
        key_val_free(&lkv);
        return true;
    }

    memcpy(buf + offset, ".key", 5);
    offset += 4;

    key_val sub_local_key_val;
    if (!key_val_get_local_key_val(&lkv, buf, &sub_local_key_val)) {
        key_val_free(&lkv);
        return false;
    }
    key_val_free(&lkv);

    int32_t length = 0;
    key_val_read_int32_t(&sub_local_key_val, buf, offset, ".length", 8, &length);
    vector_kft3_reserve(&value->keys, length);

    kft3 k = { 0.0f, 0.0f, 0.0f, 0.0f };
    size_t len = offset;
    for (int32_t i = 0; i < length; i++) {
        size_t len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", i);
        offset = len + len1;

        char* data;
        int32_t type = 0;
        key_val_read_string_ptr(&sub_local_key_val, buf, offset, ".data", 6, &data);
        key_val_read_int32_t(&sub_local_key_val, buf, offset, ".type", 6, &type);

        switch (type) {
        case 0: {
            float_t f;
            if (sscanf_s(data, "%g", &f) == 1)
                k = (kft3){ f, 0.0f, 0.0f, 0.0f };
            else
                k = (kft3){ 0.0f, 0.0f, 0.0f, 0.0f };
            vector_kft3_push_back(&value->keys, &k);
        } break;
        case 1: {
            float_t f;
            float_t v;
            if (sscanf_s(data, "(%g,%g)", &f, &v) == 2)
                k = (kft3){ f, v, 0.0f, 0.0f };
            else
                k = (kft3){ 0.0f, 0.0f, 0.0f, 0.0f };
            vector_kft3_push_back(&value->keys, &k);
        } break;
        case 2: {
            float_t f;
            float_t v;
            float_t t;
            if (sscanf_s(data, "(%g,%g,%g)", &f, &v, &t) == 3)
                k = (kft3){ f, v, t, t };
            else
                k = (kft3){ 0.0f, 0.0f, 0.0f, 0.0f };
            vector_kft3_push_back(&value->keys, &k);
        } break;
        case 3: {
            float_t f;
            float_t v;
            float_t t1;
            float_t t2;
            if (sscanf_s(data, "(%g,%g,%g,%g)", &f, &v, &t1, &t2) == 4)
                k = (kft3){ f, v, t1, t2 };
            else
                k = (kft3){ 0.0f, 0.0f, 0.0f, 0.0f };
            vector_kft3_push_back(&value->keys, &k);
        } break;
        default: {
            k = (kft3){ 0.0f, 0.0f, 0.0f, 0.0f };
            vector_kft3_push_back(&value->keys, &k);
        } break;
        }
    }
    key_val_free(&sub_local_key_val);
    return true;
}

static void key_val_write_a3da_key(stream* s, char* buf,
    size_t offset, char* str_add, size_t str_add_len, a3da_key* value) {
    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    if (value->flags & A3DA_KEY_BIN_OFFSET) {
        key_val_write_int32_t(s, buf, offset, ".bin_offset", 12, value->bin_offset);
        value->flags &= ~A3DA_KEY_BIN_OFFSET;
        value->bin_offset = 0;
        return;
    }

    if (value->type == A3DA_KEY_NONE) {
        key_val_write_int32_t(s, buf, offset, ".type", 6, 0);;
        return;
    }
    else if (value->type == A3DA_KEY_STATIC) {
        key_val_write_int32_t(s, buf, offset, ".type", 6, 1);
        key_val_write_float_t(s, buf, offset, ".value", 7, value->value);
        return;
    }

    if (value->ep_type_post != A3DA_EP_NONE)
        key_val_write_int32_t(s, buf, offset,
            ".ep_type_post", 14, value->ep_type_post);
    if (value->ep_type_pre != A3DA_EP_NONE)
        key_val_write_int32_t(s, buf, offset,
            ".ep_type_pre", 13, value->ep_type_pre);

    if (value->raw_data) {
        key_val_write_a3da_key_raw_data(s, buf, offset, value);
        return;
    }

    memcpy(buf + offset, ".key", 5);
    offset += 4;

    int32_t length = (int32_t)(value->keys.end - value->keys.begin);

    vector_int32_t sort_index = vector_empty(int32_t);
    key_val_get_lexicographic_order(&sort_index, length);
    size_t len = offset;
    for (int32_t i = 0; i < length; i++) {
        size_t len1 = sprintf_s(buf + len, A3DA_TEXT_BUF_SIZE - len, ".%d", sort_index.begin[i]);
        offset = len + len1;

        kft3 k = value->keys.begin[sort_index.begin[i]];
        kf_type kt = KEY_FRAME_TYPE_3;
        kft_check(&k, kt, &k, &kt);

        char data_buf[0x200];
        switch (kt) {
        case KEY_FRAME_TYPE_0:
            sprintf_s(data_buf, sizeof(data_buf), "%g",
                k.frame);
            break;
        case KEY_FRAME_TYPE_1:
            sprintf_s(data_buf, sizeof(data_buf), "(%g,%g)",
                k.frame, k.value);
            break;
        case KEY_FRAME_TYPE_2:
            sprintf_s(data_buf, sizeof(data_buf), "(%g,%g,%g)",
                k.frame, k.value, k.tangent1);
            break;
        case KEY_FRAME_TYPE_3:
            sprintf_s(data_buf, sizeof(data_buf), "(%g,%g,%g,%g)",
                k.frame, k.value, k.tangent1, k.tangent2);
            break;
        }

        key_val_write_string_ptr(s, buf, offset, ".data", 6, data_buf);
        key_val_write_int32_t(s, buf, offset, ".type", 6, kt);
    }
    vector_int32_t_free(&sort_index, 0);

    offset = len;
    key_val_write_int32_t(s, buf, offset, ".length", 8, length);

    offset -= 4;
    key_val_write_float_t(s, buf, offset, ".max", 5, value->max);
    key_val_write_int32_t(s, buf, offset, ".type", 6, value->type);
}

static bool key_val_read_a3da_key_raw_data(key_val* kv, char* buf,
    size_t offset, a3da_key* value) {
    int32_t key_type = 0;
    if (!key_val_read_int32_t(kv, buf, offset, ".raw_data_key_type", 19, &key_type))
        return false;

    char* value_type;
    key_val_read_string_ptr(kv, buf, offset, ".raw_data.value_type", 21, &value_type);
    if (str_utils_compare(value_type, "float"))
        return false;

    int32_t value_list_size = 0;
    key_val_read_int32_t(kv, buf, offset, ".raw_data.value_list_size", 26, &value_list_size);

    int32_t value_list_offset = 0;
    if (key_val_read_int32_t(kv, buf, offset, ".raw_data.value_list_offset", 28, &value_list_offset)) {
        if (key_type != 3)
            return false;

        value->raw_data = true;
        value->raw_data_binary = true;
        value->raw_data_value_list_size = value_list_size;
        value->raw_data_value_list_offset = value_list_offset;
        return true;
    }

    char* value_list;
    key_val_read_string_ptr(kv, buf, offset, ".raw_data.value_list", 21, &value_list);

    char* s = value_list;
    size_t c = 1;
    while (s = strchr(s, ',')) {
        s++;
        c++;
    }

    if (c != value_list_size)
        return false;

    float_t* fs = force_malloc_s(float_t, c);
    s = value_list;
    for (size_t i = 0; i < c; i++) {
        char* t = s;
        s = strchr(s, ',');
        if (s)
            *s++ = 0;
        fs[i] = (float_t)atof(t);
    }

    switch (key_type) {
    case 0: {
        kft3 k = { 0.0f, 0.0f, 0.0f, 0.0f };
        vector_kft3_reserve(&value->keys, c);
        for (size_t i = 0; i < c; i++) {
            k.frame = *fs++;
            vector_kft3_push_back(&value->keys, &k);
        }
        fs -= c;
    } break;
    case 1: {
        kft3 k = { 0.0f, 0.0f, 0.0f, 0.0f };
        c /= 2;
        vector_kft3_reserve(&value->keys, c);
        for (size_t i = 0; i < c; i++) {
            k.frame = *fs++;
            k.value = *fs++;
            vector_kft3_push_back(&value->keys, &k);
        }
        fs -= c * 2;
    } break;
    case 2: {
        kft3 k = { 0.0f, 0.0f, 0.0f, 0.0f };
        c /= 3;
        vector_kft3_reserve(&value->keys, c);
        for (size_t i = 0; i < c; i++) {
            k.frame = *fs++;
            k.value = *fs++;
            k.tangent1 = k.tangent2 = *fs++;
            vector_kft3_push_back(&value->keys, &k);
        }
        fs -= c * 3;
    } break;
    case 3: {
        kft3 k = { 0.0f, 0.0f, 0.0f, 0.0f };
        c /= 4;
        vector_kft3_reserve(&value->keys, c);
        for (size_t i = 0; i < c; i++) {
            k.frame = *fs++;
            k.value = *fs++;
            k.tangent1 = *fs++;
            k.tangent2 = *fs++;
            vector_kft3_push_back(&value->keys, &k);
        }
        fs -= c * 4;
    } break;
    default:
        free(fs);
        return false;
    }
    free(fs);

    value->raw_data = true;
    return true;
}

static void key_val_write_a3da_key_raw_data(stream* s, char* buf,
    size_t offset, a3da_key* value) {
    key_val_write_float_t(s, buf, offset, ".max", 5, value->max);

    int32_t length = (int32_t)(value->keys.end - value->keys.begin);
    if (value->raw_data_binary) {
        key_val_write_int32_t(s, buf, offset, ".raw_data.value_list_offset", 6,
            value->raw_data_value_list_offset);
        key_val_write_int32_t(s, buf, offset, ".raw_data.value_list_size", 26,
            value->raw_data_value_list_size);
        key_val_write_string_ptr(s, buf, offset, ".raw_data.value_type", 21, "float");
        key_val_write_int32_t(s, buf, offset, ".raw_data_key_type", 19, 3);
        key_val_write_int32_t(s, buf, offset, ".type", 6, value->type);
        return;
    }

    kf_type key_type = KEY_FRAME_TYPE_0;
    for (int32_t i = 0; i < length; i++) {
        kft3 k = value->keys.begin[i];
        kf_type kt = KEY_FRAME_TYPE_3;
        kft_check(&k, kt, &k, &kt);
        if (key_type < kt)
            key_type = kt;
        if (key_type == KEY_FRAME_TYPE_3)
            break;
    }

    memcpy(buf + offset, ".raw_data.value_list=", 22);

    io_write_utf8_string(s, buf);
    char data_buf[0x200];
    switch (key_type) {
    case KEY_FRAME_TYPE_0:
        for (int32_t i = 0; i < length; i++) {
            kft3 k = value->keys.begin[i];
            sprintf_s(data_buf, sizeof(data_buf), "%g",
                k.frame);
            io_write_utf8_string(s, data_buf);
            if (i + 1 < length)
                io_write_char(s, ',');
        }
        break;
    case KEY_FRAME_TYPE_1:
        for (int32_t i = 0; i < length; i++) {
            kft3 k = value->keys.begin[i];
            sprintf_s(data_buf, sizeof(data_buf), "%g,%g",
                k.frame, k.value);
            io_write_utf8_string(s, data_buf);
            if (i + 1 < length)
                io_write_char(s, ',');
        }
        break;
    case KEY_FRAME_TYPE_2:
        for (int32_t i = 0; i < length; i++) {
            kft3 k = value->keys.begin[i];
            sprintf_s(data_buf, sizeof(data_buf), "%g,%g,%g",
                k.frame, k.value, k.tangent1);
            io_write_utf8_string(s, data_buf);
            if (i + 1 < length)
                io_write_char(s, ',');
        }
        break;
    case KEY_FRAME_TYPE_3:
        for (int32_t i = 0; i < length; i++) {
            kft3 k = value->keys.begin[i];
            sprintf_s(data_buf, sizeof(data_buf), "%g,%g,%g,%g",
                k.frame, k.value, k.tangent1, k.tangent2);
            io_write_utf8_string(s, data_buf);
            if (i + 1 < length)
                io_write_char(s, ',');
        }
        break;
    }
    io_write_char(s, '\n');

    key_val_write_int32_t(s, buf, offset, ".raw_data.value_list_size", 26,
        (int32_t)(length * ((size_t)key_type + 1)));
    key_val_write_string_ptr(s, buf, offset, ".raw_data.value_type", 21, "float");
    key_val_write_int32_t(s, buf, offset, ".raw_data_key_type", 19, key_type);
    key_val_write_int32_t(s, buf, offset, ".type", 6, value->type);
}

static bool key_val_read_a3da_model_transform(key_val* kv, char* buf,
    size_t offset, char* str_add, size_t str_add_len,
    a3da_model_transform* value) {
    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    memset(value, 0, sizeof(a3da_model_transform));
    key_val lkv;
    if (!key_val_get_local_key_val(kv, buf, &lkv))
        return false;
    else if (key_val_read_int32_t(&lkv, buf, offset,
        ".model_transform.bin_offset", 28, &value->bin_offset)) {
        value->flags = A3DA_MODEL_TRANSFORM_BIN_OFFSET;
        key_val_free(&lkv);
        return true;
    }

    key_val_read_a3da_vec3(&lkv, buf, offset, ".rot", 5, &value->rotation);
    key_val_read_a3da_vec3(&lkv, buf, offset, ".scale", 7, &value->scale);
    key_val_read_a3da_vec3(&lkv, buf, offset, ".trans", 7, &value->translation);
    key_val_read_a3da_key(&lkv, buf, offset,".visibility", 12, &value->visibility);
    key_val_free(&lkv);
    return true;
}

static void key_val_write_a3da_model_transform(stream* s, char* buf,
    size_t offset, char* str_add, size_t str_add_len, a3da_model_transform* value, int32_t write_mask) {
    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    if (value->flags & A3DA_MODEL_TRANSFORM_BIN_OFFSET) {
        if (write_mask & 0x10) {
            key_val_write_int32_t(s, buf, offset,
                ".model_transform.bin_offset", 28, value->bin_offset);
        }
        if (write_mask & 0x01)
            if (value->flags & A3DA_MODEL_TRANSFORM_BIN_OFFSET) {
                value->flags &= ~A3DA_MODEL_TRANSFORM_BIN_OFFSET;
                value->bin_offset = 0;
            }
    }
    else {
        if (write_mask & 0x08)
            key_val_write_a3da_vec3(s, buf, offset, ".rot", 5, &value->rotation);
        if (write_mask & 0x04)
            key_val_write_a3da_vec3(s, buf, offset, ".scale", 7, &value->scale);
        if (write_mask & 0x02)
            key_val_write_a3da_vec3(s, buf, offset, ".trans", 7, &value->translation);
        if (write_mask & 0x01)
            key_val_write_a3da_key(s, buf, offset, ".visibility", 12, &value->visibility);
    }
}

static bool key_val_read_a3da_rgba(key_val* kv, char* buf,
    size_t offset, char* str_add, size_t str_add_len, a3da_rgba* value) {
    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    memset(value, 0, sizeof(a3da_rgba));
    key_val lkv;
    if (!key_val_get_local_key_val(kv, buf, &lkv))
        return false;

    if (key_val_read_a3da_key(&lkv, buf, offset, ".r", 3, &value->r))
        value->flags |= A3DA_RGBA_R;
    if (key_val_read_a3da_key(&lkv, buf, offset, ".g", 3, &value->g))
        value->flags |= A3DA_RGBA_G;
    if (key_val_read_a3da_key(&lkv, buf, offset, ".b", 3, &value->b))
        value->flags |= A3DA_RGBA_B;
    if (key_val_read_a3da_key(&lkv, buf, offset, ".a", 3, &value->a))
        value->flags |= A3DA_RGBA_A;
    key_val_free(&lkv);
    return true;
}

static void key_val_write_a3da_rgba(stream* s, char* buf,
    size_t offset, char* str_add, size_t str_add_len, a3da_rgba* value) {
    if (!value->flags)
        return;

    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    if (value->flags & A3DA_RGBA_R)
        key_val_write_a3da_key(s, buf, offset, ".r", 3, &value->r);
    if (value->flags & A3DA_RGBA_G)
        key_val_write_a3da_key(s, buf, offset, ".g", 3, &value->g);
    if (value->flags & A3DA_RGBA_B)
        key_val_write_a3da_key(s, buf, offset, ".b", 3, &value->b);
    if (value->flags & A3DA_RGBA_A)
        key_val_write_a3da_key(s, buf, offset, ".a", 3, &value->a);
}

static bool key_val_read_a3da_vec3(key_val* kv, char* buf,
    size_t offset, char* str_add, size_t str_add_len, a3da_vec3* value) {
    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    memset(value, 0, sizeof(a3da_vec3));
    key_val lkv;
    if (!key_val_get_local_key_val(kv, buf, &lkv))
        return false;

    key_val_read_a3da_key(&lkv, buf, offset, ".x", 3, &value->x);
    key_val_read_a3da_key(&lkv, buf, offset, ".y", 3, &value->y);
    key_val_read_a3da_key(&lkv, buf, offset, ".z", 3, &value->z);
    key_val_free(&lkv);
    return true;
}

static void key_val_write_a3da_vec3(stream* s, char* buf,
    size_t offset, char* str_add, size_t str_add_len, a3da_vec3* value) {
    memcpy(buf + offset, str_add, str_add_len);
    offset += str_add_len - 1;

    key_val_write_a3da_key(s, buf, offset, ".x", 3, &value->x);
    key_val_write_a3da_key(s, buf, offset, ".y", 3, &value->y);
    key_val_write_a3da_key(s, buf, offset, ".z", 3, &value->z);
}

inline static void a3dc_read_a3da_key(void* data, size_t length, a3da_key* value) {
    a3dc_read_a3da_key_f16(data, length, value, A3DA_COMPRESS_F32F32F32F32);
}

inline static void a3dc_write_a3da_key(stream* s, a3da_key* value) {
    a3dc_write_a3da_key_f16(s, value, A3DA_COMPRESS_F32F32F32F32);
}

static void a3dc_read_a3da_key_f16(void* data, size_t length, a3da_key* value, a3da_compress_f16 f16) {
    if (value->raw_data) {
        if (!value->raw_data_binary)
            return;

        size_t _d = (size_t)data + value->raw_data_value_list_offset;
        value->raw_data_value_list_offset = 0;

        int32_t len = value->raw_data_value_list_size / 4;
        vector_kft3_reserve(&value->keys, length);
        for (int32_t i = 0; i < len; i++) {
            kft3 k;
            k.frame = *(float_t*)_d;
            k.value = *(float_t*)(_d + 4);
            k.tangent1 = *(float_t*)(_d + 8);
            k.tangent2 = *(float_t*)(_d + 12);
            vector_kft3_push_back(&value->keys, &k);
            _d += 16;
        }
        return;
    }
    else if (~value->flags & A3DA_KEY_BIN_OFFSET)
        return;

    a3dc_key_header* head = (void*)((size_t)data + value->bin_offset);
    value->flags &= ~A3DA_KEY_BIN_OFFSET;
    value->bin_offset = 0;

    size_t d = (size_t)head + sizeof(a3dc_key_header);

    if (head->type == A3DA_KEY_NONE) {
        value->type = head->type;
        return;
    }
    else if (head->type == A3DA_KEY_STATIC) {
        value->type = head->type;
        value->value = head->value;
        return;
    }

    value->type = head->type;
    value->ep_type_pre = head->ep_type_pre;
    value->ep_type_post = head->ep_type_post;
    value->max = head->max;
    uint32_t len = head->length;

    vector_kft3_reserve(&value->keys, len);
    switch (f16) {
    case A3DA_COMPRESS_F32F32F32F32:
        for (uint32_t i = 0; i < len; i++) {
            kft3 k;
            k.frame = *(float_t*)d;
            k.value = *(float_t*)(d + 4);
            k.tangent1 = *(float_t*)(d + 8);
            k.tangent2 = *(float_t*)(d + 12);
            vector_kft3_push_back(&value->keys, &k);
            d += 16;
        }
        break;
    case A3DA_COMPRESS_I16F16F32F32:
        for (uint32_t i = 0; i < len; i++) {
            kft3 k;
            k.frame = (float_t)*(int16_t*)d;
            k.value = half_to_float(*(half_t*)(d + 2));
            k.tangent1 = *(float_t*)(d + 4);
            k.tangent2 = *(float_t*)(d + 8);
            vector_kft3_push_back(&value->keys, &k);
            d += 12;
        }
        break;
    case A3DA_COMPRESS_I16F16F16F16:
        for (uint32_t i = 0; i < len; i++) {
            kft3 k;
            k.frame = (float_t)*(int16_t*)d;
            k.value = half_to_float(*(half_t*)(d + 2));
            k.tangent1 = half_to_float(*(half_t*)(d + 4));
            k.tangent2 = half_to_float(*(half_t*)(d + 6));
            vector_kft3_push_back(&value->keys, &k);
            d += 8;
        }
        break;
    }
}

static void a3dc_write_a3da_key_f16(stream* s, a3da_key* value, a3da_compress_f16 f16) {
    if (value->raw_data) {
        if ((int32_t)(value->keys.end - value->keys.begin) < 1) {
            value->raw_data = false;
            value->raw_data_binary = false;
            value->type = A3DA_KEY_NONE;
            vector_kft3_free(&value->keys, 0);
        }
        else if ((int32_t)(value->keys.end - value->keys.begin) == 1) {
            value->raw_data = false;
            value->raw_data_binary = false;
            value->type = A3DA_KEY_STATIC;
            value->value = value->keys.begin[0].value;
            vector_kft3_free(&value->keys, 0);
        }
    }

    if (value->raw_data) {
        if (!value->raw_data_binary)
            return;

        value->raw_data_value_list_offset = (int32_t)io_get_position(s);

        int32_t len = (int32_t)(value->keys.end - value->keys.begin);
        value->raw_data_value_list_size = len * 4;
        for (int32_t i = 0; i < len; i++) {
            kft3* k = &value->keys.begin[i];
            io_write_float_t(s, k->frame);
            io_write_float_t(s, k->value);
            io_write_float_t(s, k->tangent1);
            io_write_float_t(s, k->tangent2);
        }
        return;
    }

    value->bin_offset = (int32_t)io_get_position(s);
    value->flags |= A3DA_KEY_BIN_OFFSET;

    if (value->type == A3DA_KEY_NONE) {
        io_write_int32_t(s, A3DA_KEY_NONE);
        io_write_float_t(s, 0.0f);
        return;
    }
    else if (value->type == A3DA_KEY_STATIC) {
        io_write_int32_t(s, A3DA_KEY_STATIC);
        io_write_float_t(s, value->value);
        return;
    }

    uint32_t len = (uint32_t)(value->keys.end - value->keys.begin);

    a3dc_key_header head;
    memset(&head, 0, sizeof(a3dc_key_header));
    head.type = value->type;
    head.ep_type_pre = value->ep_type_pre;
    head.ep_type_post = value->ep_type_post;
    head.max = value->max;
    head.length = len;
    io_write(s, &head, sizeof(a3dc_key_header));

    switch (f16) {
    case A3DA_COMPRESS_F32F32F32F32:
        for (uint32_t i = 0; i < len; i++) {
            kft3* k = &value->keys.begin[i];
            io_write_float_t(s, k->frame);
            io_write_float_t(s, k->value);
            io_write_float_t(s, k->tangent1);
            io_write_float_t(s, k->tangent2);
        }
        break;
    case A3DA_COMPRESS_I16F16F32F32:
        for (uint32_t i = 0; i < len; i++) {
            kft3* k = &value->keys.begin[i];
            io_write_int16_t(s, (int16_t)roundf(k->frame));
            io_write_half_t(s, float_to_half(k->value));
            io_write_float_t(s, k->tangent1);
            io_write_float_t(s, k->tangent2);
        }
        break;
    case A3DA_COMPRESS_I16F16F16F16:
        for (uint32_t i = 0; i < len; i++) {
            kft3* k = &value->keys.begin[i];
            io_write_int16_t(s, (int16_t)roundf(k->frame));
            io_write_half_t(s, float_to_half(k->value));
            io_write_half_t(s, float_to_half(k->tangent1));
            io_write_half_t(s, float_to_half(k->tangent2));
        }
        break;
    }
}

static void a3dc_read_a3da_model_transform(void* data, size_t length,
    a3da_model_transform* value, a3da_compress_f16 f16) {
    if (~value->flags & A3DA_MODEL_TRANSFORM_BIN_OFFSET)
        return;

    a3da_model_transform_header* head = (void*)((size_t)data + value->bin_offset);
    value->flags &= ~A3DA_MODEL_TRANSFORM_BIN_OFFSET;
    value->bin_offset = 0;

    value->scale.x.bin_offset = head->scale.x;
    value->scale.x.flags |= A3DA_KEY_BIN_OFFSET;
    value->scale.y.bin_offset = head->scale.y;
    value->scale.y.flags |= A3DA_KEY_BIN_OFFSET;
    value->scale.z.bin_offset = head->scale.z;
    value->scale.z.flags |= A3DA_KEY_BIN_OFFSET;
    value->rotation.x.bin_offset = head->rotation.x;
    value->rotation.x.flags |= A3DA_KEY_BIN_OFFSET;
    value->rotation.y.bin_offset = head->rotation.y;
    value->rotation.y.flags |= A3DA_KEY_BIN_OFFSET;
    value->rotation.z.bin_offset = head->rotation.z;
    value->rotation.z.flags |= A3DA_KEY_BIN_OFFSET;
    value->translation.x.bin_offset = head->translation.x;
    value->translation.x.flags |= A3DA_KEY_BIN_OFFSET;
    value->translation.y.bin_offset = head->translation.y;
    value->translation.y.flags |= A3DA_KEY_BIN_OFFSET;
    value->translation.z.bin_offset = head->translation.z;
    value->translation.z.flags |= A3DA_KEY_BIN_OFFSET;
    value->visibility.bin_offset = head->visibility;
    value->visibility.flags |= A3DA_KEY_BIN_OFFSET;

    a3dc_read_a3da_vec3(data, length, &value->scale);
    a3dc_read_a3da_vec3_f16(data, length, &value->rotation, f16);
    a3dc_read_a3da_vec3(data, length, &value->translation);
    a3dc_read_a3da_key(data, length, &value->visibility);
}

static void a3dc_write_a3da_model_transform(stream* s,
    a3da_model_transform* value, a3da_compress_f16 f16) {
    a3dc_write_a3da_vec3(s, &value->scale);
    a3dc_write_a3da_vec3(s, &value->rotation);
    a3dc_write_a3da_vec3(s, &value->translation);
    a3dc_write_a3da_key(s, &value->visibility);
}

static void a3dc_write_a3da_model_transform_offset(stream* s,
    a3da_model_transform* value) {
    value->bin_offset = (int32_t)io_get_position(s);
    value->flags |= A3DA_MODEL_TRANSFORM_BIN_OFFSET;
    io_write(s, 0, 0x30);
}

static void a3dc_write_a3da_model_transform_offset_data(stream* s,
    a3da_model_transform* value) {
    io_position_push(s, value->bin_offset, SEEK_SET);
    io_write_uint32_t(s, value->scale.x.bin_offset);
    value->scale.x.flags &= ~A3DA_KEY_BIN_OFFSET;
    value->scale.x.bin_offset = 0;
    io_write_uint32_t(s, value->scale.y.bin_offset);
    value->scale.y.flags &= ~A3DA_KEY_BIN_OFFSET;
    value->scale.y.bin_offset = 0;
    io_write_uint32_t(s, value->scale.z.bin_offset);
    value->scale.z.flags &= ~A3DA_KEY_BIN_OFFSET;
    value->scale.z.bin_offset = 0;
    io_write_uint32_t(s, value->rotation.x.bin_offset);
    value->rotation.x.flags &= ~A3DA_KEY_BIN_OFFSET;
    value->rotation.x.bin_offset = 0;
    io_write_uint32_t(s, value->rotation.y.bin_offset);
    value->rotation.y.flags &= ~A3DA_KEY_BIN_OFFSET;
    value->rotation.y.bin_offset = 0;
    io_write_uint32_t(s, value->rotation.z.bin_offset);
    value->rotation.z.flags &= ~A3DA_KEY_BIN_OFFSET;
    value->rotation.z.bin_offset = 0;
    io_write_uint32_t(s, value->translation.x.bin_offset);
    value->translation.x.flags &= ~A3DA_KEY_BIN_OFFSET;
    value->translation.x.bin_offset = 0;
    io_write_uint32_t(s, value->translation.y.bin_offset);
    value->translation.y.flags &= ~A3DA_KEY_BIN_OFFSET;
    value->translation.y.bin_offset = 0;
    io_write_uint32_t(s, value->translation.z.bin_offset);
    value->translation.z.flags &= ~A3DA_KEY_BIN_OFFSET;
    value->translation.z.bin_offset = 0;
    io_write_uint32_t(s, value->visibility.bin_offset);
    value->visibility.flags &= ~A3DA_KEY_BIN_OFFSET;
    value->visibility.bin_offset = 0;
    io_position_pop(s);
}

static void a3dc_read_a3da_rgba(void* data, size_t length, a3da_rgba* value) {
    if (!value->flags)
        return;

    if (value->flags & A3DA_RGBA_R)
        a3dc_read_a3da_key(data, length, &value->r);
    if (value->flags & A3DA_RGBA_G)
        a3dc_read_a3da_key(data, length, &value->g);
    if (value->flags & A3DA_RGBA_B)
        a3dc_read_a3da_key(data, length, &value->b);
    if (value->flags & A3DA_RGBA_A)
        a3dc_read_a3da_key(data, length, &value->a);
}

static void a3dc_write_a3da_rgba(stream* s, a3da_rgba* value) {
    if (!value->flags)
        return;

    if (value->flags & A3DA_RGBA_R)
        a3dc_write_a3da_key(s, &value->r);
    if (value->flags & A3DA_RGBA_G)
        a3dc_write_a3da_key(s, &value->g);
    if (value->flags & A3DA_RGBA_B)
        a3dc_write_a3da_key(s, &value->b);
    if (value->flags & A3DA_RGBA_A)
        a3dc_write_a3da_key(s, &value->a);
}

static void a3dc_read_a3da_vec3(void* data, size_t length, a3da_vec3* value) {
    a3dc_read_a3da_key(data, length, &value->x);
    a3dc_read_a3da_key(data, length, &value->y);
    a3dc_read_a3da_key(data, length, &value->z);
}

static void a3dc_write_a3da_vec3(stream* s, a3da_vec3* value) {
    a3dc_write_a3da_key(s, &value->x);
    a3dc_write_a3da_key(s, &value->y);
    a3dc_write_a3da_key(s, &value->z);
}

static void a3dc_read_a3da_vec3_f16(void* data, size_t length, a3da_vec3* value, a3da_compress_f16 f16) {
    a3dc_read_a3da_key_f16(data, length, &value->x, f16);
    a3dc_read_a3da_key_f16(data, length, &value->y, f16);
    a3dc_read_a3da_key_f16(data, length, &value->z, f16);
}

static void a3dc_write_a3da_vec3_f16(stream* s, a3da_vec3* value, a3da_compress_f16 f16) {
    a3dc_write_a3da_key_f16(s, &value->x, f16);
    a3dc_write_a3da_key_f16(s, &value->y, f16);
    a3dc_write_a3da_key_f16(s, &value->z, f16);
}
