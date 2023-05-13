/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "rob.hpp"
#include "../../KKdLib/prj/algorithm.hpp"
#include "../../KKdLib/str_utils.hpp"
#include "../mdata_manager.hpp"
#include "skin_param.hpp"

struct osage_setting {
    std::map<std::pair<std::string, std::string>, osage_setting_osg_cat> cat;
    std::map<object_info, std::string> obj;

    osage_setting();
    ~osage_setting();

    void clear();
    const osage_setting_osg_cat* get_cat_value(
        object_info* obj_info, const char* root_node);
    void load();
    void parse(key_val* kv);
    rob_osage_parts parse_parts_string(std::string& s);
};

struct skin_param_file {
    p_file_handler* file_handler;
    item_id id;
    int32_t type;
    std::vector<skin_param_file_data>* data;
    rob_chara* rob_chr;

    skin_param_file();
    ~skin_param_file();
};

struct SkinParamManager : public app::Task {
    int32_t state;
    uint8_t index;
    std::vector<osage_init_data> osage_init;
    std::map<std::pair<object_info, std::pair<int32_t, int32_t>>,
        std::vector<skin_param_file_data>*> object_motion_frame;
    std::map<std::pair<object_info, std::pair<int32_t, int32_t>>,
        std::vector<skin_param_file_data>*> object_motion;
    std::map<std::pair<object_info, std::pair<int32_t, int32_t>>,
        std::vector<skin_param_file_data>*> object;
    std::list<skin_param_file*> files;

    SkinParamManager();
    virtual ~SkinParamManager() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;

    void AddFiles();
    bool AddTask(std::vector<osage_init_data>& vec);
    bool CtrlFiles();
    int32_t GetExtSkpFile(const osage_init_data& osage_init, const std::string& path,
        std::string& file, void* data, motion_database* mot_db, object_database* obj_db);
    std::vector<skin_param_file_data>* GetSkinParamFileData(
        object_info obj_info, int32_t motion_id, int32_t frame);
    void Reset();
};

struct sp_skp_db {
    std::map<int32_t, std::shared_ptr<std::map<object_info, uint32_t>>> pv;
    std::map<std::string, std::shared_ptr<std::map<object_info, uint32_t>>> motion;
    std::vector<std::string> farc_list;
    size_t farc_list_count;

    sp_skp_db();
    ~sp_skp_db();

    void clear();
    void load();
    void parse(key_val* kv);

    int32_t get_ext_skp_file(const osage_init_data& osage_init,
        const object_info obj_info, std::string& file, std::string& farc,
        void* data, motion_database* mot_db, object_database* obj_db);
};

static bool osage_setting_data_load_file(void* data, const char* path, const char* file, uint32_t hash);

static SkinParamManager* skin_param_manager_get(int32_t chara_id);

static bool sp_skp_db_load_file(void* data, const char* path, const char* file, uint32_t hash);

osage_setting* osage_setting_data;
sp_skp_db* sp_skp_db_data;

SkinParamManager* skin_param_manager;

std::map<object_info, prj::shared_ptr<key_val>> skin_param_storage;


SkinParam::CollisionParam::CollisionParam() : type(), node_idx(), pos() {
    radius = 0.2f;
}

skin_param_osage_root_normal_ref::skin_param_osage_root_normal_ref() {

}

skin_param_osage_root_normal_ref::~skin_param_osage_root_normal_ref() {

}

skin_param_osage_root_boc::skin_param_osage_root_boc() : ed_node(), st_node() {

}

skin_param_osage_root_boc::~skin_param_osage_root_boc() {

}

skin_param_osage_root::skin_param_osage_root() : field_0(), force(), force_gain(),
rot_y(), rot_z(), init_rot_y(), init_rot_z(), coli_r(), yz_order(), coli_type(), stiffness() {
    air_res = 0.5f;
    hinge_y = 90.0f;
    hinge_z = 90.0f;
    name ="NO-PARAM";
    friction = 1.0f;
    wind_afc = 0.5f;
    move_cancel = -0.01f;
    coli.resize(13);
}

skin_param_osage_root::~skin_param_osage_root() {

}

skin_param::skin_param() : friction(), wind_afc(), air_res(), rot(), init_rot(),
coli_type(), stiffness(), move_cancel(), coli_r(), force(), force_gain(), colli_tgt_osg() {
    reset();
}

skin_param::~skin_param() {

}

