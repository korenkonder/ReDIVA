/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "fog.hpp"
#include "../../CRE/light_param/fog.hpp"
#include "../../CRE/light_param.hpp"
#include "../../CRE/render_context.hpp"

extern render_context* rctx_ptr;

class FogDw : public dw::Shell {
public:
    class GroupListBox : public dw::SelectionAdapter {
    public:
        FogDw* fog_dw;
        dw::ListBox* group;

        GroupListBox(FogDw* fog_dw, dw::Widget* parent);
        virtual ~GroupListBox();

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class TypeListBox : public dw::SelectionAdapter {
    public:
        FogDw* fog_dw;
        dw::ListBox* type;

        TypeListBox(FogDw* fog_dw, dw::Widget* parent);
        virtual ~TypeListBox();

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class DensitySlider : public dw::SelectionAdapter {
    public:
        FogDw* fog_dw;
        dw::Slider* density;

        DensitySlider(FogDw* fog_dw, dw::Widget* parent);
        virtual ~DensitySlider();

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class LinearSlider : public dw::SelectionAdapter {
    public:
        FogDw* fog_dw;
        dw::Slider* start;
        dw::Slider* end;

        LinearSlider(FogDw* fog_dw, dw::Widget* parent);
        virtual ~LinearSlider();

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class ColorSlider : public dw::SelectionAdapter {
    public:
        FogDw* fog_dw;
        dw::Slider* r;
        dw::Slider* g;
        dw::Slider* b;

        ColorSlider(FogDw* fog_dw, dw::Widget* parent);
        virtual ~ColorSlider();

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    class IoButton : public dw::SelectionAdapter {
    public:
        FogDw* fog_dw;
        int64_t field_10;
        dw::Button* save;
        dw::Button* load;

        IoButton(FogDw* fog_dw, dw::Widget* parent);
        virtual ~IoButton();

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;

        static void LoadCallback(dw::Widget* data);
        static void SaveCallback(dw::Widget* data);
    };

    class CutLightEnableButton : public dw::SelectionAdapter {
    public:
        FogDw* fog_dw;
        dw::Button* set_for_each_cut;

        CutLightEnableButton(FogDw* fog_dw, dw::Widget* parent);
        virtual ~CutLightEnableButton();

        virtual void Callback(dw::SelectionListener::CallbackData* data) override;
    };

    GroupListBox* group_list_box;
    TypeListBox* type_list_box;
    DensitySlider* density;
    LinearSlider* linear_slider;
    ColorSlider* color_slider;
    IoButton* io_button;
    CutLightEnableButton* cut_light_enable_button;

    FogDw();
    virtual ~FogDw() override;

    virtual void Hide() override;

    virtual void ResetData();
};

FogDw* fog_dw ;

fog_id fog_dw_id = FOG_DEPTH;
bool fog_dw_set_for_each_cut = false;

rectangle fog_dw_rect = { 0.0f, vec2(300.0f, 320.0f) };

static FogDw* fog_dw_get();
static fog* fog_dw_get_fog();

void fog_dw_init() {
    if (!fog_dw) {
        fog_dw = new FogDw;
        fog_dw->sub_1402F38B0();
    }
    else
        fog_dw->Disp();
}

FogDw::GroupListBox::GroupListBox(FogDw* fog_dw, dw::Widget* parent) {
    this->fog_dw = fog_dw;
    group = 0;

    dw::Composite* comp = dynamic_cast<dw::Composite*>(parent);
    if (comp) {
        dw::Label* label = new dw::Label(comp);
        label->SetText("ID");

        group = new dw::ListBox(comp, dw::MULTISELECT);

        const char* fog_id_labels[] = {
            "0:DEPTH",
            "1:HEIGHT",
            "2:BUMP",
        };

        for (const char*& i : fog_id_labels)
            group->AddItem(i);

        group->SetItemIndex(fog_dw_id);
        group->AddSelectionListener(this);
    }
}

FogDw::GroupListBox::~GroupListBox() {

}

void FogDw::GroupListBox::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data->widget);
    if (slider) {
        fog* fog = fog_dw_get_fog();
        fog->set_density(slider->GetValue());
    }
}

FogDw::TypeListBox::TypeListBox(FogDw* fog_dw, dw::Widget* parent) {
    this->fog_dw = fog_dw;
    type = 0;

    dw::Composite* comp = dynamic_cast<dw::Composite*>(parent);
    if (comp) {
        fog* fog = fog_dw_get_fog();

        dw::Label* label = new dw::Label(comp);
        label->SetText("TYPE");

        type = new dw::ListBox(comp, dw::MULTISELECT);

        const char* fog_type_labels[] = {
            "OFF",
            "PARALLEL",
            "POINT",
            "SPOT",
        };

        for (const char*& i : fog_type_labels)
            type->AddItem(i);

        type->SetItemIndex(fog->get_type());
        type->AddSelectionListener(this);
    }
}

FogDw::TypeListBox::~TypeListBox() {

}

void FogDw::TypeListBox::Callback(dw::SelectionListener::CallbackData* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data->widget);
    if (list_box) {
        fog* fog = fog_dw_get_fog();
        fog->set_type((fog_type)list_box->list->selected_item);
        fog_dw->ResetData();
    }
}

FogDw::DensitySlider::DensitySlider(FogDw* fog_dw, dw::Widget* parent) {
    this->fog_dw = fog_dw;
    density = 0;

    dw::Composite* comp = dynamic_cast<dw::Composite*>(parent);
    if (comp) {
        fog* fog = fog_dw_get_fog();

        dw::Label* label = new dw::Label(comp);
        label->SetText("DENSITY");

        density = dw::Slider::Create(comp);
        density->SetText("DENSITY");
        density->format = "%4.3f";
        density->SetParams(fog->get_density(), 0.0f, 1.0f, 0.1f, 0.01f, 0.1f);
        density->AddSelectionListener(this);
    }
}

FogDw::DensitySlider::~DensitySlider() {

}

void FogDw::DensitySlider::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data->widget);
    if (slider) {
        fog* fog = fog_dw_get_fog();
        fog->set_density(slider->GetValue());
    }
}

