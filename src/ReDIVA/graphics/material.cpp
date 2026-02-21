/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "material.hpp"
#include "../../CRE/light_param/face.hpp"
#include "../../CRE/light_param.hpp"
#include "../../CRE/render_context.hpp"
#include "../dw.hpp"

extern render_context* rctx_ptr;

class MaterialDw : public dw::Shell {
public:
    dw::Button* refresh;
    dw::ListBox* object_sets;
    dw::ListBox* objects;
    dw::ListBox* object_meshes;
    dw::Slider* diffuse_r;
    dw::Slider* diffuse_g;
    dw::Slider* diffuse_b;
    dw::Slider* transparency;
    dw::Slider* specular_r;
    dw::Slider* specular_g;
    dw::Slider* specular_b;
    dw::Slider* reflectivity;
    dw::Slider* shininess;
    dw::Slider* ambient_r;
    dw::Slider* ambient_g;
    dw::Slider* ambient_b;
    dw::Slider* ambient_a;
    dw::Slider* emission_r;
    dw::Slider* emission_g;
    dw::Slider* emission_b;
    dw::Slider* emission_a;
    dw::Slider* intensity;
    dw::Slider* bump_depth;
    dw::Slider* fresnel;
    dw::Slider* line_light;
    dw::ListBox* specular_quality;
    dw::ListBox* aniso_direction;
    dw::ListBox* double_sided;
    dw::ListBox* src_blend_factor;
    dw::ListBox* dst_blend_factor;
    dw::Slider* zbias;
    dw::Slider* mipmap_bias;
    dw::Slider* aniso_filter;
    dw::Layout* vertical_layout;
    dw::Layout* horizontal_layout;
    uint32_t obj_set_id;
    int32_t obj_index;
    int32_t material_index;
    obj_material_data material_array[100];

    MaterialDw();
    virtual ~MaterialDw() override;
    virtual void Draw() override;

    virtual void Hide() override;

    obj_material_data* GetMaterial(bool own = false);
    obj* GetObj();
    void ResetMaterialArray();
    void SetValue(bool own = false);
    void UpdateObjectMeshes();
    void UpdateObjects();

    static void AmbientACallback(dw::Slider* data);
    static void AmbientBCallback(dw::Slider* data);
    static void AmbientGCallback(dw::Slider* data);
    static void AmbientRCallback(dw::Slider* data);
    static void AnisoDirectionCallback(dw::ListBox* data);
    static void AnisoFilterCallback(dw::Slider* data);
    static void BumpDepthCallback(dw::Slider* data);
    static void DiffuseBCallback(dw::Slider* data);
    static void DiffuseGCallback(dw::Slider* data);
    static void DiffuseRCallback(dw::Slider* data);
    static void DoubleSidedCallback(dw::ListBox* data);
    static void DstBlendFactorCallback(dw::ListBox* data);
    static void EmissionACallback(dw::Slider* data);
    static void EmissionBCallback(dw::Slider* data);
    static void EmissionGCallback(dw::Slider* data);
    static void EmissionRCallback(dw::Slider* data);
    static void FresnelCallback(dw::Slider* data);
    static void IntensityCallback(dw::Slider* data);
    static void LineLightCallback(dw::Slider* data);
    static void MipmapBiasCallback(dw::Slider* data);
    static void ObjectCallback(dw::ListBox* data);
    static void ObjectMeshCallback(dw::ListBox* data);
    static void ObjectSetCallback(dw::ListBox* data);
    static void ReflectivityCallback(dw::Slider* data);
    static void RefreshCallback(dw::Widget* data);
    static void ResetCallback(dw::Widget* data);
    static void ShininessCallback(dw::Slider* data);
    static void SpecularBCallback(dw::Slider* data);
    static void SpecularGCallback(dw::Slider* data);
    static void SpecularQualityCallback(dw::ListBox* data);
    static void SpecularRCallback(dw::Slider* data);
    static void SrcBlendFactorCallback(dw::ListBox* data);
    static void TransparencyCallback(dw::Slider* data);
    static void ZBiasCallback(dw::Slider* data);
};

MaterialDw* material_dw;

void material_dw_init() {
    if (!material_dw) {
        material_dw = new MaterialDw;
        material_dw->LimitPosDisp();
        material_dw->UpdateLayout();
    }
    else
        material_dw->Disp();
}

