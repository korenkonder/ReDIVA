/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "rob.hpp"

struct MhdListFile {
    int32_t type;
    uint32_t data_offset;
};

struct MhpListFile {
    int32_t type;
    int32_t frame;
    uint32_t data_offset;
};

struct MhcListFile {
    uint32_t data_offset;
};

struct MhdDataFile {
    MhdKind mot_kind;
    uint16_t start_style;
    uint16_t end_style;
    uint32_t mh_list_offset;
    uint32_t pp_list_offset;
    uint32_t cm_list_offset;
};

struct MhdFileHeaderFile {
    uint32_t file_id;
    uint32_t min_mot_id;
    uint32_t max_mot_id;
    uint32_t data_list_offsets_offset;
};

/*struct MhdFileHeaderFileModern {
    uint32_t file_id;
    uint32_t num_mot;
    int64_t ids_offset;
    int64_t data_list_offsets_offset;
};*/

struct MhdFile {
    MhdFileHeader* data;
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
    MhdFileHeader* ParseMothead(MhdFileHeaderFile* mhdf, size_t data);
    MhdData* ParseMotheadMot(MhdDataFile* mhdf, size_t data);
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

extern const mot_data* get_motiondata_address(uint32_t mot_id, const motion_database* mot_db);
extern float_t get_mot_frame_max(uint32_t mot_id, const motion_database* mot_db);

extern void motion_set_load_mothead(uint32_t set, std::string&& mdata_dir, const motion_database* mot_db);
extern void motion_set_load_motion(uint32_t set, std::string&& mdata_dir, const motion_database* mot_db);
extern void motion_set_unload_mothead(uint32_t set);
extern void motion_set_unload_motion(uint32_t set);

extern bool mothead_storage_check_mhd_file_not_ready(uint32_t set_id);
extern MhdFile* mothead_storage_get_mhd_file(uint32_t set_id);
extern const MhdData* mothead_storage_get_mot_by_motion_id(
    uint32_t motion_id, const motion_database* mot_db);
extern bool motion_storage_check_mot_file_not_ready(uint32_t set_id);
extern MotFile* motion_storage_get_mot_file(uint32_t set_id);
extern const mot_set* motion_storage_get_motion_set(uint32_t set_id);

extern void motion_init();
extern void motion_free();
