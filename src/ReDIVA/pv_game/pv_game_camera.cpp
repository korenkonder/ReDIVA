/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "pv_game_camera.hpp"
#include "../../CRE/rob/rob.hpp"
#include "../../CRE/render_context.hpp"

struct pv_game_camera {
    bool enable;
    bool follow_chara;
    bool edit_camera;
    float_t duration;
    float_t curr_time;
    vec3 view_point_start;
    vec3 view_point_end;
    vec3 interest_start;
    vec3 interest_end;
    float_t start_distance;
    float_t end_distance;
    mat4 dir_mat;
    mat4 up_mat;
    vec3 view_direction;
    vec3 up_vec;
    float_t fov;
    float_t min_dist;
    int32_t chara_id;
    int32_t chara_follow_point;
    float_t acceleration_1;
    float_t acceleration_2;
    int32_t field_F4;
    float_t acceleration;
    float_t acceleration_curr_time;

    pv_game_camera();

    void reset();
};

extern render_context* rctx_ptr;

pv_game_camera pv_game_camera_data;

static float_t pv_game_camera_get_acceleration();
static bool pv_game_camera_get_edit_camera(cam_struct* cam, float_t delta_time);
static bool pv_game_camera_get_move_camera(cam_struct* cam, float_t delta_time);

pv_game_camera::pv_game_camera() : enable(), follow_chara(), edit_camera(), duration(), curr_time(),
start_distance(), end_distance(), fov(), min_dist(), chara_id(), chara_follow_point(),
acceleration_1(), acceleration_2(), field_F4(), acceleration(), acceleration_curr_time() {

}

void pv_game_camera::reset() {
    enable = false;
    follow_chara = false;
    edit_camera = false;
    duration = 0.0f;
    curr_time = 0.0f;
    view_point_end = 0.0f;
    view_point_start = 0.0f;
    interest_end = 0.0f;
    interest_start = 0.0f;
    dir_mat = mat4_identity;
    up_mat = mat4_identity;
    view_direction = { 0.0f, 0.0f, 1.0f };
    up_vec = { 0.0f, 1.0f, 0.0f };
    fov = 32.2673416137695f;
    min_dist = 0.05f;
    chara_id = 0;
    chara_follow_point = 0;
    acceleration_1 = 0.0f;
    acceleration_2 = 1.0f;
    field_F4 = 0;
    acceleration = 0.0f;
    acceleration_curr_time = 0.0f;
}

pv_game_camera_dsc_data::pv_game_camera_dsc_data() {

}

void pv_game_camera_ctrl(float_t delta_time) {
    if (!pv_game_camera_data.enable)
        return;

    bool in_transition;
    cam_struct cam;
    if (pv_game_camera_data.edit_camera)
        in_transition = pv_game_camera_get_edit_camera(&cam, delta_time);
    else
        in_transition = pv_game_camera_get_move_camera(&cam, delta_time);

    cam.fov = pv_game_camera_data.fov * DEG_TO_RAD_FLOAT;
    cam.min_distance = pv_game_camera_data.min_dist;
    cam.set(rctx_ptr->camera);

    if (!in_transition)
        pv_game_camera_data.reset();
}

void pv_game_camera_reset() {
    pv_game_camera_data.reset();
}

void pv_game_camera_set_dsc_data(float_t duration, pv_game_camera_dsc_data& start,
    pv_game_camera_dsc_data& end, float_t acceleration_1, float_t acceleration_2,
    int32_t follow_chara, int32_t chara_id, int32_t chara_follow_point, bool edit_camera) {
    pv_game_camera_data.reset();

    pv_game_camera_data.edit_camera = edit_camera;

    start.up_vec = vec3::normalize(start.up_vec);
    end.up_vec = vec3::normalize(end.up_vec);

    pv_game_camera_data.view_point_start = start.view_point;
    pv_game_camera_data.view_point_end = end.view_point;
    pv_game_camera_data.interest_start = start.interest;
    pv_game_camera_data.interest_end = end.interest;

    if (!edit_camera) {
        vec3 start_direction = start.view_point - start.interest;
        vec3 end_direction = end.view_point - end.interest;

        pv_game_camera_data.start_distance = vec3::length(start_direction);
        pv_game_camera_data.end_distance = vec3::length(end_direction);

        start_direction = vec3::normalize(start_direction);
        end_direction = vec3::normalize(end_direction);

        pv_game_camera_data.view_direction = start_direction;
        pv_game_camera_data.up_vec = start.up_vec;

        mat4_from_two_vectors(&start_direction, &end_direction, &pv_game_camera_data.dir_mat);
        mat4_from_two_vectors(&start.up_vec, &end.up_vec, &pv_game_camera_data.up_mat);
    }

    pv_game_camera_data.duration = duration;
    pv_game_camera_data.curr_time = 0.0f;

    if (follow_chara) {
        pv_game_camera_data.follow_chara = true;
        pv_game_camera_data.chara_id = chara_id;

        switch (chara_follow_point) {
        case 0:
            pv_game_camera_data.chara_follow_point = 4;
            break;
        case 1:
            pv_game_camera_data.chara_follow_point = 1;
            break;
        default:
            pv_game_camera_data.chara_follow_point = 0;
            break;
        }
    }

    pv_game_camera_data.enable = true;
    pv_game_camera_data.field_F4 = 0;
    pv_game_camera_data.acceleration = 0.0f;
    pv_game_camera_data.acceleration_curr_time = 0.0f;
    pv_game_camera_data.acceleration_1 = acceleration_1;
    pv_game_camera_data.acceleration_2 = acceleration_2;
}

void pv_game_camera_set_fov_min_dist(float_t fov, float_t min_dist) {
    pv_game_camera_data.fov = fov;
    pv_game_camera_data.min_dist = min_dist;
}

