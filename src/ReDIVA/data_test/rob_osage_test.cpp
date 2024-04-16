/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "rob_osage_test.hpp"
#include "../../CRE/Glitter/glitter.hpp"
#include "../../CRE/data.hpp"
#include "../../CRE/render_context.hpp"
#include "../../CRE/resolution_mode.hpp"
#include "../dw.hpp"

class RobOsageTestDw : public dw::Shell {
public:
    struct Rob {
        dw::ListBox* list_box;

        dw::Button* save_button;
        dw::Button* load_button;
        dw::Button* default_button;

        dw::ListBox* object_list_box;

        void Init(dw::Composite* parent);

        static void CharaCallback(dw::Widget* data);
        static void DefaultCallback(dw::Widget* data);
        static void LoadCallback(dw::Widget* data);
        static void ObjectCallback(dw::Widget* data);
        static void SaveCallback(dw::Widget* data);
    };

    struct Flags {
        dw::Button* line_button;
        dw::Button* lock_button;
        dw::Button* no_pause_button;
        dw::Button* coli_button;
        dw::Button* name_button;
        dw::Button* init_button;

        void Init(dw::Composite* parent);

        static void ColiCallback(dw::Widget* data);
        static void InitCallback(dw::Widget* data);
        static void LineCallback(dw::Widget* data);
        static void LockCallback(dw::Widget* data);
        static void NameCallback(dw::Widget* data);
        static void NoPauseCallback(dw::Widget* data);
    };

    struct Root {
        class Force : public dw::SelectionAdapter {
        public:
            dw::Composite* comp;
            dw::Button* button;
            dw::Slider* slider;

            Force(dw::Composite* parent);
            virtual ~Force() override;

            virtual void Callback(dw::SelectionListener::CallbackData* data) override;

            void SetValue(float_t value);
            void Update(bool value);
        };
        
        class Gain : public dw::SelectionAdapter {
        public:
            dw::Composite* comp;
            dw::Button* button;
            dw::Slider* slider;

            Gain(dw::Composite* parent);
            virtual ~Gain() override;

            virtual void Callback(dw::SelectionListener::CallbackData* data) override;

            void SetValue(float_t value);
            void Update(bool value);
        };
        
        class AirRes : public dw::SelectionAdapter {
        public:
            dw::Composite* comp;
            dw::Button* button;
            dw::Slider* slider;

            AirRes(dw::Composite* parent);
            virtual ~AirRes() override;

            virtual void Callback(dw::SelectionListener::CallbackData* data) override;

            void SetValue(float_t value);
            void Update(bool value);
        };
        
        class RootYRot : public dw::SelectionAdapter {
        public:
            dw::Composite* comp;
            dw::Button* button;
            dw::Slider* slider;

            RootYRot(dw::Composite* parent);
            virtual ~RootYRot() override;

            virtual void Callback(dw::SelectionListener::CallbackData* data) override;

            void SetValue(float_t value);
            void Update(bool value);
        };
        
        class RootZRot : public dw::SelectionAdapter {
        public:
            dw::Composite* comp;
            dw::Button* button;
            dw::Slider* slider;

            RootZRot(dw::Composite* parent);
            virtual ~RootZRot() override;

            virtual void Callback(dw::SelectionListener::CallbackData* data) override;

            void SetValue(float_t value);
            void Update(bool value);
        };
        
        class Fric : public dw::SelectionAdapter {
        public:
            dw::Composite* comp;
            dw::Button* button;
            dw::Slider* slider;

            Fric(dw::Composite* parent);
            virtual ~Fric() override;

            virtual void Callback(dw::SelectionListener::CallbackData* data) override;

            void SetValue(float_t value);
            void Update(bool value);
        };
        
        class WindAfc : public dw::SelectionAdapter {
        public:
            dw::Composite* comp;
            dw::Button* button;
            dw::Slider* slider;

            WindAfc(dw::Composite* parent);
            virtual ~WindAfc() override;

            virtual void Callback(dw::SelectionListener::CallbackData* data) override;

            void SetValue(float_t value);
            void Update(bool value);
        };
        
        class ColiType : public dw::SelectionAdapter {
        public:
            dw::Composite* comp;
            dw::Button* button;
            dw::ListBox* list_box;

            ColiType(dw::Composite* parent);
            virtual ~ColiType() override;

            virtual void Callback(dw::SelectionListener::CallbackData* data) override;

            void SetItemIndex(size_t value);
            void Update(bool value);

            static void ColiTypeCallback(dw::Widget* data);
        };

        class InitYRot : public dw::SelectionAdapter {
        public:
            dw::Composite* comp;
            dw::Button* button;
            dw::Slider* slider;

            InitYRot(dw::Composite* parent);
            virtual ~InitYRot() override;

            virtual void Callback(dw::SelectionListener::CallbackData* data) override;

            void SetValue(float_t value);
            void Update(bool value);
        };

        class InitZRot : public dw::SelectionAdapter {
        public:
            dw::Composite* comp;
            dw::Button* button;
            dw::Slider* slider;

            InitZRot(dw::Composite* parent);
            virtual ~InitZRot() override;

            virtual void Callback(dw::SelectionListener::CallbackData* data) override;

            void SetValue(float_t value);
            void Update(bool value);
        };

        dw::ListBox* list_box;

        dw::Button* reset_button;
        dw::Button* copy_button;
        dw::Button* paste_button;

        Force* force;
        Gain* gain;
        AirRes* air_res;
        RootYRot* root_y_rot;
        RootZRot* root_z_rot;
        Fric* fric;
        WindAfc* wind_afc;
        ColiType* coli_type;
        InitYRot* init_y_rot;
        InitZRot* init_z_rot;

        void Init(dw::Composite* parent);
        void Update();

        static void CopyCallback(dw::Widget* data);
        static void OsageCallback(dw::Widget* data);
        static void PasteCallback(dw::Widget* data);
        static void ResetCallback(dw::Widget* data);
    };

    struct Node {
        class CollisionRadius : public dw::SelectionAdapter {
        public:
            dw::Composite* comp;
            dw::Button* button;
            dw::Slider* slider;

            CollisionRadius(dw::Composite* parent);
            virtual ~CollisionRadius() override;

            virtual void Callback(dw::SelectionListener::CallbackData* data) override;

            void SetValue(float_t value);
            void Update(bool value);
        };

        class Hinge : public dw::SelectionAdapter {
        public:
            dw::Composite* comp;
            dw::Button* button;
            dw::Slider* y_min_slider;
            dw::Slider* y_max_slider;
            dw::Slider* z_min_slider;
            dw::Slider* z_max_slider;

            Hinge(dw::Composite* parent);
            virtual ~Hinge() override;

            virtual void Callback(dw::SelectionListener::CallbackData* data) override;

            void SetValue(const RobOsageTest::Node::Hinge& value);
            void Update(bool value);
        };

        class InertialCancel : public dw::SelectionAdapter {
        public:
            dw::Composite* comp;
            dw::Button* button;
            dw::Slider* slider;

            InertialCancel(dw::Composite* parent);
            virtual ~InertialCancel() override;

            virtual void Callback(dw::SelectionListener::CallbackData* data) override;

            void SetValue(float_t value);
            void Update(bool value);
        };

        class Weight : public dw::SelectionAdapter {
        public:
            dw::Composite* comp;
            dw::Button* button;
            dw::Slider* slider;

            Weight(dw::Composite* parent);
            virtual ~Weight() override;

            virtual void Callback(dw::SelectionListener::CallbackData* data) override;

            void SetValue(float_t value);
            void Update(bool value);
        };

        dw::Group* group;

        CollisionRadius* coli_r;
        Hinge* hinge;
        InertialCancel* inertial_cancel;
        Weight* weight;

        void Init(dw::Composite* parent);
        void Update();
    };

    struct BetweenOsageCollision {
        dw::Button* button;

        void Init(dw::Composite* parent);
    };

    struct Collision {
        dw::Group* group;

        dw::Button* reset_button;
        dw::Button* copy_button;
        dw::Button* paste_button;

        dw::Composite* element_comp;

        void Init(dw::Composite* parent);
        void Update();

        static void CopyCallback(dw::Widget* data);
        static void ElementCallback(dw::Widget* data);
        static void PasteCallback(dw::Widget* data);
        static void ResetCallback(dw::Widget* data);
    };

    struct ColliElement {
        dw::Group* group;

        dw::Button* reset_button;
        dw::Button* copy_button;
        dw::Button* paste_button;
        dw::Button* flip_button;

