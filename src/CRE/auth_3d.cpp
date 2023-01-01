/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "auth_3d.hpp"
#include "../KKdLib/io/file_stream.hpp"
#include "../KKdLib/io/json.hpp"
#include "../KKdLib/io/path.hpp"
#include "../KKdLib/hash.hpp"
#include "../KKdLib/interpolation.hpp"
#include "../KKdLib/msgpack.hpp"
#include "../KKdLib/sort.hpp"
#include "../KKdLib/str_utils.hpp"
#include "rob/rob.hpp"
#include "clear_color.hpp"
#include "draw_task.hpp"
#include "object.hpp"
#include "pv_db.hpp"
#include "task_effect.hpp"
#include "sound.hpp"
#include "stage.hpp"

namespace auth_3d_detail {
    class TaskAuth3d : public app::Task {
    public:
        TaskAuth3d();
        virtual ~TaskAuth3d() override;

        virtual bool Init() override;
        virtual bool Ctrl() override;
        virtual void Disp() override;
    };

    class FrameRateTimeStop : public FrameRateControl {
    public:
        FrameRateTimeStop();
        virtual ~FrameRateTimeStop() override;

        virtual float_t GetDeltaFrame() override;
    };
}

typedef a3da_ambient auth_3d_ambient_file;
typedef a3da_camera_auxiliary auth_3d_camera_auxiliary_file;
typedef a3da_camera_root auth_3d_camera_root_file;
typedef a3da_camera_root_view_point auth_3d_camera_root_view_point_file;
typedef a3da_chara auth_3d_chara_file;
typedef a3da_curve auth_3d_curve_file;
typedef a3da_dof auth_3d_dof_file;
typedef a3da_event auth_3d_event_file;
typedef a3da_fog auth_3d_fog_file;
typedef a3da_key auth_3d_key_file;
typedef a3da_light auth_3d_light_file;
typedef a3da_m_object_hrc auth_3d_m_object_hrc_file;
typedef a3da_object_instance auth_3d_object_instance_file;
typedef a3da_material_list auth_3d_material_list_file;
typedef a3da_model_transform auth_3d_model_transform_file;
typedef a3da_object auth_3d_object_file;
typedef a3da_object_hrc auth_3d_object_hrc_file;
typedef a3da_object_node auth_3d_object_node_file;
typedef a3da_object_texture_pattern auth_3d_object_texture_pattern_file;
typedef a3da_object_texture_transform auth_3d_object_texture_transform_file;
typedef a3da_play_control auth_3d_play_control_file;
typedef a3da_point auth_3d_point_file;
typedef a3da_post_process auth_3d_post_process_file;
typedef a3da_rgba auth_3d_rgba_file;
typedef a3da_vec3 auth_3d_vec3_file;

static void a3da_msgpack_read(const char* path, const char* file, a3da* auth_file);

static bool auth_3d_key_detect_fast_change(auth_3d_key* data, float_t frame, float_t threshold);
static kft3* auth_3d_key_find_keyframe(auth_3d_key* data, float_t frame);
static float_t auth_3d_key_interpolate_inner(auth_3d_key* data, float_t frame);
static float_t auth_3d_interpolate_value(auth_3d_key_type type,
    float_t frame, kft3* curr_key, kft3* next_key);
static void auth_3d_key_load(auth_3d* auth, auth_3d_key* k, auth_3d_key_file* kf);
static void auth_3d_rgba_load(auth_3d* auth, auth_3d_rgba* rgba, auth_3d_rgba_file* rgbaf);
static void auth_3d_vec3_load(auth_3d* auth, auth_3d_vec3* vec, auth_3d_vec3_file* vecf);
static void auth_3d_model_transform_load(auth_3d* auth, auth_3d_model_transform* mt, auth_3d_model_transform_file* mtf);

static int32_t auth_3d_get_auth_3d_object_index_by_object_info(auth_3d* auth,
    object_info obj_info, int32_t instance);
static int32_t auth_3d_get_auth_3d_object_index_by_hash(auth_3d* auth,
    uint32_t object_hash, int32_t instance);
static mat4* auth_3d_get_auth_3d_object_hrc_bone_mats(auth_3d* auth, size_t index);
static int32_t auth_3d_get_auth_3d_object_hrc_index_by_object_info(auth_3d* auth,
    object_info obj_info, int32_t instance = -1);
static int32_t auth_3d_get_auth_3d_object_hrc_index_by_hash(auth_3d* auth,
    uint32_t object_hash, int32_t instance = -1);
static void auth_3d_read_file(auth_3d* auth, auth_3d_database* auth_3d_db);
static void auth_3d_read_file_modern(auth_3d* auth);
static void auth_3d_set_material_list(auth_3d* auth, render_context* rctx);

static void auth_3d_ambient_load(auth_3d* auth, auth_3d_ambient* a, auth_3d_ambient_file* af);
static void auth_3d_ambient_set(auth_3d_ambient* a, render_context* rctx);
static void auth_3d_camera_auxiliary_load(auth_3d* auth, auth_3d_camera_auxiliary_file* caf);
static void auth_3d_camera_auxiliary_restore_prev_value(auth_3d_camera_auxiliary* ca, render_context* rctx);
static void auth_3d_camera_auxiliary_set(auth_3d_camera_auxiliary* ca, render_context* rctx);
static float_t auth_3d_camera_root_calc_frame(auth_3d_camera_root* cr, float_t frame, render_context* rctx);
static void auth_3d_camera_root_load(auth_3d* auth, auth_3d_camera_root* cr, auth_3d_camera_root_file* crf);
static void auth_3d_camera_root_view_point_load(auth_3d* auth, auth_3d_camera_root_view_point* crvp,
    auth_3d_camera_root_view_point_file* crvpf);
static void auth_3d_chara_disp(auth_3d_chara* c, mat4* parent_mat, render_context* rctx);
static void auth_3d_chara_load(auth_3d* auth, auth_3d_chara* c, auth_3d_chara_file* cf);
static void auth_3d_curve_load(auth_3d* auth, auth_3d_curve* c, auth_3d_curve_file* cf);
static void auth_3d_dof_load(auth_3d* auth, auth_3d_dof* d, auth_3d_dof_file* df);
static void auth_3d_dof_restore_prev_value(auth_3d_dof* d, render_context* rctx);
static void auth_3d_dof_set(auth_3d_dof* d, render_context* rctx);
static auth_3d_detail::Event* auth_3d_event_load(auth_3d * auth, auth_3d_event_file * ef);
static const char* auth_3d_event_type_string(auth_3d_detail::Event::Type type);
static void auth_3d_fog_load(auth_3d* auth, auth_3d_fog* e, auth_3d_fog_file* ff);
static void auth_3d_fog_restore_prev_value(auth_3d_fog* f, render_context* rctx);
static void auth_3d_fog_set(auth_3d_fog* f, render_context* rctx);
static void auth_3d_light_load(auth_3d* auth, auth_3d_light* a, auth_3d_light_file* lf);
static void auth_3d_light_restore_prev_value(auth_3d_light* l, render_context* rctx);
static void auth_3d_light_set(auth_3d_light* l, render_context* rctx);
static void auth_3d_m_object_hrc_disp(auth_3d_m_object_hrc* moh, auth_3d* auth, render_context* rctx);
static void auth_3d_m_object_hrc_get_mat(auth_3d_m_object_hrc* moh, mat4* mat);
static void auth_3d_m_object_hrc_load(auth_3d* auth, auth_3d_m_object_hrc* moh,
    auth_3d_m_object_hrc_file* mohf, object_database* obj_db);
static void auth_3d_m_object_hrc_nodes_mat_mult(auth_3d_m_object_hrc* moh);
static void auth_3d_material_list_load(auth_3d* auth, auth_3d_material_list* ml, auth_3d_material_list_file* mlf);
static void auth_3d_object_disp(auth_3d_object* o, auth_3d* auth, render_context* rctx);
static void auth_3d_object_load(auth_3d* auth, auth_3d_object* o,
    auth_3d_object_file* of, object_database* obj_db, texture_database* tex_db);
static void auth_3d_object_curve_load(auth_3d* auth, auth_3d_object_curve* curve,
    std::string& name, float_t frame_offset);
static void auth_3d_object_hrc_disp(auth_3d_object_hrc* oh, auth_3d* auth, render_context* rctx);
static void auth_3d_object_hrc_load(auth_3d* auth, auth_3d_object_hrc* oh,
    auth_3d_object_hrc_file* ohf, object_database* obj_db);
static bool auth_3d_object_hrc_replace_chara(auth_3d_object_hrc* oh,
    chara_index src_chara, chara_index dst_chara, object_database* obj_db);
static void auth_3d_object_hrc_nodes_mat_mult(auth_3d_object_hrc* oh, const mat4* mat);
static void auth_3d_object_instance_load(auth_3d* auth, auth_3d_object_instance* oi,
    auth_3d_object_instance_file* oif, auth_3d_m_object_hrc* moh, object_database* obj_db);
static void auth_3d_object_model_transform_load(auth_3d* auth,
    auth_3d_object_model_transform* omt, auth_3d_model_transform_file* mtf);
static void auth_3d_object_node_load(auth_3d* auth,
    auth_3d_object_node* on, auth_3d_object_node_file* onf);
static void auth_3d_object_texture_pattern_load(auth_3d* auth, auth_3d_object_texture_pattern* otp,
    auth_3d_object_texture_pattern_file* otpf, texture_database* tex_db);
static void auth_3d_object_texture_transform_load(auth_3d* auth, auth_3d_object_texture_transform* ott,
    auth_3d_object_texture_transform_file* ottf, texture_database* tex_db);
static void auth_3d_play_control_load(auth_3d* auth, auth_3d_play_control_file* pcf);
static void auth_3d_point_disp(auth_3d_point* p, mat4* parent_mat, render_context* rctx);
static void auth_3d_point_load(auth_3d* auth, auth_3d_point* p, auth_3d_point_file* pf);
static void auth_3d_post_process_load(auth_3d* auth, auth_3d_post_process* pp, auth_3d_post_process_file* ppf);
static void auth_3d_post_process_restore_prev_value(auth_3d_post_process* pp, render_context* rctx);
static void auth_3d_post_process_set(auth_3d_post_process* pp, render_context* rctx);

static bool auth_3d_data_struct_init(auth_3d_data_struct* auth_3d_data, render_context* rctx);
static bool auth_3d_data_struct_check_category_loaded(
    auth_3d_data_struct* auth_3d_data, const char* category_name);
static bool auth_3d_data_struct_check_category_loaded(
    auth_3d_data_struct* auth_3d_data, uint32_t category_hash);
static bool auth_3d_data_struct_ctrl(auth_3d_data_struct* auth_3d_data, render_context* rctx);
static void auth_3d_data_struct_disp(auth_3d_data_struct* auth_3d_data, render_context* rctx);
static auth_3d_farc* auth_3d_data_struct_get_farc(
    auth_3d_data_struct* auth_3d_data, const char* category_name);
static void auth_3d_data_struct_load_category(auth_3d_data_struct* auth_3d_data,
    const char* category_name, const char* mdata_dir);
static void auth_3d_data_struct_load_category(auth_3d_data_struct* auth_3d_data,
    void* data, const char* category_name, uint32_t category_hash);
static void auth_3d_data_struct_unload_category(auth_3d_data_struct* auth_3d_data, const char* category_name);
static void auth_3d_data_struct_unload_category(
    auth_3d_data_struct* auth_3d_data, uint32_t category_hash);

static void auth_3d_farc_load(auth_3d_farc* a3da_farc, const char* mdata_dir);
static void auth_3d_farc_load_modern(auth_3d_farc* a3da_farc, void* data);
static void auth_3d_farc_read_file(auth_3d_farc* a3da_farc, const char* mdata_dir);
static void auth_3d_farc_read_file_modern(auth_3d_farc* a3da_farc, void* data);
static bool auth_3d_farc_read_func(auth_3d_farc* a3da_farc, const void* data, size_t size);
static void auth_3d_farc_reset(auth_3d_farc* a3da_farc);
static void auth_3d_farc_unload(auth_3d_farc* a3da_farc);

static void auth_3d_uid_file_load(auth_3d_uid_file* uid_file);
static void auth_3d_uid_file_modern_load(auth_3d_uid_file_modern* uid_file);
static void auth_3d_uid_file_load_file(auth_3d_uid_file* uid_file, auth_3d_database* auth_3d_db);
static void auth_3d_uid_file_modern_load_file(auth_3d_uid_file_modern* uid_file);
static void auth_3d_uid_file_unload(auth_3d_uid_file* uid_file);
static void auth_3d_uid_file_modern_unload(auth_3d_uid_file_modern* uid_file);
static void auth_3d_uid_file_unload_file(auth_3d_uid_file* uid_file);
static void auth_3d_uid_file_modern_unload_file(auth_3d_uid_file_modern* uid_file);

static size_t auth_3d_time_event_radix_index_func_time(auth_3d_time_event* data, size_t index);

static void Auth3dTestTask__SetAuth3dId(Auth3dTestTask* tt);
static void Auth3dTestTask__SetStage(Auth3dTestTask* tt);

auth_3d_data_struct* auth_3d_data;
auth_3d_detail::TaskAuth3d* task_auth_3d;
Auth3dTestTask* auth_3d_test_task;

static int16_t auth_3d_load_counter;
static auth_3d_detail::FrameRateTimeStop frame_rate_time_stop;

extern render_context* rctx_ptr;

auth_3d::auth_3d() : uid(), id(), mat(), enable(), camera_root_update(), visible(), repeat(), ended(),
left_right_reverse(), once(), alpha(), chara_id(), chara_item(), shadow(), frame_rate(), frame(),
req_frame(), max_frame(), frame_changed(), frame_offset(), last_frame(), paused(), event_time_next() {
    hash = hash_murmurhash_empty;
    src_chara = CHARA_MAX;
    dst_chara = CHARA_MAX;
    reset();
}

auth_3d::~auth_3d() {
    reset();
}

void auth_3d::ctrl(render_context* rctx) {
    if (state == 1)
        if (hash == hash_murmurhash_empty) {
            data_struct* aft_data = &data_list[DATA_AFT];
            auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;
            object_database* aft_obj_db = &aft_data->data_ft.obj_db;
            texture_database* aft_tex_db = &aft_data->data_ft.tex_db;

            auth_3d_database_uid* db_uid = &aft_auth_3d_db->uid[uid];
            if (db_uid->enabled) {
                auth_3d_uid_file* uid_file = &auth_3d_data->uid_files[uid];
                if (uid_file->uid == uid && uid_file->state == 1 && uid_file->farc->state == 2) {
                    state = 2;
                    farc_file* ff = uid_file->farc->farc->read_file(uid_file->file_name.c_str());
                    if (ff) {
                        a3da a;
                        a.read(ff->data, ff->size);
                        load(&a, aft_obj_db, aft_tex_db);
                        frame = 0.0f;
                    }
                }
            }
        }
        else {
            auto elem = auth_3d_data->uid_files_modern.find(hash);
            if (elem == auth_3d_data->uid_files_modern.end())
                return;

            auth_3d_uid_file_modern* uid_file = &elem->second;
            if (uid_file->hash == hash && uid_file->state == 1 && uid_file->farc->state == 2) {
                state = 2;
                farc_file* ff = uid_file->farc->farc->read_file(uid_file->hash);
                if (ff) {
                    a3da a;
                    a.read(ff->data, ff->size);

                    a3da_msgpack_read("patch\\AFT\\auth_3d", a._file_name.c_str(), &a);

                    const char* l_str = ff->name.c_str();
                    const char* t = strrchr(l_str, '.');
                    size_t l_len = ff->name.size();
                    if (t)
                        l_len = t - l_str;

                    uid_file->name.assign(ff->name.c_str(), l_len);
                    uid_file->file_name.assign(ff->name);
                    file_name.assign(ff->name);
                    load(&a, uid_file->obj_db, uid_file->tex_db);
                    hash = uid_file->hash;
                    frame = 0.0f;
                }
            }
        }

    if (state != 2 || !enable)
        return;

    int32_t event_flags = 0;
    bool set = false;
    while (true) {
        if (frame_changed) {
            frame_changed = false;
            if (req_frame < frame) {
                event_time_next = event_time.data();
                event_flags |= 0x02;
            }
            else
                event_flags |= 0x04;
            frame = req_frame;
            ended = false;
            if (max_frame >= 0.0f && frame >= max_frame)
                max_frame = -1.0f;
        }
        else if (!paused) {
            event_flags |= 0x01;
            frame += frame_rate->GetDeltaFrame();
            if (max_frame >= 0.0f && frame > max_frame)
                frame = max_frame;
        }

        float_t frame = this->frame;
        for (auth_3d_point& i : point)
            i.interpolate(frame);

        for (auth_3d_curve& i : curve)
            i.interpolate(frame);

        for (auth_3d_chara& i : chara)
            i.interpolate(frame);

        for (auth_3d_material_list& i : material_list)
            i.interpolate(frame);

        for (auth_3d_object& i : object)
            i.interpolate(frame);

        for (auth_3d_object*& i : object_list)
            i->list_ctrl(&mat);

        for (auth_3d_object_hrc& i : object_hrc)
            i.interpolate(frame);

        for (auth_3d_object_hrc*& i : object_hrc_list)
            i->list_ctrl(&mat);

        for (auth_3d_m_object_hrc& i : m_object_hrc)
            i.interpolate(frame);

        for (auth_3d_m_object_hrc*& i : m_object_hrc_list)
            i->list_ctrl(&mat);

        for (auth_3d_ambient& i : ambient) {
            i.interpolate(frame);
            auth_3d_ambient_set(&i, rctx);
        }

        for (auth_3d_light& i : light) {
            i.interpolate(frame);
            auth_3d_light_set(&i, rctx);
        }

        for (auth_3d_fog& i : fog) {
            i.interpolate(frame);
            auth_3d_fog_set(&i, rctx);
        }

        post_process.interpolate(frame);
        auth_3d_post_process_set(&post_process, rctx);

        auth_3d_time_event* i_event = event_time_next;
        auth_3d_time_event* i_event_end = event_time.data() + event_time.size();
        for (; i_event != i_event_end && frame >= i_event->frame; i_event++) {
            auth_3d_detail::Event* e = event[i_event->event_index];
            if (!i_event->type) {
                printf_debug("  [%s]: [%s][%s]\n",
                    auth_3d_event_type_string(e->type), e->ref.c_str(), e->param1.c_str());
                e->Begin(this, event_flags, rctx);
                e->active = true;
            }
            else if (i_event->type) {
                printf_debug("  [%s]: [%s][%s]\n",
                    auth_3d_event_type_string(e->type), e->ref.c_str(), e->param1.c_str());
                if (e->active) {
                    e->active = false;
                    e->End(this, event_flags, rctx);
                }
            }
        }
        event_time_next = i_event;

        for (auth_3d_detail::Event*& i : event)
            if (i->active)
                i->Ctrl(this, event_flags, rctx);

        camera_auxiliary.interpolate(frame);
        auth_3d_camera_auxiliary_set(&camera_auxiliary, rctx);

        if (camera_root_update)
            for (auth_3d_camera_root& i : camera_root) {
                i.interpolate(auth_3d_camera_root_calc_frame(&i, frame, rctx));

                vec3 interest = i.interest_value;
                vec3 view_point = i.view_point_value;
                float_t fov = i.fov_value;
                float_t roll = i.roll_value;

                if (left_right_reverse) {
                    interest.x = -interest.x;
                    view_point.x = -view_point.x;
                    roll = -roll;
                }

                mat4_mult_vec3_trans(&mat, &interest, &interest);
                mat4_mult_vec3_trans(&mat, &view_point, &view_point);

                camera* cam = rctx->camera;
                cam->set_interest(interest);
                cam->set_view_point(view_point);
                cam->set_fov(fov * RAD_TO_DEG);
                cam->set_roll(roll * RAD_TO_DEG);
                break;
            }

        dof.interpolate(frame);
        auth_3d_dof_set(&dof, rctx);

        if (set || !repeat || last_frame > frame)
            break;

        req_frame = frame - last_frame + frame_offset;
        frame_changed = true;
        set = true;
    }

    bool ended = play_control.size <= frame;
    if (ended && this->ended != ended) {
        paused = true;
        if (once)
            enable = false;
    }
    this->ended = ended;
}

void auth_3d::disp(render_context* rctx) {
    if (state != 2 || !enable)
        return;

    auth_3d_set_material_list(this, rctx);

    for (auth_3d_point& i : point)
        auth_3d_point_disp(&i, &mat, rctx);

    for (auth_3d_chara& i : chara)
        auth_3d_chara_disp(&i, &mat, rctx);

    for (auth_3d_object*& i : object_list)
        auth_3d_object_disp(i, this, rctx);

    for (auth_3d_object_hrc*& i : object_hrc_list)
        auth_3d_object_hrc_disp(i, this, rctx);

    for (auth_3d_m_object_hrc*& i : m_object_hrc_list)
        auth_3d_m_object_hrc_disp(i, this, rctx);

    for (auth_3d_detail::Event*& i : event)
        if (i->active)
            i->Disp(this, &mat, rctx);

    rctx->object_data.set_material_list(0, 0);
}

void auth_3d::load(a3da* auth_file,
    object_database* obj_db, texture_database* tex_db) {
    if (!auth_file)
        return;

    hash = hash_murmurhash_empty;
    auth_3d_play_control_load(this, &auth_file->play_control);

    frame_offset = 0.0f;
    max_frame = play_control.size;
    last_frame = play_control.size;

    this->obj_db = obj_db;
    this->tex_db = tex_db;

    auth_3d_camera_auxiliary_load(this, &auth_file->camera_auxiliary);
    auth_3d_dof_load(this, &dof, &auth_file->dof);
    auth_3d_post_process_load(this, &post_process, &auth_file->post_process);

    if (auth_file->ambient.size()) {
        size_t ambient_index = 0;
        ambient.resize(auth_file->ambient.size());
        for (auth_3d_ambient_file& i : auth_file->ambient)
            auth_3d_ambient_load(this, &ambient[ambient_index++], &i);
    }

    if (auth_file->auth_2d.size()) {
        size_t auth_2d_index = 0;
        auth_2d.resize(auth_file->auth_2d.size());
        for (std::string& i : auth_file->auth_2d)
            auth_2d[auth_2d_index++] = i;
    }

    if (auth_file->camera_root.size()) {
        size_t camera_root_index = 0;
        camera_root.resize(auth_file->camera_root.size());
        for (auth_3d_camera_root_file& i : auth_file->camera_root)
            auth_3d_camera_root_load(this, &camera_root[camera_root_index++], &i);
    }

    if (auth_file->chara.size()) {
        size_t chara_index = 0;
        chara.resize(auth_file->chara.size());
        for (auth_3d_chara_file& i : auth_file->chara)
            auth_3d_chara_load(this, &chara[chara_index++], &i);
    }

    if (auth_file->curve.size()) {
        size_t curve_index = 0;
        curve.resize(auth_file->curve.size());
        for (auth_3d_curve_file& i : auth_file->curve)
            auth_3d_curve_load(this, &curve[curve_index++], &i);
    }

    if (auth_file->event.size()) {
        event.reserve(auth_file->event.size());
        for (auth_3d_event_file& i : auth_file->event) {
            auth_3d_detail::Event* e = auth_3d_event_load(this, &i);
            if (e)
                event.push_back(e);
        }
    }

    if (auth_file->fog.size()) {
        size_t fog_index = 0;
        fog.resize(auth_file->fog.size());
        for (auth_3d_fog_file& i : auth_file->fog)
            auth_3d_fog_load(this, &fog[fog_index++], &i);
    }

    if (auth_file->light.size()) {
        size_t light_index = 0;
        light.resize(auth_file->light.size());
        for (auth_3d_light_file& i : auth_file->light)
            auth_3d_light_load(this, &light[light_index++], &i);
    }

    if (auth_file->m_object_hrc.size()) {
        size_t m_object_hrc_index = 0;
        m_object_hrc.resize(auth_file->m_object_hrc.size());
        for (auth_3d_m_object_hrc_file& i : auth_file->m_object_hrc)
            auth_3d_m_object_hrc_load(this, &m_object_hrc[m_object_hrc_index++], &i, obj_db);
    }

    if (auth_file->material_list.size()) {
        size_t material_list_index = 0;
        material_list.resize(auth_file->material_list.size());
        for (auth_3d_material_list_file& i : auth_file->material_list)
            auth_3d_material_list_load(this, &material_list[material_list_index++], &i);
    }

    if (auth_file->motion.size()) {
        size_t motion_index = 0;
        motion.resize(auth_file->motion.size());
        for (std::string& i : auth_file->motion)
            motion[motion_index++] = i;
    }

    if (auth_file->object.size()) {
        size_t object_index = 0;
        object.resize(auth_file->object.size());
        for (auth_3d_object_file& i : auth_file->object)
            auth_3d_object_load(this, &object[object_index++], &i, obj_db, tex_db);
    }

    if (auth_file->object_hrc.size()) {
        size_t object_hrc_index = 0;
        object_hrc.resize(auth_file->object_hrc.size());
        for (auth_3d_object_hrc_file& i : auth_file->object_hrc)
            auth_3d_object_hrc_load(this, &object_hrc[object_hrc_index++], &i, obj_db);

        for (auth_3d_object_hrc& i : object_hrc)
            auth_3d_object_hrc_replace_chara(&i, (chara_index)src_chara, (chara_index)dst_chara, obj_db);
    }

    if (auth_file->point.size()) {
        size_t point_index = 0;
        point.resize(auth_file->point.size());
        for (auth_3d_point_file& i : auth_file->point)
            auth_3d_point_load(this, &point[point_index++], &i);
    }

    if (auth_file->m_object_hrc_list.size()) {
        size_t m_object_hrc_list_index = 0;
        m_object_hrc_list.resize(auth_file->m_object_hrc_list.size());
        for (std::string& i : auth_file->m_object_hrc_list)
            for (auth_3d_m_object_hrc& j : m_object_hrc)
                if (!i.compare(j.name)) {
                    m_object_hrc_list[m_object_hrc_list_index++] = &j;
                    break;
                }
        m_object_hrc_list.resize(m_object_hrc_list_index);
    }

    if (auth_file->object_list.size()) {
        size_t object_list_index = 0;
        object_list.resize(auth_file->object_list.size());
        for (std::string& i : auth_file->object_list)
            for (auth_3d_object& j : object)
                if (!i.compare(j.name)) {
                    object_list[object_list_index++] = &j;
                    break;
                }
        object_list.resize(object_list_index);
    }

    if (auth_file->object_hrc_list.size()) {
        size_t object_hrc_list_index = 0;
        object_hrc_list.resize(auth_file->object_hrc_list.size());
        for (std::string& i : auth_file->object_hrc_list)
            for (auth_3d_object_hrc& j : object_hrc)
                if (!i.compare(j.name)) {
                    object_hrc_list[object_hrc_list_index++] = &j;
                    break;
                }
        object_hrc_list.resize(object_hrc_list_index);
    }

    if (object.size()) {
        for (auth_3d_object& i : object) {
            if (!i.parent_name.size())
                continue;

            if (i.parent_node.size())
                for (auth_3d_object_hrc& j : object_hrc) {
                    if (i.parent_name.compare(j.name))
                        continue;

                    int32_t node_index = j.get_node_index(i.parent_node.c_str());
                    if (node_index > -1) {
                        j.children_object_parent_node.push_back(node_index);
                        j.children_object.push_back(&i);
                    }
                    break;
                }
            else
                for (auth_3d_object& j : object) {
                    if (i.parent_name.compare(j.name))
                        continue;

                    j.children_object.push_back(&i);
                    break;
                }
        }
    }

    if (object_hrc.size()) {
        for (auth_3d_object_hrc& i : object_hrc) {
            if (!i.parent_name.size())
                continue;

            if (i.parent_node.size())
                for (auth_3d_object_hrc& j : object_hrc) {
                    if (i.parent_name.compare(j.name))
                        continue;

                    int32_t node_index = j.get_node_index(i.parent_node.c_str());
                    if (node_index > -1) {
                        j.children_object_hrc_parent_node.push_back(node_index);
                        j.children_object_hrc.push_back(&i);
                    }
                    break;
                }
            else
                for (auth_3d_object& j : object) {
                    if (i.parent_name.compare(j.name))
                        continue;

                    j.children_object_hrc.push_back(&i);
                    break;
                }
        }
    }

    if (event.size()) {
        for (auth_3d_detail::Event*& i : event)
            if (i)
                i->Load(this);

        size_t event_index = 0;
        event_time.clear();
        event_time.shrink_to_fit();
        event_time.reserve(event.size() * 2);
        for (auth_3d_detail::Event*& i : event) {
            event_time.push_back({ i->begin, 0, event_index });
            event_time.push_back({ i->end  , 1, event_index });
            event_index++;
        }

        radix_sort_custom(event_time.data(), event_time.size(), sizeof(auth_3d_time_event),
            sizeof(float_t), (radix_index_func)auth_3d_time_event_radix_index_func_time);

        event_time_next = event_time.data();
    }

    file_name.assign(auth_file->_file_name);
}

