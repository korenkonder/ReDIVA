/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include <unordered_map>
#include "aet.hpp"
#include "f2/struct.hpp"
#include "io/path.hpp"
#include "io/memory_stream.hpp"

struct aet_layer_header {
    int64_t markers_offset;
    int64_t video_offset;
    int64_t audio_offset;
};

struct aet_scene_header {
    int64_t camera_offset;
    int64_t comp_offset;
    int64_t video_offset;
    int64_t audio_offset;
};

static void aet_move_data_audio(aet_audio* audio_dst, const aet_audio* audio_src,
    prj::shared_ptr<prj::stack_allocator> alloc);
static aet_camera* aet_move_data_camera(const aet_camera* cam_src,
    prj::shared_ptr<prj::stack_allocator> alloc);
static void aet_move_data_comp(aet_comp* comp_dst, const aet_comp* comp_src,
    prj::shared_ptr<prj::stack_allocator> alloc, std::unordered_map<void*, const aet_layer*>& layers);
static void aet_move_data_fcurve(aet_fcurve* fcurve_dst, const aet_fcurve* fcurve_src,
    prj::shared_ptr<prj::stack_allocator> alloc);
static void aet_move_data_layer(aet_layer* layer_dst, const aet_layer* layer_src,
    prj::shared_ptr<prj::stack_allocator> alloc);
static aet_layer_audio* aet_move_data_layer_audio(const aet_layer_audio* layer_audio_src,
    prj::shared_ptr<prj::stack_allocator> alloc);
static aet_layer_video* aet_move_data_layer_video(const aet_layer_video* layer_video_src,
    prj::shared_ptr<prj::stack_allocator> alloc);
static aet_layer_video_3d* aet_move_data_layer_video_3d(const aet_layer_video_3d* layer_video_3d_src,
    prj::shared_ptr<prj::stack_allocator> alloc);
static void aet_move_data_marker(aet_marker* marker_dst, const aet_marker* marker_src,
    prj::shared_ptr<prj::stack_allocator> alloc);
static void aet_move_data_scene(aet_scene* scene_dst, const aet_scene* scene_src,
    prj::shared_ptr<prj::stack_allocator> alloc,
    std::unordered_map<int64_t, const aet_layer*>& layers,
    std::unordered_map<int64_t, aet_item>& items);
static void aet_move_data_video(aet_video* video_dst, const aet_video* video_src,
    prj::shared_ptr<prj::stack_allocator> alloc);
static void aet_move_data_video_src(aet_video_src* video_dst, const aet_video_src* video_src,
    prj::shared_ptr<prj::stack_allocator> alloc);

static void aet_set_classic_read_inner(aet_set* as, prj::shared_ptr<prj::stack_allocator>& alloc, stream& s);
static void aet_set_classic_write_inner(aet_set* as, stream& s);
static void aet_set_modern_read_inner(aet_set* as, prj::shared_ptr<prj::stack_allocator>& alloc, stream& s);
static void aet_set_modern_write_inner(aet_set* as, stream& s);

static void aet_classic_read_audio(aet_audio* audio,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s,
    std::unordered_map<int64_t, aet_item>& items);
static void aet_classic_write_audio(const aet_audio* audio, stream& s,
    std::unordered_map<const void*, int64_t>& offsets);
static aet_camera* aet_classic_read_camera(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t offset);
static void aet_classic_write_camera(const aet_camera* cam, stream& s,
    std::unordered_map<const void*, int64_t>& offsets);
static void aet_classic_read_comp(aet_comp* comp,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s,
    std::unordered_map<int64_t, const aet_layer*>& layers,
    std::unordered_map<int64_t, aet_item>& items);
static void aet_classic_read_fcurve(aet_fcurve* fcurve,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s);
static void aet_classic_write_fcurve(const aet_fcurve* fcurve, stream& s,
    std::unordered_map<const void*, int64_t>& offsets);
static void aet_classic_write_fcurve_header(const aet_fcurve* fcurve, stream& s,
    std::unordered_map<const void*, int64_t>& offsets, int64_t null_data_pos, uint32_t& null_data_count);
static void aet_classic_read_layer(aet_layer* layer,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s);
static void aet_classic_write_layer(const aet_layer* layer, stream& s,
    std::unordered_map<const void*, int64_t>& offsets);
static aet_layer_audio* aet_classic_read_layer_audio(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t offset);
static void aet_classic_write_layer_audio(const aet_layer_audio* layer_audio, stream& s,
    std::unordered_map<const void*, int64_t>& offsets);
static aet_layer_video* aet_classic_read_layer_video(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t offset);
static void aet_classic_write_layer_video(const aet_layer_video* layer_video, stream& s,
    std::unordered_map<const void*, int64_t>& offsets);
static aet_layer_video_3d* aet_classic_read_layer_video_3d(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t offset);
static void aet_classic_write_layer_video_3d(const aet_layer_video_3d* layer_video_3d, stream& s,
    std::unordered_map<const void*, int64_t>& offsets);
static aet_scene* aet_classic_read_scene(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t offset, std::unordered_map<int64_t, const aet_layer*>& layers,
    std::unordered_map<int64_t, aet_item>& items);
static void aet_classic_read_video(aet_video* video,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s,
    std::unordered_map<int64_t, aet_item>& items);

static void aet_modern_read_audio(aet_audio* audio,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s,
    uint32_t header_length, bool is_x);
static aet_camera* aet_modern_read_camera(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t offset,
    uint32_t header_length, bool is_x);
static void aet_modern_read_comp(aet_comp* comp,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s,
    uint32_t header_length, bool is_x,
    std::unordered_map<int64_t, const aet_layer*>& layers, std::unordered_map<int64_t, aet_item>& items);
static void aet_modern_read_fcurve(aet_fcurve* fcurve,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s,
    uint32_t header_length, bool is_x);
static void aet_modern_read_layer(aet_layer* layer,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s,
    uint32_t header_length, bool is_x);
static aet_layer_audio* aet_modern_read_layer_audio(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t offset,
    uint32_t header_length, bool is_x);
static aet_layer_video* aet_modern_read_layer_video(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t offset,
    uint32_t header_length, bool is_x);
static aet_layer_video_3d* aet_modern_read_layer_video_3d(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t offset,
    uint32_t header_length, bool is_x);
static void aet_modern_read_marker(aet_marker* marker,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s,
    uint32_t header_length, bool is_x);
static aet_scene* aet_modern_read_scene(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t offset,
    uint32_t header_length, bool is_x,
    std::unordered_map<int64_t, const aet_layer*>& layers, std::unordered_map<int64_t, aet_item>& items);
static void aet_modern_read_video(aet_video* video,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s,
    uint32_t header_length, bool is_x,
    std::unordered_map<int64_t, aet_item>& items);

static void aet_comp_set_item_parent(const aet_comp* comp,
    std::unordered_map<int64_t, const aet_layer*>& layers,
    std::unordered_map<int64_t, aet_item>& items);

static const char* aet_move_data_string(const char* str,
    prj::shared_ptr<prj::stack_allocator>& alloc);

static const char* aet_read_utf8_string_null_terminated_offset(
    prj::shared_ptr<prj::stack_allocator>& alloc, stream& s, int64_t offset);

static int64_t aet_strings_get_string_offset(
    const std::unordered_map<std::string, int64_t>& vec, const std::string& str);
static void aet_strings_push_back_check(stream& s,
    std::unordered_map<std::string, int64_t>& vec, const std::string& str);

aet_fcurve::aet_fcurve() : keys_count(), keys() {

}

float_t aet_fcurve::interpolate(float_t frame) const {
    if (!keys_count)
        return 0.0f;
    else if (keys_count == 1)
        return keys[0];

    const size_t keys_count = this->keys_count;
    const float_t* frames = keys;
    const float_t* values = &keys[keys_count];
    if (frame <= frames[0])
        return values[0];
    else if (frame >= frames[keys_count - 1])
        return values[(keys_count - 1) * 2];

    const float_t* f_begin = frames;
    const float_t* f_end = &frames[keys_count];

    const float_t* f = &frames[keys_count / 2];
    while (f_begin < f) {
        if (frame >= *f)
            f_begin = f;
        else
            f_end = f;
        f = &f_begin[(f_end - f_begin) / 2];
    }

    if (&frames[keys_count - 1] <= f)
        return values[(keys_count - 1) * 2];

    values += (f - frames) * 2;

    float_t p1 = values[0];
    float_t p2 = values[2];
    float_t t1 = values[1];
    float_t t2 = values[3];

    float_t df = f[1] - f[0];
    float_t t = (frame - f[0]) / df;
    float_t t_2 = t * t;
    float_t t_3 = t_2 * t;
    float_t t_23 = 3.0f * t_2;
    float_t t_32 = 2.0f * t_3;

    float_t h00 = t_32 - t_23 + 1.0f;
    float_t h01 = t_23 - t_32;
    float_t h10 = t_3 - 2.0f * t_2 + t;
    float_t h11 = t_3 - t_2;

    return (h10 * t1 + h11 * t2) * df + (h01 * p2 + h00 * p1);
}

aet_layer_video_3d::aet_layer_video_3d() {

}

aet_transfer_mode::aet_transfer_mode() : mode(), flag(), matte() {

}

aet_layer_video::aet_layer_video() : _3d() {

}

aet_layer_audio::aet_layer_audio() {

}

aet_camera::aet_camera() {

}

aet_video_src::aet_video_src() : sprite_name() {
    sprite_index = -1;
}

