/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "color_change.hpp"
#include "../../CRE/light_param/face.hpp"
#include "../../CRE/rob/rob.hpp"
#include "../../CRE/light_param.hpp"
#include "../../CRE/object.hpp"
#include "../../CRE/render_context.hpp"
#include "../../CRE/sprite.hpp"
#include "../../KKdLib/image.hpp"
#include "../dw.hpp"

extern render_context* rctx_ptr;

class ColorChangeDw : public dw::Shell {
public:
    dw::ListBox* object_sets;
    dw::ListBox* texture_names;
    dw::Widget* type;
    dw::Widget* format;
    dw::Widget* size;
    dw::Button* show_button;
    dw::Button* auto_resize_button;
    dw::ListBox* item_chara;
    dw::ListBox* item_names;
    dw::ListBox* item_texture_names;
    dw::Button* over_write;
    dw::Button* inverse;
    dw::Slider* blend_r;
    dw::Slider* blend_g;
    dw::Slider* blend_b;
    dw::Slider* offset_r;
    dw::Slider* offset_g;
    dw::Slider* offset_b;
    dw::Slider* hue;
    dw::Slider* value;
    dw::Slider* saturation;
    dw::Slider* contrast;
    dw::Button* copy_button;
    dw::Button* paset_button;
    dw::Button* reset_button;
    dw::Button* save_button;
    dw::RowLayout* row_layout;
    int32_t obj_set_index;
    uint32_t obj_set_id;
    bool set;
    int32_t color_change_index;
    std::vector<texture*> textures;
    std::vector<color_tone> color_tones;
    color_tone color_tone_temp;
    bool show;
    bool auto_resize;
    chara_index chara_index;
    int32_t item_index;
    size_t item_texture_index;

    ColorChangeDw();
    virtual ~ColorChangeDw() override;
    virtual void Draw() override;
    virtual void Reset() override;

    virtual void Hide() override;

    texture* GetChgTex();
    color_tone* GetColorTone();
    void GetColorToneDefault(color_tone& value);
    const item_table_item* GetItem(int32_t item_index);
    void GetItemColTone();
    const item_table_item_data* GetItemData(int32_t item_index);
    int32_t GetItemNo(int32_t item_index);
    const char* GetItemTexChangeName();
    texture* GetOrgTex();
    void GetSetColorTone();
    void GetTextureNames();
    bool LoadColorTone();
    void LoadItemNames();
    bool LoadTexture();
    void ReloadData();
    void ResetColorTone();
    void ResetData();
    void ResetTexture();
    void SetObjectSet(const char* str);
    void SetTextureName(const char* name);
    void UpdateData();
    void UpdateItemTextureNames();
    void UpdateTextureInfo();

    static void AutoResizeCallback(dw::Button* data);
    static void BlendBCallback(dw::Slider* data);
    static void BlendGCallback(dw::Slider* data);
    static void BlendRCallback(dw::Slider* data);
    static void ContrastCallback(dw::Slider* data);
    static void CopyCallback(dw::Button* data);
    static void HueCallback(dw::Slider* data);
    static void InverseCallback(dw::Button* data);
    static void ItemCallback(dw::ListBox* data);
    static void ItemCharaCallback(dw::ListBox* data);
    static void ItemTextureCallback(dw::ListBox* data);
    static void ObjectSetCallback(dw::ListBox* data);
    static void OffsetBCallback(dw::Slider* data);
    static void OffsetGCallback(dw::Slider* data);
    static void OffsetRCallback(dw::Slider* data);
    static void OverwriteCallback(dw::Button* data);
    static void PasteCallback(dw::Button* data);
    static void ResetCallback(dw::Button* data);
    static void SaturationCallback(dw::Slider* data);
    static void SaveCallback(dw::Button* data);
    static void ShowCallback(dw::Button* data);
    static void TextureCallback(dw::ListBox* data);
    static void ValueCallback(dw::Slider* data);
};

ColorChangeDw* color_change_dw;

static const char* get_dev_ram_color_change_param_dir();