void auth_3d::load_from_farc(farc* f, const char* file,
    object_database* obj_db, texture_database* tex_db) {
    farc_file* ff = f->read_file(file);
    if (!ff)
        return;

    const char* l_str = ff->name.c_str();
    const char* t = strrchr(l_str, '.');
    size_t l_len = ff->name.size();
    if (t)
        l_len = t - l_str;

    uint32_t h = hash_murmurhash(l_str, l_len);

    a3da a;
    a.read(ff->data, ff->size);
    load(&a, obj_db, tex_db);
}

void auth_3d::reset() {
    uid = -1;
    id = -1;
    mat = mat4_identity;
    enable = false;
    camera_root_update = true;
    visible = true;
    repeat = false;
    ended = false;
    left_right_reverse = false;
    once = false;
    mat = mat4_identity;
    alpha = 1.0f;
    draw_task_flags = DRAW_TASK_ALPHA_ORDER_1;
    chara_id = -1;
    chara_item = false;
    shadow = false;
    src_chara = CHARA_MAX;
    dst_chara = CHARA_MAX;
    frame_rate = &sys_frame_rate;
    frame = 0.0f;
    req_frame = 0.0f;
    max_frame = -1.0f;
    frame_changed = true;
    frame_offset = 0.0f;
    last_frame = 0.0f;
    paused = true;
    ambient.clear();
    ambient.shrink_to_fit();
    auth_2d.clear();
    auth_2d.shrink_to_fit();
    camera_auxiliary.reset();
    camera_root.clear();
    camera_root.shrink_to_fit();
    chara.clear();
    chara.shrink_to_fit();
    curve.clear();
    curve.shrink_to_fit();
    dof.reset();
    for (auth_3d_detail::Event*& i : event)
        if (i) {
            delete i;
            i = 0;
        }
    event.clear();
    event.shrink_to_fit();
    event_time.clear();
    event_time.shrink_to_fit();
    event_time_next = 0;
    fog.clear();
    fog.shrink_to_fit();
    light.clear();
    light.shrink_to_fit();
    m_object_hrc.clear();
    m_object_hrc.shrink_to_fit();
    m_object_hrc_list.clear();
    m_object_hrc_list.shrink_to_fit();
    material_list.clear();
    material_list.shrink_to_fit();
    motion.clear();
    motion.shrink_to_fit();
    object.clear();
    object.shrink_to_fit();
    object_hrc.clear();
    object_hrc.shrink_to_fit();
    object_hrc_list.clear();
    object_hrc_list.shrink_to_fit();
    object_list.clear();
    object_list.shrink_to_fit();
    play_control.reset();
    point.clear();
    point.shrink_to_fit();
    post_process.reset();
    hash = hash_murmurhash_empty;
}

void auth_3d::unload(render_context* rctx) {
    auth_3d_camera_auxiliary_restore_prev_value(&camera_auxiliary, rctx);
    for (auth_3d_light& i : light)
        auth_3d_light_restore_prev_value(&i, rctx);
    for (auth_3d_fog& i : fog)
        auth_3d_fog_restore_prev_value(&i, rctx);
    auth_3d_dof_restore_prev_value(&dof, rctx);
    auth_3d_post_process_restore_prev_value(&post_process, rctx);

    if (state)
        if (hash == hash_murmurhash_empty) {
            data_struct* aft_data = &data_list[DATA_AFT];
            auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;

            if (aft_auth_3d_db->uid[uid].enabled) {
                auth_3d_uid_file* uid_file = &auth_3d_data->uid_files[uid];
                if (uid_file->uid == uid)
                    auth_3d_uid_file_unload(uid_file);
            }
        }
        else {
            auto elem = auth_3d_data->uid_files_modern.find(hash);
            if (elem == auth_3d_data->uid_files_modern.end())
                return;

            auth_3d_uid_file_modern* uid_file = &elem->second;
            if (uid_file->hash == hash)
                auth_3d_uid_file_modern_unload(uid_file);
        }
    reset();
}

namespace auth_3d_detail {
    Event::Event(a3da_event* e) {
        active = false;
        switch (e->type) {
        case A3DA_EVENT_MISC:
        default:
            type = Event::Type::MISC;
        case A3DA_EVENT_FILT:
            type = Event::Type::FILT;
        case A3DA_EVENT_FX:
            type = Event::Type::FX;
        case A3DA_EVENT_SND:
            type = Event::Type::SND;
        case A3DA_EVENT_MOT:
            type = Event::Type::MOT;
        case A3DA_EVENT_A2D:
            type = Event::Type::A2D;
        }
        name.assign(e->name);
        begin = e->begin;
        end = e->end;
        param1.assign(e->param1);
        ref.assign(e->ref);
        index = -1;
    }

    Event::~Event() {

    }

    void Event::Load(auth_3d* auth) {

    }

    void Event::Begin(auth_3d* auth, int32_t flags, render_context* rctx) {

    }

    void Event::End(auth_3d* auth, int32_t flags, render_context* rctx) {

    }

    void Event::Ctrl(auth_3d* auth, int32_t flags, render_context* rctx) {

    }

    void Event::Disp(auth_3d* auth, const mat4* mat, render_context* rctx) {

    }

    EventA2d::EventA2d(a3da_event* e) : Event(e) {

    }

    EventA2d::~EventA2d() {

    }

    void EventA2d::Begin(auth_3d* auth, int32_t flags, render_context* rctx) {

    }

    void EventA2d::End(auth_3d* auth, int32_t flags, render_context* rctx) {

    }

    void EventA2d::Ctrl(auth_3d* auth, int32_t flags, render_context* rctx) {

    }

    void EventA2d::Disp(auth_3d* auth, const mat4* mat, render_context* rctx) {

    }

    EventFX::EventFX(a3da_event* e) : Event(e) {

    }

    EventFX::~EventFX() {

    }

    void EventFX::Load(auth_3d* auth) {
        if (param1.compare("DOWN") && param1.compare("KABEHIT1") && param1.compare("PAPER1"))
            return;

        index = -1;
        for (auth_3d_point& i : auth->point)
            if (!i.name.compare(ref)) {
                index = &i - auth->point.data();
                break;
            }
    }

    void EventFX::Begin(auth_3d* auth, int32_t flags, render_context* rctx) {
        printf_debug("  [FX]: %s\n", param1.c_str());
    }

    void EventFX::Ctrl(auth_3d* auth, int32_t flags, render_context* rctx) {
        if (index >= auth->point.size() || auth->frame < begin || auth->frame >= end || param1.compare("PAPER1"))
            return;

        auth_3d_point& point = auth->point[index];
        vec3 trans = point.model_transform.translation_value;
        if (auth->left_right_reverse)
            trans.x = -trans.x;

        mat4_mult_vec3_trans(&auth->mat, &trans, &trans);
        particle_event_data event;
        event.type = 1.0f;
        event.count = point.model_transform.scale_value.x * 100.0f;
        event.size = point.model_transform.scale_value.z;
        event.trans = trans;
        event.force = point.model_transform.scale_value.y;
        task_effect_parent_event(TASK_EFFECT_PARTICLE, 1, &event);
    }

    EventFXSmoothCut::EventFXSmoothCut(a3da_event* e) : Event(e), cam() {

    }

    EventFXSmoothCut::~EventFXSmoothCut() {

    }

    void EventFXSmoothCut::Load(auth_3d* auth) {
        if (!ref.compare("CAM")) {
            cam = true;
            return;
        }

        index = -1;
        for (auth_3d_point& i : auth->point)
            if (!i.name.compare(ref)) {
                index = &i - auth->point.data();
                break;
            }
    }

    void EventFXSmoothCut::Begin(auth_3d* auth, int32_t flags, render_context* rctx) {
        if (cam || !auth->chara.size() || index >= auth->chara.size())
            return;

        int32_t chr_index = auth->chara[index].index;
        rob_chara* rob_chr = rob_chara_array_get(chr_index);
        if (rob_chr)
            rob_chr->field_C = true;
    }

    void EventFXSmoothCut::End(auth_3d* auth, int32_t flags, render_context* rctx) {

    }

    void EventFXSmoothCut::Disp(auth_3d* auth, const mat4* mat, render_context* rctx) {

    }

    EventFilterFade::EventFilterFade(a3da_event* e) : Event(e) {
        type = Type::FADE_NONE;
    }

    EventFilterFade::~EventFilterFade() {

    }

    void EventFilterFade::Begin(auth_3d* auth, int32_t flags, render_context* rctx) {

    }

    void EventFilterFade::End(auth_3d* auth, int32_t flags, render_context* rctx) {
        switch (type) {
        case EventFilterFade::Type::FADE_OUT:
        case EventFilterFade::Type::FADE_IN:
            break;
        default:
            return;
        }

        rctx->post_process.tone_map->set_scene_fade_color(0.0f);
        rctx->post_process.tone_map->set_scene_fade_alpha(0.0f);
        rctx->post_process.tone_map->set_scene_fade_blend_func(0);
    }

    void EventFilterFade::Disp(auth_3d* auth, const mat4* mat, render_context* rctx) {
        switch (type) {
        case EventFilterFade::Type::FADE_OUT:
        case EventFilterFade::Type::FADE_IN:
            break;
        default:
            return;
        }

        float_t t = (auth->frame - begin) / (end - begin);

        float_t alpha;
        if (type == EventFilterFade::Type::FADE_IN)
            alpha = t;
        else
            alpha = 1.0f - t;

        rctx->post_process.tone_map->set_scene_fade_color(color);
        rctx->post_process.tone_map->set_scene_fade_alpha(alpha);
    }

    EventFilterTimeStop::EventFilterTimeStop(a3da_event* e) : Event(e) {

    }

    EventFilterTimeStop::~EventFilterTimeStop() {

    }

    void EventFilterTimeStop::Begin(auth_3d* auth, int32_t flags, render_context* rctx) {
        for (auth_3d_chara& i : auth->chara) {
            rob_chara* rob_chr = rob_chara_array_get(i.index);
            if (rob_chr)
                rob_chr->item_equip->disable_update = true;
        }
        task_effect_parent_set_frame_rate_control(&frame_rate_time_stop);
    }

    void EventFilterTimeStop::End(auth_3d* auth, int32_t flags, render_context* rctx) {
        for (auth_3d_chara& i : auth->chara) {
            rob_chara* rob_chr = rob_chara_array_get(i.index);
            if (rob_chr)
                rob_chr->item_equip->disable_update = true;
        }
        task_effect_parent_set_frame_rate_control(&sys_frame_rate);
    }

    void EventFilterTimeStop::Disp(auth_3d* auth, const mat4* mat, render_context* rctx) {

    }

    EventMot::EventMot(a3da_event* e) : Event(e) {

    }

    EventMot::~EventMot() {

    }

    void EventMot::Load(auth_3d* auth) {
        index = -1;
        for (auth_3d_chara& i : auth->chara)
            if (!i.name.compare(ref)) {
                index = &i - auth->chara.data();
                break;
            }
    }

    void EventMot::Begin(auth_3d* auth, int32_t flags, render_context* rctx) {

    }

    void EventMot::End(auth_3d* auth, int32_t flags, render_context* rctx) {

    }

    void EventMot::Ctrl(auth_3d* auth, int32_t flags, render_context* rctx) {

    }

    void EventMot::Disp(auth_3d* auth, const mat4* mat, render_context* rctx) {

    }

    EventSnd::EventSnd(a3da_event* e) : Event(e) {
        field_190 = param1.find("zz", 0, 2) == 0;
    }

    EventSnd::~EventSnd() {

    }

    void EventSnd::Load(auth_3d* auth) {

    }

    static void sub_140532980(int32_t chara_id, const char* name) {

    }

    void EventSnd::Begin(auth_3d* auth, int32_t flags, render_context* rctx) {
        float_t frame = auth->frame;
        if (begin > frame || frame >= end || ((flags & 0x02) && fabsf(frame - begin) > 0.1f))
            return;

        int32_t index = -1;
        if (auth->chara.size())
            index = auth->chara.front().index;

        if (index >= 0 && index < ROB_CHARA_COUNT)
            sub_140532980(index, param1.c_str());
        else if (sound_work_has_property(param1.c_str()))
            sound_work_play_se(1, param1.c_str(), 1.0f);
    }

    void EventSnd::End(auth_3d* auth, int32_t flags, render_context* rctx) {

    }

    void EventSnd::Disp(auth_3d* auth, const mat4* mat, render_context* rctx) {

    }

}

auth_3d_key::auth_3d_key() : type(), value(), max_frame(), ep_type_pre(), ep_type_post(),
frame_delta(), value_delta(), length(), keys(), frame(), value_interp(), value_init() {
    reset();
}

auth_3d_key::~auth_3d_key() {
    reset();
}

float_t auth_3d_key::interpolate(float_t frame) {
    if (type == AUTH_3D_KEY_STATIC)
        return value;
    else if (type < AUTH_3D_KEY_STATIC || type > AUTH_3D_KEY_HOLD || !length)
        return 0.0f;

    kft3* first_key = &keys[0];
    kft3* last_key = &keys[length - 1];
    float_t first_frame = first_key->frame;
    float_t last_frame = last_key->frame;

    float_t offset = 0.0f;
    if (frame < first_frame) {
        float_t delta_frame = first_frame - frame;
        switch (ep_type_pre) {
        case A3DA_EP_NONE:
        default:
            this->frame = frame;
            value_interp = first_key->value;
            return value_interp;
        case A3DA_EP_LINEAR:
            this->frame = frame;
            value_interp = first_key->value + delta_frame * first_key->tangent1;
            return value_interp;
        case A3DA_EP_CYCLE:
            frame = last_frame - fmodf(delta_frame, frame_delta);
            break;
        case A3DA_EP_CYCLE_OFFSET:
            offset = -(float_t)((int32_t)(delta_frame / frame_delta) + 1) * value_delta;
            frame = last_frame - fmodf(delta_frame, frame_delta);
            break;
        }
    }
    else if (frame >= last_frame) {
        float_t delta_frame = frame - last_frame;
        switch (ep_type_post) {
        case A3DA_EP_NONE:
        default:
            this->frame = frame;
            value_interp = last_key->value;
            return value_interp;
        case A3DA_EP_LINEAR:
            this->frame = frame;
            value_interp = last_key->value - delta_frame * last_key->tangent2;
            return value_interp;
        case A3DA_EP_CYCLE:
            frame = first_frame + fmodf(delta_frame, frame_delta);
            break;
        case A3DA_EP_CYCLE_OFFSET:
            offset = (float_t)((int32_t)(delta_frame / frame_delta) + 1) * value_delta;
            frame = first_frame + fmodf(delta_frame, frame_delta);
            break;
        }
    }

    this->frame = frame;
    value_interp = auth_3d_key_interpolate_inner(this, frame) + offset;
    return value_interp;
}

void auth_3d_key::reset() {
    type = AUTH_3D_KEY_NONE;
    value = 0.0f;
    max_frame = 0.0f;
    ep_type_pre = AUTH_3D_EP_NONE;
    ep_type_post = AUTH_3D_EP_NONE;
    frame_delta = 0.0f;
    value_delta = 0.0f;
    keys_vec.clear();
    keys_vec.shrink_to_fit();
    length = 0;
    keys = 0;
    frame = 0.0f;
    value_interp = 0.0f;
    value_init = 0.0f;
}

auth_3d_key& auth_3d_key::operator=(const auth_3d_key& k) {
    type = k.type;
    value = k.value;
    max_frame = k.max_frame;
    ep_type_pre = k.ep_type_pre;
    ep_type_post = k.ep_type_post;
    frame_delta = k.frame_delta;
    value_delta = k.value_delta;
    keys_vec.assign(k.keys_vec.begin(), k.keys_vec.end());
    length = k.length;
    keys = keys_vec.data();

    frame = k.frame;
    value_interp = k.value_interp;
    value_init = k.value_init;
    return *this;
}

auth_3d_rgba::auth_3d_rgba() : flags(), value() {
    reset();
}

auth_3d_rgba::~auth_3d_rgba() {
    reset();
}

void auth_3d_rgba::interpolate(float_t frame){
    if (flags & A3DA_RGBA_R)
        value.x = r.interpolate(frame);

    if (flags & A3DA_RGBA_G)
        value.y = g.interpolate(frame);

    if (flags & A3DA_RGBA_B)
        value.z = b.interpolate(frame);

    if (flags & A3DA_RGBA_A)
        value.w = a.interpolate(frame);
}

void auth_3d_rgba::reset() {
    flags = (auth_3d_rgba_flags)0;
    r.reset();
    g.reset();
    b.reset();
    a.reset();
    value = 0.0f;
}

auth_3d_vec3::auth_3d_vec3() {
    reset();
}

auth_3d_vec3::~auth_3d_vec3() {
    reset();
}

vec3 auth_3d_vec3::interpolate(float_t frame) {
    vec3 value;
    value.x = x.interpolate(frame);
    value.y = y.interpolate(frame);
    value.z = z.interpolate(frame);
    return value;
}

void auth_3d_vec3::reset() {
    x.reset();
    y.reset();
    z.reset();
}

auth_3d_model_transform::auth_3d_model_transform() : mat(),
scale_value(), rotation_value(), translation_value(), visible() {
    reset();
}

auth_3d_model_transform::~auth_3d_model_transform() {
    reset();
}

void auth_3d_model_transform::interpolate(float_t frame) {
    scale_value = scale.interpolate(frame);
    rotation_value = rotation.interpolate(frame);
    translation_value = translation.interpolate(frame);
    visible = visibility.interpolate(frame) >= 0.999f;
    if (visible && (scale_value.x == 0.0f || scale_value.y == 0.0f || scale_value.z == 0.0f))
        visible = false;
}

void auth_3d_model_transform::reset() {
    mat = mat4_identity;
    scale.reset();
    rotation.reset();
    translation.reset();
    visibility.reset();
    scale_value = 0.0f;
    rotation_value = 0.0f;
    translation_value = 0.0f;
    visible = false;
}

void auth_3d_model_transform::set_mat(const mat4* parent_mat) {
    mat4 mat;
    mat4_translate_mult(parent_mat, &translation_value, &mat);
    mat4_rotate_mult(&mat, &rotation_value, &mat);
    mat4_scale_rot(&mat, &scale_value, &this->mat);
}

auth_3d_ambient::auth_3d_ambient() : flags() {
    reset();
}

auth_3d_ambient::~auth_3d_ambient() {
    reset();
}

void auth_3d_ambient::interpolate(float_t frame) {
    if (flags & AUTH_3D_AMBIENT_LIGHT_DIFFUSE)
        light_diffuse.interpolate(frame);

    if (flags & AUTH_3D_AMBIENT_LIGHT_DIFFUSE)
        rim_light_diffuse.interpolate(frame);
}

void auth_3d_ambient::reset() {
    flags = (auth_3d_ambient_flags)0;
    light_diffuse.reset();
    name.clear();
    name.shrink_to_fit();
    rim_light_diffuse.reset();
}

auth_3d_camera_auxiliary::auth_3d_camera_auxiliary() : flags(), auto_exposure_value(),
exposure_value(), exposure_rate_value(), gamma_value(), gamma_rate_value(), saturate_value() {
    reset();
}

auth_3d_camera_auxiliary::~auth_3d_camera_auxiliary() {
    reset();
}

void auth_3d_camera_auxiliary::interpolate(float_t frame) {
    if (flags & AUTH_3D_CAMERA_AUXILIARY_AUTO_EXPOSURE)
        auto_exposure_value = auto_exposure.interpolate(frame);
    if (flags & AUTH_3D_CAMERA_AUXILIARY_EXPOSURE)
        exposure_value = exposure.interpolate(frame);
    if (flags & AUTH_3D_CAMERA_AUXILIARY_EXPOSURE_RATE)
        exposure_rate_value = exposure_rate.interpolate(frame);
    if (flags & AUTH_3D_CAMERA_AUXILIARY_GAMMA)
        gamma_value = gamma.interpolate(frame);
    if (flags & AUTH_3D_CAMERA_AUXILIARY_GAMMA_RATE)
        gamma_rate_value = gamma_rate.interpolate(frame);
    if (flags & AUTH_3D_CAMERA_AUXILIARY_SATURATE)
        saturate_value = saturate.interpolate(frame);
}

void auth_3d_camera_auxiliary::reset() {
    flags = (auth_3d_camera_auxiliary_flags)0;
    auto_exposure.reset();
    exposure.reset();
    exposure_rate.reset();
    gamma.reset();
    gamma_rate.reset();
    saturate.reset();
    auto_exposure_value = 1.0f;
    exposure_value = 2.0f;
    exposure_rate_value = 1.0f;
    gamma_value = 1.0f;
    gamma_rate_value = 1.0f;
    saturate_value = 1.0f;
}

auth_3d_camera_root::auth_3d_camera_root() : fov_value(),
interest_value(), roll_value(), view_point_value() {
    reset();
}

void auth_3d_camera_root::interpolate(float_t frame) {
    model_transform.interpolate(frame);
    model_transform.set_mat(&mat4_identity);

    mat4& cr_mat = model_transform.mat;

    vec3 _view_point = view_point.model_transform.translation.interpolate(frame);
    mat4_mult_vec3_trans(&cr_mat, &_view_point, &view_point_value);

    vec3 _interest = interest.translation.interpolate(frame);
    mat4_mult_vec3_trans(&cr_mat, &_interest, &interest_value);

    float_t fov;
    if (view_point.flags & AUTH_3D_CAMERA_ROOT_VIEW_POINT_FOV) {
        fov = view_point.fov.interpolate(frame);
        if (view_point.fov_is_horizontal)
            fov = atanf(tanf(fov * 0.5f) / view_point.aspect) * 2.0f;
    }
    else {
        float_t camera_aperture_h = view_point.camera_aperture_h;
        float_t focal_lenth = view_point.focal_length.interpolate(frame);
        fov = atanf((camera_aperture_h * 25.4f) * 0.5f / focal_lenth) * 2.0f;
    }
    fov_value = fov;

    if (view_point.flags & AUTH_3D_CAMERA_ROOT_VIEW_POINT_ROLL)
        roll_value = view_point.roll.interpolate(frame);
}

void auth_3d_camera_root::reset() {
    interest.reset();
    model_transform.reset();
    view_point.reset();
    fov_value = 0.0f;
    interest_value = 0.0f;
    roll_value = 0.0f;
    view_point_value = 0.0f;
}

auth_3d_camera_root::~auth_3d_camera_root() {
    reset();
}

auth_3d_camera_root_view_point::auth_3d_camera_root_view_point() : flags(),
aspect(), camera_aperture_h(), camera_aperture_w(), fov_is_horizontal() {
    reset();
}

auth_3d_camera_root_view_point::~auth_3d_camera_root_view_point() {
    reset();
}

void auth_3d_camera_root_view_point::reset() {
    flags = (auth_3d_camera_root_view_point_flags)0;
    aspect = 0.0f;
    camera_aperture_h = 0.0f;
    camera_aperture_w = 0.0f;
    focal_length.reset();
    fov.reset();
    fov_is_horizontal = false;
    model_transform.reset();
    roll.reset();
}

auth_3d_chara::auth_3d_chara() : index() {
    reset();
}

auth_3d_chara::~auth_3d_chara() {
    reset();
}

void auth_3d_chara::interpolate(float_t frame) {
    model_transform.interpolate(frame);
}

void auth_3d_chara::reset() {
    model_transform.reset();
    name.clear();
    name.shrink_to_fit();
    index = -1;
}

auth_3d_curve::auth_3d_curve() : value() {
    reset();
}

auth_3d_curve::~auth_3d_curve() {
    reset();
}

void auth_3d_curve::interpolate(float_t frame) {
    value = curve.interpolate(frame);
}

void auth_3d_curve::reset() {
    curve.reset();
    name.clear();
    name.shrink_to_fit();
    value = 0.0f;
}

auth_3d_dof::auth_3d_dof() : has_dof() {
    reset();
}

auth_3d_dof::~auth_3d_dof() {
    reset();
}

void auth_3d_dof::interpolate(float_t frame) {
    if (!has_dof)
        return;

    model_transform.interpolate(frame);
}

void auth_3d_dof::reset() {
    has_dof = false;
    model_transform.reset();
}

auth_3d_fog::auth_3d_fog() : flags(), flags_init(), id(), density_value(),
end_value(), start_value(), color_init(), density_init(), end_init(), start_init() {
    reset();
}

auth_3d_fog::~auth_3d_fog() {
    reset();
}

void auth_3d_fog::interpolate(float_t frame) {
    if (flags & AUTH_3D_FOG_DENSITY)
        density_value = density.interpolate(frame);
    if (flags & AUTH_3D_FOG_START)
        start_value = start.interpolate(frame);
    if (flags & AUTH_3D_FOG_END)
        end_value = end.interpolate(frame);
    if (flags & AUTH_3D_FOG_COLOR)
        color.interpolate(frame);
}

void auth_3d_fog::reset() {
    flags = (auth_3d_fog_flags)0;
    flags_init = (auth_3d_fog_flags)0;
    color.reset();
    density.reset();
    end.reset();
    id = (fog_id)0;
    start.reset();

    density_value = 0.0f;
    end_value = 0.0f;
    start_value = 0.0f;

    color_init = 0.0f;
    density_init = 0.0f;
    end_init = 0.0f;
    start_init = 0.0f;
}

auth_3d_light::auth_3d_light() : flags(), flags_init(), id(), cone_angle_value(),
constant_value(), drop_off_value(), far_value(), intensity_value(), linear_value(),
quadratic_value(), ambient_init(), cone_angle_init(), constant_init(), diffuse_init(), drop_off_init(),
far_init(), intensity_init(), linear_init(), quadratic_init(), specular_init(), tone_curve_init() {
    reset();
}

auth_3d_light::~auth_3d_light() {
    reset();
}

void auth_3d_light::interpolate(float_t frame) {
    if (flags & AUTH_3D_LIGHT_AMBIENT)
        ambient.interpolate(frame);
    if (flags & AUTH_3D_LIGHT_CONE_ANGLE)
        cone_angle_value = cone_angle.interpolate(frame);
    if (flags & AUTH_3D_LIGHT_CONSTANT)
        constant_value = constant.interpolate(frame);
    if (flags & AUTH_3D_LIGHT_DIFFUSE)
        diffuse.interpolate(frame);
    if (flags & AUTH_3D_LIGHT_DROP_OFF)
        drop_off_value = drop_off.interpolate(frame);
    if (flags & AUTH_3D_LIGHT_FAR)
        far_value = _far.interpolate(frame);
    if (flags & AUTH_3D_LIGHT_INTENSITY)
        intensity_value = intensity.interpolate(frame);
    if (flags & AUTH_3D_LIGHT_LINEAR)
        linear_value = linear.interpolate(frame);
    if (flags & AUTH_3D_LIGHT_POSITION)
        position.interpolate(frame);
    if (flags & AUTH_3D_LIGHT_QUADRATIC)
        quadratic_value = quadratic.interpolate(frame);
    if (flags & AUTH_3D_LIGHT_SPECULAR)
        specular.interpolate(frame);
    if (flags & AUTH_3D_LIGHT_SPOT_DIRECTION)
        spot_direction.interpolate(frame);
    if (flags & AUTH_3D_LIGHT_TONE_CURVE)
        tone_curve.interpolate(frame);
}

void auth_3d_light::reset() {
    flags = (auth_3d_light_flags)0;
    flags_init = (auth_3d_light_flags)0;
    ambient.reset();
    cone_angle.reset();
    constant.reset();
    diffuse.reset();
    drop_off.reset();
    _far.reset();
    id = (light_id)0;
    intensity.reset();
    linear.reset();
    position.reset();
    quadratic.reset();
    specular.reset();
    spot_direction.reset();
    tone_curve.reset();
    type.clear();
    type.shrink_to_fit();
    cone_angle_value = 0.0f;
    constant_value = 0.0f;
    drop_off_value = 0.0f;
    far_value = 0.0f;
    intensity_value = 0.0f;
    linear_value = 0.0f;
    quadratic_value = 0.0f;
    ambient_init = 0.0f;
    cone_angle_init = 0.0f;
    constant_init = 0.0f;
    diffuse_init = 0.0f;
    drop_off_init = 0.0f;
    far_init = 0.0f;
    intensity_init = 0.0f;
    linear_init = 0.0f;
    quadratic_init = 0.0f;
    specular_init = 0.0f;
    tone_curve_init = {};
}