void skin_param::set_skin_param_osage_root(const skin_param_osage_root& skp_root) {
    coli.assign(skp_root.coli.begin(), skp_root.coli.end());
    friction = skp_root.friction;
    wind_afc = skp_root.wind_afc;
    air_res = skp_root.air_res;
    rot.y = skp_root.rot_y;
    rot.z = skp_root.rot_z;
    init_rot.y = skp_root.init_rot_y * DEG_TO_RAD_FLOAT;
    init_rot.z = skp_root.init_rot_z * DEG_TO_RAD_FLOAT;
    coli_type = skp_root.coli_type;
    stiffness = skp_root.stiffness;
    move_cancel = skp_root.move_cancel;
    coli_r = skp_root.coli_r;
    hinge.ymin = -skp_root.hinge_y;
    hinge.ymax = skp_root.hinge_y;
    hinge.zmin = -skp_root.hinge_z;
    hinge.zmax = skp_root.hinge_z;
    hinge.limit();
    force = skp_root.force;
    force_gain = skp_root.force_gain;
}

skin_param_file_data::skin_param_file_data() : field_88() {

}

skin_param_file_data::~skin_param_file_data() {

}

osage_setting_osg_cat::osage_setting_osg_cat() : exf() {
    parts = ROB_OSAGE_PARTS_NONE;
}

const osage_setting_osg_cat* osage_setting_data_get_cat_value(
    object_info* obj_info, const char* root_node) {
    return osage_setting_data->get_cat_value(obj_info, root_node);
}

bool osage_setting_data_obj_has_key(object_info key) {
    return osage_setting_data->obj.find(key) != osage_setting_data->obj.end();
}

void skin_param_data_init() {
    if (!osage_setting_data)
        osage_setting_data = new osage_setting;

    if (!sp_skp_db_data)
        sp_skp_db_data = new sp_skp_db;

    if (!skin_param_manager)
        skin_param_manager = new SkinParamManager[ROB_CHARA_COUNT];

    skin_param_storage = {};
}

void skin_param_data_load() {
    osage_setting_data->load();
    sp_skp_db_data->load();
}

void skin_param_data_free() {
    skin_param_storage.clear();

    if (skin_param_manager) {
        delete[] skin_param_manager;
        skin_param_manager = 0;
    }

    if (sp_skp_db_data) {
        delete sp_skp_db_data;
        sp_skp_db_data = 0;
    }

    if (osage_setting_data) {
        delete osage_setting_data;
        osage_setting_data = 0;
    }
}

bool skin_param_manager_array_check_task_ready() {
    bool ret = false;
    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++)
        ret |= app::TaskWork::CheckTaskReady(&skin_param_manager[i]);
    return ret;
}

bool skin_param_manager_add_task(int32_t chara_id, std::vector<osage_init_data>& vec) {
    return skin_param_manager[chara_id].AddTask(vec);
}

bool skin_param_manager_check_task_ready(int32_t chara_id) {
    return app::TaskWork::CheckTaskReady(skin_param_manager_get(chara_id));
}

std::vector<skin_param_file_data>* skin_param_manager_get_skin_param_file_data(
    int32_t chara_id, object_info obj_info, int32_t motion_id, int32_t frame) {
    return skin_param_manager_get(chara_id)->GetSkinParamFileData(obj_info, motion_id, frame);
}

void skin_param_manager_reset(int32_t chara_id) {
    skin_param_manager_get(chara_id)->Reset();
}

void skin_param_osage_node_parse(void* kv, const char* name,
    std::vector<skin_param_osage_node>* vec, skin_param_osage_root& skp_root) {
    key_val* _kv = (key_val*)kv;
    if (!_kv->open_scope(name))
        return;

    if (!_kv->open_scope("node")) {
        _kv->close_scope();
        return;
    }

    int32_t count;
    if (_kv->read("length", count)) {
        size_t c = vec->size();
        if (c < count)
            vec->resize(count);
    }
    else
        vec->clear();

    int32_t j = 0;
    for (auto i = vec->begin(); i != vec->end(); i++, j++) {
        if (!_kv->open_scope_fmt(j))
            continue;

        *i = skin_param_osage_node();

        float_t coli_r = 0.0f;
        if (_kv->read("coli_r", coli_r))
            i->coli_r = coli_r;

        float_t weight = 0.0f;
        if (_kv->read("weight", weight))
            i->weight = weight;

        float_t inertial_cancel = 0.0f;
        if (_kv->read("inertial_cancel", inertial_cancel))
            i->inertial_cancel = inertial_cancel;

        i->hinge.ymin = -skp_root.hinge_y;
        i->hinge.ymax = skp_root.hinge_y;
        i->hinge.zmin = -skp_root.hinge_z;
        i->hinge.zmax = skp_root.hinge_z;

        float_t hinge_ymax = 0.0f;
        if (_kv->read("hinge_ymax", hinge_ymax))
            i->hinge.ymax = hinge_ymax;

        float_t hinge_ymin = 0.0f;
        if (_kv->read("hinge_ymin", hinge_ymin))
            i->hinge.ymin = hinge_ymin;

        float_t hinge_zmax = 0.0f;
        if (_kv->read("hinge_zmax", hinge_zmax))
            i->hinge.zmax = hinge_zmax;

        float_t hinge_zmin = 0.0f;
        if (_kv->read("hinge_zmin", hinge_zmin))
            i->hinge.zmin = hinge_zmin;
        _kv->close_scope();
    }

    _kv->close_scope();
    _kv->close_scope();
}