void color_change_dw_init() {
    if (!color_change_dw) {
        color_change_dw = new ColorChangeDw;
        color_change_dw->UpdateLayout();
        color_change_dw->LimitPosDisp();
    }
    else
        color_change_dw->Disp();
}

ColorChangeDw::ColorChangeDw() {
    data_struct* aft_data = &data_list[DATA_AFT];
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    set = false;
    obj_set_index = 0;
    obj_set_id = aft_obj_db->get_object_set_id(0u);
    color_change_index = 0;

    color_tone col_tone;
    GetColorToneDefault(col_tone);
    color_tone_temp = col_tone;

    show = true;
    auto_resize = true;
    chara_index = CHARA_MIKU;
    item_index = 0;
    item_texture_index = 0;

    SetText("COLOR CHANGE");

    rect.pos = 0.0f;
    SetSize({ 512.0f, 200.f });

    const float_t glyph_height = font.GetFontGlyphHeight();

    row_layout = new dw::RowLayout(dw::VERTICAL);

    dw::Composite* comp = new dw::Composite(this);
    comp->layout = row_layout;

    dw::Group* texture_group = new dw::Group(comp);
    texture_group->SetText("TEXTURE");

    object_sets = new dw::ListBox(texture_group, (dw::Flags)(dw::FLAG_800 | dw::VERTICAL | dw::MULTISELECT));
    object_sets->SetText("listbox0");
    uint32_t obj_set_count = aft_obj_db->get_object_set_count();
    for (uint32_t i = obj_set_count, j = 0; i; i--, j++)
        object_sets->AddItem(aft_obj_db->get_object_set_name(aft_obj_db->get_object_set_id(j)));
    object_sets->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::SelectionListenerOnHook::CallbackFunc)ColorChangeDw::ObjectSetCallback));
    object_sets->SetItemIndex(obj_set_index);
    object_sets->SetMaxItems(20);
    object_sets->callback_data.v64 = this;

    texture_names = new dw::ListBox(texture_group, (dw::Flags)(dw::FLAG_800 | dw::VERTICAL | dw::MULTISELECT));
    texture_names->SetText("listbox1");

    const int32_t tex_num = objset_info_storage_get_obj_set_tex_num(obj_set_id);
    for (int32_t i = 0; i < tex_num; i++)
        texture_names->AddItem(sprintf_s_string("%d", i));

    texture_names->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::SelectionListenerOnHook::CallbackFunc)ColorChangeDw::TextureCallback));
    texture_names->SetItemIndex(0);
    texture_names->SetMaxItems(20);
    texture_names->callback_data.v64 = this;

    type = new dw::Label(texture_group, dw::FLAG_4000);
    type->SetText("Type                   ");
    type->callback_data.i64 = 2;

    format = new dw::Label(texture_group, dw::FLAG_4000);
    format->SetText("Format                 ");
    format->callback_data.i64 = 1;

    size = new dw::Label(texture_group, dw::FLAG_4000);
    size->SetText("Size                   ");
    size->callback_data.i64 = 1;

    show_button = new dw::Button(texture_group, dw::CHECKBOX);
    show_button->SetText("Show");
    show_button->SetValue(show);
    show_button->callback = (dw::Widget::Callback)ColorChangeDw::ShowCallback;
    show_button->callback_data.v64 = this;

    auto_resize_button = new dw::Button(texture_group, dw::CHECKBOX);
    auto_resize_button->SetText("Auto Resize 256x256");
    auto_resize_button->SetValue(auto_resize);
    auto_resize_button->callback = (dw::Widget::Callback)ColorChangeDw::AutoResizeCallback;
    auto_resize_button->callback_data.v64 = this;

    dw::Group* item_group = new dw::Group(comp);
    item_group->SetText("ITEM");

    item_chara = new dw::ListBox(item_group, (dw::Flags)(dw::FLAG_800 | dw::VERTICAL | dw::MULTISELECT));
    item_chara->SetText("listbox0");
    for (int32_t i = 0; i < CHARA_MAX; i++)
        item_chara->AddItem(chara_index_get_name((::chara_index)i));
    item_chara->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::SelectionListenerOnHook::CallbackFunc)ColorChangeDw::ItemCharaCallback));
    item_chara->SetMaxItems(20);
    item_chara->callback_data.v64 = this;

    item_names = new dw::ListBox(item_group, (dw::Flags)(dw::FLAG_800 | dw::VERTICAL | dw::MULTISELECT));
    item_names->SetText("listbox0");
    item_names->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::SelectionListenerOnHook::CallbackFunc)ColorChangeDw::ItemCallback));
    item_names->SetItemIndex(0);
    item_names->SetMaxItems(20);
    item_names->callback_data.v64 = this;

    item_texture_names = new dw::ListBox(item_group, (dw::Flags)(dw::FLAG_800 | dw::VERTICAL | dw::MULTISELECT));
    item_texture_names->SetText("listbox0");
    item_texture_names->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::SelectionListenerOnHook::CallbackFunc)ColorChangeDw::ItemTextureCallback));
    item_texture_names->SetItemIndex(0);
    item_texture_names->SetMaxItems(20);
    item_texture_names->callback_data.v64 = this;

    over_write = new dw::Button(item_group, dw::FLAG_8);
    over_write->SetText("OVER WRITE");
    over_write->callback = (dw::Widget::Callback)ColorChangeDw::OverwriteCallback;
    over_write->callback_data.v64 = this;

    dw::Group* color_group = new dw::Group(comp);
    color_group->SetText("COLOR");

    const char* inverse_text;
    if (dw::translate)
        inverse_text = u8"Inverse";
    else
        inverse_text = u8"反転";

    inverse = new dw::Button(color_group, dw::CHECKBOX);
    inverse->SetText(inverse_text);
    inverse->SetValue(col_tone.inverse);
    inverse->callback = (dw::Widget::Callback)ColorChangeDw::InverseCallback;
    inverse->callback_data.v64 = this;

    blend_r = dw::Slider::Create(color_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "BLEND R");
    blend_r->SetParams(col_tone.blend.x, 0.0f, 5.0f, 0.4f, 0.01f, 0.1f);
    blend_r->format = "%4.2f";
    blend_r->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::SelectionListenerOnHook::CallbackFunc)ColorChangeDw::BlendRCallback));
    blend_r->callback_data.v64 = this;

    blend_g = dw::Slider::Create(color_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "BLEND G");
    blend_g->SetParams(col_tone.blend.y, 0.0f, 5.0f, 0.4f, 0.01f, 0.1f);
    blend_g->format = "%4.2f";
    blend_g->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::SelectionListenerOnHook::CallbackFunc)ColorChangeDw::BlendGCallback));
    blend_g->callback_data.v64 = this;

    blend_b = dw::Slider::Create(color_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "BLEND B");
    blend_b->SetParams(col_tone.blend.z, 0.0f, 5.0f, 0.4f, 0.01f, 0.1f);
    blend_b->format = "%4.2f";
    blend_b->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::SelectionListenerOnHook::CallbackFunc)ColorChangeDw::BlendBCallback));
    blend_b->callback_data.v64 = this;

    offset_r = dw::Slider::Create(color_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "OFFSET R");
    offset_r->SetParams(col_tone.offset.x, -1.0f, 1.0f, 0.4f, 0.01f, 0.1f);
    offset_r->format = "%4.2f";
    offset_r->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::SelectionListenerOnHook::CallbackFunc)ColorChangeDw::OffsetRCallback));
    offset_r->callback_data.v64 = this;

    offset_g = dw::Slider::Create(color_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "OFFSET G");
    offset_g->SetParams(col_tone.offset.y, -1.0f, 1.0f, 0.4f, 0.01f, 0.1f);
    offset_g->format = "%4.2f";
    offset_g->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::SelectionListenerOnHook::CallbackFunc)ColorChangeDw::OffsetGCallback));
    offset_g->callback_data.v64 = this;

    offset_b = dw::Slider::Create(color_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "OFFSET B");
    offset_b->SetParams(col_tone.offset.z, -1.0f, 1.0f, 0.4f, 0.01f, 0.1f);
    offset_b->format = "%4.2f";
    offset_b->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::SelectionListenerOnHook::CallbackFunc)ColorChangeDw::OffsetBCallback));
    offset_b->callback_data.v64 = this;

    hue = dw::Slider::Create(color_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "H");
    hue->SetParams(col_tone.hue, 0.0f, 360.0f, 72.0f, 1.0f, 10.0);
    hue->format = "%4.2f";
    hue->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::SelectionListenerOnHook::CallbackFunc)ColorChangeDw::HueCallback));
    hue->callback_data.v64 = this;

    saturation = dw::Slider::Create(color_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "S");
    saturation->SetParams(col_tone.saturation, 0.0f, 2.0f, 0.4f, 0.01f, 0.1f);
    saturation->format = "%4.2f";
    saturation->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::SelectionListenerOnHook::CallbackFunc)ColorChangeDw::SaturationCallback));
    saturation->callback_data.v64 = this;

    value = dw::Slider::Create(color_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "V");
    value->SetParams(col_tone.value, -1.0f, 1.0f, 0.4f, 0.01f, 0.1f);
    value->format = "%4.2f";
    value->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::SelectionListenerOnHook::CallbackFunc)ColorChangeDw::ValueCallback));
    value->callback_data.v64 = this;

    contrast = dw::Slider::Create(color_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "CONTRAST");
    contrast->SetParams(col_tone.contrast, 0.0f, 10.0, 2.0f, 0.01f, 0.1f);
    contrast->format = "%4.2f";
    contrast->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::SelectionListenerOnHook::CallbackFunc)ColorChangeDw::ContrastCallback));
    contrast->callback_data.v64 = this;

    copy_button = new dw::Button(color_group, dw::FLAG_8);
    copy_button->SetText("Copy");
    copy_button->callback = (dw::Widget::Callback)ColorChangeDw::CopyCallback;
    copy_button->callback_data.v64 = this;

    paset_button = new dw::Button(color_group, dw::FLAG_8);
    paset_button->SetText("Paste");
    paset_button->callback = (dw::Widget::Callback)ColorChangeDw::PasteCallback;
    paset_button->callback_data.v64 = this;

    reset_button = new dw::Button(color_group, dw::FLAG_8);
    reset_button->SetText("Reset");
    reset_button->callback = (dw::Widget::Callback)ColorChangeDw::ResetCallback;
    reset_button->callback_data.v64 = this;

    (new dw::Label(color_group, dw::FLAG_4000))->SetText(" ");

    save_button = new dw::Button(color_group, dw::FLAG_8);
    save_button->SetText("Save");
    save_button->callback = (dw::Widget::Callback)ColorChangeDw::SaveCallback;
    save_button->callback_data.v64 = this;
}

