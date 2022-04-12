/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "auth_3d.h"
#include "../KKdLib/hash.h"
#include "../KKdLib/interpolation.h"
#include "../KKdLib/str_utils.h"
#include "draw_task.h"
#include "object.h"

namespace auth_3d_detail {
    class TaskAuth3d : public Task {
    public:
        TaskAuth3d();
        virtual ~TaskAuth3d() override;
        virtual bool Init() override;
        virtual bool Ctrl() override;
        virtual void Disp() override;
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

static bool auth_3d_key_detect_fast_change(auth_3d_key* data, float_t frame, float_t threshold);
static kft3* auth_3d_key_find_keyframe(auth_3d_key* data, float_t frame);
static float_t auth_3d_key_interpolate_inner(auth_3d_key* data, float_t frame);
static float_t auth_3d_interpolate_value(auth_3d_key_type type,
    float_t frame, kft3* curr_key, kft3* next_key);
static void auth_3d_key_load(auth_3d* auth, auth_3d_key* k, auth_3d_key_file* kf);
static void auth_3d_rgba_load(auth_3d* auth, auth_3d_rgba* rgba, auth_3d_rgba_file* rgbaf);
static void auth_3d_vec3_load(auth_3d* auth, auth_3d_vec3* vec, auth_3d_vec3_file* vecf);
static void auth_3d_model_transform_load(auth_3d* auth, auth_3d_model_transform* mt, auth_3d_model_transform_file* mtf);
static void auth_3d_model_transform_set_mat(auth_3d_model_transform* mt, mat4* parent_mat);

static int32_t auth_3d_get_auth_3d_object_index_by_object_info(auth_3d* auth,
    object_info obj_info, int32_t instance);
static int32_t auth_3d_get_auth_3d_object_index_by_hash(auth_3d* auth,
    uint32_t object_hash, int32_t instance);
static mat4* auth_3d_get_auth_3d_object_hrc_bone_mats(auth_3d* auth, size_t index);
static int32_t auth_3d_get_auth_3d_object_hrc_index_by_object_info(auth_3d* auth,
    object_info obj_info, int32_t instance);
static int32_t auth_3d_get_auth_3d_object_hrc_index_by_hash(auth_3d* auth,
    uint32_t object_hash, int32_t instance);
static void auth_3d_read_file(auth_3d* auth, auth_3d_database* auth_3d_db);
static void auth_3d_read_file_modern(auth_3d* auth);

static void auth_3d_ambient_ctrl(auth_3d_ambient* a, float_t frame);
static void auth_3d_ambient_load(auth_3d* auth, auth_3d_ambient* a, auth_3d_ambient_file* af);
static void auth_3d_ambient_set(auth_3d_ambient* a, render_context* rctx);
static void auth_3d_camera_auxiliary_ctrl(auth_3d_camera_auxiliary* ca, float_t frame);
static void auth_3d_camera_auxiliary_load(auth_3d* auth, auth_3d_camera_auxiliary_file* caf);
static void auth_3d_camera_auxiliary_restore_prev_value(auth_3d_camera_auxiliary* ca, render_context* rctx);
static void auth_3d_camera_auxiliary_set(auth_3d_camera_auxiliary* ca, render_context* rctx);
static void auth_3d_camera_root_ctrl(auth_3d_camera_root* cr, float_t frame, mat4* mat, render_context* rctx);
static void auth_3d_camera_root_load(auth_3d* auth, auth_3d_camera_root* cr, auth_3d_camera_root_file* crf);
static void auth_3d_camera_root_view_point_load(auth_3d* auth, auth_3d_camera_root_view_point* crvp,
    auth_3d_camera_root_view_point_file* crvpf);
static void auth_3d_chara_ctrl(auth_3d_chara* c, float_t frame);
static void auth_3d_chara_disp(auth_3d_chara* c, mat4* parent_mat, render_context* rctx);
static void auth_3d_chara_load(auth_3d* auth, auth_3d_chara* c, auth_3d_chara_file* cf);
static void auth_3d_curve_ctrl(auth_3d_curve* c, float_t frame);
static void auth_3d_curve_load(auth_3d* auth, auth_3d_curve* c, auth_3d_curve_file* cf);
static void auth_3d_dof_ctrl(auth_3d_dof* d, float_t frame);
static void auth_3d_dof_load(auth_3d* auth, auth_3d_dof* d, auth_3d_dof_file* df);
static void auth_3d_dof_restore_prev_value(auth_3d_dof* d, render_context* rctx);
static void auth_3d_dof_set(auth_3d_dof* d, render_context* rctx);
static void auth_3d_event_load(auth_3d* auth, auth_3d_event* e, auth_3d_event_file* ef);
static void auth_3d_fog_ctrl(auth_3d_fog* f, float_t frame);
static void auth_3d_fog_load(auth_3d* auth, auth_3d_fog* e, auth_3d_fog_file* ff);
static void auth_3d_fog_restore_prev_value(auth_3d_fog* f, render_context* rctx);
static void auth_3d_fog_set(auth_3d_fog* f, render_context* rctx);
static void auth_3d_light_ctrl(auth_3d_light* l, float_t frame);
static void auth_3d_light_load(auth_3d* auth, auth_3d_light* a, auth_3d_light_file* lf);
static void auth_3d_light_restore_prev_value(auth_3d_light* l, render_context* rctx);
static void auth_3d_light_set(auth_3d_light* l, render_context* rctx);
static void auth_3d_m_object_hrc_ctrl(auth_3d_m_object_hrc* moh, float_t frame);
static void auth_3d_m_object_hrc_get_mat(auth_3d_m_object_hrc* moh, mat4* mat);
static void auth_3d_m_object_hrc_load(auth_3d* auth, auth_3d_m_object_hrc* moh,
    auth_3d_m_object_hrc_file* mohf, object_database* obj_db);
static void auth_3d_m_object_hrc_disp(auth_3d_m_object_hrc* moh, auth_3d* auth, render_context* rctx);
static void auth_3d_m_object_hrc_list_ctrl(auth_3d_m_object_hrc* moh, mat4* parent_mat);
static void auth_3d_m_object_hrc_nodes_mat_mult(auth_3d_m_object_hrc* moh);
static void auth_3d_material_list_ctrl(auth_3d_material_list* ml, float_t frame);
static void auth_3d_material_list_load(auth_3d* auth, auth_3d_material_list* ml, auth_3d_material_list_file* mlf);
static void auth_3d_material_list_restore_prev_value(auth_3d_material_list* ml);
static void auth_3d_material_list_set(auth_3d_material_list* ml);
static void auth_3d_object_ctrl(auth_3d_object* o, float_t frame);
static void auth_3d_object_disp(auth_3d_object* o, auth_3d* auth, render_context* rctx);
static void auth_3d_object_load(auth_3d* auth, auth_3d_object* o,
    auth_3d_object_file* of, object_database* obj_db, texture_database* tex_db);
static void auth_3d_object_curve_ctrl(auth_3d_object_curve* oc, float_t frame);
static void auth_3d_object_curve_load(auth_3d* auth, auth_3d_object_curve* curve,
    std::string& name, float_t frame_offset);
static void auth_3d_object_list_ctrl(auth_3d_object* o, mat4* parent_mat);
static void auth_3d_object_hrc_ctrl(auth_3d_object_hrc* oh, float_t frame);
static void auth_3d_object_hrc_disp(auth_3d_object_hrc* oh, auth_3d* auth, render_context* rctx);
static void auth_3d_object_hrc_load(auth_3d* auth, auth_3d_object_hrc* oh,
    auth_3d_object_hrc_file* ohf, object_database* obj_db);
static void auth_3d_object_hrc_list_ctrl(auth_3d_object_hrc* oh, mat4* mat);
static void auth_3d_object_hrc_nodes_mat_mult(auth_3d_object_hrc* oh, mat4* mat);
static void auth_3d_object_instance_load(auth_3d* auth, auth_3d_object_instance* oi,
    auth_3d_object_instance_file* oif, auth_3d_m_object_hrc* moh, object_database* obj_db);
static void auth_3d_object_model_transform_ctrl(auth_3d_object_model_transform* obj_mt, float_t frame);
static void auth_3d_object_model_transform_load(auth_3d* auth,
    auth_3d_object_model_transform* omt, auth_3d_model_transform_file* mtf);
static void auth_3d_object_model_transform_mat_mult(auth_3d_object_model_transform* obj_mt, mat4* mat);
static void auth_3d_object_model_transform_set_mat_inner(auth_3d_object_model_transform* obj_mt);
static void auth_3d_object_node_load(auth_3d* auth,
    auth_3d_object_node* on, auth_3d_object_node_file* onf);
static void auth_3d_object_texture_pattern_ctrl(
    auth_3d_object_texture_pattern* otp, float_t frame);
static void auth_3d_object_texture_pattern_load(auth_3d* auth, auth_3d_object_texture_pattern* otp,
    auth_3d_object_texture_pattern_file* otpf, texture_database* tex_db);
static void auth_3d_object_texture_transform_ctrl(
    auth_3d_object_texture_transform* ott, float_t frame);
static void auth_3d_object_texture_transform_load(auth_3d* auth, auth_3d_object_texture_transform* ott,
    auth_3d_object_texture_transform_file* ottf, texture_database* tex_db);
static void auth_3d_play_control_load(auth_3d* auth, auth_3d_play_control_file* pcf);
static void auth_3d_point_ctrl(auth_3d_point* p, float_t frame);
static void auth_3d_point_disp(auth_3d_point* p, mat4* parent_mat, render_context* rctx);
static void auth_3d_point_load(auth_3d* auth, auth_3d_point* p, auth_3d_point_file* pf);
static void auth_3d_post_process_ctrl(auth_3d_post_process* pp, float_t frame);
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

static void auth_3d_farc_free_data(auth_3d_farc* a3da_farc);
static void auth_3d_farc_load(auth_3d_farc* a3da_farc, const char* mdata_dir);
static void auth_3d_farc_load_modern(auth_3d_farc* a3da_farc, void* data);
static void auth_3d_farc_read_file(auth_3d_farc* a3da_farc, const char* mdata_dir);
static void auth_3d_farc_read_file_modern(auth_3d_farc* a3da_farc, void* data);
static bool auth_3d_farc_read_func(auth_3d_farc* a3da_farc, void* data, size_t size);
static void auth_3d_farc_unload(auth_3d_farc* a3da_farc);

static void auth_3d_uid_file_load(auth_3d_uid_file* uid_file);
static void auth_3d_uid_file_modern_load(auth_3d_uid_file_modern* uid_file);
static void auth_3d_uid_file_load_file(auth_3d_uid_file* uid_file, auth_3d_database* auth_3d_db);
static void auth_3d_uid_file_modern_load_file(auth_3d_uid_file_modern* uid_file);
static void auth_3d_uid_file_unload(auth_3d_uid_file* uid_file);
static void auth_3d_uid_file_modern_unload(auth_3d_uid_file_modern* uid_file);
static void auth_3d_uid_file_unload_file(auth_3d_uid_file* uid_file);
static void auth_3d_uid_file_modern_unload_file(auth_3d_uid_file_modern* uid_file);

auth_3d_data_struct* auth_3d_data;
auth_3d_detail::TaskAuth3d task_auth_3d;
static int16_t auth_3d_load_counter;

extern render_context* rctx_ptr;

auth_3d::auth_3d() : uid(), id(), mat(), enable(), camera_root_update(), visible(), repeat(),
ended(), left_right_reverse(), once(), alpha(), chara_id(), shadow(), frame_rate(), frame(),
req_frame(), max_frame(), frame_changed(), frame_offset(), last_frame(), paused() {
    hash = hash_murmurhash_empty;
    reset();
}

auth_3d::~auth_3d() {
    reset();
}

void auth_3d::ctrl(render_context* rctx) {
    if (state == 1)
        if (hash == hash_murmurhash_empty) {
            data_struct* data = rctx->data;
            auth_3d_database* auth_3d_db = &data->data_ft.auth_3d_db;
            auth_3d_database_uid* db_uid = &auth_3d_db->uid[uid];
            if (db_uid->enabled) {
                auth_3d_uid_file* uid_file = &auth_3d_data->uid_files[uid];
                if (uid_file->uid == uid && uid_file->state == 1 && uid_file->farc->state == 2) {
                    state = 2;
                    farc_file* ff = uid_file->farc->farc->read_file(uid_file->file_name.c_str());
                    if (ff) {
                        a3da a;
                        a.read(ff->data, ff->size);
                        load(&a, &data->data_ft.obj_db, &data->data_ft.tex_db);
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

                    const char* l_str = ff->name.c_str();
                    const char* t = strrchr(l_str, '.');
                    size_t l_len = ff->name.size();
                    if (t)
                        l_len = t - l_str;

                    uid_file->name = std::string(l_str, l_len);
                    uid_file->file_name = ff->name;
                    load(&a, uid_file->obj_db, uid_file->tex_db);
                    hash = uid_file->hash;
                }
            }
        }

    if (state != 2 || !enable)
        return;

    mat4 mat = this->mat;

    bool set = false;
    while (true) {
        if (frame_changed) {
            frame_changed = false;
            frame = req_frame;
            ended = false;
            if (max_frame >= 0.0f && frame >= max_frame)
                max_frame = -1.0f;
        }
        else if (!paused) {
            frame += frame_rate->GetDeltaFrame();
            if (max_frame >= 0.0f && frame > max_frame)
                frame = max_frame;
        }

        for (auth_3d_point& i : point)
            auth_3d_point_ctrl(&i, frame);

        for (auth_3d_curve& i : curve)
            auth_3d_curve_ctrl(&i, frame);

        for (auth_3d_chara& i : chara)
            auth_3d_chara_ctrl(&i, frame);

        for (auth_3d_object& i : object)
            auth_3d_object_ctrl(&i, frame);

        for (auth_3d_object*& i : object_list)
            auth_3d_object_list_ctrl(i, &mat);

        for (auth_3d_object_hrc& i : object_hrc)
            auth_3d_object_hrc_ctrl(&i, frame);

        for (auth_3d_object_hrc*& i : object_hrc_list)
            auth_3d_object_hrc_list_ctrl(i, &mat);

        for (auth_3d_m_object_hrc& i : m_object_hrc)
            auth_3d_m_object_hrc_ctrl(&i, frame);

        for (auth_3d_m_object_hrc*& i : m_object_hrc_list)
            auth_3d_m_object_hrc_list_ctrl(i, &mat);

        for (auth_3d_ambient& i : ambient) {
            auth_3d_ambient_ctrl(&i, frame);
            auth_3d_ambient_set(&i, rctx);
        }

        for (auth_3d_light& i : light) {
            auth_3d_light_ctrl(&i, frame);
            auth_3d_light_set(&i, rctx);
        }

        for (auth_3d_fog& i : fog) {
            auth_3d_fog_ctrl(&i, frame);
            auth_3d_fog_set(&i, rctx);
        }

        auth_3d_post_process_ctrl(&post_process, frame);
        auth_3d_post_process_set(&post_process, rctx);

        for (auth_3d_material_list& i : material_list) {
            auth_3d_material_list_ctrl(&i, frame);
            auth_3d_material_list_set(&i);
        }

        auth_3d_camera_auxiliary_set(&camera_auxiliary, rctx);
        auth_3d_camera_auxiliary_ctrl(&camera_auxiliary, frame);

        if (camera_root_update)
            for (auth_3d_camera_root& i : camera_root) {
                auth_3d_camera_root_ctrl(&i, frame, &mat, rctx);

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
                camera_set_interest(cam, &interest);
                camera_set_view_point(cam, &view_point);
                camera_set_fov(cam, fov * RAD_TO_DEG);
                camera_set_roll(cam, roll * RAD_TO_DEG);
                break;
            }

        auth_3d_dof_ctrl(&dof, frame);
        auth_3d_dof_set(&dof, rctx);

        if (set || !repeat || last_frame > frame)
            break;

        req_frame = frame - frame + frame_offset;
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

    mat4 mat = this->mat;

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
}

void auth_3d::load(a3da* auth_file,
    object_database* obj_db, texture_database* tex_db) {
    if (!auth_file)
        return;

    hash = hash_murmurhash_empty;
    auth_3d_play_control_load(this, &auth_file->play_control);

    camera_root_update = true;
    visible = true;
    repeat = true;
    ended = false;
    left_right_reverse = false;
    once = false;
    alpha = 1.0f;
    draw_task_flags = DRAW_TASK_10000;
    chara_id = -1;
    shadow = false;
    frame_rate = &sys_frame_rate;
    frame = 0.0f;
    req_frame = 0.0f;
    max_frame = play_control.size;
    frame_changed = false;
    frame_offset = 0.0f;
    last_frame = play_control.size;
    paused = false;

    this->obj_db = obj_db;
    this->tex_db = tex_db;

    auth_3d_camera_auxiliary_load(this, &auth_file->camera_auxiliary);
    auth_3d_dof_load(this, &dof, &auth_file->dof);
    auth_3d_post_process_load(this, &post_process, &auth_file->post_process);

    if (auth_file->ambient.size() > 0) {
        size_t ambient_index = 0;
        ambient = std::vector<auth_3d_ambient>(auth_file->ambient.size());
        for (auth_3d_ambient_file& i : auth_file->ambient)
            auth_3d_ambient_load(this, &ambient[ambient_index++], &i);
    }

    if (auth_file->auth_2d.size() > 0) {
        size_t auth_2d_index = 0;
        auth_2d = std::vector<std::string>(auth_file->auth_2d.size());
        for (std::string& i : auth_file->auth_2d)
            auth_2d[auth_2d_index++] = i;
    }

    if (auth_file->camera_root.size() > 0) {
        size_t camera_root_index = 0;
        camera_root = std::vector<auth_3d_camera_root>(auth_file->camera_root.size());
        for (auth_3d_camera_root_file& i : auth_file->camera_root)
            auth_3d_camera_root_load(this, &camera_root[camera_root_index++], &i);
    }

    if (auth_file->chara.size() > 0) {
        size_t chara_index = 0;
        chara = std::vector<auth_3d_chara>(auth_file->chara.size());
        for (auth_3d_chara_file& i : auth_file->chara)
            auth_3d_chara_load(this, &chara[chara_index++], &i);
    }

    if (auth_file->curve.size() > 0) {
        size_t curve_index = 0;
        curve = std::vector<auth_3d_curve>(auth_file->curve.size());
        for (auth_3d_curve_file& i : auth_file->curve)
            auth_3d_curve_load(this, &curve[curve_index++], &i);
    }

    if (auth_file->event.size() > 0) {
        size_t event_index = 0;
        event = std::vector<auth_3d_event>(auth_file->event.size());
        for (auth_3d_event_file& i : auth_file->event)
            auth_3d_event_load(this, &event[event_index++], &i);
    }

    if (auth_file->fog.size() > 0) {
        size_t fog_index = 0;
        fog = std::vector<auth_3d_fog>(auth_file->fog.size());
        for (auth_3d_fog_file& i : auth_file->fog)
            auth_3d_fog_load(this, &fog[fog_index++], &i);
    }

    if (auth_file->light.size() > 0) {
        size_t light_index = 0;
        light = std::vector<auth_3d_light>(auth_file->light.size());
        for (auth_3d_light_file& i : auth_file->light)
            auth_3d_light_load(this, &light[light_index++], &i);
    }

    if (auth_file->m_object_hrc.size() > 0) {
        size_t m_object_hrc_index = 0;
        m_object_hrc = std::vector<auth_3d_m_object_hrc>(auth_file->m_object_hrc.size());
        for (auth_3d_m_object_hrc_file& i : auth_file->m_object_hrc)
            auth_3d_m_object_hrc_load(this, &m_object_hrc[m_object_hrc_index++], &i, obj_db);
    }

    if (auth_file->material_list.size() > 0) {
        size_t material_list_index = 0;
        material_list = std::vector<auth_3d_material_list>(auth_file->material_list.size());
        for (auth_3d_material_list_file& i : auth_file->material_list)
            auth_3d_material_list_load(this, &material_list[material_list_index++], &i);
    }

    if (auth_file->motion.size() > 0) {
        size_t motion_index = 0;
        motion = std::vector<std::string>(auth_file->motion.size());
        for (std::string& i : auth_file->motion)
            motion[motion_index++] = i;
    }

    if (auth_file->object.size() > 0) {
        size_t object_index = 0;
        object = std::vector<auth_3d_object>(auth_file->object.size());
        for (auth_3d_object_file& i : auth_file->object)
            auth_3d_object_load(this, &object[object_index++], &i, obj_db, tex_db);
    }

    if (auth_file->object_hrc.size() > 0) {
        size_t object_hrc_index = 0;
        object_hrc = std::vector<auth_3d_object_hrc>(auth_file->object_hrc.size());
        for (auth_3d_object_hrc_file& i : auth_file->object_hrc)
            auth_3d_object_hrc_load(this, &object_hrc[object_hrc_index++], &i, obj_db);
    }

    if (auth_file->point.size() > 0) {
        size_t point_index = 0;
        point = std::vector<auth_3d_point>(auth_file->point.size());
        for (auth_3d_point_file& i : auth_file->point)
            auth_3d_point_load(this, &point[point_index++], &i);
    }

    if (auth_file->m_object_hrc_list.size() > 0) {
        size_t m_object_hrc_list_index = 0;
        m_object_hrc_list = std::vector<auth_3d_m_object_hrc*>(auth_file->m_object_hrc_list.size());
        for (std::string& i : auth_file->m_object_hrc_list)
            for (auth_3d_m_object_hrc& j : m_object_hrc)
                if (i == j.name) {
                    m_object_hrc_list[m_object_hrc_list_index++] = &j;
                    break;
                }
        m_object_hrc_list.resize(m_object_hrc_list_index);
    }

    if (auth_file->object_list.size() > 0) {
        size_t object_list_index = 0;
        object_list = std::vector<auth_3d_object*>(auth_file->object_list.size());
        for (std::string & i : auth_file->object_list)
            for (auth_3d_object& j : object)
                if (i == j.name) {
                    object_list[object_list_index++] = &j;
                    break;
                }
        object_list.resize(object_list_index);
    }

    if (auth_file->object_hrc_list.size() > 0) {
        size_t object_hrc_list_index = 0;
        object_hrc_list = std::vector<auth_3d_object_hrc*>(auth_file->object_hrc_list.size());
        for (std::string& i : auth_file->object_hrc_list)
            for (auth_3d_object_hrc& j : object_hrc)
                if (i == j.name) {
                    object_hrc_list[object_hrc_list_index++] = &j;
                    break;
                }
        object_hrc_list.resize(object_hrc_list_index);
    }

    if (object.size() > 0) {
        std::vector<auth_3d_object>* vo = &object;
        std::vector<auth_3d_object_hrc>* voh = &object_hrc;
        for (auth_3d_object& i : object) {
            if (!i.parent_name.size())
                continue;

            if (i.parent_node.size())
                for (auth_3d_object_hrc& j : object_hrc) {
                    if (i.parent_name != j.name)
                        continue;

                    for (auth_3d_object_node& k : j.node) {
                        if (i.parent_node != k.name)
                            continue;

                        int32_t node_index = (int32_t)(&k - j.node.data());
                        j.childer_object_parent_node.push_back(node_index);
                        j.children_object.push_back(&i);
                        break;
                    }
                    break;
                }
            else
                for (auth_3d_object& j : object) {
                    if (i.parent_name != j.name)
                        continue;

                    j.children_object.push_back(&i);
                    break;
                }
        }
    }

    if (object_hrc.size() > 0) {
        for (auth_3d_object_hrc& i : object_hrc) {
            if (!i.parent_name.size())
                continue;

            if (i.parent_node.size())
                for (auth_3d_object_hrc& j : object_hrc) {
                    if (i.parent_name != j.name)
                        continue;

                    for (auth_3d_object_node& k : j.node) {
                        if (i.parent_node != k.name)
                            continue;

                        int32_t node_index = (int32_t)(&k - j.node.data());
                        j.childer_object_hrc_parent_node.push_back(node_index);
                        j.children_object_hrc.push_back(&i);
                        break;
                    }
                    break;
                }
            else
                for (auth_3d_object& j : object) {
                    if (i.parent_name != j.name)
                        continue;

                    j.children_object_hrc.push_back(&i);
                    break;
                }
        }
    }

    file_name = auth_file->_file_name;
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
    mat = mat4u_identity;
    enable = false;
    camera_root_update = true;
    visible = true;
    repeat = false;
    ended = false;
    left_right_reverse = false;
    once = false;
    alpha = 1.0f;
    draw_task_flags = DRAW_TASK_10000;
    chara_id = -1;
    shadow = false;
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
    event.clear();
    event.shrink_to_fit();
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
    for (auth_3d_material_list& i : material_list)
        auth_3d_material_list_restore_prev_value(&i);
    auth_3d_post_process_restore_prev_value(&post_process, rctx);

    if (state)
        if (hash == hash_murmurhash_empty) {
            data_struct* data = rctx->data;
            auth_3d_database* auth_3d_db = &data->data_ft.auth_3d_db;
            auth_3d_database_uid* db_uid = &auth_3d_db->uid[uid];
            if (db_uid->enabled) {
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

inline void auth_3d_key::reset() {
    type = (auth_3d_key_type)0;
    value = 0.0f;
    max_frame = 0.0f;
    ep_type_pre = (auth_3d_ep_type)0;
    ep_type_post = (auth_3d_ep_type)0;
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
    value = vec4u_null;
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
    visible = visibility.interpolate(frame) >= 0.99900001f;
    if (visible && (scale_value.x == 0.0f || scale_value.y == 0.0f || scale_value.z == 0.0f))
        visible = false;
}

void auth_3d_model_transform::reset() {
    mat = mat4u_identity;
    scale.reset();
    rotation.reset();
    translation.reset();
    visibility.reset();
    scale_value = vec3_null;
    rotation_value = vec3_null;
    translation_value = vec3_null;
    visible = false;
}

auth_3d_ambient::auth_3d_ambient() : flags() {
    reset();
}

auth_3d_ambient::~auth_3d_ambient() {
    reset();
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

void auth_3d_camera_root::reset() {
    interest.reset();
    model_transform.reset();
    view_point.reset();
    fov_value = 0.0f;
    interest_value = vec3_null;
    roll_value = 0.0f;
    view_point_value = vec3_null;
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

auth_3d_chara::auth_3d_chara() {
    reset();
}

auth_3d_chara::~auth_3d_chara() {
    reset();
}

void auth_3d_chara::reset() {
    model_transform.reset();
    name.clear();
    name.shrink_to_fit();
}

auth_3d_curve::auth_3d_curve() : value() {
    reset();
}

auth_3d_curve::~auth_3d_curve() {
    reset();
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

void auth_3d_dof::reset() {
    has_dof = false;
    model_transform.reset();
}

auth_3d_event::auth_3d_event() : begin(), clip_begin(),
clip_end(), end(), time_ref_scale(), type() {
    reset();
}

auth_3d_event::~auth_3d_event() {
    reset();
}

void auth_3d_event::reset() {
    begin = 0.0f;
    clip_begin = 0.0f;
    clip_end = 0.0f;
    end = 0.0f;
    name.clear();
    name.shrink_to_fit();
    param_1.clear();
    param_1.shrink_to_fit();
    ref.clear();
    ref.shrink_to_fit();
    time_ref_scale = 0.0f;
    type = (auth_3d_event_type)0;
}

auth_3d_fog::auth_3d_fog() : flags(), flags_init(), id(), density_value(),
end_value(), start_value(), color_init(), density_init(), end_init(), start_init() {
    reset();
}

auth_3d_fog::~auth_3d_fog() {
    reset();
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

    color_init = vec4u_null;
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
    ambient_init = vec4u_null;
    cone_angle_init = 0.0f;
    constant_init = 0.0f;
    diffuse_init = vec4u_null;
    drop_off_init = 0.0f;
    far_init = 0.0f;
    intensity_init = 0.0f;
    linear_init = 0.0f;
    quadratic_init = 0.0f;
    specular_init = vec4u_null;
    tone_curve_init = vec3_null;
}

auth_3d_m_object_hrc::auth_3d_m_object_hrc() {
    reset();
}

auth_3d_m_object_hrc::~auth_3d_m_object_hrc() {
    reset();
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

auth_3d_material_list::auth_3d_material_list() : flags(), flags_init(),
material(), glow_intensity_value(), incandescence_init() {
    reset();
}

auth_3d_material_list::~auth_3d_material_list() {
    reset();
}

void auth_3d_material_list::reset() {
    flags = (auth_3d_material_list_flags)0;
    flags_init = (auth_3d_material_list_flags)0;
    blend_color.reset();
    glow_intensity.reset();
    incandescence.reset();
    name.clear();
    name.shrink_to_fit();
    material = 0;
    glow_intensity_value = 0.0f;
    incandescence_init = vec4u_null;
}

auth_3d_object::auth_3d_object() : object_info(), object_hash(), reflect(), refract() {
    reset();
}

auth_3d_object::~auth_3d_object() {
    reset();
}

void auth_3d_object::reset() {
    for (auth_3d_object*& i : children_object)
        i = 0;
    children_object.clear();
    children_object.shrink_to_fit();
    for (auth_3d_object_hrc*& i : children_object_hrc)
        i = 0;
    children_object_hrc.clear();
    children_object_hrc.shrink_to_fit();
    model_transform.reset();
    morph.reset();
    object_info = ::object_info();
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

void auth_3d_object_hrc::reset() {
    for (auth_3d_object*& i : children_object)
        i = 0;
    children_object.clear();
    children_object.shrink_to_fit();
    for (auth_3d_object_hrc*& i : children_object_hrc)
        i = 0;
    children_object_hrc.clear();
    children_object_hrc.shrink_to_fit();
    childer_object_hrc_parent_node.clear();
    childer_object_hrc_parent_node.shrink_to_fit();
    childer_object_parent_node.clear();
    childer_object_parent_node .shrink_to_fit();
    mats.clear();
    mats.shrink_to_fit();
    name.clear();
    name.shrink_to_fit();
    for (auth_3d_object_node& i : node)
        i.reset();
    node.clear();
    node.shrink_to_fit();
    object_info = ::object_info();
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

void auth_3d_object_instance::reset() {
    mats.clear();
    mats.shrink_to_fit();
    model_transform.reset();
    name.clear();
    name.shrink_to_fit();
    object_bone_indices.clear();
    object_bone_indices.shrink_to_fit();
    object_info = ::object_info();
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

void auth_3d_object_model_transform::reset() {
    mat = mat4u_identity;
    mat_inner = mat4u_identity;
    mat_rot = mat4u_identity;
    translation.reset();
    rotation.reset();
    scale.reset();
    visibility.reset();
    translation_value = vec3_null;
    rotation_value = vec3_null;
    scale_value = vec3_identity;
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

void auth_3d_object_node::reset() {
    flags = (auth_3d_object_node_flags)0;
    bone_id = -1;
    joint_orient = vec3_null;
    joint_orient_mat = mat4u_identity;
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

void auth_3d_object_texture_transform::reset() {
    flags = (auth_3d_object_texture_transform_flags)0;;
    coverage_u.reset();
    coverage_v.reset();
    mat = mat4u_identity;
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
    intensity_init = vec3_null;
    lens_flare_init = 1.0f;
    lens_ghost_init = 1.0f;
    lens_shaft_init = 1.0f;
    radius_init = vec3_null;
    scene_fade_init = vec4u_null;
}

auth_3d_data_struct::auth_3d_data_struct() {

}

auth_3d_data_struct::~auth_3d_data_struct() {

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
    category_hash = hash_murmurhash_empty;    
}

auth_3d_uid_file_modern::~auth_3d_uid_file_modern() {

}

/*
void sub_140248480(string *auth_3d_data, list_int32_t *a2)
{
    list_int32_t_node *uid_file; // rcx
    list_int32_t_node *v5; // rbx
    char *v6; // rcx
    int32_t *i; // rbx
    char *v8; // rax
    size_t v9; // r8
    size_t v10; // rsi
    char *v11; // rdi
    _BYTE *v12; // rax
    _BYTE *v13; // r9
    int v14; // ecx
    char *v15; // rax
    ssize_t v16; // r9
    char *v17; // rcx
    list_int32_t_node *v18; // rdi
    list_int32_t_node *v19; // rax
    size_t v20; // rdx
    int32_t v21; // eax
    pv_db_pv *v22; // rax
    pv_db_pv_difficulty *v23; // rcx
    string_union v24; // kr00_16
    pv_item *__shifted(pv_item,8) v25; // rbx
    char *v26; // rcx
    object_info v27; // eax
    list_int32_t_node *v28; // rdi
    list_int32_t_node *v29; // rax
    size_t v30; // rdx
    pv_item *__shifted(pv_item,0x20) v31; // rbx
    char *v32; // rcx
    list_int32_t_node *v33; // rbx
    list_int32_t_node *v34; // rdx
    size_t v35; // rax
    ssize_t v36; // rax
    string *v37; // rbx
    ssize_t v38; // rax
    string *v39; // rbx
    char *v40; // rcx
    list_int32_t_node *v41; // rbx
    list_int32_t_node *v42; // rdx
    size_t v43; // rax
    char *v44; // rcx
    list_int32_t_node *v45; // rbx
    list_int32_t_node *v46; // rcx
    size_t v47; // rax
    int32_t a4; // [rsp+20h] [rbp-49h] BYREF
    __int64 v49; // [rsp+28h] [rbp-41h]
    string Dst; // [rsp+30h] [rbp-39h] BYREF
    string v51; // [rsp+30h] [rbp-39h] FORCED BYREF
    string Memory; // [rsp+50h] [rbp-19h] BYREF
    vector_old_int32_t uids; // [rsp+70h] [rbp+7h] BYREF

    v49 = -2i64;
    uid_file = a2->head->next;
    a2->head->next = a2->head;
    a2->head->prev = a2->head;
    a2->size = 0i64;
    if ( uid_file != a2->head )
    {
        do
        {
            v5 = uid_file->next;
            operator delete(uid_file);
            uid_file = v5;
        }
        while ( v5 != a2->head );
    }
    uids.begin = 0i64;
    uids.end = 0i64;
    uids.capacity_end = 0i64;
    if ( auth_3d_data->capacity < 0x10 )
        v6 = (char *)auth_3d_data;
    else
        v6 = auth_3d_data->data.ptr;
    sub_1401CBC70(v6, &uids);
    for ( i = uids.begin; i != uids.end; ++i )
    {
        v8 = auth_3d_data_get_uid_name(*i);
        Dst.capacity = 15i64;
        Dst.length = 0i64;
        Dst.data.data[0] = 0;
        if ( *v8 )
        {
            v9 = -1i64;
            do
                ++v9;
            while ( v8[v9] );
        }
        else
        {
            v9 = 0i64;
        }
        string_append_char_str(&Dst, v8, v9);
        v10 = Dst.length;
        if ( Dst.length )
        {
            v11 = (char *)&Dst;
            if ( Dst.capacity >= 0x10 )
                v11 = Dst.data.ptr;
            while ( v10 )
            {
                v12 = memchr(v11, '_', v10);
                v13 = v12;
                if ( !v12 )
                    break;
                if ( *v12 == asc_14096CDF0[0] )
                    v14 = 0;
                else
                    v14 = *v12 < asc_14096CDF0[0] ? -1 : 1;
                if ( !v14 )
                {
                    v15 = (char *)&Dst;
                    if ( Dst.capacity >= 0x10 )
                        v15 = Dst.data.ptr;
                    v16 = v13 - v15;
                    if ( v16 != -1 )
                    {
                        sub_1400215A0(&Dst, &Memory, 0i64, v16);
                        v17 = (char *)&Memory;
                        if ( Memory.capacity >= 0x10 )
                            v17 = Memory.data.ptr;
                        a4 = obj_database_get_object_set(v17);
                        if ( a4 != -1 )
                        {
                            v18 = a2->head;
                            v19 = list_int32_t_buy_node(a2, a2->head, a2->head->prev, &a4);
                            v20 = a2->size;
                            if ( v20 == 0xAAAAAAAAAAAAAA9i64 )
                                ThrowException("list<T> too long");
                            a2->size = v20 + 1;
                            v18->prev = v19;
                            v19->prev->next = v19;
                        }
                        if ( Memory.capacity >= 0x10 )
                            operator delete(Memory.data.ptr);
                    }
                    break;
                }
                v10 += v11 - v12 - 1;
                v11 = v12 + 1;
            }
        }
        if ( Dst.capacity >= 0x10 )
            operator delete(Dst.data.ptr);
    }
    if ( !string_compare_ptr_char(auth_3d_data, "ITMPV", 0i64, 5ui64) )
    {
        sub_1400215A0(auth_3d_data, &Memory, 5i64, -1i64);
        v21 = sub_1400D29A0(&Memory, 0xAu);
        v22 = task_pv_db_get_pv(v21);
        if ( v22 )
        {
            v23 = v22->difficulty[2].begin;
            if ( v22->difficulty[2].end - v23 )
            {
                v51.data = 0ui64;
                v51.length = 0i64;
                sub_14023FDE0((vector_old_pv_item *)&v51, &v23->pv_item);
                v24 = v51.data;
                if ( v51.data.ptr != *(char **)&v51.data.data[8] )
                {
                    v25 = (pv_item *__shifted(pv_item,8))(v51.data.ptr + 8);
                    do
                    {
                        if ( ADJ(v25)->index > 0 )
                        {
                            v26 = (char *)(ADJ(v25)->name.capacity < 0x10 ? v25 : ADJ(v25)->name.data.ptr);
                            v27 = obj_database_get_object_info(v26);
                            if ( v27 != -1 )
                            {
                                a4 = HIWORD(*(unsigned int *)&v27);
                                v28 = a2->head;
                                v29 = sub_14023B3E0(a2, a2->head, a2->head->prev, &a4);
                                v30 = a2->size;
                                if ( v30 == 0xAAAAAAAAAAAAAA9i64 )
                                    ThrowException("list<T> too long");
                                a2->size = v30 + 1;
                                v28->prev = v29;
                                v29->prev->next = v29;
                            }
                        }
                        ++v25;
                    }
                    while ( ADJ(v25) != *(pv_item *__shifted(pv_item,8) *)&v24.data[8] );
                }
                if ( v24.ptr )
                {
                    if ( v24.ptr != *(char **)&v24.data[8] )
                    {
                        v31 = (pv_item *__shifted(pv_item,0x20))(v24.ptr + 32);
                        do
                        {
                            if ( ADJ(v31)->name.capacity >= 0x10 )
                                operator delete(ADJ(v31)->name.data.ptr);
                            ADJ(v31)->name.capacity = 15i64;
                            ADJ(v31)->name.length = 0i64;
                            ADJ(v31++)->name.data.data[0] = 0;
                        }
                        while ( ADJ(v31) != *(pv_item *__shifted(pv_item,0x20) *)&v24.data[8] );
                    }
                    operator delete(v24.ptr);
                }
            }
        }
        if ( Memory.capacity >= 0x10 )
            operator delete(Memory.data.ptr);
    }
    if ( !string_compare_ptr_char(auth_3d_data, "EFFCHRPV", 0i64, 8ui64) )
    {
        v32 = (char *)(auth_3d_data->capacity < 0x10 ? auth_3d_data : auth_3d_data->data.ptr);
        a4 = obj_database_get_object_set(v32);
        if ( a4 != -1 )
        {
            v33 = a2->head;
            v34 = list_int32_t_buy_node(a2, a2->head, a2->head->prev, &a4);
            v35 = a2->size;
            if ( v35 == 0xAAAAAAAAAAAAAA9i64 )
                ThrowException("list<T> too long");
            a2->size = v35 + 1;
            v33->prev = v34;
            v34->prev->next = v34;
        }
    }
    sub_14023EF10(a2);
    sub_14023F860(a2);
    v51.capacity = 15i64;
    v51.length = 0i64;
    v51.data.data[0] = 0;
    string_append_char_str(&v51, null_string, 0i64);
    v36 = string_compare_ptr_char(auth_3d_data, "STGPV", 0i64, 5ui64);
    if ( v36 != -1 )
    {
        v37 = sub_1400215A0(auth_3d_data, &Memory, v36, 8i64);
        if ( &v51 != v37 )
        {
            if ( v51.capacity >= 0x10 )
                operator delete(v51.data.ptr);
            v51.capacity = 15i64;
            v51.length = 0i64;
            v51.data.data[0] = 0;
            if ( v37->capacity >= 0x10 )
            {
                v51.data.ptr = v37->data.ptr;
                v37->data.ptr = 0i64;
            }
            else if ( v37->length != -1i64 )
            {
                memmove(&v51, v37, v37->length + 1);
            }
            v51.length = v37->length;
            v51.capacity = v37->capacity;
            v37->capacity = 15i64;
            v37->length = 0i64;
            v37->data.data[0] = 0;
        }
        if ( Memory.capacity >= 0x10 )
            operator delete(Memory.data.ptr);
    }
    v38 = string_compare_ptr_char(auth_3d_data, "STGD2PV", 0i64, 7ui64);
    if ( v38 != -1 )
    {
        v39 = sub_1400215A0(auth_3d_data, &Memory, v38, 10i64);
        if ( &v51 != v39 )
        {
            if ( v51.capacity >= 0x10 )
                operator delete(v51.data.ptr);
            v51.capacity = 15i64;
            v51.length = 0i64;
            v51.data.data[0] = 0;
            if ( v39->capacity >= 0x10 )
            {
                v51.data.ptr = v39->data.ptr;
                v39->data.ptr = 0i64;
            }
            else if ( v39->length != -1i64 )
            {
                memmove(&v51, v39, v39->length + 1);
            }
            v51.length = v39->length;
            v51.capacity = v39->capacity;
            v39->capacity = 15i64;
            v39->length = 0i64;
            v39->data.data[0] = 0;
        }
        if ( Memory.capacity >= 0x10 )
            operator delete(Memory.data.ptr);
    }
    if ( v51.length )
    {
        v40 = (char *)&v51;
        if ( v51.capacity >= 0x10 )
            v40 = v51.data.ptr;
        a4 = obj_database_get_object_set(v40);
        if ( a4 != -1 )
        {
            v41 = a2->head->next;
            v42 = list_int32_t_buy_node(a2, v41, v41->prev, &a4);
            v43 = a2->size;
            if ( v43 == 0xAAAAAAAAAAAAAA9i64 )
                ThrowException("list<T> too long");
            a2->size = v43 + 1;
            v41->prev = v42;
            v42->prev->next = v42;
        }
    }
    sub_14000BF30(&Memory, &v51, "HRC");
    v44 = (char *)&Memory;
    if ( Memory.capacity >= 0x10 )
        v44 = Memory.data.ptr;
    a4 = obj_database_get_object_set(v44);
    if ( a4 != -1 )
    {
        v45 = a2->head;
        v46 = list_int32_t_buy_node(a2, a2->head, a2->head->prev, &a4);
        v47 = a2->size;
        if ( 0xAAAAAAAAAAAAAA9i64 == v47 )
            ThrowException("list<T> too long");
        a2->size = v47 + 1;
        v45->prev = v46;
        v46->prev->next = v46;
    }
    if ( Memory.capacity >= 0x10 )
        operator delete(Memory.data.ptr);
    if ( v51.capacity >= 0x10 )
        operator delete(v51.data.ptr);
    if ( uids.begin )
        operator delete(uids.begin);
}
*/

void auth_3d_data_init() {
    auth_3d_data = new auth_3d_data_struct;
}

bool auth_3d_data_check_id_not_empty(int32_t* id) {
    if (*id >= 0 && ((*id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[*id & 0x7FFF];
        if (auth->id == *id)
            return auth->id != -1;
    }
    return false;
}

bool auth_3d_data_check_category_loaded(const char* category_name) {
    return auth_3d_data_struct_check_category_loaded(auth_3d_data, category_name);
}

bool auth_3d_data_check_category_loaded(uint32_t category_hash) {
    return auth_3d_data_struct_check_category_loaded(auth_3d_data, category_hash);
}

auth_3d* auth_3d_data_get_auth_3d(int32_t id) {
    int32_t index = 0;
    while (auth_3d_data->data[index].id != id)
        if (++index >= AUTH_3D_DATA_COUNT)
            return 0;

    return &auth_3d_data->data[index];
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
        if (auth->id != i || !auth->enable
            || (file_name_hash != hash_murmurhash_empty && auth->hash != file_name_hash))
            continue;

        int32_t obj_hrc_index = auth_3d_get_auth_3d_object_hrc_index_by_hash(
            auth, object_hash, instance);
        if (obj_hrc_index >= 0) {
            if (object_index)
                *object_index = obj_hrc_index;
            if (hrc)
                *hrc = true;
            return i;
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
    }
    return -1;
}

int32_t auth_3d_data_get_chara_id(int32_t id) {
    auth_3d* auth = auth_3d_data_get_auth_3d(id);
    if (auth)
        return auth->chara_id;
    return -1;
}

mat4* auth_3d_data_get_auth_3d_object_mat(int32_t id, size_t index, bool hrc, mat4* mat) {
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

bool auth_3d_data_get_enable(int32_t* id) {
    if (*id >= 0 && ((*id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[*id & 0x7FFF];
        if (auth->id == *id)
            return auth->enable;
    }
    return true;
}

bool auth_3d_data_get_ended(int32_t* id) {
    if (*id >= 0 && ((*id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[*id & 0x7FFF];
        if (auth->id == *id)
            return auth->ended;
    }
    return true;
}

float_t auth_3d_data_get_frame(int32_t* id) {
    if (*id >= 0 && ((*id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[*id & 0x7FFF];
        if (auth->id == *id)
            return auth->frame;
    }
    return 0.0f;
}

bool auth_3d_data_get_left_right_reverse(int32_t* id) {
    if (*id >= 0 && ((*id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[*id & 0x7FFF];
        if (auth->id == *id)
            return auth->left_right_reverse;
    }
    return true;
}

bool auth_3d_data_get_paused(int32_t* id) {
    if (*id >= 0 && ((*id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[*id & 0x7FFF];
        if (auth->id == *id)
            return auth->paused;
    }
    return true;
}

bool auth_3d_data_get_repeat(int32_t* id) {
    if (*id >= 0 && ((*id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[*id & 0x7FFF];
        if (auth->id == *id)
            return auth->repeat;
    }
    return true;
}

float_t auth_3d_data_get_play_control_begin(int32_t* id) {
    if (*id >= 0 && ((*id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[*id & 0x7FFF];
        if (auth->id == *id)
            return auth->play_control.begin;
    }
    return 0.0f;
}

float_t auth_3d_data_get_play_control_size(int32_t* id) {
    if (*id >= 0 && ((*id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[*id & 0x7FFF];
        if (auth->id == *id)
            return auth->play_control.size;
    }
    return 0.0f;
}

void auth_3d_data_load_auth_3d_db(auth_3d_database* auth_3d_db) {
    auth_3d_data->farcs.clear();
    auth_3d_data->farcs.resize(auth_3d_db->category.size());

    size_t cat_index = 0;
    for (auth_3d_farc& i : auth_3d_data->farcs)
        i.name = auth_3d_db->category[cat_index++].name;

    auth_3d_data->uid_files.clear();
    auth_3d_data->uid_files.resize(auth_3d_db->uid.size());

    size_t uid_index = 0;
    for (auth_3d_uid_file& i : auth_3d_data->uid_files)
        i.uid = (int32_t)(uid_index++);
}

int32_t auth_3d_data_get_uid(int32_t* id) {
    if (*id >= 0 && ((*id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[*id & 0x7FFF];
        if (auth->id == *id)
            return auth->uid;
    }
    return -1;
}

void auth_3d_data_load_category(const char* category_name) {
    auth_3d_data_struct_load_category(auth_3d_data, category_name, 0);
}

void auth_3d_data_load_category(void* data, const char* category_name, uint32_t category_hash) {
    auth_3d_data_struct_load_category(auth_3d_data, data, category_name, category_hash);
}

int32_t auth_3d_data_load_hash(uint32_t hash, uint32_t category_hash,
    const char* category_name, void* data, object_database* obj_db, texture_database* tex_db) {
    auto elem = auth_3d_data->uid_files_modern.find(hash);
    if (elem == auth_3d_data->uid_files_modern.end())
        elem = auth_3d_data->uid_files_modern.insert({ hash, {} }).first;

    auth_3d_uid_file_modern* uid_file = &elem->second;
    uid_file->load_count = 0;
    uid_file->hash = hash;
    uid_file->category_hash = category_hash;
    uid_file->file_name = {};
    uid_file->state = 0;
    uid_file->name = {};
    uid_file->farc = {};
    uid_file->data = data;
    uid_file->obj_db = obj_db;
    uid_file->tex_db = tex_db;

    int32_t index = 0;
    while (auth_3d_data->data[index].uid != -1 || auth_3d_data->data[index].hash
        != hash_murmurhash_empty && auth_3d_data->data[index].hash != -1)
        if (++index >= AUTH_3D_DATA_COUNT)
            return -1;

    auth_3d_data->data[index].reset();

    if (++auth_3d_load_counter < 0)
        auth_3d_load_counter = 1;

    int32_t id = ((auth_3d_load_counter & 0x7FFF) << 16) | index & 0x7FFF;
    auth_3d_data->data[index].hash = hash;
    auth_3d_data->data[index].id = id;
    auth_3d_data->loaded_ids.push_back(id);
    return id;
}

int32_t auth_3d_data_load_uid(int32_t uid, auth_3d_database* auth_3d_db) {
    if (uid >= auth_3d_db->uid.size() || !auth_3d_db->uid[uid].enabled)
        return -1;

    int32_t index = 0;
    while (auth_3d_data->data[index].uid != -1 || auth_3d_data->data[index].hash
        != hash_murmurhash_empty && auth_3d_data->data[index].hash != -1)
        if (++index >= AUTH_3D_DATA_COUNT)
            return -1;

    auth_3d_data->data[index].reset();

    if (++auth_3d_load_counter < 0)
        auth_3d_load_counter = 1;

    int32_t id = ((auth_3d_load_counter & 0x7FFF) << 16) | index & 0x7FFF;
    auth_3d_data->data[index].uid = uid;
    auth_3d_data->data[index].id = id;
    auth_3d_data->loaded_ids.push_back(id);
    return id;
}

void auth_3d_data_read_file(int32_t* id, auth_3d_database* auth_3d_db) {
    if (*id >= 0 && ((*id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[*id & 0x7FFF];
        if (auth && auth->id == *id)
            auth_3d_read_file(auth, auth_3d_db);
    }
}

void auth_3d_data_read_file_modern(int32_t* id) {
    if (*id >= 0 && ((*id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[*id & 0x7FFF];
        if (auth && auth->id == *id)
            auth_3d_read_file_modern(auth);
    }
}

void auth_3d_data_set_camera_root_update(int32_t* id, bool value) {
    if (*id >= 0 && ((*id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[*id & 0x7FFF];
        if (auth->id == *id)
            auth->camera_root_update = value;
    }
}

void auth_3d_data_set_chara_id(int32_t* id, int32_t value) {
    if (*id >= 0 && ((*id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[*id & 0x7FFF];
        if (auth->id == *id)
            auth->chara_id = value;
    }
}

void auth_3d_data_set_enable(int32_t* id, bool value) {
    if (*id >= 0 && ((*id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[*id & 0x7FFF];
        if (auth->id == *id)
            auth->enable = value;
    }
}

void auth_3d_data_set_frame_rate(int32_t* id, FrameRateControl* value) {
    if (*id >= 0 && ((*id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[*id & 0x7FFF];
        if (auth->id == *id)
            if (value)
                auth->frame_rate = value;
            else
                auth->frame_rate = &sys_frame_rate;
    }
}

void auth_3d_data_set_last_frame(int32_t* id, float_t value) {
    if (*id >= 0 && ((*id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[*id & 0x7FFF];
        if (auth->id == *id)
            auth->last_frame = value;
    }
}

void auth_3d_data_set_left_right_reverse(int32_t* id, bool value) {
    if (*id >= 0 && ((*id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[*id & 0x7FFF];
        if (auth->id == *id)
            auth->left_right_reverse = value;
    }
}

void auth_3d_data_set_mat(int32_t* id, mat4* value) {
    if (*id >= 0 && ((*id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[*id & 0x7FFF];
        if (auth->id == *id)
            auth->mat = *value;
    }
}

void auth_3d_data_set_max_frame(int32_t* id, float_t value) {
    if (*id >= 0 && ((*id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[*id & 0x7FFF];
        if (auth->id == *id)
            auth->max_frame = value;
    }
}

void auth_3d_data_set_paused(int32_t* id, bool value) {
    if (*id >= 0 && ((*id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[*id & 0x7FFF];
        if (auth->id == *id)
            auth->paused = value;
    }
}

void auth_3d_data_set_repeat(int32_t* id, bool value) {
    if (*id >= 0 && ((*id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[*id & 0x7FFF];
        if (auth->id == *id)
            auth->repeat = value;
    }
}

void auth_3d_data_set_req_frame(int32_t* id, float_t value) {
    if (*id >= 0 && ((*id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[*id & 0x7FFF];
        if (auth->id == *id) {
            auth->req_frame = value;
            auth->frame_changed = true;
        }
    }
}

void auth_3d_data_set_shadow(int32_t* id, bool value) {
    if (*id >= 0 && ((*id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[*id & 0x7FFF];
        if (auth->id == *id)
            auth->shadow = value;
    }
}

void auth_3d_data_set_visibility(int32_t* id, bool value) {
    if (*id >= 0 && ((*id & 0x7FFF) < AUTH_3D_DATA_COUNT)) {
        auth_3d* auth = &auth_3d_data->data[*id & 0x7FFF];
        if (auth->id == *id)
            auth->visible = value;
    }
}

void auth_3d_data_unload_category(const char* category_name) {
    auth_3d_data_struct_unload_category(auth_3d_data, category_name);
}

void auth_3d_data_unload_category(uint32_t category_hash) {
    auth_3d_data_struct_unload_category(auth_3d_data, category_hash);
}

void task_auth_3d_append_task() {
    TaskWork::AppendTask(&task_auth_3d, "AUTH_3D");
}

void auth_3d_data_unload_id(int32_t id, render_context* rctx) {
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

void auth_3d_data_free() {
    delete auth_3d_data;
}

auth_3d_detail::TaskAuth3d::TaskAuth3d() {

}

auth_3d_detail::TaskAuth3d:: ~TaskAuth3d() {

}

bool auth_3d_detail::TaskAuth3d::Init() {
    return auth_3d_data_struct_init(auth_3d_data, rctx_ptr);
}

bool auth_3d_detail::TaskAuth3d::Ctrl() {
    return auth_3d_data_struct_ctrl(auth_3d_data, rctx_ptr);
}

void auth_3d_detail::TaskAuth3d::Disp() {
    auth_3d_data_struct_disp(auth_3d_data, rctx_ptr);
}

static bool auth_3d_key_detect_fast_change(auth_3d_key* data, float_t frame, float_t threshold) {
    if (data->type < AUTH_3D_KEY_LINEAR || data->type > AUTH_3D_KEY_HOLD)
        return false;

    frame = clamp(frame, 0.0f, data->max_frame);
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
        if (curr_key->frame < next_key->frame) {
            float_t df = next_key->frame - curr_key->frame;
            return interpolate_chs_value(curr_key->value, next_key->value,
                curr_key->tangent2, next_key->tangent1,
                curr_key->frame, next_key->frame, frame);
        }
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
        k->keys_vec = kf->keys;
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
        float_t value = kf->keys[0].value;
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

static void auth_3d_model_transform_set_mat(auth_3d_model_transform* mt, mat4* parent_mat) {
    mat4 mat;
    mat4_translate_mult(parent_mat, mt->translation_value.x,
        mt->translation_value.y, mt->translation_value.z, &mat);
    mat4_rotate_mult(&mat, mt->rotation_value.x, mt->rotation_value.y, mt->rotation_value.z, &mat);
    mat4_scale_rot(&mat, mt->scale_value.x, mt->scale_value.y, mt->scale_value.z, &mat);
    mt->mat = mat;
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
    int32_t obj_instance = 0;
    for (auth_3d_object_hrc& i : auth->object_hrc)
        if (obj_info == i.object_info) {
            if (obj_instance == instance)
                return (int32_t)(&i - auth->object_hrc.data());
            obj_instance++;
        }
    return -1;
}

static int32_t auth_3d_get_auth_3d_object_hrc_index_by_hash(auth_3d* auth,
    uint32_t object_hash, int32_t instance) {
    int32_t obj_instance = 0;
    for (auth_3d_object_hrc& i : auth->object_hrc)
        if (object_hash == i.object_hash) {
            if (obj_instance == instance)
                return (int32_t)(&i - auth->object_hrc.data());
            obj_instance++;
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

static void auth_3d_ambient_ctrl(auth_3d_ambient* a, float_t frame) {
    if (a->flags & AUTH_3D_AMBIENT_LIGHT_DIFFUSE)
        a->light_diffuse.interpolate(frame);

    if (a->flags & AUTH_3D_AMBIENT_LIGHT_DIFFUSE)
        a->rim_light_diffuse.interpolate(frame);
}

static void auth_3d_ambient_load(auth_3d* auth, auth_3d_ambient* a, auth_3d_ambient_file* af) {
    if (af->flags & A3DA_AMBIENT_LIGHT_DIFFUSE) {
        auth_3d_rgba_load(auth, &a->light_diffuse, &af->light_diffuse);
        enum_or(a->flags, AUTH_3D_AMBIENT_LIGHT_DIFFUSE);
    }

    a->name = af->name;

    if (af->flags & A3DA_AMBIENT_LIGHT_DIFFUSE) {
        auth_3d_rgba_load(auth, &a->rim_light_diffuse, &af->rim_light_diffuse);
        enum_or(a->flags, AUTH_3D_AMBIENT_LIGHT_DIFFUSE);
    }
}

static void auth_3d_ambient_set(auth_3d_ambient* a, render_context* rctx) {

}

void auth_3d_camera_auxiliary_ctrl(auth_3d_camera_auxiliary* ca, float_t frame) {
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_AUTO_EXPOSURE)
        ca->auto_exposure_value = ca->auto_exposure.interpolate(frame);
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_EXPOSURE)
        ca->exposure_value = ca->exposure.interpolate(frame);
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_EXPOSURE_RATE)
        ca->exposure_rate_value = ca->exposure_rate.interpolate(frame);
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_GAMMA)
        ca->gamma_value = ca->gamma.interpolate(frame);
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_GAMMA_RATE)
        ca->gamma_rate_value = ca->gamma_rate.interpolate(frame);
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_SATURATE)
        ca->saturate_value = ca->saturate.interpolate(frame);
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
        post_process_tone_map_set_auto_exposure(tm, true);
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_EXPOSURE)
        post_process_tone_map_set_exposure(tm, 1.0);
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_GAMMA)
        post_process_tone_map_set_gamma(tm, 1.0);
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_GAMMA_RATE)
        post_process_tone_map_set_gamma_rate(tm, 1.0);
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_SATURATE)
        post_process_tone_map_set_saturate_coeff(tm, 1.0);
}

static void auth_3d_camera_auxiliary_set(auth_3d_camera_auxiliary* ca, render_context* rctx) {
    post_process_tone_map* tm = rctx->post_process.tone_map;

    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_AUTO_EXPOSURE)
        post_process_tone_map_set_auto_exposure(tm, ca->auto_exposure_value > 0.0);
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_EXPOSURE)
        post_process_tone_map_set_exposure(tm, ca->exposure_value);
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_GAMMA)
        post_process_tone_map_set_gamma(tm, ca->gamma_value);
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_GAMMA_RATE)
        post_process_tone_map_set_gamma_rate(tm, ca->gamma_rate_value);
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_SATURATE)
        post_process_tone_map_set_saturate_coeff(tm, ca->saturate_value);
}

static void auth_3d_camera_root_ctrl(auth_3d_camera_root* cr,
    float_t frame, mat4* mat, render_context* rctx) {
    camera* cam = rctx->camera;
    if (auth_3d_key_detect_fast_change(&cr->view_point.model_transform.translation.x, frame, 0.3f)
        || auth_3d_key_detect_fast_change(&cr->view_point.model_transform.translation.y, frame, 0.3f)
        || auth_3d_key_detect_fast_change(&cr->view_point.model_transform.translation.z, frame, 0.3f)
        || auth_3d_key_detect_fast_change(&cr->interest.translation.x, frame, 0.3f)
        || auth_3d_key_detect_fast_change(&cr->interest.translation.y, frame, 0.3f)
        || auth_3d_key_detect_fast_change(&cr->interest.translation.z, frame, 0.3f)) {
        frame = (float_t)(int32_t)frame;
        camera_set_fast_change(cam, true);
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
                camera_set_fast_change_hist0(cam, true);
        }
    }

    cr->model_transform.interpolate(frame);
    auth_3d_model_transform_set_mat(&cr->model_transform, (mat4*)&mat4_identity);

    mat4 cr_mat = cr->model_transform.mat;

    vec3 view_point = cr->view_point.model_transform.translation.interpolate(frame);
    mat4_mult_vec3_trans(&cr_mat, &view_point, &cr->view_point_value);

    vec3 interest = cr->interest.translation.interpolate(frame);
    mat4_mult_vec3_trans(&cr_mat, &interest, &cr->interest_value);

    float_t fov;
    if (cr->view_point.flags & AUTH_3D_CAMERA_ROOT_VIEW_POINT_FOV) {
        fov = cr->view_point.fov.interpolate(frame);
        if (cr->view_point.fov_is_horizontal)
            fov = atanf(tanf(fov * 0.5f) / cr->view_point.aspect) * 2.0f;
    }
    else {
        float_t camera_aperture_h = cr->view_point.camera_aperture_h;
        float_t focal_lenth = cr->view_point.focal_length.interpolate(frame);
        fov = atanf((camera_aperture_h * 25.4f) * 0.5f / focal_lenth) * 2.0f;
    }
    cr->fov_value = fov;

    if (cr->view_point.flags & AUTH_3D_CAMERA_ROOT_VIEW_POINT_ROLL)
        cr->roll_value = cr->view_point.roll.interpolate(frame);
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

static void auth_3d_chara_ctrl(auth_3d_chara* c, float_t frame) {
    c->model_transform.interpolate(frame);
}

static void auth_3d_chara_load(auth_3d* auth, auth_3d_chara* c, auth_3d_chara_file* cf) {
    auth_3d_model_transform_load(auth, &c->model_transform, &cf->model_transform);
    c->name = cf->name;
}

static void auth_3d_curve_ctrl(auth_3d_curve* c, float_t frame) {
    c->value = c->curve.interpolate(frame);
}

static void auth_3d_curve_load(auth_3d* auth, auth_3d_curve* c, auth_3d_curve_file* cf) {
    auth_3d_key_load(auth, &c->curve, &cf->curve);
    c->name = cf->name;
}

static void auth_3d_dof_ctrl(auth_3d_dof* d, float_t frame) {
    if (!d->has_dof)
        return;

    d->model_transform.interpolate(frame);
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

    post_process_dof_set_dof_pv(rctx->post_process.dof, 0);
}

static void auth_3d_dof_set(auth_3d_dof* d, render_context* rctx) {
    if (!d->has_dof)
        return;

    vec3 view_point;
    camera_get_view_point(rctx->camera, &view_point);

    float_t focus;
    vec3_distance(d->model_transform.translation_value, view_point, focus);

    dof_pv pv;
    pv.enable = fabsf(d->model_transform.rotation_value.z) > 0.000001f;
    pv.f2.distance_to_focus = focus;
    pv.f2.focus_range = d->model_transform.scale_value.y;
    pv.f2.fuzzing_range = d->model_transform.rotation_value.y;
    pv.f2.ratio = d->model_transform.rotation_value.z;
    post_process_dof_set_dof_pv(rctx->post_process.dof, &pv);
}

static void auth_3d_event_load(auth_3d* auth, auth_3d_event* e, auth_3d_event_file* ef) {
    e->begin = ef->begin;
    e->clip_begin = ef->clip_begin;
    e->clip_end = ef->clip_end;
    e->end = ef->end;
    e->name = ef->name;
    e->param_1 = ef->param_1;
    e->ref = ef->ref;
    e->time_ref_scale = ef->time_ref_scale;
    switch (ef->type) {
    case A3DA_EVENT_MISC:
    default:
        e->type = AUTH_3D_EVENT_MISC;
    case A3DA_EVENT_FILT:
        e->type = AUTH_3D_EVENT_FILT;
    case A3DA_EVENT_FX:
        e->type = AUTH_3D_EVENT_FX;
    case A3DA_EVENT_SND:
        e->type = AUTH_3D_EVENT_SND;
    case A3DA_EVENT_MOT:
        e->type = AUTH_3D_EVENT_MOT;
    case A3DA_EVENT_A2D:
        e->type = AUTH_3D_EVENT_A2D;
    }
}

static void auth_3d_fog_ctrl(auth_3d_fog* f, float_t frame) {
    if (f->flags & AUTH_3D_FOG_DENSITY)
        f->density_value = f->density.interpolate(frame);
    if (f->flags & AUTH_3D_FOG_START)
        f->start_value = f->start.interpolate(frame);
    if (f->flags & AUTH_3D_FOG_END)
        f->end_value = f->end.interpolate(frame);
    if (f->flags & AUTH_3D_FOG_COLOR)
        f->color.interpolate(frame);
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

    fog* data = &rctx->fog_data[id];
    if (f->flags_init & AUTH_3D_FOG_COLOR) {
        vec4 color = f->color_init;
        fog_set_color(data, &color);
    }

    if (f->flags_init & AUTH_3D_FOG_DENSITY)
        fog_set_density(data, f->density_value);

    if (f->flags_init & AUTH_3D_FOG_END)
        fog_set_end(data, f->end_value);

    if (f->flags_init & AUTH_3D_FOG_START)
        fog_set_start(data, f->start_value);
}

static void auth_3d_fog_set(auth_3d_fog* f, render_context* rctx) {
    fog_id id = f->id;
    if (id < FOG_DEPTH || id > FOG_BUMP)
        return;

    fog* data = &rctx->fog_data[id];
    if (f->flags & AUTH_3D_FOG_COLOR) {
        if (~f->flags_init & AUTH_3D_FOG_COLOR) {
            vec4 color_init;
            fog_get_color(data, &color_init);
            f->color_init = color_init;
            enum_or(f->flags_init, AUTH_3D_FOG_COLOR);
        }

        vec4 color = f->color.value;
        fog_set_color(data, &color);
    }

    if (f->flags & AUTH_3D_FOG_DENSITY) {
        if (~f->flags_init & AUTH_3D_FOG_DENSITY) {
            f->density_init = fog_get_density(data);
            enum_or(f->flags_init, AUTH_3D_FOG_DENSITY);
        }

        fog_set_density(data, f->density_value);
    }

    if (f->flags & AUTH_3D_FOG_END) {
        if (~f->flags_init & AUTH_3D_FOG_END) {
            f->end_init = fog_get_end(data);
            enum_or(f->flags_init, AUTH_3D_FOG_END);
        }

        fog_set_end(data, f->end_value);
    }

    if (f->flags & AUTH_3D_FOG_START) {
        if (~f->flags_init & AUTH_3D_FOG_START) {
            f->start_init = fog_get_start(data);
            enum_or(f->flags_init, AUTH_3D_FOG_START);
        }

        fog_set_start(data, f->start_value);
    }
}

static void auth_3d_light_ctrl(auth_3d_light* l, float_t frame) {
    if (l->flags & AUTH_3D_LIGHT_AMBIENT)
        l->ambient.interpolate(frame);
    if (l->flags & AUTH_3D_LIGHT_CONE_ANGLE)
        l->cone_angle_value = l->cone_angle.interpolate(frame);
    if (l->flags & AUTH_3D_LIGHT_CONSTANT)
        l->constant_value = l->constant.interpolate(frame);
    if (l->flags & AUTH_3D_LIGHT_DIFFUSE)
        l->diffuse.interpolate(frame);
    if (l->flags & AUTH_3D_LIGHT_DROP_OFF)
        l->drop_off_value = l->drop_off.interpolate(frame);
    if (l->flags & AUTH_3D_LIGHT_FAR)
        l->far_value = l->_far.interpolate(frame);
    if (l->flags & AUTH_3D_LIGHT_INTENSITY)
        l->intensity_value = l->intensity.interpolate(frame);
    if (l->flags & AUTH_3D_LIGHT_LINEAR)
        l->linear_value = l->linear.interpolate(frame);
    if (l->flags & AUTH_3D_LIGHT_POSITION)
        l->position.interpolate(frame);
    if (l->flags & AUTH_3D_LIGHT_QUADRATIC)
        l->quadratic_value = l->quadratic.interpolate(frame);
    if (l->flags & AUTH_3D_LIGHT_SPECULAR)
        l->specular.interpolate(frame);
    if (l->flags & AUTH_3D_LIGHT_SPOT_DIRECTION)
        l->spot_direction.interpolate(frame);
    if (l->flags & AUTH_3D_LIGHT_TONE_CURVE)
        l->tone_curve.interpolate(frame);

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

    l->type = lf->type;
}

static void auth_3d_light_restore_prev_value(auth_3d_light* l, render_context* rctx) {
    light_set* set_data = &rctx->light_set_data[LIGHT_SET_MAIN];

    light_id id = l->id;
    if (id < LIGHT_CHARA || id > LIGHT_PROJECTION)
        return;

    light_data* data = &set_data->lights[id];
    if (l->flags_init & AUTH_3D_LIGHT_AMBIENT) {
        vec4 ambient_init = l->ambient_init;
        light_set_ambient(data, &ambient_init);
    }

    if (l->flags_init & AUTH_3D_LIGHT_DIFFUSE) {
        vec4 diffuse_init = l->diffuse_init;
        light_set_diffuse(data, &diffuse_init);
    }

    if (l->flags_init & AUTH_3D_LIGHT_SPECULAR) {
        vec4 specular_init = l->specular_init;
        light_set_specular(data, &specular_init);
    }

    if (l->flags_init & AUTH_3D_LIGHT_TONE_CURVE)
        light_set_tone_curve(data, &l->tone_curve_init);
}

static void auth_3d_light_set(auth_3d_light* l, render_context* rctx) {
    light_set* set_data = &rctx->light_set_data[LIGHT_SET_MAIN];

    light_id id = l->id;
    if (id < LIGHT_CHARA || id > LIGHT_PROJECTION)
        return;

    light_data* data = &set_data->lights[id];
    if (l->flags & AUTH_3D_LIGHT_AMBIENT) {
        if (~l->flags_init & AUTH_3D_LIGHT_AMBIENT) {
            vec4 ambient_init;
            light_get_ambient(data, &ambient_init);
            l->ambient_init = ambient_init;
            enum_or(l->flags_init, AUTH_3D_LIGHT_AMBIENT);
        }

        vec4 ambient = l->ambient.value;
        light_set_ambient(data, &ambient);
    }

    if (l->flags & AUTH_3D_LIGHT_DIFFUSE) {
        if (~l->flags_init & AUTH_3D_LIGHT_DIFFUSE) {
            vec4 diffuse_init;
            light_get_diffuse(data, &diffuse_init);
            l->diffuse_init = diffuse_init;
            enum_or(l->flags_init, AUTH_3D_LIGHT_DIFFUSE);
        }

        vec4 diffuse = l->diffuse.value;
        light_set_diffuse(data, &diffuse);
    }

    if (l->flags & AUTH_3D_LIGHT_POSITION)
        light_set_position(data, &l->position.translation_value);

    if (l->flags & AUTH_3D_LIGHT_SPECULAR) {
        if (~l->flags_init & AUTH_3D_LIGHT_SPECULAR) {
            vec4 specular_init;
            light_get_specular(data, &specular_init);
            l->specular_init = specular_init;
            enum_or(l->flags_init, AUTH_3D_LIGHT_SPECULAR);
        }

        vec4 specular = l->specular.value;
        light_set_specular(data, &specular);
    }

    if (l->flags & AUTH_3D_LIGHT_SPOT_DIRECTION)
        light_set_spot_direction(data, &l->spot_direction.translation_value);

    if (l->flags & AUTH_3D_LIGHT_TONE_CURVE) {
        if (~l->flags_init & AUTH_3D_LIGHT_TONE_CURVE) {
            light_get_tone_curve(data, &l->tone_curve_init);
            enum_or(l->flags_init, AUTH_3D_LIGHT_TONE_CURVE);
        }

        light_set_tone_curve(data, (vec3*)&l->tone_curve.value);
    }
}

static void auth_3d_m_object_hrc_get_mat(auth_3d_m_object_hrc* moh, mat4* mat) {
    for (auth_3d_object_node& i : moh->node)
        if (i.mat) {
            *mat = *i.mat;
            return;
        }

    *mat = moh->node[0].model_transform.mat;
}

static void auth_3d_m_object_hrc_ctrl(auth_3d_m_object_hrc* moh, float_t frame) {
    auth_3d_object_model_transform_ctrl(&moh->model_transform, frame);
    auth_3d_object_model_transform_set_mat_inner(&moh->model_transform);

    for (auth_3d_object_instance& i : moh->instance) {
        auth_3d_object_model_transform_ctrl(&i.model_transform, frame);
        auth_3d_object_model_transform_set_mat_inner(&i.model_transform);
    }

    for (auth_3d_object_node& i : moh->node) {
        auth_3d_object_model_transform_ctrl(&i.model_transform, frame);
        auth_3d_object_model_transform_set_mat_inner(&i.model_transform);
    }
}

static void auth_3d_m_object_hrc_load(auth_3d* auth, auth_3d_m_object_hrc* moh,
    auth_3d_m_object_hrc_file* mohf, object_database* obj_db) {
    size_t node_index = 0;
    moh->node = std::vector<auth_3d_object_node>(mohf->node.size());
    for (auth_3d_object_node_file& i : mohf->node)
        auth_3d_object_node_load(auth,
            &moh->node[node_index++], &i);

    auth_3d_object_model_transform_load(auth, &moh->model_transform, &mohf->model_transform);
    moh->name = mohf->name;

    size_t instance_index = 0;
    moh->instance = std::vector<auth_3d_object_instance>(mohf->instance.size());
    for (auth_3d_object_instance_file& i : mohf->instance)
        auth_3d_object_instance_load(auth,
            &moh->instance[instance_index++], &i, moh, obj_db);
}

static void auth_3d_m_object_hrc_disp(auth_3d_m_object_hrc* moh, auth_3d* auth, render_context* rctx) {
    if (!auth->visible || !moh->model_transform.visible)
        return;

    object_data* object_data = &rctx->object_data;
    object_database* obj_db = auth->obj_db;
    texture_database* tex_db = auth->tex_db;

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

        object_data_set_draw_task_flags(object_data, (draw_task_flags)flags);
        object_data_set_shadow_type(object_data, shadow_type);

        shadow* shad = rctx->draw_pass.shadow_ptr;
        if (shad && flags & DRAW_TASK_SHADOW) {
            object_data_set_shadow_type(object_data, SHADOW_STAGE);

            mat4u* m = &moh->model_transform.mat;
            for (auth_3d_object_node& j : moh->node)
                if (j.mat) {
                    m = (mat4u*)j.mat;
                    break;
                }

            mat4 mat = i.model_transform.mat;
            mat4 t = *m;
            mat4_mult(&mat, &t, &mat);

            vec3 pos = *(vec3*)&mat.row3;
            pos.y -= 0.2f;
            vector_old_vec3_push_back(&shad->field_1D0[shadow_type], &pos);
        }

        if (i.mats.size() > 0) {
            mat4 mat = i.model_transform.mat;
            draw_task_add_draw_object_by_object_info_object_skin(rctx,
                i.object_info, 0, 0, auth->alpha, i.mats.data(), 0, 0, &mat);
        }
    }

    object_data_set_draw_task_flags(object_data, (draw_task_flags)0);
    object_data_set_shadow_type(object_data, SHADOW_CHARA);
}

static void auth_3d_m_object_hrc_list_ctrl(auth_3d_m_object_hrc* moh, mat4* parent_mat) {
    if (!moh->model_transform.visible)
        return;

    auth_3d_object_node* node = moh->node.data();
    size_t instance_count = moh->instance.size();

    auth_3d_m_object_hrc_nodes_mat_mult(moh);

    auth_3d_object_model_transform_mat_mult(&moh->model_transform, parent_mat);

    mat4 mat =  moh->model_transform.mat;

    for (auth_3d_object_instance& i : moh->instance) {
        if (!i.model_transform.visible)
            continue;

        auth_3d_object_model_transform_mat_mult(&i.model_transform, &mat);
        int32_t* object_bone_indices = i.object_bone_indices.data();
        size_t object_bone_indices_count = i.object_bone_indices.size();
        mat4* mats = i.mats.data();
        for (size_t j = 0; j < object_bone_indices_count; j++)
            mats[j] = node[object_bone_indices[j]].model_transform.mat;
    }
}

static void auth_3d_m_object_hrc_nodes_mat_mult(auth_3d_m_object_hrc* moh) {
    auth_3d_object_node* node = moh->node.data();

    for (auth_3d_object_node& i : moh->node)
        if (i.parent >= 0) {
            mat4 mat = node[i.parent].model_transform.mat;
            auth_3d_object_model_transform_mat_mult(&i.model_transform, &mat);
        }
        else
            i.model_transform.mat = i.model_transform.mat_inner;
}

static void auth_3d_material_list_ctrl(auth_3d_material_list* ml, float_t frame) {
    if (ml->flags & AUTH_3D_MATERIAL_LIST_BLEND_COLOR)
        ml->blend_color.interpolate(frame);
    if (ml->flags & AUTH_3D_MATERIAL_LIST_GLOW_INTENSITY)
        ml->glow_intensity_value = ml->glow_intensity.interpolate(frame);
    if (ml->flags & AUTH_3D_MATERIAL_LIST_INCANDESCENCE)
        ml->incandescence.interpolate(frame);
}

static void auth_3d_material_list_load(auth_3d* auth, auth_3d_material_list* ml, auth_3d_material_list_file* mlf) {
    if (mlf->flags & A3DA_MATERIAL_LIST_BLEND_COLOR) {
        auth_3d_rgba_load(auth, &ml->blend_color, &mlf->blend_color);
        enum_or(ml->flags, AUTH_3D_MATERIAL_LIST_BLEND_COLOR);
    }

    if (mlf->flags & A3DA_MATERIAL_LIST_GLOW_INTENSITY) {
        auth_3d_key_load(auth, &ml->glow_intensity, &mlf->glow_intensity);
        enum_or(ml->flags, AUTH_3D_MATERIAL_LIST_GLOW_INTENSITY);
    }

    if (mlf->flags & A3DA_MATERIAL_LIST_INCANDESCENCE) {
        auth_3d_rgba_load(auth, &ml->incandescence, &mlf->incandescence);
        enum_or(ml->flags, AUTH_3D_MATERIAL_LIST_INCANDESCENCE);
    }

    ml->name = mlf->name;

    ml->material = object_storage_get_material(ml->name.c_str());

    material_change_storage_load(ml->name.c_str());
}

static void auth_3d_material_list_restore_prev_value(auth_3d_material_list* ml) {
    if (ml->flags_init & AUTH_3D_MATERIAL_LIST_INCANDESCENCE) {
        if (ml->material)
            ml->material->emission = ml->incandescence_init;
    }
    material_change_storage_unload(ml->name.c_str());
}

static void auth_3d_material_list_set(auth_3d_material_list* ml) {
    vec4 blend_color = vec4_identity;
    if (ml->flags & AUTH_3D_MATERIAL_LIST_BLEND_COLOR) {
        if (ml->blend_color.flags & AUTH_3D_RGBA_R)
            blend_color.x = ml->blend_color.value.x;
        if (ml->blend_color.flags & AUTH_3D_RGBA_G)
            blend_color.y = ml->blend_color.value.y;
        if (ml->blend_color.flags & AUTH_3D_RGBA_B)
            blend_color.z = ml->blend_color.value.z;
        if (ml->blend_color.flags & AUTH_3D_RGBA_A)
            blend_color.w = ml->blend_color.value.w;
    }

    float_t glow_intensity = 1.0f;
    if (ml->flags & AUTH_3D_MATERIAL_LIST_GLOW_INTENSITY)
        glow_intensity = ml->glow_intensity_value;

    vec4 incandescence = vec4_identity;
    if (ml->flags & AUTH_3D_MATERIAL_LIST_INCANDESCENCE) {
        if (~ml->flags_init & AUTH_3D_MATERIAL_LIST_INCANDESCENCE) {
            if (ml->material)
                ml->incandescence_init = ml->material->emission;
            else
                ml->incandescence_init = vec4_identity;
            enum_or(ml->flags_init, AUTH_3D_MATERIAL_LIST_INCANDESCENCE);
        }

        incandescence = ml->incandescence_init;
        if (ml->incandescence.flags & AUTH_3D_RGBA_R)
            incandescence.x = ml->incandescence.value.x;
        if (ml->incandescence.flags & AUTH_3D_RGBA_G)
            incandescence.y = ml->incandescence.value.y;
        if (ml->incandescence.flags & AUTH_3D_RGBA_B)
            incandescence.z = ml->incandescence.value.z;
        if (ml->incandescence.flags & AUTH_3D_RGBA_A)
            incandescence.w = ml->incandescence.value.w;
        if (ml->material)
            ml->material->emission = incandescence;
    }

    material_change* mat_chg = material_change_storage_get(ml->name.c_str());
    if (mat_chg) {
        mat_chg->blend_color = blend_color;
        mat_chg->glow_intensity = glow_intensity;
        mat_chg->incandescence = incandescence;
    }
}

static void auth_3d_object_ctrl(auth_3d_object* o, float_t frame) {
    o->model_transform.interpolate(frame);
    auth_3d_object_curve_ctrl(&o->pattern, frame);
    auth_3d_object_curve_ctrl(&o->morph, frame);
    for (auth_3d_object_texture_pattern& i : o->texture_pattern)
        auth_3d_object_texture_pattern_ctrl(&i, frame);
    for (auth_3d_object_texture_transform& i : o->texture_transform)
        auth_3d_object_texture_transform_ctrl(&i, frame);
}

static void auth_3d_object_load(auth_3d* auth, auth_3d_object* o,
    auth_3d_object_file* of, object_database* obj_db, texture_database* tex_db) {
    auth_3d_model_transform_load(auth, &o->model_transform, &of->model_transform);
    auth_3d_object_curve_load(auth, &o->morph, of->morph, of->morph_offset);
    o->name = of->name;
    o->parent_node = of->parent_node;
    o->parent_node = of->parent_node;
    auth_3d_object_curve_load(auth, &o->pattern, of->pattern, of->pattern_offset);

    size_t texture_pattern_index = 0;
    o->texture_pattern = std::vector<auth_3d_object_texture_pattern>(of->texture_pattern.size());
    for (auth_3d_object_texture_pattern_file& i : of->texture_pattern)
        auth_3d_object_texture_pattern_load(auth,
            &o->texture_pattern[texture_pattern_index++], &i, tex_db);

    size_t texture_transform_index = 0;
    o->texture_transform = std::vector<auth_3d_object_texture_transform>(of->texture_transform.size());
    for (auth_3d_object_texture_transform_file& i : of->texture_transform)
        auth_3d_object_texture_transform_load(auth,
            &o->texture_transform[texture_transform_index++], &i, tex_db);

    o->uid_name = of->uid_name;

    o->reflect = strstr(o->uid_name.c_str(), "_REFLECT") ? true : false;
    o->refract = strstr(o->uid_name.c_str(), "_REFRACT") ? true : false;

    o->object_info = obj_db->get_object_info(o->uid_name.c_str());
    o->object_hash = hash_string_murmurhash(&o->uid_name);
}

static void auth_3d_object_curve_ctrl(auth_3d_object_curve* oc, float_t frame) {
    auth_3d_curve* c = oc->curve;
    if (!c)
        return;

    if (oc->frame_offset != 0.0f) {
        frame += oc->frame_offset;
        if (frame >= c->curve.max_frame)
            frame -= c->curve.max_frame;
    }
    oc->value = c->curve.interpolate(frame);
}

static void auth_3d_object_curve_load(auth_3d* auth, auth_3d_object_curve* oc,
    std::string& name, float_t frame_offset) {
    oc->curve = 0;
    oc->name = name;
    oc->frame_offset = 0.0f;
    oc->value = 0.0f;

    for (auth_3d_curve& i : auth->curve)
        if (oc->name == i.name) {
            oc->curve = &i;
            oc->frame_offset = frame_offset;
            break;
        }
}

static void auth_3d_object_disp(auth_3d_object* o, auth_3d* auth, render_context* rctx) {
    if (!auth->visible || !o->model_transform.visible)
        return;

    mat4 mat = o->model_transform.mat;

    object_data* object_data = &rctx->object_data;
    object_database* obj_db = auth->obj_db;
    texture_database* tex_db = auth->tex_db;

    draw_task_flags flags = (draw_task_flags)0;
    if (auth->shadow)
        enum_or(flags, DRAW_TASK_4 | DRAW_TASK_SHADOW);
    if (o->reflect)
        enum_or(flags, DRAW_TASK_NO_TRANSLUCENCY | DRAW_TASK_REFLECT);
    if (o->refract)
        enum_or(flags, DRAW_TASK_NO_TRANSLUCENCY | DRAW_TASK_REFRACT);

    object_data_set_draw_task_flags(object_data, (draw_task_flags)flags);

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
        object_data_set_texture_pattern(object_data, tex_pat_count, tex_pat);

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
        object_data_set_texture_transform(object_data, tex_trans_count, tex_trans);

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
            object_data_set_morph(object_data, morph_obj_info, morph);

            sprintf_s(buf, sizeof(buf), "%.*s%03d", uid_name_length - 3, uid_name, morph_int);
            object_info obj_info = obj_db->get_object_info(buf);
            draw_task_add_draw_object_by_object_info(rctx, &mat, obj_info);
            object_data_set_morph(object_data, object_info(), 0.0f);
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

    object_data_set_texture_transform(object_data, 0, 0);
    object_data_set_texture_pattern(object_data, 0, 0);
    object_data_set_draw_task_flags(object_data, (draw_task_flags)0);

    for (auth_3d_object*& i : o->children_object)
        auth_3d_object_disp(i, auth, rctx);
    for (auth_3d_object_hrc*& i : o->children_object_hrc)
        auth_3d_object_hrc_disp(i, auth, rctx);
}

static void auth_3d_object_list_ctrl(auth_3d_object* o, mat4* parent_mat) {
    if (!o->model_transform.visible)
        return;
    
    auth_3d_model_transform_set_mat(&o->model_transform, parent_mat);

    mat4 mat = o->model_transform.mat;

    for (auth_3d_object*& i : o->children_object)
        auth_3d_object_list_ctrl(i, &mat);
    for (auth_3d_object_hrc*& i : o->children_object_hrc)
        auth_3d_object_hrc_list_ctrl(i, &mat);
}

static void auth_3d_object_hrc_ctrl(auth_3d_object_hrc* oh, float_t frame) {
    for (auth_3d_object_node& i : oh->node)
        auth_3d_object_model_transform_ctrl(&i.model_transform, frame);
}

static void auth_3d_object_hrc_load(auth_3d* auth, auth_3d_object_hrc* oh,
    auth_3d_object_hrc_file* ohf, object_database* obj_db) {
    oh->name = ohf->name;

    size_t node_index = 0;
    oh->node = std::vector<auth_3d_object_node>(ohf->node.size());
    for (auth_3d_object_node_file& i : ohf->node)
        auth_3d_object_node_load(auth,
            &oh->node[node_index++], &i);

    oh->parent_name = ohf->parent_name;
    oh->parent_node = ohf->parent_node;
    oh->shadow = ohf->shadow;
    oh->uid_name = ohf->uid_name;

    oh->reflect = strstr(oh->uid_name.c_str(), "_REFLECT") ? true : false;
    oh->refract = strstr(oh->uid_name.c_str(), "_REFRACT") ? true : false;

    oh->object_info = obj_db->get_object_info(oh->uid_name.c_str());
    oh->object_hash = hash_string_murmurhash(&oh->uid_name);

    obj_skin* skin = object_storage_get_obj_skin(oh->object_info);
    if (!skin)
        return;

    oh->mats = std::vector<mat4>(skin->bones_count);
    mat4* mats = oh->mats.data();

    for (auth_3d_object_node& i : oh->node) {
        i.bone_id = -1;
        uint64_t name_hash = hash_string_fnv1a64m(&i.name);
        for (int32_t j = 0; j < skin->bones_count; j++)
            if (hash_utf8_fnv1a64m(string_data(&skin->bones[j].name)) == name_hash) {
                i.bone_id = skin->bones[j].id;
                break;
            }

        if (i.bone_id > -1 && ~i.bone_id & 0x8000)
            i.mat = &mats[i.bone_id];
    }
}

static void auth_3d_object_hrc_disp(auth_3d_object_hrc* oh, auth_3d* auth, render_context* rctx) {
    if (!auth->visible || !oh->node[0].model_transform.visible)
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
        enum_or(flags, DRAW_TASK_10000);

    object_data_set_draw_task_flags(object_data, (draw_task_flags)flags);
    object_data_set_shadow_type(object_data, SHADOW_CHARA);
    
    shadow* shad = rctx->draw_pass.shadow_ptr;
    if (shad && flags & DRAW_TASK_SHADOW) {
        object_data_set_shadow_type(object_data, SHADOW_STAGE);

        mat4u* m = &oh->node[0].model_transform.mat;
        for (auth_3d_object_node& i : oh->node)
            if (i.mat) {
                m = (mat4u*)i.mat;
                break;
            }

        vec3 pos = *(vec3*)&m->row3;
        pos.y -= 0.2f;
        vector_old_vec3_push_back(&shad->field_1D0[SHADOW_STAGE], &pos);
    }

    if (oh->mats.size() > 0) {
        mat4 mat = mat4_identity;
        draw_task_add_draw_object_by_object_info_object_skin(rctx,
            oh->object_info, 0, 0, auth->alpha, oh->mats.data(), 0, 0, &mat);
    }

    object_data_set_draw_task_flags(object_data, (draw_task_flags)0);
    object_data_set_shadow_type(object_data, SHADOW_CHARA);

    for (auth_3d_object*& i : oh->children_object)
        auth_3d_object_disp(i, auth, rctx);

    for (auth_3d_object_hrc*& i : oh->children_object_hrc)
        auth_3d_object_hrc_disp(i, auth, rctx);
}

static void auth_3d_object_hrc_list_ctrl(auth_3d_object_hrc* oh, mat4* mat) {
    if (oh->node.size() < 1 || !oh->node[0].model_transform.visible)
        return;

    auth_3d_object_hrc_nodes_mat_mult(oh, mat);

    int32_t* childer_object_parent_node = oh->childer_object_parent_node.data();
    size_t children_object_count = oh->children_object.size();
    for (size_t i = 0; i < children_object_count; i++) {
        mat4 mat = oh->node[childer_object_parent_node[i]].model_transform.mat;
        auth_3d_object_list_ctrl(oh->children_object[i], &mat);
    }

    int32_t* childer_object_hrc_parent_node = oh->childer_object_hrc_parent_node.data();
    size_t children_object_hrc_count = oh->children_object_hrc.size();
    for (size_t i = 0; i < children_object_hrc_count; i++) {
        mat4 mat = oh->node[childer_object_hrc_parent_node[i]].model_transform.mat;
        auth_3d_object_hrc_list_ctrl(oh->children_object_hrc[i], &mat);
    }
}

static void auth_3d_object_hrc_nodes_mat_mult(auth_3d_object_hrc* oh, mat4* mat) {
    for (auth_3d_object_node& i : oh->node) {
        auth_3d_object_model_transform_set_mat_inner(&i.model_transform);

        mat4 m;
        if (i.parent >= 0)
            m = oh->node[i.parent].model_transform.mat;
        else
            m = *mat;

        mat4 joint_orient = i.joint_orient_mat;
        mat4_mult(&joint_orient, &m, &m);
        auth_3d_object_model_transform_mat_mult(&i.model_transform, &m);

        if (i.mat)
            *i.mat = i.model_transform.mat;
    }
}

static void auth_3d_object_instance_load(auth_3d* auth, auth_3d_object_instance* oi,
    auth_3d_object_instance_file* oif, auth_3d_m_object_hrc* moh, object_database* obj_db) {
    auth_3d_object_model_transform_load(auth, &oi->model_transform, &oif->model_transform);
    oi->name = oif->name;
    oi->shadow = oif->shadow;
    oi->uid_name = oif->uid_name;

    oi->object_info = obj_db->get_object_info(oi->uid_name.c_str());
    oi->object_hash = hash_string_murmurhash(&oi->uid_name);

    obj_skin* skin = object_storage_get_obj_skin(oi->object_info);
    if (!skin)
        return;

    oi->object_bone_indices = std::vector<int32_t>(skin->bones_count);
    int32_t* object_bone_indices = oi->object_bone_indices.data();

    oi->mats = std::vector<mat4>(skin->bones_count);
    mat4* mats = oi->mats.data();

    for (int32_t i = 0; i < skin->bones_count; i++) {
        uint64_t name_hash = hash_utf8_fnv1a64m(string_data(&skin->bones[i].name));
        for (auth_3d_object_node& j : moh->node)
            if (hash_string_fnv1a64m(&j.name) == name_hash) {
                int32_t bone_id = skin->bones[i].id;
                if (~bone_id & 0x8000)
                    object_bone_indices[bone_id] = (int32_t)(&j - moh->node.data());
                break;
            }
    }
}

static void auth_3d_object_model_transform_ctrl(auth_3d_object_model_transform* obj_mt, float_t frame) {
    if (frame == obj_mt->frame && !obj_mt->has_rotation)
        return;

    obj_mt->translation_value = obj_mt->translation.interpolate(frame);
    obj_mt->rotation_value = obj_mt->rotation.interpolate(frame);

    if (obj_mt->has_scale)
        obj_mt->scale_value = obj_mt->scale.interpolate(frame);

    if (obj_mt->has_visibility)
        obj_mt->visible = obj_mt->visibility.interpolate(frame) >= 0.99900001f;

    mat4 mat_rot;
    vec3 rotation = obj_mt->rotation_value;
    mat4_rotate(rotation.x, rotation.y, rotation.z, &mat_rot);
    obj_mt->mat_rot = mat_rot;
    obj_mt->frame = frame;
    obj_mt->has_rotation = false;
    obj_mt->has_translation = false;
}

static void auth_3d_object_model_transform_load(auth_3d* auth,
    auth_3d_object_model_transform* omt, auth_3d_model_transform_file* mtf) {
    omt->mat = mat4u_identity;
    omt->mat_inner = mat4u_identity;
    omt->mat_rot = mat4u_identity;
    omt->translation_value = vec3_null;
    omt->rotation_value = vec3_null;
    omt->scale_value = vec3_identity;
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
        omt->scale_value = vec3_identity;
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

static void auth_3d_object_model_transform_mat_mult(auth_3d_object_model_transform* obj_mt, mat4* mat) {
    mat4 obj_mt_mat = obj_mt->mat_inner;
    mat4_mult(&obj_mt_mat, mat, &obj_mt_mat);
    obj_mt->mat = obj_mt_mat;
}

static void auth_3d_object_model_transform_set_mat_inner(auth_3d_object_model_transform* obj_mt) {
    if (obj_mt->has_translation && !obj_mt->has_rotation)
        return;

    mat4 mat;
    vec3 translation = obj_mt->translation_value;
    mat4_translate(translation.x, translation.y, translation.z, &mat);

    mat4 mat_rot = obj_mt->mat_rot;
    mat4_mult(&mat_rot, &mat, &mat);
    if (obj_mt->has_scale) {
        vec3 scale = obj_mt->scale_value;
        mat4_scale_rot(&mat, scale.x, scale.y, scale.z, &mat);
    }
    obj_mt->mat_inner = mat;
    obj_mt->has_translation = true;
}

static void auth_3d_object_node_load(auth_3d* auth,
    auth_3d_object_node* on, auth_3d_object_node_file* onf) {
    if (onf->flags & A3DA_OBJECT_NODE_JOINT_ORIENT) {
        vec3 rot = onf->joint_orient;
        mat4 mat;
        mat4_rotate(rot.x, rot.y, rot.z, &mat);
        on->joint_orient = rot;
        on->joint_orient_mat = mat;
        enum_or(on->flags, AUTH_3D_OBJECT_NODE_JOINT_ORIENT);
    }
    else {
        on->joint_orient = vec3_null;
        on->joint_orient_mat = mat4u_identity;
    }

    auth_3d_object_model_transform_load(auth, &on->model_transform, &onf->model_transform);
    on->name = onf->name;
    on->parent = onf->parent;
}

static void auth_3d_object_texture_pattern_ctrl(
    auth_3d_object_texture_pattern* otp, float_t frame) {
    auth_3d_object_curve_ctrl(&otp->pattern, frame);
}

static void auth_3d_object_texture_pattern_load(auth_3d* auth, auth_3d_object_texture_pattern* otp,
    auth_3d_object_texture_pattern_file* otpf, texture_database* tex_db) {
    otp->name = otpf->name;
    auth_3d_object_curve_load(auth, &otp->pattern, otpf->pattern, otpf->pattern_offset);
    otp->texture_id = tex_db->get_texture_id(otp->name.c_str());
}

static void auth_3d_object_texture_transform_ctrl(
    auth_3d_object_texture_transform* ott, float_t frame) {
    int32_t flags = ott->flags;

    vec3 scale = vec3_identity;
    vec3 repeat = vec3_identity;
    vec3 rotate = vec3_null;
    vec3 translate_frame = vec3_null;

    if (flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_COVERAGE_U && ott->coverage_u.max_frame > 0.0f) {
        float_t coverage_u = ott->coverage_u.interpolate(frame);
        if (fabsf(coverage_u) > 0.000001f)
            scale.x = 1.0f / coverage_u;
    }

    if (flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_COVERAGE_V && ott->coverage_v.max_frame > 0.0f) {
        float_t coverage_v = ott->coverage_v.interpolate(frame);
        if (fabsf(coverage_v) > 0.000001f)
            scale.y = 1.0f / coverage_v;
    }

    if (flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_REPEAT_U && ott->repeat_u.max_frame > 0.0f) {
        float_t value = ott->repeat_u.interpolate(frame);
        if (fabsf(value) > 0.000001f)
            repeat.x = value;
    }

    if (flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_REPEAT_V && ott->repeat_v.max_frame > 0.0f) {
        float_t value = ott->repeat_v.interpolate(frame);
        if (fabsf(value) > 0.000001f)
            repeat.y = value;
    }

    if (flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_ROTATE_FRAME)
        rotate.z = ott->rotate_frame.interpolate(frame);

    if (flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_ROTATE)
        rotate.z += ott->rotate.interpolate(frame);

    float_t translate_frame_u;
    if (flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_TRANSLATE_FRAME_U)
        translate_frame_u = -ott->translate_frame_u.interpolate(frame);
    else
        translate_frame_u = 0.0f;

    float_t offset_u;
    if (flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_OFFSET_U)
        offset_u = ott->offset_u.interpolate(frame);
    else
        offset_u = 0.0f;

    translate_frame.x = (translate_frame_u + offset_u) * repeat.x;

    float_t translate_frame_v;
    if (flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_TRANSLATE_FRAME_V)
        translate_frame_v = -ott->translate_frame_v.interpolate(frame);
    else
        translate_frame_v = 0.0f;

    float_t offset_v;
    if (flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_OFFSET_V)
        offset_v = ott->offset_v.interpolate(frame);
    else
        offset_v = 0.0f;

    translate_frame.y = (translate_frame_v + offset_v) * repeat.y;

    mat4 mat;
    mat4_translate(translate_frame.x, translate_frame.y, translate_frame.z, &mat);
    mat4_scale_rot(&mat, scale.x, scale.y, scale.z, &mat);

    mat4_translate_mult(&mat, 1.0f, 1.0f, 0.0f, &mat);
    mat4_scale_rot(&mat, repeat.x, repeat.y, repeat.z, &mat);
    mat4_translate_mult(&mat, -1.0f, -1.0f, 0.0f, &mat);

    mat4_translate_mult(&mat, 0.5f, 0.5f, 0.0f, &mat);
    mat4_rotate_z_mult(&mat, rotate.z, &mat);
    mat4_translate_mult(&mat, -0.5f, -0.5f, 0.0f, &mat);
    ott->mat = mat;
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

    ott->name = ottf->name;

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

static void auth_3d_point_ctrl(auth_3d_point* p, float_t frame) {
    p->model_transform.interpolate(frame);
}

static void auth_3d_point_load(auth_3d* auth, auth_3d_point* p, auth_3d_point_file* pf) {
    auth_3d_model_transform_load(auth, &p->model_transform, &pf->model_transform);
    p->name = pf->name;
}

static void auth_3d_post_process_ctrl(auth_3d_post_process* pp, float_t frame) {
    if (pp->flags & AUTH_3D_POST_PROCESS_INTENSITY)
        pp->intensity.interpolate(frame);
    if (pp->flags & AUTH_3D_POST_PROCESS_LENS_FLARE)
        pp->lens_flare_value = pp->lens_flare.interpolate(frame);
    if (pp->flags & AUTH_3D_POST_PROCESS_LENS_SHAFT)
        pp->lens_shaft_value = pp->lens_shaft.interpolate(frame);
    if (pp->flags & AUTH_3D_POST_PROCESS_LENS_GHOST)
        pp->lens_ghost_value = pp->lens_ghost.interpolate(frame);
    if (pp->flags & AUTH_3D_POST_PROCESS_RADIUS)
        pp->radius.interpolate(frame);
    if (pp->flags & AUTH_3D_POST_PROCESS_SCENE_FADE)
        pp->scene_fade.interpolate(frame);
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
        post_process_blur_set_intensity(blur, &pp->intensity_init);

    if (pp->flags_init & AUTH_3D_POST_PROCESS_LENS_FLARE)
        post_process_tone_map_set_lens_ghost(tm, pp->lens_flare_init);

    if (pp->flags_init & AUTH_3D_POST_PROCESS_LENS_GHOST)
        post_process_tone_map_set_lens_ghost(tm, pp->lens_ghost_init);

    if (pp->flags_init & AUTH_3D_POST_PROCESS_LENS_SHAFT)
        post_process_tone_map_set_lens_shaft(tm, pp->lens_shaft_init);

    if (pp->flags_init & AUTH_3D_POST_PROCESS_RADIUS)
        post_process_blur_set_radius(blur, &pp->radius_init);

    if (pp->flags_init & AUTH_3D_POST_PROCESS_SCENE_FADE) {
        vec4 scene_fade_init = pp->scene_fade_init;
        post_process_tone_map_set_scene_fade(tm, &scene_fade_init);
    }
}

static void auth_3d_post_process_set(auth_3d_post_process* pp, render_context* rctx) {
    post_process_blur* blur = rctx->post_process.blur;
    post_process_tone_map* tm = rctx->post_process.tone_map;

    if (pp->flags & AUTH_3D_POST_PROCESS_INTENSITY) {
        if (~pp->flags_init & AUTH_3D_POST_PROCESS_INTENSITY) {
            post_process_blur_get_intensity(blur, &pp->intensity_init);
            enum_or(pp->flags_init, AUTH_3D_POST_PROCESS_INTENSITY);
        }

        post_process_blur_set_intensity(blur, (vec3*)&pp->intensity.value);
    }

    if (pp->flags & AUTH_3D_POST_PROCESS_LENS_FLARE) {
        if (~pp->flags_init & AUTH_3D_POST_PROCESS_LENS_FLARE) {
            pp->lens_flare_init = post_process_tone_map_get_lens_flare(tm);
            enum_or(pp->flags_init, AUTH_3D_POST_PROCESS_LENS_FLARE);
        }

        post_process_tone_map_set_lens_ghost(tm, pp->lens_flare_value);
    }

    if (pp->flags & AUTH_3D_POST_PROCESS_LENS_GHOST) {
        if (~pp->flags_init & AUTH_3D_POST_PROCESS_LENS_GHOST) {
            pp->lens_ghost_init = post_process_tone_map_get_lens_ghost(tm);
            enum_or(pp->flags_init, AUTH_3D_POST_PROCESS_LENS_GHOST);
        }

        post_process_tone_map_set_lens_ghost(tm, pp->lens_ghost_value);
    }

    if (pp->flags & AUTH_3D_POST_PROCESS_LENS_SHAFT) {
        if (~pp->flags_init & AUTH_3D_POST_PROCESS_LENS_SHAFT) {
            pp->lens_shaft_init = post_process_tone_map_get_lens_shaft(tm);
            enum_or(pp->flags_init, AUTH_3D_POST_PROCESS_LENS_SHAFT);
        }

        post_process_tone_map_set_lens_shaft(tm, pp->lens_shaft_value);
    }

    if (pp->flags & AUTH_3D_POST_PROCESS_RADIUS) {
        if (~pp->flags_init & AUTH_3D_POST_PROCESS_RADIUS) {
            post_process_blur_get_radius(blur, &pp->radius_init);
            enum_or(pp->flags_init, AUTH_3D_POST_PROCESS_RADIUS);
        }

        post_process_blur_set_radius(blur, (vec3*)&pp->radius.value);
    }

    if (pp->flags & AUTH_3D_POST_PROCESS_SCENE_FADE) {
        if (~pp->flags_init & AUTH_3D_POST_PROCESS_SCENE_FADE) {
            vec4 scene_fade_init;
            post_process_tone_map_get_scene_fade(tm, &scene_fade_init);
            pp->scene_fade_init = scene_fade_init;
            enum_or(pp->flags_init, AUTH_3D_POST_PROCESS_SCENE_FADE);
        }

        vec4 scene_fade = pp->scene_fade.value;
        post_process_tone_map_set_scene_fade(tm, &scene_fade);
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
        if (hash_string_fnv1a64m(&i.name) == name_hash)
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
        a3da_farc->name = std::string(category_name);
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
    auth_3d_data->farcs_modern.erase(elem);
}

static void auth_3d_farc_free_data(auth_3d_farc* a3da_farc) {
    if (a3da_farc->state == 1)
        a3da_farc->file_handler.call_free_func_free_data();
    else if (a3da_farc->state == 2) {
        delete a3da_farc->farc;
        a3da_farc->farc = 0;
        a3da_farc->file_handler.free_data();
    }

    a3da_farc->path.clear();
    a3da_farc->path.shrink_to_fit();
    a3da_farc->file.clear();
    a3da_farc->file.shrink_to_fit();
    a3da_farc->data = 0;
    a3da_farc->size = 0;
    a3da_farc->state = 0;
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
    a3da_farc->path = std::string("rom/auth_3d/");
    if (mdata_dir && *mdata_dir)
        a3da_farc->path += mdata_dir;
    a3da_farc->file = std::string(a3da_farc->name) + ".farc";


    if (a3da_farc->file_handler.read_file(rctx_ptr->data, a3da_farc->path.c_str(), a3da_farc->file.c_str()))
        a3da_farc->file_handler.set_read_free_func_data(0,
            (void(*)(void*, void*, size_t))auth_3d_farc_read_func, a3da_farc);
    else
        a3da_farc->state = 2;
}

static void auth_3d_farc_read_file_modern(auth_3d_farc* a3da_farc, void* data) {
    if (a3da_farc->state) {
        a3da_farc->state = 2;
        return;
    }

    a3da_farc->state = 1;
    a3da_farc->path = std::string("root+/auth_3d/");
    a3da_farc->file = std::string(a3da_farc->name) + ".farc";


    if (a3da_farc->file_handler.read_file(data, a3da_farc->path.c_str(), a3da_farc->file.c_str()))
        a3da_farc->file_handler.set_read_free_func_data(0,
            (void(*)(void*, void*, size_t))auth_3d_farc_read_func, a3da_farc);
    else
        a3da_farc->state = 2;
}

static bool auth_3d_farc_read_func(auth_3d_farc* a3da_farc, void* data, size_t size) {
    if (a3da_farc->state == 1) {
        a3da_farc->state = 2;
        a3da_farc->data = data;
        a3da_farc->size = size;
    }

    a3da_farc->farc = new farc;
    if (!a3da_farc->farc)
        return false;

    a3da_farc->farc->read(a3da_farc->data, a3da_farc->size, true);
    return true;
}

static void auth_3d_farc_unload(auth_3d_farc* a3da_farc) {
    if (--a3da_farc->load_count < 0)
        a3da_farc->load_count = 0;
    else if (!a3da_farc->load_count)
        auth_3d_farc_free_data(a3da_farc);
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
    uid_file->file_name = db_uid->name + ".a3da";
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

    uid_file->file_name = uid_file->name + ".a3da";

    uint32_t category_hash = uid_file->category_hash;

    auth_3d_farc* farc = 0;
    if (category_hash && category_hash != -1 && category_hash != hash_murmurhash_empty
        && auth_3d_data->farcs_modern.size()) {
        auto elem = auth_3d_data->farcs_modern.find(category_hash);
        if (elem != auth_3d_data->farcs_modern.end())
            farc = &elem->second;
    }

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
    uid_file->name = {};
    uid_file->farc = 0;
    uid_file->state = 0;
    auth_3d_data->uid_files_modern.erase(uid_file->hash);
}
