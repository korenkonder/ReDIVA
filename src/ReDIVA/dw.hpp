/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../KKdLib/vec.hpp"
#include "../CRE/sprite.hpp"
#include "font_info.hpp"
#include <vector>

namespace dw {
    enum Flags {
        FLAG_1       = 0x00001,
        FLAG_2       = 0x00002,
        MULTISELECT  = 0x00004,
        FLAG_8       = 0x00008,
        RADIOBUTTON  = 0x00010,
        CHECKBOX     = 0x00020,
        CLOSE_BUTTON = 0x00040,
        FLAG_80      = 0x00080,
        HORIZONTAL   = 0x00100,
        VERTICAL     = 0x00200,
        FLAG_400     = 0x00400,
        FLAG_800     = 0x00800,
        FLAG_1000    = 0x01000,
        FLAG_2000    = 0x02000,
        FLAG_4000    = 0x04000,
        FLAG_8000    = 0x08000,
        FLAG_10000   = 0x10000,
        FLAG_20000   = 0x20000,
    };

    struct Font {
        font_info font;
        std::string name;
        bool half_width;

        Font(int32_t font_handler_index, std::string& name);
        Font(int32_t font_handler_index, std::string& name, bool half_width);

        inline Font(int32_t font_handler_index, std::string&& name)
            : Font(font_handler_index, name) {

        }
        
        inline Font(int32_t font_handler_index, std::string&& name, bool half_width)
            : Font(font_handler_index, name, half_width) {

        }

        ~Font();

        float_t GetFontGlyphHeight();
        vec2 GetFontGlyphSize();
        float_t GetFontGlyphWidth();
        std::string GetName();
        void SetGlyphSize(float_t glyph_width, float_t glyph_height);
    };

    struct p_Font {
        Font* ptr;

        p_Font();

        inline p_Font(Font* ptr) {
            this->ptr = ptr;
        }

        ~p_Font();

        float_t GetFontGlyphHeight();
        vec2 GetFontGlyphSize();
        float_t GetFontGlyphWidth();
        std::string GetName();
        void SetGlyphSize(float_t glyph_width, float_t glyph_height);
    };

    class Widget;

    class SelectionListener {
    public:
        struct CallbackData {
            int64_t field_0;
            int32_t field_8;
            int8_t field_C;
            vec2 mouse_pos;
            int32_t field_18;
            Widget* widget;

            inline CallbackData() : field_0(), field_8(), field_C(), field_18(), widget() {
                field_8 = -1;
            }

            inline CallbackData(Widget* widget) : field_0(), field_8(), field_C(), field_18() {
                this->widget = widget;
            }
        };

        typedef void(*CallbackFunc)(SelectionListener::CallbackData* data);

        SelectionListener();
        virtual ~SelectionListener();

        virtual void Callback(CallbackData* data) = 0;
        virtual void Field_10(CallbackData* data) = 0;
    };

    class SelectionAdapter : public SelectionListener {
    public:
        SelectionAdapter();
        virtual ~SelectionAdapter() override;

        virtual void Callback(SelectionListener::CallbackData* data) override;
        virtual void Field_10(SelectionListener::CallbackData* data) override;
    };

    class SelectionListenerOnHook : public SelectionAdapter {
    public:
        typedef void(*CallbackFunc)(Widget* widget);

        CallbackFunc callback;

        SelectionListenerOnHook();
        SelectionListenerOnHook(SelectionListenerOnHook::CallbackFunc callback);
        virtual ~SelectionListenerOnHook() override;

        virtual void Callback(SelectionListener::CallbackData* data) override;
    };

    class Widget {
    public:
        typedef void(*Callback)(Widget* widget);

        struct KeyCallbackData {
            Widget* widget;
            int32_t modifier;
            int8_t field_C;
            int8_t field_D;
            int32_t input[2];

            KeyCallbackData();
        };

        struct MouseCallbackData {
            Widget* widget;
            vec2 pos;
            int32_t input;
            int32_t modifier;

            MouseCallbackData();
        };

        bool freed;

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
        std::wstring text;
        rectangle rect;
        Widget* parent;
        Flags flags;
        p_Font font;

        Widget(Widget* parent = 0, Flags flags = (Flags)0);
        virtual ~Widget();

