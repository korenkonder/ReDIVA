/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include <map>
#include <vector>
#include "auth_2d.hpp"
#include "../KKdLib/str_utils.hpp"
#include "data.hpp"
#include "file_handler.hpp"
#include "sound.hpp"
#include "task.hpp"

class AetObj {
public:
    aet_info info;
    const aet_scene* scene;
    const aet_comp* comp;
    const aet_layer* layer;
    float_t start_time;
    float_t end_time;
    AetFlags flags;
    int32_t index;
    int32_t layer_index;
    spr::SprPrio prio;
    resolution_mode src_mode;
    vec3 pos;
    vec3 rot;
    vec3 scale;
    vec3 anchor;
    float_t frame_speed;
    vec4 color;
    float_t frame;
    resolution_mode dst_mode;
    vec2 scale_size;
    bool matte;
    spr::SprArgs spr_args;
    float_t opacity;
    std::map<std::string, int32_t> layer_sprite;
    std::string sound_path;
    std::map<std::string, std::string> sound_replace;
    int32_t sound_queue_index;
    std::map<uint32_t, uint32_t> sprite_replace;
    std::map<uint32_t, texture*> sprite_texture;
    std::map<uint32_t, uint32_t> sprite_discard;
    FrameRateControl* frame_rate_control;
    bool sound_voice;
    bool use_float;

    const sprite_database* spr_db;

    AetObj();
    virtual ~AetObj();

    virtual void Init(AetArgs& args, const aet_scene* scene, const aet_comp* comp, const aet_layer* layer,
        const aet_marker* start_marker, const aet_marker* end_marker, resolution_mode mode);
    virtual void Ctrl();
    virtual void Disp();
    virtual bool StepFrame();
    virtual void SetPlay(bool value);
    virtual void SetVisible(bool value);
    virtual void SetPosition(const vec3& value);
    virtual void SetScale(const vec3& value);
    virtual void SetRotation(const vec3& value);
    virtual void SetAlpha(float_t value);
    virtual void SetSpriteReplace(std::map<uint32_t, uint32_t>& value);
    virtual void SetSpriteTexture(std::map<uint32_t, texture*>& value);
    virtual void SetSpriteDiscard(std::map<uint32_t, uint32_t>& value);
    virtual void SetColor(const vec4& value);
    virtual void SetEndTime(float_t value);
    virtual void SetFrame(float_t value);
    virtual void SetFrameRateControl(FrameRateControl* value);
    virtual void SetPrio(spr::SprPrio value);
    virtual aet_info GetInfo();
    virtual bool GetPlay();
    virtual bool GetVisible();
    virtual float_t GetFrame();
    virtual bool GetEnd();
    virtual void CtrlComp(const aet_comp* comp, float_t frame);
    virtual void DispComp(const mat4& mat, const aet_comp* comp,
        float_t frame, float_t opacity, const sprite_database* spr_db);
    virtual void CtrlLayer(const aet_layer* layer, float_t frame);
    virtual void DispLayer(const mat4& mat, const aet_layer* layer,
        float_t frame, float_t opacity, const sprite_database* spr_db);
    virtual void DispVideo(const mat4& mat, const aet_layer* layer,
        float_t frame, float_t opacity, const sprite_database* spr_db);
    virtual void DispSprite(const mat4& mat, const aet_layer* layer,
        float_t opacity, const sprite_database* spr_db);
    virtual void DispSpriteSource(const mat4& mat, const aet_layer* layer,
        uint32_t source_index, float_t opacity, const sprite_database* spr_db);
    virtual void CalcMat(mat4& mat, const aet_layer* layer, float_t frame);
};

class AetLyo : public AetObj {
public:
    AetComp* comp;

    AetLyo(AetComp* comp);
    virtual ~AetLyo() override;

    virtual void CtrlLayer(const aet_layer* layer, float_t frame) override;
    virtual void DispLayer(const mat4& mat, const aet_layer* layer,
        float_t frame, float_t opacity, const sprite_database* spr_db) override;
};

class AetMgr : public app::Task {
public:
    std::map<uint32_t, AetSet> sets;
    std::map<uint32_t, AetObj> objects;
    std::list<std::map<uint32_t, AetObj>::iterator> free_objects;
    uint32_t load_counter;

    uint32_t set_counter;

    AetMgr();
    virtual ~AetMgr();

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void disp() override;
    virtual void basic() override;

    uint32_t AddSetModern();
    void AddAetSets(const aet_database* aet_db);
    void FreeAetObject(uint32_t id);
    void FreeAetObject(std::map<uint32_t, AetObj>::iterator it);
    void FreeAetSetObjects(uint32_t set_index);
    AetObj* GetObj(uint32_t id);
    bool GetObjEnd(uint32_t id);
    float_t GetObjFrame(uint32_t id);
    bool GetObjVisible(uint32_t id);
    AetSet* GetSet(uint32_t index);
    bool GetSetReady(uint32_t index);
    void GetSetSceneCompLayerNames(std::vector<std::string>& vec, aet_info info);
    float_t GetSetSceneEndTime(aet_info info);
    float_t GetSetSceneLayerEndTime(aet_info info, const char* layer_name);
    void GetSetSceneLayerMarkerNames(std::vector<std::string>& vec, aet_info info, const char* layer_name);
    float_t GetSetSceneLayerMarkerTime(aet_info info, const char* scene_name, const char* marker_name);
    float_t GetSetSceneLayerStartTime(aet_info info, const char* layer_name);
    const char* GetSetSceneName(aet_info info);
    float_t GetSetSceneStartTime(aet_info info);
    uint32_t GetSetScenesCount(uint32_t index);
    void InitAetLayout(AetComp* comp, AetArgs& args);
    bool InitAetObj(AetObj& obj, AetArgs& args);
    uint32_t InitAetObject(AetArgs& args);
    bool LoadFile(uint32_t index);
    bool LoadFileModern(uint32_t index, aet_database* aet_db);
    void ReadFile(uint32_t index, const char* file, std::string& mdata_dir, void* data);
    void ReadFileModern(uint32_t index, uint32_t set_hash, void* data, aet_database* aet_db);
    void RemoveAetSets(const aet_database* aet_db);
    void SetObjAlpha(uint32_t id, float_t value);
    void SetObjColor(uint32_t id, const vec4& value);
    void SetObjEndTime(uint32_t id, float_t value);
    void SetObjFrame(uint32_t id, float_t value);
    void SetObjFrameRateControl(uint32_t id, FrameRateControl* value);
    void SetObjPlay(uint32_t id, bool value);
    void SetObjPosition(uint32_t id, const vec3& value);
    void SetObjPrio(uint32_t id, spr::SprPrio value);
    void SetObjRotation(uint32_t id, const vec3& value);
    void SetObjScale(uint32_t id, const vec3& value);
    void SetObjSpriteDiscard(uint32_t id, std::map<uint32_t, uint32_t>& value);
    void SetObjSpriteReplace(uint32_t id, std::map<uint32_t, uint32_t>& value);
    void SetObjSpriteTexture(uint32_t id, std::map<uint32_t, texture*>& value);
    void SetObjVisible(uint32_t id, bool value);
    void UnloadSet(uint32_t index);
    void UnloadSetModern(uint32_t index, aet_database* aet_db);
};

AetMgr* aet_manager;

AetArgs::AetArgs() : layer_name(), start_marker(), end_marker(), flags(),
index(), layer(), frame_rate_control(), sound_voice(), spr_db() {
    start_time = -1.0f;
    end_time = -1.0f;
    prio = spr::SPR_PRIO_DEFAULT;
    mode = RESOLUTION_MODE_HD;
    scale = 1.0f;
    frame_speed = 1.0f;
    color = 1.0f;
    sound_queue_index = 1;
}

AetArgs::~AetArgs() {

}

aet_layout_data::aet_layout_data() : width(), height() {
    mat = mat4_identity;
    opacity = 1.0f;
    color = 0xFFFFFFFF;
    mode = RESOLUTION_MODE_HD;
}

