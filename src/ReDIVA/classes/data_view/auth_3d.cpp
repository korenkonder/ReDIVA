/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "auth_3d.hpp"
#include "../../../KKdLib/database/item_table.hpp"
#include "../../../CRE/auth_3d.hpp"
#include "../../imgui_helper.hpp"
#include "../../input.hpp"

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

extern int32_t width;
extern int32_t height;

const char* data_view_auth_3d_window_title = "Auth 3D##Data Viewer";

static void data_view_auth_3d_imgui_auth_3d_key(auth_3d_key* k, const char* format, bool offset);
static void data_view_auth_3d_imgui_auth_3d_rgba(auth_3d_rgba* rgba, const char* format);
static void data_view_auth_3d_imgui_auth_3d_vec3(auth_3d_vec3* vec, const char* format);
static void data_view_auth_3d_imgui_auth_3d_model_transform(auth_3d_model_transform* mt);

static void data_view_auth_3d_imgui_auth_3d_ambient(auth_3d_ambient* a);
static void data_view_auth_3d_imgui_auth_3d_camera_auxiliary(auth_3d_camera_auxiliary* ca);
static void data_view_auth_3d_imgui_auth_3d_camera_root(auth_3d_camera_root* cr, size_t index);
static void data_view_auth_3d_imgui_auth_3d_chara(auth_3d_chara* c);
static void data_view_auth_3d_imgui_auth_3d_curve(auth_3d_curve* c);
static void data_view_auth_3d_imgui_auth_3d_dof(auth_3d_dof* d);
static void data_view_auth_3d_imgui_auth_3d_fog(auth_3d_fog* f);
static void data_view_auth_3d_imgui_auth_3d_light(auth_3d_light* l);
static void data_view_auth_3d_imgui_auth_3d_m_object_hrc(auth_3d_m_object_hrc* moh);
static void data_view_auth_3d_imgui_auth_3d_material_list(auth_3d_material_list* ml);
static void data_view_auth_3d_imgui_auth_3d_object(auth_3d_object* o);
static void data_view_auth_3d_imgui_auth_3d_object_curve(auth_3d_object_curve* oc, const char* format);
static void data_view_auth_3d_imgui_auth_3d_object_hrc(auth_3d_object_hrc* oh);
static void data_view_auth_3d_imgui_auth_3d_object_instance(auth_3d_object_instance* oi);
static void data_view_auth_3d_imgui_auth_3d_object_model_transform(auth_3d_object_model_transform* omt);
static void data_view_auth_3d_imgui_auth_3d_object_node(auth_3d_object_node* on);
static void data_view_auth_3d_imgui_auth_3d_object_texture_pattern(auth_3d_object_texture_pattern* otp);
static void data_view_auth_3d_imgui_auth_3d_object_texture_transform(auth_3d_object_texture_transform* ott);
static void data_view_auth_3d_imgui_auth_3d_point(auth_3d_point* p);
static void data_view_auth_3d_imgui_auth_3d_post_process(auth_3d_post_process* pp);

bool data_view_auth_3d_init(class_data* data, render_context* rctx) {
    data_view_auth_3d_dispose(data);
    return true;
}

