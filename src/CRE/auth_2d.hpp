/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../KKdLib/database/aet.hpp"
#include "../KKdLib/aet.hpp"
#include "../KKdLib/vec.hpp"
#include "color.hpp"
#include "frame_rate_control.hpp"
#include "file_handler.hpp"
#include "sprite.hpp"

enum AetFlags {
    AET_END       = 0x000001,
    AET_REVERSE   = 0x000002,
    AET_PAUSE     = 0x000004,
    AET_HIDDEN    = 0x000008,
    AET_LOOP      = 0x010000,
    AET_PLAY_ONCE = 0x020000,
    AET_FLIP_H    = 0x040000,
    AET_FLIP_V    = 0x080000,
    AET_100000    = 0x100000,
};

struct AetArgs {
    union IDUnion {
        int32_t index;
        uint32_t id;
        aet_info info;

        IDUnion() {
            index = -1;
            id = -1;
            info = {};
        }
    };

    IDUnion id;
    const char* layer_name;
    const char* start_marker;
    const char* end_marker;
    float_t start_time;
    float_t end_time;
    AetFlags flags;
    int32_t index;
    int32_t layer;
    spr::SprPrio prio;
    resolution_mode mode;
    vec3 pos;
    vec3 rot;
    vec3 scale;
    vec3 anchor;
    float_t frame_speed;
    vec4 color;
    std::map<std::string, int32_t> layer_sprite;
    std::string sound_path;
    std::map<std::string, std::string> sound_replace;
    int32_t sound_queue_index;
    std::map<uint32_t, uint32_t> sprite_replace;
    std::map<uint32_t, texture*> sprite_texture;
    std::map<uint32_t, uint32_t> sprite_discard;
    FrameRateControl* frame_rate_control;
    bool sound_voice;

    const sprite_database* spr_db;

    AetArgs();
    ~AetArgs();

    AetArgs& operator=(const AetArgs& other);
};

struct aet_layout_data {
    mat4 mat;
    vec3 position;
    vec3 anchor;
    float_t width;
    float_t height;
    float_t opacity;
    color4u8 color;
    resolution_mode mode;

    aet_layout_data();

    static void put_sprite(int32_t spr_id, spr::SprAttr attr, spr::SprPrio prio,
        const vec2* pos, const aet_layout_data* layout, const sprite_database* spr_db);
    static void set_args(const aet_layout_data* layout, spr::SprArgs* args);
};

struct AetComp {
    std::map<std::string, aet_layout_data> data;

    AetComp();
    ~AetComp();

    void Add(const char* name, const aet_layout_data& data);
    void Clear();
    const aet_layout_data* Find(const char* name);

    static void put_number_sprite(int32_t value, int32_t max_digits,
        AetComp* comp, const char** names, const int32_t* spr_ids, spr::SprPrio prio,
        const vec2* pos, bool all_digits, const sprite_database* spr_db);
};

class AetSet {
public:
    uint32_t index;
    ::aet_set* aet_set;
    int32_t load_count;
    p_file_handler file_handler;
    bool ready;
    prj::shared_ptr<prj::stack_allocator> alloc_handler;

    std::string name;
    std::vector<uint32_t> aet_ids;
    uint32_t hash;

    AetSet(uint32_t index);
    virtual ~AetSet();

    virtual void ReadFile(const char* dir, const char* file, void* data);
    virtual bool LoadFile();
    virtual void Unload();
    virtual bool Load();