void aet_layout_data::put_sprite(int32_t spr_id, spr::SprAttr attr, spr::SprPrio prio,
    const vec2* pos, const aet_layout_data* layout, const sprite_database* spr_db) {
    if (!layout)
        return;

    spr::SprArgs args;
    aet_layout_data::set_args(layout, &args);
    args.id.id = spr_id;
    args.attr = attr;
    args.prio = prio;
    if (pos)
        *(vec2*)&args.trans.x += *pos;
    spr::put_sprite(args, spr_db);
}

void aet_layout_data::set_args(const aet_layout_data* layout, spr::SprArgs* args) {
    if (!layout || !args)
        return;

    args->trans = layout->position;
    args->center = layout->anchor;
    args->scale.x = layout->mat.row0.x;
    args->scale.y = layout->mat.row1.y;
    args->scale.z = layout->mat.row2.z;
}

AetComp::AetComp() {

}

AetComp::~AetComp() {

}

void AetComp::Add(const char* name, const aet_layout_data& data) {
    this->data.insert({ name, data });
}

void AetComp::Clear() {
    data.clear();
}

const aet_layout_data* AetComp::Find(const char* name) {
    auto elem = data.find(name);
    if (elem != data.end())
        return &elem->second;
    return 0;
}

void AetComp::put_number_sprite(int32_t value, int32_t max_digits,
    AetComp* comp, const char** names, const int32_t* spr_ids, spr::SprPrio prio,
    const vec2* pos, bool all_digits, const sprite_database* spr_db) {
    if (!comp || !names || !spr_ids)
        return;

    for (int32_t i = 0; i < max_digits; i++) {
        int32_t digit = value % 10;
        value /= 10;

        const aet_layout_data* layout = comp->Find(names[i]);
        if (layout)
            aet_layout_data::put_sprite(spr_ids[digit],
                spr::SPR_ATTR_CTR_CC, prio, pos, layout, spr_db);

        if (!all_digits && !value)
            break;
    }
}

AetSet::AetSet(uint32_t index) : aet_set(), load_count(), ready() {
    this->index = index;
    hash = hash_murmurhash_empty;
}

AetSet::~AetSet() {

}

void AetSet::ReadFile(const char* dir, const char* file, void* data) {
    if (load_count > 1) {
        load_count++;
        return;
    }

    load_count = 1;
    file_handler.read_file(data, dir, file);
    ready = false;
    aet_set = 0;
}

bool AetSet::LoadFile() {
    if (ready)
        return false;
    else if (file_handler.check_not_ready())
        return true;
    else if (!Load())
        return false;

    ready = true;
    return true;
}

void AetSet::Unload() {
    if (load_count > 1) {
        load_count--;
        return;
    }

    alloc_handler.reset();
    file_handler.reset();
    aet_set = 0;

    ready = false;
    load_count = 0;
}

bool AetSet::Load() {
    const void* data = file_handler.get_data();
    size_t size = file_handler.get_size();
    if (!data || !size)
        return false;

    prj::shared_ptr<prj::stack_allocator>& alloc = alloc_handler;
    alloc = prj::shared_ptr<prj::stack_allocator>(new prj::stack_allocator);

    ::aet_set* set = alloc->allocate<::aet_set>();
    this->aet_set = set;
    set->unpack_file(alloc_handler, data, size, false);
    if (!set->ready)
        return false;

    file_handler.reset();
    return true;
}

void AetSet::ReadFileModern(uint32_t set_hash, void* data) {
    if (load_count > 1) {
        load_count++;
        return;
    }

    std::string file;
    if (((data_struct*)data)->get_file("root+/2d/", set_hash, ".farc", file))
        file_handler.read_file(data, "root+/2d/", file.c_str());
    this->hash = set_hash;
    ready = false;
    aet_set = 0;
}

bool AetSet::LoadFileModern(aet_database* aet_db) {
    if (ready)
        return false;
    else if (file_handler.check_not_ready())
        return true;
    else if (!LoadModern(aet_db))
        return false;

    ready = true;
    return true;
}

void AetSet::UnloadModern(aet_database* aet_db) {
    if (load_count > 1) {
        load_count--;
        return;
    }

    aet_db->remove_aet_set(hash, index, name.c_str(), aet_ids);

    if (load_count > 1) {
        load_count--;
        return;
    }

    alloc_handler.reset();
    file_handler.reset();
    aet_set = 0;

    ready = false;
    load_count = 0;

    hash = hash_murmurhash_empty;
    name.clear();
    aet_ids.clear();
}

bool AetSet::LoadModern(aet_database* aet_db) {
    if (ready)
        return false;
    else if (file_handler.check_not_ready())
        return true;

    const void* data = file_handler.get_data();
    size_t size = file_handler.get_size();
    if (!data || !size)
        return false;

    farc f;
    f.read(data, size, true);

    std::string& file = file_handler.ptr->file;

    size_t file_len = file.size();
    if (file_len >= 0x100 - 4)
        return false;

    const char* t = strrchr(file.c_str(), '.');
    if (t)
        file_len = t - file.c_str();

    char buf[0x100];
    memcpy(buf, file.c_str(), file_len);
    char* ext = buf + file_len;
    size_t ext_len = sizeof(buf) - file_len;

    memcpy_s(ext, ext_len, ".aec", 5);
    farc_file* aec = f.read_file(buf);
    if (!aec)
        return false;

    memcpy_s(ext, ext_len, ".aei", 5);
    farc_file* aei = f.read_file(buf);
    if (!aei)
        return false;

    aet_database_file aet_db_file;
    aet_db_file.read(aei->data, aei->size, true);

    aet_db_aet_set_file* aet_set_file = 0;
    if (aet_db_file.ready)
        for (aet_db_aet_set_file& m : aet_db_file.aet_set)
            if (m.id == hash) {
                aet_set_file = &m;
                break;
            }

    if (!aet_set_file)
        return false;

    if (aet_db_file.ready)
        aet_db->parse(aet_set_file, name, aet_ids);

    prj::shared_ptr<prj::stack_allocator>& alloc = alloc_handler;
    alloc = prj::shared_ptr<prj::stack_allocator>(new prj::stack_allocator);

    ::aet_set* set = alloc->allocate<::aet_set>();
    this->aet_set = set;
    set->unpack_file(alloc_handler, aec->data, aec->size, true);
    if (!set->ready)
        return false;

    file_handler.reset();
    return true;
}

const aet_marker* AetSet::GetLayerMarker(const aet_layer* layer, const char* marker_name) {
    if (!layer->markers_count)
        return 0;

    const aet_marker* marker = layer->markers;
    for (uint32_t i = layer->markers_count; i; i--, marker++)
        if (!str_utils_compare(marker->name, marker_name))
            return marker;
    return 0;
}

void AetSet::GetLayerMarkerNames(std::vector<std::string>& vec, const aet_layer* layer) {
    vec.reserve(layer->markers_count);
    const aet_marker* marker = layer->markers;
    for (uint32_t i = layer->markers_count; i; i--, marker++)
        vec.push_back(marker->name);
}

const aet_scene* AetSet::GetSceneByInfo(aet_info info) {
    if (index == info.set_index && ready && info.index < aet_set->scenes_count)
        return aet_set->scenes[info.index];
    return 0;
}

void AetSet::GetSceneCompLayerNames(std::vector<std::string>& vec, const aet_scene* scene) {
    const aet_comp* comp = scene->comp;
    for (uint32_t i = scene->comp_count; i; i--, comp++) {
        vec.reserve(comp->layers_count);
        const aet_layer* layer = comp->layers;
        for (uint32_t j = comp->layers_count; j; j--, layer++)
            vec.push_back(layer->name);
    }
}

float_t AetSet::GetSceneEndTime(uint16_t index) {
    return aet_set->scenes[index]->end_time;
}