void data_view_auth_3d_imgui(class_data* data) {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImFont* font = ImGui::GetFont();

    float_t w = min_def((float_t)width, 480.0f);
    float_t h = min_def((float_t)height, 540.0f);

    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ w, h }, ImGuiCond_Appearing);

    data->imgui_focus = false;
    bool open = data->flags & CLASS_HIDDEN ? false : true;
    bool collapsed = !ImGui::Begin(data_view_auth_3d_window_title, &open, 0);
    if (!open) {
        enum_or(data->flags, CLASS_HIDE);
        ImGui::End();
        return;
    }
    else if (collapsed) {
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
        auth_3d* auth = &auth_3d_data->data[i];
        if (auth->id == -1)
            continue;

        size_t str_len = min_def(sizeof(buf) - 1, auth->file_name.size());
        memcpy(buf, auth->file_name.c_str(), str_len);
        buf[str_len] = 0;

        if (str_len >= 5 && !memcmp(&buf[str_len - 5], ".a3da", 6))
            buf[str_len - 5] = 0;

        tree_node_flags = tree_node_base_flags;

        bool visible = auth->enable && auth->visible;
        if (!visible)
            ImGui::PushStyleColor(ImGuiCol_Text, 0xFF888888);
        ImGui::PushID(auth->id);
        if (!ImGui::TreeNodeEx(auth, tree_node_flags,
            "ID: %5d; Frame: %5d; File: %s", auth->id & 0x7FFF, (int32_t)auth->frame, buf)) {
            ImGui::PopID();
            if (!visible)
                ImGui::PopStyleColor();
            continue;
        }

        tree_node_flags = tree_node_base_flags;
        tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;

        if (auth->chara_id != -1)
            ImGui::Text("Chara: %d", auth->chara_id);
        else
            ImGui::Text("Chara: None");

        if (auth->src_chara != auth->dst_chara)
            ImGui::Text("Chara Replace: %s/%s",
                chara_index_get_auth_3d_name((chara_index)auth->src_chara),
                chara_index_get_auth_3d_name((chara_index)auth->dst_chara));

        if (auth->ambient.size() > 0
            && ImGui::TreeNodeEx("Ambient", tree_node_flags)) {
            for (auth_3d_ambient& l : auth->ambient)
                data_view_auth_3d_imgui_auth_3d_ambient(&l);
            ImGui::TreePop();
        }

        if (auth->auth_2d.size() > 0
            && ImGui::TreeNodeEx("Auth 2D", tree_node_flags)) {
            for (std::string& l : auth->auth_2d)
                ImGui::Text("%s", l.c_str());
            ImGui::TreePop();
        }

        data_view_auth_3d_imgui_auth_3d_camera_auxiliary(&auth->camera_auxiliary);

        if (auth->camera_root.size() > 1
            && ImGui::TreeNodeEx("Camera Root", tree_node_flags)) {
            for (auth_3d_camera_root& l : auth->camera_root)
                data_view_auth_3d_imgui_auth_3d_camera_root(&l, &l - auth->camera_root.data());
            ImGui::TreePop();
        }
        else if (auth->camera_root.size() == 1)
            data_view_auth_3d_imgui_auth_3d_camera_root(&auth->camera_root[0], -1);

        if (auth->chara.size() > 0
            && ImGui::TreeNodeEx("Chara", tree_node_flags)) {
            for (auth_3d_chara& l : auth->chara)
                data_view_auth_3d_imgui_auth_3d_chara(&l);
            ImGui::TreePop();
        }

        if (auth->curve.size() > 0
            && ImGui::TreeNodeEx("Curve", tree_node_flags)) {
            for (auth_3d_curve& l : auth->curve)
                data_view_auth_3d_imgui_auth_3d_curve(&l);
            ImGui::TreePop();
        }

        data_view_auth_3d_imgui_auth_3d_dof(&auth->dof);

        if (auth->fog.size() > 0
            && ImGui::TreeNodeEx("Fog", tree_node_flags)) {
            for (auth_3d_fog& l : auth->fog)
                data_view_auth_3d_imgui_auth_3d_fog(&l);
            ImGui::TreePop();
        }

        if (auth->light.size() > 0
            && ImGui::TreeNodeEx("Light", tree_node_flags)) {
            for (auth_3d_light& l : auth->light)
                data_view_auth_3d_imgui_auth_3d_light(&l);
            ImGui::TreePop();
        }

        if (auth->m_object_hrc_list.size() > 0
            && ImGui::TreeNodeEx("M Object HRC", tree_node_flags)) {
            for (auth_3d_m_object_hrc*& l : auth->m_object_hrc_list)
                data_view_auth_3d_imgui_auth_3d_m_object_hrc(l);
            ImGui::TreePop();
        }

        if (auth->material_list.size() > 0
            && ImGui::TreeNodeEx("Material List", tree_node_flags)) {
            for (auth_3d_material_list& l : auth->material_list)
                data_view_auth_3d_imgui_auth_3d_material_list(&l);
            ImGui::TreePop();
        }

        if (auth->motion.size() > 0
            && ImGui::TreeNodeEx("Motion", tree_node_flags)) {
            for (std::string& l : auth->motion)
                ImGui::Text("%s", l.c_str());
            ImGui::TreePop();
        }

        if (auth->object_list.size() > 0
            && ImGui::TreeNodeEx("Object", tree_node_flags)) {
            for (auth_3d_object*& l : auth->object_list)
                data_view_auth_3d_imgui_auth_3d_object(l);
            ImGui::TreePop();
        }

        if (auth->object_hrc_list.size() > 0
            && ImGui::TreeNodeEx("Object HRC", tree_node_flags)) {
            for (auth_3d_object_hrc*& l : auth->object_hrc_list)
                data_view_auth_3d_imgui_auth_3d_object_hrc(l);
            ImGui::TreePop();
        }

        if (auth->point.size() > 0
            && ImGui::TreeNodeEx("Point", tree_node_flags)) {
            for (auth_3d_point& l : auth->point)
                data_view_auth_3d_imgui_auth_3d_point(&l);
            ImGui::TreePop();
        }

        data_view_auth_3d_imgui_auth_3d_post_process(&auth->post_process);

        ImGui::TreePop();
        ImGui::PopID();
        if (!visible)
            ImGui::PopStyleColor();
    }

    data->imgui_focus |= ImGui::IsWindowFocused();
    ImGui::End();
}

