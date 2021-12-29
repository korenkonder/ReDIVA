/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "auth_3d.h"
#include "../KKdLib/interpolation.h"
#include "../KKdLib/str_utils.h"
#include "draw_task.h"
#include "object.h"

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

vector_func(auth_3d_ambient)
vector_func(auth_3d_model_transform)
vector_func(auth_3d_camera_root)
vector_func(auth_3d_chara)
vector_func(auth_3d_curve)
vector_func(auth_3d_event)
vector_func(auth_3d_fog)
vector_func(auth_3d_light)
vector_func(auth_3d_m_object_hrc)
vector_func(auth_3d_object_instance)
vector_func(auth_3d_material_list)
vector_func(auth_3d_object)
vector_func(auth_3d_object_hrc)
vector_func(auth_3d_object_node)
vector_func(auth_3d_object_texture_pattern)
vector_func(auth_3d_object_texture_transform)
vector_func(auth_3d_point)
vector_func(ptr_auth_3d_m_object_hrc)
vector_func(ptr_auth_3d_object)
vector_func(ptr_auth_3d_object_hrc)

static kft3* auth_3d_key_find_keyframe(auth_3d_key* data, float_t frame);
static float_t auth_3d_key_get_value(auth_3d_key* data, float_t frame);
static float_t auth_3d_key_interpolate(auth_3d_key* data, float_t frame);
static float_t auth_3d_interpolate_value(auth_3d_key_type type,
    float_t frame, kft3* curr_key, kft3* next_key);
static void auth_3d_key_load(auth_3d* auth, auth_3d_key* k, auth_3d_key_file* kf);
static void auth_3d_rgba_get_value(auth_3d_rgba* rgba, float_t frame);
static void auth_3d_rgba_load(auth_3d* auth, auth_3d_rgba* rgba, auth_3d_rgba_file* rgbaf);
static void auth_3d_vec3_get_value(auth_3d_vec3* vec, float_t frame, vec3* value);
static void auth_3d_vec3_load(auth_3d* auth, auth_3d_vec3* vec, auth_3d_vec3_file* vecf);
static void auth_3d_model_transform_get_value(auth_3d_model_transform* mt, float_t frame);
static void auth_3d_model_transform_load(auth_3d* auth, auth_3d_model_transform* mt, auth_3d_model_transform_file* mtf);
static void auth_3d_model_transform_set_mat(auth_3d_model_transform* mt, mat4* parent_mat);

static int32_t auth_3d_get_auth_3d_object_index_by_object_info(auth_3d* auth, object_info obj_info);
static mat4* auth_3d_get_auth_3d_object_hrc_bone_mats(auth_3d* auth, size_t index);
static int32_t auth_3d_get_auth_3d_object_hrc_index_by_object_info(auth_3d* auth, object_info obj_info);

static void auth_3d_ambient_data_set(auth_3d_ambient* a, render_context* rctx);
static void auth_3d_ambient_get_value(auth_3d_ambient* a, float_t frame);
static void auth_3d_ambient_load(auth_3d* auth, auth_3d_ambient_file* af);
static void auth_3d_camera_auxiliary_data_set(auth_3d_camera_auxiliary* ca, render_context* rctx);
static void auth_3d_camera_auxiliary_get_value(auth_3d_camera_auxiliary* ca, float_t frame);
static void auth_3d_camera_auxiliary_load(auth_3d* auth, auth_3d_camera_auxiliary_file* caf);
static void auth_3d_camera_auxiliary_restore_prev_value(auth_3d_camera_auxiliary* ca, render_context* rctx);
static void auth_3d_camera_root_data_set(auth_3d_camera_root* cr,
    bool left_right_reverse, mat4* mat, render_context* rctx);
static void auth_3d_camera_root_get_value(auth_3d_camera_root* cr, float_t frame, mat4* mat);
static void auth_3d_camera_root_load(auth_3d* auth, auth_3d_camera_root_file* crf);
static void auth_3d_camera_root_view_point_load(auth_3d* auth, auth_3d_camera_root* cr,
    auth_3d_camera_root_view_point_file* crvpf);
static void auth_3d_chara_data_set(auth_3d_chara* c, mat4* parent_mat, render_context* rctx);
static void auth_3d_chara_get_value(auth_3d_chara* c, float_t frame);
static void auth_3d_chara_load(auth_3d* auth, auth_3d_chara_file* cf);
static void auth_3d_curve_get_value(auth_3d_curve* c, float_t frame);
static void auth_3d_curve_load(auth_3d* auth, auth_3d_curve_file* cf);
static void auth_3d_dof_data_set(auth_3d_dof* d, render_context* rctx);
static void auth_3d_dof_get_value(auth_3d_dof* d, float_t frame);
static void auth_3d_dof_load(auth_3d* auth, auth_3d_dof_file* df);
static void auth_3d_dof_restore_prev_value(auth_3d_dof* d, render_context* rctx);
static void auth_3d_event_load(auth_3d* auth, auth_3d_event_file* ef);
static void auth_3d_fog_data_set(auth_3d_fog* f, render_context* rctx);
static void auth_3d_fog_get_value(auth_3d_fog* f, float_t frame);
static void auth_3d_fog_load(auth_3d* auth, auth_3d_fog_file* ff);
static void auth_3d_fog_restore_prev_value(auth_3d_fog* f, render_context* rctx);
static void auth_3d_light_data_set(auth_3d_light* l, render_context* rctx);
static void auth_3d_light_get_value(auth_3d_light* l, float_t frame);
static void auth_3d_light_load(auth_3d* auth, auth_3d_light_file* lf);
static void auth_3d_light_restore_prev_value(auth_3d_light* l, render_context* rctx);
static void auth_3d_m_object_hrc_get_mat(auth_3d_m_object_hrc* moh, mat4* mat);
static void auth_3d_m_object_hrc_get_value(auth_3d_m_object_hrc* moh, float_t frame);
static void auth_3d_m_object_hrc_load(auth_3d* auth,
    auth_3d_m_object_hrc_file* mohf, object_database* obj_db);
static void auth_3d_m_object_hrc_data_set(auth_3d_m_object_hrc* moh, auth_3d* auth, render_context* rctx);
static void auth_3d_m_object_hrc_list_get_value(auth_3d_m_object_hrc* moh, mat4* parent_mat);
static void auth_3d_m_object_hrc_nodes_mat_mult(auth_3d_m_object_hrc* moh);
static void auth_3d_material_list_load(auth_3d* auth, auth_3d_material_list_file* mlf);
static void auth_3d_object_data_set(auth_3d_object* o, auth_3d* auth, render_context* rctx);
static void auth_3d_object_get_value(auth_3d_object* o, float_t frame);
static void auth_3d_object_load(auth_3d* auth, auth_3d_object_file* of,
    object_database* obj_db, texture_database* tex_db);
static void auth_3d_object_curve_get_value(auth_3d_object_curve* oc, float_t frame);
static void auth_3d_object_curve_load(auth_3d* auth, auth_3d_object_curve* curve,
    string* name, float_t frame_offset);
static void auth_3d_object_list_get_value(auth_3d_object* o, mat4* parent_mat);
static void auth_3d_object_hrc_data_set(auth_3d_object_hrc* oh, auth_3d* auth, render_context* rctx);
static void auth_3d_object_hrc_get_value(auth_3d_object_hrc* oh, float_t frame);
static void auth_3d_object_hrc_load(auth_3d* auth,
    auth_3d_object_hrc_file* ohf, object_database* obj_db);
static void auth_3d_object_hrc_list_get_value(auth_3d_object_hrc* oh, mat4* mat);
static void auth_3d_object_hrc_nodes_mat_mult(auth_3d_object_hrc* oh, mat4* mat);
static void auth_3d_object_instance_load(auth_3d* auth, auth_3d_m_object_hrc* moh,
    auth_3d_object_instance_file* oif, object_database* obj_db);
static void auth_3d_object_model_transform_get_value(auth_3d_object_model_transform* obj_mt, float_t frame);
static void auth_3d_object_model_transform_load(auth_3d* auth,
    auth_3d_object_model_transform* omt, auth_3d_model_transform_file* mtf);
static void auth_3d_object_model_transform_mat_mult(auth_3d_object_model_transform* obj_mt, mat4* mat);
static void auth_3d_object_model_transform_set_mat_inner(auth_3d_object_model_transform* obj_mt);
static void auth_3d_object_node_load(auth_3d* auth,
    vector_auth_3d_object_node* von, auth_3d_object_node_file* onf);
static void auth_3d_object_texture_pattern_get_value(
    auth_3d_object_texture_pattern* otp, float_t frame);
static void auth_3d_object_texture_pattern_load(auth_3d* auth, auth_3d_object* o,
    auth_3d_object_texture_pattern_file* otpf, texture_database* tex_db);
static void auth_3d_object_texture_transform_get_value(
    auth_3d_object_texture_transform* ott, float_t frame);
static void auth_3d_object_texture_transform_load(auth_3d* auth, auth_3d_object* o,
    auth_3d_object_texture_transform_file* ottf, texture_database* tex_db);
static void auth_3d_play_control_load(auth_3d* auth, auth_3d_play_control_file* pcf);
static void auth_3d_point_data_set(auth_3d_point* p, mat4* parent_mat, render_context* rctx);
static void auth_3d_point_get_value(auth_3d_point* p, float_t frame);
static void auth_3d_point_load(auth_3d* auth, auth_3d_point_file* pf);
static void auth_3d_post_process_data_set(auth_3d_post_process* pp, render_context* rctx);
static void auth_3d_post_process_get_value(auth_3d_post_process* pp, float_t frame);
static void auth_3d_post_process_load(auth_3d* auth, auth_3d_post_process_file* ppf);
static void auth_3d_post_process_restore_prev_value(auth_3d_post_process* pp, render_context* rctx);

auth_3d_data_struct auth_3d_data;
static int16_t auth_3d_load_counter;

void auth_3d_init(auth_3d* auth) {
    if (!auth)
        return;

    memset(auth, 0, sizeof(auth_3d));
}

void auth_3d_data_set(auth_3d* auth, mat4* mat, render_context* rctx) {
    if (!auth)
        return;

    for (auth_3d_ambient* i = auth->ambient.begin; i != auth->ambient.end; i++)
        auth_3d_ambient_data_set(i, rctx);

    for (auth_3d_camera_root* i = auth->camera_root.begin; i != auth->camera_root.end; i++) {
        auth_3d_camera_root_data_set(i, auth->left_right_reverse, mat, rctx);
        break;
    }

    for (auth_3d_chara* i = auth->chara.begin; i != auth->chara.end; i++)
        auth_3d_chara_data_set(i, mat, rctx);

    for (auth_3d_fog* i = auth->fog.begin; i != auth->fog.end; i++)
        auth_3d_fog_data_set(i, rctx);

    for (auth_3d_light* i = auth->light.begin; i != auth->light.end; i++)
        auth_3d_light_data_set(i, rctx);

    for (auth_3d_point* i = auth->point.begin; i != auth->point.end; i++)
        auth_3d_point_data_set(i, mat, rctx);

    for (ptr_auth_3d_object* i = auth->object_list.begin; i != auth->object_list.end; i++)
        auth_3d_object_data_set(*i, auth, rctx);

    for (ptr_auth_3d_object_hrc* i = auth->object_hrc_list.begin; i != auth->object_hrc_list.end; i++)
        auth_3d_object_hrc_data_set(*i, auth, rctx);

    for (ptr_auth_3d_m_object_hrc* i = auth->m_object_hrc_list.begin;
        i != auth->m_object_hrc_list.end; i++)
        auth_3d_m_object_hrc_data_set(*i, auth, rctx);

    auth_3d_camera_auxiliary_data_set(&auth->camera_auxiliary, rctx);
    auth_3d_dof_data_set(&auth->dof, rctx);
    auth_3d_post_process_data_set(&auth->post_process, rctx);
}

void auth_3d_get_value(auth_3d* auth, mat4* mat, float_t delta_frame) {
    if (!auth || !auth->enable)
        return;

    bool set = false;
    while (true) {
        if (auth->frame_changed) {
            auth->frame_changed = false;
            auth->frame = auth->set_frame;
            auth->ended = false;
            if (auth->max_frame >= 0.0f && auth->frame >= auth->max_frame)
                auth->max_frame = -1.0f;
        }
        else if (!auth->paused) {
            auth->frame += delta_frame;
            if (auth->max_frame >= 0.0f && auth->frame > auth->max_frame)
                auth->frame = auth->max_frame;
        }

        float_t frame = auth->frame;

        auth->frame_int = (int32_t)frame;
        frame = (float_t)auth->frame_int;

        for (auth_3d_ambient* i = auth->ambient.begin; i != auth->ambient.end; i++)
            auth_3d_ambient_get_value(i, frame);

        for (auth_3d_chara* i = auth->chara.begin; i != auth->chara.end; i++)
            auth_3d_chara_get_value(i, frame);

        if (auth->camera_root_update)
            for (auth_3d_camera_root* i = auth->camera_root.begin; i != auth->camera_root.end; i++)
                auth_3d_camera_root_get_value(i, frame, mat);

        for (auth_3d_chara* i = auth->chara.begin; i != auth->chara.end; i++)
            auth_3d_chara_get_value(i, frame);

        for (auth_3d_curve* i = auth->curve.begin; i != auth->curve.end; i++)
            auth_3d_curve_get_value(i, frame);

        for (auth_3d_fog* i = auth->fog.begin; i != auth->fog.end; i++)
            auth_3d_fog_get_value(i, frame);

        for (auth_3d_light* i = auth->light.begin; i != auth->light.end; i++)
            auth_3d_light_get_value(i, frame);

        for (auth_3d_object* i = auth->object.begin; i != auth->object.end; i++)
            auth_3d_object_get_value(i, frame);

        for (auth_3d_object_hrc* i = auth->object_hrc.begin; i != auth->object_hrc.end; i++)
            auth_3d_object_hrc_get_value(i, frame);

        for (auth_3d_m_object_hrc* i = auth->m_object_hrc.begin; i != auth->m_object_hrc.end; i++)
            auth_3d_m_object_hrc_get_value(i, frame);

        for (auth_3d_point* i = auth->point.begin; i != auth->point.end; i++)
            auth_3d_point_get_value(i, frame);

        for (ptr_auth_3d_object* i = auth->object_list.begin; i != auth->object_list.end; i++)
            auth_3d_object_list_get_value(*i, mat);

        for (ptr_auth_3d_object_hrc* i = auth->object_hrc_list.begin; i != auth->object_hrc_list.end; i++)
            auth_3d_object_hrc_list_get_value(*i, mat);

        for (ptr_auth_3d_m_object_hrc* i = auth->m_object_hrc_list.begin;
            i != auth->m_object_hrc_list.end; i++)
            auth_3d_m_object_hrc_list_get_value(*i, mat);

        auth_3d_camera_auxiliary_get_value(&auth->camera_auxiliary, frame);
        auth_3d_dof_get_value(&auth->dof, frame);
        auth_3d_post_process_get_value(&auth->post_process, frame);

        auth_3d_model_transform_get_value(&auth->dof.model_transform, frame);
        if (set || !auth->repeat || auth->last_frame > auth->frame)
            break;

        auth->set_frame = auth->frame - auth->frame + auth->frame_offset;
        auth->frame_changed = true;
        set = true;
    }

    bool ended = auth->play_control.size <= auth->frame;
    if (ended && auth->ended != ended) {
        auth->paused = true;
        if (auth->once)
            auth->enable = false;
    }
    auth->ended = ended;
}

void auth_3d_unload(auth_3d* auth, render_context* rctx) {
    if (!auth)
        return;

    auth_3d_camera_auxiliary_restore_prev_value(&auth->camera_auxiliary, rctx);
    for (auth_3d_light* i = auth->light.begin; i != auth->light.end; i++)
        auth_3d_light_restore_prev_value(i, rctx);
    for (auth_3d_fog* i = auth->fog.begin; i != auth->fog.end; i++)
        auth_3d_fog_restore_prev_value(i, rctx);
    auth_3d_dof_restore_prev_value(&auth->dof, rctx);
    auth_3d_post_process_restore_prev_value(&auth->post_process, rctx);
    auth_3d_free(auth);
}

