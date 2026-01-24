/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "motion.hpp"
#include "../../KKdLib/io/path.hpp"
#include "../data.hpp"

static const mothead_mot mothead_mot_null;

std::map<uint32_t, MhdFile> mothead_storage_data;
std::map<uint32_t, MotFile> motion_storage_data;

MhdFile::MhdFile() : data(), set(), load_count() {
    FreeData();
}

MhdFile::~MhdFile() {
    FreeData();
}

bool MhdFile::CheckNotReady() {
    return file_handler.check_not_ready();
}

void MhdFile::FreeData() {
    data = 0;
    alloc_handler.reset();
    set = -1;
    file_path.clear();
    file_handler.reset();
    load_count = 0;
}

void MhdFile::LoadFile(const char* path, const char* file, uint32_t set) {
    if (load_count > 0) {
        load_count++;
        return;
    }

    this->set = set;
    file_path.assign(path);
    file_path.append(file);
    if (file_handler.read_file(&data_list[DATA_AFT], path, file))
        file_handler.set_callback_data(0, (PFNFILEHANDLERCALLBACK*)ParseFileParent, this);
    load_count = 1;
}

void MhdFile::ParseFile(const void* data) {
    alloc_handler = prj::shared_ptr<prj::stack_allocator>(new prj::stack_allocator);
    mothead_file* mhdf = (mothead_file*)((size_t)data + ((uint32_t*)data)[1]);
    this->data = MhdFile::ParseMothead(mhdf, (size_t)mhdf);
}

mothead* MhdFile::ParseMothead(mothead_file* mhdf, size_t data) {
    mothead* mhd = alloc_handler->allocate<mothead>();
    mhd->mot_set_id = mhdf->mot_set_id;
    mhd->first_mot_id = mhdf->first_mot_id;
    mhd->last_mot_id = mhdf->last_mot_id;
    uint32_t* mot_offsets = (uint32_t*)(data + mhdf->mot_offsets_offset);
    size_t mot_count = (size_t)mhdf->last_mot_id - mhdf->first_mot_id + 1;
    mhd->mots = alloc_handler->allocate<mothead_mot*>(mot_count);
    mothead_mot** mots = mhd->mots;
    for (; mot_count; mot_count--) {
        *mots = 0;
        if (*mot_offsets)
            *mots = ParseMotheadMot((mothead_mot_file*)(data + *mot_offsets), data);
        mots++;
        mot_offsets++;
    }
    return mhd;
}

mothead_mot* MhdFile::ParseMotheadMot(mothead_mot_file* mhdsf, size_t data) {
    struct mothead_mot_data_file {
        int32_t type;
        uint32_t offset;
    };

    struct mothead_data_file {
        mothead_data_type type;
        int32_t frame;
        uint32_t offset;
    };

    mothead_mot* mhdm = alloc_handler->allocate<mothead_mot>();
    mhdm->field_0.field_0 = mhdsf->field_0;
    mhdm->field_0.field_4 = mhdsf->field_4;
    mhdm->field_0.field_8 = mhdsf->field_8;
    mhdm->field_0.field_C = mhdsf->field_C;
    mhdm->field_10 = mhdsf->field_10;
    mhdm->field_12 = mhdsf->field_12;

    if (mhdsf->mot_data_offset) {
        mothead_mot_data_file* mot_data_file = (mothead_mot_data_file*)(data + mhdsf->mot_data_offset);
        mothead_mot_data_file* mot_data_file_end = mot_data_file;
        while ((mot_data_file_end++)->type >= 0);

        size_t count = mot_data_file_end - mot_data_file;
        mhdm->mot_data = alloc_handler->allocate<mothead_mot_data>(count);
        mothead_mot_data* mot_data = mhdm->mot_data;
        for (size_t i = count; i; i--, mot_data_file++, mot_data++) {
            mot_data->type = mot_data_file->type;
            mot_data->data = (void*)(data + mot_data_file->offset);
        }
    }

    if (mhdsf->data_offset) {
        mothead_data_file* data_file = (mothead_data_file*)(data + mhdsf->data_offset);
        mothead_data_file* data_file_end = data_file;
        while ((data_file_end++)->type >= 0);

        size_t count = data_file_end - data_file;
        mhdm->data = alloc_handler->allocate<mothead_data>(count);
        mothead_data* _data = mhdm->data;
        for (size_t i = count; i; i--, data_file++, _data++) {
            _data->type = data_file->type;
            _data->frame = data_file->frame;
            _data->data = (void*)(data + data_file->offset);
        }
    }

    if (mhdsf->field_1C) {
        uint32_t* v29 = (uint32_t*)(data + mhdsf->field_1C);
        uint32_t* v30 = v29;
        while (*v30++);

        size_t count = v30 - v29;
        mhdm->field_28 = alloc_handler->allocate<int64_t>(count);
        int64_t* v33 = mhdm->field_28;
        for (size_t i = count; i; i--)
            *v33++ = data + *v29++;
    }
    return mhdm;
}

