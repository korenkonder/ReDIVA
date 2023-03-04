/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "dw.hpp"
#include "imgui_helper.hpp"

namespace dw_gui_detail {
    struct Display {
        /*dw::Shell* field_8;
        dw::Menu* field_10;
        dw::Widget* field_18;
        dw::Widget* field_20;
        dw::Widget* field_28;
        dw::Widget* field_30;
        dw::Shell* field_38;
        char field_40;
        char field_41;*/
        std::vector<dw::Shell*> shells;
        /*std::vector<dw::Menu*> menus;
        struc_748 field_78;
        struc_748 field_90;*/
        std::vector<dw::Widget*> free_widgets;
        /*struc_751 field_C0;
        int field_F4;
        int field_F8;
        int field_FC;
        int field_100;
        int field_104;
        int field_108;
        int field_10C;
        int field_110;
        int field_114;
        __int64 field_118;
        vec2 field_120;
        float_t field_128;
        dw::DragBoundsControl* field_130;
        std::string name;
        dw_gui_detail::Display::RootKeySelection root_key_selection;*/

        Display();
        ~Display();
        
        void AddShell(dw::Shell* value);
        void CheckShells();
        void Ctrl();
        void Draw();
        void FreeWidgets();
        void RemoveShell(dw::Shell* value);
    };
}

struct dw_gui_colors {
    vec4u8 background;
    vec4u8 foreground;
    vec4u8 selection_background;
    vec4u8 selection_foreground;
    vec4u8 selection_inactive_background;
    vec4u8 selection_inactive_foreground;
    vec4u8 disable_background;
    vec4u8 disable_foreground;
    vec4u8 title_background;
    vec4u8 title_foreground;
    vec4u8 title_inactive_background;
    vec4u8 title_inactive_foreground;
    vec4u8 active_border_color;
    vec4u8 inactive_border_color;
    vec4u8 focus_border_color;
    vec4u8 border_color;
    vec4u8 border_light_color;
    vec4u8 border_dark_color;
    vec4u8 popup_background;
    vec4u8 popup_foreground;
    vec4u8 popup_selection_background;
    vec4u8 popup_selection_foreground;
    vec4u8 menu_background;
    vec4u8 menu_foreground;
    vec4u8 menu_selection_background;
    vec4u8 menu_selection_foreground;
    vec4u8 button_background;
    vec4u8 button_foreground;
    vec4u8 button_disable_background;
    vec4u8 button_disable_foreground;
};

static dw_gui_colors dw_gui_colors_default = {
    { 0xFF, 0xFF, 0xFF, 0xFF },
    { 0x00, 0x00, 0x00, 0xFF },
    { 0x00, 0x00, 0x80, 0xFF },
    { 0xFF, 0xFF, 0xFF, 0xFF },
    { 0xC0, 0xC0, 0xC0, 0xFF },
    { 0xFF, 0xFF, 0xFF, 0xFF },
    { 0xC0, 0xC0, 0xC0, 0xFF },
    { 0x80, 0x80, 0x80, 0xFF },
    { 0x00, 0x00, 0x80, 0xFF },
    { 0xFF, 0xFF, 0xFF, 0xFF },
    { 0xC0, 0xC0, 0xC0, 0xFF },
    { 0x80, 0x80, 0x80, 0xFF },
    { 0xC0, 0xC0, 0xC0, 0xFF },
    { 0xC0, 0xC0, 0xC0, 0xFF },
    { 0xFF, 0x00, 0x00, 0xFF },
    { 0xC0, 0xC0, 0xC0, 0xFF },
    { 0xE0, 0xE0, 0xE0, 0xFF },
    { 0x80, 0x80, 0x80, 0xFF },
    { 0xFF, 0xFF, 0xFF, 0xFF },
    { 0x00, 0x00, 0x00, 0xFF },
    { 0x00, 0x00, 0x80, 0xFF },
    { 0xFF, 0xFF, 0xFF, 0xFF },
    { 0xC0, 0xC0, 0xC0, 0xFF },
    { 0x00, 0x00, 0x00, 0xFF },
    { 0x00, 0x00, 0x80, 0xFF },
    { 0xFF, 0xFF, 0xFF, 0xFF },
    { 0xC0, 0xC0, 0xC0, 0xFF },
    { 0x00, 0x00, 0x00, 0xFF },
    { 0xF0, 0xF0, 0xF0, 0xFF },
    { 0x80, 0x80, 0x80, 0xFF },
};

