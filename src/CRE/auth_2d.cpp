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

enum aet_blend_mode : uint8_t {
    AET_BLEND_MODE_NONE = 0,
    AET_BLEND_MODE_COPY,
    AET_BLEND_MODE_BEHIND,
    AET_BLEND_MODE_NORMAL,
    AET_BLEND_MODE_DISSOLVE,
    AET_BLEND_MODE_ADD,
    AET_BLEND_MODE_MULTIPLY,
    AET_BLEND_MODE_SCREEN,
    AET_BLEND_MODE_OVERLAY,
    AET_BLEND_MODE_SOFT_LIGHT,
    AET_BLEND_MODE_HARD_LIGHT,
    AET_BLEND_MODE_DARKEN,
    AET_BLEND_MODE_LIGHTEN,
    AET_BLEND_MODE_CLASSIC_DIFFERENCE,
    AET_BLEND_MODE_HUE,
    AET_BLEND_MODE_SATURATION,
    AET_BLEND_MODE_COLOR,
    AET_BLEND_MODE_LUMINOSITY,
    AET_BLEND_MODE_STENCIL_ALPHA,
    AET_BLEND_MODE_STENCIL_LUMA,
    AET_BLEND_MODE_SILHOUETTE_ALPHA,
    AET_BLEND_MODE_SILHOUETTE_LUMA,
    AET_BLEND_MODE_LUMINESCENT_PREMUL,
    AET_BLEND_MODE_ALPHA_ADD,
    AET_BLEND_MODE_CLASSIC_COLOR_DODGE,
    AET_BLEND_MODE_CLASSIC_COLOR_BURN,
    AET_BLEND_MODE_EXCLUSION,
    AET_BLEND_MODE_DIFFERENCE,
    AET_BLEND_MODE_COLOR_DODGE,
    AET_BLEND_MODE_COLOR_BURN,
    AET_BLEND_MODE_LINEAR_DODGE,
    AET_BLEND_MODE_LINEAR_BURN,
    AET_BLEND_MODE_LINEAR_LIGHT,
    AET_BLEND_MODE_VIVID_LIGHT,
    AET_BLEND_MODE_PIN_LIGHT,
    AET_BLEND_MODE_HARD_MIX,
    AET_BLEND_MODE_LIGHTER_COLOR,
    AET_BLEND_MODE_DARKER_COLOR,
    AET_BLEND_MODE_SUBTRACT,
    AET_BLEND_MODE_DIVIDE,
};

enum aet_layer_quality : uint8_t {
    AET_LAYER_QUALITY_NONE = 0,
    AET_LAYER_QUALITY_WIREFRAME,
    AET_LAYER_QUALITY_DRAFT,
    AET_LAYER_QUALITY_BEST,
};

enum aet_item_type : uint8_t {
    AET_ITEM_TYPE_NONE = 0,
    AET_ITEM_TYPE_VIDEO,
    AET_ITEM_TYPE_AUDIO,
    AET_ITEM_TYPE_COMPOSITION,
};

struct aet_layer_flag {
    uint16_t video_active : 1;
    uint16_t audio_active : 1;
    uint16_t effects_active : 1;
    uint16_t motion_blur : 1;
    uint16_t frame_blending : 1;
    uint16_t locked : 1;
    uint16_t shy : 1;
    uint16_t collapse : 1;
    uint16_t auto_orient_rotation : 1;
    uint16_t adjustment_layer : 1;
    uint16_t time_remapping : 1;
    uint16_t layer_is_3d : 1;
    uint16_t look_at_camera : 1;
    uint16_t look_at_point_of_interest : 1;
    uint16_t solo : 1;
    uint16_t markers_locked : 1;
    //uint16_t null_layer : 1;
    //uint16_t hide_locked_masks : 1;
    //uint16_t guide_layer : 1;
    //uint16_t advanced_frame_blending : 1;
    //uint16_t sub_layers_render_separately : 1;
    //uint16_t environment_layer : 1;
};

struct aet_fcurve {
    uint32_t keys_count;
    const float_t* keys;

    float_t interpolate(float_t frame) const;
};

struct aet_fcurve_file {
    uint32_t keys_count;
    uint32_t keys_offset;
};

struct aet_layer_video_3d {
    aet_fcurve anchor_z;
    aet_fcurve pos_z;
    aet_fcurve dir_x;
    aet_fcurve dir_y;
    aet_fcurve dir_z;
    aet_fcurve rot_x;
    aet_fcurve rot_y;
    aet_fcurve scale_z;
};

struct aet_layer_video_3d_file {
    aet_fcurve_file anchor_z;
    aet_fcurve_file pos_z;
    aet_fcurve_file dir_x;
    aet_fcurve_file dir_y;
    aet_fcurve_file dir_z;
    aet_fcurve_file rot_x;
    aet_fcurve_file rot_y;
    aet_fcurve_file scale_z;
};

struct aet_transfer_mode {
    aet_blend_mode mode;
    uint8_t flag;
    uint8_t matte;
};

struct aet_layer_video {
    aet_transfer_mode transfer_mode;
    aet_fcurve anchor_x;
    aet_fcurve anchor_y;
    aet_fcurve pos_x;
    aet_fcurve pos_y;
    aet_fcurve rot_z;
    aet_fcurve scale_x;
    aet_fcurve scale_y;
    aet_fcurve opacity;
    const aet_layer_video_3d* _3d;
};

struct aet_layer_video_file {
    aet_transfer_mode transfer_mode;
    aet_fcurve_file anchor_x;
    aet_fcurve_file anchor_y;
    aet_fcurve_file pos_x;
    aet_fcurve_file pos_y;
    aet_fcurve_file rot_z;
    aet_fcurve_file scale_x;
    aet_fcurve_file scale_y;
    aet_fcurve_file opacity;
    uint32_t _3d_offset;
};

