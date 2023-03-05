/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <list>
#include "../KKdLib/default.hpp"

namespace app {
    class TaskInterface {
    public:
        TaskInterface();
        virtual ~TaskInterface();

        virtual bool Init();
        virtual bool Ctrl();
        virtual bool Dest();
        virtual void Disp();
        virtual void Basic();
    };

    class Task : public TaskInterface {
    public:
        enum class Op {
            None = 0,
            Init,
            Ctrl,
            Dest,
            Max,
        };

        enum class Request {
            None = 0,
            Init,
            Dest,
            Suspend,
            Hide,
            Run,
        };

        enum class State {
            None = 0,
            Running,
            Suspended,
            Hidden,
        };

        int32_t priority;
        Task* parent_task;
        Op op;
        State state;
        Request request;
        Op next_op;
        State next_state;
        bool field_2C;
        bool is_frame_dependent;
        char name[32];
        bool field_4E;
        bool field_4F;
        uint32_t base_calc_time;
        uint32_t calc_time;
        uint32_t calc_time_max;
        uint32_t disp_time;
        uint32_t disp_time_max;

        Task();
        virtual ~Task() override;

        uint32_t GetCalcTime();
        uint32_t GetCalcTimeMax();
        uint32_t GetDispTime();
        uint32_t GetDispTimeMax();
        char* GetName();

        bool DelTask();
        bool HideTask();
        bool RunTask();
        bool SuspendTask();

        void SetName(const char* name);
        void SetPriority(int32_t priority);
    };

    struct TaskWork;

    extern TaskWork* task_work;

    struct TaskWork {
        std::list<Task*> tasks;
        Task* current;
        bool disp;

        TaskWork();
        ~TaskWork();

        static bool AddTask(Task* t,
            const char* name = "(unknown)", int32_t priority = 1);
        static bool AddTask(Task* t, Task* parent_task,
            const char* name = "(unknown)", int32_t priority = 1);
        static void Basic();
        static bool CheckTaskCtrl(Task* t);
        static bool CheckTaskReady(Task* t);
        static void Ctrl();
        static void Dest();
        static void Disp();
        static Task* GetTaskByIndex(int32_t index);
        static bool HasTask(Task* t);
        static bool HasTaskInit(Task* t);
        static bool HasTaskCtrl(Task* t);
        static bool HasTaskDest(Task* t);
        static bool HasTasksDest();
    };

    extern void task_work_init();
    extern void task_work_free();
}
