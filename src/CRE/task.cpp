/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "task.hpp"
#include "../CRE/app_system_detail.hpp"
#include "../KKdLib/time.hpp"

namespace app {
    static void ctrl_task_sub(int32_t frames, bool frame_skip);

    TaskInfo* task_info;

    TaskInterface::TaskInterface() {

    }

    TaskInterface::~TaskInterface() {

    }

    bool TaskInterface::init() {
        return true;
    }

    bool TaskInterface::ctrl() {
        return false;
    }

    bool TaskInterface::dest() {
        return true;
    }

    void TaskInterface::disp() {

    }

    void TaskInterface::post() {

    }

    bool Task::check_req(TASK_REQ req) {
        if (task_info->now_exec_disp)
            return false;

        switch (req) {
        case TASK_REQ_OPEN:
            return true;
        case TASK_REQ_CLOSE:
            return M_stat != TASK_STAT_DEAD;
        case TASK_REQ_PAUSE:
            return M_stat == TASK_STAT_ACTIVE || M_stat == TASK_STAT_SUSPEND;
        case TASK_REQ_SUSPEND:
            return M_stat == TASK_STAT_ACTIVE || M_stat == TASK_STAT_PAUSE;
        case TASK_REQ_RESTART:
            return M_stat == TASK_STAT_PAUSE || M_stat == TASK_STAT_SUSPEND;
        }
        return false;
    }

    void Task::set_req(TASK_REQ req) {
        if (req > TASK_REQ_OPEN)
            for (Task* i : task_info->list)
                if (i->get_parent() == this)
                    i->set_req(req);
        M_req = req;
    }

    void Task::set_name(const char* name) {
        M_name.assign(name);
    }

    Task::Task() {
        priority = TASK_PRIO_NORMAL;
        M_parent = 0;
        M_proc = TASK_PROC_UNKNOWN;
        M_stat = TASK_STAT_DEAD;
        M_req = TASK_REQ_NONE;
        M_next_proc = TASK_PROC_UNKNOWN;
        M_next_stat = TASK_STAT_DEAD;
        reopen_flag = false;
        sync_pulse_mode = false;
        set_name("(unknown)");
        m_calc_time = 0;
        m_total_calc_time = 0;
        m_calc_time_max = 0;
        m_disp_time = 0;
        m_disp_time_max = 0;
    }

    Task::~Task() {

    }

    bool Task::open(Task* parent, const char* name, TASK_PRIO prio) {
        set_req(TASK_REQ_NONE);
        if (check_entry() || !check_req(TASK_REQ_OPEN))
            return false;

        set_priority(prio);
        M_parent = parent;
        M_proc = TASK_PROC_UNKNOWN;
        M_stat = TASK_STAT_DEAD;
        M_next_proc = TASK_PROC_UNKNOWN;
        M_next_stat = TASK_STAT_DEAD;
        set_name(name);
        set_req(TASK_REQ_OPEN);
        transition();
        task_info->list.push_back(this);
        return true;
    }

    bool Task::open(Task* parent, const char* name) {
        return open(parent, name, TASK_PRIO_NORMAL);
    }

    bool Task::open(const char* name, TASK_PRIO prio) {
        return open(task_info->current, name, prio);
    }

    bool Task::open(const char* name) {
        return open(task_info->current, name, TASK_PRIO_NORMAL);
    }

    bool Task::close() {
        if (!check_entry() || !check_req(TASK_REQ_CLOSE))
            return false;

        set_req(TASK_REQ_CLOSE);
        return true;
    }

    bool Task::reopen() {
        bool ret = close();
        if (ret)
            reopen_flag = true;
        return ret;
    }

    bool Task::pause() {
        if (!check_entry() || !check_req(TASK_REQ_PAUSE))
            return false;

        set_req(TASK_REQ_PAUSE);
        return true;
    }

    bool Task::suspend() {
        if (!check_entry() || !check_req(TASK_REQ_SUSPEND))
            return false;

        set_req(TASK_REQ_SUSPEND);
        return true;
    }

    bool Task::restart() {
        if (!check_entry() || !check_req(TASK_REQ_RESTART))
            return false;

        set_req(TASK_REQ_RESTART);
        return true;
    }

    bool Task::check_alive() {
        return check_entry() && (M_proc == TASK_PROC_UNKNOWN || M_stat != TASK_STAT_DEAD);
    }