        dw::ListBox* type_list_box;
        dw::Slider* radius_slider;
        dw::ListBox* bone0_list_box;
        dw::Slider* bone0_x_slider;
        dw::Slider* bone0_y_slider;
        dw::Slider* bone0_z_slider;
        dw::ListBox* bone1_list_box;
        dw::Slider* bone1_x_slider;
        dw::Slider* bone1_y_slider;
        dw::Slider* bone1_z_slider;

        void Init(dw::Composite* parent);
        void Update();

        static void BoneCallback(dw::Widget* data);
        static void BonePosXCallback(dw::Widget* data);
        static void BonePosYCallback(dw::Widget* data);
        static void BonePosZCallback(dw::Widget* data);
        static void CopyCallback(dw::Widget* data);
        static void FlipCallback(dw::Widget* data);
        static void PasteCallback(dw::Widget* data);
        static void RadiusCallback(dw::Widget* data);
        static void ResetCallback(dw::Widget* data);
        static void TypeCallback(dw::Widget* data);
    };

    Rob rob;
    Flags flags;
    Root root;
    Node node;
    BetweenOsageCollision boc;
    Collision collision;
    ColliElement colli_element;

    RobOsageTestDw();
    virtual ~RobOsageTestDw() override;

    virtual void Hide() override;
};

const char* collision_type_name_list[] = {
    "END",
    "BALL",
    "CYLINDER",
    "PLANE",
    "ELLIPSE",
    "AABB",
};

RobOsageTest* rob_osage_test;
RobOsageTestDw* rob_osage_test_dw;

extern render_context* rctx_ptr;

RobOsageTest::RobOsageTest() : load(), save(), coli(), line(),
osage_index(), collision_index(), collision_update() {
    chara_id = -1;
    load_chara_id = 0;
    item_id = ITEM_NONE;
    osage_index = -1;
    collision_index = -1;
}

RobOsageTest::~RobOsageTest() {

}

bool RobOsageTest::init() {
    if (!rob_osage_test_dw) {
        rob_osage_test_dw = new RobOsageTestDw;
        rob_osage_test_dw->sub_1402F38B0();
    }
    else
        rob_osage_test_dw->Disp();
    return true;
}

bool RobOsageTest::ctrl() {
    if (load) {
        load = false;

        chara_id = load_chara_id;

        objects.clear();
        rob_osage_test_dw->rob.object_list_box->ClearItems();
        rob_osage_test_dw->rob.object_list_box->SetItemIndex(-1);
        rob_osage_test_dw->root.list_box->ClearItems();
        rob_osage_test_dw->root.list_box->SetItemIndex(-1);

        item_id = ITEM_NONE;
        obj_info = object_info();
        osage_index = -1;
        collision_index = -1;

        rob_chara_item_equip* rob_itm_equip = rob_chara_array_get_item_equip(chara_id);
        if (!rob_itm_equip)
            return 0;

        for (int32_t i = 0; i < ITEM_MAX; i++) {
            rob_chara_item_equip_object* itm_eq_obj = rob_itm_equip->get_item_equip_object((::item_id)i);
            obj* obj = object_storage_get_obj(itm_eq_obj->obj_info);
            if (obj && (itm_eq_obj->osage_blocks.size() || itm_eq_obj->cloth_blocks.size())) {
                objects.push_back((::item_id)i, itm_eq_obj->obj_info);
                rob_osage_test_dw->rob.object_list_box->AddItem(obj->name);
            }
        }
    }

    if (save)
        save = false;

    return false;
}

bool RobOsageTest::dest() {
    rob_osage_test_dw->SetDisp();
    return true;
}

void RobOsageTest::disp() {
    rob_chara* rob_chr = rob_chara_array_get(chara_id);
    if (rob_chr && !pv_osage_manager_array_get_disp(chara_id)
        && rob_chr->is_visible() && !(rob_chr->data.field_3 & 0x80)) {
        disp_coli();
        disp_line();
    }
}

void RobOsageTest::basic() {
    if (collision_update) {
        collision_update = false;

        rob_osage_test_dw->collision.Update();
        rob_osage_test_dw->colli_element.Update();
        rob_osage_test_dw->UpdateLayout();
    }

    skin_param* skp = get_skin_param();
    if (skp) {
        skp->force = root.force;
        skp->force_gain = root.gain;
        skp->air_res = root.air_res;
        skp->rot.y = root.root_y_rot;
        skp->rot.z = root.root_z_rot;
        skp->friction = root.fric;
        skp->wind_afc = root.wind_afc;
        skp->coli_type = root.coli_type;
        skp->init_rot.y = root.init_y_rot;
        skp->init_rot.z = root.init_z_rot;
    }
}

void RobOsageTest::disp_coli() {
    if (!coli)
        return;

    rob_chara_item_equip_object* itm_eq_obj = get_item_equip_object();
    if (!itm_eq_obj)
        return;

    std::vector<SkinParam::CollisionParam>* cls_list = 0;
    mat4* transform = 0;

    ExOsageBlock* osg = get_osage_block(itm_eq_obj);
    ExClothBlock* cls = 0;
    if (osg) {
        cls_list = get_cls_list(osg);
        transform = osg->mats;
    }
    else {
        cls = get_cloth_block(itm_eq_obj);
        if (cls) {
            cls_list = get_cls_list(cls);
            transform = cls->mats;
        }
    }

    if (!cls_list || !transform)
        return;

    static const color4u8 selected_color = 0xCF00EF00;
    static const color4u8 cls_node_color = 0xCFEFEF00;
    static const color4u8 osg_node_color = 0xCFEFEF00;
    static const color4u8 default_color = 0xCFFFFFFF;

    SkinParam::CollisionParam* cls_param = get_cls_param(cls_list);
    for (const SkinParam::CollisionParam& i : *cls_list)
        switch (i.type) {
        case SkinParam::CollisionTypeBall: {
            mdl::EtcObj etc(mdl::ETC_OBJ_SPHERE);
            etc.color = cls_param && &i == cls_param ? selected_color : default_color;
            etc.constant = true;

            etc.data.sphere.radius = i.radius;
            etc.data.sphere.slices = 16;
            etc.data.sphere.stacks = 16;
            etc.data.sphere.wire = false;

            vec3 pos;
            mat4_transform_point(&transform[i.node_idx[0]], &i.pos[0], &pos);

            mat4 mat;
            mat4_translate(&pos, &mat);
            rctx_ptr->disp_manager->entry_obj_etc(&mat, &etc);
        } break;
        case SkinParam::CollisionTypeCapsule: {
            mdl::EtcObj etc(mdl::ETC_OBJ_CAPSULE);
            etc.color = cls_param && &i == cls_param ? selected_color : default_color;
            etc.constant = true;

            etc.data.capsule.radius = i.radius;
            etc.data.capsule.slices = 16;
            etc.data.capsule.stacks = 16;
            etc.data.capsule.wire = false;
            mat4_transform_point(&transform[i.node_idx[0]], &i.pos[0], &etc.data.capsule.pos[0]);
            mat4_transform_point(&transform[i.node_idx[1]], &i.pos[1], &etc.data.capsule.pos[1]);
            rctx_ptr->disp_manager->entry_obj_etc(&mat4_identity, &etc);
        } break;
        case SkinParam::CollisionTypePlane: {
            mdl::EtcObj etc(mdl::ETC_OBJ_PLANE);
            etc.color = cls_param && &i == cls_param ? selected_color : default_color;
            etc.constant = true;

            etc.data.plane.w = 2;
            etc.data.plane.h = 2;

            vec3 pos;
            mat4_transform_point(&transform[i.node_idx[0]], &i.pos[0], &pos);

            vec3 dir = vec3::normalize(i.pos[1]);
            vec3 up = { 0.0f, 1.0f, 0.0f };
            vec3 axis;
            float_t angle;
            Glitter::axis_angle_from_vectors(&axis, &angle, &up, &dir);

            mat4 mat_rot = mat4_identity;
            mat4_mul_rotation(&mat_rot, &axis, angle, &mat_rot);

            mat4 mat;
            mat4_translate(&pos, &mat);
            mat4_mul(&mat_rot, &mat, &mat);
            mat4_scale_rot(&mat, i.radius, &mat);
            rctx_ptr->disp_manager->entry_obj_etc(&mat, &etc);
        } break;
        case SkinParam::CollisionTypeEllipse: {
            mdl::EtcObj etc(mdl::ETC_OBJ_SPHERE);
            etc.color = cls_param && &i == cls_param ? selected_color : default_color;
            etc.constant = true;

            etc.data.sphere.radius = 1.0f;
            etc.data.sphere.slices = 16;
            etc.data.sphere.stacks = 16;
            etc.data.sphere.wire = false;

            vec3 pos[2];
            mat4_transform_point(&transform[i.node_idx[0]], &i.pos[0], &pos[0]);
            mat4_transform_point(&transform[i.node_idx[1]], &i.pos[1], &pos[1]);

            vec3 origin = (pos[0] + pos[1]) * 0.5f;
            mat4 mat;
            mat4_translate(&origin, &mat);

            vec3 dir = vec3::normalize(pos[1] - pos[0]);
            vec3 up = { 0.0f, 1.0f, 0.0f };
            vec3 axis;
            float_t angle;
            Glitter::axis_angle_from_vectors(&axis, &angle, &up, &dir);

            mat4 m = mat4_identity;
            mat4_mul_rotation(&m, &axis, angle, &m);
            mat4_mul(&m, &mat, &mat);

            const float_t length = vec3::length((pos[0] - pos[1]) * 0.5f);
            const float_t scale_y = sqrtf(i.radius * i.radius + length * length);

            mat4_scale_rot(&mat, i.radius, scale_y, i.radius, &mat);
            rctx_ptr->disp_manager->entry_obj_etc(&mat, &etc);

            etc.color = 0xFF000000;
            etc.data.sphere.wire = true;
            rctx_ptr->disp_manager->entry_obj_etc(&mat, &etc);
        } break;
        case SkinParam::CollisionTypeAABB: {
            mdl::EtcObj etc(mdl::ETC_OBJ_CUBE);
            etc.color = cls_param && &i == cls_param ? selected_color : default_color;
            etc.constant = true;

            vec3 pos;
            mat4_transform_point(&transform[i.node_idx[0]], &i.pos[0], &pos);

            etc.data.cube.size = i.radius;
            etc.data.cube.wire = false;

            mat4 mat;
            mat4_translate(&pos, &mat);
            rctx_ptr->disp_manager->entry_obj_etc(&mat, &etc);
        } break;
        }

    if (osg && osg->rob.nodes.size() > 1) {
        RobOsageNode* i_begin = osg->rob.nodes.data() + 1;
        RobOsageNode* i_end = osg->rob.nodes.data() + osg->rob.nodes.size();
        for (RobOsageNode* i = i_begin; i != i_end; i++) {
            mdl::EtcObj etc(mdl::ETC_OBJ_CAPSULE);
            etc.color = osg_node_color;
            etc.constant = true;

            etc.data.capsule.radius = i->data_ptr->skp_osg_node.coli_r;
            etc.data.capsule.slices = 16;
            etc.data.capsule.stacks = 16;
            etc.data.capsule.wire = false;
            etc.data.capsule.pos[0] = i[-1].trans;
            etc.data.capsule.pos[1] = i[ 0].trans;
            rctx_ptr->disp_manager->entry_obj_etc(&mat4_identity, &etc);
        }
    }

    if (cls && cls->rob.nodes.size() > 1) {
        CLOTHNode* i_begin = cls->rob.nodes.data();
        CLOTHNode* i_end = cls->rob.nodes.data() + cls->rob.nodes.size();
        for (CLOTHNode* i = i_begin; i != i_end; i++) {
            mdl::EtcObj etc(mdl::ETC_OBJ_SPHERE);
            etc.color = cls_node_color;
            etc.constant = true;

            etc.data.sphere.radius = max_def(cls->rob.skin_param_ptr->coli_r, 0.005f);
            etc.data.sphere.slices = 16;
            etc.data.sphere.stacks = 16;
            etc.data.sphere.wire = false;

            vec3 pos;
            mat4_transform_point(&itm_eq_obj->item_equip->mat, &i->trans, &pos);

            mat4 mat;
            mat4_translate(&pos, &mat);
            rctx_ptr->disp_manager->entry_obj_etc(&mat, &etc);
        }
    }
}

