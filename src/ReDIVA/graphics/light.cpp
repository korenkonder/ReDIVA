/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "light.hpp"
#include "../../KKdLib/io/path.hpp"
#include "../../CRE/light_param/light.hpp"
#include "../../CRE/light_param.hpp"
#include "../../CRE/render_context.hpp"
#include "../../CRE/stage.hpp"
#include "../../CRE/task.hpp"
#include "../pv_game/pv_game.hpp"
#include "../dw.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

extern render_context* rctx_ptr;

class LightDw : public dw::Shell {
public:
    class GAmbientSlider : public dw::SelectionAdapter {
    public:
        LightDw* light_dw;
        dw::Slider* start_point;
        dw::Slider* end_point;
        dw::Slider* coefficient;

        GAmbientSlider(LightDw* light_dw, dw::Widget* parent);
        virtual ~GAmbientSlider();

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class SyncPosButton : public dw::SelectionAdapter {
    public:
        LightDw* light_dw;
        dw::Button* sync_pos;

        SyncPosButton(LightDw* light_dw, dw::Widget* parent);
        virtual ~SyncPosButton();

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class LightAuthEnableButton : public dw::SelectionAdapter {
    public:
        LightDw* light_dw;
        dw::Button* light_auth_enable;

        LightAuthEnableButton(LightDw* light_dw, dw::Widget* parent);
        virtual ~LightAuthEnableButton();

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class CutLightEnableButton : public dw::SelectionAdapter {
    public:
        LightDw* light_dw;
        dw::Button* set_for_each_cut;

        CutLightEnableButton(LightDw* light_dw, dw::Widget* parent);
        virtual ~CutLightEnableButton();

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class IdListBox : public dw::SelectionAdapter {
    public:
        LightDw* light_dw;
        dw::ListBox* id;

        IdListBox(LightDw* light_dw, dw::Widget* parent);
        virtual ~IdListBox();

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class TypeListBox : public dw::SelectionAdapter {
    public:
        LightDw* light_dw;
        dw::ListBox* type;

        TypeListBox(LightDw* light_dw, dw::Widget* parent);
        virtual ~TypeListBox();

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class AmbientSlider : public dw::SelectionAdapter {
    public:
        LightDw* light_dw;
        dw::Slider* r;
        dw::Slider* g;
        dw::Slider* b;
        dw::Slider* a;

        AmbientSlider(LightDw* light_dw, dw::Widget* parent);
        virtual ~AmbientSlider();

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class DiffuseSlider : public dw::SelectionAdapter {
    public:
        LightDw* light_dw;
        dw::Slider* r;
        dw::Slider* g;
        dw::Slider* b;
        dw::Slider* a;

        DiffuseSlider(LightDw* light_dw, dw::Widget* parent);
        virtual ~DiffuseSlider();

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class SpecularSlider : public dw::SelectionAdapter {
    public:
        LightDw* light_dw;
        dw::Slider* r;
        dw::Slider* g;
        dw::Slider* b;
        dw::Slider* a;

        SpecularSlider(LightDw* light_dw, dw::Widget* parent);
        virtual ~SpecularSlider();

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class PositionSlider : public dw::SelectionAdapter {
    public:
        LightDw* light_dw;
        dw::Slider* x;
        dw::Slider* y;
        dw::Slider* z;
        dw::Slider* rot_y;

        PositionSlider(LightDw* light_dw, dw::Widget* parent);
        virtual ~PositionSlider();

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;

        float_t GetRotY(float_t x, float_t z);
        void SetPositionSliderParams(dw::Slider* slider, float_t value);
        void SetRotYParams(float_t value);
        void SetRotation(float_t& x, float_t& y, float_t rot_y);
    };

    class DirectionSlider : public dw::SelectionAdapter {
    public:
        LightDw* light_dw;
        dw::Slider* x;
        dw::Slider* y;
        dw::Slider* z;

        DirectionSlider(LightDw* light_dw, dw::Widget* parent);
        virtual ~DirectionSlider();

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class ExponentSlider : public dw::SelectionAdapter {
    public:
        LightDw* light_dw;
        dw::Slider* exponent;

        ExponentSlider(LightDw* light_dw, dw::Widget* parent);
        virtual ~ExponentSlider();

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class CutoffSlider : public dw::SelectionAdapter {
    public:
        LightDw* light_dw;
        dw::Slider* cutoff;

        CutoffSlider(LightDw* light_dw, dw::Widget* parent);
        virtual ~CutoffSlider();

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class AttenuationSlider : public dw::SelectionAdapter {
    public:
        LightDw* light_dw;
        dw::Slider* constant;
        dw::Slider* linear;
        dw::Slider* quadratic;

        AttenuationSlider(LightDw* light_dw, dw::Widget* parent);
        virtual ~AttenuationSlider();

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    struct File {
        LightDw* light_dw;
        dw::Button* save;
        dw::Button* load;
        dw::Button* light0_coord_init;

        File(LightDw* light_dw, dw::Widget* parent);

        static void Light0CoordinateInitializationCallback(dw::Widget* data);
        static void LoadCallback(dw::Widget* data);
        static void ReopenCallback(dw::Widget* data);
        static void SaveCallback(dw::Widget* data);
    };

    dw::ListBox* stage;
    GAmbientSlider* g_ambient_slider;
    SyncPosButton* sync_pos_button;
    LightAuthEnableButton* light_auth_enable_button;
    CutLightEnableButton* cut_light_enable_button;
    IdListBox* id_list_box;
    TypeListBox* type_list_box;
    AmbientSlider* ambient_slider;
    DiffuseSlider* diffuse_slider;
    SpecularSlider* specular_slider;
    PositionSlider* position_slider;
    DirectionSlider* direction_slider;
    ExponentSlider* exponent_slider;
    CutoffSlider* cutoff_slider;
    AttenuationSlider* attenuation_slider;
    File* file;

    LightDw();
    virtual ~LightDw() override;

    virtual void Hide() override;

    virtual void ResetData();

    void GetPosition();
    void ResetStage(int32_t stage_index);

    static void ReflectiveSurfaceClipStageCallback(dw::Widget* data);
    static void StageCallback(dw::Widget* data);
};

class LightDwTask : public app::Task {
public:
    int32_t stage_index;
    int32_t curr_stage_index;
    bool ready;
    bool set;

    LightDwTask();
    virtual ~LightDwTask() override;

    virtual bool ctrl() override;
    virtual void disp() override;
};

LightDw* light_dw;
LightDwTask light_dw_task;

light_set_id light_dw_light_set_id = LIGHT_SET_MAIN;
light_id light_dw_light_id = LIGHT_CHARA;
bool light_dw_auth_enable = true;
bool light_dw_cut_light_enable = false;
bool light_dw_sync_pos = false;

rectangle light_dw_rect = { 0.0f, vec2(256.0f, 610.0f) };

static std::string dev_ram_light_param_path = "dev_ram/light_param/";

static LightDw* light_dw_get();
static light_data* light_dw_get_light();

void light_dw_init() {
    if (!light_dw) {
        light_dw = new LightDw;
        light_dw->sub_1402F38B0();
    }
    else
        light_dw->Disp();

    if (!light_dw_task.ready)
        app::TaskWork::add_task(&light_dw_task, "LIGHT_DW_TASK");
}

LightDw::GAmbientSlider::GAmbientSlider(LightDw* light_dw, dw::Widget* parent) {
    this->light_dw = light_dw;
    start_point = 0;
    end_point = 0;
    coefficient = 0;

    dw::Composite* comp = dynamic_cast<dw::Composite*>(parent);
    if (comp) {
        light_data* light = &rctx_ptr->light_set[light_dw_light_set_id].lights[light_dw_light_id];

#if DW_TRANSLATE
        const char* chara_f_tone_curve_adjustment_text = u8"CHARA(F) Tone Curve Adjustment";
        const char* start_point_text = u8"Start Point";
        const char* end_point_text = u8"End Point";
        const char* coefficient_text = u8"Coefficient";
#else
        const char* chara_f_tone_curve_adjustment_text = u8"CHARA(F)トーンカーブ調整";
        const char* start_point_text = u8"始点";
        const char* end_point_text = u8"終点";
        const char* coefficient_text = u8"係数";
#endif

        dw::Label* label = new dw::Label(comp);
        label->SetText(chara_f_tone_curve_adjustment_text);

        light_tone_curve value;
        light->get_tone_curve(value);

        start_point = dw::Slider::Create(comp);
        start_point->SetText(start_point_text);
        start_point->format = "%4.2f";
        start_point->SetParams(value.start_point, 0.0f, 1.0f, 0.2f, 0.01f, 0.1f);
        start_point->callback_data.i64 = 0;
        start_point->AddSelectionListener(this);

        end_point = dw::Slider::Create(comp);
        end_point->SetText(end_point_text);
        end_point->format = "%4.2f";
        end_point->SetParams(value.end_point, 0.0f, 1.0f, 0.2f, 0.01f, 0.1f);
        end_point->callback_data.i64 = 1;
        end_point->AddSelectionListener(this);

        coefficient = dw::Slider::Create(comp);
        coefficient->SetText(coefficient_text);
        coefficient->format = "%4.2f";
        coefficient->SetParams(value.coefficient, 0.0f, 4.0f, 0.2f, 0.01f, 0.1f);
        coefficient->callback_data.i64 = 2;
        coefficient->AddSelectionListener(this);
    }
}

LightDw::GAmbientSlider::~GAmbientSlider() {

}

void LightDw::GAmbientSlider::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data->widget);
    if (slider) {
        light_data* light = light_dw_get_light();
        light_tone_curve value;
        light->get_tone_curve(value);
        ((float_t*)&value)[slider->callback_data.i32] = slider->GetValue();
        light->set_tone_curve(value);
    }
}

LightDw::SyncPosButton::SyncPosButton(LightDw* light_dw, dw::Widget* parent) {
    this->light_dw = light_dw;
    sync_pos = 0;

    dw::Composite* comp = dynamic_cast<dw::Composite*>(parent);
    if (comp) {
#if DW_TRANSLATE
        const char* sync_pos_text = u8"Light 0-6 Sync Position";
#else
        const char* sync_pos_text = u8"Light 0-6 座標同期";
#endif

        sync_pos = new dw::Button(comp, dw::CHECKBOX);
        sync_pos->SetText(sync_pos_text);
        sync_pos->SetValue(light_dw_sync_pos);
        sync_pos->callback_data.i64 = 0;
        sync_pos->AddSelectionListener(this);
    }
}

LightDw::SyncPosButton::~SyncPosButton() {

}

void LightDw::SyncPosButton::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
    if (button && !button->callback_data.i32)
        light_dw_sync_pos = button->value;
}

LightDw::LightAuthEnableButton::LightAuthEnableButton(LightDw* light_dw, dw::Widget* parent) {
    this->light_dw = light_dw;
    light_auth_enable = 0;

    dw::Composite* comp = dynamic_cast<dw::Composite*>(parent);
    if (comp) {
#if DW_TRANSLATE
        const char* light_auth_enable_text = u8"Light Auth Enable";
#else
        const char* light_auth_enable_text = u8"ライトオーサ有効";
#endif

        light_auth_enable = new dw::Button(comp, dw::CHECKBOX);
        light_auth_enable->SetText(light_auth_enable_text);
        light_auth_enable->SetValue(light_dw_auth_enable);
        light_auth_enable->callback_data.i64 = 0;
        light_auth_enable->AddSelectionListener(this);
    }
}

LightDw::LightAuthEnableButton::~LightAuthEnableButton() {

}

void LightDw::LightAuthEnableButton::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
    if (button && !button->callback_data.i32)
        light_dw_auth_enable = button->value;
}

LightDw::CutLightEnableButton::CutLightEnableButton(LightDw* light_dw, dw::Widget* parent) {
    this->light_dw = light_dw;
    set_for_each_cut = 0;

    dw::Composite* comp = dynamic_cast<dw::Composite*>(parent);
    if (comp) {
#if DW_TRANSLATE
        const char* set_for_each_cut_text = u8"Set for each PV cut";
#else
        const char* set_for_each_cut_text = u8"PVのカットごとに設定";
#endif

        set_for_each_cut = new dw::Button(comp, dw::CHECKBOX);
        set_for_each_cut->SetText(set_for_each_cut_text);
        set_for_each_cut->SetValue(light_dw_cut_light_enable);
        set_for_each_cut->callback_data.i64 = 0;
        set_for_each_cut->AddSelectionListener(this);
    }
}

LightDw::CutLightEnableButton::~CutLightEnableButton() {

}

void LightDw::CutLightEnableButton::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
    if (button && !button->callback_data.i32)
        light_dw_cut_light_enable = button->value;
}

LightDw::IdListBox::IdListBox(LightDw* light_dw, dw::Widget* parent) {
    this->light_dw = light_dw;
    id = 0;

    dw::Composite* comp = dynamic_cast<dw::Composite*>(parent);
    if (comp) {
        dw::Label* label = new dw::Label(comp);
        label->SetText("ID");

        id = new dw::ListBox(comp, dw::MULTISELECT);

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

        for (const char*& i : light_id_labels)
            id->AddItem(i);

        id->SetItemIndex(light_dw_light_id);
        id->AddSelectionListener(this);
    }
}

LightDw::IdListBox::~IdListBox() {

}

void LightDw::IdListBox::Callback(dw::SelectionListener::CallbackData* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data->widget);
    if (list_box) {
        light_dw_light_id = (light_id)list_box->list->selected_item;
        light_dw->ResetData();
    }
}

LightDw::TypeListBox::TypeListBox(LightDw* light_dw, dw::Widget* parent) {
    this->light_dw = light_dw;
    type = 0;

    dw::Composite* comp = dynamic_cast<dw::Composite*>(parent);
    if (comp) {
        light_data* light = light_dw_get_light();

        dw::Label* label = new dw::Label(comp);
        label->SetText("TYPE");

        type = new dw::ListBox(comp, dw::MULTISELECT);

        const char* light_type_labels[] = {
            "OFF",
            "PARALLEL",
            "POINT",
            "SPOT",
        };

        for (const char*& i : light_type_labels)
            type->AddItem(i);

        type->SetItemIndex(light->get_type());
        type->AddSelectionListener(this);
    }
}

LightDw::TypeListBox::~TypeListBox() {

}

void LightDw::TypeListBox::Callback(dw::SelectionListener::CallbackData* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data->widget);
    if (list_box) {
        light_data* light = light_dw_get_light();
        light->set_type((light_type)list_box->list->selected_item);
        light_dw->ResetData();
    }
}

LightDw::AmbientSlider::AmbientSlider(LightDw* light_dw, dw::Widget* parent) {
    this->light_dw = light_dw;
    r = 0;
    g = 0;
    b = 0;
    a = 0;

    dw::Composite* comp = dynamic_cast<dw::Composite*>(parent);
    if (comp) {
        light_data* light = &rctx_ptr->light_set[light_dw_light_set_id].lights[light_dw_light_id];

        dw::Label* label = new dw::Label(comp);
        label->SetText("AMBIENT");

        vec4 value;
        light->get_ambient(value);

        r = dw::Slider::Create(comp);
        r->SetText("R");
        r->format = "%4.3f";
        r->SetParams(value.x, 0.0f, 2.0f, 0.2f, 0.01f, 0.1f);
        r->callback_data.i64 = 0;
        r->AddSelectionListener(this);

        g = dw::Slider::Create(comp);
        g->SetText("G");
        g->format = "%4.3f";
        g->SetParams(value.y, 0.0f, 2.0f, 0.2f, 0.01f, 0.1f);
        g->callback_data.i64 = 1;
        g->AddSelectionListener(this);

        b = dw::Slider::Create(comp);
        b->SetText("B");
        b->format = "%4.3f";
        b->SetParams(value.z, 0.0f, 2.0f, 0.2f, 0.01f, 0.1f);
        b->callback_data.i64 = 2;
        b->AddSelectionListener(this);
    }
}

LightDw::AmbientSlider::~AmbientSlider() {

}

void LightDw::AmbientSlider::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data->widget);
    if (slider) {
        light_data* light = light_dw_get_light();
        vec4 value;
        light->get_ambient(value);
        ((float_t*)&value)[slider->callback_data.i32] = slider->GetValue();
        light->set_ambient(value);
    }
}

LightDw::DiffuseSlider::DiffuseSlider(LightDw* light_dw, dw::Widget* parent) {
    this->light_dw = light_dw;
    r = 0;
    g = 0;
    b = 0;
    a = 0;

    dw::Composite* comp = dynamic_cast<dw::Composite*>(parent);
    if (comp) {
        light_data* light = &rctx_ptr->light_set[light_dw_light_set_id].lights[light_dw_light_id];

        dw::Label* label = new dw::Label(comp);
        label->SetText("DIFFUSE");

        vec4 value;
        light->get_diffuse(value);

        r = dw::Slider::Create(comp);
        r->SetText("R");
        r->format = "%4.3f";
        r->SetParams(value.x, 0.0f, 2.0f, 0.2f, 0.01f, 0.1f);
        r->callback_data.i64 = 0;
        r->AddSelectionListener(this);

        g = dw::Slider::Create(comp);
        g->SetText("G");
        g->format = "%4.3f";
        g->SetParams(value.y, 0.0f, 2.0f, 0.2f, 0.01f, 0.1f);
        g->callback_data.i64 = 1;
        g->AddSelectionListener(this);

        b = dw::Slider::Create(comp);
        b->SetText("B");
        b->format = "%4.3f";
        b->SetParams(value.z, 0.0f, 2.0f, 0.2f, 0.01f, 0.1f);
        b->callback_data.i64 = 2;
        b->AddSelectionListener(this);
    }
}

LightDw::DiffuseSlider::~DiffuseSlider() {

}

void LightDw::DiffuseSlider::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data->widget);
    if (slider) {
        light_data* light = light_dw_get_light();
        vec4 value;
        light->get_diffuse(value);
        ((float_t*)&value)[slider->callback_data.i32] = slider->GetValue();
        light->set_diffuse(value);
    }
}

LightDw::SpecularSlider::SpecularSlider(LightDw* light_dw, dw::Widget* parent) {
    this->light_dw = light_dw;
    r = 0;
    g = 0;
    b = 0;
    a = 0;

    dw::Composite* comp = dynamic_cast<dw::Composite*>(parent);
    if (comp) {
        light_data* light = &rctx_ptr->light_set[light_dw_light_set_id].lights[light_dw_light_id];

        dw::Label* label = new dw::Label(comp);
        label->SetText("SPECULAR");

        vec4 value;
        light->get_specular(value);

        r = dw::Slider::Create(comp);
        r->SetText("R");
        r->format = "%4.3f";
        r->SetParams(value.x, 0.0f, 2.0f, 0.2f, 0.01f, 0.1f);
        r->callback_data.i64 = 0;
        r->AddSelectionListener(this);

        g = dw::Slider::Create(comp);
        g->SetText("G");
        g->format = "%4.3f";
        g->SetParams(value.y, 0.0f, 2.0f, 0.2f, 0.01f, 0.1f);
        g->callback_data.i64 = 1;
        g->AddSelectionListener(this);

        b = dw::Slider::Create(comp);
        b->SetText("B");
        b->format = "%4.3f";
        b->SetParams(value.z, 0.0f, 2.0f, 0.2f, 0.01f, 0.1f);
        b->callback_data.i64 = 2;
        b->AddSelectionListener(this);

        a = dw::Slider::Create(comp);
        a->SetText("A");
        a->format = "%4.3f";
        a->SetParams(value.w, 0.0f, 2.0f, 0.2f, 0.01f, 0.1f);
        a->callback_data.i64 = 3;
        a->AddSelectionListener(this);
    }
}

LightDw::SpecularSlider::~SpecularSlider() {

}

void LightDw::SpecularSlider::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data->widget);
    if (slider) {
        light_data* light = light_dw_get_light();
        vec4 value;
        light->get_specular(value);
        ((float_t*)&value)[slider->callback_data.i32] = slider->GetValue();
        light->set_specular(value);
    }
}

