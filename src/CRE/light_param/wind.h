/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.h"
#include "../../KKdLib/light_param/wind.h"
#include "../../KKdLib/vec.h"
#include "../static_var.h"
#include "../task.h"

typedef struct wind_spc {
    float_t cos;
    float_t sin;
} wind_spc;

class wind {
public:
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

    float_t get_scale();
    void set_scale(float_t value);
    float_t get_cycle();
    void set_cycle(float_t value);
    void get_rot(vec2* value);
    void set_rot(vec2* value);
    float_t get_rot_y();
    void set_rot_y(float_t value);
    float_t get_rot_z();
    void set_rot_z(float_t value);
    float_t get_bias();
    void set_bias(float_t value);
    void reset();
};

class TaskWind : public Task {
public:
    wind* ptr;

    TaskWind();
    virtual ~TaskWind() override;
    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;
};

extern TaskWind task_wind;
