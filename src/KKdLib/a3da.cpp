/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "a3da.hpp"
#include "f2/struct.hpp"
#include "io/file_stream.hpp"
#include "io/memory_stream.hpp"
#include "io/path.hpp"
#include "half_t.hpp"
#include "hash.hpp"
#include "key_val.hpp"
#include "str_utils.hpp"
#include <time.h>

namespace a3d {
    struct a3da_vec3_header {
        uint32_t x;
        uint32_t y;
        uint32_t z;
    };

    struct a3da_model_transform_header {
        a3da_vec3_header scale;
        a3da_vec3_header rotation;
        a3da_vec3_header translation;
        uint32_t visibility;
    };

    struct a3dc_header {
        uint32_t binary_length;
        uint32_t binary_offset;
        uint32_t string_length;
        uint32_t string_offset;
    };

    struct a3dc_key_header {
        FC_TYPE type : 8;
        EP_TYPE ep_type_pre : 4;
        EP_TYPE ep_type_post : 4;
        uint32_t padding : 16;
        float_t value;
        float_t max_frame;
        uint32_t length;
    };

    static void a3da_read_inner(Scene* sc, stream& s);
    static void a3da_write_inner(Scene* sc, stream& s);
    static void a3da_read_text(Scene* sc, void* data, size_t size);
    static void a3da_write_text(Scene* sc, void** data, size_t* size, bool a3dc);
    static void a3da_read_data(Scene* sc, void* data, size_t size);
    static void a3da_write_data(Scene* sc, void** data, size_t* size);
    static void a3da_get_time_stamp(char* buf, size_t buf_size);
    static bool key_val_read(key_val* kv,
        const char* key, Fcurve& value);
    static void key_val_out_write(key_val_out* kv, stream& s,
        const char* key, Fcurve& value, bool write_true = false);
    static bool key_val_read_raw_data(key_val* kv,
        Fcurve& value);
    static void key_val_out_write_raw_data(key_val_out* kv, stream& s,
        Fcurve& value);
    static bool key_val_read(key_val* kv,
        const char* key, ModelTransform& value);
    static void key_val_out_write(key_val_out* kv, stream& s,
        const char* key, ModelTransform& value, int32_t write_mask = 0x1F);
    static bool key_val_read(key_val* kv,
        const char* key, FcurveColor4f& value);
    static void key_val_out_write(key_val_out* kv, stream& s,
        const char* key, FcurveColor4f& value);
    static bool key_val_read(key_val* kv,
        const char* key, Fcurve3f& value);
    static void key_val_out_write(key_val_out* kv, stream& s,
        const char* key, Fcurve3f& value);
    static void a3dc_read_a3da_key(void* data, size_t size, Fcurve* value);
    static void a3dc_write_a3da_key(stream& s, Fcurve& value);
    static void a3dc_read_a3da_key_f16(void* data, size_t size, Fcurve* value, COMPRESS_TYPE f16);
    static void a3dc_write_a3da_key_f16(stream& s, Fcurve& value, COMPRESS_TYPE f16);
    static void a3dc_read_a3da_model_transform(void* data, size_t size,
        ModelTransform* value, COMPRESS_TYPE f16);
    static void a3dc_write_a3da_model_transform(stream& s,
        ModelTransform& value, COMPRESS_TYPE f16);
    static void a3dc_write_a3da_model_transform_offset(stream& s,
        ModelTransform& value);
    static void a3dc_write_a3da_model_transform_offset_data(stream& s,
        ModelTransform& value);
    static void a3dc_read_a3da_rgba(void* data, size_t size, FcurveColor4f* value);
    static void a3dc_write_a3da_rgba(stream& s, FcurveColor4f& value);
    static void a3dc_read_a3da_vec3(void* data, size_t size, Fcurve3f* value);
    static void a3dc_write_a3da_vec3(stream& s, Fcurve3f& value);
    static void a3dc_read_a3da_vec3_f16(void* data, size_t size, Fcurve3f* value, COMPRESS_TYPE f16);
    static void a3dc_write_a3da_vec3_f16(stream& s, Fcurve3f& value, COMPRESS_TYPE f16);

    static void a3da_key_make_raw_data_binary(Fcurve* value);
    static void a3da_model_transform_make_raw_data_binary(ModelTransform* value);
    static void a3da_rgba_make_raw_data_binary(FcurveColor4f* value);
    static void a3da_vec3_make_raw_data_binary(Fcurve3f* value);

    Fcurve::Fcurve() : flag(), bin_offset(), type(), ep_type_pre(), ep_type_post(), max_frame(),
    raw_data(), raw_data_binary(), raw_data_value_list_size(), raw_data_value_list_offset(), value() {
    }

    Fcurve::~Fcurve() {

    }

    Fcurve& Fcurve::operator=(const Fcurve& other) {
        flag = other.flag;
        bin_offset = other.bin_offset;
        type = other.type;
        ep_type_pre = other.ep_type_pre;
        ep_type_post = other.ep_type_post;
        keys.assign(other.keys.begin(), other.keys.end());
        max_frame = other.max_frame;
        raw_data = other.raw_data;
        raw_data_binary = other.raw_data_binary;
        raw_data_value_list_size = other.raw_data_value_list_size;
        raw_data_value_list_offset = other.raw_data_value_list_offset;
        value = other.value;
        return *this;
    }

    Fcurve3f::Fcurve3f() {

    }

    Fcurve3f::~Fcurve3f() {

    }

    FcurveColor4f::FcurveColor4f() : has_r(), has_g(), has_b(), has_a() {

    }

    FcurveColor4f::~FcurveColor4f() {

    }

    ModelTransform::ModelTransform() : bin_offset(), flag() {

    }

    ModelTransform::~ModelTransform() {

    }

    // MGF
    Ambient::Ambient() : flag() {

    }

    // MGF
    Ambient::~Ambient() {

    }

    Auth2d::Auth2d() {

    }

    Auth2d::~Auth2d() {

    }

    CameraAuxiliary::CameraAuxiliary() : flag() {

    }

    CameraAuxiliary::~CameraAuxiliary() {

    }

    CameraInterest::CameraInterest() {

    }

    CameraInterest::~CameraInterest() {

    }

    CameraViewPoint::CameraViewPoint() : fov_is_horizontal(),
    has_fov(), aspect(), camera_aperture_w(), camera_aperture_h() {

    }

    CameraViewPoint::~CameraViewPoint() {

    }

    CameraRoot::CameraRoot() {

    }

    CameraRoot::~CameraRoot() {

    }

    Chara::Chara() {

    }

    Chara::~Chara() {

    }

    Curve::Curve() {

    }

    Curve::~Curve() {

    }

    Dof::Dof() : has_dof() {

    }

    Dof::~Dof() {

    }

    Event::Event() : begin(), clip_begin(),
    clip_end(), end(), time_ref_scale(), type() {

    }

    Event::~Event() {

    }

    Fog::Fog() : flag(), id(-1), name("(init)") {

    }

    Fog::~Fog() {

    }

    Light::Light() : flag(), id(-1), name("(init)"), type("(init)") {

    }

    Light::~Light() {

    }

    MObjectHrc::MObjectHrc() {

    }

    MObjectHrc::~MObjectHrc() {

    }

    // X/HD
    MaterialList::MaterialList() : flag() {

    }

    // X/HD
    MaterialList::~MaterialList() {

    }

    Motion::Motion() {

    }

    Motion::~Motion() {

    }

    Object::Object() : morph_offset(), pattern_offset() {

    }

    Object::~Object() {

    }

    ObjectHrc::ObjectHrc() : shadow() {

    }

    ObjectHrc::~ObjectHrc() {

    }

    ObjectInstance::ObjectInstance() : shadow() {

    }

    ObjectInstance::~ObjectInstance() {

    }

    ObjectNode::ObjectNode() : flag(), parent(), joint_orient() {

    }

    ObjectNode::~ObjectNode() {

    }

    PlayControl::PlayControl() : flag(), begin(), div(), fps(), offset(), size() {

    }

    PlayControl::~PlayControl() {

    }

    Point::Point() {

    }

    Point::~Point() {

    }

    PostProcess::PostProcess() : flag() {

    }

    PostProcess::~PostProcess() {

    }

    Texture::Texture() : pattern_offset() {

    }

    Texture::~Texture() {

    }

    TextureTransform::TextureTransform() : flag() {

    }

    TextureTransform::~TextureTransform() {

    }

    Scene::Scene() : ready(), compressed(), format(), _compress_f16(),
        _file_name(), _property_version(), _converter_version() {

    }

    Scene::~Scene() {

    }

    void Scene::read(const char* path) {
        if (!path)
            return;

        char* path_a3da = str_utils_add(path, (char*)".a3da");
        if (!path_a3da)
            return;

        if (path_check_file_exists(path_a3da)) {
            file_stream s;
            s.open(path_a3da, "rb");
            if (s.check_not_null())
                a3da_read_inner(this, s);
        }
        free_def(path_a3da);
    }

    void Scene::read(const wchar_t* path) {
        if (!path)
            return;

        wchar_t* path_a3da = str_utils_add(path, (wchar_t*)L".a3da");
        if (!path_a3da)
            return;

        if (path_check_file_exists(path_a3da)) {
            file_stream s;
            s.open(path_a3da, L"rb");
            if (s.check_not_null())
                a3da_read_inner(this, s);
        }
        free_def(path_a3da);
    }

    void Scene::read(const void* data, size_t size) {
        if (!data || !size)
            return;

        memory_stream s;
        s.open(data, size);
        a3da_read_inner(this, s);
    }

    void Scene::write(const char* path) {
        if (!path || !ready)
            return;

        char* path_a3da = str_utils_add(path, ".a3da");
        if (!path_a3da)
            return;

        file_stream s;
        s.open(path_a3da, "wb");
        if (s.check_not_null())
            a3da_write_inner(this, s);
        free_def(path_a3da);
    }

    void Scene::write(const wchar_t* path) {
        if (!path || !ready)
            return;

        wchar_t* path_a3da = str_utils_add(path, L".a3da");
        if (!path_a3da)
            return;

        file_stream s;
        s.open(path_a3da, L"wb");
        if (s.check_not_null())
            a3da_write_inner(this, s);
        free_def(path_a3da);
    }

    void Scene::write(void** data, size_t* size) {
        if (!data || !size || !ready)
            return;

        memory_stream s;
        a3da_write_inner(this, s);
        s.copy(data, size);
    }

    bool Scene::load_file(void* data, const char* dir, const char* file, uint32_t hash) {
        size_t file_len = utf8_length(file);

        const char* t = strrchr(file, '.');
        if (t)
            file_len = t - file;

        std::string path(dir);
        path.append(file, file_len);

        Scene* sc = (Scene*)data;
        sc->read(path.c_str());

        return sc->ready;
    }

    static void a3da_read_inner(Scene* sc, stream& s) {
        a3dc_header header = {};

        sc->format = FORMAT_F;
        uint32_t signature = s.read_uint32_t();
        s.set_position(0x00, SEEK_SET);

        memory_stream _s;
        if (signature == reverse_endianness_int32_t('A3DA')) {
            f2_struct st;
            st.read(s);
            _s.open(st.data);
            sc->format = FORMAT_F2;
        }
        else {
            size_t length = s.length;
            void* data = force_malloc(length);
            s.read(data, length);
            _s.open(data, length);
            free_def(data);
        }

        void* a3da_data;

        signature = _s.read_uint32_t();
        if (signature != reverse_endianness_int32_t('#A3D'))
            return;

        signature = _s.read_uint32_t();
        if ((signature & 0xFF) == 'A') {
            header.string_offset = 0x10;
            header.string_length = (int32_t)(_s.length - 0x10);
        }
        else if ((signature & 0xFF) == 'C') {
            _s.set_position(0x10, SEEK_SET);
            _s.read_int32_t();
            _s.read_int32_t();
            uint32_t sub_headers_offset = _s.read_uint32_t_reverse_endianness(true);
            uint16_t sub_headers_count = _s.read_uint16_t_reverse_endianness(true);
            uint16_t sub_headers_stride = _s.read_uint16_t_reverse_endianness(true);

            if (sub_headers_count != 0x02)
                return;

            _s.set_position(sub_headers_offset, SEEK_SET);
            if (_s.read_int32_t() != 0x50)
                return;
            header.string_offset = _s.read_uint32_t_reverse_endianness(true);
            header.string_length = _s.read_uint32_t_reverse_endianness(true);

            _s.set_position((int64_t)sub_headers_offset + sub_headers_stride, SEEK_SET);
            if (_s.read_int32_t() != 0x4C42)
                return;
            header.binary_offset = _s.read_uint32_t_reverse_endianness(true);
            header.binary_length = _s.read_uint32_t_reverse_endianness(true);
        }
        else
            return;

        _s.set_position(header.string_offset, SEEK_SET);
        a3da_data = force_malloc(header.string_length + 1LL);
        _s.read(a3da_data, header.string_length);
        ((uint8_t*)a3da_data)[header.string_length] = 0;
        a3da_read_text(sc, a3da_data, header.string_length);
        free_def(a3da_data);

        if (signature == reverse_endianness_int32_t('C___')) {
            sc->compressed = true;
            _s.set_position(header.binary_offset, SEEK_SET);
            void* a3dc_data = force_malloc(header.binary_length);
            _s.read(a3dc_data, header.binary_length);
            a3da_read_data(sc, a3dc_data, header.binary_length);
            free_def(a3dc_data);
        }
        sc->ready = true;
    }