ColorChangeDw::~ColorChangeDw() {

}

void ColorChangeDw::Draw() {
    UpdateData();
    UpdateTextureInfo();
    GetSetColorTone();

    const texture* org_tex = GetOrgTex();
    const texture* chg_tex = GetChgTex();
    const color_tone* col_tone = GetColorTone();
    if (org_tex && chg_tex && col_tone)
        texture_apply_color_tone(org_tex, chg_tex, col_tone);

    if (show) {
        data_struct* aft_data = &data_list[DATA_AFT];
        sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

        if (org_tex) {
            spr::SprArgs args;
            args.texture = org_tex;
            if (auto_resize)
                args.SetSpriteSize(256.0f);
            spr::put_sprite(args, aft_spr_db);
        }
        if (chg_tex) {
            spr::SprArgs args;
            args.texture = chg_tex;
            args.trans.x = (float_t)chg_tex->width;
            if (auto_resize) {
                args.SetSpriteSize(256.0f);
                args.trans.x = 266.0;
            }
            spr::put_sprite(args, aft_spr_db);
        }
    }

    dw::Shell::Draw();
}

void ColorChangeDw::Reset() {
    if (row_layout) {
        delete row_layout;
        row_layout = 0;
    }
    dw::Shell::Reset();
}

void ColorChangeDw::Hide() {
    SetDisp();
}

