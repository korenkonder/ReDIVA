/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "auth_3d.h"
#include "../../../CRE/auth_3d.h"
#include "../../input.h"
#include "../imgui_helper.h"

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
    ImGuiIO* io = igGetIO();
    ImGuiStyle* style = igGetStyle();
    ImFont* font = igGetFont();

    float_t w = min((float_t)width, 480.0f);
    float_t h = min((float_t)height, 540.0f);

    igSetNextWindowPos(ImVec2_Empty, ImGuiCond_Appearing, ImVec2_Empty);
    igSetNextWindowSize({ w, h }, ImGuiCond_Appearing);

    data->imgui_focus = false;
    bool open = data->flags & CLASS_HIDDEN ? false : true;
    bool collapsed = !igBegin(data_view_auth_3d_window_title, &open, 0);
    if (!open) {
        enum_or(data->flags, CLASS_HIDE);
        igEnd();
        return;
    }
    else if (collapsed) {
        igEnd();
        return;
    }

    ImGuiTreeNodeFlags tree_node_base_flags = 0;
    tree_node_base_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_base_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_base_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGuiTreeNodeFlags tree_node_flags;

    char buf[0x200];
    for (size_t i = 0; i < AUTH_3D_DATA_COUNT; i++) {
        auth_3d* auth = &auth_3d_data.data[i];
        if (auth->id == -1)
            continue;

        size_t str_len = min(sizeof(buf) - 1, auth->file_name.size());
        memcpy(buf, auth->file_name.c_str(), str_len);
        buf[str_len] = 0;

        if (str_len >= 5 && !memcmp(&buf[str_len - 5], ".a3da", 6))
            buf[str_len - 5] = 0;

        tree_node_flags = tree_node_base_flags;

        igPushID_Int(auth->id);
        if (!igTreeNodeEx_Ptr(auth, tree_node_flags,
            "ID: %5d; File: %s; Frame: %5d", auth->id & 0x7FFF, buf, auth->frame_int)) {
            igPopID();
            continue;
        }

        tree_node_flags = tree_node_base_flags;
        tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;

        if (auth->ambient.size() > 0
            && igTreeNodeEx_Str("Ambient", tree_node_flags)) {
            for (auth_3d_ambient& l : auth->ambient)
                data_view_auth_3d_imgui_auth_3d_ambient(&l);
            igTreePop();
        }

        if (auth->auth_2d.size() > 0
            && igTreeNodeEx_Str("Auth 2D", tree_node_flags)) {
            for (std::string& l : auth->auth_2d)
                igText(l.c_str());
            igTreePop();
        }

        data_view_auth_3d_imgui_auth_3d_camera_auxiliary(&auth->camera_auxiliary);

        if (auth->camera_root.size() > 1
            && igTreeNodeEx_Str("Camera Root", tree_node_flags)) {
            for (auth_3d_camera_root& l : auth->camera_root)
                data_view_auth_3d_imgui_auth_3d_camera_root(&l, &l - auth->camera_root.data());
            igTreePop();
        }
        else if (auth->camera_root.size() == 1)
            data_view_auth_3d_imgui_auth_3d_camera_root(&auth->camera_root[0], -1);

        if (auth->chara.size() > 0
            && igTreeNodeEx_Str("Chara", tree_node_flags)) {
            for (auth_3d_chara& l : auth->chara)
                data_view_auth_3d_imgui_auth_3d_chara(&l);
            igTreePop();
        }

        data_view_auth_3d_imgui_auth_3d_dof(&auth->dof);

        if (auth->fog.size() > 0
            && igTreeNodeEx_Str("Fog", tree_node_flags)) {
            for (auth_3d_fog& l : auth->fog)
                data_view_auth_3d_imgui_auth_3d_fog(&l);
            igTreePop();
        }

        if (auth->light.size() > 0
            && igTreeNodeEx_Str("Light", tree_node_flags)) {
            for (auth_3d_light& l : auth->light)
                data_view_auth_3d_imgui_auth_3d_light(&l);
            igTreePop();
        }

        if (auth->m_object_hrc_list.size() > 0
            && igTreeNodeEx_Str("M Object HRC", tree_node_flags)) {
            for (auth_3d_m_object_hrc*& l : auth->m_object_hrc_list)
                data_view_auth_3d_imgui_auth_3d_m_object_hrc(l);
            igTreePop();
        }

        if (auth->material_list.size() > 0
            && igTreeNodeEx_Str("Material List", tree_node_flags)) {
            for (auth_3d_material_list& l : auth->material_list)
                data_view_auth_3d_imgui_auth_3d_material_list(&l);
            igTreePop();
        }

        if (auth->motion.size() > 0
            && igTreeNodeEx_Str("Motion", tree_node_flags)) {
            for (std::string& l : auth->motion)
                igText(l.c_str());
            igTreePop();
        }

        if (auth->object_list.size() > 0
            && igTreeNodeEx_Str("Object", tree_node_flags)) {
            for (auth_3d_object*& l : auth->object_list)
                data_view_auth_3d_imgui_auth_3d_object(l);
            igTreePop();
        }

        if (auth->object_hrc_list.size() > 0
            && igTreeNodeEx_Str("Object HRC", tree_node_flags)) {
            for (auth_3d_object_hrc*& l : auth->object_hrc_list)
                data_view_auth_3d_imgui_auth_3d_object_hrc(l);
            igTreePop();
        }

        if (auth->point.size() > 0
            && igTreeNodeEx_Str("Point", tree_node_flags)) {
            for (auth_3d_point& l : auth->point)
                data_view_auth_3d_imgui_auth_3d_point(&l);
            igTreePop();
        }

        data_view_auth_3d_imgui_auth_3d_post_process(&auth->post_process);

        igTreePop();
        igPopID();
    }

    data->imgui_focus |= igIsWindowFocused(0);
    igEnd();
}