auth_3d_m_object_hrc::auth_3d_m_object_hrc() {
    reset();
}

auth_3d_m_object_hrc::~auth_3d_m_object_hrc() {
    reset();
}

void auth_3d_m_object_hrc::interpolate(float_t frame) {
    model_transform.interpolate(frame);
    for (auth_3d_object_instance& i : instance)
        i.interpolate(frame);
    for (auth_3d_object_node& i : node)
        i.interpolate(frame);
}

void auth_3d_m_object_hrc::list_ctrl(const mat4* parent_mat) {
    //if (!model_transform.visible)
    //    return;

    auth_3d_object_node* node = this->node.data();

    auth_3d_m_object_hrc_nodes_mat_mult(this);

    this->model_transform.mat_mult(parent_mat);

    mat4& mat = this->model_transform.mat;

    for (auth_3d_object_instance& i : this->instance) {
        //if (!i.model_transform.visible)
        //    continue;

        i.model_transform.mat_mult(&mat);
        int32_t* object_bone_indices = i.object_bone_indices.data();
        size_t object_bone_indices_count = i.object_bone_indices.size();
        mat4* mats = i.mats.data();
        for (size_t j = 0; j < object_bone_indices_count; j++)
            mats[j] = node[object_bone_indices[j]].model_transform.mat;
    }
}

void auth_3d_m_object_hrc::reset() {
    for (auth_3d_object_instance& i : instance)
        i.reset();
    instance.clear();
    instance.shrink_to_fit();
    model_transform.reset();
    name.clear();
    name.shrink_to_fit();
    for (auth_3d_object_node& i : node)
        i.reset();
    node.clear();
    node.shrink_to_fit();
}

auth_3d_material_list::auth_3d_material_list() : flags(), glow_intensity_value() {
    reset();
}

auth_3d_material_list::~auth_3d_material_list() {
    reset();
}

void auth_3d_material_list::interpolate(float_t frame) {
    if (flags & AUTH_3D_MATERIAL_LIST_BLEND_COLOR)
        blend_color.interpolate(frame);
    if (flags & AUTH_3D_MATERIAL_LIST_GLOW_INTENSITY)
        glow_intensity_value = glow_intensity.interpolate(frame);
    if (flags & AUTH_3D_MATERIAL_LIST_INCANDESCENCE)
        incandescence.interpolate(frame);
}

void auth_3d_material_list::reset() {
    flags = (auth_3d_material_list_flags)0;
    blend_color.reset();
    glow_intensity.reset();
    incandescence.reset();
    name.clear();
    name.shrink_to_fit();
    glow_intensity_value = 0.0f;
}

auth_3d_object::auth_3d_object() : object_info(), object_hash(), reflect(), refract() {
    reset();
}

auth_3d_object::~auth_3d_object() {
    reset();
}

void auth_3d_object::interpolate(float_t frame) {
    model_transform.interpolate(frame);
    pattern.interpolate(frame);
    morph.interpolate(frame);
    for (auth_3d_object_texture_pattern& i : texture_pattern)
        i.interpolate(frame);
    for (auth_3d_object_texture_transform& i : texture_transform)
        i.interpolate(frame);
}

void auth_3d_object::list_ctrl(const mat4* parent_mat) {
    //if (!model_transform.visible)
    //    return;

    model_transform.set_mat(parent_mat);

    mat4& mat = model_transform.mat;

    for (auth_3d_object*& i : children_object)
        i->list_ctrl(&mat);
    for (auth_3d_object_hrc*& i : children_object_hrc)
        i->list_ctrl(&mat);
}

void auth_3d_object::reset() {
    children_object.clear();
    children_object.shrink_to_fit();
    children_object_hrc.clear();
    children_object_hrc.shrink_to_fit();
    model_transform.reset();
    morph.reset();
    object_info = {};
    object_hash = (uint32_t)-1;
    name.clear();
    name.shrink_to_fit();
    parent_name.clear();
    parent_name.shrink_to_fit();
    parent_node.clear();
    parent_node.shrink_to_fit();
    pattern.reset();
    reflect = false;
    refract = false;
    for (auth_3d_object_texture_pattern& i : texture_pattern)
        i.reset();
    texture_pattern.clear();
    texture_pattern.shrink_to_fit();
    for (auth_3d_object_texture_transform& i : texture_transform)
        i.reset();
    texture_transform.clear();
    texture_transform.shrink_to_fit();
    uid_name.clear();
    uid_name.shrink_to_fit();
}

auth_3d_object_curve::auth_3d_object_curve() : curve(), frame_offset(), value() {
    reset();
}

auth_3d_object_curve::~auth_3d_object_curve() {
    reset();
}

void auth_3d_object_curve::interpolate(float_t frame) {
    auth_3d_curve* c = curve;
    if (!c)
        return;

    if (frame_offset != 0.0f) {
        frame += frame_offset;
        if (frame >= c->curve.max_frame)
            frame -= c->curve.max_frame;
    }
    value = c->curve.interpolate(frame);
}

void auth_3d_object_curve::reset() {
    curve = 0;
    name.clear();
    name.shrink_to_fit();
    frame_offset = 0.0f;
    value = 0.0f;
}

auth_3d_object_hrc::auth_3d_object_hrc() : object_info(),
object_hash(), reflect(), refract(), shadow() {
    reset();
}

auth_3d_object_hrc::~auth_3d_object_hrc() {
    reset();
}

int32_t auth_3d_object_hrc::get_node_index(const char* node) {
    for (auth_3d_object_node& i : this->node)
        if (!i.name.compare(node))
            return (int32_t)(&i - this->node.data());
    return -1;
}

void auth_3d_object_hrc::interpolate(float_t frame) {
    for (auth_3d_object_node& i : node)
        i.interpolate(frame);
}

void auth_3d_object_hrc::list_ctrl(const mat4* mat) {
    //if (node.size() < 1 || !node.front().model_transform.visible)
    //    return;

    auth_3d_object_hrc_nodes_mat_mult(this, mat);

    auth_3d_object_node* node = this->node.data();

    int32_t* children_object_parent_node = this->children_object_parent_node.data();
    auth_3d_object** children_object = this->children_object.data();
    size_t children_object_count = this->children_object.size();
    for (size_t i = 0; i < children_object_count; i++)
        children_object[i]->list_ctrl(
            &node[children_object_parent_node[i]].model_transform.mat);

    int32_t* children_object_hrc_parent_node = this->children_object_hrc_parent_node.data();
    auth_3d_object_hrc** children_object_hrc = this->children_object_hrc.data();
    size_t children_object_hrc_count = this->children_object_hrc.size();
    for (size_t i = 0; i < children_object_hrc_count; i++)
        children_object_hrc[i]->list_ctrl(
            &node[children_object_hrc_parent_node[i]].model_transform.mat);
}

void auth_3d_object_hrc::reset() {
    children_object.clear();
    children_object.shrink_to_fit();
    children_object_hrc.clear();
    children_object_hrc.shrink_to_fit();
    children_object_parent_node.clear();
    children_object_parent_node .shrink_to_fit();
    children_object_hrc_parent_node.clear();
    children_object_hrc_parent_node.shrink_to_fit();
    mats.clear();
    mats.shrink_to_fit();
    name.clear();
    name.shrink_to_fit();
    for (auth_3d_object_node& i : node)
        i.reset();
    node.clear();
    node.shrink_to_fit();
    object_info = {};
    object_hash = (uint32_t)-1;
    parent_name.clear();
    parent_name.shrink_to_fit();
    parent_node.clear();
    parent_node.shrink_to_fit();
    reflect = false;
    refract = false;
    shadow = false;
    uid_name.clear();
    uid_name.shrink_to_fit();
}

auth_3d_object_instance::auth_3d_object_instance() : object_info(), object_hash(), shadow()  {
    reset();
}

auth_3d_object_instance::~auth_3d_object_instance() {
    reset();
}

void auth_3d_object_instance::interpolate(float_t frame) {
    model_transform.interpolate(frame);
}

void auth_3d_object_instance::reset() {
    mats.clear();
    mats.shrink_to_fit();
    model_transform.reset();
    name.clear();
    name.shrink_to_fit();
    object_bone_indices.clear();
    object_bone_indices.shrink_to_fit();
    object_info = {};
    object_hash = (uint32_t)-1;
    shadow = false;
    uid_name.clear();
    uid_name.shrink_to_fit();
}

auth_3d_object_model_transform::auth_3d_object_model_transform() : mat(),
mat_inner(), mat_rot(), translation_value(), rotation_value(), scale_value(),
visible(), frame(), has_rotation(), has_translation(), has_scale(), has_visibility() {
    reset();
}

auth_3d_object_model_transform::~auth_3d_object_model_transform() {
    reset();
}

void auth_3d_object_model_transform::interpolate(float_t frame) {
    if (frame != this->frame || has_rotation) {
        translation_value = translation.interpolate(frame);
        rotation_value = rotation.interpolate(frame);

        if (has_scale)
            scale_value = scale.interpolate(frame);

        if (has_visibility)
            visible = visibility.interpolate(frame) >= 0.99900001f;

        mat4_rotate(&rotation_value, &mat_rot);
        this->frame = frame;
        has_rotation = false;
        has_translation = false;
    }

    if (has_translation && !has_rotation)
        return;

    mat4 mat;
    mat4_translate(&translation_value, &mat);

    mat4_mult(&mat_rot, &mat, &mat);
    if (has_scale)
        mat4_scale_rot(&mat, &scale_value, &mat);
    mat_inner = mat;
    has_translation = true;
}

void auth_3d_object_model_transform::reset() {
    mat = mat4_identity;
    mat_inner = mat4_identity;
    mat_rot = mat4_identity;
    translation.reset();
    rotation.reset();
    scale.reset();
    visibility.reset();
    translation_value = 0.0f;
    rotation_value = 0.0f;
    scale_value = 1.0f;
    visible = true;
    frame = -1.0f;
    has_rotation = false;
    has_translation = false;
    has_scale = false;
    has_visibility = false;
}

auth_3d_object_node::auth_3d_object_node() : flags(),
bone_id(), joint_orient(), joint_orient_mat(), mat(), parent() {
    reset();
}

auth_3d_object_node::~auth_3d_object_node() {
    reset();
}

void auth_3d_object_node::interpolate(float_t frame) {
    model_transform.interpolate(frame);
}

void auth_3d_object_node::reset() {
    flags = (auth_3d_object_node_flags)0;
    bone_id = -1;
    joint_orient = 0.0f;
    joint_orient_mat = mat4_identity;
    mat = 0;
    model_transform.reset();
    name.clear();
    name.shrink_to_fit();
    parent = -1;
}

auth_3d_object_texture_pattern::auth_3d_object_texture_pattern() : texture_id() {
    reset();
}

auth_3d_object_texture_pattern::~auth_3d_object_texture_pattern() {
    reset();
}

void auth_3d_object_texture_pattern::interpolate(float_t frame) {
    pattern.interpolate(frame);
}

void auth_3d_object_texture_pattern::reset() {
    name.clear();
    name.shrink_to_fit();
    pattern.reset();
    texture_id = -1;
}

auth_3d_object_texture_transform::auth_3d_object_texture_transform() : flags(), mat(), texture_id() {
    reset();
}

auth_3d_object_texture_transform::~auth_3d_object_texture_transform() {
    reset();
}

void auth_3d_object_texture_transform::interpolate(float_t frame) {
    int32_t flags = this->flags;

    vec3 scale = 1.0f;
    vec3 repeat = 1.0f;
    vec3 rotate = 0.0f;
    vec3 translate_frame = 0.0f;

    if (flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_COVERAGE_U && this->coverage_u.max_frame > 0.0f) {
        float_t coverage_u = this->coverage_u.interpolate(frame);
        if (fabsf(coverage_u) > 0.000001f)
            scale.x = 1.0f / coverage_u;
    }

    if (flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_COVERAGE_V && this->coverage_v.max_frame > 0.0f) {
        float_t coverage_v = this->coverage_v.interpolate(frame);
        if (fabsf(coverage_v) > 0.000001f)
            scale.y = 1.0f / coverage_v;
    }

    if (flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_REPEAT_U && this->repeat_u.max_frame > 0.0f) {
        float_t value = this->repeat_u.interpolate(frame);
        if (fabsf(value) > 0.000001f)
            repeat.x = value;
    }

    if (flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_REPEAT_V && this->repeat_v.max_frame > 0.0f) {
        float_t value = this->repeat_v.interpolate(frame);
        if (fabsf(value) > 0.000001f)
            repeat.y = value;
    }

    if (flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_ROTATE_FRAME)
        rotate.z = this->rotate_frame.interpolate(frame);

    if (flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_ROTATE)
        rotate.z += this->rotate.interpolate(frame);

    float_t translate_frame_u;
    if (flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_TRANSLATE_FRAME_U)
        translate_frame_u = -this->translate_frame_u.interpolate(frame);
    else
        translate_frame_u = 0.0f;

    float_t offset_u;
    if (flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_OFFSET_U)
        offset_u = this->offset_u.interpolate(frame);
    else
        offset_u = 0.0f;

    translate_frame.x = (translate_frame_u + offset_u) * repeat.x;

    float_t translate_frame_v;
    if (flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_TRANSLATE_FRAME_V)
        translate_frame_v = -this->translate_frame_v.interpolate(frame);
    else
        translate_frame_v = 0.0f;

    float_t offset_v;
    if (flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_OFFSET_V)
        offset_v = this->offset_v.interpolate(frame);
    else
        offset_v = 0.0f;

    translate_frame.y = (translate_frame_v + offset_v) * repeat.y;

    mat4 mat;
    mat4_translate(&translate_frame, &mat);
    mat4_scale_rot(&mat, &scale, &mat);

    mat4_translate_mult(&mat, 1.0f, 1.0f, 0.0f, &mat);
    mat4_scale_rot(&mat, &repeat, &mat);
    mat4_translate_mult(&mat, -1.0f, -1.0f, 0.0f, &mat);

    mat4_translate_mult(&mat, 0.5f, 0.5f, 0.0f, &mat);
    mat4_rotate_z_mult(&mat, rotate.z, &mat);
    mat4_translate_mult(&mat, -0.5f, -0.5f, 0.0f, &mat);
    this->mat = mat;
}

void auth_3d_object_texture_transform::reset() {
    flags = (auth_3d_object_texture_transform_flags)0;
    coverage_u.reset();
    coverage_v.reset();
    mat = mat4_identity;
    name.clear();
    name.shrink_to_fit();
    offset_u.reset();
    offset_v.reset();
    repeat_u.reset();
    repeat_v.reset();
    rotate.reset();
    rotate_frame.reset();
    texture_id = -1;
    translate_frame_u.reset();
    translate_frame_v.reset();
}

auth_3d_play_control::auth_3d_play_control() : flags(),
begin(), div(), fps(), offset(), size() {
    reset();
}

auth_3d_play_control::~auth_3d_play_control() {
    reset();
}

void auth_3d_play_control::reset() {
    flags = (auth_3d_play_control_flags)0;
    begin = 0.0f;
    div = 0;
    fps = 0.0f;
    offset = 0.0f;
    size = 0.0f;
}

auth_3d_point::auth_3d_point() {
    reset();
}

auth_3d_point::~auth_3d_point() {
    reset();
}

void auth_3d_point::interpolate(float_t frame) {
    model_transform.interpolate(frame);
}

void auth_3d_point::reset() {
    model_transform.reset();
    name.clear();
    name.shrink_to_fit();
}

auth_3d_post_process::auth_3d_post_process() : flags(), flags_init(),
lens_flare_value(), lens_ghost_value(), lens_shaft_value(), intensity_init(),
lens_flare_init(), lens_ghost_init(), lens_shaft_init(), radius_init(), scene_fade_init() {
    reset();
}

auth_3d_post_process::~auth_3d_post_process() {
    reset();
}

void auth_3d_post_process::interpolate(float_t frame) {
    if (flags & AUTH_3D_POST_PROCESS_INTENSITY)
        intensity.interpolate(frame);
    if (flags & AUTH_3D_POST_PROCESS_LENS_FLARE)
        lens_flare_value = lens_flare.interpolate(frame);
    if (flags & AUTH_3D_POST_PROCESS_LENS_SHAFT)
        lens_shaft_value = lens_shaft.interpolate(frame);
    if (flags & AUTH_3D_POST_PROCESS_LENS_GHOST)
        lens_ghost_value = lens_ghost.interpolate(frame);
    if (flags & AUTH_3D_POST_PROCESS_RADIUS)
        radius.interpolate(frame);
    if (flags & AUTH_3D_POST_PROCESS_SCENE_FADE)
        scene_fade.interpolate(frame);
}

void auth_3d_post_process::reset() {
    flags = (auth_3d_post_process_flags)0;
    flags_init = (auth_3d_post_process_flags)0;
    intensity.reset();
    lens_flare.reset();
    lens_ghost.reset();
    lens_shaft.reset();
    radius.reset();
    scene_fade.reset();
    lens_flare_value = 1.0f;
    lens_ghost_value = 1.0f;
    lens_shaft_value = 1.0f;
    intensity_init = 0.0f;
    lens_flare_init = 1.0f;
    lens_ghost_init = 1.0f;
    lens_shaft_init = 1.0f;
    radius_init = 0.0f;
    scene_fade_init = 0.0f;
}

auth_3d_farc::auth_3d_farc() : load_count(), name(), state(), farc(), data(), size() {

}

auth_3d_farc::~auth_3d_farc() {
    delete farc;
}

auth_3d_uid_file::auth_3d_uid_file() : load_count(), uid(), state(), name(), farc() {

}

auth_3d_uid_file::~auth_3d_uid_file() {

}

auth_3d_uid_file_modern::auth_3d_uid_file_modern() : load_count(),
state(), name(), farc(),data(), obj_db(), tex_db() {
    hash = hash_murmurhash_empty;
}

auth_3d_uid_file_modern::~auth_3d_uid_file_modern() {

}

bool auth_3d_id::check_not_empty() {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth->id == id)
            return auth->id != -1;
    }
    return false;
}

bool auth_3d_id::check_loading() {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth->id == id)
            return auth->state == 1;
    }
    return false;
}

bool auth_3d_id::check_loaded() {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth->id == id)
            return auth->state == 2;
    }
    return true;
}

auth_3d* auth_3d_id::get_auth_3d() {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth->id == id)
            return auth;
    }
    return 0;
}

int32_t auth_3d_id::get_chara_id() {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth->id == id)
            return auth->chara_id;
    }
    return -1;
}

mat4* auth_3d_id::get_auth_3d_object_mat(size_t index, bool hrc, mat4* mat) {
    if (id < 0 || (id & 0x7FFF) >= AUTH_3D_DATA_COUNT)
        return 0;

    auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
    if (auth->id != id || !auth->enable)
        return 0;

    if (hrc)
        return auth_3d_get_auth_3d_object_hrc_bone_mats(auth, index);

    if (index >= auth->object.size())
        return 0;

    *mat = auth->object[index].model_transform.mat;
    return mat;
}

bool auth_3d_id::get_enable() {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth->id == id)
            return auth->enable;
    }
    return false;
}

bool auth_3d_id::get_ended() {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth->id == id)
            return auth->ended;
    }
    return true;
}

float_t auth_3d_id::get_frame() {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth->id == id)
            return auth->frame;
    }
    return 0.0f;
}

float_t auth_3d_id::get_frame_offset() {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth->id == id)
            return auth->frame_offset;
    }
    return 0.0f;
}

float_t auth_3d_id::get_last_frame() {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth->id == id)
            return auth->last_frame;
    }
    return 0.0f;
}

bool auth_3d_id::get_left_right_reverse() {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth->id == id)
            return auth->left_right_reverse;
    }
    return false;
}

float_t auth_3d_id::get_play_control_begin() {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth->id == id)
            return auth->play_control.begin;
    }
    return 0.0f;
}

float_t auth_3d_id::get_play_control_size() {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth->id == id)
            return auth->play_control.size;
    }
    return 0.0f;
}

bool auth_3d_id::get_paused() {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth->id == id)
            return auth->paused;
    }
    return false;
}

bool auth_3d_id::get_repeat() {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth->id == id)
            return auth->repeat;
    }
    return false;
}

int32_t auth_3d_id::get_uid() {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth->id == id)
            return auth->uid;
    }
    return -1;
}

void auth_3d_id::read_file(auth_3d_database* auth_3d_db) {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth && auth->id == id)
            auth_3d_read_file(auth, auth_3d_db);
    }
}

void auth_3d_id::read_file_modern() {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth && auth->id == id)
            auth_3d_read_file_modern(auth);
    }
}

void auth_3d_id::set_camera_root_update(bool value) {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth->id == id)
            auth->camera_root_update = value;
    }
}

void auth_3d_id::set_chara_id(int32_t value) {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth->id == id)
            auth->chara_id = value;
    }
}

void auth_3d_id::set_chara_item(bool value) {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth->id == id)
            auth->chara_item = value;
    }
}

void auth_3d_id::set_draw_task_flags_alpha(draw_task_flags draw_task_flags, float_t alpha) {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth->id == id) {
            auth->draw_task_flags = draw_task_flags;
            auth->alpha = alpha;
        }
    }
}

void auth_3d_id::set_enable(bool value) {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth->id == id)
            auth->enable = value;
    }
}

void auth_3d_id::set_frame_rate(FrameRateControl* value) {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth->id == id)
            if (value)
                auth->frame_rate = value;
            else
                auth->frame_rate = &sys_frame_rate;
    }
}

void auth_3d_id::set_last_frame(float_t value) {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth->id == id)
            auth->last_frame = value;
    }
}

void auth_3d_id::set_left_right_reverse(bool value) {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth->id == id)
            auth->left_right_reverse = value;
    }
}

void auth_3d_id::set_mat(const mat4& value) {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth->id == id)
            auth->mat = value;
    }
}

void auth_3d_id::set_max_frame(float_t value) {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth->id == id)
            auth->max_frame = value;
    }
}

void auth_3d_id::set_paused(bool value) {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth->id == id)
            auth->paused = value;
    }
}

void auth_3d_id::set_repeat(bool value) {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth->id == id)
            auth->repeat = value;
    }
}

void auth_3d_id::set_req_frame(float_t value) {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth->id == id) {
            auth->req_frame = value;
            auth->frame_changed = true;
        }
    }
}

void auth_3d_id::set_shadow(bool value) {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth->id == id)
            auth->shadow = value;
    }
}

void auth_3d_id::set_src_dst_chara(int32_t src_chara, int32_t dst_chara) {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth->id == id) {
            auth->src_chara = src_chara;
            auth->dst_chara = dst_chara;
        }
    }
}

