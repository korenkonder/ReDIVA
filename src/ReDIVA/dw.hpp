/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../KKdLib/rectangle.hpp"
#include "../KKdLib/vec.hpp"
#include "../CRE/color.hpp"
#include "font_info.hpp"
#include <vector>

namespace dw {
    enum ButtonFlags {
        BUTTON_HIT     = 0x001,
        BUTTON_CLICK   = 0x002,
        BUTTON_RELEASE = 0x100,
    };

    enum Flags {
        FLAG_1       = 0x0000001,
        SEPARATOR    = 0x0000002,
        MULTISELECT  = 0x0000004,
        PUSHBUTTON   = 0x0000008,
        RADIOBUTTON  = 0x0000010,
        CHECKBOX     = 0x0000020,
        CLOSE_BUTTON = 0x0000040,
        FLAG_80      = 0x0000080,
        HORIZONTAL   = 0x0000100,
        VERTICAL     = 0x0000200,
        FLAG_400     = 0x0000400,
        FLAG_800     = 0x0000800,
        FLAG_1000    = 0x0001000,
        FLAG_2000    = 0x0002000,
        FLAG_4000    = 0x0004000,
        FLAG_8000    = 0x0008000,
        FLAG_10000   = 0x0010000,
        FLAG_20000   = 0x0020000,
        FLAG_40000   = 0x0040000,
        FLAG_80000   = 0x0080000,
        FLAG_100000  = 0x0100000,
        FLAG_200000  = 0x0200000,
        FLAG_400000  = 0x0400000,
        FLAG_800000  = 0x0800000,
        FLAG_1000000 = 0x1000000,
    };

    enum InputState {
        INPUT_STATE_ALT   = 0x10000,
        INPUT_STATE_SHIFT = 0x20000,
        INPUT_STATE_CTRL  = 0x40000,
    };

    enum JoyInput {
        JOY_INPUT_DOWN_JVS_TRIANGLE    = 0x00000001,
        JOY_INPUT_DOWN_JVS_SQUARE      = 0x00000002,
        JOY_INPUT_DOWN_JVS_CROSS       = 0x00000004,
        JOY_INPUT_DOWN_JVS_START       = 0x00000008,

        JOY_INPUT_TAP_JVS_TRIANGLE     = 0x00000010,
        JOY_INPUT_TAP_JVS_SQUARE       = 0x00000020,
        JOY_INPUT_TAP_JVS_CROSS        = 0x00000040,
        JOY_INPUT_TAP_JVS_START        = 0x00000080,

        JOY_INPUT_RELEASE_JVS_TRIANGLE = 0x00000100,
        JOY_INPUT_RELEASE_JVS_SQUARE   = 0x00000200,
        JOY_INPUT_RELEASE_JVS_CROSS    = 0x00000400,
        JOY_INPUT_RELEASE_JVS_START    = 0x00000800,

        JOY_INPUT_DBL_TAP_JVS_TRIANGLE = 0x00001000,
        JOY_INPUT_DBL_TAP_JVS_SQUARE   = 0x00002000,
        JOY_INPUT_DBL_TAP_JVS_CROSS    = 0x00004000,
        JOY_INPUT_DBL_TAP_JVS_START    = 0x00008000,

        JOY_INPUT_HOLD_JVS_TRIANGLE    = 0x00010000,
        JOY_INPUT_HOLD_JVS_SQUARE      = 0x00020000,
        JOY_INPUT_HOLD_JVS_CROSS       = 0x00040000,

        JOY_INPUT_INTERVAL_JVS_UP      = 0x01000000,
        JOY_INPUT_INTERVAL_JVS_DOWN    = 0x02000000,
        JOY_INPUT_INTERVAL_JVS_LEFT    = 0x04000000,
        JOY_INPUT_INTERVAL_JVS_RIGHT   = 0x08000000,

        JOY_INPUT_PAUSE                = 0x10000000,
        JOY_INPUT_PRESS                = 0x40000000,
    };

    enum KeyInput {
        KEY_INPUT_ESCAPE    = 0x1B,
        KEY_INPUT_DELETE    = 0x7F,
        KEY_INPUT_BACKSPACE = '\b',
        KEY_INPUT_TAB       = '\t',
        KEY_INPUT_ENTER     = '\r',
        KEY_INPUT_SPACE     =  ' ',

