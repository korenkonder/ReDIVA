/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "auth_3d.hpp"
#include "../KKdLib/io/file_stream.hpp"
#include "../KKdLib/io/json.hpp"
#include "../KKdLib/io/path.hpp"
#include "../KKdLib/prj/algorithm.hpp"
#include "../KKdLib/farc.hpp"
#include "../KKdLib/hash.hpp"
#include "../KKdLib/interpolation.hpp"
#include "../KKdLib/msgpack.hpp"
#include "../KKdLib/str_utils.hpp"
#include "rob/rob.hpp"
#include "rob/motion.hpp"
#include "app_system_detail.hpp"
#include "clear_color.hpp"
#include "data.hpp"
#include "debug_print.hpp"
#include "effect.hpp"
#include "object.hpp"
#include "pv_db.hpp"
#include "task.hpp"
#include "sound.hpp"
#include "stage.hpp"

namespace auth_3d_detail {
    class TaskAuth3d : public app::Task {
    public:
        TaskAuth3d();
        virtual ~TaskAuth3d() override;

        virtual bool init() override;
        virtual bool ctrl() override;
        virtual void disp() override;
    };

    class FrameRateTimeStop : public FrameRateControl {
    public:
        FrameRateTimeStop();
        virtual ~FrameRateTimeStop() override;

        virtual float_t get_delta_frame() override;
    };

    static void event_log(const char* fmt, ...);
    static const char* get_ID_EVENT_TYPE(EVENT_TYPE type);

    template <class T>
    size_t get_index_by_name(const T& in_c, const std::string& in_name);

    template <class T, class U, typename... Args>
    void list_load(T& in_c, const SceneFile& in_file, const U& in_a, Args... args);

    template <class T, class U, typename... Args>
    void list_store(const T& in_c, const SceneFile& in_file,  U& in_a, Args... args);
}

static void a3da_msgpack_read(const char* path, const char* file, a3d::Scene* auth_file);

static void auth_3d_set_material_list(auth_3d_detail::Scene& in_scene, render_context* rctx);

namespace auth_3d_detail {
    Auth3dManager* g_manager;
    TaskAuth3d* task_auth_3d;
}

static int16_t magic_number;
static const mat4 bad_matrix;
static auth_3d_detail::FrameRateTimeStop frame_rate_time_stop;

extern render_context* rctx_ptr;

namespace a3d {
    EventListener::EventListener() {

    }

    EventListener::~EventListener() {

    }

    EventAdapter::EventAdapter() {

    }

    EventAdapter::~EventAdapter() {

    }

    void EventAdapter::loadFinished(auth_3d_detail::Handle& in_handle) {

    }

    void EventAdapter::playStart(auth_3d_detail::Handle& in_handle) {

    }

    void EventAdapter::playFinished(auth_3d_detail::Handle& in_handle) {

    }

    void EventAdapter::playLoopJumpAfter(auth_3d_detail::Handle& in_handle) {

    }

    void EventAdapter::playLoopJumpBefore(auth_3d_detail::Handle& in_handle) {

    }
}

namespace auth_3d_detail {
    // 0x1401AF2F0
    const Fcurve::KeyData* Fcurve::KeyData::next() const {
        return this + 1;
    }

    // Missing
    Fcurve::KeyData* Fcurve::KeyData::next() {
        return this + 1;
    }

    // 0x1401AF360
    const Fcurve::KeyData* Fcurve::KeyData::prev() const {
        return this - 1;
    }

    // Missing
    Fcurve::KeyData* Fcurve::KeyData::prev() {
        return this - 1;
    }

    // 0x1401C1DF0
    float_t Fcurve::KeyData::S_calc(FC_TYPE in_type, float_t in_frame,
        const Fcurve::KeyData* curr_key, const Fcurve::KeyData* next_key) {
        switch (in_type) {
        case FC_TYPE_LINEAR:
            if (curr_key->frame < next_key->frame)
                return interpolate_linear_value(curr_key->value, next_key->value,
                    curr_key->frame, next_key->frame, in_frame);
            else
                return curr_key->value;
        case FC_TYPE_HERMITE:
            if (curr_key->frame < next_key->frame)
                return interpolate_chs_value(curr_key->value, next_key->value,
                    curr_key->r_slope, next_key->l_slope,
                    curr_key->frame, next_key->frame, in_frame);
            else
                return curr_key->value;
        case FC_TYPE_NO_INTERPOLATION:
            if (in_frame >= next_key->frame)
                return next_key->value;
            else
                return curr_key->value;
        default:
            return 0.0f;
        }
    }

    // 0x1401BF960
    void Fcurve::M_init() {
        m_max_frame = 0.0f;
        m_ep_type_pre = EP_TYPE_CONSTANT;
        m_ep_type_post = EP_TYPE_CONSTANT;
        m_frame_delta = 0.0f;
        m_value_delta = 0.0f;
        m_key_data_list.clear();
        m_key_data_list.shrink_to_fit();
        m_key_data_size = 0;
        m_key_data_ptr = 0;

        m_frame = 0.0f;
        m_value = 0.0f;
    }

    // 0x1401BF870
    float_t Fcurve::M_get(float_t in_frame) const {
        const KeyData* first_key = m_key_data_ptr;
        const KeyData* next_key = find_key_data(in_frame);

        if (next_key == first_key)
            return first_key->value;

        const KeyData* prev_key = next_key->prev();
        if (next_key == &first_key[m_key_data_size])
            return prev_key->value;

        return KeyData::S_calc(m_type, in_frame, prev_key, next_key);
    }

    // 0x1401C1B00
    void Fcurve::M_set_ep_type(EP_TYPE in_ep_type_pre, EP_TYPE in_ep_type_post) {
        m_ep_type_pre = EP_TYPE_CONSTANT;
        m_ep_type_post = EP_TYPE_CONSTANT;
        m_frame_delta = m_max_frame;
        m_value_delta = 0.0f;

        if (m_key_data_size) {
            const KeyData* first_key = &m_key_data_ptr[0];
            const KeyData* last_key = &m_key_data_ptr[m_key_data_size - 1];
            if (first_key->frame < last_key->frame
                && check_ep_range(0.0f, m_max_frame, first_key->frame, last_key->frame)) {
                m_ep_type_pre = in_ep_type_pre;
                m_ep_type_post = in_ep_type_post;
                m_frame_delta = last_key->frame - first_key->frame;
                m_value_delta = last_key->value - first_key->value;
            }
        }
    }

    // 0x1401B3E60
    Fcurve::Fcurve() : m_type(), m_static_value(), m_max_frame(),
        m_ep_type_pre(), m_ep_type_post(), m_frame_delta(), m_value_delta(),
        m_key_data_list(), m_key_data_size(), m_key_data_ptr(), m_frame(), m_value() {

    }

    // Missing
    Fcurve::Fcurve(float_t in_value) : m_type(FC_TYPE_STATIC_DATA), m_static_value(in_value),
        m_max_frame(), m_ep_type_pre(), m_ep_type_post(), m_frame_delta(), m_value_delta(),
        m_key_data_list(), m_key_data_size(), m_key_data_ptr(), m_frame(), m_value() {

    }

    // 0x1401B3DF0
    Fcurve::Fcurve(const Fcurve& other) : m_type(other.m_type), m_static_value(other.m_static_value),
        m_max_frame(other.m_max_frame), m_ep_type_pre(other.m_ep_type_pre), m_ep_type_post(other.m_ep_type_post),
        m_frame_delta(other.m_frame_delta), m_value_delta(other.m_value_delta),
        m_key_data_list(other.m_key_data_list), m_key_data_size(other.m_key_data_size),
        m_key_data_ptr(other.m_key_data_ptr), m_frame(other.m_frame), m_value(other.m_value) {

    }

    // Missing
    Fcurve::~Fcurve() {

    }

    // 0x1401D6F80
    void Fcurve::init() {
        m_type = FC_TYPE_STATIC_0;
        m_static_value = 0.0f;
        M_init();
    }

    // 0x1401D6F20
    void Fcurve::init(float_t in_value) {
        m_type = FC_TYPE_STATIC_DATA;
        m_static_value = in_value;
        M_init();
    }

    // 0x1401D4230
    float_t Fcurve::get(float_t in_frame) {
        if (m_type == FC_TYPE_STATIC_DATA)
            return m_static_value;
        else if (m_type < FC_TYPE_STATIC_DATA
            || m_type > FC_TYPE_NO_INTERPOLATION || !m_key_data_size)
            return 0.0f;

        const KeyData* first_key = &m_key_data_ptr[0];
        const KeyData* last_key = &m_key_data_ptr[m_key_data_size - 1];
        float_t first_frame = first_key->frame;
        float_t last_frame = last_key->frame;

        float_t offset = 0.0f;
        float_t frame = in_frame;
        if (frame < first_frame) {
            float_t delta_frame = first_frame - frame;
            switch (m_ep_type_pre) {
            case a3d::EP_TYPE_CONSTANT:
            default:
                m_frame = frame;
                m_value = first_key->value;
                return m_value;
            case a3d::EP_TYPE_LINEAR:
                m_frame = frame;
                m_value = first_key->value + delta_frame * first_key->l_slope;
                return m_value;
            case a3d::EP_TYPE_CYCLE:
                frame = last_frame - fmodf(delta_frame, m_frame_delta);
                break;
            case a3d::EP_TYPE_CYCLE_WITH_OFFSET:
                offset = (prj::truncf(delta_frame / m_frame_delta) + 1.0f) * m_value_delta;
                frame = last_frame - fmodf(delta_frame, m_frame_delta);
                break;
            }
        }
        else if (frame >= last_frame) {
            float_t delta_frame = frame - last_frame;
            switch (m_ep_type_post) {
            case a3d::EP_TYPE_CONSTANT:
            default:
                m_frame = frame;
                m_value = last_key->value;
                return m_value;
            case a3d::EP_TYPE_LINEAR:
                m_frame = frame;
                m_value = last_key->value - delta_frame * last_key->r_slope;
                return m_value;
            case a3d::EP_TYPE_CYCLE:
                frame = first_frame + fmodf(delta_frame, m_frame_delta);
                break;
            case a3d::EP_TYPE_CYCLE_WITH_OFFSET:
                offset = (prj::truncf(delta_frame / m_frame_delta) + 1.0f) * m_value_delta;
                frame = first_frame + fmodf(delta_frame, m_frame_delta);
                break;
            }
        }

        m_frame = frame;
        m_value = M_get(frame) + offset;
        return m_value;
    }

    // Missing
    float_t Fcurve::operator()(float_t in_frame) {
        return get(in_frame);
    }

    // 0x1401D3AB0
    const Fcurve::KeyData* Fcurve::find_key_data(float_t in_frame) const {
        return std::upper_bound(m_key_data_ptr, m_key_data_ptr + m_key_data_size,
            in_frame, [](float_t in_frame, const KeyData& key) {
                return in_frame < key.frame;
            });
    }

    // Missing
    float_t Fcurve::get_size() const {
        return m_max_frame;
    }

    // 0x1401CBFF0
    bool Fcurve::check_discontinuity(float_t in_frame) const {
        if (m_type < FC_TYPE_LINEAR || m_type > FC_TYPE_NO_INTERPOLATION)
            return false;

        const float_t frame = clamp_def(in_frame, 0.0f, m_max_frame);
        if (!m_key_data_size)
            return false;

        const KeyData* key = find_key_data(frame);
        if (key == m_key_data_ptr || key == &m_key_data_ptr[m_key_data_size])
            return false;

        return key->frame - key->prev()->frame < 2.0f;
    }

    // 0x1401CC540
    bool Fcurve::check_threshold_discontinuity(float_t in_frame, float_t in_threshold) const {
        if (m_type < FC_TYPE_LINEAR || m_type > FC_TYPE_NO_INTERPOLATION)
            return false;

        const float_t frame = clamp_def(in_frame, 0.0f, m_max_frame);
        if (!m_key_data_size)
            return false;

        const KeyData* keys = m_key_data_ptr;
        const KeyData* keys_end = &keys[m_key_data_size];
        const KeyData* key = find_key_data(frame);
        if (key == keys || key == keys_end || key == keys->next())
            return false;

        const KeyData* next_key = key->next();
        if (next_key == keys_end)
            return false;

        const KeyData* prev_key = key->prev();

        const float_t frame_diff = key->frame - prev_key->frame;
        if (frame_diff >= 2.0f || frame_diff <= 0.0f)
            return false;

        if (fabsf(key->value - prev_key->value) <= in_threshold)
            return false;

        const KeyData* prev2_key = prev_key->prev();
        while (prev_key->frame <= prev2_key->frame)
            if (prev2_key == keys)
                return false;
            else
                prev2_key = prev2_key->prev();

        const KeyData* next_key_1 = next_key;
        while (key->frame >= next_key->frame)
            next_key_1 = next_key;

        // Intentional! Should be (V2 - V1) / (F2 - F1), but first parenthesis is missing
        const float_t center_slope = key->value - prev_key->value / frame_diff;

        float_t left_slope;
        float_t right_slope;
        if (m_type != FC_TYPE_HERMITE) {
            left_slope = (prev_key->value - prev2_key->value) / (prev_key->frame - prev2_key->frame);
            right_slope = (next_key_1->value - key->value) / (next_key_1->frame - key->frame);
        }
        else {
            if ((prev_key->frame - prev2_key->frame) >= 2.0f) {
                float_t value;
                if (prev2_key->frame < prev_key->frame)
                    value = interpolate_chs_value(
                        prev2_key->value, prev_key->value, prev2_key->r_slope, prev_key->l_slope,
                        prev_key->frame, prev2_key->frame, prev_key->frame - 1.0f);
                else
                    value = prev2_key->value;
                left_slope = prev_key->value - value;
            }
            else
                left_slope = (prev_key->value - prev2_key->value) / (prev_key->frame - prev2_key->frame);

            if (next_key_1->frame - key->frame >= 2.0f) {
                float_t value;
                if (key->frame < next_key_1->frame)
                    value = interpolate_chs_value(
                        key->value, next_key_1->value, key->r_slope, next_key_1->l_slope,
                        key->frame, next_key_1->frame, key->frame + 1.0f);
                else
                    value = key->value;
                right_slope = value - key->value;
            }
            else
                right_slope = (next_key_1->value - key->value) / (next_key_1->frame - key->frame);
        }

        const float_t left_dot = left_slope * center_slope;
        const float_t right_dot = right_slope * center_slope;

        const float_t abs_left_slope = fabsf(left_slope);
        const float_t abs_center_slope = fabsf(center_slope);
        const float_t abs_right_slope = fabsf(right_slope);

        bool left_small = abs_left_slope < abs_center_slope && abs_left_slope <= in_threshold;
        bool right_small = abs_right_slope < abs_center_slope && abs_right_slope <= in_threshold;
        return (left_dot <= 0.0f || left_small)
            && (right_dot <= 0.0f || right_small);
    }

    // 0x1401DB1A0
    bool Fcurve::load(const SceneFile& in_file, const a3d::Fcurve& in_fcv) {
        m_max_frame = in_file.frame_max;
        m_frame_delta = m_max_frame;
        m_value_delta = 0.0f;

        switch (in_fcv.type) {
        case a3d::FC_TYPE_STATIC_0:
            m_type = FC_TYPE_STATIC_0;
            m_static_value = 0.0f;
            return true;
        case a3d::FC_TYPE_STATIC_DATA:
            m_type = in_fcv.value != 0.0f ? FC_TYPE_STATIC_DATA : FC_TYPE_STATIC_0;
            m_static_value = in_fcv.value;
            return true;
        case a3d::FC_TYPE_LINEAR:
            m_type = FC_TYPE_LINEAR;
            break;
        case a3d::FC_TYPE_HERMITE:
        default:
            m_type = FC_TYPE_HERMITE;
            break;
        case a3d::FC_TYPE_NO_INTERPOLATION:
            m_type = FC_TYPE_NO_INTERPOLATION;
            break;
        }

        size_t key_data_size = in_fcv.keys.size();
        if (key_data_size > 1) {
            m_key_data_list.assign(in_fcv.keys.begin(), in_fcv.keys.end());
            m_key_data_size = key_data_size;
            m_key_data_ptr = m_key_data_list.data();

            M_set_ep_type((EP_TYPE)in_fcv.ep_type_pre, (EP_TYPE)in_fcv.ep_type_post);
        }
        else if (key_data_size == 1) {
            float_t value = in_fcv.keys.front().value;
            m_type = value != 0.0f ? FC_TYPE_STATIC_DATA : FC_TYPE_STATIC_0;
            m_static_value = value;
        }
        else {
            m_type = FC_TYPE_STATIC_0;
            m_static_value = 0.0f;
        }
        return true;
    }

    // Added
    void Fcurve::store(const SceneFile& in_file, a3d::Fcurve& out_fcv) const {
        out_fcv.max_frame = in_file.frame_max;

        switch (m_type) {
        case FC_TYPE_STATIC_0:
            out_fcv.type = a3d::FC_TYPE_STATIC_0;
            out_fcv.value = 0.0f;
            return;
        case FC_TYPE_STATIC_DATA:
            out_fcv.type = m_static_value != 0.0f ? a3d::FC_TYPE_STATIC_DATA : a3d::FC_TYPE_STATIC_0;
            out_fcv.value = m_static_value;
            return;
        case FC_TYPE_LINEAR:
            out_fcv.type = a3d::FC_TYPE_LINEAR;
            break;
        case FC_TYPE_HERMITE:
        default:
            out_fcv.type = a3d::FC_TYPE_HERMITE;
            break;
        case FC_TYPE_NO_INTERPOLATION:
            out_fcv.type = a3d::FC_TYPE_NO_INTERPOLATION;
            break;
        }

        size_t key_data_size = m_key_data_list.size();
        if (key_data_size > 1) {
            out_fcv.keys.assign(m_key_data_list.begin(), m_key_data_list.end());

            const KeyData* first_key = &m_key_data_ptr[0];
            const KeyData* last_key = &m_key_data_ptr[key_data_size - 1];
            if (first_key->frame < last_key->frame
                && check_ep_range(0.0f, m_max_frame, first_key->frame, last_key->frame)) {
                out_fcv.ep_type_pre = (a3d::EP_TYPE)m_ep_type_pre;
                out_fcv.ep_type_post = (a3d::EP_TYPE)m_ep_type_post;
            }
        }
        else if (key_data_size == 1) {
            float_t value = m_key_data_list.front().value;
            out_fcv.type = value != 0.0f ? a3d::FC_TYPE_STATIC_DATA : a3d::FC_TYPE_STATIC_0;
            out_fcv.value = value;
        }
        else {
            out_fcv.type = a3d::FC_TYPE_STATIC_0;
            out_fcv.value = 0.0f;
        }
    }

    // 0x1401BBAB0
    Fcurve& Fcurve::operator=(const Fcurve& other) {
        m_type = other.m_type;
        m_static_value = other.m_static_value;
        m_max_frame = other.m_max_frame;
        m_ep_type_pre = other.m_ep_type_pre;
        m_ep_type_post = other.m_ep_type_post;
        m_frame_delta = other.m_frame_delta;
        m_value_delta = other.m_value_delta;
        m_key_data_list.assign(other.m_key_data_list.begin(), other.m_key_data_list.end());
        m_key_data_size = other.m_key_data_size;
        m_key_data_ptr = m_key_data_list.data();
        return *this;
    }

    // 0x1401AF540
    bool Fcurve::check_ep_range(float_t in_frame_begin, float_t in_frame_end,
        float_t in_key_frame_begin, float_t in_key_frame_end) {
        return in_key_frame_end > in_frame_begin && in_frame_end > in_key_frame_begin;
    }

    // 0x1401B3D60
    Fcurve3f::Fcurve3f() : x(), y(), z() {

    }

    // Missing
    Fcurve3f::Fcurve3f(float_t in_value) : x(in_value), y(in_value), z(in_value) {

    }

    // 0x1401B3C10
    Fcurve3f::Fcurve3f(const Fcurve3f& other) : x(other.x), y(other.y), z(other.z) {

    }

    // 0x1401B8850
    Fcurve3f::~Fcurve3f() {

    }

    // 0x1401D6E20
    void Fcurve3f::init() {
        x.init();
        y.init();
        z.init();
    }

    // 0x1401D6D00
    void Fcurve3f::init(float_t in_value) {
        x.init(in_value);
        y.init(in_value);
        z.init(in_value);
    }

    // 0x1401D41B0
    vec3 Fcurve3f::get(float_t in_frame) {
        vec3 value;
        value.x = x.get(in_frame);
        value.y = y.get(in_frame);
        value.z = z.get(in_frame);
        return value;
    }

    // 0x1401DB050
    bool Fcurve3f::load(const SceneFile& in_file, const a3d::Fcurve3f& in_fcv3f) {
        x.load(in_file, in_fcv3f.x);
        y.load(in_file, in_fcv3f.y);
        z.load(in_file, in_fcv3f.z);
        return true;
    }

    // Added
    void Fcurve3f::store(const SceneFile& in_file, a3d::Fcurve3f& out_fcv3f) const {
        x.store(in_file, out_fcv3f.x);
        y.store(in_file, out_fcv3f.y);
        z.store(in_file, out_fcv3f.z);
    }

    // 0x1401BB970
    Fcurve3f& Fcurve3f::operator=(const Fcurve3f& other) {
        x = other.x;
        y = other.y;
        z = other.z;
        return *this;
    }

    // 0x1401B4090
    FcurveColor4f::FcurveColor4f() : r(), g(), b(), a(), has_r(), has_g(), has_b(), has_a() {

    }

    // 0x1401B3E90
    FcurveColor4f::FcurveColor4f(const FcurveColor4f& other) : r(other.r), g(other.g), b(other.b), a(other.a),
        has_r(other.has_r), has_g(other.has_g), has_b(other.has_g), has_a(other.has_g) {

    }

    // 0x1401B8900
    FcurveColor4f::~FcurveColor4f() {

    }

    // 0x1401D6FE0
    void FcurveColor4f::init() {
        r.init();
        g.init();
        b.init();
        a.init();

        has_r = false;
        has_g = false;
        has_b = false;
        has_a = false;
    }

    // 0x1401D4450
    vec4 FcurveColor4f::get(float_t in_frame) {
        vec4 value;
        value.x = r.get(in_frame);
        value.y = g.get(in_frame);
        value.z = b.get(in_frame);
        value.w = a.get(in_frame);
        return value;
    }

    // 0x1401DB520
    bool FcurveColor4f::load(const SceneFile& in_file, const a3d::FcurveColor4f& in_fcv_col4f) {
        if (in_fcv_col4f.has_r) {
            r.load(in_file, in_fcv_col4f.r);
            has_r = true;
        }

        if (in_fcv_col4f.has_g) {
            g.load(in_file, in_fcv_col4f.g);
            has_g = true;
        }

        if (in_fcv_col4f.has_b) {
            b.load(in_file, in_fcv_col4f.b);
            has_b = true;
        }

        if (in_fcv_col4f.has_a) {
            a.load(in_file, in_fcv_col4f.a);
            has_a = true;
        }
        return has_r || has_g || has_b || has_a;
    }

    // Added
    void FcurveColor4f::store(const SceneFile& in_file, a3d::FcurveColor4f& out_fcv_col4f) const {
        if (has_r) {
            r.store(in_file, out_fcv_col4f.r);
            out_fcv_col4f.has_r = true;
        }

        if (has_g) {
            g.store(in_file, out_fcv_col4f.g);
            out_fcv_col4f.has_g = true;
        }

        if (has_b) {
            b.store(in_file, out_fcv_col4f.b);
            out_fcv_col4f.has_b = true;
        }

        if (has_a) {
            a.store(in_file, out_fcv_col4f.a);
            out_fcv_col4f.has_a = true;
        }
    }

    // 0x1401BBB20
    FcurveColor4f& FcurveColor4f::operator=(const FcurveColor4f& other) {
        r = other.r;
        g = other.g;
        b = other.b;
        a = other.a;

        has_r = other.has_r;
        has_g = other.has_g;
        has_b = other.has_b;
        has_a = other.has_a;
        return *this;
    }

    // 0x1401B30B0
    Color::Color() : fcurve_color(), color(), has_color() {

    }

    // 0x1401B3050
    Color::Color(const Color& other) : fcurve_color(other.fcurve_color), color(other.color) {
        has_color[0] = other.has_color[0];
        has_color[1] = other.has_color[1];
        has_color[2] = other.has_color[2];
        has_color[3] = other.has_color[3];
    }

    // 0x1401B85A0
    Color::~Color() {
        init();
    }

    /// 0x1401D6CA0
    void Color::init() {
        fcurve_color.init();

        has_color[0] = true;
        has_color[1] = true;
        has_color[2] = true;
        has_color[3] = true;
    }

    // 0x1401D4070
    void Color::get(float_t in_frame) {
        color = fcurve_color.get(in_frame);

        has_color[0] = fcurve_color.has_r;
        has_color[1] = fcurve_color.has_b; // TYPO!
        has_color[2] = fcurve_color.has_g; // TYPO!
        has_color[3] = fcurve_color.has_a;
    }

    // 0x1401DA200
    bool Color::load(const SceneFile& in_file, const a3d::FcurveColor4f& in_fcv_col4f) {
        fcurve_color.load(in_file, in_fcv_col4f);
        return true;
    }

    // Added
    void Color::store(const SceneFile& in_file, a3d::FcurveColor4f& out_fcv_col4f) const {
        fcurve_color.store(in_file, out_fcv_col4f);
    }

    // 0x1401BB740
    Color& Color::operator=(const Color& other) {
        fcurve_color = other.fcurve_color;
        color = other.color;
        has_color[0] = other.has_color[0];
        has_color[1] = other.has_color[1];
        has_color[2] = other.has_color[2];
        has_color[3] = other.has_color[3];
        return *this;
    }

    // 0x1401B4E10
    ModelTransform::ModelTransform() : matrix(), fcurve_scale(), fcurve_rotation(),
        fcurve_translation(), fcurve_visibility(), scale(), rotation(), translation(), visibility() {

    }

    // 0x1401B4C50
    ModelTransform::ModelTransform(const ModelTransform& other) : matrix(other.matrix),
        fcurve_scale(other.fcurve_scale), fcurve_rotation(other.fcurve_rotation),
        fcurve_translation(other.fcurve_translation), fcurve_visibility(other.fcurve_visibility),
        scale(other.scale), rotation(other.rotation), translation(other.translation), visibility(other.visibility) {

    }

    // 0x1401B8D50
    ModelTransform::~ModelTransform() {

    }

    // 0x1401D74B0
    void ModelTransform::init() {
        matrix = mat4_identity;

        fcurve_scale.init();
        fcurve_rotation.init();
        fcurve_translation.init();
        fcurve_visibility.init();

        scale = 0.0f;
        rotation = 0.0f;
        translation = 0.0f;
        visibility = false;
    }

    // 0x1401D8840
    void ModelTransform::init_static() {
        matrix = mat4_identity;

        fcurve_scale.init(1.0f);
        fcurve_rotation.init(0.0f);
        fcurve_translation.init(0.0f);
        fcurve_visibility.init(1.0f);

        scale = 1.0f;
        rotation = 0.0f;
        translation = 0.0f;
        visibility = true;
    }

    // 0x1401D4680
    void ModelTransform::get(float_t in_frame) {
        scale = fcurve_scale.get(in_frame);
        rotation = fcurve_rotation.get(in_frame);
        translation = fcurve_translation.get(in_frame);
        visibility = fcurve_visibility.get(in_frame) >= 0.999f;
        if (visibility && (scale.x == 0.0f || scale.y == 0.0f || scale.z == 0.0f))
            visibility = false;
    }

    // 0x1401E1CB0
    const mat4& ModelTransform::mul(const mat4& in_base_mat) {
        matrix = in_base_mat;
        mat4_mul_translate(&matrix, &translation, &matrix);
        mat4_mul_rotate_zyx(&matrix, &rotation, &matrix);
        mat4_scale_rot(&matrix, &scale, &matrix);
        return matrix;
    }

    // Inlined
    inline const mat4& ModelTransform::get_matrix() {
        return matrix;
    }

    // 0x1401DCFB0
    bool ModelTransform::load(const SceneFile& in_file, const a3d::ModelTransform& in_mt) {
        fcurve_rotation.load(in_file, in_mt.rotation);
        fcurve_scale.load(in_file, in_mt.scale);
        fcurve_translation.load(in_file, in_mt.translation);
        fcurve_visibility.load(in_file, in_mt.visibility);
        return true;
    }

    // Added
    void ModelTransform::store(const SceneFile& in_file, a3d::ModelTransform& out_mt) const {
        fcurve_rotation.store(in_file, out_mt.rotation);
        fcurve_scale.store(in_file, out_mt.scale);
        fcurve_translation.store(in_file, out_mt.translation);
        fcurve_visibility.store(in_file, out_mt.visibility);
    }

    // 0x1401BC1F0
    ModelTransform& ModelTransform::operator=(const ModelTransform& other) {
        matrix = other.matrix;

        fcurve_scale = other.fcurve_scale;
        fcurve_rotation = other.fcurve_rotation;
        fcurve_translation = other.fcurve_translation;
        fcurve_visibility = other.fcurve_visibility;

        scale = other.scale;
        rotation = other.rotation;
        translation = other.translation;
        visibility = other.visibility;
        return *this;
    }

    // MGF
    Ambient::Ambient() : flag(), name(), light_diffuse(), rim_light_diffuse() {

    }

    // MGF
    Ambient::Ambient(const Ambient& other) : flag(other.flag), name(other.name),
        light_diffuse(other.light_diffuse), rim_light_diffuse(other.rim_light_diffuse) {

    }

    // MGF
    Ambient::~Ambient() {

    }

    // MGF
    void Ambient::init() {
        flag = 0;
        name.clear();
        name.shrink_to_fit();
        light_diffuse.init();
        rim_light_diffuse.init();
    }

    // MGF
    void Ambient::get(float_t in_frame) {
        if (has_light_diffuse())
            light_diffuse.get(in_frame);

        if (has_rim_light_diffuse())
            rim_light_diffuse.get(in_frame);
    }

    // MGF
    void Ambient::ctrl(render_context* rctx) {

    }

    // MGF
    bool Ambient::load(const SceneFile& in_file, const a3d::Ambient& in_a) {
        name.assign(in_a.name);

        if (in_a.flag & a3d::Ambient::FLAG_LIGHT_DIFFUSE) {
            light_diffuse.load(in_file, in_a.light_diffuse);
            flag |= FLAG_LIGHT_DIFFUSE;
        }

        if (in_a.flag & a3d::Ambient::FLAG_RIM_LIGHT_DIFFUSE) {
            rim_light_diffuse.load(in_file, in_a.rim_light_diffuse);
            flag |= FLAG_RIM_LIGHT_DIFFUSE;
        }
        return true;
    }

    // MGF, Added
    void Ambient::store(const SceneFile& in_file, a3d::Ambient& out_a) const {
        out_a.name.assign(name);

        if (flag & FLAG_LIGHT_DIFFUSE) {
            light_diffuse.store(in_file, out_a.light_diffuse);
            out_a.flag |= a3d::Ambient::FLAG_LIGHT_DIFFUSE;
        }

        if (flag & FLAG_RIM_LIGHT_DIFFUSE) {
            rim_light_diffuse.store(in_file, out_a.rim_light_diffuse);
            out_a.flag |= a3d::Ambient::FLAG_RIM_LIGHT_DIFFUSE;
        }
    }

    // MGF
    Ambient& Ambient::operator=(const Ambient& other) {
        flag = other.flag;
        name.assign(other.name);
        light_diffuse = other.light_diffuse;
        rim_light_diffuse = other.rim_light_diffuse;
        return *this;
    }

    // 0x1401B2790
    Auth2d::Auth2d() : name() {

    }

    // 0x1401B2750
    Auth2d::Auth2d(const Auth2d& other) : name(other.name) {

    }

    // 0x1401B7D80
    Auth2d::~Auth2d() {

    }

    // 0x1401D9960
    bool Auth2d::load(const SceneFile& in_file, const a3d::Auth2d& in_a2) {
        name.assign(in_a2.name);

        if (!name.find("S_ADVPR0"))
            name[7] = 'O';
        return true;
    }

    // Added
    void Auth2d::store(const SceneFile& in_file, a3d::Auth2d& out_a2) const {
        out_a2.name.assign(name);
    }

    // 0x1401BB350
    Auth2d& Auth2d::operator=(const Auth2d& other) {
        name.assign(other.name);
        return *this;
    }

    // 0x1401B28C0
    CameraAuxiliary::CameraAuxiliary() : flag(), fcurve_exposure(), fcurve_exposure_rate(),
        fcurve_gamma(), fcurve_gamma_rate(), fcurve_saturate(), fcurve_auto_exposure(), exposure(2.0f),
        exposure_rate(1.0f), gamma(1.0f), gamma_rate(1.0f), saturate(1.0f), auto_exposure(1.0f) {

    }