bool data_view_auth_3d_dispose(class_data* data) {
    return true;
}

static void data_view_auth_3d_imgui_auth_3d_key(auth_3d_key* k, const char* format, bool offset) {
    if (!k)
        return;

    if (offset) {
        igText("  ");
        if (format) {
            igSameLine(0.0f, -1.0f);
            igText(format);
        }
        igSameLine(0.0f, -1.0f);
    }
    else if (format) {
        igText(format);
        igSameLine(0.0f, -1.0f);
    }

    switch (k->type) {
    case A3DA_KEY_NONE:
    default:
        igText("V: %#.6g", 0.0f);
        break;
    case A3DA_KEY_STATIC:
        igText("V: %#.6g", k->value);
        break;
    case A3DA_KEY_LINEAR:
    case A3DA_KEY_HERMITE:
    case A3DA_KEY_HOLD:
        igText("F: %5g; V: %#.6g", k->frame, k->value_interp);
        break;
    }
}

static void data_view_auth_3d_imgui_auth_3d_rgba(auth_3d_rgba* rgba, const char* format) {
    if (!rgba || !rgba->flags || !format)
        return;

    igText(format);
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

    igText(format);
    igPushStyleColor_U32(ImGuiCol_WindowBg, 0xFF404040);
    data_view_auth_3d_imgui_auth_3d_key(&vec->x, "X:", true);
    data_view_auth_3d_imgui_auth_3d_key(&vec->y, "Y:", true);
    data_view_auth_3d_imgui_auth_3d_key(&vec->z, "Z:", true);
    igPopStyleColor(1);
}

static void data_view_auth_3d_imgui_auth_3d_model_transform(auth_3d_model_transform* mt) {
    if (!mt)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!igTreeNodeEx_Ptr(mt, tree_node_flags, "Model Transform"))
        return;

    data_view_auth_3d_imgui_auth_3d_vec3(&mt->translation, "Translation");
    data_view_auth_3d_imgui_auth_3d_vec3(&mt->rotation, "Rotation");
    data_view_auth_3d_imgui_auth_3d_vec3(&mt->scale, "Scale");

    igText("Visibility");
    data_view_auth_3d_imgui_auth_3d_key(&mt->visibility, 0, true);
    igTreePop();
}

static void data_view_auth_3d_imgui_auth_3d_ambient(auth_3d_ambient* a) {
    if (!a || !a->flags)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!igTreeNodeEx_Ptr(a, tree_node_flags, a->name.c_str()))
        return;

    if (a->flags & A3DA_AMBIENT_LIGHT_DIFFUSE)
        data_view_auth_3d_imgui_auth_3d_rgba(&a->light_diffuse, "Light Diffuse");
    if (a->flags & A3DA_AMBIENT_RIM_LIGHT_DIFFUSE)
        data_view_auth_3d_imgui_auth_3d_rgba(&a->rim_light_diffuse, "Rim Light Diffuse");
    igTreePop();
}