LightDw::PositionSlider::PositionSlider(LightDw* light_dw, dw::Widget* parent) {
    this->light_dw = light_dw;
    x = 0;
    y = 0;
    z = 0;
    rot_y = 0;

    dw::Composite* comp = dynamic_cast<dw::Composite*>(parent);
    if (comp) {
        light_data* light = &rctx_ptr->light_set[light_dw_light_set_id].lights[light_dw_light_id];

        dw::Label* label = new dw::Label(comp);
        label->SetText("POSITION");

        vec3 value;
        light->get_position(value);

        x = dw::Slider::Create(comp, (dw::Flags)(dw::FLAG_800
            | dw::HORIZONTAL), 0.0f, 0.0f, 160.0f, 20.0f);
        x->SetText("X");
        x->format = "%6.2f";
        SetPositionSliderParams(x, value.x);
        x->callback_data.i64 = 0;
        x->AddSelectionListener(this);

        y = dw::Slider::Create(comp, (dw::Flags)(dw::FLAG_800
            | dw::HORIZONTAL), 0.0f, 0.0f, 160.0f, 20.0f);
        y->SetText("Y");
        y->format = "%6.2f";
        SetPositionSliderParams(y, value.y);
        y->callback_data.i64 = 1;
        y->AddSelectionListener(this);

        z = dw::Slider::Create(comp, (dw::Flags)(dw::FLAG_800
            | dw::HORIZONTAL), 0.0f, 0.0f, 160.0f, 20.0f);
        z->SetText("Z");
        z->format = "%6.2f";
        SetPositionSliderParams(z, value.z);
        z->callback_data.i64 = 2;
        z->AddSelectionListener(this);

        rot_y = dw::Slider::Create(comp);
        rot_y->SetText("ROTY");
        rot_y->format = "%2.2f";
        SetRotYParams(GetRotY(value.x, value.z));
        rot_y->callback_data.i64 = 3;
        rot_y->AddSelectionListener(this);
    }
}

