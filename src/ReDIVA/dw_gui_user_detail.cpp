/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "dw_gui_user_detail.hpp"

namespace dw_gui_user_detail {
    class ColorListWindow : public dw::Shell {
    public:
        ColorListWindow();
        virtual ~ColorListWindow() override;

        virtual void Hide() override;

        static void ColorCallback(dw::Widget* data);
        static void DefaultSettingCallback(dw::Widget* data);
    };

    class FontListWindow : public dw::Shell {
    public:
        dw::ListBox* font_list;

        FontListWindow();
        virtual ~FontListWindow() override;

        virtual void Hide() override;

        static void ApplyCallback(dw::Widget* data);
    };

    ColorListWindow* color_list_window;
    FontListWindow* font_list_window;
}

void dw_color_list_window_init() {
    if (dw_gui_user_detail::color_list_window)
        dw_gui_user_detail::color_list_window->Disp();
    else {
        dw_gui_user_detail::color_list_window = new dw_gui_user_detail::ColorListWindow;
        dw_gui_user_detail::color_list_window->LimitPosDisp();
    }
}

void dw_font_list_window_init() {
    if (dw_gui_user_detail::font_list_window)
        dw_gui_user_detail::font_list_window->Disp();
    else {
        dw_gui_user_detail::font_list_window = new dw_gui_user_detail::FontListWindow;
        dw_gui_user_detail::font_list_window->LimitPosDisp();
    }
}

namespace dw_gui_user_detail {
    ColorListWindow::ColorListWindow() {
        for (auto& i : dw::color_list) {
            dw::Button* color = new dw::Button(this);
            color->SetText(i.first);
            color->callback = ColorListWindow::ColorCallback;
        }

        dw::Button* default_setting = new dw::Button(this);
        default_setting->SetText(L"default setting");
        default_setting->callback = (dw::Widget::Callback)ColorListWindow::DefaultSettingCallback;

        layout = new dw::GridLayout(2);

        SetText(L"ColorListWindow");

        UpdateLayout();
    }

    ColorListWindow::~ColorListWindow() {

    }

    void ColorListWindow::Hide() {
        SetDisp(false);
    }

    void ColorListWindow::ColorCallback(dw::Widget* data) {
        dw::Button* button = dynamic_cast<dw::Button*>(data);
        if (button) {
            std::wstring name = button->GetText();

            dw::ColorDialog* color_dialog = new dw::ColorDialog(button->parent_shell);
            color_dialog->SetText(name);

            auto elem = dw::color_list.find(utf16_to_utf8(name).c_str());
            if (elem != dw::color_list.end()) {
                color_dialog->color_ptr = elem->second;
                color_dialog->SetColor(*elem->second);
            }

            color_dialog->LimitPosDisp();
        }
    }

    void ColorListWindow::DefaultSettingCallback(dw::Widget* data) {
        dw::colors_current_reset();
    }

    FontListWindow::FontListWindow() {
        (new dw::Label(this))->SetText(L"font");

        font_list = new dw::ListBox(this, dw::MULTISELECT);
        font_list->SetText(L"listBox");
        font_list->AddItem(dw::p_font_type_10x16.GetName());
        font_list->AddItem(dw::p_font_type_8x12.GetName());
        font_list->AddItem(dw::p_font_type_8x16.GetName());
        font_list->AddItem(dw::p_font_type_10x20.GetName());
        font_list->AddItem(dw::p_font_type_12x24.GetName());
        font_list->AddItem(dw::p_font_type_6x12.GetName());
        font_list->SetItemIndex(2);

        (new dw::Label(this))->SetText(L"");
        (new dw::Label(this))->SetText(L" !\"#$%&'()*+,-./");
        (new dw::Label(this))->SetText(L"0123456789:;<=>?");
        (new dw::Label(this))->SetText(L"@ABCDEFGHIJKLMNO");
        (new dw::Label(this))->SetText(L"PQRSTUVWXYZ[\\]^_");
        (new dw::Label(this))->SetText(L"`abcdefghijklmno");
        (new dw::Label(this))->SetText(L"pqrstuvwxyz{|}~");
        (new dw::Label(this))->SetText(L"");

        dw::Button* apply = new dw::Button(this);
        apply->SetText(L"Apply");
        apply->callback = FontListWindow::ApplyCallback;

        SetText(L"FontListWindow");

        UpdateLayout();
    }
    
    FontListWindow::~FontListWindow() {

    }

    void FontListWindow::Hide() {
        SetDisp(false);
    }

    void FontListWindow::ApplyCallback(dw::Widget* data) {
        dw::Button* button = dynamic_cast<dw::Button*>(data);
        if (button) {
            FontListWindow* font_list_window = dynamic_cast<FontListWindow*>(button->parent_shell);
            if (font_list_window)
                dw::font_get_index(font_list_window->font_list->GetSelectedItemStr());
        }
    }
}
