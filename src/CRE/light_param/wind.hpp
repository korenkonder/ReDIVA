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

struct WindSpectrum {
    float_t cos;
    float_t sin;
};

struct Wind {
    float_t scale;
    float_t cycle;
    float_t yrot;
    float_t zrot;
    float_t bias;
    size_t count;
    WindSpectrum spectrum[16];
    vec3 val;
    float_t frame;
    float_t strength;

    Wind();
    ~Wind();

    float_t calc_spectrum(float_t time) const;
    void ctrl();
    float_t get_scale() const;
    void set_scale(float_t value);
    float_t get_cycle() const;
    void set_cycle(float_t value);
    void get_rot(vec2& value) const;
    void set_rot(const vec2& value);
    float_t get_yrot() const;
    void set_yrot(float_t value);
    float_t get_zrot() const;
    void set_zrot(float_t value);
    float_t get_bias() const;
    void set_bias(float_t value);
    void reset();
};

struct StageWind {
    Wind* wind;

    StageWind();
    ~StageWind();
};

class TaskWind : public app::Task {
public:
    StageWind stage_wind;

    TaskWind();
    virtual ~TaskWind() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void disp() override;
};

extern TaskWind* task_wind;

extern void task_wind_init();
extern bool task_wind_hide_task();
extern bool task_wind_run_task();
extern void task_wind_free();