        virtual void Draw();
        virtual void Reset();
        virtual std::wstring GetText();
        virtual void SetText(std::wstring& str);
        virtual void SetSize(vec2 value);
        virtual int32_t KeyCallback(Widget::KeyCallbackData data);
        virtual int32_t MouseCallback(Widget::MouseCallbackData data);
        virtual vec2 GetPos();

        bool CheckHitPos(vec2 hit_pos);
        void Free();
        float_t GetFontGlyphHeight();
        rectangle GetRectangle();
        void SetText(std::string& str);
        void UpdateDraw();
        
        inline void SetText(std::wstring&& str) {
            SetText(str);
        }

        inline void SetText(std::string&& str) {
            SetText(str);
        }

        static void sub_1402F3770(Widget* widget);
    };

    class KeyListener;
    class MouseListener;
    class MouseMoveListener;
    class Composite;
    class Shell;
    class Menu;

    class Control : public Widget {
    public:
        bool field_68;
        color4u8 foreground_color;
        color4u8 background_color;
        std::vector<KeyListener*> key_listener;
        std::vector<MouseListener*> mouse_listener;
        std::vector<MouseMoveListener*> mouse_move_listener;
        Composite* parent_comp;
        Shell* parent_shell;
        Menu* parent_menu;

        Control(Composite* parent = 0, Flags flags = (Flags)0);
        virtual ~Control() override;

        virtual void Draw() override;
        virtual void Reset() override;
        virtual int32_t KeyCallback(Widget::KeyCallbackData data) override;
        virtual int32_t MouseCallback(Widget::MouseCallbackData data) override;
        virtual vec2 GetPos() override;

        virtual void UpdateLayout();
        virtual vec2 GetSize();
        virtual bool Field_58();
        virtual bool Field_60();
        virtual void SetFont(p_Font& font);
        virtual p_Font& GetFont();
        virtual void SetForegroundColor(color4u8 value);
        virtual void SetBackgroundColor(color4u8 value);
        virtual color4u8 GetForegroundColor();
        virtual color4u8 GetBackgroundColor();
        virtual bool Field_98();
        virtual bool Field_A0();
        virtual void Field_A8(bool value);
        virtual Widget* GetHitWidget(vec2 hit_pos);

        void SetParentMenu(Menu* menu);
    };

    class ScrollBar;

    class Scrollable : public Control {
    public:
        ScrollBar* h_bar;
        ScrollBar* v_bar;

        Scrollable(Composite* parent = 0, Flags flags = (Flags)0);
        virtual ~Scrollable() override;

        virtual void Draw() override;
        virtual void Reset() override;
        virtual int32_t MouseCallback(Widget::MouseCallbackData data) override;
        virtual void SetSize(vec2 value) override;
        virtual void SetFont(p_Font& font) override;
        virtual Widget* GetHitWidget(vec2 hit_pos) override;

        virtual rectangle GetBoundingBox();

        rectangle GetScrollableRectangle();
    };

    class Layout;

    class Composite : public Scrollable {
    public:
        size_t current_control;
        Layout* layout;
        std::vector<Control*> controls;

        Composite(Composite* parent = 0, Flags flags = (Flags)0);
        virtual ~Composite() override;

        virtual void Draw() override;
        virtual void Reset() override;
        virtual int32_t KeyCallback(Widget::KeyCallbackData data) override;
        virtual int32_t MouseCallback(Widget::MouseCallbackData data) override;
        virtual void SetSize(vec2 size) override;

        virtual void UpdateLayout() override;
        virtual vec2 GetSize() override;
        virtual bool Field_58() override;
        virtual bool Field_60() override;
        virtual Widget* GetHitWidget(vec2 hit_pos) override;

        virtual bool Field_C0();
        virtual bool SetCurrentControl(Control* control);

        size_t GetControlIndex(Control* control);
        void SetLayout(Layout* value);

        bool sub_1402F1E60(Control* control);
    };

    class ShellCloseButton;

    class Shell : public Composite {
    public:
        typedef void(*Callback)(Shell* data);

        bool disp;
        Callback disp_callback;
        Callback hide_callback;
        int64_t field_128;
        bool destroy;
        ShellCloseButton* close_button;
        std::vector<Widget*> children;
        std::vector<Menu*> menus;
        bool field_170;
        Widget* field_178;

        Shell(Shell* parent = 0, Flags flags
            = (Flags)(FLAG_400 | FLAG_80 | CLOSE_BUTTON | CHECKBOX | RADIOBUTTON));
        virtual ~Shell() override;