LightDw::PositionSlider::~PositionSlider() {

}

void LightDw::PositionSlider::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data->widget);
    if (slider) {
        light_data* light = light_dw_get_light();

        float_t pos_rot[4];
        light->get_position(*(vec3*)pos_rot);
        pos_rot[slider->callback_data.i32] = slider->GetValue();
        if (slider->callback_data.i32 == 3) {
            SetRotYParams(slider->GetValue());
            SetRotation(pos_rot[0], pos_rot[2], pos_rot[3]);
            light->set_position(*(vec3*)pos_rot);
            SetPositionSliderParams(x, pos_rot[0]);
            SetPositionSliderParams(z, pos_rot[2]);
        }
        else {
            light->set_position(*(vec3*)pos_rot);
            SetPositionSliderParams(slider, pos_rot[slider->callback_data.i32]);
            SetRotYParams(GetRotY(pos_rot[0], pos_rot[2]));
        }

        if (light_dw_sync_pos) {
            light_set* set = &rctx_ptr->light_set[light_dw_light_set_id];
            switch (light_dw_light_id) {
            case LIGHT_CHARA:
                set->lights[LIGHT_TONE_CURVE].set_position(*(vec3*)pos_rot);
                break;
            case LIGHT_TONE_CURVE:
                set->lights[LIGHT_CHARA].set_position(*(vec3*)pos_rot);
                break;
            }
        }
    }
}