void skin_param_osage_root_parse(void* kv, const char* name,
    skin_param_osage_root& skp_root, const bone_database* bone_data) {
    key_val* _kv = (key_val*)kv;
    if (!_kv->open_scope(name))
        return;

    int32_t node_length = 0;
    _kv->read("node.length", node_length);

    if (!_kv->open_scope("root")) {
        _kv->close_scope();
        return;
    }

    float_t force = 0.0f;
    float_t force_gain = 0.0f;
    if (_kv->read("force", force)
        && _kv->read("force_gain", force_gain)) {
        skp_root.force = force;
        skp_root.force_gain = force_gain;
    }

    float_t air_res = 0.0f;
    if (_kv->read("air_res", air_res))
        skp_root.air_res = min_def(air_res, 0.9f);

    float_t rot_y = 0.0f;
    float_t rot_z = 0.0f;
    if (_kv->read("rot_y", rot_y)
        && _kv->read("rot_z", rot_z)) {
        skp_root.rot_y = rot_y;
        skp_root.rot_z = rot_z;
    }

    float_t friction = 0.0f;
    if (_kv->read("friction", friction))
        skp_root.friction = friction;

    float_t wind_afc = 0;
    if (_kv->read("wind_afc", wind_afc)) {
        skp_root.wind_afc = wind_afc;
        if (!str_utils_compare_length(name, utf8_length(name), "c_opa", 5))
            skp_root.wind_afc = 0.0f;
    }

    int32_t coli_type = 0;
    if (_kv->read("coli_type", coli_type))
        skp_root.coli_type = coli_type;

    float_t init_rot_y = 0.0f;
    float_t init_rot_z = 0.0f;
    if (_kv->read("init_rot_y", init_rot_y)
        && _kv->read("init_rot_z", init_rot_z)) {
        skp_root.init_rot_y = init_rot_y;
        skp_root.init_rot_z = init_rot_z;
    }

    float_t hinge_y = 0.0f;
    float_t hinge_z = 0.0f;
    if (_kv->read("hinge_y", hinge_y)
        && _kv->read("hinge_z", hinge_z)) {
        skp_root.hinge_y = hinge_y;
        skp_root.hinge_z = hinge_z;
    }

    float_t coli_r = 0.0f;
    if (_kv->read("coli_r", coli_r))
        skp_root.coli_r = coli_r;

    float_t stiffness = 0.0f;
    if (_kv->read("stiffness", stiffness))
        skp_root.stiffness = stiffness;

    float_t move_cancel = 0.0f;
    if (_kv->read("move_cancel", move_cancel))
        skp_root.move_cancel = move_cancel;

    int32_t count;
    if (_kv->read("coli", "length", count)) {
        std::vector<SkinParam::CollisionParam>& vc = skp_root.coli;
        for (int32_t i = 0; i < count; i++) {
            if (!_kv->open_scope_fmt(i))
                continue;

            SkinParam::CollisionParam* c = &vc[i];

            int32_t type = 0;
            if (!_kv->read("type", type)) {
                _kv->close_scope();
                break;
            }
            c->type = (SkinParam::CollisionType)type;

            float_t radius = 0;
            if (!_kv->read("radius", radius)) {
                _kv->close_scope();
                break;
            }
            c->radius = radius;

            const char* bone0_name;
            if (!_kv->read("bone.0.name", bone0_name)) {
                _kv->close_scope();
                break;
            }

            c->node_idx[0] = bone_data->get_skeleton_object_bone_index(
                bone_database_skeleton_type_to_string(BONE_DATABASE_SKELETON_COMMON), bone0_name);

            vec3 bone0_pos = 0.0f;
            if (!_kv->read("bone.0.posx", bone0_pos.x)
                || !_kv->read("bone.0.posy", bone0_pos.y)
                || !_kv->read("bone.0.posz", bone0_pos.z)) {
                _kv->close_scope();
                break;
            }

            c->pos[0] = bone0_pos;

            const char* bone1_name;
            if (_kv->read("bone.1.name", bone1_name)) {
                c->node_idx[1] = bone_data->get_skeleton_object_bone_index(
bone_database_skeleton_type_to_string(BONE_DATABASE_SKELETON_COMMON), bone1_name);

vec3 bone1_pos = 0.0f;
if (!_kv->read("bone.1.posx", bone1_pos.x)
    || !_kv->read("bone.1.posy", bone1_pos.y)
    || !_kv->read("bone.1.posz", bone1_pos.z)) {
    _kv->close_scope();
    break;
}

c->pos[1] = bone1_pos;
            }
            _kv->close_scope();
        }
        _kv->close_scope();
    }

    if (_kv->read("boc", "length", count)) {
        std::vector<skin_param_osage_root_boc>* vb = &skp_root.boc;

        vb->resize(count);
        for (int32_t i = 0; i < count; i++) {
            if (!_kv->open_scope_fmt(i))
                continue;

            int32_t st_node = 0;
            std::string ed_root;
            int32_t ed_node = 0;
            if (_kv->read("st_node", st_node)
                && st_node < node_length
                && _kv->read("ed_root", ed_root)
                && _kv->read("ed_node", ed_node)
                && ed_node < node_length) {
                skin_param_osage_root_boc b;
                b.st_node = st_node;
                b.ed_root.assign(ed_root);
                b.ed_node = ed_node;
                vb->push_back(b);
            }
            _kv->close_scope();
        }
        _kv->close_scope();
    }

    std::string colli_tgt_osg;
    if (_kv->read("colli_tgt_osg", colli_tgt_osg))
        skp_root.colli_tgt_osg.assign(colli_tgt_osg);

    if (_kv->read("normal_ref", "length", count)) {
        std::vector<skin_param_osage_root_normal_ref>* vnr = &skp_root.normal_ref;

        vnr->resize(count);
        for (int32_t i = 0; i < count; i++) {
            if (!_kv->open_scope_fmt(i))
                continue;

            std::string n;
            if (_kv->read("N", n)) {
                skin_param_osage_root_normal_ref nr;
                nr.n.assign(n);
                _kv->read("U", nr.u);
                _kv->read("D", nr.d);
                _kv->read("L", nr.l);
                _kv->read("R", nr.r);
                vnr->push_back(nr);
            }
            _kv->close_scope();
        }
        _kv->close_scope();
    }

    _kv->close_scope();
    _kv->close_scope();
}

