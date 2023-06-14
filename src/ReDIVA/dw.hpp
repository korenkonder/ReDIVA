/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../KKdLib/vec.hpp"
#include "../CRE/sprite.hpp"
#include <vector>

namespace dw {
    enum Flags {
        FLAG_1       = 0x0001,
        FLAG_2       = 0x0002,
        MULTISELECT  = 0x0004,
        FLAG_8       = 0x0008,
        RADIOBUTTON  = 0x0010,
        CHECKBOX     = 0x0020,
        CLOSE_BUTTON = 0x0040,
        FLAG_80      = 0x0080,
        HORIZONTAL   = 0x0100,
        VERTICAL     = 0x0200,
        FLAG_400     = 0x0400,
        FLAG_800     = 0x0800,
        FLAG_1000    = 0x1000,
        FLAG_2000    = 0x2000,
        FLAG_4000    = 0x4000,

        // Added
        LABEL_SIZE   = 0x8000,
    };

    class Widget {
    public:
        typedef void(*Callback)(Widget* data);

        bool free;

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

        Callback free_callback;
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

        void Free();
        void SetName(std::string&& str);
        void UpdateDraw();
    };

    class SelectionListener {
    public:
        SelectionListener();
        virtual ~SelectionListener();

        virtual void Callback(Widget* data) = 0;
        virtual void Field_10(Widget* data) = 0;
    };

    class SelectionAdapter : public SelectionListener {
    public:
        SelectionAdapter();
        virtual ~SelectionAdapter() override;

        virtual void Callback(Widget* data) override;
        virtual void Field_10(Widget* data) override;
    };

    class SelectionListenerOnHook : public SelectionAdapter {
    public:
        Widget::Callback callback;

        SelectionListenerOnHook();
        SelectionListenerOnHook(Widget::Callback callback);
        virtual ~SelectionListenerOnHook() override;

        virtual void Callback(Widget* data) override;
    };

    class DropDownListScrollBarSelectionListener : public SelectionAdapter {
    public:
        DropDownListScrollBarSelectionListener();
        virtual ~DropDownListScrollBarSelectionListener() override;

        virtual void Callback(Widget* data) override;
    };
    
    class ScrollBarTestSelectionListener : public SelectionAdapter {
    public:
        ScrollBarTestSelectionListener();
        virtual ~ScrollBarTestSelectionListener() override;

        virtual void Callback(Widget* data) override;
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
        virtual vec2 GetPos() override;

        virtual void GetSetSize();
        virtual vec2 GetSize();
    };

    class Label : public Control {
    public:
        Label(Composite* parent = 0, Flags flags = (Flags)0);
        virtual ~Label() override;

        virtual void Draw() override;
    };

    class ScrollBar : public Widget {
    public:
        Control* field_68;
        std::vector<SelectionListener*> selection_listeners;
        float_t value;
        float_t min;
        float_t max;
        float_t field_94;
        float_t step;
        float_t step_fast;
        int field_A0;
        bool round;
        char field_A5;
        char field_A6;
        __int64 field_A8;
        __int64 field_B0;
        __int64 field_B8;
        __int64 field_C0;
        __int64 field_C8;
        __int64 field_D0;
        __int64 field_D8;
        __int64 field_E0;
        __int64 field_E8;
        __int64 field_F0;
        __int64 field_F8;
        __int64 field_100;
        __int64 field_108;
        __int64 field_110;
        __int64 field_118;
        __int64 field_120;
        __int64 field_128;
        __int64 field_130;
        __int64 field_138;
        __int64 field_140;
        __int64 field_148;
        __int64 field_150;
        __int64 field_158;
        __int64 field_160;

        ScrollBar(Control* parent, Flags flags);
        virtual ~ScrollBar() override;

        virtual void Draw() override;
        virtual void SetSize(vec2 value) override;

        void SetParams(float_t value, float_t min, float_t max,
            float_t a5, float_t step, float_t step_fast);
        void SetValue(float_t value);
        void SetWidth(float_t value);

        void AddSelectionListener(SelectionListener* value);
    };

    class Scrollable : public Control {
    public:
        ScrollBar* h_bar;
        ScrollBar* v_bar;

        Scrollable(Composite* parent = 0, Flags flags = (Flags)0);
        virtual ~Scrollable() override;

        virtual void Draw() override;
        virtual void Reset() override;
        virtual void SetSize(vec2 value) override;

        virtual rectangle GetBoundingBox();
    };

    class Layout {
    public:
        Layout();
        virtual ~Layout();

        virtual vec2 GetSize(Composite* comp) = 0;
        virtual void SetSize(Composite* comp) = 0;

        // Added
        virtual void Begin() = 0;
        virtual void Next() = 0;
        virtual void End() = 0;
    };

    class FillLayout : public Layout {
    public:
        int32_t field_8;

        FillLayout(int32_t a2 = 512);
        virtual ~FillLayout() override;

        virtual vec2 GetSize(Composite* comp) override;
        virtual void SetSize(Composite* comp) override;

        // Added
        virtual void Begin() override;
        virtual void Next() override;
        virtual void End() override;
    };

    class GraphLayout : public Layout {
    public:
        int32_t field_8;

        GraphLayout(int32_t a2 = 512);
        virtual ~GraphLayout() override;

        virtual vec2 GetSize(Composite* comp) override;
        virtual void SetSize(Composite* comp) override;

        // Added
        virtual void Begin() override;
        virtual void Next() override;
        virtual void End() override;
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

        virtual vec2 GetSize(Composite* comp) override;
        virtual void SetSize(Composite* comp) override;

