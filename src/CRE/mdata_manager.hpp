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
};

extern bool mdata_manager_append_task();
extern bool mdata_manager_free_task();
extern MdataMgr* mdata_manager_get();