key_val* skin_param_storage_get_key_val(object_info obj_info) {
    auto elem = skin_param_storage.find(obj_info);
    if (elem != skin_param_storage.end())
        return elem->second.get();
    return 0;
}

void skin_param_storage_load(std::vector<object_info>& obj_infos,
    void* data, const object_database* obj_db) {
    prj::sort_unique(obj_infos);
    for (const object_info& i : obj_infos) {
        if (i.is_null())
            continue;

        const char* obj_name = obj_db->get_object_name(i);
        if (!obj_name)
            continue;

        char buf[0x200];
        sprintf_s(buf, sizeof(buf), "ext_skp_%s.txt", obj_name);

        for (int32_t i = 0; buf[i]; i++) {
            char c = buf[i];
            if (c >= 'A' && c <= 'Z')
                c += 0x20;
            buf[i] = c;
        }

        if (((data_struct*)data)->check_file_exists("rom/skin_param/", buf)) {
            prj::shared_ptr<key_val> kv = prj::shared_ptr<key_val>(new key_val);
            ((data_struct*)data)->load_file(kv.get(), "rom/skin_param/", buf, key_val::load_file);
            skin_param_storage.insert({ i, kv });
        }
    }
}

void skin_param_storage_reset() {
    skin_param_storage.clear();
}

osage_setting::osage_setting() {

}

osage_setting::~osage_setting() {

}

void osage_setting::clear() {
    cat.clear();
    obj.clear();
}

const osage_setting_osg_cat* osage_setting::get_cat_value(
    object_info* obj_info, const char* root_node) {
    auto elem_obj = osage_setting_data->obj.find(*obj_info);
    if (elem_obj == osage_setting_data->obj.end())
        return 0;

    auto elem_cat = osage_setting_data->cat.find({ elem_obj->second, root_node });
    if (elem_cat == osage_setting_data->cat.end())
        return 0;

    return &elem_cat->second;
}

void osage_setting::load() {
    clear();

    data_struct* aft_data = &data_list[DATA_AFT];
    for (const std::string& i : mdata_manager_get()->GetPrefixes()) {
        std::string file;
        file.assign(i);
        file.append("osage_setting.txt");
        aft_data->load_file(this, "rom/skin_param/",
            file.c_str(), osage_setting_data_load_file);
    }
}