aet_video::aet_video() : color(), width(), height(), fpf(), sources_count(), sources() {

}

aet_audio::aet_audio() : sound_index() {

}

aet_marker::aet_marker() : time(), name() {

}

aet_layer::aet_layer() : name(), start_time(), end_time(), offset_time(), time_scale(),
flags(), quality(), item_type(), parent(), markers_count(), markers(), video(), audio() {

}

aet_comp::aet_comp() : layers_count(), layers() {

}

aet_scene::aet_scene() : name(), start_time(), end_time(), fps(), color(), width(),
height(), camera(), comp_count(), comp(), video_count(), video(), audio_count(), audio() {

}

const aet_comp* aet_scene::get_root_comp(const aet_scene* scene) {
    if (scene && scene->comp_count)
        return &scene->comp[scene->comp_count - 1];
    return 0;
}

aet_set::aet_set() : ready(), modern(), big_endian(), is_x(), scenes_count(), scenes() {

}

void aet_set::move_data(aet_set* set_src, prj::shared_ptr<prj::stack_allocator> alloc) {
    if (!set_src->ready) {
        ready = false;
        modern = false;
        big_endian = false;
        is_x = false;
        return;
    }

    ready = true;
    modern = set_src->modern;
    big_endian = set_src->big_endian;
    is_x = set_src->is_x;

    std::unordered_map<int64_t, aet_item> items;
    std::unordered_map<int64_t, const aet_layer*> layers;

    uint32_t scenes_count = set_src->scenes_count;
    const aet_scene** scenes_src = set_src->scenes;
    aet_scene** scenes_dst = alloc->allocate<aet_scene*>(scenes_count);

    for (uint32_t i = 0; i < scenes_count; i++) {
        scenes_dst[i] = alloc->allocate<aet_scene>();
        aet_move_data_scene(scenes_dst[i], scenes_src[i], alloc, layers, items);
    }

    this->scenes = (const aet_scene**)scenes_dst;
    this->scenes_count = scenes_count;

    for (uint32_t i = 0; i < scenes_count; i++) {
        aet_scene* scene = scenes_dst[i];
        const aet_comp* comp = scene->comp;
        for (uint32_t j = scene->comp_count; j; j--, comp++)
            aet_comp_set_item_parent(comp, layers, items);
    }
}

void aet_set::pack_file(void** data, size_t* size) {
    if (!data || !ready)
        return;

    memory_stream s;
    s.open();
    if (!modern)
        aet_set_classic_write_inner(this, s);
    else
        aet_set_modern_write_inner(this, s);
    s.align_write(0x10);
    s.copy(data, size);
}

void aet_set::unpack_file(prj::shared_ptr<prj::stack_allocator> alloc, const void* data, size_t size, bool modern) {
    if (!data || !size)
        return;

    memory_stream s;
    s.open(data, size);
    if (!modern)
        aet_set_classic_read_inner(this, alloc, s);
    else
        aet_set_modern_read_inner(this, alloc, s);
}

static void aet_move_data_audio(aet_audio* audio_dst, const aet_audio* audio_src,
    prj::shared_ptr<prj::stack_allocator> alloc) {
    audio_dst->sound_index = audio_src->sound_index;
}

static aet_camera* aet_move_data_camera(const aet_camera* cam_src,
    prj::shared_ptr<prj::stack_allocator> alloc) {
    if (!cam_src)
        return 0;

    aet_camera* cam_dst = alloc->allocate<aet_camera>();
    aet_move_data_fcurve(&cam_dst->eye_x, &cam_src->eye_x, alloc);
    aet_move_data_fcurve(&cam_dst->eye_y, &cam_src->eye_y, alloc);
    aet_move_data_fcurve(&cam_dst->eye_z, &cam_src->eye_z, alloc);
    aet_move_data_fcurve(&cam_dst->pos_x, &cam_src->pos_x, alloc);
    aet_move_data_fcurve(&cam_dst->pos_y, &cam_src->pos_y, alloc);
    aet_move_data_fcurve(&cam_dst->pos_z, &cam_src->pos_z, alloc);
    aet_move_data_fcurve(&cam_dst->dir_x, &cam_src->dir_x, alloc);
    aet_move_data_fcurve(&cam_dst->dir_y, &cam_src->dir_y, alloc);
    aet_move_data_fcurve(&cam_dst->dir_z, &cam_src->dir_z, alloc);
    aet_move_data_fcurve(&cam_dst->rot_x, &cam_src->rot_x, alloc);
    aet_move_data_fcurve(&cam_dst->rot_y, &cam_src->rot_y, alloc);
    aet_move_data_fcurve(&cam_dst->rot_z, &cam_src->rot_z, alloc);
    aet_move_data_fcurve(&cam_dst->zoom, &cam_src->zoom, alloc);
    return cam_dst;
}

static void aet_move_data_comp(aet_comp* comp_dst, const aet_comp* comp_src,
    prj::shared_ptr<prj::stack_allocator> alloc, std::unordered_map<int64_t, const aet_layer*>& layers) {
    uint32_t layers_count = comp_src->layers_count;
    const aet_layer* layers_src = comp_src->layers;
    aet_layer* layers_dst = alloc->allocate<aet_layer>(layers_count);

    for (uint32_t i = 0; i < layers_count; i++) {
        aet_move_data_layer(&layers_dst[i], &layers_src[i], alloc);
        layers[(int64_t)&layers_src[i]] = &layers_dst[i];
    }

    comp_dst->layers = layers_dst;
    comp_dst->layers_count = layers_count;
}

static void aet_move_data_fcurve(aet_fcurve* fcurve_dst, const aet_fcurve* fcurve_src,
    prj::shared_ptr<prj::stack_allocator> alloc) {
    fcurve_dst->keys_count = fcurve_src->keys_count;

    if (fcurve_src->keys_count > 1)
        fcurve_dst->keys = alloc->allocate<float_t>(fcurve_src->keys, fcurve_src->keys_count * 3ULL);
    else if (fcurve_src->keys_count)
        fcurve_dst->keys = alloc->allocate<float_t>(fcurve_src->keys);
    else
        fcurve_dst->keys = 0;
}

static void aet_move_data_layer(aet_layer* layer_dst, const aet_layer* layer_src,
    prj::shared_ptr<prj::stack_allocator> alloc) {
    layer_dst->name = aet_move_data_string(layer_src->name, alloc);
    layer_dst->start_time = layer_src->start_time;
    layer_dst->end_time = layer_src->end_time;
    layer_dst->offset_time = layer_src->offset_time;
    layer_dst->time_scale = layer_src->time_scale;
    layer_dst->flags = layer_src->flags;
    layer_dst->quality = layer_src->quality;
    layer_dst->item_type = layer_src->item_type;
    layer_dst->item.none = layer_src->item.none;
    layer_dst->parent = layer_src->parent;

    uint32_t markers_count = layer_src->markers_count;
    const aet_marker* markers_src = layer_src->markers;
    aet_marker* markers_dst = alloc->allocate<aet_marker>(markers_count);

    for (uint32_t i = 0; i < markers_count; i++)
        aet_move_data_marker(&markers_dst[i], &markers_src[i], alloc);

    layer_dst->markers = markers_dst;
    layer_dst->markers_count = markers_count;

    layer_dst->video = aet_move_data_layer_video(layer_src->video, alloc);
    layer_dst->audio = aet_move_data_layer_audio(layer_src->audio, alloc);
}

static aet_layer_audio* aet_move_data_layer_audio(const aet_layer_audio* layer_audio_src,
    prj::shared_ptr<prj::stack_allocator> alloc) {
    if (!layer_audio_src)
        return 0;

    aet_layer_audio* layer_audio_dst = alloc->allocate<aet_layer_audio>();
    aet_move_data_fcurve(&layer_audio_dst->volume_l, &layer_audio_src->volume_l, alloc);
    aet_move_data_fcurve(&layer_audio_dst->volume_r, &layer_audio_src->volume_r, alloc);
    aet_move_data_fcurve(&layer_audio_dst->pan_l, &layer_audio_src->pan_l, alloc);
    aet_move_data_fcurve(&layer_audio_dst->pan_r, &layer_audio_src->pan_r, alloc);
    return layer_audio_dst;
}

static aet_layer_video* aet_move_data_layer_video(const aet_layer_video* layer_video_src,
    prj::shared_ptr<prj::stack_allocator> alloc) {
    if (!layer_video_src)
        return 0;

    aet_layer_video* layer_video_dst = alloc->allocate<aet_layer_video>();
    layer_video_dst->transfer_mode = layer_video_src->transfer_mode;
    aet_move_data_fcurve(&layer_video_dst->anchor_x, &layer_video_src->anchor_x, alloc);
    aet_move_data_fcurve(&layer_video_dst->anchor_y, &layer_video_src->anchor_y, alloc);
    aet_move_data_fcurve(&layer_video_dst->pos_x, &layer_video_src->pos_x, alloc);
    aet_move_data_fcurve(&layer_video_dst->pos_y, &layer_video_src->pos_y, alloc);
    aet_move_data_fcurve(&layer_video_dst->rot_z, &layer_video_src->rot_z, alloc);
    aet_move_data_fcurve(&layer_video_dst->scale_x, &layer_video_src->scale_x, alloc);
    aet_move_data_fcurve(&layer_video_dst->scale_y, &layer_video_src->scale_y, alloc);
    aet_move_data_fcurve(&layer_video_dst->opacity, &layer_video_src->opacity, alloc);
    layer_video_dst->_3d = aet_move_data_layer_video_3d(layer_video_src->_3d, alloc);
    return layer_video_dst;
}