dw_gui_detail::Display* dw_gui_detail_display;

static bool dw_gui_get_ctrl();
static bool dw_gui_get_disp();

namespace dw {
    SelectionListener::SelectionListener() {

    }
    
    SelectionListener::~SelectionListener() {

    }
    
    SelectionAdapter::SelectionAdapter() {

    }
    
    SelectionAdapter::~SelectionAdapter() {

    }

    void SelectionAdapter::Field_8(Widget* data) {

    }

    void SelectionAdapter::Field_10(Widget* data) {

    }

    SelectionListenerOnHook::SelectionListenerOnHook() : callback() {

    }
    
    SelectionListenerOnHook::SelectionListenerOnHook(Widget::Callback callback) {
        this->callback = callback;
    }
    
    SelectionListenerOnHook::~SelectionListenerOnHook() {

    }

    void SelectionListenerOnHook::Field_8(Widget* data) {
        callback(data);
    }

    Widget::Widget(Widget* parent, Flags flags) : free(),
        callback_data(), free_callback(), parent() {
        this->flags = flags;
    }
    
    Widget::~Widget() {

    }

    void Widget::Draw() {

    }

    void Widget::Reset() {
        name.clear();
    }

    std::string Widget::GetName() {
        return name;
    }

    void Widget::SetName(std::string& str) {
        name.assign(str);
    }

    void Widget::SetSize(vec2 value) {
        size = value;
    }

    vec2 Widget::GetPos() {
        return position;
    }

    void Widget::Free() {
        if (free)
            return;

        Reset();
        if (free_callback)
            free_callback(this);
        free = 1;

        if (dw_gui_detail_display)
            dw_gui_detail_display->free_widgets.push_back(this);
    }

    void Widget::SetName(std::string&& str) {
        SetName(str);
    }

    void Widget::UpdateDraw() {
        //sub_140302800(qword_1411900C8);
        Draw();
    }

    Control::Control(Composite* parent, Flags flags) : Widget(parent, flags),
        parent_comp(), parent_shell()/*, parent_menu()*/{
        parent_comp = parent;
        if (parent) {
            parent->controls.push_back(this);
            parent_shell = parent->parent_shell;
        }
    }
    
    Control::~Control() {

    }

    void Control::Draw() {

    }

    void Control::Reset() {
        parent_comp = 0;
        Widget::Reset();
    }

    Label::Label(Composite* parent, Flags flags) : Control(parent, flags) {

    }

    Label::~Label() {

    }

    void Label::Draw() {
        ImGui::Text(name.c_str());
    }

    ScrollBar::ScrollBar(Control* parent, Widget::Flags flags) : Widget(parent, flags),
        value(), min(), field_A0(), field_A5(), field_A6() {
        field_68 = parent;
        max = 100.0f;
        field_94 = 10.0f;
        step = 1.0f;
        step_fast = 10.0f;
        round = true;
        SetSize({ 8.0f, 8.0f });
    }

    ScrollBar::~ScrollBar() {

    }

    void ScrollBar::Draw() {

    }

    void ScrollBar::SetSize(vec2 value) {
        size = value;
    }

    void ScrollBar::SetParams(float_t value, float_t min,
        float_t max, float_t a5, float_t step, float_t step_fast) {
        this->value = value;
        this->field_94 = a5;
        this->min = min;
        this->step_fast = step_fast;
        this->max = max;
        this->step = step;
    }