void auth_3d_id::set_visibility(bool value) {
    if (id >= 0 && ((id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
        if (auth->id == id)
            auth->visible = value;
    }
}

void auth_3d_id::unload_id(render_context* rctx) {
    if (id < 0 || (id & 0x7FFF) >= AUTH_3D_DATA_COUNT)
        return;

    auth_3d* auth = &auth_3d_data->data[id & 0x7FFF];
    if (!auth || auth->id != id)
        return;

    for (auto i = auth_3d_data->loaded_ids.begin(); i != auth_3d_data->loaded_ids.end();) {
        if (*i != id) {
            i++;
            continue;
        }

        i = auth_3d_data->loaded_ids.erase(i);
        auth->unload(rctx);
        break;
    }
}

auth_3d_data_struct::auth_3d_data_struct() {

}

auth_3d_data_struct::~auth_3d_data_struct() {

}

Auth3dTestTask::Auth3dTestTask::Window::Window() {
    stage_link_change = true;
    obj_link = true;
}

Auth3dTestTask::Auth3dTestTask() {
    field_1C0 = 10;
    field_1C4 = 10;
    field_1C8 = 10;
    field_1CC = 10;
    field_1D0 = false;
    field_1D4 = 0;
    auth_3d_id = {};
    auth_3d_uid = -1;
    repeat = true;
    left_right_reverse = false;
    field_1E3 = false;
    field_1E4 = 0;
    field_1E8 = false;
    field_1E9 = false;
    black_mask_listener = false;
    field_1EB = true;
    field_1EC = 1;
    effcmn_obj_set = -1;
    field_210 = 1;
    field_328 = false;
    field_329 = true;
    field_32A = true;
    stage_index = -1;
    load_stage_index = 0;
    trans_value = 0.0f;
    rot_y_value = 0.0f;
    field_388 = false;
    field_38C = 0;
    field_390 = false;
    field_394 = 0;
}

Auth3dTestTask:: ~Auth3dTestTask() {

}

bool Auth3dTestTask::Init() {
    data_struct* aft_data = &data_list[DATA_AFT];
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    field_1D0 = false;
    field_1D4 = 0;
    auth_3d_id = {};
    auth_3d_uid = -1;
    repeat = true;
    left_right_reverse = false;
    field_1E3 = false;
    field_1E4 = 0;
    field_1E8 = false;
    field_1E9 = false;
    black_mask_listener = false;
    field_1EC = 1;
    effcmn_obj_set = aft_obj_db->get_object_set_id("EFFCMN");
    field_210 = 1;
    field_328 = false;
    field_329 = true;
    field_32A = true;
    stage_index = -1;
    load_stage_index = 0;
    trans_value = 0.0f;
    rot_y_value = 0.0f;
    field_390 = true;
    field_394 = 0;
    category.clear();
    category.shrink_to_fit();
    load_category.clear();
    load_category.shrink_to_fit();
    obj_sets.clear();
    obj_sets.shrink_to_fit();

    rctx_ptr->draw_pass.shadow_ptr->self_shadow = true;
    clear_color = { 0x99, 0x99, 0x99, 0x00 };
    task_stage_load_task("A3D_STAGE");
    object_storage_load_set(aft_data, aft_obj_db, effcmn_obj_set);
    return true;
}

bool Auth3dTestTask::Ctrl() {
    Auth3dTestTask__SetStage(this);
    Auth3dTestTask__SetAuth3dId(this);

    if (field_1EC == 1 && !object_storage_load_obj_set_check_not_read(effcmn_obj_set))
        field_1EC = 4;

    if (field_1D4 == 1 && auth_3d_id.check_loaded())
        field_1D4 = 2;

    if (field_1D4 == 2) {
        auth_3d_id.set_enable(true);
        auth_3d_id.set_paused(false);
        auth_3d_id.set_repeat(repeat);
        auth_3d_id.set_left_right_reverse(left_right_reverse);
        field_1D4 = 4;
    }

    if (auth_3d_id.check_not_empty()) {
        mat4 mat;
        mat4_translate(&trans_value, &mat);
        mat4_rotate_y_mult(&mat, rot_y_value * DEG_TO_RAD_FLOAT, &mat);
        auth_3d_id.set_mat(mat);
    }
    return false;
}

bool Auth3dTestTask::Dest() {
    auth_3d_id.unload_id(rctx_ptr);
    object_storage_unload_set(effcmn_obj_set);
    task_stage_unload_task();
    clear_color = color_black;
    rctx_ptr->draw_pass.shadow_ptr->self_shadow = true;
    if (category.size())
        auth_3d_data_unload_category(category.c_str());
    category.clear();
    category.shrink_to_fit();
    load_category.clear();
    load_category.shrink_to_fit();
    for (uint32_t& i : obj_sets)
        object_storage_unload_set(i);
    obj_sets.clear();
    obj_sets.shrink_to_fit();
    return true;
}

void auth_3d_data_init() {
    auth_3d_data = new auth_3d_data_struct;
}

bool auth_3d_data_check_category_loaded(const char* category_name) {
    return auth_3d_data_struct_check_category_loaded(auth_3d_data, category_name);
}

bool auth_3d_data_check_category_loaded(uint32_t category_hash) {
    return auth_3d_data_struct_check_category_loaded(auth_3d_data, category_hash);
}

int32_t auth_3d_data_get_auth_3d_id(const char* object_name) {
    for (int32_t& i : auth_3d_data->loaded_ids) {
        if (i < 0 || (i & 0x7FFF) >= AUTH_3D_DATA_COUNT)
            continue;

        auth_3d* auth = &auth_3d_data->data[i & 0x7FFF];
        if (auth->id != i || !auth->enable)
            continue;

        for (auth_3d_object_hrc& j : auth->object_hrc)
            if (!j.name.compare(object_name))
                return i;

        for (auth_3d_object& j : auth->object)
            if (!j.name.compare(object_name))
                return i;
    }
    return -1;
}

int32_t auth_3d_data_get_auth_3d_id(object_info obj_info,
    int32_t* object_index, bool* hrc, int32_t instance) {
    for (int32_t& i : auth_3d_data->loaded_ids) {
        if (i < 0 || (i & 0x7FFF) >= AUTH_3D_DATA_COUNT)
            continue;

        auth_3d* auth = &auth_3d_data->data[i & 0x7FFF];
        if (auth->id != i || !auth->enable)
            continue;

        int32_t obj_hrc_index = auth_3d_get_auth_3d_object_hrc_index_by_object_info(
            auth, obj_info, instance);
        if (obj_hrc_index >= 0) {
            if (object_index)
                *object_index = obj_hrc_index;
            if (hrc)
                *hrc = true;
            return i;
        }

        int32_t obj_index = auth_3d_get_auth_3d_object_index_by_object_info(
            auth, obj_info, instance);
        if (obj_index >= 0) {
            if (object_index)
                *object_index = obj_index;
            if (hrc)
                *hrc = false;
            return i;
        }
    }
    return -1;
}

int32_t auth_3d_data_get_auth_3d_id(uint32_t file_name_hash, uint32_t object_hash,
    int32_t* object_index, bool* hrc, int32_t instance) {
    for (int32_t& i : auth_3d_data->loaded_ids) {
        if (i < 0 || (i & 0x7FFF) >= AUTH_3D_DATA_COUNT)
            continue;

        auth_3d* auth = &auth_3d_data->data[i & 0x7FFF];
        if (auth->id != i || !auth->enable)
            continue;

        bool ret = false;
        if (file_name_hash != hash_murmurhash_empty) {
            if (auth->hash != file_name_hash)
                continue;
            ret = true;
        }

        int32_t obj_index = auth_3d_get_auth_3d_object_index_by_hash(
            auth, object_hash, instance);
        if (obj_index >= 0) {
            if (object_index)
                *object_index = obj_index;
            if (hrc)
                *hrc = false;
            return i;
        }

        int32_t obj_hrc_index = auth_3d_get_auth_3d_object_hrc_index_by_hash(
            auth, object_hash, instance);
        if (obj_hrc_index >= 0) {
            if (object_index)
                *object_index = obj_hrc_index;
            if (hrc)
                *hrc = true;
            return i;
        }

        if (ret)
            return -1;
    }
    return -1;
}

void auth_3d_data_get_obj_sets_from_category(std::string& name, std::vector<uint32_t>& obj_sets,
    auth_3d_database* auth_3d_db, object_database* obj_db) {
    obj_sets.clear();

    std::vector<int32_t> uid;
    auth_3d_db->get_category_uids(name.c_str(), uid);
    for (int32_t& i : uid) {
        const char* uid_name = auth_3d_data_get_uid_name(i, auth_3d_db);
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
        pv_db_pv* pv = task_pv_db_get_pv(pv_id);
        if (pv && pv->difficulty[PV_DIFFICULTY_HARD].size()) {
            pv_db_pv_difficulty& diff = pv->difficulty[PV_DIFFICULTY_HARD][0];

            for (pv_db_pv_item& i : diff.pv_item) {
                if (i.index <= 0)
                    continue;

                object_info info = obj_db->get_object_info(i.name.c_str());
                if (info.not_null())
                    obj_sets.push_back(info.set_id);
            }
        }
    }

    if (!name.find("EFFCHRPV")) {
        uint32_t obj_set = obj_db->get_object_set_id(name.c_str());
        if (obj_set != -1)
            obj_sets.push_back(obj_set);
    }

    uint32_t prev_obj_set = -1;
    radix_sort_uint32_t(obj_sets.data(), obj_sets.size());
    for (std::vector<uint32_t>::iterator i = obj_sets.begin(); i != obj_sets.end();)
        if (*i != prev_obj_set)
            prev_obj_set = *i++;
        else
            i = obj_sets.erase(i);

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

    stgpv += "HRC";

    uint32_t obj_set = obj_db->get_object_set_id(stgpv.c_str());
    if (obj_set != -1)
        obj_sets.push_back(obj_set);
}

const char* auth_3d_data_get_uid_name(int32_t uid, auth_3d_database* auth_3d_db) {
    if (uid < 0 || uid > auth_3d_db->uid.size())
        return 0;

    auth_3d_database_uid* db_uid = &auth_3d_db->uid[uid];
    if (db_uid->enabled)
        return db_uid->name.c_str();
    return 0;
}

void auth_3d_data_load_auth_3d_db(auth_3d_database* auth_3d_db) {
    auth_3d_data->farcs.clear();
    auth_3d_data->farcs.resize(auth_3d_db->category.size());

    size_t cat_index = 0;
    for (auth_3d_farc& i : auth_3d_data->farcs)
        i.name.assign(auth_3d_db->category[cat_index++].name);

    auth_3d_data->uid_files.clear();
    auth_3d_data->uid_files.resize(auth_3d_db->uid.size());

    size_t uid_index = 0;
    for (auth_3d_uid_file& i : auth_3d_data->uid_files)
        i.uid = (int32_t)(uid_index++);
}

void auth_3d_data_load_category(const char* category_name, const char* mdata_dir) {
    auth_3d_data_struct_load_category(auth_3d_data, category_name, mdata_dir);
}

void auth_3d_data_load_category(void* data, const char* category_name, uint32_t category_hash) {
    auth_3d_data_struct_load_category(auth_3d_data, data, category_name, category_hash);
}

auth_3d_id auth_3d_data_load_hash(uint32_t hash, void* data, object_database* obj_db, texture_database* tex_db) {
    auto elem = auth_3d_data->uid_files_modern.find(hash);
    if (elem == auth_3d_data->uid_files_modern.end())
        elem = auth_3d_data->uid_files_modern.insert({ hash, {} }).first;

    auth_3d_uid_file_modern* uid_file = &elem->second;
    uid_file->load_count = 0;
    uid_file->hash = hash;
    uid_file->file_name.clear();
    uid_file->file_name.shrink_to_fit();
    uid_file->state = 0;
    uid_file->name.clear();
    uid_file->name.shrink_to_fit();
    uid_file->farc = 0;
    uid_file->data = data;
    uid_file->obj_db = obj_db;
    uid_file->tex_db = tex_db;

    int32_t index = 0;
    while (auth_3d_data->data[index].uid != -1 || auth_3d_data->data[index].hash
        != hash_murmurhash_empty && auth_3d_data->data[index].hash != -1)
        if (++index >= AUTH_3D_DATA_COUNT)
            return auth_3d_id();

    auth_3d_data->data[index].reset();

    if (++auth_3d_load_counter < 0)
        auth_3d_load_counter = 1;

    int32_t id = ((auth_3d_load_counter & 0x7FFF) << 16) | index & 0x7FFF;
    auth_3d_data->data[index].hash = hash;
    auth_3d_data->data[index].id = id;
    auth_3d_data->loaded_ids.push_back(id);
    return auth_3d_id(id);
}

auth_3d_id auth_3d_data_load_uid(int32_t uid, auth_3d_database* auth_3d_db) {
    if (uid >= auth_3d_db->uid.size() || !auth_3d_db->uid[uid].enabled)
        return auth_3d_id();

    int32_t index = 0;
    while (auth_3d_data->data[index].uid != -1 || auth_3d_data->data[index].hash
        != hash_murmurhash_empty && auth_3d_data->data[index].hash != -1)
        if (++index >= AUTH_3D_DATA_COUNT)
            return auth_3d_id();

    auth_3d_data->data[index].reset();

    if (++auth_3d_load_counter < 0)
        auth_3d_load_counter = 1;

    int32_t id = ((auth_3d_load_counter & 0x7FFF) << 16) | index & 0x7FFF;
    auth_3d_data->data[index].uid = uid;
    auth_3d_data->data[index].id = id;
    auth_3d_data->loaded_ids.push_back(id);
    return auth_3d_id(id);
}

void auth_3d_data_unload_category(const char* category_name) {
    auth_3d_data_struct_unload_category(auth_3d_data, category_name);
}

void auth_3d_data_unload_category(uint32_t category_hash) {
    auth_3d_data_struct_unload_category(auth_3d_data, category_hash);
}

void auth_3d_data_free() {
    delete auth_3d_data;
}

void auth_3d_test_task_init() {
    auth_3d_test_task = new Auth3dTestTask;
}

void auth_3d_test_task_free() {
    if (auth_3d_test_task) {
        delete auth_3d_test_task;
        auth_3d_test_task = 0;
    }
}

void task_auth_3d_init() {
    task_auth_3d = new auth_3d_detail::TaskAuth3d;
}

bool task_auth_3d_append_task() {
    return app::TaskWork::AppendTask(task_auth_3d, "AUTH_3D");
}

bool task_auth_3d_check_task_ready() {
    return app::TaskWork::CheckTaskReady(task_auth_3d);
}

bool task_auth_3d_free_task() {
    return task_auth_3d->SetDest();
}

void task_auth_3d_free() {
    if (task_auth_3d) {
        delete task_auth_3d;
        task_auth_3d = 0;
    }
}

namespace auth_3d_detail  {
    TaskAuth3d::TaskAuth3d() {

    }

    TaskAuth3d:: ~TaskAuth3d() {

    }

    bool TaskAuth3d::Init() {
        return auth_3d_data_struct_init(auth_3d_data, rctx_ptr);
    }

    bool TaskAuth3d::Ctrl() {
        return auth_3d_data_struct_ctrl(auth_3d_data, rctx_ptr);
    }

    void TaskAuth3d::Disp() {
        auth_3d_data_struct_disp(auth_3d_data, rctx_ptr);
    }

    FrameRateTimeStop::FrameRateTimeStop() {

    }

    FrameRateTimeStop::~FrameRateTimeStop() {

    }

    float_t FrameRateTimeStop::GetDeltaFrame() {
        return 0.0f;
    }
}

static void a3da_msgpack_read(const char* path, const char* file, a3da* auth_file) {
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

    if (msg.type != MSGPACK_MAP)
        return;

    msgpack* m_objhrcs = msg.read_array("m_objhrc");
    if (m_objhrcs) {
        msgpack_array* ptr = m_objhrcs->data.arr;
        for (msgpack& i : *ptr) {
            msgpack& m_objhrc = i;

            std::string name = m_objhrc.read_string("name");
            uint32_t name_hash = hash_string_murmurhash(name);

            for (a3da_m_object_hrc& j : auth_file->m_object_hrc) {
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

    msgpack* objhrcs = msg.read_array("objhrc");
    if (objhrcs) {
        msgpack_array* ptr = objhrcs->data.arr;
        for (msgpack& i : *ptr) {
            msgpack& objhrc = i;

            std::string name = objhrc.read_string("name");
            uint32_t name_hash = hash_string_murmurhash(name);

            for (a3da_object_hrc& j : auth_file->object_hrc) {
                if (name_hash != hash_string_murmurhash(j.name))
                    continue;

                j.shadow = objhrc.read_bool("shadow");
                break;
            }
        }
    }
}

static bool auth_3d_key_detect_fast_change(auth_3d_key* data, float_t frame, float_t threshold) {
    if (data->type < AUTH_3D_KEY_LINEAR || data->type > AUTH_3D_KEY_HOLD)
        return false;

    frame = clamp_def(frame, 0.0f, data->max_frame);
    size_t length = data->length;
    if (!length)
        return false;

    kft3* keys = data->keys;
    kft3* key = auth_3d_key_find_keyframe(data, frame);
    if (key == keys || key == &keys[1] || key == &keys[length])
        return false;

    kft3* next_key = &key[1];
    if (next_key == &keys[length])
        return false;

    kft3* prev_key = &key[-1];
    float_t v18 = key->frame - prev_key->frame;
    if (v18 >= 2.0f || v18 <= 0.0f)
        return 0;

    if (fabsf(key->value - prev_key->value) <= threshold)
        return 0;

    kft3* v21 = &prev_key[-1];
    if (prev_key->frame <= v21->frame) {
        while (v21 != keys) {
            if (prev_key->frame > v21->frame)
                break;
            v21++;
        }
        if (v21 == keys)
            return 0;
    }

    float_t v23 = key->value - (prev_key->value / v18);
    float_t v25;
    float_t v30;
    if (data->type != AUTH_3D_KEY_HERMITE) {
        v25 = (prev_key->value - v21->value) / (prev_key->frame - v21->frame);
        v30 = (next_key->value - key->value) / (next_key->frame - key->frame);
    }
    else {
        if (prev_key->frame - v21->frame >= 2.0f) {
            float_t v26;
            if (v21->frame < prev_key->frame)
                v26 = interpolate_chs_value(key->value, next_key->value,
                    key->tangent2, next_key->tangent1, prev_key->frame, v21->frame, prev_key->frame - 1.0f);
            else
                v26 = v21->value;
            v25 = prev_key->value - v26;
        }
        else
            v25 = (prev_key->value - v21->value) / (prev_key->frame - v21->frame);

        if (next_key->frame - key->frame >= 2.0f) {
            float_t v29;
            if (key->frame < next_key->frame)
                v29 = interpolate_chs_value(key->value, next_key->value,
                    key->tangent2, next_key->tangent1, key->frame, next_key->frame, 1.0f);
            else
                v29 = key->value;
            v30 = v29 - key->value;
        }
        else
            v30 = (next_key->value - key->value) / (next_key->frame - key->frame);
    }

    float_t v32 = fabsf(v25);
    float_t v33 = v25 * v23;
    float_t v34 = v30 * v23;
    float_t v35 = fabsf(v23);
    float_t v36 = fabsf(v30);
    return (v33 <= 0.0f || (v32 < v35 && v32 <= threshold))
        && (v34 <= 0.0f || (v36 < v35 && v36 <= threshold));
}

static float_t auth_3d_interpolate_value(auth_3d_key_type type,
    float_t frame, kft3* curr_key, kft3* next_key) {
    switch (type) {
    case AUTH_3D_KEY_LINEAR:
        if (curr_key->frame < next_key->frame)
            return interpolate_linear_value(curr_key->value, next_key->value,
                curr_key->frame, next_key->frame, frame);
        else
            return curr_key->value;
    case AUTH_3D_KEY_HERMITE:
        if (curr_key->frame < next_key->frame)
            return interpolate_chs_value(curr_key->value, next_key->value,
                curr_key->tangent2, next_key->tangent1,
                curr_key->frame, next_key->frame, frame);
        else
            return curr_key->value;
    case AUTH_3D_KEY_HOLD:
        if (frame >= next_key->frame)
            return next_key->value;
        else
            return curr_key->value;
    default:
        return 0.0f;
    }
}

static kft3* auth_3d_key_find_keyframe(auth_3d_key* data, float_t frame) {
    kft3* key = data->keys;
    size_t length = data->length;
    size_t temp;
    while (length > 0)
        if (frame < key[temp = length / 2].frame)
            length = temp;
        else {
            key += temp + 1;
            length -= temp + 1;
        }
    return key;
}

static float_t auth_3d_key_interpolate_inner(auth_3d_key* data, float_t frame) {
    kft3* first_key = data->keys;
    kft3* key = auth_3d_key_find_keyframe(data, frame);

    if (key == first_key)
        return first_key->value;
    else if (key == &first_key[data->length])
        return key[-1].value;
    else
        return auth_3d_interpolate_value(data->type, frame, key - 1, key);
}

static void auth_3d_key_load(auth_3d* auth, auth_3d_key* k, auth_3d_key_file* kf) {
    k->max_frame = auth->max_frame;
    k->frame_delta = k->max_frame;
    k->value_delta = 0.0f;

    size_t length = kf->keys.size();
    switch (kf->type) {
    case A3DA_KEY_NONE:
        k->type = AUTH_3D_KEY_NONE;
        k->value = 0.0f;
        return;
    case A3DA_KEY_STATIC:
        k->type = kf->value != 0.0f ? AUTH_3D_KEY_STATIC : AUTH_3D_KEY_NONE;
        k->value = kf->value;
        return;
    case A3DA_KEY_LINEAR:
        k->type = AUTH_3D_KEY_LINEAR;
        break;
    case A3DA_KEY_HERMITE:
    default:
        k->type = AUTH_3D_KEY_HERMITE;
        break;
    case A3DA_KEY_HOLD:
        k->type = AUTH_3D_KEY_HOLD;
        break;
    }

    if (length > 1) {
        k->keys_vec.assign(kf->keys.begin(), kf->keys.end());
        k->length = length;
        k->keys = k->keys_vec.data();

        kft3* first_key = &k->keys[0];
        kft3* last_key = &k->keys[length - 1];
        if (first_key->frame < last_key->frame
            && last_key->frame > 0.0f && k->max_frame > first_key->frame) {
            k->ep_type_pre = (auth_3d_ep_type)kf->ep_type_pre;
            k->ep_type_post = (auth_3d_ep_type)kf->ep_type_post;
            k->frame_delta = last_key->frame - first_key->frame;
            k->value_delta = last_key->value - first_key->value;
        }
    }
    else if (length == 1) {
        float_t value = kf->keys.front().value;
        k->type = value != 0.0f ? AUTH_3D_KEY_STATIC : AUTH_3D_KEY_NONE;
        k->value = value;
    }
    else {
        k->type = AUTH_3D_KEY_NONE;
        k->value = 0.0f;
    }
}

static void auth_3d_rgba_load(auth_3d* auth, auth_3d_rgba* rgba, auth_3d_rgba_file* rgbaf) {
    if (rgbaf->flags & A3DA_RGBA_R) {
        auth_3d_key_load(auth, &rgba->r, &rgbaf->r);
        enum_or(rgba->flags, AUTH_3D_RGBA_R);
    }

    if (rgbaf->flags & A3DA_RGBA_G) {
        auth_3d_key_load(auth, &rgba->g, &rgbaf->g);
        enum_or(rgba->flags, AUTH_3D_RGBA_G);
    }

    if (rgbaf->flags & A3DA_RGBA_B) {
        auth_3d_key_load(auth, &rgba->b, &rgbaf->b);
        enum_or(rgba->flags, AUTH_3D_RGBA_B);
    }

    if (rgbaf->flags & A3DA_RGBA_A) {
        auth_3d_key_load(auth, &rgba->a, &rgbaf->a);
        enum_or(rgba->flags, AUTH_3D_RGBA_A);
    }
}

static void auth_3d_vec3_load(auth_3d* auth, auth_3d_vec3* vec, auth_3d_vec3_file* vecf) {
    auth_3d_key_load(auth, &vec->x, &vecf->x);
    auth_3d_key_load(auth, &vec->y, &vecf->y);
    auth_3d_key_load(auth, &vec->z, &vecf->z);
}

static void auth_3d_model_transform_load(auth_3d* auth, auth_3d_model_transform* mt, auth_3d_model_transform_file* mtf) {
    auth_3d_vec3_load(auth, &mt->rotation, &mtf->rotation);
    auth_3d_vec3_load(auth, &mt->scale, &mtf->scale);
    auth_3d_vec3_load(auth, &mt->translation, &mtf->translation);
    auth_3d_key_load(auth, &mt->visibility, &mtf->visibility);
}

static int32_t auth_3d_get_auth_3d_object_index_by_object_info(auth_3d* auth,
    object_info obj_info, int32_t instance) {
    int32_t obj_instance = 0;
    for (auth_3d_object& i : auth->object)
        if (obj_info == i.object_info) {
            if (obj_instance == instance)
                return (int32_t)(&i - auth->object.data());
            obj_instance++;
        }
    return -1;
}

static int32_t auth_3d_get_auth_3d_object_index_by_hash(auth_3d* auth,
    uint32_t object_hash, int32_t instance) {
    int32_t obj_instance = 0;
    for (auth_3d_object& i : auth->object)
        if (object_hash == i.object_hash) {
            if (obj_instance == instance)
                return (int32_t)(&i - auth->object.data());
            obj_instance++;
        }
    return -1;
}

static mat4* auth_3d_get_auth_3d_object_hrc_bone_mats(auth_3d* auth, size_t index) {
    if (index >= auth->object_hrc.size() || auth->object_hrc[index].mats.size() < 1)
        return 0;
    return auth->object_hrc[index].mats.data();
}

static int32_t auth_3d_get_auth_3d_object_hrc_index_by_object_info(auth_3d* auth,
    object_info obj_info, int32_t instance) {
    if (instance < 0) {
        for (auth_3d_object_hrc& i : auth->object_hrc)
            if (obj_info == i.object_info)
                return (int32_t)(&i - auth->object_hrc.data());
    }
    else {
        int32_t obj_instance = 0;
        for (auth_3d_object_hrc& i : auth->object_hrc)
            if (obj_info == i.object_info) {
                if (obj_instance == instance)
                    return (int32_t)(&i - auth->object_hrc.data());
                obj_instance++;
            }
    }
    return -1;
}

static int32_t auth_3d_get_auth_3d_object_hrc_index_by_hash(auth_3d* auth,
    uint32_t object_hash, int32_t instance) {
    if (instance < 0) {
        for (auth_3d_object_hrc& i : auth->object_hrc)
            if (object_hash == i.object_hash)
                return (int32_t)(&i - auth->object_hrc.data());
    }
    else {
        int32_t obj_instance = 0;
        for (auth_3d_object_hrc& i : auth->object_hrc)
            if (object_hash == i.object_hash) {
                if (obj_instance == instance)
                    return (int32_t)(&i - auth->object_hrc.data());
                obj_instance++;
            }
    }
    return -1;
}

static void auth_3d_read_file(auth_3d* auth, auth_3d_database* auth_3d_db) {
    if (auth->uid >= auth_3d_db->uid.size())
        return;

    auth_3d_database_uid* db_uid = &auth_3d_db->uid[auth->uid];
    if (db_uid->enabled) {
        auth_3d_uid_file* uid_file = &auth_3d_data->uid_files[auth->uid];
        if (uid_file->uid == auth->uid) {
            auth_3d_uid_file_load_file(uid_file, auth_3d_db);
            auth->state = 1;
        }
    }
}

static void auth_3d_read_file_modern(auth_3d* auth) {
    auto elem = auth_3d_data->uid_files_modern.find(auth->hash);
    if (elem == auth_3d_data->uid_files_modern.end())
        return;

    auth_3d_uid_file_modern* uid_file = &elem->second;
    if (uid_file->hash == auth->hash) {
        auth_3d_uid_file_modern_load_file(uid_file);
        auth->state = 1;
    }
}

static void auth_3d_set_material_list(auth_3d* auth, render_context* rctx) {
    object_data* object_data = &rctx->object_data;

    int32_t mat_list_count = 0;
    material_list_struct mat_list[TEXTURE_PATTERN_COUNT];
    for (auth_3d_material_list& i : auth->material_list) {
        if (i.blend_color.flags) {
            vec4& blend_color = mat_list[mat_list_count].blend_color;
            vec4u8& has_blend_color = mat_list[mat_list_count].has_blend_color;

            blend_color = i.blend_color.value;
            has_blend_color.x = !!(i.blend_color.flags & AUTH_3D_RGBA_R);
            has_blend_color.y = !!(i.blend_color.flags & AUTH_3D_RGBA_B);
            has_blend_color.z = !!(i.blend_color.flags & AUTH_3D_RGBA_G);
            has_blend_color.w = !!(i.blend_color.flags & AUTH_3D_RGBA_A);
        }
        else {
            mat_list[mat_list_count].blend_color = {};
            mat_list[mat_list_count].has_blend_color = {};
        }

        if (i.incandescence.flags) {
            vec4& emission = mat_list[mat_list_count].emission;
            vec4u8& has_emission = mat_list[mat_list_count].has_emission;

            emission = i.incandescence.value;
            has_emission.x = !!(i.incandescence.flags & AUTH_3D_RGBA_R);
            has_emission.y = !!(i.incandescence.flags & AUTH_3D_RGBA_B);
            has_emission.z = !!(i.incandescence.flags & AUTH_3D_RGBA_G);
            has_emission.w = !!(i.incandescence.flags & AUTH_3D_RGBA_A);
        }
        else {
            mat_list[mat_list_count].emission = {};
            mat_list[mat_list_count].has_emission = {};
        }

        if (!i.blend_color.flags && !i.incandescence.flags)
            continue;

        mat_list[mat_list_count].hash = i.hash;
        mat_list_count++;
    }

    object_data->set_material_list(mat_list_count, mat_list);
}

static void auth_3d_ambient_load(auth_3d* auth, auth_3d_ambient* a, auth_3d_ambient_file* af) {
    if (af->flags & A3DA_AMBIENT_LIGHT_DIFFUSE) {
        auth_3d_rgba_load(auth, &a->light_diffuse, &af->light_diffuse);
        enum_or(a->flags, AUTH_3D_AMBIENT_LIGHT_DIFFUSE);
    }

    a->name.assign(af->name);

    if (af->flags & A3DA_AMBIENT_LIGHT_DIFFUSE) {
        auth_3d_rgba_load(auth, &a->rim_light_diffuse, &af->rim_light_diffuse);
        enum_or(a->flags, AUTH_3D_AMBIENT_LIGHT_DIFFUSE);
    }
}

static void auth_3d_ambient_set(auth_3d_ambient* a, render_context* rctx) {

}

static void auth_3d_camera_auxiliary_load(auth_3d* auth, auth_3d_camera_auxiliary_file* caf) {
    auth_3d_camera_auxiliary* ca = &auth->camera_auxiliary;

    if (caf->flags & A3DA_CAMERA_AUXILIARY_AUTO_EXPOSURE) {
        auth_3d_key_load(auth, &ca->auto_exposure, &caf->auto_exposure);
        enum_or(ca->flags, AUTH_3D_CAMERA_AUXILIARY_AUTO_EXPOSURE);
    }

    if (caf->flags & A3DA_CAMERA_AUXILIARY_EXPOSURE) {
        auth_3d_key_load(auth, &ca->exposure, &caf->exposure);
        enum_or(ca->flags, AUTH_3D_CAMERA_AUXILIARY_EXPOSURE);
    }

    if (caf->flags & A3DA_CAMERA_AUXILIARY_EXPOSURE_RATE) {
        auth_3d_key_load(auth, &ca->exposure_rate, &caf->exposure_rate);
        enum_or(ca->flags, AUTH_3D_CAMERA_AUXILIARY_EXPOSURE_RATE);
    }

    if (caf->flags & A3DA_CAMERA_AUXILIARY_GAMMA) {
        auth_3d_key_load(auth, &ca->gamma, &caf->gamma);
        enum_or(ca->flags, AUTH_3D_CAMERA_AUXILIARY_GAMMA);
    }

    if (caf->flags & A3DA_CAMERA_AUXILIARY_GAMMA_RATE) {
        auth_3d_key_load(auth, &ca->gamma_rate, &caf->gamma_rate);
        enum_or(ca->flags, AUTH_3D_CAMERA_AUXILIARY_GAMMA_RATE);
    }

    if (caf->flags & A3DA_CAMERA_AUXILIARY_SATURATE) {
        auth_3d_key_load(auth, &ca->saturate, &caf->saturate);
        enum_or(ca->flags, AUTH_3D_CAMERA_AUXILIARY_SATURATE);
    }
}

static void auth_3d_camera_auxiliary_restore_prev_value(auth_3d_camera_auxiliary* ca, render_context* rctx) {
    post_process_tone_map* tm = rctx->post_process.tone_map;

    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_AUTO_EXPOSURE)
        tm->set_auto_exposure(true);
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_EXPOSURE)
        tm->set_exposure(1.0);
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_GAMMA)
        tm->set_gamma(1.0);
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_GAMMA_RATE)
        tm->set_gamma_rate(1.0);
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_SATURATE)
        tm->set_saturate_coeff(1.0);
}

static void auth_3d_camera_auxiliary_set(auth_3d_camera_auxiliary* ca, render_context* rctx) {
    post_process_tone_map* tm = rctx->post_process.tone_map;

    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_AUTO_EXPOSURE)
        tm->set_auto_exposure(ca->auto_exposure_value > 0.0f);
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_EXPOSURE)
        tm->set_exposure(ca->exposure_value);
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_GAMMA)
        tm->set_gamma(ca->gamma_value);
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_GAMMA_RATE)
        tm->set_gamma_rate(ca->gamma_rate_value);
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_SATURATE)
        tm->set_saturate_coeff(ca->saturate_value);
}

static float_t auth_3d_camera_root_calc_frame(auth_3d_camera_root* cr, float_t frame, render_context* rctx) {
    camera* cam = rctx->camera;
    if (auth_3d_key_detect_fast_change(&cr->view_point.model_transform.translation.x, frame, 0.3f)
        || auth_3d_key_detect_fast_change(&cr->view_point.model_transform.translation.y, frame, 0.3f)
        || auth_3d_key_detect_fast_change(&cr->view_point.model_transform.translation.z, frame, 0.3f)
        || auth_3d_key_detect_fast_change(&cr->interest.translation.x, frame, 0.3f)
        || auth_3d_key_detect_fast_change(&cr->interest.translation.y, frame, 0.3f)
        || auth_3d_key_detect_fast_change(&cr->interest.translation.z, frame, 0.3f)) {
        frame = (float_t)(int32_t)frame;
        cam->set_fast_change(true);
    }
    else {
        float_t frame_prev = frame - get_delta_frame();
        if (frame_prev > 0.0f && (int32_t)frame - (int32_t)frame_prev == 2) {
            frame_prev = (float_t)((int32_t)frame_prev + 1);
            if (auth_3d_key_detect_fast_change(&cr->view_point.model_transform.translation.x, frame_prev, 0.3f)
                || auth_3d_key_detect_fast_change(&cr->view_point.model_transform.translation.y, frame_prev, 0.3f)
                || auth_3d_key_detect_fast_change(&cr->view_point.model_transform.translation.z, frame_prev, 0.3f)
                || auth_3d_key_detect_fast_change(&cr->interest.translation.x, frame_prev, 0.3f)
                || auth_3d_key_detect_fast_change(&cr->interest.translation.y, frame_prev, 0.3f)
                || auth_3d_key_detect_fast_change(&cr->interest.translation.z, frame_prev, 0.3f))
                cam->set_fast_change_hist0(true);
        }
    }
    return frame;
}