void auth_3d_load(auth_3d* auth, a3da* auth_file,
    object_database* obj_db, texture_database* tex_db) {
    if (!auth || !auth_file)
        return;

    auth_3d_camera_auxiliary_load(auth, &auth_file->camera_auxiliary);
    auth_3d_dof_load(auth, &auth_file->dof);
    auth_3d_play_control_load(auth, &auth_file->play_control);
    auth_3d_post_process_load(auth, &auth_file->post_process);

    auth->frame = -1;
    auth->max_frame = auth->play_control.size;

    auth->enable = true;
    auth->camera_root_update = true;
    auth->repeat = true;
    auth->ended = false;
    auth->left_right_reverse = false;
    auth->once = false;

    auth->frame = 0.0f;
    auth->set_frame = 0.0f;
    auth->max_frame = auth->play_control.size;
    auth->frame_changed = false;
    auth->frame_offset = 0.0f;
    auth->last_frame = auth->play_control.size;
    auth->paused = false;

    if (auth_file->ambient.end - auth_file->ambient.begin > 0) {
        vector_auth_3d_ambient* va = &auth->ambient;
        vector_a3da_ambient* vaf = &auth_file->ambient;
        vector_auth_3d_ambient_reserve(va, vaf->end - vaf->begin);
        for (auth_3d_ambient_file* i = vaf->begin; i != vaf->end; i++)
            auth_3d_ambient_load(auth, i);
    }

    if (auth_file->auth_2d.end - auth_file->auth_2d.begin > 0) {
        vector_string* va2 = &auth->auth_2d;
        vector_string* va2f = &auth_file->auth_2d;
        vector_string_reserve(va2, va2f->end - va2f->begin);
        for (string* i = va2f->begin; i != va2f->end; i++) {
            string* m = vector_string_reserve_back(va2f);
            string_copy(i, m);
        }
    }

    if (auth_file->camera_root.end - auth_file->camera_root.begin > 0) {
        vector_auth_3d_camera_root* vcr = &auth->camera_root;
        vector_a3da_camera_root* vcrf = &auth_file->camera_root;
        vector_auth_3d_camera_root_reserve(vcr, vcrf->end - vcrf->begin);
        for (auth_3d_camera_root_file* i = vcrf->begin; i != vcrf->end; i++)
            auth_3d_camera_root_load(auth, i);
    }

    if (auth_file->chara.end - auth_file->chara.begin > 0) {
        vector_auth_3d_chara* vc = &auth->chara;
        vector_a3da_chara* vcf = &auth_file->chara;
        vector_auth_3d_chara_reserve(vc, vcf->end - vcf->begin);
        for (auth_3d_chara_file* i = vcf->begin; i != vcf->end; i++)
            auth_3d_chara_load(auth, i);
    }

    if (auth_file->curve.end - auth_file->curve.begin > 0) {
        vector_auth_3d_curve* vc = &auth->curve;
        vector_a3da_curve* vcf = &auth_file->curve;
        vector_auth_3d_curve_reserve(vc, vcf->end - vcf->begin);
        for (auth_3d_curve_file* i = vcf->begin; i != vcf->end; i++)
            auth_3d_curve_load(auth, i);
    }

    if (auth_file->event.end - auth_file->event.begin > 0) {
        vector_auth_3d_event* ve = &auth->event;
        vector_a3da_event* vef = &auth_file->event;
        vector_auth_3d_event_reserve(ve, vef->end - vef->begin);
        for (auth_3d_event_file* i = vef->begin; i != vef->end; i++)
            auth_3d_event_load(auth, i);
    }

    if (auth_file->fog.end - auth_file->fog.begin > 0) {
        vector_auth_3d_fog* vf = &auth->fog;
        vector_a3da_fog* vff = &auth_file->fog;
        vector_auth_3d_fog_reserve(vf, vff->end - vff->begin);
        for (auth_3d_fog_file* i = vff->begin; i != vff->end; i++)
            auth_3d_fog_load(auth, i);
    }

    if (auth_file->light.end - auth_file->light.begin > 0) {
        vector_auth_3d_light* vl = &auth->light;
        vector_a3da_light* vlf = &auth_file->light;
        vector_auth_3d_light_reserve(vl, vlf->end - vlf->begin);
        for (auth_3d_light_file* i = vlf->begin; i != vlf->end; i++)
            auth_3d_light_load(auth, i);
    }

    if (auth_file->m_object_hrc.end - auth_file->m_object_hrc.begin > 0) {
        vector_auth_3d_m_object_hrc* vmoh = &auth->m_object_hrc;
        vector_a3da_m_object_hrc* vmohf = &auth_file->m_object_hrc;
        vector_auth_3d_m_object_hrc_reserve(vmoh, vmohf->end - vmohf->begin);
        for (auth_3d_m_object_hrc_file* i = vmohf->begin; i != vmohf->end; i++)
            auth_3d_m_object_hrc_load(auth, i, obj_db);
    }

    if (auth_file->material_list.end - auth_file->material_list.begin > 0) {
        vector_auth_3d_material_list* vml = &auth->material_list;
        vector_a3da_material_list* vmlf = &auth_file->material_list;
        vector_auth_3d_material_list_reserve(vml, vmlf->end - vmlf->begin);
        for (auth_3d_material_list_file* i = vmlf->begin; i != vmlf->end; i++)
            auth_3d_material_list_load(auth, i);
    }

    if (auth_file->motion.end - auth_file->motion.begin > 0) {
        vector_string* vm = &auth->motion;
        vector_string* vmf = &auth_file->motion;
        vector_string_reserve(vm, vmf->end - vmf->begin);
        for (string* i = vmf->begin; i != vmf->end; i++) {
            string* m = vector_string_reserve_back(vm);
            string_copy(i, m);
        }
    }

    if (auth_file->object.end - auth_file->object.begin > 0) {
        vector_auth_3d_object* vo = &auth->object;
        vector_a3da_object* vof = &auth_file->object;
        vector_auth_3d_object_reserve(vo, vof->end - vof->begin);
        for (auth_3d_object_file* i = vof->begin; i != vof->end; i++)
            auth_3d_object_load(auth, i, obj_db, tex_db);
    }

    if (auth_file->object_hrc.end - auth_file->object_hrc.begin > 0) {
        vector_auth_3d_object_hrc* voh = &auth->object_hrc;
        vector_a3da_object_hrc* vohf = &auth_file->object_hrc;
        vector_auth_3d_object_hrc_reserve(voh, vohf->end - vohf->begin);
        for (auth_3d_object_hrc_file* i = vohf->begin; i != vohf->end; i++)
            auth_3d_object_hrc_load(auth, i, obj_db);
    }

    if (auth_file->point.end - auth_file->point.begin > 0) {
        vector_auth_3d_point* vp = &auth->point;
        vector_a3da_point* vpf = &auth_file->point;
        vector_auth_3d_point_reserve(vp, vpf->end - vpf->begin);
        for (auth_3d_point_file* i = vpf->begin; i != vpf->end; i++)
            auth_3d_point_load(auth, i);
    }

    if (auth_file->m_object_hrc_list.end - auth_file->m_object_hrc_list.begin > 0) {
        vector_auth_3d_m_object_hrc* vmoh = &auth->m_object_hrc;
        vector_ptr_auth_3d_m_object_hrc* vmohl = &auth->m_object_hrc_list;
        vector_string* vmohlf = &auth_file->m_object_hrc_list;
        vector_ptr_auth_3d_m_object_hrc_reserve(vmohl, vmohlf->end - vmohlf->begin);
        for (string* i = vmohlf->begin; i != vmohlf->end; i++)
            for (auth_3d_m_object_hrc* j = vmoh->begin; vmoh->begin != vmoh->end; j++)
                if (string_compare(i, &j->name)) {
                    vector_ptr_auth_3d_m_object_hrc_push_back(vmohl, &j);
                    break;
                }
    }

    if (auth_file->object_list.end - auth_file->object_list.begin > 0) {
        vector_auth_3d_object* vo = &auth->object;
        vector_ptr_auth_3d_object* vol = &auth->object_list;
        vector_string* volf = &auth_file->object_list;
        vector_ptr_auth_3d_object_reserve(vol, volf->end - volf->begin);
        for (string* i = volf->begin; i != volf->end; i++)
            for (auth_3d_object* j = vo->begin; vo->begin != vo->end; j++)
                if (string_compare(i, &j->name)) {
                    vector_ptr_auth_3d_object_push_back(vol, &j);
                    break;
                }
    }

    if (auth_file->object_hrc_list.end - auth_file->object_hrc_list.begin > 0) {
        vector_auth_3d_object_hrc* voh = &auth->object_hrc;
        vector_ptr_auth_3d_object_hrc* vohl = &auth->object_hrc_list;
        vector_string* vohlf = &auth_file->object_hrc_list;
        vector_ptr_auth_3d_object_hrc_reserve(vohl, vohlf->end - vohlf->begin);
        for (string* i = vohlf->begin; i != vohlf->end; i++)
            for (auth_3d_object_hrc* j = voh->begin; voh->begin != voh->end; j++)
                if (string_compare(i, &j->name)) {
                    vector_ptr_auth_3d_object_hrc_push_back(vohl, &j);
                    break;
                }
    }

    if (auth->object.end - auth->object.begin > 0) {
        vector_auth_3d_object* vo = &auth->object;
        vector_auth_3d_object_hrc* voh = &auth->object_hrc;
        for (auth_3d_object* i = vo->begin; i != vo->end; i++) {
            if (!i->parent_name.length)
                continue;

            if (i->parent_node.length)
                for (auth_3d_object_hrc* j = voh->begin; j != voh->end; j++) {
                    if (!string_compare(&i->parent_name, &j->name))
                        continue;

                    vector_auth_3d_object_node* von = &j->node;
                    for (auth_3d_object_node* k = von->begin; von->begin != von->end; k++) {
                        if (!string_compare(&i->parent_node, &k->name))
                            continue;

                        int32_t node_index = (int32_t)(k - von->begin);
                        vector_int32_t_push_back(&j->childer_object_parent_node, &node_index);
                        vector_ptr_auth_3d_object_push_back(&j->children_object, &i);
                        break;
                    }
                    break;
                }
            else
                for (auth_3d_object* j = vo->begin; j != vo->end; j++) {
                    if (!string_compare(&i->parent_name, &j->name))
                        continue;

                    vector_ptr_auth_3d_object_push_back(&j->children_object, &i);
                }
        }
    }

    if (auth->object_hrc.end - auth->object_hrc.begin > 0) {
        vector_auth_3d_object* vo = &auth->object;
        vector_auth_3d_object_hrc* voh = &auth->object_hrc;
        for (auth_3d_object_hrc* i = voh->begin; i != voh->end; i++) {
            if (!i->parent_name.length)
                continue;

            if (i->parent_node.length)
                for (auth_3d_object_hrc* j = voh->begin; j != voh->end; j++) {
                    if (!string_compare(&i->parent_name, &j->name))
                        continue;

                    vector_auth_3d_object_node* von = &j->node;
                    for (auth_3d_object_node* k = von->begin; von->begin != von->end; k++) {
                        if (!string_compare(&i->parent_node, &k->name))
                            continue;

                        int32_t node_index = (int32_t)(k - von->begin);
                        vector_int32_t_push_back(&j->childer_object_hrc_parent_node, &node_index);
                        vector_ptr_auth_3d_object_hrc_push_back(&j->children_object_hrc, &i);
                        break;
                    }
                    break;
                }
            else
                for (auth_3d_object* j = vo->begin; j != vo->end; j++) {
                    if (!string_compare(&i->parent_name, &j->name))
                        continue;

                    vector_ptr_auth_3d_object_hrc_push_back(&j->children_object_hrc, &i);
                }
        }
    }

    string_copy(&auth_file->_file_name, &auth->file_name);
}

void auth_3d_load_from_farc(auth_3d* auth, farc* f, char* file,
    object_database* obj_db, texture_database* tex_db) {
    farc_file* ff = farc_read_file(f, file);
    if (!ff)
        return;

    a3da a;
    a3da_init(&a);
    a3da_mread(&a, ff->data, ff->size);
    auth_3d_load(auth, &a, obj_db, tex_db);
    a3da_free(&a);
}

void auth_3d_free(auth_3d* auth) {
    if (!auth)
        return;

    auth->uid = -1;
    auth->id = -1;
    auth->alpha = 1.0f;

    auth_3d_camera_auxiliary_free(&auth->camera_auxiliary);
    auth_3d_dof_free(&auth->dof);
    auth_3d_post_process_free(&auth->post_process);

    vector_auth_3d_ambient_free(&auth->ambient, auth_3d_ambient_free);
    vector_string_free(&auth->auth_2d, string_free);
    vector_auth_3d_camera_root_free(&auth->camera_root, auth_3d_camera_root_free);
    vector_auth_3d_chara_free(&auth->chara, auth_3d_chara_free);
    vector_auth_3d_curve_free(&auth->curve, auth_3d_curve_free);
    vector_auth_3d_event_free(&auth->event, auth_3d_event_free);
    vector_auth_3d_fog_free(&auth->fog, auth_3d_fog_free);
    vector_auth_3d_light_free(&auth->light, auth_3d_light_free);
    vector_auth_3d_m_object_hrc_free(&auth->m_object_hrc, auth_3d_m_object_hrc_free);
    vector_ptr_auth_3d_m_object_hrc_free(&auth->m_object_hrc_list, 0);
    vector_auth_3d_material_list_free(&auth->material_list, auth_3d_material_list_free);
    vector_string_free(&auth->motion, string_free);
    vector_auth_3d_object_free(&auth->object, auth_3d_object_free);
    vector_auth_3d_object_hrc_free(&auth->object_hrc, auth_3d_object_hrc_free);
    vector_ptr_auth_3d_object_hrc_free(&auth->object_hrc_list, 0);
    vector_ptr_auth_3d_object_free(&auth->object_list, 0);
    vector_auth_3d_point_free(&auth->point, auth_3d_point_free);

    string_free(&auth->file_name);
}

void auth_3d_farc_init(auth_3d_farc* f) {
    if (!f)
        return;

    memset(f, 0, sizeof(auth_3d_farc));
}

void auth_3d_farc_load(auth_3d_farc* f, void* data, char* name, object_database* obj_db) {
    string file;
    string_init(&file, name);
    string_add_length(&file, ".farc", 5);
    data_struct_load_file(data, &f->auth_3d_farc,
        "rom\\auth_3d\\", string_data(&file), farc_load_file);
    string_free(&file);

    bool found_base = false;
    f->obj_set_id = vector_empty(uint32_t);
    vector_uint32_t* obj_set_id = &f->obj_set_id;
    for (farc_file* i = f->auth_3d_farc.files.begin; i != f->auth_3d_farc.files.end; i++) {
        char* name_str = string_data(&i->name);
        size_t name_len = i->name.length;

        string name = string_empty;
        char* s = 0;
        if (s = strchr(name_str, '_'))
            string_init_length(&name, name_str, s - name_str);
        else
            string_init_length(&name, name_str, name_len);

        object_set_info* set_info;
        if (!object_database_get_object_set_info(obj_db, string_data(&name), &set_info)) {
            string_free(&name);
            continue;
        }

        string_free(&name);

        uint32_t set_id = set_info->id;
        for (uint32_t* j = obj_set_id->begin; j != obj_set_id->end; j++)
            if (set_id == *j) {
                set_id = -1;
                break;
            }

        if (set_id == -1)
            continue;

        if (!found_base) {
            char* n = string_data(&set_info->name);
            size_t n_len = set_info->name.length;

            char set_name[11];
            set_name[0] = 0;
            if (!str_utils_compare_length(n, n_len, "STGPV", 5)) {
                memcpy(set_name, n, 8);
                set_name[8] = 0;
                found_base = true;
            }
            else if (!str_utils_compare_length(n, n_len, "STGD2PV", 7)) {
                memcpy(set_name, n, 10);
                set_name[10] = 0;
                found_base = true;
            }

            if (found_base && object_database_get_object_set_info(obj_db, set_name, &set_info)) {
                object_set_load_by_db_index(&set_info, data, obj_db, set_info->id);
                vector_uint32_t_push_back(obj_set_id, &set_info->id);
                found_base = true;
            }
            else
                found_base = false;
        }

        object_set_load_by_db_index(&set_info, data, obj_db, set_id);
        vector_uint32_t_push_back(obj_set_id, &set_id);
    }

    char* name_str = name;
    size_t name_len = utf8_length(name);

    object_set_info* set_info;
    if (!memcmp(name_str, "ITMPV", 5)
        && object_database_get_object_set_info(obj_db, name_str, &set_info)) {
        object_set_load_by_db_index(&set_info, data, obj_db, set_info->id);
        vector_uint32_t_push_back(obj_set_id, &set_info->id);
    }

    if (!memcmp(name_str, "EFFCHRPV", 8)
        && object_database_get_object_set_info(obj_db, name_str, &set_info)) {
        object_set_load_by_db_index(&set_info, data, obj_db, set_info->id);
        vector_uint32_t_push_back(obj_set_id, &set_info->id);
    }

    string obj = string_empty;
    for (size_t i = 0; i < name_len && name_len - i >= 8; i++)
        if (!memcmp(&name_str[i], "STGPV", 5)) {
            string_free(&obj);
            string_init_length(&obj, &name_str[i], 8);
            break;
        }

    for (size_t i = 0; i < name_len && name_len - i >= 10; i++)
        if (!memcmp(&name_str[i], "STGD2PV", 7)) {
            string_free(&obj);
            string_init_length(&obj, &name_str[i], 10);
            break;
        }

    if (object_database_get_object_set_info(obj_db, string_data(&obj), &set_info)) {
        object_set_load_by_db_index(&set_info, data, obj_db, set_info->id);
        vector_uint32_t_push_back(obj_set_id, &set_info->id);
    }

    string_add_length(&obj, "HRC", 3);
    if (object_database_get_object_set_info(obj_db, string_data(&obj), &set_info)) {
        object_set_load_by_db_index(&set_info, data, obj_db, set_info->id);
        vector_uint32_t_push_back(obj_set_id, &set_info->id);
    }
    string_free(&obj);
}

