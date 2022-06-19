/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "task_effect.hpp"
#include "auth_3d.hpp"
#include "data.hpp"
#include "render_context.hpp"
#include "shader_ft.hpp"
#include "texture.hpp"

extern render_context* rctx_ptr;

static TaskEffectFogAnim::Data* task_effect_fog_anim_data;
static TaskEffectFogRing::Data* task_effect_fog_ring_data;

TaskEffect::TaskEffect() {

}

TaskEffect::~TaskEffect() {

}

void TaskEffect::PreInit(int32_t stage_index) {

}

void TaskEffect::SetStageIndices(std::vector<int32_t>& stage_indices) {
    PreInit(stage_indices[0]);
}

void TaskEffect::SetFrame(int32_t value) {

}

void TaskEffect::Field_48() {

}

void TaskEffect::SetEnable(bool value) {

}

void TaskEffect::SetStageIndex(int32_t value) {

}

void TaskEffect::SetFrameRateControl(FrameRateControl* value) {

}

void TaskEffect::Field_68() {

}

void TaskEffect::Reset() {

}

void TaskEffect::Field_78() {

}

void TaskEffect::Field_80() {

}

void TaskEffect::Field_88() {

}

void TaskEffect::Field_90() {

}

void TaskEffect::Field_98(int32_t a2, int32_t* a3) {
    if (a3)
        *a3 = 0;
}

void TaskEffect::Field_A0(int32_t a2, int32_t* a3) {
    if (a3)
        *a3 = 0;
}

void TaskEffect::Field_A8(int32_t a2, int8_t* a3) {
    if (a3)
        *a3 = 0;
}

TaskEffectAuth3D::Stage::Stage() : auth_3d_ids() {
    count = 0;
    max_count = TASK_STAGE_STAGE_COUNT;
    for (int32_t& i : auth_3d_ids)
        i = -1;
    auth_3d_ids_ptr = auth_3d_ids;
}

TaskEffectAuth3D::TaskEffectAuth3D() : enable(),
field_13C(), field_158(), field_159(), field_15C(), frame() {
    stage_index = -1;
}

TaskEffectAuth3D::~TaskEffectAuth3D() {

}

bool TaskEffectAuth3D::Init() {
    if (!task_auth_3d_check_task_ready()) {
        SetDest();
        return true;
    }

    if (stage.count) {
        for (int32_t i = 0; i < stage.count; i++)
            if (!auth_3d_data_check_id_loaded(&stage.auth_3d_ids_ptr[i]))
                return false;
    }

    for (int32_t i = 0; i < stage.count; i++) {
        int32_t& id = stage.auth_3d_ids_ptr[i];
        auth_3d_data_set_repeat(&id, true);
        auth_3d_data_set_paused(&id, false);
        auth_3d_data_set_enable(&id, true);
    }

    //if (field_158)
    //    sub_14036D310(2, (__int64)sub_140345B70, (__int64)this);
    return true;
}

bool TaskEffectAuth3D::Ctrl() {
    if (field_158 && field_15C > 0)
        field_15C = 0;

    if (field_159) {
        int32_t frame_int;
        if (frame >= 0.0)
        {
            frame_int = (int32_t)frame;
            frame += get_delta_frame();
            if (frame > (float_t)(512 * 360))
                frame = 0.0f;
        }
        else
            frame_int = 0;

        mat4 mat;
        mat4_translate(0.0f, -0.2f, 0.0f, &mat);
        mat4_rotate_x_mult(&mat, sinf((float_t)((double_t)((float_t)(frame_int % 512)
            * (float_t)(2.0 / 512.0)) * M_PI)) * 0.015f, &mat);
        mat4_rotate_z_mult(&mat, sinf((float_t)((double_t)((float_t)(frame_int % 360)
            * (float_t)(2.0 / 360.0)) * M_PI)) * 0.015f, &mat);
        task_stage_set_mat(&mat);
    }
    return false;
}

bool TaskEffectAuth3D::Dest() {
    //if (field_158)
    //    sub_14036D1E0(2, (__int64)sub_140345B70, (__int64)this);
    ResetData();
    return true;
}

void TaskEffectAuth3D::Disp() {

}

void TaskEffectAuth3D::PreInit(int32_t stage_index) {

}

void sub_140344BE0(TaskEffectAuth3D* a1, int32_t stage_index) {
}

