/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "motion.hpp"

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
    void LoadFile(std::string* mdata_dir, uint32_t set);
    void ParseFile(const void* data, size_t size);
    bool Unload();

    static void ParseFileParent(MotFile* mot, const void* file_data, size_t size);
};

std::map<uint32_t, MotFile> motion_storage_data;

void motion_set_load_motion(uint32_t set, std::string* mdata_dir, motion_database* mot_db) {
    const motion_set_info* set_info = mot_db->get_motion_set_by_id(set);
    if (!set_info)
        return;

    auto elem = motion_storage_data.find(set);
    if (elem == motion_storage_data.end())
        elem = motion_storage_data.insert({ set, {} }).first;

    elem->second.mot_set_info = set_info;
    elem->second.LoadFile(mdata_dir, set);
}

void motion_set_unload_motion(uint32_t set) {
    auto elem = motion_storage_data.find(set);
    if (elem != motion_storage_data.end() && elem->second.Unload())
        motion_storage_data.erase(elem);
}

bool motion_storage_check_mot_file_not_ready(uint32_t set_id) {
    auto elem = motion_storage_data.find(set_id);
    if (elem != motion_storage_data.end())
        return elem->second.CheckNotReady();
    return false;
}

const mot_data* motion_storage_get_mot_data(uint32_t motion_id, motion_database* mot_db) {
    uint32_t set_id = -1;
    size_t motion_index = -1;
    for (motion_set_info& i : mot_db->motion_set) {
        for (motion_info& j : i.motion)
            if (j.id == motion_id) {
                set_id = i.id;
                motion_index = &j - i.motion.data();

                break;
            }

        if (set_id != -1)
            break;
    }

    if (set_id == -1)
        return 0;

    auto elem = motion_storage_data.find(set_id);
    if (elem != motion_storage_data.end())
        return &elem->second.mot_set->mot_data[motion_index];
    return 0;
}

float_t motion_storage_get_mot_data_frame_count(uint32_t motion_id, motion_database* mot_db) {
    const mot_data* mot = motion_storage_get_mot_data(motion_id, mot_db);
    if (mot)
        return mot->frame_count;
    return 0.0f;
}

const mot_set* motion_storage_get_motion_set(uint32_t set_id) {
    auto elem = motion_storage_data.find(set_id);
    if (elem != motion_storage_data.end())
        return elem->second.mot_set;
    return 0;
}

void motion_init() {
    motion_storage_data = {};
}

void motion_free() {
    motion_storage_data.clear();
}

MotFile::MotFile() : mot_set_info(), mot_set(), load_count() {
    FreeData();
}

MotFile:: ~MotFile() {
    FreeData();
}

bool MotFile::CheckNotReady() {
    return file_handler.check_not_ready();
}

void MotFile::FreeData() {
    mot_set_info = 0;
    alloc_handler.reset();
    mot_set = 0;
    file_handler.reset();
    load_count = 0;
}

void MotFile::LoadFile(std::string* mdata_dir, uint32_t set) {
    if (load_count > 0) {
        load_count++;
        return;
    }

    data_struct* aft_data = &data_list[DATA_AFT];

    std::string rom_dir = "rom/";
    std::string rob_dir = "rob/";
    std::string farc_file = "mot_" + mot_set_info->name + ".farc";
    std::string mot_file = "mot_" + mot_set_info->name + ".bin";
    if (mdata_dir && mdata_dir->size())
        if (aft_data->check_file_exists(mdata_dir->c_str(), farc_file.c_str())) {
            rom_dir = *mdata_dir;
            rob_dir.clear();
        }

    std::string dir = rom_dir + rob_dir;
    if (aft_data->check_file_exists(dir.c_str(), farc_file.c_str())) {
        if (file_handler.read_file(aft_data, dir.c_str(), farc_file.c_str(), mot_file.c_str(), false))
            file_handler.set_callback_data(0, (PFNFILEHANDLERCALLBACK*)ParseFileParent, this);
    }
    else {
        if (file_handler.read_file(aft_data, rom_dir.c_str(), mot_file.c_str()))
            file_handler.set_callback_data(0, (PFNFILEHANDLERCALLBACK*)ParseFileParent, this);
    }
    load_count = 1;
}

void MotFile::ParseFile(const void* data, size_t size) {
    prj::shared_ptr<prj::stack_allocator>& alloc = alloc_handler;
    alloc = prj::shared_ptr<prj::stack_allocator>(new prj::stack_allocator);

    ::mot_set* set = alloc->allocate<::mot_set>();
    mot_set = set;
    set->unpack_file(alloc, data, size, false);
}

bool MotFile::Unload() {
    if (--load_count < 0) {
        load_count = 0;
        return true;
    }
    else if (load_count <= 0) {
        FreeData();
        return true;
    }
    else
        return false;
}

void MotFile::ParseFileParent(MotFile* mot, const void* file_data, size_t size) {
    mot->ParseFile(file_data, size);
}
