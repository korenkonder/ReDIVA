/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../KKdLib/vec.hpp"
#include <vector>

namespace dw {
    class Widget {
    public:
        typedef void(*Callback)(Widget* data);

        enum Flags {
            WIDGET_RADIOBUTTON  = 0x10,
            WIDGET_CHECKBOX     = 0x20,
            WIDGET_CLOSE_BUTTON = 0x40,
        };

        union {
            int8_t i8;
            uint8_t u8;
            int16_t i16;
            uint16_t u16;
            int32_t i32;
            uint32_t u32;
            int64_t i64;
            uint64_t u64;
            void* v64;
        } callback_data;

        std::string name;
        vec2 position;
        vec2 size;
        Widget* parent;
        Flags flags;

        Widget(Widget* parent = 0, Flags flags = (Flags)0);
        virtual ~Widget();

        virtual void Draw();
        virtual void Reset();
        virtual std::string GetName();
        virtual void SetName(std::string& str);
        virtual void SetSize(vec2 value);
        virtual vec2 GetPos();

        void SetName(std::string&& str);
    };

    class SelectionListener {
    public:
        SelectionListener();
        virtual ~SelectionListener();
    };

    class SelectionAdapter : public SelectionListener {
    public:
        SelectionAdapter();
        virtual ~SelectionAdapter() override;
    };

    class SelectionListenerOnHook : public SelectionAdapter {
    public:
        Widget::Callback callback;

        SelectionListenerOnHook();
        SelectionListenerOnHook(Widget::Callback callback);
        virtual ~SelectionListenerOnHook() override;
    };

    class Composite;
    class Shell;

    class Control : public Widget {
    public:
        Composite* parent_comp;
        Shell* parent_shell;
        //Menu* parent_menu;

        Control(Composite* parent = 0, Flags flags = (Flags)0);
        virtual ~Control() override;

        virtual void Draw() override;
        virtual void Reset() override;
    };

    class Label : public Control {
    public:
        Label(Composite* parent = 0, Flags flags = (Flags)0);
        virtual ~Label() override;

        virtual void Draw() override;
    };

    class Scrollable : public Control {
    public:
        Scrollable(Composite* parent = 0, Flags flags = (Flags)0);
        virtual ~Scrollable() override;

        virtual void Draw() override;
        virtual void Reset() override;
    };

    class Layout {
    public:
        Layout();
        virtual ~Layout();

        virtual vec2 GetSize(dw::Composite* comp) = 0;
        virtual void SetSize(dw::Composite* comp) = 0;
    };

    class FillLayout : public Layout {
    public:
        int32_t field_8;

        FillLayout(int32_t a2 = 512);
        virtual ~FillLayout() override;

        virtual vec2 GetSize(dw::Composite* comp) override;
        virtual void SetSize(dw::Composite* comp) override;
    };

    class GraphLayout : public Layout {
    public:
        int32_t field_8;

        GraphLayout(int32_t a2 = 512);
        virtual ~GraphLayout() override;

        virtual vec2 GetSize(dw::Composite* comp) override;
        virtual void SetSize(dw::Composite* comp) override;
    };

    class GridLayout : public Layout {
    public:
        __int64 field_8;
        __int64 field_10;
        __int64 field_18;
        __int64 field_20;
        __int64 field_28;
        __int64 field_30;
        __int64 field_38;
        size_t field_40;
        float_t field_48;
        float_t field_4C;

        GridLayout(size_t a2 = 2);
        virtual ~GridLayout() override;

        virtual vec2 GetSize(dw::Composite* comp) override;
        virtual void SetSize(dw::Composite* comp) override;
    };

    class RowLayout : public Layout {
    public:
        int32_t field_8;
        float_t field_C;

        RowLayout(int32_t a2 = 256);
        virtual ~RowLayout() override;

        virtual vec2 GetSize(dw::Composite* comp) override;
        virtual void SetSize(dw::Composite* comp) override;
    };

    class Composite : public Scrollable {
    public:
        dw::Layout* layout;
        std::vector<dw::Control*> controls;

        Composite(Composite* parent = 0, Flags flags = (Flags)0);
        virtual ~Composite() override;

        virtual void Draw() override;
        virtual void Reset() override;

        void SetLayout(dw::Layout* value);
    };

    class Button : public Control {
    public:
        bool value;
        std::vector<SelectionListener*> selection_listeners;
        Widget::Callback callback;

        Button(Composite* parent = 0, Flags flags = (Flags)0);
        virtual ~Button() override;

        virtual void Draw() override;

        void SetValue(bool value);
    };

    class Shell : public Composite {
    public:
        bool disp;

        Shell(Composite* parent = 0, Flags flags
            = (dw::Widget::Flags)(0x480 | WIDGET_CLOSE_BUTTON | WIDGET_CHECKBOX | WIDGET_RADIOBUTTON));
        virtual ~Shell() override;

        virtual void Draw() override;

        virtual void Hide();

        void Disp();
        bool GetDisp();
        void SetDisp(bool value);
    };

    class List : public Scrollable {
    public:
        std::vector<std::string> items;
        size_t hovered_item;
        size_t selected_item;
        std::vector<SelectionListener*> selection_listeners;

        List(Composite* parent = 0, Flags flags = (Flags)0);
        virtual ~List() override;

        virtual void Draw() override;
        virtual void Reset() override;

        void AddItem(const std::string& str);
        void AddItem(const std::string&& str);
        void AddSelectionListener(SelectionListener* value);
        void ClearItems();
        std::string GetItem(size_t index) const;
    };
    
    class ListBox : public Composite {
    public:
        dw::List* list;

        ListBox(Composite* parent = 0, Flags flags = (Flags)0x204);
        virtual ~ListBox() override;

        virtual void Draw() override;
        virtual void Reset() override;

        void AddItem(const std::string& str);
        void AddItem(const std::string&& str);
        void AddSelectionListener(SelectionListener* value);
        void ClearItems();
        std::string GetItem(size_t index) const;
    };
}