void TaskEffectAuth3D::SetStageIndices(std::vector<int32_t>& stage_indices) {
    this->stage_indices = stage_indices;
    for (int32_t& i : this->stage_indices) {
        data_struct* aft_data = &data_list[DATA_AFT];
        auth_3d_database* aft_auth_3d_db = &aft_data->data_ft.auth_3d_db;
        stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

        stage_data* stage_data = aft_stage_data->get_stage_data(i);
        if (!stage_data || !stage_data->auth_3d_ids.size())
            continue;

        struc_621 v30;
        v30.stage_index = i;

        if (stage.count != stage.max_count) {
            int32_t id = -1;
            for (int32_t& j : stage_data->auth_3d_ids) {
                id = -1;
                if (stage.count == stage.max_count)
                    break;

                id = auth_3d_data_load_uid(j, aft_auth_3d_db);
                if (auth_3d_data_check_id_not_empty(&id))
                    break;
            }

            if (id != -1) {
                auth_3d_data_read_file(&id, aft_auth_3d_db);
                auth_3d_data_set_visibility(&id, false);
                if (stage.count + 1 <= stage.max_count)
                    stage.auth_3d_ids_ptr[stage.count++] = id;
                v30.auth_3d_ids.push_back(id);
            }
        }

        field_120.push_back(v30);
    }
}

void TaskEffectAuth3D::SetFrame(int32_t value) {
    double_t frame = (double_t)value;
    for (int32_t i = 0; i < stage.count; i++) {
        int32_t& id = stage.auth_3d_ids_ptr[i];
        float_t play_control_size = auth_3d_data_get_play_control_size(&id);
        float_t req_frame;
        if (play_control_size != 0.0)
            req_frame = (float_t)fmod(frame, play_control_size);
        else
            req_frame = 0.0f;
        auth_3d_data_set_req_frame(&id, req_frame);
    }
}

void TaskEffectAuth3D::SetStageIndex(int32_t value) {
    if (stage_index == value)
        return;

    SetVisibility(false);
    stage_index = value;
    SetVisibility(enable);
}

void TaskEffectAuth3D::SetFrameRateControl(FrameRateControl* value) {
    for (int32_t i = 0; i < stage.count; i++) {
        int32_t& id = stage.auth_3d_ids_ptr[i];
        auth_3d_data_set_frame_rate(&id, value);
    }
}

void TaskEffectAuth3D::Reset() {
    for (int32_t i = 0; i < stage.count; i++) {
        int32_t& id = stage.auth_3d_ids_ptr[i];
        auth_3d_data_set_enable(&id, true);
        auth_3d_data_set_paused(&id, false);
        auth_3d_data_set_req_frame(&id, 0.0f);
    }
}

void TaskEffectAuth3D::ResetData() {
    for (int32_t i = 0; i < stage.count; i++) {
        int32_t& id = stage.auth_3d_ids_ptr[i];
        auth_3d_data_unload_id(id, rctx_ptr);
    }
    stage.count = 0;
    enable = false;

    field_120.clear();
    stage_index = -1;
    stage_indices.clear();
    field_158 = 0;
    field_159 = 0;
    field_15C = 0;
    frame = -1.0f;
}

void TaskEffectAuth3D::SetVisibility(bool value) {
    if (stage_index == -1)
        return;

    for (struc_621& i : field_120) {
        if (i.stage_index != stage_index)
            continue;

        for (int32_t& j : i.auth_3d_ids)
            auth_3d_data_set_visibility(&j, value);
    }
}

void TaskEffectAuth3D::SetEnable(bool value) {
    enable = value;
    SetVisibility(value);
}

TaskEffectFogAnim::Data::Data() : field_0(), field_4(), field_8(), field_C(),
field_18(), field_24(), field_28(), field_2C(), field_30(), field_34() {
    Reset();
}

void TaskEffectFogAnim::Data::Ctrl() {
    if (!field_0 || !field_8)
        return;

    float_t v4 = field_C[0] * DEG_TO_RAD_FLOAT;
    float_t v7 = (sinf(v4 - 1.5f) + 1.0f) * 0.5f + field_34.x;
    v7 = min(v7, 1.0f) * field_24;
    float_t v8 = sinf(v4) * v7;
    float_t v9 = cosf(v4);

    vec4 color;
    color.x = (float_t)((v9 * v7) * 1.4022 + 1.0);
    color.y = (float_t)(1.0 - v8 * 0.345686 - ((double_t)v9 * v7) * 0.714486);
    color.z = (float_t)(v8 * 1.771 + 1.0);
    color.w = 1.0f;
    vec3_max(*(vec3*)&color, vec3_null, *(vec3*)&color);

    for (int32_t i = 0; i < 3; i++) {
        float_t v20 = (field_18[i] * 10.0f) + field_C[i];
        if (v20 > 360.0)
            v20 = fmodf(v20, 360.0);
        field_C[i] = v20;
    }

    fog& fog = rctx_ptr->fog[FOG_DEPTH];
    fog.set_color(color);

    vec3_add_scalar(*(vec3*)&color, 1.0f, *(vec3*)&color);
    vec3_mult_scalar(*(vec3*)&color, 0.8f, *(vec3*)&color);

    light_set& light_set = rctx_ptr->light_set[LIGHT_SET_MAIN];
    light_set.lights[LIGHT_CHARA].set_diffuse(color);
    light_set.lights[LIGHT_CHARA].set_specular(color);
    light_set.lights[LIGHT_STAGE].set_diffuse(color);
    light_set.lights[LIGHT_STAGE].set_specular(color);
}

