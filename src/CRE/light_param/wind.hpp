/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../../KKdLib/light_param/wind.hpp"
#include "../../KKdLib/vec.hpp"
#include "../static_var.hpp"
#include "../task.hpp"

struct wind_spc {
    float_t cos;
    float_t sin;
};

struct wind {
    float_t scale;
    float_t cycle;
    float_t rot_y;
    float_t rot_z;
    float_t bias;
    size_t count;
    wind_spc spc[16];
    vec3 wind_direction;
    float_t frame;
    float_t strength;

    wind();
    ~wind();

    float_t get_scale() const;
    void set_scale(float_t value);
    float_t get_cycle() const;
    void set_cycle(float_t value);
    void get_rot(vec2* value) const;
    void set_rot(vec2* value);
    float_t get_rot_y() const;
    void set_rot_y(float_t value);
    float_t get_rot_z() const;
    void set_rot_z(float_t value);
    float_t get_bias() const;
    void set_bias(float_t value);
    void reset();
};

class TaskWind : public app::Task {
public:
    wind* ptr;

    TaskWind();
    virtual ~TaskWind() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;
};

extern TaskWind* task_wind;

extern void task_wind_init();
extern bool task_wind_hide_task();
extern bool task_wind_run_task();
extern void task_wind_free();