struct aet_layer_audio {
    aet_fcurve volume_l;
    aet_fcurve volume_r;
    aet_fcurve pan_l;
    aet_fcurve pan_r;
};

struct aet_layer_audio_file {
    aet_fcurve_file volume_l;
    aet_fcurve_file volume_r;
    aet_fcurve_file pan_l;
    aet_fcurve_file pan_r;
};

struct aet_camera {
    aet_fcurve eye_x;
    aet_fcurve eye_y;
    aet_fcurve eye_z;
    aet_fcurve pos_x;
    aet_fcurve pos_y;
    aet_fcurve pos_z;
    aet_fcurve dir_x;
    aet_fcurve dir_y;
    aet_fcurve dir_z;
    aet_fcurve rot_x;
    aet_fcurve rot_y;
    aet_fcurve rot_z;
    aet_fcurve zoom;
};

struct aet_camera_file {
    aet_fcurve_file eye_x;
    aet_fcurve_file eye_y;
    aet_fcurve_file eye_z;
    aet_fcurve_file pos_x;
    aet_fcurve_file pos_y;
    aet_fcurve_file pos_z;
    aet_fcurve_file dir_x;
    aet_fcurve_file dir_y;
    aet_fcurve_file dir_z;
    aet_fcurve_file rot_x;
    aet_fcurve_file rot_y;
    aet_fcurve_file rot_z;
    aet_fcurve_file zoom;
};

struct aet_video_src {
    const char* sprite_name;
    int32_t sprite_index;
};

struct aet_video_src_file {
    uint32_t sprite_name_offset;
    int32_t sprite_index;
};

struct aet_video {
    uint8_t color[3];
    uint16_t width;
    uint16_t height;
    float_t fpf;
    uint32_t sources_count;
    const aet_video_src* sources;
};

struct aet_video_file {
    uint8_t color[3];
    uint16_t width;
    uint16_t height;
    float_t fpf;
    uint32_t sources_count;
    uint32_t sources_offset;
};

struct aet_audio {
    uint32_t sound_index;
};

struct aet_comp;

union aet_item {
    const void* none;
    uint32_t offset;
    const aet_video* video;
    const aet_audio* audio;
    const aet_comp* comp;

    inline aet_item() : none() {

    }

    inline aet_item(aet_video* video) {
        this->video = video;
    }

    inline aet_item(aet_audio* audio) {
        this->audio = audio;
    }

    inline aet_item(aet_comp* comp) {
        this->comp = comp;
    }
};

struct aet_marker {
    float_t time;
    const char* name;
};

struct aet_marker_file {
    float_t time;
    uint32_t name_offset;
};

#pragma warning(push)
#pragma warning(disable: 26495)
struct aet_layer {
    const char* name;
    float_t start_time;
    float_t end_time;
    float_t offset_time;
    float_t time_scale;
    aet_layer_flag flags;
    aet_layer_quality quality;
    aet_item_type item_type;
    aet_item item;
    union {
        const aet_layer* parent;
        uint32_t parent_offset;
    };
    uint32_t markers_count;
    const aet_marker* markers;
    const aet_layer_video* video;
    const aet_layer_audio* audio;
};
#pragma warning(pop)

struct aet_layer_file {
    uint32_t name_offset;
    float_t start_time;
    float_t end_time;
    float_t offset_time;
    float_t time_scale;
    aet_layer_flag flags;
    aet_layer_quality quality;
    aet_item_type item_type;
    uint32_t item_offset;
    uint32_t parent_offset;
    uint32_t markers_count;
    uint32_t markers_offset;
    uint32_t video_offset;
    uint32_t audio_offset;
};

struct aet_comp {
    uint32_t layers_count;
    const aet_layer* layers;
};

struct aet_comp_file {
    uint32_t layers_count;
    uint32_t layers_offset;
};

struct aet_scene {
    const char* name;
    float_t start_time;
    float_t end_time;
    float_t fps;
    uint8_t color[3];
    uint32_t width;
    uint32_t height;
    const aet_camera* camera;
    uint32_t comp_count;
    const aet_comp* comp;
    uint32_t video_count;
    const aet_video* video;
    uint32_t audio_count;
    const aet_audio* audio;
};

struct aet_scene_file {
    uint32_t name_offset;
    float_t start_time;
    float_t end_time;
    float_t fps;
    uint8_t color[3];
    uint32_t width;
    uint32_t height;
    uint32_t camera_offset;
    uint32_t comp_count;
    uint32_t comp_offset;
    uint32_t video_count;
    uint32_t video_offset;
    uint32_t audio_count;
    uint32_t audio_offset;
};

struct aet_set_file {
#pragma warning(suppress: 4200)
    uint32_t scene_offsets[];
};

struct aet_set_load_data {
    prj::shared_ptr<prj::stack_allocator> alloc_handler;
    size_t data;
    std::map<uint32_t, aet_layer*> layers;
    std::map<uint32_t, aet_item> items;

    aet_set_load_data(prj::shared_ptr<prj::stack_allocator> alloc_handler, size_t data);
    ~aet_set_load_data();
};

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
    virtual void SetPrio(spr::SprPrio value);
    virtual aet_info GetInfo();
    virtual bool GetPlay();
    virtual bool GetVisible();
    virtual float_t GetFrame();
    virtual bool GetDisp();
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

class AetSet {
public:
    uint32_t index;
    const aet_scene** scenes;
    uint32_t scenes_count;
    int32_t load_count;
    p_file_handler file_handler;
    bool ready;
    prj::shared_ptr<prj::stack_allocator> alloc_handler;