static void data_view_auth_3d_imgui_auth_3d_camera_auxiliary(auth_3d_camera_auxiliary* ca) {
    if (!ca || !ca->flags)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!igTreeNodeEx_Ptr(ca, tree_node_flags, "Camera Auxiliary"))
        return;

    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_AUTO_EXPOSURE) {
        igText("Auto Exposure");
        data_view_auth_3d_imgui_auth_3d_key(&ca->auto_exposure, 0, true);
    }
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_EXPOSURE) {
        igText("Exposure");
        data_view_auth_3d_imgui_auth_3d_key(&ca->exposure, 0, true);
    }
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_EXPOSURE_RATE) {
        igText("Exposure Rate");
        data_view_auth_3d_imgui_auth_3d_key(&ca->exposure_rate, 0, true);
    }
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_GAMMA) {
        igText("Gamma");
        data_view_auth_3d_imgui_auth_3d_key(&ca->gamma, 0, true);
    }
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_GAMMA_RATE) {
        igText("Gamma Rate");
        data_view_auth_3d_imgui_auth_3d_key(&ca->gamma_rate, 0, true);
    }
    if (ca->flags & AUTH_3D_CAMERA_AUXILIARY_SATURATE) {
        igText("Saturate");
        data_view_auth_3d_imgui_auth_3d_key(&ca->saturate, 0, true);
    }
    igTreePop();
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

    if (!igTreeNodeEx_Ptr(cr, tree_node_flags,
        index == -1 ? "Camera Root" : "%d", (int32_t)index))
        return;

    data_view_auth_3d_imgui_auth_3d_vec3(&cr->interest.translation, "Interest");
    data_view_auth_3d_imgui_auth_3d_model_transform(&cr->model_transform);

    auth_3d_camera_root_view_point* vp = &cr->view_point;
    data_view_auth_3d_imgui_auth_3d_vec3(&vp->model_transform.translation, "View Point");

    igText("Aspect");
    igText("   %#.6g", vp->aspect);

    if (vp->flags & AUTH_3D_CAMERA_ROOT_VIEW_POINT_FOV) {
        igText(vp->fov_is_horizontal ? "FOV Horizontal" : "FOV");
        data_view_auth_3d_imgui_auth_3d_key(&vp->fov, 0, true);
    }
    else {
        igText("Camera Aperture Width");
        igText("   %#.6g", vp->camera_aperture_w);

        igText("Camera Aperture Height");
        igText("   %#.6g", vp->camera_aperture_h);

        igText("Focal Length");
        data_view_auth_3d_imgui_auth_3d_key(&vp->focal_length, 0, true);
    }

    if (vp->flags & AUTH_3D_CAMERA_ROOT_VIEW_POINT_ROLL) {
        igText("Roll");
        data_view_auth_3d_imgui_auth_3d_key(&vp->roll, 0, true);
    }
    igTreePop();
}

static void data_view_auth_3d_imgui_auth_3d_chara(auth_3d_chara* c) {
    if (!c)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!igTreeNodeEx_Ptr(c, tree_node_flags, c->name.c_str()))
        return;

    data_view_auth_3d_imgui_auth_3d_model_transform(&c->model_transform);
    igTreePop();
}

static void data_view_auth_3d_imgui_auth_3d_curve(auth_3d_curve* c) {
    if (!c)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!igTreeNodeEx_Ptr(c, tree_node_flags, c->name.c_str()))
        return;

    data_view_auth_3d_imgui_auth_3d_key(&c->curve, 0, false);
    igTreePop();
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
        igPushStyleColor_U32(ImGuiCol_Text, 0xFF888888);
    if (!igTreeNodeEx_Ptr(d, tree_node_flags, "DOF"))
        if (!enable)
            igPopStyleColor(1);
        return;

    auth_3d_model_transform* mt = &d->model_transform;

    data_view_auth_3d_imgui_auth_3d_vec3(&mt->translation, "Position");

    igText("Focus Range");
    data_view_auth_3d_imgui_auth_3d_key(&mt->scale.y, 0, true);

    igText("Fuzzing Range");
    data_view_auth_3d_imgui_auth_3d_key(&mt->rotation.y, 0, true);

    igText("Ratio");
    data_view_auth_3d_imgui_auth_3d_key(&mt->rotation.z, 0, true);
    igTreePop();

    if (!enable)
        igPopStyleColor(1);
}