static void auth_3d_camera_root_load(auth_3d* auth, auth_3d_camera_root* cr, auth_3d_camera_root_file* crf) {
    auth_3d_model_transform_load(auth, &cr->interest, &crf->interest);
    auth_3d_model_transform_load(auth, &cr->model_transform, &crf->model_transform);
    auth_3d_camera_root_view_point_load(auth, &cr->view_point, &crf->view_point);
}

static void auth_3d_camera_root_view_point_load(auth_3d* auth, auth_3d_camera_root_view_point* crvp,
    auth_3d_camera_root_view_point_file* crvpf) {
    crvp->aspect = crvpf->aspect;

    if (crvpf->flags & A3DA_CAMERA_ROOT_VIEW_POINT_FOV) {
        auth_3d_key_load(auth, &crvp->fov, &crvpf->fov);
        crvp->fov_is_horizontal = crvpf->fov_is_horizontal;
        enum_or(crvp->flags, AUTH_3D_CAMERA_ROOT_VIEW_POINT_FOV);
    }
    else {
        crvp->camera_aperture_h = crvpf->camera_aperture_h;
        crvp->camera_aperture_w = crvpf->camera_aperture_w;
        auth_3d_key_load(auth, &crvp->focal_length, &crvpf->focal_length);
    }

    auth_3d_model_transform_load(auth, &crvp->model_transform, &crvpf->model_transform);

    if (crvpf->flags & A3DA_CAMERA_ROOT_VIEW_POINT_ROLL) {
        auth_3d_key_load(auth, &crvp->roll, &crvpf->roll);
        enum_or(crvp->flags, AUTH_3D_CAMERA_ROOT_VIEW_POINT_ROLL);
    }
}

static void auth_3d_chara_disp(auth_3d_chara* c, mat4* parent_mat, render_context* rctx) {

}

static void auth_3d_chara_load(auth_3d* auth, auth_3d_chara* c, auth_3d_chara_file* cf) {
    auth_3d_model_transform_load(auth, &c->model_transform, &cf->model_transform);
    c->name.assign(cf->name);
}

static void auth_3d_curve_load(auth_3d* auth, auth_3d_curve* c, auth_3d_curve_file* cf) {
    auth_3d_key_load(auth, &c->curve, &cf->curve);
    c->name.assign(cf->name);
}

static void auth_3d_dof_load(auth_3d* auth, auth_3d_dof* d, auth_3d_dof_file* df) {
    if (df->has_dof) {
        auth_3d_model_transform_load(auth, &d->model_transform, &df->model_transform);
        d->has_dof = true;
    }
}

static void auth_3d_dof_restore_prev_value(auth_3d_dof* d, render_context* rctx) {
    if (!d->has_dof)
        return;

    rctx->post_process.dof->set_dof_pv(0);
}

static void auth_3d_dof_set(auth_3d_dof* d, render_context* rctx) {
    if (!d->has_dof)
        return;

    vec3 view_point;
    rctx->camera->get_view_point(view_point);

    float_t focus = vec3::distance(d->model_transform.translation_value, view_point);

    dof_pv pv;
    pv.enable = fabsf(d->model_transform.rotation_value.z) > 0.000001f;
    pv.f2.focus = focus;
    pv.f2.focus_range = d->model_transform.scale_value.x;
    pv.f2.fuzzing_range = d->model_transform.rotation_value.x;
    pv.f2.ratio = d->model_transform.rotation_value.y;
    rctx->post_process.dof->set_dof_pv(&pv);
}

static auth_3d_detail::Event* auth_3d_event_load(auth_3d* auth, auth_3d_event_file* ef) {
    auth_3d_detail::Event* e = 0;
    switch (ef->type) {
    case A3DA_EVENT_MISC:
    default:
        e = new auth_3d_detail::Event(ef);
        break;
    case A3DA_EVENT_FILT: {
        if (!ef->param1.compare("WHITEIN") || !ef->param1.compare("WHITEOUT")
            || !ef->param1.compare("FADEIN") || !ef->param1.compare("FADEOUT"))
            e = new auth_3d_detail::EventFilterFade(ef);
        else if (!ef->param1.compare("TIMESTOP"))
            e = new auth_3d_detail::EventFilterTimeStop(ef);
        else
            e = new auth_3d_detail::Event(ef);
        break;
    }
    case A3DA_EVENT_FX:
        if (!ef->param1.compare("SMOOTH"))
            e = new auth_3d_detail::EventFXSmoothCut(ef);
        else
            e = new auth_3d_detail::EventFX(ef);
        break;
    case A3DA_EVENT_SND:
        e = new auth_3d_detail::EventSnd(ef);
        break;
    case A3DA_EVENT_MOT:
        e = new auth_3d_detail::EventMot(ef);
        break;
    case A3DA_EVENT_A2D:
        e = new auth_3d_detail::EventA2d(ef);
        break;
    }
    return e;
}

static const char* auth_3d_event_type_string(auth_3d_detail::Event::Type type) {
    switch (type) {
    case auth_3d_detail::Event::Type::MISC:
        return "MISC";
    case auth_3d_detail::Event::Type::FILT:
        return "FLT";
    case auth_3d_detail::Event::Type::FX:
        return "FX";
    case auth_3d_detail::Event::Type::SND:
        return "SND";
    case auth_3d_detail::Event::Type::MOT:
        return "MOT";
    case auth_3d_detail::Event::Type::A2D:
        return "A2D";
    default:
        return "(unknown)";
    }
}

static void auth_3d_fog_load(auth_3d* auth, auth_3d_fog* f, auth_3d_fog_file* ff) {
    if (ff->flags & A3DA_FOG_COLOR) {
        auth_3d_rgba_load(auth, &f->color, &ff->color);
        enum_or(f->flags, AUTH_3D_FOG_COLOR);
    }

    if (ff->flags & A3DA_FOG_DENSITY) {
        auth_3d_key_load(auth, &f->density, &ff->density);
        enum_or(f->flags, AUTH_3D_FOG_DENSITY);
    }

    if (ff->flags & A3DA_FOG_END) {
        auth_3d_key_load(auth, &f->end, &ff->end);
        enum_or(f->flags, AUTH_3D_FOG_END);
    }

    f->id = ff->id;

    if (ff->flags & A3DA_FOG_START) {
        auth_3d_key_load(auth, &f->start, &ff->start);
        enum_or(f->flags, AUTH_3D_FOG_START);
    }
}

static void auth_3d_fog_restore_prev_value(auth_3d_fog* f, render_context* rctx) {
    fog_id id = f->id;
    if (id < FOG_DEPTH || id > FOG_BUMP)
        return;

    fog* data = &rctx->fog[id];
    if (f->flags_init & AUTH_3D_FOG_COLOR)
        data->set_color(f->color_init);

    if (f->flags_init & AUTH_3D_FOG_DENSITY)
        data->set_density(f->density_value);

    if (f->flags_init & AUTH_3D_FOG_END)
        data->set_end(f->end_value);

    if (f->flags_init & AUTH_3D_FOG_START)
        data->set_start(f->start_value);
}

static void auth_3d_fog_set(auth_3d_fog* f, render_context* rctx) {
    fog_id id = f->id;
    if (id < FOG_DEPTH || id > FOG_BUMP)
        return;

    fog* data = &rctx->fog[id];
    if (f->flags & AUTH_3D_FOG_COLOR) {
        if (!(f->flags_init & AUTH_3D_FOG_COLOR)) {
            vec4 color_init;
            data->get_color(color_init);
            f->color_init = color_init;
            enum_or(f->flags_init, AUTH_3D_FOG_COLOR);
        }

        data->set_color(f->color.value);
    }

    if (f->flags & AUTH_3D_FOG_DENSITY) {
        if (!(f->flags_init & AUTH_3D_FOG_DENSITY)) {
            f->density_init = data->get_density();
            enum_or(f->flags_init, AUTH_3D_FOG_DENSITY);
        }

        data->set_density(f->density_value);
    }

    if (f->flags & AUTH_3D_FOG_END) {
        if (!(f->flags_init & AUTH_3D_FOG_END)) {
            f->end_init = data->get_end();
            enum_or(f->flags_init, AUTH_3D_FOG_END);
        }

        data->set_end(f->end_value);
    }

    if (f->flags & AUTH_3D_FOG_START) {
        if (!(f->flags_init & AUTH_3D_FOG_START)) {
            f->start_init = data->get_start();
            enum_or(f->flags_init, AUTH_3D_FOG_START);
        }

        data->set_start(f->start_value);
    }
}

static void auth_3d_light_load(auth_3d* auth, auth_3d_light* l, auth_3d_light_file* lf) {
    if (lf->flags & A3DA_LIGHT_AMBIENT) {
        auth_3d_rgba_load(auth, &l->ambient, &lf->ambient);
        enum_or(l->flags, AUTH_3D_LIGHT_AMBIENT);
    }

    if (lf->flags & A3DA_LIGHT_CONE_ANGLE) {
        auth_3d_key_load(auth, &l->cone_angle, &lf->cone_angle);
        enum_or(l->flags, AUTH_3D_LIGHT_CONE_ANGLE);
    }

    if (lf->flags & A3DA_LIGHT_CONSTANT) {
        auth_3d_key_load(auth, &l->constant, &lf->constant);
        enum_or(l->flags, AUTH_3D_LIGHT_CONSTANT);
    }

    if (lf->flags & A3DA_LIGHT_DIFFUSE) {
        auth_3d_rgba_load(auth, &l->diffuse, &lf->diffuse);
        enum_or(l->flags, AUTH_3D_LIGHT_DIFFUSE);
    }

    if (lf->flags & A3DA_LIGHT_DROP_OFF) {
        auth_3d_key_load(auth, &l->drop_off, &lf->drop_off);
        enum_or(l->flags, AUTH_3D_LIGHT_DROP_OFF);
    }

    if (lf->flags & A3DA_LIGHT_FAR) {
        auth_3d_key_load(auth, &l->_far, &lf->_far);
        enum_or(l->flags, AUTH_3D_LIGHT_FAR);
    }

    l->id = lf->id;

    if (lf->flags & A3DA_LIGHT_INTENSITY) {
        auth_3d_key_load(auth, &l->intensity, &lf->intensity);
        enum_or(l->flags, AUTH_3D_LIGHT_INTENSITY);
    }

    if (lf->flags & A3DA_LIGHT_LINEAR) {
        auth_3d_key_load(auth, &l->linear, &lf->linear);
        enum_or(l->flags, AUTH_3D_LIGHT_LINEAR);
    }

    if (lf->flags & A3DA_LIGHT_POSITION) {
        auth_3d_model_transform_load(auth, &l->position, &lf->position);
        enum_or(l->flags, AUTH_3D_LIGHT_POSITION);
    }

    if (lf->flags & A3DA_LIGHT_QUADRATIC) {
        auth_3d_key_load(auth, &l->quadratic, &lf->quadratic);
        enum_or(l->flags, AUTH_3D_LIGHT_QUADRATIC);
    }

    if (lf->flags & A3DA_LIGHT_SPECULAR) {
        auth_3d_rgba_load(auth, &l->specular, &lf->specular);
        enum_or(l->flags, AUTH_3D_LIGHT_SPECULAR);
    }

    if (lf->flags & A3DA_LIGHT_SPOT_DIRECTION) {
        auth_3d_model_transform_load(auth, &l->spot_direction, &lf->spot_direction);
        enum_or(l->flags, AUTH_3D_LIGHT_SPOT_DIRECTION);
    }

    if (lf->flags & A3DA_LIGHT_TONE_CURVE) {
        auth_3d_rgba_load(auth, &l->tone_curve, &lf->tone_curve);
        enum_or(l->flags, AUTH_3D_LIGHT_TONE_CURVE);
    }

    l->type.assign(lf->type);
}

static void auth_3d_light_restore_prev_value(auth_3d_light* l, render_context* rctx) {
    light_set* set_data = &rctx->light_set[LIGHT_SET_MAIN];

    light_id id = l->id;
    if (id < LIGHT_CHARA || id > LIGHT_PROJECTION)
        return;

    light_data* data = &set_data->lights[id];
    if (l->flags_init & AUTH_3D_LIGHT_AMBIENT)
        data->set_ambient(l->ambient_init);

    if (l->flags_init & AUTH_3D_LIGHT_DIFFUSE)
        data->set_diffuse(l->diffuse_init);

    if (l->flags_init & AUTH_3D_LIGHT_SPECULAR)
        data->set_specular(l->specular_init);

    if (l->flags_init & AUTH_3D_LIGHT_TONE_CURVE)
        data->set_tone_curve(l->tone_curve_init);
}

static void auth_3d_light_set(auth_3d_light* l, render_context* rctx) {
    light_set* set_data = &rctx->light_set[LIGHT_SET_MAIN];

    light_id id = l->id;
    if (id < LIGHT_CHARA || id > LIGHT_PROJECTION)
        return;

    light_data* data = &set_data->lights[id];
    if (l->flags & AUTH_3D_LIGHT_AMBIENT) {
        if (!(l->flags_init & AUTH_3D_LIGHT_AMBIENT)) {
            vec4 ambient_init;
            data->get_ambient(ambient_init);
            l->ambient_init = ambient_init;
            enum_or(l->flags_init, AUTH_3D_LIGHT_AMBIENT);
        }

        bool set[4];
        set[0] = !!(l->ambient.flags & AUTH_3D_RGBA_R);
        set[1] = !!(l->ambient.flags & AUTH_3D_RGBA_G);
        set[2] = !!(l->ambient.flags & AUTH_3D_RGBA_B);
        set[3] = !!(l->ambient.flags & AUTH_3D_RGBA_A);
        data->set_ambient(l->ambient.value, set);
    }

    if (l->flags & AUTH_3D_LIGHT_DIFFUSE) {
        if (!(l->flags_init & AUTH_3D_LIGHT_DIFFUSE)) {
            vec4 diffuse_init;
            data->get_diffuse(diffuse_init);
            l->diffuse_init = diffuse_init;
            enum_or(l->flags_init, AUTH_3D_LIGHT_DIFFUSE);
        }

        bool set[4];
        set[0] = !!(l->diffuse.flags & AUTH_3D_RGBA_R);
        set[1] = !!(l->diffuse.flags & AUTH_3D_RGBA_G);
        set[2] = !!(l->diffuse.flags & AUTH_3D_RGBA_B);
        set[3] = !!(l->diffuse.flags & AUTH_3D_RGBA_A);
        data->set_diffuse(l->diffuse.value, set);
    }

    if (l->flags & AUTH_3D_LIGHT_POSITION)
        data->set_position(l->position.translation_value);

    if (l->flags & AUTH_3D_LIGHT_SPECULAR) {
        if (!(l->flags_init & AUTH_3D_LIGHT_SPECULAR)) {
            vec4 specular_init;
            data->get_specular(specular_init);
            l->specular_init = specular_init;
            enum_or(l->flags_init, AUTH_3D_LIGHT_SPECULAR);
        }

        bool set[4];
        set[0] = !!(l->specular.flags & AUTH_3D_RGBA_R);
        set[1] = !!(l->specular.flags & AUTH_3D_RGBA_G);
        set[2] = !!(l->specular.flags & AUTH_3D_RGBA_B);
        set[3] = !!(l->specular.flags & AUTH_3D_RGBA_A);
        data->set_specular(l->specular.value, set);
    }

    if (l->flags & AUTH_3D_LIGHT_SPOT_DIRECTION)
        data->set_spot_direction(l->spot_direction.translation_value);

    if (l->flags & AUTH_3D_LIGHT_TONE_CURVE) {
        if (!(l->flags_init & AUTH_3D_LIGHT_TONE_CURVE)) {
            data->get_tone_curve(l->tone_curve_init);
            enum_or(l->flags_init, AUTH_3D_LIGHT_TONE_CURVE);
        }

        light_tone_curve tone_curve;
        tone_curve.start_point = l->tone_curve.value.x;
        tone_curve.end_point = l->tone_curve.value.y;
        tone_curve.coefficient = l->tone_curve.value.z;
        data->set_tone_curve(tone_curve);
    }
}

static void auth_3d_m_object_hrc_get_mat(auth_3d_m_object_hrc* moh, mat4* mat) {
    for (auth_3d_object_node& i : moh->node)
        if (i.mat) {
            *mat = *i.mat;
            return;
        }

    *mat = moh->node.front().model_transform.mat;
}

static void auth_3d_m_object_hrc_disp(auth_3d_m_object_hrc* moh, auth_3d* auth, render_context* rctx) {
    if (!auth->visible || !moh->model_transform.visible)
        return;

    object_data* object_data = &rctx->object_data;

    for (auth_3d_object_instance& i : moh->instance) {
        if (!i.model_transform.visible)
            return;

        draw_task_flags flags = DRAW_TASK_SSS;
        shadow_type_enum shadow_type = SHADOW_CHARA;
        if (auth->shadow || i.shadow) {
            enum_or(flags, DRAW_TASK_4 | DRAW_TASK_SHADOW);
            shadow_type = SHADOW_STAGE;
        }
        if (auth->alpha < 1.0f)
            enum_or(flags, auth->draw_task_flags);

        object_data->set_draw_task_flags((draw_task_flags)flags);
        object_data->set_shadow_type(shadow_type);

        shadow* shad = rctx->draw_pass.shadow_ptr;
        if (shad && flags & DRAW_TASK_SHADOW) {
            object_data->set_shadow_type(SHADOW_STAGE);

            mat4* m = &moh->model_transform.mat;
            for (auth_3d_object_node& j : moh->node)
                if (j.mat) {
                    m = j.mat;
                    break;
                }

            mat4 mat = i.model_transform.mat;
            mat4 t = *m;
            mat4_mult(&mat, &t, &mat);

            vec3 pos = *(vec3*)&mat.row3;
            pos.y -= 0.2f;
            shad->field_1D0[shadow_type].push_back(pos);
        }

        if (i.mats.size())
            draw_task_add_draw_object_by_object_info_object_skin(rctx,
                i.object_info, 0, 0, auth->alpha, i.mats.data(), 0, 0, &i.model_transform.mat);
    }

    object_data->set_draw_task_flags();
    object_data->set_shadow_type(SHADOW_CHARA);
}

static void auth_3d_m_object_hrc_load(auth_3d* auth, auth_3d_m_object_hrc* moh,
    auth_3d_m_object_hrc_file* mohf, object_database* obj_db) {
    size_t node_index = 0;
    moh->node.resize(mohf->node.size());
    for (auth_3d_object_node_file& i : mohf->node)
        auth_3d_object_node_load(auth,
            &moh->node[node_index++], &i);

    auth_3d_object_model_transform_load(auth, &moh->model_transform, &mohf->model_transform);
    moh->name.assign(mohf->name);

    size_t instance_index = 0;
    moh->instance.resize(mohf->instance.size());
    for (auth_3d_object_instance_file& i : mohf->instance)
        auth_3d_object_instance_load(auth,
            &moh->instance[instance_index++], &i, moh, obj_db);
}

static void auth_3d_m_object_hrc_nodes_mat_mult(auth_3d_m_object_hrc* moh) {
    auth_3d_object_node* node = moh->node.data();

    for (auth_3d_object_node& i : moh->node)
        if (i.parent >= 0)
            i.model_transform.mat_mult(&node[i.parent].model_transform.mat);
        else
            i.model_transform.mat = i.model_transform.mat_inner;
}

