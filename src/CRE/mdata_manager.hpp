/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "task.hpp"
#include <list>
#include <string>

class MdataMgr : public app::Task {
public:
    int32_t state;
    int32_t inner_state;
    int32_t pv_db_paths_count;
    int32_t pv_db_total_paths_count;
    std::string log;
    std::list<std::string> prefixes;

    MdataMgr();
    virtual ~MdataMgr() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void disp() override;
    virtual void basic() override;

    void Log(const char* fmt, ...);
    const std::list<std::string>& GetPrefixes();
    std::string GetStateString();
};

extern bool mdata_manager_add_task();
extern bool mdata_manager_del_task();
extern MdataMgr* mdata_manager_get();