void RobOsageTest::disp_line() {
    if (!line)
        return;

}

inline ExClothBlock* RobOsageTest::get_cloth_block(rob_chara_item_equip_object* itm_eq_obj) const {
    if (itm_eq_obj && itm_eq_obj->obj_info == obj_info && osage_index >= itm_eq_obj->osage_blocks.size()
        && osage_index - itm_eq_obj->osage_blocks.size() < itm_eq_obj->cloth_blocks.size())
        return itm_eq_obj->cloth_blocks[osage_index - itm_eq_obj->osage_blocks.size()];
    return 0;
}

inline rob_chara_item_equip_object* RobOsageTest::get_item_equip_object() const {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return 0;

    rob_chara_item_equip* rob_itm_equip = rob_chara_array_get_item_equip(chara_id);
    if (rob_itm_equip && item_id >= ITEM_BODY && item_id < ITEM_MAX)
        return rob_itm_equip->get_item_equip_object(item_id);
    return 0;
}

inline ExOsageBlock* RobOsageTest::get_osage_block(rob_chara_item_equip_object* itm_eq_obj) const {
    if (itm_eq_obj && itm_eq_obj->obj_info == obj_info && osage_index < itm_eq_obj->osage_blocks.size())
        return itm_eq_obj->osage_blocks[osage_index];
    return 0;
}

inline std::vector<SkinParam::CollisionParam>* RobOsageTest::get_cls_list(ExClothBlock* cls) const {
    if (cls)
        return &cls->rob.skin_param_ptr->coli;
    return 0;
}

inline std::vector<SkinParam::CollisionParam>* RobOsageTest::get_cls_list(ExOsageBlock* osg) const {
    if (osg)
        return &osg->rob.skin_param_ptr->coli;
    return 0;
}

inline SkinParam::CollisionParam* RobOsageTest::get_cls_param(
    std::vector<SkinParam::CollisionParam>* cls_list) const {
    if (cls_list && collision_index < cls_list->size())
        return &cls_list->data()[collision_index];
    return 0;
}

void RobOsageTest::set_root(skin_param* skp) {
    if (!skp)
        return;

    root.force = skp->force;
    root.gain = skp->force_gain;
    root.air_res = skp->air_res;
    root.root_y_rot = skp->rot.y;
    root.root_z_rot = skp->rot.z;
    root.fric = skp->friction;
    root.wind_afc = skp->wind_afc;
    root.coli_type = skp->coli_type;
    root.init_y_rot = skp->init_rot.y;
    root.init_z_rot = skp->init_rot.z;
}

void rob_osage_test_init() {
    if (!rob_osage_test)
        rob_osage_test = new RobOsageTest;
}

void rob_osage_test_free() {
    if (rob_osage_test) {
        delete rob_osage_test;
        rob_osage_test = 0;
    }
}

void RobOsageTestDw::Rob::Init(dw::Composite* parent) {
    dw::Composite* comp = new dw::Composite(parent);

    dw::Composite* ribbon_comp = new dw::Composite(comp);
    ribbon_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    list_box = new dw::ListBox(ribbon_comp);
    list_box->AddItem("1P");
    list_box->AddItem("2P");
    list_box->AddItem("3P");
    list_box->AddItem("4P");
    list_box->AddItem("5P");
    list_box->AddItem("6P");
    list_box->SetItemIndex(0);
    list_box->AddSelectionListener(new dw::SelectionListenerOnHook(RobOsageTestDw::Rob::CharaCallback));

    save_button = new dw::Button(ribbon_comp, dw::FLAG_8);
    save_button->SetText("  SAVE  ");
    save_button->callback = RobOsageTestDw::Rob::SaveCallback;

    load_button = new dw::Button(ribbon_comp, dw::FLAG_8);
    load_button->SetText("  LOAD  ");
    load_button->callback = RobOsageTestDw::Rob::LoadCallback;

    default_button = new dw::Button(ribbon_comp, dw::FLAG_8);
    default_button->SetText("DEFAULT");
    default_button->callback = RobOsageTestDw::Rob::DefaultCallback;

    object_list_box = new dw::ListBox(comp);
    object_list_box->AddSelectionListener(new dw::SelectionListenerOnHook(
        RobOsageTestDw::Rob::ObjectCallback));
}

void RobOsageTestDw::Rob::CharaCallback(dw::Widget* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box)
        rob_osage_test->load_chara_id = (int32_t)list_box->list->selected_item;
}

void RobOsageTestDw::Rob::DefaultCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button) {
        rob_osage_test->chara_id = -1;
        rob_osage_test->load_chara_id = 0;
        rob_osage_test->item_id = ITEM_NONE;
        rob_osage_test->obj_info = object_info();
        rob_osage_test->osage_index = -1;
        rob_osage_test->collision_index = -1;
        rob_osage_test->collision_update = true;

        rob_osage_test->root = {};
        rob_osage_test->node = {};

        rob_osage_test_dw->root.Update();
        rob_osage_test_dw->node.Update();
        rob_osage_test_dw->colli_element.Update();
    }
}