void TaskEffectFogAnim::Data::Reset() {
    field_0 = 0;
    field_4 = -1;
    field_8 = 1;
    field_C[0] = 0.0f;
    field_C[1] = 0.0f;
    field_C[2] = 0.0f;
    field_18[0] = 0.0f;
    field_18[1] = 0.0f;
    field_18[2] = 0.0f;
    field_24 = 0;
    field_28 = 0;
    field_2C = 0;
    field_30 = 0;
    field_34 = vec4u_identity;
}

TaskEffectFogAnim::TaskEffectFogAnim() {

}

TaskEffectFogAnim::~TaskEffectFogAnim() {

}

bool TaskEffectFogAnim::Init() {
    return true;
}

bool TaskEffectFogAnim::Ctrl() {
    data.Ctrl();
    return false;
}

bool TaskEffectFogAnim::Dest() {
    data.Reset();
    task_effect_fog_anim_data = 0;
    return true;
}

void TaskEffectFogAnim::Disp() {

}

void TaskEffectFogAnim::PreInit(int32_t) {
    data.Reset();
    task_effect_fog_anim_data = &data;
}

TaskEffectFogRing::Data::Data() {

}

TaskEffectFogRing::Data::~Data() {

}

void TaskEffectFogRing::Data::Ctrl() {
    if (!disp)
        return;

    //sub_140349FD0(this, delta_frame * (float_t)(1.0 / 60.0));
    //sub_1403495B0(this);
    field_8 = 0;
}

void TaskEffectFogRing::Data::SetStageIndices(std::vector<int32_t>& stage_indices) {

}

TaskEffectFogRing::TaskEffectFogRing() {

}

TaskEffectFogRing::~TaskEffectFogRing() {

}

bool TaskEffectFogRing::Init() {
    return true;
}

bool TaskEffectFogRing::Ctrl() {
    data.delta_frame = data.frame_rate_control->GetDeltaFrame();
    data.Ctrl();;
    return 0;
}

bool TaskEffectFogRing::Dest() {
    //sub_140348090(&this->data);
    task_effect_fog_ring_data = 0;
    return 1;
}

void draw_fog_particle(TaskEffectFogRing::Data* data) {
    if (!data->count)
        return;

    glPushAttrib(GL_SCISSOR_BIT | GL_TEXTURE_BIT | GL_LIST_BIT | GL_EVAL_BIT | GL_HINT_BIT
        | GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT | GL_TRANSFORM_BIT | GL_VIEWPORT_BIT
        | GL_STENCIL_BUFFER_BIT | GL_ACCUM_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_FOG_BIT
        | GL_LIGHTING_BIT | GL_PIXEL_MODE_BIT | GL_POLYGON_STIPPLE_BIT | GL_POLYGON_BIT | GL_LINE_BIT | GL_CLIENT_VERTEX_ARRAY_BIT | GL_CLIENT_PIXEL_STORE_BIT);
    glPushClientAttrib(GL_POINT_BIT);
    glDisableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    shaders_ft.set(SHADER_FT_FOGPTCL);
    glEnable(GL_BLEND);
    glPointSize(data->point_size);
    glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
    glEnable(GL_POINT_SPRITE);
    glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, 1);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    texture* tex = texture_storage_get_texture(data->tex_id);
    if (tex)
        gl_state_active_bind_texture_2d(0, tex->tex);

    glVertexPointer(3, GL_FLOAT, 32, data->data);
    glColorPointer(4, GL_FLOAT, 32, &data->data->color);
    glEnableVertexAttribArray(0xE);
    glVertexAttribPointer(0xE, 1, GL_FLOAT, 0, 32, &data->data->size);
    glDrawArrays(GL_POINTS, 0, data->count);
    shaders_ft.set(-1);
    glPopClientAttrib();
    glPopAttrib();
}

void TaskEffectFogRing::Disp() {
    //if (data.enable && data.disp)
    //    draw_task_preprocess_append((void(*)(void*))draw_fog_particle, &data, DRAW_OBJECT_RIPPLE);
}

void TaskEffectFogRing::PreInit(int32_t stage_index) {

}

void TaskEffectFogRing::SetStageIndices(std::vector<int32_t>& stage_indices) {
    SetFrameRateControl(0);
    task_effect_fog_ring_data = &data;
    data.SetStageIndices(stage_indices);
}

void TaskEffectFogRing::SetEnable(bool value) {
    data.enable = value;
}

void TaskEffectFogRing::SetStageIndex(int32_t value) {
    if (data.stage_index == value)
        return;

    data.stage_index = value;
    if (value != -1) {
        for (int32_t& i : data.stage_indices)
            if (i == value) {
                data.disp = true;
                return;
            }
    }
    data.disp = false;
}

void TaskEffectFogRing::SetFrameRateControl(FrameRateControl* value) {
    if (value)
        data.frame_rate_control = value;
    else
        data.frame_rate_control = &sys_frame_rate;
}

void TaskEffectFogRing::Reset() {
    //sub_140349C30(&this->data);
}