    Scrollable::Scrollable(Composite* parent, Flags flags) : Control(parent, flags), h_bar(), v_bar() {
        if (flags & WIDGET_SCROLL_H) {
            h_bar = new ScrollBar(parent, WIDGET_SCROLL_H);
            h_bar->SetName("h_bar");
        }

        if (flags & WIDGET_SCROLL_V) {
            v_bar = new ScrollBar(parent, WIDGET_SCROLL_V);
            v_bar->SetName("v_bar");
        }
    }
    
    Scrollable::~Scrollable() {

    }

    void Scrollable::Draw() {
        if (v_bar)
            v_bar->UpdateDraw();
    }

    void Scrollable::Reset() {
        Control::Reset();
    }

    Layout::Layout() {

    }

    Layout::~Layout() {

    }
    
    FillLayout::FillLayout(int32_t a2) {
        field_8 = a2;
    }

    FillLayout::~FillLayout() {

    }

    vec2 FillLayout::GetSize(Composite* comp) {
        return comp->size;
    }

    void FillLayout::SetSize(Composite* comp) {
        const vec2 position = comp->position;
        const vec2 size = comp->size;
        for (Control* i : comp->controls) {
            i->position = position;
            i->SetSize(size);
        }
    }
    
    GraphLayout::GraphLayout(int32_t a2) {
        field_8 = a2;
    }

    GraphLayout::~GraphLayout() {

    }

    vec2 GraphLayout::GetSize(Composite* comp) {
        return comp->size;
    }

    void GraphLayout::SetSize(Composite* comp) {
        const vec2 position = comp->position;
        const vec2 size = comp->size;
        for (Control* i : comp->controls) {
            i->position = position;
            i->SetSize(size);
        }
    }

    GridLayout::GridLayout(size_t a2) : field_8(), field_10(),
        field_18(), field_20(), field_28(), field_30(), field_38() {
        field_40 = a2;
        field_48 = 2.0f;
        field_4C = 2.0f;
    }

    GridLayout::~GridLayout() {

    }

    vec2 GridLayout::GetSize(Composite* comp) {
        return comp->size;
    }

    void GridLayout::SetSize(Composite* comp) {
        const vec2 position = comp->position;
        const vec2 size = comp->size;
        for (Control* i : comp->controls) {
            i->position = position;
            i->SetSize(size);
        }
    }

    RowLayout::RowLayout(int32_t a2) {
        field_8 = 512;
        field_C = 2.0f;
    }

    RowLayout::~RowLayout() {

    }

    vec2 RowLayout::GetSize(Composite* comp) {
        return comp->size;
    }

    void RowLayout::SetSize(Composite* comp) {
        const vec2 position = comp->position;
        const vec2 size = comp->size;
        for (Control* i : comp->controls) {
            i->position = position;
            i->SetSize(size);
        }
    }

    Composite::Composite(Composite* parent, Flags flags) : Scrollable(parent, flags), layout() {

    }
    
    Composite::~Composite() {

    }

    void Composite::Draw() {
        for (Control* i : controls)
            i->Draw();
    }

    void Composite::Reset() {
        //for (Control* i : controls)
            //Widget::sub_1402ECDE0(i);

        controls.clear();
        Scrollable::Reset();
    }

    void Composite::SetLayout(Layout* value) {
        layout = value;
    }

    Button::Button(Composite* parent, Flags flags) : Control(parent, flags),  value(), callback() {

    }
    
    Button::~Button() {

    }

    void Button::Draw() {
        if (flags & WIDGET_CHECKBOX) {
            if (ImGui::Checkbox(name.c_str(), &value) && callback)
                callback(this);
        }
        else if (flags & WIDGET_RADIOBUTTON) {
            if (ImGui::RadioButton(name.c_str(), value)) {
                if (callback)
                    callback(this);
                value = true;
            }
        }
        else
            if (ImGui::Button(name.c_str()) && callback)
                callback(this);
    }