texture* ColorChangeDw::GetChgTex() {
    if (color_change_index < textures.size())
        return textures.data()[color_change_index];
    return 0;
}

color_tone* ColorChangeDw::GetColorTone() {
    if (color_change_index < color_tones.size())
        return &color_tones.data()[color_change_index];
    return 0;
}

void ColorChangeDw::GetColorToneDefault(color_tone& value) {
    value = color_tone();
}

const item_table_item* ColorChangeDw::GetItem(int32_t item_index) {
    int32_t item_no = GetItemNo(item_index);
    if (item_no != -1)
        return item_table_handler_array_get_item(this->chara_index, item_no);
    return 0;
}

void ColorChangeDw::GetItemColTone() {
    color_tone* col_tone = GetColorTone();
    if (col_tone) {
        const item_table_item* item = GetItem(item_index);
        if (item && item_texture_index < item->data.col.size())
            *col_tone = item->data.col.data()[item_texture_index].col_tone;
    }
}

const item_table_item_data* ColorChangeDw::GetItemData(int32_t item_index) {
    const item_table_item* item = GetItem(item_index);
    if (item)
        return &item->data;
    return 0;
}

int32_t ColorChangeDw::GetItemNo(int32_t item_index) {
    int32_t index = 0;
    const item_table* item = item_table_handler_array_get_table(chara_index);
    if (item)
        for (auto& i : item->item)
            if (i.second.attr & 0x68) {
                if (item_index == index)
                    return 0;
                index++;
            }
    return -1;
}