const aet_layer* AetSet::GetSceneLayer(const aet_scene* scene, const char* layer_name) {
    const aet_layer* ret = 0;
    const aet_comp* comp = scene->comp;
    for (uint32_t i = scene->comp_count; i; i--, comp++) {
        const aet_layer* layer = comp->layers;
        for (uint32_t j = comp->layers_count; j; j--, layer++)
            if (!str_utils_compare(layer->name, layer_name)) {
                ret = layer;
                break;
            }
    }
    return ret;
}

const char* AetSet::GetSceneName(uint16_t index) {
    return aet_set->scenes[index]->name;
}

resolution_mode AetSet::GetSceneResolutionMode(const aet_scene* scene) {
    for (int32_t i = RESOLUTION_MODE_QVGA; i < RESOLUTION_MODE_MAX; i++) {
        resolution_struct v5((resolution_mode)i);
        if (scene->width == v5.width && scene->height == v5.height)
            return (resolution_mode)i;
    }
    return RESOLUTION_MODE_HD;
}

float_t AetSet::GetSceneStartTime(uint16_t index) {
    return aet_set->scenes[index]->start_time;
}

uint32_t AetSet::GetScenesCount() {
    return aet_set->scenes_count;
}

void aet_manager_init() {
    if (!aet_manager)
        aet_manager = new AetMgr;
}

void aet_manager_add_aet_sets(const aet_database* aet_db) {
    aet_manager->AddAetSets(aet_db);
}

bool aet_manager_add_task() {
    return app::TaskWork::add_task(aet_manager, "2DAUTH_TASK", 2);
}

bool aet_manager_check_task_ready() {
    return app::TaskWork::check_task_ready(aet_manager);
}

bool aet_manager_del_task() {
    return aet_manager->del();
}

void aet_manager_free_aet_object(uint32_t id) {
    aet_manager->FreeAetObject(id);
}

void aet_manager_free_aet_object_reset(uint32_t* id) {
    if (id && *id) {
        aet_manager_free_aet_object(*id);
        *id = 0;
    }
}

void aet_manager_free_aet_set_objects(uint32_t set_id, const aet_database* aet_db) {
    aet_manager->FreeAetSetObjects(aet_db->get_aet_set_by_id(set_id)->index);
}

AetSet* aet_manager_get_aet_set(uint32_t set_id, const aet_database* aet_db) {
    return aet_manager->GetSet(aet_db->get_aet_set_by_id(set_id)->index);
}

bool aet_manager_get_obj_end(uint32_t id) {
    return aet_manager->GetObjEnd(id);
}

float_t aet_manager_get_obj_frame(uint32_t id) {
    return aet_manager->GetObjFrame(id);
}

bool aet_manager_get_obj_visible(uint32_t id) {
    return aet_manager->GetObjVisible(id);
}

void aet_manager_get_scene_comp_layer_names(std::vector<std::string>& vec,
    uint32_t aet_id, const aet_database* aet_db) {
    return aet_manager->GetSetSceneCompLayerNames(vec, aet_db->get_aet_by_id(aet_id)->info);
}

float_t aet_manager_get_scene_end_time(uint32_t aet_id, const aet_database* aet_db) {
    return aet_manager->GetSetSceneEndTime(aet_db->get_aet_by_id(aet_id)->info);
}

float_t aet_manager_get_scene_layer_end_time(uint32_t aet_id,
    const char* layer_name, const aet_database* aet_db) {
    return aet_manager->GetSetSceneLayerEndTime(aet_db->get_aet_by_id(aet_id)->info, layer_name);
}

void aet_manager_get_scene_layer_marker_names(std::vector<std::string>& vec,
    uint32_t aet_id, const char* layer_name, const aet_database* aet_db) {
    aet_manager->GetSetSceneLayerMarkerNames(vec, aet_db->get_aet_by_id(aet_id)->info, layer_name);
}

float_t aet_manager_get_scene_layer_marker_time(uint32_t aet_id,
    const char* scene_name, const char* marker_name, const aet_database* aet_db) {
    return aet_manager->GetSetSceneLayerMarkerTime(
        aet_db->get_aet_by_id(aet_id)->info, scene_name, marker_name);
}

float_t aet_manager_get_scene_layer_start_time(uint32_t aet_id,
    const char* layer_name, const aet_database* aet_db) {
    return aet_manager->GetSetSceneLayerStartTime(aet_db->get_aet_by_id(aet_id)->info, layer_name);
}

const char* aet_manager_get_scene_name(uint32_t aet_id, const aet_database* aet_db) {
    return aet_manager->GetSetSceneName(aet_db->get_aet_by_id(aet_id)->info);
}

float_t aet_manager_get_scene_start_time(uint32_t aet_id, const aet_database* aet_db) {
    return aet_manager->GetSetSceneStartTime(aet_db->get_aet_by_id(aet_id)->info);
}

uint32_t aet_manager_get_scenes_count(uint32_t set_id, const aet_database* aet_db) {
    return aet_manager->GetSetScenesCount(aet_db->get_aet_set_by_id(set_id)->index);
}

::aet_set* aet_manager_get_set(uint32_t set_id, const aet_database* aet_db) {
    AetSet* set = aet_manager->GetSet(aet_db->get_aet_set_by_id(set_id)->index);
    if (set)
        return set->aet_set;
    return 0;
}

bool aet_manager_get_set_ready(uint32_t set_id, const aet_database* aet_db) {
    return aet_manager->GetSetReady(aet_db->get_aet_set_by_id(set_id)->index);
};

void aet_manager_init_aet_layout(AetComp* comp, AetArgs args, const aet_database* aet_db) {
    args.id.info = aet_db->get_aet_by_id(args.id.id)->info;
    aet_manager->InitAetLayout(comp, args);
}

void aet_manager_init_aet_layout(AetComp* comp, uint32_t aet_id, const char* layer_name,
    AetFlags flags, resolution_mode mode, const char* start_marker, float_t start_time,
    const aet_database* aet_db, const sprite_database* spr_db) {
    AetArgs args;
    args.id.info = aet_db->get_aet_by_id(aet_id)->info;
    args.layer_name = layer_name;
    args.flags = flags;
    args.mode = mode;
    args.start_marker = start_marker;
    args.start_time = start_time;
    args.spr_db = spr_db;
    aet_manager->InitAetLayout(comp, args);
}

uint32_t aet_manager_init_aet_object(AetArgs args, const aet_database* aet_db) {
    args.id.info = aet_db->get_aet_by_id(args.id.id)->info;
    return aet_manager->InitAetObject(args);
}

uint32_t aet_manager_init_aet_object(uint32_t id, const char* layer_name,
    spr::SprPrio prio, AetFlags flags, const char* start_marker, const char* end_marker,
    const aet_database* aet_db, const sprite_database* spr_db) {
    AetArgs args;
    args.id.info = aet_db->get_aet_by_id(id)->info;
    args.layer_name = layer_name;
    args.prio = prio;
    args.flags = flags;
    args.start_marker = start_marker;
    args.end_marker = end_marker;
    args.spr_db = spr_db;
    return aet_manager->InitAetObject(args);
}

uint32_t aet_manager_init_aet_object(uint32_t aet_id, spr::SprPrio prio, AetFlags flags,
    const char* layer_name, const vec2* pos, int32_t index, const char* start_marker, const char* end_marker,
    float_t start_time, float_t end_time, const vec2* scale, FrameRateControl* frame_rate_control,
    const aet_database* aet_db, const sprite_database* spr_db) {
    AetArgs args;
    args.id.id = aet_id;
    args.prio = prio;
    enum_or(args.flags, flags);
    args.layer_name = layer_name;
    args.mode = RESOLUTION_MODE_HD;
    args.layer = 0;
    args.index = index;
    args.start_marker = start_marker;
    args.end_marker = end_marker;

    if (pos) {
        args.pos.x = pos->x;
        args.pos.y = pos->y;
    }

    if (scale) {
        args.scale.x = scale->x;
        args.scale.y = scale->y;
    }

    if (start_time >= 0.0f)
        args.start_time = start_time;

    if (end_time >= 0.0f)
        args.end_time = end_time;

    if (frame_rate_control)
        args.frame_rate_control = frame_rate_control;

    args.spr_db = spr_db;
    return aet_manager_init_aet_object(args, aet_db);
}