    // Missing
    CameraAuxiliary::CameraAuxiliary(const CameraAuxiliary& other) : flag(other.flag),
        fcurve_exposure(other.fcurve_exposure), fcurve_exposure_rate(other.fcurve_exposure_rate),
        fcurve_gamma(other.fcurve_gamma), fcurve_gamma_rate(other.fcurve_gamma_rate),
        fcurve_saturate(other.fcurve_saturate), fcurve_auto_exposure(other.fcurve_auto_exposure),
        exposure(other.exposure), exposure_rate(other.exposure_rate), gamma(other.gamma),
        gamma_rate(other.gamma_rate), saturate(other.saturate), auto_exposure(other.auto_exposure) {

    }

    // 0x1401B8140
    CameraAuxiliary::~CameraAuxiliary() {

    }

    // 0x1401D69D0
    void CameraAuxiliary::init() {
        flag = 0;
        fcurve_exposure.init();
        fcurve_exposure_rate.init(); // F/F2/X/XHD
        fcurve_gamma.init();
        fcurve_gamma_rate.init(); // F/F2/X/XHD
        fcurve_saturate.init();
        fcurve_auto_exposure.init();
    }

    // 0x1401D3BD0
    void CameraAuxiliary::get(float_t in_frame) {
        if (has_exposure())
            exposure = fcurve_exposure.get(in_frame);
        if (has_exposure_rate()) // F/F2/X/XHD
            exposure_rate = fcurve_exposure_rate.get(in_frame);
        if (has_gamma())
            gamma = fcurve_gamma.get(in_frame);
        if (has_gamma_rate()) // F/F2/X/XHD
            gamma_rate = fcurve_gamma_rate.get(in_frame);
        if (has_saturate())
            saturate = fcurve_saturate.get(in_frame);
        if (has_auto_exposure())
            auto_exposure = fcurve_auto_exposure.get(in_frame);
    }

    // 0x1401CE3E0
    void CameraAuxiliary::ctrl(Scene& in_scene, render_context* rctx) {
        if (has_exposure())
            rctx->render.set_exposure(exposure);
        if (has_gamma())
            rctx->render.set_gamma(gamma);
        if (has_saturate())
            rctx->render.set_saturate_coeff(saturate, 0, false);
        if (has_auto_exposure())
            rctx->render.set_auto_exposure(auto_exposure > 0.0f);
    }

    // 0x1401CFDE0
    void CameraAuxiliary::dest(render_context* rctx) {
        if (has_exposure())
            rctx->render.set_exposure(2.0f);
        if (has_gamma())
            rctx->render.set_gamma(1.0f);
        if (has_saturate())
            rctx->render.set_saturate_coeff(1.0f, 0, false);
        if (has_auto_exposure())
            rctx->render.set_auto_exposure(true);

        init();
    }

    // 0x1401D9A60
    bool CameraAuxiliary::load(const SceneFile& in_file, const a3d::CameraAuxiliary& in_ca) {
        if (in_ca.flag & a3d::CameraAuxiliary::FLAG_EXPOSURE) {
            fcurve_exposure.load(in_file, in_ca.exposure);
            flag |= FLAG_EXPOSURE;
        }

        // F/F2/X/XHD
        if (in_ca.flag & a3d::CameraAuxiliary::FLAG_EXPOSURE_RATE) {
            fcurve_exposure_rate.load(in_file, in_ca.exposure_rate);
            flag |= FLAG_EXPOSURE_RATE;
        }

        if (in_ca.flag & a3d::CameraAuxiliary::FLAG_GAMMA) {
            fcurve_gamma.load(in_file, in_ca.gamma);
            flag |= FLAG_GAMMA;
        }

        // F/F2/X/XHD
        if (in_ca.flag & a3d::CameraAuxiliary::FLAG_GAMMA_RATE) {
            fcurve_gamma_rate.load(in_file, in_ca.gamma_rate);
            flag |= FLAG_GAMMA_RATE;
        }

        if (in_ca.flag & a3d::CameraAuxiliary::FLAG_SATURATE) {
            fcurve_saturate.load(in_file, in_ca.saturate);
            flag |= FLAG_SATURATE;
        }

        if (in_ca.flag & a3d::CameraAuxiliary::FLAG_AUTO_EXPOSURE) {
            fcurve_auto_exposure.load(in_file, in_ca.auto_exposure);
            flag |= FLAG_AUTO_EXPOSURE;
        }
        return true;
    }

    // Added
    void CameraAuxiliary::store(const SceneFile& in_file, a3d::CameraAuxiliary& out_ca) const {
        if (flag & FLAG_EXPOSURE) {
            fcurve_exposure.store(in_file, out_ca.exposure);
            out_ca.flag |= a3d::CameraAuxiliary::FLAG_EXPOSURE;
        }

        // F/F2/X/XHD
        if (flag & FLAG_EXPOSURE_RATE) {
            fcurve_exposure_rate.store(in_file, out_ca.exposure_rate);
            out_ca.flag |= a3d::CameraAuxiliary::FLAG_EXPOSURE_RATE;
        }

        if (flag & FLAG_GAMMA) {
            fcurve_gamma.store(in_file, out_ca.gamma);
            out_ca.flag |= a3d::CameraAuxiliary::FLAG_GAMMA;
        }

        // F/F2/X/XHD
        if (flag & FLAG_GAMMA_RATE) {
            fcurve_gamma_rate.store(in_file, out_ca.gamma_rate);
            out_ca.flag |= a3d::CameraAuxiliary::FLAG_GAMMA_RATE;
        }

        if (flag & FLAG_SATURATE) {
            fcurve_saturate.store(in_file, out_ca.saturate);
            out_ca.flag |= a3d::CameraAuxiliary::FLAG_SATURATE;
        }

        if (flag & FLAG_AUTO_EXPOSURE) {
            fcurve_auto_exposure.store(in_file, out_ca.auto_exposure);
            out_ca.flag |= a3d::CameraAuxiliary::FLAG_AUTO_EXPOSURE;
        }
    }

    // Missing
    CameraAuxiliary& CameraAuxiliary::operator=(const CameraAuxiliary& other) {
        flag = other.flag;
        fcurve_exposure = other.fcurve_exposure;
        fcurve_exposure_rate = other.fcurve_exposure_rate; // F/F2/X/XHD
        fcurve_gamma = other.fcurve_gamma;
        fcurve_gamma_rate = other.fcurve_gamma_rate; // F/F2/X/XHD
        fcurve_saturate = other.fcurve_saturate;
        fcurve_auto_exposure = other.fcurve_auto_exposure;

        exposure = other.exposure;
        exposure_rate = other.exposure_rate; // F/F2/X/XHD
        gamma = other.gamma;
        gamma_rate = other.gamma_rate; // F/F2/X/XHD
        saturate = other.saturate;
        auto_exposure = other.auto_exposure;
        return *this;
    }

    // 0x1401B29E0
    CameraInterest::CameraInterest() : model_transform() {
        init();
    }

    // 0x1401B29C0
    CameraInterest::CameraInterest(const CameraInterest& other) : model_transform(other.model_transform) {

    }

    // 0x1401B81E0
    CameraInterest::~CameraInterest() {

    }

    // Inlined
    inline void CameraInterest::init() {
        model_transform.init();
    }

    // 0x1401D9C90
    bool CameraInterest::load(const SceneFile& in_file, const a3d::CameraInterest& in_ci) {
        model_transform.load(in_file, in_ci.model_transform);
        return true;
    }

    // Added
    void CameraInterest::store(const SceneFile& in_file, a3d::CameraInterest& out_ci) const {
        model_transform.store(in_file, out_ci.model_transform);
    }

    // 0x1401BB380
    CameraInterest& CameraInterest::operator=(const CameraInterest& other) {
        model_transform = other.model_transform;
        return *this;
    }

    // 0x1401B2CA0
    CameraViewPoint::CameraViewPoint() : model_transform(), roll(), has_fov(), fov_is_horizontal(),
        fov(), aspect(), focal_length(), camera_aperture_w(), camera_aperture_h() {
        init();
    }

    // 0x1401B2AA0
    CameraViewPoint::CameraViewPoint(const CameraViewPoint& other) : model_transform(other.model_transform),
        roll(other.roll), has_fov(other.has_fov), fov_is_horizontal(other.fov_is_horizontal),
        fov(other.fov), aspect(other.aspect), focal_length(other.focal_length),
        camera_aperture_w(other.camera_aperture_w), camera_aperture_h(other.camera_aperture_h) {

    }

    // 0x1401B8370
    CameraViewPoint::~CameraViewPoint() {

    }

    // 0x1401D6B30
    void CameraViewPoint::init() {
        model_transform.init();
        roll.init();
        has_fov = true;
        fov.init();
        fov_is_horizontal = false;
        aspect = 1.667f;
        focal_length.init();
        camera_aperture_w = 1.0f;
        camera_aperture_h = 1.0f;
    }

    // 0x1401D9D90
    bool CameraViewPoint::load(const SceneFile& in_file, const a3d::CameraViewPoint& in_cvp) {
        model_transform.load(in_file, in_cvp.model_transform);
        roll.load(in_file, in_cvp.roll);

        if (in_cvp.has_fov) {
            has_fov = true;
            fov.load(in_file, in_cvp.fov);
            fov_is_horizontal = in_cvp.fov_is_horizontal;
            aspect = in_cvp.aspect;
        }
        else {
            has_fov = false;
            focal_length.load(in_file, in_cvp.focal_length);
            camera_aperture_w = in_cvp.camera_aperture_w;
            camera_aperture_h = in_cvp.camera_aperture_h;
        }
        return true;
    }

    // 0x1401D6380
    float_t CameraViewPoint::get_v_fov(float_t in_frame) {
        if (has_fov) {
            float_t fov_value = fov.get(in_frame);
            if (fov_is_horizontal)
                fov_value = atanf(tanf(fov_value * 0.5f) / aspect) * 2.0f;
            return fov_value;
        }
        else {
            float_t half_cam_aper_h_mm = (camera_aperture_h * 25.4f) * 0.5f;
            float_t focal_lenth_value = focal_length.get(in_frame);
            return atanf(half_cam_aper_h_mm / focal_lenth_value) * 2.0f;
        }
    }

    // Added
    void CameraViewPoint::store(const SceneFile& in_file, a3d::CameraViewPoint& out_cvp) const {
        model_transform.store(in_file, out_cvp.model_transform);
        roll.store(in_file, out_cvp.roll);

        if (has_fov) {
            out_cvp.has_fov = true;
            fov.store(in_file, out_cvp.fov);
            out_cvp.fov_is_horizontal = fov_is_horizontal;
            out_cvp.aspect = aspect;
        }
        else {
            focal_length.store(in_file, out_cvp.focal_length);
            out_cvp.camera_aperture_w = camera_aperture_w;
            out_cvp.camera_aperture_h = camera_aperture_h;
        }
    }

    // 0x1401BB3F0
    CameraViewPoint& CameraViewPoint::operator=(const CameraViewPoint& other) {
        model_transform = other.model_transform;
        roll = other.roll;
        has_fov = other.has_fov;
        fov_is_horizontal = other.fov_is_horizontal;
        fov = other.fov;
        aspect = other.aspect;
        focal_length = other.focal_length;
        camera_aperture_w = other.camera_aperture_w;
        camera_aperture_h = other.camera_aperture_h;
        return *this;
    }

    // 0x1401B2A60
    CameraRoot::CameraRoot() : model_transform(), interest(), view_point() {

    }

    // 0x1401B2A00
    CameraRoot::CameraRoot(const CameraRoot& other) : model_transform(other.model_transform),
        interest(other.interest), view_point(other.view_point) {

    }

    // 0x1401B8240
    CameraRoot::~CameraRoot() {

    }

    // Inlined
    inline void CameraRoot::init() {
        model_transform.init();
        interest.init();
        view_point.init();
    }

    // 0x1401D3C70
    CameraParam CameraRoot::get(float_t in_frame, const Scene& in_scene, render_context* rctx) {
        float_t frame = in_frame;
        Fcurve3f& vp_translation = view_point.model_transform.fcurve_translation;
        Fcurve3f& intr_translation = interest.model_transform.fcurve_translation;
        if (vp_translation.x.check_threshold_discontinuity(frame)
            || vp_translation.y.check_threshold_discontinuity(frame)
            || vp_translation.z.check_threshold_discontinuity(frame)
            || intr_translation.x.check_threshold_discontinuity(frame)
            || intr_translation.y.check_threshold_discontinuity(frame)
            || intr_translation.z.check_threshold_discontinuity(frame)) {
            frame = prj::floorf(frame);
            rctx->camera->set_fast_change(true);
        }
        else {
            float_t frame_prev = frame - get_delta_frame();
            if (frame_prev > 0.0f && (int32_t)frame - (int32_t)frame_prev == 2) {
                frame_prev = prj::ceilf(frame_prev);
                if (vp_translation.x.check_threshold_discontinuity(frame_prev)
                    || vp_translation.y.check_threshold_discontinuity(frame_prev)
                    || vp_translation.z.check_threshold_discontinuity(frame_prev)
                    || intr_translation.x.check_threshold_discontinuity(frame_prev)
                    || intr_translation.y.check_threshold_discontinuity(frame_prev)
                    || intr_translation.z.check_threshold_discontinuity(frame_prev))
                    rctx->camera->set_fast_change_hist0(true);
            }
        }

        CameraParam cam;

        model_transform.get(frame);
        model_transform.mul(mat4_identity);

        const mat4& cr_mat = model_transform.get_matrix();

        vec3 _view_point = vp_translation.get(frame);
        mat4_transform_point(&cr_mat, &_view_point, &cam.view_point);

        vec3 _interest = intr_translation.get(frame);
        mat4_transform_point(&cr_mat, &_interest, &cam.interest);

        cam.v_fov = view_point.get_v_fov(frame) * in_scene.M_fov_adjust;
        cam.roll = view_point.roll.get(frame);
        return cam;
    }

    // 0x1401D9CA0
    bool CameraRoot::load(const SceneFile& in_file, const a3d::CameraRoot& in_cr) {
        model_transform.load(in_file, in_cr.model_transform);
        interest.load(in_file, in_cr.interest);
        view_point.load(in_file, in_cr.view_point);
        return true;
    }

    // Added
    void CameraRoot::store(const SceneFile& in_file, a3d::CameraRoot& out_cr) const {
        model_transform.store(in_file, out_cr.model_transform);
        interest.store(in_file, out_cr.interest);
        view_point.store(in_file, out_cr.view_point);
    }

    // 0x1401BB3A0
    CameraRoot& CameraRoot::operator=(const CameraRoot& other) {
        model_transform = other.model_transform;
        interest = other.interest;
        view_point = other.view_point;
        return *this;
    }

    // 0x1401B3000
    Chara::Chara() : name(), model_transform(), rob_id(ROB_ID_NULL) {

    }

    // 0x1401B2F90
    Chara::Chara(const Chara& other)
        : name(other.name), model_transform(other.model_transform), rob_id(other.rob_id) {

    }

    // 0x1401B8520
    Chara::~Chara() {

    }

    // 0x1401D4060
    void Chara::get(float_t in_frame) {
        model_transform.get(in_frame);
    }

    // 0x1401CE470
    void Chara::ctrl(Scene& in_scene) {
        mat4 base_matrix = in_scene.get_base_matrix();

        vec3 pos = model_transform.translation;
        if (in_scene.get_reverse_side())
            pos.x = -pos.x;
        mat4_transform_point(&base_matrix, &pos, &pos);

        RobAngle mt_yang;
        mt_yang.set_rad(model_transform.rotation.y);
        if (in_scene.get_reverse_side())
            mt_yang = -mt_yang;

        RobAngle base_yang;
        base_yang.set_rad(atan2f(base_matrix.row2.x, base_matrix.row2.z));
        RobAngle yang = base_yang + mt_yang;
        if (rob_id >= 0 && rob_id < ROB_ID_MAX) {
            RobManagement* rob_man = get_rob_management();
            rob_man->set_rob_pos((ROB_ID)rob_id, pos);
            rob_man->set_rob_yang((ROB_ID)rob_id, yang);
        }
    }

    // 0x1401D0380
    void Chara::disp(const Scene& in_scene, const mat4& in_base_mat, render_context* rctx) {
        if (rob_id >= 0 && rob_id < ROB_ID_MAX)
            get_rob_management()->set_disp_on((ROB_ID)rob_id, model_transform.visibility);

        if (in_scene.M_debug) {
            model_transform.mul(in_scene.base_matrix);
            debug_put_line_axis(model_transform.get_matrix());
        }
    }

    // 0x1401E63E0
    void Chara::set_disp_freeze(bool in_value) const {
        if (rob_id >= 0 && rob_id < ROB_ID_MAX)
            get_rob_management()->set_disp_freeze((ROB_ID)rob_id, in_value);
    }

    // 0x1401D8A70
    bool Chara::is_bind() const {
        return rob_id >= 0 && rob_id < ROB_ID_MAX;
    }

    // 0x1401DA100
    bool Chara::load(const SceneFile& in_file, const a3d::Chara& in_c) {
        name.assign(in_c.name);
        model_transform.load(in_file, in_c.model_transform);
        return true;
    }

    // Added
    void Chara::store(const SceneFile& in_file, a3d::Chara& out_c) const {
        out_c.name.assign(name);
        model_transform.store(in_file, out_c.model_transform);
    }

    // 0x1401BB6F0
    Chara& Chara::operator=(const Chara& other) {
        name.assign(other.name);
        model_transform = other.model_transform;
        rob_id = other.rob_id;
        return *this;
    }

    // 0x1401B3180
    Curve::Curve() : curve(), name(), value() {

    }

    // 0x1401B30E0
    Curve::Curve(const Curve& other) : curve(other.curve), name(other.name), value(other.value) {

    }

    // 0x1401B85B0
    Curve::~Curve() {

    }

    // 0x1401D4140
    void Curve::get(float_t in_frame) {
        value = curve.get(in_frame);
    }

    // Inlined
    inline Fcurve* Curve::M_get_fcurve() {
        return &curve;
    }

    // 0x1401DA230
    bool Curve::load(const SceneFile& in_file, const a3d::Curve& in_c) {
        curve.load(in_file, in_c.fcurve);
        name.assign(in_c.name);
        return true;
    }

    // Added
    void Curve::store(const SceneFile& in_file, a3d::Curve& out_c) const {
        curve.store(in_file, out_c.fcurve);
        out_c.name.assign(name);
    }

    // 0x1401BB7D0
    Curve& Curve::operator=(const Curve& other) {
        curve = other.curve;
        name.assign(other.name);
        value = other.value;
        return *this;
    }

    // 0x1401B3220
    CurveWithOffset::CurveWithOffset() : fcurve_ptr(), offset(), value() {
        init();
    }

    // 0x1401B31C0
    CurveWithOffset::CurveWithOffset(const CurveWithOffset& other)
        : fcurve_ptr(other.fcurve_ptr), offset(other.offset), value(other.value) {

    }

    // 0x1401B8610
    CurveWithOffset::~CurveWithOffset() {

    }

    // 0x1401D6CD0
    void CurveWithOffset::init() {
        fcurve_ptr = 0;
        name.clear();
        name.shrink_to_fit();
        offset = 0.0f;
        value = 0.0f;
    }

    // Inlined
    inline void CurveWithOffset::bind(Curve& in_c) {
        fcurve_ptr = &in_c;
    }

    // 0x1401D4160
    void CurveWithOffset::get(float_t in_frame) {
        if (!fcurve_ptr)
            return;

        float_t frame = in_frame;
        if (offset != 0.0f) {
            frame += offset;
            if (frame >= fcurve_ptr->curve.m_max_frame)
                frame -= fcurve_ptr->curve.m_max_frame;
        }
        value = fcurve_ptr->curve.get(frame);
    }

    // Inlined
    inline bool CurveWithOffset::is_valid() const {
        return fcurve_ptr != 0;
    }

    // 0x1401BB860
    CurveWithOffset& CurveWithOffset::operator=(const CurveWithOffset& other) {
        fcurve_ptr = other.fcurve_ptr;
        name.assign(other.name);
        offset = other.offset;
        value = other.value;
        return *this;
    }

    // 0x1401B32C0
    Dof::Dof() : has_dof() {
        init();
    }

    // Missing
    Dof::Dof(const Dof& other) : has_dof(other.has_dof), model_transform(other.model_transform) {

    }

    // 0x1401B86D0
    Dof::~Dof() {

    }

    // 0x1401D6CF0
    void Dof::init() {
        has_dof = false;
        model_transform.init();
    }

    // 0x1401D41A0
    void Dof::get(float_t in_frame) {
        if (has_dof)
            model_transform.get(in_frame);
    }

    // 0x1401CE5F0
    void Dof::ctrl(render_context* rctx) {
        if (!has_dof)
            return;

        vec3 view_point;
        rctx->camera->get_view_point(view_point);

        float_t focus = vec3::distance(model_transform.translation, view_point);

        rctx->render.set_dof_enable(fabsf(model_transform.rotation.z) > 0.000001f);
        rctx->render.set_dof_data(focus, model_transform.scale.x,
            model_transform.rotation.x, model_transform.rotation.y);
    }

    // 0x1401CFE70
    void Dof::dest(render_context* rctx) {
        if (has_dof)
            rctx->render.set_dof_enable(false);
        has_dof = false;
        init();
    }

    // 0x1401DAFA0
    bool Dof::load(const SceneFile& in_file, const a3d::Dof& in_d) {
        if (in_d.has_dof) {
            model_transform.load(in_file, in_d.model_transform);
            has_dof = true;
        }
        return true;
    }

    // Added
    void Dof::store(const SceneFile& in_file, a3d::Dof& out_d) const {
        if (has_dof) {
            model_transform.store(in_file, out_d.model_transform);
            out_d.has_dof = true;
        }
    }

    // Missing
    Dof& Dof::operator=(const Dof& other) {
        has_dof = other.has_dof;
        model_transform = other.model_transform;
        return *this;
    }

    // 0x1401B32E0
    Event::Event(const a3d::Event& in_e) {
        active = false;
        switch (in_e.type) {
        case a3d::EVENT_MISC:
        default:
            type = EVENT_MISC;
            break;
        case a3d::EVENT_FLT:
            type = EVENT_FLT;
            break;
        case a3d::EVENT_FX:
            type = EVENT_FX;
            break;
        case a3d::EVENT_SND:
            type = EVENT_SND;
            break;
        case a3d::EVENT_MOT:
            type = EVENT_MOT;
            break;
        case a3d::EVENT_A2D:
            type = EVENT_A2D;
            break;
        }
        name.assign(in_e.name);
        index = 0;
        begin = in_e.begin;
        end = in_e.end;
        param1.assign(in_e.param1);
        ref.assign(in_e.ref);
        ref_index = -1;
    }

    // 0x1401B8730
    Event::~Event() {

    }

    // 0x1401E5CD0
    void Event::resolve_relation(const Scene& in_scene) {

    }

    // 0x1401D1E80
    void Event::entry(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) {

    }

    // 0x1401D3880
    void Event::exit(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) {

    }

    // 0x1401D14F0
    void Event::do_ctrl(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) {

    }

    // 0x1401D1750
    void Event::do_disp(const Scene& in_scene, const mat4& in_mat, render_context* rctx) {

    }

    // Added
    void Event::store(const SceneFile& in_file, a3d::Event& out_e) const {

    }

    // 0x1401B3710
    EventA2d::EventA2d(const a3d::Event& in_e) : Event(in_e) {

    }

    // 0x1401B87C0
    EventA2d::~EventA2d() {

    }

    // 0x1401D1E90
    void EventA2d::entry(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) {
        float_t frame = in_scene.get_frame();
        if (frame >= begin && frame < end)
            M_a2d_handle.restart();
    }

    // 0x1401D3890
    void EventA2d::exit(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) {

    }

    // 0x1401D1500
    void EventA2d::do_ctrl(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) {

    }

    // 0x1401D1760
    void EventA2d::do_disp(const Scene& in_scene, const mat4& in_mat, render_context* rctx) {

    }

    // 0x1401B3860
    EventFX::EventFX(const a3d::Event& in_e) : Event(in_e) {

    }

    // 0x1401B87F0
    EventFX::~EventFX() {

    }

    // 0x1401E5CE0
    void EventFX::resolve_relation(const Scene& in_scene) {
        if (param1.compare("DOWN") && param1.compare("KABEHIT1") && param1.compare("PAPER1"))
            return;

        ref_index = in_scene.get_point_index(ref);;
    }

    // 0x1401D1EF0
    void EventFX::entry(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) {
        event_log("  [FX]: %s\n", param1.c_str());
    }

    // 0x1401D1510
    void EventFX::do_ctrl(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) {
        if (ref_index >= in_scene.point_list.size() || in_scene.M_frame < begin
            || in_scene.M_frame >= end || param1.compare("PAPER1"))
            return;

        mat4 base_matrix = in_scene.get_base_matrix();

        const Point& point = in_scene.point_list[ref_index];
        vec3 pos = point.model_transform.translation;
        if (in_scene.get_reverse_side())
            pos.x = -pos.x;
        mat4_transform_point(&base_matrix, &pos, &pos);

        particle_event_data event;
        event.type = 1.0f;
        event.count = point.model_transform.scale.x * 100.0f;
        event.size = point.model_transform.scale.z;
        event.pos = pos;
        event.force = point.model_transform.scale.y;
        effect_manager_event(EFFECT_PARTICLE, 1, &event);
    }

    // 0x1401B3890
    EventFXSmoothCut::EventFXSmoothCut(const a3d::Event& in_e) : Event(in_e), is_camera() {

    }

    // 0x1401B8800
    EventFXSmoothCut::~EventFXSmoothCut() {

    }

    // 0x1401E5E10
    void EventFXSmoothCut::resolve_relation(const Scene& in_scene) {
        if (!ref.compare("CAM")) {
            is_camera = true;
            return;
        }

        ref_index = in_scene.get_chara_index(ref);
    }

    // 0x1401D1F10
    void EventFXSmoothCut::entry(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) {
        if (is_camera || !in_scene.chara_list.size() || ref_index >= in_scene.chara_list.size())
            return;

        int32_t rob_id = in_scene.chara_list[ref_index].rob_id;
        if (rob_id >= 0 && rob_id < ROB_ID_MAX)
            get_rob_management()->reset_osage_pos((ROB_ID)rob_id);
    }

    // 0x1401D38A0
    void EventFXSmoothCut::exit(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) {

    }

    // 0x1401D1770
    void EventFXSmoothCut::do_disp(const Scene& in_scene, const mat4& in_mat, render_context* rctx) {

    }

    // 0x1401B38C0
    EventFilterFade::EventFilterFade(const a3d::Event& in_e) : Event(in_e) {
        fade_type = Type::FADE_NONE;
    }

    // 0x1401B8810
    EventFilterFade::~EventFilterFade() {

    }

    // 0x1401D1F90
    void EventFilterFade::entry(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) {

    }

    // 0x1401D38B0
    void EventFilterFade::exit(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) {
        switch (fade_type) {
        case EventFilterFade::Type::FADE_OUT:
        case EventFilterFade::Type::FADE_IN:
            break;
        default:
            return;
        }

        rctx->render.reset_scene_fade(0);
    }

    // 0x1401D1780
    void EventFilterFade::do_disp(const Scene& in_scene, const mat4& in_mat, render_context* rctx) {
        switch (fade_type) {
        case EventFilterFade::Type::FADE_OUT:
        case EventFilterFade::Type::FADE_IN:
            break;
        default:
            return;
        }

        float_t t = (in_scene.M_frame - begin) / (end - begin);

        vec4 value;
        *(vec3*)&value = fade_color;
        if (fade_type == EventFilterFade::Type::FADE_IN)
            value.w = t;
        else
            value.w = 1.0f - t;

        rctx->render.set_scene_fade(value, 0);
    }

    // 0x1401B3AD0
    EventFilterTimeStop::EventFilterTimeStop(const a3d::Event& in_e) : Event(in_e) {

    }

    // 0x1401B8820
    EventFilterTimeStop::~EventFilterTimeStop() {

    }

    // 0x1401D1FA0
    void EventFilterTimeStop::entry(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) {
        for (const Chara& i : in_scene.chara_list)
            i.set_disp_freeze(true);
        effect_manager_set_frame_rate_control(&frame_rate_time_stop);
    }

    // 0x1401D38E0
    void EventFilterTimeStop::exit(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) {
        for (const Chara& i : in_scene.chara_list)
            i.set_disp_freeze(false);
        effect_manager_set_frame_rate_control(get_sys_frame_rate());
    }

    // 0x1401D1830
    void EventFilterTimeStop::do_disp(const Scene& in_scene, const mat4& in_mat, render_context* rctx) {

    }

    // 0x1401B3B00
    EventMot::EventMot(const a3d::Event& in_e) : Event(in_e) {

    }

    // 0x1401B8830
    EventMot::~EventMot() {

    }

    // 0x1401E5EC0
    void EventMot::resolve_relation(const Scene& in_scene) {
        ref_index = in_scene.get_chara_index(ref);
    }

    // 0x1401D1FF0
    void EventMot::entry(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) {
        if (ref_index < 0 || ref_index >= in_scene.chara_list.size())
            return;

        const Chara& chara = in_scene.chara_list[ref_index];
        if (chara.rob_id < 0 || chara.rob_id >= ROB_ID_MAX)
            return;

        data_struct* aft_data = &data_list[DATA_AFT];
        bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        mat4 base_matrix = in_scene.get_base_matrix();

        vec3 pos = chara.model_transform.translation;
        if (in_scene.get_reverse_side())
            pos.x = -pos.x;
        mat4_transform_point(&base_matrix, &pos, &pos);

        RobAngle mt_yang;
        mt_yang.set_rad(chara.model_transform.rotation.y);
        if (in_scene.get_reverse_side())
            mt_yang = -mt_yang;

        RobAngle base_yang;
        base_yang.set_rad(-atan2f(base_matrix.row2.x, base_matrix.row2.z));
        RobAngle yang = base_yang + mt_yang;

        RobInit rob_init;
        rob_init.rob_type = ROB_TYPE_AUTH;
        rob_init.mirror = in_scene.get_reverse_side();
        rob_init.pos = pos;
        rob_init.yang = yang;
        rob_init.energy = 200;

        ROB_ID rob_id = (ROB_ID)chara.rob_id;
        RobManagement* rob_man = get_rob_management();
        rob_man->reset_rob(rob_id, rob_init, aft_bone_data, aft_mot_db);
        rob_man->set_disp_on(rob_id, true);

        uint32_t motnum = aft_mot_db->get_motion_id(param1.c_str());
        if (motnum != -1 && motion_storage_check_mot_file_not_ready(
            aft_mot_db->get_motion_set_id_by_motion_id(motnum)))
            rob_man->set_motion(rob_id, motnum);
    }

    // 0x1401D3940
    void EventMot::exit(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) {

    }

    // 0x1401D16F0
    void EventMot::do_ctrl(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) {
        float_t frame = in_scene.get_frame();
        if (begin <= frame && frame < end) {
            int32_t rob_id = in_scene.chara_list[ref_index].rob_id;
            get_rob_management()->set_mot_frame((ROB_ID)rob_id, frame);
        }
    }

    // 0x1401D1840
    void EventMot::do_disp(const Scene& in_scene, const mat4& in_mat, render_context* rctx) {

    }

    // 0x1401B3B30
    EventSnd::EventSnd(const a3d::Event& in_e) : Event(in_e) {
        is_pseudo = param1.find("zz", 0, 2) == 0;
    }

    // 0x1401B8840
    EventSnd::~EventSnd() {

    }

    // 0x1401E5EF0
    void EventSnd::resolve_relation(const Scene& in_scene) {

    }

    // 0x1401B8840
    void EventSnd::entry(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) {
        float_t frame = in_scene.M_frame;
        if (begin > frame || frame >= end || ((in_eaf & EAF_JUMP_BACKWARD) && fabsf(frame - begin) > 0.1f))
            return;

        ROB_ID rob_id = ROB_ID_NULL;
        if (in_scene.chara_list.size())
            rob_id = (ROB_ID)in_scene.chara_list.front().rob_id;

        if (rob_id >= 0 && rob_id < ROB_ID_MAX)
            get_rob_management()->play_auth3d_sound(rob_id, param1.c_str());
        else if (sound_work_has_property(param1.c_str()))
            sound_work_play_se(1, param1.c_str());
    }

    // 0x1401D3950
    void EventSnd::exit(const Scene& in_scene, EventActionFlag in_eaf, render_context* rctx) {

    }

    // 0x1401D1850
    void EventSnd::do_disp(const Scene& in_scene, const mat4& in_mat, render_context* rctx) {

    }

    // 0x1401B4470
    Fog::Fog() : flag(), backup_flag(), id(), name(), fcurve_density(), fcurve_start(), fcurve_end(),
        density(), start(), end(), color(), backup_density(), backup_start(), backup_end(), backup_color() {

    }