        virtual void Draw() override;
        virtual void Reset() override;
        virtual int32_t KeyCallback(Widget::KeyCallbackData data) override;
        virtual int32_t MouseCallback(Widget::MouseCallbackData data) override;
        virtual void SetSize(vec2 size) override;
        virtual vec2 GetPos() override;

        virtual vec2 GetSize() override;
        virtual bool Field_58() override;
        virtual Widget* GetHitWidget(vec2 hit_pos) override;

        virtual rectangle GetBoundingBox() override;

        virtual bool Field_C0() override;
        virtual bool SetCurrentControl(Control* control) override;

        virtual void Hide();

        void Disp();
        bool GetDisp();
        void SetDisp(bool value);

        void sub_1402E43C0(Widget* widget);
        void sub_1402E61F0(Widget* widget);
        void sub_1402F38B0();
    };

    class KeyListener {
    public:
        KeyListener();
        virtual ~KeyListener();

        virtual void Field_8(Widget::KeyCallbackData data) = 0;
        virtual void Field_10(Widget::KeyCallbackData data) = 0;
        virtual void Field_18(Widget::KeyCallbackData data) = 0;
    };

    class KeyAdapter : public KeyListener {
    public:
        KeyAdapter();
        virtual ~KeyAdapter() override;

        virtual void Field_8(Widget::KeyCallbackData data) override;
        virtual void Field_10(Widget::KeyCallbackData data) override;
        virtual void Field_18(Widget::KeyCallbackData data) override;
    };

    class MouseListener {
    public:
        MouseListener();
        virtual ~MouseListener();

        virtual void Field_8(Widget::MouseCallbackData data) = 0;
        virtual void Field_10(Widget::MouseCallbackData data) = 0;
        virtual void Field_18(Widget::MouseCallbackData data) = 0;
        virtual void Field_20(Widget::MouseCallbackData data) = 0;
    };

    class MouseAdapter : public MouseListener {
    public:
        MouseAdapter();
        virtual ~MouseAdapter() override;

        virtual void Field_8(Widget::MouseCallbackData data) override;
        virtual void Field_10(Widget::MouseCallbackData data) override;
        virtual void Field_18(Widget::MouseCallbackData data) override;
        virtual void Field_20(Widget::MouseCallbackData data) override;
    };

    class MouseMoveListener {
    public:
        MouseMoveListener();
        virtual ~MouseMoveListener();

        virtual void Field_8(Widget::MouseCallbackData data) = 0;
    };

    class Layout {
    public:
        Layout();
        virtual ~Layout();

        virtual vec2 GetSize(Composite* comp) = 0;
        virtual void SetSize(Composite* comp) = 0;
    };

    class FillLayout : public Layout {
    public:
        Flags flags;

        FillLayout(Flags flags = VERTICAL);
        virtual ~FillLayout() override;

        virtual vec2 GetSize(Composite* comp) override;
        virtual void SetSize(Composite* comp) override;
    };

    class GraphLayout : public Layout {
    public:
        Flags flags;

        GraphLayout(Flags flags = VERTICAL);
        virtual ~GraphLayout() override;

        virtual vec2 GetSize(Composite* comp) override;
        virtual void SetSize(Composite* comp) override;
    };

    class GridLayout : public Layout {
    public:
        size_t rows;
        std::vector<float_t> column_size;
        std::vector<float_t> row_size;
        size_t columns;
        vec2 spacing;

        GridLayout(size_t columns = 2);
        virtual ~GridLayout() override;

        virtual vec2 GetSize(Composite* comp) override;
        virtual void SetSize(Composite* comp) override;
    };

    class RowLayout : public Layout {
    public:
        Flags flags;
        float_t spacing;

        RowLayout(Flags flags = VERTICAL);
        virtual ~RowLayout() override;

        virtual vec2 GetSize(Composite* comp) override;
        virtual void SetSize(Composite* comp) override;
    };

    class Group : public Composite {
    public:
        Group(Composite* parent = 0, Flags flags = (Flags)0);
        virtual ~Group() override;

        virtual void Draw() override;

        virtual vec2 GetSize() override;

        virtual rectangle GetBoundingBox() override;
    };

    class Label : public Control {
    public:
        Label(Composite* parent = 0, Flags flags = (Flags)0);
        virtual ~Label() override;

        virtual void Draw() override;
        virtual vec2 GetSize() override;
    };