void RobOsageTestDw::Rob::LoadCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button)
        rob_osage_test->load = true;
}

void RobOsageTestDw::Rob::ObjectCallback(dw::Widget* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box && list_box->list->selected_item < rob_osage_test->objects.size()) {
        auto elem = rob_osage_test->objects.data()[list_box->list->selected_item];
        rob_osage_test->item_id = elem.first;
        rob_osage_test->obj_info = elem.second;
        rob_osage_test->osage_index = -1;
        rob_osage_test->collision_index = -1;
        rob_osage_test->set_root();

        rob_osage_test_dw->root.list_box->ClearItems();

        rob_chara_item_equip_object* itm_eq_obj = rob_osage_test->get_item_equip_object();
        if (itm_eq_obj) {
            for (ExOsageBlock*& i : itm_eq_obj->osage_blocks)
                rob_osage_test_dw->root.list_box->AddItem(i->name);

            for (ExClothBlock*& i : itm_eq_obj->cloth_blocks)
                rob_osage_test_dw->root.list_box->AddItem(i->name);
        }
    }
}

void RobOsageTestDw::Rob::SaveCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button)
        rob_osage_test->save = true;
}

void RobOsageTestDw::Flags::Init(dw::Composite* parent) {
    dw::Composite* comp = new dw::Composite(parent);
    comp->SetLayout(new dw::GridLayout(3));

    line_button = new dw::Button(comp, dw::CHECKBOX);
    line_button->SetText("Line   ");
    line_button->SetValue(rob_osage_test->line);
    line_button->callback = RobOsageTestDw::Flags::LineCallback;

    lock_button = new dw::Button(comp, dw::CHECKBOX);
    lock_button->SetText("Lock   ");
    lock_button->callback = RobOsageTestDw::Flags::LockCallback;

    no_pause_button = new dw::Button(comp, dw::CHECKBOX);
    no_pause_button->SetText("NoPause");
    no_pause_button->callback = RobOsageTestDw::Flags::NoPauseCallback;

    coli_button = new dw::Button(comp, dw::CHECKBOX);
    coli_button->SetText("Coli   ");
    coli_button->SetValue(rob_osage_test->coli);
    coli_button->callback = RobOsageTestDw::Flags::ColiCallback;

    name_button = new dw::Button(comp, dw::CHECKBOX);
    name_button->SetText("Name   ");
    name_button->callback = RobOsageTestDw::Flags::NameCallback;

    init_button = new dw::Button(comp, dw::CHECKBOX);
    init_button->SetText("Init   ");
    init_button->callback = RobOsageTestDw::Flags::InitCallback;
}

void RobOsageTestDw::Flags::ColiCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button)
        rob_osage_test->coli = button->value;
}

void RobOsageTestDw::Flags::InitCallback(dw::Widget* data) {

}

void RobOsageTestDw::Flags::LineCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button)
        rob_osage_test->line = button->value;
}

void RobOsageTestDw::Flags::LockCallback(dw::Widget* data) {

}

void RobOsageTestDw::Flags::NameCallback(dw::Widget* data) {

}

void RobOsageTestDw::Flags::NoPauseCallback(dw::Widget* data) {

}

RobOsageTestDw::Root::Force::Force(dw::Composite* parent) : slider() {
    comp = new dw::Composite(parent);
    comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    button = new dw::Button(comp, dw::CHECKBOX);
    button->SetText("force    ");
    button->SetValue(true);
    button->AddSelectionListener(this);

    Update(true);
}

RobOsageTestDw::Root::Force::~Force() {

}

void RobOsageTestDw::Root::Force::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
    if (button) {
        Update(button->value);
        rob_osage_test_dw->UpdateLayout();
    }
}

void RobOsageTestDw::Root::Force::SetValue(float_t value) {
    if (slider)
        slider->SetValue(value);
}

void RobOsageTestDw::Root::Force::Update(bool value) {
    if (value) {
        if (slider)
            return;

        slider = dw::Slider::Create(comp);
        slider->SetText("");
        slider->format = "% 2.3f";
        slider->SetParams(rob_osage_test->root.force, 0.0f, 0.1f, 0.01f, 0.001f, 0.01f);
    }
    else {
        if (!slider)
            return;

        comp->controls.erase(comp->controls.begin() + comp->GetControlIndex(slider));
        slider->Reset();
        slider = 0;
    }
}

RobOsageTestDw::Root::Gain::Gain(dw::Composite* parent) : slider() {
    comp = new dw::Composite(parent);
    comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    button = new dw::Button(comp, dw::CHECKBOX);
    button->SetText("gain     ");
    button->SetValue(true);
    button->AddSelectionListener(this);

    Update(true);
}

RobOsageTestDw::Root::Gain::~Gain() {

}

void RobOsageTestDw::Root::Gain::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
    if (button) {
        Update(button->value);
        rob_osage_test_dw->UpdateLayout();
    }
}

void RobOsageTestDw::Root::Gain::SetValue(float_t value) {
    if (slider)
        slider->SetValue(value);
}

void RobOsageTestDw::Root::Gain::Update(bool value) {
    if (value) {
        if (slider)
            return;

        slider = dw::Slider::Create(comp);
        slider->SetText("");
        slider->format = "% 2.3f";
        slider->SetParams(rob_osage_test->root.gain, 0.0f, 1.0f, 0.1f, 0.01f, 0.1f);
    }
    else {
        if (!slider)
            return;

        comp->controls.erase(comp->controls.begin() + comp->GetControlIndex(slider));
        slider->Reset();
        slider = 0;
    }
}

RobOsageTestDw::Root::AirRes::AirRes(dw::Composite* parent) : slider() {
    comp = new dw::Composite(parent);
    comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    button = new dw::Button(comp, dw::CHECKBOX);
    button->SetText("air res  ");
    button->SetValue(true);
    button->AddSelectionListener(this);

    Update(true);
}

RobOsageTestDw::Root::AirRes::~AirRes() {

}

void RobOsageTestDw::Root::AirRes::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
    if (button) {
        Update(button->value);
        rob_osage_test_dw->UpdateLayout();
    }
}

void RobOsageTestDw::Root::AirRes::SetValue(float_t value) {
    if (slider)
        slider->SetValue(value);
}

void RobOsageTestDw::Root::AirRes::Update(bool value) {
    if (value) {
        if (slider)
            return;

        slider = dw::Slider::Create(comp);
        slider->SetText("");
        slider->format = "% 2.3f";
        slider->SetParams(rob_osage_test->root.air_res, 0.0f, 1.0f, 0.1f, 0.01f, 0.1f);
    }
    else {
        if (!slider)
            return;

        comp->controls.erase(comp->controls.begin() + comp->GetControlIndex(slider));
        slider->Reset();
        slider = 0;
    }
}

RobOsageTestDw::Root::RootYRot::RootYRot(dw::Composite* parent) : slider() {
    comp = new dw::Composite(parent);
    comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    button = new dw::Button(comp, dw::CHECKBOX);
    button->SetText("rootYrot ");
    button->SetValue(true);
    button->AddSelectionListener(this);

    Update(true);
}

RobOsageTestDw::Root::RootYRot::~RootYRot() {

}

void RobOsageTestDw::Root::RootYRot::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
    if (button) {
        Update(button->value);
        rob_osage_test_dw->UpdateLayout();
    }
}

void RobOsageTestDw::Root::RootYRot::SetValue(float_t value) {
    if (slider)
        slider->SetValue(value);
}

void RobOsageTestDw::Root::RootYRot::Update(bool value) {
    if (value) {
        if (slider)
            return;

        slider = dw::Slider::Create(comp);
        slider->SetText("");
        slider->format = "% 6.0f";
        slider->SetParams(rob_osage_test->root.root_y_rot, -180.0f, 180.0f, 90.0f, 1.0f, 10.0f);
    }
    else {
        if (!slider)
            return;

        comp->controls.erase(comp->controls.begin() + comp->GetControlIndex(slider));
        slider->Reset();
        slider = 0;
    }
}

RobOsageTestDw::Root::RootZRot::RootZRot(dw::Composite* parent) : slider() {
    comp = new dw::Composite(parent);
    comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    button = new dw::Button(comp, dw::CHECKBOX);
    button->SetText("rootZrot ");
    button->SetValue(true);
    button->AddSelectionListener(this);

    Update(true);
}

RobOsageTestDw::Root::RootZRot::~RootZRot() {

}