MaterialDw::MaterialDw() {
    data_struct* aft_data = &data_list[DATA_AFT];
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    obj_set_id = -1;
    obj_index = 0;
    material_index = 0;

    SetText("MATERIAL");

    rect.pos = 0.0f;
    SetSize({ 512.0f, 200.0f });

    const float_t glyph_height = font.GetFontGlyphHeight();

    vertical_layout = new dw::RowLayout(dw::VERTICAL);
    horizontal_layout = new dw::RowLayout(dw::HORIZONTAL);

    dw::Composite* comp = new dw::Composite(this);
    comp->layout = vertical_layout;

    dw::Group* object_select_group = new dw::Group(comp);
    object_select_group->SetText("OBJECT SELECT");

    dw::Composite* object_select_comp = new dw::Composite(object_select_group);
    object_select_comp->layout = horizontal_layout;

    refresh = new dw::Button(object_select_comp, dw::FLAG_8);
    refresh->SetText("Refresh");
    refresh->callback = (dw::Widget::Callback)MaterialDw::RefreshCallback;

    dw::Button* reset_button = new dw::Button(object_select_comp, dw::FLAG_8);
    reset_button->SetText("Reset");
    reset_button->callback = (dw::Widget::Callback)MaterialDw::ResetCallback;

    object_sets = new dw::ListBox(object_select_comp, (dw::Flags)(dw::FLAG_800 | dw::VERTICAL | dw::MULTISELECT));
    object_sets->SetText("listbox0");
    uint32_t obj_set_count = aft_obj_db->get_object_set_count();
    for (uint32_t i = obj_set_count, j = 0; i; i--, j++) {
        uint32_t obj_set_id = aft_obj_db->get_object_set_id(j);
        if (objset_info_storage_get_obj_set_loaded(obj_set_id))
            object_sets->AddItem(aft_obj_db->get_object_set_name(obj_set_id));
    }
    object_sets->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)MaterialDw::ObjectSetCallback));
    object_sets->SetItemIndex(0);
    object_sets->SetMaxItems(20);
    object_sets->callback_data.v64 = this;

    objects = new dw::ListBox(object_select_group, (dw::Flags)(dw::FLAG_800 | dw::VERTICAL | dw::MULTISELECT));
    objects->SetText("listbox0");
    objects->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)MaterialDw::ObjectCallback));
    objects->SetItemIndex(0);
    objects->SetMaxItems(20);
    objects->callback_data.v64 = this;
    objects->AddItem("                             ");

    object_meshes = new dw::ListBox(object_select_group, (dw::Flags)(dw::FLAG_800 | dw::VERTICAL | dw::MULTISELECT));
    object_meshes->SetText("listbox1");
    object_meshes->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)MaterialDw::ObjectMeshCallback));
    object_meshes->SetItemIndex(0);
    object_meshes->SetMaxItems(20);
    object_meshes->callback_data.v64 = this;

    dw::Group* material_group = new dw::Group(comp);
    material_group->SetText("MATERIAL");

    diffuse_r = dw::Slider::Create(material_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "DIFFUSE R");
    diffuse_r->SetParams(0.0f, 0.0f, 1.0f, 0.2f, 0.01f, 0.1f);
    diffuse_r->format = "%4.2f";
    diffuse_r->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)MaterialDw::DiffuseRCallback));
    diffuse_r->callback_data.v64 = this;

    diffuse_g = dw::Slider::Create(material_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "DIFFUSE G");
    diffuse_g->SetParams(0.0f, 0.0f, 1.0f, 0.2f, 0.01f, 0.1f);
    diffuse_g->format = "%4.2f";
    diffuse_g->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)MaterialDw::DiffuseGCallback));
    diffuse_g->callback_data.v64 = this;

    diffuse_b = dw::Slider::Create(material_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "DIFFUSE B");
    diffuse_b->SetParams(0.0f, 0.0f, 1.0f, 0.2f, 0.01f, 0.1f);
    diffuse_b->format = "%4.2f";
    diffuse_b->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)MaterialDw::DiffuseBCallback));
    diffuse_b->callback_data.v64 = this;

    transparency = dw::Slider::Create(material_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "TRANSPARENCY");
    transparency->SetParams(0.0f, 0.0f, 1.0f, 0.2f, 0.01f, 0.1f);
    transparency->format = "%4.2f";
    transparency->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)MaterialDw::TransparencyCallback));
    transparency->callback_data.v64 = this;

    specular_r = dw::Slider::Create(material_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "SPECULAR R");
    specular_r->SetParams(0.0f, 0.0f, 1.0f, 0.2f, 0.01f, 0.1f);
    specular_r->format = "%4.2f";
    specular_r->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)MaterialDw::SpecularRCallback));
    specular_r->callback_data.v64 = this;

    specular_g = dw::Slider::Create(material_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "SPECULAR G");
    specular_g->SetParams(0.0f, 0.0f, 1.0f, 0.2f, 0.01f, 0.1f);
    specular_g->format = "%4.2f";
    specular_g->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)MaterialDw::SpecularGCallback));
    specular_g->callback_data.v64 = this;

    specular_b = dw::Slider::Create(material_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "SPECULAR B");
    specular_b->SetParams(0.0f, 0.0f, 1.0f, 0.2f, 0.01f, 0.1f);
    specular_b->format = "%4.2f";
    specular_b->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)MaterialDw::SpecularBCallback));
    specular_b->callback_data.v64 = this;

    reflectivity = dw::Slider::Create(material_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "REFLECTIVITY");
    reflectivity->SetParams(0.0f, 0.0f, 5.0, 1.0f, 0.01f, 0.1f);
    reflectivity->format = "%4.2f";
    reflectivity->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)MaterialDw::ReflectivityCallback));
    reflectivity->callback_data.v64 = this;

    shininess = dw::Slider::Create(material_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "SHININESS");
    shininess->SetParams(0.0f, 0.0f, 128.0, 24.0, 0.1f, 1.0f);
    shininess->format = "%4.2f";
    shininess->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)MaterialDw::ShininessCallback));
    shininess->callback_data.v64 = this;

    ambient_r = dw::Slider::Create(material_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "AMBIENT R");
    ambient_r->SetParams(0.0f, 0.0f, 1.0f, 0.2f, 0.01f, 0.1f);
    ambient_r->format = "%4.2f";
    ambient_r->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)MaterialDw::AmbientRCallback));
    ambient_r->callback_data.v64 = this;

    ambient_g = dw::Slider::Create(material_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "AMBIENT G");
    ambient_g->SetParams(0.0f, 0.0f, 1.0f, 0.2f, 0.01f, 0.1f);
    ambient_g->format = "%4.2f";
    ambient_g->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)MaterialDw::AmbientGCallback));
    ambient_g->callback_data.v64 = this;

    ambient_b = dw::Slider::Create(material_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "AMBIENT B");
    ambient_b->SetParams(0.0f, 0.0f, 1.0f, 0.2f, 0.01f, 0.1f);
    ambient_b->format = "%4.2f";
    ambient_b->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)MaterialDw::AmbientBCallback));
    ambient_b->callback_data.v64 = this;

    ambient_a = dw::Slider::Create(material_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "AMBIENT A");
    ambient_a->SetParams(0.0f, 0.0f, 1.0f, 0.2f, 0.01f, 0.1f);
    ambient_a->format = "%4.2f";
    ambient_a->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)MaterialDw::AmbientACallback));
    ambient_a->callback_data.v64 = this;

    emission_r = dw::Slider::Create(material_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "EMISSION R");
    emission_r->SetParams(0.0f, 0.0f, 50.0f, 10.0f, 0.01f, 0.1f);
    emission_r->format = "%4.2f";
    emission_r->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)MaterialDw::EmissionRCallback));
    emission_r->callback_data.v64 = this;

    emission_g = dw::Slider::Create(material_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "EMISSION G");
    emission_g->SetParams(0.0f, 0.0f, 50.0f, 10.0f, 0.01f, 0.1f);
    emission_g->format = "%4.2f";
    emission_g->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)MaterialDw::EmissionGCallback));
    emission_g->callback_data.v64 = this;

    emission_b = dw::Slider::Create(material_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "EMISSION B");
    emission_b->SetParams(0.0f, 0.0f, 50.0f, 10.0f, 0.01f, 0.1f);
    emission_b->format = "%4.2f";
    emission_b->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)MaterialDw::EmissionBCallback));
    emission_b->callback_data.v64 = this;

    emission_a = dw::Slider::Create(material_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "EMISSION A");
    emission_a->SetParams(0.0f, 0.0f, 1.0f, 0.2f, 0.01f, 0.1f);
    emission_a->format = "%4.2f";
    emission_a->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)MaterialDw::EmissionACallback));
    emission_a->callback_data.v64 = this;

    intensity = dw::Slider::Create(material_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "INTENSITY");
    intensity->SetParams(0.0f, 0.0f, 100.0f, 20.0, 0.1f, 1.0f);
    intensity->format = "%4.2f";
    intensity->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)MaterialDw::IntensityCallback));
    intensity->callback_data.v64 = this;

    bump_depth = dw::Slider::Create(material_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "BUMP DEPTH");
    bump_depth->SetParams(0.0f, 0.0f, 1.0f, 0.2f, 0.01f, 0.1f);
    bump_depth->format = "%4.2f";
    bump_depth->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)MaterialDw::BumpDepthCallback));
    bump_depth->callback_data.v64 = this;

    dw::Group* material_flag_group = new dw::Group(comp);
    material_flag_group->SetText("MATERIAL FLAG");

    fresnel = dw::Slider::Create(material_flag_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "FRESNEL      F(n)");
    fresnel->SetParams(0.0f, 0.0f, 9.0f, 3.0f, 1.0f, 1.0f);
    fresnel->format = "%4.0f";
    fresnel->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)MaterialDw::FresnelCallback));
    fresnel->callback_data.v64 = this;

    line_light = dw::Slider::Create(material_flag_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "LINE LIGHT   T(n)");
    line_light->SetParams(0.0f, 0.0f, 9.0f, 1.8f, 1.0f, 1.0f);
    line_light->format = "%4.0f";
    line_light->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)MaterialDw::LineLightCallback));
    line_light->callback_data.v64 = this;

    dw::Composite* specular_quality_comp = new dw::Composite(material_flag_group);
    specular_quality_comp->layout = horizontal_layout;

    (new dw::Label(specular_quality_comp, dw::FLAG_4000))->SetText("SPECULAR QUALITY      ");

    specular_quality = new dw::ListBox(specular_quality_comp,
        (dw::Flags)(dw::FLAG_800 | dw::VERTICAL | dw::MULTISELECT));
    specular_quality->SetText("listbox0");
    specular_quality->AddItem("H0:LOW");
    specular_quality->AddItem("H1:HIGH");

    specular_quality->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)MaterialDw::SpecularQualityCallback));
    specular_quality->SetItemIndex(0);
    specular_quality->SetMaxItems(2);
    specular_quality->callback_data.v64 = this;

    dw::Composite* aniso_direction_comp = new dw::Composite(material_flag_group);
    aniso_direction_comp->layout = horizontal_layout;

    (new dw::Label(aniso_direction_comp, dw::FLAG_4000))->SetText("ANISO DIRECTION       ");

    aniso_direction = new dw::ListBox(aniso_direction_comp,
        (dw::Flags)(dw::FLAG_800 | dw::VERTICAL | dw::MULTISELECT));
    aniso_direction->SetText("listbox0");
    aniso_direction->AddItem("A0:NORMAL");
    aniso_direction->AddItem("A1:U");
    aniso_direction->AddItem("A2:V");
    aniso_direction->AddItem("A3:RADIAL");
    aniso_direction->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)MaterialDw::AnisoDirectionCallback));
    aniso_direction->SetItemIndex(0);
    aniso_direction->SetMaxItems(4);
    aniso_direction->callback_data.v64 = this;

    dw::Composite* double_sized_comp = new dw::Composite(material_flag_group);
    double_sized_comp->layout = horizontal_layout;

    (new dw::Label(double_sized_comp, dw::FLAG_4000))->SetText("DOUBLE SIDED          ");

    double_sided = new dw::ListBox(double_sized_comp,
        (dw::Flags)(dw::FLAG_800 | dw::VERTICAL | dw::MULTISELECT));
    double_sided->SetText("listbox0");
    double_sided->AddItem("OFF");
    double_sided->AddItem("D :2-FACE LIGHT");
    double_sided->AddItem("D1:1-FACE LIGHT");
    double_sided->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)MaterialDw::DoubleSidedCallback));
    double_sided->SetItemIndex(0);
    double_sided->SetMaxItems(3);
    double_sided->SetCallbackData(this);

    dw::Composite* src_blend_factor_comp = new dw::Composite(material_flag_group);
    src_blend_factor_comp->SetLayout(horizontal_layout);

    (new dw::Label(src_blend_factor_comp, dw::FLAG_4000))->SetText("SRC BLEND FACTOR      ");

    src_blend_factor = new dw::ListBox(src_blend_factor_comp,
        (dw::Flags)(dw::FLAG_800 | dw::VERTICAL | dw::MULTISELECT));
    src_blend_factor->SetText("listbox0");

    src_blend_factor->AddItem("R0:ZERO");
    src_blend_factor->AddItem("R1:ONE");
    src_blend_factor->AddItem("R2:SRC COLOR");
    src_blend_factor->AddItem("R3:ISRC COLOR");
    src_blend_factor->AddItem("R4:SRC ALPHA");
    src_blend_factor->AddItem("R5:ISRC ALPHA");
    src_blend_factor->AddItem("R6:DST ALPHA");
    src_blend_factor->AddItem("R7:IDST ALPHA");
    src_blend_factor->AddItem("R8:DST COLOR");
    src_blend_factor->AddItem("R9:IDST COLOR");
    src_blend_factor->AddItem("R10:SRC A SAT");
    src_blend_factor->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)MaterialDw::SrcBlendFactorCallback));
    src_blend_factor->SetItemIndex(0);
    src_blend_factor->SetMaxItems(11);
    src_blend_factor->SetCallbackData(this);

    dw::Composite* dst_blend_factor_comp = new dw::Composite(material_flag_group);
    dst_blend_factor_comp->SetLayout(horizontal_layout);

    (new dw::Label(dst_blend_factor_comp, dw::FLAG_4000))->SetText("DST BLEND FACTOR      ");

    dst_blend_factor = new dw::ListBox(dst_blend_factor_comp,
        (dw::Flags)(dw::FLAG_800 | dw::VERTICAL | dw::MULTISELECT));
    dst_blend_factor->SetText("listbox0");
    dst_blend_factor->AddItem("S0:ZERO");
    dst_blend_factor->AddItem("S1:ONE");
    dst_blend_factor->AddItem("S2:SRC COLOR");
    dst_blend_factor->AddItem("S3:ISRC COLOR");
    dst_blend_factor->AddItem("S4:SRC ALPHA");
    dst_blend_factor->AddItem("S5:ISRC ALPHA");
    dst_blend_factor->AddItem("S6:DST ALPHA");
    dst_blend_factor->AddItem("S7:IDST ALPHA");
    dst_blend_factor->AddItem("S8:DST COLOR");
    dst_blend_factor->AddItem("S9:IDST COLOR");
    dst_blend_factor->AddItem("S10:SRC A SAT");

    dst_blend_factor->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)MaterialDw::DstBlendFactorCallback));
    dst_blend_factor->SetItemIndex(0);
    dst_blend_factor->SetMaxItems(11);
    dst_blend_factor->SetCallbackData(this);

    zbias = dw::Slider::Create(material_flag_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "ZBIAS        X(n)");
    zbias->SetParams(0.0f, 0.0f, 15.0f, 3.0f, 1.0f, 1.0f);
    zbias->SetFormat("%4.0f");
    zbias->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)MaterialDw::ZBiasCallback));
    zbias->SetCallbackData(this);

    dw::Group* texture_flag_group = new dw::Group(comp);
    texture_flag_group->SetText("TEXTURE FLAG");

    mipmap_bias = dw::Slider::Create(texture_flag_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "MIPMAP BIAS  J(n)");
    mipmap_bias->SetParams(0.0f, 0.0f, 99.0f, 18.0f, 1.0f, 10.0f);
    mipmap_bias->SetFormat("%4.0f");
    mipmap_bias->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)MaterialDw::MipmapBiasCallback));
    mipmap_bias->SetCallbackData(this);

    aniso_filter = dw::Slider::Create(texture_flag_group, (dw::Flags)(dw::FLAG_800 | dw::HORIZONTAL),
        0.0f, 0.0f, 100.0f, glyph_height, "ANISO FILTER S(n)");
    aniso_filter->SetParams(0.0f, 0.0f, 3.0f, 0.6f, 1.0f, 1.0f);
    aniso_filter->SetFormat("%4.0f");
    aniso_filter->AddSelectionListener(new dw::SelectionListenerOnHook(
        (dw::Widget::Callback)MaterialDw::AnisoFilterCallback));
    aniso_filter->SetCallbackData(this);

    ResetMaterialArray();
}