    AetSet(uint32_t index);
    virtual ~AetSet();

    virtual void ReadFile(const char* dir, const char* file, void* data);
    virtual bool LoadFile();
    virtual void Unload();
    virtual bool Load();

    const aet_marker* GetLayerMarker(const aet_layer* layer, const char* marker_name);
    void GetLayerMarkerNames(std::vector<std::string>& vec, const aet_layer* layer);
    const aet_scene* GetSceneByInfo(aet_info info);
    void GetSceneCompLayerNames(std::vector<std::string>& vec, const aet_scene* scene);
    float_t GetSceneEndTime(uint16_t index);
    const aet_layer* GetSceneLayer(const aet_scene* scene, const char* layer_name);
    const char* GetSceneName(uint16_t index);
    resolution_mode GetSceneResolutionMode(const aet_scene* scene);
    float_t GetSceneStartTime(uint16_t index);
    uint32_t GetScenesCount();
};

class AetMgr : public app::Task {
public:
    std::map<int32_t, AetSet> sets;
    std::map<uint32_t, AetObj> objects;
    std::list<std::map<uint32_t, AetObj>::iterator> free_objects;
    uint32_t load_counter;

    AetMgr();
    virtual ~AetMgr();

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;
    virtual void Basic() override;

    void AddAetSets(const aet_database* aet_db);
    void FreeAetObject(uint32_t id);
    void FreeAetObject(std::map<uint32_t, AetObj>::iterator it);
    void FreeAetSetObjects(uint32_t set_index);
    AetObj* GetObj(uint32_t id);
    bool GetObjDisp(uint32_t id);
    float_t GetObjFrame(uint32_t id);
    bool GetObjVisible(uint32_t id);
    AetSet* GetSet(int32_t index);
    bool GetSetReady(int32_t index);
    void GetSetSceneCompLayerNames(std::vector<std::string>& vec, aet_info info);
    float_t GetSetSceneEndTime(aet_info info);
    float GetSetSceneLayerEndTime(aet_info info, const char* layer_name);
    void GetSetSceneLayerMarkerNames(std::vector<std::string>& vec, aet_info info, const char* layer_name);
    float_t GetSetSceneLayerMarkerTime(aet_info info, const char* scene_name, const char* marker_name);
    float_t GetSetSceneLayerStartTime(aet_info info, const char* layer_name);
    const char* GetSetSceneName(aet_info info);
    float_t GetSetSceneStartTime(aet_info info);
    uint32_t GetSetScenesCount(int32_t index);
    void InitAetLayout(AetComp* comp, AetArgs& args);
    bool InitAetObj(AetObj& obj, AetArgs& args);
    uint32_t InitAetObject(AetArgs& args);
    bool LoadFile(int32_t index);
    void ReadFile(int32_t index, const char* file, std::string& mdata_dir, void* data);
    void RemoveAetSets(const aet_database* aet_db);
    void SetObjAlpha(uint32_t id, float_t value);
    void SetObjColor(uint32_t id, const vec4& value);
    void SetObjEndTime(uint32_t id, float_t value);
    void SetObjFrame(uint32_t id, float_t value);
    void SetObjPlay(uint32_t id, bool value);
    void SetObjPosition(uint32_t id, const vec3& value);
    void SetObjPrio(uint32_t id, spr::SprPrio value);
    void SetObjRotation(uint32_t id, const vec3& value);
    void SetObjScale(uint32_t id, const vec3& value);
    void SetObjSpriteDiscard(uint32_t id, std::map<uint32_t, uint32_t>& value);
    void SetObjSpriteReplace(uint32_t id, std::map<uint32_t, uint32_t>& value);
    void SetObjSpriteTexture(uint32_t id, std::map<uint32_t, texture*>& value);
    void SetObjVisible(uint32_t id, bool value);
    void UnloadSet(int32_t index);
};

AetMgr* aet_manager;

static aet_camera* aet_camera_load(aet_set_load_data& load_data, const aet_camera_file* cam_file);
static void aet_comp_set_item_parent(aet_set_load_data& load_data, const aet_comp* comp);
static aet_comp* aet_comps_load(aet_set_load_data& load_data,
    size_t comp_count, const aet_comp_file* comps_file);
static void aet_fcurve_load(aet_set_load_data& load_data,
    aet_fcurve& fcurve, const aet_fcurve_file& fcurve_file);
static aet_layer* aet_layer_load(aet_set_load_data& load_data,
    aet_layer* layer, const aet_layer_file* layer_file);
static aet_layer_audio* aet_layer_audio_load(
    aet_set_load_data& load_data, const aet_layer_audio_file* layer_audio_file);
static aet_layer_video* aet_layer_video_load(
    aet_set_load_data& load_data, const aet_layer_video_file* layer_video_file);
static aet_layer_video_3d* aet_layer_video_3d_load(
    aet_set_load_data& load_data, const aet_layer_video_3d_file* layer_video_3d_file);
static aet_marker* aet_markers_load(aet_set_load_data& load_data,
    size_t markers_count, const aet_marker_file* markers_file);
static const aet_comp* aet_scene_get_root_comp(const aet_scene* scene);
static aet_scene* aet_scene_load(aet_set_load_data& load_data,
    aet_scene* scene, const aet_scene_file* scene_file);
static aet_video* aet_video_load(aet_set_load_data& load_data,
    aet_video* video, const aet_video_file* video_file);

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

AetComp::AetComp() {

}

AetComp::~AetComp() {

}

void AetComp::Add(const char* name, aet_layout_data& data) {
    this->data.insert({ name, data });
}