void auth_3d_farc_free(auth_3d_farc* f) {
    farc_free(&f->auth_3d_farc);
    vector_uint32_t_free(&f->obj_set_id, 0);
}

void auth_3d_key_free(auth_3d_key* k) {
    vector_kft3_free(&k->keys_vec, 0);
}

void auth_3d_rgba_free(auth_3d_rgba* rgba) {
    if (rgba->flags & AUTH_3D_RGBA_R)
        auth_3d_key_free(&rgba->r);
    if (rgba->flags & AUTH_3D_RGBA_G)
        auth_3d_key_free(&rgba->g);
    if (rgba->flags & AUTH_3D_RGBA_B)
        auth_3d_key_free(&rgba->b);
    if (rgba->flags & AUTH_3D_RGBA_A)
        auth_3d_key_free(&rgba->a);
}

void auth_3d_vec3_free(auth_3d_vec3* vec) {
    auth_3d_key_free(&vec->x);
    auth_3d_key_free(&vec->y);
    auth_3d_key_free(&vec->z);
}

void auth_3d_model_transform_free(auth_3d_model_transform* mt) {
    auth_3d_vec3_free(&mt->rotation);
    auth_3d_vec3_free(&mt->scale);
    auth_3d_vec3_free(&mt->translation);
    auth_3d_key_free(&mt->visibility);
}

void auth_3d_ambient_free(auth_3d_ambient* a) {
    if (a->flags & AUTH_3D_AMBIENT_LIGHT_DIFFUSE)
        auth_3d_rgba_free(&a->light_diffuse);
    string_free(&a->name);
    if (a->flags & AUTH_3D_AMBIENT_RIM_LIGHT_DIFFUSE)
        auth_3d_rgba_free(&a->rim_light_diffuse);
}

void auth_3d_camera_auxiliary_free(auth_3d_camera_auxiliary* ca) {
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_AUTO_EXPOSURE)
        auth_3d_key_free(&ca->auto_exposure);
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_EXPOSURE)
        auth_3d_key_free(&ca->exposure);
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_EXPOSURE_RATE)
        auth_3d_key_free(&ca->exposure_rate);
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_GAMMA)
        auth_3d_key_free(&ca->gamma);
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_GAMMA_RATE)
        auth_3d_key_free(&ca->gamma_rate);
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_SATURATE)
        auth_3d_key_free(&ca->saturate);
}

void auth_3d_camera_root_free(auth_3d_camera_root* cr) {
    auth_3d_model_transform_free(&cr->interest);
    auth_3d_model_transform_free(&cr->model_transform);
    auth_3d_camera_root_view_point_free(&cr->view_point);
}

void auth_3d_camera_root_view_point_free(auth_3d_camera_root_view_point* crvp) {
    if (crvp->flags & AUTH_3D_CAMERA_ROOT_VIEW_POINT_FOV)
        auth_3d_key_free(&crvp->fov);
    else
        auth_3d_key_free(&crvp->focal_length);
    auth_3d_model_transform_free(&crvp->model_transform);
    if (crvp->flags & AUTH_3D_CAMERA_ROOT_VIEW_POINT_ROLL)
        auth_3d_key_free(&crvp->roll);
}

void auth_3d_chara_free(auth_3d_chara* c) {
    auth_3d_model_transform_free(&c->model_transform);
    string_free(&c->name);
}

void auth_3d_curve_free(auth_3d_curve* c) {
    auth_3d_key_free(&c->curve);
    string_free(&c->name);
}

void auth_3d_dof_free(auth_3d_dof* d) {
    auth_3d_model_transform_free(&d->model_transform);
}

void auth_3d_event_free(auth_3d_event* e) {
    string_free(&e->name);
    string_free(&e->param_1);
    string_free(&e->ref);
}

void auth_3d_fog_free(auth_3d_fog* f) {
    if (f->flags & AUTH_3D_FOG_COLOR)
        auth_3d_rgba_free(&f->color);
    if (f->flags & AUTH_3D_FOG_DENSITY)
        auth_3d_key_free(&f->density);
    if (f->flags & AUTH_3D_FOG_END)
        auth_3d_key_free(&f->end);
    if (f->flags & AUTH_3D_FOG_START)
        auth_3d_key_free(&f->start);
}

void auth_3d_light_free(auth_3d_light* l) {
    if (l->flags & AUTH_3D_LIGHT_AMBIENT)
        auth_3d_rgba_free(&l->ambient);
    if (l->flags & AUTH_3D_LIGHT_CONE_ANGLE)
        auth_3d_key_free(&l->cone_angle);
    if (l->flags & AUTH_3D_LIGHT_CONSTANT)
        auth_3d_key_free(&l->constant);
    if (l->flags & AUTH_3D_LIGHT_DIFFUSE)
        auth_3d_rgba_free(&l->diffuse);
    if (l->flags & AUTH_3D_LIGHT_DROP_OFF)
        auth_3d_key_free(&l->drop_off);
    if (l->flags & AUTH_3D_LIGHT_FAR)
        auth_3d_key_free(&l->_far);
    if (l->flags & AUTH_3D_LIGHT_INTENSITY)
        auth_3d_key_free(&l->intensity);
    if (l->flags & AUTH_3D_LIGHT_LINEAR)
        auth_3d_key_free(&l->linear);
    if (l->flags & AUTH_3D_LIGHT_POSITION)
        auth_3d_model_transform_free(&l->position);
    if (l->flags & AUTH_3D_LIGHT_QUADRATIC)
        auth_3d_key_free(&l->quadratic);
    if (l->flags & AUTH_3D_LIGHT_SPECULAR)
        auth_3d_rgba_free(&l->specular);
    if (l->flags & AUTH_3D_LIGHT_SPOT_DIRECTION)
        auth_3d_model_transform_free(&l->spot_direction);
    if (l->flags & AUTH_3D_LIGHT_TONE_CURVE)
        auth_3d_rgba_free(&l->tone_curve);
    string_free(&l->type);
}

void auth_3d_m_object_hrc_free(auth_3d_m_object_hrc* moh) {
    vector_auth_3d_object_instance_free(&moh->instance, auth_3d_object_instance_free);
    auth_3d_object_model_transform_free(&moh->model_transform);
    string_free(&moh->name);
    vector_auth_3d_object_node_free(&moh->node, auth_3d_object_node_free);
}

void auth_3d_material_list_free(auth_3d_material_list* ml) {
    if (ml->flags & AUTH_3D_MATERIAL_LIST_BLEND_COLOR)
        auth_3d_rgba_free(&ml->blend_color);
    if (ml->flags & AUTH_3D_MATERIAL_LIST_GLOW_INTENSITY)
        auth_3d_key_free(&ml->glow_intensity);
    if (ml->flags & AUTH_3D_MATERIAL_LIST_INCANDESCENCE)
        auth_3d_rgba_free(&ml->incandescence);
    string_free(&ml->name);
}

void auth_3d_object_free(auth_3d_object* o) {
    vector_ptr_auth_3d_object_free(&o->children_object, 0);
    vector_ptr_auth_3d_object_hrc_free(&o->children_object_hrc, 0);
    auth_3d_model_transform_free(&o->model_transform);
    auth_3d_object_curve_free(&o->morph);
    string_free(&o->name);
    string_free(&o->parent_name);
    string_free(&o->parent_node);
    auth_3d_object_curve_free(&o->pattern);
    vector_auth_3d_object_texture_pattern_free(&o->texture_pattern,
        auth_3d_object_texture_pattern_free);
    vector_auth_3d_object_texture_transform_free(&o->texture_transform,
        auth_3d_object_texture_transform_free);
    string_free(&o->uid_name);
}

void auth_3d_object_curve_free(auth_3d_object_curve* oc) {
    string_free(&oc->name);
}

void auth_3d_object_hrc_free(auth_3d_object_hrc* oh) {
    vector_ptr_auth_3d_object_free(&oh->children_object, 0);
    vector_ptr_auth_3d_object_hrc_free(&oh->children_object_hrc, 0);
    vector_int32_t_free(&oh->childer_object_hrc_parent_node, 0);
    vector_int32_t_free(&oh->childer_object_parent_node, 0);
    vector_mat4_free(&oh->mats, 0);
    string_free(&oh->name);
    vector_auth_3d_object_node_free(&oh->node, auth_3d_object_node_free);
    string_free(&oh->parent_name);
    string_free(&oh->parent_node);
    string_free(&oh->uid_name);
}

void auth_3d_object_instance_free(auth_3d_object_instance* oi) {
    vector_mat4_free(&oi->mats, 0);
    auth_3d_object_model_transform_free(&oi->model_transform);
    string_free(&oi->name);
    vector_int32_t_free(&oi->object_bone_indices, 0);
    string_free(&oi->uid_name);
}

void auth_3d_object_model_transform_free(auth_3d_object_model_transform* omt) {
    auth_3d_vec3_free(&omt->translation);
    auth_3d_vec3_free(&omt->rotation);
    auth_3d_vec3_free(&omt->scale);
    auth_3d_key_free(&omt->visibility);
}

void auth_3d_object_node_free(auth_3d_object_node* on) {
    auth_3d_object_model_transform_free(&on->model_transform);
    string_free(&on->name);
}

void auth_3d_object_texture_pattern_free(auth_3d_object_texture_pattern* otp) {
    string_free(&otp->name);
    auth_3d_object_curve_free(&otp->pattern);
}

void auth_3d_object_texture_transform_free(auth_3d_object_texture_transform* ott) {
    if (ott->flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_COVERAGE_U)
        auth_3d_key_free(&ott->coverage_u);
    if (ott->flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_COVERAGE_V)
        auth_3d_key_free(&ott->coverage_v);
    if (ott->flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_OFFSET_U)
        auth_3d_key_free(&ott->offset_u);
    if (ott->flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_OFFSET_V)
        auth_3d_key_free(&ott->offset_v);
    string_free(&ott->name);
    if (ott->flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_REPEAT_U)
        auth_3d_key_free(&ott->repeat_u);
    if (ott->flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_REPEAT_V)
        auth_3d_key_free(&ott->repeat_v);
    if (ott->flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_ROTATE)
        auth_3d_key_free(&ott->rotate);
    if (ott->flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_ROTATE_FRAME)
        auth_3d_key_free(&ott->rotate_frame);
    if (ott->flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_TRANSLATE_FRAME_U)
        auth_3d_key_free(&ott->translate_frame_u);
    if (ott->flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_TRANSLATE_FRAME_V)
        auth_3d_key_free(&ott->translate_frame_v);
}

void auth_3d_point_free(auth_3d_point* p) {
    auth_3d_model_transform_free(&p->model_transform);
    string_free(&p->name);
}

void auth_3d_post_process_free(auth_3d_post_process* pp) {
    if (pp->flags & AUTH_3D_POST_PROCESS_INTENSITY)
        auth_3d_rgba_free(&pp->intensity);
    if (pp->flags & AUTH_3D_POST_PROCESS_LENS_FLARE)
        auth_3d_key_free(&pp->lens_flare);
    if (pp->flags & AUTH_3D_POST_PROCESS_LENS_GHOST)
        auth_3d_key_free(&pp->lens_ghost);
    if (pp->flags & AUTH_3D_POST_PROCESS_LENS_SHAFT)
        auth_3d_key_free(&pp->lens_shaft);
    if (pp->flags & AUTH_3D_POST_PROCESS_RADIUS)
        auth_3d_rgba_free(&pp->radius);
    if (pp->flags & AUTH_3D_POST_PROCESS_SCENE_FADE)
        auth_3d_rgba_free(&pp->scene_fade);
}
/*
void __fastcall sub_140248480(string *a1, list_int32_t *a2)
{
    list_int32_t_node *v4; // rcx
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
    vector_int32_t uids; // [rsp+70h] [rbp+7h] BYREF

    v49 = -2i64;
    v4 = a2->head->next;
    a2->head->next = a2->head;
    a2->head->prev = a2->head;
    a2->size = 0i64;
    if ( v4 != a2->head )
    {
        do
        {
            v5 = v4->next;
            j_free_2(v4);
            v4 = v5;
        }
        while ( v5 != a2->head );
    }
    uids.begin = 0i64;
    uids.end = 0i64;
    uids.capacity_end = 0i64;
    if ( a1->capacity < 0x10 )
        v6 = (char *)a1;
    else
        v6 = a1->data.ptr;
    sub_1401CBC70(v6, &uids);
    for ( i = uids.begin; i != uids.end; ++i )
    {
        v8 = sub_1401E7E90(*i);
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
                            j_free_2(Memory.data.ptr);
                    }
                    break;
                }
                v10 += v11 - v12 - 1;
                v11 = v12 + 1;
            }
        }
        if ( Dst.capacity >= 0x10 )
            j_free_2(Dst.data.ptr);
    }
    if ( !string_compare_ptr_char(a1, "ITMPV", 0i64, 5ui64) )
    {
        sub_1400215A0(a1, &Memory, 5i64, -1i64);
        v21 = sub_1400D29A0(&Memory, 0xAu);
        v22 = task_pv_db_get_pv(v21);
        if ( v22 )
        {
            v23 = v22->difficulty[2].begin;
            if ( v22->difficulty[2].end - v23 )
            {
                v51.data = 0ui64;
                v51.length = 0i64;
                sub_14023FDE0((vector_pv_db_pv_pv_item *)&v51, &v23->pv_item);
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
                                v29 = sub_14023B3E0(&a2->head, a2->head, a2->head->prev, &a4);
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
                                j_free_2(ADJ(v31)->name.data.ptr);
                            ADJ(v31)->name.capacity = 15i64;
                            ADJ(v31)->name.length = 0i64;
                            ADJ(v31++)->name.data.data[0] = 0;
                        }
                        while ( ADJ(v31) != *(pv_item *__shifted(pv_item,0x20) *)&v24.data[8] );
                    }
                    j_free_2(v24.ptr);
                }
            }
        }
        if ( Memory.capacity >= 0x10 )
            j_free_2(Memory.data.ptr);
    }
    if ( !string_compare_ptr_char(a1, "EFFCHRPV", 0i64, 8ui64) )
    {
        v32 = (char *)(a1->capacity < 0x10 ? a1 : a1->data.ptr);
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
    v36 = string_compare_ptr_char(a1, "STGPV", 0i64, 5ui64);
    if ( v36 != -1 )
    {
        v37 = sub_1400215A0(a1, &Memory, v36, 8i64);
        if ( &v51 != v37 )
        {
            if ( v51.capacity >= 0x10 )
                j_free_2(v51.data.ptr);
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
            j_free_2(Memory.data.ptr);
    }
    v38 = string_compare_ptr_char(a1, "STGD2PV", 0i64, 7ui64);
    if ( v38 != -1 )
    {
        v39 = sub_1400215A0(a1, &Memory, v38, 10i64);
        if ( &v51 != v39 )
        {
            if ( v51.capacity >= 0x10 )
                j_free_2(v51.data.ptr);
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
            j_free_2(Memory.data.ptr);
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
        j_free_2(Memory.data.ptr);
    if ( v51.capacity >= 0x10 )
        j_free_2(v51.data.ptr);
    if ( uids.begin )
        j_free_2(uids.begin);
}

void sub_140248480(string* a1, vector_uint32_t* a2) {
    char* v6; // rcx
    int32_t* i; // rbx
    char* v8; // rax
    size_t v9; // r8
    size_t v10; // rsi
    char* v11; // rdi
    char* v12; // rax
    char* v13; // r9
    int v14; // ecx
    char* v15; // rax
    ssize_t v16; // r9
    char* v17; // rcx
    list_int32_t_node* v18; // rdi
    list_int32_t_node* v19; // rax
    size_t v20; // rdx
    int v21; // eax
    ssize_t* v22; // rax
    __int64 v23; // rcx
    string_union v24; // kr00_16
    string* v25; // rbx
    string* v26; // rcx
    object_info v27; // eax
    list_int32_t_node* v28; // rdi
    list_int32_t_node* v29; // rax
    size_t v30; // rdx
    string* v31; // rbx
    char* v32; // rcx
    list_int32_t_node* v33; // rbx
    list_int32_t_node* v34; // rdx
    size_t v35; // rax
    __int64 v36; // rax
    string* v37; // rbx
    __int64 v38; // rax
    string* v39; // rbx
    char* v40; // rcx
    list_int32_t_node* v41; // rbx
    list_int32_t_node* v42; // rdx
    size_t v43; // rax
    char* v44; // rcx
    list_int32_t_node* v45; // rbx
    list_int32_t_node* v46; // rcx
    size_t v47; // rax
    int32_t a4; // [rsp+20h] [rbp-49h] BYREF
    __int64 v49; // [rsp+28h] [rbp-41h]
    string Dst; // [rsp+30h] [rbp-39h] BYREF
    vector_int32_t uids; // [rsp+70h] [rbp+7h] BYREF

    v49 = -2;
    vector_uint32_t_free(a2, 0);

    uids.begin = 0;
    uids.end = 0;
    uids.capacity_end = 0;
    v6 = string_data(a1);
    sub_1401CBC70(v6, &uids);
    for (i = uids.begin; i != uids.end; i++) {
        v8 = sub_1401E7E90(*i);
        string_init(&Dst, v8);
        v10 = Dst.length;
        if (Dst.length)
        {
            v11 = string_data(&Dst);
            while (v10)
            {
                v12 = memchr(v11, '_', v10);
                v13 = v12;
                if (!v12)
                    break;
                if (*v12 == "_"[0])
                    v14 = 0;
                else
                    v14 = *v12 < "_"[0] ? -1 : 1;
                if (!v14)
                {
                    v15 = string_data(&Dst);
                    v16 = v13 - v15;
                    if (v16 != -1)
                    {
                        sub_1400215A0(&Dst, &Memory, 0, v16);
                        v17 = string_data(&Memory);
                        a4 = obj_database_get_object_set(v17);
                        if (a4 != -1)
                            vector_uint32_t_push_back(a2, &a4);
                        string_free(&Memory);
                    }
                    break;
                }
                v10 += v11 - v12 - 1;
                v11 = v12 + 1;
            }
        }
        string_free(&Dst);
    }

    if (!str_utils_get_substring_offset(string_data(a1), a1->length, 0, "ITMPV", 5)) {
        string_init_length(&Dst, string_data(a1), 8);
        sscanf_s(string_data(a1) + 5, "%03d", &v21);
        v22 = sub_1404E6220(v21);
        if (v22) {
            v23 = v22[28];
            if ((v22[29] - v23) / 1248) {
                Dst.data = 0;
                Dst.length = 0;
                sub_14023FDE0(&Dst, (_QWORD*)(v23 + 504));
                v24 = Dst.data;
                if (Dst.data.ptr != *(char**)&Dst.data[8]) {
                    v25 = (string*)(Dst.data.ptr + 8);
                    do {
                        if (SLODWORD(v25[-1].max_length) > 0) {
                            v26 = string_data(v25);
                            v27 = obj_database_get_object_info((char*)v26);
                            if (v27.id != -1 && v27.set_id != -1)
                                vector_uint32_t_push_back(a2, &v27.set_id);
                        }
                        v25 = (string*)((char*)v25 + 40);
                    } while (&v25[-1].max_length != (size_t*)*(string**)&v24.data[8]);
                }
                if (v24.ptr) {
                    if (v24.ptr != *(char**)&v24.data[8]) {
                        v31 = (string * __shifted(string, 0x18))(v24.ptr + 32);
                        do {
                            if (v31->max_length >= 0x10)
                                j_free_2(v31->data.ptr);
                            v31->max_length = 15;
                            v31->length = 0;
                            v31->data[0] = 0;
                            v31 = (string * __shifted(string, 0x18))((char*)v31 + 40);
                        } while (&v31[-1] != *(string * __shifted(string, 0x18)*) & v24.data[8]);
                    }
                    j_free_2(v24.ptr);
                }
            }
        }
    }
    if (!str_utils_get_substring_offset(string_data(a1), a1->length, 0, "EFFCHRPV", 8)) {
        v32 = string_data(a1->data);
        a4 = obj_database_get_object_set(v32);
        if (a4 != -1)
            vector_uint32_t_push_back(a2, &a4);
    }
    sub_14023EF10(a2);
    sub_14023F860(a2);

    string_init(&Dst, 0);
    v36 = str_utils_get_substring_offset(string_data(a1), a1->length, 0, "STGPV", 5);
    if (v36 != -1)
        string_init_length(&Dst, string_data(a1) + v36, 8);
    v38 = str_utils_get_substring_offset(string_data(a1), a1->length, 0, "STGD2PV", 7);
    if (v38 != -1)
        string_init_length(&Dst, string_data(a1) + v36, 10);

    if (Dst.length) {
        v40 = string_data(&Dst);
        a4 = obj_database_get_object_set(v40);
        if (a4 != -1)
            vector_uint32_t_push_back(a2, &a4);
    }

    string_add_length(&Dst, "HRC", 3);
    v44 = string_data(&Dst);
    a4 = obj_database_get_object_set(v44);
    if (a4 != -1)
        vector_uint32_t_push_back(a2, &a4);

    string_free(&Dst);
    vector_uint32_t_free(&uids, 0);
}
*/
void auth_3d_data_init() {
    memset(&auth_3d_data, 0, sizeof(auth_3d_data_struct));
    for (int32_t i = 0; i < AUTH_3D_DATA_COUNT; i++)
        auth_3d_free(&auth_3d_data.data[i]);
}