FogDw::LinearSlider::LinearSlider(FogDw* fog_dw, dw::Widget* parent) {
    this->fog_dw = fog_dw;
    start = 0;
    end = 0;

    dw::Composite* comp = dynamic_cast<dw::Composite*>(parent);
    if (comp) {
        fog* fog = fog_dw_get_fog();

        dw::Label* label = new dw::Label(comp);
        label->SetText("LINEAR");

        start = dw::Slider::Create(comp, (dw::Flags)(dw::FLAG_800
            | dw::HORIZONTAL), 0.0f, 0.0f, 176.0f, 20.0f);
        start->SetText("START");
        start->format = "%6.2f";
        start->SetParams(fog->get_end(), -100.0f, 1000.0f, 200.0f, 0.1f, 1.0f);
        start->callback_data.i64 = 0;
        start->AddSelectionListener(this);
        start->AddSelectionListener(this);

        end = dw::Slider::Create(comp, (dw::Flags)(dw::FLAG_800
            | dw::HORIZONTAL), 0.0f, 0.0f, 176.0f, 20.0f);
        end->SetText("END");
        end->format = "%6.2f";
        end->SetParams(fog->get_end(), -100.0f, 1000.0f, 200.0f, 0.1f, 1.0f);
        end->callback_data.i64 = 1;
        end->AddSelectionListener(this);
    }
}

FogDw::LinearSlider::~LinearSlider() {

}

void FogDw::LinearSlider::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data->widget);
    if (slider) {
        fog* fog = fog_dw_get_fog();

        switch (slider->callback_data.i32) {
        case 0:
            fog->set_start(slider->GetValue());
            break;
        case 1:
            fog->set_end(slider->GetValue());
            break;
        }
    }
}

FogDw::ColorSlider::ColorSlider(FogDw* fog_dw, dw::Widget* parent) {
    this->fog_dw = fog_dw;
    r = 0;
    g = 0;
    b = 0;

    dw::Composite* comp = dynamic_cast<dw::Composite*>(parent);
    if (comp) {
        fog* fog = fog_dw_get_fog();

        vec4 value;
        fog->get_color(value);

        dw::Label* label = new dw::Label(comp);
        label->SetText("COLOR");

        r = dw::Slider::Create(comp);
        r->SetText("R");
        r->format = "%3.2f";
        r->SetParams(value.y, 0.0f, 8.0f, 0.8f, 0.01f, 0.1f);
        r->callback_data.i64 = 1;
        r->AddSelectionListener(this);

        g = dw::Slider::Create(comp);
        g->SetText("G");
        g->format = "%3.2f";
        g->SetParams(value.y, 0.0f, 8.0f, 0.8f, 0.01f, 0.1f);
        g->callback_data.i64 = 1;
        g->AddSelectionListener(this);

        b = dw::Slider::Create(comp);
        b->SetText("B");
        b->format = "%3.2f";
        b->SetParams(value.z, 0.0f, 8.0f, 0.8f, 0.01f, 0.1f);
        b->callback_data.i64 = 2;
        b->AddSelectionListener(this);
    }
}