    // 0x1401B41C0
    Fog::Fog(const Fog& other) : flag(other.flag), backup_flag(other.backup_flag), id(other.id), name(other.name),
        fcurve_density(other.fcurve_density), fcurve_start(other.fcurve_start), fcurve_end(other.fcurve_end),
        density(other.density), start(other.start), end(other.end), color(other.color),
        backup_density(other.backup_density), backup_start(other.backup_start),
        backup_end(other.backup_end), backup_color(other.backup_color) {

    }

    // 0x1401B89B0
    Fog::~Fog() {

    }

    // 0x1401D7150
    void Fog::init() {
        id = -1;
        flag = 0;
        backup_flag = 0;
        name.assign("(init)");

        density = 0.0f;
        start = 10.0f;
        end = 100.0f;

        fcurve_density.init();
        fcurve_start.init();
        fcurve_end.init();
    }

    // 0x1401CE730
    void Fog::ctrl(render_context* rctx) {
        if (id < 0 || id > FOG_MAX)
            return;

        fog* data = &rctx->fog[id];

        if (has_start()) {
            if (!(backup_flag & FLAG_START)) {
                backup_start = data->get_start();
                backup_flag |= FLAG_START;
            }

            data->set_start(start);
        }

        if (has_end()) {
            if (!(backup_flag & FLAG_END)) {
                backup_end = data->get_end();
                backup_flag |= FLAG_END;
            }

            data->set_end(end);
        }

        if (has_density()) {
            if (!(backup_flag & FLAG_DENSITY)) {
                backup_density = data->get_density();
                backup_flag |= FLAG_DENSITY;
            }

            data->set_density(density);
        }

        if (has_color()) {
            if (!(backup_flag & FLAG_COLOR)) {
                vec4 color;
                data->get_color(color);
                backup_color = color;
                backup_flag |= FLAG_COLOR;
            }

            data->set_color(color.color);
        }
    }

    // 0x1401CFEB0
    void Fog::dest(render_context* rctx) {
        if (id < 0 || id >= FOG_MAX)
            return;

        fog* data = &rctx->fog[id];
        if (backup_flag & FLAG_DENSITY)
            data->set_density(backup_density);
        if (backup_flag & FLAG_START)
            data->set_start(backup_start);
        if (backup_flag & FLAG_END)
            data->set_end(backup_end);
        if (backup_flag & FLAG_COLOR)
            data->set_color(backup_color);

        init();
    }

    // 0x1401D4500
    void Fog::get(float_t in_frame) {
        if (has_density())
            density = fcurve_density.get(in_frame);
        if (has_start())
            start = fcurve_start.get(in_frame);
        if (has_end())
            end = fcurve_end.get(in_frame);
        if (has_color())
            color.get(in_frame);
    }

    // 0x1401DBB20
    bool Fog::load(const SceneFile& in_file, const a3d::Fog& in_f) {
        id = in_f.id;
        name.assign(in_f.name);

        if (in_f.flag & a3d::Fog::FLAG_DENSITY) {
            fcurve_density.load(in_file, in_f.density);
            flag |= FLAG_DENSITY;
        }

        if (in_f.flag & a3d::Fog::FLAG_START) {
            fcurve_start.load(in_file, in_f.start);
            flag |= FLAG_START;
        }

        if (in_f.flag & a3d::Fog::FLAG_END) {
            fcurve_end.load(in_file, in_f.end);
            flag |= FLAG_END;
        }

        if (in_f.flag & a3d::Fog::FLAG_COLOR) {
            color.load(in_file, in_f.color);
            flag |= FLAG_COLOR;
        }
        return true;
    }

    // Added
    void Fog::store(const SceneFile& in_file, a3d::Fog& out_f) const {
        out_f.id = id;
        out_f.name.assign(name);

        if (flag & FLAG_DENSITY) {
            fcurve_density.store(in_file, out_f.density);
            out_f.flag |= a3d::Fog::FLAG_DENSITY;
        }

        if (flag & FLAG_START) {
            fcurve_start.store(in_file, out_f.start);
            out_f.flag |= a3d::Fog::FLAG_START;
        }

        if (flag & FLAG_END) {
            fcurve_end.store(in_file, out_f.end);
            out_f.flag |= a3d::Fog::FLAG_END;
        }

        if (flag & FLAG_COLOR) {
            color.store(in_file, out_f.color);
            out_f.flag |= a3d::Fog::FLAG_COLOR;
        }
    }

    // 0x1401BBD30
    Fog& Fog::operator=(const Fog& other) {
        flag = other.flag;
        backup_flag = other.backup_flag;
        id = other.id;
        name.assign(other.name);

        fcurve_density = other.fcurve_density;
        fcurve_start = other.fcurve_start;
        fcurve_end = other.fcurve_end;

        density = other.density;
        start = other.start;
        end = other.end;

        color = other.color;

        backup_density = other.backup_density;
        backup_start = other.backup_start;
        backup_end = other.backup_end;
        backup_color = other.backup_color;
        return *this;
    }

    // 0x1401B4730
    HierarchyObject::HierarchyObject() : node_list(), mats(), ex_node_mat(), name(), uid_name(),
        obj_uid(), obj_hash(), is_reflect(), is_refract(), has_shadow(), parent_name(), parent_node(),
        child_object_list(), child_object_list_parent_node(),
        child_object_hrc_list(), child_object_hrc_list_parent_node() {

    }

    // 0x1401B45D0
    HierarchyObject::HierarchyObject(const HierarchyObject& other) : node_list(other.node_list), mats(other.mats),
        ex_node_mat(other.ex_node_mat), name(other.name), uid_name(other.uid_name), obj_uid(other.obj_uid),
        obj_hash(other.obj_hash), is_reflect(other.is_reflect), is_refract(other.is_refract),
        has_shadow(other.has_shadow), parent_name(other.parent_name), parent_node(other.parent_node),
        child_object_list(other.child_object_list),
        child_object_list_parent_node(other.child_object_list_parent_node),
        child_object_hrc_list(other.child_object_hrc_list),
        child_object_hrc_list_parent_node(other.child_object_hrc_list_parent_node) {

    }

    // 0x1401B8A80
    HierarchyObject::~HierarchyObject() {

    }

    // 0x1401D72A0
    void HierarchyObject::init() {
        name.clear();
        name.shrink_to_fit();
        uid_name.clear();
        uid_name.shrink_to_fit();

        obj_uid = {};
        obj_hash = (uint32_t)-1;
        is_reflect = false;
        is_refract = false;
        has_shadow = false;

        node_list.clear();
        node_list.shrink_to_fit();
        mats.clear();
        mats.shrink_to_fit();
        parent_name.clear();
        parent_name.shrink_to_fit();
        parent_node.clear();
        parent_node.shrink_to_fit();

        child_object_list.clear();
        child_object_list.shrink_to_fit();
        child_object_list_parent_node.clear();
        child_object_list_parent_node.shrink_to_fit();

        child_object_hrc_list.clear();
        child_object_hrc_list.shrink_to_fit();
        child_object_hrc_list_parent_node.clear();
        child_object_hrc_list_parent_node.shrink_to_fit();
    }

    // 0x1401D45A0
    void HierarchyObject::get(float_t in_frame) {
        for (TransformNode& i : node_list)
            i.get(in_frame);
    }

    // 0x1401CE860
    void HierarchyObject::ctrl(const mat4& in_base_mat) {
        //if (!node.front().motion_transform.visibility)
        //    return;

        calc_matrix_hierarchy(in_base_mat);

        TransformNode* node = node_list.data();

        for (size_t i = 0; i < child_object_list.size(); i++)
            child_object_list[i]->ctrl(node[child_object_list_parent_node[i]].motion_transform.matrix);

        for (size_t i = 0; i < child_object_hrc_list.size(); i++)
            child_object_hrc_list[i]->ctrl(
                node[child_object_hrc_list_parent_node[i]].motion_transform.matrix);
    }

    // 0x1401D04A0
    void HierarchyObject::disp(Scene& in_scene, render_context* rctx) {
        if (!in_scene.M_is_visible || !node_list.front().motion_transform.visibility)
            return;

        mdl::DispManager& disp_manager = *rctx->disp_manager;
        const object_database* obj_db = in_scene.obj_db;
        const texture_database* tex_db = in_scene.tex_db;

        mdl::ObjFlags flag = mdl::OBJ_SSS;
        if (in_scene.M_shadow | has_shadow)
            enum_or(flag, mdl::OBJ_4 | mdl::OBJ_SHADOW);
        if (is_reflect)
            enum_or(flag, mdl::OBJ_NO_TRANSLUCENCY | mdl::OBJ_REFLECT);
        if (is_refract)
            enum_or(flag, mdl::OBJ_NO_TRANSLUCENCY | mdl::OBJ_REFRACT);
        if (in_scene.trnsl < 1.0f)
            enum_or(flag, in_scene.trnsl_state);

        disp_manager.set_obj_flags(flag);
        disp_manager.set_shadow_group(SHADOW_GROUP_CHARA);

        mat4 mat = mat4_identity;
        if (in_scene.M_assign_rob_id >= 0 && in_scene.M_assign_rob_id < ROB_ID_MAX) {
            RobManagement* rob_man = get_rob_management();
            if (rob_man->is_init((ROB_ID)in_scene.M_assign_rob_id)) {
                rob_chara* rob_chr = rob_man->get_rob((ROB_ID)in_scene.M_assign_rob_id);
                mat4_mul(&rob_chr->rob_base.position.rob_mat,
                    in_scene.M_chara_item
                    ? &rob_chr->rob_base.adjust.item_mat
                    : &rob_chr->rob_base.adjust.mat, &mat);
                if (in_scene.M_assign_rob_id)
                    disp_manager.set_shadow_group(SHADOW_GROUP_STAGE);
            }
        }
        else if (flag & mdl::OBJ_SHADOW) {
            disp_manager.set_shadow_group(SHADOW_GROUP_STAGE);

            const mat4& root_matrix = get_root_matrix();

            Shadow* shad = shadow_ptr_get();
            if (shad) {
                vec3 pos;
                mat4_get_translation(&root_matrix, &pos);
                pos.y -= 0.2f;
                shad->positions[SHADOW_GROUP_STAGE].push_back(pos);
            }
        }

        if (mats.size())
            disp_manager.entry_obj_by_object_info_object_skin(
                obj_uid, 0, 0, in_scene.trnsl, mats.data(), 0, 0, mat);

        disp_manager.set_obj_flags();
        disp_manager.set_shadow_group(SHADOW_GROUP_CHARA);

        for (Object*& i : child_object_list)
            i->disp(in_scene, rctx);

        for (HierarchyObject*& i : child_object_hrc_list)
            i->disp(in_scene, rctx);
    }

    void HierarchyObject::add_child_object(int32_t in_index, Object* in_obj) {
        child_object_list_parent_node.push_back(in_index);
        child_object_list.push_back(in_obj);
    }

    void HierarchyObject::add_child_object(int32_t in_index, HierarchyObject * in_obj) {
        child_object_hrc_list_parent_node.push_back(in_index);
        child_object_hrc_list.push_back(in_obj);
    }

    // 0x1401CA530
    void HierarchyObject::assign_object_skin() {
        const obj_skin* skin = get_object_skin(obj_uid);
        if (!skin) {
            std::string error_str = sprintf_s_string("%s skin data is not found", uid_name.c_str());
            throw std::runtime_error(error_str.c_str());
        }

        size_t bone_num = 0;
        for (TransformNode& i : node_list) {
            if (!skin->num_bone)
                continue;

            size_t num_bone = skin->num_bone;
            for (size_t j = 0; j < num_bone; j++)
                if (!i.name.compare(skin->bone_array[j].name)) {
                    i.node_index = skin->bone_array[j].id;
                    bone_num++;
                    break;
                }
        }

        if (!node_list.size() || !bone_num) {
            std::string error_str = sprintf_s_string("%s skin data has no bone", uid_name.c_str());
            throw std::runtime_error(error_str.c_str());
        }

        if (ex_node_mat) {
            for (TransformNode& i : node_list)
                if (i.node_index >= 0 && (i.node_index & 0x8000))
                    i.mat_ptr = &ex_node_mat[i.node_index & 0x7FFF];
        }
        else {
            if (bone_num != skin->num_bone)
                throw std::runtime_error("bone_num mismatch");

            mats.resize(bone_num);
            for (TransformNode& i : node_list)
                if (i.node_index >= 0 && !(i.node_index & 0x8000))
                    i.mat_ptr = &mats[i.node_index];
        }
    }

    // 0x1401CB190
    void HierarchyObject::calc_matrix_hierarchy(const mat4& in_base_mat) {
        for (TransformNode& i : node_list) {
            i.motion_transform.calc_matrix_local();

            mat4 mat;
            if (i.parent_id >= 0)
                mat = node_list[i.parent_id].motion_transform.matrix;
            else
                mat = in_base_mat;

            i.calc_matrix_local(mat);
        }
    }

    // 0x1401E3510
    bool HierarchyObject::replace_chara(int32_t in_cn_src, int32_t in_cn_dst, const object_database* obj_db) {
        if (in_cn_src < 0 || in_cn_src >= CN_MAX
            || in_cn_dst < 0 || in_cn_dst >= CN_MAX || in_cn_src == in_cn_dst)
            return false;

        std::string cn_src_str(get_char_id_str((CHARA_NUM)in_cn_src));
        std::string cn_dst_str(get_char_id_str((CHARA_NUM)in_cn_dst));

        std::string uid_name(this->uid_name);
        size_t pos = uid_name.find(cn_src_str);
        if (pos != -1)
            uid_name.assign(uid_name.replace(pos, cn_src_str.size(), cn_dst_str));

        object_info obj_uid = obj_db->get_object_info(uid_name.c_str());
        if (obj_uid.not_null()) {
            this->obj_uid = obj_uid;
            this->uid_name.assign(uid_name);
            return true;
        }
        return false;
    }

    // 0x1401CC0B0
    void HierarchyObject::check_rob_chara(const std::string& in_name) {
        static std::string chara_str("_CHARA");
        static std::string exchr_str("_EXCHR");

        for (TransformNode& i : node_list) {
            if (i.name.find(chara_str) == -1) {
                if (i.name.find(exchr_str.c_str()) == -1)
                    continue;

                std::string chr = i.name.substr(i.name.find(exchr_str) + exchr_str.size()) + "____";
                if (in_name.size() && chr.find(in_name) == -1 || !in_name.size())
                    i.motion_transform.init_static();
            }
            else {
                std::string chr = i.name.substr(i.name.find(chara_str) + chara_str.size());
                if (in_name.size() && chr.find(in_name) == -1)
                    i.motion_transform.init_static();
            }
        }
    }

    // 0x1401D5EB0
    const mat4& HierarchyObject::get_root_matrix() const {
        for (const TransformNode& i : node_list)
            if (i.mat_ptr)
                return *i.mat_ptr;

        return node_list.front().motion_transform.matrix;
    }

    // 0x1401D8D20
    void HierarchyObject::join(const HierarchyObject& in_other, float_t in_blend, float_t in_inv_blend) {
        if (node_list.size() != in_other.node_list.size())
            return;

        for (size_t i = 0; i < node_list.size(); i++)
            node_list[i].motion_transform.join(
                in_other.node_list[i].motion_transform, in_blend, in_inv_blend);
    }

    // 0x1401D93B0
    void HierarchyObject::join_lock(const HierarchyObject& in_other, float_t in_blend, float_t in_inv_blend) {
        if (node_list.size() != in_other.node_list.size())
            return;

        for (size_t i = 0; i < node_list.size(); i++)
            node_list[i].motion_transform.join_lock(
                in_other.node_list[i].motion_transform, in_blend, in_inv_blend);
    }

    // 0x1401E1340
    void HierarchyObject::lock_set() {
        for (TransformNode& i : node_list)
            i.motion_transform.lock_set();
    }

    // 0x1401E5F30
    int32_t HierarchyObject::query_node_index(const char* in_name) const {
        for (const TransformNode& i : node_list)
            if (!i.name.compare(in_name))
                return (int32_t)(&i - node_list.data());
        return -1;
    }

    // 0x1401E69E0
    void HierarchyObject::set_obj_uid(object_info objuid) {
        if (objuid.not_null())
            obj_uid = objuid;
        assign_object_skin();
    }

    // 0x1401DBED0
    bool HierarchyObject::load(const SceneFile& in_file,
        const a3d::ObjectHrc& in_oh, const object_database* obj_db) {
        name.assign(in_oh.name);
        uid_name.assign(in_oh.uid_name);

        obj_uid = obj_db->get_object_info(uid_name.c_str());
        obj_hash = hash_string_murmurhash(uid_name);

        static std::string reflect_str = "_REFLECT";
        is_reflect = uid_name.find(reflect_str) != -1;

        static std::string refract_str = "_REFRACT";
        is_refract = uid_name.find(refract_str) != -1;

        has_shadow = in_oh.shadow;

        list_load(node_list, in_file, in_oh.node);

        parent_name.assign(in_oh.parent_name);
        parent_node.assign(in_oh.parent_node);

        int32_t node_index = 0;
        for (TransformNode& i : node_list) {
            if (i.parent_id >= node_index)
                throw std::range_error("parent_id is not less than child");
            node_index++;
        }
        return true;
    }

    // Added
    void HierarchyObject::store(const SceneFile& in_file, a3d::ObjectHrc& out_oh) const {
        out_oh.name.assign(name);
        out_oh.uid_name.assign(uid_name);

        out_oh.shadow = has_shadow;

        list_store(node_list, in_file, out_oh.node);

        out_oh.parent_name.assign(parent_name);
        out_oh.parent_node.assign(parent_node);
    }

    // 0x1401BBF50
    HierarchyObject& HierarchyObject::operator=(const HierarchyObject& other) {
        node_list.assign(other.node_list.begin(), other.node_list.end());
        mats.assign(other.mats.begin(), other.mats.end());
        ex_node_mat = other.ex_node_mat;
        name.assign(other.name);
        uid_name.assign(other.uid_name);
        obj_uid = other.obj_uid;
        obj_hash = other.obj_hash;
        is_reflect = other.is_reflect;
        is_refract = other.is_refract;
        has_shadow = other.has_shadow;
        parent_name.assign(other.parent_name);
        parent_node.assign(other.parent_node);
        child_object_list.assign(
            other.child_object_list.begin(), other.child_object_list.end());
        child_object_list_parent_node.assign(
            other.child_object_list_parent_node.begin(), other.child_object_list_parent_node.end());
        child_object_hrc_list.assign(
            other.child_object_hrc_list.begin(), other.child_object_hrc_list.end());
        child_object_hrc_list_parent_node.assign(
            other.child_object_hrc_list_parent_node.begin(), other.child_object_hrc_list_parent_node.end());
        return *this;
    }

    // 0x1401B4B20
    Light::Light() : flag(), backup_flag(), id(), name(), type(), position(), spot_direction(),
        ambient(), diffuse(), specular(), tone_curve(), fcurve_constant(), fcurve_intensity(), fcurve_far(),
        fcurve_linear(), fcurve_quadratic(), fcurve_drop_off(), fcurve_cone_angle(), constant(), intensity(),
        far_(), linear(), quadratic(), drop_off(), cone_angle(), backup_ambient(), backup_diffuse(),
        backup_specular(), backup_tone_curve(), backup_constant(), backup_intensity(), backup_far(),
        backup_linear(), backup_quadratic(), backup_drop_off(), backup_cone_angle() {

    }

    // 0x1401B4840
    Light::Light(const Light& other) : flag(other.flag), backup_flag(other.backup_flag), id(other.id),
        name(other.name), type(other.type), position(other.position), spot_direction(other.spot_direction),
        ambient(other.ambient), diffuse(other.diffuse), specular(other.specular), tone_curve(other.tone_curve),
        fcurve_constant(other.fcurve_constant), fcurve_intensity(other.fcurve_intensity),
        fcurve_far(other.fcurve_far), fcurve_linear(other.fcurve_linear), fcurve_quadratic(other.fcurve_quadratic),
        fcurve_drop_off(other.fcurve_drop_off), fcurve_cone_angle(other.fcurve_cone_angle), constant(other.constant),
        intensity(other.intensity), far_(other.far_), linear(other.linear), quadratic(other.quadratic),
        drop_off(other.drop_off), cone_angle(other.cone_angle),
        backup_ambient(other.backup_ambient), backup_diffuse(other.backup_diffuse),
        backup_specular(other.backup_specular), backup_tone_curve(other.backup_tone_curve),
        backup_constant(other.backup_constant), backup_intensity(other.backup_intensity),
        backup_far(other.backup_far), backup_linear(other.backup_linear), backup_quadratic(other.backup_quadratic),
        backup_drop_off(other.backup_drop_off), backup_cone_angle(other.backup_cone_angle) {

    }

    // 0x1401B8C30
    Light::~Light() {

    }

    // 0x1401D7450
    void Light::init() {
        id = -1;
        flag = 0;
        backup_flag = 0;

        name.assign("(init)");
        type.assign("(init)");

        position.init();
        spot_direction.init();
    }

    // 0x1401CEA90
    void Light::ctrl(render_context* rctx) {
        light_set* set = &rctx->light_set[LIGHT_SET_MAIN];

        if (has_ambient()) {
            if (!(backup_flag & FLAG_AMBIENT)) {
                vec4 ambient;
                set->lights[id].get_ambient(ambient);
                backup_ambient = ambient;
                backup_flag |= FLAG_AMBIENT;
            }

            set->lights[id].set_ambient(ambient.color, ambient.has_color);
        }

        if (has_diffuse()) {
            if (!(backup_flag & FLAG_DIFFUSE)) {
                vec4 diffuse;
                set->lights[id].get_diffuse(diffuse);
                backup_diffuse = diffuse;
                backup_flag |= FLAG_DIFFUSE;
            }

            set->lights[id].set_diffuse(diffuse.color, diffuse.has_color);
        }

        if (has_specular()) {
            if (!(backup_flag & FLAG_SPECULAR)) {
                vec4 specular;
                set->lights[id].get_specular(specular);
                backup_specular = specular;
                backup_flag |= FLAG_SPECULAR;
            }

            set->lights[id].set_specular(specular.color, specular.has_color);
        }

        if (has_tone_curve()) {
            if (!(backup_flag & FLAG_TONE_CURVE)) {
                set->lights[id].get_tone_curve(backup_tone_curve);
                backup_flag |= FLAG_TONE_CURVE;
            }

            light_tone_curve curve;
            curve.start_point = tone_curve.color.x;
            curve.end_point = tone_curve.color.y;
            curve.coefficient = tone_curve.color.z;
            set->lights[id].set_tone_curve(curve);
        }

        if (id >= 0 && id < LIGHT_MAX) {
            light_data* data = &set->lights[id];
            data->set_position(position.translation);
            data->set_spot_direction(spot_direction.translation);
        }
    }

    // 0x1401CFF60
    void Light::dest(render_context* rctx) {
        light_set* set = &rctx->light_set[LIGHT_SET_MAIN];

        if (backup_flag & FLAG_AMBIENT)
            set->lights[id].set_ambient(backup_ambient);
        if (backup_flag & FLAG_DIFFUSE)
            set->lights[id].set_diffuse(backup_diffuse);
        if (backup_flag & FLAG_SPECULAR)
            set->lights[id].set_specular(backup_specular);
        if (backup_flag & FLAG_TONE_CURVE)
            set->lights[id].set_tone_curve(backup_tone_curve);

        init();
    }

    // 0x1401DCA70
    bool Light::load(const SceneFile& in_file, const a3d::Light& in_l) {
        id = in_l.id;
        name.assign(in_l.name);
        type.assign(in_l.type);

        position.load(in_file, in_l.position);
        spot_direction.load(in_file, in_l.spot_direction);

        if (in_l.flag & a3d::Light::FLAG_AMBIENT) {
            ambient.load(in_file, in_l.ambient);
            flag |= FLAG_AMBIENT;
        }

        if (in_l.flag & a3d::Light::FLAG_DIFFUSE) {
            diffuse.load(in_file, in_l.diffuse);
            flag |= FLAG_DIFFUSE;
        }

        if (in_l.flag & a3d::Light::FLAG_SPECULAR) {
            specular.load(in_file, in_l.specular);
            flag |= FLAG_SPECULAR;
        }

        if (in_l.flag & a3d::Light::FLAG_TONE_CURVE) {
            tone_curve.load(in_file, in_l.tone_curve);
            flag |= FLAG_TONE_CURVE;
        }

        /// XHD
        if (in_l.flag & a3d::Light::FLAG_CONSTANT) {
            fcurve_constant.load(in_file, in_l.constant);
            flag |= FLAG_CONSTANT;
        }

        /// XHD
        if (in_l.flag & a3d::Light::FLAG_INTENSITY) {
            fcurve_intensity.load(in_file, in_l.intensity);
            flag |= FLAG_INTENSITY;
        }

        /// XHD
        if (in_l.flag & a3d::Light::FLAG_FAR) {
            fcurve_far.load(in_file, in_l.far_);
            flag |= FLAG_FAR;
        }

        /// XHD
        if (in_l.flag & a3d::Light::FLAG_LINEAR) {
            fcurve_linear.load(in_file, in_l.linear);
            flag |= FLAG_LINEAR;
        }

        /// XHD
        if (in_l.flag & a3d::Light::FLAG_QUADRATIC) {
            fcurve_quadratic.load(in_file, in_l.quadratic);
            flag |= FLAG_QUADRATIC;
        }

        /// XHD
        if (in_l.flag & a3d::Light::FLAG_DROP_OFF) {
            fcurve_drop_off.load(in_file, in_l.drop_off);
            flag |= FLAG_DROP_OFF;
        }

        /// XHD
        if (in_l.flag & a3d::Light::FLAG_CONE_ANGLE) {
            fcurve_cone_angle.load(in_file, in_l.cone_angle);
            flag |= FLAG_CONE_ANGLE;
        }
        return true;
    }

    // Added
    void Light::store(const SceneFile& in_file, a3d::Light& out_l) const {
        out_l.id = id;
        out_l.name.assign(name);
        out_l.type.assign(type);

        position.store(in_file, out_l.position);
        spot_direction.store(in_file, out_l.spot_direction);

        if (flag & FLAG_AMBIENT) {
            ambient.store(in_file, out_l.ambient);
            out_l.flag |= a3d::Light::FLAG_AMBIENT;
        }

        if (flag & FLAG_DIFFUSE) {
            diffuse.store(in_file, out_l.diffuse);
            out_l.flag |= a3d::Light::FLAG_DIFFUSE;
        }

        if (flag & FLAG_SPECULAR) {
            specular.store(in_file, out_l.specular);
            out_l.flag |= a3d::Light::FLAG_SPECULAR;
        }

        if (flag & FLAG_TONE_CURVE) {
            tone_curve.store(in_file, out_l.tone_curve);
            out_l.flag |= a3d::Light::FLAG_TONE_CURVE;
        }

        /// XHD
        if (flag & FLAG_CONSTANT) {
            fcurve_constant.store(in_file, out_l.constant);
            out_l.flag |= a3d::Light::FLAG_CONSTANT;
        }

        /// XHD
        if (flag & FLAG_INTENSITY) {
            fcurve_intensity.store(in_file, out_l.intensity);
            out_l.flag |= a3d::Light::FLAG_INTENSITY;
        }

        /// XHD
        if (flag & FLAG_FAR) {
            fcurve_far.store(in_file, out_l.far_);
            out_l.flag |= a3d::Light::FLAG_FAR;
        }

        /// XHD
        if (flag & FLAG_LINEAR) {
            fcurve_linear.store(in_file, out_l.linear);
            out_l.flag |= a3d::Light::FLAG_LINEAR;
        }

        /// XHD
        if (flag & FLAG_QUADRATIC) {
            fcurve_quadratic.store(in_file, out_l.quadratic);
            out_l.flag |= a3d::Light::FLAG_QUADRATIC;
        }

        /// XHD
        if (flag & FLAG_DROP_OFF) {
            fcurve_drop_off.store(in_file, out_l.drop_off);
            out_l.flag |= a3d::Light::FLAG_DROP_OFF;
        }

        /// XHD
        if (flag & FLAG_CONE_ANGLE) {
            fcurve_cone_angle.store(in_file, out_l.cone_angle);
            out_l.flag |= a3d::Light::FLAG_CONE_ANGLE;
        }
    }

    // 0x1401D45F0
    void Light::get(float_t in_frame) {
        position.get(in_frame);
        spot_direction.get(in_frame);

        if (has_ambient())
            ambient.get(in_frame);
        if (has_diffuse())
            diffuse.get(in_frame);
        if (has_specular())
            specular.get(in_frame);
        if (has_tone_curve())
            tone_curve.get(in_frame);

        /// XHD
        if (has_constant())
            constant = fcurve_constant.get(in_frame);
        if (has_intensity())
            intensity = fcurve_intensity.get(in_frame);
        if (has_far())
            far_ = fcurve_far.get(in_frame);
        if (has_linear())
            linear = fcurve_linear.get(in_frame);
        if (has_quadratic())
            quadratic = fcurve_quadratic.get(in_frame);
        if (has_drop_off())
            drop_off = fcurve_drop_off.get(in_frame);
        if (has_cone_angle())
            cone_angle = fcurve_cone_angle.get(in_frame);
    }

    // 0x1401BC060
    Light& Light::operator=(const Light& other) {
        flag = other.flag;
        backup_flag = other.backup_flag;
        id = other.id;
        name.assign(other.name);
        type.assign(other.type);

        position = other.position;
        spot_direction = other.spot_direction;
        ambient = other.ambient;
        diffuse = other.diffuse;
        specular = other.specular;
        tone_curve = other.tone_curve;

        /// XHD
        fcurve_constant = other.fcurve_constant;
        fcurve_intensity = other.fcurve_intensity;
        fcurve_far = other.fcurve_far;
        fcurve_linear = other.fcurve_linear;
        fcurve_quadratic = other.fcurve_quadratic;
        fcurve_drop_off = other.fcurve_drop_off;
        fcurve_cone_angle = other.fcurve_cone_angle;

        /// XHD
        constant = other.constant;
        intensity = other.intensity;
        far_ = other.far_;
        linear = other.linear;
        quadratic = other.quadratic;
        drop_off = other.drop_off;
        cone_angle = other.cone_angle;

        backup_ambient = other.backup_ambient;
        backup_diffuse = other.backup_diffuse;
        backup_specular = other.backup_specular;
        backup_tone_curve = other.backup_tone_curve;

        /// XHD
        backup_constant = other.backup_constant;
        backup_intensity = other.backup_intensity;
        backup_far = other.backup_far;
        backup_linear = other.backup_linear;
        backup_quadratic = other.backup_quadratic;
        backup_drop_off = other.backup_drop_off;
        backup_cone_angle = other.backup_cone_angle;
        return *this;
    }

    // X/XHD
    MaterialList::MaterialList() : flag(), name(), hash(hash_murmurhash_empty),
        emission(), blend_color(), fcurve_glow_intensity(), glow_intensity() {

    }

    // X/XHD
    MaterialList::MaterialList(const MaterialList& other) : flag(other.flag), name(other.name),
        hash(other.hash), emission(other.emission), blend_color(other.blend_color),
        fcurve_glow_intensity(other.fcurve_glow_intensity), glow_intensity(other.glow_intensity) {

    }

    // X/XHD
    MaterialList::~MaterialList() {

    }

    // X/XHD
    void MaterialList::init() {
        flag = 0;
        name.clear();
        name.shrink_to_fit();
        hash = hash_murmurhash_empty;

        fcurve_glow_intensity.init();
    }

    // X/XHD
    void MaterialList::get(float_t in_frame) {
        if (has_emission())
            emission.get(in_frame);
        if (has_blend_color())
            blend_color.get(in_frame);
        if (has_glow_intensity())
            glow_intensity = fcurve_glow_intensity.get(in_frame);
    }

    // X/XHD
    bool MaterialList::load(const SceneFile& in_file, const a3d::MaterialList& in_ml) {
        name.assign(in_ml.name);
        hash = hash_string_murmurhash(name);

        if (in_ml.flag & a3d::MaterialList::FLAG_EMISSION) {
            emission.load(in_file, in_ml.emission);
            flag |= FLAG_EMISSION;
        }

        if (in_ml.flag & a3d::MaterialList::FLAG_BLEND_COLOR) {
            blend_color.load(in_file, in_ml.blend_color);
            flag |= FLAG_BLEND_COLOR;
        }

        if (in_ml.flag & a3d::MaterialList::FLAG_GLOW_INTENSITY) {
            fcurve_glow_intensity.load(in_file, in_ml.glow_intensity);
            flag |= FLAG_GLOW_INTENSITY;
        }
        return true;
    }