    void Button::SetValue(bool value) {
        this->value = value;
        if (!(flags & WIDGET_RADIOBUTTON))
            return;

        for (Control* i : parent_comp->controls) {
            Button* button = (Button*)dynamic_cast<Button*>(i);
            if (button && button != this && button->flags & WIDGET_RADIOBUTTON)
                button->value = false;
        }
    }

    ShellCloseButton::ShellCloseButton(Shell* parent) : Button(parent) {
        SetSize({ 14.0f, 14.0f });
        SetName("Close Box");
        callback = Callback;
        //field_100 = 0;
        //field_104 = 4;
    }

    ShellCloseButton::~ShellCloseButton() {

    }

    void ShellCloseButton::Draw() {

    }

    void ShellCloseButton::Callback(Widget* data) {
        ShellCloseButton* close_button = dynamic_cast<ShellCloseButton*>(data);
        if (close_button)
            close_button->parent_shell->Hide();
    }

    Shell::Shell(Shell* parent, Flags flags) : Composite(parent, flags),
        disp(), disp_callback(), hide_callback(), destroy(), close_button() {
        parent_shell = this;

        if (flags & WIDGET_CLOSE_BUTTON) {
            close_button = new ShellCloseButton(this);
            if (controls.size())
                controls.pop_back();
        }

        if (parent) {
            parent_comp = parent;
            parent->children.push_back(this);
        }

        dw_gui_detail_display->shells.push_back(this);
    }

    Shell::~Shell() {

    }

    void Shell::Draw() {
        if (!GetDisp())
            return;

        if (ImGui::Begin(name.c_str())) {
            Composite::Draw();
            ImGui::End();
        }
    }

    void Shell::Reset() {
        std::vector<Widget*> vec;
        vec.assign(children.begin(), children.end());
        children.clear();

        for (Widget* i : vec)
            i->Free();
        
        /*for (Menu* i : menus)
            i->Free();
        menus.clear();*/

        if (close_button) {
            close_button->Free();
            close_button = 0;
        }

        if (parent_comp) {
            Shell* parent_shell = dynamic_cast<Shell*>(parent_comp);
            if (parent_shell) {
                auto i = parent_shell->children.begin();
                auto i_end = parent_shell->children.end();
                while (i != i_end)
                    if (*i == this) {
                        parent_shell->children.erase(i);
                        break;
                    }
            }
        }

        dw_gui_detail_display->RemoveShell(this);

        Composite::Reset();
    }

    void Shell::Hide() {

    }

    void Shell::Disp() {
        SetDisp(true);
        dw_gui_detail_display->AddShell(this);
        //dw_gui_detail_display->field_38 = this;
        //dw_gui_detail_display->field_40 = true;
    }

    bool Shell::GetDisp() {
        return disp;
    }

    void Shell::SetDisp(bool value) {
        if (disp == value)
            return;

        disp = value;
        if (disp) {
            dw_gui_detail_display->AddShell(this);
            //dw_gui_detail_display->field_38 = this;
            //dw_gui_detail_display->field_40 = true;
            if (disp_callback)
                disp_callback(this);
        }
        else {
            //dw_gui_detail_display->field_41 = true;
            if (hide_callback)
                hide_callback(this);
        }
    }

    List::List(Composite* parent, Flags flags) : Scrollable(parent, flags) {
        hovered_item = -1;
        selected_item = -1;
    }

    List::~List() {

    }

    void List::Draw() {

    }

    void List::Reset() {
        items.clear();
        Scrollable::Reset();
    }

    void List::AddItem(const std::string& str) {
        items.push_back(str);
    }

    void List::AddItem(const std::string&& str) {
        items.push_back(str);
    }

