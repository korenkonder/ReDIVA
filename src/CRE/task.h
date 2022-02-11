/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.h"

typedef enum class task_enum {
    NONE = 0x00,
    INIT = 0x01,
    CTRL = 0x02,
    DEST = 0x03,
    MAX  = 0x04,
} task_enum;

class Task {
public:
    int32_t priority;
    Task* parent;
    task_enum field_18;
    uint32_t field_1C;
    uint32_t field_20;
    task_enum field_24;
    uint32_t field_28;
    bool field_2C;
    bool field_2D;
    char name[32];
    bool field_4E;
    bool field_4F;
    uint32_t base_calc_time;
    uint32_t calc_time;
    uint32_t calc_time_max;
    uint32_t disp_time;
    uint32_t disp_time_max;

    Task();
    virtual void Basic();
    virtual bool Ctrl();
    virtual bool Dest();
    virtual void Disp();
    virtual void Dispose(bool free_data);
    virtual bool Init();

    void Free();
    void SetName(char* name);
    void SetName(const char* name);
    void SetPriority(int32_t priority);
    ~Task();
};

extern void TaskWork_init();
extern void TaskWork_basic();
extern void TaskWork_ctrl();
extern void TaskWork_disp();
extern bool TaskWork_has_task(Task* t);
extern void TaskWork_free();