    static void a3da_write_inner(Scene* sc, stream& s) {
        bool a3dc = sc->compressed || sc->format > FORMAT_AFT && sc->format != FORMAT_AFT_X_PACK;

        if (sc->format <= FORMAT_AFT || sc->format == FORMAT_AFT_X_PACK)
            sc->_compress_f16 = COMPRESS_F32F32F32F32;

        void* a3dc_data = 0;
        size_t a3dc_data_length = 0;
        if (a3dc)
            a3da_write_data(sc, &a3dc_data, &a3dc_data_length);

        void* a3da_data = 0;
        size_t a3da_data_length = 0;
        a3da_write_text(sc, &a3da_data, &a3da_data_length, a3dc);

        memory_stream s_a3da;
        stream& _s = s;
        if (sc->format > FORMAT_AFT && sc->format != FORMAT_AFT_X_PACK) {
            s_a3da.open();
            _s = s_a3da;
        }

        if (a3dc) {
            a3dc_header header = {};

            header.string_offset = (uint32_t)0x40;
            header.string_length = (uint32_t)a3da_data_length;
            header.binary_offset = (uint32_t)(0x40 + align_val(a3da_data_length, 0x20));
            header.binary_length = (uint32_t)a3dc_data_length;

            _s.write("#A3DC__________\n", 16);
            _s.write_int32_t(0x2000);
            _s.write_int32_t(0x00);
            _s.write_uint32_t_reverse_endianness(0x20, true);
            _s.write_uint16_t_reverse_endianness(0x02, true);
            _s.write_uint16_t_reverse_endianness(0x10, true);
            _s.write_char('P');
            _s.align_write(0x04);
            _s.write_uint32_t_reverse_endianness(header.string_offset, true);
            _s.write_uint32_t_reverse_endianness(header.string_length, true);
            _s.write_uint32_t_reverse_endianness(0x01, true);
            _s.write_char('B');
            _s.write_char('L');
            _s.align_write(0x04);
            _s.write_uint32_t_reverse_endianness(header.binary_offset, true);
            _s.write_uint32_t_reverse_endianness(header.binary_length, true);
            _s.write_uint32_t_reverse_endianness(0x20, true);
            _s.write(a3da_data, a3da_data_length);
            _s.align_write(0x20);
            _s.write(a3dc_data, a3dc_data_length);
            free_def(a3dc_data);
        }
        else
            _s.write(a3da_data, a3da_data_length);
        free_def(a3da_data);

        if (sc->format > FORMAT_AFT && sc->format != FORMAT_AFT_X_PACK) {
            f2_struct st;
            s_a3da.align_write(0x10);
            s_a3da.copy(st.data);
            s_a3da.close();

            new (&st.header) f2_header('A3DA');
            st.header.inner_signature = sc->format == FORMAT_XHD ? 0x00131010 : 0x01131010;

            st.write(s, true, sc->format == FORMAT_X || sc->format == FORMAT_XHD);
        }
    }

    static void a3da_read_text(Scene* sc, void* data, size_t size) {
        key_val kv;
        kv.parse((uint8_t*)data, size);

        if (kv.open_scope("_")) {
            int32_t compress_f16;
            if (kv.read("compress_f16", compress_f16))
                sc->_compress_f16 = (COMPRESS_TYPE)compress_f16;
            kv.read("converter.version", sc->_converter_version);
            kv.read("file_name", sc->_file_name);
            kv.read("property.version", sc->_property_version);

            kv.close_scope();
        }

        if (kv.open_scope("camera_auxiliary")) {
            CameraAuxiliary* ca = &sc->camera_auxiliary;

            if (key_val_read(&kv, "exposure", ca->exposure))
                ca->flag |= CameraAuxiliary::FLAG_EXPOSURE;
            if (key_val_read(&kv, "exposure_rate", ca->exposure_rate)) // F/F2/X
                ca->flag |= CameraAuxiliary::FLAG_EXPOSURE_RATE;
            if (key_val_read(&kv, "gamma", ca->gamma))
                ca->flag |= CameraAuxiliary::FLAG_GAMMA;
            if (key_val_read(&kv, "gamma_rate", ca->gamma_rate)) // F/F2/X
                ca->flag |= CameraAuxiliary::FLAG_GAMMA_RATE;
            if (key_val_read(&kv, "saturate", ca->saturate))
                ca->flag |= CameraAuxiliary::FLAG_SATURATE;
            if (key_val_read(&kv, "auto_exposure", ca->auto_exposure))
                ca->flag |= CameraAuxiliary::FLAG_AUTO_EXPOSURE;

            if (ca->flag & (CameraAuxiliary::FLAG_EXPOSURE_RATE | CameraAuxiliary::FLAG_GAMMA_RATE))
                if (sc->format < FORMAT_F || sc->format == FORMAT_AFT)
                    sc->format = FORMAT_F;

            kv.close_scope();
        }

        if (kv.open_scope("play_control")) {
            PlayControl* pc = &sc->play_control;

            kv.read("begin", pc->begin);
            if (kv.read("div", pc->div))
                pc->flag |= PlayControl::FLAG_DIV;
            kv.read("fps", pc->fps);
            if (kv.read("offset", pc->offset))
                pc->flag |= PlayControl::FLAG_OFFSET;
            kv.read("size", pc->size);

            kv.close_scope();
        }

        if (kv.open_scope("post_process")) {
            PostProcess* pp = &sc->post_process;

            if (key_val_read(&kv, "lens_flare", pp->lens_flare))
                pp->flag |= PostProcess::FLAG_LENS_FLARE;
            if (key_val_read(&kv, "lens_shaft", pp->lens_shaft))
                pp->flag |= PostProcess::FLAG_LENS_SHAFT;
            if (key_val_read(&kv, "lens_ghost", pp->lens_ghost))
                pp->flag |= PostProcess::FLAG_LENS_GHOST;
            if (key_val_read(&kv, "Ambient", pp->radius))
                pp->flag |= PostProcess::FLAG_RADIUS;
            if (key_val_read(&kv, "Diffuse", pp->intensity))
                pp->flag |= PostProcess::FLAG_INTENSITY;
            if (key_val_read(&kv, "Specular", pp->scene_fade))
                pp->flag |= PostProcess::FLAG_SCENE_FADE;

            kv.close_scope();
        }

        if (kv.open_scope("dof")) {
            Dof* d = &sc->dof;

            d->has_dof = key_val_read(&kv, "", d->model_transform);

            kv.close_scope();
        }

        int32_t count;

        // MGF
        if (kv.read("ambient", "length", count)) {
            sc->ambient.resize(count);
            Ambient* va = sc->ambient.data();
            for (int32_t i = 0; i < count; i++) {
                if (!kv.open_scope_fmt(i))
                    continue;

                Ambient* sc = &va[i];
                if (key_val_read(&kv, "light.Diffuse", sc->light_diffuse))
                    sc->flag |= Ambient::FLAG_LIGHT_DIFFUSE;
                kv.read("name", sc->name);
                if (key_val_read(&kv, "rim.light.Diffuse", sc->rim_light_diffuse))
                    sc->flag |= Ambient::FLAG_RIM_LIGHT_DIFFUSE;

                kv.close_scope();
            }
            kv.close_scope();
        }

        if (kv.read("auth_2d", "length", count)) {
            sc->auth_2d.resize(count);
            Auth2d* va2 = sc->auth_2d.data();
            for (int32_t i = 0; i < count; i++) {
                if (!kv.open_scope_fmt(i))
                    continue;

                Auth2d* a2 = &va2[i];
                kv.read(a2->name);
                kv.close_scope();
            }
            kv.close_scope();
        }

        if (kv.read("camera_root", "length", count)) {
            sc->camera_root.resize(count);
            CameraRoot* vcr = sc->camera_root.data();
            for (int32_t i = 0; i < count; i++) {
                if (!kv.open_scope_fmt(i))
                    continue;

                CameraRoot* cr = &vcr[i];
                key_val_read(&kv, "", cr->model_transform);

                CameraInterest* intr = &cr->interest;
                key_val_read(&kv, "interest", intr->model_transform);

                CameraViewPoint* vp = &cr->view_point;
                if (!kv.open_scope("view_point")) {
                    kv.close_scope();
                    continue;
                }

                key_val_read(&kv, "", vp->model_transform);
                key_val_read(&kv, "roll", vp->roll);

                if (kv.has_key("fov_is_horizontal")) {
                    vp->has_fov = true;
                    key_val_read(&kv, "fov", vp->fov);
                    kv.read("fov_is_horizontal", vp->fov_is_horizontal);
                    kv.read("aspect", vp->aspect);
                }
                else {
                    key_val_read(&kv, "focal_length", vp->focal_length);
                    kv.read("camera_aperture_w", vp->camera_aperture_w);
                    kv.read("camera_aperture_h", vp->camera_aperture_h);
                }

                kv.close_scope();
                kv.close_scope();
            }
            kv.close_scope();
        }

        if (kv.read("chara", "length", count)) {
            sc->chara.resize(count);
            Chara* vc = sc->chara.data();
            for (int32_t i = 0; i < count; i++) {
                if (!kv.open_scope_fmt(i))
                    continue;

                Chara* c = &vc[i];
                key_val_read(&kv, "", c->model_transform);
                kv.read("name", c->name);

                kv.close_scope();
            }
            kv.close_scope();
        }

        if (kv.read("curve", "length", count)) {
            sc->curve.resize(count);
            Curve* vc = sc->curve.data();
            for (int32_t i = 0; i < count; i++) {
                if (!kv.open_scope_fmt(i))
                    continue;

                Curve* c = &vc[i];
                key_val_read(&kv, "cv", c->fcurve);
                kv.read("name", c->name);

                kv.close_scope();
            }
            kv.close_scope();
        }

        if (kv.read("event", "length", count)) {
            sc->event.resize(count);
            Event* ve = sc->event.data();
            for (int32_t i = 0; i < count; i++) {
                if (!kv.open_scope_fmt(i))
                    continue;

                Event* e = &ve[i];
                kv.read("begin", e->begin);
                kv.read("clip_begin", e->clip_begin);
                kv.read("clip_end", e->clip_end);
                kv.read("end", e->end);
                kv.read("name", e->name);
                kv.read("param1", e->param1);
                kv.read("ref", e->ref);
                kv.read("time_ref_scale", e->time_ref_scale);

                int32_t type;
                if (kv.read("type", type))
                    e->type = (EVENT_TYPE)type;

                kv.close_scope();
            }
            kv.close_scope();
        }

        if (kv.read("fog", "length", count)) {
            sc->fog.resize(count);
            Fog* vf = sc->fog.data();
            for (int32_t i = 0; i < count; i++) {
                if (!kv.open_scope_fmt(i))
                    continue;

                Fog* f = &vf[i];

                int32_t id;
                if (kv.read("id", id))
                    f->id = id;

                if (kv.read("name", f->name)) {
                    if (!f->name.compare("Z"))
                        f->id = FOG_DEPTH;
                    else if (!f->name.compare("Height"))
                        f->id = FOG_HEIGHT;
                }

                if (key_val_read(&kv, "density", f->density))
                    f->flag |= Fog::FLAG_DENSITY;
                if (key_val_read(&kv, "end", f->end))
                    f->flag |= Fog::FLAG_END;
                if (key_val_read(&kv, "start", f->start))
                    f->flag |= Fog::FLAG_START;
                if (key_val_read(&kv, "Diffuse", f->color))
                    f->flag |= Fog::FLAG_COLOR;

                kv.close_scope();
            }
            kv.close_scope();
        }

        if (kv.read("light", "length", count)) {
            sc->light.resize(count);
            Light* vl = sc->light.data();
            for (int32_t i = 0; i < count; i++) {
                if (!kv.open_scope_fmt(i))
                    continue;

                Light* l = &vl[i];
                int32_t id;
                if (kv.read("id", id))
                    l->id = id;

                kv.read("name", l->name);
                kv.read("type", l->type);

                key_val_read(&kv, "position", l->position);
                key_val_read(&kv, "spot_direction", l->spot_direction);

                if (key_val_read(&kv, "Ambient", l->ambient))
                    l->flag |= Light::FLAG_AMBIENT;
                if (key_val_read(&kv, "Diffuse", l->diffuse))
                    l->flag |= Light::FLAG_DIFFUSE;
                if (key_val_read(&kv, "Specular", l->specular))
                    l->flag |= Light::FLAG_SPECULAR;
                if (key_val_read(&kv, "Incandescence", l->tone_curve))
                    l->flag |= Light::FLAG_TONE_CURVE;

                // XHD
                if (key_val_read(&kv, "CONSTANT", l->constant))
                    l->flag |= Light::FLAG_CONSTANT;
                if (key_val_read(&kv, "Intensity", l->intensity))
                    l->flag |= Light::FLAG_INTENSITY;
                if (key_val_read(&kv, "FAR", l->far_))
                    l->flag |= Light::FLAG_FAR;
                if (key_val_read(&kv, "LINEAR", l->linear))
                    l->flag |= Light::FLAG_LINEAR;
                if (key_val_read(&kv, "QUADRATIC", l->quadratic))
                    l->flag |= Light::FLAG_QUADRATIC;
                if (key_val_read(&kv, "DropOff", l->drop_off))
                    l->flag |= Light::FLAG_DROP_OFF;
                if (key_val_read(&kv, "ConeAngle", l->cone_angle))
                    l->flag |= Light::FLAG_CONE_ANGLE;

                kv.close_scope();
            }
            kv.close_scope();
        }

        if (kv.read("m_objhrc", "length", count)) {
            sc->m_object_hrc.resize(count);
            MObjectHrc* vmoh = sc->m_object_hrc.data();
            for (int32_t i = 0; i < count; i++) {
                if (!kv.open_scope_fmt(i))
                    continue;

                MObjectHrc* moh = &vmoh[i];
                int32_t count;
                if (kv.read("instance", "length", count)) {
                    moh->instance.resize(count);
                    ObjectInstance* voi = moh->instance.data();
                    for (int32_t j = 0; j < count; j++) {
                        if (!kv.open_scope_fmt(j))
                            continue;

                        ObjectInstance* oi = &voi[j];
                        kv.read("name", oi->name);
                        kv.read("uid_name", oi->uid_name);
                        kv.read("shadow", oi->shadow);
                        key_val_read(&kv, "", oi->model_transform);

                        kv.close_scope();
                    }
                    kv.close_scope();
                }

                key_val_read(&kv, "", moh->model_transform);
                kv.read("name", moh->name);

                if (kv.read("node", "length", count)) {
                    moh->node.resize(count);
                    ObjectNode* von = moh->node.data();
                    for (int32_t j = 0; j < count; j++) {
                        if (!kv.open_scope_fmt(j))
                            continue;

                        ObjectNode* on = &von[j];
                        kv.read("name", on->name);
                        kv.read("parent", on->parent);
                        key_val_read(&kv, "", on->model_transform);
                        if (kv.read("joint_orient", on->joint_orient))
                            on->flag |= ObjectNode::FLAG_JOINT_ORIENT;

                        kv.close_scope();
                    }
                    kv.close_scope();
                }

                kv.close_scope();
            }
            kv.close_scope();
        }

        if (kv.read("m_objhrc_list", "length", count)) {
            sc->m_object_hrc_list.resize(count);
            std::string* vmohl = sc->m_object_hrc_list.data();
            for (int32_t i = 0; i < count; i++) {
                if (!kv.open_scope_fmt(i))
                    continue;

                kv.read(vmohl[i]);
                kv.close_scope();
            }
            kv.close_scope();
        }

        // X/XHD
        if (kv.read("material_list", "length", count)) {
            sc->material_list.resize(count);
            MaterialList* vml = sc->material_list.data();
            for (int32_t i = 0; i < count; i++) {
                if (!kv.open_scope_fmt(i))
                    continue;

                MaterialList* ml = &vml[i];
                kv.read("name", ml->name);

                if (key_val_read(&kv, "incandescence", ml->emission))
                    ml->flag |= MaterialList::FLAG_EMISSION;
                if (key_val_read(&kv, "blend_color", ml->blend_color))
                    ml->flag |= MaterialList::FLAG_BLEND_COLOR;
                if (key_val_read(&kv, "glow_intensity", ml->glow_intensity))
                    ml->flag |= MaterialList::FLAG_GLOW_INTENSITY;
                kv.close_scope();
            }
            kv.close_scope();
        }

        if (kv.read("motion", "length", count)) {
            sc->motion.resize(count);
            Motion* vm = sc->motion.data();
            for (int32_t i = 0; i < count; i++) {
                if (!kv.open_scope_fmt(i))
                    continue;

                Motion* m = &vm[i];
                kv.read(m->name);
                kv.close_scope();
            }
            kv.close_scope();
        }

        if (kv.read("object", "length", count)) {
            sc->object.resize(count);
            Object* vo = sc->object.data();
            for (int32_t i = 0; i < count; i++) {
                if (!kv.open_scope_fmt(i))
                    continue;

                Object* o = &vo[i];
                kv.read("name", o->name);
                kv.read("uid_name", o->uid_name);
                key_val_read(&kv, "", o->model_transform);
                if (kv.read("morph", o->morph))
                    kv.read("morph_offset", o->morph_offset);
                if (kv.read("pat", o->pattern))
                    kv.read("pat_offset", o->pattern_offset);
                kv.read("parent_name", o->parent_name);
                kv.read("parent_node", o->parent_node);

                int32_t count;
                if (kv.read("tex_pat", "length", count)) {
                    o->texture.resize(count);
                    Texture* votp = o->texture.data();
                    for (int32_t j = 0; j < count; j++) {
                        if (!kv.open_scope_fmt(j))
                            continue;

                        Texture* otp = &votp[j];
                        kv.read("name", otp->name);
                        if (kv.read("pat", otp->pattern))
                            kv.read("pat_offset", otp->pattern_offset);

                        kv.close_scope();
                    }
                    kv.close_scope();
                }

                if (kv.read("tex_transform", "length", count)) {
                    o->texture_transform.resize(count);
                    TextureTransform* vott = o->texture_transform.data();
                    for (int32_t j = 0; j < count; j++) {
                        if (!kv.open_scope_fmt(j))
                            continue;

                        TextureTransform* ott = &vott[j];
                        kv.read("name", ott->name);

                        if (key_val_read(&kv, "coverageU", ott->coverage_u))
                            ott->flag |= TextureTransform::FLAG_COVERAGE_U;
                        if (key_val_read(&kv, "coverageV", ott->coverage_v))
                            ott->flag |= TextureTransform::FLAG_COVERAGE_V;
                        if (key_val_read(&kv, "repeatU", ott->repeat_u))
                            ott->flag |= TextureTransform::FLAG_REPEAT_U;
                        if (key_val_read(&kv, "repeatV", ott->repeat_v))
                            ott->flag |= TextureTransform::FLAG_REPEAT_V;
                        if (key_val_read(&kv, "rotate", ott->rotate))
                            ott->flag |= TextureTransform::FLAG_ROTATE;
                        if (key_val_read(&kv, "rotateFrame", ott->rotate_frame))
                            ott->flag |= TextureTransform::FLAG_ROTATE_FRAME;
                        if (key_val_read(&kv, "offsetU", ott->offset_u))
                            ott->flag |= TextureTransform::FLAG_OFFSET_U;
                        if (key_val_read(&kv, "offsetV", ott->offset_v))
                            ott->flag |= TextureTransform::FLAG_OFFSET_V;
                        if (key_val_read(&kv, "translateFrameU", ott->translate_frame_u))
                            ott->flag |= TextureTransform::FLAG_TRANSLATE_FRAME_U;
                        if (key_val_read(&kv, "translateFrameV", ott->translate_frame_v))
                            ott->flag |= TextureTransform::FLAG_TRANSLATE_FRAME_V;

                        kv.close_scope();
                    }
                    kv.close_scope();
                }

                kv.close_scope();
            }
            kv.close_scope();
        }

        if (kv.read("object_list", "length", count)) {
            sc->object_list.resize(count);
            std::string* vol = sc->object_list.data();
            for (int32_t i = 0; i < count; i++) {
                if (!kv.open_scope_fmt(i))
                    continue;

                kv.read(vol[i]);
                kv.close_scope();
            }
            kv.close_scope();
        }

        if (kv.read("objhrc", "length", count)) {
            sc->object_hrc.resize(count);
            ObjectHrc* voh = sc->object_hrc.data();
            for (int32_t i = 0; i < count; i++) {
                if (!kv.open_scope_fmt(i))
                    continue;

                ObjectHrc* oh = &voh[i];
                kv.read("name", oh->name);
                kv.read("uid_name", oh->uid_name);

                kv.read("shadow", oh->shadow);

                int32_t count;
                if (kv.read("node", "length", count)) {
                    oh->node.resize(count);
                    ObjectNode* vohn = oh->node.data();
                    for (int32_t j = 0; j < count; j++) {
                        if (!kv.open_scope_fmt(j))
                            continue;

                        ObjectNode* ohn = &vohn[j];
                        kv.read("name", ohn->name);
                        kv.read("parent", ohn->parent);
                        key_val_read(&kv, "", ohn->model_transform);
                        if (kv.read("joint_orient", ohn->joint_orient))
                            ohn->flag |= ObjectNode::FLAG_JOINT_ORIENT;

                        kv.close_scope();
                    }
                    kv.close_scope();
                }

                kv.read("parent_name", oh->parent_name);
                kv.read("parent_node", oh->parent_node);

                kv.close_scope();
            }
            kv.close_scope();
        }

        if (kv.read("objhrc_list", "length", count)) {
            sc->object_hrc_list.resize(count);
            std::string* vohl = sc->object_hrc_list.data();
            for (int32_t i = 0; i < count; i++) {
                if (!kv.open_scope_fmt(i))
                    continue;

                kv.read(vohl[i]);
                kv.close_scope();
            }
            kv.close_scope();
        }

        if (kv.read("point", "length", count)) {
            sc->point.resize(count);
            Point* vp = sc->point.data();
            for (int32_t i = 0; i < count; i++) {
                if (!kv.open_scope_fmt(i))
                    continue;

                Point* p = &vp[i];
                key_val_read(&kv, "", p->model_transform);
                kv.read("name", p->name);

                kv.close_scope();
            }
            kv.close_scope();
        }
    }