void RobOsageTestDw::Root::RootZRot::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
    if (button) {
        Update(button->value);
        rob_osage_test_dw->UpdateLayout();
    }
}

void RobOsageTestDw::Root::RootZRot::SetValue(float_t value) {
    if (slider)
        slider->SetValue(value);
}

void RobOsageTestDw::Root::RootZRot::Update(bool value) {
    if (value) {
        if (slider)
            return;

        slider = dw::Slider::Create(comp);
        slider->SetText("");
        slider->format = "% 6.0f";
        slider->SetParams(rob_osage_test->root.root_z_rot, -180.0f, 180.0f, 90.0f, 1.0f, 10.0f);
    }
    else {
        if (!slider)
            return;

        comp->controls.erase(comp->controls.begin() + comp->GetControlIndex(slider));
        slider->Reset();
        slider = 0;
    }
}

RobOsageTestDw::Root::Fric::Fric(dw::Composite* parent) : slider() {
    comp = new dw::Composite(parent);
    comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    button = new dw::Button(comp, dw::CHECKBOX);
    button->SetText("fric     ");
    button->SetValue(true);
    button->AddSelectionListener(this);

    Update(true);
}

RobOsageTestDw::Root::Fric::~Fric() {

}

void RobOsageTestDw::Root::Fric::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
    if (button) {
        Update(button->value);
        rob_osage_test_dw->UpdateLayout();
    }
}

void RobOsageTestDw::Root::Fric::SetValue(float_t value) {
    if (slider)
        slider->SetValue(value);
}

void RobOsageTestDw::Root::Fric::Update(bool value) {
    if (value) {
        if (slider)
            return;

        slider = dw::Slider::Create(comp);
        slider->SetText("");
        slider->format = "% 2.3f";
        slider->SetParams(rob_osage_test->root.fric, 0.0f, 1.0f, 0.1f, 0.01f, 0.1f);
    }
    else {
        if (!slider)
            return;

        comp->controls.erase(comp->controls.begin() + comp->GetControlIndex(slider));
        slider->Reset();
        slider = 0;
    }
}

RobOsageTestDw::Root::WindAfc::WindAfc(dw::Composite* parent) : slider() {
    comp = new dw::Composite(parent);
    comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    button = new dw::Button(comp, dw::CHECKBOX);
    button->SetText("windAfc  ");
    button->SetValue(true);
    button->AddSelectionListener(this);

    Update(true);
}

RobOsageTestDw::Root::WindAfc::~WindAfc() {

}

void RobOsageTestDw::Root::WindAfc::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
    if (button) {
        Update(button->value);
        rob_osage_test_dw->UpdateLayout();
    }
}

void RobOsageTestDw::Root::WindAfc::SetValue(float_t value) {
    if (slider)
        slider->SetValue(value);
}

void RobOsageTestDw::Root::WindAfc::Update(bool value) {
    if (value) {
        if (slider)
            return;

        slider = dw::Slider::Create(comp);
        slider->SetText("");
        slider->format = "% 2.3f";
        slider->SetParams(rob_osage_test->root.wind_afc, 0.0f, 1.0f, 0.1f, 0.01f, 0.1f);
    }
    else {
        if (!slider)
            return;

        comp->controls.erase(comp->controls.begin() + comp->GetControlIndex(slider));
        slider->Reset();
        slider = 0;
    }
}

RobOsageTestDw::Root::ColiType::ColiType(dw::Composite* parent) : list_box() {
    comp = new dw::Composite(parent);
    comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    button = new dw::Button(comp, dw::CHECKBOX);
    button->SetText("");
    button->SetValue(true);
    button->AddSelectionListener(this);

    Update(true);
}

RobOsageTestDw::Root::ColiType::~ColiType() {

}

void RobOsageTestDw::Root::ColiType::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
    if (button) {
        Update(button->value);
        rob_osage_test_dw->UpdateLayout();
    }
}

void RobOsageTestDw::Root::ColiType::SetItemIndex(size_t value) {
    if (list_box)
        list_box->SetItemIndex(value);
}

void RobOsageTestDw::Root::ColiType::Update(bool value) {
    if (value) {
        if (list_box)
            return;

        list_box = new dw::ListBox(comp, dw::MULTISELECT);
        list_box->AddItem("coli_type: End");
        list_box->AddItem("coli_type: Ball");
        list_box->AddItem("coli_type: Capsule(+root)");
        list_box->SetItemIndex(rob_osage_test->root.coli_type);
        list_box->AddSelectionListener(new dw::SelectionListenerOnHook(
            RobOsageTestDw::Root::ColiType::ColiTypeCallback));
    }
    else {
        if (!list_box)
            return;

        comp->controls.erase(comp->controls.begin() + comp->GetControlIndex(list_box));
        list_box->Reset();
        list_box = 0;
    }
}

void RobOsageTestDw::Root::ColiType::ColiTypeCallback(dw::Widget* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box) {
        skin_param* skp = rob_osage_test->get_skin_param();
        if (skp)
            skp->coli_type = (SkinParam::RootCollisionType)(int32_t)list_box->list->selected_item;
    }
}

RobOsageTestDw::Root::InitYRot::InitYRot(dw::Composite* parent) : slider() {
    comp = new dw::Composite(parent);
    comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    button = new dw::Button(comp, dw::CHECKBOX);
    button->SetText("initYrot ");
    button->SetValue(true);
    button->AddSelectionListener(this);

    Update(true);
}

RobOsageTestDw::Root::InitYRot::~InitYRot() {

}

void RobOsageTestDw::Root::InitYRot::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
    if (button) {
        Update(button->value);
        rob_osage_test_dw->UpdateLayout();
    }
}

void RobOsageTestDw::Root::InitYRot::SetValue(float_t value) {
    if (slider)
        slider->SetValue(value);
}

void RobOsageTestDw::Root::InitYRot::Update(bool value) {
    if (value) {
        if (slider)
            return;

        slider = dw::Slider::Create(comp);
        slider->SetText("");
        slider->format = "% 6.0f";
        slider->SetParams(rob_osage_test->root.init_y_rot, -180.0f, 180.0f, 90.0f, 1.0f, 10.0f);
    }
    else {
        if (!slider)
            return;

        comp->controls.erase(comp->controls.begin() + comp->GetControlIndex(slider));
        slider->Reset();
        slider = 0;
    }
}

RobOsageTestDw::Root::InitZRot::InitZRot(dw::Composite* parent) : slider() {
    comp = new dw::Composite(parent);
    comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    button = new dw::Button(comp, dw::CHECKBOX);
    button->SetText("initZrot ");
    button->SetValue(true);
    button->AddSelectionListener(this);

    Update(true);
}

RobOsageTestDw::Root::InitZRot::~InitZRot() {

}

void RobOsageTestDw::Root::InitZRot::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
    if (button) {
        Update(button->value);
        rob_osage_test_dw->UpdateLayout();
    }
}

void RobOsageTestDw::Root::InitZRot::SetValue(float_t value) {
    if (slider)
        slider->SetValue(value);
}

void RobOsageTestDw::Root::InitZRot::Update(bool value) {
    if (value) {
        if (slider)
            return;

        slider = dw::Slider::Create(comp);
        slider->SetText("");
        slider->format = "% 6.0f";
        slider->SetParams(rob_osage_test->root.init_z_rot, -180.0f, 180.0f, 90.0f, 1.0f, 10.0f);
    }
    else {
        if (!slider)
            return;

        comp->controls.erase(comp->controls.begin() + comp->GetControlIndex(slider));
        slider->Reset();
        slider = 0;
    }
}

void RobOsageTestDw::Root::Init(dw::Composite* parent) {
    dw::Composite* comp = new dw::Composite(parent);

    list_box = new dw::ListBox(comp);
    list_box->AddSelectionListener(new dw::SelectionListenerOnHook(RobOsageTestDw::Root::OsageCallback));

    dw::Composite* ribbon_comp = new dw::Composite(comp);
    ribbon_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    reset_button = new dw::Button(ribbon_comp, dw::FLAG_8);
    reset_button->SetText("  RESET ");
    reset_button->callback = RobOsageTestDw::Root::ResetCallback;

    copy_button = new dw::Button(ribbon_comp, dw::FLAG_8);
    copy_button->SetText("  COPY  ");
    copy_button->callback = RobOsageTestDw::Root::CopyCallback;

    paste_button = new dw::Button(ribbon_comp, dw::FLAG_8);
    paste_button->SetText("  PASTE ");
    paste_button->callback = RobOsageTestDw::Root::PasteCallback;

    force = new Force(comp);
    gain = new Gain(comp);
    air_res = new AirRes(comp);
    root_y_rot = new RootYRot(comp);
    root_z_rot = new RootZRot(comp);
    fric = new Fric(comp);
    wind_afc = new WindAfc(comp);
    coli_type = new ColiType(comp);
    init_y_rot = new InitYRot(comp);
    init_z_rot = new InitZRot(comp);
}