const char* ColorChangeDw::GetItemTexChangeName() {
    data_struct* aft_data = &data_list[DATA_AFT];
    texture_database* aft_tex_db = &aft_data->data_ft.tex_db;

    std::string item = item_texture_names->GetItemStr(item_texture_index);
    const item_table_item_data* itm_data = GetItemData(item_index);
    if (itm_data)
        for (const item_table_item_data_tex& i : itm_data->tex)
            if (!item.compare(aft_tex_db->get_texture_name(i.org)))
                return aft_tex_db->get_texture_name(i.chg);
    return 0;
}

texture* ColorChangeDw::GetOrgTex() {
    texture** textures = objset_info_storage_get_obj_set_textures(obj_set_id);
    if (textures && color_change_index < objset_info_storage_get_obj_set_tex_num(obj_set_id))
        return textures[color_change_index];
    return 0;
}

void ColorChangeDw::GetSetColorTone() {
    color_tone* col_tone = GetColorTone();
    if (col_tone) {
        inverse->SetValue(col_tone->inverse);
        blend_r->SetValue(col_tone->blend.x);
        blend_g->SetValue(col_tone->blend.y);
        blend_b->SetValue(col_tone->blend.z);
        offset_r->SetValue(col_tone->offset.x);
        offset_g->SetValue(col_tone->offset.y);
        offset_b->SetValue(col_tone->offset.z);
        hue->SetValue(col_tone->hue);
        saturation->SetValue(col_tone->saturation);
        value->SetValue(col_tone->value);
        contrast->SetValue(col_tone->contrast);
    }
}

void ColorChangeDw::GetTextureNames() {
    data_struct* aft_data = &data_list[DATA_AFT];
    texture_database* aft_tex_db = &aft_data->data_ft.tex_db;

    if (!texture_names)
        return;

    texture_names->ClearItems();

    const int32_t tex_num = objset_info_storage_get_obj_set_tex_num(obj_set_id);
    for (int32_t i = 0; i < tex_num; i++) {
        uint32_t tex_id = objset_info_storage_get_obj_set_tex_id(obj_set_id, i);

        char buf[64];
        sprintf_s(buf, sizeof(buf), "%s", aft_tex_db->get_texture_name(tex_id));
        texture_names->AddItem(buf);
    }

    texture_names->SetItemIndex(0);
}