    static void a3da_write_text(Scene* sc, void** data, size_t* size, bool a3dc) {
        char a3da_timestamp[0x100];
        a3da_get_time_stamp(a3da_timestamp, 0x100);

        memory_stream s;
        s.open();

        if (a3dc) {
            if (sc->_compress_f16 != COMPRESS_F32F32F32F32)
                s.write("#-compress_f16\n", 15);
        }
        else
            s.write("#A3DA__________\n", 16);

        s.write_utf8_string(a3da_timestamp);

        key_val_out kv;

        {
            kv.open_scope("_");

            if (a3dc && sc->format > FORMAT_AFT && sc->format != FORMAT_AFT_X_PACK)
                kv.write(s, "compress_f16", sc->_compress_f16);
            kv.write(s, "converter.version", sc->_converter_version);
            kv.write(s, "file_name", sc->_file_name);
            kv.write(s, "property.version", sc->_property_version);

            kv.close_scope();
        }

        // MGF
        if (sc->ambient.size() && sc->format == FORMAT_MGF) {
            kv.open_scope("ambient");

            int32_t count = (int32_t)sc->ambient.size();
            Ambient* va = sc->ambient.data();

            std::vector<int32_t> sort_index;
            key_val_out::get_lexicographic_order(sort_index, count);
            int32_t* sort_index_data = sort_index.data();
            for (int32_t i = 0; i < count; i++) {
                kv.open_scope_fmt(sort_index_data[i]);

                Ambient* sc = &va[sort_index_data[i]];
                if (sc->flag & Ambient::FLAG_LIGHT_DIFFUSE)
                    key_val_out_write(&kv, s, "light.Diffuse", sc->light_diffuse);
                kv.write(s, "name", sc->name);
                if (sc->flag & Ambient::FLAG_RIM_LIGHT_DIFFUSE)
                    key_val_out_write(&kv, s, "rimlight.Diffuse", sc->rim_light_diffuse);

                kv.close_scope();
            }

            kv.write(s, "length", count);
            kv.close_scope();
        }

        if (sc->auth_2d.size()) {
            kv.open_scope("auth_2d");

            int32_t count = (int32_t)sc->auth_2d.size();
            Auth2d* va2 = sc->auth_2d.data();

            std::vector<int32_t> sort_index;
            key_val_out::get_lexicographic_order(sort_index, count);
            int32_t* sort_index_data = sort_index.data();
            for (int32_t i = 0; i < count; i++) {
                kv.open_scope_fmt(sort_index_data[i]);
                Auth2d* a2 = &va2[sort_index_data[i]];
                kv.write(s, "name", a2->name);
                kv.close_scope();
            }

            kv.write(s, "length", count);
            kv.close_scope();
        }

        if (sc->camera_auxiliary.flag) {
            kv.open_scope("camera_auxiliary");

            CameraAuxiliary* ca = &sc->camera_auxiliary;
            if (sc->format == FORMAT_F || sc->format > FORMAT_AFT && sc->format != FORMAT_AFT_X_PACK) {
                // F/F2/X
                if (ca->flag & CameraAuxiliary::FLAG_EXPOSURE_RATE)
                    key_val_out_write(&kv, s, "exposure_rate", ca->exposure_rate, true);
                if (ca->flag & CameraAuxiliary::FLAG_GAMMA_RATE)
                    key_val_out_write(&kv, s, "gamma_rate", ca->gamma_rate, true);
                if (ca->flag & CameraAuxiliary::FLAG_SATURATE)
                    key_val_out_write(&kv, s, "saturate", ca->saturate, true);
            }
            else {
                if (ca->flag & CameraAuxiliary::FLAG_AUTO_EXPOSURE)
                    key_val_out_write(&kv, s, "auto_exposure", ca->auto_exposure, true);
                if (ca->flag & CameraAuxiliary::FLAG_EXPOSURE)
                    key_val_out_write(&kv, s, "exposure", ca->exposure, true);
                if (ca->flag & CameraAuxiliary::FLAG_GAMMA)
                    key_val_out_write(&kv, s, "gamma", ca->gamma, true);
                if (ca->flag & CameraAuxiliary::FLAG_SATURATE)
                    key_val_out_write(&kv, s, "saturate", ca->saturate, true);
            }

            kv.close_scope();
        }

        if (sc->camera_root.size() > 0) {
            kv.open_scope("camera_root");

            int32_t count = (int32_t)sc->camera_root.size();
            CameraRoot* vcr = sc->camera_root.data();

            std::vector<int32_t> sort_index;
            key_val_out::get_lexicographic_order(sort_index, count);
            int32_t* sort_index_data = sort_index.data();
            for (int32_t i = 0; i < count; i++) {
                kv.open_scope_fmt(sort_index_data[i]);

                CameraRoot* cr = &vcr[sort_index_data[i]];
                CameraInterest* intr = &cr->interest;
                key_val_out_write(&kv, s, "interest", intr->model_transform, 0x1F);

                key_val_out_write(&kv, s, "", cr->model_transform, 0x1E);

                {
                    kv.open_scope("view_point");

                    CameraViewPoint* vp = &cr->view_point;
                    if (vp->has_fov) {
                        kv.write(s, "aspect", vp->aspect);
                        key_val_out_write(&kv, s, "fov", vp->fov);
                        kv.write(s, "fov_is_horizontal", vp->fov_is_horizontal);
                    }
                    else {
                        kv.write(s, "camera_aperture_h", vp->camera_aperture_h);
                        kv.write(s, "camera_aperture_w", vp->camera_aperture_w);
                        key_val_out_write(&kv, s, "focal_length", vp->focal_length);
                    }
                    key_val_out_write(&kv, s, "", vp->model_transform, 0x10);
                    key_val_out_write(&kv, s, "roll", vp->roll);
                    key_val_out_write(&kv, s, "", vp->model_transform, 0x0F);

                    kv.close_scope();
                }

                key_val_out_write(&kv, s, "", cr->model_transform, 0x01);

                kv.close_scope();
            }

            kv.write(s, "length", count);
            kv.close_scope();
        }

        if (sc->chara.size() > 0) {
            kv.open_scope("chara");

            int32_t count = (int32_t)sc->chara.size();
            Chara* vc = sc->chara.data();

            std::vector<int32_t> sort_index;
            key_val_out::get_lexicographic_order(sort_index, count);
            int32_t* sort_index_data = sort_index.data();
            for (int32_t i = 0; i < count; i++) {
                kv.open_scope_fmt(sort_index_data[i]);

                Chara* c = &vc[sort_index_data[i]];
                kv.write(s, "name", c->name);
                key_val_out_write(&kv, s, "", c->model_transform);

                kv.close_scope();
            }

            kv.write(s, "length", count);
            kv.close_scope();
        }

        // X/XHD
        if (sc->format == FORMAT_AFT_X_PACK && (sc->curve.size() > 0 || sc->material_list.size() > 0)) {
            kv.open_scope("curve");

            int32_t curve_count = (int32_t)sc->curve.size();
            int32_t mat_list_count = (int32_t)sc->material_list.size();
            Curve* vc = sc->curve.data();
            MaterialList* vml = sc->material_list.data();

            std::vector<int32_t> sort_index;
            key_val_out::get_lexicographic_order(sort_index, curve_count + mat_list_count);
            int32_t* sort_index_data = sort_index.data();
            for (int32_t i = 0; i < curve_count + mat_list_count; i++) {
                kv.open_scope_fmt(sort_index_data[i]);

                if (sort_index_data[i] < curve_count) {
                    Curve* c = &vc[sort_index_data[i]];
                    key_val_out_write(&kv, s, "cv", c->fcurve);
                    kv.write(s, "name", c->name);
                }
                else {
                    MaterialList* ml = &vml[sort_index_data[i] - curve_count];
                    kv.open_scope("ml");
                    kv.write(s, "true");
                    if (ml->flag & MaterialList::FLAG_BLEND_COLOR)
                        key_val_out_write(&kv, s, "blend_color", ml->blend_color);
                    if (ml->flag & MaterialList::FLAG_EMISSION)
                        key_val_out_write(&kv, s, "emission", ml->emission);
                    kv.close_scope();
                    kv.write(s, "name", ml->name);
                }

                kv.close_scope();
            }

            kv.write(s, "length", curve_count + mat_list_count);
            kv.close_scope();
        }
        else if (sc->curve.size() > 0) {
            kv.open_scope("curve");

            int32_t count = (int32_t)sc->curve.size();
            Curve* vc = sc->curve.data();

            std::vector<int32_t> sort_index;
            key_val_out::get_lexicographic_order(sort_index, count);
            int32_t* sort_index_data = sort_index.data();
            for (int32_t i = 0; i < count; i++) {
                kv.open_scope_fmt(sort_index_data[i]);

                Curve* c = &vc[sort_index_data[i]];
                key_val_out_write(&kv, s, "cv", c->fcurve);
                kv.write(s, "name", c->name);

                kv.close_scope();
            }

            kv.write(s, "length", count);
            kv.close_scope();
        }

        if (sc->dof.has_dof && (sc->format == FORMAT_AFT || sc->format == FORMAT_AFT_X_PACK)) {
            kv.open_scope("dof");

            Dof* d = &sc->dof;
            kv.write(s, "name", "DOF");
            key_val_out_write(&kv, s, "", d->model_transform);

            kv.close_scope();
        }

        if (sc->event.size() > 0) {
            kv.open_scope("event");

            int32_t count = (int32_t)sc->event.size();
            Event* ve = sc->event.data();

            std::vector<int32_t> sort_index;
            key_val_out::get_lexicographic_order(sort_index, count);
            int32_t* sort_index_data = sort_index.data();
            for (int32_t i = 0; i < count; i++) {
                kv.open_scope_fmt(sort_index_data[i]);

                Event* e = &ve[sort_index_data[i]];
                kv.write(s, "begin", e->begin);
                kv.write(s, "clip_begin", e->clip_begin);
                kv.write(s, "clip_en", e->clip_end);
                kv.write(s, "end", e->end);
                kv.write(s, "name", e->name);
                kv.write(s, "param1", e->param1);
                kv.write(s, "ref", e->ref);
                kv.write(s, "time_ref_scale", e->time_ref_scale);
                kv.write(s, "type", e->type);

                kv.close_scope();
            }

            kv.write(s, "length", count);
            kv.close_scope();
        }

        if (sc->fog.size() > 0) {
            kv.open_scope("fog");

            int32_t count = (int32_t)sc->fog.size();
            Fog* vf = sc->fog.data();

            std::vector<int32_t> sort_index;
            key_val_out::get_lexicographic_order(sort_index, count);
            int32_t* sort_index_data = sort_index.data();
            for (int32_t i = 0; i < count; i++) {
                kv.open_scope_fmt(sort_index_data[i]);

                Fog* f = &vf[sort_index_data[i]];
                if (f->flag & Fog::FLAG_COLOR)
                    key_val_out_write(&kv, s, "Diffuse", f->color);
                if (f->flag & Fog::FLAG_DENSITY)
                    key_val_out_write(&kv, s, "density", f->density, true);
                if (f->flag & Fog::FLAG_END)
                    key_val_out_write(&kv, s, "end", f->end, true);
                kv.write(s, "id", f->id);
                if (f->flag & Fog::FLAG_START)
                    key_val_out_write(&kv, s, "start", f->start, true);

                kv.close_scope();
            }

            kv.write(s, "length", count);
            kv.close_scope();
        }

        if (sc->light.size() > 0) {
            kv.open_scope("light");

            bool xhd = sc->format == FORMAT_XHD;

            int32_t count = (int32_t)sc->light.size();
            Light* vl = sc->light.data();

            std::vector<int32_t> sort_index;
            key_val_out::get_lexicographic_order(sort_index, count);
            int32_t* sort_index_data = sort_index.data();
            for (int32_t i = 0; i < count; i++) {
                kv.open_scope_fmt(sort_index_data[i]);

                Light* l = &vl[sort_index_data[i]];
                if (l->flag & Light::FLAG_AMBIENT)
                    key_val_out_write(&kv, s, "Ambient", l->ambient);
                if (l->flag & Light::FLAG_CONSTANT && xhd) // XHD
                    key_val_out_write(&kv, s, "CONSTANT", l->constant, true);
                if (l->flag & Light::FLAG_CONE_ANGLE && xhd) // XHD
                    key_val_out_write(&kv, s, "ConeAngle", l->cone_angle, true);
                if (l->flag & Light::FLAG_DIFFUSE)
                    key_val_out_write(&kv, s, "Diffuse", l->diffuse);
                if (l->flag & Light::FLAG_DROP_OFF && xhd) // XHD
                    key_val_out_write(&kv, s, "DropOff", l->drop_off, true);
                if (l->flag & Light::FLAG_FAR && xhd) // XHD
                    key_val_out_write(&kv, s, "FAR", l->far_, true);
                if (l->flag & Light::FLAG_TONE_CURVE)
                    key_val_out_write(&kv, s, "Incandescence", l->tone_curve);
                if (l->flag & Light::FLAG_INTENSITY && xhd) // XHD
                    key_val_out_write(&kv, s, "Intensity", l->intensity, true);
                if (l->flag & Light::FLAG_LINEAR && xhd) // XHD
                    key_val_out_write(&kv, s, "LINEAR", l->linear, true);
                if (l->flag & Light::FLAG_QUADRATIC && xhd) // XHD
                    key_val_out_write(&kv, s, "QUADRATIC", l->quadratic, true);
                if (l->flag & Light::FLAG_SPECULAR)
                    key_val_out_write(&kv, s, "Specular", l->specular);
                kv.write(s, "id", l->id);
                const char* name = "none";
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
                kv.write(s, "name", name);
                if (l->position.flag)
                    key_val_out_write(&kv, s, "position", l->position);
                if (l->spot_direction.flag)
                    key_val_out_write(&kv, s, "spot_direction", l->spot_direction);
                kv.write(s, "type", l->type);

                kv.close_scope();
            }

            kv.write(s, "length", count);
            kv.close_scope();
        }

        if (sc->m_object_hrc.size() > 0) {
            kv.open_scope("m_objhrc");

            int32_t count = (int32_t)sc->m_object_hrc.size();
            MObjectHrc* vmoh = sc->m_object_hrc.data();

            std::vector<int32_t> sort_index;
            key_val_out::get_lexicographic_order(sort_index, count);
            int32_t* sort_index_data = sort_index.data();
            for (int32_t i = 0; i < count; i++) {
                kv.open_scope_fmt(sort_index_data[i]);

                MObjectHrc* moh = &vmoh[sort_index_data[i]];
                if (moh->instance.size()) {
                    kv.open_scope("instance");

                    int32_t count = (int32_t)moh->instance.size();
                    ObjectInstance* voi = moh->instance.data();

                    std::vector<int32_t> sort_index;
                    key_val_out::get_lexicographic_order(sort_index, count);
                    int32_t* sort_index_data = sort_index.data();
                    for (int32_t j = 0; j < count; j++) {
                        kv.open_scope_fmt(sort_index_data[j]);

                        ObjectInstance* oi = &voi[sort_index_data[j]];
                        key_val_out_write(&kv, s, "", oi->model_transform, 0x10);
                        kv.write(s, "name", oi->name);
                        key_val_out_write(&kv, s, "", oi->model_transform, 0x0C);
                        kv.write(s, "shadow", oi->shadow);
                        key_val_out_write(&kv, s, "", oi->model_transform, 0x02);
                        kv.write(s, "uid_name", oi->uid_name);
                        key_val_out_write(&kv, s, "", oi->model_transform, 0x01);

                        kv.close_scope();
                    }

                    kv.write(s, "length", count);
                    kv.close_scope();
                }

                key_val_out_write(&kv, s, "", moh->model_transform, 0x10);
                kv.write(s, "name", moh->name);

                if (moh->node.size()) {
                    kv.open_scope("node");

                    int32_t count = (int32_t)moh->node.size();
                    ObjectNode* von = moh->node.data();

                    std::vector<int32_t> sort_index;
                    key_val_out::get_lexicographic_order(sort_index, count);
                    int32_t* sort_index_data = sort_index.data();
                    for (int32_t j = 0; j < count; j++) {
                        kv.open_scope_fmt(sort_index_data[j]);

                        ObjectNode* on = &von[sort_index_data[j]];
                        key_val_out_write(&kv, s, "", on->model_transform, 0x10);
                        if (on->flag & ObjectNode::FLAG_JOINT_ORIENT)
                            kv.write(s, "joint_orient", on->joint_orient);
                        kv.write(s, "name", on->name);
                        kv.write(s, "parent", on->parent);
                        key_val_out_write(&kv, s, "", on->model_transform, 0x0F);

                        kv.close_scope();
                    }

                    kv.write(s, "length", count);
                    kv.close_scope();
                }

                key_val_out_write(&kv, s, "", moh->model_transform, 0x0F);

                kv.close_scope();
            }

            kv.write(s, "length", count);
            kv.close_scope();
        }

        if (sc->m_object_hrc_list.size()) {
            kv.open_scope("m_objhrc_list");

            int32_t count = (int32_t)sc->m_object_hrc_list.size();
            std::string* vmohl = sc->m_object_hrc_list.data();

            std::vector<int32_t> sort_index;
            key_val_out::get_lexicographic_order(sort_index, count);
            int32_t* sort_index_data = sort_index.data();
            for (int32_t i = 0; i < count; i++) {
                kv.open_scope_fmt(sort_index_data[i]);
                kv.write(s, vmohl[sort_index_data[i]]);
                kv.close_scope();
            }

            kv.write(s, "length", count);
            kv.close_scope();
        }

        // X/XHD
        if (sc->material_list.size() > 0 && (sc->format == FORMAT_X || sc->format == FORMAT_XHD)) {
            kv.open_scope("material_list");

            int32_t count = (int32_t)sc->material_list.size();
            MaterialList* vml = sc->material_list.data();

            std::vector<int32_t> sort_index;
            key_val_out::get_lexicographic_order(sort_index, count);
            int32_t* sort_index_data = sort_index.data();
            for (int32_t i = 0; i < count; i++) {
                kv.open_scope_fmt(sort_index_data[i]);

                MaterialList* ml = &vml[sort_index_data[i]];
                if (ml->flag & MaterialList::FLAG_BLEND_COLOR)
                    key_val_out_write(&kv, s, "blend_color", ml->blend_color);
                if (ml->flag & MaterialList::FLAG_GLOW_INTENSITY)
                    key_val_out_write(&kv, s, "glow_intensity", ml->glow_intensity, true);
                kv.write(s, "hash_name", hash_string_murmurhash(ml->name));
                if (ml->flag & MaterialList::FLAG_EMISSION)
                    key_val_out_write(&kv, s, "incandescence", ml->emission);
                kv.write(s, "name", ml->name);

                kv.close_scope();
            }

            kv.write(s, "length", count);
            kv.close_scope();
        }

        if (sc->motion.size()) {
            kv.open_scope("motion");

            int32_t count = (int32_t)sc->motion.size();
            Motion* vm = sc->motion.data();

            std::vector<int32_t> sort_index;
            key_val_out::get_lexicographic_order(sort_index, count);
            int32_t* sort_index_data = sort_index.data();
            for (int32_t i = 0; i < count; i++) {
                kv.open_scope_fmt(sort_index_data[i]);
                Motion* m = &vm[sort_index_data[i]];
                kv.write(s, "name", m->name);
                kv.close_scope();
            }

            kv.write(s, "length", count);
            kv.close_scope();
        }

        if (sc->object.size() > 0) {
            kv.open_scope("object");

            int32_t count = (int32_t)sc->object.size();
            Object* vo = sc->object.data();

            std::vector<int32_t> sort_index;
            key_val_out::get_lexicographic_order(sort_index, count);
            int32_t* sort_index_data = sort_index.data();
            for (int32_t i = 0; i < count; i++) {
                kv.open_scope_fmt(sort_index_data[i]);

                Object* o = &vo[sort_index_data[i]];
                key_val_out_write(&kv, s, "", o->model_transform, 0x10);
                if (o->morph.size()) {
                    kv.write(s, "morph", o->morph);
                    kv.write(s, "morph_offset", o->morph_offset);
                }
                kv.write(s, "name", o->name);
                if (o->parent_name.size())
                    kv.write(s, "parent_name", o->parent_name);
                if (o->parent_node.size())
                    kv.write(s, "parent_node", o->parent_node);
                if (o->pattern.size()) {
                    kv.write(s, "pat", o->pattern);
                    kv.write(s, "pat_offset",  o->pattern_offset);
                }
                key_val_out_write(&kv, s, "", o->model_transform, 0x0C);

                if (o->texture.size()) {
                    kv.open_scope("tex_pat");

                    int32_t count = (int32_t)o->texture.size();
                    Texture* votp = o->texture.data();

                    std::vector<int32_t> sort_index;
                    key_val_out::get_lexicographic_order(sort_index, count);
                    int32_t* sort_index_data = sort_index.data();
                    for (int32_t j = 0; j < count; j++) {
                        kv.open_scope_fmt(sort_index_data[j]);

                        Texture* otp = &votp[sort_index_data[j]];
                        kv.write(s, "name", otp->name);
                        if (otp->pattern.size()) {
                            kv.write(s, "pat", otp->pattern);
                            kv.write(s, "pat_offset", otp->pattern_offset);
                        }

                        kv.close_scope();
                    }

                    kv.write(s, "length", count);
                    kv.close_scope();
                }

                if (o->texture_transform.size()) {
                    kv.open_scope("tex_transform");

                    int32_t count = (int32_t)o->texture_transform.size();
                    TextureTransform* vott = o->texture_transform.data();

                    std::vector<int32_t> sort_index;
                    key_val_out::get_lexicographic_order(sort_index, count);
                    int32_t* sort_index_data = sort_index.data();
                    for (int32_t j = 0; j < count; j++) {
                        kv.open_scope_fmt(sort_index_data[j]);

                        TextureTransform* ott = &vott[sort_index_data[j]];
                        if (ott->flag & TextureTransform::FLAG_COVERAGE_U)
                            key_val_out_write(&kv, s, "coverageU", ott->coverage_u, true);
                        if (ott->flag & TextureTransform::FLAG_COVERAGE_V)
                            key_val_out_write(&kv, s, "coverageV", ott->coverage_v, true);
                        kv.write(s, "name", ott->name);
                        if (ott->flag & TextureTransform::FLAG_OFFSET_U)
                            key_val_out_write(&kv, s, "offsetU", ott->offset_u, true);
                        if (ott->flag & TextureTransform::FLAG_OFFSET_V)
                            key_val_out_write(&kv, s, "offsetV", ott->offset_v, true);
                        if (ott->flag & TextureTransform::FLAG_REPEAT_U)
                            key_val_out_write(&kv, s, "repeatU", ott->repeat_u, true);
                        if (ott->flag & TextureTransform::FLAG_REPEAT_V)
                            key_val_out_write(&kv, s, "repeatV", ott->repeat_v, true);
                        if (ott->flag & TextureTransform::FLAG_ROTATE)
                            key_val_out_write(&kv, s, "rotate", ott->rotate, true);
                        if (ott->flag & TextureTransform::FLAG_ROTATE_FRAME)
                            key_val_out_write(&kv, s, "rotateFrame", ott->rotate_frame, true);
                        if (ott->flag & TextureTransform::FLAG_TRANSLATE_FRAME_U)
                            key_val_out_write(&kv, s, "translateFrameU", ott->translate_frame_u, true);
                        if (ott->flag & TextureTransform::FLAG_TRANSLATE_FRAME_V)
                            key_val_out_write(&kv, s, "translateFrameV", ott->translate_frame_v, true);

                        kv.close_scope();
                    }

                    kv.write(s, "length", count);
                    kv.close_scope();
                }

                key_val_out_write(&kv, s, "", o->model_transform, 0x02);
                kv.write(s, "uid_name", o->uid_name);
                key_val_out_write(&kv, s, "", o->model_transform, 0x01);

                kv.close_scope();
            }

            kv.write(s, "length", count);
            kv.close_scope();
        }

        if (sc->object_list.size()) {
            kv.open_scope("object_list");

            int32_t count = (int32_t)sc->object_list.size();
            std::string* vol = sc->object_list.data();

            std::vector<int32_t> sort_index;
            key_val_out::get_lexicographic_order(sort_index, count);
            int32_t* sort_index_data = sort_index.data();
            for (int32_t i = 0; i < count; i++) {
                kv.open_scope_fmt(sort_index_data[i]);
                kv.write(s, vol[sort_index_data[i]]);
                kv.close_scope();
            }

            kv.write(s, "length", count);
            kv.close_scope();
        }

        if (sc->object_hrc.size() > 0) {
            kv.open_scope("objhrc");

            int32_t count = (int32_t)sc->object_hrc.size();
            ObjectHrc* voh = sc->object_hrc.data();

            std::vector<int32_t> sort_index;
            key_val_out::get_lexicographic_order(sort_index, count);
            int32_t* sort_index_data = sort_index.data();
            for (int32_t i = 0; i < count; i++) {
                kv.open_scope_fmt(sort_index_data[i]);

                ObjectHrc* oh = &voh[sort_index_data[i]];
                kv.write(s, "name", oh->name);

                if (oh->node.size()) {
                    kv.open_scope("node");

                    int32_t count = (int32_t)oh->node.size();
                    ObjectNode* vohn = oh->node.data();

                    std::vector<int32_t> sort_index;
                    key_val_out::get_lexicographic_order(sort_index, count);
                    int32_t* sort_index_data = sort_index.data();
                    for (int32_t j = 0; j < count; j++) {
                        kv.open_scope_fmt(sort_index_data[j]);

                        ObjectNode* ohn = &vohn[sort_index_data[j]];
                        key_val_out_write(&kv, s, "", ohn->model_transform, 0x10);
                        if (ohn->flag & ObjectNode::FLAG_JOINT_ORIENT)
                            kv.write(s, "joint_orient", ohn->joint_orient);
                        kv.write(s, "name", ohn->name);
                        kv.write(s, "parent", ohn->parent);
                        key_val_out_write(&kv, s, "", ohn->model_transform, 0x0F);

                        kv.close_scope();
                    }

                    kv.write(s, "length", count);
                    kv.close_scope();
                }

                if (oh->parent_name.size())
                    kv.write(s, "parent_name", oh->parent_name);
                if (oh->parent_node.size())
                    kv.write(s, "parent_node", oh->parent_node);
                kv.write(s, "shadow", oh->shadow);
                kv.write(s, "uid_name", oh->uid_name);

                kv.close_scope();
            }

            kv.write(s, "length", count);
            kv.close_scope();
        }

        if (sc->object_hrc_list.size()) {
            kv.open_scope("objhrc_list");

            int32_t count = (int32_t)sc->object_hrc_list.size();
            std::string* vohl = sc->object_hrc_list.data();

            std::vector<int32_t> sort_index;
            key_val_out::get_lexicographic_order(sort_index, count);
            int32_t* sort_index_data = sort_index.data();
            for (int32_t i = 0; i < count; i++) {
                kv.open_scope_fmt(sort_index_data[i]);
                kv.write(s, vohl[sort_index_data[i]]);
                kv.close_scope();
            }

            kv.write(s, "length", count);
            kv.close_scope();
        }

        {
            kv.open_scope("play_control");

            PlayControl* pc = &sc->play_control;
            kv.write(s, "begin", pc->begin);
            if (pc->flag & PlayControl::FLAG_DIV && sc->format > FORMAT_AFT
                && sc->format != FORMAT_AFT_X_PACK)
                kv.write(s, "div", pc->div);
            kv.write(s, "fps", pc->fps);
            if (pc->flag & PlayControl::FLAG_OFFSET) {
                if (sc->format > FORMAT_AFT && sc->format != FORMAT_AFT_X_PACK) {
                    kv.write(s, "offset", pc->offset);
                    kv.write(s, "size", pc->size);
                }
                else
                    kv.write(s, "size", pc->size + pc->offset);
            }
            else
                kv.write(s, "size", pc->size);

            kv.close_scope();
        }

        if (sc->post_process.flag) {
            kv.open_scope("post_process");

            PostProcess* pp = &sc->post_process;
            if (sc->post_process.flag & PostProcess::FLAG_RADIUS)
                key_val_out_write(&kv, s, "Ambient", pp->radius);
            if (sc->post_process.flag & PostProcess::FLAG_INTENSITY)
                key_val_out_write(&kv, s, "Diffuse", pp->intensity);
            if (sc->post_process.flag & PostProcess::FLAG_SCENE_FADE)
                key_val_out_write(&kv, s, "Specular", pp->scene_fade);
            if (sc->post_process.flag & PostProcess::FLAG_LENS_FLARE)
                key_val_out_write(&kv, s, "lens_flare", pp->lens_flare, true);
            if (sc->post_process.flag & PostProcess::FLAG_LENS_GHOST)
                key_val_out_write(&kv, s, "lens_ghost", pp->lens_ghost, true);
            if (sc->post_process.flag & PostProcess::FLAG_LENS_SHAFT)
                key_val_out_write(&kv, s, "lens_shaft", pp->lens_shaft, true);

            kv.close_scope();
        }

        if (sc->point.size() > 0) {
            kv.open_scope("point");

            int32_t count = (int32_t)sc->point.size();
            Point* vp = sc->point.data();

            std::vector<int32_t> sort_index;
            key_val_out::get_lexicographic_order(sort_index, count);
            int32_t* sort_index_data = sort_index.data();
            for (int32_t i = 0; i < count; i++) {
                kv.open_scope_fmt(sort_index_data[i]);

                Point* p = &vp[sort_index_data[i]];
                kv.write(s, p->name);
                key_val_out_write(&kv, s, "", p->model_transform);

                kv.close_scope();
            }

            kv.write(s, "length", count);
            kv.close_scope();
        }

        s.copy(data, size);
    }

