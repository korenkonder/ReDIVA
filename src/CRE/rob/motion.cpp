/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "motion.hpp"
#include "../../KKdLib/io/path.hpp"
#include "../data.hpp"

static const MhdData mothead_mot_null;

std::map<uint32_t, MhdFile> mothead_storage_data;
std::map<uint32_t, MotFile> sp_mot_data;

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
    if (file_handler.read_file(&data_list[DATA_AFT], path, file, prj::MemCSystem))
        file_handler.set_callback_data(0, (PFNFILEHANDLERCALLBACK*)ParseFileParent, this);
    load_count = 1;
}

void MhdFile::ParseFile(const void* data) {
    alloc_handler = prj::shared_ptr<prj::stack_allocator>(new prj::stack_allocator);
    MhdFileHeaderFile* mhdf = (MhdFileHeaderFile*)((size_t)data + ((uint32_t*)data)[1]);
    this->data = MhdFile::ParseMothead(mhdf, (size_t)mhdf);
}

MhdFileHeader* MhdFile::ParseMothead(MhdFileHeaderFile* mhdf, size_t data) {
    MhdFileHeader* mhd = alloc_handler->allocate<MhdFileHeader>();
    mhd->file_id = mhdf->file_id;
    mhd->min_mot_id = mhdf->min_mot_id;
    mhd->max_mot_id = mhdf->max_mot_id;

    uint32_t* data_list_offsets = (uint32_t*)(data + mhdf->data_list_offsets_offset);
    size_t mot_count = (size_t)mhdf->max_mot_id - mhdf->min_mot_id + 1;
    mhd->data_list = alloc_handler->allocate<MhdData*>(mot_count);
    MhdData** data_list = mhd->data_list;
    for (; mot_count; mot_count--, data_list_offsets++, data_list++) {
        *data_list = 0;
        if (*data_list_offsets)
            *data_list = ParseMotheadMot((MhdDataFile*)(data + *data_list_offsets), data);
    }
    return mhd;
}