bool aet_manager_load_file(uint32_t set_id, const aet_database* aet_db) {
    return aet_manager->LoadFile(aet_db->get_aet_set_by_id(set_id)->index);
}

bool aet_manager_load_file_modern(uint32_t set_hash, aet_database* aet_db) {
    return aet_manager->LoadFileModern(aet_db->get_aet_set_by_id(set_hash)->index, aet_db);
}

void aet_manager_read_file(uint32_t set_id,
    std::string& mdata_dir, void* data, const aet_database* aet_db) {
    const aet_db_aet_set* set = aet_db->get_aet_set_by_id(set_id);
    aet_manager->ReadFile(set->index, set->file_name.c_str(), mdata_dir, data);
}

void aet_manager_read_file(uint32_t set_id,
    std::string&& mdata_dir, void* data, const aet_database* aet_db) {
    const aet_db_aet_set* set = aet_db->get_aet_set_by_id(set_id);
    aet_manager->ReadFile(set->index, set->file_name.c_str(), mdata_dir, data);
}

void aet_manager_read_file_modern(uint32_t set_hash, void* data, aet_database* aet_db) {
    const aet_db_aet_set* aet_set = aet_db->get_aet_set_by_id(set_hash);
    aet_manager->ReadFileModern(aet_set->index, set_hash, data, aet_db);
}

void aet_manager_remove_aet_sets(const aet_database* aet_db) {
    aet_manager->RemoveAetSets(aet_db);
}

void aet_manager_set_obj_alpha(uint32_t id, float_t value) {
    aet_manager->SetObjAlpha(id, value);
}

void aet_manager_set_obj_color(uint32_t id, const vec4& value) {
    aet_manager->SetObjColor(id, value);
}

void aet_manager_set_obj_end_time(uint32_t id, float_t value) {
    aet_manager->SetObjEndTime(id, value);
}

void aet_manager_set_obj_frame(uint32_t id, float_t value) {
    aet_manager->SetObjFrame(id, value);
}

void aet_manager_set_obj_frame_rate_control(uint32_t id, FrameRateControl* value) {
    aet_manager->SetObjFrameRateControl(id, value);
}

void aet_manager_set_obj_play(uint32_t id, bool value) {
    aet_manager->SetObjPlay(id, value);
}

void aet_manager_set_obj_position(uint32_t id, const vec3& value) {
    aet_manager->SetObjPosition(id, value);
}

void aet_manager_set_obj_prio(uint32_t id, spr::SprPrio value) {
    aet_manager->SetObjPrio(id, value);
}

void aet_manager_set_obj_rotation(uint32_t id, const vec3& value) {
    aet_manager->SetObjRotation(id, value);
}

void aet_manager_set_obj_scale(uint32_t id, const vec3& value) {
    aet_manager->SetObjScale(id, value);
}

void aet_manager_set_obj_sprite_discard(uint32_t id, std::map<uint32_t, uint32_t>& value) {
    aet_manager->SetObjSpriteDiscard(id, value);
}

void aet_manager_set_obj_sprite_replace(uint32_t id, std::map<uint32_t, uint32_t>& value) {
    aet_manager->SetObjSpriteReplace(id, value);
}

void aet_manager_set_obj_sprite_texture(uint32_t id, std::map<uint32_t, texture*>& value) {
    aet_manager->SetObjSpriteTexture(id, value);
}

void aet_manager_set_obj_visible(uint32_t id, bool value) {
    aet_manager->SetObjVisible(id, value);
}

void aet_manager_unload_set(uint32_t set_id, const aet_database* aet_db) {
    aet_manager->UnloadSet(aet_db->get_aet_set_by_id(set_id)->index);
}

void aet_manager_unload_set_modern(uint32_t set_hash, aet_database* aet_db) {
    aet_manager->UnloadSetModern(aet_db->get_aet_set_by_id(set_hash)->index, aet_db);
}

void aet_manager_free() {
    if (aet_manager) {
        delete aet_manager;
        aet_manager = 0;
    }
}

AetObj::AetObj() : scene(), comp(), layer(), start_time(), end_time(),
flags(), layer_index(), frame_speed(), frame(), matte(), opacity(),
frame_rate_control(), sound_voice(), use_float(), spr_db() {
    index = -1;
    layer_index = 0;
    prio = spr::SPR_PRIO_03;
    src_mode = RESOLUTION_MODE_HD;
    scale = 1.0f;
    dst_mode = RESOLUTION_MODE_HD;
    sound_queue_index = 2;
}

AetObj::~AetObj() {

}

void AetObj::Init(AetArgs& args, const aet_scene* scene, const aet_comp* comp, const aet_layer* layer,
    const aet_marker* start_marker, const aet_marker* end_marker, resolution_mode mode) {
    start_time = scene->start_time;
    end_time = scene->end_time - 1.0f;

    if (layer) {
        start_time = layer->start_time;
        end_time = layer->end_time - 1.0f;
    }

    if (start_marker)
        start_time = start_marker->time;

    if (end_marker)
        end_time = end_marker->time;

    if (args.start_time >= 0.0f) {
        if (args.start_time < scene->start_time)
            start_time = scene->start_time;
        else if (scene->end_time >= args.start_time)
            start_time = args.start_time;
        else
            start_time = scene->end_time;
    }

    if (args.end_time >= 0.0f) {
        if (args.end_time < scene->start_time)
            end_time = scene->start_time;
        else if (scene->end_time >= args.end_time)
            end_time = args.end_time;
        else
            end_time = scene->end_time;
    }

    flags = args.flags;
    enum_and(flags, 0xFFFF0000);
    if (start_time > end_time) {
        float_t start_time = this->start_time;
        float_t end_time = this->end_time;
        this->start_time = end_time;
        this->end_time = start_time;
        enum_or(flags, AET_REVERSE);
    }

    if (flags & AET_REVERSE)
        frame = end_time;
    else
        frame = start_time;

    info = args.id.info;
    this->scene = scene;
    this->comp = comp;
    this->layer = layer;
    index = args.index;
    layer_index = args.layer;
    prio = args.prio;
    src_mode = args.mode;
    pos = args.pos;
    rot = args.rot;
    scale = args.scale;
    anchor = args.anchor;
    frame_speed = args.frame_speed;
    color = args.color;
    dst_mode = mode;
    scale_size.x = flags & AET_FLIP_H ? -1.0f : 1.0f;
    scale_size.y = flags & AET_FLIP_V ? -1.0f : 1.0f;
    matte = false;
    opacity = 0.0f;
    layer_sprite.clear();
    layer_sprite.insert(args.layer_sprite.begin(), args.layer_sprite.end());
    sound_path.assign(args.sound_path);
    sound_replace.clear();
    sound_replace.insert(args.sound_replace.begin(), args.sound_replace.end());
    sound_queue_index = args.sound_queue_index;
    sprite_replace.clear();
    sprite_replace.insert(args.sprite_replace.begin(), args.sprite_replace.end());
    sprite_texture.clear();
    sprite_texture.insert(args.sprite_texture.begin(), args.sprite_texture.end());
    sprite_discard.clear();
    sprite_discard.insert(args.sprite_discard.begin(), args.sprite_discard.end());
    frame_rate_control = args.frame_rate_control;
    sound_voice = args.sound_voice;
    spr_db = args.spr_db;

    if (args.layer_name) {
        const char* name = args.layer_name;
        size_t name_len = utf8_length(name);
        if (name_len >= 3 && !memcmp(name + name_len - 3, "__f", 3))
            use_float = true;
    }
}

void AetObj::Ctrl() {
    if (layer)
        CtrlLayer(layer, frame);
    else
        CtrlComp(comp, frame);
}