static void data_view_auth_3d_imgui_auth_3d_fog(auth_3d_fog* f) {
    if (!f || !f->flags)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!igTreeNodeEx_Ptr(f, tree_node_flags, auth_3d_fog_name[f->id]))
        return;

    if (f->flags & AUTH_3D_FOG_COLOR)
        data_view_auth_3d_imgui_auth_3d_rgba(&f->color, "Color");

    if (f->flags & AUTH_3D_FOG_DENSITY) {
        igText("Density");
        data_view_auth_3d_imgui_auth_3d_key(&f->density, 0, true);
    }

    if (f->flags & AUTH_3D_FOG_END) {
        igText("End");
        data_view_auth_3d_imgui_auth_3d_key(&f->end, 0, true);
    }

    if (f->flags & AUTH_3D_FOG_START) {
        igText("Start");
        data_view_auth_3d_imgui_auth_3d_key(&f->start, 0, true);
    }
    igTreePop();
}

static void data_view_auth_3d_imgui_auth_3d_light(auth_3d_light* l) {
    if (!l || !l->flags)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!igTreeNodeEx_Ptr(l, tree_node_flags, auth_3d_light_name[l->id]))
        return;
    
    if (l->flags & AUTH_3D_LIGHT_AMBIENT)
        data_view_auth_3d_imgui_auth_3d_rgba(&l->ambient, "Ambient");

    if (l->flags & AUTH_3D_LIGHT_CONE_ANGLE) {
        igText("Cone Angle");
        data_view_auth_3d_imgui_auth_3d_key(&l->cone_angle, 0, true);
    }

    if (l->flags & AUTH_3D_LIGHT_CONSTANT) {
        igText("Constant");
        data_view_auth_3d_imgui_auth_3d_key(&l->constant, 0, true);
    }

    if (l->flags & AUTH_3D_LIGHT_DIFFUSE)
        data_view_auth_3d_imgui_auth_3d_rgba(&l->diffuse, "Diffuse");

    if (l->flags & AUTH_3D_LIGHT_DROP_OFF) {
        igText("Drop Off");
        data_view_auth_3d_imgui_auth_3d_key(&l->drop_off, 0, true);
    }

    if (l->flags & AUTH_3D_LIGHT_FAR) {
        igText("Far");
        data_view_auth_3d_imgui_auth_3d_key(&l->_far, 0, true);
    }

    if (l->flags & AUTH_3D_LIGHT_INTENSITY) {
        igText("Intensity");
        data_view_auth_3d_imgui_auth_3d_key(&l->intensity, 0, true);
    }

    if (l->flags & AUTH_3D_LIGHT_LINEAR) {
        igText("Linear");
        data_view_auth_3d_imgui_auth_3d_key(&l->linear, 0, true);
    }

    if (l->flags & AUTH_3D_LIGHT_POSITION)
        data_view_auth_3d_imgui_auth_3d_vec3(&l->position.translation, "Position");

    if (l->flags & AUTH_3D_LIGHT_QUADRATIC) {
        igText("Quadratic");
        data_view_auth_3d_imgui_auth_3d_key(&l->quadratic, 0, true);
    }

    if (l->flags & AUTH_3D_LIGHT_SPECULAR)
        data_view_auth_3d_imgui_auth_3d_rgba(&l->specular, "Specular");

    if (l->flags & AUTH_3D_LIGHT_SPOT_DIRECTION)
        data_view_auth_3d_imgui_auth_3d_vec3(&l->spot_direction.translation, "Spot Direction");

    if (l->flags & AUTH_3D_LIGHT_TONE_CURVE)
        data_view_auth_3d_imgui_auth_3d_rgba(&l->tone_curve, "Tone Curve");
    igTreePop();
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
        igPushStyleColor_U32(ImGuiCol_Text, 0xFF888888);

    if (!igTreeNodeEx_Ptr(moh, tree_node_flags, moh->name.c_str())) {
        if (!visible)
            igPopStyleColor(1);
        return;
    }

    if (moh->instance.size() > 0
        && igTreeNodeEx_Str("Instance", tree_node_flags)) {
        for (auth_3d_object_instance& i : moh->instance)
            data_view_auth_3d_imgui_auth_3d_object_instance(&i);
        igTreePop();
    }

    data_view_auth_3d_imgui_auth_3d_object_model_transform(&moh->model_transform);

    if (moh->node.size() > 0
        && igTreeNodeEx_Str("Node", tree_node_flags)) {
        for (auth_3d_object_node& i : moh->node)
            data_view_auth_3d_imgui_auth_3d_object_node(&i);
        igTreePop();
    }
    igTreePop();

    if (!visible)
        igPopStyleColor(1);
}