    // X/XHD, Added
    void MaterialList::store(const SceneFile& in_file, a3d::MaterialList& out_ml) const {
        out_ml.name.assign(name);

        if (flag & FLAG_EMISSION) {
            emission.store(in_file, out_ml.emission);
            out_ml.flag |= a3d::MaterialList::FLAG_EMISSION;
        }

        if (flag & FLAG_BLEND_COLOR) {
            blend_color.store(in_file, out_ml.blend_color);
            out_ml.flag |= a3d::MaterialList::FLAG_BLEND_COLOR;
        }

        if (flag & FLAG_GLOW_INTENSITY) {
            fcurve_glow_intensity.store(in_file, out_ml.glow_intensity);
            out_ml.flag |= a3d::MaterialList::FLAG_GLOW_INTENSITY;
        }
    }

    // X/XHD
    MaterialList& MaterialList::operator=(const MaterialList & other) {
        flag = other.flag;
        name.assign(other.name);
        hash = other.hash;

        emission = other.emission;
        blend_color = other.blend_color;
        fcurve_glow_intensity = other.fcurve_glow_intensity;

        glow_intensity = other.glow_intensity;
        return *this;
    }

    // 0x1401B5100
    Motion::Motion() : name() {

    }

    // 0x1401B50C0
    Motion::Motion(const Motion& other) : name(other.name) {

    }

    // 0x1401B8DB0
    Motion::~Motion() {

    }

    // 0x1401DD2E0
    bool Motion::load(const SceneFile& in_file, const a3d::Motion& in_m) {
        name.assign(in_m.name);
        return true;
    }

    // Added
    void Motion::store(const SceneFile& in_file, a3d::Motion& out_m) const {
        out_m.name.assign(name);
    }

    // 0x1401BC3A0
    Motion& Motion::operator=(const Motion& other) {
        name.assign(other.name);
        return *this;
    }

    // 0x1401B5550
    MotionTransform::MotionTransform() : matrix(), matrix_local(), fcurve_translation(),
        fcurve_rotation(), fcurve_scale(), fcurve_visibility(), translation(0.0f), rotation(0.0f), scale(1.0f),
        visibility(false), rot(), trans_lock(0.0f), rot_lock(), scale_lock(1.0f), frame(-1.0f),
        update_rot(), matrix_local_ready(), has_scale(), has_visibility() {
        matrix = mat4_identity;
        matrix_local = mat4_identity;
        rot = mat4_identity;
        rot = mat4_identity;
    }

    // 0x1401B5120
    MotionTransform::MotionTransform(const MotionTransform& other) : matrix(other.matrix),
        matrix_local(other.matrix_local), fcurve_translation(other.fcurve_translation),
        fcurve_rotation(other.fcurve_rotation), fcurve_scale(other.fcurve_scale),
        fcurve_visibility(other.fcurve_visibility), translation(other.translation),
        rotation(other.rotation), scale(other.scale), visibility(other.visibility), rot(other.rot),
        trans_lock(other.trans_lock), rot_lock(other.rot_lock), scale_lock(other.scale_lock),
        frame(other.frame), update_rot(other.update_rot), matrix_local_ready(other.matrix_local_ready),
        has_scale(other.has_scale), has_visibility(other.has_visibility) {

    }

    // 0x1401B8DF0
    MotionTransform::~MotionTransform() {

    }

    // 0x1401D47F0
    void MotionTransform::get(float_t in_frame) {
        if (frame != in_frame || update_rot) {
            translation = fcurve_translation.get(in_frame);
            rotation = fcurve_rotation.get(in_frame);

            if (has_scale)
                scale = fcurve_scale.get(in_frame);

            if (has_visibility)
                visibility = fcurve_visibility.get(in_frame) >= 0.999f;

            mat4_rotate_zyx(&rotation, &rot);

            frame = in_frame;
            update_rot = false;
            matrix_local_ready = false;
        }

        if (matrix_local_ready && !update_rot)
            return;

    }

    // 0x1401D7550
    void MotionTransform::init() {
        matrix = mat4_identity;
        matrix_local = mat4_identity;

        fcurve_translation.init();
        fcurve_rotation.init();
        fcurve_scale.init();
        fcurve_visibility.init();

        rot = mat4_identity;

        frame = -1.0f;
        update_rot = false;
        matrix_local_ready = false;
        has_scale = false;
        has_visibility = false;
    }

    // 0x1401D8900
    void MotionTransform::init_static() {
        matrix = mat4_identity;
        matrix_local = mat4_identity;

        fcurve_translation.init(0.0f);
        fcurve_rotation.init(0.0f);
        fcurve_scale.init(1.0f);
        fcurve_visibility.init(1.0f);

        rot = mat4_identity;

        frame = -1.0f;
        update_rot = false;
        matrix_local_ready = false;
        has_scale = false;
        has_visibility = false;
    }

    // 0x1401CADD0
    void MotionTransform::calc_matrix(const mat4& in_mat) {
        mat4_mul(&matrix_local, &in_mat, &matrix);
    }

    // 0x1401CB020
    void MotionTransform::calc_matrix_local() {
        if (!matrix_local_ready || update_rot) {
            mat4_translate(&translation, &matrix_local);
            mat4_mul(&rot, &matrix_local, &matrix_local);
            if (has_scale)
                mat4_scale_rot(&matrix_local, &scale, &matrix_local);
            matrix_local_ready = true;
        }
    }

    // 0x1401CC4A0
    void MotionTransform::check_scale() {
        if (fcurve_scale.x.m_type == FC_TYPE_STATIC_DATA
            && fabsf(1.0f - fcurve_scale.x.m_static_value) <= 0.000001f
            && fcurve_scale.y.m_type == FC_TYPE_STATIC_DATA
            && fabsf(1.0f - fcurve_scale.y.m_static_value) <= 0.000001f
            && fcurve_scale.z.m_type == FC_TYPE_STATIC_DATA
            && fabsf(1.0f - fcurve_scale.z.m_static_value) <= 0.000001f) {
            has_scale = false;
            scale = 1.0f;
        }
        else
            has_scale = true;
    }

    // 0x1401CC8C0
    void MotionTransform::check_visibility() {
        if (fcurve_visibility.m_type == FC_TYPE_STATIC_DATA) {
            has_visibility = false;
            visibility = fcurve_visibility.m_static_value >= 0.999f;
        }
        else
            has_visibility = true;
    }

    // 0x1401D8E60
    void MotionTransform::join(const MotionTransform& in_mt1,
        const MotionTransform& in_mt2, float_t in_inv_blend, float_t in_blend) {
        translation = in_mt2.translation * in_blend + in_mt1.translation * in_inv_blend;
        mat4_blend_rotation(&in_mt2.rot, &in_mt1.rot, &rot, in_blend);
        if (has_scale)
            scale = in_mt2.scale * in_blend + in_mt1.scale * in_inv_blend;
        update_rot = true;
    }

    // 0x1401D9020
    void MotionTransform::join(
        const MotionTransform& in_mt1, float_t in_inv_blend, float_t in_blend) {
        translation = in_mt1.translation * in_blend + translation * in_inv_blend;
        mat4_blend_rotation(&rot, &in_mt1.rot, &rot, in_blend);
        if (has_scale)
            scale = in_mt1.scale * in_blend + scale * in_inv_blend;
        update_rot = true;
    }

    // 0x1401D94F0
    void MotionTransform::join_lock(const MotionTransform& in_mt1,
        const MotionTransform& in_mt2, float_t in_inv_blend, float_t in_blend) {
        translation = in_mt2.trans_lock * in_blend + in_mt1.translation * in_inv_blend;
        mat4_blend_rotation(&in_mt2.rot_lock, &in_mt1.rot, &rot, in_blend);
        if (has_scale)
            scale = in_mt2.scale_lock * in_blend + in_mt1.scale * in_inv_blend;
        update_rot = true;
    }

    // 0x1401D96B0
    void MotionTransform::join_lock(
        const MotionTransform& in_mt1, float_t in_inv_blend, float_t in_blend) {
        translation = in_mt1.trans_lock * in_blend + translation * in_inv_blend;
        mat4_blend_rotation(&in_mt1.rot_lock, &rot, &rot, in_blend);
        if (has_scale)
            scale = in_mt1.scale_lock * in_blend + scale * in_inv_blend;
        update_rot = true;
    }

    // 0x1401E1380
    void MotionTransform::lock_set() {
        trans_lock = translation;
        rot_lock = rot;
        if (has_scale)
            scale_lock = scale;
    }

    // 0x1401C8FF0
    void MotionTransform::add(
        const MotionTransform& in_mt1, const MotionTransform& in_mt2) {
        translation = in_mt1.translation + in_mt2.translation;
        mat4_mul(&in_mt2.rot, &in_mt1.rot, &rot);
        if (has_scale)
            scale = in_mt1.scale * in_mt2.scale;
        update_rot = true;
    }

    // 0x1401E7B00
    void MotionTransform::sub(
        const MotionTransform& in_mt1, const MotionTransform& in_mt2) {
        translation = in_mt1.translation - in_mt2.translation;
        mat4_mul(&in_mt1.rot, &in_mt2.rot, &rot);
        if (has_scale)
            scale = in_mt1.scale / in_mt2.scale;
        update_rot = true;
    }

    // 0x1401DD3B0
    bool MotionTransform::load(const SceneFile& in_file, const a3d::ModelTransform& in_mt) {
        fcurve_translation.load(in_file, in_mt.translation);
        fcurve_rotation.load(in_file, in_mt.rotation);
        fcurve_scale.load(in_file, in_mt.scale);
        fcurve_visibility.load(in_file, in_mt.visibility);

        check_scale();
        check_visibility();
        return true;
    }

    // Added
    void MotionTransform::store(const SceneFile& in_file, a3d::ModelTransform& out_mt) const {
        fcurve_translation.store(in_file, out_mt.translation);
        fcurve_rotation.store(in_file, out_mt.rotation);
        fcurve_scale.store(in_file, out_mt.scale);
        fcurve_visibility.store(in_file, out_mt.visibility);
    }

    // 0x1401BC3D0
    MotionTransform& MotionTransform::operator=(const MotionTransform& other) {
        matrix = other.matrix;
        matrix_local = other.matrix_local;

        fcurve_translation = other.fcurve_translation;
        fcurve_rotation = other.fcurve_rotation;
        fcurve_scale = other.fcurve_scale;
        fcurve_visibility = other.fcurve_visibility;

        translation = other.translation;
        rotation = other.rotation;
        scale = other.scale;
        visibility = other.visibility;

        rot = other.rot;

        trans_lock = other.trans_lock;
        rot_lock = other.rot_lock;
        scale_lock = other.scale_lock;

        frame = other.frame;
        update_rot = other.update_rot;
        matrix_local_ready = other.matrix_local_ready;
        has_scale = other.has_scale;
        has_visibility = other.has_visibility;
        return *this;
    }

    // 0x1401B5950
    MultiHierarchyObject::MultiHierarchyObject() : name(), motion_transform(), node_list(), instance_list() {

    }

    // 0x1401B58C0
    MultiHierarchyObject::MultiHierarchyObject(const MultiHierarchyObject& other) : name(other.name),
        motion_transform(other.motion_transform), node_list(other.node_list), instance_list(other.instance_list) {

    }

    // 0x1401B8E50
    MultiHierarchyObject::~MultiHierarchyObject() {

    }

    // 0x1401D7620
    void MultiHierarchyObject::init() {
        name.clear();
        name.shrink_to_fit();
        node_list.clear();
        node_list.shrink_to_fit();
        instance_list.clear();
        instance_list.shrink_to_fit();
    }

    // 0x1401D49B0
    void MultiHierarchyObject::get(float_t in_frame) {
        motion_transform.get(in_frame);
        motion_transform.calc_matrix_local();

        for (ObjectInstance& i : instance_list)
            i.get(in_frame);
        for (TransformNode& i : node_list)
            i.get(in_frame);
    }

    // 0x1401CEC40
    void MultiHierarchyObject::ctrl(const mat4& in_base_mat) {
        //if (!motion_transform.visibility)
        //    return;

        TransformNode* node = node_list.data();

        calc_matrix_hierarchy();

        motion_transform.calc_matrix(in_base_mat);

        mat4& mat = motion_transform.matrix;

        for (ObjectInstance& i : instance_list) {
            //if (!i.motion_transform.visibility)
            //    continue;

            i.motion_transform.calc_matrix(mat);
            int32_t* matrix_id = i.matrix_id.data();
            size_t matrix_id_count = i.matrix_id.size();
            mat4* mats = i.mats.data();
            for (size_t j = 0; j < matrix_id_count; j++)
                mats[j] = node[matrix_id[j]].motion_transform.matrix;
        }
    }

    // 0x1401D0760
    void MultiHierarchyObject::disp(Scene& in_scene, render_context* rctx) {
        if (!in_scene.M_is_visible || !motion_transform.visibility)
            return;

        mdl::DispManager& disp_manager = *rctx->disp_manager;

        const mat4& root_matrix = get_root_matrix();

        for (ObjectInstance& i : instance_list) {
            if (!i.motion_transform.visibility)
                continue;

            mdl::ObjFlags flag = mdl::OBJ_SSS;
            SHADOW_GROUP shadow_group = SHADOW_GROUP_CHARA;
            if (in_scene.M_shadow || i.shadow) {
                enum_or(flag, mdl::OBJ_4 | mdl::OBJ_SHADOW);
                shadow_group = SHADOW_GROUP_STAGE;
            }
            if (in_scene.trnsl < 1.0f)
                enum_or(flag, in_scene.trnsl_state);

            disp_manager.set_obj_flags(flag);
            disp_manager.set_shadow_group(shadow_group);

            Shadow* shad = shadow_ptr_get();
            if (shad && (flag & mdl::OBJ_SHADOW)) {
                disp_manager.set_shadow_group(SHADOW_GROUP_STAGE);

                mat4 mat;
                mat4_mul(&i.motion_transform.matrix, &root_matrix, &mat);

                vec3 pos;
                mat4_get_translation(&mat, &pos);
                pos.y -= 0.2f;
                shad->positions[shadow_group].push_back(pos);
            }

            if (i.mats.size())
                disp_manager.entry_obj_by_object_info_object_skin(
                    i.obj_uid, 0, 0, in_scene.trnsl, i.mats.data(), 0, 0, i.motion_transform.matrix);
        }

        disp_manager.set_obj_flags();
        disp_manager.set_shadow_group(SHADOW_GROUP_CHARA);
    }

    // 0x1401CAE40
    void MultiHierarchyObject::calc_matrix_hierarchy() {
        TransformNode* node = node_list.data();

        for (TransformNode& i : node_list)
            if (i.parent_id >= 0)
                i.motion_transform.calc_matrix(node[i.parent_id].motion_transform.matrix);
            else
                i.motion_transform.matrix = i.motion_transform.matrix_local;
    }

    // 0x1401CB0A0
    void MultiHierarchyObject::calc_matrix_local() {
        for (TransformNode& i : node_list)
            i.motion_transform.calc_matrix_local();
    }

    // 0x1401D5F20
    const mat4& MultiHierarchyObject::get_root_matrix() const {
        for (const TransformNode& i : node_list)
            if (i.mat_ptr)
                return *i.mat_ptr;

        return node_list.front().motion_transform.matrix;
    }

    // 0x1401DD710
    bool MultiHierarchyObject::load(const SceneFile& in_file,
        const a3d::MObjectHrc& in_moh, const object_database* obj_db) {
        name.assign(in_moh.name);
        motion_transform.load(in_file, in_moh.model_transform);

        list_load(node_list, in_file, in_moh.node);
        list_load(instance_list, in_file, in_moh.instance, *this, obj_db);

        int32_t node_index = 0;
        for (TransformNode& i : node_list) {
            if (i.parent_id >= node_index)
                throw std::range_error("parent_id is not less than child");
            node_index++;
        }
        return true;
    }

    // Added
    void MultiHierarchyObject::store(const SceneFile& in_file, a3d::MObjectHrc& out_moh) const {
        out_moh.name.assign(name);
        motion_transform.store(in_file, out_moh.model_transform);

        list_store(node_list, in_file, out_moh.node);
        list_store(instance_list, in_file, out_moh.instance, *this);
    }

    // 0x1401BC7F0
    MultiHierarchyObject& MultiHierarchyObject::operator=(const MultiHierarchyObject& other) {
        name.assign(other.name);
        motion_transform = other.motion_transform;
        node_list.assign(other.node_list.begin(), other.node_list.end());
        instance_list.assign(other.instance_list.begin(), other.instance_list.end());
        return *this;
    }

    // 0x1401B5BD0
    Object::Object() : model_transform(), index(), name(), uid_name(), parent_name(), parent_node(),
        obj_pat(), obj_morph(), texture_list(), texture_transform_list(), child_object_list(),
        child_object_hrc_list(), obj_uid(), obj_hash(hash_murmurhash_empty), is_reflect(), is_refract() {

    }

    // 0x1401B59C0
    Object::Object(const Object& other) : model_transform(other.model_transform), index(other.index),
        name(other.name), uid_name(other.uid_name), parent_name(other.parent_name), parent_node(other.parent_node),
        obj_pat(other.obj_pat), obj_morph(other.obj_morph), texture_list(other.texture_list),
        texture_transform_list(other.texture_transform_list), child_object_list(other.child_object_list),
        child_object_hrc_list(other.child_object_hrc_list), obj_uid(other.obj_uid),
        obj_hash(other.obj_hash), is_reflect(other.is_reflect), is_refract(other.is_refract) {

    }

    // 0x1401B8F80
    Object::~Object() {

    }

    // 0x1401D76D0
    void Object::init() {
        index = -1;

        name.clear();
        name.shrink_to_fit();
        uid_name.clear();
        uid_name.shrink_to_fit();
        parent_name.clear();
        parent_name.shrink_to_fit();
        parent_node.clear();
        parent_node.shrink_to_fit();

        obj_pat.init();
        obj_morph.init();

        texture_list.clear();
        texture_list.shrink_to_fit();
        texture_transform_list.clear();
        texture_transform_list.shrink_to_fit();
        child_object_list.clear();
        child_object_list.shrink_to_fit();
        child_object_hrc_list.clear();
        child_object_hrc_list.shrink_to_fit();

        obj_uid = {};
        obj_hash = hash_murmurhash_empty;
        is_reflect = false;
        is_refract = false;
    }

    // 0x1401D4A70
    void Object::get(float_t in_frame) {
        model_transform.get(in_frame);

        obj_morph.get(in_frame);
        obj_pat.get(in_frame);

        for (Texture& i : texture_list)
            i.get(in_frame);
        for (TextureTransform& i : texture_transform_list)
            i.get(in_frame);
    }

    // 0x1401CEE70
    void Object::ctrl(const mat4& in_base_mat) {
        //if (!model_transform.visibility)
        //    return;

        model_transform.mul(in_base_mat);

        const mat4& mat = model_transform.get_matrix();

        for (Object*& i : child_object_list)
            i->ctrl(mat);
        for (HierarchyObject*& i : child_object_hrc_list)
            i->ctrl(mat);
    }

    // 0x1401D0970
    void Object::disp(Scene& in_scene, render_context* rctx) {
        if (!model_transform.visibility)
            return;

        if (!in_scene.M_is_visible) {
            if (in_scene.M_debug)
                debug_put_line_axis(model_transform.get_matrix());

            for (Object*& i : child_object_list)
                i->disp(in_scene, rctx);
            for (HierarchyObject*& i : child_object_hrc_list)
                i->disp(in_scene, rctx);
            return;
        }

        mat4 matrix = model_transform.get_matrix();

        mdl::DispManager& disp_manager = *rctx->disp_manager;
        const object_database* obj_db = in_scene.obj_db;
        const texture_database* tex_db = in_scene.tex_db;

        if (in_scene.M_assign_rob_id >= 0 && in_scene.M_assign_rob_id < ROB_ID_MAX) {
            RobManagement* rob_man = get_rob_management();
            if (rob_man->is_init((ROB_ID)in_scene.M_assign_rob_id)) {
                rob_chara* rob_chr = rob_man->get_rob((ROB_ID)in_scene.M_assign_rob_id);
                mat4 m;
                mat4_mul(&rob_chr->rob_base.position.rob_mat,
                    in_scene.M_chara_item
                    ? &rob_chr->rob_base.adjust.item_mat
                    : &rob_chr->rob_base.adjust.mat, &m);
                mat4_mul(&matrix, &m, &matrix);
                disp_manager.set_shadow_group(in_scene.M_assign_rob_id ? SHADOW_GROUP_STAGE : SHADOW_GROUP_CHARA);
            }
        }

        mdl::ObjFlags flag = (mdl::ObjFlags)0;
        if (in_scene.M_shadow)
            enum_or(flag, mdl::OBJ_4 | mdl::OBJ_SHADOW);
        if (is_reflect)
            enum_or(flag, mdl::OBJ_NO_TRANSLUCENCY | mdl::OBJ_REFLECT);
        if (is_refract)
            enum_or(flag, mdl::OBJ_NO_TRANSLUCENCY | mdl::OBJ_REFRACT);

        disp_manager.set_obj_flags(flag);

        char buf[0x80];
        int32_t tex_pat_count = 0;
        TexChange tex_pat[TEXTURE_PATTERN_COUNT];

        for (Texture& i : texture_list) {
            if (!i.tex_pat.is_valid() || i.name.size() <= 3)
                continue;
            else if (tex_pat_count >= TEXTURE_PATTERN_COUNT)
                break;

            sprintf_s(buf, sizeof(buf), "%.*s%03d",
                (int32_t)(i.name.size() - 3), i.name.c_str(), (int32_t)prj::roundf(i.tex_pat.value));

            tex_pat[tex_pat_count].org_texid = texture_id(0x00, i.tex_uid);
            tex_pat[tex_pat_count].change_texid = texture_id(0x00, tex_db->get_texture_id(buf));
            tex_pat_count++;
        }

        if (tex_pat_count)
            disp_manager.set_texture_pattern(tex_pat_count, tex_pat);

        int32_t tex_trans_count = 0;
        texture_transform_struct tex_trans[TEXTURE_TRANSFORM_COUNT];

        for (TextureTransform& i : texture_transform_list) {
            if (tex_trans_count >= TEXTURE_TRANSFORM_COUNT || i.tex_uid == -1
                || !texture_manager_get_texture(i.tex_uid))
                continue;

            tex_trans[tex_trans_count].texid = i.tex_uid;
            tex_trans[tex_trans_count].mat = i.mat;
            tex_trans_count++;
        }

        if (tex_trans_count)
            disp_manager.set_texture_transform(tex_trans_count, tex_trans);

        if (obj_morph.is_valid()) {
            const char* uid_name = this->uid_name.c_str();
            size_t uid_name_length = this->uid_name.size();
            if (uid_name_length > 3) {
                float_t morph = obj_morph.value;
                int32_t morph_int = (int32_t)morph;
                morph = fmodf(morph, 1.0f);
                if (morph > 0.0f && morph < 1.0f) {
                    sprintf_s(buf, sizeof(buf), "%.*s%03d",
                        (int32_t)uid_name_length - 3, uid_name, morph_int + 1);
                    object_info morph_obj_uid = obj_db->get_object_info(buf);
                    if (morph_obj_uid.is_null())
                        morph_obj_uid = obj_uid;
                    disp_manager.set_morph(morph_obj_uid, morph);

                    sprintf_s(buf, sizeof(buf), "%.*s%03d",
                        (int32_t)uid_name_length - 3, uid_name, morph_int);
                    object_info obj_uid = obj_db->get_object_info(buf);
                    in_scene.M_put_obj(matrix, obj_uid, rctx);
                    disp_manager.set_morph({}, 0.0f);
                }
                else {
                    if (morph >= 1.0f)
                        morph_int++;

                    sprintf_s(buf, sizeof(buf), "%.*s%03d",
                        (int32_t)uid_name_length - 3, uid_name, morph_int);
                    object_info obj_uid = obj_db->get_object_info(buf);
                    if (obj_uid.is_null())
                        obj_uid = this->obj_uid;
                    in_scene.M_put_obj(matrix, obj_uid, rctx);
                }
            }
        }
        else if (obj_pat.is_valid()) {
            const char* uid_name = this->uid_name.c_str();
            size_t uid_name_length = this->uid_name.size();
            if (uid_name_length > 3) {
                float_t pat = prj::roundf(obj_pat.value);
                int32_t pat_int = (int32_t)pat;
                sprintf_s(buf, sizeof(buf), "%.*s%03d",
                    (int32_t)uid_name_length - 3, uid_name, pat_int);
                object_info obj_uid = obj_db->get_object_info(buf);
                in_scene.M_put_obj(matrix, obj_uid, rctx);
            }
            else
                in_scene.M_put_obj(matrix, obj_uid, rctx);
        }
        else
            in_scene.M_put_obj(matrix, obj_uid, rctx);

        disp_manager.set_texture_transform();
        disp_manager.set_texture_pattern();
        disp_manager.set_obj_flags();

        if (in_scene.M_debug)
            debug_put_line_axis(model_transform.get_matrix());

        for (Object*& i : child_object_list)
            i->disp(in_scene, rctx);
        for (HierarchyObject*& i : child_object_hrc_list)
            i->disp(in_scene, rctx);
    }

    // 0x1401E3510
    bool Object::replace_chara(int32_t in_cn_src, int32_t in_cn_dst, const object_database* obj_db) {
        if (in_cn_src < 0 || in_cn_src >= CN_MAX || in_cn_dst < 0 || in_cn_dst >= CN_MAX || in_cn_src == in_cn_dst)
            return false;

        std::string cn_src_str(get_char_id_str((CHARA_NUM)in_cn_src));
        std::string cn_dst_str(get_char_id_str((CHARA_NUM)in_cn_dst));

        std::string uid_name(this->uid_name);
        size_t pos = uid_name.find(cn_src_str);
        if (pos != -1)
            uid_name.assign(uid_name.replace(pos, cn_src_str.size(), cn_dst_str));

        object_info obj_uid = obj_db->get_object_info(uid_name.c_str());
        if (obj_uid.not_null()) {
            this->obj_uid = obj_uid;
            this->uid_name.assign(uid_name);
            return true;
        }
        return false;
    }

    // Added
    void Object::prepare_morph(const object_database* obj_db) {
        if (!obj_morph.is_valid())
            return;

        float_t min_value = FLT_MAX;
        float_t max_value = -FLT_MAX;

        float_t first_frame = 0.0f;
        float_t last_frame = obj_morph.fcurve_ptr->curve.m_max_frame;
        for (float_t i = first_frame; i <= last_frame; i += 1.0f) {
            obj_morph.get(i);
            min_value = fminf(min_value, obj_morph.value);
            max_value = fmaxf(max_value, obj_morph.value);
        }
        obj_morph.get(0.0f);

        int32_t min = (int32_t)prj::floorf(min_value);
        int32_t max = (int32_t)prj::ceilf(max_value);
        if (min >= max)
            return;

        const char* uid_name = this->uid_name.c_str();
        int32_t uid_name_length = (int32_t)this->uid_name.size();

        char buf[0x80];
        for (int32_t i = min; i < max; i++) {
            sprintf_s(buf, sizeof(buf), "%.*s%03d", uid_name_length - 3, uid_name, i + 1);
            object_info morph_obj_uid = obj_db->get_object_info(buf);
            if (morph_obj_uid.is_null())
                morph_obj_uid = this->obj_uid;

            sprintf_s(buf, sizeof(buf), "%.*s%03d", uid_name_length - 3, uid_name, i);
            object_info obj_uid = obj_db->get_object_info(buf);

            ::obj* obj = get_object_header(obj_uid);
            if (!obj)
                continue;

            VertexBuffer* vbhn_array = get_object_vertex_buffer(obj_uid);
            IndexBuffer* ibhn_array = get_object_index_buffer(obj_uid);

            VertexBuffer* morph_vbhn_array = 0;
            if (morph_obj_uid.set_id != -1)
                morph_vbhn_array = get_object_vertex_buffer(morph_obj_uid);

            for (int32_t i = 0; i < obj->num_mesh; i++) {
                const obj_mesh* mesh = &obj->mesh_array[i];
                for (int32_t j = 0; j < mesh->num_submesh; j++) {
                    const obj_sub_mesh* sub_mesh = &mesh->submesh_array[j];
                    if (sub_mesh->attrib.m.hide)
                        continue;

                    obj_material_data* material = &obj->material_array[sub_mesh->material_index];

                    GLuint morph_vb = 0;
                    uint32_t morph_vb_offset = 0;
                    if (morph_vbhn_array) {
                        morph_vb = morph_vbhn_array[i].get_glvb();
                        morph_vb_offset = morph_vbhn_array[i].get_glvb_offset();
                    }

                    GLuint ib = 0;
                    if (ibhn_array)
                        ib = ibhn_array[i].ib;

                    GLuint vb = 0;
                    uint32_t vb_offset = 0;
                    if (vbhn_array) {
                        vb = vbhn_array[i].get_glvb();
                        vb_offset = vbhn_array[i].get_glvb_offset();
                    }

                    if (vb && ib && (!morph_vbhn_array || morph_vb))
                        rctx_ptr->disp_manager->add_vertex_array(mesh, sub_mesh, material,
                            vb, vb_offset, ib, morph_vb, morph_vb_offset);
                }
            }
        }
    }

    // 0x1401DE5F0
    bool Object::load(const SceneFile& in_file, const a3d::Object& in_o,
        const object_database* obj_db, const texture_database* tex_db) {
        name.assign(in_o.name);
        uid_name.assign(in_o.uid_name);

        obj_uid = obj_db->get_object_info(uid_name.c_str());
        obj_hash = hash_string_murmurhash(uid_name);

        static std::string reflect_str = "_REFLECT";
        is_reflect = uid_name.find(reflect_str) != -1;

        static std::string refract_str = "_REFRACT";
        is_refract = uid_name.find(refract_str) != -1;

        model_transform.load(in_file, in_o.model_transform);

        obj_morph.name.assign(in_o.morph);
        obj_morph.offset = in_o.morph_offset;

        obj_pat.name.assign(in_o.pattern);
        obj_pat.offset = in_o.pattern_offset;

        parent_name.assign(in_o.parent_name);
        parent_node.assign(in_o.parent_node);

        list_load(texture_list, in_file, in_o.texture, tex_db);
        list_load(texture_transform_list, in_file, in_o.texture_transform, tex_db);
        return true;
    }

    // Added
    void Object::store(const SceneFile& in_file, a3d::Object& out_o) const {
        out_o.name.assign(name);
        out_o.uid_name.assign(uid_name);

        model_transform.store(in_file, out_o.model_transform);

        out_o.morph.assign(obj_morph.name);
        out_o.morph_offset = obj_morph.offset;

        out_o.pattern.assign(obj_pat.name);
        out_o.pattern_offset = obj_pat.offset;

        out_o.parent_name.assign(parent_name);
        out_o.parent_node.assign(parent_node);

        list_store(texture_list, in_file, out_o.texture);
        list_store(texture_transform_list, in_file, out_o.texture_transform);
    }

    // 0x1401BC860
    Object& Object::operator=(const Object& other) {
        model_transform = other.model_transform;
        index = other.index;
        name.assign(other.name);
        uid_name.assign(other.uid_name);
        parent_name.assign(other.parent_name);
        parent_node.assign(other.parent_node);
        obj_pat = other.obj_pat;
        obj_morph = other.obj_morph;
        texture_list.assign(other.texture_list.begin(), other.texture_list.end());
        texture_transform_list.assign(other.texture_transform_list.begin(), other.texture_transform_list.end());
        child_object_list.assign(other.child_object_list.begin(), other.child_object_list.end());
        child_object_hrc_list.assign(other.child_object_hrc_list.begin(), other.child_object_hrc_list.end());
        obj_uid = other.obj_uid;
        obj_hash = other.obj_hash;
        is_reflect = other.is_reflect;
        is_refract = other.is_refract;
        return *this;
    }

    // 0x1401B5DE0
    ObjectInstance::ObjectInstance() : name(), uid_name(), obj_uid(),
        obj_hash(hash_murmurhash_empty), shadow(), motion_transform(), matrix_id(), mats() {

    }

    // 0x1401B5D20
    ObjectInstance::ObjectInstance(const ObjectInstance& other) : name(other.name),
        uid_name(other.uid_name), obj_uid(other.obj_uid), obj_hash(other.obj_hash), shadow(other.shadow),
        motion_transform(other.motion_transform), matrix_id(other.matrix_id), mats(other.mats) {

    }

    // 0x1401B91E0
    ObjectInstance::~ObjectInstance() {

    }

    // Missing
    void ObjectInstance::init() {
        name.clear();
        name.shrink_to_fit();
        uid_name.clear();
        uid_name.shrink_to_fit();

        obj_uid = {};
        obj_hash = (uint32_t)-1;
        shadow = false;

        matrix_id.clear();
        matrix_id.shrink_to_fit();
        mats.clear();
        mats.shrink_to_fit();
    }

    // Inlined
    inline void ObjectInstance::get(float_t in_frame) {
        motion_transform.get(in_frame);
        motion_transform.calc_matrix_local();
    }