void AetObj::Disp() {
    if (flags & AET_HIDDEN || !scene)
        return;

    vec3 pos = this->pos;
    resolution_mode_scale_pos(*(vec2*)&pos, dst_mode, *(vec2*)&pos, src_mode);

    vec3 anchor = this->anchor;
    if (flags & AET_100000) {
        anchor.x = (float_t)scene->width * 0.5f;
        anchor.y = (float_t)scene->height * 0.5f;
        anchor.z = 0.0f;
    }

    mat4 mat;
    mat4_translate(&pos, &mat);
    mat4_mul_rotate_xyz(&mat, rot.x, rot.y, rot.z * scale_size.x * scale_size.y, &mat);
    mat4_scale_rot(&mat, scale.x * scale_size.x, scale.y * scale_size.y, scale.z, &mat);
    mat4_mul_translate(&mat, -anchor.x, -anchor.y, -anchor.z, &mat);

    const aet_camera* camera = scene->camera;
    if (camera) {
        float_t eye_x = camera->eye_x.interpolate(frame) - (float_t)scene->width * 0.5f;
        float_t eye_y = camera->eye_y.interpolate(frame) - (float_t)scene->height * 0.5f;
        float_t eye_z = camera->eye_z.interpolate(frame);
        mat4_mul_translate(&mat, -eye_x, -eye_y, -eye_z, &mat);
    }

    if (layer)
        DispLayer(mat, layer, frame, 1.0f, spr_db);
    else
        DispComp(mat, comp, frame, 1.0f, spr_db);
}

bool AetObj::StepFrame() {
    if (flags & AET_PAUSE)
        return false;

    float_t delta_frame = 1.0f;
    if (frame_rate_control)
        delta_frame = frame_rate_control->get_delta_frame();

    delta_frame = delta_frame * frame_speed * scene->fps * (float_t)(1.0 / 60.0);
    if (flags & AET_REVERSE)
        delta_frame = -delta_frame;
    frame += delta_frame;

    if (!(flags & AET_REVERSE)) {
        if (frame > end_time) {
            if (flags & AET_LOOP)
                frame = start_time;
            else if (flags & AET_PLAY_ONCE) {
                frame = end_time;
                enum_or(flags, AET_END);
            }
            else
                return true;
        }
        return false;
    }
    else {
        if (start_time > frame) {
            if (flags & AET_LOOP)
                frame = end_time;
            else if (flags & AET_PLAY_ONCE) {
                frame = start_time;
                enum_or(flags, AET_END);
            }
            else
                return true;
        }
        return false;
    }
}

void AetObj::SetPlay(bool value) {
    if (value)
        enum_and(flags, ~AET_PAUSE);
    else
        enum_or(flags, AET_PAUSE);
}

void AetObj::SetVisible(bool value) {
    if (value)
        enum_and(flags, ~AET_HIDDEN);
    else
        enum_or(flags, AET_HIDDEN);
}

void AetObj::SetPosition(const vec3& value) {
    pos = value;
}

void AetObj::SetScale(const vec3& value) {
    scale = value;
}

void AetObj::SetRotation(const vec3& value) {
    rot = value;
}

void AetObj::SetAlpha(float_t value) {
    color.w = value;
}

void AetObj::SetSpriteReplace(std::map<uint32_t, uint32_t>& value) {
    sprite_replace.insert(value.begin(), value.end());
}

void AetObj::SetSpriteTexture(std::map<uint32_t, texture*>& value) {
    sprite_texture.insert(value.begin(), value.end());
}

void AetObj::SetSpriteDiscard(std::map<uint32_t, uint32_t>& value) {
    sprite_discard.insert(value.begin(), value.end());
}

void AetObj::SetColor(const vec4& value) {
    color = value;
}

void AetObj::SetEndTime(float_t value) {
    end_time = value;
}

void AetObj::SetFrame(float_t value) {
    frame = value;
}

void AetObj::SetFrameRateControl(FrameRateControl* value) {
    frame_rate_control = value;
}

void AetObj::SetPrio(spr::SprPrio value) {
    prio = value;
}

aet_info AetObj::GetInfo() {
    return info;
}

bool AetObj::GetPlay() {
    return !(flags & AET_PAUSE);
}

bool AetObj::GetVisible() {
    return !(flags & AET_HIDDEN);
}

float_t AetObj::GetFrame() {
    return frame;
}

bool AetObj::GetEnd() {
    return !!(flags & AET_END);
}

void AetObj::CtrlComp(const aet_comp* comp, float_t frame) {
    if (!comp->layers_count)
        return;

    const aet_layer* layers = comp->layers;
    for (int64_t i = comp->layers_count - 1; i >= 0; i--)
       CtrlLayer(&layers[i], frame);
}

void AetObj::DispComp(const mat4& mat, const aet_comp* comp,
    float_t frame, float_t opacity, const sprite_database* spr_db) {
    if (!comp->layers_count)
        return;

    const aet_layer* layers = comp->layers;
    for (int64_t i = comp->layers_count - 1; i >= 0; i--)
        DispLayer(mat, &layers[i], frame, opacity, spr_db);
}

void AetObj::CtrlLayer(const aet_layer* layer, float_t frame) {
    if (frame < layer->start_time || frame >= layer->end_time || !layer->flags.audio_active)
        return;

    frame = (frame - layer->start_time) * layer->time_scale + layer->offset_time;
    if (layer->item_type != AET_ITEM_TYPE_AUDIO) {
        if (layer->item_type == AET_ITEM_TYPE_COMPOSITION)
            CtrlComp(layer->item.comp, frame);
        return;
    }

    if (frame != 0.0f)
        return;

    std::string file(layer->name);
    auto elem = sound_replace.find(file);
    if (elem != sound_replace.end())
        file.assign(elem->second);

    size_t aif_off = file.rfind(".aif");
    if (aif_off != -1 && aif_off == file.size() - 4) {
        file.assign(file.substr(0, aif_off));
        sound_work_play_se(sound_queue_index, file.c_str(), 1.0f);
        return;
    }

    size_t mp3_off = file.rfind(".mp3");
    if (mp3_off != -1 && mp3_off == file.size() - 4) {
        file.assign(file.substr(0, mp3_off));
        file.append(".adx");
    }

    data_struct* aft_data = &data_list[DATA_AFT];

    std::string path;
    if (sound_path.size()) {
        path.assign(sound_path);
        path.append(file);
    }
    else if (sound_voice) {
        const char* rom_sound_voice_array[] = {
            "rom/tv/sound/voice/",
            "rom/sound/voice/",
        };

        int32_t path_count = 0;
        for (size_t i = 0; i < sizeof(rom_sound_voice_array) / sizeof(const char*); i++)
            if (aft_data->check_file_exists(rom_sound_voice_array[i], file.c_str())) {
                path.assign(rom_sound_voice_array[i]);
                path.append(file);
                path_count++;
            }

        if (path_count != 1)
            path.clear();
    }
    else
        return;

    if (path.size() && aft_data->check_file_exists(path.c_str()))
        sound_work_play_stream(1, path.c_str(), false);
}

void AetObj::DispLayer(const mat4& mat, const aet_layer* layer,
    float_t frame, float_t opacity, const sprite_database* spr_db) {
    if (frame < layer->start_time
        || frame >= layer->end_time
        || layer->item_type == AET_ITEM_TYPE_AUDIO
        || (!layer->flags.video_active && !matte))
        return;

    mat4 m = mat;
    CalcMat(m, layer, frame);

    float_t _opacity = layer->video->opacity.interpolate(frame);
    opacity *= clamp_def(_opacity, 0.0f, 1.0f);

    frame = (frame - layer->start_time) * layer->time_scale + layer->offset_time;

    if (layer->item_type == AET_ITEM_TYPE_VIDEO)
        DispVideo(m, layer, frame, opacity, spr_db);
    else if (layer->item_type == AET_ITEM_TYPE_COMPOSITION)
        DispComp(m, layer->item.comp, frame, opacity, spr_db);
}