inline int32_t auth_3d_data_load_uid(int32_t uid, auth_3d_database* auth_3d_db) {
    if (uid >= auth_3d_db->uid.end - auth_3d_db->uid.begin
        || !auth_3d_db->uid.begin[uid].enabled)
        return -1;

    int32_t index = 0;
    while (auth_3d_data.data[index].uid != -1)
        if (++index >= AUTH_3D_DATA_COUNT)
            return -1;

    auth_3d_free(&auth_3d_data.data[index]);

    if (++auth_3d_load_counter < 0)
        auth_3d_load_counter = 1;

    int32_t id = ((auth_3d_load_counter & 0x7FFF) << 16) | index & 0x7FFF;
    auth_3d_data.data[index].uid = uid;
    auth_3d_data.data[index].id = id;
    vector_int32_t_push_back(&auth_3d_data.loaded_ids, &id);
    return id;
}

inline auth_3d* auth_3d_data_get_auth_3d(int32_t id) {
    int32_t index = 0;
    while (auth_3d_data.data[index].id != id)
        if (++index >= AUTH_3D_DATA_COUNT)
            return 0;

    return &auth_3d_data.data[index];
}

mat4* auth_3d_data_struct_get_auth_3d_object_mat(int32_t id, size_t index, bool hrc, mat4* mat) {
    if (id < 0 || (id & 0x7FFF) >= AUTH_3D_DATA_COUNT)
        return 0;

    auth_3d* auth = &auth_3d_data.data[id & 0x7FFF];
    if (auth->id != id || !auth->enable)
        return 0;

    if (hrc)
        return auth_3d_get_auth_3d_object_hrc_bone_mats(auth, index);

    if ((ssize_t)index < auth->object.end - auth->object.begin) {
        mat4u_to_mat4(&auth->object.begin[index].model_transform.mat, mat);
        return mat;
    }
    return 0;
}


int32_t auth_3d_data_get_auth_3d_id_by_object_info(object_info obj_info, uint64_t* mesh_index, bool* hrc) {
    for (int32_t* i = auth_3d_data.loaded_ids.begin;
        i != auth_3d_data.loaded_ids.end; i++) {
        if (*i < 0 || (*i & 0x7FFF) >= AUTH_3D_DATA_COUNT)
            continue;

        auth_3d* auth = &auth_3d_data.data[*i & 0x7FFF];
        if (auth->id != *i || !auth->enable)
            continue;

        int32_t obj_hrc_mesh_index = auth_3d_get_auth_3d_object_hrc_index_by_object_info(auth, obj_info);
        if (obj_hrc_mesh_index >= 0) {
            if (mesh_index)
                *mesh_index = obj_hrc_mesh_index;
            if (hrc)
                *hrc = true;
            return *i;
        }

        int32_t obj_mesh_index = auth_3d_get_auth_3d_object_index_by_object_info(auth, obj_info);
        if (obj_mesh_index >= 0)
        {
            if (mesh_index)
                *mesh_index = obj_mesh_index;
            if (hrc)
                *hrc = false;
            return *i;
        }
    }
    return -1;
}

void auth_3d_data_unload_id(int32_t id, render_context* rctx) {
    if (id < 0 || (id & 0x7FFF) >= AUTH_3D_DATA_COUNT)
        return;

    auth_3d* auth = &auth_3d_data.data[id & 0x7FFF];
    if (!auth || auth->id != id)
        return;

    int32_t* v5 = auth_3d_data.loaded_ids.end;
    int32_t* v6 = auth_3d_data.loaded_ids.begin;
    if (v6 == v5)
        return;

    int32_t index = 0;
    while (*v6 != id)
        if (++index >= AUTH_3D_DATA_COUNT)
            return;
        else
            v6++;

    if (v6 != v5) {
        vector_int32_t_erase(&auth_3d_data.loaded_ids, v6 - auth_3d_data.loaded_ids.begin, 0);
        auth_3d_unload(auth, rctx);
    }
}

void auth_3d_data_free() {
    int32_t index = 0;
    while (true)
        if (++index >= AUTH_3D_DATA_COUNT)
            break;
        else
            auth_3d_free(&auth_3d_data.data[index]);
    vector_int32_t_free(&auth_3d_data.loaded_ids, 0);
}

static kft3* auth_3d_key_find_keyframe(auth_3d_key* data, float_t frame) {
    kft3* key = data->keys;
    size_t length = data->length;
    size_t temp;
    while (length > 0) {
        if (frame < key[temp = length / 2].frame)
            length = temp;
        else {
            key += temp + 1;
            length -= temp + 1;
        }
    }
    return key;
}

static float_t auth_3d_key_get_value(auth_3d_key* data, float_t frame) {
    if (data->type == AUTH_3D_KEY_STATIC)
        return data->value;
    else if (data->type < AUTH_3D_KEY_STATIC || data->type > AUTH_3D_KEY_HOLD || !data->length)
        return 0.0f;

    kft3* first_key = &data->keys[0];
    kft3* last_key = &data->keys[data->length - 1];
    float_t first_frame = first_key->frame;
    float_t last_frame = last_key->frame;
    float_t frame_delta = data->frame_delta;

    size_t v4 = data->length;
    float_t offset = 0.0f;
    if (frame < first_frame) {
        float_t delta_frame = first_frame - frame;
        switch (data->ep_type_pre) {
        case A3DA_EP_NONE:
        default:
            data->frame = frame;
            data->value_interp = first_key->value;
            return data->value_interp;
        case A3DA_EP_LINEAR:
            data->frame = frame;
            data->value_interp = first_key->value + delta_frame * first_key->tangent1;
            return data->value_interp;
        case A3DA_EP_CYCLE:
            frame = last_frame - fmodf(delta_frame, frame_delta);
            break;
        case A3DA_EP_CYCLE_OFFSET:
            offset = -(float_t)((int32_t)(delta_frame / frame_delta + 1.0)) * data->value_delta;
            frame = last_frame - fmodf(delta_frame, frame_delta);
            break;
        }
    }
    else if (frame >= last_frame) {
        float_t delta_frame = frame - last_frame;
        switch (data->ep_type_post) {
        case A3DA_EP_NONE:
        default:
            data->frame = frame;
            data->value_interp = last_key->value;
            return data->value_interp;
        case A3DA_EP_LINEAR:
            data->frame = frame;
            data->value_interp = last_key->value - delta_frame * last_key->tangent2;
            return data->value_interp;
        case A3DA_EP_CYCLE:
            frame = first_frame + fmodf(delta_frame, frame_delta);
            break;
        case A3DA_EP_CYCLE_OFFSET:
            offset = (float_t)((int32_t)(delta_frame / frame_delta + 1.0)) * data->value_delta;
            frame = first_frame + fmodf(delta_frame, frame_delta);
            break;
        }
    }

    data->frame = frame;
    data->value_interp = auth_3d_key_interpolate(data, frame) + offset;
    return data->value_interp;
}

static float_t auth_3d_key_interpolate(auth_3d_key* data, float_t frame) {
    kft3* first_key = data->keys;
    kft3* key = auth_3d_key_find_keyframe(data, frame);

    if (key == first_key)
        return first_key->value;
    else if (key == &first_key[data->length])
        return key[-1].value;
    else
        return auth_3d_interpolate_value(data->type, frame, key - 1, key);
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

static void auth_3d_key_load(auth_3d* auth, auth_3d_key* k, auth_3d_key_file* kf) {
    k->max_frame = auth->max_frame;
    k->frame_delta = k->max_frame;
    k->value_delta = 0.0f;

    ssize_t length = kf->keys.end - kf->keys.begin;
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

    k->keys_vec = vector_empty(kft3);
    if (length > 1) {
        vector_kft3_insert_range(&k->keys_vec, 0, kf->keys.begin, kf->keys.end);
        k->length = length;
        k->keys = k->keys_vec.begin;

        kft3* first_key = &k->keys[0];
        kft3* last_key = &k->keys[length - 1];
        if (first_key->frame < last_key->frame
            && last_key->frame > 0.0f && k->max_frame > first_key->frame) {
            k->ep_type_pre = kf->ep_type_pre;
            k->ep_type_post = kf->ep_type_post;
            k->frame_delta = last_key->frame - first_key->frame;
            k->value_delta = last_key->value - first_key->value;
        }
    }
    else if (length == 1) {
        float_t value = kf->keys.begin[0].value;
        k->type = value != 0.0f ? AUTH_3D_KEY_STATIC : AUTH_3D_KEY_NONE;
        k->value = value;
    }
    else {
        k->type = AUTH_3D_KEY_NONE;
        k->value = 0.0f;
    }
}

static void auth_3d_rgba_get_value(auth_3d_rgba* rgba, float_t frame) {
    if (rgba->flags & A3DA_RGBA_R)
        rgba->value.x = auth_3d_key_get_value(&rgba->r, frame);

    if (rgba->flags & A3DA_RGBA_G)
        rgba->value.y = auth_3d_key_get_value(&rgba->g, frame);

    if (rgba->flags & A3DA_RGBA_B)
        rgba->value.z = auth_3d_key_get_value(&rgba->b, frame);

    if (rgba->flags & A3DA_RGBA_A)
        rgba->value.w = auth_3d_key_get_value(&rgba->a, frame);
}

static void auth_3d_rgba_load(auth_3d* auth, auth_3d_rgba* rgba, auth_3d_rgba_file* rgbaf) {
    if (rgbaf->flags & A3DA_RGBA_R) {
        auth_3d_key_load(auth, &rgba->r, &rgbaf->r);
        rgba->flags |= AUTH_3D_RGBA_R;
    }

    if (rgbaf->flags & A3DA_RGBA_G) {
        auth_3d_key_load(auth, &rgba->g, &rgbaf->g);
        rgba->flags |= AUTH_3D_RGBA_G;
    }

    if (rgbaf->flags & A3DA_RGBA_B) {
        auth_3d_key_load(auth, &rgba->b, &rgbaf->b);
        rgba->flags |= AUTH_3D_RGBA_B;
    }

    if (rgbaf->flags & A3DA_RGBA_A) {
        auth_3d_key_load(auth, &rgba->a, &rgbaf->a);
        rgba->flags |= AUTH_3D_RGBA_A;
    }
}

static void auth_3d_vec3_get_value(auth_3d_vec3* vec, float_t frame, vec3* value) {
    value->x = auth_3d_key_get_value(&vec->x, frame);
    value->y = auth_3d_key_get_value(&vec->y, frame);
    value->z = auth_3d_key_get_value(&vec->z, frame);
}

static void auth_3d_vec3_load(auth_3d* auth, auth_3d_vec3* vec, auth_3d_vec3_file* vecf) {
    auth_3d_key_load(auth, &vec->x, &vecf->x);
    auth_3d_key_load(auth, &vec->y, &vecf->y);
    auth_3d_key_load(auth, &vec->z, &vecf->z);
}

static void auth_3d_model_transform_get_value(auth_3d_model_transform* mt, float_t frame) {
    auth_3d_vec3_get_value(&mt->scale, frame, &mt->scale_value);
    auth_3d_vec3_get_value(&mt->rotation, frame, &mt->rotation_value);
    auth_3d_vec3_get_value(&mt->translation, frame, &mt->translation_value);
    mt->visible = auth_3d_key_get_value(&mt->visibility, frame) >= 0.99900001f;
    if (mt->visible && (mt->scale_value.x == 0.0f
        || mt->scale_value.y == 0.0f || mt->scale_value.z == 0.0f))
        mt->visible = false;
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
    mat4_to_mat4u(&mat, &mt->mat);
}

static int32_t auth_3d_get_auth_3d_object_index_by_object_info(auth_3d* auth, object_info obj_info) {
    auth_3d_object* v2 = auth->object.begin;
    auth_3d_object* v3 = auth->object.end;
    int32_t v4 = 0;
    if (v2 == v3)
        return -1;

    while (obj_info.id != v2->object_info.id
        || obj_info.set_id != v2->object_info.set_id) {
        v2++;
        v4++;
        if (v2 == v3)
            return -1;
    }
    return v4;
}

static mat4* auth_3d_get_auth_3d_object_hrc_bone_mats(auth_3d* auth, size_t index) {
    vector_auth_3d_object_hrc* voh = &auth->object_hrc;
    if ((ssize_t)index >= voh->end - voh->begin
        || voh->begin[index].mats.begin == voh->begin[index].mats.end)
        return 0;
    return voh->begin[index].mats.begin;
}

static int32_t auth_3d_get_auth_3d_object_hrc_index_by_object_info(auth_3d* auth, object_info obj_info) {
    auth_3d_object_hrc* v2 = auth->object_hrc.begin;
    auth_3d_object_hrc* v3 = auth->object_hrc.end;
    int32_t v4 = 0;
    if (v2 == v3)
        return -1;

    while (obj_info.id != v2->object_info.id
        || obj_info.set_id != v2->object_info.set_id) {
        v2++;
        v4++;
        if (v2 == v3)
            return -1;
    }
    return v4;
}

static void auth_3d_ambient_data_set(auth_3d_ambient* a, render_context* rctx) {

}

static void auth_3d_ambient_get_value(auth_3d_ambient* a, float_t frame) {
    if (a->flags & AUTH_3D_AMBIENT_LIGHT_DIFFUSE)
        auth_3d_rgba_get_value(&a->light_diffuse, frame);

    if (a->flags & AUTH_3D_AMBIENT_LIGHT_DIFFUSE)
        auth_3d_rgba_get_value(&a->rim_light_diffuse, frame);
}

static void auth_3d_ambient_load(auth_3d* auth, auth_3d_ambient_file* af) {
    auth_3d_ambient* a = vector_auth_3d_ambient_reserve_back(&auth->ambient);

    if (af->flags & A3DA_AMBIENT_LIGHT_DIFFUSE) {
        auth_3d_rgba_load(auth, &a->light_diffuse, &af->light_diffuse);
        a->flags |= AUTH_3D_AMBIENT_LIGHT_DIFFUSE;
    }

    string_copy(&af->name, &a->name);

    if (af->flags & A3DA_AMBIENT_LIGHT_DIFFUSE) {
        auth_3d_rgba_load(auth, &a->rim_light_diffuse, &af->rim_light_diffuse);
        a->flags |= AUTH_3D_AMBIENT_LIGHT_DIFFUSE;
    }
}

static void auth_3d_camera_auxiliary_data_set(auth_3d_camera_auxiliary* ca, render_context* rctx) {
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

void auth_3d_camera_auxiliary_get_value(auth_3d_camera_auxiliary* ca, float_t frame) {
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_AUTO_EXPOSURE)
        ca->auto_exposure_value = auth_3d_key_get_value(&ca->auto_exposure, frame);
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_EXPOSURE)
        ca->exposure_value = auth_3d_key_get_value(&ca->exposure, frame);
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_EXPOSURE_RATE)
        ca->exposure_rate_value = auth_3d_key_get_value(&ca->exposure_rate, frame);
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_GAMMA)
        ca->gamma_value = auth_3d_key_get_value(&ca->gamma, frame);
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_GAMMA_RATE)
        ca->gamma_rate_value = auth_3d_key_get_value(&ca->gamma_rate, frame);
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_SATURATE)
        ca->saturate_value = auth_3d_key_get_value(&ca->saturate, frame);
}

