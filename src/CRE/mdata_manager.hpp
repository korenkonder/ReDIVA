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
    std::string log;
    std::list<std::string> prefixes;

    MdataMgr();
    virtual ~MdataMgr() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;
    virtual void Basic() override;

    void Log(const char* fmt, ...);
    const std::list<std::string>& GetPrefixes();
};

extern bool mdata_manager_add_task();
extern bool mdata_manager_del_task();
extern MdataMgr* mdata_manager_get();