MaterialDw::~MaterialDw() {

}

void MaterialDw::Draw() {
    dw::Shell::Draw();
}

void MaterialDw::Hide() {
    SetDisp();
}

obj_material_data* MaterialDw::GetMaterial(bool own) {
    if (own) {
        if (material_index < 100)
            return &material_array[material_index];
    }
    else {
        obj* obj = GetObj();
        if (obj && material_index < obj->num_material)
            return &obj->material_array[material_index];
    }
    return 0;
}

obj* MaterialDw::GetObj() {
    data_struct* aft_data = &data_list[DATA_AFT];
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    uint32_t obj_id = objset_info_storage_get_obj_set_obj_id(obj_set_id, obj_index);
    return objset_info_storage_get_obj({ obj_id, obj_set_id });
}

void MaterialDw::ResetMaterialArray() {
    for (int32_t i = 0; i < 100; i++)
        material_array[i] = {};
}

void MaterialDw::SetValue(bool own) {
    obj_material_data* material_data = GetMaterial(own);
    if (!material_data)
        return;

    obj_material& material = material_data->material;
    diffuse_r->SetValue(material.color.diffuse.x);
    diffuse_g->SetValue(material.color.diffuse.y);
    diffuse_b->SetValue(material.color.diffuse.z);
    transparency->SetValue(material.color.diffuse.w);
    specular_r->SetValue(material.color.specular.x);
    specular_g->SetValue(material.color.specular.y);
    specular_b->SetValue(material.color.specular.z);
    reflectivity->SetValue(material.color.specular.w);
    ambient_r->SetValue(material.color.ambient.x);
    ambient_g->SetValue(material.color.ambient.y);
    ambient_b->SetValue(material.color.ambient.z);
    ambient_a->SetValue(material.color.ambient.w);
    emission_r->SetValue(material.color.emission.x);
    emission_g->SetValue(material.color.emission.y);
    emission_b->SetValue(material.color.emission.z);
    emission_a->SetValue(material.color.emission.w);
    shininess->SetValue(material.color.shininess);
    intensity->SetValue(material.color.intensity);
    bump_depth->SetValue(material.bump_depth);
    fresnel->SetValue((float_t)material.shader_info.m.fresnel_type);
    line_light->SetValue((float_t)material.shader_info.m.line_light);
    specular_quality->SetItemIndex(material.shader_info.m.specular_quality);
    aniso_direction->SetItemIndex(material.shader_info.m.aniso_direction);
    if (!material.attrib.m.double_sided)
        double_sided->SetItemIndex(0);
    else if (material.attrib.m.normal_dir_light)
        double_sided->SetItemIndex(1);
    else
        double_sided->SetItemIndex(2);
    src_blend_factor->SetItemIndex(material.attrib.m.src_blend_factor);
    dst_blend_factor->SetItemIndex(material.attrib.m.dst_blend_factor);
    zbias->SetValue((float_t)material.attrib.m.zbias);
    mipmap_bias->SetValue((float_t)material.texdata[0].attrib.m.mipmap_bias);
    aniso_filter->SetValue((float_t)material.texdata[0].attrib.m.anisotropic_filter);
}