    bool Task::check_pause() {
        return check_entry() && (M_proc == TASK_PROC_UNKNOWN || M_stat == TASK_STAT_PAUSE);
    }

    bool Task::check_suspend() {
        return check_entry() && (M_proc == TASK_PROC_UNKNOWN || M_stat == TASK_STAT_SUSPEND);
    }

    bool Task::check_proc_ctrl() {
        if (check_entry() && M_stat == TASK_STAT_ACTIVE)
            return M_proc == TASK_PROC_CTRL;
        return false;
    }

    bool Task::check_closing() {
        if (check_entry())
            return M_proc == TASK_PROC_DEST;
        return false;
    }

    bool Task::check_entry() {
        for (Task* i : task_info->list)
            if (i == this)
                return true;
        return false;
    }

    void Task::transition() {
        if (M_proc != TASK_PROC_INIT && M_proc != TASK_PROC_DEST && check_req(M_req)) {
            switch (M_req) {
            case TASK_REQ_OPEN:
                M_next_proc = TASK_PROC_INIT;
                M_next_stat = TASK_STAT_ACTIVE;
                break;
            case TASK_REQ_CLOSE:
                M_next_proc = TASK_PROC_DEST;
                M_next_stat = TASK_STAT_ACTIVE;
                break;
            case TASK_REQ_PAUSE:
                M_next_stat = TASK_STAT_PAUSE;
                break;
            case TASK_REQ_SUSPEND:
                M_next_stat = TASK_STAT_SUSPEND;
                break;
            case TASK_REQ_RESTART:
                M_next_stat = TASK_STAT_ACTIVE;
                break;
            }
            M_req = TASK_REQ_NONE;
        }

        M_proc = M_next_proc;
        M_stat = M_next_stat;
    }

    void Task::exec_calc() {
        if (M_stat != TASK_STAT_ACTIVE)
            return;

        if (M_proc == TASK_PROC_INIT && init()) {
            M_next_proc = TASK_PROC_CTRL;
            M_proc = TASK_PROC_CTRL;
        }

        if (M_proc == TASK_PROC_CTRL && ctrl()) {
            M_next_proc = TASK_PROC_DEST;
            M_proc = TASK_PROC_DEST;
        }

        if (M_proc == TASK_PROC_DEST && dest()) {
            if (reopen_flag) {
                M_proc = TASK_PROC_UNKNOWN;
                M_stat = TASK_STAT_DEAD;
                M_next_proc = TASK_PROC_UNKNOWN;
                M_next_stat = TASK_STAT_DEAD;

                set_req(TASK_REQ_OPEN);
                transition();
                reopen_flag = false;
            }
            else {
                M_next_proc = TASK_PROC_MAX;
                M_next_stat = TASK_STAT_DEAD;
                M_req = TASK_REQ_NONE;
            }

        }
    }

    void Task::exec_disp() {
        if ((M_stat == TASK_STAT_ACTIVE || M_stat == TASK_STAT_PAUSE)
            && M_proc != TASK_PROC_INIT && M_proc != TASK_PROC_DEST)
            disp();
    }

    void Task::exec_pre() {

    }

    void Task::exec_post() {
        if ((M_stat == TASK_STAT_ACTIVE || M_stat == TASK_STAT_PAUSE)
            && M_proc != TASK_PROC_INIT && M_proc != TASK_PROC_DEST)
            post();
    }

    void Task::set_priority(TASK_PRIO prio) {
        priority = prio;
    }

    TASK_PRIO Task::get_priority() const {
        return priority;
    }

    bool Task::check_priority(TASK_PRIO prio) const {
        return priority == prio;
    }

    void Task::set_sync_pulse_mode(bool mode) {
        sync_pulse_mode = mode;
    }

    bool Task::check_sync_pulse_mode() {
        return sync_pulse_mode;
    }

    Task* Task::get_parent() const {
        return M_parent;
    }

    const char* Task::get_name() const {
        return M_name.c_str();
    }

    void Task::set_calc_time(uint32_t time) {
        m_calc_time = time;
    }

    void Task::add_calc_time(uint32_t time) {
        m_calc_time += time;
    }

    uint32_t Task::get_calc_time() const {
        return m_total_calc_time;
    }

