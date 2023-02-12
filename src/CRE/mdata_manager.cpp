/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "mdata_manager.hpp"
#include "pv_db.hpp"

MdataMgr mdata_manager;

MdataMgr::MdataMgr() : state() {
    prefixes.push_back("");
    prefixes.push_back("mdata_");
}

MdataMgr::~MdataMgr() {

}

bool MdataMgr::Init() {
    state = 0;
    return true;
}

bool MdataMgr::Ctrl() {
    switch (state) {
    case 0:
        state = 1;
        break;
    case 1:
        if (!task_pv_db_get()->field_99)
            task_pv_db_add_paths();
        state = 2;
        break;
    case 2:
        if (task_pv_db_is_paths_empty())
            state = 3;
        break;
    case 3:
        task_pv_db_free_pv_data();
        state = 4;
        break;
    case 4:
        Log("--->LOAD ROM PVDB!\n");
        if (!task_pv_db_get()->field_99)
            task_pv_db_add_paths();
        state = 5;
        break;
    case 5:
        if (task_pv_db_is_paths_empty())
            return true;
        break;
    case 6:
        state = 15;
        break;
    case 15:
        return true;
    }
    return false;
}

bool MdataMgr::Dest() {
    return true;
}

void MdataMgr::Disp() {

}

void MdataMgr::Basic() {

}

void MdataMgr::Log(const char* fmt, ...) {
    char buf[0x400];
    va_list args;
    va_start(args, fmt);
    size_t len = vsprintf_s(buf, sizeof(buf), fmt, args);
    va_end(args);
    log.append(fmt, len);
    log.append(1, '\n');
}

const std::list<std::string>& MdataMgr::GetPrefixes() {
    return prefixes;
}

bool mdata_manager_add_task() {
    return app::TaskWork::AddTask(mdata_manager_get(), 0, "M_DATA_MANAGER");
}

bool mdata_manager_del_task() {
    return mdata_manager_get()->DelTask();
}

MdataMgr* mdata_manager_get() {
    return &mdata_manager;
}