static aet_layer_video_3d* aet_move_data_layer_video_3d(const aet_layer_video_3d* layer_video_3d_src,
    prj::shared_ptr<prj::stack_allocator> alloc) {
    if (!layer_video_3d_src)
        return 0;

    aet_layer_video_3d* layer_video_3d_dst = alloc->allocate<aet_layer_video_3d>();
    aet_move_data_fcurve(&layer_video_3d_dst->anchor_z, &layer_video_3d_src->anchor_z, alloc);
    aet_move_data_fcurve(&layer_video_3d_dst->pos_z, &layer_video_3d_src->pos_z, alloc);
    aet_move_data_fcurve(&layer_video_3d_dst->dir_x, &layer_video_3d_src->dir_x, alloc);
    aet_move_data_fcurve(&layer_video_3d_dst->dir_y, &layer_video_3d_src->dir_y, alloc);
    aet_move_data_fcurve(&layer_video_3d_dst->dir_z, &layer_video_3d_src->dir_z, alloc);
    aet_move_data_fcurve(&layer_video_3d_dst->rot_x, &layer_video_3d_src->rot_x, alloc);
    aet_move_data_fcurve(&layer_video_3d_dst->rot_y, &layer_video_3d_src->rot_y, alloc);
    aet_move_data_fcurve(&layer_video_3d_dst->scale_z, &layer_video_3d_src->scale_z, alloc);
    return layer_video_3d_dst;
}

static void aet_move_data_marker(aet_marker* marker_dst, const aet_marker* marker_src,
    prj::shared_ptr<prj::stack_allocator> alloc) {
    marker_dst->time = marker_src->time;
    marker_dst->name = aet_move_data_string(marker_src->name, alloc);
}

static void aet_move_data_scene(aet_scene* scene_dst, const aet_scene* scene_src,
    prj::shared_ptr<prj::stack_allocator> alloc,
    std::unordered_map<int64_t, const aet_layer*>& layers,
    std::unordered_map<int64_t, aet_item>& items) {
    scene_dst->name = aet_move_data_string(scene_src->name, alloc);
    scene_dst->start_time = scene_src->start_time;
    scene_dst->end_time = scene_src->end_time;
    scene_dst->fps = scene_src->fps;
    scene_dst->color[0] = scene_src->color[0];
    scene_dst->color[1] = scene_src->color[1];
    scene_dst->color[2] = scene_src->color[2];
    scene_dst->width = scene_src->width;
    scene_dst->height = scene_src->height;

    scene_dst->camera = aet_move_data_camera(scene_src->camera, alloc);

    uint32_t video_count = scene_src->video_count;
    const aet_video* video_src = scene_src->video;
    aet_video* video_dst = alloc->allocate<aet_video>(video_count);

    for (uint32_t i = 0; i < video_count; i++) {
        aet_move_data_video(&video_dst[i], &video_src[i], alloc);
        items[(int64_t)&video_src[i]] = &video_dst[i];
    }

    scene_dst->video = video_dst;
    scene_dst->video_count = video_count;

    uint32_t audio_count = scene_src->audio_count;
    const aet_audio* audio_src = scene_src->audio;
    aet_audio* audio_dst = alloc->allocate<aet_audio>(audio_count);

    for (uint32_t i = 0; i < audio_count; i++)
        aet_move_data_audio(&audio_dst[i], &audio_src[i], alloc);

    scene_dst->audio = audio_dst;
    scene_dst->audio_count = audio_count;

    uint32_t comp_count = scene_src->comp_count;
    const aet_comp* comp_src = scene_src->comp;
    aet_comp* comp_dst = alloc->allocate<aet_comp>(comp_count);

    for (uint32_t i = 0; i < comp_count; i++) {
        aet_move_data_comp(&comp_dst[i], &comp_src[i], alloc, layers);
        items[(int64_t)&comp_src[i]] = &comp_dst[i];
    }

    scene_dst->comp = comp_dst;
    scene_dst->comp_count = comp_count;
}

static void aet_move_data_video(aet_video* video_dst, const aet_video* video_src,
    prj::shared_ptr<prj::stack_allocator> alloc) {
    video_dst->color[0] = video_src->color[0];
    video_dst->color[1] = video_src->color[1];
    video_dst->color[2] = video_src->color[2];
    video_dst->width = video_src->width;
    video_dst->height = video_src->height;
    video_dst->fpf = video_src->fpf;

    uint32_t sources_count = video_src->sources_count;
    const aet_video_src* sources_src = video_src->sources;
    aet_video_src* sources_dst = alloc->allocate<aet_video_src>(sources_count);

    for (uint32_t i = 0; i < sources_count; i++)
        aet_move_data_video_src(&sources_dst[i], &sources_src[i], alloc);

    video_dst->sources = sources_dst;
    video_dst->sources_count = sources_count;
}

static void aet_move_data_video_src(aet_video_src* video_dst, const aet_video_src* video_src,
    prj::shared_ptr<prj::stack_allocator> alloc) {
    video_dst->sprite_name = aet_move_data_string(video_src->sprite_name, alloc);
    video_dst->sprite_index = video_src->sprite_index;
}

static void aet_set_classic_read_inner(aet_set* as, prj::shared_ptr<prj::stack_allocator>& alloc, stream& s) {
    uint32_t scenes_count = 0;
    while (s.read_uint32_t())
        scenes_count++;
    as->scenes_count = scenes_count;

    aet_scene** scenes = alloc->allocate<aet_scene*>(scenes_count);
    as->scenes = (const aet_scene**)scenes;

    uint32_t* data = force_malloc<uint32_t>(scenes_count);
    s.set_position(0, SEEK_SET);
    for (uint32_t i = 0; i < scenes_count; i++)
        data[i] = s.read_uint32_t();

    std::unordered_map<int64_t, aet_item> items;
    std::unordered_map<int64_t, const aet_layer*> layers;

    for (uint32_t i = 0; i < scenes_count; i++)
        scenes[i] = aet_classic_read_scene(alloc, s, data[i], layers, items);

    for (uint32_t i = 0; i < scenes_count; i++) {
        aet_scene* scene = scenes[i];
        const aet_comp* comp = scene->comp;
        for (uint32_t j = scene->comp_count; j; j--, comp++)
            aet_comp_set_item_parent(comp, layers, items);
    }

    free_def(data);

    as->ready = true;
    as->modern = false;
    as->big_endian = false;
    as->is_x = false;
}