bool data_view_auth_3d_dispose(class_data* data) {
    return true;
}

static void data_view_auth_3d_imgui_auth_3d_key(auth_3d_key* k, const char* format, bool offset) {
    if (!k)
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

    switch (k->type) {
    case A3DA_KEY_NONE:
    default:
        ImGui::Text("V: %#.6g", 0.0f);
        break;
    case A3DA_KEY_STATIC:
        ImGui::Text("V: %#.6g", k->value);
        break;
    case A3DA_KEY_LINEAR:
    case A3DA_KEY_HERMITE:
    case A3DA_KEY_HOLD:
        ImGui::Text("F: %5g; V: %#.6g", k->frame, k->value_interp);
        break;
    }
}

static void data_view_auth_3d_imgui_auth_3d_rgba(auth_3d_rgba* rgba, const char* format) {
    if (!rgba || !rgba->flags || !format)
        return;

    ImGui::Text(format);
    if (rgba->flags & A3DA_RGBA_R)
        data_view_auth_3d_imgui_auth_3d_key(&rgba->r, "R:", true);
    if (rgba->flags & A3DA_RGBA_G)
        data_view_auth_3d_imgui_auth_3d_key(&rgba->g, "G:", true);
    if (rgba->flags & A3DA_RGBA_B)
        data_view_auth_3d_imgui_auth_3d_key(&rgba->b, "B:", true);
    if (rgba->flags & A3DA_RGBA_A)
        data_view_auth_3d_imgui_auth_3d_key(&rgba->a, "A:", true);
}

static void data_view_auth_3d_imgui_auth_3d_vec3(auth_3d_vec3* vec, const char* format) {
    if (!vec || !format)
        return;

    ImGui::Text(format);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, 0xFF404040);
    data_view_auth_3d_imgui_auth_3d_key(&vec->x, "X:", true);
    data_view_auth_3d_imgui_auth_3d_key(&vec->y, "Y:", true);
    data_view_auth_3d_imgui_auth_3d_key(&vec->z, "Z:", true);
    ImGui::PopStyleColor();
}

static void data_view_auth_3d_imgui_auth_3d_model_transform(auth_3d_model_transform* mt) {
    if (!mt)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!ImGui::TreeNodeEx(mt, tree_node_flags, "Model Transform"))
        return;

    data_view_auth_3d_imgui_auth_3d_vec3(&mt->translation, "Translation");
    data_view_auth_3d_imgui_auth_3d_vec3(&mt->rotation, "Rotation");
    data_view_auth_3d_imgui_auth_3d_vec3(&mt->scale, "Scale");

    ImGui::Text("Visibility");
    data_view_auth_3d_imgui_auth_3d_key(&mt->visibility, 0, true);
    ImGui::TreePop();
}

static void data_view_auth_3d_imgui_auth_3d_ambient(auth_3d_ambient* a) {
    if (!a || !a->flags)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!ImGui::TreeNodeEx(a, tree_node_flags, a->name.c_str()))
        return;

    if (a->flags & A3DA_AMBIENT_LIGHT_DIFFUSE)
        data_view_auth_3d_imgui_auth_3d_rgba(&a->light_diffuse, "Light Diffuse");
    if (a->flags & A3DA_AMBIENT_RIM_LIGHT_DIFFUSE)
        data_view_auth_3d_imgui_auth_3d_rgba(&a->rim_light_diffuse, "Rim Light Diffuse");
    ImGui::TreePop();
}