void MaterialDw::UpdateObjectMeshes() {
    if (!object_meshes)
        return;

    object_meshes->ClearItems();
    obj* obj = GetObj();
    if (obj) {
        ResetMaterialArray();
        for (int32_t i = 0; i < obj->num_material; i++) {
            object_meshes->AddItem(obj->material_array[i].material.name);
            material_array[i] = obj->material_array[i];
        }
        object_meshes->SetItemIndex(0);
    }

    SetValue();
}

void MaterialDw::UpdateObjects() {
    data_struct* aft_data = &data_list[DATA_AFT];
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    if (!objects)
        return;

    objects->ClearItems();

    for (int32_t i = 0; i < objset_info_storage_get_obj_set_obj_num(obj_set_id); i++) {
        uint32_t obj_id = objset_info_storage_get_obj_set_obj_id(obj_set_id, i);
        objects->AddItem(objset_info_storage_get_obj_name({ obj_id, obj_set_id }));
    }
    objects->SetItemIndex(0);

    UpdateObjectMeshes();
}

void MaterialDw::AmbientACallback(dw::Slider* data) {
    MaterialDw* material_dw = (MaterialDw*)data->callback_data.v64;
    obj_material_data* material_data = material_dw->GetMaterial();
    if (material_data)
        material_data->material.color.ambient.w = data->scroll_bar->value;
}

