/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../KKdLib/database/aet.hpp"
#include "../KKdLib/vec.hpp"
#include "frame_rate_control.hpp"
#include "sprite.hpp"

enum AetFlags {
    AET_DISP      = 0x000001,
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
};

struct aet_layout_data {
    mat4 mat;
    vec3 position;
    vec3 anchor;
    float_t width;
    float_t height;
    float_t opacity;
    vec4u8 color;
    resolution_mode mode;

    aet_layout_data();
};

struct AetComp {
    std::map<std::string, aet_layout_data> data;

    AetComp();
    ~AetComp();

    void Add(const char* name, aet_layout_data& data);
    aet_layout_data* Find(const char* name);
};

extern void aet_manager_init();
extern void aet_manager_add_aet_sets(const aet_database* aet_db);
extern bool aet_manager_add_task();
extern bool aet_manager_check_task_ready();
extern bool aet_manager_del_task();
extern void aet_manager_free_aet_object(uint32_t id);
extern void aet_manager_free_aet_object_reset(uint32_t* id);
extern void aet_manager_free_aet_set_objects(uint32_t set_id, const aet_database* aet_db);
extern bool aet_manager_get_obj_disp(uint32_t id);
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
extern bool aet_manager_get_set_ready(uint32_t set_id, const aet_database* aet_db);
extern void aet_manager_init_aet_layout(AetComp* composition,
    AetArgs args, const aet_database* aet_db);
extern void aet_manager_init_aet_layout(AetComp* composition,
    uint32_t aet_id, const char* layer_name, AetFlags flags, resolution_mode mode,
    const char* start_marker, float_t start_time, const aet_database* aet_db);
extern uint32_t aet_manager_init_aet_object(AetArgs args, const aet_database* aet_db);
extern uint32_t aet_manager_init_aet_object(uint32_t id, const char* layer_name, spr::SprPrio prio,
    AetFlags flags, const char* start_marker, const char* end_marker, const aet_database* aet_db);
extern uint32_t aet_manager_init_aet_object(uint32_t aet_id, spr::SprPrio prio,
    AetFlags flags, const char* layer_name, const vec2* pos, int32_t index,
    const char* start_marker, const char* end_marker, float_t start_time, float_t end_time,
    const vec2* scale, FrameRateControl* frame_rate_control, const aet_database* aet_db);
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
