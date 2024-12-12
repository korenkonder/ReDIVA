/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "reflect_refract.hpp"
#include "../../CRE/light_param/face.hpp"
#include "../../CRE/light_param.hpp"
#include "../../CRE/render_context.hpp"
#include "../dw.hpp"

extern render_context* rctx_ptr;

class ReflectRefractDw : public dw::Shell {
public:
    dw::RowLayout* vertical_layout;
    dw::RowLayout* horizontal_layout;

    ReflectRefractDw();
    virtual ~ReflectRefractDw() override;
    virtual void Draw() override;

    virtual void Hide() override;

    static void ReflectBlurFilterCallback(dw::ListBox* data);
    static void ReflectBlurNumCallback(dw::Slider* data);
    static void ReflectResolutionCallback(dw::ListBox* data);
    static void RefractResolutionCallback(dw::ListBox* data);
};

ReflectRefractDw* reflect_refract_dw;

void reflect_refract_dw_init() {
    if (!reflect_refract_dw) {
        reflect_refract_dw = new ReflectRefractDw;
        reflect_refract_dw->UpdateLayout();
        reflect_refract_dw->sub_1402F38B0();
    }
    else
        reflect_refract_dw->Disp();
}

ReflectRefractDw::ReflectRefractDw() {
    const reflect_refract_resolution_mode reflect_resolution_mode
        = rctx_ptr->render_manager->get_reflect_resolution_mode();
    const reflect_refract_resolution_mode refract_resolution_mode
        = rctx_ptr->render_manager->get_reflect_resolution_mode(); // Intentional!
    const int32_t reflect_blur_num = rctx_ptr->render_manager->reflect_blur_num;
    const blur_filter_mode reflect_blur_filter = rctx_ptr->render_manager->reflect_blur_filter;

    SetText("REFLECT/REFRACT");

    rect.pos = 0.0f;
    SetSize({ 320.0f, 200.0f });

    const float_t glyph_height = font.GetFontGlyphHeight();

    vertical_layout = new dw::RowLayout(dw::VERTICAL);
    horizontal_layout = new dw::RowLayout(dw::HORIZONTAL);

    dw::Composite* comp = new dw::Composite(this);
    comp->layout = vertical_layout;

    dw::Group* reflect_group = new dw::Group(comp);
    reflect_group->SetText("REFLECT");

    dw::Composite* reflect_comp = new dw::Composite(reflect_group);
    reflect_comp->layout = horizontal_layout;

    (new dw::Label(reflect_comp, dw::FLAG_4000))->SetText("RESOLUTION    ");

    dw::ListBox* reflect_resolution = new dw::ListBox(reflect_comp,
        (dw::Flags)(dw::FLAG_800 | dw::VERTICAL | dw::MULTISELECT));
    reflect_resolution->SetText("listbox0");
    reflect_resolution->AddItem("256x256");
    reflect_resolution->AddItem("512x256");
    reflect_resolution->AddItem("512x512");
    reflect_resolution->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)ReflectRefractDw::ReflectResolutionCallback));
    reflect_resolution->SetItemIndex(reflect_resolution_mode);
    reflect_resolution->SetMaxItems(3);

    dw::Slider* reflect_blur_num_slider = dw::Slider::Create(reflect_group,
        (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL), 0.0f, 0.0f, 100.0f, glyph_height, "BLUR NUM");
    reflect_blur_num_slider->SetParams((float_t)reflect_blur_num, 0.0f, 10.0f, 2.0f, 1.0f, 1.0f);
    reflect_blur_num_slider->format = "%4.0f";
    reflect_blur_num_slider->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)ReflectRefractDw::ReflectBlurNumCallback));
    reflect_blur_num_slider->callback_data.v64 = this;

    dw::Composite* reflect_blur_filter_comp = new dw::Composite(reflect_group);
    reflect_blur_filter_comp->layout = horizontal_layout;

    (new dw::Label(reflect_blur_filter_comp, dw::FLAG_4000))->SetText("BLUR FILTER   ");

    dw::ListBox* reflect_blur_filter_list_box = new dw::ListBox(reflect_blur_filter_comp,
        (dw::Flags)(dw::FLAG_800 | dw::VERTICAL | dw::MULTISELECT));
    reflect_blur_filter_list_box->SetText("listbox0");
    reflect_blur_filter_list_box->AddItem("4");
    reflect_blur_filter_list_box->AddItem("9");
    reflect_blur_filter_list_box->AddItem("16");
    reflect_blur_filter_list_box->AddItem("32");
    reflect_blur_filter_list_box->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)ReflectRefractDw::ReflectBlurFilterCallback));
    reflect_blur_filter_list_box->SetItemIndex(reflect_blur_filter);
    reflect_blur_filter_list_box->SetMaxItems(7);

    dw::Group* refract_group = new dw::Group(comp);
    refract_group->SetText("REFRACT");

    dw::Composite* refract_comp = new dw::Composite(refract_group);
    refract_comp->layout = horizontal_layout;

    (new dw::Label(refract_comp, dw::FLAG_4000))->SetText("RESOLUTION    ");

    dw::ListBox* refract_resolution = new dw::ListBox(refract_comp,
        (dw::Flags)(dw::FLAG_800 | dw::VERTICAL | dw::MULTISELECT));
    refract_resolution->SetText("listbox0");
    refract_resolution->AddItem("256x256");
    refract_resolution->AddItem("512x256");
    refract_resolution->AddItem("512x512");
    refract_resolution->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)ReflectRefractDw::RefractResolutionCallback));
    refract_resolution->SetItemIndex(refract_resolution_mode);
    refract_resolution->SetMaxItems(3);
}

ReflectRefractDw::~ReflectRefractDw() {

}

void ReflectRefractDw::Draw() {
    dw::Shell::Draw();
}

void ReflectRefractDw::Hide() {
    SetDisp();
}

void ReflectRefractDw::ReflectBlurFilterCallback(dw::ListBox* data) {
    rctx_ptr->render_manager->reflect_blur_filter = (blur_filter_mode)(int32_t)data->list->selected_item;
}

void ReflectRefractDw::ReflectBlurNumCallback(dw::Slider* data) {
    rctx_ptr->render_manager->reflect_blur_num = (int32_t)data->GetValue();
}

void ReflectRefractDw::ReflectResolutionCallback(dw::ListBox* data) {
    switch (data->list->selected_item) {
    case 0:
        rctx_ptr->render_manager->set_reflect_resolution_mode(REFLECT_REFRACT_RESOLUTION_256x256);
        break;
    case 1:
        rctx_ptr->render_manager->set_reflect_resolution_mode(REFLECT_REFRACT_RESOLUTION_512x256);
        break;
    case 2:
        rctx_ptr->render_manager->set_reflect_resolution_mode(REFLECT_REFRACT_RESOLUTION_512x512);
        break;
    }
}

void ReflectRefractDw::RefractResolutionCallback(dw::ListBox* data) {
    switch (data->list->selected_item) {
    case 0:
        rctx_ptr->render_manager->set_refract_resolution_mode(REFLECT_REFRACT_RESOLUTION_256x256);
        break;
    case 1:
        rctx_ptr->render_manager->set_refract_resolution_mode(REFLECT_REFRACT_RESOLUTION_512x256);
        break;
    case 2:
        rctx_ptr->render_manager->set_refract_resolution_mode(REFLECT_REFRACT_RESOLUTION_512x512);
        break;
    }
}