static void aet_set_classic_write_inner(aet_set* as, stream& s) {
    for (uint32_t i = align_val(as->scenes_count + 1, 8); i; i--)
        s.write_uint32_t(0x90669066);

    std::unordered_map<std::string, int64_t> strings;

    std::unordered_map<const void*, int64_t> offsets;

    uint32_t scenes_count = as->scenes_count;
    const aet_scene** scenes = as->scenes;

    for (uint32_t i = 0; i < scenes_count; i++) {
        const aet_scene* scene = scenes[i];

        const aet_video* video = scene->video;
        for (uint32_t j = scene->video_count; j; j--, video++) {
            if (video->sources_count == 0) {
                offsets[video->sources] = 0;
                continue;
            }

            if (video->sources_count > 1)
                s.align_write(0x20);
            offsets[video->sources] = s.get_position();
            s.write(0x08ULL * video->sources_count);
        }

        if (scene->video_count > 1)
            s.align_write(0x20);
        offsets[scene->video] = s.get_position();
        video = scene->video;
        for (uint32_t j = scene->video_count; j; j--, video++) {
            offsets[video] = s.get_position();

            s.write_uint8_t(video->color[0]);
            s.write_uint8_t(video->color[1]);
            s.write_uint8_t(video->color[2]);
            s.write(0x01);
            s.write_uint16_t(video->width);
            s.write_uint16_t(video->height);
            s.write_float_t(video->fpf);
            s.write_uint32_t(video->sources_count);
            s.write_uint32_t(video->sources ? (uint32_t)offsets[video->sources] : 0);
        }

        if (scene->comp_count > 1)
            s.align_write(0x20);

        offsets[scene->comp] = s.get_position();
        const aet_comp* comp = scene->comp;
        for (uint32_t j = scene->comp_count; j; j--, comp++) {
            offsets[comp] = s.get_position();
            s.write(0x08);
        }

        comp = scene->comp;
        for (uint32_t j = scene->comp_count; j; j--, comp++) {
            if (comp->layers_count < 1)
                continue;

            const aet_layer* layer = comp->layers;
            for (uint32_t k = comp->layers_count; k; k--, layer++) {
                if (layer->markers) {
                    if (layer->markers_count > 3)
                        s.align_write(0x20);
                    offsets[layer->markers] = s.get_position();
                    s.write(0x08ULL * layer->markers_count);
                }

                const aet_layer_video* layer_video = layer->video;
                if (layer_video) {
                    const aet_layer_video_3d* layer_video_3d = layer_video->_3d;
                    aet_classic_write_layer_video(layer_video, s, offsets);

                    if (layer_video_3d) {
                        s.align_write(0x20);
                        offsets[layer_video_3d] = s.get_position();
                        s.write(0x40);
                    }
                }

                const aet_layer_audio* layer_audio = layer->audio;
                aet_classic_write_layer_audio(layer_audio, s, offsets);

                if (layer_video) {
                    s.align_write(0x20);
                    offsets[layer_video] = s.get_position();
                    s.write(0x48);
                }

                if (layer_audio) {
                    s.align_write(0x20);
                    offsets[layer_audio] = s.get_position();
                    s.write(0x20);
                }
            }

            s.align_write(0x20);
            offsets[comp->layers] = s.get_position();
            layer = comp->layers;
            for (uint32_t k = comp->layers_count; k; k--, layer++) {
                offsets[layer] = s.get_position();
                s.write(0x30);
            }
        }

        aet_classic_write_camera(scene->camera, s, offsets);

        s.align_write(0x20);
        offsets[scene] = s.get_position();
        s.write(0x40);

        s.align_write(0x10);
    }

    for (uint32_t i = 0; i < scenes_count; i++) {
        const aet_scene* scene = scenes[i];

        const aet_video* video = scene->video;
        for (uint32_t j = scene->video_count; j; j--, video++)
            for (uint32_t k = 0; k < video->sources_count; k++)
                aet_strings_push_back_check(s, strings, video->sources[k].sprite_name);

        const aet_comp* comp = scene->comp;
        for (uint32_t j = scene->comp_count; j; j--, comp++) {
            const aet_layer* layer = comp->layers;
            for (uint32_t k = comp->layers_count; k; k--, layer++) {
                const aet_marker* marker = layer->markers;
                for (uint32_t l = layer->markers_count; l; l--, marker++)
                    aet_strings_push_back_check(s, strings, marker->name);
            }

            layer = comp->layers;
            for (uint32_t k = comp->layers_count; k; k--, layer++)
                aet_strings_push_back_check(s, strings, layer->name);
        }

        aet_strings_push_back_check(s, strings, scene->name);
        s.align_write(0x04);
    }

    for (uint32_t i = 0; i < scenes_count; i++) {
        const aet_scene* scene = scenes[i];

        if (scene->audio) {
            offsets[scene->audio] = s.get_position();
            const aet_audio* audio = scene->audio;
            for (uint32_t j = scene->audio_count; j; j--, audio++)
                aet_classic_write_audio(&scene->audio[j], s, offsets);
        }
    }

    int64_t null_data_pos = s.get_position();
    uint32_t null_data_count = 0;
    for (uint32_t i = 0; i < scenes_count; i++) {
        const aet_scene* scene = scenes[i];

        const aet_comp* comp = scene->comp;
        for (uint32_t j = scene->comp_count; j; j--, comp++) {
            if (comp->layers_count < 1)
                continue;

            const aet_layer* layer = comp->layers;
            for (uint32_t k = comp->layers_count; k; k--, layer++) {
                const aet_layer_video* layer_video = layer->video;
                const aet_layer_audio* layer_audio = layer->audio;
                if (layer_video) {
                    const aet_layer_video_3d* layer_video_3d = layer_video->_3d;
                    if (layer_video_3d) {
                        s.position_push(offsets[layer_video_3d], SEEK_SET);
                        aet_classic_write_fcurve_header(&layer_video_3d->anchor_z,
                            s, offsets, null_data_pos, null_data_count);
                        aet_classic_write_fcurve_header(&layer_video_3d->pos_z,
                            s, offsets, null_data_pos, null_data_count);
                        aet_classic_write_fcurve_header(&layer_video_3d->dir_x,
                            s, offsets, null_data_pos, null_data_count);
                        aet_classic_write_fcurve_header(&layer_video_3d->dir_y,
                            s, offsets, null_data_pos, null_data_count);
                        aet_classic_write_fcurve_header(&layer_video_3d->dir_z,
                            s, offsets, null_data_pos, null_data_count);
                        aet_classic_write_fcurve_header(&layer_video_3d->rot_x,
                            s, offsets, null_data_pos, null_data_count);
                        aet_classic_write_fcurve_header(&layer_video_3d->rot_y,
                            s, offsets, null_data_pos, null_data_count);
                        aet_classic_write_fcurve_header(&layer_video_3d->scale_z,
                            s, offsets, null_data_pos, null_data_count);
                        s.position_pop();
                    }

                    s.position_push(offsets[layer_video], SEEK_SET);
                    s.write_uint8_t((uint8_t)layer_video->transfer_mode.mode);
                    s.write_uint8_t((uint8_t)layer_video->transfer_mode.flag);
                    s.write_uint8_t((uint8_t)layer_video->transfer_mode.matte);
                    s.write(0x01);

                    aet_classic_write_fcurve_header(&layer_video->anchor_x,
                        s, offsets, null_data_pos, null_data_count);
                    aet_classic_write_fcurve_header(&layer_video->anchor_y,
                        s, offsets, null_data_pos, null_data_count);
                    aet_classic_write_fcurve_header(&layer_video->pos_x,
                        s, offsets, null_data_pos, null_data_count);
                    aet_classic_write_fcurve_header(&layer_video->pos_y,
                        s, offsets, null_data_pos, null_data_count);
                    aet_classic_write_fcurve_header(&layer_video->rot_z,
                        s, offsets, null_data_pos, null_data_count);
                    aet_classic_write_fcurve_header(&layer_video->scale_x,
                        s, offsets, null_data_pos, null_data_count);
                    aet_classic_write_fcurve_header(&layer_video->scale_y,
                        s, offsets, null_data_pos, null_data_count);
                    aet_classic_write_fcurve_header(&layer_video->opacity,
                        s, offsets, null_data_pos, null_data_count);
                    s.write_uint32_t(layer_video_3d ? (uint32_t)offsets[layer_video_3d] : 0);
                    s.position_pop();
                }

                if (layer_audio) {
                    s.position_push(offsets[layer_audio], SEEK_SET);
                    aet_classic_write_fcurve_header(&layer_audio->volume_l,
                        s, offsets, null_data_pos, null_data_count);
                    aet_classic_write_fcurve_header(&layer_audio->volume_r,
                        s, offsets, null_data_pos, null_data_count);
                    aet_classic_write_fcurve_header(&layer_audio->pan_l,
                        s, offsets, null_data_pos, null_data_count);
                    aet_classic_write_fcurve_header(&layer_audio->pan_r,
                        s, offsets, null_data_pos, null_data_count);
                    s.position_pop();
                }
            }
        }
    }

    s.write(sizeof(uint32_t) * null_data_count);
    s.align_write(0x10);

    for (uint32_t i = 0; i < scenes_count; i++) {
        const aet_scene* scene = scenes[i];
        const aet_comp* comp = scene->comp;
        for (uint32_t j = scene->comp_count; j; j--, comp++) {
            const aet_layer* layer = comp->layers;
            for (uint32_t k = comp->layers_count; k; k--, layer++) {
                s.set_position(offsets[layer], SEEK_SET);
                s.write_uint32_t((uint32_t)aet_strings_get_string_offset(strings, layer->name));
                s.write_float_t(layer->start_time);
                s.write_float_t(layer->end_time);
                s.write_float_t(layer->offset_time);
                s.write_float_t(layer->time_scale);
                s.write_uint16_t(*(uint16_t*)&layer->flags);
                s.write_uint8_t((uint8_t)layer->quality);
                s.write_uint8_t((uint8_t)layer->item_type);
                if (layer->item_type == AET_ITEM_TYPE_VIDEO
                    || layer->item_type == AET_ITEM_TYPE_COMPOSITION)
                    s.write_uint32_t((uint32_t)offsets[layer->item.none]);
                else
                    s.write(0x04);

                s.write_uint32_t(layer->parent ? (uint32_t)offsets[layer->parent] : 0);
                s.write_uint32_t(layer->markers_count);
                s.write_uint32_t(layer->markers ? (uint32_t)offsets[layer->markers] : 0);
                s.write_uint32_t(layer->video ? (uint32_t)offsets[layer->video] : 0);
                s.write_uint32_t(layer->audio ? (uint32_t)offsets[layer->audio] : 0);

                if (layer->markers) {
                    s.set_position(offsets[layer->markers], SEEK_SET);
                    const aet_marker* marker = layer->markers;
                    for (uint32_t l = layer->markers_count; l; l--, marker++) {
                        s.write_float_t(marker->time);
                        s.write_uint32_t((uint32_t)aet_strings_get_string_offset(strings, marker->name));
                    }
                }
            }
        }

        const aet_video* video = scene->video;
        for (uint32_t j = scene->video_count; j; j--, video++) {
            if (!video->sources)
                continue;

            s.set_position(offsets[video->sources], SEEK_SET);
            const aet_video_src* source = video->sources;
            for (uint32_t k = video->sources_count; k; k--, source++) {
                s.write_uint32_t((uint32_t)aet_strings_get_string_offset(strings, source->sprite_name));
                s.write_uint32_t(source->sprite_index);
            }
        }

        s.set_position(offsets[scene->comp], SEEK_SET);
        comp = scene->comp;
        for (uint32_t j = scene->comp_count; j; j--, comp++) {
            if (comp->layers) {
                s.write_uint32_t(comp->layers_count);
                s.write_uint32_t((uint32_t)offsets[comp->layers]);
            }
            else
                s.write(0x08);
        }

        s.set_position(offsets[scene], SEEK_SET);
        s.write_uint32_t((uint32_t)aet_strings_get_string_offset(strings, scene->name));
        s.write_float_t(scene->start_time);
        s.write_float_t(scene->end_time);
        s.write_float_t(scene->fps);
        s.write_uint8_t(scene->color[0]);
        s.write_uint8_t(scene->color[1]);
        s.write_uint8_t(scene->color[2]);
        s.write_uint8_t(0x00);
        s.write_uint32_t(scene->width);
        s.write_uint32_t(scene->height);
        s.write_uint32_t(scene->camera ? (uint32_t)offsets[scene->camera] : 0);
        s.write_uint32_t(scene->comp_count);
        s.write_uint32_t(scene->comp ? (uint32_t)offsets[scene->comp] : 0);
        s.write_uint32_t(scene->video_count);
        s.write_uint32_t(scene->video ? (uint32_t)offsets[scene->video] : 0);
        s.write_uint32_t(scene->audio_count);
        s.write_uint32_t(scene->audio ? (uint32_t)offsets[scene->audio] : 0);
        s.write(0x08);
    }

    s.position_push(0x00, SEEK_SET);
    for (uint32_t i = 0; i < scenes_count; i++)
        s.write_uint32_t((uint32_t)offsets[scenes[i]]);
    s.write(0x04);
    s.position_pop();
}