static void auth_3d_material_list_load(auth_3d* auth, auth_3d_material_list* ml, auth_3d_material_list_file* mlf) {
    //int32_t ident[24] = {};
    if (mlf->flags & A3DA_MATERIAL_LIST_BLEND_COLOR) {
        auth_3d_rgba_load(auth, &ml->blend_color, &mlf->blend_color);
        enum_or(ml->flags, AUTH_3D_MATERIAL_LIST_BLEND_COLOR);

        /*auth_3d_key& r = ml->blend_color.r;
        auth_3d_key& g = ml->blend_color.g;
        auth_3d_key& b = ml->blend_color.b;
        auth_3d_key& a = ml->blend_color.a;

        switch (r.type) {
        case AUTH_3D_KEY_NONE:
        case AUTH_3D_KEY_STATIC:
            switch (g.type) {
            case AUTH_3D_KEY_NONE:
            case AUTH_3D_KEY_STATIC:
            case AUTH_3D_KEY_LINEAR:
            case AUTH_3D_KEY_HERMITE:
            case AUTH_3D_KEY_HOLD:
                ident[0] = 1;
                ident[6] = 1;
                break;
            }
            break;
        case AUTH_3D_KEY_LINEAR:
        case AUTH_3D_KEY_HERMITE:
        case AUTH_3D_KEY_HOLD:
            switch (g.type) {
            case AUTH_3D_KEY_NONE:
            case AUTH_3D_KEY_STATIC:
                ident[0] = 1;
                ident[6] = 1;
                break;
            case AUTH_3D_KEY_LINEAR:
            case AUTH_3D_KEY_HERMITE:
            case AUTH_3D_KEY_HOLD:
                if (r.length == g.length) {
                    ident[0] = 1;

                    kft3* keys1 = r.keys;
                    kft3* keys2 = g.keys;
                    size_t length = r.length;
                    for (size_t i = 0; i < length; i++)
                        if (*(uint32_t*)&keys1[i].frame != *(uint32_t*)&keys2[i].frame) {
                            ident[0] = 0;
                            break;
                        }

                    if (ident[0]) {
                        ident[6] = 1;

                        for (size_t i = 0; i < length; i++)
                            if (*(uint64_t*)&keys1[i].tangent1 != *(uint64_t*)&keys2[i].tangent1) {
                                ident[6] = 0;
                                break;
                            }
                    }
                }
                else if (r.length > g.length) {
                    if (g.type == AUTH_3D_KEY_LINEAR) {
                        kft3* keys1 = r.keys;
                        kft3* keys2 = g.keys;
                        size_t keys1_length = r.length;
                        size_t keys2_length = g.length;

                        for (size_t i = 0; i < keys1_length; i++) {
                            float_t frame = keys1[i].frame;
                            bool found = false;
                            for (size_t j = 0; j < keys2_length && i < keys1_length; j++)
                                if (frame == keys2[j].frame) {
                                    found = true;
                                    break;
                                }

                            if (!found)
                                if (frame <= keys2[0].frame || frame >= keys2[keys2_length - 1].frame) {
                                    ident[0] = 1;
                                    ident[6] = 1;
                                }
                                else
                                    for (size_t j = 0; j + 1 < keys2_length && i < keys1_length; j++)
                                        if (frame >= keys2[j].frame && keys2[j + 1].frame) {
                                            if (*(uint32_t*)&keys2[j].value == *(uint32_t*)&keys2[j + 1].value) {
                                                ident[0] = 1;
                                                ident[6] = 1;
                                            }
                                            break;
                                        }
                        }
                    }
                }
                else if (r.length < g.length) {
                    if (r.type == AUTH_3D_KEY_LINEAR) {
                        kft3* keys1 = g.keys;
                        kft3* keys2 = r.keys;
                        size_t keys1_length = g.length;
                        size_t keys2_length = r.length;

                        for (size_t i = 0; i < keys1_length; i++) {
                            float_t frame = keys1[i].frame;
                            bool found = false;
                            for (size_t j = 0; j < keys2_length && i < keys1_length; j++)
                                if (frame == keys2[j].frame) {
                                    found = true;
                                    break;
                                }

                            if (!found)
                                if (frame <= keys2[0].frame || frame >= keys2[keys2_length - 1].frame) {
                                    ident[0] = 1;
                                    ident[6] = 1;
                                }
                                else
                                    for (size_t j = 0; j + 1 < keys2_length && i < keys1_length; j++)
                                        if (frame >= keys2[j].frame && keys2[j + 1].frame) {
                                            if (*(uint32_t*)&keys2[j].value == *(uint32_t*)&keys2[j + 1].value) {
                                                ident[0] = 1;
                                                ident[6] = 1;
                                            }
                                            break;
                                        }
                        }
                    }
                }
                break;
            }
            break;
        }

        switch (r.type) {
        case AUTH_3D_KEY_NONE:
        case AUTH_3D_KEY_STATIC:
            switch (b.type) {
            case AUTH_3D_KEY_NONE:
            case AUTH_3D_KEY_STATIC:
            case AUTH_3D_KEY_LINEAR:
            case AUTH_3D_KEY_HERMITE:
            case AUTH_3D_KEY_HOLD:
                ident[1] = 1;
                ident[7] = 1;
                break;
            }
            break;
        case AUTH_3D_KEY_LINEAR:
        case AUTH_3D_KEY_HERMITE:
        case AUTH_3D_KEY_HOLD:
            switch (b.type) {
            case AUTH_3D_KEY_NONE:
            case AUTH_3D_KEY_STATIC:
                ident[1] = 1;
                ident[7] = 1;
                break;
            case AUTH_3D_KEY_LINEAR:
            case AUTH_3D_KEY_HERMITE:
            case AUTH_3D_KEY_HOLD:
                if (r.length == b.length) {
                    ident[1] = 1;

                    kft3* keys1 = r.keys;
                    kft3* keys2 = b.keys;
                    size_t length = r.length;
                    for (size_t i = 0; i < length; i++)
                        if (*(uint32_t*)&keys1[i].frame != *(uint32_t*)&keys2[i].frame) {
                            ident[1] = 0;
                            break;
                        }

                    if (ident[1]) {
                        ident[7] = 1;

                        for (size_t i = 0; i < length; i++)
                            if (*(uint64_t*)&keys1[i].tangent1 != *(uint64_t*)&keys2[i].tangent1) {
                                ident[7] = 0;
                                break;
                            }
                    }
                }
                else if (r.length > b.length) {
                    if (b.type == AUTH_3D_KEY_LINEAR) {
                        kft3* keys1 = r.keys;
                        kft3* keys2 = b.keys;
                        size_t keys1_length = r.length;
                        size_t keys2_length = b.length;

                        for (size_t i = 0; i < keys1_length; i++) {
                            float_t frame = keys1[i].frame;
                            bool found = false;
                            for (size_t j = 0; j < keys2_length && i < keys1_length; j++)
                                if (frame == keys2[j].frame) {
                                    found = true;
                                    break;
                                }

                            if (!found)
                                if (frame <= keys2[0].frame || frame >= keys2[keys2_length - 1].frame) {
                                    ident[1] = 1;
                                    ident[7] = 1;
                                }
                                else
                                    for (size_t j = 0; j + 1 < keys2_length && i < keys1_length; j++)
                                        if (frame >= keys2[j].frame && keys2[j + 1].frame) {
                                            if (*(uint32_t*)&keys2[j].value == *(uint32_t*)&keys2[j + 1].value) {
                                                ident[1] = 1;
                                                ident[7] = 1;
                                            }
                                            break;
                                        }
                        }
                    }
                }
                else if (r.length < b.length) {
                    if (r.type == AUTH_3D_KEY_LINEAR) {
                        kft3* keys1 = b.keys;
                        kft3* keys2 = r.keys;
                        size_t keys1_length = b.length;
                        size_t keys2_length = r.length;

                        for (size_t i = 0; i < keys1_length; i++) {
                            float_t frame = keys1[i].frame;
                            bool found = false;
                            for (size_t j = 0; j < keys2_length && i < keys1_length; j++)
                                if (frame == keys2[j].frame) {
                                    found = true;
                                    break;
                                }

                            if (!found)
                                if (frame <= keys2[0].frame || frame >= keys2[keys2_length - 1].frame) {
                                    ident[1] = 1;
                                    ident[7] = 1;
                                }
                                else
                                    for (size_t j = 0; j + 1 < keys2_length && i < keys1_length; j++)
                                        if (frame >= keys2[j].frame && keys2[j + 1].frame) {
                                            if (*(uint32_t*)&keys2[j].value == *(uint32_t*)&keys2[j + 1].value) {
                                                ident[1] = 1;
                                                ident[7] = 1;
                                            }
                                            break;
                                        }
                        }
                    }
                }
                break;
            }
            break;
        }

        switch (r.type) {
        case AUTH_3D_KEY_NONE:
        case AUTH_3D_KEY_STATIC:
            switch (a.type) {
            case AUTH_3D_KEY_NONE:
            case AUTH_3D_KEY_STATIC:
            case AUTH_3D_KEY_LINEAR:
            case AUTH_3D_KEY_HERMITE:
            case AUTH_3D_KEY_HOLD:
                ident[2] = 1;
                ident[8] = 1;
                break;
            }
            break;
        case AUTH_3D_KEY_LINEAR:
        case AUTH_3D_KEY_HERMITE:
        case AUTH_3D_KEY_HOLD:
            switch (a.type) {
            case AUTH_3D_KEY_NONE:
            case AUTH_3D_KEY_STATIC:
                ident[2] = 1;
                ident[8] = 1;
                break;
            case AUTH_3D_KEY_LINEAR:
            case AUTH_3D_KEY_HERMITE:
            case AUTH_3D_KEY_HOLD:
                if (r.length == a.length) {
                    ident[2] = 1;

                    kft3* keys1 = r.keys;
                    kft3* keys2 = a.keys;
                    size_t length = r.length;
                    for (size_t i = 0; i < length; i++)
                        if (*(uint32_t*)&keys1[i].frame != *(uint32_t*)&keys2[i].frame) {
                            ident[2] = 0;
                            break;
                        }

                    if (ident[2]) {
                        ident[8] = 1;

                        for (size_t i = 0; i < length; i++)
                            if (*(uint64_t*)&keys1[i].tangent1 != *(uint64_t*)&keys2[i].tangent1) {
                                ident[8] = 0;
                                break;
                            }
                    }
                }
                else if (r.length > a.length) {
                    if (a.type == AUTH_3D_KEY_LINEAR) {
                        kft3* keys1 = r.keys;
                        kft3* keys2 = a.keys;
                        size_t keys1_length = r.length;
                        size_t keys2_length = a.length;

                        for (size_t i = 0; i < keys1_length; i++) {
                            float_t frame = keys1[i].frame;
                            bool found = false;
                            for (size_t j = 0; j < keys2_length && i < keys1_length; j++)
                                if (frame == keys2[j].frame) {
                                    found = true;
                                    break;
                                }

                            if (!found)
                                if (frame <= keys2[0].frame || frame >= keys2[keys2_length - 1].frame) {
                                    ident[2] = 1;
                                    ident[8] = 1;
                                }
                                else
                                    for (size_t j = 0; j + 1 < keys2_length && i < keys1_length; j++)
                                        if (frame >= keys2[j].frame && keys2[j + 1].frame) {
                                            if (*(uint32_t*)&keys2[j].value == *(uint32_t*)&keys2[j + 1].value) {
                                                ident[2] = 1;
                                                ident[8] = 1;
                                            }
                                            break;
                                        }
                        }
                    }
                }
                else if (r.length < a.length) {
                    if (r.type == AUTH_3D_KEY_LINEAR) {
                        kft3* keys1 = a.keys;
                        kft3* keys2 = r.keys;
                        size_t keys1_length = a.length;
                        size_t keys2_length = r.length;

                        for (size_t i = 0; i < keys1_length; i++) {
                            float_t frame = keys1[i].frame;
                            bool found = false;
                            for (size_t j = 0; j < keys2_length && i < keys1_length; j++)
                                if (frame == keys2[j].frame) {
                                    found = true;
                                    break;
                                }

                            if (!found)
                                if (frame <= keys2[0].frame || frame >= keys2[keys2_length - 1].frame) {
                                    ident[2] = 1;
                                    ident[8] = 1;
                                }
                                else
                                    for (size_t j = 0; j + 1 < keys2_length && i < keys1_length; j++)
                                        if (frame >= keys2[j].frame && keys2[j + 1].frame) {
                                            if (*(uint32_t*)&keys2[j].value == *(uint32_t*)&keys2[j + 1].value) {
                                                ident[2] = 1;
                                                ident[8] = 1;
                                            }
                                            break;
                                        }
                        }
                    }
                }
                break;
            }
            break;
        }

        switch (g.type) {
        case AUTH_3D_KEY_NONE:
        case AUTH_3D_KEY_STATIC:
            switch (b.type) {
            case AUTH_3D_KEY_NONE:
            case AUTH_3D_KEY_STATIC:
            case AUTH_3D_KEY_LINEAR:
            case AUTH_3D_KEY_HERMITE:
            case AUTH_3D_KEY_HOLD:
                ident[3] = 1;
                ident[9] = 1;
                break;
            }
            break;
        case AUTH_3D_KEY_LINEAR:
        case AUTH_3D_KEY_HERMITE:
        case AUTH_3D_KEY_HOLD:
            switch (b.type) {
            case AUTH_3D_KEY_NONE:
            case AUTH_3D_KEY_STATIC:
                ident[3] = 1;
                ident[9] = 1;
                break;
            case AUTH_3D_KEY_LINEAR:
            case AUTH_3D_KEY_HERMITE:
            case AUTH_3D_KEY_HOLD:
                if (g.length == b.length) {
                    ident[3] = 1;

                    kft3* keys1 = g.keys;
                    kft3* keys2 = b.keys;
                    size_t length = g.length;
                    for (size_t i = 0; i < length; i++)
                        if (*(uint32_t*)&keys1[i].frame != *(uint32_t*)&keys2[i].frame) {
                            ident[3] = 0;
                            break;
                        }

                    if (ident[3]) {
                        ident[9] = 1;

                        for (size_t i = 0; i < length; i++)
                            if (*(uint64_t*)&keys1[i].tangent1 != *(uint64_t*)&keys2[i].tangent1) {
                                ident[9] = 0;
                                break;
                            }
                    }
                }
                else if (g.length > b.length) {
                    if (b.type == AUTH_3D_KEY_LINEAR) {
                        kft3* keys1 = g.keys;
                        kft3* keys2 = b.keys;
                        size_t keys1_length = g.length;
                        size_t keys2_length = b.length;

                        for (size_t i = 0; i < keys1_length; i++) {
                            float_t frame = keys1[i].frame;
                            bool found = false;
                            for (size_t j = 0; j < keys2_length && i < keys1_length; j++)
                                if (frame == keys2[j].frame) {
                                    found = true;
                                    break;
                                }

                            if (!found)
                                if (frame <= keys2[0].frame || frame >= keys2[keys2_length - 1].frame) {
                                    ident[3] = 1;
                                    ident[9] = 1;
                                }
                                else
                                    for (size_t j = 0; j + 1 < keys2_length && i < keys1_length; j++)
                                        if (frame >= keys2[j].frame && keys2[j + 1].frame) {
                                            if (*(uint32_t*)&keys2[j].value == *(uint32_t*)&keys2[j + 1].value) {
                                                ident[3] = 1;
                                                ident[9] = 1;
                                            }
                                            break;
                                        }
                        }
                    }
                }
                else if (g.length < b.length) {
                    if (g.type == AUTH_3D_KEY_LINEAR) {
                        kft3* keys1 = b.keys;
                        kft3* keys2 = g.keys;
                        size_t keys1_length = b.length;
                        size_t keys2_length = g.length;

                        for (size_t i = 0; i < keys1_length; i++) {
                            float_t frame = keys1[i].frame;
                            bool found = false;
                            for (size_t j = 0; j < keys2_length && i < keys1_length; j++)
                                if (frame == keys2[j].frame) {
                                    found = true;
                                    break;
                                }

                            if (!found)
                                if (frame <= keys2[0].frame || frame >= keys2[keys2_length - 1].frame) {
                                    ident[3] = 1;
                                    ident[9] = 1;
                                }
                                else
                                    for (size_t j = 0; j + 1 < keys2_length && i < keys1_length; j++)
                                        if (frame >= keys2[j].frame && keys2[j + 1].frame) {
                                            if (*(uint32_t*)&keys2[j].value == *(uint32_t*)&keys2[j + 1].value) {
                                                ident[3] = 1;
                                                ident[9] = 1;
                                            }
                                            break;
                                        }
                        }
                    }
                }
                break;
            }
            break;
        }

        switch (g.type) {
        case AUTH_3D_KEY_NONE:
        case AUTH_3D_KEY_STATIC:
            switch (a.type) {
            case AUTH_3D_KEY_NONE:
            case AUTH_3D_KEY_STATIC:
            case AUTH_3D_KEY_LINEAR:
            case AUTH_3D_KEY_HERMITE:
            case AUTH_3D_KEY_HOLD:
                ident[4] = 1;
                ident[10] = 1;
                break;
            }
            break;
        case AUTH_3D_KEY_LINEAR:
        case AUTH_3D_KEY_HERMITE:
        case AUTH_3D_KEY_HOLD:
            switch (a.type) {
            case AUTH_3D_KEY_NONE:
            case AUTH_3D_KEY_STATIC:
                ident[4] = 1;
                ident[10] = 1;
                break;
            case AUTH_3D_KEY_LINEAR:
            case AUTH_3D_KEY_HERMITE:
            case AUTH_3D_KEY_HOLD:
                if (g.length == a.length) {
                    ident[4] = 1;

                    kft3* keys1 = g.keys;
                    kft3* keys2 = a.keys;
                    size_t length = g.length;
                    for (size_t i = 0; i < length; i++)
                        if (*(uint32_t*)&keys1[i].frame != *(uint32_t*)&keys2[i].frame) {
                            ident[4] = 0;
                            break;
                        }

                    if (ident[4]) {
                        ident[10] = 1;

                        for (size_t i = 0; i < length; i++)
                            if (*(uint64_t*)&keys1[i].tangent1 != *(uint64_t*)&keys2[i].tangent1) {
                                ident[10] = 0;
                                break;
                            }
                    }
                }
                else if (g.length > a.length) {
                    if (a.type == AUTH_3D_KEY_LINEAR) {
                        kft3* keys1 = g.keys;
                        kft3* keys2 = a.keys;
                        size_t keys1_length = g.length;
                        size_t keys2_length = a.length;

                        for (size_t i = 0; i < keys1_length; i++) {
                            float_t frame = keys1[i].frame;
                            bool found = false;
                            for (size_t j = 0; j < keys2_length && i < keys1_length; j++)
                                if (frame == keys2[j].frame) {
                                    found = true;
                                    break;
                                }

                            if (!found)
                                if (frame <= keys2[0].frame || frame >= keys2[keys2_length - 1].frame) {
                                    ident[4] = 1;
                                    ident[10] = 1;
                                }
                                else
                                    for (size_t j = 0; j + 1 < keys2_length && i < keys1_length; j++)
                                        if (frame >= keys2[j].frame && keys2[j + 1].frame) {
                                            if (*(uint32_t*)&keys2[j].value == *(uint32_t*)&keys2[j + 1].value) {
                                                ident[4] = 1;
                                                ident[10] = 1;
                                            }
                                            break;
                                        }
                        }
                    }
                }
                else if (g.length < a.length) {
                    if (g.type == AUTH_3D_KEY_LINEAR) {
                        kft3* keys1 = a.keys;
                        kft3* keys2 = g.keys;
                        size_t keys1_length = a.length;
                        size_t keys2_length = g.length;

                        for (size_t i = 0; i < keys1_length; i++) {
                            float_t frame = keys1[i].frame;
                            bool found = false;
                            for (size_t j = 0; j < keys2_length && i < keys1_length; j++)
                                if (frame == keys2[j].frame) {
                                    found = true;
                                    break;
                                }

                            if (!found)
                                if (frame <= keys2[0].frame || frame >= keys2[keys2_length - 1].frame) {
                                    ident[4] = 1;
                                    ident[10] = 1;
                                }
                                else
                                    for (size_t j = 0; j + 1 < keys2_length && i < keys1_length; j++)
                                        if (frame >= keys2[j].frame && keys2[j + 1].frame) {
                                            if (*(uint32_t*)&keys2[j].value == *(uint32_t*)&keys2[j + 1].value) {
                                                ident[4] = 1;
                                                ident[10] = 1;
                                            }
                                            break;
                                        }
                        }
                    }
                }
                break;
            }
            break;
        }

        switch (b.type) {
        case AUTH_3D_KEY_NONE:
        case AUTH_3D_KEY_STATIC:
            switch (a.type) {
            case AUTH_3D_KEY_NONE:
            case AUTH_3D_KEY_STATIC:
            case AUTH_3D_KEY_LINEAR:
            case AUTH_3D_KEY_HERMITE:
            case AUTH_3D_KEY_HOLD:
                ident[5] = 1;
                ident[11] = 1;
                break;
            }
            break;
        case AUTH_3D_KEY_LINEAR:
        case AUTH_3D_KEY_HERMITE:
        case AUTH_3D_KEY_HOLD:
            switch (a.type) {
            case AUTH_3D_KEY_NONE:
            case AUTH_3D_KEY_STATIC:
                ident[5] = 1;
                ident[11] = 1;
                break;
            case AUTH_3D_KEY_LINEAR:
            case AUTH_3D_KEY_HERMITE:
            case AUTH_3D_KEY_HOLD:
                if (b.length == a.length) {
                    ident[5] = 1;

                    kft3* keys1 = b.keys;
                    kft3* keys2 = a.keys;
                    size_t length = b.length;
                    for (size_t i = 0; i < length; i++)
                        if (*(uint32_t*)&keys1[i].frame != *(uint32_t*)&keys2[i].frame) {
                            ident[5] = 0;
                            break;
                        }

                    if (ident[5]) {
                        ident[11] = 1;

                        for (size_t i = 0; i < length; i++)
                            if (*(uint64_t*)&keys1[i].tangent1 != *(uint64_t*)&keys2[i].tangent1) {
                                ident[11] = 0;
                                break;
                            }
                    }
                }
                else if (b.length > a.length) {
                    if (a.type == AUTH_3D_KEY_LINEAR) {
                        kft3* keys1 = b.keys;
                        kft3* keys2 = a.keys;
                        size_t keys1_length = b.length;
                        size_t keys2_length = a.length;

                        for (size_t i = 0; i < keys1_length; i++) {
                            float_t frame = keys1[i].frame;
                            bool found = false;
                            for (size_t j = 0; j < keys2_length && i < keys1_length; j++)
                                if (frame == keys2[j].frame) {
                                    found = true;
                                    break;
                                }

                            if (!found)
                                if (frame <= keys2[0].frame || frame >= keys2[keys2_length - 1].frame) {
                                    ident[5] = 1;
                                    ident[11] = 1;
                                }
                                else
                                    for (size_t j = 0; j + 1 < keys2_length && i < keys1_length; j++)
                                        if (frame >= keys2[j].frame && keys2[j + 1].frame) {
                                            if (*(uint32_t*)&keys2[j].value == *(uint32_t*)&keys2[j + 1].value) {
                                                ident[5] = 1;
                                                ident[11] = 1;
                                            }
                                            break;
                                        }
                        }
                    }
                }
                else if (b.length < a.length) {
                    if (b.type == AUTH_3D_KEY_LINEAR) {
                        kft3* keys1 = a.keys;
                        kft3* keys2 = b.keys;
                        size_t keys1_length = a.length;
                        size_t keys2_length = b.length;

                        for (size_t i = 0; i < keys1_length; i++) {
                            float_t frame = keys1[i].frame;
                            bool found = false;
                            for (size_t j = 0; j < keys2_length && i < keys1_length; j++)
                                if (frame == keys2[j].frame) {
                                    found = true;
                                    break;
                                }

                            if (!found)
                                if (frame <= keys2[0].frame || frame >= keys2[keys2_length - 1].frame) {
                                    ident[5] = 1;
                                    ident[11] = 1;
                                }
                                else
                                    for (size_t j = 0; j + 1 < keys2_length && i < keys1_length; j++)
                                        if (frame >= keys2[j].frame && keys2[j + 1].frame) {
                                            if (*(uint32_t*)&keys2[j].value == *(uint32_t*)&keys2[j + 1].value) {
                                                ident[5] = 1;
                                                ident[11] = 1;
                                            }
                                            break;
                                        }
                        }
                    }
                }
                break;
            }
            break;
        }*/
    }

    if (mlf->flags & A3DA_MATERIAL_LIST_GLOW_INTENSITY) {
        auth_3d_key_load(auth, &ml->glow_intensity, &mlf->glow_intensity);
        enum_or(ml->flags, AUTH_3D_MATERIAL_LIST_GLOW_INTENSITY);
    }

    if (mlf->flags & A3DA_MATERIAL_LIST_INCANDESCENCE) {
        auth_3d_rgba_load(auth, &ml->incandescence, &mlf->incandescence);
        enum_or(ml->flags, AUTH_3D_MATERIAL_LIST_INCANDESCENCE);

        /*auth_3d_key& r = ml->incandescence.r;
        auth_3d_key& g = ml->incandescence.g;
        auth_3d_key& b = ml->incandescence.b;
        auth_3d_key& a = ml->incandescence.a;

        switch (r.type) {
        case AUTH_3D_KEY_NONE:
        case AUTH_3D_KEY_STATIC:
            switch (g.type) {
            case AUTH_3D_KEY_NONE:
            case AUTH_3D_KEY_STATIC:
            case AUTH_3D_KEY_LINEAR:
            case AUTH_3D_KEY_HERMITE:
            case AUTH_3D_KEY_HOLD:
                ident[12] = 1;
                ident[18] = 1;
                break;
            }
            break;
        case AUTH_3D_KEY_LINEAR:
        case AUTH_3D_KEY_HERMITE:
        case AUTH_3D_KEY_HOLD:
            switch (g.type) {
            case AUTH_3D_KEY_NONE:
            case AUTH_3D_KEY_STATIC:
                ident[12] = 1;
                ident[18] = 1;
                break;
            case AUTH_3D_KEY_LINEAR:
            case AUTH_3D_KEY_HERMITE:
            case AUTH_3D_KEY_HOLD:
                if (r.length == g.length) {
                    ident[12] = 1;

                    kft3* keys1 = r.keys;
                    kft3* keys2 = g.keys;
                    size_t length = r.length;
                    for (size_t i = 0; i < length; i++)
                        if (*(uint32_t*)&keys1[i].frame != *(uint32_t*)&keys2[i].frame) {
                            ident[12] = 0;
                            break;
                        }

                    if (ident[12]) {
                        ident[18] = 1;

                        for (size_t i = 0; i < length; i++)
                            if (*(uint64_t*)&keys1[i].tangent1 != *(uint64_t*)&keys2[i].tangent1) {
                                ident[18] = 0;
                                break;
                            }
                    }
                }
                else if (b.length > a.length) {
                    if (a.type == AUTH_3D_KEY_LINEAR) {
                        kft3* keys1 = b.keys;
                        kft3* keys2 = a.keys;
                        size_t keys1_length = b.length;
                        size_t keys2_length = a.length;

                        for (size_t i = 0; i < keys1_length; i++) {
                            float_t frame = keys1[i].frame;
                            bool found = false;
                            for (size_t j = 0; j < keys2_length && i < keys1_length; j++)
                                if (frame == keys2[j].frame) {
                                    found = true;
                                    break;
                                }

                            if (!found)
                                if (frame <= keys2[0].frame || frame >= keys2[keys2_length - 1].frame) {
                                    ident[12] = 1;
                                    ident[18] = 1;
                                }
                                else
                                    for (size_t j = 0; j + 1 < keys2_length && i < keys1_length; j++)
                                        if (frame >= keys2[j].frame && keys2[j + 1].frame) {
                                            if (*(uint32_t*)&keys2[j].value == *(uint32_t*)&keys2[j + 1].value) {
                                                ident[12] = 1;
                                                ident[18] = 1;
                                            }
                                            break;
                                        }
                        }
                    }
                }
                else if (b.length < a.length) {
                    if (b.type == AUTH_3D_KEY_LINEAR) {
                        kft3* keys1 = a.keys;
                        kft3* keys2 = b.keys;
                        size_t keys1_length = a.length;
                        size_t keys2_length = b.length;

                        for (size_t i = 0; i < keys1_length; i++) {
                            float_t frame = keys1[i].frame;
                            bool found = false;
                            for (size_t j = 0; j < keys2_length && i < keys1_length; j++)
                                if (frame == keys2[j].frame) {
                                    found = true;
                                    break;
                                }

                            if (!found)
                                if (frame <= keys2[0].frame || frame >= keys2[keys2_length - 1].frame) {
                                    ident[12] = 1;
                                    ident[18] = 1;
                                }
                                else
                                    for (size_t j = 0; j + 1 < keys2_length && i < keys1_length; j++)
                                        if (frame >= keys2[j].frame && keys2[j + 1].frame) {
                                            if (*(uint32_t*)&keys2[j].value == *(uint32_t*)&keys2[j + 1].value) {
                                                ident[12] = 1;
                                                ident[18] = 1;
                                            }
                                            break;
                                        }
                        }
                    }
                }
                break;
            }
            break;
        }

        switch (r.type) {
        case AUTH_3D_KEY_NONE:
        case AUTH_3D_KEY_STATIC:
            switch (b.type) {
            case AUTH_3D_KEY_NONE:
            case AUTH_3D_KEY_STATIC:
            case AUTH_3D_KEY_LINEAR:
            case AUTH_3D_KEY_HERMITE:
            case AUTH_3D_KEY_HOLD:
                ident[13] = 1;
                ident[19] = 1;
                break;
            }
            break;
        case AUTH_3D_KEY_LINEAR:
        case AUTH_3D_KEY_HERMITE:
        case AUTH_3D_KEY_HOLD:
            switch (b.type) {
            case AUTH_3D_KEY_NONE:
            case AUTH_3D_KEY_STATIC:
                ident[13] = 1;
                ident[19] = 1;
                break;
            case AUTH_3D_KEY_LINEAR:
            case AUTH_3D_KEY_HERMITE:
            case AUTH_3D_KEY_HOLD:
                if (r.length == b.length) {
                    ident[13] = 1;

                    kft3* keys1 = r.keys;
                    kft3* keys2 = b.keys;
                    size_t length = r.length;
                    for (size_t i = 0; i < length; i++)
                        if (*(uint32_t*)&keys1[i].frame != *(uint32_t*)&keys2[i].frame) {
                            ident[13] = 0;
                            break;
                        }

                    if (ident[13]) {
                        ident[19] = 1;

                        for (size_t i = 0; i < length; i++)
                            if (*(uint64_t*)&keys1[i].tangent1 != *(uint64_t*)&keys2[i].tangent1) {
                                ident[19] = 0;
                                break;
                            }
                    }
                }
                else if (r.length > b.length) {
                    if (b.type == AUTH_3D_KEY_LINEAR) {
                        kft3* keys1 = r.keys;
                        kft3* keys2 = b.keys;
                        size_t keys1_length = r.length;
                        size_t keys2_length = b.length;

                        for (size_t i = 0; i < keys1_length; i++) {
                            float_t frame = keys1[i].frame;
                            bool found = false;
                            for (size_t j = 0; j < keys2_length && i < keys1_length; j++)
                                if (frame == keys2[j].frame) {
                                    found = true;
                                    break;
                                }

                            if (!found)
                                if (frame <= keys2[0].frame || frame >= keys2[keys2_length - 1].frame) {
                                    ident[13] = 1;
                                    ident[19] = 1;
                                }
                                else
                                    for (size_t j = 0; j + 1 < keys2_length && i < keys1_length; j++)
                                        if (frame >= keys2[j].frame && keys2[j + 1].frame) {
                                            if (*(uint32_t*)&keys2[j].value == *(uint32_t*)&keys2[j + 1].value) {
                                                ident[13] = 1;
                                                ident[19] = 1;
                                            }
                                            break;
                                        }
                        }
                    }
                }
                else if (r.length < b.length) {
                    if (r.type == AUTH_3D_KEY_LINEAR) {
                        kft3* keys1 = b.keys;
                        kft3* keys2 = r.keys;
                        size_t keys1_length = b.length;
                        size_t keys2_length = r.length;

                        for (size_t i = 0; i < keys1_length; i++) {
                            float_t frame = keys1[i].frame;
                            bool found = false;
                            for (size_t j = 0; j < keys2_length && i < keys1_length; j++)
                                if (frame == keys2[j].frame) {
                                    found = true;
                                    break;
                                }

                            if (!found)
                                if (frame <= keys2[0].frame || frame >= keys2[keys2_length - 1].frame) {
                                    ident[13] = 1;
                                    ident[19] = 1;
                                }
                                else
                                    for (size_t j = 0; j + 1 < keys2_length && i < keys1_length; j++)
                                        if (frame >= keys2[j].frame && keys2[j + 1].frame) {
                                            if (*(uint32_t*)&keys2[j].value == *(uint32_t*)&keys2[j + 1].value) {
                                                ident[13] = 1;
                                                ident[19] = 1;
                                            }
                                            break;
                                        }
                        }
                    }
                }
                break;
            }
            break;
        }

        switch (r.type) {
        case AUTH_3D_KEY_NONE:
        case AUTH_3D_KEY_STATIC:
            switch (a.type) {
            case AUTH_3D_KEY_NONE:
            case AUTH_3D_KEY_STATIC:
            case AUTH_3D_KEY_LINEAR:
            case AUTH_3D_KEY_HERMITE:
            case AUTH_3D_KEY_HOLD:
                ident[14] = 1;
                ident[20] = 1;
                break;
            }
            break;
        case AUTH_3D_KEY_LINEAR:
        case AUTH_3D_KEY_HERMITE:
        case AUTH_3D_KEY_HOLD:
            switch (a.type) {
            case AUTH_3D_KEY_NONE:
            case AUTH_3D_KEY_STATIC:
                ident[14] = 1;
                ident[20] = 1;
                break;
            case AUTH_3D_KEY_LINEAR:
            case AUTH_3D_KEY_HERMITE:
            case AUTH_3D_KEY_HOLD:
                if (r.length == a.length) {
                    ident[14] = 1;

                    kft3* keys1 = r.keys;
                    kft3* keys2 = a.keys;
                    size_t length = r.length;
                    for (size_t i = 0; i < length; i++)
                        if (*(uint32_t*)&keys1[i].frame != *(uint32_t*)&keys2[i].frame) {
                            ident[14] = 0;
                            break;
                        }

                    if (ident[14]) {
                        ident[20] = 1;

                        for (size_t i = 0; i < length; i++)
                            if (*(uint64_t*)&keys1[i].tangent1 != *(uint64_t*)&keys2[i].tangent1) {
                                ident[20] = 0;
                                break;
                            }
                    }
                }
                else if (r.length > a.length) {
                    if (a.type == AUTH_3D_KEY_LINEAR) {
                        kft3* keys1 = r.keys;
                        kft3* keys2 = a.keys;
                        size_t keys1_length = r.length;
                        size_t keys2_length = a.length;

                        for (size_t i = 0; i < keys1_length; i++) {
                            float_t frame = keys1[i].frame;
                            bool found = false;
                            for (size_t j = 0; j < keys2_length && i < keys1_length; j++)
                                if (frame == keys2[j].frame) {
                                    found = true;
                                    break;
                                }

                            if (!found)
                                if (frame <= keys2[0].frame || frame >= keys2[keys2_length - 1].frame) {
                                    ident[14] = 1;
                                    ident[20] = 1;
                                }
                                else
                                    for (size_t j = 0; j + 1 < keys2_length && i < keys1_length; j++)
                                        if (frame >= keys2[j].frame && keys2[j + 1].frame) {
                                            if (*(uint32_t*)&keys2[j].value == *(uint32_t*)&keys2[j + 1].value) {
                                                ident[14] = 1;
                                                ident[20] = 1;
                                            }
                                            break;
                                        }
                        }
                    }
                }
                else if (r.length < a.length) {
                    if (b.type == AUTH_3D_KEY_LINEAR) {
                        kft3* keys1 = a.keys;
                        kft3* keys2 = r.keys;
                        size_t keys1_length = a.length;
                        size_t keys2_length = r.length;

                        for (size_t i = 0; i < keys1_length; i++) {
                            float_t frame = keys1[i].frame;
                            bool found = false;
                            for (size_t j = 0; j < keys2_length && i < keys1_length; j++)
                                if (frame == keys2[j].frame) {
                                    found = true;
                                    break;
                                }

                            if (!found)
                                if (frame <= keys2[0].frame || frame >= keys2[keys2_length - 1].frame) {
                                    ident[14] = 1;
                                    ident[20] = 1;
                                }
                                else
                                    for (size_t j = 0; j + 1 < keys2_length && i < keys1_length; j++)
                                        if (frame >= keys2[j].frame && keys2[j + 1].frame) {
                                            if (*(uint32_t*)&keys2[j].value == *(uint32_t*)&keys2[j + 1].value) {
                                                ident[14] = 1;
                                                ident[20] = 1;
                                            }
                                            break;
                                        }
                        }
                    }
                }
                break;
            }
            break;
        }

        switch (g.type) {
        case AUTH_3D_KEY_NONE:
        case AUTH_3D_KEY_STATIC:
            switch (b.type) {
            case AUTH_3D_KEY_NONE:
            case AUTH_3D_KEY_STATIC:
                ident[15] = 1;
                ident[21] = 1;
                break;
            }
            break;
        case AUTH_3D_KEY_LINEAR:
        case AUTH_3D_KEY_HERMITE:
        case AUTH_3D_KEY_HOLD:
            switch (b.type) {
            case AUTH_3D_KEY_NONE:
            case AUTH_3D_KEY_STATIC:
                ident[15] = 1;
                ident[21] = 1;
                break;
            case AUTH_3D_KEY_LINEAR:
            case AUTH_3D_KEY_HERMITE:
            case AUTH_3D_KEY_HOLD:
                if (g.length == b.length) {
                    ident[15] = 1;

                    kft3* keys1 = g.keys;
                    kft3* keys2 = b.keys;
                    size_t length = g.length;
                    for (size_t i = 0; i < length; i++)
                        if (*(uint32_t*)&keys1[i].frame != *(uint32_t*)&keys2[i].frame) {
                            ident[15] = 0;
                            break;
                        }

                    if (ident[15]) {
                        ident[21] = 1;

                        for (size_t i = 0; i < length; i++)
                            if (*(uint64_t*)&keys1[i].tangent1 != *(uint64_t*)&keys2[i].tangent1) {
                                ident[21] = 0;
                                break;
                            }
                    }
                }
                else if (g.length > b.length) {
                    if (b.type == AUTH_3D_KEY_LINEAR) {
                        kft3* keys1 = g.keys;
                        kft3* keys2 = b.keys;
                        size_t keys1_length = g.length;
                        size_t keys2_length = b.length;

                        for (size_t i = 0; i < keys1_length; i++) {
                            float_t frame = keys1[i].frame;
                            bool found = false;
                            for (size_t j = 0; j < keys2_length && i < keys1_length; j++)
                                if (frame == keys2[j].frame) {
                                    found = true;
                                    break;
                                }

                            if (!found)
                                if (frame <= keys2[0].frame || frame >= keys2[keys2_length - 1].frame) {
                                    ident[15] = 1;
                                    ident[21] = 1;
                                }
                                else
                                    for (size_t j = 0; j + 1 < keys2_length && i < keys1_length; j++)
                                        if (frame >= keys2[j].frame && keys2[j + 1].frame) {
                                            if (*(uint32_t*)&keys2[j].value == *(uint32_t*)&keys2[j + 1].value) {
                                                ident[15] = 1;
                                                ident[21] = 1;
                                            }
                                            break;
                                        }
                        }
                    }
                }
                else if (g.length < b.length) {
                    if (g.type == AUTH_3D_KEY_LINEAR) {
                        kft3* keys1 = b.keys;
                        kft3* keys2 = g.keys;
                        size_t keys1_length = b.length;
                        size_t keys2_length = g.length;

                        for (size_t i = 0; i < keys1_length; i++) {
                            float_t frame = keys1[i].frame;
                            bool found = false;
                            for (size_t j = 0; j < keys2_length && i < keys1_length; j++)
                                if (frame == keys2[j].frame) {
                                    found = true;
                                    break;
                                }

                            if (!found)
                                if (frame <= keys2[0].frame || frame >= keys2[keys2_length - 1].frame) {
                                    ident[15] = 1;
                                    ident[21] = 1;
                                }
                                else
                                    for (size_t j = 0; j + 1 < keys2_length && i < keys1_length; j++)
                                        if (frame >= keys2[j].frame && keys2[j + 1].frame) {
                                            if (*(uint32_t*)&keys2[j].value == *(uint32_t*)&keys2[j + 1].value) {
                                                ident[15] = 1;
                                                ident[21] = 1;
                                            }
                                            break;
                                        }
                        }
                    }
                }
                break;
            }
            break;
        }

        switch (g.type) {
        case AUTH_3D_KEY_NONE:
        case AUTH_3D_KEY_STATIC:
            switch (a.type) {
            case AUTH_3D_KEY_NONE:
            case AUTH_3D_KEY_STATIC:
                ident[16] = 1;
                ident[22] = 1;
                break;
            }
            break;
        case AUTH_3D_KEY_LINEAR:
        case AUTH_3D_KEY_HERMITE:
        case AUTH_3D_KEY_HOLD:
            switch (a.type) {
            case AUTH_3D_KEY_NONE:
            case AUTH_3D_KEY_STATIC:
                ident[16] = 1;
                ident[22] = 1;
                break;
            case AUTH_3D_KEY_LINEAR:
            case AUTH_3D_KEY_HERMITE:
            case AUTH_3D_KEY_HOLD:
                if (g.length == a.length) {
                    ident[16] = 1;

                    kft3* keys1 = g.keys;
                    kft3* keys2 = a.keys;
                    size_t length = g.length;
                    for (size_t i = 0; i < length; i++)
                        if (*(uint32_t*)&keys1[i].frame != *(uint32_t*)&keys2[i].frame) {
                            ident[16] = 0;
                            break;
                        }

                    if (ident[16]) {
                        ident[22] = 1;

                        for (size_t i = 0; i < length; i++)
                            if (*(uint64_t*)&keys1[i].tangent1 != *(uint64_t*)&keys2[i].tangent1) {
                                ident[22] = 0;
                                break;
                            }
                    }
                }
                else if (g.length > a.length) {
                    if (a.type == AUTH_3D_KEY_LINEAR) {
                        kft3* keys1 = g.keys;
                        kft3* keys2 = a.keys;
                        size_t keys1_length = g.length;
                        size_t keys2_length = a.length;

                        for (size_t i = 0; i < keys1_length; i++) {
                            float_t frame = keys1[i].frame;
                            bool found = false;
                            for (size_t j = 0; j < keys2_length && i < keys1_length; j++)
                                if (frame == keys2[j].frame) {
                                    found = true;
                                    break;
                                }

                            if (!found)
                                if (frame <= keys2[0].frame || frame >= keys2[keys2_length - 1].frame) {
                                    ident[16] = 1;
                                    ident[22] = 1;
                                }
                                else
                                    for (size_t j = 0; j + 1 < keys2_length && i < keys1_length; j++)
                                        if (frame >= keys2[j].frame && keys2[j + 1].frame) {
                                            if (*(uint32_t*)&keys2[j].value == *(uint32_t*)&keys2[j + 1].value) {
                                                ident[16] = 1;
                                                ident[22] = 1;
                                            }
                                            break;
                                        }
                        }
                    }
                }
                else if (g.length < a.length) {
                    if (g.type == AUTH_3D_KEY_LINEAR) {
                        kft3* keys1 = a.keys;
                        kft3* keys2 = g.keys;
                        size_t keys1_length = a.length;
                        size_t keys2_length = g.length;

                        for (size_t i = 0; i < keys1_length; i++) {
                            float_t frame = keys1[i].frame;
                            bool found = false;
                            for (size_t j = 0; j < keys2_length && i < keys1_length; j++)
                                if (frame == keys2[j].frame) {
                                    found = true;
                                    break;
                                }

                            if (!found)
                                if (frame <= keys2[0].frame || frame >= keys2[keys2_length - 1].frame) {
                                    ident[16] = 1;
                                    ident[22] = 1;
                                }
                                else
                                    for (size_t j = 0; j + 1 < keys2_length && i < keys1_length; j++)
                                        if (frame >= keys2[j].frame && keys2[j + 1].frame) {
                                            if (*(uint32_t*)&keys2[j].value == *(uint32_t*)&keys2[j + 1].value) {
                                                ident[16] = 1;
                                                ident[22] = 1;
                                            }
                                            break;
                                        }
                        }
                    }
                }
                break;
            }
            break;
        }

        switch (b.type) {
        case AUTH_3D_KEY_NONE:
        case AUTH_3D_KEY_STATIC:
            switch (a.type) {
            case AUTH_3D_KEY_NONE:
            case AUTH_3D_KEY_STATIC:
            case AUTH_3D_KEY_LINEAR:
            case AUTH_3D_KEY_HERMITE:
            case AUTH_3D_KEY_HOLD:
                ident[17] = 1;
                ident[23] = 1;
                break;
            }
            break;
        case AUTH_3D_KEY_LINEAR:
        case AUTH_3D_KEY_HERMITE:
        case AUTH_3D_KEY_HOLD:
            switch (a.type) {
            case AUTH_3D_KEY_NONE:
            case AUTH_3D_KEY_STATIC:
                ident[17] = 1;
                ident[23] = 1;
                break;
            case AUTH_3D_KEY_LINEAR:
            case AUTH_3D_KEY_HERMITE:
            case AUTH_3D_KEY_HOLD:
                if (b.length == a.length) {
                    ident[17] = 1;

                    kft3* keys1 = b.keys;
                    kft3* keys2 = a.keys;
                    size_t length = b.length;
                    for (size_t i = 0; i < length; i++)
                        if (*(uint32_t*)&keys1[i].frame != *(uint32_t*)&keys2[i].frame) {
                            ident[17] = 0;
                            break;
                        }

                    if (ident[17]) {
                        ident[23] = 1;

                        for (size_t i = 0; i < length; i++)
                            if (*(uint64_t*)&keys1[i].tangent1 != *(uint64_t*)&keys2[i].tangent1) {
                                ident[23] = 0;
                                break;
                            }
                    }
                }
                else if (b.length > a.length) {
                    if (a.type == AUTH_3D_KEY_LINEAR) {
                        kft3* keys1 = b.keys;
                        kft3* keys2 = a.keys;
                        size_t keys1_length = b.length;
                        size_t keys2_length = a.length;

                        for (size_t i = 0; i < keys1_length; i++) {
                            float_t frame = keys1[i].frame;
                            bool found = false;
                            for (size_t j = 0; j < keys2_length && i < keys1_length; j++)
                                if (frame == keys2[j].frame) {
                                    found = true;
                                    break;
                                }

                            if (!found)
                                if (frame <= keys2[0].frame || frame >= keys2[keys2_length - 1].frame) {
                                    ident[17] = 1;
                                    ident[23] = 1;
                                }
                                else
                                    for (size_t j = 0; j + 1 < keys2_length && i < keys1_length; j++)
                                        if (frame >= keys2[j].frame && keys2[j + 1].frame) {
                                            if (*(uint32_t*)&keys2[j].value == *(uint32_t*)&keys2[j + 1].value) {
                                                ident[17] = 1;
                                                ident[23] = 1;
                                            }
                                            break;
                                        }
                        }
                    }
                }
                else if (b.length < a.length) {
                    if (b.type == AUTH_3D_KEY_LINEAR) {
                        kft3* keys1 = a.keys;
                        kft3* keys2 = b.keys;
                        size_t keys1_length = a.length;
                        size_t keys2_length = b.length;

                        for (size_t i = 0; i < keys1_length; i++) {
                            float_t frame = keys1[i].frame;
                            bool found = false;
                            for (size_t j = 0; j < keys2_length && i < keys1_length; j++)
                                if (frame == keys2[j].frame) {
                                    found = true;
                                    break;
                                }

                            if (!found)
                                if (frame <= keys2[0].frame || frame >= keys2[keys2_length - 1].frame) {
                                    ident[17] = 1;
                                    ident[23] = 1;
                                }
                                else
                                    for (size_t j = 0; j + 1 < keys2_length && i < keys1_length; j++)
                                        if (frame >= keys2[j].frame && keys2[j + 1].frame) {
                                            if (*(uint32_t*)&keys2[j].value == *(uint32_t*)&keys2[j + 1].value) {
                                                ident[17] = 1;
                                                ident[23] = 1;
                                            }
                                            break;
                                        }
                        }
                    }
                }
                break;
            }
            break;
        }*/
    }

    /*OutputDebugStringA(auth->file_name.c_str());
    OutputDebugStringA("\n");
    OutputDebugStringA(mlf->name.c_str());
    OutputDebugStringA("\n");

    if (ml->flags & AUTH_3D_MATERIAL_LIST_BLEND_COLOR) {
        if ((ident[0] || ident[1] || ident[2] || ident[4] || ident[5] || ident[6])
            && (!ident[0] || !ident[1] || !ident[2] || !ident[4] || !ident[5] || !ident[6]))
            OutputDebugStringA("A\n");
        if (!(ident[0] || ident[1] || ident[2] || ident[3] || ident[4] || ident[5]))
            OutputDebugStringA("B\n");
        if (ident[0] || ident[1] || ident[2] || ident[3] || ident[4] || ident[5]) {
            if ((ident[6] || ident[7] || ident[8] || ident[9] || ident[10] || ident[11])
                && (!ident[6] || !ident[7] || !ident[8] || !ident[9] || !ident[10] || !ident[11]))
                OutputDebugStringA("C\n");
            if (!(ident[6] || ident[7] || ident[8] || ident[9] || ident[10] || ident[11]))
                OutputDebugStringA("D\n");
        }
    }

    if (ml->flags & AUTH_3D_MATERIAL_LIST_INCANDESCENCE) {
        if ((ident[12] || ident[13] || ident[14] || ident[15] || ident[16] || ident[17])
            && (!ident[12] || !ident[13] || !ident[14] || !ident[15] || !ident[16] || !ident[17]))
            OutputDebugStringA("E\n");
        if (!(ident[12] || ident[13] || ident[14] || ident[15] || ident[16] || ident[17]))
            OutputDebugStringA("F\n");
        if (ident[12] || ident[13] || ident[14] || ident[15] || ident[16] || ident[17]) {
            if ((ident[18] || ident[19] || ident[20] || ident[21] || ident[22] || ident[23])
                && (!ident[18] || !ident[19] || !ident[20] || !ident[21] || !ident[22] || !ident[23]))
                OutputDebugStringA("G\n");
            if (!(ident[18] || ident[19] || ident[20] || ident[21] || ident[22] || ident[23]))
                OutputDebugStringA("H\n");
        }
    }*/

    ml->name.assign(mlf->name);
    ml->hash = hash_string_murmurhash(ml->name);

    /*char buf[0x200];
    sprintf_s(buf, sizeof(buf), "\n%d %d %d %d %d %d %d %d %d %d %d %d\n"
        "%d %d %d %d %d %d %d %d %d %d %d %d\n\n",
        ident[0], ident[1], ident[2],
        ident[3], ident[4], ident[5],
        ident[6], ident[7], ident[8],
        ident[9], ident[10], ident[11],
        ident[12], ident[13], ident[14],
        ident[15], ident[16], ident[17],
        ident[18], ident[19], ident[20],
        ident[21], ident[22], ident[23]);
    OutputDebugStringA(buf);*/
}