void RobOsageTestDw::Root::Update() {
    const RobOsageTest::Root& root = rob_osage_test->root;

    force->SetValue(root.force);
    gain->SetValue(root.gain);
    air_res->SetValue(root.air_res);
    root_y_rot->SetValue(root.root_y_rot);
    root_z_rot->SetValue(root.root_z_rot);
    fric->SetValue(root.fric);
    wind_afc->SetValue(root.wind_afc);
    coli_type->SetItemIndex(root.coli_type);
    init_y_rot->SetValue(root.init_y_rot);
    init_z_rot->SetValue(root.init_z_rot);
}

void RobOsageTestDw::Root::CopyCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button) {
        rob_osage_test->root_temp = rob_osage_test->root;
        rob_osage_test->node_temp = rob_osage_test->node;
    }
}

void RobOsageTestDw::Root::OsageCallback(dw::Widget* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box) {
        rob_osage_test->osage_index = list_box->list->selected_item;
        rob_osage_test->collision_index = -1;
        rob_osage_test->collision_update = true;

        rob_osage_test->set_root();
        rob_osage_test_dw->root.Update();
        rob_osage_test_dw->node.Update();
    }
}

void RobOsageTestDw::Root::PasteCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button) {
        rob_osage_test->root = rob_osage_test->root_temp;
        rob_osage_test->node = rob_osage_test->node_temp;

        rob_osage_test_dw->root.Update();
    }
}

void RobOsageTestDw::Root::ResetCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button) {
        rob_osage_test->root = {};
        rob_osage_test->node = {};

        rob_osage_test_dw->root.Update();
        rob_osage_test_dw->node.Update();
    }
}

RobOsageTestDw::Node::CollisionRadius::CollisionRadius(dw::Composite* parent) : slider() {
    comp = new dw::Composite(parent);
    comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    button = new dw::Button(comp, dw::CHECKBOX);
    button->SetText("coli_r  ");
    button->AddSelectionListener(this);

    Update(false);
}

RobOsageTestDw::Node::CollisionRadius::~CollisionRadius() {

}

void RobOsageTestDw::Node::CollisionRadius::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
    if (button) {
        Update(button->value);
        rob_osage_test_dw->UpdateLayout();
    }
}

void RobOsageTestDw::Node::CollisionRadius::SetValue(float_t value) {
    if (slider)
        slider->SetValue(value);
}

void RobOsageTestDw::Node::CollisionRadius::Update(bool value) {
    if (value) {
        if (slider)
            return;

        slider = dw::Slider::Create(comp);
        slider->SetText("");
        slider->format = "% 2.3f";
        slider->SetParams(rob_osage_test->node.coli_r, 0.0f, 2.0f, 0.01f, 0.001f, 0.01f);
    }
    else {
        if (!slider)
            return;

        comp->controls.erase(comp->controls.begin() + comp->GetControlIndex(slider));
        slider->Reset();
        slider = 0;
    }
}

RobOsageTestDw::Node::Hinge::Hinge(dw::Composite* parent)
    : y_min_slider(), y_max_slider(), z_min_slider(), z_max_slider() {
    comp = new dw::Composite(parent);

    button = new dw::Button(comp, dw::CHECKBOX);
    button->SetText("hinge   ");
    button->AddSelectionListener(this);

    Update(false);
}

RobOsageTestDw::Node::Hinge::~Hinge() {

}

void RobOsageTestDw::Node::Hinge::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
    if (button) {
        Update(button->value);
        rob_osage_test_dw->UpdateLayout();
    }
}

void RobOsageTestDw::Node::Hinge::SetValue(const RobOsageTest::Node::Hinge& value) {
    if (y_min_slider)
        y_min_slider->SetValue(value.ymin);

    if (y_max_slider)
        y_max_slider->SetValue(value.ymax);

    if (z_min_slider)
        z_min_slider->SetValue(value.zmin);

    if (z_max_slider)
        z_max_slider->SetValue(value.zmax);
}

void RobOsageTestDw::Node::Hinge::Update(bool value) {
    if (value) {
        y_min_slider = dw::Slider::Create(comp);
        y_min_slider->SetText("   Ymin    ");
        y_min_slider->format = "% 6.0f";
        y_min_slider->SetParams(rob_osage_test->node.hinge.ymin, -179.0f, 0.0f, 89.5f, 0.5f, 5.0f);

        y_max_slider = dw::Slider::Create(comp);
        y_max_slider->SetText("   Ymax    ");
        y_max_slider->format = "% 6.0f";
        y_max_slider->SetParams(rob_osage_test->node.hinge.ymax, 0.0f, 179.0f, 89.5f, 0.5f, 5.0f);

        z_min_slider = dw::Slider::Create(comp);
        z_min_slider->SetText("   Zmin    ");
        z_min_slider->format = "% 6.0f";
        z_min_slider->SetParams(rob_osage_test->node.hinge.zmin, -179.0f, 0.0f, 89.5f, 0.5f, 5.0f);

        z_max_slider = dw::Slider::Create(comp);
        z_max_slider->SetText("   Zmax    ");
        z_max_slider->format = "% 6.0f";
        z_max_slider->SetParams(rob_osage_test->node.hinge.zmax, 0.0f, 179.0f, 89.5f, 0.5f, 5.0f);
    }
    else if (y_min_slider) {
        comp->controls.erase(comp->controls.begin() + comp->GetControlIndex(y_min_slider));
        comp->controls.erase(comp->controls.begin() + comp->GetControlIndex(y_max_slider));
        comp->controls.erase(comp->controls.begin() + comp->GetControlIndex(z_min_slider));
        comp->controls.erase(comp->controls.begin() + comp->GetControlIndex(z_max_slider));

        y_min_slider->Reset();
        y_max_slider->Reset();
        z_min_slider->Reset();
        z_max_slider->Reset();
        y_min_slider = 0;
        y_max_slider = 0;
        z_min_slider = 0;
        z_max_slider = 0;
    }
}

RobOsageTestDw::Node::InertialCancel::InertialCancel(dw::Composite* parent) : slider() {
    comp = new dw::Composite(parent);
    comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    button = new dw::Button(comp, dw::CHECKBOX);
    button->SetText("i.c.    ");
    button->AddSelectionListener(this);

    Update(false);
}

RobOsageTestDw::Node::InertialCancel::~InertialCancel() {

}

void RobOsageTestDw::Node::InertialCancel::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
    if (button) {
        Update(button->value);
        rob_osage_test_dw->UpdateLayout();
    }
}

void RobOsageTestDw::Node::InertialCancel::SetValue(float_t value) {
    if (slider)
        slider->SetValue(value);
}

void RobOsageTestDw::Node::InertialCancel::Update(bool value) {
    if (value) {
        slider = dw::Slider::Create(comp);
        slider->SetText("");
        slider->format = "% 2.3f";
        slider->SetParams(rob_osage_test->node.inertial_cancel, 0.0f, 1.0f, 0.1f, 0.01f, 0.1f);
    }
    else if (slider) {
        comp->controls.erase(comp->controls.begin() + comp->GetControlIndex(slider));
        slider->Reset();
        slider = 0;
    }
}

RobOsageTestDw::Node::Weight::Weight(dw::Composite* parent) : slider() {
    comp = new dw::Composite(parent);
    comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    button = new dw::Button(comp, dw::CHECKBOX);
    button->SetText("weight  ");
    button->AddSelectionListener(this);

    Update(false);
}

RobOsageTestDw::Node::Weight::~Weight() {

}

void RobOsageTestDw::Node::Weight::Callback(dw::SelectionListener::CallbackData* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data->widget);
    if (button) {
        Update(button->value);
        rob_osage_test_dw->UpdateLayout();
    }
}

void RobOsageTestDw::Node::Weight::SetValue(float_t value) {
    if (slider)
        slider->SetValue(value);
}

void RobOsageTestDw::Node::Weight::Update(bool value) {
    if (value) {
        slider = dw::Slider::Create(comp);
        slider->SetText("");
        slider->format = "% 2.3f";
        slider->SetParams(rob_osage_test->node.weight, 0.0f, 1.0f, 0.1f, 0.01f, 0.1f);
    }
    else if (slider) {
        comp->controls.erase(comp->controls.begin() + comp->GetControlIndex(slider));
        slider->Reset();
        slider = 0;
    }
}