void osage_setting::parse(key_val* kv) {
    int32_t count;
    if (kv->read("cat", "length", count)) {
        for (int32_t i = 0; i < count; i++) {
            if (!kv->open_scope_fmt(i))
                continue;

            std::string name;
            kv->read("name", name);

            int32_t count;
            if (kv->read("osg", "length", count)) {
                for (int32_t j = 0; j < count; j++) {
                    if (!kv->open_scope_fmt(j))
                        continue;

                    int32_t exf = 0;
                    kv->read("exf", exf);

                    std::string parts;
                    kv->read("parts", parts);

                    std::string root;
                    kv->read("root", root);

                    std::pair<std::string, std::string> key = { name, root };
                    auto elem = cat.find(key);
                    if (elem == cat.end())
                        elem = cat.insert({ key, {} }).first;

                    elem->second.exf = exf;
                    elem->second.parts = parse_parts_string(parts);
                    kv->close_scope();
                }
                kv->close_scope();
            }
            kv->close_scope();
        }
        kv->close_scope();
    }

    data_struct* aft_data = &data_list[DATA_AFT];
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    if (kv->read("obj", "length", count)) {
        for (int32_t i = 0; i < count; i++) {
            if (!kv->open_scope_fmt(i))
                continue;

            std::string cat;
            kv->read("cat", cat);

            std::string name;
            kv->read("name", name);

            obj.insert_or_assign(aft_obj_db->get_object_info(name.c_str()), cat);
            kv->close_scope();
        }
        kv->close_scope();
    }
}

rob_osage_parts osage_setting::parse_parts_string(std::string& s) {
    static const char* parts_string_array[] = {
        "LEFT", "RIGHT", "CENTER", "LONG_C", "SHORT_L", "SHORT_R",
        "APPEND_L", "APPEND_R", "MUFFLER", "WHITE_ONE_L", "PONY", "ANGEL_L", "ANGEL_R",
    };

    for (const char*& i : parts_string_array)
        if (!s.compare(i))
            return (rob_osage_parts)(&i - parts_string_array);
    return ROB_OSAGE_PARTS_NONE;
}

skin_param_file::skin_param_file() : file_handler(), id(), type(), data(), rob_chr() {

}

skin_param_file::~skin_param_file() {
    if (file_handler)
        delete file_handler;
}

static uint8_t skin_param_manager_index = 0;

SkinParamManager::SkinParamManager() {
    index = skin_param_manager_index++;
    state = 1;
}

SkinParamManager::~SkinParamManager() {
    state = 1;
    Reset();
}

bool SkinParamManager::Init() {
    state = 0;
    AddFiles();
    return true;
}

bool SkinParamManager::Ctrl() {
    if (state == 0) {
        if (CtrlFiles())
            return false;
        state = 1;
    }
    else if (state != 1)
        return false;
    return true;
}

bool SkinParamManager::Dest() {
    return true;
}

void SkinParamManager::Disp() {

}

void SkinParamManager::AddFiles() {
    if (!osage_init.size())
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    files.clear();
    for (osage_init_data& i : osage_init) {
        rob_chara_item_equip* rob_itm_equip = i.rob_chr->item_equip;
        for (int32_t j = ITEM_KAMI; j < ITEM_MAX; j++) {
            object_info obj_info = rob_itm_equip->get_object_info((item_id)j);
            if (obj_info.is_null())
                continue;

            const char* obj_name = aft_obj_db->get_object_name(obj_info);
            if (!obj_name)
                continue;

            obj_skin* skin = object_storage_get_obj_skin(obj_info);
            if (!skin || !skin->ex_data)
                return;

            obj_skin_ex_data* ex_data = skin->ex_data;
            int32_t osage_count = 0;
            int32_t cloth_count = 0;

            obj_skin_block* b = ex_data->block_array;
            for (int32_t k = ex_data->num_block; k; k--, b++)
                if (b->type == OBJ_SKIN_BLOCK_OSAGE)
                    osage_count++;
                else if (b->type == OBJ_SKIN_BLOCK_CLOTH)
                    cloth_count++;

            if (!ex_data || !(osage_count + cloth_count))
                continue;

            std::string path;
            std::string farc_file;
            std::string file;

            path.assign("rom/skin_param/");

            farc_file.assign("");

            file.assign("ext_skp_");
            file.append(obj_name);
            file.append(".txt");

            skin_param_file* skp_file = new skin_param_file;
            if (!skp_file)
                continue;

            skp_file->id = (item_id)j;
            skp_file->rob_chr = i.rob_chr;
            skp_file->type = -1;

            if (i.path.size()) {
                skp_file->type = GetExtSkpFile(i, i.path,
                    file, aft_data, aft_mot_db, aft_obj_db);
                if (skp_file->type != -1)
                    path.assign(i.path);
            }

            bool read_farc = false;
            if (skp_file->type == -1) {
                skp_file->type = sp_skp_db_data->get_ext_skp_file(i,
                    obj_info, file, farc_file, aft_data, aft_mot_db, aft_obj_db);
                if (skp_file->type == -1) {
                    delete skp_file;
                    continue;
                }

                switch (skp_file->type) {
                case 0:
                case 1:
                    read_farc = true;
                    break;
                }
            }

            std::vector<skin_param_file_data>* skp_file_data = 0;
            switch (skp_file->type) {
            case 0: {
                std::pair<object_info, std::pair<int32_t, int32_t>> v49 = { obj_info, { i.motion_id, i.frame } };
                auto elem = object_motion_frame.find(v49);
                if (elem != object_motion_frame.end()) {
                    delete skp_file;
                    continue;
                }

                skp_file_data = new std::vector<skin_param_file_data>;
                object_motion_frame.insert({ v49, skp_file_data });
            } break;
            case 1: {
                std::pair<object_info, std::pair<int32_t, int32_t>> v49 = { obj_info, { i.motion_id, -1 } };
                auto elem = object_motion.find(v49);
                if (elem != object_motion.end()) {
                    delete skp_file;
                    continue;
                }

                skp_file_data = new std::vector<skin_param_file_data>;
                object_motion.insert({ v49, skp_file_data });
            } break;
            case 2: {
                std::pair<object_info, std::pair<int32_t, int32_t>> v49 = { obj_info, { -1, -1 } };
                auto elem = object.find(v49);
                if (elem != object.end()) {
                    delete skp_file;
                    continue;
                }

                skp_file_data = new std::vector<skin_param_file_data>;
                object.insert({ v49, skp_file_data });
            } break;
            }

            if (!skp_file_data) {
                delete skp_file;
                continue;
            }

            skp_file_data->resize((size_t)cloth_count + osage_count);
            skp_file->data = skp_file_data;

            data_struct* aft_data = &data_list[DATA_AFT];

            p_file_handler* pfhndl = new p_file_handler;
            if (read_farc)
                pfhndl->read_file(aft_data, path.c_str(), farc_file.c_str(), file.c_str(), false);
            else
                pfhndl->read_file(aft_data, path.c_str(), file.c_str());
            skp_file->file_handler = pfhndl;
            files.push_back(skp_file);
        }
    }
}