static void auth_3d_object_disp(auth_3d_object* o, auth_3d* auth, render_context* rctx) {
    if (!o->model_transform.visible)
        return;

    if (!auth->visible) {
        for (auth_3d_object*& i : o->children_object)
            auth_3d_object_disp(i, auth, rctx);
        for (auth_3d_object_hrc*& i : o->children_object_hrc)
            auth_3d_object_hrc_disp(i, auth, rctx);
        return;
    }

    mat4 mat = o->model_transform.mat;

    object_data* object_data = &rctx->object_data;
    object_database* obj_db = auth->obj_db;
    texture_database* tex_db = auth->tex_db;

    if (auth->chara_id >= 0 && auth->chara_id < ROB_CHARA_COUNT)
        if (rob_chara_pv_data_array_check_chara_id(auth->chara_id)) {
            rob_chara* rob_chr = rob_chara_array_get(auth->chara_id);
            mat4 m;
            mat4_mult(&rob_chr->data.miku_rot.field_6C,
                auth->chara_item
                    ? &rob_chr->data.adjust_data.item_mat
                    : &rob_chr->data.adjust_data.mat, &m);
            mat4_mult(&m, &mat, &mat);
            object_data->set_shadow_type(auth->chara_id ? SHADOW_STAGE : SHADOW_CHARA);
        }

    draw_task_flags flags = (draw_task_flags)0;
    if (auth->shadow)
        enum_or(flags, DRAW_TASK_4 | DRAW_TASK_SHADOW);
    if (o->reflect)
        enum_or(flags, DRAW_TASK_NO_TRANSLUCENCY | DRAW_TASK_REFLECT);
    if (o->refract)
        enum_or(flags, DRAW_TASK_NO_TRANSLUCENCY | DRAW_TASK_REFRACT);

    object_data->set_draw_task_flags((draw_task_flags)flags);

    char buf[0x80];
    int32_t tex_pat_count = 0;
    texture_pattern_struct tex_pat[TEXTURE_PATTERN_COUNT];

    for (auth_3d_object_texture_pattern& i : o->texture_pattern) {
        if (!i.pattern.curve || i.name.size() <= 3)
            continue;
        else if (tex_pat_count >= TEXTURE_PATTERN_COUNT)
            break;

        float_t pat = i.pattern.value;
        if (pat > 0.0f)
            pat = (float_t)(int32_t)(pat + 0.5f);
        else if (pat < 0.0f)
            pat = (float_t)(int32_t)(pat - 0.5f);

        sprintf_s(buf, sizeof(buf), "%.*s%03d",
            (int32_t)(i.name.size() - 3), i.name.c_str(), (int32_t)pat);

        tex_pat[tex_pat_count].src = texture_id(0, i.texture_id);
        tex_pat[tex_pat_count].dst = texture_id(0, tex_db->get_texture_id(buf));
        tex_pat_count++;
    }

    if (tex_pat_count)
        object_data->set_texture_pattern(tex_pat_count, tex_pat);

    int32_t tex_trans_count = 0;
    texture_transform_struct tex_trans[TEXTURE_TRANSFORM_COUNT];

    for (auth_3d_object_texture_transform& i : o->texture_transform) {
        if (tex_trans_count >= TEXTURE_TRANSFORM_COUNT)
            break;

        tex_trans[tex_trans_count].id = i.texture_id;
        tex_trans[tex_trans_count].mat = i.mat;
        tex_trans_count++;
    }

    if (tex_trans_count)
        object_data->set_texture_transform(tex_trans_count, tex_trans);

    const char* uid_name = o->uid_name.c_str();
    int32_t uid_name_length = (int32_t)o->uid_name.size();

    if (uid_name_length <= 3)
        draw_task_add_draw_object_by_object_info(rctx, &mat, o->object_info);
    else if (o->morph.curve) {
        float_t morph = o->morph.value;
        int32_t morph_int = (int32_t)morph;
        morph = fmodf(morph, 1.0f);
        if (morph > 0.0f && morph < 1.0f) {
            sprintf_s(buf, sizeof(buf), "%.*s%03d", uid_name_length - 3, uid_name, morph_int + 1);
            object_info morph_obj_info = obj_db->get_object_info(buf);
            if (morph_obj_info.is_null())
                morph_obj_info = o->object_info;
            object_data->set_morph(morph_obj_info, morph);

            sprintf_s(buf, sizeof(buf), "%.*s%03d", uid_name_length - 3, uid_name, morph_int);
            object_info obj_info = obj_db->get_object_info(buf);
            draw_task_add_draw_object_by_object_info(rctx, &mat, obj_info);
            object_data->set_morph({}, 0.0f);
        }
        else {
            if (morph >= 1.0f)
                morph_int++;

            sprintf_s(buf, sizeof(buf), "%.*s%03d", uid_name_length - 3, uid_name, morph_int);
            object_info obj_info = obj_db->get_object_info(buf);
            if (obj_info.is_null())
                obj_info = o->object_info;
            draw_task_add_draw_object_by_object_info(rctx, &mat, obj_info);
        }
    }
    else if (o->pattern.curve) {
        float_t pat = o->pattern.value;
        if (pat > 0.0f)
            pat = (float_t)(int32_t)(pat + 0.5f);
        else if (pat < 0.0f)
            pat = (float_t)(int32_t)(pat - 0.5f);

        sprintf_s(buf, sizeof(buf), "%.*s%03d", uid_name_length - 3, uid_name, (int32_t)pat);
        object_info obj_info = obj_db->get_object_info(buf);
        draw_task_add_draw_object_by_object_info(rctx, &mat, obj_info);
    }
    else
        draw_task_add_draw_object_by_object_info(rctx, &mat, o->object_info);

    object_data->set_texture_transform();
    object_data->set_texture_pattern();
    object_data->set_draw_task_flags();

    for (auth_3d_object*& i : o->children_object)
        auth_3d_object_disp(i, auth, rctx);
    for (auth_3d_object_hrc*& i : o->children_object_hrc)
        auth_3d_object_hrc_disp(i, auth, rctx);
}

static void auth_3d_object_load(auth_3d* auth, auth_3d_object* o,
    auth_3d_object_file* of, object_database* obj_db, texture_database* tex_db) {
    auth_3d_model_transform_load(auth, &o->model_transform, &of->model_transform);
    auth_3d_object_curve_load(auth, &o->morph, of->morph, of->morph_offset);
    o->name.assign(of->name);
    o->parent_name.assign(of->parent_name);
    o->parent_node.assign(of->parent_node);
    auth_3d_object_curve_load(auth, &o->pattern, of->pattern, of->pattern_offset);

    size_t texture_pattern_index = 0;
    o->texture_pattern.resize(of->texture_pattern.size());
    for (auth_3d_object_texture_pattern_file& i : of->texture_pattern)
        auth_3d_object_texture_pattern_load(auth,
            &o->texture_pattern[texture_pattern_index++], &i, tex_db);

    size_t texture_transform_index = 0;
    o->texture_transform.resize(of->texture_transform.size());
    for (auth_3d_object_texture_transform_file& i : of->texture_transform)
        auth_3d_object_texture_transform_load(auth,
            &o->texture_transform[texture_transform_index++], &i, tex_db);

    o->uid_name.assign(of->uid_name);

    o->reflect = strstr(o->uid_name.c_str(), "_REFLECT") ? true : false;
    o->refract = strstr(o->uid_name.c_str(), "_REFRACT") ? true : false;

    o->object_info = obj_db->get_object_info(o->uid_name.c_str());
    o->object_hash = hash_string_murmurhash(o->uid_name);
}

static void auth_3d_object_curve_load(auth_3d* auth, auth_3d_object_curve* oc,
    std::string& name, float_t frame_offset) {
    oc->curve = 0;
    oc->name.assign(name);
    oc->frame_offset = 0.0f;
    oc->value = 0.0f;

    for (auth_3d_curve& i : auth->curve)
        if (!oc->name.compare(i.name)) {
            oc->curve = &i;
            oc->frame_offset = frame_offset;
            break;
        }
}

static void auth_3d_object_hrc_disp(auth_3d_object_hrc* oh, auth_3d* auth, render_context* rctx) {
    if (!auth->visible || !oh->node.front().model_transform.visible)
        return;

    object_data* object_data = &rctx->object_data;
    object_database* obj_db = auth->obj_db;
    texture_database* tex_db = auth->tex_db;

    draw_task_flags flags = DRAW_TASK_SSS;
    if (auth->shadow | oh->shadow)
        enum_or(flags, DRAW_TASK_4 | DRAW_TASK_SHADOW);
    if (oh->reflect)
        enum_or(flags, DRAW_TASK_NO_TRANSLUCENCY | DRAW_TASK_REFLECT);
    if (oh->refract)
        enum_or(flags, DRAW_TASK_NO_TRANSLUCENCY | DRAW_TASK_REFRACT);
    if (auth->alpha < 1.0f)
        enum_or(flags, auth->draw_task_flags);

    object_data->set_draw_task_flags(flags);
    object_data->set_shadow_type(SHADOW_CHARA);

    mat4 mat = mat4_identity;
    if (auth->chara_id >= 0 && auth->chara_id < ROB_CHARA_COUNT) {
        if (rob_chara_pv_data_array_check_chara_id(auth->chara_id)) {
            rob_chara* rob_chr = rob_chara_array_get(auth->chara_id);
            mat4_mult(&rob_chr->data.miku_rot.field_6C,
                auth->chara_item
                    ? &rob_chr->data.adjust_data.item_mat
                    : &rob_chr->data.adjust_data.mat, &mat);
            if (auth->chara_id)
                object_data->set_shadow_type(SHADOW_STAGE);
        }
    }
    else if (flags & DRAW_TASK_SHADOW) {
        object_data->set_shadow_type(SHADOW_STAGE);

        mat4* m = &oh->node.front().model_transform.mat;
        for (auth_3d_object_node& i : oh->node)
            if (i.mat) {
                m = i.mat;
                break;
            }

        shadow* shad = rctx->draw_pass.shadow_ptr;
        if (shad) {
            vec3 pos = *(vec3*)&m->row3;
            pos.y -= 0.2f;
            shad->field_1D0[SHADOW_STAGE].push_back(pos);
        }
    }

    if (oh->mats.size())
        draw_task_add_draw_object_by_object_info_object_skin(rctx,
            oh->object_info, 0, 0, auth->alpha, oh->mats.data(), 0, 0, &mat);

    object_data->set_draw_task_flags();
    object_data->set_shadow_type(SHADOW_CHARA);

    for (auth_3d_object*& i : oh->children_object)
        auth_3d_object_disp(i, auth, rctx);

    for (auth_3d_object_hrc*& i : oh->children_object_hrc)
        auth_3d_object_hrc_disp(i, auth, rctx);
}

static void auth_3d_object_hrc_load(auth_3d* auth, auth_3d_object_hrc* oh,
    auth_3d_object_hrc_file* ohf, object_database* obj_db) {
    oh->name.assign(ohf->name);

    size_t node_index = 0;
    oh->node.resize(ohf->node.size());
    for (auth_3d_object_node_file& i : ohf->node)
        auth_3d_object_node_load(auth,
            &oh->node[node_index++], &i);

    oh->parent_name.assign(ohf->parent_name);
    oh->parent_node.assign(ohf->parent_node);
    oh->shadow = ohf->shadow;
    oh->uid_name.assign(ohf->uid_name);

    oh->reflect = strstr(oh->uid_name.c_str(), "_REFLECT") ? true : false;
    oh->refract = strstr(oh->uid_name.c_str(), "_REFRACT") ? true : false;

    oh->object_info = obj_db->get_object_info(oh->uid_name.c_str());
    oh->object_hash = hash_string_murmurhash(oh->uid_name);

    obj_skin* skin = object_storage_get_obj_skin(oh->object_info);
    if (!skin)
        return;

    oh->mats.resize(skin->num_bone);
    mat4* mats = oh->mats.data();

    for (auth_3d_object_node& i : oh->node) {
        i.bone_id = -1;
        uint64_t name_hash = hash_string_fnv1a64m(i.name);
        for (uint32_t j = 0; j < skin->num_bone; j++)
            if (hash_utf8_fnv1a64m(skin->bone_array[j].name) == name_hash) {
                i.bone_id = skin->bone_array[j].id;
                break;
            }

        if (i.bone_id > -1 && !(i.bone_id & 0x8000))
            i.mat = &mats[i.bone_id];
    }
}

static bool auth_3d_object_hrc_replace_chara(auth_3d_object_hrc* oh,
    chara_index src_chara, chara_index dst_chara, object_database* obj_db) {
    if (src_chara < CHARA_MIKU || src_chara > CHARA_TETO || dst_chara < CHARA_MIKU || dst_chara > CHARA_TETO || src_chara == dst_chara)
        return false;

    std::string src_chara_str = chara_index_get_auth_3d_name(src_chara);
    std::string dst_chara_str = chara_index_get_auth_3d_name(dst_chara);

    std::string uid_name = oh->uid_name;
    size_t pos = uid_name.find(src_chara_str);
    if (pos != -1)
        uid_name.assign(uid_name.replace(pos, src_chara_str.size(), dst_chara_str));

    object_info obj_info = obj_db->get_object_info(uid_name.c_str());
    if (obj_info.not_null()) {
        oh->object_info = obj_info;
        return true;
    }
    return false;
}

static void auth_3d_object_hrc_nodes_mat_mult(auth_3d_object_hrc* oh, const mat4* mat) {
    for (auth_3d_object_node& i : oh->node) {
        mat4 m;
        if (i.parent >= 0)
            m = oh->node[i.parent].model_transform.mat;
        else
            m = *mat;

        mat4_mult(&i.joint_orient_mat, &m, &m);
        i.model_transform.mat_mult(&m);

        if (i.mat)
            *i.mat = i.model_transform.mat;
    }
}

