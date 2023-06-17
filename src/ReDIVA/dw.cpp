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
        Free();
    }

    void Widget::Draw() {

    }

    void Widget::Reset() {
        text.clear();
    }

    std::string Widget::GetText() {
        return text;
    }

    void Widget::SetText(std::string& str) {
        text.assign(str);
        if (flags & LABEL_SIZE) {
            ImVec2 size = ImGui::CalcTextSize(text.c_str(), 0, true);
            rect.size = { size.x, size.y };
        }
    }

    void Widget::SetSize(vec2 value) {
        rect.size = value;
    }

    vec2 Widget::GetPos() {
        return rect.pos;
    }

    void Widget::Free() {
        if (free)
            return;

        Reset();

        if (free_callback)
            free_callback(this);

        free = true;

        if (dw_gui_detail_display)
            dw_gui_detail_display->free_widgets.push_back(this);
    }

    rectangle Widget::GetRectangle() {
        return { GetPos(), rect.size };
    }

    void Widget::SetText(std::string&& str) {
        SetText(str);
    }

    void Widget::UpdateDraw() {
        //print->sub_140302800();
        Draw();
    }

    Control::Control(Composite* parent, Flags flags) : Widget(parent, flags),
        parent_comp(), parent_shell()/*, parent_menu()*/ {
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

    vec2 Control::GetPos() {
        vec2 pos = rect.pos;
        if (parent_comp) {
            rectangle parent_comp_bb = parent_comp->GetBoundingBox();
            pos += parent_comp->GetPos() + parent_comp_bb.pos;
            if (parent_comp->v_bar)
                pos.y -= parent_comp->v_bar->value;
        }
        return pos;
    }

    void Control::GetSetSize() {
        SetSize(GetSize());
    }

    vec2 Control::GetSize() {
        return rect.size;
    }

    Label::Label(Composite* parent, Flags flags) : Control(parent, flags) {

    }

    Label::~Label() {

    }

    void Label::Draw() {
        rectangle rect = GetRectangle();
        //print->SetFont(font);
        //vec2 text_size = print->GetTextSize(name);
        ImVec2 _text_size = ImGui::CalcTextSize(text.c_str());
        vec2 text_size(_text_size.x, _text_size.y);

        vec2 pos;
        if (flags & FLAG_4000)
            pos.x = rect.pos.x;
        else if (flags & FLAG_20000)
            pos.x = rect.pos.x + rect.size.x - text_size.x;
        else
            pos.x = rect.pos.x + (rect.size.x - text_size.x) * 0.5f;
        pos.y = rect.pos.y + (rect.size.y - text_size.y) * 0.5f;

        /*print->SetColor(Field_A0()
            ? dw_gui_colors_current.foreground
            : &dw_gui_colors_current.disable_foreground);
        print->sub_1403027C0(rect);
        print->SetFont(font);
        print->PrintText(name, pos.x, pos.y);*/

        mat4_mult_vec2(&mat4_identity, &pos, &pos);

        ImGui::PushID(this);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (pos.x - rect.pos.x));
        ImGui::Text(text.c_str());
        ImGui::PopID();
    }

    vec2 Label::GetSize() {
        //sub_140302810(qword_1411900C8, &font);
        //vec2 size = sub_140302A70(qword_1411900C8, text);
        ImVec2 _size = ImGui::CalcTextSize(text.c_str(), 0, true);
        vec2 size(_size.x, _size.y);
        return size + 2.0f * 2.0f;
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

    void ScrollBar::SetSize(vec2 size) {
        if (flags & HORIZONTAL) {
            rect.size.x = size.x;
            //rect.size.y = font->GetFontGlyphHeight();
            rect.size.y = ImGui::CalcEmptyGlyphHeight();
        }
        else {
            //rect.size.x = font->GetFontGlyphWidth();
            rect.size.x = ImGui::CalcEmptyGlyphHeight();
            rect.size.y = size.y;
        }
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

    void ScrollBar::SetParams(float_t size, float_t step, size_t items_count) {
        if (!items_count)
            return;

        float_t v5 = (float_t)(int64_t)items_count;
        if (v5 < 0.0f)
            v5 += (float_t)UINT64_MAX;

        float_t v6 = v5 * step;
        if (v6 > size) {
            min = 0.0f;
            max = v6 - size;
            field_94 = (size / v6) * (v6 - size);
            dw::ScrollBar::SetValue(value);
            this->step = step;

            int64_t v8 = 0;
            float_t v9 = size / step;
            if (v9 >= INT64_MAX) {
                v9 -= INT64_MAX;
                if (v9 < INT64_MAX)
                    v8 = INT64_MIN;
            }

            int64_t v10 = (int64_t)v9;
            float_t v11 = (float_t)(v8 + v10);
            if (v11 < 0.0f)
                v11 += UINT64_MAX;
            step_fast = v11 * step;
        }
        else {
            min = 0.0f;
            max = 0.0f;
            field_94 = v6;
            SetValue(value);
            this->step = step;
            step_fast = v6;
        }
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
            rect.size.x = value;
            rect.size.y = glyph_size.y;
        }
        else {
            rect.size.x = glyph_size.x;
            rect.size.y = value;
        }
    }

    void ScrollBar::AddSelectionListener(SelectionListener* value) {
        selection_listeners.push_back(value);
    }

    Scrollable::Scrollable(Composite* parent, Flags flags) : Control(parent, flags), h_bar(), v_bar() {
        if (flags & HORIZONTAL) {
            h_bar = new ScrollBar(parent, HORIZONTAL);
            h_bar->SetText("h_bar");
        }

        if (flags & VERTICAL) {
            v_bar = new ScrollBar(parent, VERTICAL);
            v_bar->SetText("v_bar");
        }
    }

    Scrollable::~Scrollable() {

    }

    void Scrollable::Draw() {
        if (v_bar)
            v_bar->UpdateDraw();
    }

    void Scrollable::Reset() {
        if (h_bar) {
            h_bar->Free();
            h_bar = 0;
        }

        if (v_bar) {
            v_bar->Free();
            v_bar = 0;
        }

        Control::Reset();
    }

    void Scrollable::SetSize(vec2 value) {
        Widget::SetSize(value);

        if (flags & FLAG_800) {
            value.x -= 2.0f * 2.0f;
            value.y -= 2.0f * 2.0f;
        }

        if (v_bar) {
            v_bar->rect.pos.x = value.x - v_bar->rect.size.x;
            v_bar->rect.pos.y = 0.0f;
            v_bar->SetWidth(value.y);
        }
    }

    rectangle Scrollable::GetBoundingBox() {
        rectangle rect;
        rect.size = this->rect.size;

        if (flags & FLAG_800) {
            rect.pos = 2.0f;
            rect.size -= 2.0f * 2.0f;
        }

        if (v_bar)
            rect.size.x -= v_bar->rect.size.x;
        return rect;
    }

    Layout::Layout() {

    }

    Layout::~Layout() {

    }

    FillLayout::FillLayout(Flags flags) {
        this->flags = flags;
    }

    FillLayout::~FillLayout() {

    }

    vec2 FillLayout::GetSize(Composite* comp) {
        if (!comp->controls.size())
        {
            vec2 size = comp->rect.size;
            if (comp->v_bar)
                size.x += comp->v_bar->rect.size.x;
            return size;
        }

        vec2 size = 0.0f;
        if (flags == VERTICAL)
            for (Control* i : comp->controls) {
                vec2 _size = i->GetSize();
                size.x = max_def(_size.x, size.x);
            }
        else
            for (Control* i : comp->controls) {
                vec2 _size = i->GetSize();
                size.y = max_def(_size.y, size.y);
            }

        if (comp->v_bar)
            size.x += comp->v_bar->rect.size.x;
        return size;
    }

    void FillLayout::SetSize(Composite* comp) {
        rectangle comp_bb = comp->GetBoundingBox();
        vec2 comp_bb_size = comp_bb.size;
        if (!comp->controls.size())
            return;

        float_t size = (float_t)(int64_t)comp->controls.size();
        if (size < 0.0f)
            size += (float_t)UINT64_MAX;

        if (flags == VERTICAL) {
            float_t pos = 0.0f;
            size = comp_bb_size.y / size;
            for (Control* i : comp->controls) {
                i->rect.pos = { 0.0f, pos };
                i->SetSize({ comp_bb_size.x, size });
                pos += size;
            }
        }
        else {
            float_t pos = 0.0f;
            size = comp_bb_size.x / size;
            for (Control* i : comp->controls) {
                i->rect.pos = { pos, 0.0f };
                i->SetSize({ size, comp_bb_size.y });
                pos += size;
            }
        }
    }

    void FillLayout::Begin() {

    }

    void FillLayout::Next() {

    }

    void FillLayout::End() {

    }

    GraphLayout::GraphLayout(Flags flags) {
        this->flags = flags;
    }

    GraphLayout::~GraphLayout() {

    }

    vec2 GraphLayout::GetSize(Composite* comp) {
        return comp->rect.size;
    }

    void GraphLayout::SetSize(Composite* comp) {
        const rectangle rect = comp->rect;
        for (Control* i : comp->controls) {
            i->rect.pos = rect.pos;
            i->SetSize(rect.size);
        }
    }

    void GraphLayout::Begin() {

    }

    void GraphLayout::Next() {

    }

    void GraphLayout::End() {

    }

    GridLayout::GridLayout(size_t columns) : rows() {
        this->columns = columns;
        spacing = 2.0f;
    }

    GridLayout::~GridLayout() {

    }

    vec2 GridLayout::GetSize(Composite* comp) {
        std::vector<Control*>& controls = comp->controls;
        size_t controls_count = controls.size();
        if (!controls_count) {
            vec2 size = comp->rect.size;
            if (comp->v_bar)
                size.x += comp->v_bar->rect.size.x;
            return size;
        }

        rows = (controls_count + columns - 1) / columns;
        rows = rows ? rows : 1;

        column_size.resize(columns);
        row_size.resize(rows);

        size_t control = 0;
        for (size_t row = 0; row < rows && control < controls_count; row++)
            for (size_t column = 0; column < columns && control < controls_count; column++, control++) {
                vec2 size = controls.data()[control]->GetSize();
                column_size.data()[column] = max_def(size.x, column_size.data()[column]);
                row_size.data()[row] = max_def(size.y, row_size.data()[row]);
            }

        vec2 size;
        size.x = (float_t)(int64_t)(columns - 1);
        if (size.x < 0.0f)
            size.x += (float_t)UINT64_MAX;

        size.y = (float_t)(int64_t)(rows - 1);
        if (size.y < 0.0f)
            size.y += (float_t)UINT64_MAX;

        size *= spacing;

        float_t size_x = size.x;
        for (float_t i : column_size)
            size_x += i;
        size.x = size_x;

        float_t size_y = size.y;
        for (float_t i : row_size)
            size_y += i;
        size.y = size_y;

        if (comp->v_bar)
            size.x += comp->v_bar->rect.size.x;
        return size;
    }

    void GridLayout::SetSize(Composite* comp) {
        std::vector<Control*>& controls = comp->controls;
        size_t controls_count = controls.size();

        rectangle comp_bb = comp->GetBoundingBox();
        vec2 comp_bb_size = comp_bb.size;
        if (!controls_count)
            return;

        if (controls_count == 1) {
            Scrollable* scrollable = dynamic_cast<Scrollable*>(comp->controls.front());
            if (scrollable) {
                scrollable->rect.pos = 0.0f;
                scrollable->SetSize(comp_bb.size);
                return;
            }
        }

        size_t control = 0;
        vec2 pos = 0.0f;
        for (size_t row = 0; row < rows && control < controls_count; row++) {
            vec2 _pos = pos;
            for (size_t column = 0; column < columns && control < controls_count; column++, control++) {
                comp->controls.data()[control]->rect.pos = _pos;
                comp->controls.data()[control]->SetSize({ column_size.data()[column], row_size.data()[row] });
                _pos.x += column_size.data()[column] + spacing.x;
            }
            pos.y += row_size.data()[row] + spacing.y;
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
        if (!comp->controls.size()) {
            vec2 size = comp->rect.size;
            if (comp->v_bar)
                size.x += comp->v_bar->rect.size.x;
            return size;
        }

        vec2 size = 0.0f;
        if (flags == VERTICAL) {
            for (Control* i : comp->controls) {
                vec2 _size = i->GetSize();
                size.x = max_def(_size.x, size.x);
                size.y += _size.y + spacing;
            }
            size.y -= spacing;
        }
        else {
            for (Control* i : comp->controls) {
                vec2 _size = i->GetSize();
                size.x += _size.x + spacing;
                size.y = max_def(_size.y, size.y);
            }
            size.x -= spacing;
        }

        if (comp->v_bar)
            size.x += comp->v_bar->rect.size.x;
        return size;
    }

    void RowLayout::SetSize(Composite* comp) {
        rectangle comp_bb = comp->GetBoundingBox();
        vec2 comp_bb_size = comp_bb.size;
        if (!comp->controls.size())
            return;

        if (comp->controls.size() == 1) {
            Scrollable* scrollable = dynamic_cast<Scrollable*>(comp->controls.front());
            if (scrollable) {
                scrollable->rect.pos = 0.0f;
                scrollable->SetSize(comp_bb.size);
                return;
            }
        }

        float_t pos = 0.0f;
        if (flags == VERTICAL)
            for (Control*& i : comp->controls) {
                i->rect.pos = { 0.0f, pos };
                float_t size_y = i->GetSize().y;
                i->SetSize({ comp_bb_size.x, size_y });
                pos += size_y + spacing;
            }
        else
            for (Control*& i : comp->controls) {
                i->rect.pos = { pos, 0.0f };
                float_t size_x = i->GetSize().x;
                i->SetSize({ size_x, comp_bb_size.y });
                pos += size_x + spacing;
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
            if (i->rect.size != 0.0f)
                ImGui::SetNextItemWidth(i->rect.size.x);
            i->UpdateDraw();
            if (layout)
                layout->Next();
        }

        if (layout)
            layout->End();
        ImGui::PopID();
    }

    void Composite::Reset() {
        for (Control*& i : controls)
            i->Free();
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
        GetSetSize();
    }

    Button::~Button() {

    }

    void Button::Draw() {
        ImGui::PushID(this);
        if (flags & CHECKBOX) {
            bool value = this->value;
            if (ImGui::Checkbox(text.c_str(), &value)) {
                SetValue(value);
                if (callback)
                    callback(this);
            }
        }
        else if (flags & RADIOBUTTON) {
            if (ImGui::RadioButton(text.c_str(), value)) {
                SetValue(value);
                if (callback)
                    callback(this);
            }
        }
        else {
            if (ImGui::Button(text.c_str())) {
                if (callback)
                    callback(this);
            }
        }
        ImGui::PopID();
    }

    vec2 Button::GetSize() {
        //print->SetFont(font);
        //vec2 size = print->GetTextSize(text);
        ImVec2 _size = ImGui::CalcTextSize(text.c_str(), 0, true);
        vec2 size = vec2(_size.x, _size.y);

        if (flags & FLAG_8) {
            size.x += 2.0f;
            size.x += 2.0f * 2.0f;
            size.y += 2.0f * 2.0f;
        }
        else if (flags & RADIOBUTTON) {
            /*print->SetFont(font);
            std::string text("(X)");
            vec2 size = print->GetTextSize(text);
            ImVec2 _size = ImGui::CalcTextSize(text.c_str(), 0, true);
            vec2 radiobutton_size = vec2(_size.x, _size.y);
            size.x += radiobutton_size.x*/

            ImGuiStyle& style = ImGui::GetCurrentContext()->Style;
            size.x += ImGui::GetFrameHeight() + style.ItemInnerSpacing.x;
        }
        else if (flags & CHECKBOX) {
            /*print->SetFont(font);
            std::string text("[X]");
            vec2 size = print->GetTextSize(text);
            ImVec2 _size = ImGui::CalcTextSize(text.c_str(), 0, true);
            vec2 checkbox_size = vec2(_size.x, _size.y);
            size.x += checkbox_size.x*/

            ImGuiStyle& style = ImGui::GetCurrentContext()->Style;
            size.x += ImGui::GetFrameHeight() + style.ItemInnerSpacing.x;
        }
        return size;
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
        SetText("Close Box");
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

        ImGui::SetNextWindowPos({ rect.pos.x, rect.pos.y }, ImGuiCond_Appearing);
        if (rect.size != 0.0f)
            ImGui::SetNextWindowSize({ rect.size.x, rect.size.y }, ImGuiCond_Appearing);

        bool open = true;
        ImGui::PushID(this);
        if (ImGui::Begin(text.c_str(), close_button ? &open : 0))
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

    void Shell::SetSize(vec2 size) {
        Widget::SetSize(size);
        if (flags & CHECKBOX) {
            if (close_button) {
                vec2 size = close_button->GetSize();
                close_button->rect.pos.x = rect.size.x - size.x + 2.0f;
                close_button->rect.pos.y = 2.0f;
            }
        }

        rectangle bounding_box = GetBoundingBox();
        if (v_bar) {
            v_bar->rect.pos.x = bounding_box.pos.x + bounding_box.size.x - v_bar->rect.size.x;
            v_bar->rect.pos.y = bounding_box.pos.y;
            v_bar->SetWidth(bounding_box.size.y);
        }

        layout->SetSize(this);
    }

    vec2 Shell::GetPos() {
        return rect.pos;
    }

    vec2 Shell::GetSize() {
        vec2 size = Composite::GetSize();
        size.x += 2.0f * 2.0f;
        size.y += 2.0f * 2.0f;
        /*if (flags & CHECKBOX)
            size.y += p_dw__font_type_8x16_0_get_font_glyph_height() + 2.0f;*/
        return size;
    }

    rectangle Shell::GetBoundingBox() {
        rectangle rect;
        rect.pos = 2.0f;
        rect.size = this->rect.size - 2.0f * 2.0f;

        /*if (flags & CHECKBOX) {
            rect.size.y -= p_dw__font_type_8x16_0_get_font_glyph_height() + 2.0f;
            rect.pos.y += p_dw__font_type_8x16_0_get_font_glyph_height() + 2.0f;
        }*/

        if (v_bar)
            rect.size.x -= v_bar->rect.size.x;
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

    void List::SetSize(vec2 size) {
        Scrollable::SetSize(size);
        SetScrollBarParams();
    }
    
    vec2 List::GetSize() {
        //print->SetFont();

        float_t max_text_size = 0.0f;
        for (std::string& i : items) {
            //vec2 text_size = print->GetTextSize(i);
            ImVec2 _text_size = ImGui::CalcTextSize(i.c_str());
            vec2 text_size(_text_size.x, _text_size.y);
            max_text_size = max_def(text_size.x, max_text_size);
        }

        size_t v7 = sub_1402EF620();
        size_t v8 = items.size();
        //float_t v12 = font->GetFontGlyphHeight();
        float_t v12 = ImGui::CalcEmptyGlyphHeight();
        float_t v14 = (float_t)(int64_t)min_def(v7, v8);
        if (v14 < 0.0f)
            v14 += (float_t)UINT64_MAX;

        vec2 size = { max_text_size, v12 * v14 };
        if (v_bar)
            size.x += v_bar->rect.size.x;

        if (flags & FLAG_800)
            size += 2.0f * 2.0f;
        return size;
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
        hovered_item = -1;
        selected_item = -1;
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

    void List::SetScrollBarParams() {
        rectangle bb = GetBoundingBox();
        if (v_bar)
            //v_bar->SetParams(bb.size.y, font->GetFontGlyphHeight(), items.size());
            v_bar->SetParams(bb.size.y, ImGui::CalcEmptyGlyphHeight(), items.size());
    }

    void List::SetSelectedItem(size_t index) {
        if (index < items.size())
            if (flags & MULTISELECT)
                selected_item = index;
            else
                selections[index] = 1;
    }

    size_t List::sub_1402EF620() {
        if (!v_bar)
            return items.size();       
        /*else if (field_140 >= this->items.size())
            return items.size();
        return field_140;*/
        return 0;
    }

    ListBox::ListBox(Composite* parent, Flags flags) : Composite(parent, flags), list() {
        list = new List(parent, flags);
        list->SetText("ddl");
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
        ImGui::SetNextItemWidth(rect.size.x);
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
                    list->selected_item = -1;
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
        ImGui::PopID();

        if (list->hovered_item != hovered_item)
            list->hovered_item = hovered_item;

        if (list->selected_item != selected_item) {
            list->selected_item = selected_item;
            for (SelectionListener*& i : list->selection_listeners)
                i->Callback(this);
        }

        input_locked |= window_focus;
    }

    void ListBox::Reset() {
        Composite::Reset();
    }

    vec2 ListBox::GetSize() {
        vec2 size = list->GetSize();
        //size.x += font->GetFontGlyphWidth();
        //size.y = font->GetFontGlyphHeight();
        ImVec2 _size = ImGui::CalcEmptyGlyphSize();
        size.x += _size.x;
        size.y = _size.y;
        return size + 2.0f * 2.0f;
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
        bool ret = ImGui::ColumnSliderFloatButton(text.c_str(), &scroll_bar->value,
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
        scroll_bar->rect.pos.x = value.x - scroll_bar->rect.size.x;
        scroll_bar->rect.pos.y = 0.0f;
    }

    void Slider::AddSelectionListener(SelectionListener* value) {
        selection_listeners.push_back(value);
    }

    Slider* Slider::make(Composite* parent, Flags flags,
        float_t pos_x, float_t pos_y, float_t width, float_t height, const char* text) {
        Slider* slider = new Slider(parent, flags);
        ScrollBar* scroll_bar = slider->scroll_bar;
        slider->SetText(text);
        scroll_bar->SetText("slider's bar");
        scroll_bar->SetSize({ width, height });
        //print->SetFont(font);
        vec2 size;
        if (flags & HORIZONTAL) {
            //vec2 v21 = print->GetTextSize(slider->text);
            //vec2 v22 = print->GetTextSize("0000");
            ImVec2 _v21 = ImGui::CalcTextSize(slider->text.c_str());
            ImVec2 _v22 = ImGui::CalcTextSize("0000");
            vec2 v21 = { _v21.x, _v21.y };
            vec2 v22 = { _v22.x, _v22.y };
            scroll_bar->rect.pos.x = v21.x + v22.x;
            scroll_bar->rect.pos.y = 0.0f;
            size.x = v21.x + v22.x + scroll_bar->rect.size.x;
            //size.y = p_dw__Font::GetFontGlyphHeight(&slider->font);
            size.y = ImGui::CalcEmptyGlyphHeight();
        }
        else {
            //vec2 v21 = print->GetTextSize(slider->text);
            //vec2 v22 = print->GetTextSize("0000");
            ImVec2 _v21 = ImGui::CalcTextSize(slider->text.c_str());
            ImVec2 _v22 = ImGui::CalcTextSize("0000");
            vec2 v21 = { _v21.x, _v21.y };
            vec2 v22 = { _v22.x, _v22.y };
            //size.x = p_dw__Font::GetFontGlyphWidth(&slider->font);
            size.x = ImGui::CalcEmptyGlyphHeight();
            size.y = v21.x + v22.x + scroll_bar->rect.size.y;
        }
        slider->rect.pos = { pos_x, pos_y };
        slider->SetSize(size);
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