void MaterialDw::AmbientBCallback(dw::Slider* data) {
    MaterialDw* material_dw = (MaterialDw*)data->callback_data.v64;
    obj_material_data* material_data = material_dw->GetMaterial();
    if (material_data)
        material_data->material.color.ambient.z = data->scroll_bar->value;
}

void MaterialDw::AmbientGCallback(dw::Slider* data) {
    MaterialDw* material_dw = (MaterialDw*)data->callback_data.v64;
    obj_material_data* material_data = material_dw->GetMaterial();
    if (material_data)
        material_data->material.color.ambient.y = data->scroll_bar->value;
}

void MaterialDw::AmbientRCallback(dw::Slider* data) {
    MaterialDw* material_dw = (MaterialDw*)data->callback_data.v64;
    obj_material_data* material_data = material_dw->GetMaterial();
    if (material_data)
        material_data->material.color.ambient.x = data->scroll_bar->value;
}

void MaterialDw::AnisoDirectionCallback(dw::ListBox* data) {
    MaterialDw* material_dw = (MaterialDw*)data->callback_data.v64;
    obj_material_data* material_data = material_dw->GetMaterial();
    if (material_data)
        material_data->material.shader_info.m.aniso_direction
        = (obj_material_aniso_direction)data->list->selected_item;
}

