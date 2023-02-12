/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "light.hpp"
#include "../../../CRE/stage.hpp"
#include "../../imgui_helper.hpp"

struct data_view_light {
    render_context* rctx;
    light_id id;
    bool sync_pos;
    bool light_auth_enable;
    int32_t stage_index;
    int32_t stage_index_prev;

    data_view_light();
    ~data_view_light();

    static void column_slider_float_button(const char* label, float_t* val);
};

extern int32_t width;
extern int32_t height;

static const char* graphics_light_window_title = "Light##Graphics";

bool graphics_light_init(class_data* data, render_context* rctx) {
    graphics_light_dispose(data);

    data_view_light* light = new data_view_light;
    if (light) {
        light->rctx = rctx;

        int32_t stage_index = task_stage_get_current_stage_index();
        stage_index = max_def(stage_index, 0);
        light->stage_index = stage_index;
        light->stage_index_prev = stage_index;
    }
    data->data = light;
    return true;
}

void graphics_light_ctrl(class_data* data) {
    if (data->flags & (CLASS_HIDE | CLASS_HIDDEN))
        return;

    data_view_light* light = (data_view_light*)data->data;
    if (!light)
        return;

    if (light->stage_index != light->stage_index_prev
        && !light_param_data_storage_data_load_file()) {
        light->stage_index_prev = light->stage_index;
        light_param_data_storage_data_set_stage(light->stage_index);
    }
}

