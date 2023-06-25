/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../KKdLib/vec.hpp"
#include "../../CRE/auth_2d.hpp"
#include "../../CRE/task.hpp"

class Game2dEnergyUnit {
public:
    aet_layout_data field_8[4];
    uint32_t field_1B8[5];
    bool field_1CC[5];
    bool field_1D1;
    bool no_fail;
    int32_t field_1D4;
    vec2 field_1D8;
    vec2 field_1E0;
    vec2 field_1E8;
    vec2 field_1F0;
    float_t max_value;
    float_t life_gauge_prev;
    float_t life_gauge_stable;
    float_t life_gauge_decrease;
    float_t life_gauge_increase;
    float_t life_gauge_border;
    float_t life_gauge;
    int32_t life_gauge_delay;
    bool field_218;
    bool life_gauge_safety;
    float_t life_gauge_safety_time;
    int32_t life_gauge_bonus;
    int32_t field_224;
    int32_t field_228;
    int32_t field_22C;

    Game2dEnergyUnit();
    virtual ~Game2dEnergyUnit();

    virtual void Init();
    virtual void Ctrl();
    virtual void Dest();
    virtual void Disp();
    virtual void DispInner();
};

class TaskGame2d : public app::Task {
public:
    bool energy_unit_init;
    Game2dEnergyUnit energy_unit;

    TaskGame2d();
    virtual ~TaskGame2d() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;
};

extern bool task_game_2d_add_task();
extern void task_game_2d_set_energy_unit_no_fail(bool value);
extern void task_game_2d_sub_140372670(bool value);
extern void task_game_2d_sub_1403726D0(int32_t value);
extern bool task_game_2d_del_task();
