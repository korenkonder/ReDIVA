/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "auth_3d.hpp"
#include "../../KKdLib/database/item_table.hpp"
#include "../../CRE/rob/rob.hpp"
#include "../../CRE/auth_3d.hpp"
#include "../imgui_helper.hpp"
#include "../task_window.hpp"

class DataViewAuth3D : public app::TaskWindow {
public:
    bool exit;

    DataViewAuth3D();
    virtual ~DataViewAuth3D() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void window() override;
};

static const char* auth_3d_fog_name[] = {
    "Depth",
    "Height",
    "Bump",
};

static const char* auth_3d_light_name[] = {
    "Chara",
    "Stage",
    "Sun",
    "Reflect",
    "Shadow",
    "Chara Color",
    "Tone Curve",
    "Projection",
};

DataViewAuth3D data_view_auth_3d;

static void data_view_auth_3d_window_auth_3d_fcurve(
    const auth_3d_detail::Fcurve* fcv, const char* format, bool offset);
static void data_view_auth_3d_window_auth_3d_fcurve3f(
    const auth_3d_detail::Fcurve3f* fcv3f, const char* format);
static void data_view_auth_3d_window_auth_3d_fcurve_color4f(
    const auth_3d_detail::FcurveColor4f* fcv_col4f, const char* format);
static void data_view_auth_3d_window_auth_3d_model_transform(
    const auth_3d_detail::ModelTransform* mt);

static void data_view_auth_3d_window_auth_3d_ambient(auth_3d_detail::Ambient* a);
static void data_view_auth_3d_window_auth_3d_auth_2d(auth_3d_detail::Auth2d* a2);
static void data_view_auth_3d_window_auth_3d_camera_auxiliary(auth_3d_detail::CameraAuxiliary* ca);
static void data_view_auth_3d_window_auth_3d_camera_root(auth_3d_detail::CameraRoot* cr, size_t index);
static void data_view_auth_3d_window_auth_3d_chara(auth_3d_detail::Chara* c);
static void data_view_auth_3d_window_auth_3d_curve(auth_3d_detail::Curve* c);
static void data_view_auth_3d_window_auth_3d_curve_with_offset(
    auth_3d_detail::CurveWithOffset* oc, const char* format);
static void data_view_auth_3d_window_auth_3d_dof(auth_3d_detail::Dof* d);
static void data_view_auth_3d_window_auth_3d_fog(auth_3d_detail::Fog* f);
static void data_view_auth_3d_window_auth_3d_light(auth_3d_detail::Light* l);
static void data_view_auth_3d_window_auth_3d_m_object_hrc(auth_3d_detail::MultiHierarchyObject* moh);
static void data_view_auth_3d_window_auth_3d_material_list(auth_3d_detail::MaterialList* ml);
static void data_view_auth_3d_window_auth_3d_motion(auth_3d_detail::Motion* m);
static void data_view_auth_3d_window_auth_3d_object(auth_3d_detail::Object* o);
static void data_view_auth_3d_window_auth_3d_object_hrc(auth_3d_detail::HierarchyObject* ho);
static void data_view_auth_3d_window_auth_3d_object_instance(auth_3d_detail::ObjectInstance* oi);
static void data_view_auth_3d_window_auth_3d_motion_transform(auth_3d_detail::MotionTransform* mt);
static void data_view_auth_3d_window_auth_3d_object_node(auth_3d_detail::TransformNode* tn);
static void data_view_auth_3d_window_auth_3d_texture(auth_3d_detail::Texture* t);
static void data_view_auth_3d_window_auth_3d_texture_transform(auth_3d_detail::TextureTransform* tt);
static void data_view_auth_3d_window_auth_3d_point(auth_3d_detail::Point* p);
static void data_view_auth_3d_window_auth_3d_post_process(auth_3d_detail::PostProcess* pp);

void data_view_auth_3d_init() {
    data_view_auth_3d.open("DATA_VIEW_AUTH_3D", app::TASK_PRIO_LOW);
}

DataViewAuth3D::DataViewAuth3D() : exit() {

}

DataViewAuth3D::~DataViewAuth3D() {

}

bool DataViewAuth3D::init() {
    exit = false;
    return true;
}

bool DataViewAuth3D::ctrl() {
    return exit;
}

bool DataViewAuth3D::dest() {
    return true;
}

