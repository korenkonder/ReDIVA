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

        virtual bool init();
        virtual bool ctrl();
        virtual bool dest();
        virtual void disp();
        virtual void basic();
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
        bool frame_dependent;
        char name[32];
        uint32_t base_calc_time;
        uint32_t calc_time;
        uint32_t calc_time_max;
        uint32_t disp_time;
        uint32_t disp_time_max;

        Task();
        virtual ~Task() override;

        uint32_t get_calc_time() const;
        uint32_t get_calc_time_max() const;
        uint32_t get_disp_time() const;
        uint32_t get_disp_time_max() const;
        const char* get_name() const;
        Task* get_parent_task() const;

        bool del();
        bool hide();
        bool run();
        bool suspend();

        void set_name(const char* name);
        void set_priority(int32_t priority);
    };

    struct TaskWork {
        std::list<Task*> tasks;
        Task* current_task;
        bool disp_task;

        TaskWork();
        ~TaskWork();

        static bool add_task(Task* t,
            const char* name = "(unknown)", int32_t priority = 1);
        static bool add_task(Task* t, Task* parent_task,
            const char* name = "(unknown)", int32_t priority = 1);
        static void basic();
        static bool check_task_ctrl(Task* t);
        static bool check_task_ready(Task* t);
        static void ctrl();
        static void dest();
        static void disp();
        static Task* get_task_by_index(int32_t index);
        static bool has_task(Task* t);
        static bool has_task_init(Task* t);
        static bool has_task_ctrl(Task* t);
        static bool has_task_dest(Task* t);
        static bool has_tasks_dest();
    };

    extern TaskWork* task_work;

    extern void task_work_init();
    extern void task_work_free();
}