void MaterialDw::AnisoFilterCallback(dw::Slider* data) {
    MaterialDw* material_dw = (MaterialDw*)data->callback_data.v64;
    obj_material_data* material_data = material_dw->GetMaterial();
    if (material_data) {
        material_data->material.texdata[0].attrib.w = material_data->material.texdata[0].attrib.w & 0x3FFFFFFF | ((int)data->scroll_bar->value << 30);
        material_data->material.texdata[1].attrib.w = material_data->material.texdata[1].attrib.w & 0x3FFFFFFF | ((int)data->scroll_bar->value << 30);
        material_data->material.texdata[2].attrib.w = material_data->material.texdata[2].attrib.w & 0x3FFFFFFF | ((int)data->scroll_bar->value << 30);
        material_data->material.texdata[3].attrib.w = material_data->material.texdata[3].attrib.w & 0x3FFFFFFF | ((int)data->scroll_bar->value << 30);
        material_data->material.texdata[4].attrib.w = material_data->material.texdata[4].attrib.w & 0x3FFFFFFF | ((int)data->scroll_bar->value << 30);
        material_data->material.texdata[5].attrib.w = material_data->material.texdata[5].attrib.w & 0x3FFFFFFF | ((int)data->scroll_bar->value << 30);
        material_data->material.texdata[6].attrib.w = material_data->material.texdata[6].attrib.w & 0x3FFFFFFF | ((int)data->scroll_bar->value << 30);
        material_data->material.texdata[7].attrib.w = material_data->material.texdata[7].attrib.w & 0x3FFFFFFF | ((int)data->scroll_bar->value << 30);
    }
}

void MaterialDw::BumpDepthCallback(dw::Slider* data) {
    MaterialDw* material_dw = (MaterialDw*)data->callback_data.v64;
    obj_material_data* material_data = material_dw->GetMaterial();
    if (material_data)
        material_data->material.bump_depth = data->scroll_bar->value;
}

void MaterialDw::DiffuseBCallback(dw::Slider* data) {
    MaterialDw* material_dw = (MaterialDw*)data->callback_data.v64;
    obj_material_data* material_data = material_dw->GetMaterial();
    if (material_data)
        material_data->material.color.diffuse.z = data->scroll_bar->value;
}

void MaterialDw::DiffuseGCallback(dw::Slider* data) {
    MaterialDw* material_dw = (MaterialDw*)data->callback_data.v64;
    obj_material_data* material_data = material_dw->GetMaterial();
    if (material_data)
        material_data->material.color.diffuse.y = data->scroll_bar->value;
}

void MaterialDw::DiffuseRCallback(dw::Slider* data) {
    MaterialDw* material_dw = (MaterialDw*)data->callback_data.v64;
    obj_material_data* material_data = material_dw->GetMaterial();
    if (material_data)
        material_data->material.color.diffuse.x = data->scroll_bar->value;
}

void MaterialDw::DoubleSidedCallback(dw::ListBox* data) {
    MaterialDw* material_dw = (MaterialDw*)data->callback_data.v64;
    obj_material_data* material_data = material_dw->GetMaterial();
    if (material_data)
        switch (data->list->selected_item) {
        case 0:
            material_data->material.attrib.m.double_sided = 0;
            material_data->material.attrib.m.normal_dir_light = 0;
            break;
        case 1:
            material_data->material.attrib.m.double_sided = 1;
            material_data->material.attrib.m.normal_dir_light = 0;
            break;
        case 2:
            material_data->material.attrib.m.double_sided = 1;
            material_data->material.attrib.m.normal_dir_light = 1;
            break;
        }
}

void MaterialDw::DstBlendFactorCallback(dw::ListBox* data) {
    MaterialDw* material_dw = (MaterialDw*)data->callback_data.v64;
    obj_material_data* material_data = material_dw->GetMaterial();
    if (material_data)
        material_data->material.attrib.m.dst_blend_factor
            = (obj_material_blend_factor)data->list->selected_item;
}

void MaterialDw::EmissionACallback(dw::Slider* data) {
    MaterialDw* material_dw = (MaterialDw*)data->callback_data.v64;
    obj_material_data* material_data = material_dw->GetMaterial();
    if (material_data)
        material_data->material.color.emission.w = data->scroll_bar->value;
}