static void data_view_auth_3d_imgui_auth_3d_camera_auxiliary(auth_3d_camera_auxiliary* ca) {
    if (!ca || !ca->flags)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!ImGui::TreeNodeEx(ca, tree_node_flags, "Camera Auxiliary"))
        return;

    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_AUTO_EXPOSURE) {
        ImGui::Text("Auto Exposure");
        data_view_auth_3d_imgui_auth_3d_key(&ca->auto_exposure, 0, true);
    }
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_EXPOSURE) {
        ImGui::Text("Exposure");
        data_view_auth_3d_imgui_auth_3d_key(&ca->exposure, 0, true);
    }
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_EXPOSURE_RATE) {
        ImGui::Text("Exposure Rate");
        data_view_auth_3d_imgui_auth_3d_key(&ca->exposure_rate, 0, true);
    }
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_GAMMA) {
        ImGui::Text("Gamma");
        data_view_auth_3d_imgui_auth_3d_key(&ca->gamma, 0, true);
    }
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_GAMMA_RATE) {
        ImGui::Text("Gamma Rate");
        data_view_auth_3d_imgui_auth_3d_key(&ca->gamma_rate, 0, true);
    }
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_SATURATE) {
        ImGui::Text("Saturate");
        data_view_auth_3d_imgui_auth_3d_key(&ca->saturate, 0, true);
    }
    ImGui::TreePop();
}

static void data_view_auth_3d_imgui_auth_3d_camera_root(auth_3d_camera_root* cr, size_t index) {
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

    data_view_auth_3d_imgui_auth_3d_vec3(&cr->interest.translation, "Interest");
    data_view_auth_3d_imgui_auth_3d_model_transform(&cr->model_transform);

    auth_3d_camera_root_view_point* vp = &cr->view_point;
    data_view_auth_3d_imgui_auth_3d_vec3(&vp->model_transform.translation, "View Point");

    ImGui::Text("Aspect");
    ImGui::Text("   %#.6g", vp->aspect);

    if (vp->flags & AUTH_3D_CAMERA_ROOT_VIEW_POINT_FOV) {
        ImGui::Text(vp->fov_is_horizontal ? "FOV Horizontal" : "FOV");
        data_view_auth_3d_imgui_auth_3d_key(&vp->fov, 0, true);
    }
    else {
        ImGui::Text("Camera Aperture Width");
        ImGui::Text("   %#.6g", vp->camera_aperture_w);

        ImGui::Text("Camera Aperture Height");
        ImGui::Text("   %#.6g", vp->camera_aperture_h);

        ImGui::Text("Focal Length");
        data_view_auth_3d_imgui_auth_3d_key(&vp->focal_length, 0, true);
    }

    if (vp->flags & AUTH_3D_CAMERA_ROOT_VIEW_POINT_ROLL) {
        ImGui::Text("Roll");
        data_view_auth_3d_imgui_auth_3d_key(&vp->roll, 0, true);
    }
    ImGui::TreePop();
}

static void data_view_auth_3d_imgui_auth_3d_chara(auth_3d_chara* c) {
    if (!c)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!ImGui::TreeNodeEx(c, tree_node_flags, c->name.c_str()))
        return;

    data_view_auth_3d_imgui_auth_3d_model_transform(&c->model_transform);
    ImGui::TreePop();
}

static void data_view_auth_3d_imgui_auth_3d_curve(auth_3d_curve* c) {
    if (!c)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!ImGui::TreeNodeEx(c, tree_node_flags, c->name.c_str()))
        return;

    data_view_auth_3d_imgui_auth_3d_key(&c->curve, 0, false);
    ImGui::TreePop();
}

static void data_view_auth_3d_imgui_auth_3d_dof(auth_3d_dof* d) {
    if (!d || !d->has_dof)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    bool enable = fabsf(d->model_transform.rotation_value.z) > 0.000001f;
    if (!enable)
        ImGui::PushStyleColor(ImGuiCol_Text, 0xFF888888);
    if (!ImGui::TreeNodeEx(d, tree_node_flags, "DOF")) {
        if (!enable)
            ImGui::PopStyleColor();
        return;
    }

    auth_3d_model_transform* mt = &d->model_transform;

    data_view_auth_3d_imgui_auth_3d_vec3(&mt->translation, "Position");

    ImGui::Text("Focus Range");
    data_view_auth_3d_imgui_auth_3d_key(&mt->scale.x, 0, true);

    ImGui::Text("Fuzzing Range");
    data_view_auth_3d_imgui_auth_3d_key(&mt->rotation.x, 0, true);

    ImGui::Text("Ratio");
    data_view_auth_3d_imgui_auth_3d_key(&mt->rotation.y, 0, true);

    ImGui::Text("Enable");
    data_view_auth_3d_imgui_auth_3d_key(&mt->rotation.z, 0, true);
    ImGui::TreePop();

    if (!enable)
        ImGui::PopStyleColor();
}