void AetObj::DispVideo(const mat4& mat, const aet_layer* layer,
    float_t frame, float_t opacity, const sprite_database* spr_db) {
    const aet_video* video = layer->item.video;
    if (video->sources_count <= 1) {
        if (video->sources_count == 1)
            DispSpriteSource(mat, layer, 0, opacity, spr_db);
        else if (layer_sprite.size())
            DispSprite(mat, layer, opacity, spr_db);
        return;
    }

#pragma warning(suppress: 26451)
    double_t v11 = video->fpf * frame + 0.0001;
    if (layer->flags.motion_blur) {
        double_t v12;
        if (video->fpf >= 1.0f) {
            v12 = 1.0 - 1.0 / video->fpf;
            if (v11 - 1.0 >= 0.0) {
                DispSpriteSource(mat, layer, (uint32_t)((int32_t)v11 - 1),
                    (float_t)(sin(v12) * opacity), spr_db);
            }
        }
        else
            v12 = v11 - (double_t)(int32_t)v11;

        DispSpriteSource(mat, layer, (uint32_t)(int32_t)v11, (float_t)(cos(v12) * opacity), spr_db);
        if (v11 + 1.0 < (double_t)(int32_t)video->sources_count) {
            DispSpriteSource(mat, layer, (uint32_t)((int32_t)v11 + 1),
                (float_t)(sin(v12) * opacity), spr_db);
        }
    }
    else
        DispSpriteSource(mat, layer, (uint32_t)(int32_t)v11, opacity, spr_db);
}

void AetObj::DispSprite(const mat4& mat, const aet_layer* layer,
    float_t opacity, const sprite_database* spr_db) {
    auto elem = layer_sprite.find(layer->name);
    if (elem == layer_sprite.end())
        return;

    vec4 color = this->color;
    color.w *= opacity;

    spr::SprArgs args;
    args.id.id = elem->second;
    args.mat = mat;
    args.index = index;
    args.layer = layer_index;
    args.prio = prio;
    args.color = color;
    args.resolution_mode_screen = dst_mode;
    args.resolution_mode_sprite = dst_mode;
    if (flags & AET_FLIP_H)
        enum_or(args.attr, spr::SPR_ATTR_FLIP_H);
    if (flags & AET_FLIP_V)
        enum_or(args.attr, spr::SPR_ATTR_FLIP_V);
    if (flags & AET_100000)
        enum_or(args.attr, spr::SPR_ATTR_10000000);
    spr::put_sprite(args, spr_db);
}

void AetObj::DispSpriteSource(const mat4& mat, const aet_layer* layer,
    uint32_t source_index, float_t opacity, const sprite_database* spr_db) {
    int32_t spr_id = layer->item.video->sources[source_index].sprite_index;

    auto elem_discard = sprite_discard.find(spr_id);
    if (elem_discard != sprite_discard.end())
        return;

    spr::SprArgs args;
    args.id.id = spr_id;

    auto elem_texture = sprite_texture.find(spr_id);
    if (elem_texture != sprite_texture.end()) {
        args.texture = elem_texture->second;
        args.id.id = -1;
    }
    else if (spr_id != -1) {
        auto elem_replace = sprite_replace.find(spr_id);
        if (elem_replace != sprite_replace.end() && elem_replace->second != -1) {
            const spr_db_spr* spr = spr_db->get_spr_by_id(elem_replace->second);
            //const spr_db_spr_set* spr_set = spr_db->get_spr_set_by_index(spr->info.set_index & 0x0FFF);
            const spr_db_spr_set* spr_set = spr_db->get_spr_set_by_index(spr->info.set_index & 0x3FFF);
            if (sprite_manager_get_set_ready(spr_set->id, spr_db))
                args.id.id = elem_replace->second;
        }
    }

    vec4 color = this->color;
    color.w *= opacity;

    args.mat = mat;
    args.index = index;
    args.layer = layer_index;
    args.prio = prio;
    args.color = color;
    args.resolution_mode_screen = dst_mode;
    args.resolution_mode_sprite = dst_mode;
    if (flags & AET_FLIP_H)
        enum_or(args.attr, spr::SPR_ATTR_FLIP_H);
    if (flags & AET_FLIP_V)
        enum_or(args.attr, spr::SPR_ATTR_FLIP_V);
    if (flags & AET_100000)
        enum_or(args.attr, spr::SPR_ATTR_10000000);

    switch (layer->video->transfer_mode.mode) {
    case AET_BLEND_MODE_SCREEN:
        args.blend = 1;
        break;
    case AET_BLEND_MODE_ADD:
        args.blend = 2;
        break;
    case AET_BLEND_MODE_MULTIPLY:
        args.blend = 3;
        break;
    case AET_BLEND_MODE_OVERLAY:
        args.blend = 5;
        break;
    }

    if (!matte) {
        if (layer->video->transfer_mode.matte) {
            this->opacity = opacity;
            matte = true;
            spr_args.Reset();
            spr_args = args;
        }
        else if (fabsf(mat4_determinant(&args.mat)) > 0.000001f)
            spr::put_sprite(args, spr_db);
    }
    else {
        matte = false;
        if (fabsf(mat4_determinant(&args.mat)) > 0.000001f) {
            vec4 _color = this->color;
            _color.w = opacity * _color.w * this->opacity;

           spr_args.color = _color;

            spr::SprArgs* v52 = spr::put_sprite(spr_args, spr_db);
            spr::SprArgs* v53 = spr::put_sprite(args, spr_db);
            if (v53 && v52)
                spr::SprArgs::SetNext(v53, v52);
        }
    }
}

void AetObj::CalcMat(mat4& mat, const aet_layer* layer, float_t frame) {
    if (layer->parent)
        CalcMat(mat, layer->parent, frame);

    float_t pos_x;
    float_t pos_y;
    float_t pos_z = 0.0f;
    float_t scale_x;
    float_t scale_y;
    float_t scale_z = 1.0f;
    float_t dir_x = 0.0f;
    float_t dir_y = 0.0f;
    float_t dir_z = 0.0f;
    float_t rot_x = 0.0f;
    float_t rot_y = 0.0f;
    float_t rot_z;
    float_t anchor_x;
    float_t anchor_y;
    float_t anchor_z = 0.0f;

    if (use_float) {
        const aet_layer_video* video = layer->video;
        pos_x = video->pos_x.interpolate(frame);
        pos_y = video->pos_y.interpolate(frame);
        rot_z = video->rot_z.interpolate(frame) * DEG_TO_RAD_FLOAT;
        scale_x = video->scale_x.interpolate(frame);
        scale_y = video->scale_y.interpolate(frame);
        anchor_x = video->anchor_x.interpolate(frame);
        anchor_y = video->anchor_y.interpolate(frame);

        const aet_layer_video_3d* _3d = video->_3d;
        if (_3d) {
            pos_z = _3d->pos_z.interpolate(frame);
            dir_x = _3d->dir_x.interpolate(frame) * DEG_TO_RAD_FLOAT;
            dir_y = _3d->dir_y.interpolate(frame) * DEG_TO_RAD_FLOAT;
            dir_z = _3d->dir_z.interpolate(frame) * DEG_TO_RAD_FLOAT;
            rot_x = _3d->rot_x.interpolate(frame) * DEG_TO_RAD_FLOAT;
            rot_y = _3d->rot_y.interpolate(frame) * DEG_TO_RAD_FLOAT;
            scale_z = _3d->scale_z.interpolate(frame);
            anchor_z = _3d->anchor_z.interpolate(frame);
        }
    }
    else {
        const aet_layer_video* video = layer->video;
        pos_x = prj::roundf(video->pos_x.interpolate(frame));
        pos_y = prj::roundf(video->pos_y.interpolate(frame));
        rot_z = video->rot_z.interpolate(frame) * DEG_TO_RAD_FLOAT;
        scale_x = video->scale_x.interpolate(frame);
        scale_y = video->scale_y.interpolate(frame);
        anchor_x = prj::roundf(video->anchor_x.interpolate(frame));
        anchor_y = prj::roundf(video->anchor_y.interpolate(frame));

        const aet_layer_video_3d* _3d = video->_3d;
        if (_3d) {
            pos_z = prj::roundf(_3d->pos_z.interpolate(frame));
            dir_x = _3d->dir_x.interpolate(frame) * DEG_TO_RAD_FLOAT;
            dir_y = _3d->dir_y.interpolate(frame) * DEG_TO_RAD_FLOAT;
            dir_z = _3d->dir_z.interpolate(frame) * DEG_TO_RAD_FLOAT;
            rot_x = _3d->rot_x.interpolate(frame) * DEG_TO_RAD_FLOAT;
            rot_y = _3d->rot_y.interpolate(frame) * DEG_TO_RAD_FLOAT;
            scale_z = _3d->scale_z.interpolate(frame);
            anchor_z = prj::roundf(_3d->anchor_z.interpolate(frame));
        }
    }

    mat4_mul_translate(&mat, pos_x, pos_y, -pos_z, &mat);
    if (fabsf(dir_x) > 0.000001f)
        mat4_mul_rotate_x(&mat, -dir_x, &mat);
    if (fabsf(dir_y) > 0.000001f)
        mat4_mul_rotate_y(&mat, -dir_y, &mat);
    if (fabsf(dir_z) > 0.000001f)
        mat4_mul_rotate_z(&mat, dir_z * scale_size.x * scale_size.y, &mat);
    if (fabsf(rot_x) > 0.000001f)
        mat4_mul_rotate_x(&mat, -rot_x, &mat);
    if (fabsf(rot_y) > 0.000001f)
        mat4_mul_rotate_y(&mat, -rot_y, &mat);
    if (fabsf(rot_z) > 0.000001f)
        mat4_mul_rotate_z(&mat, rot_z * scale_size.x * scale_size.y, &mat);
    mat4_scale_rot(&mat, scale_x, scale_y, scale_z, &mat);
    mat4_mul_translate(&mat, -anchor_x, -anchor_y, -anchor_z, &mat);
}