    // Inlined
    inline bool ObjectInstance::load(const SceneFile& in_file, const a3d::ObjectInstance& in_oi,
        const MultiHierarchyObject& in_mho, const object_database* obj_db) {
        name.assign(in_oi.name);
        uid_name.assign(in_oi.uid_name);

        obj_uid = obj_db->get_object_info(uid_name.c_str());
        obj_hash = hash_string_murmurhash(uid_name);

        shadow = in_oi.shadow;
        motion_transform.load(in_file, in_oi.model_transform);

        const obj_skin* skin = get_object_skin(obj_uid);
        if (!skin) {
            std::string error_str = sprintf_s_string("%s skin data is not found", uid_name.c_str());
            throw std::runtime_error(error_str.c_str());
        }

        matrix_id.resize(skin->num_bone);

        size_t bone_num = 0;
        for (size_t i = 0; i < in_mho.node_list.size(); i++) {
            const TransformNode& node = in_mho.node_list[i];
            size_t num_bone = skin->num_bone;
            for (size_t j = 0; j < num_bone; j++)
                if (!node.name.compare(skin->bone_array[j].name)) {
                    int32_t node_index = skin->bone_array[j].id;
                    if (node_index & 0x8000) {
                        std::string error_str = sprintf_s_string(
                            "%s skin bone type is not normal bone", uid_name.c_str());
                        throw std::runtime_error(error_str.c_str());
                    }

                    matrix_id[node_index] = (int32_t)i;
                    bone_num++;
                    break;
                }
        }

        if (!bone_num) {
            std::string error_str = sprintf_s_string("%s skin data has no bone", uid_name.c_str());
            throw std::runtime_error(error_str.c_str());
        }

        if (bone_num != skin->num_bone)
            throw std::range_error("bone_num mismatch");

        mats.resize(bone_num);
        return true;
    }

    // Added
    inline void ObjectInstance::store(const SceneFile& in_file, a3d::ObjectInstance& out_oi,
        const MultiHierarchyObject& in_mho) const {
        out_oi.name.assign(name);
        out_oi.uid_name.assign(uid_name);
        out_oi.shadow = shadow;
        motion_transform.store(in_file, out_oi.model_transform);
    }

    // 0x1401BCA10
    ObjectInstance& ObjectInstance::operator=(const ObjectInstance& other) {
        name.assign(other.name);
        uid_name.assign(other.uid_name);
        obj_uid = other.obj_uid;
        obj_hash = other.obj_hash;
        shadow = other.shadow;
        motion_transform = other.motion_transform;
        matrix_id.assign(other.matrix_id.begin(), other.matrix_id.end());
        mats.assign(other.mats.begin(), other.mats.end());
        return *this;
    }

    // Missing
    PlayControl::PlayControl() : flag(),
        begin(), div(), fps(), offset(), size() {

    }

    // Missing
    PlayControl::PlayControl(const PlayControl& other) : flag(other.flag),
        begin(other.begin), div(other.div), fps(other.fps), offset(other.offset), size(other.size) {

    }

    // Missing
    PlayControl::~PlayControl() {

    }

    // 0x1401D78B0
    void PlayControl::init() {
        flag = 0;
        begin = 0.0f;
        div = 0; // F2/X/XHD
        fps = 0.0f;
        offset = 0.0f;
        size = 0.0f;
    }

    // 0x1401DEE10
    bool PlayControl::load(const SceneFile& in_file, const a3d::PlayControl& in_pc) {
        begin = in_pc.begin;

        // F2/X/XHD
        if (in_pc.flag & a3d::PlayControl::FLAG_DIV) {
            div = in_pc.div;
            flag |= FLAG_DIV;
        }

        fps = in_pc.fps;

        // F2/X/XHD
        if (in_pc.flag & a3d::PlayControl::FLAG_OFFSET) {
            offset = in_pc.offset;
            flag |= FLAG_OFFSET;
        }

        size = in_pc.size;
        return true;
    }

    // Added
    void PlayControl::store(const SceneFile& in_file, a3d::PlayControl& out_pc) const {
        out_pc.begin = begin;

        // F2/X/XHD
        if (flag & FLAG_DIV) {
            out_pc.div = div;
            out_pc.flag |= a3d::PlayControl::FLAG_DIV;
        }

        out_pc.fps = fps;

        // F2/X/XHD
        if (flag & FLAG_OFFSET) {
            out_pc.offset = offset;
            out_pc.flag |= a3d::PlayControl::FLAG_OFFSET;
        }

        out_pc.size = size;
    }

    // Missing
    PlayControl& PlayControl::operator=(const PlayControl& other) {
        flag = other.flag;
        begin = other.begin;
        div = other.div;
        fps = other.fps;
        offset = other.offset;
        size = other.size;
        return *this;
    }

    // 0x1401B5EE0
    Point::Point() : name(), model_transform() {

    }

    // 0x1401B5E80
    Point::Point(const Point& other) : name(other.name), model_transform(other.model_transform) {

    }

    // 0x1401B92D0
    Point::~Point() {

    }

    // 0x1401D4B20
    void Point::get(float_t in_frame) {
        model_transform.get(in_frame);
    }

    // 0x1401D1150
    void Point::disp(const Scene& in_scene, render_context* rctx) {
        if (in_scene.M_debug) {
            model_transform.mul(in_scene.base_matrix);
            debug_put_line_axis(model_transform.get_matrix());
        }
    }

    // 0x1401DEF50
    bool Point::load(const SceneFile& in_file, const a3d::Point& in_p) {
        name.assign(in_p.name);
        model_transform.load(in_file, in_p.model_transform);
        return true;
    }

    // Added
    void Point::store(const SceneFile& in_file, a3d::Point& out_p) const {
        out_p.name.assign(name);
        model_transform.store(in_file, out_p.model_transform);
    }

    // 0x1401BCAA0
    Point& Point::operator=(const Point& other) {
        name.assign(other.name);
        model_transform = other.model_transform;
        return *this;
    }

    // 0x1401B5F20
    PostProcess::PostProcess() : flag(), backup_flag(),
        fcurve_lens_flare(), fcurve_lens_shaft(), fcurve_lens_ghost(), radius(), intensity(),
        scene_fade(), backup_lens_flare(), backup_lens_shaft(), backup_lens_ghost(), backup_radius(),
        backup_intensity(), backup_scene_fade(), lens_flare(), lens_shaft(), lens_ghost() {

    }

    // Missing
    PostProcess::PostProcess(const PostProcess& other) : flag(other.flag), backup_flag(other.backup_flag),
        fcurve_lens_flare(other.fcurve_lens_flare), fcurve_lens_shaft(other.fcurve_lens_shaft),
        fcurve_lens_ghost(other.fcurve_lens_ghost), radius(other.radius), intensity(other.intensity),
        scene_fade(other.scene_fade), backup_lens_flare(other.backup_lens_flare),
        backup_lens_shaft(other.backup_lens_shaft), backup_lens_ghost(other.backup_lens_ghost),
        backup_radius(other.backup_radius), backup_intensity(other.backup_intensity),
        backup_scene_fade(other.backup_scene_fade), lens_flare(other.lens_flare),
        lens_shaft(other.lens_shaft), lens_ghost(other.lens_ghost) {

    }

    // 0x1401B9350
    PostProcess::~PostProcess() {

    }

    // 0x1401D78C0
    void PostProcess::init() {
        flag = 0;
        backup_flag = 0;

        fcurve_lens_flare.init();
        fcurve_lens_shaft.init();
        fcurve_lens_ghost.init();
    }

    // 0x1401D4B30
    void PostProcess::get(float_t in_frame) {
        if (has_lens_flare())
            lens_flare = fcurve_lens_flare.get(in_frame);
        if (has_lens_shaft())
            lens_shaft = fcurve_lens_shaft.get(in_frame);
        if (has_lens_ghost())
            lens_ghost = fcurve_lens_ghost.get(in_frame);
        if (has_radius())
            radius.get(in_frame);
        if (has_intensity())
            intensity.get(in_frame);
        if (has_scene_fade())
            scene_fade.get(in_frame);
    }

    // 0x1401CEF00
    void PostProcess::ctrl(Scene& in_scene, render_context* rctx) {
        if (has_lens()) {
            vec3 lens = rctx->render.get_lens();
            if (has_lens_flare()) {
                if (!(backup_flag & FLAG_LENS_FLARE)) {
                    backup_lens_flare = lens.x;
                    backup_flag |= FLAG_LENS_FLARE;
                }
                lens.x = lens_flare;
            }

            if (has_lens_shaft()) {
                if (!(backup_flag & FLAG_LENS_SHAFT)) {
                    backup_lens_shaft = lens.y;
                    backup_flag |= FLAG_LENS_SHAFT;
                }
                lens.y = lens_shaft;
            }

            if (has_lens_ghost()) {
                if (!(backup_flag & FLAG_LENS_GHOST)) {
                    backup_lens_ghost = lens.z;
                    backup_flag |= FLAG_LENS_GHOST;
                }
                lens.z = lens_ghost;
            }
            rctx->render.set_lens(lens);
        }

        if (has_radius()) {
            if (!(backup_flag & FLAG_RADIUS)) {
                *(vec3*)&backup_radius = rctx->render.get_radius();
                backup_flag |= FLAG_RADIUS;
            }

            rctx->render.set_radius(*(vec3*)&radius.color);
        }

        if (has_intensity()) {
            if (!(backup_flag & FLAG_INTENSITY)) {
                *(vec3*)&backup_intensity = rctx->render.get_intensity();
                backup_flag |= FLAG_INTENSITY;
            }

            rctx->render.set_intensity(*(vec3*)&intensity.color);
        }

        if (has_scene_fade()) {
            if (!(backup_flag & FLAG_SCENE_FADE)) {
                backup_scene_fade = rctx->render.get_scene_fade();
                backup_flag |= FLAG_SCENE_FADE;
            }

            rctx->render.set_scene_fade(scene_fade.color, 0);
        }
    }

    // 0x1401D0050
    void PostProcess::dest(render_context* rctx) {
        if (backup_flag & (FLAG_LENS_FLARE | FLAG_LENS_SHAFT | FLAG_LENS_GHOST)) {
            vec3 value = rctx->render.get_lens();
            if (backup_flag & FLAG_LENS_FLARE)
                value.x = backup_lens_flare;
            if (backup_flag & FLAG_LENS_SHAFT)
                value.y = backup_lens_shaft;
            if (backup_flag & FLAG_LENS_GHOST)
                value.z = backup_lens_ghost;
            rctx->render.set_lens(value);
        }

        if (backup_flag & FLAG_RADIUS)
            rctx->render.set_radius(*(vec3*)&backup_radius);
        if (backup_flag & FLAG_INTENSITY)
            rctx->render.set_intensity(*(vec3*)&backup_intensity);
        if (backup_flag & FLAG_SCENE_FADE)
            rctx->render.set_scene_fade(backup_scene_fade, 0);
    }

    // 0x1401DF050
    bool PostProcess::load(const SceneFile& in_file, const a3d::PostProcess& in_pp) {
        if (in_pp.flag & a3d::PostProcess::FLAG_LENS_FLARE) {
            fcurve_lens_flare.load(in_file, in_pp.lens_flare);
            flag |= FLAG_LENS_FLARE;
        }

        if (in_pp.flag & a3d::PostProcess::FLAG_LENS_SHAFT) {
            fcurve_lens_shaft.load(in_file, in_pp.lens_shaft);
            flag |= FLAG_LENS_SHAFT;
        }

        if (in_pp.flag & a3d::PostProcess::FLAG_LENS_GHOST) {
            fcurve_lens_ghost.load(in_file, in_pp.lens_ghost);
            flag |= FLAG_LENS_GHOST;
        }

        if (in_pp.flag & a3d::PostProcess::FLAG_RADIUS) {
            radius.load(in_file, in_pp.radius);
            flag |= FLAG_RADIUS;
        }

        if (in_pp.flag & a3d::PostProcess::FLAG_INTENSITY) {
            intensity.load(in_file, in_pp.intensity);
            flag |= FLAG_INTENSITY;
        }

        if (in_pp.flag & a3d::PostProcess::FLAG_SCENE_FADE) {
            scene_fade.load(in_file, in_pp.scene_fade);
            flag |= FLAG_SCENE_FADE;
        }
        return true;
    }

    // Added
    void PostProcess::store(const SceneFile& in_file, a3d::PostProcess& out_pp) const {
        if (flag & FLAG_LENS_FLARE) {
            fcurve_lens_flare.store(in_file, out_pp.lens_flare);
            out_pp.flag |= a3d::PostProcess::FLAG_LENS_FLARE;
        }

        if (flag & FLAG_LENS_SHAFT) {
            fcurve_lens_shaft.store(in_file, out_pp.lens_shaft);
            out_pp.flag |= a3d::PostProcess::FLAG_LENS_SHAFT;
        }

        if (flag & FLAG_LENS_GHOST) {
            fcurve_lens_ghost.store(in_file, out_pp.lens_ghost);
            out_pp.flag |= a3d::PostProcess::FLAG_LENS_GHOST;
        }

        if (flag & FLAG_RADIUS) {
            radius.store(in_file, out_pp.radius);
            out_pp.flag |= a3d::PostProcess::FLAG_RADIUS;
        }

        if (flag & FLAG_INTENSITY) {
            intensity.store(in_file, out_pp.intensity);
            out_pp.flag |= a3d::PostProcess::FLAG_INTENSITY;
        }

        if (flag & FLAG_SCENE_FADE) {
            scene_fade.store(in_file, out_pp.scene_fade);
            out_pp.flag |= a3d::PostProcess::FLAG_SCENE_FADE;
        }
    }

    // Missing
    PostProcess& PostProcess::operator=(const PostProcess& other) {
        flag = other.flag;
        backup_flag = other.backup_flag;
        fcurve_lens_flare = other.fcurve_lens_flare;
        fcurve_lens_shaft = other.fcurve_lens_shaft;
        fcurve_lens_ghost = other.fcurve_lens_ghost;
        radius = other.radius;
        intensity = other.intensity;
        scene_fade = other.scene_fade;
        backup_lens_flare = other.backup_lens_flare;
        backup_lens_shaft = other.backup_lens_shaft;
        backup_lens_ghost = other.backup_lens_ghost;
        backup_radius = other.backup_radius;
        backup_intensity = other.backup_intensity;
        backup_scene_fade = other.backup_scene_fade;
        lens_flare = other.lens_flare;
        lens_shaft = other.lens_shaft;
        lens_ghost = other.lens_ghost;
        return *this;
    }

    // 0x1401B6550
    Texture::Texture() : name(), tex_uid(-1), tex_pat() {

    }

    // 0x1401B64C0
    Texture::Texture(const Texture& other)
        : name(other.name), tex_uid(other.tex_uid), tex_pat(other.tex_pat) {

    }

    // 0x1401B9BF0
    Texture::~Texture() {

    }

    // 0x1401D8420
    void Texture::init() {
        name.clear();
        name.shrink_to_fit();
        tex_uid = -1;
        tex_pat.init();
    }

    // 0x1401D4C00
    void Texture::get(float_t in_frame) {
        tex_pat.get(in_frame);
    }

    // 0x1401DFA50
    bool Texture::load(const SceneFile& in_file,
        const a3d::Texture& in_t, const texture_database* tex_db) {
        name.assign(in_t.name);
        tex_uid = tex_db->get_texture_id(name.c_str());
        tex_pat.name.assign(in_t.pattern);
        tex_pat.offset = in_t.pattern_offset;
        return true;
    }

    // Added
    void Texture::store(const SceneFile& in_file, a3d::Texture& out_t) const {
        out_t.name.assign(name);
        out_t.pattern.assign(tex_pat.name);
        out_t.pattern_offset = tex_pat.offset;
    }

    // 0x1401BCBA0
    Texture& Texture::operator=(const Texture& other) {
        name.assign(other.name);
        tex_uid = other.tex_uid;
        tex_pat = other.tex_pat;
        return *this;
    }

    // 0x1401B6C30
    TextureTransform::TextureTransform() : flag(), backup_flag(), name(), tex_uid(), fcurve_coverage_u(),
        fcurve_coverage_v(), fcurve_repeat_u(), fcurve_repeat_v(), fcurve_translate_frame_u(),
        fcurve_translate_frame_v(), fcurve_offset_u(), fcurve_offset_v(), fcurve_rotate_frame(),
        fcurve_rotate(), mat(), scale(), repeat(), rotate(), translate_frame() {
    }

    // 0x1401B65A0
    TextureTransform::TextureTransform(const TextureTransform& other)
        : flag(other.flag), backup_flag(other.backup_flag), name(other.name), tex_uid(other.tex_uid),
        fcurve_coverage_u(other.fcurve_coverage_u), fcurve_coverage_v(other.fcurve_coverage_v),
        fcurve_repeat_u(other.fcurve_repeat_u), fcurve_repeat_v(other.fcurve_repeat_v),
        fcurve_translate_frame_u(other.fcurve_translate_frame_u),
        fcurve_translate_frame_v(other.fcurve_translate_frame_v),
        fcurve_offset_u(other.fcurve_offset_u), fcurve_offset_v(other.fcurve_offset_v),
        fcurve_rotate_frame(other.fcurve_rotate_frame), fcurve_rotate(other.fcurve_rotate),
        mat(other.mat), scale(other.scale), repeat(other.repeat), rotate(other.rotate),
        translate_frame(other.translate_frame) {

    }

    // 0x1401B9C50
    TextureTransform::~TextureTransform() {

    }

    // 0x1401D8460
    void TextureTransform::init() {
        flag = 0;
        tex_uid = -1;
        name.clear();
        name.shrink_to_fit();

        mat = mat4_identity;

        fcurve_coverage_u.init();
        fcurve_coverage_v.init();
        fcurve_repeat_u.init();
        fcurve_repeat_v.init();
        fcurve_translate_frame_u.init();
        fcurve_translate_frame_v.init();
        fcurve_offset_u.init();
        fcurve_offset_v.init();
        fcurve_rotate_frame.init();
        fcurve_rotate.init();
    }

    // 0x1401D4C10
    void TextureTransform::get(float_t in_frame) {
        scale = 1.0f;

        if (has_coverage_u() && fcurve_coverage_u.m_max_frame > 0.0f) {
            float_t coverage_u = fcurve_coverage_u.get(in_frame);
            if (fabsf(coverage_u) > 0.000001f)
                scale.x = 1.0f / coverage_u;
        }

        if (has_coverage_v() && fcurve_coverage_v.m_max_frame > 0.0f) {
            float_t coverage_v = fcurve_coverage_v.get(in_frame);
            if (fabsf(coverage_v) > 0.000001f)
                scale.y = 1.0f / coverage_v;
        }

        repeat = 1.0f;

        if (has_repeat_u() && fcurve_repeat_u.m_max_frame > 0.0f) {
            float_t value = fcurve_repeat_u.get(in_frame);
            if (fabsf(value) > 0.000001f)
                repeat.x = value;
        }

        if (has_repeat_v() && fcurve_repeat_v.m_max_frame > 0.0f) {
            float_t value = fcurve_repeat_v.get(in_frame);
            if (fabsf(value) > 0.000001f)
                repeat.y = value;
        }

        float_t rotate_frame_value = 0.0f;
        if (has_rotate_frame())
            rotate_frame_value = fcurve_rotate_frame.get(in_frame);

        float_t rotate_value = 0.0f;
        if (has_rotate())
            rotate_value = fcurve_rotate.get(in_frame);

        rotate.z = rotate_frame_value + rotate_value;

        float_t translate_frame_u = 0.0f;
        if (has_translate_frame_u())
            translate_frame_u = -fcurve_translate_frame_u.get(in_frame);

        float_t offset_u = 0.0f;
        if (has_offset_u())
            offset_u = fcurve_offset_u.get(in_frame);

        translate_frame.x = (translate_frame_u + offset_u) * repeat.x;

        float_t translate_frame_v = 0.0f;
        if (has_translate_frame_v())
            translate_frame_v = -fcurve_translate_frame_v.get(in_frame);

        float_t offset_v = 0.0f;
        if (has_offset_v())
            offset_v = fcurve_offset_v.get(in_frame);

        translate_frame.y = (translate_frame_v + offset_v) * repeat.y;

        mul();
    }

    // 0x1401E1DB0
    void TextureTransform::mul() {
        mat4_translate(&translate_frame, &mat);
        mat4_scale_rot(&mat, &scale, &mat);

        mat4_mul_translate(&mat, 1.0f, 1.0f, 0.0f, &mat);
        mat4_scale_rot(&mat, &repeat, &mat);
        mat4_mul_translate(&mat, -1.0f, -1.0f, 0.0f, &mat);

        mat4_mul_translate(&mat, 0.5f, 0.5f, 0.0f, &mat);
        mat4_mul_rotate_z(&mat, rotate.z, &mat);
        mat4_mul_translate(&mat, -0.5f, -0.5f, 0.0f, &mat);
    }

    // 0x1401DFCB0
    bool TextureTransform::load(const SceneFile& in_file,
        const a3d::TextureTransform& in_tt, const texture_database* tex_db) {
        name.assign(in_tt.name);
        tex_uid = tex_db->get_texture_id(name.c_str());

        if (in_tt.flag & a3d::TextureTransform::FLAG_COVERAGE_U) {
            fcurve_coverage_u.load(in_file, in_tt.coverage_u);
            flag |= FLAG_COVERAGE_U;
        }

        if (in_tt.flag & a3d::TextureTransform::FLAG_COVERAGE_V) {
            fcurve_coverage_v.load(in_file, in_tt.coverage_v);
            flag |= FLAG_COVERAGE_V;
        }

        if (in_tt.flag & a3d::TextureTransform::FLAG_REPEAT_U) {
            fcurve_repeat_u.load(in_file, in_tt.repeat_u);
            flag |= FLAG_REPEAT_U;
        }

        if (in_tt.flag & a3d::TextureTransform::FLAG_REPEAT_V) {
            fcurve_repeat_v.load(in_file, in_tt.repeat_v);
            flag |= FLAG_REPEAT_V;
        }

        if (in_tt.flag & a3d::TextureTransform::FLAG_ROTATE) {
            fcurve_rotate.load(in_file, in_tt.rotate);
            flag |= FLAG_ROTATE;
        }

        if (in_tt.flag & a3d::TextureTransform::FLAG_ROTATE_FRAME) {
            fcurve_rotate_frame.load(in_file, in_tt.rotate_frame);
            flag |= FLAG_ROTATE_FRAME;
        }

        if (in_tt.flag & a3d::TextureTransform::FLAG_OFFSET_U) {
            fcurve_offset_u.load(in_file, in_tt.offset_u);
            flag |= FLAG_OFFSET_U;
        }

        if (in_tt.flag & a3d::TextureTransform::FLAG_OFFSET_V) {
            fcurve_offset_v.load(in_file, in_tt.offset_v);
            flag |= FLAG_OFFSET_V;
        }

        if (in_tt.flag & a3d::TextureTransform::FLAG_TRANSLATE_FRAME_U) {
            fcurve_translate_frame_u.load(in_file, in_tt.translate_frame_u);
            flag |= FLAG_TRANSLATE_FRAME_U;
        }

        if (in_tt.flag & a3d::TextureTransform::FLAG_TRANSLATE_FRAME_V) {
            fcurve_translate_frame_v.load(in_file, in_tt.translate_frame_v);
            flag |= FLAG_TRANSLATE_FRAME_V;
        }
        return true;
    }

    // Added
    void TextureTransform::store(const SceneFile& in_file, a3d::TextureTransform& out_tt) const {
        out_tt.name.assign(name);

        if (flag & FLAG_COVERAGE_U) {
            fcurve_coverage_u.store(in_file, out_tt.coverage_u);
            out_tt.flag |= a3d::TextureTransform::FLAG_COVERAGE_U;
        }

        if (flag & FLAG_COVERAGE_V) {
            fcurve_coverage_v.store(in_file, out_tt.coverage_v);
            out_tt.flag |= a3d::TextureTransform::FLAG_COVERAGE_V;
        }

        if (flag & FLAG_REPEAT_U) {
            fcurve_repeat_u.store(in_file, out_tt.repeat_u);
            out_tt.flag |= a3d::TextureTransform::FLAG_REPEAT_U;
        }

        if (flag & FLAG_REPEAT_V) {
            fcurve_repeat_v.store(in_file, out_tt.repeat_v);
            out_tt.flag |= a3d::TextureTransform::FLAG_REPEAT_V;
        }

        if (flag & FLAG_ROTATE) {
            fcurve_rotate.store(in_file, out_tt.rotate);
            out_tt.flag |= a3d::TextureTransform::FLAG_ROTATE;
        }

        if (flag & FLAG_ROTATE_FRAME) {
            fcurve_rotate_frame.store(in_file, out_tt.rotate_frame);
            out_tt.flag |= a3d::TextureTransform::FLAG_ROTATE_FRAME;
        }

        if (flag & FLAG_OFFSET_U) {
            fcurve_offset_u.store(in_file, out_tt.offset_u);
            out_tt.flag |= a3d::TextureTransform::FLAG_OFFSET_U;
        }

        if (flag & FLAG_OFFSET_V) {
            fcurve_offset_v.store(in_file, out_tt.offset_v);
            out_tt.flag |= a3d::TextureTransform::FLAG_OFFSET_V;
        }

        if (flag & FLAG_TRANSLATE_FRAME_U) {
            fcurve_translate_frame_u.store(in_file, out_tt.translate_frame_u);
            out_tt.flag |= a3d::TextureTransform::FLAG_TRANSLATE_FRAME_U;
        }

        if (flag & FLAG_TRANSLATE_FRAME_V) {
            fcurve_translate_frame_v.store(in_file, out_tt.translate_frame_v);
            out_tt.flag |= a3d::TextureTransform::FLAG_TRANSLATE_FRAME_V;
        }
    }

    // 0x1401BCC10
    TextureTransform& TextureTransform::operator=(const TextureTransform& other) {
        flag = other.flag;
        backup_flag = other.backup_flag;
        name.assign(other.name);
        tex_uid = other.tex_uid;
        fcurve_coverage_u = other.fcurve_coverage_u;
        fcurve_coverage_v = other.fcurve_coverage_v;
        fcurve_repeat_u = other.fcurve_repeat_u;
        fcurve_repeat_v = other.fcurve_repeat_v;
        fcurve_translate_frame_u = other.fcurve_translate_frame_u;
        fcurve_translate_frame_v = other.fcurve_translate_frame_v;
        fcurve_offset_u = other.fcurve_offset_u;
        fcurve_offset_v = other.fcurve_offset_v;
        fcurve_rotate_frame = other.fcurve_rotate_frame;
        fcurve_rotate = other.fcurve_rotate;
        mat = other.mat;
        scale = other.scale;
        repeat = other.repeat;
        rotate = other.rotate;
        translate_frame = other.translate_frame;
        return *this;
    }

    // 0x1401B7080
    TransformNode::TransformNode() : flag(), name(), node_index(-1), parent_id(-1),
        motion_transform(), mat_ptr(), joint_orient(), joint_orient_mat() {

    }

    // 0x1401B6F40
    TransformNode::TransformNode(const TransformNode& other) : flag(other.flag), name(other.name),
        node_index(other.node_index), parent_id(other.parent_id), motion_transform(other.motion_transform),
        mat_ptr(other.mat_ptr), joint_orient(other.joint_orient), joint_orient_mat(other.joint_orient_mat) {

    }

    // 0x1401B9E00
    TransformNode::~TransformNode() {

    }

    // Missing
    void TransformNode::init() {
        flag = 0;
        name.clear();
        name.shrink_to_fit();
        node_index = -1;
        parent_id = -1;

        mat_ptr = 0;
    }

    // Inlined
    inline void TransformNode::get(float_t in_frame) {
        motion_transform.get(in_frame);
        motion_transform.calc_matrix_local();
    }

    // 0x1401CAF10
    void TransformNode::calc_matrix_local(const mat4& in_base_mat) {
        mat4 mat = in_base_mat;
        mat4_mul(&joint_orient_mat, &mat, &mat);
        motion_transform.calc_matrix(mat);

        if (mat_ptr)
            *mat_ptr = motion_transform.matrix;
    }

    // Inlined
    inline bool TransformNode::load(const SceneFile& in_file, const a3d::ObjectNode& in_on) {
        name.assign(in_on.name);
        parent_id = in_on.parent;
        motion_transform.load(in_file, in_on.model_transform);

        if (in_on.flag & a3d::ObjectNode::FLAG_JOINT_ORIENT) {
            joint_orient = in_on.joint_orient;
            mat4_rotate_zyx(&joint_orient, &joint_orient_mat);
            flag |= FLAG_JOINT_ORIENT;
        }
        else {
            joint_orient = 0.0f;
            joint_orient_mat = mat4_identity;
        }
        return true;
    }

    // Added
    inline void TransformNode::store(const SceneFile& in_file, a3d::ObjectNode& out_on) const {
        out_on.name.assign(name);
        out_on.parent = parent_id;
        motion_transform.store(in_file, out_on.model_transform);

        if (flag & FLAG_JOINT_ORIENT) {
            out_on.joint_orient = joint_orient;
            out_on.flag |= a3d::ObjectNode::FLAG_JOINT_ORIENT;
        }
        else
            out_on.joint_orient = 0.0f;
    }

    // 0x1401BD280
    TransformNode& TransformNode::operator=(const TransformNode& other) {
        flag = other.flag;
        name.assign(other.name);
        node_index = other.node_index;
        parent_id = other.parent_id;
        motion_transform = other.motion_transform;
        mat_ptr = other.mat_ptr;
        joint_orient = other.joint_orient;
        joint_orient_mat = other.joint_orient_mat;
        return *this;
    }

    // 0x1401B3250
    DB::DB() {

    }

    // 0x1401B8650
    DB::~DB() {

    }

    // 0x1401C1850
    void CategoryData::M_load_req(const char* in_mdata_dir) {
        if (load_state != S_BEFORE_LOADING) {
            load_state = S_DONE;
            return;
        }

        load_state = S_NOW_LOADING;

        if (in_mdata_dir && *in_mdata_dir) {
            path.assign(in_mdata_dir);
            path.append("rom/auth_3d/");
        }
        else
            path.assign("rom/auth_3d/");
        file.assign(category_name);
        file.append(".farc");

        if (file_handler.read_file(&data_list[DATA_AFT],
            path.c_str(), file.c_str(), prj::MemCSystem))
            file_handler.set_callback_data(0, (PFNFILEHANDLERCALLBACK*)S_load_cb, this);
        else
            load_state = S_DONE;
    }

    // Added
    void CategoryData::M_load_req_modern(void* data) {
        if (load_state != S_BEFORE_LOADING) {
            load_state = S_DONE;
            return;
        }

        load_state = S_NOW_LOADING;
        path.assign("root+/auth_3d/");
        file.assign(category_name);
        file.append(".farc");

        if (file_handler.read_file(data, path.c_str(),
            hash_string_murmurhash(category_name), ".farc", prj::MemCSystem))
            file_handler.set_callback_data(0, (PFNFILEHANDLERCALLBACK*)S_load_cb, this);
        else
            load_state = S_DONE;
    }

    // 0x1401BF7B0
    void CategoryData::M_free() {
        if (load_state == S_NOW_LOADING)
            file_handler.call_free_callback();
        else if (load_state == S_DONE) {
            delete ar;
            ar = 0;
            file_handler.reset();
        }

        path.clear();
        path.shrink_to_fit();
        file.clear();
        file.shrink_to_fit();
        raw_data = 0;
        raw_size = 0;
        load_state = S_BEFORE_LOADING;
    }

    // 0x1401C1EE0
    bool CategoryData::S_load_cb(CategoryData* data, const void* file_data, size_t file_size) {
        if (data->load_state == S_NOW_LOADING) {
            data->load_state = S_DONE;
            data->raw_data = file_data;
            data->raw_size = file_size;
        }

        if (!data->ar)
            data->ar = new farc;

        if (data->ar) {
            data->ar->read(data->raw_data, data->raw_size, true);
            return true;
        }
        return false;
    }

    // 0x1401B2F20
    CategoryData::CategoryData() : ref_count(), load_state(), ar(), raw_data(), raw_size() {

    }

    // 0x1401B2E80
    CategoryData::CategoryData(const CategoryData& other) {
        *this = other;
    }

    // 0x1401B84C0
    CategoryData::~CategoryData() {
        delete ar;
    }

    // 0x1401E0F60
    void CategoryData::load_req(const char* in_mdata_dir) {
        if (ref_count)
            ref_count++;
        else {
            ref_count = 1;
            M_load_req(in_mdata_dir);
        }
    }

    // Added
    void CategoryData::load_req_modern(void* data) {
        if (ref_count)
            ref_count++;
        else {
            ref_count = 1;
            M_load_req_modern(data);
        }
    }

    // 0x1401D3B10
    void CategoryData::free() {
        if (--ref_count < 0)
            ref_count = 0;
        else if (!ref_count)
            M_free();
    }