static void data_view_auth_3d_imgui_auth_3d_material_list(auth_3d_material_list* ml) {
    if (!ml || !ml->flags)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!igTreeNodeEx_Ptr(ml, tree_node_flags, ml->name.c_str()))
        return;

    if (ml->flags & AUTH_3D_MATERIAL_LIST_BLEND_COLOR)
        data_view_auth_3d_imgui_auth_3d_rgba(&ml->blend_color, "Blend Color");

    if (ml->flags & AUTH_3D_MATERIAL_LIST_GLOW_INTENSITY) {
        igText("Glow Intensity");
        data_view_auth_3d_imgui_auth_3d_key(&ml->glow_intensity, 0, false);
    }

    if (ml->flags & AUTH_3D_MATERIAL_LIST_INCANDESCENCE)
        data_view_auth_3d_imgui_auth_3d_rgba(&ml->incandescence, "Incandescence");
    igTreePop();
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
        igPushStyleColor_U32(ImGuiCol_Text, 0xFF888888);

    tree_node_flags = tree_node_base_flags;

    if (!igTreeNodeEx_Ptr(o, tree_node_flags, name)) {
        if (!visible)
            igPopStyleColor(1);
        return;
    }

    data_view_auth_3d_imgui_auth_3d_model_transform(&o->model_transform);

    data_view_auth_3d_imgui_auth_3d_object_curve(&o->morph, "Morph");
    data_view_auth_3d_imgui_auth_3d_object_curve(&o->pattern, "Pattern");

    tree_node_flags = tree_node_base_flags;

    if (o->texture_pattern.size() > 0
        && igTreeNodeEx_Str("Texture Pattern", tree_node_flags)) {
        for (auth_3d_object_texture_pattern& i : o->texture_pattern)
            data_view_auth_3d_imgui_auth_3d_object_texture_pattern(&i);
        igTreePop();
    }

    tree_node_flags = tree_node_base_flags;

    if (o->texture_transform.size() > 0
        && igTreeNodeEx_Str("Texture Transform", tree_node_flags)) {
        for (auth_3d_object_texture_transform& i : o->texture_transform)
            data_view_auth_3d_imgui_auth_3d_object_texture_transform(&i);
        igTreePop();
    }


    tree_node_flags = tree_node_base_flags;
    tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;

    if (o->children_object.size() > 0
        && igTreeNodeEx_Str("Children Object", tree_node_flags)) {
        for (auth_3d_object*& i : o->children_object)
            data_view_auth_3d_imgui_auth_3d_object(i);
        igTreePop();
    }

    tree_node_flags = tree_node_base_flags;
    tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;

    if (o->children_object_hrc.size() > 0
        && igTreeNodeEx_Str("Children Object HRC", tree_node_flags)) {
        for (auth_3d_object_hrc*& i : o->children_object_hrc)
            data_view_auth_3d_imgui_auth_3d_object_hrc(i);
        igTreePop();
    }
    igTreePop();

    if (!visible)
        igPopStyleColor(1);
}

static void data_view_auth_3d_imgui_auth_3d_object_curve(auth_3d_object_curve* oc, const char* format) {
    if (!oc || !oc->curve || !format)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!igTreeNodeEx_Ptr(oc, tree_node_flags, format))
        return;

    data_view_auth_3d_imgui_auth_3d_curve(oc->curve);
    igTreePop();
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
        igPushStyleColor_U32(ImGuiCol_Text, 0xFF888888);

    tree_node_flags = tree_node_base_flags;

    if (!igTreeNodeEx_Ptr(oh, tree_node_flags, name)) {
        if (!visible)
            igPopStyleColor(1);
        return;
    }

    tree_node_flags = tree_node_base_flags;

    if (oh->node.size() > 0
        && igTreeNodeEx_Str("Node", tree_node_flags)) {
        for (auth_3d_object_node& i : oh->node)
            data_view_auth_3d_imgui_auth_3d_object_node(&i);
        igTreePop();
    }

    tree_node_flags = tree_node_base_flags;
    tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;

    if (oh->children_object.size() > 0
        && igTreeNodeEx_Str("Children Object", tree_node_flags)) {
        for (auth_3d_object*& i : oh->children_object)
            data_view_auth_3d_imgui_auth_3d_object(i);
        igTreePop();
    }

    tree_node_flags = tree_node_base_flags;
    tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;

    if (oh->children_object_hrc.size() > 0
        && igTreeNodeEx_Str("Children Object HRC", tree_node_flags)) {
        for (auth_3d_object_hrc*& i : oh->children_object_hrc)
            data_view_auth_3d_imgui_auth_3d_object_hrc(i);
        igTreePop();
    }
    igTreePop();

    if (!visible)
        igPopStyleColor(1);
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
        igPushStyleColor_U32(ImGuiCol_Text, 0xFF888888);

    if (!igTreeNodeEx_Ptr(oi, tree_node_flags, oi->name.c_str())) {
        if (!visible)
            igPopStyleColor(1);
        return;
    }

    data_view_auth_3d_imgui_auth_3d_object_model_transform(&oi->model_transform);

    if (oi->shadow)
        igText("Shadow: True");
    
    igText("UID Name: %s", oi->uid_name.c_str());
    igTreePop();

    if (!visible)
        igPopStyleColor(1);
}