AetLyo::AetLyo(AetComp* comp) {
    this->comp = comp;
}

AetLyo::~AetLyo() {

}

void AetLyo::CtrlLayer(const aet_layer* layer, float_t frame) {

}

void AetLyo::DispLayer(const mat4& mat, const aet_layer* layer,
    float_t frame, float_t opacity, const sprite_database* spr_db) {
    if (frame < layer->start_time
        || frame >= layer->end_time
        || layer->item_type == AET_ITEM_TYPE_AUDIO)
        return;

    mat4 m = mat;
    CalcMat(m, layer, frame);

    float_t _opacity = layer->video->opacity.interpolate(frame);
    opacity *= clamp_def(_opacity, 0.0f, 1.0f);

    frame = (frame - layer->start_time) * layer->time_scale + layer->offset_time;

    if (layer->item_type == AET_ITEM_TYPE_VIDEO) {
        aet_layout_data data;
        const aet_layer_video* layer_video = layer->video;
        data.anchor.x = layer_video->anchor_x.interpolate(frame);
        data.anchor.y = layer_video->anchor_y.interpolate(frame);
        const aet_layer_video_3d* layer_video_3d = layer_video->_3d;
        if (layer_video_3d)
            data.anchor.z = layer_video_3d->anchor_z.interpolate(frame);
        data.mat = m;
        mat4_transform_point(&m, &data.anchor, &data.position);

        const aet_video* video = layer->item.video;
        data.width = (float_t)video->width;
        data.height = (float_t)video->height;
        data.color.r = video->color[0];
        data.color.g = video->color[1];
        data.color.b = video->color[2];
        data.opacity = opacity;
        data.mode = dst_mode;
        comp->Add(layer->name, data);
    }
    else if (layer->item_type == AET_ITEM_TYPE_COMPOSITION)
        DispComp(m, layer->item.comp, frame, opacity, spr_db);
}

AetMgr::AetMgr() : load_counter(), set_counter() {

}

AetMgr::~AetMgr() {

}

bool AetMgr::init() {
    objects.clear();
    load_counter = 0;
    return true;
}

bool AetMgr::ctrl() {
    for (auto& i : free_objects)
        objects.erase(i);
    free_objects.clear();

    for (auto& i : objects)
        i.second.Ctrl();
    return false;
}

bool AetMgr::dest() {
    objects.clear();
    load_counter = 0;
    return true;
}

void AetMgr::disp() {
    for (auto& i : objects)
        i.second.Disp();
}

void AetMgr::basic() {
    auto i = objects.begin();
    auto i_end = objects.end();
    while (i != i_end) {
        if (i->second.StepFrame())
            FreeAetObject(i);
        i++;
    }
}

uint32_t AetMgr::AddSetModern() {
    uint32_t index = this->set_counter;
    //for (; index <= 0x0FFF; index++) {
    for (; index <= 0x3FFF; index++) {
        auto elem = sets.find(0x8000 | index);
        if (elem == sets.end())
            break;
    }

    //if (!index || index > 0x0FFF) {
    if (!index || index > 0x3FFF) {
        //for (index = 1; index <= 0x0FFF; index++) {
        for (index = 1; index <= 0x3FFF; index++) {
            auto elem = sets.find(0x8000 | index);
            if (elem == sets.end())
                break;
        }

        //if (!index || index > 0x0FFF)
        if (!index || index > 0x3FFF)
            return 0x8000;
    }

    sets.insert({ 0x8000 | index, AetSet(0x8000 | index) });
    set_counter = index + 1;
    //if (index + 1 > 0x0FFF)
    if (index + 1 > 0x3FFF)
        set_counter = 1;
    return 0x8000 | index;
}

void AetMgr::AddAetSets(const aet_database* aet_db) {
    for (auto& i : aet_db->aet_set_indices)
        sets.insert({ i.first, AetSet(i.first) });
}

void AetMgr::FreeAetObject(uint32_t id) {
    auto elem = objects.find(id);
    if (elem != objects.end())
        FreeAetObject(elem);
}

void AetMgr::FreeAetObject(std::map<uint32_t, AetObj>::iterator it) {
    auto i = free_objects.begin();
    auto i_end = free_objects.end();
    while (i != i_end) {
        if (it == *i) {
            free_objects.erase(i);
            break;
        }
        i++;
    }

    free_objects.push_back(it);
}

void AetMgr::FreeAetSetObjects(uint32_t set_index) {
    auto i = objects.begin();
    auto i_end = objects.end();
    while (i != i_end) {
        if (i->second.GetInfo().set_index == set_index)
            FreeAetObject(i);
        i++;
    }
}

AetObj* AetMgr::GetObj(uint32_t id) {
    auto elem = objects.find(id);
    if (elem != objects.end())
        return &elem->second;
    return 0;
}

bool AetMgr::GetObjEnd(uint32_t id) {
    if (app::TaskWork::check_task_ready(this)) {
        AetObj* obj = AetMgr::GetObj(id);
        if (obj)
            return obj->GetEnd();
    }
    return true;
}

float_t AetMgr::GetObjFrame(uint32_t id) {
    if (app::TaskWork::check_task_ready(this)) {
        AetObj* obj = AetMgr::GetObj(id);
        if (obj)
            return obj->GetFrame();
    }
    return 0.0f;
}

bool AetMgr::GetObjVisible(uint32_t id) {
    if (app::TaskWork::check_task_ready(this)) {
        AetObj* obj = AetMgr::GetObj(id);
        if (obj)
            return obj->GetVisible();
    }
    return false;
}

AetSet* AetMgr::GetSet(uint32_t index) {
    auto elem = sets.find(index);
    if (elem != sets.end())
        return &elem->second;
    return 0;
}

bool AetMgr::GetSetReady(uint32_t index) {
    return GetSet(index)->ready;
}