float_t LightDw::PositionSlider::GetRotY(float_t x, float_t z) {
    if (fabsf(x) > 0.000001f || fabsf(z) > 0.000001f)
        return atan2f(z, x) * RAD_TO_DEG_FLOAT;
    return 0.0f;
}

void LightDw::PositionSlider::SetPositionSliderParams(dw::Slider* slider, float_t value) {
    float_t scale;
    if (fabsf(value) < 150.0f)
        scale = 1.0f;
    else if (fabsf(value) < 1900.0f)
        scale = 10.0f;
    else
        scale = 50.0f;
    slider->SetParams(value, 1.0f - scale * 200.0f,
        scale * 200.0f - 1.0f, scale * 40.0f, scale * 0.02f, scale);
}

void LightDw::PositionSlider::SetRotYParams(float_t value) {
    rot_y->SetParams(value, -180.0f, 180.0f, 0.2f, 0.01f, 0.1f);
}

void LightDw::PositionSlider::SetRotation(float_t& x, float_t& z, float_t rot_y) {
    rot_y *= DEG_TO_RAD_FLOAT;
    float_t length = sqrtf(x * x + z * z);
    x = cosf(rot_y) * length;
    z = sinf(rot_y) * length;
}

LightDw::DirectionSlider::DirectionSlider(LightDw* light_dw, dw::Widget* parent) {
    this->light_dw = light_dw;
    x = 0;
    y = 0;
    z = 0;

    dw::Composite* comp = dynamic_cast<dw::Composite*>(parent);
    if (comp) {
        light_data* light = &rctx_ptr->light_set[light_dw_light_set_id].lights[light_dw_light_id];

        dw::Label* label = new dw::Label(comp);
        label->SetText("SPOT DIRECTION");

        vec3 value;
        light->get_spot_direction(value);

        x = dw::Slider::Create(comp);
        x->SetText("R");
        x->format = "%5.2f";
        x->SetParams(value.x, -1.0f, 1.0f, 0.2f, 0.02f, 0.1f);
        x->callback_data.i64 = 0;
        x->AddSelectionListener(this);

        y = dw::Slider::Create(comp);
        y->SetText("G");
        y->format = "%5.2f";
        y->SetParams(value.y, -1.0f, 1.0f, 0.2f, 0.02f, 0.1f);
        y->callback_data.i64 = 1;
        y->AddSelectionListener(this);

        z = dw::Slider::Create(comp);
        z->SetText("B");
        z->format = "%5.2f";
        z->SetParams(value.z, -1.0f, 1.0f, 0.2f, 0.02f, 0.1f);
        z->callback_data.i64 = 2;
        z->AddSelectionListener(this);
    }
}

