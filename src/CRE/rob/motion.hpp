/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "rob.hpp"

struct MhdFile {
    mothead* data;
    uint32_t set;
    std::string file_path; // Added
    p_file_handler file_handler;
    prj::shared_ptr<prj::stack_allocator> alloc_handler;
    int32_t load_count;

    MhdFile();
    virtual ~MhdFile();

    bool CheckNotReady();
    void FreeData();
    void LoadFile(const char* path, const char* file, uint32_t set);
    void ParseFile(const void* data);
    mothead* ParseMothead(mothead_file* mhdsf, size_t data);
    mothead_mot* ParseMotheadMot(mothead_mot_file* mhdsf, size_t data);
    bool Unload();

    static void ParseFileParent(MhdFile* mhd, const void* file_data, size_t size);
};

struct MotFile {
    const motion_set_info* mot_set_info;
    prj::shared_ptr<prj::stack_allocator> alloc_handler;
    mot_set* mot_set;
    p_file_handler file_handler;
    int32_t load_count;

    MotFile();
    virtual ~MotFile();

    bool CheckNotReady();
    void FreeData();
    void LoadFile(std::string&& mdata_dir, uint32_t set);
    void ParseFile(const void* data, size_t size);
    bool Unload();

    static void ParseFileParent(MotFile* mot, const void* file_data, size_t size);
};

extern void motion_set_load_mothead(uint32_t set, std::string&& mdata_dir, const motion_database* mot_db);
extern void motion_set_load_motion(uint32_t set, std::string&& mdata_dir, const motion_database* mot_db);
extern void motion_set_unload_mothead(uint32_t set);
extern void motion_set_unload_motion(uint32_t set);

extern bool mothead_storage_check_mhd_file_not_ready(uint32_t set_id);
extern MhdFile* mothead_storage_get_mhd_file(uint32_t set_id);
extern const mothead_mot* mothead_storage_get_mot_by_motion_id(
    uint32_t motion_id, const motion_database* mot_db);
extern bool motion_storage_check_mot_file_not_ready(uint32_t set_id);
extern MotFile* motion_storage_get_mot_file(uint32_t set_id);
extern const mot_data* motion_storage_get_mot_data(uint32_t motion_id, const motion_database* mot_db);
extern float_t motion_storage_get_mot_data_frame_count(uint32_t motion_id, const motion_database* mot_db);
extern const mot_set* motion_storage_get_motion_set(uint32_t set_id);

extern void motion_init();
extern void motion_free();