static void data_view_auth_3d_imgui_auth_3d_fog(auth_3d_fog* f) {
    if (!f || !f->flags)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!ImGui::TreeNodeEx(f, tree_node_flags, auth_3d_fog_name[f->id]))
        return;

    if (f->flags & AUTH_3D_FOG_COLOR)
        data_view_auth_3d_imgui_auth_3d_rgba(&f->color, "Color");

    if (f->flags & AUTH_3D_FOG_DENSITY) {
        ImGui::Text("Density");
        data_view_auth_3d_imgui_auth_3d_key(&f->density, 0, true);
    }

    if (f->flags & AUTH_3D_FOG_END) {
        ImGui::Text("End");
        data_view_auth_3d_imgui_auth_3d_key(&f->end, 0, true);
    }

    if (f->flags & AUTH_3D_FOG_START) {
        ImGui::Text("Start");
        data_view_auth_3d_imgui_auth_3d_key(&f->start, 0, true);
    }
    ImGui::TreePop();
}

static void data_view_auth_3d_imgui_auth_3d_light(auth_3d_light* l) {
    if (!l || !l->flags)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!ImGui::TreeNodeEx(l, tree_node_flags, auth_3d_light_name[l->id]))
        return;

    if (l->flags & AUTH_3D_LIGHT_AMBIENT)
        data_view_auth_3d_imgui_auth_3d_rgba(&l->ambient, "Ambient");

    if (l->flags & AUTH_3D_LIGHT_CONE_ANGLE) {
        ImGui::Text("Cone Angle");
        data_view_auth_3d_imgui_auth_3d_key(&l->cone_angle, 0, true);
    }

    if (l->flags & AUTH_3D_LIGHT_CONSTANT) {
        ImGui::Text("Constant");
        data_view_auth_3d_imgui_auth_3d_key(&l->constant, 0, true);
    }

    if (l->flags & AUTH_3D_LIGHT_DIFFUSE)
        data_view_auth_3d_imgui_auth_3d_rgba(&l->diffuse, "Diffuse");

    if (l->flags & AUTH_3D_LIGHT_DROP_OFF) {
        ImGui::Text("Drop Off");
        data_view_auth_3d_imgui_auth_3d_key(&l->drop_off, 0, true);
    }

    if (l->flags & AUTH_3D_LIGHT_FAR) {
        ImGui::Text("Far");
        data_view_auth_3d_imgui_auth_3d_key(&l->_far, 0, true);
    }

    if (l->flags & AUTH_3D_LIGHT_INTENSITY) {
        ImGui::Text("Intensity");
        data_view_auth_3d_imgui_auth_3d_key(&l->intensity, 0, true);
    }

    if (l->flags & AUTH_3D_LIGHT_LINEAR) {
        ImGui::Text("Linear");
        data_view_auth_3d_imgui_auth_3d_key(&l->linear, 0, true);
    }

    if (l->flags & AUTH_3D_LIGHT_POSITION)
        data_view_auth_3d_imgui_auth_3d_vec3(&l->position.translation, "Position");

    if (l->flags & AUTH_3D_LIGHT_QUADRATIC) {
        ImGui::Text("Quadratic");
        data_view_auth_3d_imgui_auth_3d_key(&l->quadratic, 0, true);
    }

    if (l->flags & AUTH_3D_LIGHT_SPECULAR)
        data_view_auth_3d_imgui_auth_3d_rgba(&l->specular, "Specular");

    if (l->flags & AUTH_3D_LIGHT_SPOT_DIRECTION)
        data_view_auth_3d_imgui_auth_3d_vec3(&l->spot_direction.translation, "Spot Direction");

    if (l->flags & AUTH_3D_LIGHT_TONE_CURVE)
        data_view_auth_3d_imgui_auth_3d_rgba(&l->tone_curve, "Tone Curve");
    ImGui::TreePop();
}