LightDw::DirectionSlider::~DirectionSlider() {

}

void LightDw::DirectionSlider::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data->widget);
    if (slider) {
        light_data* light = light_dw_get_light();
        vec3 value;
        light->get_spot_direction(value);
        ((float_t*)&value)[slider->callback_data.i32] = slider->GetValue();
        light->set_spot_direction(value);
    }
}

LightDw::ExponentSlider::ExponentSlider(LightDw* light_dw, dw::Widget* parent) {
    this->light_dw = light_dw;
    exponent = 0;

    dw::Composite* comp = dynamic_cast<dw::Composite*>(parent);
    if (comp) {
        light_data* light = &rctx_ptr->light_set[light_dw_light_set_id].lights[light_dw_light_id];

        dw::Label* label = new dw::Label(comp);
        label->SetText("SPOT EXPONENT");

        float_t value = light->get_spot_exponent();

        exponent = dw::Slider::Create(comp);
        exponent->SetText("EXPONENT");
        exponent->format = "%8.4f";
        exponent->SetParams(value, 0.0f, 128.0f, 12.8f, 1.0f, 10.0f);
        exponent->AddSelectionListener(this);
    }
}

LightDw::ExponentSlider::~ExponentSlider() {

}

void LightDw::ExponentSlider::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data->widget);
    if (slider) {
        light_data* light = light_dw_get_light();
        light->set_spot_exponent(slider->GetValue());
    }
}

LightDw::CutoffSlider::CutoffSlider(LightDw* light_dw, dw::Widget* parent) {
    this->light_dw = light_dw;
    cutoff = 0;

    dw::Composite* comp = dynamic_cast<dw::Composite*>(parent);
    if (comp) {
        light_data* light = &rctx_ptr->light_set[light_dw_light_set_id].lights[light_dw_light_id];

        dw::Label* label = new dw::Label(comp);
        label->SetText("SPOT CUTOFF");

        float_t value = light->get_spot_cutoff();

        cutoff = dw::Slider::Create(comp);
        cutoff->SetText("CUTOFF");
        cutoff->format = "%8.4f";
        cutoff->SetParams(value, 0.0f, 9.0f, 9.0f, 1.0f, 5.0f);
        cutoff->AddSelectionListener(this);
    }
}

LightDw::CutoffSlider::~CutoffSlider() {

}

void LightDw::CutoffSlider::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data->widget);
    if (slider) {
        light_data* light = light_dw_get_light();
        light->set_spot_cutoff(slider->GetValue());
    }
}

LightDw::AttenuationSlider::AttenuationSlider(LightDw* light_dw, dw::Widget* parent) {
    this->light_dw = light_dw;
    constant = 0;
    linear = 0;
    quadratic = 0;

    dw::Composite* comp = dynamic_cast<dw::Composite*>(parent);
    if (comp) {
        light_data* light = &rctx_ptr->light_set[light_dw_light_set_id].lights[light_dw_light_id];

        dw::Label* label = new dw::Label(comp);
        label->SetText("ATTENUATION");

        light_attenuation value;
        light->get_attenuation(value);

        constant = dw::Slider::Create(comp);
        constant->SetText("CONSTANT");
        constant->format = "%8.4f";
        constant->SetParams(value.constant, 0.0f, 1.0f, 0.1f, 0.01f, 0.1f);
        constant->callback_data.i64 = 0;
        constant->AddSelectionListener(this);

        linear = dw::Slider::Create(comp);
        linear->SetText("LINEAR");
        linear->format = "%8.4f";
        linear->SetParams(value.linear, 0.0f, 1.0f, 0.1f, 0.01f, 0.1f);
        linear->callback_data.i64 = 1;
        linear->AddSelectionListener(this);

        quadratic = dw::Slider::Create(comp);
        quadratic->SetText("QUADRATIC");
        quadratic->format = "%8.4f";
        quadratic->SetParams(value.quadratic, 0.0f, 1.0f, 0.1f, 0.01f, 0.1f);
        quadratic->callback_data.i64 = 2;
        quadratic->AddSelectionListener(this);
    }
}