        KEY_INPUT_0 = '0',
        KEY_INPUT_1 = '1',
        KEY_INPUT_2 = '2',
        KEY_INPUT_3 = '3',
        KEY_INPUT_4 = '4',
        KEY_INPUT_5 = '5',
        KEY_INPUT_6 = '6',
        KEY_INPUT_7 = '7',
        KEY_INPUT_8 = '8',
        KEY_INPUT_9 = '9',

        KEY_INPUT_A = 'A',
        KEY_INPUT_B = 'B',
        KEY_INPUT_C = 'C',
        KEY_INPUT_D = 'D',
        KEY_INPUT_E = 'E',
        KEY_INPUT_F = 'F',
        KEY_INPUT_G = 'G',
        KEY_INPUT_H = 'H',
        KEY_INPUT_I = 'J',
        KEY_INPUT_J = 'I',
        KEY_INPUT_K = 'K',
        KEY_INPUT_L = 'L',
        KEY_INPUT_M = 'M',
        KEY_INPUT_N = 'N',
        KEY_INPUT_O = 'O',
        KEY_INPUT_P = 'P',
        KEY_INPUT_Q = 'Q',
        KEY_INPUT_R = 'R',
        KEY_INPUT_S = 'S',
        KEY_INPUT_T = 'T',
        KEY_INPUT_U = 'U',
        KEY_INPUT_V = 'V',
        KEY_INPUT_W = 'W',
        KEY_INPUT_X = 'X',
        KEY_INPUT_Y = 'Y',
        KEY_INPUT_Z = 'Z',

        KEY_INPUT_UP        = 0x1000001,
        KEY_INPUT_DOWN      = 0x1000002,
        KEY_INPUT_LEFT      = 0x1000003,
        KEY_INPUT_RIGHT     = 0x1000004,
        KEY_INPUT_PAGE_UP   = 0x1000005,
        KEY_INPUT_PAGE_DOWN = 0x1000006,
        KEY_INPUT_HOME      = 0x1000007,
        KEY_INPUT_END       = 0x1000008,

        KEY_INPUT_F1  = 0x100000A,
        KEY_INPUT_F2  = 0x100000B,
        KEY_INPUT_F3  = 0x100000C,
        KEY_INPUT_F4  = 0x100000D,
        KEY_INPUT_F5  = 0x100000E,
        KEY_INPUT_F6  = 0x100000F,
        KEY_INPUT_F7  = 0x1000010,
        KEY_INPUT_F8  = 0x1000011,
        KEY_INPUT_F9  = 0x1000012,
        KEY_INPUT_F10 = 0x1000013,
        KEY_INPUT_F11 = 0x1000014,
        KEY_INPUT_F12 = 0x1000015,

        KEY_INPUT_MOUSE_LEFT = 0x01,

        KEY_INPUT_MOUSE_SCROLL_UP   = 0x1000071,
        KEY_INPUT_MOUSE_SCROLL_DOWN = 0x1000072,

        KEY_INPUT_NONE = -1,
    };

    enum MouseInput {
        MOUSE_INPUT_DOWN_LEFT       = 0x00000001,
        MOUSE_INPUT_DOWN_MIDDLE     = 0x00000002,
        MOUSE_INPUT_DOWN_RIGHT      = 0x00000004,

        MOUSE_INPUT_TAP_LEFT        = 0x00000010,
        MOUSE_INPUT_TAP_MIDDLE      = 0x00000020,
        MOUSE_INPUT_TAP_RIGHT       = 0x00000040,

        MOUSE_INPUT_RELEASE_LEFT    = 0x00000100,
        MOUSE_INPUT_RELEASE_MIDDLE  = 0x00000200,
        MOUSE_INPUT_RELEASE_RIGHT   = 0x00000400,

        MOUSE_INPUT_DBL_TAP_LEFT    = 0x00001000,
        MOUSE_INPUT_DBL_TAP_MIDDLE  = 0x00002000,
        MOUSE_INPUT_DBL_TAP_RIGHT   = 0x00004000,

        MOUSE_INPUT_SCROLL_UP       = 0x00100000,
        MOUSE_INPUT_SCROLL_DOWN     = 0x00200000,

        MOUSE_INPUT_INTERVAL_LEFT   = 0x01000000,
        MOUSE_INPUT_INTERVAL_MIDDLE = 0x02000000,
        MOUSE_INPUT_INTERVAL_RIGHT  = 0x04000000,