static void aet_set_modern_read_inner(aet_set* as, prj::shared_ptr<prj::stack_allocator>& alloc, stream& s) {
    f2_struct st;
    st.read(s);
    if (st.header.signature != reverse_endianness_uint32_t('AETC') || !st.data.size())
        return;

    uint32_t header_length = st.header.get_length();
    bool big_endian = st.header.attrib.get_big_endian();
    bool is_x = st.pof.shift_x;

    memory_stream s_aetc;
    s_aetc.open(st.data);
    s_aetc.big_endian = big_endian;

    uint32_t scenes_count = 0;
    if (!is_x)
        while (s_aetc.read_uint32_t_reverse_endianness())
            scenes_count++;
    else
        while (s_aetc.read_int64_t_reverse_endianness())
            scenes_count++;
    as->scenes_count = scenes_count;

    aet_scene** scenes = alloc->allocate<aet_scene*>(scenes_count);
    as->scenes = (const aet_scene**)scenes;

    int64_t* data = force_malloc<int64_t>(scenes_count);
    s_aetc.set_position(0, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < scenes_count; i++)
            data[i] = s_aetc.read_uint32_t_reverse_endianness() - header_length;
    else
        for (uint32_t i = 0; i < scenes_count; i++)
            data[i] = s_aetc.read_int64_t_reverse_endianness();

    std::unordered_map<int64_t, aet_item> items;
    std::unordered_map<int64_t, const aet_layer*> layers;

    for (uint32_t i = 0; i < scenes_count; i++)
        scenes[i] = aet_modern_read_scene(alloc, s_aetc, data[i],
            header_length, is_x, layers, items);

    for (uint32_t i = 0; i < scenes_count; i++) {
        aet_scene* scene = scenes[i];
        const aet_comp* comp = scene->comp;
        for (uint32_t j = scene->comp_count; j; j--, comp++)
            aet_comp_set_item_parent(comp, layers, items);
    }

    free_def(data);

    as->ready = true;
    as->modern = true;
    as->big_endian = big_endian;
    as->is_x = is_x;
}

static void aet_set_modern_write_inner(aet_set* as, stream& s) {
    bool big_endian = as->big_endian;
    bool is_x = as->is_x;

    memory_stream s_aetc;
    s_aetc.open();
    s_aetc.big_endian = big_endian;

    uint32_t off;
    enrs e;
    enrs_entry ee;
    pof pof;

    off = align_val(0, 0x10);

    f2_struct st;
    s_aetc.align_write(0x10);
    s_aetc.copy(st.data);
    s_aetc.close();

    st.enrs = e;
    st.pof = pof;

    new (&st.header) f2_header('AETC');
    st.header.attrib.set_big_endian(big_endian);

    st.write(s, true, is_x);
}

static void aet_classic_read_audio(aet_audio* audio,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s,
    std::unordered_map<int64_t, aet_item>& items) {
    items[s.get_position()] = audio;
    audio->sound_index = s.read_int32_t();
}

static void aet_classic_write_audio(const aet_audio* audio, stream& s,
    std::unordered_map<const void*, int64_t>& offsets) {
    offsets[audio] = s.get_position();
    s.write_int32_t(audio->sound_index);
}

static aet_camera* aet_classic_read_camera(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t offset) {
    if (!offset)
        return 0;

    aet_camera* cam = alloc->allocate<aet_camera>();
    s.position_push(offset, SEEK_SET);
    aet_classic_read_fcurve(&cam->eye_x, alloc, s);
    aet_classic_read_fcurve(&cam->eye_y, alloc, s);
    aet_classic_read_fcurve(&cam->eye_z, alloc, s);
    aet_classic_read_fcurve(&cam->pos_x, alloc, s);
    aet_classic_read_fcurve(&cam->pos_y, alloc, s);
    aet_classic_read_fcurve(&cam->pos_z, alloc, s);
    aet_classic_read_fcurve(&cam->dir_x, alloc, s);
    aet_classic_read_fcurve(&cam->dir_y, alloc, s);
    aet_classic_read_fcurve(&cam->dir_z, alloc, s);
    aet_classic_read_fcurve(&cam->rot_x, alloc, s);
    aet_classic_read_fcurve(&cam->rot_y, alloc, s);
    aet_classic_read_fcurve(&cam->rot_z, alloc, s);
    aet_classic_read_fcurve(&cam->zoom, alloc, s);
    s.position_pop();
    return cam;
}

static void aet_classic_write_camera(const aet_camera* cam, stream& s,
    std::unordered_map<const void*, int64_t>& offsets) {
    if (!cam)
        return;

    s.align_write(0x10);
    aet_classic_write_fcurve(&cam->eye_x, s, offsets);
    aet_classic_write_fcurve(&cam->eye_y, s, offsets);
    aet_classic_write_fcurve(&cam->eye_z, s, offsets);
    aet_classic_write_fcurve(&cam->pos_x, s, offsets);
    aet_classic_write_fcurve(&cam->pos_y, s, offsets);
    aet_classic_write_fcurve(&cam->pos_z, s, offsets);
    aet_classic_write_fcurve(&cam->dir_x, s, offsets);
    aet_classic_write_fcurve(&cam->dir_y, s, offsets);
    aet_classic_write_fcurve(&cam->dir_z, s, offsets);
    aet_classic_write_fcurve(&cam->rot_x, s, offsets);
    aet_classic_write_fcurve(&cam->rot_y, s, offsets);
    aet_classic_write_fcurve(&cam->rot_z, s, offsets);
    aet_classic_write_fcurve(&cam->zoom, s, offsets);

    s.align_write(0x20);
    offsets[cam] = s.get_position();
    s.write(0x34);
}

static void aet_classic_read_comp(aet_comp* comp,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s,
    std::unordered_map<int64_t, const aet_layer*>& layers,
    std::unordered_map<int64_t, aet_item>& items) {
    items[s.get_position()] = comp;

    comp->layers_count = s.read_uint32_t();
    uint32_t layers_offset = s.read_uint32_t();

    if (layers_offset) {
        aet_layer* layer = alloc->allocate<aet_layer>(comp->layers_count);
        comp->layers = layer;

        s.position_push(layers_offset, SEEK_SET);
        for (uint32_t i = comp->layers_count; i; i--, layer++) {
            layers[s.get_position()] = layer;
            aet_classic_read_layer(layer, alloc, s);
        }
        s.position_pop();
    }
    else
        comp->layers = 0;
}

static void aet_classic_read_fcurve(aet_fcurve* fcurve,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s) {
    fcurve->keys_count = s.read_uint32_t();
    uint32_t keys_offset = s.read_uint32_t();

    s.position_push(keys_offset, SEEK_SET);
    if (fcurve->keys_count > 1) {
        float_t* key = alloc->allocate<float_t>(fcurve->keys_count * 3ULL);
        fcurve->keys = key;

        s.read(key, fcurve->keys_count * 3ULL * sizeof(float_t));
    }
    else if (fcurve->keys_count) {
        float_t* key = alloc->allocate<float_t>();
        fcurve->keys = key;

        *key = s.read_float_t();
    }
    else
        fcurve->keys = 0;
    s.position_pop();
}

static void aet_classic_write_fcurve(const aet_fcurve* fcurve, stream& s,
    std::unordered_map<const void*, int64_t>& offsets) {
    if (fcurve->keys_count < 1)
        return;
    else if (fcurve->keys_count == 1) {
        const float_t* key = fcurve->keys;
        if (*(const uint32_t*)key) {
            offsets[fcurve] = s.get_position();
            s.write_float_t(*key);
        }
    }
    else {
        if (fcurve->keys_count > 2)
            s.align_write(0x20);

        offsets[fcurve] = s.get_position();
        s.write(fcurve->keys, fcurve->keys_count * 3ULL * sizeof(float_t));
    }
}

static void aet_classic_write_fcurve_header(const aet_fcurve* fcurve, stream& s,
    std::unordered_map<const void*, int64_t>& offsets, int64_t null_data_pos, uint32_t& null_data_count) {
    if (fcurve->keys_count < 1)
        s.write(0x08);
    else if (fcurve->keys_count == 1 && !*(const uint32_t*)fcurve->keys) {
        int64_t offset = null_data_pos + null_data_count * sizeof(float_t);
        offsets[fcurve] = offset;

        s.write_uint32_t(fcurve->keys_count);
        s.write_uint32_t((uint32_t)offset);
        null_data_count++;
    }
    else {
        auto elem = offsets.find(fcurve);
        if (elem != offsets.end()) {
            s.write_uint32_t(fcurve->keys_count);
            s.write_uint32_t((uint32_t)elem->second);
        }
        else
            s.write(0x08);
    }
}