    uint32_t Task::get_calc_time_max() const {
        return m_calc_time_max;
    }

    void Task::set_total_calc_time() {
        m_total_calc_time = m_calc_time;
        uint32_t main_timer = get_main_timer();
        if (main_timer == (main_timer / 300) * 300)
            m_calc_time_max = 0;
        m_calc_time_max = max_def(m_total_calc_time, m_calc_time_max);
    }

    void Task::set_disp_time(uint32_t time) {
        m_disp_time = time;
        uint32_t main_timer = get_main_timer();
        if (main_timer == (main_timer / 300) * 300)
            m_disp_time_max = 0;
        m_disp_time_max = max_def(m_disp_time, m_disp_time_max);
    }

    uint32_t Task::get_disp_time() const {
        return m_disp_time;
    }

    uint32_t Task::get_disp_time_max() const {
        return m_disp_time_max;
    }

    TaskInfo::TaskInfo() : current(), now_exec_disp() {

    }

    TaskInfo::~TaskInfo() {

    }

    extern void task_info_init() {
        task_info = new TaskInfo;
    }

    extern void task_info_free() {
        do {
            close_all_task();
            ctrl_task();
        } while (check_closing_task());

        delete task_info;
    }

    bool check_closing_task() {
        for (Task* i : task_info->list)
            if (i->check_closing())
                return true;
        return false;
    }

    void close_all_task() {
        for (Task* i : task_info->list)
            i->close();
    }

    void ctrl_task() {
        for (Task* i : task_info->list) {
            i->transition();
            i->set_calc_time(0);
        }

        for (int32_t i = TASK_PRIO_MAX - 1; i >= 0; i--)
            for (auto j = task_info->list.end(); j != task_info->list.begin(); ) {
                --j;
                Task* tsk = *j;
                if (!tsk->check_priority((TASK_PRIO)i) || !tsk->check_closing())
                    continue;

                time_struct t;
                task_info->current = tsk;
                tsk->exec_calc();
                task_info->current = 0;
                tsk->add_calc_time((uint32_t)(t.calc_time() * 1000.0));
            }

        for (auto i = task_info->list.begin(); i != task_info->list.end(); ) {
            if ((*i)->check_alive()) {
                i++;
                continue;
            }

            i = task_info->list.erase(i);
        }

        int32_t frames = get_delta_frame_history_int();
        if (frames > 1)
            for (int32_t i = frames - 1; i; i--)
                ctrl_task_sub(frames, true);
        ctrl_task_sub(frames, false);

        for (Task* i : task_info->list)
            i->set_total_calc_time();
    }

    void dest_task() {
        task_info->list.clear();
    }

    void disp_task() {
        task_info->now_exec_disp = true;
        for (int32_t i = 0; i < 3; i++)
            for (Task* j : task_info->list) {
                Task* tsk = j;
                if (!tsk->check_priority((TASK_PRIO)i))
                    continue;

                time_struct t;
                tsk->exec_disp();
                tsk->set_disp_time((uint32_t)(t.calc_time() * 1000.0));
            }
        task_info->now_exec_disp = false;
    }

    Task* get_task_info(int32_t index) {
        int32_t k = 0;
        for (int32_t i = 0; i < TASK_PRIO_MAX; i++)
            for (Task* j : task_info->list)
                if (j->check_priority((TASK_PRIO)i))
                    if (k++ == index)
                        return j;
        return 0;
    }

    void post_task() {
        for (int32_t i = 0; i < TASK_PRIO_MAX; i++)
            for (Task* j : task_info->list)
                if (j->check_priority((TASK_PRIO)i))
                    j->exec_post();
    }

    static void ctrl_task_sub(int32_t frames, bool sync) {
        for (int32_t i = 0; i < TASK_PRIO_MAX; i++)
            for (Task* j : task_info->list) {
                Task* tsk = j;
                if (!tsk->check_priority((TASK_PRIO)i) || tsk->check_closing())
                    continue;
                else if (tsk->check_sync_pulse_mode()) {
                    if (frames <= 0)
                        continue;
                }
                else if (sync)
                    continue;

                time_struct t;
                task_info->current = tsk;
                tsk->exec_calc();
                task_info->current = 0;
                tsk->add_calc_time((uint32_t)(t.calc_time() * 1000.0));
            }
    }
}