bool ColorChangeDw::LoadColorTone() {
    ResetColorTone();
    if (!textures.size())
        return false;

    color_tone col_tone;
    ColorChangeDw::GetColorToneDefault(col_tone);
    for (texture*& i : textures)
        color_tones.push_back(col_tone);
    return true;
}

void ColorChangeDw::LoadItemNames() {

}

bool ColorChangeDw::LoadTexture() {
    ResetTexture();

    texture** textures = objset_info_storage_get_obj_set_textures(obj_set_id);
    if (!textures)
        return false;

    const int32_t tex_num = objset_info_storage_get_obj_set_tex_num(obj_set_id);
    for (int32_t i = 0; i < tex_num; i++) {
        texture* tex = 0;
        if (textures[i]->target == GL_TEXTURE_2D) {
            tex = texture_create_copy_texture(texture_id(0x22, i), textures[i]);
            if (!tex)
                break;
        }
        this->textures.push_back(tex);
    }
    return true;
}

void ColorChangeDw::ReloadData() {
    ResetData();
    if (LoadTexture() && LoadColorTone())
        set = true;
    else
        ResetData();
}

void ColorChangeDw::ResetColorTone() {
    color_tones.clear();
}

void ColorChangeDw::ResetData() {
    set = false;
    ResetTexture();
    ResetColorTone();
}

void ColorChangeDw::ResetTexture() {
    if (!textures.size())
        return;

    for (texture*& i : textures)
        if (i) {
            texture_release(i);
            i = 0;
        }
    textures.clear();
}

void ColorChangeDw::SetObjectSet(const char* str) {
    if (!str)
        return;

    const size_t size = object_sets->GetItemCount();
    for (size_t i = 0; i < size; i++) {
        std::string item = object_sets->GetItemStr(i);
        if (!item.compare(str)) {
            object_sets->list->SetItemIndex(i);
            ColorChangeDw::ObjectSetCallback(object_sets);
            break;
        }
    }
}

void ColorChangeDw::SetTextureName(const char* name) {
    if (!name)
        return;

    const size_t size = texture_names->GetItemCount();
    for (size_t i = 0; i < size; i++) {
        std::string item = texture_names->GetItemStr(i);
        if (!item.compare(name)) {
            object_sets->list->SetItemIndex(i);
            ColorChangeDw::TextureCallback(texture_names);
            break;
        }
    }
}

void ColorChangeDw::UpdateData() {
    if (!set)
        ReloadData();
    else if (!objset_info_storage_get_obj_set_textures(obj_set_id))
        ResetData();
}

void ColorChangeDw::UpdateItemTextureNames() {
    data_struct* aft_data = &data_list[DATA_AFT];
    texture_database* aft_tex_db = &aft_data->data_ft.tex_db;

    if (!item_texture_names)
        return;

    item_texture_names->ClearItems();
    const item_table_item_data* itm_data = GetItemData(item_index);
    if (!itm_data)
        return;

    for (const item_table_item_data_col& i : itm_data->col) {
        const char* name = aft_tex_db->get_texture_name(i.tex_id);
        if (name)
            item_texture_names->AddItem(name);
    }

    item_texture_names->SetItemIndex(0);
    item_texture_index = 0;

    ColorChangeDw::ItemTextureCallback(item_texture_names);
}

void ColorChangeDw::UpdateTextureInfo() {
    const texture* org_tex = GetOrgTex();
    if (org_tex) {
        std::string type("Type   ");
        type.append(texture_get_target_name(org_tex->target));
        this->type->SetText(type);

        std::string format("Format ");
        format.append(texture_get_internal_format_name(org_tex->internal_format));
        this->format->SetText(format);

        std::string size("Size   ");
        size.append(sprintf_s_string("%d", org_tex->width));
        size.append("x");
        size.append(sprintf_s_string("%d", org_tex->height));
        this->size->SetText(size);
    }
    else {
        type->SetText("Type   ****");
        format->SetText("Format ****");
        size->SetText("Size   ****");
    }
}

void ColorChangeDw::AutoResizeCallback(dw::Button* data) {
    ColorChangeDw* color_change_dw = (ColorChangeDw*)data->callback_data.v64;
    color_change_dw->auto_resize = data->value;
}