void MaterialDw::EmissionBCallback(dw::Slider* data) {
    MaterialDw* material_dw = (MaterialDw*)data->callback_data.v64;
    obj_material_data* material_data = material_dw->GetMaterial();
    if (material_data)
        material_data->material.color.emission.z = data->scroll_bar->value;
}

void MaterialDw::EmissionGCallback(dw::Slider* data) {
    MaterialDw* material_dw = (MaterialDw*)data->callback_data.v64;
    obj_material_data* material_data = material_dw->GetMaterial();
    if (material_data)
        material_data->material.color.emission.y = data->scroll_bar->value;
}

void MaterialDw::EmissionRCallback(dw::Slider* data) {
    MaterialDw* material_dw = (MaterialDw*)data->callback_data.v64;
    obj_material_data* material_data = material_dw->GetMaterial();
    if (material_data)
        material_data->material.color.emission.x = data->scroll_bar->value;
}

void MaterialDw::FresnelCallback(dw::Slider* data) {
    MaterialDw* material_dw = (MaterialDw*)data->callback_data.v64;
    obj_material_data* material_data = material_dw->GetMaterial();
    if (material_data)
        material_data->material.shader_info.m.fresnel_type = (int32_t)data->scroll_bar->value;
}

void MaterialDw::IntensityCallback(dw::Slider* data) {
    MaterialDw* material_dw = (MaterialDw*)data->callback_data.v64;
    obj_material_data* material_data = material_dw->GetMaterial();
    if (material_data)
        material_data->material.color.intensity = data->scroll_bar->value;
}

void MaterialDw::LineLightCallback(dw::Slider* data) {
    MaterialDw* material_dw = (MaterialDw*)data->callback_data.v64;
    obj_material_data* material_data = material_dw->GetMaterial();
    if (material_data)
        material_data->material.shader_info.m.line_light = (int32_t)data->scroll_bar->value;
}

void MaterialDw::MipmapBiasCallback(dw::Slider* data) {
    MaterialDw* material_dw = (MaterialDw*)data->callback_data.v64;
    obj_material_data* material_data = material_dw->GetMaterial();
    if (material_data) {
        material_data->material.texdata[0].attrib.m.mipmap_bias = (int32_t)data->scroll_bar->value;
        material_data->material.texdata[1].attrib.m.mipmap_bias = (int32_t)data->scroll_bar->value;
        material_data->material.texdata[2].attrib.m.mipmap_bias = (int32_t)data->scroll_bar->value;
        material_data->material.texdata[3].attrib.m.mipmap_bias = (int32_t)data->scroll_bar->value;
        material_data->material.texdata[4].attrib.m.mipmap_bias = (int32_t)data->scroll_bar->value;
        material_data->material.texdata[5].attrib.m.mipmap_bias = (int32_t)data->scroll_bar->value;
        material_data->material.texdata[6].attrib.m.mipmap_bias = (int32_t)data->scroll_bar->value;
        material_data->material.texdata[7].attrib.m.mipmap_bias = (int32_t)data->scroll_bar->value;
    }
}

void MaterialDw::ObjectCallback(dw::ListBox* data) {
    MaterialDw* material_dw = (MaterialDw*)data->callback_data.i64;
    material_dw->obj_index = (int32_t)data->list->selected_item;
    material_dw->UpdateObjectMeshes();
}

void MaterialDw::ObjectMeshCallback(dw::ListBox* data) {
    MaterialDw* material_dw = (MaterialDw*)data->callback_data.v64;
    material_dw->material_index = (int32_t)data->list->selected_item;
    material_dw->SetValue();
}

void MaterialDw::ObjectSetCallback(dw::ListBox* data) {
    data_struct* aft_data = &data_list[DATA_AFT];
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    MaterialDw* material_dw = (MaterialDw*)data->callback_data.v64;
    std::string item = data->GetItemStr(data->list->selected_item);
    material_dw->obj_set_id = aft_obj_db->get_object_set_id(item.c_str());
    material_dw->UpdateObjects();
}

void MaterialDw::ReflectivityCallback(dw::Slider* data) {
    MaterialDw* material_dw = (MaterialDw*)data->callback_data.v64;
    obj_material_data* material_data = material_dw->GetMaterial();
    if (material_data)
        material_data->material.color.specular.w = data->scroll_bar->value;
}

void MaterialDw::RefreshCallback(dw::Widget* data) {
    data_struct* aft_data = &data_list[DATA_AFT];
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    dw::ListBox* object_sets = material_dw->object_sets;
    if (!object_sets)
        return;

    object_sets->ClearItems();
    uint32_t obj_set_count = aft_obj_db->get_object_set_count();
    for (uint32_t i = obj_set_count, j = 0; i; i--, j++) {
        uint32_t obj_set_id = aft_obj_db->get_object_set_id(j);
        if (objset_info_storage_get_obj_set_loaded(obj_set_id))
            object_sets->AddItem(aft_obj_db->get_object_set_name(obj_set_id));
    }
    object_sets->SetItemIndex(0);
    MaterialDw::ObjectSetCallback(object_sets);
    material_dw->UpdateLayout();
}