    void List::AddSelectionListener(SelectionListener* value) {
        selection_listeners.push_back(value);
    }

    void List::ClearItems() {
        items.clear();
    }

    std::string List::GetItem(size_t index) const {
        return items[index];
    }

    void List::ResetSelectedItem() {
        if (flags & WIDGET_4)
            selected_item = -1;
        else
            for (int32_t& i : selections)
                i = 0;
    }

    void List::ResetSetSelectedItem(size_t index) {
        ResetSelectedItem();
        SetSelectedItem(index);
    }

    void List::SetSelectedItem(size_t index) {
        if (index < items.size())
            if (flags & WIDGET_4)
                selected_item = index;
            else
                selections[index] = 1;
    }

    ListBox::ListBox(Composite* parent, Flags flags) : Composite(parent, flags), list() {
        list = new List(parent, flags);
    }

    ListBox::~ListBox() {
        delete list;
    }

    void ListBox::Draw() {
        bool window_focus = false;

        std::vector<std::string>& items = list->items;
        size_t hovered_item = list->selected_item;
        size_t selected_item = list->selected_item;
        ImGui::GetContentRegionAvailSetNextItemWidth();
        ImGui::PushID(this);
        if (ImGui::BeginCombo("##", selected_item != -1
            && items.size() ? items[selected_item].c_str() : "", 0)) {
            for (std::string& i : items) {
                size_t item = &i - items.data();

                ImGui::PushID(&i);
                if (ImGui::Selectable(i.c_str(), selected_item == item)
                    || ImGui::ItemKeyPressed(ImGuiKey_Enter)
                    || (ImGui::IsItemFocused() && selected_item != item)) {
                    selected_item = item;
                }

                if (ImGui::IsItemHovered())
                    hovered_item = item;
                ImGui::PopID();

                if (selected_item == item)
                    ImGui::SetItemDefaultFocus();
            }

            window_focus |= true;
            ImGui::EndCombo();
        }

        if (list->hovered_item != hovered_item)
            list->hovered_item = hovered_item;

        if (list->selected_item != selected_item) {
            list->selected_item = selected_item;
            for (SelectionListener*& i : list->selection_listeners)
                i->Field_8(this);
        }
        ImGui::PopID();
    }

    void ListBox::Reset() {
        Composite::Reset();
    }

    void ListBox::AddItem(const std::string& str) {
        list->AddItem(str);
    }

    void ListBox::AddItem(const std::string&& str) {
        list->AddItem(str);
    }

    void ListBox::AddSelectionListener(SelectionListener* value) {
        list->AddSelectionListener(value);
    }

    void ListBox::ClearItems() {
        list->ClearItems();
    }

    std::string ListBox::GetItem(size_t index) const {
        return list->GetItem(index);
    }

    Slider::Slider(Composite* parent, Widget::Flags flags) : Control(parent, flags) {
        format = "%f";
        scroll_bar = new ScrollBar(this, (Widget::Flags)(flags & (WIDGET_SCROLL_H | WIDGET_SCROLL_V)));
        scroll_bar->parent = this;
    }

    Slider::~Slider() {
        delete scroll_bar;
    }

    void Slider::Draw() {
        ScrollBar* scroll_bar = this->scroll_bar;
        if (ImGui::ColumnSliderFloatButton(name.c_str(), &scroll_bar->value,
            scroll_bar->step, scroll_bar->min, scroll_bar->max, scroll_bar->step_fast, format, 0))
            for (SelectionListener*& i : selection_listeners)
                i->Field_8(this);
    }

    void Slider::Reset() {
        if (scroll_bar) {
            scroll_bar->Free();
            scroll_bar = 0;
        }

        Control::Reset();
    }

    void Slider::SetSize(vec2 value) {
        Widget::SetSize(value);
        scroll_bar->position.x = value.x - scroll_bar->size.x;
        scroll_bar->position.y = 0.0f;
    }

