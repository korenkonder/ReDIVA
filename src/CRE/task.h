/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <vector>
#include "../KKdLib/default.h"

typedef enum task_enum {
    TASK_NONE = 0x00,
    TASK_INIT = 0x01,
    TASK_CTRL = 0x02,
    TASK_DEST = 0x03,
    TASK_MAX  = 0x04,
} task_enum;

class Task {
public:
    int32_t priority;
    Task* parent_task;
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
    virtual ~Task();
    virtual bool Init();
    virtual bool Ctrl();
    virtual bool Dest();
    virtual void Disp();
    virtual void Basic();

    void SetName(char* name);
    void SetName(const char* name);
    void SetPriority(int32_t priority);
};

class TaskWork;

extern TaskWork task_work;

class TaskWork {
public:
    std::vector<Task*> tasks;
    Task* current;
    bool disp;

    TaskWork();
    ~TaskWork();

    static bool AppendTask(Task* t,
        const char* name = "(unknown)", int32_t priority = 1);
    static bool AppendTask(Task* t, Task* parent_task = task_work.current,
        const char* name = "(unknown)", int32_t priority = 1);
    static void Basic();
    static void Ctrl();
    static void Disp();
    static bool HasTask(Task* t);
};