void MaterialDw::ResetCallback(dw::Widget* data) {
    material_dw->SetValue(true);

    obj_material_data* material_data = material_dw->GetMaterial();
    if (!material_data)
        return;

    obj_material& material = material_data->material;
    material.color.diffuse.x = material_dw->diffuse_r->GetValue();
    material.color.diffuse.y = material_dw->diffuse_g->GetValue();
    material.color.diffuse.z = material_dw->diffuse_b->GetValue();
    material.color.diffuse.w = material_dw->transparency->GetValue();
    material.color.specular.x = material_dw->specular_r->GetValue();
    material.color.specular.y = material_dw->specular_g->GetValue();
    material.color.specular.z = material_dw->specular_b->GetValue();
    material.color.specular.w = material_dw->reflectivity->GetValue();
    material.color.ambient.x = material_dw->ambient_r->GetValue();
    material.color.ambient.y = material_dw->ambient_g->GetValue();
    material.color.ambient.z = material_dw->ambient_b->GetValue();
    material.color.ambient.w = material_dw->ambient_a->GetValue();
    material.color.emission.x = material_dw->emission_r->GetValue();
    material.color.emission.y = material_dw->emission_g->GetValue();
    material.color.emission.z = material_dw->emission_b->GetValue();
    material.color.emission.w = material_dw->emission_a->GetValue();
    material.color.shininess = material_dw->shininess->GetValue();
    material.color.intensity = material_dw->intensity->GetValue();
    material.bump_depth = material_dw->bump_depth->GetValue();
    material.shader_info.m.fresnel_type = (int32_t)material_dw->fresnel->GetValue();
    material.shader_info.m.line_light = (int32_t)material_dw->line_light->GetValue();
    material.shader_info.m.specular_quality
        = (obj_material_specular_quality)material_dw->specular_quality->list->selected_item;
    material.shader_info.m.aniso_direction
        = (obj_material_aniso_direction)material_dw->aniso_direction->list->selected_item;
    switch (material_dw->double_sided->list->selected_item) {
    case 0:
        material.attrib.m.double_sided = 0;
        break;
    case 1:
        material.attrib.m.double_sided = 1;
        break;
    }
    material.attrib.m.src_blend_factor
        = (obj_material_blend_factor)material_dw->src_blend_factor->list->selected_item;
    material.attrib.m.dst_blend_factor
        = (obj_material_blend_factor)material_dw->dst_blend_factor->list->selected_item;
    material.attrib.m.zbias = (int32_t)material_dw->zbias->GetValue();
    material.texdata[0].attrib.m.mipmap_bias = (int32_t)material_dw->mipmap_bias->GetValue();
    material.texdata[0].attrib.m.anisotropic_filter = (int32_t)material_dw->aniso_filter->GetValue();
}

void MaterialDw::ShininessCallback(dw::Slider* data) {
    MaterialDw* material_dw = (MaterialDw*)data->callback_data.v64;
    obj_material_data* material_data = material_dw->GetMaterial();
    if (material_data)
        material_data->material.color.shininess = data->scroll_bar->value;
}

void MaterialDw::SpecularBCallback(dw::Slider* data) {
    MaterialDw* material_dw = (MaterialDw*)data->callback_data.v64;
    obj_material_data* material_data = material_dw->GetMaterial();
    if (material_data)
        material_data->material.color.specular.z = data->scroll_bar->value;
}

void MaterialDw::SpecularGCallback(dw::Slider* data) {
    MaterialDw* material_dw = (MaterialDw*)data->callback_data.v64;
    obj_material_data* material_data = material_dw->GetMaterial();
    if (material_data)
        material_data->material.color.specular.y = data->scroll_bar->value;
}

void MaterialDw::SpecularQualityCallback(dw::ListBox* data) {
    MaterialDw* material_dw = (MaterialDw*)data->callback_data.v64;
    obj_material_data* material_data = material_dw->GetMaterial();
    if (material_data)
        material_data->material.shader_info.m.specular_quality
            = (obj_material_specular_quality)data->list->selected_item;
}

void MaterialDw::SpecularRCallback(dw::Slider* data) {
    MaterialDw* material_dw = (MaterialDw*)data->callback_data.v64;
    obj_material_data* material_data = material_dw->GetMaterial();
    if (material_data)
        material_data->material.color.specular.x = data->scroll_bar->value;
}

void MaterialDw::SrcBlendFactorCallback(dw::ListBox* data) {
    MaterialDw* material_dw = (MaterialDw*)data->callback_data.v64;
    obj_material_data* material_data = material_dw->GetMaterial();
    if (material_data)
        material_data->material.attrib.m.src_blend_factor
            = (obj_material_blend_factor)data->list->selected_item;
}

void MaterialDw::TransparencyCallback(dw::Slider* data) {
    MaterialDw* material_dw = (MaterialDw*)data->callback_data.v64;
    obj_material_data* material_data = material_dw->GetMaterial();
    if (material_data)
        material_data->material.color.diffuse.w = data->scroll_bar->value;
}

void MaterialDw::ZBiasCallback(dw::Slider* data) {
    MaterialDw* material_dw = (MaterialDw*)data->callback_data.v64;
    obj_material_data* material_data = material_dw->GetMaterial();
    if (material_data)
        material_data->material.attrib.m.zbias = (int32_t)data->scroll_bar->value;
}