        // Added
        virtual void Begin() override;
        virtual void Next() override;
        virtual void End() override;
    };

    class RowLayout : public Layout {
    public:
        Flags flags;
        float_t spacing;

        // Added
        vec2 imgui_cursor_pos;
        vec2 imgui_curr_line_size;
        float_t imgui_curr_line_text_base_offset;

        RowLayout(Flags flags = VERTICAL);
        virtual ~RowLayout() override;

        virtual vec2 GetSize(Composite* comp) override;
        virtual void SetSize(Composite* comp) override;

        // Added
        virtual void Begin() override;
        virtual void Next() override;
        virtual void End() override;
    };

    class Composite : public Scrollable {
    public:
        Layout* layout;
        std::vector<Control*> controls;

        Composite(Composite* parent = 0, Flags flags = (Flags)0);
        virtual ~Composite() override;

        virtual void Draw() override;
        virtual void Reset() override;
        virtual void SetSize(vec2 size) override;

        virtual void GetSetSize() override;
        virtual vec2 GetSize()  override;

        void SetLayout(Layout* value);
    };

    class Group : public Composite {
    public:
        Group(Composite* parent = 0, Flags flags = (Flags)0);
        virtual ~Group() override;

        virtual void Draw() override;
    };

    class Button : public Control {
    public:
        bool value;
        std::vector<SelectionListener*> selection_listeners;
        Widget::Callback callback;

        Button(Composite* parent = 0, Flags flags = (Flags)0);
        virtual ~Button() override;

        virtual void Draw() override;

        void AddSelectionListener(SelectionListener* value);
        void SetValue(bool value);
    };

    class ShellCloseButton : public Button {
    public:
        ShellCloseButton(dw::Shell* parent);
        virtual ~ShellCloseButton() override;

        virtual void Draw() override;

        static void Callback(dw::Widget* data);
    };

    class Shell : public Composite {
    public:
        typedef void(*Callback)(Shell* data);

        bool disp;
        Callback disp_callback;
        Callback hide_callback;
        bool destroy;
        ShellCloseButton* close_button;
        std::vector<Widget*> children;
        //std::vector<Menu*> menus;

        Shell(Shell* parent = 0, Flags flags
            = (Flags)(FLAG_400 | FLAG_80 | CLOSE_BUTTON | CHECKBOX | RADIOBUTTON));
        virtual ~Shell() override;

        virtual void Draw() override;
        virtual void Reset() override;
        virtual void SetSize(vec2 size) override;
        virtual vec2 GetPos() override;

        virtual vec2 GetSize() override;

        virtual rectangle GetBoundingBox() override;

        virtual void Hide();

        void Disp();
        bool GetDisp();
        void SetDisp(bool value);

        void sub_1402F38B0();
    };

    class List : public Scrollable {
    public:
        std::vector<std::string> items;
        std::vector<int32_t> selections;
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
        size_t GetItemCount();
        void ResetSelectedItem();
        void ResetSetSelectedItem(size_t index);
        void SetItemIndex(size_t index);
        void SetSelectedItem(size_t index);

        inline std::string GetHoveredItem() {
            return GetItem(hovered_item);
        }
        
        inline std::string GetSelectedItem() {
            return GetItem(selected_item);
        }
    };

    class ListBox : public Composite {
    public:
        List* list;

        ListBox(Composite* parent = 0, Flags flags = (Flags)(VERTICAL | MULTISELECT));
        virtual ~ListBox() override;

        virtual void Draw() override;
        virtual void Reset() override;

        inline void AddItem(const std::string& str) {
            if (list)
                list->AddItem(str);
        }

        inline void AddItem(const std::string&& str) {
            if (list)
                list->AddItem(str);
        }

        inline void AddSelectionListener(SelectionListener* value) {
            if (list)
                list->AddSelectionListener(value);
        }

        inline void ClearItems() {
            if (list)
                list->ClearItems();
        }

        inline std::string GetItem(size_t index) {
            if (list)
                return list->GetItem(index);
            return {};
        }

        inline size_t GetItemCount() {
            if (list)
                return list->GetItemCount();
            return 0;
        }

        inline std::string GetHoveredItem() {
            if (list)
                return list->GetHoveredItem();
            return {};
        }

        inline std::string GetSelectedItem() {
            if (list)
                return list->GetHoveredItem();
            return {};
        }

        inline void SetItemIndex(size_t index) {
            if (list)
                list->SetItemIndex(index);
        }
    };

    class Slider : public Control {
    public:
        const char* format;
        std::vector<SelectionListener*> selection_listeners;
        ScrollBar* scroll_bar;

        Slider(Composite* parent, Flags flags);
        virtual ~Slider();

        virtual void Draw() override;
        virtual void Reset() override;
        virtual void SetSize(vec2 value) override;

        void AddSelectionListener(SelectionListener* value);

        static Slider* make(Composite* parent = 0,
            Flags flags = (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL), float_t x = 0.0f, float_t y = 0.0f,
            float_t width = 128.0f, float_t height = 20.0f, const char* name = "slider");

        inline void SetParams(float_t value, float_t min, float_t max,
            float_t a5, float_t step, float_t step_fast) {
            scroll_bar->SetParams(value, min, max, a5, step, step_fast);
        }

        inline void SetValue(float_t value) {
            scroll_bar->SetValue(value);
        }
    };
}

extern void dw_gui_detail_display_init();
extern void dw_gui_detail_display_free();

extern void dw_gui_ctrl_disp();