aet_layout_data* AetComp::Find(const char* name) {
    auto elem = data.find(name);
    if (elem != data.end())
        return &elem->second;
    return 0;
}

aet_layout_data::aet_layout_data() : width(), height() {
    mat = mat4_identity;
    opacity = 1.0f;
    color = { 0xFF, 0xFF, 0xFF, 0xFF };
    mode = RESOLUTION_MODE_HD;
}

void aet_manager_init() {
    if (!aet_manager)
        aet_manager = new AetMgr;
}

void aet_manager_add_aet_sets(const aet_database* aet_db) {
    aet_manager->AddAetSets(aet_db);
}

bool aet_manager_add_task() {
    return app::TaskWork::AddTask(aet_manager, "2DAUTH_TASK", 2);
}

bool aet_manager_check_task_ready() {
    return app::TaskWork::CheckTaskReady(aet_manager);
}

bool aet_manager_del_task() {
    return aet_manager->DelTask();
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

bool aet_manager_get_obj_disp(uint32_t id) {
    return aet_manager->GetObjDisp(id);
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

bool aet_manager_get_set_ready(uint32_t set_id, const aet_database* aet_db) {
    return aet_manager->GetSetReady(aet_db->get_aet_set_by_id(set_id)->index);
};

void aet_manager_init_aet_layout(AetComp* comp,
    AetArgs args, const aet_database* aet_db) {
    args.id.info = aet_db->get_aet_by_id(args.id.id)->info;
    aet_manager->InitAetLayout(comp, args);
}

void aet_manager_init_aet_layout(AetComp* comp,
    uint32_t aet_id, const char* layer_name, AetFlags flags, resolution_mode mode,
    const char* start_marker, float_t start_time, const aet_database* aet_db) {
    AetArgs args;
    args.id.info = aet_db->get_aet_by_id(aet_id)->info;
    args.layer_name = layer_name;
    args.flags = flags;
    args.mode = mode;
    args.start_marker = start_marker;
    args.start_time = start_time;
    aet_manager->InitAetLayout(comp, args);
}

uint32_t aet_manager_init_aet_object(AetArgs args, const aet_database* aet_db) {
    args.id.info = aet_db->get_aet_by_id(args.id.id)->info;
    return aet_manager->InitAetObject(args);
}

uint32_t aet_manager_init_aet_object(uint32_t id, const char* layer_name, spr::SprPrio prio,
    AetFlags flags, const char* start_marker, const char* end_marker, const aet_database* aet_db) {
    AetArgs args;
    args.id.info = aet_db->get_aet_by_id(id)->info;
    args.layer_name = layer_name;
    args.prio = prio;
    args.flags = flags;
    args.start_marker = start_marker;
    args.end_marker = end_marker;
    return aet_manager->InitAetObject(args);
}

uint32_t aet_manager_init_aet_object(uint32_t aet_id, spr::SprPrio prio,
    AetFlags flags, const char* layer_name, const vec2* pos, int32_t index,
    const char* start_marker, const char* end_marker, float_t start_time, float_t end_time,
    const vec2* scale, FrameRateControl* frame_rate_control, const aet_database* aet_db) {
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
    return aet_manager_init_aet_object(args, aet_db);
}

bool aet_manager_load_file(uint32_t set_id, const aet_database* aet_db) {
    return aet_manager->LoadFile(aet_db->get_aet_set_by_id(set_id)->index);
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

void aet_manager_free() {
    if (aet_manager) {
        delete aet_manager;
        aet_manager = 0;
    }
}

float_t aet_fcurve::interpolate(float_t frame) const {
    if (!keys_count)
        return 0.0f;
    else if (keys_count == 1)
        return keys[0];

    size_t keys_count = this->keys_count;
    const float_t* keys = this->keys;
    const float_t* values = &keys[keys_count];
    if (frame <= keys[0])
        return values[0];

    const float_t* v6 = keys;
    const float_t* v7 = &keys[keys_count];
    if (frame >= keys[keys_count - 1])
        return values[(keys_count - 1) * 2];

    const float_t* v8 = &keys[keys_count / 2];
    while (v6 < v8) {
        if (frame >= *v8)
            v6 = v8;
        else
            v7 = v8;
        v8 = &v6[(v7 - v6) / 2];
    }

    if (values - 1 <= v8)
        return values[(keys_count - 1) * 2];

    size_t v9 = (v8 - keys) * 2;

    float_t df = v8[1] - v8[0];
    float_t t = (frame - v8[0]) / df;
    float_t t_1 = t - 1.0f;
    return (t_1 * values[v9 + 1] + t * values[v9 + 3]) * t * t_1 * df
        + t * t * (3.0f - 2.0f * t) * values[v9 + 2]
        + (1.0f + 2.0f * t) * ((1.0f - t) * (1.0f - t)) * values[v9];
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

aet_set_load_data::aet_set_load_data(
    prj::shared_ptr<prj::stack_allocator> alloc_handler, size_t data) {
    this->alloc_handler = alloc_handler;
    this->data = data;
}

aet_set_load_data::~aet_set_load_data() {

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
        anchor.x = (float_t)(int32_t)scene->width * 0.5f;
        anchor.y = (float_t)(int32_t)scene->height * 0.5f;
        anchor.z = 0.0f;
    }

    mat4 mat;
    mat4_translate(&pos, &mat);
    mat4_rotate_xyz_mult(&mat, rot.x, rot.y, rot.z * scale_size.x * scale_size.y, &mat);
    mat4_scale_rot(&mat, scale.x * scale_size.x, scale.y * scale_size.y, scale.z, &mat);
    mat4_translate_mult(&mat, -anchor.x, -anchor.y, -anchor.z, &mat);

    const aet_camera* camera = scene->camera;
    if (camera) {
        float_t eye_x = camera->eye_x.interpolate(frame) - (float_t)(int32_t)scene->width * 0.5f;
        float_t eye_y = camera->eye_y.interpolate(frame) - (float_t)(int32_t)scene->height * 0.5f;
        float_t eye_z = camera->eye_z.interpolate(frame);
        mat4_translate_mult(&mat, -eye_x, -eye_y, -eye_z, &mat);
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
        delta_frame = frame_rate_control->GetDeltaFrame();

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
                enum_or(flags, AET_DISP);
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
                enum_or(flags, AET_DISP);
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

bool AetObj::GetDisp() {
    return !!(flags & AET_DISP);
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

        if (path_count != -1)
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
    vec4_to_vec4u8(color * 255.0f, args.color);
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
    int32_t sprite_index = layer->item.video->sources[source_index].sprite_index;

    auto elem_discard = sprite_discard.find(sprite_index);
    if (elem_discard != sprite_discard.end())
        return;

    spr::SprArgs args;
    args.id.index = sprite_index;

    auto elem_texture = sprite_texture.find(sprite_index);
    if (elem_texture != sprite_texture.end()) {
        args.texture = elem_texture->second;
        args.id.index = -1;
    }
    else if (sprite_index != -1) {
        auto elem_replace = sprite_replace.find(sprite_index);
        if (elem_replace != sprite_replace.end() && elem_replace->second != -1) {
            const spr_db_spr* spr = spr_db->get_spr_by_id(elem_replace->second);
            const spr_db_spr_set* spr_set = spr_db->get_spr_set_by_index(spr->info.set_index & 0xFFF);
            if (sprite_manager_get_set_ready(spr_set->id, spr_db))
                args.id.index = elem_replace->second;
        }
    }

    vec4 color = this->color;
    color.w *= opacity;

    args.mat = mat;
    args.index = index;
    args.layer = layer_index;
    args.prio = prio;
    vec4_to_vec4u8(color * 255.0f, args.color);
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

            vec4_to_vec4u8(_color * 255.0f, spr_args.color);

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
        pos_x = roundf(video->pos_x.interpolate(frame));
        pos_y = roundf(video->pos_y.interpolate(frame));
        rot_z = video->rot_z.interpolate(frame) * DEG_TO_RAD_FLOAT;
        scale_x = video->scale_x.interpolate(frame);
        scale_y = video->scale_y.interpolate(frame);
        anchor_x = roundf(video->anchor_x.interpolate(frame));
        anchor_y = roundf(video->anchor_y.interpolate(frame));

        const aet_layer_video_3d* _3d = video->_3d;
        if (_3d) {
            pos_z = roundf(_3d->pos_z.interpolate(frame));
            dir_x = _3d->dir_x.interpolate(frame) * DEG_TO_RAD_FLOAT;
            dir_y = _3d->dir_y.interpolate(frame) * DEG_TO_RAD_FLOAT;
            dir_z = _3d->dir_z.interpolate(frame) * DEG_TO_RAD_FLOAT;
            rot_x = _3d->rot_x.interpolate(frame) * DEG_TO_RAD_FLOAT;
            rot_y = _3d->rot_y.interpolate(frame) * DEG_TO_RAD_FLOAT;
            scale_z = _3d->scale_z.interpolate(frame);
            anchor_z = roundf(_3d->anchor_z.interpolate(frame));
        }
    }

    mat4_translate_mult(&mat, pos_x, pos_y, -pos_z, &mat);
    if (fabs(dir_x) > 0.000001f)
        mat4_rotate_x_mult(&mat, -dir_x, &mat);
    if (fabs(dir_y) > 0.000001f)
        mat4_rotate_y_mult(&mat, -dir_y, &mat);
    if (fabs(dir_z) > 0.000001f)
        mat4_rotate_z_mult(&mat, dir_z * scale_size.x * scale_size.y, &mat);
    if (fabs(rot_x) > 0.000001f)
        mat4_rotate_x_mult(&mat, -rot_x, &mat);
    if (fabs(rot_y) > 0.000001f)
        mat4_rotate_y_mult(&mat, -rot_y, &mat);
    if (fabs(rot_z) > 0.000001f)
        mat4_rotate_z_mult(&mat, rot_z * scale_size.x * scale_size.y, &mat);
    mat4_scale_rot(&mat, scale_x, scale_y, scale_z, &mat);
    mat4_translate_mult(&mat, -anchor_x, -anchor_y, -anchor_z, &mat);
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
        mat4_mult_vec3_trans(&m, &data.anchor, &data.position);

        const aet_video* video = layer->item.video;
        data.width = (float_t)video->width;
        data.height = (float_t)video->height;
        data.color.x = video->color[0];
        data.color.y = video->color[1];
        data.color.z = video->color[2];;
        data.opacity = opacity;
        data.mode = dst_mode;
        comp->Add(layer->name, data);
    }
    else if (layer->item_type == AET_ITEM_TYPE_COMPOSITION)
        DispComp(m, layer->item.comp, frame, opacity, spr_db);
}

AetSet::AetSet(uint32_t index) : scenes(), scenes_count(), load_count(), ready() {
    this->index = index;
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

bool AetSet::Load() {
    size_t data = (size_t)file_handler.get_data();
    if (!data) {
        scenes = 0;
        return false;
    }

    prj::shared_ptr<prj::stack_allocator>& alloc = alloc_handler;
    alloc = prj::shared_ptr<prj::stack_allocator>(new prj::stack_allocator);

    uint32_t scenes_count = 0;
    const aet_set_file* set_file = (const aet_set_file*)data;
    while (set_file->scene_offsets[scenes_count])
        scenes_count++;
    this->scenes_count = scenes_count;

    aet_set_load_data load_data(alloc, data);
    aet_scene** scenes = alloc->allocate<aet_scene*>(scenes_count);
    this->scenes = (const aet_scene**)scenes;

    for (uint32_t i = scenes_count, j = 0; i; i--, j++)
        scenes[j] = aet_scene_load(load_data, alloc->allocate<aet_scene>(),
            (aet_scene_file*)(data + set_file->scene_offsets[j]));

    for (uint32_t i = scenes_count, j = 0; i; i--, j++) {
        aet_scene* scene = scenes[j];
        const aet_comp* comp = scene->comp;
        for (uint32_t k = scene->comp_count; k; k--, comp++)
            aet_comp_set_item_parent(load_data, comp);
    }
    return true;
}

void AetSet::Unload() {
    if (load_count > 1) {
        load_count--;
        return;
    }

    alloc_handler.reset();
    file_handler.reset();

    scenes = 0;
    ready = false;
    load_count = 0;
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
    if (index == info.set_index && ready && info.index < scenes_count)
        return scenes[info.index];
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
    return scenes[index]->end_time;
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
    return scenes[index]->name;
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
    return scenes[index]->start_time;
}

uint32_t AetSet::GetScenesCount() {
    return scenes_count;
}

AetMgr::AetMgr() : load_counter() {

}

AetMgr::~AetMgr() {

}

bool AetMgr::Init() {
    objects.clear();
    load_counter = 0;
    return true;
}

bool AetMgr::Ctrl() {
    for (auto& i : free_objects)
        objects.erase(i);
    free_objects.clear();

    for (auto& i : objects)
        i.second.Ctrl();
    return false;
}

bool AetMgr::Dest() {
    objects.clear();
    load_counter = 0;
    return true;
}

void AetMgr::Disp() {
    for (auto& i : objects)
        i.second.Disp();
}

void AetMgr::Basic() {
    auto i = objects.begin();
    auto i_end = objects.end();
    while (i != i_end) {
        if (i->second.StepFrame())
            FreeAetObject(i);
        i++;
    }
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

bool AetMgr::GetObjDisp(uint32_t id) {
    if (app::TaskWork::CheckTaskReady(this)) {
        AetObj* obj = AetMgr::GetObj(id);
        if (obj)
            return obj->GetDisp();
    }
    return true;
}

float_t AetMgr::GetObjFrame(uint32_t id) {
    if (app::TaskWork::CheckTaskReady(this)) {
        AetObj* obj = AetMgr::GetObj(id);
        if (obj)
            return obj->GetFrame();
    }
    return 0.0f;
}

bool AetMgr::GetObjVisible(uint32_t id) {
    if (app::TaskWork::CheckTaskReady(this)) {
        AetObj* obj = AetMgr::GetObj(id);
        if (obj)
            return obj->GetVisible();
    }
    return false;
}

AetSet* AetMgr::GetSet(int32_t index) {
    auto elem = sets.find(index);
    if (elem != sets.end())
        return &elem->second;
    return 0;
}

bool AetMgr::GetSetReady(int32_t index) {
    return GetSet(index)->ready;
}

void AetMgr::GetSetSceneCompLayerNames(std::vector<std::string>& vec, aet_info info) {
    AetSet* set = GetSet(info.set_index);
    set->GetSceneCompLayerNames(vec, set->GetSceneByInfo(info));
}

uint32_t AetMgr::GetSetScenesCount(int32_t index) {
    return GetSet(index)->GetScenesCount();
}

float_t AetMgr::GetSetSceneEndTime(aet_info info) {
    return GetSet(info.set_index)->GetSceneEndTime(info.index) - 1.0f;
}

float AetMgr::GetSetSceneLayerEndTime(aet_info info, const char* layer_name) {
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

    const aet_comp* comp = aet_scene_get_root_comp(scene);
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

bool AetMgr::LoadFile(int32_t index) {
    AetSet* set = GetSet(index);
    if (set)
        return set->LoadFile();
    return false;
}

void AetMgr::ReadFile(int32_t index, const char* file, std::string& mdata_dir, void* data) {
    const char* dir = "./rom/2d/";
    if (mdata_dir.size() && ((data_struct*)data)->check_directory_exists(mdata_dir.c_str()))
        dir = mdata_dir.c_str();

    AetSet* set = GetSet(index);
    if (set && ((data_struct*)data)->check_file_exists(dir, file))
        set->ReadFile(dir, file, data);
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

void AetMgr::UnloadSet(int32_t index) {
    AetSet* set = GetSet(index);
    if (set)
        set->Unload();
}

static aet_camera* aet_camera_load(aet_set_load_data& load_data, const aet_camera_file* cam_file) {
    aet_camera* cam = load_data.alloc_handler->allocate<aet_camera>();
    aet_fcurve_load(load_data, cam->eye_x, cam_file->eye_x);
    aet_fcurve_load(load_data, cam->eye_y, cam_file->eye_y);
    aet_fcurve_load(load_data, cam->eye_z, cam_file->eye_z);
    aet_fcurve_load(load_data, cam->pos_x, cam_file->pos_x);
    aet_fcurve_load(load_data, cam->pos_y, cam_file->pos_y);
    aet_fcurve_load(load_data, cam->pos_z, cam_file->pos_z);
    aet_fcurve_load(load_data, cam->dir_x, cam_file->dir_x);
    aet_fcurve_load(load_data, cam->dir_y, cam_file->dir_y);
    aet_fcurve_load(load_data, cam->dir_z, cam_file->dir_z);
    aet_fcurve_load(load_data, cam->rot_x, cam_file->rot_x);
    aet_fcurve_load(load_data, cam->rot_y, cam_file->rot_y);
    aet_fcurve_load(load_data, cam->rot_z, cam_file->rot_z);
    aet_fcurve_load(load_data, cam->zoom, cam_file->zoom);
    return cam;
}

static void aet_comp_set_item_parent(aet_set_load_data& load_data, const aet_comp* comp) {
    aet_layer* layer = (aet_layer*)comp->layers;
    for (uint32_t i = comp->layers_count; i; i--, layer++) {
        if (layer->item_type == AET_ITEM_TYPE_VIDEO
            || layer->item_type == AET_ITEM_TYPE_COMPOSITION) {
            uint32_t item_offset = layer->item.offset;
            layer->item.none = 0;

            if (item_offset) {
                auto elem = load_data.items.find(item_offset);
                if (elem != load_data.items.end())
                    layer->item = elem->second;
            }
        }

        uint32_t parent_offset = layer->parent_offset;
        layer->parent = 0;

        if (parent_offset) {
            auto elem = load_data.layers.find(parent_offset);
            if (elem != load_data.layers.end())
                layer->parent = elem->second;
        }
    }
}

static aet_comp* aet_comps_load(aet_set_load_data& load_data,
    size_t comp_count, const aet_comp_file* comps_file) {
    aet_comp* comps = load_data.alloc_handler->allocate<aet_comp>(comp_count);

    aet_comp* comp = comps;
    const aet_comp_file* comp_file = comps_file;

    for (size_t i = comp_count; i; i--, comp++, comp_file++) {
        load_data.items.insert_or_assign((uint32_t)((size_t)comp_file - load_data.data), comp);

        comp->layers_count = comp_file->layers_count;
        if (comp_file->layers_offset) {
            const aet_layer_file* layer_file = (const aet_layer_file*)(load_data.data + comp_file->layers_offset);

            aet_layer* layer = load_data.alloc_handler->allocate<aet_layer>(comp_file->layers_count);
            comp->layers = layer;

            for (uint32_t i = comp_file->layers_count; i; i--, layer++, layer_file++)
                aet_layer_load(load_data, layer, layer_file);
        }
        else
            comp->layers = 0;
    }
    return comps;
}

inline static void aet_fcurve_load(aet_set_load_data& load_data,
    aet_fcurve& fcurve, const aet_fcurve_file& fcurve_file) {
    fcurve.keys_count = fcurve_file.keys_count;
    if (fcurve_file.keys_offset)
        fcurve.keys = (const float_t*)(load_data.data + fcurve_file.keys_offset);
    else
        fcurve.keys = 0;
}

static aet_layer* aet_layer_load(aet_set_load_data& load_data,
    aet_layer* layer, const aet_layer_file* layer_file) {
    load_data.layers.insert_or_assign((uint32_t)((size_t)layer_file - load_data.data), layer);

    if (layer_file->name_offset)
        layer->name = (const char*)(load_data.data + layer_file->name_offset);
    else
        layer->name = 0;

    layer->start_time = layer_file->start_time;
    layer->end_time = layer_file->end_time;
    layer->offset_time = layer_file->offset_time;
    layer->time_scale = layer_file->time_scale;
    layer->flags = layer_file->flags;
    layer->quality = layer_file->quality;
    layer->item.none = 0;
    layer->item_type = layer_file->item_type;
    if ((layer_file->item_type == AET_ITEM_TYPE_VIDEO
        || layer_file->item_type == AET_ITEM_TYPE_COMPOSITION)
        && layer_file->item_offset)
        layer->item.offset = layer_file->item_offset;

    layer->parent = 0;
    if (layer_file->parent_offset)
        layer->parent_offset = layer_file->parent_offset;

    layer->markers_count = layer_file->markers_count;
    if (layer_file->markers_offset)
        layer->markers = aet_markers_load(load_data, layer_file->markers_count,
            (const aet_marker_file*)(load_data.data + layer_file->markers_offset));
    else
        layer->markers = 0;

    if (layer_file->video_offset)
        layer->video = aet_layer_video_load(load_data,
            (const aet_layer_video_file*)(load_data.data + layer_file->video_offset));
    else
        layer->video = 0;

    if (layer_file->audio_offset)
        layer->audio = aet_layer_audio_load(load_data,
            (const aet_layer_audio_file*)(load_data.data + layer_file->audio_offset));
    else
        layer->audio = 0;
    return layer;
}

static aet_layer_audio* aet_layer_audio_load(
    aet_set_load_data& load_data, const aet_layer_audio_file* layer_audio_file) {
    aet_layer_audio* layer_audio = load_data.alloc_handler->allocate<aet_layer_audio>();
    aet_fcurve_load(load_data, layer_audio->volume_l, layer_audio_file->volume_l);
    aet_fcurve_load(load_data, layer_audio->volume_r, layer_audio_file->volume_r);
    aet_fcurve_load(load_data, layer_audio->pan_l, layer_audio_file->pan_l);
    aet_fcurve_load(load_data, layer_audio->pan_r, layer_audio_file->pan_r);
    return layer_audio;
}

static aet_layer_video* aet_layer_video_load(
    aet_set_load_data& load_data, const aet_layer_video_file* layer_video_file) {
    aet_layer_video* layer_video = load_data.alloc_handler->allocate<aet_layer_video>();
    layer_video->transfer_mode = layer_video_file->transfer_mode;
    aet_fcurve_load(load_data, layer_video->anchor_x, layer_video_file->anchor_x);
    aet_fcurve_load(load_data, layer_video->anchor_y, layer_video_file->anchor_y);
    aet_fcurve_load(load_data, layer_video->pos_x, layer_video_file->pos_x);
    aet_fcurve_load(load_data, layer_video->pos_y, layer_video_file->pos_y);
    aet_fcurve_load(load_data, layer_video->rot_z, layer_video_file->rot_z);
    aet_fcurve_load(load_data, layer_video->scale_x, layer_video_file->scale_x);
    aet_fcurve_load(load_data, layer_video->scale_y, layer_video_file->scale_y);
    aet_fcurve_load(load_data, layer_video->opacity, layer_video_file->opacity);
    if (layer_video_file->_3d_offset)
        layer_video->_3d = aet_layer_video_3d_load(load_data,
            (const aet_layer_video_3d_file*)(load_data.data + layer_video_file->_3d_offset));
    return layer_video;
}

static aet_layer_video_3d* aet_layer_video_3d_load(
    aet_set_load_data& load_data, const aet_layer_video_3d_file* layer_video_3d_file) {
    aet_layer_video_3d* layer_video_3d = load_data.alloc_handler->allocate<aet_layer_video_3d>();
    aet_fcurve_load(load_data, layer_video_3d->anchor_z, layer_video_3d_file->anchor_z);
    aet_fcurve_load(load_data, layer_video_3d->pos_z, layer_video_3d_file->pos_z);
    aet_fcurve_load(load_data, layer_video_3d->dir_x, layer_video_3d_file->dir_x);
    aet_fcurve_load(load_data, layer_video_3d->dir_y, layer_video_3d_file->dir_y);
    aet_fcurve_load(load_data, layer_video_3d->dir_z, layer_video_3d_file->dir_z);
    aet_fcurve_load(load_data, layer_video_3d->rot_x, layer_video_3d_file->rot_x);
    aet_fcurve_load(load_data, layer_video_3d->rot_y, layer_video_3d_file->rot_y);
    aet_fcurve_load(load_data, layer_video_3d->scale_z, layer_video_3d_file->scale_z);
    return layer_video_3d;
}

static aet_marker* aet_markers_load(aet_set_load_data& load_data,
    size_t markers_count, const aet_marker_file* markers_file) {
    aet_marker* markers = load_data.alloc_handler->allocate<aet_marker>(markers_count);

    aet_marker* marker = markers;
    const aet_marker_file* marker_file = markers_file;

    for (size_t i = markers_count; i; i--, marker++, marker_file++) {
        marker->time = marker_file->time;
        if (marker_file->name_offset)
            marker->name = (const char*)(load_data.data + marker_file->name_offset);
        else
            marker->name = 0;
    }
    return markers;
}

static const aet_comp* aet_scene_get_root_comp(const aet_scene* scene) {
    if (scene && scene->comp_count)
        return &scene->comp[scene->comp_count - 1];
    return 0;
}

static aet_scene* aet_scene_load(aet_set_load_data& load_data,
    aet_scene* scene, const aet_scene_file* scene_file) {
    if (scene_file->name_offset)
        scene->name = (const char*)(load_data.data + scene_file->name_offset);
    else
        scene->name = 0;

    scene->start_time = scene_file->start_time;
    scene->end_time = scene_file->end_time;
    scene->fps = scene_file->fps;
    scene->color[0] = scene_file->color[0];
    scene->color[1] = scene_file->color[1];
    scene->color[2] = scene_file->color[2];
    scene->width = scene_file->width;
    scene->height = scene_file->height;

    if (scene_file->camera_offset)
        scene->camera = aet_camera_load(load_data,
            (const aet_camera_file*)(load_data.data + scene_file->camera_offset));
    else
        scene->camera = 0;

    scene->comp_count = scene_file->comp_count;
    if (scene_file->comp_offset)
        scene->comp = aet_comps_load(load_data, scene_file->comp_count,
            (const aet_comp_file*)(load_data.data + scene_file->comp_offset));
    else
        scene->comp = 0;

    scene->video_count = scene_file->video_count;
    const aet_video_file* video_file;
    if (scene_file->video_offset)
        video_file = (const aet_video_file*)(load_data.data + scene_file->video_offset);
    else
        video_file = 0;

    aet_video* video = load_data.alloc_handler->allocate<aet_video>(scene_file->video_count);
    scene->video = video;
    for (uint32_t i = scene_file->video_count; i; i--, video++, video_file++)
        aet_video_load(load_data, video, video_file);

    scene->audio_count = scene_file->audio_count;
    if (scene_file->audio_offset)
        scene->audio = (aet_audio*)(load_data.data + scene_file->audio_offset);
    else
        scene->audio = 0;
    return scene;
}

static aet_video* aet_video_load(aet_set_load_data& load_data,
    aet_video* video, const aet_video_file* video_file) {
    load_data.items.insert_or_assign((uint32_t)((size_t)video_file - load_data.data), video);
    video->color[0] = video_file->color[0];
    video->color[1] = video_file->color[1];
    video->color[2] = video_file->color[2];
    video->width = video_file->width;
    video->height = video_file->height;
    video->fpf = video_file->fpf;
    video->sources_count = video_file->sources_count;

    if (video_file->sources_offset) {
        const aet_video_src_file* source_file
            = (const aet_video_src_file*)(load_data.data + video_file->sources_offset);

        aet_video_src* source = load_data.alloc_handler->allocate<aet_video_src>(video_file->sources_count);
        video->sources = source;

        for (uint32_t i = video_file->sources_count; i; i--, source++, source_file++) {
            if (source_file->sprite_name_offset)
                source->sprite_name = (const char*)(load_data.data + source_file->sprite_name_offset);
            else
                source->sprite_name = 0;
            source->sprite_index = source_file->sprite_index;
        }
    }
    else
        video->sources = 0;
    return video;
}