static void auth_3d_camera_auxiliary_load(auth_3d* auth, auth_3d_camera_auxiliary_file* caf) {
    memset(&auth->camera_auxiliary, 0, sizeof(auth_3d_camera_auxiliary));
    auth_3d_camera_auxiliary* ca = &auth->camera_auxiliary;

    if (caf->flags & A3DA_CAMERA_AUXILIARY_AUTO_EXPOSURE) {
        auth_3d_key_load(auth, &ca->auto_exposure, &caf->auto_exposure);
        ca->flags |= AUTH_3D_CAMERA_AUXILIARY_AUTO_EXPOSURE;
    }

    if (caf->flags & A3DA_CAMERA_AUXILIARY_EXPOSURE) {
        auth_3d_key_load(auth, &ca->exposure, &caf->exposure);
        ca->flags |= AUTH_3D_CAMERA_AUXILIARY_EXPOSURE;
    }

    if (caf->flags & A3DA_CAMERA_AUXILIARY_EXPOSURE_RATE) {
        auth_3d_key_load(auth, &ca->exposure_rate, &caf->exposure_rate);
        ca->flags |= AUTH_3D_CAMERA_AUXILIARY_EXPOSURE_RATE;
    }

    if (caf->flags & A3DA_CAMERA_AUXILIARY_GAMMA) {
        auth_3d_key_load(auth, &ca->gamma, &caf->gamma);
        ca->flags |= AUTH_3D_CAMERA_AUXILIARY_GAMMA;
    }

    if (caf->flags & A3DA_CAMERA_AUXILIARY_GAMMA_RATE) {
        auth_3d_key_load(auth, &ca->gamma_rate, &caf->gamma_rate);
        ca->flags |= AUTH_3D_CAMERA_AUXILIARY_GAMMA_RATE;
    }

    if (caf->flags & A3DA_CAMERA_AUXILIARY_SATURATE) {
        auth_3d_key_load(auth, &ca->saturate, &caf->saturate);
        ca->flags |= AUTH_3D_CAMERA_AUXILIARY_SATURATE;
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

static void auth_3d_camera_root_data_set(auth_3d_camera_root* cr,
    bool left_right_reverse, mat4* mat, render_context* rctx) {
    vec3 interest = cr->interest_value;
    vec3 view_point = cr->view_point_value;
    float_t fov = cr->fov_value;
    float_t roll = cr->roll_value;

    mat4_mult_vec3_trans(mat, &interest, &interest);
    mat4_mult_vec3_trans(mat, &view_point, &view_point);

    if (left_right_reverse) {
        interest.x = -interest.x;
        view_point.x = -view_point.x;
        roll = -roll;
    }

    camera* cam = rctx->camera;
    camera_set_interest(cam, &interest);
    camera_set_view_point(cam, &view_point);
    camera_set_fov(cam, fov * RAD_TO_DEG);
    camera_set_roll(cam, roll * RAD_TO_DEG);
}

static void auth_3d_camera_root_get_value(auth_3d_camera_root* cr, float_t frame, mat4* mat) {
    auth_3d_model_transform_get_value(&cr->model_transform, frame);
    auth_3d_model_transform_set_mat(&cr->model_transform, mat);

    mat4 cr_mat;
    mat4u_to_mat4(&cr->model_transform.mat, &cr_mat);

    vec3 view_point;
    auth_3d_vec3_get_value(&cr->view_point.model_transform.translation, frame, &view_point);
    mat4_mult_vec3_trans(&cr_mat, &view_point, &cr->view_point_value);

    vec3 interest;
    auth_3d_vec3_get_value(&cr->interest.translation, frame, &interest);
    mat4_mult_vec3_trans(&cr_mat, &interest, &cr->interest_value);

    float_t fov;
    if (cr->view_point.flags & AUTH_3D_CAMERA_ROOT_VIEW_POINT_FOV) {
        fov = auth_3d_key_get_value(&cr->view_point.fov, frame);
        if (cr->view_point.fov_is_horizontal)
            fov = atanf(tanf(fov * 0.5f) / cr->view_point.aspect) * 2.0f;
    }
    else {
        float_t camera_aperture_h = cr->view_point.camera_aperture_h;
        float_t focal_lenth = auth_3d_key_get_value(&cr->view_point.focal_length, frame);
        fov = atanf((camera_aperture_h * 25.4f) * 0.5f / focal_lenth) * 2.0f;
    }
    cr->fov_value = fov;

    if (cr->view_point.flags & AUTH_3D_CAMERA_ROOT_VIEW_POINT_ROLL)
        cr->roll_value = auth_3d_key_get_value(&cr->view_point.roll, frame);
}

static void auth_3d_camera_root_load(auth_3d* auth, auth_3d_camera_root_file* crf) {
    auth_3d_camera_root* cr = vector_auth_3d_camera_root_reserve_back(&auth->camera_root);

    auth_3d_model_transform_load(auth, &cr->interest, &crf->interest);
    auth_3d_model_transform_load(auth, &cr->model_transform, &crf->model_transform);
    auth_3d_camera_root_view_point_load(auth, cr, &crf->view_point);
}

static void auth_3d_camera_root_view_point_load(auth_3d* auth, auth_3d_camera_root* cr,
    auth_3d_camera_root_view_point_file* crvpf) {
    auth_3d_camera_root_view_point* crvp = &cr->view_point;

    crvp->aspect = crvpf->aspect;

    if (crvpf->flags & A3DA_CAMERA_ROOT_VIEW_POINT_FOV) {
        auth_3d_key_load(auth, &crvp->fov, &crvpf->fov);
        crvp->fov_is_horizontal = crvpf->fov_is_horizontal;
        crvp->flags |= AUTH_3D_CAMERA_ROOT_VIEW_POINT_FOV;
    }
    else {
        crvp->camera_aperture_h = crvpf->camera_aperture_h;
        crvp->camera_aperture_w = crvpf->camera_aperture_w;
        auth_3d_key_load(auth, &crvp->focal_length, &crvpf->focal_length);
    }

    auth_3d_model_transform_load(auth, &crvp->model_transform, &crvpf->model_transform);

    if (crvpf->flags & A3DA_CAMERA_ROOT_VIEW_POINT_ROLL) {
        auth_3d_key_load(auth, &crvp->roll, &crvpf->roll);
        crvp->flags |= AUTH_3D_CAMERA_ROOT_VIEW_POINT_ROLL;
    }
}

static void auth_3d_chara_data_set(auth_3d_chara* c, mat4* parent_mat, render_context* rctx) {

}

static void auth_3d_chara_get_value(auth_3d_chara* c, float_t frame) {
    auth_3d_model_transform_get_value(&c->model_transform, frame);
}

static void auth_3d_chara_load(auth_3d* auth, auth_3d_chara_file* cf) {
    auth_3d_chara* c = vector_auth_3d_chara_reserve_back(&auth->chara);

    auth_3d_model_transform_load(auth, &c->model_transform, &cf->model_transform);
    string_copy(&cf->name, &c->name);
}

static void auth_3d_curve_get_value(auth_3d_curve* c, float_t frame) {
    c->value = auth_3d_key_get_value(&c->curve, frame);
}

static void auth_3d_curve_load(auth_3d* auth, auth_3d_curve_file* cf) {
    auth_3d_curve* c = vector_auth_3d_curve_reserve_back(&auth->curve);

    auth_3d_key_load(auth, &c->curve, &cf->curve);
    string_copy(&cf->name, &c->name);
}

static void auth_3d_dof_data_set(auth_3d_dof* d, render_context* rctx) {
    if (!d->has_dof)
        return;

    vec3 view_point;
    camera_get_view_point(rctx->camera, &view_point);

    vec3 diff;
    float_t focus;
    vec3_sub(d->model_transform.translation_value, view_point, diff);
    vec3_length(diff, focus);

    dof_pv pv;
    pv.enable = fabsf(d->model_transform.rotation_value.z) > 0.000001f;
    pv.f2.distance_to_focus = focus;
    pv.f2.focus_range = d->model_transform.scale_value.y;
    pv.f2.fuzzing_range = d->model_transform.rotation_value.y;
    pv.f2.ratio = d->model_transform.rotation_value.z;
    post_process_dof_set_dof_pv(rctx->post_process.dof, &pv);
}

static void auth_3d_dof_get_value(auth_3d_dof* d, float_t frame) {
    if (!d->has_dof)
        return;

    auth_3d_model_transform_get_value(&d->model_transform, frame);
}

static void auth_3d_dof_load(auth_3d* auth, auth_3d_dof_file* df) {
    memset(&auth->dof, 0, sizeof(auth_3d_dof));
    auth_3d_dof* d = &auth->dof;

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

static void auth_3d_event_load(auth_3d* auth, auth_3d_event_file* ef) {
    auth_3d_event* e = vector_auth_3d_event_reserve_back(&auth->event);

    e->begin = ef->begin;
    e->clip_begin = ef->clip_begin;
    e->clip_end = ef->clip_end;
    e->end = ef->end;
    string_copy(&ef->name, &e->name);
    string_copy(&ef->param_1, &e->param_1);
    string_copy(&ef->ref, &e->ref);
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

static void auth_3d_fog_data_set(auth_3d_fog* f, render_context* rctx) {
    fog_id id = f->id;
    if (id < FOG_DEPTH || id > FOG_BUMP)
        return;

    fog* data = &rctx->fog_data[id];
    if (f->flags & AUTH_3D_FOG_COLOR) {
        if (~f->flags_init & AUTH_3D_FOG_COLOR) {
            vec4 color_init;
            fog_get_color(data, &color_init);
            vec4_to_vec4u(color_init, f->color_init);
            f->flags_init |= AUTH_3D_FOG_COLOR;
        }

        vec4 color;
        vec4u_to_vec4(f->color.value, color);
        fog_set_color(data, &color);
    }

    if (f->flags & AUTH_3D_FOG_DENSITY) {
        if (~f->flags_init & AUTH_3D_FOG_DENSITY) {
            f->density_init = fog_get_density(data);
            f->flags_init |= AUTH_3D_FOG_DENSITY;
        }

        fog_set_density(data, f->density_value);
    }

    if (f->flags & AUTH_3D_FOG_END) {
        if (~f->flags_init & AUTH_3D_FOG_END) {
            f->end_init = fog_get_end(data);
            f->flags_init |= AUTH_3D_FOG_END;
        }

        fog_set_end(data, f->end_value);
    }

    if (f->flags & AUTH_3D_FOG_START) {
        if (~f->flags_init & AUTH_3D_FOG_START) {
            f->start_init = fog_get_start(data);
            f->flags_init |= AUTH_3D_FOG_START;
        }

        fog_set_start(data, f->start_value);
    }
}

static void auth_3d_fog_get_value(auth_3d_fog* f, float_t frame) {
    if (f->flags & AUTH_3D_FOG_DENSITY)
        f->density_value = auth_3d_key_get_value(&f->density, frame);
    if (f->flags & AUTH_3D_FOG_START)
        f->start_value = auth_3d_key_get_value(&f->start, frame);
    if (f->flags & AUTH_3D_FOG_END)
        f->end_value = auth_3d_key_get_value(&f->end, frame);
    if (f->flags & AUTH_3D_FOG_COLOR)
        auth_3d_rgba_get_value(&f->color, frame);
}

static void auth_3d_fog_load(auth_3d* auth, auth_3d_fog_file* ff) {
    auth_3d_fog* f = vector_auth_3d_fog_reserve_back(&auth->fog);

    if (ff->flags & A3DA_FOG_COLOR) {
        auth_3d_rgba_load(auth, &f->color, &ff->color);
        f->flags |= AUTH_3D_FOG_COLOR;
    }

    if (ff->flags & A3DA_FOG_DENSITY) {
        auth_3d_key_load(auth, &f->density, &ff->density);
        f->flags |= AUTH_3D_FOG_DENSITY;
    }

    if (ff->flags & A3DA_FOG_END) {
        auth_3d_key_load(auth, &f->end, &ff->end);
        f->flags |= AUTH_3D_FOG_END;
    }

    f->id = ff->id;

    if (ff->flags & A3DA_FOG_START) {
        auth_3d_key_load(auth, &f->start, &ff->start);
        f->flags |= AUTH_3D_FOG_START;
    }
}

static void auth_3d_fog_restore_prev_value(auth_3d_fog* f, render_context* rctx) {
    fog_id id = f->id;
    if (id < FOG_DEPTH || id > FOG_BUMP)
        return;

    fog* data = &rctx->fog_data[id];
    if (f->flags_init & AUTH_3D_FOG_COLOR) {
        vec4 color;
        vec4u_to_vec4(f->color_init, color);
        fog_set_color(data, &color);
    }

    if (f->flags_init & AUTH_3D_FOG_DENSITY)
        fog_set_density(data, f->density_value);

    if (f->flags_init & AUTH_3D_FOG_END)
        fog_set_end(data, f->end_value);

    if (f->flags_init & AUTH_3D_FOG_START)
        fog_set_start(data, f->start_value);
}

static void auth_3d_light_data_set(auth_3d_light* l, render_context* rctx) {
    light_set* set_data = &rctx->light_set_data[LIGHT_SET_MAIN];

    light_id id = l->id;
    if (id < LIGHT_CHARA || id > LIGHT_PROJECTION)
        return;

    light_data* data = &set_data->lights[id];
    if (l->flags & AUTH_3D_LIGHT_AMBIENT) {
        if (~l->flags_init & AUTH_3D_LIGHT_AMBIENT) {
            vec4 ambient_init;
            light_get_ambient(data, &ambient_init);
            vec4_to_vec4u(ambient_init, l->ambient_init);
            l->flags_init |= AUTH_3D_LIGHT_AMBIENT;
        }

        vec4 ambient;
        vec4u_to_vec4(l->ambient.value, ambient);
        light_set_ambient(data, &ambient);
    }

    if (l->flags & AUTH_3D_LIGHT_DIFFUSE) {
        if (~l->flags_init & AUTH_3D_LIGHT_DIFFUSE) {
            vec4 diffuse_init;
            light_get_diffuse(data, &diffuse_init);
            vec4_to_vec4u(diffuse_init, l->diffuse_init);
            l->flags_init |= AUTH_3D_LIGHT_DIFFUSE;
        }

        vec4 diffuse;
        vec4u_to_vec4(l->diffuse.value, diffuse);
        light_set_diffuse(data, &diffuse);
    }

    if (l->flags & AUTH_3D_LIGHT_POSITION)
        light_set_position(data, &l->position.translation_value);

    if (l->flags & AUTH_3D_LIGHT_SPECULAR) {
        if (~l->flags_init & AUTH_3D_LIGHT_SPECULAR) {
            vec4 specular_init;
            light_get_specular(data, &specular_init);
            vec4_to_vec4u(specular_init, l->specular_init);
            l->flags_init |= AUTH_3D_LIGHT_SPECULAR;
        }

        vec4 specular;
        vec4u_to_vec4(l->specular.value, specular);
        light_set_specular(data, &specular);
    }

    if (l->flags & AUTH_3D_LIGHT_SPOT_DIRECTION)
        light_set_spot_direction(data, &l->spot_direction.translation_value);

    if (l->flags & AUTH_3D_LIGHT_TONE_CURVE) {
        if (~l->flags_init & AUTH_3D_LIGHT_TONE_CURVE) {
            light_get_tone_curve(data, &l->tone_curve_init);
            l->flags_init |= AUTH_3D_LIGHT_TONE_CURVE;
        }

        light_set_tone_curve(data, (vec3*)&l->tone_curve.value);
    }
}

static void auth_3d_light_get_value(auth_3d_light* l, float_t frame) {
    if (l->flags & AUTH_3D_LIGHT_AMBIENT)
        auth_3d_rgba_get_value(&l->ambient, frame);
    if (l->flags & AUTH_3D_LIGHT_CONE_ANGLE)
        l->cone_angle_value = auth_3d_key_get_value(&l->cone_angle, frame);
    if (l->flags & AUTH_3D_LIGHT_CONSTANT)
        l->constant_value = auth_3d_key_get_value(&l->constant, frame);
    if (l->flags & AUTH_3D_LIGHT_DIFFUSE)
        auth_3d_rgba_get_value(&l->diffuse, frame);
    if (l->flags & AUTH_3D_LIGHT_DROP_OFF)
        l->drop_off_value = auth_3d_key_get_value(&l->drop_off, frame);
    if (l->flags & AUTH_3D_LIGHT_FAR)
        l->far_value = auth_3d_key_get_value(&l->_far, frame);
    if (l->flags & AUTH_3D_LIGHT_INTENSITY)
        l->intensity_value = auth_3d_key_get_value(&l->intensity, frame);
    if (l->flags & AUTH_3D_LIGHT_LINEAR)
        l->linear_value = auth_3d_key_get_value(&l->linear, frame);
    if (l->flags & AUTH_3D_LIGHT_POSITION)
        auth_3d_model_transform_get_value(&l->position, frame);
    if (l->flags & AUTH_3D_LIGHT_QUADRATIC)
        l->quadratic_value = auth_3d_key_get_value(&l->quadratic, frame);
    if (l->flags & AUTH_3D_LIGHT_SPECULAR)
        auth_3d_rgba_get_value(&l->specular, frame);
    if (l->flags & AUTH_3D_LIGHT_SPOT_DIRECTION)
        auth_3d_model_transform_get_value(&l->spot_direction, frame);
    if (l->flags & AUTH_3D_LIGHT_TONE_CURVE)
        auth_3d_rgba_get_value(&l->tone_curve, frame);

}

static void auth_3d_light_load(auth_3d* auth, auth_3d_light_file* lf) {
    auth_3d_light* l = vector_auth_3d_light_reserve_back(&auth->light);

    if (lf->flags & A3DA_LIGHT_AMBIENT) {
        auth_3d_rgba_load(auth, &l->ambient, &lf->ambient);
        l->flags |= AUTH_3D_LIGHT_AMBIENT;
    }

    if (lf->flags & A3DA_LIGHT_CONE_ANGLE) {
        auth_3d_key_load(auth, &l->cone_angle, &lf->cone_angle);
        l->flags |= AUTH_3D_LIGHT_CONE_ANGLE;
    }

    if (lf->flags & A3DA_LIGHT_CONSTANT) {
        auth_3d_key_load(auth, &l->constant, &lf->constant);
        l->flags |= AUTH_3D_LIGHT_CONSTANT;
    }

    if (lf->flags & A3DA_LIGHT_DIFFUSE) {
        auth_3d_rgba_load(auth, &l->diffuse, &lf->diffuse);
        l->flags |= AUTH_3D_LIGHT_DIFFUSE;
    }

    if (lf->flags & A3DA_LIGHT_DROP_OFF) {
        auth_3d_key_load(auth, &l->drop_off, &lf->drop_off);
        l->flags |= AUTH_3D_LIGHT_DROP_OFF;
    }

    if (lf->flags & A3DA_LIGHT_FAR) {
        auth_3d_key_load(auth, &l->_far, &lf->_far);
        l->flags |= AUTH_3D_LIGHT_FAR;
    }

    l->id = lf->id;

    if (lf->flags & A3DA_LIGHT_INTENSITY) {
        auth_3d_key_load(auth, &l->intensity, &lf->intensity);
        l->flags |= AUTH_3D_LIGHT_INTENSITY;
    }

    if (lf->flags & A3DA_LIGHT_LINEAR) {
        auth_3d_key_load(auth, &l->linear, &lf->linear);
        l->flags |= AUTH_3D_LIGHT_LINEAR;
    }

    if (lf->flags & A3DA_LIGHT_POSITION) {
        auth_3d_model_transform_load(auth, &l->position, &lf->position);
        l->flags |= AUTH_3D_LIGHT_POSITION;
    }

    if (lf->flags & A3DA_LIGHT_QUADRATIC) {
        auth_3d_key_load(auth, &l->quadratic, &lf->quadratic);
        l->flags |= AUTH_3D_LIGHT_QUADRATIC;
    }

    if (lf->flags & A3DA_LIGHT_SPECULAR) {
        auth_3d_rgba_load(auth, &l->specular, &lf->specular);
        l->flags |= AUTH_3D_LIGHT_SPECULAR;
    }

    if (lf->flags & A3DA_LIGHT_SPOT_DIRECTION) {
        auth_3d_model_transform_load(auth, &l->spot_direction, &lf->spot_direction);
        l->flags |= AUTH_3D_LIGHT_SPOT_DIRECTION;
    }

    if (lf->flags & A3DA_LIGHT_TONE_CURVE) {
        auth_3d_rgba_load(auth, &l->tone_curve, &lf->tone_curve);
        l->flags |= AUTH_3D_LIGHT_TONE_CURVE;
    }

    string_copy(&lf->type, &l->type);
}

static void auth_3d_light_restore_prev_value(auth_3d_light* l, render_context* rctx) {
    light_set* set_data = &rctx->light_set_data[LIGHT_SET_MAIN];

    light_id id = l->id;
    if (id < LIGHT_CHARA || id > LIGHT_PROJECTION)
        return;

    light_data* data = &set_data->lights[id];
    if (l->flags_init & AUTH_3D_LIGHT_AMBIENT) {
        vec4 ambient_init;
        vec4u_to_vec4(l->ambient_init, ambient_init);
        light_set_ambient(data, &ambient_init);
    }

    if (l->flags_init & AUTH_3D_LIGHT_DIFFUSE) {
        vec4 diffuse_init;
        vec4u_to_vec4(l->diffuse_init, diffuse_init);
        light_set_diffuse(data, &diffuse_init);
    }

    if (l->flags_init & AUTH_3D_LIGHT_SPECULAR) {
        vec4 specular_init;
        vec4u_to_vec4(l->specular_init, specular_init);
        light_set_specular(data, &specular_init);
    }

    if (l->flags_init & AUTH_3D_LIGHT_TONE_CURVE)
        light_set_tone_curve(data, &l->tone_curve_init);
}

static void auth_3d_m_object_hrc_get_mat(auth_3d_m_object_hrc* moh, mat4* mat) {
    for (auth_3d_object_node* i = moh->node.begin; i != moh->node.end; i++)
        if (i->mat) {
            *mat = *i->mat;
            return;
        }

    mat4u_to_mat4(&moh->node.begin[0].model_transform.mat, mat);
}

static void auth_3d_m_object_hrc_get_value(auth_3d_m_object_hrc* moh, float_t frame) {
    auth_3d_object_model_transform_get_value(&moh->model_transform, frame);
    auth_3d_object_model_transform_set_mat_inner(&moh->model_transform);

    for (auth_3d_object_instance* i = moh->instance.begin; i != moh->instance.end; i++) {
        auth_3d_object_model_transform_get_value(&i->model_transform, frame);
        auth_3d_object_model_transform_set_mat_inner(&i->model_transform);
    }

    for (auth_3d_object_node* i = moh->node.begin; i != moh->node.end; i++) {
        auth_3d_object_model_transform_get_value(&i->model_transform, frame);
        auth_3d_object_model_transform_set_mat_inner(&i->model_transform);
    }
}

static void auth_3d_m_object_hrc_load(auth_3d* auth,
    auth_3d_m_object_hrc_file* mohf, object_database* obj_db) {
    auth_3d_m_object_hrc* moh
        = vector_auth_3d_m_object_hrc_reserve_back(&auth->m_object_hrc);

    vector_auth_3d_object_node_reserve(&moh->node, mohf->node.end - mohf->node.begin);
    for (auth_3d_object_node_file* i = mohf->node.begin; i != mohf->node.end; i++)
        auth_3d_object_node_load(auth, &moh->node, i);

    auth_3d_object_model_transform_load(auth, &moh->model_transform, &mohf->model_transform);
    string_copy(&mohf->name, &moh->name);

    for (auth_3d_object_instance_file* i = mohf->instance.begin; i != mohf->instance.end; i++)
        auth_3d_object_instance_load(auth, moh, i, obj_db);
}

static void auth_3d_m_object_hrc_data_set(auth_3d_m_object_hrc* moh, auth_3d* auth, render_context* rctx) {
    if (!moh->model_transform.visible)
        return;

    object_data* object_data = &rctx->object_data;
    object_database* obj_db = &rctx->data->data_ft.obj_db;
    texture_database* tex_db = &rctx->data->data_ft.tex_db;

    for (auth_3d_object_instance* i = moh->instance.begin; i != moh->instance.end; i++) {
        if (!i->model_transform.visible)
            return;

        draw_task_flags flags = DRAW_TASK_SSS;
        shadow_type_enum shadow_type = SHADOW_CHARA;
        if (i->shadow) {
            flags |= DRAW_TASK_4 | DRAW_TASK_SHADOW;
            shadow_type = SHADOW_STAGE;
        }

        object_data_set_draw_task_flags(object_data, flags);
        object_data_set_shadow_type(object_data, shadow_type);

        shadow* shad = rctx->draw_pass.shadow_ptr;
        if (shad && flags & DRAW_TASK_SHADOW) {
            object_data_set_shadow_type(object_data, SHADOW_STAGE);

            mat4u* m = &moh->model_transform.mat;
            for (auth_3d_object_node* i = moh->node.begin; i != moh->node.end; i++)
                if (i->mat) {
                    m = (mat4u*)i->mat;
                    break;
                }

            mat4 mat;
            mat4 t;
            mat4u_to_mat4(&i->model_transform.mat, &mat);
            mat4u_to_mat4(m, &t);
            mat4_mult(&mat, &t, &mat);

            vec3 pos = *(vec3*)&mat.row3;
            pos.y -= 0.2f;
            vector_vec3_push_back(&shad->field_1D0[shadow_type], &pos);
        }

        if (i->mats.end - i->mats.begin > 0) {
            mat4 mat;
            mat4u_to_mat4(&i->model_transform.mat, &mat);
            draw_task_add_draw_object_by_object_info_object_skin(rctx,
                i->object_info, 0, 0, auth->alpha, i->mats.begin, 0, 0, &mat);
        }
    }

    object_data_set_draw_task_flags(object_data, 0);
    object_data_set_shadow_type(object_data, SHADOW_CHARA);
}

static void auth_3d_m_object_hrc_list_get_value(auth_3d_m_object_hrc* moh, mat4* parent_mat) {
    if (!moh->model_transform.visible)
        return;

    auth_3d_object_node* node = moh->node.begin;
    size_t instance_count = moh->instance.end - moh->instance.begin;

    auth_3d_m_object_hrc_nodes_mat_mult(moh);

    auth_3d_object_model_transform_mat_mult(&moh->model_transform, parent_mat);

    mat4 mat;
    mat4u_to_mat4(&moh->model_transform.mat, &mat);

    for (auth_3d_object_instance* i = moh->instance.begin; i != moh->instance.end; i++) {
        if (!i->model_transform.visible)
            continue;

        auth_3d_object_model_transform_mat_mult(&i->model_transform, &mat);
        int32_t* object_bone_indices = i->object_bone_indices.begin;
        size_t object_bone_indices_count = i->object_bone_indices.end - i->object_bone_indices.begin;
        mat4* mats = i->mats.begin;
        for (size_t j = 0; j < object_bone_indices_count; j++)
            mat4u_to_mat4(&node[object_bone_indices[j]].model_transform.mat, &mats[j]);
    }
}

static void auth_3d_m_object_hrc_nodes_mat_mult(auth_3d_m_object_hrc* moh) {
    auth_3d_object_node* node = moh->node.begin;

    for (auth_3d_object_node* i = moh->node.begin; i != moh->node.end; i++)
        if (i->parent >= 0) {
            mat4 mat;
            mat4u_to_mat4(&node[i->parent].model_transform.mat, &mat);
            auth_3d_object_model_transform_mat_mult(&i->model_transform, &mat);
        }
        else
            i->model_transform.mat = i->model_transform.mat_inner;
}

static void auth_3d_material_list_load(auth_3d* auth, auth_3d_material_list_file* mlf) {
    auth_3d_material_list* ml = vector_auth_3d_material_list_reserve_back(&auth->material_list);

    if (mlf->flags & A3DA_MATERIAL_LIST_BLEND_COLOR) {
        auth_3d_rgba_load(auth, &ml->blend_color, &mlf->blend_color);
        ml->flags |= AUTH_3D_MATERIAL_LIST_BLEND_COLOR;
    }

    if (mlf->flags & A3DA_MATERIAL_LIST_GLOW_INTENSITY) {
        auth_3d_key_load(auth, &ml->glow_intensity, &mlf->glow_intensity);
        ml->flags |= AUTH_3D_MATERIAL_LIST_GLOW_INTENSITY;
    }

    if (mlf->flags & A3DA_MATERIAL_LIST_INCANDESCENCE) {
        auth_3d_rgba_load(auth, &ml->incandescence, &mlf->incandescence);
        ml->flags |= AUTH_3D_MATERIAL_LIST_INCANDESCENCE;
    }

    string_copy(&mlf->name, &ml->name);
}

static void auth_3d_object_get_value(auth_3d_object* o, float_t frame) {
    auth_3d_model_transform_get_value(&o->model_transform, frame);
    auth_3d_object_curve_get_value(&o->pattern, frame);
    auth_3d_object_curve_get_value(&o->morph, frame);
    for (auth_3d_object_texture_pattern* i = o->texture_pattern.begin;
        i != o->texture_pattern.end; i++)
        auth_3d_object_texture_pattern_get_value(i, frame);
    for (auth_3d_object_texture_transform* i = o->texture_transform.begin;
        i != o->texture_transform.end; i++)
        auth_3d_object_texture_transform_get_value(i, frame);
}

static void auth_3d_object_load(auth_3d* auth, auth_3d_object_file* of,
    object_database* obj_db, texture_database* tex_db) {
    auth_3d_object* o = vector_auth_3d_object_reserve_back(&auth->object);

    auth_3d_model_transform_load(auth, &o->model_transform, &of->model_transform);
    auth_3d_object_curve_load(auth, &o->morph, &of->morph, of->morph_offset);
    string_copy(&of->name, &o->name);
    string_copy(&of->parent_name, &o->parent_name);
    string_copy(&of->parent_node, &o->parent_node);
    auth_3d_object_curve_load(auth, &o->pattern, &of->pattern, of->pattern_offset);

    vector_auth_3d_object_texture_pattern_reserve(&o->texture_pattern,
        of->texture_pattern.end - of->texture_pattern.begin);
    for (auth_3d_object_texture_pattern_file* i = of->texture_pattern.begin;
        i != of->texture_pattern.end; i++)
        auth_3d_object_texture_pattern_load(auth, o, i, tex_db);

    vector_auth_3d_object_texture_transform_reserve(&o->texture_transform,
        of->texture_transform.end - of->texture_transform.begin);
    for (auth_3d_object_texture_transform_file* i = of->texture_transform.begin;
        i != of->texture_transform.end; i++)
        auth_3d_object_texture_transform_load(auth, o, i, tex_db);

    string_copy(&of->uid_name, &o->uid_name);

    o->reflect = strstr(string_data(&o->uid_name), "_REFLECT");
    o->refract = strstr(string_data(&o->uid_name), "_REFRACT");

    o->object_info = object_database_get_object_info(obj_db, string_data(&o->uid_name));
}

static void auth_3d_object_curve_get_value(auth_3d_object_curve* oc, float_t frame) {
    auth_3d_curve* c = oc->curve;
    if (!c)
        return;

    if (oc->frame_offset != 0.0f) {
        frame += oc->frame_offset;
        if (frame >= c->curve.max_frame)
            frame -= c->curve.max_frame;
    }
    oc->value = auth_3d_key_get_value(&c->curve, frame);
}

static void auth_3d_object_curve_load(auth_3d* auth, auth_3d_object_curve* oc,
    string* name, float_t frame_offset) {
    oc->curve = 0;
    string_copy(name, &oc->name);
    oc->frame_offset = 0.0f;
    oc->value = 0.0f;

    for (auth_3d_curve* i = auth->curve.begin; i != auth->curve.end; i++)
        if (string_compare(name, &i->name)) {
            oc->curve = i;
            oc->frame_offset = frame_offset;
            return;
        }
}

static void auth_3d_object_data_set(auth_3d_object* o, auth_3d* auth, render_context* rctx) {
    if (!o->model_transform.visible)
        return;

    mat4 mat;
    mat4u_to_mat4(&o->model_transform.mat, &mat);

    object_data* object_data = &rctx->object_data;
    object_database* obj_db = &rctx->data->data_ft.obj_db;
    texture_database* tex_db = &rctx->data->data_ft.tex_db;

    draw_task_flags flags = 0;
    if (o->reflect)
        flags |= DRAW_TASK_NO_TRANSLUCENCY | DRAW_TASK_REFLECT;
    if (o->refract)
        flags |= DRAW_TASK_NO_TRANSLUCENCY | DRAW_TASK_REFRACT;

    object_data_set_draw_task_flags(object_data, flags);

    char buf[0x80];
    int32_t tex_pat_count = 0;
    texture_pattern_struct tex_pat[TEXTURE_PATTERN_COUNT];

    for (auth_3d_object_texture_pattern* i = o->texture_pattern.begin;
        i != o->texture_pattern.end; i++) {
        if (!i->pattern.curve || i->name.length <= 3)
            continue;
        else if (tex_pat_count >= TEXTURE_PATTERN_COUNT)
            break;

        float_t pat = i->pattern.value;
        if (pat > 0.0f)
            pat = (float_t)(int32_t)(pat + 0.5f);
        else if (pat < 0.0f)
            pat = (float_t)(int32_t)(pat - 0.5f);

        sprintf_s(buf, sizeof(buf), "%.*s%03d", (int32_t)(i->name.length - 3),
            string_data(&i->name), (int32_t)pat);

        tex_pat[tex_pat_count].src = i->texture_id;
        tex_pat[tex_pat_count].dst = texture_database_get_texture_id(tex_db, buf);
        tex_pat_count++;
    }

    if (tex_pat_count)
        object_data_set_texture_pattern(object_data, tex_pat_count, tex_pat);

    int32_t tex_trans_count = 0;
    texture_transform_struct tex_trans[TEXTURE_TRANSFORM_COUNT];

    for (auth_3d_object_texture_transform* i = o->texture_transform.begin;
        i != o->texture_transform.end; i++) {
        if (tex_trans_count >= TEXTURE_TRANSFORM_COUNT)
            break;

        tex_trans[tex_trans_count].id = i->texture_id;
        tex_trans[tex_trans_count].mat = i->mat;
        tex_trans_count++;
    }

    if (tex_trans_count)
        object_data_set_texture_transform(object_data, tex_trans_count, tex_trans);

    char* uid_name = string_data(&o->uid_name);
    int32_t uid_name_length = (int32_t)o->uid_name.length;

    if (uid_name_length <= 3)
        draw_task_add_draw_object_by_object_info_opaque(rctx, &mat, o->object_info);
    else if (o->morph.curve) {
        float_t morph = o->morph.value;
        int32_t morph_int = (int32_t)morph;
        morph = fmodf(morph, 1.0f);
        if (morph > 0.0f && morph < 1.0f) {
            sprintf_s(buf, sizeof(buf), "%.*s%03d", uid_name_length - 3, uid_name, morph_int + 1);
            object_info morph_obj_info = object_database_get_object_info(obj_db, buf);
            if (morph_obj_info.set_id == -1 && morph_obj_info.id == -1)
                morph_obj_info = o->object_info;
            object_data_set_morph(object_data, morph_obj_info, morph);

            sprintf_s(buf, sizeof(buf), "%.*s%03d", uid_name_length - 3, uid_name, morph_int);
            object_info obj_info = object_database_get_object_info(obj_db, buf);
            draw_task_add_draw_object_by_object_info_opaque(rctx, &mat, obj_info);
            object_data_set_morph(object_data, object_info_null, 0.0f);
        }
        else {
            if (morph >= 1.0f)
                morph_int++;

            sprintf_s(buf, sizeof(buf), "%.*s%03d", uid_name_length - 3, uid_name, morph_int);
            object_info obj_info = object_database_get_object_info(obj_db, buf);
            if (obj_info.set_id == -1 && obj_info.id == -1)
                obj_info = o->object_info;
            draw_task_add_draw_object_by_object_info_opaque(rctx, &mat, obj_info);
        }
    }
    else if (o->pattern.curve) {
        float_t pat = o->pattern.value;
        if (pat > 0.0f)
            pat = (float_t)(int32_t)(pat + 0.5f);
        else if (pat < 0.0f)
            pat = (float_t)(int32_t)(pat - 0.5f);

        sprintf_s(buf, sizeof(buf), "%.*s%03d", uid_name_length - 3, uid_name, (int32_t)pat);
        object_info obj_info = object_database_get_object_info(obj_db, buf);
        draw_task_add_draw_object_by_object_info_opaque(rctx, &mat, obj_info);
    }
    else
        draw_task_add_draw_object_by_object_info_opaque(rctx, &mat, o->object_info);

    object_data_set_texture_transform(object_data, 0, 0);
    object_data_set_texture_pattern(object_data, 0, 0);
    object_data_set_draw_task_flags(object_data, 0);

    for (ptr_auth_3d_object* i = o->children_object.begin;
        i != o->children_object.end; i++)
        auth_3d_object_data_set(*i, auth, rctx);

    for (ptr_auth_3d_object_hrc* i = o->children_object_hrc.begin;
        i != o->children_object_hrc.end; i++)
        auth_3d_object_hrc_data_set(*i, auth, rctx);
}

static void auth_3d_object_list_get_value(auth_3d_object* o, mat4* parent_mat) {
    if (!o->model_transform.visible)
        return;
    
    auth_3d_model_transform_set_mat(&o->model_transform, parent_mat);

    mat4 mat;
    mat4u_to_mat4(&o->model_transform.mat, &mat);

    for (auth_3d_object** i = o->children_object.begin;
        i != o->children_object.end; i++)
        auth_3d_object_list_get_value(*i, &mat);
    for (auth_3d_object_hrc** j = o->children_object_hrc.begin;
        j != o->children_object_hrc.end; j++)
        auth_3d_object_hrc_list_get_value(*j, &mat);
}

static void auth_3d_object_hrc_get_value(auth_3d_object_hrc* oh, float_t frame) {
    for (auth_3d_object_node* i = oh->node.begin; i != oh->node.end; i++)
        auth_3d_object_model_transform_get_value(&i->model_transform, frame);
}

static void auth_3d_object_hrc_load(auth_3d* auth,
    auth_3d_object_hrc_file* ohf, object_database* obj_db) {
    auth_3d_object_hrc* oh = vector_auth_3d_object_hrc_reserve_back(&auth->object_hrc);

    string_copy(&ohf->name, &oh->name);

    vector_auth_3d_object_node_reserve(&oh->node, ohf->node.end - ohf->node.begin);
    for (auth_3d_object_node_file* i = ohf->node.begin; i != ohf->node.end; i++)
        auth_3d_object_node_load(auth, &oh->node, i);

    string_copy(&ohf->parent_name, &oh->parent_name);
    string_copy(&ohf->parent_node, &oh->parent_node);
    oh->shadow = ohf->shadow;
    string_copy(&ohf->uid_name, &oh->uid_name);

    oh->reflect = strstr(string_data(&oh->uid_name), "_REFLECT");
    oh->refract = strstr(string_data(&oh->uid_name), "_REFRACT");

    oh->object_info = object_database_get_object_info(obj_db, string_data(&oh->uid_name));

    object* obj = object_storage_get_object(oh->object_info);
    if (!obj || !obj->skin_init)
        return;

    object_skin* skin = &obj->skin;

    oh->mats = vector_empty(mat4);
    vector_mat4_reserve(&oh->mats, skin->bones_count);
    oh->mats.end += skin->bones_count;
    mat4* mats = oh->mats.begin;

    for (auth_3d_object_node* i = oh->node.begin; i != oh->node.end; i++) {
        i->bone_id = -1;
        for (int32_t j = 0; j != skin->bones_count; j++)
            if (string_compare(&i->name, &skin->bones[j].name)) {
                i->bone_id = skin->bones[j].id;
                break;
            }

        if (i->bone_id > -1 && !(i->bone_id & 0x8000))
            i->mat = &mats[i->bone_id];
    }
}

static void auth_3d_object_hrc_data_set(auth_3d_object_hrc* oh, auth_3d* auth, render_context* rctx) {
    if (!oh->node.begin[0].model_transform.visible)
        return;

    object_data* object_data = &rctx->object_data;
    object_database* obj_db = &rctx->data->data_ft.obj_db;
    texture_database* tex_db = &rctx->data->data_ft.tex_db;

    draw_task_flags flags = DRAW_TASK_SSS;
    if (oh->shadow)
        flags |= DRAW_TASK_4 | DRAW_TASK_SHADOW;
    if (oh->reflect)
        flags |= DRAW_TASK_NO_TRANSLUCENCY | DRAW_TASK_REFLECT;
    if (oh->refract)
        flags |= DRAW_TASK_NO_TRANSLUCENCY | DRAW_TASK_REFRACT;

    object_data_set_draw_task_flags(object_data, flags);
    object_data_set_shadow_type(object_data, SHADOW_CHARA);
    
    shadow* shad = rctx->draw_pass.shadow_ptr;
    if (shad && flags & DRAW_TASK_SHADOW) {
        object_data_set_shadow_type(object_data, SHADOW_STAGE);

        mat4u* m = &oh->node.begin->model_transform.mat;
        for (auth_3d_object_node* i = oh->node.begin; i != oh->node.end; i++)
            if (i->mat) {
                m = (mat4u*)i->mat;
                break;
            }

        vec3 pos = *(vec3*)&m->row3;
        pos.y -= 0.2f;
        vector_vec3_push_back(&shad->field_1D0[SHADOW_STAGE], &pos);
    }

    if (oh->mats.end - oh->mats.begin > 0) {
        mat4 mat = mat4_identity;
        draw_task_add_draw_object_by_object_info_object_skin(rctx,
            oh->object_info, 0, 0, auth->alpha, oh->mats.begin, 0, 0, &mat);
    }

    object_data_set_draw_task_flags(object_data, 0);
    object_data_set_shadow_type(object_data, SHADOW_CHARA);

    for (ptr_auth_3d_object* i = oh->children_object.begin;
        i != oh->children_object.end; i++)
        auth_3d_object_data_set(*i, auth, rctx);

    for (ptr_auth_3d_object_hrc* i = oh->children_object_hrc.begin;
        i != oh->children_object_hrc.end; i++)
        auth_3d_object_hrc_data_set(*i, auth, rctx);
}

static void auth_3d_object_hrc_list_get_value(auth_3d_object_hrc* oh, mat4* mat) {
    if (!(oh->node.end - oh->node.begin)
        || ! oh->node.begin[0].model_transform.visible)
        return;

    auth_3d_object_hrc_nodes_mat_mult(oh, mat);

    int32_t* childer_object_parent_node = oh->childer_object_parent_node.begin;
    size_t children_object_count = oh->children_object.end - oh->children_object.begin;
    for (size_t i = 0; i < children_object_count; i++) {
        mat4 mat;
        mat4u_to_mat4(&oh->node.begin[childer_object_parent_node[i]].model_transform.mat, &mat);
        auth_3d_object_list_get_value(oh->children_object.begin[i], &mat);
    }

    int32_t* childer_object_hrc_parent_node = oh->childer_object_hrc_parent_node.begin;
    size_t children_object_hrc_count = oh->children_object_hrc.end - oh->children_object_hrc.begin;
    for (size_t i = 0; i < children_object_hrc_count; i++) {
        mat4 mat;
        mat4u_to_mat4(&oh->node.begin[childer_object_hrc_parent_node[i]].model_transform.mat, &mat);
        auth_3d_object_hrc_list_get_value(oh->children_object_hrc.begin[i], &mat);
    }
}

static void auth_3d_object_hrc_nodes_mat_mult(auth_3d_object_hrc* oh, mat4* mat) {
    for (auth_3d_object_node* i = oh->node.begin; i != oh->node.end; i++) {
        auth_3d_object_model_transform_set_mat_inner(&i->model_transform);

        mat4 m;
        if (i->parent >= 0)
            mat4u_to_mat4(&oh->node.begin[i->parent].model_transform.mat, &m);
        else
            m = *mat;

        mat4 joint_orient;
        mat4u_to_mat4(&i->joint_orient_mat, &joint_orient);
        mat4_mult(&joint_orient, &m, &m);
        auth_3d_object_model_transform_mat_mult(&i->model_transform, &m);

        if (i->mat)
            mat4u_to_mat4(&i->model_transform.mat, i->mat);
    }
}

static void auth_3d_object_instance_load(auth_3d* auth, auth_3d_m_object_hrc* moh,
    auth_3d_object_instance_file* oif, object_database* obj_db) {
    auth_3d_object_instance* oi
        = vector_auth_3d_object_instance_reserve_back(&moh->instance);

    auth_3d_object_model_transform_load(auth, &oi->model_transform, &oif->model_transform);
    string_copy(&oif->name, &oi->name);
    oi->shadow = oif->shadow;
    string_copy(&oif->uid_name, &oi->uid_name);

    oi->object_info = object_database_get_object_info(obj_db, string_data(&oi->uid_name));

    object* obj = object_storage_get_object(oi->object_info);
    if (!obj || !obj->skin_init)
        return;

    object_skin* skin = &obj->skin;

    oi->object_bone_indices = vector_empty(int32_t);
    vector_int32_t_reserve(&oi->object_bone_indices, skin->bones_count);
    oi->object_bone_indices.end += skin->bones_count;
    int32_t* object_bone_indices = oi->object_bone_indices.begin;

    oi->mats = vector_empty(mat4);
    vector_mat4_reserve(&oi->mats, skin->bones_count);
    oi->mats.end += skin->bones_count;
    mat4* mats = oi->mats.begin;

    for (int32_t i = 0; i < skin->bones_count; i++)
        for (auth_3d_object_node* j = moh->node.begin; j != moh->node.end; j++)
            if (string_compare(&j->name, &skin->bones[i].name)) {
                int32_t bone_id = skin->bones[i].id;
                if (!(bone_id & 0x8000))
                    object_bone_indices[bone_id] = (int32_t)(j - moh->node.begin);
                break;
            }
}

static void auth_3d_object_model_transform_get_value(auth_3d_object_model_transform* obj_mt, float_t frame) {
    if (frame == obj_mt->frame && !obj_mt->has_rotation)
        return;

    auth_3d_vec3_get_value(&obj_mt->translation, frame, &obj_mt->translation_value);
    auth_3d_vec3_get_value(&obj_mt->rotation, frame, &obj_mt->rotation_value);

    if (obj_mt->has_scale)
        auth_3d_vec3_get_value(&obj_mt->scale, frame, &obj_mt->scale_value);

    if (obj_mt->has_visibility)
        obj_mt->visible = auth_3d_key_get_value(&obj_mt->visibility, frame) >= 0.99900001f;

    mat4 mat_rot;
    vec3 rotation = obj_mt->rotation_value;
    mat4_rotate(rotation.x, rotation.y, rotation.z, &mat_rot);
    mat4_to_mat4u(&mat_rot, &obj_mt->mat_rot);
    obj_mt->frame = frame;
    obj_mt->has_rotation = false;
    obj_mt->has_translation = false;
}

static void auth_3d_object_model_transform_load(auth_3d* auth,
    auth_3d_object_model_transform* omt, auth_3d_model_transform_file* mtf) {
    omt->mat = mat4u_identity;
    omt->mat_inner = mat4u_identity;
    omt->mat_rot = mat4u_identity;
    memset(&omt->translation, 0, sizeof(auth_3d_vec3));
    memset(&omt->rotation, 0, sizeof(auth_3d_vec3));
    memset(&omt->scale, 0, sizeof(auth_3d_vec3));
    memset(&omt->visibility, 0, sizeof(auth_3d_key));
    omt->translation_value = vec3_null;
    omt->rotation_value = vec3_null;
    omt->scale_value = vec3_identity;
    omt->visible = false;
    omt->frame = 0.0f;
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
    mat4 obj_mt_mat;
    mat4u_to_mat4(&obj_mt->mat_inner, &obj_mt_mat);
    mat4_mult(&obj_mt_mat, mat, &obj_mt_mat);
    mat4_to_mat4u(&obj_mt_mat, &obj_mt->mat);
}

static void auth_3d_object_model_transform_set_mat_inner(auth_3d_object_model_transform* obj_mt) {
    if (obj_mt->has_translation && !obj_mt->has_rotation)
        return;

    mat4 mat;
    vec3 translation = obj_mt->translation_value;
    mat4_translate(translation.x, translation.y, translation.z, &mat);

    mat4 mat_rot;
    mat4u_to_mat4(&obj_mt->mat_rot, &mat_rot);
    mat4_mult(&mat_rot, &mat, &mat);
    if (obj_mt->has_scale) {
        vec3 scale = obj_mt->scale_value;
        mat4_scale_rot(&mat, scale.x, scale.y, scale.z, &mat);
    }
    mat4_to_mat4u(&mat, &obj_mt->mat_inner);
    obj_mt->has_translation = true;
}

static void auth_3d_object_node_load(auth_3d* auth,
    vector_auth_3d_object_node* von, auth_3d_object_node_file* onf) {
    auth_3d_object_node* on = vector_auth_3d_object_node_reserve_back(von);

    if (onf->flags & A3DA_OBJECT_NODE_JOINT_ORIENT) {
        vec3 rot = onf->joint_orient;
        mat4 mat;
        mat4_rotate(rot.x, rot.y, rot.z, &mat);
        on->joint_orient = rot;
        mat4_to_mat4u(&mat, &on->joint_orient_mat);
        on->flags |= AUTH_3D_OBJECT_NODE_JOINT_ORIENT;
    }
    else {
        on->joint_orient = vec3_null;
        on->joint_orient_mat = mat4u_identity;
    }

    auth_3d_object_model_transform_load(auth, &on->model_transform, &onf->model_transform);
    string_copy(&onf->name, &on->name);
    on->parent = onf->parent;
}

static void auth_3d_object_texture_pattern_get_value(
    auth_3d_object_texture_pattern* otp, float_t frame) {
    auth_3d_object_curve_get_value(&otp->pattern, frame);
}

static void auth_3d_object_texture_pattern_load(auth_3d* auth, auth_3d_object* o,
    auth_3d_object_texture_pattern_file* otpf, texture_database* tex_db) {
    auth_3d_object_texture_pattern* otp
        = vector_auth_3d_object_texture_pattern_reserve_back(&o->texture_pattern);

    string_copy(&otpf->name, &otp->name);
    auth_3d_object_curve_load(auth, &otp->pattern, &otpf->pattern, otpf->pattern_offset);
    otp->texture_id = texture_database_get_texture_id(tex_db, string_data(&otp->name));
}

static void auth_3d_object_texture_transform_get_value(
    auth_3d_object_texture_transform* ott, float_t frame) {
    auth_3d_object_texture_transform_flags flags = ott->flags;

    vec3 scale = vec3_identity;
    vec3 repeat = vec3_identity;
    vec3 rotate = vec3_null;
    vec3 translate_frame = vec3_null;

    if (flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_COVERAGE_U && ott->coverage_u.max_frame > 0.0f) {
        float_t coverage_u = auth_3d_key_get_value(&ott->coverage_u, frame);
        if (fabsf(coverage_u) > 0.000001f)
            scale.x = 1.0f / coverage_u;
    }

    if (flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_COVERAGE_V && ott->coverage_v.max_frame > 0.0f) {
        float_t coverage_v = auth_3d_key_get_value(&ott->coverage_v, frame);
        if (fabsf(coverage_v) > 0.000001f)
            scale.y = 1.0f / coverage_v;
    }

    if (flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_REPEAT_U && ott->repeat_u.max_frame > 0.0f) {
        float_t value = auth_3d_key_get_value(&ott->repeat_u, frame);
        if (fabsf(value) > 0.000001f)
            repeat.x = value;
    }

    if (flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_REPEAT_V && ott->repeat_v.max_frame > 0.0f) {
        float_t value = auth_3d_key_get_value(&ott->repeat_v, frame);
        if (fabsf(value) > 0.000001f)
            repeat.y = value;
    }

    if (flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_ROTATE_FRAME)
        rotate.z = auth_3d_key_get_value(&ott->rotate_frame, frame);

    if (flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_ROTATE)
        rotate.z += auth_3d_key_get_value(&ott->rotate, frame);

    float_t translate_frame_u;
    if (flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_TRANSLATE_FRAME_U)
        translate_frame_u = -auth_3d_key_get_value(&ott->translate_frame_u, frame);
    else
        translate_frame_u = 0.0f;

    float_t offset_u;
    if (flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_OFFSET_U)
        offset_u = auth_3d_key_get_value(&ott->offset_u, frame);
    else
        offset_u = 0.0f;

    translate_frame.x = (translate_frame_u + offset_u) * repeat.x;

    float_t translate_frame_v;
    if (flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_TRANSLATE_FRAME_V)
        translate_frame_v = -auth_3d_key_get_value(&ott->translate_frame_v, frame);
    else
        translate_frame_v = 0.0f;

    float_t offset_v;
    if (flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_OFFSET_V)
        offset_v = auth_3d_key_get_value(&ott->offset_v, frame);
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
    mat4_to_mat4u(&mat, &ott->mat);
}

static void auth_3d_object_texture_transform_load(auth_3d* auth, auth_3d_object* o,
    auth_3d_object_texture_transform_file* ottf, texture_database* tex_db) {
    auth_3d_object_texture_transform* ott
        = vector_auth_3d_object_texture_transform_reserve_back(&o->texture_transform);

    if (ottf->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_COVERAGE_U) {
        auth_3d_key_load(auth, &ott->coverage_u, &ottf->coverage_u);
        ott->flags |= AUTH_3D_OBJECT_TEXTURE_TRANSFORM_COVERAGE_U;
    }

    if (ottf->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_COVERAGE_V) {
        auth_3d_key_load(auth, &ott->coverage_v, &ottf->coverage_v);
        ott->flags |= AUTH_3D_OBJECT_TEXTURE_TRANSFORM_COVERAGE_V;
    }

    string_copy(&ottf->name, &ott->name);

    if (ottf->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_OFFSET_U) {
        auth_3d_key_load(auth, &ott->offset_u, &ottf->offset_u);
        ott->flags |= AUTH_3D_OBJECT_TEXTURE_TRANSFORM_OFFSET_U;
    }

    if (ottf->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_OFFSET_V) {
        auth_3d_key_load(auth, &ott->offset_v, &ottf->offset_v);
        ott->flags |= AUTH_3D_OBJECT_TEXTURE_TRANSFORM_OFFSET_V;
    }

    if (ottf->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_REPEAT_U) {
        auth_3d_key_load(auth, &ott->repeat_u, &ottf->repeat_u);
        ott->flags |= AUTH_3D_OBJECT_TEXTURE_TRANSFORM_REPEAT_U;
    }

    if (ottf->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_REPEAT_V) {
        auth_3d_key_load(auth, &ott->repeat_v, &ottf->repeat_v);
        ott->flags |= AUTH_3D_OBJECT_TEXTURE_TRANSFORM_REPEAT_V;
    }

    if (ottf->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_ROTATE) {
        auth_3d_key_load(auth, &ott->rotate, &ottf->rotate);
        ott->flags |= AUTH_3D_OBJECT_TEXTURE_TRANSFORM_ROTATE;
    }

    if (ottf->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_ROTATE_FRAME) {
        auth_3d_key_load(auth, &ott->rotate_frame, &ottf->rotate_frame);
        ott->flags |= AUTH_3D_OBJECT_TEXTURE_TRANSFORM_ROTATE_FRAME;
    }

    if (ottf->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_TRANSLATE_FRAME_U) {
        auth_3d_key_load(auth, &ott->translate_frame_u, &ottf->translate_frame_u);
        ott->flags |= AUTH_3D_OBJECT_TEXTURE_TRANSFORM_TRANSLATE_FRAME_U;
    }

    if (ottf->flags & A3DA_OBJECT_TEXTURE_TRANSFORM_TRANSLATE_FRAME_V) {
        auth_3d_key_load(auth, &ott->translate_frame_v, &ottf->translate_frame_v);
        ott->flags |= AUTH_3D_OBJECT_TEXTURE_TRANSFORM_TRANSLATE_FRAME_V;
    }

    ott->texture_id = texture_database_get_texture_id(tex_db, string_data(&ott->name));
}

static void auth_3d_play_control_load(auth_3d* auth, auth_3d_play_control_file* pcf) {
    memset(&auth->play_control, 0, sizeof(auth_3d_play_control));
    auth_3d_play_control* pc = &auth->play_control;

    pc->begin = pcf->begin;

    if (pcf->flags & A3DA_PLAY_CONTROL_DIV) {
        pc->div = pcf->div;
        pc->flags |= AUTH_3D_PLAY_CONTROL_DIV;
    }

    pc->fps = pcf->fps;

    if (pcf->flags & A3DA_PLAY_CONTROL_OFFSET) {
        pc->div = pcf->div;
        pc->flags |= AUTH_3D_PLAY_CONTROL_OFFSET;
    }

    pc->size = pcf->size;
}

static void auth_3d_point_data_set(auth_3d_point* p, mat4* parent_mat, render_context* rctx) {

}

static void auth_3d_point_get_value(auth_3d_point* p, float_t frame) {
    auth_3d_model_transform_get_value(&p->model_transform, frame);
}

static void auth_3d_point_load(auth_3d* auth, auth_3d_point_file* pf) {
    auth_3d_point* p = vector_auth_3d_point_reserve_back(&auth->point);

    auth_3d_model_transform_load(auth, &p->model_transform, &pf->model_transform);
    string_copy(&pf->name, &p->name);
}

static void auth_3d_post_process_data_set(auth_3d_post_process* pp, render_context* rctx) {
    post_process_blur* blur = rctx->post_process.blur;
    post_process_tone_map* tm = rctx->post_process.tone_map;

    if (pp->flags & AUTH_3D_POST_PROCESS_INTENSITY) {
        if (~pp->flags_init & AUTH_3D_POST_PROCESS_INTENSITY) {
            post_process_blur_get_intensity(blur, &pp->intensity_init);
            pp->flags_init |= AUTH_3D_POST_PROCESS_INTENSITY;
        }

        post_process_blur_set_intensity(blur, (vec3*)&pp->intensity.value);
    }

    if (pp->flags & AUTH_3D_POST_PROCESS_LENS_FLARE) {
        if (~pp->flags_init & AUTH_3D_POST_PROCESS_LENS_FLARE) {
            pp->lens_flare_init = post_process_tone_map_get_lens_flare(tm);
            //pp->flags_init |= AUTH_3D_POST_PROCESS_LENS_FLARE;
        }

        post_process_tone_map_set_lens_ghost(tm, pp->lens_flare_value);
    }

    if (pp->flags & AUTH_3D_POST_PROCESS_LENS_GHOST) {
        if (~pp->flags_init & AUTH_3D_POST_PROCESS_LENS_GHOST) {
            pp->lens_ghost_init = post_process_tone_map_get_lens_ghost(tm);
            //pp->flags_init |= AUTH_3D_POST_PROCESS_LENS_GHOST;
        }

        post_process_tone_map_set_lens_ghost(tm, pp->lens_ghost_value);
    }

    if (pp->flags & AUTH_3D_POST_PROCESS_LENS_SHAFT) {
        if (~pp->flags_init & AUTH_3D_POST_PROCESS_LENS_SHAFT) {
            pp->lens_shaft_init = post_process_tone_map_get_lens_shaft(tm);
            //pp->flags_init |= AUTH_3D_POST_PROCESS_LENS_SHAFT;
        }

        post_process_tone_map_set_lens_shaft(tm, pp->lens_shaft_value);
    }

    if (pp->flags & AUTH_3D_POST_PROCESS_RADIUS) {
        if (~pp->flags_init & AUTH_3D_POST_PROCESS_RADIUS) {
            post_process_blur_get_radius(blur, &pp->radius_init);
            pp->flags_init |= AUTH_3D_POST_PROCESS_RADIUS;
        }

        post_process_blur_set_radius(blur, (vec3*)&pp->radius.value);
    }

    if (pp->flags & AUTH_3D_POST_PROCESS_SCENE_FADE) {
        if (~pp->flags_init & AUTH_3D_POST_PROCESS_SCENE_FADE) {
            vec4 scene_fade_init;
            post_process_tone_map_get_scene_fade(tm, &scene_fade_init);
            vec4_to_vec4u(scene_fade_init, pp->scene_fade_init);
            pp->flags_init |= AUTH_3D_POST_PROCESS_SCENE_FADE;
        }

        vec4 scene_fade;
        vec4u_to_vec4(pp->scene_fade.value, scene_fade);
        post_process_tone_map_set_scene_fade(tm, &scene_fade);
    }
}

static void auth_3d_post_process_get_value(auth_3d_post_process* pp, float_t frame) {
    if (pp->flags & AUTH_3D_POST_PROCESS_INTENSITY)
        auth_3d_rgba_get_value(&pp->intensity, frame);
    if (pp->flags & AUTH_3D_POST_PROCESS_LENS_FLARE)
        pp->lens_flare_value = auth_3d_key_get_value(&pp->lens_flare, frame);
    if (pp->flags & AUTH_3D_POST_PROCESS_LENS_SHAFT)
        pp->lens_shaft_value = auth_3d_key_get_value(&pp->lens_shaft, frame);
    if (pp->flags & AUTH_3D_POST_PROCESS_LENS_GHOST)
        pp->lens_ghost_value = auth_3d_key_get_value(&pp->lens_ghost, frame);
    if (pp->flags & AUTH_3D_POST_PROCESS_RADIUS)
        auth_3d_rgba_get_value(&pp->radius, frame);
    if (pp->flags & AUTH_3D_POST_PROCESS_SCENE_FADE)
        auth_3d_rgba_get_value(&pp->scene_fade, frame);
}

static void auth_3d_post_process_load(auth_3d* auth, auth_3d_post_process_file* ppf) {
    memset(&auth->post_process, 0, sizeof(auth_3d_post_process));
    auth_3d_post_process* pp = &auth->post_process;

    if (ppf->flags & A3DA_POST_PROCESS_INTENSITY) {
        auth_3d_rgba_load(auth, &pp->intensity, &ppf->intensity);
        pp->flags |= AUTH_3D_POST_PROCESS_INTENSITY;
    }

    if (ppf->flags & A3DA_POST_PROCESS_LENS_FLARE) {
        auth_3d_key_load(auth, &pp->lens_flare, &ppf->lens_flare);
        pp->flags |= AUTH_3D_POST_PROCESS_LENS_FLARE;
    }

    if (ppf->flags & A3DA_POST_PROCESS_LENS_GHOST) {
        auth_3d_key_load(auth, &pp->lens_ghost, &ppf->lens_ghost);
        pp->flags |= AUTH_3D_POST_PROCESS_LENS_GHOST;
    }

    if (ppf->flags & A3DA_POST_PROCESS_LENS_SHAFT) {
        auth_3d_key_load(auth, &pp->lens_shaft, &ppf->lens_shaft);
        pp->flags |= AUTH_3D_POST_PROCESS_LENS_SHAFT;
    }

    if (ppf->flags & A3DA_POST_PROCESS_RADIUS) {
        auth_3d_rgba_load(auth, &pp->radius, &ppf->radius);
        pp->flags |= AUTH_3D_POST_PROCESS_RADIUS;
    }

    if (ppf->flags & A3DA_POST_PROCESS_SCENE_FADE) {
        auth_3d_rgba_load(auth, &pp->scene_fade, &ppf->scene_fade);
        pp->flags |= AUTH_3D_POST_PROCESS_SCENE_FADE;
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
        vec4 scene_fade_init;
        vec4u_to_vec4(pp->scene_fade_init, scene_fade_init);
        post_process_tone_map_set_scene_fade(tm, &scene_fade_init);
    }
}