    // 0x1401D54C0
    std::tuple<size_t, const void*> CategoryData::get_file(const std::string& in_name) {
        if (ar) {
            farc_file* ff = ar->read_file(in_name.c_str());
            if (ff)
                return { ff->size, ff->data };
        }
        return {};
    }

    // Added
    std::tuple<size_t, const void*, const char*> CategoryData::get_file(uint32_t in_hash) {
        if (ar) {
            farc_file* ff = ar->read_file(in_hash);
            if (ff)
                return { ff->size, ff->data, ff->name.c_str() };
        }
        return {};
    }

    // 0x1401E0670
    inline bool CategoryData::load_is_done() const {
        return load_state == S_DONE;
    }

    // 0x1401BB670
    CategoryData& CategoryData::operator=(const CategoryData& other) {
        ref_count = other.ref_count;
        category_name.assign(other.category_name);
        load_state = other.load_state;
        path.assign(other.path);
        file.assign(other.file);
        file_handler = other.file_handler;
        ar = other.ar ? new farc(*other.ar) : 0;
        raw_data = other.raw_data;
        raw_size = other.raw_size;
        return *this;
    }

    // 0x1401B63F0
    SceneFile::SceneFile() : frame_max(), raw_data(), raw_size(), has_raw(), prop_is_ready() {

    }

    // 0x1401B6320
    SceneFile::SceneFile(const SceneFile& other) {
        *this = other;
    }

    // 0x1401B9B20
    SceneFile::~SceneFile() {

    }

    // 0x1401DF370
    void SceneFile::load(const void* in_data, size_t in_size) {
        frame_max = 0.0f;
        raw_data = in_data;
        raw_size = in_size;
        has_raw = true;

        prop.read(in_data, in_size);
        prop_is_ready = prop.ready;
    }

    // 0x1401CD130
    void SceneFile::clear() {
        frame_max = 0.0f;
        raw_data = 0;
        raw_size = 0;
        has_raw = false;
        prop = {};
        prop_is_ready = 0;
    }

    // 0x1401BCAE0
    SceneFile& SceneFile::operator=(const SceneFile& other) {
        frame_max = other.frame_max;
        raw_data = other.raw_data;
        raw_size = other.raw_size;
        has_raw = other.has_raw;
        prop = other.prop;
        prop_is_ready = other.prop_is_ready;
        return *this;
    }

    // Inlined
    void UidData::M_load_req(const auth_3d_database* auth_3d_db) {
        load_state = S_DONE;
        if (uid >= auth_3d_db->uid.size())
            return;

        const auth_3d_database_uid* db_uid = &auth_3d_db->uid[uid];

        if (!db_uid->enabled)
            return;

        name = db_uid->name.c_str();
        file_name.assign(db_uid->name);
        file_name.append(".a3da");

        category = g_manager->get_category_data(db_uid->category_name.c_str());
        if (category) {
            ref_count = 1;
            M_load_req_category();
        }
    }

    // 0x1401C1A80
    void UidData::M_load_req_category() {
        load_state = S_NOW_LOADING;
        category->load_req(0);
    }

    // 0x1401BF810
    void UidData::M_free() {
        if (load_state == S_NOW_LOADING)
            category->free();
        else if (load_state == S_DONE) {
            file.clear();
            category->free();
        }
        name = 0;
        category = 0;
        load_state = S_BEFORE_LOADING;
    }

    // 0x1401C1710
    void UidData::M_load_is_done() {
        if (load_state != S_NOW_LOADING || !category->load_is_done())
            return;

        std::string file_name(name);
        file_name.append(".a3da");

        size_t size;
        const void* data;
        std::tie(size, data) = category->get_file(file_name);

        time_struct time;
        file.load(data, size);
        time.calc_time();
        load_state = S_DONE;
    }

    // 0x1401B7240
    UidData::UidData() : ref_count(), uid(), load_state(), name(), category() {

    }

    // 0x1401B71B0
    UidData::UidData(const UidData& other) {
        *this = other;
    }

    // 0x1401B9ED0
    UidData::~UidData() {

    }

    // 0x1401E1040
    void UidData::load_req(const auth_3d_database* auth_3d_db) {
        if (ref_count)
            ref_count++;
        else if (load_state == S_BEFORE_LOADING)
            M_load_req(auth_3d_db);
    }

    // 0x1401D3B30
    void UidData::free() {
        if (--ref_count < 0)
            ref_count = 0;
        else if (!ref_count)
            M_free();
    }

    // 0x1401E06E0
    bool UidData::load_is_done() {
        M_load_is_done();
        return load_state == S_DONE;
    }

    // 0x1401BD3F0
    UidData& UidData::operator=(const UidData& other) {
        ref_count = other.ref_count;
        uid = other.uid;
        file_name.assign(file_name);
        load_state = other.load_state;
        file = other.file;
        name = other.name;
        category = other.category;
        return *this;
    }

    // Added
    void UidDataModern::M_load_req() {
        load_state = S_DONE;

        category = g_manager->get_category_data_modern(hash);
        if (category) {
            file_name.assign(category->ar->get_file_name(hash));
            ref_count = 1;
            M_load_req_category();
        }
    }

    // Added
    void UidDataModern::M_load_req_category() {
        load_state = S_NOW_LOADING;
        category->load_req(0);
    }

    // Added
    void UidDataModern::M_free() {
        if (load_state == S_NOW_LOADING)
            category->free();
        else if (load_state == S_DONE) {
            file.clear();
            category->free();
        }
        name.clear();
        name.shrink_to_fit();
        category = 0;
        load_state = S_BEFORE_LOADING;
        g_manager->uid_data_list_modern.erase(hash);
    }

    // Added
    void UidDataModern::M_load_is_done() {
        if (load_state != S_NOW_LOADING || !category->load_is_done())
            return;

        size_t size;
        const void* data;
        const char* name;
        std::tie(size, data, name) = category->get_file(hash);

        time_struct time;
        file_name.assign(name);
        file.load(data, size);
        time.calc_time();
        load_state = S_DONE;
    }

    // Added
    UidDataModern::UidDataModern() : ref_count(),
        load_state(), name(), category(), data(), obj_db(), tex_db() {
        hash = hash_murmurhash_empty;
    }

    // Added
    UidDataModern::UidDataModern(const UidDataModern& other) {
        *this = other;
    }

    // Added
    UidDataModern::~UidDataModern() {

    }

    // Added
    void UidDataModern::load_req() {
        if (ref_count)
            ref_count++;
        else if (load_state == S_BEFORE_LOADING)
            M_load_req();
    }

    // Added
    void UidDataModern::free() {
        if (--ref_count < 0)
            ref_count = 0;
        else if (!ref_count)
            M_free();
    }

    // Added
    bool UidDataModern::load_is_done() {
        M_load_is_done();
        return load_state == S_DONE;
    }

    // Added
    UidDataModern& UidDataModern::operator=(const UidDataModern& other) {
        ref_count = other.ref_count;
        hash = other.hash;
        file_name.assign(file_name);
        load_state = other.load_state;
        file = other.file;
        name.assign(name);
        category = other.category;
        data = other.data;
        obj_db = other.obj_db;
        tex_db = other.tex_db;
        return *this;
    }

    // 0x1401B60B0
    Scene::Scene() : M_uid(), my_handle(), M_is_enabled(), M_camera_is_enabled(),
        M_is_visible(), M_is_looped(), M_is_start(), M_is_finished(), M_is_reverse_side(),
        M_is_auto_disable(), trnsl(), trnsl_state(), M_is_attribute_wall(),  M_assign_rob_id(),
        M_ex_node_mat(), M_shadow(), M_chara_item(), M_reflect(), M_cn_src(), M_cn_dst(),
        M_debug(), M_data(), M_frc(), M_frame(), frame_req(), frame_max(), is_requested_frame(),
        M_frame_loop_begin(), M_frame_loop_end(), M_fov_adjust(), object_hrc_blend_list(),
        object_hrc_blend(), now_pause(), event_time_unread_ptr() {
        hash = hash_murmurhash_empty;
        init();
    }

    // 0x1401B93F0
    Scene::~Scene() {
        init();
    }

    // 0x1401D79C0
    void Scene::init() {
        M_uid = -1;
        my_handle = -1;
        M_is_enabled = false;
        M_camera_is_enabled = true;
        M_is_visible = true;
        M_is_looped = false;
        M_is_start = false;
        M_is_finished = false;
        M_is_reverse_side = false;
        M_is_auto_disable = false;

        base_matrix = mat4_identity;
        trnsl = 1.0f;
        trnsl_state = mdl::OBJ_ALPHA_ORDER_POST_GLITTER;

        M_is_attribute_wall = false;

        item_attr_map.clear();
        item_texchg_list.clear();
        item_texchg_list.shrink_to_fit();

        M_assign_rob_id = ROB_ID_NULL;
        M_ex_node_mat = 0;
        M_obj_uid = {};

        M_shadow = false;
        M_chara_item = false;
        M_reflect = false;

        M_cn_src = CN_MAX;
        M_cn_dst = CN_MAX;
        M_debug = 0;
        M_data = 0;

        M_frc = get_sys_frame_rate();
        M_frame = 0.0f;
        frame_req = 0.0f;
        frame_max = -1.0f;

        is_requested_frame = true;

        M_frame_loop_begin = 0.0f;
        M_frame_loop_end = 0.0f;
        M_fov_adjust = 1.0f;

        object_hrc_blend_list = 0;
        object_hrc_blend = 0.0f;
        now_pause = true;

        for (Event*& i : event_list)
            if (i) {
                delete i;
                i = 0;
            }

        event_list.clear();
        event_list.shrink_to_fit();
        event_time_list.clear();
        event_time_list.shrink_to_fit();
        event_time_unread_ptr = event_time_list.begin();

        play_control.init();

        point_list.clear();
        point_list.shrink_to_fit();
        curve_list.clear();
        curve_list.shrink_to_fit();

        camera_root_list.clear();
        camera_root_list.shrink_to_fit();
        camera_auxiliary.init();

        light_list.clear();
        light_list.shrink_to_fit();

        // MGF
        ambient_list.clear();
        ambient_list.shrink_to_fit();

        fog_list.clear();
        fog_list.shrink_to_fit();

        post_process.init();
        dof.init();

        chara_list.clear();
        chara_list.shrink_to_fit();
        motion_list.clear();
        motion_list.shrink_to_fit();

        object_holder.clear();
        object_holder.shrink_to_fit();
        object_list.clear();
        object_list.shrink_to_fit();
        object_hrc_holder.clear();
        object_hrc_holder.shrink_to_fit();
        object_hrc_list.clear();
        object_hrc_list.shrink_to_fit();
        m_object_hrc_holder.clear();
        m_object_hrc_holder.shrink_to_fit();
        m_object_hrc_list.clear();
        m_object_hrc_list.shrink_to_fit();

        adx_sound_list.clear();
        adx_sound_list.shrink_to_fit();

        // X/XHD
        material_list.clear();
        material_list.shrink_to_fit();

        object_name_list.clear();
        object_name_list.shrink_to_fit();
        object_hrc_name_list.clear();
        object_hrc_name_list.shrink_to_fit();
        m_object_hrc_name_list.clear();
        m_object_hrc_name_list.shrink_to_fit();

        auth_2d_list.clear();
        auth_2d_list.shrink_to_fit();

        load_state = S_BEFORE_LOADING;

        my_event_listener.clear();
        my_event_listener.shrink_to_fit();

        file_name.clear();
        file_name.shrink_to_fit();
        hash = hash_murmurhash_empty;
        obj_db = 0;
        tex_db = 0;
    }

    // 0x1401D01C0
    void Scene::destroy(render_context* rctx) {
        camera_auxiliary.dest(rctx);

        for (Light& i : light_list)
            i.dest(rctx);

        for (Fog& i : fog_list)
            i.dest(rctx);

        dof.dest(rctx);
        post_process.dest(rctx);

        for (Event* i : event_list)
            if (i->active) {
                i->active = false;
                i->exit(*this, EAF_DESTROY, rctx);
            }

        if (load_state != S_BEFORE_LOADING)
            if (hash == hash_murmurhash_empty) {
                data_struct* aft_data = &data_list[DATA_AFT];
                auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;
                g_manager->uid_free(M_uid, aft_auth_3d_db);
            }
            else
                g_manager->uid_free_modern(hash);
        init();
    }

    // 0x1401D6310
    int32_t Scene::get_uid() const {
        return M_uid;
    }

    // 0x1401CF110
    void Scene::ctrl(render_context* rctx) {
        M_load_exec();

        if (load_state != S_DONE || !M_is_enabled)
            return;

        EventActionFlag eaf = (EventActionFlag)0;
        bool set = false;
        while (true) {
            bool is_start = M_is_start;
            if (is_requested_frame) {
                is_requested_frame = false;
                if (frame_req < M_frame) {
                    event_time_unread_ptr = event_time_list.begin();
                    enum_or(eaf, EAF_JUMP_BACKWARD);
                }
                else
                    enum_or(eaf, EAF_JUMP_FORWARD);
                M_frame = frame_req;
                M_is_start = true;
                M_is_finished = false;
                if (frame_max >= 0.0f && M_frame >= frame_max)
                    frame_max = -1.0f;
            }
            else if (!now_pause) {
                enum_or(eaf, EAF_PASS);
                M_frame += M_frc->get_delta_frame();
                if (frame_max >= 0.0f && M_frame > frame_max)
                    M_frame = frame_max;
            }

            float_t frame = M_frame;
            for (Point& i : point_list)
                i.get(frame);

            for (Curve& i : curve_list)
                i.get(frame);

            for (Chara& i : chara_list)
                i.get(frame);

            for (Object& i : object_holder)
                i.get(frame);

            for (Object*& i : object_list)
                i->ctrl(base_matrix);

            for (HierarchyObject& i : object_hrc_holder)
                i.get(frame);

            for (HierarchyObject*& i : object_hrc_list)
                i->ctrl(base_matrix);

            for (MultiHierarchyObject& i : m_object_hrc_holder)
                i.get(frame);

            for (MultiHierarchyObject*& i : m_object_hrc_list)
                i->ctrl(base_matrix);

            // X/XHD
            for (MaterialList& i : material_list)
                i.get(frame);

            for (Light& i : light_list) {
                i.get(frame);
                i.ctrl(rctx);
            }

            // MGF
            for (Ambient& i : ambient_list) {
                i.get(frame);
                i.ctrl(rctx);
            }

            for (Fog& i : fog_list) {
                i.get(frame);
                i.ctrl(rctx);
            }

            post_process.get(frame);
            post_process.ctrl(*this, rctx);

            for (Chara& i : chara_list)
                i.ctrl(*this);

            auto i_event = event_time_unread_ptr;
            auto i_event_end = event_time_list.end();
            for (; i_event != i_event_end && frame >= i_event->frame; i_event++) {
                Event* e = event_list[i_event->event_index];
                if (!i_event->type) {
                    event_log("  [%s]: [%s][%s]\n",
                        get_ID_EVENT_TYPE(e->type), e->ref.c_str(), e->param1.c_str());
                    e->entry(*this, eaf, rctx);
                    e->active = true;
                }
                else if (i_event->type) {
                    event_log("  [%s]: [%s][%s]\n",
                        get_ID_EVENT_TYPE(e->type), e->ref.c_str(), e->param1.c_str());
                    if (e->active) {
                        e->active = false;
                        e->exit(*this, eaf, rctx);
                    }
                }
            }
            event_time_unread_ptr = i_event;

            for (Event* i : event_list)
                if (i->active)
                    i->do_ctrl(*this, eaf, rctx);

            camera_auxiliary.get(frame);
            camera_auxiliary.ctrl(*this, rctx);

            if (M_camera_is_enabled)
                for (CameraRoot& i : camera_root_list) {
                    CameraParam cam = i.get(frame, *this, rctx);

                    if (M_is_reverse_side) {
                        cam.interest.x = -cam.interest.x;
                        cam.view_point.x = -cam.view_point.x;
                        cam.roll = -cam.roll;
                    }

                    mat4_transform_point(&base_matrix, &cam.interest, &cam.interest);
                    mat4_transform_point(&base_matrix, &cam.view_point, &cam.view_point);

                    cam.set(rctx->camera);
                    break;
                }

            dof.get(frame);
            dof.ctrl(rctx);

            if (M_is_start && is_start != M_is_start)
                for (a3d::EventListener*& i : my_event_listener) {
                    Handle handle = to_handle();
                    i->playStart(handle);
                }

            if (set || !M_is_looped || M_frame_loop_end > frame)
                break;

            for (a3d::EventListener*& i : my_event_listener) {
                Handle handle = to_handle();
                i->playLoopJumpBefore(handle);
            }

            frame_req = frame - M_frame_loop_end + M_frame_loop_begin;
            is_requested_frame = true;
            set = true;
        }

        for (a3d::EventListener*& i : my_event_listener) {
            Handle handle = to_handle();
            i->playLoopJumpAfter(handle);
        }

        bool is_finished = play_control.size <= M_frame;
        if (is_finished && M_is_finished != is_finished) {
            now_pause = true;
            if (M_is_auto_disable)
                M_is_enabled = false;

            for (a3d::EventListener*& i : my_event_listener) {
                Handle handle = to_handle();
                i->playFinished(handle);
            }
        }
        M_is_finished = is_finished;
    }

    // 0x1401D1230
    void Scene::disp(render_context* rctx) {
        if (load_state != S_DONE || !M_is_enabled)
            return;

        if (M_debug)
            debug_put_line_axis(base_matrix);

        // X/XHD
        auth_3d_set_material_list(*this, rctx);
        extern bool reflect_full;
        extern bool reflect_draw;
        reflect_draw = reflect_full && M_reflect;

        for (Point& i : point_list)
            i.disp(*this, rctx);

        for (Chara& i : chara_list)
            i.disp(*this, base_matrix, rctx);

        for (Object*& i : object_list)
            i->disp(*this, rctx);

        for (HierarchyObject*& i : object_hrc_list)
            i->disp(*this, rctx);

        for (MultiHierarchyObject*& i : m_object_hrc_list)
            i->disp(*this, rctx);

        for (Event* i : event_list)
            if (i->active)
                i->do_disp(*this, base_matrix, rctx);

        reflect_draw = false;
        rctx->disp_manager->set_material_list();
    }

    // 0x1401E0FC0
    void Scene::load_req(const auth_3d_database* auth_3d_db) {
        if (g_manager->uid_load_req(M_uid, auth_3d_db))
            load_state = S_NOW_LOADING;
    }

    // Added
    void Scene::load_req_modern() {
        if (g_manager->uid_load_req_modern(hash))
            load_state = S_NOW_LOADING;
    }

    // 0x1401C1AE0
    void Scene::M_put_obj(const mat4& in_mat, object_info in_id, render_context* rctx) const {
        if (trnsl < 0.999f)
            rctx->disp_manager->entry_obj_by_object_info(in_mat, in_id, trnsl);
        else
            rctx->disp_manager->entry_obj_by_object_info(in_mat, in_id);
    }

    // 0x1401E06D0
    bool Scene::load_is_done() const {
        return load_state == S_DONE;
    }

    // 0x1401C92A0
    void Scene::add_event_listener(a3d::EventListener* in_evt_list) {
        my_event_listener.push_back(in_evt_list);
    }

    // 0x1401E3490
    void Scene::remove_event_listener(a3d::EventListener* in_evt_list) {
        prj::find_and_erase(my_event_listener, in_evt_list);
    }

    // 0x1401BF9C0
    void Scene::M_loadFromProperties(SceneFile& in_file,
        const object_database* in_obj_db, const texture_database* in_tex_db) {
        a3d::Scene& prop = in_file.prop;

        hash = hash_murmurhash_empty;
        play_control.load(in_file, prop.play_control);

        in_file.frame_max = play_control.size;
        frame_max = play_control.size;

        M_frame_loop_begin = 0.0f;
        M_frame_loop_end = play_control.size;

        obj_db = in_obj_db;
        tex_db = in_tex_db;

        list_load(point_list, in_file, prop.point);
        list_load(curve_list, in_file, prop.curve);
        list_load(camera_root_list, in_file, prop.camera_root);

        camera_auxiliary.load(in_file, prop.camera_auxiliary);

        list_load(light_list, in_file, prop.light);

        // MGF
        list_load(ambient_list, in_file, prop.ambient);

        list_load(fog_list, in_file, prop.fog);

        post_process.load(in_file, prop.post_process);
        dof.load(in_file, prop.dof);

        list_load(chara_list, in_file, prop.chara);
        list_load(motion_list, in_file, prop.motion);
        list_load(auth_2d_list, in_file, prop.auth_2d);

        list_load(object_holder, in_file, prop.object, obj_db, tex_db);

        size_t object_index = 0;
        for (Object& i : object_holder) {
            i.replace_chara(M_cn_src, M_cn_dst, obj_db);
            i.index = object_index++;
        }

        if (prop.object_list.size())
            object_name_list.assign(prop.object_list.begin(), prop.object_list.end());

        for (Object& i : object_holder) {
            if (i.obj_pat.name.size()) {
                size_t index = get_curve_index(i.obj_pat.name);
                if (index != -1)
                    i.obj_pat.bind(curve_list[index]);
            }

            if (i.obj_morph.name.size()) {
                size_t index = get_curve_index(i.obj_morph.name);
                if (index != -1)
                    i.obj_morph.bind(curve_list[index]);
            }

            for (Texture& j : i.texture_list)
                if (j.tex_pat.name.size()) {
                    size_t index = get_curve_index(j.tex_pat.name);
                    if (index != -1)
                        j.tex_pat.bind(curve_list[index]);
                }

            i.prepare_morph(obj_db);
        }

        object_list.reserve(object_name_list.size());
        for (const std::string& i : object_name_list)
            object_list.push_back(&object_holder[get_object_index(i)]);

        list_load(object_hrc_holder, in_file, prop.object_hrc, obj_db);

        for (HierarchyObject& i : object_hrc_holder) {
            i.ex_node_mat = M_ex_node_mat;
            bool v45 = i.replace_chara(M_cn_src, M_cn_dst, obj_db);
            object_info obj_uid;
            if (!v45 && !i.parent_name.size())
                obj_uid = M_obj_uid;
            i.set_obj_uid(obj_uid);
        }

        std::string chara_str;
        if (M_assign_rob_id >= 0 || M_assign_rob_id < ROB_ID_MAX) {
            std::string char_str = get_char_id_str(get_rob_management()->get_chara((ROB_ID)M_assign_rob_id));
            chara_str.assign("_" + char_str);
        }
        else
            chara_str.assign("____");

        for (HierarchyObject& i : object_hrc_holder)
            i.check_rob_chara(chara_str);

        if (prop.object_hrc_list.size())
            object_hrc_name_list.assign(prop.object_hrc_list.begin(), prop.object_hrc_list.end());

        object_hrc_list.reserve(object_hrc_name_list.size());
        for (const std::string& i : object_hrc_name_list) {
            size_t index = get_object_hrc_index(i);
            if (index != -1)
                object_hrc_list.push_back(&object_hrc_holder[index]);
        }

        list_load(m_object_hrc_holder, in_file, prop.m_object_hrc, obj_db);

        if (prop.m_object_hrc_list.size())
            m_object_hrc_name_list.assign(prop.m_object_hrc_list.begin(), prop.m_object_hrc_list.end());

        m_object_hrc_list.reserve(m_object_hrc_name_list.size());
        for (const std::string& i : m_object_hrc_name_list) {
            size_t index = get_m_object_hrc_index(i);
            if (index != -1)
                m_object_hrc_list.push_back(&m_object_hrc_holder[index]);
        }

        if (object_holder.size()) {
            for (Object& i : object_holder) {
                if (!i.parent_name.size())
                    continue;

                if (i.parent_node.size())
                    for (HierarchyObject& j : object_hrc_holder) {
                        if (i.parent_name.compare(j.name))
                            continue;

                        int32_t node_index = j.query_node_index(i.parent_node.c_str());
                        if (node_index >= 0)
                            j.add_child_object(node_index, &i);
                        break;
                    }
                else
                    for (Object& j : object_holder) {
                        if (i.parent_name.compare(j.name))
                            continue;

                        j.child_object_list.push_back(&i);
                        break;
                    }
            }
        }

        if (object_hrc_holder.size()) {
            for (HierarchyObject& i : object_hrc_holder) {
                if (!i.parent_name.size())
                    continue;

                if (i.parent_node.size())
                    for (HierarchyObject& j : object_hrc_holder) {
                        if (i.parent_name.compare(j.name))
                            continue;

                        int32_t node_index = j.query_node_index(i.parent_node.c_str());
                        if (node_index >= 0)
                            j.add_child_object(node_index, &i);
                        break;
                    }
                else
                    for (Object& j : object_holder) {
                        if (i.parent_name.compare(j.name))
                            continue;

                        j.child_object_hrc_list.push_back(&i);
                        break;
                    }
            }
        }

        adx_sound_list.clear();

        // X/XHD
        list_load(material_list, in_file, prop.material_list);

        if (prop.event.size()) {
            event_list.reserve(prop.event.size());

            for (const a3d::Event& i : prop.event) {
                Event* e = createEvent(in_file, i);
                if (e) {
                    event_list.push_back(e);

                    if (e->type == a3d::EVENT_SND) {
                        std::string param1(e->param1);
                        if (param1.find("_adx") == param1.size() - 4)
                            adx_sound_list.push_back(param1);
                    }
                }
            }

            if (adx_sound_list.size()) {
                std::string path("rom/sound/voice/auth_voice/");
                path.append(adx_sound_list.front());
                path.append(".adx");
                sound_work_play_stream(2, path.c_str(), true);
            }

            for (Event* i : event_list)
                if (i)
                    i->resolve_relation(*this);
        }

        event_time_list.clear();
        event_time_list.shrink_to_fit();

        if (event_list.size()) {
            size_t event_index = 0;
            event_time_list.reserve(event_list.size() * 2);
            for (Event* i : event_list) {
                event_time_list.push_back({ i->begin, 0, event_index });
                event_time_list.push_back({ i->end  , 1, event_index });
                event_index++;
            }

            std::sort(event_time_list.begin(), event_time_list.end(),
                [](const TimeEvent& left, const TimeEvent& right) {
                    return left.frame < right.frame;
                });
        }

        event_time_unread_ptr = event_time_list.begin();

        file_name.assign(prop._file_name);
    }

    // Added
    void Scene::M_storeToProperties(SceneFile& in_file) const {
        a3d::Scene& prop = in_file.prop;

        play_control.store(in_file, prop.play_control);

        list_store(point_list, in_file, prop.point);
        list_store(curve_list, in_file, prop.curve);
        list_store(camera_root_list, in_file, prop.camera_root);

        camera_auxiliary.store(in_file, prop.camera_auxiliary);

        list_store(light_list, in_file, prop.light);

        // MGF
        list_store(ambient_list, in_file, prop.ambient);

        list_store(fog_list, in_file, prop.fog);

        post_process.store(in_file, prop.post_process);
        dof.store(in_file, prop.dof);

        list_store(chara_list, in_file, prop.chara);
        list_store(motion_list, in_file, prop.motion);
        list_store(auth_2d_list, in_file, prop.auth_2d);

        list_store(object_holder, in_file, prop.object);

        if (object_name_list.size())
            prop.object_list.assign(object_name_list.begin(), object_name_list.end());

        list_store(object_hrc_holder, in_file, prop.object_hrc);

        if (object_hrc_name_list.size())
            prop.object_hrc_list.assign(
                object_hrc_name_list.begin(), object_hrc_name_list.end());

        list_store(m_object_hrc_holder, in_file, prop.m_object_hrc);

        if (m_object_hrc_name_list.size())
            prop.m_object_hrc_list.assign(
                m_object_hrc_name_list.begin(), m_object_hrc_name_list.end());

        // X/XHD
        list_store(material_list, in_file, prop.material_list);

        if (event_list.size()) {
            size_t event_index = 0;
            prop.motion.resize(event_list.size());
            for (const Event* i : event_list)
                i->store(in_file, prop.event[event_index++]);
        }
    }

    // 0x1401D8C10
    inline bool Scene::is_valid() const {
        return my_handle != -1;
    }

    // 0x1401E6730
    void Scene::set_frame_rate_control(FrameRateControl* in_value) {
        if (in_value)
            M_frc = in_value;
        else
            M_frc = get_sys_frame_rate();
    }

    // 0x1401D58D0
    float_t Scene::get_frame_speed() const {
        return M_frc->get_delta_frame();
    }

    // 0x1401E67B0
    void Scene::set_frame_req(float_t in_frame) {
        frame_req = in_frame;
        is_requested_frame = true;
    }

    void Scene::set_frame_max(float_t in_frame) {
        frame_max = in_frame;
    }

    float_t Scene::get_frame() const {
        return M_frame;
    }

    float_t Scene::get_frame_size() const {
        return play_control.size;
    }

    float_t Scene::get_frame_fps() const {
        return play_control.fps;
    }

    float_t Scene::get_frame_begin() const {
        return play_control.begin;
    }

    // 0x1401E6A50
    void Scene::set_pause(bool in_value) {
        now_pause = in_value;
    }

    // 0x1401D5DD0
    bool Scene::get_pause() const {
        return now_pause;
    }

    // 0x1401E6460
    void Scene::set_enabled(bool in_value) {
        M_is_enabled = in_value;
    }

    // 0x1401D55C0
    bool Scene::get_enabled() const {
        return M_is_enabled;
    }

    // 0x1401E6BC0
    void Scene::set_visible(bool in_value) {
        M_is_visible = in_value;
    }

    // 0x1401D64E0
    bool Scene::get_visible() const {
        return M_is_visible;
    }

    // 0x1401E6990
    void Scene::set_looped(bool in_value) {
        M_is_looped = in_value;
    }

    // 0x1401D59D0
    bool Scene::get_looped() const {
        return M_is_looped;
    }

    // 0x1401E65F0
    void Scene::set_frame_loop_begin(float_t in_frame) {
        M_frame_loop_begin = in_frame;
    }

    // 0x1401E6600
    void Scene::set_frame_loop_begin() {
        M_frame_loop_begin = 0.0f;
    }

    // 0x1401D5800
    float_t Scene::get_frame_loop_begin() const {
        return M_frame_loop_begin;
    }

    // 0x1401E6690
    void Scene::set_frame_loop_end(float_t in_frame) {
        M_frame_loop_end = in_frame;
    }

    // 0x1401E66A0
    void Scene::set_frame_loop_end() {
        M_frame_loop_end = play_control.size;
    }

    // 0x1401D5850
    float_t Scene::get_frame_loop_end() const {
        return M_frame_loop_end;
    }

    // 0x1401D8AF0
    bool Scene::is_finished() const {
        return M_is_finished;
    }

    // 0x1401E6AC0
    void Scene::set_reverse_side(bool in_value) {
        M_is_reverse_side = in_value;
    }

    // 0x1401D5EA0
    bool Scene::get_reverse_side() const {
        return M_is_reverse_side;
    }

    // 0x1401E6320
    void Scene::set_auto_disable(bool in_value) {
        M_is_auto_disable = in_value;
    }

    // 0x1401D51F0
    bool Scene::get_auto_disable() const {
        return M_is_auto_disable;
    }

    // 0x1401E6370
    void Scene::set_base_matrix(const mat4& in_value) {
        base_matrix = in_value;
    }

    // 0x1401D5280
    const mat4& Scene::get_base_matrix() const {
        return base_matrix;
    }

    // 0x1401E6B60
    void Scene::set_trnsl(float_t in_value, mdl::ObjFlags in_state) {
        trnsl = in_value;
        trnsl_state = in_state;
    }

    // 0x1401D6110
    float_t Scene::get_trnsl() const {
        return trnsl;
    }

    // 0x1401E62D0
    void Scene::set_attribute_wall(bool in_value) {
        M_is_attribute_wall = in_value;
    }

    // 0x1401D50E0
    bool Scene::get_attribute_wall() const {
        return M_is_attribute_wall;
    }

    // 0x1401E6860
    void Scene::set_item_attr(uint32_t in_id, uint32_t in_attr) {
        item_attr_map.insert({ in_id, in_attr });
    }

    // Missing
    uint32_t Scene::get_item_attr(uint32_t in_id) const {
        auto elem = item_attr_map.find(in_id);
        if (elem != item_attr_map.end())
            return elem->second;
        return 0;
    }

    // 0x1401E68E0
    void Scene::set_item_texchange(const std::vector<ItemTexChange>& in_list) {
        item_texchg_list.assign(in_list.begin(), in_list.end());
    }

    // Missing
    const std::vector<ItemTexChange>& Scene::get_item_texchange() const {
        return item_texchg_list;
    }

    // 0x1401D5480
    inline size_t Scene::get_chara_size() const {
        return chara_list.size();
    }

    // 0x1401D5400
    inline size_t Scene::get_chara_index(const std::string& in_name) const {
        return get_index_by_name(chara_list, in_name);
    }