void ColorChangeDw::BlendBCallback(dw::Slider* data) {
    ColorChangeDw* color_change_dw = (ColorChangeDw*)data->callback_data.v64;
    color_tone* col_tone = color_change_dw->GetColorTone();
    if (col_tone)
        col_tone->blend.z = data->scroll_bar->value;
}

void ColorChangeDw::BlendGCallback(dw::Slider* data) {
    ColorChangeDw* color_change_dw = (ColorChangeDw*)data->callback_data.v64;
    color_tone* col_tone = color_change_dw->GetColorTone();
    if (col_tone)
        col_tone->blend.y = data->scroll_bar->value;
}

void ColorChangeDw::BlendRCallback(dw::Slider* data) {
    ColorChangeDw* color_change_dw = (ColorChangeDw*)data->callback_data.v64;
    color_tone* col_tone = color_change_dw->GetColorTone();
    if (col_tone)
        col_tone->blend.x = data->scroll_bar->value;
}

void ColorChangeDw::ContrastCallback(dw::Slider* data) {
    ColorChangeDw* color_change_dw = (ColorChangeDw*)data->callback_data.v64;
    color_tone* col_tone = color_change_dw->GetColorTone();
    if (col_tone)
        col_tone->contrast = data->scroll_bar->value;
}

void ColorChangeDw::CopyCallback(dw::Button* data) {
    ColorChangeDw* color_change_dw = (ColorChangeDw*)data->callback_data.v64;
    color_tone* col_tone = color_change_dw->GetColorTone();
    if (col_tone)
        color_change_dw->color_tone_temp = *col_tone;
}

void ColorChangeDw::HueCallback(dw::Slider* data) {
    ColorChangeDw* color_change_dw = (ColorChangeDw*)data->callback_data.v64;
    color_tone* col_tone = color_change_dw->GetColorTone();
    if (col_tone)
        col_tone->hue = data->scroll_bar->value;
}

void ColorChangeDw::InverseCallback(dw::Button* data) {
    ColorChangeDw* color_change_dw = (ColorChangeDw*)data->callback_data.v64;
    color_tone* col_tone = color_change_dw->GetColorTone();
    if (col_tone)
        col_tone->inverse = data->value;
}

void ColorChangeDw::ItemCallback(dw::ListBox* data) {
    data_struct* aft_data = &data_list[DATA_AFT];
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    int32_t index = (int32_t)data->list->selected_item;
    ColorChangeDw* color_change_dw = (ColorChangeDw*)data->callback_data.v64;
    color_change_dw->item_index = index;

    const item_table_item* item = color_change_dw->GetItem(index);
    if (!item) {
        color_change_dw->UpdateItemTextureNames();
        return;
    }

    const char* name = 0;
    for (const uint32_t& i : item->objset)
        name = aft_obj_db->get_object_set_name(i);

    if (item->attr & 0x40)
        name = aft_obj_db->get_object_set_name(chara_init_data_get(color_change_dw->chara_index)->object_set);

    if (name) {
        color_change_dw->SetObjectSet(name);
        color_change_dw->UpdateItemTextureNames();
    }
}

void ColorChangeDw::ItemCharaCallback(dw::ListBox* data) {
    ColorChangeDw* color_change_dw = (ColorChangeDw*)data->callback_data.v64;
    color_change_dw->LoadItemNames();
    color_change_dw->ItemCallback(color_change_dw->item_names);
}

void ColorChangeDw::ItemTextureCallback(dw::ListBox* data) {
    ColorChangeDw* color_change_dw = (ColorChangeDw*)data->callback_data.v64;
    color_change_dw->item_texture_index = data->list->selected_item;

    const item_table_item* item = color_change_dw->GetItem(color_change_dw->item_index);
    if (!item)
        return;

    const size_t size = data->GetItemCount();
    if (!size || color_change_dw->item_texture_index >= size)
        return;

    if (item->attr & 0x04) {
        const char* name = color_change_dw->GetItemTexChangeName();
        if (name)
            color_change_dw->SetTextureName(name);
    }
    else {
        std::string item = data->GetItemStr(color_change_dw->item_texture_index);
        color_change_dw->SetTextureName(item.c_str());
    }
}