    class Button : public Control {
    public:
        bool value;
        std::vector<SelectionListener*> selection_listeners;
        Widget::Callback callback;
        int32_t field_100;
        int32_t field_104;

        Button(Composite* parent = 0, Flags flags = (Flags)0);
        virtual ~Button() override;

        virtual void Draw() override;
        virtual int32_t KeyCallback(Widget::KeyCallbackData data) override;
        virtual int32_t MouseCallback(Widget::MouseCallbackData data) override;

        virtual vec2 GetSize() override;

        void AddSelectionListener(SelectionListener* value);
        void SetValue(bool value);
    };

    class List : public Scrollable {
    public:
        std::vector<std::wstring> items;
        std::vector<int32_t> selections;
        size_t hovered_item;
        size_t selected_item;
        std::vector<SelectionListener*> selection_listeners;
        size_t max_items;
        bool field_148;

        List(Composite* parent = 0, Flags flags = (Flags)0);
        virtual ~List() override;

        virtual void Draw() override;
        virtual void Reset() override;
        virtual int32_t KeyCallback(Widget::KeyCallbackData data) override;
        virtual int32_t MouseCallback(Widget::MouseCallbackData data) override;
        virtual void SetSize(vec2 size) override;

        virtual vec2 GetSize() override;

        void AddItem(const std::string& str);
        void AddItem(const std::wstring& str);
        void AddSelectionListener(SelectionListener* value);
        void Callback(SelectionListener::CallbackData* data);
        bool CheckItemSelected(size_t index);
        void ClearItems();
        std::string GetItemStr(size_t index) const;
        std::wstring GetItem(size_t index) const;
        size_t GetItemCount();
        size_t GetMaxItemsVisible();
        void ResetSelectedItem();
        void SetMaxItems(size_t value);
        void SetScrollBarParams();
        void SetSelectedItem(size_t index);
        void SetSelectedItems(size_t begin, size_t end);
        void UnsetSelectedItem(size_t index);

        inline void AddItem(const std::string&& str) {
            AddItem(str);
        }

        inline void AddItem(const std::wstring&& str) {
            AddItem(str);
        }

        inline std::string GetHoveredItemStr() {
            return GetItemStr(hovered_item);
        }

        inline std::wstring GetHoveredItem() {
            return GetItem(hovered_item);
        }

        inline std::string GetSelectedItemStr() {
            return GetItemStr(selected_item);
        }

        inline std::wstring GetSelectedItem() {
            return GetItem(selected_item);
        }

        inline void ResetSetSelectedItem(size_t index) {
            ResetSelectedItem();
            SetSelectedItem(index);
        }

        inline void SetItemIndex(size_t index) {
            hovered_item = index;
            ResetSetSelectedItem(index);
        }

        int64_t sub_1402EF570();
        bool sub_1402F1F20(size_t index);
        void sub_1402F9680(int64_t index);
        void sub_1402F9C90(size_t index);
    };

    class ListBox : public Composite {
    public:
        List* list;
        bool field_118;

        ListBox(Composite* parent = 0, Flags flags = (Flags)(VERTICAL | MULTISELECT));
        virtual ~ListBox() override;

        virtual void Draw() override;
        virtual void Reset() override;
        virtual int32_t KeyCallback(Widget::KeyCallbackData data) override;
        virtual int32_t MouseCallback(Widget::MouseCallbackData data) override;

        virtual vec2 GetSize() override;
        virtual void SetFont(p_Font& value) override;

        rectangle GetListRectangle();

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

        inline std::string GetItemStr(size_t index) {
            if (list)
                return list->GetItemStr(index);
            return {};
        }

        inline std::wstring GetItem(size_t index) {
            if (list)
                return list->GetItem(index);
            return {};
        }

        inline size_t GetItemCount() {
            if (list)
                return list->GetItemCount();
            return 0;
        }

        inline std::string GetHoveredItemStr() {
            if (list)
                return list->GetHoveredItemStr();
            return {};
        }

        inline std::wstring GetHoveredItem() {
            if (list)
                return list->GetHoveredItem();
            return {};
        }

        inline std::string GetSelectedItemStr() {
            if (list)
                return list->GetSelectedItemStr();
            return {};
        }

        inline std::wstring GetSelectedItem() {
            if (list)
                return list->GetSelectedItem();
            return {};
        }

        inline void SetItemIndex(size_t index) {
            if (list)
                list->SetItemIndex(index);
        }