static void data_view_auth_3d_imgui_auth_3d_m_object_hrc(auth_3d_m_object_hrc* moh) {
    if (!moh)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    bool visible = moh->model_transform.visible;
    if (!visible)
        ImGui::PushStyleColor(ImGuiCol_Text, 0xFF888888);

    if (!ImGui::TreeNodeEx(moh, tree_node_flags, moh->name.c_str())) {
        if (!visible)
            ImGui::PopStyleColor();
        return;
    }

    if (moh->instance.size() > 0
        && ImGui::TreeNodeEx("Instance", tree_node_flags)) {
        for (auth_3d_object_instance& i : moh->instance)
            data_view_auth_3d_imgui_auth_3d_object_instance(&i);
        ImGui::TreePop();
    }

    data_view_auth_3d_imgui_auth_3d_object_model_transform(&moh->model_transform);

    if (moh->node.size() > 0
        && ImGui::TreeNodeEx("Node", tree_node_flags)) {
        for (auth_3d_object_node& i : moh->node)
            data_view_auth_3d_imgui_auth_3d_object_node(&i);
        ImGui::TreePop();
    }
    ImGui::TreePop();

    if (!visible)
        ImGui::PopStyleColor();
}

static void data_view_auth_3d_imgui_auth_3d_material_list(auth_3d_material_list* ml) {
    if (!ml || !ml->flags)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!ImGui::TreeNodeEx(ml, tree_node_flags, ml->name.c_str()))
        return;

    if (ml->flags & AUTH_3D_MATERIAL_LIST_BLEND_COLOR)
        data_view_auth_3d_imgui_auth_3d_rgba(&ml->blend_color, "Blend Color");

    if (ml->flags & AUTH_3D_MATERIAL_LIST_GLOW_INTENSITY) {
        ImGui::Text("Glow Intensity");
        data_view_auth_3d_imgui_auth_3d_key(&ml->glow_intensity, 0, false);
    }

    if (ml->flags & AUTH_3D_MATERIAL_LIST_EMISSION)
        data_view_auth_3d_imgui_auth_3d_rgba(&ml->emission, "Emission");
    ImGui::TreePop();
}

static void data_view_auth_3d_imgui_auth_3d_object(auth_3d_object* o) {
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

    bool visible = o->model_transform.visible;
    if (!visible)
        ImGui::PushStyleColor(ImGuiCol_Text, 0xFF888888);

    tree_node_flags = tree_node_base_flags;

    if (!ImGui::TreeNodeEx(o, tree_node_flags, name)) {
        if (!visible)
            ImGui::PopStyleColor();
        return;
    }

    data_view_auth_3d_imgui_auth_3d_model_transform(&o->model_transform);

    data_view_auth_3d_imgui_auth_3d_object_curve(&o->morph, "Morph");
    data_view_auth_3d_imgui_auth_3d_object_curve(&o->pattern, "Pattern");

    tree_node_flags = tree_node_base_flags;

    if (o->texture_pattern.size() > 0
        && ImGui::TreeNodeEx("Texture Pattern", tree_node_flags)) {
        for (auth_3d_object_texture_pattern& i : o->texture_pattern)
            data_view_auth_3d_imgui_auth_3d_object_texture_pattern(&i);
        ImGui::TreePop();
    }

    tree_node_flags = tree_node_base_flags;

    if (o->texture_transform.size() > 0
        && ImGui::TreeNodeEx("Texture Transform", tree_node_flags)) {
        for (auth_3d_object_texture_transform& i : o->texture_transform)
            data_view_auth_3d_imgui_auth_3d_object_texture_transform(&i);
        ImGui::TreePop();
    }

    tree_node_flags = tree_node_base_flags;
    tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;

    if (o->children_object.size() > 0
        && ImGui::TreeNodeEx("Children Object", tree_node_flags)) {
        for (auth_3d_object*& i : o->children_object)
            data_view_auth_3d_imgui_auth_3d_object(i);
        ImGui::TreePop();
    }

    tree_node_flags = tree_node_base_flags;
    tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;

    if (o->children_object_hrc.size() > 0
        && ImGui::TreeNodeEx("Children Object HRC", tree_node_flags)) {
        for (auth_3d_object_hrc*& i : o->children_object_hrc)
            data_view_auth_3d_imgui_auth_3d_object_hrc(i);
        ImGui::TreePop();
    }
    ImGui::TreePop();

    if (!visible)
        ImGui::PopStyleColor();
}