void DataViewAuth3D::window() {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    extern int32_t height;
    extern int32_t width;

    float_t w = min_def((float_t)width, 480.0f);
    float_t h = min_def((float_t)height, 540.0f);

    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Appearing);

    reset_focus();
    bool open = true;
    if (!ImGui::Begin("Auth 3D##Data Viewer", &open, 0)) {
        ImGui::End();
        return;
    }
    else if (!open) {
        exit = true;
        ImGui::End();
        return;
    }

    ImGuiTreeNodeFlags tree_node_base_flags = 0;
    tree_node_base_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_base_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_base_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGuiTreeNodeFlags tree_node_flags;

    char buf[0x200];
    for (size_t i = 0; i < AUTH_3D_DATA_COUNT; i++) {
        auth_3d_detail::Scene* scene = &auth_3d_detail::g_manager->scene_buffer[i];
        if (scene->my_handle == -1)
            continue;

        size_t str_len = min_def(sizeof(buf) - 1, scene->file_name.size());
        memcpy(buf, scene->file_name.c_str(), str_len);
        buf[str_len] = 0;

        if (str_len >= 5 && !memcmp(&buf[str_len - 5], ".a3da", 6))
            buf[str_len - 5] = 0;

        tree_node_flags = tree_node_base_flags;

        bool visibility = scene->M_is_enabled && scene->M_is_visible;
        if (!visibility)
            ImGui::PushStyleColor(ImGuiCol_Text, 0xFF888888);
        ImGui::PushID(scene->my_handle);
        if (!ImGui::TreeNodeEx(scene, tree_node_flags, "ID: %5d; Frame: %5d; File: %s",
            scene->my_handle & 0x7FFF, (int32_t)prj::roundf(scene->M_frame), buf)) {
            ImGui::PopID();
            if (!visibility)
                ImGui::PopStyleColor();
            continue;
        }

        tree_node_flags = tree_node_base_flags;
        tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;

        int32_t rob_id = scene->get_assign_rob_id();
        if (rob_id != ROB_ID_NULL)
            ImGui::Text("Chara: %d", rob_id + 1);
        else
            ImGui::Text("Chara: None");

        if (scene->M_cn_src != scene->M_cn_dst)
            ImGui::Text("Chara Replace: %s/%s",
                get_char_id_str((CHARA_NUM)scene->M_cn_src),
                get_char_id_str((CHARA_NUM)scene->M_cn_dst));

        // MGF
        if (scene->ambient_list.size() > 0
            && ImGui::TreeNodeEx("Ambient", tree_node_flags)) {
            for (auth_3d_detail::Ambient& l : scene->ambient_list)
                data_view_auth_3d_window_auth_3d_ambient(&l);
            ImGui::TreePop();
        }

        if (scene->auth_2d_list.size() > 0
            && ImGui::TreeNodeEx("Auth 2D", tree_node_flags)) {
            for (auth_3d_detail::Auth2d& l : scene->auth_2d_list)
                data_view_auth_3d_window_auth_3d_auth_2d(&l);
            ImGui::TreePop();
        }

        data_view_auth_3d_window_auth_3d_camera_auxiliary(&scene->camera_auxiliary);

        if (scene->camera_root_list.size() > 1
            && ImGui::TreeNodeEx("Camera Root", tree_node_flags)) {
            for (auth_3d_detail::CameraRoot& l : scene->camera_root_list)
                data_view_auth_3d_window_auth_3d_camera_root(&l, &l - scene->camera_root_list.data());
            ImGui::TreePop();
        }
        else if (scene->camera_root_list.size() == 1)
            data_view_auth_3d_window_auth_3d_camera_root(&scene->camera_root_list[0], -1);

        if (scene->chara_list.size() > 0
            && ImGui::TreeNodeEx("Chara", tree_node_flags)) {
            for (auth_3d_detail::Chara& l : scene->chara_list)
                data_view_auth_3d_window_auth_3d_chara(&l);
            ImGui::TreePop();
        }

        if (scene->curve_list.size() > 0
            && ImGui::TreeNodeEx("Curve", tree_node_flags)) {
            for (auth_3d_detail::Curve& l : scene->curve_list)
                data_view_auth_3d_window_auth_3d_curve(&l);
            ImGui::TreePop();
        }

        data_view_auth_3d_window_auth_3d_dof(&scene->dof);

        if (scene->fog_list.size() > 0
            && ImGui::TreeNodeEx("Fog", tree_node_flags)) {
            for (auth_3d_detail::Fog& l : scene->fog_list)
                data_view_auth_3d_window_auth_3d_fog(&l);
            ImGui::TreePop();
        }

        if (scene->light_list.size() > 0
            && ImGui::TreeNodeEx("Light", tree_node_flags)) {
            for (auth_3d_detail::Light& l : scene->light_list)
                data_view_auth_3d_window_auth_3d_light(&l);
            ImGui::TreePop();
        }

        if (scene->m_object_hrc_list.size() > 0
            && ImGui::TreeNodeEx("M Object HRC", tree_node_flags)) {
            for (auth_3d_detail::MultiHierarchyObject*& l : scene->m_object_hrc_list)
                data_view_auth_3d_window_auth_3d_m_object_hrc(l);
            ImGui::TreePop();
        }

        // X/XHD
        if (scene->material_list.size() > 0
            && ImGui::TreeNodeEx("Material List", tree_node_flags)) {
            for (auth_3d_detail::MaterialList& l : scene->material_list)
                data_view_auth_3d_window_auth_3d_material_list(&l);
            ImGui::TreePop();
        }

        if (scene->motion_list.size() > 0
            && ImGui::TreeNodeEx("Motion", tree_node_flags)) {
            for (auth_3d_detail::Motion& l : scene->motion_list)
                data_view_auth_3d_window_auth_3d_motion(&l);
            ImGui::TreePop();
        }

        if (scene->object_list.size() > 0
            && ImGui::TreeNodeEx("Object", tree_node_flags)) {
            for (auth_3d_detail::Object*& l : scene->object_list)
                data_view_auth_3d_window_auth_3d_object(l);
            ImGui::TreePop();
        }

        if (scene->object_hrc_list.size() > 0
            && ImGui::TreeNodeEx("Object HRC", tree_node_flags)) {
            for (auth_3d_detail::HierarchyObject*& l : scene->object_hrc_list)
                data_view_auth_3d_window_auth_3d_object_hrc(l);
            ImGui::TreePop();
        }

        if (scene->point_list.size() > 0
            && ImGui::TreeNodeEx("Point", tree_node_flags)) {
            for (auth_3d_detail::Point& l : scene->point_list)
                data_view_auth_3d_window_auth_3d_point(&l);
            ImGui::TreePop();
        }

        data_view_auth_3d_window_auth_3d_post_process(&scene->post_process);

        ImGui::TreePop();
        ImGui::PopID();
        if (!visibility)
            ImGui::PopStyleColor();
    }

    set_focus(ImGui::IsWindowFocused());
    ImGui::End();
}