LightDw::AttenuationSlider::~AttenuationSlider() {

}

void LightDw::AttenuationSlider::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data->widget);
    if (slider) {
        light_data* light = light_dw_get_light();
        light_attenuation value;
        light->get_attenuation(value);
        ((float_t*)&value)[slider->callback_data.i32] = slider->GetValue();
        light->set_attenuation(value);
    }
}

LightDw::File::File(LightDw* light_dw, dw::Widget* parent) {
    this->light_dw = 0;
    save = 0;
    load = 0;
    light0_coord_init = 0;

    dw::Composite* parent_comp = dynamic_cast<dw::Composite*>(parent);
    if (!parent_comp)
        return;

    dw::Composite* comp = new dw::Composite(parent_comp);
    comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    save = new dw::Button(comp, dw::FLAG_8);
    save->SetText("SAVE");
    save->callback_data.v64 = light_dw;
    save->callback = LightDw::File::SaveCallback;

    load = new dw::Button(comp, dw::FLAG_8);
    load->SetText("LOAD");
    load->callback_data.v64 = light_dw;
    load->callback = LightDw::File::LoadCallback;

#if DW_TRANSLATE
    const char* light0_coord_init_text = u8"Light0 Coordinate Initialization";
#else
    const char* light0_coord_init_text = u8"Light0座標初期化";
#endif

    light0_coord_init = new dw::Button(comp, dw::FLAG_8);
    light0_coord_init->SetText(light0_coord_init_text);
    light0_coord_init->callback_data.v64 = light_dw;
    light0_coord_init->callback = LightDw::File::Light0CoordinateInitializationCallback;

    dw::Button* reopen = new dw::Button(parent_comp, dw::FLAG_8);
    reopen->SetText("Reopen");
    reopen->callback_data.v64 = light_dw;
    reopen->callback = LightDw::File::ReopenCallback;
}

void LightDw::File::Light0CoordinateInitializationCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button) {
        LightDw* light_dw = (LightDw*)button->callback_data.v64;
        light_set* set = &rctx_ptr->light_set[light_dw_light_set_id];

        vec4 value;
        set->lights[LIGHT_CHARA].get_ibl_direction(value);
        set->lights[LIGHT_CHARA].set_position(value);

        if (light_dw_sync_pos)
            set->lights[LIGHT_TONE_CURVE].set_position(value);

        light_dw->GetPosition();
    }
}

void LightDw::File::LoadCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (!button)
        return;

    LightDw* light_dw = (LightDw*)button->callback_data.v64;

    std::string path(dev_ram_light_param_path);
    if (light_dw_cut_light_enable) {
        pv_game_get();

        char buf[0x06];
        sprintf_s(buf, sizeof(buf), "pv%03d", 0);
        path.append(buf);
        path_create_directory(path.c_str());

        char cut_buf[0x04];
        sprintf_s(cut_buf, sizeof(cut_buf), "%03d", 0);

        path.append("/");
        path.append("light_");
        path.append(buf);
        path.append("_c");
        path.append(cut_buf);
    }
    else {
        path.append(light_param_data_storage_data_get_name());

        path.append("/");
        path.append("light_");
        path.append(light_param_data_storage_data_get_name());
    }

    path.append(".txt");
    if (path.find("pv000") != -1)
        return;

    std::ifstream ifs(path);
    if (!ifs.is_open())
        return;

    light_set_id set_id = LIGHT_SET_MAIN;
    light_id id = LIGHT_CHARA;

    do {
        char buf[0x200];
        ifs.get(buf, sizeof(buf), '\n');

        std::stringstream ss(buf);

        std::string str;
        ss >> str;

        if (!str.compare(0, 11, "group_start")) {
            int32_t value;
            ss >> value;
            set_id = (light_set_id)value;
        }

        if (!str.compare(0, 8, "id_start")) {
            int32_t value;
            ss >> value;
            id = (light_id)value;
        }

        if (!str.compare(0, 4, "type")) {
            int32_t value;
            ss >> value;
            rctx_ptr->light_set[set_id].lights[id].set_type((light_type)value);
        }

        if (!str.compare(0, 7, "ambient")) {
            vec4 ambient;
            ss >> ambient.x >> ambient.y >> ambient.z >> ambient.w;
            rctx_ptr->light_set[set_id].lights[id].set_ambient(ambient);
        }

        if (!str.compare(0, 7, "diffuse")) {
            vec4 diffuse;
            ss >> diffuse.x >> diffuse.y >> diffuse.z >> diffuse.w;
            rctx_ptr->light_set[set_id].lights[id].set_diffuse(diffuse);
        }

        if (!str.compare(0, 8, "specular")) {
            vec4 specular;
            ss >> specular.x >> specular.y >> specular.z >> specular.w;
            rctx_ptr->light_set[set_id].lights[id].set_specular(specular);
        }

        if (!str.compare(0, 8, "position")) {
            vec4 position;
            ss >> position.x >> position.y >> position.z >> position.w;
            rctx_ptr->light_set[set_id].lights[id].set_position(*(vec3*)&position);
        }

        if (!str.compare(0, 14, "spot_direction")) {
            vec3 spot_direction;
            ss >> spot_direction.x >> spot_direction.y >> spot_direction.z;
            rctx_ptr->light_set[set_id].lights[id].set_spot_direction(spot_direction);
        }

        if (!str.compare(0, 13, "spot_exponent")) {
            float_t spot_exponent;
            ss >> spot_exponent;
            rctx_ptr->light_set[set_id].lights[id].set_spot_exponent(spot_exponent);
        }

        if (!str.compare(0, 11, "spot_cutoff")) {
            float_t spot_cutoff;
            ss >> spot_cutoff;
            rctx_ptr->light_set[set_id].lights[id].set_spot_cutoff(spot_cutoff);
        }

        if (!str.compare(0, 11, "attenuation")) {
            light_attenuation attenuation;
            ss >> attenuation.constant >> attenuation.linear >> attenuation.quadratic;
            rctx_ptr->light_set[set_id].lights[id].set_attenuation(attenuation);
        }

        if (!str.compare(0, 9, "clipplane")) {
            light_clip_plane clip_plane;
            ss >> clip_plane.data[0] >> clip_plane.data[1] >> clip_plane.data[2] >> clip_plane.data[3];
            rctx_ptr->light_set[set_id].lights[id].set_clip_plane(clip_plane);
        }

        if (!str.compare(0, 9, "tonecurve")) {
            light_tone_curve tone_curve;
            ss >> tone_curve.start_point >> tone_curve.end_point >> tone_curve.coefficient;
            rctx_ptr->light_set[set_id].lights[id].set_tone_curve(tone_curve);
        }
    } while (!ifs.eof());
    light_dw->ResetData();
}