    // 0x1401CBF60
    const std::string* Scene::chara_get_name(size_t in_index) const {
        if (in_index < chara_list.size())
            return &chara_list[in_index].name;
        return 0;
    }

    // Inlined
    void Scene::chara_bind_rob(size_t in_index, int32_t in_rob_id) {
        if (in_index < chara_list.size()
            && (in_rob_id == ROB_ID_NULL || in_rob_id == ROB_ID_1P || in_rob_id == ROB_ID_2P)
            && chara_list[in_index].rob_id != in_rob_id)
                chara_list[in_index].rob_id = in_rob_id;
    }

    // 0x1401D5C80
    size_t Scene::get_object_size() const {
        return object_holder.size();
    }

    // 0x1401D5AD0
    size_t Scene::get_object_index(const std::string& in_name) const {
        return get_index_by_name(object_holder, in_name);
    }

    // 0x1401D5AE0
    int32_t Scene::get_object_index(object_info in_obj_uid, int32_t in_instance) const {
        if (in_instance >= 0) {
            int32_t index = 0;
            int32_t instance = 0;
            for (auto i = object_holder.cbegin(); i != object_holder.cend(); i++, index++)
                if (in_obj_uid == i->obj_uid) {
                    if (in_instance == instance)
                        return index;
                    instance++;
                }
            return -1;
        }

        int32_t index = 0;
        for (auto i = object_holder.cbegin(); i != object_holder.cend(); i++, index++)
            if (in_obj_uid == i->obj_uid)
                return index;
        return -1;
    }

    // Added
    int32_t Scene::get_object_index(uint32_t in_obj_hash, int32_t in_instance) const {
        if (in_instance >= 0) {
            int32_t index = 0;
            int32_t instance = 0;
            for (auto i = object_holder.cbegin(); i != object_holder.cend(); i++, index++)
                if (in_obj_hash == i->obj_hash) {
                    if (in_instance == instance)
                        return index;
                    instance++;
                }
            return -1;
        }

        int32_t index = 0;
        for (auto i = object_holder.cbegin(); i != object_holder.cend(); i++, index++)
            if (in_obj_hash == i->obj_hash)
                return index;
        return -1;
    }

    // Inlined
    const std::string* Scene::object_get_name(size_t in_index) const {
        if (in_index < object_holder.size())
            return &object_holder[in_index].name;
        return 0;
    }

    // 0x1401D5B70
    const mat4* Scene::get_object_matrix(size_t in_index) const {
        if (in_index < object_holder.size())
            return &object_holder[in_index].model_transform.matrix;
        return 0;
    }

    // Inlined
    const mat4* Scene::get_object_matrix(size_t in_index, bool in_is_hrc) const {
        if (in_is_hrc)
            return get_object_hrc_matrix(in_index);
        return get_object_matrix(in_index);
    }

    // Inlined
    inline size_t Scene::get_object_hrc_size() const {
        return object_hrc_holder.size();
    }

    // 0x0x1401D5CB0
    size_t Scene::get_object_hrc_index(const std::string& in_name) const {
        return get_index_by_name(object_hrc_holder, in_name);
    }

    // 0x1401D5CC0
    int32_t Scene::get_object_hrc_index(object_info in_obj_uid, int32_t in_instance) const {
        if (in_instance >= 0) {
            int32_t index = 0;
            int32_t instance = 0;
            for (auto i = object_hrc_holder.cbegin(); i != object_hrc_holder.cend(); i++, index++)
                if (in_obj_uid == i->obj_uid) {
                    if (in_instance == instance)
                        return index;
                    instance++;
                }
            return -1;
        }

        int32_t index = 0;
        for (auto i = object_hrc_holder.cbegin(); i != object_hrc_holder.cend(); i++, index++)
            if (in_obj_uid == i->obj_uid)
                return index;
        return -1;
    }

    // Added
    int32_t Scene::get_object_hrc_index(uint32_t in_obj_hash, int32_t in_instance) const {
        if (in_instance >= 0) {
            int32_t index = 0;
            int32_t instance = 0;
            for (auto i = object_hrc_holder.cbegin(); i != object_hrc_holder.cend(); i++, index++)
                if (in_obj_hash == i->obj_hash) {
                    if (in_instance == instance)
                        return index;
                    instance++;
                }
            return -1;
        }

        int32_t index = 0;
        for (auto i = object_hrc_holder.cbegin(); i != object_hrc_holder.cend(); i++, index++)
            if (in_obj_hash == i->obj_hash)
                return index;
        return -1;
    }

    // Missing
    const std::string* Scene::object_hrc_get_name(size_t in_index) const {
        if (in_index < object_hrc_holder.size())
            return &object_hrc_holder[in_index].name;
        return 0;
    }

    // 0x1401D5D00
    const mat4* Scene::get_object_hrc_matrix(size_t in_index) const {
        if (in_index < object_hrc_holder.size()) {
            const HierarchyObject& hrc_obj = object_hrc_holder[in_index];
            if (hrc_obj.mats.size())
                return &hrc_obj.mats[0];
        }
        return 0;
    }

    // Inlined
    inline size_t Scene::get_m_object_hrc_size() const {
        return m_object_hrc_holder.size();
    }

    // 0x1401D5A70
    size_t Scene::get_m_object_hrc_index(const std::string& in_name) const {
        return get_index_by_name(m_object_hrc_holder, in_name);
    }

    // Missing
    const std::string* Scene::m_object_hrc_get_name(size_t in_index) const {
        if (in_index < m_object_hrc_holder.size())
            return &m_object_hrc_holder[in_index].name;
        return 0;
    }

    // 0x1401CB5C0
    void Scene::camera_set_enabled(bool in_value) {
        M_camera_is_enabled = in_value;
    }

    // 0x1401CB510
    bool Scene::camera_get_enabled() const {
        return M_camera_is_enabled;
    }

    // 0x1401CB610
    void Scene::camera_set_fov_adjust(float_t in_value) {
        M_fov_adjust = in_value;
    }

    // 0x1401CB570
    float_t Scene::camera_get_fov_adjust() const {
        return M_fov_adjust;
    }

    // 0x1401D5190
    size_t Scene::get_auth_2d_size() const {
        return auth_2d_list.size();
    }

    // 0x1401D5140
    size_t Scene::get_auth_2d_index(const std::string& in_name) const {
        return get_index_by_name(auth_2d_list, in_name);
    }

    // 0x1401CA890
    const std::string* Scene::auth_2d_get_name(size_t in_index) const {
        if (in_index < get_auth_2d_size())
            return &auth_2d_list[in_index].name;
        return 0;
    }

    // 0x1401D5DE0
    size_t Scene::get_point_index(const std::string& in_name) const {
        return get_index_by_name(point_list, in_name);
    }

    // 0x1401D54B0
    size_t Scene::get_curve_index(const std::string& in_name) const {
        return get_index_by_name(curve_list, in_name);
    }

    // Missing
    void Scene::set_data(size_t in_value) {
        M_data = in_value;
    }

    // Missing
    size_t Scene::get_data() const {
        return M_data;
    }

    // 0x1401E63D0
    void Scene::set_debug(uint32_t in_value) {
        M_debug = in_value;
    }

    // 0x1401D5570
    uint32_t Scene::get_debug() const {
        return M_debug;
    }

    // 0x1401CA880
    void Scene::assign_rob_id(int32_t in_value) {
        M_assign_rob_id = in_value;
    }

    // 0x1401D5090
    int32_t Scene::get_assign_rob_id() const {
        return M_assign_rob_id;
    }

    // 0x1401E6A00
    void Scene::set_obj_uid(object_info in_value) {
        M_obj_uid = in_value;
    }

    // 0x1401E6B10
    void Scene::set_shadow(bool in_value) {
        M_shadow = in_value;
    }

    // 0x1401D8B00
    bool Scene::is_shadow() const {
        return M_shadow;
    }

    // 0x1401E3B70
    void Scene::replace_chara(int32_t in_cn_src, int32_t in_cn_dst) {
        M_cn_src = in_cn_src;
        M_cn_dst = in_cn_dst;
    }

    // 0x1401E64D0
    void Scene::set_frame_object_hrc(float_t in_frame) {
        M_is_start = true;
        now_pause = true;
        M_is_finished = !(in_frame < play_control.size);
        M_frame = in_frame;

        if (frame_max >= 0.0f && frame_max <= in_frame)
            frame_max = -1.0f;
    }

    // 0x1401E68F0
    void Scene::set_object_hrc_blend(const Scene * in_scene, float_t in_value) {
        if (in_scene && object_hrc_list.size()
            && object_hrc_list.size() == in_scene->object_hrc_list.size()) {
            object_hrc_blend_list = &in_scene->object_hrc_list;
            object_hrc_blend = in_value;
        }
        else {
            object_hrc_blend_list = 0;
            object_hrc_blend = 0.0f;
        }
    }

    // Inlined
    inline void Scene::calc_matrix_hierarchy(const mat4& in_value) {
        base_matrix = in_value;
        calc_matrix_hierarchy_mul();
    }

    // 0x1401CB250
    void Scene::calc_matrix_hierarchy_get() {
        if (M_ex_node_mat)
            for (HierarchyObject*& i : object_hrc_list)
                i->calc_matrix_hierarchy(base_matrix);
    }

    // 0x1401CB2B0
    void Scene::calc_matrix_hierarchy_mul() {
        if (M_ex_node_mat)
            for (HierarchyObject*& i : object_hrc_list)
                i->calc_matrix_hierarchy(base_matrix);
    }

    // 0x1401D91C0
    void Scene::join() {
        if (!M_ex_node_mat || !object_hrc_blend_list
            || object_hrc_list.size() != object_hrc_blend_list->size())
            return;

        float_t blend = object_hrc_blend;
        float_t inv_blend = 1.0f - object_hrc_blend;
        for (size_t i = 0; i < object_hrc_list.size(); i++)
            object_hrc_list[i]->join(*(*object_hrc_blend_list)[i], blend, inv_blend);
    }

    // 0x1401D9850
    void Scene::join_lock() {
        if (!M_ex_node_mat || !object_hrc_blend_list
            || object_hrc_list.size() != object_hrc_blend_list->size())
            return;

        float_t blend = object_hrc_blend;
        float_t inv_blend = 1.0f - object_hrc_blend;
        for (size_t i = 0; i < object_hrc_list.size(); i++)
            object_hrc_list[i]->join_lock(*(*object_hrc_blend_list)[i], blend, inv_blend);
    }

    // 0x1401E64B0
    void Scene::set_ex_node_mat(mat4* in_value) {
        M_ex_node_mat = in_value;
    }

    // 0x1401D8AA0
    bool Scene::is_ex_node_mat() const {
        return M_ex_node_mat;
    }

    // 0x1401E1280
    void Scene::lock_list_set() {
        for (HierarchyObject*& i : object_hrc_list)
            i->lock_set();
    }

    // Added
    void Scene::set_chara_item(bool in_value) {
        M_chara_item = in_value;
    }

    // Added
    bool Scene::get_chara_item() const {
        return M_chara_item;
    }

    // Added
    void Scene::set_reflect(bool in_value) {
        M_reflect = in_value;
    }

    // Added
    bool Scene::get_reflect() const {
        return M_reflect;
    }

    // 0x1401B27B0
    Auth3dManager::Auth3dManager() {

    }

    // 0x1401B7DC0
    Auth3dManager::~Auth3dManager() {

    }

    // Inlined
    inline UidData* Auth3dManager::get_uid_data(int32_t in_uid, const auth_3d_database* auth_3d_db) {
        if (in_uid >= auth_3d_db->uid.size())
            return 0;

        const auth_3d_database_uid* db_uid = &auth_3d_db->uid[in_uid];
        if (db_uid && db_uid->enabled) {
            UidData* uid_data = &uid_data_list[in_uid];
            if (uid_data->uid == in_uid)
                return uid_data;
        }
        return 0;
    }

    // Added
    inline UidDataModern* Auth3dManager::get_uid_data_modern(uint32_t hash) {
        auto elem = uid_data_list_modern.find(hash);
        if (elem == uid_data_list_modern.end())
            return 0;

        UidDataModern* uid_data = &elem->second;
        if (uid_data->hash == hash)
            return uid_data;
        return 0;
    }

    // 0x1401CBA80
    void Auth3dManager::category_free(const char* in_name) {
        if (!in_name)
            return;

        CategoryData* category_data = get_category_data(in_name);
        if (category_data)
            category_data->free();
    }

    // Added
    void Auth3dManager::category_free(uint32_t in_hash) {
        if (!in_hash || in_hash == hash_murmurhash_empty)
            return;

        auto elem = category_data_list_modern.find(in_hash);
        if (elem != category_data_list_modern.end()) {
            elem->second.free();
            if (elem->second.ref_count <= 0)
                category_data_list_modern.erase(elem);
        }
    }

    // 0x1401CBDF0
    bool Auth3dManager::category_load_is_done(const char* in_name) {
        if (!in_name)
            return true;

        CategoryData* category_data = get_category_data(in_name);
        if (category_data)
            return category_data->load_is_done();
        return true;
    }

    // Added
    bool Auth3dManager::category_load_is_done(uint32_t in_hash) {
        if (!in_hash || in_hash == hash_murmurhash_empty)
            return true;

        auto elem = category_data_list_modern.find(in_hash);
        if (elem != category_data_list_modern.end())
            return elem->second.load_is_done();
        return true;
    }

    // 0x1401CBE30
    void Auth3dManager::category_load_req(const char* in_name, const char* in_mdata_dir) {
        if (!in_name)
            return;

        CategoryData* category_data = get_category_data(in_name);
        if (category_data)
            category_data->load_req(in_mdata_dir);
    }

    // Added
    void Auth3dManager::category_load_req(void* data, const char* in_name, uint32_t in_hash) {
        if (!in_hash || in_hash == hash_murmurhash_empty)
            return;

        auto elem = category_data_list_modern.find(in_hash);
        if (elem == category_data_list_modern.end()) {
            elem = category_data_list_modern.insert({ in_hash, {} }).first;
            CategoryData* category_data = &elem->second;
            category_data->category_name.assign(in_name);
        }

        CategoryData* category_data = &elem->second;
        if (category_data)
            category_data->load_req_modern(data);
    }

    // 0x1401D5B20
    const mat4* Auth3dManager::get_object_matrix(int32_t in_handle, size_t in_index, bool in_is_hrc) const {
        const Scene* scene = get_work_enabled(in_handle);
        if (scene)
            return scene->get_object_matrix(in_index, in_is_hrc);
        return 0;
    }

    // 0x1401D5A80
    const char* Auth3dManager::get_name_uid(int32_t in_uid, const auth_3d_database* auth_3d_db) const {
        if (in_uid >= auth_3d_db->uid.size())
            return 0;

        const auth_3d_database_uid* db_uid = &auth_3d_db->uid[in_uid];
        if (db_uid && db_uid->enabled)
            return db_uid->name.c_str();
        return 0;
    }

    // Added
    Scene* Auth3dManager::get_scene(int32_t in_uid) {
        for (int32_t& i : handle_list) {
            Scene* scene = get_work(i);
            if (scene && scene->M_uid == in_uid)
                return scene;
        }
        return 0;
    }

    // Added
    Scene* Auth3dManager::get_scene(uint32_t hash) {
        for (int32_t& i : handle_list) {
            Scene* scene = get_work(i);
            if (scene && scene->hash == hash)
                return scene;
        }
        return 0;
    }

    // 0x1401D5FA0
    int32_t Auth3dManager::get_scene_object_index(object_info in_obj_uid,
        int32_t* out_index, bool* out_is_hrc, int32_t in_instance) {
        for (const int32_t handle : handle_list) {
            Scene* scene = get_work_enabled(handle);
            if (!scene)
                continue;

            int32_t obj_hrc_index = scene->get_object_hrc_index(in_obj_uid, in_instance);
            if (obj_hrc_index >= 0) {
                if (out_index)
                    *out_index = obj_hrc_index;
                if (out_is_hrc)
                    *out_is_hrc = true;
                return handle;
            }

            int32_t obj_index = scene->get_object_index(in_obj_uid, in_instance);
            if (obj_index >= 0) {
                if (out_index)
                    *out_index = obj_index;
                if (out_is_hrc)
                    *out_is_hrc = false;
                return handle;
            }
        }
        return -1;
    }

    // Added
    int32_t Auth3dManager::get_scene_object_index(uint32_t in_file_name_hash, uint32_t in_obj_hash,
        int32_t* out_index, bool* out_is_hrc, int32_t in_instance) {
        for (int32_t& i : g_manager->handle_list) {
            Scene* scene = get_work_enabled(i);
            if (!scene)
                continue;

            bool ret = false;
            if (in_file_name_hash != hash_murmurhash_empty) {
                if (scene->hash != in_file_name_hash)
                    continue;
                ret = true;
            }

            int32_t obj_index = scene->get_object_index(in_obj_hash, in_instance);
            if (obj_index >= 0) {
                if (out_index)
                    *out_index = obj_index;
                if (out_is_hrc)
                    *out_is_hrc = false;
                return i;
            }

            int32_t obj_hrc_index = scene->get_object_hrc_index(in_obj_hash, in_instance);
            if (obj_hrc_index >= 0) {
                if (out_index)
                    *out_index = obj_hrc_index;
                if (out_is_hrc)
                    *out_is_hrc = true;
                return i;
            }

            if (ret)
                return -1;
        }
        return -1;
    }

    // 0x1401E7DE0
    bool Auth3dManager::uid_load_req(int32_t in_uid, const auth_3d_database* auth_3d_db) {
        UidData* uid_data = get_uid_data(in_uid, auth_3d_db);
        if (uid_data) {
            uid_data->load_req(auth_3d_db);
            return true;
        }
        return false;
    }

    // Added
    bool Auth3dManager::uid_load_req_modern(uint32_t hash) {
        UidDataModern* uid_data = get_uid_data_modern(hash);
        if (uid_data) {
            uid_data->load_req();
            return true;
        }
        return false;
    }

    // Inlined
    void Auth3dManager::uid_free(int32_t in_uid, const auth_3d_database* auth_3d_db) {
        UidData* uid_data = get_uid_data(in_uid, auth_3d_db);
        if (uid_data)
            uid_data->free();
    }

    // Added
    void Auth3dManager::uid_free_modern(uint32_t hash) {
        UidDataModern* uid_data = get_uid_data_modern(hash);
        if (uid_data)
            uid_data->free();
    }

    // 0x1401D6910
    bool Auth3dManager::init() {
        handle_list.clear();
        handle_list.shrink_to_fit();
        for (int32_t i = 0; i < AUTH_3D_DATA_COUNT; i++)
            scene_buffer[i].init();
        return true;
    }

    // 0x1401CE370
    bool Auth3dManager::ctrl() {
        for (const int32_t handle : handle_list) {
            Scene* scene = get_work(handle);
            if (scene)
                scene->ctrl(rctx_ptr);
        }
        return false;
    }

    // 0x1401D0310
    void Auth3dManager::disp() {
        for (const int32_t handle : handle_list) {
            Scene* scene = get_work(handle);
            if (scene)
                scene->disp(rctx_ptr);
        }
    }

    // Missing
    inline const Scene* Auth3dManager::M_get_work(int32_t in_handle) const {
        if (in_handle >= 0) {
            size_t idx = in_handle & 0x7FFF;
            if (idx < AUTH_3D_DATA_COUNT)
                return &scene_buffer[idx];
        }
        return 0;
    }

    // 0x1401BF8F0
    inline Scene* Auth3dManager::M_get_work(int32_t in_handle) {
        if (in_handle >= 0) {
            size_t idx = in_handle & 0x7FFF;
            if (idx < AUTH_3D_DATA_COUNT)
                return &scene_buffer[idx];
        }
        return 0;
    }

    // Missing
    inline const Scene* Auth3dManager::get_work(int32_t in_handle) const {
        const Scene* scene = M_get_work(in_handle);
        if (scene && scene->my_handle == in_handle)
            return scene;
        return 0;
    }

    // 0x1401D64F0
    inline Scene* Auth3dManager::get_work(int32_t in_handle) {
        Scene* scene = M_get_work(in_handle);
        if (scene && scene->my_handle == in_handle)
            return scene;
        return 0;
    }

    // Missing
    inline const Scene* Auth3dManager::get_work_enabled(int32_t in_handle) const {
        const Scene* scene = M_get_work(in_handle);
        if (scene && scene->my_handle == in_handle && scene->M_is_enabled)
            return scene;
        return 0;
    }

    // Inlined
    inline Scene* Auth3dManager::get_work_enabled(int32_t in_handle) {
        Scene* scene = M_get_work(in_handle);
        if (scene && scene->my_handle == in_handle && scene->M_is_enabled)
            return scene;
        return 0;
    }

    // 0x1401D6580
    int32_t Auth3dManager::create(int32_t uid, const auth_3d_database* auth_3d_db) {
        if (uid >= auth_3d_db->uid.size() || !auth_3d_db->uid[uid].enabled)
            return -1;

        int32_t index = 0;
        while (scene_buffer[index].is_valid() || scene_buffer[index].hash
            != hash_murmurhash_empty && scene_buffer[index].hash != -1)
            if (++index >= AUTH_3D_DATA_COUNT)
                return -1;

        scene_buffer[index].init();

        if (++magic_number < 0)
            magic_number = 1;

        int32_t handle = ((magic_number & 0x7FFF) << 16) | index & 0x7FFF;
        scene_buffer[index].M_uid = uid;
        scene_buffer[index].my_handle = handle;
        handle_list.push_back(handle);
        return handle;
    }

    // Added
    int32_t Auth3dManager::create(uint32_t hash, void* data,
        const object_database* obj_db, const texture_database* tex_db) {
        int32_t index = 0;
        while (scene_buffer[index].is_valid() || scene_buffer[index].hash
            != hash_murmurhash_empty && scene_buffer[index].hash != -1)
            if (++index >= AUTH_3D_DATA_COUNT)
                return -1;

        auto elem = uid_data_list_modern.find(hash);
        if (elem == uid_data_list_modern.end())
            elem = uid_data_list_modern.insert({ hash, {} }).first;

        UidDataModern* uid_data = &elem->second;
        uid_data->ref_count = 0;
        uid_data->hash = hash;
        uid_data->file_name.clear();
        uid_data->file_name.shrink_to_fit();
        uid_data->load_state = S_BEFORE_LOADING;
        uid_data->name.clear();
        uid_data->name.shrink_to_fit();
        uid_data->category = 0;
        uid_data->data = data;
        uid_data->obj_db = obj_db;
        uid_data->tex_db = tex_db;

        scene_buffer[index].init();

        if (++magic_number < 0)
            magic_number = 1;

        int32_t handle = ((magic_number & 0x7FFF) << 16) | index & 0x7FFF;
        scene_buffer[index].hash = hash;
        scene_buffer[index].my_handle = handle;
        handle_list.push_back(handle);
        return handle;
    }

    // 0x1401D6710
    void Auth3dManager::destroy(int32_t in_handle, render_context* rctx) {
        Scene* scene = get_work(in_handle);
        if (scene && prj::find_and_erase(handle_list, in_handle))
            scene->destroy(rctx);
    }

    // 0x1401D4FD0
    int32_t Auth3dManager::get_assign_rob_id(int32_t in_handle) const {
        const Scene* scene = get_work(in_handle);
        if (scene)
            return scene->get_assign_rob_id();
        return ROB_ID_NULL;
    }

    // 0x1401D5290
    CategoryData* Auth3dManager::get_category_data(const char* in_name) {
        if (!in_name || !category_data_list.size())
            return 0;

        uint64_t name_hash = hash_utf8_xxh3_64bits(in_name);
        for (CategoryData& i : category_data_list)
            if (hash_string_xxh3_64bits(i.category_name) == name_hash)
                return &i;
        return 0;
    }

    // Added
    CategoryData* Auth3dManager::get_category_data_modern(uint32_t in_hash) {
        for (auto& i : g_manager->category_data_list_modern)
            if (i.second.ar && i.second.ar->has_file(in_hash))
                return &i.second;
        return 0;
    }

    // 0x1401C1ED0
    Handle Handle::S_init(int32_t in_handle) {
        return Handle(in_handle);
    }

    // Missing
    int32_t Handle::get() {
        return m_handle;
    }

    // 0x1401CDC80
    Handle Handle::create(int32_t uid, const auth_3d_database* auth_3d_db) {
        return Handle(g_manager->create(uid, auth_3d_db));
    }

    // Added
    Handle Handle::create(uint32_t hash, void* data,
        const object_database* obj_db, const texture_database* tex_db) {
        return Handle(g_manager->create(hash, data, obj_db, tex_db));
    }

    // 0x1401D0190
    void Handle::destroy(render_context* rctx) {
        g_manager->destroy(m_handle, rctx);
        m_handle = -1;
    }

    // 0x1401D8BC0
    bool Handle::is_valid() const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->is_valid();
        return false;
    }

    void Handle::set_invalid() {
        // NOT IMPL!
    }

    // 0x1401D62D0
    int32_t Handle::get_uid() const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->get_uid();
        return -1;
    }

    // 0x1401C9230
    void Handle::add_event_listener(a3d::EventListener* in_evt_list) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            scene->add_event_listener(in_evt_list);
    }

    // 0x1401E3420
    void Handle::remove_event_listener(a3d::EventListener* in_evt_list) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            scene->remove_event_listener(in_evt_list);
    }

    // 0x1401E0F80
    void Handle::load_req(const auth_3d_database* auth_3d_db) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            scene->load_req(auth_3d_db);
    }

    // Added
    void Handle::load_req_modern() {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            scene->load_req_modern();
    }

    // 0x1401E0680
    bool Handle::load_is_done() const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->load_is_done();
        return true;
    }

    // 0x1401E6760
    void Handle::set_frame_req(float_t in_frame) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->set_frame_req(in_frame);
    }

    // 0x1401E6520
    void Handle::set_frame_max(float_t in_frame) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->set_frame_max(in_frame);
    }

    // 0x1401D56C0
    float_t Handle::get_frame() const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->get_frame();
        return 0.0f;
    }

    // 0x1401D5860
    float_t Handle::get_frame_size() const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->get_frame_size();
        return 0.0f;
    }

    // 0x1401D5760
    float_t Handle::get_frame_fps() const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->get_frame_fps();
        return 60.0f;
    }

    // 0x1401D5710
    float_t Handle::get_frame_begin() const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->get_frame_begin();
        return 0.0f;
    }

    // 0x1401E66C0
    void Handle::set_frame_rate_control(FrameRateControl* in_value) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->set_frame_rate_control(in_value);
    }

    // 0x1401E6A10
    void Handle::set_pause(bool in_value) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->set_pause(in_value);
    }

    // 0x1401D5D90
    bool Handle::get_pause() const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->get_pause();
        return false;
    }

    // 0x1401E6420
    void Handle::set_enabled(bool in_value) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->set_enabled(in_value);
    }

    // 0x1401D5580
    bool Handle::get_enabled() const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->get_enabled();
        return false;
    }

    // 0x1401E6950
    void Handle::set_looped(bool in_value) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->set_looped(in_value);
    }

    // 0x1401D5990
    bool Handle::get_looped() const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->get_looped();
        return false;
    }

    // 0x1401E65B0
    void Handle::set_frame_loop_begin() {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->set_frame_loop_begin();
    }

    // 0x1401E6570
    void Handle::set_frame_loop_begin(float_t in_frame) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->set_frame_loop_begin(in_frame);
    }

    // 0x1401D57C0
    float_t Handle::get_frame_loop_begin() const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->get_frame_loop_begin();
        return 0.0f;
    }

    // 0x1401E6650
    void Handle::set_frame_loop_end() {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->set_frame_loop_end();
    }

    // 0x1401E6610
    void Handle::set_frame_loop_end(float_t in_frame) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->set_frame_loop_end(in_frame);
    }

    // 0x1401D5810
    float_t Handle::get_frame_loop_end() const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->get_frame_loop_end();
        return 0.0f;
    }

    // 0x1401D8AB0
    bool Handle::is_finished() const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->is_finished();
        return false;
    }

    // 0x1401E62E0
    void Handle::set_auto_disable(bool in_value) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->set_auto_disable(in_value);
    }

    // 0x1401D51B0
    bool Handle::get_auto_disable() const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->get_auto_disable();
        return false;
    }

    // 0x1401E6330
    void Handle::set_base_matrix(const mat4& in_value) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->set_base_matrix(in_value);
    }

    // 0x1401D5200
    const mat4& Handle::get_base_matrix() const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->get_base_matrix();
        return bad_matrix;
    }

    // 0x1401E6A80
    void Handle::set_reverse_side(bool in_value) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->set_reverse_side(in_value);
    }

    // 0x1401D5E60
    bool Handle::get_reverse_side() const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->get_reverse_side();
        return false;
    }

    // 0x1401D5410
    size_t Handle::get_chara_size() const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->get_chara_size();
        return 0;
    }

    // 0x1401D53C0
    size_t Handle::get_chara_index(const std::string& in_name) const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->get_chara_index(in_name);
        return 0;
    }

    // Missing
    const std::string* Handle::chara_get_name(size_t in_index) const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->chara_get_name(in_index);
        return 0;
    }

    // 0x1401CBEC0
    void Handle::chara_bind_rob(size_t in_index, int32_t in_rob_id) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            scene->chara_bind_rob(in_index, in_rob_id);
    }

    // 0x1401D5C10
    size_t Handle::get_object_size() const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->get_object_size();
        return 0;
    }

    // Missing
    size_t Handle::get_object_index(const std::string& in_name) const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->get_object_index(in_name);
        return -1;
    }

    // 0x1401E1FC0
    const std::string* Handle::object_get_name(size_t in_index) const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->object_get_name(in_index);
        return 0;
    }

    // Missing
    const mat4* Handle::get_object_matrix(size_t in_index) const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->get_object_matrix(in_index);
        return 0;
    }

    // 0x1401E6B80
    void Handle::set_visible(bool in_value) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->set_visible(in_value);
    }

    // 0x1401D64A0
    bool Handle::get_visible() const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->get_visible();
        return false;
    }

    // 0x1401E6B20
    void Handle::set_trnsl(float_t in_value, mdl::ObjFlags in_state) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->set_trnsl(in_value, in_state);
    }

    // 0x1401D60D0
    float_t Handle::get_trnsl() const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->get_trnsl();
        return 1.0f;
    }

    // 0x1401E6290
    void Handle::set_attribute_wall(bool in_value) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            scene->set_attribute_wall(in_value);
    }

    // 01401D50A0
    bool Handle::get_attribute_wall() const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->get_attribute_wall();
        return false;
    }

    // 0x1401E67E0
    void Handle::set_item_attr(uint32_t in_id, uint32_t in_attr) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->set_item_attr(in_id, in_attr);
    }

    // 0x1401E68A0
    void Handle::set_item_texchange(const std::vector<ItemTexChange>& in_list) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->set_item_texchange(in_list);
    }

    // 0x1401CB580
    void Handle::camera_set_enabled(bool in_value) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->camera_set_enabled(in_value);
    }

    // 0x1401CB4D0
    bool Handle::camera_get_enabled() const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->camera_get_enabled();
        return false;
    }

    // 0x1401CB5D0
    void Handle::camera_set_fov_adjust(float_t in_value) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->camera_set_fov_adjust(in_value);
    }

    // 0x1401CB520
    float_t Handle::camera_get_fov_adjust() const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->camera_get_fov_adjust();
        return 1.0f;
    }

    // 0x1401D5140
    size_t Handle::get_auth_2d_size() const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->get_auth_2d_size();
        return 0;
    }

    // 0x1401D50F0
    size_t Handle::get_auth_2d_index(const std::string& in_name) const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->get_auth_2d_index(in_name);
        return -1;
    }

    const std::string* Handle::auth_2d_get_name(size_t in_index) const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->auth_2d_get_name(in_index);
        return 0;
    }

    // Missing
    void Handle::set_data(size_t in_value) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            scene->set_data(in_value);
    }

    // Missing
    size_t Handle::get_data() const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->get_data();
        return 0;
    }

    // 0x1401E6390
    void Handle::set_debug(uint32_t in_debug) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            scene->set_debug(in_debug);
    }

    // 0x1401D5530
    uint32_t Handle::get_debug() const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->get_debug();
        return 0;
    }

    // 0x1401CA840
    void Handle::assign_rob_id(int32_t in_value) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            scene->assign_rob_id(in_value);
    }

    // 0x1401D5050
    int32_t Handle::get_assign_rob_id() const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->get_assign_rob_id();
        return ROB_ID_NULL;
    }

    // 0x1401E69A0
    void Handle::set_obj_uid(object_info in_value) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return set_obj_uid(in_value);
    }

    // 0x1401E6AD0
    void Handle::set_shadow(bool in_value) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            scene->set_shadow(in_value);
    }

    // Missing
    bool Handle::is_shadow() const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->is_shadow();
        return false;
    }

    // 0x1401E34C0
    void Handle::replace_chara(int32_t in_cn_src, int32_t in_cn_dst) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            scene->replace_chara(in_cn_src, in_cn_dst);
    }

    // 0x1401CAD50
    void Handle::set_frame_object_hrc(float_t in_frame) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            scene->set_frame_object_hrc(in_frame);
    }

    // 0x1401CB0F0
    void Handle::calc_matrix_hierarchy(const mat4& in_value) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            scene->calc_matrix_hierarchy(in_value);
    }

    // 0x1401D8C40
    void Handle::join(int32_t in_handle, float_t in_blend) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene) {
            Scene* other_scene = g_manager->get_work(in_handle);
            if (other_scene && scene->object_hrc_list.size()
                && scene->object_hrc_list.size() == other_scene->object_hrc_list.size()) {
                scene->object_hrc_blend_list = &other_scene->object_hrc_list;
                scene->object_hrc_blend = in_blend;
                scene->join();
            }
            else {
                scene->object_hrc_blend_list = 0;
                scene->object_hrc_blend = 0.0f;
                scene->join();
            }
        }
    }

    // 0x1401D92D
    void Handle::join_lock(int32_t in_handle, float_t in_blend) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene) {
            Scene* other_scene = g_manager->get_work(in_handle);
            if (other_scene && scene->object_hrc_list.size()
                && scene->object_hrc_list.size() == other_scene->object_hrc_list.size()) {
                scene->object_hrc_blend_list = &other_scene->object_hrc_list;
                scene->object_hrc_blend = in_blend;
                scene->join_lock();
            }
            else {
                scene->object_hrc_blend_list = 0;
                scene->object_hrc_blend = 0.0f;
                scene->join_lock();
            }
        }
    }

    // 0x1401E6470
    void Handle::set_ex_node_mat(mat4* in_value) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            scene->set_ex_node_mat(in_value);
    }

    // Missing
    bool Handle::is_ex_node_mat() const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->is_ex_node_mat();
        return false;
    }

    // 0x1401E12C0
    void Handle::lock_set() {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            scene->lock_list_set();
    }

    // Added
    void Handle::set_chara_item(bool in_value) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            scene->set_chara_item(in_value);
    }

    // Added
    bool Handle::get_chara_item() const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->get_chara_item();
        return false;
    }

    // Added
    void Handle::set_reflect(bool in_value) {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            scene->set_reflect(in_value);
    }

    // Added
    bool Handle::get_reflect() const {
        const Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene->get_reflect();
        return false;
    }

    // Added
    Scene* Handle::get_scene() {
        Scene* scene = g_manager->get_work(m_handle);
        if (scene)
            return scene;
        return 0;
    }

    // 0x1401C15B0
    void Scene::M_load_exec() {
        if (load_state != S_NOW_LOADING)
            return;

        if (hash == hash_murmurhash_empty) {
            data_struct* aft_data = &data_list[DATA_AFT];
            auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;
            object_database* aft_obj_db = &aft_data->data_ft.obj_db;
            texture_database* aft_tex_db = &aft_data->data_ft.tex_db;

            UidData* uid_data = g_manager->get_uid_data(M_uid, aft_auth_3d_db);
            if (uid_data && uid_data->load_is_done()) {
                load_state = S_DONE;

                time_struct time;
                M_loadFromProperties(uid_data->file, aft_obj_db, aft_tex_db);
                time.calc_time();

                for (a3d::EventListener*& i : my_event_listener) {
                    Handle handle = to_handle();
                    i->loadFinished(handle);
                }

                M_frame = 0.0f;
            }
        }
        else {
            UidDataModern* uid_data = g_manager->get_uid_data_modern(hash);
            if (uid_data && uid_data->load_is_done()) {
                load_state = S_DONE;

                a3da_msgpack_read("patch\\AFT\\auth_3d", uid_data->file_name.c_str(), &uid_data->file.prop);

                const char* l_str = uid_data->file_name.c_str();
                const char* t = strrchr(l_str, '.');
                size_t l_len = uid_data->file_name.size();
                if (t)
                    l_len = t - l_str;

                uid_data->name.assign(uid_data->file_name.c_str(), l_len);
                uid_data->file_name.assign(uid_data->file_name);
                file_name.assign(uid_data->file_name);

                time_struct time;
                M_loadFromProperties(uid_data->file, uid_data->obj_db, uid_data->tex_db);
                time.calc_time();

                hash = uid_data->hash;

                for (a3d::EventListener*& i : my_event_listener) {
                    Handle handle = to_handle();
                    i->loadFinished(handle);
                }

                M_frame = 0.0f;
            }
        }
    }

    //Inlined
    Handle Scene::to_handle() {
        return Handle::S_init(my_handle);
    }

    // 0x1401CBAB0
    void category_free(const char* in_name) {
        g_manager->category_free(in_name);
    }

    // Added
    void category_free(uint32_t in_hash) {
        g_manager->category_free(in_hash);
    }

    // 0x1401CBE20
    bool category_load_is_done(const char* in_name) {
        return g_manager->category_load_is_done(in_name);
    }

    // Added
    bool category_load_is_done(uint32_t in_hash) {
        return g_manager->category_load_is_done(in_hash);
    }

    // 0x1401CBE60
    void category_load_req(const char* in_name) {
        g_manager->category_load_req(in_name, 0);
    }

    // 0x1401CBE30
    void category_load_req(const char* in_name, const char* in_mdata_dir) {
        g_manager->category_load_req(in_name, in_mdata_dir);
    }

    // Added
    void category_load_req(void* data, const char* in_name, uint32_t in_hash) {
        g_manager->category_load_req(data, in_name, in_hash);
    }

    // 0x1401CDCA0
    Event* createEvent(const SceneFile& in_file, const a3d::Event& in_e) {
        switch (in_e.type) {
        case a3d::EVENT_MISC:
        default:
            return new Event(in_e);
        case a3d::EVENT_FLT:
            return createEventFilter(in_file, in_e);
        case a3d::EVENT_FX:
            return createEventFX(in_file, in_e);
        case a3d::EVENT_SND:
            return new EventSnd(in_e);
        case a3d::EVENT_MOT:
            return new EventMot(in_e);
        case a3d::EVENT_A2D:
            return new EventA2d(in_e);
        }
    }

    // 0x1401CDE30
    Event* createEventFX(const SceneFile& in_file, const a3d::Event& in_e) {
        if (!in_e.param1.compare("SMOOTH"))
            return new EventFXSmoothCut(in_e);
        return new EventFX(in_e);
    }

    // 0x1401CDFF0
    Event* createEventFilter(const SceneFile& in_file, const a3d::Event& in_e) {
        if (!in_e.param1.compare("WHITEIN") || !in_e.param1.compare("WHITEOUT")
            || !in_e.param1.compare("FADEIN") || !in_e.param1.compare("FADEOUT"))
            return new EventFilterFade(in_e);
        else if (!in_e.param1.compare("TIMESTOP"))
            return new EventFilterTimeStop(in_e);
        return new Event(in_e);
    }

    // 0x1401D5BC0
    const mat4* get_object_matrix(int32_t in_handle, size_t in_index, bool in_is_hrc) {
        return g_manager->get_object_matrix(in_handle, in_index, in_is_hrc);
    }

    // 0x1401D5010
    int32_t get_assign_rob_id(int32_t in_handle) {
        return g_manager->get_assign_rob_id(in_handle);
    }

    // Added
    Scene* get_scene(int32_t in_uid) {
        return g_manager->get_scene(in_uid);
    }

    // Added
    Scene* get_scene(uint32_t hash) {
        return g_manager->get_scene(hash);
    }

    // 0x1401D6090
    int32_t get_scene_object_index(object_info in_obj_uid,
        int32_t* out_index, bool* out_is_hrc, int32_t in_instance) {
        return g_manager->get_scene_object_index(in_obj_uid, out_index, out_is_hrc, in_instance);
    }

    // Added
    int32_t get_scene_object_index(uint32_t in_file_name_hash, uint32_t in_obj_hash,
        int32_t* out_index, bool* out_is_hrc, int32_t in_instance) {
        return g_manager->get_scene_object_index(in_file_name_hash, in_obj_hash, out_index, out_is_hrc, in_instance);
    }

    // 0x1401E7E90
    const char* get_name_uid(int32_t in_uid, const auth_3d_database* auth_3d_db) {
        return g_manager->get_name_uid(in_uid, auth_3d_db);
    }
}