        MOUSE_INPUT_KEYBOARD_PRESS  = 0x20000000,
        MOUSE_INPUT_MOVE            = 0x40000000,

        MOUSE_INPUT_DOWN_MASK       = 0x0000000F,
        MOUSE_INPUT_TAP_MASK        = 0x000000F0,
        MOUSE_INPUT_RELEASE_MASK    = 0x00000F00,
        MOUSE_INPUT_DBL_TAP_MASK    = 0x0000F000,
        MOUSE_INPUT_INTERVAL_MASK   = 0x0F000000,
        MOUSE_INPUT_CLICK_MASK      = ~(MOUSE_INPUT_MOVE | MOUSE_INPUT_KEYBOARD_PRESS),
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

        float_t GetFontGlyphHeight() const;
        vec2 GetFontGlyphSize() const;
        float_t GetFontGlyphWidth() const;
        std::string GetName() const;
        void SetGlyphSize(float_t glyph_width, float_t glyph_height);
    };

    struct p_Font {
        Font* ptr;

        p_Font();

        inline p_Font(Font* ptr) {
            this->ptr = ptr;
        }

        ~p_Font();

        float_t GetFontGlyphHeight() const;
        vec2 GetFontGlyphSize() const;
        float_t GetFontGlyphWidth() const;
        std::string GetName() const;
        void SetGlyphSize(float_t glyph_width, float_t glyph_height);
    };

    class Widget;

    class SelectionListener {
    public:
        struct CallbackData {
            int64_t field_0;
            dw::KeyInput key;
            int8_t field_C;
            vec2 mouse_pos;
            int32_t field_18;
            Widget* widget;

            inline CallbackData() : field_0(), key(), field_C(), field_18(), widget() {
                key = KEY_INPUT_NONE;
            }