bool SkinParamManager::AddTask(std::vector<osage_init_data>& vec) {
    if (app::TaskWork::CheckTaskReady(this) || !app::TaskWork::AddTask(this, "SKIN_PARAM_MANAGER"))
        return false;

    osage_init.assign(vec.begin(), vec.end());
    return true;
}

bool SkinParamManager::CtrlFiles() {
    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;

    for (auto i = files.begin(); i != files.end(); ) {
        skin_param_file* skp_file = *i;
        if (skp_file->file_handler->check_not_ready())
            break;
        else if (skp_file->id <= ITEM_NONE || skp_file->id >= ITEM_MAX) {
            i = files.erase(i);
            continue;
        }

        rob_chara_item_equip* rob_itm_equip = skp_file->rob_chr->item_equip;

        key_val kv;
        kv.parse(skp_file->file_handler->get_data(), skp_file->file_handler->get_size());

        object_info obj_info = rob_itm_equip->get_object_info(skp_file->id);
        if (!obj_info.not_null()) {
            i = files.erase(i);
            continue;
        }

        rob_chara_item_equip_object* itm_eq_obj = &rob_itm_equip->item_equip_object[skp_file->id];

        skin_param_file_data* skp_file_data = skp_file->data->data();
        for (ExNodeBlock* j : itm_eq_obj->node_blocks) {
            if (j->type != EX_OSAGE && j->type != EX_CLOTH)
                continue;

            const char* name = j->name;

            skin_param_osage_root root;
            skin_param_osage_root_parse(&kv, name, root, aft_bone_data);
            skp_file_data->skin_param.set_skin_param_osage_root(root);
            std::vector<skin_param_osage_node> vec;
            skin_param_osage_node_parse(&kv, name, &vec, root);
            skp_file_data->nodes_data.resize(vec.size());
            rob_itm_equip->skp_load(skp_file->id, root, vec, skp_file_data, aft_bone_data);
            skp_file_data++;
        }
        delete skp_file;

        i = files.erase(i);
    }
    return !!files.size();
}

