/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include <map>
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

static void aet_set_classic_read_inner(aet_set* as, prj::shared_ptr<prj::stack_allocator>& alloc, stream& s);
static void aet_set_classic_write_inner(aet_set* as, stream& s);
static void aet_set_modern_read_inner(aet_set* as, prj::shared_ptr<prj::stack_allocator>& alloc, stream& s);
static void aet_set_modern_write_inner(aet_set* as, stream& s);

static void aet_classic_read_audio(aet_audio* audio,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s);
static aet_camera* aet_classic_read_camera(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t offset);
static void aet_classic_read_comp(aet_comp* comp,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s,
    std::map<int64_t, aet_layer*>& layers,
    std::map<int64_t, aet_item>& items);
static void aet_classic_read_fcurve(aet_fcurve* fcurve,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s);
static void aet_classic_read_layer(aet_layer* layer,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s);
static aet_layer_audio* aet_classic_read_layer_audio(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t offset);
static aet_layer_video* aet_classic_read_layer_video(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t offset);
static aet_layer_video_3d* aet_classic_read_layer_video_3d(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t offset);
static void aet_classic_read_marker(aet_marker* marker,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s);
static aet_scene* aet_classic_read_scene(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t offset, std::map<int64_t, aet_layer*>& layers,
    std::map<int64_t, aet_item>& items);
static void aet_classic_read_video(aet_video* video,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s,
    std::map<int64_t, aet_item>& items);

static void aet_modern_read_audio(aet_audio* audio,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s,
    uint32_t header_length, bool is_x);
static aet_camera* aet_modern_read_camera(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t offset,
    uint32_t header_length, bool is_x);
static void aet_modern_read_comp(aet_comp* comp,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s,
    uint32_t header_length, bool is_x,
    std::map<int64_t, aet_layer*>& layers, std::map<int64_t, aet_item>& items);
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
    std::map<int64_t, aet_layer*>& layers, std::map<int64_t, aet_item>& items);
static void aet_modern_read_video(aet_video* video,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s,
    uint32_t header_length, bool is_x,
    std::map<int64_t, aet_item>& items);

static void aet_comp_set_item_parent(const aet_comp* comp,
    std::map<int64_t, aet_layer*>& layers, std::map<int64_t, aet_item>& items);

static const char* aet_move_data_string(const char* str,
    prj::shared_ptr<prj::stack_allocator>& alloc);

static const char* aet_read_utf8_string_null_terminated_offset(
    prj::shared_ptr<prj::stack_allocator>& alloc, stream& s, int64_t offset);

aet_fcurve::aet_fcurve() : keys_count(), keys() {

}