static void data_view_auth_3d_window_auth_3d_fcurve(
    const auth_3d_detail::Fcurve* fcv, const char* format, bool offset) {
    if (!fcv)
        return;

    if (offset) {
        ImGui::Text("  ");
        if (format) {
            ImGui::SameLine();
            ImGui::Text(format);
        }
        ImGui::SameLine();
    }
    else if (format) {
        ImGui::Text(format);
        ImGui::SameLine();
    }

    switch (fcv->m_type) {
    case auth_3d_detail::FC_TYPE_STATIC_0:
    default:
        ImGui::Text("V: %#.6g", 0.0f);
        break;
    case auth_3d_detail::FC_TYPE_STATIC_DATA:
        ImGui::Text("V: %#.6g", fcv->m_static_value);
        break;
    case auth_3d_detail::FC_TYPE_LINEAR:
    case auth_3d_detail::FC_TYPE_HERMITE:
    case auth_3d_detail::FC_TYPE_NO_INTERPOLATION:
        ImGui::Text("F: %5g; V: %#.6g", fcv->m_frame, fcv->m_value);
        break;
    }
}

static void data_view_auth_3d_window_auth_3d_fcurve3f(
    const auth_3d_detail::Fcurve3f* fcv3f, const char* format) {
    if (!fcv3f || !format)
        return;

    ImGui::Text(format);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, 0xFF404040);
    data_view_auth_3d_window_auth_3d_fcurve(&fcv3f->x, "X:", true);
    data_view_auth_3d_window_auth_3d_fcurve(&fcv3f->y, "Y:", true);
    data_view_auth_3d_window_auth_3d_fcurve(&fcv3f->z, "Z:", true);
    ImGui::PopStyleColor();
}

static void data_view_auth_3d_window_auth_3d_fcurve_color4f(
    const auth_3d_detail::FcurveColor4f* fcv_col4f, const char* format) {
    if (!fcv_col4f || !(fcv_col4f->has_r || fcv_col4f->has_g || fcv_col4f->has_b || fcv_col4f->has_a) || !format)
        return;

    ImGui::Text(format);
    if (fcv_col4f->has_r)
        data_view_auth_3d_window_auth_3d_fcurve(&fcv_col4f->r, "R:", true);
    if (fcv_col4f->has_g)
        data_view_auth_3d_window_auth_3d_fcurve(&fcv_col4f->g, "G:", true);
    if (fcv_col4f->has_b)
        data_view_auth_3d_window_auth_3d_fcurve(&fcv_col4f->b, "B:", true);
    if (fcv_col4f->has_a)
        data_view_auth_3d_window_auth_3d_fcurve(&fcv_col4f->a, "A:", true);
}

static void data_view_auth_3d_window_auth_3d_model_transform(
    const auth_3d_detail::ModelTransform* mt) {
    if (!mt)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!ImGui::TreeNodeEx(mt, tree_node_flags, "Model Transform"))
        return;

    data_view_auth_3d_window_auth_3d_fcurve3f(&mt->fcurve_translation, "Translation");
    data_view_auth_3d_window_auth_3d_fcurve3f(&mt->fcurve_rotation, "Rotation");
    data_view_auth_3d_window_auth_3d_fcurve3f(&mt->fcurve_scale, "Scale");

    ImGui::Text("Visibility");
    data_view_auth_3d_window_auth_3d_fcurve(&mt->fcurve_visibility, 0, true);
    ImGui::TreePop();
}

