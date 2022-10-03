/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "aet.hpp"
#include "f2/struct.hpp"
#include "io/path.hpp"
#include "io/memory_stream.hpp"

static void aet_set_classic_read_inner(aet_set* as, stream& s);
static void aet_set_classic_write_inner(aet_set* as, stream& s);
static void aet_set_modern_read_inner(aet_set* as, stream& s);
static void aet_set_modern_write_inner(aet_set* as, stream& s);

aet_marker::aet_marker() : time() {

}

aet_marker::~aet_marker() {

}

aet_layer_video_3d::aet_layer_video_3d() {

}

aet_layer_video_3d::~aet_layer_video_3d() {

}

aet_transfer_mode::aet_transfer_mode() : mode(), flag(), matte() {

}

aet_layer_video::aet_layer_video() : _3d_init() {

}

aet_layer_video::~aet_layer_video() {

}

aet_layer_audio::aet_layer_audio() {

}

aet_layer_audio::~aet_layer_audio() {

}

aet_color::aet_color() : r(), g(), b() {

}

aet_video_src::aet_video_src() : sprite_index() {

}

aet_video_src::~aet_video_src() {

}

aet_video::aet_video() : width(), height(), fpf() {

}

aet_video::~aet_video() {

}

aet_audio::aet_audio() : sound_index() {

}

aet_layer::aet_layer() : start_time(), end_time(), offset_time(), time_scale(), flags(),
quality(), item_type(), item(), parent(), video(), video_init(), audio(), audio_init() {

}

aet_layer::~aet_layer() {

}

aet_camera::aet_camera() {

}

aet_camera::~aet_camera() {

}

aet_scene::aet_scene() : start_time(), end_time(), fps(), width(), height(), camera_init() {

}

aet_scene::~aet_scene() {

}

aet_set::aet_set() : ready(), modern(), big_endian(), is_x() {

}

aet_set::~aet_set() {

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

void aet_set::unpack_file(const void* data, size_t size, bool modern) {
    if (!data || !size)
        return;

    memory_stream s;
    s.open(data, size);
    if (!modern)
        aet_set_classic_read_inner(this, s);
    else
        aet_set_modern_read_inner(this, s);
}

static void aet_set_classic_read_inner(aet_set* as, stream& s) {
    as->ready = true;
    as->modern = false;
    as->big_endian = false;
    as->is_x = false;
}

static void aet_set_classic_write_inner(aet_set* as, stream& s) {

}

static void aet_set_modern_read_inner(aet_set* as, stream& s) {
    f2_struct st;
    st.read(s);
    if (st.header.signature != reverse_endianness_uint32_t('AETC') || !st.data.size())
        return;

    bool big_endian = st.header.use_big_endian;

    memory_stream s_aetc;
    s_aetc.open(st.data);
    s_aetc.big_endian = big_endian;

    bool is_x = false;

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