void RobOsageTestDw::Node::Init(dw::Composite* parent) {
    group = new dw::Group(parent);
    group->SetText("Node Param");

    coli_r = new CollisionRadius(group);
    weight = new Weight(group);
    inertial_cancel = new InertialCancel(group);
    hinge = new Hinge(group);
}

void RobOsageTestDw::Node::Update() {
    const RobOsageTest::Node& node = rob_osage_test->node;

    coli_r->SetValue(node.coli_r);
    weight->SetValue(node.weight);
    inertial_cancel->SetValue(node.inertial_cancel);
    hinge->SetValue(node.hinge);
}

void RobOsageTestDw::BetweenOsageCollision::Init(dw::Composite* parent) {
    dw::Composite* comp = new dw::Composite(parent);

    button = new dw::Button(comp, dw::CHECKBOX);
    button->SetText("Between Osage Collision");
}

void RobOsageTestDw::Collision::Init(dw::Composite* parent) {
    group = new dw::Group(parent);
    group->SetText("COLLISION");

    dw::Composite* ribbon_comp = new dw::Composite(group);
    ribbon_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    reset_button = new dw::Button(ribbon_comp, dw::FLAG_8);
    reset_button->SetText("  RESET ");
    reset_button->callback = RobOsageTestDw::Collision::ResetCallback;

    copy_button = new dw::Button(ribbon_comp, dw::FLAG_8);
    copy_button->SetText("  COPY  ");
    copy_button->callback = RobOsageTestDw::Collision::CopyCallback;

    paste_button = new dw::Button(ribbon_comp, dw::FLAG_8);
    paste_button->SetText("  PASTE ");
    paste_button->callback = RobOsageTestDw::Collision::PasteCallback;

    element_comp = 0;
}

void RobOsageTestDw::Collision::Update() {
    if (element_comp) {
        Composite* parent_comp = element_comp->parent_comp;
        auto i_begin = parent_comp->controls.begin();
        auto i_end = parent_comp->controls.end();
        for (auto i = i_begin; i != i_end; )
            if (*i == element_comp) {
                parent_comp->controls.erase(i);
                break;
            }
            else
                i++;

        element_comp->Free();
        element_comp = 0;
    }

    element_comp = new dw::Composite(group);

    const std::vector<SkinParam::CollisionParam>* cls_list = rob_osage_test->get_cls_list();
    if (!cls_list)
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    const std::vector<std::string>* object_bones = aft_bone_data->get_skeleton_object_bones(
        bone_database_skeleton_type_to_string(BONE_DATABASE_SKELETON_COMMON));

    char buf[0x200];
    int32_t index = 0;
    for (const SkinParam::CollisionParam& i : *cls_list) {
        dw::Button* element_button = new dw::Button(element_comp, dw::RADIOBUTTON);
        switch (i.type) {
        case SkinParam::CollisionTypeEnd:
            sprintf_s(buf, sizeof(buf), "%d %s", index, collision_type_name_list[i.type]);
            break;
        case SkinParam::CollisionTypeBall:
        case SkinParam::CollisionTypeCapsule:
        case SkinParam::CollisionTypePlane:
        case SkinParam::CollisionTypeEllipse:
        case SkinParam::CollisionTypeAABB:
            sprintf_s(buf, sizeof(buf), "%d %s/%s", index,
                collision_type_name_list[i.type], object_bones->data()[i.node_idx[0]].c_str());
            break;
        default:
            sprintf_s(buf, sizeof(buf), "%d", index);
            break;
        }

        element_button->SetText(buf);
        element_button->callback_data.i32 = index;
        element_button->callback = RobOsageTestDw::Collision::ElementCallback;
        if (index == rob_osage_test->collision_index)
            element_button->SetValue(true);

        if (i.type == SkinParam::CollisionTypeEnd)
            break;

        index++;
    }
}

void RobOsageTestDw::Collision::CopyCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button) {
        std::vector<SkinParam::CollisionParam>* cls_list = rob_osage_test->get_cls_list();
        if (cls_list)
            rob_osage_test->cls_list_temp.assign(cls_list->begin(), cls_list->end());
    }
}

void RobOsageTestDw::Collision::ElementCallback(dw::Widget* data) {
    int32_t index = data->callback_data.i32;
    std::vector<SkinParam::CollisionParam>* cls_list = rob_osage_test->get_cls_list();
    if (cls_list) {
        rob_osage_test->collision_index = index;

        rob_osage_test_dw->colli_element.Update();
    }
}

void RobOsageTestDw::Collision::PasteCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);

    if (button) {
        std::vector<SkinParam::CollisionParam>* cls_list = rob_osage_test->get_cls_list();
        if (cls_list) {
            cls_list->assign(
                rob_osage_test->cls_list_temp.begin(), rob_osage_test->cls_list_temp.end());

            rob_osage_test->collision_index = -1;
            rob_osage_test->collision_update = true;
        }
    }
}

void RobOsageTestDw::Collision::ResetCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button) {
        std::vector<SkinParam::CollisionParam>* cls_list = rob_osage_test->get_cls_list();
        if (cls_list) {
            cls_list->clear();
            cls_list->push_back({});
            rob_osage_test->collision_index = -1;
            rob_osage_test->collision_update = true;
        }
    }
}

void RobOsageTestDw::ColliElement::Init(dw::Composite* parent) {
    group = new dw::Group(parent);
    group->SetText("COLLI ELEMENT");

    dw::Composite* ribbon_comp = new dw::Composite(group);
    ribbon_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    reset_button = new dw::Button(ribbon_comp, dw::FLAG_8);
    reset_button->SetText("RESET ");
    reset_button->callback = RobOsageTestDw::ColliElement::ResetCallback;

    copy_button = new dw::Button(ribbon_comp, dw::FLAG_8);
    copy_button->SetText(" COPY ");
    copy_button->callback = RobOsageTestDw::ColliElement::CopyCallback;

    paste_button = new dw::Button(ribbon_comp, dw::FLAG_8);
    paste_button->SetText("PASTE ");
    paste_button->callback = RobOsageTestDw::ColliElement::PasteCallback;

    flip_button = new dw::Button(ribbon_comp, dw::FLAG_8);
    flip_button->SetText(" FLIP ");
    flip_button->callback = RobOsageTestDw::ColliElement::FlipCallback;

    type_list_box = new dw::ListBox(group, dw::MULTISELECT);
    for (const char*& i : collision_type_name_list)
        type_list_box->AddItem(i);
    type_list_box->AddSelectionListener(new dw::SelectionListenerOnHook(
        RobOsageTestDw::ColliElement::TypeCallback));

    radius_slider = dw::Slider::Create(group);
    radius_slider->SetText("R");
    radius_slider->format = "% 2.3f   ";
    radius_slider->SetParams(0.0f, 0.0f, 2.0f, 0.01f, 0.001f, 0.01f);
    radius_slider->AddSelectionListener(new dw::SelectionListenerOnHook(
        RobOsageTestDw::ColliElement::RadiusCallback));

    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    const std::vector<std::string>* object_bones = aft_bone_data->get_skeleton_object_bones(
        bone_database_skeleton_type_to_string(BONE_DATABASE_SKELETON_COMMON));

    bone0_list_box = new dw::ListBox(group);
    bone0_list_box->SetMaxItems(20);
    for (const std::string& i : *object_bones)
        bone0_list_box->AddItem(i);
    bone0_list_box->callback_data.i32 = 0;
    bone0_list_box->AddSelectionListener(new dw::SelectionListenerOnHook(
        RobOsageTestDw::ColliElement::BoneCallback));

    bone0_x_slider = dw::Slider::Create(group);
    bone0_x_slider->SetText("x");
    bone0_x_slider->format = "% 2.3f   ";
    bone0_x_slider->SetParams(0.0f, -1.0f, 1.0f, 0.1f, 0.01f, 0.1f);
    bone0_x_slider->callback_data.i32 = 0;
    bone0_x_slider->AddSelectionListener(new dw::SelectionListenerOnHook(
        RobOsageTestDw::ColliElement::BonePosXCallback));
    
    bone0_y_slider = dw::Slider::Create(group);
    bone0_y_slider->SetText("y");
    bone0_y_slider->format = "% 2.3f   ";
    bone0_y_slider->SetParams(0.0f, -1.0f, 1.0f, 0.1f, 0.01f, 0.1f);
    bone0_y_slider->callback_data.i32 = 0;
    bone0_y_slider->AddSelectionListener(new dw::SelectionListenerOnHook(
        RobOsageTestDw::ColliElement::BonePosYCallback));
    
    bone0_z_slider = dw::Slider::Create(group);
    bone0_z_slider->SetText("z");
    bone0_z_slider->format = "% 2.3f   ";
    bone0_z_slider->SetParams(0.0f, -1.0f, 1.0f, 0.1f, 0.01f, 0.1f);
    bone0_z_slider->callback_data.i32 = 0;
    bone0_z_slider->AddSelectionListener(new dw::SelectionListenerOnHook(
        RobOsageTestDw::ColliElement::BonePosZCallback));

    bone1_list_box = new dw::ListBox(group);
    bone1_list_box->SetMaxItems(20);
    for (const std::string& i : *object_bones)
        bone1_list_box->AddItem(i);
    bone1_list_box->callback_data.i32 = 1;
    bone1_list_box->AddSelectionListener(new dw::SelectionListenerOnHook(
        RobOsageTestDw::ColliElement::BoneCallback));

    bone1_x_slider = dw::Slider::Create(group);
    bone1_x_slider->SetText("x");
    bone1_x_slider->format = "% 2.3f   ";
    bone1_x_slider->SetParams(0.0f, -1.0f, 1.0f, 0.1f, 0.01f, 0.1f);
    bone1_x_slider->callback_data.i32 = 1;
    bone1_x_slider->AddSelectionListener(new dw::SelectionListenerOnHook(
        RobOsageTestDw::ColliElement::BonePosXCallback));

    bone1_y_slider = dw::Slider::Create(group);
    bone1_y_slider->SetText("y");
    bone1_y_slider->format = "% 2.3f   ";
    bone1_y_slider->SetParams(0.0f, -1.0f, 1.0f, 0.1f, 0.01f, 0.1f);
    bone1_y_slider->callback_data.i32 = 1;
    bone1_y_slider->AddSelectionListener(new dw::SelectionListenerOnHook(
        RobOsageTestDw::ColliElement::BonePosYCallback));

    bone1_z_slider = dw::Slider::Create(group);
    bone1_z_slider->SetText("z");
    bone1_z_slider->format = "% 2.3f   ";
    bone1_z_slider->SetParams(0.0f, -1.0f, 1.0f, 0.1f, 0.01f, 0.1f);
    bone1_z_slider->callback_data.i32 = 1;
    bone1_z_slider->AddSelectionListener(new dw::SelectionListenerOnHook(
        RobOsageTestDw::ColliElement::BonePosZCallback));
}