// MGF
static void data_view_auth_3d_window_auth_3d_ambient(auth_3d_detail::Ambient* a) {
    if (!a || !a->flag)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!ImGui::TreeNodeEx(a, tree_node_flags, a->name.c_str()))
        return;

    if (a->has_light_diffuse())
        data_view_auth_3d_window_auth_3d_fcurve_color4f(&a->light_diffuse.fcurve_color, "Light Diffuse");
    if (a->has_rim_light_diffuse())
        data_view_auth_3d_window_auth_3d_fcurve_color4f(&a->rim_light_diffuse.fcurve_color, "Rim Light Diffuse");
    ImGui::TreePop();
}

static void data_view_auth_3d_window_auth_3d_auth_2d(auth_3d_detail::Auth2d* a2) {
    if (!a2)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!ImGui::TreeNodeEx(a2, tree_node_flags, a2->name.c_str()))
        return;

    ImGui::TreePop();
}

static void data_view_auth_3d_window_auth_3d_camera_auxiliary(auth_3d_detail::CameraAuxiliary* ca) {
    if (!ca || !ca->flag)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!ImGui::TreeNodeEx(ca, tree_node_flags, "Camera Auxiliary"))
        return;

    if (ca->has_exposure()) {
        ImGui::Text("Exposure");
        data_view_auth_3d_window_auth_3d_fcurve(&ca->fcurve_exposure, 0, true);
    }

    // F/F2?X/XHD
    if (ca->has_exposure_rate()) {
        ImGui::Text("Exposure Rate");
        data_view_auth_3d_window_auth_3d_fcurve(&ca->fcurve_exposure_rate, 0, true);
    }

    if (ca->has_gamma()) {
        ImGui::Text("Gamma");
        data_view_auth_3d_window_auth_3d_fcurve(&ca->fcurve_gamma, 0, true);
    }

    // F/F2?X/XHD
    if (ca->has_gamma_rate()) {
        ImGui::Text("Gamma Rate");
        data_view_auth_3d_window_auth_3d_fcurve(&ca->fcurve_gamma_rate, 0, true);
    }

    if (ca->has_saturate()) {
        ImGui::Text("Saturate");
        data_view_auth_3d_window_auth_3d_fcurve(&ca->fcurve_saturate, 0, true);
    }

    if (ca->has_auto_exposure()) {
        ImGui::Text("Auto Exposure");
        data_view_auth_3d_window_auth_3d_fcurve(&ca->fcurve_auto_exposure, 0, true);
    }
    ImGui::TreePop();
}

static void data_view_auth_3d_window_auth_3d_camera_root(auth_3d_detail::CameraRoot* cr, size_t index) {
    if (!cr)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    if (index == -1)
        tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!ImGui::TreeNodeEx(cr, tree_node_flags,
        index == -1 ? "Camera Root" : "%d", (int32_t)index))
        return;

    data_view_auth_3d_window_auth_3d_model_transform(&cr->model_transform);

    auth_3d_detail::CameraInterest* intr = &cr->interest;
    data_view_auth_3d_window_auth_3d_fcurve3f(&intr->model_transform.fcurve_translation, "Interest");

    auth_3d_detail::CameraViewPoint* vp = &cr->view_point;
    data_view_auth_3d_window_auth_3d_fcurve3f(&vp->model_transform.fcurve_translation, "View Point");

    ImGui::Text("Roll");
    data_view_auth_3d_window_auth_3d_fcurve(&vp->roll, 0, true);

    if (vp->has_fov) {
        ImGui::Text(vp->fov_is_horizontal ? "FOV Horizontal" : "FOV");
        data_view_auth_3d_window_auth_3d_fcurve(&vp->fov, 0, true);

        ImGui::Text("Aspect");
        ImGui::Text("   %#.6g", vp->aspect);
    }
    else {
        ImGui::Text("Focal Length");
        data_view_auth_3d_window_auth_3d_fcurve(&vp->focal_length, 0, true);

        ImGui::Text("Camera Aperture Width");
        ImGui::Text("   %#.6g", vp->camera_aperture_w);

        ImGui::Text("Camera Aperture Height");
        ImGui::Text("   %#.6g", vp->camera_aperture_h);
    }
    ImGui::TreePop();
}

static void data_view_auth_3d_window_auth_3d_chara(auth_3d_detail::Chara* c) {
    if (!c)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!ImGui::TreeNodeEx(c, tree_node_flags, c->name.c_str()))
        return;

    data_view_auth_3d_window_auth_3d_model_transform(&c->model_transform);
    ImGui::TreePop();
}

static void data_view_auth_3d_window_auth_3d_curve(auth_3d_detail::Curve* c) {
    if (!c)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!ImGui::TreeNodeEx(c, tree_node_flags, c->name.c_str()))
        return;

    data_view_auth_3d_window_auth_3d_fcurve(&c->curve, 0, false);
    ImGui::TreePop();
}

static void data_view_auth_3d_window_auth_3d_curve_with_offset(
    auth_3d_detail::CurveWithOffset* oc, const char* format) {
    if (!oc || !oc->fcurve_ptr || !format)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!ImGui::TreeNodeEx(oc, tree_node_flags, format))
        return;

    data_view_auth_3d_window_auth_3d_curve(oc->fcurve_ptr);
    ImGui::TreePop();
}