FogDw::ColorSlider::~ColorSlider() {

}

void FogDw::ColorSlider::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data->widget);
    if (slider) {
        fog* fog = fog_dw_get_fog();
        fog->set_density(slider->GetValue());
    }
}

FogDw::IoButton::IoButton(FogDw* fog_dw, dw::Widget* parent) {
    this->fog_dw = fog_dw;
    field_10 = 0;
    save = 0;
    load = 0;

    dw::Composite* parent_comp = dynamic_cast<dw::Composite*>(parent);
    if (parent_comp) {
        dw::Composite* comp = new dw::Composite(parent_comp);
        comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

        save = new dw::Button(comp, dw::FLAG_8);
        save->SetText("SAVE");
        save->callback_data.v64 = fog_dw;
        save->callback = FogDw::IoButton::SaveCallback;

        load = new dw::Button(comp, dw::FLAG_8);
        load->SetText("LOAD");
        load->callback_data.v64 = fog_dw;
        load->callback = FogDw::IoButton::LoadCallback;
    }
}

FogDw::IoButton::~IoButton() {

}

void FogDw::IoButton::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data->widget);
    if (slider) {
        fog* fog = fog_dw_get_fog();
        fog->set_density(slider->GetValue());
    }
}

void FogDw::IoButton::LoadCallback(dw::Widget* data) {

}

void FogDw::IoButton::SaveCallback(dw::Widget* data) {

}

FogDw::CutLightEnableButton::CutLightEnableButton(FogDw* fog_dw, dw::Widget* parent) {
    this->fog_dw = fog_dw;
    set_for_each_cut = 0;

    dw::Composite* comp = dynamic_cast<dw::Composite*>(parent);
    if (comp) {
        const char* set_for_each_cut_text;
        if (dw::translate)
            set_for_each_cut_text = u8"Set for each PV cut";
        else
            set_for_each_cut_text = u8"PVのカットごとに設定";

        set_for_each_cut = new dw::Button(comp, dw::CHECKBOX);
        set_for_each_cut->SetText(set_for_each_cut_text);
        set_for_each_cut->SetValue(fog_dw_set_for_each_cut);
        set_for_each_cut->callback_data.i64 = 0;
        set_for_each_cut->AddSelectionListener(this);
    }
}

FogDw::CutLightEnableButton::~CutLightEnableButton() {

}

void FogDw::CutLightEnableButton::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
    if (button && !button->callback_data.i32)
        fog_dw_set_for_each_cut = button->value;
}

FogDw::FogDw() {
    SetText("FOG");

    fog* fog = fog_dw_get_fog();

    io_button = new FogDw::IoButton(this, this);
    cut_light_enable_button = new FogDw::CutLightEnableButton(this, this);
    group_list_box = new FogDw::GroupListBox(this, this);
    type_list_box = new FogDw::TypeListBox(this, this);

    switch (fog->get_type()) {
    case 0:
    default:
        density = 0;
        linear_slider = 0;
        color_slider = 0;
        break;
    case 1:
        density = new FogDw::DensitySlider(this, this);
        linear_slider = new FogDw::LinearSlider(this, this);
        color_slider = new FogDw::ColorSlider(this, this);
        break;
    case 2:
        density = new FogDw::DensitySlider(this, this);
        linear_slider = new FogDw::LinearSlider(this, this);
        color_slider = new FogDw::ColorSlider(this, this);
        break;
    case 3:
        density = new FogDw::DensitySlider(this, this);
        linear_slider = new FogDw::LinearSlider(this, this);
        color_slider = new FogDw::ColorSlider(this, this);
        break;
    }

    UpdateLayout();

    rect.pos = fog_dw_rect.pos;
    SetSize(fog_dw_rect.size);
}

FogDw::~FogDw() {

}

void FogDw::Hide() {
    dw::Shell::Hide();
    fog_dw = 0;
}

void FogDw::ResetData() {
    fog_dw_rect = rect;
    dw::Shell::Hide();
    fog_dw = 0;
    fog_dw_init();
}

static FogDw* fog_dw_get() {
    return fog_dw;
}

static fog* fog_dw_get_fog() {
    return &rctx_ptr->fog[fog_dw_id];
}