int32_t SkinParamManager::GetExtSkpFile(const osage_init_data& osage_init, const std::string& path,
    std::string& file, void* data, motion_database* mot_db, object_database* obj_db) {
    if (!path.size())
        return -1;

    char buf0[0x200];
    char buf1[0x200];
    char buf2[0x200];
    size_t buf0_size = sizeof(buf0);
    size_t buf1_size = sizeof(buf1);
    size_t buf2_size = sizeof(buf2);

    if (osage_init.motion_id != -1) {
        const char* motion_name = mot_db->get_motion_name(osage_init.motion_id);
        if (!motion_name)
            return -1;

        if (osage_init.frame > -1) {
            strcpy_s(buf0, buf0_size, motion_name);
            sprintf_s(buf1, buf1_size, "_%d", osage_init.frame);
            strcat_s(buf0, buf0_size, buf1);
            motion_name = buf0;
        }

        strcpy_s(buf1, buf1_size, motion_name);
        strcat_s(buf1, buf1_size, "_");
        strcat_s(buf1, buf1_size, file.c_str());
        strcpy_s(buf0, buf0_size, buf1);

        for (char* i = buf0; *i; i++) {
            char c = *i;
            if (c >= 'A' && c <= 'Z')
                c += 0x20;
            *i = c;
        }

        if (((data_struct*)data)->check_file_exists(path.c_str(), buf0)) {
            file.assign(buf0);
            return 0;
        }
    }

    if (osage_init.pv_id > -1) {
        sprintf_s(buf1, buf1_size, "pv%03d_", osage_init.pv_id);
        strcat_s(buf1, buf1_size, file.c_str());
        strcpy_s(buf0, buf0_size, buf1);

        for (char* i = buf0; *i; i++) {
            char c = *i;
            if (c >= 'A' && c <= 'Z')
                c += 0x20;
            *i = c;
        }

        if (((data_struct*)data)->check_file_exists(path.c_str(), buf0)) {
            file.assign(buf0);
            return 1;
        }
    }

    if (((data_struct*)data)->check_file_exists(path.c_str(), file.c_str()))
        return 2;

    return -1;
}

std::vector<skin_param_file_data>* SkinParamManager::GetSkinParamFileData(
    object_info obj_info, int32_t motion_id, int32_t frame) {
    if (object_motion_frame.size()) {
        auto elem = object_motion_frame.find({ obj_info, { motion_id, frame } });
        if (elem != object_motion_frame.end())
            return elem->second;
    }

    if (object_motion.size()) {
        auto elem = object_motion.find({ obj_info, { motion_id, -1 } });
        if (elem != object_motion.end())
            return elem->second;
    }

    if (object.size()) {
        auto elem = object.find({ obj_info, { -1, -1 } });
        if (elem != object.end())
            return elem->second;
    }
    return 0;
}

void SkinParamManager::Reset() {
    osage_init.clear();

    for (auto i : object_motion_frame) {
        if (!i.second)
            continue;

        delete i.second;
        i.second = 0;
    }
    object_motion_frame.clear();

    for (auto i : object_motion) {
        if (!i.second)
            continue;

        delete i.second;
        i.second = 0;
    }
    object_motion.clear();

    for (auto i : object) {
        if (!i.second)
            continue;

        delete i.second;
        i.second = 0;
    }
    object.clear();

    for (skin_param_file*& i : files) {
        if (i->file_handler) {
            delete i->file_handler;
            i->file_handler = 0;
        }
        delete i;
    }
    files.clear();

    DelTask();
}

sp_skp_db::sp_skp_db() : farc_list_count() {

}

sp_skp_db::~sp_skp_db() {

}

void sp_skp_db::clear() {
    pv.clear();
    motion.clear();
    farc_list.clear();
    farc_list.shrink_to_fit();
    farc_list_count = 0;
}

void sp_skp_db::load() {
    data_struct* aft_data = &data_list[DATA_AFT];
    for (const std::string& i : mdata_manager_get()->GetPrefixes()) {
        std::string file;
        file.assign(i);
        file.append("sp_skp_db.txt");
        aft_data->load_file(this, "rom/skin_param/",
            file.c_str(), sp_skp_db_load_file);
    }
}

void sp_skp_db::parse(key_val* kv) {
    data_struct* aft_data = &data_list[DATA_AFT];
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    size_t farc_list_count = this->farc_list_count;

    int32_t count;
    if (kv->read("motion", "length", count)) {
        for (int32_t i = 0; i < count; i++) {
            if (!kv->open_scope_fmt(i))
                continue;

            std::string name;
            if (!kv->read("name", name))
                continue;

            std::shared_ptr<std::map<object_info, uint32_t>> skp
                = std::make_shared< std::map<object_info, uint32_t>>();

            motion.insert({ name, skp });

            int32_t count;
            if (kv->read("skp", "length", count)) {
                for (int32_t j = 0; j < count; j++) {
                    if (!kv->open_scope_fmt(j))
                        continue;

                    const char* obj;
                    int32_t farc;
                    if (!kv->read("obj", obj) || !kv->read("farc", farc))
                        continue;

                    skp->insert({ aft_obj_db->get_object_info(obj),
                        (uint32_t)(farc_list_count + farc) });
                    kv->close_scope();
                }
                kv->close_scope();
            }
            kv->close_scope();
        }
        kv->close_scope();
    }

    if (kv->read("pv", "length", count)) {
        for (int32_t i = 0; i < count; i++) {
            if (!kv->open_scope_fmt(i))
                continue;

            std::string name;
            if (!kv->read("name", name) || name.size() != 5)
                continue;

            std::shared_ptr<std::map<object_info, uint32_t>> skp
                = std::make_shared< std::map<object_info, uint32_t>>();

            int32_t pv_id = (name[2] - '0') * 100;
            pv_id += (name[3] - '0') * 10;
            pv_id += name[4] - '0';

            pv.insert({ pv_id, skp });

            int32_t count;
            if (kv->read("skp", "length", count)) {
                for (int32_t j = 0; j < count; j++) {
                    if (!kv->open_scope_fmt(j))
                        continue;

                    const char* obj;
                    int32_t farc;
                    if (!kv->read("obj", obj) || !kv->read("farc", farc))
                        continue;

                    skp->insert({ aft_obj_db->get_object_info(obj),
                        (uint32_t)(farc_list_count + farc) });
                    kv->close_scope();
                }
                kv->close_scope();
            }
            kv->close_scope();
        }
        kv->close_scope();
    }

    if (kv->read("farc_list", "length", count)) {
        farc_list.resize(farc_list_count + count);
        std::string* vfl = farc_list.data();
        for (int32_t i = 0; i < count; i++) {
            if (!kv->open_scope_fmt(i))
                continue;

            kv->read(vfl[farc_list_count + i]);
            kv->close_scope();
        }
        kv->close_scope();
        this->farc_list_count = farc_list_count + count;
    }
}