static void data_view_auth_3d_window_auth_3d_dof(auth_3d_detail::Dof* d) {
    if (!d || !d->has_dof)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    bool enable = fabsf(d->model_transform.rotation.z) > 0.000001f;
    if (!enable)
        ImGui::PushStyleColor(ImGuiCol_Text, 0xFF888888);
    if (!ImGui::TreeNodeEx(d, tree_node_flags, "DOF")) {
        if (!enable)
            ImGui::PopStyleColor();
        return;
    }

    auth_3d_detail::ModelTransform* mt = &d->model_transform;

    data_view_auth_3d_window_auth_3d_fcurve3f(&mt->fcurve_translation, "Position");

    ImGui::Text("Focus Range");
    data_view_auth_3d_window_auth_3d_fcurve(&mt->fcurve_scale.x, 0, true);

    ImGui::Text("??? (Scale Y)");
    data_view_auth_3d_window_auth_3d_fcurve(&mt->fcurve_scale.y, 0, true);

    ImGui::Text("??? (Scale Z)");
    data_view_auth_3d_window_auth_3d_fcurve(&mt->fcurve_scale.z, 0, true);

    ImGui::Text("Fuzzing Range");
    data_view_auth_3d_window_auth_3d_fcurve(&mt->fcurve_rotation.x, 0, true);

    ImGui::Text("Ratio");
    data_view_auth_3d_window_auth_3d_fcurve(&mt->fcurve_rotation.y, 0, true);

    ImGui::Text("Enable");
    data_view_auth_3d_window_auth_3d_fcurve(&mt->fcurve_rotation.z, 0, true);
    ImGui::TreePop();

    if (!enable)
        ImGui::PopStyleColor();
}

static void data_view_auth_3d_window_auth_3d_fog(auth_3d_detail::Fog* f) {
    if (!f || !f->flag)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!ImGui::TreeNodeEx(f, tree_node_flags, auth_3d_fog_name[f->id]))
        return;

    if (f->flag & auth_3d_detail::Fog::FLAG_DENSITY) {
        ImGui::Text("Density");
        data_view_auth_3d_window_auth_3d_fcurve(&f->fcurve_density, 0, true);
    }

    if (f->flag & auth_3d_detail::Fog::FLAG_START) {
        ImGui::Text("Start");
        data_view_auth_3d_window_auth_3d_fcurve(&f->fcurve_start, 0, true);
    }

    if (f->flag & auth_3d_detail::Fog::FLAG_END) {
        ImGui::Text("End");
        data_view_auth_3d_window_auth_3d_fcurve(&f->fcurve_end, 0, true);
    }

    if (f->flag & auth_3d_detail::Fog::FLAG_COLOR)
        data_view_auth_3d_window_auth_3d_fcurve_color4f(&f->color.fcurve_color, "Color");
    ImGui::TreePop();
}

static void data_view_auth_3d_window_auth_3d_light(auth_3d_detail::Light* l) {
    if (!l || !l->flag)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!ImGui::TreeNodeEx(l, tree_node_flags, auth_3d_light_name[l->id]))
        return;

    data_view_auth_3d_window_auth_3d_fcurve3f(&l->position.fcurve_translation, "Position");
    data_view_auth_3d_window_auth_3d_fcurve3f(&l->spot_direction.fcurve_translation, "Spot Direction");

    if (l->flag & auth_3d_detail::Light::FLAG_DIFFUSE)
        data_view_auth_3d_window_auth_3d_fcurve_color4f(&l->diffuse.fcurve_color, "Diffuse");

    if (l->flag & auth_3d_detail::Light::FLAG_AMBIENT)
        data_view_auth_3d_window_auth_3d_fcurve_color4f(&l->ambient.fcurve_color, "Ambient");

    if (l->flag & auth_3d_detail::Light::FLAG_SPECULAR)
        data_view_auth_3d_window_auth_3d_fcurve_color4f(&l->specular.fcurve_color, "Specular");

    if (l->flag & auth_3d_detail::Light::FLAG_TONE_CURVE)
        data_view_auth_3d_window_auth_3d_fcurve_color4f(&l->tone_curve.fcurve_color, "Tone Curve");

    // XHD
    if (l->flag & auth_3d_detail::Light::FLAG_CONSTANT) {
        ImGui::Text("Constant");
        data_view_auth_3d_window_auth_3d_fcurve(&l->fcurve_constant, 0, true);
    }

    // XHD
    if (l->flag & auth_3d_detail::Light::FLAG_INTENSITY) {
        ImGui::Text("Intensity");
        data_view_auth_3d_window_auth_3d_fcurve(&l->fcurve_intensity, 0, true);
    }

    // XHD
    if (l->flag & auth_3d_detail::Light::FLAG_FAR) {
        ImGui::Text("Far");
        data_view_auth_3d_window_auth_3d_fcurve(&l->fcurve_far, 0, true);
    }

    // XHD
    if (l->flag & auth_3d_detail::Light::FLAG_LINEAR) {
        ImGui::Text("Linear");
        data_view_auth_3d_window_auth_3d_fcurve(&l->fcurve_linear, 0, true);
    }

    // XHD
    if (l->flag & auth_3d_detail::Light::FLAG_QUADRATIC) {
        ImGui::Text("Quadratic");
        data_view_auth_3d_window_auth_3d_fcurve(&l->fcurve_quadratic, 0, true);
    }

    // XHD
    if (l->flag & auth_3d_detail::Light::FLAG_DROP_OFF) {
        ImGui::Text("Drop Off");
        data_view_auth_3d_window_auth_3d_fcurve(&l->fcurve_drop_off, 0, true);
    }

    // XHD
    if (l->flag & auth_3d_detail::Light::FLAG_CONE_ANGLE) {
        ImGui::Text("Cone Angle");
        data_view_auth_3d_window_auth_3d_fcurve(&l->fcurve_cone_angle, 0, true);
    }

    ImGui::TreePop();
}