float_t aet_fcurve::interpolate(float_t frame) const {
    if (!keys_count)
        return 0.0f;
    else if (keys_count == 1)
        return keys[0];

    size_t keys_count = this->keys_count;
    const float_t* keys = this->keys;
    const float_t* values = &keys[keys_count];
    if (frame <= keys[0])
        return values[0];

    const float_t* v6 = keys;
    const float_t* v7 = &keys[keys_count];
    if (frame >= keys[keys_count - 1])
        return values[(keys_count - 1) * 2];

    const float_t* v8 = &keys[keys_count / 2];
    while (v6 < v8) {
        if (frame >= *v8)
            v6 = v8;
        else
            v7 = v8;
        v8 = &v6[(v7 - v6) / 2];
    }

    if (values - 1 <= v8)
        return values[(keys_count - 1) * 2];

    size_t v9 = (v8 - keys) * 2;

    float_t df = v8[1] - v8[0];
    float_t t = (frame - v8[0]) / df;
    float_t t_1 = t - 1.0f;
    return (t_1 * values[v9 + 1] + t * values[v9 + 3]) * t * t_1 * df
        + t * t * (3.0f - 2.0f * t) * values[v9 + 2]
        + (1.0f + 2.0f * t) * (t_1 * t_1) * values[v9];
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

static void aet_set_classic_read_inner(aet_set* as, prj::shared_ptr<prj::stack_allocator>& alloc, stream& s) {
    uint32_t scenes_count = 0;
    while (s.read_uint32_t())
        scenes_count++;
    as->scenes_count = scenes_count;

    aet_scene** scenes = alloc->allocate<aet_scene*>(scenes_count);
    as->scenes = (const aet_scene**)scenes;

    uint32_t* data = force_malloc_s(uint32_t, scenes_count);
    s.set_position(0, SEEK_SET);
    for (uint32_t i = 0; i < scenes_count; i++)
        data[i] = s.read_uint32_t();

    std::map<int64_t, aet_layer*> layers;
    std::map<int64_t, aet_item> items;

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

}

static void aet_set_modern_read_inner(aet_set* as, prj::shared_ptr<prj::stack_allocator>& alloc, stream& s) {
    f2_struct st;
    st.read(s);
    if (st.header.signature != reverse_endianness_uint32_t('AETC') || !st.data.size())
        return;

    uint32_t header_length = st.header.length;
    bool big_endian = st.header.use_big_endian;
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

    int64_t* data = force_malloc_s(int64_t, scenes_count);
    s_aetc.set_position(0, SEEK_SET);
    if (!is_x)
        for (uint32_t i = 0; i < scenes_count; i++)
            data[i] = s_aetc.read_uint32_t_reverse_endianness();
    else
        for (uint32_t i = 0; i < scenes_count; i++)
            data[i] = s_aetc.read_int64_t_reverse_endianness();

    std::map<int64_t, aet_layer*> layers;
    std::map<int64_t, aet_item> items;

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

    st.header.signature = reverse_endianness_uint32_t('AETC');
    st.header.length = 0x20;
    st.header.use_big_endian = big_endian;
    st.header.use_section_size = true;

    st.write(s, true, is_x);
}

static void aet_classic_read_audio(aet_audio* audio,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s) {
    audio->sound_index = s.read_int32_t();
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

static void aet_classic_read_comp(aet_comp* comp,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s,
    std::map<int64_t, aet_layer*>& layers,
    std::map<int64_t, aet_item>& items) {
    items.insert_or_assign(s.get_position(), comp);

    comp->layers_count = s.read_uint32_t();
    uint32_t layers_offset = s.read_uint32_t();

    if (layers_offset) {
        aet_layer* layer = alloc->allocate<aet_layer>(comp->layers_count);
        comp->layers = layer;

        s.position_push(layers_offset, SEEK_SET);
        for (uint32_t i = comp->layers_count; i; i--, layer++) {
            layers.insert_or_assign(s.get_position(), layer);
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
        for (uint32_t i = layer->markers_count; i; i--, marker++)
            aet_classic_read_marker(marker, alloc, s);
        s.position_pop();
    }
    else
        layer->audio = 0;

    layer->video = aet_classic_read_layer_video(alloc, s, alh.video_offset);
    layer->audio = aet_classic_read_layer_audio(alloc, s, alh.audio_offset);
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

static void aet_classic_read_marker(aet_marker* marker,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s) {
    marker->time = s.read_float_t();
    marker->name = aet_read_utf8_string_null_terminated_offset(
        alloc, s, s.read_uint32_t());
}

static aet_scene* aet_classic_read_scene(prj::shared_ptr<prj::stack_allocator> alloc,
    stream& s, int64_t offset, std::map<int64_t, aet_layer*>& layers,
    std::map<int64_t, aet_item>& items) {
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
            aet_classic_read_audio(audio, alloc, s);
        s.position_pop();
    }
    else
        scene->audio = 0;
    return scene;
}

static void aet_classic_read_video(aet_video* video,
    prj::shared_ptr<prj::stack_allocator> alloc, stream& s,
    std::map<int64_t, aet_item>& items) {
    items.insert_or_assign(s.get_position(), video);

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
    std::map<int64_t, aet_layer*>& layers, std::map<int64_t, aet_item>& items) {
    items.insert_or_assign(s.get_position(), comp);

    comp->layers_count = s.read_uint32_t();
    int64_t layers_offset = s.read_offset(header_length, is_x);

    if (layers_offset) {
        aet_layer* layer = alloc->allocate<aet_layer>(comp->layers_count);
        comp->layers = layer;

        s.position_push(layers_offset, SEEK_SET);
        for (uint32_t i = comp->layers_count; i; i--, layer++) {
            layers.insert_or_assign(s.get_position(), layer);
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
    fcurve->keys_count = s.read_uint32_t();
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
        layer->audio = 0;

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
    std::map<int64_t, aet_layer*>& layers, std::map<int64_t, aet_item>& items) {
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
    std::map<int64_t, aet_item>& items) {
    items.insert_or_assign(s.get_position(), video);

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
    std::map<int64_t, aet_layer*>& layers, std::map<int64_t, aet_item>& items) {
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
