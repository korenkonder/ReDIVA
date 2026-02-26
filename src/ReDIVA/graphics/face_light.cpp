/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "face_light.hpp"
#include "../../CRE/light_param/face.hpp"
#include "../../CRE/light_param.hpp"
#include "../../CRE/render_context.hpp"
#include "../dw.hpp"

extern render_context* rctx_ptr;

class DwFaceLight : public dw::Shell {
public:
    class SliderSelectionListener : public dw::SelectionAdapter {
    public:
        SliderSelectionListener();
        virtual ~SliderSelectionListener() override;

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    dw::Button* set_for_each_cut_button;
    face face_data;
    dw::Slider* strength;
    //int64_t field_1B0;
    //int64_t field_1B8;
    bool set_for_each_cut;

    DwFaceLight();
    virtual ~DwFaceLight() override;
    virtual void Draw() override;

    virtual void Hide() override;

    void FaceDataGet();

    static void ResetCallback(dw::Widget* data);
    static void SetForEachPvCutCallback(dw::Widget* data);
};

DwFaceLight* dw_face_light;
DwFaceLight::SliderSelectionListener dw_face_light_slider_selection_listener;

void dw_face_light_init() {
    if (!dw_face_light) {
        dw_face_light = new DwFaceLight;
        dw_face_light->LimitPosDisp();
    }
    else
        dw_face_light->Disp();
}

DwFaceLight::SliderSelectionListener::SliderSelectionListener() {

}

DwFaceLight::SliderSelectionListener::~SliderSelectionListener() {

}

void DwFaceLight::SliderSelectionListener::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data->widget);
    if (slider)
        ((face*)slider->callback_data.v64)->offset = slider->GetValue();
}

DwFaceLight::DwFaceLight() {
    data_struct* aft_data = &data_list[DATA_AFT];
    stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

    FaceDataGet();

    SetText("FACE LIGHT");

    dw::Button* reset = new dw::Button(this);
    reset->SetText("RESET");
    reset->callback = DwFaceLight::ResetCallback;

    const char* set_for_each_cut_text;
    const char* strength_text;
    if (dw::translate) {
        set_for_each_cut_text = u8"Set for each PV cut";
        strength_text = u8"Strength";
    }
    else {
        set_for_each_cut_text = u8"PVのカットごとに設定";
        strength_text = u8"強さ";
    }

    set_for_each_cut_button = new dw::Button(this, dw::CHECKBOX);
    set_for_each_cut_button->SetText(set_for_each_cut_text);
    set_for_each_cut_button->SetValue(set_for_each_cut);
    set_for_each_cut_button->callback_data.i64 = 0i64;
    set_for_each_cut_button->callback = DwFaceLight::SetForEachPvCutCallback;

    strength = dw::Slider::Create(this);
    strength->SetText(strength_text);
    strength->SetParams(face_data.offset, 0.0f, 10.0f, 1.0f, 0.25f, 1.0f);
    strength->callback_data.v64 = &face_data;
    strength->format = "%5.2f";
    strength->AddSelectionListener(&dw_face_light_slider_selection_listener);

    UpdateLayout();
}

DwFaceLight::~DwFaceLight() {

}

void DwFaceLight::Draw() {
    strength->SetValue(face_data.offset);
    rctx_ptr->face = face_data;
    dw::Shell::Draw();
}

void DwFaceLight::Hide() {
    SetDisp();
}

void DwFaceLight::FaceDataGet() {
    face_data = rctx_ptr->face;
}

void DwFaceLight::ResetCallback(dw::Widget* data) {
    rctx_ptr->face = {};
}

void DwFaceLight::SetForEachPvCutCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button)
        dw_face_light->set_for_each_cut = button->value;
}