static void data_view_auth_3d_window_auth_3d_m_object_hrc(auth_3d_detail::MultiHierarchyObject* moh) {
    if (!moh)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    bool visibility = moh->motion_transform.visibility;
    if (!visibility)
        ImGui::PushStyleColor(ImGuiCol_Text, 0xFF888888);

    if (!ImGui::TreeNodeEx(moh, tree_node_flags, moh->name.c_str())) {
        if (!visibility)
            ImGui::PopStyleColor();
        return;
    }

    data_view_auth_3d_window_auth_3d_motion_transform(&moh->motion_transform);

    if (moh->instance_list.size() > 0
        && ImGui::TreeNodeEx("Instance", tree_node_flags)) {
        for (auth_3d_detail::ObjectInstance& i : moh->instance_list)
            data_view_auth_3d_window_auth_3d_object_instance(&i);
        ImGui::TreePop();
    }

    if (moh->node_list.size() > 0
        && ImGui::TreeNodeEx("Node", tree_node_flags)) {
        for (auth_3d_detail::TransformNode& i : moh->node_list)
            data_view_auth_3d_window_auth_3d_object_node(&i);
        ImGui::TreePop();
    }
    ImGui::TreePop();

    if (!visibility)
        ImGui::PopStyleColor();
}

// X/XHD
static void data_view_auth_3d_window_auth_3d_material_list(auth_3d_detail::MaterialList* ml) {
    if (!ml || !ml->flag)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!ImGui::TreeNodeEx(ml, tree_node_flags, ml->name.c_str()))
        return;

    if (ml->flag & auth_3d_detail::MaterialList::FLAG_EMISSION)
        data_view_auth_3d_window_auth_3d_fcurve_color4f(&ml->emission.fcurve_color, "Emission");

    if (ml->flag & auth_3d_detail::MaterialList::FLAG_BLEND_COLOR)
        data_view_auth_3d_window_auth_3d_fcurve_color4f(&ml->blend_color.fcurve_color, "Blend Color");

    if (ml->flag & auth_3d_detail::MaterialList::FLAG_GLOW_INTENSITY) {
        ImGui::Text("Glow Intensity");
        data_view_auth_3d_window_auth_3d_fcurve(&ml->fcurve_glow_intensity, 0, false);
    }

    ImGui::TreePop();
}

static void data_view_auth_3d_window_auth_3d_motion(auth_3d_detail::Motion* m) {
    if (!m)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!ImGui::TreeNodeEx(m, tree_node_flags, m->name.c_str()))
        return;

    ImGui::TreePop();
}

static void data_view_auth_3d_window_auth_3d_object(auth_3d_detail::Object* o) {
    if (!o)
        return;

    ImGuiTreeNodeFlags tree_node_base_flags = 0;
    tree_node_base_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_base_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_base_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGuiTreeNodeFlags tree_node_flags;

    const char* s = o->name.c_str();
    const char* name = s;
    while (s = strchr(name = s, '|'))
        s++;

    bool visibility = o->model_transform.visibility;
    if (!visibility)
        ImGui::PushStyleColor(ImGuiCol_Text, 0xFF888888);

    tree_node_flags = tree_node_base_flags;

    if (!ImGui::TreeNodeEx(o, tree_node_flags, name)) {
        if (!visibility)
            ImGui::PopStyleColor();
        return;
    }

    data_view_auth_3d_window_auth_3d_model_transform(&o->model_transform);

    data_view_auth_3d_window_auth_3d_curve_with_offset(&o->obj_morph, "Morph");
    data_view_auth_3d_window_auth_3d_curve_with_offset(&o->obj_pat, "Pattern");

    tree_node_flags = tree_node_base_flags;

    if (o->texture_list.size() > 0
        && ImGui::TreeNodeEx("Texture", tree_node_flags)) {
        for (auth_3d_detail::Texture& i : o->texture_list)
            data_view_auth_3d_window_auth_3d_texture(&i);
        ImGui::TreePop();
    }

    tree_node_flags = tree_node_base_flags;

    if (o->texture_transform_list.size() > 0
        && ImGui::TreeNodeEx("Texture Transform", tree_node_flags)) {
        for (auth_3d_detail::TextureTransform& i : o->texture_transform_list)
            data_view_auth_3d_window_auth_3d_texture_transform(&i);
        ImGui::TreePop();
    }

    tree_node_flags = tree_node_base_flags;
    tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;

    if (o->child_object_list.size() > 0
        && ImGui::TreeNodeEx("Child Object List", tree_node_flags)) {
        for (auth_3d_detail::Object*& i : o->child_object_list)
            data_view_auth_3d_window_auth_3d_object(i);
        ImGui::TreePop();
    }

    tree_node_flags = tree_node_base_flags;
    tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;

    if (o->child_object_hrc_list.size() > 0
        && ImGui::TreeNodeEx("Child Object HRC List", tree_node_flags)) {
        for (auth_3d_detail::HierarchyObject*& i : o->child_object_hrc_list)
            data_view_auth_3d_window_auth_3d_object_hrc(i);
        ImGui::TreePop();
    }
    ImGui::TreePop();

    if (!visibility)
        ImGui::PopStyleColor();
}

