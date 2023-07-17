/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "dw_console.hpp"
#include "../KKdLib/str_utils.hpp"
#include "dw.hpp"

struct c_buff_data {
    char* buf;
    char* buf_end;
    char* position;

    void init(char* data, size_t size, size_t position);
    void move(int32_t size);
    void printf(const char* fmt, ...);
    void vprintf(const char* fmt, va_list args);
};

struct c_buff {
    char buf[4096];
    c_buff_data data;

    void init();
    void free();
};

class DwConsole : public dw::Shell {
public:
    dw_console_index index;
    dw::ListBox* list_box;
    dw::List* list;

    DwConsole();
    virtual ~DwConsole() override;

    virtual void Draw() override;

    virtual void Hide() override;

    void CtrlList();

    static void ListBoxCallback(dw::Widget* data);
};

static const char* dw_console_index_names[] = {
    "SYSTEM",
    "MATCHING",
    "PV SCRIPT",
    "GAME",
    "GAME_SUB COMMAND",
    "",
    "",
    "",
    "",
    "",
    "MODE_CTRL",
    "MODE_MEMORY",
    "EVENT_MEMORY",
    "TEMP_MEMORY",
    "AUTH_BILLING",
    "BOOKKEEP",
    "AIME",
    "",
    "",
    "",
};

c_buff dw_console_c_buff_array[DW_CONSOLE_MAX];

DwConsole* dw_console;

static const char* dw_console_index_get_name(dw_console_index index);
static const char* dw_console_c_buff_array_get_buf(int32_t index);

void dw_console_init() {
    if (!dw_console) {
        dw_console = new DwConsole();
        dw_console->sub_1402F38B0();
    }
    else
        dw_console->Disp();
}

void dw_console_printf(dw_console_index index, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    dw_console_vprintf(index, fmt, args);
    va_end(args);
}

void dw_console_vprintf(dw_console_index index, const char* fmt, va_list args) {
    /*if (!main_thread_id_check())
        return;*/

    c_buff* str_buf = &dw_console_c_buff_array[index];

    char buf[0x100];
    int32_t len = vsprintf_s(buf, sizeof(buf), fmt, args);
    if (&str_buf->data.position[len] >= str_buf->data.buf_end)
        str_buf->data.move(len + 1);
    str_buf->data.vprintf(fmt, args);
}

void dw_console_c_buff_array_init() {
    for (c_buff& i : dw_console_c_buff_array)
        i.init();
}

void dw_console_c_buff_array_free() {
    for (c_buff& i : dw_console_c_buff_array)
        i.free();
}

DwConsole::DwConsole() : index() {
    SetText("Console");

    rect.pos = { 512.0f, 64.0f };
    SetSize({ 400.0f, 600.0f });

    list_box = new dw::ListBox(this, (dw::Flags)(dw::FLAG_800 | dw::VERTICAL | dw::MULTISELECT));

    for (int32_t i = 0; i < DW_CONSOLE_MAX; i++) {
        char buf[0x04];
        sprintf_s(buf, sizeof(buf), "%02u:", i);

        std::string item(buf);
        item.append(dw_console_index_get_name((dw_console_index)i));
        list_box->AddItem(item);
    }

    list_box->SetItemIndex(index);
    list_box->SetMaxItems(5);
    list_box->callback_data.v64 = this;
    list_box->AddSelectionListener(new dw::SelectionListenerOnHook(DwConsole::ListBoxCallback));

    list = new dw::List(this, (dw::Flags)(dw::FLAG_800 | dw::VERTICAL | dw::MULTISELECT));

    CtrlList();

    UpdateLayout();

    rect.pos = { 512.0f, 64.0f };
    SetSize({ 400.0f, 600.0f });
}

DwConsole::~DwConsole() {

}

void DwConsole::Draw() {
    CtrlList();

    dw::Shell::Draw();
}

void DwConsole::Hide() {
    SetDisp(false);
}

void DwConsole::ListBoxCallback(dw::Widget* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box)
        ((DwConsole*)(list_box->callback_data.v64))->index
            = (dw_console_index)list_box->list->selected_item;
}

void DwConsole::CtrlList() {
    size_t hovered_item = list->hovered_item;
    size_t selected_item = list->selected_item;

    list->ClearItems();

    const char* str = dw_console_c_buff_array_get_buf(index);
    if (str) {
        while (str) {
            std::string s;
            str = str_utils_get_next_string(str, s, '\n');
            list->AddItem(s);
        }
    }

    list->hovered_item = hovered_item;
    list->ResetSetSelectedItem(selected_item);

    int32_t v9 = (int32_t)(rect.size.y / list->GetFontGlyphHeight()) - 3;
    v9 = max_def(v9, 1);

    list->SetMaxItems(v9);

    while (list->items.size() < v9)
        list->AddItem("");

    dw::ScrollBar* v_bar = list->v_bar;
    if (v_bar) {
        bool equal = *(uint32_t*)&v_bar->value == *(uint32_t*)&v_bar->max;

        float_t v15 = (float_t)(int64_t)list->GetMaxItemsVisible();
        if (v15 < 0.0f)
            v15 += (float_t)UINT64_MAX;
        float_t v16 = list->GetFontGlyphHeight() * v15;

        float_t v20 = (float_t)(int64_t)list->items.size();
        if (v20 < 0.0f)
            v20 += (float_t)UINT64_MAX;
        float_t v21 = list->GetFontGlyphHeight() * v20;

        float_t v22 = v21 - v16;
        if (v22 >= 0.0f) {
            v_bar->SetMin(0.0f);
            v_bar->SetMax(v22);
            v_bar->sub_1402F9670(v16 / v21 * v22);
        }
        else {
            v22 = 0.0f;
            v_bar->SetMin(0.0f);
            v_bar->SetMax(0.0f);
            v_bar->sub_1402F9670(v21);
        }

        if (equal)
            v_bar->SetValue(v22);
    }
}

void c_buff_data::init(char* data, size_t size, size_t position) {
    buf = data;
    buf_end = &data[size];
    this->position = &data[position];
}

void c_buff_data::move(int32_t size) {
    char* v4 = this->position;
    char* v5 = &buf[size];

    while (v5 < v4 && *v5++ != '\n');

    if (v5 >= v4 && v5 != v4)
        v5 = position;

    memmove(buf, v5, v4 - v5);

    this->position -= v5 - buf;
}

void c_buff_data::printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

void c_buff_data::vprintf(const char* fmt, va_list args) {
    char* pos = &position[vsprintf_s(position, buf_end - position, fmt, args)];
    position = min_def(pos, buf_end);
}

void c_buff::init() {
    data.init(buf, sizeof(buf), 0);
}

void c_buff::free() {

}

static const char* dw_console_index_get_name(dw_console_index index) {
    if (index >= 0 && index < DW_CONSOLE_MAX)
        return dw_console_index_names[index];
    return 0;
}

static const char* dw_console_c_buff_array_get_buf(int32_t index) {
    return dw_console_c_buff_array[index].data.buf;
}