static float_t sub_14011B160(float_t acceleration_1, float_t acceleration_2, float_t t) {
    if (acceleration_2 == 1.0f)
        return 1.0f;

    if (acceleration_1 == 0.0f)
        return cosf((float_t)(t * M_PI)) * (acceleration_2 - 1.0f) + 1.0f;
    else if (acceleration_1 == 1.0f)
        return cosf((float_t)(t * M_PI + M_PI))
            * (float_t)abs((int32_t)(1.0f - acceleration_2)) + 1.0f;
    else if (t < acceleration_1)
        return 2.0f - (cosf((float_t)((t / acceleration_1) * M_PI))
            * (acceleration_2 - 1.0f) + 1.0f);
    else
        return cosf((float_t)((t - acceleration_1) / (1.0f - acceleration_1) * M_PI + M_PI))
            * (1.0f - acceleration_2) + 1.0f;
}

static float_t pv_game_camera_get_acceleration() {
    bool v2 = false;
    const float_t curr_time = pv_game_camera_data.curr_time;
    const float_t inv_duration = 1.0f / pv_game_camera_data.duration;
    const float_t acceleration_1 = pv_game_camera_data.acceleration_1;
    const float_t acceleration_2 = pv_game_camera_data.acceleration_2;
    float_t acceleration = pv_game_camera_data.acceleration;
    float_t acceleration_curr_time = pv_game_camera_data.acceleration_curr_time;
    while (acceleration_curr_time < curr_time) {
        float_t delta_time = 0.005f;
        if (v2)
            delta_time = 0.0005f;
        else if (acceleration_curr_time + 0.005f >= curr_time) {
            v2 = true;
            delta_time = 0.0005f;
        }
        float_t _acceleration = sub_14011B160(acceleration_1, acceleration_2,
            inv_duration * acceleration_curr_time);
        acceleration_curr_time += delta_time;
        pv_game_camera_data.acceleration_curr_time = acceleration_curr_time;
        acceleration += _acceleration * (inv_duration * delta_time);
        pv_game_camera_data.acceleration = acceleration;
    }
    return min_def(acceleration, 1.0f);
}

static bool pv_game_camera_get_edit_camera(cam_struct* cam, float_t delta_time) {
    if (!cam)
        return false;

    pv_game_camera_data.curr_time += delta_time;

    vec3 interest;
    vec3 view_point;
    bool in_transition;
    if (pv_game_camera_data.duration == 0.0f
        || pv_game_camera_data.duration <= pv_game_camera_data.curr_time) {
        view_point = pv_game_camera_data.view_point_end;
        interest = pv_game_camera_data.interest_end;
        in_transition = false;
    }
    else {
        float_t t = pv_game_camera_get_acceleration();
        interest = vec3::lerp(pv_game_camera_data.interest_start, pv_game_camera_data.interest_end, t);
        view_point = vec3::lerp(pv_game_camera_data.view_point_start, pv_game_camera_data.view_point_end, t);
        in_transition = true;
    }

    if (pv_game_camera_data.follow_chara) {
        rob_chara* rob_chr = rob_chara_array_get(pv_game_camera_data.chara_id);
        if (rob_chr)
            mat4_get_translation(&rob_chr->data.field_1E68.field_78[
                pv_game_camera_data.chara_follow_point], &interest);
    }

    cam->interest = interest;
    cam->view_point = view_point;
    return in_transition;
}

static bool pv_game_camera_get_move_camera(cam_struct* cam, float_t delta_time) {
    if (!cam)
        return false;

    pv_game_camera_data.curr_time += delta_time;

    mat4 dir_mat;
    mat4 up_mat;
    vec3 interest;
    vec3 view_point;
    float_t distance;
    bool in_transition;
    if (pv_game_camera_data.duration == 0.0f
        || pv_game_camera_data.duration <= pv_game_camera_data.curr_time) {
        dir_mat = pv_game_camera_data.dir_mat;
        up_mat = pv_game_camera_data.up_mat;
        view_point = pv_game_camera_data.view_point_end;
        interest = pv_game_camera_data.interest_end;
        distance = pv_game_camera_data.end_distance;
        in_transition = false;
    }
    else {
        float_t t = pv_game_camera_get_acceleration();
        mat4_blend(&mat4_identity, &pv_game_camera_data.dir_mat, &dir_mat, t);
        mat4_blend(&mat4_identity, &pv_game_camera_data.up_mat, &up_mat, t);
        distance = ((pv_game_camera_data.end_distance - pv_game_camera_data.start_distance) * t)
            + pv_game_camera_data.start_distance;
        interest = vec3::lerp(pv_game_camera_data.interest_start, pv_game_camera_data.interest_end, t);
        view_point = vec3::lerp(pv_game_camera_data.view_point_start, pv_game_camera_data.view_point_end, t);
        distance = lerp_def(pv_game_camera_data.start_distance, pv_game_camera_data.end_distance, t);
        in_transition = true;
    }

    if (pv_game_camera_data.follow_chara) {
        rob_chara* rob_chr = rob_chara_array_get(pv_game_camera_data.chara_id);
        if (rob_chr)
            mat4_get_translation(&rob_chr->data.field_1E68.field_78[
                pv_game_camera_data.chara_follow_point], &interest);
    }

    if (!pv_game_camera_data.follow_chara) {
        vec3 direction;
        mat4_transform_vector(&dir_mat, &pv_game_camera_data.view_direction, &direction);
        view_point = direction * distance + interest;
    }

    cam->interest = interest;
    cam->view_point = view_point;

    mat4_transform_vector(&up_mat, &pv_game_camera_data.up_vec, &cam->up);
    cam->use_up = true;
    return in_transition;
}