static void aet_classic_read_layer(aet_layer* layer,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s) {
    aet_layer_header alh = {};

    layer->name = aet_read_utf8_string_null_terminated_offset(
        alloc, s, s.read_uint32_t());
    layer->start_time = s.read_float_t();
    layer->end_time = s.read_float_t();
    layer->offset_time = s.read_float_t();
    layer->time_scale = s.read_float_t();
    *(uint16_t*)&layer->flags = s.read_uint16_t();
    layer->quality = (aet_layer_quality)s.read_uint8_t();
    layer->item_type = (aet_item_type)s.read_uint8_t();
    if (layer->item_type == AET_ITEM_TYPE_VIDEO
        || layer->item_type == AET_ITEM_TYPE_COMPOSITION)
        layer->item.none = (void*)(size_t)s.read_uint32_t();
    else
        s.read_uint32_t();

    layer->parent = (const aet_layer*)(size_t)s.read_uint32_t();
    layer->markers_count = s.read_uint32_t();
    alh.markers_offset = s.read_uint32_t();
    alh.video_offset = s.read_uint32_t();
    alh.audio_offset = s.read_uint32_t();

    if (alh.markers_offset) {
        aet_marker* marker = alloc->allocate<aet_marker>(layer->markers_count);
        layer->markers = marker;

        s.position_push(alh.markers_offset, SEEK_SET);
        for (uint32_t i = layer->markers_count; i; i--, marker++) {
            marker->time = s.read_float_t();
            marker->name = aet_read_utf8_string_null_terminated_offset(
                alloc, s, s.read_uint32_t());
        }
        s.position_pop();
    }
    else
        layer->markers = 0;

    layer->video = aet_classic_read_layer_video(alloc, s, alh.video_offset);
    layer->audio = aet_classic_read_layer_audio(alloc, s, alh.audio_offset);
}

static void aet_classic_write_layer(const aet_layer* layer, stream& s,
    std::unordered_map<const void*, int64_t>& offsets) {

}

static aet_layer_audio* aet_classic_read_layer_audio(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t offset) {
    if (!offset)
        return 0;

    aet_layer_audio* layer_audio = alloc->allocate<aet_layer_audio>();
    s.position_push(offset, SEEK_SET);
    aet_classic_read_fcurve(&layer_audio->volume_l, alloc, s);
    aet_classic_read_fcurve(&layer_audio->volume_r, alloc, s);
    aet_classic_read_fcurve(&layer_audio->pan_l, alloc, s);
    aet_classic_read_fcurve(&layer_audio->pan_r, alloc, s);
    s.position_pop();
    return layer_audio;
}

static void aet_classic_write_layer_audio(const aet_layer_audio* layer_audio, stream& s,
    std::unordered_map<const void*, int64_t>& offsets) {
    if (!layer_audio)
        return;

    aet_classic_write_fcurve(&layer_audio->volume_l, s, offsets);
    aet_classic_write_fcurve(&layer_audio->volume_r, s, offsets);
    aet_classic_write_fcurve(&layer_audio->pan_l, s, offsets);
    aet_classic_write_fcurve(&layer_audio->pan_r, s, offsets);
}

static aet_layer_video* aet_classic_read_layer_video(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t offset) {
    if (!offset)
        return 0;

    aet_layer_video* layer_video = alloc->allocate<aet_layer_video>();
    s.position_push(offset, SEEK_SET);
    layer_video->transfer_mode.mode = (aet_blend_mode)s.read_uint8_t();
    layer_video->transfer_mode.flag = s.read_uint8_t();
    layer_video->transfer_mode.matte = s.read_uint8_t();
    s.read_uint8_t();
    aet_classic_read_fcurve(&layer_video->anchor_x, alloc, s);
    aet_classic_read_fcurve(&layer_video->anchor_y, alloc, s);
    aet_classic_read_fcurve(&layer_video->pos_x, alloc, s);
    aet_classic_read_fcurve(&layer_video->pos_y, alloc, s);
    aet_classic_read_fcurve(&layer_video->rot_z, alloc, s);
    aet_classic_read_fcurve(&layer_video->scale_x, alloc, s);
    aet_classic_read_fcurve(&layer_video->scale_y, alloc, s);
    aet_classic_read_fcurve(&layer_video->opacity, alloc, s);
    layer_video->_3d = aet_classic_read_layer_video_3d(alloc, s, s.read_uint32_t());
    s.position_pop();
    return layer_video;
}

static void aet_classic_write_layer_video(const aet_layer_video* layer_video, stream& s,
    std::unordered_map<const void*, int64_t>& offsets) {
    if (!layer_video)
        return;

    aet_classic_write_layer_video_3d(layer_video->_3d, s, offsets);

    aet_classic_write_fcurve(&layer_video->anchor_x, s, offsets);
    aet_classic_write_fcurve(&layer_video->anchor_y, s, offsets);
    aet_classic_write_fcurve(&layer_video->pos_x, s, offsets);
    aet_classic_write_fcurve(&layer_video->pos_y, s, offsets);
    aet_classic_write_fcurve(&layer_video->rot_z, s, offsets);
    aet_classic_write_fcurve(&layer_video->scale_x, s, offsets);
    aet_classic_write_fcurve(&layer_video->scale_y, s, offsets);
    aet_classic_write_fcurve(&layer_video->opacity, s, offsets);
}

static aet_layer_video_3d* aet_classic_read_layer_video_3d(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t offset) {
    if (!offset)
        return 0;

    aet_layer_video_3d* layer_video_3d = alloc->allocate<aet_layer_video_3d>();
    s.position_push(offset, SEEK_SET);
    aet_classic_read_fcurve(&layer_video_3d->anchor_z, alloc, s);
    aet_classic_read_fcurve(&layer_video_3d->pos_z, alloc, s);
    aet_classic_read_fcurve(&layer_video_3d->dir_x, alloc, s);
    aet_classic_read_fcurve(&layer_video_3d->dir_y, alloc, s);
    aet_classic_read_fcurve(&layer_video_3d->dir_z, alloc, s);
    aet_classic_read_fcurve(&layer_video_3d->rot_x, alloc, s);
    aet_classic_read_fcurve(&layer_video_3d->rot_y, alloc, s);
    aet_classic_read_fcurve(&layer_video_3d->scale_z, alloc, s);
    s.position_pop();
    return layer_video_3d;
}

static void aet_classic_write_layer_video_3d(const aet_layer_video_3d* layer_video_3d, stream& s,
    std::unordered_map<const void*, int64_t>& offsets) {
    if (!layer_video_3d)
        return;

    aet_classic_write_fcurve(&layer_video_3d->anchor_z, s, offsets);
    aet_classic_write_fcurve(&layer_video_3d->pos_z, s, offsets);
    aet_classic_write_fcurve(&layer_video_3d->dir_x, s, offsets);
    aet_classic_write_fcurve(&layer_video_3d->dir_y, s, offsets);
    aet_classic_write_fcurve(&layer_video_3d->dir_z, s, offsets);
    aet_classic_write_fcurve(&layer_video_3d->rot_x, s, offsets);
    aet_classic_write_fcurve(&layer_video_3d->rot_y, s, offsets);
    aet_classic_write_fcurve(&layer_video_3d->scale_z, s, offsets);
}

static aet_scene* aet_classic_read_scene(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t offset, std::unordered_map<int64_t, const aet_layer*>& layers,
    std::unordered_map<int64_t, aet_item>& items) {
    if (!offset)
        return 0;

    aet_scene_header ash = {};

    aet_scene* scene = alloc->allocate<aet_scene>();
    s.position_push(offset, SEEK_SET);
    scene->name = aet_read_utf8_string_null_terminated_offset(
        alloc, s, s.read_uint32_t());
    scene->start_time = s.read_float_t();
    scene->end_time = s.read_float_t();
    scene->fps = s.read_float_t();
    scene->color[0] = s.read_uint8_t();
    scene->color[1] = s.read_uint8_t();
    scene->color[2] = s.read_uint8_t();
    s.read_uint8_t();
    scene->width = s.read_uint32_t();
    scene->height = s.read_uint32_t();
    ash.camera_offset = s.read_uint32_t();
    scene->comp_count = s.read_uint32_t();
    ash.comp_offset = s.read_uint32_t();
    scene->video_count = s.read_uint32_t();
    ash.video_offset = s.read_uint32_t();
    scene->audio_count = s.read_uint32_t();
    ash.audio_offset = s.read_uint32_t();
    s.position_pop();

    scene->camera = aet_classic_read_camera(alloc, s, ash.camera_offset);

    if (ash.comp_offset) {
        aet_comp* comp = alloc->allocate<aet_comp>(scene->comp_count);
        scene->comp = comp;

        s.position_push(ash.comp_offset, SEEK_SET);
        for (uint32_t i = scene->comp_count; i; i--, comp++)
            aet_classic_read_comp(comp, alloc, s, layers, items);
        s.position_pop();
    }
    else
        scene->comp = 0;

    if (ash.video_offset) {
        aet_video* video = alloc->allocate<aet_video>(scene->video_count);
        scene->video = video;

        s.position_push(ash.video_offset, SEEK_SET);
        for (uint32_t i = scene->video_count; i; i--, video++)
            aet_classic_read_video(video, alloc, s, items);
        s.position_pop();
    }
    else
        scene->video = 0;

    if (ash.audio_offset) {
        aet_audio* audio = alloc->allocate<aet_audio>(scene->audio_count);
        scene->audio = audio;

        s.position_push(ash.audio_offset, SEEK_SET);
        for (uint32_t i = scene->audio_count; i; i--, audio++)
            aet_classic_read_audio(audio, alloc, s, items);
        s.position_pop();
    }
    else
        scene->audio = 0;
    return scene;
}

