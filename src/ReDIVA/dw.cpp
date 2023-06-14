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
        rectangle field_F8;
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

extern bool input_locked;

static bool dw_gui_get_ctrl();
static bool dw_gui_get_disp();

namespace dw {
    static RowLayout* row_layout_vertical = new RowLayout(VERTICAL);

    static DropDownListScrollBarSelectionListener drop_down_list_scroll_bar_selection_listener;
    static ScrollBarTestSelectionListener scroll_bar_test_Selection_listener;

    SelectionListener::SelectionListener() {

    }

    SelectionListener::~SelectionListener() {

    }

    SelectionAdapter::SelectionAdapter() {

    }

    SelectionAdapter::~SelectionAdapter() {

    }

    void SelectionAdapter::Callback(Widget* data) {

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

    void SelectionListenerOnHook::Callback(Widget* data) {
        callback(data);
    }

    DropDownListScrollBarSelectionListener::DropDownListScrollBarSelectionListener() {

    }
    
    DropDownListScrollBarSelectionListener::~DropDownListScrollBarSelectionListener() {

    }

    void DropDownListScrollBarSelectionListener::Callback(Widget* data) {

    }
    
    ScrollBarTestSelectionListener::ScrollBarTestSelectionListener() {

    }
    
    ScrollBarTestSelectionListener::~ScrollBarTestSelectionListener() {

    }

    void ScrollBarTestSelectionListener::Callback(Widget* data) {

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
        if (flags & LABEL_SIZE) {
            ImVec2 size = ImGui::CalcTextSize(name.c_str(), 0, true);
            this->size = { size.x, size.y };
        }
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

    vec2 dw::Control::GetPos() {
        vec2 pos = position;
        if (parent_comp) {
            rectangle parent_rect = parent_comp->GetBoundingBox();
            vec2 parent_pos = parent_comp->GetPos();
            pos += parent_pos + parent_rect.pos;
            if (parent_comp->v_bar)
                pos.y -= parent_comp->v_bar->value;
        }
        return pos;
    }

    void Control::GetSetSize() {
        SetSize(GetSize());
    }

    vec2 Control::GetSize() {
        return size;
    }

    Label::Label(Composite* parent, Flags flags) : Control(parent, flags) {

    }

    Label::~Label() {

    }

    void Label::Draw() {
        ImGui::PushID(this);
        if (size != 0.0f)
            ImGui::Text(name.c_str());
        else
            ImGui::TextCentered(name.c_str());
        ImGui::PopID();
    }

    ScrollBar::ScrollBar(Control* parent, Flags flags) : Widget(parent, flags),
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

    void ScrollBar::SetValue(float_t value) {
        value = clamp_def(value, min, max);
        if (round)
            this->value = roundf(value / step) * step;
        else
            this->value = value;
    }

    void ScrollBar::SetWidth(float_t value) {
        vec2 glyph_size = value;//font->GetFontGlyphSize();
        if (flags & HORIZONTAL) {
            size.x = value;
            size.y = glyph_size.y;
        }
        else {
            size.x = glyph_size.x;
            size.y = value;
        }
    }

    void ScrollBar::AddSelectionListener(SelectionListener* value) {
        selection_listeners.push_back(value);
    }

    Scrollable::Scrollable(Composite* parent, Flags flags) : Control(parent, flags), h_bar(), v_bar() {
        if (flags & HORIZONTAL) {
            h_bar = new ScrollBar(parent, HORIZONTAL);
            h_bar->SetName("h_bar");
        }

        if (flags & VERTICAL) {
            v_bar = new ScrollBar(parent, VERTICAL);
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

    void Scrollable::SetSize(vec2 value) {
        Widget::SetSize(value);

        if (flags & FLAG_800) {
            value.x -= 2.0f * 2.0f;
            value.y -= 2.0f * 2.0f;
        }

        if (v_bar) {
            v_bar->position.x = value.x - v_bar->size.x;
            v_bar->position.y = 0.0f;
            v_bar->SetWidth(value.y);
        }
    }

    rectangle Scrollable::GetBoundingBox() {
        rectangle rect;
        rect.size.x = size.x;
        rect.size.y = size.y;

        if (flags & FLAG_800) {
            rect.pos.x = 2.0f;
            rect.pos.y = 2.0f;
            rect.size.x = size.x - 2.0f * 2.0f;
            rect.size.y = size.y - 2.0f * 2.0f;
        }

        if (v_bar)
            rect.size.x -= v_bar->size.x;
        return rect;
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

    void FillLayout::Begin() {

    }

    void FillLayout::Next() {

    }

    void FillLayout::End() {

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

    void GraphLayout::Begin() {

    }

    void GraphLayout::Next() {

    }

    void GraphLayout::End() {

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

    void GridLayout::Begin() {

    }

    void GridLayout::Next() {

    }

    void GridLayout::End() {

    }

    RowLayout::RowLayout(Flags flags) : imgui_curr_line_size(),
        imgui_curr_line_text_base_offset() {
        this->flags = flags;
        spacing = 2.0f;
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

    void RowLayout::Begin() {

    }

    void RowLayout::Next() {
        if (flags == HORIZONTAL) {
            ImGuiWindow* window = GImGui->CurrentWindow;
            if (!window->SkipItems && !window->DC.IsSameLine) {
                imgui_cursor_pos.x = window->DC.CursorPos.x;
                imgui_cursor_pos.y = window->DC.CursorPos.y;
                imgui_curr_line_size.x = window->DC.CurrLineSize.x;
                imgui_curr_line_size.y = window->DC.CurrLineSize.y;
                imgui_curr_line_text_base_offset = window->DC.CurrLineTextBaseOffset;
            }

            ImGui::SameLine(0.0f, spacing);
        }
    }

    void RowLayout::End() {
        if (flags == HORIZONTAL) {
            ImGuiWindow* window = GImGui->CurrentWindow;
            if (!window->SkipItems) {
                window->DC.CursorPos.x = imgui_cursor_pos.x;
                window->DC.CursorPos.y = imgui_cursor_pos.y;
                window->DC.CurrLineSize.x = imgui_curr_line_size.x;
                window->DC.CurrLineSize.y = imgui_curr_line_size.y;
                window->DC.CurrLineTextBaseOffset = imgui_curr_line_text_base_offset;
                window->DC.IsSameLine = false;
            }
        }
    }

    Composite::Composite(Composite* parent, Flags flags) : Scrollable(parent, flags) {
        layout = row_layout_vertical;
    }

    Composite::~Composite() {

    }

    void Composite::Draw() {
        ImGui::PushID(this);
        if (layout)
            layout->Begin();

        for (Control*& i : controls) {
            if (i->size != 0.0f)
                ImGui::SetNextItemWidth(i->size.x);
            i->Draw();
            if (layout)
                layout->Next();
        }

        if (layout)
            layout->End();
        ImGui::PopID();
    }

    void Composite::Reset() {
        //for (Control* i : controls)
            //Widget::sub_1402ECDE0(i);

        controls.clear();
        Scrollable::Reset();
    }

    void Composite::SetSize(vec2 size) {
        Scrollable::SetSize(size);
        layout->SetSize(this);
    }

    void Composite::GetSetSize() {
        SetSize(GetSize());
    }

    vec2 Composite::GetSize() {
        return layout->GetSize(this);
    }

    void Composite::SetLayout(Layout* value) {
        layout = value;
    }

    Group::Group(Composite* parent, Flags flag) : Composite(parent, (Flags)(flags & ~(VERTICAL | HORIZONTAL))) {

    }

    Group::~Group() {

    }

    void Group::Draw() {
        Composite::Draw();
    }

    Button::Button(Composite* parent, Flags flags) : Control(parent, flags),  value(), callback() {

    }

    Button::~Button() {

    }

    void Button::Draw() {
        ImGui::PushID(this);
        if (flags & CHECKBOX) {
            if (ImGui::Checkbox(name.c_str(), &value) && callback)
                callback(this);
        }
        else if (flags & RADIOBUTTON) {
            if (ImGui::RadioButton(name.c_str(), value)) {
                if (callback)
                    callback(this);
                value = true;
            }
        }
        else
            if (ImGui::Button(name.c_str()) && callback)
                callback(this);
        ImGui::PopID();
    }

    void Button::AddSelectionListener(SelectionListener* value) {
        selection_listeners.push_back(value);
    }

    void Button::SetValue(bool value) {
        this->value = value;
        if (!(flags & RADIOBUTTON))
            return;

        for (Control* i : parent_comp->controls) {
            Button* button = (Button*)dynamic_cast<Button*>(i);
            if (button && button != this && button->flags & RADIOBUTTON)
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

        if (flags & CLOSE_BUTTON) {
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

        ImGui::SetNextWindowPos({ position.x, position.y }, ImGuiCond_Appearing);
        if (size != 0.0f)
            ImGui::SetNextWindowSize({ size.x, size.y }, ImGuiCond_Appearing);

        bool open = true;
        ImGui::PushID(this);
        if (ImGui::Begin(name.c_str(), close_button ? &open : 0))
            Composite::Draw();
        input_locked |= ImGui::IsWindowFocused();
        ImGui::End();
        ImGui::PopID();

        if (close_button && !open)
            close_button->callback(close_button);
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
        destroy = true;
    }

    void dw::Shell::SetSize(vec2 size) {
        dw::Widget::SetSize(size);
        if (flags & CHECKBOX) {
            if (close_button) {
                vec2 size = close_button->GetSize();
                close_button->position.x = this->size.x - size.x + 2.0f;
                close_button->position.y = 2.0f;
            }
        }

        rectangle bounding_box = GetBoundingBox();
        if (v_bar) {
            v_bar->position.x = bounding_box.pos.x + bounding_box.size.x - v_bar->size.x;
            v_bar->position.y = bounding_box.pos.y;
            v_bar->SetWidth(bounding_box.size.y);
        }

        layout->SetSize(this);
    }

    vec2 Shell::GetPos() {
        return position;
    }

    vec2 Shell::GetSize() {
        vec2 size = dw::Composite::GetSize();
        size.x += 2.0f * 2.0f;
        size.y += 2.0f * 2.0f;
        /*if (flags & CHECKBOX)
            size.y += p_dw__font_type_8x16_0_get_font_glyph_height() + 2.0f;*/
        return size;
    }

    rectangle dw::Shell::GetBoundingBox() {
        rectangle rect;
        rect.pos.x = 2.0;
        rect.pos.y = 2.0;
        rect.size.x = size.x - 2.0f * 2.0f;
        rect.size.y = size.y - 2.0f * 2.0f;

        /*if (flags & CHECKBOX) {
            rect.size.y -= p_dw__font_type_8x16_0_get_font_glyph_height() + 2.0f;
            rect.pos.y += p_dw__font_type_8x16_0_get_font_glyph_height() + 2.0f;
        }*/

        if (v_bar)
            rect.size.x -= v_bar->size.x;
        return rect;
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

    void Shell::sub_1402F38B0() {
        //sub_1402F3770();
        Disp();
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

    size_t List::GetItemCount() {
        return items.size();
    }

    void List::ResetSelectedItem() {
        if (flags & MULTISELECT)
            selected_item = -1;
        else
            for (int32_t& i : selections)
                i = 0;
    }

    void List::ResetSetSelectedItem(size_t index) {
        ResetSelectedItem();
        SetSelectedItem(index);
    }

    void List::SetItemIndex(size_t index) {
        hovered_item = index;
        ResetSetSelectedItem(index);
    }

    void List::SetSelectedItem(size_t index) {
        if (index < items.size())
            if (flags & MULTISELECT)
                selected_item = index;
            else
                selections[index] = 1;
    }

    ListBox::ListBox(Composite* parent, Flags flags) : Composite(parent, flags), list() {
        list = new List(parent, flags);
        list->SetName("ddl");
        list->parent = this;

        if (list->v_bar) {
            list->v_bar->parent = this;
            list->v_bar->AddSelectionListener(&drop_down_list_scroll_bar_selection_listener);
        }
    }

    ListBox::~ListBox() {
        delete list;
    }

    void ListBox::Draw() {
        bool window_focus = false;

        std::vector<std::string>& items = list->items;
        size_t hovered_item = list->selected_item;
        size_t selected_item = list->selected_item;
        if (size == 0.0f)
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
                i->Callback(this);
        }
        ImGui::PopID();

        input_locked |= window_focus;
    }

    void ListBox::Reset() {
        Composite::Reset();
    }

    Slider::Slider(Composite* parent, Flags flags) : Control(parent, flags) {
        format = "%f";
        scroll_bar = new ScrollBar(this, (Flags)(flags & (HORIZONTAL | VERTICAL)));
        scroll_bar->parent = this;
    }

    Slider::~Slider() {
        delete scroll_bar;
    }

    void Slider::Draw() {
        ScrollBar* scroll_bar = this->scroll_bar;
        ImGui::PushID(this);
        bool ret = ImGui::ColumnSliderFloatButton(name.c_str(), &scroll_bar->value,
            scroll_bar->step, scroll_bar->min, scroll_bar->max, scroll_bar->step_fast, format, 0);
        ImGui::PopID();
        if (ret)
            for (SelectionListener*& i : selection_listeners)
                i->Callback(this);
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

    Slider* Slider::make(Composite* parent, Flags flags,
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
        if (flags & HORIZONTAL) {
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