int32_t sp_skp_db::get_ext_skp_file(const osage_init_data& osage_init,
    const object_info obj_info, std::string& file, std::string& farc,
    void* data, motion_database* mot_db, object_database* obj_db) {
    if (obj_info.is_null())
        return 2;

    int32_t type = 2;
    farc.clear();
    if (osage_init.pv_id > -1) {
        auto elem0 = pv.find(osage_init.pv_id);
        if (elem0 != pv.end()) {
            auto elem1 = elem0->second->find(obj_info);
            if (elem1 != elem0->second->end()) {
                type = 1;
                farc.assign(farc_list[elem1->second]);
            }
        }
    }

    char buf0[0x200];
    char buf1[0x200];
    char buf2[0x200];
    size_t buf0_size = sizeof(buf0);
    size_t buf1_size = sizeof(buf1);
    size_t buf2_size = sizeof(buf2);

    if (osage_init.motion_id != -1) {
        const char* motion_name = mot_db->get_motion_name(osage_init.motion_id);
        if (motion_name) {
            if (osage_init.frame > -1) {
                strcpy_s(buf0, buf0_size, motion_name);
                sprintf_s(buf1, buf1_size, "_%d", osage_init.frame);
                strcat_s(buf0, buf0_size, buf1);
                motion_name = buf0;
            }

            auto elem0 = motion.find(motion_name);
            if (elem0 != motion.end()) {
                auto elem1 = elem0->second->find(obj_info);
                if (elem1 != elem0->second->end()) {
                    type = 0;
                    farc.assign(farc_list[elem1->second]);
                }
            }
        }
    }

    const char* obj_name = obj_db->get_object_name(obj_info);
    sprintf_s(buf0, buf0_size, "ext_skp_%s.txt", obj_name);

    if (type == 1) {
        sprintf_s(buf1, buf1_size, "pv%03d_", osage_init.pv_id);
        strcat_s(buf1, buf1_size, buf0);
        strcpy_s(buf0, buf0_size, buf1);
    }
    else if (!type) {
        const char* motion_name = mot_db->get_motion_name(osage_init.motion_id);
        if (osage_init.frame > -1) {
            strcpy_s(buf2, buf2_size, motion_name);
            sprintf_s(buf1, buf1_size, "_%d", osage_init.frame);
            strcat_s(buf2, buf2_size, buf1);
            motion_name = buf2;
        }

        strcpy_s(buf1, buf1_size, motion_name);
        strcat_s(buf1, buf1_size, "_");
        strcat_s(buf1, buf1_size, buf0);
        strcpy_s(buf0, buf0_size, buf1);
    }

    for (char* i = buf0; *i; i++) {
        char c = *i;
        if (c >= 'A' && c <= 'Z')
            c += 0x20;
        *i = c;
    }

    if (type == 2 && !((data_struct*)data)->check_file_exists("rom/skin_param/", buf0)) {
        file.clear();
        type = -1;
    }
    else
        file.assign(buf0);
    return type;
}

static bool osage_setting_data_load_file(void* data, const char* path, const char* file, uint32_t hash) {
    key_val kv;
    if (key_val::load_file(&kv, path, file, hash)) {
        ((osage_setting*)data)->parse(&kv);
        return true;
    }
    return false;
}

static SkinParamManager* skin_param_manager_get(int32_t chara_id) {
    if (chara_id < 0 || chara_id > 6)
        chara_id = 0;
    return &skin_param_manager[chara_id];
}

static bool sp_skp_db_load_file(void* data, const char* path, const char* file, uint32_t hash) {
    key_val kv;
    if (key_val::load_file(&kv, path, file, hash)) {
        ((sp_skp_db*)data)->parse(&kv);
        return true;
    }
    return false;
}