static void aet_classic_read_video(aet_video* video,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s,
    std::unordered_map<int64_t, aet_item>& items) {
    items[s.get_position()] = video;

    video->color[0] = s.read_uint8_t();
    video->color[1] = s.read_uint8_t();
    video->color[2] = s.read_uint8_t();
    s.read_uint8_t();
    video->width = s.read_uint16_t();
    video->height = s.read_uint16_t();
    video->fpf = s.read_float_t();
    video->sources_count = s.read_uint32_t();
    uint32_t sources_offset = s.read_uint32_t();

    if (sources_offset) {
        aet_video_src* source = alloc->allocate<aet_video_src>(video->sources_count);
        video->sources = source;

        s.position_push(sources_offset, SEEK_SET);
        for (uint32_t i = video->sources_count; i; i--, source++) {
            source->sprite_name = aet_read_utf8_string_null_terminated_offset(
                alloc, s, s.read_uint32_t());
            source->sprite_index = s.read_int32_t();
        }
        s.position_pop();
    }
    else
        video->sources = 0;
}

static void aet_modern_read_audio(aet_audio* audio,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s,
    uint32_t header_length, bool is_x) {
    audio->sound_index = s.read_int32_t();
}

static aet_camera* aet_modern_read_camera(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t offset,
    uint32_t header_length, bool is_x) {
    if (!offset)
        return 0;

    aet_camera* cam = alloc->allocate<aet_camera>();
    s.position_push(offset, SEEK_SET);
    aet_modern_read_fcurve(&cam->eye_x, alloc, s, header_length, is_x);
    aet_modern_read_fcurve(&cam->eye_y, alloc, s, header_length, is_x);
    aet_modern_read_fcurve(&cam->eye_z, alloc, s, header_length, is_x);
    aet_modern_read_fcurve(&cam->pos_x, alloc, s, header_length, is_x);
    aet_modern_read_fcurve(&cam->pos_y, alloc, s, header_length, is_x);
    aet_modern_read_fcurve(&cam->pos_z, alloc, s, header_length, is_x);
    aet_modern_read_fcurve(&cam->dir_x, alloc, s, header_length, is_x);
    aet_modern_read_fcurve(&cam->dir_y, alloc, s, header_length, is_x);
    aet_modern_read_fcurve(&cam->dir_z, alloc, s, header_length, is_x);
    aet_modern_read_fcurve(&cam->rot_x, alloc, s, header_length, is_x);
    aet_modern_read_fcurve(&cam->rot_y, alloc, s, header_length, is_x);
    aet_modern_read_fcurve(&cam->rot_z, alloc, s, header_length, is_x);
    aet_modern_read_fcurve(&cam->zoom, alloc, s, header_length, is_x);
    s.position_pop();
    return cam;
}

static void aet_modern_read_comp(aet_comp* comp,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s,
    uint32_t header_length, bool is_x,
    std::unordered_map<int64_t, const aet_layer*>& layers, std::unordered_map<int64_t, aet_item>& items) {
    items[s.get_position()] = comp;

    comp->layers_count = s.read_uint32_t_reverse_endianness();
    int64_t layers_offset = s.read_offset(header_length, is_x);

    if (layers_offset) {
        aet_layer* layer = alloc->allocate<aet_layer>(comp->layers_count);
        comp->layers = layer;

        s.position_push(layers_offset, SEEK_SET);
        for (uint32_t i = comp->layers_count; i; i--, layer++) {
            layers[s.get_position()] = layer;
            aet_modern_read_layer(layer, alloc, s, header_length, is_x);
        }
        s.position_pop();
    }
    else
        comp->layers = 0;
}

static void aet_modern_read_fcurve(aet_fcurve* fcurve,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s,
    uint32_t header_length, bool is_x) {
    fcurve->keys_count = s.read_uint32_t_reverse_endianness();
    int64_t keys_offset = s.read_offset(header_length, is_x);

    s.position_push(keys_offset, SEEK_SET);
    if (fcurve->keys_count > 1) {
        float_t* key = alloc->allocate<float_t>(fcurve->keys_count * 3ULL);
        fcurve->keys = key;

        s.read(key, fcurve->keys_count * 3ULL * sizeof(float_t));
    }
    else if (fcurve->keys_count) {
        float_t* key = alloc->allocate<float_t>();
        fcurve->keys = key;

        *key = s.read_float_t_reverse_endianness();
    }
    else
        fcurve->keys = 0;
    s.position_pop();
}

static void aet_modern_read_layer(aet_layer* layer,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s,
    uint32_t header_length, bool is_x) {
    aet_layer_header alh = {};

    if (!is_x) {
        layer->name = aet_read_utf8_string_null_terminated_offset(
            alloc, s, s.read_offset_f2(header_length));
        layer->start_time = s.read_float_t_reverse_endianness();
        layer->end_time = s.read_float_t_reverse_endianness();
        layer->offset_time = s.read_float_t_reverse_endianness();
        layer->time_scale = s.read_float_t_reverse_endianness();
        *(uint16_t*)&layer->flags = s.read_uint16_t_reverse_endianness();
        layer->quality = (aet_layer_quality)s.read_uint8_t();
        layer->item_type = (aet_item_type)s.read_uint8_t();
        if (layer->item_type == AET_ITEM_TYPE_VIDEO
            || layer->item_type == AET_ITEM_TYPE_COMPOSITION)
            layer->item.none = (void*)(size_t)s.read_offset_f2(header_length);
        else
            s.read_offset_f2(header_length);

        layer->parent = (const aet_layer*)(size_t)s.read_offset_f2(header_length);
        layer->markers_count = s.read_uint32_t_reverse_endianness();
        alh.markers_offset = s.read_offset_f2(header_length);
        alh.video_offset = s.read_offset_f2(header_length);
        alh.audio_offset = s.read_offset_f2(header_length);
    }
    else {
        layer->name = aet_read_utf8_string_null_terminated_offset(
            alloc, s, s.read_offset_x());
        layer->start_time = s.read_float_t_reverse_endianness();
        layer->end_time = s.read_float_t_reverse_endianness();
        layer->offset_time = s.read_float_t_reverse_endianness();
        layer->time_scale = s.read_float_t_reverse_endianness();
        *(uint16_t*)&layer->flags = s.read_uint16_t_reverse_endianness();
        layer->quality = (aet_layer_quality)s.read_uint8_t();
        layer->item_type = (aet_item_type)s.read_uint8_t();
        if (layer->item_type == AET_ITEM_TYPE_VIDEO
            || layer->item_type == AET_ITEM_TYPE_COMPOSITION)
            layer->item.none = (void*)s.read_offset_x();
        else
            s.read_offset_x();

        layer->parent = (const aet_layer*)s.read_offset_x();
        layer->markers_count = s.read_uint32_t_reverse_endianness();
        alh.markers_offset = s.read_offset_x();
        alh.video_offset = s.read_offset_x();
        alh.audio_offset = s.read_offset_x();
    }

    if (alh.markers_offset) {
        aet_marker* marker = alloc->allocate<aet_marker>(layer->markers_count);
        layer->markers = marker;

        s.position_push(alh.markers_offset, SEEK_SET);
        for (uint32_t i = layer->markers_count; i; i--, marker++)
            aet_modern_read_marker(marker, alloc, s, header_length, is_x);
        s.position_pop();
    }
    else
        layer->markers = 0;

    layer->video = aet_modern_read_layer_video(alloc, s, alh.video_offset, header_length, is_x);
    layer->audio = aet_modern_read_layer_audio(alloc, s, alh.audio_offset, header_length, is_x);
}

static aet_layer_audio* aet_modern_read_layer_audio(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t offset,
    uint32_t header_length, bool is_x) {
    if (!offset)
        return 0;

    aet_layer_audio* layer_audio = alloc->allocate<aet_layer_audio>();
    s.position_push(offset, SEEK_SET);
    aet_modern_read_fcurve(&layer_audio->volume_l, alloc, s, header_length, is_x);
    aet_modern_read_fcurve(&layer_audio->volume_r, alloc, s, header_length, is_x);
    aet_modern_read_fcurve(&layer_audio->pan_l, alloc, s, header_length, is_x);
    aet_modern_read_fcurve(&layer_audio->pan_r, alloc, s, header_length, is_x);
    s.position_pop();
    return layer_audio;
}

static aet_layer_video* aet_modern_read_layer_video(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t offset,
    uint32_t header_length, bool is_x) {
    if (!offset)
        return 0;

    aet_layer_video* layer_video = alloc->allocate<aet_layer_video>();
    s.position_push(offset, SEEK_SET);
    layer_video->transfer_mode.mode = (aet_blend_mode)s.read_uint8_t();
    layer_video->transfer_mode.flag = s.read_uint8_t();
    layer_video->transfer_mode.matte = s.read_uint8_t();
    s.read_uint8_t();
    if (is_x)
        s.read_uint32_t();
    aet_modern_read_fcurve(&layer_video->anchor_x, alloc, s, header_length, is_x);
    aet_modern_read_fcurve(&layer_video->anchor_y, alloc, s, header_length, is_x);
    aet_modern_read_fcurve(&layer_video->pos_x, alloc, s, header_length, is_x);
    aet_modern_read_fcurve(&layer_video->pos_y, alloc, s, header_length, is_x);
    aet_modern_read_fcurve(&layer_video->rot_z, alloc, s, header_length, is_x);
    aet_modern_read_fcurve(&layer_video->scale_x, alloc, s, header_length, is_x);
    aet_modern_read_fcurve(&layer_video->scale_y, alloc, s, header_length, is_x);
    aet_modern_read_fcurve(&layer_video->opacity, alloc, s, header_length, is_x);
    layer_video->_3d = aet_modern_read_layer_video_3d(alloc,
        s, s.read_offset(header_length, is_x), header_length, is_x);
    s.position_pop();
    return layer_video;
}