static void data_view_auth_3d_window_auth_3d_object_hrc(auth_3d_detail::HierarchyObject* ho) {
    if (!ho)
        return;

    ImGuiTreeNodeFlags tree_node_base_flags = 0;
    tree_node_base_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_base_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_base_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGuiTreeNodeFlags tree_node_flags;

    const char* s = ho->name.c_str();
    const char* name = s;
    while (s = strchr(name = s, '|'))
        s++;

    bool visibility = ho->node_list[0].motion_transform.visibility;
    if (!visibility)
        ImGui::PushStyleColor(ImGuiCol_Text, 0xFF888888);

    tree_node_flags = tree_node_base_flags;

    if (!ImGui::TreeNodeEx(ho, tree_node_flags, name)) {
        if (!visibility)
            ImGui::PopStyleColor();
        return;
    }

    tree_node_flags = tree_node_base_flags;

    if (ho->node_list.size() > 0
        && ImGui::TreeNodeEx("Node", tree_node_flags)) {
        for (auth_3d_detail::TransformNode& i : ho->node_list)
            data_view_auth_3d_window_auth_3d_object_node(&i);
        ImGui::TreePop();
    }

    tree_node_flags = tree_node_base_flags;
    tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;

    if (ho->child_object_list.size() > 0
        && ImGui::TreeNodeEx("Child Object List", tree_node_flags)) {
        for (auth_3d_detail::Object*& i : ho->child_object_list)
            data_view_auth_3d_window_auth_3d_object(i);
        ImGui::TreePop();
    }

    tree_node_flags = tree_node_base_flags;
    tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;

    if (ho->child_object_hrc_list.size() > 0
        && ImGui::TreeNodeEx("Child Object HRC List", tree_node_flags)) {
        for (auth_3d_detail::HierarchyObject*& i : ho->child_object_hrc_list)
            data_view_auth_3d_window_auth_3d_object_hrc(i);
        ImGui::TreePop();
    }
    ImGui::TreePop();

    if (!visibility)
        ImGui::PopStyleColor();
}

static void data_view_auth_3d_window_auth_3d_object_instance(auth_3d_detail::ObjectInstance* oi) {
    if (!oi)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    bool visibility = oi->motion_transform.visibility;
    if (!visibility)
        ImGui::PushStyleColor(ImGuiCol_Text, 0xFF888888);

    if (!ImGui::TreeNodeEx(oi, tree_node_flags, oi->name.c_str())) {
        if (!visibility)
            ImGui::PopStyleColor();
        return;
    }

    data_view_auth_3d_window_auth_3d_motion_transform(&oi->motion_transform);

    if (oi->shadow)
        ImGui::Text("Shadow: True");

    ImGui::Text("UID Name: %s", oi->uid_name.c_str());
    ImGui::TreePop();

    if (!visibility)
        ImGui::PopStyleColor();
}

static void data_view_auth_3d_window_auth_3d_motion_transform(auth_3d_detail::MotionTransform* mt) {
    if (!mt)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!ImGui::TreeNodeEx(mt, tree_node_flags, "Model Transform"))
        return;

    ImGui::Text("Frame: %5f", mt->frame);

    data_view_auth_3d_window_auth_3d_fcurve3f(&mt->fcurve_translation, "Translation");
    data_view_auth_3d_window_auth_3d_fcurve3f(&mt->fcurve_rotation, "Rotation");
    data_view_auth_3d_window_auth_3d_fcurve3f(&mt->fcurve_scale, "Scale");

    ImGui::Text("Visibility");
    data_view_auth_3d_window_auth_3d_fcurve(&mt->fcurve_visibility, 0, true);
    ImGui::TreePop();
}