bool MhdFile::Unload() {
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

void MhdFile::ParseFileParent(MhdFile* mhd, const void* file_data, size_t size) {
    mhd->ParseFile(file_data);
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

void MotFile::LoadFile(std::string&& mdata_dir, uint32_t set) {
    if (load_count > 0) {
        load_count++;
        return;
    }

    data_struct* aft_data = &data_list[DATA_AFT];

    std::string rom_dir;
    rom_dir.assign("rom/");

    std::string rob_dir;
    rob_dir.assign("rob/");

    std::string farc_file;
    farc_file.assign("mot_");
    farc_file.append(mot_set_info->name);
    farc_file.append(".farc");

    std::string mot_file;
    mot_file.assign("mot_");
    mot_file.append(mot_set_info->name);
    mot_file.append(".bin");

    if (mdata_dir.size())
        if (aft_data->check_file_exists(mdata_dir.c_str(), farc_file.c_str())) {
            rom_dir.assign(mdata_dir);
            rob_dir.clear();
        }

    std::string dir;
    dir.assign(rom_dir);
    dir.append(rob_dir);
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

void motion_set_load_mothead(uint32_t set, std::string&& mdata_dir, const motion_database* mot_db) {
    const motion_set_info* set_info = mot_db->get_motion_set_by_id(set);
    if (!set_info) {
        printf_debug_error("request_load_mhd_file(): illegal file_id(%d).\n", set);
        return;
    }

    std::string file;
    file.assign("mothead_");
    file.append(set_info->name);
    file.append(".bin");

    std::string path("ram/rob/");
    path.append(file);
    if (path_check_file_exists(path.c_str()))
        path.erase(path.size() - file.size());
    else {
        path.assign("rom/rob/");
        if (data_list[DATA_AFT].check_file_exists(mdata_dir.c_str(), file.c_str()))
            path.assign(mdata_dir);
    }

    auto elem = mothead_storage_data.find(set);
    if (elem == mothead_storage_data.end())
        elem = mothead_storage_data.insert({ set, {} }).first;

    elem->second.LoadFile(path.c_str(), file.c_str(), set);
}

void motion_set_load_motion(uint32_t set, std::string&& mdata_dir, const motion_database* mot_db) {
    const motion_set_info* set_info = mot_db->get_motion_set_by_id(set);
    if (!set_info)
        return;

    auto elem = motion_storage_data.find(set);
    if (elem == motion_storage_data.end())
        elem = motion_storage_data.insert({ set, {} }).first;

    elem->second.mot_set_info = set_info;
    elem->second.LoadFile(std::string(mdata_dir), set);
}

void motion_set_unload_mothead(uint32_t set) {
    auto elem = mothead_storage_data.find(set);
    if (elem != mothead_storage_data.end() && elem->second.Unload())
        mothead_storage_data.erase(elem);
}

void motion_set_unload_motion(uint32_t set) {
    auto elem = motion_storage_data.find(set);
    if (elem != motion_storage_data.end() && elem->second.Unload())
        motion_storage_data.erase(elem);
}

bool mothead_storage_check_mhd_file_not_ready(uint32_t set_id) {
    auto elem = mothead_storage_data.find(set_id);
    if (elem != mothead_storage_data.end())
        return elem->second.CheckNotReady();
    return false;
}

MhdFile* mothead_storage_get_mhd_file(uint32_t set_id) {
    auto elem = mothead_storage_data.find(set_id);
    if (elem != mothead_storage_data.end())
        return &elem->second;
    return 0;
}

const mothead_mot* mothead_storage_get_mot_by_motion_id(
    uint32_t motion_id, const motion_database* mot_db) {
    uint32_t set_id = mot_db->get_motion_set_id_by_motion_id(motion_id);
    if (set_id == -1)
        return &mothead_mot_null;

    auto elem = mothead_storage_data.find(set_id);
    if (elem == mothead_storage_data.end() || !elem->second.data)
        return &mothead_mot_null;

    mothead* mhd = elem->second.data;
    if ((int32_t)motion_id < mhd->first_mot_id || (int32_t)motion_id > mhd->last_mot_id)
        return &mothead_mot_null;

    mothead_mot* mot = mhd->mots[(ssize_t)(int32_t)motion_id - mhd->first_mot_id];
    if (!mot)
        return &mothead_mot_null;
    return mot;
}

bool motion_storage_check_mot_file_not_ready(uint32_t set_id) {
    auto elem = motion_storage_data.find(set_id);
    if (elem != motion_storage_data.end())
        return elem->second.CheckNotReady();
    return false;
}

MotFile* motion_storage_get_mot_file(uint32_t set_id) {
    auto elem = motion_storage_data.find(set_id);
    if (elem != motion_storage_data.end())
        return &elem->second;
    return 0;
}

const mot_data* motion_storage_get_mot_data(uint32_t motion_id, const motion_database* mot_db) {
    const motion_set_info* set_info = mot_db->get_motion_set_by_motion_id(motion_id);
    if (!set_info) {
        printf_debug_info("%08x : illegal motfile Num.\n", motion_id);
        return 0;
    }

    for (const motion_info& i : set_info->motion) {
        if (i.id != motion_id)
            continue;

        auto elem = motion_storage_data.find(set_info->id);
        if (elem != motion_storage_data.end())
            return &elem->second.mot_set->mot_data[&i - set_info->motion.data()];
        return 0;
    }

    return 0;
}

float_t motion_storage_get_mot_data_frame_count(uint32_t motion_id, const motion_database* mot_db) {
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
    mothead_storage_data = {};
    motion_storage_data = {};
}

void motion_free() {
    motion_storage_data.clear();
    mothead_storage_data.clear();
}