void LightDw::File::ReopenCallback(dw::Widget* data) {
    //DwReopen::ReopenCallback(0);
}

void LightDw::File::SaveCallback(dw::Widget* data) {
    std::string path(dev_ram_light_param_path);
    if (light_dw_cut_light_enable) {
        pv_game_get();

        char buf[0x06];
        sprintf_s(buf, sizeof(buf), "pv%03d", 0);
        path.append(buf);
        path_create_directory(path.c_str());

        char cut_buf[0x04];
        sprintf_s(cut_buf, sizeof(cut_buf), "%03d", 0);

        path.append("/");
        path.append("light_");
        path.append(buf);
        path.append("_c");
        path.append(cut_buf);
    }
    else {
        path.append(light_param_data_storage_data_get_name());
        path_create_directory(path.c_str());

        path.append("/");
        path.append("light_");
        path.append(light_param_data_storage_data_get_name());
    }

    path.append(".txt");
    if (path.find("pv000") != -1)
        return;

    std::ofstream ofs(path, std::ios::binary);
    if (!ofs.is_open())
        return;

    for (int32_t i = LIGHT_SET_MAIN; i < LIGHT_SET_MAX; i++) {
        light_set* set = &rctx_ptr->light_set[LIGHT_SET_MAIN];
        ofs << "group_start " << i << '\n' << std::flush;
        for (int32_t j = LIGHT_CHARA; j < LIGHT_MAX; j++) {
            light_data* light = &set->lights[j];
            ofs << "id_start " << j << '\n' << std::flush;

            light_type type = light->get_type();

            vec4 ambient;
            vec4 diffuse;
            vec4 specular;
            vec4 position;
            light->get_ambient(ambient);
            light->get_diffuse(diffuse);
            light->get_specular(specular);
            light->get_position(*(vec3*)&position);
            position.w = 0.0f;

            ofs << "type " << (int32_t)type << '\n' << std::flush;
            ofs << std::showpoint << "ambient " << ambient.x << " " << ambient.y
                << " " << ambient.z << " " << ambient.w << '\n' << std::flush;
            ofs << std::showpoint << "diffuse " << diffuse.x << " " << diffuse.y
                << " " << diffuse.z << " " << diffuse.w << '\n' << std::flush;
            ofs << std::showpoint << "specular " << specular.x << " " << specular.y
                << " " << specular.z << " " << specular.w << '\n' << std::flush;
            ofs << std::showpoint << "position " << position.x << " " << position.y
                << " " << position.z << " " << position.w << '\n' << std::flush;
            if (type == LIGHT_SPOT) {
                vec3 spot_direction;
                light->get_spot_direction(spot_direction);
                ofs << std::showpoint << "spot_direction " << spot_direction.x
                    << " " << spot_direction.y << " " << spot_direction.z << '\n' << std::flush;

                float_t spot_exponent = light->get_spot_exponent();
                ofs << std::showpoint << "spot_exponent " << spot_exponent << '\n' << std::flush;

                float_t spot_cutoff = light->get_spot_cutoff();
                ofs << std::showpoint << "spot_cutoff " << spot_cutoff << '\n' << std::flush;

                light_attenuation attenuation;
                light->get_attenuation(attenuation);
                ofs << std::showpoint << "attenuation " << attenuation.constant
                    << " " << attenuation.linear << " " << attenuation.quadratic << '\n' << std::flush;
            }

            if (j == LIGHT_REFLECT) {
                light_clip_plane clip_plane;
                light->get_clip_plane(clip_plane);
                ofs << "clipplane " << clip_plane.data[0] << " " << clip_plane.data[1]
                    << " " << clip_plane.data[2] << " " << clip_plane.data[3] << '\n' << std::flush;
            }
            else if (j == LIGHT_TONE_CURVE) {
                vec3 tone_curve;
                ofs << std::showpoint << "tonecurve " << tone_curve.x
                    << " " << tone_curve.y << " " << tone_curve.z << '\n' << std::flush;
            }
            ofs << "id_end " << j << '\n' << std::flush;
        }
        ofs << "group_end " << i << '\n' << std::flush;
    }
    ofs << "EOF" << '\n' << std::flush;
}