void graphics_light_imgui(class_data* data) {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    float_t w = min_def((float_t)width, 464.0f);
    float_t h = min_def((float_t)height, 604.0f);

    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Appearing);

    data->imgui_focus = false;
    bool open = data->flags & CLASS_HIDDEN ? false : true;
    bool collapsed = !ImGui::Begin(graphics_light_window_title, &open, 0);
    if (!open) {
        enum_or(data->flags, CLASS_HIDE);
        ImGui::End();
        return;
    }
    else if (collapsed) {
        ImGui::End();
        return;
    }

    data_view_light* light = (data_view_light*)data->data;
    if (!light) {
        ImGui::End();
        return;
    }

    render_context* rctx = light->rctx;

    data_struct* aft_data = &data_list[DATA_AFT];
    stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

    std::vector<stage_data>& stg_data = aft_stage_data->stage_data;

    light_set* light_set = rctx->light_set;

    int32_t stage_index = light_param_data_storage_data_get_stage_index();

    int32_t _stage_index = stage_index < 0 ? light->stage_index : stage_index;

    ImGui::TextCentered("STAGE (Light/Fog/PP)");

    ImGui::GetContentRegionAvailSetNextItemWidth();
    if (ImGui::BeginCombo("##Stage Index", _stage_index > -1
        ? stg_data[_stage_index].name.c_str() : "", 0)) {
        for (stage_data& i : stg_data) {
            int32_t stage_idx = (int32_t)(&i - stg_data.data());

            ImGui::PushID(&i);
            if (ImGui::Selectable(i.name.c_str(), _stage_index == stage_idx)
                || ImGui::ItemKeyPressed(ImGuiKey_Enter)
                || (ImGui::IsItemFocused() && _stage_index != stage_idx)) {
                stage_index = -1;
                _stage_index = stage_idx;
            }
            ImGui::PopID();

            if (_stage_index == stage_idx)
                ImGui::SetItemDefaultFocus();
        }

        data->imgui_focus |= ImGui::IsWindowFocused();
        ImGui::EndCombo();
    }

    ::light_data* light_data = &light_set->lights[light->id];

    const char* light_id_labels[] = {
        "0:CHARA",
        "1:STAGE",
        "2:SUN",
        "3:REFLECT",
        "4:SHADOW",
        "5:CHARA COLOR",
        "6:CHARA(F)",
        "7:PROJECTION",
    };

    ImGui::TextCentered("ID");

    ImGui::GetContentRegionAvailSetNextItemWidth();
    ImGui::ComboBox("##ID", light_id_labels, LIGHT_MAX,
        (int32_t*)&light->id, 0, false, &data->imgui_focus);

    const char* light_type_labels[] = {
        "OFF",
        "PARALLEL",
        "POINT",
        "SPOT",
    };

    ImGui::TextCentered("TYPE");

    light_type type = light_data->get_type();
    ImGui::GetContentRegionAvailSetNextItemWidth();
    ImGui::ComboBox("##Type", light_type_labels, LIGHT_SPOT,
        (int32_t*)&type, 0, true, &data->imgui_focus);
    light_data->set_type(type);

    if (light->id == LIGHT_CHARA || light->id == LIGHT_TONE_CURVE) {
        ::light_data* light_data = &light_set->lights[LIGHT_TONE_CURVE];

        ImGui::TextCentered("CHARA(F) Tone Curve Adjustment");

        ImGui::PushID("Tone Curve");
        light_tone_curve tone_curve;
        light_data->get_tone_curve(tone_curve);
        ImGui::ColumnSliderFloatButton("Start Point", &tone_curve.start_point, 0.01f, 0.0f, 1.0f, 0.1f, "%4.2f", 0);
        ImGui::ColumnSliderFloatButton("End Point", &tone_curve.end_point, 0.01f, 0.0f, 1.0f, 0.1f, "%4.2f", 0);
        ImGui::ColumnSliderFloatButton("Coefficient", &tone_curve.coefficient, 0.01f, 0.0f, 1.0f, 0.1f, "%4.2f", 0);
        light_data->set_tone_curve(tone_curve);
        ImGui::PopID();

        ImGui::Checkbox("Light 0-6 Sync Position", &light->sync_pos);
        ImGui::Checkbox("Light Auth Enable", &light->light_auth_enable);
    }

    if (light_data->type != LIGHT_OFF) {
        ImGui::TextCentered("AMBIENT");

        ImGui::PushID("Ambient");
        vec4 ambient;
        light_data->get_ambient(ambient);
        ImGui::ColumnSliderFloatButton("R", &ambient.x, 0.01f, 0.0f, 2.0f, 0.1f, "%4.3f", 0);
        ImGui::ColumnSliderFloatButton("G", &ambient.y, 0.01f, 0.0f, 2.0f, 0.1f, "%4.3f", 0);
        ImGui::ColumnSliderFloatButton("B", &ambient.z, 0.01f, 0.0f, 2.0f, 0.1f, "%4.3f", 0);
        light_data->set_ambient(ambient);
        ImGui::PopID();

        ImGui::TextCentered("DIFFUSE");

        ImGui::PushID("Diffuse");
        vec4 diffuse;
        light_data->get_diffuse(diffuse);
        ImGui::ColumnSliderFloatButton("R", &diffuse.x, 0.01f, 0.0f, 2.0f, 0.1f, "%4.3f", 0);
        ImGui::ColumnSliderFloatButton("G", &diffuse.y, 0.01f, 0.0f, 2.0f, 0.1f, "%4.3f", 0);
        ImGui::ColumnSliderFloatButton("B", &diffuse.z, 0.01f, 0.0f, 2.0f, 0.1f, "%4.3f", 0);
        light_data->set_diffuse(diffuse);
        ImGui::PopID();

        ImGui::TextCentered("SPECULAR");

        ImGui::PushID("Specular");
        vec4 specular;
        light_data->get_specular(specular);
        ImGui::ColumnSliderFloatButton("R", &specular.x, 0.01f, 0.0f, 2.0f, 0.1f, "%4.3f", 0);
        ImGui::ColumnSliderFloatButton("G", &specular.y, 0.01f, 0.0f, 2.0f, 0.1f, "%4.3f", 0);
        ImGui::ColumnSliderFloatButton("B", &specular.z, 0.01f, 0.0f, 2.0f, 0.1f, "%4.3f", 0);
        ImGui::ColumnSliderFloatButton("A", &specular.w, 0.02f, 0.0f, 4.0f, 0.2f, "%4.3f", 0);
        light_data->set_specular(specular);
        ImGui::PopID();

        ImGui::TextCentered("POSITION");

        ImGui::PushID("Position");
        vec3 position;
        light_data->get_position(position);

        float_t rot_y;
        if (fabsf(position.x) > 0.000001f || fabsf(position.y) > 0.000001f)
            rot_y = atan2f(position.z, position.x) * RAD_TO_DEG_FLOAT;
        else
            rot_y = 0.0f;

        data_view_light::column_slider_float_button("X", &position.x);
        data_view_light::column_slider_float_button("Y", &position.y);
        data_view_light::column_slider_float_button("Z", &position.z);
        if (ImGui::ColumnSliderFloatButton("ROTY", &rot_y, 0.01f, -180.0f, 180.0f, 0.1f, "%6.2f", 0)) {
            vec2 pos = { position.x, position.z };
            float_t len = vec2::length(pos);
            position.x = cosf(rot_y * DEG_TO_RAD_FLOAT) * len;
            position.z = sinf(rot_y * DEG_TO_RAD_FLOAT) * len;
        }
        light_data->set_position(position);
        ImGui::PopID();
    }

    if (light_data->type == LIGHT_SPOT) {
        ImGui::TextCentered("SPOT DIRECTION");

        ImGui::PushID("Spot Direction");
        vec3 spot_direction;
        light_data->get_spot_direction(spot_direction);
        ImGui::ColumnSliderFloatButton("X", &spot_direction.x, 0.02f, -1.0f, 1.0f, 0.1f, "%5.2f", 0);
        ImGui::ColumnSliderFloatButton("Y", &spot_direction.y, 0.02f, -1.0f, 1.0f, 0.1f, "%5.2f", 0);
        ImGui::ColumnSliderFloatButton("Z", &spot_direction.z, 0.02f, -1.0f, 1.0f, 0.1f, "%5.2f", 0);
        light_data->set_spot_direction(spot_direction);
        ImGui::PopID();

        if (light->id == LIGHT_REFLECT) {
            light_clip_plane clip_plane;
            light_data->get_clip_plane(clip_plane);
            ImGui::Checkbox("Reflective surface clip(STAGE)", &clip_plane.data[1]);
            light_data->set_clip_plane(clip_plane);
        }

        ImGui::TextCentered("SPOT EXPONENT");

        float_t spot_exponent = light_data->get_spot_exponent();
        ImGui::ColumnSliderFloatButton("EXPONENT", &spot_exponent, 1.0f, 0.0f, 128.0f, 10.0f, "%8.4f", 0);
        light_data->set_spot_exponent(spot_exponent);

        ImGui::TextCentered("SPOT CUTOFF");

        float_t spot_cutoff = light_data->get_spot_cutoff();
        ImGui::ColumnSliderFloatButton("CUTOFF", &spot_cutoff, 0.0f, 0.0f, 90.0f, 5.0f, "%8.4f", 0);
        light_data->set_spot_cutoff(spot_cutoff);

        ImGui::TextCentered("ATTENUATION");

        ImGui::PushID("Attenuation");
        light_attenuation attenuation;
        light_data->get_attenuation(attenuation);
        ImGui::ColumnSliderFloatButton("CONSTANT", &attenuation.constant, 0.01f, 0.0f, 1.0f, 0.1f, "%8.4f", 0);
        ImGui::ColumnSliderFloatButton("LINEAR", &attenuation.linear, 0.01f, 0.0f, 1.0f, 0.1f, "%8.4f", 0);
        ImGui::ColumnSliderFloatButton("QUADRATIC", &attenuation.quadratic, 0.01f, 0.0f, 1.0f, 0.1f, "%8.4f", 0);
        light_data->set_attenuation(attenuation);
    }

    if (_stage_index != stage_index && _stage_index != light->stage_index) {
        light_param_data_storage_data_load_stage(_stage_index);
        light->stage_index = _stage_index;
    }

    data->imgui_focus |= ImGui::IsWindowFocused();
    ImGui::End();
}

bool graphics_light_dispose(class_data* data) {
    data_view_light* light = (data_view_light*)data->data;
    if (light)
        delete light;
    data->data = 0;
    return true;
}

data_view_light::data_view_light() : rctx(), id(), sync_pos(),
light_auth_enable(), stage_index(), stage_index_prev() {

}

data_view_light::~data_view_light() {

}

void data_view_light::column_slider_float_button(const char* label, float_t* val) {
    float_t val_abs = fabsf(*val);
    float_t step;
    if (val_abs < 150.0f)
        step = 1.0;
    else if (val_abs >= 1900.0)
        step = 50.0f;
    else
        step = 10.0f;

    ImGui::ColumnSliderFloatButton(label, val,
        step * 0.2f, 1.0f - step * 200.0f, step * 200.0f - 1.0f, step, "%6.2f", 0);
}