    void Slider::AddSelectionListener(SelectionListener* value) {
        selection_listeners.push_back(value);
    }

    Slider* Slider::make(Composite* parent, Widget::Flags flags,
        float_t x, float_t y, float_t width, float_t height, const char* name) {
        Slider* slider = new Slider(parent, flags);
        ScrollBar* scroll_bar = slider->scroll_bar;
        slider->SetName(name);
        scroll_bar->SetName("slider's bar");
        scroll_bar->SetSize({ width, height });
        /*sub_140302810(qword_1411900C8, &slider->font);
        std::string v13 = slider->GetName();
        float_t v15;
        float_t v17;
        if (flags & WIDGET_100) {
            vec2 v21;
            vec2 v22;
            sub_140302A70(qword_1411900C8, &v22, v13);
            sub_140302A70(qword_1411900C8, &v21, "0000");
            scroll_bar->position.x = v21.x + v22.x;
            scroll_bar->position.y = 0.0f;
            v17 = v21.x + v22.x + scroll_bar->size.x ;
            v15 = p_dw__Font::GetFontGlyphHeight(&slider->font);
        }
        else {
            vec2 v21;
            vec2 v22;
            sub_140302A70(qword_1411900C8, &v21, v13);
            sub_140302A70(qword_1411900C8, &v22, "0000");
            v17 = p_dw__Font::GetFontGlyphWidth(&slider->font);
            v15 = v21.x + v22.x + scroll_bar->size.y;
        }*/
        slider->position.x = x;
        slider->position.y = y;
        //slider->SetSize({ v17, v15 });
        return slider;
    }
}

void dw_gui_detail_display_init() {
    if (!dw_gui_detail_display)
        dw_gui_detail_display = new dw_gui_detail::Display;
}

void dw_gui_detail_display_free() {
    if (dw_gui_detail_display) {
        delete dw_gui_detail_display;
        dw_gui_detail_display = 0;
    }
}

void dw_gui_ctrl_disp() {
    if (dw_gui_get_ctrl())
        dw_gui_detail_display->Ctrl();
    if (dw_gui_get_disp())
        dw_gui_detail_display->Draw();
};

dw_gui_detail::Display::Display() {

}

dw_gui_detail::Display::~Display() {

}

void dw_gui_detail::Display::AddShell(dw::Shell* value) {
    if (!value)
        return;

    for (dw::Shell* i : shells)
        if (i == value)
            return;

    shells.push_back(value);
}

void dw_gui_detail::Display::CheckShells() {
    std::vector<dw::Shell*> vec;

    for (dw::Shell* i : shells)
        if (i->destroy)
            vec.push_back(i);

    if (vec.size()) {
        for (dw::Shell* i : vec)
            i->Free();
        //field_41 = true;
    }
}

void dw_gui_detail::Display::Ctrl() {
    CheckShells();
    FreeWidgets();
}

void dw_gui_detail::Display::Draw() {
    for (dw::Shell* i : shells)
        if (i->GetDisp())
            i->UpdateDraw();
}

void dw_gui_detail::Display::FreeWidgets() {
    for (auto& i : free_widgets) {
        delete i;
        /*if (field_28 == i)
            field_28 = 0;
        if (field_30 == i)
            field_30 = 0;*/
    }
    free_widgets.clear();
}

void dw_gui_detail::Display::RemoveShell(dw::Shell* value) {
    auto i = shells.begin();
    auto i_end = shells.end();
    while (i != i_end)
        if (*i == value) {
            shells.erase(i);
            break;
        }

    /*if (field_8 == value) {
        field_8 = 0;
        field_41 = true;
    }

    if (field_18 == value)
        field_18 = 0;

    if (field_20 == value)
        field_20 = 0;*/
}

static bool dw_gui_get_ctrl() {
    //return false;
    return true;
}

static bool dw_gui_get_disp() {
    //return false;
    return true;
}