static void data_view_auth_3d_imgui_auth_3d_object_model_transform(auth_3d_object_model_transform* omt) {
    if (!omt)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!igTreeNodeEx_Ptr(omt, tree_node_flags, "Model Transform"))
        return;

    igText("Frame: %5f", omt->frame);

    data_view_auth_3d_imgui_auth_3d_vec3(&omt->translation, "Translation");
    data_view_auth_3d_imgui_auth_3d_vec3(&omt->rotation, "Rotation");
    data_view_auth_3d_imgui_auth_3d_vec3(&omt->scale, "Scale");

    igText("Visibility");
    data_view_auth_3d_imgui_auth_3d_key(&omt->visibility, 0, true);
    igTreePop();
}

static void data_view_auth_3d_imgui_auth_3d_object_node(auth_3d_object_node* on) {
    if (!on)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!igTreeNodeEx_Ptr(on, tree_node_flags, on->name.c_str()))
        return;

    data_view_auth_3d_imgui_auth_3d_object_model_transform(&on->model_transform);

    if (on->flags & AUTH_3D_OBJECT_NODE_JOINT_ORIENT) {
        igText("Joint Orient ");
        igText("   X: %#.6g", on->joint_orient.x);
        igText("   Y: %#.6g", on->joint_orient.y);
        igText("   Z: %#.6g", on->joint_orient.z);
    }

    igText("Parent: %d", on->parent);
    igTreePop();
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

    if (!igTreeNodeEx_Ptr(ott, tree_node_flags, ott->name.c_str()))
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
        data_view_auth_3d_imgui_auth_3d_key(&ott->translate_frame_v, "Translate Frame V:", false);;
    igTreePop();
}

static void data_view_auth_3d_imgui_auth_3d_point(auth_3d_point* p) {
    if (!p)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!igTreeNodeEx_Ptr(p, tree_node_flags, p->name.c_str()))
        return;

    data_view_auth_3d_imgui_auth_3d_model_transform(&p->model_transform);
    igTreePop();
}

static void data_view_auth_3d_imgui_auth_3d_post_process(auth_3d_post_process* pp) {
    if (!pp || !pp->flags)
        return;

    ImGuiTreeNodeFlags tree_node_flags = 0;
    tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!igTreeNodeEx_Ptr(pp, tree_node_flags, "Post Process"))
        return;

    if (pp->flags & AUTH_3D_POST_PROCESS_INTENSITY)
        data_view_auth_3d_imgui_auth_3d_rgba(&pp->intensity, "intensity");

    if (pp->flags & AUTH_3D_POST_PROCESS_LENS_FLARE) {
        igText("Lens Flare");
        data_view_auth_3d_imgui_auth_3d_key(&pp->lens_flare, 0, false);
    }

    if (pp->flags & AUTH_3D_POST_PROCESS_LENS_GHOST) {
        igText("Lens Gghost");
        data_view_auth_3d_imgui_auth_3d_key(&pp->lens_ghost, 0, false);
    }

    if (pp->flags & AUTH_3D_POST_PROCESS_LENS_SHAFT) {
        igText("Lens Shaft");
        data_view_auth_3d_imgui_auth_3d_key(&pp->lens_shaft, 0, false);
    }

    if (pp->flags & AUTH_3D_POST_PROCESS_RADIUS)
        data_view_auth_3d_imgui_auth_3d_rgba(&pp->radius, "Radius");

    if (pp->flags & AUTH_3D_POST_PROCESS_SCENE_FADE)
        data_view_auth_3d_imgui_auth_3d_rgba(&pp->scene_fade, "Scene Fade");
    igTreePop();
}
