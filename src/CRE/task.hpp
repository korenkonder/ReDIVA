/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <list>
#include "../KKdLib/default.hpp"
#include "../KKdLib/prj/string_on_array.hpp"

namespace app {
    enum TASK_PRIO {
        TASK_PRIO_00 = 0,
        TASK_PRIO_01,
        TASK_PRIO_02,
        TASK_PRIO_MAX,

        TASK_PRIO_HIGH   = TASK_PRIO_00,
        TASK_PRIO_NORMAL = TASK_PRIO_01,
        TASK_PRIO_LOW    = TASK_PRIO_02,
    };

    class TaskInterface {
    public:
        TaskInterface();
        virtual ~TaskInterface();

        virtual bool init();
        virtual bool ctrl();
        virtual bool dest();
        virtual void disp();
        virtual void post();
    };

    class Task : public TaskInterface {
        friend class TaskWindow;
    public:
        enum TASK_PROC {
            TASK_PROC_UNKNOWN = 0,
            TASK_PROC_INIT,
            TASK_PROC_CTRL,
            TASK_PROC_DEST,
            TASK_PROC_MAX,
        };

        enum TASK_STAT {
            TASK_STAT_DEAD = 0,
            TASK_STAT_ACTIVE,
            TASK_STAT_PAUSE,
            TASK_STAT_SUSPEND,
            TASK_STAT_MAX,
        };

        enum TASK_REQ {
            TASK_REQ_NONE = 0,
            TASK_REQ_OPEN,
            TASK_REQ_CLOSE,
            TASK_REQ_PAUSE,
            TASK_REQ_SUSPEND,
            TASK_REQ_RESTART,
            TASK_REQ_MAX,
        };

    private:
        bool check_req(TASK_REQ req);
        void set_req(TASK_REQ req);
        void set_name(const char* name);

        TASK_PRIO priority;
        Task* M_parent;
        TASK_PROC M_proc;
        TASK_STAT M_stat;
        TASK_REQ M_req;
        TASK_PROC M_next_proc;
        TASK_STAT M_next_stat;
        bool reopen_flag;
        bool sync_pulse_mode;
        prj::string_on_array<32> M_name;
        uint32_t m_calc_time;
        uint32_t m_total_calc_time;
        uint32_t m_calc_time_max;
        uint32_t m_disp_time;
        uint32_t m_disp_time_max;

    public:
        Task();
        virtual ~Task() override;

        bool open(Task* parent, const char* name, TASK_PRIO prio);
        bool open(Task* parent, const char* name);
        bool open(const char* name, TASK_PRIO prio);
        bool open(const char* name);
        bool close();
        bool reopen();
        bool pause();
        bool suspend();
        bool restart();
        bool check_alive();
        bool check_pause();
        bool check_suspend();
        bool check_proc_ctrl();
        bool check_closing();
        bool check_entry();
        void transition();
        void exec_calc();
        void exec_disp();
        void exec_pre();
        void exec_post();
        void set_priority(TASK_PRIO prio);
        TASK_PRIO get_priority() const;
        bool check_priority(TASK_PRIO prio) const;
        void set_sync_pulse_mode(bool mode);
        bool check_sync_pulse_mode();
        Task* get_parent() const;
        const char* get_name() const;
        void set_calc_time(uint32_t time);
        void add_calc_time(uint32_t time);
        uint32_t get_calc_time() const;
        uint32_t get_calc_time_max() const;
        void set_total_calc_time();
        void set_disp_time(uint32_t time);
        uint32_t get_disp_time() const;
        uint32_t get_disp_time_max() const;
    };

    struct TaskInfo {
        std::list<Task*> list;
        Task* current;
        bool now_exec_disp;

        TaskInfo();
        ~TaskInfo();
    };

    extern TaskInfo* task_info;

    extern void task_info_init();
    extern void task_info_free();

    extern bool check_closing_task();
    extern void close_all_task();
    extern void ctrl_task();
    extern void dest_task();
    extern void disp_task();
    extern Task* get_task_info(int32_t index);
    extern void post_task();
}