MhdData* MhdFile::ParseMotheadMot(MhdDataFile* mhdf, size_t data) {
    MhdData* mhdm = alloc_handler->allocate<MhdData>();
    mhdm->mot_kind = mhdf->mot_kind;
    mhdm->start_style = mhdf->start_style;
    mhdm->end_style = mhdf->end_style;

    if (mhdf->mh_list_offset) {
        MhdListFile* mh_list_file = (MhdListFile*)(data + mhdf->mh_list_offset);
        MhdListFile* mh_list_file_end = mh_list_file;
        while ((mh_list_file_end++)->type >= 0);

        size_t count = mh_list_file_end - mh_list_file;
        mhdm->mh_list = alloc_handler->allocate<MhdList>(count);
        MhdList* mh_list = mhdm->mh_list;
        for (size_t i = count; i; i--, mh_list_file++, mh_list++) {
            mh_list->type = mh_list_file->type;
            mh_list->data = (void*)(data + mh_list_file->data_offset);
        }
    }

    if (mhdf->pp_list_offset) {
        MhpListFile* pp_list_file = (MhpListFile*)(data + mhdf->pp_list_offset);
        MhpListFile* pp_list_file_end = pp_list_file;
        while ((pp_list_file_end++)->type >= 0);

        size_t count = pp_list_file_end - pp_list_file;
        mhdm->pp_list = alloc_handler->allocate<MhpList>(count);
        MhpList* pp_list = mhdm->pp_list;
        for (size_t i = count; i; i--, pp_list_file++, pp_list++) {
            pp_list->type = pp_list_file->type;
            pp_list->frame = pp_list_file->frame;
            pp_list->data = (void*)(data + pp_list_file->data_offset);
        }
    }

    if (mhdf->cm_list_offset) {
        MhcListFile* cm_list_file = (MhcListFile*)(data + mhdf->cm_list_offset);
        MhcListFile* cm_list_file_end = cm_list_file;
        while ((cm_list_file_end++)->data_offset);

        size_t count = cm_list_file_end - cm_list_file;
        mhdm->cm_list = alloc_handler->allocate<MhcList>(count);
        MhcList* cm_list = mhdm->cm_list;
        for (size_t i = count; i; i--, cm_list_file++, cm_list++)
            cm_list->data = (void*)(data + cm_list_file->data_offset);
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
        if (file_handler.read_file(aft_data, dir.c_str(),
            farc_file.c_str(), mot_file.c_str(), prj::MemCSystem, false))
            file_handler.set_callback_data(0, (PFNFILEHANDLERCALLBACK*)ParseFileParent, this);
    }
    else {
        if (file_handler.read_file(aft_data, rom_dir.c_str(), mot_file.c_str(), prj::MemCSystem))
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

const mot_data* get_motiondata_address(uint32_t motnum, const motion_database* mot_db) {
    if (motnum == -1) // It should be motnum < 0, but we're dealing with unsigned value, so there it is
        return 0;

    const motion_set_info* set_info = mot_db->get_motion_set_by_motion_id(motnum);
    if (!set_info) {
        printf_debug_info("%08x : illegal motfile Num.\n", motnum);
        return 0;
    }

    for (const motion_info& i : set_info->motion) {
        if (i.id != motnum)
            continue;

        auto elem = sp_mot_data.find(set_info->id);
        if (elem != sp_mot_data.end())
            return &elem->second.mot_set->mot_data[&i - set_info->motion.data()];
        return 0;
    }

    return 0;
}

float_t get_mot_frame_max(uint32_t mot_id, const motion_database* mot_db) {
    const mot_data* mot = get_motiondata_address(mot_id, mot_db);
    if (mot)
        return mot->frame_max;
    return 0.0f;
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

    auto elem = sp_mot_data.find(set);
    if (elem == sp_mot_data.end())
        elem = sp_mot_data.insert({ set, {} }).first;

    elem->second.mot_set_info = set_info;
    elem->second.LoadFile(std::string(mdata_dir), set);
}

void motion_set_unload_mothead(uint32_t set) {
    auto elem = mothead_storage_data.find(set);
    if (elem != mothead_storage_data.end() && elem->second.Unload())
        mothead_storage_data.erase(elem);
}

void motion_set_unload_motion(uint32_t set) {
    auto elem = sp_mot_data.find(set);
    if (elem != sp_mot_data.end() && elem->second.Unload())
        sp_mot_data.erase(elem);
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

const MhdData* mothead_storage_get_mot_by_motion_id(
    uint32_t motion_id, const motion_database* mot_db) {
    uint32_t set_id = mot_db->get_motion_set_id_by_motion_id(motion_id);
    if (set_id == -1)
        return &mothead_mot_null;

    auto elem = mothead_storage_data.find(set_id);
    if (elem == mothead_storage_data.end() || !elem->second.data)
        return &mothead_mot_null;

    MhdFileHeader* mhd = elem->second.data;
    if (motion_id < mhd->min_mot_id || motion_id > mhd->max_mot_id)
        return &mothead_mot_null;

    MhdData* mot = mhd->data_list[(ssize_t)motion_id - mhd->min_mot_id];
    if (!mot)
        return &mothead_mot_null;
    return mot;
}

bool motion_storage_check_mot_file_not_ready(uint32_t set_id) {
    auto elem = sp_mot_data.find(set_id);
    if (elem != sp_mot_data.end())
        return elem->second.CheckNotReady();
    return false;
}

MotFile* motion_storage_get_mot_file(uint32_t set_id) {
    auto elem = sp_mot_data.find(set_id);
    if (elem != sp_mot_data.end())
        return &elem->second;
    return 0;
}

const mot_set* motion_storage_get_motion_set(uint32_t set_id) {
    auto elem = sp_mot_data.find(set_id);
    if (elem != sp_mot_data.end())
        return elem->second.mot_set;
    return 0;
}

void motion_init() {
    mothead_storage_data = {};
    sp_mot_data = {};
}

void motion_free() {
    sp_mot_data.clear();
    mothead_storage_data.clear();
}