    static void a3da_read_data(Scene* sc, void* data, size_t size) {
        COMPRESS_TYPE& _compress_f16 = sc->_compress_f16;

        // MGF
        for (Ambient& i : sc->ambient) {
            if (i.flag & Ambient::FLAG_LIGHT_DIFFUSE)
                a3dc_read_a3da_rgba(data, size, &i.light_diffuse);
            if (i.flag & Ambient::FLAG_RIM_LIGHT_DIFFUSE)
                a3dc_read_a3da_rgba(data, size, &i.rim_light_diffuse);
        }

        if (sc->camera_auxiliary.flag) {
            CameraAuxiliary* ca = &sc->camera_auxiliary;

            if (ca->flag & CameraAuxiliary::FLAG_EXPOSURE)
                a3dc_read_a3da_key(data, size, &ca->exposure);
            if (ca->flag & CameraAuxiliary::FLAG_EXPOSURE_RATE) // F/F2/X
                a3dc_read_a3da_key(data, size, &ca->exposure_rate);
            if (ca->flag & CameraAuxiliary::FLAG_GAMMA)
                a3dc_read_a3da_key(data, size, &ca->gamma);
            if (ca->flag & CameraAuxiliary::FLAG_GAMMA_RATE) // F/F2/X
                a3dc_read_a3da_key(data, size, &ca->gamma_rate);
            if (ca->flag & CameraAuxiliary::FLAG_SATURATE)
                a3dc_read_a3da_key(data, size, &ca->saturate);
            if (ca->flag & CameraAuxiliary::FLAG_AUTO_EXPOSURE)
                a3dc_read_a3da_key(data, size, &ca->auto_exposure);
        }

        for (CameraRoot& i : sc->camera_root) {
            a3dc_read_a3da_model_transform(data, size, &i.model_transform, _compress_f16);

            CameraInterest& intr = i.interest;
            a3dc_read_a3da_model_transform(data, size, &intr.model_transform, _compress_f16);

            CameraViewPoint& vp = i.view_point;
            a3dc_read_a3da_model_transform(data, size, &vp.model_transform, _compress_f16);
            a3dc_read_a3da_key(data, size, &vp.roll);

            if (vp.has_fov)
                a3dc_read_a3da_key(data, size, &vp.fov);
            else
                a3dc_read_a3da_key(data, size, &vp.focal_length);
        }

        for (Chara& i : sc->chara)
            a3dc_read_a3da_model_transform(data, size, &i.model_transform, _compress_f16);

        for (Curve& i : sc->curve)
            a3dc_read_a3da_key(data, size, &i.fcurve);

        if (sc->dof.has_dof) {
            Dof* d = &sc->dof;

            a3dc_read_a3da_model_transform(data, size, &d->model_transform, _compress_f16);
        }

        for (Fog& i : sc->fog) {
            if (i.flag & Fog::FLAG_DENSITY)
                a3dc_read_a3da_key(data, size, &i.density);
            if (i.flag & Fog::FLAG_START)
                a3dc_read_a3da_key(data, size, &i.start);
            if (i.flag & Fog::FLAG_END)
                a3dc_read_a3da_key(data, size, &i.end);
            if (i.flag & Fog::FLAG_COLOR)
                a3dc_read_a3da_rgba(data, size, &i.color);
        }

        for (Light& i : sc->light) {
            if (i.position.flag)
                a3dc_read_a3da_model_transform(data, size, &i.position, _compress_f16);
            if (i.spot_direction.flag)
                a3dc_read_a3da_model_transform(data, size, &i.spot_direction, _compress_f16);
            if (i.flag & Light::FLAG_AMBIENT)
                a3dc_read_a3da_rgba(data, size, &i.ambient);
            if (i.flag & Light::FLAG_DIFFUSE)
                a3dc_read_a3da_rgba(data, size, &i.diffuse);
            if (i.flag & Light::FLAG_SPECULAR)
                a3dc_read_a3da_rgba(data, size, &i.specular);
            if (i.flag & Light::FLAG_TONE_CURVE)
                a3dc_read_a3da_rgba(data, size, &i.tone_curve);

            // XHD
            if (i.flag & Light::FLAG_CONSTANT)
                a3dc_read_a3da_key(data, size, &i.constant);
            if (i.flag & Light::FLAG_INTENSITY)
                a3dc_read_a3da_key(data, size, &i.intensity);
            if (i.flag & Light::FLAG_FAR)
                a3dc_read_a3da_key(data, size, &i.far_);
            if (i.flag & Light::FLAG_LINEAR)
                a3dc_read_a3da_key(data, size, &i.linear);
            if (i.flag & Light::FLAG_QUADRATIC)
                a3dc_read_a3da_key(data, size, &i.quadratic);
            if (i.flag & Light::FLAG_DROP_OFF)
                a3dc_read_a3da_key(data, size, &i.drop_off);
            if (i.flag & Light::FLAG_CONE_ANGLE)
                a3dc_read_a3da_key(data, size, &i.cone_angle);
        }

        for (MObjectHrc& i : sc->m_object_hrc) {
            for (ObjectInstance& j : i.instance)
                a3dc_read_a3da_model_transform(data, size, &j.model_transform, _compress_f16);

            a3dc_read_a3da_model_transform(data, size, &i.model_transform, _compress_f16);

            for (ObjectNode& j : i.node)
                a3dc_read_a3da_model_transform(data, size, &j.model_transform, _compress_f16);
        }

        // X/XHD
        for (MaterialList& i : sc->material_list) {
            if (i.flag & MaterialList::FLAG_EMISSION)
                a3dc_read_a3da_rgba(data, size, &i.emission);
            if (i.flag & MaterialList::FLAG_BLEND_COLOR)
                a3dc_read_a3da_rgba(data, size, &i.blend_color);
            if (i.flag & MaterialList::FLAG_GLOW_INTENSITY)
                a3dc_read_a3da_key(data, size, &i.glow_intensity);
        }

        for (Object& i : sc->object) {
            a3dc_read_a3da_model_transform(data, size, &i.model_transform, _compress_f16);

            for (TextureTransform& j : i.texture_transform) {
                if (j.flag & TextureTransform::FLAG_COVERAGE_U)
                    a3dc_read_a3da_key(data, size, &j.coverage_u);
                if (j.flag & TextureTransform::FLAG_COVERAGE_V)
                    a3dc_read_a3da_key(data, size, &j.coverage_v);
                if (j.flag & TextureTransform::FLAG_REPEAT_U)
                    a3dc_read_a3da_key(data, size, &j.repeat_u);
                if (j.flag & TextureTransform::FLAG_REPEAT_V)
                    a3dc_read_a3da_key(data, size, &j.repeat_v);
                if (j.flag & TextureTransform::FLAG_ROTATE)
                    a3dc_read_a3da_key(data, size, &j.rotate);
                if (j.flag & TextureTransform::FLAG_ROTATE_FRAME)
                    a3dc_read_a3da_key(data, size, &j.rotate_frame);
                if (j.flag & TextureTransform::FLAG_OFFSET_U)
                    a3dc_read_a3da_key(data, size, &j.offset_u);
                if (j.flag & TextureTransform::FLAG_OFFSET_V)
                    a3dc_read_a3da_key(data, size, &j.offset_v);
                if (j.flag & TextureTransform::FLAG_TRANSLATE_FRAME_U)
                    a3dc_read_a3da_key(data, size, &j.translate_frame_u);
                if (j.flag & TextureTransform::FLAG_TRANSLATE_FRAME_V)
                    a3dc_read_a3da_key(data, size, &j.translate_frame_v);
            }
        }

        for (ObjectHrc& i : sc->object_hrc)
            for (ObjectNode& j : i.node)
                a3dc_read_a3da_model_transform(data, size, &j.model_transform, _compress_f16);

        for (Point& i : sc->point)
            a3dc_read_a3da_model_transform(data, size, &i.model_transform, _compress_f16);

        if (sc->post_process.flag) {
            PostProcess* pp = &sc->post_process;

            if (pp->flag & PostProcess::FLAG_LENS_FLARE)
                a3dc_read_a3da_key(data, size, &pp->lens_flare);
            if (pp->flag & PostProcess::FLAG_LENS_SHAFT)
                a3dc_read_a3da_key(data, size, &pp->lens_shaft);
            if (pp->flag & PostProcess::FLAG_LENS_GHOST)
                a3dc_read_a3da_key(data, size, &pp->lens_ghost);
            if (pp->flag & PostProcess::FLAG_RADIUS)
                a3dc_read_a3da_rgba(data, size, &pp->radius);
            if (pp->flag & PostProcess::FLAG_INTENSITY)
                a3dc_read_a3da_rgba(data, size, &pp->intensity);
            if (pp->flag & PostProcess::FLAG_SCENE_FADE)
                a3dc_read_a3da_rgba(data, size, &pp->scene_fade);
        }
    }