void auth_3d_data_init() {
    if (!auth_3d_detail::g_manager)
        auth_3d_detail::g_manager = new auth_3d_detail::Auth3dManager;
}

// 0x140248480
void auth_3d_data_get_obj_sets_from_category(std::string& name, std::vector<uint32_t>& obj_sets,
    const auth_3d_database* auth_3d_db, const object_database* obj_db) {
    obj_sets.clear();

    std::vector<int32_t> uids;
    auth_3d_db->get_category_uids(name.c_str(), uids);
    for (int32_t& i : uids) {
        const char* uid_name = auth_3d_detail::get_name_uid(i, auth_3d_db);
        if (!uid_name)
            continue;

        const char* s = strchr(uid_name, '_');
        if (!s)
            break;

        uint32_t obj_set = obj_db->get_object_set_id(std::string(uid_name, s - uid_name).c_str());
        if (obj_set != -1)
            obj_sets.push_back(obj_set);
        break;
    }

    if (!name.find("ITMPV")) {
        int32_t pv_id = atoi(name.substr(5, 3).c_str());
        const pv_db_pv_difficulty* diff = task_pv_db_get_pv_difficulty(
            pv_id, PV_DIFFICULTY_HARD, PV_EDITION_ORIGINAL);
        if (diff)
            for (const pv_db_pv_item& i : diff->pv_item) {
                if (i.index <= 0)
                    continue;

                object_info info = obj_db->get_object_info(i.name.c_str());
                if (info.not_null())
                    obj_sets.push_back(info.set_id);
            }
    }

    if (!name.find("EFFCHRPV")) {
        uint32_t obj_set = obj_db->get_object_set_id(name.c_str());
        if (obj_set != -1)
            obj_sets.push_back(obj_set);
    }

    prj::sort_and_erase_non_unique(obj_sets);

    std::string stgpv;
    size_t stgpv_offset = name.find("STGPV");
    if (stgpv_offset != -1)
        stgpv = name.substr(stgpv_offset, 8);

    size_t stgd2pv_offset = name.find("STGD2PV");
    if (stgd2pv_offset != -1)
        stgpv = name.substr(stgd2pv_offset, 10);

    if (stgpv.size()) {
        uint32_t obj_set = obj_db->get_object_set_id(stgpv.c_str());
        if (obj_set != -1)
            obj_sets.push_back(obj_set);
    }

    stgpv.append("HRC");

    uint32_t obj_set = obj_db->get_object_set_id(stgpv.c_str());
    if (obj_set != -1)
        obj_sets.push_back(obj_set);
}

void auth_3d_data_load_auth_3d_db(const auth_3d_database* auth_3d_db) {
    auth_3d_detail::g_manager->category_data_list.clear();
    auth_3d_detail::g_manager->category_data_list.resize(auth_3d_db->category.size());

    size_t cat_index = 0;
    for (auth_3d_detail::CategoryData& i : auth_3d_detail::g_manager->category_data_list)
        i.category_name.assign(auth_3d_db->category[cat_index++].name);

    auth_3d_detail::g_manager->uid_data_list.clear();
    auth_3d_detail::g_manager->uid_data_list.resize(auth_3d_db->uid.size());

    size_t uid_index = 0;
    for (auth_3d_detail::UidData& i : auth_3d_detail::g_manager->uid_data_list)
        i.uid = (int32_t)(uid_index++);
}

void auth_3d_data_free() {
    if (auth_3d_detail::g_manager) {
        delete auth_3d_detail::g_manager;
        auth_3d_detail::g_manager = 0;
    }
}

void task_auth_3d_init() {
    if (!auth_3d_detail::task_auth_3d)
        auth_3d_detail::task_auth_3d = new auth_3d_detail::TaskAuth3d;
}

bool task_auth_3d_open() {
    return auth_3d_detail::task_auth_3d->open("AUTH_3D");
}

bool task_auth_3d_check_alive() {
    return auth_3d_detail::task_auth_3d->check_alive();
}

bool task_auth_3d_close() {
    return auth_3d_detail::task_auth_3d->close();
}

void task_auth_3d_free() {
    if (auth_3d_detail::task_auth_3d) {
        delete auth_3d_detail::task_auth_3d;
        auth_3d_detail::task_auth_3d = 0;
    }
}

namespace auth_3d_detail {
    TaskAuth3d::TaskAuth3d() {

    }

    TaskAuth3d:: ~TaskAuth3d() {

    }

    bool TaskAuth3d::init() {
        return g_manager->init();
    }

    bool TaskAuth3d::ctrl() {
        return g_manager->ctrl();
    }

    void TaskAuth3d::disp() {
        g_manager->disp();
    }

    FrameRateTimeStop::FrameRateTimeStop() {

    }

    FrameRateTimeStop::~FrameRateTimeStop() {

    }

    float_t FrameRateTimeStop::get_delta_frame() {
        return 0.0f;
    }

    // 0x1401D3860
    static void event_log(const char* fmt, ...) {
#ifdef DEBUG
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
#endif
    }

    // 0x1401D4F30
    static const char* get_ID_EVENT_TYPE(EVENT_TYPE type) {
        switch (type) {
        case EVENT_MISC:
            return "MISC";
        case EVENT_FLT:
            return "FLT";
        case EVENT_FX:
            return "FX";
        case EVENT_SND:
            return "SND";
        case EVENT_MOT:
            return "MOT";
        case EVENT_A2D:
            return "A2D";
        default:
            return "(unknown)";
        }
    }

    template <class T>
    size_t get_index_by_name(const T& in_c, const std::string& in_name) {
        size_t index = 0;
        for (auto i = in_c.cbegin(); i != in_c.cend(); i++, index++)
            if (!i->name.compare(in_name))
                return index;
        return -1;
    }

    template <class T, class U, typename... Args>
    void list_load(T& in_c, const SceneFile& in_file, const U& in_a, Args... args) {
        if (in_a.size()) {
            size_t index = 0;
            in_c.resize(in_a.size());
            for (size_t i = 0; i < in_a.size(); i++)
                in_c[i].load(in_file, in_a[i], args...);
        }
    }

    template <class T, class U, typename... Args>
    void list_store(const T& in_c, const SceneFile& in_file, U& out_a, Args... args) {
        if (in_c.size()) {
            size_t index = 0;
            out_a.resize(in_c.size());
            for (size_t i = 0; i < in_c.size(); i++)
                in_c[i].store(in_file, out_a[i], args...);
        }
    }
}

static bool a3da_msgpack_read_key(a3d::Fcurve& key, msgpack* msg) {
    if (msg->read_bool("remove")) {
        key = {};
        return false;
    }

    if (msg->read_bool("ignore_tangents") && key.keys.size()) {
        float_t ep_pre_val = key.keys.front().l_slope;
        float_t ep_post_val = key.keys.back().r_slope;
        for (kft3& i : key.keys) {
            i.l_slope = 0.0f;
            i.r_slope = 0.0f;
        }

        if (key.ep_type_pre == a3d::EP_TYPE_LINEAR)
            key.keys.front().l_slope = ep_pre_val;
        if (key.ep_type_post == a3d::EP_TYPE_LINEAR)
            key.keys.back().r_slope = ep_post_val;
    }

    msgpack* max_frame = msg->read("max_frame");
    if (max_frame)
        key.max_frame = max_frame->read_float_t();

    msgpack* change_type = msg->read("change_type");
    if (change_type) {
        a3d::FC_TYPE type = (a3d::FC_TYPE)change_type->read_int32_t();
        switch (type) {
        case a3d::FC_TYPE_STATIC_0:
            key.keys.clear();
            key.type = a3d::FC_TYPE_STATIC_0;
            key.value = 0.0f;
            return true;
        case a3d::FC_TYPE_STATIC_DATA:
            key.keys.clear();
            key.type = a3d::FC_TYPE_STATIC_DATA;
            key.value = msg->read_float_t("value");
            return true;
        case a3d::FC_TYPE_LINEAR:
        case a3d::FC_TYPE_HERMITE:
        case a3d::FC_TYPE_NO_INTERPOLATION:
            switch (key.type) {
            case a3d::FC_TYPE_LINEAR:
            case a3d::FC_TYPE_HERMITE:
            case a3d::FC_TYPE_NO_INTERPOLATION:
                key.type = type;
                break;
            }
            break;
        }
    }

    msgpack* replace_keys = msg->read_array("replace_keys");
    if (replace_keys) {
        key.keys.clear();

        msgpack_array* ptr = replace_keys->data.arr;
        for (msgpack& i : *ptr) {
            msgpack& _key = i;

            msgpack* data = _key.read_array();
            if (!data)
                continue;

            msgpack_array* ptr = data->data.arr;

            kft3 k = {};
            switch (ptr->size()) {
            case 1:
                k.frame = (*ptr)[0].read_float_t();
                break;
            case 2:
                k.frame = (*ptr)[0].read_float_t();
                k.value = (*ptr)[1].read_float_t();
                break;
            case 3: {
                k.frame = (*ptr)[0].read_float_t();
                k.value = (*ptr)[1].read_float_t();
                float_t slope = (*ptr)[2].read_float_t();
                k.l_slope = slope;
                k.r_slope = slope;
            } break;
            case 4:
                k.frame = (*ptr)[0].read_float_t();
                k.value = (*ptr)[1].read_float_t();
                k.l_slope = (*ptr)[2].read_float_t();
                k.r_slope = (*ptr)[3].read_float_t();
                break;
            }
            key.keys.push_back(k);
        }
    }
    return true;
}

static bool a3da_msgpack_read_vec3(a3d::Fcurve3f& vec, msgpack* msg) {
    if (msg->read_bool("remove")) {
        vec = {};
        return false;
    }

    msgpack* x = msg->read_map("x");
    if (x && !a3da_msgpack_read_key(vec.x, x))
        vec.x = {};

    msgpack* y = msg->read_map("y");
    if (y && !a3da_msgpack_read_key(vec.y, y))
        vec.y = {};

    msgpack* z = msg->read_map("z");
    if (z && !a3da_msgpack_read_key(vec.z, z))
        vec.z = {};
    return true;
}

static bool a3da_msgpack_read_rgba(a3d::FcurveColor4f& col4, msgpack* msg) {
    if (msg->read_bool("remove")) {
        col4 = {};
        return false;
    }

    msgpack* rgb = msg->read_map("rgb");
    if (rgb) {
        a3d::Fcurve rgb_key = col4.r;
        if (a3da_msgpack_read_key(rgb_key, rgb)) {
            col4.r = rgb_key;
            col4.g = rgb_key;
            col4.b = rgb_key;
            col4.has_r = true;
            col4.has_g = true;
            col4.has_b = true;
        }
        else {
            col4.r = {};
            col4.g = {};
            col4.b = {};
            col4.has_r = false;
            col4.has_g = false;
            col4.has_b = false;
        }
    }

    msgpack* r = msg->read_map("r");
    if (r) {
        col4.has_r = true;
        if (!a3da_msgpack_read_key(col4.r, r)) {
            col4.r = {};
            col4.has_r = false;
        }
    }

    msgpack* g = msg->read_map("g");
    if (g) {
        col4.has_g = true;
        if (!a3da_msgpack_read_key(col4.g, g)) {
            col4.g = {};
            col4.has_g = false;
        }
    }

    msgpack* b = msg->read_map("b");
    if (b) {
        col4.has_b = true;
        if (!a3da_msgpack_read_key(col4.b, b)) {
            col4.b = {};
            col4.has_b = false;
        }
    }

    msgpack* a = msg->read_map("a");
    if (a) {
        col4.has_a = true;
        if (!a3da_msgpack_read_key(col4.a, a)) {
            col4.a = {};
            col4.has_a = false;
        }
    }
    return true;
}

static bool a3da_msgpack_read_model_transform(a3d::ModelTransform& mt, msgpack* msg) {
    if (msg->read_bool("remove")) {
        mt = {};
        mt.scale.x.type = a3d::FC_TYPE_STATIC_DATA;
        mt.scale.x.value = 1.0f;
        mt.scale.y.type = a3d::FC_TYPE_STATIC_DATA;
        mt.scale.y.value = 1.0f;
        mt.scale.z.type = a3d::FC_TYPE_STATIC_DATA;
        mt.scale.z.value = 1.0f;
        mt.visibility.type = a3d::FC_TYPE_STATIC_DATA;
        mt.visibility.value = 1.0f;
        return false;
    }

    msgpack* trans = msg->read_map("trans");
    if (trans && !a3da_msgpack_read_vec3(mt.translation, trans))
        mt.translation = {};

    msgpack* rot = msg->read_map("rot");
    if (rot && !a3da_msgpack_read_vec3(mt.rotation, rot))
        mt.rotation = {};

    msgpack* scale = msg->read_map("scale");
    if (scale && !a3da_msgpack_read_vec3(mt.scale, scale)) {
        mt.scale = {};
        mt.scale.x.type = a3d::FC_TYPE_STATIC_DATA;
        mt.scale.x.value = 1.0f;
        mt.scale.y.type = a3d::FC_TYPE_STATIC_DATA;
        mt.scale.y.value = 1.0f;
        mt.scale.z.type = a3d::FC_TYPE_STATIC_DATA;
        mt.scale.z.value = 1.0f;
    }

    msgpack* visibility = msg->read_map("visibility");
    if (visibility && !a3da_msgpack_read_key(mt.visibility, visibility)) {
        mt.visibility = {};
        mt.visibility.type = a3d::FC_TYPE_STATIC_DATA;
        mt.visibility.value = 1.0f;
    }
    return true;
}

static void a3da_msgpack_read(const char* path, const char* file, a3d::Scene* auth_file) {
    if (!path_check_directory_exists(path))
        return;

    char file_buf[0x80];
    for (const char* i = file; *i && *i != '.'; i++) {
        char c = *i;
        file_buf[i - file] = c;
        file_buf[i - file + 1] = 0;
    }

    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "%s\\%s.json", path, file_buf);
    if (!path_check_file_exists(buf))
        return;

    msgpack msg;

    file_stream s;
    s.open(buf, "rb");
    io_json_read(s, &msg);
    s.close();

    if (msg.type != MSGPACK_MAP) {
        printf("Failed to load Auth 3D JSON!\nPath: %s\n", buf);
        return;
    }

    msgpack* curves = msg.read_array("curve");
    if (curves) {
        msgpack_array* ptr = curves->data.arr;
        for (msgpack& i : *ptr) {
            msgpack& curve = i;

            std::string name = curve.read_string("name");
            uint32_t name_hash = hash_string_murmurhash(name);

            for (a3d::Curve& j : auth_file->curve) {
                if (name_hash != hash_string_murmurhash(j.name))
                    continue;

                msgpack* cv = curve.read_map("cv");
                if (cv)
                    a3da_msgpack_read_key(j.fcurve, cv);
                break;
            }
        }
    }

    msgpack* m_objhrcs = msg.read_array("m_objhrc");
    if (m_objhrcs) {
        msgpack_array* ptr = m_objhrcs->data.arr;
        for (msgpack& i : *ptr) {
            msgpack& m_objhrc = i;

            std::string name = m_objhrc.read_string("name");
            uint32_t name_hash = hash_string_murmurhash(name);

            for (a3d::MObjectHrc& j : auth_file->m_object_hrc) {
                if (name_hash != hash_string_murmurhash(j.name))
                    continue;

                msgpack* instances = m_objhrc.read_array("instance");
                if (instances) {
                    msgpack_array* ptr =  instances->data.arr;
                    for (msgpack& k : *ptr) {
                        msgpack& instance = k;

                        int32_t index = instance.read_int32_t("index");
                        if (index < j.instance.size())
                            j.instance[index].shadow = instance.read_bool("shadow");
                    }
                }
                break;
            }
        }
    }

    // X/XHD
    msgpack* material_lists = msg.read_array("material_list");
    if (material_lists) {
        msgpack_array* ptr = material_lists->data.arr;
        for (msgpack& i : *ptr) {
            msgpack& material_list = i;

            std::string name = material_list.read_string("name");
            uint32_t name_hash = hash_string_murmurhash(name);

            auto j_begin = auth_file->material_list.begin();
            auto j_end = auth_file->material_list.end();
            for (auto j = j_begin; j != j_end; j++) {
                if (name_hash != hash_string_murmurhash(j->name))
                    continue;

                bool remove = material_list.read_bool("remove");
                if (remove) {
                    auth_file->material_list.erase(j);
                    break;
                }

                msgpack* new_name = material_list.read("new_name");
                if (new_name)
                    j->name.assign(new_name->read_string());

                msgpack* blend_color = material_list.read_map("blend_color");
                if (blend_color) {
                    j->flag |= a3d::MaterialList::FLAG_BLEND_COLOR;
                    if (!a3da_msgpack_read_rgba(j->blend_color, blend_color))
                        j->flag &= ~a3d::MaterialList::FLAG_BLEND_COLOR;
                }

                msgpack* emission = material_list.read_map("emission");
                if (emission) {
                    j->flag |= a3d::MaterialList::FLAG_EMISSION;
                    if (!a3da_msgpack_read_rgba(j->emission, emission))
                        j->flag &= ~a3d::MaterialList::FLAG_EMISSION;
                }

                if (!j->flag)
                    auth_file->material_list.erase(j);
                break;
            }
        }
    }

    std::vector<std::string> remove_parent_name;
    std::vector<std::string> remove_parent_node;

    msgpack* objhrcs = msg.read_array("objhrc");
    if (objhrcs) {
        msgpack_array* ptr = objhrcs->data.arr;
        for (msgpack& i : *ptr) {
            msgpack& objhrc = i;

            std::string name = objhrc.read_string("name");
            uint32_t name_hash = hash_string_murmurhash(name);

            auto j_begin = auth_file->object_hrc.begin();
            auto j_end = auth_file->object_hrc.end();
            for (auto j = j_begin; j != j_end; j++) {
                if (name_hash != hash_string_murmurhash(j->name))
                    continue;

                bool remove = objhrc.read_bool("remove");
                if (remove) {
                    auto k_begin = auth_file->object_list.begin();
                    auto k_end = auth_file->object_list.end();
                    for (auto k = k_begin; k != k_end; k++)
                        if (name_hash == hash_string_murmurhash(*k)) {
                            auth_file->object_list.erase(k);
                            break;
                        }

                    remove_parent_name.push_back(j->name);

                    for (a3d::ObjectNode& k : j->node)
                        remove_parent_node.push_back(k.name);

                    auth_file->object_hrc.erase(j);
                    break;
                }

                j->shadow = objhrc.read_bool("shadow");
                break;
            }
        }
    }

    msgpack* objects = msg.read_array("object");
    if (objects) {
        msgpack_array* ptr = objects->data.arr;
        for (msgpack& i : *ptr) {
            msgpack& object = i;

            std::string name = object.read_string("name");
            uint32_t name_hash = hash_string_murmurhash(name);

            auto j_begin = auth_file->object.begin();
            auto j_end = auth_file->object.end();
            for (auto j = j_begin; j != j_end; j++) {
                if (name_hash != hash_string_murmurhash(j->name))
                    continue;

                bool remove = object.read_bool("remove");
                if (remove) {
                    auto k_begin = auth_file->object_hrc_list.begin();
                    auto k_end = auth_file->object_hrc_list.end();
                    for (auto k = k_begin; k != k_end; k++)
                        if (name_hash == hash_string_murmurhash(*k)) {
                            auth_file->object_hrc_list.erase(k);
                            break;
                        }

                    remove_parent_name.push_back(j->name);

                    auth_file->object.erase(j);
                    break;
                }

                bool remove_morph = object.read_bool("remove_morph");
                if (remove_morph) {
                    j->morph.assign("");
                    j->morph_offset = 0.0f;
                }

                a3da_msgpack_read_model_transform(j->model_transform, &object);
                break;
            }
        }
    }

    msgpack* play_control = msg.read_map("play_control");
    if (play_control) {
        msgpack* begin = play_control->read("begin");
        if (begin)
            auth_file->play_control.begin = begin->read_float_t();

        msgpack* fps = play_control->read("fps");
        if (fps)
            auth_file->play_control.begin = fps->read_float_t();

        msgpack* size = play_control->read("size");
        if (size)
            auth_file->play_control.size = size->read_float_t();
    }

    while (remove_parent_name.size() || remove_parent_node.size()) {
        size_t remove_parent_name_size = remove_parent_name.size();
        for (size_t i = remove_parent_name_size, i1 = 0; i; i--, i1++) {
            uint32_t remove_parent_name_hash = hash_string_murmurhash(remove_parent_name[i1]);

            auto j_begin = auth_file->object.begin();
            auto j_end = auth_file->object.end();
            for (auto j = j_begin; j != j_end; j++)
                if (remove_parent_name_hash == hash_string_murmurhash(j->parent_name)) {
                    remove_parent_name.push_back(j->name);
                    auth_file->object.erase(j);
                    break;
                }

            auto k_begin = auth_file->object_hrc.begin();
            auto k_end = auth_file->object_hrc.end();
            for (auto k = k_begin; k != k_end; k++)
                if (remove_parent_name_hash == hash_string_murmurhash(k->parent_name)) {
                    remove_parent_name.push_back(k->name);

                    for (a3d::ObjectNode& l : k->node)
                        remove_parent_node.push_back(l.name);
                    auth_file->object_hrc.erase(k);
                    break;
                }
        }

        remove_parent_name.erase(remove_parent_name.begin(),
            remove_parent_name.begin() + remove_parent_name_size);

        size_t remove_parent_node_size = remove_parent_node.size();
        for (size_t i = remove_parent_node_size, i1 = 0; i; i--, i1++) {
            uint32_t remove_parent_node_hash = hash_string_murmurhash(remove_parent_node[i1]);

            auto j_begin = auth_file->object.begin();
            auto j_end = auth_file->object.end();
            for (auto j = j_begin; j != j_end; j++)
                if (remove_parent_node_hash == hash_string_murmurhash(j->parent_node)) {
                    remove_parent_node.push_back(j->name);
                    auth_file->object.erase(j);
                    break;
                }

            auto k_begin = auth_file->object_hrc.begin();
            auto k_end = auth_file->object_hrc.end();
            for (auto k = k_begin; k != k_end; k++)
                if (remove_parent_node_hash == hash_string_murmurhash(k->parent_node)) {
                    remove_parent_node.push_back(k->name);

                    for (a3d::ObjectNode& l : k->node)
                        remove_parent_node.push_back(l.name);
                    auth_file->object_hrc.erase(k);
                    break;
                }
        }

        remove_parent_node.erase(remove_parent_node.begin(),
            remove_parent_node.begin() + remove_parent_node_size);
    };
}

static void auth_3d_set_material_list(auth_3d_detail::Scene& in_scene, render_context* rctx) {
    mdl::DispManager& disp_manager = *rctx->disp_manager;

    int32_t mat_list_count = 0;
    material_list_struct mat_list[MATERIAL_LIST_COUNT];
    for (auth_3d_detail::MaterialList& i : in_scene.material_list) {

        if (!(i.blend_color.has_color[0] || i.blend_color.has_color[1]
            || i.blend_color.has_color[2] || i.blend_color.has_color[3])
            && !(i.emission.has_color[0] || i.emission.has_color[1]
                || i.emission.has_color[2] || i.emission.has_color[3]))
            continue;

        vec4& blend_color = mat_list[mat_list_count].blend_color;
        bool* has_blend_color = mat_list[mat_list_count].has_blend_color;
        if (i.blend_color.has_color[0] || i.blend_color.has_color[1]
            || i.blend_color.has_color[2] || i.blend_color.has_color[3]) {
            blend_color = i.blend_color.color;
            has_blend_color[0] = i.blend_color.has_color[0];
            has_blend_color[1] = i.blend_color.has_color[1];
            has_blend_color[2] = i.blend_color.has_color[2];
            has_blend_color[3] = i.blend_color.has_color[3];
        }
        else {
            blend_color = 0.0f;
            has_blend_color[0] = false;
            has_blend_color[1] = false;
            has_blend_color[2] = false;
            has_blend_color[3] = false;
        }

        vec4& emission = mat_list[mat_list_count].emission;
        bool* has_emission = mat_list[mat_list_count].has_emission;
        if (i.emission.has_color[0] || i.emission.has_color[1]
            || i.emission.has_color[2] || i.emission.has_color[3]) {

            emission = i.emission.color;
            has_emission[0] = i.emission.has_color[0];
            has_emission[1] = i.emission.has_color[1];
            has_emission[2] = i.emission.has_color[2];
            has_emission[3] = i.emission.has_color[3];
        }
        else {
            emission = 0.0f;
            has_emission[0] = false;
            has_emission[1] = false;
            has_emission[2] = false;
            has_emission[3] = false;
        }

        mat_list[mat_list_count].hash = i.hash;
        mat_list_count++;
    }

    disp_manager.set_material_list(mat_list_count, mat_list);
}