        inline void SetMaxItems(size_t value) {
            if (list)
                list->SetMaxItems(value);
        }

        void sub_1402E4440();
        void sub_1402E4F40();
        void sub_1402E4F90();

        static bool sub_1402EC8C0(rectangle rect, vec2 pos);
    };

    class ScrollBar : public Widget {
    public:
        Control* parent_control;
        std::vector<SelectionListener*> selection_listeners;
        float_t value;
        float_t min;
        float_t max;
        float_t field_94;
        float_t step;
        float_t step_fast;
        float_t field_A0;
        bool round;
        bool field_A5;
        bool field_A6;
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
        virtual int32_t KeyCallback(Widget::KeyCallbackData data) override;
        virtual int32_t MouseCallback(Widget::MouseCallbackData data) override;
        virtual void SetSize(vec2 value) override;
        virtual vec2 GetPos() override;

        virtual Control* GetParentControl();
        virtual void SetFont(p_Font& value);
        virtual bool Field_58();
        virtual bool Field_60();
        virtual void Field_68(bool value);

        void SetParams(float_t value, float_t min, float_t max,
            float_t a5, float_t step, float_t step_fast);
        void SetParams(float_t size, float_t step, size_t items_count);
        void SetValue(float_t value);
        void SetWidth(float_t value);

        void AddSelectionListener(SelectionListener* value);

        vec2 sub_1402E4790();
        SelectionListener::CallbackData sub_1402E5140(Widget::KeyCallbackData key_callback_data);
        SelectionListener::CallbackData sub_1402E5380(Widget::MouseCallbackData mouse_callback_data);

        static void sub_1402E6CC0(SelectionListener::CallbackData callback_data);
    };

    class ShellCloseButton : public Button {
    public:
        ShellCloseButton(Shell* parent);
        virtual ~ShellCloseButton() override;

        virtual void Draw() override;
        virtual vec2 GetPos() override;

        virtual vec2 GetSize() override;

        static void Callback(Widget* data);
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
        virtual int32_t KeyCallback(Widget::KeyCallbackData data) override;
        virtual int32_t MouseCallback(Widget::MouseCallbackData data) override;
        virtual void SetSize(vec2 value) override;

        virtual void UpdateLayout() override;
        virtual vec2 GetSize() override;

        void AddSelectionListener(SelectionListener* value);
        vec2 GetTextSize();

        static Slider* Create(Composite* parent, Flags flags, const char* text,
            const char* format, float_t width = 128.f);
        static Slider* Create(Composite* parent = 0,
            Flags flags = (Flags)(FLAG_800 | HORIZONTAL),
            float_t pos_x = 0.0f, float_t pos_y = 0.0f,
            float_t width = 128.0f, float_t height = 20.0f, const char* text = "slider");

        inline void SetParams(float_t value, float_t min, float_t max,
            float_t a5, float_t step, float_t step_fast) {
            scroll_bar->SetParams(value, min, max, a5, step, step_fast);
        }

        inline void SetValue(float_t value) {
            scroll_bar->SetValue(value);
        }
    };

    struct init_data {
        rectangle field_0;
        rectangle field_10;

        init_data();
    };

    class DropDownListScrollBarSelectionListener : public SelectionAdapter {
    public:
        DropDownListScrollBarSelectionListener();
        virtual ~DropDownListScrollBarSelectionListener() override;

        virtual void Callback(SelectionListener::CallbackData* data) override;
    };

    class ScrollBarTestSelectionListener : public SelectionAdapter {
    public:
        ScrollBarTestSelectionListener();
        virtual ~ScrollBarTestSelectionListener() override;

        virtual void Callback(SelectionListener::CallbackData* data) override;
    };

    extern p_Font current_font;

    extern p_Font p_font_type_6x12;
    extern p_Font p_font_type_8x12;
    extern p_Font p_font_type_8x16;
    extern p_Font p_font_type_10x16;
    extern p_Font p_font_type_10x20;
    extern p_Font p_font_type_12x24;
    extern p_Font p_font_type_scroll_bar;

    extern void font_init();
    extern int32_t font_get_index(std::string& name);
    extern void font_free();
}

extern void dw_init();
extern void dw_init(dw::init_data& init_data);
extern void dw_free();

extern void dw_gui_ctrl_disp();

extern int32_t sub_140192D00();
extern void sub_140194880(int32_t a1);