    static void a3da_write_data(Scene* sc, void** data, size_t* size) {
        COMPRESS_TYPE& _compress_f16 = sc->_compress_f16;

        memory_stream s;
        s.open();

        bool aft_rgba = sc->format == FORMAT_AFT || sc->format == FORMAT_AFT_X_PACK;
        bool x_pack = sc->format == FORMAT_AFT_X_PACK;

        for (CameraRoot& i : sc->camera_root) {
            a3dc_write_a3da_model_transform_offset(s, i.model_transform);
            a3dc_write_a3da_model_transform_offset(s, i.view_point.model_transform);
            a3dc_write_a3da_model_transform_offset(s, i.interest.model_transform);
        }

        for (Chara& i : sc->chara)
            a3dc_write_a3da_model_transform_offset(s, i.model_transform);

        for (Light& i : sc->light) {
            if (i.position.flag)
                a3dc_write_a3da_model_transform_offset(s, i.position);
            if (i.spot_direction.flag)
                a3dc_write_a3da_model_transform_offset(s, i.spot_direction);
        }

        for (MObjectHrc& i : sc->m_object_hrc) {
            a3dc_write_a3da_model_transform_offset(s, i.model_transform);

            for (ObjectNode& j : i.node)
                a3dc_write_a3da_model_transform_offset(s, j.model_transform);

            for (ObjectInstance& j : i.instance)
                a3dc_write_a3da_model_transform_offset(s, j.model_transform);
        }

        for (Object& i : sc->object)
            a3dc_write_a3da_model_transform_offset(s, i.model_transform);

        for (ObjectHrc& i : sc->object_hrc)
            for (ObjectNode& j : i.node)
                a3dc_write_a3da_model_transform_offset(s, j.model_transform);

        for (Point& i : sc->point)
            a3dc_write_a3da_model_transform_offset(s, i.model_transform);

        // MGF
        for (Ambient& i : sc->ambient) {
            if ((i.flag & Ambient::FLAG_LIGHT_DIFFUSE) && !aft_rgba)
                a3dc_write_a3da_rgba(s, i.light_diffuse);
            if ((i.flag & Ambient::FLAG_RIM_LIGHT_DIFFUSE) && !aft_rgba)
                a3dc_write_a3da_rgba(s, i.rim_light_diffuse);
        }

        if (sc->camera_auxiliary.flag) {
            CameraAuxiliary* ca = &sc->camera_auxiliary;

            if (ca->flag & CameraAuxiliary::FLAG_EXPOSURE)
                a3dc_write_a3da_key(s, ca->exposure);
            if (ca->flag & CameraAuxiliary::FLAG_EXPOSURE_RATE) // F/F2/X
                a3dc_write_a3da_key(s, ca->exposure_rate);
            if (ca->flag & CameraAuxiliary::FLAG_GAMMA)
                a3dc_write_a3da_key(s, ca->gamma);
            if (ca->flag & CameraAuxiliary::FLAG_GAMMA_RATE) // F/F2/X
                a3dc_write_a3da_key(s, ca->gamma_rate);
            if (ca->flag & CameraAuxiliary::FLAG_SATURATE)
                a3dc_write_a3da_key(s, ca->saturate);
            if (ca->flag & CameraAuxiliary::FLAG_AUTO_EXPOSURE)
                a3dc_write_a3da_key(s, ca->auto_exposure);
        }

        for (CameraRoot& i : sc->camera_root) {
            a3dc_write_a3da_model_transform(s, i.model_transform, _compress_f16);

            CameraInterest& intr = i.interest;
            CameraViewPoint& vp = i.view_point;

            a3dc_write_a3da_model_transform(s, vp.model_transform, _compress_f16);
            a3dc_write_a3da_key(s, vp.roll);

            if (vp.has_fov)
                a3dc_write_a3da_key(s, vp.fov);
            else
                a3dc_write_a3da_key(s, vp.focal_length);

            a3dc_write_a3da_model_transform(s, intr.model_transform, _compress_f16);
        }

        for (Chara& i : sc->chara)
            a3dc_write_a3da_model_transform(s, i.model_transform, _compress_f16);

        for (Curve& i : sc->curve)
            a3dc_write_a3da_key(s, i.fcurve);

        if (sc->dof.has_dof && sc->format == FORMAT_AFT_X_PACK) {
            Dof* d = &sc->dof;
            a3da_model_transform_make_raw_data_binary(&d->model_transform);
            a3dc_write_a3da_model_transform(s, d->model_transform, _compress_f16);
        }

        for (Light& i : sc->light) {
            if (i.position.flag)
                a3dc_write_a3da_model_transform(s, i.position, _compress_f16);
            if (i.spot_direction.flag)
                a3dc_write_a3da_model_transform(s, i.spot_direction, _compress_f16);
            if (x_pack) {
                if (i.flag & Light::FLAG_AMBIENT) {
                    a3da_rgba_make_raw_data_binary(&i.ambient);
                    a3dc_write_a3da_rgba(s, i.ambient);
                }
                if (i.flag & Light::FLAG_DIFFUSE) {
                    a3da_rgba_make_raw_data_binary(&i.diffuse);
                    a3dc_write_a3da_rgba(s, i.diffuse);
                }
                if (i.flag & Light::FLAG_SPECULAR) {
                    a3da_rgba_make_raw_data_binary(&i.specular);
                    a3dc_write_a3da_rgba(s, i.specular);
                }
                if (i.flag & Light::FLAG_TONE_CURVE) {
                    a3da_rgba_make_raw_data_binary(&i.tone_curve);
                    a3dc_write_a3da_rgba(s, i.tone_curve);
                }
            }
            else if (!aft_rgba) {
                if (i.flag & Light::FLAG_AMBIENT)
                    a3dc_write_a3da_rgba(s, i.ambient);
                if (i.flag & Light::FLAG_DIFFUSE)
                    a3dc_write_a3da_rgba(s, i.diffuse);
                if (i.flag & Light::FLAG_SPECULAR)
                    a3dc_write_a3da_rgba(s, i.specular);
                if (i.flag & Light::FLAG_TONE_CURVE)
                    a3dc_write_a3da_rgba(s, i.tone_curve);
            }

            // XHD
            if (sc->format == FORMAT_XHD) {
                if (i.flag & Light::FLAG_INTENSITY)
                    a3dc_write_a3da_key(s, i.intensity);
                if (i.flag & Light::FLAG_FAR)
                    a3dc_write_a3da_key(s, i.far_);
                if (i.flag & Light::FLAG_CONSTANT)
                    a3dc_write_a3da_key(s, i.constant);
                if (i.flag & Light::FLAG_LINEAR)
                    a3dc_write_a3da_key(s, i.linear);
                if (i.flag & Light::FLAG_QUADRATIC)
                    a3dc_write_a3da_key(s, i.quadratic);
                if (i.flag & Light::FLAG_DROP_OFF)
                    a3dc_write_a3da_key(s, i.drop_off);
                if (i.flag & Light::FLAG_CONE_ANGLE)
                    a3dc_write_a3da_key(s, i.cone_angle);
            }
        }

        for (Fog& i : sc->fog) {
            if (i.flag & Fog::FLAG_DENSITY)
                a3dc_write_a3da_key(s, i.density);
            if (i.flag & Fog::FLAG_END)
                a3dc_write_a3da_key(s, i.end);
            if (i.flag & Fog::FLAG_START)
                a3dc_write_a3da_key(s, i.start);
            if (x_pack) {
                if (i.flag & Fog::FLAG_COLOR) {
                    a3da_rgba_make_raw_data_binary(&i.color);
                    a3dc_write_a3da_rgba(s, i.color);
                }
            }
            else if (!aft_rgba) {
                if (i.flag & Fog::FLAG_COLOR)
                    a3dc_write_a3da_rgba(s, i.color);
            }
        }

        for (MObjectHrc& i : sc->m_object_hrc) {
            a3dc_write_a3da_model_transform(s, i.model_transform, _compress_f16);

            for (ObjectNode& j : i.node)
                a3dc_write_a3da_model_transform(s, j.model_transform, _compress_f16);

            for (ObjectInstance& j : i.instance)
                a3dc_write_a3da_model_transform(s, j.model_transform, _compress_f16);
        }

        // X/XHD
        if (sc->format == FORMAT_X || sc->format == FORMAT_XHD)
            for (MaterialList& i : sc->material_list) {
                if (i.flag & MaterialList::FLAG_GLOW_INTENSITY)
                    a3dc_write_a3da_key(s, i.glow_intensity);
                if (i.flag & MaterialList::FLAG_BLEND_COLOR)
                    a3dc_write_a3da_rgba(s, i.blend_color);
                if (i.flag & MaterialList::FLAG_EMISSION)
                    a3dc_write_a3da_rgba(s, i.emission);
            }
        else if (sc->format == FORMAT_AFT_X_PACK)
            for (MaterialList& i : sc->material_list) {
                if (i.flag & MaterialList::FLAG_BLEND_COLOR) {
                    a3da_rgba_make_raw_data_binary(&i.blend_color);
                    a3dc_write_a3da_rgba(s, i.blend_color);
                }

                if (i.flag & MaterialList::FLAG_EMISSION) {
                    a3da_rgba_make_raw_data_binary(&i.emission);
                    a3dc_write_a3da_rgba(s, i.emission);
                }
            }

        for (Object& i : sc->object) {
            a3dc_write_a3da_model_transform(s, i.model_transform, _compress_f16);

            for (TextureTransform& j : i.texture_transform) {
                if (j.flag & TextureTransform::FLAG_COVERAGE_U)
                    a3dc_write_a3da_key(s, j.coverage_u);
                if (j.flag & TextureTransform::FLAG_COVERAGE_V)
                    a3dc_write_a3da_key(s, j.coverage_v);
                if (j.flag & TextureTransform::FLAG_REPEAT_U)
                    a3dc_write_a3da_key(s, j.repeat_u);
                if (j.flag & TextureTransform::FLAG_REPEAT_V)
                    a3dc_write_a3da_key(s, j.repeat_v);
                if (j.flag & TextureTransform::FLAG_OFFSET_U)
                    a3dc_write_a3da_key(s, j.offset_u);
                if (j.flag & TextureTransform::FLAG_OFFSET_V)
                    a3dc_write_a3da_key(s, j.offset_v);
                if (j.flag & TextureTransform::FLAG_ROTATE)
                    a3dc_write_a3da_key(s, j.rotate);
                if (j.flag & TextureTransform::FLAG_ROTATE_FRAME)
                    a3dc_write_a3da_key(s, j.rotate_frame);
                if (j.flag & TextureTransform::FLAG_TRANSLATE_FRAME_U)
                    a3dc_write_a3da_key(s, j.translate_frame_u);
                if (j.flag & TextureTransform::FLAG_TRANSLATE_FRAME_V)
                    a3dc_write_a3da_key(s, j.translate_frame_v);
            }
        }

        for (ObjectHrc& i : sc->object_hrc)
            for (ObjectNode& j : i.node)
                a3dc_write_a3da_model_transform(s, j.model_transform, _compress_f16);

        for (Point& i : sc->point)
            a3dc_write_a3da_model_transform(s, i.model_transform, _compress_f16);

        if (sc->post_process.flag) {
            PostProcess* pp = &sc->post_process;

            if (pp->flag & PostProcess::FLAG_LENS_FLARE)
                a3dc_write_a3da_key(s, pp->lens_flare);
            if (pp->flag & PostProcess::FLAG_LENS_SHAFT)
                a3dc_write_a3da_key(s, pp->lens_shaft);
            if (pp->flag & PostProcess::FLAG_LENS_GHOST)
                a3dc_write_a3da_key(s, pp->lens_ghost);
            if (x_pack) {
                if (pp->flag & PostProcess::FLAG_RADIUS) {
                    a3da_rgba_make_raw_data_binary(&pp->radius);
                    a3dc_write_a3da_rgba(s, pp->radius);
                }
                if (pp->flag & PostProcess::FLAG_INTENSITY) {
                    a3da_rgba_make_raw_data_binary(&pp->intensity);
                    a3dc_write_a3da_rgba(s, pp->intensity);
                }
                if (pp->flag & PostProcess::FLAG_SCENE_FADE) {
                    a3da_rgba_make_raw_data_binary(&pp->scene_fade);
                    a3dc_write_a3da_rgba(s, pp->scene_fade);
                }
            }
            else if (!aft_rgba) {
                if (pp->flag & PostProcess::FLAG_RADIUS)
                    a3dc_write_a3da_rgba(s, pp->radius);
                if (pp->flag & PostProcess::FLAG_INTENSITY)
                    a3dc_write_a3da_rgba(s, pp->intensity);
                if (pp->flag & PostProcess::FLAG_SCENE_FADE)
                    a3dc_write_a3da_rgba(s, pp->scene_fade);
            }
        }

        for (CameraRoot& i : sc->camera_root) {
            CameraInterest& intr = i.interest;
            CameraViewPoint& vp = i.view_point;
            a3dc_write_a3da_model_transform_offset_data(s, i.model_transform);
            a3dc_write_a3da_model_transform_offset_data(s, vp.model_transform);
            a3dc_write_a3da_model_transform_offset_data(s, intr.model_transform);
        }

        for (Chara& i : sc->chara)
            a3dc_write_a3da_model_transform_offset_data(s, i.model_transform);

        for (Light& i : sc->light) {
            if (i.position.flag)
                a3dc_write_a3da_model_transform_offset_data(s, i.position);
            if (i.spot_direction.flag)
                a3dc_write_a3da_model_transform_offset_data(s, i.spot_direction);
        }

        for (MObjectHrc& i : sc->m_object_hrc) {
            a3dc_write_a3da_model_transform_offset_data(s, i.model_transform);

            for (ObjectNode& j : i.node)
                a3dc_write_a3da_model_transform_offset_data(s, j.model_transform);

            for (ObjectInstance& j : i.instance)
                a3dc_write_a3da_model_transform_offset_data(s, j.model_transform);
        }

        for (Object& i : sc->object)
            a3dc_write_a3da_model_transform_offset_data(s, i.model_transform);

        for (ObjectHrc& i : sc->object_hrc)
            for (ObjectNode& j : i.node)
                a3dc_write_a3da_model_transform_offset_data(s, j.model_transform);

        for (Point& i : sc->point)
            a3dc_write_a3da_model_transform_offset_data(s, i.model_transform);

        s.align_write(0x10);
        s.copy(data, size);
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

    static bool key_val_read(key_val* kv,
        const char* key, Fcurve& value) {
        if (!kv->open_scope(key))
            return false;
        else if (kv->read("bin_offset", value.bin_offset)) {
            value.flag = Fcurve::BIN_OFFSET;
            kv->close_scope();
            return true;
        }

        int32_t type;
        if (!kv->read("type", type)) {
            kv->close_scope();
            return false;
        }
        value.type = (FC_TYPE)type;

        if (value.type == FC_TYPE_STATIC_0) {
            kv->close_scope();
            return true;
        }
        else if (value.type == FC_TYPE_STATIC_DATA) {
            kv->read("value", value.value);
            kv->close_scope();
            return true;
        }

        int32_t ep_type_post;
        if (kv->read("ep_type_post", ep_type_post))
            value.ep_type_post = (EP_TYPE)ep_type_post;

        int32_t ep_type_pre;
        if (kv->read("ep_type_pre", ep_type_pre))
            value.ep_type_pre = (EP_TYPE)ep_type_pre;

        kv->read("max", value.max_frame);

        if (key_val_read_raw_data(kv, value)) {
            kv->close_scope();
            return true;
        }

        int32_t length;
        if (!kv->read("key", "length", length)) {
            kv->close_scope();
            return false;
        }

        int32_t act_length = length;
        value.keys.resize(length);
        kft3* keys = value.keys.data();

        for (int32_t i = 0, j = 0; i < length; i++) {
            if (!kv->open_scope_fmt(i))
                continue;

            const char* data;
            int32_t type;
            if (!kv->read("data", data) || !kv->read("type", type)) {
                kv->close_scope();
                continue;
            }

            switch (type) {
            case KEY_FRAME_TYPE_0: {
                float_t f;
                if (sscanf_s(data, "%g", &f) == 1)
                    keys[j++] = { f, 0.0f, 0.0f, 0.0f };
            } break;
            case KEY_FRAME_TYPE_1: {
                float_t f;
                float_t v;
                if (sscanf_s(data, "(%g,%g)", &f, &v) == 2)
                    keys[j++] = { f, v, 0.0f, 0.0f };
            } break;
            case KEY_FRAME_TYPE_2: {
                float_t f;
                float_t v;
                float_t t;
                if (sscanf_s(data, "(%g,%g,%g)", &f, &v, &t) == 3)
                    keys[j++] = { f, v, t, t };
            } break;
            case KEY_FRAME_TYPE_3: {
                float_t f;
                float_t v;
                float_t t1;
                float_t t2;
                if (sscanf_s(data, "(%g,%g,%g,%g)", &f, &v, &t1, &t2) == 4)
                    keys[j++] = { f, v, t1, t2 };
            } break;
            default:
                keys[j++] = {};
                break;
            }

            kv->close_scope();
        }
        value.keys.resize(act_length);

        kv->close_scope();
        kv->close_scope();
        return true;
    }

    static void key_val_out_write(key_val_out* kv, stream& s,
        const char* key, Fcurve& value, bool write_true) {
        kv->open_scope(key);

        if (write_true)
            kv->write(s, "true");

        if (value.flag & Fcurve::BIN_OFFSET) {
            kv->write(s, "bin_offset", value.bin_offset);
            value.flag &= ~Fcurve::BIN_OFFSET;
            value.bin_offset = 0;
            kv->close_scope();
            return;
        }

        if (value.type == FC_TYPE_STATIC_0) {
            kv->write(s, "type", 0);
            kv->close_scope();
            return;
        }
        else if (value.type == FC_TYPE_STATIC_DATA) {
            kv->write(s, "type", 1);
            kv->write(s, "value", value.value);
            kv->close_scope();
            return;
        }

        if (value.ep_type_post != EP_TYPE_CONSTANT)
            kv->write(s, "ep_type_post", value.ep_type_post);
        if (value.ep_type_pre != EP_TYPE_CONSTANT)
            kv->write(s, "ep_type_pre", value.ep_type_pre);

        if (value.raw_data) {
            key_val_out_write_raw_data(kv, s, value);
            kv->close_scope();
            return;
        }

        kv->open_scope("key");

        int32_t length = (int32_t)value.keys.size();

        std::vector<int32_t> sort_index;
        key_val_out::get_lexicographic_order(sort_index, length);
        int32_t* sort_index_data = sort_index.data();
        for (int32_t i = 0; i < length; i++) {
            kv->open_scope_fmt(sort_index_data[i]);

            kft3 k = value.keys[sort_index_data[i]];
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
                    k.frame, k.value, k.l_slope);
                break;
            case KEY_FRAME_TYPE_3:
                sprintf_s(data_buf, sizeof(data_buf), "(%g,%g,%g,%g)",
                    k.frame, k.value, k.l_slope, k.r_slope);
                break;
            }

            kv->write(s, "data", data_buf);
            kv->write(s, "type", kt);

            kv->close_scope();
        }

