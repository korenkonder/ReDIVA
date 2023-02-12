/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "dw.hpp"
#include "imgui_helper.hpp"

namespace dw {
    SelectionListener::SelectionListener() {

    }
    
    SelectionListener::~SelectionListener() {

    }
    
    SelectionAdapter::SelectionAdapter() {

    }
    
    SelectionAdapter::~SelectionAdapter() {

    }
    
    SelectionListenerOnHook::SelectionListenerOnHook() : callback() {

    }
    
    SelectionListenerOnHook::SelectionListenerOnHook(Widget::Callback callback) {
        this->callback = callback;
    }
    
    SelectionListenerOnHook::~SelectionListenerOnHook() {

    }

    Widget::Widget(Widget* parent, Flags flags) : callback_data(), parent() {
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

    void Widget::SetName(std::string&& str) {
        SetName(str);
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

    Scrollable::Scrollable(Composite* parent, Flags flags) : Control(parent, flags) {

    }
    
    Scrollable::~Scrollable() {

    }

    void Scrollable::Draw() {

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

    vec2 FillLayout::GetSize(dw::Composite* comp) {
        return comp->size;
    }

    void FillLayout::SetSize(dw::Composite* comp) {
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

    vec2 GraphLayout::GetSize(dw::Composite* comp) {
        return comp->size;
    }

    void GraphLayout::SetSize(dw::Composite* comp) {
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

    vec2 GridLayout::GetSize(dw::Composite* comp) {
        return comp->size;
    }

    void GridLayout::SetSize(dw::Composite* comp) {
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

    vec2 RowLayout::GetSize(dw::Composite* comp) {
        return comp->size;
    }

    void RowLayout::SetSize(dw::Composite* comp) {
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
            //dw::Widget::sub_1402ECDE0(i);

        controls.clear();
        Scrollable::Reset();
    }

    void Composite::SetLayout(dw::Layout* value) {
        layout = value;
    }

    Button::Button(Composite* parent, Flags flags) : Control(parent, flags),  value(), callback() {

    }
    
    Button::~Button() {

    }

    void Button::Draw() {
        if (flags & WIDGET_CHECKBOX)
            ImGui::Checkbox(name.c_str(), &value);
        else if (flags & WIDGET_RADIOBUTTON) {
            if (ImGui::RadioButton(name.c_str(), value)) {
                if (callback)
                    callback(this);
                value = true;
            }
        }
        else
            ImGui::Button(name.c_str());
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

    Shell::Shell(Composite* parent, Flags flags) : Composite(parent, flags), disp() {

    }

    Shell::~Shell() {

    }

    void Shell::Draw() {
        if (!GetDisp())
            return;

        Composite::Draw();
    }

    void Shell::Hide() {

    }

    void Shell::Disp() {
        SetDisp(true);
    }

    bool Shell::GetDisp() {
        return disp;
    }

    void Shell::SetDisp(bool value) {
        if (disp == value)
            return;

        disp = value;
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

    ListBox::ListBox(Composite* parent, Flags flags) : Composite(parent, flags), list() {
        list = new List(parent, flags);
    }

    ListBox::~ListBox() {
        delete list;
    }

    void ListBox::Draw() {

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
}
