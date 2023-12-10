/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "../../KKdLib/default.hpp"
#include "../../CRE/Glitter/glitter.hpp"
#include "../../CRE/task.hpp"

class TaskDataTestGlitterParticle : public app::Task {
public:
    uint64_t hash;
    Glitter::SceneCounter scene_counter;
    class DataTestGlitterParticleDw* dw;
    float_t frame;
    bool auto_and_repeat;
    bool reload;
    bool pv_mode;
    bool show_grid;
    bool rebuild_geff;

    TaskDataTestGlitterParticle();
    virtual ~TaskDataTestGlitterParticle() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void disp() override;

    void LoadFile(const char* file);
    void SceneFree();
    void SetPause(bool value);
    void SetReload();
};

extern TaskDataTestGlitterParticle* task_data_test_glitter_particle;

extern void task_data_test_glitter_particle_init();
extern void task_data_test_glitter_particle_free();