static void auth_3d_object_instance_load(auth_3d* auth, auth_3d_object_instance* oi,
    auth_3d_object_instance_file* oif, auth_3d_m_object_hrc* moh, object_database* obj_db) {
    auth_3d_object_model_transform_load(auth, &oi->model_transform, &oif->model_transform);
    oi->name.assign(oif->name);
    oi->shadow = oif->shadow;
    oi->uid_name.assign(oif->uid_name);

    oi->object_info = obj_db->get_object_info(oi->uid_name.c_str());
    oi->object_hash = hash_string_murmurhash(oi->uid_name);

    obj_skin* skin = object_storage_get_obj_skin(oi->object_info);
    if (!skin)
        return;

    oi->object_bone_indices.resize(skin->num_bone);
    int32_t* object_bone_indices = oi->object_bone_indices.data();

    oi->mats.resize(skin->num_bone);
    mat4* mats = oi->mats.data();

    for (uint32_t i = 0; i < skin->num_bone; i++) {
        uint64_t name_hash = hash_utf8_fnv1a64m(skin->bone_array[i].name);
        for (auth_3d_object_node& j : moh->node)
            if (hash_string_fnv1a64m(j.name) == name_hash) {
                int32_t bone_id = skin->bone_array[i].id;
                if (!(bone_id & 0x8000))
                    object_bone_indices[bone_id] = (int32_t)(&j - moh->node.data());
                break;
            }
    }
}

static void auth_3d_object_model_transform_load(auth_3d* auth,
    auth_3d_object_model_transform* omt, auth_3d_model_transform_file* mtf) {
    omt->mat = mat4_identity;
    omt->mat_inner = mat4_identity;
    omt->mat_rot = mat4_identity;
    omt->translation_value = 0.0f;
    omt->rotation_value = 0.0f;
    omt->scale_value = 1.0f;
    omt->visible = false;
    omt->frame = -1.0f;
    omt->has_rotation = false;
    omt->has_translation = false;
    omt->has_scale = false;
    omt->has_visibility = false;

    auth_3d_vec3_load(auth, &omt->translation, &mtf->translation);
    auth_3d_vec3_load(auth, &omt->rotation, &mtf->rotation);
    auth_3d_vec3_load(auth, &omt->scale, &mtf->scale);
    auth_3d_key_load(auth, &omt->visibility, &mtf->visibility);

    if (omt->scale.x.type == AUTH_3D_KEY_STATIC && fabsf(1.0f - omt->scale.x.value) <= 0.000001f
        && omt->scale.y.type == AUTH_3D_KEY_STATIC && fabsf(1.0f - omt->scale.y.value) <= 0.000001f
        && omt->scale.z.type == AUTH_3D_KEY_STATIC && fabsf(1.0f - omt->scale.z.value) <= 0.000001f) {
        omt->has_scale = false;
        omt->scale_value = 1.0f;
    }
    else
        omt->has_scale = true;

    if (omt->visibility.type == AUTH_3D_KEY_STATIC) {
        omt->has_visibility = false;
        omt->visible = omt->visibility.value >= 0.99900001f;
    }
    else
        omt->has_visibility = true;
}

void auth_3d_object_model_transform::mat_mult(const mat4* mat) {
    mat4_mult(&this->mat_inner, mat, &this->mat);
}

static void auth_3d_object_node_load(auth_3d* auth,
    auth_3d_object_node* on, auth_3d_object_node_file* onf) {
    if (onf->flags & A3DA_OBJECT_NODE_JOINT_ORIENT) {
        on->joint_orient = onf->joint_orient;
        mat4_rotate(&on->joint_orient, &on->joint_orient_mat);
        enum_or(on->flags, AUTH_3D_OBJECT_NODE_JOINT_ORIENT);
    }
    else {
        on->joint_orient = 0.0f;
        on->joint_orient_mat = mat4_identity;
    }

    auth_3d_object_model_transform_load(auth, &on->model_transform, &onf->model_transform);
    on->name.assign(onf->name);
    on->parent = onf->parent;
}

static void auth_3d_object_texture_pattern_load(auth_3d* auth, auth_3d_object_texture_pattern* otp,
    auth_3d_object_texture_pattern_file* otpf, texture_database* tex_db) {
    otp->name.assign(otpf->name);
    auth_3d_object_curve_load(auth, &otp->pattern, otpf->pattern, otpf->pattern_offset);
    otp->texture_id = tex_db->get_texture_id(otp->name.c_str());
}

static void auth_3d_object_texture_transform_load(auth_3d* auth, auth_3d_object_texture_transform* ott,
    auth_3d_object_texture_transform_file* ottf, texture_database* tex_db) {
    if (ottf->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_COVERAGE_U) {
        auth_3d_key_load(auth, &ott->coverage_u, &ottf->coverage_u);
        enum_or(ott->flags, AUTH_3D_OBJECT_TEXTURE_TRANSFORM_COVERAGE_U);
    }

    if (ottf->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_COVERAGE_V) {
        auth_3d_key_load(auth, &ott->coverage_v, &ottf->coverage_v);
        enum_or(ott->flags, AUTH_3D_OBJECT_TEXTURE_TRANSFORM_COVERAGE_V);
    }

    ott->name.assign(ottf->name);

    if (ottf->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_OFFSET_U) {
        auth_3d_key_load(auth, &ott->offset_u, &ottf->offset_u);
        enum_or(ott->flags, AUTH_3D_OBJECT_TEXTURE_TRANSFORM_OFFSET_U);
    }

    if (ottf->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_OFFSET_V) {
        auth_3d_key_load(auth, &ott->offset_v, &ottf->offset_v);
        enum_or(ott->flags, AUTH_3D_OBJECT_TEXTURE_TRANSFORM_OFFSET_V);
    }

    if (ottf->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_REPEAT_U) {
        auth_3d_key_load(auth, &ott->repeat_u, &ottf->repeat_u);
        enum_or(ott->flags, AUTH_3D_OBJECT_TEXTURE_TRANSFORM_REPEAT_U);
    }

    if (ottf->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_REPEAT_V) {
        auth_3d_key_load(auth, &ott->repeat_v, &ottf->repeat_v);
        enum_or(ott->flags, AUTH_3D_OBJECT_TEXTURE_TRANSFORM_REPEAT_V);
    }

    if (ottf->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_ROTATE) {
        auth_3d_key_load(auth, &ott->rotate, &ottf->rotate);
        enum_or(ott->flags, AUTH_3D_OBJECT_TEXTURE_TRANSFORM_ROTATE);
    }

    if (ottf->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_ROTATE_FRAME) {
        auth_3d_key_load(auth, &ott->rotate_frame, &ottf->rotate_frame);
        enum_or(ott->flags, AUTH_3D_OBJECT_TEXTURE_TRANSFORM_ROTATE_FRAME);
    }

    if (ottf->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_TRANSLATE_FRAME_U) {
        auth_3d_key_load(auth, &ott->translate_frame_u, &ottf->translate_frame_u);
        enum_or(ott->flags, AUTH_3D_OBJECT_TEXTURE_TRANSFORM_TRANSLATE_FRAME_U);
    }

    if (ottf->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_TRANSLATE_FRAME_V) {
        auth_3d_key_load(auth, &ott->translate_frame_v, &ottf->translate_frame_v);
        enum_or(ott->flags, AUTH_3D_OBJECT_TEXTURE_TRANSFORM_TRANSLATE_FRAME_V);
    }

    ott->texture_id = tex_db->get_texture_id(ott->name.c_str());
}

static void auth_3d_play_control_load(auth_3d* auth, auth_3d_play_control_file* pcf) {
    auth_3d_play_control* pc = &auth->play_control;

    pc->begin = pcf->begin;

    if (pcf->flags & A3DA_PLAY_CONTROL_DIV) {
        pc->div = pcf->div;
        enum_or(pc->flags, AUTH_3D_PLAY_CONTROL_DIV);
    }

    pc->fps = pcf->fps;

    if (pcf->flags & A3DA_PLAY_CONTROL_OFFSET) {
        pc->div = pcf->div;
        enum_or(pc->flags, AUTH_3D_PLAY_CONTROL_OFFSET);
    }

    pc->size = pcf->size;
}

static void auth_3d_point_disp(auth_3d_point* p, mat4* parent_mat, render_context* rctx) {

}

static void auth_3d_point_load(auth_3d* auth, auth_3d_point* p, auth_3d_point_file* pf) {
    auth_3d_model_transform_load(auth, &p->model_transform, &pf->model_transform);
    p->name.assign(pf->name);
}

static void auth_3d_post_process_load(auth_3d* auth, auth_3d_post_process* pp, auth_3d_post_process_file* ppf) {
    if (ppf->flags & A3DA_POST_PROCESS_INTENSITY) {
        auth_3d_rgba_load(auth, &pp->intensity, &ppf->intensity);
        enum_or(pp->flags, AUTH_3D_POST_PROCESS_INTENSITY);
    }

    if (ppf->flags & A3DA_POST_PROCESS_LENS_FLARE) {
        auth_3d_key_load(auth, &pp->lens_flare, &ppf->lens_flare);
        enum_or(pp->flags, AUTH_3D_POST_PROCESS_LENS_FLARE);
    }

    if (ppf->flags & A3DA_POST_PROCESS_LENS_GHOST) {
        auth_3d_key_load(auth, &pp->lens_ghost, &ppf->lens_ghost);
        enum_or(pp->flags, AUTH_3D_POST_PROCESS_LENS_GHOST);
    }

    if (ppf->flags & A3DA_POST_PROCESS_LENS_SHAFT) {
        auth_3d_key_load(auth, &pp->lens_shaft, &ppf->lens_shaft);
        enum_or(pp->flags, AUTH_3D_POST_PROCESS_LENS_SHAFT);
    }

    if (ppf->flags & A3DA_POST_PROCESS_RADIUS) {
        auth_3d_rgba_load(auth, &pp->radius, &ppf->radius);
        enum_or(pp->flags, AUTH_3D_POST_PROCESS_RADIUS);
    }

    if (ppf->flags & A3DA_POST_PROCESS_SCENE_FADE) {
        auth_3d_rgba_load(auth, &pp->scene_fade, &ppf->scene_fade);
        enum_or(pp->flags, AUTH_3D_POST_PROCESS_SCENE_FADE);
    }
}

static void auth_3d_post_process_restore_prev_value(auth_3d_post_process* pp, render_context* rctx) {
    post_process_blur* blur = rctx->post_process.blur;
    post_process_tone_map* tm = rctx->post_process.tone_map;

    if (pp->flags_init & AUTH_3D_POST_PROCESS_INTENSITY)
        blur->set_intensity(pp->intensity_init);

    if (pp->flags_init & AUTH_3D_POST_PROCESS_LENS_FLARE)
        tm->set_lens_flare(pp->lens_flare_init);

    if (pp->flags_init & AUTH_3D_POST_PROCESS_LENS_GHOST)
        tm->set_lens_ghost(pp->lens_ghost_init);

    if (pp->flags_init & AUTH_3D_POST_PROCESS_LENS_SHAFT)
        tm->set_lens_shaft(pp->lens_shaft_init);

    if (pp->flags_init & AUTH_3D_POST_PROCESS_RADIUS)
        blur->set_radius(pp->radius_init);

    if (pp->flags_init & AUTH_3D_POST_PROCESS_SCENE_FADE)
        tm->set_scene_fade(pp->scene_fade_init);
}

static void auth_3d_post_process_set(auth_3d_post_process* pp, render_context* rctx) {
    post_process_blur* blur = rctx->post_process.blur;
    post_process_tone_map* tm = rctx->post_process.tone_map;

    if (pp->flags & AUTH_3D_POST_PROCESS_INTENSITY) {
        if (!(pp->flags_init & AUTH_3D_POST_PROCESS_INTENSITY)) {
            pp->intensity_init = blur->get_intensity();
            enum_or(pp->flags_init, AUTH_3D_POST_PROCESS_INTENSITY);
        }

        blur->set_intensity(*(vec3*)&pp->intensity.value);
    }

    if (pp->flags & AUTH_3D_POST_PROCESS_LENS_FLARE) {
        if (!(pp->flags_init & AUTH_3D_POST_PROCESS_LENS_FLARE)) {
            pp->lens_flare_init = tm->get_lens_flare();
            enum_or(pp->flags_init, AUTH_3D_POST_PROCESS_LENS_FLARE);
        }

        tm->set_lens_flare(pp->lens_flare_value);
    }

    if (pp->flags & AUTH_3D_POST_PROCESS_LENS_GHOST) {
        if (!(pp->flags_init & AUTH_3D_POST_PROCESS_LENS_GHOST)) {
            pp->lens_ghost_init = tm->get_lens_ghost();
            enum_or(pp->flags_init, AUTH_3D_POST_PROCESS_LENS_GHOST);
        }

        tm->set_lens_ghost(pp->lens_ghost_value);
    }

    if (pp->flags & AUTH_3D_POST_PROCESS_LENS_SHAFT) {
        if (!(pp->flags_init & AUTH_3D_POST_PROCESS_LENS_SHAFT)) {
            pp->lens_shaft_init = tm->get_lens_shaft();
            enum_or(pp->flags_init, AUTH_3D_POST_PROCESS_LENS_SHAFT);
        }

        tm->set_lens_shaft(pp->lens_shaft_value);
    }

    if (pp->flags & AUTH_3D_POST_PROCESS_RADIUS) {
        if (!(pp->flags_init & AUTH_3D_POST_PROCESS_RADIUS)) {
            pp->radius_init = blur->get_radius();
            enum_or(pp->flags_init, AUTH_3D_POST_PROCESS_RADIUS);
        }

        blur->set_radius(*(vec3*)&pp->radius.value);
    }

    if (pp->flags & AUTH_3D_POST_PROCESS_SCENE_FADE) {
        if (!(pp->flags_init & AUTH_3D_POST_PROCESS_SCENE_FADE)) {
            pp->scene_fade_init = tm->get_scene_fade();
            enum_or(pp->flags_init, AUTH_3D_POST_PROCESS_SCENE_FADE);
        }

        tm->set_scene_fade(pp->scene_fade.value);
    }
}

static bool auth_3d_data_struct_init(auth_3d_data_struct* auth_3d_data, render_context* rctx) {
    auth_3d_data->loaded_ids.clear();
    auth_3d_data->loaded_ids.shrink_to_fit();
    for (int32_t i = 0; i < AUTH_3D_DATA_COUNT; i++)
        auth_3d_data->data[i].reset();
    return true;
}

static bool auth_3d_data_struct_check_category_loaded(
    auth_3d_data_struct* auth_3d_data, const char* category_name) {
    if (category_name) {
        auth_3d_farc* a3da_farc = auth_3d_data_struct_get_farc(auth_3d_data, category_name);
        if (a3da_farc)
            return a3da_farc->state == 2;
    }
    return true;
}

static bool auth_3d_data_struct_check_category_loaded(
    auth_3d_data_struct* auth_3d_data, uint32_t category_hash) {
    if (!category_hash || category_hash == hash_murmurhash_empty)
        return true;

    auto elem = auth_3d_data->farcs_modern.find(category_hash);
    if (elem != auth_3d_data->farcs_modern.end())
        return elem->second.state == 2;
    return true;
}

static bool auth_3d_data_struct_ctrl(auth_3d_data_struct* auth_3d_data, render_context* rctx) {
    for (int32_t& i : auth_3d_data->loaded_ids)
        if (i >= 0 && (i & 0x7FFF) < AUTH_3D_DATA_COUNT) {
            auth_3d* auth = &auth_3d_data->data[i & 0x7FFF];
            if (auth && auth->id == i)
                auth->ctrl(rctx);
        }
    return false;
}

static void auth_3d_data_struct_disp(auth_3d_data_struct* auth_3d_data, render_context* rctx) {
    for (int32_t& i : auth_3d_data->loaded_ids)
        if (i >= 0 && (i & 0x7FFF) < AUTH_3D_DATA_COUNT) {
            auth_3d* auth = &auth_3d_data->data[i & 0x7FFF];
            if (auth && auth->id == i)
                auth->disp(rctx);
        }
}

static auth_3d_farc* auth_3d_data_struct_get_farc(
    auth_3d_data_struct* auth_3d_data, const char* category_name) {
    if (!category_name || !auth_3d_data->farcs.size())
        return 0;

    uint64_t name_hash = hash_utf8_fnv1a64m(category_name);
    for (auth_3d_farc& i : auth_3d_data->farcs)
        if (hash_string_fnv1a64m(i.name) == name_hash)
            return &i;
    return 0;
}

static void auth_3d_data_struct_load_category(auth_3d_data_struct* auth_3d_data,
    const char* category_name, const char* mdata_dir) {
    if (!category_name)
        return;

    auth_3d_farc* a3da_farc = auth_3d_data_struct_get_farc(auth_3d_data, category_name);
    if (a3da_farc)
        auth_3d_farc_load(a3da_farc, mdata_dir);
}

static void auth_3d_data_struct_load_category(auth_3d_data_struct* auth_3d_data,
    void* data, const char* category_name, uint32_t category_hash) {
    if (!category_hash || category_hash == hash_murmurhash_empty)
        return;

    auto elem = auth_3d_data->farcs_modern.find(category_hash);
    if (elem == auth_3d_data->farcs_modern.end()) {
        elem = auth_3d_data->farcs_modern.insert({ category_hash, {} }).first;
        auth_3d_farc* a3da_farc = &elem->second;
        a3da_farc->name.assign(category_name);
    }

    auth_3d_farc* a3da_farc = &elem->second;
    if (a3da_farc)
        auth_3d_farc_load_modern(a3da_farc, data);
}

static void auth_3d_data_struct_unload_category(
    auth_3d_data_struct* auth_3d_data, const char* category_name) {
    if (!category_name)
        return;

    auth_3d_farc* a3da_farc = auth_3d_data_struct_get_farc(auth_3d_data, category_name);
    if (a3da_farc)
        auth_3d_farc_unload(a3da_farc);
}

static void auth_3d_data_struct_unload_category(
    auth_3d_data_struct* auth_3d_data, uint32_t category_hash) {
    if (!category_hash || category_hash == hash_murmurhash_empty)
        return;

    auto elem = auth_3d_data->farcs_modern.find(category_hash);
    if (elem == auth_3d_data->farcs_modern.end())
        return;

    auth_3d_farc_unload(&elem->second);
    if (elem->second.load_count <= 0)
        auth_3d_data->farcs_modern.erase(elem);
}

static void auth_3d_farc_load(auth_3d_farc* a3da_farc, const char* mdata_dir) {
    if (a3da_farc->load_count) {
        a3da_farc->load_count++;
        return;
    }

    a3da_farc->load_count = 1;
    auth_3d_farc_read_file(a3da_farc, mdata_dir);
}

static void auth_3d_farc_load_modern(auth_3d_farc* a3da_farc, void* data) {
    if (a3da_farc->load_count) {
        a3da_farc->load_count++;
        return;
    }

    a3da_farc->load_count = 1;
    auth_3d_farc_read_file_modern(a3da_farc, data);
}

static void auth_3d_farc_read_file(auth_3d_farc* a3da_farc, const char* mdata_dir) {
    if (a3da_farc->state) {
        a3da_farc->state = 2;
        return;
    }

    a3da_farc->state = 1;

    if (mdata_dir && *mdata_dir) {
        a3da_farc->path.assign(mdata_dir);
        a3da_farc->path.append("rom/auth_3d/");
    }
    else
        a3da_farc->path.assign("rom/auth_3d/");
    a3da_farc->file.assign(a3da_farc->name);
    a3da_farc->file.append(".farc");

    if (a3da_farc->file_handler.read_file(&data_list[DATA_AFT],
        a3da_farc->path.c_str(), a3da_farc->file.c_str()))
        a3da_farc->file_handler.set_callback_data(0,
            (PFNFILEHANDLERCALLBACK*)auth_3d_farc_read_func, a3da_farc);
    else
        a3da_farc->state = 2;
}

static void auth_3d_farc_read_file_modern(auth_3d_farc* a3da_farc, void* data) {
    if (a3da_farc->state) {
        a3da_farc->state = 2;
        return;
    }

    a3da_farc->state = 1;
    a3da_farc->path.assign("root+/auth_3d/");
    a3da_farc->file.assign(a3da_farc->name);
    a3da_farc->file.append(".farc");

    if (a3da_farc->file_handler.read_file(data, a3da_farc->path.c_str(),
        hash_string_murmurhash(a3da_farc->name), ".farc"))
        a3da_farc->file_handler.set_callback_data(0,
            (PFNFILEHANDLERCALLBACK*)auth_3d_farc_read_func, a3da_farc);
    else
        a3da_farc->state = 2;
}

static bool auth_3d_farc_read_func(auth_3d_farc* a3da_farc, const void* data, size_t size) {
    if (a3da_farc->state == 1) {
        a3da_farc->state = 2;
        a3da_farc->data = data;
        a3da_farc->size = size;
    }

    if (!a3da_farc->farc)
        a3da_farc->farc = new farc;
    if (!a3da_farc->farc)
        return false;

    a3da_farc->farc->read(a3da_farc->data, a3da_farc->size, true);
    return true;
}

static void auth_3d_farc_reset(auth_3d_farc* a3da_farc) {
    if (a3da_farc->state == 1)
        a3da_farc->file_handler.call_free_callback();
    else if (a3da_farc->state == 2) {
        delete a3da_farc->farc;
        a3da_farc->farc = 0;
        a3da_farc->file_handler.reset();
    }

    a3da_farc->path.clear();
    a3da_farc->path.shrink_to_fit();
    a3da_farc->file.clear();
    a3da_farc->file.shrink_to_fit();
    a3da_farc->data = 0;
    a3da_farc->size = 0;
    a3da_farc->state = 0;
}

static void auth_3d_farc_unload(auth_3d_farc* a3da_farc) {
    if (--a3da_farc->load_count < 0)
        a3da_farc->load_count = 0;
    else if (!a3da_farc->load_count)
        auth_3d_farc_reset(a3da_farc);
}

static void auth_3d_uid_file_load(auth_3d_uid_file* uid_file) {
    uid_file->state = 1;
    auth_3d_farc_load(uid_file->farc, 0);
}

static void auth_3d_uid_file_modern_load(auth_3d_uid_file_modern* uid_file) {
    uid_file->state = 1;
    auth_3d_farc_load(uid_file->farc, 0);
}

static void auth_3d_uid_file_load_file(auth_3d_uid_file* uid_file, auth_3d_database* auth_3d_db) {
    if (uid_file->load_count)
        uid_file->load_count++;
    else if (uid_file->state)
        return;

    uid_file->state = 2;
    if (uid_file->uid >= auth_3d_db->uid.size())
        return;

    auth_3d_database_uid* db_uid = &auth_3d_db->uid[uid_file->uid];

    if (!db_uid->enabled)
        return;

    uid_file->name = db_uid->name.c_str();
    uid_file->file_name.assign(db_uid->name);
    uid_file->file_name.append(".a3da");
    auth_3d_farc* farc = auth_3d_data_struct_get_farc(auth_3d_data, db_uid->category.c_str());
    uid_file->farc = farc;
    if (farc) {
        uid_file->load_count = 1;
        auth_3d_uid_file_load(uid_file);
    }
}

static void auth_3d_uid_file_modern_load_file(auth_3d_uid_file_modern* uid_file) {
    if (uid_file->load_count)
        uid_file->load_count++;
    else if (uid_file->state)
        return;

    uid_file->state = 2;

    auth_3d_farc* farc = 0;
    for (auto& i : auth_3d_data->farcs_modern)
        if (i.second.farc && i.second.farc->has_file(uid_file->hash)) {
            farc = &i.second;
            break;
        }

    if (!farc)
        return;

    uid_file->file_name.assign(farc->farc->get_file_name(uid_file->hash));

    uid_file->farc = farc;
    if (farc) {
        uid_file->load_count = 1;
        auth_3d_uid_file_modern_load(uid_file);
    }
}

static void auth_3d_uid_file_unload(auth_3d_uid_file* uid_file) {
    if (--uid_file->load_count < 0)
        uid_file->load_count = 0;
    else if (!uid_file->load_count)
        auth_3d_uid_file_unload_file(uid_file);
}

static void auth_3d_uid_file_modern_unload(auth_3d_uid_file_modern* uid_file) {
    if (--uid_file->load_count < 0)
        uid_file->load_count = 0;
    else if (!uid_file->load_count)
        auth_3d_uid_file_modern_unload_file(uid_file);
}

static void auth_3d_uid_file_unload_file(auth_3d_uid_file* uid_file) {
    if (uid_file->state == 1 || uid_file->state == 2)
        auth_3d_farc_unload(uid_file->farc);
    uid_file->name = 0;
    uid_file->farc = 0;
    uid_file->state = 0;
}

static void auth_3d_uid_file_modern_unload_file(auth_3d_uid_file_modern* uid_file) {
    if (uid_file->state == 1 || uid_file->state == 2)
        auth_3d_farc_unload(uid_file->farc);
    uid_file->name.clear();
    uid_file->name.shrink_to_fit();
    uid_file->farc = 0;
    uid_file->state = 0;
    auth_3d_data->uid_files_modern.erase(uid_file->hash);
}

static size_t auth_3d_time_event_radix_index_func_time(auth_3d_time_event* data, size_t index) {
    return *(uint32_t*)&data[index].frame;
}

static void Auth3dTestTask__SetAuth3dId(Auth3dTestTask* tt) {
    if (task_stage_check_not_loaded() || tt->auth_3d_uid == -1
        || !tt->load_category.size() || tt->auth_3d_uid == tt->auth_3d_id.get_uid())
        return;

    if (tt->load_category.compare(tt->category)) {
        if (tt->category.size()) {
            auth_3d_data_unload_category(tt->category.c_str());
            for (uint32_t& i : tt->obj_sets)
                object_storage_unload_set(i);
            tt->obj_sets.clear();
        }
        auth_3d_data_load_category(tt->load_category.c_str());
        tt->category.assign(tt->load_category);
        if (tt->window.obj_link)
            tt->field_394 = 1;
    }

    if (!auth_3d_data_check_category_loaded(tt->category.c_str()))
        return;

    if (tt->field_394 == 1) {
        data_struct* aft_data = &data_list[DATA_AFT];
        auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;
        object_database* aft_obj_db = &aft_data->data_ft.obj_db;

        auth_3d_data_get_obj_sets_from_category(tt->category, tt->obj_sets, aft_auth_3d_db, aft_obj_db);

        for (uint32_t& i : tt->obj_sets)
            object_storage_load_set(aft_data, aft_obj_db, i);
        tt->field_394 = 2;
    }
    else if (tt->field_394 == 2) {
        bool wait_load = false;
        for (uint32_t& i : tt->obj_sets)
            if (object_storage_load_obj_set_check_not_read(i))
                wait_load = true;

        if (!wait_load)
            tt->field_394 = 0;
    }

    if (!tt->field_394) {
        data_struct* aft_data = &data_list[DATA_AFT];
        auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;

        tt->auth_3d_id.unload_id(rctx_ptr);
        tt->auth_3d_id = auth_3d_data_load_uid(tt->auth_3d_uid, aft_auth_3d_db);
        if (tt->auth_3d_id.check_not_empty()) {
            tt->auth_3d_id.set_enable(false);
            tt->auth_3d_id.read_file(aft_auth_3d_db);
            tt->field_1D4 = 1;
        }
        tt->auth_3d_uid = -1;
        return;
    }
}

static void Auth3dTestTask__SetStage(Auth3dTestTask* tt) {
    if (tt->window.stage_link_change) {
        data_struct* aft_data = &data_list[DATA_AFT];
        auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;
        stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

        const char* name = auth_3d_data_get_uid_name(tt->auth_3d_uid, aft_auth_3d_db);
        size_t name_length = utf8_length(name);
        if (name && name_length >= 3) {
            size_t v5 = name_length - 2;
            const char* v6 = name;
            while (v5) {
                const char* v7 = (const char*)memchr(v6, 'S', v5);
                if (!v7)
                    break;

                if (memcmp(v7, "STG", min_def(v5, 3))) {
                    v5 += v6 - v7 - 1;
                    v6 = v7 + 1;
                    continue;
                }

                size_t v16 = v7 - name;
                if (v16 == -1 || v16 >= name_length || name_length == v16)
                    break;

                size_t v17 = name_length - v16;
                for (const char* i = name + v16; v17; ) {
                    const char* v20 = (const char*)memchr(i, '_', v17);
                    if (!v20)
                        break;

                    if (*v20 != '_') {
                        v17 += i - v20 - 1;
                        i = v20 + 1;
                        continue;
                    }

                    size_t v24 = v20 - name;
                    if (v24 != -1) {
                        std::string v30 = std::string(name + v16, v24 - v16);
                        int32_t stage_index = aft_stage_data->get_stage_index(v30.c_str());
                        if (tt->stage_index != stage_index)
                            tt->load_stage_index = stage_index;
                    }
                    break;
                }
                break;
            }
        }
    }

    if (tt->load_stage_index != -1 && tt->load_stage_index != tt->stage_index) {
        task_stage_set_stage_index(tt->load_stage_index);
        tt->stage_index = tt->load_stage_index;
        tt->load_stage_index = -1;
    }
}