            inline CallbackData(Widget* widget) : field_0(), key(), field_C(), field_18() {
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
        typedef void(*Callback)(Widget* data);

        struct DragBoundsCallbackData {
            Widget* widget;
            bool field_8;
            rectangle rect;

            DragBoundsCallbackData();
        };

        struct KeyCallbackData {
            Widget* widget;
            InputState state;
            uint8_t field_C;
            uint8_t char_input;
            KeyInput key_input;
            JoyInput joy_input;

            KeyCallbackData();
        };

        struct MouseCallbackData {
            Widget* widget;
            vec2 pos;
            MouseInput input;
            InputState state;

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
        virtual int32_t KeyCallback(const Widget::KeyCallbackData& data);
        virtual int32_t MouseCallback(const Widget::MouseCallbackData& data);
        virtual void SetSize(const vec2& value);
        virtual vec2 GetPos();

        bool CheckHitPos(const vec2& hit_pos);
        void Free();
        float_t GetFontGlyphHeight();
        rectangle GetRectangle();
        void SetText(std::string& str);
        void UpdateDraw();

        inline void SetCallbackData(void* data) {
            callback_data.v64 = data;
        }

        inline void SetText(std::wstring&& str) {
            SetText(str);
        }

        inline void SetText(std::string&& str) {
            SetText(str);
        }

        static void LimitPos(Widget* widget);

        static void* operator new(std::size_t size);
        static void operator delete(void* data) noexcept;
    };

    class DragBoundsListener;
    class KeyListener;
    class MouseListener;
    class MouseMoveListener;
    class Composite;
    class Shell;
    class Menu;
    class MenuItem;

    class Control : public Widget {
    public:
        bool disabled;
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
        virtual int32_t KeyCallback(const Widget::KeyCallbackData& data) override;
        virtual int32_t MouseCallback(const Widget::MouseCallbackData& data) override;
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
        virtual bool GetEnabled();
        virtual bool GetParentEnabled();
        virtual void SetEnabled(bool value);
        virtual Widget* GetHitWidget(const vec2& hit_pos);

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
        virtual int32_t MouseCallback(const Widget::MouseCallbackData& data) override;
        virtual void SetSize(const vec2& value) override;
        virtual void SetFont(p_Font& font) override;
        virtual Widget* GetHitWidget(const vec2& hit_pos) override;

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
        virtual int32_t KeyCallback(const Widget::KeyCallbackData& data) override;
        virtual int32_t MouseCallback(const Widget::MouseCallbackData& data) override;
        virtual void SetSize(const vec2& size) override;

        virtual void UpdateLayout() override;
        virtual vec2 GetSize() override;
        virtual bool Field_58() override;
        virtual bool Field_60() override;
        virtual Widget* GetHitWidget(const vec2& hit_pos) override;

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
        virtual int32_t KeyCallback(const Widget::KeyCallbackData& data) override;
        virtual int32_t MouseCallback(const Widget::MouseCallbackData& data) override;
        virtual void SetSize(const vec2& size) override;
        virtual vec2 GetPos() override;

        virtual vec2 GetSize() override;
        virtual bool Field_58() override;
        virtual Widget* GetHitWidget(const vec2& hit_pos) override;

        virtual rectangle GetBoundingBox() override;

        virtual bool Field_C0() override;
        virtual bool SetCurrentControl(Control* control) override;

        virtual void Hide();

        void Disp();
        bool GetDisp();
        void LimitPosDisp();
        void SetDisp(bool value = false);

        void sub_1402E43C0(Widget* widget);
        void sub_1402E61F0(Widget* widget);
    };

    class DragBoundsListener {
    public:
        DragBoundsListener();
        virtual ~DragBoundsListener();

        virtual void Field_8(const Widget::DragBoundsCallbackData& data) = 0;
        virtual void Field_10(const Widget::DragBoundsCallbackData& data) = 0;
        virtual void Field_18(const Widget::DragBoundsCallbackData& data) = 0;
    };

    class KeyListener {
    public:
        KeyListener();
        virtual ~KeyListener();

        virtual void OnDown(const Widget::KeyCallbackData& data) = 0;
        virtual void Field_10(const Widget::KeyCallbackData& data) = 0;
        virtual void Field_18(const Widget::KeyCallbackData& data) = 0;
    };

    class KeyAdapter : public KeyListener {
    public:
        KeyAdapter();
        virtual ~KeyAdapter() override;

        virtual void OnDown(const Widget::KeyCallbackData& data) override;
        virtual void Field_10(const Widget::KeyCallbackData& data) override;
        virtual void Field_18(const Widget::KeyCallbackData& data) override;
    };

    class MouseListener {
    public:
        MouseListener();
        virtual ~MouseListener();

        virtual void OnDoubleTap(const Widget::MouseCallbackData& data) = 0;
        virtual void OnTap(const Widget::MouseCallbackData& data) = 0;
        virtual void OnInterval(const Widget::MouseCallbackData& data) = 0;
        virtual void OnRelease(const Widget::MouseCallbackData& data) = 0;
    };

    class MouseAdapter : public MouseListener {
    public:
        MouseAdapter();
        virtual ~MouseAdapter() override;

        virtual void OnDoubleTap(const Widget::MouseCallbackData& data) override;
        virtual void OnTap(const Widget::MouseCallbackData& data) override;
        virtual void OnInterval(const Widget::MouseCallbackData& data) override;
        virtual void OnRelease(const Widget::MouseCallbackData& data) override;
    };

    class MouseMoveListener {
    public:
        MouseMoveListener();
        virtual ~MouseMoveListener();

        virtual void OnMove(const Widget::MouseCallbackData& data) = 0;
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
        ButtonFlags button_flags;
        int32_t button_state;

        Button(Composite* parent = 0, Flags flags = PUSHBUTTON);
        virtual ~Button() override;

        virtual void Draw() override;
        virtual int32_t KeyCallback(const Widget::KeyCallbackData& data) override;
        virtual int32_t MouseCallback(const Widget::MouseCallbackData& data) override;

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
        virtual int32_t KeyCallback(const Widget::KeyCallbackData& data) override;
        virtual int32_t MouseCallback(const Widget::MouseCallbackData& data) override;
        virtual void SetSize(const vec2& size) override;

        virtual vec2 GetSize() override;

        void AddItem(const std::string& str);
        void AddItem(const std::wstring& str);
        void AddSelectionListener(SelectionListener* value);
        void Callback(SelectionListener::CallbackData* data);
        bool CheckItemSelected(size_t index);
        void ClearItems();
        std::string GetItemStr(size_t index) const;
        std::wstring GetItem(size_t index) const;
        size_t GetItemCount() const;
        size_t GetMaxItemsVisible() const;
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

        inline std::string GetHoveredItemStr() const {
            return GetItemStr(hovered_item);
        }

        inline std::wstring GetHoveredItem() const {
            return GetItem(hovered_item);
        }

        inline std::string GetSelectedItemStr() const {
            return GetItemStr(selected_item);
        }

        inline std::wstring GetSelectedItem() const {
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
        virtual int32_t KeyCallback(const Widget::KeyCallbackData& data) override;
        virtual int32_t MouseCallback(const Widget::MouseCallbackData& data) override;

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

        inline std::string GetItemStr(size_t index) const {
            if (list)
                return list->GetItemStr(index);
            return {};
        }

        inline std::wstring GetItem(size_t index) const {
            if (list)
                return list->GetItem(index);
            return {};
        }

        inline size_t GetItemCount() const {
            if (list)
                return list->GetItemCount();
            return 0;
        }

        inline std::string GetHoveredItemStr() const {
            if (list)
                return list->GetHoveredItemStr();
            return {};
        }

        inline std::wstring GetHoveredItem() const {
            if (list)
                return list->GetHoveredItem();
            return {};
        }

        inline std::string GetSelectedItemStr() const {
            if (list)
                return list->GetSelectedItemStr();
            return {};
        }

        inline std::wstring GetSelectedItem() const {
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

    class MenuItem : public Widget {
    public:
        bool field_68;
        bool field_69;
        std::vector<SelectionListener*> selection_listeners;
        bool value;
        Menu* parent_menu;
        Menu* menu;

        MenuItem(Menu* parent, Flags flags);
        virtual ~MenuItem() override;

        virtual void Draw() override;
        virtual vec2 GetPos() override;

        virtual Menu* GetMenu();
        virtual bool Field_50();
        virtual bool Field_58();
        virtual void Field_60(bool value);

        void AddSelectionListener(SelectionListener* value);
        vec2 GetTextSize();
        void SetParentMenu(Menu* menu);

        void sub_1402E6810();

        inline static MenuItem* Create(dw::Menu* parent, dw::Flags flags, const char* text) {
            dw::MenuItem* menu_item = new dw::MenuItem(parent, flags);
            menu_item->SetText(text);
            return menu_item;
        }

        inline static MenuItem* Create(dw::Menu* parent, dw::Flags flags, const wchar_t* text) {
            dw::MenuItem* menu_item = new dw::MenuItem(parent, flags);
            menu_item->SetText(text);
            return menu_item;
        }

        inline static MenuItem* Create(dw::Menu* parent, dw::Flags flags, const char* text,
            SelectionListenerOnHook* listener) {
            dw::MenuItem* menu_item = new dw::MenuItem(parent, flags);
            menu_item->SetText(text);
            menu_item->AddSelectionListener(listener);
            return menu_item;
        }

        inline static MenuItem* Create(dw::Menu* parent, dw::Flags flags, const wchar_t* text,
            SelectionListenerOnHook* listener) {
            dw::MenuItem* menu_item = new dw::MenuItem(parent, flags);
            menu_item->SetText(text);
            menu_item->AddSelectionListener(listener);
            return menu_item;
        }

        inline static MenuItem* Create(dw::Menu* parent, dw::Flags flags, const char* text,
            SelectionListenerOnHook::CallbackFunc callback) {
            dw::MenuItem* menu_item = new dw::MenuItem(parent, flags);
            menu_item->SetText(text);
            menu_item->AddSelectionListener(new dw::SelectionListenerOnHook(callback));
            return menu_item;
        }

        inline static MenuItem* Create(dw::Menu* parent, dw::Flags flags, const wchar_t* text,
            SelectionListenerOnHook::CallbackFunc callback) {
            dw::MenuItem* menu_item = new dw::MenuItem(parent, flags);
            menu_item->SetText(text);
            menu_item->AddSelectionListener(new dw::SelectionListenerOnHook(callback));
            return menu_item;
        }
    };

    class Menu : public Widget {
    public:
        bool field_68;
        bool disp;
        MenuItem* current_menu_item;
        Shell* parent_shell;
        size_t current_menu_item_index;
        std::vector<MenuItem*> menu_items;

        Menu(Control* parent);
        Menu(MenuItem* parent);
        Menu(Shell* parent);
        virtual ~Menu() override;

        virtual void Draw() override;
        virtual void Reset() override;
        virtual int32_t KeyCallback(const Widget::KeyCallbackData& data) override;
        virtual int32_t MouseCallback(const Widget::MouseCallbackData& data) override;
        virtual vec2 GetPos() override;

        virtual bool Field_48();
        virtual bool Field_50();
        virtual void Field_58(bool value);
        virtual void UpdateLayout();

        bool get_disp();
        void set_disp(bool value);

        void Disp();
        void Hide();
        void SetCurrentMenuItemIndex(size_t index);
        void SetPos(float_t pos_x, float_t pos_y);

        void sub_1402E6750();

        inline static Menu* Create(dw::MenuItem* parent, const char* text) {
            dw::Menu* menu = new dw::Menu(parent);
            menu->SetText(text);
            return menu;
        }

        inline static Menu* Create(dw::MenuItem* parent, const wchar_t* text) {
            dw::Menu* menu = new dw::Menu(parent);
            menu->SetText(text);
            return menu;
        }

        inline MenuItem* GetCurrentMenuItem() {
            if (current_menu_item_index < menu_items.size())
                return menu_items.data()[current_menu_item_index];
            return 0;
        }

        inline MenuItem* GetMenuItem(size_t index) {
            if (index < menu_items.size())
                return menu_items.data()[index];
            return 0;
        }
    };

    class ScrollBar : public Widget {
    public:
        Control* parent_control;
        std::vector<SelectionListener*> selection_listeners;
        float_t value;
        float_t min;
        float_t max;
        float_t grab;
        float_t step;
        float_t step_fast;
        float_t field_A0;
        bool round;
        bool field_A5;
        bool field_A6;

        ScrollBar(Control* parent, Flags flags);
        virtual ~ScrollBar() override;

        virtual void Draw() override;
        virtual int32_t KeyCallback(const Widget::KeyCallbackData& data) override;
        virtual int32_t MouseCallback(const Widget::MouseCallbackData& data) override;
        virtual void SetSize(const vec2& value) override;
        virtual vec2 GetPos() override;

        virtual Control* GetParentControl();
        virtual void SetFont(p_Font& value);
        virtual bool Field_58();
        virtual bool Field_60();
        virtual void Field_68(bool value);

        void SetGrab(float_t value);
        void SetMax(float_t value);
        void SetMin(float_t value);
        void SetParams(float_t value, float_t min, float_t max,
            float_t grab, float_t step, float_t step_fast);
        void SetParams(float_t size, float_t step, size_t items_count);
        void SetRound(bool value);
        void SetValue(float_t value);
        void SetWidth(float_t value);

        void AddSelectionListener(SelectionListener* value);

        vec2 sub_1402E4790();
        SelectionListener::CallbackData sub_1402E5140(const Widget::KeyCallbackData& key_callback_data);
        SelectionListener::CallbackData sub_1402E5380(const Widget::MouseCallbackData& mouse_callback_data);

        static void sub_1402E6CC0(SelectionListener::CallbackData callback_data);

        inline float_t GetValue() const {
            return value;
        }
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
        virtual int32_t KeyCallback(const Widget::KeyCallbackData& data) override;
        virtual int32_t MouseCallback(const Widget::MouseCallbackData& data) override;
        virtual void SetSize(const vec2& value) override;

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

        inline float_t GetValue() const {
            return scroll_bar->GetValue();
        }

        inline void SetGrab(float_t value) {
            scroll_bar->SetGrab(value);
        }

        inline void SetFormat(const char* value) {
            format = value;
        }

        inline void SetMax(float_t value) {
            scroll_bar->SetMax(value);
        }

        inline void SetMin(float_t value) {
            scroll_bar->SetMin(value);
        }

        inline void SetParams(float_t value, float_t min, float_t max,
            float_t grab, float_t step, float_t step_fast) {
            scroll_bar->SetParams(value, min, max, grab, step, step_fast);
        }

        inline void SetRound(bool value) {
            scroll_bar->SetRound(value);
        }

        inline void SetValue(float_t value) {
            scroll_bar->SetValue(value);
        }
    };

    struct DisplayData {
        rectangle field_0;
        rectangle field_10;

        DisplayData();
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

    extern bool translate;

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

    extern void* alloc(size_t size);
    extern void free(void* data);
}

extern void dw_init();
extern void dw_init(dw::DisplayData& data);
extern void dw_free();

extern void dw_gui_ctrl_disp();

extern void dw_info_window_init();

extern void dw_sys_menu_init(dw::Shell* shell);