void AetMgr::GetSetSceneCompLayerNames(std::vector<std::string>& vec, aet_info info) {
    AetSet* set = GetSet(info.set_index);
    set->GetSceneCompLayerNames(vec, set->GetSceneByInfo(info));
}

uint32_t AetMgr::GetSetScenesCount(uint32_t index) {
    return GetSet(index)->GetScenesCount();
}

float_t AetMgr::GetSetSceneEndTime(aet_info info) {
    return GetSet(info.set_index)->GetSceneEndTime(info.index) - 1.0f;
}

float_t AetMgr::GetSetSceneLayerEndTime(aet_info info, const char* layer_name) {
    AetSet* set = GetSet(info.set_index);
    return set->GetSceneLayer(set->GetSceneByInfo(info), layer_name)->end_time - 1.0f;
}

void AetMgr::GetSetSceneLayerMarkerNames(std::vector<std::string>& vec, aet_info info, const char* layer_name) {
    AetSet* set = GetSet(info.set_index);
    set->GetLayerMarkerNames(vec, set->GetSceneLayer(set->GetSceneByInfo(info), layer_name));
}

float_t AetMgr::GetSetSceneLayerMarkerTime(aet_info info, const char* scene_name, const char* marker_name) {
    AetSet* set = GetSet(info.set_index);
    return set->GetLayerMarker(set->GetSceneLayer(set->GetSceneByInfo(info), scene_name), marker_name)->time;
}

float_t AetMgr::GetSetSceneLayerStartTime(aet_info info, const char* layer_name) {
    AetSet* set = GetSet(info.set_index);
    return set->GetSceneLayer(set->GetSceneByInfo(info), layer_name)->start_time;
}

const char* AetMgr::GetSetSceneName(aet_info info) {
    return GetSet(info.set_index)->GetSceneName(info.index);
}

float_t AetMgr::GetSetSceneStartTime(aet_info info) {
    return GetSet(info.set_index)->GetSceneStartTime(info.index);
}

void AetMgr::InitAetLayout(AetComp* comp, AetArgs& args) {
    AetLyo layout(comp);
    InitAetObj(layout, args);
    layout.Disp();
}

bool AetMgr::InitAetObj(AetObj& obj, AetArgs& args) {
    AetSet* set = GetSet(args.id.info.set_index);
    if (!set)
        return false;

    const aet_scene* scene = set->GetSceneByInfo(args.id.info);
    if (!scene)
        return false;

    const aet_comp* comp = aet_scene::get_root_comp(scene);
    if (!comp)
        return false;

    const aet_layer* layer = 0;
    if (args.layer_name) {
        layer = set->GetSceneLayer(scene, args.layer_name);
        if (!layer)
            return false;
    }

    const aet_marker* start_marker = 0;
    const aet_marker* end_marker = 0;
    if ((!args.start_marker || (start_marker = set->GetLayerMarker(layer, args.start_marker)))
        && (!args.end_marker || (end_marker = set->GetLayerMarker(layer, args.end_marker)))) {
        resolution_mode mode = set->GetSceneResolutionMode(scene);
        obj.Init(args, scene, comp, layer, start_marker, end_marker, mode);
        return true;
    }

    return false;
}

uint32_t AetMgr::InitAetObject(AetArgs& args) {
    AetObj obj;
    if (!InitAetObj(obj, args))
        return 0;

    while (true) {
        auto elem = objects.find(++load_counter);
        if (elem == objects.end() && load_counter)
            break;
    }

    objects.insert({ load_counter, obj });
    return load_counter;
}

bool AetMgr::LoadFile(uint32_t index) {
    AetSet* set = GetSet(index);
    if (set)
        return set->LoadFile();
    return false;
}

bool AetMgr::LoadFileModern(uint32_t index, aet_database* aet_db) {
    AetSet* set = GetSet(index);
    if (set)
        return set->LoadFileModern(aet_db);
    return false;
}

void AetMgr::ReadFile(uint32_t index, const char* file, std::string& mdata_dir, void* data) {
    const char* dir = "./rom/2d/";
    if (mdata_dir.size() && ((data_struct*)data)->check_directory_exists(mdata_dir.c_str()))
        dir = mdata_dir.c_str();

    AetSet* set = GetSet(index);
    if (set && ((data_struct*)data)->check_file_exists(dir, file))
        set->ReadFile(dir, file, data);
}

void AetMgr::ReadFileModern(uint32_t index,
    uint32_t set_hash, void* data, aet_database* aet_db) {
    if (index == -1) {
        index = AddSetModern();
        aet_db->add_aet_set(set_hash, index);
    }

    AetSet* set = GetSet(index);
    if (set)
        set->ReadFileModern(set_hash, data);
}

void AetMgr::RemoveAetSets(const aet_database* aet_db) {
    for (auto& i : aet_db->aet_set_indices)
        sets.erase(i.first);
}

void AetMgr::SetObjAlpha(uint32_t id, float_t value) {
    AetObj* obj = GetObj(id);
    if (obj)
        obj->SetAlpha(value);
}

void AetMgr::SetObjColor(uint32_t id, const vec4& value) {
    AetObj* obj = GetObj(id);
    if (obj)
        obj->SetColor(value);
}

void AetMgr::SetObjEndTime(uint32_t id, float_t value) {
    AetObj* obj = GetObj(id);
    if (obj)
        obj->SetEndTime(value);
}

void AetMgr::SetObjFrame(uint32_t id, float_t value) {
    AetObj* obj = GetObj(id);
    if (obj)
        obj->SetFrame(value);
}

void AetMgr::SetObjFrameRateControl(uint32_t id, FrameRateControl* value) {
    AetObj* obj = GetObj(id);
    if (obj)
        obj->SetFrameRateControl(value);
}

void AetMgr::SetObjPlay(uint32_t id, bool value) {
    AetObj* obj = GetObj(id);
    if (obj)
        obj->SetPlay(value);
}

void AetMgr::SetObjPosition(uint32_t id, const vec3& value) {
    AetObj* obj = GetObj(id);
    if (obj)
        obj->SetPosition(value);
}

void AetMgr::SetObjPrio(uint32_t id, spr::SprPrio value) {
    AetObj* obj = GetObj(id);
    if (obj)
        obj->SetPrio(value);
}

void AetMgr::SetObjRotation(uint32_t id, const vec3& value) {
    AetObj* obj = GetObj(id);
    if (obj)
        obj->SetRotation(value);
}

void AetMgr::SetObjScale(uint32_t id, const vec3& value) {
    AetObj* obj = GetObj(id);
    if (obj)
        obj->SetScale(value);
}

void AetMgr::SetObjSpriteDiscard(uint32_t id, std::map<uint32_t, uint32_t>& value) {
    AetObj* obj = GetObj(id);
    if (obj)
        obj->SetSpriteDiscard(value);
}

void AetMgr::SetObjSpriteReplace(uint32_t id, std::map<uint32_t, uint32_t>& value) {
    AetObj* obj = GetObj(id);
    if (obj)
        obj->SetSpriteReplace(value);
}

void AetMgr::SetObjSpriteTexture(uint32_t id, std::map<uint32_t, texture*>& value) {
    AetObj* obj = GetObj(id);
    if (obj)
        obj->SetSpriteTexture(value);
}

void AetMgr::SetObjVisible(uint32_t id, bool value) {
    AetObj* obj = GetObj(id);
    if (obj)
        obj->SetVisible(value);
}

void AetMgr::UnloadSet(uint32_t index) {
    AetSet* set = GetSet(index);
    if (set)
        set->Unload();
}

void AetMgr::UnloadSetModern(uint32_t index, aet_database* aet_db) {
    AetSet* set = GetSet(index);
    if (set) {
        set->UnloadModern(aet_db);
        if (set->load_count <= 0) {
            auto elem = sets.find(index);
            if (elem != sets.end())
                sets.erase(elem);
        }
    }
}