static void data_view_auth_3d_window_auth_3d_object_node(auth_3d_detail::TransformNode* tn) {
    if (!tn)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!ImGui::TreeNodeEx(tn, tree_node_flags, tn->name.c_str()))
        return;

    data_view_auth_3d_window_auth_3d_motion_transform(&tn->motion_transform);

    if (tn->flag & auth_3d_detail::TransformNode::FLAG_JOINT_ORIENT) {
        ImGui::Text("Joint Orient ");
        ImGui::Text("   X: %#.6g", tn->joint_orient.x);
        ImGui::Text("   Y: %#.6g", tn->joint_orient.y);
        ImGui::Text("   Z: %#.6g", tn->joint_orient.z);
    }

    ImGui::Text("Parent ID: %d", tn->parent_id);
    ImGui::TreePop();
}

static void data_view_auth_3d_window_auth_3d_texture(auth_3d_detail::Texture* t) {
    if (!t)
        return;

    data_view_auth_3d_window_auth_3d_curve_with_offset(&t->tex_pat, t->name.c_str());
}

static void data_view_auth_3d_window_auth_3d_texture_transform(auth_3d_detail::TextureTransform* tt) {
    if (!tt || !tt->flag)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!ImGui::TreeNodeEx(tt, tree_node_flags, tt->name.c_str()))
        return;

    if (tt->has_coverage_u())
        data_view_auth_3d_window_auth_3d_fcurve(&tt->fcurve_coverage_u, "Coverage U:", false);
    if (tt->has_coverage_v())
        data_view_auth_3d_window_auth_3d_fcurve(&tt->fcurve_coverage_v, "Coverage V:", false);
    if (tt->has_repeat_u())
        data_view_auth_3d_window_auth_3d_fcurve(&tt->fcurve_repeat_u, "Repeat U:", false);
    if (tt->has_repeat_v())
        data_view_auth_3d_window_auth_3d_fcurve(&tt->fcurve_repeat_v, "Repeat V:", false);
    if (tt->has_translate_frame_u())
        data_view_auth_3d_window_auth_3d_fcurve(&tt->fcurve_translate_frame_u, "Translate Frame U:", false);
    if (tt->has_translate_frame_v())
        data_view_auth_3d_window_auth_3d_fcurve(&tt->fcurve_translate_frame_v, "Translate Frame V:", false);
    if (tt->has_offset_u())
        data_view_auth_3d_window_auth_3d_fcurve(&tt->fcurve_offset_u, "Offset U:", false);
    if (tt->has_offset_v())
        data_view_auth_3d_window_auth_3d_fcurve(&tt->fcurve_offset_v, "Offset V:", false);
    if (tt->has_rotate_frame())
        data_view_auth_3d_window_auth_3d_fcurve(&tt->fcurve_rotate_frame, "Rotate Frame:", false);
    if (tt->has_rotate())
        data_view_auth_3d_window_auth_3d_fcurve(&tt->fcurve_rotate, "Rotate:", false);
    ImGui::TreePop();
}

static void data_view_auth_3d_window_auth_3d_point(auth_3d_detail::Point* p) {
    if (!p)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!ImGui::TreeNodeEx(p, tree_node_flags, p->name.c_str()))
        return;

    data_view_auth_3d_window_auth_3d_model_transform(&p->model_transform);
    ImGui::TreePop();
}

static void data_view_auth_3d_window_auth_3d_post_process(auth_3d_detail::PostProcess* pp) {
    if (!pp || !pp->flag)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!ImGui::TreeNodeEx(pp, tree_node_flags, "Post Process"))
        return;

    if (pp->flag & auth_3d_detail::PostProcess::FLAG_LENS_FLARE) {
        ImGui::Text("Lens Flare");
        data_view_auth_3d_window_auth_3d_fcurve(&pp->fcurve_lens_flare, 0, false);
    }

    if (pp->flag & auth_3d_detail::PostProcess::FLAG_LENS_GHOST) {
        ImGui::Text("Lens Gghost");
        data_view_auth_3d_window_auth_3d_fcurve(&pp->fcurve_lens_ghost, 0, false);
    }

    if (pp->flag & auth_3d_detail::PostProcess::FLAG_LENS_SHAFT) {
        ImGui::Text("Lens Shaft");
        data_view_auth_3d_window_auth_3d_fcurve(&pp->fcurve_lens_shaft, 0, false);
    }

    if (pp->flag & auth_3d_detail::PostProcess::FLAG_RADIUS)
        data_view_auth_3d_window_auth_3d_fcurve_color4f(&pp->radius.fcurve_color, "Radius");

    if (pp->flag & auth_3d_detail::PostProcess::FLAG_INTENSITY)
        data_view_auth_3d_window_auth_3d_fcurve_color4f(&pp->intensity.fcurve_color, "intensity");

    if (pp->flag & auth_3d_detail::PostProcess::FLAG_SCENE_FADE)
        data_view_auth_3d_window_auth_3d_fcurve_color4f(&pp->scene_fade.fcurve_color, "Scene Fade");
    ImGui::TreePop();
}