    virtual void ReadFileModern(uint32_t set_hash, void* data);
    virtual bool LoadFileModern(aet_database* aet_db);
    virtual void UnloadModern(aet_database* aet_db);
    virtual bool LoadModern(aet_database* aet_db);

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

extern void aet_manager_init();
extern void aet_manager_add_aet_sets(const aet_database* aet_db);
extern bool aet_manager_add_task();
extern bool aet_manager_check_task_ready();
extern bool aet_manager_del_task();
extern void aet_manager_free_aet_object(uint32_t id);
extern void aet_manager_free_aet_object_reset(uint32_t* id);
extern void aet_manager_free_aet_set_objects(uint32_t set_id, const aet_database* aet_db);
extern AetSet* aet_manager_get_aet_set(uint32_t set_id, const aet_database* aet_db);
extern bool aet_manager_get_obj_end(uint32_t id);
extern float_t aet_manager_get_obj_frame(uint32_t id);
extern bool aet_manager_get_obj_visible(uint32_t id);
extern void aet_manager_get_scene_comp_layer_names(std::vector<std::string>& vec,
    uint32_t aet_id, const aet_database* aet_db);
extern float_t aet_manager_get_scene_end_time(uint32_t aet_id, const aet_database* aet_db);
extern float_t aet_manager_get_scene_layer_end_time(uint32_t aet_id,
    const char* layer_name, const aet_database* aet_db);
extern void aet_manager_get_scene_layer_marker_names(std::vector<std::string>& vec,
    uint32_t aet_id, const char* layer_name, const aet_database* aet_db);
extern float_t aet_manager_get_scene_layer_marker_time(uint32_t aet_id,
    const char* scene_name, const char* marker_name, const aet_database* aet_db);
extern float_t aet_manager_get_scene_layer_start_time(uint32_t aet_id,
    const char* layer_name, const aet_database* aet_db);
extern const char* aet_manager_get_scene_name(uint32_t aet_id, const aet_database* aet_db);
extern float_t aet_manager_get_scene_start_time(uint32_t aet_id, const aet_database* aet_db);
extern uint32_t aet_manager_get_scenes_count(uint32_t set_id, const aet_database* aet_db);
extern ::aet_set* aet_manager_get_set(uint32_t set_id, const aet_database* aet_db);
extern bool aet_manager_get_set_ready(uint32_t set_id, const aet_database* aet_db);
extern void aet_manager_init_aet_layout(AetComp* composition,
    AetArgs args, const aet_database* aet_db);
extern void aet_manager_init_aet_layout(AetComp* composition, uint32_t aet_id, const char* layer_name,
    AetFlags flags, resolution_mode mode, const char* start_marker, float_t start_time,
    const aet_database* aet_db, const sprite_database* spr_db);
extern uint32_t aet_manager_init_aet_object(AetArgs args, const aet_database* aet_db);
extern uint32_t aet_manager_init_aet_object(uint32_t id, const char* layer_name,
    spr::SprPrio prio, AetFlags flags, const char* start_marker, const char* end_marker,
    const aet_database* aet_db, const sprite_database* spr_db);
extern uint32_t aet_manager_init_aet_object(uint32_t aet_id, spr::SprPrio prio, AetFlags flags,
    const char* layer_name, const vec2* pos, int32_t index, const char* start_marker, const char* end_marker,
    float_t start_time, float_t end_time, const vec2* scale, FrameRateControl* frame_rate_control,
    const aet_database* aet_db, const sprite_database* spr_db);
extern bool aet_manager_load_file(uint32_t set_id, const aet_database* aet_db);
extern bool aet_manager_load_file_modern(uint32_t set_hash, aet_database* aet_db);
extern void aet_manager_read_file(uint32_t set_id,
    std::string& mdata_dir, void* data, const aet_database* aet_db);
extern void aet_manager_read_file(uint32_t set_id,
    std::string&& mdata_dir, void* data, const aet_database* aet_db);
extern void aet_manager_read_file_modern(uint32_t set_hash, void* data, aet_database* aet_db);
extern void aet_manager_remove_aet_sets(const aet_database* aet_db);
extern void aet_manager_set_obj_alpha(uint32_t id, float_t value);
extern void aet_manager_set_obj_color(uint32_t id, const vec4& value);
extern void aet_manager_set_obj_end_time(uint32_t id, float_t value);
extern void aet_manager_set_obj_frame(uint32_t id, float_t value);
extern void aet_manager_set_obj_frame_rate_control(uint32_t id, FrameRateControl* value);
extern void aet_manager_set_obj_play(uint32_t id, bool value);
extern void aet_manager_set_obj_position(uint32_t id, const vec3& value);
extern void aet_manager_set_obj_prio(uint32_t id, spr::SprPrio value);
extern void aet_manager_set_obj_rotation(uint32_t id, const vec3& value);
extern void aet_manager_set_obj_scale(uint32_t id, const vec3& value);
extern void aet_manager_set_obj_sprite_discard(uint32_t id, std::map<uint32_t, uint32_t>& value);
extern void aet_manager_set_obj_sprite_replace(uint32_t id, std::map<uint32_t, uint32_t>& value);
extern void aet_manager_set_obj_sprite_texture(uint32_t id, std::map<uint32_t, texture*>& value);
extern void aet_manager_set_obj_visible(uint32_t id, bool value);
extern void aet_manager_unload_set(uint32_t set_id, const aet_database* aet_db);
extern void aet_manager_unload_set_modern(uint32_t set_hash, aet_database* aet_db);
extern void aet_manager_free();