void ColorChangeDw::ObjectSetCallback(dw::ListBox* data) {
    data_struct* aft_data = &data_list[DATA_AFT];
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    ColorChangeDw* color_change_dw = (ColorChangeDw*)data->callback_data.v64;
    int32_t obj_set_index = (int32_t)data->list->selected_item;
    color_change_dw->obj_set_index = obj_set_index;

    uint32_t obj_set_id = aft_obj_db->get_object_set_id(obj_set_index);
    if (obj_set_id != color_change_dw->obj_set_id) {
        color_change_dw->obj_set_id = obj_set_id;
        color_change_dw->GetTextureNames();
        color_change_dw->ResetData();
        color_change_dw->ReloadData();
    }
}

void ColorChangeDw::OffsetBCallback(dw::Slider* data) {
    ColorChangeDw* color_change_dw = (ColorChangeDw*)data->callback_data.v64;
    color_tone* col_tone = color_change_dw->GetColorTone();
    if (col_tone)
        col_tone->offset.z = data->scroll_bar->value;
}

void ColorChangeDw::OffsetGCallback(dw::Slider* data) {
    ColorChangeDw* color_change_dw = (ColorChangeDw*)data->callback_data.v64;
    color_tone* col_tone = color_change_dw->GetColorTone();
    if (col_tone)
        col_tone->offset.y = data->scroll_bar->value;
}

void ColorChangeDw::OffsetRCallback(dw::Slider* data) {
    ColorChangeDw* color_change_dw = (ColorChangeDw*)data->callback_data.v64;
    color_tone* col_tone = color_change_dw->GetColorTone();
    if (col_tone)
        col_tone->offset.x = data->scroll_bar->value;
}

void ColorChangeDw::OverwriteCallback(dw::Button* data) {

}

void ColorChangeDw::PasteCallback(dw::Button* data) {
    ColorChangeDw* color_change_dw = (ColorChangeDw*)data->callback_data.v64;
    color_tone* col_tone = color_change_dw->GetColorTone();
    if (col_tone)
        *col_tone = color_change_dw->color_tone_temp;
}

void ColorChangeDw::ResetCallback(dw::Button* data) {
    ColorChangeDw* color_change_dw = (ColorChangeDw*)data->callback_data.v64;
    color_tone* col_tone = color_change_dw->GetColorTone();
    if (col_tone)
        color_change_dw->GetColorToneDefault(*col_tone);
}

void ColorChangeDw::SaturationCallback(dw::Slider* data) {
    ColorChangeDw* color_change_dw = (ColorChangeDw*)data->callback_data.v64;
    color_tone* col_tone = color_change_dw->GetColorTone();
    if (col_tone)
        col_tone->saturation = data->scroll_bar->value;
}

void ColorChangeDw::SaveCallback(dw::Button* data) {

}

void ColorChangeDw::ShowCallback(dw::Button* data) {
    ColorChangeDw* color_change_dw = (ColorChangeDw*)data->callback_data.v64;
    color_change_dw->show = data->value;
}

void ColorChangeDw::TextureCallback(dw::ListBox* data) {
    int32_t index = (int32_t)data->list->selected_item;
    ColorChangeDw* color_change_dw = (ColorChangeDw*)data->callback_data.v64;
    if (objset_info_storage_get_obj_set_textures(color_change_dw->obj_set_id)
        && index < objset_info_storage_get_obj_set_tex_num(color_change_dw->obj_set_id))
        color_change_dw->color_change_index = index;
}

void ColorChangeDw::ValueCallback(dw::Slider* data) {
    ColorChangeDw* color_change_dw = (ColorChangeDw*)data->callback_data.v64;
    color_tone* col_tone = color_change_dw->GetColorTone();
    if (col_tone)
        col_tone->value = data->scroll_bar->value;
}

static const char* get_dev_ram_color_change_param_dir() {
    return "dev_ram/color_change_param";
}