static void data_view_auth_3d_imgui_auth_3d_object_curve(auth_3d_object_curve* oc, const char* format) {
    if (!oc || !oc->curve || !format)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!ImGui::TreeNodeEx(oc, tree_node_flags, format))
        return;

    data_view_auth_3d_imgui_auth_3d_curve(oc->curve);
    ImGui::TreePop();
}

static void data_view_auth_3d_imgui_auth_3d_object_hrc(auth_3d_object_hrc* oh) {
    if (!oh)
        return;

    ImGuiTreeNodeFlags tree_node_base_flags = 0;
    tree_node_base_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_base_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_base_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGuiTreeNodeFlags tree_node_flags;

    const char* s = oh->name.c_str();
    const char* name = s;
    while (s = strchr(name = s, '|'))
        s++;

    bool visible = oh->node[0].model_transform.visible;
    if (!visible)
        ImGui::PushStyleColor(ImGuiCol_Text, 0xFF888888);

    tree_node_flags = tree_node_base_flags;

    if (!ImGui::TreeNodeEx(oh, tree_node_flags, name)) {
        if (!visible)
            ImGui::PopStyleColor();
        return;
    }

    tree_node_flags = tree_node_base_flags;

    if (oh->node.size() > 0
        && ImGui::TreeNodeEx("Node", tree_node_flags)) {
        for (auth_3d_object_node& i : oh->node)
            data_view_auth_3d_imgui_auth_3d_object_node(&i);
        ImGui::TreePop();
    }

    tree_node_flags = tree_node_base_flags;
    tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;

    if (oh->children_object.size() > 0
        && ImGui::TreeNodeEx("Children Object", tree_node_flags)) {
        for (auth_3d_object*& i : oh->children_object)
            data_view_auth_3d_imgui_auth_3d_object(i);
        ImGui::TreePop();
    }

    tree_node_flags = tree_node_base_flags;
    tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;

    if (oh->children_object_hrc.size() > 0
        && ImGui::TreeNodeEx("Children Object HRC", tree_node_flags)) {
        for (auth_3d_object_hrc*& i : oh->children_object_hrc)
            data_view_auth_3d_imgui_auth_3d_object_hrc(i);
        ImGui::TreePop();
    }
    ImGui::TreePop();

    if (!visible)
        ImGui::PopStyleColor();
}

static void data_view_auth_3d_imgui_auth_3d_object_instance(auth_3d_object_instance* oi) {
    if (!oi)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    bool visible = oi->model_transform.visible;
    if (!visible)
        ImGui::PushStyleColor(ImGuiCol_Text, 0xFF888888);

    if (!ImGui::TreeNodeEx(oi, tree_node_flags, oi->name.c_str())) {
        if (!visible)
            ImGui::PopStyleColor();
        return;
    }

    data_view_auth_3d_imgui_auth_3d_object_model_transform(&oi->model_transform);

    if (oi->shadow)
        ImGui::Text("Shadow: True");

    ImGui::Text("UID Name: %s", oi->uid_name.c_str());
    ImGui::TreePop();

    if (!visible)
        ImGui::PopStyleColor();
}

static void data_view_auth_3d_imgui_auth_3d_object_model_transform(auth_3d_object_model_transform* omt) {
    if (!omt)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!ImGui::TreeNodeEx(omt, tree_node_flags, "Model Transform"))
        return;

    ImGui::Text("Frame: %5f", omt->frame);

    data_view_auth_3d_imgui_auth_3d_vec3(&omt->translation, "Translation");
    data_view_auth_3d_imgui_auth_3d_vec3(&omt->rotation, "Rotation");
    data_view_auth_3d_imgui_auth_3d_vec3(&omt->scale, "Scale");

    ImGui::Text("Visibility");
    data_view_auth_3d_imgui_auth_3d_key(&omt->visibility, 0, true);
    ImGui::TreePop();
}

static void data_view_auth_3d_imgui_auth_3d_object_node(auth_3d_object_node* on) {
    if (!on)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!ImGui::TreeNodeEx(on, tree_node_flags, on->name.c_str()))
        return;

    data_view_auth_3d_imgui_auth_3d_object_model_transform(&on->model_transform);

    if (on->flags & AUTH_3D_OBJECT_NODE_JOINT_ORIENT) {
        ImGui::Text("Joint Orient ");
        ImGui::Text("   X: %#.6g", on->joint_orient.x);
        ImGui::Text("   Y: %#.6g", on->joint_orient.y);
        ImGui::Text("   Z: %#.6g", on->joint_orient.z);
    }

    ImGui::Text("Parent: %d", on->parent);
    ImGui::TreePop();
}