LightDw::LightDw() {
    data_struct* aft_data = &data_list[DATA_AFT];
    stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

    SetText("LIGHT");

    dw::Composite* comp = new dw::Composite(this, dw::VERTICAL);

    file = new LightDw::File(this, comp);
    cut_light_enable_button = new LightDw::CutLightEnableButton(this, comp);

    dw::Label* stage_label = new dw::Label(comp);
    stage_label->SetText("STAGE (Light/Fog/PP)");

    stage = new dw::ListBox(comp);

    for (const stage_data& i : aft_stage_data->stage_data)
        stage->list->AddItem(i.name);

    if (!light_dw_task.set) {
        light_dw_task.curr_stage_index = max_def(task_stage_get_current_stage_index(), 0);
        light_dw_task.stage_index = light_dw_task.curr_stage_index;
    }
    light_dw_task.set = false;

    stage->SetItemIndex(light_dw_task.curr_stage_index);
    stage->SetMaxItems(30);
    stage->AddSelectionListener(new dw::SelectionListenerOnHook(LightDw::StageCallback));

    id_list_box = new LightDw::IdListBox(this, comp);

    g_ambient_slider = 0;
    sync_pos_button = 0;
    light_auth_enable_button = 0;

    if (light_dw_light_id == LIGHT_CHARA || light_dw_light_id == LIGHT_TONE_CURVE) {
        g_ambient_slider = new LightDw::GAmbientSlider(this, comp);
        sync_pos_button = new LightDw::SyncPosButton(this, comp);
        light_auth_enable_button = new LightDw::LightAuthEnableButton(this, comp);
    }

    type_list_box = new LightDw::TypeListBox(this, comp);

    ambient_slider = 0;
    diffuse_slider = 0;
    specular_slider = 0;
    position_slider = 0;
    direction_slider = 0;
    exponent_slider = 0;
    cutoff_slider = 0;
    attenuation_slider = 0;

    switch (type_list_box->type->list->selected_item) {
    case LIGHT_PARALLEL:
        ambient_slider = new LightDw::AmbientSlider(this, comp);
        diffuse_slider = new LightDw::DiffuseSlider(this, comp);
        specular_slider = new LightDw::SpecularSlider(this, comp);
        position_slider = new LightDw::PositionSlider(this, comp);
        break;
    case LIGHT_POINT:
        ambient_slider = new LightDw::AmbientSlider(this, comp);
        diffuse_slider = new LightDw::DiffuseSlider(this, comp);
        specular_slider = new LightDw::SpecularSlider(this, comp);
        position_slider = new LightDw::PositionSlider(this, comp);
        break;
    case LIGHT_SPOT:
        ambient_slider = new LightDw::AmbientSlider(this, comp);
        diffuse_slider = new LightDw::DiffuseSlider(this, comp);
        specular_slider = new LightDw::SpecularSlider(this, comp);
        position_slider = new LightDw::PositionSlider(this, comp);
        direction_slider = new LightDw::DirectionSlider(this, comp);

        if (light_dw_light_id == LIGHT_REFLECT) {
            light_clip_plane clip_plane;
            light_dw_get_light()->get_clip_plane(clip_plane);

#if DW_TRANSLATE
            const char* reflective_surface_clip_stage_text = u8"Reflective Surface Clip (STAGE)";
#else
            const char* reflective_surface_clip_stage_text = u8"反射面クリップ(STAGE)";
#endif

            dw::Button* reflective_surface_clip_stage = new dw::Button(comp, dw::CHECKBOX);
            reflective_surface_clip_stage->SetText(reflective_surface_clip_stage_text);
            reflective_surface_clip_stage->SetValue(!!clip_plane.data[1]);
            reflective_surface_clip_stage->callback = LightDw::ReflectiveSurfaceClipStageCallback;
        }

        exponent_slider = new LightDw::ExponentSlider(this, comp);
        cutoff_slider = new LightDw::CutoffSlider(this, comp);
        attenuation_slider = new LightDw::AttenuationSlider(this, comp);
        break;
    }

    comp->UpdateLayout();

    float_t size_y = comp->rect.size.y;
    rect.pos = light_dw_rect.pos;
    SetSize(light_dw_rect.size);

    comp->v_bar->SetMax(size_y - comp->rect.size.y);
}

LightDw::~LightDw() {

}

void LightDw::Hide() {
    light_dw_rect = parent_shell->rect;
    dw::Shell::Hide();
    light_dw = 0;
    if (!light_dw_task.ready)
        light_dw_task.del();
}

void LightDw::ResetData() {
    ResetStage(-1);
}

void LightDw::GetPosition() {
    if (position_slider) {
        light_data* light = light_dw_get_light();

        vec3 value;
        light->get_position(value);

        position_slider->x->SetValue(value.x);
        position_slider->y->SetValue(value.y);
        position_slider->z->SetValue(value.z);
        position_slider->SetRotYParams(position_slider->GetRotY(value.x, value.z));
    }
}

void LightDw::ResetStage(int32_t stage_index) {
    light_dw_task.ready = true;
    if (stage_index != -1) {
        light_dw_task.set = true;
        light_dw_task.stage_index = stage_index;
        light_dw_task.curr_stage_index = stage_index;
    }

    Hide();

    light_dw_init();
    light_dw_task.ready = false;
}

void LightDw::ReflectiveSurfaceClipStageCallback(dw::Widget* data) {
    if (!light_dw_get())
        return;

    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button) {
        light_data* light = light_dw_get_light();
        light_clip_plane clip_plane;
        light->get_clip_plane(clip_plane);
        clip_plane.data[1] = button->value;
        light->set_clip_plane(clip_plane);
    }
}

void LightDw::StageCallback(dw::Widget* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box) {
        data_struct* aft_data = &data_list[DATA_AFT];
        stage_database* aft_stage_data = &aft_data->data_ft.stage_data;

        std::string name = list_box->GetSelectedItemStr();
        light_dw_task.stage_index = aft_stage_data->get_stage_index(name.c_str());

        if (light_dw_task.curr_stage_index != light_dw_task.stage_index)
            light_param_data_storage_data_load_stage(light_dw_task.stage_index);
    }
}

LightDwTask::LightDwTask() : ready(), set() {
    stage_index = -1;
    curr_stage_index = -1;
}

LightDwTask::~LightDwTask() {

}

bool LightDwTask::ctrl() {
    if (stage_index != curr_stage_index && !light_param_data_storage_data_load_file()) {
        set = true;
        curr_stage_index = stage_index;
        light_param_data_storage_data_set_stage(stage_index);
    }
    return false;
}

void LightDwTask::disp() {

}

static LightDw* light_dw_get() {
    return light_dw;
}

static light_data* light_dw_get_light() {
    return &rctx_ptr->light_set[light_dw_light_set_id].lights[light_dw_light_id];
}