static aet_layer_video_3d* aet_modern_read_layer_video_3d(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t offset,
    uint32_t header_length, bool is_x) {
    if (!offset)
        return 0;

    aet_layer_video_3d* layer_video_3d = alloc->allocate<aet_layer_video_3d>();
    s.position_push(offset, SEEK_SET);
    aet_modern_read_fcurve(&layer_video_3d->anchor_z, alloc, s, header_length, is_x);
    aet_modern_read_fcurve(&layer_video_3d->pos_z, alloc, s, header_length, is_x);
    aet_modern_read_fcurve(&layer_video_3d->dir_x, alloc, s, header_length, is_x);
    aet_modern_read_fcurve(&layer_video_3d->dir_y, alloc, s, header_length, is_x);
    aet_modern_read_fcurve(&layer_video_3d->dir_z, alloc, s, header_length, is_x);
    aet_modern_read_fcurve(&layer_video_3d->rot_x, alloc, s, header_length, is_x);
    aet_modern_read_fcurve(&layer_video_3d->rot_y, alloc, s, header_length, is_x);
    aet_modern_read_fcurve(&layer_video_3d->scale_z, alloc, s, header_length, is_x);
    s.position_pop();
    return layer_video_3d;
}

static void aet_modern_read_marker(aet_marker* marker,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s,
    uint32_t header_length, bool is_x) {
    marker->time = s.read_float_t_reverse_endianness();
    marker->name = aet_read_utf8_string_null_terminated_offset(
        alloc, s, s.read_offset(header_length, is_x));
}

static aet_scene* aet_modern_read_scene(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t offset,
    uint32_t header_length, bool is_x,
    std::unordered_map<int64_t, const aet_layer*>& layers, std::unordered_map<int64_t, aet_item>& items) {
    if (!offset)
        return 0;

    aet_scene_header ash = {};

    aet_scene* scene = alloc->allocate<aet_scene>();
    s.position_push(offset, SEEK_SET);
    if (!is_x) {
        scene->name = aet_read_utf8_string_null_terminated_offset(
            alloc, s, s.read_offset_f2(header_length));
        scene->start_time = s.read_float_t_reverse_endianness();
        scene->end_time = s.read_float_t_reverse_endianness();
        scene->fps = s.read_float_t_reverse_endianness();
        scene->color[0] = s.read_uint8_t();
        scene->color[1] = s.read_uint8_t();
        scene->color[2] = s.read_uint8_t();
        s.read_uint8_t();
        scene->width = s.read_uint32_t_reverse_endianness();
        scene->height = s.read_uint32_t_reverse_endianness();
        ash.camera_offset = s.read_offset_f2(header_length);
        scene->comp_count = s.read_uint32_t_reverse_endianness();
        ash.comp_offset = s.read_offset_f2(header_length);
        scene->video_count = s.read_uint32_t_reverse_endianness();
        ash.video_offset = s.read_offset_f2(header_length);
        scene->audio_count = s.read_uint32_t_reverse_endianness();
        ash.audio_offset = s.read_offset_f2(header_length);
    }
    else {
        scene->name = aet_read_utf8_string_null_terminated_offset(
            alloc, s, s.read_offset_x());
        scene->start_time = s.read_float_t_reverse_endianness();
        scene->end_time = s.read_float_t_reverse_endianness();
        scene->fps = s.read_float_t_reverse_endianness();
        scene->color[0] = s.read_uint8_t();
        scene->color[1] = s.read_uint8_t();
        scene->color[2] = s.read_uint8_t();
        s.read_uint8_t();
        scene->width = s.read_uint32_t_reverse_endianness();
        scene->height = s.read_uint32_t_reverse_endianness();
        ash.camera_offset = s.read_offset_x();
        scene->comp_count = s.read_uint32_t_reverse_endianness();
        ash.comp_offset = s.read_offset_x();
        scene->video_count = s.read_uint32_t_reverse_endianness();
        ash.video_offset = s.read_offset_x();
        scene->audio_count = s.read_uint32_t_reverse_endianness();
        ash.audio_offset = s.read_offset_x();
    }
    s.position_pop();

    scene->camera = aet_modern_read_camera(alloc, s, ash.camera_offset, header_length, is_x);

    if (ash.comp_offset) {
        aet_comp* comp = alloc->allocate<aet_comp>(scene->comp_count);
        scene->comp = comp;

        s.position_push(ash.comp_offset, SEEK_SET);
        for (uint32_t i = scene->comp_count; i; i--, comp++)
            aet_modern_read_comp(comp, alloc, s, header_length, is_x, layers, items);
        s.position_pop();
    }
    else
        scene->comp = 0;

    if (ash.video_offset) {
        aet_video* video = alloc->allocate<aet_video>(scene->video_count);
        scene->video = video;

        s.position_push(ash.video_offset, SEEK_SET);
        for (uint32_t i = scene->video_count; i; i--, video++)
            aet_modern_read_video(video, alloc, s, header_length, is_x, items);
        s.position_pop();
    }
    else
        scene->video = 0;

    if (ash.audio_offset) {
        aet_audio* audio = alloc->allocate<aet_audio>(scene->audio_count);
        scene->audio = audio;

        s.position_push(ash.audio_offset, SEEK_SET);
        for (uint32_t i = scene->audio_count; i; i--, audio++)
            aet_modern_read_audio(audio, alloc, s, header_length, is_x);
        s.position_pop();
    }
    else
        scene->audio = 0;
    return scene;
}

static void aet_modern_read_video(aet_video* video,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s,
    uint32_t header_length, bool is_x,
    std::unordered_map<int64_t, aet_item>& items) {
    items[s.get_position()] = video;

    video->color[0] = s.read_uint8_t();
    video->color[1] = s.read_uint8_t();
    video->color[2] = s.read_uint8_t();
    s.read_uint8_t();
    video->width = s.read_uint16_t_reverse_endianness();
    video->height = s.read_uint16_t_reverse_endianness();
    video->fpf = s.read_float_t_reverse_endianness();
    video->sources_count = s.read_uint32_t_reverse_endianness();
    int64_t sources_offset = s.read_offset(header_length, is_x);

    if (sources_offset) {
        aet_video_src* source = alloc->allocate<aet_video_src>(video->sources_count);
        video->sources = source;

        s.position_push(sources_offset, SEEK_SET);
        if (!is_x)
            for (uint32_t i = video->sources_count; i; i--, source++) {
                source->sprite_name = aet_read_utf8_string_null_terminated_offset(
                    alloc, s, s.read_offset_f2(header_length));
                source->sprite_index = s.read_int32_t();
            }
        else
            for (uint32_t i = video->sources_count; i; i--, source++) {
                source->sprite_name = aet_read_utf8_string_null_terminated_offset(
                    alloc, s, s.read_offset_x());
                source->sprite_index = s.read_int32_t();
            }
        s.position_pop();
    }
    else
        video->sources = 0;
}

static void aet_comp_set_item_parent(const aet_comp* comp,
    std::unordered_map<int64_t, const aet_layer*>& layers,
    std::unordered_map<int64_t, aet_item>& items) {
    aet_layer* layer = (aet_layer*)comp->layers;
    for (uint32_t i = comp->layers_count; i; i--, layer++) {
        if (layer->item_type == AET_ITEM_TYPE_VIDEO
            || layer->item_type == AET_ITEM_TYPE_COMPOSITION) {
            int64_t item_offset = (int64_t)layer->item.none;
            layer->item.none = 0;

            if (item_offset) {
                auto elem = items.find(item_offset);
                if (elem != items.end())
                    layer->item = elem->second;
            }
        }

        int64_t parent_offset = (int64_t)layer->parent;
        layer->parent = 0;

        if (parent_offset) {
            auto elem = layers.find(parent_offset);
            if (elem != layers.end())
                layer->parent = elem->second;
        }
    }
}

inline static const char* aet_move_data_string(const char* str,
    prj::shared_ptr<prj::stack_allocator>& alloc) {
    if (str)
        return alloc->allocate<char>(str, utf8_length(str) + 1);
    return 0;
}

inline static const char* aet_read_utf8_string_null_terminated_offset(
    prj::shared_ptr<prj::stack_allocator>& alloc, stream& s, int64_t offset) {
    size_t len = s.read_utf8_string_null_terminated_offset_length(offset);
    char* str = alloc->allocate<char>(len + 1);
    s.position_push(offset, SEEK_SET);
    s.read(str, len);
    s.position_pop();
    str[len] = 0;
    return str;
}

inline static int64_t aet_strings_get_string_offset(
    const std::unordered_map<std::string, int64_t>& vec, const std::string& str) {
    auto elem = vec.find(str);
    if (elem != vec.end())
        return elem->second;
    return 0;
}

inline static void aet_strings_push_back_check(stream& s,
    std::unordered_map<std::string, int64_t>& vec, const std::string& str) {
   if (vec.find(str) != vec.end())
       return;

    vec[str] = s.get_position();
    s.write_string_null_terminated(str);
}