void RobOsageTestDw::ColliElement::Update() {
    const SkinParam::CollisionParam* cls_param = rob_osage_test->get_cls_param();
    if (!cls_param)
        return;

    type_list_box->SetItemIndex(cls_param->type);

    radius_slider->SetValue(cls_param->radius);

    bone0_list_box->SetItemIndex(cls_param->node_idx[0]);

    bone0_x_slider->SetValue(cls_param->pos[0].x);
    bone0_y_slider->SetValue(cls_param->pos[0].y);
    bone0_z_slider->SetValue(cls_param->pos[0].z);

    bone1_list_box->SetItemIndex(cls_param->node_idx[1]);

    bone1_x_slider->SetValue(cls_param->pos[1].x);
    bone1_y_slider->SetValue(cls_param->pos[1].y);
    bone1_z_slider->SetValue(cls_param->pos[1].z);
}

void RobOsageTestDw::ColliElement::BoneCallback(dw::Widget* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box) {
        SkinParam::CollisionParam* cls_param = rob_osage_test->get_cls_param();
        if (cls_param) {
            cls_param->type = (SkinParam::CollisionType)(int32_t)rob_osage_test_dw->
                colli_element.type_list_box->list->selected_item;
            cls_param->node_idx[list_box->callback_data.i32] = (int32_t)list_box->list->selected_item;

            rob_osage_test->collision_update = true;
        }
    }
}

void RobOsageTestDw::ColliElement::BonePosXCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider) {
        SkinParam::CollisionParam* cls_param = rob_osage_test->get_cls_param();
        if (cls_param) {
            cls_param->type = (SkinParam::CollisionType)(int32_t)rob_osage_test_dw->
                colli_element.type_list_box->list->selected_item;
            cls_param->pos[slider->callback_data.i32].x = slider->scroll_bar->value;
        }
    }
}

void RobOsageTestDw::ColliElement::BonePosYCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider) {
        SkinParam::CollisionParam* cls_param = rob_osage_test->get_cls_param();
        if (cls_param) {
            cls_param->type = (SkinParam::CollisionType)(int32_t)rob_osage_test_dw->
                colli_element.type_list_box->list->selected_item;
            cls_param->pos[slider->callback_data.i32].y = slider->scroll_bar->value;
        }
    }
}

void RobOsageTestDw::ColliElement::BonePosZCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider) {
        SkinParam::CollisionParam* cls_param = rob_osage_test->get_cls_param();
        if (cls_param) {
            cls_param->type = (SkinParam::CollisionType)(int32_t)rob_osage_test_dw->
                colli_element.type_list_box->list->selected_item;
            cls_param->pos[slider->callback_data.i32].z = slider->scroll_bar->value;
        }
    }
}

void RobOsageTestDw::ColliElement::CopyCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button) {
        SkinParam::CollisionParam* cls_param = rob_osage_test->get_cls_param();
        if (cls_param)
            rob_osage_test->cls_param_temp = *cls_param;
    }
}

void RobOsageTestDw::ColliElement::FlipCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button) {
        SkinParam::CollisionParam* cls_param = rob_osage_test->get_cls_param();
        if (cls_param) {
            int32_t node_idx = cls_param->node_idx[0];
            vec3 pos = cls_param->pos[0];
            cls_param->node_idx[0] = cls_param->node_idx[1];
            cls_param->pos[0] = cls_param->pos[1];
            cls_param->node_idx[1] = node_idx;
            cls_param->pos[1] = pos;

            rob_osage_test->collision_update = true;
            rob_osage_test_dw->colli_element.Update();
        }
    }
}

void RobOsageTestDw::ColliElement::PasteCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button) {
        SkinParam::CollisionParam* cls_param = rob_osage_test->get_cls_param();
        if (cls_param) {
            *cls_param = rob_osage_test->cls_param_temp;

            rob_osage_test->collision_update = true;
            rob_osage_test_dw->colli_element.Update();
        }
    }
}

void RobOsageTestDw::ColliElement::RadiusCallback(dw::Widget* data) {
    dw::Slider* slider = dynamic_cast<dw::Slider*>(data);
    if (slider) {
        SkinParam::CollisionParam* cls_param = rob_osage_test->get_cls_param();
        if (cls_param) {
            cls_param->type = (SkinParam::CollisionType)(int32_t)rob_osage_test_dw->
                colli_element.type_list_box->list->selected_item;
            cls_param->radius = slider->scroll_bar->value;
        }
    }
}

void RobOsageTestDw::ColliElement::ResetCallback(dw::Widget* data) {
    dw::Button* button = dynamic_cast<dw::Button*>(data);
    if (button) {
        SkinParam::CollisionParam* cls_param = rob_osage_test->get_cls_param();
        if (cls_param) {
            *cls_param = {};

            rob_osage_test->collision_update = true;
            rob_osage_test_dw->colli_element.Update();
        }
    }
}

void RobOsageTestDw::ColliElement::TypeCallback(dw::Widget* data) {
    dw::ListBox* list_box = dynamic_cast<dw::ListBox*>(data);
    if (list_box) {
        SkinParam::CollisionParam* cls_param = rob_osage_test->get_cls_param();
        if (cls_param) {
            cls_param->type = (SkinParam::CollisionType)(int32_t)list_box->list->selected_item;

            rob_osage_test->collision_update = true;
        }
    }
}

RobOsageTestDw::RobOsageTestDw() : rob(), flags(), root(), node(), boc(), collision(), colli_element() {
    SetText("RobOsage TEST");

    dw::Composite* main_comp = new dw::Composite(this);
    main_comp->SetLayout(new dw::RowLayout(dw::HORIZONTAL));

    dw::Composite* left_comp = new dw::Composite(main_comp);

    rob.Init(left_comp);
    flags.Init(left_comp);
    root.Init(left_comp);
    node.Init(left_comp);
    boc.Init(left_comp);

    dw::Composite* right_comp = new dw::Composite(main_comp);

    collision.Init(right_comp);
    colli_element.Init(right_comp);

    UpdateLayout();

    rect.pos.x = (float_t)res_window_get()->width - rect.size.x;
    rect.pos.y = 160.0f;

    SetSize(rect.size);
}

RobOsageTestDw::~RobOsageTestDw() {

}

void RobOsageTestDw::Hide() {
    SetDisp();
}