        kv->write(s, "length", length);

        kv->close_scope();

        kv->write(s, "max", value.max_frame);
        kv->write(s, "type", value.type);

        kv->close_scope();
    }

    static bool key_val_read_raw_data(key_val* kv,
        Fcurve& value) {
        int32_t key_type;
        if (!kv->read("raw_data_key_type", key_type))
            return false;

        const char* value_type;
        kv->read("raw_data.value_type", value_type);
        if (str_utils_compare(value_type, "float"))
            return false;

        int32_t value_list_size;
        kv->read("raw_data.value_list_size", value_list_size);

        int32_t value_list_offset;
        if (kv->read("raw_data.value_list_offset", value_list_offset)) {
            if (key_type != 3)
                return false;

            value.raw_data = true;
            value.raw_data_binary = true;
            value.raw_data_value_list_size = value_list_size;
            value.raw_data_value_list_offset = value_list_offset;
            return true;
        }

        const char* value_list;
        if (!kv->read("raw_data.value_list", value_list))
            return false;

        const char* s = value_list;
        size_t c = 1;
        while (s = strchr(s, ',')) {
            s++;
            c++;
        }

        if (c != value_list_size)
            return false;

        char b[0x200];
        float_t* fs = force_malloc<float_t>(c);
        s = value_list;
        for (size_t i = 0; i < c; i++) {
            const char* t = s;
            s = strchr(s, ',');

            size_t length = s ? s++ - t : utf8_length(t);
            memcpy_s(b, sizeof(b) - 1, t, length);
            b[length] = 0;
            fs[i] = (float_t)atof(b);
        }

        switch (key_type) {
        case 0: {
            value.keys.resize(c);
            kft3* keys = value.keys.data();
            for (size_t i = 0; i < c; i++, fs++)
                keys[i] = { fs[0], 0.0f, 0.0f, 0.0f };
            fs -= c;
        } break;
        case 1: {
            c /= 2;
            value.keys.resize(c);
            kft3* keys = value.keys.data();
            for (size_t i = 0; i < c; i++, fs += 2)
                keys[i] = { fs[0], fs[1], 0.0f, 0.0f };
            fs -= c * 2;
        } break;
        case 2: {
            c /= 3;
            value.keys.resize(c);
            kft3* keys = value.keys.data();
            for (size_t i = 0; i < c; i++, fs += 3)
                keys[i] = { fs[0], fs[1], fs[2], fs[2] };
            fs -= c * 3;
        } break;
        case 3: {
            c /= 4;
            value.keys.resize(c);
            kft3* keys = value.keys.data();
            for (size_t i = 0; i < c; i++, fs += 4)
                keys[i] = { fs[0], fs[1], fs[2], fs[3] };
            fs -= c * 4;
        } break;
        default:
            free_def(fs);
            return false;
        }
        free_def(fs);

        value.raw_data = true;
        return true;
    }

    static void key_val_out_write_raw_data(key_val_out* kv, stream& s,
        Fcurve& value) {
        kv->write(s, "max", value.max_frame);

        int32_t length = (int32_t)value.keys.size();
        if (value.raw_data_binary) {
            kv->write(s, "raw_data.value_list_offset",
                value.raw_data_value_list_offset);
            kv->write(s, "raw_data.value_list_size",
                value.raw_data_value_list_size);
            kv->write(s, "raw_data.value_type", "float");
            kv->write(s, "raw_data_key_type", 3);
            kv->write(s, "type", value.type);
            return;
        }

        kft3* keys = value.keys.data();

        kf_type key_type = KEY_FRAME_TYPE_0;
        for (int32_t i = 0; i < length; i++) {
            kft3 k = keys[i];
            kf_type kt = KEY_FRAME_TYPE_3;
            kft_check(&k, kt, &k, &kt);
            if (key_type < kt)
                key_type = kt;
            if (key_type == KEY_FRAME_TYPE_3)
                break;
        }

        s.write_string(*kv->curr_scope);
        s.write_char('.');
        s.write_utf8_string("raw_data.value_list");
        s.write_char('=');

        char data_buf[0x200];
        switch (key_type) {
        case KEY_FRAME_TYPE_0:
            for (int32_t i = 0; i < length; i++) {
                kft3 k = keys[i];
                sprintf_s(data_buf, sizeof(data_buf), "%g",
                    k.frame);
                s.write_utf8_string(data_buf);
                if (i + 1 < length)
                    s.write_char(',');
            }
            break;
        case KEY_FRAME_TYPE_1:
            for (int32_t i = 0; i < length; i++) {
                kft3 k = keys[i];
                sprintf_s(data_buf, sizeof(data_buf), "%g,%g",
                    k.frame, k.value);
                s.write_utf8_string(data_buf);
                if (i + 1 < length)
                    s.write_char(',');
            }
            break;
        case KEY_FRAME_TYPE_2:
            for (int32_t i = 0; i < length; i++) {
                kft3 k = keys[i];
                sprintf_s(data_buf, sizeof(data_buf), "%g,%g,%g",
                    k.frame, k.value, k.l_slope);
                s.write_utf8_string(data_buf);
                if (i + 1 < length)
                    s.write_char(',');
            }
            break;
        case KEY_FRAME_TYPE_3:
            for (int32_t i = 0; i < length; i++) {
                kft3 k = keys[i];
                sprintf_s(data_buf, sizeof(data_buf), "%g,%g,%g,%g",
                    k.frame, k.value, k.l_slope, k.r_slope);
                s.write_utf8_string(data_buf);
                if (i + 1 < length)
                    s.write_char(',');
            }
            break;
        }
        s.write_char('\n');

        kv->write(s, "raw_data.value_list_size",
            (int32_t)(length * ((size_t)key_type + 1)));
        kv->write(s, "raw_data.value_type", "float");
        kv->write(s, "raw_data_key_type", key_type);
        kv->write(s, "type", value.type);
    }

    static bool key_val_read(key_val* kv,
        const char* key, ModelTransform& value) {
        if (!kv->open_scope(key))
            return false;
        else if (kv->read("model_transform.bin_offset", value.bin_offset)) {
            value.flag = ModelTransform::BIN_OFFSET;
            kv->close_scope();
            return true;
        }

        key_val_read(kv, "rot", value.rotation);
        key_val_read(kv, "scale", value.scale);
        key_val_read(kv, "trans", value.translation);
        key_val_read(kv, "visibility", value.visibility);

        kv->close_scope();
        return true;
    }

    static void key_val_out_write(key_val_out* kv, stream& s,
        const char* key, ModelTransform& value, int32_t write_mask) {
        kv->open_scope(key);

        if (value.flag & ModelTransform::BIN_OFFSET) {
            if (write_mask & 0x10) {
                kv->write(s, "model_transform.bin_offset", value.bin_offset);
            }
            if (write_mask & 0x01)
                if (value.flag & ModelTransform::BIN_OFFSET) {
                    value.flag &= ~ModelTransform::BIN_OFFSET;
                    value.bin_offset = 0;
                }
        }
        else {
            if (write_mask & 0x08)
                key_val_out_write(kv, s, "rot", value.rotation);
            if (write_mask & 0x04)
                key_val_out_write(kv, s, "scale", value.scale);
            if (write_mask & 0x02)
                key_val_out_write(kv, s, "trans", value.translation);
            if (write_mask & 0x01)
                key_val_out_write(kv, s, "visibility", value.visibility);
        }

        kv->close_scope();
    }

    static bool key_val_read(key_val* kv,
        const char* key, FcurveColor4f& value) {
        if (!kv->open_scope(key))
            return false;

        value.has_r = key_val_read(kv, "r", value.r);
        value.has_g = key_val_read(kv, "g", value.g);
        value.has_b = key_val_read(kv, "b", value.b);
        value.has_a = key_val_read(kv, "a", value.a);

        kv->close_scope();
        return true;
    }

    static void key_val_out_write(key_val_out* kv, stream& s,
        const char* key, FcurveColor4f& value) {
        if (!value.has_r && !value.has_g && !value.has_b && !value.has_a)
            return;

        kv->open_scope(key);

        kv->write(s, "true");

        if (value.has_a)
            key_val_out_write(kv, s, "a", value.a);
        if (value.has_b)
            key_val_out_write(kv, s, "b", value.b);
        if (value.has_g)
            key_val_out_write(kv, s, "g", value.g);
        if (value.has_r)
            key_val_out_write(kv, s, "r", value.r);

        kv->close_scope();
    }

    static bool key_val_read(key_val* kv,
        const char* key, Fcurve3f& value) {
        if (!kv->open_scope(key))
            return false;

        key_val_read(kv, "x", value.x);
        key_val_read(kv, "y", value.y);
        key_val_read(kv, "z", value.z);

        kv->close_scope();
        return true;
    }

    static void key_val_out_write(key_val_out* kv, stream& s,
        const char* key, Fcurve3f& value) {
        kv->open_scope(key);

        key_val_out_write(kv, s, "x", value.x);
        key_val_out_write(kv, s, "y", value.y);
        key_val_out_write(kv, s, "z", value.z);

        kv->close_scope();
    }

    inline static void a3dc_read_a3da_key(void* data, size_t size, Fcurve* value) {
        a3dc_read_a3da_key_f16(data, size, value, COMPRESS_F32F32F32F32);
    }

    inline static void a3dc_write_a3da_key(stream& s, Fcurve& value) {
        a3dc_write_a3da_key_f16(s, value, COMPRESS_F32F32F32F32);
    }

    static void a3dc_read_a3da_key_f16(void* data, size_t size, Fcurve* value, COMPRESS_TYPE f16) {
        if (value->raw_data) {
            if (!value->raw_data_binary)
                return;

            size_t _d = (size_t)data + value->raw_data_value_list_offset;
            value->raw_data_value_list_offset = 0;

            int32_t len = value->raw_data_value_list_size / 4;
            value->keys.resize(len);
            kft3* keys = value->keys.data();
            for (int32_t i = 0; i < len; i++) {
                kft3& k = keys[i];
                k.frame = *(float_t*)_d;
                k.value = *(float_t*)(_d + 4);
                k.l_slope = *(float_t*)(_d + 8);
                k.r_slope = *(float_t*)(_d + 12);
                _d += 16;
            }
            return;
        }
        else if (!(value->flag & Fcurve::BIN_OFFSET))
            return;

        a3dc_key_header* head = (a3dc_key_header*)((size_t)data + value->bin_offset);
        value->flag &= ~Fcurve::BIN_OFFSET;
        value->bin_offset = 0;

        size_t d = (size_t)head + sizeof(a3dc_key_header);

        if (head->type == FC_TYPE_STATIC_0) {
            value->type = head->type;
            return;
        }
        else if (head->type == FC_TYPE_STATIC_DATA) {
            value->type = head->type;
            value->value = head->value;
            return;
        }

        value->type = head->type;
        value->ep_type_pre = head->ep_type_pre;
        value->ep_type_post = head->ep_type_post;
        value->max_frame = head->max_frame;
        uint32_t len = head->length;

        value->keys.resize(len);
        kft3* keys = value->keys.data();
        switch (f16) {
        case COMPRESS_F32F32F32F32:
        default:
            for (uint32_t i = 0; i < len; i++) {
                kft3& k = keys[i];
                k.frame = *(float_t*)d;
                k.value = *(float_t*)(d + 4);
                k.l_slope = *(float_t*)(d + 8);
                k.r_slope = *(float_t*)(d + 12);
                d += 16;
            }
            break;
        case COMPRESS_I16F16F32F32:
            for (uint32_t i = 0; i < len; i++) {
                kft3& k = keys[i];
                k.frame = (float_t)*(int16_t*)d;
                k.value = half_to_float(*(half_t*)(d + 2));
                k.l_slope = *(float_t*)(d + 4);
                k.r_slope = *(float_t*)(d + 8);
                d += 12;
            }
            break;
        case COMPRESS_I16F16F16F16:
            for (uint32_t i = 0; i < len; i++) {
                kft3& k = keys[i];
                k.frame = (float_t)*(int16_t*)d;
                k.value = half_to_float(*(half_t*)(d + 2));
                k.l_slope = half_to_float(*(half_t*)(d + 4));
                k.r_slope = half_to_float(*(half_t*)(d + 6));
                d += 8;
            }
            break;
        }
    }

    static void a3dc_write_a3da_key_f16(stream& s, Fcurve& value, COMPRESS_TYPE f16) {
        if (value.raw_data) {
            if (value.type == FC_TYPE_STATIC_0 || value.type == FC_TYPE_STATIC_DATA) {
                value.raw_data = false;
                value.raw_data_binary = false;
                return;
            }
            else if (value.keys.size() < 1) {
                value.raw_data = false;
                value.raw_data_binary = false;
                value.type = FC_TYPE_STATIC_0;
                value.keys.resize(0);
                return;
            }
            else if (value.keys.size() == 1) {
                value.raw_data = false;
                value.raw_data_binary = false;
                value.type = FC_TYPE_STATIC_DATA;
                value.value = value.keys[0].value;
                value.keys.resize(0);
                return;
            }
        }

        if (value.raw_data) {
            if (!value.raw_data_binary)
                return;

            value.raw_data_value_list_offset = (int32_t)s.get_position();

            int32_t len = (int32_t)value.keys.size();
            kft3* keys = value.keys.data();
            value.raw_data_value_list_size = len * 4;
            for (int32_t i = 0; i < len; i++) {
                kft3* k = &keys[i];
                s.write_float_t(k->frame);
                s.write_float_t(k->value);
                s.write_float_t(k->l_slope);
                s.write_float_t(k->r_slope);
            }
            return;
        }

        value.bin_offset = (int32_t)s.get_position();
        value.flag |= Fcurve::BIN_OFFSET;

        if (value.type == FC_TYPE_STATIC_0) {
            s.write_int32_t(FC_TYPE_STATIC_0);
            s.write_float_t(0.0f);
            return;
        }
        else if (value.type == FC_TYPE_STATIC_DATA) {
            s.write_int32_t(FC_TYPE_STATIC_DATA);
            s.write_float_t(value.value);
            return;
        }

        uint32_t len = (uint32_t)value.keys.size();
        kft3* keys = value.keys.data();

        a3dc_key_header head = {};
        head.type = value.type;
        head.ep_type_pre = value.ep_type_pre;
        head.ep_type_post = value.ep_type_post;
        head.max_frame = value.max_frame;
        head.length = len;
        s.write_data(head);

        switch (f16) {
        case COMPRESS_F32F32F32F32:
        default:
            for (uint32_t i = 0; i < len; i++) {
                kft3* k = &keys[i];
                s.write_float_t(k->frame);
                s.write_float_t(k->value);
                s.write_float_t(k->l_slope);
                s.write_float_t(k->r_slope);
            }
            break;
        case COMPRESS_I16F16F32F32:
            for (uint32_t i = 0; i < len; i++) {
                kft3* k = &keys[i];
                s.write_int16_t((int16_t)prj::roundf(k->frame));
                s.write_half_t(float_to_half(k->value));
                s.write_float_t(k->l_slope);
                s.write_float_t(k->r_slope);
            }
            break;
        case COMPRESS_I16F16F16F16:
            for (uint32_t i = 0; i < len; i++) {
                kft3* k = &keys[i];
                s.write_int16_t((int16_t)prj::roundf(k->frame));
                s.write_half_t(float_to_half(k->value));
                s.write_half_t(float_to_half(k->l_slope));
                s.write_half_t(float_to_half(k->r_slope));
            }
            break;
        }
    }

    static void a3dc_read_a3da_model_transform(void* data, size_t size,
        ModelTransform* value, COMPRESS_TYPE f16) {
        if (!(value->flag & ModelTransform::BIN_OFFSET))
            return;

        a3da_model_transform_header* head = (a3da_model_transform_header*)((size_t)data + value->bin_offset);
        value->flag &= ~ModelTransform::BIN_OFFSET;
        value->bin_offset = 0;

        value->scale.x.bin_offset = head->scale.x;
        value->scale.x.flag |= Fcurve::BIN_OFFSET;
        value->scale.y.bin_offset = head->scale.y;
        value->scale.y.flag |= Fcurve::BIN_OFFSET;
        value->scale.z.bin_offset = head->scale.z;
        value->scale.z.flag |= Fcurve::BIN_OFFSET;
        value->rotation.x.bin_offset = head->rotation.x;
        value->rotation.x.flag |= Fcurve::BIN_OFFSET;
        value->rotation.y.bin_offset = head->rotation.y;
        value->rotation.y.flag |= Fcurve::BIN_OFFSET;
        value->rotation.z.bin_offset = head->rotation.z;
        value->rotation.z.flag |= Fcurve::BIN_OFFSET;
        value->translation.x.bin_offset = head->translation.x;
        value->translation.x.flag |= Fcurve::BIN_OFFSET;
        value->translation.y.bin_offset = head->translation.y;
        value->translation.y.flag |= Fcurve::BIN_OFFSET;
        value->translation.z.bin_offset = head->translation.z;
        value->translation.z.flag |= Fcurve::BIN_OFFSET;
        value->visibility.bin_offset = head->visibility;
        value->visibility.flag |= Fcurve::BIN_OFFSET;

        a3dc_read_a3da_vec3(data, size, &value->scale);
        a3dc_read_a3da_vec3_f16(data, size, &value->rotation, f16);
        a3dc_read_a3da_vec3(data, size, &value->translation);
        a3dc_read_a3da_key(data, size, &value->visibility);
    }

    static void a3dc_write_a3da_model_transform(stream& s,
        ModelTransform& value, COMPRESS_TYPE f16) {
        a3dc_write_a3da_vec3(s, value.scale);
        a3dc_write_a3da_vec3_f16(s, value.rotation, f16);
        a3dc_write_a3da_vec3(s, value.translation);
        a3dc_write_a3da_key(s, value.visibility);
    }

    static void a3dc_write_a3da_model_transform_offset(stream& s,
        ModelTransform& value) {
        value.bin_offset = (int32_t)s.get_position();
        value.flag |= ModelTransform::BIN_OFFSET;
        s.write(0x30);
    }

    static void a3dc_write_a3da_model_transform_offset_data(stream& s,
        ModelTransform& value) {
        s.position_push(value.bin_offset, SEEK_SET);
        s.write_uint32_t(value.scale.x.bin_offset);
        value.scale.x.flag &= ~Fcurve::BIN_OFFSET;
        value.scale.x.bin_offset = 0;
        s.write_uint32_t(value.scale.y.bin_offset);
        value.scale.y.flag &= ~Fcurve::BIN_OFFSET;
        value.scale.y.bin_offset = 0;
        s.write_uint32_t(value.scale.z.bin_offset);
        value.scale.z.flag &= ~Fcurve::BIN_OFFSET;
        value.scale.z.bin_offset = 0;
        s.write_uint32_t(value.rotation.x.bin_offset);
        value.rotation.x.flag &= ~Fcurve::BIN_OFFSET;
        value.rotation.x.bin_offset = 0;
        s.write_uint32_t(value.rotation.y.bin_offset);
        value.rotation.y.flag &= ~Fcurve::BIN_OFFSET;
        value.rotation.y.bin_offset = 0;
        s.write_uint32_t(value.rotation.z.bin_offset);
        value.rotation.z.flag &= ~Fcurve::BIN_OFFSET;
        value.rotation.z.bin_offset = 0;
        s.write_uint32_t(value.translation.x.bin_offset);
        value.translation.x.flag &= ~Fcurve::BIN_OFFSET;
        value.translation.x.bin_offset = 0;
        s.write_uint32_t(value.translation.y.bin_offset);
        value.translation.y.flag &= ~Fcurve::BIN_OFFSET;
        value.translation.y.bin_offset = 0;
        s.write_uint32_t(value.translation.z.bin_offset);
        value.translation.z.flag &= ~Fcurve::BIN_OFFSET;
        value.translation.z.bin_offset = 0;
        s.write_uint32_t(value.visibility.bin_offset);
        value.visibility.flag &= ~Fcurve::BIN_OFFSET;
        value.visibility.bin_offset = 0;
        s.position_pop();
    }

    static void a3dc_read_a3da_rgba(void* data, size_t size, FcurveColor4f* value) {
        if (value->has_r)
            a3dc_read_a3da_key(data, size, &value->r);
        if (value->has_g)
            a3dc_read_a3da_key(data, size, &value->g);
        if (value->has_b)
            a3dc_read_a3da_key(data, size, &value->b);
        if (value->has_a)
            a3dc_read_a3da_key(data, size, &value->a);
    }

    static void a3dc_write_a3da_rgba(stream& s, FcurveColor4f& value) {
        if (value.has_r)
            a3dc_write_a3da_key(s, value.r);
        if (value.has_g)
            a3dc_write_a3da_key(s, value.g);
        if (value.has_b)
            a3dc_write_a3da_key(s, value.b);
        if (value.has_a)
            a3dc_write_a3da_key(s, value.a);
    }

    static void a3dc_read_a3da_vec3(void* data, size_t size, Fcurve3f* value) {
        a3dc_read_a3da_key(data, size, &value->x);
        a3dc_read_a3da_key(data, size, &value->y);
        a3dc_read_a3da_key(data, size, &value->z);
    }

    static void a3dc_write_a3da_vec3(stream& s, Fcurve3f& value) {
        a3dc_write_a3da_key(s, value.x);
        a3dc_write_a3da_key(s, value.y);
        a3dc_write_a3da_key(s, value.z);
    }

    static void a3dc_read_a3da_vec3_f16(void* data, size_t size, Fcurve3f* value, COMPRESS_TYPE f16) {
        a3dc_read_a3da_key_f16(data, size, &value->x, f16);
        a3dc_read_a3da_key_f16(data, size, &value->y, f16);
        a3dc_read_a3da_key_f16(data, size, &value->z, f16);
    }

    static void a3dc_write_a3da_vec3_f16(stream& s, Fcurve3f& value, COMPRESS_TYPE f16) {
        a3dc_write_a3da_key_f16(s, value.x, f16);
        a3dc_write_a3da_key_f16(s, value.y, f16);
        a3dc_write_a3da_key_f16(s, value.z, f16);
    }

    inline static void a3da_key_make_raw_data_binary(Fcurve* value) {
        value->raw_data = true;
        value->raw_data_binary = true;
    }

    inline static void a3da_model_transform_make_raw_data_binary(ModelTransform* value) {
        a3da_vec3_make_raw_data_binary(&value->rotation);
        a3da_vec3_make_raw_data_binary(&value->scale);
        a3da_vec3_make_raw_data_binary(&value->translation);
        a3da_key_make_raw_data_binary(&value->visibility);
    }

    inline static void a3da_rgba_make_raw_data_binary(FcurveColor4f* value) {
        if (value->has_r)
            a3da_key_make_raw_data_binary(&value->r);
        if (value->has_g)
            a3da_key_make_raw_data_binary(&value->g);
        if (value->has_b)
            a3da_key_make_raw_data_binary(&value->b);
        if (value->has_a)
            a3da_key_make_raw_data_binary(&value->a);
    }

    inline static void a3da_vec3_make_raw_data_binary(Fcurve3f* value) {
        a3da_key_make_raw_data_binary(&value->x);
        a3da_key_make_raw_data_binary(&value->y);
        a3da_key_make_raw_data_binary(&value->z);
    }
}