static void data_view_auth_3d_imgui_auth_3d_object_texture_pattern(auth_3d_object_texture_pattern* otp) {
    if (!otp)
        return;

    data_view_auth_3d_imgui_auth_3d_object_curve(&otp->pattern, otp->name.c_str());
}

static void data_view_auth_3d_imgui_auth_3d_object_texture_transform(auth_3d_object_texture_transform* ott) {
    if (!ott || !ott->flags)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!ImGui::TreeNodeEx(ott, tree_node_flags, ott->name.c_str()))
        return;

    if (ott->flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_COVERAGE_U)
        data_view_auth_3d_imgui_auth_3d_key(&ott->coverage_u, "Coverage U:", false);
    if (ott->flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_COVERAGE_V)
        data_view_auth_3d_imgui_auth_3d_key(&ott->coverage_v, "Coverage V:", false);
    if (ott->flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_OFFSET_U)
        data_view_auth_3d_imgui_auth_3d_key(&ott->offset_u, "Offset U:", false);
    if (ott->flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_OFFSET_V)
        data_view_auth_3d_imgui_auth_3d_key(&ott->offset_v, "Offset V:", false);
    if (ott->flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_REPEAT_U)
        data_view_auth_3d_imgui_auth_3d_key(&ott->repeat_u, "Repeat U:", false);
    if (ott->flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_REPEAT_V)
        data_view_auth_3d_imgui_auth_3d_key(&ott->repeat_v, "Repeat V:", false);
    if (ott->flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_ROTATE)
        data_view_auth_3d_imgui_auth_3d_key(&ott->rotate, "Rotate:", false);
    if (ott->flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_ROTATE_FRAME)
        data_view_auth_3d_imgui_auth_3d_key(&ott->rotate_frame, "Rotate Frame:", false);
    if (ott->flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_TRANSLATE_FRAME_U)
        data_view_auth_3d_imgui_auth_3d_key(&ott->translate_frame_u, "Translate Frame U:", false);
    if (ott->flags & AUTH_3D_OBJECT_TEXTURE_TRANSFORM_TRANSLATE_FRAME_V)
        data_view_auth_3d_imgui_auth_3d_key(&ott->translate_frame_v, "Translate Frame V:", false);
    ImGui::TreePop();
}

static void data_view_auth_3d_imgui_auth_3d_point(auth_3d_point* p) {
    if (!p)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!ImGui::TreeNodeEx(p, tree_node_flags, p->name.c_str()))
        return;

    data_view_auth_3d_imgui_auth_3d_model_transform(&p->model_transform);
    ImGui::TreePop();
}

static void data_view_auth_3d_imgui_auth_3d_post_process(auth_3d_post_process* pp) {
    if (!pp || !pp->flags)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!ImGui::TreeNodeEx(pp, tree_node_flags, "Post Process"))
        return;

    if (pp->flags & AUTH_3D_POST_PROCESS_INTENSITY)
        data_view_auth_3d_imgui_auth_3d_rgba(&pp->intensity, "intensity");

    if (pp->flags & AUTH_3D_POST_PROCESS_LENS_FLARE) {
        ImGui::Text("Lens Flare");
        data_view_auth_3d_imgui_auth_3d_key(&pp->lens_flare, 0, false);
    }

    if (pp->flags & AUTH_3D_POST_PROCESS_LENS_GHOST) {
        ImGui::Text("Lens Gghost");
        data_view_auth_3d_imgui_auth_3d_key(&pp->lens_ghost, 0, false);
    }

    if (pp->flags & AUTH_3D_POST_PROCESS_LENS_SHAFT) {
        ImGui::Text("Lens Shaft");
        data_view_auth_3d_imgui_auth_3d_key(&pp->lens_shaft, 0, false);
    }

    if (pp->flags & AUTH_3D_POST_PROCESS_RADIUS)
        data_view_auth_3d_imgui_auth_3d_rgba(&pp->radius, "Radius");

    if (pp->flags & AUTH_3D_POST_PROCESS_SCENE_FADE)
        data_view_auth_3d_imgui_auth_3d_rgba(&pp->scene_fade, "Scene Fade");
    ImGui::TreePop();
}
