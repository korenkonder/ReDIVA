/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "mdata_manager.hpp"
#include "pv_db.hpp"

MdataMgr mdata_manager;

MdataMgr::MdataMgr() : state(), inner_state(), pv_db_paths_count(), pv_db_total_paths_count() {
    prefixes.push_back("");
    prefixes.push_back("mdata_");
}

MdataMgr::~MdataMgr() {

}

bool MdataMgr::init() {
    state = 0;
    return true;
}

bool MdataMgr::ctrl() {
    switch (state) {
    case 0:
        state = 1;
        break;
    case 1:
        if (!task_pv_db_get()->field_99)
            task_pv_db_add_paths();

        inner_state = 3;
        pv_db_paths_count = 0;
        pv_db_total_paths_count = task_pv_db_get_paths_count();
        state = 2;
        break;
    case 2:
        pv_db_paths_count = pv_db_total_paths_count - task_pv_db_get_paths_count();
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

        inner_state = 5;
        pv_db_paths_count = 0;
        pv_db_total_paths_count = task_pv_db_get_paths_count();
        state = 5;
        break;
    case 5:
        if (task_pv_db_is_paths_empty())
            return true;
        break;
    case 6:
        inner_state = 7;
        pv_db_paths_count = 0;
        pv_db_total_paths_count = 0;
        state = 15;
        break;
    case 15:
        inner_state = 9;
        return true;
    }
    return false;
}

bool MdataMgr::dest() {
    return true;
}

void MdataMgr::disp() {

}

void MdataMgr::basic() {

}

void MdataMgr::Log(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    std::string buf = vsprintf_s_string(fmt, args);
    va_end(args);
    log.append(buf);
    log.append(1, '\n');
}

const std::list<std::string>& MdataMgr::GetPrefixes() {
    return prefixes;
}

std::string MdataMgr::GetStateString() {
    char buf[0x40];
    std::string str;
    switch (inner_state) {
    case 1:
        str.assign("WAIT 1");
        if (pv_db_total_paths_count <= 0)
            break;

        sprintf_s(buf, sizeof(buf), " (%d/%d)", pv_db_paths_count, pv_db_total_paths_count);
        str.append(buf);
        break;
    case 2:
        str.assign("WAIT 2");
        break;
    case 3:
        str.assign("WAIT 7");
        if (pv_db_total_paths_count <= 0)
            break;

        sprintf_s(buf, sizeof(buf), " (%d/%d)", pv_db_paths_count, pv_db_total_paths_count);
        str.append(buf);
        break;
    case 4:
        str.assign("WAIT 8");
        if (pv_db_total_paths_count <= 0)
            break;

        sprintf_s(buf, sizeof(buf), " (%d/%d)", pv_db_paths_count, pv_db_total_paths_count);
        str.append(buf);
        break;
    case 5:
        str.assign("WAIT 9");
        if (pv_db_total_paths_count <= 0)
            break;

        sprintf_s(buf, sizeof(buf), " (%d/%d)", pv_db_paths_count, pv_db_total_paths_count);
        str.append(buf);
        break;
    case 6:
        str.assign("WAIT 10", 7ui64);
        if (pv_db_total_paths_count <= 0)
            break;

        sprintf_s(buf, sizeof(buf), " (%d/%d)", pv_db_paths_count, pv_db_total_paths_count);
        str.append(buf);
        break;
    case 7:
        str.assign("WAIT 15");
        break;
    case 8:
        str.assign("WAIT 16");
        if (pv_db_total_paths_count <= 0)
            break;

        sprintf_s(buf, sizeof(buf), " (%d/%d)", pv_db_paths_count, pv_db_total_paths_count);
        str.append(buf);
        break;
    case 9:
        str.assign("OK");
        break;
    }
    return str;
}

bool mdata_manager_add_task() {
    return app::TaskWork::add_task(mdata_manager_get(), 0, "M_DATA_MANAGER");
}

bool mdata_manager_del_task() {
    return mdata_manager_get()->del();
}

MdataMgr* mdata_manager_get() {
    return &mdata_manager;
}
